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
 *	$XConsortium: motifdefsP.h /main/3 1995/11/06 18:09:33 rswiston $
 *
 * @(#)motifdefsP.h	3.49 02 May 1995	cde_app_builder/src/abmf
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

#ifndef _ABMF_MOTIFDEFSP_H__
#define _ABMF_MOTIFDEFSP_H__

#include <ab_private/istr.h>

/*
 * Widget class names
 */
extern ISTRING	abmfP_abPopupMenu;		/* pseudo class */
extern ISTRING	abmfP_abPopupMenu2;		/* same as above */
extern ISTRING	abmfP_abPulldownMenu;		/* pseudo class */
extern ISTRING	abmfP_abPulldownMenu2;		/* same as above */
extern ISTRING	abmfP_dtComboBox;
extern ISTRING	abmfP_dtEditor;
extern ISTRING	abmfP_dtHelpDialog;
extern ISTRING	abmfP_dtHelpQuickDialog;
extern ISTRING	abmfP_dtMenuButton;
extern ISTRING	abmfP_dtSpinBox;
extern ISTRING	abmfP_dtTerm;
extern ISTRING	abmfP_dtTermPrim;
extern ISTRING	abmfP_xmArrowButton;
extern ISTRING	abmfP_xmBulletinBoard;
extern ISTRING	abmfP_xmCascadeButton;
extern ISTRING	abmfP_xmCommand;
extern ISTRING	abmfP_xmDialogShell;
extern ISTRING	abmfP_xmDragOverShell;
extern ISTRING	abmfP_xmDrawingArea;
extern ISTRING	abmfP_xmDrawnButton;
extern ISTRING	abmfP_xmFileSelectionBox;
extern ISTRING	abmfP_xmForm;
extern ISTRING	abmfP_xmFrame;
extern ISTRING	abmfP_xmLabel;
extern ISTRING	abmfP_xmList;
extern ISTRING	abmfP_xmMainWindow;
extern ISTRING	abmfP_xmManager;
extern ISTRING	abmfP_xmMenuShell;
extern ISTRING	abmfP_xmMessageBox;
extern ISTRING	abmfP_xmPanedWindow;
extern ISTRING	abmfP_xmPrimitive;
extern ISTRING	abmfP_xmPushButton;
extern ISTRING	abmfP_xmRowColumn;
extern ISTRING	abmfP_xmSash;
extern ISTRING	abmfP_xmScale;
extern ISTRING	abmfP_xmScrollbar;
extern ISTRING	abmfP_xmScrolledWindow;
extern ISTRING	abmfP_xmSelectionBox;
extern ISTRING	abmfP_xmSeparator;
extern ISTRING	abmfP_xmTearOffButton;
extern ISTRING	abmfP_xmText;
extern ISTRING	abmfP_xmTextField;
extern ISTRING	abmfP_xmToggleButton;
extern ISTRING	abmfP_xtApplicationShell;
extern ISTRING	abmfP_xtComposite;
extern ISTRING	abmfP_xtConstraint;
extern ISTRING	abmfP_xtCore;
extern ISTRING	abmfP_xtOverrideShell;
extern ISTRING	abmfP_xtShell;
extern ISTRING	abmfP_xtTopLevelShell;
extern ISTRING	abmfP_xtTransientShell;
extern ISTRING	abmfP_xtVendorShell;
extern ISTRING	abmfP_xtWMShell;

/* backwards compatibitility (these have inconsistent names) */
extern ISTRING	abmfP_applicationShell;
extern ISTRING	abmfP_topLevelShell;
extern ISTRING	abmfP_xmComboBox;
extern ISTRING	abmfP_xmMenuButton;
extern ISTRING	abmfP_xmSpinBox;
extern ISTRING	abmfP_xmTerm;
extern ISTRING	abmfP_xmTermPrim;

/*
 * Resources
 */
extern ISTRING	abmfP_XmNheight;
extern ISTRING	abmfP_XmNlabelString;
extern ISTRING	abmfP_XmNset;


int abmfP_motifdefs_init(void);	/* call this first!!! */

#endif /* _ABMF_MOTIFDEFSP_H__ */
