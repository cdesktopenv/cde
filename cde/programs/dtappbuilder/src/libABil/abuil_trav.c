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
 *	$XConsortium: abuil_trav.c /main/5 1996/10/02 16:31:28 drk $
 *
 * %W% %G%	cose/unity1/cde_app_builder/src/libABil
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
 * This file contains routines to traverse the
 * uil parse tree for debugging
 */
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include <ab_private/UilDef.h>
#include <uil/UilDBDef.h>
#include <uil/UilSymGl.h>	/* For databases */
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>

#include <ab_private/abuil_load.h>


static void	trav_uil_node(
		    sym_entry_type		*node_entry
		);
static void	print_uil_symbol(
		    sym_entry_type	*az_symbol_entry
		);
static void	print_uil_widget(
		    sym_widget_entry_type	*az_widget_entry
		);
static void	print_uil_argument(
		    sym_argument_entry_type	*az_argument_entry
		);
static void	print_uil_control(
		    sym_control_entry_type	*az_control_entry
		);
static void	print_uil_callback(
		    sym_callback_entry_type	*az_callback_entry
		);
static void	print_uil_list(
		    sym_list_entry_type	*az_list_entry
		);
static void	print_uil_name(
		    sym_name_entry_type	*az_name_entry
		);
static void	print_uil_module(
		    sym_module_entry_type	*az_module_entry
		);
static void	print_uil_color_item(
		    sym_color_item_entry_type	*az_color_item_entry
		);
static void	print_uil_parent_list_item(
		    sym_parent_list_type	*az_parent_list_item
		);
static void	print_uil_external_def(
		    sym_external_def_entry_type	*az_external_def_entry
		);
static void	print_uil_proc_def(
		    sym_proc_def_entry_type	*az_proc_def_entry
		);
static void	print_uil_proc_ref(
		    sym_proc_ref_entry_type	*az_proc_ref_entry
		);
static void	print_uil_forward_ref(
		    sym_forward_ref_entry_type	*az_forward_ref_entry
		);
static void	print_uil_value(
		    sym_value_entry_type	*az_value_entry
		);
static void	output_text(
		    int		length,
		    char	*text
		);
static void	print_uil_source_info(
		    sym_entry_header_type	*hdr
		);
static void	print_uil_obj_header(
		    sym_obj_entry_type	*az_obj_entry
		);
static void	print_uil_include_file(
		    sym_include_file_entry_type	*az_symbol_entry
		);
static void	print_uil_section(
		    sym_section_entry_type	*az_symbol_entry
		);
static void	print_uil_object_variant(
		    sym_def_obj_entry_type	*az_symbol_entry
		);
static void	print_uil_root_entry(
		    sym_root_entry_type	*az_symbol_entry
		);
static char	*get_sym_section_text(
		    int	b_type
		);
static char	*get_tag_text(
		    int b_tag
		);
static char	*get_object_text(
		    int	b_type
		);
static char	*get_value_text(
		    int b_type
		);
static char	*get_charset_text(
		    int b_type
		);

/*
 * Recursively goes through the uil parse tree, dumping each node
 * accessible from the root node.
 */
extern void
abuil_trav_uil_tree(
    void	*uil_root
)
{
    trav_uil_node((sym_entry_type *)uil_root);
}

static void
trav_uil_node(
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
     * No action on null node. Else dump and processing is based on the kind
     * of the current node.
     */
    if (node_entry == NULL)
	return;
    printf("> ");
    print_uil_symbol(node_entry);
    switch (node_entry->header.b_tag)
    {
      case sym_k_value_entry:
	val_node = (sym_value_entry_type *) node_entry;
	trav_uil_node((sym_entry_type *) val_node->az_charset_value);
	trav_uil_node((sym_entry_type *) val_node->az_first_table_value);
	trav_uil_node((sym_entry_type *) val_node->az_next_table_value);
	trav_uil_node((sym_entry_type *) val_node->az_exp_op1);
	trav_uil_node((sym_entry_type *) val_node->az_exp_op2);
	break;

      case sym_k_widget_entry:
      case sym_k_gadget_entry:
      case sym_k_child_entry:
	widget_node = (sym_widget_entry_type *) node_entry;
	trav_uil_node((sym_entry_type *) widget_node->az_callbacks);
	trav_uil_node((sym_entry_type *) widget_node->az_arguments);
	trav_uil_node((sym_entry_type *) widget_node->az_controls);
	trav_uil_node((sym_entry_type *) widget_node->az_create_proc);
	break;

      case sym_k_module_entry:
	module_node = (sym_module_entry_type *) node_entry;
	trav_uil_node((sym_entry_type *) module_node->az_version);
	trav_uil_node((sym_entry_type *) module_node->az_character_set);
	trav_uil_node((sym_entry_type *) module_node->az_case_sense);
	trav_uil_node((sym_entry_type *) module_node->az_def_obj);
	break;

      case sym_k_list_entry:
	/*
	 * Sublists (nested lists) are not processed recursively to pick up
	 * definitions, since all named lists are picked up in their list
	 * sections.
	 * We assume no list of interest to us can possibly be
	 * encountered only in a nested list reference.
	 */
	list_node = (sym_list_entry_type *) node_entry;
	for (list_entry = (sym_obj_entry_type *)
		list_node->obj_header.az_next;
		list_entry != NULL;
		list_entry = (sym_obj_entry_type *)
		list_entry->obj_header.az_next)
	    trav_uil_node((sym_entry_type *) list_entry);
	break;

      case sym_k_root_entry:
	root_node = (sym_root_entry_type *) node_entry;
	trav_uil_node((sym_entry_type *) root_node->module_hdr);
	trav_uil_node((sym_entry_type *) root_node->sections);
	break;

      case sym_k_include_file_entry:
	ifile_node = (sym_include_file_entry_type *) node_entry;
	trav_uil_node((sym_entry_type *) ifile_node->sections);
	break;

      case sym_k_section_entry:
	section_node = (sym_section_entry_type *) node_entry;
	switch (section_node->header.b_type)
	{
	  case sym_k_section_tail:
	    break;

	  default:
	    trav_uil_node((sym_entry_type *) section_node->entries);
	    trav_uil_node((sym_entry_type *) section_node->next);
	    break;
	}
	break;
    }
}

/*
 * Prints a symbol node.
 */
static void
print_uil_symbol(
    sym_entry_type	*az_symbol_entry
)
{
    switch (az_symbol_entry->header.b_tag)
    {
      case sym_k_name_entry:
	print_uil_name((sym_name_entry_type *) az_symbol_entry);
	break;

      case sym_k_module_entry:
	print_uil_module((sym_module_entry_type *) az_symbol_entry);
	break;

      case sym_k_value_entry:
	print_uil_value((sym_value_entry_type *) az_symbol_entry);
	break;

      case sym_k_widget_entry:
      case sym_k_gadget_entry:
      case sym_k_child_entry:
	print_uil_widget((sym_widget_entry_type *) az_symbol_entry);
	break;

      case sym_k_forward_ref_entry:
	print_uil_forward_ref((sym_forward_ref_entry_type *) az_symbol_entry);
	break;

      case sym_k_external_def_entry:
	print_uil_external_def((sym_external_def_entry_type *) az_symbol_entry);
	break;

      case sym_k_proc_def_entry:
	print_uil_proc_def((sym_proc_def_entry_type *) az_symbol_entry);
	break;

      case sym_k_proc_ref_entry:
	print_uil_proc_ref((sym_proc_ref_entry_type *) az_symbol_entry);
	break;

      case sym_k_control_entry:
	print_uil_control((sym_control_entry_type *) az_symbol_entry);
	break;

      case sym_k_argument_entry:
	print_uil_argument((sym_argument_entry_type *) az_symbol_entry);
	break;

      case sym_k_callback_entry:
	print_uil_callback((sym_callback_entry_type *) az_symbol_entry);
	break;

      case sym_k_list_entry:
	print_uil_list((sym_list_entry_type *) az_symbol_entry);
	break;

      case sym_k_color_item_entry:
	print_uil_color_item((sym_color_item_entry_type *) az_symbol_entry);
	break;

      case sym_k_parent_list_entry:
	print_uil_parent_list_item((sym_parent_list_type *) az_symbol_entry);
	break;

      case sym_k_include_file_entry:
	print_uil_include_file((sym_include_file_entry_type *) az_symbol_entry);
	break;

      case sym_k_section_entry:
	print_uil_section((sym_section_entry_type *) az_symbol_entry);
	break;

      case sym_k_def_obj_entry:
	print_uil_object_variant((sym_def_obj_entry_type *) az_symbol_entry);
	break;

      case sym_k_root_entry:
	print_uil_root_entry((sym_root_entry_type *) az_symbol_entry);
	break;

      default:
	{
	    int	*l_array;
	    int	i;

	    printf("%lx  unknown type: %d  size: %d  byte: 0x%x\n",
			  az_symbol_entry,
			  az_symbol_entry->header.b_tag,
			  az_symbol_entry->header.w_node_size,
			  az_symbol_entry->header.b_type);

	    l_array = (int *) az_symbol_entry->b_value;

	    for (i = 0; i < (int) (az_symbol_entry->header.w_node_size - 1); i++)
		printf("\t%x", l_array[i]);

	    printf("\n");
	    break;
	}
    }

    print_uil_source_info((sym_entry_header_type *) az_symbol_entry);
    printf("\n");
}

/*
 * Prints an object entry in the symbol table
 */
static void
print_uil_widget(
    sym_widget_entry_type	*az_widget_entry
)
{
    char	*wid_name = NULL;

    if (az_widget_entry->obj_header.az_name != NULL)
        wid_name = (char *)&(az_widget_entry->obj_header.az_name->c_text);
 
    if (wid_name == NULL)
        wid_name = "<NoName>";
 
    /*
    print_uil_obj_header((sym_obj_entry_type *) az_widget_entry);
     */

    if (az_widget_entry->obj_header.b_flags & sym_m_obj_is_gadget)
	printf("GADGET ");
    printf(
    "%s %s(0x%lx) name: %s\n",
		  get_object_text(az_widget_entry->header.b_type),
		  get_tag_text(az_widget_entry->header.b_tag),
		  az_widget_entry,
		  wid_name);

    printf(
    "  controls: 0x%lx  callbacks: 0x%lx  arguments: 0x%lx  parent_list: 0x%lx\n",
		  az_widget_entry->az_controls,
		  az_widget_entry->az_callbacks,
		  az_widget_entry->az_arguments,
		  az_widget_entry->parent_list);

    if (az_widget_entry->az_create_proc != NULL)
    {
	printf("  create proc: 0x%lx\n",
		      az_widget_entry->az_create_proc);
    }
}

/*
 * Prints an argument entry in the symbol table
 */
static void
print_uil_argument(
    sym_argument_entry_type	*az_argument_entry
)
{
    char		 *name_str;

    sym_value_entry_type *resource_name =
		(sym_value_entry_type *)az_argument_entry->az_arg_name;
    sym_value_entry_type *resource_value =
		(sym_value_entry_type *)az_argument_entry->az_arg_value;
    
    if (resource_name->obj_header.b_flags & sym_m_builtin)
    {
	key_keytable_entry_type *key_entry;

	key_entry = (key_keytable_entry_type *)resource_name->value.l_integer;
	name_str = key_entry->at_name;
    }
    else
	name_str = (char *)resource_name->value.c_value;
    printf("  ARGUMENT name: %s  arg value: ", name_str);

    /*
     * No special action if value is not an integer enumeration value.
     * Else:
     *      - argument must support enumeration set
     *      - value must be from set
     */
    if (resource_value != NULL &&
	resource_value->b_type == sym_k_integer_value &&
	resource_value->b_enumeration_value_code != 0)
    {
	unsigned short int      enumval_code;
	 
	enumval_code = resource_value->b_enumeration_value_code;
	printf("%s\n", uil_enumval_names[enumval_code]);
    }
    else
	print_uil_value(resource_value);

}

/*
 * Prints a control entry in the symbol table
 */
static void
print_uil_control(
    sym_control_entry_type	*az_control_entry
)
{
    sym_widget_entry_type	*control_obj;

    /*
    print_uil_obj_header((sym_obj_entry_type *) az_control_entry);
     */

    printf("  CONTROL");
    /*
     * These are for control objects only.
     */
    if (az_control_entry->obj_header.b_flags & sym_m_def_in_progress)
    {
	printf("  def in progress");
    }

    if (az_control_entry->obj_header.b_flags & sym_m_managed)
    {
	printf("  managed");
    }
    else
    {
	printf("  unmanaged");
    }

    printf("  managed object: ");

    control_obj = az_control_entry->az_con_obj;
    /*
     *  If the reference field is set, this is a reference to a control
     *  defined elsewhere.  Otherwise it is an inline definition.
     */
    while (control_obj->obj_header.az_reference != NULL)
	control_obj =
	    (sym_widget_entry_type *)control_obj->obj_header.az_reference;

    print_uil_widget(control_obj);
}

/*
 * Prints a callback entry in the symbol table
 */
static void
print_uil_callback(
    sym_callback_entry_type	*az_callback_entry
)
{
    char		 *name_str;

    sym_value_entry_type *reason_name =
		(sym_value_entry_type *)az_callback_entry->az_call_reason_name;
    
    if (reason_name->obj_header.b_flags & sym_m_builtin)
    {
	key_keytable_entry_type *key_entry;

	key_entry = (key_keytable_entry_type *)reason_name->value.l_integer;
	name_str = key_entry->at_name;
    }
    else
	name_str = (char *)reason_name->value.c_value;

    printf(
    "  CALLBACK name: %s  proc ref: 0x%lx  proc ref list: 0x%lx\n",
		  name_str,
		  az_callback_entry->az_call_proc_ref,
		  az_callback_entry->az_call_proc_ref_list);
}

/*
 * Prints a  list entry in the symbol table
 */
static void
print_uil_list(
    sym_list_entry_type	*az_list_entry
)
{
    /*
    print_uil_obj_header((sym_obj_entry_type *) az_list_entry);
     */

    printf("%s LIST(0x%lx)  count: %d  gadget count: %d\n",
		  get_tag_text(az_list_entry->header.b_type),
		  az_list_entry,
		  az_list_entry->w_count,
		  az_list_entry->w_gadget_count);
}

/*
 * Prints a name entry in the symbol table
 */
static void
print_uil_name(
    sym_name_entry_type	*az_name_entry
)
{
    printf("NAME(0x%lx)  size: %d  next name: 0x%lx  object: 0x%lx",
     az_name_entry,
     az_name_entry->header.w_node_size,
     az_name_entry->az_next_name_entry,
     az_name_entry->az_object);

    if (az_name_entry->b_flags & sym_m_referenced)
    {
	printf(" referenced");
    }

    printf("  name: %s \n", az_name_entry->c_text);
}

/*
 * Prints a module entry in the symbol table
 */
static void
print_uil_module(
    sym_module_entry_type	*az_module_entry
)
{
    printf("%s MODULE(0x%lx)  version: 0x%lx \n",
     az_module_entry->obj_header.az_name->c_text,
     az_module_entry,
     az_module_entry->az_version);
}

/*
 * Prints a color item entry in the symbol table
 */
static void
print_uil_color_item(
    sym_color_item_entry_type	*az_color_item_entry
)
{
    printf(
    "COLOR ITEM(0x%lx)  size: %d  letter: %c  index: %d  color: 0x%lx  next: 0x%lx\n",
     az_color_item_entry,
     az_color_item_entry->header.w_node_size,
     az_color_item_entry->b_letter,
     az_color_item_entry->b_index,
     az_color_item_entry->az_color,
     az_color_item_entry->az_next);
}

/*
 * Prints a parent_list entry in the symbol table
 */
static void
print_uil_parent_list_item(
    sym_parent_list_type	*az_parent_list_item
)
{
    printf("PARENT LIST(0x%lx)  parent: 0x%lx  next: 0x%lx \n",
     az_parent_list_item,
     az_parent_list_item->parent,
     az_parent_list_item->next);
}

/*
 * Prints an external definition entry in the symbol table
 */
static void
print_uil_external_def(
    sym_external_def_entry_type	*az_external_def_entry
)
{
    printf("EXTERNAL DEF(0x%lx)  next external: 0x%lx  object: 0x%lx \n",
     az_external_def_entry,
     az_external_def_entry->az_next_object,
     az_external_def_entry->az_name);
}

/*
 * Prints a procedure definition entry in the symbol table
 */
static void
print_uil_proc_def(
    sym_proc_def_entry_type	*az_proc_def_entry
)
{
    char           *private_flag;
    char           *imported_flag;
    char           *exported_flag;
    char           *checking_flag;

    private_flag = "";
    imported_flag = "";
    exported_flag = "";
    checking_flag = " no-check";

    if (az_proc_def_entry->v_arg_checking)
	checking_flag = " check";
    if (az_proc_def_entry->obj_header.b_flags & sym_m_private)
	private_flag = " private";
    if (az_proc_def_entry->obj_header.b_flags & sym_m_exported)
	exported_flag = " exported";
    if (az_proc_def_entry->obj_header.b_flags & sym_m_imported)
	imported_flag = " imported";

    printf("PROC DEF(0x%lx)  name: 0x%lx %s%s%s%s  count: %d  %s\n",
	 az_proc_def_entry,
	 az_proc_def_entry->obj_header.az_name,
	 checking_flag,
	 private_flag,
	 exported_flag,
	 imported_flag,
	 az_proc_def_entry->b_arg_count,
	 get_value_text(az_proc_def_entry->b_arg_type));
}

/*
 * Prints a procedure reference entry in the symbol table
 */
static void
print_uil_proc_ref(
    sym_proc_ref_entry_type	*az_proc_ref_entry
)
{
    /*
    print_uil_obj_header((sym_obj_entry_type *) az_proc_ref_entry);
     */

    printf("PROC REF(0x%lx)  proc def: 0x%lx  value: 0x%lx\n",
	 az_proc_ref_entry,
	 az_proc_ref_entry->az_proc_def,
	 az_proc_ref_entry->az_arg_value);
}

/*
 * Prints an forward reference entry in the symbol table
 */
static void
print_uil_forward_ref(
    sym_forward_ref_entry_type	*az_forward_ref_entry
)
{
    printf(
    "FORWARD REF(0x%lx)  next ref: 0x%lx  location: 0x%lx  %s  parent: 0x%lx\n",
     az_forward_ref_entry,
     az_forward_ref_entry->az_next_ref,
     az_forward_ref_entry->a_update_location,
     get_object_text(az_forward_ref_entry->header.b_type),
     az_forward_ref_entry->parent);

    printf("  name: 0x%lx %s\n",
	 az_forward_ref_entry->az_name,
	 az_forward_ref_entry->az_name->c_text);
}

/*
 * Prints a value entry in the symbol table
 */
static void
print_uil_value(
    sym_value_entry_type	*az_value_entry
)
{
    char	*private_flag;
    char	*imported_flag;
    char	*exported_flag;
    char	*builtin_flag;
    char	*special_type;
    char	*table_type;

    private_flag = "";
    imported_flag = "";
    exported_flag = "";
    builtin_flag = "";

    if (az_value_entry->obj_header.b_flags & sym_m_builtin)
	builtin_flag = " builtin";
    if (az_value_entry->obj_header.b_flags & sym_m_private)
	private_flag = " private";
    if (az_value_entry->obj_header.b_flags & sym_m_exported)
	exported_flag = " exported";
    if (az_value_entry->obj_header.b_flags & sym_m_imported)
	imported_flag = " imported";

    printf("  VALUE(0x%lx)  size: %d  name: 0x%lx  %s%s%s%s",
	 az_value_entry,
	 az_value_entry->header.w_node_size,
	 az_value_entry->obj_header.az_name,
	 builtin_flag, private_flag, exported_flag, imported_flag);

    if (az_value_entry->obj_header.b_flags & sym_m_imported)
    {
	printf("  %s \n", get_value_text(az_value_entry->b_type));
	return;
    }

    switch (az_value_entry->b_type)
    {
      case sym_k_integer_value:
	printf("  integer: %d \n",
		      az_value_entry->value.l_integer);
	break;

      case sym_k_bool_value:
	printf("  boolean: %d \n",
		      az_value_entry->value.l_integer);
	break;

      case sym_k_float_value:
	printf("  double: %g \n",
		      az_value_entry->value.d_real);
	break;

      case sym_k_color_value:
	{
	    char           *ptr;

	    switch (az_value_entry->b_arg_type)
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

	    printf("  color  type: %s", ptr);

	    output_text(az_value_entry->w_length,
			az_value_entry->value.c_value);

	    break;
	}

      case sym_k_reason_value:
	special_type = "reason";
	goto common_special_type;

      case sym_k_argument_value:
	special_type = "argument";

common_special_type:

	printf("  %s", special_type);

	if (az_value_entry->obj_header.b_flags & sym_m_builtin)
	    printf("  code: %ld \n", az_value_entry->value.l_integer);
	else
	    output_text(az_value_entry->w_length,
			az_value_entry->value.c_value);

	break;

      case sym_k_compound_string_value:
	printf("  compound string\n  first component: 0x%lx\n",
		      az_value_entry->az_first_table_value);

	if ((az_value_entry->b_aux_flags & sym_m_table_entry) != 0)
	{
	    printf("  next table entry: 0x%lx",
			  az_value_entry->az_next_table_value);
	}

	break;


      case sym_k_font_value:
      case sym_k_fontset_value:
	if (az_value_entry->b_charset != sym_k_userdefined_charset)
	    printf("  font  charset: %s",
			  get_charset_text(az_value_entry->b_charset));
	else
	    printf("  font  charset: userdefined(%x)",
		 get_charset_text((long) az_value_entry->az_charset_value));

	goto check_for_table_value;


      case sym_k_char_8_value:
	if (az_value_entry->b_charset != sym_k_userdefined_charset)
	    switch (az_value_entry->b_direction)
	    {
	      case XmSTRING_DIRECTION_L_TO_R:
		printf("  string length: %d\n  charset: %s  L_TO_R",
		     az_value_entry->w_length,
		     get_charset_text(
				       az_value_entry->b_charset));
		break;
	      case XmSTRING_DIRECTION_R_TO_L:
		printf("  string length: %d\n  charset: %s  R_TO_L",
		     az_value_entry->w_length,
		     get_charset_text(
				       az_value_entry->b_charset));
		break;
	    }
	else
	    switch (az_value_entry->b_direction)
	    {
	      case XmSTRING_DIRECTION_L_TO_R:
		printf(
		"  string length: %d\n  charset: userdefined(%x)  L_TO_R",
		     az_value_entry->w_length,
		     az_value_entry->az_charset_value);
		break;
	      case XmSTRING_DIRECTION_R_TO_L:
		printf(
		"  string length: %d\n  charset: userdefined(%x)  R_TO_L",
		     az_value_entry->w_length,
		     az_value_entry->az_charset_value);
		break;
	    }

	/*
	 * See if this is an entry in a table.
	 */
check_for_table_value:

	if ((az_value_entry->b_aux_flags & sym_m_table_entry) != 0)
	{
	    printf("  next table entry: 0x%lx",
			  az_value_entry->az_next_table_value);
	}

	output_text
	    (az_value_entry->w_length, az_value_entry->value.c_value);

	break;

      case sym_k_identifier_value:
	printf("  identifier: %s", az_value_entry->value.c_value);

	break;

      case sym_k_icon_value:
	printf("  icon  width: %d  height: %d  colors: 0x%lx  rows: 0x%lx \n",
		      az_value_entry->value.z_icon->w_width,
		      az_value_entry->value.z_icon->w_height,
		      az_value_entry->value.z_icon->az_color_table,
		      az_value_entry->value.z_icon->az_rows);

	break;

      case sym_k_string_table_value:
	table_type = "string table";
	goto common_table;

      case sym_k_font_table_value:
	table_type = "font table";
	goto common_table;

      case sym_k_trans_table_value:
	table_type = "translation table";

common_table:

	printf("  %s  first table entry: 0x%lx\n",
		      table_type, az_value_entry->az_first_table_value);

	break;

      case sym_k_color_table_value:
	{
	    int             index;

	    printf("  color_table  count: %d  max_index: %d \n",
			  az_value_entry->b_table_count,
			  az_value_entry->b_max_index);

	    for (index = 0; index < (int) az_value_entry->b_table_count;
		 index++)
	    {

		printf("    letter: %c  index: %d  color: 0x%lx\n",
			      az_value_entry->value.z_color[index].b_letter,
			      az_value_entry->value.z_color[index].b_index,
			      az_value_entry->value.z_color[index].az_color);
	    }

	    break;
	}

      case sym_k_error_value:
	printf("  error \n");

	break;

      default:
	printf(" unknown type: %d \n", az_value_entry->b_type);
    }
}

/*
 * Prints an arbitrarily long amount of text
 * 
 * length	length of the text
 * text		pointer to the text
 */
static void
output_text(
    int		length,
    char	*text
)
{
    char            c_buffer[71];
    char           *c_ptr;
    int             l_length;

    l_length = length;

    printf("\n");

    for (c_ptr = text;

	    l_length > 0;

	    l_length -= 70,
	    c_ptr += 70)
    {
	int             last;
	int             i;

	last = (l_length > 70) ? 70 : l_length;

	memmove(c_buffer, c_ptr, last);

	for (i = 0; i < last; i++)
	{
	    if (iscntrl(c_buffer[i]))
		c_buffer[i] = '.';
	}

	c_buffer[last] = 0;
	printf("    \"%s\"\n", c_buffer);
    }
}

/*
 * Prints the source information in the header of symbol entries.
 */
static void
print_uil_source_info(
    sym_entry_header_type	*hdr
)
{
}

/*
 * Prints the common header of "object" entries.
 */
static void
print_uil_obj_header(
    sym_obj_entry_type	*az_obj_entry
)
{
    printf("%s(0x%lx)",
     get_tag_text(az_obj_entry->header.b_tag),
     az_obj_entry);

    if (az_obj_entry->obj_header.az_name != NULL)
    {
	printf("  name: 0x%lx", az_obj_entry->obj_header.az_name);
    }

    if (az_obj_entry->obj_header.az_reference != NULL)
    {
	printf("  reference: 0x%lx",
		      az_obj_entry->obj_header.az_reference);
    }

    if (az_obj_entry->obj_header.az_next != NULL)
    {
	printf("  next: 0x%lx", az_obj_entry->obj_header.az_next);
    }

    if (az_obj_entry->obj_header.b_flags & sym_m_private)
    {
	printf(" private");
    }

    if (az_obj_entry->obj_header.b_flags & sym_m_exported)
    {
	printf(" exported");
    }

    if (az_obj_entry->obj_header.b_flags & sym_m_imported)
    {
	printf(" imported");
    }

    printf("\n");
}


static void
print_uil_include_file(
    sym_include_file_entry_type	*az_symbol_entry
)
{
    printf("INCLUDE FILE(0x%lx)  file name: %s  full file name: %s\n",
	       az_symbol_entry,
	       az_symbol_entry->file_name, az_symbol_entry->full_file_name);
}


static void
print_uil_section(
    sym_section_entry_type	*az_symbol_entry
)
{
    printf("%s SECTION(0x%lx)  entries: 0x%lx\n",
		  get_sym_section_text(az_symbol_entry->header.b_type),
		  az_symbol_entry,
		  az_symbol_entry->entries);
}

static void
print_uil_object_variant(
    sym_def_obj_entry_type	*az_symbol_entry
)
{
    printf(
    "DEFAULT OBJ VAR(0x%lx)  next: 0x%lx  object info: %d, variant_info: %d\n",
		  az_symbol_entry,
		  az_symbol_entry->next, az_symbol_entry->b_object_info,
		  az_symbol_entry->b_variant_info);
}

static void
print_uil_root_entry(
    sym_root_entry_type	*az_symbol_entry
)
{
    printf("ROOT(0x%lx)  tag: %d  module: 0x%lx  sections: 0x%lx\n  module tail: ",
		  az_symbol_entry,
		  az_symbol_entry->header.b_tag,
		  az_symbol_entry->module_hdr,
		  az_symbol_entry->sections);
}

static char *
get_sym_section_text(
    int	b_type
)
{
    switch (b_type)
    {
      case 0:
	return "<UNTYPED>";
      case sym_k_list_section:
	return "LIST";
      case sym_k_procedure_section:
	return "PROCEDURE";
      case sym_k_value_section:
	return "VALUE";
      case sym_k_identifier_section:
	return "IDENTIFIER";
      case sym_k_object_section:
	return "OBJECT";
      case sym_k_include_section:
	return "INCLUDE";
      case sym_k_section_tail:
	return "TAIL";
      default:
	return "*UNKNOWN*";
    }
}

/*
 * The major constructs in the UIL language each have a data structure and a
 * tag value in the first byte of that data structure.  Given a tag value,
 * this routine will return a string that can be substituted in an error
 * message to describe that construct.
 */
static char *
get_tag_text(
    int b_tag
)
{
    switch (b_tag)
    {
      case sym_k_value_entry:
	return "VALUE";
      case sym_k_widget_entry:
	return "WIDGET";
      case sym_k_gadget_entry:
	return "GADGET";
      case sym_k_child_entry:
	return "AUTO CHILD";
      case sym_k_module_entry:
	return "MODULE";
      case sym_k_proc_def_entry:
      case sym_k_proc_ref_entry:
	return "PROCEDURE";
      case sym_k_identifier_entry:
	return "IDENTIFIER";
      case sym_k_argument_entry:
	return "ARGUMENT";
      case sym_k_callback_entry:
	return "CALLBACK";
      case sym_k_control_entry:
	return "CONTROL";
      case sym_k_name_entry:
	return "NAME";
      case sym_k_forward_ref_entry:
	return "FORWARD REF";
      case sym_k_external_def_entry:
	return "EXTERNAL DEF";
      case sym_k_list_entry:
	return "LIST";
      case sym_k_root_entry:
	return "ROOT";
      case sym_k_include_file_entry:
	return "INCLUDE FILE";
      case sym_k_def_obj_entry:
	return "DEFAULT VARIANT";
      case sym_k_section_entry:
	return "SECTION";

      default:
	return "**UNKNOWN**";
    }
}

/*
 * Each widget in the UIL language has a integer value.  Given this value,
 * this routine will return a string that can be substituted in an error
 * message to describe that widget.
 */
static char *
get_object_text(
    int	b_type
)
{
    if (b_type <= sym_k_error_object)
	return "** error **";
    if (b_type <= uil_max_object)
	return uil_widget_names[b_type];
    return "** unknown **";
}

/*
 * Each value in the UIL language has a integer value.  Given this value,
 * this routine will return a string that can be substituted in an error
 * message to describe that value.
 */
static char *
get_value_text(
    int b_type
)
{
    if (b_type <= sym_k_error_value)
	return "** error **";
    if (b_type <= sym_k_max_value)
	return uil_datatype_names[b_type];
    return "** unknown **";
}

/*
 * Each charset supported by UIL has a integer value.  Given this value, this
 * routine will return a string that can be substituted in an error message
 * to describe that charset.
 */
static char *
get_charset_text(
    int b_type
)
{
    if (b_type <= sym_k_error_charset)
	return "** error **";
    if (b_type <= uil_max_charset)
	return uil_charset_names[b_type];
    return "** unknown **";
}
