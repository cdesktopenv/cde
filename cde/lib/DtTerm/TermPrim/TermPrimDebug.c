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
static char rcs_id[] = "$XConsortium: TermPrimDebug.c /main/4 1996/11/21 19:58:15 drk $";
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

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#if defined(linux) || defined(CSRG_BASED)
# include <stdarg.h>
#else
# include <varargs.h>
#endif
#include "TermHeader.h"
#include "TermPrimDebug.h"
#include <signal.h>

#define X_INCLUDE_STRING_H
#define XOS_USE_NO_LOCKING
#include <X11/Xos_r.h>

unsigned char *debugLevel[256] = { NULL }; /* debug level		*/

#ifdef	BBA
#pragma BBA_IGNORE
#endif	/*BBA*/
int
isDebugFSet(int c, int f)
{
    int i;
    
    _DtTermProcessLock();
    i = ((f < _TERM_MAX_DEBUG_FLAG) ?
	    debugLevel[c % 256] &&
	    (debugLevel[c % 256][__TERM_DEBUG_BYTE(f)] & __TERM_DEBUG_BIT(f)) :
	    0);
    _DtTermProcessUnlock();
    return(i);
}

#ifdef	BBA
#pragma BBA_IGNORE
#endif	/*BBA*/
static
void
catchExit(void)
{
    int a;
    int b;

    a = 1;
    b = a;
    a = b;
    return;
}

typedef struct _argArray {
    char *str;
    struct _argArray *next;
} argArray;

#ifdef	BBA
#pragma BBA_IGNORE
#endif	/*BBA*/
void
setDebugFlags(char *c1)
{
    argArray argHead;
    argArray *argPtr;
    int i1;
    int i2;
    char *c2;
    char *c3;
    char *charList;
    char *flagList;
    char charArray[BUFSIZ];
    unsigned char theseFlags[_TERM_DEBUG_NUM_BYTES];
    char startChar;
    char endChar;
    int startFlag;
    int endFlag;
    Boolean isValid;
    static Boolean catchExitRegistered = False;
    _Xstrtokparams strtok_buf;

    /* ignore null or empty strings... */
    if (!c1 || !*c1) {
	return;
    }

    /* register our catchExit() function... */
    _DtTermProcessLock();
    if (!catchExitRegistered) {
	(void) atexit(catchExit);
	catchExitRegistered = True;
    }

    /* strtok out c1...
     */

    /* don't destroy the original string... */
    c2 = malloc(strlen(c1) + 1);
    (void) strcpy(c2, c1);


    argHead.next = (argArray *) 0;
    argPtr = &argHead;
    while ((c3 = _XStrtok(c2, " \t\n", strtok_buf))) {
	/* null out c2 so we won't restart strtok... */
	c2 = (char *) 0;
	argPtr->next = (argArray *) malloc(sizeof(argArray));
	argPtr = argPtr->next;
	argPtr->next = (argArray *) 0;
	argPtr->str = c3;
    }

    /* run through the linked list and parse each string... */
    for (argPtr = argHead.next; argPtr; argPtr = argPtr->next) {
	charList = argPtr->str;
	flagList = (char *) 0;
	if ((c2 = strchr(argPtr->str, ':'))) {
	    /* null out the ':'... */
	    *c2++ = '\0';
	    flagList = c2;
	}

	/* fill out the char array... */
	for (c2 = charArray; *charList; ) {
	    isValid = False;
	    if (isalpha(*charList)) {
		isValid = True;
		startChar = *charList++;
	    } else {
		startChar = 'A';
	    }

	    if (*charList == '-') {
		isValid = True;
		(void) charList++;
		if (isalpha(*charList)) {
		    endChar = *charList++;
		} else {
		    endChar = 'z';
		}
	    } else {
		endChar = startChar;
	    }

	    if (isValid) {
		while (startChar <= endChar) {
		    *c2++ = startChar++;
		}
		/* null term... */
		*c2 = '\0';
	    } else {
		break;
	    }
	}

	/* fill out theseFlags... */
	(void) memset(theseFlags, '\0', sizeof(theseFlags));
	if (!flagList || !*flagList) {
	    flagList = strdup("-");
	}

	c3 = flagList;
	while ((flagList = _XStrtok(c3, ",", strtok_buf))) {
	    /* null out c3 so we won't restart strtok... */
	    c3 = (char *) 0;
	    if (!flagList || !*flagList) {
		continue;
	    }
	    if (isdigit(*flagList)) {
		startFlag = strtol(flagList, &flagList, 0);
	    } else {
		startFlag = 0;
	    }
	    endFlag = startFlag;

	    if (*flagList == '-') {
		/* we have a range... */
		/* skip over the '-'... */
		(void) flagList++;
		if (isdigit(*flagList)) {
		    endFlag = strtol(flagList, (char **) 0, 0);
		} else {
		    endFlag = _TERM_MAX_DEBUG_FLAG - 1;
		}
	    }

	    if (startFlag < 0) {
		startFlag = 0;
	    }
	    if (endFlag > (_TERM_MAX_DEBUG_FLAG - 1)) {
		endFlag = _TERM_MAX_DEBUG_FLAG - 1;
	    }
	    for (; startFlag <= endFlag; startFlag++) {
		theseFlags[__TERM_DEBUG_BYTE(startFlag)] |=
			__TERM_DEBUG_BIT(startFlag);
	    }
	}

	/* now run through the the char array and or in these flags... */
	for (c2 = charArray; *c2; c2++) {
	    if (isalpha(*c2)) {
		if (!debugLevel[*c2]) {
		    debugLevel[*c2] =
			    (unsigned char *) malloc(_TERM_DEBUG_NUM_BYTES);
		    (void) memset(debugLevel[*c2], '\0', _TERM_DEBUG_NUM_BYTES);
		}
		for (i1 = 0; i1 < _TERM_DEBUG_NUM_BYTES; i1++) {
		    debugLevel[*c2][i1] |= theseFlags[i1];
		}
	    }
	}
    }

    /* dump out a list of set flags... */
    if (isDebugFSet('v', 0)) {
	for (i1 = 0; i1 < 256; i1++) {
	    if (debugLevel[i1]) {
		(void) fprintf(stderr, "debug flag '%c'  ", i1);
		for (i2 = 0; i2 < _TERM_MAX_DEBUG_FLAG; i2++) {
		    if (i2 > 0 && (0 == i2 % 50)) {
			(void) fputs("\n                ", stderr);
		    }
		    if (debugLevel[i1][__TERM_DEBUG_BYTE(i2)] &
			    __TERM_DEBUG_BIT(i2)) {
			(void) putc('X', stderr);
		    } else {
			(void) putc('.', stderr);
		    }
		}
		(void) putc('\n', stderr);
	    }
	}
    }
    _DtTermProcessUnlock();
}

static Boolean timedOut;

#ifdef	BBA
#pragma BBA_IGNORE
#endif	/*BBA*/
/*ARGSUSED*/
static void
ding(int sig)
{
    timedOut = True;
}

#ifdef	BBA
#pragma BBA_IGNORE
#endif	/*BBA*/
void
shortSleep(int msec)
{
    struct sigaction action;
    struct sigaction oldAction;
    sigset_t sigset;
    sigset_t oldSigset;
    struct itimerval itime;
    struct itimerval oldItime;

    _DtTermProcessLock();
    timedOut = False;

    /* block SIGALRM... */
    (void) sigemptyset(&sigset);
    (void) sigaddset(&sigset, SIGALRM);
    (void) sigprocmask(SIG_BLOCK, &sigset, &oldSigset);

    /* set up a singnal handler for SIGALRM... */
    action.sa_handler = ding;
    action.sa_flags = 0;
    (void) sigemptyset(&action.sa_mask);
    (void) sigaction(SIGALRM, &action, &oldAction);

    /* set an alarm... */
    timerclear(&itime.it_interval);
    itime.it_value.tv_usec = msec % 1000000;
    itime.it_value.tv_sec = msec / 1000000;
    (void) setitimer(ITIMER_REAL, &itime, &oldItime);

    /* unblock SIGALRM and wait for a signal... */
    (void) sigprocmask(SIG_BLOCK, (sigset_t *) 0, &sigset);
    (void) sigdelset(&sigset, SIGALRM);
    while (!timedOut) {
	sigsuspend(&sigset);
    }

    /* restore old handler... */
    (void) sigaction(SIGALRM, &oldAction, (struct sigaction *) 0);

    /* restore old signal mask... */
    (void) sigprocmask(SIG_SETMASK, &oldSigset, (sigset_t *) 0);
    _DtTermProcessUnlock();
}

static FILE *timeStampFile = (FILE *) 0;

#ifdef	BBA
#pragma BBA_IGNORE
#endif	/*BBA*/
void
timeStamp(char *msg)
{
    struct timeval tv;
    struct timezone tz;

    _DtTermProcessLock();
    if (!timeStampFile) {
	char *c;
	char buffer[BUFSIZ];

	if (!(c = getenv("timeStampFileName"))) {
	    (void) sprintf(buffer, "timeStamp-%d", getpid());
	    c = buffer;
	}

	if (!(timeStampFile = fopen(c, "a"))) {
	    _DtTermProcessUnlock();
	    return;
	}

	(void) setvbuf(timeStampFile, (char *) 0, _IOLBF, 0);
    }
    _DtTermProcessUnlock();

    (void) gettimeofday(&tv, &tz);
    (void) fprintf(timeStampFile, "%lu %ld %s\n", tv.tv_sec, tv.tv_usec,
	    (msg && *msg) ? msg : "");
}

typedef struct {
    char		 *string;
    int			  value;
} enumName;

static enumName eventTypes[] = {
    {"KeyPress",		2},
    {"KeyRelease",		3},
    {"ButtonPress",		4},
    {"ButtonRelease",		5},
    {"MotionNotify",		6},
    {"EnterNotify",		7},
    {"LeaveNotify",		8},
    {"FocusIn",			9},
    {"FocusOut",		10},
    {"KeymapNotify",		11},
    {"Expose",			12},
    {"GraphicsExpose",		13},
    {"NoExpose",		14},
    {"VisibilityNotify",	15},
    {"CreateNotify",		16},
    {"DestroyNotify",		17},
    {"UnmapNotify",		18},
    {"MapNotify",		19},
    {"MapRequest",		20},
    {"ReparentNotify",		21},
    {"ConfigureNotify",		22},
    {"ConfigureRequest",	23},
    {"GravityNotify",		24},
    {"ResizeRequest",		25},
    {"CirculateNotify",		26},
    {"CirculateRequest",	27},
    {"PropertyNotify",		28},
    {"SelectionClear",		29},
    {"SelectionRequest",	30},
    {"SelectionNotify",		31},
    {"ColormapNotify",		32},
    {"ClientMessage",		33},
    {"MappingNotify",		34},
    {"LASTEvent",		35},
    {NULL},
};

static enumName notifyModes[] = {
    {"NotifyNormal",		0},
    {"NotifyGrab",		1},
    {"NotifyUngrab",		2},
    {"NotifyWhileGrabbed",	3},
    {NULL},
};

static enumName notifyDetails[] = {
    {"NotifyAncestor",		0},
    {"NotifyVirtual",		1},
    {"NotifyInferior",		2},
    {"NotifyNonlinear",		3},
    {"NotifyNonlinearVirtual",	4},
    {"NotifyPointer",		5},
    {"NotifyPointerRoot",	6},
    {"NotifyDetailNone",	7},
    {NULL},
};

static enumName boolTypes[] = {
    {"True",			1},
    {"False",			0},
    {NULL},
};

static char *
enumToName
(
    enumName		 *list,
    int			  value
)
{
    int 		  i1;
    char		  buffer[BUFSIZ];
    static char		 *retBuffer = (char *) 0;

    for (i1 = 0; list[i1].string; i1++) {
	if (list[i1].value == value) {
	    return(list[i1].string);
	}
    }
    (void) sprintf(buffer, "Unknown Value %d", value);
    retBuffer = realloc(retBuffer, strlen(buffer));
    (void) strcpy(retBuffer, buffer);
    return(retBuffer);
}

void
_DtTermPrimDebugDumpEvent
(
    FILE		 *f,
    Widget		  w,
    XEvent		 *ev
)
{
    _DtTermProcessLock();
    (void) fprintf(f, ">>  widget: name=\"%s\"  widget=0x%lx  window=0x%lx\n",
	    XtName(w), w, XtWindow(w));
    (void) fprintf(f, ">> event {\n");
    (void) fprintf(f, ">>     type=%s;\n",
	    enumToName(eventTypes, ev->xany.type));
    (void) fprintf(f, ">>     serial=%lu\n", ev->xany.serial);
    (void) fprintf(f, ">>     send_event=%s\n",
	    enumToName(boolTypes, ev->xany.send_event));
    (void) fprintf(f, ">>     display=0x%lx\n", ev->xany.display);
    (void) fprintf(f, ">>     window=0x%lx\n", ev->xany.window);
    switch (ev->type) {
    case EnterNotify:
    case LeaveNotify:
	(void) fprintf(f, ">>     root=0x%lx\n", ev->xcrossing.root);
	(void) fprintf(f, ">>     subwindow=0x%lx\n", ev->xcrossing.subwindow);
	(void) fprintf(f, ">>     mode=%s\n",
		enumToName(notifyModes, ev->xcrossing.mode));
	(void) fprintf(f, ">>     detail=%s\n",
		enumToName(notifyDetails, ev->xcrossing.detail));
	(void) fprintf(f, ">>     focus=%s\n",
		enumToName(boolTypes, ev->xcrossing.focus));
	break;

    case FocusIn:
    case FocusOut:
	(void) fprintf(f, ">>     mode=%s\n",
		enumToName(notifyModes, ev->xcrossing.mode));
	(void) fprintf(f, ">>     detail=%s\n",
		enumToName(notifyDetails, ev->xcrossing.detail));
	break;
    }
    (void) fprintf(f, ">> }\n");
    _DtTermProcessUnlock();
}
