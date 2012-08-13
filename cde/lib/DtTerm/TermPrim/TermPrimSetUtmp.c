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
static char rcs_id[] = "$TOG: TermPrimSetUtmp.c /main/10 1998/04/03 17:11:42 mgreess $";
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

#include "TermPrimDebug.h"
#include "TermHeader.h"
#include <Xm/Xm.h>
#include <TermPrimP.h>
#include <TermPrimSetUtmp.h>
#include <TermPrimUtil.h>

/* for sigprocmask... */
#include <signal.h>

/* for open... */
#include <fcntl.h>

/* for getpw*() and getlogin() calls... */
#define X_INCLUDE_PWD_H
#define X_INCLUDE_NETDB_H
#define X_INCLUDE_UNISTD_H
#define XOS_USE_XT_LOCKING
#include <X11/Xos_r.h>

#ifdef	linux
#define	UT_NO_pututline
#endif	/* sun */

#ifdef	__FreeBSD__
#define	UT_UTMPX
#define	UT_HOST		ut_host
#define	UT_NO_pututline
#endif

#ifdef	sun
#define	UT_UTMPX
#define	UT_HOST		ut_host
#define	UT_HOST_LEN	ut_syslen
#define	UT_NO_pututline
#endif	/* sun */

#ifdef	__hpux
#define	UT_HOST		ut_host
#define UT_ADDR		ut_addr
#endif	/* __hpux */

#ifdef	__AIX
#define	UT_HOST		ut_host
#define	UT_NO_pututline
#endif	/* __AIX */

#ifdef __osf__
#define UT_HOST         ut_host
#define UT_NO_pututline
#endif /* __osf__ */


/* /etc/utmp include files... */
#ifdef	UT_UTMPX
#include <utmpx.h>
#define	getutent		getutxent	
#define	getutid			getutxid
#define	getutline		getutxline
#ifdef	NOTDEF
#define	pututline(entry)	updwtmpx(UTMPX_FILE, entry)
#endif	/* NOTDEF */
#define	pututline		pututxline
#define	setutent		setutxent
#define	endutent		endutxent

#define	utmp			utmpx

#define	ut_time			ut_tv.tv_sec
#else	/* UT_UTMPX */
#include <utmp.h>
#endif	/* UT_UTMPX */

/* gethostbyname include files... */
#include <sys/socket.h>
#include <netinet/in.h>

typedef struct _utmpInfo {
    char *utmpLine;
    struct _utmpInfo *next;
    struct _utmpInfo *prev;
} utmpInfo;

static utmpInfo _utmpInfoHead;
static utmpInfo *utmpInfoHead = &_utmpInfoHead;

void
_DtTermPrimUtmpAddEntry
(
    char		 *utmpLine
)
{
    utmpInfo		 *utmpInfoTmp;
    sigset_t		  newSigs;
    sigset_t		  oldSigs;

    /* malloc a new entry... */
    utmpInfoTmp = (utmpInfo *) XtMalloc(sizeof(utmpInfo));
    (void) memset(utmpInfoTmp, '\0', sizeof(utmpInfo));

    /* fill in the structure... */
    utmpInfoTmp->utmpLine = (char *) XtMalloc(strlen(utmpLine) + 1);
    (void) strcpy(utmpInfoTmp->utmpLine, utmpLine);

    /* insert it after the head of the list...
     */
    /* block all signals... */
    (void) sigfillset(&newSigs);
    (void) sigemptyset(&oldSigs);
    (void) sigprocmask(SIG_BLOCK, &newSigs, &oldSigs);
    /* insert the entry into the list... */
    _DtTermProcessLock();
    utmpInfoTmp->prev = utmpInfoHead;
    utmpInfoTmp->next = utmpInfoHead->next;
    utmpInfoHead->next = utmpInfoTmp;
    if (utmpInfoTmp->next) {
	utmpInfoTmp->next->prev = utmpInfoTmp;
    }
    _DtTermProcessUnlock();
    /* restore signals... */
    (void) sigprocmask(SIG_SETMASK, &oldSigs, (sigset_t *) 0);
}

static void
DeleteUtmpInfo
(
    char		 *utmpLine
)
{
    utmpInfo		 *utmpInfoTmp;
    sigset_t		  newSigs;
    sigset_t		  oldSigs;

    _DtTermProcessLock();

    /* find the entry... */
    for (utmpInfoTmp = utmpInfoHead->next; utmpInfoTmp;
	    utmpInfoTmp = utmpInfoTmp->next) {
	if (!strcmp(utmpInfoTmp->utmpLine, utmpLine)) {
	    break;
	}
    }

    /* did we find anything... */
    if (!utmpInfoTmp) {
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
    utmpInfoTmp->prev->next = utmpInfoTmp->next;
    if (utmpInfoTmp->next) {
	utmpInfoTmp->next->prev = utmpInfoTmp->prev;
    }
    /* restore signals... */
    (void) sigprocmask(SIG_SETMASK, &oldSigs, (sigset_t *) 0);

    /* free up the data... */
    if (utmpInfoTmp->utmpLine) {
	(void) XtFree(utmpInfoTmp->utmpLine);
	utmpInfoTmp->utmpLine = (char *) 0;
    }
    (void) XtFree((char *) utmpInfoTmp);
    _DtTermProcessUnlock();
}

static char *userName = (char *) 0;
Atom _DtTermPrim_XA_UtmpLine = (Atom) 0;
#ifdef	UT_ADDR
static char *localHostname = (char *) 0;
#endif	/* UT_ADDR */

#ifdef	UT_NO_pututline
static struct utmp *
_pututline(struct utmp *ut)
{
    (void) pututline(ut);
    return(ut);
}
#endif	/* UT_NO_pututline */

/* the following things should be done in the parent so that they will
 * be available to all utmp entry creates which are done in the child.
 * If we wait until we do the create, the info will not be passed back
 * to the parent, and we will have to do it each and every time...
 */
void
_DtTermPrimUtmpInit(Widget w)
{
    char buffer[BUFSIZ];
    char *c;
    struct passwd * pw_ret;
    _Xgetpwparams pw_buf;

    char *namebuf;
    _Xgetloginparams login_buf;

#ifdef UT_ADDR
    struct hostent *		name_ret;
    _Xgethostbynameparams	name_buf;
#endif

    _DtTermProcessLock();

    if (!userName) {
/* getpw{uid,nam}_r routines fail on IBM when searching passwd info via NIS.
   The AIX specific code could be removed after IBM fixes the problem. Note
   that there could still be a failure on IBM platform if "USER" env variable
   is not defined. In any case, we would really not want to rely on the env
   variable.
*/
#if defined(XTHREADS) && defined(XUSE_MTSAFE_API) && defined(AIXV3)
      if ((c = getenv("USER")) == NULL) {
#endif
	/* get the user's name from:
	 *	-/etc/utmp if possible,
	 *	-/etc/passwd if not.
	 */
	if ((((namebuf = _XGetlogin(login_buf))) != NULL)
	    && (((pw_ret = _XGetpwnam(namebuf, pw_buf))) != NULL) 
	    && (pw_ret->pw_uid == getuid())) {
	    userName = XtMalloc(strlen(namebuf) + 1);
	    (void) strcpy(userName, namebuf);
	} else if (((pw_ret = _XGetpwuid(getuid(), pw_buf))) != NULL) {
	    userName = XtMalloc(strlen(pw_ret->pw_name) + 1);
	    (void) strcpy(userName, pw_ret->pw_name);
	}
#if defined(XTHREADS) && defined(XUSE_MTSAFE_API) && defined(AIXV3)
      }
      else {
	userName = XtMalloc(strlen(c) + 1);
	(void) strcpy(userName, c);
      }
#endif
    }

    if (!_DtTermPrim_XA_UtmpLine) {
	_DtTermPrim_XA_UtmpLine = XInternAtom(XtDisplay(w), TermUtmpIdString, False);
    }
	
#ifdef	UT_ADDR
    if (!localHostname && !gethostname(buffer, sizeof(buffer)) &&
	(name_ret = _XGethostbyname(buffer, name_buf)) != NULL) {
	localHostname = XtMalloc(strlen(buffer) + 1);
	(void) strcpy(localHostname, buffer);
    }
#endif	/* UT_ADDR */
    _DtTermProcessUnlock();
}

char *
_DtTermPrimUtmpGetUtLine(int pty, char *ptyName)
{
    Boolean closePty = False;
    char *c;

#ifdef	DKS
    /* use the same pty name that everyone else will use (the one
     * returned by ttyname())...
     */
    _Xttynameparams tty_buf;

    /* if we weren't passed a pty, let's try opening ptyName.  By using
     * O_NOCTTY we are able to open the pty without accidentally becoming
     * the session leader for it...
     */
    if ((pty < 0) && ((pty = open(ptyName, O_RDWR | O_NOCTTY, 0)) >= 0)) {
	closePty = True;
    }

    /* if we have a pty, use ttyname to get it's "canonical" name... */
    if ((pty >= 0) && (c = _XTtyname(pty, tty_buf))) {
	ptyName = c;
    }

    /* close the pty if we opened it... */
    if (closePty) {
	(void) close(pty);
	pty = -1;
    }
#endif	/* DKS */

    if (!strncmp(ptyName, "/dev/", strlen("/dev/"))) {
	ptyName = ptyName + strlen("/dev/");
    }

    c = XtMalloc(strlen(ptyName) + 1);
    (void) strcpy(c, ptyName);

    return(c);
}

static char *
UtmpEntryCreate(Widget w, pid_t pid, char *utmpLine)
{
#if !defined(CSRG_BASED) /* XXX */
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    struct termData *tpd = tw->term.tpd;
    struct utmp ut;
    struct utmp *utPtr;
    char *c;
    char *displayName;
    time_t now;
    Boolean closePty = False;
#ifdef	UT_HOST
    char *seat;
#endif	/* UT_HOST */
#ifdef	UT_ADDR
    struct sockaddr_in from;
    int fromLen;
#endif	/* UT_ADDR */

    /* initialize utmp stuff, just incase... */
    (void) _DtTermPrimUtmpInit(w);

    /* set up the entry to search for...
     */

    /* create the ut_line... */
    (void) strncpy(ut.ut_line, utmpLine, sizeof(ut.ut_line));

    ut.ut_type = DEAD_PROCESS;

    /* position to entry in utmp file... */
    (void) setutent();
    if (NULL == (utPtr = getutline(&ut))) {
	/* build a base utmp entry... */
	utPtr = &ut;
#ifdef	__hpux
	if (c = strstr(utmpLine, "tty")) {
	    c += strlen("tty");
	} else if (c = strstr(utmpLine, "pts")) {
	    c += strlen("pts");
	} else {
	    c = utmpLine;
	    if (strlen(utmpLine) > sizeof(utPtr->ut_id)) {
		c += strlen(utmpLine) - sizeof(utPtr->ut_id);
	    }
	}
	(void) strncpy(utPtr->ut_id, c, sizeof(utPtr->ut_id));
#else	/* __hpux */
#if defined(__AIX) || defined(__osf__)
	(void) strncpy(utPtr->ut_id, utmpLine,
		sizeof(utPtr->ut_id));
#else	/* __AIX || __osf__ */
#if defined(linux) || defined(sun) || defined(USL) || defined(__uxp__)
	if (c = strchr(utmpLine, '/')) {
	    c++;
	} else {
	    c = utmpLine;
	}
	(void) strncpy(utPtr->ut_id, c, sizeof(utPtr->ut_id));
#else	/* linux || sun  || USL || __uxp__ */
	error out -- missing code for utPtr->ut_id
#endif	/* sun */
#endif	/* __AIX || __osf__ */
#endif	/* __hpux */
    }

    /* set up the new entry... */
    utPtr->ut_type = USER_PROCESS;
#if !defined(linux)
    utPtr->ut_exit.e_termination = 0;
    utPtr->ut_exit.e_exit = 2;
#endif
		
    (void) strncpy(utPtr->ut_user, (userName && *userName) ? userName : "????",
	    sizeof(utPtr->ut_user));
    (void) strncpy(utPtr->ut_line, utmpLine, sizeof(utPtr->ut_line));
    utPtr->ut_pid = pid;
    (void) time(&now);
    utPtr->ut_time = now;

    /* clear and set the host and addr fields... */
#ifdef	UT_HOST
    (void) memset(utPtr->UT_HOST, '\0', sizeof(utPtr->UT_HOST));
    /* stuff the display name into ut_host.  We will stuff as much as
     * will fit dropping domain chunks as necessary to make it fit.  If
     * we still can't fit with the entire domain removed, we will truncate
     * the display name...
     */
    displayName = DisplayString(XtDisplay(w));
    (void) strncpy(utPtr->UT_HOST, displayName, sizeof(utPtr->UT_HOST));
#ifdef	UT_HOST_LEN
    utPtr->UT_HOST_LEN = strlen(displayName + 1);
#endif	/* UT_HOST_LEN */
    if (sizeof(utPtr->UT_HOST) < strlen(displayName)) {
	/* pull off the seat number... */
	seat = strchr(displayName, ':');
	/* back up through the display name.  Each time we hit a '.' that
	 * signals the start of a new domain chunk, see if we can fit
	 * the display name (less these domain chunks) plus the seat number
	 * into the structure...
	 */
	if (seat - displayName < sizeof(utPtr->UT_HOST)) {
	    c = utPtr->UT_HOST + (seat - displayName) - 1;
	} else {
	    c = utPtr->UT_HOST + sizeof(utPtr->UT_HOST) - 1;
	}
	for (; c >= utPtr->UT_HOST; c--) {
	    /* hit a '.'... */
	    if ((*c == '.') && (c - utPtr->UT_HOST + strlen(seat) <
		    sizeof(utPtr->UT_HOST))) {
		/* everything left of the dot plus the seat will fit!... */
		break;
	    }
	}
	if (c >= utPtr->UT_HOST) {
	    /* we can perform a fit with some domains stripped... */
	    (void) strncpy(c, seat, sizeof(utPtr->UT_HOST) -
		    (c - utPtr->UT_HOST));
	    if ((c - utPtr->UT_HOST) + strlen(seat) < sizeof(utPtr->UT_HOST)) {
		/* null out the end of the host name... */
		utPtr->UT_HOST[c - utPtr->UT_HOST + strlen(seat)] = '\0';
#ifdef	UT_HOST_LEN
		utPtr->UT_HOST_LEN = c - utPtr->UT_HOST + strlen(seat) + 1;
#endif	/* UT_HOST_LEN */
	    }
	} else {
	    /* we can't fit even a single full chunk from the domain.
	     * since it is more important to get the seat number in (the
	     * host can be obtained from the addr), truncate the host.
	     */
	    (void) strncpy(utPtr->UT_HOST - strlen(seat), seat,
		    strlen(seat));
#ifdef	UT_HOST_LEN
	    utPtr->UT_HOST_LEN = strlen(seat);
#endif	/* UT_HOST_LEN */
	}
    }
#endif	/* UT_HOST */

#ifdef	UT_ADDR
    (void) memset(&utPtr->ut_addr, '\0', sizeof(utPtr->ut_addr));

    /* get the canonical host of the X socket... */
    fromLen = sizeof(from);
    if (!getpeername(ConnectionNumber(XtDisplay(w)), &from, &fromLen) &&
	    (from.sin_family == AF_INET)) {
	utPtr->ut_addr = from.sin_addr.s_addr;
    }
#endif	/* UT_ADDR */

    /* write it out... */
    if (_pututline(utPtr)) {
	/* success...
	 */
	(void) endutent();
	return(utmpLine);
    }
    /* failure... */
    (void) endutent();
    return((char *) 0);
#else /* __OpenBSD__ */
    return(utmpLine);
#endif
}

/* this is a public wrapper around the previous function that runs the
 * previous function setuid root...
 */
char *
_DtTermPrimUtmpEntryCreate(Widget w, pid_t pid, char *utmpLine)
{
    char *retValue;

    /* this function needs to be suid root... */
    (void) _DtTermPrimToggleSuidRoot(True);
    retValue = UtmpEntryCreate(w, pid, utmpLine);
    /* we now need to turn off setuid root... */
    (void) _DtTermPrimToggleSuidRoot(False);

    return(retValue);
}

static void
UtmpEntryDestroy(Widget w, char *utmpLine)
{
#if !defined(CSRG_BASED)
    struct utmp ut;
    struct utmp *utPtr;
    time_t now;

    ut.ut_type = USER_PROCESS;
    (void) strncpy(ut.ut_line, utmpLine, sizeof(ut.ut_line));
    (void) setutent();
    if (utPtr = getutline(&ut)) {
	utPtr->ut_type = DEAD_PROCESS;
#if !defined(linux)
	utPtr->ut_exit.e_termination = 0;
	utPtr->ut_exit.e_exit = 0;
#endif
	(void) time(&now);
	utPtr->ut_time = now;
#ifdef	UT_HOST
	(void) memset(utPtr->ut_host, '\0', sizeof(utPtr->ut_host));
#endif	/* UT_HOST */
#ifdef	UT_ADDR
	(void) memset(&utPtr->ut_addr, '\0', sizeof(utPtr->ut_addr));
#endif	/* UT_ADDR */
    	(void) pututline(utPtr);
    }
    (void) endutent();

    (void) DeleteUtmpInfo(utmpLine);
#endif /* !__OpenBSD__ */
}

/* this is a public wrapper around the previous function that runs the
 * previous function setuid root...
 */
void
_DtTermPrimUtmpEntryDestroy(Widget w, char *utmpLine)
{
    /* this function needs to be suid root... */
    (void) _DtTermPrimToggleSuidRoot(True);
    (void) UtmpEntryDestroy(w, utmpLine);
    /* we now need to turn off setuid root... */
    (void) _DtTermPrimToggleSuidRoot(False);
}

void
_DtTermPrimUtmpCleanup()
{
    DebugF('s', 10, fprintf(stderr, ">>_DtTermPrimUtmpCleanup() starting\n"));
    _DtTermProcessLock();
    while (utmpInfoHead->next && utmpInfoHead->next->utmpLine) {
	DebugF('s', 10, fprintf(stderr, ">>resetting utmp for id \"%s\"\n",
		utmpInfoHead->next->utmpLine));
	(void) _DtTermPrimUtmpEntryDestroy((Widget) 0,
		utmpInfoHead->next->utmpLine);
    }
    _DtTermProcessUnlock();
    DebugF('s', 10, fprintf(stderr, ">>_DtTermPrimUtmpCleanup() finished\n"));
}
