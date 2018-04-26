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
 * $XConsortium: WXmBulletinBoard.h /main/3 1996/06/11 16:59:22 cde-hal $
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
#ifndef WXmBulletinBoard_h
#define WXmBulletinBoard_h

#include "WXmManager.h"
#include <Xm/BulletinB.h>

class WXmBulletinBoard : public WXmManager {
public :
	DEFINE_INIT (WXmBulletinBoard, WXmManager, xmBulletinBoardWidgetClass)
	DEFINE_GETTER_SETTER (ShadowType,unsigned char,"shadowType")
	DEFINE_GETTER_SETTER (ShadowThickness,short,"shadowThickness")
	DEFINE_GETTER_SETTER (MarginWidth,short,"marginWidth")
	DEFINE_GETTER_SETTER (MarginHeight,short,"marginHeight")
	DEFINE_GETTER_SETTER (DefaultButton,Widget,"defaultButton")
	DEFINE_GETTER_SETTER (CancelButton,Widget,"cancelButton")
	DEFINE_CALLBACK (FocusCallback,"focusCallback")
	DEFINE_CALLBACK (MapCallback,"mapCallback")
	DEFINE_CALLBACK (UnmapCallback,"unmapCallback")
	DEFINE_GETTER_SETTER (StringDirection,unsigned char,"stringDirection")
	DEFINE_GETTER_SETTER (ButtonFontList,XmFontList,"buttonFontList")
	DEFINE_GETTER_SETTER (LabelFontList,XmFontList,"labelFontList")
	DEFINE_GETTER_SETTER (TextFontList,XmFontList,"textFontList")
	DEFINE_GETTER_SETTER (TextTranslations,XtTranslations,"textTranslations")
	DEFINE_GETTER_SETTER (AllowOverlap,Boolean,"allowOverlap")
	DEFINE_GETTER_SETTER (AutoUnmanage,Boolean,"autoUnmanage")
	DEFINE_GETTER_SETTER (DefaultPosition,Boolean,"defaultPosition")
	DEFINE_GETTER_SETTER (ResizePolicy,unsigned char,"resizePolicy")
	DEFINE_GETTER_SETTER (NoResize,Boolean,"noResize")
	DEFINE_GETTER_SETTER (DialogStyle,unsigned char,"dialogStyle")
	DEFINE_STRING_GETSET (DialogTitle,"dialogTitle")
};

#define	NULLWXmBulletinBoard		WXmBulletinBoard((Widget)0)

class CXmBulletinBoard : public CXmManager {
public :
   DEFINE_CINIT (CXmBulletinBoard, CXmManager, xmBulletinBoardWidgetClass)
};

#endif
