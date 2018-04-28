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
 *	$XConsortium: instancesP.h /main/3 1995/11/06 18:06:09 rswiston $
 *
 * @(#)instancesP.h	3.33 02 May 1995
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

#ifndef _ABMF_INSTANCESP_H_
#define _ABMF_INSTANCESP_H_

#include "write_codeP.h"
#include "motifdefsP.h"

STRING	abmfP_get_widget_parent_name(GenCodeInfo genCodeInfo, ABObj obj);

int	abmfP_write_create_widgets_for_comp_obj(
		GenCodeInfo	genCodeInfo,
		ABObj		obj
		);

int	abmfP_write_create_widgets_for_one_obj(
		GenCodeInfo	genCodeInfo,
		ABObj		obj
		);

int	abmfP_write_add_callbacks_and_actions(
			GenCodeInfo	genCodeInfo,
			ABObj		obj
			);

int	abmfP_write_add_callback(
			 GenCodeInfo genCodeInfo,
			 STRING resource,
			 STRING func_name,
			 ABObj src,
			 ABObj dest);

int	abmfP_write_add_callback_with_string(
			 GenCodeInfo genCodeInfo,
			 STRING resource,
			 STRING func_name,
			 ABObj src,
			 STRING client_data);

int	abmfP_write_help_indirect_callback(
			GenCodeInfo genCodeInfo,
			ABObj src_obj, 
			ABObj help_obj);

int	write_widget_creation_proc(
			GenCodeInfo	genCodeInfo, 
			ABObj		obj
			);

#define	ObjWClassIs(obj, motif_class) \
	    (util_streq(obj_get_class_name(obj), istr_string(motif_class)))

#define ObjWClassIsApplicationShell(obj) \
		(ObjWClassIs((obj), abmfP_applicationShell))

#define ObjWClassIsArrowButton(obj) \
		(ObjWClassIs((obj), abmfP_xmArrowButton))

#define	ObjWClassIsBulletinBoard(obj) \
		(ObjWClassIs((obj), abmfP_xmBulletinBoard))

#define ObjWClassIsCommand(obj) \
		(ObjWClassIs((obj), abmfP_xmCommand))

#define ObjWClassIsComposite(obj) \
		(ObjWClassIs((obj), abmfP_xtComposite))

#define ObjWClassIsConstraint(obj) \
		(ObjWClassIs((obj), abmfP_xtConstraint))

#define	ObjWClassIsDialogShell(obj) \
		(ObjWClassIs((obj), abmfP_xmDialogShell))

#define ObjWClassIsDrawingArea(obj) \
		(ObjWClassIs((obj), abmfP_xmDrawingArea))

#define ObjWClassIsFileSelectionBox(obj) \
		(ObjWClassIs((obj), abmfP_xmFileSelectionBox))

#define	ObjWClassIsForm(obj) \
		(ObjWClassIs((obj), abmfP_xmForm))

#define	ObjWClassIsFrame(obj) \
		(ObjWClassIs((obj), abmfP_xmFrame))

#define ObjWClassIsLabel(obj) \
		(ObjWClassIs((obj), abmfP_xmLabel))

#define ObjWClassIsList(obj) \
		(ObjWClassIs((obj), abmfP_xmList))

#define ObjWClassIsMainWindow(obj) \
		(ObjWClassIs((obj), abmfP_xmMainWindow))

#define ObjWClassIsManager(obj) \
		(ObjWClassIs((obj), abmfP_xmManager))

#define	ObjWClassIsMenuShell(obj) \
		(ObjWClassIs((obj), abmfP_xmMenuShell))

#define ObjWClassIsMessageBox(obj) \
		(ObjWClassIs((obj), abmfP_xmMessageBox))

#define ObjWClassIsOverrideShell(obj) \
		(ObjWClassIs((obj), abmfP_xtOverrideShell))

#define	ObjWClassIsPanedWindow(obj) \
		(ObjWClassIs((obj), abmfP_xmPanedWindow))

#define	ObjWClassIsPopupMenu(obj) \
		(   (ObjWClassIs((obj), abmfP_abPopupMenu)) \
		 || (ObjWClassIs((obj), abmfP_abPopupMenu2)) )

#define	ObjWClassIsPrimitive(obj) \
		(ObjWClassIs((obj), abmfP_xmPrimitive))

#define	ObjWClassIsPulldownMenu(obj) \
		(   (ObjWClassIs((obj), abmfP_abPulldownMenu)) \
		 || (ObjWClassIs((obj), abmfP_abPulldownMenu2)) )

#define	ObjWClassIsPushButton(obj) \
		(ObjWClassIs((obj), abmfP_xmPushButton))

#define	ObjWClassIsRowColumn(obj) \
		(ObjWClassIs((obj), abmfP_xmRowColumn))

#define	ObjWClassIsScrolledWindow(obj) \
		(ObjWClassIs((obj), abmfP_xmScrolledWindow))

#define ObjWClassIsSelectionBox(obj) \
		(ObjWClassIs((obj), abmfP_xmSelectionBox))

#define ObjWClassIsShell(obj) \
		(ObjWClassIs((obj), abmfP_xtShell))

#define	ObjWClassIsTextField(obj) \
		(ObjWClassIs((obj), abmfP_xmTextField))

#define	ObjWClassIsText(obj) \
		(ObjWClassIs((obj), abmfP_xmText))

#define ObjWClassIsTopLevelShell(obj) \
		(ObjWClassIs((obj), abmfP_topLevelShell))

#define ObjWClassIsTransientShell(obj) \
		(ObjWClassIs((obj), abmfP_xtTransientShell))

#define ObjWClassIsVendorShell(obj) \
		(ObjWClassIs((obj), abmfP_xtVendorShell))

#define ObjWClassIsWMShell(obj) \
		(ObjWClassIs((obj), abmfP_xtWMShell))

#define ObjWClassIsBaseWinShell(obj) \
	    	(ObjWClassIsApplicationShell(obj))

/*
 * Complex checks
 */
BOOL	ObjWClassIsCompositeSubclass(ABObj obj);
BOOL	ObjWClassIsManagerSubclass(ABObj obj);
BOOL	ObjWClassIsShellSubclass(ABObj obj);

#endif /*_ABMF_INSTANCESP_H_ */
