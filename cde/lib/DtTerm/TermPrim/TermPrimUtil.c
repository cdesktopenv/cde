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
static char rcs_id[] = "$TOG: TermPrimUtil.c /main/3 1997/04/17 18:25:29 samborn $";
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
#include "TermPrimOSDepI.h"
#include "TermPrimP.h"
#include "TermPrimData.h"
#include "TermPrimUtil.h"
#include "TermPrimDebug.h"
#include "Xm/Xm.h"
#include <errno.h>
#include <signal.h>
#include <sys/file.h>

void _DtTermPrimRemoveSuidRoot();
static void ForceCloseLog(DtTermPrimitiveWidget);
static void PointerMoved(Widget w, XtPointer closure, XEvent *event, Boolean *cont) ;
void _DtTermPrimPointerOff(Widget w, XtIntervalId *id) ;

/*
** the following white pixmap is used to create the noPointer (blank) pointer.
*/
#define whiteW 16
#define whiteH 16
static char whiteBits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static void
InitPointerBlank(Widget w)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    DtTermPrimData tpd = tw->term.tpd;
    XColor fg = { 0, 0, 0, 0, DoRed | DoGreen | DoBlue, 0 };
    XColor bg = { 0, 0, 0, 0, DoRed | DoGreen | DoBlue, 0 };
    Pixmap noPointerBitmap;

    /*
    ** set a pointer motion handler...
    */
    tpd->pointerTimeoutID = 0 ;
    tpd->pointerFrozen = True ;
    fg.pixel = bg.pixel = BlackPixelOfScreen(XtScreen(w));
    noPointerBitmap = XCreateBitmapFromData(XtDisplay(w),XtWindow(tw),
                                whiteBits, whiteW, whiteH);

    tpd->noPointer   = XCreatePixmapCursor(XtDisplay(tw),
                                   noPointerBitmap,  /* source bitmap    */
                                   noPointerBitmap,  /* mask bitmap      */
                                   &fg, &bg,	     /* Do not care	 */
				   0, 0);	     /* hotspot		 */

    XFreePixmap(XtDisplay(w), noPointerBitmap);
    XtAddEventHandler((Widget)tw, PointerMotionMask, FALSE, PointerMoved, 
                                       (XtPointer)NULL);

    tpd->pointerFirst = False ;
}

static void 
PointerMoved(Widget w, XtPointer closure, XEvent *event, Boolean *cont)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    DtTermPrimData tpd = tw->term.tpd;

    if (!tpd->pointerFrozen) {
        if (tpd->pointerOn) {
            /*
            ** pointer is on, just reset the timer...
            */
            if (tw->term.pointerBlankDelay) {
                /*
                ** remove the old motion timeout...
                */
                if (tpd->pointerTimeoutID)
                    XtRemoveTimeOut(tpd->pointerTimeoutID);

                /*
                ** and set a new motion timeout...
                */
                tpd->pointerTimeoutID = 
                      XtAppAddTimeOut(XtWidgetToApplicationContext((Widget)tw),
                      (unsigned long) 1000 * tw->term.pointerBlankDelay,
                      (XtTimerCallbackProc) _DtTermPrimPointerOff,
                      (XtPointer)tw);
            }
        } else {
            /* pointer is off, turn it on... */
            (void) _DtTermPrimPointerOn(w);
        }
    }
}

void
_DtTermPrimPointerOff(Widget w, XtIntervalId *id)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    DtTermPrimData tpd = tw->term.tpd;

    if (tpd->pointerFirst) InitPointerBlank(w) ;

    if (tpd->pointerOn) {
        /*
        ** define the window's cursor...
        */
        (void) XDefineCursor(XtDisplay(tw), XtWindow(tw), tpd->noPointer);

        /*
        ** remove the motion timeout...
        */
        if (tw->term.pointerBlankDelay) {
            if (tpd->pointerTimeoutID)
                XtRemoveTimeOut(tpd->pointerTimeoutID);
            tpd->pointerTimeoutID = 0;
        }

        /*
        ** and clear the pointer on flag...
        */
        tpd->pointerOn = False;
    }
    return;
}

void 
_DtTermPrimPointerOn(Widget w)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    DtTermPrimData tpd = tw->term.tpd;

    if (tpd->pointerFirst) InitPointerBlank(w) ;

    if (!tpd->pointerOn) {
        /*
        ** define the window's cursor...
        */
        XDefineCursor(XtDisplay(tw), XtWindow(tw), tw->term.pointerShape);

        /*
        ** set a motion timeout...
        */
        if (tw->term.pointerBlankDelay) {
            if (tpd->pointerTimeoutID)
                /*
                ** remove old timeout...
                */
                XtRemoveTimeOut(tpd->pointerTimeoutID);

            tpd->pointerTimeoutID = 
                    XtAppAddTimeOut(XtWidgetToApplicationContext((Widget)tw),
                    (unsigned long) (1000 * tw->term.pointerBlankDelay),
                    (XtTimerCallbackProc)_DtTermPrimPointerOff, (XtPointer)tw);
        }
        /*
        ** and set the pointer on flag...
        */
        tpd->pointerOn = True;
    }
    return;
}

void 
_DtTermPrimPointerFreeze(Widget w, Boolean freeze)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    DtTermPrimData tpd = tw->term.tpd;

    tpd->pointerFrozen = freeze;

    /*
    ** make sure that the pointer is on...
    */
    if (tpd->pointerOn) {
        /*
        ** pointer is on...
        */
        if (tw->term.pointerBlankDelay) {
            if (freeze) {
                /*
                ** freezing -- turn the timeout off...
                */
                if (tpd->pointerTimeoutID)
                    XtRemoveTimeOut(tpd->pointerTimeoutID);

                tpd->pointerTimeoutID = 0;
            } 
            else {
                /*
                ** un freezing -- turn the timeout on...
                */
                if (tpd->pointerTimeoutID)
                    /*
                    ** remove old timeout...
                    */
                    XtRemoveTimeOut(tpd->pointerTimeoutID);

                tpd->pointerTimeoutID = 
                       XtAppAddTimeOut(XtWidgetToApplicationContext((Widget)tw),
                       (unsigned long) 1000 * tw->term.pointerBlankDelay,
                       (XtTimerCallbackProc) _DtTermPrimPointerOff,
                       (XtPointer)tw);
            }
        }
    } 
    else {
        /*
        ** let's turn on the pointer...
        ** define the window's cursor...
        */
        XDefineCursor(XtDisplay(tw), XtWindow(tw), tw->term.pointerShape);

        if (freeze)
            /*
            ** the timeout is off, so we don't need to clear it...
            */
            /* NOOP */ ;
        else
            /*
            ** we are unfreezing -- turn the timeout on...
            */
            if (tw->term.pointerBlankDelay) {
                if (tpd->pointerTimeoutID)
                    /*
                    ** remove old timeout...
                    */
                    XtRemoveTimeOut(tpd->pointerTimeoutID);

                tpd->pointerTimeoutID = 
                      XtAppAddTimeOut(XtWidgetToApplicationContext((Widget)tw),
                      (unsigned long) (1000 * tw->term.pointerBlankDelay),
                      (XtTimerCallbackProc)_DtTermPrimPointerOff,(XtPointer)tw);
            }
        /*
        ** and set the flag...
        */
        tpd->pointerOn = True;
    }
    return;
}

void
_DtTermPrimRecolorPointer(Widget w)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;

    XColor colordefs[2];        /* 0 is foreground, 1 is background */
    Display *dpy = XtDisplay(w);

    colordefs[0].pixel = tw->term.pointerColor;
    colordefs[1].pixel = tw->term.pointerColorBackground;
    XQueryColors (dpy, DefaultColormap (dpy, DefaultScreen (dpy)),
          colordefs, 2);
    XRecolorCursor (dpy, tw->term.pointerShape, colordefs, colordefs+1);
    return;
}

/* linked list of log files to flush if we are killed...
 */
typedef struct _logInfo {
    FILE *logFile;
    struct _logInfo *next;
    struct _logInfo *prev;
} logInfo;

static logInfo _logInfoHead;
static logInfo *logInfoHead = &_logInfoHead;

static void
AddLogFileEntry
(
    FILE		 *logFile
)
{
    logInfo		 *logInfoTmp;
    sigset_t		  newSigs;
    sigset_t		  oldSigs;

    /* malloc a new entry... */
    logInfoTmp = (logInfo *) XtMalloc(sizeof(logInfo));
    (void) memset(logInfoTmp, '\0', sizeof(logInfo));

    /* fill in the structure... */
    logInfoTmp->logFile = logFile;

    /* insert it after the head of the list...
     */
    /* block all signals... */
    (void) sigfillset(&newSigs);
    (void) sigemptyset(&oldSigs);
    (void) sigprocmask(SIG_BLOCK, &newSigs, &oldSigs);

    /* insert the entry into the list... */
    _DtTermProcessLock();
    logInfoTmp->prev = logInfoHead;
    logInfoTmp->next = logInfoHead->next;
    logInfoHead->next = logInfoTmp;
    if (logInfoTmp->next) {
	logInfoTmp->next->prev = logInfoTmp;
    }
    _DtTermProcessUnlock();

    /* restore signals... */
    (void) sigprocmask(SIG_SETMASK, &oldSigs, (sigset_t *) 0);
}

static void
DeleteLogFileEntry
(
    FILE		 *logFile
)
{
    logInfo		 *logInfoTmp;
    sigset_t		  newSigs;
    sigset_t		  oldSigs;

    /* find the entry... */
    _DtTermProcessLock();
    for (logInfoTmp = logInfoHead->next; logInfoTmp;
	    logInfoTmp = logInfoTmp->next) {
	if (logInfoTmp->logFile == logFile) {
	    break;
	}
    }

    /* did we find anything... */
    if (!logInfoTmp) {
	/* not found... */
        _DtTermProcessUnlock();
	return;
    }

    /* delete entry from the list...
     */
    /* block all signals... */
    (void) sigfillset(&newSigs);
    (void) sigemptyset(&oldSigs);
    (void) sigprocmask(SIG_BLOCK, &newSigs, &oldSigs);

    /* remove it... */
    logInfoTmp->prev->next = logInfoTmp->next;
    if (logInfoTmp->next) {
	logInfoTmp->next->prev = logInfoTmp->prev;
    }

    /* restore signals... */
    (void) sigprocmask(SIG_SETMASK, &oldSigs, (sigset_t *) 0);

    /* free up the data... */
    (void) XtFree((char *) logInfoTmp);
    _DtTermProcessUnlock();
}

#ifdef NOTDEF
void 
logpipe(Widget w)
{
    win_data *wp = &term->Wp;

    if (wp->log_on) { CloseLog(wp); }
}
#endif  /* NOTDEF */


void
_DtTermPrimStartLog(Widget w)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    DtTermPrimData tpd = tw->term.tpd;

    char            *cp;
    int              i;

    if ( tw->term.log_on || tw->term.logInhibit ) { return; }

    if (!tw->term.logFile || !*tw->term.logFile) {
	tw->term.logFile = "DttermLogXXXXX";
    }

    if (!strcmp(tw->term.logFile + strlen(tw->term.logFile) - 5, "XXXXX")) {
	/* make a local copy in case we are going to change it... */
	cp = XtMalloc(strlen(tw->term.logFile) + 1);
	(void) strcpy(cp, tw->term.logFile);

        (void) mktemp(cp);
	if (cp && *cp) {
	    tw->term.logFile = cp;
	} else {
	    (void) XtFree(cp);
	    return;
	}
    }

    if ('|' == *tw->term.logFile ) {
        /*
        ** pipe logfile into command
        */
        int p[2];

	_DtTermProcessLock();
        if (pipe(p) < 0 || (i = fork()) < 0) {
	    _DtTermProcessUnlock();
	    return;
	}

        if (i == 0) {
            /*
            ** child
            */
	    _DtTermProcessUnlock();

	    /* Remove suid root capability...
	     */
	    (void) _DtTermPrimRemoveSuidRoot();

            (void) close(p[1]);
            (void) close(0);
            (void) dup(p[0]);
            (void) close(p[0]);
            /*
            ** set close on exec flag on all other fd's
            */            for (i = 3; i < _NFILE; i++) {
                (void) fcntl(i, F_SETFD, 1);
            }
            /*
            ** reset signals
            */
            (void) signal(SIGHUP, SIG_DFL);
            (void) signal(SIGCHLD, SIG_DFL);
#ifdef	BBA
	    _bA_dump();
#endif	/* BBA */
            (void) execl(DEFAULT_SHELL, DEFAULT_SHELL_ARGV0, 
                         "-c", &tw->term.logFile[1], NULL);
            (void) fprintf(stderr, " Can't exec \"%s\"\n",
                                       &tw->term.logFile[1]);
            (void) exit(1);
        }

        _DtTermProcessUnlock();
        (void) close(p[0]);
        tpd->logStream = fdopen(p[1], "w");
	(void) AddLogFileEntry(tpd->logStream);
        (void) signal(SIGPIPE, SIG_IGN);
    }
    else {
        if (access(tw->term.logFile, F_OK) == 0) {
            if (access(tw->term.logFile, W_OK) < 0) {
		return;
	    }
        } else if ((cp = strrchr(tw->term.logFile, '/'))) {
            *cp = 0;
            i   = access(tw->term.logFile, W_OK);
            *cp = '/';
            if (i < 0) {
		return;
	    }
        } else if (access(".", W_OK) < 0) {
	    return;
	}
        if ((tpd->logStream = fopen(tw->term.logFile, "a")) == NULL) {
            return;
        }
	(void) AddLogFileEntry(tpd->logStream);
        (void) chown(tw->term.logFile, getuid(), getgid());
    }
    tw->term.log_on = True ;
}

static void
ForceCloseLog(DtTermPrimitiveWidget tw)
{
    DtTermPrimData tpd = tw->term.tpd;

    if (tw->term.log_on)
    {
	(void) DeleteLogFileEntry(tpd->logStream);
	(void) fclose(tpd->logStream);
        tpd->logStream = NULL;
	tw->term.log_on = False;
    }
}

void
_DtTermPrimCloseLog(Widget w)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    DtTermPrimData tpd = tw->term.tpd;

    /*
    ** if we are not logging, or it is inhibited, do nothing
    */
    if (!tw->term.log_on || tw->term.logInhibit ) { return; }

    (void) fflush(tpd->logStream);
    ForceCloseLog(tw);
}

void
_DtTermPrimWriteLog(DtTermPrimitiveWidget tw, char *buffer, int cnt)
{
    DtTermPrimData tpd = tw->term.tpd;

    if (cnt > 0)
    {
	_DtTermProcessLock();

	(void) fwrite(buffer, cnt, 1, tpd->logStream);

	if ((errno == EPIPE) && ferror(tpd->logStream))
	{
	    ForceCloseLog(tw);
	}

	_DtTermProcessUnlock();
    }
}

static Boolean first = True;
static uid_t uid_user;
static uid_t uid_root;
static gid_t gid_user;
static gid_t gid_root;

static void
suidInit()
{
    _DtTermProcessLock();
    if (first) {
	uid_user = getuid();
	uid_root = geteuid();

	gid_user = getgid();
	gid_root = getegid();

	first = False;
    }
    _DtTermProcessUnlock();
}

void
_DtTermPrimRemoveSuidRoot()
{
    (void) suidInit();
#if	defined(HAS_SETRESUID)
    (void) setresgid(gid_user, gid_user, gid_user);
    (void) setresuid(uid_user, uid_user, uid_user);
#elif	defined(HAS_SETREUID)
    (void) setregid(gid_user, gid_user);
    (void) setreuid(uid_user, uid_user);
#else	/* !HAS_SETRESUID && !HAS_SETREUID */
    (void) setgid(gid_user);
    (void) setuid(uid_user);
#endif	/* !HAS_SETRESUID && !HAS_SETREUID */
}

void
_DtTermPrimToggleSuidRoot(Boolean root)
{
    (void) suidInit();

#if	defined(HAS_SETRESUID)
    (void) setresuid(-1, root ? uid_root : uid_user, -1);
    (void) setresgid(-1, root ? gid_root : gid_user, -1);
#elif	defined(HAS_SETEUID)
    (void) seteuid(root ? uid_root : uid_user);
    (void) setegid(root ? gid_root : gid_user);
#endif	/* HAS_SETEUID */
}

void
_DtTermPrimLogFileCleanup
(
    void
)
{
    logInfo		 *logInfoTmp;

    DebugF('s', 10, fprintf(stderr,
	    ">>_DtTermPrimLogFileCleanup() starting\n"));

    /* flush all the log files... */
    _DtTermProcessLock();
    for (logInfoTmp = logInfoHead->next; logInfoTmp;
	    logInfoTmp = logInfoTmp->next) {
	DebugF('s', 10, fprintf(stderr,
		">>flushing logfile 0x%lx\n", logInfoTmp->logFile));
	(void) fflush(logInfoTmp->logFile);
    }
    _DtTermProcessUnlock();
    DebugF('s', 10, fprintf(stderr,
	    ">>_DtTermPrimLogFileCleanup() finished\n"));
}
