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
/*
 * File:         DbUtil.h $XConsortium: DbUtil.h /main/4 1995/10/26 15:04:04 rswiston $
 * Language:     C
 *
 * (c) Copyright 1988, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#ifndef _Dt_DbUtil_h
#define _Dt_DbUtil_h

#include <X11/Intrinsic.h>		/* Boolean */

/* 
 * This structure is used by many of the database functions.  It is used to
 * specify a collection of filenames or directory names.  Each file/directory
 * name is returned in two forms:
 *
 *  dirs[n]   The fully host-qualified pathname for the file or directory,
 *            in "host:/path" format.
 *
 *  paths[n]  The internal format of the file or directory anme, which
 *            can be passed to any of the standard tools which expect a
 *            valid filename; i.e. /nfs/host/path.
 *
 *  Both of the arrays are NULL-terminated.
 */
typedef struct {
   char ** dirs;
   char ** paths;
} DtDirPaths;



/*****************************************************************************
 *
 *
 *
 ****************************************************************************/
extern DtDirPaths * _DtFindMatchingFiles( DtDirPaths * dirs, 
                                           char * suffix, 
                                           Boolean sortFiles );

/*****************************************************************************
 *
 * _DtGetDatabaseDirPaths() returns a NULL-terminated array of directories,
 * which are to be searched for database files.  The paths are obtained by
 * querying the DTDATABASESEARCHPATH environment variable, which specifies
 * a set of comma separated pathnames, int "host:/path" format.  The
 * return structure should be freed up, when no longer needed, by invoking
 * _DtFreeDatabaseDirPaths().
 *
 * The returned directory names are represented in two formats:
 *
 *     1) Fully host qualified; i.e. "host:/path"
 *     2) Internal format; i.e. "/nfs/host/path"
 *
 ****************************************************************************/
extern DtDirPaths * _DtGetDatabaseDirPaths( void );

/*****************************************************************************
 *
 * _DtFreeDatabaseDirPaths() is used to free the memory occupied by the
 * passed-in instance of the DtDirPaths structure.  Typically, this
 * information was originally obtained by a call to DtGetDatabasePaths()
 * or _DtFindMatchingFiles().
 *
 * Parameters:
 *
 *      dirs       The structure which is to be freed up.
 *
 ****************************************************************************/
extern void _DtFreeDatabaseDirPaths( DtDirPaths * dirs );

#endif /* _Dt_DbUtil_h */

/* DON'T ADD ANYTHING AFTER THIS #endif */
