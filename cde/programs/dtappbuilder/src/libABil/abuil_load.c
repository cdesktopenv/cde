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
 *	$XConsortium: abuil_load.c /main/6 1996/10/17 16:20:11 drk $
 *
 * @(#)abuil_load.c	1.38 31 May 1994	cose/unity1/cde_app_builder/src/libABil
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
 * This file contains the implementation of the uil load
 * component
 */
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>

#include <ab_private/util_ds.h>

#include "abuil_loadP.h"

/*
 * There is no public header file for this function (only an
 * internal header XmStringI.h).
 */
extern XtPointer _XmStringUngenerate (XmString string,
                        XmStringTag tag,
                        XmTextType tag_type,
                        XmTextType output_type);


typedef Uil_continue_type(*UIL_CB)();


/*
 * Private function declarations
 */

static ABObjPtr		uil_tree_into_ab_project(
			    sym_entry_type	*uil_parse_tree,
			    ABObjPtr		ab_project
			);
static ABObjPtr		root_into_project(
			    sym_entry_type	*uil_root,
			    ABObjPtr		ab_project
			);
static ABObjPtr		module_hdr_to_module(
			    sym_entry_type	*uil_module_hdr
			);
static ABObjPtr		sections_into_module(
			    ABObjPtr		ab_module,
			    sym_entry_type	*uil_sections
			);
static ABObjPtr		object_into_module(
			    ABObjPtr		ab_module,
			    sym_entry_type	*uil_section_entries
			);
static ABObjPtr		object_child_into_module(
			    ABObjPtr		ab_module,
			    sym_entry_type	*uil_section_entries
			);
static ABObjPtr		widget_into_module(
			    ABObjPtr		ab_module,
			    ABObjPtr		ab_parent,
			    sym_widget_entry_type *uil_widget
			);
static ABObjPtr		gadget_into_module(
			    ABObjPtr		ab_module,
			    ABObjPtr		ab_parent,
			    sym_widget_entry_type *uil_gadget
			);
static ABObjPtr		arguments_into_ab_widget(
			    ABObjPtr		ab_widget,
			    sym_list_entry_type *uil_widget_arguments
			);
static ABObjPtr		callbacks_into_ab_module(
			    ABObjPtr		ab_widget,
			    sym_list_entry_type *uil_widget_callbacks
			);
static STRING		ab_name_for_uil_widget(
			    sym_widget_entry_type *uil_widget
			);
static STRING		resource_name_for_uil_arg(
			    sym_argument_entry_type *uil_arg
			);
static XtPointer 	resource_value_for_uil_arg(
			    sym_argument_entry_type *uil_arg,
			    AB_ARG_TYPE		*p_res_type
			);
static STRING		*ab_action_handler_list_for_uil_callback(
			    sym_callback_entry_type *uil_cb,
			    AB_WHEN		*p_ab_action_when
			);
static sym_entry_type	*parse_uil(
			    char		*uil_file_name
			);
static Uil_continue_type	message_cb(
			    int			*message_user_data,
			    int			msg_number,
			    int			msg_severity,
			    char		*msg_text,
			    char		*src_text,
			    char		*ptr_text,
			    char		*loc_text,
			    int			message_count[]
			);
static Uil_continue_type	status_cb(
			    char		*status_user_data,
			    int			percent_complete,
			    int			lines_processed,
			    char		*current_file,
			    int			message_count[]
			);
static void		init_sym_stacks(
			    void
			);
static void		cleanup_sym_stacks(
			    void
			);
static void		sym_stack_push(
			    void
			);
static void		sym_stack_pop(
			    void
			);
static void		uil_sym_push(
			    sym_entry_type	*sym_entry
			);
static sym_entry_type *	uil_sym_pop(
			    void
			);

static void		init_unresolved_refs(
			    void
			);
static void		resolve_unresolved_refs(
			    void
			);
static void		object_for_widget(
			    void	*client_data
			);

static void		free_uil_tree(
			    sym_entry_type	*node_entry
			);
/*
 * Recursively goes through the uil parse tree, freeing each node
 * accessible from the root node in postfix order.
 */
static void
free_uil_node(
    sym_entry_type		*node_entry
)
{
    sym_value_entry_type	*val_node;
    sym_widget_entry_type	*widget_node;
    sym_module_entry_type	*module_node;
    sym_list_entry_type		*list_node;
    sym_obj_entry_type		*list_entry;
    sym_root_entry_type		*root_node;
    sym_include_file_entry_type	*ifile_node;
    sym_section_entry_type	*section_node;


    /*
     * No action on null node. Else freeing is based on the kind
     * of the current node.
     */
    if (node_entry == NULL)
	return;
    switch (node_entry->header.b_tag)
    {
      case sym_k_value_entry:
	val_node = (sym_value_entry_type *) node_entry;
	free_uil_node((sym_entry_type *) val_node->az_charset_value);
	free_uil_node((sym_entry_type *) val_node->az_first_table_value);
	free_uil_node((sym_entry_type *) val_node->az_next_table_value);
	free_uil_node((sym_entry_type *) val_node->az_exp_op1);
	free_uil_node((sym_entry_type *) val_node->az_exp_op2);
	break;

      case sym_k_widget_entry:
      case sym_k_gadget_entry:
      case sym_k_child_entry:
	widget_node = (sym_widget_entry_type *) node_entry;
	free_uil_node((sym_entry_type *) widget_node->az_callbacks);
	free_uil_node((sym_entry_type *) widget_node->az_arguments);
	free_uil_node((sym_entry_type *) widget_node->az_controls);
	free_uil_node((sym_entry_type *) widget_node->az_create_proc);
	break;

      case sym_k_module_entry:
	module_node = (sym_module_entry_type *) node_entry;
	free_uil_node((sym_entry_type *) module_node->az_version);
	free_uil_node((sym_entry_type *) module_node->az_character_set);
	free_uil_node((sym_entry_type *) module_node->az_case_sense);
	free_uil_node((sym_entry_type *) module_node->az_def_obj);
	break;

      case sym_k_list_entry:
	list_node = (sym_list_entry_type *) node_entry;
	for (list_entry = (sym_obj_entry_type *)list_node->obj_header.az_next;
	     list_entry != NULL;
	    )
	{
	    list_entry = (sym_obj_entry_type *)list_entry->obj_header.az_next;

	    free_uil_node((sym_entry_type *) list_entry);
	}
	break;

      case sym_k_root_entry:
	root_node = (sym_root_entry_type *) node_entry;
	free_uil_node((sym_entry_type *) root_node->module_hdr);
	free_uil_node((sym_entry_type *) root_node->sections);
	break;

      case sym_k_include_file_entry:
	ifile_node = (sym_include_file_entry_type *) node_entry;
	free_uil_node((sym_entry_type *) ifile_node->sections);
	break;

      case sym_k_section_entry:
	section_node = (sym_section_entry_type *) node_entry;
	switch (section_node->header.b_type)
	{
	  case sym_k_section_tail:
	    break;

	  default:
	    free_uil_node((sym_entry_type *) section_node->entries);
	    free_uil_node((sym_entry_type *) section_node->next);
	    break;
	}
	break;
    }
    XtFree((char *)node_entry);
}
/* ----------------------------------------------------------------------
 * abuil_get_uil_file:
 *
 * This function parses a UIL file specified by uil_file_path
 * using the standard callable UIL compiler and constructs a
 * ABObj tree rooted at a AB_TYPE_MODULE after adding it to
 * the supplied AB_TYPE_PROJECT, ab_project.
 * Returns NULL in case of error; otherwise returns the AB_TYPE_MODULE
 * object.
 */
extern ABObjPtr
abuil_get_uil_file(
    STRING	uil_file_path,
    ABObjPtr	ab_project
)
{
    sym_entry_type	*uil_parse_tree;
    ABObj		ab_module = NULL;

    uil_parse_tree = parse_uil(uil_file_path);

    if (uil_parse_tree != NULL)
    {

	if (util_get_verbosity() > 3)
	{
	    abuil_trav_uil_tree((void *) uil_parse_tree);
	}

	init_sym_stacks();
	init_unresolved_refs();
	ab_module = uil_tree_into_ab_project(uil_parse_tree, ab_project);
	resolve_unresolved_refs();
	cleanup_sym_stacks();
	free_uil_tree(uil_parse_tree);
    }

    if (util_get_verbosity() > 3)
    {
	obj_tree_print(ab_module);
    }
    return(ab_module);
}


/*
 * This function goes through the uil parse tree (symbol table),and
 * recursively constructs a ABOBJ tree rooted in a project, which it then
 * returns. Returns NULL if there is an error.
 */
static ABObjPtr
uil_tree_into_ab_project(
    sym_entry_type	*uil_parse_tree,
    ABObjPtr		ab_project
)
{
    return(root_into_project(uil_parse_tree, ab_project));
}

/*
 * Root into project
 */
static ABObjPtr
root_into_project(
    sym_entry_type	*uil_root,
    ABObjPtr		ab_project
)
{
    sym_root_entry_type	*root_node;
    ABObjPtr		ab_module;
    ABObjPtr		ret_val;

    if (uil_root == (sym_entry_type *)NULL ||
	uil_root->header.b_tag != sym_k_root_entry)
	return((ABObjPtr)NULL);
    else
	root_node = (sym_root_entry_type *)uil_root;

    if (ab_project != NULL && obj_is_project(ab_project))
    {
	/*
	 * REMIND: populate project;
	 *	 use fields in uil_root
	 */
	ab_project->info.project.is_default = TRUE;
	/*
	obj_set_name(ab_project, strdup(ab_ident_from_file_name(root_node->file_name)));
	 */

	ab_module = module_hdr_to_module((sym_entry_type *)root_node->module_hdr);

	if (ab_module != NULL)
	{
	    if (sections_into_module(ab_module,
		    (sym_entry_type *)root_node->sections) != NULL)
	    {
		obj_append_child(ab_project, ab_module);
		ret_val = ab_module;
	    }
	    else
	    {
		obj_destroy(ab_module);
		ret_val = (ABObjPtr)NULL;
	    }
	}
	else
	    ret_val = (ABObjPtr)NULL;
    }
    return(ret_val);
}

static ABObjPtr
module_hdr_to_module(
    sym_entry_type	*uil_module_hdr
)
{
    ABObjPtr			ab_module;
    sym_module_entry_type	*module_node;

    if (uil_module_hdr == (sym_entry_type *)NULL ||
	uil_module_hdr->header.b_tag != sym_k_module_entry)
	return((ABObjPtr)NULL);
    else
	module_node = (sym_module_entry_type *)uil_module_hdr;

    ab_module = obj_create(AB_TYPE_MODULE, NULL);
    if (ab_module != NULL)
    {
	/*
	 * REMIND: populate module;
	 *       traverse module header
	 */
	obj_set_name(ab_module, strdup(ab_ident_from_file_name(
		  (char *)module_node->obj_header.az_name->c_text)));
    }
    return(ab_module);
}

/*
 * The merge-in routines.
 */

/*
 * Pre-condition:	ab_module != NULL
 */
static ABObjPtr
sections_into_module(
    ABObjPtr		ab_module,
    sym_entry_type	*uil_sections
)
{
    sym_entry_type		*next_node;
    sym_section_entry_type	*section_node;
    short int			done;
    ABObjPtr			ret_val;

    /*
     * populate module;
     *       traverse tail terminated
     *	     list of sections.
     */
    next_node = uil_sections;

    sym_stack_push();
    done = 0;
    while (!done)
    {
	if (next_node == (sym_entry_type *)NULL ||
	    next_node->header.b_tag != sym_k_section_entry)
	{
	    return((ABObjPtr)NULL);
	}
	else
	    section_node = (sym_section_entry_type *)next_node;

	switch (section_node->header.b_type)
	{
	  case sym_k_section_tail:
	    done = 1;
	    break;
	  case sym_k_object_section:
	  case 0:
	    uil_sym_push((sym_entry_type *)section_node);
	    break;
	  default:
	    break;
	}
	next_node = section_node->next;
    }

    done = 0;
    while (!done)
    {
	if ((section_node = (sym_section_entry_type *)uil_sym_pop()) == NULL)
	    break;

	switch (section_node->header.b_type)
	{
	  case sym_k_object_section:
	    if ((ret_val = object_into_module(ab_module,
				section_node->entries)) == NULL)
		done = 1;
	    break;
	  case 0:
	    if ((ret_val = object_child_into_module(ab_module,
				section_node->entries)) == NULL)
		done = 1;
	    break;
	}
    }
    sym_stack_pop();
    return(ret_val);
}

static ABObjPtr
object_into_module(
    ABObjPtr		ab_module,
    sym_entry_type	*uil_section_entries
)
{
    sym_section_entry_type	*section_node;
    sym_section_entry_type	*section_trav;
    ABObjPtr			ret_val = NULL;

    if (uil_section_entries == (sym_entry_type *)NULL ||
	uil_section_entries->header.b_tag != sym_k_section_entry)
    {
	return((ABObjPtr)NULL);
    }
    else
	section_node = (sym_section_entry_type *)uil_section_entries;

    /*
     * entries field of an object section is a small chain
     * consisting of type 0 sections (typically one)
     */
    for (section_trav = section_node;
	 section_trav != (sym_section_entry_type *)NULL;
	 section_trav = (sym_section_entry_type *)section_trav->next)
    {
	if (section_trav->header.b_type != 0)
	    continue;
	if ((ret_val = object_child_into_module(ab_module,
			    section_trav->entries)) == NULL)
	    break;
    }
    return(ret_val);
}

static ABObjPtr
object_child_into_module(
    ABObjPtr		ab_module,
    sym_entry_type	*uil_section_entries
)
{
    ABObjPtr			ret_val;
    sym_widget_entry_type	*uil_widget;

    if (uil_section_entries == (sym_entry_type *)NULL)
    {
	return((ABObjPtr)NULL);
    }

    ret_val = ab_module;
    switch (uil_section_entries->header.b_tag)
    {
      case sym_k_widget_entry:
	uil_widget = (sym_widget_entry_type *)uil_section_entries;
	if (uil_widget->parent_list == NULL)
	    ret_val = widget_into_module(ab_module, ab_module,
					    uil_widget);
	break;
      case sym_k_gadget_entry:
	uil_widget = (sym_widget_entry_type *)uil_section_entries;
	if (uil_widget->parent_list == NULL)
	    ret_val = gadget_into_module(ab_module, ab_module,
					    uil_widget);
	break;
      default:
	obj_destroy(ab_module);
	ret_val = (ABObjPtr)NULL;
    }
    return(ret_val);
}

/*
 * Precondition: uil_widget->header.b_tag = sym_k_widget_entry
 *               uil_widget->parent_list = NULL
 */
static ABObjPtr
widget_into_module(
    ABObjPtr			ab_module,
    ABObjPtr			ab_parent,
    sym_widget_entry_type	*uil_widget
)
{
    ABObjPtr			ab_widget;
    sym_list_entry_type		*controls;
    sym_control_entry_type	*a_control;
    sym_widget_entry_type	*uil_child;

    AB_OBJECT_TYPE		ab_type;

    WidgetABObjMap	*ab_map_entry	= abuilP_entry_for_uil_widget(uil_widget);
    STRING		ab_name		= ab_name_for_uil_widget(uil_widget);

    /*
     * Should never happen
     */
    if (ab_map_entry == (WidgetABObjMap *)NULL)
	return(NULL);

    if (ab_parent == (ABObj)NULL)
	return(NULL);

    ab_type = ab_map_entry->obj_type;
    if (ab_type != AB_TYPE_UNKNOWN)
    {
	char	class_name[BUFSIZ];

	ab_widget = obj_create(ab_type, NULL);
	if (ab_map_entry->sub_type != 0)
	    obj_set_subtype(ab_widget, ab_map_entry->sub_type);

	obj_append_child(ab_parent, ab_widget);

	obj_set_name(ab_widget, ab_name);

	strcpy(class_name, ab_map_entry->widget_name);
	class_name[0] = tolower(class_name[0]);
	strcat(class_name, CLASS_SUFFIX);
	obj_set_class_name(ab_widget, class_name);

	/*
	 * Store ABObj reference in uil widget node
	 */
	uil_widget->header.user_data = (long)ab_widget;

	/*
	 * Populate arguments and callbacks
	 */
	if (arguments_into_ab_widget(ab_widget,
		    uil_widget->az_arguments) == NULL ||
	    callbacks_into_ab_module(ab_widget,
		    uil_widget->az_callbacks) == NULL)
	{
	    obj_destroy(ab_widget);
	    return((ABObjPtr)NULL);
	}

    }
    else
    {
	/*
	 * skip this child: it goes as an attribute
	 * in either its parent or its child
	 */
	ab_widget = ab_parent;
	/*
	 * Store ABObj reference in uil widget node
	 */
	uil_widget->header.user_data = (long)ab_widget;
    }


    if (ab_map_entry->object_proc != NULL)
    {
	ABObj	ret_widget;

	ret_widget = (*ab_map_entry->object_proc)(ab_map_entry->widget_name,
					ab_parent, ab_widget);
	if (ret_widget == NULL)
	{
	    obj_unparent(ab_widget);
	    obj_move_children(ab_parent, ab_widget);
	    obj_destroy(ab_widget);
	    return(ab_parent);
	}
	else
	    ab_widget = ret_widget;
    }

    controls = uil_widget->az_controls;
    if (controls == NULL)
	return(ab_widget);

    sym_stack_push();
    for (a_control = (sym_control_entry_type *)controls->obj_header.az_next;
	 a_control != NULL;
	 a_control = (sym_control_entry_type *)a_control->obj_header.az_next)
    {
	uil_child = a_control->az_con_obj;

	/*
	 *  If the reference field is set, this is a reference to a control
	 *  defined elsewhere.  Otherwise it is an inline definition.
	 */
	while (uil_child->obj_header.az_reference != NULL)
	    uil_child =
	    	(sym_widget_entry_type *)uil_child->obj_header.az_reference;
	uil_sym_push((sym_entry_type *) uil_child);
    }

    while ((uil_child = (sym_widget_entry_type *)uil_sym_pop()) != NULL)
    {
	ABObj	ab_child;

	ab_child = widget_into_module(ab_module, ab_widget, uil_child);

	if (ab_child == ab_widget)
	    continue;

	if (ab_map_entry->child_proc != NULL)
	{
	    ABObj	ret_child;

	    ret_child = (*ab_map_entry->child_proc)(ab_map_entry->widget_name,
					    ab_widget, ab_child);
	    if (ret_child == NULL)
		obj_destroy(ab_child);
	    else
		ab_child = ret_child;
	}
    }
    sym_stack_pop();

    return(ab_widget);
}

/*
 * Precondition: uil_widget->header.b_tag = sym_k_gadget_entry
 */
static ABObjPtr
gadget_into_module(
    ABObjPtr			ab_module,
    ABObjPtr			ab_parent,
    sym_widget_entry_type	*uil_gadget
)
{
    fprintf(stderr, "gadget_into_module: UNIMPLEMENTED.\n");
    return(ab_module);
}

static ABObjPtr
arguments_into_ab_widget(
    ABObjPtr		ab_widget,
    sym_list_entry_type *uil_widget_arguments
)
{
    sym_list_entry_type	*widget_args;

    if (uil_widget_arguments == (sym_list_entry_type *)NULL)
	return(ab_widget);

    if (uil_widget_arguments->header.b_tag != sym_k_list_entry ||
	uil_widget_arguments->header.b_type != sym_k_argument_entry)
	return(NULL);

    abuilP_init_context_attrs();
    widget_args = (sym_list_entry_type *)uil_widget_arguments;
    for (widget_args = (sym_list_entry_type *)widget_args->obj_header.az_next;
	 widget_args != NULL;
	 widget_args = (sym_list_entry_type *)widget_args->obj_header.az_next)
    {

	/*
	 * REMIND: All this stuff should be pushed into a function
	 */
	sym_argument_entry_type	*uil_arg;
	STRING			res_name;
	XtPointer		res_value;
	AB_ARG_TYPE		res_type;

	if (widget_args->header.b_tag != sym_k_argument_entry)
	    continue;

	uil_arg = (sym_argument_entry_type *)widget_args;
	res_name = resource_name_for_uil_arg(uil_arg);
	res_value = resource_value_for_uil_arg(uil_arg, &res_type);

	if (!abuilP_store_attr_in_abobj(ab_widget, res_name, res_type, res_value))
	{
	    /* REMIND: we use Xt Args, now!
	    obj_set_arg(ab_widget, res_type, res_name, res_value);
	    */
	}
    }
    abuilP_store_context_attrs(ab_widget);
    return(ab_widget);
}

static ABObjPtr
callbacks_into_ab_module(
    ABObjPtr		ab_widget,
    sym_list_entry_type	*uil_widget_callbacks
)
{
    sym_list_entry_type	*widget_cbs;
    ABObjPtr		ab_module;

    if (uil_widget_callbacks == (sym_list_entry_type *)NULL)
	return(ab_widget);

    ab_module = obj_get_module(ab_widget);
    if (uil_widget_callbacks->header.b_tag != sym_k_list_entry ||
	uil_widget_callbacks->header.b_type != sym_k_callback_entry)
	return(NULL);

    widget_cbs = (sym_list_entry_type *)uil_widget_callbacks;
    for (widget_cbs = (sym_list_entry_type *)widget_cbs->obj_header.az_next;
	 widget_cbs != NULL;
	 widget_cbs = (sym_list_entry_type *)widget_cbs->obj_header.az_next)
    {
	/*
	 * REMIND: All this stuff should be pushed into a function
	 */
	sym_callback_entry_type	*uil_cb;
	AB_WHEN			ab_action_when;
	STRING			*ab_handler_list;
	STRING			*ab_handler_trav;

	/* REMIND: incorporate new action API
	 * ABTrigger		ab_trigger;
	 */

	if (widget_cbs->header.b_tag != sym_k_callback_entry)
	    continue;

	uil_cb = (sym_callback_entry_type *)widget_cbs;

	ab_handler_list = ab_action_handler_list_for_uil_callback(uil_cb,
				&ab_action_when);
	for (ab_handler_trav = ab_handler_list;
	     ab_handler_trav != (STRING *)NULL;
	     ab_handler_trav++)
	{
	    ABObjPtr	ab_action = obj_create(AB_TYPE_ACTION, NULL);

	    obj_set_func_type(ab_action, AB_FUNC_USER_DEF);
	    obj_set_func_name(ab_action, *ab_handler_trav);
	    obj_set_arg_type(ab_action, AB_ARG_VOID_PTR);
	    ab_action->info.action.to = NULL;
	    /*
	     * REMIND: incorporate new action API
	     *
	     * obj_add_action(ab_module, ab_action);

	     * ab_trigger = trigger_create();
	     * ab_trigger->from = ab_widget;
	     * ab_trigger->when = ab_action_when;

	     * obj_action_add_trigger(ab_action, ab_trigger);
	     */
	}
	if (ab_handler_list != (STRING *)NULL)
	    XtFree((char*)ab_handler_list);
    }
    return(ab_module);
}


/*
 * Convenience functions
 */

static STRING
ab_name_for_uil_widget(sym_widget_entry_type	*uil_widget)
{
    STRING	wid_name = NULL;

    if (uil_widget->obj_header.az_name != NULL)
	wid_name = (STRING)&(uil_widget->obj_header.az_name->c_text);

    if (wid_name == NULL)
	wid_name = "NoName";

    return(wid_name);
}

static STRING
resource_name_for_uil_arg(sym_argument_entry_type	*uil_arg)
{
    sym_value_entry_type *resource_name =
		(sym_value_entry_type *)uil_arg->az_arg_name;

    if (resource_name->obj_header.b_flags & sym_m_builtin)
    {
	key_keytable_entry_type *key_entry;

	key_entry = (key_keytable_entry_type *)resource_name->value.l_integer;
	return(key_entry->at_name);
    }
    else
	return((char *)resource_name->value.c_value);
}

static XtPointer
resource_value_for_uil_arg(
    sym_argument_entry_type	*uil_arg,
    AB_ARG_TYPE			*p_res_type
)
{
    sym_value_entry_type *resource_value =
		(sym_value_entry_type *)uil_arg->az_arg_value;

    XtPointer ret_val = (XtPointer)NULL;

    *p_res_type = AB_ARG_VOID_PTR;

    if (resource_value->obj_header.b_flags & sym_m_imported)
    {
	/*
	 * REMIND: implement imported values
	 */
	fprintf(stderr, "imported value for argument: UNIMPLEMENTED\n");
	return(NULL);
    }

    /*
     * Note that the argument value entry need not
     * be of type sym_k_value_entry.
     * It could be a sym_k_widget_entry!! (Oh joy).
     * In that case, process it as a widget reference
     */
    if (resource_value->header.b_tag == sym_k_widget_entry)
    {
	ABObj			abobj_ref;

	sym_widget_entry_type *uil_child =
		(sym_widget_entry_type *)resource_value;

	/*
	 *  The reference field will be set; this is a reference to a control
	 *  defined elsewhere.
	 */
	while (uil_child->obj_header.az_reference != NULL)
	    uil_child =
	    	(sym_widget_entry_type *)uil_child->obj_header.az_reference;

	ret_val = (XtPointer)uil_child;


	abobj_ref = (ABObj)uil_child->header.user_data;

	if (abobj_ref == NULL)
	{
	    *p_res_type = AB_ARG_WIDGET;
	    ret_val = (XtPointer)uil_child;
	}
	else
	{
	    *p_res_type = AB_ARG_VOID_PTR;
	    ret_val = (XtPointer)abobj_ref;
	}
	return(ret_val);
    }

    switch (resource_value->b_type)
    {
      case sym_k_integer_value:
	/*
	 * No special action if value is not an integer enumeration value.
	 * Else; value must be from enumeration set
	 */
	if (resource_value->b_enumeration_value_code != 0)
	{
	    unsigned short int      enumval_code;

	    enumval_code = resource_value->b_enumeration_value_code;
	    ret_val = (XtPointer)uil_enumval_names[enumval_code];
	    *p_res_type = AB_ARG_LITERAL;
	}
	else
	{
	    ret_val = (XtPointer)resource_value->value.l_integer;
	    *p_res_type = AB_ARG_INT;
	}
	break;

      case sym_k_bool_value:
	ret_val = (XtPointer)resource_value->value.l_integer;
	*p_res_type = AB_ARG_BOOLEAN;
	break;

      case sym_k_float_value:
	{
	    float *f_val = (float *)&(resource_value->value.d_real);

	    ret_val = (XtPointer)f_val;
	    *p_res_type = AB_ARG_FLOAT;
	}
	break;

      case sym_k_color_value:
	/*
	{
	    char           *ptr;

	    switch (resource_value->b_arg_type)
	    {
	      case sym_k_background_color:
		ptr = "background";
		break;
	      case sym_k_foreground_color:
		ptr = "foreground";
		break;
	      case sym_k_unspecified_color:
		ptr = "unspecified";
		break;
	      default:
		ptr = "illegal";
	    }
	}
	*/
	ret_val = (XtPointer)resource_value->value.c_value;
	*p_res_type = AB_ARG_STRING;
	break;

      case sym_k_reason_value:
      case sym_k_argument_value:
	break;

      case sym_k_compound_string_value:
          ret_val = _XmStringUngenerate((XmString)resource_value->value.c_value,
                                        XmFONTLIST_DEFAULT_TAG,
                                        XmCHARSET_TEXT, XmCHARSET_TEXT);
          if (ret_val != NULL)
          {
              *p_res_type = AB_ARG_STRING;
          }

          break;

      case sym_k_font_value:
      case sym_k_fontset_value:
	fprintf(stderr,
		"font_value or fontset_value for argument: UNIMPLEMENTED\n");
	goto check_for_table_value;

      case sym_k_char_8_value:
	fprintf(stderr, "char_8_value for argument: UNIMPLEMENTED\n");
	if (resource_value->b_charset != sym_k_userdefined_charset)
	    switch (resource_value->b_direction)
	    {
	      case XmSTRING_DIRECTION_L_TO_R:
		break;
	      case XmSTRING_DIRECTION_R_TO_L:
		break;
	    }
	else
	    switch (resource_value->b_direction)
	    {
	      case XmSTRING_DIRECTION_L_TO_R:
		break;
	      case XmSTRING_DIRECTION_R_TO_L:
		break;
	    }

	/* See if this is an entry in a table.	 */
      check_for_table_value:

	if ((resource_value->b_aux_flags & sym_m_table_entry) != 0)
	{
	    printf("  next table entry: %p",
		       resource_value->az_next_table_value);
	}
	ret_val = resource_value->value.c_value;
	*p_res_type = AB_ARG_LITERAL;
	break;

      case sym_k_identifier_value:
	ret_val = (XtPointer)resource_value->value.c_value;
	*p_res_type = AB_ARG_STRING;
	break;

      case sym_k_icon_value:
	fprintf(stderr, "icon_value for argument: UNIMPLEMENTED\n");
	break;

      case sym_k_string_table_value:
	fprintf(stderr, "string_table_value for argument: UNIMPLEMENTED\n");
	goto common_table;

      case sym_k_font_table_value:
	fprintf(stderr, "font_table_value for argument: UNIMPLEMENTED\n");
	goto common_table;

      case sym_k_trans_table_value:
	fprintf(stderr, "trans_table_value for argument: UNIMPLEMENTED\n");

      common_table:
	break;

      case sym_k_color_table_value:
	fprintf(stderr, "color_table_value for argument: UNIMPLEMENTED\n");
	break;

      case sym_k_error_value:
	fprintf(stderr, "error_value for argument: UNIMPLEMENTED\n");
	break;

      case sym_k_keysym_value:
	/* fprintf(stderr, "keysym_value for argument: UNIMPLEMENTED\n"); */
	ret_val = (XtPointer)resource_value->value.c_value;
	*p_res_type = AB_ARG_STRING;
	break;

      default:
	fprintf(stderr, "unknown type %d for value for argument\n",
	  (int) resource_value->b_type);
	break;
    }
    return(ret_val);
}

static STRING *
ab_action_handler_list_for_uil_callback(
    sym_callback_entry_type	*uil_cb,
    AB_WHEN			*p_ab_action_when
)
{
    return((STRING *)NULL);
}


/*
 * parse_uil: Use the callable UIL compiler to create a uil parse tree.
 *            Check all error conditionals and print a report on stdout.
 *	      Returns the uil parse tree or NULL if there were errors.
 */
static sym_entry_type *
parse_uil(
    char	*uil_file_name
)
{
    Uil_command_type		command_desc;
    Uil_compile_desc_type	compile_desc;
    Uil_status_type		compile_stat;
    int				user_mess_data	= 1;
    int				user_stat_data	= 1;
    char			*ret_val;

    /* Set up command description structure */

    command_desc.source_file 		= uil_file_name;
    command_desc.resource_file		= NULL;
    command_desc.listing_file		= NULL;
    command_desc.include_dir_count	= 0;


    /* give info on what files you want created */

    command_desc.listing_file_flag	= 0;	/* no listing */
    command_desc.resource_file_flag	= 0;	/* no UID file */
    command_desc.machine_code_flag	= 0;	/* no machine code */
    command_desc.report_info_msg_flag	= 1;	/* report info messages */
    command_desc.report_warn_msg_flag	= 1;	/* report all warnings */
    command_desc.parse_tree_flag	= 1;	/* create a parse tree */
    command_desc.status_update_delay	= 300;	/* set proper delay */

    command_desc.database_flag 		= 0;
    command_desc.use_setlocale_flag 	= 1;

    fprintf(stderr, "Compiling UIL file %s...\n", uil_file_name);

    /* Call uil compiler with appropriate paramters */

    compile_stat = Uil(&command_desc, &compile_desc,
		       (UIL_CB)message_cb, (char *)&user_mess_data,
		       (UIL_CB)status_cb, (char *)&user_stat_data);

    fprintf(stderr, "\nUIL Compiler Version %d, ",
		compile_desc.compiler_version);
    fprintf(stderr, "Data Structures Version %d: ", compile_desc.data_version);

    /* check status and print out appropriate messages */

   switch (compile_stat)
   {
     case Uil_k_success_status:
	fprintf(stderr, "no errors.\n");
	ret_val = compile_desc.parse_tree_root;
	break;
     case Uil_k_info_status:
	fprintf(stderr, "no errors, some information.\n");
	ret_val = compile_desc.parse_tree_root;
	break;
     case Uil_k_warning_status:
	fprintf(stderr, "no errors, some warning(s).\n");
	ret_val = compile_desc.parse_tree_root;
	break;
     case Uil_k_error_status:
	fprintf(stderr, "compile failed, some error(s).\n");
	ret_val = (char *)NULL;
	break;
     case Uil_k_severe_status:
	fprintf(stderr, "compile failed, severe error(s).\n");
	ret_val = (char *)NULL;
	break;
   }
   /*
    * Disable Uil specified signal handlers, grrr!
    */
   signal( SIGBUS, SIG_DFL);
#ifdef SIGSYS
   signal( SIGSYS, SIG_DFL);
#endif
   signal( SIGFPE, SIG_DFL);

   return((sym_entry_type *)ret_val);
}

/*
 * message_cb: This procedure will print to stdout information on the
 *             compilation process. Specifically if errors warnings appear.
 */
static Uil_continue_type
message_cb(
    int			*message_user_data,	/* user message number */
    int			msg_number,		/* to identify the error */
    int			msg_severity,		/* severity level */
    char		*msg_text,		/* text of message */
    char		*src_text,		/* text of source of error */
    char		*ptr_text,		/* source column information */
    char		*loc_text,		/* location line */
    int			message_count[]
)
{
    if (msg_number != 38)	/* if not an error summary */
    {
	fprintf(stderr, "\t Diagnostic %d: Code: %d, Severity: %d\n",
		*message_user_data, msg_number, msg_severity);
	fprintf(stderr, "%s: %s\n", loc_text, msg_text);
	fprintf(stderr, "\t\t Source: %s\n", src_text);
    }
    else
	fprintf(stderr, "Compilation results: %s\n", msg_text);

    *message_user_data = *message_user_data + 1;
    return(Uil_k_continue);
}

/*
 * status_cb: print to stdout information the status of the compilation
 *            process. This summary will print the source file being
 *            worked on, the number of lines, and the percentage completed
 */

static Uil_continue_type
status_cb(
    char 	*status_user_data,
    int  	percent_complete,
    int  	lines_processed,
    char 	*current_file,
    int		message_count[]
)
{
 /*
    fprintf(stderr, "Status #%d -- Lines: %d, File: %s, Percentage: %d%%\n",
	*status_user_data, lines_processed, current_file, percent_complete);
  */
    *status_user_data = *status_user_data + 1;
    return(Uil_k_continue);
}

/*
 * Stack management to reverse order of section, list and other
 * sequences in the UIL parse tree
 */
#define	STACK_INIT	32
#define STACK_INCR	16

typedef struct _uil_sym_stack_struct
{
    sym_entry_type	**stack_entries;
    int			stack_ptr;
    int			stack_size;
} UilSymStackStruct, *UilSymStack;

static UilSymStack	*stack_stack;
static int		stack_stack_ptr;
static int		stack_stack_size;


static void
init_sym_stacks(
    void
)
{
    stack_stack_size = STACK_INIT;
    stack_stack = (UilSymStack *)
		    malloc(sizeof(UilSymStack) * stack_stack_size);
    if (stack_stack == (UilSymStack *)NULL)
    {
	fprintf(stderr, "loaduil: malloc failed.\n");
	exit(1);
    }
    stack_stack_ptr = 0;
}

static void
cleanup_sym_stacks(
    void
)
{
    free((void *)stack_stack);
    stack_stack_ptr = 0;
    stack_stack_size = 0;
}

static void
sym_stack_push(
    void
)
{
    UilSymStack	new_stack;

    /*
     * Build new stack
     */
    new_stack = (UilSymStack)malloc(sizeof(UilSymStackStruct));
    if (new_stack == (UilSymStack)NULL)
    {
	fprintf(stderr, "loaduil: malloc failed.\n");
	exit(1);
    }

    new_stack->stack_size = STACK_INIT;
    new_stack->stack_entries = (sym_entry_type **)
		    malloc(sizeof(sym_entry_type *) * new_stack->stack_size);
    if (new_stack->stack_entries == (sym_entry_type **)NULL)
    {
	fprintf(stderr, "loaduil: malloc failed.\n");
	exit(1);
    }
    new_stack->stack_ptr = 0;


    /*
     * Push new stack on stack-stack
     */
    if (stack_stack_ptr == stack_stack_size)
    {
	stack_stack_size += STACK_INCR;
	stack_stack = (UilSymStack *)
	    realloc((void *)stack_stack, sizeof(UilSymStack)*stack_stack_size);
	if (stack_stack == (UilSymStack *)NULL)
	{
	    fprintf(stderr, "loaduil: realloc failed.\n");
	    exit(1);
	}
    }
    stack_stack[stack_stack_ptr++] = new_stack;
}

static void
sym_stack_pop(
    void
)
{
    if (stack_stack_ptr > 0)
	free((void *)stack_stack[--stack_stack_ptr]);
}

/*
 * Pushes a symbol table entry into a stack
 *
 *      sym_entry		pointer to symbol table to push
 */
static void
uil_sym_push(
    sym_entry_type	*sym_entry
)
{
    UilSymStack	cur_stack = stack_stack[stack_stack_ptr - 1];

    if (cur_stack->stack_ptr == cur_stack->stack_size)
    {
	cur_stack->stack_size += STACK_INCR;
	cur_stack->stack_entries = (sym_entry_type **)
	    realloc((void *)cur_stack->stack_entries,
		    sizeof(sym_entry_type *) * cur_stack->stack_size);
	if (cur_stack->stack_entries == (sym_entry_type **)NULL)
	{
	    fprintf(stderr, "loaduil: realloc failed.\n");
	    exit(1);
	}
    }
    cur_stack->stack_entries[cur_stack->stack_ptr++] = sym_entry;
}

/*
 *  Pops a symbol table entry from a stack
 *
 *  It returns
 *      sym_entry		pointer to symbol table popped
 *	NULL			when no more to pop
 */
static sym_entry_type *
uil_sym_pop(
    void
)
{
    UilSymStack	cur_stack = stack_stack[stack_stack_ptr - 1];

    if (cur_stack->stack_ptr == 0)
	return((sym_entry_type *) NULL);
    else
	return(cur_stack->stack_entries[--cur_stack->stack_ptr]);
}


static LList	unresolved_refs;

static void
init_unresolved_refs(
    void
)
{
    unresolved_refs = util_llist_create();
}

extern void
abuil_add_unresolved_ref(
    void	*entry
)
{
    util_llist_insert_after(unresolved_refs, entry);
}

static void
resolve_unresolved_refs(
    void
)
{
    util_llist_iterate(unresolved_refs, object_for_widget);
    util_llist_destroy(unresolved_refs);
}

static void
object_for_widget(
    void	*client_data
)
{
    RefResolve			*rr = (RefResolve *)client_data;
    sym_widget_entry_type	*field_widget;
    ABObj			obj_ref;

    if (rr == (RefResolve *)NULL)
	return;


    field_widget = (sym_widget_entry_type *)
	obj_get_attach_value(rr->obj, rr->dir);
    if (field_widget == NULL)
	return;
    obj_ref = (ABObj)field_widget->header.user_data;
    obj_set_attach_value(rr->obj, rr->dir, obj_ref);
    XtFree((char *)rr);
}

/*
 * Recursively goes through the uil parse tree, freeing each node
 * accessible from the root node in postfix order.
 */
static void
free_uil_tree(
    sym_entry_type		*node_entry
)
{
    sym_value_entry_type	*val_node;
    sym_widget_entry_type	*widget_node;
    sym_module_entry_type	*module_node;
    sym_list_entry_type		*list_node;
    sym_obj_entry_type		*list_entry;
    sym_root_entry_type		*root_node;
    sym_include_file_entry_type	*ifile_node;
    sym_section_entry_type	*section_node;


    /*
     * No action on null node. Else freeing is based on the kind
     * of the current node.
     */
    if (node_entry == NULL)
	return;
    switch (node_entry->header.b_tag)
    {
      case sym_k_value_entry:
	val_node = (sym_value_entry_type *) node_entry;
	free_uil_tree((sym_entry_type *) val_node->az_charset_value);
	free_uil_tree((sym_entry_type *) val_node->az_first_table_value);
	free_uil_tree((sym_entry_type *) val_node->az_next_table_value);
	free_uil_tree((sym_entry_type *) val_node->az_exp_op1);
	free_uil_tree((sym_entry_type *) val_node->az_exp_op2);
	break;

      case sym_k_widget_entry:
      case sym_k_gadget_entry:
      case sym_k_child_entry:
	widget_node = (sym_widget_entry_type *) node_entry;
	free_uil_tree((sym_entry_type *) widget_node->az_callbacks);
	free_uil_tree((sym_entry_type *) widget_node->az_arguments);
	free_uil_tree((sym_entry_type *) widget_node->az_controls);
	free_uil_tree((sym_entry_type *) widget_node->az_create_proc);
	break;

      case sym_k_module_entry:
	module_node = (sym_module_entry_type *) node_entry;
	free_uil_tree((sym_entry_type *) module_node->az_version);
	free_uil_tree((sym_entry_type *) module_node->az_character_set);
	free_uil_tree((sym_entry_type *) module_node->az_case_sense);
	free_uil_tree((sym_entry_type *) module_node->az_def_obj);
	break;

      case sym_k_list_entry:
	list_node = (sym_list_entry_type *) node_entry;
	for (list_entry = (sym_obj_entry_type *)list_node->obj_header.az_next;
	     list_entry != NULL;
	    )
	{
	    list_entry = (sym_obj_entry_type *)list_entry->obj_header.az_next;

	    free_uil_tree((sym_entry_type *) list_entry);
	}
	break;

      case sym_k_root_entry:
	root_node = (sym_root_entry_type *) node_entry;
	free_uil_tree((sym_entry_type *) root_node->module_hdr);
	free_uil_tree((sym_entry_type *) root_node->sections);
	break;

      case sym_k_include_file_entry:
	ifile_node = (sym_include_file_entry_type *) node_entry;
	free_uil_tree((sym_entry_type *) ifile_node->sections);
	break;

      case sym_k_section_entry:
	section_node = (sym_section_entry_type *) node_entry;
	switch (section_node->header.b_type)
	{
	  case sym_k_section_tail:
	    break;

	  default:
	    free_uil_tree((sym_entry_type *) section_node->entries);
	    free_uil_tree((sym_entry_type *) section_node->next);
	    break;
	}
	break;
    }
    XtFree((char*)node_entry);
}
