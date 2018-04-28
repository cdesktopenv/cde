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
 *	$XConsortium: bil_loadact.c /main/3 1995/11/06 18:24:00 rswiston $
 *
 * @(#)bil_loadact.c	1.47 02 Feb 1995
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
 *  billdact.c - load action and handler attributes
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ab_private/trav.h>
#include <ab_private/util.h>
#include <ab/util_types.h>
#include "../libABobj/obj_utils.h"
#include "load.h"
#include "bil_loadattP.h"
#include "bilP.h"

/*static int	add_user_handler(ABObj obj, ABObj module,
				ISTRING handler, AB_WHEN when);
static int	install_action(ABObj obj, ABObj module, ABObj action);*/

static int	install_action(ABObj obj, ABObj module, ABObj action);
#ifdef BOGUS
static ABObj    find_or_create_target(ABObj obj, ABObj module,
                        ISTRING parent_name, ISTRING name);
#endif /* BOGUS */

#define nset_att(a) (bilP_load_set_current_att(a))

/*************************************************************************
**									**
**									**
**		General actions						**
**									**
**									**
**************************************************************************/


/*
 * Get the source object.
 */
STRING
bilP_load_att_from(BIL_TOKEN valueToken)
{
    ABObj       newObj = NULL;
    ABObj       src_module = NULL;
    STRING      stringValue = bilP_load_get_value();
 
#ifdef DEBUG
    util_dprintf(4, "bilP_load_att_from: %d/%s\n",
		valueToken,
		str_safe(bilP_token_to_string(valueToken)));
#endif

    nset_att(AB_BIL_FROM);
    if (valueToken == AB_BIL_APPLICATION)
    {
	bilP_load.obj->info.action.from = bilP_load.project;
    }
    else if (valueToken == AB_BIL_VALUE_IDENT)
    {
	/* This will either find the correct ABObj for the connection
	 * source or it will create a proxy object for it.  If the
	 * connection is a cross-module one, this will create both
	 * the source module and the source object if they have not
	 * yet been loaded into the tree.  In this case, the dummy
	 * module and object will be handled when the bil file is
	 * read in and the name of the module is encountered.  See
	 * bilP_load_att_name() in bil_loadatt.c.
	 */
	newObj = obj_scoped_find_or_create_undef(bilP_load.module, 
			stringValue, AB_TYPE_UNKNOWN);
	src_module = obj_get_module(newObj);

	/* bilP_load.obj is the action created in bilP_load_att_class().
	 * If the connection is a cross-module one, it is stored off of
	 * the project object, otherwise it is stored off of the module.
	 */
	bilP_load.obj->info.action.from = newObj;
    }
    else if (valueToken == AB_BIL_NIL)
    {
    }
    else
    {
	abil_print_load_err(ERR_WANT_NAME);
    }
    return NULL;
}


/*
 * Get the destination object.
 */
STRING
bilP_load_att_to(BIL_TOKEN valueToken)
{
    ABObj       toObj= NULL; 
    STRING      stringValue= bilP_load_get_value(); 
 
#ifdef DEBUG
    util_dprintf(3, "bilP_load_att_to: %d/%s\n",
		valueToken,
		str_safe(bilP_token_to_string(valueToken)));
#endif

    nset_att(AB_BIL_TO);

    if (valueToken == AB_BIL_VALUE_IDENT) 
    { 
        /* This will either find the correct ABObj for the connection
         * source or it will create a proxy object for it.  If the
         * connection is a cross-module one, this will create both
         * the source module and the source object if they have not
         * yet been loaded into the tree.  In this case, the dummy
         * module and object will be handled when the bil file is
         * read in and the name of the module is encountered.  See
         * bilP_load_att_name() in bil_loadatt.c.
         */
	toObj = obj_scoped_find_or_create_undef(bilP_load.module, 
			stringValue, AB_TYPE_UNKNOWN);
	bilP_load.obj->info.action.to = toObj;
    } 
    else if (valueToken == AB_BIL_NIL) 
    { 
    } 
    else 
    {  
        abil_print_load_err(ERR_WANT_NAME);
    }
    return NULL;
}


/*
 * Get the when part of a connection.
 */
STRING
bilP_load_att_when(BIL_TOKEN valueToken)
{
	AB_WHEN			when= AB_WHEN_UNDEF;

#ifdef DEBUG
    util_dprintf(3, "bilP_load_att_when: %d/%s\n",
		valueToken,
		str_safe(bilP_token_to_string(valueToken)));
#endif

/*
	abil_loadmsg_set_att(":when");
*/
        nset_att(AB_BIL_WHEN);

        when = bilP_token_to_when(valueToken);
        if (when == AB_WHEN_UNDEF)
	{
 	    abil_print_load_err(ERR_WANT_NAME);
	}

	/* If the source object is a radiobox or
	 * checkbox item and the when is :activate,
	 * convert it to be :toggle. This is done
	 * to maintain backwards compatibility.
	 */
	if (obj_is_choice_item(bilP_load.obj->info.action.from) &&
	    !obj_is_option_menu(obj_get_root(
		obj_get_parent(bilP_load.obj->info.action.from))))
	{
	    if (when == AB_WHEN_ACTIVATED)
	    	when = AB_WHEN_TOGGLED;    
	}
	bilP_load.obj->info.action.when = when;

	return NULL;
}

/*
 * Get the action_type for an action holder
 */
STRING
bilP_load_att_action_type(BIL_TOKEN valueToken)
{
    AB_FUNC_TYPE func_type = bilP_token_to_func_type(valueToken);
    
    util_dprintf(3, "bilP_load_att_action: %d/%s\n",
                valueToken,
                str_safe(bilP_token_to_string(valueToken)));

    nset_att(AB_BIL_ACTION_TYPE);
    if (func_type == AB_FUNC_UNDEF)
    {
	abil_print_load_err(ERR_UNKNOWN_OBJ);
	return NULL;
    }
    obj_set_func_type(bilP_load.obj, func_type);
    return NULL;
}

STRING
bilP_load_att_action(BIL_TOKEN valueToken)
{
    STRING	stringValue = bilP_load_get_value();

#ifdef DEBUG
    util_dprintf(3, "bilP_load_att_action: %d/%s\n",
		valueToken,
		str_safe(bilP_token_to_string(valueToken)));
#endif

    nset_att(AB_BIL_ACTION);

    /* Check if this is user defined */
    if (valueToken == AB_BIL_VALUE_IDENT)
    {
	obj_set_func_name(bilP_load.obj, stringValue);
    }
    /* Check if this is execute code */
    else 
	if (valueToken == AB_BIL_VALUE_STRING)
    	{
	    obj_set_func_code(bilP_load.obj, stringValue);
    	}
	else 
	    {
		AB_BUILTIN_ACTION	action = AB_STDACT_UNDEF;
		action = bilP_token_to_builtin_action(valueToken);
		obj_set_func_builtin(bilP_load.obj, action);
    	    }
    return NULL;
}

/*
 * Get the arg_type of an action_holder
 */
STRING
bilP_load_att_arg_type(BIL_TOKEN valueToken)
{
    AB_ARG_TYPE arg_type = bilP_token_to_arg_type(valueToken);

#ifdef DEBUG
    util_dprintf(3, "bilP_load_att_arg_type: %d/%s\n",
                valueToken,
                str_safe(bilP_token_to_string(valueToken)));
#endif

    nset_att(AB_BIL_ARG_TYPE);

    if (arg_type == AB_ARG_UNDEF)
    {
        abil_print_load_err(ERR_UNKNOWN_OBJ);
        return NULL;
    }
    obj_set_arg_type(bilP_load.obj, arg_type);
    return NULL;
}


static int
install_action(ABObj obj, ABObj module, ABObj action)
{
	ABObj	oldaction= NULL;
	ABObj	project= obj_get_project(module);

	/*
	 * For efficiency, we check this module first to avoid searching
	 * the entire project unless necessary.
	 */

	oldaction= obj_find_action(module, action);
	if (oldaction == NULL)
	{
		oldaction= obj_find_action(project, action);
	}

	if (oldaction != NULL)
	{
		/* we've seen this action, before */
		obj_destroy(action);
		action= oldaction;
		if (obj_get_module(action) != module)
		{
			/*
			 *  The action is in another module.  Move
			 *  it to the project
			 */
			if (!obj_is_project(action->parent))
			{
				obj_unparent(action);
				obj_add_action(project, action);
			}
		}
	}
	else
	{
		/* insert it into the current module */
		obj_add_action(module, action);
	}

	return 0;
}


#ifdef BOGUS
/*
 * Finds the target matching the given description, creating it if necessary.
 *
 * Assumes: strings are pointers to allocated space. Sets strings to NULL,
 *          if the values are used.
 *
 * Assumes that obj may not be in the object tree yet, and may return
 *         it as the target.
 */
static ABObj
find_or_create_target(ABObj obj, ABObj module, 
			ISTRING parent_name, 
			ISTRING obj_name )
{
	STRING	target_parent_name[256];
	STRING	target_name[256];
	ABObj	target_parent= NULL;
	ABObj	target= NULL;

	/* util_dprintf(3, "find_or_create_target('%s' '%s' '%s')\n",
			nullstr(parent_name),
			nullstr(obj_name), nullstr(item_label)); */

	*target_parent_name= 0;
	*target_name= 0;
	if (obj_name == NULL)
	{
		return NULL;
	}
	if (parent_name != NULL)
	{
		/* we have parent name and object name */
		strcpy((STRING)target_parent_name, istr_string(parent_name));
		strcpy((STRING)target_name, istr_string(obj_name));
	}
	else
	{
		/* object name only (what a concept!) */
		strcpy((STRING)target_name, istr_string(obj_name));
	}

	/*
	 * Find target parent
	 */
	if (*target_parent_name == 0)
	{
		target_parent= module;
	}
	else
	{
		if (istr_equalstr(obj->name, (STRING)target_parent_name))
		{
			target_parent= obj;
		}
		else
		{
			target_parent= obj_scoped_find_or_create_undef(module, 
					(STRING)target_parent_name,
					AB_TYPE_UNKNOWN);
		}
	}

	/*
	 * Find target
	 */
	if (istr_equalstr(obj->name, (STRING)target_name))
	{
		target= obj;
	}
	else
	{
		target= obj_scoped_find_or_create_undef(target_parent, 
				(STRING)target_name, AB_TYPE_UNKNOWN);
	}
	return target;
}
#endif /* BOGUS */

STRING
bilP_load_att_arg_value(BIL_TOKEN valueToken)
{
    STRING	stringValue = bilP_load_get_value();

#ifdef DEBUG
    util_dprintf(3, "bilP_load_att_arg_value: %d/%s\n",
                valueToken,
                str_safe(bilP_token_to_string(valueToken)));
#endif

    nset_att(AB_BIL_ARG_VALUE);

    /* Check if arg is an integer */
    if (valueToken == AB_BIL_VALUE_INT) 
    { 
#ifdef DEBUG
        util_dprintf(0, "\targ is an int = %d\n", atoi(stringValue));
#endif
	obj_set_arg_int(bilP_load.obj, atoi(stringValue));
    }
    /* Check if arg is a float */
    else
        if (valueToken == AB_BIL_VALUE_FLOAT)
        {
	    float	fval;
	    fval = (float) atof(stringValue);
#ifdef DEBUG
util_dprintf(3, "\targ is a float = %f\n", fval);
#endif
	    obj_set_arg_float(bilP_load.obj, fval);
        }
        else
            if (valueToken == AB_BIL_VALUE_STRING)
            {
    		ISTRING     istringValue = bilP_get_string();
#ifdef DEBUG
util_dprintf(3, "\targ is an istring = %s\n", istr_string(istringValue));
#endif
		obj_set_arg_string(bilP_load.obj, stringValue);
            }
    return NULL;
}
