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
/* $XConsortium: ResizeI.h /main/5 1995/10/26 12:31:35 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        ResizeI.h
 **
 **   Project:     TextGraphic Display routines
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
 **
 ****************************************************************************
 ************************************<+>*************************************/
#ifndef	_DtResizeI_h
#define	_DtResizeI_h

/*****************************************************************************
 *                 Defines
 *****************************************************************************/
/*****************************************************************************
 *                 Variables Global to the library.
 *****************************************************************************/
/*****************************************************************************
 *                 Routine Declarations
 *****************************************************************************/
extern	void	_DtHelpResizeDisplayArea (
			Widget	parent,
			XtPointer client_data,
			int	rows,
			int	columns );

#endif /* _DtResizeI_h */
