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
 * $XConsortium: TermView.h /main/1 1996/04/21 19:20:17 drk $";
 */

/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef	_Dt_TermView_h
#define	_Dt_TermView_h

#include <Xm/Xm.h>
#include "TermPrim.h"
#ifdef	__cplusplus
extern "C" {
#endif	/* __cplusplus */

typedef enum {
    DtTermTERM_WIDGET,
    DtTermMENUBAR_WIDGET,
    DtTermSCROLLBAR_WIDGET,
    DtTermSTATUS_WIDGET
} _DtTermViewChildType;

#define	TermRECOLOR_FUNCTIONKEY_AREA	(1 << 0)
#define	TermRECOLOR_FUNCTIONKEY_BUTTONS	(1 << 1)
#define	TermRECOLOR_STATUS_AREA		(1 << 2)

/* Term widget... */
externalref WidgetClass dtTermViewWidgetClass;

typedef struct _DtTermViewClassRec *DtTermViewWidgetClass;
typedef struct _DtTermViewRec      *DtTermViewWidget;

/* public function declarations... */

typedef struct {
    int reason;
    XEvent *event;
    Arg *arglist;
    int argcount;
    pid_t pid;
    int status;
} DtTermViewCallbackStruct;

extern Widget DtCreateTermView(
	Widget parent,
	char *name,
	ArgList arglist,
	Cardinal argcount);
extern Boolean DtTermViewGetCloneEnabled(Widget w);
extern void DtTermViewCloneCallback(
	Widget w,
	XtPointer client_data,
	XtPointer call_data);
extern void _DtTermViewMapHelp(Widget w, char *volume, char *id);
extern Widget _DtTermViewGetChild(Widget w, _DtTermViewChildType child);

#ifdef	__cplusplus
} /* close scope of 'extern "C"'... */
#endif	/* __cplusplus */

#ifndef DtNfunctionKeys
#define	DtNfunctionKeys		"functionKeys"
#endif
#ifndef DtNmenuBar
#define	DtNmenuBar		"menuBar"
#endif
#ifndef DtNmenuPopup
#define	DtNmenuPopup		"menuPopup"
#endif
#ifndef DtNnewCallback
#define	DtNnewCallback		"newCallback"
#endif
#ifndef DtNscrollBar
#define	DtNscrollBar		"scrollBar"
#endif
#ifndef DtNspacing
#define	DtNspacing		XmNspacing
#endif
#ifndef DtNuserFontList
#define	DtNuserFontList		"userFontList"
#endif
#ifndef DtCFunctionKeys
#define	DtCFunctionKeys		"FunctionKeys"
#endif
#ifndef DtCMenuBar
#define	DtCMenuBar		"MenuBar"
#endif
#ifndef DtCMenuPopup
#define	DtCMenuPopup		"MenuPopup"
#endif
#ifndef DtCSpacing
#define	DtCSpacing		XmCSpacing
#endif
#ifndef DtCScrollBar
#define	DtCScrollBar		"ScrollBar"
#endif
#ifndef DtCUserFontList
#define	DtCUserFontList		"UserFontList"
#endif

#endif	/* _Dt_TermView_h */

/* DON'T ADD ANYTHING AFTER THIS #endif... */
