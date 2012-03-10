/*
 * $XConsortium: TermPrimI.h /main/1 1996/04/21 19:17:45 drk $";
 */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
#ifndef	_Dt_TermPrimI_h
#define	_Dt_TermPrimI_h

#include <Xm/Xm.h>
#include <TermPrim.h>
#ifdef	__cplusplus
extern "C" {
#endif	/* __cplusplus */

extern void _DtTermPrimForcePtyRead(XtPointer client_data, XtIntervalId *id);
extern void _DtTermPrimLoopBackData(Widget w, char *data, int dataLength);
extern void _DtTermPrimDrawShadow(Widget w);
extern void _DtTermPrimStartOrStopPtyInput(Widget w);
extern void _DtTermPrimStartOrStopPtyOutput(Widget w);
extern void _DtTermPrimSendInput(Widget w, unsigned char *buffer, int len);
extern void _DtTermPrimInsertCharUpdate(Widget w, DtTermInsertCharMode insertCharMode);
extern void _DtTermPrimWarningDialog(Widget w, char *msg);
extern void _DtTermPrimPutEnv(char *c1, char *c2);
extern void
_DtTermPrimInvokeStatusChangeCallback
(
    Widget			  w
);

extern void
_DtTermPrimGetFontSet
(
    Widget w,
    XmFontList fontList,
    XFontSet *fontSet,
    XFontStruct **font
);

#ifdef	__cplusplus
} /* close scope of 'extern "C"'... */
#endif	/* __cplusplus */

/* DON'T ADD ANYTHING AFTER THIS #endif... */
#endif	/* _Dt_TermPrimI_h */
