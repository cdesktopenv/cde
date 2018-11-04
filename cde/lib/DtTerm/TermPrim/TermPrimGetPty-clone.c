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

#include "TermPrimDebug.h"
#include "TermHeader.h"
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <signal.h>
#include <Xm/Xm.h>
#ifdef	HP_ARCHITECTURE
# define X_INCLUDE_GRP_H
#endif	/* HP_ARCHITECTURE */
#define X_INCLUDE_UNISTD_H
#define XOS_USE_XT_LOCKING
#include <X11/Xos_r.h>

#if defined(__AIX)
# define	PTY_CLONE_DEVICE	"/dev/ptc"
#elif defined(__linux__)
# define	PTY_CLONE_DEVICE	"/dev/ptyc"
#endif	/* __AIX */

static int
GetPty(char **ptySlave, char **ptyMaster)
{
    char *c;
    int ptyFd;
    int ttyFd;

    *ptyMaster = malloc(strlen(PTY_CLONE_DEVICE) + 1);
    (void) strcpy(*ptyMaster, PTY_CLONE_DEVICE);

    if (isDebugFSet('p', 10)) {
#ifdef	BBA
#pragma	BBA_IGNORE
#endif	/*BBA*/
	return(-1);
    }

    if ((ptyFd = open(*ptyMaster, O_RDWR, 0))) {
        _Xttynameparams tty_buf;
#if defined(__linux__)
	if (c = _XTtyname(ptyFd)) {
#else
	if (c = _XTtyname(ptyFd, tty_buf)) {
#endif
	    *ptySlave = malloc(strlen(c) + 1);
	    (void) strcpy(*ptySlave, c);
	    
	    if ((ttyFd = open(*ptySlave, O_RDWR | O_NOCTTY, 0)) < 0) {
		/* failure... */
		(void) perror(*ptySlave);
		(void) close(ptyFd);
	    } else {
		/* success...
		 */
		/* close off the pty slave... */
		(void) close(ttyFd);

		/* fix the owner, mode, and group... */
#ifdef	HP_ARCHITECTURE
		{
		    struct group *grp;
		    gid_t gid;
		    _Xgetgrparams grp_buf;

		    if (grp = _XGetgrnam("tty", grp_buf)) {
			gid = grp->gr_gid;
		    } else {
			gid = 0;
		    }
		    (void) endgrent();
		    (void) chown(*ptySlave, getuid(), gid);
		    (void) chmod(*ptySlave, 0620);
		}
#else	/* HP_ARCHITECTURE */
		(void) chown(*ptySlave, getuid(), getgid());
		(void) chmod(*ptySlave, 0622);
#endif	/* HP_ARCHITECTURE */

		/* pty master and slave names are already set.  Return
		 * the file descriptor...
		 */

		return(ptyFd);
	    }
	} else {
	    /* ttyname on the pty master failed.  This should not happen!... */
	    (void) perror("ttyname");
	    (void) close(ptyFd);
	}
    } else {
	(void) perror(*ptyMaster);
    }
    return(-1);
}

/* this is a public wrapper around the previous function that runs the
 * previous function setuid root...
 */
int
_DtTermPrimGetPty(char **ptySlave, char **ptyMaster)
{
    int retValue;

    /* this function needs to be suid root... */
    (void) _DtTermPrimToggleSuidRoot(True);
    retValue = GetPty(ptySlave, ptyMaster);
    /* we now need to turn off setuid root... */
    (void) _DtTermPrimToggleSuidRoot(False);

    return(retValue);
}

static int
SetupPty(char *ptySlave, int ptyFd)
{
#ifdef	HP_ARCHITECTURE
    {
	struct group *grp;
	gid_t gid;
	_Xgetgrparams grp_buf;

	if (grp = _XGetgrnam("tty", grp_buf)) {
	    gid = grp->gr_gid;
	} else {
	    gid = 0;
	}
	(void) endgrent();
	(void) chown(ptySlave, getuid(), gid);
	(void) chmod(ptySlave, 0620);
    }
#else	/* HP_ARCHITECTURE */
    (void) chown(ptySlave, getuid(), getgid());
    (void) chmod(ptySlave, 0622);
#endif	/* HP_ARCHITECTURE */
}
    
int
_DtTermPrimSetupPty(char *ptySlave, int ptyFd)
{
    int retValue;

    /* this function needs to be suid root... */
    (void) _DtTermPrimToggleSuidRoot(True);
    retValue = SetupPty(ptySlave, ptyFd);
    /* we now need to turn off setuid root... */
    (void) _DtTermPrimToggleSuidRoot(False);

    return(retValue);
}

static void
ReleasePty(char *ptySlave)
{
    (void) chown(ptySlave, 0, 0);
    (void) chmod(ptySlave, 0666);
}
    
void
_DtTermPrimReleasePty(char *ptySlave)
{
    /* this function needs to be suid root... */
    (void) _DtTermPrimToggleSuidRoot(True);
    (void) ReleasePty(ptySlave);
    /* we now need to turn off setuid root... */
    (void) _DtTermPrimToggleSuidRoot(False);
}

void
_DtTermPrimPtyCleanup()
{
    return;
}
