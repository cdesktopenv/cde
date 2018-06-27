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
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/*
 * xdm - display manager daemon
 *
 * $TOG: daemon.c /main/5 1998/04/06 13:21:16 mgreess $
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

#include <X11/Xos.h>
#include <sys/ioctl.h>
#ifdef hpux
#include <sys/ptyio.h>
#endif

#ifdef SVR4
#include <termio.h>
#endif

extern void exit ();


void
BecomeDaemon( void )
{
    int i;

    /*
     * fork so that the process goes into the background automatically. Also
     * has a nice side effect of having the child process get inherited by
     * init (pid 1).
     */

    if (fork ())			/* if parent */
      exit (0);				/* then no more work to do */

    /*
     * Close standard file descriptors and get rid of controlling tty
     */

#if defined(SYSV) || defined (SVR4) || defined(__linux__)
    setpgrp ();
#else
    setpgrp (0, getpid());
#endif

    close (0); 
    close (1);
    close (2);

    if ((i = open ("/dev/tty", O_RDWR)) >= 0) {	/* did open succeed? */
#if defined(SYSV) && defined(TIOCTTY)
	int zero = 0;
	(void) ioctl (i, TIOCTTY, &zero);
#else
	(void) ioctl (i, TIOCNOTTY, (char *) 0);    /* detach, BSD style */
#endif
	(void) close (i);
    }

    /*
     * Set up the standard file descriptors.
     */
    (void) open ("/", O_RDONLY);	/* root inode already in core */
    (void) dup2 (0, 1);
    (void) dup2 (0, 2);
}
