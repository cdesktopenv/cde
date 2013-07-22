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
/* $XConsortium: XInterfaceI.h /main/8 1995/12/18 16:23:57 cde-hp $ */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:   XInterface.h
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
#ifndef _XInterface_h
#define _XInterface_h


#ifdef __cplusplus
extern "C" {
#endif

/********    Public Defines Declarations    ********/

/********    Public Typedef Declarations    ********/

/********    Public Structures Declarations    ********/

/********    Public Macro Declarations    ********/

/********    Semi-Private Function Declarations    ********/
extern	void		 _DtHelpDADestroyRegion (
				_DtCvPointer	 client_data,
				_DtCvPointer	 reg_info);
extern	void		 _DtHelpDADestroyGraphic (
				_DtCvPointer	 client_data,
				_DtCvPointer	 graphic_ptr);
extern	void		 _DtHelpDADestroySpc (
				_DtCvPointer	 client_data,
				_DtCvPointer	 spc_handle);
extern	const char	*_DtHelpDAGetSpcString(int idx);
extern	_DtCvStatus	 _DtHelpDALoadGraphic (
				_DtCvPointer	 client_data,
				char		*vol_xid,
				char		*topic_xid,
				char		*file_xid,
				char		*format,
				char		*method,
				_DtCvUnit	*ret_width,
				_DtCvUnit	*ret_height,
				_DtCvPointer	*ret_region);
extern	void		 _DtHelpDAResolveFont (
				_DtCvPointer	 client_data,
				char		*lang,
				const char	*char_set,
				_DtHelpFontHints font_attr,
				_DtCvPointer	*ret_font);
extern	_DtCvStatus	 _DtHelpDAResolveSpc (
				_DtCvPointer	 client_data,
				char		*lang,
				const char	*char_set,
				_DtHelpFontHints font_attr,
				const char	*spc_string,
				_DtCvPointer	*ret_handle,
				_DtCvUnit	*ret_width,
				_DtCvUnit	*ret_height,
				_DtCvUnit	*ret_ascent);

/********    Virtual UI Function Declarations    ********/

extern	_DtCvStatus	_DtHelpDABuildSelection (
				_DtCvPointer	 client_data,
				_DtCvElemType	 elem_type,
				unsigned int	 mask,
				_DtCvPointer	*prev_info,
				_DtCvUnit	 space,
				_DtCvUnit	 width,
				_DtCvFlags	 flags,
				_DtCvPointer	 data);
extern	void		_DtHelpDAGetCvsMetrics (
				_DtCvPointer		 client_data,
				_DtCvElemType	 elem_type,
				_DtCvPointer	 ret_metrics);
extern	void		_DtHelpDAGetFontMetrics (
				_DtCvPointer	 client_data,
				_DtCvPointer	 font_ptr,
				_DtCvUnit	*ret_ascent,
				_DtCvUnit	*ret_descent,
				_DtCvUnit	*char_width,
				_DtCvUnit	*ret_super,
				_DtCvUnit	*ret_sub);
extern	_DtCvUnit	_DtHelpDAGetStrWidth (
				_DtCvPointer	 client_data,
				_DtCvElemType	 elem_type,
				_DtCvPointer	 data);
extern	void		_DtHelpDARenderElem (
				_DtCvPointer	client_data,
				_DtCvElemType	elem_type,
				_DtCvUnit	x,
				_DtCvUnit	y,
				int		link_type,
				_DtCvFlags	old_flags,
				_DtCvFlags	new_flags,
				_DtCvElemType	trav_type,
				_DtCvPointer	trav_data,
				_DtCvPointer	data);
extern void		_DtHelpDATocMarker (
				XtPointer    client_data,
				Boolean      draw);

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif


#endif /* _XInterface_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
