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

/*
 *	$XConsortium: motifdefs.c /main/3 1995/11/06 18:09:19 rswiston $
 *
 *	@(#)motifdefs.c	1.13 02 May 1995	cde_app_builder/src/abmf
 *
 * 	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 */


/*
 *  motifdefs.c
 */

#include "motifdefsP.h"
#include <ab_private/objxm.h>

/*************************************************************************
**                                                                      **
**       Constants (#define and const)					**
**                                                                      **
**************************************************************************/

/*************************************************************************
**                                                                      **
**       Private Functions (C declarations and macros)			**
**                                                                      **
**************************************************************************/

/*************************************************************************
**                                                                      **
**       Data     	                                                **
**                                                                      **
**************************************************************************/

/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/


/*
 * Widget class names
 */
ISTRING	abmfP_abPopupMenu = NULL;
ISTRING	abmfP_abPopupMenu2 = NULL;
ISTRING	abmfP_abPulldownMenu = NULL;
ISTRING	abmfP_abPulldownMenu2 = NULL;
ISTRING	abmfP_dtComboBox = NULL;
ISTRING	abmfP_dtEditor = NULL;
ISTRING	abmfP_dtHelpDialog = NULL;
ISTRING	abmfP_dtHelpQuickDialog = NULL;
ISTRING	abmfP_dtMenuButton = NULL;
ISTRING	abmfP_dtSpinBox = NULL;
ISTRING	abmfP_dtTerm = NULL;
ISTRING	abmfP_dtTermPrim = NULL;
ISTRING	abmfP_xmArrowButton = NULL;
ISTRING	abmfP_xmBulletinBoard = NULL;
ISTRING	abmfP_xmCascadeButton = NULL;
ISTRING	abmfP_xmCommand = NULL;
ISTRING	abmfP_xmDialogShell = NULL;
ISTRING abmfP_xmDragOverShell = NULL;
ISTRING	abmfP_xmDrawingArea = NULL;
ISTRING	abmfP_xmDrawnButton = NULL;
ISTRING	abmfP_xmFileSelectionBox = NULL;
ISTRING	abmfP_xmForm = NULL;
ISTRING	abmfP_xmFrame = NULL;
ISTRING	abmfP_xmLabel = NULL;
ISTRING	abmfP_xmList = NULL;
ISTRING	abmfP_xmMainWindow = NULL;
ISTRING	abmfP_xmManager = NULL;
ISTRING	abmfP_xmMenuShell = NULL;
ISTRING	abmfP_xmMessageBox = NULL;
ISTRING	abmfP_xmPanedWindow = NULL;
ISTRING	abmfP_xmPrimitive = NULL;
ISTRING	abmfP_xmPushButton = NULL;
ISTRING	abmfP_xmRowColumn = NULL;
ISTRING abmfP_xmSash = NULL;
ISTRING	abmfP_xmScale = NULL;
ISTRING	abmfP_xmScrollbar = NULL;
ISTRING	abmfP_xmScrolledWindow = NULL;
ISTRING	abmfP_xmSelectionBox = NULL;
ISTRING	abmfP_xmSeparator = NULL;
ISTRING abmfP_xmTearOffButton = NULL;
ISTRING	abmfP_xmText = NULL;
ISTRING	abmfP_xmTextField = NULL;
ISTRING	abmfP_xmToggleButton = NULL;
ISTRING	abmfP_xtApplicationShell = NULL;
ISTRING	abmfP_xtComposite = NULL;
ISTRING	abmfP_xtConstraint = NULL;
ISTRING	abmfP_xtCore = NULL;
ISTRING	abmfP_xtOverrideShell = NULL;
ISTRING	abmfP_xtShell = NULL;
ISTRING	abmfP_xtTopLevelShell = NULL;
ISTRING	abmfP_xtTransientShell = NULL;
ISTRING	abmfP_xtVendorShell = NULL;
ISTRING	abmfP_xtWMShell = NULL;

/* backwards compatibitility (inconsistent names) */
ISTRING	abmfP_applicationShell = NULL;
ISTRING	abmfP_topLevelShell = NULL;
ISTRING	abmfP_xmComboBox = NULL;
ISTRING	abmfP_xmMenuButton = NULL;
ISTRING	abmfP_xmSpinBox = NULL;
ISTRING	abmfP_xmTerm = NULL;
ISTRING	abmfP_xmTermPrim = NULL;


/*
 * Resources
 */
ISTRING	abmfP_XmNheight = NULL;
ISTRING	abmfP_XmNlabelString = NULL;
ISTRING	abmfP_XmNset = NULL;


int 
abmfP_motifdefs_init(void)
{
    static BOOL	inited= FALSE;
    if (inited)
    {
	return 0;
    }
    inited= TRUE;

    /*
     * Widget class names.  The _<widget> string variables are declared as part
     * of libABobjXm.
     *
     * abPulldownMenu, and abPulldownMenu2 are equivalent. It's a minor bug
     * that we have two.
     */
    abmfP_abPopupMenu        = istr_const("xmPopupMenuWidgetClass");
    abmfP_abPopupMenu2       = istr_const("xmPopupMenuPseudoClass");
    abmfP_abPulldownMenu     = istr_const("xmPulldownMenuWidgetClass");
    abmfP_abPulldownMenu2    = istr_const("xmPulldownMenuPseudoClass");
    abmfP_dtComboBox         = istr_const(_dtComboBox);
    abmfP_dtEditor           = istr_const("dtEditorWidgetClass");
    abmfP_dtHelpDialog       = istr_const("dtHelpDialogWidgetClass");
    abmfP_dtHelpQuickDialog  = istr_const("dtHelpQuickDialogWidgetClass");
    abmfP_dtMenuButton 	     = istr_const(_dtMenuButton);
    abmfP_dtSpinBox 	     = istr_const(_dtSpinBox);
    abmfP_dtTerm 	     = istr_const(_dtTerm);
    abmfP_dtTermPrim 	     = istr_const("dtTermPrimitiveWidgetClass");
    abmfP_xmArrowButton	     = istr_const(_xmArrowButton);
    abmfP_xmBulletinBoard    = istr_const(_xmBulletinBoard);
    abmfP_xmCascadeButton    = istr_const(_xmCascadeButton);
    abmfP_xmCommand 	     = istr_const(_xmCommand);
    abmfP_xmDialogShell      = istr_const(_xmDialogShell);
    abmfP_xmDragOverShell    = istr_const("xmDragOverShellWidgetClass");
    abmfP_xmDrawingArea      = istr_const(_xmDrawingArea);
    abmfP_xmDrawnButton      = istr_const(_xmDrawnButton);
    abmfP_xmFileSelectionBox = istr_const(_xmFileSelectionBox);
    abmfP_xmForm 	     = istr_const(_xmForm);
    abmfP_xmFrame 	     = istr_const(_xmFrame);
    abmfP_xmLabel 	     = istr_const(_xmLabel);
    abmfP_xmList 	     = istr_const(_xmList);
    abmfP_xmMainWindow 	     = istr_const(_xmMainWindow);
    abmfP_xmManager	     = istr_const(_xmManager);
    abmfP_xmMenuShell 	     = istr_const(_xmMenuShell);
    abmfP_xmMessageBox	     = istr_const(_xmMessageBox);
    abmfP_xmPanedWindow      = istr_const(_xmPanedWindow);
    abmfP_xmPrimitive 	     = istr_const(_xmPrimitive);
    abmfP_xmPushButton 	     = istr_const(_xmPushButton);
    abmfP_xmRowColumn 	     = istr_const(_xmRowColumn);
    abmfP_xmSash	     = istr_const("xmSashWidgetClass");
    abmfP_xmScale 	     = istr_const(_xmScale);
    abmfP_xmScrollbar	     = istr_const(_xmScrollBar);
    abmfP_xmScrolledWindow   = istr_const(_xmScrolledWindow);
    abmfP_xmSelectionBox     = istr_const(_xmSelectionBox);
    abmfP_xmSeparator 	     = istr_const(_xmSeparator);
    abmfP_xmTearOffButton    = istr_const("xmTearOffButtonWidgetClass");
    abmfP_xmText 	     = istr_const(_xmText);
    abmfP_xmTextField 	     = istr_const(_xmTextField);
    abmfP_xmToggleButton     = istr_const(_xmToggleButton);
    abmfP_xtApplicationShell = istr_const("applicationShellWidgetClass");
    abmfP_xtComposite	     = istr_const("compositeWidgetClass");
    abmfP_xtConstraint	     = istr_const("constraintWidgetClass");
    abmfP_xtCore	     = istr_const("coreWidgetClass");
    abmfP_xtOverrideShell    = istr_const("overrideShellWidgetClass");
    abmfP_xtShell	     = istr_const("shellWidgetClass");
    abmfP_xtTopLevelShell    = istr_const("topLevelShellWidgetClass");
    abmfP_xtTransientShell   = istr_const("transientShellWidgetClass");
    abmfP_xtVendorShell	     = istr_const("vendorShellWidgetClass");
    abmfP_xtWMShell	     = istr_const("wmShellWidgetClass");

    /* backwards compatibitility (inconsistent names) */
    abmfP_applicationShell   = istr_dup(abmfP_xtApplicationShell);
    abmfP_topLevelShell      = istr_dup(abmfP_xtTopLevelShell);
    abmfP_xmComboBox	     = istr_dup(abmfP_dtComboBox);
    abmfP_xmMenuButton       = istr_dup(abmfP_dtMenuButton);
    abmfP_xmSpinBox	     = istr_dup(abmfP_dtSpinBox);
    abmfP_xmTerm	     = istr_dup(abmfP_dtTerm);
    abmfP_xmTermPrim	     = istr_dup(abmfP_dtTermPrim);


    /*
     * Resources
     */
    abmfP_XmNheight = istr_const("XmNheight");
    abmfP_XmNlabelString = istr_const("XmNlabelString");
    abmfP_XmNset = istr_const("XmNset");

    return 0;
}

