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
/* $XConsortium: HelpDialogI.h /main/6 1996/08/28 19:07:35 cde-hp $ */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:   HelpDialogI.h
 **
 **  Project:  Cache Creek (Rivers) Project:
 **
 **  Description:  Internal header file for HelpDialog Widget.
 **  -----------
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 **
 *******************************************************************
 *************************************<+>*************************************/
#ifndef _HelpDialogI_h
#define _HelpDialogI_h

#include <Dt/HelpDialog.h>
#include "HelpDialogP.h"


/* Update list defines use in _DtHelpUpdateDisplayArea */

#define DtHISTORY_AND_JUMP      1
#define DtHISTORY_ONLY          2
#define DtJUMP_ONLY             3


/* Defines for processing a request to "Top Level..." */
#define DtBROWSER_HOME          _DtHelpDefaultLocationId
#define DtBROWSER_NAME          "browser.hv"


/* Jump List Max */

#define DtJUMP_LIST_MAX         999



/****************************************************************
 *
 *  Current Color Info Structure Definition
 *
 ****************************************************************/
typedef	struct	_colorStruct {
    Pixel	background;
    Pixel	foreground;
    Pixel	topShadow;
    Pixel	bottomShadow;
    Pixel	select;
} ColorStruct;


/*****************************************************************************
 * Function:	     void _DtHelpUpdateDisplayArea(
 *                             char                  *locationId,
 *                             DtHelpDialogWidget   nw,
 *                             int                   listUpdateType, 
 *                             int                   topicUpdateType);
 *
 *
 * Parameters:      locationId
 *
 *                  nw
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Updates the History List for the current help dialog.
 *
 *****************************************************************************/
extern void _DtHelpUpdateDisplayArea(
    char *locationId,
    DtHelpDialogWidget nw,
    Boolean vol_changed,
    int listUpdateType,
    int topicUpdateType);


/*****************************************************************************
 * Function:	     void _DtHelpUpdateJumpList(
 *                                  char *topicInfo,
 *                             DtHelpDialogWidget nw);
 *
 *
 * Parameters:      topicInfo
 *
 *                  nw
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Updates the History List for the current help dialog.
 *
 *****************************************************************************/
extern void _DtHelpUpdateJumpList(
    char *topicInfo,
    int topicType,
    Widget nw);


/*****************************************************************************
 * Function:	    void _DtHelpDialogHypertextCB(
 *                              XtPointer pDisplayAreaStruct,
 *                              XtPointer clientData,
 *                      	DtHelpHyperTextStruct *hyperData)
 *
 * Parameters:      pDisplayAreaStruct  Specifies the curretn display are info.
 *
 *                  hyperData           Specifies the current hypertext info
 *                                      structure.
 *
 *                  clientData          Specifies the client data passed into
 *                                      the hypertext callback. 
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Process all hypertext requests in a given Help Dialogs
 *                  display area.
 *
 *****************************************************************************/
extern void _DtHelpDialogHypertextCB (
    XtPointer pDisplayAreaStruct,
    XtPointer clientData,
    DtHelpHyperTextStruct *hyperData);

/*****************************************************************************
 * Function:	    void _DtHelpSetupDisplayType(DtHelpDialogWidget nw);  
 *                                            int updateType);  
 *
 *
 * Parameters:      nw          Specifies the current help dialog widget.
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Determins the type of topic the user want's to display
 *                  in the current help dialog and sets it up for display.
 *
 *****************************************************************************/
extern void _DtHelpSetupDisplayType(
     DtHelpDialogWidget nw,
     Boolean vol_changed,
     int updateType);

/*****************************************************************************
 * Function:	    void _DtHelpTopicListFree (DtTopicListStruct *pHead);  
 *
 *
 * Parameters:      pHead   Specifies the head pointer to the topic list.
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Frees all elements in a Topic List.
 *
 *****************************************************************************/
extern void _DtHelpTopicListFree(
    DtTopicListStruct *pHead);

/*****************************************************************************
 * Function:	   extern void DisplayHistoryCB(
 *                            Widget w,   
 *                            XtPointer client_data,
 *                            XtPointer call_data);
 *       
 * Parameters:   
 *
 * Return Value:    
 *
 * Purpose: 	   This routine will cause the history dialog to be 
 *                 posted.
 *
 ****************************************************************************/
extern void  _DtHelpDisplayHistoryCB(
    Widget w,
    XtPointer client_data,
    XtPointer call_data );

/*****************************************************************************
 * Function:	   extern void _DtHelpDisplayBackCB(
 *                            Widget w,   
 *                            XtPointer client_data,
 *                            XtPointer call_data);
 *       
 * Parameters:   
 *
 * Return Value:    
 *
 * Purpose: 	   This routine will cause the top element in the jump
 *                 stack to be displayed.
 *
 ****************************************************************************/
extern void  _DtHelpDisplayBackCB(
    Widget w,
    XtPointer client_data,
    XtPointer call_data);


/*****************************************************************************
 * Function:	   extern void _DtHelpDuplicateWindowCB(
 *                             Widget w,   
 *                             XtPointer client_data,
 *                             XtPointer call_data);
 *       
 * Parameters:   
 *
 * Return Value:    
 *
 * Purpose: 	   This routine creates a new help widget by forcing the
 *                 equivelent of a Jump New hyper text call
 *
 ****************************************************************************/
extern void  _DtHelpDuplicateWindowCB(
    Widget w,
    XtPointer client_data,
    XtPointer call_data);

/*****************************************************************************
 * Function:	       void _DtHelpDisplayIndexCB(
 *                            Widget w,   
 *                            XtPointer client_data,
 *                            XtPointer call_data);
 *       
 * Parameters:   
 *
 * Return Value:    
 *
 * Purpose: 	   This routine will cause the index dialog to be 
 *                 posted.
 *
 ****************************************************************************/
extern void  _DtHelpDisplayIndexCB(
    Widget w,
    XtPointer client_data,
    XtPointer call_data);

/*****************************************************************************
 * Function:	       void  _DtHelpDisplayBrowserHomeCB(
 *                            Widget w,   
 *                            XtPointer client_data,
 *                            XtPointer call_data);
 *       
 * Parameters:   
 *
 * Return Value:    
 *
 * Purpose: 	   This routine will cause the top level of the browser.hv 
 *                 help volume to be displayed.
 *
 ****************************************************************************/
extern void  _DtHelpDisplayBrowserHomeCB(
    Widget w,
    XtPointer client_data,
    XtPointer call_data);

/*****************************************************************************
 * Function:        _DtHelpFilterExecCmdCB
 *
 *  clientData:     The general help dialog widget
 *  cmdStr:         cmd string to filter
 *  ret_filteredCmdStr:  string after filtering.  NULL if exec denied
 *
 * Return Value:    0: ok, < 0: error
 *
 * Purpose:         filter an execution command using executionPolicy rsrc
 *
 * Memory:
 *   The caller must free memory allocated for the ret_filteredCmdStr
 *****************************************************************************/
extern int _DtHelpFilterExecCmdCB(
    void *   clientData,
    const char *   cmdStr,
    char * * ret_filteredCmdStr);

#endif /* _HelpDialogI_h */
/* Do not add anything after this endif. */
