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
/* $TOG: auth.c /main/6 1997/03/14 13:44:25 barstow $ */
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
 * Revision 1.1.2.3  1995/06/06  20:21:58  Chris_Beute
 * 	Code snapshot merge from March 15 and SIA changes
 * 	[1995/05/31  20:09:58  Chris_Beute]
 *
 * Revision 1.1.2.2  1995/04/21  13:05:15  Peter_Derr
 * 	dtlogin auth key fixes from deltacde
 * 	[1995/04/12  19:20:51  Peter_Derr]
 * 
 * 	R6 version of auth.c to handle multiple authentication protocols.
 * 	[1995/04/12  18:05:30  Peter_Derr]
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
 * auth.c
 *
 * maintain the authorization generation daemon
 */

#include "dm.h"
#include "vgmsg.h"

#include <X11/X.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <errno.h>
#ifdef X_NOT_STDC_ENV
extern int errno;
#endif

#include <sys/socket.h>
#ifndef ESIX
# include <sys/ioctl.h>
#endif /* !ESIX */

#ifdef TCPCONN
# include <netinet/in.h>
#endif
#ifdef DNETCONN
# include <netdnet/dn.h>
# include <netdnet/dnetdb.h>
#endif

#if (defined(_POSIX_SOURCE) && !defined(AIXV3)) || defined(hpux) || defined(USG) || defined(SVR4)
#define NEED_UTSNAME
#include <sys/utsname.h>
#endif

#if defined(SYSV) && defined(SYSV386)
# include <sys/stream.h>
# ifdef ISC
#  include <sys/sioctl.h>
# endif /* ISC */
# ifdef ESIX
#  include <lan/net_ioctl.h>
# endif /* ESIX */
#endif /* SYSV386 */

#ifdef SVR4
# include <netdb.h>
# include <sys/sockio.h>
#ifdef USL
# include <sys/stropts.h>
#endif
#endif
#ifdef __convex__
# include <sync/queue.h>
# include <sync/sema.h>
#endif
#include <net/if.h>

#ifdef SECURE_RPC
extern int	SecureRPCInitAuth ();
extern Xauth	*SecureRPCGetAuth ();
#endif

#ifdef K5AUTH
extern int	Krb5InitAuth ();
extern Xauth	*Krb5GetAuth ();
#endif

struct AuthProtocol {
    unsigned short  name_length;
    char	    *name;

    int		    (*InitAuth)(
#if NeedWidePrototypes
			unsigned int name_len,
#else
			unsigned short name_len,
#endif /* NeedWidePrototypes */
			char *name) ;

    Xauth	    *(*GetAuth)(
#if NeedWidePrototypes
			unsigned int namelen,
#else
			unsigned short namelen,
#endif /* NeedWidePrototypes */
			char *name) ;

    void	    (*GetXdmcpAuth)();
    int		    inited;
};

static struct AuthProtocol AuthProtocols[] = {
{ (unsigned short) 18,	"MIT-MAGIC-COOKIE-1",
    MitInitAuth, MitGetAuth, NULL
},
#ifdef HASXDMAUTH
{ (unsigned short) 19,	"XDM-AUTHORIZATION-1",
    XdmInitAuth, XdmGetAuth, XdmGetXdmcpAuth,
},
#endif
#ifdef SECURE_RPC
{ (unsigned short) 9, "SUN-DES-1",
    SecureRPCInitAuth, SecureRPCGetAuth, NULL,
},
#endif
#ifdef K5AUTH
{ (unsigned short) 14, "MIT-KERBEROS-5",
    Krb5InitAuth, Krb5GetAuth, NULL,
},
#endif
};

#define NUM_AUTHORIZATION (sizeof (AuthProtocols) / sizeof (AuthProtocols[0]))

static struct AuthProtocol *
findProtocol (unsigned short name_length, const char *name)
{
    int	i;

    for (i = 0; i < NUM_AUTHORIZATION; i++)
	if (AuthProtocols[i].name_length == name_length &&
	    memcmp(AuthProtocols[i].name, name, name_length) == 0)
	{
	    return &AuthProtocols[i];
	}
    return (struct AuthProtocol *) 0;
}


#if NeedWidePrototypes
int
ValidAuthorization (unsigned int name_length, char *name)
#else
int
ValidAuthorization (unsigned short name_length, char *name)
#endif /* NeedWidePrototypes */
{
    if (findProtocol (name_length, name))
	return TRUE;
    return FALSE;
}

static Xauth *
GenerateAuthorization (unsigned short name_length, char	*name)
{
    struct AuthProtocol	*a;
    Xauth   *auth = 0;
    int	    i;

    Debug ("GenerateAuthorization %*.*s\n",
	    name_length, name_length, name);
    a = findProtocol (name_length, name);
    if (a)
    {
	if (!a->inited)
	{
	    (*a->InitAuth) (name_length, name);
	    a->inited = TRUE;
	}
	auth = (*a->GetAuth) (name_length, name);
	if (auth)
	{
	    Debug ("Got 0x%x (%d %*.*s) ", auth,
		auth->name_length, auth->name_length,
 		auth->name_length, auth->name);
	    for (i = 0; i < (int)auth->data_length; i++)
		Debug (" %02x", auth->data[i] & 0xff);
	    Debug ("\n");
	}
	else
	    Debug ("Got (null)\n");
    }
    else
    {
	Debug ("Unknown authorization %*.*s\n", name_length, name_length, name);
    }
    return auth;
}

#if NeedWidePrototypes
void
SetProtoDisplayAuthorization (struct protoDisplay *pdpy, unsigned int authorizationNameLen, char *authorizationName)
#else
void
SetProtoDisplayAuthorization (struct protoDisplay *pdpy, unsigned short	authorizationNameLen, char *authorizationName)
#endif /* NeedWidePrototypes */
{
    struct AuthProtocol	*a;
    Xauth   *auth;

    a = findProtocol (authorizationNameLen, authorizationName);
    pdpy->xdmcpAuthorization = pdpy->fileAuthorization = 0;
    if (a)
    {
	if (!a->inited)
	{
	    (*a->InitAuth) (authorizationNameLen, authorizationName);
	    a->inited = TRUE;
	}
	if (a->GetXdmcpAuth)
	{
	    (*a->GetXdmcpAuth) (pdpy, authorizationNameLen, authorizationName);
	    auth = pdpy->xdmcpAuthorization;
	}
	else
	{
	    auth = (*a->GetAuth) (authorizationNameLen, authorizationName);
	    pdpy->fileAuthorization = auth;
	    pdpy->xdmcpAuthorization = 0;
	}
	if (auth)
	    Debug ("Got 0x%x (%d %*.*s)\n", auth,
		auth->name_length, auth->name_length,
 		auth->name_length, auth->name);
	else
	    Debug ("Got (null)\n");
    }
}

void
CleanUpFileName (char *src, char *dst, int len)
{
    while (*src) {
	if (--len <= 0)
		break;
	switch (*src & 0x7f)
	{
	case '/':
	    *dst++ = '_';
	    break;
	case '-':
	    *dst++ = '.';
	    break;
	default:
	    *dst++ = (*src & 0x7f);
	}
	++src;
    }
    *dst = '\0';
}

static char authdir1[] = "authdir";
static char authdir2[] = "authfiles";

static int
MakeServerAuthFile (struct display *d)
{
    int len;
#ifdef SYSV
#define NAMELEN	14
#else
#define NAMELEN	255
#endif
    char    cleanname[NAMELEN];
    int r;
    struct stat	statb;

    if (d->clientAuthFile && *d->clientAuthFile)
	len = strlen (d->clientAuthFile) + 1;
    else
    {
    	CleanUpFileName (d->name, cleanname, NAMELEN - 8);
    	len = strlen (authDir) + strlen (authdir1) + strlen (authdir2)
	    + strlen (cleanname) + 14;
    }
    if (d->authFile)
	free (d->authFile);
    d->authFile = malloc ((unsigned) len);
    if (!d->authFile)
	return FALSE;
    if (d->clientAuthFile && *d->clientAuthFile)
	strcpy (d->authFile, d->clientAuthFile);
    else
    {
	sprintf (d->authFile, "%s/%s", authDir, authdir1);
	r = stat(d->authFile, &statb);
	if (r == 0) {
	    if (statb.st_uid != 0) {
		if(-1 == chown(d->authFile, 0, statb.st_gid)) {
                    perror(strerror(errno));
                    return FALSE;
                }
            }
	    if ((statb.st_mode & 0077) != 0) {
		if(-1 == chmod(d->authFile, statb.st_mode & 0700)) {
                    perror(strerror(errno));
                    return FALSE;
                }
            }
	} else {
	    if (errno == ENOENT)
		r = mkdir(d->authFile, 0700);
	    if (r < 0) {
		free (d->authFile);
		d->authFile = NULL;
		return FALSE;
	    }
	}
	sprintf (d->authFile, "%s/%s/%s", authDir, authdir1, authdir2);
	r = mkdir(d->authFile, 0700);
	if (r < 0  &&  errno != EEXIST) {
	    free (d->authFile);
	    d->authFile = NULL;
	    return FALSE;
	}
    	sprintf (d->authFile, "%s/%s/%s/A%s-XXXXXX",
		 authDir, authdir1, authdir2, cleanname);
    	(void) mktemp (d->authFile);
    }
    return TRUE;
}

int
SaveServerAuthorizations (struct display *d, Xauth **auths, int count)
{
    FILE	*auth_file;
    int		mask;
    int		ret;
    int		i;

    mask = umask (0077);
    if (!d->authFile && !MakeServerAuthFile (d))
	return FALSE;
    (void) unlink (d->authFile);
    auth_file = fopen (d->authFile, "w");
    umask (mask);
    if (!auth_file) {
	Debug ("Can't creat auth file %s\n", d->authFile);
	LogError (ReadCatalog(MC_LOG_SET,MC_LOG_SRV_OPEN,MC_DEF_LOG_SRV_OPEN),
		  d->authFile);
	free (d->authFile);
	d->authFile = NULL;
	ret = FALSE;
    }
    else
    {
    	Debug ("File: %s auth: %x\n", d->authFile, auths);
	ret = TRUE;
	for (i = 0; i < count; i++)
	{
	    /*
	     * User-based auths may not have data until
	     * a user logs in.  In which case don't write
	     * to the auth file so xrdb and setup programs don't fail.
	     */
	    if (auths[i]->data_length > 0)
		if (!XauWriteAuth (auth_file, auths[i]) ||
		    fflush (auth_file) == EOF)
		{
		    LogError (
		      ReadCatalog(MC_LOG_SET,MC_LOG_SRV_WRT,MC_DEF_LOG_SRV_WRT),
			  d->authFile);
		    ret = FALSE;
		    free (d->authFile);
		    d->authFile = NULL;
		}
    	}
	fclose (auth_file);
    }
    return ret;
}

void
SetLocalAuthorization (struct display *d)
{
    Xauth	*auth, **auths;
    int		i, j;

    if (d->authorizations)
    {
	for (i = 0; i < d->authNum; i++)
	    XauDisposeAuth (d->authorizations[i]);
	free ((char *) d->authorizations);
	d->authorizations = (Xauth **) NULL;
	d->authNum = 0;
    }
    if (!d->authNames)
	return;
    for (i = 0; d->authNames[i]; i++)
	;
    d->authNameNum = i;
    if (d->authNameLens)
	free ((char *) d->authNameLens);
    d->authNameLens = (unsigned short *) malloc
				(d->authNameNum * sizeof (unsigned short));
    if (!d->authNameLens)
	return;
    for (i = 0; i < d->authNameNum; i++)
	d->authNameLens[i] = strlen (d->authNames[i]);
    auths = (Xauth **) malloc (d->authNameNum * sizeof (Xauth *));
    if (!auths)
	return;
    j = 0;
    for (i = 0; i < d->authNameNum; i++)
    {
	auth = GenerateAuthorization (d->authNameLens[i], d->authNames[i]);
	if (auth)
	    auths[j++] = auth;
    }
    if (SaveServerAuthorizations (d, auths, j))
    {
	d->authorizations = auths;
	d->authNum = j;
    }
    else
    {
	for (i = 0; i < j; i++)
	    XauDisposeAuth (auths[i]);
	free ((char *) auths);
    }
}

/*
 * Set the authorization to use for xdm's initial connection
 * to the X server.  Cannot use user-based authorizations
 * because no one has logged in yet, so we don't have any
 * user credentials.
 * Well, actually we could use SUN-DES-1 because we tell the server
 * to allow root in.  This is bogus and should be fixed.
 */
void 
SetAuthorization (struct display *d)
{
    register Xauth **auth = d->authorizations;
    int i;

    for (i = 0; i < d->authNum; i++)
    {
	if (auth[i]->name_length == 9 &&
	    memcmp(auth[i]->name, "SUN-DES-1", 9) == 0)
	    continue;
	if (auth[i]->name_length == 14 &&
	    memcmp(auth[i]->name, "MIT-KERBEROS-5", 14) == 0)
	    continue;
	XSetAuthorization (auth[i]->name, (int) auth[i]->name_length,
			   auth[i]->data, (int) auth[i]->data_length);
    }
}

static int
openFiles (char *name, char *new_name, FILE **oldp, FILE **newp)
{
	int	mask;
	int	null_data = 0;

	strcpy (new_name, name);
	strcat (new_name, "-n");
	mask = umask (0077);
	(void) unlink (new_name);
	*newp = fopen (new_name, "w");
	(void) umask (mask);
	if (!*newp) {
		Debug ("can't open new file %s\n", new_name);
		return 0;
	}
	/*
	 * Make sure that the device is not 100% full by actually writing
	 * to the file.
	 */
	if ((sizeof(int) != fwrite(&null_data, 1, sizeof(int), *newp))
	   || fflush(*newp)) {
		Debug ("can't write to new file %s\n", new_name);
		fclose(*newp);
		(void) unlink (new_name);
		return 0;
	}
	rewind(*newp);
	*oldp = fopen (name, "r");
	Debug ("opens succeeded %s %s\n", name, new_name);
	return 1;
}

int
binaryEqual (char *a, char *b, unsigned short len)
{
	while (len-- > 0)
		if (*a++ != *b++)
			return 0;
	return 1;
}

static void
dumpBytes (unsigned short len, char *data)
{
	unsigned short	i;

	Debug ("%d: ", len);
	for (i = 0; i < len; i++)
		Debug ("%02x ", data[i] & 0377);
	Debug ("\n");
}

static void
dumpAuth (Xauth	*auth)
{
	Debug ("family: %d\n", auth->family);
	Debug ("addr:   ");
	dumpBytes (auth->address_length, auth->address);
	Debug ("number: ");
	dumpBytes (auth->number_length, auth->number);
	Debug ("name:   ");
	dumpBytes (auth->name_length, auth->name);
	Debug ("data:   ");
	dumpBytes (auth->data_length, auth->data);
}

struct addrList {
	unsigned short	family;
	unsigned short	address_length;
	char	*address;
	unsigned short	number_length;
	char	*number;
	unsigned short	name_length;
	char	*name;
	struct addrList	*next;
};

static struct addrList	*addrs;

static void
initAddrs (void)
{
	addrs = 0;
}

static void
doneAddrs (void)
{
	struct addrList	*a, *n;
	for (a = addrs; a; a = n) {
		n = a->next;
		if (a->address)
			free (a->address);
		if (a->number)
			free (a->number);
		free ((char *) a);
	}
}

static int checkEntry (Xauth *auth);

static void
saveEntry (Xauth *auth)
{
	struct addrList	*new;

	new = (struct addrList *) malloc (sizeof (struct addrList));
	if (!new) {
		LogOutOfMem (ReadCatalog(
                        MC_LOG_SET,MC_LOG_SAVE_ADDR,MC_DEF_LOG_SAVE_ADDR));
		return;
	}
	if ((new->address_length = auth->address_length) > 0) {
		new->address = malloc (auth->address_length);
		if (!new->address) {
			LogOutOfMem (ReadCatalog(
                            MC_LOG_SET,MC_LOG_SAVE_ADDR,MC_DEF_LOG_SAVE_ADDR));
			free ((char *) new);
			return;
		}
		memmove( new->address, auth->address, (int) auth->address_length);
	} else
		new->address = 0;
	if ((new->number_length = auth->number_length) > 0) {
		new->number = malloc (auth->number_length);
		if (!new->number) {
			LogOutOfMem (ReadCatalog(
                            MC_LOG_SET,MC_LOG_SAVE_ADDR,MC_DEF_LOG_SAVE_ADDR));
			free (new->address);
			free ((char *) new);
			return;
		}
		memmove( new->number, auth->number, (int) auth->number_length);
	} else
		new->number = 0;
	if ((new->name_length = auth->name_length) > 0) {
		new->name = malloc (auth->name_length);
		if (!new->name) {
			LogOutOfMem (ReadCatalog(
                            MC_LOG_SET,MC_LOG_SAVE_ADDR,MC_DEF_LOG_SAVE_ADDR));
			free (new->number);
			free (new->address);
			free ((char *) new);
			return;
		}
		memmove( new->name, auth->name, (int) auth->name_length);
	} else
		new->name = 0;
	new->family = auth->family;
	new->next = addrs;
	addrs = new;
}

static int
checkEntry (Xauth *auth)
{
	struct addrList	*a;

	for (a = addrs; a; a = a->next) {
		if (a->family == auth->family &&
		    a->address_length == auth->address_length &&
 		    binaryEqual (a->address, auth->address, auth->address_length) &&
		    a->number_length == auth->number_length &&
 		    binaryEqual (a->number, auth->number, auth->number_length) &&
		    a->name_length == auth->name_length &&
		    binaryEqual (a->name, auth->name, auth->name_length))
		{
			return 1;
		}
	}
	return 0;
}

static int  doWrite;

static
writeAuth (file, auth)
    FILE	*file;
    Xauth	*auth;
{
    if (debugLevel >= 15) {	/* normally too verbose */
        Debug ("writeAuth: doWrite = %d\n", doWrite);
	dumpAuth (auth);	/* does Debug only */
    }
    
    if (doWrite) {
        if (!XauWriteAuth (file, auth) || fflush (file) == EOF)  {
                LogError(
                    ReadCatalog(MC_LOG_SET,MC_LOG_SRV_WRT,MC_DEF_LOG_SRV_WRT),
                    file);
                return 0;
        }
        else
        {
                return 1;
        }
    }
    return 1;
}

static void
writeAddr (int family, int addr_length, char *addr, FILE *file, Xauth *auth)
{
	auth->family = (unsigned short) family;
	auth->address_length = addr_length;
	auth->address = addr;
	Debug ("writeAddr: writing and saving an entry\n");
	writeAuth (file, auth);
	saveEntry (auth);
}

static void
DefineLocal (FILE *file, Xauth *auth)
{
	char	displayname[100];

	/* stolen from xinit.c */

/* Make sure this produces the same string as _XGetHostname in lib/X/XlibInt.c.
 * Otherwise, Xau will not be able to find your cookies in the Xauthority file.
 *
 * Note: POSIX says that the ``nodename'' member of utsname does _not_ have
 *       to have sufficient information for interfacing to the network,
 *       and so, you may be better off using gethostname (if it exists).
 */

#ifdef NEED_UTSNAME

	/* hpux:
	 * Why not use gethostname()?  Well, at least on my system, I've had to
	 * make an ugly kernel patch to get a name longer than 8 characters, and
	 * uname() lets me access to the whole string (it smashes release, you
	 * see), whereas gethostname() kindly truncates it for me.
	 */
	{
	struct utsname name;

	uname(&name);
	strcpy(displayname, name.nodename);
	}
	writeAddr (FamilyLocal, strlen (displayname), displayname, file, auth);
#endif

#if (!defined(NEED_UTSNAME) || defined (hpux))
        /* AIXV3:
	 * In AIXV3, _POSIX_SOURCE is defined, but uname gives only first
	 * field of hostname. Thus, we use gethostname instead.
	 */

	/*
	 * For HP-UX, HP's Xlib expects a fully-qualified domain name, which
	 * is achieved by using gethostname().  For compatability, we must
	 * also still create the entry using uname() above.
	 */

	gethostname(displayname, sizeof(displayname));
	writeAddr (FamilyLocal, strlen (displayname), displayname, file, auth);
#endif
}

#ifdef USL
/* Deal with different SIOCGIFCONF ioctl semantics on UnixWare */
static int
ifioctl (int fd, int cmd, char *arg)
{
    struct strioctl ioc;
    int ret;

    bzero((char *) &ioc, sizeof(ioc));
    ioc.ic_cmd = cmd;
    ioc.ic_timout = 0;
    if (cmd == SIOCGIFCONF)
    {
	ioc.ic_len = ((struct ifconf *) arg)->ifc_len;
	ioc.ic_dp = ((struct ifconf *) arg)->ifc_buf;
    }
    else
    {
	ioc.ic_len = sizeof(struct ifreq);
	ioc.ic_dp = arg;
    }
    ret = ioctl(fd, I_STR, (char *) &ioc);
    if (ret >= 0 && cmd == SIOCGIFCONF)
	((struct ifconf *) arg)->ifc_len = ioc.ic_len;
    return(ret);
}
#endif /* USL */


#ifdef WINTCP /* NCR with Wollongong TCP */

#include <sys/un.h>
#include <stropts.h>
#include <tiuser.h>

#include <sys/stream.h>
#include <net/if.h>
#include <netinet/ip.h>
#include <netinet/ip_var.h>
#include <netinet/in.h>
#include <netinet/in_var.h>

static void
DefineSelf (int fd, FILE *file, Xauth *auth)
{
    /*
     * The Wolongong drivers used by NCR SVR4/MP-RAS don't understand the
     * socket IO calls that most other drivers seem to like. Because of
     * this, this routine must be special cased for NCR. Eventually,
     * this will be cleared up.
     */

    struct ipb ifnet;
    struct in_ifaddr ifaddr;
    struct strioctl str;
    unsigned char *addr;
    int	family, len, ipfd;

    if ((ipfd = open ("/dev/ip", O_RDWR, 0 )) < 0)
        LogError ((unsigned char *)"Getting interface configuration");

    /* Indicate that we want to start at the begining */
    ifnet.ib_next = (struct ipb *) 1;

    while (ifnet.ib_next)
    {
	str.ic_cmd = IPIOC_GETIPB;
	str.ic_timout = 0;
	str.ic_len = sizeof (struct ipb);
	str.ic_dp = (char *) &ifnet;

	if (ioctl (ipfd, (int) I_STR, (char *) &str) < 0)
	{
	    close (ipfd);
	    LogError ((unsigned char *) "Getting interface configuration");
	}

	ifaddr.ia_next = (struct in_ifaddr *) ifnet.if_addrlist;
	str.ic_cmd = IPIOC_GETINADDR;
	str.ic_timout = 0;
	str.ic_len = sizeof (struct in_ifaddr);
	str.ic_dp = (char *) &ifaddr;

	if (ioctl (ipfd, (int) I_STR, (char *) &str) < 0)
	{
	    close (ipfd);
	    LogError ((unsigned char *) "Getting interface configuration");
	}

	/*
	 * Ignore the 127.0.0.1 entry.
	 */
	if (IA_SIN(&ifaddr)->sin_addr.s_addr == htonl(0x7f000001) )
		continue;

	writeAddr (FamilyInternet, 4, (char *)&(IA_SIN(&ifaddr)->sin_addr), file, auth);
 
    }
    close(ipfd);

}

#else /* WINTCP */

#ifdef SIOCGIFCONF
#ifdef __osf__
#define DECnetInstalled (0 == access("/usr/shlib/libdnet.so", F_OK))
#else
    /* think of something... */
#define DECnetInstalled True
#endif /* __osf __ */

/* Define this host for access control.  Find all the hosts the OS knows about 
 * for this fd and add them to the selfhosts list.
 */
static void
#ifdef __osf__
DefineSelf (fd, file, auth, addr_family)
#else
DefineSelf (fd, file, auth)
#endif /* __osf__*/
    int fd;
    FILE	*file;
    Xauth	*auth;
{
    char		buf[2048];
    struct ifconf	ifc;
    register int	n;
    int 		len;
    char 		*addr;
    int 		family;
    register struct ifreq *ifr;
    
    ifc.ifc_len = sizeof (buf);
    ifc.ifc_buf = buf;

#ifdef USL
    if (ifioctl (fd, SIOCGIFCONF, (char *) &ifc) < 0)
#else
    if (ioctl (fd, SIOCGIFCONF, (char *) &ifc) < 0)
#endif
#ifdef __osf__
	switch (addr_family)
	    {
#ifdef TCPCONN
	    case AF_INET:
		perror("xdm");
		LogError ((unsigned char *)"Trouble getting Internet network interface configuration\n");
		break;
#endif
#ifdef DNETCONN
	    case AF_DECnet:
		if (DECnetInstalled)
		    LogError ((unsigned char *)"Trouble getting DECnet network interface configuration\n");
		break;
#endif
	    default:
		LogError ((unsigned char *)"Trouble getting network interface configuration\n");
	    }
#else
        LogError ((unsigned char *)"Trouble getting network interface configuration");
#endif /* __osf__ */

    for (ifr = ifc.ifc_req
#if defined(BSD44SOCKETS) || defined(CSRG_BASED)
	 ; (char *)ifr < ifc.ifc_buf + ifc.ifc_len;
	 ifr = (struct ifreq *)((char *)ifr + sizeof (struct ifreq) +
		   (ifr->ifr_addr.sa_len > sizeof (ifr->ifr_addr) ?
		    ifr->ifr_addr.sa_len - sizeof (ifr->ifr_addr) : 0))
#else
	 , n = ifc.ifc_len / sizeof (struct ifreq); --n >= 0; ifr++
#endif
	 )
    {
#ifdef DNETCONN
	/*
	 * this is ugly but SIOCGIFCONF returns decnet addresses in
	 * a different form from other decnet calls
	 */
	if (ifr->ifr_addr.sa_family == AF_DECnet) {
		len = sizeof (struct dn_naddr);
		addr = (char *)ifr->ifr_addr.sa_data;
		family = FamilyDECnet;
	} else
#endif
	{
	    if (ConvertAddr ((XdmcpNetaddr) &ifr->ifr_addr, &len, &addr) < 0)
		continue;
	    if (len == 0)
 	    {
		Debug ("Skipping zero length address\n");
		continue;
	    }
	    /*
	     * don't write out 'localhost' entries, as
	     * they may conflict with other local entries.
	     * DefineLocal will always be called to add
	     * the local entry anyway, so this one can
	     * be tossed.
	     */
	    if (len == 4 &&
		addr[0] == 127 && addr[1] == 0 &&
		addr[2] == 0 && addr[3] == 1)
	    {
		    Debug ("Skipping localhost address\n");
		    continue;
	    }
	    family = FamilyInternet;
	}
	Debug ("DefineSelf: write network address, length %d\n", len);
	writeAddr (family, len, addr, file, auth);
    }
}

#else /* SIOCGIFCONF */

/* Define this host for access control.  Find all the hosts the OS knows about 
 * for this fd and add them to the selfhosts list.
 */
static void
DefineSelf (fd, file, auth)
    int fd;
{
    register int n;
    int	len;
    caddr_t	addr;
    int		family;

    struct utsname name;
    register struct hostent  *hp;

    union {
	struct  sockaddr   sa;
	struct  sockaddr_in  in;
    } saddr;
	
    struct	sockaddr_in	*inetaddr;

    /* hpux:
     * Why not use gethostname()?  Well, at least on my system, I've had to
     * make an ugly kernel patch to get a name longer than 8 characters, and
     * uname() lets me access to the whole string (it smashes release, you
     * see), whereas gethostname() kindly truncates it for me.
     */
    uname(&name);
    hp = gethostbyname (name.nodename);
    if (hp != NULL) {
	saddr.sa.sa_family = hp->h_addrtype;
	inetaddr = (struct sockaddr_in *) (&(saddr.sa));
	memmove( (char *) &(inetaddr->sin_addr), (char *) hp->h_addr, (int) hp->h_length);
	family = ConvertAddr ( &(saddr.sa), &len, &addr);
	if ( family >= 0) {
	    writeAddr (FamilyInternet, sizeof (inetaddr->sin_addr),
			(char *) (&inetaddr->sin_addr), file, auth);
	}
    }
}

#endif /* SIOCGIFCONF else */

#endif /* WINTCP */


static void
setAuthNumber (auth, name)
    Xauth   *auth;
    char    *name;
{
    char	*colon;
    char	*dot, *number;

    Debug ("setAuthNumber %s\n", name);
    colon = strrchr(name, ':');
    if (colon) {
	++colon;
	dot = strchr(colon, '.');
	if (dot)
	    auth->number_length = dot - colon;
	else
	    auth->number_length = strlen (colon);
	number = malloc (auth->number_length + 1);
	if (number) {
	    strncpy (number, colon, auth->number_length);
	    number[auth->number_length] = '\0';
	} else {
	    LogOutOfMem (ReadCatalog(
                MC_LOG_SET,MC_LOG_NET_CFG,MC_DEF_LOG_NET_CFG));
	    auth->number_length = 0;
	}
	auth->number = number;
	Debug ("setAuthNumber: %s\n", number);
    }
}

static void
writeLocalAuth (FILE *file, Xauth *auth, char *name)
{
    int	fd;

    Debug ("writeLocalAuth: %s %.*s\n", name, auth->name_length, auth->name);
    setAuthNumber (auth, name);
#ifdef STREAMSCONN
    fd = t_open ("/dev/tcp", O_RDWR, 0);
    t_bind(fd, NULL, NULL);
    DefineSelf (fd, file, auth);
    t_unbind (fd);
    t_close (fd);
#endif
#ifdef TCPCONN
    fd = socket (AF_INET, SOCK_STREAM, 0);
    DefineSelf (fd, file, auth);
    close (fd);
#endif
#ifdef DNETCONN
    fd = socket (AF_DECnet, SOCK_STREAM, 0);
    DefineSelf (fd, file, auth);
    close (fd);
#endif
    DefineLocal (file, auth);
}

#ifdef XDMCP

static void
writeRemoteAuth (FILE *file, Xauth *auth, XdmcpNetaddr peer, int peerlen, char *name)
{
    int	    family = FamilyLocal;
    char    *addr;
    
    Debug ("writeRemoteAuth: %s %.*s\n", name, auth->name_length, auth->name);
    if (!peer || peerlen < 2)
	return;
    setAuthNumber (auth, name);
    family = ConvertAddr (peer, &peerlen, &addr);
    Debug ("writeRemoteAuth: family %d\n", family);
    if (family != FamilyLocal)
    {
	Debug ("writeRemoteAuth: %d, %d, %x\n",
		family, peerlen, *(int *)addr);
	writeAddr (family, peerlen, addr, file, auth);
    }
    else
    {
	writeLocalAuth (file, auth, name);
    }
}

#endif /* XDMCP */

void
SetUserAuthorization (struct display *d, struct verify_info *verify)
{
    FILE	*old, *new;
    char	home_name[1024], backup_name[1024], new_name[1024];
    char	*name;
    char	*home;
    char	*envname = 0;
    int	lockStatus;
    Xauth	*entry, **auths;
    int	setenv;
    char	**setEnv (), *getEnv ();
    struct stat	statb;
    int		i;
    int		magicCookie;
    int		data_len;

    Debug ("SetUserAuthorization\n");
    auths = d->authorizations;
    if (auths) {
	home = getEnv (verify->userEnviron, "HOME");
	lockStatus = LOCK_ERROR;
	if (home) {
	    strcpy (home_name, home);
	    if (home[strlen(home) - 1] != '/')
		strcat (home_name, "/");
	    strcat (home_name, ".Xauthority");
	    Debug ("XauLockAuth %s\n", home_name);
	    lockStatus = XauLockAuth (home_name, 1, 2, 10);
	    Debug ("Lock is %d\n", lockStatus);
	    if (lockStatus == LOCK_SUCCESS) {
		if (openFiles (home_name, new_name, &old, &new)) {
		    name = home_name;
		    setenv = 0;
		} else {
		    Debug ("openFiles failed\n");
		    XauUnlockAuth (home_name);
		    lockStatus = LOCK_ERROR;
		}	
	    }
	}
	if (lockStatus != LOCK_SUCCESS) {
	    sprintf (backup_name, "%s/.XauthXXXXXX", d->userAuthDir);
	    (void) mktemp (backup_name);
	    Debug ("XauLockAuth %s\n", backup_name);
	    lockStatus = XauLockAuth (backup_name, 1, 2, 10);
	    Debug ("backup lock is %d\n", lockStatus);
	    if (lockStatus == LOCK_SUCCESS) {
		if (openFiles (backup_name, new_name, &old, &new)) {
		    name = backup_name;
		    setenv = 1;
		} else {
		    XauUnlockAuth (backup_name);
		    lockStatus = LOCK_ERROR;
		}	
	    }
	    /*
	     * Won't be using this file so unlock it.
	     */
	    XauUnlockAuth (home_name);
	}
	if (lockStatus != LOCK_SUCCESS) {
	    Debug ("can't lock auth file %s or backup %s\n",
			    home_name, backup_name);
	    LogError (ReadCatalog
   			 (MC_LOG_SET,MC_LOG_LCK_AUTH,MC_DEF_LOG_LCK_AUTH),
			 home_name, backup_name);
	    return;
	}
	initAddrs ();
	doWrite = 1;
	Debug ("%d authorization protocols for %s\n", d->authNum, d->name);
	/*
	 * Write MIT-MAGIC-COOKIE-1 authorization first, so that
	 * R4 clients which only knew that, and used the first
	 * matching entry will continue to function
	 */
	magicCookie = -1;
	for (i = 0; i < d->authNum; i++)
	{
	    if (auths[i]->name_length == 18 &&
		!strncmp (auths[i]->name, "MIT-MAGIC-COOKIE-1", 18))
	    {
		magicCookie = i;
	    	if (d->displayType.location == Local)
	    	    writeLocalAuth (new, auths[i], d->name);
#ifdef XDMCP
	    	else
	    	    writeRemoteAuth (new, auths[i], d->peer, d->peerlen, d->name);
#endif
		break;
	    }
	}
	/* now write other authorizations */
	for (i = 0; i < d->authNum; i++)
	{
	    Debug("SetUserAuthorization: checking authorization # %d\n", i+1);
	    if (i != magicCookie)
	    {
		data_len = auths[i]->data_length;
		/* client will just use default Kerberos cache, so don't
		 * even write cache info into the authority file.
		 */
		if (auths[i]->name_length == 14 &&
		    !strncmp (auths[i]->name, "MIT-KERBEROS-5", 14))
		    auths[i]->data_length = 0;
	    	if (d->displayType.location == Local)
	    	    writeLocalAuth (new, auths[i], d->name);
#ifdef XDMCP
	    	else
	    	    writeRemoteAuth (new, auths[i], d->peer, d->peerlen, d->name);
#endif
		auths[i]->data_length = data_len;
	    }
	}
	Debug ("SetUserAuthorization: old = %x\n", old);
	if (old) {
	    if (fstat (fileno (old), &statb) != -1)
		chmod (new_name, (int) (statb.st_mode & 0777));
	    /*SUPPRESS 560*/
	    while (entry = XauReadAuth (old)) {
		if (!checkEntry (entry))
		{
		    Debug ("Writing an entry\n");
		    writeAuth (new, entry);
		}
		XauDisposeAuth (entry);
	    }
	    fclose (old);
	}
	doneAddrs ();
	fclose (new);
	Debug ("SetUserAuthorization: name     = %s\n", name);
	Debug ("SetUserAuthorization: new_name = %s\n", new_name);
	Debug ("SetUserAuthorization: unlink(%s)\n", name);
	if (unlink (name) == -1)
	    Debug ("SetUserAuthorization: unlink(%s) failed!\n", name);
	envname = name;
	if (link (new_name, name) == -1) {
	    Debug ("link failed %s %s\n", new_name, name);
	    LogError (
                ReadCatalog(MC_LOG_SET,MC_LOG_NOT_AUTH,MC_DEF_LOG_NOT_AUTH));
	    setenv = 1;
	    envname = new_name;
	} else {
	    Debug ("SetUserAuthorization: link(%s,%s) OK\n", new_name, name);
	    Debug ("SetUserAuthorization: unlink(%s)\n", new_name);
	    unlink (new_name);
	}
	if (setenv) {
	    Debug ("SetUserAuthorization: setenv(XAUTHORITY=%s)\n", envname);
	    verify->userEnviron = setEnv (verify->userEnviron,
				    "XAUTHORITY", envname);
	    verify->systemEnviron = setEnv (verify->systemEnviron,
				    "XAUTHORITY", envname);
	}
	Debug ("SetUserAuthorization: XauUnLockAuth(%s)\n", name);
	XauUnlockAuth (name);
	Debug ("SetUserAuthorization: envname = %s\n", envname);
	if (envname) {
#ifdef NGROUPS
            Debug ("SetUserAuthorization: chown(%s,%d,%d)\n",
		   envname, verify->uid, verify->groups[0]);
	    if(-1 == chown (envname, verify->uid, verify->groups[0])) {
                perror(strerror(errno));
            }
#else
            Debug ("SetUserAuthorization: chown(%s,%d,%d)\n",
		   envname, verify->uid, verify->gid);
	    if(-1 == chown (envname, verify->uid, verify->gid)) {
                perror(strerror(errno));
            }
#endif /* NGROUPS */
        }
    }
    Debug ("done SetUserAuthorization\n");
}

void
RemoveUserAuthorization (struct display *d, struct verify_info *verify)
{
    char    *home;
    Xauth   **auths, *entry;
    char    name[1024], new_name[1024];
    int	    lockStatus;
    FILE    *old, *new;
    struct stat	statb;
    int	    i;
    char    *getEnv ();

    if (!(auths = d->authorizations))
	return;
    home = getEnv (verify->userEnviron, "HOME");
    if (!home)
	return;
    Debug ("RemoveUserAuthorization\n");
    strcpy (name, home);
    if (home[strlen(home) - 1] != '/')
	strcat (name, "/");
    strcat (name, ".Xauthority");
    Debug ("XauLockAuth %s\n", name);
    lockStatus = XauLockAuth (name, 1, 2, 10);
    Debug ("Lock is %d\n", lockStatus);
    if (lockStatus != LOCK_SUCCESS)
	return;
    if (openFiles (name, new_name, &old, &new))
    {
	initAddrs ();
	doWrite = 0;
	for (i = 0; i < d->authNum; i++)
	{
	    if (d->displayType.location == Local)
	    	writeLocalAuth (new, auths[i], d->name);
#ifdef XDMCP
	    else
	    	writeRemoteAuth (new, auths[i], d->peer, d->peerlen, d->name);
#endif
	}
	doWrite = 1;
	if (old) {
	    if (fstat (fileno (old), &statb) != -1)
		chmod (new_name, (int) (statb.st_mode & 0777));
	    /*SUPPRESS 560*/
	    while (entry = XauReadAuth (old)) {
		if (!checkEntry (entry))
		{
		    Debug ("Writing an entry\n");
		    writeAuth (new, entry);
		}
		XauDisposeAuth (entry);
	    }
	    fclose (old);
	}
	doneAddrs ();
	fclose (new);
	if (unlink (name) == -1)
	    Debug ("unlink %s failed\n", name);
	if (link (new_name, name) == -1) {
	    Debug ("link failed %s %s\n", new_name, name);
	    LogError (
	        ReadCatalog(MC_LOG_SET,MC_LOG_NOT_AUTH,MC_DEF_LOG_NOT_AUTH));
	} else {
	    Debug ("new is in place, go for it!\n");
	    unlink (new_name);
	}
    }
    XauUnlockAuth (name);
}
