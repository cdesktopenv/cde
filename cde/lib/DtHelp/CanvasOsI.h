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
/* $XConsortium: CanvasOsI.h /main/3 1996/05/09 03:40:52 drk $ */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:   CanvasOsI.h
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
 *******************************************************************
 *************************************<+>*************************************/
#ifndef _DtCanvasOsI_h
#define _DtCanvasOsI_h


#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

/********    Internal Defines Declarations    ********/

/********    Internal Enum Declarations    ********/

/********    Internal Typedef Declarations    ********/

/********    Internal Structures Declarations    ********/

/********    Internal Define Declarations    ********/

/********    Internal Macros Declarations    ********/

/********    Internal Function Declarations    ********/
extern	int		_DtCvRunInterp(
				int		(*filter)(),
				_DtCvPointer	  client_data,
				char		 *interp,
				char		 *data,
				char		**ret_data);

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _DtCanvasOsI_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
