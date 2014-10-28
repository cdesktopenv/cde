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
 * $XConsortium: WXmTextField.h /main/3 1996/06/11 17:02:46 cde-hal $
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

// This code is automatically generated in -*- C++ -*-
#ifndef WXmTextField_h
#define WXmTextField_h

#include "WXmPrimitive.h"
#include <Xm/TextF.h>

class WXmTextField : public WXmPrimitive {
public :
		DEFINE_INIT (WXmTextField,WXmPrimitive,xmTextFieldWidgetClass)
inline	void	ClearSelection(Time t) const { XmTextFieldClearSelection (widget, t); } 
inline	Boolean	Copy (Time copy_time) const { return XmTextFieldCopy (widget, copy_time); }
inline	Boolean	Cut (Time cut_time) const { return XmTextFieldCut (widget, cut_time); }
inline	int	GetBaseline () const { return XmTextFieldGetBaseline (widget); }
inline	Boolean	GetEditable () const { return XmTextFieldGetEditable (widget); }
inline	XmTextPosition	GetInsertionPosition () const { return XmTextFieldGetInsertionPosition (widget); }
inline	XmTextPosition	GetLastPosition () const { return XmTextFieldGetLastPosition (widget); }
inline	XmTextPosition	LastPosition () const { return XmTextFieldGetLastPosition (widget); }
inline	int	GetMaxLength () const { return XmTextFieldGetMaxLength (widget); }
inline	char*	GetSelection () const { return XmTextFieldGetSelection (widget); }
inline	Boolean	GetSelectionPosition (XmTextPosition* left, XmTextPosition* right) const { return XmTextFieldGetSelectionPosition (widget, left, right); }
inline	char*	GetString () const { return XmTextFieldGetString (widget); }
inline	void	Insert (XmTextPosition position, char* value) const { XmTextFieldInsert (widget, position, value); }
inline	Boolean	Paste (Widget twidget) const { return XmTextFieldPaste (twidget); }
inline	Boolean	 PosToXY (XmTextPosition position, Position* x, Position* y) const { return XmTextFieldPosToXY (widget, position, x, y); }
inline	Boolean	Remove () const { return XmTextFieldRemove (widget); }
inline	void	Replace (XmTextPosition from, XmTextPosition to, char* s) const { XmTextFieldReplace (widget, from, to, s); }
inline	void	SetAddMode (Boolean state) const { XmTextFieldSetAddMode (widget, state); }
inline	void	SetEditable (Boolean editable) const { XmTextFieldSetEditable (widget, editable); }
inline	void	SetHighlight (XmTextPosition left, XmTextPosition right, XmHighlightMode mode) const { XmTextFieldSetHighlight (widget, left, right, mode); }
inline	void	SetInsertionPosition (XmTextPosition position) const { XmTextFieldSetInsertionPosition (widget, position); }
inline	void	InsertionPosition (XmTextPosition position) const { XmTextFieldSetInsertionPosition (widget, position); }
inline	void	SetMaxLength (int max_length) const { XmTextFieldSetMaxLength (widget, max_length); }
inline	void	SetSelection (XmTextPosition first, XmTextPosition last, Time set_time) const { XmTextFieldSetSelection (widget, first, last, set_time); }
inline	void	SetString(char* str) const { XmTextFieldSetString (widget, str); }
inline	void	ShowPosition (XmTextPosition position) const { XmTextFieldShowPosition (widget, position); }
inline	XmTextPosition	XYToPos (Position x, Position y) const { return XmTextFieldXYToPos (widget, x, y); }
	DEFINE_CALLBACK (ActivateCallback,"activateCallback")
	DEFINE_CALLBACK (FocusCallback,"focusCallback")
	DEFINE_CALLBACK (LosingFocusCallback,"losingFocusCallback")
	DEFINE_CALLBACK (ValueChangedCallback,"valueChangedCallback")
	DEFINE_CALLBACK (ModifyVerifyCallback,"modifyVerifyCallback")
	DEFINE_CALLBACK (MotionVerifyCallback,"motionVerifyCallback")
        DEFINE_CALLBACK (GainPrimaryCallback,"gainPrimaryCallback")
	DEFINE_CALLBACK (LosePrimaryCallback,"losePrimaryCallback")
	DEFINE_GETTER_SETTER (BlinkRate,int,"blinkRate")
	DEFINE_GETTER_SETTER (Columns,short,"columns")
	DEFINE_GETTER_SETTER (CursorPosition,XmTextPosition,"cursorPosition")
	DEFINE_GETTER_SETTER (CursorPositionVisible,Boolean,
			      "cursorPositionVisible")
	DEFINE_GETTER_SETTER (Editable,Boolean,"editable")
	DEFINE_GETTER_SETTER (FontList,XmFontList,"fontList")
	DEFINE_GETTER_SETTER (MarginHeight,short,"marginHeight")
	DEFINE_GETTER_SETTER (MarginWidth,short,"marginWidth")
	DEFINE_GETTER_SETTER (MaxLength,int,"maxLength")
	DEFINE_GETTER_SETTER (PendingDelete,Boolean,"pendingDelete")
        DEFINE_GETTER_SETTER (ResizeWidth,Boolean,"resizeWidth")
	DEFINE_GETTER_SETTER (SelectionArray,XtPointer,"selectionArray")
	DEFINE_GETTER_SETTER (SelectionArrayCount,int,"selectionArrayCount")
	DEFINE_GETTER_SETTER (SelectThreshold,int,"selectThreshold")
	DEFINE_GETTER_SETTER (Value,char*,"value")
	DEFINE_GETTER_SETTER (VerifyBell,Boolean,"verifyBell")
};

#define	NULLWXmTextField		WXmTextField((Widget)0)

#endif
