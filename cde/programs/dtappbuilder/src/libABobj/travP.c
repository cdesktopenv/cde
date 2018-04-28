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
 *	$XConsortium: travP.c /main/3 1995/11/06 18:42:00 rswiston $
 *
 *	@(#)travP.c	1.9 14 Feb 1994	
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
 *  TraversalP.c - functions used by traversal code.  DO NOT USE OR MODIFY
 *  THESE FUNCTIONS UNLESS YOU ARE WORKING ON TRAVERSALS!!!
 */

#include <stdio.h>
#include "travP.h"

static ABObj	travP_obj_salient_parent(ABObj obj, ABTraversal trav);
static ABObj	travP_obj_salient_first_child(ABObj obj, ABTraversal trav);
static ABObj	travP_obj_salient_next_sibling(ABObj obj, ABTraversal trav);
static ABObj	travP_obj_salient_prev_sibling(ABObj obj, ABTraversal trav);
static ABObj	find_first_non_subobj_child(ABObj obj, ABObj compRoot);
static ABObj	find_ancestor_prev_sibling(ABObj obj, ABObj searchRoot);
static ABObj	find_ancestor_next_sibling(ABObj obj, ABObj searchRoot);


ABObj
travP_obj_parent(ABObj obj, ABTraversal trav)
{
    ABObj	parent= NULL;

    switch (travP_get_qualifier(trav))
    {
	case AB_TRAV_SALIENT:
	case AB_TRAV_SALIENT_CHILDREN:
	case AB_TRAV_SALIENT_UI:
	case AB_TRAV_SALIENT_UI_CHILDREN:
	    parent= travP_obj_salient_parent(obj, trav);
	break;

	default:
	    parent= obj->parent;
	break;
    }

    return parent;
}


ABObj
travP_obj_first_child(ABObj obj, ABTraversal trav)
{
    ABObj	child= NULL;

    switch (travP_get_qualifier(trav))
    {
	case AB_TRAV_SALIENT:
	case AB_TRAV_SALIENT_CHILDREN:
	case AB_TRAV_SALIENT_UI:
	case AB_TRAV_SALIENT_UI_CHILDREN:
	    child= travP_obj_salient_first_child(obj, trav);
	break;

	default:
	    child= obj->first_child;
	break;
    }

    return child;
}


ABObj 
travP_obj_next_sibling(ABObj obj, ABTraversal trav)
{
    ABObj	sib= NULL;

    switch (travP_get_qualifier(trav))
    {
	case AB_TRAV_SALIENT:
	case AB_TRAV_SALIENT_CHILDREN:
	case AB_TRAV_SALIENT_UI:
	case AB_TRAV_SALIENT_UI_CHILDREN:
	    sib= travP_obj_salient_next_sibling(obj, trav);
	break;

	default:
	    /*
	     * Don't go past the root node.
	     */
	    if (obj != trav->rootObj)
	    {
	        sib= obj->next_sibling;
	    }
	break;
    }

    return sib;
}


ABObj
travP_obj_prev_sibling(ABObj obj, ABTraversal trav)
{
    ABObj	sib= NULL;

    switch (travP_get_qualifier(trav))
    {
	case AB_TRAV_SALIENT:
	case AB_TRAV_SALIENT_CHILDREN:
	case AB_TRAV_SALIENT_UI:
	case AB_TRAV_SALIENT_UI_CHILDREN:
	    sib= travP_obj_salient_prev_sibling(obj, trav);
	break;

	default:
	    sib= obj->prev_sibling;
	break;
    }

    return sib;
}

/*************************************************************************
**									**
**		SALIENTS						**
**									**
**************************************************************************/

static ABObj
travP_obj_salient_parent(ABObj obj, ABTraversal trav)
{
    ABObj	parent= obj;

    if (parent->part_of != NULL)
    {
	parent= parent->part_of;
    }
    parent= parent->parent;
    if ((parent != NULL) && (parent->part_of != NULL))
    {
	parent= parent->part_of;
    }

    /*
     * Be sure to stay inside traversal bounds
     */
    if (parent != NULL)
    {
	ABObj	ancestor= NULL;
	for (ancestor= obj->parent; 
	    (ancestor != NULL) && (ancestor != parent); 
	    ancestor= ancestor->parent)
	{
	    if (ancestor == trav->rootObj)
	    {
		/* parent is above the traversal root. */
		parent= NULL;
		break;
	    }
	}
    }

    return parent;
}


static ABObj
travP_obj_salient_first_child(ABObj obj, ABTraversal trav)
{
    ABObj	child= NULL;
    trav= trav;

    if (obj_is_sub(obj) || obj_is_root(obj))
    {
	child= find_first_non_subobj_child(obj, obj_get_root(obj));
    }
    else
    {
	child= obj->first_child;
    }
    return child;
}

/*
 *  We need to find the leftmost child that is not part of this
 *  composite object.
 */
static ABObj 
find_first_non_subobj_child(ABObj obj, ABObj compRoot)
{
    AB_TRAVERSAL	trav;
    ABObj		child= NULL;

    /*
     * Mondo recursion!! - we're going to recurse back into ourselves,
     * as well as call trav_ functions.  Can you say "no global variables?"
     *
     * Thought you could. :-)
     */
    for (trav_open(&trav, obj, AB_TRAV_CHILDREN);
	(child= trav_next(&trav)) != NULL; )
    {
	if (obj_get_root(child) != compRoot)
	{
	    /*
	     * This is the object we want!!
	     */
	    break;
	}
	else
	{
	    /* this child is still part of the composite. */
	    /* look at its children */
	    child= find_first_non_subobj_child(child, compRoot);
	    if (child != NULL)
	    {
		/* 
		 * found it!!
		 */
		break;
	    }
	}
    }
    trav_close(&trav);

    return child;
}

static ABObj
travP_obj_salient_prev_sibling(ABObj obj, ABTraversal trav)
{
    ABObj	curObj= NULL;
    ABObj	compRoot= NULL;
    BOOL	firstTime= TRUE;

    curObj= obj;
    compRoot= obj_get_root(curObj->parent);
    while (   (curObj != NULL)
	   && ((!obj_is_salient(curObj)) || (firstTime)) )
    {
	firstTime= FALSE;
	if (obj_is_root(curObj))
	{
	    if (curObj->prev_sibling != NULL)
	    {
		curObj= curObj->prev_sibling;
	    }
	    else
	    {
		curObj= find_ancestor_prev_sibling(curObj, compRoot);
	    }
	}
	else
	{
	    if (curObj->first_child != NULL)
	    {
		curObj= curObj->first_child;
	    }
	    else
	    {
		if (curObj->prev_sibling != NULL)
		{
		    curObj= curObj->prev_sibling;
		}
		else
		{
		    curObj= find_ancestor_prev_sibling(curObj, compRoot);
		}
	    }
	}
    } /* while curObj */

    return curObj;
} /* travP_obj_salient_prev_sibling */


static ABObj 
travP_obj_salient_next_sibling(ABObj obj, ABTraversal trav)
{
    ABObj	curObj= NULL;
    ABObj	compRoot= NULL;
    BOOL	firstTime= TRUE;
    ABObj	searchTop = NULL;

    curObj= obj;
    compRoot= obj_get_root(curObj->parent);

    /*
     * Be sure we don't go above the traversal root.
     */
    searchTop = 
        (obj_is_descendant_of(trav->rootObj, compRoot)?
	    trav->rootObj		/* compRoot is above traversal root */
        :
	    compRoot);

    while (   (curObj != NULL)
	   && ((!obj_is_salient(curObj)) || (firstTime)) )
    {
	firstTime= FALSE;

	/* don't go past traversal root */
	if (curObj == trav->rootObj)
	{
	    curObj = NULL;
	    break;
	}

	if (obj_is_root(curObj))
	{
	    if (curObj->next_sibling != NULL)
	    {
		curObj= curObj->next_sibling;
	    }
	    else
	    {
		curObj= find_ancestor_next_sibling(curObj, searchTop);
	    }
	}
	else
	{
	    if (curObj->first_child != NULL)
	    {
		curObj= curObj->first_child;
	    }
	    else
	    {
		if (curObj->next_sibling != NULL)
		{
		    curObj= curObj->next_sibling;
		}
		else
		{
		    curObj= find_ancestor_next_sibling(curObj, searchTop);
		}
	    }
	}
    } /* while curObj */

    return curObj;
} /* travP_obj_salient_next_sibling */




/*************************************************************************
**									**
**		GENERAL UTITILITY FUNCTIONS				**
**									**
**************************************************************************/


/*
 * Finds the first ancestor with a sibling to its left (prev_sibling).
 * Returns the sibling.
 */
static ABObj
find_ancestor_prev_sibling(ABObj obj, ABObj searchRoot)
{
    ABObj	newObj= NULL;
    ABObj	ancestor= NULL;

    for (ancestor= obj->parent;
	(ancestor != NULL) && (ancestor != searchRoot);
	ancestor= ancestor->parent)
    {
	if (ancestor->prev_sibling != NULL)
	{
	    newObj= ancestor->prev_sibling;
	    break;
	}
    }

    return newObj;
}


/*
 * Finds the first ancestor with a sibling to its right (next_sibling).
 * Returns the sibling.
 */
static ABObj
find_ancestor_next_sibling(ABObj obj, ABObj searchRoot)
{
    ABObj	newObj= NULL;
    ABObj	ancestor;

    for (ancestor= obj->parent; 
	(ancestor != NULL) && (ancestor != searchRoot);
	ancestor= ancestor->parent)
    {
	if (ancestor->next_sibling != NULL)
	{
	    newObj= ancestor->next_sibling;
	    break;
	}
    }

    return newObj;
}

