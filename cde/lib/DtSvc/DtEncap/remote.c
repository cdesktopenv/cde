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
 * File:         remote.c $XConsortium: remote.c /main/5 1996/06/21 17:34:53 ageorge $
 * Language:     C
 *
 * (c) Copyright 1989, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#include <bms/sbport.h>
#include <bms/usersig.h>

#include <SPC/spcP.h>
#include <SPC/spc-proto.h>
#include "DtSvcLock.h"

/*----------------------------------------------------------------------+*/
void remote_channel_class_init(object_clasp t)
/*----------------------------------------------------------------------+*/
{
  channel_clasp c=(channel_clasp) t;
  
  c->new_obj   = alloc_channel_object;
  
  c->open      = open_remote_channel_object;
  c->close     = close_remote_channel_object;
  c->read      = read_remote_channel_object;
  c->write     = write_remote_channel_object;
  c->reset     = reset_remote_channel_object;
  /* pre_fork & post_fork have no method */
  c->exec_proc = exec_proc_remote_channel_object;
  c->signal    = signal_remote_channel_object;
  /* wait_for_termination has no method */
  c->attach    = attach_remote_channel_object;
  c->add_input = add_input_remote_channel_object;
  c->input     = SPC_Conditional_Packet_Handler;
  c->remove_logfile = remove_logfile_remote_channel_object;

  /* New B.00 methods */

  c->send_eof = send_eof_remote_channel_object;
  c->set_termio = set_termio_remote_channel_object;
  
}

static struct remote_channel_class remote_channel_class_struct = {
  (channel_clasp) &channel_class, /* base class pointer */
  "remote_channel",         /* class name */
  remote_channel_class_init,/* class initialize function */
  sizeof(SPC_Channel),      /* size */
  0
  };

remote_channel_clasp remote_channel_class = &remote_channel_class_struct;

/*----------------------------------------------------------------------+*/
Wire *get_new_remote_wire(Wire *prevwire)
/*----------------------------------------------------------------------+*/
{
  Wire *mywire=get_new_wire();

  if(mywire==SPC_ERROR)
    return(SPC_ERROR);
  mywire->next=prevwire;
  return(mywire);
}
  
/*
 ***
 *** Method definitions for remote channel objects
 ***
*/

/*----------------------------------------------------------------------+*/
SPC_Channel_Ptr open_remote_channel_object(SPC_Channel_Ptr channel,
					   int iomode,
					   XeString hostname)
/*----------------------------------------------------------------------+*/
{

  SPC_Channel_Ptr result;
  int seqno, retval;
  Wire *tmpwire=NULL;
  int sid;
  
  call_parent_method(channel, open, (channel, iomode, hostname), result);

  if(result==SPC_ERROR)
    return(SPC_ERROR);
  
  channel->cid=0;
  channel->queued_remote_data=Xe_make_queue(NULL);
  iomode=channel->IOMode;

  if(IS_SPCIO_STDIN(iomode)) {
    tmpwire=get_new_remote_wire(tmpwire);
    channel->wires[STDIN]=tmpwire;
  }
  if(IS_SPCIO_STDOUT(iomode)) {
    tmpwire=get_new_remote_wire(tmpwire);
    channel->wires[STDOUT]=tmpwire;
  }
  if(IS_SPCIO_STDERR(iomode)) {
    if(!tmpwire ||                      /* ERRORONLY */
       IS_SPCIO_SEPARATE(iomode))
      tmpwire=get_new_remote_wire(tmpwire);
    channel->wires[STDERR]=tmpwire;
  }

  channel->wire_list=tmpwire;
  
  if(!(channel->connection=SPC_Open_Connection(hostname)))
    return(SPC_ERROR);

  seqno=SPC_Write_Protocol_Request(channel->connection, channel,
				   CHANNEL_OPEN, iomode);
  retval=SPC_Waitfor_Reply(channel->connection, channel, seqno);
  if(retval==SPC_ERROR)
    return(SPC_ERROR);
  channel->cid=retval;
  sid=channel->connection->sid;
  channel->file_descs[STDIN]=sid;
  channel->file_descs[STDOUT]=sid;
  channel->file_descs[STDERR]=sid;

  if ((SPC_client_version_number >= SPC_PROTOCOL_VERSION_CDE_BASE) &&
      (SPC_who_am_i == SPC_I_AM_A_CLIENT)) {
    channel->wires[STDIN]->master_name = NULL;
    channel->wires[STDIN]->slave_name = NULL;
    channel->wires[STDOUT]->master_name = NULL;
    channel->wires[STDOUT]->slave_name = NULL;
    channel->wires[STDERR]->master_name = NULL;
    channel->wires[STDERR]->slave_name = NULL;
  }
  else
    SPC_Query_Devices(channel);

  SPC_Query_Logfile(channel);
  
  return(channel);
}

/*
 **
 ** Note that the close routines call the parent method AFTER the
 ** work done for the child method.  This is because the parent method
 ** will do all the deallocation.
 **
*/

/*----------------------------------------------------------------------+*/
int close_remote_channel_object(SPC_Channel_Ptr channel)
/*----------------------------------------------------------------------+*/
{

  int result;
  int seqno, retval;

  channel->IOMode |= SPCIO_DELAY_CLOSE;
  
  seqno=SPC_Write_Protocol_Request(channel->connection, channel,
				   CHANNEL_CLOSE);
  retval=SPC_Waitfor_Reply(channel->connection, channel, seqno);

  call_parent_method(channel, close, (channel), result);

  if(result==SPC_ERROR)
    return(SPC_ERROR);

  return(retval);
}

/*----------------------------------------------------------------------+*/
int reset_remote_channel_object(SPC_Channel_Ptr channel)
/*----------------------------------------------------------------------+*/
{

  int result;
  int seqno, retval;
  XeQueue tmpqueue;
  protocol_request_ptr prot;
  
  call_parent_method(channel, reset, (channel), result);

  if(result==SPC_ERROR)
    return(SPC_ERROR);

  seqno=SPC_Write_Protocol_Request(channel->connection, channel,
				   CHANNEL_RESET);
  retval=SPC_Waitfor_Reply(channel->connection, channel, seqno);
  if(retval==SPC_ERROR)
    return(SPC_ERROR);

  /* At this point, we know that the remote server has sent us all
     necessary data (since RESET expects a reply).  So, we are safe to
     flush any queued data on this channel. */

  {
    if ((tmpqueue=channel->queued_remote_data))
    {
      Xe_for_queue(protocol_request_ptr, prot, tmpqueue) {
	Xe_delete_queue_element(channel->queued_remote_data, prot);
	SPC_Free_Protocol_Ptr(prot);
      }
    }
  }
  
  return(TRUE);
}

/*----------------------------------------------------------------------+*/
int exec_proc_remote_channel_object(SPC_Channel_Ptr channel)
/*----------------------------------------------------------------------+*/
{
  int result;
  int retval, seqno;
  int tmp_errorno;
  
  call_parent_method(channel, exec_proc, (channel), result);
  
  if(result==SPC_ERROR)
    return(SPC_ERROR);
  
  call_parent_method(channel, pre_fork, (channel), result);
  
  if(result==SPC_ERROR)
    return(SPC_ERROR);

  if((channel->connection->protocol_version > 2) ||
     IS_SPCIO_FORCE_CONTEXT(channel->IOMode))
    seqno=SPC_Write_Protocol_Request(channel->connection, channel,
				     APP_B00_SPAWN,
				     channel->path, channel->context_dir,
				     channel->argv, channel->envp);
  else
    seqno=SPC_Write_Protocol_Request(channel->connection, channel,
				     APPLICATION_SPAWN,
				     channel->path, channel->context_dir,
				     channel->argv, channel->envp);

  _DtSvcProcessLock();
  tmp_errorno = XeSPCErrorNumber;
  retval=SPC_Waitfor_Reply(channel->connection, channel, seqno);
  if (tmp_errorno != 0)
     XeSPCErrorNumber = tmp_errorno;
  _DtSvcProcessUnlock();
  if(retval==SPC_ERROR)
    return(SPC_ERROR);

  channel->pid=retval;

  call_parent_method(channel, post_fork, (channel, retval), result);
  
  if(result==SPC_ERROR)
    return(SPC_ERROR);
  
  return(TRUE);
}

/*----------------------------------------------------------------------+*/
int write_remote_channel_object(SPC_Channel_Ptr channel,
				XeString buffer,
				int len)
/*----------------------------------------------------------------------+*/
{
  int result;
  int seqno, retval;
  int lentogo=len, lentowrite;
  
  call_parent_method(channel, write, (channel, buffer, len), result);
  
  if(result==SPC_ERROR)
    return(SPC_ERROR);

  do {

    lentowrite=min(lentogo, SPC_BUFSIZ);
    seqno=SPC_Write_Protocol_Request(channel->connection, channel,
				      APPLICATION_DATA, buffer, lentowrite);
    retval=SPC_Waitfor_Reply(channel->connection, channel, seqno);

    if(retval==SPC_ERROR)
      return(SPC_ERROR);

    lentogo -= lentowrite;
    buffer  += lentowrite;
    
  } while(lentogo>0);
  
  return(len);
}
  
/*----------------------------------------------------------------------+*/
int read_remote_channel_object(SPC_Channel_Ptr channel,
			       int connection,
			       XeString buffer,
			       int len)
/*----------------------------------------------------------------------+*/
{
  int result;
  
  call_parent_method(channel, read, (channel, connection, buffer, len), result);
  
  if(result==SPC_ERROR)
    return(SPC_ERROR);

  if(!IS_SPCIO_DATA(channel->wires[connection]->flags))
    return(0);
  
  result=SPC_Read_Remote_Data(channel, connection, buffer, len);
  if(result == 0)
    /* Got EOF.  Yank the data line */
    SPC_Change_State(channel, connection, 0, -1);
  
  return(result);
}

/*----------------------------------------------------------------------+*/
int signal_remote_channel_object(SPC_Channel_Ptr channel,
				 int sig)
/*----------------------------------------------------------------------+*/
{
  int result;
  int seqno, retval;
  
  call_parent_method(channel, signal, (channel, sig), result);
  
  if(result==SPC_ERROR)
    return(SPC_ERROR);
  
  if (channel->connection->protocol_version >= 2) {
      XeString signame = XeSignalToName( sig );
      
      if (!signame)
      {
	  SPC_Error(SPC_Bad_Signal_Value, sig);
	  return (SPC_ERROR);
      }
      seqno=SPC_Write_Protocol_Request(channel->connection, channel,
				       APPLICATION_SIGNAL, signame);
  }
  else
      seqno=SPC_Write_Protocol_Request(channel->connection, channel,
				       APPLICATION_SIGNAL, sig);

  retval=SPC_Waitfor_Reply(channel->connection, channel, seqno);

  return(retval);
}

/*----------------------------------------------------------------------+*/
int attach_remote_channel_object(SPC_Channel_Ptr channel,
				 int pid)
/*----------------------------------------------------------------------+*/
{
  int result;
  int seqno, retval;
  
  call_parent_method(channel, attach, (channel, pid), result);
  
  if(result==SPC_ERROR)
    return(SPC_ERROR);
  
  seqno=SPC_Write_Protocol_Request(channel->connection, channel,
				   CHANNEL_ATTACH, pid);
  retval=SPC_Waitfor_Reply(channel->connection, channel, seqno);
  if(retval==SPC_ERROR)
    return(SPC_ERROR);

  SPC_Flush_Queued_Data(channel);
  
  if(!mempf0(channel, pre_fork))
    return(SPC_ERROR);
  channel->pid = pid;

  return(TRUE);
}

/*----------------------------------------------------------------------+*/
int
remove_logfile_remote_channel_object(SPC_Channel_Ptr channel)
/*----------------------------------------------------------------------+*/
{
  int result;
  int seqno, retval;
  
  call_parent_method(channel, remove_logfile, (channel), result);
  
  if(result==SPC_ERROR)
    return(SPC_ERROR);
  
  seqno=SPC_Write_Protocol_Request(channel->connection, channel, DELETE_LOGFILE);
  retval=SPC_Waitfor_Reply(channel->connection, channel, seqno);
  if(retval==SPC_ERROR)
    return(SPC_ERROR);
  return(retval);
}

/*----------------------------------------------------------------------+*/
int add_input_remote_channel_object(SPC_Channel_Ptr channel,
				    SbInputHandlerProc handler, 
				    void *data)
/*----------------------------------------------------------------------+*/
{
  int result;
  SPC_Connection_Ptr conn=channel->connection;
  
  call_parent_method(channel, add_input, (channel, handler, data), result);

  if(result==SPC_ERROR)
    return(SPC_ERROR);
  
  if(conn->termination_id != -1)
    /* We already have a handler, don't need another one */
    return(TRUE);

  SPC_XtAddInput(channel,
		 &conn->termination_id,
		 conn->sid,
		 channel->class_ptr->input,
		 SPC_Input);
  return(TRUE);
  
}

/*----------------------------------------------------------------------+*/
int
SPC_Debug_Mode(SPC_Channel_Ptr channel,
	       XeString file)
/*----------------------------------------------------------------------+*/
{

  int retval, seqno;
  
  if(!channel->connection)
    return(SPC_ERROR);

  retval=SPC_Write_Protocol_Request(channel->connection, channel,
				    SERVER_DEBUG, file);
  seqno=SPC_Waitfor_Reply(channel->connection, channel, retval);

  if(retval==SPC_ERROR)
    return(SPC_ERROR);
  return(seqno);

}

int send_eof_remote_channel_object(SPC_Channel_Ptr channel)
{
  int retval, seqno;

  if(!channel->connection)
    return(SPC_ERROR);
  
  retval =
    SPC_Write_Protocol_Request(channel->connection, channel, CHANNEL_SEND_EOF);
  seqno = SPC_Waitfor_Reply(channel->connection, channel, retval);

  if(retval==SPC_ERROR)
    return(SPC_ERROR);

  return(seqno);

}

int set_termio_remote_channel_object(SPC_Channel_Ptr channel,
				     int connector,
				     int side,
				     struct termios *termios)
{
  int retval, seqno;

  if(!channel->connection)
    return(SPC_ERROR);

  retval =
    SPC_Write_Protocol_Request(channel->connection, channel,
			       CHANNEL_TERMIOS,
			       connector, side, termios);
  seqno = SPC_Waitfor_Reply(channel->connection, channel, retval);

  if(retval==SPC_ERROR)
    return(SPC_ERROR);

  return(seqno);

}
