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
 *	$XConsortium: trav.c /main/3 1995/11/06 18:41:30 rswiston $
 *
 * @(#)trav.c	3.41 14 Feb 1994	cde_app_builder/src/libABobj
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
 *  traversal.c - traversals of object trees
 */

#include <stdio.h>
#include <stdlib.h>
#include "objP.h"
#include "travP.h"

static int	travP_find_first(ABTraversal trav);
static int	travP_find_next(ABTraversal trav);
static int	travP_find_next_parents_first(ABTraversal trav);
static int	travP_find_first_for_parents(ABTraversal trav);
static int	travP_find_next_for_parents(ABTraversal trav);
static BOOL	travP_qualifies(ABTraversal trav);
static int	travP_verify_type(ABTraversal travType, BOOL warn);
static ABObj	find_first_item(ABObj obj);
static ABObj	find_first_child_of_type(ABObj obj, 
				ABTraversal trav, AB_OBJECT_TYPE type);
static ABObj	find_first_action(ABObj, ABTraversal);
static ABObj	find_first_sibling_child(ABObj obj, ABTraversal trav);
static ABObj	find_next_sibling_child(ABObj obj, ABTraversal trav);
static ABObj	find_first_ancestor_sibling_child(ABObj obj, ABTraversal trav);


int
travP_open(
			ABTraversal trav, ABObj root, unsigned travType,
			ABObjTestFunc	testFunc)
{
	travP_clean(trav);
	trav->travType= travType;
	trav->rootObj= root;
	trav->testFunc= testFunc;
	travP_reset(trav);
	travP_verify_type(trav, TRUE);
	return 0;
}


int 
travP_close(ABTraversal trav)
{
    trav->rootObj= NULL;
    trav->travType= AB_TRAV_UNDEF;
    return 0;
}

/*
 * Restarts the traversal from the beginning.  With a safe traversal,
 * the list of objects is not recalculated.
 */
int
travP_reset(ABTraversal trav)
{
	trav->curObj= NULL;
	trav->done= FALSE;
	return 0;
}


/*
 * Hint: travP_goto(trav, -1) is equivalent to travP_reset(trav).
 */
ABObj
travP_goto(ABTraversal trav, int node_num)
{
	int	i;

	travP_reset(trav);
	for (i= 0; i <= node_num; ++i)
	{
		travP_next(trav);
	}

	return trav->curObj;
}


/*
 * Effects: gets the next object in the traversal
 * Modifies: checks/sets done flag
 */
ABObj
travP_next(ABTraversal trav)
{
	BOOL	found_next= FALSE;

	if (trav->done)
	{
	    return NULL;
	}
	else if (trav->curObj == NULL)
	{
	    /* starting a new traversal */
	    travP_find_first(trav);
	    found_next= ((trav->curObj == NULL) || travP_qualifies(trav));
	}

	while (!found_next)
	{
		travP_find_next(trav);
		if (   (trav->curObj == NULL)
		    || (travP_qualifies(trav)) )
		{
			found_next= TRUE;
		}
	} /* while !found_next */

	if (trav->curObj == NULL)
	{
	    trav->done= TRUE;
	}

	return trav->curObj;
} /* travP_next */


/*
 * ASSUMES: rootObj is not NULL
 *		- done is FALSE
 */
static int
travP_find_first(ABTraversal trav)
{
#define rootObj (trav->rootObj)
#define curObj (trav->curObj)
    curObj = NULL;

    switch (travP_get_qualifier(trav))
    {
	case AB_TRAV_ACTIONS_FOR_OBJ:
	    curObj= find_first_action(rootObj, trav);
	break;

	case AB_TRAV_CHILDREN:
	case AB_TRAV_SALIENT_CHILDREN:
	case AB_TRAV_SALIENT_UI_CHILDREN:
	    curObj= travP_obj_first_child(rootObj, trav);
	break;

	case AB_TRAV_COMP_SUBOBJS:
	    if (obj_is_root(rootObj))
	    {
		curObj = rootObj;
	    }
	break;

	case AB_TRAV_ITEMS_FOR_OBJ:
	    curObj= find_first_item(rootObj);
	break;

	case AB_TRAV_MODULES:
	    curObj= obj_is_module(rootObj)?
			rootObj
		    :
			find_first_child_of_type(rootObj, trav, AB_TYPE_MODULE);
	break;

	case AB_TRAV_PARENTS:
	    travP_find_first_for_parents(trav);
	break;

	case AB_TRAV_SIBLINGS:
	{
	    ABObj	prevSibling= NULL;
	    curObj= rootObj;
	    while ((prevSibling= travP_obj_prev_sibling(curObj, trav)) != NULL)
	    {
		curObj= prevSibling;
	    }
	}
	break;

	default:
	    curObj= rootObj;
	break;
    }

    return 0;
#undef rootObj
#undef curObj
}


/*
 * Assumes:	- traversal is not "done"
 *		- curObj is not NULL
 */
static int
travP_find_next(ABTraversal trav)
{
#define curObj (trav->curObj)
    switch (travP_get_qualifier(trav))
   {
	/*
	 * Types that return only a set of siblings
	 */
	case AB_TRAV_ACTIONS_FOR_OBJ:
	case AB_TRAV_CHILDREN:
	case AB_TRAV_ITEMS_FOR_OBJ:
	case AB_TRAV_MODULES:
	case AB_TRAV_SALIENT_CHILDREN:
	case AB_TRAV_SALIENT_UI_CHILDREN:
	case AB_TRAV_SIBLINGS:
	    curObj= travP_obj_next_sibling(curObj, trav);
	break;

	/* 
	 * Types that return parents
	 */
	case AB_TRAV_PARENTS:
	    travP_find_next_for_parents(trav);
	break;

	/*
	 * Traversals that return an entire tree
	 */
	default:
	    travP_find_next_parents_first(trav);
	break;
    }

    return 0;
#undef curObj
} /* travP_find_next */


static int
travP_find_first_for_parents(ABTraversal trav)
{
#define curObj (trav->curObj)
    ABObj	parent= NULL;

    if (travP_is_parents_first(trav))
    {
	curObj= travP_obj_parent(trav->rootObj, trav);
	if (curObj != NULL)
	{
	    while ((parent= travP_obj_parent(curObj, trav)) != NULL)
	    {
		curObj= parent;
	    }
	}
    }
    else
    {
        curObj= travP_obj_parent(trav->rootObj, trav);
    }

    return 0;
#undef curObj
}


static int
travP_find_next_for_parents(ABTraversal trav)
{
#define curObj (trav->curObj)

    if (travP_is_parents_first(trav))
    {
	curObj= travP_obj_first_child(curObj, trav);
	if (curObj == trav->rootObj)
	{
	    curObj= NULL;
	}
    }
    else
    {
        curObj= travP_obj_parent(curObj, trav);
    }

    return 0;
#undef curObj
}

/*****************************************************************
**								**
**	PARENTS_FIRST TRAVERSAL					**
**								**
******************************************************************/


/*
 * Assumes: traversal is open and not done
 */
static int
travP_find_next_parents_first(ABTraversal trav)
{
    ABObj	curObj= trav->curObj;
    ABObj	tmpObj= NULL;

    if (curObj == trav->rootObj)
    {
	curObj= travP_obj_first_child(curObj, trav);
    }
    else
    {
	if ((tmpObj= travP_obj_next_sibling(curObj, trav)) != NULL)
	{
	    curObj= tmpObj;
	}
	else
	{
	    tmpObj= find_first_sibling_child(curObj, trav);
	    if (tmpObj == NULL)
	    {
	        tmpObj= find_first_ancestor_sibling_child(curObj, trav);
	    }
	    curObj= tmpObj;
	}
    }

    trav->curObj= curObj;
    return 0;
}


static ABObj
find_first_item(ABObj obj)
{
    ABObj	compRoot= obj_get_root(obj);
    ABObj	child= NULL;
    ABObj	item= NULL;

    for (child= obj->first_child; child != NULL; child= child->next_sibling)
    {
	if (obj_get_root(child) == compRoot)
	{
	   /* it's a subobj - look at its children */
	   item= find_first_item(child);
	   if (item != NULL)
	   {
	       break;
	   }
	}
	else
	{
	    if (obj_is_item(child))
	    {
		item= child;
		break;
	    }
	}
    }

    return item;
}


/*
 * Finds the first sibling (from "left" to "right") of the object
 * that has a child.  Returns the first child of that sibling.
 */
static ABObj
find_first_sibling_child(ABObj obj, ABTraversal trav)
{
    ABObj	sibling= obj;
    ABObj	child= NULL;
    ABObj	prevSibling= NULL;
    ABObj	firstChild= NULL;

    while ((prevSibling= travP_obj_prev_sibling(sibling, trav)) != NULL)
    {
	sibling= prevSibling;
    }

    if ((firstChild= travP_obj_first_child(sibling, trav)) != NULL)
    {
	child= firstChild;
    }
    else
    {
        child= find_next_sibling_child(sibling, trav);
    }
    return child;
}


/*
 * Finds the next sibling of the object that has a child.
 * Returns the first child of that sibling.
 */
static ABObj
find_next_sibling_child(ABObj obj, ABTraversal trav)
{
    ABObj	sibling= travP_obj_next_sibling(obj, trav);
    ABObj	child= NULL;
    ABObj	firstChild= NULL;

    while (   (sibling != NULL) 
	   && ((firstChild= travP_obj_first_child(sibling, trav)) == NULL) )
    {
	sibling= travP_obj_next_sibling(sibling, trav);
    }

    if (sibling != NULL)
    {
	child= firstChild;
    }
    return child;
}


/*
 * Finds the first ancestor that has a sibling to its "right"
 * that has a child.  Returns the child.
 */
static ABObj
find_first_ancestor_sibling_child(ABObj obj, ABTraversal trav)
{
    ABObj	ancestor= travP_obj_parent(obj, trav);
    ABObj	child= NULL;

    for (; ((ancestor != NULL) && (ancestor != trav->rootObj));
	  ancestor= travP_obj_parent(ancestor, trav))
    {
	child= find_next_sibling_child(ancestor, trav);
	if (child != NULL)
	{
	    break;
	}
    }

    return child;
}


/*
 *  Check the new current object to see if it qualifies for the traversal.
 *
 *  Assumes: trav->curObj is not null
 */
static BOOL
travP_qualifies(ABTraversal trav)
{
	ABObj	curObj = trav->curObj;
	BOOL	qualifies= TRUE;

	if (obj_has_impl_flags(curObj, ObjFlagDestroyed))
	{
	    qualifies= FALSE;
	    goto epilogue;
	}

	switch (travP_get_qualifier(trav))
	{
		case AB_TRAV_ACTIONS:
		case AB_TRAV_ACTIONS_FOR_OBJ:
			qualifies= obj_is_action(curObj);
			break;

		case AB_TRAV_COMP_SUBOBJS:
			qualifies = (obj_get_root(curObj) == trav->rootObj);
			break;

		case AB_TRAV_FILES:
			qualifies= (curObj->type == AB_TYPE_FILE);
			break;

		case AB_TRAV_GROUPS:
			qualifies= obj_is_group(curObj);
			break;

		case AB_TRAV_ITEMS:
		case AB_TRAV_ITEMS_FOR_OBJ:
			qualifies= obj_is_item(curObj);
			break;

		case AB_TRAV_MENUS:
			qualifies= obj_is_menu(curObj);
			break;

		case AB_TRAV_MODULES:
			qualifies= obj_is_module(curObj);
			break;

		case AB_TRAV_SALIENT:
		case AB_TRAV_SALIENT_CHILDREN:
			qualifies= obj_is_salient(curObj);
			break;

		case AB_TRAV_SALIENT_UI:
		case AB_TRAV_SALIENT_UI_CHILDREN:
			qualifies= obj_is_salient_ui(curObj);
			break;

		case AB_TRAV_UI:	
			qualifies= obj_is_ui(curObj);
			break;

		case AB_TRAV_WINDOWS:
			qualifies= obj_is_window(curObj);
			break;
	}

	if ((qualifies) && (trav->testFunc != NULL))
	{
	    qualifies = trav->testFunc(curObj);
	}

epilogue:
	return qualifies;
} /* travP_qualifies */


/*
 * Verifies that the traversal type is valid, and changes
 * it, if necessary.
 */
static int
travP_verify_type(ABTraversal trav, BOOL warn)
{
	int		iRet= 0;
	unsigned	travQualifier= travP_get_qualifier(trav);
	char		errMsg[256];
	errMsg[0]= 0;

	switch (travQualifier)
	{
	    case AB_TRAV_ACTIONS:
	    case AB_TRAV_ACTIONS_FOR_OBJ:
	    case AB_TRAV_ALL:
	    case AB_TRAV_CHILDREN:
	    case AB_TRAV_COMP_SUBOBJS:
	    case AB_TRAV_FILES:
	    case AB_TRAV_GROUPS:
	    case AB_TRAV_ITEMS:
	    case AB_TRAV_ITEMS_FOR_OBJ:
	    case AB_TRAV_MODULES:
	    case AB_TRAV_MENUS:
	    case AB_TRAV_SALIENT:
	    case AB_TRAV_SALIENT_CHILDREN:
	    case AB_TRAV_SALIENT_UI:
	    case AB_TRAV_SALIENT_UI_CHILDREN:
	    case AB_TRAV_SIBLINGS:
	    case AB_TRAV_UI:
	    case AB_TRAV_WINDOWS:
		/*
		 * All supported
		 */
	    break;

	    case AB_TRAV_PARENTS:
		if (travP_is_parents_first(trav) && debugging())
		{
		    sprintf(errMsg,
			"WARNING: "
			"AB_TRAV_PARENTS - Ignoring unsupported modifier "
			"AB_TRAV_MOD_PARENTS_FIRST.");
		    trav->travType &= ~AB_TRAV_MOD_PARENTS_FIRST;
		}
	    break;

	    default:
		if (debugging())
		{
		    sprintf(errMsg, "WARNING: travType %d unknown - "
			"Using AB_TRAV_ALL.",
			travQualifier);
		}
		travQualifier= AB_TRAV_ALL;
		trav->travType &= TRAV_MODIFIER_MASK;
		trav->travType |= travQualifier;
	    break;
	}

	if (warn && ((*errMsg) != 0))
	{
		fprintf(stderr, "%s\n", errMsg);
	}

	return iRet;
}


/*
 * If parent is NULL, returns NULL.
 */
static ABObj	
find_first_child_of_type(ABObj parent, ABTraversal trav, AB_OBJECT_TYPE type)
{
    ABObj	child= NULL;

    if (parent == NULL)
    {
        return NULL;
    }
    for (child= travP_obj_first_child(parent, trav);
	    child != NULL; child= travP_obj_next_sibling(child, trav))
    {
        if (child->type == type)
        {
   	    break;
	}
    }
    return child;
}


static ABObj
find_first_action(ABObj obj, ABTraversal trav)
{
	obj= find_first_child_of_type(obj, trav, AB_TYPE_ACTION_LIST);
	if (obj != NULL)
	{
	    obj= find_first_child_of_type(obj, trav, AB_TYPE_ACTION);
	}
	return obj;
}

