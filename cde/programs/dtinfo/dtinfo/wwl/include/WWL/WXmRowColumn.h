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
 * $XConsortium: WXmRowColumn.h /main/3 1996/06/11 17:01:51 cde-hal $
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
#ifndef WXmRowColumn_h
#define WXmRowColumn_h

#include "WXmManager.h"
#include <Xm/RowColumn.h>

class WXmRowColumn : public WXmManager {
public :
		DEFINE_INIT (WXmRowColumn, WXmManager, xmRowColumnWidgetClass)
	DEFINE_GETTER_SETTER (ResizeWidth,Boolean,"resizeWidth")
	DEFINE_GETTER_SETTER (ResizeHeight,Boolean,"resizeHeight")
	DEFINE_GETTER_SETTER (WhichButton,unsigned int,"whichButton")
	DEFINE_GETTER_SETTER (AdjustLast,Boolean,"adjustLast")
	DEFINE_GETTER_SETTER (MarginWidth,Dimension,"marginWidth")
	DEFINE_GETTER_SETTER (MarginHeight,Dimension,"marginHeight")
	DEFINE_CALLBACK (EntryCallback,"entryCallback")
	DEFINE_CALLBACK (MapCallback,"mapCallback")
	DEFINE_CALLBACK (UnmapCallback,"unmapCallback")
	DEFINE_GETTER_SETTER (Orientation,unsigned char,"orientation")
	DEFINE_GETTER_SETTER (Spacing,Dimension,"spacing")
	DEFINE_GETTER_SETTER (EntryBorder,Dimension,"entryBorder")
	DEFINE_GETTER_SETTER (IsAligned,Boolean,"isAligned")
	DEFINE_GETTER_SETTER (EntryAlignment,unsigned char,"entryAlignment")
	DEFINE_GETTER_SETTER (AdjustMargin,Boolean,"adjustMargin")
	DEFINE_GETTER_SETTER (Packing,unsigned char,"packing")
	DEFINE_GETTER_SETTER (NumColumns,short,"numColumns")
	DEFINE_GETTER_SETTER (RadioBehavior,Boolean,"radioBehavior")
	DEFINE_GETTER_SETTER (RadioAlwaysOne,Boolean,"radioAlwaysOne")
	DEFINE_GETTER_SETTER (IsHomogeneous,Boolean,"isHomogeneous")
	DEFINE_GETTER_SETTER (EntryClass,int,"entryClass")
	DEFINE_GETTER_SETTER (RowColumnType,unsigned char,"rowColumnType")
	DEFINE_GETTER_SETTER (MenuHelpWidget,Widget,"menuHelpWidget")
	DEFINE_STRING_GETSET (LabelString,"labelString")
	DEFINE_GETTER_SETTER (SubMenuId,Widget,"subMenuId")
	DEFINE_GETTER_SETTER (MenuHistory,Widget,"menuHistory")
	DEFINE_GETTER_SETTER (PopupEnabled,Boolean,"popupEnabled")
	DEFINE_GETTER_SETTER (MenuAccelerator,char*,"menuAccelerator")
	DEFINE_GETTER_SETTER (Mnemonic,char,"mnemonic")

// Constraint resources valid within paned window

   DEFINE_GETTER_SETTER (AllowResize, Boolean, "allowResize")
   DEFINE_GETTER_SETTER (PaneMaximum, Dimension, "paneMaximum")
   DEFINE_GETTER_SETTER (PaneMinimum, Dimension, "paneMinimum")
   DEFINE_GETTER_SETTER (SkipAdjust,  Boolean, "skipAdjust")
};

#define	NULLWXmRowColumn		WXmRowColumn((Widget)0)

class CXmRowColumn : public CXmManager {
public :
		DEFINE_CINIT(CXmRowColumn, CXmManager, xmRowColumnWidgetClass)
};

#endif
