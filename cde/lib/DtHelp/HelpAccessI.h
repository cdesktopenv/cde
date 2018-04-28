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
/* $XConsortium: HelpAccessI.h /main/4 1995/10/26 12:23:38 rswiston $ */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:   HelpAccessI.h
 **
 **  Project:  Cache Creek (Rivers) Project:
 **
 **  Description:  Internal header file for HelpAccess.c
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
#ifndef _HelpAccessI_h
#define _HelpAccessI_h




/* The following Defines are for all the F1 key help requests */

/* Help Dialog */
#define DtHELP_dialogShell_STR  "_HOMETOPIC"

/* Menubar help access */
#define DtHELP_onHelpMenu_STR                "_HOMETOPIC"
#define DtHELP_onTableOfContenseMenu_STR     "TABLE-OF-CONTENTS"
#define DtHELP_onTasksMenu_STR               "TASKS"
#define DtHELP_onReferenceMenu_STR           "REFERENCE"
#define DtHELP_onMouseAndKeysMenu_STR        "MOUSE-AND-KEYBOARD"
#define DtHELP_onVersionMenu_STR             "_COPYRIGHT"

#define DtHELP_ExecutionPolicy_STR           "EXECUTION-POLICY"

/* Menu Item Id's (F1 Key) */
#define DtHELP_FileMenu_STR           "HELP-FILE-MENU"
#define DtHELP_EditMenu_STR           "HELP-EDIT-MENU"
#define DtHELP_SearchMenu_STR         "HELP-SEARCH-MENU"
#define DtHELP_NavigateMenu_STR       "HELP-NAVIGATE-MENU"
#define DtHELP_HelpMenu_STR           "HELP-HELP-MENU"
#define DtHELP_PopupMenu_STR          "HELP-POPUP-MENU"

/* Quick Help Dialog help access defines */
#define DtHELP_quickHelpShell_STR   "QUICK-HELP"
#define DtHELP_quickHelpBtn_STR     "QUICK-HELP"

/* History Dialog help access defines */
#define DtHELP_historyShell_STR       "HISTORY"
#define DtHELP_historyHelpBtn_STR     "HISTORY"

/* Printing Dialog help access defines */
#define DtHELP_printShell_STR       "PRINTING"
#define DtHELP_printHelpBtn_STR     "PRINTING"

/* Index Search Dialog help access defines */
#define DtHELP_srchIndexShell_STR   "INDEX-SEARCH"
#define DtHELP_srchIndexHelpBtn_STR "INDEX-SEARCH"

/* Volume Selection Dialog help access defines */
#define DtHELP_volSelectShell_STR    "VOLUME-SELECT"
#define DtHELP_volSelectHelpBtn_STR  "VOLUME-SELECT"



/****************************************************************************
 * Function:          void  _DtHelpCB (widget, clientData, callData) 
 *                                    Widget	widget;
 *                                    XtPointer	clientData;
 *                                    XtPointer   callData;
 * 
 * Parameters:           widget
 *                       clientData
 *                       callData
 *   
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Catches any "F1" key presses in a help dialog widget, or 
 *                  selects any of the help buttons in any of the help dialogs
 *                  dialogs.
 *
 ***************************************************************************/
extern void  _DtHelpCB(
    Widget widget,
    XtPointer clientData,
    XtPointer callData);





/*****************************************************************************
 * Function:	    void _DtHelpListFree (DtHelpListStruct *pHead);  
 *
 *
 * Parameters:      pHead   Specifies the head pointer to the help list.
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Frees all elements in a Help List.
 *
 *****************************************************************************/
extern void _DtHelpListFree(
    DtHelpListStruct * * pHead);





/*****************************************************************************
 * Function:	    DtHelpListStruct * _DtHelpListAdd(
 *                                       char *locationId,
 *                                       Widget widget,
 *                                       int widgetType,
 *                                       DtHelpListStruct *pHead)
 *                            
 *
 * Parameters:  
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Adds an element to the top of the given topicList.
 *
 *****************************************************************************/
extern DtHelpListStruct * _DtHelpListAdd(
    char *locationId,
    Widget widget,
    _DtHelpCommonHelpStuff * help,
    DtHelpListStruct **pHead);



#endif /* _HelpAccessI_h */
/* Do not add anything after this endif. */

