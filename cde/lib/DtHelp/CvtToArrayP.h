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
/* $XConsortium: CvtToArrayP.h /main/3 1996/05/09 03:41:33 drk $ */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:         CvtToArrayP.h
 **
 **  Project:      Cde Help System
 **
 **  Description:  Private header file. Contains defines for converting
 **                a format specific file to a chunked string.
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 *****************************************************************************
 *************************************<+>*************************************/
#ifndef _DtHelpCvtToArrayP_h
#define _DtHelpCvtToArrayP_h


#ifdef __cplusplus
extern "C" {
#endif

/********    Public Defines Declarations    ********/
/*********************************
 * Title Chunk types
 *********************************/
#define DT_HELP_CE_END		0
#define DT_HELP_CE_STRING	(1 << 0)
#define DT_HELP_CE_CHARSET	(1 << 1)
#define DT_HELP_CE_FONT_PTR	(1 << 2)
#define DT_HELP_CE_SPC		(1 << 3)
#define DT_HELP_CE_NEWLINE	(1 << 4)

/********    Public Typedef Declarations    ********/
/********    Public Enum Declarations    ********/
/********    Public Structures Declarations    ********/
/********    Public Prototype Procedures    ********/

/********    Public Macro Declarations    ********/

/********    Public Function Declarations    ********/

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _DtHelpCvtToArrayP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
