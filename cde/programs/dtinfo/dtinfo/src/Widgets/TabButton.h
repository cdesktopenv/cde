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

#endif /* _TabButton_h */
/* DO NOT ADD ANY LINES AFTER THIS #endif */
