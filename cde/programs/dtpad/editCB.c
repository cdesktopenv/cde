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
/* $XConsortium: editCB.c /main/3 1995/11/01 10:35:26 rswiston $ */
/**********************************<+>*************************************
***************************************************************************
**
**  File:        editCB.c
**
**  Project:     DT dtpad, a memo maker type editor based on the Dt Editor
**               widget.
**
**  Description:
**  -----------
**
**	This file contains the callbacks for the [Edit] menu items.
**
*******************************************************************
**  (c) Copyright Hewlett-Packard Company, 1991, 1992.  All rights are
**  reserved.  Copying or other reproduction of this program
**  except for archival purposes is prohibited without prior
**  written consent of Hewlett-Packard Company.
********************************************************************
**
********************************************************************
**  (c) Copyright 1993, 1994 Hewlett-Packard Company
**  (c) Copyright 1993, 1994 International Business Machines Corp.
**  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
**  (c) Copyright 1993, 1994 Novell, Inc.
********************************************************************
**
**
**************************************************************************
**********************************<+>*************************************/
#include "dtpad.h"
#include <Dt/HourGlass.h>


/************************************************************************
 * EditUndoCB - [Edit] menu, [Undo] button.
 *	Undoes the last edit.
 ************************************************************************/
/* ARGSUSED */
void
EditUndoCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    Editor *pPad = (Editor *)client_data;
    DtEditorUndoEdit(pPad->editor);
}


/************************************************************************
 * EditCutCB - [Edit] menu, [Cut] button.
 *	Cuts the current selection to the Motif clipboard.
 ************************************************************************/
/* ARGSUSED */
void
EditCutCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    Editor *pPad = (Editor *)client_data;
    DtEditorCutToClipboard(pPad->editor);
}


/************************************************************************
 * EditCopyCB - [Edit] menu, [Copy] button.
 *	Copies the current selection to the Motif clipboard.
 ************************************************************************/
/* ARGSUSED */
void
EditCopyCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    Editor *pPad = (Editor *)client_data;
    DtEditorCopyToClipboard(pPad->editor);
}


/************************************************************************
 * EditPasteCB - [Edit] menu, [Paste] button.
 *	Pastes from the Motif clipboard.
 ************************************************************************/
/* ARGSUSED */
void
EditPasteCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    Editor *pPad = (Editor *)client_data;
    DtEditorPasteFromClipboard(pPad->editor);

    /* XXX - Do DtEditorGetInsertPosition & DtEditorSetInsertionPosition
     *       need to be executed here??
     * XmTextPosition cursorPos;
     * cursorPos = XmTextGetInsertionPosition(pPad->text);
     * XmTextShowPosition(pPad->text, cursorPos);
     */
}


/************************************************************************
 * EditClearCB - [Edit] menu, [Replace] button
 *	Replaces the current selection with blanks.
 ************************************************************************/
/* ARGSUSED */
void
EditClearCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    Editor *pPad = (Editor *)client_data;
    DtEditorClearSelection(pPad->editor);
}


/************************************************************************
 * EditDeleteCB - [Edit] menu, [Delete] button.
 *	Deletes the current selection.
 ************************************************************************/
/* ARGSUSED */
void
EditDeleteCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    Editor *pPad = (Editor *)client_data;
    DtEditorDeleteSelection(pPad->editor);
}


/************************************************************************
 * SelectAllCB - [Edit] menu, [Select All] button.
 *	Selects all text.
 ************************************************************************/
/* ARGSUSED */
void
SelectAllCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    Editor *pPad = (Editor *)client_data;
    DtEditorSelectAll(pPad->editor);
}


/************************************************************************
 * FindChangeCB - [Edit] menu, [Find/Change...] button.
 *	Invokes the Dt Editor widget search dialog.
 ************************************************************************/
/* ARGSUSED */
void
FindChangeCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    Editor *pPad = (Editor *)client_data;
    DtEditorInvokeFindChangeDialog(pPad->editor);
}


/************************************************************************
 * CheckSpellingCB - [Edit] menu, [Check Spelling...] button.
 *	Invokes the Dt Editor widget spell dialog.
 ************************************************************************/
/* ARGSUSED */
void
CheckSpellingCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    Editor *pPad = (Editor *)client_data;
    DtEditorInvokeSpellDialog(pPad->editor);
}


/************************************************************************
 * SetSelectionMenuItems - Sets the sensitivity of [Edit] menu items
 *	that deal with the current selection in the edit window - allowing
 *	for viewOnly mode.
 ************************************************************************/
/* ARGSUSED */
void
SetSelectionMenuItems(
        Editor *pPad,
	Boolean sensitivity)
{
    XtSetSensitive(pPad->editStuff.widgets.cutBtn,
      sensitivity && ! pPad->xrdb.viewOnly);
    XtSetSensitive(pPad->editStuff.widgets.copyBtn,
      sensitivity);			/* Copy can be done in viewOnly mode */
    XtSetSensitive(pPad->editStuff.widgets.clearBtn,
      sensitivity && ! pPad->xrdb.viewOnly);
    XtSetSensitive(pPad->editStuff.widgets.deleteBtn,
      sensitivity && ! pPad->xrdb.viewOnly);
}


/************************************************************************
 * TextSelectedCB - DtEditor widget DtNtextSelectCallback called when
 *	text in the editor window is selected.
 *	Makes [Edit] menu items related to a selection sensitive.
 ************************************************************************/
/* ARGSUSED */
void
TextSelectedCB(
	Widget w,
        caddr_t client_data,
        caddr_t call_data)
{
    Editor *pPad = (Editor *)client_data;
    SetSelectionMenuItems(pPad, True);
}


/************************************************************************
 * TextDeselectedCB - DtEditor widget DtNtextSelectCallback called when
 *	text in the editor window is deselected.
 *	Makes [Edit] menu items related to a selection insensitive.
 ************************************************************************/
/* ARGSUSED */
void
TextDeselectedCB(
	Widget w,
        caddr_t client_data,
        caddr_t call_data)
{
    Editor *pPad = (Editor *)client_data;
    SetSelectionMenuItems(pPad, False);
}
