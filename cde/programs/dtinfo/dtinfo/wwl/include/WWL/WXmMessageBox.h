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
 * $XConsortium: WXmMessageBox.h /main/3 1996/06/11 17:01:16 cde-hal $
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
#ifndef WXmMessageBox_h
#define WXmMessageBox_h

#include "WXmBulletinBoard.h"
#include "WXmPushButton.h" /* DWC IBM */
#include <Xm/MessageB.h>
#include <Xm/DialogS.h>

class WXmPushButton;

class WXmMessageBox : public WXmBulletinBoard {
public :
   DEFINE_INIT (WXmMessageBox, WXmBulletinBoard, xmMessageBoxWidgetClass)
   DEFINE_CALLBACK      (CancelCallback,"cancelCallback")
   DEFINE_STRING_GETSET (CancelLabelString,"cancelLabelString")
   DEFINE_GETTER_SETTER (DefaultButtonType,unsigned char,"defaultButtonType")
   DEFINE_GETTER_SETTER (DialogType,unsigned char,"dialogType")
   DEFINE_STRING_GETSET (HelpLabelString,"helpLabelString")
   DEFINE_GETTER_SETTER (MessageAlignment,unsigned char,"messageAlignment")
   DEFINE_STRING_GETSET (MessageString,"messageString")
   DEFINE_GETTER_SETTER (MinimizeButtons,Boolean,"minimizeButtons")
   DEFINE_CALLBACK      (OkCallback,"okCallback")
   DEFINE_STRING_GETSET (OkLabelString,"okLabelString")
   DEFINE_GETTER_SETTER (SymbolPixmap,Pixmap,"symbolPixmap")
   inline	Widget	GetChild (unsigned char child) const {
      return XmMessageBoxGetChild (widget, child);
   }
};

#define	NULLWXmMessageBox		WXmMessageBox((Widget)0)

class CXmMessageBox : public CXmBulletinBoard {
public :
   DEFINE_CINIT(CXmMessageBox, CXmBulletinBoard, xmMessageBoxWidgetClass)
};

//-----------------------------------------------------------------------------
// WXmMessageDialog kind of takes the place of the XmCreateMessageDialog
// convenience function.  This creates a dialog shell and a message box,
// returning the Id of the message box.
//
class WXmMessageDialog : public WXmMessageBox
{
 public:
   inline WXmMessageDialog(const WComposite& father,
		    char*             name,
		    ArgList           args = NULL,
		    Cardinal          card = 0 )
      : WXmMessageBox(XmCreateDialogShell(Widget(father),name,NULL,0),
		      name, args, card ){};
   inline WXmMessageDialog(const WComposite& father,
		    char*             name,
		    WArgList          args)
      : WXmMessageBox(XmCreateDialogShell(Widget(father),name,NULL,0),
		      name, args){};

   inline WXmPushButton OkPB() { 
      return WXmPushButton(GetChild(XmDIALOG_OK_BUTTON));}

   inline WXmPushButton CancelPB() { 
      return WXmPushButton(GetChild(XmDIALOG_CANCEL_BUTTON));}

   inline WXmPushButton HelpPB() { 
      return WXmPushButton(GetChild(XmDIALOG_HELP_BUTTON));}

   inline WXmPushButton Separator() { 
      return WXmPushButton(GetChild(XmDIALOG_SEPARATOR));}

};


#endif
