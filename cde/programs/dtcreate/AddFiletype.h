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
/* $XConsortium: AddFiletype.h /main/5 1995/11/01 16:05:05 rswiston $ */
/*******************************************************************************
       AddFiletype.h
       This header file is included by AddFiletype.c

*******************************************************************************/

#ifndef _ADDFILETYPE_H_INCLUDED
#define _ADDFILETYPE_H_INCLUDED

#include <stdio.h>
#include <Xm/Xm.h>
#include <Xm/MwmUtil.h>
#include <Xm/DialogS.h>
#include <Xm/MenuShell.h>

#include <Xm/Separator.h>
#include <Xm/Text.h>
#include <Xm/ScrolledW.h>
#include <Xm/PushB.h>
#include <Xm/Form.h>
#include <Xm/Text.h>
#include <Xm/PushBG.h>
#include <Xm/TextF.h>
#include <Xm/Label.h>
#include <Xm/Form.h>

#include "UxXt.h"

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
        Widget  UxAddFiletype;
        Widget  UxAF_FiletypeFamilyNameLabel;
        Widget  UxAF_OkButton;
        Widget  UxAF_ApplyButton;
        Widget  UxAF_CancelButton;
        Widget  UxAF_HelpButton;
        Widget  UxAF_IdCharacteristicsText;
        Widget  UxAF_FiletypeIconHelpLabel;
        Widget  UxAF_IdCharacteristicsEdit;
        Widget  UxAF_FiletypePrintCmdLabel;
        Widget  UxAF_OpenFiletypeCmdLabel;
        Widget  UxAF_CommandsLabel;
        Widget  UxAF_FiletypeIconForm;
        Widget  UxAF_MED_IconGadget;
        Widget  UxAF_TINY_IconGadget;
        Widget  UxAF_FiletypeIconsEditButton;
        Widget  UxAF_FiletypeIconFindSetButton;
        Widget  UxAF_FiletypeIconsBoxLabel;
        Widget  UxAF_FiletypeHelpScrolledWindow;
        Widget  UxAF_separator2;
        Widget  UxAF_separator1;
        Widget  UxAF_IdCharacteristicsLabel;
        swidget UxUxParent;
} _UxCAddFiletype;

#ifdef CONTEXT_MACRO_ACCESS
static _UxCAddFiletype         *UxAddFiletypeContext;
#define AddFiletype             UxAddFiletypeContext->UxAddFiletype
#define AF_FiletypeFamilyNameLabel UxAddFiletypeContext->UxAF_FiletypeFamilyNameLabel
#define AF_OkButton             UxAddFiletypeContext->UxAF_OkButton
#define AF_ApplyButton          UxAddFiletypeContext->UxAF_ApplyButton
#define AF_CancelButton         UxAddFiletypeContext->UxAF_CancelButton
#define AF_HelpButton           UxAddFiletypeContext->UxAF_HelpButton
/*#define AF_IdCharacteristicsText UxAddFiletypeContext->UxAF_IdCharacteristicsText*/
#define AF_FiletypeIconHelpLabel UxAddFiletypeContext->UxAF_FiletypeIconHelpLabel
/*#define AF_IdCharacteristicsEdit UxAddFiletypeContext->UxAF_IdCharacteristicsEdit*/
#define AF_FiletypePrintCmdLabel UxAddFiletypeContext->UxAF_FiletypePrintCmdLabel
#define AF_OpenFiletypeCmdLabel UxAddFiletypeContext->UxAF_OpenFiletypeCmdLabel
#define AF_CommandsLabel        UxAddFiletypeContext->UxAF_CommandsLabel
#define AF_FiletypeIconForm     UxAddFiletypeContext->UxAF_FiletypeIconForm
/*
#define AF_MED_IconGadget       UxAddFiletypeContext->UxAF_MED_IconGadget
#define AF_TINY_IconGadget      UxAddFiletypeContext->UxAF_TINY_IconGadget
*/
#define AF_FiletypeIconsEditButton UxAddFiletypeContext->UxAF_FiletypeIconsEditButton
#define AF_FiletypeIconFindSetButton UxAddFiletypeContext->UxAF_FiletypeIconFindSetButton
#define AF_FiletypeIconsBoxLabel UxAddFiletypeContext->UxAF_FiletypeIconsBoxLabel
#define AF_FiletypeHelpScrolledWindow UxAddFiletypeContext->UxAF_FiletypeHelpScrolledWindow
#define AF_separator2           UxAddFiletypeContext->UxAF_separator2
#define AF_separator1           UxAddFiletypeContext->UxAF_separator1
#define AF_IdCharacteristicsLabel UxAddFiletypeContext->UxAF_IdCharacteristicsLabel
#define UxParent                UxAddFiletypeContext->UxUxParent

#endif /* CONTEXT_MACRO_ACCESS */

extern Widget   AF_FileTypeNameTextField;
extern Widget   AF_FiletypePrintCmdTextField;
extern Widget   AF_OpenCmdText;
extern Widget   AF_FiletypeHelpText;
extern Widget   AF_MED_IconGadget;
extern Widget   AF_TINY_IconGadget;

/*******************************************************************************
       Declarations of global functions.
*******************************************************************************/


void    activateCB_edit_id_characteristics (
			   Widget filesel, 
			   XtPointer cdata,
			   XmFileSelectionBoxCallbackStruct *cbstruct);
Widget  create_AddFiletype(swidget _UxUxParent);



#endif  /* _ADDFILETYPE_H_INCLUDED */

