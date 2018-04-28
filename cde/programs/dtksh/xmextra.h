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
/* $XConsortium: xmextra.h /main/6 1996/04/01 17:54:30 rswiston $ */

/*	Copyright (c) 1991, 1992 UNIX System Laboratories, Inc. */
/*	All Rights Reserved     */

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF          */
/*	UNIX System Laboratories, Inc.			        */
/*	The copyright notice above does not evidence any        */
/*	actual or intended publication of such source code.     */

#ifndef _Dtksh_xmextra_h
#define _Dtksh_xmextra_h

#include "xmcmds.h"


/*
 * CDExc17788
 *
 * The following corrects for a type-mismatch throughout the following
 * functions with the element in the table into which they are to be
 * inserted.
 */

#if defined(bltin) && ( defined(__STDC__) || defined(__STDPP__) )
#define lcl_cast(x)	((int (*)__PROTO__((int, char*[], __V_*)))x)
#else
#define lcl_cast(x)	x
#endif


#define DTK_TK_EXTRA_TABLE \
  { "XmCreateArrowButton", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateArrowButton) }, \
  { "XmCreateArrowButtonGadget",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateArrowButtonGadget) },  \
  { "XmCreateBulletinBoard",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateBulletinBoard) },  \
  { "XmCreateBulletinBoardDialog",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateBulletinBoardDialog) },  \
  { "XmCreateCascadeButton",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateCascadeButton) },  \
  { "XmCreateCascadeButtonGadget",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateCascadeButtonGadget) },  \
  { "XmCreateComboBox",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateComboBox) },  \
  { "XmCreateCommand",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateCommand) },  \
  { "XmCreateDialogShell",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateDialogShell) },  \
  { "XmCreateDrawingArea",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateDrawingArea) },  \
  { "XmCreateDrawnButton",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateDrawnButton) },  \
  { "XmCreateErrorDialog",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateErrorDialog) },  \
  { "XmCreateFileSelectionBox",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateFileSelectionBox) },  \
  { "XmCreateFileSelectionDialog",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateFileSelectionDialog) },  \
  { "XmCreateForm",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateForm) },  \
  { "XmCreateFormDialog",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateFormDialog) },  \
  { "XmCreateFrame",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateFrame) },  \
  { "XmCreateInformationDialog",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateInformationDialog) },  \
  { "XmCreateLabel",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateLabel) },  \
  { "XmCreateLabelGadget",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateLabelGadget) },  \
  { "XmCreateList",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateList) },  \
  { "XmCreateMainWindow",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateMainWindow) },  \
  { "XmCreateMenuBar",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateMenuBar) },  \
  { "XmCreateMenuShell",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateMenuShell) },  \
  { "XmCreateMessageBox",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateMessageBox) },  \
  { "XmCreateMessageDialog",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateMessageDialog) },  \
  { "XmCreateOptionMenu",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateOptionMenu) },  \
  { "XmCreatePanedWindow",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreatePanedWindow) },  \
  { "XmCreatePopupMenu",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreatePopupMenu) },  \
  { "XmCreatePromptDialog",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreatePromptDialog) },\
  { "XmCreatePulldownMenu",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreatePulldownMenu) },\
  { "XmCreatePushButton",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreatePushButton) },  \
  { "XmCreatePushButtonGadget",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreatePushButtonGadget) },  \
  { "XmCreateQuestionDialog",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateQuestionDialog) },  \
  { "XmCreateRadioBox",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateRadioBox) },  \
  { "XmCreateRowColumn",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateRowColumn) },  \
  { "XmCreateScale",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateScale) },  \
  { "XmCreateScrollBar",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateScrollBar) },  \
  { "XmCreateScrolledList",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateScrolledList) },\
  { "XmCreateScrolledText",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateScrolledText) },\
  { "XmCreateScrolledWindow",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateScrolledWindow) },  \
  { "XmCreateSelectionBox",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateSelectionBox) },\
  { "XmCreateSelectionDialog",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateSelectionDialog) },  \
  { "XmCreateSeparator",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateSeparator) },  \
  { "XmCreateSeparatorGadget",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateSeparatorGadget) },  \
  { "XmCreateText",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateText) },  \
  { "XmCreateTextField",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateTextField) },\
  { "XmCreateToggleButton",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateToggleButton) },\
  { "XmCreateToggleButtonGadget",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateToggleButtonGadget) },  \
  { "XmCreateWarningDialog",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateWarningDialog) },  \
  { "XmCreateWorkArea",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateWorkArea) },  \
  { "XmCreateWorkingDialog",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCreateWorkingDialog) }, \
  { "DtCreateHelpDialog",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_DtCreateHelpDialog) }, \
  { "DtCreateHelpQuickDialog",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_DtCreateHelpQuickDialog) }, \
  { "DtCreatePrintSetupBox",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_DtCreatePrintSetupBox)}, \
  { "DtCreatePrintSetupDialog",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_DtCreatePrintSetupDialog)}, \
  { "DtCreatePDMJobSetup",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_DtCreatePDMJobSetup)}, \
  { "DtHelpReturnSelectedWidgetId",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_DtHelpReturnSelectedWidgetId) }, \
  { "DtHelpSetCatalogName",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_DtHelpSetCatalogName) }, \
  { "DtHelpQuickDialogGetChild",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_DtHelpQuickDialogGetChild) }, \
  { "XmCommandAppendValue",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCommandAppendValue) },\
  { "XmCommandError",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCommandError) },\
  { "XmCommandSetValue",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCommandSetValue) },\
  { "XmCommandGetChild",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmCommandGetChild) },\
  { "XmMessageBoxGetChild",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmMessageBoxGetChild) },\
  { "XmFileSelectionBoxGetChild",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmFileSelectionBoxGetChild) },\
  { "XmSelectionBoxGetChild",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmSelectionBoxGetChild) },\
  { "XmMainWindowSetAreas",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmMainWindowSetAreas) },\
  { "XmMainWindowSep1",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmMainWindowSep1) },\
  { "XmMainWindowSep2",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmMainWindowSep2) },\
  { "XmMainWindowSep3",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmMainWindowSep3) },\
  { "XmProcessTraversal",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmProcessTraversal) }, \
  { "XmInternAtom",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmInternAtom) }, \
  { "XmGetAtomName",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmGetAtomName) }, \
  { "XmGetColors",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmGetColors) }, \
  { "XmUpdateDisplay",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmUpdateDisplay) }, \
  { "XmAddWMProtocols",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmAddWMProtocols) }, \
  { "XmRemoveWMProtocols",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmRemoveWMProtocols) }, \
  { "XmAddWMProtocolCallback",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmAddWMProtocolCallback) }, \
  { "XmRemoveWMProtocolCallback",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmRemoveWMProtocolCallback) }, \
  { "XmMenuPosition",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmMenuPosition) }, \
  { "XmScaleGetValue",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmScaleGetValue) }, \
  { "XmScaleSetValue",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmScaleSetValue) }, \
  { "XmScrollBarGetValues",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmScrollBarGetValues) },\
  { "XmScrollBarSetValues",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmScrollBarSetValues) },\
  { "XmScrollVisible",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmScrollVisible) },\
  { "XmToggleButtonGetState",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmToggleButtonGetState) },\
  { "XmToggleButtonGadgetGetState",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmToggleButtonGadgetGetState) },\
  { "XmToggleButtonSetState",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmToggleButtonSetState) },\
  { "XmToggleButtonGadgetSetState",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmToggleButtonGadgetSetState) },\
  { "XmIsTraversable",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmIsTraversable) }, \
  { "XmOptionButtonGadget",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmOptionButtonGadget) }, \
  { "XmOptionLabelGadget",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmOptionLabelGadget) }, \
  { "XmGetVisibility",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmGetVisibility) }, \
  { "XmGetTearOffControl",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmGetTearOffControl) }, \
  { "XmGetTabGroup",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmGetTabGroup) }, \
  { "XmGetPostedFromWidget",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmGetPostedFromWidget) }, \
  { "XmGetFocusWidget",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmGetFocusWidget) }, \
  { "XmFileSelectionDoSearch",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmFileSelectionDoSearch) }, \


#define DTK_TK_LIST_TABLE \
  { "XmListAddItem",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmListAddItem) },  \
  { "XmListAddItemUnselected",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmListAddItemUnselected) },  \
  { "XmListAddItems",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmListAddItems) },  \
  { "XmListAddItemsUnselected",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmListAddItemsUnselected) },  \
  { "XmListDeleteAllItems",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmListDeleteAllItems) },  \
  { "XmListDeleteItem",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmListDeleteItem) },  \
  { "XmListDeleteItems",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmListDeleteItems) },  \
  { "XmListDeleteItemsPos",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmListDeleteItemsPos) },  \
  { "XmListDeletePos",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmListDeletePos) },  \
  { "XmListDeletePositions",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmListDeletePositions) },  \
  { "XmListDeselectAllItems",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmListDeselectAllItems) },  \
  { "XmListDeselectItem",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmListDeselectItem) },  \
  { "XmListDeselectPos",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmListDeselectPos) },  \
  { "XmListGetKbdItemPos",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmListGetKbdItemPos) },  \
  { "XmListGetSelectedPos",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmListGetSelectedPos) },  \
  { "XmListGetMatchPos",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmListGetMatchPos) },  \
  { "XmListItemExists",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmListItemExists) },  \
  { "XmListItemPos",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmListItemPos) },  \
  { "XmListPosSelected",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmListPosSelected) },  \
  { "XmListPosToBounds",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmListPosToBounds) },  \
  { "XmListReplaceItemsPos",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmListReplaceItemsPos) },  \
  { "XmListReplaceItemsPosUnselected",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmListReplaceItemsPosUnselected) },  \
  { "XmListSelectItem",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmListSelectItem) },  \
  { "XmListSelectPos",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmListSelectPos) },  \
  { "XmListSetAddMode",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmListSetAddMode) },  \
  { "XmListSetBottomItem",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmListSetBottomItem) },  \
  { "XmListSetBottomPos",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmListSetBottomPos) },  \
  { "XmListSetHorizPos",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmListSetHorizPos) },  \
  { "XmListSetItem",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmListSetItem) },  \
  { "XmListSetKbdItemPos",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmListSetKbdItemPos) },  \
  { "XmListSetPos",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmListSetPos) },  \
  { "XmListUpdateSelectedList",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmListUpdateSelectedList) }, \

#define DTK_TK_TEXT_TABLE \
  { "XmTextDisableRedisplay",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextDisableRedisplay) },  \
  { "XmTextEnableRedisplay",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextEnableRedisplay) },  \
  { "XmTextGetTopCharacter",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextGetTopCharacter) },  \
  { "XmTextSetTopCharacter",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextSetTopCharacter) },  \
  { "XmTextScroll",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextScroll) },  \
  { "XmTextFindString",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextFindString) },  \
  { "XmTextGetBaseline",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextGetBaseline) },  \
  { "XmTextGetEditable",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextGetEditable) },  \
  { "XmTextGetInsertionPosition",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextGetInsertionPosition) },  \
  { "XmTextGetLastPosition",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextGetLastPosition) },  \
  { "XmTextGetMaxLength",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextGetMaxLength) },  \
  { "XmTextGetSelection",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextGetSelection) },  \
  { "XmTextGetSelectionPosition",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextGetSelectionPosition) },  \
  { "XmTextGetString",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextGetString) },  \
  { "XmTextInsert",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextInsert) },  \
  { "XmTextPosToXY",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextPosToXY) },  \
  { "XmTextRemove",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextRemove) },  \
  { "XmTextReplace",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextReplace) },  \
  { "XmTextSetEditable",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextSetEditable) },  \
  { "XmTextSetHighlight",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextSetHighlight) },  \
  { "XmTextSetInsertionPosition",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextSetInsertionPosition) },  \
  { "XmTextSetMaxLength",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextSetMaxLength) },  \
  { "XmTextSetSelection",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextSetSelection) },  \
  { "XmTextSetString",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextSetString) },  \
  { "XmTextShowPosition",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextShowPosition) },  \
  { "XmTextXYToPos",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextXYToPos) },  \
  { "XmTextClearSelection",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextClearSelection) },  \
  { "XmTextCopy",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextCopy) },  \
  { "XmTextCut",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextCut) },  \
  { "XmTextPaste",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextPaste) },  \
  { "XmTextSetAddMode",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextSetAddMode) },  \
  { "XmTextFieldGetBaseline",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextGetBaseline) },  \
  { "XmTextFieldGetEditable",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextGetEditable) },  \
  { "XmTextFieldGetInsertionPosition",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextGetInsertionPosition) },  \
  { "XmTextFieldGetLastPosition",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextGetLastPosition) },  \
  { "XmTextFieldGetMaxLength",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextGetMaxLength) },  \
  { "XmTextFieldGetSelection",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextGetSelection) },  \
  { "XmTextFieldGetSelectionPosition",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextGetSelectionPosition) },  \
  { "XmTextFieldGetString",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextGetString) },  \
  { "XmTextFieldInsert",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextInsert) },  \
  { "XmTextFieldPosToXY",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextPosToXY) },  \
  { "XmTextFieldRemove",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextRemove) },  \
  { "XmTextFieldReplace",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextReplace) },  \
  { "XmTextFieldSetEditable",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextSetEditable) },  \
  { "XmTextFieldSetHighlight",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextSetHighlight) },  \
  { "XmTextFieldSetInsertionPosition",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextSetInsertionPosition) },  \
  { "XmTextFieldSetMaxLength",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextSetMaxLength) },  \
  { "XmTextFieldSetSelection",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextSetSelection) },  \
  { "XmTextFieldSetString",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextSetString) },  \
  { "XmTextFieldShowPosition",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextShowPosition) },  \
  { "XmTextFieldXYToPos",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextXYToPos) },  \
  { "XmTextFieldClearSelection",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextClearSelection) },  \
  { "XmTextFieldCopy",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextCopy) },  \
  { "XmTextFieldCut",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextCut) },  \
  { "XmTextFieldPaste",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextPaste) },  \
  { "XmTextFieldSetAddMode",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XmTextSetAddMode) },  \


#endif /* _Dtksh_xmextra_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
