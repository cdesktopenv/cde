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
/* $XConsortium: Lock.h /main/1 1996/04/13 11:53:37 ageorge $ */
/*                                                                      *
 * (c) Copyright 1996 Hewlett-Packard Company                           *
 * (c) Copyright 1996 International Business Machines Corp.             *
 * (c) Copyright 1996 Sun Microsystems, Inc.                            *
 * (c) Copyright 1996 Novell, Inc.                                      *
 * (c) Copyright 1996 Digital Equipment Corporation.			*
 * (c) Copyright 1996 FUJITSU LIMITED.					*
 * (c) Copyright 1996 Hitachi.						*
 */

#ifndef	_Dt_Lock_h
#define	_Dt_Lock_h

/********    Conditionally defined macros for thread_safe DtHelp ******/
#ifdef XTHREADS
#define _DtHelpWidgetToAppContext(w) \
        XtAppContext app = XtWidgetToApplicationContext(w)
#define _DtHelpDisplayToAppContext(d) \
        XtAppContext app = XtDisplayToApplicationContext(d)
#define _DtHelpAppLock(app) XtAppLock(app)
#define _DtHelpAppUnlock(app) XtAppUnlock(app)
#define _DtHelpProcessLock() XtProcessLock()
#define _DtHelpProcessUnlock() XtProcessUnlock()
#else /* XTHREADS */
#define _DtHelpWidgetToAppContext(w)
#define _DtHelpDisplayToAppContext(d)
#define _DtHelpAppLock(app)
#define _DtHelpAppUnlock(app)
#define _DtHelpProcessLock()
#define _DtHelpProcessUnlock()
#endif /* XTHREADS */

#endif	/* _Dt_Lock_h */
/* DON'T ADD ANYTHING AFTER THIS #endif... */
