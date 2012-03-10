/*
 * $XConsortium: TermFunction.h /main/1 1996/04/21 19:15:50 drk $";
 */

/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef	_Dt_TermFunction_h
#define	_Dt_TermFunction_h

#include "TermPrimFunction.h"

extern void _DtTermFuncScroll(Widget w, int count,
	FunctionSource functionSource);
extern void _DtTermFuncBeginningOfBuffer(Widget w, int count,
	FunctionSource functionSource);
extern void _DtTermFuncEndOfBuffer(Widget w, int count,
	FunctionSource functionSource);

extern void _DtTermFuncEraseInLine(Widget w, int count,
	FunctionSource functionSource);
extern void _DtTermFuncEraseInDisplay(Widget w, int count,
	FunctionSource functionSource);
extern void _DtTermFuncEraseCharacter(Widget w, int count,
	FunctionSource functionSource);

extern void _DtTermFuncClearToEndOfBuffer(Widget w, int count,
	FunctionSource functionSource);
extern void _DtTermFuncClearBuffer(Widget w, int count,
	FunctionSource functionSource);
extern void _DtTermFuncClearLine(Widget w, int count,
	FunctionSource functionSource);
extern void _DtTermFuncClearToEndOfLine(Widget w, int count,
	FunctionSource functionSource);

extern void _DtTermFuncHardReset(Widget w, int count,
	FunctionSource functionSource);
extern void _DtTermFuncSoftReset(Widget w, int count,
	FunctionSource functionSource);

extern void _DtTermFuncDeleteChar(Widget w, int count,
	FunctionSource functionSource);
extern void _DtTermFuncDeleteCharWrap(Widget w, int count,
	FunctionSource functionSource);

extern void _DtTermFuncInsertLine(Widget w, int count,
	FunctionSource functionSource);

extern void _DtTermFuncDeleteLine(Widget w, int count,
	FunctionSource functionSource);

extern void _DtTermFuncTab(Widget w, int count,
	FunctionSource functionSource);

extern void _DtTermVideoEnhancement(Widget w,int value);

extern void _DtTermFontEnhancement(Widget w,int value);

extern void _DtTermSetUserKeyLock(Widget w,Boolean lock_state);
extern Boolean _DtTermGetUserKeyLock(Widget w);
extern void _DtTermSetAutoLineFeed(Widget w,Boolean alf_state);
extern Boolean _DtTermGetAutoLineFeed(Widget w);
extern void _DtTermSetCurrentWorkingDirectory(Widget w, char *cwd);
extern char *_DtTermGetCurrentWorkingDirectory(Widget w);

#endif	/* _Dt_TermFunction_h */
/* DON'T ADD ANYTHING AFTER THIS #endif... */

