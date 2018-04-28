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
 *	$XConsortium: abobj_list.c /main/3 1995/11/06 17:16:26 rswiston $
 *
 * @(#)abobj_list.c	1.7 25 Jul 1994 cde_app_builder/src/ab
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
 * File: abobj_list.c - handles a scrolling list of ABObjs
 */

#include <stdio.h>
#include <Xm/List.h>
#include <ab_private/abobj_list.h>
#include <ab_private/ui_util.h>
#include <ab_private/trav.h>


/*************************************************************************
**                                                                      **
**       Private Function Declarations                                  **
**                                                                      **
**************************************************************************/

/*************************************************************************
**                                                                      **
**       Data     	                                                **
**                                                                      **
**************************************************************************/

/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/

/*
** Traverse the project and load all objects into the scrolling list.  This
** is a generalized function that traverses all salient objects as candidates
** to be displayed.  It takes an additional test function as an argument, and
** applies that function to each object in turn.  The object will appear in
** the list only if the test function returns non-zero.
**
** ("He's making a list, and checking it twice..."  (well, o.k., once))
*/
int
abobj_list_load(
	Widget		list,
	ABObj		root,
	ABObjTestFunc	list_obj_func
)
{
    int		return_value = 0;

    if ((root == NULL) || (list == NULL)) return(0);

    /* Clear out the list so we can start afresh */
    XmListDeleteAllItems(list);

    return_value = abobj_list_update(list, root, list_obj_func);
    return return_value;
}


int
abobj_list_update(
	Widget		list,
	ABObj		root,
	ABObjTestFunc	list_obj_func
)
{
    ABObj		obj = NULL;
    AB_TRAVERSAL    trav;
    STRING	    modname;
    int		    item_count = 0;
    int			item_pos = 0;

    if ((root == NULL) || (list == NULL)) return(0);

    for( (trav_open(&trav,root,AB_TRAV_SALIENT));
	 (obj = trav_next(&trav)) != NULL; ) 
	{
	    if (obj_get_name(obj) != NULL)
	    {
		modname = abobj_get_moduled_name(obj);
	        if((*list_obj_func)(obj))
		{
		    if (ui_list_find_item(list,modname,&item_pos)
			    == ERR_NOT_FOUND)
		    {
		        ui_list_add_item(list,modname,0);
		    }
		    item_count++;
		}
		else
		{
		    ui_list_delete_item(list,modname);
		}
		XtFree(modname);
	    }
        }
    trav_close(&trav);
    return(item_count);
}


/*
 * An object was created - add it to the list (maybe)
 *
 * Returns a negative ERR_ code if an error occurs
 * Returns 0 if the object was not added
 * Returns 1 if the object was added to the list
 */
int
abobj_list_obj_created(
    Widget		list,
    ABObj		obj,
    ABObjTestFunc	list_obj_test
)
{
    int		return_value = 0;
    int		rc = 0;			/* return code */

    if ((obj == NULL) || (list == NULL)) return(0);

    if (!list_obj_test(obj))
    {
	return_value = 0;
	goto epilogue;
    }
    if (obj_get_name(obj) != NULL)
    {
	STRING	modname = abobj_get_moduled_name(obj);
        rc = ui_list_add_item(list, modname, 0);
	if (rc >= 0)
	{
	    return_value = 1;
	}
	else
	{
	    return_value = rc;
	}

	XtFree(modname); modname = NULL;
    }

epilogue:
    return return_value;
}


/*
** Some object has been deleted and we should act accordingly.
** Note -- this is a synthetic callback generated from within AppBuilder,
** not a traditional Xt-style callback (hence the different argument style)
*/
int
abobj_list_obj_destroyed(
    Widget		list,
    ABObj		obj,
    ABObjTestFunc	list_obj_test
)
{
    STRING	modname = NULL;

    if ((obj == NULL) || (list == NULL)) return(0);

    /* 
    ** Check to make sure this is an object we'd have in the list to 
    ** begin with.
    */
    if (list_obj_test(obj)) {
	modname = abobj_get_moduled_name(obj);
	ui_list_delete_item(list, modname);
	XtFree(modname);
    }
    return(0);
}

/*
** Some object has been renamed and we should act accordingly.
** Note -- this is a synthetic callback generated from within AppBuilder,
** not a traditional Xt-style callback (hence the different argument style)
*/
int
abobj_list_obj_renamed(
    Widget		list,
    ABObj		obj,
    STRING		old_name,
    ABObjTestFunc	list_obj_test
)
{
    int		rc = 0;		/* return code */
    STRING	new_modname = NULL;
    STRING	old_modname = NULL;
    ABObj	module = obj_get_module(obj);

    if ((obj == NULL) || (list == NULL)) return(0);

    if (obj_is_module(obj))
    {
	ui_list_replace_item_prefix(
	    list, old_name, obj_get_name(obj));
    }
    else if ((module != NULL) && list_obj_test(obj)) {

	new_modname = abobj_get_moduled_name(obj);
	old_modname = abobj_alloc_moduled_name(
			obj_get_name(module), old_name);
	rc = ui_list_replace_item(list, old_modname, new_modname);
	if (rc == ERR_NOT_FOUND)
	{
	    ui_list_add_item(list, new_modname, 0);
	}
	XtFree(new_modname);
	XtFree(old_modname);
    }
    return(0);
}


/*
 * Completely rebuild all that is known about this object or tree
 */
int
abobj_list_obj_updated(
    Widget		list,
    ObjEvUpdateInfo	info,
    ABObjTestFunc	list_obj_test
)
{
    return abobj_list_update(list, info->obj, list_obj_test);
}

int
abobj_list_obj_reparented(
    Widget              list,
    ObjEvReparentInfo   info,
    ABObjTestFunc       list_obj_test
)
{
    static char full_name[BUFSIZ];
    STRING      moduled_name = NULL,
		module = NULL,
		name = NULL;
    ABObj	obj = NULL;
    int		ret = 0;

    obj = info->obj; 
    if ((obj == NULL) || (list == NULL)) return(0);

    if (obj_get_parent(obj) == NULL)
    {
	module = obj_get_name(obj_get_module(info->old_parent));
	snprintf(full_name, sizeof(full_name), "%s :: %s", module, obj_get_name(obj));
	name = full_name;
    }
    else
	name = abobj_get_moduled_name(obj);

    if (list_obj_test(obj))
	ret = ui_list_add_item(list, name, 0);
    else
	ret = ui_list_delete_item(list, name);

    return (ret);
}
