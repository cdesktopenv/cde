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
 * File:         spc-proto.h $XConsortium: spc-proto.h /main/3 1995/10/26 15:43:39 rswiston $
 * Language:     C
 *
 * (c) Copyright 1988, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#ifndef _spc_proto_h
#define _spc_proto_h

/*
 * Commands which are known to spcd and clients
 */

/* The internet service */

#define SPC_SERVICE	(XeString)"dtspc"  /* The name of the registered service */
#define SPC_PROTOCOL	(XeString)"tcp"	 /* The name of the registered protocol */

  /*  MUST be a DECIMAL number as "%d" */
#define SPC_PROTOCOL_VERSION_CDE_BASE 1000
#define SPC_PROTOCOL_VERSION SPC_PROTOCOL_VERSION_CDE_BASE 
  /*  is used to read the string version */
#define SPC_PROTOCOL_VERSION_CDE_BASE_STR "1000"
#define SPC_PROTOCOL_VERSION_STR SPC_PROTOCOL_VERSION_CDE_BASE_STR 

        /* We could make this user config for old domain systems? */
#define SPC_UNKNOWN_HOSTINFO_STR  "HP-UX:7.0:*"
/*
 **
 ** Pseudo-filenames used for validating user
 **
*/

#define PASSED_FILE_NAME (XeString)"<!passed>!"
#define FAILED_FILE_NAME (XeString)"<!failed>!"

/*
 *** Protocol command numbers.  These MUST remain in the same order
 *** to work with previous version of the spcd
 */

/* Connections to the server */

#define APPLICATION_DATA   0 /* send data to subprocess */
#define APPLICATION_STDOUT 1 /* data from subprocess */
#define APPLICATION_STDERR 2 /* error messages from subprocess */

#define ABORT              3 /* Client aborted */
#define REGISTER           4 /* Register client */
#define UNREGISTER         5 /* Unregister client */

/* Channel requests */

#define CHANNEL_OPEN       6 /* enable this channel for read/write */
#define CHANNEL_CLOSE      7 /* no more i/o to this channel (send EOF) */
#define CHANNEL_RESET      8 /* reset this channel */
#define CHANNEL_ATTACH     9 /* attach PTY device to this channel */

/* Application messages, client -> server */

#define APPLICATION_SPAWN  10 /* start up subprocess */
#define APPLICATION_SIGNAL 11 /* send signal to subprocess */

/* application messages, server -> client */

#define APPLICATION_DIED   12 /* sent when subprocess dies */

#define SERVER_ERROR       13 /* server has detected an error */
#define REPLY              14
#define SERVER_DEBUG       15
#define ENVIRON_RESET      16

/* Query / reply protocols, client -> server -> client */

#define QUERY_DEVICES      17
#define DEVICE_REPLY       18

#define QUERY_LOGFILE      19
#define LOGFILE_REPLY      20

/* Application messages, client -> server */

#define DELETE_LOGFILE     21

#define RESET_TERMIO       22	/* This is obsolete, (hpux binary version) */
#define RESET_TERMIOS      23

/* New B.00 protocol requests */
  
#define CHANNEL_SEND_EOF   24   /* Send EOF to standard input of sub process */
#define CHANNEL_TERMIOS    25   /* Send termios info for a particular pty */ 
#define APP_B00_SPAWN      26   /* Enhanced spawn */

#define NREQS              27  /* total number of requests.  If
				  any are added, this should constant
				  should reflect the new total */

#define PROT_TO_CONNECTOR(a)  a
#define CONNECTOR_TO_PROT(a)  a
  
#define PDRP(_pdata) _pdata->data+REQUEST_HEADER_LENGTH

#define WRITE_INT(pdata, val)\
  sprintf_len(PDRP(pdata), (XeString)"%x", val)
#define READ_INT(pdata, val)    sscanf(PDRP(pdata),  (XeString)"%x", &val)

#define WRITE_STRING(pdata, name)\
  sprintf_len(PDRP(pdata), (XeString)"%s", name)
#define READ_STRING(pdata, name)   sscanf(PDRP(pdata),  (XeString)"%s", name)
#define READ_STRING_NO_COPY(pdata, name)   (name = PDRP(pdata))

#define WRITE_ABORT             WRITE_INT
#define READ_ABORT              READ_INT

#define WRITE_OPEN              WRITE_INT
#define READ_OPEN               READ_INT

#define WRITE_ATTACH            WRITE_INT
#define READ_ATTACH             READ_INT

#define WRITE_APPLICATION_DIED  WRITE_INT
#define READ_APPLICATION_DIED   READ_INT

#define WRITE_DEBUG             WRITE_STRING
#define READ_DEBUG              READ_STRING

#define WRITE_ERROR             WRITE_INT
#define READ_ERROR              READ_INT

#define WRITE_REGISTER(pdata, name, passwd, proto_ver, hostinfo)\
  sprint_register_data(PDRP(pdata), name, passwd, proto_ver, hostinfo)

#define READ_REGISTER(pdata, name, passwd, proto_rev, hostinfo)\
  sscan_register_data(PDRP(pdata),  &name, &passwd, &proto_rev, &hostinfo)

#define WRITE_REPLY(pdata, val, err)\
  sprintf_len(PDRP(pdata), (XeString)"%x %x", val, err)
#define READ_REPLY(pdata, val, err)\
  sscanf(PDRP(pdata),  (XeString)"%x %x", &val, &err)

#define WRITE_DEVICE_REPLY(pdata, m0, s0, m1, s1, m2, s2) \
  sprint_device_data(PDRP(pdata), m0, s0, m1, s1, m2, s2)
#define READ_DEVICE_REPLY(pdata, m0, s0, m1, s1, m2, s2) \
  sscan_device_data(PDRP(pdata), m0, s0, m1, s1, m2, s2)

#define WRITE_LOGFILE_REPLY(pdata, logfile, proto_ver, hostinfo) \
  sprint_logfile_data(PDRP(pdata), logfile, proto_ver, hostinfo)
#define READ_LOGFILE_REPLY(pdata, logfile, proto_ver, hostinfo) \
  sscan_logfile_data(PDRP(pdata), logfile, proto_ver, hostinfo)

#define WRITE_APPLICATION_SPAWN(pdata, path, dir, argv, envp) \
  sprint_application_data(PDRP(pdata), (XeString)"%s %d %d ",  \
			  path, dir, argv, envp, REQUEST_HEADER_LENGTH)
#define READ_APPLICATION_SPAWN(pdata, path, dir, argv, envp) \
  sscan_application_data(PDRP(pdata), (XeString)"%s %d %d ", \
			 &path, &dir, &argv, &envp, REQUEST_HEADER_LENGTH)

#define WRITE_ENVIRON_RESET(pdata, numenv) \
  sprintf_len(PDRP(pdata), (XeString)"%d ", numenv)
#define READ_ENVIRON_RESET(pdata, numenv) \
  sscanf(PDRP(pdata), (XeString)"%d ", &numenv)

#define WRITE_APP_DATA(pdata, buffer, len) \
  (memcpy(PDRP(pdata), buffer, len), len)

/* New B.00 protocol requests */
#define WRITE_TERMIOS(pdata, connector, side, buffer) \
  sprintf_len(PDRP(pdata), (XeString)"%d %d %s", connector, side, buffer)
#define READ_TERMIOS(pdata, connector, side, buffer) \
  sscanf(PDRP(pdata), (XeString)"%d %d %s", &connector, &side, buffer)
  
/* Writing a header is special, as we don't want to be offset by
   REQUEST_HEADER_LENGTH */

#define WRITE_HEADER(pdata, cid, type, len, seq) \
  sprintf_len(pdata->data, (XeString)"%08x%02x%04x%04x", cid, type, len, seq)
#define READ_HEADER(pdata, cid, type, len, seq) \
  sscanf(pdata->data, (XeString)"%8x%2x%4x%4x", cid, type, len, seq)

#define min(a, b) (((a) < (b)) ? (a) : (b))

/*
 **
 ** The test here is reversed, that is, we check only for those
 ** requests where there is no reply expected, and assume the other
 ** ones do expect replies.
 **
*/

#define NO_REPLY_VAL  (-1)

#define REPLY_EXPECTED(a, retval)  \
  (((a==REPLY)                  || \
    (a==APPLICATION_DATA)       || \
    (a==APPLICATION_STDOUT)     || \
    (a==APPLICATION_STDERR)     || \
    (a==REGISTER)               || \
    (a==QUERY_LOGFILE)          || \
    (a==QUERY_DEVICES)          || \
    (a==ABORT)                  || \
    (a==SERVER_ERROR)              \
    ) ? NO_REPLY_VAL : retval)

typedef struct _prot_request {
  buffered_data_ptr dataptr;
  int seqno;
  int request_type;
  SPC_Channel_Ptr channel;
  struct _prot_request *next;
  } protocol_request, *protocol_request_ptr;

typedef int (*protocol_request_handler)(protocol_request_ptr);

/* spc-proto.c */
buffered_data_ptr SPC_New_Buffered_Data_Ptr (void);
void SPC_Reset_Protocol_Ptr (protocol_request_ptr prot, SPC_Channel_Ptr channel, XeChar req, int len);
protocol_request_ptr SPC_New_Protocol_Ptr (SPC_Channel_Ptr channel, XeChar req, int len);
void SPC_Free_Protocol_Ptr (protocol_request_ptr prot);
SPC_Channel_Ptr SPC_Lookup_Channel (int cid, SPC_Connection_Ptr connection);
SPC_Connection_Ptr SPC_Alloc_Connection (void);
SPC_Connection_Ptr SPC_Lookup_Connection (XeString hostname);
SPC_Connection_Ptr SPC_Lookup_Connection_Fd (int fd);
SPC_Connection_Ptr SPC_Make_Connection (XeString hostname);
void SPC_Add_Connection (SPC_Connection_Ptr connection);
void SPC_Close_Connection (SPC_Connection_Ptr connection);
int SPC_Read_Chars (SPC_Connection_Ptr connection, int request_len, XeString charptr);
int SPC_Write_Chars (int fd, XeString charptr, int request_len);
protocol_request_ptr SPC_Read_Protocol (SPC_Connection_Ptr connection);
protocol_request_ptr SPC_Filter_Connection (SPC_Connection_Ptr connection, SPC_Channel_Ptr channel, int reqtype, int deletep);
void SPC_Flush_Queued_Data (SPC_Channel_Ptr channel);
int SPC_Read_Remote_Data (SPC_Channel_Ptr channel, int connector, XeString client_buffer, int nbytes);
int print_protocol_request (XeString name, protocol_request_ptr proto);
int SPC_Write_Protocol_Request (SPC_Connection_Ptr connection, SPC_Channel_Ptr channel, int request, ...);
int SPC_Write_Single_Prot_Request (SPC_Connection_Ptr connection, XeString name, protocol_request_ptr prot);
int SPC_Waitfor_Reply (SPC_Connection_Ptr connection, SPC_Channel_Ptr channel, int seqno);
int SPC_Dispatch_Protocol (protocol_request_ptr proto, protocol_request_handler *);
int SPC_Write_Reply (SPC_Connection_Ptr conn, protocol_request_ptr proto, int retval, int errval);
int SPC_Send_Environ (SPC_Connection_Ptr connection, protocol_request_ptr prot);
int sprint_counted_string (XeString buf, int count, XeString *vect, int limit);
XeString *sscan_counted_string (XeString buf, XeString *newbuf);
int sprint_application_data (XeString buf, XeString fmt, XeString path, XeString dir, XeString *argv, XeString *envp, int chars_used);
int sscan_application_data (XeString buf, XeString fmt, XeString *path, XeString *dir, XeString **argv, XeString **envp, int offset);
int sprint_device_data (XeString buf, XeString m0, XeString s0, XeString m1, XeString s1, XeString m2, XeString s2);
int sscan_device_data (XeString buf, XeString *m0, XeString *s0, XeString *m1, XeString *s1, XeString *m2, XeString *s2);
int sprint_logfile_data (XeString buf, XeString logfile, XeString proto_ver, XeString hostinfo);
int sscan_logfile_data (XeString buf, XeString *logfile, XeString *proto_ver, XeString *hostinfo);
int sprint_register_data (XeString buf, XeString username, XeString passwd, XeString proto_ver, XeString hostinfo);
int sscan_register_data (XeString buf, XeString *username, XeString *passwd, XeString *proto_ver, XeString *hostinfo);
int SPC_Query_Devices (SPC_Channel_Ptr channel);
int SPC_Query_Logfile (SPC_Channel_Ptr channel);
int SPC_Validate_User (XeString hostname, SPC_Connection_Ptr connection);
int SPC_Get_Termio (protocol_request_ptr prot_request);
int SPC_Get_Termios (protocol_request_ptr prot_request);
XeString SPC_LocalHostinfo(void);
int SPC_Send_Multi_Packet(SPC_Connection_Ptr connection,
			  protocol_request_ptr prot,
			  char **str_vect,
			  int num_str,
			  int req,
			  XeString name,
			  int errid);
char **SPC_Get_Multi_Packet(SPC_Connection_Ptr connection,
			    protocol_request_ptr prot,
			    char **out,
			    int *outlen,
			    int request,
			    XeString name);

#endif /* _spc_proto_h */
