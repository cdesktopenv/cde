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
/* $XConsortium: PrintI.h /main/1 1996/10/31 11:19:07 drk $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */
#ifndef _DtPrintI_h
#define _DtPrintI_h

#include <X11/Intrinsic.h>

#ifdef __cplusplus
extern "C" {
#endif

/********    Conditionally defined macros for thread safe DtPrint ******/
#ifdef XTHREADS

#define _DtPrintWidgetToAppContext(w) \
        XtAppContext app = XtWidgetToApplicationContext(w)

#define _DtPrintDisplayToAppContext(d) \
        XtAppContext app = XtDisplayToApplicationContext(d)

#define _DtPrintAppLock(app)		XtAppLock(app)
#define _DtPrintAppUnlock(app)		XtAppUnlock(app)
#define _DtPrintProcessLock()		XtProcessLock()
#define _DtPrintProcessUnlock()		XtProcessUnlock()

#else /* XTHREADS */

#define _DtPrintWidgetToAppContext(w)
#define _DtPrintDisplayToAppContext(d)
#define _DtPrintAppLock(app)
#define _DtPrintAppUnlock(app)
#define _DtPrintProcessLock()
#define _DtPrintProcessUnlock()

#endif /* XTHREADS */

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _DtPrintI_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
