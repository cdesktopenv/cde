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
 * xdm - display manager daemon
 *
 * $XConsortium: file.c /main/4 1995/10/27 16:13:19 rswiston $
 *
 * Copyright 1988 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * Author:  Keith Packard, MIT X Consortium
 */

/*
 * file.c
 */

# include	"dm.h"
# include	"vgmsg.h"
# include	<ctype.h>
# include	<signal.h>
# include	<pwd.h>

# include	<sys/socket.h>
# include	<netinet/in.h>
# include	<netdb.h>



/***************************************************************************
 *
 *  Local procedure declarations
 *
 ***************************************************************************/

static int DisplayTypeMatch( DisplayType d1, DisplayType d2) ;
static void freeArgs( char **args) ;
static void freeSomeArgs( char **args, int n) ;
static DisplayType parseDisplayType( char *string, int *usedDefaultType, int *parse_uid) ;
static char ** splitIntoWords( char *s) ;
static char ** copyArgs( char **args) ;




/***************************************************************************
 *
 *  main
 *
 ***************************************************************************/

static int 
DisplayTypeMatch( DisplayType d1, DisplayType d2 )
{
	return d1.location == d2.location &&
	       d1.lifetime == d2.lifetime &&
	       d1.origin == d2.origin;
}

static void 
freeArgs( char **args )
{
    char    **a;

    for (a = args; *a; a++)
	free (*a);
    free ((char *) args);
}

static char ** 
splitIntoWords( char *s )
{
    char    **args, **newargs;
    char    *wordStart;
    int	    nargs;

    args = 0;
    nargs = 0;
    while (*s)
    {
	while (*s && isspace (*s))
	    ++s;
	if (!*s || *s == '#')
	    break;
	wordStart = s;
	while (*s && *s != '#' && !isspace (*s))
	    ++s;
	if (!args)
	{
    	    args = (char **) malloc (2 * sizeof (char *));
    	    if (!args)
	    	return NULL;
	}
	else
	{
	    newargs = (char **) realloc ((char *) args,
					 (nargs+2)*sizeof (char *));
	    if (!newargs)
	    {
	    	freeArgs (args);
	    	return NULL;
	    }
	    args = newargs;
	}
	args[nargs] = malloc (s - wordStart + 1);
	if (!args[nargs])
	{
	    freeArgs (args);
	    return NULL;
	}
	strncpy (args[nargs], wordStart, s - wordStart);
	args[nargs][s-wordStart] = '\0';
	++nargs;
	args[nargs] = NULL;
    }
    return args;
}

static char ** 
copyArgs( char **args )
{
    char    **a, **new, **n;

    for (a = args; *a; a++)
	;
    new = (char **) malloc ((a - args + 1) * sizeof (char *));
    if (!new)
	return NULL;
    n = new;
    a = args;
    while (*n++ = *a++)
	;
    return new;
}

static void 
freeSomeArgs( char **args, int n )
{
    char    **a;

    a = args;
    while (n--)
	free (*a++);
    free ((char *) args);
}

int 
ParseDisplay( char *source, 
	      DisplayType *acceptableTypes, 
	      int numAcceptable,
	      struct passwd *puser)
{
    char		**args, **argv, **a;
    char		*name = NULL, *class, *type;
    struct display	*d;
    int			usedDefaultType;
    int			parse_uid;
    DisplayType		displayType;

    char		*device=NULL; /* ITE device associated with display */


    args = splitIntoWords (source);
    if (!args)
	return 0;
    if (!args[0])
    {
	LogError(ReadCatalog(MC_LOG_SET,MC_LOG_MISS_NAME,MC_DEF_LOG_MISS_NAME));
	freeArgs (args);
	return 0;
    }
    name = strdup(args[0]);
    if (!args[1])
    {
	LogError(ReadCatalog(MC_LOG_SET,MC_LOG_MISS_TYPE,MC_DEF_LOG_MISS_TYPE),
		args[0]);
	freeArgs (args);
        free(name);
	return 0;
    }

    /*
     *  strip off display device if found in second field...
     */

    if ( (device = strchr(args[1],'@')) != NULL) {
	*device = '\0';
	device++;
    }
    	
    displayType = parseDisplayType (args[1], &usedDefaultType, &parse_uid);
    class = NULL;
    type = args[1];
    argv = args + 2;

    /*
     *  check for special syntax "*" and expand to host name.
     *  if hostname cannot be found in a database, assume invalid and
     *  delete.
     */
    if ( strcmp(name, "*") == 0) {
	char 		tname[128];
	struct hostent	*hostent;

        memset(tname, 0, 128);
	gethostname(tname, 128 - 1);
	if ( (hostent = gethostbyname(tname)) == NULL ) {
	    LogError(
		ReadCatalog(MC_LOG_SET,MC_LOG_INV_HOSTNM,MC_DEF_LOG_INV_HOSTNM),
		      tname);
	    tname[0] = 0;
	}

	strncat(tname, ":0", 128 - 1);
	
        free(name);
	name = strdup(tname);
    }

    /*
     * extended syntax; if the second argument doesn't
     * exactly match a legal display type and the third
     * argument does, use the second argument as the
     * display class string
     */
    if (usedDefaultType && args[2])
    {

	/*
	 *  strip off display device if found in third field...
	 */

	if ( device == NULL && (device = strchr(args[2],'@')) != NULL) {
	    *device = '\0';
	    device++;
	}

	displayType = parseDisplayType (args[2], &usedDefaultType, &parse_uid);
	if (!usedDefaultType)
	{
	    class = args[1];
	    type = args[2];
	    argv = args + 3;
	}
    }
    /*
     * extended syntax; if the display type argument was 
     * "local_uid", then next argument is pseudo user id
     * under which the local Xserver is to be run.
     */
    if (parse_uid) {
	struct passwd *p;

        Debug("Xservers 'local_uid' pseudo user = %s\n", *argv);

        if ( (p = getpwnam (*argv)) != NULL) {
	    *puser = *p;
	} else {
	    Debug("Could not get password entry for user name '%s'\n", *argv);
	    Debug("Using default pseudo user = %s\n", puser->pw_name);
	}

	argv = argv + 1;
    } else {
        Debug("Default pseudo user = %s\n", puser->pw_name);
    }

    while (numAcceptable)
    {
	if (DisplayTypeMatch (*acceptableTypes, displayType))
	    break;
	--numAcceptable;
	++acceptableTypes;
    }
    if (!numAcceptable)
    {
	LogError(ReadCatalog(
		MC_LOG_SET,MC_LOG_BAD_DPYTYPE,MC_DEF_LOG_BAD_DPYTYPE),
		  type, name);
    }


    /*
     *  see if this display is already being managed...
     */

    d = FindDisplayByName (name);
    if (d)
    {
	d->state = OldEntry;
	if (class && strcmp (d->class, class))
	{
	    char    *newclass;

	    newclass = malloc ((unsigned) (strlen (class) + 1));
	    if (newclass)
	    {
		free (d->class);
		strcpy (newclass, class);
		d->class = newclass;
	    }
	}
	Debug ("Found existing display:  %s %s %s", d->name, d->class ? d->class : "", type);
	freeArgs (d->argv);
    }
    else
    {
	d = NewDisplay (name, class);
	Debug ("Found new display:  %s %s %s", d->name, d->class ? d->class : "", type);
    }
    d->displayType = displayType;
    d->argv = copyArgs (argv);
    for (a = d->argv; a && *a; a++)
	Debug (" %s", *a);
    Debug ("\n");

    /*
     * add device to display information...
     */
     
    if ( device != NULL && strlen(device) != 0 ) {
	if (d->gettyLine != NULL)
	    free(d->gettyLine);

	d->gettyLine = strdup(device);

    }

    if (d->gettyLine && 
        (strcmp(d->gettyLine, "None") == 0 ||
         strcmp(d->gettyLine, "none") == 0   ) ) {

	strcpy(d->gettyLine,"??");
    }


    freeSomeArgs (args, argv - args);
    free(name);

    return 1;
}

static struct displayMatch {
	char		*name;
	DisplayType	type;
} displayTypes[] = {
	"local",		{ Local, Permanent, FromFile },
	"local_uid",		{ Local, Permanent, FromFile },
	"foreign",		{ Foreign, Permanent, FromFile },
	0,			{ Local, Permanent, FromFile },
};

static DisplayType 
parseDisplayType( char *string, int *usedDefaultType, int *parse_uid )
{
	struct displayMatch	*d;

        *parse_uid = 0;

	for (d = displayTypes; d->name; d++) {
	    if (strcmp(d->name, string) == 0)
	    {
		if (strcmp(d->name, "local_uid") == 0) {
		   *parse_uid = 1; 
		}
		*usedDefaultType = 0;
		return d->type;
	    }
        }

	*usedDefaultType = 1;
	return d->type;
}
