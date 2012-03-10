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
