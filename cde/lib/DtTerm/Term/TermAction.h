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

#endif	/* _Dt_TermAction_h */
/* DON'T ADD ANYTHING AFTER THIS #endif... */
