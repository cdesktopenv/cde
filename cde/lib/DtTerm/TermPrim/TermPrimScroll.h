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
/*
 * $XConsortium: TermPrimScroll.h /main/1 1996/04/21 19:19:21 drk $";
 */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
#ifndef	_Dt_TermPrimScroll_h
#define	_Dt_TermPrimScroll_h

extern void _DtTermPrimScrollWait(Widget w);
extern void _DtTermPrimScrollComplete(Widget w, Boolean flush);
extern void _DtTermPrimScrollCompleteIfNecessary(Widget w, short scrollTopRow,
	short scrollBottomRow, short lines);
extern void _DtTermPrimScrollText(Widget w, short lines);
extern void _DtTermPrimScrollTextTo(Widget w, short topRow);
extern void _DtTermPrimScrollTextArea(Widget w, short scrollStart,
	short scrollLength, short scrollDistance);
#endif	/* _Dt_TermPrimScroll_h */
/* DON'T ADD ANYTHING AFTER THIS #endif... */
