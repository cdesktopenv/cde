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
 *	$XConsortium: bil.h /main/3 1995/11/06 18:23:13 rswiston $
 *
 *	@(#)bil.h	1.11 14 Feb 1994	cde_app_builder/src/libABil
 *
 * 	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 */

#ifndef _ABIL_BIL_H_
#define _ABIL_BIL_H_

#include <X11/Intrinsic.h>
#include <ab_private/bil_parse.h>

typedef enum
{
    BIL_SAVE_UNDEF = 0,
    BIL_SAVE_DUMP,		/* just dump the whole thing */
    BIL_SAVE_FILE_PER_MODULE,	/* assume 1 project file, 1 for each module */
    BIL_SAVE_ENCAPSULATED,	/* assume everything goes in one file */
    BIL_SAVE_TYPE_NUM_VALUES
} BIL_SAVE_TYPE;


extern ABObj	bil_load_file(
		    STRING	path,
		    FILE 	*inFile,
		    ABObj	proj,
		    ABObjList	*compObjsOut	/* composite objs loaded */
		);
extern int	bil_load_file_and_resolve_all(
		    STRING	fileName, 
		    FILE	*inFile,
		    ABObj 	*newProjectPtr
		);
extern int	bil_save_tree(
		    ABObj	root,
		    STRING	filename,
		    STRING	old_proj_dir,
		    BIL_SAVE_TYPE type
		);
extern STRING 	bil_get_filename(
		    ABObj	proj,
		    ABObj	module
		);

#endif /* _ABIL_BIL_H_ */
