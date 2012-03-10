/*
 * $TOG: TermHeader.h /main/3 1999/10/14 16:34:46 mgreess $";
 */

/*                                                                      *
 * (c) Copyright 1993, 1994, 1996 Hewlett-Packard Company               *
 * (c) Copyright 1993, 1994, 1996 International Business Machines Corp. *
 * (c) Copyright 1993, 1994, 1996 Sun Microsystems, Inc.                *
 * (c) Copyright 1993, 1994, 1996 Novell, Inc.                          *
 * (c) Copyright 1996 Digital Equipment Corporation.			*
 * (c) Copyright 1996 FUJITSU LIMITED.					*
 * (c) Copyright 1996 Hitachi.						*
 */

#ifndef	_Dt_TermHeader_h
#define	_Dt_TermHeader_h
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/param.h>	/* for MIN, MAX macros */
#include <Xm/Xm.h>
#include <X11/Xos.h>

#if defined(linux) || defined(UXPArchitecture) || (defined(USL) && (OSMAJORVERSION > 1))
#define _NFILE   FOPEN_MAX
#endif

#ifndef	MIN
#define	MIN(a,b)	(((a) > (b)) ? (b) : (a))
#endif	/* MIN */
#ifndef	MAX
#define	MAX(a,b)	(((a) < (b)) ? (b) : (a))
#endif	/* MAX */

/********    Conditionally defined macros for thread_safe DtTerm ******/
#ifdef XTHREADS
#define _DtTermWidgetToAppContext(w) \
        XtAppContext app = XtWidgetToApplicationContext(w)
#define _DtTermDisplayToAppContext(d) \
        XtAppContext app = XtDisplayToApplicationContext(d)
#define _DtTermAppLock(app) XtAppLock(app)
#define _DtTermAppUnlock(app) XtAppUnlock(app)
#define _DtTermProcessLock() XtProcessLock()
#define _DtTermProcessUnlock() XtProcessUnlock()
#else /* XTHREADS */
#define _DtTermWidgetToAppContext(w)
#define _DtTermDisplayToAppContext(d)
#define _DtTermAppLock(app)
#define _DtTermAppUnlock(app)
#define _DtTermProcessLock()
#define _DtTermProcessUnlock()
#endif /* XTHREADS */

#endif	/* _Dt_TermHeader_h */
/* DON'T ADD ANYTHING AFTER THIS #endif... */
