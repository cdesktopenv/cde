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
 * $XConsortium: TermPrimAction.h /main/1 1996/04/21 19:16:47 drk $";
 */

/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef	_Dt_TermPrimAction_h
#define	_Dt_TermPrimAction_h

extern void _DtTermPrimActionKeyInput(Widget w, XEvent *event,
	String *params, Cardinal *num_params);
extern void _DtTermPrimActionKeyRelease(Widget w, XEvent *event,
	String *params, Cardinal *num_params);
extern void _termActionGrabFocus(Widget w, XEvent *event,
	String *params, Cardinal *num_params);
extern void _DtTermPrimActionLeave(Widget w, XEvent *event,
	String *params, Cardinal *num_params);
extern void _DtTermPrimActionEnter(Widget w, XEvent *event,
	String *params, Cardinal *num_params);
extern void _DtTermPrimActionFocusIn(Widget w, XEvent *event,
	String *params, Cardinal *num_params);
extern void _DtTermPrimActionFocusOut(Widget w, XEvent *event,
	String *params, Cardinal *num_params);
extern void _DtTermPrimActionInsert(Widget w, XEvent *event,
	String *params, Cardinal *num_params);
extern void _DtTermPrimActionKeymap(Widget w, XEvent *event,
	String *params, Cardinal *num_params);
extern void _DtTermPrimActionRedrawDisplay(Widget w, XEvent *event,
	String *params, Cardinal *num_params);
extern void _DtTermPrimActionReturn(Widget w, XEvent *event,
	String *params, Cardinal *num_params);
extern void _DtTermPrimActionStop(Widget w, XEvent *event,
	String *params, Cardinal *num_params);
extern void _DtTermPrimActionString(Widget w, XEvent *event,
	String *params, Cardinal *num_params);
extern void _DtTermPrimActionTab(Widget w, XEvent *event,
	String *params, Cardinal *num_params);
#endif	/* _Dt_TermPrimAction_h */
/* DON'T ADD ANYTHING AFTER THIS #endif... */
