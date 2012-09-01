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
 * $TOG: spc-proto.c /main/12 1999/10/14 16:00:27 mgreess $
 * Language:     C
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1989,1993,1994,1996 Hewlett-Packard Company.
 * (c) Copyright 1993,1994,1996 International Business Machines Corp.
 * (c) Copyright 1993,1994,1996 Sun Microsystems, Inc.
 * (c) Copyright 1993,1994,1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

#define __need_timeval
#define __need_all_errors

#include <bms/sbport.h> /* NOTE: sbport.h must be the first include. */
#include <errno.h>
#include <stdarg.h>
#include <sys/utsname.h>
#include <limits.h>

#define X_INCLUDE_PWD_H
#define XOS_USE_XT_LOCKING
#include <X11/Xos_r.h>

#include <SPC/spcP.h>

#include <bms/MemoryMgr.h>

#include <SPC/spc-proto.h>

#include <Tt/tt_c.h>
#include "DtSvcLock.h"

/* Externals */
/*************/

extern struct termios *XeTermioStruct;    /* In pty.c */

extern XeString *environ;
extern SPC_Channel_Ptr spc_activation_list;

/* Gobals */
/**********/

/* FILE *SPC_Print_Protocol=NULL; -- now in bmsglob.c */

SPC_Connection_Ptr connection_list = NULL;
protocol_request_ptr free_protocol_requests = NULL;

/* Forwards */
/************/

static int SPC_Send_Termios(protocol_request_ptr prot_request);
static int SPC_Send_B00_Spawn(SPC_Connection_Ptr connection,
			      protocol_request_ptr prot,
			      char *path,
			      char *dir,
			      char **argv,
			      char **envp);

/*----------------------------------------------------------------------+*/
buffered_data_ptr SPC_New_Buffered_Data_Ptr(void)
/*----------------------------------------------------------------------+*/
{
  buffered_data_ptr bdata;

  bdata=(buffered_data_ptr)XeMalloc(sizeof(buffered_data));
  bdata->len = bdata->offset = 0;
  return(bdata);
}


/*----------------------------------------------------------------------+*/
void SPC_Reset_Protocol_Ptr (protocol_request_ptr prot, 
			     SPC_Channel_Ptr channel, 
			     XeChar req, 
			     int len)
/*----------------------------------------------------------------------+*/
{
  buffered_data_ptr  dptr=prot->dataptr;
  
  dptr->len          = len;
  dptr->offset       = 0;
  prot->seqno        = 0;
  prot->request_type = req;
  prot->channel      = channel;
  prot->next         = NULL;
  memset(dptr->data, (XeChar)' ', REQUEST_HEADER_LENGTH);
}

/*----------------------------------------------------------------------+*/
protocol_request_ptr SPC_New_Protocol_Ptr (SPC_Channel_Ptr channel, 
					   XeChar req, 
					   int len)
/*----------------------------------------------------------------------+*/
{
  protocol_request_ptr prot;

  _DtSvcProcessLock();
  if(free_protocol_requests) {
    prot = free_protocol_requests;
    free_protocol_requests = free_protocol_requests->next;
  } else {
    prot = (protocol_request_ptr)XeMalloc(sizeof(protocol_request));
    prot->dataptr = SPC_New_Buffered_Data_Ptr();
  }
  SPC_Reset_Protocol_Ptr(prot, channel, req, len);
  _DtSvcProcessUnlock();
  return(prot);
}

/*----------------------------------------------------------------------+*/
void SPC_Free_Protocol_Ptr(protocol_request_ptr prot)
/*----------------------------------------------------------------------+*/
{
  _DtSvcProcessLock();
  prot->next = free_protocol_requests;
  free_protocol_requests = prot;
  _DtSvcProcessUnlock();
}


/*----------------------------------------------------------------------+*/
SPC_Channel_Ptr SPC_Lookup_Channel(int cid,
				   SPC_Connection_Ptr connection)
/*----------------------------------------------------------------------+*/
{
  
  SPC_Channel_Ptr spc;

  if(!cid)
    return(NULL);
  
  _DtSvcProcessLock();
  for(spc=spc_activation_list; spc; spc=spc->next)
    /* This test is here because:
       a. Only remote channels have cid's
       b. It is possible for multiple remote servers to have
          the same cid (which is simply the address of the channel),
	  so we need to distinguish among remote channels, but
       c. channels on the remote daemon have cid's, but a null connection.
       */
    if((spc->cid == cid) &&
       (!spc->connection ||
	(spc->connection == connection))) {
	_DtSvcProcessUnlock();
	return(spc);
    }
  
  _DtSvcProcessUnlock();
  return(NULL);
}

/*
 * Connection management routines
 */


/*----------------------------------------------------------------------+*/
SPC_Connection_Ptr SPC_Alloc_Connection(void)
/*----------------------------------------------------------------------+*/
{
  SPC_Connection_Ptr conn;

  _DtSvcProcessLock();
  conn=(SPC_Connection_Ptr) XeMalloc(sizeof(SPC_Connection));
  /* Zero the connection */
  memset(conn, 0, sizeof(SPC_Connection));
  conn->queued_remote_data = Xe_make_queue(FALSE);
  conn->termination_id = (-1);
  /* Init the socket id to "-1" because "0" is a valid file descriptor. */
  conn->sid = (-1);
  _DtSvcProcessUnlock();
  return(conn);
}


/*----------------------------------------------------------------------+*/
SPC_Connection_Ptr SPC_Lookup_Connection(XeString hostname)
/*----------------------------------------------------------------------+*/
{
  /* Search for an existing connection to a server */
  SPC_Connection_Ptr conn;

  _DtSvcProcessLock();
  for (conn = connection_list; conn != NULL; conn = conn->next) {

    /* Look for a connection with the same hostname */
      if (!strcmp(conn->hostname, hostname)) {
	  _DtSvcProcessUnlock();
	  return(conn);
      }
  }
  
  _DtSvcProcessUnlock();
  return(FALSE);
}

/*----------------------------------------------------------------------+*/
SPC_Connection_Ptr SPC_Lookup_Connection_Fd(int fd)
/*----------------------------------------------------------------------+*/
{
  /* Search for an existing connection to a server, using fd (file descriptor)
   as a key */
  SPC_Connection_Ptr conn;

  _DtSvcProcessLock();
  for (conn = connection_list; conn != NULL; conn = conn->next) {

    /* Look for a connection with the same hostname */
      if (conn->sid==fd) {
	  _DtSvcProcessUnlock();
	  return(conn);
      }
  }
  
  _DtSvcProcessUnlock();
  return(FALSE);
}

/*----------------------------------------------------------------------+*/
SPC_Connection_Ptr SPC_Make_Connection(XeString hostname)
/*----------------------------------------------------------------------+*/
{
  /* Search for a connection to hostname, create one if none exists */
  
  SPC_Connection_Ptr conn;

  /* Searching for connected host maintains only one connection per remote */

  if(hostname && (conn=SPC_Lookup_Connection(hostname)))
    return(conn);
  
  /* Not found, so make one */
  
  if((conn = SPC_Alloc_Connection())==SPC_ERROR)
    return(SPC_ERROR);

  if(hostname)
    strcpy(conn->hostname, hostname);
  
  SPC_Add_Connection(conn);
  return(conn);
}

/*----------------------------------------------------------------------+*/
void SPC_Add_Connection(SPC_Connection_Ptr connection)
/*----------------------------------------------------------------------+*/
{
  
  /* Add a connection to the connection_list */
  _DtSvcProcessLock();
  connection->next = connection_list;
  connection_list = connection;
  _DtSvcProcessUnlock();
}

/*----------------------------------------------------------------------+*/
void SPC_Close_Connection(SPC_Connection_Ptr connection)
/*----------------------------------------------------------------------+*/
{
  SPC_Channel_Ptr channel;
  SPC_Channel_Ptr next;
  SPC_Connection_Ptr trail, ptr;

  /* We have to be careful here.  SPC_Input_Handler may call the users
     termination handler, which in turn might close the channel, which
     may deallocate the channel.  Therefore, we grab the next channel
     from the list while we are still alive. */

  _DtSvcProcessLock();
  channel=spc_activation_list;
  connection->connected = FALSE;
  
  while(channel) {
    next=channel->next;
    if(channel->connection == connection) {
      if(!IS_SPCIO_DELAY_CLOSE(channel->IOMode))
	SPC_Channel_Terminated(channel);
      channel->connection = NULL;
    }
    channel=next;
  }
  
  SPC_XtRemoveInput(&connection->termination_id, SPC_Terminator);

  spc_close(connection->sid);
  connection->sid = (-1);

  if (connection->hostinfo)
      XeFree(connection->hostinfo);

  /* Remove the connection from the connection list */

  if(connection_list == connection)
    connection_list = connection->next;
  else {
    trail = connection_list;
    while(trail) {
      ptr = trail->next;
      if(ptr == connection) {
	trail->next = ptr->next;
	break;
      }
      trail=ptr;
    }
    if(!trail) {
      /* Here if no such connection found. */
    }
  }

  free((char *)connection);
  _DtSvcProcessUnlock();
}

/*
 **
 ** Read the specified number of characters, or die trying.
 **
*/

/*----------------------------------------------------------------------+*/
int SPC_Read_Chars(SPC_Connection_Ptr   connection,
		   int 			request_len,
		   XeString 		charptr)
/*----------------------------------------------------------------------+*/
{

  int numchars, numread;
  int numtoread=request_len;
  int sid=connection->sid;
  
  numread=0;

  while(numread<request_len) {
    do
      numchars=read(sid, charptr, numtoread);
    while(numchars == ERROR && errno == EINTR);
    if(numchars == ERROR) {
      XeString connection_hostname = CONNECTION_HOSTNAME(connection);
      if(errno == ECONNRESET)
	SPC_Error(SPC_Connection_Reset, connection_hostname);
      else
	SPC_Error(SPC_Reading, connection_hostname);
      XeFree(connection_hostname);
      return(SPC_ERROR);
    }
    if(numchars == 0) {
      XeString connection_hostname = CONNECTION_HOSTNAME(connection);
      SPC_Error(SPC_Connection_EOF, connection_hostname);
      XeFree(connection_hostname);
      return(SPC_ERROR);    /* Bad news, EOF on incoming channel */
    }
    charptr += numchars;
    numread += numchars;
    numtoread -= numchars;
  }
  *charptr=(XeChar)'\0';
  return(numread);
}

/* Write len chars, or die trying */

/*----------------------------------------------------------------------+*/
int SPC_Write_Chars(int 	fd,
		    XeString 	charptr,
		    int 	request_len)
/*----------------------------------------------------------------------+*/
{
  int numchars, numwritten;
  int numtowrite=request_len;
  
  numwritten=0;
  
  while(numwritten<request_len) {
    do
      numchars=write(fd, charptr, numtowrite);
    while(numchars == ERROR && errno == EINTR);

#ifdef DEBUG
    _DtSvcProcessLock();
    if(SPC_Print_Protocol)
      fprintf(SPC_Print_Protocol,
	      "SPC_Write_Chars -- wrote: %d of %d, expected: %d, errno: %d\n",
	      numchars, request_len, numtowrite, errno);
    _DtSvcProcessUnlock();
#endif    
       
    if(numchars == ERROR)
      return(ERROR);

    charptr    += numchars;
    numwritten += numchars;
    numtowrite -= numchars;
  }
  return(numwritten);
}
  

/*
 **
 ** Read a single protocol request from the passed channel.
 **
*/

/*----------------------------------------------------------------------+*/
protocol_request_ptr SPC_Read_Protocol(SPC_Connection_Ptr connection)
/*----------------------------------------------------------------------+*/
{
  int                channel_id;
  protocol_request_ptr prot;
  buffered_data_ptr        dptr;
  int len;

  if(!connection->connected)
    return(SPC_ERROR);
  
  if((prot=SPC_New_Protocol_Ptr(NULL, 0, 0))==SPC_ERROR) {
    SPC_Close_Connection(connection);
    return(SPC_ERROR);
  }
  
  /* read header */

  dptr=prot->dataptr;

  len=SPC_Read_Chars(connection, REQUEST_HEADER_LENGTH, dptr->data);
  if(len != REQUEST_HEADER_LENGTH) {
    SPC_Close_Connection(connection);
    SPC_Free_Protocol_Ptr(prot);
    return(SPC_ERROR);
  }
  
  /* we have the header.  Parse out the fields */
  
  READ_HEADER(dptr,
	      &channel_id, &prot->request_type, &dptr->len, &prot->seqno);
  prot->channel=SPC_Lookup_Channel(channel_id, connection);
  

  /* JET - 11/12/2001 - correct an exploitable buffer overrun where the user */
  /* can supply a data len that is larger than the available buffer */
  /* MAXREQLEN */
  /* CERT - VU#172583 */

  if (dptr->len >= MAXREQLEN)
    {				/* we have a problem.  Initiate DefCon 1 */
				/* and launch our missiles. */
      XeString connection_hostname = CONNECTION_HOSTNAME(connection);

      SPC_Error(SPC_Buffer_Overflow, connection_hostname);
      XeFree(connection_hostname);
      SPC_Close_Connection(connection);
      SPC_Free_Protocol_Ptr(prot);
      return(SPC_ERROR);
    }

  /* read header */
  
  len=SPC_Read_Chars(connection, dptr->len, dptr->data+REQUEST_HEADER_LENGTH);
  if(len != dptr->len) {
    SPC_Close_Connection(connection);
    SPC_Free_Protocol_Ptr(prot);
    return(SPC_ERROR);
  }

  dptr->offset=REQUEST_HEADER_LENGTH;
  return(prot);
  
}

/*
 **
 ** Filter the connection for the desired type of protocol request.
 ** If there is a protocol request of the desired type already queued,
 ** return it.  If not, read a new one.  If we read requests destined
 ** for another channel, or for our channel but not the correct
 ** request type, queue it up.  If the deletep flag is TRUE, remove it
 ** from the queue.
 **
*/

/*----------------------------------------------------------------------+*/
protocol_request_ptr  SPC_Filter_Connection(SPC_Connection_Ptr connection,
					    SPC_Channel_Ptr channel,
					    int reqtype, 
					    int deletep)
/*----------------------------------------------------------------------+*/
{
  SPC_Connection_Ptr connptr=NULL;
  SPC_Channel_Ptr conn_channel;
  XeQueue   tmpqueue;
  protocol_request_ptr retval;
  int protreqtype;

  /* check if there are any queued prot. requests.  If so,
     check for their having the type we want */
  
  if(channel && (tmpqueue=channel->queued_remote_data)) {
    tmpqueue=channel->queued_remote_data;
    {
      Xe_for_queue(protocol_request_ptr, retval, tmpqueue) {
	/* found a queued packet.  Is it what we are looking for? */
	if(retval->request_type == reqtype) {
	  /* yes. */
	  if(deletep)
	    Xe_delete_queue_element(channel->queued_remote_data, retval);
	  return(retval);
	}
      }
    }
  }
  
  /* No queued elements.  Read until we get the reply we
     are looking for */
  while(TRUE) {
    if((retval=SPC_Read_Protocol(connection))==SPC_ERROR)
      return(SPC_ERROR);

    protreqtype=retval->request_type;
    conn_channel=retval->channel;

    if(protreqtype == ABORT) {
      XeString connection_hostname = CONNECTION_HOSTNAME(connection);
      SPC_Error(SPC_Protocol_Abort, connection_hostname);
      XeFree (connection_hostname);
      return(SPC_ERROR);
    }

    _DtSvcProcessLock();
    if(protreqtype == SERVER_ERROR) {
      READ_ERROR(retval->dataptr, XeSPCErrorNumber);
      SPC_Error(XeSPCErrorNumber, XeString_NULL, 0);
      _DtSvcProcessUnlock();
      return(SPC_ERROR);
    }
    _DtSvcProcessUnlock();

    if(!conn_channel)
      return(retval);

    if((conn_channel == channel) && (protreqtype == reqtype)) {
      /* We found one that matches.  Check if we need to queue it up. */
      if(!deletep)
	Xe_push_queue(conn_channel->queued_remote_data, retval);
      return(retval);
    }
    
    /* No match.  Queue it up */
    if(IS_SPCIO_SYNC_TERM(channel->IOMode))
      Xe_push_queue(connection->queued_remote_data, retval);
    else
      Xe_push_queue(conn_channel->queued_remote_data, retval);
    
  }
}

/*----------------------------------------------------------------------+*/
void SPC_Flush_Queued_Data(SPC_Channel_Ptr channel)
/*----------------------------------------------------------------------+*/
{
  XeQueue tmpqueue;
  protocol_request_ptr prot;
  
  if(tmpqueue=channel->queued_remote_data) {
    while(prot=(protocol_request_ptr)Xe_pop_queue(tmpqueue))
      SPC_Free_Protocol_Ptr(prot);
  }

  if(channel->connection && (tmpqueue=channel->connection->queued_remote_data))
    {
      Xe_for_queue(protocol_request_ptr, prot, tmpqueue) {
	if(prot->channel == channel) {
	  Xe_delete_queue_element(tmpqueue, prot);
	  SPC_Free_Protocol_Ptr(prot);
	}
      }
    }
}

/*----------------------------------------------------------------------+*/
int SPC_Read_Remote_Data(SPC_Channel_Ptr channel,
			 int connector,
			 XeString client_buffer,
			 int nbytes)
/*----------------------------------------------------------------------+*/
{
  int ret_len;

  SPC_Connection_Ptr connection=channel->connection;
  protocol_request_ptr prot;
  buffered_data_ptr    pdata;
  int req_type=CONNECTOR_TO_PROT(connector);

  prot=SPC_Filter_Connection(connection, channel, req_type, FALSE);
  if(prot==SPC_ERROR)
    return(SPC_ERROR);
  
  pdata=prot->dataptr;
  
  ret_len = min(nbytes, pdata->len);
  
  memcpy(client_buffer, pdata->data+pdata->offset, ret_len);
  pdata->offset += ret_len;
  pdata->len    -= ret_len;
  if(pdata->len == 0) {
    SPC_Filter_Connection(connection, channel, req_type, TRUE);
    SPC_Free_Protocol_Ptr(prot);
  }

  return(ret_len);
}

/* Dump out a protocol request */

/*----------------------------------------------------------------------+*/
int print_protocol_request(XeString name, protocol_request_ptr proto)
/*----------------------------------------------------------------------+*/
{
  buffered_data_ptr dptr = proto->dataptr;

  _DtSvcProcessLock();
  if(!SPC_Print_Protocol) {
    _DtSvcProcessUnlock();
    return FALSE;
  }

  dptr->data[dptr->offset+dptr->len]=0;
  
  fprintf(SPC_Print_Protocol,
	  "%s channel: %x, request: %d, length: %d, seq: %d data: %s\n",
	  name, proto->channel, proto->request_type, dptr->len, proto->seqno,
	  dptr->data+dptr->offset);

  fflush(SPC_Print_Protocol);

  _DtSvcProcessUnlock();
  return TRUE;
}
  
/*
 * Write a protocol request to the given channel
 *
*/

int current_sequence_number=1;

/*----------------------------------------------------------------------+*/
int SPC_Write_Protocol_Request (SPC_Connection_Ptr connection, 
				SPC_Channel_Ptr channel, 
				int request, 
				...)
/*----------------------------------------------------------------------+*/
{
  va_list 		ap;
  protocol_request_ptr 	prot_request;
  buffered_data_ptr    	pdata;
  int 			reply_expected;
  XeString              prot_name=NULL;
  
  prot_request=SPC_New_Protocol_Ptr(channel, request, 0);
  pdata=prot_request->dataptr;
  _DtSvcProcessLock();
  prot_request->seqno = current_sequence_number++;
  _DtSvcProcessUnlock();

  /* We are overloading the "channel" field.  We put the cid rather  */
  /* than the actual channel pointer in when we pass it to the other */
  /* side of the connection.					     */
  prot_request->channel=(SPC_Channel_Ptr)(channel ? channel->cid : 0);
  
  switch (request) {

  case ABORT:
    pdata->len=WRITE_ABORT(pdata, 0);
    prot_name=(XeString)"  <-- ABORT";
    break;
    
 case REGISTER:
    {
    XeString username;
    XeString passwd;
    XeString proto_ver;
    XeString hostinfo;

    va_start(ap, request);
    username=va_arg(ap, XeString );    
    passwd=va_arg(ap, XeString );
    proto_ver=va_arg(ap, XeString );
    hostinfo=va_arg(ap, XeString );
    va_end(ap);
    pdata->len=WRITE_REGISTER(pdata, username, passwd, proto_ver, hostinfo);
    prot_name=(XeString)"  <-- REGISTER";
    break;
    }
    
    
  case UNREGISTER:
    prot_name=(XeString)"  <-- UNREGISTER";
    break;
    
  case CHANNEL_OPEN:
    va_start(ap, request);
    pdata->len=WRITE_OPEN(pdata, va_arg(ap, int));
    va_end(ap);
    prot_name=(XeString)"  <-- CHANNEL_OPEN";
    break;

  case CHANNEL_CLOSE:
    /* This is correct.  This protocol request takes no args */
    prot_name=(XeString)"  <-- CHANNEL_CLOSE";
    break;
    
  case CHANNEL_RESET:
    /* This one, either */
    prot_name=(XeString)"  <-- CHANNEL_RESET";
    break;
    
  case CHANNEL_ATTACH:
    prot_name=(XeString)"  <-- CHANNEL_ATTACH";
    va_start(ap, request);
    pdata->len=WRITE_ATTACH(pdata, va_arg(ap, int));
    va_end(ap);
    break;
    
  case APPLICATION_SPAWN:
    {
    XeString path;
    XeString dir;
    XeString *argv;
    XeString *envp;
    
    va_start(ap, request);

    /* It is left as an exercise to the reader to figure out
       what would happen if we didn't use these temp. variables
       and instead used the va_arg macros directly in the
       WRITE_APPLICATION_SPAWN... */
    
    path=va_arg(ap, XeString );
    dir =va_arg(ap, XeString );
    argv=va_arg(ap, XeString *);
    envp=va_arg(ap, XeString *);
    va_end(ap);
    pdata->len=WRITE_APPLICATION_SPAWN(pdata, path, dir, argv, envp);
    if(pdata->len == SPC_ERROR)
      return(SPC_ERROR);
    prot_name=(XeString)"  <-- APPLICATION_SPAWN";
    break;
    }
    
  case APPLICATION_SIGNAL:
    {
	if (connection->protocol_version >= 2) 
	{
	    XeString signame;
	    
	    va_start(ap, request);
	    signame = va_arg(ap, XeString);
	    va_end(ap);
	    
	    pdata->len=WRITE_STRING(pdata, signame);
	}
	else
	{
	    int      sig;
	    
	    va_start(ap, request);
	    sig = va_arg(ap, int);
	    va_end(ap);

	    pdata->len=WRITE_INT(pdata, sig);
	}
	
	prot_name=(XeString)"  <-- APPLICATION_SIGNAL";
	break;
    }
    
  case APPLICATION_DIED:
    va_start(ap, request);
    pdata->len=WRITE_APPLICATION_DIED(pdata, va_arg(ap, int));
    va_end(ap);
    prot_name=(XeString)"  <-- APPLICATION_DIED";
    break;

  case APPLICATION_DATA:
    prot_name=(XeString)"  <-- APPLICATION_DATA";
    
  case APPLICATION_STDOUT:
    if(!prot_name)
      prot_name=(XeString)"  <-- APPLICATION_STDOUT";
    
  case APPLICATION_STDERR:
    {
    int      buflen;
    XeString buffer;
    
    if(!prot_name)
      prot_name=(XeString)"  <-- APPLICATION_STDERR";
    
    va_start(ap, request);
    buffer=va_arg(ap, XeString );
    buflen=va_arg(ap, int);
    va_end(ap);

    pdata->len=WRITE_APP_DATA(pdata, buffer, buflen);

    break;
    }

  case SERVER_ERROR:
    va_start(ap, request);
    pdata->len=WRITE_ERROR(pdata, va_arg(ap, int));
    va_end(ap);
    prot_name=(XeString)"  <-- SERVER_ERROR";
    break;
    
  case REPLY:
    {
    int replyval, errval;
    
    va_start(ap, request);
    prot_request->seqno=va_arg(ap, int);
    replyval=va_arg(ap, int);
    errval=va_arg(ap, int);
    va_end(ap);
    pdata->len=WRITE_REPLY(pdata, replyval, errval);
    prot_name=(XeString)"  <-- REPLY";
    break;
    }

  case DEVICE_REPLY:
    { 
    XeString m0;
    XeString s0;
    XeString m1;
    XeString s1;
    XeString m2;
    XeString s2;

    va_start(ap, request);
    m0=va_arg(ap, XeString );
    s0=va_arg(ap, XeString );
    m1=va_arg(ap, XeString );
    s1=va_arg(ap, XeString );
    m2=va_arg(ap, XeString );
    s2=va_arg(ap, XeString );
    va_end(ap);
    pdata->len=WRITE_DEVICE_REPLY(pdata, m0, s0, m1, s1, m2, s2);
    prot_name=(XeString)"  <-- DEVICE_REPLY";
    break;
    }
    
  case QUERY_DEVICES:
    prot_name=(XeString)"  <-- QUERY_DEVICES";
    break;

 case LOGFILE_REPLY:
    {
    XeString fname;
    XeString proto_ver;
    XeString hostinfo;

    va_start(ap, request);
    fname=va_arg(ap, XeString );
    proto_ver=va_arg(ap, XeString );
    hostinfo=va_arg(ap, XeString );
    va_end(ap);
    pdata->len=WRITE_LOGFILE_REPLY(pdata, fname, proto_ver, hostinfo);
    prot_name=(XeString)"  <-- LOGFILE_REPLY";
    break;
    }
    
  case QUERY_LOGFILE:
    prot_name=(XeString)"  <-- QUERY_LOGFILE";
    break;

  case DELETE_LOGFILE:
    prot_name=(XeString)"  <-- DELETE_LOGFILE";
    break;

  case SERVER_DEBUG:
    va_start(ap, request);
    pdata->len=WRITE_DEBUG(pdata, va_arg(ap, XeString));
    va_end(ap);
    prot_name=(XeString)"  <-- SERVER_DEBUG";
    break;

  case ENVIRON_RESET:
    return(SPC_Send_Environ(connection, prot_request));

  /* We used to send the hp-ux version of a termio struct */
  /* This is non-portable, so we don't do it anymore.     */

  case RESET_TERMIOS:
    pdata->len=SPC_Send_Termios(prot_request);
    prot_name=(XeString)"  <-- RESET_TERMIOS";
    break;

  /* B.00 (protocol version 3) requests */
    
  case CHANNEL_SEND_EOF:

    if(connection->protocol_version < 3) {
      SPC_Error(SPC_Protocol_Version_Error,
		3, channel->connection->protocol_version);
      return(SPC_ERROR);
    }
  
    prot_name = (XeString)"  <-- CHANNEL_SEND_EOF";
    break;

  case CHANNEL_TERMIOS:

    if(connection->protocol_version < 3) {
      SPC_Error(SPC_Protocol_Version_Error,
		3, channel->connection->protocol_version);
      return(SPC_ERROR);
    }
  
    {
      int connector, side;
      struct termios *termios_ptr;
      char *buffer;

      va_start(ap, request);

      connector   = va_arg(ap, int);
      side        = va_arg(ap, int);
      termios_ptr = va_arg(ap, struct termios *);
      va_end(ap);

      buffer = SPC_Decode_Termios(termios_ptr);
      pdata->len=WRITE_TERMIOS(pdata, connector, side, buffer);
      prot_name=(XeString)"  <-- CHANNEL_TERMIOS";
      break;
    }

  case APP_B00_SPAWN:
    if(connection->protocol_version < 3) {
      SPC_Error(SPC_Protocol_Version_Error,
		3, channel->connection->protocol_version);
      return(SPC_ERROR);
    }

    {
    XeString path;
    XeString dir;
    XeString *argv;
    XeString *envp;
    int retval;
    
    va_start(ap, request);

    /* It is left as an exercise to the reader to figure out
       what would happen if we didn't use these temp. variables
       and instead used the va_arg macros directly in the
       WRITE_APPLICATION_SPAWN... */
    
    path=va_arg(ap, XeString );
    dir =va_arg(ap, XeString );
    argv=va_arg(ap, XeString *);
    envp=va_arg(ap, XeString *);
    va_end(ap);

    retval =
      SPC_Send_B00_Spawn(connection, prot_request, path, dir, argv, envp);
    return(retval);
    }
    
  }

  reply_expected=
    SPC_Write_Single_Prot_Request(connection, prot_name, prot_request);
    
  SPC_Free_Protocol_Ptr(prot_request);
  return(reply_expected);
}


/*----------------------------------------------------------------------+*/
int SPC_Write_Single_Prot_Request(SPC_Connection_Ptr connection,
				  XeString name,
				  protocol_request_ptr prot)
/*----------------------------------------------------------------------+*/
{
  int reply_expected, length;
  buffered_data_ptr   pdata=prot->dataptr;

  if(!connection->connected)
    return(SPC_ERROR);
  
  reply_expected=
    REPLY_EXPECTED(prot->request_type, prot->seqno);
  length=WRITE_HEADER(pdata, prot->channel,
		      prot->request_type,
		      pdata->len,
		      prot->seqno);
  pdata->data[length]=(XeChar)' ';
  
  length=pdata->len+REQUEST_HEADER_LENGTH;
  if(SPC_Write_Chars(connection->sid, pdata->data, length) == ERROR) {
    XeString connection_hostname = CONNECTION_HOSTNAME(connection);
    SPC_Close_Connection(connection);
    SPC_Error(SPC_Write_Prot, connection_hostname);
    XeFree(connection_hostname);
    reply_expected = (SPC_ERROR);
  }
  
  pdata->offset=REQUEST_HEADER_LENGTH;
  print_protocol_request(name, prot);

  return(reply_expected);
}

/*----------------------------------------------------------------------+*/
int SPC_Waitfor_Reply(SPC_Connection_Ptr connection,
		      SPC_Channel_Ptr channel,
		      int seqno)
/*----------------------------------------------------------------------+*/
{

  protocol_request_ptr prot;
  int retval, errval;
  int thisseq;
  
  if(seqno == NO_REPLY_VAL)
    return(TRUE);

  if(seqno==SPC_ERROR)
    return(SPC_ERROR);
  
  prot=SPC_Filter_Connection(connection, channel, REPLY, TRUE);

  if(prot==SPC_ERROR)
    return(SPC_ERROR);

  thisseq = prot->seqno;
  READ_REPLY(prot->dataptr, retval, errval);

  SPC_Free_Protocol_Ptr(prot);
  
  if(thisseq != seqno) {
    SPC_Error(SPC_Unexpected_Reply);
    return(SPC_ERROR);
  }
  
  if(retval<0) {
    errno=errval;
    retval = -retval;
    if(retval != SPC_Protocol_Abort) {
      if (retval == SPC_Cannot_Create_Netfilename)
	 SPC_Error(retval, channel->context_dir, connection->hostname);
      else if (retval == SPC_Cannot_Exec)
	 SPC_Error(retval, channel->path, 0);
      else if (retval == SPC_cannot_Chdir)
	 SPC_Error(retval, channel->context_dir, 0);
      else
	 SPC_Error(retval, XeString_NULL, 0);
    }
    return(SPC_ERROR);
  }
  return(retval);
}
      

/*----------------------------------------------------------------------+*/
int SPC_Dispatch_Protocol(protocol_request_ptr proto,
			  protocol_request_handler *table)
/*----------------------------------------------------------------------+*/
{
  int req_type;
  
  if(!proto)
    return(FALSE);
  req_type=proto->request_type;
  
  if(req_type<0 || req_type>NREQS)
    return(FALSE);

  return((* table[req_type])(proto));
}

/*----------------------------------------------------------------------+*/
int SPC_Write_Reply(SPC_Connection_Ptr conn,
		    protocol_request_ptr proto,
		    int retval,
                    int errval)
/*----------------------------------------------------------------------+*/
{
  _DtSvcProcessLock();
  if(retval==ERROR)
    retval=(-XeSPCErrorNumber);
  _DtSvcProcessUnlock();

  return(SPC_Write_Protocol_Request(conn, proto->channel, REPLY,
				    proto->seqno, retval, errval));
}

/*
 **
 ** Send the current environment out to the world.
 **
*/

#define valid_str(x) (((x) != NULL) && (*(x) != 0))
#define NULL_STR     "\001\001\001"
#define EMPTY_STR    "\001\002\003"

int SPC_Send_Multi_Packet(SPC_Connection_Ptr connection,
			  protocol_request_ptr prot,
			  char **str_vect,
			  int num_str,
			  int req,
			  XeString name,
			  int errid)
{
  int counter, tmp_len;
  int bytes_left, numbytes;
  XeString buf;
  int reply_seqno;
  int this_seqno;
  int valid_ep = 0;
  char *this_str;

  _DtSvcProcessLock();
  this_seqno=current_sequence_number;
  numbytes=WRITE_ENVIRON_RESET(prot->dataptr, num_str);
  bytes_left=SPC_BUFSIZ-numbytes;
  buf=(PDRP(prot->dataptr) + numbytes);
  reply_seqno=prot->seqno;
  
  for(counter=0; counter<num_str; counter++) {
    this_str = str_vect[counter];
    
    if(this_str == NULL)
      this_str = NULL_STR;
    if(*this_str == '\0')
      this_str = EMPTY_STR;
    
    tmp_len=strlen(this_str)+1;         /* Room for NULL char */
    if((bytes_left-tmp_len) < 1) {
      *buf='\0';
      prot->dataptr->len=numbytes+1;
      SPC_Write_Single_Prot_Request(connection, name, prot);
      SPC_Free_Protocol_Ptr(prot);
      prot=SPC_New_Protocol_Ptr(0, req, 0);
      prot->seqno=current_sequence_number++;
      numbytes=0;
      bytes_left=SPC_BUFSIZ;
      buf=(PDRP(prot->dataptr) + numbytes);
    }
    
    if(tmp_len>SPC_BUFSIZ-1) {
      SPC_Error(errid,
		this_str,
		SPC_BUFSIZ-1);
      SPC_Free_Protocol_Ptr(prot);
      if(this_seqno != current_sequence_number)
	SPC_Write_Protocol_Request(connection, NULL, ABORT);
      _DtSvcProcessUnlock();
      return(reply_seqno);
    }

    strncpy(buf, this_str, tmp_len);
    bytes_left -= tmp_len;
    numbytes   += tmp_len;
    buf        += tmp_len;
  }
  
  if(numbytes) {
    *buf='\0';
    prot->dataptr->len=numbytes+1;
    SPC_Write_Single_Prot_Request(connection, (XeString)"  <-- ENVIRON_RESET", prot);
    SPC_Free_Protocol_Ptr(prot);
  }

  _DtSvcProcessUnlock();
  return(reply_seqno);
  
}

char **SPC_Get_Multi_Packet(SPC_Connection_Ptr connection,
			    protocol_request_ptr prot,
			    char **out,
			    int *outlen,
			    int request,
			    XeString name)

{
  int num_vars, i, len;
  XeString bufptr;
  protocol_request_ptr localprot = NULL;

  print_protocol_request(name, prot);
  READ_ENVIRON_RESET(prot->dataptr, num_vars);
  bufptr=strchr(PDRP(prot->dataptr), Space)+1;
  if(out == NULL)
    out = (char **)malloc((num_vars+1) * sizeof(char **));
  
  for(i=0; i<num_vars; i++) {
    len=strlen(bufptr);
    if(len==0) {
      if(localprot)
	SPC_Free_Protocol_Ptr(localprot);
      prot=SPC_Filter_Connection(connection, NULL, request, TRUE);
      if(prot==SPC_ERROR)
	return(SPC_ERROR);
      print_protocol_request(name, prot);
      localprot=prot;
      bufptr=PDRP(prot->dataptr);
      len=strlen(bufptr);
    }
    if(strcmp(bufptr, NULL_STR) == 0)
      out[i] = NULL;
    else {
      if(strcmp(bufptr, EMPTY_STR) == 0)
	out[i] = strdup("");
      else
	out[i] = strdup(bufptr);
    }
    bufptr += (len+1);
  }
  if(localprot)
    SPC_Free_Protocol_Ptr(localprot);

  *outlen = num_vars;
  out[num_vars] = NULL;
  
  return(out);
}
 
/*----------------------------------------------------------------------+*/
int SPC_Send_Environ(SPC_Connection_Ptr connection,
		     protocol_request_ptr prot)
/*----------------------------------------------------------------------+*/
{

  int ep_count=0;
  int result;

  _DtSvcProcessLock();
  while(environ[ep_count])
    ep_count++;
  
  result = SPC_Send_Multi_Packet(connection, prot, environ, ep_count,
			       ENVIRON_RESET, "  <-- ENVIRON_RESET",
			       SPC_Env_Too_Big);
  _DtSvcProcessUnlock();
  return (result);
}

/*----------------------------------------------------------------------+*/
int sprint_counted_string(XeString buf,
			  int count,
			  XeString *vect,
			  int orig_limit)
/*----------------------------------------------------------------------+*/
{
  XeString bufptr=buf;
  int len;
  int i;
  int limit=orig_limit;
#define ERRBUFLEN 100
  char errbuf[ERRBUFLEN];

  len=sprintf_len(bufptr, (XeString)"%x ", count)+1;
  if(len>limit) {
    sprintf(errbuf, "(%d chars), max. length is %d", len, orig_limit);
    SPC_Error(SPC_Arg_Too_Long,
	      bufptr,
	      errbuf);
    return(SPC_ERROR);
  }
  bufptr += len;
  limit  -= len;
  
  for(i=0; i<count; (i++, vect++)) {
    if (*vect)
      len = strlen(*vect)+1;
    else
      len = 1;

    if(len>limit) {
      sprintf(errbuf, "(%d chars), max. length is %d", len, orig_limit);
      SPC_Error(SPC_Arg_Too_Long,
		*vect,
		errbuf);
      return(SPC_ERROR);
    }
    sprintf(bufptr, "%s", *vect ? *vect : "");
    bufptr += len;
    limit  -= len;
  }
  
  return(bufptr-buf);
}

/*----------------------------------------------------------------------+*/
XeString *sscan_counted_string(XeString buf,
  			       XeString *newbuf)
/*----------------------------------------------------------------------+*/
{
  
  int i, numstrings, len;
  XeString bufptr; 
  XeString *tmpptr;
  XeString *tmpidx;
  
  sscanf(buf, (XeString)"%x", &numstrings);
  len=strlen(buf)+1;
  bufptr=buf+len;
  
  tmpptr=(XeString*)XeMalloc((numstrings+1) * sizeof(XeString *));
  tmpidx=tmpptr;
  
  for(i=0; i<numstrings; (i++, tmpidx++)){
    len=strlen(bufptr)+1;  /* len is string SIZE (with room for NULL) */
    *tmpidx=(XeString)XeMalloc(len);
    strncpy(*tmpidx, bufptr, len);
    (*tmpidx)[len-1]='\0';
    bufptr+= len;
  }
  *tmpidx=NULL;
  if (newbuf)
    *newbuf=bufptr;
  return(tmpptr);
}

/*----------------------------------------------------------------------+*/
int 
sprint_application_data(XeString buf,
			XeString UNUSED_PARM(fmt),
			XeString path,
			XeString dir,
			XeString *argv,
			XeString *envp,
			int      UNUSED_PARM(chars_used))
/*----------------------------------------------------------------------+*/
{

  int av_count=0;
  int ep_count=0;
  int data_len=0;
  int tmp_len;
  int limit;
  
  if(argv)
    while(argv[av_count]) av_count++;
  
  if(envp)
    while(envp[ep_count]) ep_count++;

  limit=SPC_BUFSIZ;
    
  tmp_len = sprint_counted_string(buf, 1, &path, limit);
  if(tmp_len == SPC_ERROR)
    return(SPC_ERROR);
  limit    -= tmp_len;
  buf      += tmp_len;
  data_len += tmp_len;
  
  tmp_len = sprint_counted_string(buf, 1, &dir, limit);
  if(tmp_len == SPC_ERROR)
    return(SPC_ERROR);
  limit    -= tmp_len;
  buf      += tmp_len;
  data_len += tmp_len;
  
  tmp_len = sprint_counted_string(buf, av_count, argv, limit);
  if(tmp_len == SPC_ERROR)
    return(SPC_ERROR);
  limit    -= tmp_len;
  buf      += tmp_len;
  data_len += tmp_len;
  
  tmp_len = sprint_counted_string(buf, ep_count, envp, limit);
  if(tmp_len == SPC_ERROR)
    return(SPC_ERROR);
  limit    -= tmp_len;
  buf      += tmp_len;
  data_len += tmp_len;
  
  return(data_len);
}

/*----------------------------------------------------------------------+*/
int
sscan_application_data(XeString buf,
		       XeString UNUSED_PARM(fmt),
		       XeString *path,
		       XeString *dir,
		       XeString **argv,
		       XeString **envp,
		       int      UNUSED_PARM(offset))
/*----------------------------------------------------------------------+*/
{
  XeString bufptr;
  XeString *tmp_vect;
  
  bufptr=buf;

  tmp_vect=sscan_counted_string(bufptr, &bufptr);
  if(tmp_vect==SPC_ERROR)
    return(SPC_ERROR);
  *path = (*tmp_vect);
  
  tmp_vect=sscan_counted_string(bufptr, &bufptr);
  if(tmp_vect==SPC_ERROR)
    return(SPC_ERROR);
  *dir = (*tmp_vect);
  
  *argv=sscan_counted_string(bufptr, &bufptr);
  if(*argv==SPC_ERROR)
    return(SPC_ERROR);

  *envp=sscan_counted_string(bufptr, &bufptr);
  if(*envp==SPC_ERROR)
    return(SPC_ERROR);
}

/*----------------------------------------------------------------------+*/
int
sprint_device_data(XeString buf,
		   XeString m0, 
		   XeString s0, 
		   XeString m1, 
		   XeString s1, 
		   XeString m2, 
		   XeString s2)
/*----------------------------------------------------------------------+*/
{
  XeString args[6];
  int i;

  args[0]=m0;
  args[1]=s0;
  args[2]=m1;
  args[3]=s1;
  args[4]=m2;
  args[5]=s2;

  i=sprint_counted_string(buf, 6, args, SPC_BUFSIZ);
  if(i==SPC_ERROR)
    return(SPC_ERROR);
  return(i);
}

/*----------------------------------------------------------------------+*/
int
sscan_device_data(XeString buf,
		  XeString *m0, 
		  XeString *s0, 
		  XeString *m1, 
		  XeString *s1, 
		  XeString *m2, 
		  XeString *s2)
/*----------------------------------------------------------------------+*/
{
  XeString *args;

  args=sscan_counted_string(buf, NULL);

  if(args==SPC_ERROR)
    return(SPC_ERROR);
  
  *m0=args[0];
  *s0=args[1];
  *m1=args[2];
  *s1=args[3];
  *m2=args[4];
  *s2=args[5];

  free((char *)args);
  
  return(TRUE);
}

/*----------------------------------------------------------------------+*/
int
sprint_logfile_data(XeString buf,
		    XeString logfile,
		    XeString proto_ver,
		    XeString hostinfo)
/*----------------------------------------------------------------------+*/
{
  int i;
  XeString args[3];
  
  args[0]=logfile;
  args[1]=proto_ver;
  args[2]=hostinfo;
  
  i=sprint_counted_string(buf, 3, args, SPC_BUFSIZ);
  if(i==SPC_ERROR)
    return(SPC_ERROR);
  return(i);
}

/*----------------------------------------------------------------------+*/
int
sscan_logfile_data(XeString buf,
		   XeString *logfile,
		   XeString *proto_ver,
		   XeString *hostinfo)
/*----------------------------------------------------------------------+*/
{
  XeString *args;

  args=sscan_counted_string(buf, NULL);

  if(args==SPC_ERROR)
    return(SPC_ERROR);
  
  *logfile=args[0];
  /* args[1] and args[2] will only be around for protocol revision 2 or later */

  *proto_ver = *hostinfo = XeString_NULL;

  if (args[1]) { 
      *proto_ver = args[1];
      *hostinfo  = args[2];
  }
  else
      *hostinfo = strdup(SPC_UNKNOWN_HOSTINFO_STR);	

  free((char *)args);
  
  return(TRUE);
}

/*----------------------------------------------------------------------+*/
int
sprint_register_data(XeString buf,
		     XeString username, 
		     XeString passwd,
		     XeString proto_ver,
		     XeString hostinfo)
/*----------------------------------------------------------------------+*/
{
  int i;
  XeString args[4];

  args[0]=username;
  args[1]=passwd;
  args[2]=proto_ver;
  args[3]=hostinfo;  
  
  i=sprint_counted_string(buf, 4, args, SPC_BUFSIZ);
  if(i==SPC_ERROR)
    return(SPC_ERROR);
  return(i);
}

/*----------------------------------------------------------------------+*/
int
sscan_register_data(XeString buf,
		    XeString *username,
		    XeString *passwd,
		    XeString *proto_ver,
                    XeString *hostinfo)
/*----------------------------------------------------------------------+*/
{
  XeString *args;

  args=sscan_counted_string(buf, NULL);

  if(args==SPC_ERROR)
    return(SPC_ERROR);
  
  *username=args[0];
  *passwd=args[1];

  /* args[2] and args[3] will only be around for protocol revision 2 or later */

  *proto_ver = *hostinfo = XeString_NULL;

  if (args[2]) { 
      *proto_ver = args[2];
      *hostinfo  = args[3];
  }
  else
      *hostinfo = strdup(SPC_UNKNOWN_HOSTINFO_STR);	

  free((char *)args);
  
  return(TRUE);
}

/*
 **
 ** Request / reply protocol requests
 **
*/

/*----------------------------------------------------------------------+*/
int SPC_Query_Devices(SPC_Channel_Ptr channel)
/*----------------------------------------------------------------------+*/
{
  SPC_Connection_Ptr connection=channel->connection;
  protocol_request_ptr prot;
  
  SPC_Write_Protocol_Request(connection, channel, QUERY_DEVICES);
  prot=SPC_Filter_Connection(connection, channel, DEVICE_REPLY, TRUE);
  if(prot==SPC_ERROR)
    return(SPC_ERROR);
  
  READ_DEVICE_REPLY(prot->dataptr,
		    &(channel->wires[STDIN]->master_name),
		    &(channel->wires[STDIN]->slave_name),
		    &(channel->wires[STDOUT]->master_name),
		    &(channel->wires[STDOUT]->slave_name),
		    &(channel->wires[STDERR]->master_name),
		    &(channel->wires[STDERR]->slave_name));

  SPC_Free_Protocol_Ptr(prot);
  return (TRUE);
}

/*----------------------------------------------------------------------+*/
int SPC_Query_Logfile(SPC_Channel_Ptr channel)
/*----------------------------------------------------------------------+*/
{
  SPC_Connection_Ptr connection=channel->connection;
  protocol_request_ptr prot;
  XeString junk1, junk2;
  
  SPC_Write_Protocol_Request(connection, channel, QUERY_LOGFILE);
  prot=SPC_Filter_Connection(connection, channel, LOGFILE_REPLY, TRUE);
  if(prot==SPC_ERROR)
    return(SPC_ERROR);
  
  READ_LOGFILE_REPLY(prot->dataptr, &channel->logfile, &junk1, &junk2);
  if (junk1) XeFree(junk1);
  if (junk1) XeFree(junk2);
		     
  SPC_Free_Protocol_Ptr(prot);
  return (TRUE);
}

#define UNK_TOKEN	"unknown"

/*----------------------------------------------------------------------+*/
XeString SPC_LocalHostinfo(void)
/*----------------------------------------------------------------------+*/
{
    struct utsname	name;
    int			s_len;
    static XeString	s = 0;

    _DtSvcProcessLock();
    if (!s) {
	if (uname(&name) >= 0) {
	    s_len = strlen(name.sysname) +
		    strlen(name.nodename) +
		    strlen(name.release) +
		    strlen(name.machine) + 4;
	    s = (XeString) XeMalloc(s_len * sizeof(XeChar));
	    sprintf(s, "%s:%s:%s:%s", name.nodename, name.sysname, name.release, name.machine);
	}
	else {
	    s_len = 4 * strlen(UNK_TOKEN) + 4;
	    s = (XeString) XeMalloc(s_len * sizeof(XeChar));
	    sprintf(s, "%s:%s:%s:%s", UNK_TOKEN, UNK_TOKEN, UNK_TOKEN, UNK_TOKEN);
	}
    }

    _DtSvcProcessUnlock();
    return s;
}

/*----------------------------------------------------------------------+*/
int
SPC_Validate_User(XeString hostname,
		  SPC_Connection_Ptr connection)
/*----------------------------------------------------------------------+*/
    /* Called by client to register itself to spcd */
{
  XeString username = XeString_NULL;
  uid_t this_uid;
  XeString proto_ver;
  XeString hostinfo;
  XeString path;
  protocol_request_ptr prot;
  int open_status, chmod_status;
  XeString logfile = NULL;
  XeString junk1 = NULL, junk2 = NULL;
  XeString connection_hostname=CONNECTION_HOSTNAME(connection);
  _Xgetpwparams	pwd_buf;
  struct passwd * pwd_ret;

  hostinfo = SPC_LocalHostinfo();

  /*
   *  We are now including the user ID to generate the LOGFILE
   *  (i.e., the authentication file)
   */
  this_uid=getuid();

  if((pwd_ret = _XGetpwuid(this_uid, pwd_buf)) == NULL) {
    /* 
     * Very strange situation - the uid isn't in the passwd file 
     */
    username = XeString_NULL;  /* we'll use the original /tmp subdirectory */
  }
  else {
    username=(XeString)(pwd_ret->pw_name);
  }

  SPC_Write_Protocol_Request(connection, NULL, REGISTER, 
			     username, XeString_Empty, 
			     SPC_PROTOCOL_VERSION_STR, hostinfo);

  
  prot=SPC_Filter_Connection(connection, NULL, LOGFILE_REPLY, TRUE);
  if(prot==SPC_ERROR) {
    XeFree(connection_hostname);
    return(SPC_ERROR);
  }

  /* In repsonse to the register, the daemon will send back a LOGFILE_REPLY */
  /* message that contains the name of a logfile to be used to do user      */
  /* authentication.  For A.02 and later daemons, it will also contain      */
  /* the spc protocol version and info about the host the daemon is on.     */

  READ_LOGFILE_REPLY(prot->dataptr, &logfile, &proto_ver, &hostinfo);

  /* For Pre A.01, this will be defaulted to (hpux 7.0 s300) */
  connection->hostinfo = hostinfo;

  if (proto_ver) {
     sscanf(proto_ver, "%d", &connection->protocol_version);
     XeFree(proto_ver);
   }

  SPC_Free_Protocol_Ptr(prot);

  if(!strcmp(logfile, PASSED_FILE_NAME))
    return(TRUE);
  if(!strcmp(logfile, FAILED_FILE_NAME)) {
    SPC_Error(SPC_Register_Username,
	      (username) ? username : (XeString)"<empty user>",
	      connection_hostname);
    XeFree(connection_hostname);
    if (logfile) XeFree(logfile);
    return(SPC_ERROR);
  }

  /*
   * Get a pathname to the authentication file.
   */
  path=tt_netfile_file(logfile);
  
  if(tt_ptr_error (path) != TT_OK) {
    SPC_Write_Protocol_Request(connection, NULL, ABORT);
    SPC_Error(SPC_Register_Netrc,
	      logfile,
	      connection_hostname);
    XeFree(connection_hostname);
    if (logfile) XeFree(logfile);
    return(SPC_ERROR);
  }

  open_status=open(path, O_CREAT, S_ISUID);
  if(open_status==ERROR) {
    SPC_Write_Protocol_Request(connection, NULL, ABORT);
    SPC_Error(SPC_Register_Open,
	      path,
	      connection_hostname);
    tt_free (path);
    XeFree(connection_hostname);
    if (logfile) XeFree(logfile);
    return(SPC_ERROR);
  }
  
  /* We need to also do a chmod because of an apparent Domain/OS bug
     where the open call does not properly set the UID bit.  We
     let chmod set the bit. */

  chmod_status=chmod(path, S_ISUID);
  if(chmod_status==ERROR) {
    SPC_Write_Protocol_Request(connection, NULL, ABORT);
    SPC_Error(SPC_Register_Open,
	      path,
	      connection_hostname);
    tt_free (path);
    XeFree(connection_hostname);
    if (logfile) XeFree(logfile);
    return(SPC_ERROR);
  }
  
  SPC_Write_Protocol_Request(connection, NULL, REGISTER, logfile, NULL, NULL, NULL);
  prot=SPC_Filter_Connection(connection, NULL, LOGFILE_REPLY, TRUE);

  close(open_status);
  unlink(path);
  tt_free (path);
  
  if(prot==SPC_ERROR) {
    XeFree(connection_hostname);
    return(SPC_ERROR);
  }
  
  /*
   * Free logfile before it gets malloc'd again.
   */
  XeFree(logfile);
  logfile = NULL;
  READ_LOGFILE_REPLY(prot->dataptr, &logfile, &junk1, &junk2);
  if (junk1) XeFree(junk1);
  if (junk2) XeFree(junk2);
  
  SPC_Free_Protocol_Ptr(prot);

  if(!strcmp(logfile, PASSED_FILE_NAME)) {
    XeFree(connection_hostname);
    if (logfile) XeFree(logfile);
    return(TRUE);
  }
  if(!strcmp(logfile, FAILED_FILE_NAME)) {
    SPC_Error(SPC_Register_Handshake,
	      username,
	      connection_hostname);
    XeFree(connection_hostname);
    if (logfile) XeFree(logfile);
    return(SPC_ERROR);
  }

  if (logfile) XeFree(logfile);
  XeFree(connection_hostname);
  SPC_Error(SPC_Protocol);
  return(SPC_ERROR);
}


/*----------------------------------------------------------------------+*/
static int SPC_Send_Termios(protocol_request_ptr prot_request)
/*----------------------------------------------------------------------+*/
{
  struct termios *tio;
  int retval;
  XeString s;

  tio = SPC_Get_Current_Termio();	/* Gets a (malloced) copy */
  s = SPC_Decode_Termios( tio );	/* Get ASCII representation */
  
  retval = WRITE_STRING(prot_request->dataptr, s);
  XeFree(tio);
  XeFree(s);

  return(retval);
  
}


/*----------------------------------------------------------------------+*/
int SPC_Get_Termios(protocol_request_ptr prot_request)
/*----------------------------------------------------------------------+*/
{
    XeString s;
    int      i;

    _DtSvcProcessLock();
    if(XeTermioStruct == NULL) {
	XeTermioStruct = (struct termios *)XeMalloc(sizeof(struct termios));

	for(i=0; i<NCCS; i++) 
	    XeTermioStruct->c_cc[i] = 0;
    }
    
    READ_STRING_NO_COPY(prot_request->dataptr, s);
    
    SPC_Encode_Termios(s, XeTermioStruct);

    _DtSvcProcessUnlock();
    return(XeSetpgrp(FALSE));
}



/*----------------------------------------------------------------------+*/
int SPC_Get_Termio(protocol_request_ptr UNUSED_PARM(prot_request))
/*----------------------------------------------------------------------+*/
{
    /* This is for old 1.0, 1.1 versions of the SPC code.  We used to  */
    /* pass an HPUX version of the termio struct around.  This was not */
    /* portable. If we get one of these requests, just bit bucket it   */
    /* as we do not know how to deal with it.                          */

    return(XeSetpgrp(FALSE));
}

static int SPC_Send_B00_Spawn(SPC_Connection_Ptr connection,
			      protocol_request_ptr prot,
			      char *path,
			      char *dir,
			      char **argv,
			      char **envp)
{
  char **merged_ptr;
  int num_elts=0, this_elt, num_argv, num_envp;
  char argv_buf[20], envp_buf[20];
  int retval;
  
  num_argv = 0;
  while(argv && argv[num_argv++])
    num_elts++;

  num_envp = 0;
  while(envp && envp[num_envp++])
    num_elts++;

  merged_ptr = (char **)malloc((num_elts+6) * sizeof(char *));

  sprintf(argv_buf, "%d", num_argv);
  sprintf(envp_buf, "%d", num_envp);
  
  num_elts=0;
  merged_ptr[num_elts++] = path;
  merged_ptr[num_elts++] = dir;
  merged_ptr[num_elts++] = argv_buf;
  merged_ptr[num_elts++] = envp_buf;

  this_elt = 0;
  while(argv && argv[this_elt])
    merged_ptr[num_elts++] = argv[this_elt++];
  
  merged_ptr[num_elts++] = "DUMMY";

  this_elt = 0;
  while(envp && envp[this_elt])
    merged_ptr[num_elts++] = envp[this_elt++];

  merged_ptr[num_elts] = NULL;
  
  retval = SPC_Send_Multi_Packet(connection, prot,
				 merged_ptr, num_elts,
				 APP_B00_SPAWN, "  <-- APP_B00_SPAWN",
				 SPC_Arg_Too_Long);
  
  free((char *)merged_ptr);

  return(retval);
}
