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
 * $XConsortium: gil_loadact.c /main/3 1995/11/06 18:27:51 rswiston $
 * 
 * @(#)gil_loadact.c    1.20 13 Feb 1994        cde_app_builder/src/libABil
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
 * loadact.c - load action and handler attributes
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <assert.h>
#include <ab_private/obj.h>
#include <ab_private/abio.h>
#include <ab_private/trav.h>
#include <ab_private/util.h>
#include <ab_private/istr.h>
#include "../libABobj/obj_utils.h"
#include "gil.h"
#include "gilP.h"
#include "load.h"
#include "loadP.h"
#include "gil_loadattP.h"

static int	get_action(
			FILE * inFile, 
			ABObj obj, 
			ABObj module,
			ABObj action
		);
static int	get_action_attribute(
			FILE * inFile, 
			ABObj obj, 
			ABObj module,
                     	ABObj action,
                     	AB_GIL_ATTRIBUTE attr
		);
static int	get_when(
			FILE * inFile,
         		ABObj obj, 
			ABObj action
		);
static int	get_from(
			FILE * inFile, 
			ABObj obj, 
			ABObj module, 
			ABObj action
		);
static int	get_to(
			FILE * inFile, 
			ABObj obj, 
			ABObj module, 
			ABObj action
		);
static int          get_operation(FILE * inFile, ABObj action);
static int          get_function_type(FILE * inFile, ABObj action);
static int          get_arg_type(FILE * inFile, ABObj action);
static int	add_user_handler(ABObj obj, ABObj module,
                 	ISTRING handler, AB_WHEN when);
static int	install_action(
			ABObj obj, 
			ABObj module, 
			ABObj action
		);
static ABObj	find_or_create_target(
			ABObj	obj, 
			ABObj	module,
		      	ISTRING	interface_name,
                      	ISTRING	parent_name,
                      	ISTRING	obj_name,
                      	ISTRING	item_label
		);

/*************************************************************************
**                                                                      **
**              Specific handlers                                       **
**                                                                      **
**************************************************************************/

/*
 * */
int
gilP_load_att_done_handler(FILE * inFile, ABObj obj, ABObj module)
{
    int                 return_value = 0;
    int                 rc = 0; /* r turn code */
    ISTRING             handler = NULL;
    ABObj               action = NULL;

    if ((rc = gilP_load_handler(inFile, &handler)) < 0)
    {
        return rc;
    }

    /*
     * I don't know what this a DONE when signifies. It doesn't exist in BIL
     * add_user_handler(obj, module, handler, AB_WHEN_DONE);
     */
    abil_print_load_err(ERR_NOT_IMPL);

    istr_destroy(handler);
    return return_value;
}

int
gilP_load_att_event_handler(FILE * inFile, ABObj obj, ABObj module)
{
    int                 return_value = 0;
    int                 rc = 0; /* r turn code */
    ISTRING             event_handler = NULL;

    if ((rc = gilP_load_handler(inFile, &event_handler)) < 0)
    {
        istr_destroy(event_handler);
        return rc;
    }

    /*
     * Doesn't exist in BIL add_user_handler(obj, module, event_handler,
     * AB_WHEN_ANY_EVENT);
     */
    abil_print_load_err(ERR_NOT_IMPL);
    istr_destroy(event_handler);
    return return_value;
}

/*
 * */
int
gilP_load_att_menu_handler(FILE * inFile, ABObj obj, ABObj module)
{
    int                 return_value = 0;
    int                 rc = 0; /* r turn code */
    ISTRING             handler = NULL;

    if ((rc = gilP_load_handler(inFile, &handler)) < 0)
    {
        return rc;
    }
    add_user_handler(obj, module, handler, AB_WHEN_ACTIVATED);
    istr_destroy(handler);
    return return_value;
}

/*
 * */
int
gilP_load_att_menu_item_handlers(FILE * inFile, ABObj obj, ABObj module)
{
    int                 return_value = 0;
    int                 rc = 0; /* r turn code */
    ISTRING_ARRAY       handlers;
    AB_TRAVERSAL        trav;
    ABObj               item = NULL;
    int                 i = 0;

    istr_array_init(&handlers);
    if ((rc = gilP_load_handlers(inFile, &handlers)) < 0)
    {
        return rc;
    }
    obj_ensure_num_children(obj, handlers.count);
    for (trav_open(&trav, obj, AB_TRAV_ITEMS_FOR_OBJ), i = 0;
         (item = trav_next(&trav)) != NULL; ++i)
    {
        add_user_handler(item, module,
                         handlers.strings[i], AB_WHEN_ACTIVATED);
        handlers.strings[i] = NULL;
    }
    trav_close(&trav);
    istr_array_uninit(&handlers);
    return return_value;
}

/*
 * */
int
gilP_load_att_notify_handler(FILE * inFile, ABObj obj, ABObj module)
{
    int                 return_value = 0;
    int                 rc = 0; /* r turn code */
    ISTRING             handler = NULL;

    if ((rc = gilP_load_handler(inFile, &handler)) < 0)
    {
        return rc;
    }
    add_user_handler(obj, module, handler, AB_WHEN_ACTIVATED);

    istr_destroy(handler);
    return return_value;
}

/*
 * */
int
gilP_load_att_repaint_proc(FILE * inFile, ABObj obj, ABObj module)
{
    int                 return_value = 0;
    int                 rc = 0; /* r turn code */
    ISTRING             repaint = NULL;

    if ((rc = gilP_load_handler(inFile, &repaint)) < 0)
    {
        return rc;
    }
    add_user_handler(obj, module, repaint, AB_WHEN_REPAINT_NEEDED);

    istr_destroy(repaint);
    return return_value;
}


/*************************************************************************
**                                                                      **
**                                                                      **
**              General actions                                         **
**                                                                      **
**                                                                      **
**************************************************************************/


/*
 * Load the actions attributes
 */
int
gilP_load_att_actions(FILE * inFile, ABObj obj, ABObj module)
{
    int                 return_value = 0;
    int                 rc = 0; /* r turn code */
    ABObj               action = NULL;

    if (!abio_get_list_begin(inFile))
        return (abil_print_load_err(ERR_WANT_LIST), -1);

    while (!abio_get_list_end(inFile))
    {
        action = obj_create(AB_TYPE_ACTION, NULL);

        if ((rc = get_action(inFile, obj, module, action)) < 0)
        {
            obj_destroy(action);
            return rc;
        }
        else
        {
	    if (obj_get_when(action) == AB_WHEN_UNDEF)
	    {
		/* unsupported :when value - ignore it and continue */
		obj_destroy(action);
	    }
	    else
	    {
                install_action(obj, module, action);
	    }
        }
    }
    return return_value;
}


/*
 * Load an action.
 */
static int
get_action(
			FILE * inFile, 
			ABObj obj, 
			ABObj module, 
			ABObj action
)
{
    int                 return_value = 0;
    int                 rc = 0; /* r turn code */
    ISTRING             keyword;
    AB_GIL_ATTRIBUTE    action_attr;

    if (!abio_get_list_begin(inFile))
        return (abil_print_load_err(ERR_WANT_LIST), -1);

    while (!abio_get_list_end(inFile))
    {
        if (abio_get_eof(inFile))
        {
            abil_print_load_err(ERR_EOF);
            return -1;
        }

        if (!abio_get_keyword(inFile, &keyword))
        {
            abil_print_load_err(ERR_WANT_KEYWORD);
            return -1;
        }

        action_attr = gilP_string_to_att(istr_string(keyword));
        if ((rc = get_action_attribute(inFile, obj, module,
                                       action, action_attr))
            < 0)
        {
            return_value = rc;
            break;
        }
        abil_loadmsg_set_action_att(NULL);
    }
    abil_loadmsg_set_action_att(NULL);

    return return_value;
}


/*
 * Load one action attribute.
 */
static int
get_action_attribute(
			FILE * inFile, 
			ABObj obj,
			ABObj module,
                     	ABObj action,
                     	AB_GIL_ATTRIBUTE attr
)
{
    int                 return_value = 0;
    int                 rc = 0; /* r turn code */

    if (abio_get_eof(inFile))
    {
        return (abil_print_load_err(ERR_EOF), -1);
    }

    switch (attr)
    {
    case AB_GIL_FROM:
        rc = get_from(inFile, obj, module, action);
        break;
    case AB_GIL_ACTION:
        rc = get_operation(inFile, action);
        break;
    case AB_GIL_TO:
        rc = get_to(inFile, obj, module, action);
        break;
    case AB_GIL_WHEN:
        rc = get_when(inFile, obj, action);
        break;
    case AB_GIL_FUNCTION_TYPE:
        rc = get_function_type(inFile, action);
        break;
    case AB_GIL_ARG_TYPE:
        rc = get_arg_type(inFile, action);
        break;
    default:
        abil_print_load_err(ERR_UNKNOWN);
    }

    return return_value;
}

/*
 * Get the source object.
 */
static int
get_from(FILE * inFile, ABObj obj, ABObj module, ABObj action)
{
    int                 return_value = 0;
    int                 rc = 0; /* r turn code */
    ISTRING             interface_name = NULL;
    ISTRING             parent_name = NULL;
    ISTRING		name = NULL;
    ISTRING		item_name = NULL;
    ABObj               from = NULL;

    if (   (abio_gil_get_full_name(inFile, 
		&interface_name, &parent_name, &name, &item_name) < 0)
	|| (name == NULL) )
    {
        abil_print_load_err(ERR_WANT_FULL_NAME);
	return_value = -1;
        goto abort;
    }

    from = find_or_create_target(obj, module,
           		interface_name, parent_name, name, item_name);

    if (from == NULL)
    {
        rc = abil_print_load_err(ERR_UNKNOWN_OBJECT);
        goto abort;
    }

    obj_set_from(action, from);

abort:
    istr_destroy(parent_name);
    istr_destroy(name);
    istr_destroy(item_name);
    return return_value;
}


/*
 * Get the destination object.
 */
static int
get_to(FILE * inFile, ABObj obj, ABObj module, ABObj action)
{
    int                 return_value = 0;
    int                 rc = 0; /* r turn code */
    ISTRING             interface_name = NULL;
    ISTRING             parent_name = NULL;
    ISTRING             name = NULL;
    ISTRING             item_name = NULL;
    ABObj               to = NULL;

    if (   (abio_gil_get_full_name(inFile,
                  &interface_name, &parent_name, &name, &item_name) < 0)
	|| (name == NULL) )
    {
        abil_print_load_err(ERR_WANT_FULL_NAME);
	return_value = -1;
        goto abort;
    }

    to = find_or_create_target(obj, module, 
			interface_name, parent_name, name, item_name);

    if (to == NULL)
    {
        rc = abil_print_load_err(ERR_UNKNOWN_OBJECT);
        goto abort;
    }

    action->info.action.to = to;

abort:
    istr_destroy(parent_name);
    istr_destroy(name);
    istr_destroy(item_name);
    return return_value;
}


/*
 * Get the operation.
 */
static int
get_operation(FILE * inFile, ABObj action)
{
    int                 return_value = 0;
    int                 rc = 0; /* r turn code */
    ISTRING             string = NULL;
    ISTRING             name = NULL;
    int                 intval;

    if (!abio_get_list_begin(inFile))
    {
        return (abil_print_load_err(ERR_WANT_LIST), -1);
    }

    switch (obj_get_func_type(action))
    {
    case AB_FUNC_CODE_FRAG:     /* code fragment */
        if (!abio_get_string(inFile, &string))
        {
            rc = abil_print_load_err(ERR_WANT_STRING);
            goto abort;
        }
        obj_set_func_code(action, istr_string(string));
        break;

    case AB_FUNC_USER_DEF:      /* function name (user defined) */
        if (!abio_gil_get_name(inFile, &name))
        {
            rc = abil_print_load_err(ERR_WANT_NAME);
            goto abort;
        }
        obj_set_func_name(action, istr_string(name));
        break;

    case AB_FUNC_BUILTIN:       /* predefined function */
        if (!abio_gil_get_name(inFile, &name))
        {
            return (abil_print_load_err(ERR_WANT_NAME), -1);
        }
        obj_set_func_builtin(action,
                          gilP_string_to_builtin_action(istr_string(name)));

        if (obj_get_func_builtin(action) != AB_STDACT_UNDEF)
        {
            if (abio_get_list_end(inFile))
                return return_value;

            switch (obj_get_arg_type(action))
            {
            case AB_ARG_STRING:
                if (!abio_get_string(inFile, &string))
                    return (abil_print_load_err(ERR_WANT_STRING), -1);
                obj_set_arg_string(action, istr_string(string));
                break;
            case AB_ARG_INT:
                if (!abio_get_integer(inFile, &intval))
                    return (abil_print_load_err(ERR_WANT_INTEGER), -1);
                obj_set_arg_int(action, intval);
                break;
            case AB_ARG_FLOAT:
                rc = abil_print_load_err(ERR_UNKNOWN);
                break;
            default:
                rc = abil_print_load_err(ERR_WANT_ARG);
                break;
            }
        }

        break;

    default:
        break;
    }

    if (rc < 0)
    {
        return_value = rc;
    }
    else
    {
        if (!abio_get_list_end(inFile))
        {
            return (abil_print_load_err(ERR_WANT_LIST), -1);
        }
    }

abort:
    istr_destroy(string);
    istr_destroy(name);
    return return_value;
}

/*
 * Get the when part of a connection.
 */
static int
get_when(FILE * inFile, ABObj obj, ABObj action)
{
    int                 return_value = 0;
    ISTRING             string = NULL;
    AB_WHEN             when = AB_WHEN_UNDEF;
    int			numWhens = 0;

    if (!abio_get_list_begin(inFile))
        return (abil_print_load_err(ERR_WANT_LIST), -1);

    while (!abio_get_list_end(inFile))
    {
        if (!abio_gil_get_name(inFile, &string))
            return (abil_print_load_err(ERR_WANT_NAME), -1);

        when = gilP_string_to_when(istr_string(string));
	if (when == AB_WHEN_UNDEF)
	{
	    if (util_be_verbose())
	    {
	        util_printf("WARNING: ignoring unsupported when: %s\n",
		    istr_string(string));
	    }
	}
	else
	{
	    obj_set_when(action, when);
            istr_destroy(string);
	    ++numWhens;
	    if (numWhens > 1)
	    {
	        util_printf_err(
		    "WARNING: multiple :when values not supported. "
		    "Using first value\n");
	    }
	}
    }
    return return_value;
}

/*
 * Get the function_type for an action holder
 */
static int
get_function_type(FILE * inFile, ABObj action)
{
    int                 return_value = 0;
    ISTRING             type_name = NULL;

    if (!abio_gil_get_name(inFile, &type_name))
    {
        return (abil_print_load_err(ERR_WANT_KEYWORD), -1);
    }

    if (istr_equalstr(type_name, "ExecuteCode"))
    {
        obj_set_func_type(action, AB_FUNC_CODE_FRAG);
    }
    else if (istr_equalstr(type_name, ":user_defined"))
    {
	/* yes, :user_defined in GIL means a builtin action */
        obj_set_func_type(action, AB_FUNC_BUILTIN);
    }
    else if (istr_equalstr(type_name, "CallFunction"))
    {
        obj_set_func_type(action, AB_FUNC_USER_DEF);
    }
    else
    {
        return (abil_print_load_err(ERR_WANT_KEYWORD), -1);
    }

    istr_destroy(type_name);
    return return_value;
}


/*
 * Get the arg_type of an action_holder
 */
static int
get_arg_type(FILE * inFile, ABObj action)
{
    int                 return_value = 0;
    ISTRING             type_name = NULL;

    obj_set_arg_type(action, AB_ARG_VOID_PTR);

    if (!abio_get_list_begin(inFile))
        return (abil_print_load_err(ERR_WANT_LIST), -1);

    if (abio_get_list_end(inFile))
    {
        return return_value;
    }

    while (!abio_get_list_end(inFile))
    {
        if (!abio_gil_get_name(inFile, &type_name))
            return (abil_print_load_err(ERR_WANT_NAME), -1);

        obj_set_arg_type(action, 
		gilP_string_to_arg_type(istr_string(type_name)));
    }

    return return_value;
}


static int
add_user_handler(ABObj obj, ABObj module,
                 ISTRING handler, AB_WHEN when)
{
    int                 retval = 0;
    ABObj               project = NULL;
    ABObj               action = NULL;

    if (handler == NULL)
    {
        return 0;
    }
    project = obj_get_project(module);

    /*
     * With "callbacks," the to field is irrelevant.  The target is generally
     * whatever widget calls the callback, so we set the "to" field to NULL,
     * so that this action may be used by multiple widgets.  I.e., if the
     * function type, name and to fields match, an action will be shared.
     */
    action = obj_create(AB_TYPE_ACTION, NULL);
    obj_set_func_type(action, AB_FUNC_USER_DEF);
    obj_set_func_name(action, istr_string(handler));
    obj_set_arg_type(action, AB_ARG_VOID_PTR);
    action->info.action.to = NULL;

    obj_set_from(action, obj);
    obj_set_when(action, when);

    install_action(obj, module, action);
    return 0;
}


static int
install_action(ABObj obj, ABObj module, ABObj action)
{
    int			return_value = 0;

    /*
     * Cross-module connections sit under the project
     * intra-module connectoins sit under the module
     */
    return_value = obj_add_action(module, action);

    return return_value;
}


/*
 * Finds the target matching the given description, creating it if necessary.
 * 
 * Assumes: strings are pointers to allocated space. Sets strings to NULL, if
 * the values are used.
 * 
 * Assumes that obj may not be in the object tree yet, and may return it as the
 * target.
 */
static ABObj
find_or_create_target(
			ABObj	obj, 
			ABObj	module,
		      	ISTRING	interface_name,
                      	ISTRING	parent_name,
                      	ISTRING	obj_name,
                      	ISTRING	item_label
)
{
    ABObj               target = NULL;	/* the real thing, baby! */
    ABObj		target_project = NULL;
    char		target_interface_file[MAXPATHLEN+1];
    char		target_interface_name[GIL_MAX_NAME_SIZE];
    ABObj		target_module = NULL;
    char		target_parent_name[GIL_MAX_NAME_SIZE];
    ABObj		target_parent = NULL;
    char		target_obj_name[GIL_MAX_NAME_SIZE];
    ABObj		target_obj = NULL;
    char		target_item_label[GIL_MAX_NAME_SIZE];
    ABObj		target_item = NULL;
    AB_TRAVERSAL	trav;
    *target_interface_file = 0;
    *target_interface_name = 0;
    *target_parent_name = 0;
    *target_obj_name = 0;
    *target_item_label = 0;
   
    /* must have object name */
    if (debugging()) 
    {
	assert(   (obj_name != NULL)
	       && (obj_is_project(module) || obj_is_module(module)));
    }

    if (obj_is_project(module))
    {
	/* The first string must be an interface file name */
	if (interface_name != NULL)
	{
	    util_strncpy(target_interface_file, istr_string(interface_name),
			GIL_MAX_NAME_SIZE);
	    if (parent_name != NULL)
	    {
		util_strncpy(target_parent_name, istr_string(parent_name),
			GIL_MAX_NAME_SIZE);
	    }
	}
	else if (parent_name != NULL)
	{
	    util_strncpy(target_interface_file, istr_string(parent_name),
			GIL_MAX_NAME_SIZE);
	}
	else
	{
	    abil_print_load_err(ERR_WANT_FULL_NAME);
	    goto epilogue;
	}

	/* derive the name from the file name */
	strcpy(target_interface_name, target_interface_file);
	{
	    int		len = strlen(target_interface_name);
	    if (    (len >= 2) 
		&& (   util_streq(&(target_interface_name[len-2]), ".G")
		    || util_streq(&(target_interface_name[len-2]), ".P")))
	    {
		target_interface_name[len-2] = 0;
	    }
	}

	util_strncpy(target_obj_name, istr_string(obj_name), 
			GIL_MAX_NAME_SIZE);
	if (item_label != NULL)
	{
	    util_strncpy(target_item_label, istr_string(item_label), 
			GIL_MAX_NAME_SIZE);
	}
    }
    else 	/* ! obj_is_project() */
    {
        if (parent_name != NULL)
        {
            /* we have parent name and object name */
            util_strncpy(target_parent_name, istr_string(parent_name), 
			GIL_MAX_NAME_SIZE);
        }

	util_strncpy(target_obj_name, istr_string(obj_name), 
			GIL_MAX_NAME_SIZE);
        if (item_label != NULL)
        {
            /* we have object name and item_label */
            util_strncpy(target_item_label, istr_string(item_label),
		GIL_MAX_NAME_SIZE);
        }
     }

     /*
      * We've got the name broken down into the appropriate pieces.
      * Now find the actual target.
      */
    /*util_dprintf(3, "finding: module:'%s' parent:'%s' obj:'%s' item:'%s'\n",
	target_interface_name, target_parent_name, 
	target_obj_name, target_item_label);*/
    

    /*
     * Find target project
     */
    target_project = obj_get_project(module);

    /*
     * Find target module
     */
    if (util_strempty(target_interface_name))
    {
	target_module = module;
    }
    else
    {
	/* find specified intefarce (module) */
	for (trav_open(&trav, target_project, AB_TRAV_MODULES);
		(target_module = trav_next(&trav)) != NULL; )
	{
	    if (   (target_module != target_project)
		&& (util_streq(
			obj_get_file(target_module), target_interface_file)))
	    {
		break;
	    }
	}
	trav_close(&trav);
	if (target_module == NULL)
	{
	    target_module = obj_create(AB_TYPE_MODULE, target_project);
	    obj_set_is_defined(target_module, FALSE);
	    obj_set_file(target_module, target_interface_file);
	    obj_set_name(target_module, target_interface_name);
	}
    }

    /*
     * Find target parent
     */
    if (util_strempty(target_parent_name))
    {
	target_parent = target_module;
    }
    else
    {
	for (trav_open(&trav, target_module, 
			AB_TRAV_ALL | AB_TRAV_MOD_PARENTS_FIRST);
	    	(target_parent = trav_next(&trav)) != NULL; )
	{
	    if (   (target_parent != target_module)
		&& (util_streq(
			obj_get_name(target_parent), target_parent_name)))
	    {
		break;
	    }
	}
	trav_close(&trav);
	if (target_parent == NULL)
	{
	    target_parent = obj_create(AB_TYPE_UNDEF, target_module);
	    obj_set_is_defined(target_parent, FALSE);
	    obj_set_file(target_parent, target_interface_file);
	    obj_set_name(target_parent, target_parent_name);
	}
    }

    /*
     * Find target obj
     */
    for (trav_open(&trav, target_parent,
			AB_TRAV_ALL | AB_TRAV_MOD_PARENTS_FIRST);
		(target_obj = trav_next(&trav)) != NULL; )
    {
	if (   (target_obj != target_parent)
	   && util_streq(target_obj_name, obj_get_name(target_obj)))
	{
	    break;
	}
    }
    trav_close(&trav);
    if (target_obj == NULL)
    {
        target_obj = obj_create(AB_TYPE_UNDEF, target_parent);
        obj_set_is_defined(target_obj, FALSE);
        obj_set_file(target_obj, target_interface_file);
        obj_set_name(target_obj, target_obj_name);
    }

    /*
     * Find item
     */
    if (util_strempty(target_item_label))
    {
	target_item = NULL;
    }
    else
    {
	for (trav_open(&trav, target_obj, AB_TRAV_ITEMS);
		(target_item = trav_next(&trav)) != NULL; )
	{
	    if (   (target_item != target_obj)
		&& util_streq(obj_get_label(target_item), target_item_label))
	    {
		break;
	    }
	}
	trav_close(&trav);
	if (target_item == NULL)
	{
	    target_item = obj_create(AB_TYPE_ITEM, target_obj);
	    obj_set_is_defined(target_item, FALSE);
	    obj_set_file(target_item, target_interface_file);
	    obj_set_label(target_item, target_item_label);
	    obj_set_name_from_label(target_item, 
				obj_get_name(obj_get_parent(target_item)));
	}
    }

    if (target_item != NULL)
    {
	target = target_item;
    }
    else
    {
	target = target_obj;
    }

epilogue:
    return target;
}

