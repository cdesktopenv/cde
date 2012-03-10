/*
 * $XConsortium: WXmCommand.h /main/3 1996/06/11 16:59:38 cde-hal $
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
#ifndef WXmCommand_h
#define WXmCommand_h

#include "WXmSelectionBox.h"
#include <Xm/Command.h>

class WXmCommand : public WXmSelectionBox {
public :
		DEFINE_INIT (WXmCommand, WXmSelectionBox, xmCommandWidgetClass)
inline	Widget	GetChild (unsigned char child) const	{ return XmCommandGetChild (widget, child); }
inline	void	SetValue (WXmString str) const	{ XmCommandSetValue (widget, str); }
inline	void	AppendValue (WXmString str) const	{ XmCommandAppendValue (widget, str); }
inline	void	Error (WXmString str) const	{ XmCommandError (widget, str); }
	DEFINE_CALLBACK (CommandEnteredCallback,"commandEnteredCallback")
	DEFINE_CALLBACK (CommandChangedCallback,"commandChangedCallback")
	DEFINE_STRING_GETSET (PromptString,"promptString")
	DEFINE_STRING_GETSET (Command,"command")
	DEFINE_GETTER_SETTER (HistoryItems,XmStringTable,"historyItems")
	DEFINE_GETTER_SETTER (HistoryItemCount,int,"historyItemCount")
	DEFINE_GETTER_SETTER (HistoryMaxItems,int,"historyMaxItems")
	DEFINE_GETTER_SETTER (HistoryVisibleItemCount,int,"historyVisibleItemCount")
	DEFINE_GETTER_SETTER (DialogType,unsigned char,"dialogType")
	DEFINE_GETTER_SETTER (DefaultPosition,Boolean,"defaultPosition")
	DEFINE_GETTER_SETTER (AutoUnmanage,Boolean,"autoUnmanage")
	DEFINE_GETTER_SETTER (ResizePolicy,unsigned char,"resizePolicy")
};

#define	NULLWXmCommand		WXmCommand((Widget)0)

class CXmCommand : public CXmSelectionBox {
public :
		DEFINE_CINIT(CXmCommand, CXmSelectionBox, xmCommandWidgetClass)
};

#endif
