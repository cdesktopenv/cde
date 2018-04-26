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
 *	$XConsortium: obj_lists.c /main/3 1995/11/06 18:36:17 rswiston $
 *
 *	@(#)obj_lists.c	1.3 09 Sep 1994	cde_app_builder/src/libABobj
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
 *  obj_lists.c
 *
 * Handles internal lists of objects
 */

#include "objP.h"

/*************************************************************************
**                                                                      **
**       Constants (#define and const)					**
**                                                                      **
**************************************************************************/

/*************************************************************************
**                                                                      **
**       Private Functions (C declarations and macros)			**
**                                                                      **
**************************************************************************/

static ABObj find_obj_in_list(ABObj *list, ABObj obj);

/*************************************************************************
**                                                                      **
**       Data     	                                                **
**                                                                      **
**************************************************************************/

ABObjList	objP_all_objs_list = NULL;

/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/

int
objP_lists_add(ABObj obj)
{
    int		return_value = 0;

    if (objP_all_objs_list == NULL)
    {
	objP_all_objs_list = objlist_create();
	objlist_set_is_unique(objP_all_objs_list, TRUE);
	if (objP_all_objs_list == NULL)
	{
	    return ERR_NO_MEMORY;
	}
    }

    return_value = objlist_add_obj(objP_all_objs_list, obj, NULL);

    return return_value;
}


int
objP_lists_remove(ABObj obj)
{
    int		return_value = 0;

    if (objP_all_objs_list == NULL)
    {
	return 0;
    }
    return_value = objlist_remove_obj(objP_all_objs_list, obj);

    return return_value;
}


