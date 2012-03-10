/* $XConsortium: HistoryI.h /main/5 1996/08/28 19:08:20 cde-hp $ */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:   HistoryI.h
 **
 **  Project:  Cache Creek (Rivers) Project:
 **
 **  Description:  Internal header file for HistoryI.c 
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
#ifndef _HistoryI_h
#define _HistoryI_h


#define DtHISTORY_LIST_MAX     999   /* ??? This should be a resource ??? */




/*****************************************************************************
 * Function:	    void _DtHelpDisplayHistoryInfo(Widget nw);
 *                             
 * 
 * Parameters:      nw        Specifies the name of the current help dialog 
 *                            widget.
 *
 * Return Value:
 *
 * Purpose: 	    Displays the pre-created history dialog.
 *
 *****************************************************************************/
extern void _DtHelpDisplayHistoryInfo(
    Widget nw);



/*****************************************************************************
 * Function:	    void _DtHelpUpdateHistoryList(char *locationId,
 *                                 Widget nw);  
 *
 *
 * Parameters:      parent      Specifies the ID string for the new topic we
 *                              are going to display in the HelpDialog widget.
 *
 *                  helpDialogWidget  Specifies the current help dialog widget.
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Updates the Path Display area on top of the help
 *                  dialog.
 *
 *****************************************************************************/
extern void _DtHelpUpdateHistoryList(
    char *locationId,
    int topicType,
    Boolean vol_changed,
    Widget nw);




#endif /* _HistoryI_h */
/* Do not add anything after this endif. */











