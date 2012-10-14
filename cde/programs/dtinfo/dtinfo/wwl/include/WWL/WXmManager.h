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
 * $XConsortium: WXmManager.h /main/3 1996/06/11 17:00:56 cde-hal $
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
#ifndef WXmManager_h
#define WXmManager_h

#include "WConstraint.h"
#include <Xm/Xm.h>

class WXmManager : public WConstraint {
public :
		DEFINE_INIT (WXmManager, WConstraint, xmManagerWidgetClass)
	DEFINE_GETTER_SETTER (Foreground,Pixel,"foreground")
	DEFINE_GETTER_SETTER (Background,Pixel,"background")
	DEFINE_GETTER_SETTER (BackgroundPixmap,Pixmap,"backgroundPixmap")
	DEFINE_GETTER_SETTER (BorderWidth,Dimension,"borderWidth")
	DEFINE_GETTER_SETTER (HighlightColor,Pixel,"highlightColor")
	DEFINE_GETTER_SETTER (HighlightPixmap,Pixmap,"highlightPixmap")
	DEFINE_GETTER_SETTER (InitialFocus,Widget,"initialFocus")
	DEFINE_GETTER_SETTER (UnitType,unsigned char,"unitType")
	DEFINE_GETTER_SETTER (ShadowThickness,short,"shadowThickness")
	DEFINE_GETTER_SETTER (TopShadowColor,Pixel,"topShadowColor")
	DEFINE_GETTER_SETTER (TopShadowPixmap,Pixmap,"topShadowPixmap")
	DEFINE_GETTER_SETTER (BottomShadowColor,Pixel,"bottomShadowColor")
	DEFINE_GETTER_SETTER (BottomShadowPixmap,Pixmap,"bottomShadowPixmap")
	DEFINE_CALLBACK (HelpCallback,"helpCallback")
	DEFINE_GETTER_SETTER (UserData,void*,"userData")

// Constraint Resources -- Valid only in a form

	DEFINE_GETTER_SETTER (TopAttachment,unsigned char,"topAttachment")
	DEFINE_GETTER_SETTER (BottomAttachment,unsigned char,"bottomAttachment")
	DEFINE_GETTER_SETTER (LeftAttachment,unsigned char,"leftAttachment")
	DEFINE_GETTER_SETTER (RightAttachment,unsigned char,"rightAttachment")
	DEFINE_GETTER_SETTER (TopWidget,Widget,"topWidget")
	DEFINE_GETTER_SETTER (BottomWidget,Widget,"bottomWidget")
	DEFINE_GETTER_SETTER (LeftWidget,Widget,"leftWidget")
	DEFINE_GETTER_SETTER (RightWidget,Widget,"rightWidget")
	DEFINE_GETTER_SETTER (TopPosition,int,"topPosition")
	DEFINE_GETTER_SETTER (BottomPosition,int,"bottomPosition")
	DEFINE_GETTER_SETTER (LeftPosition,int,"leftPosition")
	DEFINE_GETTER_SETTER (RightPosition,int,"rightPosition")
	DEFINE_GETTER_SETTER (TopOffset,int,"topOffset")
	DEFINE_GETTER_SETTER (BottomOffset,int,"bottomOffset")
	DEFINE_GETTER_SETTER (LeftOffset,int,"leftOffset")
	DEFINE_GETTER_SETTER (RightOffset,int,"rightOffset")
	DEFINE_GETTER_SETTER (Resizable,Boolean,"resizable")

#if XmVersion >= 1002
// Constraint Resources -- Valid only in a frame

   DEFINE_GETTER_SETTER (ChildType,unsigned char,"childType")
   DEFINE_GETTER_SETTER (ChildHorizontalAlignment,unsigned char,"childHorizontalAlignment")
   DEFINE_GETTER_SETTER (ChildHorizontalSpacing,Dimension,"childHorizontalSpacing")
   DEFINE_GETTER_SETTER (ChildVerticalAlignment,unsigned char,"childVerticalAlignment")
#endif

// Constraint resources - valid only in a paned window

   DEFINE_GETTER_SETTER (AllowResize,        Boolean,        "allowResize")
   DEFINE_GETTER_SETTER (PaneMinimum,        Dimension,      "paneMinimum")
   DEFINE_GETTER_SETTER (PaneMaximum,        Dimension,      "paneMaximum")
   DEFINE_GETTER_SETTER (PositionIndex,      short,          "positionIndex")
   DEFINE_GETTER_SETTER (SkipAdjust,         Boolean,        "skipAdjust")

   inline void LeftAttachment( unsigned char to_what, Widget w ) {
      Arg a; a.name = CASTRNAM "leftAttachment"; a.value = (XtArgVal)to_what; _Set(a);
       a.name = CASTRNAM "leftWidget";     a.value = (XtArgVal)w;       _Set(a);
   }

   inline void LeftAttachment( unsigned char to_what, Widget w, int offset ) {
      Arg a; a.name = CASTRNAM "leftAttachment"; a.value = (XtArgVal)to_what; _Set(a);
       a.name = CASTRNAM "leftOffset";     a.value = (XtArgVal)offset;  _Set(a);
       a.name = CASTRNAM "leftWidget";     a.value = (XtArgVal)w;       _Set(a);
   }

   inline void LeftAttachment( unsigned char to_what, int poffset ) {
      Arg a; a.name = CASTRNAM "leftAttachment"; a.value = (XtArgVal)to_what; _Set(a);
      if( to_what == XmATTACH_POSITION ) {
	 a.name = CASTRNAM "leftPosition";
	 a.value = (XtArgVal)poffset;  _Set(a);
      } else {
	 a.name = CASTRNAM "leftOffset";
	 a.value = (XtArgVal)poffset;  _Set(a);
      }
   }

   inline void LeftAttachment( unsigned char to_what, int position,
			      int offset ) {
      Arg a; a.name = CASTRNAM "leftAttachment"; a.value = (XtArgVal)to_what; _Set(a);
      a.name = CASTRNAM "leftPosition"; a.value = (XtArgVal)position;  _Set(a);
      a.name = CASTRNAM "leftOffset";   a.value = (XtArgVal)offset;  _Set(a);
   }

   inline void RightAttachment( unsigned char to_what, Widget w ) {
      Arg a; a.name = CASTRNAM "rightAttachment"; a.value = (XtArgVal)to_what; _Set(a);
       a.name = CASTRNAM "rightWidget";     a.value = (XtArgVal)w;       _Set(a);
   }

   inline void RightAttachment( unsigned char to_what, Widget w, int offset ) {
      Arg a; a.name = CASTRNAM "rightAttachment"; a.value = (XtArgVal)to_what; _Set(a);
       a.name = CASTRNAM "rightOffset";     a.value = (XtArgVal)offset;  _Set(a);
       a.name = CASTRNAM "rightWidget";     a.value = (XtArgVal)w;       _Set(a);
   }

   inline void RightAttachment( unsigned char to_what, int poffset ) {
      Arg a; a.name = CASTRNAM "rightAttachment"; a.value = (XtArgVal)to_what; _Set(a);
      if( to_what == XmATTACH_POSITION ) {
	 a.name = CASTRNAM "rightPosition";
	 a.value = (XtArgVal)poffset;  _Set(a);
      } else {
	 a.name = CASTRNAM "rightOffset";
	 a.value = (XtArgVal)poffset;  _Set(a);
      }
   }

   inline void RightAttachment( unsigned char to_what, int position,
			      int offset ) {
      Arg a; a.name = CASTRNAM "rightAttachment"; a.value = (XtArgVal)to_what; _Set(a);
      a.name = CASTRNAM "rightPosition"; a.value = (XtArgVal)position;  _Set(a);
      a.name = CASTRNAM "rightOffset";   a.value = (XtArgVal)offset;  _Set(a);
   }
 
   inline void TopAttachment( unsigned char to_what, Widget w ) {
      Arg a; a.name = CASTRNAM "topAttachment"; a.value = (XtArgVal)to_what; _Set(a);
       a.name = CASTRNAM "topWidget";     a.value = (XtArgVal)w;       _Set(a);
   }

   inline void TopAttachment( unsigned char to_what, Widget w, int offset ) {
      Arg a; a.name = CASTRNAM "topAttachment"; a.value = (XtArgVal)to_what; _Set(a);
       a.name = CASTRNAM "topOffset";     a.value = (XtArgVal)offset;  _Set(a);
       a.name = CASTRNAM "topWidget";     a.value = (XtArgVal)w;       _Set(a);
   }

   inline void TopAttachment( unsigned char to_what, int poffset ) {
      Arg a; a.name = CASTRNAM "topAttachment"; a.value = (XtArgVal)to_what; _Set(a);
      if( to_what == XmATTACH_POSITION ) {
	 a.name = CASTRNAM "topPosition";
	 a.value = (XtArgVal)poffset;  _Set(a);
      } else {
	 a.name = CASTRNAM "topOffset";
	 a.value = (XtArgVal)poffset;  _Set(a);
      }
   }

   inline void TopAttachment( unsigned char to_what, int position,
			      int offset ) {
      Arg a; a.name = CASTRNAM "topAttachment"; a.value = (XtArgVal)to_what; _Set(a);
      a.name = CASTRNAM "topPosition"; a.value = (XtArgVal)position;  _Set(a);
      a.name = CASTRNAM "topOffset";   a.value = (XtArgVal)offset;  _Set(a);
   }

   inline void BottomAttachment( unsigned char to_what, Widget w ) {
      Arg a; a.name = CASTRNAM "bottomAttachment"; a.value = (XtArgVal)to_what; _Set(a);
       a.name = CASTRNAM "bottomWidget";     a.value = (XtArgVal)w;       _Set(a);
   }

   inline void BottomAttachment( unsigned char to_what, Widget w, int offset ) {
      Arg a; a.name = CASTRNAM "bottomAttachment"; a.value = (XtArgVal)to_what; _Set(a);
       a.name = CASTRNAM "bottomOffset";     a.value = (XtArgVal)offset;  _Set(a);
       a.name = CASTRNAM "bottomWidget";     a.value = (XtArgVal)w;       _Set(a);
   }

   inline void BottomAttachment( unsigned char to_what, int poffset ) {
      Arg a; a.name = CASTRNAM "bottomAttachment"; a.value = (XtArgVal)to_what; _Set(a);
      if( to_what == XmATTACH_POSITION ) {
	 a.name = CASTRNAM "bottomPosition";
	 a.value = (XtArgVal)poffset;  _Set(a);
      } else {
	 a.name = CASTRNAM "bottomOffset";
	 a.value = (XtArgVal)poffset;  _Set(a);
      }
   }

   inline void BottomAttachment( unsigned char to_what, int position,
			      int offset ) {
      Arg a; a.name = CASTRNAM "bottomAttachment"; a.value = (XtArgVal)to_what; _Set(a);
      a.name = CASTRNAM "bottomPosition"; a.value = (XtArgVal)position;  _Set(a);
      a.name = CASTRNAM "bottomOffset";   a.value = (XtArgVal)offset;  _Set(a);
   }


};

#define	NULLWXmManager		WXmManager((Widget)0)

class CXmManager : public CConstraint {
public :
		DEFINE_CINIT(CXmManager, CConstraint, xmManagerWidgetClass)
};

#endif  // WXmManager_h
