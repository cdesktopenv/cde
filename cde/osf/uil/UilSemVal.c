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
 *  @OSF_COPYRIGHT@
 *  COPYRIGHT NOTICE
 *  Copyright (c) 1990, 1991, 1992, 1993 Open Software Foundation, Inc.
 *  ALL RIGHTS RESERVED (MOTIF). See the file named COPYRIGHT.MOTIF for
 *  the full copyright text.
*/ 
/* 
 * HISTORY
*/ 
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: UilSemVal.c /main/18 1997/09/15 14:15:21 cshi $"
#endif
#endif

/*
*  (c) Copyright 1989, 1990, DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASS. */

/*
**++
**  FACILITY:
**
**      User Interface Language Compiler (UIL)
**
**  ABSTRACT:
**
**      This module contains the second pass routines for performing
**	semantic validation.
**
**--
**/


/*
**
**  INCLUDE FILES
**
**/

#include <stdlib.h>
#include <setjmp.h>
#include <Mrm/MrmAppl.h>
#include <Xm/XmStrDefs.h>


#include "UilDefI.h"


/*
**
**  DEFINE and MACRO DEFINITIONS
**
**  The order of these constants is significant. The constants lower in
**  value than error_arg_type are basically numeric types and various
**  conversions may be done on those types. Those constants are compared
**  numerically with error_arg_type to determine whether they may be
**  converted. Furthermore, the numeric_convert_table is indexed by
**  those constants. If you are adding new arg types, add before error_arg_type
**  if it is a numeric type, but also remember to update the numeric_
**  convert_table below to have an entry for that type. Add the new type
**  after error_arg_type if it is not numeric. lstr_arg_type must be after
**  char_arg_type and before cstr_arg_type.  When done, update all the constants
**  to be sequential.
**
**/

#define	boolean_arg_type    		0
#define	integer_arg_type    		1
#define single_float_arg_type 		2
#define float_arg_type	    		3
#define horizontal_integer_arg_type 	4
#define vertical_integer_arg_type 	5
#define horizontal_float_arg_type 	6
#define vertical_float_arg_type 	7
#define error_arg_type	    		8

#define	char_arg_type	    		9
#define lstr_arg_type			10
#define	cstr_arg_type	    		11
#define keysym_arg_type  		12
#define font_arg_type	    		13
#define color_arg_type	    		14
#define xbitmap_arg_type   		15
#define reason_arg_type	   		16
#define argument_arg_type  		17
#define font_table_arg_type 		18
#define wcstr_arg_type			19
#define fontset_arg_type		20
/*  BEGIN HaL fix CR 5429 */ 
#define classrec_arg_type     		21
/* END HaL Fix CR 5429 */

/*
**
**  EXTERNAL VARIABLE DECLARATIONS
**
**/


/*
**
**  GLOBAL VARIABLE DECLARATIONS
**
**/


/*
**
**  OWN VARIABLE DECLARATIONS
**
**/

static unsigned int		ref_chk_value = 0;
static short			in_expr = 0;
static int			cycle_id = 1;

/*
**  This table is indexed by arg_types defined above that are less than
**  error_arg_type.
**/

static int ( * numeric_convert_table[])() = {
	    0,
	    sem_convert_to_integer,
	    sem_convert_to_single_float,
	    sem_convert_to_float,
	    sem_convert_to_integer,
	    sem_convert_to_integer,
	    sem_convert_to_float,
	    sem_convert_to_float,
	    sem_convert_to_error };

/*
 * The next two definitions must match value sets defining 
 * expression operators in UilSymDef.h
 */

static unsigned int legal_operand_type[ ] = {
    /* unused */	0,
    /* not */		1 << sym_k_bool_value | 1 << sym_k_integer_value, 
    /* unary plus */	1 << sym_k_integer_value |
      			1 << sym_k_horizontal_integer_value |
      			1 << sym_k_vertical_integer_value |
      			1 << sym_k_float_value |
			1 << sym_k_horizontal_float_value |
			1 << sym_k_vertical_float_value | 
                        1 << sym_k_single_float_value,
    /* unary minus */	1 << sym_k_integer_value |
      			1 << sym_k_horizontal_integer_value |
      			1 << sym_k_vertical_integer_value |
      			1 << sym_k_float_value |
			1 << sym_k_horizontal_float_value |
			1 << sym_k_vertical_float_value | 
                        1 << sym_k_single_float_value, 
    /* comp_str */	1 << sym_k_char_8_value | 
			1 << sym_k_localized_string_value |
			1 << sym_k_compound_string_value,
    /* wchar_str */	1 << sym_k_localized_string_value,
    /* multiply */	1 << sym_k_integer_value |
      			1 << sym_k_horizontal_integer_value |
      			1 << sym_k_vertical_integer_value |
      			1 << sym_k_float_value |
			1 << sym_k_horizontal_float_value |
			1 << sym_k_vertical_float_value | 
                        1 << sym_k_single_float_value, 
    /* divide */	1 << sym_k_integer_value |
      			1 << sym_k_horizontal_integer_value |
      			1 << sym_k_vertical_integer_value |
      			1 << sym_k_float_value |
			1 << sym_k_horizontal_float_value |
			1 << sym_k_vertical_float_value | 
                        1 << sym_k_single_float_value,
    /* add */		1 << sym_k_integer_value |
      			1 << sym_k_horizontal_integer_value |
      			1 << sym_k_vertical_integer_value |
      			1 << sym_k_float_value |
			1 << sym_k_horizontal_float_value |
			1 << sym_k_vertical_float_value | 
                        1 << sym_k_single_float_value, 
    /* subtract */	1 << sym_k_integer_value |
      			1 << sym_k_horizontal_integer_value |
      			1 << sym_k_vertical_integer_value |
      			1 << sym_k_float_value |
			1 << sym_k_horizontal_float_value |
			1 << sym_k_vertical_float_value | 
                        1 << sym_k_single_float_value, 
    /* left shift */	1 << sym_k_integer_value,
    /* right shift */	1 << sym_k_integer_value,
    /* and */		1 << sym_k_bool_value |
                        1 << sym_k_integer_value |
			1 << sym_k_char_8_value | 
			1 << sym_k_localized_string_value | 
			1 << sym_k_compound_string_value |
			1 << sym_k_localized_string_value, 
    /* xor */		1 << sym_k_bool_value | 1 << sym_k_integer_value, 
    /* or */		1 << sym_k_bool_value | 1 << sym_k_integer_value, 
    /* cat */		1 << sym_k_char_8_value | 
			1 << sym_k_compound_string_value |
			1 << sym_k_localized_string_value,
    /* valref */	0xFFFFFFFF,
    /* coerce */	0xFFFFFFFF
				  };
static char	*operator_symbol[ ] = {
    /* unused */	"** OPERATOR ERROR**",
    /* not */		"not operator",
    /* unary plus */	"unary plus operator",
    /* unary minus */	"unary minus operator",
    /* comp str */	"compound string function",
    /* wchar str */	"wide_character string function",
    /* multiply */	"multiply operator",
    /* divide */	"divide operator",
    /* add */		"add operator",
    /* subtract */	"subtract operator",
    /* left shift */	"left shift operator",
    /* right shift */	"right shift operator",
    /* and */		"and operator",
    /* xor */		"exclusive or operator",
    /* or */		"or operator",
    /* cat */		"concatenate operator",
    /* coerce */	"coerce operator",
				  };

static char	*value_text[ ] = {
    /* boolean */	"boolean expression",
    /* integer */	"integer expression",
    /* float */		"floating point expression",
			};

static sym_argument_entry_type		**arg_seen;
static sym_callback_entry_type		**reason_seen;




/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function walks the entire parse tree for the input, and
**	performs semantic validation. It guarantees type matching,
**	that arguments and controls and legal, etc.
**
**  FORMAL PARAMETERS:
**
**      none
**
**  IMPLICIT INPUTS:
**
**      sym_az_root_entry
**
**  IMPLICIT OUTPUTS:
**
**      none
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**      error messages may be issued for objects that are still undefined
**	or of the wrong type
**
**--
**/

void	sem_validation ()
{

/*
 * Allocate storage if required
 */
if ( arg_seen == NULL )
    arg_seen = (sym_argument_entry_type **)
	XtMalloc (sizeof(sym_argument_entry_type *)*(uil_max_arg+1));
if ( reason_seen == NULL )
    reason_seen = (sym_callback_entry_type **)
	XtMalloc (sizeof(sym_argument_entry_type *)*(uil_max_reason+1));

/*
 * Walk the parse tree, performing validation on each node which
 * requires it.
 */
sem_validate_node (( sym_entry_type *)sym_az_root_entry->sections);

}



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This routine recursively walks through the parse tree. It
**	ignores any nodes which require no pass 2 validation, and
**	calls a specialized routine for any others. It checks for
**	any requests to terminate.
**
**  FORMAL PARAMETERS:
**
**      node		current parse tree node
**
**  IMPLICIT INPUTS:
**
**      >
**
**  IMPLICIT OUTPUTS:
**
**      >
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**      may terminate processing
**
**--
**/

void sem_validate_node (node)
    sym_entry_type		*node;

{

/*
 * Local variables
 */
sym_value_entry_type		*value_node;
sym_widget_entry_type		*widget_node;
sym_widget_entry_type		*child_node;
sym_list_entry_type		*list_node;
sym_include_file_entry_type	*ifile_node;
sym_section_entry_type		*section_node;
sym_obj_entry_type		*entry_node;
sym_obj_entry_type		*list_member;
sym_nested_list_entry_type	*nested_list_entry;
sym_control_entry_type		*control_entry;

/*
 * Call the status callback routine to report progress and check status
 */
/* %COMPLETE */
Uil_percent_complete = 80;
if ( Uil_cmd_z_command.status_cb != (Uil_continue_type(*)())NULL )
    diag_report_status ();

/*
 * Switch on the node type for validation and recursion.
 */
if ( node == NULL ) return;
switch ( node->header.b_tag )
    {
    case sym_k_value_entry:
        value_node = (sym_value_entry_type *) node;
	sem_validate_value_node (value_node);
	break;
    case sym_k_widget_entry:
    case sym_k_gadget_entry:
    case sym_k_child_entry:
	widget_node = (sym_widget_entry_type *) node;
	sem_validate_widget_node (widget_node);

	/*
	 * Recurse for children and validate all children. Duplicate
	 * validation will not occur since sem_validate_widget_node checks
	 * bflags for validated flag.
	 */
 	sem_validate_node (( sym_entry_type *)widget_node->az_controls);
	break;
    case sym_k_list_entry:
	/*
	 * recursive entry point for processing controls lists.
	 */
	list_node = (sym_list_entry_type *) node;
	if ( list_node->header.b_type != sym_k_control_list )
	    break;
	for (list_member=(sym_obj_entry_type *)list_node->obj_header.az_next;
	     list_member!=NULL;
	     list_member=(sym_obj_entry_type *)list_member->obj_header.az_next)
	    switch ( list_member->header.b_tag )
		{
		case sym_k_nested_list_entry:
		    nested_list_entry =
			(sym_nested_list_entry_type *) list_member;
		    sem_validate_node (( sym_entry_type *)nested_list_entry->az_list);
		    break;
		case sym_k_control_entry:
		    control_entry = (sym_control_entry_type *) list_member;
		    child_node = (sym_widget_entry_type *)
			control_entry->az_con_obj;
		    sem_validate_node (( sym_entry_type *)child_node);
		break;
		}
	break;
    case sym_k_include_file_entry:
	ifile_node = (sym_include_file_entry_type *) node;
	sem_validate_node (( sym_entry_type *)ifile_node->sections);
	break;
    case sym_k_section_entry:
	section_node = (sym_section_entry_type *) node;
	sem_validate_node (( sym_entry_type *)section_node->next);
	switch ( section_node->header.b_type )
	    {
	    case sym_k_section_tail:
		break;
	    default:
	        entry_node = (sym_obj_entry_type *) section_node->entries;
		sem_validate_node (( sym_entry_type *)entry_node);
		break;
	    }
	break;
    }

}



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This routine validates a value node
**
**  FORMAL PARAMETERS:
**
**      value_node		the symbol table node to be validated.
**
**  IMPLICIT INPUTS:
**
**  IMPLICIT OUTPUTS:
**
**  FUNCTION VALUE:
**
**      pointer to the value node resulting from the operation (may be
**      different from input)
**
**  SIDE EFFECTS:
**
**      error reporting
**
**--
**/

sym_value_entry_type *sem_validate_value_node (value_node)
    sym_value_entry_type	*value_node;

{

/*
 * Local variables
 */


/*
 * Both evaluation and validation are done by the value evaluation routine
 */
if ( value_node == NULL )
    return NULL;
if ( value_node->obj_header.b_flags & sym_m_validated )
    return value_node;

sem_evaluate_value (value_node);
value_node->obj_header.b_flags |= sym_m_validated;
return value_node;
}



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This routine validates a widget node
**
**  FORMAL PARAMETERS:
**
**      widget_node		the symbol table node to be validated.
**
**  IMPLICIT INPUTS:
**
**  IMPLICIT OUTPUTS:
**
**  FUNCTION VALUE:
**
**  SIDE EFFECTS:
**
**      error reporting
**
**--
**/

void sem_validate_widget_node (widget_node)
    sym_widget_entry_type	*widget_node;

{

/*
 * Local variables
 */
unsigned int			widget_type;
sym_list_entry_type		*list_entry;	/* for various lists */

/*
 * if this widget has already been validated just return
 */
if (widget_node->obj_header.b_flags & sym_m_validated)
    return;

/*
 * Pick up widget parameters
 */
if (widget_node->header.b_tag == sym_k_child_entry)
  widget_type = child_class_table[widget_node->header.b_type];
else widget_type = widget_node->header.b_type;

/*
 * Validate the arguments. Each argument in the list is validated
 * by an argument validation routine.
 */
if ( widget_node->az_arguments != NULL )
    {
    int			ndx;
    for ( ndx=0 ; ndx<uil_max_arg+1 ; ndx++ )
	arg_seen[ndx] = 0;
    sem_validate_argument_list (widget_node, widget_type,
				widget_node->az_arguments, arg_seen);
    }
	
/*
 * Validate the callbacks. Each callback is validated by a validation
 * routine
 */
if ( widget_node->az_callbacks != NULL )
    {
    int			ndx;
    for ( ndx=0 ; ndx<uil_max_reason+1 ; ndx++ )
	reason_seen[ndx] = 0;
    sem_validate_callback_list (widget_node, widget_type,
				widget_node->az_callbacks, reason_seen);
    }

/*
 * Validate the controls. Each is validated by a validation routine.
 * Also check the node for cycles.
 */
if ( widget_node->az_controls != NULL )
    {
    int					gadget_count = 0;

    list_entry = (sym_list_entry_type *) widget_node->az_controls;
    sem_validate_control_list (widget_node, widget_type,
			       list_entry, &gadget_count);
    list_entry->w_gadget_count = gadget_count;
    sem_validate_widget_cycle (list_entry, widget_node->obj_header.az_name);
    }

/*
 * Mark the widget as validated
 */
widget_node->obj_header.b_flags |= sym_m_validated;
}



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This routine validates all the arguments in an argument list.
**	It recurse down nested lists.
**
**  FORMAL PARAMETERS:
**
**      widget_node	the current widget
**	widget_type	the current widget's type
**	list_entry	list to be validated
**	seen  	        flag table to detect duplicate arguments
**
**  IMPLICIT INPUTS:
**
**  IMPLICIT OUTPUTS:
**
**  FUNCTION VALUE:
**
**  SIDE EFFECTS:
**
**--
**/

void sem_validate_argument_list	(widget_node, widget_type, list_entry, seen)
    sym_widget_entry_type		*widget_node;
    unsigned int			widget_type;
    sym_list_entry_type			*list_entry;
    sym_argument_entry_type		**seen;

{

/*
 * Local variables
 */
sym_obj_entry_type		*list_member;
sym_nested_list_entry_type	*nested_list_entry;
sym_argument_entry_type		*argument_entry;
/* For fixing DTS 9540 */
static    int nest_count=0; 
static    sym_list_entry_type *nest_head = NULL;


/*
 * loop down the list
 */
if ( list_entry == NULL ) return;
for (list_member=(sym_obj_entry_type *)list_entry->obj_header.az_next;
     list_member!=NULL;
     list_member=(sym_obj_entry_type *)list_member->obj_header.az_next)
    switch ( list_member->header.b_tag )
	{
	case sym_k_nested_list_entry:
	    nested_list_entry = (sym_nested_list_entry_type *) list_member;
            /* Begin fixing DTS 9540 */
            if(!nest_count)
               nest_head = nested_list_entry->az_list;
            nest_count++;
            if(nest_count == 1 || nest_head != nested_list_entry->az_list){
	    	sem_validate_argument_list (widget_node, widget_type,
					nested_list_entry->az_list, seen);
                nest_count--;
            }else
    		diag_issue_diagnostic
			(d_circular_ref,
	 		_sar_source_pos2(list_entry),
	 		"argument name");
            /* End fixing DTS 9540 */
	    break;
	case sym_k_argument_entry:
	    argument_entry = (sym_argument_entry_type *) list_member;
	    sem_validate_argument_entry	(widget_node, widget_type,
					 list_entry, argument_entry, seen);
	    break;
	default:
	    diag_issue_diagnostic
	        ( d_list_item,
	          _sar_source_pos2 ( list_entry ),
	          diag_tag_text (sym_k_argument_entry),
	          diag_tag_text (list_entry->header.b_type),
	          diag_tag_text (list_entry->header.b_tag) );
	}

}



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This routine performs validation for a single argument entry
**	for the current widget node.
**
**  FORMAL PARAMETERS:
**
**      widget_node	the current widget
**	widget_type	the current widget's type
**	list_entry	list entry for current argument entry
**	argument_entry	the current argument entry
**	seen 	       flag table to detect duplicate arguments
**
**  IMPLICIT INPUTS:
**
**
**  IMPLICIT OUTPUTS:
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**      error reporting
**
**--
**/

void sem_validate_argument_entry
	(widget_node, widget_type, list_entry, argument_entry, seen)
    sym_widget_entry_type		*widget_node;
    unsigned int			widget_type;
    sym_list_entry_type			*list_entry;
    sym_argument_entry_type		*argument_entry;
    sym_argument_entry_type		**seen;

{

/*
 * Local variables
 */
sym_value_entry_type		*argname_value_entry;
sym_value_entry_type		*argvalue_value_entry;
key_keytable_entry_type		*keytable_entry;
sym_argument_entry_type		**seen_entry;
boolean				supported_flag;
unsigned char			expected_type, actual_type, actual_tag;
boolean				valid_value;


/*
 * ignore error entries, consistency check
 */
if ( argument_entry->header.b_tag == sym_k_error_entry ) return;
_assert (argument_entry->header.b_tag==sym_k_argument_entry,
	 "unexpected non argument entry");

/*
 * Validate and evaluate the argument name and argument value entries.
 */

sem_validate_node (( sym_entry_type *)argument_entry->az_arg_name);

/*
 * There is no need to validate the value if it is a widget since widgets are
 * validated elsewhere and you can't define widgets in an argument list anyway.
 */

if ( argument_entry->az_arg_value == NULL )
    return;

if ((argument_entry->az_arg_value->header.b_tag != sym_k_widget_entry) &&
    (argument_entry->az_arg_value->header.b_tag != sym_k_gadget_entry))
    sem_validate_node (( sym_entry_type *)argument_entry->az_arg_value);

argname_value_entry = (sym_value_entry_type *) argument_entry->az_arg_name;
if ( argname_value_entry == NULL )
    {
    diag_issue_diagnostic
	(d_bad_argument,
	 _sar_source_pos2(argument_entry),
	 "argument name");
    return;
    }

sem_evaluate_value_expr(argname_value_entry);
_assert (argname_value_entry->header.b_tag==sym_k_value_entry,
	 "invalid argument name value_entry");
if (argname_value_entry->b_type != sym_k_argument_value)
    {
    diag_issue_diagnostic
	(d_list_item,
	 _sar_source_pos2(argname_value_entry),
	 diag_value_text(argname_value_entry->b_type),
	 diag_tag_text(list_entry->header.b_type),
	 diag_tag_text(list_entry->header.b_tag));
    return;
    }

argvalue_value_entry = (sym_value_entry_type *) argument_entry->az_arg_value;

/* BEGIN HAL Fix CR 3857 */
if ((argument_entry->az_arg_value->header.b_tag != sym_k_widget_entry) &&
    (argument_entry->az_arg_value->header.b_tag != sym_k_gadget_entry))
/* END HAL Fix CR 3857 */
  sem_evaluate_value_expr(argvalue_value_entry);

/*
 * Check for unsupported arguments. Validate constraints.
 * This check is required for known toolkit arguments in
 * toolkit widgets.
 */
if ( (argname_value_entry->obj_header.b_flags&sym_m_builtin) &&
     (widget_type!=uil_sym_user_defined_object) &&
     (argname_value_entry->obj_header.az_name == NULL) )
    {
    /*
     * Pick up the token keytable entry for the argument.
     * Fork on whether it is a constraint argument
     */
    keytable_entry = (key_keytable_entry_type *)
	argname_value_entry->value.az_data;
    _assert (keytable_entry->b_class==tkn_k_class_argument,
	     "unexpected non-argument keytable entry");
    if ( _constraint_check(keytable_entry->b_subclass) )
	sem_validate_constraint_entry (widget_node, argument_entry, widget_type);
    else
	{
	supported_flag = sem_argument_allowed
	    (keytable_entry->b_subclass, widget_type);
	if ( ! supported_flag )
	    diag_issue_diagnostic
		(d_unsupported,
		 _sar_source_pos2(argument_entry),
		 keytable_entry->at_name,
		 diag_tag_text(argument_entry->header.b_tag),
		 diag_object_text(widget_type));
	}

    /*
     * Check for duplicate arguments. A warning is issued, but the
     * argument is not removed from the list, since it may occur in
     * an argument list - and the argument list may be referenced in
     * another context where this argument is not duplicated.
     */
    seen_entry = (sym_argument_entry_type **)
	&seen[keytable_entry->b_subclass];
    if ( *seen_entry != NULL )
	{
	diag_issue_diagnostic
	    (d_supersede,
	     _sar_source_pos2(argument_entry),
	     keytable_entry->at_name,
	     diag_tag_text(argument_entry->header.b_tag),
	     diag_tag_text(list_entry->header.b_type),
	     diag_tag_text(list_entry->header.b_tag));
	}
    else
	{
	*seen_entry = argument_entry;
	}

    /*
     * Make sure that any enumeration value reference is valid.
     */
    sem_validate_argument_enumset (argument_entry,
				   keytable_entry->b_subclass,
				   argvalue_value_entry);
    }

/*
**  Verify the value type for this argument, if it is a
**  built-in argument or a user_defined argument. Check for
**  proper enumeration set match
*/
if ( (argname_value_entry->obj_header.b_flags & sym_m_builtin) &&
     (argname_value_entry->obj_header.az_name == NULL) ) 
    {
    key_keytable_entry_type		* keytable_entry;

    keytable_entry =
	(key_keytable_entry_type *) argname_value_entry->value.az_data;
    _assert (keytable_entry->b_class==tkn_k_class_argument,
	     "name is not an argument");
    expected_type = argument_type_table[keytable_entry->b_subclass];
    }
else 
    expected_type = argname_value_entry->b_arg_type;

/*
** Argument value validation
**
** Acquire and evaluate argument value.
** Allow a widget reference as the value of an argument.  NOTE: gadgets
** are not allowed as argument values, only controls.
**
** This entry may be absent due to extensive compilation errors.
*/
if ( argvalue_value_entry == NULL ) return;

actual_tag = argvalue_value_entry->header.b_tag;
switch ( actual_tag )
    {
    case sym_k_value_entry:
        actual_type = argvalue_value_entry->b_type;
	break;
    case sym_k_widget_entry:
	actual_type = sym_k_widget_ref_value;
	break;
    default:
	_assert (FALSE, "value entry missing");    
	break;
    }
valid_value = (actual_type == expected_type);

/*
** Coerce actual_type to expected_type for certain special cases.
** We'll do this by creating a new value node for the coerced value with
** the coerce unary operator set.  We'll actually perform the coersion
** operation here and set the flag indicating that expression evaluation
** has already taken place.
*/
if ( expected_type == sym_k_any_value )
    valid_value = TRUE;
if ( actual_type == sym_k_any_value )
    valid_value = TRUE;
if ( actual_type == sym_k_identifier_value )
    valid_value = TRUE;
if (( expected_type == sym_k_pixmap_value ) &&
    ( actual_type == sym_k_icon_value ))
    valid_value = TRUE;
if (( expected_type == sym_k_pixmap_value ) &&
    ( actual_type == sym_k_xbitmapfile_value ))
    valid_value = TRUE;
if (( expected_type == sym_k_color_value) &&      /* RAP rgb data type */
    ( actual_type == sym_k_rgb_value))
    valid_value = TRUE;
/*  BEGIN HaL fix CR 5429 */ 
if (( expected_type == sym_k_class_rec_name_value) &&      
    ( actual_type == sym_k_class_rec_name_value))
    valid_value = TRUE;
/*  END HaL fix CR 5429 */ 
/* For boolean values converted to enums */
if ((expected_type == sym_k_integer_value) &&      
    (actual_type == sym_k_bool_value))
    valid_value = TRUE;
if (( expected_type == sym_k_char_8_value ) &&
    ( actual_type == sym_k_localized_string_value ))
    valid_value = TRUE;
if (( expected_type == sym_k_compound_string_value ) &&
    ( actual_type == sym_k_char_8_value ))
    {
    valid_value = TRUE;
    if ( (argvalue_value_entry->obj_header.b_flags & sym_m_private) != 0)
	{
	sym_value_entry_type   *cstr_value;
	
	cstr_value = (sym_value_entry_type *) sem_create_cstr();
	cstr_value->b_expr_opr = sym_k_coerce_op;
	cstr_value->az_exp_op1 = argvalue_value_entry;
	sem_evaluate_value_expr (cstr_value);
	argument_entry->az_arg_value = cstr_value;
	}
    }
if (( expected_type == sym_k_compound_string_value ) &&
    ( actual_type == sym_k_localized_string_value ))
    {
    valid_value = TRUE;
    if ( (argvalue_value_entry->obj_header.b_flags & sym_m_private) != 0)
	{
	sym_value_entry_type   *cstr_value;
	
	cstr_value = (sym_value_entry_type *) sem_create_cstr();
	cstr_value->b_expr_opr = sym_k_coerce_op;
	cstr_value->az_exp_op1 = argvalue_value_entry;
	sem_evaluate_value_expr (cstr_value);
	argument_entry->az_arg_value = cstr_value;
	}
    }
if (( expected_type == sym_k_wchar_string_value ) &&
    ( actual_type == sym_k_localized_string_value ))
    {
    valid_value = TRUE;
    if ( (argvalue_value_entry->obj_header.b_flags & sym_m_private) != 0)
	{
	sym_value_entry_type   *wcstr_value;
	
	wcstr_value = (sym_value_entry_type *) sem_create_wchar_str();
	wcstr_value->b_expr_opr = sym_k_coerce_op;
	wcstr_value->az_exp_op1 = argvalue_value_entry;
	sem_evaluate_value_expr (wcstr_value);
	argument_entry->az_arg_value = wcstr_value;
	}
    }
if (( expected_type == sym_k_font_table_value ) &&
    (( actual_type == sym_k_font_value ) ||
     ( actual_type == sym_k_fontset_value )))
    {
    valid_value = TRUE;
    if ( (argvalue_value_entry->obj_header.b_flags & sym_m_private) != 0)
	{
        sym_value_entry_type	*font_table;

        font_table = 
	    sem_create_value_entry
	        ((char*)&argvalue_value_entry, sizeof(long),
		 sym_k_font_table_value);
        font_table->b_table_count = 1;
        font_table->az_first_table_value = argvalue_value_entry;
        font_table->b_expr_opr = sym_k_coerce_op;
        font_table->az_exp_op1 = argvalue_value_entry;
        font_table->b_aux_flags |= sym_m_exp_eval;
        argument_entry->az_arg_value = sem_evaluate_value (font_table);
	}
    }
if (( expected_type == sym_k_keysym_value ) &&
    ( actual_type == sym_k_integer_value ))
    {
/*
 * If an integer is incountered when expecting a keysym then just make the 
 * integer into a keysym if the integer is private. If it isn't give an error 
 * message because Mrm won't be able to handle that problem.
 * When allocating the space for c_value the size of the string is one since an
 * integer can only be one character.
 */
    if ( (argvalue_value_entry->obj_header.b_flags & sym_m_private) != 0)
	{
	char	   tmp;

	valid_value = TRUE;
	tmp = argument_entry->az_arg_value->value.l_integer;
	argument_entry->az_arg_value->value.c_value = (char *) XtCalloc(1,2);
	/* 
	 * This is a very strange move. While we Calloc 2 bytes we only need to move
	 * one of those bytes. We calloc 2 bytes for a null termination so HP type 
	 * machines will work. It looks wierd but it works.
	 */
	_move (argument_entry->az_arg_value->value.c_value, &tmp, 1);
	argument_entry->az_arg_value->b_type = sym_k_keysym_value;
	argument_entry->az_arg_value->w_length = 1;
	}
    }

if ((( expected_type == sym_k_horizontal_integer_value ) ||
     ( expected_type == sym_k_vertical_integer_value ))
    &&
    (( actual_type == sym_k_integer_value) ||
     ( actual_type == sym_k_float_value)))
    {
	/* If the expected type was either a horizontal or
	   vertical integer and we got an integer, then simply make
	   the actual type become the expected type. */
	valid_value = TRUE;
	argument_entry->az_arg_value->b_type = expected_type;
	/* If the actual_type is a float then coerce it into
	   being an integer */
	if (actual_type == sym_k_float_value)
	  argument_entry->az_arg_value->value.l_integer =
	    (long) argument_entry->az_arg_value->value.d_real;
	/* XmPIXELS currently has a value of 0 so the following
	   isn't really necessary but I suppose it is more robust. */
	if (argument_entry->az_arg_value->b_arg_type == 0)
	  argument_entry->az_arg_value->b_arg_type = XmPIXELS;
    }

if ((( expected_type == sym_k_horizontal_float_value ) ||
     ( expected_type == sym_k_vertical_float_value ))
    &&
    (( actual_type == sym_k_horizontal_integer_value ) ||
     ( actual_type == sym_k_vertical_integer_value )))
    {
	/* If the expected type was either a horizontal or
	   vertical float and we got a horizontal or
	   vertical integer, then make the actual type become
	   a horizontal or vertical integer, respectively. */
	valid_value = TRUE;
	if (expected_type == sym_k_horizontal_float_value)
	  argument_entry->az_arg_value->b_type =
	    sym_k_horizontal_integer_value;
	else if (expected_type == sym_k_vertical_float_value)
	  argument_entry->az_arg_value->b_type = sym_k_vertical_integer_value;
	/* Coerce the value into being a float */
	argument_entry->az_arg_value->value.d_real =
	  (double) argument_entry->az_arg_value->value.l_integer;
	/* XmPIXELS currently has a value of 0 so the following
	   isn't really necessary but I suppose it is more robust. */
	if (argument_entry->az_arg_value->b_arg_type == 0)
	  argument_entry->az_arg_value->b_arg_type = XmPIXELS;
    }

if ((( expected_type == sym_k_horizontal_integer_value ) ||
     ( expected_type == sym_k_vertical_integer_value ))
    &&
    (( actual_type == sym_k_horizontal_float_value ) ||
     ( actual_type == sym_k_vertical_float_value )))
    {
	/* If the expected type was either a horizontal or
	   vertical integer and we got a horizontal or vertical
	   float, then make the actual type become a horizontal
	   or vertical float, respectively. */
	valid_value = TRUE;
	if (expected_type == sym_k_horizontal_integer_value)
	  argument_entry->az_arg_value->b_type = sym_k_horizontal_float_value;
	else if (expected_type == sym_k_vertical_integer_value)
	  argument_entry->az_arg_value->b_type = sym_k_vertical_float_value;
	/* Coerce the value into being an integer */
	argument_entry->az_arg_value->value.l_integer =
	  (long) argument_entry->az_arg_value->value.d_real;
	/* XmPIXELS currently has a value of 0 so the following
	   isn't really necessary but I suppose it is more robust. */
	if (argument_entry->az_arg_value->b_arg_type == 0)
	  argument_entry->az_arg_value->b_arg_type = XmPIXELS;
    }

if ((( expected_type == sym_k_horizontal_float_value ) ||
     ( expected_type == sym_k_vertical_float_value ))
    &&
    (( actual_type == sym_k_integer_value) ||
     ( actual_type == sym_k_float_value)))
    {
	/* If the expected type was either a horizontal or
	   vertical float and we got a float, then simply make
	   the actual type become the expected type. */
	valid_value = TRUE;
	argument_entry->az_arg_value->b_type = expected_type;
	/* If actual_type is an integer, then coerce into being
	   a float */
	if (actual_type == sym_k_integer_value)
	  argument_entry->az_arg_value->value.d_real =
	    (double) argument_entry->az_arg_value->value.l_integer;
	/* XmPIXELS currently has a value of 0 so the following
	   isn't really necessary but I suppose it is more robust. */
	if (argument_entry->az_arg_value->b_arg_type == 0)
	  argument_entry->az_arg_value->b_arg_type = XmPIXELS;
    }

/* It is also possible for us to encounter a horizontal float or
   int when we are expecting a vertical float or int. When using
   'value' defined constants, we coerce the type of the constant
   the first time it is used. We have to be able to accept the
   coerced value in a different context later. */
if ((( expected_type == sym_k_horizontal_float_value ) &&
     ( actual_type == sym_k_vertical_float_value ))
    ||
    (( expected_type == sym_k_horizontal_integer_value ) &&
     ( actual_type == sym_k_vertical_integer_value )))
{
    /* Leave the actual type and value alone, but flag it as
       an acceptable value. This will mean that the first orientation
       the value is coerced to will be the orientation for all
       occurences of the value. */
    valid_value = TRUE;
}

if (!valid_value)
    diag_issue_diagnostic
	(d_obj_type,
	 _sar_source_pos2(argname_value_entry),
	 diag_value_text(actual_type),
	 diag_tag_text(actual_tag),
	 diag_value_text(expected_type),
	 diag_tag_text(sym_k_value_entry) );

}



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This routine performs enumeration set validation for a single
**	argument entry. If it's value is an enumeration set reference,
**	then we verify that the value comes from the argument's supported
**	enumeration set.
**
**  FORMAL PARAMETERS:
**
**	argument_entry	the current argument entry
**	arg_code	the sym_k_..._arg code for the argument
**	arg_value_entry	the value node for the argument value
**
**  IMPLICIT INPUTS:
**
**  IMPLICIT OUTPUTS:
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**      error reporting
**
**--
**/

void sem_validate_argument_enumset
	(argument_entry, arg_code, arg_value_entry)
    sym_argument_entry_type		*argument_entry;
    int					arg_code;
    sym_value_entry_type		*arg_value_entry;

{

/*
 * Local variables
 */
unsigned short int	enumval_code;
unsigned short int	enumset_code;
int			ndx;


/*
 * No action if value is not an integer enumeration value. Else:
 *	- argument must support enumeration set
 *	- value must be from set
 */
if ( arg_value_entry == NULL ) return;
if ((arg_value_entry->b_type != sym_k_integer_value) &&
    (arg_value_entry->b_type != sym_k_bool_value)) return;
enumval_code = arg_value_entry->b_enumeration_value_code;
if ( enumval_code == 0 ) return;
enumset_code = argument_enumset_table[arg_code];
if ( enumset_code == 0 )
  {
    if (arg_value_entry->b_type != sym_k_bool_value)
      diag_issue_diagnostic(d_no_enumset,
			    _sar_source_pos2(argument_entry),
			    uil_argument_names[arg_code]);
    return;
  }
for ( ndx=0 ; ndx<enum_set_table[enumset_code].values_cnt ; ndx++ )
    if ( enum_set_table[enumset_code].values[ndx] == enumval_code ) return;
diag_issue_diagnostic (d_invalid_enumval,
		       _sar_source_pos2(argument_entry),
		       uil_argument_names[arg_code],
		       uil_enumval_names[enumval_code]);
return;

}



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This routine validates a constraint entry. It checks to make
**	sure that the constraint reference is valid in each of the
**	parents of the widget using the constraint
**
**  FORMAL PARAMETERS:
**
**      widget_node	the widget using the constraint
**	argument_entry	the constraint argument
**
**  IMPLICIT INPUTS:
**
**      >
**
**  IMPLICIT OUTPUTS:
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**      error reporting
**
**--
**/

void sem_validate_constraint_entry (widget_node, argument_entry, widget_type)
    sym_widget_entry_type		*widget_node;
    sym_argument_entry_type		*argument_entry;
    unsigned int			widget_type;
{

/*
 * Local variables
 */
sym_parent_list_type		*parent_entry;
sym_widget_entry_type		*parent_object;
unsigned int			parent_type;
unsigned int			parent_tag;
key_keytable_entry_type		*keytable_entry;
sym_value_entry_type		*arg_name_entry;
boolean				supported_flag;


/*
 * Validate the constraint with each of the referencing widget's parents
 */
for (parent_entry=widget_node->parent_list;
     parent_entry!=NULL;
     parent_entry=parent_entry->next)
    {

    /*
     * Acquire the parent object and its type
     */
    parent_object = parent_entry->parent;
    parent_type = parent_object->header.b_type;
    if ( parent_object->obj_header.b_flags & sym_m_obj_is_gadget )
	{
	parent_tag = sym_k_gadget_entry;
	}
    else
	{
	parent_tag = sym_k_widget_entry;
	}

    /*
     * Acquire the appropriate pointers, and validate the reference
     */
    arg_name_entry = (sym_value_entry_type *)argument_entry->az_arg_name;
    keytable_entry = (key_keytable_entry_type *)arg_name_entry->value.az_data;
    supported_flag = sem_argument_allowed(keytable_entry->b_subclass,
					  parent_type);
    if (!supported_flag)
      {
	/* Check for both argument and constraint, e.g. decimalPoints. */
	supported_flag = sem_argument_allowed(keytable_entry->b_subclass,
					      widget_type);
	if (!supported_flag)
	  diag_issue_diagnostic(d_unsupp_const,
				_sar_source_pos2(argument_entry),
				keytable_entry->at_name,
				diag_object_text(parent_type),
				diag_tag_text(parent_tag));
      }
  }

/*
 * Checks on nodes pointed to by this node
 *
 * There is no need to validate the value if it is a widget since widgets are
 * validated elsewhere and you can't define widgets in an argument list anyway.
 */

if ((argument_entry->az_arg_value->header.b_tag != sym_k_widget_entry) &&
    (argument_entry->az_arg_value->header.b_tag != sym_k_gadget_entry))
    sem_validate_node (( sym_entry_type *)argument_entry->az_arg_value);

}



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This routine validates all the callbacks in an callback list.
**	It recurse down nested lists.
**
**  FORMAL PARAMETERS:
**
**      widget_node	the current widget
**	widget_type	the current widget's type
**	list_entry	list to be validated
**	seen  	        flag table to detect duplicate callbacks
**
**  IMPLICIT INPUTS:
**
**  IMPLICIT OUTPUTS:
**
**  FUNCTION VALUE:
**
**  SIDE EFFECTS:
**
**--
**/

void sem_validate_callback_list	(widget_node, widget_type, list_entry, seen)
    sym_widget_entry_type		*widget_node;
    unsigned int			widget_type;
    sym_list_entry_type			*list_entry;
    sym_callback_entry_type		**seen;

{

/*
 * Local variables
 */
sym_obj_entry_type		*list_member;
sym_nested_list_entry_type	*nested_list_entry;
sym_callback_entry_type		*callback_entry;


/*
 * loop down the list
 */
if ( list_entry == NULL ) return;
for (list_member=(sym_obj_entry_type *)list_entry->obj_header.az_next;
     list_member!=NULL;
     list_member=(sym_obj_entry_type *)list_member->obj_header.az_next)
    switch ( list_member->header.b_tag )
	{
	case sym_k_nested_list_entry:
	    nested_list_entry = (sym_nested_list_entry_type *) list_member;
	    sem_validate_callback_list (widget_node, widget_type,
					nested_list_entry->az_list, seen);
	    break;
	case sym_k_callback_entry:
	    callback_entry = (sym_callback_entry_type *) list_member;
	    sem_validate_callback_entry	(widget_node, widget_type,
					 list_entry, callback_entry, seen);
	    break;
	default:
	    diag_issue_diagnostic
	        ( d_list_item,
	          _sar_source_pos2 ( list_entry ),
	          diag_tag_text (sym_k_callback_entry),
	          diag_tag_text (list_entry->header.b_type),
	          diag_tag_text (list_entry->header.b_tag) );
	}

}



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This routine performs validation for a single callback entry
**	for the current widget node.
**
**  FORMAL PARAMETERS:
**
**      widget_node	the current widget
**	widget_type	the current widget's type
**	list_entry	list entry for current callback entry
**	callback_entry	the current callback entry
**	seen 	 	flag table to detect duplicate callbacks
**
**  IMPLICIT INPUTS:
**
**  IMPLICIT OUTPUTS:
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**      error reporting
**
**--
**/

void sem_validate_callback_entry
	(widget_node, widget_type, list_entry, callback_entry, seen)
    sym_widget_entry_type		*widget_node;
    unsigned int			widget_type;
    sym_list_entry_type			*list_entry;
    sym_callback_entry_type		*callback_entry;
    sym_callback_entry_type		**seen;

{

/*
 * Local variables
 */
sym_value_entry_type		*reason_value_entry;
key_keytable_entry_type		*keytable_entry;
sym_callback_entry_type		**seen_entry;
boolean				supported_flag;
static sym_value_entry_type     *widget_az_arg_value = NULL; 


/*
 * ignore error entries, consistency check
 */
if ( callback_entry->header.b_tag == sym_k_error_entry ) return;
_assert (callback_entry->header.b_tag==sym_k_callback_entry,
	 "unexpected non callback entry");

reason_value_entry = (sym_value_entry_type *)
    callback_entry->az_call_reason_name;

/*
 * Force expression evaluation
 */
sem_evaluate_value_expr (reason_value_entry);

if (reason_value_entry == NULL)
    return;

_assert (reason_value_entry->header.b_tag == sym_k_value_entry,
	 "reason value entry missing");

if (reason_value_entry->b_type != sym_k_reason_value) {
    diag_issue_diagnostic
	( d_list_item,
	  _sar_source_pos2 ( reason_value_entry ),
	  diag_value_text (reason_value_entry->b_type),
	  diag_tag_text (list_entry->header.b_type),
	  diag_tag_text (list_entry->header.b_tag) );
    return;
    }

/*
 * Check for unsupported callbacks.
 * This check is required for known toolkit callbacks in
 * toolkit widgets.
 */
if ( (reason_value_entry->obj_header.b_flags&sym_m_builtin) &&
     (widget_type!=uil_sym_user_defined_object) )
    {
    /*
     * Pick up the token keytable entry for the callback.
     * Validate that the reason is supported
     */
    keytable_entry = (key_keytable_entry_type *)
	reason_value_entry->value.az_data;
    _assert (keytable_entry->b_class==tkn_k_class_reason,
	     "unexpected non-reason keytable entry");
    supported_flag = sem_reason_allowed
	(keytable_entry->b_subclass, widget_type);
    if ( ! supported_flag )
	diag_issue_diagnostic
	    (d_unsupported,
	     _sar_source_pos2(callback_entry),
	     keytable_entry->at_name,
	     diag_tag_text(callback_entry->header.b_tag),
	     diag_object_text(widget_type));

    /*
     * Check for duplicate callbacks. A warning is issued, but the
     * callback is not removed from the list, since it may occur in
     * an callback list - and the callback list may be referenced in
     * another context where this callback is not duplicated.
     */
    seen_entry = (sym_callback_entry_type **)
	&seen[keytable_entry->b_subclass];
    if ( *seen_entry != NULL )
	{
	diag_issue_diagnostic
	    (d_supersede,
	     _sar_source_pos2(callback_entry),
	     keytable_entry->at_name,
	     diag_tag_text(callback_entry->header.b_tag),
	     diag_tag_text(list_entry->header.b_type),
	     diag_tag_text(list_entry->header.b_tag));
	}
    else
	{
	*seen_entry = callback_entry;
	}
    }

/*
 * Checks on nodes pointed to by this node
 */
    /* Begin fixing DTS 10391 and OSF CR 8715*/
    if(callback_entry->az_call_proc_ref &&
      callback_entry->az_call_proc_ref->az_arg_value &&
      (callback_entry->az_call_proc_ref->az_arg_value->header.b_tag
        == sym_k_widget_entry || 
        callback_entry->az_call_proc_ref->az_arg_value->header.b_tag
        == sym_k_gadget_entry) && 
        widget_az_arg_value == callback_entry->az_call_proc_ref->az_arg_value){
           diag_issue_diagnostic
                        (d_circular_def,
                        _sar_source_pos2(callback_entry),
                        "callback client_data");
    }else{
        if(callback_entry->az_call_proc_ref &&
        callback_entry->az_call_proc_ref->az_arg_value &&
        (callback_entry->az_call_proc_ref->az_arg_value->header.b_tag
           == sym_k_widget_entry || 
           callback_entry->az_call_proc_ref->az_arg_value->header.b_tag
           == sym_k_gadget_entry) && !widget_az_arg_value)
	      widget_az_arg_value = callback_entry->az_call_proc_ref->az_arg_value;
	sem_validate_procref_entry (callback_entry->az_call_proc_ref);
	sem_validate_procref_list (callback_entry->az_call_proc_ref_list);
     }
    widget_az_arg_value = NULL;
    /* End fixing DTS 10391 and OSF CR 8715*/
}



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This routine validates all the controls in an control list.
**	It recurse down nested lists.
**
**  FORMAL PARAMETERS:
**
**      widget_node	the current widget
**	widget_type	the current widget's type
**	list_entry	list to be validated
**	count		to return gadget count
**
**  IMPLICIT INPUTS:
**
**  IMPLICIT OUTPUTS:
**
**  FUNCTION VALUE:
**
**  SIDE EFFECTS:
**
**--
**/

void sem_validate_control_list (widget_node, widget_type, list_entry, count)
    sym_widget_entry_type		*widget_node;
    unsigned int			widget_type;
    sym_list_entry_type			*list_entry;
    int					*count;

{

/*
 * Local variables
 */
sym_obj_entry_type		*list_member;
sym_nested_list_entry_type	*nested_list_entry;
sym_control_entry_type		*control_entry;


/*
 * loop down the list
 */
if ( list_entry == NULL ) return;
for (list_member=(sym_obj_entry_type *)list_entry->obj_header.az_next;
     list_member!=NULL;
     list_member=(sym_obj_entry_type *)list_member->obj_header.az_next)
    switch ( list_member->header.b_tag )
	{
	case sym_k_nested_list_entry:
	    nested_list_entry = (sym_nested_list_entry_type *) list_member;
	    sem_validate_control_list
		(widget_node, widget_type, nested_list_entry->az_list, count);
	    break;
	case sym_k_control_entry:
	    control_entry = (sym_control_entry_type *) list_member;
	    sem_validate_control_entry
		(widget_node, widget_type, list_entry, control_entry, count);
	    break;
	}

}



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This routine performs validation for a single control entry
**	for the current widget node.
**
**  FORMAL PARAMETERS:
**
**      widget_node	the current widget
**	widget_type	the current widget's type
**	list_entry	list entry for current control entry
**	control_entry	the current control entry
**	gadget_count	to accumulate count of controlled gadgets
**
**  IMPLICIT INPUTS:
**
**  IMPLICIT OUTPUTS:
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**      error reporting
**
**--
**/

void sem_validate_control_entry
	(widget_node, widget_type, list_entry, control_entry, gadget_count)
    sym_widget_entry_type		*widget_node;
    unsigned int			widget_type;
    sym_list_entry_type			*list_entry;
    sym_control_entry_type		*control_entry;
    int					*gadget_count;

{

/*
 * Local variables
 */
sym_widget_entry_type		*control_obj_entry;
boolean				supported_flag;


/*
 * ignore error entries, consistency check
 */
if ( control_entry->header.b_tag == sym_k_error_entry ) return;
_assert (control_entry->header.b_tag==sym_k_control_entry,
	 "unexpected non control entry");

/*
 * Similar checks for the object being controlled
 */
control_obj_entry = (sym_widget_entry_type *) control_entry->az_con_obj;
if ( control_obj_entry->header.b_tag == sym_k_error_entry )
    {
    control_entry->header.b_tag = sym_k_error_entry;
    return;
    }
_assert (control_obj_entry->header.b_tag==sym_k_widget_entry ||
	 control_obj_entry->header.b_tag==sym_k_gadget_entry ||
	 control_obj_entry->header.b_tag==sym_k_child_entry,
	 "unexpected non-control object entry");

if ( control_obj_entry->header.b_tag == sym_k_gadget_entry )
    *gadget_count += 1;

/*
 * Check for unsupported controls or automatic children.
 */
if (control_obj_entry->header.b_tag == sym_k_child_entry)
  {
    supported_flag = 
      sem_child_allowed(control_obj_entry->header.b_type, widget_type);
    if ( ! supported_flag )
      diag_issue_diagnostic
	(d_unsupported,
	 _sar_source_pos2(control_entry),
	 uil_child_names[control_obj_entry->header.b_type],
	 "automatic child",
	 diag_object_text(widget_type));
  }
else 
  {
    supported_flag = 
      sem_control_allowed(control_obj_entry->header.b_type, widget_type);
    if ( ! supported_flag )
      diag_issue_diagnostic
	(d_unsupported,
	 _sar_source_pos2(control_entry),
	 diag_object_text(control_obj_entry->header.b_type),
	 diag_tag_text(control_entry->header.b_tag),
	 diag_object_text(widget_type));
  }

}



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This routine recursively checks for cycles in a widget hierarchy.
**	A cycle is defined as the appearance of a widget's name in its
**	subtree hierarchy. A cycle is detected by setting a unique id
**	for this cycle check in each name entry encountered. If this id
**	is ever encountered again, there is a cycle.
**
**	This is the root routine of the recursion, which is responsible
**	for setting flags in the name entry.
**
**  FORMAL PARAMETERS:
**
**	list_entry	list to be validated
**	cycle_name	if non-NULL, a widget name for cycle check
**
**  IMPLICIT INPUTS:
**
**  IMPLICIT OUTPUTS:
**
**  FUNCTION VALUE:
**
**  SIDE EFFECTS:
**
**--
**/

void sem_validate_widget_cycle (list_entry, cycle_name)
    sym_list_entry_type			*list_entry;
    sym_name_entry_type			*cycle_name;

{

/*
 * Local variables
 */
boolean				cycle_res;


/*
 * Acquire a new cycle id value.Call the auxiliary recursion routine,
 * and set results in the name entry.
 */
if ( cycle_name == NULL ) return;
cycle_id += 1;
cycle_name->az_cycle_id = cycle_id;
cycle_res = sem_validate_widget_cycle_aux (list_entry, cycle_name);
cycle_name->b_flags |= sym_m_cycle_checked;
if ( cycle_res )
    cycle_name->b_flags |= sym_m_has_cycle;

}



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This routine recursively checks for cycles in a widget hierarchy.
**	A cycle is defined as the appearance of a widget's name in its
**	subtree hierarchy. This is the fully recursive auxiliary for
**	sem_validate_widget_cycle
**
**	Checking is based on the fact that for any named widget, its
**	subtree definition is fixed and can be checked exactly once. Once
**	checked, it is marked as checked, and also flagged if it contains
**	a cycle. Since any tree containing a subtree which has a cycle also
**	has a cycle, checking stops immediately if such a subtree is detected,
**	and subtrees must always be (recursively) checked in advance of
**	the current check.
**
**  FORMAL PARAMETERS:
**
**	list_entry	list to be validated
**	cycle_name	if non-NULL, a widget name for cycle check
**
**  IMPLICIT INPUTS:
**
**  IMPLICIT OUTPUTS:
**
**  FUNCTION VALUE:
**			TRUE	cycle detected
**			FALSE	no cycle detected
**
**  SIDE EFFECTS:
**
**--
**/

boolean sem_validate_widget_cycle_aux (list_entry, cycle_name)
    sym_list_entry_type			*list_entry;
    sym_name_entry_type			*cycle_name;

{

/*
 * Local variables
 */
sym_obj_entry_type		*list_member;
sym_nested_list_entry_type	*nested_list_entry;
sym_control_entry_type		*control_entry;
sym_widget_entry_type		*control_obj_entry;
sym_name_entry_type		*control_obj_name;


/*
 * loop down the list. Check for cycles in each leaf (widget) node. Note
 * we must step through the az_reference for named widgets. If a cycle is
 * ever detected, we exit (or we'll recurse forever). Note that an error
 * node returns TRUE, as if it had a cycle (which inhibits further checking).
 *
 * If we encounter a previously visited node, we may have either a cycle
 * or a legitimate multiple reference. Verify that it is a cycle, and
 * issue an error message if so. If it is not verified, it need not be
 * checked again.
 */
if ( list_entry == NULL ) return FALSE;
if ( cycle_name == NULL ) return FALSE;
if ( cycle_name->b_flags & sym_m_cycle_checked )
    return (cycle_name->b_flags&sym_m_has_cycle) == 1;

for (list_member=(sym_obj_entry_type *)list_entry->obj_header.az_next;
     list_member!=NULL;
     list_member=(sym_obj_entry_type *)list_member->obj_header.az_next)
    switch ( list_member->header.b_tag )
	{
	case sym_k_nested_list_entry:
	    nested_list_entry = (sym_nested_list_entry_type *) list_member;
	    if ( sem_validate_widget_cycle_aux
		     (nested_list_entry->az_list, cycle_name) )
		return TRUE;
	    break;
	case sym_k_control_entry:
	    control_entry = (sym_control_entry_type *) list_member;
	    control_obj_entry =
		(sym_widget_entry_type *) control_entry->az_con_obj;
	    if ( control_obj_entry->header.b_tag == sym_k_error_entry )
		return TRUE;
	    _assert (control_obj_entry->header.b_tag==sym_k_widget_entry ||
		     control_obj_entry->header.b_tag==sym_k_gadget_entry ||
		     control_obj_entry->header.b_tag==sym_k_child_entry,
		     "unexpected non-control object entry");
	    if ( control_obj_entry->obj_header.az_reference != NULL )
		control_obj_entry = (sym_widget_entry_type *)
		    control_obj_entry->obj_header.az_reference;
	    if ( control_obj_entry->az_controls == NULL )
		break;
	    control_obj_name = control_obj_entry->obj_header.az_name;
	    if ( control_obj_name != NULL )
		{
		if ( control_obj_name->az_cycle_id == cycle_id )
		    {
		    if ( sem_validate_verify_cycle
			     (control_obj_entry,
			      control_obj_entry->az_controls) )
			{
			diag_issue_diagnostic
			    (d_widget_cycle,
			     _sar_source_pos2(control_entry),
			     control_obj_name->c_text);
			control_obj_name->b_flags |= sym_m_cycle_checked;
			control_obj_name->b_flags |= sym_m_has_cycle;
			return TRUE;
			}
		    else
			{
			control_obj_name->b_flags |= sym_m_cycle_checked;
			break;
			}
		    }
		control_obj_name->az_cycle_id = cycle_id;
		}
	    if ( sem_validate_widget_cycle_aux
		(control_obj_entry->az_controls, cycle_name) )
		return TRUE;
	    break;
	}
return FALSE;

}



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine verifies that a cycle found by widget_cycle_aux is
**	really a cycle. widget_cycle_aux may have detected a legitimate
**	multiple appearance of a widget in a hierarchy which is not a cycle.
**	This routine uses a pointer-marching technique to see if the given
**	node is in a real cycle. If the cycle_obj is ever encountered in
**	the pointer march, then there is a cycle. Otherwise, the march
**	terminates.
**	
**
**  FORMAL PARAMETERS:
**
**	cycle_obj	object to be found in cycle
**	list_entry	current controls list
**
**  IMPLICIT INPUTS:
**
**  IMPLICIT OUTPUTS:
**
**  FUNCTION VALUE:
**			TRUE	cycle detected
**			FALSE	no cycle detected
**
**  SIDE EFFECTS:
**
**--
**/

boolean sem_validate_verify_cycle (cycle_obj, list_entry)
    sym_widget_entry_type		*cycle_obj;
    sym_list_entry_type			*list_entry;

{

/*
 * Local variables
 */
sym_obj_entry_type		*list_member;
sym_nested_list_entry_type	*nested_list_entry;
sym_control_entry_type		*control_entry;
sym_widget_entry_type		*control_obj_entry;


/*
 * Search all objects in the controls list, and recurse.
 * objects controlled by the current object.
 */
if ( list_entry == NULL )
    return FALSE;
for (list_member=(sym_obj_entry_type *)list_entry->obj_header.az_next;
     list_member!=NULL;
     list_member=(sym_obj_entry_type *)list_member->obj_header.az_next)
    switch ( list_member->header.b_tag )
	{
	case sym_k_nested_list_entry:
	    nested_list_entry = (sym_nested_list_entry_type *) list_member;
	    if ( sem_validate_verify_cycle
		     (cycle_obj, nested_list_entry->az_list) )
		return TRUE;
	    break;
	case sym_k_control_entry:
	    control_entry = (sym_control_entry_type *) list_member;
	    control_obj_entry =
		(sym_widget_entry_type *) control_entry->az_con_obj;
	    if ( control_obj_entry->obj_header.az_reference != NULL )
		control_obj_entry = (sym_widget_entry_type *)
		    control_obj_entry->obj_header.az_reference;
	    if ( control_obj_entry == cycle_obj )
		return TRUE;
	    if ( control_obj_entry->az_controls == NULL )
		break;
	    if ( sem_validate_verify_cycle 
		     (cycle_obj, control_obj_entry->az_controls) )
		return TRUE;
	    break;
	}
return FALSE;

}



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This routine validates all the procrefs in an procref list.
**	It recurses down nested lists.
**
**  FORMAL PARAMETERS:
**
**	list_entry	list to be validated
**
**  IMPLICIT INPUTS:
**
**  IMPLICIT OUTPUTS:
**
**  FUNCTION VALUE:
**
**  SIDE EFFECTS:
**
**--
**/

void sem_validate_procref_list (list_entry)
    sym_list_entry_type			*list_entry;

{

/*
 * Local variables
 */
sym_obj_entry_type		*list_member;
sym_nested_list_entry_type	*nested_list_entry;
sym_proc_ref_entry_type		*procref_entry;


/*
 * loop down the list
 */
if ( list_entry == NULL ) return;
for (list_member=(sym_obj_entry_type *)list_entry->obj_header.az_next;
     list_member!=NULL;
     list_member=(sym_obj_entry_type *)list_member->obj_header.az_next)
    switch ( list_member->header.b_tag )
	{
	case sym_k_nested_list_entry:
	    nested_list_entry = (sym_nested_list_entry_type *) list_member;
	    sem_validate_procref_list (nested_list_entry->az_list);
	    break;
	case sym_k_proc_ref_entry:
	    procref_entry = (sym_proc_ref_entry_type *) list_member;
	    sem_validate_procref_entry (procref_entry);
	    break;
	}

}



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This routine performs validation for a single procref entry.
**
**  FORMAL PARAMETERS:
**
**	procref_entry	the current procref entry
**
**  IMPLICIT INPUTS:
**
**  IMPLICIT OUTPUTS:
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**      error reporting
**
**--
**/

void sem_validate_procref_entry (procref_entry)
    sym_proc_ref_entry_type		*procref_entry;

{

/*
 * Local variables
 */
sym_value_entry_type    	*value_entry;
sym_proc_def_entry_type 	*proc_def_entry;
int			    	actual_arg_count;
int			    	expected_arg_type;
int			    	actual_arg_type;
int			    	arg_checking;


/*
 * ignore error entries, consistency check
 */
if ( procref_entry == NULL ) return;
if ( procref_entry->header.b_tag == sym_k_error_entry ) return;
_assert (procref_entry->header.b_tag==sym_k_proc_ref_entry,
	 "unexpected non procref entry");


    /*
    ** Validate procedure reference argument:
    **	Correct number of args
    **	Correct datatype of arg - coerce if necessary
    */

    proc_def_entry = procref_entry->az_proc_def;
    /* Could be NULL due to previous compilation errors. */
    if (proc_def_entry == NULL) return;

    /*
    **	if checking is required, check that the values
    **	agree with the parameters
    */

    arg_checking = proc_def_entry->v_arg_checking;

    value_entry = procref_entry->az_arg_value;

    if (arg_checking)
    {
	boolean	    valid_arg;

        if (value_entry == NULL)
	    {
	    actual_arg_count = 0;
	    actual_arg_type = sym_k_no_value;
	    }
        else
	    {
            sem_evaluate_value_expr(value_entry);
	    actual_arg_count = 1;
	    actual_arg_type = value_entry->b_type;
	    }

	if (actual_arg_count != proc_def_entry->b_arg_count)
	{
	    diag_issue_diagnostic
		( d_arg_count,
		  _sar_source_pos2 (procref_entry),
		  proc_def_entry->obj_header.az_name->c_text,
		  proc_def_entry->b_arg_count );

	    return;
	}

	expected_arg_type = proc_def_entry->b_arg_type;
	valid_arg = (actual_arg_type == expected_arg_type);

	if ( expected_arg_type == sym_k_any_value )
	    valid_arg = TRUE;

	if ( actual_arg_type == sym_k_identifier_value )
	    valid_arg = TRUE;

	if (( expected_arg_type == sym_k_pixmap_value ) &&
	    ( actual_arg_type == sym_k_icon_value ))
	    valid_arg = TRUE;

	if (( expected_arg_type == sym_k_color_value) &&/* RAP rgb data type */
	    ( actual_arg_type == sym_k_rgb_value))
	  valid_arg = TRUE;

	if ((expected_arg_type == sym_k_char_8_value) &&
	    (actual_arg_type == sym_k_localized_string_value))
	  valid_arg = TRUE;
	
	if (( expected_arg_type == sym_k_compound_string_value ) &&
	    ( actual_arg_type == sym_k_char_8_value ))
	{
	    valid_arg = TRUE;
	    if ( (value_entry->obj_header.b_flags & sym_m_private) != 0)
	    {
		sym_value_entry_type   *cstr_value;

		cstr_value = (sym_value_entry_type *) sem_create_cstr();
		cstr_value->b_expr_opr = sym_k_coerce_op;
		cstr_value->az_exp_op1 = value_entry;
		sem_evaluate_value_expr (cstr_value);
	    }
	}

	if (( expected_arg_type == sym_k_compound_string_value ) &&
	    ( actual_arg_type == sym_k_localized_string_value ))
	{
	    valid_arg = TRUE;
	    if ( (value_entry->obj_header.b_flags & sym_m_private) != 0)
	    {
		sym_value_entry_type   *cstr_value;

		cstr_value = (sym_value_entry_type *) sem_create_cstr();
		cstr_value->b_expr_opr = sym_k_coerce_op;
		cstr_value->az_exp_op1 = value_entry;
		sem_evaluate_value_expr (cstr_value);
	    }
	}

	if (( expected_arg_type == sym_k_wchar_string_value ) &&
	    ( actual_arg_type == sym_k_localized_string_value ))
	{
	    valid_arg = TRUE;
	    if ( (value_entry->obj_header.b_flags & sym_m_private) != 0)
	    {
		sym_value_entry_type   *wcstr_value;

		wcstr_value = (sym_value_entry_type *) sem_create_wchar_str();
		wcstr_value->b_expr_opr = sym_k_coerce_op;
		wcstr_value->az_exp_op1 = value_entry;
		sem_evaluate_value_expr (wcstr_value);
	    }
	}

	if (( expected_arg_type == sym_k_font_table_value ) &&
	    (( actual_arg_type == sym_k_font_value ) ||
	     ( actual_arg_type == sym_k_fontset_value )))
	{
	    valid_arg = TRUE;
	    if ( (value_entry->obj_header.b_flags & sym_m_private) != 0)
	    {
		sym_value_entry_type   *font_table;

		font_table = 
		    sem_create_value_entry
			( (char*)&value_entry, sizeof(long),
			 sym_k_font_table_value );
		font_table->b_expr_opr = sym_k_coerce_op;
		font_table->az_exp_op1 = value_entry;
		value_entry = sem_evaluate_value (font_table);
	    }
	}

	if ((expected_arg_type == sym_k_widget_ref_value) &&
	    (value_entry->header.b_tag == sym_k_widget_entry))
	  {
	    expected_arg_type = proc_def_entry->b_widget_type;
	    actual_arg_type = value_entry->header.b_type;
	    
	    if ((expected_arg_type > uil_max_object) ||
		(actual_arg_type == expected_arg_type))
	      {
		valid_arg = TRUE;
	      }
	    else 
	      {
		diag_issue_diagnostic
		  (d_arg_type,
		   _sar_source_pos2(procref_entry),
		   diag_object_text(actual_arg_type),
		   proc_def_entry->obj_header.az_name->c_text,
		   diag_object_text(expected_arg_type));

		return;
	      }
	  }
	
	if (!valid_arg)
	{
	    diag_issue_diagnostic
		( d_arg_type,
		  _sar_source_pos2 (procref_entry),
		  diag_value_text( actual_arg_type ),
		  proc_def_entry->obj_header.az_name->c_text,
		  diag_value_text( expected_arg_type ) );

	    return;
	}
    }

/*
 * Perform validation of tags
 */
sem_validate_node (( sym_entry_type *)procref_entry->az_arg_value);
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This predicate specifies if an argument is allowed in a class.
**
**  FORMAL PARAMETERS:
**
**      arg_code	The sym_k_..._arg code for the argument
**	class_code	The sym_k_..._object code for the class
**
**  IMPLICIT INPUTS:
**
**      allowed_argument_table
**
**  IMPLICIT OUTPUTS:
**
**  FUNCTION VALUE:
**
**      TRUE		argument is allowed
**	FALSE		argument is not allowed
**
**  SIDE EFFECTS:
**
**--
**/

boolean sem_argument_allowed (arg_code, class_code)
    unsigned int		arg_code;
    unsigned int		class_code;

{

unsigned char		*entry_vec;
unsigned char		vec_byte;

entry_vec = allowed_argument_table[arg_code];
vec_byte = entry_vec[_BIT_INDEX(class_code)];
return (boolean) vec_byte & _BIT_MASK(class_code);

}



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This predicate specifies if a reason is allowed in a class.
**
**  FORMAL PARAMETERS:
**
**      rsn_code	The sym_k_..._reason code for the reason
**	class_code	The sym_k_..._object code for the class
**
**  IMPLICIT INPUTS:
**
**      allowed_reason_table
**
**  IMPLICIT OUTPUTS:
**
**  FUNCTION VALUE:
**
**      TRUE		reason is allowed
**	FALSE		reason is not allowed
**
**  SIDE EFFECTS:
**
**--
**/

boolean sem_reason_allowed (rsn_code, class_code)
    unsigned int		rsn_code;
    unsigned int		class_code;

{

unsigned char		*entry_vec;
unsigned char		vec_byte;

entry_vec = allowed_reason_table[rsn_code];
vec_byte = entry_vec[_BIT_INDEX(class_code)];
return (boolean) vec_byte & _BIT_MASK(class_code);

}



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This predicate specifies if a control is allowed in a class.
**
**  FORMAL PARAMETERS:
**
**      ctl_code	The sym_k_..._object code for the control, that is,
**			the class of object which is to be a child of
**			the class below.
**	class_code	The sym_k_..._object code for the class
**
**  IMPLICIT INPUTS:
**
**      allowed_control_table
**
**  IMPLICIT OUTPUTS:
**
**  FUNCTION VALUE:
**
**      TRUE		control is allowed
**	FALSE		control is not allowed
**
**  SIDE EFFECTS:
**
**--
**/

boolean sem_control_allowed (ctl_code, class_code)
    unsigned int		ctl_code;
    unsigned int		class_code;

{

unsigned char		*entry_vec;
unsigned char		vec_byte;

entry_vec = allowed_control_table[ctl_code];
vec_byte = entry_vec[_BIT_INDEX(class_code)];
return (boolean) vec_byte & _BIT_MASK(class_code);

}



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This predicate specifies if an automatically create child is
**	 allowed in a class.
**
**  FORMAL PARAMETERS:
**
**      ctl_code	The sym_k_..._child code for the child, that is,
**			the class of child which is to be automatically
**			created in the class below.
**	class_code	The sym_k_..._object code for the class
**
**  IMPLICIT INPUTS:
**
**      allowed_child_table
**
**  IMPLICIT OUTPUTS:
**
**  FUNCTION VALUE:
**
**      TRUE		child is allowed
**	FALSE		child is not allowed
**
**  SIDE EFFECTS:
**
**--
**/

boolean sem_child_allowed (ctl_code, class_code)
    unsigned int		ctl_code;
    unsigned int		class_code;

{

unsigned char		*entry_vec;
unsigned char		vec_byte;

entry_vec = allowed_child_table[ctl_code];
vec_byte = entry_vec[_BIT_INDEX(class_code)];
return (boolean) vec_byte & _BIT_MASK(class_code);

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**	
**	This function does evaluation and validation of value nodes. It
**	guarantees that a value usable by the output routine or other consumers
**	of values is available in the value union of the node, with
**	any other corollary fields also set. The result of the operation may
**	be a new node, if coercion is required.
**
**  FORMAL PARAMETERS:
**
**	val_entry	pointer to the value node to be evaluated
**
**  IMPLICIT INPUTS:
**
**      none
**
**  IMPLICIT OUTPUTS:
**
**      none
**
**  FUNCTION VALUE:
**
**      The value node which results from the evaluation/validation operation
**
**  SIDE EFFECTS:
**
**      The value union and other fields may be modified.
**
**--
**/

sym_value_entry_type *sem_evaluate_value (val_entry)
    sym_value_entry_type	*val_entry;

{
/*
 * Force expression evaluation
 */
sem_evaluate_value_expr (val_entry);

/* BEGIN HAL Fix CR 4774 */
/* 	Do not execute case statement if val_entry was previously 
 *      a sym_k_valref_op node type.  This can be determined by
 *      examining the state of the sym_m_exp_eval flag in the 
 *      b_aux_flags field.
 */


if ((val_entry->b_aux_flags & sym_m_exp_eval) == 0)
  /*
   * Perform evaluations which depend on the type of the value
   */
  switch ( val_entry->b_type )
/* END HAL Fix CR 4774 */
    {
    case sym_k_integer_value:
    case sym_k_horizontal_integer_value:
    case sym_k_vertical_integer_value:
	break;
    case sym_k_compound_string_value:
        sem_evaluate_value_cs (val_entry);
	break;
/* BEGIN OSF Fix CR 4859 */
/* END OSF Fix CR 4859 */
    case sym_k_string_table_value:
	{
	sym_value_entry_type *value_segment;
	for (value_segment=val_entry->az_first_table_value;
	     value_segment!=NULL;
	     value_segment=value_segment->az_next_table_value)
	    {
	    sem_evaluate_value_expr (value_segment);
	    if ((value_segment->b_type == sym_k_char_8_value) ||
		(value_segment->b_type == sym_k_localized_string_value))
		{
		sym_value_entry_type	*cstr;
		sym_value_entry_type	*save_next;

		save_next = (sym_value_entry_type *)
		    (value_segment -> az_next_table_value);
	        cstr = (sym_value_entry_type *) sem_create_cstr();
	        sem_append_str_to_cstr( cstr, 
		    value_segment, FALSE);
		_sym_copy_entry (value_segment,
		    cstr,
		    sym_k_value_entry_size );
		value_segment -> az_next_table_value =
		    save_next;
		cstr->value.xms_value = NULL;
		cstr->az_first_table_value = NULL;
		sem_free_node (( sym_entry_type *)cstr);
		}
	    if (value_segment->b_type != sym_k_compound_string_value) 
		diag_issue_diagnostic
		    ( d_wrong_type,
		      _sar_source_pos2( value_segment ),
		      diag_value_text( value_segment->b_type),
		      diag_value_text( sym_k_compound_string_value ) );
	    sem_evaluate_value_cs (value_segment);
	    }
        }
	break;
    case sym_k_integer_table_value:
	{
	sym_value_entry_type *value_segment;

	for (value_segment=val_entry->az_first_table_value;
	     value_segment!=NULL;
	     value_segment=value_segment->az_next_table_value)
	    {
	    sem_evaluate_value_expr (value_segment);
	    if (value_segment->b_type != sym_k_integer_value &&
		value_segment->b_type != sym_k_horizontal_integer_value &&
		value_segment->b_type != sym_k_vertical_integer_value)
		diag_issue_diagnostic
		    ( d_wrong_type,
		      _sar_source_pos2( value_segment ),
		      diag_value_text( value_segment->b_type),
		      diag_value_text( sym_k_integer_value ) );
	    }
	}
	break;
    case sym_k_asciz_table_value:
    case sym_k_trans_table_value:
	{
	sym_value_entry_type *value_segment;

	for (value_segment=val_entry->az_first_table_value;
	     value_segment!=NULL;
	     value_segment=value_segment->az_next_table_value)
	    {
	    sem_evaluate_value_expr (value_segment);
	    if ((value_segment->b_type != sym_k_char_8_value) &&
		(value_segment->b_type != sym_k_localized_string_value))
		diag_issue_diagnostic
		    ( d_wrong_type,
		      _sar_source_pos2( value_segment ),
		      diag_value_text( value_segment->b_type),
		      diag_value_text( sym_k_char_8_value ) );
	    }
	}
	break;
    case sym_k_font_table_value:
	{
	sym_value_entry_type *value_segment;

	for (value_segment=val_entry->az_first_table_value;
	     value_segment!=NULL;
	     value_segment=value_segment->az_next_table_value)
	    {
	    sem_evaluate_value_expr (value_segment);
	    if ((value_segment->b_type != sym_k_char_8_value) &&
		(value_segment->b_type != sym_k_localized_string_value) &&
		(value_segment->b_type != sym_k_font_value) &&
		(value_segment->b_type != sym_k_fontset_value))
		diag_issue_diagnostic
		    ( d_wrong_type,
		      _sar_source_pos2( value_segment ),
		      diag_value_text( value_segment->b_type),
		      diag_value_text( sym_k_char_8_value ) );
	    }
	break;
	}
/*
 * Fix for CR 5403 - check to make sure each item in the rgb table is an
 *                   integer value.  If not, print an error message.  Treat
 *                   this exactly like the integer_table_value section above.
 */
    case sym_k_rgb_value:
        {
        sym_value_entry_type *value_segment;

        for (value_segment=val_entry->az_first_table_value;
             value_segment!=NULL;
             value_segment=value_segment->az_next_table_value)
            {
            sem_evaluate_value_expr (value_segment);
            if (value_segment->b_type != sym_k_integer_value &&
                value_segment->b_type != sym_k_identifier_value &&
		value_segment->b_type != sym_k_horizontal_integer_value &&
		value_segment->b_type != sym_k_vertical_integer_value)
                diag_issue_diagnostic
                    ( d_wrong_type,
                      _sar_source_pos2( value_segment ),
                      diag_value_text( value_segment->b_type),
                      diag_value_text( sym_k_integer_value ) );
            }
        }
        break;
/*
 * End Fix for CR 5403
 */
    case sym_k_color_table_value:
	{
	sym_color_element	*colorVec;
	int			ndx;

	colorVec = val_entry->value.z_color;
	for ( ndx=0 ; ndx<(int)val_entry->b_table_count ; ndx++ )
	    if ( (int)colorVec[ndx].b_index > 1 )	/* omit FG, BG */
/*
 * Fix for CR 5428 - check to make sure that the expression result is a 
 *                   color value.  If not, print diagnostics.
 */
            {
		sem_evaluate_value_expr (colorVec[ndx].az_color);
                if ((colorVec[ndx].az_color->b_type != sym_k_color_value) && 
		    ( colorVec[ndx].az_color->b_type != sym_k_rgb_value)) 
                  diag_issue_diagnostic
                    ( d_wrong_type,
                      _sar_source_pos2( colorVec[ndx].az_color ),
                      diag_value_text( colorVec[ndx].az_color->b_type),
                      diag_value_text( sym_k_color_value ) );
             }
/*
 * End Fix for CR 5428
 */
	break;
	}
    case sym_k_pixmap_value:
    case sym_k_icon_value:
	{
	sym_icon_element	*iconDesc;
	sym_value_entry_type	*cTable;

	iconDesc = val_entry->value.z_icon;

/* BEGIN OSF Fix pir 2869*/
	if ( iconDesc == NULL )
	    break;
	else cTable = iconDesc->az_color_table;
/* END OSF Fix pir 2869 */
	if ( cTable == NULL )
	    break;
	switch ( cTable->b_type )
	    {
	    case sym_k_color_table_value:
	        {
		sym_color_element	*colorVec;
		int			vecLen = cTable->b_table_count;
		int			ndx;
		int			i, j;
		boolean			found;
		char			*row;
		sym_value_entry_type	*rowEntry;
		
		/*
		 * Find each icon character in the table, and replace its
		 * character by its index.
		 */
		colorVec = cTable->value.z_color;
		rowEntry = iconDesc->az_rows;
		for ( ndx=0 ;
		     ndx<(int)iconDesc->w_height ;
		     ndx++, rowEntry=rowEntry->az_next_table_value )
		    for ( row = rowEntry->value.c_value, i = 0 ;
			 i<(int)rowEntry->w_length ;
			 i++)
			{
			found = FALSE;
			for ( j=0 ; j<vecLen ; j++ )
			    {
			    if (colorVec[j].b_letter == row[i])
				{
				found = TRUE;
				row[i] = colorVec[j].b_index;
				break;
				}
			    }

			if ( ! found )
			    {
			    diag_issue_diagnostic
				(d_icon_letter,
				 _sar_source_pos2(rowEntry),
				 ndx+1,
				 i+1,
				 row[i]);
			    }
			}

		}
	    case sym_k_error_value:
		break;
	    default:
		diag_issue_diagnostic
		    (d_wrong_type,
		     _sar_source_pos2(cTable),
		     diag_value_text(cTable->b_type),
		     diag_value_text(sym_k_color_table_value));
		break;
	    }
	break;
	}
    }
/*
 * Perform evaluations which depend on evaluating expressions
 */
return(sem_evaluate_value_expr (val_entry));

}



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This function evaluates a compound string value node, and
**	returns the resulting compound string.
**
**  FORMAL PARAMETERS:
**
**	csval_entry	pointer to the value node to be evaluated
**
**  IMPLICIT INPUTS:
**
**      none
**
**  IMPLICIT OUTPUTS:
**
**      none
**
**  FUNCTION VALUE:
**
**      none
**
**  SIDE EFFECTS:
**
**      The value and length fields for the value entry are set.
**
**--
**/

sym_value_entry_type *sem_evaluate_value_cs (csval_entry)
    sym_value_entry_type	*csval_entry;

{

sym_value_entry_type	*next_segment;

XmString		cstr_1;
XmString		cstr_r;
int			charset;		/* sym_k_..._charset */
char			*csetptr;		/* charset name string */

_assert( (csval_entry->header.b_tag == sym_k_value_entry) &&
	(csval_entry->b_type == sym_k_compound_string_value),
	"value not compound string" );

/*
 **	You can't do anyting about imported compound strings so return.
 */
if ((csval_entry->obj_header.b_flags & sym_m_imported) != 0) 
    return(csval_entry);

/*
 **	If the pointer to the first segment of the compound string has been
 **	cleared (az_first_table_value) and there's a pointer to the evaluated
 **	compound string (value.xms_value), then we must have already evaluated
 **	this compound string.  Leave!
 */
if ((csval_entry->az_first_table_value == NULL) &&
    (csval_entry->value.xms_value != NULL))
    return(csval_entry);

/*
 **	Get the first segment of the compound string and create
 **	a compound string for it.
 */
next_segment = csval_entry->az_first_table_value;
_assert( next_segment != NULL, "compound string with no segments" );

/*
 **	If the csval_entry direction is set reset the first segments
 **	direction (actually it will be the only segments direction).
 */
if (csval_entry->b_direction != NOSTRING_DIRECTION)
    {
    next_segment->b_direction = csval_entry->b_direction;
    };

/*
 **  Initial segment: acquire character set, then
 **  create initial segment.
 */
charset = sem_map_subclass_to_charset (next_segment->b_charset );
    csetptr = sem_charset_name
	(charset, next_segment->az_charset_value);

if (next_segment->b_type == sym_k_localized_string_value)
  cstr_r = XmStringCreateLocalized(next_segment->value.c_value);
else
  cstr_r =
    XmStringConcatAndFree(XmStringDirectionCreate(next_segment->b_direction),
			  XmStringCreate(next_segment->value.c_value,
					 csetptr));

if (next_segment->b_aux_flags & sym_m_separate)
  cstr_r = XmStringConcatAndFree(cstr_r,
				 XmStringSeparatorCreate());

/*
 **  Loop through the rest of the segments of the string and append
 **  them to the first segment of the string.
 */
for (next_segment = next_segment->az_next_table_value;  
     next_segment != NULL;  
     next_segment = next_segment->az_next_table_value)
    {
    /*
     **  Acquire each segment, and set the character set, as for the
     **  initial segment.
     */
    charset = sem_map_subclass_to_charset (next_segment->b_charset );
    csetptr = sem_charset_name
	(charset, next_segment->az_charset_value);
    
    /*
     **  Create this segment, then concatenate to the result string.
     **  Free the two inputs now that a concatenated result string
     **  exists.
     */
    cstr_1 =
      XmStringConcatAndFree(XmStringDirectionCreate(next_segment->b_direction),
			    XmStringCreate(next_segment->value.c_value,
					   csetptr));

    if (next_segment->b_aux_flags & sym_m_separate)
      cstr_1 = XmStringConcatAndFree(cstr_1,
				     XmStringSeparatorCreate());
    
    cstr_r = XmStringConcatAndFree(cstr_r, cstr_1);
    }

csval_entry->value.xms_value = cstr_r;
csval_entry->w_length = XmStringLength (cstr_r);

/*
** Now deallocate the nodes for the compound string segments and put a null
** in az_first_table_value
*/
for (next_segment = csval_entry->az_first_table_value;  
     next_segment != NULL;  
     next_segment = next_segment->az_next_table_value)
	sem_free_node (( sym_entry_type *)next_segment);

csval_entry->az_first_table_value = NULL;

_assert( csval_entry->w_length <= MrmMaxResourceSize, "compound string too long" );

return (csval_entry);
}
/* BEGIN OSF Fix CR 4859 */

/* END OSF Fix CR 4859 */

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This function evaluates an expression, placing the resulting value
**	in the top-level value node of the expression tree.  A flag is set
**	in the top-level node to indicate whether the evaluation has taken
**	place.  The expression tree is left intact so that programs using
**	the callable UIL compiler will have access to the expression as well
**	as its evalutated value.
**
**  FORMAL PARAMETERS:
**
**	value_entry		the top-level value node of the expression
**
**  IMPLICIT INPUTS:
**
**  IMPLICIT OUTPUTS:
**
**  FUNCTION VALUE:
**
**	The value node resulting from this operation
**
**  SIDE EFFECTS:
**
**--
**/

sym_value_entry_type *sem_evaluate_value_expr (value_entry)
    sym_value_entry_type	*value_entry;

{

    /*
     * Local variables
     */
    char		    		op1_type;
    char		    		op2_type;
    char		    		res_type;
    data_value_type	    		op1_data;
    data_value_type	    		op2_data;
    sym_value_entry_type    	*op1_entry;
    sym_value_entry_type    	*op2_entry;
    data_value_type	    		*op1_ptr;
    data_value_type	    		*op2_ptr;
    data_value_type	    		res_data;
    sym_value_entry_type		*cat_str_entry;
    
    
    /*
     ** If this isn't an operation or if we've already evaluated it, just
     ** leave. Also, guard against attempting to deal with NULLs.
     */
    if ( value_entry == NULL )
      return value_entry;
    if ( (value_entry->b_aux_flags & sym_m_exp_eval) != 0 )
      return value_entry;
    if ( value_entry->b_expr_opr == sym_k_unspecified_op )
      return value_entry;
    
    /*
     ** If we're just beginning to evaluate a new expression, increment the
     ** value for checking circular references.
     */
    
    if (!in_expr)
      ref_chk_value++;
    in_expr = TRUE;
    
    /*
     ** Check for circular references.
     ** Place a value in each node of this expression.  If we see the same
     ** value again as we evaluate the expression, we've been here before
     ** (kind of like dropping bread crumbs).
     */
    
    if (value_entry->l_circular_ref_chk == ref_chk_value)
    {
	if ( value_entry->obj_header.az_name != NULL )
	  diag_issue_diagnostic
	    (d_circular_ref,
	     _sar_source_pos2(value_entry),
	     value_entry->obj_header.az_name->c_text);
	else
	  diag_issue_diagnostic
	    (d_circular_ref,
	     _sar_source_pos2(value_entry),
	     "unnamed value");
	return NULL;
    }
    value_entry->l_circular_ref_chk = ref_chk_value;
    
    /*
     ** Validate the first argument for the expression. If it is NULL,
     ** then return with no further processing, since this is usually
     ** due to previous compilation errors.
     */
    if ( value_entry->az_exp_op1 == NULL )
      return NULL;
    sem_evaluate_value_expr(value_entry->az_exp_op1);
    in_expr = TRUE;
    op1_type = validate_arg (value_entry->az_exp_op1,
			     value_entry->b_expr_opr);
    op1_entry = value_entry->az_exp_op1;
    res_type = op1_type;
    /*
     ** If it's a binary expression, evaluate the second argument and
     ** perform any necessary conversions
     */
    if (value_entry->az_exp_op2 != NULL)
    {
	sem_evaluate_value_expr(value_entry->az_exp_op2);
	in_expr = TRUE;
	op2_type = validate_arg (value_entry->az_exp_op2,
				 value_entry->b_expr_opr);
	
	/*
	 ** Perform conversions 
	 */
	
	op2_entry = value_entry->az_exp_op2;
	
        res_type = op1_type;
	if (res_type < op2_type)
	  res_type = op2_type;
	
	if (op1_type != res_type)
	{
	    op1_ptr  = &op1_data;
	    if (res_type <= error_arg_type)
	      op1_type = (* numeric_convert_table[ res_type ])
		( op1_entry, op1_ptr );
	    else if ((res_type != cstr_arg_type) &&
		     (res_type != lstr_arg_type))
	    {
		diag_issue_diagnostic
		  ( d_cannot_convert,
		   _sar_source_pos2( value_entry),
		   diag_value_text( op1_entry->b_type ),
		   diag_value_text( res_type ) );
		res_type = error_arg_type;
		goto continue_after_error;
	    }
	}
	else
	{
	    op1_ptr = (data_value_type *) &(op1_entry->value);
	}
	
	
	if (op2_type != res_type)
	{
	    op2_ptr  = &op2_data;
	    if (res_type <= error_arg_type)
	      op2_type = (* numeric_convert_table[ res_type ])
		( op2_entry, op2_ptr );
	    else if ((res_type != cstr_arg_type) &&
		     (res_type != lstr_arg_type))
	    {
		diag_issue_diagnostic
		  ( d_cannot_convert,
		   _sar_source_pos2( value_entry),
		   diag_value_text( op1_entry->b_type ),
		   diag_value_text( res_type ) );
		res_type = error_arg_type;
		goto continue_after_error;
	    }
	}
	else
	{
	    op2_ptr = (data_value_type *) &(op2_entry->value);
	}
    }
    
    /*
     ** Perform the operation
     */
    
    switch (value_entry->b_expr_opr)
    {
      case sym_k_unary_plus_op:
	switch (op1_type)
	{
	  case integer_arg_type:
	  case horizontal_integer_arg_type:
	  case vertical_integer_arg_type:
	    value_entry->value.l_integer = op1_entry->value.l_integer;
	    value_entry->b_arg_type = op1_entry->b_arg_type;
	    break;
	    
	  case float_arg_type:
	  case horizontal_float_arg_type:
	  case vertical_float_arg_type:
	    value_entry->value.d_real = op1_entry->value.d_real;
	    value_entry->b_arg_type = op1_entry->b_arg_type;
	    break;
	    
	  case single_float_arg_type: /* single float data type RAP */
	    value_entry->value.single_float = op1_entry->value.single_float;
	    break;
	    
	  case error_arg_type:
	    break;
	    
	  default:
	    diag_issue_diagnostic
	      ( d_cannot_convert,
	       _sar_source_pos2( value_entry),
	       diag_value_text( op1_entry->b_type ),
	       diag_value_text( value_entry->b_type ) );
	    res_type = error_arg_type;
	}
	break;
	
      case sym_k_unary_minus_op:
	switch (op1_type)
	{
	  case integer_arg_type:
	  case horizontal_integer_arg_type:
	  case vertical_integer_arg_type:
	    value_entry->value.l_integer = - op1_entry->value.l_integer;
	    value_entry->b_arg_type = op1_entry->b_arg_type;
	    break;
	    
	  case float_arg_type:
	  case horizontal_float_arg_type:
	  case vertical_float_arg_type:
	    value_entry->value.d_real = - op1_entry->value.d_real;
	    value_entry->b_arg_type = op1_entry->b_arg_type;
	    break;
	    
	  case single_float_arg_type: /* single float data type RAP */
	    value_entry->value.single_float = - op1_entry->value.single_float;
	    break;
	    
	  case error_arg_type:
	    break;
	    
	  default:
	    diag_issue_diagnostic
	      ( d_cannot_convert,
	       _sar_source_pos2( value_entry),
	       diag_value_text( op1_entry->b_type ),
	       diag_value_text( value_entry->b_type ) );
	    res_type = error_arg_type;
	}
	break;
	
      case sym_k_not_op:
	switch (op1_type)
	{
	  case boolean_arg_type:
	    value_entry->value.l_integer = ! op1_entry->value.l_integer;
	    break;
	    
	  case integer_arg_type:
	    value_entry->value.l_integer = ~ op1_entry->value.l_integer;
	    break;
	    
	  case error_arg_type:
	    break;
	    
	  default:
	    diag_issue_diagnostic
	      ( d_cannot_convert,
	       _sar_source_pos2( value_entry),
	       diag_value_text( op1_entry->b_type ),
	       diag_value_text( value_entry->b_type ) );
	    res_type = error_arg_type;
	}
	break;
	
      case sym_k_comp_str_op:
	switch (op1_type)
	{
	case char_arg_type:	/* char_8_type */
	case lstr_arg_type:
	  sem_append_str_to_cstr(value_entry, value_entry->az_exp_op1, FALSE);
	  value_entry->az_first_table_value->b_aux_flags =
	    value_entry->b_aux_flags;
             /*
              * Fix for CN 16149 (DTS 10023) part 2 -- If it exists, put the
              * charset info collected by sar_chk_comp_str_attr() onto the
              * char_8 string data structure.
              */
             if (value_entry->b_charset != sym_k_error_charset) {
                  value_entry->az_first_table_value->b_charset =
                        value_entry->b_charset;
                  if (value_entry->az_charset_value)
                        value_entry->az_first_table_value->az_charset_value =
                                value_entry->az_charset_value;
             }
             /* End fix for CN 16149 */
	    sem_evaluate_value_cs(value_entry);
	    res_type = cstr_arg_type;
	    break;
	  case cstr_arg_type:	/*  comp_str */;
	  {
	      XmString	cstr;
	      
	      /*
	       * If we're dealing with a combination 1-byte, 2-byte
	       * string, then we have to evaluate it first. (if not 
	       * already done)
	       */
	      if (value_entry->az_exp_op1->az_first_table_value != NULL)
	      {
		  sem_evaluate_value_cs(value_entry->az_exp_op1);
	      }
	      /*
	       * If there is a separater invoved, makes sure it gets
	       * concatendated onto the end of the string.  Also free
	       * up used memory.
	       */
	      if ((value_entry->b_aux_flags 
		   & sym_m_separate) != 0 )
	      {
		  cstr = XmStringSeparatorCreate();
		  value_entry->value.l_integer = 
		    (long)XmStringConcatAndFree((XmString)value_entry->az_exp_op1->
						value.l_integer, cstr);
	      }
	      else
	      {
		  value_entry->value.l_integer = 
		    value_entry->az_exp_op1->value.l_integer;
	      }
	      
	      sem_evaluate_value_cs(value_entry);
	      
	      res_type = cstr_arg_type;
	  };
	    break;
	  default:
	    diag_issue_diagnostic
	      ( d_cannot_convert,
	       _sar_source_pos2( value_entry),
	       diag_value_text( op1_entry->b_type ),
	       diag_value_text( value_entry->b_type ) );
	    res_type = error_arg_type;
	}
        break;
	
      case sym_k_wchar_str_op:
	switch (op1_type)
	{
	  case lstr_arg_type:	/* localized string type */
	    sem_append_str_to_cstr(value_entry, value_entry->az_exp_op1, FALSE);
	    value_entry->az_first_table_value->b_aux_flags =
	      value_entry->b_aux_flags;
	    /* BEGIN OSF Fix CR 4859 */
	    /* END OSF Fix CR 4859 */
	    res_type = wcstr_arg_type;
	    break;
	  default:
	    diag_issue_diagnostic
	      ( d_cannot_convert,
	       _sar_source_pos2( value_entry),
	       diag_value_text( op1_entry->b_type ),
	       diag_value_text( value_entry->b_type ) );
	    res_type = error_arg_type;
	}
        break;
	
      case sym_k_coerce_op:
	switch (value_entry->b_type)
	{
	  case sym_k_compound_string_value:
	    switch (op1_entry->b_type)
	    {
	      case sym_k_char_8_value:
	      case sym_k_localized_string_value:
		sem_append_str_to_cstr
		  (value_entry,
		   op1_entry,
		   FALSE);
		sem_evaluate_value_cs(value_entry);
		res_type = cstr_arg_type;
		break;
	      case sym_k_compound_string_value:
		_sym_copy_entry (value_entry,
				 op1_entry,
				 sym_k_value_entry_size);
		res_type = cstr_arg_type;
		break;
	      default:
		diag_issue_diagnostic
		  ( d_cannot_convert,
		   _sar_source_pos2( value_entry),
		   diag_value_text( op1_entry->b_type ),
		   diag_value_text( value_entry->b_type ) );
		res_type = error_arg_type;
		break;
	    }
	    break;
	    
	  case sym_k_wchar_string_value:
	    switch (op1_entry->b_type)
	    {
	      case sym_k_localized_string_value:
		sem_append_str_to_cstr
		  (value_entry,
		   op1_entry,
		   FALSE);
		/* BEGIN OSF Fix CR 4859 */
		/* END OSF Fix CR 4859 */
		res_type = wcstr_arg_type;
		break;
	      default:
		diag_issue_diagnostic
		  ( d_cannot_convert,
		   _sar_source_pos2( value_entry),
		   diag_value_text( op1_entry->b_type ),
		   diag_value_text( value_entry->b_type ) );
		res_type = error_arg_type;
		break;
	    }
	    break;
	    
	  case sym_k_font_table_value:
	    if ((op1_entry->b_type == sym_k_font_value) ||
		(op1_entry->b_type == sym_k_fontset_value))
	    {
		value_entry->b_table_count = 1;
		value_entry->az_first_table_value = op1_entry;
		res_type = font_table_arg_type;
	    }
	    else
	    {
		diag_issue_diagnostic
		  ( d_cannot_convert,
		   _sar_source_pos2( value_entry),
		   diag_value_text( op1_entry->b_type ),
		   diag_value_text( value_entry->b_type ) );
		res_type = error_arg_type;
	    }
	    break;
	    
	  case sym_k_font_value:
	    if ((op1_entry->b_type == sym_k_char_8_value) ||
		(op1_entry->b_type == sym_k_localized_string_value) ||
		(op1_entry->b_type == sym_k_font_value))
	    {
	        value_entry->value.c_value = op1_entry->value.c_value;
	        value_entry->w_length = op1_entry->w_length;
		res_type = font_arg_type;
	    }
	    else
	    {
		diag_issue_diagnostic
		  ( d_cannot_convert,
		   _sar_source_pos2( value_entry),
		   diag_value_text( op1_entry->b_type ),
		   diag_value_text( value_entry->b_type ) );
		res_type = error_arg_type;
	    }
	    break;
	    
	  case sym_k_fontset_value:
	    if ((op1_entry->b_type == sym_k_char_8_value) ||
		(op1_entry->b_type == sym_k_localized_string_value) ||
		(op1_entry->b_type == sym_k_fontset_value))
	    {
	        value_entry->value.c_value = op1_entry->value.c_value;
	        value_entry->w_length = op1_entry->w_length;
		res_type = fontset_arg_type;
	    }
	    else
	    {
		diag_issue_diagnostic
		  ( d_cannot_convert,
		   _sar_source_pos2( value_entry),
		   diag_value_text( op1_entry->b_type ),
		   diag_value_text( value_entry->b_type ) );
		res_type = error_arg_type;
	    }
	    
	  case sym_k_color_value:
	  case sym_k_xbitmapfile_value:
	  case sym_k_reason_value:
	  case sym_k_argument_value:
	  case sym_k_keysym_value:
	  case sym_k_class_rec_name_value:
	    switch (value_entry->b_type)
	    {
	      case sym_k_color_value:
		res_type = color_arg_type;
		break;
	      case sym_k_xbitmapfile_value:
		res_type = xbitmap_arg_type;
		break;
	      case sym_k_reason_value:
		res_type = reason_arg_type;
		break;
	      case sym_k_argument_value:
		res_type = argument_arg_type;
		break;
	      case sym_k_keysym_value:
		res_type = keysym_arg_type;
		break;
		/*  Begin fixing CR 5429 */ 
	      case sym_k_class_rec_name_value:
		res_type = classrec_arg_type;
		break;
		/*  End fixing CR 5429 */ 
	    }
	    switch (op1_entry->b_type)
	    {
	    case sym_k_char_8_value:
	    case sym_k_localized_string_value:
	        value_entry->value.c_value = op1_entry->value.c_value;
	        value_entry->w_length = op1_entry->w_length;
		break;
	      default:
		diag_issue_diagnostic
		  ( d_cannot_convert,
		   _sar_source_pos2( value_entry),
		   diag_value_text( op1_entry->b_type ),
		   diag_value_text( value_entry->b_type ) );
		res_type = error_arg_type;
		break;
	    }
	    break;
	    
	  case sym_k_integer_value:
	  case sym_k_horizontal_integer_value:
	  case sym_k_vertical_integer_value:
	    res_type = integer_arg_type;
	    switch (op1_entry->b_type)
	    {
	      case sym_k_bool_value:
	      case sym_k_integer_value:
	      case sym_k_horizontal_integer_value:
	      case sym_k_vertical_integer_value:
	        value_entry->value.l_integer = op1_entry->value.l_integer;
		value_entry->b_arg_type = op1_entry->b_arg_type;
	        break;
	      case sym_k_float_value:
	      case sym_k_horizontal_float_value:
	      case sym_k_vertical_float_value:
	        res_type = sem_convert_to_integer( op1_entry, &res_data );
	        value_entry->value.l_integer = res_data.integer_value;
		value_entry->b_arg_type = op1_entry->b_arg_type;
	        break;
	      case sym_k_single_float_value: /* single float data type RAP */
		res_type = sem_convert_to_integer( op1_entry, &res_data );
	        value_entry->value.l_integer = res_data.integer_value;
                break;
	      default:
		diag_issue_diagnostic
		  ( d_cannot_convert,
		   _sar_source_pos2( value_entry),
		   diag_value_text( op1_entry->b_type ),
		   diag_value_text( value_entry->b_type ) );
		res_type = error_arg_type;
		break;
	    }
	    break;
	  case sym_k_float_value:
	  case sym_k_horizontal_float_value:
	  case sym_k_vertical_float_value:
	    res_type = float_arg_type;
	    switch (op1_entry->b_type)
	    {
	      case sym_k_bool_value:
	      case sym_k_integer_value:
	      case sym_k_single_float_value: /* single float data type RAP */
	      case sym_k_horizontal_integer_value:
	      case sym_k_vertical_integer_value:
	        res_type = sem_convert_to_float( op1_entry, &res_data );
	        value_entry->value.d_real = res_data.real_value;
		value_entry->b_arg_type = op1_entry->b_arg_type;
	        break;
	      case sym_k_float_value:
	      case sym_k_horizontal_float_value:
	      case sym_k_vertical_float_value:
	        value_entry->value.d_real = op1_entry->value.d_real;
		value_entry->b_arg_type = op1_entry->b_arg_type;
	        break;
	      default:
		diag_issue_diagnostic
		  ( d_cannot_convert,
		   _sar_source_pos2( value_entry),
		   diag_value_text( op1_entry->b_type ),
		   diag_value_text( value_entry->b_type ) );
		res_type = error_arg_type;
		break;
	    }
	    break;
	  case sym_k_single_float_value:
	    res_type = single_float_arg_type;
	    switch (op1_entry->b_type)
	    {
	      case sym_k_bool_value:
	      case sym_k_integer_value:
	      case sym_k_horizontal_integer_value:
	      case sym_k_vertical_integer_value:
	      case sym_k_float_value:
	      case sym_k_horizontal_float_value:
	      case sym_k_vertical_float_value:
		res_type = sem_convert_to_single_float( op1_entry, &res_data );
		value_entry->value.single_float = res_data.single_float_value;
		value_entry->b_arg_type = op1_entry->b_arg_type;
		break;
	      case sym_k_single_float_value:
		value_entry->value.single_float = op1_entry->value.single_float;
		break;
	      default:
		diag_issue_diagnostic
		  ( d_cannot_convert,
		   _sar_source_pos2( value_entry),
		   diag_value_text( op1_entry->b_type ),
		   diag_value_text( value_entry->b_type ) );
		res_type = error_arg_type;
		break;
	    }
	    break;
	    
	  case sym_k_error_value:
	    break;
	    
	  default:
	    /*  Begin fixing CR 5429 */ 
	    if ((op1_entry->b_type != sym_k_char_8_value) &&
		(op1_entry->b_type != sym_k_localized_string_value))
	      diag_issue_diagnostic
	        ( d_wrong_type,
		 _sar_source_pos2( value_entry ),
		 diag_value_text( op1_entry->b_type ),
		 diag_value_text( sym_k_char_8_value ) );
	    else
	      diag_issue_diagnostic
                ( d_wrong_type,
		 _sar_source_pos2( value_entry ),
		 "wrong",
		 diag_value_text( sym_k_char_8_value  ) );
	    value_entry = sym_az_error_value_entry;
	    res_type = error_arg_type;
	}
	break;
	/*  End fixing CR 5429 */ 
	
      case sym_k_valref_op:
      {
	  /*
	   ** Copy all the value-related fields from the referenced
	   ** node to the referencing node. All non value-related fields
	   ** are left intact, except that the forward reference flag
	   ** is turned off
	   */
	  value_entry->obj_header.b_flags &= ~sym_m_forward_ref;
	  value_entry->b_type = op1_entry->b_type;
	  value_entry->w_length = op1_entry->w_length;
	  value_entry->b_table_count = op1_entry->b_table_count;
	  value_entry->b_aux_flags = op1_entry->b_aux_flags;
	  value_entry->b_arg_type = op1_entry->b_arg_type;
	  value_entry->b_data_offset = op1_entry->b_data_offset;
	  value_entry->b_pixel_type = op1_entry->b_pixel_type;
	  value_entry->b_charset = op1_entry->b_charset;
	  value_entry->b_direction = op1_entry->b_direction;
	  value_entry->b_enumeration_value_code =
	    op1_entry->b_enumeration_value_code;
	  value_entry->az_first_table_value = op1_entry->az_first_table_value;
	  value_entry->az_charset_value = op1_entry->az_charset_value;
	  /*
	   ** Because of alignment requirements, we can't just move the largest
	   ** field of the union, but actually have to move the correct value
	   ** field based upon the datatype.
	   */
	  switch (op1_entry->b_type)
	  {
	    case sym_k_integer_value:
	    case sym_k_horizontal_integer_value:
	    case sym_k_vertical_integer_value:
	      value_entry->value.l_integer = op1_entry->value.l_integer;
	      value_entry->b_arg_type = op1_entry->b_arg_type;
	      break;
	    case sym_k_float_value:
	    case sym_k_horizontal_float_value:
	    case sym_k_vertical_float_value:
	      value_entry->value.d_real = op1_entry->value.d_real;
	      value_entry->b_arg_type = op1_entry->b_arg_type;
	      break;
	    case sym_k_char_8_value:
	    case sym_k_localized_string_value:
	      value_entry->value.c_value = op1_entry->value.c_value;
	      break;
	    case sym_k_single_float_value:
	      value_entry->value.single_float = op1_entry->value.single_float;
	      break;
	    case sym_k_color_value:
	      value_entry->value.z_color = op1_entry->value.z_color;
	      break;
	    case sym_k_icon_value:
	      value_entry->value.z_icon = op1_entry->value.z_icon;
	      break;
	    default:
	      value_entry->value.az_data = op1_entry->value.az_data;
	      break;
	  }
	  break;
      }
	
      case sym_k_add_op:
	switch (res_type)
	{
	  case integer_arg_type:
	  case horizontal_integer_arg_type:
	  case vertical_integer_arg_type:
	    value_entry->value.l_integer = 
	      op1_ptr->integer_value + op2_ptr->integer_value;
	    if (op1_entry->b_arg_type != op2_entry->b_arg_type)
	      {
		diag_issue_diagnostic(d_different_units,
				      _sar_source_pos2(value_entry));
		res_type = error_arg_type;
	      }
	    else value_entry->b_arg_type = op1_entry->b_arg_type;
            break;
	    
	  case float_arg_type:
	  case horizontal_float_arg_type:
	  case vertical_float_arg_type:
            value_entry->value.d_real = 
	      op1_ptr->real_value + op2_ptr->real_value;
	    if (op1_entry->b_arg_type != op2_entry->b_arg_type)
	      {
		diag_issue_diagnostic(d_different_units,
				      _sar_source_pos2(value_entry));
		res_type = error_arg_type;
	      }
	    else value_entry->b_arg_type = op1_entry->b_arg_type;
            break;
	    
	  case single_float_arg_type:
            value_entry->value.single_float = 
	      op1_ptr->single_float_value + op2_ptr->single_float_value;
            break;
	    
	  case error_arg_type:
            break;
	    
	  default:
	    diag_issue_diagnostic
	      ( d_cannot_convert,
	       _sar_source_pos2( value_entry),
	       diag_value_text( op1_entry->b_type ),
	       diag_value_text( value_entry->b_type ) );
	    res_type = error_arg_type;
        }
	break;
	
      case sym_k_subtract_op:
	switch (res_type)
	{
	  case integer_arg_type:
	  case horizontal_integer_arg_type:
	  case vertical_integer_arg_type:
	    value_entry->value.l_integer = 
	      op1_ptr->integer_value - op2_ptr->integer_value;
	    if (op1_entry->b_arg_type != op2_entry->b_arg_type)
	      {
		diag_issue_diagnostic(d_different_units,
				      _sar_source_pos2(value_entry));
		res_type = error_arg_type;
	      }
	    else value_entry->b_arg_type = op1_entry->b_arg_type;
	    break;
	    
	  case float_arg_type:
	  case horizontal_float_arg_type:
	  case vertical_float_arg_type:
	    value_entry->value.d_real = 
	      op1_ptr->real_value - op2_ptr->real_value;
	    if (op1_entry->b_arg_type != op2_entry->b_arg_type)
	      {
		diag_issue_diagnostic(d_different_units,
				      _sar_source_pos2(value_entry));
		res_type = error_arg_type;
	      }
	    else value_entry->b_arg_type = op1_entry->b_arg_type;
	    break;
	    
	  case single_float_arg_type:
            value_entry->value.single_float = 
	      op1_ptr->single_float_value - op2_ptr->single_float_value;
            break;
	    
	  case error_arg_type:
	    break;
	    
	  default:
	    diag_issue_diagnostic
	      ( d_cannot_convert,
	       _sar_source_pos2( value_entry),
	       diag_value_text( op1_entry->b_type ),
	       diag_value_text( value_entry->b_type ) );
	    res_type = error_arg_type;
	}
	break;
	
      case sym_k_multiply_op:
	switch (res_type)
	{
	  case integer_arg_type:
	  case horizontal_integer_arg_type:
	  case vertical_integer_arg_type:
	    value_entry->value.l_integer = 
	      op1_ptr->integer_value * op2_ptr->integer_value;
	    if (op1_entry->b_arg_type != op2_entry->b_arg_type)
	      {
		diag_issue_diagnostic(d_different_units,
				      _sar_source_pos2(value_entry));
		res_type = error_arg_type;
	      }
	    else value_entry->b_arg_type = op1_entry->b_arg_type;
	    break;
	    
	  case float_arg_type:
	  case horizontal_float_arg_type:
	  case vertical_float_arg_type:
	    value_entry->value.d_real = 
	      op1_ptr->real_value * op2_ptr->real_value;
	    if (op1_entry->b_arg_type != op2_entry->b_arg_type)
	      {
		diag_issue_diagnostic(d_different_units,
				      _sar_source_pos2(value_entry));
		res_type = error_arg_type;
	      }
	    else value_entry->b_arg_type = op1_entry->b_arg_type;
	    break;
	    
	  case single_float_arg_type:
            value_entry->value.single_float = 
	      op1_ptr->single_float_value * op2_ptr->single_float_value;
            break;
	    
	  case error_arg_type:
	    break;
	    
	  default:
	    diag_issue_diagnostic
	      ( d_cannot_convert,
	       _sar_source_pos2( value_entry),
	       diag_value_text( op1_entry->b_type ),
	       diag_value_text( value_entry->b_type ) );
	    res_type = error_arg_type;
	}
	break;
	
      case sym_k_divide_op:
	switch (res_type)
	{
	  case integer_arg_type:
	  case horizontal_integer_arg_type:
	  case vertical_integer_arg_type:
	    value_entry->value.l_integer = 
	      op1_ptr->integer_value / op2_ptr->integer_value;
	    if (op1_entry->b_arg_type != op2_entry->b_arg_type)
	      {
		diag_issue_diagnostic(d_different_units,
				      _sar_source_pos2(value_entry));
		res_type = error_arg_type;
	      }
	    else value_entry->b_arg_type = op1_entry->b_arg_type;
	    break;
	    
	  case float_arg_type:
	  case horizontal_float_arg_type:
	  case vertical_float_arg_type:
	    value_entry->value.d_real = 
	      op1_ptr->real_value / op2_ptr->real_value;
	    if (op1_entry->b_arg_type != op2_entry->b_arg_type)
	      {
		diag_issue_diagnostic(d_different_units,
				      _sar_source_pos2(value_entry));
		res_type = error_arg_type;
	      }
	    else value_entry->b_arg_type = op1_entry->b_arg_type;
	    break;
	    
	  case single_float_arg_type:
            value_entry->value.single_float = 
	      op1_ptr->single_float_value / op2_ptr->single_float_value;
            break;
	    
	  case error_arg_type:
	    break;
	    
	  default:
	    diag_issue_diagnostic
	      ( d_cannot_convert,
	       _sar_source_pos2( value_entry),
	       diag_value_text( op1_entry->b_type ),
	       diag_value_text( value_entry->b_type ) );
	    res_type = error_arg_type;
	}
	break;
	
      case sym_k_left_shift_op:
	switch (res_type)
	{
	  case integer_arg_type:
	  {
	      int	    shift_count;
	      
	      shift_count = op2_ptr->integer_value;
	      
	      if ((shift_count < 0) || (shift_count > 32))
		goto error_occurred;
	      
	      value_entry->value.l_integer = 
		op1_ptr->integer_value << op2_ptr->integer_value;
	      
	      break;
	  }
	  
	case error_arg_type:
	  break;
	  
	default:
	  diag_issue_diagnostic
	    ( d_cannot_convert,
	     _sar_source_pos2( value_entry),
	     diag_value_text( op1_entry->b_type ),
	     diag_value_text( value_entry->b_type ) );
	  res_type = error_arg_type;
      }
	break;
	
      case sym_k_right_shift_op:
	switch (res_type)
	{
	  case integer_arg_type:
	  {
	      int	    shift_count;
	      
	      shift_count = op2_ptr->integer_value;
	      
	      if ((shift_count < 0) || (shift_count > 32))
		goto error_occurred;
	      
	      value_entry->value.l_integer = 
		op1_ptr->integer_value >> op2_ptr->integer_value;
	      
	      break;
	  }
	  
	case error_arg_type:
	  break;
	  
	default:
	  diag_issue_diagnostic
	    ( d_cannot_convert,
	     _sar_source_pos2( value_entry),
	     diag_value_text( op1_entry->b_type ),
	     diag_value_text( value_entry->b_type ) );
	  res_type = error_arg_type;
      }
	break;
	
      case sym_k_and_op:
	switch (res_type)
	{
	  case integer_arg_type:
	  case boolean_arg_type:
	    value_entry->value.l_integer = 
	      op1_ptr->integer_value & op2_ptr->integer_value;
	    break;
	  case char_arg_type:
	  case lstr_arg_type:
	    sar_cat_value_entry( &cat_str_entry, op1_entry, op2_entry );
	    value_entry->b_type = cat_str_entry->b_type;
	    if (cat_str_entry->b_type == sym_k_compound_string_value)
	    {
		res_type = cstr_arg_type;
		value_entry->az_first_table_value = cat_str_entry->az_first_table_value;
	    }
	    value_entry->b_charset = cat_str_entry->b_charset;
	    value_entry->b_direction = cat_str_entry->b_direction;
	    value_entry->value.c_value = cat_str_entry->value.c_value;
	    value_entry->w_length = cat_str_entry->w_length;
	    cat_str_entry->value.c_value = NULL;
	    sem_free_node (( sym_entry_type *)cat_str_entry);
	    break;
	  case cstr_arg_type:
	    sar_cat_value_entry( &cat_str_entry, op1_entry, op2_entry );
	    value_entry->b_type = cat_str_entry->b_type;
	    value_entry->b_charset = cat_str_entry->b_charset;
	    value_entry->b_direction = cat_str_entry->b_direction;
	    value_entry->value.xms_value = cat_str_entry->value.xms_value;
	    value_entry->az_first_table_value = 
	      cat_str_entry->az_first_table_value;
	    value_entry->w_length = cat_str_entry->w_length;
	    sem_evaluate_value_cs (value_entry);
	    cat_str_entry->value.xms_value = NULL;
	    sem_free_node (( sym_entry_type *)cat_str_entry);
	    break;
	  case error_arg_type:
	    break;
	    
	  default:
	    diag_issue_diagnostic
	      ( d_cannot_convert,
	       _sar_source_pos2( value_entry),
	       diag_value_text( op1_entry->b_type ),
	       diag_value_text( value_entry->b_type ) );
	    res_type = error_arg_type;
	}
	break;
	
      case sym_k_or_op:
	switch (res_type)
	{
	  case integer_arg_type:
	  case boolean_arg_type:
	    value_entry->value.l_integer = 
	      op1_ptr->integer_value | op2_ptr->integer_value;
	    break;
	    
	  case error_arg_type:
	    break;
	    
	  default:
	    diag_issue_diagnostic
	      ( d_cannot_convert,
	       _sar_source_pos2( value_entry),
	       diag_value_text( op1_entry->b_type ),
	       diag_value_text( value_entry->b_type ) );
	    res_type = error_arg_type;
	}
	break;
	
      case sym_k_xor_op:
	switch (res_type)
	{
	  case integer_arg_type:
	  case boolean_arg_type:
	    value_entry->value.l_integer = 
	      op1_ptr->integer_value ^ op2_ptr->integer_value;
	    break;
	    
	  case error_arg_type:
	    break;
	    
	  default:
	    diag_issue_diagnostic
	      ( d_cannot_convert,
	       _sar_source_pos2( value_entry),
	       diag_value_text( op1_entry->b_type ),
	       diag_value_text( value_entry->b_type ) );
	    res_type = error_arg_type;
	}
	break;
      default:
	_assert( FALSE, "unexpected operator" );
    }	/* End of switch statement */
    
    
  continue_after_error:
    
    /*
     ** Set data type for expression value.  If binary operation, use res_type
     ** because conversions may have taken place.  Otherwise use b_type from
     ** the operand of the unary operator.
     */
    
    if (value_entry->b_expr_opr == sym_k_valref_op) 
      value_entry->b_type = op1_entry->b_type;
    else
      switch (res_type)
      {
        case boolean_arg_type:
	  value_entry->b_type = sym_k_bool_value;
	  break;
	  
        case integer_arg_type:
	  value_entry->b_type = sym_k_integer_value;
	  break;
	  
        case single_float_arg_type:
	  value_entry->b_type = sym_k_single_float_value;
	  break;
	  
        case float_arg_type:
	  value_entry->b_type = sym_k_float_value;
	  break;
	  
	case horizontal_integer_arg_type:
	  value_entry->b_type = sym_k_horizontal_integer_value;
	  break;
	  
	case vertical_integer_arg_type:
	  value_entry->b_type = sym_k_vertical_integer_value;
	  break;
	  
	case horizontal_float_arg_type:
	  value_entry->b_type = sym_k_horizontal_float_value;
	  break;
	  
	case vertical_float_arg_type:
	  value_entry->b_type = sym_k_vertical_float_value;
	  break;
	  
	case keysym_arg_type:
	  value_entry->b_type = sym_k_keysym_value;
	  break;
	  
        case char_arg_type:
	case lstr_arg_type:
	  value_entry->b_type = sym_k_char_8_value;
	  break;
	  
        case cstr_arg_type:
	  value_entry->b_type = sym_k_compound_string_value;
	  break;
	  
        case wcstr_arg_type:
	  value_entry->b_type = sym_k_wchar_string_value;
	  break;
	  
	case font_arg_type:
	  value_entry->b_type = sym_k_font_value;
	  break;
	  
	case fontset_arg_type:
	  value_entry->b_type = sym_k_fontset_value;
	  break;
	  
	case color_arg_type:
	  value_entry->b_type = sym_k_color_value;
	  break;
	  
	  /*  Begin fixing CR 5429 */ 
	case classrec_arg_type:
	  value_entry->b_type = sym_k_class_rec_name_value;
	  break;
	  /*  End fixing CR 5429 */ 
	  
	case xbitmap_arg_type:
	  value_entry->b_type = sym_k_xbitmapfile_value;
	  break;
	  
	case reason_arg_type:
	  value_entry->b_type = sym_k_reason_value;
	  break;
	  
	case argument_arg_type:
	  value_entry->b_type = sym_k_argument_value;
	  break;
	  
	case font_table_arg_type:
	  value_entry->b_type = sym_k_font_table_value;
	  break;
	  
	case error_arg_type:
	  value_entry->b_type = sym_k_error_value;
	  break;
	  
        default:    
	  _assert( FALSE, "unexpected type" );
      }
    
    /*
     ** indicate that this expression has been evaluated
     */
    
    value_entry->b_aux_flags |= sym_m_exp_eval;
    in_expr = FALSE;
    return value_entry;
    
    /*
     **	Point where errors are transferred
     */
    
  error_occurred:
    
    diag_issue_diagnostic
      ( d_out_range,
       _sar_source_pos2( value_entry ),
       value_text[ res_type ],
       ""
       );
    res_type = error_arg_type;
    diag_reset_overflow_handler();
    
    goto continue_after_error;
}


int validate_arg( operand_entry, operator)

sym_value_entry_type    *operand_entry;
int	    		operator;

{

    char    operand_type;

    operand_type = operand_entry->b_type;

    if (operand_type == sym_k_error_value )
        return error_arg_type;

    if ((( 1 << operand_type ) & legal_operand_type[ operator ]) == 0)
    {
	diag_issue_diagnostic
	    ( d_operand_type,
	      _sar_source_pos2( operand_entry ),
	      diag_value_text( operand_type ),
	      operator_symbol[ operator ]
	    );
	return error_arg_type;
    }

    if ((operand_entry->obj_header.b_flags & sym_m_imported) != 0)
    {
	sym_value_entry_type    *value_entry;

	value_entry = operand_entry;

	diag_issue_diagnostic
	    ( d_nonpvt,
	      _sar_source_pos2( operand_entry ),
	      value_entry->obj_header.az_name->c_text
	    );
	return error_arg_type;
    }

    switch (operand_type)
    {
    case sym_k_bool_value:
	return boolean_arg_type;

    case sym_k_integer_value:
	return integer_arg_type;

    case sym_k_float_value:
	return float_arg_type;

    case sym_k_single_float_value:
	return single_float_arg_type;
	
    case sym_k_horizontal_integer_value:
	return horizontal_integer_arg_type;

    case sym_k_vertical_integer_value:
	return vertical_integer_arg_type;

    case sym_k_horizontal_float_value:
	return horizontal_float_arg_type;

    case sym_k_vertical_float_value:
	return vertical_float_arg_type;

    case sym_k_char_8_value:
	return char_arg_type;

    case sym_k_compound_string_value:
	return cstr_arg_type;

    case sym_k_localized_string_value:
	return lstr_arg_type;

    case sym_k_wchar_string_value:
	return wcstr_arg_type;

    case sym_k_font_value:
	return font_arg_type;

    case sym_k_fontset_value:
	return fontset_arg_type;

    case sym_k_color_value:
	return color_arg_type;

    default:
	return error_arg_type;
    }
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function converts a value to floating point.
**
**  FORMAL PARAMETERS:
**
**      operand_entry	    frame of the value to convert
**	data_value	    data structure to hold float result
**
**  IMPLICIT INPUTS:
**
**      none
**
**  IMPLICIT OUTPUTS:
**
**      none
**
**  FUNCTION VALUE:
**
**      float_arg_type	    if operation succeeds
**	error_arg_type	    if operation fails
**
**  SIDE EFFECTS:
**
**      none
**
**--
**/
int	sem_convert_to_float(operand_entry, data_value)

sym_value_entry_type	*operand_entry;
data_value_type	    	*data_value;

{
    switch (operand_entry->b_type)
    {
    case sym_k_error_value:
	return error_arg_type;

    case sym_k_integer_value:
    case sym_k_horizontal_integer_value:
    case sym_k_vertical_integer_value:
    case sym_k_bool_value:
	data_value->real_value = operand_entry->value.l_integer;
	return float_arg_type;

      case sym_k_single_float_value: /* single float data type RAP */
	data_value->real_value = operand_entry->value.single_float;
	return float_arg_type;

    case sym_k_float_value:
    case sym_k_horizontal_float_value:
    case sym_k_vertical_float_value:
	data_value->real_value = operand_entry->value.d_real;
	return float_arg_type;

    default:
	_assert( FALSE, "unexpected operand type" );
    }
    return error_arg_type;
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function converts a value to type integer
**
**  FORMAL PARAMETERS:
**
**      operand_entry	    frame of the value to convert
**	data_value	    data structure to hold integer result
**
**  IMPLICIT INPUTS:
**
**      none
**
**  IMPLICIT OUTPUTS:
**
**      none
**
**  FUNCTION VALUE:
**
**      integer_arg_type    if operation succeeds
**	error_arg_type	    if operation fails
**
**  SIDE EFFECTS:
**
**      none
**
**--
**/
int	sem_convert_to_integer(operand_entry, data_value)

sym_value_entry_type	*operand_entry;
data_value_type	    	*data_value;

{
    int			    res_type;

    uil_az_error_env_valid = TRUE; 
    if (setjmp(uil_az_error_env_block) == 0 ) 
      {
	switch (operand_entry->b_type)
	  {
	  case sym_k_error_value:
	    res_type = error_arg_type;
	    break;

	  case sym_k_integer_value:
	  case sym_k_horizontal_integer_value:
	  case sym_k_vertical_integer_value:
	  case sym_k_bool_value:
	    data_value->integer_value = operand_entry->value.l_integer;
	    res_type = integer_arg_type;
	    break;

	  case sym_k_float_value:
	  case sym_k_horizontal_float_value:
	  case sym_k_vertical_float_value:
	    data_value->integer_value = operand_entry->value.d_real;
	    res_type = integer_arg_type;
	    break;

	  case sym_k_single_float_value: /* single float data type RAP */
	    data_value->integer_value = 
	      (int) operand_entry->value.single_float;
	    res_type = integer_arg_type;
	    break;

	  default:
	    _assert( FALSE, "unexpected operand type" );
	  }

	uil_az_error_env_valid = FALSE;
	return res_type;
      }
    else
      {
	diag_issue_diagnostic
	  ( d_out_range,
	   _sar_source_pos2( operand_entry ),
	   value_text[ integer_arg_type ],
	   ""
	   );
	diag_reset_overflow_handler();
	uil_az_error_env_valid = FALSE;
	return error_arg_type;
      }
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function converts a value to single floating point.
**  (RAP single float data type)
**
**  FORMAL PARAMETERS:
**
**      operand_entry	    symbol table entry of the value to convert
**	data_value	    data structure to hold float result
**
**  IMPLICIT INPUTS:
**
**      none
**
**  IMPLICIT OUTPUTS:
**
**      none
**
**  FUNCTION VALUE:
**
**      single_float_arg_type	    if operation succeeds
**	error_arg_type   	    if operation fails
**
**  SIDE EFFECTS:
**
**      none
**
**--
**/
int	sem_convert_to_single_float(operand_entry, data_value)

sym_value_entry_type	    *operand_entry;
data_value_type	    *data_value;

{
    switch (operand_entry->b_type)
    {
    case sym_k_error_value:
	return error_arg_type;

    case sym_k_integer_value:
    case sym_k_horizontal_integer_value:
    case sym_k_vertical_integer_value:
    case sym_k_bool_value:
	data_value->single_float_value = (float)operand_entry->value.l_integer;
	return single_float_arg_type;

    case sym_k_float_value:
    case sym_k_horizontal_float_value:
    case sym_k_vertical_float_value:
	data_value->single_float_value = (float)operand_entry->value.d_real;
	return single_float_arg_type;

    default:
	_assert( FALSE, "unexpected operand type" );
    }
    return error_arg_type;
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function converts a value to an error - just needed to
**	fill a slot in the conversion table.
**
**  FORMAL PARAMETERS:
**
**      operand_entry	    frame of the value to convert
**	data_value	    not used
**
**  IMPLICIT INPUTS:
**
**      none
**
**  IMPLICIT OUTPUTS:
**
**      none
**
**  FUNCTION VALUE:
**
**	error_arg_type
**
**  SIDE EFFECTS:
**
**      none
**
**--
**/
int	sem_convert_to_error(operand_entry, data_value)

sym_value_entry_type	*operand_entry;
data_value_type	    	*data_value;

{
    return error_arg_type;
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function processes the concatenation of 2 strings.
**
**  FORMAL PARAMETERS:
**
**      operator_entry	[in/out] pointer to resultant value stack frame
**      op1_entry	[in] pointer to operand 1 value frame 
**      op2_entry	[in] pointer to operand 2 value frame 
**
**  IMPLICIT INPUTS:
**
**      none
**
**  IMPLICIT OUTPUTS:
**
**      none
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**      error message is issued if value is out of range
**
**--
**/

void	sar_cat_value_entry( target_entry, op1_entry, op2_entry )

sym_value_entry_type	    **target_entry;
sym_value_entry_type	    *op1_entry;
sym_value_entry_type	    *op2_entry;

{

/*
**  For pcc conversion, use defines instead of this enum.
**
**    enum op_state
**    {
**	error=0, simple, compound, 
**    };
*/

#define		k_op_state_error	0
#define		k_op_state_simple	1
#define		k_op_state_compound	2
#define		k_op_state_localized	4

    int			    target_type;
    sym_value_entry_type    *value1_entry;
    sym_value_entry_type    *value2_entry;
    unsigned int	    op1_state;
    unsigned int	    op2_state;

    /*
    **  The target type is dependent on the type of the sources.  If both
    **	operands are primitive and have the same writing direction and
    **	charset, the result is still of that type.  If both operands are 
    **	localized strings, the result is a localized string. If not, the result
    **	is a compound string.
    */

    switch (op1_entry->b_type)
    {
    case sym_k_char_8_value:
	op1_state = k_op_state_simple;
	break;
    case sym_k_compound_string_value:
	op1_state = k_op_state_compound;
	break;
    case sym_k_localized_string_value:
	op1_state = k_op_state_localized;
	break;
    case sym_k_error_value:
	op1_state = k_op_state_error;
	break;
    default:
	diag_issue_diagnostic
	    ( d_wrong_type,
	      _sar_source_pos2( op1_entry ),
	      diag_value_text( op1_entry->b_type),
	      "string or compound string" );
	op1_state = k_op_state_error;
    }

    switch (op2_entry->b_type)
    {
    case sym_k_char_8_value:
	op2_state = k_op_state_simple;
	break;
    case sym_k_compound_string_value:
	op2_state = k_op_state_compound;
	break;
    case sym_k_localized_string_value:
	op2_state = k_op_state_localized;
	break;
    case sym_k_error_value:
	op2_state = k_op_state_error;
	break;
    default:
	diag_issue_diagnostic
	    ( d_wrong_type,
	      _sar_source_pos2( op2_entry ),
	      diag_value_text( op2_entry->b_type),
	      "string or compound string" );
	op2_state = k_op_state_error;
    }

    value1_entry = op1_entry;
    value2_entry = op2_entry;

    /*
    **	Verify that both operands are private values
    */
    /* Begin fixing OSF CR 5509 */
    if ((op1_entry->obj_header.b_flags & (sym_m_private|sym_m_exported)) == 0) 
	{
	op1_state = k_op_state_error;
	diag_issue_diagnostic
		(d_nonpvt,
		_sar_source_pos2 (op1_entry),
		value1_entry->obj_header.az_name->c_text );
	}
    if ((op2_entry->obj_header.b_flags & (sym_m_private|sym_m_exported)) == 0) 
	{
	op2_state = k_op_state_error;
	diag_issue_diagnostic
		(d_nonpvt,
		_sar_source_pos2 (op2_entry),
		value2_entry->obj_header.az_name->c_text );
	}	
    /* End fixing OSF CR 5509 */
    switch (op1_state + (op2_state<<3))
    {
    /*
    **	This is the case of appending two simple strings or a simple string 
    **	and a localized	string.  Just append them
    **	unless they have different directions or the first one has the separate
    **	attribute.
    */
    case k_op_state_simple + (k_op_state_simple<<3):
    case k_op_state_simple + (k_op_state_localized<<3):
    case k_op_state_localized + (k_op_state_simple<<3):
	if ((value1_entry->b_charset == value2_entry->b_charset) 
	    &&
	    ((value1_entry->b_direction) == (value2_entry->b_direction))
	    && 
	    ((value1_entry->b_aux_flags & sym_m_separate) == 0))
	{
	    *target_entry = (sym_value_entry_type *)
		sem_cat_str_to_str
		    (value1_entry, FALSE,
		     value2_entry, FALSE);
	    target_type  = sym_k_char_8_value;
	}
	else
	{
	    *target_entry = (sym_value_entry_type *) sem_create_cstr( );
	    sem_append_str_to_cstr
		(*target_entry, 
		 value1_entry, FALSE);
	    sem_append_str_to_cstr
		(*target_entry, 
		 value2_entry, FALSE);
	    sem_evaluate_value_cs(*target_entry);
	    target_type  = sym_k_compound_string_value;
	}
	break;

    /*
    **	This is the case of one simple/localized and one compound string. 
    **	Change the
    **	simple/localized to a compound and append them together.  Depend on
    **	the append
    **	routine to do the right thing.
    */
    case k_op_state_simple + (k_op_state_compound<<3):
    case k_op_state_localized + (k_op_state_compound<<3):
	*target_entry = (sym_value_entry_type *) sem_create_cstr( );
	sem_append_str_to_cstr
	    (*target_entry, 
	     value1_entry, FALSE);
	sem_evaluate_value_cs(*target_entry);
	/*
	 * We must evaluate both  entries to the XmStringConcat routine so
	 * that it will work properly.   However this MAY be a pointer to
	 * a compound string, use that value instead or we will concat a 
	 * NULL value and lose part of the string.
	 */
	if ((value2_entry->az_first_table_value == NULL) &&
	    (value2_entry->value.xms_value == NULL))
	    value2_entry->value.xms_value = 
		value2_entry->az_exp_op1->value.xms_value;
	else
	    sem_evaluate_value_cs(value2_entry);

	(*target_entry)->value.xms_value = 
	    XmStringConcat((*target_entry)->value.xms_value,
			   value2_entry->value.xms_value);
	target_type  = sym_k_compound_string_value;
	break;

    /*
    **	This is the case of one simple/localized and one compound string.  
    **  Append the simple/localized to the compound.  
    */
    case k_op_state_compound + (k_op_state_simple<<3):
    case k_op_state_compound + (k_op_state_localized<<3):

	*target_entry = (sym_value_entry_type *) sem_create_cstr( );
	sem_append_str_to_cstr
	    (*target_entry,
	     value2_entry, FALSE);
	sem_evaluate_value_cs (*target_entry);
	/*
	 * We must evaluate both  entries to the XmStringConcat routine so
	 * that it will work properly.   However this MAY be a pointer to
	 * a compound string, use that value instead or we will concat a 
	 * NULL value and lose part of the string.
	 */
	if ((value1_entry->az_first_table_value == NULL) &&
	    (value1_entry->value.xms_value == NULL))
	    value1_entry->value.xms_value = 
		value1_entry->az_exp_op1->value.xms_value;
	else
	    sem_evaluate_value_cs(value1_entry);

	(*target_entry)->value.xms_value = 
	    XmStringConcat (value1_entry->value.xms_value, 
			    (*target_entry)->value.xms_value);
	target_type  = sym_k_compound_string_value;
	break;

    /*
    **  This is the case of two compound strings.  Just let the append routine
    **  do the right thing.
    */
    case k_op_state_compound + (k_op_state_compound<<3):
	*target_entry = (sym_value_entry_type *) sem_create_cstr( );
	/*
	 * We must evaluate both  entries to the XmStringConcat routine so
	 * that it will work properly.   However this MAY be a pointer to
	 * a compound string, use that value instead or we will concat a 
	 * NULL value and lose part of the string.
	 */
	if ((value1_entry->az_first_table_value == NULL) &&
	    (value1_entry->value.xms_value == NULL))
	    value1_entry->value.xms_value = 
		value1_entry->az_exp_op1->value.xms_value;
	else
	    sem_evaluate_value_cs(value1_entry);

	if ((value2_entry->az_first_table_value == NULL) &&
	    (value2_entry->value.xms_value == NULL)) 
	    value2_entry->value.xms_value = 
		value2_entry->az_exp_op1->value.xms_value;
	else
	    sem_evaluate_value_cs(value2_entry);

	(*target_entry)->value.xms_value = 
	    XmStringConcat(value1_entry->value.xms_value,
			   value2_entry->value.xms_value);
	target_type  = sym_k_compound_string_value;
	break;

    /*
    **	This is the case of two localized strings.  Just concatenate them.
    */
    case k_op_state_localized + (k_op_state_localized<<3):
      *target_entry = (sym_value_entry_type *)
	sem_cat_str_to_str
	  (value1_entry, FALSE,
	   value2_entry, FALSE);
	target_type  = sym_k_localized_string_value;
	
    default:   /* some form of error */
	target_type = sym_k_error_value;
	*target_entry = (sym_value_entry_type *) sym_az_error_value_entry;
	break;
    }

}

