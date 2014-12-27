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
/* $TOG: xdmcp.c /main/5 1998/04/06 13:36:04 mgreess $ */
/* (c) Copyright 1997 The Open Group */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

/*
 * @DEC_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log$
 * Revision 1.1.2.3  1995/06/06  20:25:54  Chris_Beute
 * 	Code snapshot merge from March 15 and SIA changes
 * 	[1995/05/31  20:17:42  Chris_Beute]
 *
 * Revision 1.1.2.2  1995/04/21  13:05:47  Peter_Derr
 * 	dtlogin auth key fixes from deltacde
 * 	[1995/04/14  18:03:44  Peter_Derr]
 * 
 * 	Merge in dtlogin changes to WaitForSomething() to support command
 * 	line console login.
 * 	[1995/04/14  17:40:05  Peter_Derr]
 * 
 * 	Use R6 xdm code to handle XDMCP
 * 	[1995/04/10  19:24:11  Peter_Derr]
 * 
 * $EndLog$
 */
/*

Copyright (c) 1988  X Consortium

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization
from the X Consortium.

*/

/*
 * xdm - display manager daemon
 * Author:  Keith Packard, MIT X Consortium
 *
 * xdmcp.c - Support for XDMCP
 */

# include "dm.h"

# include	<X11/X.h>
# include	<X11/Xfuncs.h>
# include	<sys/types.h>
# include	<ctype.h>

#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<sys/un.h>
#include	<netdb.h>

#ifdef X_NOT_STDC_ENV
#define Time_t long
extern Time_t time ();
#else
#include <time.h>
#define Time_t time_t
#endif

#define getString(name,len)	((name = malloc (len + 1)) ? 1 : 0)

/*
 * interface to policy routines
 */

extern ARRAY8Ptr	ChooseAuthentication ();
extern int		SelectConnectionTypeIndex ();

void query_respond (struct sockaddr *from, int fromlen, int length);
void broadcast_respond (struct sockaddr *from, int fromlen, int length);
void forward_respond (struct sockaddr *from, int fromlen, int length);
void request_respond (struct sockaddr *from, int fromlen, int length);
void send_willing (struct sockaddr *from, int fromlen, ARRAY8Ptr authenticationName, ARRAY8Ptr status);
void send_unwilling (struct sockaddr *from, int fromlen, ARRAY8Ptr authenticationName, ARRAY8Ptr status);
void send_accept (struct sockaddr *to, int tolen, CARD32 sessionID, ARRAY8Ptr authenticationName, ARRAY8Ptr authenticationData, ARRAY8Ptr authorizationName, ARRAY8Ptr authorizationData);
void manage (struct sockaddr *from, int fromlen, int length);
void send_decline (struct sockaddr *to, int tolen, ARRAY8Ptr authenticationName, ARRAY8Ptr authenticationData, ARRAY8Ptr status);
void send_failed (struct sockaddr *from, int fromlen, char *name, CARD32 sessionID, char *reason);
void send_refuse (struct sockaddr *from, int fromlen, CARD32 sessionID);
void send_alive (struct sockaddr *from, int fromlen, int length);



int	xdmcpFd = -1;
int	chooserFd = -1;

FD_TYPE	WellKnownSocketsMask;
int	WellKnownSocketsMax;

#define pS(s)	((s) ? ((char *) (s)) : "empty string")

void
DestroyWellKnownSockets (void)
{
    if (xdmcpFd != -1)
    {
	close (xdmcpFd);
	xdmcpFd = -1;
    }
    if (chooserFd != -1)
    {
	close (chooserFd);
	chooserFd = -1;
    }
}

int
AnyWellKnownSockets (void)
{
    return xdmcpFd != -1 || chooserFd != -1;
}

static XdmcpBuffer	buffer;

/*ARGSUSED*/
static int
sendForward (CARD16 connectionType, ARRAY8Ptr address, char *closure)
{
#ifdef AF_INET
    struct sockaddr_in	    in_addr;
#endif
#ifdef AF_DECnet
#endif
    struct sockaddr	    *addr;
    int			    addrlen;

    switch (connectionType)
    {
#ifdef AF_INET
    case FamilyInternet:
	addr = (struct sockaddr *) &in_addr;
	bzero ((char *) &in_addr, sizeof (in_addr));
#ifdef BSD44SOCKETS
	in_addr.sin_len = sizeof(in_addr);
#endif
	in_addr.sin_family = AF_INET;
	in_addr.sin_port = htons ((short) XDM_UDP_PORT);
	if (address->length != 4)
	    return 0;
	memmove( (char *) &in_addr.sin_addr, address->data, address->length);
	addrlen = sizeof (struct sockaddr_in);
	break;
#endif
#ifdef AF_DECnet
    case FamilyDECnet:
#endif
    default:
	return 0;
    }
    XdmcpFlush (xdmcpFd, &buffer, (XdmcpNetaddr)addr, addrlen);
    return 0;
}

extern char *NetaddrAddress();
extern char *NetaddrPort();

static void
ClientAddress (struct sockaddr *from, ARRAY8Ptr addr, ARRAY8Ptr port, CARD16 *type)
{
    int length, family;
    char *data;

    data = NetaddrPort(from, &length);
    XdmcpAllocARRAY8 (port, length);
    memmove( port->data, data, length);
    port->length = length;

    family = ConvertAddr((char *)from, &length, &data);

    XdmcpAllocARRAY8 (addr, length);
    memmove( addr->data, data, length);
    addr->length = length;

    *type = family;
}

static void
all_query_respond (struct sockaddr *from, int fromlen, ARRAYofARRAY8Ptr authenticationNames, xdmOpCode type)
{
    ARRAY8Ptr	authenticationName;
    ARRAY8	status;
    ARRAY8	addr;
    CARD16	connectionType;
    int		family;
    int		length;

    family = ConvertAddr((char *)from, &length, (char **) &(addr.data));

    addr.length = length;	/* convert int to short */
    Debug ("all_query_respond: conntype=%d, addr=%lx, len=%d\n",
	   family, *(addr.data), addr.length);
    if (family < 0)
	return;
    connectionType = family;

    if (type == INDIRECT_QUERY)
	RememberIndirectClient (&addr, connectionType);
    else
	ForgetIndirectClient (&addr, connectionType);

    authenticationName = ChooseAuthentication (authenticationNames);
    if (Willing (&addr, connectionType, authenticationName, &status, type))
	send_willing (from, fromlen, authenticationName, &status);
    else
	if (type == QUERY)
	    send_unwilling (from, fromlen, authenticationName, &status);
    XdmcpDisposeARRAY8 (&status);
}

static void
indirect_respond (struct sockaddr *from, int fromlen, int length)
{
    ARRAYofARRAY8   queryAuthenticationNames;
    ARRAY8	    clientAddress;
    ARRAY8	    clientPort;
    CARD16	    connectionType;
    int		    expectedLen;
    int		    i;
    XdmcpHeader	    header;
    int		    localHostAsWell;
    
    Debug ("Indirect respond %d\n", length);
    if (!XdmcpReadARRAYofARRAY8 (&buffer, &queryAuthenticationNames))
	return;
    expectedLen = 1;
    for (i = 0; i < (int)queryAuthenticationNames.length; i++)
	expectedLen += 2 + queryAuthenticationNames.data[i].length;
    if (length == expectedLen)
    {
	ClientAddress (from, &clientAddress, &clientPort, &connectionType);
	/*
	 * set up the forward query packet
	 */
    	header.version = XDM_PROTOCOL_VERSION;
    	header.opcode = (CARD16) FORWARD_QUERY;
    	header.length = 0;
    	header.length += 2 + clientAddress.length;
    	header.length += 2 + clientPort.length;
    	header.length += 1;
    	for (i = 0; i < (int)queryAuthenticationNames.length; i++)
	    header.length += 2 + queryAuthenticationNames.data[i].length;
    	XdmcpWriteHeader (&buffer, &header);
    	XdmcpWriteARRAY8 (&buffer, &clientAddress);
    	XdmcpWriteARRAY8 (&buffer, &clientPort);
    	XdmcpWriteARRAYofARRAY8 (&buffer, &queryAuthenticationNames);

	localHostAsWell = ForEachMatchingIndirectHost (&clientAddress, connectionType, sendForward, (char *) 0);
	
	XdmcpDisposeARRAY8 (&clientAddress);
	XdmcpDisposeARRAY8 (&clientPort);
	if (localHostAsWell)
	    all_query_respond (from, fromlen, &queryAuthenticationNames,
			   INDIRECT_QUERY);
    }
    else
    {
	Debug ("Indirect length error got %d expect %d\n", length, expectedLen);
    }
    XdmcpDisposeARRAYofARRAY8 (&queryAuthenticationNames);
}

static void
ProcessRequestSocket (void)
{
    XdmcpHeader		header;
    struct sockaddr_in	addr;
    int			addrlen = sizeof addr;

    Debug ("ProcessRequestSocket\n");
    bzero ((char *) &addr, sizeof (addr));
    if (!XdmcpFill (xdmcpFd, &buffer, (XdmcpNetaddr)&addr, &addrlen)) {
	Debug ("XdmcpFill failed\n");
	return;
    }
    if (!XdmcpReadHeader (&buffer, &header)) {
	Debug ("XdmcpReadHeader failed\n");
	return;
    }
    if (header.version != XDM_PROTOCOL_VERSION) {
	Debug ("XDMCP header version read was %d, expected %d\n",
	       header.version, XDM_PROTOCOL_VERSION);
	return;
    }
    Debug ("header: %d %d %d\n", header.version, header.opcode, header.length);
    switch (header.opcode)
    {
    case BROADCAST_QUERY:
      broadcast_respond ((struct sockaddr *)&addr, addrlen, header.length);
	break;
    case QUERY:
	query_respond ((struct sockaddr *)&addr, addrlen, header.length);
	break;
    case INDIRECT_QUERY:
	indirect_respond ((struct sockaddr *)&addr, addrlen, header.length);
	break;
    case FORWARD_QUERY:
	forward_respond ((struct sockaddr *)&addr, addrlen, header.length);
	break;
    case REQUEST:
	request_respond ((struct sockaddr *)&addr, addrlen, header.length);
	break;
    case MANAGE:
	manage ((struct sockaddr *)&addr, addrlen, header.length);
	break;
    case KEEPALIVE:
	send_alive ((struct sockaddr *)&addr, addrlen, header.length);
	break;
    }
}

/*
 * dtlogin changes to WaitForSomething () merged in to support command line
 * login.
 */
void WaitForSomething (void)
{
    FD_TYPE	reads;
    struct timeval	timeout, *ptimeout;
    int	nready;
    extern int Rescan, ChildReady, wakeupTime;

    Debug ("WaitForSomething\n");
    if (AnyWellKnownSockets () && !ChildReady || wakeupTime > 0 ) {
	reads = WellKnownSocketsMask;

	if (wakeupTime >= 0 ) {
	    timeout.tv_sec  = wakeupTime;
	    timeout.tv_usec = 10;
	    ptimeout = &timeout;
	    Debug("Setting timer on select() for %d seconds.\n", wakeupTime);
	}
	else
	    ptimeout = NULL;

#ifdef __hpux
	nready = select (WellKnownSocketsMax + 1, (int *) &reads, 0, 0, ptimeout);
#else
	nready = select (WellKnownSocketsMax + 1, &reads, 0, 0, ptimeout);
#endif
	Debug ("select returns %d.  Rescan: %d  ChildReady: %d\n",
		nready, Rescan, ChildReady);
	if (nready > 0)
	{
	    if (xdmcpFd >= 0 && FD_ISSET (xdmcpFd, &reads))
		ProcessRequestSocket ();
	    if (chooserFd >= 0 && FD_ISSET (chooserFd, &reads))
		ProcessChooserSocket (chooserFd);
	}
	if (ChildReady)
	{
	    WaitForChild ();
	}
	else
	    StartDisplays();
    } else
	WaitForChild ();
}

/*
 * respond to a request on the UDP socket.
 */

static ARRAY8	Hostname;

void
registerHostname (char *name, int namelen)
{
    int	i;

    if (!XdmcpReallocARRAY8 (&Hostname, namelen))
	return;
    for (i = 0; i < namelen; i++)
	Hostname.data[i] = name[i];
}

static void
direct_query_respond (struct sockaddr *from, int fromlen, int length, xdmOpCode type)
{
    ARRAYofARRAY8   queryAuthenticationNames;
    int		    expectedLen;
    int		    i;
    
    if (!XdmcpReadARRAYofARRAY8 (&buffer, &queryAuthenticationNames))
	return;
    expectedLen = 1;
    for (i = 0; i < (int)queryAuthenticationNames.length; i++)
	expectedLen += 2 + queryAuthenticationNames.data[i].length;
    if (length == expectedLen)
	all_query_respond (from, fromlen, &queryAuthenticationNames, type);
    XdmcpDisposeARRAYofARRAY8 (&queryAuthenticationNames);
}

void
query_respond (struct sockaddr *from, int fromlen, int length)
{
    Debug ("Query respond %d\n", length);
    direct_query_respond (from, fromlen, length, QUERY);
}

void
broadcast_respond (struct sockaddr *from, int fromlen, int length)
{
    direct_query_respond (from, fromlen, length, BROADCAST_QUERY);
}

/* computes an X display name */
#if NeedWidePrototypes
char *
NetworkAddressToName(int connectionType, ARRAY8Ptr connectionAddress, int displayNumber)
#else
char *
NetworkAddressToName(CARD16 connectionType, ARRAY8Ptr connectionAddress, CARD16 displayNumber)
#endif
{
    switch (connectionType)
    {
    case FamilyInternet:
	{
	    CARD8		*data;
	    struct hostent	*hostent;
	    char		*name;
	    char		*localhost, *localHostname();

	    data = connectionAddress->data;
	    hostent = gethostbyaddr ((char *)data,
				     connectionAddress->length, AF_INET);

	    localhost = localHostname ();

	    /* 
	     * protect against bogus host names 
	     */
	    if (hostent && hostent->h_name && hostent->h_name[0]
			&& (hostent->h_name[0] != '.'))
	    {
		if (!strcmp (localhost, hostent->h_name))
		{
		    if (!getString (name, 10))
			return 0;
		    sprintf (name, ":%d", displayNumber);
		}
		else
		{
		    if (removeDomainname)
		    {
		    	char    *localDot, *remoteDot;
    
			/* check for a common domain name.  This
			 * could reduce names by recognising common
			 * super-domain names as well, but I don't think
			 * this is as useful, and will confuse more
			 * people
 			 */
		    	if ((localDot = strchr(localhost, '.')) &&
		            (remoteDot = strchr(hostent->h_name, '.')))
			{
			    /* smash the name in place; it won't
			     * be needed later.
			     */
			    if (!strcmp (localDot+1, remoteDot+1))
				*remoteDot = '\0';
			}
		    }

		    if (!getString (name, strlen (hostent->h_name) + 10))
			return 0;
		    sprintf (name, "%s:%d", hostent->h_name, displayNumber);
		}
	    }
	    else
	    {
		if (!getString (name, 25))
		    return 0;
		sprintf(name, "%d.%d.%d.%d:%d",
			data[0], data[1], data[2], data[3], displayNumber);
	    }
	    return name;
	}
#ifdef DNET
    case FamilyDECnet:
	return NULL;
#endif /* DNET */
    default:
	return NULL;
    }
}

/*ARGSUSED*/
void
forward_respond (struct sockaddr *from, int fromlen, int length)
{
    ARRAY8	    clientAddress;
    ARRAY8	    clientPort;
    ARRAYofARRAY8   authenticationNames;
    struct sockaddr *client;
    int		    clientlen;
    int		    expectedLen;
    int		    i;
    
    Debug ("Forward respond %d\n", length);
    clientAddress.length = 0;
    clientAddress.data = 0;
    clientPort.length = 0;
    clientPort.data = 0;
    authenticationNames.length = 0;
    authenticationNames.data = 0;
    if (XdmcpReadARRAY8 (&buffer, &clientAddress) &&
	XdmcpReadARRAY8 (&buffer, &clientPort) &&
	XdmcpReadARRAYofARRAY8 (&buffer, &authenticationNames))
    {
	expectedLen = 0;
	expectedLen += 2 + clientAddress.length;
	expectedLen += 2 + clientPort.length;
	expectedLen += 1;	    /* authenticationNames */
	for (i = 0; i < (int)authenticationNames.length; i++)
	    expectedLen += 2 + authenticationNames.data[i].length;
	if (length == expectedLen)
	{
	    int	j;

	    j = 0;
	    for (i = 0; i < (int)clientPort.length; i++)
		j = j * 256 + clientPort.data[i];
	    Debug ("Forward client address (port %d)", j);
	    for (i = 0; i < (int)clientAddress.length; i++)
		Debug (" %d", clientAddress.data[i]);
	    Debug ("\n");
    	    switch (from->sa_family)
    	    {
#ifdef AF_INET
	    case AF_INET:
		{
		    static struct sockaddr_in	in_addr;

		    if (clientAddress.length != 4 ||
		        clientPort.length != 2)
		    {
			goto badAddress;
		    }
		    bzero ((char *) &in_addr, sizeof (in_addr));
#ifdef BSD44SOCKETS
		    in_addr.sin_len = sizeof(in_addr);
#endif
		    in_addr.sin_family = AF_INET;
		    memmove( &in_addr.sin_addr, clientAddress.data, 4);
		    memmove( (char *) &in_addr.sin_port, clientPort.data, 2);
		    client = (struct sockaddr *) &in_addr;
		    clientlen = sizeof (in_addr);
		}
		break;
#endif
#ifdef AF_UNIX
	    case AF_UNIX:
		{
		    static struct sockaddr_un	un_addr;

		    if (clientAddress.length >= sizeof (un_addr.sun_path))
			goto badAddress;
		    bzero ((char *) &un_addr, sizeof (un_addr));
		    un_addr.sun_family = AF_UNIX;
		    memmove( un_addr.sun_path, clientAddress.data, clientAddress.length);
		    un_addr.sun_path[clientAddress.length] = '\0';
		    client = (struct sockaddr *) &un_addr;
#ifdef BSD44SOCKETS
		    un_addr.sun_len = strlen(un_addr.sun_path);
		    clientlen = SUN_LEN(&un_addr);
#else
		    clientlen = sizeof (un_addr);
#endif
		}
		break;
#endif
#ifdef AF_CHAOS
	    case AF_CHAOS:
		goto badAddress;
#endif
#ifdef AF_DECnet
	    case AF_DECnet:
		goto badAddress;
#endif
    	    }
	    all_query_respond (client, clientlen, &authenticationNames,
			       FORWARD_QUERY);
	}
	else
	{
	    Debug ("Forward length error got %d expect %d\n", length, expectedLen);
	}
    }
badAddress:
    XdmcpDisposeARRAY8 (&clientAddress);
    XdmcpDisposeARRAY8 (&clientPort);
    XdmcpDisposeARRAYofARRAY8 (&authenticationNames);
}

void
send_willing (struct sockaddr *from, int fromlen, ARRAY8Ptr authenticationName, ARRAY8Ptr status)
{
    XdmcpHeader	header;

    Debug ("Send willing %*.*s %*.*s\n", authenticationName->length,
					 authenticationName->length,
					 pS(authenticationName->data),
					 status->length,
					 status->length,
					 pS(status->data));
    header.version = XDM_PROTOCOL_VERSION;
    header.opcode = (CARD16) WILLING;
    header.length = 6 + authenticationName->length +
		    Hostname.length + status->length;
    XdmcpWriteHeader (&buffer, &header);
    XdmcpWriteARRAY8 (&buffer, authenticationName);
    XdmcpWriteARRAY8 (&buffer, &Hostname);
    XdmcpWriteARRAY8 (&buffer, status);
    XdmcpFlush (xdmcpFd, &buffer, (XdmcpNetaddr)from, fromlen);
}

void
send_unwilling (struct sockaddr *from, int fromlen, ARRAY8Ptr authenticationName, ARRAY8Ptr status)
{
    XdmcpHeader	header;

    Debug ("Send unwilling %*.*s %*.*s\n", authenticationName->length,
					 authenticationName->length,
					 pS(authenticationName->data),
					 status->length,
					 status->length,
					 pS(status->data));
    header.version = XDM_PROTOCOL_VERSION;
    header.opcode = (CARD16) UNWILLING;
    header.length = 4 + Hostname.length + status->length;
    XdmcpWriteHeader (&buffer, &header);
    XdmcpWriteARRAY8 (&buffer, &Hostname);
    XdmcpWriteARRAY8 (&buffer, status);
    XdmcpFlush (xdmcpFd, &buffer, (XdmcpNetaddr)from, fromlen);
}

static unsigned long	globalSessionID;

#define NextSessionID()    (++globalSessionID)

void
init_session_id(void)
{
    /* Set randomly so we are unlikely to reuse id's from a previous
     * incarnation so we don't say "Alive" to those displays.
     * Start with low digits 0 to make debugging easier.
     */
    globalSessionID = (time((Time_t)0)&0x7fff) * 16000;
}
    
static ARRAY8 outOfMemory = { (CARD16) 13, (CARD8Ptr) "Out of memory" };
static ARRAY8 noValidAddr = { (CARD16) 16, (CARD8Ptr) "No valid address" };
static ARRAY8 noValidAuth = { (CARD16) 22, (CARD8Ptr) "No valid authorization" };
static ARRAY8 noAuthentic = { (CARD16) 29, (CARD8Ptr) "XDM has no authentication key" };

void
request_respond (struct sockaddr *from, int fromlen, int length)
{
    CARD16	    displayNumber;
    ARRAY16	    connectionTypes;
    ARRAYofARRAY8   connectionAddresses;
    ARRAY8	    authenticationName;
    ARRAY8	    authenticationData;
    ARRAYofARRAY8   authorizationNames;
    ARRAY8	    manufacturerDisplayID;
    ARRAY8Ptr	    reason;
    int		    expectlen;
    int		    i, j;
    struct protoDisplay  *pdpy;
    ARRAY8	    authorizationName, authorizationData;
    ARRAY8Ptr	    connectionAddress;

    Debug ("Request respond %d\n", length);
    connectionTypes.data = 0;
    connectionAddresses.data = 0;
    authenticationName.data = 0;
    authenticationData.data = 0;
    authorizationNames.data = 0;
    authorizationName.length = 0;
    authorizationData.length = 0;
    manufacturerDisplayID.data = 0;
    if (XdmcpReadCARD16 (&buffer, &displayNumber) &&
	XdmcpReadARRAY16 (&buffer, &connectionTypes) &&
	XdmcpReadARRAYofARRAY8 (&buffer, &connectionAddresses) &&
	XdmcpReadARRAY8 (&buffer, &authenticationName) &&
	XdmcpReadARRAY8 (&buffer, &authenticationData) &&
	XdmcpReadARRAYofARRAY8 (&buffer, &authorizationNames) &&
	XdmcpReadARRAY8 (&buffer, &manufacturerDisplayID))
    {
	expectlen = 0;
	expectlen += 2;				    /* displayNumber */
	expectlen += 1 + 2*connectionTypes.length;  /* connectionTypes */
	expectlen += 1;				    /* connectionAddresses */
	for (i = 0; i < (int)connectionAddresses.length; i++)
	    expectlen += 2 + connectionAddresses.data[i].length;
	expectlen += 2 + authenticationName.length; /* authenticationName */
	expectlen += 2 + authenticationData.length; /* authenticationData */
	expectlen += 1;				    /* authoriationNames */
	for (i = 0; i < (int)authorizationNames.length; i++)
	    expectlen += 2 + authorizationNames.data[i].length;
	expectlen += 2 + manufacturerDisplayID.length;	/* displayID */
	if (expectlen != length)
	{
	    Debug ("Request length error got %d expect %d\n", length, expectlen);
	    goto abort;
	}
	if (connectionTypes.length == 0 ||
	    connectionAddresses.length != connectionTypes.length)
	{
	    reason = &noValidAddr;
	    pdpy = 0;
	    goto decline;
	}
	pdpy = FindProtoDisplay (from, fromlen, displayNumber);
	if (!pdpy) {

	    /* Check this Display against the Manager's policy */
	    reason = Accept (from, fromlen, displayNumber);
	    if (reason)
		goto decline;

	    /* Check the Display's stream services against Manager's policy */
	    i = SelectConnectionTypeIndex (&connectionTypes,
					   &connectionAddresses);
	    if (i < 0) {
		reason = &noValidAddr;
		goto decline;
	    }
	
	    /* The Manager considers this a new session */
	    connectionAddress = &connectionAddresses.data[i];
	    pdpy = NewProtoDisplay (from, fromlen, displayNumber,
				    connectionTypes.data[i], connectionAddress,
				    NextSessionID());
	    Debug ("NewProtoDisplay 0x%x\n", pdpy);
	    if (!pdpy) {
		reason = &outOfMemory;
		goto decline;
	    }
	}
	if (authorizationNames.length == 0)
	    j = 0;
	else
	    j = SelectAuthorizationTypeIndex (&authenticationName,
					      &authorizationNames);
	if (j < 0)
	{
	    reason = &noValidAuth;
	    goto decline;
	}
	if (!CheckAuthentication (pdpy,
				  &manufacturerDisplayID,
				  &authenticationName,
				  &authenticationData))
	{
	    reason = &noAuthentic;
	    goto decline;
	}
	if (j < (int)authorizationNames.length)
	{
	    Xauth   *auth;
	    SetProtoDisplayAuthorization (pdpy,
		(unsigned short) authorizationNames.data[j].length,
		(char *) authorizationNames.data[j].data);
	    auth = pdpy->xdmcpAuthorization;
	    if (!auth)
		auth = pdpy->fileAuthorization;
	    if (auth)
	    {
		authorizationName.length = auth->name_length;
		authorizationName.data = (CARD8Ptr) auth->name;
		authorizationData.length = auth->data_length;
		authorizationData.data = (CARD8Ptr) auth->data;
	    }
	}
	if (pdpy)
	{
	    send_accept (from, fromlen, pdpy->sessionID,
				        &authenticationName,
					&authenticationData,
					&authorizationName,
					&authorizationData);
	}
	else
	{
decline:    ;
	    send_decline (from, fromlen, &authenticationName,
				 &authenticationData,
				 reason);
            if (pdpy)
		DisposeProtoDisplay (pdpy);
	}
    }
abort:
    XdmcpDisposeARRAY16 (&connectionTypes);
    XdmcpDisposeARRAYofARRAY8 (&connectionAddresses);
    XdmcpDisposeARRAY8 (&authenticationName);
    XdmcpDisposeARRAY8 (&authenticationData);
    XdmcpDisposeARRAYofARRAY8 (&authorizationNames);
    XdmcpDisposeARRAY8 (&manufacturerDisplayID);
}

void
send_accept (struct sockaddr *to, int tolen, CARD32 sessionID, ARRAY8Ptr authenticationName, ARRAY8Ptr authenticationData, ARRAY8Ptr authorizationName, ARRAY8Ptr authorizationData)
{
    XdmcpHeader	header;

    Debug ("Accept Session ID %d\n", sessionID);
    header.version = XDM_PROTOCOL_VERSION;
    header.opcode = (CARD16) ACCEPT;
    header.length = 4;			    /* session ID */
    header.length += 2 + authenticationName->length;
    header.length += 2 + authenticationData->length;
    header.length += 2 + authorizationName->length;
    header.length += 2 + authorizationData->length;
    XdmcpWriteHeader (&buffer, &header);
    XdmcpWriteCARD32 (&buffer, sessionID);
    XdmcpWriteARRAY8 (&buffer, authenticationName);
    XdmcpWriteARRAY8 (&buffer, authenticationData);
    XdmcpWriteARRAY8 (&buffer, authorizationName);
    XdmcpWriteARRAY8 (&buffer, authorizationData);
    XdmcpFlush (xdmcpFd, &buffer, (XdmcpNetaddr)to, tolen);
}

void
send_decline (struct sockaddr *to, int tolen, ARRAY8Ptr authenticationName, ARRAY8Ptr authenticationData, ARRAY8Ptr status)
{
    XdmcpHeader	header;

    Debug ("Decline %*.*s\n", status->length, status->length, pS(status->data));
    header.version = XDM_PROTOCOL_VERSION;
    header.opcode = (CARD16) DECLINE;
    header.length = 0;
    header.length += 2 + status->length;
    header.length += 2 + authenticationName->length;
    header.length += 2 + authenticationData->length;
    XdmcpWriteHeader (&buffer, &header);
    XdmcpWriteARRAY8 (&buffer, status);
    XdmcpWriteARRAY8 (&buffer, authenticationName);
    XdmcpWriteARRAY8 (&buffer, authenticationData);
    XdmcpFlush (xdmcpFd, &buffer, (XdmcpNetaddr)to, tolen);
}

void
manage (struct sockaddr *from, int fromlen, int length)
{
    CARD32		sessionID;
    CARD16		displayNumber;
    ARRAY8		displayClass;
    int			expectlen;
    struct protoDisplay	*pdpy;
    struct display	*d;
    char		*name = NULL;
    char		*class = NULL;
    XdmcpNetaddr	from_save;
    ARRAY8		clientAddress, clientPort;
    CARD16		connectionType;

    Debug ("Manage %d\n", length);
    displayClass.data = 0;
    displayClass.length = 0;
    if (XdmcpReadCARD32 (&buffer, &sessionID) &&
	XdmcpReadCARD16 (&buffer, &displayNumber) &&
	XdmcpReadARRAY8 (&buffer, &displayClass))
    {
	expectlen = 4 +				/* session ID */
		    2 +				/* displayNumber */
		    2 + displayClass.length;	/* displayClass */
	if (expectlen != length)
	{
	    Debug ("Manage length error got %d expect %d\n", length, expectlen);
	    goto abort;
	}
	pdpy = FindProtoDisplay (from, fromlen, displayNumber);
	Debug ("Manage Session ID %d, pdpy 0x%x\n", sessionID, pdpy);
	if (!pdpy || pdpy->sessionID != sessionID)
	{
	    /*
	     * We may have already started a session for this display
	     * but it hasn't seen the response in the form of an
	     * XOpenDisplay() yet. So check if it is in the list of active
	     * displays, and if so check that the session id's match.
	     * If all this is true, then we have a duplicate request that
	     * can be ignored.
	     */
	    if (!pdpy 
		&& (d = FindDisplayByAddress(from, fromlen, displayNumber))
		&& d->sessionID == sessionID) {
		     Debug("manage: got duplicate pkt, ignoring\n");
		     goto abort;
	    }
	    Debug ("Session ID %d refused\n", sessionID);
	    if (pdpy)
		Debug ("Existing Session ID %d\n", pdpy->sessionID);
	    send_refuse (from, fromlen, sessionID);
	}
	else
	{
	    name = NetworkAddressToName (pdpy->connectionType,
					 &pdpy->connectionAddress,
					 pdpy->displayNumber);
	    Debug ("Computed display name: %s\n", name);
	    if (!name)
	    {
		send_failed (from, fromlen, "(no name)", sessionID, "out of memory");
		goto abort;
	    }
	    d = FindDisplayByName (name);
	    if (d)
	    {
		extern void StopDisplay ();

		Debug ("Terminating active session for %s\n", d->name);
		StopDisplay (d);
	    }
	    class = malloc (displayClass.length + 1);
	    if (!class)
	    {
		send_failed (from, fromlen, name, sessionID, "out of memory");
		goto abort;
	    }
	    if (displayClass.length)
	    {
		memmove( class, displayClass.data, displayClass.length);
		class[displayClass.length] = '\0';
	    }
	    else
	    {
		free ((char *) class);
		class = (char *) NULL;
	    }
	    from_save = (XdmcpNetaddr) malloc (fromlen);
	    if (!from_save)
	    {
		send_failed (from, fromlen, name, sessionID, "out of memory");
		goto abort;
	    }
	    memmove( from_save, from, fromlen);
	    d = NewDisplay (name, class);
	    if (!d)
	    {
		free ((char *) from_save);
		send_failed (from, fromlen, name, sessionID, "out of memory");
		goto abort;
	    }
	    d->displayType.location = Foreign;
	    d->displayType.lifetime = Transient;
	    d->displayType.origin = FromXDMCP;
	    d->sessionID = pdpy->sessionID;
	    d->from = (struct sockaddr *)from_save;
	    d->fromlen = fromlen;
	    d->displayNumber = pdpy->displayNumber;
#ifdef BYPASSLOGIN
	    d->bypassLogin = 0;
#endif /* BYPASSLOGIN */
	    ClientAddress (from, &clientAddress, &clientPort, &connectionType);
	    d->useChooser = 0;
	    if (IsIndirectClient (&clientAddress, connectionType))
	    {
		Debug ("IsIndirectClient\n");
		ForgetIndirectClient (&clientAddress, connectionType);
		if (UseChooser (&clientAddress, connectionType))
		{
		    d->useChooser = 1;
		    Debug ("Use chooser for %s\n", d->name);
		}
	    }
	    d->clientAddr = clientAddress;
	    d->connectionType = connectionType;
	    XdmcpDisposeARRAY8 (&clientPort);
	    if (pdpy->fileAuthorization)
	    {
		d->authorizations = (Xauth **) malloc (sizeof (Xauth *));
		if (!d->authorizations)
		{
		    free ((char *) from_save);
		    free ((char *) d);
		    send_failed (from, fromlen, name, sessionID, "out of memory");
		    goto abort;
		}
		d->authorizations[0] = pdpy->fileAuthorization;
		d->authNum = 1;
		pdpy->fileAuthorization = 0;
	    }
	    DisposeProtoDisplay (pdpy);
	    Debug ("Starting display %s,%s\n", d->name, d->class);
	    StartDisplay (d);
	}
    }
abort:
    XdmcpDisposeARRAY8 (&displayClass);
    if (name) free ((char*) name);
    if (class) free ((char*) class);
}

void
SendFailed (struct display *d, char *reason)
{
    Debug ("Display start failed, sending Failed\n");
    send_failed (d->from, d->fromlen, d->name, d->sessionID, reason);
}

void
send_failed (struct sockaddr *from, int fromlen, char *name, CARD32 sessionID, char *reason)
{
    static char	buf[256];
    XdmcpHeader	header;
    ARRAY8	status;

    sprintf (buf, "Session %d failed for display %s: %s",
	     sessionID, name, reason);
    Debug ("Send failed %d %s\n", sessionID, buf);
    status.length = strlen (buf);
    status.data = (CARD8Ptr) buf;
    header.version = XDM_PROTOCOL_VERSION;
    header.opcode = (CARD16) FAILED;
    header.length = 6 + status.length;
    XdmcpWriteHeader (&buffer, &header);
    XdmcpWriteCARD32 (&buffer, sessionID);
    XdmcpWriteARRAY8 (&buffer, &status);
    XdmcpFlush (xdmcpFd, &buffer, (XdmcpNetaddr)from, fromlen);
}

void
send_refuse (struct sockaddr *from, int fromlen, CARD32 sessionID)
{
    XdmcpHeader	header;

    Debug ("Send refuse %d\n", sessionID);
    header.version = XDM_PROTOCOL_VERSION;
    header.opcode = (CARD16) REFUSE;
    header.length = 4;
    XdmcpWriteHeader (&buffer, &header);
    XdmcpWriteCARD32 (&buffer, sessionID);
    XdmcpFlush (xdmcpFd, &buffer, (XdmcpNetaddr)from, fromlen);
}

void
send_alive (struct sockaddr *from, int fromlen, int length)
{
    CARD32		sessionID;
    CARD16		displayNumber;
    struct display	*d;
    XdmcpHeader		header;
    CARD8		sendRunning;
    CARD32		sendSessionID;

    Debug ("Send alive\n");
    if (XdmcpReadCARD16 (&buffer, &displayNumber) &&
	XdmcpReadCARD32 (&buffer, &sessionID))
    {
	if (length == 6)
	{
	    d = FindDisplayBySessionID (sessionID);
	    if (!d) {
		d = FindDisplayByAddress (from, fromlen, displayNumber);
	    }
	    sendRunning = 0;
	    sendSessionID = 0;
	    if (d && d->status == running)
 	    {
		if (d->sessionID == sessionID)
		    sendRunning = 1;
		sendSessionID = d->sessionID;
	    }
	    header.version = XDM_PROTOCOL_VERSION;
	    header.opcode = (CARD16) ALIVE;
	    header.length = 5;
	    Debug ("alive: %d %d\n", sendRunning, sendSessionID);
	    XdmcpWriteHeader (&buffer, &header);
	    XdmcpWriteCARD8 (&buffer, sendRunning);
	    XdmcpWriteCARD32 (&buffer, sendSessionID);
	    XdmcpFlush (xdmcpFd, &buffer, (XdmcpNetaddr)from, fromlen);
	}
    }
}

#if NeedWidePrototypes
char *
NetworkAddressToHostname (int connectionType, ARRAY8Ptr connectionAddress)
#else
char *
NetworkAddressToHostname (CARD16 connectionType, ARRAY8Ptr connectionAddress)
#endif
{
    char    *name = 0;

    switch (connectionType)
    {
    case FamilyInternet:
	{
	    struct hostent	*hostent;
	    char dotted[20];
	    char *local_name;

	    hostent = gethostbyaddr ((char *)connectionAddress->data,
				     connectionAddress->length, AF_INET);

	    if (hostent)
		local_name = hostent->h_name;
	    else {
		/* can't get name, so use emergency fallback */
		sprintf(dotted, "%d.%d.%d.%d",
			connectionAddress->data[0],
			connectionAddress->data[1],
			connectionAddress->data[2],
			connectionAddress->data[3]);
		local_name = dotted;

		LogError ((unsigned char *)"Cannot convert Internet address %s to host name\n",
			  dotted);
	    }
	    if (!getString (name, strlen (local_name)))
		break;
	    strcpy (name, local_name);
	    break;
	}
#ifdef DNET
    case FamilyDECnet:
	break;
#endif /* DNET */
    default:
	break;
    }
    return name;
}

static int
HostnameToNetworkAddress (char *name, CARD16 connectionType, ARRAY8Ptr connectionAddress)
{
    switch (connectionType)
    {
    case FamilyInternet:
	{
	    struct hostent	*hostent;

	    hostent = gethostbyname (name);
	    if (!hostent)
		return FALSE;
	    if (!XdmcpAllocARRAY8 (connectionAddress, hostent->h_length))
		return FALSE;
	    memmove( connectionAddress->data, hostent->h_addr, hostent->h_length);
	    return TRUE;
	}
#ifdef DNET
    case FamilyDECnet:
	return FALSE;
#endif
    }
    return FALSE;
}

/*
 * converts a display name into a network address, using
 * the same rules as XOpenDisplay (algorithm cribbed from there)
 */

static int
NameToNetworkAddress(char *name, CARD16Ptr connectionTypep, ARRAY8Ptr connectionAddress, CARD16Ptr displayNumber)
{
    char    *colon, *display_number;
    char    hostname[1024];
    int	    dnet = FALSE;
    CARD16  number;
    CARD16  connectionType;

    colon = strchr(name, ':');
    if (!colon)
	return FALSE;
    if (colon != name)
    {
	if (colon - name > sizeof (hostname))
	    return FALSE;
	strncpy (hostname, name, colon - name);
	hostname[colon - name] = '\0';
    }
    else
    {
	strcpy (hostname, localHostname ());
    }
    if (colon[1] == ':')
    {
	dnet = TRUE;
	colon++;
    }
#ifndef DNETCONN
    if (dnet)
	return FALSE;
#endif
    display_number = colon + 1;
    while (*display_number && *display_number != '.')
    {
	if (!isascii (*display_number) || !isdigit(*display_number))
	    return FALSE;
    }
    if (display_number == colon + 1)
	return FALSE;
    number = atoi (colon + 1);
#ifdef DNETCONN
    if (dnet)
	connectionType = FamilyDECnet;
    else
#endif
	connectionType = FamilyInternet;
    if (!HostnameToNetworkAddress (hostname, connectionType, connectionAddress))
	return FALSE;
    *displayNumber = number;
    *connectionTypep = connectionType;
    return TRUE;
}

