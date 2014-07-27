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
/* $TOG: socket.c /main/9 1997/12/23 12:25:32 bill $ */
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
 * Revision 1.1.2.3  1995/06/06  20:24:26  Chris_Beute
 * 	Code snapshot merge from March 15 and SIA changes
 * 	[1995/05/31  20:15:01  Chris_Beute]
 *
 * Revision 1.1.2.2  1995/04/21  13:05:38  Peter_Derr
 * 	dtlogin auth key fixes from deltacde
 * 	[1995/04/12  19:21:26  Peter_Derr]
 * 
 * 	R6 xdm version used to pick up XDMCP improvements.
 * 	[1995/04/12  18:05:54  Peter_Derr]
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
 * socket.c - Support for BSD sockets
 */

#include "dm.h"
#include "vgmsg.h"

#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <netdb.h>

#ifdef X_NOT_STDC_ENV
extern int errno;
#endif


extern int	xdmcpFd;
extern int	chooserFd;

extern FD_TYPE	WellKnownSocketsMask;
extern int	WellKnownSocketsMax;

CreateWellKnownSockets ()
{
    struct sockaddr_in	sock_addr;
    char		*name, *localHostname();

    if (request_port == 0)
	    return 0;
    Debug ("creating socket %d\n", request_port);
    xdmcpFd = socket (AF_INET, SOCK_DGRAM, 0);
    if (xdmcpFd == -1) {
	LogError (ReadCatalog(MC_LOG_SET,MC_LOG_FAIL_SOCK,MC_DEF_LOG_FAIL_SOCK),
		  request_port);
	return 0;
    }
    name = localHostname ();
    registerHostname (name, strlen (name));
    RegisterCloseOnFork (xdmcpFd);
    /* zero out the entire structure; this avoids 4.4 incompatibilities */
    bzero ((char *) &sock_addr, sizeof (sock_addr));
#ifdef BSD44SOCKETS
    sock_addr.sin_len = sizeof(sock_addr);
#endif
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons ((short) request_port);
    sock_addr.sin_addr.s_addr = htonl (INADDR_ANY);
    if (bind (xdmcpFd, (struct sockaddr *)&sock_addr, sizeof (sock_addr)) == -1)
    {
	LogError (ReadCatalog(MC_LOG_SET,MC_LOG_ERR_BIND,MC_DEF_LOG_ERR_BIND),
		  request_port, errno);
	close (xdmcpFd);
	xdmcpFd = -1;
	return 0;
    }
    WellKnownSocketsMax = xdmcpFd;
    FD_SET (xdmcpFd, &WellKnownSocketsMask);

    chooserFd = socket (AF_INET, SOCK_STREAM, 0);
    Debug ("Created chooser socket %d\n", chooserFd);
    if (chooserFd == -1)
    {
	LogError ((unsigned char *)"chooser socket creation failed, errno %d\n", errno);
	return 0;
    }
    listen (chooserFd, 5);
    if (chooserFd > WellKnownSocketsMax)
	WellKnownSocketsMax = chooserFd;
    FD_SET (chooserFd, &WellKnownSocketsMask);
}

GetChooserAddr (addr, lenp)
    char	*addr;
    int		*lenp;
{
    struct sockaddr_in	in_addr;
    int			len;

    len = sizeof in_addr;
#if defined (_AIX) && (OSMAJORVERSION==4) && (OSMINORVERSION==2)
    if (getsockname (chooserFd, (struct sockaddr *)&in_addr, (size_t *)&len) < 0)
#else
    if (getsockname (chooserFd, (struct sockaddr *)&in_addr, &len) < 0)
#endif
	return -1;
    Debug ("Chooser socket port: %d\n", ntohs(in_addr.sin_port));
    memmove( addr, (char *) &in_addr, len);
    *lenp = len;
    return 0;
}

