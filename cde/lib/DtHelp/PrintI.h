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
/* $XConsortium: PrintI.h /main/4 1995/10/26 12:31:15 rswiston $ */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:   PrintI.h
 **
 **  Project:  Cache Creek (Rivers) Project:
 **
 **  Description:  Internal Header file for Print.c 
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
#ifndef _PrintI_h
#define _PrintI_h

/* Printer Type Defines */

#define _DtPRINT_ALL     1
#define _DtPRINT_SUB     2
#define _DtPRINT_CURRENT 3
#define _DtPRINT_TOC     4 

/* default helpprint executable */
extern char _DtHelpDefaultHelpPrint[];
/* paper size resource names */
extern char * _DtHelpPaperSizeNames[];
/* number paper size resource names */
extern int    _DtHelpPaperSizeNamesCnt;

/*****************************************************************************
 * Function:        void _DtHelpDisplayPrintDialog();
 *
 *
 * Parameters:      new      Specifies the help widget.
 *
 * Return Value:
 *
 * Purpose:         Creates and displays an instance of the print dialog.
 *
 *****************************************************************************/
void _DtHelpDisplayPrintDialog(
   Widget               widget,
   _DtHelpPrintStuff *   print,
   _DtHelpDisplayWidgetStuff * display,
   _DtHelpCommonHelpStuff * help);

/*****************************************************************************
 * Function:        void _DtHelpUpdatePrintDialog();
 *
 *
 * Parameters:      new      Specifies the help widget.
 *
 * Return Value:
 *
 * Purpose:         Updates the print dialog to reflect current topic
 *
 *****************************************************************************/
void _DtHelpUpdatePrintDialog(
    _DtHelpPrintStuff *         print,
    _DtHelpDisplayWidgetStuff * display,
    _DtHelpCommonHelpStuff *    help,
    Boolean                     setDefaults);

/************************************************************************
 * Function: _DtHelpPrintSetValues()
 *
 *      set print-related data
 *
 ************************************************************************/
void _DtHelpPrintSetValues (
    _DtHelpPrintStuff * currentPrint,
    _DtHelpPrintStuff * newPrint,
    _DtHelpDisplayWidgetStuff * newDisplay,
    _DtHelpCommonHelpStuff * newCommonHelp);


/*****************************************************************************
 * Function:	    void _DtHelpPrintJob(
 *                   
 *                            
 *
 * Parameters:  
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Sets up and forks off a print job to helpprint
 *
 *****************************************************************************/
void _DtHelpPrintJob(
    Widget	widget,
    char *	printExec,
    char *	printer,
    int		paperSize,
    char *	copies,
    char *	helpVolume,
    int 	helpType,
    char *	helpData,
    Boolean	printAll,
    Boolean	printSub,
    Boolean	printToc,
    char *	topicTitle);

extern void _DtHelpInitPrintStuff (
    _DtHelpPrintStuff * print);

extern void _DtHelpFreePrintStuff (
    _DtHelpPrintStuff * print,
    int                 cleanUpKind);


#endif /* _PrintI_h */
/* Do not add anything after this endif. */






