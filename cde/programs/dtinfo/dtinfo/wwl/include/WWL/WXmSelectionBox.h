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
 * $XConsortium: WXmSelectionBox.h /main/3 1996/06/11 17:02:15 cde-hal $
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
#ifndef WXmSelectionBox_h
#define WXmSelectionBox_h

#include "WXmBulletinBoard.h"
#include <Xm/SelectioB.h>

class WXmSelectionBox : public WXmBulletinBoard {
public :
   DEFINE_INIT (WXmSelectionBox, WXmBulletinBoard, xmSelectionBoxWidgetClass)
	DEFINE_GETTER_SETTER (TextAccelerators,XtTranslations,"textAccelerators")
	DEFINE_STRING_GETSET (SelectionLabelString,"selectionLabelString")
	DEFINE_STRING_GETSET (ListLabelString,"listLabelString")
	DEFINE_GETTER_SETTER (TextColumns,short,"textColumns")
	DEFINE_STRING_GETSET (TextString,"textString")
	DEFINE_GETTER_SETTER (ListItems,XmStringTable,"listItems")
	DEFINE_GETTER_SETTER (ListItemCount,int,"listItemCount")
	DEFINE_GETTER_SETTER (ListVisibleItemCount,int,"listVisibleItemCount")
	DEFINE_STRING_GETSET (OkLabelString,"okLabelString")
	DEFINE_STRING_GETSET (ApplyLabelString,"applyLabelString")
	DEFINE_STRING_GETSET (CancelLabelString,"cancelLabelString")
	DEFINE_STRING_GETSET (HelpLabelString,"helpLabelString")
	DEFINE_CALLBACK (NoMatchCallback,"noMatchCallback")
	DEFINE_GETTER_SETTER (MustMatch,Boolean,"mustMatch")
	DEFINE_GETTER_SETTER (MinimizeButtons,Boolean,"minimizeButtons")
	DEFINE_CALLBACK (OkCallback,"okCallback")
	DEFINE_CALLBACK (ApplyCallback,"applyCallback")
	DEFINE_CALLBACK (CancelCallback,"cancelCallback")
	DEFINE_GETTER_SETTER (DialogType,unsigned char,"dialogType")

	Widget GetChild (unsigned char child)
	  { return (XmSelectionBoxGetChild (widget, child)); }
};

#define	NULLWXmSelectionBox		WXmSelectionBox((Widget)0)

class CXmSelectionBox : public CXmBulletinBoard {
public :
   DEFINE_CINIT(CXmSelectionBox, CXmBulletinBoard, xmSelectionBoxWidgetClass)
};

#endif
