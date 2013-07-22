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
/* $XConsortium: SetListI.h /main/6 1995/12/08 13:00:59 cde-hal $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        SetList.h
 **
 **   Project:     TextGraphic Display routines
 **
 **   Description: Header file for SetListTG.h
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
#ifndef _DtHelpSetListI_h
#define _DtHelpSetListI_h

#ifdef __cplusplus
extern "C" {
#endif

extern  XtPointer	_DtHelpDisplayAreaData(
				XtPointer       client_data);
extern	void		_DtHelpDisplayAreaDimensionsReturn (
				XtPointer	 client_data,
				short		*ret_rows,
				short		*ret_columns );
extern	void		_DtHelpDisplayAreaSetList (
				XtPointer	client_data,
				XtPointer	topicHandle,
				Boolean		append_flag,
				int		scroll_percent);
extern  Widget		_DtHelpDisplayAreaWidget(
				XtPointer       client_data);
extern	int		_DtHelpGetScrollbarValue (
				XtPointer       client_data);
extern	Boolean		_DtHelpSetScrollBars (
				XtPointer	client_data,
				Dimension	new_width,
				Dimension	new_height );
extern int		_DtHelpUpdatePath (
				DtHelpDispAreaStruct        *pDAS,
				_DtHelpVolumeHdl             volume_handle,
				char                        *loc_id);

#ifdef __cplusplus
}
#endif
#endif /* _DtHelpSetListI_h */
