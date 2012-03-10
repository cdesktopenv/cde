
/*
 *	$XConsortium: obj_select.c /main/3 1995/11/06 18:38:49 rswiston $
 *
 *	@(#)obj_select.c	1.10 11 Feb 1994	cde_app_builder/src/libABobj
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
 *  obj_select.c - object selection
 */

#include "objP.h"		/* must put private includes first! */
#include "obj_notifyP.h"
#include <ab_private/obj.h>

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

/* logical exclusive or */
#ifndef util_xor
#define util_xor(a,b)  ( ((a) && (!(b))) || ((!(a)) && (b)) )
#endif

static int obj_set_selected_false(ABObj obj);
static int obj_set_selected_true(ABObj obj);


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

int
obj_set_is_selected(ABObj obj, BOOL new_is_selected)
{
    return (new_is_selected? 
	obj_set_selected_true(obj) : obj_set_selected_false(obj));
}


/*
 * Sends notification only if the object's state changes
 */
static int
obj_set_selected_true(ABObj obj)
{
    BOOL	changed= !obj_is_selected(obj);

    obj_set_impl_flags(obj, ObjFlagIsSelected);

    if (changed)
    {
        objP_notify_send_selected_change(obj);
    }
    return 0;
}


/*
 * Sends notification only if the object's state changes
 */
static int
obj_set_selected_false(ABObj obj)
{
    BOOL	changed= obj_is_selected(obj);

    obj_clear_impl_flags(obj, ObjFlagIsSelected);

    if (changed)
    {
        objP_notify_send_selected_change(obj);
    }
    return 0;
}


BOOL
obj_is_selected(ABObj obj)
{
    return obj_has_impl_flags(obj, ObjFlagIsSelected);
}

