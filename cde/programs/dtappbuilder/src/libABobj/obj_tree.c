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
 * $XConsortium: obj_tree.c /main/3 1995/11/06 18:39:03 rswiston $
 * 
 * @(#)obj_tree.c	3.38 17 Feb 1994	cde_app_builder/src/libABobj
 * 
 * RESTRICTED CONFIDENTIAL INFORMATION:
 * 
 * The information in this document is subject to special restrictions in a
 * confidential disclosure agreement between HP, IBM, Sun, USL, SCO and
 * Univel.  Do not distribute this document outside HP, IBM, Sun, USL, SCO,
 * or Univel without Sun's specific written approval.  This document and all
 * copies and derivative works thereof must be returned or destroyed at Sun's
 * request.
 * 
 * Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 * 
 */


/*
 * tree.c - handles object trees
 */

#include <stdio.h>
#include "objP.h"		/* gotsta include this first! */
#include <ab_private/trav.h>
#include <ab_private/obj_list.h>
#include "obj_notifyP.h"
#include "obj_names_listP.h"

/* reads get verified at a higher debug leve */
static int verify_obj_read(ABObj obj);		/* verify data read */
static int verify_obj_write(ABObj obj);		/* verify data write */

#ifdef DEBUG
static int verify_obj_read_impl(ABObj obj, STRING file, int line);
static int verify_obj_write_impl(ABObj obj, STRING file, int line);
#define verify_obj_read(obj) (verify_obj_read_impl(obj, __FILE__, __LINE__))
#define verify_obj_write(obj) (verify_obj_write_impl(obj, __FILE__, __LINE__))
#else
#define verify_obj_read(obj) /* ignore */
#define verify_obj_write(obj) /* ignore */
#endif


static int	add_to_ref_list(
			ABObjList	*refListInOutPtr,
			ABObj 		refObj, 
			AB_OBJ_REF_TYPE refType
		);

static int	comp_change_flag(
		    ABObj root,
		    unsigned flags,
		    BOOL clear
		);

static int	update_names_lists(
			ABObj		obj, 
			StringList	oldNamesList, 
			StringList	newNamesList
		);


/*
 * Test to see if a reparent will be allowed
 */
int
obj_test_reparent(ABObj obj, ABObj new_parent)
{
    int                 iReturn = 0;
    verify_obj_read(obj);

    objP_notify_push_mode();

    objP_notify_clear_mode(OBJEV_MODE_DISALLOW_ALL_EVS);
    iReturn = objP_notify_send_allow_reparent(obj, new_parent);

    objP_notify_pop_mode();
    return iReturn;
}


int
obj_unparent(ABObj obj)
{
    int                 iRC = 0;
    int                 iReturn = 0;
    ABObj               oldParent = obj->parent;
    ABObj               parent = obj->parent;
    ABObj               prev_sibling = obj->prev_sibling;
    ABObj               next_sibling = obj->next_sibling;
    verify_obj_write(obj);

    /* get permission... */
    if ((iRC = objP_notify_send_allow_reparent(obj, NULL)) < 0)
    {
	iReturn = iRC;
	goto epilogue;
    }

    if (parent == NULL)
    {
	goto epilogue;
    }

    verify_obj_write(parent);
    if (parent->first_child == obj)
    {
	parent->first_child = next_sibling;
    }
    if (next_sibling != NULL)
    {
	next_sibling->prev_sibling = prev_sibling;
    }
    if (prev_sibling != NULL)
    {
	prev_sibling->next_sibling = next_sibling;
    }

    objP_set_parent(obj, NULL);
    obj->prev_sibling = NULL;
    obj->next_sibling = NULL;

    iReturn = iRC = objP_notify_send_reparent(obj, oldParent);

epilogue:
    return iReturn;
}


/*
 * Current parent may be null (parent will be set) new_parent may be null
 * (same as unparent)
 */
int
obj_reparent(ABObj obj, ABObj new_parent)
{
    int                 iReturn = 0;

    if (new_parent == NULL)
    {
	iReturn = obj_unparent(obj);
    }
    else
    {
	iReturn = obj_append_child(new_parent, obj);
    }
    return iReturn;
}


/*
 * Returns >= 0 on success, < 0 otherwise
 */
int
obj_insert_child(ABObj obj, ABObj new_child, int new_child_num)
{
    int                 iReturn = 0;
    int                 iRC;	/* return code */
    int                 child_num = 0;
    ABObj               oldChildParent = new_child->parent;
    ABObj               old_child = NULL;

    verify_obj_write(obj);
    verify_obj_write(new_child);

    /*
     * See if we can ignore this operation.
     */
    if (   (new_child->parent == obj)
	&& (obj_get_child_num(new_child) == new_child_num) )
    {
	/* this child is already in the requested position */
	return 0;
    }

    if ((iRC = objP_notify_send_allow_reparent(new_child, obj)) < 0)
    {
	iReturn = iRC;
	goto epilogue;
    }

    /*
     * unparent the new child (don't send events)
     */
    objP_notify_push_mode();
    objP_notify_clear_mode(OBJEV_MODE_SEND_ALLOW_EVS);
    objP_notify_clear_mode(OBJEV_MODE_SEND_NOTIFY_EVS);
    obj_unparent(new_child);
    objP_notify_pop_mode();

    if (new_child_num == 0)
    {
	new_child->next_sibling = obj->first_child;
	obj->first_child = new_child;
	new_child->prev_sibling = NULL;
	if (new_child->next_sibling != NULL)
	{
	    new_child->next_sibling->prev_sibling = new_child;
	}
    }
    else
    {
	BOOL                child_inserted = FALSE;
	for (child_num = 0, old_child = obj->first_child;
	     old_child != NULL;
	     ++child_num, old_child = old_child->next_sibling)
	{
	    if (child_num == (new_child_num - 1))
	    {
		new_child->next_sibling = old_child->next_sibling;
		old_child->next_sibling = new_child;
		new_child->prev_sibling = old_child;
		if (new_child->next_sibling != NULL)
		{
		    new_child->next_sibling->prev_sibling =
			new_child;
		}

		child_inserted = TRUE;
		break;
	    }
	}
	if (!child_inserted)
	{
	    iReturn = -1;
	}
    }

    if (iReturn >= 0)
    {
	objP_set_parent(new_child, obj);
    }

    objP_notify_send_reparent(new_child, oldChildParent);

epilogue:
    return iReturn;
}


int
obj_append_child(ABObj obj, ABObj new_child_obj)
{
    int                 iRC = 0;/* return code */
    int                 iReturn = 0;
    ABObj               oldParent = new_child_obj->parent;
    ABObj		child_obj = NULL;
    verify_obj_write(obj);
    verify_obj_write(new_child_obj);

    /*
     * See if this is already our last child.  If so, ignore.
     */
    for (child_obj = obj->first_child; 
	(child_obj != NULL) && (child_obj->next_sibling != NULL);
	child_obj= child_obj->next_sibling)
    { /* null */ }
    if ((child_obj != NULL) && (child_obj == new_child_obj))
    {
	return 0;
    }

    /*
     * Get permission
     */
    if ((iRC = objP_notify_send_allow_reparent(new_child_obj, obj)) < 0)
    {
	iReturn = iRC;
	goto epilogue;
    }

    /*
     * Don't send notification of reparent to NULL
     */
    objP_notify_push_mode();
    objP_notify_clear_mode(OBJEV_MODE_SEND_ALLOW_EVS);
    objP_notify_clear_mode(OBJEV_MODE_SEND_NOTIFY_EVS);
    obj_unparent(new_child_obj);
    objP_notify_pop_mode();

    /* Must grab first child again in case the unparent 
     * changed obj children. 
     */
    child_obj = obj->first_child;
    if (child_obj == NULL)
    {
	obj->first_child = new_child_obj;
    }
    else
    {
	while (child_obj->next_sibling != NULL)
	{
	    child_obj = child_obj->next_sibling;
	}
	child_obj->next_sibling = new_child_obj;
	new_child_obj->prev_sibling = child_obj;
    }

    objP_set_parent(new_child_obj, obj);
    new_child_obj->next_sibling = NULL;

    objP_notify_send_reparent(new_child_obj, oldParent);

epilogue:
    return iReturn;
}


ABObj
obj_get_parent(ABObj obj)
{
    ABObj	parent = NULL;
    verify_obj_read(obj);

    parent = obj->parent;

    verify_obj_read(parent);
    return parent;
}


ABObj
obj_get_child(ABObj obj, int child_num)
{
    ABObj               child_obj = NULL;
    AB_TRAVERSAL        trav;
    verify_obj_read(obj);

    trav_open(&trav, obj, AB_TRAV_CHILDREN);
    trav_goto(&trav, child_num);
    child_obj = trav_obj(&trav);
    trav_close(&trav);

    return child_obj;
}

int
obj_get_num_children(ABObj obj)
{
    verify_obj_read(obj);
    return trav_count(obj, AB_TRAV_CHILDREN);
}

/*
 * returns the number of immediate children that are salient (directly
 * manipulatable by the user).
 */
int
obj_get_num_salient_children(ABObj obj)
{
    verify_obj_read(obj);
    return trav_count(obj, AB_TRAV_SALIENT_CHILDREN);
}

int
obj_get_num_salient_ui_children(ABObj obj)
{
    verify_obj_read(obj);
    return trav_count(obj, AB_TRAV_SALIENT_UI_CHILDREN);
}

/*
 * Returns immediate salient (user-manipulatable) child #child_num
 */
ABObj
obj_get_salient_child(ABObj obj, int child_num)
{
    AB_TRAVERSAL        trav;
    ABObj               child = NULL;
    verify_obj_read(obj);

    if (trav_open(&trav, obj, AB_TRAV_SALIENT_CHILDREN) < 0)
    {
	return NULL;
    }
    trav_goto(&trav, child_num);
    child = trav_obj(&trav);
    trav_close(&trav);
    return child;
}

/*
 * Returns immediate salient UI child #child_num
 */
ABObj
obj_get_salient_ui_child(ABObj obj, int child_num)
{
    AB_TRAVERSAL        trav;
    ABObj               child = NULL;
    verify_obj_read(obj);

    if (trav_open(&trav, obj, AB_TRAV_SALIENT_UI_CHILDREN) < 0)
    {
	return NULL;
    }
    trav_goto(&trav, child_num);
    child = trav_obj(&trav);
    trav_close(&trav);
    return child;
}

/*
 * Same as obj_get_child(), but returns the Nth child that satisfies the
 * condition function fn() i.e. make it return a non zero result.
 * 
 * fn() has the format:
 * 
 * int fn(AB_OBJ	*obj);
 * 
 */
ABObj
obj_get_child_cond(ABObj obj, int child_num, BOOL(*fn) (ABObj))
{
    int                 i;
    ABObj               child_obj;
    verify_obj_read(obj);

    for (child_obj = obj->first_child, i = 0; child_obj != NULL;
	 child_obj = child_obj->next_sibling)
    {
	if ((*fn) (child_obj))
	{
	    if (i == child_num)
		return (child_obj);

	    ++i;
	}

    }
    return NULL;
}

/*
 * Same as obj_get_num_children(), but returns # of children that satisfies
 * the condition function fn() i.e. make it return a non zero result.
 * 
 * fn() has the format:
 * 
 * int fn(AB_OBJ	*obj);
 * 
 */
int
obj_get_num_children_cond(ABObj obj, BOOL(*fn) (ABObj))
{
    int                 child_count = 0;
    ABObj               child_obj;
    verify_obj_read(obj);

    for (child_obj = obj->first_child; child_obj != NULL;
	 child_obj = child_obj->next_sibling)
    {
	if ((*fn) (child_obj))
	    ++child_count;
    }

    return child_count;
}

/*
 * Replaces all references to obj with references to its replacement. The
 * original obj is left with no references to it
 *
 * obj must be a valid object
 * replacement may be NULL.
 * 
 * REMIND: This should send some sort of message (destroy and create?)
 */
int
obj_replace(ABObj obj, ABObj replacement)
{
    int			return_value = 0;
    ABObj		objParent = obj->parent;
    int			objChildNum = obj_get_child_num(obj);
    int			numObjRefs = 0;
    int			i = 0;
    void		*voidRefType = NULL;
    ABObjList		refList = NULL;
    AB_OBJ_REF_TYPE	refType = AB_REF_UNDEF;
    ABObj		refObj = NULL;

#ifdef DEBUG
    verify_obj_write(obj);
    if (replacement != NULL)
    {
        verify_obj_write(replacement);
    }
#endif /* DEBUG */

    /*
     * Insert the new object into the tree, or remove it
     */
    if (objParent != NULL)
    {
        obj_unparent(obj);
	if (replacement != NULL)
	{
            obj_insert_child(objParent, replacement, objChildNum);
	}
    }
    if (replacement != NULL)
    {
	obj_move_children(replacement, obj);
    }

    refList = obj_get_refs_to(obj);
    numObjRefs = objlist_get_num_objs(refList);

    for (i = 0; i < numObjRefs; ++i)
    {
	/*
	 * All parent and sibling references were handled above
	 */
	refObj = objlist_get_obj(refList, i, &voidRefType);
	refType = (AB_OBJ_REF_TYPE)voidRefType;

	switch (refType)
	{
	    case AB_REF_ATTACH_EAST:
		if (replacement == NULL)
		{
	            refObj->attachments->east.type = AB_ATTACH_UNDEF;
	            refObj->attachments->east.offset = 0;
		}
	        refObj->attachments->east.value = (void*)replacement;
	    break;
	    case AB_REF_ATTACH_NORTH:
		if (replacement == NULL)
		{
	            refObj->attachments->north.type = AB_ATTACH_UNDEF;
	            refObj->attachments->north.offset = 0;
		}
	        refObj->attachments->north.value = (void*)replacement;
	    break;
	    case AB_REF_ATTACH_SOUTH:
		if (replacement == NULL)
		{
	            refObj->attachments->north.type = AB_ATTACH_UNDEF;
	            refObj->attachments->north.offset = 0;
		}
	        refObj->attachments->north.value = (void*)replacement;
	    break;
	    case AB_REF_ATTACH_WEST:
		if (replacement == NULL)
		{
	            refObj->attachments->north.type = AB_ATTACH_UNDEF;
	            refObj->attachments->north.offset = 0;
		}
	        refObj->attachments->north.value = (void*)replacement;
	    break;
	    case AB_REF_CONNECT_FROM:
		obj_set_from(refObj, replacement);
	    break;
	    case AB_REF_CONNECT_TO:
		obj_set_to(refObj, replacement);
	    break;
	    case AB_REF_DEFAULT_BUTTON:
		obj_set_default_act_button(refObj, replacement);
	    break;
	    case AB_REF_ROOT_WINDOW:
		obj_set_root_window(refObj, replacement);
	    break;
	    case AB_REF_WIN_PARENT:
	        obj_set_win_parent(refObj, replacement);
	    break;
	}
    } /* for i */
	
epilogue:
    objlist_destroy(refList);
    return return_value;
}


#ifdef BOGUS /* this is of questionable usefulness and is a pain in
		the rear to implement (this doesn't work any more). */
/*
 * Replaces all references to obj with references to its replacement. The
 * original obj is left with no references to it, except from its children.
 * 
 * Possible references: parent, next_sibling, prev_sibling action.to
 */
int
obj_tree_replace(ABObj obj, ABObj replacement)
{
    AB_TRAVERSAL        trav;
    ABObj               parent = obj->parent;
    ABObj               child = NULL;
    ABObj               project = obj_get_project(obj);
    ABObj               action = NULL;
    AB_ACTION_INFO     *actinfo = NULL;

    /*
     * Child pointers
     */
    if (parent != NULL)
    {
	ABObj              *lastnext;
	lastnext = &(parent->first_child);
	child = *lastnext;
	for (; child != NULL;
	     lastnext = &(child->next_sibling), child = *lastnext)
	{
	    if ((child) == obj)
	    {
		*lastnext = replacement;
		replacement->prev_sibling = child->prev_sibling;
		replacement->next_sibling = child->next_sibling;
		if (replacement->next_sibling != NULL)
		{
		    replacement->next_sibling->prev_sibling =
			replacement;
		}
		objP_set_parent(replacement, parent);
		break;
	    }
	}
	objP_set_parent(obj, NULL);
	obj->next_sibling = NULL;
    }

    /*
     * Action sources and destinations
     */
    for (trav_open(&trav, project, AB_TRAV_ACTIONS);
	 (action = trav_next(&trav)) != NULL;)
    {
	actinfo = &(action->info.action);
	if (actinfo->to == obj)
	{
	    actinfo->to = replacement;
	}
    }
    trav_close(&trav);

    return 0;
}
#endif /* BOGUS */

/*
 * Destroys all objects that have the DestroyFlag Returns the # of objects
 * destroyed
 */
int
obj_tree_destroy_flagged_impl(ABObj *rootPtr)
{
#define root (*rootPtr)
    AB_TRAVERSAL        trav;
    ABObj               obj = NULL;
    BOOL                destroyedOne = FALSE;
    int                 numDestroyed = 0;

    if (obj_has_flag(root, BeingDestroyedFlag))
    {
	++numDestroyed;
	obj_destroy(root);
	goto epilogue;
    }

    /*
     * Destroying an object invalidates the traversal, so we have to restart
     * it.
     */
    destroyedOne = TRUE;
    while (destroyedOne)
    {
	destroyedOne = FALSE;
	for (trav_open(&trav, root, AB_TRAV_ALL);
	     (obj = trav_next(&trav)) != NULL;)
	{
	    if (obj_has_flag(obj, BeingDestroyedFlag))
	    {
		destroyedOne = TRUE;
		break;
	    }
	}
	trav_close(&trav);
	if (destroyedOne)
	{
	    ++numDestroyed;
	    obj_destroy(obj);
	}
    }

epilogue:
    return numDestroyed;
#undef root
}

int
obj_tree_set_flag(ABObj root, unsigned flags)
{
    AB_TRAVERSAL        trav;
    ABObj               obj = NULL;
    for (trav_open(&trav, root, AB_TRAV_ALL);
	 (obj = trav_next(&trav)) != NULL;)
    {
	obj_set_flag(obj, flags);
    }
    trav_close(&trav);
    return 0;
}

int
obj_tree_clear_flag(ABObj root, unsigned flags)
{
    AB_TRAVERSAL        trav;
    ABObj               obj = NULL;
    for (trav_open(&trav, root, AB_TRAV_ALL);
	 (obj = trav_next(&trav)) != NULL;)
    {
	obj_clear_flag(obj, flags);
    }
    trav_close(&trav);
    return 0;
}

/*
 * Set Flag for all SubObjs within a CompositeObject
 */
int
obj_comp_set_flag(
		  ABObj root,
		  unsigned flags
)
{
    return (comp_change_flag(root, flags, FALSE));
}

/*
 * Clear Flag for all SubObjs within a CompositeObject
 */
int
obj_comp_clear_flag(
		    ABObj root,
		    unsigned flags
)
{
    return (comp_change_flag(root, flags, TRUE));

}

static int
comp_change_flag(
		 ABObj root,
		 unsigned flags,
		 BOOL clear
)
{
    AB_TRAVERSAL        trav;
    static ABObj        comp_root = NULL;
    ABObj               comp_sub = NULL;

    if (clear)
	obj_clear_flag(root, flags);
    else
	obj_set_flag(root, flags);

    if (root->part_of == NULL)	/* No SubObjs */
	return 0;

    if (comp_root == NULL)	/* Set Root */
	comp_root = root;

    for (trav_open(&trav, root, AB_TRAV_CHILDREN);
	 (comp_sub = trav_next(&trav)) != NULL;)
    {
	if (comp_sub->part_of == comp_root)
	    comp_change_flag(comp_sub, flags, clear);
    }
    trav_close(&trav);

    if (root == comp_root)
	comp_root = NULL;	/* Reset */

    return 0;
}

/*
 * Inserts newsib before obj in obj->parent's child list
 */
int
obj_prepend_sibling(ABObj obj, ABObj newsib)
{
    ABObj               parent = obj->parent;

    return ((parent == NULL) ?
	    -1 :
	    obj_insert_child(parent, newsib,
			     obj_get_child_num(obj)));
}

/*
 * Inserts newsib after obj in obj->parent's child list
 */
int
obj_append_sibling(ABObj obj, ABObj newsib)
{
    ABObj               parent = obj->parent;

    return ((parent == NULL) ?
	    -1 :
	    obj_insert_child(parent, newsib,
			     obj_get_child_num(obj) + 1));
}

/*
 * Swap two siblings.
 */
int
obj_swap_siblings(ABObj child1, ABObj child2)
{
    ABObj	tmp_sibling;
    ABObj	parent_obj = child1->parent;
    verify_obj_write(child1);
    verify_obj_write(child2);

    tmp_sibling = child1->next_sibling;

    child1->next_sibling = child2->next_sibling;

    if (child1->next_sibling)
	child1->next_sibling->prev_sibling = child1;

    child2->next_sibling = tmp_sibling;
    if (child2->next_sibling)
	child2->next_sibling->prev_sibling = child2;

    tmp_sibling = child2->prev_sibling;

    child2->prev_sibling = child1->prev_sibling;

    if (child2->prev_sibling)
	child2->prev_sibling->next_sibling = child2;

    child1->prev_sibling = tmp_sibling;

    if (child1->prev_sibling)
	child1->prev_sibling->next_sibling = child1;

    if (parent_obj->first_child == child1)
	parent_obj->first_child = child2;

    return 0;
}


int 
obj_get_num_comp_subobjs(ABObj obj)
{
    if (!obj_is_root(obj))
    {
	return 0;
    }
    return trav_count(obj, AB_TRAV_COMP_SUBOBJS);
}


/*
 * Changes the parent of the object, keeping the names lists up-to-date.
 * Does *not* send any events.
 *
 * The old or newParent may be NULL
 */
int
objP_set_parent(ABObj obj, ABObj newParent)
{
    StringList		oldNames = objP_get_names_scope(obj);
    StringList		newNames = NULL;
    BOOL		updateNamesLists = FALSE;
    
    if (newParent != NULL)
    {
        newNames = objP_get_names_scope_for_children(newParent);
    }

    /* don't bother to traverse if the name lists are the same,
     * or this obj has no name or children 
     */
    updateNamesLists = 
	(   (oldNames != newNames)
	 && ((obj->name != NULL) || (obj->first_child != NULL)) );

    obj->parent = newParent;

    /*
     * Update the names lists for this object and all of its descendants
     */
    if (updateNamesLists)
    {
	update_names_lists(obj, oldNames, newNames);
    }

    return 0;
}


/*
 * Returns a list of objects that reference obj.
 * The data associated with each entry in the list is the type of
 * reference. For instance,
 *
 *	ABObjList 	refList = obj_get_refs_to(mainwindow); ---may be NULL!
 *	int	  	numRefs = 0;
 *	void 		*voidRefType = NULL;
 *	AB_OBJ_REF_TYPE	refType = AB_REF_UNDEF;
 *	int		i = 0;
 *      if (refList != NULL)
 *      {
 *     	    numRefs = objlist_get_num_objs(refList);
 *          for (i = 0; i < numRefs; ++i)
 *          {
 *              refObj = objlist_get_obj(refList, i, &voidRefType);
 *	        refType = (AB_OBJ_REF_TYPE)voidRefType;
 *
 *              --- refObj refers to obj, as specified by refType
 *	    }
 *	}
 *      objlist_destroy(refList);
 *
 * NOTE: MAY RETURN NULL! If this is the case, there are no references to
 *	 the object!
 */
ABObjList
obj_get_refs_to(ABObj obj)
{
    int		num_objs = objlist_get_num_objs(objP_all_objs_list);
    int		i;
    ABObjList	refList = NULL;
    ABObj	*internalObjList = NULL;
    ABObj	refObj = NULL;

    /* this is a speed hack - we grab the internal object array from
     * the object list to speed up this function, 'cause it gets
     * used lots!
     */
    internalObjList = objP_all_objs_list->objs;

    for (i = 0; i < num_objs; ++i)
    {
	refObj = internalObjList[i];
    
        if (refObj->type == AB_TYPE_PROJECT &&
	    obj_get_root_window(refObj) == obj)
        {
            add_to_ref_list(&refList, refObj, AB_REF_ROOT_WINDOW);
        }

	/* window parent */
	if (obj_is_popup_win(refObj))
	{
            if (obj_get_win_parent(refObj) == obj)
	    {
	    	add_to_ref_list(&refList, refObj, AB_REF_WIN_PARENT);
	    }

	    if (obj_get_default_act_button(refObj) == obj)
	    {
	    	add_to_ref_list(&refList, refObj, AB_REF_DEFAULT_BUTTON);
	    }
	}
    
        /* from/to */
        if (refObj->type == AB_TYPE_ACTION)
        {
	    if (refObj->info.action.from == obj)
	    {
	        add_to_ref_list(&refList, refObj, AB_REF_CONNECT_FROM);
	    }
	    if (refObj->info.action.to == obj)
	    {
	        add_to_ref_list(&refList, refObj, AB_REF_CONNECT_TO);
	    }
        }
    
	/*
	 * Attachments
	 */
        if (refObj->attachments != NULL)
        {
	    if (   (refObj->attachments->north.type == AB_ATTACH_OBJ)
	        && (refObj->attachments->north.value == (void*)obj) )
	    {
	        add_to_ref_list(&refList, refObj, AB_REF_ATTACH_NORTH);
	    }
	    if (   (refObj->attachments->south.type == AB_ATTACH_OBJ)
	        && (refObj->attachments->south.value == (void*)obj) )
	    {
	        add_to_ref_list(&refList, refObj, AB_REF_ATTACH_SOUTH);
	    }
	    if (   (refObj->attachments->east.type == AB_ATTACH_OBJ)
	        && (refObj->attachments->east.value == (void*)obj) )
	    {
	        add_to_ref_list(&refList, refObj, AB_REF_ATTACH_EAST);
	    }
	    if (   (refObj->attachments->west.type == AB_ATTACH_OBJ)
	        && (refObj->attachments->west.value == (void*)obj) )
	    {
	        add_to_ref_list(&refList, refObj, AB_REF_ATTACH_WEST);
	    }
        }

	/*
	 * Tree structure
	 */
	if (obj->parent == obj)
	{
	    add_to_ref_list(&refList, refObj, AB_REF_PARENT);
	}
	if (obj->first_child == obj)
	{
	    add_to_ref_list(&refList, refObj, AB_REF_CHILD);
	}
	if (obj->prev_sibling == obj)
	{
	    add_to_ref_list(&refList, refObj, AB_REF_SIBLING_PREV);
	}
	if (obj->next_sibling == obj)
	{
	    add_to_ref_list(&refList, refObj, AB_REF_SIBLING_NEXT);
	}
    
	/*
	 * Reference to
	 */
	if ((obj_get_ref_to(refObj) == obj) && (refObj != obj))
	{
	    add_to_ref_list(&refList, refObj, AB_REF_REF_TO);
	}
	
    } /* for i */

    return refList;
}


/*
 * refList may be NULL - creates list, if necessary
 */
static int
add_to_ref_list(
			ABObjList	*refListInOutPtr, 
			ABObj		refObj, 
			AB_OBJ_REF_TYPE	refType
)
{
#define refList (*refListInOutPtr)
    if (refList == NULL)
    {
	refList = objlist_create();
    }
    objlist_add_obj(refList, refObj, (void *)refType);
    return 0;
#undef refList
}


/*
 * Moves object names from one list to another, possibly modifying them
 * in the process.
 *
 * This routine doesn't use traversals because traversals skip destroyed
 * objects.
 */
static int
update_names_lists(
			ABObj		obj, 
			StringList	oldNamesList, 
			StringList	newNamesList
)
{
    ABObj	child = NULL;
    ISTRING	newName = NULL;

    if (obj->name != NULL)
    {
        newName = obj_alloc_unique_name_istr(obj, obj->name, -1);
        if (!istr_equal(newName, obj->name))
        {
            obj_set_name_istr(obj, newName);
        }
        else
        {
	    /* The name has not changed, so set_name() would ignore us */
	    /* need to update names lists */
    
	    if (oldNamesList != NULL)
	    {
	        strlist_remove_istr(oldNamesList, obj->name);
	    }
	    if (newNamesList != NULL)
	    {
	        strlist_add_istr(newNamesList, obj->name, obj);
	    }
        }
        istr_destroy(newName);
    } /* obj->name != NULL */

    /*
     * Update all the children of this obj
     */
    for (child = obj->first_child; child != NULL; child = child->next_sibling)
    {
        update_names_lists(child, oldNamesList, newNamesList);
    }

    return 0;
}


#ifdef DEBUG

static int
verify_obj_read_impl(ABObj obj, STRING file, int line)
{
    if (!debugging())
    {
	return 0;
    }
    if (obj == NULL)
    {
	return 0;
    }

    if (objP_update_verify(obj) < 0)
    {
	util_dprintf(0, "WARNING: Corrupt obj detected(%s:%d).\n",
		file, line);
	if (debug_level() >= 5)
	{
	    abort();
	}
    }

    return 0;
}

#endif /* DEBUG */


#ifdef DEBUG

static int
verify_obj_write_impl(ABObj obj, STRING file, int line)
{
    if (!debugging())
    {
	return 0;
    }
    if (obj == NULL)
    {
	return 0;
    }

    if (objP_update_verify(obj) < 0)
    {
	util_dprintf(0, "WARNING: Corrupt obj detected(%s:%d).\n",
		file, line);
	if (debug_level() >= 5)
	{
	    abort();
	}
    }

    return 0;
}

#endif /* DEBUG */
