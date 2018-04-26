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
