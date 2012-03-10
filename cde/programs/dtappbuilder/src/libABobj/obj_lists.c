
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


