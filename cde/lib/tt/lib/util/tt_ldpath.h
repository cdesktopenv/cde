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
/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: tt_ldpath.h /main/3 1995/10/23 10:40:55 rswiston $ 			 				 */
/*
 *
 * tt_ldpath.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */
#ifndef _TT_LDPATH_H
#define _TT_LDPATH_H
#include <util/tt_string.h>

/* 
 * Attempts to find the best path to a dynamic library named by libname.
 * If successful then 1 is returned and path will be set to the path to
 * the library. Otherwise 0 is returned. This function will use the
 * appropiate combination of LD_LIBRARY_PATH and the required hardcoded
 * paths to perform the search.
 */
int	tt_ldpath(_Tt_string libname, _Tt_string &path);

#endif				/* _TT_LDPATH_H */
