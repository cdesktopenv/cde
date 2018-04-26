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
 *	$XConsortium: projP.h /main/3 1995/11/06 17:46:14 rswiston $
 *
 * @(#)projP.h	1.5 15 Feb 1994	cde_app_builder/src/
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

/*
 * projP.h
 * Private declarations for project window
 */

#ifndef _PROJP_H
#define _PROJP_H

#include <ab_private/proj.h>
#include "proj_ui.h"

extern void     projP_show_save_as_bil_chooser(
                    Widget      widget,
                    ABObj       obj
                );

extern int      projP_write_out_bil_file(
                    ABObj       obj,
                    char        *exp_path,
                    STRING      old_proj_dir,
                    BOOL        Exporting
                );

extern int      projP_save_export_bil(
                    ABObj       module_obj,
                    STRING      fullpath,
                    BOOL        Exporting
                );

extern void	projP_store_viewer(
		    Widget      widget,
		    XtPointer   client_data,
		    XtPointer   call_data
		);

extern void	projP_update_menu_items(
		    int         old_count,
		    int         new_count
		);

#endif /* _PROJP_H */
