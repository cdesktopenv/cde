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
/* $TOG: xmdtksym.c /main/6 1998/04/20 12:55:58 mgreess $ */

#include	"shell.h" 
#include <signal.h>
#include <fcntl.h>
#include <X11/X.h>
#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/CoreP.h>
#include <X11/StringDefs.h>
#include <Xm/XmStrDefs.h>
#include <Xm/List.h>
#include <setjmp.h>
#include <string.h>
#include <ctype.h>
#include <Xm/Xm.h>
#include <Xm/Protocols.h>
#include <Dt/Service.h>
#include <Dt/Wsm.h>
#include <Dt/HourGlass.h>
#include <Dt/Help.h>
#include <Dt/EnvControlP.h>
#include "hash.h"
#include "stdio.h"
#define NO_AST
#include "dtksh.h"
#undef NO_AST
#include "exksh.h"
#include "xmksh.h"
#include "xmcmds.h"
#include "dtkcmds.h"
#include "xmcvt.h"
#include "widget.h"
#include "extra.h"
#include "xmwidgets.h"
#include "msgs.h"

extern unsigned long ed_crlf ();
extern unsigned long ed_expand ();
extern unsigned long ed_flush ();
extern unsigned long ed_fulledit ();
extern unsigned long ed_getchar ();
extern unsigned long ed_macro ();
extern unsigned long ed_putchar ();
extern unsigned long ed_ringbell ();
extern unsigned long ed_setup ();
extern unsigned long ed_ungetchar ();
extern unsigned long ed_virt_to_phys ();
extern unsigned long ed_window ();
extern unsigned long editb;
extern unsigned long env_init ();
extern unsigned long hist_cancel ();
extern unsigned long hist_close ();
extern unsigned long hist_copy ();
extern unsigned long hist_eof ();
extern unsigned long hist_find ();
extern unsigned long hist_flush ();
extern unsigned long hist_list ();
extern unsigned long hist_locate ();
extern unsigned long hist_match ();
extern unsigned long hist_open ();
extern unsigned long hist_position ();
extern unsigned long hist_ptr;
extern unsigned long hist_subst ();
extern unsigned long hist_word ();
extern unsigned long main ();

extern unsigned long path_absolute ();
extern unsigned long path_alias ();
extern unsigned long path_basename ();
extern unsigned long path_exec ();
extern unsigned long path_expand ();
extern unsigned long path_get ();
extern unsigned long path_join ();
extern unsigned long path_open ();
extern unsigned long path_pwd ();
extern unsigned long path_relative ();
extern unsigned long path_search ();
/* extern unsigned long pathcanon (); */
/* extern unsigned long sh; */
/* extern unsigned long streval (); */
/* extern unsigned long strmatch (); */
/* extern unsigned long strperm (); */
extern unsigned long st;
extern unsigned long test_binop ();
extern unsigned long test_inode ();
extern unsigned long tty_alt ();
extern unsigned long tty_check ();
extern unsigned long tty_cooked ();
extern unsigned long tty_get ();
extern unsigned long tty_raw ();
extern unsigned long tty_set ();

struct symarray Symarray[] = {
	{ "Toplevel", (unsigned long) &Toplevel },
	{ "do_XBell", (unsigned long) &do_XBell },
	{ "do_XRootWindowOfScreen", (unsigned long) &do_XRootWindowOfScreen },
	{ "do_XWidthOfScreen", (unsigned long) &do_XWidthOfScreen },
	{ "do_XHeightOfScreen", (unsigned long) &do_XHeightOfScreen },
	{ "do_XClearArea", (unsigned long) &do_XClearArea },
	{ "do_XClearWindow", (unsigned long) &do_XClearWindow },
	{ "do_XCopyArea", (unsigned long) &do_XCopyArea },
	{ "do_XDefineCursor", (unsigned long) &do_XDefineCursor },
	{ "do_XDrawArc", (unsigned long) &do_XDrawArc },
	{ "do_XDrawImageString", (unsigned long) &do_XDrawImageString },
	{ "do_XDrawLine", (unsigned long) &do_XDrawLine },
	{ "do_XDrawLines", (unsigned long) &do_XDrawLines },
	{ "do_XDrawPoint", (unsigned long) &do_XDrawPoint },
	{ "do_XDrawPoints", (unsigned long) &do_XDrawPoints },
	{ "do_XDrawRectangle", (unsigned long) &do_XDrawRectangle },
	{ "do_XDrawSegments", (unsigned long) &do_XDrawSegments },
	{ "do_XDrawString", (unsigned long) &do_XDrawString },
	{ "do_XFillArc", (unsigned long) &do_XFillArc },
	{ "do_XFillPolygon", (unsigned long) &do_XFillPolygon },
	{ "do_XFillRectangle", (unsigned long) &do_XFillRectangle },
	{ "do_XFlush", (unsigned long) &do_XFlush },
	{ "do_XSync", (unsigned long) &do_XSync },
	{ "do_XRaiseWindow", (unsigned long) &do_XRaiseWindow },
	{ "do_XUndefineCursor", (unsigned long) &do_XUndefineCursor },
	{ "do_XmCreateArrowButton", (unsigned long) &do_XmCreateArrowButton },
	{ "do_XmCreateArrowButtonGadget", (unsigned long) &do_XmCreateArrowButtonGadget },
	{ "do_XmCreateBulletinBoard", (unsigned long) &do_XmCreateBulletinBoard },
	{ "do_XmCreateBulletinBoardDialog", (unsigned long) &do_XmCreateBulletinBoardDialog },
	{ "do_XmCreateCascadeButton", (unsigned long) &do_XmCreateCascadeButton },
	{ "do_XmCreateCascadeButtonGadget", (unsigned long) &do_XmCreateCascadeButtonGadget },
	{ "do_XmCreateComboBox", (unsigned long) &do_XmCreateComboBox },
	{ "do_XmCreateCommand", (unsigned long) &do_XmCreateCommand },
	{ "do_XmCreateDialogShell", (unsigned long) &do_XmCreateDialogShell },
	{ "do_XmCreateDrawingArea", (unsigned long) &do_XmCreateDrawingArea },
	{ "do_XmCreateDrawnButton", (unsigned long) &do_XmCreateDrawnButton },
	{ "do_XmCreateErrorDialog", (unsigned long) &do_XmCreateErrorDialog },
	{ "do_XmCreateFileSelectionBox", (unsigned long) &do_XmCreateFileSelectionBox },
	{ "do_XmCreateFileSelectionDialog", (unsigned long) &do_XmCreateFileSelectionDialog },
	{ "do_XmCreateForm", (unsigned long) &do_XmCreateForm },
	{ "do_XmCreateFormDialog", (unsigned long) &do_XmCreateFormDialog },
	{ "do_XmCreateFrame", (unsigned long) &do_XmCreateFrame },
	{ "do_XmCreateInformationDialog", (unsigned long) &do_XmCreateInformationDialog },
	{ "do_XmCreateLabel", (unsigned long) &do_XmCreateLabel },
	{ "do_XmCreateLabelGadget", (unsigned long) &do_XmCreateLabelGadget },
	{ "do_XmCreateList", (unsigned long) &do_XmCreateList },
	{ "do_XmListAddItem", (unsigned long) &do_XmListAddItem },
	{ "do_XmListAddItemUnselected", (unsigned long) &do_XmListAddItemUnselected },
	{ "do_XmListAddItems", (unsigned long) &do_XmListAddItems },
	{ "do_XmListAddItemsUnselected", (unsigned long) &do_XmListAddItemsUnselected },
	{ "do_XmListDeleteAllItems", (unsigned long) &do_XmListDeleteAllItems },
	{ "do_XmListDeleteItem", (unsigned long) &do_XmListDeleteItem },
	{ "do_XmListDeleteItems", (unsigned long) &do_XmListDeleteItems },
	{ "do_XmListDeleteItemsPos", (unsigned long) &do_XmListDeleteItemsPos },
	{ "do_XmListDeletePos", (unsigned long) &do_XmListDeletePos },
	{ "do_XmListDeletePositions", (unsigned long) &do_XmListDeletePositions },
	{ "do_XmListDeselectAllItems", (unsigned long) &do_XmListDeselectAllItems },
	{ "do_XmListDeselectItem", (unsigned long) &do_XmListDeselectItem },
	{ "do_XmListDeselectPos", (unsigned long) &do_XmListDeselectPos },
	{ "do_XmListGetKbdItemPos", (unsigned long) &do_XmListGetKbdItemPos },
	{ "do_XmListGetSelectedPos", (unsigned long) &do_XmListGetSelectedPos },
	{ "do_XmListGetMatchPos", (unsigned long) &do_XmListGetMatchPos },
	{ "do_XmListItemExists", (unsigned long) &do_XmListItemExists },
	{ "do_XmListItemPos", (unsigned long) &do_XmListItemPos },
	{ "do_XmListPosSelected", (unsigned long) &do_XmListPosSelected },
	{ "do_XmListPosToBounds", (unsigned long) &do_XmListPosToBounds },
	{ "do_XmListReplaceItemsPos", (unsigned long) &do_XmListReplaceItemsPos },
	{ "do_XmListReplaceItemsPosUnselected", (unsigned long) &do_XmListReplaceItemsPosUnselected },
	{ "do_XmListSelectItem", (unsigned long) &do_XmListSelectItem },
	{ "do_XmListSelectPos", (unsigned long) &do_XmListSelectPos },
	{ "do_XmListSetAddMode", (unsigned long) &do_XmListSetAddMode },
	{ "do_XmListSetBottomItem", (unsigned long) &do_XmListSetBottomItem },
	{ "do_XmListSetBottomPos", (unsigned long) &do_XmListSetBottomPos },
	{ "do_XmListSetHorizPos", (unsigned long) &do_XmListSetHorizPos },
	{ "do_XmListSetItem", (unsigned long) &do_XmListSetItem },
	{ "do_XmListSetKbdItemPos", (unsigned long) &do_XmListSetKbdItemPos },
	{ "do_XmListSetPos", (unsigned long) &do_XmListSetPos },
	{ "do_XmListUpdateSelectedList", (unsigned long) &do_XmListUpdateSelectedList },
	{ "do_XmCreateMainWindow", (unsigned long) &do_XmCreateMainWindow },
	{ "do_XmCreateMenuBar", (unsigned long) &do_XmCreateMenuBar },
	{ "do_XmCreateMenuShell", (unsigned long) &do_XmCreateMenuShell },
	{ "do_XmCreateMessageBox", (unsigned long) &do_XmCreateMessageBox },
	{ "do_XmCreateMessageDialog", (unsigned long) &do_XmCreateMessageDialog },
	{ "do_XmCreateOptionMenu", (unsigned long) &do_XmCreateOptionMenu },
	{ "do_XmCreatePanedWindow", (unsigned long) &do_XmCreatePanedWindow },
	{ "do_XmCreatePopupMenu", (unsigned long) &do_XmCreatePopupMenu },
	{ "do_XmCreatePromptDialog", (unsigned long) &do_XmCreatePromptDialog },
	{ "do_XmCreatePulldownMenu", (unsigned long) &do_XmCreatePulldownMenu },
	{ "do_XmCreatePushButton", (unsigned long) &do_XmCreatePushButton },
	{ "do_XmCreatePushButtonGadget", (unsigned long) &do_XmCreatePushButtonGadget },
	{ "do_XmCreateQuestionDialog", (unsigned long) &do_XmCreateQuestionDialog },
	{ "do_XmCreateRadioBox", (unsigned long) &do_XmCreateRadioBox },
	{ "do_XmCreateRowColumn", (unsigned long) &do_XmCreateRowColumn },
	{ "do_XmCreateScale", (unsigned long) &do_XmCreateScale },
	{ "do_XmCreateScrollBar", (unsigned long) &do_XmCreateScrollBar },
	{ "do_XmCreateScrolledList", (unsigned long) &do_XmCreateScrolledList },
	{ "do_XmCreateScrolledText", (unsigned long) &do_XmCreateScrolledText },
	{ "do_XmCreateScrolledWindow", (unsigned long) &do_XmCreateScrolledWindow },
	{ "do_XmCreateSelectionBox", (unsigned long) &do_XmCreateSelectionBox },
	{ "do_XmCreateSelectionDialog", (unsigned long) &do_XmCreateSelectionDialog },
	{ "do_XmCreateSeparator", (unsigned long) &do_XmCreateSeparator },
	{ "do_XmCreateSeparatorGadget", (unsigned long) &do_XmCreateSeparatorGadget },
	{ "do_XmCreateText", (unsigned long) &do_XmCreateText },
	{ "do_XmCreateTextField", (unsigned long) &do_XmCreateTextField },
	{ "do_XmCreateToggleButton", (unsigned long) &do_XmCreateToggleButton },
	{ "do_XmCreateToggleButtonGadget", (unsigned long) &do_XmCreateToggleButtonGadget },
	{ "do_XmCreateWarningDialog", (unsigned long) &do_XmCreateWarningDialog },
	{ "do_XmCreateWorkArea", (unsigned long) &do_XmCreateWorkArea },
	{ "do_XmCreateWorkingDialog", (unsigned long) &do_XmCreateWorkingDialog },
	{ "do_DtCreateHelpDialog", (unsigned long) &do_DtCreateHelpDialog },
	{ "do_DtCreateHelpQuickDialog", (unsigned long) &do_DtCreateHelpQuickDialog },
	{ "do_DtCreatePrintSetupBox", (unsigned long) &do_DtCreatePrintSetupBox },
	{ "do_DtCreatePrintSetupDialog", (unsigned long) &do_DtCreatePrintSetupDialog },
	{ "do_DtCreatePDMJobSetup", (unsigned long) &do_DtCreatePDMJobSetup },
	{ "do_DtHelpReturnSelectedWidgetId", (unsigned long) &do_DtHelpReturnSelectedWidgetId },
	{ "do_DtHelpSetCatalogName", (unsigned long) &do_DtHelpSetCatalogName },
	{ "do_DtHelpQuickDialogGetChild", (unsigned long) &do_DtHelpQuickDialogGetChild },
	{ "do_XmCommandAppendValue", (unsigned long) &do_XmCommandAppendValue },
	{ "do_XmCommandError", (unsigned long) &do_XmCommandError },
	{ "do_XmCommandSetValue", (unsigned long) &do_XmCommandSetValue },
	{ "do_XmCommandGetChild", (unsigned long) &do_XmCommandGetChild },
	{ "do_XmMessageBoxGetChild", (unsigned long) &do_XmMessageBoxGetChild },
	{ "do_XmFileSelectionBoxGetChild", (unsigned long) &do_XmFileSelectionBoxGetChild },
	{ "do_XmSelectionBoxGetChild", (unsigned long) &do_XmSelectionBoxGetChild },
	{ "do_XmMainWindowSetAreas", (unsigned long) &do_XmMainWindowSetAreas },
	{ "do_XmMainWindowSep1", (unsigned long) &do_XmMainWindowSep1 },
	{ "do_XmMainWindowSep2", (unsigned long) &do_XmMainWindowSep2 },
	{ "do_XmMainWindowSep3", (unsigned long) &do_XmMainWindowSep3 },
	{ "do_XmProcessTraversal", (unsigned long) &do_XmProcessTraversal },
	{ "do_XmInternAtom", (unsigned long) &do_XmInternAtom },
	{ "do_XmGetAtomName", (unsigned long) &do_XmGetAtomName },
	{ "do_XmGetColors", (unsigned long) &do_XmGetColors },
	{ "do_XmUpdateDisplay", (unsigned long) &do_XmUpdateDisplay },
	{ "do_XmAddWMProtocols", (unsigned long) &do_XmAddWMProtocols },
	{ "do_XmRemoveWMProtocols", (unsigned long) &do_XmRemoveWMProtocols },
	{ "do_XmAddWMProtocolCallback", (unsigned long) &do_XmAddWMProtocolCallback },
	{ "do_XmRemoveWMProtocolCallback", (unsigned long) &do_XmRemoveWMProtocolCallback },
	{ "do_XmMenuPosition", (unsigned long) &do_XmMenuPosition },
	{ "do_XmIsTraversable", (unsigned long) &do_XmIsTraversable },
	{ "do_XmScaleGetValue", (unsigned long) &do_XmScaleGetValue },
	{ "do_XmScaleSetValue", (unsigned long) &do_XmScaleSetValue },
	{ "do_XmScrollBarGetValues", (unsigned long) &do_XmScrollBarGetValues },
	{ "do_XmScrollBarSetValues", (unsigned long) &do_XmScrollBarSetValues },
	{ "do_XmScrollVisible", (unsigned long) &do_XmScrollVisible },
	{ "do_XmToggleButtonGetState", (unsigned long) &do_XmToggleButtonGetState },
	{ "do_XmToggleButtonGadgetGetState", (unsigned long) &do_XmToggleButtonGadgetGetState },
	{ "do_XmToggleButtonSetState", (unsigned long) &do_XmToggleButtonSetState },
	{ "do_XmToggleButtonGadgetSetState", (unsigned long) &do_XmToggleButtonGadgetSetState },

	{ "do_XmTextDisableRedisplay", (unsigned long) &do_XmTextDisableRedisplay },
	{ "do_XmTextEnableRedisplay", (unsigned long) &do_XmTextEnableRedisplay },
	{ "do_XmTextGetTopCharacter", (unsigned long) &do_XmTextGetTopCharacter },
	{ "do_XmTextSetTopCharacter", (unsigned long) &do_XmTextSetTopCharacter },
	{ "do_XmTextScroll", (unsigned long) &do_XmTextScroll },
	{ "do_XmTextFindString", (unsigned long) &do_XmTextFindString },
	{ "do_XmTextGetBaseline", (unsigned long) &do_XmTextGetBaseline },
	{ "do_XmTextGetEditable", (unsigned long) &do_XmTextGetEditable },
	{ "do_XmTextGetInsertionPosition", (unsigned long) &do_XmTextGetInsertionPosition },
	{ "do_XmTextGetLastPosition", (unsigned long) &do_XmTextGetLastPosition },
	{ "do_XmTextGetMaxLength", (unsigned long) &do_XmTextGetMaxLength },
	{ "do_XmTextGetSelection", (unsigned long) &do_XmTextGetSelection },
	{ "do_XmTextGetSelectionPosition", (unsigned long) &do_XmTextGetSelectionPosition },
	{ "do_XmTextGetString", (unsigned long) &do_XmTextGetString },
	{ "do_XmTextInsert", (unsigned long) &do_XmTextInsert },
	{ "do_XmTextPosToXY", (unsigned long) &do_XmTextPosToXY },
	{ "do_XmTextRemove", (unsigned long) &do_XmTextRemove },
	{ "do_XmTextReplace", (unsigned long) &do_XmTextReplace },
	{ "do_XmTextSetEditable", (unsigned long) &do_XmTextSetEditable },
	{ "do_XmTextSetHighlight", (unsigned long) &do_XmTextSetHighlight },
	{ "do_XmTextSetInsertionPosition", (unsigned long) &do_XmTextSetInsertionPosition },
	{ "do_XmTextSetMaxLength", (unsigned long) &do_XmTextSetMaxLength },
	{ "do_XmTextSetSelection", (unsigned long) &do_XmTextSetSelection },
	{ "do_XmTextSetString", (unsigned long) &do_XmTextSetString },
	{ "do_XmTextShowPosition", (unsigned long) &do_XmTextShowPosition },
	{ "do_XmTextXYToPos", (unsigned long) &do_XmTextXYToPos },
	{ "do_XmTextClearSelection", (unsigned long) &do_XmTextClearSelection },
	{ "do_XmTextCopy", (unsigned long) &do_XmTextCopy },
	{ "do_XmTextCut", (unsigned long) &do_XmTextCut },
	{ "do_XmTextPaste", (unsigned long) &do_XmTextPaste },
	{ "do_XmTextSetAddMode", (unsigned long) &do_XmTextSetAddMode },
	{ "do_XmOptionButtonGadget", (unsigned long) &do_XmOptionButtonGadget },
	{ "do_XmOptionLabelGadget", (unsigned long) &do_XmOptionLabelGadget },
	{ "do_XmGetVisibility", (unsigned long) &do_XmGetVisibility },
	{ "do_XmGetTearOffControl", (unsigned long) &do_XmGetTearOffControl },
	{ "do_XmGetTabGroup", (unsigned long) &do_XmGetTabGroup },
	{ "do_XmGetPostedFromWidget", (unsigned long) &do_XmGetPostedFromWidget },
	{ "do_XmGetFocusWidget", (unsigned long) &do_XmGetFocusWidget },
	{ "do_XmFileSelectionDoSearch", (unsigned long) &do_XmFileSelectionDoSearch },
	{ "do_XtAddCallback", (unsigned long) &do_XtAddCallback },
	{ "do_XtRemoveCallback", (unsigned long) &do_XtRemoveCallback },
	{ "do_XtAddEventHandler", (unsigned long) &do_XtAddEventHandler },
	{ "do_XtRemoveEventHandler", (unsigned long) &do_XtRemoveEventHandler },
	{ "do_XtAddInput", (unsigned long) &do_XtAddInput },
	{ "do_XtAddTimeOut", (unsigned long) &do_XtAddTimeOut },
	{ "do_XtCreateApplicationShell", (unsigned long) &do_XtCreateApplicationShell },
	{ "do_XtInitialize", (unsigned long) &do_XtInitialize },
	{ "do_XtAugmentTranslations", (unsigned long) &do_XtAugmentTranslations },
	{ "do_XtCallCallbacks", (unsigned long) &do_XtCallCallbacks },
	{ "do_XtHasCallbacks", (unsigned long) &do_XtHasCallbacks },
	{ "do_XtCreateManagedWidget", (unsigned long) &do_XtCreateManagedWidget },
	{ "do_XtCreatePopupShell", (unsigned long) &do_XtCreatePopupShell },
	{ "do_XtCreateWidget", (unsigned long) &do_XtCreateWidget },
	{ "do_XtDestroyWidget", (unsigned long) &do_XtDestroyWidget },
	{ "do_XtGetValues", (unsigned long) &do_XtGetValues },
	{ "do_XtIsManaged", (unsigned long) &do_XtIsManaged },
	{ "do_XtIsRealized", (unsigned long) &do_XtIsRealized },
	{ "do_XtIsSensitive", (unsigned long) &do_XtIsSensitive },
	{ "do_XtIsShell", (unsigned long) &do_XtIsShell },
	{ "do_XtLastTimestampProcessed", (unsigned long) &do_XtLastTimestampProcessed },
	{ "do_XtMainLoop", (unsigned long) &do_XtMainLoop },
	{ "do_XtDisplay", (unsigned long) &do_XtDisplay },
	{ "do_XtDisplayOfObject", (unsigned long) &do_XtDisplayOfObject },
	{ "do_XtNameToWidget", (unsigned long) &do_XtNameToWidget },
	{ "do_XtScreen", (unsigned long) &do_XtScreen },
	{ "do_XtWindow", (unsigned long) &do_XtWindow },
	{ "do_XtManageChildren", (unsigned long) &do_XtManageChildren },
	{ "do_XtIsSubclass", (unsigned long) &do_XtIsSubclass },
	{ "do_XtClass", (unsigned long) &do_XtClass },
	{ "do_XtMapWidget", (unsigned long) &do_XtMapWidget },
	{ "do_XtOverrideTranslations", (unsigned long) &do_XtOverrideTranslations },
	{ "do_XtParent", (unsigned long) &do_XtParent },
	{ "do_XtPopdown", (unsigned long) &do_XtPopdown },
	{ "do_XtPopup", (unsigned long) &do_XtPopup },
	{ "do_XtRealizeWidget", (unsigned long) &do_XtRealizeWidget },
	{ "do_XtRemoveAllCallbacks", (unsigned long) &do_XtRemoveAllCallbacks },
	{ "do_XtSetSensitive", (unsigned long) &do_XtSetSensitive },
	{ "do_XtSetValues", (unsigned long) &do_XtSetValues },
	{ "do_XtUninstallTranslations", (unsigned long) &do_XtUninstallTranslations },
	{ "do_XtUnmanageChildren", (unsigned long) &do_XtUnmanageChildren },
	{ "do_XtUnmapWidget", (unsigned long) &do_XtUnmapWidget },
	{ "do_XtUnrealizeWidget", (unsigned long) &do_XtUnrealizeWidget },
	{ "do_DtSessionRestorePath", (unsigned long) &do_DtSessionRestorePath },
	{ "do_DtSessionSavePath", (unsigned long) &do_DtSessionSavePath },
	{ "do_DtShellIsIconified", (unsigned long) &do_DtShellIsIconified },
	{ "do_DtSetStartupCommand", (unsigned long) &do_DtSetStartupCommand },
	{ "do_DtSetIconifyHint", (unsigned long) &do_DtSetIconifyHint },
	{ "do_DtWsmAddWorkspaceFunctions", (unsigned long) &do_DtWsmAddWorkspaceFunctions },
	{ "do_DtWsmRemoveWorkspaceFunctions", (unsigned long) &do_DtWsmRemoveWorkspaceFunctions },
	{ "do_DtWsmGetCurrentWorkspace", (unsigned long) &do_DtWsmGetCurrentWorkspace },
	{ "do_DtWsmSetCurrentWorkspace", (unsigned long) &do_DtWsmSetCurrentWorkspace },
	{ "do_DtWsmGetWorkspaceList", (unsigned long) &do_DtWsmGetWorkspaceList },
	{ "do_DtWsmGetWorkspacesOccupied", (unsigned long) &do_DtWsmGetWorkspacesOccupied },
	{ "do_DtWsmSetWorkspacesOccupied", (unsigned long) &do_DtWsmSetWorkspacesOccupied },
	{ "do_DtWsmGetCurrentBackdropWindow", (unsigned long) &do_DtWsmGetCurrentBackdropWindow },
	{ "do_DtWsmOccupyAllWorkspaces", (unsigned long) &do_DtWsmOccupyAllWorkspaces },
	{ "do__DtGetHourGlassCursor", (unsigned long) &do__DtGetHourGlassCursor },
	{ "do__DtTurnOnHourGlass", (unsigned long) &do__DtTurnOnHourGlass },
	{ "do__DtTurnOffHourGlass", (unsigned long) &do__DtTurnOffHourGlass },
	{ "do_DtWsmAddCurrentWorkspaceCallback", (unsigned long) &do_DtWsmAddCurrentWorkspaceCallback },
	{ "do_DtWsmRemoveWorkspaceCallback", (unsigned long) &do_DtWsmRemoveWorkspaceCallback },
	{ "do_DtDbLoad", (unsigned long) &do_DtDbLoad },
	{ "do_DtDbReloadNotify", (unsigned long) &do_DtDbReloadNotify },
	{ "do_DtActionExists", (unsigned long) &do_DtActionExists },
	{ "do_DtActionLabel", (unsigned long) &do_DtActionLabel },
	{ "do_DtActionDescription", (unsigned long) &do_DtActionDescription },
	{ "do_DtActionInvoke", (unsigned long) &do_DtActionInvoke },
	{ "do_DtDtsLoadDataTypes", (unsigned long) &do_DtDtsLoadDataTypes },
	{ "do_DtDtsFileToDataType", (unsigned long) &do_DtDtsFileToDataType },
	{ "do_DtDtsFileToAttributeValue", (unsigned long) &do_DtDtsFileToAttributeValue },
	{ "do_DtDtsFileToAttributeList", (unsigned long) &do_DtDtsFileToAttributeList },
	{ "do_DtDtsDataTypeToAttributeValue", (unsigned long) &do_DtDtsDataTypeToAttributeValue },
	{ "do_DtDtsDataTypeToAttributeList", (unsigned long) &do_DtDtsDataTypeToAttributeList },
	{ "do_DtDtsFindAttribute", (unsigned long) &do_DtDtsFindAttribute },
	{ "do_DtDtsDataTypeNames", (unsigned long) &do_DtDtsDataTypeNames },
	{ "do_DtDtsSetDataType", (unsigned long) &do_DtDtsSetDataType },
	{ "do_DtDtsDataTypeIsAction", (unsigned long) &do_DtDtsDataTypeIsAction },
	{ "do_ttdt_open", (unsigned long) &do_ttdt_open },
	{ "do_ttdt_close", (unsigned long) &do_ttdt_close },
	{ "do_tttk_Xt_input_handler", (unsigned long) &do_tttk_Xt_input_handler },
	{ "do_ttdt_session_join", (unsigned long) &do_ttdt_session_join },
	{ "do_ttdt_session_quit", (unsigned long) &do_ttdt_session_quit },
	{ "do_ttdt_file_event", (unsigned long) &do_ttdt_file_event },
	{ "do_ttdt_file_join", (unsigned long) &do_ttdt_file_join },
	{ "do_ttdt_file_quit", (unsigned long) &do_ttdt_file_quit },
	{ "do_ttdt_Get_Modified", (unsigned long) &do_ttdt_Get_Modified },
	{ "do_ttdt_Save", (unsigned long) &do_ttdt_Save },
	{ "do_ttdt_Revert", (unsigned long) &do_ttdt_Revert },
	{ "do_tt_error_pointer", (unsigned long) &do_tt_error_pointer },
	{ "do_tttk_message_destroy", (unsigned long) &do_tttk_message_destroy },
	{ "do_tttk_message_reject", (unsigned long) &do_tttk_message_reject },
	{ "do_tttk_message_fail", (unsigned long) &do_tttk_message_fail },
	{ "do_tt_file_netfile", (unsigned long) &do_tt_file_netfile },
	{ "do_tt_netfile_file", (unsigned long) &do_tt_netfile_file },
	{ "do_tt_host_file_netfile", (unsigned long) &do_tt_host_file_netfile },
	{ "do_tt_host_netfile_file", (unsigned long) &do_tt_host_netfile_file },
	{ "do_tt_message_reply", (unsigned long) &do_tt_message_reply },
	{ "do_catopen", (unsigned long) &do_catopen },
	{ "do_catgets", (unsigned long) &do_catgets },
	{ "do_catclose", (unsigned long) &do_catclose },
	{ "do_DtWidgetInfo", (unsigned long) &do_DtWidgetInfo },
	{ "do_DtLoadWidget", (unsigned long) &do_DtLoadWidget },
	{ "ed_crlf", (unsigned long) &ed_crlf },
	{ "ed_expand", (unsigned long) &ed_expand },
	{ "ed_flush", (unsigned long) &ed_flush },
	{ "ed_fulledit", (unsigned long) &ed_fulledit },
	{ "ed_getchar", (unsigned long) &ed_getchar },
	{ "ed_macro", (unsigned long) &ed_macro },
	{ "ed_putchar", (unsigned long) &ed_putchar },
	{ "ed_ringbell", (unsigned long) &ed_ringbell },
	{ "ed_setup", (unsigned long) &ed_setup },
	{ "ed_ungetchar", (unsigned long) &ed_ungetchar },
	{ "ed_virt_to_phys", (unsigned long) &ed_virt_to_phys },
	{ "ed_window", (unsigned long) &ed_window },
	{ "editb", (unsigned long) &editb },
	{ "env_blank", (unsigned long) &env_blank },
	{ "env_get", (unsigned long) &env_get },
	{ "env_set", (unsigned long) &env_set },
	{ "env_set_gbl", (unsigned long) &env_set_gbl },
	{ "env_set_var", (unsigned long) &env_set_var },
	{ "handle_to_widget", (unsigned long) &handle_to_widget },
	{ "ksh_eval", (unsigned long) &ksh_eval },
	{ "main", (unsigned long) &main },
	{ "path_absolute", (unsigned long) &path_absolute },
	{ "path_alias", (unsigned long) &path_alias },
	{ "path_basename", (unsigned long) &path_basename },
	{ "path_exec", (unsigned long) &path_exec },
	{ "path_expand", (unsigned long) &path_expand },
	{ "path_get", (unsigned long) &path_get },
	{ "path_join", (unsigned long) &path_join },
	{ "path_open", (unsigned long) &path_open },
	{ "path_pwd", (unsigned long) &path_pwd },
	{ "path_relative", (unsigned long) &path_relative },
	{ "path_search", (unsigned long) &path_search },
	{ "pathcanon", (unsigned long) &pathcanon },
	{ "printerr", (unsigned long) &printerr },
	{ "printerrf", (unsigned long) &printerrf },
	{ "sh", (unsigned long) &sh },
	{ "streval", (unsigned long) &streval },
	{ "strmatch", (unsigned long) &strmatch },
	{ "strperm", (unsigned long) &strperm },
	{ "test_binop", (unsigned long) &test_binop },
	{ "test_inode", (unsigned long) &test_inode },
	{ "tty_alt", (unsigned long) &tty_alt },
	{ "tty_check", (unsigned long) &tty_check },
	{ "tty_cooked", (unsigned long) &tty_cooked },
	{ "tty_get", (unsigned long) &tty_get },
	{ "tty_raw", (unsigned long) &tty_raw },
	{ "tty_set", (unsigned long) &tty_set },
	{ 0, 0 }
};

int Symsize = sizeof(Symarray) / sizeof(struct symarray);
