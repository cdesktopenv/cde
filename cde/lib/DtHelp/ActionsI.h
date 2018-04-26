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
/* $XConsortium: ActionsI.h /main/5 1995/12/08 13:00:31 cde-hal $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        ActionsI.h
 **
 **   Project:     Display area routines
 **
 **   Description: Header file for Actions.c
 **
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 ****************************************************************************
 ************************************<+>*************************************/
#ifndef _DtHelpActionsI_h
#define _DtHelpActionsI_h

#ifdef __cplusplus
extern "C" {
#endif
/*****************************************************************************
 *              Semi Public Routines
 *****************************************************************************/
extern	void	_DtHelpActivateLink (
			Widget		widget,
			XEvent		*event,
			String		*params,
			Cardinal	*num_params);
extern	void	_DtHelpCopyAction (
			Widget		widget,
			XEvent		*event,
			String		*params,
			Cardinal	*num_params);
extern	void	_DtHelpDeSelectAll (
			Widget		widget,
			XEvent		*event,
			String		*params,
			Cardinal	*num_params);
extern	void	_DtHelpNextLink (
			Widget		widget,
			XEvent		*event,
			String		*params,
			Cardinal	*num_params);
extern	void	_DtHelpPageLeftOrRight (
			Widget		widget,
			XEvent		*event,
			String		*params,
			Cardinal	*num_params);
extern	void	_DtHelpPageUpOrDown (
			Widget		widget,
			XEvent		*event,
			String		*params,
			Cardinal	*num_params);
extern	void	_DtHelpSelectAll (
			Widget		widget,
			XEvent		*event,
			String		*params,
			Cardinal	*num_params);

#ifdef __cplusplus
}
#endif
#endif /* _DtHelpActionsI_h */
