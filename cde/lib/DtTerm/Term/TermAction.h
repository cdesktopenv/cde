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
 * $XConsortium: TermAction.h /main/1 1996/04/21 19:15:20 drk $";
 */

/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef	_Dt_TermAction_h
#define	_Dt_TermAction_h

extern void _DtTermActionBeginningOfBuffer(Widget w, XEvent *event,
	String *params, Cardinal *num_params);
extern void _DtTermActionBreak(Widget w, XEvent *event,
	String *params, Cardinal *num_params);
extern void _DtTermActionEndOfBuffer(Widget w, XEvent *event,
	String *params, Cardinal *num_params);
extern void _DtTermActionFunctionKeyExecute(Widget w, XEvent *event,
	String *params, Cardinal *num_params);
extern void _DtTermActionHardReset(Widget w, XEvent *event,
	String *params, Cardinal *num_params);
extern void _DtTermActionInsertLine(Widget w, XEvent *event,
	String *params, Cardinal *num_params);
extern void _DtTermActionMoveCursor(Widget w, XEvent *event,
	String *params, Cardinal *num_params);
extern void _DtTermActionScroll(Widget w, XEvent *event,
	String *params, Cardinal *num_params);
extern void _DtTermActionSoftReset(Widget w, XEvent *event,
	String *params, Cardinal *num_params);
extern void _DtTermActionTab(Widget w, XEvent *event,
	String *params, Cardinal *num_params);
extern void _DtTermActionKeypadKeyExecute(Widget w, XEvent *event,
	String *params, Cardinal *num_params);
extern void _DtTermActionEditKeyExecute(Widget w, XEvent *event,
	String *params, Cardinal *num_params);
extern void _DtTermActionKeypadKeyExecute(Widget w, XEvent *event,
	String *params, Cardinal *num_params);
extern void _DtTermWriteEscSeq(Widget w, char *transmitString);

#endif	/* _Dt_TermAction_h */
/* DON'T ADD ANYTHING AFTER THIS #endif... */
