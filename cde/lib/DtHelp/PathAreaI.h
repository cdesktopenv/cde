/* $XConsortium: PathAreaI.h /main/4 1995/10/26 12:30:48 rswiston $ */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:   PathAreaI.h
 **
 **  Project:  Cache Creek (Rivers) Project:
 **
 **  Description:  Internal header file for PathArea.c
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
#ifndef _PathAreaI_h
#define _PathAreaI_h


#define DtPATH_LIST_MAX    99





/*****************************************************************************
 * Function:	    void _DtHelpUpdatePathArea(char *locationId,
 *                                 DtHelpDialogWidget nw);  
 *
 *
 * Parameters:      locationId Specifies the ID string for the new topic we
 *                              are going to display in the HelpDialog widget.
 *
 *                  helpDialogWidget  Specifies the current help dialog widget.
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Updates the Path Display area on top of the help
 *                  dialog based on the new locationId we are about to display.
 *
 *****************************************************************************/
void _DtHelpUpdatePathArea(
    char *locationId,
    DtHelpDialogWidget nw);





/*****************************************************************************
 * Function:	    void _DtHelpBuildPathArea(Widget parent,
 *                                 DtHelpDialogWidget nw);  
 *
 *
 * Parameters:      parent      Specifies the widget ID of the help dialog you
 *                              want to set the topic in.
 *                  nw          Specifies the current help dialog widget.
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Creates/updates the Path Display area on top of the help 
 *                  dialog.
 *
 *****************************************************************************/
extern void _DtHelpBuildPathArea(
    Widget parent,
    DtHelpDialogWidget nw);






#endif /* _PathAreaI_h */
/* Do not add anything after this endif. */











