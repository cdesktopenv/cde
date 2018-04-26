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
/* $XConsortium: RegionI.h /main/3 1996/05/09 03:44:34 drk $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        RegionI.h
 **
 **   Project:     Cde Help System
 **
 **   Description: Defines the Region structure.
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 ****************************************************************************
 ************************************<+>*************************************/
#ifndef	_DtHelpRegionI_h
#define	_DtHelpRegionI_h

/********    Public Enum Declarations    ********/

enum	_dtHelpDARegType
    {
      _DtHelpDAGraphic,
      _DtHelpDASpc
    };

typedef enum _dtHelpDARegType _DtHelpDARegType;

/********    Public Structure Declarations    ********/

typedef	struct _dtHelpDARegion {
	short			inited;
	_DtHelpDARegType	type;
	_DtCvPointer		handle;
} _DtHelpDARegion;

typedef	struct _dtHelpDASpcInfo {
	char			*name;
	_DtHelpFontHints	 spc_fonts;
} _DtHelpDASpcInfo;

#endif /* _DtHelpRegionI_h */
