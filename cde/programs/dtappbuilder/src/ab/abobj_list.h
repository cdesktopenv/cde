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
 *	$XConsortium: abobj_list.h /main/3 1995/11/06 17:16:36 rswiston $
 *
 * @(#)abobj_list.h	1.3 22 Jul 1994
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
 * File: abobj_list.h - keeps a scrolling list of object names
 */

#ifndef _ab_abobj_list_h
#define _ab_abobj_list_h

#include <ab_private/obj_notify.h>
#include <ab_private/abobj.h>

int		abobj_list_load(
		    Widget		w,
		    ABObj		root,
		    ABObjTestFunc	show_obj_func
		);

int		abobj_list_update(
		    Widget		w,
		    ABObj		root,
		    ABObjTestFunc	show_obj_func
		);

int		abobj_list_obj_created(
			Widget			list,
			ABObj			obj,
			ABObjTestFunc		show_obj_func
		);

int		abobj_list_obj_renamed(
			Widget			list,
			ABObj			obj,
			STRING			old_name,
			ABObjTestFunc		show_obj_func
		);

int		abobj_list_obj_destroyed(
			Widget			list,
			ABObj			obj,
			ABObjTestFunc		show_obj_func
		);

int		abobj_list_obj_updated(
			Widget			list,
			ObjEvUpdateInfo		update_info,
			ABObjTestFunc		show_obj_func
		);


int 		abobj_list_obj_reparented(
		    Widget              	list,
		    ObjEvReparentInfo   	info,
		    ABObjTestFunc       	list_obj_test
		);

#endif /* _ab_abobj_list_h */
