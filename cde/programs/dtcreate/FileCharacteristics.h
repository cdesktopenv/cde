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
/* $XConsortium: FileCharacteristics.h /main/5 1995/11/01 16:07:48 rswiston $ */

/*******************************************************************************
       FileCharacteristics.h
       This header file is included by FileCharacteristics.c

*******************************************************************************/

#ifndef _FILECHARACTERISTICS_H_INCLUDED
#define _FILECHARACTERISTICS_H_INCLUDED


#include <stdio.h>
#include <Xm/Xm.h>
#include <Xm/MwmUtil.h>
#include <Xm/DialogS.h>
#include <Xm/MenuShell.h>
#include "UxXt.h"

#include <Xm/RowColumn.h>
#include <Xm/Form.h>
#include <Xm/ToggleBG.h>
#include <Xm/TextF.h>
#include <Xm/Label.h>
#include <Xm/PushBG.h>
#include <Xm/Form.h>

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
#if 0
typedef struct
{
        Widget  UxFC_OkButton;
        Widget  UxFC_ClearButton;
        Widget  UxFC_CancelButton;
        Widget  UxFC_HelpButton;
        Widget  UxFC_MainLabel;
        Widget  UxFC_NamePatternLabel;
        Widget  UxFC_PermissionLabel;
        Widget  UxFC_PermissionToggle;
        Widget  UxFC_ContentsLabel;
        Widget  UxFC_ContentsBox;
        Widget  UxFC_TypeRowColumn;
        Widget  UxFC_StartByteLabel;
        Widget  UxFC_TypeLabel;
        Widget  UxFC_ContentsPatternLabel;
        Widget  UxFC_EndByteLabel;
        Widget  UxFC_ContentsToggle;
        Widget  UxFC_NameOrPathToggle;
        swidget UxUxParent;
} _UxCFileCharacteristics;

#ifdef CONTEXT_MACRO_ACCESS
static _UxCFileCharacteristics *UxFileCharacteristicsContext;
#define FC_OkButton             UxFileCharacteristicsContext->UxFC_OkButton
#define FC_ClearButton          UxFileCharacteristicsContext->UxFC_ClearButton
#define FC_CancelButton         UxFileCharacteristicsContext->UxFC_CancelButton
#define FC_HelpButton           UxFileCharacteristicsContext->UxFC_HelpButton
#define FC_MainLabel            UxFileCharacteristicsContext->UxFC_MainLabel
#define FC_NamePatternLabel     UxFileCharacteristicsContext->UxFC_NamePatternLabel
#define FC_PermissionLabel      UxFileCharacteristicsContext->UxFC_PermissionLabel
#define FC_PermissionToggle     UxFileCharacteristicsContext->UxFC_PermissionToggle
#define FC_ContentsLabel        UxFileCharacteristicsContext->UxFC_ContentsLabel
#define FC_ContentsBox          UxFileCharacteristicsContext->UxFC_ContentsBox
#define FC_TypeRowColumn        UxFileCharacteristicsContext->UxFC_TypeRowColumn
#define FC_StartByteLabel       UxFileCharacteristicsContext->UxFC_StartByteLabel
#define FC_TypeLabel            UxFileCharacteristicsContext->UxFC_TypeLabel
#define FC_ContentsPatternLabel UxFileCharacteristicsContext->UxFC_ContentsPatternLabel
#define FC_EndByteLabel         UxFileCharacteristicsContext->UxFC_EndByteLabel
#define FC_ContentsToggle       UxFileCharacteristicsContext->UxFC_ContentsToggle
#define FC_NameOrPathToggle     UxFileCharacteristicsContext->UxFC_NameOrPathToggle
#define UxParent                UxFileCharacteristicsContext->UxUxParent

#endif /* CONTEXT_MACRO_ACCESS */

extern Widget   FileCharacteristics;
extern Widget   FC_PermissionText;
extern Widget   FC_AndLabel2;
extern Widget   FC_ContentsPatternText;
extern Widget   FC_StringToggle;
extern Widget   FC_ByteToggle;
extern Widget   FC_ShortToggle;
extern Widget   FC_LongToggle;
extern Widget   FC_StartByteTextField;
extern Widget   FC_EndByteTextField;
extern Widget   FC_NameOrPathText;
extern Widget   FC_AndLabel1;
#endif

/*******************************************************************************
       Declarations of global functions.
*******************************************************************************/



Widget  create_FileCharacteristics(swidget _UxUxParent);
void    GetPermissionsPattern(char **ppszPermPattern);
void    init_FileCharacteristics_dialog_fields(FiletypeData *pFtD);
void    clear_FileCharacteristics_dialog_fields(void);
void    ParseAndUpdateID(FiletypeData *pFtD);
Boolean FileCharCheckFields(void);


#endif  /* _FILECHARACTERISTICS_H_INCLUDED */
