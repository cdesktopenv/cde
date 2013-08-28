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
 * $XConsortium: TabButton.h /main/3 1996/06/11 16:46:10 cde-hal $
 *
 * Copyright (c) 1992 HaL Computer Systems, Inc.  All rights reserved.
 * UNPUBLISHED -- rights reserved under the Copyright Laws of the United
 * States.  Use of a copyright notice is precautionary only and does not
 * imply publication or disclosure.
 * 
 * This software contains confidential information and trade secrets of HaL
 * Computer Systems, Inc.  Use, disclosure, or reproduction is prohibited
 * without the prior express written permission of HaL Computer Systems, Inc.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions as set forth in subparagraph (c)(l)(ii) of the Rights in
 * Technical Data and Computer Software clause at DFARS 252.227-7013.
 *                        HaL Computer Systems, Inc.
 *                  1315 Dell Avenue, Campbell, CA  95008
 * 
 */

#ifndef _TabButton_h
#define _TabButton_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef XyzIsTabButton
#define XyzIsTabButton(w) XtIsSubclass(w, xyzTabButtonWidgetClass)
#endif

typedef struct
{
    int     reason;
    XEvent  *event;
    Window  window;
    int     click_count;
} XyzTabButtonCallbackStruct;

externalref WidgetClass xyzTabButtonWidgetClass;

typedef struct _XyzTabButtonClassRec *XyzTabButtonWidgetClass;
typedef struct _XyzTabButtonRec      *XyzTabButtonWidget;

extern Widget XyzCreateTabButton (Widget parent, const char *name,
				  ArgList arglist, Cardinal argcount);

#define XtNdisarmDelay _XtSCdisarmDelay
#define XtCDisarmDelay _XtSCDisarmDelay
#define XtNradius _XtSCradius
#define XtCRadius _XtSCRadius

extern char _XtSCdisarmDelay[];
extern char _XtSCDisarmDelay[];
extern char _XtSCradius[];
extern char _XtSCRadius[];

#ifdef __cplusplus
}
#endif

_XFUNCPROTOBEGIN

extern void _XmCalcLabelDimensions(Widget wid);

extern Boolean _XmFontListSearch (XmFontList fontlist, XmStringCharSet charset,
				  short *indx, XFontStruct **font_struct);

extern Boolean _XmDifferentBackground (Widget w, Widget parent);

_XFUNCPROTOEND

#endif /* _TabButton_h */
/* DO NOT ADD ANY LINES AFTER THIS #endif */
