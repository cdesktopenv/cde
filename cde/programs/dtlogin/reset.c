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
 * $XConsortium: reset.c /main/4 1995/10/27 16:14:40 rswiston $
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
 * pseudoReset -- pretend to reset the server by killing all clients
 * with windows.  It will reset the server most of the time, unless
 * a client remains connected with no windows.
 */

# include	<setjmp.h>
# include	<sys/types.h>
# include	<sys/signal.h>
# include	"dm.h"
# include	"vgmsg.h"


/***************************************************************************
 *
 *  Local procedure declarations
 *
 ***************************************************************************/

static SIGVAL abortReset( int arg ) ;
static int ignoreErrors( Display *dpy, XErrorEvent *event) ;
static void killWindows( Display *dpy, Window window) ;



/***************************************************************************
 *
 *  
 *
 ***************************************************************************/

/*ARGSUSED*/
static int 
ignoreErrors( Display *dpy, XErrorEvent *event )
{
	Debug ("Ignoring error...\n");
	return 1;
}

/*
 * this is mostly bogus -- but quite useful.  I wish the protocol
 * had some way of enumerating and identifying clients, that way
 * this code wouldn't have to be this kludgy.
 */

static void 
killWindows( Display *dpy, Window window )
{
	Window	root, parent, *children;
	int	child;
	unsigned int nchildren = 0;
	
	while (XQueryTree (dpy, window, &root, &parent, &children, &nchildren)
	       && nchildren > 0)
	{
		for (child = 0; child < nchildren; child++) {
			Debug ("Calling XKillClient() for window 0x%x\n",
				children[child]);
			XKillClient (dpy, children[child]);
		}
		XFree ((char *)children);
	}
}

static jmp_buf	resetJmp;

static SIGVAL
abortReset( int arg )
{
	longjmp (resetJmp, 1);
}

/*
 * this display connection better not have any windows...
 */
 
void 
pseudoReset( Display *dpy )
{
	Window	root;
	int	screen;

	if (setjmp (resetJmp)) {
		LogError(
		  ReadCatalog(MC_LOG_SET,MC_LOG_PSEUDO,MC_DEF_LOG_PSEUDO));
	} else {
		signal (SIGALRM, abortReset);
		alarm (30);
		XSetErrorHandler (ignoreErrors);
		for (screen = 0; screen < ScreenCount (dpy); screen++) {
			Debug ("Pseudo reset screen %d\n", screen);
			root = RootWindow (dpy, screen);
			killWindows (dpy, root);
		}
		Debug ("Before XSync\n");
		XSync (dpy, False);
		(void) alarm (0);
	}
	signal (SIGALRM, SIG_DFL);
	XSetErrorHandler ((int (*)()) 0);
	Debug ("pseudoReset() done\n");
}
