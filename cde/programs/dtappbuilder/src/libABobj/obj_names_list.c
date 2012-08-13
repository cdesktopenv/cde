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
 *	$XConsortium: obj_names_list.c /main/3 1995/11/06 18:37:04 rswiston $
 *
 *	@(#)obj_names_list.c	1.1 26 May 1994	cde_app_builder/src/libABobj
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
 *  template.c - template c file.
 */

#include "objP.h"
#include <ab_private/trav.h>
#include "obj_names_listP.h"

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

/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/

ABObj		
objP_get_names_scope_obj(ABObj obj)
{
    ABObj	namesObj = obj_get_module(obj);
    
    if (obj_is_project(obj))
    {
	return NULL;
    }
    else if ((namesObj == NULL) || obj_is_module(obj))
    {
	/* module names go in project */
	namesObj = obj_get_project(obj);
    }
    return namesObj;
}


StringList
objP_get_names_scope(ABObj obj)
{
    ABObj	nameScopeObj = objP_get_names_scope_obj(obj);
    if (nameScopeObj != NULL)
    {
	return objP_get_names_list(nameScopeObj);
    }
    return NULL;
}


ABObj
objP_get_names_scope_obj_for_children(ABObj parent)
{
    ABObj	scopeObj = obj_get_module(parent);
    if (scopeObj == NULL)
    {
	scopeObj = obj_get_project(parent);
    }
    return scopeObj;
}


StringList
objP_get_names_scope_for_children(ABObj parent)
{
    ABObj	scopeObj = objP_get_names_scope_obj_for_children(parent);
    if (scopeObj != NULL)
    {
	return objP_get_names_list(scopeObj);
    }
    return NULL;
}


StringList	
objP_get_names_list(ABObj obj)
{
    switch (obj->type)
    {
	case AB_TYPE_MODULE:
	    return obj->info.module.obj_names_list;
	case AB_TYPE_PROJECT:
	    return obj->info.project.obj_names_list;
    }

    return NULL;
}


int
objP_add_to_names_list(ABObj obj)
{
    StringList	names = NULL;
    
    names = objP_get_names_scope(obj);
    if (names != NULL)
    {
	strlist_add_istr(names, obj->name, obj);
    }
    return 0;
}


int
objP_remove_from_names_list(ABObj obj)
{
    StringList	names = objP_get_names_scope(obj);
    if (names != NULL)
    {
	strlist_remove_istr(names, obj->name);
    }
    return 0;
}


int
objP_tree_add_to_names_list(ABObj tree)
{
    ABObj		scopeObj = objP_get_names_scope_obj(tree);
    StringList		names = NULL;
    AB_TRAVERSAL	trav;
    ABObj		obj = NULL;

    if (scopeObj != NULL)
    {
	names = objP_get_names_list(scopeObj);
    }
    if (names == NULL)
    {
	return 0;
    }

    for (trav_open(&trav, tree, AB_TRAV_ALL);
	(obj = trav_next(&trav)) != NULL; )
    {
	if (objP_get_names_scope_obj(obj) == scopeObj)
	{
	    strlist_add_istr(names, obj->name, obj);
	}
    }
    trav_close(&trav);

    return 0;
}


int
objP_tree_remove_from_names_list(ABObj tree)
{
    ABObj		scopeObj = objP_get_names_scope_obj(tree);
    StringList		names = NULL;
    AB_TRAVERSAL	trav;
    ABObj		obj = NULL;

    if (scopeObj != NULL)
    {
	names = objP_get_names_list(scopeObj);
    }
    if (names == NULL)
    {
	return 0;
    }

    for (trav_open(&trav, tree, AB_TRAV_ALL);
	(obj = trav_next(&trav)) != NULL; )
    {
	if (objP_get_names_scope_obj(obj) == scopeObj)
	{
	    strlist_remove_istr(names, obj->name);
	}
    }
    trav_close(&trav);

    return 0;
}


