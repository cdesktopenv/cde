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
/* $XConsortium: ca_aux.h /main/5 1995/11/01 16:10:18 rswiston $ */
/***************************************************************************/
/*                                                                         */
/*  Header file for ca_aux.c                                               */
/*                                                                         */
/***************************************************************************/
#ifndef _CA_AUX_H_INCLUDED
#define _CA_AUX_H_INCLUDED

#include "UxXt.h"

#if !defined(_DIRENT_H)
#include <dirent.h>
#endif
#if !defined(_DtIcon_h_)
#include <Dt/Icon.h> 
#endif 

/***************************************************************************/
/*                                                                         */
/*  Constants                                                              */
/*                                                                         */
/***************************************************************************/
#define EQUAL      0
#define NOT_EQUAL  1

/***************************************************************************/
/*                                                                         */
/*  Macros                                                                 */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/*  Function Declarations                                                  */
/*                                                                         */
/***************************************************************************/

void initAD( ActionData *pAD );
void readCAFromGUI (struct _ActionData *ca_struct);
void getCAactionName (struct _ActionData *ca_struct);
void getCAdblClkCmd (struct _ActionData *ca_struct);
void getCAactionHelpText (struct _ActionData *ca_struct);
void getCAactionOpensText (struct _ActionData *ca_struct);
void getCAwindowType (struct _ActionData *ca_struct);
void writeCAToGUI (struct _ActionData *ca_struct);

void putCAactionName (struct _ActionData *ca_struct);
void putCAdblClkCmd (struct _ActionData *ca_struct);
void putCAfileTypes (struct _ActionData *ca_struct);
void putCAactionOpensText (struct _ActionData *ca_struct);
void putCAwindowType (struct _ActionData *ca_struct);
void putCAactionHelpText (struct _ActionData *ca_struct);
void putCAactionIcons (struct _ActionData *ca_struct);
void putCAdropFiletypes (ActionData *pAD);

void set_ca_dialog_height (void);
Widget get_selected_action_icon (void);
Boolean CreateActionAppShellCheckFields(void);
void FreeAndClearAD(ActionData *pAD);
void clear_CreateActionAppShell_fields(void);
Boolean compareAD(ActionData *pAD1, ActionData *pAD2);
ActionData *copyAD(ActionData *pAD);
void FreeResources (void);


/***************************************************************************/
/*                                                                         */
/*      Callbacks for Create Action 'File' menu                            */
/*                                                                         */
/***************************************************************************/

void activateCB_FileNew (Widget wid, XtPointer cdata,
                         XtPointer cbstruct);
void activateCB_FileOpen (Widget wid, XtPointer cdata,
                         XtPointer cbstruct);
void activateCB_FileQuit (Widget wid, XtPointer cdata,
                         XtPointer cbstruct);


/***************************************************************************/
/*                                                                         */
/*      Callbacks for Create Action 'Options' menu                         */
/*                                                                         */
/***************************************************************************/

void activateCB_ExpertOption (Widget wid, XtPointer client_data, XtPointer *cbs);
void valueChangedCB_ColorMonoOption (Widget, XtPointer,
                              XmToggleButtonCallbackStruct *);
void createCB_ColorMonoOption (Widget);


/***************************************************************************/
/*                                                                         */
/*      Callbacks for Create Action Panel Buttons                          */
/*                                                                         */
/***************************************************************************/

void activateCB_action_icon (Widget wid, XtPointer client_data, 
			     DtIconCallbackStruct *cbs);
void activateCB_add_filetype (Widget wid, XtPointer client_data,
                              XmPushButtonCallbackStruct *cbs);
void activateCB_edit_filetype (Widget wid, XtPointer client_data,
			       XmPushButtonCallbackStruct *cbs);


#endif /* _CA_AUX_H_INCLUDED */
