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
 * File:         spc.c $XConsortium: spc.c /main/6 1996/06/21 17:33:08 ageorge $
 * Language:     C
 *
 * (c) Copyright 1988, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#include <bms/sbport.h> /* NOTE: sbport.h must be the first include. */
#include <signal.h>
#include <SPC/spcP.h>
#include <SPC/spc-proto.h>
#include <bms/spc.h>
#include "DtSvcLock.h"

/* spc.c */
int SPC_Process_Single_Prot_Request (protocol_request_ptr req, SPC_Channel_Ptr channel);


/* This is the SPC error number variable */
/* extern int XeSPCErrorNumber; */

/* Externals */

extern int SPC_Initialized;
extern SPC_Channel_Ptr spc_activation_list;
extern SPC_Connection_Ptr connection_list;
extern SPC_Connection_Ptr read_terminator;
extern XeString official_hostname;

int max_fds = 0;   /* Set up below. */

/*----------------------------------------------------------------------+*/
void 
spc_init_fds(void)
/*----------------------------------------------------------------------+*/
{
   _DtSvcProcessLock();
   if (!max_fds)
#     ifdef __bsd
         max_fds = getdtablesize();
#     else
         max_fds = (int)sysconf(_SC_OPEN_MAX);
#     endif
   _DtSvcProcessUnlock();
}

/*
 ***
 *** Sub-Process Control access routines.  These are just functional
 *** interfaces to the underlying methods.
 ***
*/

/*----------------------------------------------------------------------+*/
SPC_Channel_Ptr XeSPCOpen(XeString hostname,
			  int iomode)
/*----------------------------------------------------------------------+*/
{
  /* Attempt to open an SPC channel - return TRUE if we succeed */
  SPC_Channel_Ptr channel;

  /* Check for initialization */
  _DtSvcProcessLock();
  if(!SPC_Initialized)
      if(SPC_Initialize() == SPC_ERROR) {
	  _DtSvcProcessUnlock();
	  return(SPC_ERROR);
      }
  _DtSvcProcessUnlock();

  /* The user specified iomode needs to be processed before we can use it.
     Process the puppy. */

  iomode=SPC_Transform_Iomode(iomode);
  if(iomode==SPC_ERROR)
    return(SPC_ERROR);
       
  /* Get a new channel object */
  
  channel=SPC_Initialize_Channel(hostname, iomode);

  /* check that everything was okay */
  
  if(channel==SPC_ERROR)
    return(SPC_ERROR);
  
  /* call the open method for it */

  return((SPC_Channel_Ptr) mempf2(channel, open, iomode, hostname));

}

/*----------------------------------------------------------------------+*/
int
XeSPCClose(SPC_Channel_Ptr channel)
/*----------------------------------------------------------------------+*/
{
  if(channel==SPC_ERROR) {
    SPC_Error(SPC_Bad_Argument);
    return(SPC_ERROR);
  }

  if(IS_ACTIVE(channel))
    XeSPCKillProcess(channel, FALSE);

  if(IS_SPCIO_DELAY_CLOSE(channel->IOMode)) {
    channel->IOMode |= SPCIO_DO_CLOSE;
    return(TRUE);
  }
  
  channel->IOMode &= ~SPCIO_DO_CLOSE;
  
  return(mempf0(channel, close));
  
}

/*----------------------------------------------------------------------+*/
int
XeSPCReset(SPC_Channel_Ptr channel)
/*----------------------------------------------------------------------+*/
{

  if(channel==SPC_ERROR) {
    SPC_Error(SPC_Bad_Argument);
    return(SPC_ERROR);
  }

  if(IS_ACTIVE(channel))
    XeSPCKillProcess(channel, FALSE);
  
  channel->IOMode &= ~SPCIO_ACTIVE;

  if(mempf0(channel, reset)==SPC_ERROR)
    return(SPC_ERROR);
  
  return(TRUE);
  
}

/*----------------------------------------------------------------------+*/
int
XeSPCRead(SPC_Channel_Ptr channel,
	  int connector,           /* STDOUT or STDERR */
	  XeString buffer,
	  int length)
/*----------------------------------------------------------------------+*/
{

  int n;
  
  /* check for legal arguments */
  
  if (channel==SPC_ERROR || !buffer ||
      !(connector==STDOUT || connector==STDERR) ||
      (length < 0)) {
    SPC_Error(SPC_Bad_Argument);
    return(SPC_ERROR);
  }

  /* check state of the channel */
  if (!IS_DATA(channel) || !IS_SPCIO_DATA(channel->wires[connector]->flags))
    return(0);
  
  /* call the read filter */

  do {
    n=(*channel->read_filter)(channel, connector, buffer, length);
  } while(n == (EXCEPT_FLAG));
  
  /* Check for an error */

  if (n == SPC_ERROR)
    return(SPC_ERROR);
  
  return(n);
}

  
/*----------------------------------------------------------------------+*/
int
XeSPCWrite(SPC_Channel_Ptr channel,
	   XeString buffer,
	   int length)
/*----------------------------------------------------------------------+*/
{
  int n;
  
  /* check for legal arguments */
  
  if (channel==SPC_ERROR || !buffer || (length<0)) {
    SPC_Error(SPC_Bad_Argument);
    return(SPC_ERROR);
  }

  /* check the state of the channel */

  if(!IS_ACTIVE(channel)) {
    SPC_Error(SPC_Inactive_Channel);
    return(SPC_ERROR);
  }
  
  /* call the write method */
  
  n=mempf2(channel, write, buffer, length);
  
  return(n);
}

/*----------------------------------------------------------------------+*/
int
XeSPCActive(SPC_Channel_Ptr channel)
/*----------------------------------------------------------------------+*/
{

  if (channel==SPC_ERROR) {
    SPC_Error(SPC_Bad_Argument);
    return(SPC_ERROR);
  }
  
  /* Is the passed channel active? */
  return (IS_ACTIVE(channel));
}

/*----------------------------------------------------------------------+*/
int
XeSPCData(SPC_Channel_Ptr channel)
/*----------------------------------------------------------------------+*/
{

  if(channel==SPC_ERROR) {
    SPC_Error(SPC_Bad_Argument);
    return(SPC_ERROR);
  }
  
  return(IS_DATA(channel));

}

/*----------------------------------------------------------------------+*/
int
XeSPCExecuteProcess(SPC_Channel_Ptr channel)
/*----------------------------------------------------------------------+*/
{
  int retval;

  if (channel==SPC_ERROR) {
    SPC_Error(SPC_Bad_Argument);
    return(SPC_ERROR);
  }
  
  if(IS_ACTIVE(channel) || IS_DATA(channel)) {
    SPC_Error(SPC_Active_Channel);
    return(SPC_ERROR);
  }

  if((retval=mempf0(channel, exec_proc))==SPC_ERROR)
    return(SPC_ERROR);

  if (IS_SPCIO_WAIT(channel->IOMode)) {
    /* Wait for sub-process to finish */
    SPC_Wait_For_Termination(channel);
  }
  
  return(retval);

}

/*----------------------------------------------------------------------+*/
int
XeSPCSignalProcess(SPC_Channel_Ptr channel,
		   int sig)
/*----------------------------------------------------------------------+*/
{
  
  if ((channel==SPC_ERROR) ||
      (channel->pid <= 0) ||
      (sig < 0) 
#ifdef NOT_IN_XPG3_YET      
      || (sig>=NSIG)	/* Not a good idea for interoperability anyway */
#endif
      ) {
    SPC_Error(SPC_Bad_Argument);
    return(SPC_ERROR);
  }

  /* This routine does not check to see if the channel is active. */

  return(mempf1(channel, signal, sig));
}

/*----------------------------------------------------------------------+*/
int
XeSPCAddInput(SPC_Channel_Ptr	channel,
	      SbInputHandlerProc handler,
/*----------------------------------------------------------------------+*/
	      void *		client_data)
{
  if(!channel) {
    SPC_Error(SPC_Bad_Argument);
    return(SPC_ERROR);
  }
  
  if(!handler && !channel->Input_Handler)
    return(TRUE);

  if(handler) {
    channel->Input_Handler = handler;
    channel->client_data   = client_data;
  }

  return(mempf2(channel, add_input, handler, client_data));
    
}


/*----------------------------------------------------------------------+*/
int
XeSPCRegisterTerminator(SPC_Channel_Ptr			channel,
			SPC_TerminateHandlerType	terminator,
			void * 				client_data)
/*----------------------------------------------------------------------+*/
{

  SPC_Connection_Ptr conn;
  
  if(channel==SPC_ERROR) {
    SPC_Error(SPC_Bad_Argument);
    return(SPC_ERROR);
  }

  /* Okay.  If we have a pty channel, we have to check that we
     have an input handler.  I don't like doing this here, but ther
     are no methods for this routine, so it has to be done. */

  if(IS_SPCIO_PTY(channel->IOMode) && !channel->Input_Handler) {
    SPC_Error(SPC_Bad_Argument);
    return(SPC_ERROR);
  }

  channel->IOMode |= SPCIO_SYNC_TERMINATOR;
  if(terminator) {
    channel->Terminate_Handler = terminator;
    channel->Terminate_Data    = client_data;
  }

  conn=SPC_Channel_Terminator_Connection(channel);
  if(conn->termination_id == -1)
    SPC_XtAddInput(channel, &conn->termination_id, conn->sid,
		   SPC_Conditional_Packet_Handler, SPC_Terminator);
  
  return(TRUE);
  
}


/*----------------------------------------------------------------------+*/
int
XeSPCAttach(SPC_Channel_Ptr channel,
	    int pid)
/*----------------------------------------------------------------------+*/

{
  if(channel==SPC_ERROR || pid<=0) {
    SPC_Error(SPC_Bad_Argument);
    return(SPC_ERROR);
  }

  return(mempf1(channel, attach, pid));
}

/*----------------------------------------------------------------------+*/
int
XeSPCDetach(SPC_Channel_Ptr channel)
/*----------------------------------------------------------------------+*/
{
  if(channel == SPC_ERROR) {
    SPC_Error(SPC_Bad_Argument);
    return(SPC_ERROR);
  }
  
  channel->pid = 0;
  channel->IOMode &= ~SPCIO_ACTIVE;

  return XeSPCReset(channel);
}
  

 
/*
 ***
 *** "Composite" functions -- Those which are defined in terms of the
 *** above primitives.  One assumption is that the above primitives check
 *** their arguments, so the composite functions need only check any
 *** additional arguments.
 ***
*/

/*
 **
 ** Start with subprocess creation routines
 **
*/

/*----------------------------------------------------------------------+*/
int
XeSPCSpawn(XeString pathname,
	   XeString context_dir,
	   XeString *argv,
	   XeString *envp,
	   SPC_Channel_Ptr channel)
/*----------------------------------------------------------------------+*/
{

  if(channel==SPC_ERROR) {
    SPC_Error(SPC_Bad_Argument);
    return(SPC_ERROR);
  }
  
  /* Assign the command arguments to this channel and attempt to Execute */
    
  if(channel->envp) {
    SPC_Free_Envp(channel->envp);
    channel->envp=NULL;
  }
  if(channel->argv && IS_SPCIO_DEALLOC_ARGV(channel->IOMode)) {
    SPC_Free_Envp(channel->argv);
    channel->IOMode &= ~SPCIO_DEALLOC_ARGV;
    channel->argv=NULL;
  }
  
  channel->context_dir=context_dir;
  channel->argv = argv;
  channel->path = pathname;

  channel->envp=SPC_Create_Default_Envp(channel->envp);
  channel->envp=SPC_Merge_Envp(channel->envp, envp);
  channel->envp=SPC_Fixup_Environment(channel->envp, channel);

  if(IS_SPCIO_SYSTEM(channel->IOMode))
    if(SPC_MakeSystemCommand(channel)==SPC_ERROR)
      return(SPC_ERROR);
  
  /* Execute the process (XeSPCExecuteProcess will check arguments */
  
  return(XeSPCExecuteProcess(channel));
}

/*----------------------------------------------------------------------+*/
SPC_Channel_Ptr XeSPCOpenAndSpawn(XeString hostname,
				  int iomode,
				  XeString pathname,
				  XeString context_dir,
				  XeString *argv,
				  XeString *envp)
/*----------------------------------------------------------------------+*/
{
  /* This simply wraps together the two steps: Open and Spawn */
  SPC_Channel_Ptr channel;

  channel = XeSPCOpen(hostname, iomode);
  if(channel==SPC_ERROR)
    return(SPC_ERROR);
  
  if (XeSPCSpawn(pathname, context_dir, argv, envp, channel)!=SPC_ERROR)
    return(channel);

  /* Close the channel and return SPC_ERROR */
  XeSPCClose(channel);
  return(SPC_ERROR);
  
}

/*
 **
 ** Signalling routines
 **
*/

/*----------------------------------------------------------------------+*/
void
XeSPCKillProcesses(int wait)
/*----------------------------------------------------------------------+*/
{
  /* Attempt to KILL all the sub-process that we know of */
  SPC_Channel_Ptr spc;

  _DtSvcProcessLock();
  for (spc = spc_activation_list; spc != (SPC_Channel_Ptr) NULL; spc = spc->next)
    XeSPCKillProcess(spc, wait);
  _DtSvcProcessUnlock();
}

/*----------------------------------------------------------------------+*/
int
XeSPCKillProcess(SPC_Channel_Ptr channel,
		 int wait)
/*----------------------------------------------------------------------+*/
{
  /* Attempt to KILL the sub-process (should we nullify the pid?) */
  int result;

  if(!channel)
    return(FALSE);

  if(IS_ACTIVE(channel)) {
    result = XeSPCSignalProcess(channel, SIGKILL);
    if(result==SPC_ERROR)
      return(SPC_ERROR);
    if (wait || IS_SPCIO_SYNC_TERM(channel->IOMode))
      SPC_Wait_For_Termination(channel);
    return result;
  } else
    return(TRUE);
}

/*----------------------------------------------------------------------+*/
int
XeSPCInterruptProcess(SPC_Channel_Ptr channel)
/*----------------------------------------------------------------------+*/
{
  /* Attempt to INTerrupt the sub-process */
  
  return(XeSPCSignalProcess(channel, SIGINT));
}

/*
 **
 ** Process information routines.
 **
*/

/*----------------------------------------------------------------------+*/
XeString XeSPCGetDevice(SPC_Channel_Ptr channel,
			      int connector, 
			      int side)
/*----------------------------------------------------------------------+*/
{
  if(!channel) {
    SPC_Error(SPC_Bad_Argument);
    return(SPC_ERROR);
  }
  
  /* Return the device name which corresponds to the side of a channel */
  if (connector>=STDIN && connector<=STDERR) {
    if (side == MASTER_SIDE)
      return(channel->wires[connector]->master_name);
    if (side == SLAVE_SIDE)
      return(channel->wires[connector]->slave_name);
  }
  
  /* For no channel or incorrect side, return SPC_ERROR */
  
  SPC_Error(SPC_Bad_Argument);
  return(SPC_ERROR);
}

/*----------------------------------------------------------------------+*/
int
XeSPCGetProcessStatus(SPC_Channel_Ptr channel,
		      int *type, 
		      int *cause)
/*----------------------------------------------------------------------+*/
{
  /* Fill in the type and cause of a process termination */
  int high, low;

  if(!channel || !type || !cause) {
    SPC_Error(SPC_Bad_Argument);
    return(SPC_ERROR);
  }
    
  low = channel->status & WAIT_STATUS_MASK;
  high = (channel->status >> 8) & WAIT_STATUS_MASK;

  *cause = high;

  switch (low) {

  case IS_WAIT_STATUS_STOPPED:
    *type = SPC_PROCESS_STOPPED;
    break;

  case IS_WAIT_STATUS_EXITED:
    *type = SPC_PROCESS_EXITED;
    break;

  default:
    if (!*cause) {
      *cause = low;
      *type = SPC_PROCESS_SIGNALLED;
    }
    break;

  }				/* End switch on status */

  /* When a process is still active return FALSE */
  return(TRUE);
}

/*----------------------------------------------------------------------+*/
int
XeSPCGetPID(SPC_Channel_Ptr channel)
/*----------------------------------------------------------------------+*/
{

  if(!channel) {
    SPC_Error(SPC_Bad_Argument);
    return(SPC_ERROR);
  }
  return (channel->pid);
}

/*----------------------------------------------------------------------+*/
int XeSPCGetLogfile(SPC_Channel_Ptr channel,
		    XeString *host, 
		    XeString *file)
/*----------------------------------------------------------------------+*/
{

  if(!channel || !IS_SPCIO_USE_LOGFILE(channel->IOMode)) {
    SPC_Error(SPC_Bad_Argument);
    return(SPC_ERROR);
  }
  *file=channel->logfile;
  _DtSvcProcessLock();
  if(IS_REMOTE(channel))
    *host=channel->connection->hostname;
  else
    *host= official_hostname;
  _DtSvcProcessUnlock();
  return(TRUE);
  
}

/*----------------------------------------------------------------------+*/
int XeSPCRemoveLogfile(SPC_Channel_Ptr channel)
/*----------------------------------------------------------------------+*/
{ 

  if(!channel) {
    SPC_Error(SPC_Bad_Argument);
    return(SPC_ERROR);
  }
  
  return(mempf0(channel, remove_logfile));
}

/*
 **
 ** Synchronous termination
 **
*/

#define SINGLE_PROT_DATA(req, channel, connector)  \
  if(!channel->queued_remote_data)                 \
    channel->queued_remote_data=Xe_make_queue(FALSE); \
  Xe_push_queue(channel->queued_remote_data, req);    \
  if(channel->Input_Handler) {                     \
    SPC_Input_Handler(channel, connector);         \
  }

/*
 **
 ** SPC_Process_Single_Prot_Request will return TRUE if it is okay
 ** for the caller to free the protocol request.
 **
*/

/*----------------------------------------------------------------------+*/
int
SPC_Process_Single_Prot_Request(protocol_request_ptr req, SPC_Channel_Ptr channel)
/*----------------------------------------------------------------------+*/
{

  switch(req->request_type) {
    
  case APPLICATION_DIED:
    
    READ_APPLICATION_DIED(req->dataptr, channel->status);
    SPC_Channel_Terminated(channel);
    return(TRUE);
    
  case APPLICATION_STDOUT:
    SINGLE_PROT_DATA(req, channel, STDOUT);
    return(FALSE);
    
  case APPLICATION_STDERR:
    SINGLE_PROT_DATA(req, channel, STDERR);
    return(FALSE);
      
  default:
    SPC_Error(SPC_Internal_Error);
    return(SPC_ERROR);
  }
}    
  
/*----------------------------------------------------------------------+*/
SPC_Channel_Ptr XeSPCHandleTerminator(int fd)
/*----------------------------------------------------------------------+*/
{
  SPC_Connection_Ptr connection;
  SPC_Channel_Ptr channel;
  protocol_request_ptr prot;
  XeQueue connection_queue;
  
  if(!(connection=SPC_Lookup_Connection_Fd(fd))) {
    SPC_Error(SPC_Bad_Argument);
    return(SPC_ERROR);
  }
  
  if(!(prot=SPC_Read_Protocol(connection))) {
    return(SPC_ERROR);
  }

  connection_queue=connection->queued_remote_data;
  Xe_push_queue(connection_queue, prot);

  while((prot=(protocol_request_ptr)Xe_pop_queue(connection_queue))) {
    
    channel=prot->channel;

    if(channel) {
      channel->IOMode |= SPCIO_DELAY_CLOSE;
      
      if(SPC_Process_Single_Prot_Request(prot, channel))
	SPC_Free_Protocol_Ptr(prot);
      
      channel->IOMode &= ~SPCIO_DELAY_CLOSE;
      if(IS_SPCIO_DO_CLOSE(channel->IOMode)) {
	XeSPCClose(channel);
	channel = NULL;
      }
    }
    
  }
  
  return(channel);
  
}

/*
 **
 ** Use this call to get the file descriptor for
 ** Synchronous termination.
 **
*/

/*----------------------------------------------------------------------+*/
int
XeSPCGetChannelSyncFd(SPC_Channel_Ptr channel)
/*----------------------------------------------------------------------+*/
{

  SPC_Connection_Ptr conn;

  if(channel==SPC_ERROR) {
    SPC_Error(SPC_Bad_Argument);
    return(SPC_ERROR);
  }
  
  conn=SPC_Channel_Terminator_Connection(channel);
  return(conn->sid);
  
}

/*
 ***
 *** Error Handling routines.
 ***
*/

/*----------------------------------------------------------------------+*/
SPCError *XeSPCLookupError(int errnum)
/*----------------------------------------------------------------------+*/
{
  if(errnum<SPC_Min_Error || errnum > SPC_Max_Error) {
    SPC_Error(SPC_Bad_Argument);
    return(SPC_ERROR); 
  }
   
  return(SPC_Lookup_Error(errnum));
}

/*
 ***
 *** Temporarily shutdown input handlers
 ***
*/

/*----------------------------------------------------------------------+*/
void XeSPCShutdownCallbacks(void)
/*----------------------------------------------------------------------+*/
{
  SPC_Channel_Ptr channel;
  SPC_Connection_Ptr conn;
  Wire *wirelist;

  _DtSvcProcessLock();
  channel=spc_activation_list;
  conn=connection_list;

  while(channel) {
    for(wirelist=channel->wire_list; wirelist; wirelist=wirelist->next) {
      if(wirelist->read_toolkit_id   != -1)
	SPC_XtRemoveInput(&wirelist->read_toolkit_id, SPC_Input);
      if(wirelist->except_toolkit_id != -1)
	SPC_XtRemoveInput(&wirelist->except_toolkit_id, SPC_Exception);
    }
    channel=channel->next;
  }

  while(conn) {
    if(conn->termination_id != -1)
      SPC_XtRemoveInput(&conn->termination_id, SPC_Terminator);
    conn=conn->next;
  }
  _DtSvcProcessUnlock();
}

/*----------------------------------------------------------------------+*/
void XeSPCRestartCallbacks(void)
/*----------------------------------------------------------------------+*/
{
  SPC_Channel_Ptr channel;

  _DtSvcProcessLock();
  channel=spc_activation_list;

  while(channel) {
    if(channel->Input_Handler)
      XeSPCAddInput(channel, (SbInputHandlerProc)NULL, NULL);
    if(channel->Terminate_Handler)
      XeSPCRegisterTerminator(channel, NULL, NULL);
    channel=channel->next;
  }
  _DtSvcProcessUnlock();
}

/*
 ***
 *** Okay, now for a non-SPC routine.  This one is dealing with setpgrp,
 *** but it is here because it uses internal SPC routines.
 ***
*/

/*----------------------------------------------------------------------+*/
int
XeSetpgrp(int read_current_termio)
/*----------------------------------------------------------------------+*/
{

  return(SPC_Setpgrp(read_current_termio));
}  

int XeSPCSendEOF(SPC_Channel_Ptr channel)
{
  if(channel==SPC_ERROR) {
    SPC_Error(SPC_Bad_Argument);
    return(SPC_ERROR);
  }
  
  return(mempf0(channel, send_eof));
}

int XeSPCSetTermio(SPC_Channel_Ptr channel, int connection, int side,
		   struct termios *termio)
{
  if(channel==SPC_ERROR || termio == NULL) {
    SPC_Error(SPC_Bad_Argument);
    return(SPC_ERROR);
  }
  
  return(mempf3(channel, set_termio, connection, side, termio));
}

