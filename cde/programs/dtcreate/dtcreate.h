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
/* $XConsortium: dtcreate.h /main/5 1996/09/17 11:01:43 rswiston $ */
/***************************************************************************/
/*                                                                         */
/*  dtcreate.h                                                             */
/*                                                                         */
/***************************************************************************/

#ifndef _DTCREATE_H_INCLUDED
#define _DTCREATE_H_INCLUDED

#define __TOOLTALK

#include <Xm/Xm.h>
#include <nl_types.h>
#include <sys/param.h>

#ifndef STORAGECLASS
#ifdef  NOEXTERN
#define STORAGECLASS
#else
#define STORAGECLASS extern
#endif
#endif

/***************************************************************************/
/*                                                                         */
/*  Constant Declarations                                                  */
/*                                                                         */
/***************************************************************************/
#define TRUE                  1
#define FALSE                 0
#define Psz                   (char *)

#define MAXFILENAME           MAXPATHLEN
#define MAXBUFSIZE            256

        /*******************************************************************/
        /* Icon Sizes                                                      */
        /*******************************************************************/
#define CA_LRG_ICON_SIZE      48
#define CA_MED_ICON_SIZE      32
#define CA_SML_ICON_SIZE      24
#define CA_TINY_ICON_SIZE     16

        /*******************************************************************/
        /* FILE SELECTION WIDGET VALUES                                    */
        /*******************************************************************/
#define CA_ACTION_ICONS        1
#define CA_FILETYPE_ICONS      2
#define CA_FILE                3

        /*******************************************************************/
        /* ActionData fsFlags values                                       */
        /*******************************************************************/
#define CA_WT_XWINDOWS        0x0001
#define CA_WT_TERM            0x0002
#define CA_WT_PERMTERM        0x0004
#define CA_WT_SHELLTERM       0x0008

#define CA_DF_ONLYFTFILES     0x0010

        /*******************************************************************/
        /* Filetype flags.                                                 */
        /*******************************************************************/
#define CA_FT_CNTSTRING       0x0001
#define CA_FT_CNTBYTE         0x0002
#define CA_FT_CNTSHORT        0x0004
#define CA_FT_CNTLONG         0x0008

        /*******************************************************************/
        /* Filetype Internal flags                                         */
        /*******************************************************************/
#define CA_FT_APPLY           0x0001
#define CA_FT_EDIT            0x0002

        /*******************************************************************/
        /* Icon filename extensions                                        */
        /*******************************************************************/
#define LARGE_EXT             ".l"
#define MEDIUM_EXT            ".m"
#define SMALL_EXT             ".t"
#define TINY_EXT              ".t"

#define PIXMAP_EXT            ".pm"
#define BITMAP_EXT            ".bm"
#define MAX_EXT_SIZE          5

#define EXT_DELIMITER         '.'

#define ICON_BORDER_WIDTH     1
#define ICON_NUMBER           5

        /*******************************************************************/
        /* Help Ids                                                        */
        /*******************************************************************/
#define HELP_OVERVIEW         1
#define HELP_TASKS            2
#define HELP_REFERENCE        3
#define HELP_USING            4
#define HELP_ABOUT            5
#define HELP_ONITEM           6

#define HELP_ADDFILETYPE      10
#define HELP_FILECHAR         20
#define HELP_ICONSELECTOR     30
#define HELP_OPENFILE         40

/*****************************************************************************/
/*                                                                           */
/*  enums                                                                    */
/*                                                                           */
/*****************************************************************************/
enum icon_size_range {None_Selected, Large_Icon, Medium_Icon, Small_Icon, Tiny_Icon};
enum icon_data_range {CA_LRG, CA_MED, CA_TINY, AF_MED, AF_TINY};

/***************************************************************************/
/*                                                                         */
/*  Structure Declarations                                                 */
/*                                                                         */
/***************************************************************************/
typedef struct _FiletypeData *pFiletypeData;
typedef struct _FiletypeData {
        char           *pszName;
        char           *pszIcon;
        char           *pszMedPmIcon;
        char           *pszMedBmIcon;
        char           *pszTinyPmIcon;
        char           *pszTinyBmIcon;
        char           *pszHelp;
        char           *pszOpenCmd;
        char           *pszPrintCmd;
        char           *pszPattern;
        char           *pszPermissions;
        char           *pszContents;
        int            sStart;
        int            sEnd;
        ushort         fsFlags;
        ushort         fsInternal;
} FiletypeData;

typedef struct _ActionData *pActionData;
typedef struct _ActionData {
        char           *pszName;
        char           *pszIcon;
        char           *pszCmd;
        char           *pszHelp;
        char           *pszPrompt;
        FiletypeData   **papFiletypes;
        short          cFiletypes;
        ushort         fsFlags;
} ActionData;

typedef struct {
    char               *pszSessionFile;
    Boolean            useSession;
    Boolean            showExpert;
    int                iconicState;
    Position           x, y;
    Dimension          width, height;
    char	       workspaces[256];
} SessionData, *SessionDataPtr;

typedef struct {
    Widget                wid;
    char                  *pmMsgID;
    char                  *bmMsgID;
    char                  pmFileName[MAXPATHLEN];
    char                  bmFileName[MAXPATHLEN];
    Boolean               pmDirtyBit;
    Boolean               bmDirtyBit;
    enum icon_size_range  size;
} IconData, *IconDataPtr;

/***************************************************************************/
/*                                                                         */
/*  Macro Declarations                                                     */
/*                                                                         */
/***************************************************************************/
#define GETMESSAGE(set, number, defstr) catgets(nlmsg_fd, set, number, defstr)

/***************************************************************************/
/*                                                                         */
/*  Global Variable Declarations                                           */
/*                                                                         */
/***************************************************************************/

        /*******************************************************************/
        /* Global variables that are not being initialized.                */
        /*******************************************************************/

STORAGECLASS  ActionData   AD;
STORAGECLASS  ActionData   *pMyCopyAD;
STORAGECLASS  char         *pszFileToEdit;
STORAGECLASS  Cursor       watch_cursor;
STORAGECLASS  Widget       widEditSource;
STORAGECLASS  Boolean      bIconEditorDisplayed;
STORAGECLASS  nl_catd      nlmsg_fd;
STORAGECLASS  char         *pszExecName;
STORAGECLASS  Widget       widSelectedIcon;
STORAGECLASS  Boolean      bLowRes;
STORAGECLASS  IconData     *IconDataList[ICON_NUMBER];

                 /**********************************************************/
                 /* CreateActionAppShell  globals.                         */
                 /**********************************************************/

STORAGECLASS  Widget       CreateActionAppShell;
STORAGECLASS  Widget       ExpertOption;
STORAGECLASS  Widget       ColorMonoOption;
STORAGECLASS  Widget       CA_ActionNameTextField;
STORAGECLASS  Widget       CA_MED_IconGadget;
STORAGECLASS  Widget       CA_SML_IconGadget;
STORAGECLASS  Widget       CA_TINY_IconGadget;
STORAGECLASS  Widget       CA_LRG_IconGadget;
STORAGECLASS  Widget       CA_DblClkText;
STORAGECLASS  Widget       XprtOptionForm;
STORAGECLASS  Widget       CA_XprtActionOpenText;
STORAGECLASS  Widget       CA_FiletypesList;
STORAGECLASS  Widget       CA_WindowTypeArea;
STORAGECLASS  Widget       CA_WindowType;
STORAGECLASS  Widget       CA_Expand;
#if 0
STORAGECLASS  Widget       CA_WindowType_OptionMenuShell;
STORAGECLASS  Widget       CA_WindowType_Pane;
#endif
STORAGECLASS  Widget       CA_WinTypeX;
STORAGECLASS  Widget       CA_WinTypeAutoClose;
STORAGECLASS  Widget       CA_WinTypeManualClose;
STORAGECLASS  Widget       CA_WinTypeNoOutput;
STORAGECLASS  Widget       CA_HelpTextWindow;
STORAGECLASS  Widget       CA_HelpText;
STORAGECLASS  Widget       CA_AllFiletypesToggle;
STORAGECLASS  Widget       CA_FiletypesInListToggle;

                 /**********************************************************/
                 /* AddFiletype  globals.                                  */
                 /**********************************************************/

STORAGECLASS  Widget       AddFiletype;
STORAGECLASS  Widget       AF_FileTypeNameTextField;
STORAGECLASS  Widget       AF_IdCharacteristicsText;
STORAGECLASS  Widget       AF_IdCharacteristicsEdit;
STORAGECLASS  Widget       AF_FiletypePrintCmdTextField;
STORAGECLASS  Widget       AF_OpenCmdText;
STORAGECLASS  Widget       AF_FiletypeHelpText;
STORAGECLASS  Widget       AF_MED_IconGadget;
STORAGECLASS  Widget       AF_TINY_IconGadget;

                 /**********************************************************/
                 /* FileCharacteristics  globals                           */
                 /**********************************************************/

STORAGECLASS  Widget       FileCharacteristics;
STORAGECLASS  Widget       FC_DirectoryToggle;
STORAGECLASS  Widget       FC_FileToggle;
STORAGECLASS  Widget       FC_AndLabel2;
STORAGECLASS  Widget       FC_ContentsPatternText;
STORAGECLASS  Widget       FC_StringToggle;
STORAGECLASS  Widget       FC_ByteToggle;
STORAGECLASS  Widget       FC_ShortToggle;
STORAGECLASS  Widget       FC_LongToggle;
STORAGECLASS  Widget       FC_StartByteTextField;
/*
STORAGECLASS  Widget       FC_EndByteTextField;
*/
STORAGECLASS  Widget       FC_NameOrPathText;
STORAGECLASS  Widget       FC_AndLabel1;
STORAGECLASS  Widget       FC_PermissionForm;
STORAGECLASS  Widget       FC_ReadToggle;
STORAGECLASS  Widget       FC_WriteToggle;
STORAGECLASS  Widget       FC_ExecuteToggle;

STORAGECLASS  Widget       FC_NamePatternLabel;
STORAGECLASS  Widget       FC_PermissionLabel;
STORAGECLASS  Widget       FC_PermissionToggle;
STORAGECLASS  Widget       FC_ContentsLabel;
STORAGECLASS  Widget       FC_ContentsBox;
STORAGECLASS  Widget       FC_TypeRowColumn;
STORAGECLASS  Widget       FC_StartByteLabel;
STORAGECLASS  Widget       FC_TypeLabel;
STORAGECLASS  Widget       FC_ContentsPatternLabel;
/*
STORAGECLASS  Widget       FC_EndByteLabel;
*/
STORAGECLASS  Widget       FC_ContentsToggle;
STORAGECLASS  Widget       FC_NameOrPathToggle;

                 /**********************************************************/
                 /* icon_selection_dialog  globals                         */
                 /**********************************************************/
STORAGECLASS  Widget       ISD_SelectedIconTextField;

                 /**********************************************************/
                 /* Other dialog globals                                   */
                 /**********************************************************/
STORAGECLASS  SessionData  sessiondata;

        /*******************************************************************/
        /* Global variables that are being initialized.                    */
        /*******************************************************************/

#ifdef  NOEXTERN

STORAGECLASS  enum icon_size_range action_icon_size = None_Selected;
STORAGECLASS  enum icon_size_range filetype_icon_size = None_Selected;
STORAGECLASS  int     pidIconEditor = 0;
STORAGECLASS  Boolean bShowPixmaps = TRUE;
STORAGECLASS  Widget  IconSelector = (Widget)NULL;
STORAGECLASS  Boolean DbInitDone = FALSE;
STORAGECLASS  Widget  OpenFile = (Widget)NULL;
STORAGECLASS  Widget  Confirmed = (Widget)NULL;
STORAGECLASS  Widget  QuestionDialog = (Widget)NULL;
STORAGECLASS  Widget  ErrorDialog = (Widget)NULL;
STORAGECLASS  Widget  last_action_pushed = (Widget)NULL;
STORAGECLASS  Widget  last_filetype_pushed = (Widget)NULL;


STORAGECLASS  const char *ca_icon_default = "Dtactn";
STORAGECLASS  const char *ca_full_icon_default =  "/usr/dt/appconfig/icons/C/Dtactn";
STORAGECLASS  const char *ca_lrg_icon_default =  "/usr/dt/appconfig/icons/C/Dtactn.l";
STORAGECLASS  const char *ca_med_icon_default =  "/usr/dt/appconfig/icons/C/Dtactn.m";
STORAGECLASS  const char *ca_sml_icon_default =  "/usr/dt/appconfig/icons/C/Dtactn.s";
STORAGECLASS  const char *ca_tiny_icon_default = "/usr/dt/appconfig/icons/C/Dtactn.t";

STORAGECLASS  const char *af_icon_default = "Dtdata";
STORAGECLASS  const char *af_full_icon_default =  "/usr/dt/appconfig/icons/C/Dtdata";
STORAGECLASS  const char *af_lrg_icon_default =  "/usr/dt/appconfig/icons/C/Dtdata.l";
STORAGECLASS  const char *af_med_icon_default =  "/usr/dt/appconfig/icons/C/Dtdata.m";
STORAGECLASS  const char *af_sml_icon_default =  "/usr/dt/appconfig/icons/C/Dtdata.s";
STORAGECLASS  const char *af_tiny_icon_default = "/usr/dt/appconfig/icons/C/Dtdata.t";

#else  /* NOEXTERN */

STORAGECLASS  enum icon_size_range action_icon_size;
STORAGECLASS  enum icon_size_range filetype_icon_size;
STORAGECLASS  int     pidIconEditor;
STORAGECLASS  Boolean bShowPixmaps;
STORAGECLASS  Widget  IconSelector;
STORAGECLASS  Boolean DbInitDone;
STORAGECLASS  Widget  OpenFile;
STORAGECLASS  Widget  Confirmed;
STORAGECLASS  Widget  QuestionDialog;
STORAGECLASS  Widget  ErrorDialog;
STORAGECLASS  Widget  last_action_pushed;
STORAGECLASS  Widget  last_filetype_pushed;


STORAGECLASS  const char *ca_icon_default;
STORAGECLASS  const char *ca_full_icon_default;
STORAGECLASS  const char *ca_lrg_icon_default;
STORAGECLASS  const char *ca_med_icon_default;
STORAGECLASS  const char *ca_sml_icon_default;
STORAGECLASS  const char *ca_tiny_icon_default;

STORAGECLASS  const char *af_icon_default;
STORAGECLASS  const char *af_full_icon_default;
STORAGECLASS  const char *af_lrg_icon_default;
STORAGECLASS  const char *af_med_icon_default;
STORAGECLASS  const char *af_sml_icon_default;
STORAGECLASS  const char *af_tiny_icon_default;

#endif /*  NOEXTERN */

#endif /* _DTCREATE_H_INCLUDED */
