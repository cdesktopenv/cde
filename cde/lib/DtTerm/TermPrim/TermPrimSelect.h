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


#ifdef	__cplusplus
} /* close scope of 'extern "C"'... */
#endif	/* __cplusplus */

#ifdef    DEBUG_INCLUDES
#include "TermPrimSelectP.h"
#endif /* DEBUG_INCLUDES */

#endif /* _Dt_TermPrimSelect_h */
/* DON'T ADD ANYTHING AFTER THIS #endif... */

