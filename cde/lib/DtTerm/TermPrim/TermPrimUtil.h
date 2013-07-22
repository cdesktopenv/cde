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
 * $XConsortium: TermPrimUtil.h /main/2 1996/09/04 17:34:57 rswiston $";
 */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
#ifndef	_Dt_TermPrimUtil_h
#define	_Dt_TermPrimUtil_h

#include <Xm/Xm.h>
#ifdef	__cplusplus
extern "C" {
#endif	/* __cplusplus */

extern void _DtTermPrimPointerOn(Widget w);
extern void _DtTermPrimPointerOff(Widget w, XtIntervalId *id) ;
extern void _DtTermPrimPointerFreeze(Widget w, Boolean frozen) ;
extern void _DtTermPrimRecolorPointer(Widget w) ;
extern void _DtTermWriteLog(DtTermPrimitiveWidget tw, char *buffer, int len) ;
extern void _DtTermStartLog(Widget w) ;
extern void _DtTermCloseLog(Widget w) ;
extern void _DtTermPrimFlushLog(Widget w);
extern void _DtTermPrimLogFileCleanup(void);

extern void _DtTermPrimRemoveSuidRoot();
extern void _DtTermPrimToggleSuidRoot(Boolean root);
extern void _DtTermPrimStartLog(Widget w);
extern void _DtTermPrimCloseLog(Widget w);
extern void _DtTermPrimWriteLog(DtTermPrimitiveWidget tw, char *buffer, int cnt);

#endif	/* _Dt_TermPrimUtil_h */

/* DON'T ADD ANYTHING AFTER THIS #endif... */
