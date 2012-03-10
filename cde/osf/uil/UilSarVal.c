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
static char rcsid[] = "$TOG: UilSarVal.c /main/13 1997/12/06 16:14:16 cshi $"
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
**      This module supports values in UIL.  UIL values are
**	quite primitive in terms of operators, however, there is a
**      concept of a private value that is local to this module
**      and a imported or exported value which needs to be
**	resolved via a lookup at runtime.  The runtime resolved values
**	cannot be modified with operators.
**
**--
**/


/*
**
**  INCLUDE FILES
**
**/

#include <Xm/Xm.h>

#include "UilDefI.h"
#include "UilSymGen.h"	/* For sym_k_[TRUE|FALSE]_enumval */
/*
**
**  TABLE OF CONTENTS
**
**/


/*
** FORWARD DECLARATIONS
*/

static sym_value_entry_type *standard_color_table  _ARGUMENTS(( void ));

/*
**
**  DEFINE and MACRO DEFINITIONS
**
**/

#define clear_class_mask \
	(~(sym_m_private | sym_m_imported | sym_m_exported | sym_m_builtin))

/*
**
**  EXTERNAL VARIABLE DECLARATIONS
**
**/

extern yystype		yylval;


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



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function takes a non-reserved keyword and makes a name
**	entry for it.  In the context in which the keyword is used,
**	it is not being used as a keyword.
**
**  FORMAL PARAMETERS:
**
**      target_frame	pointer to resultant token stack frame
**      keyword_frame	pointer to token stack frame holding the keyword
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
**      a name entry is generated
**
**--
**/

void	sar_map_keyword_to_name( target_frame, keyword_frame )

yystype	    *target_frame;
yystype	    *keyword_frame;
{
    sym_name_entry_type	    *name_entry;

    _assert( keyword_frame->b_tag == sar_k_token_frame,
	     "keyword frame missing from stack" );

    /*
    ** make the target frame a token frame for a name token
    */

    _sar_move_source_info( target_frame, keyword_frame );
    target_frame->b_tag = sar_k_token_frame;
    target_frame->b_type = NAME;

    /* 
    ** insert the keyword name into the symbol table
    */

    name_entry = 
	sym_insert_name
	    ( keyword_frame->value.az_keyword_entry->b_length,
	      keyword_frame->value.az_keyword_entry->at_name );

    target_frame->value.az_symbol_entry = (sym_entry_type *) name_entry;

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function processes an id that is being used as an operand
**	in a value.  It can be either a value or an identifier.
**
**  FORMAL PARAMETERS:
**
**      target_frame	pointer to resultant value stack frame
**      id_frame	pointer to token stack frame holding the id
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
**      error may be issued for undefined name
**
**--
**/

void	sar_process_id( target_frame, id_frame )

yystype	    *target_frame;
yystype	    *id_frame;
{

    sym_name_entry_type	    *name_entry;
    sym_value_entry_type    *value_entry;
    int			    enum_code;

    _assert( id_frame->b_tag == sar_k_token_frame,
	     "id frame missing from stack" );

    /*
    ** make the target frame an expression frame
    */

    _sar_move_source_info( target_frame, id_frame );
    target_frame->b_tag = sar_k_value_frame;

    /* id frame may be a:
    **	    font_name, color_name, reason_name, argument_name...
    **	    name
    ** if the font_name, color_name, etc... has a name been defined by
    ** the user, we use that name, otherwise these special names are
    ** treated as private values.
    */

    if (id_frame->b_type != NAME)
    {
	name_entry = 
	    sym_find_name
		( id_frame->value.az_keyword_entry->b_length,
		  id_frame->value.az_keyword_entry->at_name );

	if (name_entry == NULL)
	{
	    int	    source_type;
	    unsigned short int	arg_code;
	    unsigned short int	rel_code;

	    switch (id_frame->b_type)
	    {
	    case FONT_NAME:
		source_type = sym_k_font_value;
		break;

	    case CHILD_NAME:
		source_type = sym_k_child_value;
		break;

	    case ARGUMENT_NAME:
		source_type = sym_k_argument_value;

		/*
		** Indicate that this argument is used so we can later 
		** generate appropriate compression code for it.
		** If this argument has a related argument, also mark it as
		** being used so we can later generate its comprssion code.
		*/

		arg_code = id_frame->value.az_keyword_entry->b_subclass;
		uil_arg_compr[arg_code] = 1;
		rel_code = related_argument_table[arg_code];
		if (rel_code != 0)
		    uil_arg_compr[rel_code] = 1;
		break;

	    case COLOR_NAME:
		source_type = sym_k_color_value;
		break;

	    case REASON_NAME:
		source_type = sym_k_reason_value;
		uil_reas_compr[id_frame->value.az_keyword_entry->b_subclass] 
		    = 1;
		break;

	    case ENUMVAL_NAME:
		source_type = sym_k_integer_value;
		enum_code = id_frame->value.az_keyword_entry->b_subclass;
		break;

	    default:
		_assert( FALSE, "unexpected token" );
	    }

	    value_entry = 
		sem_create_value_entry
		    ( (char*)&(id_frame->value.az_keyword_entry), sizeof(long),
		     source_type );
	    if ( id_frame->b_type == ENUMVAL_NAME )
	    {
		value_entry->b_enumeration_value_code = enum_code;
		_assert (( (enum_code > 0) && (enum_code <= uil_max_enumval) ),
			"Enumeration code out of range");
		value_entry->value.l_integer = 
		    enumval_values_table[enum_code];
	    }

	    target_frame->b_flags = value_entry->obj_header.b_flags;
	    target_frame->b_type = value_entry->b_type;
	    target_frame->value.az_symbol_entry =
				(sym_entry_type *) value_entry;

	    return;
	}

	id_frame->value.az_symbol_entry = (sym_entry_type *) name_entry;
    }

    /* 
    ** first check if the name entry points to a value.
    ** If the value_entry is NULL, this is a forward reference.
    */

    name_entry = (sym_name_entry_type *) id_frame->value.az_symbol_entry;
    value_entry = (sym_value_entry_type *) name_entry->az_object;

    if (value_entry == NULL)
    {
	value_entry = sem_create_value_entry ("0", 0, sym_k_any_value);
	value_entry->obj_header.b_flags = sym_m_forward_ref;
	value_entry->obj_header.az_name = name_entry;
	target_frame->b_flags = value_entry->obj_header.b_flags;
	target_frame->b_type = value_entry->b_type;
	target_frame->value.az_symbol_entry = (sym_entry_type *) value_entry;
	return;
    }

    if (value_entry->header.b_tag != sym_k_value_entry && 
	value_entry->header.b_tag != sym_k_widget_entry)
    {
	diag_issue_diagnostic
	    ( d_ctx_req,
	      _sar_source_position( id_frame ),
	      diag_tag_text( sym_k_value_entry ),
	      diag_tag_text( value_entry->header.b_tag ) );
	goto error_path;
    }

    /* 
    ** set up the target frame
    */

    target_frame->b_flags = value_entry->obj_header.b_flags;
    target_frame->b_type = value_entry->b_type;
    target_frame->value.az_symbol_entry = (sym_entry_type *) value_entry;

    return;

error_path:

    target_frame->b_flags = sym_m_private;
    target_frame->b_type = sym_k_error_value;
    target_frame->value.az_symbol_entry =
		(sym_entry_type *) sym_az_error_value_entry;

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function checks an identifier and makes a name
**	entry for it if it is not already a name.
**
**  FORMAL PARAMETERS:
**
**      id_frame	pointer to token stack frame holding the identifier
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
**      a name entry may be generated
**
**--
**/

void	sar_process_id_ref ( id_frame )

yystype	    * id_frame;

{

    _assert( id_frame->b_tag == sar_k_token_frame,
	     "id frame missing from stack" );

    switch (id_frame->b_type) {

    /*
    ** if already a name, then do nothing.
    */

	case NAME:
	    return;

    /* 
    ** make the keyword into a name and insert it into the symbol table
    */

	case FONT_NAME:
	case ARGUMENT_NAME:
	case COLOR_NAME:
	case REASON_NAME:
	case CHILD_NAME:
	    
	    id_frame->b_type = NAME;

	    id_frame->value.az_symbol_entry =
		(sym_entry_type *) sym_insert_name (
		    id_frame->value.az_keyword_entry->b_length,
		    id_frame->value.az_keyword_entry->at_name );

	    break;

	default:
	    _assert( FALSE, "unexpected token" );
	    break;
    }

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function takes a parse frame, extracts the string for the token
**	out of it and converts the string into a units type.
**
**  FORMAL PARAMETERS:
**
**      parse_frame	pointer to parse stack frame holding the value
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
**      
**
**--
**/

int	sar_get_units_type ( parse_frame )

yystype	    *parse_frame;

{
    char *units_name;
    int units_type;
    XmParseResult result;

    units_name = parse_frame->value.az_keyword_entry->at_name;
    result = XmeParseUnits(units_name, &units_type);
    switch(result)
    {
      case XmPARSE_ERROR:
	/* I don't expect a parse error since the UIL compiler knows what
	   the valid unit strings are. */
	units_type = XmPIXELS;
	break;
      case XmPARSE_NO_UNITS:
	/* For now, just set the units to XmPIXELS when none specified.
	   What we really need is to be able to specify that there were
	   no units and therefore no conversion should be done. We can
	   sort of do that by specifying XmPIXELS here. */
	units_type = XmPIXELS;
	break;
      case XmPARSE_UNITS_OK:
	/* Everything is groovy */
	break;
    }
    return(units_type);
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function takes a token frame for a value and converts it 
**	into a value frame.
**
**  FORMAL PARAMETERS:
**
**      value_frame	pointer to resultant value stack frame
**      token_frame	pointer to token stack frame holding the value
**	value_type	type of value being created
**	keyword_frame	frame to use as locator for the value
**	arg_type	type of argument value being created - for args only
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
**      
**
**--
**/

void	sar_make_private_value
	    ( value_frame, token_frame, value_type, keyword_frame, arg_type )

yystype	    *value_frame;
yystype	    *token_frame;
int	    value_type;
yystype	    *keyword_frame;
int	    arg_type;

{
    sym_value_entry_type    *value_entry;
    /*
    **	This should be a long because the call to sem_create_value_entry passes the sizeof(long).   This also
    **	maps on top of value.l_integer which is a long and is used in sar_chk_comp_str_attr.
    */
    long		    boolean_value;

/*  Empty string tables and translation tables can have a null token frame.  */

    _assert( (token_frame->b_tag == sar_k_token_frame) ||
	     (token_frame->b_tag == sar_k_value_frame) ||
	     (token_frame->b_tag == sar_k_null_frame),
	     "token or value frame missing from stack" );

    /*
    ** The goal of the routine is to create a value entry in the
    ** symbol table for each constant and then place a pointer to
    ** that value entry in a value frame on the parse stack.
    ** Based on the type of constant, it may or may not already be
    ** in the symbol table.
    */

    switch (value_type)
    {
    case sym_k_char_8_value:
    case sym_k_compound_string_value:
    case sym_k_integer_value:
    case sym_k_float_value:
    case sym_k_single_float_value:
    case sym_k_localized_string_value:
	value_entry =
		(sym_value_entry_type *) token_frame->value.az_symbol_entry;
	/* Save the arg_type. This value should be zero for all cases above,
	   except when the integer or float has units specified. In that case,
	   the arg_type is used to store the type of units specified. */
	value_entry->b_arg_type = arg_type;
	value_entry->b_type = value_type;
	break;

    case sym_k_font_value:
    case sym_k_fontset_value:
    case sym_k_reason_value:
    case sym_k_argument_value:
    case sym_k_xbitmapfile_value:
    case sym_k_keysym_value:
    case sym_k_class_rec_name_value:
	/* 
	** transform the char 8 value entry into one for this
	** special type.  Before doing this, we need to insure
	** that the char 8 value is not in error or non private.
	*/
	value_entry =
		(sym_value_entry_type *) token_frame->value.az_symbol_entry;
	if (token_frame->b_type == sym_k_error_value)
	    {
	    value_type = sym_k_error_value;
	    }
	else
	    {
	    sym_value_entry_type	*value_save;

	    value_save = value_entry;
	    if ((value_entry->obj_header.az_name != NULL) ||
		(token_frame->b_type == sym_k_any_value))
		{
		value_entry =
		    sem_create_value_entry ("",0,sym_k_any_value);
	        value_entry->b_expr_opr = sym_k_coerce_op;
	        if ((token_frame -> b_flags & sym_m_forward_ref) != 0)
		    sym_make_value_forward_ref (token_frame,
		    (char*)&(value_entry->az_exp_op1), sym_k_patch_add);
	        else
	            value_entry->az_exp_op1 = value_save;
		}

	    value_entry->b_type = value_type;
	    value_entry->obj_header.b_flags = sym_m_private;

	    /* save the arg type for arguments */
	    if (value_type == sym_k_argument_value) 
		value_entry->b_arg_type = arg_type;
	    }
	
	break;

    case sym_k_bool_value:
	boolean_value = 0;
	if ((token_frame->b_type == UILTRUE) ||
	    (token_frame->b_type == ON ))
	    boolean_value = 1;
	value_entry =
	    sem_create_value_entry
		( (char*)&boolean_value, sizeof(long), sym_k_bool_value );

	break;

    case sym_k_integer_table_value:
    case sym_k_asciz_table_value:
    case sym_k_trans_table_value:
    case sym_k_string_table_value: 
    case sym_k_rgb_value:

    {
	int			count;
	sym_value_entry_type	* table_entry;

	/*  Save the pointer to the table elements   */

	table_entry =
		(sym_value_entry_type *) token_frame->value.az_symbol_entry;
	value_entry =
	    sem_create_value_entry
		(0, 0, value_type );
	value_entry->az_first_table_value = table_entry;
	

	/*  Get the count of elements in the table. Resets table_entry */

	for (table_entry=value_entry->az_first_table_value,
	     count = 0;
	     table_entry != NULL;
	     table_entry = table_entry->az_next_table_value,
	     count++) {
	}

	value_entry->b_table_count = count;

	break;

    }
    default:
	_assert( FALSE, "unexpected value type" );
	break;
    }

    /*
    ** make the target frame a value frame
    */

    _sar_move_source_info( value_frame, keyword_frame );
    value_frame->b_tag = sar_k_value_frame;
    value_frame->b_type = value_type;
    value_frame->b_flags = value_entry->obj_header.b_flags;
    value_frame->value.az_symbol_entry = (sym_entry_type *) value_entry;

}
 void    sar_make_rgb_private_value
            ( value_frame, token_frame, value_type, keyword_frame, arg_type )

yystype     *value_frame;
yystype     *token_frame;
int         value_type;
yystype     *keyword_frame;
int         arg_type;

{
/* placeholder RAP for RGB data type */

}









/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function takes a value frame which is the contents
**	of a table of strings and appends it to a value entry which is
**	the next entry in that table.  It is used by string_table, icon,
**	and translation_table value types.
**
**  FORMAL PARAMETERS:
**
**      value_frame	pointer to current table entry stack frame
**      table_frame	pointer to stack frame holding the table
**	table_type	dictates the type of table being generated
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
**      
**
**--
**/

void	sar_append_table_value( value_frame, table_frame, table_type, comma_frame )

yystype	    *value_frame;
yystype	    *table_frame;
int	    table_type;
yystype	    *comma_frame;

{
    sym_value_entry_type    *value_entry, *table_entry;
    int			    value_type;

    _assert( (value_frame->b_tag == sar_k_value_frame),
	     "value frame missing from stack" );

    value_entry = (sym_value_entry_type *) value_frame->value.az_symbol_entry;
    value_type = value_entry->b_type;

    if (value_type == sym_k_error_value) return;

    table_entry = (sym_value_entry_type *) table_frame->value.az_symbol_entry;

    /*
    ** Some entries require checking for a forward reference. If the
    ** table entry is named, than a valref entry linked to it is placed
    ** in the table. If the new entry is a forward reference, than a
    ** valref is always required, but we can use the value node which
    ** is supplied as it is a parser artifact, and has no other use.
    */

    switch (table_type)
	{
	case sym_k_icon_value:
	    if ((value_frame->b_flags & sym_m_forward_ref) != 0)
		{
		diag_issue_diagnostic
		    (d_undefined,
		     _sar_source_position(value_frame),
		     "icon row",
		     value_entry->obj_header.az_name->c_text);
		}

	    if (value_entry->obj_header.az_name != NULL)
		{
		sym_value_entry_type	*value_save;

		value_save = value_entry;
		value_entry = sem_create_value_entry (0, 0, value_type);
		value_entry->b_type = value_type;
		value_entry->obj_header.b_flags = sym_m_private;
		value_entry->b_expr_opr = sym_k_valref_op;
		value_entry->az_exp_op1 = value_save;
		}
	    break;

	case sym_k_trans_table_value:
	case sym_k_asciz_table_value:
	case sym_k_integer_table_value:
	case sym_k_rgb_value:
	    if (value_entry->obj_header.az_name != NULL)
		{
		sym_value_entry_type	*value_save;
		
		if ( (value_frame->b_flags & sym_m_forward_ref) != 0)
		    {
		    value_entry->obj_header.b_flags = sym_m_private;
		    value_entry->b_expr_opr = sym_k_valref_op;	
		    sym_make_value_forward_ref
			(value_frame,
			 (char*)&(value_entry->az_exp_op1),
			 sym_k_patch_add);
		    }
		else
		    {
		    value_save = value_entry;
		    value_entry = sem_create_value_entry (0, 0, value_type);
		    value_entry->b_type = value_type;
		    value_entry->obj_header.b_flags = sym_m_private;
		    value_entry->b_expr_opr = sym_k_valref_op;
		    value_entry->az_exp_op1 = value_save;
		    }
		}
	    break;

	case sym_k_string_table_value:
	    /*
	    ** value needs to be a compound string, so a coerce operator is
	    ** inserted as required. We don't need both a coerce and a
	    ** valref entry; if coerce is applied to a name, it also
	    ** functions as the valref.
	    */
	    if (value_entry->obj_header.az_name != NULL)
		{
		sym_value_entry_type	*value_save;
		
		if ( (value_frame->b_flags & sym_m_forward_ref) != 0)
		    {
		    value_entry->obj_header.b_flags = sym_m_private;
		    value_entry->b_type = sym_k_compound_string_value;
		    value_entry->b_expr_opr = sym_k_coerce_op;	
		    sym_make_value_forward_ref
			(value_frame,
			 (char*)&(value_entry->az_exp_op1),
			 sym_k_patch_add);
		    }
		else
		    {
		    value_save = value_entry;
		    value_entry = sem_create_value_entry (0, 0, value_type);
		    value_entry->obj_header.b_flags = sym_m_private;
		    value_entry->b_type = sym_k_compound_string_value;
		    value_entry->az_exp_op1 = value_save;
		    if ( value_type == sym_k_compound_string_value )
			value_entry->b_expr_opr = sym_k_valref_op;
		    else
			value_entry->b_expr_opr = sym_k_coerce_op;
		    }
		}
	    break;
	    
	default:
	    _assert ( FALSE, "unknown table type found");
	}

    /*	
    ** Prepend the value to the table.  The table elements will be
    ** in reverse order.  
    */
    value_entry->b_aux_flags |= sym_m_table_entry;
    value_entry->az_next_table_value = table_entry;

    /*
    ** Save source information
    */
    _sar_save_source_info ( &value_entry->header, comma_frame, value_frame);

    value_entry->header.b_type = value_frame->b_source_pos;
	
    /*
    ** make the target frame a value frame
    */
    value_frame->b_tag = sar_k_value_frame;
    value_frame->b_type = value_type;
    value_frame->b_flags = value_entry->obj_header.b_flags;
    value_frame->value.az_symbol_entry = (sym_entry_type *) value_entry;

}



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function issues an error message saying that a feature is
**	not implemented yet.
**
**  FORMAL PARAMETERS:
**
**      value_frame	pointer to resultant value stack frame
**      token_frame	pointer to source token frame (error position info)
**      error_text	pointer to text to be substituted in message
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
**      error message is issued
**
**--
**/

void	sar_value_not_implemented( value_frame, token_frame, error_text )

yystype	    *value_frame;
yystype	    *token_frame;
char	    *error_text;

{
    /*
    ** make the target frame an error value frame
    */

    _sar_move_source_info( value_frame, token_frame );
    value_frame->b_tag = sar_k_value_frame;
    value_frame->b_type = sym_k_error_value;
    value_frame->b_flags = sym_m_private;
    value_frame->value.az_symbol_entry =
	(sym_entry_type *) sym_az_error_value_entry;

    diag_issue_diagnostic
	( d_not_impl,
	  _sar_source_position( value_frame ),
	  error_text );
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function processes the concatenation of 2 strings.
**
**  FORMAL PARAMETERS:
**
**      operator_frame	[in/out] pointer to resultant value stack frame
**      op1_frame	[in] pointer to operand 1 value frame 
**      op2_frame	[in] pointer to operand 2 value frame 
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

void	sar_cat_value( operator_frame, op1_frame, op2_frame )

yystype	    *operator_frame;
yystype	    *op1_frame;
yystype	    *op2_frame;

{

/*
**  For pcc conversion, use defines instead of this enum.
**
**    enum op_state
**    {
**	error=0, simple, compound, localized
**    };
*/

#define		k_op_state_error	0
#define		k_op_state_simple	1
#define		k_op_state_compound	2
#define		k_op_state_localized	4

    int			    target_type;
    sym_value_entry_type    *value1_entry;
    sym_value_entry_type    *value2_entry;
    sym_value_entry_type    *target_entry;
    unsigned int	    op1_state;
    unsigned int	    op2_state;

    _assert( (op1_frame->b_tag == sar_k_value_frame) &&
	     (op2_frame->b_tag == sar_k_value_frame), "value frame missing" );

    /*
    **  The target type is dependent on the type of the sources.  If both
    **	operands are primitive and have the same writing direction and
    **	charset, the result is still of that type.  If not, the result
    **	is a compound string.
    */

    switch (op1_frame->b_type)
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
	    (d_wrong_type,
	     _sar_source_position( op1_frame ),
	     diag_value_text( op1_frame->b_type),
	     "string or compound string");
	op1_state = k_op_state_error;
    }

    switch (op2_frame->b_type)
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
	    (d_wrong_type,
	     _sar_source_position( op2_frame ),
	     diag_value_text( op2_frame->b_type),
	     "string or compound string");
	op2_state = k_op_state_error;
    }

    value1_entry = (sym_value_entry_type *) op1_frame->value.az_symbol_entry;
    value2_entry = (sym_value_entry_type *) op2_frame->value.az_symbol_entry;

    /*
    **	Verify that both operands are private values
    */

    if ((op1_frame->b_flags & sym_m_private) == 0) 
	{
	op1_state = k_op_state_error;
	diag_issue_diagnostic
		(d_nonpvt,
		_sar_source_position (op1_frame),
		value1_entry->obj_header.az_name->c_text );
	}
    if ((op2_frame->b_flags & sym_m_private) == 0) 
	{
	op2_state = k_op_state_error;
	diag_issue_diagnostic
		(d_nonpvt,
		_sar_source_position (op2_frame),
		value2_entry->obj_header.az_name->c_text );
	}	

    switch (op1_state + (op2_state<<2))
    {
    /*
    **	This is the case of appending to simple strings.  Just append them
    **	unless they have different directions or the first one has the separate
    **	attribute.
    */
    case k_op_state_simple + (k_op_state_simple<<2):
	if ((value1_entry->b_charset == value2_entry->b_charset) 
	    &&
	    ((value1_entry->b_direction) == (value2_entry->b_direction))
	    && 
	    ((value1_entry->b_aux_flags & sym_m_separate) == 0))
	    {
	    target_entry = (sym_value_entry_type *)
		sem_cat_str_to_str
		    (value1_entry, (value1_entry->obj_header.az_name==NULL),
		     value2_entry, (value2_entry->obj_header.az_name==NULL));
	    target_type  = sym_k_char_8_value;
	    }
	else
	    {
	    target_entry = (sym_value_entry_type *) sem_create_cstr( );
	    sem_append_str_to_cstr
		(target_entry, 
		 value1_entry, (value1_entry->obj_header.az_name==NULL));
	    sem_append_str_to_cstr
		(target_entry, 
		 value2_entry, (value2_entry->obj_header.az_name==NULL));
	    target_type  = sym_k_compound_string_value;
	    }
	break;

    /*
    **	This is the case of one simple and one compound string.  Change the
    **	simple to a compound and append them together.  Depend on the append
    **	routine to do the right thing.
    */
    case k_op_state_simple + (k_op_state_compound<<2):
	target_entry = (sym_value_entry_type *) sem_create_cstr( );
	sem_append_str_to_cstr
	    (target_entry, 
	     value1_entry, (value1_entry->obj_header.az_name==NULL));
	sem_append_cstr_to_cstr
	    (target_entry, 
	     value2_entry, (value2_entry->obj_header.az_name==NULL));
	target_type  = sym_k_compound_string_value;
	break;

    /*
    **	This is the case of one simple and one compound string.  Append the
    **	simple to the compound.  Depend on the append routine to do the right
    **	thing.
    */
    case k_op_state_compound + (k_op_state_simple<<2):
	target_entry = (sym_value_entry_type *) sem_create_cstr( );
	sem_append_cstr_to_cstr
	    (target_entry, 
	     value1_entry, (value1_entry->obj_header.az_name==NULL));
	sem_append_str_to_cstr
	    (target_entry, 
	     value2_entry, (value2_entry->obj_header.az_name==NULL));
	target_type  = sym_k_compound_string_value;
	break;

    /*
    **  This is the case of two compound strings.  Just let the append routine
    **  do the right thing.
    */
    case k_op_state_compound + (k_op_state_compound<<2):
	target_entry = (sym_value_entry_type *) sem_create_cstr( );
	sem_append_cstr_to_cstr
	    (target_entry, 
	     value1_entry, (value1_entry->obj_header.az_name==NULL));
	sem_append_cstr_to_cstr
	    (target_entry, 
	     value2_entry, (value2_entry->obj_header.az_name==NULL));
	target_type  = sym_k_compound_string_value;
	break;

    default:   /* some form of error */
	target_type = sym_k_error_value;
	target_entry = (sym_value_entry_type *) sym_az_error_value_entry;
	break;
    }

    /*
    ** initialize the target frame
    */

    _sar_save_source_pos ( &target_entry->header, op2_frame);

/*    target_entry->az_source_rec = op2_frame->az_source_record; */

    operator_frame->b_tag = sar_k_value_frame;
    operator_frame->b_type = target_type;
    operator_frame->b_flags = sym_m_private;
    operator_frame->value.az_symbol_entry = (sym_entry_type *) target_entry;


}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function checks the attributes of COMPOUND_STRING function.
**
**  FORMAL PARAMETERS:
**
**      target_frame		pointer to resultant token stack frame
**      value_frame		pointer to frame holding keyword and value
**      prior_value_frame	pointer to previous properties
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
**      attribute information is stuffed in target frame
**
**--
**/

void	sar_chk_comp_str_attr( target_frame, value_frame, prior_value_frame )

yystype	    *target_frame;
yystype	    *value_frame;
yystype	    *prior_value_frame;
{
sym_value_entry_type	*value_entry;

    /*
    **	Set up not specified values in the target frame.
    **	    b_type will hold the writing direction and separate propertied
    **	    az_symbol_entry will hold the pointer to the character set.
    */

    switch (prior_value_frame->b_tag)
    {
    case sar_k_null_frame:
	/*
	**	no prior values
	*/

	target_frame->b_tag = sar_k_token_frame;
	target_frame->b_direction = NOSTRING_DIRECTION;

     /* Fix for CN 16149 (DTS 10023) part 3 -- flag b_charset as non-existent */
	target_frame->b_charset = sym_k_error_charset;
	target_frame->b_type = 0;
	target_frame->value.az_symbol_entry = NULL;
	break;

    case sar_k_token_frame:
    case sar_k_value_frame:
	/*
	**  prior values - transfer them
	*/

	target_frame->b_tag = sar_k_token_frame;
	target_frame->b_direction = prior_value_frame->b_direction;
	target_frame->b_charset = prior_value_frame->b_charset;
	target_frame->b_type = prior_value_frame->b_type;
	target_frame->value.az_symbol_entry =
	    target_frame->value.az_symbol_entry;
	break;
    default:
	_assert( FALSE, "prior value frame missing from stack" );
    }

    /*
    **	Case on the keyword for the attribute given
    */

	value_entry =
		(sym_value_entry_type *) value_frame->value.az_symbol_entry;

        if ((value_entry != NULL) && (value_frame->b_type != CHARACTER_SET) &&
	    (value_entry->obj_header.b_flags & sym_m_forward_ref) != 0)
	    {
	    diag_issue_diagnostic
		(d_undefined,
		_sar_source_position(value_frame),
		"compound string attribute",
		value_entry->obj_header.az_name->c_text);
	    }

    switch (value_frame->b_type)
    {
    case RIGHT_TO_LEFT:
    {
	/*
	**  If the value is a boolean, then set the b_direction field.
	*/
	if (value_entry->b_type == sym_k_bool_value)
	    if (value_entry->value.l_integer == TRUE)
		target_frame->b_direction = XmSTRING_DIRECTION_R_TO_L;
	    else
		target_frame->b_direction = XmSTRING_DIRECTION_L_TO_R;

	break;
    }

    case SEPARATE:
    {
	/*
	**  If the value is a boolean, then just set the corresponding mask 
	**  accordingly.
	*/
	if (value_entry->b_type == sym_k_bool_value)
	    if (value_entry->value.l_integer == TRUE)
		target_frame->b_type |= sym_m_separate; 
	    else
		target_frame->b_type &= ~sym_m_separate; 

	break;
    }

    case CHARACTER_SET:
    {
	/*
	**  There are two different kinds of character sets.  One is a
	**  token frame, the other is a value frame which points to a
	**  char8 string value in the symbol table that represents the charset.
	*/
	switch (value_frame->b_tag) 
	{
	    /*
	    **  For token frames, acquire the charset from the keytable entry
	    **  and set frame type so sar_make_comp_str knows how to interpret
	    **  the frame.
	    */
	    case sar_k_token_frame:
	        {
		key_keytable_entry_type	*keyword_entry;
		
		keyword_entry = (key_keytable_entry_type *)
		    value_frame->value.az_keyword_entry;
		target_frame->b_tag = sar_k_token_frame;
		target_frame->b_charset =
		    sem_map_subclass_to_charset( keyword_entry->b_subclass );
		break;
		}
	    /*
	    **  For value frames, save the value pointer and mark the
	    **  frame again for correct use by sar_make_comp_str.
	    */
	    case sar_k_value_frame:
		target_frame->b_tag = sar_k_value_frame;
		target_frame->value.az_symbol_entry =
		    value_frame->value.az_symbol_entry;
		break;
	}
	break;

    }

    default:
	_assert( FALSE, "keyword missing from stack" );
    }

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function makes a COMPOUND_STRING and sets the properties
**	of the string.
**
**  FORMAL PARAMETERS:
**
**      target_frame	pointer to resultant token stack frame
**	value_frame	pointer to string value
**      attr_frame	pointer to strings attributes
**	keyword_frame	frame to use as locator for result
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
**      none
**
**--
**/

void	sar_make_comp_str
    ( target_frame, value_frame, attr_frame, keyword_frame )

yystype	    *target_frame;
yystype	    *value_frame;
yystype	    *attr_frame;
yystype	    *keyword_frame;
{

    sym_value_entry_type    *value_entry;
    sym_value_entry_type    *cstr_entry;

    _assert( value_frame->b_tag == sar_k_value_frame,
	     "value frame missing from stack" );

    /*
    ** Make a compound string operation.  The compound string will be created
    ** during expression evaluation in UilSemVal.c.
    */

    cstr_entry = (sym_value_entry_type *) sem_create_cstr();
    cstr_entry->b_expr_opr = sym_k_comp_str_op;
    if ((value_frame->b_flags & sym_m_forward_ref) != 0)
	sym_make_value_forward_ref (value_frame,
	    (char*)&(cstr_entry->az_exp_op1), sym_k_patch_add);
    else
	{
	value_entry =
	    (sym_value_entry_type *) value_frame->value.az_symbol_entry;
	cstr_entry->az_exp_op1 = value_entry;
	}

    /*
    **  If the attr_frame is not null, it must be a value frame with contains
    **  a pointer to the value entry for the userdefined charset, or a token frame
    **  which contains the charset token subclass.
    */
    switch (attr_frame->b_tag)
    {
    case sar_k_value_frame:
	/*
	**  Set the attributes of the string, as specified by the options
	**  to the COMPOUND_STRING function, without disturbing any
	**  existing bits.
	*/
	cstr_entry->b_direction = attr_frame->b_direction;
	cstr_entry->b_aux_flags |= (attr_frame->b_type & sym_m_separate);
	    
	/*
	**  If the symbol_entry pointer is not null then a charset was
	**  specified for this CS,  just copy the b_charset and
	**  az_charset_value pointers into the value entry for this CS.
	*/
	if ((attr_frame->value.az_symbol_entry) != 0)
	    {
	    sym_value_entry_type	* az_value_entry;
	    
	    az_value_entry = (sym_value_entry_type *)
	        attr_frame->value.az_symbol_entry;
	    cstr_entry->b_charset = az_value_entry->b_charset;
	    cstr_entry->b_direction = az_value_entry->b_direction;
	    cstr_entry->az_charset_value =
	        az_value_entry;
	    }

	break;

    case sar_k_token_frame:
	if ((attr_frame->b_charset) != 0)
	    cstr_entry->b_charset = 
	        sem_map_subclass_to_charset (attr_frame->b_charset);
	cstr_entry->b_direction = attr_frame->b_direction;
	cstr_entry->b_aux_flags |= (attr_frame->b_type & sym_m_separate);
	break;
    }

    /*
    ** initialize the target frame
    */

    _sar_move_source_info( target_frame, keyword_frame );

    _sar_save_source_pos ( &cstr_entry->header, value_frame );

/*    cstr_entry->az_source_rec = value_frame->az_source_record; */

    target_frame->b_tag = sar_k_value_frame;
    target_frame->b_type = sym_k_compound_string_value;
    target_frame->b_flags = sym_m_private;
    target_frame->value.az_symbol_entry =
			(sym_entry_type *)  cstr_entry;

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function makes a COMPOUND_STRING_COMPONENT and sets the properties
**	of the string.
**
**  FORMAL PARAMETERS:
**
**      target_frame	pointer to resultant token stack frame
**	type_frame	pointer to type value
**      value_frame	pointer to component value
**	keyword_frame	frame to use as locator for result
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
**      none
**
**--
**/

void	sar_make_comp_str_comp
    ( target_frame, type_frame, value_frame, keyword_frame )

yystype	    *target_frame;
yystype	    *type_frame;
yystype	    *value_frame;
yystype	    *keyword_frame;
{
  key_keytable_entry_type	*type_entry;
  key_keytable_entry_type	*value_entry;
  sym_value_entry_type		*cstr_entry;
  unsigned short int		enumval_code;
  unsigned short int		enumset_code;
  unsigned short int		type;
  char				*cset_name;
  int				i;
  Boolean			found;
  String			str;
  XmStringDirection		dir;
  XmDirection			lay_dir;
  XmString			cstr_r = NULL;
  
  _assert(type_frame->b_tag == sar_k_token_frame,
	  "value frame missing from stack" );

  /*
   ** Make a compound string component. 
   */

  cstr_entry = (sym_value_entry_type *) sem_create_cstr();

  /* Evaluate type. */
  type_entry = (key_keytable_entry_type *)type_frame->value.az_keyword_entry;

  enumval_code = type_entry->b_subclass;
  enumset_code = argument_enumset_table[sym_k_XmStringComponent_arg];
  found = FALSE; 
  for (i = 0; i < enum_set_table[enumset_code].values_cnt; i++)
    if (enum_set_table[enumset_code].values[i] == enumval_code)
      {
	found = TRUE;
	break;
      }
  
  if (found) type = enumval_values_table[enumval_code];
  else 
    {
      diag_issue_diagnostic(d_arg_type,
			    _sar_source_position(type_frame),
			    uil_enumval_names[enumval_code],
			    "compound_string_component",
			    "XmStringComponentType");
      
      type = XmSTRING_COMPONENT_UNKNOWN;
    }
  
  switch (type)
    {
    case XmSTRING_COMPONENT_UNKNOWN:
    case XmSTRING_COMPONENT_SEPARATOR:
    case XmSTRING_COMPONENT_LAYOUT_POP:
    case XmSTRING_COMPONENT_TAB:
      /* If value_frame is not null, issue diagnostic. */
      if (value_frame->b_tag != sar_k_null_frame)
	diag_issue_diagnostic(d_arg_type,
			      _sar_source_position(value_frame),
			      "non-NULL",
			      "compound_string_component",
			      "NULL");

      cstr_r = XmStringComponentCreate(type, 0, NULL);
      break;

    case XmSTRING_COMPONENT_LOCALE:
      if ((value_frame->b_tag != sar_k_null_frame) &&
	  (value_frame->b_type != CHARSET_NAME))
	diag_issue_diagnostic(d_arg_type,
			      _sar_source_position(value_frame),
			      "non-NULL",
			      "compound_string_component",
			      "NULL or _MOTIF_DEFAULT_LOCALE");
      else if (value_frame->b_type == CHARSET_NAME) 
	{
	  cset_name =
	    sem_charset_name((value_frame->value.az_keyword_entry)->b_subclass,
			     (sym_value_entry_type *)
			     (value_frame->value.az_keyword_entry));
	  if (strcmp(cset_name, "_MOTIF_DEFAULT_LOCALE") != 0)
	    diag_issue_diagnostic(d_arg_type,
				  _sar_source_position(value_frame),
				  cset_name,
				  "compound_string_component",
				  "_MOTIF_DEFAULT_LOCALE");
	}

      cstr_r = XmStringComponentCreate(type, strlen(_MOTIF_DEFAULT_LOCALE),
				       _MOTIF_DEFAULT_LOCALE);
      break;

    case XmSTRING_COMPONENT_CHARSET:
    case XmSTRING_COMPONENT_TEXT:
    case XmSTRING_COMPONENT_LOCALE_TEXT:
    case XmSTRING_COMPONENT_WIDECHAR_TEXT:
    case XmSTRING_COMPONENT_RENDITION_BEGIN:
    case XmSTRING_COMPONENT_RENDITION_END:
      str = "";
      
      if (value_frame->b_tag == sar_k_null_frame)
	diag_issue_diagnostic(d_arg_type,
			      type_frame->az_source_record,
			      type_frame->b_source_end,
			      "NULL",
			      "compound_string_component",
			      diag_value_text(sym_k_char_8_value));
      else if ((value_frame->b_type != CHAR_8_LITERAL) &&
	       (value_frame->b_type != LOC_STRING) &&
	       ((value_frame->b_type != CHARSET_NAME) ||
		(type != XmSTRING_COMPONENT_CHARSET)))
	diag_issue_diagnostic(d_arg_type,
			      _sar_source_position(value_frame),
			      tok_token_name_table[value_frame->b_type],
			      "compound_string_component",
			      diag_value_text(sym_k_char_8_value));
      else 
	{
	  if (value_frame->b_type == CHARSET_NAME)
	    {
	      cset_name =
		sem_charset_name((value_frame->value.az_keyword_entry)->b_subclass,
				 (sym_value_entry_type *)
				 (value_frame->value.az_keyword_entry));
	      if (strcmp(cset_name, "XmFONTLIST_DEFAULT_TAG") == 0)
		str = XmFONTLIST_DEFAULT_TAG;
	      else str = cset_name;
	    }
	  else
	    /* Extract string */
	    str = ((sym_value_entry_type *)
		   (value_frame->value.az_symbol_entry))->value.c_value;
	}
      
      cstr_r = XmStringComponentCreate(type, strlen(str), (XtPointer)str);
      break;

    case XmSTRING_COMPONENT_DIRECTION:
      if (value_frame->b_tag == sar_k_null_frame)
	diag_issue_diagnostic(d_arg_type,
			      type_frame->az_source_record,
			      type_frame->b_source_end,
			      "NULL",
			      "compound_string_component",
			      "XmStringDirection");
      else if (value_frame->b_type != ENUMVAL_NAME)
	diag_issue_diagnostic(d_arg_type,
			      _sar_source_position(value_frame),
			      diag_value_text(value_frame->b_type),
			      "compound_string_component",
			      "XmStringDirection");
      else
	{
	  /* Extract and validate enumval */
	  value_entry = (key_keytable_entry_type *)value_frame->value.az_keyword_entry;

	  enumval_code = value_entry->b_subclass;
	  enumset_code = argument_enumset_table[sym_k_XmNstringDirection_arg];
	  found = FALSE; 
	  for (i = 0; i < enum_set_table[enumset_code].values_cnt; i++)
	    if (enum_set_table[enumset_code].values[i] == enumval_code)
	      {
		found = TRUE;
		break;
	      }
  
	  if (found) dir = enumval_values_table[enumval_code];
	  else
	    {
	      diag_issue_diagnostic(d_arg_type,
				    _sar_source_position(value_frame),
				    uil_enumval_names[enumval_code],
				    "compound_string_component",
				    "XmStringDirection");
      
	      dir = XmSTRING_DIRECTION_L_TO_R;
	    }
	}

      cstr_r = XmStringComponentCreate(type, sizeof(XmStringDirection), &dir);
      break;

    case XmSTRING_COMPONENT_LAYOUT_PUSH:
      if (value_frame->b_tag == sar_k_null_frame)
	diag_issue_diagnostic(d_arg_type,
			      type_frame->az_source_record,
			      type_frame->b_source_end,
			      "NULL",
			      "compound_string_component",
			      "XmDirection");
      else if (value_frame->b_type != ENUMVAL_NAME)
	diag_issue_diagnostic(d_arg_type,
			      _sar_source_position(value_frame),
			      diag_value_text(value_frame->b_type),
			      "compound_string_component",
			      "XmDirection");
      else
	{
	  /* Extract and validate enumval */
	  value_entry = (key_keytable_entry_type *)value_frame->value.az_keyword_entry;

	  enumval_code = value_entry->b_subclass;
	  enumset_code = argument_enumset_table[sym_k_XmNlayoutDirection_arg];
	  found = FALSE; 
	  for (i = 0; i < enum_set_table[enumset_code].values_cnt; i++)
	    if (enum_set_table[enumset_code].values[i] == enumval_code)
	      {
		found = TRUE;
		break;
	      }
  
	  if (found) lay_dir = enumval_values_table[enumval_code];
	  else
	    {
	      diag_issue_diagnostic(d_arg_type,
				    _sar_source_position(value_frame),
				    uil_enumval_names[enumval_code],
				    "compound_string_component",
				    "XmDirection");
      
	      lay_dir = XmLEFT_TO_RIGHT;
	    }
	}

      cstr_r = XmStringComponentCreate(type, sizeof(XmDirection), &lay_dir);
      break;
    }

  cstr_entry->value.xms_value = cstr_r;
  cstr_entry->w_length = XmStringLength(cstr_r);

  cstr_entry->az_first_table_value = NULL;

  _assert(cstr_entry->w_length <= MrmMaxResourceSize, "compound string too long" );

  /*
   ** initialize the target frame
   */
  _sar_move_source_info( target_frame, keyword_frame );

  _sar_save_source_pos ( &cstr_entry->header, type_frame );

  target_frame->b_tag = sar_k_value_frame;
  target_frame->b_type = sym_k_compound_string_value;
  target_frame->b_flags = sym_m_private;
  target_frame->value.az_symbol_entry = (sym_entry_type *)cstr_entry;
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function makes a WIDE_CHARACTER and sets the properties
**	of the string.
**
**  FORMAL PARAMETERS:
**
**      target_frame	pointer to resultant token stack frame
**	value_frame	pointer to string value
**      attr_frame	pointer to strings attributes
**	keyword_frame	frame to use as locator for result
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
**      none
**
**--
**/

void	sar_make_wchar_str
    ( target_frame, value_frame, attr_frame, keyword_frame )

yystype	    *target_frame;
yystype	    *value_frame;
yystype	    *attr_frame;
yystype	    *keyword_frame;
{

    sym_value_entry_type    *value_entry;
    sym_value_entry_type    *wchar_str_entry;

    _assert( value_frame->b_tag == sar_k_value_frame,
	     "value frame missing from stack" );

    /*
    ** Make a wide_character string operation.  The wide_character string will
    ** be created during retrieval from the UID file.
    */

    wchar_str_entry = (sym_value_entry_type *) sem_create_wchar_str();
    wchar_str_entry->b_expr_opr = sym_k_wchar_str_op;
    if ((value_frame->b_flags & sym_m_forward_ref) != 0)
	sym_make_value_forward_ref (value_frame,
	    (char*)&(wchar_str_entry->az_exp_op1), sym_k_patch_add);
    else
	{
	value_entry =
	    (sym_value_entry_type *) value_frame->value.az_symbol_entry;
	value_entry->b_type = value_frame->b_type;
	wchar_str_entry->az_exp_op1 = value_entry;
	}

    /*
    ** initialize the target frame
    */

    _sar_move_source_info( target_frame, keyword_frame );

    _sar_save_source_pos ( &wchar_str_entry->header, value_frame );

    target_frame->b_tag = sar_k_value_frame;
    target_frame->b_type = sym_k_wchar_string_value;
    target_frame->b_flags = sym_m_private;
    target_frame->value.az_symbol_entry =
			(sym_entry_type *)wchar_str_entry;

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function issues an error message saying that the value is
**	the wrong type for this context.
**
**  FORMAL PARAMETERS:
**
**      value_frame	pointer to resultant value stack frame
**      expected_type	type of constant required by this context
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
**      error message is issued
**
**--
**/

void	sar_value_type_error( value_frame, expected_type )

yystype	    *value_frame;
int	    expected_type;


{

    _assert( value_frame->b_tag == sar_k_value_frame, "value frame missing" );

    /*
    ** make the target frame an error value frame
    */

    if (value_frame->b_type != sym_k_error_value)
	diag_issue_diagnostic
	    ( d_wrong_type,
	      _sar_source_position( value_frame ),
	      diag_value_text( value_frame->b_type ),
	      diag_value_text( expected_type ) );

    value_frame->b_type = sym_k_error_value;
    value_frame->b_flags = sym_m_private;
    value_frame->value.az_symbol_entry =
		(sym_entry_type *) sym_az_error_value_entry;

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function issues an error message saying that the value
**	must be private.  Expect for arguments and argument values,
**	values used by UIL need to be private rather than public.
**	To make them public would mean that URM would need to perform
**	the function at runtime.
**
**  FORMAL PARAMETERS:
**
**      value_frame	pointer to resultant value stack frame
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
**      error message is issued
**
**--
**/

void	sar_private_error( value_frame )

yystype	    *value_frame;


{

    _assert( value_frame->b_tag == sar_k_value_frame, "value frame missing" );

    /*
    ** make the target frame an error value frame
    */

    if (value_frame->b_type != sym_k_error_value)
    {
	sym_value_entry_type    *value_entry;

	value_entry =
		(sym_value_entry_type *) value_frame->value.az_symbol_entry;

	diag_issue_diagnostic
	    ( d_nonpvt,
	      _sar_source_position( value_frame ),
	      value_entry->obj_header.az_name->c_text );

    }

    value_frame->b_type = sym_k_error_value;
    value_frame->b_flags = sym_m_private;
    value_frame->value.az_symbol_entry =
		(sym_entry_type *) sym_az_error_value_entry;

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function creates a value entry for an imported value.
**
**  FORMAL PARAMETERS:
**
**      target_frame	    ptr to target value frame on parse stack
**	token_frame	    ptr to token frame giving the data type
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
**      none
**
**--
**/

void	sar_import_value_entry(target_frame, token_frame)

yystype	    *target_frame;
yystype	    *token_frame;
{
    sym_value_entry_type    *value_entry;

    _assert( token_frame->b_tag == sar_k_token_frame, "token frame missing" );

    /* 
    ** Need to create a value entry and mark it as imported.
    ** The b_type field of the token has been set to the type of value
    ** by a prior grammar reduction
    */

    value_entry = (sym_value_entry_type *)
	sem_allocate_node (sym_k_value_entry, sym_k_value_entry_size);
    _sar_save_source_pos ( &value_entry->header, &yylval );

/*    value_entry->az_source_rec = yylval.az_source_record; */

    value_entry->b_type = token_frame->b_type;
    value_entry->obj_header.b_flags = sym_m_imported;

    /* 
    ** set up the target frame
    */

    target_frame->b_tag = sar_k_value_frame;
    target_frame->b_flags = sym_m_imported;
    target_frame->b_type = value_entry->b_type;
    target_frame->value.az_symbol_entry =
		(sym_entry_type *) value_entry;

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function binds the name of a value with its value.
**
**  FORMAL PARAMETERS:
**
**      id_frame	ptr to token frame holding the name for the value
**	value_frame	ptr to value frame
**
**  IMPLICIT INPUTS:
**
**	sym_az_current_section_entry	global pointer to the "current" section list
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
**      errors if name has previously been used to declare another object
**	may be a new value entry
**
**--
**/

void	sar_bind_value_name(id_frame, value_frame, semi_frame)

yystype	    *id_frame;
yystype	    *value_frame;
yystype	    *semi_frame;

{
    sym_name_entry_type	    *name_entry;
    sym_value_entry_type    *value_entry;
    sym_section_entry_type  *section_entry;
    int			    flags;
    boolean		    error;

    _assert( id_frame->b_tag == sar_k_token_frame, "id frame missing" );
    _assert( value_frame->b_tag == sar_k_value_frame, "value frame missing" );

    /* 
    ** First we check on the name to see if it has been previously used.
    ** This function returns NULL if name cannot be used, in which case
    ** processing is over.
    */

    name_entry = sem_dcl_name( id_frame );

    if (name_entry == NULL)
	return;

    /*
    ** Processing is now based on where the value is private, imported, or
    ** exported.
    */

    value_entry = (sym_value_entry_type *) value_frame->value.az_symbol_entry;
    flags = value_frame->b_flags;
    error = (value_frame->b_type == sym_k_error_value);

    if ((flags & sym_m_imported) == 0)
    {
	if ((value_entry->obj_header.az_name != NULL) || error)
	{
	    /*
	    ** Create a new value node for the unary value reference operator.
	    ** az_exp_op1 will point to the node being referenced.
	    */

	    sym_value_entry_type	*saved_value_entry_ptr;

	    saved_value_entry_ptr = value_entry;

	    value_entry = 
		    sem_create_value_entry ("",0,sym_k_any_value);

	    value_entry->b_expr_opr = sym_k_valref_op;

	    /*
	    ** If the value is a forward reference, we'll patch in the
	    ** address of the the referenced value between passes.  Otherwise,
	    ** just point to the referenced value node.
	    */

    	    if ((value_frame->b_flags & sym_m_forward_ref) != 0)
		sym_make_value_forward_ref (value_frame, 
		(char*)&(value_entry->az_exp_op1), sym_k_patch_add);
	    else
	        value_entry->az_exp_op1 = 
		    saved_value_entry_ptr;
	}

	if ((flags & sym_m_exported) != 0)
	    sym_make_external_def( name_entry );

    }

    /*
    **  Place the name and flags in the value entry.
    */

    value_entry->obj_header.az_name = name_entry;
    value_entry->obj_header.b_flags = 
      (value_entry->obj_header.b_flags & clear_class_mask) | flags;

    name_entry->az_object = (sym_entry_type *) value_entry;

    /*
    ** save the source file info for this value entry
    */
    _sar_save_source_info (&value_entry->header, id_frame, semi_frame );

    /*
    ** allocate a section entry to link the value entry into the structure
    */
    section_entry = (sym_section_entry_type *) sem_allocate_node
	(sym_k_section_entry, sym_k_section_entry_size);

    /*
    ** Link this entry off of the current section list
    */
    section_entry->next = (sym_entry_type *)
	sym_az_current_section_entry->entries;
    sym_az_current_section_entry->entries = (sym_entry_type *) section_entry;
    section_entry->entries = (sym_entry_type *)value_entry;

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function checks to see if a name is available for naming a
**	construct.
**
**  FORMAL PARAMETERS:
**
**      id_frame	ptr to a token frame on the parse stack holding the name
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
**      ptr to name entry or NULL
**
**  SIDE EFFECTS:
**
**      name entry may be created
**	error message for duplicate declaration may be issued
**
**--
**/

sym_name_entry_type
    	*sem_dcl_name(id_frame)

XmConst yystype	    *id_frame;

{
    sym_name_entry_type	    *name_entry;
    char		    * ptr;

    _assert( id_frame->b_tag == sar_k_token_frame, "arg1 not id frame" );

    /* 
    ** The id frame may hold a name or the keyword for a font name, color
    ** name, reason name etc.  If it is one of these special name, then
    ** we insert the special name in the symbol table as a name.  This has
    ** the effect of creating a name entry if one doesn't existing or finding
    ** the name entry if one does.
    */

    if (id_frame->b_type != NAME)
    {
      diag_issue_diagnostic
	( d_override_builtin,
	 _sar_source_position( id_frame ),
	 id_frame->value.az_keyword_entry->at_name);
      
      name_entry = 
	sym_insert_name
	  ( id_frame->value.az_keyword_entry->b_length,
	   id_frame->value.az_keyword_entry->at_name );
    }
    else
	name_entry = (sym_name_entry_type *) id_frame->value.az_symbol_entry;

    /*
    ** If the name entry already has an object linked from it, we have an
    ** duplicate definition of the same name.  Otherwise, everything is fine.
    */

    if (name_entry->az_object == NULL )
	return name_entry;

    if (name_entry->az_object->header.b_tag == sym_k_value_entry) {
	ptr = diag_value_text (
		((sym_value_entry_type *) (name_entry->az_object))->b_type);
    } else if (name_entry->az_object->header.b_tag == sym_k_widget_entry) {
	ptr = diag_object_text (
		((sym_widget_entry_type *) (name_entry->az_object))
			-> header.b_type);
    } else {
	ptr = diag_tag_text( name_entry->az_object->header.b_tag );
    }

    diag_issue_diagnostic
	( d_previous_def,
	  _sar_source_position( id_frame ),
	  name_entry->c_text,
	  ptr );

    return NULL;

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function creates a value entry for a token that represents
**	a value.
**
**  FORMAL PARAMETERS:
**
**      value		pointer to value
**      length		length of the value in bytes
**      value_type	type of value to create 
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
**      a value_entry
**
**  SIDE EFFECTS:
**
**      value entry is created in the symbol table
**
**--
**/

sym_value_entry_type
	*sem_create_value_entry( value, length, value_type )

char	    *value;
int	    length;
int	    value_type;
{
    sym_value_entry_type    *value_entry;

    /*
    **	the value  can be a string, integer, float, boolean, or
    **  a font_name, argument_name, color_name, or reason_name
    */

    /*
    **	the strategy of the function is to determine the contents
    **  of the value entry, then centrally allocate and initialize it.
    **
    **  Allocate the entry and save the source position. Initialize
    **  all fields to either default values or call parameters.
    */
    value_entry = (sym_value_entry_type *)
	sem_allocate_node (sym_k_value_entry, sym_k_value_entry_size);
    _sar_save_source_pos ( &value_entry->header, &yylval );
    sar_assoc_comment ((sym_obj_entry_type *)value_entry);  /* preserve comments */

    value_entry->b_type = value_type;
    value_entry->obj_header.b_flags = (sym_m_private | sym_m_builtin);
    value_entry->w_length = length;
    value_entry->output_state = 0;
    value_entry->b_table_count = 0;
    value_entry->b_aux_flags = 0;
    value_entry->b_arg_type = 0;
    value_entry->b_data_offset = 0;
    value_entry->b_pixel_type = sym_k_unspecified_color;
    value_entry->b_charset = 0;
    value_entry->b_direction = NOSTRING_DIRECTION;
    value_entry->b_max_index = 0;
    value_entry->b_expr_opr = sym_k_unspecified_op;
    value_entry->b_enumeration_value_code = 0;
    value_entry->resource_id = 0;
    value_entry->obj_header.az_name = NULL;
    value_entry->az_charset_value = NULL;
    value_entry->az_next_table_value = NULL;
    value_entry->value.l_integer = 0;
    if ((value_type == sym_k_char_8_value || 
	value_type == sym_k_font_value ||
	value_type == sym_k_fontset_value ||
	value_type == sym_k_keysym_value ||
	value_type == sym_k_xbitmapfile_value ||
	value_type == sym_k_class_rec_name_value ||
	value_type == sym_k_identifier_value) &&
	(length > 0))
	{
	value_entry->value.c_value = (char *) XtCalloc(1,length);
	_move( value_entry->value.c_value, value, length );
	}
    else if (value_type == sym_k_compound_string_value  && (length > 0))
      {
	value_entry->value.xms_value = (XmString) XtCalloc(1,length);
	_move( value_entry->value.xms_value, value, length );
      }
    else
	if ( length > 0 )
	    _move( &(value_entry->value.c_value), value, length );

    /* For enumerations which accept boolean values */
    if (value_type == sym_k_bool_value)
      value_entry->b_enumeration_value_code =
	(*value) ? sym_k_TRUE_enumval : sym_k_FALSE_enumval;
	
    return value_entry;
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function creates the value symbol node for an
**	identifier declaration.
**
**  FORMAL PARAMETERS:
**
**	id_frame	ptr to token frame for the identifier name
**
**  IMPLICIT INPUTS:
**
**	sym_az_current_section_entry	global that points to the "current" section list
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
**      errors may be issued for previously defined name
**
**--
**/

void	sar_create_identifier (id_frame, semi_frame)

XmConst yystype	    *id_frame;
XmConst yystype	    *semi_frame;

{
    sym_name_entry_type	    *name_entry;
    sym_value_entry_type    *value_entry;
    sym_section_entry_type  *section_entry;
    int			    len;
    char		    * ptr;

    /*
    **  Call standard routine to check name entry for id_frame.
    **	This routine handles font name, color names, etc used as ids
    */

    name_entry = sem_dcl_name( id_frame );

    if (name_entry == NULL)
	return;

    /*
    **	Allocate the value entry and fill it in.  The b_type field
    **  in name entries holds the length of the name.  Add one for null.
    */

    len = name_entry->header.b_type + 1;
    ptr = name_entry->c_text;

    value_entry = sem_create_value_entry ( ptr, len, sym_k_identifier_value );

    _move (value_entry->value.c_value, ptr, len);

    value_entry->obj_header.b_flags |= sym_m_private;

    value_entry->obj_header.az_name = name_entry;
    name_entry->az_object = (sym_entry_type *) value_entry;

    /*
    ** save the source file info for this identifier entry
    */
    _sar_save_source_info (&name_entry->header, id_frame, semi_frame );

    /*
    ** allocate a section entry to link the identifier entry into the structure
    */
    section_entry = (sym_section_entry_type *) sem_allocate_node
			(sym_k_section_entry, sym_k_section_entry_size);

    /*
    ** Link this entry off of the current section list
    */
    section_entry->next =
	(sym_entry_type *) sym_az_current_section_entry->entries;
    sym_az_current_section_entry->entries =
	(sym_entry_type *) section_entry;
    section_entry->entries = (sym_entry_type *)name_entry;

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function builds a font table.  This font table
**	is a list of symbol table entries of font items.
**
**  FORMAL PARAMETERS:
**
**      target_frame	    frame holding the font table generated
**	font_frame	    value frame specifying the font value
**	prior_target_frame  frame holding the font table generated so far
**	keyword_frame	    frame holding the font_table keyword
**
**  IMPLICIT INPUTS:
**
**      none
**
**  IMPLICIT OUTPUTS:
**
**      target_frame
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**      font table symbol entry is generated if prior target is null
**
**--
**/

void	sar_make_font_table
	    (target_frame, font_frame, prior_target_frame, keyword_frame)

yystype	    *target_frame;
yystype	    *font_frame;
yystype	    *prior_target_frame;
yystype	    *keyword_frame;

{
    sym_value_entry_type    *font_table_entry;
    sym_value_entry_type    *font_item;

    _assert( font_frame->b_tag == sar_k_value_frame, "font item missing" );

    font_item = (sym_value_entry_type *) font_frame->value.az_symbol_entry;

    /*
    **	If the prior frame is null, this is the first item in the
    **	table.  We need to generate the font table symbol entry.
    */

    switch (prior_target_frame->b_tag)
    {
    case sar_k_null_frame:
	font_table_entry = 
	    sem_create_value_entry ((char*)&font_item, sizeof(long),
				    sym_k_font_table_value);
	font_table_entry->b_table_count = 1;

	/*
	 * If we are dealing with an indirect reference in the table,
	 * make it a valref node so we reference it correctly.
	 */
	if (font_item->obj_header.az_name != NULL)
	  {
	    sym_value_entry_type	*font_save;

	    font_save = font_item;
	    font_item = sem_create_value_entry (0, 0, font_save->b_type);
	    font_item->b_type = font_save->b_type;
	    font_item->obj_header.b_flags = sym_m_private;
	    font_item->b_expr_opr = sym_k_valref_op;
	    font_item->az_exp_op1 = font_save;
	  }
	
	font_table_entry->az_first_table_value = font_item;

	break;

    case sar_k_value_frame:
    {
	int			count;
	sym_value_entry_type	*last_font_item;

	font_table_entry = (sym_value_entry_type *)
	    prior_target_frame->value.az_symbol_entry;
	for (count = 0,
	     last_font_item = font_table_entry->az_first_table_value;  

	     last_font_item->az_next_table_value != NULL;  

	     last_font_item =
	         last_font_item->az_next_table_value)
	    count++;

	if (count >= sym_k_max_list_count)
	   diag_issue_diagnostic
		(d_too_many,
		 _sar_source_position( font_frame ),
		 diag_value_text( sym_k_font_value ),
		 diag_value_text( sym_k_font_table_value ),
		 sym_k_max_list_count );
	else
	  {
	    /*
	    * If we are dealing with an indirect reference in the table,
	    * make it a valref node so we reference it correctly.
	    */
	    if (font_item->obj_header.az_name != NULL)
	      {
		sym_value_entry_type	*font_save;

		font_save = font_item;
		font_item = sem_create_value_entry (0, 0, font_save->b_type);
		font_item->b_type = font_save->b_type;
		font_item->obj_header.b_flags = sym_m_private;
		font_item->b_expr_opr = sym_k_valref_op;
		font_item->az_exp_op1 = font_save;
	      }

	    last_font_item->az_next_table_value = font_item;
	    font_table_entry->b_table_count = count + 1;
	}
	break;
    }

    default:
	_assert( FALSE, "prior frame in error" );
    }

    /*
    **	font item needs to be marked as a table entry
    */
    font_item->b_aux_flags |= sym_m_table_entry;
    font_item->az_next_table_value = NULL;

    /*
    ** initialize the target frame
    */
    _sar_move_source_info (target_frame, keyword_frame);
    target_frame->b_tag = sar_k_value_frame;
    target_frame->b_type = sym_k_font_table_value;
    target_frame->b_flags = sym_m_private;
    target_frame->value.az_symbol_entry =
	(sym_entry_type *) font_table_entry;

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function builds a item for a font table.  This font table
**	is a list of symbol table entries of font items.
**
**  FORMAL PARAMETERS:
**
**      target_frame	    frame holding the font item generated
**	charset_frame	    token or null frame holding charset token
**	font_frame	    value frame specifying the font value
**
**  IMPLICIT INPUTS:
**
**      none
**
**  IMPLICIT OUTPUTS:
**
**      target_frame
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**      copy the font value unless it is unnamed
**
**--
**/

void	sar_make_font_item(target_frame, charset_frame, font_frame)

yystype	    *target_frame;
yystype	    *charset_frame;
yystype	    *font_frame;

{
    sym_value_entry_type    *font_value_entry;
    int			    item_type;

    _assert( font_frame->b_tag == sar_k_value_frame, "font exp is missing" );

    font_value_entry =
		(sym_value_entry_type *) font_frame->value.az_symbol_entry;

    if ((font_frame->b_flags & sym_m_forward_ref) != 0)
	    {
	    diag_issue_diagnostic
		(d_undefined,
		_sar_source_position(font_frame),
		"font entry",
		font_value_entry->obj_header.az_name->c_text);
	    }

    item_type = font_value_entry->b_type;

    switch (item_type)
    {
    case sym_k_font_value:
    case sym_k_fontset_value:
    {
	int	charset;
	
	charset = font_value_entry->b_charset;

	/*
	**  If the attr_frame is not null, it must be a value frame with contains
	**  a pointer to the value entry for the userdefined charset, or a token frame
	**  which contains the charset token subclass.
	*/
	switch (charset_frame->b_tag)
	{
	case sar_k_value_frame:
	    {
	    sym_value_entry_type	* az_value_entry;

	    az_value_entry = (sym_value_entry_type *)charset_frame->value.az_symbol_entry;
	    font_value_entry->b_charset = az_value_entry->b_charset;
	    font_value_entry->az_charset_value =
		az_value_entry->az_charset_value;
	    break;
	    }

	case sar_k_token_frame:
	    {
	    key_keytable_entry_type	*keyword_entry;

	    keyword_entry = (key_keytable_entry_type *)
				charset_frame->value.az_keyword_entry;
	    font_value_entry->b_charset = 
		sem_map_subclass_to_charset( keyword_entry->b_subclass );
	    break;
	    }
	}

	break;
    }

    case sym_k_error_value:
	break;

    default:
	diag_issue_diagnostic
	    ( d_wrong_type,
	      _sar_source_position( font_frame ),
	      diag_value_text( item_type ),
	      diag_value_text( sym_k_font_value ) );

	item_type = sym_k_error_value;
	font_value_entry = (sym_value_entry_type *) sym_az_error_value_entry;
    }

    /*
    ** initialize the target frame
    */

    _sar_move_source_info( target_frame, font_frame );
    target_frame->b_tag = sar_k_value_frame;
    target_frame->b_type = item_type;
    target_frame->b_flags = sym_m_private;
    target_frame->value.az_symbol_entry =
		(sym_entry_type *) font_value_entry;

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function builds a font value.  
**
**  FORMAL PARAMETERS:
**
**      target_frame	    frame holding the font item generated
**	charset_frame	    token or null frame holding charset token
**	value_frame	    value frame specifying the font value
**	keyword_frame	    frame to use as locator for result
**
**  IMPLICIT INPUTS:
**
**      none
**
**  IMPLICIT OUTPUTS:
**
**      target_frame
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**      create a font symbol table entry
**
**--
**/

void	sar_make_font(target_frame, charset_frame, value_frame, keyword_frame)

yystype	    *target_frame;
yystype	    *charset_frame;
yystype	    *value_frame;
yystype	    *keyword_frame;

{
    sym_value_entry_type    *font_value_entry;
    sym_value_entry_type    *value_entry;

    _assert( value_frame->b_tag == sar_k_value_frame, "font name is missing" );

	font_value_entry = sem_create_value_entry ("", 0, sym_k_font_value );
	font_value_entry->b_type = sym_k_font_value;
	font_value_entry->obj_header.b_flags = sym_m_private;
	font_value_entry->b_expr_opr = sym_k_coerce_op;

        if ((value_frame->b_flags & sym_m_forward_ref) != 0)
	    sym_make_value_forward_ref (value_frame,
	        (char*)&(font_value_entry->az_exp_op1), 
		sym_k_patch_add);
        else
	    {
	    value_entry =
	        (sym_value_entry_type *) value_frame->value.az_symbol_entry;
	    font_value_entry->az_exp_op1 = value_entry;
	    }
	

	/*
	**  If the attr_frame is not null, it must be a value frame with contains
	**  a pointer to the value entry for the userdefined charset, or a token frame
	**  which contains the charset token subclass.
	*/
	switch (charset_frame->b_tag)
	{
	case sar_k_value_frame:
	    {
	    sym_value_entry_type	* az_value_entry;

	    az_value_entry = (sym_value_entry_type *)
		charset_frame->value.az_symbol_entry;

	    font_value_entry->b_charset = az_value_entry->b_charset;
/* BEGIN HAL Fix CR 5266 */
	    font_value_entry->az_charset_value = az_value_entry;
/* END HAL Fix CR 5266 */
	    break;
	    }

	case sar_k_token_frame:
	    {
	    key_keytable_entry_type	*keyword_entry;

	    keyword_entry = (key_keytable_entry_type *)
		charset_frame->value.az_keyword_entry;
	    font_value_entry->b_charset = 
		sem_map_subclass_to_charset( keyword_entry->b_subclass );
	    break;
	    }

	default:
/* BEGIN OSF Fix CR 5443 */
	    font_value_entry->b_charset = Uil_lex_l_user_default_charset;
/* END OSF Fix CR 5443 */
	    break;
	}

    /*
    ** initialize the target frame
    */
    _sar_move_source_info( target_frame, keyword_frame );
    target_frame->b_tag = sar_k_value_frame;
    target_frame->b_type = sym_k_font_value;
    target_frame->b_flags = sym_m_private;
    target_frame->value.az_symbol_entry =
	(sym_entry_type *) font_value_entry;

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function builds a fontset value.  
**
**  FORMAL PARAMETERS:
**
**      target_frame	    frame holding the font item generated
**	charset_frame	    token or null frame holding charset token
**	value_frame	    value frame specifying the font value
**	keyword_frame	    frame to use as locator for result
**
**  IMPLICIT INPUTS:
**
**      none
**
**  IMPLICIT OUTPUTS:
**
**      target_frame
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**      create a font symbol table entry
**
**--
**/

void	sar_make_fontset(target_frame, charset_frame, value_frame, keyword_frame)

yystype	    *target_frame;
yystype	    *charset_frame;
yystype	    *value_frame;
yystype	    *keyword_frame;

{
    sym_value_entry_type    *font_value_entry;
    sym_value_entry_type    *value_entry;

    _assert( value_frame->b_tag == sar_k_value_frame, "font name is missing" );

	font_value_entry = sem_create_value_entry ("", 0, sym_k_fontset_value );
	font_value_entry->b_type = sym_k_fontset_value;
	font_value_entry->obj_header.b_flags = sym_m_private;
	font_value_entry->b_expr_opr = sym_k_coerce_op;

        if ((value_frame->b_flags & sym_m_forward_ref) != 0)
	    sym_make_value_forward_ref (value_frame,
	        (char*)&(font_value_entry->az_exp_op1), 
		sym_k_patch_add);
        else
	    {
	    value_entry =
	        (sym_value_entry_type *) value_frame->value.az_symbol_entry;
	    font_value_entry->az_exp_op1 = value_entry;
	    }
	

	/*
	**  If the attr_frame is not null, it must be a value frame with contains
	**  a pointer to the value entry for the userdefined charset, or a token frame
	**  which contains the charset token subclass.
	*/
	switch (charset_frame->b_tag)
	{
	case sar_k_value_frame:
	    {
	    sym_value_entry_type	* az_value_entry;

	    az_value_entry = (sym_value_entry_type *)
		charset_frame->value.az_symbol_entry;

	    font_value_entry->b_charset = az_value_entry->b_charset;
/* BEGIN HAL Fix CR 5266 */
	    font_value_entry->az_charset_value = az_value_entry;
/* END HAL Fix CR 5266 */
	    break;
	    }

	case sar_k_token_frame:
	    {
	    key_keytable_entry_type	*keyword_entry;

	    keyword_entry = (key_keytable_entry_type *)
		charset_frame->value.az_keyword_entry;
	    font_value_entry->b_charset = 
		sem_map_subclass_to_charset( keyword_entry->b_subclass );
	    break;
	    }

	default:
/* BEGIN OSF Fix CR 5443 */
	    font_value_entry->b_charset = Uil_lex_l_user_default_charset;
/* END OSF Fix CR 5443 */
	    break;
	}

    /*
    ** initialize the target frame
    */
    _sar_move_source_info( target_frame, keyword_frame );
    target_frame->b_tag = sar_k_value_frame;
    target_frame->b_type = sym_k_fontset_value;
    target_frame->b_flags = sym_m_private;
    target_frame->value.az_symbol_entry =
	(sym_entry_type *) font_value_entry;
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function builds a color item which is a temporary respository
**	to hold data for a single color to be placed in a color table.  The
**	color item is deleted when the color table is built by 
**	sar_make_color_table.
**
**  FORMAL PARAMETERS:
**
**      target_frame	    frame holding the color item generated
**	color_frame	    token or value frame giving the color
**	letter_frame	    value frame specifying the letter to use for color
**	keyword_frame	    frame to use as locator for result
**
**  IMPLICIT INPUTS:
**
**      none
**
**  IMPLICIT OUTPUTS:
**
**      target_frame
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**      create a color item symbol table entry
**
**--
**/

void	sar_make_color_item
	    (target_frame, color_frame, letter_frame )

yystype	    *target_frame;
yystype	    *color_frame;
yystype	    *letter_frame;

{
    sym_color_item_entry_type   *item_entry;
    sym_value_entry_type	*letter_entry;

    _assert( letter_frame->b_tag == sar_k_value_frame, "letter frame missing" );

    /*
    ** initialize the target frame
    */
    _sar_move_source_info( target_frame, color_frame );
    target_frame->b_tag = sar_k_value_frame;
    target_frame->b_type = 0;
    target_frame->b_flags = 0;
    target_frame->value.az_symbol_entry = 0;

    /*
    **	The color can be either a color value or the keyword background or
    **	foreground.  Want to step up:
    **	    target_frame->b_tag:    with either sar_k_value_frame for no error
    **				    or sar_k_null_frame for an error
    **	    color_entry:	    color value if there is one
    **				    0 for background
    **				    1 for foreground
    */

    /*
    **	Allocate the color item and fill it in
    */

    item_entry = (sym_color_item_entry_type *)
	sem_allocate_node(sym_k_color_item_entry,
			  sym_k_color_item_entry_size );
    _sar_save_source_pos (&item_entry->header, color_frame);

    item_entry->b_index = 0;
    item_entry->az_next = NULL;

    switch (color_frame->b_tag)
    {
    case sar_k_token_frame:
    {
	key_keytable_entry_type	*keyword_entry;

	/*
	**  This is the foreground or background case
	*/

	keyword_entry = color_frame->value.az_keyword_entry;

	switch (keyword_entry->b_token)
	{
	case BACKGROUND:
	    item_entry->az_color = (sym_value_entry_type *) URMColorTableBG;
	    break;

	case FOREGROUND:
	    item_entry->az_color = (sym_value_entry_type *) URMColorTableFG;
	    break;

	default:
	    _assert( FALSE, "missing keyword frame" );
	}

	break;
    }

    case sar_k_value_frame:

        if ((color_frame->b_flags & sym_m_forward_ref) != 0)
	    {
	    sym_value_entry_type	*diag_value;

	    diag_value = (sym_value_entry_type *)
		color_frame->value.az_symbol_entry;
	    diag_issue_diagnostic
		(d_undefined,
		_sar_source_position(color_frame),
		"color entry",
		diag_value->obj_header.az_name->c_text);
	    }
        else
	    {
	    item_entry->az_color =
	        (sym_value_entry_type *) color_frame->value.az_symbol_entry;
	    }
	break;

    default:
	_assert( FALSE, "color frame missing" );
    }

    /*
    **	Letter frame has already been checked in the grammar to be a character
    **	string.  Need to further check that it has a length of 1.
    */

    letter_entry =
	(sym_value_entry_type *) letter_frame->value.az_symbol_entry;

    if (letter_entry->w_length != 1)
    {
	diag_issue_diagnostic
	    ( d_single_letter,
	      _sar_source_position( letter_frame ) );

        target_frame->b_tag = sar_k_null_frame;
	return;
    }

    item_entry->b_letter = letter_entry->value.c_value[0];

    /*
    **	If the tag is in error - return
    */

    if (target_frame->b_tag == sar_k_null_frame)
	return;

    /*
    ** Save source information
    */

    _sar_save_source_info ( &item_entry->header, color_frame, letter_frame);

    target_frame->value.az_symbol_entry = (sym_entry_type *) item_entry;

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function builds a list of the color items that are used to
**	produce the color table.  Each color is checked to see that
**	its letter is unique.  The new color item is placed on the
**	front of the list.
**
**  FORMAL PARAMETERS:
**
**      target_frame	    frame holding the color item list
**	item_frame	    color item to be added to list
**	prior_target_frame  existing color list
**
**  IMPLICIT INPUTS:
**
**      none
**
**  IMPLICIT OUTPUTS:
**
**      target_frame
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**      none
**
**--
**/

void	sar_append_color_item(target_frame, item_frame, prior_target_frame)

yystype	    *target_frame;
yystype	    *item_frame;
yystype	    *prior_target_frame;

{
    sym_color_item_entry_type   *item_entry;
    sym_color_item_entry_type   *prior_item_entry;

    /*
    **	Tag for the prior_target frame indicates if this is the first
    **	or a subsequent color item on the list.
    */

    switch (prior_target_frame->b_tag)
    {
    case sar_k_null_frame:
	prior_item_entry = NULL;
	break;

    case sar_k_value_frame:
	prior_item_entry =
	    (sym_color_item_entry_type *)
		prior_target_frame->value.az_symbol_entry;
	break;

    default:
	_assert( FALSE, "prior frame missing" );
    }

    /*
    ** initialize the target frame
    */

    _sar_move_source_info( target_frame, item_frame );
    target_frame->b_tag = sar_k_value_frame;
    target_frame->b_type = 0;
    target_frame->b_flags = 0;
    target_frame->value.az_symbol_entry =
		(sym_entry_type *) prior_item_entry;

    /*
    **	Need to verify that the letter associated with the current color
    **	item is unique. 
    */

    switch (item_frame->b_tag)
    {
    case sar_k_value_frame:
    {
	sym_color_item_entry_type   *next_item_entry;

	item_entry =
	    (sym_color_item_entry_type *) item_frame->value.az_symbol_entry;

	for (next_item_entry = prior_item_entry;  
	     next_item_entry != NULL;  
	     next_item_entry = next_item_entry->az_next)
	    if (next_item_entry->b_letter == item_entry->b_letter)
	    {
	    	diag_issue_diagnostic
		    ( d_dup_letter,
		      _sar_source_position( item_frame ) );
	    
		return;
	    }

	item_entry->az_next = prior_item_entry;

	target_frame->value.az_symbol_entry =
		(sym_entry_type *) item_entry;

	return;
    }

    case sar_k_null_frame:
	return;

    default:
	_assert( FALSE, "list frame missing" );
    }

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function builds a color table.  It takes a series of
**	color items and repackages them within the color table.
**
**  FORMAL PARAMETERS:
**
**      target_frame	    frame for holding the color table
**	list_frame	    frame holding first of color item lists
**	keyword_frame	    frame for COLOR_TABLE keyword
**
**  IMPLICIT INPUTS:
**
**      none
**
**  IMPLICIT OUTPUTS:
**
**      target_frame
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**      none
**
**--
**/

void	sar_make_color_table(target_frame, list_frame, keyword_frame)

yystype	    *target_frame;
yystype	    *list_frame;
yystype	    *keyword_frame;

{
    sym_color_item_entry_type   *next_item_entry;
    sym_value_entry_type	*color_table_entry;
    int				target_type;

    /*
    **	Tag for the list frame indicates if there are any color items.
    */

    switch (list_frame->b_tag)
    {
    case sar_k_null_frame:
	target_type = sym_k_error_value;
	color_table_entry = sym_az_error_value_entry;
	break;

    case sar_k_value_frame:
    {
	int	count;
	int	index;

	count = 0;
	index = 1;

	for (next_item_entry =
	     (sym_color_item_entry_type *) list_frame->value.az_symbol_entry;  
	     next_item_entry != NULL;  
	     next_item_entry = next_item_entry->az_next)
	{
	    count++;
	    switch ((long)next_item_entry->az_color)
	    {
	    case 0:
		next_item_entry->b_index = URMColorTableBG;
		break;
	    case 1:
		next_item_entry->b_index = URMColorTableFG;
		break;
	    default:
		next_item_entry->b_index = (unsigned char) ++index;
		break;
	    }
	}
	
	if (index >= sym_k_max_color_count)
	{
	   diag_issue_diagnostic
		(d_too_many,
		 _sar_source_position( keyword_frame ),
		 diag_value_text( sym_k_color_value ),
		 diag_value_text( sym_k_color_table_value ),
		 sym_k_max_color_count );
	    target_type = sym_k_error_value;
	    color_table_entry = sym_az_error_value_entry;
	    break;
	}

	color_table_entry = (sym_value_entry_type *)
	    sem_allocate_node (sym_k_value_entry, sym_k_value_entry_size);
	color_table_entry->value.z_color = (sym_color_element *)
	    XtCalloc(1,sizeof(sym_color_element)*count);
	color_table_entry->b_type = sym_k_color_table_value;
	color_table_entry->b_table_count = (unsigned char) count;
	color_table_entry->b_max_index = (unsigned char) index;
	color_table_entry->obj_header.b_flags = sym_m_private;

	_sar_save_source_pos ( &color_table_entry->header, list_frame);

	for (index = 0,
	     next_item_entry = (sym_color_item_entry_type *)
	         list_frame->value.az_symbol_entry;  
	     next_item_entry != NULL;  
	     index++ )
	    {
	    sym_color_item_entry_type	*temp;
	    
	    color_table_entry->value.z_color[index].b_index =
		next_item_entry->b_index;	    
	    color_table_entry->value.z_color[index].b_letter =
		next_item_entry->b_letter;	    
	    color_table_entry->value.z_color[index].az_color =
		next_item_entry->az_color;	    
	    
	    temp = next_item_entry;
	    next_item_entry = next_item_entry->az_next;
	    sem_free_node(( sym_entry_type *) temp );
	    }

	target_type = sym_k_color_table_value;

	break;

    }

    default:
	_assert( FALSE, "list frame missing" );
    }

    /*
    ** initialize the target frame
    */

    _sar_move_source_info( target_frame, keyword_frame );
    target_frame->b_tag = sar_k_value_frame;
    target_frame->b_type = target_type;
    target_frame->b_flags = sym_m_private;
    target_frame->value.az_symbol_entry =
		(sym_entry_type *) color_table_entry;

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function builds a color value.  Input to the function is a
**	character string give the color name and an option keyword specifying
**	display of the color on a monochrome device.
**
**  FORMAL PARAMETERS:
**
**      target_frame	frame to hold the created color value
**	color_frame	frame giving the character string specifying the color
**	mono_frame	frame describing monochrome attribute
**	keyword_frame	frame pointing to COLOR keyword
**
**  IMPLICIT INPUTS:
**
**      none
**
**  IMPLICIT OUTPUTS:
**
**      target_frame
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**      icon value is created
**
**--
**/

void	sar_make_color(target_frame, color_frame, mono_frame, keyword_frame)

yystype	    *target_frame;
yystype	    *color_frame;
yystype	    *mono_frame;
yystype	    *keyword_frame;

{
    sym_value_entry_type    *color_entry;
    sym_value_entry_type    *value_entry;
    int			    state;

    /*
    **	Mono frame can point to the keyword FOREGROUND or BACKGROUND
    **	or be null meaning unspecified.
    */

    switch (mono_frame->b_tag)
    {
    case sar_k_null_frame:
	state = sym_k_unspecified_color;
	break;

    case sar_k_token_frame:

	switch (mono_frame->value.az_keyword_entry->b_token)
	{
	case BACKGROUND:
	    state = sym_k_background_color;
	    break;

	case FOREGROUND:
	    state = sym_k_foreground_color;
	    break;

	default:
	    _assert( FALSE, "monochrome info missing" );
	}
	break;

    default:
	_assert( FALSE, "monochrome info missing" );
    }

    /* 
    ** Transform the char 8 value entry into a color value.
    ** Before doing this, we need to insure that the string
    ** is not in error or named.
    */

    _assert( color_frame->b_tag == sar_k_value_frame, "value missing" );

	color_entry = 
	    sem_create_value_entry 
		( "",
		  0,
		  sym_k_color_value );
	color_entry->b_type = sym_k_color_value;
	color_entry->obj_header.b_flags = sym_m_private;
	color_entry->b_arg_type = state;
	color_entry->b_expr_opr = sym_k_coerce_op;

        if ((color_frame->b_flags & sym_m_forward_ref) != 0)
	    sym_make_value_forward_ref (color_frame,
	        (char*)&(color_entry->az_exp_op1), 
		sym_k_patch_add);
        else
	    {
	    value_entry =
	        (sym_value_entry_type *) color_frame->value.az_symbol_entry;
	    color_entry->az_exp_op1 = value_entry;
	    }

    /*
    ** initialize the target frame
    */

    _sar_move_source_info( target_frame, keyword_frame );
    target_frame->b_tag = sar_k_value_frame;
    target_frame->b_type = sym_k_color_value;
    target_frame->b_flags = sym_m_private;
    target_frame->value.az_symbol_entry =
		(sym_entry_type *) color_entry;

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function builds a icon value.  Input to the function is a
**	color table and a list of string that represent the rows of the
**	icon.
**
**  FORMAL PARAMETERS:
**
**      target_frame	frame to hold the created icon value
**	table_frame	frame describing the color table
**	list_frame	frame pointing to the list of row strings
**	keyword_frame	frame pointing to the icon keyword
**
**  IMPLICIT INPUTS:
**
**      none
**
**  IMPLICIT OUTPUTS:
**
**      target_frame
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**      icon value is created
**
**--
**/

void	sar_make_icon(target_frame, list_frame, table_frame, keyword_frame)

yystype	    *target_frame;
yystype	    *table_frame;
yystype	    *list_frame;
yystype	    *keyword_frame;

{
    sym_value_entry_type    *table_entry;
    sym_value_entry_type    *icon_entry;
    sym_value_entry_type    *head_row_entry;
    int			    target_type;
    int			    width;
    int			    height;

    /*
    **	Table_frame specifies the color table for the icon.  If this
    **	argument is null, use the standard color table.
    */

    target_type = sym_k_icon_value;
    height = 0;
    width = 0;

    switch (table_frame->b_tag)
    {
    case sar_k_null_frame:
	table_entry = standard_color_table();
	break;

    case sar_k_value_frame:
	table_entry =
		(sym_value_entry_type *) table_frame->value.az_symbol_entry;

        /*
        **  Check that the table is indeed a color table
        **  Forward references are set up once the icon entry is created
        */

	if ((table_frame->b_flags & sym_m_forward_ref) != 0)
	    table_entry = NULL;
	else
	    switch (table_entry->b_type)
		{
		case sym_k_color_table_value:
		    break;

		case sym_k_error_value:
		default:
/* BEGIN OSF Fix CR 5421 */
		    /*
		     * If the value_frame is not a color_table_value, print
		     * a diagnostic to flag it as an error further down the
		     * road.  Otherwise, if it is exported, will cause problems.
		     */
                    diag_issue_diagnostic
                        ( d_wrong_type,
                          _sar_source_position(table_frame),  
                          diag_value_text(table_entry->b_type), 
                          diag_value_text(sym_k_color_table_value) );
/* END OSF Fix CR 5421 */

		    target_type = sym_k_error_value;
		    table_entry = standard_color_table();
		    break;
		}
	break;

    default:
	_assert( FALSE, "color table missing" );
    }

    /*
    **	Now start checking the rows of the table.  If the tag on list
    **	frame is null, there are no rows (this is due to prior errors).
    **	Rows are linked in reverse order.  We reorder them at this
    **	point since it simplifies .uid generation.
    */

    switch (list_frame->b_tag)
    {
    case sar_k_null_frame:
	target_type = sym_k_error_value;
	break;

    case sar_k_value_frame:
    {
	sym_value_entry_type	*row_entry;
	sym_value_entry_type	*temp_row_entry;


	/*
	**  Reorder the rows
	*/

	for (row_entry =
		(sym_value_entry_type *) list_frame->value.az_symbol_entry,
	     head_row_entry = NULL;

	     row_entry != NULL;  
	    
	     temp_row_entry = row_entry,
	     row_entry = row_entry->az_next_table_value,
	     temp_row_entry->az_next_table_value =
	         head_row_entry,
	     head_row_entry = temp_row_entry)
		;

	for (row_entry = head_row_entry,
	     width = row_entry->w_length;

	     row_entry != NULL;  
	    
	     row_entry = row_entry->az_next_table_value)
	{
/* BEGIN OSF Fix CR 5420 */
	  /*
	   * Check to make sure each row_entry is a character string.
	   * If it isn't, print a diagnostic message indicating an error.
	   */
            if ((row_entry->b_type != sym_k_char_8_value) &&
		(row_entry->b_type != sym_k_localized_string_value))
            {
                diag_issue_diagnostic
                    ( d_wrong_type,
                      _sar_source_pos2(row_entry),
                      diag_value_text(row_entry->b_type),
                      diag_value_text(sym_k_char_8_value) );
                target_type = sym_k_error_value;
            }
/* END OSF Fix CR 5420 */

	    height++;

	    if (width != row_entry->w_length)
	    {
		diag_issue_diagnostic
		    ( d_icon_width,
		      row_entry->header.az_src_rec,	/* line info */
		      row_entry->header.b_type,		/* column infor */
		      height );
		target_type = sym_k_error_value;
	    }

	}
	break;
    }
	
    default:
	_assert( FALSE, "row list missing" );
    }

    if (width > sym_k_max_list_count)
    {
	diag_issue_diagnostic
	    ( d_too_many,
	      _sar_source_position( keyword_frame ),
	      "column", diag_value_text( sym_k_icon_value ),
	       sym_k_max_list_count );

	target_type = sym_k_error_value;
    }
   
    if (height > sym_k_max_list_count)
    {
	diag_issue_diagnostic
	    ( d_too_many,
	      _sar_source_position( keyword_frame ),
	      "row", diag_value_text( sym_k_icon_value ),
	       sym_k_max_list_count );

	target_type = sym_k_error_value;
    }
   

    /*
    **	If we have no errors, allocate the icon
    */

    if (target_type == sym_k_error_value)
	icon_entry = sym_az_error_value_entry;
    else
    {
	icon_entry = (sym_value_entry_type *)
	    sem_allocate_node (sym_k_value_entry, sym_k_value_entry_size);
	icon_entry->value.z_icon = (sym_icon_element *)
	    XtCalloc(1,sizeof(sym_icon_element));
	icon_entry->b_type = sym_k_icon_value;
	icon_entry->value.z_icon->w_height = height;
	icon_entry->value.z_icon->w_width = width;
	icon_entry->value.z_icon->az_color_table = table_entry;
	icon_entry->value.z_icon->az_rows = head_row_entry;
	icon_entry->obj_header.b_flags = sym_m_private;

	_sar_save_source_pos (&icon_entry->header, list_frame );

	if ((table_frame->b_flags & sym_m_forward_ref) != 0)
	    sym_make_value_forward_ref
		(table_frame,
		 (char*)&icon_entry->value.z_icon->az_color_table,
		 sym_k_patch_add);
    }

    /*
    ** initialize the target frame
    */

    _sar_move_source_info( target_frame, keyword_frame );
    target_frame->b_tag = sar_k_value_frame;
    target_frame->b_type = target_type;
    target_frame->b_flags = sym_m_private;
    target_frame->value.az_symbol_entry =
		(sym_entry_type *) icon_entry;

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function defines the standard color table.
**
**  FORMAL PARAMETERS:
**
**      none
**
**  IMPLICIT INPUTS:
**
**      color_table	local static storage
**
**  IMPLICIT OUTPUTS:
**
**      none
**
**  FUNCTION VALUE:
**
**      address of standard color table entry
**
**  SIDE EFFECTS:
**
**      may allocate the standard color table
**
**--
**/

static sym_value_entry_type	*standard_color_table()
{
    static  sym_value_entry_type    *color_table = NULL;

    if (color_table == NULL)
    {
	color_table = (sym_value_entry_type *)
	    sem_allocate_node (sym_k_value_entry,
			       sym_k_value_entry_size);
	color_table->value.z_color = (sym_color_element *)
	    XtCalloc(1,sizeof(sym_color_element)*2);

	color_table->b_type = sym_k_color_table_value;
	color_table->b_table_count = 2;
	color_table->b_max_index = 1;
	color_table->obj_header.b_flags = sym_m_private;

	color_table->header.az_src_rec = src_az_module_source_record;

	color_table->value.z_color[0].b_index = URMColorTableBG;
	color_table->value.z_color[0].b_letter = ' ';
	color_table->value.z_color[0].az_color =
	    (sym_value_entry_type *) URMColorTableBG;
	    
	color_table->value.z_color[1].b_index = URMColorTableFG;
	color_table->value.z_color[1].b_letter = '*';
	color_table->value.z_color[1].az_color =
	    (sym_value_entry_type *) URMColorTableFG;

    }

    return color_table;
}
