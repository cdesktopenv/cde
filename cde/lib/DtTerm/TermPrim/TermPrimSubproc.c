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
static char rcs_id[] = "$TOG: TermPrimSubproc.c /main/11 1998/04/20 12:45:57 mgreess $";
#endif  /* VERBOSE_REV_INFO */
#endif  /* lint */

/*                                                                      *
 * (c) Copyright 1993, 1994, 1996 Hewlett-Packard Company               *
 * (c) Copyright 1993, 1994, 1996 International Business Machines Corp. *
 * (c) Copyright 1993, 1994, 1996 Sun Microsystems, Inc.                *
 * (c) Copyright 1993, 1994, 1996 Novell, Inc.                          *
 * (c) Copyright 1996 Digital Equipment Corporation.			*
 * (c) Copyright 1996 FUJITSU LIMITED.					*
 * (c) Copyright 1996 Hitachi.						*
 */

#include "TermHeader.h"
#include <fcntl.h>
#if defined(ALPHA_ARCHITECTURE) || defined(CSRG_BASED) || defined(LINUX_ARCHITECTURE)
/* For TIOCSTTY definitions */
#include <sys/ioctl.h>
#endif /* ALPHA_ARCHITECTURE */
#include <sys/wait.h>

#include <signal.h>
#include <errno.h>

#define X_INCLUDE_PWD_H
#define X_INCLUDE_UNISTD_H
#define XOS_USE_XT_LOCKING
#include <X11/Xos_r.h>

#include <Xm/Xm.h>
#if defined(HPVUE)
#include <Xv/EnvControl.h>
#else    /* HPVUE */
#include <Dt/EnvControlP.h>
#endif   /* HPVUE */

#include "TermPrimP.h"
#include "TermPrimI.h"
#include "TermPrimGetPty.h"
#include "TermPrimSetPty.h"
#include "TermPrimSubproc.h"
#include "TermPrimDebug.h"
#include "TermPrimSetUtmp.h"
#include "TermPrimUtil.h"

typedef struct _subprocInfo {
    pid_t		pid;
    int			stat_loc;
    Widget		w;
    _termSubprocProc	proc;
    XtPointer		client_data;
    XtSignalId		signal_id;
    struct _subprocInfo *next;
    struct _subprocInfo *prev;
} subprocInfo;

static subprocInfo _subprocHead;
static subprocInfo *subprocHead = &_subprocHead;

static pid_t
FakeFork (void)
{
    static Boolean debugInit = True;
    static int debugForkFailures = 0;
    char *c;

    if (isDebugFSet('f', 10)) {
#ifdef	BBA
#pragma BBA_IGNORE
#endif	/*BBA*/
        _DtTermProcessLock();
	if (debugInit) {
	    if ((c = getenv("dttermDebugForkFailures"))) {
		debugForkFailures = strtol(c, (char **) 0, 0);
		debugInit = 0;
	    }
	}
	if (debugForkFailures > 0) {
	    /* decrement the number of failures... */
	    (void) debugForkFailures--;

	    /* set our error return... */
	    errno = EAGAIN;

	    /* and error out... */
	    _DtTermProcessUnlock();
	    return(-1);
	}
        _DtTermProcessUnlock();
    }

    /* just do a fork()... */
    return(fork());
}

/*ARGSUSED*/
static void
InvokeCallbacks(XtPointer client_data, XtSignalId *id)
{
    subprocInfo *subprocTmp = (subprocInfo *) client_data;

    if (subprocTmp->w && subprocTmp->proc)
      (subprocTmp->proc)(subprocTmp->w, subprocTmp->pid, &subprocTmp->stat_loc);
}

_termSubprocId
_DtTermPrimAddSubproc(Widget		w, 
		      pid_t		pid, 
		      _termSubprocProc	proc,
		      XtPointer		client_data)
{
    subprocInfo *subprocTmp;

    /* malloc a new entry... */
    subprocTmp = (subprocInfo *) XtCalloc(1, sizeof(subprocInfo));

    /* fill in the structures... */
    subprocTmp->pid = pid;
    subprocTmp->w = w;
    subprocTmp->proc = proc;
    subprocTmp->client_data = client_data;
    subprocTmp->signal_id = XtAppAddSignal(XtWidgetToApplicationContext(w), 
					   InvokeCallbacks, subprocTmp);

    /* insert it after the head of the list... */
    _DtTermProcessLock();
    subprocTmp->prev = subprocHead;
    subprocTmp->next = subprocHead->next;
    subprocHead->next = subprocTmp;
    if (subprocTmp->next) {
	subprocTmp->next->prev = subprocTmp;
    }
    _DtTermProcessUnlock();

    /* return the pointer... */
    return((_termSubprocId) subprocTmp);
}

void
_DtTermPrimSubprocRemoveSubproc(Widget w, _termSubprocId id)
{
    subprocInfo *subprocTmp = (subprocInfo *) id;

    /* remove the entry from the linked list...
     */
    /* there will always be a head, so we can always update it... */
    _DtTermProcessLock();
    subprocTmp->w = NULL;
    subprocTmp->prev->next = subprocTmp->next;
    if (subprocTmp->next) {
	subprocTmp->next->prev = subprocTmp->prev;
    }
    _DtTermProcessUnlock();

    XtRemoveSignal(subprocTmp->signal_id);

    /* free our storage... */
    (void) XtFree((char *) subprocTmp);
}

/*ARGSUSED*/
static void
ReapChild(int sig)
{
    pid_t pid;
    int	  stat_loc;
    int   err = errno;

    /* There may be several children waiting. */
    while ((pid = waitpid(-1, &stat_loc, WNOHANG)) > 0)
      DtTermSubprocReap(pid, &stat_loc);

    /*
     * Because our signal handler was installed with sigaction()
     * instead of signal() it should remain installed after it is
     * invoked, even on SVR4 machines.  Otherwise we would need to
     * reinstall it each time, creating a race condition in which
     * signals could be lost. 
     */

    /* Preserve errno, like all good signal handlers should. */
    errno = err;
}

void
_DtTermPrimSetChildSignalHandler(void)
{
    struct sigaction new_action;

    new_action.sa_handler = ReapChild;
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = 0;
#ifdef SA_RESTART 
    new_action.sa_flags |= SA_RESTART;
#endif

    /* Use new sigaction() signal handling semantics, not signal(). */
    (void) sigaction(SIGCHLD, &new_action, (struct sigaction*) NULL);
}

void
DtTermSubprocReap(pid_t pid, int *stat_loc)
{
    /*
     * This procedure has special constraints, since it may be invoked
     * inside a signal handler.  That means it (and anything it calls)
     * can only use POSIX async-signal safe library routines.  A notable
     * omission from the list of reentrant routines is pthread_mutex_lock(),
     * which means we cannot call XtProcessLock() or XtAppLock().
     *
     * That makes it challenging to transfer the pid and stat_loc
     * information out of the signal handler to a routine where it is
     * safe to invoke callbacks.  Storing them in static globals will not
     * work, because overlapping signals may arrive.  The approach used
     * here is imperfect, but the best I could contrive.  We block signals
     * and then search the global data structures without using any locks. 
     * The routines that update the subprocHead list try not to leave it
     * in a transient inconsistent state, but that cannot be guaranteed.
     */

    subprocInfo *subprocTmp;
    sigset_t new_sigs;
    sigset_t old_sigs;

    /* 
     * Block additional SIGCHLD signals temporarily.  This is not
     * necessary if the handler was installed with sigaction(), but we
     * may be called from an application's signal handler, and it may
     * have been installed with signal().
     */
    (void) sigemptyset(&new_sigs);
    (void) sigaddset(&new_sigs, SIGCHLD);
    (void) sigprocmask(SIG_BLOCK, &new_sigs, &old_sigs);

    if (pid > 0) {
	/* find the subprocInfo structure for this subprocess... */
	for (subprocTmp = subprocHead->next; 
	     subprocTmp;
	     subprocTmp = subprocTmp->next) {
	    if (subprocTmp->pid == pid) {
	        if (subprocTmp->w && !subprocTmp->w->core.being_destroyed) {
		    subprocTmp->stat_loc = *stat_loc;
		    XtNoticeSignal(subprocTmp->signal_id);
		}
		break;
	    }
	}
    }

    /* Restore SIGCHLD handling to its original state. */
    (void) sigprocmask(SIG_SETMASK, &old_sigs, NULL);
}

pid_t
_DtTermPrimSubprocExec(Widget		  w,
		       char		 *ptyName,
		       Boolean		  consoleMode,
		       char		 *cwd,
		       char		 *cmd,
		       char		**argv,
		       Boolean		  loginShell)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    static char *defaultCmd = (char *) 0;
    int i;
    int pty;
    pid_t pid;
    char *c;
    int err;
#ifdef	MOVE_FDS
    int saveFd[3];
#else	/* MOVE_FDS */
    int savedStderr;
#endif	/* MOVE_FDS */
    Boolean argvFree = False;
    struct sigaction sa;
    sigset_t ss;
    char buffer[BUFSIZ];
    Widget parent;
    char *namebuf;
    struct passwd * pw;
    _Xgetpwparams pw_buf;
    _Xgetloginparams login_buf;

#ifdef  ALPHA_ARCHITECTURE
    /* merge code from xterm, ignore so that TIOCSWINSZ doesn't block */
    signal(SIGTTOU, SIG_IGN);
#endif /* ALPHA_ARCHITECTURE */

    /* build a default exec command and argv list if one wasn't supplied...
     */
    /* cmd... */
    /* the command will be taken as follows:
     *	    - from the passed in cmd,
     *	    - from $SHELL,
     *	    - from the /etc/passwd entry for the /etc/utmp entry for this
     *        terminal,
     *	    - from the /etc/passwd entry for this userid, or
     *	    - /bin/sh.
     */
    if (!cmd || !*cmd) {
	if (!defaultCmd) {
	    /* from $SHELL... */
	    c = getenv("SHELL");

	    /* if not valid, try the /etc/passwd entry for the username
	     * associated with the /etc/utmp entry for this tty...
	     */
	    if (!c || !*c) {
		/* get the /etc/passwd entry for the username associated with
		 * /etc/utmp...
		 */
		if ((namebuf = _XGetlogin(login_buf)) != NULL) {
		    /* get the user's passwd entry... */
		    pw = _XGetpwnam(namebuf, pw_buf);
		    /* if we weren't able to come up with one for the
		     * username...
		     */
		    if (pw != NULL)
			c = pw->pw_shell;
		}
	    }

	    /* if not valid, try the /etc/passwd entry for the username
	     * associate with the real uid...
	     */
	    if (!c || !*c) {
		/* if we weren't able to come up with one for the userid... */
		pw = _XGetpwuid(getuid(), pw_buf);
		if (pw != NULL) {
		    c = pw->pw_shell;
		}
	    }

	    /* if not valid, use /bin/sh... */
	    if (!c || !*c) {
		c = DEFAULT_SHELL;
	    }

	    /* malloc space for this string.  It will be free'ed in the
	     * destroy function...
	     */
	    defaultCmd = XtMalloc(strlen(c) + 1);
	    (void) strcpy(defaultCmd, c);
	}

	cmd = defaultCmd;
    }

    if (!argv) {
	/* base it on cmd... */
	argv = (char **) XtMalloc(2 * sizeof(char *));
	/* if loginShell is set, then pre-pend a '-' to argv[0].  That's
	 * also why we allocate an extra byte in argv[0]...
	 */
	argv[0] = XtMalloc(strlen(cmd) + 2);
	*argv[0] = '\0';
	if (loginShell) {
	    /* pre-pend an '-' for loginShell... */
	    (void) strcat(argv[0], "-");
	    if ((c = strrchr(cmd, '/'))) {
		strcat(argv[0], ++c);
	    } else {
		strcat(argv[0], cmd);
	    }
	} else {
	    (void) strcat(argv[0], cmd);
	}
	/* null term the list... */
	argv[1] = (char *) 0;

	/* we will need to free it up later... */
	argvFree = True;
    }

#ifdef	OLDCODE
    /* this is left around from when we were using vfork().... */
    /* open the pty slave so that we can set the modes.
     *
     * NOTE: this code depends on support for the O_NOCTTY ioctl.  This
     *     ioctl allows us to open the device without becoming the
     *     session group leader for it.  If that can't be done, it may
     *     be necessary to rethink the way we open the pty slave...
     */
    if ((pty = open(ptyName, O_RDWR | O_NOCTTY, 0)) < 0) {
	(void) perror(ptyName);
	return((pid_t) -1);
    }
#endif	/* OLDCODE */

#ifdef	MOVE_FDS
    /* move fd[0:2] out of the way for now... */
    for (i = 0; i <= 2; i++) {
	saveFd[i] = fcntl(i, F_DUPFD, 3);
	(void) close(i);
    }
#else	/* MOVE_FDS */
    savedStderr = fcntl(2, F_DUPFD, 3);
#endif	/* MOVE_FDS */

    /* set close on exec flags on all files... */
    for (i = 0; i < _NFILE; i++) {
	(void) fcntl(i, F_SETFD, 1);
    }

    /* fork.  We can't use vfork() since we need to do lots of stuff
     * below...
     */
    if (isDebugSet('T')) {
#ifdef	BBA
#pragma BBA_IGNORE
#endif	/*BBA*/
	(void) timeStamp("about to fork()");
    }

    _DtTermProcessLock();
    for (i = 0; ((pid = FakeFork()) < 0) && (i < 10); i++) {
	/* if we are out of process slots, then let's sleep a bit and
	 * try again...
	 */
	if (errno != EAGAIN) {
	    break;
	}

	/* give it a chance to clear up... */
	(void) sleep((unsigned long) 2);
    }

    if (pid < 0) {
	(void) perror("fork()");
#ifdef	OLDCODE
	/* this is left around from when we were using vfork().... */
	(void) close(pty);
#endif	/* OLDCODE */
	return((pid_t) - 1);
    } else if (pid == 0) {
	/* child...
	 */
        _DtTermProcessUnlock();
#if defined(ALPHA_ARCHITECTURE) || defined(CSRG_BASED) || defined(LINUX_ARCHITECTURE)
        /* establish a new session for child */
        setsid();
#else
	/* do a setpgrp() so that we can... */
	(void) setpgrp();
#endif /* ALPHA_ARCHITECTURE */

#if defined(LINUX_ARCHITECTURE)
	/* set the ownership and mode of the pty... */
	(void) _DtTermPrimSetupPty(ptyName, pty);
#endif

	/* open the pty slave as our controlling terminal... */
	pty = open(ptyName, O_RDWR, 0);

	if (pty < 0) {
	    (void) perror(ptyName);
	    (void) _exit(1);
	}

#if defined(ALPHA_ARCHITECTURE) || defined(CSRG_BASED) || defined(LINUX_ARCHITECTURE)
        /* BSD needs to do this to acquire pty as controlling terminal */
        if (ioctl(pty, TIOCSCTTY, (char *)NULL) < 0) {
	    (void) close(pty);
	    (void) perror("Error acquiring pty slave as controlling terminal");
	    /* exit the subprocess */
	    _exit(1);
        }

        /* Do it when no controlling terminal doesn't work for OSF/1 */
        _DtTermPrimPtyGetDefaultModes();
#endif /* ALPHA_ARCHITECTURE */

#if !defined(LINUX_ARCHITECTURE)
	/* set the ownership and mode of the pty... */
	(void) _DtTermPrimSetupPty(ptyName, pty);
#endif /* LINUX_ARCHITECTURE */

	/* apply the ttyModes... */
	_DtTermPrimPtyInit(pty, tw->term.ttyModes, tw->term.csWidth);
	/* set the window size... */
	_DtTermPrimPtySetWindowSize(pty,
		tw->term.columns * tw->term.widthInc +
		(2 * (tw->primitive.shadow_thickness +
		      tw->primitive.highlight_thickness +
		      tw->term.marginWidth)),
		tw->term.rows * tw->term.heightInc +
		(2 * (tw->primitive.shadow_thickness +
		      tw->primitive.highlight_thickness +
		      tw->term.marginHeight)),
		tw->term.rows, tw->term.columns);

	/* if we are in console mode, turn it on... */
	if (consoleMode) {
	    _DtTermPrimPtyConsoleModeEnable(pty);
	}

#ifdef	MOVE_FDS
	/* that should have open'ed into fd 0.  Dup it into fd's 1 and 2... */
	(void) dup(pty);
	(void) dup(pty);
#else	/* MOVE_FDS */
	/* dup pty into fd's 0, 1, and 2... */
	for (i = 0; i < 3; i++) {
	    if (i != pty) {
		(void) close(i);
		(void) dup(pty);
	    }
	}
	if (pty >= 3) {
	    (void) close(pty);
	}
#endif	/* MOVE_FDS */

	/* reset any alarms... */
	(void) alarm(0);

	/* reset all signal handlers... */
	sa.sa_handler = SIG_DFL;
	(void) sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	for (i = 1; i < NSIG; i++) {
	    (void) sigaction(i, &sa, (struct sigaction *) 0);
	}

	/* unblock all signals... */
	(void) sigemptyset(&ss);
	(void) sigprocmask(SIG_SETMASK, &ss, (sigset_t *) 0);

	/*
	** Restore the original (pre-DT) environment, removing any
	** DT-specific environment variables that were added before
	** we...
	*/
#if defined(HPVUE)
#if       (OSMINORVERSION > 01)
	(void) VuEnvControl(VUE_ENV_RESTORE_PRE_VUE);
#endif /* (OSMINORVERSION > 01) */
#else   /* (HPVUE) */  
	(void) _DtEnvControl(DT_ENV_RESTORE_PRE_DT);
#endif  /* (HPVUE) */  
            
	/*
	** set a few environment variables of our own...
	*/
	for (parent = w; !XtIsShell(parent); parent = XtParent(parent))
	    ;
	(void) sprintf(buffer, "%ld", XtWindow(parent));
	_DtTermPrimPutEnv("WINDOWID=", buffer);
	_DtTermPrimPutEnv("DISPLAY=", XDisplayString(XtDisplay(w)));
	if (((DtTermPrimitiveWidget)w)->term.emulationId) {
	    _DtTermPrimPutEnv("TERMINAL_EMULATOR=",
			      ((DtTermPrimitiveWidget)w)->term.emulationId);
	}
		 
	/* set our utmp entry... */
	(void) _DtTermPrimUtmpEntryCreate(w, getpid(),
		((DtTermPrimitiveWidget)w)->term.tpd->utmpId);

	if (isDebugSet('T')) {
#ifdef	BBA
#pragma BBA_IGNORE
#endif	/*BBA*/
	    (void) timeStamp("about to execvp()");
	}

	/* turn off suid forever...
	 */
	_DtTermPrimRemoveSuidRoot();

	/* change to the requested directory... */
	if (cwd && *cwd) {
	    (void) chdir(cwd);
	}

#ifdef	BBA
	_bA_dump();
#endif	/* BBA */
	_DtEnvControl(DT_ENV_RESTORE_PRE_DT);
	(void) execvp(cmd, argv);
	/* if we got to this point we error'ed out.  Let's write out the
	 * error...
	 */
	err = errno;
	/* restore stderr... */
	(void) close(2);
	(void) dup(savedStderr);
	/* restore errno... */
	errno = err;
	(void) perror(cmd);
	/* and we need to exit the subprocess... */
	_exit(1);
    }

    /* parent...
     */
    _DtTermProcessUnlock();
    if (isDebugSet('T')) {
#ifdef	BBA
#pragma BBA_IGNORE
#endif	/*BBA*/
	(void) timeStamp("parent resuming");
    }
#ifdef	MOVE_FDS
    /* DKS: we should check this out and see if it is necessary... */
    (void) close(0);
    (void) close(1);
    (void) close(2);
    /* move fd[0:2] back in place... */
    for (i = 0; i <= 2; i++) {
	if (saveFd[i] >= 0) { 
	    (void) fcntl(saveFd[i], F_DUPFD, i);
	    (void) close(saveFd[i]);
	}
    }
#else	/* MOVE_FDS */
    (void) close(savedStderr);
#endif	/* MOVE_FDS */

    /* clean up malloc'ed memory... */
    if (argvFree) {
	(void) XtFree(argv[0]);
	(void) XtFree((char *) argv);
    }

#ifdef	OLDCODE
    /* since we no longer open it in the parent, we probably don't want
     * to close it either...
     */
    (void) close(pty);
#endif	/* OLDCODE */

    /* assume that our child set up a utmp entry (since we have no way
     * for it to report to us) and add it to the list to cleanup)...
     */
    _DtTermPrimUtmpAddEntry(((DtTermPrimitiveWidget)w)->term.tpd->utmpId);

    return(pid);
}
