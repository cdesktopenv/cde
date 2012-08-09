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

#if defined(linux) || defined(UXPArchitecture) || (defined(USL) && (OSMAJORVERSION > 1)) || defined(CSRG_BASED)
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
