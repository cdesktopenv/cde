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
#ifndef lint
#ifdef  VERBOSE_REV_INFO
static char rcs_id[] = "$XConsortium: TermPrimGetPty-bsd.c /main/4 1996/11/21 19:58:32 drk $";
#endif  /* VERBOSE_REV_INFO */
#endif  /* lint */

/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include <fcntl.h>
#include <termios.h>
#if defined(OPENBSD_ARCHITECTURE)
#include <sys/stat.h>
#include <util.h>
#endif
#include <sys/wait.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <Xm/Xm.h>
#define X_INCLUDE_GRP_H
#define X_INCLUDE_UNISTD_H
#define XOS_USE_XT_LOCKING
#include <X11/Xos_r.h>
#include "TermPrim.h"
#include "TermHeader.h"
#include "TermPrimDebug.h"
#include "TermPrimUtil.h"

typedef struct _ptyInfo {
    char *ptyName;
    struct _ptyInfo *next;
    struct _ptyInfo *prev;
} ptyInfo;

static ptyInfo _ptyInfoHead;
static ptyInfo *ptyInfoHead = &_ptyInfoHead;

static void
AddPtyInfo
(
    char		 *ptyName
)
{
    ptyInfo		 *ptyInfoTmp;
    sigset_t		  newSigs;
    sigset_t		  oldSigs;

    /* malloc a new entry... */
    ptyInfoTmp = (ptyInfo *) XtMalloc(sizeof(ptyInfo));
    (void) memset(ptyInfoTmp, '\0', sizeof(ptyInfo));

    /* fill in the structure... */
    ptyInfoTmp->ptyName = (char *) XtMalloc(strlen(ptyName) + 1);
    (void) strcpy(ptyInfoTmp->ptyName, ptyName);

    /* insert it after the head of the list...
     */
    /* block all signals... */
    (void) sigfillset(&newSigs);
    (void) sigemptyset(&oldSigs);
    (void) sigprocmask(SIG_BLOCK, &newSigs, &oldSigs);
    /* insert the entry into the list... */
    ptyInfoTmp->prev = ptyInfoHead;
    ptyInfoTmp->next = ptyInfoHead->next;
    ptyInfoHead->next = ptyInfoTmp;
    if (ptyInfoTmp->next) {
	ptyInfoTmp->next->prev = ptyInfoTmp;
    }
    /* restore signals... */
    (void) sigprocmask(SIG_SETMASK, &oldSigs, (sigset_t *) 0);
}

static void
DeletePtyInfo
(
    char		 *ptyName
)
{
    ptyInfo		 *ptyInfoTmp;
    sigset_t		  newSigs;
    sigset_t		  oldSigs;

    /* find the entry... */
    for (ptyInfoTmp = ptyInfoHead->next; ptyInfoTmp;
	    ptyInfoTmp = ptyInfoTmp->next) {
	if (!strcmp(ptyInfoTmp->ptyName, ptyName)) {
	    break;
	}
    }

    /* did we find anything... */
    if (!ptyInfoTmp) {
	/* not found... */
	return;
    }

    /* delete entry from the list...
     */
    /* block all signals... */
    (void) sigfillset(&newSigs);
    (void) sigemptyset(&oldSigs);
    (void) sigprocmask(SIG_BLOCK, &newSigs, &oldSigs);
    /* remove it... */
    ptyInfoTmp->prev->next = ptyInfoTmp->next;
    if (ptyInfoTmp->next) {
	ptyInfoTmp->next->prev = ptyInfoTmp->prev;
    }
    /* restore signals... */
    (void) sigprocmask(SIG_SETMASK, &oldSigs, (sigset_t *) 0);

    /* free up the data... */
    if (ptyInfoTmp->ptyName) {
	(void) XtFree(ptyInfoTmp->ptyName);
	ptyInfoTmp->ptyName = (char *) 0;
    }
    (void) XtFree((char *) ptyInfoTmp);
}

#define True	1
#define	False	0

static char PTY_dev[]           = "/dev";
static char PTY_dev_pty[]       = "/dev/pty";
static char PTY_dev_ptym[]      = "/dev/ptym";
static char PTY_a_ce_o[]        = "abcefghijklmno";
static char PTY_a_ce_z[]        = "abcefghijklmnopqrstuvwxyz";
static char PTY_0_9[]           = "0123456789";
static char PTY_0_9a_f[]        = "0123456789abcdef";
static char PTY_p_z[]           = "pqrstuvwxyz";
static char PTY_p_r[]           = "pqr";
#define         PTY_null        (char *) 0
static struct _pty_dirs {
    char *pty_dir;
    char *ptym_dir;
    char *char_1;
    char *char_2;
    char *char_3;
    int fast;
} pty_dirs[] = {
    {PTY_dev_pty, PTY_dev_ptym, PTY_a_ce_o, PTY_0_9,    PTY_0_9,  True},
    {PTY_dev_pty, PTY_dev_ptym, PTY_p_z,    PTY_0_9,    PTY_0_9,  True},
    {PTY_dev_pty, PTY_dev_ptym, PTY_a_ce_o, PTY_0_9a_f, PTY_null, True},
    {PTY_dev_pty, PTY_dev_ptym, PTY_p_z,    PTY_0_9a_f, PTY_null, False},
    {PTY_dev,     PTY_dev,      PTY_p_r,    PTY_0_9a_f, PTY_null, False},
    {PTY_null,    PTY_null,     PTY_null,   PTY_null,   PTY_null, False},
};

#if defined(ALPHA_ARCHITECTURE) || defined(OPENBSD_ARCHITECTURE)
/* Use openpty() to open Master/Slave pseudo-terminal pair */
/* Current version of openpty() uses non-STREAM device. BSD name space */
#define TTYNAMELEN      25
  static int
  GetPty(char **ptySlave, char **ptyMaster)
  {
    int master, slave;

    *ptySlave = malloc(TTYNAMELEN);
    /* malloc error */
    if (!(*ptySlave)) return -1;

    if(!openpty(&master, &slave, *ptySlave, NULL, NULL))
    {
        /* success */
        close(slave);

        /* add it to the list... */
        (void) AddPtyInfo(*ptySlave);

        return master;
    }
    else    /* failure */
        return -1;
}
#else
static int
GetPty(char **ptySlave, char **ptyMaster)
{
    struct _pty_dirs *pty_dirs_ptr;
    char *char_1;
    char *char_2;
    char *char_3;
    int first;
    int ptyFd;
    int ttyFd;

    char *ttyDev = (char *) 0;
    char *ptyDev = (char *) 0;

    for (pty_dirs_ptr = pty_dirs;
	    pty_dirs_ptr->pty_dir && pty_dirs_ptr->ptym_dir; pty_dirs_ptr++) {
	ttyDev = realloc(ttyDev,
		(unsigned) (strlen(pty_dirs_ptr->pty_dir) + 8));
	ptyDev = realloc(ptyDev,
		(unsigned) (strlen(pty_dirs_ptr->ptym_dir) + 8));
	if (!ttyDev || !ptyDev) {
	    (void) perror("malloc");
	    return(-1);
	}

	if (isDebugFSet('p', 10)) {
#ifdef	BBA
#pragma	BBA_IGNORE
#endif	/*BBA*/
	    return(-1);
	}

	for (first = 1, char_1 = pty_dirs_ptr->char_1;
		(first || !pty_dirs_ptr->fast) && *char_1; char_1++) {
	    for (char_2 = pty_dirs_ptr->char_2;
		    (first || !pty_dirs_ptr->fast) && *char_2; char_2++) {
		for (char_3 = pty_dirs_ptr->char_3;
			(first || !pty_dirs_ptr->fast); ) {
		    (void) sprintf(ttyDev, char_3 ? "%s/tty%c%c%c" :
			    "%s/tty%c%c", pty_dirs_ptr->pty_dir,
			    *char_1, *char_2, char_3 ? *char_3 : 0);
		    (void) sprintf(ptyDev, char_3 ? "%s/pty%c%c%c" :
			    "%s/pty%c%c", pty_dirs_ptr->ptym_dir,
			    *char_1, *char_2, char_3 ? *char_3 : 0);
		    ptyFd = -1;
		    ttyFd = -1;
		    errno = 0;
		    if ((ptyFd = open(ptyDev, O_RDWR, 0)) >= 0) {
			if ((ttyFd = open(ttyDev, O_RDWR | O_NOCTTY, 0)) < 0) {
			    if (isDebugSet('p'))
				(void) perror(ttyDev);
			} else {
			    /* Success...
			     */
			    char *c1;
			    _Xttynameparams tty_buf;

			    /* use ttyname so that we get the same
			     * name for the tty that everyone else will
			     * use...
			     */
#if defined(XTHREADS)
			    if (c1 = _XTtyname(ttyFd, tty_buf)) {
#else
			    if (c1 = _XTtyname(ttyFd)) {
#endif
				ttyDev = realloc(ttyDev, strlen(c1) + 1);
				(void) strcpy(ttyDev, c1);
			    }

			    /* change the ownership and mode of the pty.
			     * This allows us to access the pty when we
			     * are no longer suid root...
			     */
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
				(void) chown(ttyDev, getuid(), gid);
				(void) chmod(ttyDev, 0620);
			    }
#else	/* HP_ARCHITECTURE */
			    (void) chown(ttyDev, getuid(), getgid());
			    (void) chmod(ttyDev, 0622);
#endif	/* HP_ARCHITECTURE */

			    /* close off the pty slave... */
			    (void) close(ttyFd);

			    /* add it to the list... */
			    (void) AddPtyInfo(ttyDev);

			    /* return file names and pty master... */
			    *ptySlave = ttyDev;
			    *ptyMaster = ptyDev;
			    return(ptyFd);
			}
		    } else {
			if (isDebugSet('p'))
			    (void) perror(ptyDev);
		    }

		    /* Failed to open...
		     */
		    if (ptyFd >= 0)
			(void) close(ptyFd);
		    if (ttyFd >= 0)
			(void) close(ttyFd);

		    /* If we either were able to open the pty master (i.e.,
		     * slave open failed), or the device was busy, keep
		     * going...
		     */
		    if ((ptyFd < 0) && (errno != EBUSY)) {
			first = False;
		    }

		    if (!char_3)
			break;
		    (void) char_3++;
		    if (!*char_3)
			break;
		}
	    }
	}
    }

    return(-1);
}
#endif /* ALPHA_ARCHITECTURE */

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
#ifdef ALPHA_ARCHITECTURE
    /* code from xterm to setup ownership and permission */
    {
        struct group *ttygrp;
	_Xgetgrparams grp_buf;

        if (ttygrp = _XGetgrnam("tty", grp_buf)) {
           /* change ownership of tty to real uid, "tty" gid */
           chown (ptySlave, getuid(), ttygrp->gr_gid);
           chmod (ptySlave, 0620);
        }
        else {
           /* change ownership of tty to real group and user id */
           chown (ptySlave, getuid(), getgid());
           chmod (ptySlave, 0622);
        }
        endgrent();
    }
#else	/* ALPHA_ARCHITECTURE */
    (void) chown(ptySlave, getuid(), getgid());
    (void) chmod(ptySlave, 0622);
#endif /* ALPHA_ARCHITECTURE */
#endif	/* HP_ARCHITECTURE */
    return 0;
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
    (void) DeletePtyInfo(ptySlave);
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
    DebugF('s', 10, fprintf(stderr, ">>_DtTermPrimPtyCleanup() starting\n"));
    while (ptyInfoHead->next && ptyInfoHead->next->ptyName) {
	DebugF('s', 10, fprintf(stderr, ">>releasing pty \"%s\"\n",
		ptyInfoHead->next->ptyName));
	(void) _DtTermPrimReleasePty(ptyInfoHead->next->ptyName);
    }
    DebugF('s', 10, fprintf(stderr, ">>_DtTermPrimPtyCleanup() finished\n"));
}
