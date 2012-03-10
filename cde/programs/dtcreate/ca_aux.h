/* $XConsortium: ca_aux.h /main/5 1995/11/01 16:10:18 rswiston $ */
/***************************************************************************/
/*                                                                         */
/*  Header file for ca_aux.c                                               */
/*                                                                         */
/***************************************************************************/
#ifndef _CA_AUX_H_INCLUDED
#define _CA_AUX_H_INCLUDED

#include "UxXt.h"

#if defined(USL) || defined(__uxp__)
#if !defined(_DIRENT_H)
#include <dirent.h>
#endif
#if !defined(_DtIcon_h_)
#include <Dt/Icon.h> 
#endif 
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
