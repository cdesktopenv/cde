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
 * File:         local.c $TOG: local.c /main/5 1999/10/14 15:05:57 mgreess $
 * Language:     C
 *
 * (c) Copyright 1989, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#define __need_timeval   /* Needed for "struct timeval" from <time.h>. */
#define __need_fd_set

#include <bms/sbport.h> 
#ifdef __osf__
#include <sys/time.h>     /* For declaration of select(). */
#else
#include <time.h>
#endif
#include <errno.h>
#include <signal.h>

#include <SPC/spcP.h>
#include <SPC/spc-proto.h>

/*
 **
 ** Note that the close routines call the parent method AFTER the
 ** work done for the child method.  This is because the parent method
 ** will do all the deallocation.
 **
*/

/*----------------------------------------------------------------------+*/
int close_local_channel_object(SPC_Channel_Ptr channel)
/*----------------------------------------------------------------------+*/
{
  Wire *wirelist;
  int result;
  
  for(wirelist=channel->wire_list; wirelist; wirelist=wirelist->next){
    spc_close(wirelist->fd[READ_SIDE]);
    spc_close(wirelist->fd[WRITE_SIDE]);
    SPC_XtRemoveInput(&wirelist->read_toolkit_id, SPC_Input);
    SPC_XtRemoveInput(&wirelist->except_toolkit_id, SPC_Exception);
  }

  call_parent_method(channel, close, (channel), result);

  if(result==SPC_ERROR)
    return(SPC_ERROR);

  return(TRUE);
}

/*----------------------------------------------------------------------+*/
int write_local_channel_object(SPC_Channel_Ptr channel,
			       XeString buffer,
			       int nbytes)
/*----------------------------------------------------------------------+*/
  
{
  int result;
  
  call_parent_method(channel,
		     write,
		     (channel, buffer, nbytes),
		     result);

  if(result==SPC_ERROR)
    return(SPC_ERROR);
  result = SPC_Write_Chars(channel->file_descs[STDIN], buffer, nbytes);
  if(result==ERROR) {
    SPC_Error(SPC_Writing);
    return(SPC_ERROR);
  }
  
  return(result);
}

/* the function exec_proc_local_channel_object is defined in spc-exec.c */

/*----------------------------------------------------------------------+*/
int signal_local_channel_object (SPC_Channel_Ptr channel,
				 int sig)
/*----------------------------------------------------------------------+*/

{
  int result;
  
  call_parent_method(channel, signal, (channel, sig), result);

  if(result==SPC_ERROR)
    return(SPC_ERROR);

  if(sig == SIGKILL || IS_SPCIO_SIGNAL_PGRP(channel->IOMode))
    result=kill(-(channel->pid), sig);
  else
    result=kill(channel->pid, sig);

  if(result==ERROR)
    return(errno!=ESRCH);

  return(TRUE);
}

/*----------------------------------------------------------------------+*/
int local_channel_object_wait_for_termination(SPC_Channel_Ptr channel)
/*----------------------------------------------------------------------+*/
{

  int result;
  
  call_parent_method(channel, wait_for_termination, (channel), result);
  
  if(result==SPC_ERROR)
    return(SPC_ERROR);
  
  /* Do we need to check for remote channel input here? */
  
  while(IS_ACTIVE(channel)) {
    sigset_t mask;
    sigemptyset(&mask);
    /* the SIGCLD signal handler will take care of us here */
    sigsuspend(&mask);
  }

  return(TRUE);
  
}

/*----------------------------------------------------------------------+*/
int remove_logfile_local_channel_object(SPC_Channel_Ptr channel)
/*----------------------------------------------------------------------+*/
{
  int result;

  call_parent_method(channel, remove_logfile, (channel), result);
  
  if(unlink(channel->logfile)==ERROR) {
    SPC_Error(SPC_Unlink_Logfile);
    return(SPC_ERROR);
  }

  /* This is malloc'ed memory from open_noio_channel_object() and tempnam() */
  XeFree(channel->logfile);
  
  return(TRUE);
}

extern SPC_Channel_Ptr spc_activation_list;

/* All this routine does is to look up the channel, and
   call the generic input handler routine */

/*----------------------------------------------------------------------+*/
void local_channel_object_input_handler(void * client_data,
				   int *source,
				   SPCInputId * UNUSED_PARM(id))
/*----------------------------------------------------------------------+*/
{

/* WARNING!!! This routine is NOT XPG3 compliant.  The timeval struct */
/*            is the problem here. 				      */

  SPC_Channel_Ptr channel=(SPC_Channel_Ptr) client_data;
  int fd=(*source);
  int connector;
  int len;
  fd_set read_fd_vect, except_fd_vect;
  SPC_Channel_Ptr tmp, this_ptr;
  struct timeval timeout;		  /* Not part of XPG3 !!! */

  /* This ^&@$#% select is here to get around an X toolkit bug */

  FD_ZERO(&read_fd_vect);
  FD_ZERO(&except_fd_vect);

  FD_SET(fd, &read_fd_vect);
  FD_SET(fd, &except_fd_vect);

  timeout.tv_sec = 0;
  timeout.tv_usec = 0;
  
#if defined(SVR4) || defined(__osf__) || defined(__hpux) || defined(__OpenBSD__)
  select(max_fds, (fd_set*)&read_fd_vect, NULL, (fd_set*)&except_fd_vect, &timeout);
#else
  /* UX has select defined with int*, not fd_set* parms */
  select(max_fds, (int*)&read_fd_vect, NULL, (int*)&except_fd_vect, &timeout);
#endif
  if(! (FD_ISSET(fd, &read_fd_vect) || FD_ISSET(fd, &except_fd_vect))) {
    return /* (FALSE) */;
  }

  /* The following is to get around an apparent Xt bug where sometimes
     the client data pointer passed to me is not the one I was expecting.
     */
  
  tmp = spc_activation_list;
  this_ptr = NULL;
  while(tmp) {
    if((fd == Stdin(tmp)) || (fd == Stderr(tmp)))
      this_ptr = tmp;
    tmp = tmp->next;
  }
  
  if(this_ptr == NULL)
    this_ptr = channel;
  if(this_ptr != channel)
    channel = this_ptr;
  
  if((connector=SPC_fd_to_connector(channel, fd)) == ERROR) {
    SPC_Error(SPC_Bad_Fd);
    return /* (SPC_ERROR) */;
  }
  len = SPC_Input_Handler(channel, connector);
  return /* (len) */;
}

int local_channel_object_send_eof(SPC_Channel_Ptr channel)
{
  Wire *wire = channel->wires[STDIN];

  spc_close(wire->fd[READ_SIDE]);
  spc_close(wire->fd[WRITE_SIDE]);
  SPC_XtRemoveInput(&wire->read_toolkit_id, SPC_Input);
  SPC_XtRemoveInput(&wire->except_toolkit_id, SPC_Exception);

  return(TRUE);
}
