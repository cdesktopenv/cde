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
/* $XConsortium: VirtFuncsI.h /main/8 1995/12/18 16:24:02 cde-hp $ */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:   VirtFunctsI.h
 **
 **  Project:
 **
 **  Description:  Public Header file for Canvas.c
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
#ifndef _DtHelpVirtFuncsI_h
#define _DtHelpVirtFuncsI_h


#ifdef __cplusplus
extern "C" {
#endif

/********    Private Defines Declarations    ********/

/********    Private Typedef Declarations    ********/

/********    Private Structures Declarations    ********/

/********    Private Define Declarations    ********/

/********    Private Macro Declarations    ********/

/********    Private Function Declarations    ********/

extern	void		_DtCvFontMetrics(
				_DtCanvasStruct	*canvas,
				_DtCvPointer	 font_handle,
				_DtCvUnit	*ret_ascent,
				_DtCvUnit	*ret_descent,
				_DtCvUnit	*ret_ave,
				_DtCvUnit	*ret_super_y,
				_DtCvUnit	*ret_sub_y);
extern	_DtCvUnit	_DtCvGetStringWidth(
				_DtCanvasStruct	 *canvas,
				_DtCvSegment	 *segment,
				void		 *string,
				int		  len);

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _DtHelpVirtFuncsI_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
