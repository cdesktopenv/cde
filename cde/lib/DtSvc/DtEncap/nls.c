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
/*
 * File:         nls.c $XConsortium: nls.c /main/3 1995/10/26 15:36:38 rswiston $
 * Language:     C
 *
 * (c) Copyright 1989, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#include <bms/sbport.h> /* NOTE: sbport.h must be the first include. */

#include <bms/bms.h>
#include <bms/MemoryMgr.h>

#ifndef CDE_LOGFILES_TOP
#define CDE_LOGFILES_TOP "/var/dt/tmp"
#endif

/*------------------------------------------------------------------------+*/
XeString
XeSBTempPath(XeString suffix)
/*------------------------------------------------------------------------+*/
{
    XeString dir   = (XeString) CDE_LOGFILES_TOP;
    XeString path;

    if (!suffix || !*suffix)
	return strdup(dir);

    path = (XeString)XeMalloc(strlen(dir) + strlen(suffix) + 2 );

    strcpy(path, dir);
    strcat(path, "/");
    strcat(path, suffix);

    return path;
}
