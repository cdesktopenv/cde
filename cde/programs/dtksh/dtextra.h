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
/* $XConsortium: dtextra.h /main/3 1995/11/01 15:51:46 rswiston $ */
/*	Copyright (c) 1991, 1992 UNIX System Laboratories, Inc. */
/*	All Rights Reserved     */

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF          */
/*	UNIX System Laboratories, Inc.			        */
/*	The copyright notice above does not evidence any        */
/*	actual or intended publication of such source code.     */

#ifndef _Dtksh_dtextra_h
#define _Dtksh_dtextra_h

#include "widget.h"
#include "dtkcmds.h"
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

#define DTK_EXTRA_TABLE \
	{ "DtLoadWidget", NV_BLTIN|BLT_ENV|BLT_SPC,  lcl_cast(do_DtLoadWidget) }, \
	{ "DtWidgetInfo", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_DtWidgetInfo) }, \
	{ "XBell", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XBell) }, \
        { "XRootWindowOfScreen", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XRootWindowOfScreen) }, \
        { "XWidthOfScreen", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XWidthOfScreen) }, \
        { "XHeightOfScreen", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XHeightOfScreen) }, \
        { "XDefineCursor", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XDefineCursor) }, \
        { "XUndefineCursor", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XUndefineCursor) }, \
	{ "XFlush", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XFlush) }, \
	{ "XSync", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XSync) }, \
	{ "XRaiseWindow", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XRaiseWindow) }, \
	{ "XtInitialize", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtInitialize) }, \
	{ "XtIsSensitive", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtIsSensitive) }, \
	{ "XtIsShell", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtIsShell) }, \
	{ "XtIsRealized", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtIsRealized) }, \
	{ "XtIsManaged", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtIsManaged) }, \
	{ "XtCreateManagedWidget", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtCreateManagedWidget) }, \
	{ "XtCreateApplicationShell", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtCreateApplicationShell) }, \
	{ "XtCreateWidget", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtCreateWidget) }, \
	{ "XtDestroyWidget", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtDestroyWidget) }, \
	{ "XtSetValues", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtSetValues) }, \
	{ "XtSetSensitive", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtSetSensitive) }, \
        { "XtAugmentTranslations", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtAugmentTranslations) }, \
        { "XtOverrideTranslations", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtOverrideTranslations) }, \
        { "XtUninstallTranslations", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtUninstallTranslations) }, \
	{ "XtAddCallback", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtAddCallback) }, \
	{ "XtRemoveCallback", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtRemoveCallback) }, \
	{ "XtRemoveAllCallbacks", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtRemoveAllCallbacks) }, \
	{ "XtCallCallbacks", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtCallCallbacks) }, \
	{ "XtHasCallbacks", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtHasCallbacks) }, \
	{ "XtAddEventHandler", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtAddEventHandler) }, \
	{ "XtRemoveEventHandler", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtRemoveEventHandler) }, \
	{ "XtGetValues", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtGetValues) }, \
	{ "XtCreatePopupShell", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtCreatePopupShell) }, \
	{ "XtPopup", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtPopup) }, \
	{ "XtPopdown", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtPopdown) }, \
	{ "XtMapWidget", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtMapWidget) }, \
	{ "XtUnmapWidget", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtUnmapWidget) }, \
	{ "XtManageChildren", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtManageChildren) }, \
	{ "XtIsSubclass", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtIsSubclass) }, \
	{ "XtClass", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtClass) }, \
	{ "XtUnmanageChildren", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtUnmanageChildren) }, \
	{ "XtAddTimeOut", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtAddTimeOut) }, \
	{ "XtRemoveTimeOut", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtRemoveTimeOut) }, \
	{ "XtAddInput", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtAddInput) }, \
	{ "XtRemoveInput", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtRemoveInput) }, \
	{ "XtAddWorkProc", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtAddWorkProc) }, \
	{ "XtRemoveWorkProc", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtRemoveWorkProc) }, \
	{ "XtRealizeWidget", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtRealizeWidget) }, \
	{ "XtUnrealizeWidget", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtUnrealizeWidget) }, \
        { "DtSessionRestorePath", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_DtSessionRestorePath) }, \
        { "DtSessionSavePath", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_DtSessionSavePath) }, \
        { "DtShellIsIconified", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_DtShellIsIconified) }, \
        { "DtSetStartupCommand", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_DtSetStartupCommand) }, \
        { "DtSetIconifyHint", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_DtSetIconifyHint) }, \
        { "DtWsmAddWorkspaceFunctions", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_DtWsmAddWorkspaceFunctions) }, \
        { "DtWsmRemoveWorkspaceFunctions", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_DtWsmRemoveWorkspaceFunctions) }, \
        { "DtWsmGetCurrentWorkspace", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_DtWsmGetCurrentWorkspace) }, \
       { "DtWsmSetCurrentWorkspace", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_DtWsmSetCurrentWorkspace) }, \
        { "DtWsmGetWorkspaceList", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_DtWsmGetWorkspaceList) }, \
        { "DtWsmGetWorkspacesOccupied", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_DtWsmGetWorkspacesOccupied) }, \
        { "DtWsmSetWorkspacesOccupied", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_DtWsmSetWorkspacesOccupied) }, \
        { "DtWsmGetCurrentBackdropWindow", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_DtWsmGetCurrentBackdropWindow) }, \
        { "DtWsmOccupyAllWorkspaces", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_DtWsmOccupyAllWorkspaces) }, \
	{ "DtGetHourGlassCursor", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do__DtGetHourGlassCursor) }, \
	{ "DtTurnOnHourGlass", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do__DtTurnOnHourGlass) }, \
	{ "DtTurnOffHourGlass", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do__DtTurnOffHourGlass) }, \
        { "_DtGetHourGlassCursor", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do__DtGetHourGlassCursor) }, \
        { "_DtTurnOnHourGlass", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do__DtTurnOnHourGlass) }, \
        { "_DtTurnOffHourGlass", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do__DtTurnOffHourGlass) }, \
        { "DtWsmAddCurrentWorkspaceCallback", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_DtWsmAddCurrentWorkspaceCallback) }, \
        { "DtWsmRemoveWorkspaceCallback", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_DtWsmRemoveWorkspaceCallback) }, \
	{ "DtDbLoad", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_DtDbLoad) }, \
	{ "DtDbReloadNotify", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_DtDbReloadNotify) }, \
	{ "DtActionExists", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_DtActionExists) }, \
	{ "DtActionLabel", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_DtActionLabel) }, \
	{ "DtActionDescription", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_DtActionDescription) }, \
	{ "DtActionInvoke", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_DtActionInvoke) }, \
	{ "DtDtsLoadDataTypes", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_DtDtsLoadDataTypes) }, \
	{ "DtDtsFileToDataType", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_DtDtsFileToDataType) }, \
	{ "DtDtsFileToAttributeValue", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_DtDtsFileToAttributeValue) }, \
	{ "DtDtsFileToAttributeList", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_DtDtsFileToAttributeList) }, \
	{ "DtDtsDataTypeToAttributeValue", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_DtDtsDataTypeToAttributeValue) }, \
	{ "DtDtsDataTypeToAttributeList", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_DtDtsDataTypeToAttributeList) }, \
	{ "DtDtsFindAttribute", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_DtDtsFindAttribute) }, \
	{ "DtDtsDataTypeNames", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_DtDtsDataTypeNames) }, \
	{ "DtDtsSetDataType", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_DtDtsSetDataType) }, \
	{ "DtDtsDataTypeIsAction", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_DtDtsDataTypeIsAction) }, \
	{ "ttdt_open", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_ttdt_open) }, \
	{ "ttdt_close", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_ttdt_close) }, \
	{ "tttk_Xt_input_handler", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_tttk_Xt_input_handler) }, \
	{ "ttdt_session_join", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_ttdt_session_join) }, \
	{ "ttdt_session_quit", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_ttdt_session_quit) }, \
	{ "ttdt_file_event", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_ttdt_file_event) }, \
	{ "ttdt_file_join", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_ttdt_file_join) }, \
	{ "ttdt_file_quit", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_ttdt_file_quit) }, \
	{ "ttdt_Get_Modified", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_ttdt_Get_Modified) }, \
	{ "ttdt_Save", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_ttdt_Save) }, \
	{ "ttdt_Revert", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_ttdt_Revert) }, \
	{ "tt_error_pointer", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_tt_error_pointer) }, \
	{ "tttk_message_destroy", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_tttk_message_destroy) }, \
	{ "tttk_message_reject", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_tttk_message_reject) }, \
	{ "tttk_message_fail", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_tttk_message_fail) }, \
	{ "tt_file_netfile", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_tt_file_netfile) }, \
	{ "tt_netfile_file", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_tt_netfile_file) }, \
	{ "tt_host_file_netfile", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_tt_host_file_netfile) }, \
	{ "tt_host_netfile_file", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_tt_host_netfile_file) }, \
	{ "tt_message_reply", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_tt_message_reply) },

#define DTK_EXTRA_TABLE2 \
	{ "XClearArea", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XClearArea) }, \
	{ "XClearWindow", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XClearWindow) }, \
        { "XCopyArea", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XCopyArea) }, \
	{ "XDrawArc", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XDrawArc) }, \
	{ "XDrawImageString", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XDrawImageString) }, \
	{ "XDrawLine", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XDrawLine) }, \
	{ "XDrawLines", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XDrawLines) }, \
	{ "XDrawPoint", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XDrawPoint) }, \
	{ "XDrawPoints", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XDrawPoints) }, \
	{ "XDrawRectangle", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XDrawRectangle) }, \
	{ "XDrawSegments", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XDrawSegments) }, \
	{ "XDrawString", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XDrawString) }, \
	{ "XFillArc", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XFillArc) }, \
	{ "XFillPolygon", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XFillPolygon) }, \
	{ "XFillRectangle", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XFillRectangle) }, \
	{ "XTextWidth", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XTextWidth) }, \
	{ "XtMainLoop", NV_BLTIN|BLT_SPC, lcl_cast(do_XtMainLoop) }, \
	{ "XtDisplay", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtDisplay) }, \
	{ "XtDisplayOfObject", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtDisplayOfObject) }, \
	{ "XtNameToWidget", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtNameToWidget) }, \
        { "XtScreen",  NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtScreen) }, \
	{ "XtWindow", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtWindow) }, \
	{ "XtParent", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtParent) }, \
	{ "XtLastTimestampProcessed", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_XtLastTimestampProcessed) }, \
	{ "catopen", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_catopen) }, \
	{ "catgets", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_catgets) }, \
	{ "catclose", NV_BLTIN|BLT_ENV|BLT_SPC, lcl_cast(do_catclose) },



#define DTK_EXTRA_ALIAS \
	"XtManageChild", NV_NOFREE|NV_EXPORT, 	"XtManageChildren", \
	"XtUnmanageChild", NV_NOFREE|NV_EXPORT, 	"XtUnmanageChildren",

#endif /* _Dtksh_dtextra_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
