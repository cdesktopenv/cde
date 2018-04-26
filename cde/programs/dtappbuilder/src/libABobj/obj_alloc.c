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
 *	$XConsortium: obj_alloc.c /main/3 1995/11/06 18:33:33 rswiston $
 *
 *	@(#)obj_alloc.c	3.55 30 Jan 1995	
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
 *  alloc.c - allocation and deallocation of AB_OBJ structures
 */

#include <stdlib.h>
#include <stdio.h>
#include "objP.h"		/* put objP.h first! */
#include "obj_names_listP.h"
#include "obj_notifyP.h"
#include <ab_private/util.h>
#include <ab_private/trav.h>

static int objP_tree_do_destroy(ABObj obj);
static ABObj	objP_create_dup_or_ref(ABObj obj, BOOL createRef);
static ABObj	objP_tree_create_dup_or_ref(ABObj tree, BOOL createRef);
static int	objP_dup_or_ref_child_trees(
			ABObj	dupRoot, 
			ABObj	orgRoot,
			BOOL	createRef
		);

static int	objP_remove_all_refs_to(ABObj obj);


ABObj 
obj_create(AB_OBJECT_TYPE obj_type, ABObj parent)
{
	ABObj obj= (ABObj)util_malloc(sizeof(AB_OBJ));
	if (obj == NULL)
	{
		return NULL;
	}

	/*
	 * Don't send reparent
	 */
	objP_notify_push_mode();
	    objP_notify_clear_mode(OBJEV_MODE_SEND_ALLOW_EVS);
	    objP_notify_clear_mode(OBJEV_MODE_SEND_NOTIFY_EVS);
	    obj_construct(obj, obj_type, parent);
	objP_notify_pop_mode();

	objP_notify_send_create(obj);

	return obj;
}


/*
 * Creates an ABObj that is a duplicates of the given object, but does 
 * NOT duplicate parent, siblings, children, et cetera.
 *
 * Most references to other objects or dynamic data are set to NULL.
 * ISTRINGS are duplicated
 */
static ABObj
objP_create_dup_or_ref(ABObj obj, BOOL createRef)
{
    ABObj	newObj= (ABObj)util_malloc(sizeof(AB_OBJ));
    if (newObj == NULL)
    {
	return NULL;
    }

    *newObj= *obj;
    newObj->parent= NULL;
    newObj->next_sibling= NULL;
    newObj->prev_sibling= NULL;
    newObj->first_child= NULL;

    newObj->name= istr_dup(obj->name);
#ifdef DEBUG
    newObj->debug_name = obj->debug_name;
#endif
    newObj->user_data= istr_dup(obj->user_data);
    newObj->bg_color= istr_dup(obj->bg_color);
    newObj->fg_color= istr_dup(obj->fg_color);
    newObj->label= istr_dup(obj->label);
    newObj->menu_name= istr_dup(obj->menu_name);
    newObj->attachments= NULL;
    newObj->part_of= NULL;
    newObj->ref_to = (createRef? obj : obj->ref_to);

    newObj->ui_handle= NULL;
    newObj->class_name= istr_dup(obj->class_name);
    newObj->ui_args= NULL;
    obj_set_is_selected(newObj, FALSE);
    newObj->browser_data= NULL;
    newObj->projwin_data= NULL;

    if (obj_is_action(newObj))
    {
	newObj->info.action.func_name_suffix= 
		istr_dup(obj->info.action.func_name_suffix);
    }
    else if (obj_is_file_chooser(newObj))
    {
	newObj->info.file_chooser.filter_pattern= 
		istr_dup(obj->info.file_chooser.filter_pattern);
	newObj->info.file_chooser.ok_label= 
		istr_dup(obj->info.file_chooser.ok_label);
	newObj->info.file_chooser.directory= 
		istr_dup(obj->info.file_chooser.directory);
     }
     else if (obj_is_menu(newObj))
     {
	newObj->info.menu.tear_off = obj->info.menu.tear_off;
     }
     else if (obj_is_message(newObj))
     {
	newObj->info.message.msg_string =
                istr_dup(obj->info.message.msg_string);
	newObj->info.message.action1_label =
                istr_dup(obj->info.message.action1_label);
	newObj->info.message.action2_label =
                istr_dup(obj->info.message.action2_label);
    }
    else if (obj_is_module(newObj))
    {
	newObj->info.module.file= istr_dup(obj->info.module.file);
	newObj->info.module.stubs_file= istr_dup(obj->info.module.stubs_file);
	newObj->info.module.ui_file= istr_dup(obj->info.module.ui_file);
    }
    else if (obj_is_item(newObj))
    {
	newObj->info.item.accelerator= istr_dup(newObj->info.item.accelerator);
	newObj->info.item.mnemonic= istr_dup(newObj->info.item.mnemonic);
    }
    else if (obj_is_project(newObj))
    {
	newObj->info.project.file= istr_dup(obj->info.project.file);
	newObj->info.project.stubs_file= istr_dup(obj->info.project.stubs_file);
	newObj->info.project.root_window= obj->info.project.root_window;
    }
    else if (obj_is_text(newObj))
    {
	newObj->info.text.initial_value_string= 
		istr_dup(obj->info.text.initial_value_string);
    }
    else if (obj_is_term_pane(newObj))
    {
	newObj->info.term.process_string= 
		istr_dup(obj->info.term.process_string);
    }
    else if (obj_is_window(newObj) && !obj_is_file_chooser(newObj))
    {
	newObj->info.window.icon= istr_dup(obj->info.window.icon);
	newObj->info.window.icon_label= istr_dup(obj->info.window.icon_label);
    }

    objP_notify_send_create(obj);
    return newObj;
}



static ABObj
objP_tree_create_dup_or_ref(ABObj tree, BOOL createRef)
{
    ABObj	dupTree = NULL;

    dupTree = objP_create_dup_or_ref(tree, createRef);
    if (dupTree != NULL)
    {
        objP_dup_or_ref_child_trees(dupTree, tree, createRef);
    }
    return dupTree;
}


static int
objP_dup_or_ref_child_trees(ABObj dupRoot, ABObj orgRoot, BOOL createRef)
{
    int		return_value = 0;
    int		rc = 0;			/* return code */
    ABObj	orgChild = NULL;
    ABObj	dupChild = NULL;

    for (orgChild = orgRoot->first_child; 
	orgChild != NULL; orgChild = orgChild->next_sibling)
    {
	dupChild = objP_create_dup_or_ref(orgChild, createRef);
	if (dupChild == NULL)
	{
	    return_value = -1;
	    goto epilogue;
	}

	rc = obj_append_child(dupRoot, dupChild);
	do_return_if_err(rc, rc);

    }

    for (orgChild = orgRoot->first_child, dupChild = dupRoot->first_child;
	 (orgChild != NULL) && (dupChild != NULL);
	 orgChild = orgChild->next_sibling, dupChild= dupChild->next_sibling)
    {
	rc = objP_dup_or_ref_child_trees(dupChild, orgChild, createRef);
	do_return_if_err(rc,rc);
    }

epilogue:
    return return_value;
}


/*************************************************************************
**									**
**		Non-reference objects					**
**									**
**************************************************************************/

ABObj
obj_dup(ABObj obj)
{
    return objP_create_dup_or_ref(obj, FALSE);
}


ABObj
obj_tree_dup(ABObj tree)
{
    return objP_tree_create_dup_or_ref(tree, FALSE);
}


int
obj_dup_child_trees(ABObj to_root, ABObj from_root)
{
    return objP_dup_or_ref_child_trees(to_root, from_root, FALSE);
}


/*************************************************************************
**									**
**		Reference objects					**
**									**
**************************************************************************/


ABObj
obj_create_ref(ABObj obj)
{
    return objP_create_dup_or_ref(obj, TRUE);
}


ABObj
obj_tree_create_ref(ABObj tree)
{
    return objP_tree_create_dup_or_ref(tree, TRUE);
}


int
obj_create_ref_subtrees(ABObj to_root, ABObj from_root)
{
    return objP_dup_or_ref_child_trees(to_root, from_root, TRUE);
}



/*
 * Destroys only one object. Children of object being destroyed go to
 * destroyed object's parent
 */
int
obj_destroy_one_impl(ABObj *objPtr)
{
#define obj (*objPtr)
    int		return_value= 0;
    int		iRC= 0;		/* return code */
    BOOL	bDestroyNotifyBatched= FALSE;

#ifdef DEBUG
    if (debugging())
    {
	if (obj_verify(obj) < 0)
	{
	    util_dprintf(0, "WARNING: Corrupt obj detected in obj_destroy (%s:d).\n",
		__FILE__, __LINE__);
	    if (debug_level() >= 3)
	    {
		abort();
	    }
	}

	/*be sure to verify if anybody accesses this now-invalid hunk o' mem*/
	obj->debug_last_verify_time = 0;
    }
#endif /* DEBUG */

    if (   (obj_has_impl_flags(obj, ObjFlagDestroyed))
	|| (obj_has_impl_flags(obj, ObjFlagBeingDestroyed)) )
    {
	return_value = (ERR_MULTIPLE_FREE);
	goto epilogue;
    }

    /*
     * Send notify while parents still in tact
     */
    obj_set_impl_flags(obj, ObjFlagBeingDestroyed);	/* before notifying! */
    iRC= objP_notify_send_destroy(obj);
    bDestroyNotifyBatched= (iRC == OBJ_NOTIFY_BATCHED);

    /*
     * If the notify batched, we don't actually destroy the object
     * until *after* the clients have been notified
     */
    if (bDestroyNotifyBatched)
    {
	/* 
	 * we need to make sure a name search does not find this object.
	 */
	objP_remove_from_names_list(obj);
    }
    else
    {
	return_value = objP_actually_destroy_one(obj);
    }

    /* 
     * It's officially destroyed, at this point. It just doesn't know it, yet.
     * It won't show up in traversal, find_by_name(), et cetera
     */
    if (obj != NULL)
    {
        obj_clear_impl_flags(obj, ObjFlagBeingDestroyed);
        obj_set_impl_flags(obj, ObjFlagDestroyed);
    }

epilogue:
    obj = NULL;			/* it's always set to NULL */
    return return_value;
#undef obj
}


/* Destroys an entire subtree from bottom to top.
 *
 * All notifications are sent in one batch, in order to avoid actually 
 * destroying object during all this business and traversals.
 */
int
obj_destroy_impl(ABObj *treePtr)
{
#define tree (*treePtr)
    int		iReturn= 0;

    objP_notify_push_mode();
    objP_notify_set_mode(OBJEV_MODE_BATCH_NOTIFY_EVS);

	iReturn= objP_tree_do_destroy(tree);

    objP_notify_pop_mode();

    tree = NULL;		/* it's toast! */
    return iReturn;
#undef tree
}


/*
 * Destroys an entire subtree, from bottom to top
 */
static int
objP_tree_do_destroy(ABObj obj)
{
    int		iReturn= 0;
    int 	iRC= 0;	/* return code */
    ABObj	doomedChild = NULL;

    if (obj == NULL)
    {
        goto epilogue;
    }

    /*
     * Queue all the events, because destroying as we go 
     * can get messy.
     */
	while (obj_has_child(obj))
	{
	    doomedChild = obj_get_child(obj, 0);
	    if ((iRC= obj_destroy(doomedChild)) < 0)
	    {
		iReturn= iRC;
		goto epilogue;
	    }
	}

	/* 
	 * we have no children at this point 
	 */
	iRC= obj_destroy_one(obj);
	if ((iRC < 0) && (iRC != ERR_MULTIPLE_FREE))
	{
	    iReturn= iRC;
	}

epilogue:
    return iReturn;
}


/*
 * Effects: Actually performs the destroy of one object.
 *
 * DOESN'T SEND ANY EVENTS OR GET PERMISSION, OR NUTHIN!
 * 
 */
int
objP_actually_destroy_one_impl(ABObj *objPtr)
{
#define obj (*objPtr)
    int		iReturn= 0;
    int		iRC= 0;		/* return code */
    ABObj	parent= NULL;

    /*
     * don't send events!
     */
    objP_notify_push_mode();
        objP_notify_clear_mode(OBJEV_MODE_SEND_ALLOW_EVS);
        objP_notify_clear_mode(OBJEV_MODE_SEND_NOTIFY_EVS);

        if ((parent= obj_get_parent(obj)) != NULL)
        {
	    if ((iRC = obj_move_children(parent, obj)) < 0)
	    {
	        iReturn= iRC;
	        goto epilogue;
	    }
        }

	/*
	 * Remove any references to this object
	 */
	objP_remove_all_refs_to(obj);

        /*
         * actually deallocate the space!
	 *
	 * We set the flags to AlreadyFreedValue in case an attempt is made to
	 * accessed the freed data - we can check.
         */
	obj->impl_flags= ObjFlagAlreadyFreedValue;
        objP_free_mem(obj);	/* actually destroy it! */
	obj = NULL;	/* objP_free_mem() does this, but I'm going to set */
			/* it to NULL, again. */

    objP_notify_pop_mode();

epilogue:
    obj = NULL;		/* forget we ever saw it... */
    return iReturn;
#undef obj
}


static int
objP_remove_all_refs_to(ABObj obj)
{
    return obj_replace(obj, NULL);
}


/*
 * Here, we (finally), actually free the memory associated with an object
 */
int
objP_free_mem_impl(ABObj *objPtr)
{
#define obj (*objPtr)

    obj_destruct(obj);
    util_free(obj);
    obj= NULL;

    return 0;
#undef obj
}
