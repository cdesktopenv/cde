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
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/*
 * $TOG: choose.c /main/5 1997/08/13 11:42:50 kaleb $
 *
 * Copyright 1990 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL M.I.T.
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Keith Packard, MIT X Consortium
 */

/*
 * choose.c
 *
 * xdm interface to chooser program
 */

# include   "dm.h"
# include   "vg.h"

# include	<X11/X.h>
# include	<sys/types.h>
# include	<sys/socket.h>
# include	<netinet/in.h>
# include	<sys/un.h>
# include	<ctype.h>
# include	<errno.h>

static char *Print8Address (ARRAY8Ptr   Address);


static int
FormatBytes (
    unsigned char *data,
    int     length,
    char    *buf,
    int     buflen)
{
    int	    i;
    static char	    HexChars[] = "0123456789abcdef";

    if (buflen < length * 2 + 1)
	return 0;
    for (i = 0; i < length; i++)
    {
	*buf++ = HexChars[(data[i] >> 4) & 0xf];
	*buf++ = HexChars[(data[i]) & 0xf];
    }
    *buf++ = '\0';
    return 1;
}

static int
FormatARRAY8 (
    ARRAY8Ptr   a,
    char        *buf,
    int         buflen)
{
    return FormatBytes (a->data, a->length, buf, buflen);
}

typedef struct _IndirectUsers {
    struct _IndirectUsers   *next;
    ARRAY8	client;
    CARD16	connectionType;
} IndirectUsersRec, *IndirectUsersPtr;

static IndirectUsersPtr	indirectUsers;

int
RememberIndirectClient (
    ARRAY8Ptr   clientAddress,
    CARD16      connectionType)
{
    IndirectUsersPtr	i;

    for (i = indirectUsers; i; i = i->next)
	if (XdmcpARRAY8Equal (clientAddress, &i->client) &&
	    connectionType == i->connectionType)
	    return 1;
    i = (IndirectUsersPtr) malloc (sizeof (IndirectUsersRec));
    if (!XdmcpCopyARRAY8 (clientAddress, &i->client))
    {
	free ((char *) i);
	return 0;
    }
    i->connectionType = connectionType;
    i->next = indirectUsers;
    indirectUsers = i;
    return 1;
}

void
ForgetIndirectClient (
    ARRAY8Ptr   clientAddress,
    CARD16      connectionType)
{
    IndirectUsersPtr	i, prev;

    prev = 0;
    for (i = indirectUsers; i; i = i->next)
    {
	if (XdmcpARRAY8Equal (clientAddress, &i->client) &&
	    connectionType == i->connectionType)
	{
	    if (prev)
		prev->next = i->next;
	    else
		indirectUsers = i->next;
	    XdmcpDisposeARRAY8 (&i->client);
	    free ((char *) i);
	    break;
	}
	prev = i;
    }
}

int
IsIndirectClient (
    ARRAY8Ptr   clientAddress,
    CARD16      connectionType)
{
    IndirectUsersPtr	i;

    for (i = indirectUsers; i; i = i->next)
	if (XdmcpARRAY8Equal (clientAddress, &i->client) &&
	    connectionType == i->connectionType)
	    return 1;
    return 0;
}

extern char *NetaddrPort();

static int
FormatChooserArgument (
    char    *buf,
    int     len)
{
    unsigned char   addr_buf[1024];
    int		    addr_len = sizeof (addr_buf);
    unsigned char   result_buf[1024];
    int		    result_len = 0;
    int		    netfamily;

    if (GetChooserAddr (addr_buf, &addr_len) == -1)
    {
	LogError ((unsigned char *)"Cannot get return address for chooser socket\n");
	Debug ("Cannot get chooser socket address\n");
	return 0;
    }
    netfamily = NetaddrFamily((XdmcpNetaddr)addr_buf);
    switch (netfamily) {
    case AF_INET:
	{
	    char *port;
	    int portlen;
	    ARRAY8Ptr localAddress, getLocalAddress ();

	    port = NetaddrPort((XdmcpNetaddr)addr_buf, &portlen);
	    result_buf[0] = netfamily >> 8;
	    result_buf[1] = netfamily & 0xFF;
	    result_buf[2] = port[0];
	    result_buf[3] = port[1];
	    localAddress = getLocalAddress ();
	    bcopy ((char *)localAddress->data, (char *)result_buf+4, 4);
	    result_len = 8;
	}
	break;
#ifdef AF_DECnet
    case AF_DECnet:
	break;
#endif
    default:
	Debug ("Chooser family %d isn't known\n", netfamily);
	return 0;
    }

    return FormatBytes (result_buf, result_len, buf, len);
}

typedef struct _Choices {
    struct _Choices *next;
    ARRAY8	    client;
    CARD16	    connectionType;
    ARRAY8	    choice;
    long	    time;
} ChoiceRec, *ChoicePtr;

static ChoicePtr   choices;

ARRAY8Ptr
IndirectChoice (
    ARRAY8Ptr   clientAddress,
    CARD16      connectionType)
{
    ChoicePtr	c, next, prev;
    long	now;

    now = time (0);
    prev = 0;
    for (c = choices; c; c = next)
    {
	next = c->next;
	if (now - c->time > 15)
	{
	    Debug ("Timeout choice\n");
	    if (prev)
		prev->next = next;
	    else
		choices = next;
	    XdmcpDisposeARRAY8 (&c->client);
	    XdmcpDisposeARRAY8 (&c->choice);
	    free ((char *) c);
	}
	else
	{
	    if (XdmcpARRAY8Equal (clientAddress, &c->client) &&
	    	connectionType == c->connectionType)
	    	return &c->choice;
	    prev = c;
	}
    }
    return 0;
}

static int
RegisterIndirectChoice (
    ARRAY8Ptr   clientAddress,
    CARD16 connectionType,
    ARRAY8Ptr   choice)
{
    ChoicePtr	c;
    int		insert;
    int		found = 0;

    Debug ("Got indirect choice back (%s)\n", Print8Address(clientAddress));
    for (c = choices; c; c = c->next) {
	if (XdmcpARRAY8Equal (clientAddress, &c->client) &&
	    connectionType == c->connectionType) {
	    found = 1;
	    break;
	}
    }
    if (!found)
	return 0;

    insert = 0;

    if (!c)
    {
	c = (ChoicePtr) malloc (sizeof (ChoiceRec));
	insert = 1;
	if (!c)
	    return 0;
	c->connectionType = connectionType;
    	if (!XdmcpCopyARRAY8 (clientAddress, &c->client))
    	{
	    free ((char *) c);
	    return 0;
    	}
    }
    else
    {
	XdmcpDisposeARRAY8 (&c->choice);
    }

    if (!XdmcpCopyARRAY8 (choice, &c->choice))
    {
	XdmcpDisposeARRAY8 (&c->client);
	free ((char *) c);
	return 0;
    }
    if (insert)
    {
	c->next = choices;
	choices = c;
    }
    c->time = time (0);

    Debug("choice=(%s)\n", Print8Address(choice)); 

    return 1;
}

#ifdef notdef
static void
RemoveIndirectChoice (clientAddress, connectionType)
    ARRAY8Ptr	clientAddress;
    CARD16	connectionType;
{
    ChoicePtr	c, prev;

    prev = 0;
    for (c = choices; c; c = c->next)
    {
	if (XdmcpARRAY8Equal (clientAddress, &c->client) &&
	    connectionType == c->connectionType)
	{
	    if (prev)
		prev->next = c->next;
	    else
		choices = c->next;
	    XdmcpDisposeARRAY8 (&c->client);
	    XdmcpDisposeARRAY8 (&c->choice);
	    free ((char *) c);
	    return;
	}
	prev = c;
    }
}
#endif

/*ARGSUSED*/
static void
AddChooserHost (
    CARD16      connectionType,
    ARRAY8Ptr   addr,
    char        *closure)
{
    char	***argp, **parseArgs();
    char	hostbuf[1024];

    argp = (char ***) closure;
    if (addr->length == strlen ("BROADCAST") &&
	!strncmp ((char *)addr->data, "BROADCAST", addr->length))
    {
	*argp = parseArgs (*argp, "BROADCAST");
    }
    else if (FormatARRAY8 (addr, hostbuf, sizeof (hostbuf)))
    {
	*argp = parseArgs (*argp, hostbuf);
    }
}

int
ProcessChooserSocket (
    int fd)
{
    int client_fd;
    char	buf[1024];
    int		len;
    XdmcpBuffer	buffer;
    ARRAY8	clientAddress;
    CARD16	connectionType;
    ARRAY8	choice;

    Debug ("Process chooser socket\n");
    len = sizeof (buf);
    client_fd = accept (fd, (struct sockaddr *)buf, &len);
    if (client_fd == -1)
    {
	LogError ((unsigned char *)"Cannot accept chooser connection\n");
	return 0;
    }
    Debug ("Accepted %d\n", client_fd);
    
    len = read (client_fd, buf, sizeof (buf));
    Debug ("Read returns %d\n", len);
    if (len > 0)
    {
    	buffer.data = (BYTE *) buf;
    	buffer.size = sizeof (buf);
    	buffer.count = len;
    	buffer.pointer = 0;
	clientAddress.data = 0;
	clientAddress.length = 0;
	choice.data = 0;
	choice.length = 0;
	if (XdmcpReadARRAY8 (&buffer, &clientAddress) &&
	    XdmcpReadCARD16 (&buffer, &connectionType) &&
	    XdmcpReadARRAY8 (&buffer, &choice))
	{
	    Debug ("Read from chooser succesfully\n");
	    if (!RegisterIndirectChoice (&clientAddress, connectionType, &choice))
		Debug ("Invalid chooser reply\n");
	}
	XdmcpDisposeARRAY8 (&clientAddress);
	XdmcpDisposeARRAY8 (&choice);
    }
    else
    {
	LogError ((unsigned char *)"Choice response read error %s\n", strerror(errno));
    }

    close (client_fd);
    return 1;
}

void
RunChooser (
    struct display  *d)
{
    char    **args, **parseArgs(), **systemEnv();
    char    buf[1024];
    char    **env;

    Debug ("RunChooser %s\n", d->name);
    SetTitle (d->name, "chooser");
    LoadXloginResources (d);

    args = parseArgs ((char **) 0, d->chooser);
    strcpy (buf, "-xdmaddress ");
    if (FormatChooserArgument (buf + strlen (buf), sizeof (buf) - strlen (buf)))
	args = parseArgs (args, buf);
    strcpy (buf, "-clientaddress ");
    if (FormatARRAY8 (&d->clientAddr, buf + strlen (buf), sizeof (buf) - strlen (buf)))
	args = parseArgs (args, buf);
    sprintf (buf, "-connectionType %d", d->connectionType);
    args = parseArgs (args, buf);
    ForEachChooserHost (&d->clientAddr,
			d->connectionType,
			(int (*)()) AddChooserHost,
			(char *) &args);
    env = systemEnv (d, (char *) 0, (char *) 0);
    if (d->authFile)
            env = setEnv (env, "XAUTHORITY", d->authFile);
    if (d->pmSearchPath)
            env = setEnv(env, "XMICONSEARCHPATH", d->pmSearchPath);
    if (d->bmSearchPath)
            env = setEnv(env, "XMICONBMSEARCHPATH", d->bmSearchPath);
    if ( d->language && strlen(d->language) > 0 )
	    env = setEnv(env, "LANG", d->language);
    if ( d->langList && strlen(d->langList) > 0 )
            env = setEnv(env, LANGLIST,  d->langList);
#if !defined (ENABLE_DYNAMIC_LANGLIST)
    else
            if (languageList && strlen(languageList) > 0 )
                 env = setEnv(env, LANGLIST, languageList);
#endif /* ENABLE_DYNAMIC_LANGLIST */
    if ( d->setup)
            env = setEnv(env, "XSETUP", d->setup);
    if(d->displayType.location == Local)
            env = setEnv (env, LOCATION, "local");
    else
            env = setEnv (env, LOCATION, "remote");


    Debug ("Running %s\n", args[0]);
    execute (args, env);
    Debug ("Couldn't run %s\n", args[0]);
    LogError ((unsigned char *)"Cannot execute %s\n", args[0]);
    exit (REMANAGE_DISPLAY);
}

static char *
Print8Address (
    ARRAY8Ptr   Address)
{
  static char buf[200];
  char *b;
  int i;

  b = buf;
  b[0]='\0';
  for (i = 0; i < (int)Address->length; i++) {
      sprintf(b, " %d", Address->data[i]);
      b = buf + strlen(buf);
  }
  return(buf);
}
