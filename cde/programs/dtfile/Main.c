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
/* $TOG: Main.c /main/29 1999/09/17 13:35:04 mgreess $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 *   FILE:           Main.c
 *
 *   COMPONENT_NAME: Desktop File Manager (dtfile)
 *
 *   Description:    This file contains the main program for dtfile.
 *                   It also contains functions for session management and
 *                   directory view control.
 *
 *   FUNCTIONS: BlackPixelOfScreen
 *		BuildAndShowIconName
 *		CheckForOpenDirectory
 *		CheckOpenDir
 *		CleanUp
 *		CloseView
 *		DirectoryChanged
 *		DisplayHeight
 *		DisplayWidth
 *		DtfileCvtStringToDTIcon
 *		DtfileCvtStringToDirection
 *		DtfileCvtStringToGrid
 *		DtfileCvtStringToObjPlace
 *		DtfileCvtStringToOpenDir
 *		DtfileCvtStringToOrder
 *		DtfileCvtStringToTree
 *		DtfileCvtStringToTreeFiles
 *		DtfileCvtStringToView
 *		DtfileStringToDirection
 *		DtfileStringToGrid
 *		DtfileStringToOrder
 *		DtfileStringToTree
 *		DtfileStringToTreeFiles
 *		DtfileStringToView
 *		ErrorHandler
 *		ExitApp
 *		ExitHandler
 *		ForceMyIconClosed
 *		ForceMyIconOpen
 *		GetNewView
 *		GetPWD
 *		GetRestrictedDirectory
 *		HandleTtRequest
 *		LoadViews
 *		MarqueeSelect
 *		MoveDefaultSettings
 *		ObserveTtNotice
 *		OpenDirectories
 *		ReloadDatabases
 *		RemoveTextFields
 *		RestoreSession
 *		RestoreSettingsFile
 *		RestrictModeUsage
 *		ReturnDesktopPtr
 *		RootWindowOfScreen
 *		SaveDefaultCancelCB
 *		SaveDefaultOkCB
 *		SaveSession
 *		SaveSessionCallback
 *		SaveSettingsCB
 *		SetupSendRequestArgs
 *		Stop
 *		ToolkitErrorHandler
 *		Usage
 *		ViewAccept
 *		ViewDirectoryHandler
 *		ViewDirectoryProc
 *		ViewHomeDirectoryHandler
 *		ViewSessionHandler
 *		ViewToolsDirectoryHandler
 *		WaitForResponseAndExit
 *		WhitePixelOfScreen
 *		XtMalloc
 *		_DtNextToken
 *		_DtWsmAddMarqueeSelectionCallback
 *		if
 *		main
 *		strcmp
 *		strlen
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *
 ****************************************************************************
 ************************************<+>*************************************/

#include <sys/types.h>
#include <stdio.h>
#include <sys/stat.h>
#include <signal.h>

#ifdef __osf__
#include <sys/wait.h>
#endif /* __osf__ */

#include <errno.h>
#include <pwd.h>
#include <stdlib.h>
#include <ctype.h>

#ifndef sun /* don't need the nl_types.h file */
#include <nl_types.h>
#endif /* !sun */

#include <locale.h>

#include <Xm/XmP.h>
#include <Xm/DrawingA.h>
#include <Xm/DrawingAP.h>
#include <Xm/MessageB.h>
#include <Xm/RowColumn.h>
#include <Xm/MwmUtil.h>

#include <Dt/Icon.h>
#include <Dt/IconP.h>
#include <Dt/IconFile.h>

#include <Dt/HelpDialog.h>

#ifdef SHAPE
#include <X11/extensions/shape.h>
#endif

#include <X11/Shell.h>
#include <X11/Xatom.h>
#include <Xm/Protocols.h>
#include <X11/keysymdef.h>
#ifdef HAVE_EDITRES
#include <X11/Xmu/Editres.h>
#endif
#include <Dt/Session.h>
#include <Dt/DtP.h>
#include <Dt/Connect.h>
#include <Dt/FileM.h>
#include <Dt/Indicator.h>
#include <Dt/Lock.h>
#include <Dt/UserMsg.h>
#include <Dt/Wsm.h>
#include <Dt/WsmP.h>
#include <Dt/DtNlUtils.h>
#include <Dt/CommandM.h>
#include <Dt/EnvControlP.h>
#include <Dt/Dts.h>

#include "Encaps.h"
#include "SharedProcs.h"

#include <Tt/tttk.h>

#include "Help.h"
#include "FileMgr.h"
#include "Desktop.h"
#include "Main.h"
#include "ChangeDir.h"
#include "Prefs.h"
#include "Common.h"
#include "Filter.h"
#include "Find.h"
#include "ModAttr.h"

/* When openDir resource is set to NEW
   File Manager will use this prefix to find for a different icon
   to display.
*/
#define ICON_OPEN_PREFIX "OPEN_"

/* THESE ARE ONLY STRINGS WHICH DO NOT NEED TO BE LOCALIZED */
/* Don't use '#define' since you end up with multiple copies */
char DTFILE_CLASS_NAME[] = "Dtfile";
char DTFILE_HELP_NAME[] = "Filemgr";
char DESKTOP_DIR[] = ".dt/Desktop";

static char WS_RES_HEADER[] = ".Workspace.";
static char SEC_HELP_RES_HEADER[] = ".secondaryHelpDialogCount: ";
static char WS_LOAD_RES_HEADER[] = "Workspace";
static char SEC_LOAD_HELP_RES_HEADER[] = "secondaryHelpDialogCount";
static char RESTRICTED_HEADER[] = "-restricted";
static char VIEW_HEADER[] = "-view";


/*  Structure, resource definitions, for View's optional parameters.  */

typedef struct
{
   char * no_view;
   char * session;
   char * directories;
   int tree_view;
   int tree_files;
   int view;
   int order;
   int direction;
   int grid;
   int instanceIconWidth;
   int instanceIconHeight;
   XmFontList user_font;
   Dimension tool_width;
   Dimension tool_height;
   Dimension dir_width;
   Dimension dir_height;
   Boolean prompt_user;
   char * root_title;
   char * title;
   char * help_volume;
   char * restricted;
   int dragThreshold;
   int rereadTime;
   int checkBrokenLink;
   int maxDirectoryProcesses;
   int maxRereadProcesses;
   int maxRereadProcsPerTick;
   int trashWait;
   int desktopIconType;
   Boolean showFilesystem;
   Boolean showDropZone;
   Boolean showEmptySet;
   Boolean showEmptyMsg;
   int openDirType;
   Boolean restrictMode;
   int desktopPlacement;
   Boolean freezeOnConfig;
#if defined(__hpux) || defined(sun)
   Boolean follow_links;
#endif
   char * fileMgrIcon;
   char * appMgrIcon;
   char * trashIcon;
   int retryLoadDesktop;
   int smallIconWidth;
   int smallIconHeight;
   int largeIconWidth;
   int largeIconHeight;
   Boolean emptyTrashOnExit;
} ApplicationArgs, *ApplicationArgsPtr;

static ApplicationArgs application_args;

/********    Static Function Declarations    ********/

static void ErrorHandler(
                        Display *disp,
                        XErrorEvent *event) ;
static void ToolkitErrorHandler(
                        char *message) ;
static void Usage(
                        char **argv) ;
static void RestrictModeUsage(
                        char **argv) ;
static void Stop( void ) ;
static void RestoreSettingsFile( void ) ;
static void MoveDefaultSettings(
                        int mode) ;
static void SaveDefaultCancelCB(
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void SaveDefaultOkCB(
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void SaveSession(
                        char *path) ;
static int RestoreSession(
                        char *path,
                        int type_of_restore,
                        char *directory) ;
static void GetPWD(
                        char current_directory[]) ;
static void OpenDirectories(
                        char *directory_set,
                        char *type) ;
static Tt_callback_action ObserveTtNotice(
			Tt_message msg,
			Tt_pattern pat) ;
static void ViewSessionHandler(
                        Tt_message msg);
static void ViewDirectoryHandler(
                        Tt_message msg);
static void ViewHomeDirectoryHandler(
                        Tt_message msg);
static void ViewToolsDirectoryHandler(
                        Tt_message msg);
static void ExitHandler(
                        Tt_message msg,
                        XtPointer clientData,
                        String * messageFields,
                        int numFields);
static void ReloadDatabases( void );
static void ViewAccept(
                        View *view,
                        Tt_message msg);
static void LoadViews (
                        int num_views,
                        XrmDatabase db,
                        char *host_name,
                        char *directory_name,
                        char *type,
                        Tt_message msg);
static void RemoveTextFields (
                        XtPointer client_data,
                        DialogData * old_dialog_data,
                        DialogData * new_dialog_data) ;
static void DtfileCvtStringToObjPlace (
                        XrmValue *args,
                        Cardinal numArgs,
                        XrmValue *fromVal,
                        XrmValue *toVal) ;
static unsigned char *_DtNextToken (
                        unsigned char *pchIn,
                        int *pLen,
                        unsigned char **ppchNext) ;
static void DtfileCvtStringToOpenDir (
                        XrmValue *args,
                        Cardinal numArgs,
                        XrmValue *fromVal,
                        XrmValue *toVal) ;
static void DtfileCvtStringToDTIcon (
                        XrmValue *args,
                        Cardinal numArgs,
                        XrmValue *fromVal,
                        XrmValue *toVal) ;
static void DtfileCvtStringToTree (
                        XrmValue *args,
                        Cardinal numArgs,
                        XrmValue *fromVal,
                        XrmValue *toVal) ;
static void DtfileCvtStringToTreeFiles (
                        XrmValue *args,
                        Cardinal numArgs,
                        XrmValue *fromVal,
                        XrmValue *toVal) ;
static void DtfileCvtStringToView (
                        XrmValue *args,
                        Cardinal numArgs,
                        XrmValue *fromVal,
                        XrmValue *toVal) ;
static void DtfileCvtStringToOrder (
                        XrmValue *args,
                        Cardinal numArgs,
                        XrmValue *fromVal,
                        XrmValue *toVal) ;
static void DtfileCvtStringToDirection (
                        XrmValue *args,
                        Cardinal numArgs,
                        XrmValue *fromVal,
                        XrmValue *toVal) ;
static void DtfileCvtStringToGrid (
                        XrmValue *args,
                        Cardinal numArgs,
                        XrmValue *fromVal,
                        XrmValue *toVal) ;
static void DtfileStringToTree(
                        char *str,
                        int *type) ;
static void DtfileStringToTreeFiles(
                        char *str,
                        int *type) ;
static void DtfileStringToView(
                        char *str,
                        int *type) ;
static void DtfileStringToOrder(
                        char *str,
                        int *type) ;
static void DtfileStringToDirection(
                        char *str,
                        int *type) ;
static void DtfileStringToGrid(
                        char *str,
                        int *type) ;
static void SetupSendRequestArgs(
                        ApplicationArgs application_args,
			Tt_message msg) ;
static void BuildAndShowIconName(
                        char *file_type_name,
                        unsigned char view,
                        unsigned char show_type,
                        Widget widget) ;
static void MarqueeSelect (
                        Widget w,
                        int type,
                        Position x,
                        Position y,
                        Dimension width,
                        Dimension height,
                        XtPointer client_data);
static void WaitForResponseAndExit( void ) ;
static void ExitApp(
                        XtPointer clientData,
                        String *words,
                        int num_fields) ;
static Widget post_dialog(
			Widget parent,
			char *title,
			char *msg,
			void (*callback)());
static void DtErrExitCB(
			Widget widget,
			XtPointer client_data,
			XtPointer call_data);

/********    End Static Function Declarations    ********/

/*  debugging flag  */
#ifdef DEBUG
int debug = 0;
#endif

/*  performance flag  */
#ifdef DT_PERFORMANCE
int perform = 0;
#endif

/*  The id's of the dialogs registered by main  */

int file_mgr_dialog;
int change_dir_dialog;
int preferences_dialog;
int filter_dialog;
int find_dialog;
int mod_attr_dialog;
int help_dialog;

/* The shared menu button and pane Id's */

Widget * create_dataBtn;
Widget * renameBtn;
Widget * moveBtn;
Widget * duplicateBtn;
Widget * linkBtn;
Widget * findBtn;
Widget * create_directoryBtn;
Widget * change_directoryBtn;
Widget * showHiddenMenu;
Widget * preferencesBtn;
Widget * filterBtn;
Widget * defaultEnvBtn;
Widget * homeBarBtn;
Widget * upBarBtn;
Widget * newViewBtn;
Widget * cleanUpBtn;
Widget * separator1;
Widget * separator2;
Widget * terminalBtn;
Widget * usingHelp = NULL;
Widget * fileManagerHelp = NULL;
Widget * applicationManagerHelp = NULL;
Widget * usingHelpTrash = NULL;


/* Bitmask used to indicate the current sensitivity state of shared menu btns */

unsigned int currentMenuStates = ( RENAME | MOVE | DUPLICATE | LINK | TRASH |
                                 MODIFY | CHANGEDIR | PREFERENCES | FILTER |
                                 FIND | CREATE_DIR | CREATE_FILE |
                                 PUT_ON_DESKTOP | PUTBACK |
                                 CLEAN_UP | MOVE_UP |
                                 HOME | CHANGE_DIR | TERMINAL);

/* Drag manager globals */
Boolean b1Drag;
Boolean dragActive = False;

/* Desktop Globals */
int numColumns;
int numRows;

/*  Globally referenced application name.  Set to argv[0] in main  */

char * application_name = NULL;


/*  uid for root user; used when we are checking access permissions  */

long root_user = -1;


/*  Home host name  */

char home_host_name[MAX_PATH];
char users_home_dir[MAX_PATH];


/*  Toolbox directory  */

char * desktop_dir = NULL;
char * trash_dir = NULL;
char * remote_sys_dir = NULL;

/* Restor type */

int restoreType = NORMAL_RESTORE;


/*  Black and White pixels  */

Pixel black_pixel;
Pixel white_pixel;


/*  File manager view set before a new view is created and   */
/*  used to propagate visual attributes from the initiating  */
/*  view to the new view.                                    */

XtPointer initiating_view = NULL;
Boolean special_view = False;
Boolean TrashView = False;


/*  system wide user font  */

XmFontList user_font;


/* Global localizable strings */

String openInPlace;
String openNewView;


/* Global dialog button labels, as XmStrings */

XmString okXmString;
XmString cancelXmString;
XmString helpXmString;
XmString applyXmString;
XmString closeXmString;


/* Global top level widget  */

Widget toplevel;

/* Global Application resources */
Boolean showFilesystem;
Boolean showDropZone;
Boolean showEmptySet;
Boolean showEmptyMsg;
Boolean restrictMode;
int openDirType;
char *root_title;
char *fileMgrTitle;
char *fileMgrHelpVol;
int desktopIconType;
int rereadTime;
int checkBrokenLink;
int trashWait;
int desktopPlacement;
Boolean freezeOnConfig;
#if defined(__hpux) || defined(sun)
Boolean follow_links;
#endif
int treeType;
int treeFiles;
int viewType;
int orderType;
int directionType;
int randomType;
int instanceWidth;
int instanceHeight;
unsigned char keybdFocusPolicy;
int special_treeType;
int special_treeFiles;
int special_viewType;
int special_orderType;
int special_directionType;
int special_randomType;
char *special_restricted;
char *special_title;
char *special_helpVol;
char *fileMgrIcon;
char *appMgrIcon;
char *trashIcon;
int retryLoadDesktopInfo;

int smallIconWidth;
int smallIconHeight;
int largeIconWidth;
int largeIconHeight;

Boolean emptyTrashOnExit;

#ifdef SHAPE
Boolean shapeExtension;
#endif

/* Drag state variables */
Boolean B1DragPossible = False;
Boolean B2DragPossible = False;
Boolean ProcessBtnUp = False;
Boolean ProcessBtnUpCD = True;
int initialDragX;
int initialDragY;
int dragThreshold;
int xErrorDetected = False;

/* BMenu button binding */
int bMenuButton;

View ** view_set = NULL;
int view_count = 0;
int view_set_size = 0;

/*  Globals used within this file.  */

static Display * display;
char * dt_path = NULL;
static Boolean message_display_enabled = True;


static Atom save_yourself_atom;
static Atom command_atom;
static Atom wm_state_atom;
static Atom save_mode;

/*  Structure used on a save session to see if a dt is iconic  */

typedef struct
{
   int state;
   Window icon;
} WM_STATE;


/*  Application resource list definition  */

static XrmOptionDescRec option_list[] =
{
   {  "-noview",            "noView",          XrmoptionIsArg,   NULL  },
   {  "-session",           "session",         XrmoptionSepArg,  NULL  },
   {  "-dir",               "folder",          XrmoptionSepArg,  NULL  },
   {  "-folder",            "folder",          XrmoptionSepArg,  NULL  },
   {  "-tree",              "treeView",        XrmoptionSepArg,  NULL  },
   {  "-tree_files",        "treeFiles",       XrmoptionSepArg,  NULL  },
   {  VIEW_HEADER,          "view",            XrmoptionSepArg,  NULL  },
   {  "-order",             "order",           XrmoptionSepArg,  NULL  },
   {  "-direction",         "direction",       XrmoptionSepArg,  NULL  },
   {  "-grid",              "grid",            XrmoptionSepArg,  NULL  },
   {  RESTRICTED_HEADER,    "restricted",      XrmoptionIsArg,   NULL  },
   {  "-title",             "title",           XrmoptionSepArg,  NULL  },
   {  "-help_volume",       "help_volume",     XrmoptionSepArg,  NULL  },
   {  "-noprompt",          "promptUser",      XrmoptionNoArg,  "False"  },
   {  "-small_icon_width",  "smallIconWidth",  XrmoptionSepArg,  NULL  },
   {  "-small_icon_height", "smallIconHeight", XrmoptionSepArg,  NULL  },
   {  "-large_icon_width",  "largeIconWidth",  XrmoptionSepArg,  NULL  },
   {  "-large_icon_height", "largeIconHeight", XrmoptionSepArg,  NULL  },
};


static XtResource resources[] =
{
   {
     "noView", "NoView", XmRString, sizeof (char *),
     XtOffset (ApplicationArgsPtr, no_view), XmRImmediate, (XtPointer) NULL,
   },

   {
     "session", "Session", XmRString, sizeof (char *),
     XtOffset (ApplicationArgsPtr, session), XmRImmediate, (XtPointer) NULL,
   },

   {
     "folder", "Folder", XmRString, sizeof (char *),
     XtOffset (ApplicationArgsPtr, directories), XmRImmediate, (XtPointer) NULL,
   },

   {
     "treeView", "TreeView", "Tree", sizeof (int),
     XtOffset (ApplicationArgsPtr, tree_view), XmRImmediate,
     (XtPointer) UNSET_VALUE,
   },

   {
     "treeFiles", "TreeFiles", "TreeFiles", sizeof (int),
     XtOffset (ApplicationArgsPtr, tree_files), XmRImmediate,
     (XtPointer) UNSET_VALUE,
   },

   {
     "view", "View", "View", sizeof (int),
     XtOffset (ApplicationArgsPtr, view), XmRImmediate, (XtPointer) UNSET_VALUE,
   },

   {
     "order", "Order", "Order", sizeof (int),
     XtOffset (ApplicationArgsPtr, order), XmRImmediate, (XtPointer) UNSET_VALUE,
   },

   {
     "direction", "Direction", "Direction", sizeof (int),
     XtOffset (ApplicationArgsPtr, direction), XmRImmediate,
     (XtPointer) UNSET_VALUE,
   },

   {
     "grid", "Grid", "Grid", sizeof (int),
     XtOffset (ApplicationArgsPtr, grid), XmRImmediate, (XtPointer) UNSET_VALUE,
   },

   {
     "instanceIconWidth", "InstanceIconWidth", XmRInt, sizeof (int),
     XtOffset (ApplicationArgsPtr, instanceIconWidth), XmRImmediate,
     (XtPointer) 256,
   },

   {
     "instanceIconHeight", "InstanceIconHeight", XmRInt, sizeof (int),
     XtOffset (ApplicationArgsPtr, instanceIconHeight), XmRImmediate,
     (XtPointer) 256,
   },

   {
     "restricted", "Restricted", XmRString, sizeof (char *),
     XtOffset (ApplicationArgsPtr, restricted), XmRImmediate, (XtPointer) NULL,
   },

   {
     "title", "Title", XmRString, sizeof (char *),
     XtOffset (ApplicationArgsPtr, title), XmRImmediate, (XtPointer)NULL,
   },

   {
     "help_volume", "Help_volume", XmRString, sizeof (char *),
     XtOffset (ApplicationArgsPtr, help_volume), XmRImmediate, (XtPointer)NULL,
   },

   {
     "userFont", "XmCFontList", XmRFontList, sizeof (XmFontList),
     XtOffset (ApplicationArgsPtr, user_font), XmRString, (XtPointer) "Fixed",
   },

   {
     "toolWidth", "ToolWidth", XmRHorizontalDimension, sizeof (Dimension),
     XtOffset (ApplicationArgsPtr, tool_width), XmRImmediate, (XtPointer) 700,
   },

   {
     "toolHeight", "ToolHeight", XmRVerticalDimension, sizeof (Dimension),
     XtOffset (ApplicationArgsPtr, tool_height), XmRImmediate, (XtPointer) 250,
   },

   {
     "dirWidth", "DirWidth", XmRHorizontalDimension, sizeof (Dimension),
     XtOffset (ApplicationArgsPtr, dir_width), XmRImmediate, (XtPointer) 555,
   },

   {
     "dirHeight", "DirHeight", XmRVerticalDimension, sizeof (Dimension),
     XtOffset (ApplicationArgsPtr, dir_height), XmRImmediate, (XtPointer) 400,
   },

   {
     "promptUser", "PromptUser", XmRBoolean, sizeof (Boolean),
     XtOffset (ApplicationArgsPtr, prompt_user), XmRImmediate, (XtPointer) True,
   },

   {
     "rootTitle", "RootTitle", XmRString, sizeof (char *),
     XtOffset (ApplicationArgsPtr, root_title), XmRImmediate, (XtPointer)"ROOT",
   },

   {
     "moveThreshold", "MoveThreshold", XmRInt, sizeof (int),
     XtOffset (ApplicationArgsPtr, dragThreshold), XmRImmediate, (XtPointer) 4,
   },

   {
     "rereadTime", "RereadTime", XmRInt, sizeof (int),
     XtOffset (ApplicationArgsPtr, rereadTime), XmRImmediate, (XtPointer) 3,
   },

   {
     "checkBrokenLink", "CheckBrokenLink", XmRInt, sizeof (int),
     XtOffset (ApplicationArgsPtr, checkBrokenLink), XmRImmediate,
     (XtPointer) 180,
   },

   {
     "maxDirectoryProcesses", "MaxDirectoryProcesses", XmRInt, sizeof (int),
     XtOffset (ApplicationArgsPtr, maxDirectoryProcesses), XmRImmediate,
     (XtPointer) 10,
   },

   {
     "maxRereadProcesses", "MaxRereadProcesses", XmRInt, sizeof (int),
     XtOffset (ApplicationArgsPtr, maxRereadProcesses), XmRImmediate,
     (XtPointer) 5,
   },

   {
     "maxRereadProcsPerTick", "MaxRereadProcsPerTick", XmRInt, sizeof (int),
     XtOffset (ApplicationArgsPtr, maxRereadProcsPerTick), XmRImmediate,
     (XtPointer) 1,
   },

   {
     "trashWait", "TrashWait", XmRInt, sizeof (int),
     XtOffset (ApplicationArgsPtr, trashWait), XmRImmediate, (XtPointer) 1,
   },

   {
     "desktopIcon", "DesktopIcon", "DesktopIcon", sizeof (int),
     XtOffset (ApplicationArgsPtr, desktopIconType), XmRImmediate,
     (XtPointer)LARGE,
   },

   {
     "showFilesystem", "ShowFilesystem", XmRBoolean, sizeof (Boolean),
     XtOffset (ApplicationArgsPtr, showFilesystem), XmRImmediate,
     (XtPointer)True,
   },

   {
     "showDropZone", "ShowDropZone", XmRBoolean, sizeof (Boolean),
     XtOffset (ApplicationArgsPtr, showDropZone), XmRImmediate,
     (XtPointer)False,
   },

   {
     "showEmptySet", "ShowEmptySet", XmRBoolean, sizeof (Boolean),
     XtOffset (ApplicationArgsPtr, showEmptySet), XmRImmediate,
     (XtPointer)False,
   },

   {
     "showEmptyMsg", "ShowEmptyMsg", XmRBoolean, sizeof (Boolean),
     XtOffset (ApplicationArgsPtr, showEmptyMsg), XmRImmediate,
     (XtPointer)False,
   },

   {
     "openFolder", "OpenFolder", "OpenFolder", sizeof (int),
     XtOffset (ApplicationArgsPtr, openDirType), XmRImmediate,
     (XtPointer)CURRENT,
   },

   {
     "restrictMode", "RestrictMode", XmRBoolean, sizeof (Boolean),
     XtOffset (ApplicationArgsPtr, restrictMode), XmRImmediate,
     (XtPointer)False,
   },

   {
     "objectPlacement", "ObjectPlacement", "ObjectPlacement", sizeof (int),
     XtOffset (ApplicationArgsPtr, desktopPlacement), XmRImmediate,
     (XtPointer)(OBJ_PLACE_TOP_PRIMARY | OBJ_PLACE_RIGHT_SECONDARY),
   },

   {
     "freezeOnConfig", "FreezeOnConfig", XmRBoolean, sizeof (Boolean),
     XtOffset (ApplicationArgsPtr, freezeOnConfig), XmRImmediate,
     (XtPointer)True,
   },

   {
     "fileManagerIcon", "FileManagerIcon", XmRString, sizeof (char *),
     XtOffset (ApplicationArgsPtr, fileMgrIcon), XmRImmediate,
     (XtPointer) HOME_ICON_NAME,
   },

   {
     "appManagerIcon", "AppManagerIcon", XmRString, sizeof (char *),
     XtOffset (ApplicationArgsPtr, appMgrIcon), XmRImmediate,
     (XtPointer) TOOL_ICON_NAME,
   },

   {
     "trashIcon", "TrashIcon", XmRString, sizeof (char *),
     XtOffset (ApplicationArgsPtr, trashIcon), XmRImmediate,
     (XtPointer) TRASH_ICON_NAME,
   },

   {
     "retryLoadDesktop", "RetryLoadDesktop", XmRInt, sizeof (int),
     XtOffset (ApplicationArgsPtr, retryLoadDesktop), XmRImmediate,
     (XtPointer) 10,
   },

   {
     "smallIconWidth", "SmallIconWidth", XmRInt, sizeof (int),
     XtOffset (ApplicationArgsPtr, smallIconWidth), XmRImmediate,
     (XtPointer) 24,
   },

   {
     "smallIconHeight", "SmallIconHeight", XmRInt, sizeof (int),
     XtOffset (ApplicationArgsPtr, smallIconHeight), XmRImmediate,
     (XtPointer) 24,
   },

   {
     "largeIconWidth", "LargeIconWidth", XmRInt, sizeof (int),
     XtOffset (ApplicationArgsPtr, largeIconWidth), XmRImmediate,
     (XtPointer) 38,
   },

   {
     "largeIconHeight", "LargeIconHeight", XmRInt, sizeof (int),
     XtOffset (ApplicationArgsPtr, largeIconHeight), XmRImmediate,
     (XtPointer) 38,
   },

   {
     "emptyTrashOnExit", "EmptyTrashOnExit", XmRBoolean, sizeof (Boolean),
     XtOffset (ApplicationArgsPtr, emptyTrashOnExit), XmRImmediate,
     (XtPointer)False,
   },

#if defined(__hpux) || defined(sun)
   { "followLinks", "FollowLinks", XmRBoolean, sizeof(Boolean),
     XtOffset(ApplicationArgsPtr, follow_links), XmRImmediate,
     (XtPointer) False,
   },
#endif

};

XtActionsRec actionTable[] = {
   {"Space", (XtActionProc)VFTextChangeSpace},
   {"EscapeFM", (XtActionProc)CancelOut},
};

/************************************************************************
 *
 *  DT File
 *      The main program for the file manager.
 *
 ************************************************************************/

extern XtInputId ProcessToolTalkInputId;

#ifdef __osf__
extern void sigchld_handler(int);
#endif /* __osf__ */

void
main(
        unsigned int argc,
        char **argv )
{
#ifdef DT_PERFORMANCE
   struct timeval update_time_s;
   struct timeval update_time_f;
   struct timeval update_time_ss;
   struct timeval update_time_fs;
#endif
   struct passwd * pw;
   char current_directory[MAX_PATH];
   struct passwd * pwInfo;
   char * homeDir;
   XEvent event;
   XtInputMask pending;
   Boolean eventDebugging;
   int offset;
   KeySym keysym;
   int displayHeight;
   int displayWidth;
   Arg args[10];
   int n;
   char * tmpStr;
   XSetWindowAttributes sAttributes;
   Window root;
#ifdef SHAPE
   int base1, base2;
#endif
   XrmValue resource_value;
   XrmDatabase db;
   char *rep_type;
   char *string, *str;
   int i;
   int ttFd;    /* ToolTalk file descriptor */
   char *sessId;
   Tt_pattern events2Watch;
   Tt_pattern requests2Handle;
   Tt_message msg;
   Tt_status status;
#ifdef __osf__
   struct sigaction sa, osa;
#endif /* __osf__ */
   int session_flag = 0;

#ifdef DT_PERFORMANCE
   printf("  Start\n");
   gettimeofday(&update_time_ss, NULL);
#endif
   (void) signal (SIGINT, (void (*)())Stop);

   /* We don't want any zombie children, do we? */
#ifdef __osf__
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags   =  0;

    if (sigaction(SIGCHLD, &sa, &osa) < 0)
    /* error handling follows, none for now */
        ;
#else
   (void) signal (SIGCHLD, SIG_IGN);
#endif /* __osf__ */
   XtSetLanguageProc( NULL, NULL, NULL );

#ifdef DT_PERFORMANCE
{ /* Initialize the checkpoint protocol  - Aloke Gupta */
Display *display;
display = XOpenDisplay("");
_DtPerfChkpntInit(display, RootWindow(display, DefaultScreen(display)),
                    argv[0], True);
}
#endif


   _DtEnvControl(DT_ENV_SET);

#ifdef DT_PERFORMANCE
   printf("  XtInitalize\n");
   gettimeofday(&update_time_s, NULL);

   /* Added by Aloke Gupta */
_DtPerfChkpntMsgSend("Begin XtInitialize");
#endif

   /*  Initialize the toolkit and open the display  */
   toplevel = XtInitialize (argv[0], DTFILE_CLASS_NAME,
                            option_list, XtNumber(option_list),
                            (int *)&argc, argv);

/* MERGE START: May not need
#ifdef __osf__
   _XmColorObjCreate ( toplevel, NULL, NULL );
#endif
*/

#ifdef DT_PERFORMANCE
   gettimeofday(&update_time_f, NULL);
   if (update_time_s.tv_usec > update_time_f.tv_usec) {
      update_time_f.tv_usec += 1000000;
      update_time_f.tv_sec--;
   }
   printf("    done XtInitialize, time: %ld.%ld\n\n", update_time_f.tv_sec - update_time_s.tv_sec, update_time_f.tv_usec - update_time_s .tv_usec);

   /* Added by Aloke Gupta */
   _DtPerfChkpntMsgSend("Done  XtInitialize");
#endif

   /* Initialize the function ptr for alphabetic sorting */
   FMStrcoll = GetStrcollProc();

   /* Open the message catalog - DO NOT PERFORM until after XtInitialize! */
   {
      char * foo = ((char *)GETMESSAGE(18, 1, ""));
   }

   /* set application name for later */
   str = strrchr(argv[0], '/');
   if(str != NULL)
      application_name = XtNewString (str + 1);
   else
      application_name = XtNewString (argv[0]);

   n = 0;
   XtSetArg(args[n], XmNallowShellResize, True); n++;
   XtSetArg(args[n], XmNmappedWhenManaged, False); n++;
   XtSetArg(args[n], XmNheight, 1); n++;
   XtSetArg(args[n], XmNwidth, 1); n++;
   XtSetValues(toplevel, args, n);
   XtRealizeWidget(toplevel);
   display = XtDisplay (toplevel);
#ifdef HAVE_EDITRES
    XtAddEventHandler(toplevel, 0, True,
                      (XtEventHandler) _XEditResCheckMessages,
                      (XtPointer) NULL);
#endif

   /* Initialize BMenu button binding */
   {
	int numMouseButtons = XGetPointerMapping(display,
					(unsigned char *)NULL, 0);
	bMenuButton = (numMouseButtons < 3) ? Button2 : Button3;
   }

   /* initialize debugging flag */
#ifdef DEBUG
   if ((tmpStr = getenv("DTFILE_DEBUG")) != NULL)
   {
      debug = atoi(tmpStr);
      if (debug <= 0)
         debug = 1;
   }
   if (getenv("DTFILE_XSYNC") != NULL)
   {
      XSynchronize(display, True);
   }
#endif

   /* initialize performance flag */
#ifdef DT_PERFORMANCE
   if ((tmpStr = getenv("DTFILE_PERFORM")) != NULL)
   {
      perform = atoi(tmpStr);
      if (perform <= 0)
         perform = 1;
   }
   if (getenv("DTFILE_XSYNC") != NULL)
   {
      XSynchronize(display, True);
   }
#endif

   /*  Create the atom set used by save and restore session  */
   save_yourself_atom = XmInternAtom (display, "WM_SAVE_YOURSELF", False);
   wm_state_atom = XmInternAtom (display, "WM_STATE", False);
   command_atom = XA_WM_COMMAND;
   save_mode = XmInternAtom (display, _XA_DT_RESTORE_MODE, False);

   root = RootWindowOfScreen(XtScreen(toplevel));

   /* Set session property on the top level window */
   XmAddWMProtocols(toplevel, &save_yourself_atom, 1);
   XmAddWMProtocolCallback(toplevel, save_yourself_atom, SaveSessionCallback,
                           NULL);

   XSetErrorHandler ((int (*)())ErrorHandler);
   XtAppSetErrorHandler (XtWidgetToApplicationContext(toplevel),
                         ToolkitErrorHandler);

   /* get the keyboard focus policy so we know how we want to set up the */
   /*  Icon gadgets */
   XtSetArg(args[0], XmNkeyboardFocusPolicy, &keybdFocusPolicy);
   XtGetValues(toplevel, args, 1);

   /*  If all of the command line parameters were not processed  */
   /*  out, print out a usage message set and exit.              */

   if (argc != 1) Usage (argv);

   displayWidth = DisplayWidth(display, DefaultScreen(display));
   displayHeight = DisplayHeight(display, DefaultScreen(display));

   /*  Get Dt initialized  */

   if (DtInitialize (display, toplevel, argv[0], FILE_MANAGER_TOOL_CLASS) == False)
   {
      /* Fatal Error: could not connect to the messaging system. */
      /* DtInitialize() has already logged an appropriate error msg */
      exit(-1);
   }

#ifdef DT_PERFORMANCE
   printf("  Setup Converters and get resources\n");
   gettimeofday(&update_time_s, NULL);

   /* Added by Aloke Gupta */
   _DtPerfChkpntMsgSend("Begin Setup Converters");

#endif

   /* First lets add the resource converters needed */
   XtAppAddConverter (XtWidgetToApplicationContext (toplevel),
                       XtRString, "ObjectPlacement",
                       (XtConverter)DtfileCvtStringToObjPlace, NULL, 0);
   XtAppAddConverter (XtWidgetToApplicationContext (toplevel),
                       XtRString, "OpenFolder",
                       (XtConverter)DtfileCvtStringToOpenDir, NULL, 0);
   XtAppAddConverter (XtWidgetToApplicationContext (toplevel),
                       XtRString, "DesktopIcon",
                       (XtConverter)DtfileCvtStringToDTIcon, NULL, 0);
   XtAppAddConverter (XtWidgetToApplicationContext (toplevel),
                       XtRString, "Tree",
                       (XtConverter)DtfileCvtStringToTree, NULL, 0);
   XtAppAddConverter (XtWidgetToApplicationContext (toplevel),
                       XtRString, "TreeFiles",
                       (XtConverter)DtfileCvtStringToTreeFiles, NULL, 0);
   XtAppAddConverter (XtWidgetToApplicationContext (toplevel),
                       XtRString, "View",
                       (XtConverter)DtfileCvtStringToView, NULL, 0);
   XtAppAddConverter (XtWidgetToApplicationContext (toplevel),
                       XtRString, "Order",
                       (XtConverter)DtfileCvtStringToOrder, NULL, 0);
   XtAppAddConverter (XtWidgetToApplicationContext (toplevel),
                       XtRString, "Direction",
                       (XtConverter)DtfileCvtStringToDirection, NULL, 0);
   XtAppAddConverter (XtWidgetToApplicationContext (toplevel),
                       XtRString, "Grid",
                       (XtConverter)DtfileCvtStringToGrid, NULL, 0);

   /*  Get the application defined resources of session and  */
   /*  directory, and get the processes host.                */

   XtGetApplicationResources(toplevel, &application_args,
                             resources, XtNumber(resources), NULL,0);

#ifdef DT_PERFORMANCE
   gettimeofday(&update_time_f, NULL);
   if (update_time_s.tv_usec > update_time_f.tv_usec) {
      update_time_f.tv_usec += 1000000;
      update_time_f.tv_sec--;
   }
   printf("    done Setting up Converters and got resources, time: %ld.%ld\n\n", update_time_f.tv_sec - update_time_s.tv_sec, update_time_f.tv_usec - update_time_s .tv_usec);

   /* Added by Aloke Gupta */
   _DtPerfChkpntMsgSend("Done  Setup Converters");
#endif

#ifdef DT_PERFORMANCE
   printf("  DtDbLoad\n");
   gettimeofday(&update_time_s, NULL);
   /* Added by Aloke Gupta */
   _DtPerfChkpntMsgSend("Begin DtDbLoad");

#endif

   /*  Set up the messaging and file types  */

   DtDbLoad();
#ifdef DT_PERFORMANCE
   gettimeofday(&update_time_f, NULL);
   if (update_time_s.tv_usec > update_time_f.tv_usec) {
      update_time_f.tv_usec += 1000000;
      update_time_f.tv_sec--;
   }
   printf("    done DtDbLoad, time: %ld.%ld\n\n", update_time_f.tv_sec - update_time_s.tv_sec, update_time_f.tv_usec - update_time_s .tv_usec);

   /* Added by Aloke Gupta */
   _DtPerfChkpntMsgSend("Done  DtDbLoad");

#endif


   /* Create some global strings */
   db = XtDatabase(display);
   if (XrmGetResource (db, "dttypes.defaultActions",
                       "Dttypes.DefaultActions", &rep_type,
                       &resource_value))
   {
     string = XtNewString(resource_value.addr);
     for(str = DtStrtok(string, ",") ,i = 0; str != NULL && i < 2 ;
     str = DtStrtok(NULL, ","), i++) {
       if(i == 0)
         openInPlace = XtNewString(str);
       else
         openNewView = XtNewString(str);
     }
     XtFree(string);
   }
   if(openInPlace == NULL || strlen(openInPlace) == 0)
       openInPlace = XtNewString("OpenInPlace");
   if(openNewView == NULL || strlen(openNewView) == 0)
       openNewView = XtNewString("OpenNewView");

   DtGetShortHostname (home_host_name, MAX_PATH);

   /*  Get the lock established to ensure only one dtfile process  */
   /*  is running.                                                  */
   if (_DtGetLock (display, DTFILE_CLASS_NAME) == 0)
   {
      status = InitializeToolTalkProcid( &ttFd, toplevel, False );
      if (TT_OK != status)
      {
          char *errfmt, *errmsg, *title, *statmsg;
          title = GETMESSAGE(21,38,"File Manager Error");
          errfmt = GETMESSAGE(18, 40,
		   "Could not connect to ToolTalk:\n%s\nExiting ...");
          statmsg = tt_status_message(status);

          errmsg = XtMalloc(strlen(errfmt) + strlen(statmsg) + 2);
          fprintf(stderr, errfmt, statmsg);
          sprintf(errmsg, errfmt, statmsg);

          /* put up error dialog and loop,
	   * application will exit in dialog callback
	   */
          post_dialog(toplevel, title, errmsg, DtErrExitCB); 
      }

      if (application_args.session != NULL)
      {
	 msg = tttk_message_create( 0, TT_REQUEST, TT_SESSION, 0,
				    "DtFileSession_Run",
				    (Tt_message_callback)ExitApp );
	 tt_message_file_set( msg, application_args.session );
	 tt_message_send( msg );
      }
      else if (application_args.directories != NULL)
      {
	 msg = tttk_message_create( 0, TT_REQUEST, TT_SESSION, 0,
				    "DtFolder_Show",
				    (Tt_message_callback)ExitApp );
	 tt_message_file_set( msg, application_args.directories );
         SetupSendRequestArgs( application_args, msg );
	 tt_message_send( msg );
      }
      else
      {
         /* Default action: Open up pwd or home dir */
         GetPWD(current_directory);

         if (current_directory[0] != NULL)
         {
	    msg = tttk_message_create( 0, TT_REQUEST, TT_SESSION, 0,
				       "DtFolder_Show",
				       (Tt_message_callback)ExitApp );
	    tt_message_file_set( msg, current_directory );
         }
         else
         {
	    msg = tttk_message_create( 0, TT_REQUEST, TT_SESSION, 0,
				       "DtHome_Show",
				       (Tt_message_callback)ExitApp );
         }
         SetupSendRequestArgs( application_args, msg );
	 tt_message_send( msg );
      }
      WaitForResponseAndExit();
   }

   /*  Initialize the encapsulation mechanism and install the dialogs  */
   /*  used by the file manager.                                       */

   _DtInitializeEncapsulation (display, argv[0], DTFILE_CLASS_NAME);
   topPositionOffset = -8;

   status = InitializeToolTalkProcid( &ttFd, toplevel, True );
   if (TT_OK != status)
   {
       char *errfmt, *errmsg, *title, *statmsg;
       title = GETMESSAGE(21,38,"File Manager Error");
       errfmt = GETMESSAGE(18, 40,
		"Could not connect to ToolTalk:\n%s\nExiting ...");
       statmsg = tt_status_message(status);

       errmsg = XtMalloc(strlen(errfmt) + strlen(statmsg) + 2);
       fprintf(stderr, errfmt, statmsg);
       sprintf(errmsg, errfmt, statmsg);

       /* put up error dialog and loop,
	* application will exit in dialog callback
	*/
       post_dialog(toplevel, title, errmsg, DtErrExitCB); 
   }


#ifdef DT_PERFORMANCE
   printf("  Setup Callbacks (messaging)\n");
   gettimeofday(&update_time_s, NULL);

   /* Added by Aloke Gupta */
   _DtPerfChkpntMsgSend("Begin Setup Callbacks");

#endif
   events2Watch = tt_pattern_create();
   tt_pattern_category_set( events2Watch, TT_OBSERVE );
   tt_pattern_class_add( events2Watch, TT_NOTICE );
   tt_pattern_scope_add( events2Watch, TT_SESSION );
   sessId = tt_default_session();
   tt_pattern_session_add( events2Watch, sessId );
   tt_free( sessId );
   tt_pattern_op_add( events2Watch, "DtTypes_Reloaded" );
   tt_pattern_op_add( events2Watch, "XSession_Ending" );
   tt_pattern_callback_add( events2Watch, ObserveTtNotice );
   tt_pattern_register( events2Watch );

   requests2Handle = tt_pattern_create();
   tt_pattern_category_set( requests2Handle, TT_HANDLE );
   tt_pattern_class_add( requests2Handle, TT_REQUEST );
   tt_pattern_scope_add( requests2Handle, TT_SESSION );
   sessId = tt_default_session();
   tt_pattern_session_add( requests2Handle, sessId );
   tt_free( sessId );
   tt_pattern_op_add( requests2Handle, "DtFileSession_Run" );
   tt_pattern_op_add( requests2Handle, "DtFolder_Show" );
   tt_pattern_op_add( requests2Handle, "DtHome_Show" );
   tt_pattern_op_add( requests2Handle, "DtTools_Show" );
   tt_pattern_op_add( requests2Handle, "DtTrash_Show" );
   tt_pattern_op_add( requests2Handle, "DtTrash_Remove" );
   tt_pattern_op_add( requests2Handle, "DtTrash_Empty" );
   tt_pattern_op_add( requests2Handle, "DtTrash_File" );
   tt_pattern_op_add( requests2Handle, "DtTrash_Restore" );
   tt_pattern_op_add( requests2Handle, "DtFile_PutOnWorkspace" );
   tt_pattern_op_add( requests2Handle, "DtFile_Move" );
   tt_pattern_op_add( requests2Handle, "DtFile_Copy" );
   tt_pattern_op_add( requests2Handle, "DtFile_Link" );
   tt_pattern_callback_add( requests2Handle, HandleTtRequest );
   tt_pattern_register( requests2Handle );

   /* Setup the settings file if any to setup */

   _DtWsmAddMarqueeSelectionCallback(toplevel,
                                     (DtWsmMarqueeSelectionProc) MarqueeSelect,
                                     NULL);

#ifdef DT_PERFORMANCE
   gettimeofday(&update_time_f, NULL);
   if (update_time_s.tv_usec > update_time_f.tv_usec) {
      update_time_f.tv_usec += 1000000;
      update_time_f.tv_sec--;
   }
   printf("    done Setting Callbacks (messaging), time: %ld.%ld\n\n", update_time_f.tv_sec - update_time_s.tv_sec, update_time_f.tv_usec - update_time_s .tv_usec);

    /* Added by Aloke Gupta */
   _DtPerfChkpntMsgSend("Done  Setup Callbacks");

#endif

   smallIconWidth = application_args.smallIconWidth;
   smallIconHeight = application_args.smallIconHeight;
   largeIconWidth = application_args.largeIconWidth;
   largeIconHeight = application_args.largeIconHeight;
   user_font = application_args.user_font;
   dragThreshold = application_args.dragThreshold;
   rereadTime = application_args.rereadTime;
   checkBrokenLink = application_args.checkBrokenLink;
   maxDirectoryProcesses = application_args.maxDirectoryProcesses;
   maxRereadProcesses = application_args.maxRereadProcesses;
   maxRereadProcsPerTick = application_args.maxRereadProcsPerTick;
   trashWait = application_args.trashWait;
   showFilesystem = application_args.showFilesystem;
   showDropZone = application_args.showDropZone;
   showEmptySet = application_args.showEmptySet;
   showEmptyMsg = application_args.showEmptyMsg;
   restrictMode = application_args.restrictMode;
   openDirType = application_args.openDirType;
   desktopIconType = application_args.desktopIconType;
   desktopPlacement = application_args.desktopPlacement;
   freezeOnConfig = application_args.freezeOnConfig;
   emptyTrashOnExit = application_args.emptyTrashOnExit;
#if defined(__hpux) || defined(sun)
   follow_links = application_args.follow_links;
#endif
   instanceWidth = application_args.instanceIconWidth;
   instanceHeight = application_args.instanceIconHeight;
   fileMgrIcon = application_args.fileMgrIcon;
   appMgrIcon = application_args.appMgrIcon;
   trashIcon = application_args.trashIcon;
   retryLoadDesktopInfo = application_args.retryLoadDesktop;
   if( application_args.directories != NULL
       || (strcmp (application_args.restricted, RESTRICTED_HEADER) == 0)
       || restrictMode )
   {
      special_view = True;
      if(strcmp (application_args.restricted, RESTRICTED_HEADER) == 0)
      {
         if(application_args.directories != NULL)
            special_restricted = XtNewString(application_args.directories);
         else
         {
            /* Get users pwd so we can set the restricted dir to it */
            GetPWD(current_directory);

            if (current_directory[0] != NULL)
               special_restricted = XtNewString(current_directory);
            else
               special_restricted = XtNewString("~");
         }
      }
      else if( restrictMode )
         special_restricted = XtNewString("~");
      else
         special_restricted = NULL;
      special_treeType = treeType = application_args.tree_view;
      special_treeFiles = treeFiles = application_args.tree_files;
      special_viewType = viewType = application_args.view;
      special_orderType = orderType = application_args.order;
      special_directionType = directionType = application_args.direction;
      special_randomType = randomType = application_args.grid;
      special_title = XtNewString(application_args.title);
      if(application_args.help_volume == NULL)
      {
         special_helpVol = XtNewString(DTFILE_HELP_NAME);
         fileMgrHelpVol = XtNewString(DTFILE_HELP_NAME);
      }
      else
      {
         special_helpVol = XtNewString(application_args.help_volume);
         fileMgrHelpVol = XtNewString(application_args.help_volume);
      }
      fileMgrTitle = application_args.title;
   }
   else
   {
      special_view = False;
      treeType = application_args.tree_view;
      treeFiles = application_args.tree_files;
      viewType = application_args.view;
      orderType = application_args.order;
      directionType = application_args.direction;
      randomType = application_args.grid;
      fileMgrTitle = application_args.title;
      if(application_args.help_volume == NULL)
         fileMgrHelpVol = XtNewString(DTFILE_HELP_NAME);
      else
         fileMgrHelpVol = XtNewString(application_args.help_volume);
   }


   if(desktopIconType == LARGE)
   {
      numRows = displayHeight / PIXELS_PER_ROW_LARGE;
      numColumns = displayWidth / PIXELS_PER_COLUMN_LARGE;
   }
   else /* small */
   {
      numRows = displayHeight / PIXELS_PER_ROW_SMALL;
      numColumns = displayWidth / PIXELS_PER_COLUMN_SMALL;
   }

#ifdef SHAPE
   /* determine whether the Server has the shape extension */
   if(XShapeQueryExtension(display, &base1, &base2) == True)
      shapeExtension = True;
   else
      shapeExtension = False;
#endif

   /* get the name for the root directory */
   root_title = (char *)XtMalloc(strlen(application_args.root_title) + 1);
   strcpy(root_title, application_args.root_title);

   if ((homeDir = getenv("HOME")) == NULL || strlen (homeDir) == 0)
   {
      pwInfo = getpwuid (getuid());
      homeDir = pwInfo->pw_dir;
   }
   strncpy(users_home_dir, homeDir, MAX_PATH - 1);
   if(restrictMode)
   {
      if(application_args.directories != NULL &&
                           strncmp(application_args.directories, "~", 1) != 0 )
      {
         if(strncmp(application_args.directories, users_home_dir,
                                                  strlen(users_home_dir)) != 0)
            RestrictModeUsage (argv);
      }
      else if (application_args.directories == NULL)
          application_args.directories = XtNewString("~");
   }
   if (strcmp(users_home_dir, "/") != 0)
      strcat(users_home_dir, "/");

   XtAppAddActions(XtWidgetToApplicationContext (toplevel), actionTable, 2);

   /* Create some global Xm strings for our dialog buttons */

   okXmString = XmStringCreateLocalized((char*)_DtOkString);
   cancelXmString = XmStringCreateLocalized((char*)_DtCancelString);
   helpXmString = XmStringCreateLocalized((char*)_DtHelpString);
   applyXmString = XmStringCreateLocalized((char*)_DtApplyString);
   closeXmString = XmStringCreateLocalized((char*)_DtCloseString);

   /*  Get the dt path created and initialized  */

   dt_path = _DtCreateDtDirs (display);

   if (dt_path == NULL)
   {
      Widget dialog;
      char *msg, *title;
      title = XtNewString((GETMESSAGE(21,38,"File Manager Error")));
      tmpStr = GETMESSAGE(18, 2, "Could not create the ~/.dt folder or sub folders.");
      msg = XtNewString(tmpStr);

      /* put up error dialog and loop, application will exit in 
         dialog callback */

      dialog = post_dialog(toplevel, title, msg, DtErrExitCB); 
      XtFree(msg);
      XtFree(title);

      while (TRUE)
      {
	XtAppProcessEvent(XtWidgetToApplicationContext(dialog), XtIMAll );
      }
    }

   /*  Set the black and white pixel globals.  */

   black_pixel = BlackPixelOfScreen (XtScreen (toplevel));
   white_pixel = WhitePixelOfScreen (XtScreen (toplevel));

   /*
    * Take over the drawing area's redisplay functions, so that we can get
    * the dtfile views to redraw according to stacking order, using our own
    * redraw function.
    */
   xmDrawingAreaWidgetClass->core_class.expose = DrawingAreaRedisplay;


   /* Get the root user id */

   if ((pw = getpwnam("root")) == NULL)
      root_user = 0;          /* Assume root is uid 0 */
   else
      root_user = pw->pw_uid;


   file_mgr_dialog = _DtInstallDialog (fileMgrClass, True, True);
   change_dir_dialog = _DtInstallDialog (changeDirClass, True, True);
   preferences_dialog = _DtInstallDialog (preferencesClass, True, True);
   filter_dialog = _DtInstallDialog (filterClass, True, True);
   find_dialog = _DtInstallDialog (findClass, True, True);
   mod_attr_dialog = _DtInstallDialog (modAttrClass, True, True);
   help_dialog = _DtInstallDialog (helpClass, False, False);

   if(special_view == True && special_restricted != NULL);
      if(strncmp(special_restricted, "~", 1) == 0)
      {
         char *ptr, *ptr1;

         special_restricted = _DtChangeTildeToHome(special_restricted);
         ptr = strrchr(special_restricted, '/');
         ptr1 = ptr + 1;
         if(ptr1[0] == NULL)
            *ptr = '\0';
      }

   /* Setup the settings file if any to setup */
   RestoreSettingsFile();

#ifdef DT_PERFORMANCE
   printf("  TrashCreateDialog\n");
   gettimeofday(&update_time_s, NULL);

   /* Added by Aloke Gupta */
   _DtPerfChkpntMsgSend("Begin TrashCreateDialog");

#endif
   if( InitializeTrash( application_args.prompt_user ) )

#ifdef DEFER_TRASH_CREATION
;
#else
     TrashCreateDialog (display);
#endif

#ifdef DT_PERFORMANCE
   gettimeofday(&update_time_f, NULL);
   if (update_time_s.tv_usec > update_time_f.tv_usec) {
      update_time_f.tv_usec += 1000000;
      update_time_f.tv_sec--;
   }
   printf("    done TrashCreateDialog, time: %ld.%ld\n\n", update_time_f.tv_sec - update_time_s.tv_sec, update_time_f.tv_usec - update_time_s .tv_usec);

   /* Added by Aloke Gupta */
   _DtPerfChkpntMsgSend("Done  TrashCreateDialog");
#endif

   /* lets go empty the trash if there is any */
/*
   TrashEmpty();
*/

#ifdef DT_PERFORMANCE
   printf("  Setup Desktop\n");
   gettimeofday(&update_time_s, NULL);
   /* Added by Aloke Gupta */
   _DtPerfChkpntMsgSend("Begin Setup Desktop");

#endif
   /* go build 10 desktop windows */
   desktop_data = NULL;
   InitializeDesktopWindows(10, display);
   InitializeDesktopGrid();

   LoadDesktopInfo(application_args.session);

   /* Install WorkSpaceRemoved handler.
      This handler will be called when a workspace is being removed
      so File Manager can go through its desktop icons and clean up.
   */
   DtWsmAddWorkspaceModifiedCallback( toplevel, WorkSpaceRemoved, NULL );

#ifdef DT_PERFORMANCE
   gettimeofday(&update_time_f, NULL);
   if (update_time_s.tv_usec > update_time_f.tv_usec) {
      update_time_f.tv_usec += 1000000;
      update_time_f.tv_sec--;
   }
   printf("    done Setting up Desktop, time: %ld.%ld\n\n", update_time_f.tv_sec - update_time_s.tv_sec, update_time_f.tv_usec - update_time_s .tv_usec);

   /* Added by Aloke Gupta */
   _DtPerfChkpntMsgSend("Done  Setup Desktop");
#endif


   /*  Process the application resources to restore a session, dt  */
   /*  a directory set or display the users home directory.         */

#ifdef DT_PERFORMANCE
   printf("  Bring up View\n");
   gettimeofday(&update_time_s, NULL);

   /* Added by Aloke Gupta */
   _DtPerfChkpntMsgSend("Begin Bring up View");

#endif

   if (strcmp (application_args.no_view, "-noview") != 0)
   {
     if (application_args.session != NULL)
        session_flag =
          RestoreSession (application_args.session, NORMAL_RESTORE, NULL);
     else if (application_args.directories != NULL)
     {
        restoreType = CURRENT_DIR_RESTORE;
        OpenDirectories (application_args.directories, NULL);
     }
   }

#ifdef DT_PERFORMANCE
   gettimeofday(&update_time_f, NULL);
   if (update_time_s.tv_usec > update_time_f.tv_usec) {
      update_time_f.tv_usec += 1000000;
      update_time_f.tv_sec--;
   }
   printf("    done Bringing up view, time: %ld.%ld\n\n", update_time_f.tv_sec - update_time_s.tv_sec, update_time_f.tv_usec - update_time_s .tv_usec);  fflush(stdout);

   /* Added by Aloke Gupta */
    _DtPerfChkpntMsgSend("Done  Bring up View");
#endif


   /*  If no views were opened upon invocation, display the  */
   /*  current directory.                                    */

   if ( (strcmp (application_args.no_view, "-noview") != 0) &&
        ((application_args.session == NULL) ||
         (session_flag != 0)) )
   {
      if (view_count == 0)
      {
         /* Get users pwd so we can create a fileviewer window of it */
         GetPWD(current_directory);

         if (current_directory[0] != NULL)
         {
           if (!GetNewView (home_host_name, current_directory, NULL, NULL, 0))
              ViewHomeDirectoryHandler (0);
         }
         else
         {
           ViewHomeDirectoryHandler (0);
         }
      }
   }


   /*  Set up the timer based directory reading.      */
   InitializeDirectoryRead (toplevel);

   /* Process and dispatch incoming events */
   eventDebugging = getenv("EVENT_DEBUGGING") != NULL;

#ifdef DT_PERFORMANCE
   printf("  InitializeToolTalkSession\n");
   gettimeofday(&update_time_s, NULL);
#endif
   (void) InitializeToolTalkSession( toplevel, ttFd );
#ifdef DT_PERFORMANCE
   gettimeofday(&update_time_f, NULL);
   if (update_time_s.tv_usec > update_time_f.tv_usec) {
      update_time_f.tv_usec += 1000000;
      update_time_f.tv_sec--;
   }
   printf("    done InitializeToolTalkSession, time: %ld.%ld\n\n", update_time_f.tv_sec - update_time_s.tv_sec, update_time_f.tv_usec - update_time_s .tv_usec);
#endif

 #ifdef DT_PERFORMANCE
   gettimeofday(&update_time_fs, NULL);
   if (update_time_ss.tv_usec > update_time_fs.tv_usec) {
      update_time_fs.tv_usec += 1000000;
      update_time_fs.tv_sec--;
   }
   printf("    Finish, time: %ld.%ld\n\n", update_time_fs.tv_sec - update_time_ss.tv_sec, update_time_fs.tv_usec - update_time_ss .tv_usec);
#endif

   while (1)
   {
      XtNextEvent(&event);

      if (event.type != 0)
         XtDispatchEvent(&event);
   }
}



/************************************************************************
 *
 *  ErrorHandler
 *
 ************************************************************************/

static void
ErrorHandler(
        Display *disp,
        XErrorEvent *event )
{
  #define _DTFILE_BUFSIZE 1024
  char errmsg[_DTFILE_BUFSIZE];

  _DtPrintDefaultErrorSafe (disp, event, errmsg, _DTFILE_BUFSIZE);
  _DtSimpleError(application_name, DtWarning, NULL, errmsg, NULL);
  xErrorDetected = True;

  /* We do not want to exit here lets try to continue... */

}

static void
ToolkitErrorHandler(
        char *message )
{
    char * msg;
    char * tmpStr;

    tmpStr = GETMESSAGE(18, 6, "An X Toolkit error occurred... Exiting.\n");
    msg = (char *)malloc(strlen(tmpStr) + strlen(message) + 3);
    strcpy(msg, message);
    strcat(msg, "\n");
    strcat(msg, tmpStr);
    _DtSimpleError (application_name, DtError, NULL, msg);
    printf ("%s",msg);
    XtFree(msg);
    FinalizeToolTalkSession( );
    exit (1);
}




/************************************************************************
 *
 *  Usage
 *      When incorrect parameters have been specified on the command
 *      line, print out a set of messages detailing the correct use
 *      and exit.
 *
 ************************************************************************/

static void
Usage(
        char **argv )
{
   char * template;
   char * message_string1 = "\nUsage: %s...\n\n"
                         "   -noview\n\n\t"
                         "Dtfile runs in server mode.\n\n"
                         "   -session SessionFile\n\n\t"
                         "Dtfile runs with the session file specified in the SessionFile\n\t"
                         "parameter.\n\n"
                         "   -folder Folder[,Folder,Folder]\n"
                         "   -dir Folder[,Folder,Folder]\n\n\t"
                         "Dtfile displays a window for each folder specified in the\n\t"
                         "Folder parameter.  The Folder parameter may contain many\n\t"
                         "folders separated by commas.  Folders may be in the form\n\t"
                         "'path'.\n\n"
                         "   -title Title\n\n\t"
                         "Dtfile uses the string specified in the Title parameter as the\n\t"
                         "title for its windows.\n\n"
                         "   -help_volume HelpVolume\n\n\t"
                         "Dtfile uses the help volume specified in the HelpVolume parameter.\n\n"
                         "   -restricted\n\n\t"
                         "Dtfile will not display folders above the restricted folder.\n\t"
                         "If the -dir option is used, the folder specified in that option\n\t"
                         "is the restricted folder.  If the -dir option is not used, the\n\t"
                         "user's current folder is the restricted folder.\n\n"
                         "   -grid on/off\n\n\t"
                         "on     = Files are displayed in a grid pattern.\n\t"
                         "off    = Files are displayed as placed.\n\n"
                         "   -tree on/off\n\n\t"
                         "on     = Files are displayed in single folder mode.\n";

   char * message_string2 = "\toff    = Files are displayed in folder tree mode.\n\n"
                         "   -tree_files never/always/choose\n\n\t"
                         "never  = Tree mode has two states: partially expanded or collapsed.\n\t"
                         "always = Tree mode has two states: fully expanded or collapsed.\n\t"
                         "choose = Tree mode has three states: partially expanded, fully\n\t"
                         "         expanded, or collapsed.\n\n"
                         "   -order alphabetical/file_type/date/size\n\n\t"
                         "Files are displayed in the specified order: alphabetical, by file\n\t"
                         "type, by date, or by size.\n\n"
                         "   -view no_icon/large_icon/small_icon/attributes\n\n\t"
                         "Files are displayed in the specified format: text only, text and\n\t"
                         "large icons, text and small icons, with attributes.\n\n"
                         "   -direction ascending/descending\n\n\t"
                         "Files are displayed in the specified direction: ascending or\n\t"
                         "descending.\n\n"
                         "   -large_icon_width  <size>\n\n"
                         "   -large_icon_height <size>\n\n"
                         "   -small_icon_width  <size>\n\n"
                         "   -small_icon_height <size>\n\n"
                         "        The display area size for the icon images in File Manager\n"
                         "        Icon images larger than this size will be clipped to this size\n"
                         "        The default display area size for large is 38 and small is 24\n\n"
                         "\n"
;

   template = (GETMESSAGE(18,23, message_string1));
   fprintf (stderr, template, argv[0]);
   template = (GETMESSAGE(18,24, message_string2));
   fprintf (stderr, template);

   exit (0);
}

static void
RestrictModeUsage(
        char **argv )
{
   char * template;
   char * message_string = "\nRestricted Mode Usage: %s...\n\n"
                         "   -folder Folder[,Folder,Folder]\n"
                         "   -dir Folder[,Folder,Folder]\n\n\t"
                         "Where Folder is a Folder below and/or including\n\t"
                         "the user's Home Folder.\n\n";

   template = (GETMESSAGE(18,26, message_string));

   fprintf (stderr, template, argv[0]);

   FinalizeToolTalkSession( );
   exit (0);
}




/************************************************************************
 *
 *  Stop
 *      Catches Ctrl C's and exits.
 *
 ************************************************************************/

static void
Stop( void )
{
  FinalizeToolTalkSession( );
  exit(0);
}







/***********************************************************************
 *
 * RestoreSettingsFile
 *         Used to restore the save settings files from
 *         either $HOME/.dt/$DISPLAY/current or $HOME/.dt/$DISPLAY/home.
 *
 ***********************************************************************/
static void
RestoreSettingsFile( void )
{
   int             status, fd;
   char            *dtPath=NULL;
   char            *homeSavePath=NULL;
   char            *homeHomePath=NULL;
   char            *toolSavePath=NULL;
   char            *toolHomePath=NULL;
   char            *dirName=NULL;
   Atom            actualType;
   int             actualFormat;
   unsigned long   nitems;
   unsigned long   leftover;
   unsigned char   *data = NULL;


  /* go get the dt path */
  /* _DtCreateDtDirs returs a path of MaxPath Length */
   dtPath = (char *)_DtCreateDtDirs(display);

  /* Determin which type of session we are running HOME or CURRENT */

  /* get the root window property of SaveMode */
  XGetWindowProperty(display, RootWindow(display,0),
                         save_mode, 0L, (long)BUFSIZ,False,
                         XA_STRING, &actualType,
                         &actualFormat,&nitems,&leftover,
                         &data);

  if(strcmp((char *)data, "home") == 0)
    dirName = XtNewString("home");
  else if(strcmp((char *)data, "current") == 0)
    dirName = XtNewString("current");
  else
  {
    XtFree(dtPath);
    return;  /* we are comming up in a system mode (e.g. default) */
  }


  /* Build the paths to read the files from */

  toolSavePath = (char *)XtMalloc(strlen(dtPath) +
                  strlen(TOOL_SETTINGS_FILENAME) +
                  strlen(dirName) + 4);

  homeSavePath = (char *)XtMalloc(strlen(dtPath) +
                  strlen(HOME_SETTINGS_FILENAME) +
                  strlen(dirName) + 4);

  sprintf( homeSavePath, "%s/%s/%s", dtPath, dirName, HOME_SETTINGS_FILENAME );
  sprintf( toolSavePath, "%s/%s/%s", dtPath, dirName, TOOL_SETTINGS_FILENAME );

  /* open the home settings file to see if its there */
  if((fd = open(homeSavePath, O_RDONLY)) != -1)
  {
      close(fd);
     /* create where the saved file is going to go */

      homeHomePath = (char *)XtMalloc(strlen(dtPath) +
                     strlen(HOME_SETTINGS_FILENAME) + 2);

      sprintf( homeHomePath, "%s/%s", dtPath, HOME_SETTINGS_FILENAME );

     /* remove any existing dtfile.home (e.g. HOME_SETTINGS_FILENAME) */
      unlink(homeHomePath);

     /* copy the saved one into $HOME/.dt/$DISPLAY ... this is the one that
      * will be used by the dtfile
      */
      status = link(homeSavePath, homeHomePath);
      if(status != 0)
      {
        char *tmpStr, *msg;

        tmpStr = GETMESSAGE(18,27, "Unable to recover the saved default home settings file, will use default.\n");
        msg = XtNewString(tmpStr);
        _DtSimpleErrnoError(application_name, DtWarning, NULL, msg, NULL);
        XtFree(msg);
      }

   }

 /* open the tool settings file to see if its there */
  if((fd = open(toolSavePath, O_RDONLY)) != -1)
  {
      close(fd);
     /* create where the saved file is going to go */
      toolHomePath = (char *)XtMalloc(strlen(dtPath) +
                     strlen(TOOL_SETTINGS_FILENAME) + 2);

      sprintf( toolHomePath, "%s/%s", dtPath, TOOL_SETTINGS_FILENAME );

     /* remove any existing dtfile.tool (e.g. TOOL_SETTINGS_FILENAME) */
      unlink(toolHomePath);

     /* copy the saved one into $HOME/.dt/$DISPLAY ... this is the one that
      * will be used by the dtfile
      */
      status = link(toolSavePath, toolHomePath);
      if(status != 0)
      {
        char *tmpStr, *msg;

        tmpStr = GETMESSAGE(18,28, "Unable to recover the saved default tool settings file, will use default.\n");
        msg = XtNewString(tmpStr);
        _DtSimpleErrnoError(application_name, DtWarning, NULL, msg, NULL);
        XtFree(msg);
      }
   }


  /* free up space */
   XFree(data);
   XtFree(dtPath);
   XtFree(dirName);
   XtFree(homeSavePath);
   XtFree(toolSavePath);
   XtFree(toolHomePath);
   XtFree(homeHomePath);


}





/***********************************************************************
 *
 * MoveDefaultSettings
 *         Used to save the dtfile.tool and dtfile.home settings files to
 *         either $HOME/.dt/$DISPLAY/current or $HOME/.dt/$DISPLAY/home.
 *         The parameter mode determines whether it is home or
 *         current.
 *
 ***********************************************************************/
static void
MoveDefaultSettings(
        int mode )
{
   int status;
   char *toolSavePath=NULL;
   char *homeSavePath=NULL;
   char *dirName=NULL;
   char *toolMovePath=NULL;
   char *homeMovePath=NULL;
   char *dtPath;

   /* determine whether home or current */
   if(mode == HOME_DIR_RESTORE)
     dirName = XtNewString("home");
   else
     dirName = XtNewString("current");

  /* go get the dt path */
  /* _DtCreateDtDirs returs a path of MaxPath Length */
  dtPath = (char *)_DtCreateDtDirs(display);


  /* Build the paths to save the files to */

  toolSavePath = (char *)XtMalloc(strlen(dtPath) +
                  strlen(TOOL_SETTINGS_FILENAME) +
                  strlen(dirName) + 4);

  homeSavePath = (char *)XtMalloc(strlen(dtPath) +
                  strlen(HOME_SETTINGS_FILENAME) +
                  strlen(dirName) + 4);


   /* create the directory and filename of where its going to be saved */
  sprintf( homeSavePath, "%s/%s/%s", dtPath, dirName, HOME_SETTINGS_FILENAME );
  sprintf( toolSavePath, "%s/%s/%s", dtPath, dirName, TOOL_SETTINGS_FILENAME );

   /* Setup the paths used to GET the old files */

   toolMovePath= (char *)XtMalloc(strlen(dtPath) +
                   strlen(TOOL_SETTINGS_FILENAME) + 3);

   homeMovePath= (char *)XtMalloc(strlen(dtPath) +
                   strlen(HOME_SETTINGS_FILENAME) + 3);

   /* create the filename of where its going to be saved from */

   /* Tool File location */
   sprintf( toolMovePath, "%s/%s", dtPath, TOOL_SETTINGS_FILENAME );

   /* Home File location */
   sprintf( homeMovePath, "%s/%s", dtPath, HOME_SETTINGS_FILENAME );


  /* get rid of the tool settings file that is already in home or current */
   status = unlink(toolSavePath);

  /* get rid of the home settings file that is already in home or current */
   status = unlink(homeSavePath);


  /* now save tool settings file in home or current determined by savePath */
   status = link(toolMovePath, toolSavePath);

  /* now save home settings file in home or current determined by savePath */
   status = link(homeMovePath, homeSavePath);

   /* free up space */
   XtFree(homeMovePath);
   XtFree(toolMovePath);
   XtFree(homeSavePath);
   XtFree(toolSavePath);
   XtFree(dirName);
   XtFree(dtPath);
}


/************************************************************************
 *
 *  SaveSettingsCB
 *      Callback for the Save Settings menupick.
 *
 ************************************************************************/
void
SaveSettingsCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
   Widget dlog;
   Widget mbar;
   Arg args[1];
   char * message;
   char * title;
   FileMgrRec * file_mgr_rec;
   DialogData * dialog_data;
   FileMgrData * file_mgr_data;
   char * tmpStr;

   /* Strip the file_mgr_rec from the current widget
   * and attach it to the ok callback button
   */


   /* Get the file_mgr_rec hanging off the menubar */
   mbar = XmGetPostedFromWidget(XtParent(w));
   XmUpdateDisplay (w);
   XtSetArg(args[0], XmNuserData, &file_mgr_rec);
   XtGetValues(mbar, args, 1);


   /* Ignore accelerators when we're insensitive */
   if ((file_mgr_rec->menuStates & SETTINGS) == 0)
   {
     XSetInputFocus(XtDisplay(w),
                    XtWindow(file_mgr_rec->defaultEnvBtn_child),
                    RevertToParent, CurrentTime);
     return;
   }

   /* Desensatize the save settings menu pick here */
   file_mgr_rec->menuStates &= ~SETTINGS;


   /* Get the file_mgr_rec dialog data info */
   if ((dialog_data = _DtGetInstanceData ((XtPointer)file_mgr_rec)) == NULL)
     return;
   file_mgr_data = (FileMgrData *) dialog_data->data;


   /* Based on the path we must determine if we are saving a Tools or
    * HomeDir dtfile view.
    */
   if (file_mgr_data->restricted_directory != NULL && file_mgr_data->toolbox)
     file_mgr_data->restoreKind = TOOL_RESTORE;
   else
     file_mgr_data->restoreKind = HOME_RESTORE;


   /* Setup and call the _DtMessageDialog procedure to post the dialog */

   if (file_mgr_data->restoreKind == HOME_RESTORE)
   {
     tmpStr = (GETMESSAGE(18,29, "Save the current File Manager View window size,\npreferences, and filter settings?\n\nThey will be used for any new File Manager View windows\nstarted from the Front Panel."));
   }
   else
   {
     tmpStr = (GETMESSAGE(18,35, "Save the current Application Manager\nView window size, preferences, and filter settings?\n\nThey will be used for any new Application Manager\nwindows started from the Front Panel."));
   }
   message = XtNewString(tmpStr);

   if(file_mgr_data->title != NULL &&
               strcmp(file_mgr_data->helpVol, DTFILE_HELP_NAME) != 0)
   {
      tmpStr = GETMESSAGE(18, 16, "Save As Default Options");
      title = (char *)XtMalloc(strlen(tmpStr) +
                                 strlen(file_mgr_data->title) + 5);
      sprintf(title, "%s - %s", file_mgr_data->title, tmpStr);
   }
   else
   {
      tmpStr = GETMESSAGE(18, 32, "File Manager - Save As Default Options");
      title = XtNewString(tmpStr);
   }
   dlog = (Widget)_DtMessageDialog(mbar, title, message, NULL, TRUE,
                                   SaveDefaultCancelCB, SaveDefaultOkCB, NULL,
                                   HelpRequestCB, False, QUESTION_DIALOG);
   file_mgr_rec->defaultEnvBtn_child=dlog;
   XtFree (message);
   XtFree (title);


   /* Add array as userdata on the dialog. */

   XtSetArg(args[0], XmNuserData, file_mgr_rec);
   XtSetValues(dlog, args, 1);

}

/************************************************************************
 *
 *  SaveDefaultCancelCB
 *      Cleanup and unmanage the save settings dialog.
 *
 ************************************************************************/
static void
SaveDefaultCancelCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
  FileMgrRec * file_mgr_rec;
  Arg args[1];

    /* Update the display, and un-post the dialog */
    XtUnmanageChild((Widget)client_data);
    XmUpdateDisplay((Widget)client_data);
    XtSetArg(args[0], XmNuserData, &file_mgr_rec);
    XtGetValues((Widget)client_data, args, 1);


    /* Re-sensatize the save settings menu pick */
    file_mgr_rec->menuStates |= SETTINGS;


    XtDestroyWidget((Widget)client_data);

}


/************************************************************************
 *
 *  SaveDefaultOkCB
 *      Save the current dtfile view as the default environment for
 *      new dtfiles created.
 *
 ************************************************************************/
static void
SaveDefaultOkCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{

   FileMgrRec * file_mgr_rec;
   static char * name_list[] = { DTFILE_CLASS_NAME, NULL, NULL };
   char view_number[5];
   int fd;
   Arg args[1];

   DialogData * dialog_data;
   FileMgrData * file_mgr_data;
   char * msg;
   char * tmp_path;
   char full_path[MAX_PATH + 1];
   char * tmpStr;
   char * save_host;
   char * save_directory;
   char ** save_branch_list;
   FileViewData ** save_selection_list;
   int save_selected_file_count;


   /* Get the file_mgr_rec hanging off the dialog */
   XtUnmanageChild((Widget)client_data);
   XmUpdateDisplay ((Widget)client_data);
   XtSetArg(args[0], XmNuserData, &file_mgr_rec);
   XtGetValues((Widget)client_data, args, 1);


   /* Re-sensatize the save settings menu pick */
   file_mgr_rec->menuStates |= SETTINGS;


   /* Get the file_mgr_rec dialog data info */

   dialog_data = _DtGetInstanceData ((XtPointer)file_mgr_rec);
   file_mgr_data = (FileMgrData *) dialog_data->data;


   /* Build the path for our env file */

   tmp_path = _DtCreateDtDirs(display);
   if (tmp_path == NULL)
     {
       XtDestroyWidget(client_data);
       return;
     }


   /* Look and see what type of restore we are doing */
   if (file_mgr_data->restoreKind == TOOL_RESTORE)
   {
      sprintf(full_path, "%s/%s", tmp_path, TOOL_SETTINGS_FILENAME);
      application_args.tool_width = file_mgr_data->width;
      application_args.tool_height = file_mgr_data->height;
   }
   else
   {
      sprintf(full_path, "%s/%s", tmp_path, HOME_SETTINGS_FILENAME);
      application_args.dir_width = file_mgr_data->width;
      application_args.dir_height = file_mgr_data->height;
   }

   XtFree(tmp_path);


   /*  Create the Environment session file  */

   if ((fd = creat(full_path, S_IRUSR | S_IRGRP | S_IWUSR | S_IWGRP)) == -1)
   {
      char *title;
      tmpStr = GETMESSAGE(18, 17, "Unable to create a file to store the default options.");
      title = XtNewString((GETMESSAGE(21,38,"Object Create Error")));
      msg = XtNewString(tmpStr);
      _DtMessage (toplevel, title, msg, NULL, HelpRequestCB);
      XtFree(msg);
      XtFree(title);
      return;
   }


   /*  Write out the general information  */

   (void) write (fd, "*", strlen ("*"));
   (void) write (fd, DTFILE_CLASS_NAME,strlen (DTFILE_CLASS_NAME));
   (void) write (fd, "*view_count:      ", strlen ("*view_count:      "));

   (void) sprintf (view_number, "%d", 1);
   (void) write (fd, view_number, strlen (view_number));
   (void) write (fd, "\n#\n", strlen ("\n#\n"));

   name_list[1] = view_number;

   /* Temporarily remove data that we don't need or want saved */

   save_host = file_mgr_data->host;
   save_directory = file_mgr_data->current_directory;
   save_branch_list = file_mgr_data->branch_list;
   save_selection_list = file_mgr_data->selection_list;
   save_selected_file_count = file_mgr_data->selected_file_count;
   file_mgr_data->host = NULL;
   file_mgr_data->current_directory = NULL;
   file_mgr_data->branch_list = NULL;
   file_mgr_data->selection_list = NULL;
   file_mgr_data->selected_file_count = 0;

   /*  Call the encapsulation mechanism to write each dialog  */

   (void) sprintf (view_number, "%d", 0);
   _DtWriteDialogData (dialog_data, fd, name_list);
   (void) write (fd, "#\n", strlen ("#\n"));

   /* Restore the data that was temporarily removed */

   file_mgr_data->host = save_host;
   file_mgr_data->current_directory = save_directory;
   file_mgr_data->branch_list = save_branch_list;
   file_mgr_data->selection_list = save_selection_list;
   file_mgr_data->selected_file_count = save_selected_file_count;

   (void) close (fd);

   XtDestroyWidget(client_data);

}



/************************************************************************
 *
 *  SaveSessionCallback
 *      Get the session name and call the function to save the session.
 *
 ************************************************************************/

void
SaveSessionCallback(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
   char * full_path=NULL;
   char * file_name=NULL;
   char * strPtr;
   int restore=NORMAL_RESTORE;
   Boolean status=FALSE;
   char * sessionFileName;
   char ** argv;

   if(view_count == 0 && desktop_data->numIconsUsed == 0)
   {
      XChangeProperty (display, XtWindow (toplevel), command_atom,
                    XA_STRING, 8, PropModeReplace, (unsigned char *)NULL, 0);
      XSync(display, False);
      FinalizeToolTalkSession( );
      exit(0);
   }

   status = DtSessionSavePath(w, &full_path, &file_name);

   if (status)
     sessionFileName = file_name;
   else
   {
     XtFree( (char *)full_path);
     full_path = (char *) XtMalloc (sizeof (char) * MAX_PATH);
     sprintf( full_path, "%s/%s", dt_path, DTFILE_CLASS_NAME );
     sessionFileName = full_path;
   }
   SaveSession( full_path );

   /* skip to the /.dt/ portion of the sessionFileName */
   strPtr = strstr(full_path, "/.dt/");
   (void)strcpy(full_path, strPtr);

   /* Save off the settings files for both tool and home views */
   do {
        strPtr = DtStrrchr(full_path, '/');
        if ( !strPtr )
	   continue;
        if ((strncmp(strPtr, "/home", 5) == 0))
           restore = HOME_DIR_RESTORE;
        else if ((strncmp(strPtr, "/current", 8) == 0))
           restore = CURRENT_DIR_RESTORE;

        if (strPtr != NULL)
          *strPtr = '\0';
   } while ((strPtr != NULL) && (restore == NORMAL_RESTORE));

   SaveDesktopInfo(restore);

   /* Cop the settings files to the proper dir */
   MoveDefaultSettings(restore);

   /*  Generate the reinvoking command and add it as the property value */
   argv = (char **) XtMalloc(3 * sizeof(char *));
   argv[0] = XtNewString(application_name);
   argv[1] = XtNewString("-session");
   argv[2] = XtNewString(sessionFileName);
   XSetCommand(XtDisplay(toplevel), XtWindow(toplevel), argv, 3);

   XtFree ((char *) argv[0]);
   XtFree ((char *) argv[1]);
   XtFree ((char *) argv[2]);
   XtFree ((char *) argv);

   XtFree ((char *) full_path);
   XtFree ((char *) file_name);
}

/************************************************************************
 *
 *  SaveSession
 *      Save the current File Manager session as a set of resources
 *      within the file denoted by path.
 *
 ************************************************************************/

static void
SaveSession(
        char *path
        )
{
   static char * name_list[] = { DTFILE_CLASS_NAME, NULL, NULL, NULL,
                                 NULL, NULL};
   char view_number[5];
   char number[5];
   char workspaceNumber[5];
   int fd;
   Atom * ws_presence = NULL;
   char * workspace_name=NULL;
   unsigned long num_workspaces = 0;

   Atom actual_type;
   int  actual_format;
   unsigned long nitems;
   unsigned long leftover;
   WM_STATE * wm_state;

   DialogData * dialog_data;
   FileMgrData * file_mgr_data;
   FileMgrRec  * file_mgr_rec;
   int i;
   int j;
   char * msg;
   int view_index;
   WorkspaceRec * workspaceInfo;
   char * tmpStr;
   Boolean saveTitle;
   char *actualSavedTitle;

   /* This needs to be done because name_list is static and the values
      can be changed during the routine.  This means that they need to be
      NULL'd out each pass */
   name_list[1] = NULL;
   name_list[2] = NULL;
   name_list[3] = NULL;
   name_list[4] = NULL;
   name_list[5] = NULL;

   /*  Disable any message box display during save session  */

   message_display_enabled = False;


   /*  Create the session file  */

   if ((fd = creat (path, S_IRUSR | S_IRGRP | S_IWUSR | S_IWGRP)) == -1)
   {
      tmpStr = GETMESSAGE(18, 18, "Could not open the session file.");
      msg = XtNewString(tmpStr);
      _DtSimpleError (application_name, DtError, NULL, msg);
      XtFree(msg);
      message_display_enabled = True;

      return;
   }


   /*  Write out the general information  */

   /* Number of dtfile views */
   (void) write (fd, "*", strlen ("*"));
   (void) write (fd, DTFILE_CLASS_NAME, strlen (DTFILE_CLASS_NAME));
   (void) write (fd, "*view_count:	", strlen ("*view_count:	"));

   if (trashFileMgrData)
   {
      if (trashFileMgrData->file_mgr_rec)
         (void) sprintf (view_number, "%d", view_count + 1);
      else
         (void) sprintf (view_number, "%d", view_count);
   }
   else
   {
      (void) sprintf (view_number, "%d", view_count);
   }
   (void) write (fd, view_number, strlen (view_number));
   (void) write (fd, "\n#\n", strlen ("\n#\n"));

   (void) write (fd, "*", strlen ("*"));
   (void) write (fd, DTFILE_CLASS_NAME, strlen (DTFILE_CLASS_NAME));
   (void) write (fd, "*showFilesystem:	", strlen ("*showFilesystem:	"));

   if(showFilesystem)
      (void) write (fd, "True", strlen ("True"));
   else
      (void) write (fd, "False", strlen ("False"));
   (void) write (fd, "\n#\n", strlen ("\n#\n"));

   (void) write (fd, "*", strlen ("*"));
   (void) write (fd, DTFILE_CLASS_NAME, strlen (DTFILE_CLASS_NAME));
   (void) write (fd, "*restrictMode:	", strlen ("*restrictMode:	"));

   if(restrictMode)
      (void) write (fd, "True", strlen ("True"));
   else
      (void) write (fd, "False", strlen ("False"));
   (void) write (fd, "\n#\n", strlen ("\n#\n"));

   (void) write (fd, "*", strlen ("*"));
   (void) write (fd, DTFILE_CLASS_NAME, strlen (DTFILE_CLASS_NAME));
   (void) write (fd, "*openFolder:	", strlen ("*openFolder:	"));

   if(openDirType == NEW)
      (void) write (fd, "NEW", strlen ("NEW"));
   else
      (void) write (fd, "CURRENT", strlen ("CURRENT"));
   (void) write (fd, "\n#\n", strlen ("\n#\n"));

   view_index = 0;
   name_list[1] = view_number;
   saveTitle = False;


   /*  Write out each of the view's resources  */
   /*  start with -1 so we can include the trash dialog */
   for (i = -1; i < view_count; i++)
   {
      if(i == -1)
      {
         if(trashDialogData)
            dialog_data = (DialogData *) trashDialogData;
         else
            continue;
      }
      else
      {
         dialog_data = (DialogData *) view_set[i]->dialog_data;
      }

      file_mgr_data = (FileMgrData *) dialog_data->data;
      if(i == -1 && trashDialogData)
          file_mgr_data->IsTrashCan = True;
      file_mgr_rec = (FileMgrRec *) file_mgr_data->file_mgr_rec;
      if(file_mgr_rec == NULL)
         continue;

      /* This is a bug fix ... We don't want to save the title if the
         View is the Trash Can of if it is an Application Manager.  This
         is because if the user saves a session in one Locale, then logs
         in in another, the Title will be in the locale that the session was
         saved in rather then the new local.  So let's save the Title,
         Null it out, Save the session info, and finally restore the Title.
      */
      if(i == -1 || file_mgr_data->toolbox)
      {
         /* the Trash Can or toolbox (i.e. Application Manager) */
         actualSavedTitle = file_mgr_data->title;
         file_mgr_data->title = NULL;
         saveTitle = True;
      }

      /*  Getting the WM_STATE property to see if iconified or not */
      XGetWindowProperty (display, XtWindow (file_mgr_rec->shell),
                             wm_state_atom, 0L, (long) BUFSIZ, False,
                             wm_state_atom, &actual_type, &actual_format,
                             &nitems, &leftover, (unsigned char **) &wm_state);

      /* Write out if iconified our not */

      write (fd, "*", strlen ("*"));
      write (fd, DTFILE_CLASS_NAME, strlen (DTFILE_CLASS_NAME));
      sprintf (view_number, "%d", view_index);
      write (fd, ".", strlen ("."));
      write (fd, view_number, strlen (view_number));

      (void) write (fd, ".iconify:      ", strlen (".iconify:   "));

      if (wm_state->state == IconicState)
        write (fd, "True\n", strlen ("True\n"));
      else
        write (fd, "False\n", strlen ("False\n"));


      /*  Get the workspaces for this dt by accessing the property.  */

      if (DtWsmGetWorkspacesOccupied (display, XtWindow (file_mgr_rec->shell),
                                  &ws_presence, &num_workspaces) == Success)
      {
         write (fd, "*", strlen ("*"));
         write (fd, DTFILE_CLASS_NAME, strlen (DTFILE_CLASS_NAME));
         (void) write (fd, ".", strlen ("."));
         (void) write (fd, view_number, strlen (view_number));
         (void) write (fd, ".workspace: ", strlen (".workspace: "));


         for (j = 0; j < num_workspaces; j++)
         {
            if (j != 0) (void) write (fd, "*", strlen ("*"));
            workspace_name = XGetAtomName (display, ws_presence[j]);
            (void) write (fd, workspace_name, strlen (workspace_name));
            XtFree ((char *) workspace_name);
         }


         (void) write (fd, "\n", strlen ("\n"));
         XFree((char *)ws_presence);
      }


      /*  Call the encapsulation mechanism to write each dialog  */

      if(i == -1)
         _DtWriteDialogData (trashDialogData, fd, name_list);
      else
         _DtWriteDialogData ((DialogData *)view_set[i]->dialog_data,
			     fd, name_list);
      (void) write (fd, "#\n", strlen ("#\n"));

      view_index++;
      if(saveTitle)
      {
         file_mgr_data->title = actualSavedTitle;
         saveTitle = False;
      }
   }

   /*
    * Save off help dialog information for each workspace.
    * These are the help dialogs used for the desktop objects.
    */
   name_list[1] = WS_LOAD_RES_HEADER;
   name_list[2] = workspaceNumber;
   name_list[3] = number;

   for (i = 0; i < desktop_data->numWorkspaces; i++)
   {
      workspaceInfo = desktop_data->workspaceData[i];

      /*
       * Save number of secondary help dialogs in format:
       *     *Dtfile.Workspace.<WS#>.secondaryHelpDialogCount:  <#>
       */
      write(fd, "*", strlen("*"));
      write(fd, DTFILE_CLASS_NAME, strlen(DTFILE_CLASS_NAME));
      write(fd, WS_RES_HEADER, strlen(WS_RES_HEADER));
      sprintf(workspaceNumber, "%d", i);
      write(fd, workspaceNumber, strlen(workspaceNumber));
      write(fd, SEC_HELP_RES_HEADER, strlen(SEC_HELP_RES_HEADER));
      sprintf(view_number, "%d", workspaceInfo->secondaryHelpDialogCount);
      write(fd, view_number, strlen(view_number));
      write (fd, "\n#\n", strlen ("\n#\n"));

      /* Save each of the secondary help dialogs */
      for (j = 0; j < workspaceInfo->secondaryHelpDialogCount; j++)
      {
         sprintf(number, "%d", j + 1);
         _DtWriteDialogData(workspaceInfo->secondaryHelpDialogList[j],
                           fd, name_list);
      }

      /* Save the primary help dialog window */
      if (workspaceInfo->primaryHelpDialog)
      {
         sprintf(number, "%d", 0);
         _DtWriteDialogData(workspaceInfo->primaryHelpDialog,
                           fd, name_list);
      }
      write (fd, "#\n", strlen ("#\n"));
   }
   (void) close (fd);

   /*  Re-able message box display flag after save session  */
   message_display_enabled = True;


}



/*
 * Given a directory name, this function will see if a view of the parent
 * directory is open; if so, then it will update the icon representing
 * this icon, in the parent view, so that it is drawn as 'open'.  This
 * function must only be called if openDirType == NEW.
 */

void
ForceMyIconOpen (
   char * host_name,
   char * directory_name)
{
   FileViewData * file_view_data = NULL;
   DesktopRec *desktopWindow;
   DialogData * dd;
   FileMgrData * fmd;
   char * parent;
   char * fname;
   char * ptr;
   char * full_path;
   int i, j, k;
   char *icon_name, *new_file_type_name, *file_type_name;
   Arg args[5];

   /* if directory_name is passed in a NULL, we want to go through all
      existing open directories and check to see if there are any open
      directories in each of them. This is used at the end of OpenNewView
      and the end of ShowNewDirectory */
   if (directory_name == NULL)
   {
      for (i = 0; i < view_count; i++)
      {
         dd = (DialogData *) view_set[i]->dialog_data;
         fmd = (FileMgrData *) dd->data;

         for(j = 0; j < fmd->directory_count; j++)
         {
            ForceMyIconOpen(view_set[i]->host_name,
                            fmd->directory_set[j]->name);
         }
      }
      return;
   }

   parent = _DtPName(directory_name);
   fname =  DName(directory_name);

   /* first lets check to see if the directory is open in one of the
      open file manager views */
   for (i = 0; i < view_count; i++)
   {
      dd = (DialogData *) view_set[i]->dialog_data;
      fmd = (FileMgrData *) dd->data;

      /* loop through until we find the file_view_data structure for the
         directory to force open */
      if (strcmp(host_name, view_set[i]->host_name) == 0)
      {
         for(j = 0; j < fmd->directory_count; j++)
         {
            if (strcmp(parent, fmd->directory_set[j]->name) == 0)
            {
               for (k = 0; k < fmd->directory_set[j]->file_count; k++)
               {
                  file_view_data = fmd->directory_set[j]->file_view_data[k];
                  if (strcmp(file_view_data->file_data->file_name, fname) == 0)
                     break;
                  file_view_data = NULL;
               }
               break;
            }
         }
      }

      if (file_view_data)
      {
         fmd = (FileMgrData *)(((DirectorySet *)file_view_data->directory_set)->
                           file_mgr_data);

         file_type_name = file_view_data->file_data->logical_type;

         if(fmd->view != BY_NAME)
            BuildAndShowIconName(file_type_name, fmd->view,
                                 fmd->show_type, file_view_data->widget);
      }
   }

   /* now we need to check to see if the directory being opened has a
      representation on the Desktop */
   for(i = 0; i < desktop_data->numIconsUsed; i++)
   {
      char buf[MAX_PATH];

      desktopWindow = desktop_data->desktopWindows[i];
      file_view_data = desktopWindow->file_view_data;

      sprintf(buf, "%s/%s", desktopWindow->dir_linked_to,
                            desktopWindow->file_name);
      DtEliminateDots (buf);

      if (strcmp(buf, directory_name) == 0 &&
                                    strcmp(desktopWindow->host, host_name) == 0)
      {
         file_type_name = file_view_data->file_data->logical_type;
         if(desktopIconType == LARGE)
             BuildAndShowIconName(file_type_name, BY_NAME_AND_ICON,
                                 SINGLE_DIRECTORY, desktopWindow->iconGadget);
         else /* SMALL */
             BuildAndShowIconName(file_type_name, BY_NAME_AND_SMALL_ICON,
                                 SINGLE_DIRECTORY, desktopWindow->iconGadget);
      }
   }
}




/************************************************************************
 *
 *  RestoreSession
 *      Open the file as a resource data base, and use the data to
 *      create a set of File Manager views.
 *
 ************************************************************************/

static int
RestoreSession(
        char *path,
        int type_of_restore,
        char *directory)
{
   static char * name_list[] = { DTFILE_CLASS_NAME, NULL, NULL, NULL,
                                 NULL, NULL };
   XrmDatabase db;
   XrmName xrm_name[10];
   XrmRepresentation rep_type;
   XrmValue value;
   int num_views = 0;
   char * full_path = NULL;
   Boolean status=False;
   char * temp = NULL;
   char wsNum[5];
   char dialogNum[5];
   int num_sec_help_dialogs;
   int i;
   int j;
   DialogData * dialogData;
   struct stat stat_buf;


   /* Build the session path if we need to.
    * (e.g. When using the -session option)
    */
    if (type_of_restore == NORMAL_RESTORE)
    {
      status = DtSessionRestorePath(toplevel, &full_path, path);

      if (!status)
         return;

      if (stat(full_path, &stat_buf) != 0)
      {
        char *tmpStr, *msg, *title;

        tmpStr = GETMESSAGE(18, 18, "Could not open the session file.");
        msg = XtNewString(tmpStr);
        title = XtNewString((GETMESSAGE(21,39,"File Open Error")));
        _DtMessage (toplevel, title, msg, NULL, HelpRequestCB);
        XtFree(msg);
        XtFree(title);
        XtFree(full_path);
        full_path = NULL;
        return(-1);
      }

      path = XtNewString(full_path);
    }

   /* This prevents the encapsulator from placing the dialogs */
   disableDialogAutoPlacement = True;

   /*  Open the file as a resource database and query it to  */
   /*  get the previously saved view count.                  */

   db = XrmGetFileDatabase (path);

   if (type_of_restore == NORMAL_RESTORE)
   {
      /* first find out if it should show the file system */
      xrm_name [0] = XrmStringToQuark (DTFILE_CLASS_NAME);
      xrm_name [1] = XrmStringToQuark ("showFilesystem");
      xrm_name [2] = NULL;
      if (XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value))
      {
         if ((temp = (char *) value.addr) != NULL &&
                 strcmp (temp, "True") == 0)
         {
            showFilesystem = True;
         }
         else
            showFilesystem = False;
      }
      else
         showFilesystem = True;

      /* find out if it should be in restricted mode */
      xrm_name [0] = XrmStringToQuark (DTFILE_CLASS_NAME);
      xrm_name [1] = XrmStringToQuark ("restrictMode");
      xrm_name [2] = NULL;
      if (XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value))
      {
         if ((temp = (char *) value.addr) != NULL &&
                 strcmp (temp, "True") == 0)
         {
            restrictMode = True;
         }
         else
            restrictMode = False;
      }
      else
         restrictMode = False;

      /* find out openFolder mode */
      xrm_name [0] = XrmStringToQuark (DTFILE_CLASS_NAME);
      xrm_name [1] = XrmStringToQuark ("openFolder");
      xrm_name [2] = NULL;
      if (XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value))
      {
         if ((temp = (char *) value.addr) != NULL &&
                 strcmp (temp, "NEW") == 0)
         {
            openDirType = NEW;
         }
         else
            openDirType = CURRENT;
      }
      else
         openDirType = CURRENT;
   }

   xrm_name [0] = XrmStringToQuark (DTFILE_CLASS_NAME);
   xrm_name [1] = XrmStringToQuark ("view_count");
   xrm_name [2] = NULL;

   /* Load standard dtfile views */
   if (XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value))
   {
      num_views = atoi (value.addr);
      if (num_views > 0)
         LoadViews(num_views, db, NULL, directory, special_restricted, 0);
   }

   /* Restore any desktop help dialogs */
   if (type_of_restore == NORMAL_RESTORE)
   {
      for (i = 0; i < desktop_data->numWorkspaces; i++)
      {
         sprintf(wsNum, "%d", i);
         xrm_name [0] = XrmStringToQuark (DTFILE_CLASS_NAME);
         xrm_name [1] = XrmStringToQuark (WS_LOAD_RES_HEADER);
         xrm_name [2] = XrmStringToQuark (wsNum);
         xrm_name [3] = XrmStringToQuark (SEC_LOAD_HELP_RES_HEADER);
         xrm_name [4] = NULL;

         /* Load standard dtfile views */
         if (XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value))
         {
            num_sec_help_dialogs = atoi (value.addr);
            if (num_sec_help_dialogs > 0)
            {
               desktop_data->workspaceData[i]->secondaryHelpDialogCount =
                  num_sec_help_dialogs;
               desktop_data->workspaceData[i]->secondaryHelpDialogList =
                (DialogData **) XtMalloc(sizeof(DialogData *) *
                   num_sec_help_dialogs);

               for (j = 0; j < num_sec_help_dialogs; j++)
               {
                  name_list[0] = DTFILE_CLASS_NAME;
                  name_list[1] = WS_LOAD_RES_HEADER;
                  name_list[2] = wsNum;
                  name_list[3] = dialogNum;
                  name_list[4] = NULL;
                  sprintf(dialogNum, "%d", j + 1);
                  dialogData =
                       _DtGetResourceDialogData(help_dialog, db, name_list);
                  desktop_data->workspaceData[i]->secondaryHelpDialogList[j]=
                     dialogData;
                  ShowDTHelpDialog(NULL, i, HYPER_HELP_DIALOG,
                                   dialogData, NULL, NULL, NULL, NULL,
                                   DtHELP_TYPE_TOPIC);
               }
            }
         }

         /* Load the primary help dialog */
         name_list[0] = DTFILE_CLASS_NAME;
         name_list[1] = WS_LOAD_RES_HEADER;
         name_list[2] = wsNum;
         name_list[3] = dialogNum;
         name_list[4] = NULL;
         sprintf(dialogNum, "%d", 0);
         dialogData = _DtGetResourceDialogData(help_dialog, db, name_list);

         /* Keep only if currently posted */
         if (!(((HelpData *)dialogData->data)->displayed))
         {
            /* Not currently displayed */
            _DtFreeDialogData(dialogData);
            dialogData = NULL;
         }
         desktop_data->workspaceData[i]->primaryHelpDialog = dialogData;
         if (dialogData)
         {
            ShowDTHelpDialog(NULL, i, MAIN_HELP_DIALOG,
                             dialogData, NULL, DTFILE_HELP_NAME,
                             NULL, NULL, 0);
         }
      }
   }

   XtFree(full_path);
   full_path = NULL;

   /* Free the Xrm Database */
   XrmDestroyDatabase(db);
   disableDialogAutoPlacement = False;

   return(0);
}


static void
ViewAccept(
   View *view,
   Tt_message msg)
{
   extern Tt_message FileCallback();
   extern Tt_message SessionCallback();

   if ((msg == 0) || tt_is_err( tt_ptr_error( msg ))) {
      return;
   }
   view->msg = msg;
   /*
    * Register for notifications on the directory viewed.
    * This is done so that requesting apps can notify the
    * view if the directory name is changed.
    */
   view->pats = ttdt_file_join( view->directory_name, TT_SESSION, 0,
			        FileCallback, view );
   if (tt_is_err( tt_ptr_error( view->pats ))) {
      view->pats = 0;
   }

   /* Returned patterns automatically get destroyed when msg is destroyed */
   ttdt_message_accept( msg, SessionCallback,
		        _DtGetDialogShell( (DialogData *)view->dialog_data ),
		        view, 1, 1 );
}


static void
LoadViews (
   int num_views,
   XrmDatabase db,
   char *host_name,
   char *directory_name,
   char *type,
   Tt_message msg)

{
   XrmName xrm_name[5];
   XrmRepresentation rep_type;
   XrmValue value;
   static char * name_list[] = { DTFILE_CLASS_NAME, NULL, NULL };
   char view_number[5];
   DialogData * dialog_data;
   FileMgrData * file_mgr_data;
   char * workspaces;
   XClassHint classHints;
   char * iconify = NULL;
   Boolean iconify_window;
   int i;
   char *title, *tmpTitle;

   name_list[1] = view_number;
   xrm_name [0] = XrmStringToQuark (DTFILE_CLASS_NAME);


   /*  Get and display view_count views.  */

   for (i = 0; i < num_views; i++)
   {
      struct stat stat_buf;

      (void) sprintf (view_number, "%d", i);
      xrm_name [1] = XrmStringToQuark (view_number);

      /* Get the main dialog data and set up the view  */

      dialog_data = _DtGetResourceDialogData (file_mgr_dialog, db, name_list);
      file_mgr_data = (FileMgrData *) dialog_data->data;

      if(file_mgr_data->toolbox && file_mgr_data->title == NULL)
         file_mgr_data->title = DtActionLabel("Dtappmgr");

      if (stat(file_mgr_data->current_directory, &stat_buf) != 0)
      {
         _DtFreeDialogData(dialog_data);
         continue;
      }

      if (trashFileMgrData
          && (file_mgr_data->IsTrashCan == True)
          && strcmp(file_mgr_data->current_directory, trash_dir) == 0)
      {
         trashFileMgrData->view = file_mgr_data->view;
         trashFileMgrData->order = file_mgr_data->order;
         trashFileMgrData->direction = file_mgr_data->direction;
         trashFileMgrData->positionEnabled = file_mgr_data->positionEnabled;
         trashFileMgrData->preferences = file_mgr_data->preferences;
         file_mgr_data->preferences = NULL;
         _DtFreeDialogData(dialog_data);
         continue;
      }

      /* Increment the view list size if necessary and add directory to list */

      if (view_count == view_set_size)
      {
         view_set_size += 10;
         view_set =
            (View **) XtRealloc ((char *)view_set,
                                     sizeof (View **) * view_set_size);
      }
      view_set[view_count] = (View *) XtMalloc (sizeof (View));
      view_set[view_count]->dialog_data = (XtPointer) dialog_data;
      view_set[view_count]->msg = 0;
      view_set[view_count]->pats = 0;

      if(restoreType == TOOL_RESTORE)
         file_mgr_data->toolbox = True;

      if(directory_name == NULL)
      {
         view_set[view_count]->host_name = XtNewString (file_mgr_data->host);
         view_set[view_count]->directory_name =
            XtNewString (file_mgr_data->current_directory);
      }
      else
      {
         XtFree(file_mgr_data->current_directory);
         file_mgr_data->current_directory = NULL;
         XtFree(file_mgr_data->restricted_directory);
         file_mgr_data->restricted_directory = NULL;

         view_set[view_count]->directory_name = XtNewString(directory_name);
         XtFree((char *)file_mgr_data->selection_list);
         file_mgr_data->selection_list = NULL;
         if(host_name == NULL)
         {
            view_set[view_count]->host_name = XtNewString (file_mgr_data->host);
            file_mgr_data->current_directory = XtNewString(directory_name);
            if(type == NULL)
              file_mgr_data->restricted_directory =
                                              XtNewString(directory_name);
            else
              file_mgr_data->restricted_directory =
                                              XtNewString(type);
         }
         else
         {
            view_set[view_count]->host_name = XtNewString (host_name);
            XtFree(file_mgr_data->host);
            file_mgr_data->host = XtNewString(host_name);
            file_mgr_data->current_directory = XtNewString(directory_name);
            if(special_view && special_restricted != NULL)
               file_mgr_data->restricted_directory =
                                     XtNewString(special_restricted);
            else
               file_mgr_data->restricted_directory = NULL;
         }
         FileMgrBuildDirectories (file_mgr_data,
                          view_set[view_count]->host_name, directory_name);
      }

      /*  Get the workspace set the view is contained in  */
      /*  and set the property for the view just created  */

      xrm_name [2] = XrmStringToQuark ("workspace");
      xrm_name [3] = NULL;

      if (XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value))
      {
         /*  Make sure we have some valid workspaces names to work with */

         /* value.addr should = NULL if no workspace names */
         workspaces = (char *) value.addr;
      }
      else
        /* we have no workspace resource so use default */
        workspaces = NULL;

      /*  Get and set whether the view is iconic  */

      xrm_name [2] = XrmStringToQuark ("iconify");
      xrm_name [3] = NULL;

      if (XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value))
      {
         /*  If there is an iconify resource and its value is True,  */
         /*  then mark the window as iconified.                      */

         if ((iconify = (char *) value.addr) != NULL &&
              strcmp (iconify, "True") == 0)
         {
           iconify_window = True;

         }
         else
          iconify_window = False;
      }
      else
         iconify_window = False;

     if(file_mgr_data->positionEnabled == RANDOM_ON &&
                        (file_mgr_data->object_positions == NULL))
        LoadPositionInfo(file_mgr_data);

      /* Call _DtShowDialog to create and manage the new window */

     tmpTitle = file_mgr_data->title;
     title = file_mgr_data->title = _DtBuildFMTitle(file_mgr_data);
     if (special_title)
        XtFree(special_title);
     special_title = XtNewString(title);

     classHints.res_name = title;
     classHints.res_class = DTFILE_CLASS_NAME;

     _DtShowDialog (NULL, NULL, NULL, dialog_data, NULL, NULL,
                    RemoveTextFields, NULL, workspaces, iconify_window,
                    special_view, title, &classHints);

     XtFree(title);
     file_mgr_data->title = tmpTitle;

     if (msg != 0) {
	ViewAccept( view_set[view_count], msg );
	msg = 0;
     }

     view_count++;
   }

   if (openDirType == NEW)
      ForceMyIconOpen(file_mgr_data->host, NULL);
}


/************************************************************************
 *
 *  GetPWD
 *
 ************************************************************************/
static void
GetPWD(
        char current_directory[] )
{
   FILE * pwd_file;
   register int i;

    /*  Open a pwd process and read the current working directory  */
    /*  from it.  If the open fails or a read fails, then display  */
    /*  the users home directory.                                  */

      pwd_file = popen ("pwd", "r");
      i = 0;

      if (pwd_file != NULL)
      {
         while (1)
         {
            errno = 0;
            if (fread (&current_directory[i], sizeof(char), 1, pwd_file) != 1)
            {
               if (errno == EINTR)
                  continue;
               else
                  break;
            }

            if (current_directory[i] == '\n')
               break;

            i++;
         }
      }
    (void) pclose (pwd_file);
    current_directory[i] = NULL;

}

/************************************************************************
 *
 *  OpenDirectories
 *      Given a string that contains a single or set of host:path
 *      specifications, parse out each host:path, validate it as
 *      a accessible directory, and call a function to create a
 *      file manager view of the directory.
 *
 ************************************************************************/

static void
OpenDirectories(
        char *directory_set,
        char *type)
{
   char * separator;
   char * host;
   char * path;

   /*  Loop the the directory set string until all of  */
   /*  the path specifications have be parsed.    */

   while (1)
   {
      separator = DtStrchr (directory_set, ',');
      if (separator != NULL)
         *separator = NULL;

      _DtPathFromInput(directory_set, NULL, &host, &path);

      if (path)
      {
         GetNewView (host, path, type, NULL, 0);
      }
      else
      {
         char *tmpStr, *errTitle, *errMsg, *dmsg;

         tmpStr = GETMESSAGE(32, 2, "File Manager Open Directory Error");
         errTitle = XtNewString(tmpStr);
         tmpStr = GETMESSAGE(18, 38, "Invalid folder specification, %s");
         errMsg = XtNewString(tmpStr);
         dmsg = XtMalloc(strlen(errMsg)+strlen(directory_set)+1);
         sprintf(dmsg, errMsg, directory_set);
         _DtMessage(toplevel, errTitle, dmsg, NULL, HelpRequestCB);

         XtFree(errTitle);
         XtFree(errMsg);
         XtFree(dmsg);
      }

      /*  Free up the unique host and directory names  */
      /*  that were allocated.                         */

      XtFree ((char *) host);
      XtFree ((char *) path);

      /*  Set the starting position of the next host:path  */

      if (separator != NULL)
         directory_set = separator + 1;
      else
         break;
   }
}




/************************************************************************
 *
 *  GetNewView
 *      Given a directory name, generate a new view for the directory.
 *
 ************************************************************************/
DialogData *
GetNewView(
        char *host_name,
        char *directory_name,
        char *type,
        WindowPosition *position,
        Tt_message msg)
{
   DialogData * dialog_data;
   DialogInstanceData * instance_data;
   FileMgrData * file_mgr_data;
   FileMgrData * fmd;
   FileMgrRec * file_mgr_rec;
   char * real_directory_name;
   int i;
   XClassHint classHints;
   char *title, *tmpTitle;
   struct stat stat_buf;
   char *tmpStr;
   char *errMsg = NULL;

   DtEliminateDots( directory_name );

   if (stat(directory_name, &stat_buf) == 0)
   {
      if ((stat_buf.st_mode & S_IFMT) != S_IFDIR)
      {
         tmpStr = GETMESSAGE(18, 19,
                  "The folder specification,\n%s\nis not a folder.");
         errMsg = XtNewString(tmpStr);
      }
   }
   else
   {
      tmpStr = GETMESSAGE(18, 20,
               "The folder specification,\n%s\ndoes not exist.");
      errMsg = XtNewString(tmpStr);
   }

   if (errMsg)
   {
      char *title, *dmsg;

      tmpStr = GETMESSAGE(32, 2, "File Manager Open Directory Error");
      title = XtNewString(tmpStr);
      dmsg = XtMalloc(strlen(errMsg) +
                      strlen(directory_name) + 1);
      sprintf(dmsg, errMsg, directory_name);
      _DtMessage(toplevel, title, dmsg, NULL, HelpRequestCB);

      XtFree(errMsg);
      XtFree(title);
      XtFree(dmsg);
      return(NULL);
   }

   /*
    * Special case: When opening any of the special desktop directory
    * icons (Home, Remote Systems, etc), we want to display the correct
    * path (i.e. $HOME instead of $HOME/.dt/Desktop/Home).  So ... we'll
    * do the remap here.
    */
   real_directory_name = directory_name;

   if(openDirType == NEW || (strcmp(real_directory_name, desktop_dir) == 0))
   {
      for(i = 0; i < view_count; i++)
      {
         if((strcmp(real_directory_name, view_set[i]->directory_name) == 0 &&
            (strcmp(host_name, view_set[i]->host_name) == 0))
                                  )
         {
             Window   rootWindow;
             Atom     pCurrent;
             Screen   *currentScreen;
             int      screen;

             dialog_data = (DialogData *) view_set[i]->dialog_data;
             file_mgr_data = (FileMgrData *) dialog_data->data;
             file_mgr_rec = (FileMgrRec *) file_mgr_data->file_mgr_rec;

             screen = XDefaultScreen(display);
             currentScreen = XScreenOfDisplay(display, screen);
             rootWindow = RootWindowOfScreen(currentScreen);

             /* Get the current Workspace */
             if (DtWsmGetCurrentWorkspace(display, rootWindow, &pCurrent)
                                                                   == Success)
             {
                Atom * ws_presence = NULL;
                unsigned long num_workspaces = 0;
                int k;

                if (DtWsmGetWorkspacesOccupied(display,
                     XtWindow(file_mgr_rec->shell), &ws_presence,
                     &num_workspaces) == Success)
                {
                   /* Already in this workspace? */
                   for (k = 0; k < num_workspaces; k++)
                   {
                      if (ws_presence[k] == pCurrent)
                         break;
                   }

                   if (k >= num_workspaces)
                   {
                      /* Add to the workspace */
                      ws_presence = (Atom *) XtRealloc((char *)ws_presence,
                           sizeof (Atom) * (num_workspaces + 1));

                      ws_presence[num_workspaces] = pCurrent;
                      DtWsmSetWorkspacesOccupied(display,
                                          XtWindow(file_mgr_rec->shell),
                                          ws_presence, num_workspaces + 1);
                   }
                   XFree((char *)ws_presence);
                }
                else
                {
                   /* Change the hints to reflect the current workspace */
                   DtWsmSetWorkspacesOccupied(display,
                                           XtWindow(file_mgr_rec->shell),
                                           &pCurrent, 1);
                }
             }

             /* must map the window to catch iconified windows */
             /* a XtPopup will not catch it */
             XtMapWidget(file_mgr_rec->shell);
             XRaiseWindow(display, XtWindow(file_mgr_rec->shell));

             /* announce activity */
	     {
	        Tt_message msg;
		msg = tt_pnotice_create(TT_SESSION, "DtActivity_Began");
		tt_message_send(msg);
		tttk_message_destroy(msg);
	     }

             return(NULL);
         }
      }
   }

   /* If in novice mode, force the icon for this dir to the 'Open' state */
   if (openDirType == NEW)
      ForceMyIconOpen(host_name, directory_name);

   /*  Increment the list size if necessary.  */

   if (view_count == view_set_size)
   {
      view_set_size += 10;
      view_set =
         (View **) XtRealloc ((char *)view_set,
                                    sizeof (View **) * view_set_size);
   }

   view_set[view_count] = (View *) XtMalloc (sizeof (View));
   view_set[view_count]->msg = 0;
   view_set[view_count]->pats = 0;

   if (initiating_view != NULL)
      dialog_data = _DtGetDefaultDialogData (file_mgr_dialog);
   else
   {
      char * tmp_path;
      char full_path[MAX_PATH + 1];
      XrmDatabase db;

      tmp_path = _DtCreateDtDirs(display);
      if(type != NULL)
         sprintf(full_path, "%s/%s", tmp_path, TOOL_SETTINGS_FILENAME);
      else
         sprintf(full_path, "%s/%s", tmp_path, HOME_SETTINGS_FILENAME);
      XtFree(tmp_path);
      db = XrmGetFileDatabase (full_path);
      if(db != NULL)
      {
         if(type != NULL)
         {
            restoreType = TOOL_RESTORE;
            LoadViews(1, db, NULL, directory_name, type, msg);
         }
         else
         {
            restoreType = HOME_RESTORE;
            LoadViews(1, db, host_name, directory_name, NULL, msg);
         }

         /*
          * Free the Xrm Database
          */
         XrmDestroyDatabase(db);
         dialog_data = (DialogData *)view_set[view_count - 1]->dialog_data;
         file_mgr_data = (FileMgrData *) dialog_data->data;
         if(type != NULL)
         {
            application_args.tool_width = file_mgr_data->width;
            application_args.tool_height = file_mgr_data->height;
         }
         else
         {
            application_args.dir_width = file_mgr_data->width;
            application_args.dir_height = file_mgr_data->height;
         }
         if(file_mgr_data->find != NULL &&
                                file_mgr_data->current_directory != NULL)
         {
            DialogData * dialog_data;
            FindData * find_data;

            dialog_data = (DialogData *)file_mgr_data->find;
            find_data = (FindData *)dialog_data->data;

            XtFree(find_data->directories);
            if(file_mgr_data->restricted_directory == NULL)
            {
               if(restrictMode)
                  find_data->directories = XtNewString(users_home_dir);
               else
                  find_data->directories =
                            XtNewString(file_mgr_data->current_directory);
            }
            else
            {
               if(strcmp(file_mgr_data->current_directory,
                         file_mgr_data->restricted_directory) == 0)
                  find_data->directories = XtNewString("/");
               else
                  find_data->directories =
                        XtNewString(file_mgr_data->current_directory +
                        strlen(file_mgr_data->restricted_directory));
            }
         }

         /* If in novice mode, force the icon for all the dir's to the
            'Open' state */
         if (openDirType == NEW)
            ForceMyIconOpen(host_name, NULL);

         return((DialogData *)view_set[view_count - 1]->dialog_data);
      }
      else
         dialog_data = _DtGetDefaultDialogData (file_mgr_dialog);
   }
   view_set[view_count]->dialog_data = (XtPointer) dialog_data;
   file_mgr_data = (FileMgrData *) dialog_data->data;

   /*  Adjust the view settings if this new view was created  */
   /*  because of an action on a previous view.               */

   if (initiating_view != NULL)
   {
      FileMgrPropagateSettings ((FileMgrData *)initiating_view, file_mgr_data);
      /* force new window to come up in flat mode */
      file_mgr_data->show_type =
        ((PreferencesData *)file_mgr_data->preferences->data)->show_type =
                                                              SINGLE_DIRECTORY;
      file_mgr_data->view = file_mgr_data->view_single;
   }

      /*  Call the FileMgr dialog to build up its directory set for  */
      /*  the directory name.                                        */
   FileMgrBuildDirectories (file_mgr_data, host_name, real_directory_name);

   if(type != NULL)
   {
      PreferencesData *preferences_data;

      file_mgr_data->toolbox = True;
      file_mgr_data->width = application_args.tool_width;
      file_mgr_data->height = application_args.tool_height;

      /* we want to default for the toolboxes (i.e. Application Manager) to
       * look different from a normal File Manager view.  Let's turn off
       * the iconic_path, current_directory, and status_line.
       */
      file_mgr_data->show_iconic_path = False;
      file_mgr_data->show_current_dir = False;
/*
      file_mgr_data->show_status_line = True;
*/

      preferences_data = (PreferencesData *)file_mgr_data->preferences->data;
      preferences_data->show_iconic_path = file_mgr_data->show_iconic_path;
      preferences_data->show_current_dir = file_mgr_data->show_current_dir;
      preferences_data->show_status_line = file_mgr_data->show_status_line;
   }
   else
   {
      file_mgr_data->toolbox = False;
      file_mgr_data->width = application_args.dir_width;
      file_mgr_data->height = application_args.dir_height;
   }


   if(file_mgr_data->find != NULL && file_mgr_data->current_directory != NULL)
   {
      DialogData * dialog_data;
      FindData * find_data;

      dialog_data = (DialogData *)file_mgr_data->find;
      find_data = (FindData *)dialog_data->data;

      XtFree(find_data->directories);
      if(file_mgr_data->restricted_directory == NULL)
      {
         if(restrictMode)
            find_data->directories = XtNewString(users_home_dir);
         else
            find_data->directories =
                            XtNewString(file_mgr_data->current_directory);
      }
      else
      {
         if(strcmp(file_mgr_data->current_directory,
                   file_mgr_data->restricted_directory) == 0)
            find_data->directories = XtNewString("/");
         else
            find_data->directories =
                   XtNewString(file_mgr_data->current_directory +
                   strlen(file_mgr_data->restricted_directory));
      }
   }

   view_set[view_count]->host_name = XtNewString (file_mgr_data->host);
   view_set[view_count]->directory_name =
         XtNewString (file_mgr_data->current_directory);

   /* Load default position info, or inherit, if appropriate */
   if ((fmd = (FileMgrData *)initiating_view) &&
       (strcmp(fmd->host, host_name) == 0) &&
       (strcmp(fmd->current_directory, real_directory_name) == 0))
   {
      InheritPositionInfo(fmd, file_mgr_data);
   }
   else
      LoadPositionInfo(file_mgr_data);


   /*  Get the dialog displayed.  */
   if (position)
   {
      instance_data = (DialogInstanceData *) dialog_data->data;
      instance_data->x = position->x;
      instance_data->y = position->y;
      instance_data->displayed = True;  /* @@@ Hack! without this,
                                          _DtShowDialog will ignore
                                           our position info */
   }

   tmpTitle = file_mgr_data->title;
   title = file_mgr_data->title = _DtBuildFMTitle(file_mgr_data);
   if (special_title)
      XtFree(special_title);
   special_title = XtNewString(title);

   classHints.res_name = title;
   classHints.res_class = DTFILE_CLASS_NAME;
   initiating_view = NULL;

   _DtShowDialog (NULL, NULL, NULL, dialog_data, NULL, NULL, RemoveTextFields,
               NULL, NULL, False, special_view, title, &classHints);

   XtFree(title);
   file_mgr_data->title = tmpTitle;

   if (msg != 0) {
      ViewAccept( view_set[view_count], msg );
      msg = 0;
   }

   view_count++;

   /* If in novice mode, force the icon for all the dir's to the 'Open' state */
   if (openDirType == NEW)
      ForceMyIconOpen(host_name, NULL);

   return(dialog_data);
}




/************************************************************************
 *
 *  CloseView
 *      Update the view set array when a view is closed.
 *
 ************************************************************************/

void
CloseView(
        DialogData *dialog_data )
{
   register int i;
   register int j;
   FileMgrData * file_mgr_data;
   DialogData * tmpDialog_data;
   FileMgrRec * file_mgr_rec;
   XmManagerWidget file_window;
   char *directory_name = NULL;
   char *host_name = NULL;

   if(dialog_data == trashDialogData)
   {
      CloseTrash(NULL, NULL, NULL);
      return;
   }

   for (i = 0; i < view_count; i++)
   {
      if (dialog_data == (DialogData *) (view_set[i]->dialog_data))
      {
         tmpDialog_data = (DialogData *) (view_set[i]->dialog_data);
         file_mgr_data = (FileMgrData *)tmpDialog_data->data;

         directory_name = (char *)XtMalloc( strlen(view_set[i]->directory_name) + 1);
         strcpy(directory_name, view_set[i]->directory_name);
         host_name = (char *)XtMalloc( strlen(view_set[i]->host_name) + 1);
         strcpy(host_name, view_set[i]->host_name);

	 if (view_set[i]->msg != 0) {
	    if (view_set[i]->pats != 0) {
	       ttdt_file_quit( view_set[i]->pats, 0 );
	       view_set[i]->pats = 0;
	    }
	    tt_message_reply( view_set[i]->msg );
	    tttk_message_destroy( view_set[i]->msg );
	    view_set[i]->msg = 0;
	 }

         XtFree ((char *) view_set[i]->directory_name);
         XtFree ((char *) view_set[i]->host_name);
         XtFree ((char *) view_set[i]);
         view_set[i] = NULL;

         for (j = i; j < view_count - 1; j++)
            view_set[j] = view_set[j + 1];

         view_count--;

         _DtHideDialog (dialog_data, True);

         file_mgr_rec = (FileMgrRec *)file_mgr_data->file_mgr_rec;
         file_window = (XmManagerWidget)file_mgr_rec->file_window;
         /* For next time, unmanage all the icons */
         XtUnmanageChildren(file_window->composite.children,
                            file_window->composite.num_children);

         if(PositionFlagSet(file_mgr_data))
            SavePositionInfo(file_mgr_data);

         /* If it is an Application Manager view, then no point in
          caching this dialog, so free it */

         if(file_mgr_data->toolbox)
            _DtFreeDialog(dialog_data);

         _DtFreeDialogData (dialog_data);

         break;
      }
   }


   /*  Update the directory cache list to remove unneeded directories.  */

   UpdateCachedDirectories (view_set, view_count);

   if (openDirType == NEW)
      ForceMyIconClosed(host_name, directory_name);

   XtFree(directory_name);
   XtFree(host_name);
}




/************************************************************************
 *
 *  DirectoryChanged
 *      This function is called when a view may be changing its
 *      current directory.  It updates the view_set list to the
 *      new host and directory.
 *
 ************************************************************************/

void
DirectoryChanged(
        XtPointer file_mgr_data,
        char *old_host_name,
        char *new_host_name,
        char *old_directory_name,
        char *new_directory_name )
{
   int i;


   /*  See if the directory is one being viewed  */

   for (i = 0; i < view_count; i++)
   {
      if (((DialogData *) (view_set[i]->dialog_data))->data == file_mgr_data)
      {
         if (strcmp (old_host_name, view_set[i]->host_name) == 0              &&
             strcmp (old_directory_name, view_set[i]->directory_name) == 0)
         {
            XtFree ((char *) view_set[i]->host_name);
            view_set[i]->host_name = XtNewString (new_host_name);

            XtFree ((char *) view_set[i]->directory_name);
            view_set[i]->directory_name = XtNewString (new_directory_name);
         }
      }
   }


   /*  Update the directory cache list to remove unneeded directories.  */

   UpdateCachedDirectories (view_set, view_count);
}



/************************************************************************
 *
 *  ObserveTtNotice
 *      Dispatch ToolTalk events to internal handlers.
 *
 ************************************************************************/

static Tt_callback_action
ObserveTtNotice(
	Tt_message msg,
	Tt_pattern pat
)
{
   char *op;
   Tt_status status;

   op = tt_message_op( msg );
   status = tt_ptr_error( op );
   if ((status != TT_OK) || (op == 0)) {
      /* Let tttk_Xt_input_handler() Do The Right Thing */
      return TT_CALLBACK_CONTINUE;
   }
   if (strcmp( op, "DtTypes_Reloaded" ) == 0) {
      ReloadDatabases();
   } else if (strcmp( op, "XSession_Ending" ) == 0) {
      if( emptyTrashOnExit )
         TrashEmpty();
   } else {
      tt_free( op );
      return TT_CALLBACK_CONTINUE;
   }
   tt_free( op );
   tttk_message_destroy( msg );
   return TT_CALLBACK_PROCESSED;
}

/************************************************************************
 *
 *  HandleTtRequest
 *      Dispatch ToolTalk requests to internal handlers.
 *
 ************************************************************************/

Tt_callback_action
HandleTtRequest(
	Tt_message msg,
	Tt_pattern pat
)
{
	char *op;
	Tt_status status;

	op = tt_message_op( msg );
	status = tt_ptr_error( op );
	if ((status != TT_OK) || (op == 0)) {
		/* Let tttk_Xt_input_handler() Do The Right Thing */
		return TT_CALLBACK_CONTINUE;
	}
	if (strcmp( op, "DtFileSession_Run" ) == 0) {
		ViewSessionHandler( msg );
	} else if (strcmp( op, "DtFolder_Show" ) == 0) {
		ViewDirectoryHandler( msg );
	} else if (strcmp( op, "DtHome_Show" ) == 0) {
		ViewHomeDirectoryHandler( msg );
	} else if (strcmp( op, "DtTools_Show" ) == 0) {
		ViewToolsDirectoryHandler( msg );
	} else if (strcmp( op, "DtTrash_Show" ) == 0) {
		TrashDisplayHandler( msg );
	} else if (strcmp( op, "DtTrash_Remove" ) == 0) {
		TrashRemoveHandler( msg );
	} else if (strcmp( op, "DtTrash_Empty" ) == 0) {
		TrashEmptyHandler( msg );
	} else if (strcmp( op, "DtTrash_File" ) == 0) {
		TrashRemoveNoConfirmHandler( msg );
    } else if (strcmp( op, "DtTrash_Restore" ) == 0) {
         TrashRestoreHandler( msg );
    } else if (strcmp( op, "DtFile_PutOnWorkspace" ) == 0) {
         PutOnWorkspaceHandler( msg );
    } else if (strcmp( op, "DtFile_Move" ) == 0) {
         MoveCopyLinkHandler( msg, MOVE_FILE );
    } else if (strcmp( op, "DtFile_Copy" ) == 0) {
         MoveCopyLinkHandler( msg, COPY_FILE );
    } else if (strcmp( op, "DtFile_Link" ) == 0) {
         MoveCopyLinkHandler( msg, LINK_FILE );
    } else {
         tt_free( op );
         return TT_CALLBACK_CONTINUE;
    }
    tt_free( op );
    return TT_CALLBACK_PROCESSED;
}




/************************************************************************
 *
 *  ViewSessionHandler
 *      This function is called upon the dt session  message
 *      being received.  The name of the session file is extracted
 *      out of the message and a function is called to create
 *      a view or set of views described by the file.
 *
 ************************************************************************/

static void
ViewSessionHandler(
   Tt_message msg)
{
   char *file = tt_message_file( msg );
   tt_message_reply( msg );
   tttk_message_destroy( msg );

   if (! tt_is_err( tt_ptr_error( file )))
   {
      if (view_count == 0 && desktop_data->numIconsUsed == 0)
      {
           int session_flag = 0;
           char *session_name;

           session_name = strrchr(file, '/');
           session_name++;
           LoadDesktopInfo(session_name);
           session_flag = RestoreSession (session_name, NORMAL_RESTORE, NULL);
           if ((session_flag != 0) && (view_count == 0))
           {
              char current_directory[MAX_PATH];

              GetPWD(current_directory);
              if (current_directory[0] != NULL)
              {
                 if (!GetNewView
                      (home_host_name, current_directory, NULL, NULL, 0))
                    ViewHomeDirectoryHandler (0);
              }
              else
              {
                 ViewHomeDirectoryHandler (0);
              }
           }
      }
      else
      {
           char *tmpStr, *msg, *title;

           tmpStr = GETMESSAGE(18, 39, "Cannot start a second File Manager session because a File Manager\nsession is already open on the desktop.  If you want to start a new File\nManager session, you must first close all current File Manager windows\nand remove all File Manager objects from the workspaces.");
           msg = XtNewString(tmpStr);
           title = XtNewString((GETMESSAGE(18,40,"Session Error")));
	   _DtMessage (toplevel, title, msg, NULL, HelpRequestCB);
           XtFree(msg);
	   XtFree(title);
      }
   }
   tt_free( file );
}




/************************************************************************
 *
 *  ViewDirectoryHandler
 *      This function is called upon the dt directory message
 *      being received.  The name of the directory is extracted
 *      out of the message and a function is called to create
 *      a view of the directory.
 *
 ************************************************************************/

static void
ViewDirectoryHandler(
   Tt_message msg)
{
   int i, j, k;
   char *msgFile;
   int numArgs;

   initiating_view = NULL;
   msgFile = tt_message_file( msg );
   if (tt_is_err( tt_ptr_error( msgFile ))) msgFile = 0;

   if(restrictMode)
   {
      if(msgFile && strncmp(msgFile, "~", 1) != 0 )
      {
         if((strcmp(users_home_dir, "/") != 0) &&
            (strncmp(msgFile, users_home_dir, strlen(users_home_dir)-1) != 0))
         {
            char *tmpStr, *errTitle, *errMsg, *dmsg;

            tmpStr = GETMESSAGE(32, 2, "File Manager Open Directory Error");
            errTitle = XtNewString(tmpStr);
            tmpStr = GETMESSAGE(2,20,"You cannot switch to folder:\n\n%s\n\nYou are not allowed to view this folder\nbecause it is a restricted folder.");
            errMsg = XtNewString(tmpStr);
            dmsg = XtMalloc(strlen(errMsg)+strlen(msgFile)+1);
            sprintf(dmsg, errMsg, msgFile);
            _DtMessage(toplevel, errTitle, dmsg, NULL, HelpRequestCB);

            XtFree(errTitle);
            XtFree(errMsg);
            XtFree(dmsg);

            tt_free( msgFile );
            tt_message_reply( msg );
            tttk_message_destroy( msg );
            return;
         }
      }
   }

   numArgs = tt_message_args_count( msg );
   if (tt_is_err( tt_int_error( numArgs ))) numArgs = 0;
   if (numArgs > 0)
   {
      special_view = True;
      special_treeType = UNSET_VALUE;
      special_treeFiles = UNSET_VALUE;
      special_viewType = UNSET_VALUE;
      special_orderType = UNSET_VALUE;
      special_directionType = UNSET_VALUE;
      special_randomType = UNSET_VALUE;
      special_restricted = NULL;
      special_title = NULL;
      special_helpVol = XtNewString(DTFILE_HELP_NAME);
      for(i = 0; i < numArgs; i++)
      {
	 char *vtype;
	 char *val;
	 vtype = tt_message_arg_type( msg, i );
	 if ((vtype == 0) || (tt_is_err( tt_ptr_error( vtype )))) {
	    continue;
         }
	 val = tt_message_arg_val( msg, i );
         if(strcmp(vtype, "-title") == 0)
         {
            special_title = XtNewString(val);
         }
         else if(strcmp(vtype, "-help_volume") == 0)
         {
            special_helpVol = XtNewString(val);
         }
         if(strcmp(vtype, "-tree") == 0)
         {
            DtfileStringToTree(val, &special_treeType);
         }
         else if(strcmp(vtype, "-tree_files") == 0)
         {
            DtfileStringToTreeFiles(val, &special_treeFiles);
         }
         else if(strcmp(vtype, VIEW_HEADER) == 0)
         {
            DtfileStringToView(val, &special_viewType);
         }
         else if(strcmp(vtype, "-order") == 0)
         {
            DtfileStringToOrder(val, &special_orderType);
         }
         else if(strcmp(vtype, "-direction") == 0)
         {
            DtfileStringToDirection(val, &special_directionType);
         }
         else if(strcmp(vtype, "-grid") == 0)
         {
            DtfileStringToGrid(val, &special_randomType);
         }
         else if(strcmp(vtype, RESTRICTED_HEADER) == 0)
         {
            special_restricted = XtNewString(msgFile);
         }
	 tt_free( val );
	 tt_free( vtype );

         if ((restrictMode) && (!special_restricted))
         {
            char *ptr;

            ptr = strrchr(users_home_dir, '/');
            *ptr = '\0';
            special_restricted = XtNewString(users_home_dir);
            *ptr = '/';
         }
      }
   }
   else if (restrictMode)
   {
     char *ptr;

     special_view = True;
     special_treeType = treeType;
     special_treeFiles = treeFiles;
     special_viewType = viewType;
     special_orderType = orderType;
     special_directionType = directionType;
     special_randomType = randomType;

     ptr = strrchr(users_home_dir, '/');
     *ptr = '\0';
     special_restricted = XtNewString(users_home_dir);
     *ptr = '/';

     special_title = NULL;
     special_helpVol = XtNewString(DTFILE_HELP_NAME);

   }
   else
      special_view = False;

   OpenDirectories (msgFile, NULL);

   tt_free( msgFile );
   tt_message_reply( msg );
   tttk_message_destroy( msg );
}




/************************************************************************
 *
 *  ViewDirectoryProc
 *      This function is called upon the dt directory message
 *      being received.  The name of the directory is extracted
 *      out of the message and a function is called to create
 *      a view of the directory.  This function is passed directly
 *	to DtActionInvoke to be used as a callback.
 *
 ************************************************************************/

DialogData *
ViewDirectoryProc(
   char* root_dir,
   int restricted,
   Tt_message msg
)
{
   DialogData *return_data;

   if(restrictMode)
   {
      if(root_dir && strncmp(root_dir, "~", 1) != 0 )
      {
         if((strcmp(users_home_dir, "/") != 0) &&
            (strncmp(root_dir, users_home_dir, strlen(users_home_dir)-1) != 0))
         {
            char *tmpStr, *errTitle, *errMsg, *dmsg;

            tmpStr = GETMESSAGE(32, 2, "File Manager Open Directory Error");
            errTitle = XtNewString(tmpStr);
            tmpStr = GETMESSAGE(2,20,"You cannot switch to folder:\n\n%s\n\nYou are not allowed to view this folder\nbecause it is a restricted folder.");
            errMsg = XtNewString(tmpStr);
            dmsg = XtMalloc(strlen(errMsg)+strlen(root_dir)+1);
            sprintf(dmsg, errMsg, root_dir);
            _DtMessage(toplevel, errTitle, dmsg, NULL, HelpRequestCB);

            XtFree(errTitle);
            XtFree(errMsg);
            XtFree(dmsg);
            return;
         }
      }
   }

   initiating_view = NULL;
   special_view = True;
   special_treeType = treeType;
   special_treeFiles = treeFiles;
   special_viewType = viewType;
   special_orderType = orderType;
   special_directionType = directionType;
   special_randomType = randomType;

   if (restricted) {
     special_restricted = XtNewString(root_dir);
   }
   else {
     special_restricted = NULL;
   }

   if ((restrictMode) && (!special_restricted))
   {
      char *ptr;

      ptr = strrchr(users_home_dir, '/');
      *ptr = '\0';
      special_restricted = XtNewString(users_home_dir);
      *ptr = '/';
   }

   special_title = NULL;
   special_helpVol = XtNewString(DTFILE_HELP_NAME);

   return_data = GetNewView (home_host_name, root_dir, NULL, NULL, msg);

   if ((!return_data) && msg)
   {
      tt_message_reply(msg);
      tttk_message_destroy( msg );
   }

   return(return_data);
}



/************************************************************************
 *
 *  ViewHomeDirectoryHandler
 *      This function is called upon the dt home directory message
 *      being received.  The home directory is extracted from the
 *      users uid and used to open the directory.
 *
 ************************************************************************/


static void
ViewHomeDirectoryHandler(
   Tt_message msg)
{
   register int i, j, k;
   struct passwd * pwInfo;
   char * homeDir;
   char * tmp_path;
   char full_path[MAX_PATH + 1];
   struct stat stat_buf;
   int numArgs;

   /* We have a new view so set initiating_view to null */
   initiating_view = NULL;

   if ((homeDir = getenv("HOME")) == NULL || strlen (homeDir) == 0)
   {
      pwInfo = getpwuid (getuid());
      homeDir = pwInfo->pw_dir;
   }

   /* Look and see if we have a default environment file present */
   tmp_path = _DtCreateDtDirs(display);

   numArgs = 0;
   if (msg) numArgs = tt_message_args_count(msg);
   if (tt_is_err(tt_int_error(numArgs))) numArgs = 0;
   if (numArgs > 0)
   {
      special_view = True;
      special_treeType = UNSET_VALUE;
      special_treeFiles = UNSET_VALUE;
      special_viewType = UNSET_VALUE;
      special_orderType = UNSET_VALUE;
      special_directionType = UNSET_VALUE;
      special_randomType = UNSET_VALUE;
      special_restricted = NULL;
      special_title = NULL;
      special_helpVol = XtNewString(DTFILE_HELP_NAME);
      for(i = 0; i < numArgs; i++)
      {
	 char *vtype;
	 char *val;
	 vtype = tt_message_arg_type( msg, i );
	 if ((vtype == 0) || (tt_is_err( tt_ptr_error( vtype )))) {
	    continue;
         }
	 val = tt_message_arg_val(msg, i);
	 if(strcmp(vtype, "-title") == 0)
	 {
	    special_title = XtNewString(val);
	 }
	 else if(strcmp(vtype, "-help_volume") == 0)
	 {
	    special_helpVol = XtNewString(val);
	 }
	 if(strcmp(vtype, "-tree") == 0)
	 {
	    DtfileStringToTree(val, &special_treeType);
	 }
	 else if(strcmp(vtype, "-tree_files") == 0)
	 {
	    DtfileStringToTreeFiles(val, &special_treeFiles);
	 }
	 else if(strcmp(vtype, VIEW_HEADER) == 0)
	 {
	    DtfileStringToView(val, &special_viewType);
	 }
	 else if(strcmp(vtype, "-order") == 0)
	 {
	    DtfileStringToOrder(val, &special_orderType);
	 }
	 else if(strcmp(vtype, "-direction") == 0)
	 {
	    DtfileStringToDirection(val, &special_directionType);
	 }
	 else if(strcmp(vtype, "-grid") == 0)
	 {
	    DtfileStringToGrid(val, &special_randomType);
	 }
	 else if(strcmp(vtype, RESTRICTED_HEADER) == 0)
	 {
	    special_restricted = XtNewString(val);
	 }
	 tt_free( val );
	 tt_free( vtype );
      }
   }
   else if (restrictMode)
   {
     char *ptr;

     special_view = True;
     special_treeType = treeType;
     special_treeFiles = treeFiles;
     special_viewType = viewType;
     special_orderType = orderType;
     special_directionType = directionType;
     special_randomType = randomType;

     ptr = strrchr(users_home_dir, '/');
     *ptr = '\0';
     special_restricted = XtNewString(users_home_dir);
     *ptr = '/';

     special_title = NULL;
     special_helpVol = XtNewString(DTFILE_HELP_NAME);
   }
   else
      special_view = False;

   if (tmp_path == NULL)
      OpenDirectories (homeDir, NULL);
   else
   {
     sprintf(full_path, "%s/%s", tmp_path, HOME_SETTINGS_FILENAME);
     XtFree(tmp_path);

     /* Stat the file and make sure its there */
     if(numArgs > 0)
     {
        if (stat (full_path, &stat_buf) == 0 && numArgs < 1)
        {
           restoreType = HOME_RESTORE;
           RestoreSession(full_path, HOME_RESTORE, NULL);
           restoreType = NORMAL_RESTORE;
        }
        else
        {
           char *ptr;

           ptr = strrchr(users_home_dir, '/');
           *ptr = '\0';
           strcpy(full_path, users_home_dir);
           *ptr = '/';
           OpenDirectories (full_path, NULL);
        }
     }
     else if (numArgs < 1 && stat (full_path, &stat_buf) == 0)
     {
        restoreType = HOME_RESTORE;
        RestoreSession(full_path, HOME_RESTORE, NULL);
        restoreType = NORMAL_RESTORE;
     }
     else
     {
       char *ptr;

       ptr = strrchr(users_home_dir, '/');

       if( ptr != users_home_dir )
         *ptr = '\0';

       strcpy(full_path, users_home_dir);
       *ptr = '/';

       OpenDirectories (full_path, NULL);
     }
   }

   if(msg)
   {
     tt_message_reply(msg);
     tttk_message_destroy(msg);
   }
}

/************************************************************************
 *
 *  ViewToolsDirectroyHandler
 *      This function is called upon the dt tools message
 *      being received.  The names of the tools directories are
 *      retrieved from libXue, a view is created and the change
 *      directory dialog for the view is displayed with the
 *      set of directories contained in the list.
 *
 ************************************************************************/

static void
ViewToolsDirectoryHandler(
   Tt_message msg)
{
   register int i;
   char * tmp_path = NULL;
   char * tool_dir = NULL;
   char * msgFile = NULL;
   int numArgs = 0;

   /* We have a new view so set initiating_view to null */
   initiating_view = NULL;

   /* Look and see if we have a default environment file present */
   tmp_path = _DtCreateDtDirs(display);

   if (msg) msgFile = tt_message_file(msg);
   if (tt_is_err(tt_ptr_error(msgFile))) msgFile = 0;

   if (msg) numArgs = tt_message_args_count(msg);
   if (tt_is_err(tt_int_error( numArgs ))) numArgs = 0;

   if(msgFile != NULL)
   {
      if(numArgs > 0)
      {
         char *link_point = NULL;
         char *root_toolbox = NULL;
         char *user_install_point = NULL;

         special_view = True;
         special_treeType = treeType;
         special_treeFiles = treeFiles;
         special_viewType = viewType;
         special_orderType = orderType;
         special_directionType = directionType;
         special_randomType = randomType;
         special_restricted = XtNewString(msgFile);
         special_title = NULL;
         special_helpVol = XtNewString(DTFILE_HELP_NAME);
         for(i = 0; i < numArgs; i++)
         {
            char *vtype;
            char *val;
            vtype = tt_message_arg_type( msg, i );
            if ((vtype == 0) || (tt_is_err( tt_ptr_error( vtype )))) {
               continue;
            }
            val = tt_message_arg_val( msg, i );
            if(strcmp(vtype, "-title") == 0)
            {
               special_title = XtNewString(val);
            }
            else if(strcmp(vtype, "-help_volume") == 0)
            {
               special_helpVol = XtNewString(val);
            }
            else if(strcmp(vtype, "-root") == 0)
            {
               root_toolbox = XtNewString(val);
            }
            else if(strcmp(vtype, "-common_link_point") == 0)
            {
               link_point = XtNewString(val);
            }
            else if(strcmp(vtype, "-user_install_point") == 0)
            {
               user_install_point = XtNewString(val);
            }
            if(strcmp(vtype, "-tree") == 0)
            {
               DtfileStringToTree(val, &special_treeType);
            }
            else if(strcmp(vtype, "-tree_files") == 0)
            {
               DtfileStringToTreeFiles(val, &special_treeFiles);
            }
            else if(strcmp(vtype, VIEW_HEADER) == 0)
            {
               DtfileStringToView(val, &special_viewType);
            }
            else if(strcmp(vtype, "-order") == 0)
            {
               DtfileStringToOrder(val, &special_orderType);
            }
            else if(strcmp(vtype, "-direction") == 0)
            {
               DtfileStringToDirection(val, &special_directionType);
            }
            else if(strcmp(vtype, "-grid") == 0)
            {
               DtfileStringToGrid(val, &special_randomType);
            }
            else if(strcmp(vtype, RESTRICTED_HEADER) == 0)
            {
               ;
            }
            tt_free( val );
            tt_free( vtype );
         }
         /* Due to the use of tooltalk messaging, app manager objects
            that are dragged either to the desktop or to the front
            panel are identified by their fully resolved names (ie.
            /usr/dt/appconfig/appmanager/$LANG/.... as opposed to
            /var/dt/appconfig/appmanager/$DTUSERSESSION/....).  In order
            for the File Manager to treat these objects as links existing
            in the /var/.... directory, we use the following ugly code:

              1. Search the incoming path for $LANG (if you can't find
                 $LANG, try C since this is the default).
              2. Concatenate the path following $LANG to the path for the
                 root toolbox (this comes in with the tooltalk message
                 so that we don't have to hardcode it).
              3. Call OpenDirectories with the new path that you have
                 created and with the root toolbox path as the restricted
                 directory.

            This problem is further complicated by:
              1. Users traversing into APPGROUPs.
              2. Users opening the parent folders for workspace APPGROUPs.

            For situation 1, File Manager kicks the user into the App Manager
            if he/she traverses into an APPGROUP.  BUT we don't want to
            make modifications to the path as we do above.
            So, if there is nothing following $LANG or if we can't find
            $LANG in the path,
            (ie. /var/dt/appconfig/appmanager/$DTUSERSESSION), call
            OpenDirectories with no parameter changes.

            For situation 2, File Manager is unable to distinguish between
            workspace objects dragged from /usr/.... or /var/...., so
            the parent folder for all workspace APPGROUPS is considered to
            be in the /var path.  In addition to the OpenAppGroup action,
            there is an OpenParentAppGroup action which also triggers
            ViewToolsDirectoryHandler but sends in an additional
            parameter (user_install_point).
            If we know that this msg was generated by the OpenParentAppGroup
            action
            AND nothing follows $LANG.
              Modify the parameters as above.
            OR we can't find $LANG but we can find the user_install_point
              (user's personal appgroup path).
              Concatenate the path following .dt/appmanager to the path for the
              root toolbox. Call OpenDirectories with the new path and the
              root toolbox as the restricted directory.
         */
         if( root_toolbox )
         {
            if( link_point )
            {
               char *ptr;

               if ((user_install_point) &&
                   (ptr = strstr(msgFile, user_install_point)))
               {
                  ptr += strlen(user_install_point);
                  tool_dir = XtMalloc(strlen(root_toolbox) + strlen(ptr) + 1);
                  sprintf(tool_dir, "%s%s", root_toolbox, ptr);
                  XtFree(special_restricted);
                  special_restricted = XtNewString(root_toolbox);
               }
               else
               {
                  ptr = strstr(msgFile, link_point);

                  if (!ptr)
                  {
                     XtFree(link_point);
                     link_point = XtNewString("C");
                     ptr = strstr(msgFile, link_point);
                  }

                  if (ptr)
                  {
                     ptr += strlen(link_point);
                     if (strcmp(ptr, "") != 0)
                     {
                        tool_dir = XtMalloc(strlen(root_toolbox) +
                                            strlen(ptr) + 1);
                        sprintf(tool_dir, "%s%s", root_toolbox, ptr);
                        XtFree(special_restricted);
                        special_restricted = XtNewString(root_toolbox);
                     }
                     else if (user_install_point)
                     {
                        tool_dir = XtNewString(root_toolbox);
                        XtFree(special_restricted);
                        special_restricted = XtNewString(root_toolbox);
                     }
                  }
               }
            }
         }
         XtFree(link_point);
         XtFree(root_toolbox);
         XtFree(user_install_point);
      }
      else if (restrictMode)
      {
         char *ptr;

         special_view = True;
         ptr = strrchr(users_home_dir, '/');
         *ptr = '\0';
         special_restricted = XtNewString(users_home_dir);
         *ptr = '/';
      }
      else
         special_view = False;
   }

   if (tmp_path == NULL)
   {
      if (tool_dir)
         OpenDirectories (tool_dir, special_restricted);
      else if (msgFile != NULL)
         OpenDirectories (msgFile, special_restricted);
   }
   else
   {
      char full_path[MAX_PATH + 1];
      struct stat stat_buf;

      sprintf(full_path, "%s/%s", tmp_path, TOOL_SETTINGS_FILENAME);
      XtFree(tmp_path);

      /* Stat the file and make sure its there */
      if (stat (full_path, &stat_buf) == 0 && numArgs > 0)
      {
         DialogData * dialog_data;
         FileMgrData * file_mgr_data;

         restoreType = TOOL_RESTORE;
         if (tool_dir)
            RestoreSession(full_path, TOOL_RESTORE, tool_dir);
         else
            RestoreSession(full_path, TOOL_RESTORE, msgFile);
         dialog_data = (DialogData *)view_set[view_count - 1]->dialog_data;
         file_mgr_data = (FileMgrData *) dialog_data->data;
         application_args.tool_width = file_mgr_data->width;
         application_args.tool_height = file_mgr_data->height;
         restoreType = NORMAL_RESTORE;
      }
      else
      {
         if (tool_dir)
            OpenDirectories (tool_dir, special_restricted);
         else if (msgFile != NULL)
            OpenDirectories (msgFile, special_restricted);
      }
   }

   if( msgFile )
     tt_free( msgFile );

   if( msg )
   {
     tt_message_reply( msg );
     tttk_message_destroy( msg );
   }

   if (tool_dir)
      XtFree(tool_dir);
}


/************************************************************************
 *
 *  ExitHandler
 *      This function is called upon the DtSTOP message.
 *
 ************************************************************************/

static void
ExitHandler(
   Tt_message msg,
   XtPointer clientData,
   String * messageFields,
   int numFields)
{
   tt_message_reply( msg );
   tttk_message_destroy( msg );
   FinalizeToolTalkSession( );
   exit(0);
}


/*
 * This is the message handling function responsible for reloading
 * the filetype and action databases, and then updating our collection
 * of action menu items, and updating all open views.
 */

static void
ReloadDatabases(void)
{
   int i;
   DialogData * dialog_data;
   FileMgrData * file_mgr_data;
   FileMgrRec  * file_mgr_rec;
   FileViewData *file_view_data;
   DesktopRec *desktopWindow;

   DtDbLoad();

   XmeFlushIconFileCache ( NULL );

   /* Update each directory_set view, both mapped and not */
   UpdateDirectorySet();

   /* Force action menus to update the next time they're posted */
   XtFree(fileMgrPopup.action_pane_file_type);
   fileMgrPopup.action_pane_file_type = NULL;
   XtFree(desktop_data->popupMenu->action_pane_file_type);
   desktop_data->popupMenu->action_pane_file_type = NULL;

   /* Update each view */
   for (i = 0; i < view_count; i++)
   {
      dialog_data = (DialogData *)view_set[i]->dialog_data;
      file_mgr_data = (FileMgrData *) dialog_data->data;
      file_mgr_rec = (FileMgrRec *) file_mgr_data->file_mgr_rec;
      XtFree(file_mgr_rec->action_pane_file_type);
      file_mgr_rec->action_pane_file_type = NULL;
      UpdateFilterAfterDBReread(file_mgr_data->filter_active);
      UpdateFilterAfterDBReread(file_mgr_data->filter_edit);
      FileMgrRedisplayFiles(file_mgr_rec, file_mgr_data, False);
   }

   /* go through the desktop objects to make sure the icons change
    * Do this by setting the logical_type to -l, CheckDesktop will
    * then update the logical type and the icon
    */
   for(i = 0; i < desktop_data->numIconsUsed; i++)
   {
      desktopWindow = desktop_data->desktopWindows[i];
      file_view_data = desktopWindow->file_view_data;

      file_view_data->file_data->logical_type = NULL;
   }
   CheckDesktop();
}


/*
 * Whenever a directory view or drawer view is closed, we need to remove
 * any of its text field children, so that they will not magically reappear
 * should this view be reused from the cache to view the same directory later.
 */

static void
RemoveTextFields (
   XtPointer client_data,
   DialogData * old_dialog_data,
   DialogData * new_dialog_data)
{
   FileMgrRec * file_mgr_rec;
   XmManagerWidget file_window;
   int i;
   int num_children;

   file_mgr_rec = (FileMgrRec *) _DtGetDialogInstance(old_dialog_data);
   file_window = (XmManagerWidget) file_mgr_rec->file_window;
   num_children = file_window->composite.num_children;

   for (i = 0; i < num_children; i++)
   {
      if (XmIsTextField(file_window->composite.children[i]))
         XtDestroyWidget(file_window->composite.children[i]);
   }

   _DtFreeDialogData (new_dialog_data);
}


/*
 * This function searches the view list, and returns the file_mgr_data
 * associated with the passed-in widget.
 */

FileMgrData *
ReturnDesktopPtr (
   Widget w)
{
   int i;
   DialogData * dialog_data;
   FileMgrData * file_mgr_data;

   for (i = 0; i < view_count; i++)
   {
      dialog_data = (DialogData *) view_set[i]->dialog_data;
      file_mgr_data = (FileMgrData *) dialog_data->data;

      if (w == ((FileMgrRec *)file_mgr_data->file_mgr_rec)->file_window ||
          w == ((FileMgrRec *)file_mgr_data->file_mgr_rec)->shell)
         return(file_mgr_data);
   }

   return(NULL);
}


PixmapData *
CheckForOpenDirectory(
     FileViewData *order_list,
     DirectorySet *directory_set,
     FileMgrData *file_mgr_data,
     char * logical_type)
{
   int i;
   char *ptr;
   char *file_type_name, *new_file_type_name;
   char directory_name[MAX_PATH];
   char * real_dir_name;
   FileMgrRec *file_mgr_rec;
   int icon_size;
   PixmapData *pixmapData = NULL;

   if (file_mgr_data->view == BY_NAME_AND_ICON)
     icon_size = LARGE;
   else
     icon_size = SMALL;

   file_mgr_rec = (FileMgrRec *)file_mgr_data->file_mgr_rec;

   if (strcmp (directory_set->name, "/") != 0)
     sprintf( directory_name, "%s/%s", directory_set->name, order_list->file_data->file_name);
   else
     sprintf( directory_name, "%s%s", directory_set->name, order_list->file_data->file_name );

   (void) DtEliminateDots (directory_name);

   real_dir_name = XtNewString(directory_name);

   for(i = 0; i < view_count; i++)
   {
      if(strcmp(real_dir_name, view_set[i]->directory_name) == 0)
      {
         file_type_name = order_list->file_data->logical_type;
         new_file_type_name = (char *)XtMalloc(strlen(file_type_name)
                                               + strlen(ICON_OPEN_PREFIX) + 1);
         sprintf(new_file_type_name, "%s%s", ICON_OPEN_PREFIX, file_type_name);
         pixmapData = _DtRetrievePixmapData(new_file_type_name,
                                            NULL,
                                            NULL,
                                            file_mgr_rec->file_window,
                                            icon_size);
         XtFree(new_file_type_name);
         break;
      }
   }
   XtFree(real_dir_name);

   if(pixmapData == NULL || pixmapData->iconFileName == NULL)
   {
     if( pixmapData != NULL )
     {
       DtDtsFreeAttributeValue(pixmapData->hostPrefix);
       DtDtsFreeAttributeValue(pixmapData->instanceIconName);
       DtDtsFreeAttributeValue(pixmapData->iconName);
       XtFree((char *)pixmapData);
     }

       pixmapData = _DtRetrievePixmapData(logical_type,
                                          NULL,
                                          NULL,
                                          file_mgr_rec->file_window,
                                          icon_size);
   }

   return(pixmapData);
}


/*
 * The is the menu callback function for cleaning up a view.
 */

void
CleanUp (
   Widget w,
   XtPointer client_data,
   XtPointer call_data)
{
   Widget mbar;
   Arg args[2];
   FileMgrRec * file_mgr_rec;
   DialogData * dialog_data;
   FileMgrData * file_mgr_data;
   int i, j;
   XmManagerWidget file_window;
   char * name;
   FileViewData * file_view_data;
   XRectangle textExtent;
   Position x, y;

   if ((int) client_data == FM_POPUP)
     mbar = XtParent(w);
   else
     mbar = XmGetPostedFromWidget(XtParent(w));

   XmUpdateDisplay(w);
   XtSetArg(args[0], XmNuserData, &file_mgr_rec);
   XtGetValues(mbar, args, 1);

   /* Ignore accelerators when we're insensitive */
   if ((file_mgr_rec->menuStates & CLEAN_UP) == 0)
      return;

   /* Ignore accelerators received after we're unposted */
   if ((dialog_data = _DtGetInstanceData((XtPointer)file_mgr_rec)) == NULL)
      return;

   file_mgr_data = (FileMgrData *)dialog_data->data;

   /* Reset the grid size, so it will be recalculated later. */
   file_mgr_data->grid_height = 0;
   file_mgr_data->grid_width = 0;

   if(file_mgr_data->object_positions)
      FreePositionInfo(file_mgr_data);

   /* CLEAN_UP_OP is not really a menu state.
      It's a flag to let GetFileData in FileMgr.c (call when the directory
      is being reread) not to reload icon positions from the .!dt<userid> file.
   */
   file_mgr_rec->menuStates |= CLEAN_UP_OP;

   /* Re-layout the view */
   FileMgrRedisplayFiles(file_mgr_rec, file_mgr_data, False);

   if ((file_mgr_data->show_type == SINGLE_DIRECTORY) &&
        (file_mgr_data->view != BY_ATTRIBUTES) &&
        (file_mgr_data->positionEnabled == RANDOM_ON))
   {
      ((PreferencesData *)(file_mgr_data->preferences->data))->positionEnabled =
          RANDOM_ON;
   }
   else
   {
      /* Update the preferences dialog */
      ((PreferencesData *)(file_mgr_data->preferences->data))->positionEnabled =
          RANDOM_OFF;
   }

   /* Move any text widget, to keep them in sync with their icons */
   file_window = (XmManagerWidget)file_mgr_rec->file_window;

   for (j = 0; j < file_window->composite.num_children; j++)
   {
      if (XmIsTextField(file_window->composite.children[j]) &&
          !file_window->composite.children[j]->core.being_destroyed)
      {
         XtSetArg(args[0], XmNuserData, &name);
         XtGetValues(file_window->composite.children[j], args, 1);

         /* Find the associated icon data */
         /* @@@ this won't work for tree mode! */
         for (i = 0; i < file_mgr_data->directory_set[0]->file_count; i++)
         {
            file_view_data = file_mgr_data->directory_set[0]->file_view_data[i];
            if (strcmp(name, file_view_data->file_data->file_name) == 0)
            {
               _DtIconGetTextExtent_r(file_view_data->widget, &textExtent);
               x = textExtent.x;
               y = textExtent.y -
                   (Dimension)(file_window->composite.children[j]->core.height -
                    textExtent.height)/(Dimension)2;
               XtSetArg (args[0], XmNx, x);
               XtSetArg (args[1], XmNy, y);
               XtSetValues (file_window->composite.children[j], args, 2);
               break;
            }
         }
      }
   }

   if ((file_mgr_rec->menuStates & CLEAN_UP_OP))
     file_mgr_rec->menuStates  &= ~CLEAN_UP_OP;
}

/*************************************<->*************************************
 *
 *  DtfileCvtStringToObjPlace (args, numArgs, fromVal, toVal)
 *
 *
 *  Description:
 *  -----------
 *  This function converts a string to an desktop placement scheme description.
 *
 *
 *  Inputs:
 *  ------
 *  args = NULL (don't care)
 *
 *  numArgs = 0 (don't care)
 *
 *  fromVal = resource value to convert
 *
 *
 *  Outputs:
 *  -------
 *  toVal = descriptor to use to return converted value
 *
 *************************************<->***********************************/

static void
DtfileCvtStringToObjPlace (
          XrmValue *args,
          Cardinal numArgs,
          XrmValue *fromVal,
          XrmValue *toVal)
{
    unsigned char       *pch = (unsigned char *) (fromVal->addr);
    unsigned char       *pchNext;
    int         len;
    static long cval;
    char tmp[2];
    Boolean     fPrimarySet = False;
    Boolean     fSecondarySet = False;

/*
 * Icon placement layout values:
 */

#define OBJ_PLACE_BOTTOM_STR           (unsigned char *)"bottom"
#define OBJ_PLACE_LEFT_STR             (unsigned char *)"left"
#define OBJ_PLACE_RIGHT_STR            (unsigned char *)"right"
#define OBJ_PLACE_TOP_STR              (unsigned char *)"top"


    /*
     * Convert the icon placement resource value:
     */

    cval = 0;

    while (*pch && _DtNextToken (pch, &len, &pchNext))
    {
        tmp[0] = pch[len];
        pch[len] = '\0';
        switch( *pch )
        {
            case 'B':
            case 'b':
                if (_DtStringsAreEquivalent((char *)pch, (char *)OBJ_PLACE_BOTTOM_STR))
                {
                  if (!fPrimarySet)
                  {
                    cval |= OBJ_PLACE_BOTTOM_PRIMARY;
                    fPrimarySet = True;
                  }
                  else if (!fSecondarySet)
                  {
                    if (!(cval &
                          (OBJ_PLACE_BOTTOM_PRIMARY | OBJ_PLACE_TOP_PRIMARY)))
                    {
                      cval |= OBJ_PLACE_BOTTOM_SECONDARY;
                      fSecondarySet = True;
                    }
                  }
                }
                break;


            case 'L':
            case 'l':
                if (_DtStringsAreEquivalent ((char *)pch, (char *)OBJ_PLACE_LEFT_STR))
                {
                  if (!fPrimarySet)
                  {
                    cval |= OBJ_PLACE_LEFT_PRIMARY;
                    fPrimarySet = True;
                  }
                  else if (!fSecondarySet)
                  {
                    if (!(cval &
                          (OBJ_PLACE_LEFT_PRIMARY | OBJ_PLACE_RIGHT_PRIMARY)))
                    {
                      cval |= OBJ_PLACE_LEFT_SECONDARY;
                      fSecondarySet = True;
                    }
                  }
                }
                break;

            case 'R':
            case 'r':
                if (_DtStringsAreEquivalent ((char *)pch, (char *)OBJ_PLACE_RIGHT_STR))
                {
                  if (!fPrimarySet)
                  {
                    cval |= OBJ_PLACE_RIGHT_PRIMARY;
                    fPrimarySet = True;
                  }
                  else if (!fSecondarySet)
                  {
                    if (!(cval &
                          (OBJ_PLACE_RIGHT_PRIMARY | OBJ_PLACE_LEFT_PRIMARY)))
                    {
                      cval |= OBJ_PLACE_RIGHT_SECONDARY;
                      fSecondarySet = True;
                    }
                  }
                }
                break;

            case 'T':
            case 't':
                if (_DtStringsAreEquivalent ((char *)pch, (char *)OBJ_PLACE_TOP_STR))
                {
                  if (!fPrimarySet)
                  {
                    cval |= OBJ_PLACE_TOP_PRIMARY;
                    fPrimarySet = True;
                  }
                  else if (!fSecondarySet)
                  {
                    if (!(cval &
                          (OBJ_PLACE_TOP_PRIMARY | OBJ_PLACE_BOTTOM_PRIMARY)))
                    {
                      cval |= OBJ_PLACE_TOP_SECONDARY;
                      fSecondarySet = True;
                    }
                  }
                }

                break;
        }
        pch[len] = tmp[0];
        pch = pchNext;
    }

    if (!fPrimarySet)
    {
        cval =  OBJ_PLACE_TOP_PRIMARY;
    }
    if (!fSecondarySet)
    {
        if (cval & (OBJ_PLACE_LEFT_PRIMARY | OBJ_PLACE_RIGHT_PRIMARY))
        {
            cval |= OBJ_PLACE_TOP_SECONDARY;
        }
        else        {
            cval |= OBJ_PLACE_RIGHT_SECONDARY;
        }
    }


    (*toVal).size = sizeof (long);
    (*toVal).addr = (XtPointer) &cval;

} /* END OF FUNCTION DtfileCvtStringToObjPlace */

/*************************************<->*************************************
 *
 *  _DtNextToken (pchIn, pLen, ppchNext)
 *
 *
 *  Description:
 *  -----------
 *  XXDescription ...
 *
 *
 *  Inputs:
 *  ------
 *  pchIn = pointer to start of next token
 *
 *
 *  Outputs:
 *  -------
 *  pLen  =    pointer to integer containing number of characters in next token
 *  ppchNext = address of pointer to following token
 *
 *  Return =   next token or NULL
 *
 *
 *  Comments:
 *  --------
 *  None.
 *
 *************************************<->***********************************/

static unsigned char
*_DtNextToken (
        unsigned char *pchIn,
        int *pLen,
        unsigned char **ppchNext)
{
    unsigned char *pchR = pchIn;
    register int   i;
#ifdef MULTIBYTE
    register int   chlen;

    for (i = 0; ((chlen = mblen ((char *)pchIn, MB_CUR_MAX)) > 0); i++)
    /* find end of word: requires singlebyte whitespace terminator */
    {
        if ((chlen == 1) && isspace (*pchIn))
        {
            break;
        }
        pchIn += chlen;
    }

#else
    for (i = 0; *pchIn && !isspace (*pchIn); i++, pchIn++)
    /* find end of word */
    {
    }
#endif

    /* skip to next word */
#ifdef MULTIBYTE
    while (pchIn && (mblen ((char *)pchIn, MB_CUR_MAX) == 1) && isspace (*pchIn))
#else
    while (pchIn && isspace (*pchIn))
#endif
    {
        *pchIn++;
    }

    *ppchNext = pchIn;
    *pLen = i;
    if (i)
    {
        return(pchR);
    }
    else
    {
       return(NULL);
    }

} /* END OF FUNCTION _DtNextToken */

/*************************************<->*************************************
 *
 *  DtfileCvtStringToOpenDir (args, numArgs, fromVal, toVal)
 *
 *
 *  Description:
 *  -----------
 *  This function converts a string to an desktop placement scheme description.
 *
 *
 *  Inputs:
 *  ------
 *  args = NULL (don't care)
 *
 *  numArgs = 0 (don't care)
 *
 *  fromVal = resource value to convert
 *
 *
 *  Outputs:
 *  -------
 *  toVal = descriptor to use to return converted value
 *
 *************************************<->***********************************/

static void
DtfileCvtStringToOpenDir (
          XrmValue *args,
          Cardinal numArgs,
          XrmValue *fromVal,
          XrmValue *toVal)
{
   char * in_str = (char *) (fromVal->addr);
   static int i;

   toVal->size = sizeof (int);
   toVal->addr = (XtPointer) &i;

   if (_DtStringsAreEquivalent (in_str, "current"))
      i = CURRENT;
   else if (_DtStringsAreEquivalent (in_str, "new"))
      i = NEW;
   else
      i = CURRENT;
/*
   else
   {
      toVal->size = 0;
      toVal->addr = NULL;
      XtStringConversionWarning ((char *)fromVal->addr, "OpenDir");
   }
*/
}

/*************************************<->*************************************
 *
 *  DtfileCvtStringToDTIcon (args, numArgs, fromVal, toVal)
 *
 *
 *  Description:
 *  -----------
 *  This function converts a string to an desktop placement scheme description.
 *
 *
 *  Inputs:
 *  ------
 *  args = NULL (don't care)
 *
 *  numArgs = 0 (don't care)
 *
 *  fromVal = resource value to convert
 *
 *
 *  Outputs:
 *  -------
 *  toVal = descriptor to use to return converted value
 *
 *************************************<->***********************************/

static void
DtfileCvtStringToDTIcon (
          XrmValue *args,
          Cardinal numArgs,
          XrmValue *fromVal,
          XrmValue *toVal)
{
   char * in_str = (char *) (fromVal->addr);
   static int i;

   toVal->size = sizeof (int);
   toVal->addr = (XtPointer) &i;

   if (_DtStringsAreEquivalent (in_str, "large"))
      i = LARGE;
   else if (_DtStringsAreEquivalent (in_str, "small"))
      i = SMALL;
   else if (_DtStringsAreEquivalent (in_str, "default"))
      i = LARGE;  /* for now, eventually want to look at screen width/height*/
   else
      i = LARGE;
/*
   else
   {
      toVal->size = 0;
      toVal->addr = NULL;
      XtStringConversionWarning ((char *)fromVal->addr, "DesktopIcon");
   }
*/
}

static void
DtfileCvtStringToTree (
          XrmValue *args,
          Cardinal numArgs,
          XrmValue *fromVal,
          XrmValue *toVal)
{
   char * in_str = (char *) (fromVal->addr);
   static int i;

   toVal->size = sizeof (int);
   toVal->addr = (XtPointer) &i;

   DtfileStringToTree(in_str, &i);
}

static void
DtfileCvtStringToTreeFiles (
          XrmValue *args,
          Cardinal numArgs,
          XrmValue *fromVal,
          XrmValue *toVal)
{
   char * in_str = (char *) (fromVal->addr);
   static int i;

   toVal->size = sizeof (int);
   toVal->addr = (XtPointer) &i;

   DtfileStringToTreeFiles(in_str, &i);
}

static void
DtfileCvtStringToView (
          XrmValue *args,
          Cardinal numArgs,
          XrmValue *fromVal,
          XrmValue *toVal)
{
   char * in_str = (char *) (fromVal->addr);
   static int i;

   toVal->size = sizeof (int);
   toVal->addr = (XtPointer) &i;

   DtfileStringToView(in_str, &i);
}

static void
DtfileCvtStringToOrder (
          XrmValue *args,
          Cardinal numArgs,
          XrmValue *fromVal,
          XrmValue *toVal)
{
   char * in_str = (char *) (fromVal->addr);
   static int i;

   toVal->size = sizeof (int);
   toVal->addr = (XtPointer) &i;

   DtfileStringToOrder(in_str, &i);
}

static void
DtfileCvtStringToDirection (
          XrmValue *args,
          Cardinal numArgs,
          XrmValue *fromVal,
          XrmValue *toVal)
{
   char * in_str = (char *) (fromVal->addr);
   static int i;

   toVal->size = sizeof (int);
   toVal->addr = (XtPointer) &i;

   DtfileStringToDirection(in_str, &i);
}

static void
DtfileCvtStringToGrid (
          XrmValue *args,
          Cardinal numArgs,
          XrmValue *fromVal,
          XrmValue *toVal)
{
   char * in_str = (char *) (fromVal->addr);
   static int i;

   toVal->size = sizeof (int);
   toVal->addr = (XtPointer) &i;

   DtfileStringToGrid(in_str, &i);
}

FileMgrData *
CheckOpenDir (
          char * directory,
          char * host)
{
   int i;
   FileMgrData *file_mgr_data;
   DialogData *dialog_data;

   for(i = 0; i < view_count; i++)
   {
      if(strcmp(directory, view_set[i]->directory_name) == 0 &&
                            strcmp(host, view_set[i]->host_name) == 0)
      {
         dialog_data = (DialogData *) view_set[i]->dialog_data;
         file_mgr_data = (FileMgrData *) dialog_data->data;
         return(file_mgr_data);
      }
   }
   return(NULL);
}

char *
GetRestrictedDirectory (
          Widget widget)
{
   int i;
   FileMgrData *file_mgr_data;
   FileMgrRec *file_mgr_rec;
   DialogData *dialog_data;

   for(i = 0; i < view_count; i++)
   {
      dialog_data = (DialogData *) view_set[i]->dialog_data;
      file_mgr_data = (FileMgrData *) dialog_data->data;
      file_mgr_rec = (FileMgrRec *)file_mgr_data->file_mgr_rec;
      if(file_mgr_rec->current_directory_text == widget &&
                                          file_mgr_data->fast_cd_enabled)
      {
         return(file_mgr_data->restricted_directory);
      }
   }
   return((char *)NULL);
}

static void
DtfileStringToTree(
   char *str,
   int *type)
{
   if (_DtStringsAreEquivalent (str, "on"))
      *type = MULTIPLE_DIRECTORY;
   else if (_DtStringsAreEquivalent (str, "off"))
      *type = SINGLE_DIRECTORY;
   else
      *type = UNSET_VALUE;
}

static void
DtfileStringToTreeFiles(
   char *str,
   int *type)
{
   if (_DtStringsAreEquivalent (str, "never"))
      *type = TREE_FILES_NEVER;
   else if (_DtStringsAreEquivalent (str, "choose"))
      *type = TREE_FILES_CHOOSE;
   else if (_DtStringsAreEquivalent (str, "always"))
      *type = TREE_FILES_ALWAYS;
   else
      *type = UNSET_VALUE;
}

static void
DtfileStringToView(
   char *str,
   int *type)
{
   if (_DtStringsAreEquivalent (str, "no_icon") ||
                            _DtStringsAreEquivalent (str, "no_icons"))
      *type = BY_NAME;
   else if (_DtStringsAreEquivalent (str, "large_icon") ||
                            _DtStringsAreEquivalent (str, "large_icons"))
      *type = BY_NAME_AND_ICON;
   else if (_DtStringsAreEquivalent (str, "small_icon") ||
                            _DtStringsAreEquivalent (str, "small_icons"))
      *type = BY_NAME_AND_SMALL_ICON;
   else if (_DtStringsAreEquivalent (str, "attributes"))
      *type = BY_ATTRIBUTES;
   else
      *type = UNSET_VALUE;
}

static void
DtfileStringToOrder(
   char *str,
   int *type)
{
   if (_DtStringsAreEquivalent (str, "file_type"))
      *type = ORDER_BY_FILE_TYPE;
   else if (_DtStringsAreEquivalent (str, "alphabetical"))
      *type = ORDER_BY_ALPHABETICAL;
   else if (_DtStringsAreEquivalent (str, "date"))
      *type = ORDER_BY_DATE;
   else if (_DtStringsAreEquivalent (str, "size"))
      *type = ORDER_BY_SIZE;
   else
      *type = UNSET_VALUE;
}


static void
DtfileStringToDirection(
   char *str,
   int *type)
{
   if (_DtStringsAreEquivalent (str, "ascending"))
      *type = DIRECTION_ASCENDING;
   else if (_DtStringsAreEquivalent (str, "descending"))
      *type = DIRECTION_DESCENDING;
   else
      *type = UNSET_VALUE;
}

static void
DtfileStringToGrid(
   char *str,
   int *type)
{
   if (_DtStringsAreEquivalent (str, "on"))
      *type = RANDOM_OFF;
   else if (_DtStringsAreEquivalent (str, "off"))
      *type = RANDOM_ON;
   else
      *type = UNSET_VALUE;
}

static void
SetupSendRequestArgs(
   ApplicationArgs application_args,
   Tt_message msg)
{
   char *vtype;
   if(application_args.title != NULL)
   {
      tt_message_arg_add( msg, TT_IN, "-title", application_args.title );
   }
   if(application_args.restricted != NULL)
   {
      tt_message_arg_add( msg, TT_IN, RESTRICTED_HEADER, 0 );
   }
   vtype = VIEW_HEADER;
   switch( application_args.view )
   {
       case BY_NAME:
	   tt_message_arg_add( msg, TT_IN, vtype, "no_icon" );
           break;
       case BY_NAME_AND_ICON:
	   tt_message_arg_add( msg, TT_IN, vtype, "large_icon" );
           break;
       case BY_NAME_AND_SMALL_ICON:
	   tt_message_arg_add( msg, TT_IN, vtype, "small_icon" );
           break;
       case BY_ATTRIBUTES:
	   tt_message_arg_add( msg, TT_IN, vtype, "attributes" );
           break;
       default:
	   tt_message_arg_add( msg, TT_IN, vtype, "none" );
           break;
   }

   vtype = "-order";
   switch( application_args.order )
   {
       case ORDER_BY_FILE_TYPE:
	   tt_message_arg_add( msg, TT_IN, vtype, "file_type" );
           break;
       case ORDER_BY_ALPHABETICAL:
	   tt_message_arg_add( msg, TT_IN, vtype, "alphabetical" );
           break;
       case ORDER_BY_DATE:
	   tt_message_arg_add( msg, TT_IN, vtype, "date" );
           break;
       case ORDER_BY_SIZE:
	   tt_message_arg_add( msg, TT_IN, vtype, "size" );
           break;
       default:
	   tt_message_arg_add( msg, TT_IN, vtype, "none" );
           break;
   }

   vtype = "-direction";
   switch( application_args.direction )
   {
       case DIRECTION_ASCENDING:
	   tt_message_arg_add( msg, TT_IN, vtype, "ascending" );
           break;
       case DIRECTION_DESCENDING:
	   tt_message_arg_add( msg, TT_IN, vtype, "descending" );
           break;
       default:
	   tt_message_arg_add( msg, TT_IN, vtype, "none" );
           break;
   }

   vtype = "-grid";
   switch( application_args.grid )
   {
       case RANDOM_OFF:
	   tt_message_arg_add( msg, TT_IN, vtype, "on" );
           break;
       case RANDOM_ON:
	   tt_message_arg_add( msg, TT_IN, vtype, "off" );
           break;
       default:
	   tt_message_arg_add( msg, TT_IN, vtype, "none" );
           break;
   }
   vtype = "-tree";
   switch( application_args.tree_view )
   {
       case MULTIPLE_DIRECTORY:
           tt_message_arg_add( msg, TT_IN, vtype, "on" );
           break;
       case SINGLE_DIRECTORY:
           tt_message_arg_add( msg, TT_IN, vtype, "off" );
           break;
       default:
           tt_message_arg_add( msg, TT_IN, vtype, "none" );
           break;
   }

   vtype = "-tree_files";
   switch( application_args.tree_files )
   {
       case TREE_FILES_NEVER:
           tt_message_arg_add( msg, TT_IN, vtype, "never" );
           break;
       case TREE_FILES_CHOOSE:
           tt_message_arg_add( msg, TT_IN, vtype, "choose" );
           break;
       case TREE_FILES_ALWAYS:
           tt_message_arg_add( msg, TT_IN, vtype, "always" );
           break;
       default:
           tt_message_arg_add( msg, TT_IN, vtype, "none" );
           break;
   }

   if(application_args.help_volume != NULL)
   {
      tt_message_arg_add( msg, TT_IN, "-help_volume",
			  application_args.help_volume );
   }
}

/*
 * This routine is used by ForceMyIconOpen to get the "Open" filetype and
 * find out what the new icon is.  It then places that icon in the
 * correct icon gadget.
 */
static void
BuildAndShowIconName(
   char *file_type_name,
   unsigned char view,
   unsigned char show_type,
   Widget widget)
{
   char *new_file_type_name;
   char *ptr;
   Arg args[1];
   PixmapData *pixmapData;

   new_file_type_name = (char *)XtMalloc(strlen(file_type_name) +
                                         strlen(ICON_OPEN_PREFIX) + 1);
   sprintf(new_file_type_name, "%s%s", ICON_OPEN_PREFIX, file_type_name);

   if (view == BY_NAME_AND_ICON && show_type != MULTIPLE_DIRECTORY)
     pixmapData = _DtRetrievePixmapData(new_file_type_name,
                                        NULL,
                                        NULL,
                                        widget,
                                        LARGE);
   else
     pixmapData = _DtRetrievePixmapData(new_file_type_name,
                                        NULL,
                                        NULL,
                                        widget,
                                        SMALL);

   if(pixmapData && pixmapData->iconFileName)
   {
     XtSetArg (args[0], XmNimageName, pixmapData->iconFileName);
     XtSetValues(widget, args, 1);
   }
/*
   else
   {
     XtSetArg (args[0], XmNimageName, NULL);
     XtSetValues(widget, args, 1);
   }
*/

   _DtCheckAndFreePixmapData(new_file_type_name,
                             widget,
                             (DtIconGadget) widget,
                             pixmapData);

   XtFree(new_file_type_name);
}


/*
 * Given a directory name, this function will see if a view of the parent
 * directory is open; if so, then it will update the icon representing
 * this icon, in the parent view, so that it is drawn as 'closed'.  This
 * function must only be called if openDirType == NEW.
 */

void
ForceMyIconClosed (
   char * host_name,
   char * directory_name)
{
   register int i;
   register int j;
   register int k;
   Arg args[1];
   FileViewData * file_view_data = NULL;
   DialogData *dialog_data;
   FileMgrData *file_mgr_data;
   DesktopRec *desktopWindow;
   char * parent;
   char * fname;
   PixmapData *pixmapData;

   /* find the parent directory of the one just removed */
   parent = _DtPName(directory_name);
   fname =  DName(directory_name);

   /* first check to see if any File Manager views have this directory */
   for (i = 0; i < view_count; i++)
   {
      dialog_data = (DialogData *) view_set[i]->dialog_data;
      file_mgr_data = (FileMgrData *) dialog_data->data;

      /* loop through until we find the file_view_data structure for
       * the directory to force open */

      for(j = 0; j < file_mgr_data->directory_count; j++)
      {
         if (strcmp(parent, file_mgr_data->directory_set[j]->name) == 0)
         {
            for (k = 0; k < file_mgr_data->directory_set[j]->file_count; k++)
            {
               file_view_data =
                       file_mgr_data->directory_set[j]->file_view_data[k];
               if (strcmp(file_view_data->file_data->file_name, fname) == 0)
                  goto search_done;
            }
            break;
         }
      }
   }

   search_done:

   if( (file_view_data) && (file_mgr_data->view != BY_NAME) )
   {
      if (file_mgr_data->view == BY_NAME_AND_ICON  &&
                       file_mgr_data->show_type != MULTIPLE_DIRECTORY)
         pixmapData = _DtRetrievePixmapData(
                                 file_view_data->file_data->logical_type,
                                 fname,
                                 parent,
                                 file_view_data->widget,
                                 LARGE);
      else
         pixmapData = _DtRetrievePixmapData(
                                 file_view_data->file_data->logical_type,
                                 fname,
                                 parent,
                                 file_view_data->widget,
                                 SMALL);

      if (pixmapData)
        XtSetArg (args[0], XmNimageName, pixmapData->iconFileName);
      else
        XtSetArg (args[0], XmNimageName, NULL);
      XtSetValues (file_view_data->widget, args, 1);

      _DtCheckAndFreePixmapData(file_view_data->file_data->logical_type,
                                file_view_data->widget,
                                (DtIconGadget) file_view_data->widget,
                                pixmapData);
   }

   /* now check to see if any desktop objects are this directory */
   for(j = 0; j < desktop_data->numIconsUsed; j++)
   {
      char buf[MAX_PATH];

      desktopWindow = desktop_data->desktopWindows[j];
      file_view_data = desktopWindow->file_view_data;

      sprintf(buf, "%s/%s", desktopWindow->dir_linked_to,
                            desktopWindow->file_name);
      DtEliminateDots (buf);

      if( (strcmp(buf, directory_name) == 0) &&
          (strcmp(desktopWindow->host, host_name) == 0) )
      {
         pixmapData = _DtRetrievePixmapData(
              file_view_data->file_data->logical_type,
              fname,
              NULL,
              desktopWindow->shell,
              desktopIconType);

         if(pixmapData)
           XtSetArg (args[0], XmNimageName, pixmapData->iconFileName);
         else
           XtSetArg (args[0], XmNimageName, NULL);
         XtSetValues (desktopWindow->iconGadget, args, 1);

         _DtCheckAndFreePixmapData(
                 desktopWindow->file_view_data->file_data->logical_type,
                 desktopWindow->shell,
                 (DtIconGadget) desktopWindow->iconGadget,
                 pixmapData);
      }
   }
}


/*************************************************************************
 *
 * MarqueeSelect - this is the callback which gets called when there is
 *    a marquee event happening on the root window.
 *
 ************************************************************************/
static void
MarqueeSelect (
      Widget w,
      int type,
      Position x,
      Position y,
      Dimension width,
      Dimension height,
      XtPointer client_data)
{
   int i;
   Window   rootWindow;
   Atom     pCurrent;
   Screen   *currentScreen;
   int      screen;
   char     *workspace_name=NULL;
   Display *display;


   switch (type)
   {
       case DT_WSM_MARQUEE_SELECTION_TYPE_BEGIN:
            display = XtDisplay(desktop_data->desktopWindows[0]->shell);
            screen = XDefaultScreen(display);
            currentScreen = XScreenOfDisplay(display, screen);
            rootWindow = RootWindowOfScreen(currentScreen);

            if(DtWsmGetCurrentWorkspace(display, rootWindow, &pCurrent)
                                                            == Success)
            {
               workspace_name = XGetAtomName (display, pCurrent);
               CleanUpWSName(workspace_name);
            }
            else
               workspace_name =
                  XtNewString(desktop_data->workspaceData[0]->name);

            for(i = 0; i < desktop_data->numWorkspaces; i++)
            {
               if(strcmp(workspace_name,
                           desktop_data->workspaceData[i]->name) == 0)
               {
                  DeselectAllDTFiles(desktop_data->workspaceData[i]);
                  break;
               }
            }
            XtFree(workspace_name);
            break;

       case DT_WSM_MARQUEE_SELECTION_TYPE_END:
            CheckDesktopMarquee(x, y, width, height);
            break;

       case DT_WSM_MARQUEE_SELECTION_TYPE_CANCEL:
            break;

       case DT_WSM_MARQUEE_SELECTION_TYPE_CONTINUE:
            CheckDesktopMarquee(x, y, width, height);
            break;

       default:
            break;
   }
}


/*
 * We must wait for the message response, before exiting
 */

static void
WaitForResponseAndExit( void )

{
   XtMainLoop();
}

static void
ExitApp(
        XtPointer clientData,
        DtString words[],
        int num_fields )

{
  FinalizeToolTalkSession( );
  exit(0);
}


/*
 * Puts up an Error dialog with Cancel and Help unmapped in the
 * center of the screen. The last argument is the OK callback
 */

static Widget
post_dialog(Widget parent, char *title, char *msg, void (*DtErrExitCB)())
{
   Widget dialog, dialogShell;
   XmString message_text, ok;
   Arg args[10];
   Dimension dialogWd, dialogHt;
   int n = 0;

   ok = XmStringCreateLocalized ((char*)_DtOkString);
   message_text = XmStringCreateLocalized (msg);

   XtSetArg(args[n], XmNautoUnmanage, False); n++;
   XtSetArg(args[n], XmNokLabelString, ok); n++;
   XtSetArg(args[n], XmNtitle, title); n++;
   XtSetArg(args[n], XmNmessageString, message_text); n++;
   XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); n++;
   XtSetArg (args[n], XmNdefaultPosition, False); n++;
   XtSetArg(args[n], XmNuseAsyncGeometry, True); n++;

   dialog = XmCreateErrorDialog (parent, title, args, n);
   XtAddCallback (dialog, XmNokCallback, DtErrExitCB, NULL);
   XtUnmanageChild (XmMessageBoxGetChild (dialog, XmDIALOG_CANCEL_BUTTON));
   XtUnmanageChild (XmMessageBoxGetChild (dialog, XmDIALOG_HELP_BUTTON));

   /* Disable the frame menu from dialog since we don't want the user
      to be able to close dialogs with the frame menu */

   dialogShell = XtParent(dialog);
   XtSetArg(args[0], XmNmappedWhenManaged, False);
   XtSetArg(args[1], XmNmwmDecorations, MWM_DECOR_ALL | MWM_DECOR_MENU);
   XtSetValues(dialogShell, args, 2);
   XtManageChild (dialog);
   XtRealizeWidget (dialogShell);

   /* Center the dialog */

   XtSetArg(args[0], XmNwidth, &dialogWd);
   XtSetArg(args[1], XmNheight, &dialogHt);
   XtGetValues(dialog, args, 2);
   XtSetArg (args[0], XmNx,
                (WidthOfScreen(XtScreen(dialog)) - dialogWd) / 2);
   XtSetArg (args[1], XmNy,
                (HeightOfScreen(XtScreen(dialog)) - dialogHt) / 2);
   XtSetArg (args[2], XmNmappedWhenManaged, True); 
   XtSetValues (dialog, args, 3);

   XtSetArg(args[0], XmNmappedWhenManaged, True);
   XtSetValues(dialogShell, args, 1);

   XtManageChild (dialog);
   XmStringFree(message_text);

   return (dialog);
}

/*  
 * This is the Callback when an error occurs while trying to create 
 * the .dt folder or sub-folders. Application exits.
 */

static void 
DtErrExitCB (Widget dialog, XtPointer client_data, XtPointer call_data)
{
XtPopdown (XtParent (dialog));
FinalizeToolTalkSession();
exit (1);
}

#ifdef __osf__
extern void
sigchld_handler(int signo)      /* Do not use the arg signo at the moment */
{
    pid_t   pid;
    int     stat_loc;

    /*
      On DUX, the process remains in the ZOMBIE
      state untill parent invokes wait or waitpid.
    */

    pid = waitpid(-1, &stat_loc, WNOHANG);
    /* Child exit handling code follows, if any */
}
#endif /* __osf__ */
