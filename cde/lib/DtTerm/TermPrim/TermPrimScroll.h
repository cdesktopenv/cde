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
