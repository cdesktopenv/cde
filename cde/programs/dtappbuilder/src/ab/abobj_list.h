
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
