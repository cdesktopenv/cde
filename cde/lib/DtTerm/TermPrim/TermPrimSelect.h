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
** $XConsortium: TermPrimSelect.h /main/1 1996/04/21 19:19:28 drk $
*/
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
#ifndef   _Dt_TermPrimSelect_h
#define   _Dt_TermPrimSelect_h

typedef struct _termSelectInfoRec   *TermSelectInfo;

#include "TermPrimBuffer.h"

typedef enum {
    TermSelect_NORMAL,
    TermSelect_RECTANGULAR
} TermSelectType;

#ifdef	   __cplusplus
extern "C" {
#endif	/* __cplusplus */

extern Boolean
_DtTermPrimSelectIsAboveSelection
(
    Widget w,
    short row,
    short col
);

extern void
_DtTermPrimSelectDisownIfNecessary
(
    Widget w
);

extern void
_DtTermPrimSelectResize
(
   Widget w
);

extern void
_DtTermPrimSelectDeleteLine
(
   TermBuffer tb,
   short lines     /* number of lines */
);

extern void
_DtTermPrimSelectMoveLines
(
   Widget w,
   short src,
   short dest,
   short len
);

extern void
_DtTermPrimSelectDeleteLines
(
   Widget w,
   short src,
   short len
);

extern void
_DtTermPrimSelectInsertLines
(
   Widget w,
   short src,
   short len
);

extern 
TermSelectInfo
_DtTermPrimSelectCreate
(
    Widget  w
);

extern
Boolean
_DtTermPrimSelectConvert
(
    Widget          w,
    Atom           *selection,
    Atom           *target,
    Atom           *type,
    XtPointer      *value,
    unsigned long  *length,
    int            *format
);

extern
void
_DtTermPrimSelectDestroy
(
    Widget          w,
    TermSelectInfo  selectInfo
);

extern
Boolean
_DtTermPrimSelectGetSelection
(
    Widget w,
    XmTextPosition *begin,
    XmTextPosition *end
);

extern
Boolean
_termSelectInSelection
(
    Widget  w,
    int     row,
    XmTextPosition *begin,
    short   length,
    short  *selLength
);

extern
void
_DtTermPrimSelectGrabFocus
(
    Widget w,
    XEvent *event,
    String *params,
    Cardinal *num_params
);

extern void
_DtTermPrimSelectExtendStart(
        Widget w,
        XEvent *event,
        String *params,
        Cardinal *num_params );

extern
void
_DtTermPrimSelectExtend
(
    Widget w,
    XEvent *event,
    String *params,
    Cardinal *num_params
);

extern
void
_DtTermPrimSelectExtendEnd
(
    Widget w,
    XEvent *event,
    String *params,
    Cardinal *num_params
);

extern
void
_DtTermPrimSelectInsert
(
    Widget w,
    XEvent *event,
    String *params,
    Cardinal *num_params
);

void
_DtTermPrimSelectLoseSelection
(
    Widget  w,
    Atom   *selection
);

extern
void
_DtTermPrimSelectProcessBDrag 
(
    Widget w,
    XEvent *event,
    String *params,
    Cardinal *num_params
);

extern
void
_DtTermPrimSelectAll
(
    Widget w,
    XEvent *event,
    String *params,
    Cardinal *num_params
);

extern
void
_DtTermPrimSelectPage
(
    Widget w,
    XEvent *event,
    String *params,
    Cardinal *num_params
);

extern
void     
_DtTermPrimSelectPasteClipboardEventIF
(
    Widget w,
    XEvent *event,
    String *params,
    Cardinal *num_params
);

extern
void     
_DtTermPrimSelectCopyClipboardEventIF
(
    Widget w,
    XEvent *event,
    String *params,
    Cardinal *num_params
);

extern
Boolean
_DtTermPrimSelectPasteClipboard
(
      Widget w
);

extern
Boolean
_DtTermPrimSelectCopyClipboard
(
    Widget w,
    Time copy_time
);

extern
void
_DtTermPrimSelectProcessCancel(
        Widget w,
        XEvent *event,
        String *params,
        Cardinal *num_params );

extern
void
_DtTermPrimSelectInitBtnEvents(
        Widget w);

extern
void
_DtTermPrimRenderRefreshTextLinear
(
    Widget          w,
    XmTextPosition  start,
    XmTextPosition  stop
);

extern
XmTextPosition
rowColToPos
(
    DtTermPrimitiveWidget   tw,
    short                   row,
    short                   col
);

extern
void
_DtTermPrimSelectDisown
(
    Widget          w
);

extern
Boolean
_DtTermPrimSelectIsInSelection
(
    Widget      w,
    int         row,
    short       startCol,
    short       width,
    short      *selWidth
);

#ifdef	__cplusplus
} /* close scope of 'extern "C"'... */
#endif	/* __cplusplus */

#ifdef    DEBUG_INCLUDES
#include "TermPrimSelectP.h"
#endif /* DEBUG_INCLUDES */

#endif /* _Dt_TermPrimSelect_h */
/* DON'T ADD ANYTHING AFTER THIS #endif... */

