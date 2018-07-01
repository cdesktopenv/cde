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
/*%%  $XConsortium: isperm.c /main/3 1995/10/23 11:43:03 rswiston $ 			 				 */
/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * isperm.c
 *
 * Description:
 *	Extract permissions from mode functions.
 */

#include "isam_impl.h"


/*
 * _getopenmode(mode)
 *
 * Extract open mode (ISINPUT, ISOUTPUT, ISINOUT) from mode.
 */

enum openmode
_getopenmode(int mode)
{
    switch (mode & ISOPENMODE) {
    case ISINPUT:
	return (OM_INPUT);
    case ISOUTPUT:
	return (OM_OUTPUT);
    case ISINOUT:
	return (OM_INOUT);
    default:
	return (OM_BADMODE);
    }
}

/*
 * _getreadmode(mode)
 *
 * Extract read mode from mode.
 */

enum readmode
_getreadmode(int mode)
{
    switch (mode & ISREADMODE) {
    case ISFIRST:
	return (RM_FIRST);
    case ISLAST:
	return (RM_LAST);
    case ISNEXT:
	return (RM_NEXT);
    case ISPREV:
	return (RM_PREV);
    case ISCURR:
	return (RM_CURR);
    case ISEQUAL:
	return (RM_EQUAL);
    case ISGREAT:
	return (RM_GREAT);
    case ISGTEQ:
	return (RM_GTEQ);
    case ISLESS:
	return (RM_LESS);
    case ISLTEQ:
	return (RM_LTEQ);
    default:
	return (RM_BADMODE);
    }
}
