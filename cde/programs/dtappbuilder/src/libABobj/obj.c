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
 * $XConsortium: obj.c /main/4 1996/10/02 16:11:56 drk $
 * 
 * @(#)obj.c	3.46 11 Feb 1994	cde_app_builder/src/libABobj
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
 * ABObj.c - manipulations of one gobj structure
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#if !defined(CSRG_BASED)
#include <values.h>
#endif
#include <stdlib.h>
#include <sys/param.h>
#include <string.h>
#include "objP.h"		/* include first! */
#include <ab_private/trav.h>
#include <ab_private/util.h>
#include "obj_names_listP.h"
#include "obj_notifyP.h"

static int 
objP_or_tree_update_clients_with_data(
				      ABObj obj,
				      BOOL update_subtree,
				      int update_code,
				      void *update_data,
				      UpdateDataFreeFunc free_func
);

/*
 * Gets the item for this object with index which_item
 */
ABObj
obj_get_item(ABObj obj, int which_item)
{
    AB_TRAVERSAL        trav;
    ABObj               item = NULL;
    int                 item_num = -1;	/* must be -1 */

    for (trav_open(&trav, obj, AB_TRAV_ITEMS_FOR_OBJ);
	 (item = trav_next(&trav)) != NULL;)
    {
	++item_num;
	if (item_num == which_item)
	{
	    break;
	}
    }
    trav_close(&trav);
    return item;
}

/*
 * Return the Help Item for a given menubar
 */
ABObj
obj_get_menubar_help_item(
			  ABObj obj
)
{
    AB_TRAVERSAL        trav;
    ABObj               help_item = NULL;
    ABObj               item;

    if (!obj_is_menubar(obj))
	return NULL;

    for (trav_open(&trav, obj, AB_TRAV_ITEMS_FOR_OBJ);
	 (item = trav_next(&trav)) != NULL;)
    {
	if (obj_is_help_item(item) == TRUE)
	{
	    help_item = item;
	    break;
	}
    }
    trav_close(&trav);
    return help_item;

}

/*
 * Returns the number of this object, as a child of it's parent.
 */
int
obj_get_child_num(ABObj obj)
{
    int                 child_num = -1;
    ABObj               child;

    if (obj->parent == NULL)
    {
	goto epilogue;
    }

    for (child = obj->parent->first_child;
	 child != NULL; child = child->next_sibling)
    {
	++child_num;
	if (obj == child)
	{
	    break;
	}
    }

epilogue:
    return child_num;
}


ABObj
obj_get_container_child(ABObj obj)
{
    ABObj               child;
    for (child = obj->first_child; child != NULL;
	 child = child->next_sibling)
    {
	if (obj_is_container(child))
	{
	    break;
	}
    }
    return child;
}


/*
 * Returns the item number of this object, with reference to it's parent
 */
int
obj_get_item_num(ABObj obj)
{
    int                 item_num = -1;
    ABObj               item;
    for (item = obj->parent->first_child;
	 item != NULL; item = item->next_sibling)
    {
	if (!obj_is_item(item))
	{
	    continue;
	}
	++item_num;
	if (item == obj)
	{
	    break;
	}
    }
    return item_num;
}


ABObj
obj_get_menu(ABObj obj)
{
    ABObj               child = NULL;
    for (child = obj->first_child; child != NULL; child = child->next_sibling)
    {
	if (obj_is_menu(child))
	{
	    break;
	}
    }
    return child;
}


ABObj
obj_get_pane_child(ABObj obj)
{
    ABObj               child;
    for (child = obj->first_child; child != NULL;
	 child = child->next_sibling)
    {
	if (obj_is_pane(child))
	{
	    break;
	}
    }
    return child;
}


BOOL
obj_has_menu(ABObj obj)
{
    if (!util_strempty(obj_get_menu_name(obj)))
	return TRUE;
    else
	return FALSE;

}

/*
 * Moves the children only if *all* the children can be moved.
 */
int
obj_move_children(ABObj to, ABObj from)
{
    int                 iReturn = 0;
    int                 iRC = 0;/* return code */
    ABObj               child = NULL;

    /*
     * Get permission for all children
     */
    for (child = from->first_child;
	 child != NULL; child = child->next_sibling)
    {
	if ((iRC = objP_notify_send_allow_reparent(child, to)) < 0)
	{
	    iReturn = iRC;
	    goto epilogue;
	}
    }

    while ((child = from->first_child) != NULL)
    {

	/*
	 * Don't send reparent to NULL
	 */

	objP_notify_push_mode();
	objP_notify_clear_mode(OBJEV_MODE_SEND_NOTIFY_EVS);
	obj_unparent(child);
	obj_append_child(to, child);
	objP_notify_pop_mode();

	objP_notify_send_reparent(child, from);
    }

epilogue:
    return iReturn;
}

/*
 * maxlen < 1 means no max.
 */
int
obj_ensure_unique_name(ABObj obj, ABObj root, int maxlen)
{
    BOOL                unique = FALSE;
    BOOL                error = FALSE;
    BOOL                name_changed = FALSE;
    char                mod_name[256];
    char                obj_name[2048];
    int                 oldlen;
    int                 modlen;
    int                 newlen;
    AB_TRAVERSAL        trav;
    ABObj               other_obj = NULL;
    int                 modifier = 0;
    unsigned            trav_type;

    if (obj->name == NULL)
    {
	return -1;
    }
    if (maxlen < 1)
    {
	maxlen = 2048;
    }
    strncpy(obj_name, istr_string(obj->name), maxlen);
    obj_name[2047] = 0;
    modifier = 0;
    unique = FALSE;
    if (obj_is_module(obj))
    {
	trav_type = AB_TRAV_MODULES;
    }
    else
    {
	trav_type = AB_TRAV_UI;
    }
    while ((!unique) && (!error))
    {
	unique = TRUE;
	for (trav_open(&trav, root, trav_type);
	     (other_obj = trav_next(&trav)) != NULL;)
	{
	    if ((other_obj != obj)
		&& (other_obj->name != NULL)
		&& (strncmp(istr_string(other_obj->name), obj_name, maxlen)
		    == 0))
	    {
		unique = FALSE;
		name_changed = TRUE;
		sprintf(mod_name, "%d", ++modifier);
		if ((strlen(mod_name) + 1) > (unsigned) maxlen)
		{
		    error = TRUE;
		    break;
		}
		oldlen = istr_len(obj->name);
		modlen = strlen(mod_name);
		newlen = oldlen + modlen;
		if (newlen > maxlen)
		{
		    newlen = maxlen;
		    oldlen = newlen - modlen;
		}
		strncpy(obj_name, istr_string(obj->name),
			oldlen);
		obj_name[oldlen] = 0;
		strcat(obj_name, mod_name);
	    }
	}
	trav_close(&trav);
    }
    if (name_changed && (!error))
    {
	/* printf("CHANGED %s -> %s\n", obj->name, obj_name); */

	/*
	 * REMIND: CHANGE ALL BY-NAME REFS TO THIS OBJECT!
	 */
	obj_set_name(obj, obj_name);
    }

    return error ? -1 : 0;
}


/*
 * Munges name, as necessary to get uniqueness
 */
int
obj_set_unique_name_istr(ABObj obj, ISTRING name)
{
    int		rc = 0;		/* return code */
    ISTRING	newName = obj_alloc_unique_name_istr(obj, name, -1);
    rc = obj_set_name_istr(obj, newName);
    istr_destroy(newName);
    return rc;
}


int
obj_set_unique_name(ABObj obj, STRING strName)
{
    int		return_value = 0;
    ISTRING	name = istr_create(strName);

    return_value = obj_set_unique_name_istr(obj, name);

    istr_destroy(name);
    return return_value;
}


STRING		
obj_alloc_unique_name(
			ABObj	obj, 
			STRING	name, 
			int	maxNameLen
)
{
    ABObj	parent = obj->parent;
    if (   (parent == NULL)
        || (obj_find_by_name(parent, name) == obj) )
    {
	return strdup(name);
    }
    return obj_alloc_unique_name_for_child(parent, name, maxNameLen);
}


ISTRING		
obj_alloc_unique_name_istr(
			ABObj	obj, 
			ISTRING	name, 
			int	maxNameLen
)
{
    ABObj	parent = obj->parent;
    if (   (parent == NULL)
        || (obj_find_by_name(parent, istr_string(name)) == obj) )
    {
	return istr_dup(name);
    }
    return obj_alloc_unique_name_istr_for_child(parent, name, maxNameLen);
}


STRING		
obj_get_unique_name(
			ABObj	obj, 
			STRING	name, 
			int	maxNameLen,
			STRING	nameOutBuf
)
{
    ABObj	parent = obj->parent;
    if (   (parent == NULL)
        || (obj_find_by_name(parent, name) == obj) )
    {
	return name;
    }
    return obj_get_unique_name_for_child(parent, name, maxNameLen-1, nameOutBuf);
}


/*
 * maxlen < 1 means no max.
 */
STRING
obj_alloc_unique_name_for_child(
		      ABObj	obj,
		      STRING	name,
		      int	maxlen
)
{
    char	nameBuf[8192];
    STRING	unique_name = NULL;
    *nameBuf = 0;

    if (maxlen < 0)
    {
	maxlen = 8192;
    }
#if defined (USL) || defined(__uxp__)
	/*
         * The USL specific changes were added ifdef due to time constraints
	 * They should be removed in the next release
	 */
    maxlen = util_min(8191,maxlen);	/* don't overrun buf */
#else
    maxlen = util_min(8192,maxlen);	/* don't overrun buf */
#endif

    /*
     * Alloc space and copy-in the unique name
     */
    unique_name = strdup(
		obj_get_unique_name_for_child(obj, name, maxlen-1, nameBuf));
     
    return unique_name;
}


ISTRING
obj_alloc_unique_name_istr_for_child(
		      ABObj	obj,
		      ISTRING	name,
		      int	maxlen
)
{
    char	nameBuf[8192];
    ISTRING	unique_name = NULL;
    *nameBuf = 0;

    if (maxlen < 0)
    {
	maxlen = 8192;
    }
#if defined (USL) || defined(__uxp__)
    maxlen = util_min(8191,maxlen);
#else
    maxlen = util_min(8192,maxlen);
#endif
    unique_name = istr_create(
	obj_get_unique_name_for_child(obj, istr_string(name), maxlen-1, nameBuf));

    return unique_name;
}


STRING
obj_get_unique_name_for_child(
    ABObj	obj,
    STRING	name,
    int		maxlen,
    STRING	nameOutBuf
)
{
    STRING	returnName = NULL;
    int		nameOutBufSize = maxlen+1;	
    StringList	namesList = NULL;
    BOOL	done = FALSE;
    BOOL	unique = FALSE;
    int		nameLen = 0;
    int		i = 0;
    int		objNumberStart = 0;
    int		objNumber = 0;

    namesList = objP_get_names_scope_for_children(obj);
    if (namesList == NULL)
    {
	returnName = name;
	goto epilogue;
    }

    unique = !strlist_str_exists(namesList, name);
    if (unique)
    {
	returnName = name;
	goto epilogue;
    }

    /*
     * Determine trailing number
     */
    util_strncpy(nameOutBuf, name, nameOutBufSize);
    nameLen = strlen(nameOutBuf);
    for (i = nameLen-1; i >= 0; --i)
    {
	if (!isdigit(nameOutBuf[i]))
	{
	    break;
	}
    }
    objNumberStart = i + 1;
    objNumber = atoi(&(nameOutBuf[objNumberStart]));

    /*
     * Search for a unique name by incrementing trailing #
     */
    unique = FALSE;
    done = FALSE;
    while (!done)
    {
	if (++objNumber == 1)
	{
	    /* skip 1 - name w/no # is implied 1 */
	    objNumber = 2;
	}
	sprintf(&(nameOutBuf[objNumberStart]), "%d", objNumber);
	unique = !strlist_str_exists(namesList, nameOutBuf);
	done = unique;
    }
    if (unique)
    {
	returnName = nameOutBuf;
    }

epilogue:
    return returnName;
}


/*
 * Don't run this on a project!!! It's a severe dog, and can take many
 * seconds to complete.
 * 
 * Run it on each module (root = an module) instead.
 */
int
obj_tree_ensure_unique_names(ABObj root, int maxnamelen)
{
    AB_TRAVERSAL        trav;
    ABObj               obj;

    for (trav_open(&trav, root, AB_TRAV_UI);
	 (obj = trav_next(&trav)) != NULL;)
    {
	obj_ensure_unique_name(obj, root, maxnamelen);
    }
    trav_close(&trav);
    return 0;
}


/*
 * Calls obj_tree_ensure_unique_names on each module in the given tree.
 */
int
obj_tree_ensure_unique_names_in_modules(ABObj root, int maxnamelen)
{
    AB_TRAVERSAL        trav;
    ABObj               module;

    for (trav_open(&trav, root, AB_TRAV_MODULES);
	 (module = trav_next(&trav)) != NULL;)
    {
	obj_tree_ensure_unique_names(module, maxnamelen);
    }
    trav_close(&trav);
    return 0;
}

/*
 * Gives an object a name by munging it's parent's name with it's own label.
 * Used for item children, which aren't given names directly.
 * 
 * actual_parent overrides the parent of the object.  If actual_parent is NULL,
 * the "normal" parent of the object is used.
 */
int
obj_set_name_from_label(ABObj obj, STRING parent_name_in)
{
    STRING              parent_name = NULL;
    STRING		item_label = NULL;
    STRING              new_name = NULL;

    parent_name =
	(parent_name_in == NULL ?
	 (obj->parent == NULL ?
	  NULL
	  :
	  istr_string(obj->parent->name)
	  )
	 :
	 parent_name
	);
    if (parent_name == NULL)
    {
	return -1;
    }

    if (istr_len(obj->label) > 0)
    {
	item_label = istr_string(obj->label);
    }
    else
    {
	item_label = "item";
    }

    obj_set_unique_name(obj,
		 ab_ident_from_name_and_label(parent_name, item_label));

    return 0;
}

int
obj_set_name_from_parent(ABObj obj, STRING suffix)
{
    STRING              parent_name;
    STRING              new_name = NULL;

    parent_name = (obj->parent == NULL ?
		   NULL
		   :
		   istr_string(obj->parent->name)
	);

    if (parent_name == NULL)
    {
	return -1;
    }

    obj_set_unique_name(obj,
		 ab_ident_from_name_and_label(parent_name,
					      suffix));

    return 0;

}

/*
 * Gets a name for an object. Object may be NULL, or have a NULL name
 * 
 * Copies name into passed-in buffer name, guarantees 0 termination. returns
 * parameter string
 */
STRING
obj_get_safe_name(ABObj obj, STRING name, int name_size)
{
    if (name_size < 1)
    {
	return name;
    }

    if (obj == NULL)
    {
	strncpy(name, "(nil ABObj)", name_size);
    }
    else if (obj_get_name(obj) == NULL)
    {
	char                buf[256];
	sprintf(buf, "(ABObj:0x%08lx)", obj);
	strncpy(name, buf, name_size);
    }
    else
    {
	strncpy(name, obj_get_name(obj), name_size);
    }

    name[name_size - 1] = 0;

    return name;
}

/*************************************************************************
**									**
**		UPDATE_CLIENTS METHODS					**
**									**
*************************************************************************/

int
obj_update_clients(ABObj obj)
{
    return objP_notify_send_update(obj, FALSE);
}


int
obj_tree_update_clients(ABObj tree)

{
    return objP_notify_send_update(tree, TRUE);
}


int
obj_update_clients_with_data(
			     ABObj obj,
			     int update_code,
			     void *update_data,
			     UpdateDataFreeFunc free_func)
{
    return objP_or_tree_update_clients_with_data(
			   obj, FALSE, update_code, update_data, free_func);
}


int
obj_tree_update_clients_with_data(ABObj obj,
				  int update_code,
				  void *update_data,
				  UpdateDataFreeFunc free_func)
{
    return objP_or_tree_update_clients_with_data(
			    obj, TRUE, update_code, update_data, free_func);
}


static int
objP_or_tree_update_clients_with_data(
				      ABObj obj,
				      BOOL update_subtree,
				      int update_code,
				      void *update_data,
				      UpdateDataFreeFunc free_func
)
{
    int                 iReturn = 0;

    /*
     * event notification will only free the data if the notify events are
     * batched.  It's pretty smart about it, so we're going to go to batch
     * mode and let it handle it.
     */
    objP_notify_push_mode();
    objP_notify_set_mode(OBJEV_MODE_BATCH_NOTIFY_EVS);

    iReturn = objP_notify_send_update_with_data(
		  obj, update_subtree, update_code, update_data, free_func);

    objP_notify_pop_mode();
    return iReturn;
}
