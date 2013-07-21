/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*
 * File:         spc-sm.c $XConsortium: spc-sm.c /main/4 1996/04/21 19:10:39 drk $
 * Language:     C
 *
 * (c) Copyright 1989, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#include <bms/sbport.h> /* NOTE: sbport.h must be the first include. */
#include <signal.h>
#include <SPC/spcP.h>

/*
 **
 ** Definitions for blocking signals
 **
*/


static int (* spc_state_table[16]) (SPC_Channel_Ptr channel, int connector)= {
  
                            /* old_state new_state */
  
  NULL,                     /*    00        00     */
  NULL,                     /*    00        01     */
  error_fun,                /*    00        10     */
  NULL,                     /*    00        11     */
  sigcld_with_reset,        /*    01        00     */
  NULL,                     /*    01        01     */
  error_fun,                /*    01        10     */
  error_fun,                /*    01        11     */
  connector_eof_with_reset, /*    10        00     */
  error_fun,                /*    10        01     */
  NULL,                     /*    10        10     */
  error_fun,                /*    10        11     */
  NULL,                     /*    11        00     */
  connector_eof,            /*    11        01     */
  NULL,                     /*    11        10     */
  NULL                      /*    11        11     */
  };

/*----------------------------------------------------------------------+*/
int
SPC_Change_State(SPC_Channel_Ptr channel,
		 int connector, 
		 int data_line, 
		 int process_line)
/*----------------------------------------------------------------------+*/
{

  int iomode=channel->IOMode;
  int old_state=CHANNEL_STATE(iomode);
  int new_state, state_index;
  int (*fun)(SPC_Channel_Ptr, int);
  int funretval;

  sigset_t newsigmask, oldsigmask;

  sigemptyset(&newsigmask);
  sigemptyset(&oldsigmask);

  /* Process don't cares */
  sigaddset(&newsigmask, SIGCHLD);
  sigprocmask(SIG_BLOCK, &newsigmask, &oldsigmask);

  if(data_line == -1)
    data_line=DATA_LINE(old_state);
  if(process_line == -1)
    process_line = PROC_LINE(old_state);

  /* create new state */
  
  new_state=MAKE_STATE(data_line, process_line);

  /* If no state change, return */
  
  if(new_state == old_state) {
    sigprocmask(SIG_SETMASK, &oldsigmask, (sigset_t *)NULL);
    return TRUE;
  }

  /* Lookup & process transition function */

  state_index=MAKE_STATE_INDEX(old_state, new_state);
  
  fun=spc_state_table[state_index];

  if(fun == error_fun) {
    sigprocmask(SIG_SETMASK, &oldsigmask, (sigset_t *)NULL);
    return TRUE;
   }
  
  channel->IOMode=MAKE_CHANNEL_STATE(iomode, new_state);
  if(!fun) {
    sigprocmask(SIG_SETMASK, &oldsigmask, (sigset_t *)NULL);
    return TRUE;
  }

  funretval=((*fun)(channel, connector));

  sigprocmask(SIG_SETMASK, &oldsigmask, (sigset_t *)NULL);
  return funretval;

}

/* error_fun is not ever called.  It is just a placeholder for an
   error condition in the state table */

/*----------------------------------------------------------------------+*/
int
error_fun(SPC_Channel_Ptr UNUSED_PARM(channel),
	  int 		  UNUSED_PARM(connector))
/*----------------------------------------------------------------------+*/
{
  return(FALSE);
}


/*
 ** 
 ** This routine is called when an EOF is detected on a specific
 ** connector within a channel, but the channel still has a subprocess
 ** associated with it. It will clear the data ready flag on the
 ** indicated wire.  After that, it will look at all the wires
 ** associated with the channel and set the channel's data flag to the
 ** inclusive OR of the individual wire's data flags.
 **
*/

/*----------------------------------------------------------------------+*/
int
connector_eof(SPC_Channel_Ptr channel,
	      int connector)
/*----------------------------------------------------------------------+*/
{
  Wire *wire=channel->wires[connector];
  Wire *tmpwire;
  int  channelflag=0;
  int  iomode=channel->IOMode;

  if(!wire)
    return(FALSE);

  wire->flags &= ~SPCIO_DATA;

  if(IS_SPCIO_STDOUT(iomode)) {
    tmpwire=channel->wires[STDOUT];
    channelflag |= IS_SPCIO_DATA(tmpwire->flags);
  }

  if(IS_SPCIO_STDERR(iomode) && IS_SPCIO_SEPARATE(iomode)) {
    tmpwire=channel->wires[STDERR];
    channelflag |= IS_SPCIO_DATA(tmpwire->flags);
  }

  if(channelflag)
    channel->IOMode |= SPCIO_DATA;
  else
    channel->IOMode &= ~SPCIO_DATA;

  return(TRUE);
}

/*
 **
 ** This routine is called when there is no subprocess associated with
 ** the channel, and an EOF is detected on a connector.  It will first
 ** call connector_eof on the channel/connector, and if the channel
 ** does not have its data flag set, it will reset the channel.
 **
*/


  
/*----------------------------------------------------------------------+*/
int
connector_eof_with_reset(SPC_Channel_Ptr channel,
			 int connector)
/*----------------------------------------------------------------------+*/
{
  connector_eof(channel, connector);
  if(!IS_DATA(channel))
    XeSPCReset(channel);

  return(TRUE);
}

/*
 **
 ** This routine is called when the child associated with the channel
 ** dies, and there is no data available to be read on the channel.
 ** It will simply reset then channel.
 **
*/

/*----------------------------------------------------------------------+*/
int
sigcld_with_reset(SPC_Channel_Ptr channel,
		  int             UNUSED_PARM(connector))
/*----------------------------------------------------------------------+*/
{
  mempf0(channel, reset);
  return(TRUE);
}
