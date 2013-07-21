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
 * File:         spc-obj.c $TOG: spc-obj.c /main/6 1997/12/29 10:43:29 bill $
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
#include <bms/MemoryMgr.h>

#include <SPC/spc-proto.h>
#include "DtSvcLock.h"

/* global declarations */

/* SPC_Initialized is in bmsglob.c */

extern int SPC_Initialized;

/* external declarations */

extern SPC_Channel_Ptr spc_activation_list;
extern XeString spc_user_environment_file;

/*
 * Global variable to specifying whether the process using this
 * library is a SPC client or a SPC daemon.  If the process is a
 * client, the SIGCLD signal handler will not be installed.  
 * However, if the process is the daemon, the signal handler will
 * be installed.
 *
 * This will be set to 'SPC_I_AM_A_DAEMON' by the spcd process.
 */
int SPC_who_am_i = SPC_I_AM_A_CLIENT;

/* Initialization functions for class objects */


/*----------------------------------------------------------------------+*/
object *alloc_channel_object(object_clasp c)
/*----------------------------------------------------------------------+*/
{
  object *p=(object *) XeMalloc((unsigned) c->object_size);
  memset(p, 0, (int) c->object_size);
  return(p);
}

/*----------------------------------------------------------------------+*/
void channel_class_init(object_clasp t)
/*----------------------------------------------------------------------+*/
{

  channel_clasp c = (channel_clasp) t;
  
  c->new_obj    = alloc_channel_object;
  
  c->open       = open_channel_object;
  c->close      = close_channel_object;
  c->read       = read_channel_object;
  c->write      = write_channel_object;
  c->reset      = reset_channel_object;
  c->pre_fork   = pre_fork_channel_object;
  c->post_fork  = post_fork_channel_object;
  c->exec_proc  = exec_proc_channel_object;
  c->signal     = signal_channel_object;
  c->wait_for_termination = channel_object_wait_for_termination;
  c->attach     = attach_channel_object;
  c->input      = NULL;
  c->add_input  = add_input_channel_object;
  c->remove_logfile = remove_logfile_channel_object;
}

static struct channel_class channel_class_struct = {
  (root_clasp) &root_class, /* base class pointer */
  "channel",                /* class name */
  channel_class_init,       /* class initialize function */
  sizeof(SPC_Channel),      /* size */
  0
  };

channel_clasp channel_class = &channel_class_struct;

static Wire dummy_wire={
  0,                             /* Flags */
  -1, -1,                        /* File Descriptors */
  (XeString) "/dev/null", 	/* Master PTY */
  (XeString) "/dev/null", 	/* Slave PTY */
  0, 0,                          /* Toolkit IDs */
  0                           /* pointer to next wire */
  };

/*----------------------------------------------------------------------+*/
int
SPC_ResetTerminator(void)
/*----------------------------------------------------------------------+*/
{
  struct sigaction svect;

  _DtSvcProcessLock();
  if (SPC_who_am_i == SPC_I_AM_A_DAEMON) {
    svect.sa_handler = SPC_Child_Terminated;
    sigemptyset(&svect.sa_mask);
    svect.sa_flags = 0;

    if(sigaction(SIGCHLD, &svect, (struct sigaction *)NULL)==ERROR) {
      SPC_Error(SPC_No_Signal_Handler);
      return(SPC_ERROR);
    }
  }

  _DtSvcProcessUnlock();
  return (TRUE);
}

/*----------------------------------------------------------------------+*/
int
SPC_Initialize(void)
/*----------------------------------------------------------------------+*/
{

  XeString home;

  _DtSvcProcessLock();
  if(SPC_Initialized) {
     _DtSvcProcessUnlock();
     return(TRUE);
  }

  spc_init_fds();

  if (!SPC_ResetTerminator()) {
     _DtSvcProcessUnlock();
     return(SPC_ERROR);
  }
    
  if(!SPC_Init_Local_Host_Info()) {
     _DtSvcProcessUnlock();
     return(SPC_ERROR);
  }

  if(SPC_Setup_Synchronous_Terminator()==SPC_ERROR) {
     _DtSvcProcessUnlock();
     return(SPC_ERROR);
  }

  if(home=getenv("HOME")) {
    spc_user_environment_file=(XeString) XeMalloc(strlen(home)+
		strlen(SPCD_ENV_HOME_DIRECTORY)+strlen(SPCD_ENV_FILE)+3);
    sprintf(spc_user_environment_file, "%s/%s/%s", 
	    home, SPCD_ENV_HOME_DIRECTORY, SPCD_ENV_FILE);
  }
  
  SPC_Initialized=TRUE;
  _DtSvcProcessUnlock();

  return(TRUE);
}

/*
 **
 ** SPC_Initialize_Channel will create & return a channel object,
 ** based on the values of hostname and iomode.
 **
*/

/*----------------------------------------------------------------------+*/
SPC_Channel_Ptr SPC_Initialize_Channel(XeString hostname,
				       int iomode)
/*----------------------------------------------------------------------+*/
{
  SPC_Channel_Ptr channel;
  
  /* Check for local or remote machine.  If remote, create a 
     remote channel object */
  if (!SPC_Local_Hostname(hostname)) {
    channel=(SPC_Channel_Ptr)object_create((object_clasp)remote_channel_class);

  } else {

    /* We are local.  Create the appropriate object. */
    
    if(IS_SPCIO_NOIOMODE(iomode))
      channel=(SPC_Channel_Ptr)object_create((object_clasp)noio_channel_class);
    
    if(IS_SPCIO_PIPE(iomode))
      channel=(SPC_Channel_Ptr)object_create((object_clasp)pipe_channel_class);
    
    if(IS_SPCIO_PTY(iomode))
      channel=(SPC_Channel_Ptr)object_create((object_clasp)pty_channel_class);

  }

  return(channel);
}

/*
 **
 ** SPC_Channel_Terminated will do any work necessary on a channel when 
 ** we detect that a subprocess has terminated.
 **
*/

/*----------------------------------------------------------------------+*/
void SPC_Channel_Terminated(SPC_Channel_Ptr channel)
/*----------------------------------------------------------------------+*/
{
  int type, cause;

  SPC_Change_State(channel, 0, -1, 0);

  /* Set the close timeout.  If we are on a PTY, we will return
     after two seconds if we are waiting for EOF */
  
  channel->close_timeout=2;

  if(IS_DATA(channel) && (channel->Input_Handler)) {
    while(IS_SPCIO_DATA(channel->wires[STDOUT]->flags))
      SPC_Input_Handler(channel, STDOUT);
    while(IS_SPCIO_DATA(channel->wires[STDERR]->flags))
      SPC_Input_Handler(channel, STDERR);
  }
  
  if(channel->Terminate_Handler) {
    XeSPCGetProcessStatus(channel, &type, &cause);
    (* channel->Terminate_Handler)
      (channel, channel->pid, type, cause, channel->Terminate_Data);
  }
      
  channel->close_timeout=0;
  
}

/*
 **
 ** SPC_Check style makes sure that we have a legal IOMode.
 **
*/

/*----------------------------------------------------------------------+*/
int
SPC_Check_Style(int iomode)
/*----------------------------------------------------------------------+*/
{

  int stylecount=0;
  
  /* First, make sure that we have only one style bit set */

  /*** NOTE - We can probably do something more tricky here, to be more
       efficient.  However, I am going to do this the slow way to be safe */

  if(IS_SPCIO_NOIOMODE(iomode))
    stylecount++;
  if(IS_SPCIO_PIPE(iomode))
    stylecount++;
  if(IS_SPCIO_PTY(iomode))
    stylecount++;

  if(stylecount != 1) {
    SPC_Error(SPC_Illegal_Iomode);
    return(SPC_ERROR);
  }

  /* Okay, now check to make sure we don't have any conflicting
     modes set */
  
  if ((IS_SPCIO_LINEEDIT(iomode) && IS_SPCIO_PIPE(iomode))     ||
      (IS_SPCIO_PTY(iomode) && IS_SPCIO_WAIT(iomode) &&
       !IS_SPCIO_TOOLKIT(iomode))                              ||
      (!IS_SPCIO_NOIO(iomode) && IS_SPCIO_USE_LOGFILE(iomode))
      )
    {
      SPC_Error(SPC_Illegal_Iomode);
      return(SPC_ERROR);
    }
  
  return(TRUE);

}

/*
 **
 ** SPC_Transform_Iomode will transform a user-specified iomode into
 ** one that is suitable for use by SPC.  It will then check the new
 ** iomode to make sure that it is legal.
 **
*/

/*----------------------------------------------------------------------+*/
int
SPC_Transform_Iomode(int iomode)
/*----------------------------------------------------------------------+*/
{
  if(IS_SPCIO_NOIO(iomode))
    iomode |= SPCIO_NOIOMODE;
  if(IS_SPCIO_DEFAULT(iomode))
    iomode |= SPCIO_DEFAULT;
  if(IS_SPCIO_TOOLKIT(iomode))
    iomode |= SPCIO_SYNC_TERMINATOR;

  /* Check to make sure that the iomode is consistent */
  
  if(SPC_Check_Style(iomode)==SPC_ERROR)
    return(SPC_ERROR);

  return(iomode);

}

/*
 **
 ** SPC_Newline_Filter will return only lines that end in newlines, or
 ** 'ntoread' characters if no newline is found in time.  It will also
 ** return as many characters as have been read in the case of EOF.
 **
*/

/*----------------------------------------------------------------------+*/
int
SPC_Newline_Filter(SPC_Channel_Ptr channel,
		   int connector, 
		   XeString buffer,
		   int ntoread)
/*----------------------------------------------------------------------+*/
{
  buffered_data_ptr cbuf;
  XeString usrptr;
  XeString cbufptr;
  int nchars, nlcopied, scalarlen, nchars_this_buffer;

  if(!(cbuf=channel->linebufs[connector])) {
    if((cbuf=SPC_New_Buffered_Data_Ptr())==SPC_ERROR)
      return(SPC_ERROR);
    channel->linebufs[connector]=cbuf;
  }

  usrptr=buffer;
  cbufptr=cbuf->data+cbuf->offset;
  nchars=0;
  nlcopied = FALSE;
  channel->IOMode &= ~SPCIO_HAS_DATA;
  
  do {

    nchars_this_buffer=0;
    scalarlen=cbuf->len;

    while(nchars<ntoread && nchars_this_buffer<scalarlen && !nlcopied) {
      nlcopied = (*cbufptr == Newline);
      *usrptr++ = (*cbufptr++);
      nchars++;
      nchars_this_buffer++;
    }

    if(nchars == ntoread || nlcopied) {
      cbuf->offset += nchars_this_buffer;
      cbuf->len    -= nchars_this_buffer;
      if(strchr(cbuf->data+cbuf->offset, Newline))
	channel->IOMode |= SPCIO_HAS_DATA;
      return(nchars);
    }

    cbufptr = cbuf->data;
    cbuf->offset = 0;
    do {
      cbuf->len=mempf3(channel, read, connector, cbufptr, SPC_BUFSIZ);
    } while(cbuf->len == (EXCEPT_FLAG));

    cbufptr[cbuf->len]=0;
    
  } while((cbuf->len) > 0);

  return(nchars);
}

/*----------------------------------------------------------------------+*/
int
SPC_Input_Handler(SPC_Channel_Ptr channel,
		  int connector)
/*----------------------------------------------------------------------+*/
{

  int nchars;
  XeChar spc_iobuffer[SPC_BUFSIZ+1];

  channel->IOMode &= ~SPCIO_HAS_DATA;

  do {
    
    nchars=(*channel->read_filter)
      (channel, connector, spc_iobuffer, SPC_BUFSIZ);
    
    /* Check nchars.  If it is EXCEPT_FLAG, we had a special occurance (such
       as an ioctl on a PTY).  In any case, don't do any more processing */
    
    if(nchars==EXCEPT_FLAG)
      return(FALSE);

    /* Call Read handlers */
    
    spc_iobuffer[nchars]=XeChar_NULL;
    
    if(channel->Input_Handler)
      (* channel->Input_Handler)
	(channel->client_data, spc_iobuffer, nchars, connector);
    
  } while(HAS_DATA(channel));
  
  return(nchars);
}

/*
 ***
 *** Method definitions for channel objects
 ***
*/

/*
 * This routine handles initialization which must occur for every channel.
 */

/*----------------------------------------------------------------------+*/
SPC_Channel_Ptr open_channel_object(SPC_Channel_Ptr channel,
				    int iomode,
				    XeString UNUSED_PARM(hostname))
/*----------------------------------------------------------------------+*/

{

  /* initialize local data structures */

  /* If we are doing line-oriented IO, set the read filter
     to be the NL filter.  Otherwise, set it to be the read
     method. */
     
  if(IS_SPCIO_LINEORIENTED(iomode))
    channel->read_filter=SPC_Newline_Filter;
  else
    channel->read_filter=channel->class_ptr->read;

  channel->cid=(int)channel;
  channel->identifier = Channel_Identifier;
  channel->IOMode     = iomode;
  channel->wires[STDIN] = (&dummy_wire);
  channel->wires[STDOUT]= (&dummy_wire);
  channel->wires[STDERR]= (&dummy_wire);
  channel->file_descs[STDIN] = -1;
  channel->file_descs[STDOUT]= -1;
  channel->file_descs[STDERR]= -1;
  channel->wire_list=NULL;
  channel->logfile=NULL;
  
  /* Link it into the activation list (at the front for now) */

  channel->next = spc_activation_list;
  spc_activation_list = channel;
  
  return(channel);
}

/*
 **
 ** This method will get called AFTER the work done in the child methods,
 ** so's we can deallocate all memory associated with this channel
 **
*/

/*----------------------------------------------------------------------+*/
int close_channel_object (SPC_Channel_Ptr channel)
/*----------------------------------------------------------------------+*/
{
  Wire *wirelist, *next_wire;
  int i;
  SPC_Channel_Ptr trail, ptr;
  
  /* Remove the channel from the activation list */

  if(spc_activation_list == channel)
    spc_activation_list = channel->next;
  else {
    trail = spc_activation_list;
    while(trail) {
      ptr = trail->next;
      if(ptr == channel) {
	trail->next = ptr->next;
	break;
      }
      trail=ptr;
    }
    if(!trail) {
      SPC_Error(SPC_Closed_Channel);
      return(SPC_ERROR);
    }
  }
  
  /* Deallocate any memory allocated to the subfields */
  
  if(IS_SPCIO_DEALLOC_ARGV(channel->IOMode))
    SPC_Free_Envp(channel->argv);
  SPC_Free_Envp(channel->envp);
  
  wirelist=channel->wire_list;
  while(wirelist) {
    next_wire=wirelist->next;
    free_wire(wirelist);
    wirelist=next_wire;
  }

  for(i=1; i<3; i++)
    if(channel->linebufs[i])
      free((char *)channel->linebufs[i]);

  /* Free the queue associated with the channel */
  
  SPC_Flush_Queued_Data(channel);
  Xe_release_queue(channel->queued_remote_data);
  
  /* Deallocate the channel */

  free((char *)channel);
  
  return(TRUE);
}

/*----------------------------------------------------------------------+*/
int read_channel_object(SPC_Channel_Ptr UNUSED_PARM(channel),
			int 		UNUSED_PARM(connector), /* STDOUT or STDERR */
			XeString 	UNUSED_PARM(buffer),
			int 		UNUSED_PARM(nbytes))
/*----------------------------------------------------------------------+*/
  
{
  /* need to check consistency between connector and READ/WRITE/ERROR here */
  return(TRUE);
}


/*----------------------------------------------------------------------+*/
int write_channel_object(SPC_Channel_Ptr UNUSED_PARM(channel),
			 XeString 	 UNUSED_PARM(buffer),
			 int 		 UNUSED_PARM(nbytes))
/*----------------------------------------------------------------------+*/
  
{
  /* check for consistent arguments (channel open for WRITE) */
  return(TRUE);
}


/*----------------------------------------------------------------------+*/
int reset_channel_object(SPC_Channel_Ptr channel)
/*----------------------------------------------------------------------+*/
{
  channel->IOMode &= ~SPCIO_DATA;
  return(TRUE);
}


/*----------------------------------------------------------------------+*/
int pre_fork_channel_object(SPC_Channel_Ptr channel)
/*----------------------------------------------------------------------+*/
{
  Wire *wirelist;
  int  flag=0;
  
  /* Set all wires to be "data ready" */
  
  for(wirelist=channel->wire_list; wirelist; wirelist=wirelist->next) {
    wirelist->flags |= SPCIO_DATA;
    flag=1;
  }

  /* Move to the "Running & (possibly) data ready" state */
  
  SPC_Change_State(channel, 0, flag, 1);

  
  return(TRUE);
}


/*----------------------------------------------------------------------+*/
int post_fork_channel_object(SPC_Channel_Ptr UNUSED_PARM(channel),
			     int 	     UNUSED_PARM(parentp))
/*----------------------------------------------------------------------+*/
{
  return(TRUE);
}


/*----------------------------------------------------------------------+*/
int  exec_proc_channel_object (SPC_Channel_Ptr channel)
/*----------------------------------------------------------------------+*/
{

  XeString *tmp_argv;
  int iomode=channel->IOMode;
  
  /* If there is no argv specified, fix it up to be the convention
     (argv[0] = file pathname) */
  
  if (channel->argv == NULL) {
    tmp_argv=Alloc_Argv(2);
    if(tmp_argv==SPC_ERROR)
      return(SPC_ERROR);
    tmp_argv[0]=SPC_copy_string(channel->path);
    tmp_argv[1]=NULL;
    channel->argv = tmp_argv;
    channel->IOMode |= SPCIO_DEALLOC_ARGV;
  }

  if(IS_SPCIO_WAIT(channel->IOMode))
    XeSPCRegisterTerminator(channel, NULL, NULL);
  
  return(TRUE);

}


/*----------------------------------------------------------------------+*/
int
signal_channel_object (SPC_Channel_Ptr UNUSED_PARM(channel),
		       int 	       UNUSED_PARM(sig))
/*----------------------------------------------------------------------+*/
{
  return(TRUE);
}


/*----------------------------------------------------------------------+*/
int channel_object_wait_for_termination(SPC_Channel_Ptr UNUSED_PARM(channel))
/*----------------------------------------------------------------------+*/
{
  return(TRUE);
}


/*----------------------------------------------------------------------+*/
int
attach_channel_object(SPC_Channel_Ptr UNUSED_PARM(channel),
		      int 	      UNUSED_PARM(pid))
/*----------------------------------------------------------------------+*/
{
  return(TRUE);
}


/*----------------------------------------------------------------------+*/
int
add_input_channel_object(SPC_Channel_Ptr    UNUSED_PARM(channel), 
			 SbInputHandlerProc UNUSED_PARM(handler),
			 void		   *UNUSED_PARM(data) )
/*----------------------------------------------------------------------+*/
{
  return(TRUE);
}


/*----------------------------------------------------------------------+*/
int
remove_logfile_channel_object(SPC_Channel_Ptr channel)
/*----------------------------------------------------------------------+*/
{
  if(IS_SPCIO_USE_LOGFILE(channel->IOMode))
    return(TRUE);
  else
    return(FALSE);
}
