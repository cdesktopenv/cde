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
 *	$XConsortium: obj_action.c /main/3 1995/11/06 18:33:18 rswiston $
 *
 * @(#)obj_action.c	3.14 13 Feb 1994	cde_app_builder/src/libABobj
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
 *  action.c - action stuff for gobjs
 */

#include <string.h>
#include "objP.h"		/* include before other ab includes! */
#include <ab_private/util.h>
#include <ab_private/trav.h>

static BOOL	actions_equal(ABObj action1, ABObj action2);

/*
 * Adds an action to a module, adding an action list, if necessary.
 * Will only add an action to a project, module, or actionlist.
 */
int
obj_add_action(ABObj obj, ABObj action)
{
	int	retval= 0;
	ABObj	child= NULL;
	ABObj	actionlist= NULL;

	if (   (!obj_is_module(obj))
	    && (!obj_is_project(obj))
	    && (!obj_is_action_list(obj)) )
	{
		return -1;
	}
	if (obj_is_action_list(obj))
	{
		actionlist= obj;
	}
	else
	{
		/* find the action list for this node */
		for (child= obj->first_child; 
			child != NULL; child= child->next_sibling)
		{
			if (child->type == AB_TYPE_ACTION_LIST)
			{
				actionlist= child;
				break;
			}
		}
	}
	if (actionlist == NULL)
	{
		/* there BE no action list - create one */
		actionlist= obj_create(AB_TYPE_ACTION_LIST, NULL);
		obj_prepend_child(obj, actionlist);
	}
	
	retval= obj_append_child(actionlist, action);

	return retval;
}



ABObj
obj_find_action(ABObj root, ABObj action_template)
{
	AB_TRAVERSAL	trav;
	ABObj		action= NULL;
	for (trav_open(&trav, root, AB_TRAV_ACTIONS);
		(action= trav_next(&trav)) != NULL; )
	{
		if (actions_equal(action, action_template))
		{
			break;
		}
	}
	trav_close(&trav);
	return action;
}

static BOOL
actions_equal(ABObj action1, ABObj action2)
{
	BOOL	equal= FALSE;
	AB_ACTION_INFO	*act1= &(action1->info.action);
	AB_ACTION_INFO	*act2= &(action2->info.action);

	if (action1 == action2)
	{
		return TRUE;
	}
	equal= FALSE;
	if (   (act1->func_type == act2->func_type)
	    && (act1->to == act2->to) )
	{
		equal= TRUE;
	}
	if (equal)
	{
		switch (act1->func_type)
		{
			case AB_FUNC_BUILTIN:
				equal= (act1->func_value.builtin ==
						act2->func_value.builtin);
			break;

			case AB_FUNC_USER_DEF:
				equal= istr_equal(act1->func_value.func_name,
						act2->func_value.func_name);
			break;

			case AB_FUNC_CODE_FRAG:
				equal= istr_equal(act1->func_value.code_frag,
						act2->func_value.code_frag);
			break;
		}
	}
	return equal;
}

