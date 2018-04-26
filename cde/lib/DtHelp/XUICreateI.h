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
/* $XConsortium: XUICreateI.h /main/10 1996/03/21 15:38:59 rcs $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        XUICreateI.h
 **
 **   Project:     Cde Help System
 **
 **   Description: Internal file for XUICreate.c
 **
 ****************************************************************************
 ************************************<+>*************************************/
/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992,
                 1993, 1994, 1996 Hewlett-Packard Company.
 * (c) Copyright 1993, 1994, 1996 International Business Machines Corp.
 * (c) Copyright 1993, 1994, 1996 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994, 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

#ifndef	_DtHelpCreateI_h
#define	_DtHelpCreateI_h

#include "Dt/CanvasP.h"

#ifdef __cplusplus
extern "C" {
#endif
/*****************************************************************************
 *                 Defines
 *****************************************************************************/
/*
 * scroll bar flags
 */
#define _DtHelpNONE         0
#define _DtHelpSTATIC       1
#define _DtHelpAS_NEEDED    2

#define	_DtHelpVERTICAL_SCROLLBAR		0
#define	_DtHelpHORIZONTAL_SCROLLBAR	1

/*
 * scroll bar macros
 */
#define	_DtHelpSET_AS_NEEDED(x,y)		((x) | (1 << y))
#define _DtHelpIS_AS_NEEDED(x, y)		((x) & (1 << y))

/*****************************************************************************
 *                 Semi-Public Routines
 *****************************************************************************/
extern	void	__DtHelpInitializeFontList (
			Display		*dpy,
			XFontStruct	*default_font);

/*****************************************************************************
 *                 Public Routines
 *****************************************************************************/
extern	XtPointer	_DtHelpCreateDisplayArea (
				Widget  parent,
				char   *name,
				short   vert_flag,
				short   horiz_flag,
				Boolean marker_flag,
				int	rows,
				int	columns,
				void	(*hyperTextCB)(),
				void	(*resizeCB)(),
				int	(*exec_ok_routine)(),
				XtPointer client_data,
				XmFontList  default_list );

extern	XtPointer	_DtHelpCreateOutputArea (
				Widget  parent,
				char   *name,
				short   vert_flag,
				short   horiz_flag,
				Boolean marker_flag,
				_DtCvValue honor_size,
				_DtCvRenderType render_type,
				Dimension	width,
				Dimension	height,
				unsigned short media_resolution,
				void	(*hyperTextCB)(),
				void	(*resizeCB)(),
				int	(*exec_ok_routine)(),
				XtPointer client_data,
				XmFontList  default_list );

#ifdef __cplusplus
}
#endif
#endif /* _DtHelpCreateI_h */
