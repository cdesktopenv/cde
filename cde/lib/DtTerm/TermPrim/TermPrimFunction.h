/*
 * $XConsortium: TermPrimFunction.h /main/1 1996/04/21 19:17:30 drk $";
 */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
#ifndef	_Dt_TermPrimFunction_h
#define	_Dt_TermPrimFunction_h

typedef enum {
    fromAction,
    fromParser,
    fromFunctionKey,
    fromMenu,
    fromOther
} FunctionSource;

typedef void (*TermFunction) (Widget w, int count,
	FunctionSource functionSource);

extern void _DtTermPrimFuncLF(Widget w, int count,
	FunctionSource functionSource);
extern void _DtTermPrimFuncBackspace(Widget w, int count,
	FunctionSource functionSource);
extern void _DtTermPrimFuncCR(Widget w, int count,
	FunctionSource functionSource);

extern void _DtTermPrimFuncNextLine(Widget w, int count,
	FunctionSource functionSource);
extern void _DtTermPrimFuncPreviousLine(Widget w, int count,
	FunctionSource functionSource);
extern void _DtTermPrimFuncBackwardCharacter(Widget w, int count,
	FunctionSource functionSource);
extern void _DtTermPrimFuncForwardCharacter(Widget w, int count,
	FunctionSource functionSource);

extern void _DtTermPrimFuncTab(Widget w, int count,
	FunctionSource functionSource);

extern void _DtTermPrimFuncTabSet(Widget w, int count,
	FunctionSource functionSource);
extern void _DtTermPrimFuncTabClear(Widget w, int count,
	FunctionSource functionSource);
extern void _DtTermPrimFuncTabClearAll(Widget w, int count,
	FunctionSource functionSource);

extern void _DtTermPrimFuncMarginSetLeft(Widget w, int count,
	FunctionSource functionSource);
extern void _DtTermPrimFuncMarginSetRight(Widget w, int count,
	FunctionSource functionSource);
extern void _DtTermPrimFuncMarginClear(Widget w, int count,
	FunctionSource functionSource);

extern void _DtTermPrimFuncRedrawDisplay(Widget w, int count,
	FunctionSource functionSource);
#endif	/* _Dt_TermPrimFunction_h */
/* DON'T ADD ANYTHING AFTER THIS #endif... */
