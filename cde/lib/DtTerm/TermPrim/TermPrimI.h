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
