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
/* $XConsortium: DtWidgetI.h /main/1 1996/03/26 19:53:18 drk $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */
#ifndef _DtWidgetI_h
#define _DtWidgetI_h

#include <X11/Intrinsic.h>

/********    Conditionally defined macros for thread safe DtWidget ******/
#ifdef XTHREADS

#define _DtWidgetToAppContext(w) \
        XtAppContext app = XtWidgetToApplicationContext(w)

#define _DtDisplayToAppContext(d) \
        XtAppContext app = XtDisplayToApplicationContext(d)

#define _DtAppLock(app)		XtAppLock(app)
#define _DtAppUnlock(app)	XtAppUnlock(app)
#define _DtProcessLock()	XtProcessLock()
#define _DtProcessUnlock()	XtProcessUnlock()

#else /* XTHREADS */

#define _DtWidgetToAppContext(w)
#define _DtDisplayToAppContext(d)
#define _DtAppLock(app)
#define _DtAppUnlock(app)
#define _DtProcessLock()
#define _DtProcessUnlock()

#endif /* XTHREADS */

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _DtWidgetI_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
