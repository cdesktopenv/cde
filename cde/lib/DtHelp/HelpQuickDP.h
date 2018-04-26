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
/* $XConsortium: HelpQuickDP.h /main/6 1996/04/05 14:41:19 mgreess $ */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:   HelpQuickDP.h
 **
 **  Project:  CDE 1.0 Common Desktop Environment
 **
 **  Description:  Privite Header file for HelpQuickD.c 
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

#ifndef _DtHelpQuickDP_h
#define _DtHelpQuickDP_h

#include <Xm/BulletinBP.h>

#include <Dt/HelpQuickD.h>

#include "HelpQuickDI.h"
#include "HelpDialogP.h"


#define DtNO_JUMP_UPDATE          1
#define DtJUMP_UPDATE             2


#define XmDIALOG_SUFFIX            "_popup"
#define XmDIALOG_SUFFIX_SIZE       6


/* Defines for use in allocation geometry matrix. */
#define TB_MAX_WIDGETS_VERT     7
#define TB_MAX_NUM_WIDGETS      12



/* Class Part Structure Definition */

typedef struct
{
	XtPointer		extension;      /* Pointer to extension record */
} DtHelpQuickDialogWidgetClassPart;




/* Full class record declaration */

typedef struct _DtHelpQuickDialogWidgetClassRec
{
	CoreClassPart			   core_class;
	CompositeClassPart		   composite_class;
	ConstraintClassPart		   constraint_class;
	XmManagerClassPart		   manager_class;
	XmBulletinBoardClassPart	   bulletin_board_class;
	DtHelpQuickDialogWidgetClassPart  selection_box_class;
} DtHelpQuickDialogWidgetClassRec;

externalref DtHelpQuickDialogWidgetClassRec dtHelpQuickDialogWidgetClassRec;


/* fields for the Quick Help widget record */

/* reuse the CommonHelpStuff structure from HelpDialogP.h */
/* reuse the BacktrackStuff structure from HelpDialogP.h */
/* reuse the PrintStuff structure from HelpDialogP.h */

typedef struct _DtHelpQuickHelpStuff
{
        int             pad;
        Widget          definitionBox;
        Boolean         minimize_buttons;

        /* Help Dialog Widgets */

        Widget          displayAreaFrame;
	Widget		separator;
	  
        Widget		closeButton;		/*  close button  */
	XmString	closeLabelString;
        XtCallbackList	closeCallback;

        Widget          moreButton;             /* Application button */
        XmString        moreLabelString;

        Widget          backButton;             /* Backtrack button */
        XmString        backLabelString;
     
	Widget		printButton;		/*  print  button */
	XmString	printLabelString;

	Widget		helpButton;		/*  help button  */
	XmString	helpLabelString;
} _DtHelpQuickHelpStuff;

typedef struct
{
       _DtHelpDisplayWidgetStuff	display;
       _DtHelpCommonHelpStuff	help;
       _DtHelpQuickHelpStuff	qhelp;
       _DtHelpBacktrackStuff	backtr;
       _DtHelpPrintStuff		print;
} _DtHelpQuickDialogWidgetPart;


/****************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************/

typedef struct _DtHelpQuickDialogWidgetRec
{
	CorePart		        core;
	CompositePart		        composite;
	ConstraintPart		        constraint;
	XmManagerPart		        manager;
	XmBulletinBoardPart	        bulletin_board;
	_DtHelpQuickDialogWidgetPart	qhelp_dialog;
} DtHelpQuickDialogWidgetRec;



/********    Private Function Declarations    ********/
extern XmGeoMatrix _DtHelpQuickDialogWidgetGeoMatrixCreate( 
                        Widget wid,
                        Widget instigator,
                        XtWidgetGeometry *desired) ;
extern Boolean _DtHelpQuickDialogWidgetNoGeoRequest( 
                        XmGeoMatrix geoSpec) ;
extern void _DtHelpQuickDialogGetCloseLabelString(
                        Widget wid,
                        int resource_offset,
                        XtArgVal *value );
extern void _DtHelpQuickDialogGetHelpLabelString(
                        Widget wid,
                        int resource_offset,
                        XtArgVal *value );

/********    End Private Function Declarations    ********/



#endif /* _DtHelpDialogP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
