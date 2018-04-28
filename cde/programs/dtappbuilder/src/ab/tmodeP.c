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
 *	$XConsortium: tmodeP.c /main/3 1995/11/06 17:54:59 rswiston $
 *
 * @(#)tmodeP.c	1.1 15 Jan 1995 cde_app_builder/src/ab
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
** File: tmodeP.c - Common Test Mode functionality
*/

#include "tmodeP.h"
#include <ab_private/trav.h>
#include <ab_private/util.h>
#include <ab_private/ui_util.h>


/*****************************************************************************
**                                                                          **
**       Private Function Declarations                                      **
**                                                                          **
*****************************************************************************/
static ABObjList tmodeP_list_create(
                     void
                 );
static int       tmodeP_list_destroy(
                     ABObjList list
                 );


/*****************************************************************************
**                                                                          **
**       Function Definitions                                               **
**                                                                          **
*****************************************************************************/

/*
 * Test Mode Data Structure
 */

/* Create a Test Mode data structure and initialize it to it's defaults     */
int
tmodeP_obj_create_data(
    ABObj obj
)
{
    if (!obj)
	return ERROR;

    if (tmodeP_obj_has_data(obj))
	tmodeP_obj_destroy_data(obj);

    tmodeP_obj_data(obj) = (TestModeData) util_malloc(sizeof(TestModeDataRec));

    if (!tmodeP_obj_has_data(obj))
	return ERROR;

    return tmodeP_obj_construct_data(obj);
}

/* Destroy a Test Mode data structure                                       */
int
tmodeP_obj_destroy_data(
    ABObj obj
)
{
    if (!obj)
	return ERROR;

    if (tmodeP_obj_has_data(obj))
	util_free(tmodeP_obj_data(obj));

    tmodeP_obj_data(obj) = NULL;
    return OK;
}

/* Initialize a Test Mode data structure to it's defaults                   */
int
tmodeP_obj_construct_data(
    ABObj obj
)
{
    if (!obj || !obj->test_mode_data)
	return ERROR;

    tmodeP_obj_set_width( obj, 0);
    tmodeP_obj_set_height(obj, 0);
    tmodeP_obj_set_x(     obj, 0);
    tmodeP_obj_set_y(     obj, 0);

    return OK;
}


/*
 * Window List
 */
static ABObjList wlist = NULL;		     /* windows in the project      */

/* Create the window list. (and populate it)                                */
int
tmodeP_window_list_create(
    ABObj     project
)
{
    if (!project)
	return ERROR;
    
    if (wlist)
	tmodeP_list_destroy(wlist);

    if ((wlist = tmodeP_list_create()) == NULL)
	return ERROR;
    
    if (tmodeP_window_list_construct(project) != OK)
    {
	tmodeP_list_destroy(wlist);
	return ERROR;
    }
    
    return OK;
}

/* Populate the window list                                                 */
int
tmodeP_window_list_construct(
    ABObj     project
)
{
    ABObj		obj;
    AB_TRAVERSAL	trav;
    int                 status;
    
    if (!project || !wlist)
	return ERROR;

    /* populate the list */
    for (trav_open(&trav, project, AB_TRAV_WINDOWS);
	 (obj = trav_next(&trav)) != NULL; )
    {
	status = objlist_add_obj(wlist, obj, NULL);
	if (status == 0)
	{
	    tmodeP_obj_construct_flags(obj);
	    if (tmodeP_obj_create_data(obj) != OK)
		util_dprintf(1,
		    "tmodeP_window_list_construct: Unable to create data.\n");	    
	}
	else if (status == ERR_DUPLICATE_KEY)
	    util_dprintf(1,
		"tmodeP_window_list_construct: Duplicate window in obj structure.\n");
	else
	    util_dprintf(1,
		"tmodeP_window_list_construct: Unable to add window to list.\n");
    }
    trav_close(&trav);

    return OK;
}

/* UnPopulate the window list                                               */
int
tmodeP_window_list_destruct(
    void
)
{
    int     i;
    ABObj obj = NULL;
    
    if (!wlist)
	return ERROR;
    
    /* trav the list of windows, destroying test data */
    for (i=0; i < objlist_get_num_objs(wlist); i++)
    {
	obj = objlist_get_obj(wlist, i, NULL);
	
	if (!obj)
	{
	    util_dprintf(1,
		"tmodeP_window_list_destruct: %d window's obj is missing.\n", i);
	    continue;
	}

	tmodeP_obj_construct_flags(obj);
	
	if (tmodeP_obj_has_data(obj))
	{
	    if (tmodeP_obj_destroy_data(obj) == ERROR)
	    {
		util_dprintf(1,
		    "tmodeP_window_list_destruct: Can't free test mode data for %d window.\n", i);
		continue;
	    }
	}
    }

    return OK;
}

/* Destroy the window list                                                  */
int
tmodeP_window_list_destroy(
    void
)
{
    if (!wlist)
	return OK;
    
    /* clean the list */
    if (tmodeP_window_list_destruct() != OK)
	util_dprintf(1, "tmodeP_window_list_destroy: Can't destroy some test mode data.\n");
    
    /* destroy the list */
    tmodeP_list_destroy(wlist);
    wlist = NULL;
    
    return OK;
}

/*
** iterate the supplied function over the window list
*/
void
tmodeP_window_list_iterate(
    ABObjListIterFn fn
)
{
    int    i;
    ABObj  obj = NULL;
    
    if (!wlist)
	return;
    
    objlist_iterate(wlist, fn);
}

/*
** foreach of the windows in the window list
**      find the window's shell
**      XtAddEventHandler() to the shell
**         client_data = obj.
*/
int
tmodeP_window_list_add_handler(
    EventMask          event_mask,
    Boolean            nonmaskable,
    XtEventHandler     event_handler
)
{
    int    i;
    Widget w;
    ABObj  obj = NULL;
    
    if (!wlist)
	return ERROR;
    
    /* trav the list of windows */
    for (i=0; i < objlist_get_num_objs(wlist); i++)
    {
	obj = objlist_get_obj(wlist, i, NULL);
	
	if (!obj)
	{
	    util_dprintf(1,
		"tmodeP_window_list_add_handler: %d window is missing obj.\n", i);
	    continue;
	}

	if (!obj->ui_handle)
	{
	    util_dprintf(2,
		"tmodeP_window_list_add_handler: no ui_handle %d window.\n", i);
	    continue;
	}

	w = ui_get_ancestor_shell(obj->ui_handle);
	
	XtAddEventHandler(
	    w,			/* Widget */
	    event_mask,		/* EventMask */
	    nonmaskable,	/* non-maskable events - Boolean */
	    event_handler,	/* XtEventHandler */
	    (XtPointer) obj	/* client_data */
	);
    }
    return OK;
}

/*
** foreach of the windows in the window list
**      find the window's shell
**      XtRemoveEventHandler() on the shell
**         client_data = obj.
*/
int
tmodeP_window_list_remove_handler(
    EventMask          event_mask,
    Boolean            nonmaskable,
    XtEventHandler     event_handler
)
{
    int    i;
    Widget w;
    ABObj  obj = NULL;
    
    if (!wlist)			/* can't remove the handlers from a */
	return OK;		/* non-existant list, but it's not an error */
    
    /* trav the list of windows */
    for (i=0; i < objlist_get_num_objs(wlist); i++)
    {
	obj = objlist_get_obj(wlist, i, NULL);
	
	if (!obj)
	{
	    util_dprintf(1,
		"tmodeP_window_list_remove_handler: %d window is missing obj.\n", i);
	    continue;
	}

	if (!obj->ui_handle)
	{
	    util_dprintf(2,
		"tmodeP_window_list_remove_handler: no ui_handle %d window.\n", i);
	    continue;
	}

	w = ui_get_ancestor_shell(obj->ui_handle);
	
	XtRemoveEventHandler(
	    w,			/* Widget */
	    event_mask,		/* EventMask */
	    nonmaskable,	/* non-maskable events - Boolean */
	    event_handler,	/* XtEventHandler */
	    (XtPointer) obj	/* client_data */
	);
    }
    return OK;
}

    
/*****************************************************************************
**                                                                          **
**       Private Function Definitions                                       **
**                                                                          **
*****************************************************************************/

/*
 * List Manipulation
 */

/* Create a List                                                            */
static ABObjList
tmodeP_list_create(
    void
)
{
    ABObjList list = NULL;
    
    if ((list = objlist_create()) == NULL)
	return NULL;

    objlist_set_sort_order(list, OBJLIST_SORT_BEST);
    
    return list;
}

/* Destroy a List                                                           */
static int
tmodeP_list_destroy(
    ABObjList list
)
{
    if (list)
	objlist_destroy(list);
    return OK;
}
