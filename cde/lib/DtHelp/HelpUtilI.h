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
/* $XConsortium: HelpUtilI.h /main/5 1995/10/26 12:26:30 rswiston $ */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:   HelpUtilI.h    
 **
 **  Project:  Rivers Project, 
 **
 **  Description:  Internal header file for our HelpUtil module.
 **  -----------
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 *******************************************************************
 *************************************<+>*************************************/


#ifndef _HelpUtilI_h
#define _HelpUtilI_h


#include "HelpDialogP.h"

/* default help4help volume */
extern char _DtHelpDefaultHelp4HelpVolume[];
extern char _DtHelpDefaultLocationId[];

/*

 *****************************************************************************
 * Function:    void _DtHelpErrorDialog(parent, message, helpLocationId)
 *
 * Parameters:      widget      Specifies the application's top level widget
 *                  message     Specifies the desired error message to 
 *                              display.
 *
 * Return Value:    None
 *
 * Purpose:         To display and error message in a motif error dialog.
 *
 *****************************************************************************
 */
void _DtHelpErrorDialog(
    Widget                   parent,
    char *                   message);

/*****************************************************************************
 * Function: _DtHelpFilterExecCmdStr
 *
 *   Args:
 *    helpWidget:       help widget requesting to exec the command
 *    pDisplayStuff:    ptr to the DisplayWidget stuff of the help widget
 *    commandStr:       command string to execute
 *    ret_cmdStr:       the screened & possibly rewritten command is put here
 *    ret_cmdWasAlias:  was the command an alias?
 *    ret_execPermitted: if executionPolicy permit exec & ret_cmdStr is valid
 *    ret_queryNeeded:  if executionPolicy requires a query before exec
 *
 * Description:
 *    ret_cmdStr gets memory owned by the calling function; it should be
 *    freed when no longer needed.  The string will be the same as the
 *    commandStr if commandStr was not an alias.  If the commandStr
 *    is an alias and if the alias is defined, the ret_cmdStr will be the
 *    value of the alias.  If the alias isn't defined, the ret_cmdStr will
 *    be the default command if available, or the alias name otherwise.
 *
 *    ret_cmdWasAlias will be True if the alias was defined or if the
 *    alias was undefined AND no default command was given.
 *    It will be False if no alias was used or if the alias was undefined
 *    and a default command was present.
 *
 * Returns:
 *    True:  if execPermitted and a valid command string
 *    False: if if execPermitted is False or invalid command string
 *
 * Comments:
 *    This code is written such that we don't need nor want to know
 *    whether it is a general or quick help widget.
 *
 * Warning:
 *    command string must be writable; it is written, but left
 *    unchanged when the function exits.
 *
 *****************************************************************************/
Boolean _DtHelpFilterExecCmdStr(
    Widget                   helpWidget,
    unsigned char            executionPolicy,
    const char *             commandStr,
    char * *                 ret_cmdStr,
    Boolean *                ret_cmdWasAlias,
    Boolean *                ret_execPermitted,
    Boolean *                ret_queryNeeded,
    char    *                hv_path);

typedef Boolean (*_DtHelpCeWaitProc)(Widget w,void * clientData);

/*********************************************************************
 * _DtHelpCeWaitAndProcessEvents
 *
 *  Purpose:
 *    _DtHelpCeWaitAndProcessEvents will process events and call
 *    the waitProc until waitProc returns False.   This function
 *    is useful to put up modal dialogs that must be reponded to
 *    in the midst of executing code that must remain on the call stack.
 *
 *  Warning:
 *    This function should only be used on modal dialogs.
 *
 *********************************************************************/

void
_DtHelpCeWaitAndProcessEvents (
    Widget             w,
    _DtHelpCeWaitProc  waitProc,
    void *             clientData);

/*****************************************************************************
 * Function: _DtHelpFilterExecCmd
 *
 *   Args:
 *    helpWidget:       help widget requesting to exec the command
 *    command:          command string to execute
 *                      DtHELP_EXECUTE_???
 *    execPolicy:       current policy setting
 *    ret_filteredCmdStr: filtered command string
 *****************************************************************************/
int _DtHelpFilterExecCmd(
    Widget        helpWidget,
    const char *  commandStr,
    unsigned char executionPolicy,
    Boolean       useQueryDialog,
    _DtHelpCommonHelpStuff * pHelpStuff,
    char * *      ret_filteredCmdStr,
    char *        hv_path);

/*****************************************************************************
 * Function: _DtHelpExecFilteredCmd
 *
 *   Args:
 *    helpWidget:       help widget requesting to exec the command
 *    command:          command string to execute
 *    modal:            is the execution modal (sync) or modeless (async)
 *    helpLocationId:   helpOnHelp file location for Help btn in error dialog
 *    pDisplayStuff:    ptr to the DisplayWidget stuff of the help widget
 *    pHelpStuff:       ptr to the CommonHelp stuff of the help widget
 *
 * Comments:
 *    This code is written such that we don't need nor want to know
 *    whether it is a general or quick help widget.
 *
 * Warning:
 *    command string must be writable; it is written, but left
 *    unchanged whent the function exits.
 *
 * Called by:
 *****************************************************************************/
void _DtHelpExecFilteredCmd(
    Widget                   helpWidget,
    char *                   commandStr,
    char *                   helpLocationId,
    _DtHelpDisplayWidgetStuff * pDisplayStuff,
    _DtHelpCommonHelpStuff * pHelpStuff);

/*****************************************************************************
 * Function:       Boolean _DtHelpExpandHelpVolume(DtHelpDialogWidget nw);
 *
 *
 * Parameters:     nw  Specifies the current help dialog widget.
 *
 * Return Value:    Boolean.
 *

 * Description: _DtHelpExpandHelpVolume looks for a $LANG variable in the
 *              helpAccesFile string and if found, replaces it with the
 *              current lang variable.
 *
 *****************************************************************************/
Boolean _DtHelpExpandHelpVolume(
   Widget			widget,
   _DtHelpDisplayWidgetStuff * display,
   _DtHelpCommonHelpStuff *    help,
   _DtHelpPrintStuff *         print);

/*****************************************************************************
 * Function:	   char *_DtHelpParseIdString(char * specification);
 *
 *
 * Parameters:     specification  Specifies an author defined help topic.
 *
 * Return Value:    Void.
 *
 * Description:   This function copies the locationId portion of the 
 *                specification and retruns it to the calling routine.
 *
 *****************************************************************************/
extern char *_DtHelpParseIdString(
   char *specification);

/*****************************************************************************
 * Function:	   char *_DtHelpParseAccessFile(char * specification);
 *
 *
 * Parameters:     specification  Specifies an author defined help topic.
 *
 * Return Value:    Void.
 *
 * Description:   This function copies the helpAccessFile portion of the 
 *                specification and retruns it to the calling routine.
 *
 *****************************************************************************/
extern char *_DtHelpParseAccessFile(
   char *specification);

/*****************************************************************************
 * Function:	    void _DtHelpTopicListAddToHead(
 *                                       char *locationId,
 *                                       char *topicTitle,
 *                                       int topicType,
 *                                       int maxNodex,
 *                                       char *accessPath,
 *                                       DtTopicListStruct **pHead,
 *                                       DtTopicListStruct **pTale,
 *                                       int *totalNodes) 
 *                            
 *
 * Parameters:  
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Adds an element to the top of the given topicList.
 *
 *****************************************************************************/
extern void _DtHelpTopicListAddToHead(
    char *locationId,
    XmString topicTitle,
    int topicType,
    int maxNodes,
    char *accessPath,
    DtTopicListStruct **pHead,
    DtTopicListStruct **pTale,
    int *totalNodes,
    int scrollPosition);

/*****************************************************************************
 * Function:	    void _DtTopicListDelUeteHead(
 *                                       DtTopicListStruct *pHead,
 *                                       DtTopicListStruct *pTale,
 *                                       totalNodes) 
 *                            
 *
 * Parameters:  
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Delets an element from the top of the given topicList.
 *
 *****************************************************************************/
extern void _DtHelpTopicListDeleteHead(
    DtTopicListStruct **pHead,
    DtTopicListStruct **pTale,
    int *totalNodes);

/*****************************************************************************
 * Function:	    void _DtHelpMapCB(
 *                   
 *                            
 *
 * Parameters:  
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Determins where a new child dialog should be mapped in
 *                  relation to its parent.
 *
 *****************************************************************************/
extern XtCallbackProc _DtHelpMapCB(
    Widget w,
    XtPointer client_data,
    XtPointer call_data );


/*****************************************************************************
 * Function:	    void _DtHelpMapCenteredCB(
 *                   
 *                            
 *
 * Parameters:  
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Determins where the center of our help dialog is and sets 
 *                  where new child dialog should be mapped such that its centered.
 *
 *****************************************************************************/
extern XtCallbackProc _DtHelpMapCenteredCB(
    Widget w,
    XtPointer client_data,
    XtPointer call_data);

/*****************************************************************************
 * Function:	   Boolean _DtHelpResolvePathname(
 *
 *
 * Parameters:     
 *
 * Return Value:    Boolean.
 *
 *
 * Description: _DtHelpResolvePathname attempts to validate and expand a path
 *              to a Cache Creek help access file.
 *
 *****************************************************************************/
extern Boolean _DtHelpResolvePathname(
   Widget widget,
   char **fileName,
   _DtHelpVolumeHdl *volumeHandle,
   char *sysVolumeSearchPath,
   char *userVolumeSearchPath);


/*****************************************************************************
 * Function:	   void _DtHelpDisplayDefinitionBox(
 *                            Widget parent,  
 *                            Widget definitionBox, 
 *                            char * path,
 *                            char * locationId);
 *       
 * Parameters:   
 *
 * Return Value:    
 *
 * Purpose: 	   This routine will create and post the definition box.
 *                 (e.g. the Quick Help Dialog widget)
 *
 ****************************************************************************/
extern void _DtHelpDisplayDefinitionBox(
    Widget parent,   
    Widget **definitionBox,
    char * path,
    char * locationId);

/*****************************************************************************
 * Function:	   void _DtHelpDisplayFormatError()
  *       
 * Parameters:   
 *
 * Return Value:    
 *
 * Purpose: 	   This routine generate and display the proper errror 
 *                 message to the display area as well as send the proper 
 *                 error to XmWarning() function.
 *
 ****************************************************************************/
void _DtHelpDisplayFormatError(
    XtPointer displayArea,
    Widget widget,   
    char *userError,
    char *systemError);

/*****************************************************************************
 * Function:       void _DtHelpCommonHelpInit()
  *
 * Parameters:
 *
 * Return Value:
 *
 * Purpose:        This routine inits common help stuff
 *
 ****************************************************************************/
void _DtHelpCommonHelpInit(
    _DtHelpCommonHelpStuff * help);

/*****************************************************************************
 * Function:       void _DtHelpCommonHelpClean()
  *
 * Parameters:
 *
 * Return Value:
 *
 * Purpose:        This routine cleans up common help stuff
 *
 ****************************************************************************/
void _DtHelpCommonHelpClean(
    _DtHelpCommonHelpStuff * help,
    Boolean                 destroy);

/*****************************************************************************
 * Function:       void _DtHelpSetDlgButtonsWidth
 *
 * Parameters:
 *
 * Return Value:
 *
 * Purpose:        This routine cleans up common help stuff
 *
 ****************************************************************************/
void _DtHelpSetButtonPositions(
    Widget     btnList[],
    int        numBtns,
    Dimension  minFormWidth,
    Dimension  btnMargins,
    Dimension  minBetweenBtnSpace);

/*****************************************************************************
 * Function:       _DtHelpXmFontListGetPropertyMax
 *
 * Parameters:
 *     fontList:  an XmFontList
 *     atom:      an XA_xxx value (see Vol 1, chpt 6.2.9)
 *     ret_propertyValue: ptr to long value that will hold the max value
 *
 * Return Value:
 *     True: got at least one value
 *     False: unable to get any value
 *
 * Purpose:
 *    This function returns the max value of XGetFontProperty calls
 *    for each font in the XmFontList
 *
 ****************************************************************************/
Boolean _DtHelpXmFontListGetPropertyMax(
        XmFontList fontList,
        Atom atom,
        unsigned long *ret_propertyValue);

#endif /* _HelpUtilI_h */
/* Do not add anything after this endif. */


