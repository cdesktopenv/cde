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
/* $XConsortium: af_aux.h /main/5 1995/11/01 16:09:45 rswiston $ */
/*****************************************************************************/
/*                                                                           */
/*  af_aux.h                                                                 */
/*                                                                           */
/*  Header file for af_aux.c                                                 */
/*                                                                           */
/*****************************************************************************/

#ifndef _AF_AUX_H_INCLUDED
#define _AF_AUX_H_INCLUDED

#include "UxXt.h"

#ifndef STORAGECLASS
#ifdef  NOEXTERN
#define STORAGECLASS
#else
#define STORAGECLASS extern
#endif
#endif

/*****************************************************************************/
/*                                                                           */
/*  Constants                                                                */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*                                                                           */
/*  Swidget Macro Definitions                                                */
/*                                                                           */
/*****************************************************************************/
#define FILE_CHARACTERISTICS_SWID (FileCharacteristics)
#define FA_TINY_ICON_BUTTON_SWID (AF_TinyIconButton)
#define FA_MED_ICON_BUTTON_SWID (AF_MedIconButton)

/*****************************************************************************/
/*                                                                           */
/*  Widget Macro Definitions                                                 */
/*                                                                           */
/*****************************************************************************/
#define FILE_CHARACTERISTICS (UxGetWidget(FILE_CHARACTERISTICS_SWID))
#define FA_TINY_ICON_BUTTON (UxGetWidget(FA_TINY_ICON_BUTTON_SWID))
#define FA_MED_ICON_BUTTON (UxGetWidget(FA_MED_ICON_BUTTON_SWID))

/*****************************************************************************/
/*                                                                           */
/*  Function Declarations                                                    */
/*                                                                           */
/*****************************************************************************/


void load_filetype_icons (Widget, XtPointer, XmFileSelectionBoxCallbackStruct *);
void clear_filetype_icon (void);
Widget get_selected_filetype_icon (void);
void activateCB_filetype_icon (Widget, XtPointer, DtIconCallbackStruct *);

void readAFFromGUI (FiletypeData *);
void getAF_FiletypeName (FiletypeData *);
void getAF_IDChars (FiletypeData *);
void getAF_HelpText (FiletypeData *);
void getAF_Icons (FiletypeData *);
void getAF_OpenCmd (FiletypeData *);
void getAF_PrintCmd (FiletypeData *);

void init_AddFiletype (FiletypeData *);
void free_Filetypedata(FiletypeData *);
void init_AddFiletype_dialog_fields(FiletypeData *pFtD);
void clear_AddFiletype_dialog_fields(void);
void AddFiletypeToList();
void UpdateFiletypeDataArray();
Boolean AddFiletypeCheckFields(void);


#endif /* _AF_AUX_H_INCLUDED */
