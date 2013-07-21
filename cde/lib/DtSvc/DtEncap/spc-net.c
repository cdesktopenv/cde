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
/* $XConsortium: spc-net.c /main/9 1996/11/21 19:53:44 drk $
 * File:         spc-net.c 
 * Language:     C
 *
 * (c) Copyright 1989, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#define  __need_timeval
#define  __need_fd_set
#define  __need_all_errors

#include <bms/sbport.h> /* NOTE: sbport.h must be the first include. */
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define X_INCLUDE_NETDB_H
#define XOS_USE_XT_LOCKING
#include <X11/Xos_r.h>

#include <SPC/spcP.h>
#include <bms/MemoryMgr.h>
#include <SPC/spc-proto.h>
#include "DtSvcLock.h"

extern int SPC_Initialized;

/*
 ****
 **** Client-side code
 ****
*/

/* Variables representing the local machine (initialized only once) */

static struct hostent *official_hp = NULL;
XeString official_hostname = NULL;

/*
 * my_gethost will return a copy of the data returned by gethostbyname
 *
*/

/*----------------------------------------------------------------------+*/
static struct hostent *my_gethost(XeString hostname)
/*----------------------------------------------------------------------+*/
{
  struct hostent *host_def, *copy;
  int alias_count, i, addr_count, addrlen;
  _Xgethostbynameparams	host_buf;

  host_def = _XGethostbyname(hostname, host_buf);
  if (host_def == NULL)
    return(FALSE);
  
  copy=(struct hostent *)XeMalloc(sizeof(struct hostent));
  
  /* Copy non-pointer info */
  memcpy((char *)copy, (char *)host_def, sizeof(struct hostent));
  
  alias_count=0;

  while(host_def->h_aliases[alias_count++]);

  copy->h_aliases=(char **)XeMalloc(alias_count*(sizeof(XeString)));
  
  addr_count=0;
  while(host_def->h_addr_list[addr_count++]);

  copy->h_addr_list=(char **)XeMalloc(addr_count*(sizeof(XeString)));
  
  /* Copy Hostname */

  copy->h_name=SPC_copy_string(host_def->h_name);

  /* Copy the host address.  We do not use SPC_copy_string here,
     because the address is not a string, and may have embedded
     NULLs in it. */
  
  addrlen = host_def->h_length;
  addr_count -= 1;
  copy->h_addr_list[addr_count]=XeChar_NULL;
  for (i=0; i < addr_count; i++) {
    copy->h_addr_list[i]=(char *)XeMalloc(addrlen);
    memcpy(copy->h_addr_list[i], host_def->h_addr_list[i], addrlen);
  }
  
  while(alias_count--) {
    copy->h_aliases[alias_count]=SPC_copy_string(host_def->h_aliases[alias_count]);
  }

  return(copy);
}
				 
/*
 * SPC_Lookup_Host will try its darndest to return a hostent structure
 * for the passed hostname.
 *
*/

/*----------------------------------------------------------------------+*/
static struct hostent *SPC_Lookup_Host(XeString hostname)
/*----------------------------------------------------------------------+*/
{

  struct hostent *official_def;
  
  official_def = my_gethost(hostname);
  if(!official_def) {
    SPC_Error(SPC_Unknown_Host, hostname);
    return(SPC_ERROR);
  }

  return(official_def);
  
}

/*
 *
 * SPC_Init_Local_Host_Info will initialize the local host info.  It
 * is intended to be called only once.
 *
*/

/*----------------------------------------------------------------------+*/
Boolean SPC_Init_Local_Host_Info(void)
/*----------------------------------------------------------------------+*/
{
  _DtSvcProcessLock();
  official_hostname=(XeString) XeMalloc(MAXHOSTNAMELEN+1);
  
  Xegethostname(official_hostname, MAXHOSTNAMELEN);
  official_hostname[MAXHOSTNAMELEN]=0;
  
  if(!official_hp)
    official_hp = SPC_Lookup_Host(official_hostname);

  _DtSvcProcessUnlock();
  return(official_hp != NULL);
}

/*
 * SPC_Local_Hostname takes a string indicating a hostname, and returns
 * TRUE if it represents a local host, and FALSE if it is remote.
 */

/*----------------------------------------------------------------------+*/
int
SPC_Local_Hostname(XeString hostname)
/*----------------------------------------------------------------------+*/
{
  /* Return TRUE if the specified hostname is local, otherwise it's remote */

  /* If no hostname specified, then local by definition */

  if (!hostname || !*hostname) return TRUE;

#ifdef DEBUG  
  return(FALSE);
#endif

  return(XeIsLocalHostP(hostname));
}

/*
 *
 * SPC_Open_Connection will a connection pointer to be used for any
 * subsequent communication to the remote host.  It will either return
 * an already opened connection, or create and initialize a new one.
 *
*/

/*----------------------------------------------------------------------+*/
SPC_Connection_Ptr SPC_Open_Connection(XeString hostname)
/*----------------------------------------------------------------------+*/
{
  SPC_Connection_Ptr connection;
  int seqno;
  XeString canonical_hostname;
  int tmp_errorno;

  /* I was told that XeFindShortHost was the correct routine to use here,
     but that may change in the future. */

  canonical_hostname=XeFindShortHost(hostname);
  
  /* check for a currently open connection */
  connection=SPC_Lookup_Connection(canonical_hostname);
  if(connection) {
    if(connection->connected) {
      XeFree(canonical_hostname);
      return(connection);
    }
    else {
      SPC_Close_Connection(connection);
      connection=NULL;
    }
  }
      
  /* None currently open.  Grab a new one & initialize it. */

  if((connection = SPC_Make_Connection(canonical_hostname))==SPC_ERROR) {
    SPC_Close_Connection(connection);
    XeFree(canonical_hostname);
    return(SPC_ERROR);
  }
  connection->local=official_hp;
  if((connection->remote = SPC_Lookup_Host(canonical_hostname)) == SPC_ERROR) {
    SPC_Close_Connection(connection);
    XeFree(canonical_hostname);
    return(SPC_ERROR);
  }
  
  if(SPC_Contact_Server(connection)==SPC_ERROR) {
    SPC_Close_Connection(connection);
    XeFree(canonical_hostname);
    return(SPC_ERROR);
  }

  connection->connected=TRUE;
  
  if(SPC_Validate_User(canonical_hostname, connection)==SPC_ERROR) {
    SPC_Close_Connection(connection);
    XeFree(canonical_hostname);
    return(SPC_ERROR);
  }

  seqno=SPC_Write_Protocol_Request(connection, NULL, ENVIRON_RESET);
  _DtSvcProcessLock();
  tmp_errorno = XeSPCErrorNumber;
  if(SPC_Waitfor_Reply(connection, NULL, seqno) == SPC_ERROR) {
    SPC_Close_Connection(connection);
    /*
     * XeSPCErrorNumber could have been changed but want to
     * return the value from Write_Protocol_Request to the 
     * client.
     */
    if (tmp_errorno != 0)
      XeSPCErrorNumber = tmp_errorno;
    XeFree(canonical_hostname);
    _DtSvcProcessUnlock();
    return(SPC_ERROR);
  }
  _DtSvcProcessUnlock();

  /* We no long ever send a RESET_TERMIO request as this was hpux */
  /* specific and VERY non-portable.				  */

  if (connection->protocol_version >= 2) {
      seqno=SPC_Write_Protocol_Request(connection, NULL, RESET_TERMIOS);
      if(SPC_Waitfor_Reply(connection, NULL, seqno) == SPC_ERROR) {
	  SPC_Close_Connection(connection);
          XeFree(canonical_hostname);
	  return(SPC_ERROR);
      }
  }
  
  XeFree(canonical_hostname);
  return(connection);
  
}

/*----------------------------------------------------------------------+*/
int
SPC_Open_Socket(SPC_Connection_Ptr conn,
		int type)
/*----------------------------------------------------------------------+*/
{

  struct servent *service;
  
  conn->sid=socket(type, SOCK_STREAM, 0);
  if(conn->sid == ERROR) {
    SPC_Error(SPC_Bad_Socket);
    return(SPC_ERROR);
  }
  
  service=getservbyname(SPC_SERVICE, SPC_PROTOCOL);
  if (!service) {
    SPC_Error(SPC_Bad_Service, SPC_SERVICE, SPC_PROTOCOL);
    return(FALSE);
  }

  return(service->s_port);
}


/*
 *
 * SPC_Contact_Server will attempt to contact the server specified by
 * the passed connection data structure.  IT ASSUMES THAT ALL FIELDS
 * EXCEPT THE SOCKET ID ARE FILLED IN!!!
 *
*/

/*----------------------------------------------------------------------+*/
int
SPC_Contact_Server(SPC_Connection_Ptr connection)
/*----------------------------------------------------------------------+*/
{
  struct sockaddr_in saddr;
  short addrtype;
  struct hostent *remote;
  
  
  /* Check that the connection is initialized correctly */
  if(!connection)
    return(SPC_ERROR);
  if(!(remote=connection->remote))
    return(SPC_ERROR);
  if(connection->connected)
    return(TRUE);
  
  addrtype=saddr.sin_family=remote->h_addrtype;
  if(!(saddr.sin_port=SPC_Open_Socket(connection, addrtype)))
    return(SPC_ERROR);
  memcpy(&saddr.sin_addr, remote->h_addr, remote->h_length);

  if(connect(connection->sid, (struct sockaddr *)&saddr, sizeof(saddr)) == ERROR) {
    SPC_Error(SPC_Bad_Connect,
	      XeFindShortHost(remote->h_name));
    return(SPC_ERROR);
  }

  return(TRUE);
}

/*
 ****
 **** Server (daemon) side code
 ****
*/ 

#define BACKLOG		         50
#define MAX_SERVER_BIND_ATTEMPTS 30
#define SERVER_PAUSE_INTERVAL    10

/*----------------------------------------------------------------------+*/
SPC_Connection_Ptr SPC_Init_Child(SPC_Connection_Ptr conn,
				  int from)
/*----------------------------------------------------------------------+*/
{
  /* We are the child.  Close the connection file descriptor
     (which is the socket, not our input). */
  close(conn->sid);

  /* Make the from file descriptor correspond to STDIN/STDOUT */
  dup2(from, STDIN);
  close(from);
  dup2(STDIN, STDOUT);

  /* make conn point to STDIN */
  
  conn->sid=STDIN;

  return(conn);
}

SPC_Connection_Ptr SPC_Standalone_Daemon(SPC_Connection_Ptr conn)
{
  struct sockaddr_in saddr, client_saddr;
#ifdef USL
  /* Only UnixWare 2.02 uses the Spec1170 parameter profile for accept(). */
  size_t len=sizeof(client_saddr);
#else
  int len=sizeof(client_saddr);
#endif
  int server_bind_attempts      = MAX_SERVER_BIND_ATTEMPTS;
  int server_bind_pause         = SERVER_PAUSE_INTERVAL;
  int pid, from;
#if defined(__aix)
  int on=1;                               /* required by setsockopt */
#endif

  saddr.sin_family=AF_INET;
  if(!(saddr.sin_port=SPC_Open_Socket(conn, saddr.sin_family)))
    return(SPC_ERROR);
  saddr.sin_addr.s_addr=INADDR_ANY; /* Any host address */

  /* Reuse the socket address if it is still in a timeout state */

#if defined(__aix)
  if (setsockopt(conn->sid, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))==ERROR) {
#else
  if (setsockopt(conn->sid, SOL_SOCKET, SO_REUSEADDR, NULL, 0)==ERROR) {
#endif
    SPC_Error(SPC_Bad_Reuse);
    return(SPC_ERROR);
  }

  while (bind(conn->sid, (struct sockaddr *)&saddr, sizeof(saddr)) == ERROR) {
    if (errno == EADDRINUSE) {
      SPC_Error(SPC_Bind_Timeout);
      /* Try to get the connection in a little while */
      if (server_bind_attempts > 0) {
	server_bind_attempts--;
	sleep(server_bind_pause);
      }
      else {
	/* We don't want to wait forever */
	SPC_Error(SPC_Timeout);
	return(SPC_ERROR);
      }
    } else {
      SPC_Error(SPC_Bad_Bind);
      return(SPC_ERROR);
    }
  }

  /* Set up a queue for incoming connection requests */

  listen(conn->sid, BACKLOG);

  /* We are running standalone, so we need to loop forever waiting for
     requests.  When we get one, we will fork a child to take care of
     the processing for us, and then the parent will listen some more */

  for(;;) {
    struct hostent		*addr_ret;
    _Xgethostbynameparams	addr_buf;

    /* Attempt to accept a connection with a client */
    
    from = accept(conn->sid, (struct sockaddr *)&client_saddr, &len);
    if (from == ERROR) {
      SPC_Error(SPC_Bad_Accept);
      return(SPC_ERROR);
    }
    
    addr_ret = _XGethostbyaddr((char *)&client_saddr.sin_addr,
			       sizeof(client_saddr.sin_addr),
			       client_saddr.sin_family,
			       addr_buf);

    conn->remote = addr_ret;
    strncpy(conn->hostname, conn->remote->h_name, MAXHOSTNAMELEN);

#ifdef DEBUG
    pid = NULL;
#else    
    /* Fork a process to handle I/O from/to client */
    pid = fork();
#endif
    
    if (pid == ERROR) {
      SPC_Error(SPC_Cannot_Fork);
      /* We don't return here, but simply go around for a new try */
    }

    if (!pid)
      /* We are the child.  Do whatever processing we need to do
	 on the connection & return */
      return(SPC_Init_Child(conn, from));
    /* Otherwise, we are still the parent.  Loop around for another
       connection request */
  }
}

/*----------------------------------------------------------------------+*/
int
SPC_Inetd_Daemon(SPC_Connection_Ptr conn)
/*----------------------------------------------------------------------+*/
{
  conn->sid=0;
  return(TRUE);
}

/*----------------------------------------------------------------------+*/
SPC_Connection_Ptr SPC_Start_Daemon(int standalone)
/*----------------------------------------------------------------------+*/
{

  SPC_Connection_Ptr connection;

  /* Do whatever it takes to initialize SPC */
  _DtSvcProcessLock();
  if (!SPC_Initialized)
      if(SPC_Initialize()==SPC_ERROR) {
	  _DtSvcProcessUnlock();
	  return(SPC_ERROR);
      }
  _DtSvcProcessUnlock();

  /* Get ourselves a connection structure.  We don't know the name
     of the remote client yet, so use the null string as hostname */

  if((connection=SPC_Make_Connection(NULL))==SPC_ERROR)
    return(SPC_ERROR);
  connection->local=official_hp;
  if(standalone) {
    if((SPC_Standalone_Daemon(connection))==SPC_ERROR)
      return(SPC_ERROR);
  } else {
    if((SPC_Inetd_Daemon(connection))==SPC_ERROR)
      return(SPC_ERROR);
  }
  connection->connected=TRUE;
  return(connection);
}
