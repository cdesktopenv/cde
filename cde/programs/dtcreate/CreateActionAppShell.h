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
/* $XConsortium: CreateActionAppShell.h /main/4 1995/11/01 16:06:24 rswiston $ */
/******************************************************************************
       CreateActionAppShell.h

       This header file is included by CreateActionAppShell.c

*******************************************************************************/

#ifndef _CREATEACTIONAPPSHELL_H_INCLUDED
#define _CREATEACTIONAPPSHELL_H_INCLUDED

#include <stdio.h>
#include <Xm/Xm.h>
#include <Xm/MwmUtil.h>
#include <Xm/MenuShell.h>
#include "UxXt.h"
#include "dtcreate.h"

#include <Xm/Text.h>
#include <Xm/ToggleBG.h>
#include <Xm/List.h>
#include <Xm/ScrolledW.h>
#include <Xm/Text.h>
#include <Xm/PushBG.h>
#include <Xm/TextF.h>
#include <Xm/Label.h>
#include <Xm/Form.h>
#include <Xm/CascadeB.h>
#include <Xm/ToggleB.h>
#include <Xm/Separator.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/MainW.h>
#include <X11/Shell.h>

/*******************************************************************************
       The definition of the context structure:
       If you create multiple copies of your interface, the context
       structure ensures that your callbacks use the variables for the
       correct copy.

       For each swidget in the interface, each argument to the Interface
       function, and each variable in the Interface Specific section of the
       Declarations Editor, there is an entry in the context structure.
       and a #define.  The #define makes the variable name refer to the
       corresponding entry in the context structure.
*******************************************************************************/

typedef struct
{
        Widget  UxCreateActionMainWindow;
        Widget  UxCA_MenuBar;
        Widget  UxCA_MB_FileOption;
        Widget  UxCA_MB_FileNew;
        Widget  UxCA_MB_FileOpen;
        Widget  UxCA_MB_FileSave;
        Widget  UxCA_MB_Sep;
        Widget  UxCA_MB_FileExit;
        Widget  UxCA_MB_FileTop;
        Widget  UxCA_MB_HelpOption;
        Widget  UxCA_MB_HelpOverview;
        Widget  UxCA_MB_HelpTOC;
        Widget  UxCA_MB_HelpTasks;
        Widget  UxCA_MB_HelpReference;
        Widget  UxCA_MB_HelpUsing;
        Widget  UxCA_MB_HelpAbout;
        Widget  UxCA_MB_HelpTop;
        Widget  UxCA_MB_OptionsOption;
        Widget  UxCA_MB_OptionTop;
        Widget  UxCA_Form;
        Widget  UxCA_ActionNameLabel;
        Widget  UxCA_ActionIconLabel;
        Widget  UxCA_ActionIconForm;
        Widget  UxCA_EditIcon_button;
        Widget  UxCA_FindSet_button;
        Widget  Uxseparator1;
        Widget  UxCA_DblClkLabel;
        Widget  UxCA_HlpTxtLabel;
        Widget  UxCA_XprtOptionLabel;
        Widget  UxCA_XprtSep2;
        Widget  UxCA_ActionOpenLabel;
        Widget  UxCA_XprtSep1;
        Widget  UxCA_FiletypesLabel;
        Widget  Uxform1;
        Widget  UxCA_DroppableFiletypesLabel;
        Widget  UxCA_DroppableFiletypesRowColumn;
        Widget  UxrowColumn1;
        Widget  UxCA_FiletypesAdd;
        Widget  UxCA_FiletypesDelete;
        Widget  UxCA_FiletypesEdit;
        Widget  UxCA_FiletypesWindow;
        Widget  UxCA_WindowTypeLabel;
        swidget UxUxParent;
} _UxCCreateActionAppShell;

#ifdef CONTEXT_MACRO_ACCESS

static _UxCCreateActionAppShell *UxCreateActionAppShellContext;
#define CreateActionMainWindow  UxCreateActionAppShellContext->UxCreateActionMainWindow
#define CA_MenuBar              UxCreateActionAppShellContext->UxCA_MenuBar
#define CA_MB_FileOption        UxCreateActionAppShellContext->UxCA_MB_FileOption
#define CA_MB_FileNew           UxCreateActionAppShellContext->UxCA_MB_FileNew
#define CA_MB_FileOpen          UxCreateActionAppShellContext->UxCA_MB_FileOpen
#define CA_MB_FileSave          UxCreateActionAppShellContext->UxCA_MB_FileSave
#define CA_MB_Sep               UxCreateActionAppShellContext->UxCA_MB_Sep
#define CA_MB_FileExit          UxCreateActionAppShellContext->UxCA_MB_FileExit
#define CA_MB_FileTop           UxCreateActionAppShellContext->UxCA_MB_FileTop
#define CA_MB_HelpOption        UxCreateActionAppShellContext->UxCA_MB_HelpOption
#define CA_MB_HelpOverview      UxCreateActionAppShellContext->UxCA_MB_HelpOverview
#define CA_MB_HelpTOC           UxCreateActionAppShellContext->UxCA_MB_HelpTOC
#define CA_MB_HelpTasks         UxCreateActionAppShellContext->UxCA_MB_HelpTasks
#define CA_MB_HelpReference     UxCreateActionAppShellContext->UxCA_MB_HelpReference
#define CA_MB_HelpUsing         UxCreateActionAppShellContext->UxCA_MB_HelpUsing
#define CA_MB_HelpAbout         UxCreateActionAppShellContext->UxCA_MB_HelpAbout
#define CA_MB_HelpTop           UxCreateActionAppShellContext->UxCA_MB_HelpTop
#define CA_MB_OptionsOption     UxCreateActionAppShellContext->UxCA_MB_OptionsOption
#define CA_MB_OptionTop         UxCreateActionAppShellContext->UxCA_MB_OptionTop
#define CA_Form                 UxCreateActionAppShellContext->UxCA_Form
#define CA_ActionNameLabel      UxCreateActionAppShellContext->UxCA_ActionNameLabel
#define CA_ActionIconLabel      UxCreateActionAppShellContext->UxCA_ActionIconLabel
#define CA_ActionIconForm       UxCreateActionAppShellContext->UxCA_ActionIconForm
#define CA_EditIcon_button      UxCreateActionAppShellContext->UxCA_EditIcon_button
#define CA_FindSet_button       UxCreateActionAppShellContext->UxCA_FindSet_button
#define separator1              UxCreateActionAppShellContext->Uxseparator1
#define CA_DblClkLabel          UxCreateActionAppShellContext->UxCA_DblClkLabel
#define CA_HlpTxtLabel          UxCreateActionAppShellContext->UxCA_HlpTxtLabel
#define CA_XprtOptionLabel      UxCreateActionAppShellContext->UxCA_XprtOptionLabel
#define CA_XprtSep2             UxCreateActionAppShellContext->UxCA_XprtSep2
#define CA_ActionOpenLabel      UxCreateActionAppShellContext->UxCA_ActionOpenLabel
#define CA_XprtSep1             UxCreateActionAppShellContext->UxCA_XprtSep1
#define CA_FiletypesLabel       UxCreateActionAppShellContext->UxCA_FiletypesLabel
#define form1                   UxCreateActionAppShellContext->Uxform1
#define CA_DroppableFiletypesLabel UxCreateActionAppShellContext->UxCA_DroppableFiletypesLabel
#define CA_DroppableFiletypesRowColumn UxCreateActionAppShellContext->UxCA_DroppableFiletypesRowColumn
#define rowColumn1              UxCreateActionAppShellContext->UxrowColumn1
#define CA_FiletypesAdd         UxCreateActionAppShellContext->UxCA_FiletypesAdd
#define CA_FiletypesDelete      UxCreateActionAppShellContext->UxCA_FiletypesDelete
#define CA_FiletypesEdit        UxCreateActionAppShellContext->UxCA_FiletypesEdit
#define CA_FiletypesWindow      UxCreateActionAppShellContext->UxCA_FiletypesWindow
#define CA_WindowTypeLabel      UxCreateActionAppShellContext->UxCA_WindowTypeLabel
#define UxParent                UxCreateActionAppShellContext->UxUxParent

#endif /* CONTEXT_MACRO_ACCESS */

/***************************************************************************/
/*                                                                         */
/*  Swidget Macro Definitions                                              */
/*                                                                         */
/***************************************************************************/

#define CREATE_ACTION_SWID (CreateActionAppShell)
#define ACTION_NAME_SWID (CA_ActionNameTextField)
#define DBL_CLK_CMND_SWID (CA_DblClkText)
#define ACTION_HELP_TXT_SWID (CA_HelpText)
#define HELP_TXT_WINDOW_SWID (CA_HelpTextWindow)
#define FILETYPES_FOR_ACTION_SWID (CA_FiletypesList)
#define ACTION_OPENS_TEXT_SWID (CA_XprtActionOpenText)
#define XWIN_TOGGLE_SWID (CA_WinTypeX)
#define AUTOCLOSE_TOGGLE_SWID (CA_WinTypeAutoClose)
#define MANUALCLOSE_TOGGLE_SWID (CA_WinTypeManualClose)
#define LRG_ICON_BUTTON_SWID (CA_LRG_IconButton)
#define MED_ICON_BUTTON_SWID (CA_MED_IconButton)
#define SML_ICON_BUTTON_SWID (CA_SML_IconButton)
#define TINY_ICON_BUTTON_SWID (CA_TINY_IconButton)
#define LRG_ICON_GADGET_SWID (CA_LRG_IconGadget)
#define MED_ICON_GADGET_SWID (CA_MED_IconGadget)
#define SML_ICON_GADGET_SWID (CA_SML_IconGadget)
#define TINY_ICON_GADGET_SWID (CA_TINY_IconGadget)
#define XPRT_OPTION_FORM_SWID (XprtOptionFrom)
#define EXPERT_OPTIONS_TOGGLE_BUTTON_SWID (ExpertOption)
#define OPEN_FILE_SWID (OpenFile)
#define FILE_CHARACTERISTICS_SWID (FileCharacteristics)
#define WINDOW_TYPE_SWID (CA_WindowType)
#define COLOR_MONO_OPTION_SWID (ColorMonoOption)


/***************************************************************************/
/*                                                                         */
/*  Widget Macro Definitions                                               */
/*                                                                         */
/***************************************************************************/

#define CREATE_ACTION (UxGetWidget(CREATE_ACTION_SWID))
#define ACTION_NAME (UxGetWidget(ACTION_NAME_SWID))
#define DBL_CLK_CMND (UxGetWidget(DBL_CLK_CMND_SWID))
#define ACTION_HELP_TXT (UxGetWidget(ACTION_HELP_TXT_SWID))
#define HELP_TXT_WINDOW (UxGetWidget(HELP_TXT_WINDOW_SWID))
#define FILETYPES_FOR_ACTION (UxGetWidget(FILETYPES_FOR_ACTION_SWID))
#define ACTION_OPENS_TEXT (UxGetWidget(ACTION_OPENS_TEXT_SWID))
#define XWIN_TOGGLE (UxGetWidget(XWIN_TOGGLE_SWID))
#define AUTOCLOSE_TOGGLE (UxGetWidget(AUTOCLOSE_TOGGLE_SWID))
#define MANUALCLOSE_TOGGLE (UxGetWidget(MANUALCLOSE_TOGGLE_SWID))
#define CA_LRG_ICON_BUTTON (UxGetWidget(LRG_ICON_BUTTON_SWID))
#define CA_MED_ICON_BUTTON (UxGetWidget(MED_ICON_BUTTON_SWID))
#define CA_SML_ICON_BUTTON (UxGetWidget(SML_ICON_BUTTON_SWID))
#define CA_TINY_ICON_BUTTON (UxGetWidget(TINY_ICON_BUTTON_SWID ))
#define CA_LRG_ICON_GADGET (UxGetWidget(LRG_ICON_GADGET_SWID))
#define CA_MED_ICON_GADGET (UxGetWidget(MED_ICON_GADGET_SWID))
#define CA_SML_ICON_GADGET (UxGetWidget(SML_ICON_GADGET_SWID))
#define CA_TINY_ICON_GADGET (UxGetWidget(TINY_ICON_GADGET_SWID ))
#define EXPERT_OPTIONS_TOGGLE_BUTTON (UxGetWidget(EXPERT_OPTIONS_TOGGLE_BUTTON_SWID))
#define XPRT_OPTION_FORM (UxGetWidget(XPRT_OPTION_FORM_SWID))
#define CA_OPEN_FILE (UxGetWidget(OPEN_FILE_SWID))
#define FILE_CHARACTERISTICS (UxGetWidget(FILE_CHARACTERISTICS_SWID))
#define WINDOW_TYPE (UxGetWidget(WINDOW_TYPE_SWID))
#define COLOR_MONO_OPTION (UxGetWidget(COLOR_MONO_OPTION_SWID))

/***************************************************************************/
/*                                                                         */
/*  Structure Access Macros                                                */
/*                                                                         */
/***************************************************************************/

#define CA_ActionName(ad_struct) (((ActionData *)(ad_struct))->pszName)
#define CA_DblClkAction(ad_struct) (((ActionData *)(ad_struct))->pszCmd)
#define CA_ActionHelpText(ad_struct) (((ActionData *)(ad_struct))->pszHelp)
#define CA_Filetypes(ad_struct) (((ActionData *)(ad_struct))->papFiletypes)
#define CA_FiletypesCount(ad_struct) (((ActionData *)(ad_struct))->cFiletypes)
#define CA_FiletypesForActionList(ad_struct,l) (((ActionData *)(ad_struct))[l]->pszName)
#define CA_ActionOpensText(ad_struct) (((ActionData *)(ad_struct))->pszPrompt)
#define CA_WindowTypeSetting(ad_struct) (((ActionData *)(ad_struct))->fsFlags)
#define CA_Icon(ad_struct) (((ActionData *)(ad_struct))->pszIcon)
#define CA_XprtMode(ad_struct) (((ActionData *)(ad_struct))->)

/*****************************************************************************/
/*     Function Declarations                                                 */
/*****************************************************************************/


Widget  create_applicationShell1(swidget);
void    activateCB_CA_MB_FileSave(Widget, XtPointer, XtPointer);


#endif /* _CREATEACTIONAPPSHELL_H_INCLUDED */
