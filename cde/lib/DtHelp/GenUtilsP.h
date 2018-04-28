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
/* $XConsortium: GenUtilsP.h /main/3 1996/05/09 03:42:42 drk $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        GenUtilsP.h
 **
 **   Project:     CDEnext Info Access
 **
 **   Description: Header file for GenUtils.c
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
#ifndef _DtHelpGenUtilsP_h
#define _DtHelpGenUtilsP_h

/****************************************************************************
 *			Public Defines
 ****************************************************************************/
#ifndef	_DtHelpObsoleteP_h
typedef	void*	CanvasHandle;
typedef	void*	VolumeHandle;
#endif

/****************************************************************************
 *			Semi-Public Externals
 ****************************************************************************/
extern	CanvasHandle	_DtHelpCeCreateDefCanvas(void);
extern  int      _DtHelpCeGetAsciiVolumeAbstract (
			CanvasHandle	canvas,
			VolumeHandle	volume,
                        char            **retAbs);
extern  char    *_DtHelpCeGetNxtToken (
			char             *str,
			char            **retToken);
extern	int	 _DtHelpCeGetVolumeTitle(
			CanvasHandle	canvas,
			VolumeHandle	volume,
			char          **ret_title);
#endif /* _DtHelpGenUtilsP_h */
