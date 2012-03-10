/* $XConsortium: UtilI.h /main/4 1995/11/08 09:23:08 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        Util.h
 **
 **   Project:     helpviewer 3.0
 **
 **   Description: Structures and defines needed HelpUtil.h in our Preview
 **                tool (aka, helpviewer).
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/

#ifndef _UtilI_h
#define _UtilI_h



#define DEFAULT_ROWS         20
#define DEFAULT_COLUMNS      20




/****************************************************************
 *
 *  Cache List Info Structure Definition.
 *
 ****************************************************************/
typedef struct _CacheListStruct {
    Widget                    helpDialog;
    Boolean                   inUseFlag;
    struct _CacheListStruct   *pNext;
    struct _CacheListStruct   *pPrevious;
} CacheListStruct;






/****************************************************************************
 * Function:	    void DisplayTopic(
 *                              Widget parent,
 *                              char *accessPath,
 *                              char *idString)
 *
 * Parameters:      
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Displays a new Cache Creek help topic in a new help dialog
 *                  widget.
 *
 ****************************************************************************/
extern void DisplayTopic (
    Widget  parent,
    char    *accessPath,
    char    *idString,
    int     argc,
    char    **argv);



/****************************************************************************
 * Function:	    void DisplayFile(
 *
 * Parameters:      
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Displays a ascii file in a quick help dialog.
 *
 ****************************************************************************/
extern void DisplayFile (
    Widget  parent,
    char    *file);




/****************************************************************************
 * Function:	    void DisplayMan();
 *
 * Parameters:      
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Displays a UNIX man page in a quick help dialog.
 *
 ****************************************************************************/
extern void DisplayMan (
    Widget  parent,
    char    *man,
    int     closeOption);


/****************************************************************************
 * Function:         CloseAndExitCB(
 *                              Widget w,
 *                              XtPointer clientData, 
 *                      	XtPointer callData
 *
 * Parameters:      
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Closes the helpview process when the users
 *                  closes either a man page view or a ascii
 *                  text file view.
 *
 ***************************************************************************/
extern void CloseAndExitCB(
    Widget w,
    XtPointer clientData,
    XtPointer callData);



/****************************************************************************
 * Function:         CloseHelpCB(
 *                              Widget w,
 *                              XtPointer clientData, 
 *                      	XtPointer callData
 *
 * Parameters:      
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Process close requests on all Help Dialog widgets 
 *                  created and managed by this application.
 *
 ***************************************************************************/
extern void CloseHelpCB (
    Widget w,
    XtPointer clientData,
    XtPointer callData);




/****************************************************************************
 * Function:	    void ProcessLinkCB(
 *                              Widget w,
 *                              XtPointer  clientData, 
 *                      	XtPointer callData
 *
 * Parameters:      
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Process JUMP-NEW and APP-LINK hypertext requests in a 
 *                  given Help Dialog Window.
 *                 
 *                  This is the callback used for the DtNhyperLinkCallback
 *                  on each of the help dialog widges created.
 *
 ****************************************************************************/
extern void ProcessLinkCB (
    Widget w,
    XtPointer clientData,
    XtPointer callData);


/****************************************************************************
 * Function:	    CacheListStruct GetFromCache(
 *                                  Widget parent);
 *
 * Parameters:      
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Gets a free help node form our cache list.  If none are
 *                  free, it will return fallse and the calling routine will
 *                  create a new help dialog widget.
 *
 ****************************************************************************/
Boolean GetFromCache(
    Widget parent,
    CacheListStruct **pCurrentNode);





#endif /* _UtilI_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */








