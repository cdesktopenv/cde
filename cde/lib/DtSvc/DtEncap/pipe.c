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
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*
 * File:         pipe.c $XConsortium: pipe.c /main/4 1996/04/21 19:10:09 drk $
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
#include <errno.h>

#include <SPC/spcP.h>

/*----------------------------------------------------------------------+*/
void pipe_channel_class_init(object_clasp t)
/*----------------------------------------------------------------------+*/
{
  pipe_channel_clasp c = (pipe_channel_clasp) t;
  
  c->new_obj    = alloc_channel_object;
  
  c->open       = open_pipe_channel_object;
  c->close      = close_local_channel_object;
  c->read       = read_pipe_channel_object;
  c->write      = write_local_channel_object;
  c->reset      = reset_pipe_channel_object;
  c->pre_fork   = pre_fork_pipe_channel_object;
  c->post_fork  = post_fork_pipe_channel_object;
  c->exec_proc  = exec_proc_local_channel_object;
  c->signal     = signal_local_channel_object;
  c->wait_for_termination = local_channel_object_wait_for_termination;
  c->attach     = attach_pipe_channel_object;
  c->add_input  = add_input_pipe_channel_object;
  c->input      = local_channel_object_input_handler;
  c->remove_logfile = remove_logfile_local_channel_object;

  /* New B.00 methods */

  c->send_eof = local_channel_object_send_eof;
  c->set_termio = set_termio_noio_channel_object;
}

static struct pipe_channel_class pipe_channel_class_struct = {
  (channel_clasp) &channel_class, /* base class pointer */
  "pipe_channel",           /* class name */
  pipe_channel_class_init,  /* class initialize function */
  sizeof(SPC_Channel),      /* size */
  0
  };

pipe_channel_clasp pipe_channel_class = &pipe_channel_class_struct;

/*----------------------------------------------------------------------+*/
Wire *getpipe(Wire *prevwire)
/*----------------------------------------------------------------------+*/
{
  Wire *wire_ptr=get_new_wire();

  if(!wire_ptr)
    return(SPC_ERROR);

  wire_ptr->next=prevwire;
  /* Get file descriptors for pipe */
  if (pipe(wire_ptr->fd) < OK) {
    SPC_Error(SPC_No_Pipe);
    XeFree(wire_ptr);
    return(SPC_ERROR);
  }
  return(wire_ptr);
}

/*
 ***
 *** Method definitions for pipe channel objects
 ***
*/

/*
 * This routine handles initialization for pipe channels
 */

/*----------------------------------------------------------------------+*/
SPC_Channel_Ptr open_pipe_channel_object(SPC_Channel_Ptr channel,
					 int iomode,
					 XeString hostname)
/*----------------------------------------------------------------------+*/
{

  Wire *tmpwire=NULL;
  SPC_Channel_Ptr result;
  
  call_parent_method(channel, open, (channel, iomode, hostname), result);

  if(result==SPC_ERROR)
    return(SPC_ERROR);

  if (IS_SPCIO_STDIN(iomode)) {
    tmpwire=channel->wires[STDIN]=getpipe(NULL);
    if(!tmpwire)
      return(SPC_ERROR);
  }
  
  if (IS_SPCIO_STDOUT(iomode)) {
    tmpwire=channel->wires[STDOUT]=getpipe(tmpwire);
    if(!tmpwire)
      return(SPC_ERROR);
  }

  if(IS_SPCIO_STDERR(iomode)) {
    if (!tmpwire ||
	IS_SPCIO_SEPARATE(iomode)) {
      tmpwire=channel->wires[STDERR]=getpipe(tmpwire);
      if(!tmpwire)
	return(SPC_ERROR);
    } else {
      channel->wires[STDERR]=channel->wires[STDOUT];
    }
  }
  
  channel->file_descs[STDIN] =(channel->wires[STDIN]) ->fd[WRITE_SIDE];
  channel->file_descs[STDOUT]=(channel->wires[STDOUT])->fd[READ_SIDE];
  channel->file_descs[STDERR]=(channel->wires[STDERR])->fd[READ_SIDE];

  channel->wire_list=tmpwire;
  
  return(channel);
}

/*----------------------------------------------------------------------+*/
int read_pipe_channel_object(SPC_Channel_Ptr channel,
			     int connector,           /* STDOUT or STDERR */
			     XeString buffer,
			     int nbytes)
/*----------------------------------------------------------------------+*/
{
  int result;

  call_parent_method(channel,
		     read,
		     (channel, connector, buffer, nbytes),
		     result);

  if(result==Undefined)
    return(Undefined);

  do {
    result = read(channel->file_descs[connector], buffer, nbytes);
  } while (result<0 && errno == EINTR);

  if(result == 0) {

    SPC_XtRemoveInput(&channel->wires[connector]->read_toolkit_id, SPC_Input);
    SPC_Change_State(channel, connector, 0, -1);

  }

  return(result);
}

/*----------------------------------------------------------------------+*/
int pre_fork_pipe_channel_object(SPC_Channel_Ptr channel)
/*----------------------------------------------------------------------+*/
{

  int result;
  
  call_parent_method(channel, pre_fork, (channel), result);
  
  if(result==SPC_ERROR)
    return(SPC_ERROR);

  return(TRUE);
}

/*----------------------------------------------------------------------+*/
int post_fork_pipe_channel_object(SPC_Channel_Ptr channel,
				  int parentp)
/*----------------------------------------------------------------------+*/
{
  int result;
  
  call_parent_method(channel, post_fork, (channel, parentp), result);

  if(result==SPC_ERROR)
    return(SPC_ERROR);

  
  if (parentp) {		/* Master process */
    
    spc_close(channel->wires[STDIN]->fd[READ_SIDE]);
    spc_close(channel->wires[STDOUT]->fd[WRITE_SIDE]);
    spc_close(channel->wires[STDERR]->fd[WRITE_SIDE]);

    channel->wires[STDIN]->fd[READ_SIDE]   = (-1);
    channel->wires[STDOUT]->fd[WRITE_SIDE] = (-1);
    channel->wires[STDERR]->fd[WRITE_SIDE] = (-1);

    return(TRUE);
  }
  else {			/* Slave process */
    
    /* Close the "other" side of the pipes */
    
    spc_close(channel->wires[STDIN]->fd[WRITE_SIDE]);
    spc_close(channel->wires[STDOUT]->fd[READ_SIDE]);
    spc_close(channel->wires[STDERR]->fd[READ_SIDE]);

    /* Dup the file descriptors to fd's 3, 4, 5.
       spc_dup2 is used to make sure these guys are hooked to something
       (/dev/null if necessary).  We do this step here just in case any
       of the source file descriptors are 0, 1, or 2. */
    
    spc_dup2(channel->wires[STDIN]->fd[READ_SIDE], 3);
    spc_dup2(channel->wires[STDOUT]->fd[WRITE_SIDE], 4);
    spc_dup2(channel->wires[STDERR]->fd[WRITE_SIDE], 5);

    /* Go to STDIN, STDOUT, STDERR */
    
    spc_dup2(3, STDIN);
    spc_dup2(4, STDOUT);
    spc_dup2(5, STDERR);
    
    /* Close any other open file descriptors in the child */
    SPC_Close_Unused();

    return(TRUE);
  }
}

/*----------------------------------------------------------------------+*/
int reset_pipe_channel_object(SPC_Channel_Ptr channel)
/*----------------------------------------------------------------------+*/
{
  int result;  
  int iomode=channel->IOMode;
  Wire *wirelist;
  
  call_parent_method(channel, reset, (channel), result);

  if(result==SPC_ERROR)
    return(SPC_ERROR);

  /* Close the file descriptors */
  
  for(wirelist=channel->wire_list; wirelist; wirelist=wirelist->next) {
    SPC_XtRemoveInput(&wirelist->read_toolkit_id, SPC_Input);
    close(wirelist->fd[READ_SIDE]);
    close(wirelist->fd[WRITE_SIDE]);
    wirelist->flags &= ~SPCIO_DATA;
  }

  /* Allocate new file descriptors */
  
  for(wirelist=channel->wire_list; wirelist; wirelist=wirelist->next) {
    if(pipe(wirelist->fd) < 0) {
      SPC_Error(SPC_No_Pipe);
      return(SPC_ERROR);
    }
  }

  /* set the file_descs array to the new file descriptors & set up
     the new read mask */
  
  channel->file_descs[STDIN]  = (channel->wires[STDIN])->fd[WRITE_SIDE];
  channel->file_descs[STDOUT] = (channel->wires[STDOUT])->fd[READ_SIDE];
  channel->file_descs[STDERR] = (channel->wires[STDERR])->fd[READ_SIDE];

  XeSPCAddInput(channel, NULL, NULL);
  
  return(TRUE);
  
}

/*----------------------------------------------------------------------+*/
int attach_pipe_channel_object(SPC_Channel_Ptr UNUSED_PARM(channel), 
			       int 	       UNUSED_PARM(foo))
/*----------------------------------------------------------------------+*/
{
  /* It is an error to try to attach to a pipe channel... */
  SPC_Error(SPC_Bad_Operation);
  return(SPC_ERROR);
}

/*----------------------------------------------------------------------+*/
int add_input_pipe_channel_object(SPC_Channel_Ptr channel,
				  SbInputHandlerProc handler, 
				  void *data)
/*----------------------------------------------------------------------+*/
{
  int result, fd;
  Wire *wirelist, *stdinwire;
  
  call_parent_method(channel, add_input, (channel, handler, data), result);

  if(result==SPC_ERROR)
    return(SPC_ERROR);

  stdinwire=channel->wires[STDIN];
  
  for(wirelist=channel->wire_list; wirelist; wirelist=wirelist->next) {
    
    if(wirelist == stdinwire)
      continue;
    
    if(wirelist->read_toolkit_id != -1)
      continue;
    
    fd=wirelist->fd[READ_SIDE];
    SPC_XtAddInput(channel,
		   &wirelist->read_toolkit_id,
		   fd,
		   channel->class_ptr->input,
		   SPC_Input);

  }
  
  return(TRUE);
  
}
