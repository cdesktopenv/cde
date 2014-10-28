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
 * $XConsortium: WXmText.h /main/3 1996/06/11 17:02:41 cde-hal $
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
#ifndef WXmText_h
#define WXmText_h

#include "WXmPrimitive.h"
#include <Xm/Text.h>

class WXmText : public WXmPrimitive {
public :
		DEFINE_INIT (WXmText, WXmPrimitive, xmTextWidgetClass)
inline	void	ClearSelection(Time t) const { XmTextClearSelection (widget, t); } 
inline	Boolean	Copy (Time copy_time) const { return XmTextCopy (widget, copy_time); }
inline	Boolean	Cut (Time cut_time) const { return XmTextCut (widget, cut_time); }
inline	Boolean	GetAddMode () const { return XmTextGetAddMode (widget); }
inline	int	GetBaseline () const { return XmTextGetBaseline (widget); }
// Non-published routine.  Use GetInsertionPosition. DJB
// inline	XmTextPosition	GetCursorPosition () const { return XmTextGetCursorPosition (widget); }
inline	Boolean	GetEditable () const { return XmTextGetEditable (widget); }
inline	XmTextPosition	GetInsertionPosition () const { return XmTextGetInsertionPosition (widget); }
inline	XmTextPosition	GetLastPosition () const { return XmTextGetLastPosition (widget); }
inline	XmTextPosition	LastPosition () const { return XmTextGetLastPosition (widget); }
inline	int	GetMaxLength () const { return XmTextGetMaxLength (widget); }
inline	char*	GetSelection () const { return XmTextGetSelection (widget); }
inline	Boolean	GetSelectionPosition (XmTextPosition* left, XmTextPosition* right) const { return XmTextGetSelectionPosition (widget, left, right); }
inline	XmTextSource	GetSource () const { return XmTextGetSource (widget); }
inline	char*	GetString () const { return XmTextGetString (widget); }
inline	XmTextPosition	GetTopCharacter () const { return XmTextGetTopCharacter (widget); }
inline	void	Insert (XmTextPosition position, char* value) const { XmTextInsert (widget, position, value); }
inline	Boolean	Paste (Widget twidget) const { return XmTextPaste (twidget); }
inline	Boolean	 PosToXY (XmTextPosition position, Position* x, Position* y) const { return XmTextPosToXY (widget, position, x, y); }
inline	Boolean	Remove () const { return XmTextRemove (widget); }
inline	void	Replace (XmTextPosition from, XmTextPosition to, char* s) const { XmTextReplace (widget, from, to, s); }
inline	void	Scroll (int n) const { XmTextScroll (widget, n); }
inline	void	SetAddMode (Boolean state) const { XmTextSetAddMode (widget, state); }
// Non-published routine, use SetInsertionPosition - DJB
// inline	void	SetCursorPosition (XmTextPosition position) const { XmTextSetCursorPosition (widget, position); }
inline	void	SetEditable (Boolean editable) const { XmTextSetEditable (widget, editable); }
inline	void	SetHighlight (XmTextPosition left, XmTextPosition right, XmHighlightMode mode) const { XmTextSetHighlight (widget, left, right, mode); }
inline	void	SetInsertionPosition (XmTextPosition position) const { XmTextSetInsertionPosition (widget, position); }
inline	void	InsertionPosition (XmTextPosition position) const { XmTextSetInsertionPosition (widget, position); }
inline	void	SetMaxLength (int max_length) const { XmTextSetMaxLength (widget, max_length); }
inline	void	SetSelection (XmTextPosition first, XmTextPosition last, Time set_time) const { XmTextSetSelection (widget, first, last, set_time); }
inline	void	SetSource (XmTextSource source, XmTextPosition top_character, XmTextPosition cursor_position) const { XmTextSetSource (widget, source, top_character, cursor_position); }
inline	void	SetString(char* str) const { XmTextSetString (widget, str); }
inline	void	SetTopCharacter (XmTextPosition top_character) const { XmTextSetTopCharacter (widget, top_character); }
inline	void	ShowPosition (XmTextPosition position) const { XmTextShowPosition (widget, position); }
inline	XmTextPosition	XYToPos (Position x, Position y) const { return XmTextXYToPos (widget, x, y); }
	DEFINE_CALLBACK (ActivateCallback,"activateCallback")
	DEFINE_GETTER_SETTER (AutoShowCursorPosition,Boolean,"autoShowCursorPosition")
	DEFINE_GETTER_SETTER (CursorPosition,XmTextPosition,"cursorPosition")
	DEFINE_GETTER_SETTER (Editable,Boolean,"editable")
	DEFINE_GETTER_SETTER (EditMode,unsigned char,"editMode")
	DEFINE_CALLBACK (FocusCallback,"focusCallback")
	DEFINE_CALLBACK (LosingFocusCallback,"losingFocusCallback")
	DEFINE_CALLBACK (GainPrimaryCallback,"gainPrimaryCallback")
	DEFINE_CALLBACK (LosePrimaryCallback,"losePrimaryCallback")
	DEFINE_GETTER_SETTER (MarginHeight,short,"marginHeight")
	DEFINE_GETTER_SETTER (MarginWidth,short,"marginWidth")
	DEFINE_GETTER_SETTER (MaxLength,int,"maxLength")
	DEFINE_CALLBACK (ModifyVerifyCallback,"modifyVerifyCallback")
	DEFINE_CALLBACK (MotionVerifyCallback,"motionVerifyCallback")
	DEFINE_GETTER_SETTER (Source,XmTextSource,"source")
	DEFINE_GETTER_SETTER (TopCharacter,XmTextPosition,"topCharacter")
	DEFINE_GETTER_SETTER (Value,char *,"value")
	DEFINE_CALLBACK (ValueChangedCallback,"valueChangedCallback")
	DEFINE_GETTER_SETTER (VerifyBell,Boolean,"verifyBell")
// Input resources
	DEFINE_GETTER_SETTER (PendingDelete,Boolean,"pendingDelete")
	DEFINE_GETTER_SETTER (SelectionArray,XtPointer,"selectionArray")
	DEFINE_GETTER_SETTER (SelectionArrayCount,int,"selectionArrayCount")
	DEFINE_GETTER_SETTER (SelectThreshold,int,"selectThreshold")
// Output resources
	DEFINE_GETTER_SETTER (BlinkRate,int,"blinkRate")
	DEFINE_GETTER_SETTER (Columns,short,"columns")
	DEFINE_GETTER_SETTER (CursorPositionVisible,Boolean,"cursorPositionVisible")
	DEFINE_GETTER_SETTER (FontList,XmFontList,"fontList")
	DEFINE_GETTER_SETTER (ResizeHeight,Boolean,"resizeHeight")
	DEFINE_GETTER_SETTER (ResizeWidth,Boolean,"resizeWidth")
	DEFINE_GETTER_SETTER (Rows,short,"rows")
	DEFINE_GETTER_SETTER (WordWrap,Boolean,"wordWrap")
// Scrolled text resources
	DEFINE_GETTER_SETTER (ScrollHorizontal,Boolean,"scrollHorizontal")
	DEFINE_GETTER_SETTER (ScrollLeftSide,Boolean,"scrollLeftSide")
	DEFINE_GETTER_SETTER (ScrollTopSide,Boolean,"scrollTopSide")
	DEFINE_GETTER_SETTER (ScrollVertical,Boolean,"scrollVertical")
// Unknown resources
	DEFINE_GETTER_SETTER (OutputCreate,XtProc,"outputCreate")
	DEFINE_GETTER_SETTER (InputCreate,XtProc,"inputCreate")
	DEFINE_GETTER_SETTER (TopPosition,int,"topPosition")
};


class WXmScrolledText : public WXmText {
public:
  WXmScrolledText() {}
  WXmScrolledText (WComposite& father, char* name, int /* automanage */,
		 ArgList args = NULL, Cardinal card = 0)
    : WXmText (XmCreateScrolledText(Widget(father), name, args, card))
    { Manage(); }

  WXmScrolledText (WComposite& father, char* name, ArgList args,
		 Cardinal card)
    : WXmText (XmCreateScrolledText(Widget(father), name, args, card))
    {}
};


#define	NULLWXmText		WXmText((Widget)0)
#define NULLWXmScrolledText     WXmScrolledText((Widget)0)

#endif
