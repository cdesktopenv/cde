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
/* $XConsortium: Help.h /main/4 1995/11/02 14:39:56 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 *   FILE:           Help.h
 *
 *   COMPONENT_NAME: Desktop File Manager
 *
 *   DESCRIPTION:    dtfile help defines
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *
 ****************************************************************************
 ************************************<+>*************************************/

#ifndef _VF_Help_h
#define _VF_Help_h


#define MAIN_HELP_DIALOG    0
#define HYPER_HELP_DIALOG   1

extern DialogClass * helpClass;


typedef struct
{
   Boolean       displayed;
   Position      x;
   Position      y;
   Dimension     width;
   Dimension     height;

   String        idString;
   String        volString;
   String        topicTitle;
   String        fileType;
   int           helpType;
} HelpData, * HelpDataPtr;


typedef struct
{
   Widget    shell;
   Widget    helpDialog;
} HelpRec;



extern void HelpRequestCB( 
                        Widget w,
                        XtPointer clientData,
                        XtPointer callData) ;
extern void DTHelpRequestCB(
                        Widget w,
                        XtPointer clientData,
                        XtPointer callData) ;
extern void TrashHelpRequestCB( 
                        Widget w,
                        XtPointer clientData,
                        XtPointer callData) ;
extern void ShowHelpDialog(
                        Widget parentShell,
                        XtPointer topLevelRec,
                        int dialogType,
                        DialogData *dialogData,
                        char *idString,
                        char *volString,
                        char *topicTitle,
                        char *fileType,
                        int helpType) ;
extern void ShowTrashHelpDialog(
                        Widget parentShell,
                        int dialogType,
                        DialogData *dialogData,
                        char *idString,
                        char *volString) ;
extern void ShowDTHelpDialog(
                        Widget parentShell,
                        int workspaceNum,
                        int dialogType,
                        DialogData *dialogData,
                        char *idString,
                        char *volString,
                        char *topicTitle,
                        char *fileType,
                        int helpType) ;
extern XtPointer MapFileTypeToHelpString (
                        String filetype,
                        String topicTitle) ;

extern void ObjectHelp( 
                        Widget w,
                        XtPointer clientData,
                        XtPointer callData) ;

extern void closeCB_mainHelpDialog(
                        Widget wid,
                        XtPointer client_data,
                        XtPointer cbs) ;



/* Help defines for Trash.c */
#define HELP_TRASH_DIALOG_STR "FMTrashDialogDE"

/* Help defines for ChangeDir.c */
#define HELP_CHANGEDIR_DIALOG_STR "FMChangeToDialogDE"

/* Help defines for File.c */
#define HELP_NAMECHANGE_DIALOG_STR "FileManagerNameChangeDialog"

/* Help defines for the File Manager Popup ... File.c */
#define HELP_POPUP_MENU_STR "FMPopupMenuDE"

/* Help defines for the Desktop popup */
#define HELP_DESKTOP_POPUP_MENU_STR "FMDesktopMenuDE"

/* Help defines for FileDialog.c (move) */
#define HELP_MOVE_DIALOG_STR "FMMoveDialogDE"

/* Help defines for FileDialog.c (copy) */
#define HELP_COPY_DIALOG_STR "FMCopyDialogDE"

/* Help defines for FileDialog.c (link) */
#define HELP_LINK_DIALOG_STR "FMCopyLinkDialogDE"

/* Help defines for FileDialog.c (create) */
#define HELP_CREATE_FILE_DIALOG_STR "FMCreateFileDialogDE"
#define HELP_CREATE_DIR_DIALOG_STR "FMCreateFolderDialogDE"

/* Help defines for FileMgr.c */
#define HELP_FILE_MANAGER_VIEW_STR "FMDirectoryViewDE"

/* Help defines for FileOp.c */
#define HELP_FILE_MANAGER_REP_REN "FMCopyWarnRenDialogDE"
#define HELP_FILE_MANAGER_REP_MRG "FMCopyWarnMrgDialogDE"
#define HELP_FILE_MANAGER_MULTI   "FMCopyWarnMultiDialogDE"

/* Help defines for Filter.c */
#define HELP_FILTER_DIALOG_STR "FMFilterDialogDE"

/* Help defines for Find.c */
#define HELP_FIND_DIALOG_STR "FMFindDialogDE"

/* Help defines for Menu.c */
#define HELP_FILE_MENU_STR "FMFileMenuDE"
#define HELP_SELECTED_MENU_STR "FMActionsMenuDE"
#define HELP_VIEW_MENU_STR "FMViewMenuDE"
#define HELP_HELP_MENU_STR "FMHelpMenuDE"
#define HELP_RENAME_COMMAND_STR "FileManagerRenameCommand"
#define HELP_MOVE_AS_COMMAND_STR "FileManagerMoveCommand"
#define HELP_COPY_AS_COMMAND_STR "FileManagerCopyAsCommand"
#define HELP_LINK_AS_COMMAND_STR "FileManagerLinkCommand"
#define HELP_PROPERTIES_COMMAND_STR "FileManagerPropertiesCommand"
#define HELP_PUT_ON_DT_COMMAND_STR "FileManagerPutOnDTCommand"

#define HELP_TRASH_FILES_COMMAND_STR "FileManagerTrashFilesCommand"
#define HELP_REREAD_COMMAND_STR "FileManagerRereadCommand"
#define HELP_FAST_CHANGE_TO_COMMAND_STR "FileManagerFastChangeToCommand"
#define HELP_CHANGE_TO_COMMAND_STR "FileManagerChangeToCommand"
#define HELP_NEW_VIEW_COMMAND_STR "FMViewMenuDE"
#define HELP_CLEAN_UP_COMMAND_STR "FileManagerCleanUpCommand"
#define HELP_SELECT_ALL_COMMAND_STR "FileManagerSelectAllCommand"
#define HELP_UNSELECT_ALL_COMMAND_STR "FileManagerUnselectAllCommand"
#define HELP_SHOW_HIDDEN_COMMAND_STR "FileManagerShowHiddenCommand"
#define HELP_PREFERENCES_COMMAND_STR "FileManagerPreferencesCommand"
#define HELP_FILTER_COMMAND_STR "FileManagerFilterCommand"
#define HELP_SAVE_SETTINGS_COMMAND_STR "FileManagerSaveSettingsCommand"
#define HELP_HOME_TOPIC "_HOMETOPIC"
#define HELP_TASKS_STR "Tasks"
#define HELP_REFERENCE_STR "Reference"
#define HELP_HELP_MODE_STR "FileManagerHelpMode"
#define HELP_USING_STR "_HOMETOPIC"
#define HELP_ABOUT_STR "_COPYRIGHT"
#define HELP_TOC_STR "TOC"
#define HELP_TRASH_OVERVIEW_TOPIC_STR "ToOpenTheTrashCanTA"
#define HELP_TRASH_TASKS_TOPIC_STR "ToDeleteAnObjectTA"

/* Help defines for ModAttr.c */
#define HELP_PROPERTIES_DIALOG_STR "FMPropertiesDialogDE"

/* Help defines for Prefs.c */
#define HELP_PREFERENCES_DIALOG_STR "FMPreferencesDialogDE"
#define HELP_PREFERENCES_HEADERS_STR "FileManagerPrefHeadersOptions"
#define HELP_PREFERENCES_PLACEMENT_STR "FileManagerPrefPlacementOptions"
#define HELP_PREFERENCES_VIEW_STR "FileManagerPrefViewOptions"
#define HELP_PREFERENCES_SHOW_STR "FileManagerPrefShowOptions"
#define HELP_PREFERENCES_ORDER_STR "FileManagerPrefOrderOptions"
#define HELP_PREFERENCES_DIRECTION_STR "FileManagerPrefDirectionOptions"

#endif /* _VF_Help_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
