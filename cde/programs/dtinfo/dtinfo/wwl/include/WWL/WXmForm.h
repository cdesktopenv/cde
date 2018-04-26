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
 * $XConsortium: WXmForm.h /main/3 1996/06/11 17:00:07 cde-hal $
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
#ifndef WXmForm_h
#define WXmForm_h

#include "WXmBulletinBoard.h"
#include <Xm/Form.h>

class WXmForm : public WXmBulletinBoard {
public :
		DEFINE_INIT (WXmForm, WXmBulletinBoard, xmFormWidgetClass)
	DEFINE_GETTER_SETTER (HorizontalSpacing,int,"horizontalSpacing")
	DEFINE_GETTER_SETTER (VerticalSpacing,int,"verticalSpacing")
	DEFINE_GETTER_SETTER (FractionBase,int,"fractionBase")
	DEFINE_GETTER_SETTER (RubberPositioning,Boolean,"rubberPositioning")

// Constraint resources valid within paned window

   DEFINE_GETTER_SETTER (AllowResize, Boolean, "allowResize")
   DEFINE_GETTER_SETTER (PaneMaximum, Dimension, "paneMaximum")
   DEFINE_GETTER_SETTER (PaneMinimum, Dimension, "paneMinimum")
   DEFINE_GETTER_SETTER (SkipAdjust,  Boolean, "skipAdjust")
};

#define	NULLWXmForm		WXmForm((Widget)0)

class CXmForm : public CXmBulletinBoard {
public :
		DEFINE_CINIT(CXmForm, CXmBulletinBoard, xmFormWidgetClass)
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
};

#endif
