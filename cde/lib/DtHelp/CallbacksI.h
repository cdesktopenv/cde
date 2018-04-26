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
/* $XConsortium: CallbacksI.h /main/7 1996/02/27 20:06:41 cde-hal $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        CallbacksTG.h
 **
 **   Project:     TextGraphic Display routines
 **
 **  
 **   Description: Header file for CallbacksTG.h
 **
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
#ifndef _DtHelpCallbacksI_h
#define _DtHelpCallbacksI_h

#ifdef __cplusplus
extern "C" {
#endif
/*****************************************************************************
 *              Semi Public Routines
 *****************************************************************************/
extern	Boolean	_DtHelpCancelSelection(
			XtPointer client_data);
extern	void	_DtHelpCleanAndDrawWholeCanvas(
			XtPointer client_data);
extern	void	_DtHelpSearchMoveTraversal(
			XtPointer client_data,
			int search_hit_index);

/*****************************************************************************
 *              Public Routines
 *****************************************************************************/
extern	void	_DtHelpClearSelection (
			XtPointer	client_data );
extern	void	_DtHelpClickOrSelectCB (
			Widget		widget,
			XtPointer	client_data,
			XtPointer	call_data );
extern	void	_DtHelpEndSelectionCB (
			Widget		widget,
			XtPointer	client_data,
			XtPointer	call_data );
extern  void    _DtHelpEnterLeaveCB (
			Widget          widget,
			XtPointer       client_data,
			XEvent          *event );
extern	void	_DtHelpExposeCB (
			Widget		widget,
			XtPointer	client_data,
			XtPointer	call_data );
extern	void	_DtHelpFocusCB (
			Widget		widget,
			XtPointer	client_data,
			XEvent		*event );
extern	void	_DtHelpGetClearSelection (
			Widget		widget,
			XtPointer	client_data);
extern	void	_DtHelpHorzScrollCB (
			Widget		widget,
			XtPointer	client_data,
			XtPointer	call_data );
extern	void	_DtHelpInitiateClipboard (
			XtPointer	client_data );
extern	void    _DtHelpLoseSelectionCB (
			Widget   widget,
			Atom    *selection );
extern	void	_DtHelpMoveBtnFocusCB (
			Widget		widget,
			XtPointer	client_data,
			XEvent		*event );
extern	void	_DtHelpMouseMoveCB (
			Widget		widget,
			XtPointer	client_data,
			XEvent		*event );
extern	void	_DtHelpResizeCB (
			Widget		widget,
			XtPointer	client_data,
			XtPointer	call_data );
extern	void	_DtHelpVertScrollCB (
			Widget		widget,
			XtPointer	client_data,
			XtPointer	call_data );
extern	void	_DtHelpVisibilityCB (
			Widget		widget,
			XtPointer	client_data,
			XEvent		*event );

#ifdef __cplusplus
}
#endif
#endif /* _DtHelpCallbacksI_h */
