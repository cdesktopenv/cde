/*
 * $XConsortium: WXmScrolledWindow.h /main/3 1996/06/11 17:02:07 cde-hal $
 *
 * Copyright (c) 1991 HaL Computer Systems, Inc.  All rights reserved.
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

// This code is automatically generated in -*- C++ -*-
#ifndef WXmScrolledWindow_h
#define WXmScrolledWindow_h

#include "WXmManager.h"
#include "WXmScrollBar.h"
#include <Xm/ScrolledW.h>

class WXmScrolledWindow : public WXmManager {
public :
   DEFINE_INIT (WXmScrolledWindow, WXmManager, xmScrolledWindowWidgetClass)
   DEFINE_GETTER_SETTER (HorizontalScrollBar,Widget,"horizontalScrollBar")
   DEFINE_GETTER_SETTER (VerticalScrollBar,Widget,"verticalScrollBar")
   DEFINE_GETTER_SETTER (WorkWindow,Widget,"workWindow")
   DEFINE_GETTER_SETTER (ClipWindow,Widget,"clipWindow")
   DEFINE_GETTER_SETTER (ScrollingPolicy,unsigned char,"scrollingPolicy")
   DEFINE_GETTER_SETTER (VisualPolicy,unsigned char,"visualPolicy")
   DEFINE_GETTER_SETTER (ScrollBarDisplayPolicy,unsigned char,"scrollBarDisplayPolicy")
   DEFINE_GETTER_SETTER (ScrollBarPlacement,unsigned char,"scrollBarPlacement")
   DEFINE_GETTER_SETTER (ScrolledWindowMarginWidth,short,"scrolledWindowMarginWidth")
   DEFINE_GETTER_SETTER (ScrolledWindowMarginHeight,short,"scrolledWindowMarginHeight")
   DEFINE_GETTER_SETTER (Spacing,Dimension,"spacing")
   DEFINE_GETTER_SETTER (ShadowThickness,short,"shadowThickness")
   inline void SetAreas(WXmScrollBar hsb, WXmScrollBar vsb, WObject work) const
   {
      XmScrolledWindowSetAreas (widget, (Widget)hsb, (Widget)vsb, (Widget)work);
   }
};

#define	NULLWXmScrolledWindow		WXmScrolledWindow((Widget)0)

class CXmScrolledWindow : public CXmManager {
public :
   DEFINE_CINIT(CXmScrolledWindow, CXmManager, xmScrolledWindowWidgetClass)
};

#endif
