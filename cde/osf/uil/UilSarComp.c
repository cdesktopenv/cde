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
static char rcsid[] = "$XConsortium: UilSarComp.c /main/11 1995/07/14 09:36:46 drk $"
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
**      This module supports compound strings in UIL.  It includes
**	the basic operations for creating, copying, and concatenating
**      strings.
**
**--
**/


/*
**
**  INCLUDE FILES
**
**/



#include <Mrm/MrmAppl.h>



#ifdef DXM_V11
#include <DXm/DXmHelpB.h>
#endif

#include "UilDefI.h"

/*
**
**  TABLE OF CONTENTS
**
**/


/*
**
**  DEFINE and MACRO DEFINITIONS
**
**/

#define clear_class_mask (~(sym_m_private | sym_m_imported | sym_m_exported))

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
**      This function creates a null compound string.
**
**  FORMAL PARAMETERS:
**
**      none
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
**      a copy of the primitive string may be made
**
**--
**/

sym_value_entry_type 	*sem_create_cstr()

{
    sym_value_entry_type    *az_cstr_entry;

    az_cstr_entry = (sym_value_entry_type *)
		sem_allocate_node
			( sym_k_value_entry,
                          sym_k_value_entry_size + sizeof( char * ) );

    az_cstr_entry->obj_header.b_flags = sym_m_builtin | sym_m_private;
    az_cstr_entry->b_type = sym_k_compound_string_value;
    az_cstr_entry->w_length = sizeof (char *);
    az_cstr_entry->b_direction = NOSTRING_DIRECTION;

    /* Fix for  CN 16149 (DTS 10023) part 1 -- initialize charset info */
    az_cstr_entry->b_charset = sym_k_error_charset;
    az_cstr_entry->az_charset_value = NULL;

    _sar_save_source_pos (&az_cstr_entry->header, &yylval);

    return az_cstr_entry;

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function creates a null wide_character string.
**
**  FORMAL PARAMETERS:
**
**      none
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
**      a copy of the primitive string may be made
**
**--
**/

sym_value_entry_type 	*sem_create_wchar_str()

{
    sym_value_entry_type    *az_wchar_str_entry;

    az_wchar_str_entry = (sym_value_entry_type *)
		sem_allocate_node
			( sym_k_value_entry,
                          sym_k_value_entry_size + sizeof( char * ) );

    az_wchar_str_entry->obj_header.b_flags = sym_m_builtin | sym_m_private;
    az_wchar_str_entry->b_type = sym_k_wchar_string_value;
    az_wchar_str_entry->w_length = sizeof (char *);
    az_wchar_str_entry->b_direction = NOSTRING_DIRECTION;

    _sar_save_source_pos (&az_wchar_str_entry->header, &yylval);

    return az_wchar_str_entry;

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function appends a primitive or a localized string to a
**	compound string.
**
**  FORMAL PARAMETERS:
**
**      az_cstr_entry	    compound string
**      az_str_entry	    primitive string or localized string
**	op2_temporary	    true if op2 is not needed after operation
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
**      a copy of the primitive string may be made
**
**--
**/

void	sem_append_str_to_cstr(az_cstr_entry, az_str_entry, op2_temporary)

sym_value_entry_type	*az_cstr_entry;
sym_value_entry_type	*az_str_entry;
boolean			op2_temporary;

{
    sym_value_entry_type	*last_str_entry;
    sym_value_entry_type	**ptr;
    sym_value_entry_type	*new_str_entry;
    boolean			merge;

    /*
    **	A couple of points:
    **	1) if op2_temporary = FALSE - we must make a copy
    **	   of it 
    **	2) if the last string of the compound string has the same attributes
    **     as the string being appended, the strings are merged into 1
    */

    /* find the last string of the compound string */

    ptr = &(az_cstr_entry->az_first_table_value);
    last_str_entry = *ptr;
    merge = FALSE;

    if (last_str_entry != NULL)
    {
	for (  ;
	     last_str_entry->az_next_table_value != NULL;  

	     ptr = (sym_value_entry_type * *) 
	         &(last_str_entry->az_next_table_value),
	     last_str_entry = *ptr);

	if ((last_str_entry->b_charset == az_str_entry->b_charset)
	    &&
	    ((last_str_entry->b_direction)== (az_str_entry->b_direction))
	    &&
	    ((last_str_entry->b_aux_flags & sym_m_separate) == 0 ))
	    merge = TRUE;
	else
	    ptr = (sym_value_entry_type * *)
		&(last_str_entry->az_next_table_value);
    }

    if (merge)
    {
	new_str_entry = 
	    sem_cat_str_to_str( last_str_entry, TRUE, 
				az_str_entry, op2_temporary );
    }
    else
    {
	/*
	**  Append a new segment to the compound string
	*/

	if( op2_temporary == FALSE )
	{
	    unsigned short	old_size;

	    /* must make a copy since user has access to string via name */
	    
	    new_str_entry = (sym_value_entry_type *) 
		sem_allocate_node( sym_k_value_entry,
				   az_str_entry->header.w_node_size<<2 );

	    old_size = new_str_entry->header.w_node_size;

	    _sym_copy_entry( new_str_entry, 
		   az_str_entry,
		   az_str_entry->header.w_node_size );

	    new_str_entry->header.w_node_size = old_size;

	}
	else
	    new_str_entry = az_str_entry;

    }

    /* link to the end of the compound string */

    new_str_entry->b_aux_flags |= sym_m_table_entry;
    new_str_entry->obj_header.b_flags = sym_m_private | sym_m_builtin;
    new_str_entry->obj_header.az_name = NULL;
    new_str_entry->az_next_table_value = NULL;
    *ptr = new_str_entry;
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function appends a compound string to a compound string.
**
**  FORMAL PARAMETERS:
**
**      az_cstr1_entry	    compound string
**      az_cstr2_entry	    compound string
**	op2_temporary	    true if op2 is not needed after operation
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
**      none
**
**--
**/

void	sem_append_cstr_to_cstr(az_cstr1_entry, az_cstr2_entry, op2_temporary)

sym_value_entry_type	*az_cstr1_entry;
sym_value_entry_type	*az_cstr2_entry;
boolean			op2_temporary;

{
    sym_value_entry_type	*last_str_entry;
    sym_value_entry_type	**ptr;
    sym_value_entry_type	*next_str_entry;

    /*
    **	A couple of points:
    **	1) if op2_temporary = FALSE - we must make a copy of 2nd compound
    **	   string
    **	2) if the last string of 1st compound string has the same attributes
    **     as the 1st string of the 2nd compound string being appended, 
    **	   the strings are merged into 1
    */

    /* find the last string of the 1st compound string */

    ptr = &(az_cstr1_entry->az_first_table_value);
    last_str_entry = *ptr;

    next_str_entry = az_cstr2_entry->az_first_table_value;

    if (op2_temporary)
	sem_free_node(( sym_entry_type *) az_cstr2_entry );

    if (next_str_entry == NULL)
	return;

    if (last_str_entry != NULL)
    {
	for (  ;
	     last_str_entry->az_next_table_value != NULL;  

	     ptr = (sym_value_entry_type * *)
	         &(last_str_entry->az_next_table_value),
	     last_str_entry = *ptr);

	if ((last_str_entry->b_charset == next_str_entry->b_charset)
	    &&
	    ((last_str_entry->b_direction)== (next_str_entry->b_direction))
    	    &&
	    ((last_str_entry->b_aux_flags & sym_m_separate) == 0 ))
	{
	    last_str_entry = 
		sem_cat_str_to_str( last_str_entry, TRUE,
				    next_str_entry, op2_temporary );
	    last_str_entry->b_aux_flags |= sym_m_table_entry;
	    *ptr = last_str_entry;
	    next_str_entry =
		(sym_value_entry_type *) next_str_entry->az_next_table_value;
	}

	ptr = (sym_value_entry_type * *)
	    &(last_str_entry->az_next_table_value);
    }

    if (op2_temporary)
    {
	*ptr = next_str_entry;
	return;
    }

    for ( ; 
	 next_str_entry != NULL;
	 next_str_entry = (sym_value_entry_type *)
	     next_str_entry->az_next_table_value )
    {
	sym_value_entry_type	*new_str_entry;
	unsigned short		old_size;

	new_str_entry = (sym_value_entry_type *) 
	    sem_allocate_node( sym_k_value_entry,
			       next_str_entry->header.w_node_size<<2 );

	old_size = new_str_entry->header.w_node_size;

	_sym_copy_entry( new_str_entry, 
	       next_str_entry,
	       next_str_entry->header.w_node_size );
	
	new_str_entry->header.w_node_size = old_size;
	new_str_entry->obj_header.b_flags = sym_m_private | sym_m_builtin;
	new_str_entry->obj_header.az_name = NULL;
	new_str_entry->b_aux_flags |= sym_m_table_entry;

	/* link to the end of the compound string */

	*ptr = new_str_entry;
	ptr = (sym_value_entry_type * *)
	    &(new_str_entry->az_next_table_value);
    }

    *ptr = NULL;

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function concatenates 2 primitive strings.
**
**  FORMAL PARAMETERS:
**
**      az_str1_entry	    primitive string
**	op1_temporary	    op1 is a temporary string
**      az_str2_entry	    primitive string
**	op2_temporary	    op2 is a temporary string
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
**      ptr to the result string
**
**  SIDE EFFECTS:
**
**      none
**
**--
**/

sym_value_entry_type
	*sem_cat_str_to_str
	    (az_str1_entry, op1_temporary, az_str2_entry, op2_temporary)

sym_value_entry_type	*az_str1_entry;
boolean			op1_temporary;
sym_value_entry_type	*az_str2_entry;
boolean			op2_temporary;

{
    sym_value_entry_type    *new_str_entry;
    int			    l1, l2;

    /*
    **	Can only append two simple strings if they have the same direction and
    **	the first does not have the separate attribute.
    */
    _assert(((az_str1_entry->b_charset == az_str2_entry->b_charset)
	     &&
	     ((az_str1_entry->b_direction) == (az_str2_entry->b_direction))
	    ),
	    "strings with different attrs concatenated" );

    l1 = az_str1_entry->w_length;
    l2 = az_str2_entry->w_length;

    /* extra 1 is for terminating null */

    new_str_entry = (sym_value_entry_type *) 
	sem_allocate_node( sym_k_value_entry,
			   sym_k_value_entry_size );
    new_str_entry->value.c_value = XtCalloc(1, l1 + l2 + 1);


    new_str_entry->obj_header.b_flags = sym_m_builtin | sym_m_private;

    _sar_move_source_info_2 (&new_str_entry->header, &az_str1_entry->header);
    new_str_entry->b_charset = az_str1_entry->b_charset;
    new_str_entry->b_direction = az_str1_entry->b_direction;
    new_str_entry->b_aux_flags =
	(az_str1_entry->b_aux_flags & sym_m_separate);
    new_str_entry->b_type = sym_k_char_8_value;
    new_str_entry->w_length = l1 + l2;

    _move( new_str_entry->value.c_value, 
	   az_str1_entry->value.c_value, l1 );

    _move( &new_str_entry->value.c_value[ l1 ],
	   az_str2_entry->value.c_value,
	   l2+1 );

    /* 
    **	if either of the operands is unnamed - free the node 
    */

    if (op1_temporary)
	{
	_free_memory( az_str1_entry->value.c_value );
	sem_free_node(( sym_entry_type *) az_str1_entry );
	}

    if (op2_temporary)
	{
	_free_memory( az_str2_entry->value.c_value );
	sem_free_node(( sym_entry_type *) az_str2_entry );
	}

    return new_str_entry;

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function takes the charset sets understood by the parser
**	and maps them to charsets understood by the symbol table and
**	toolkit.
**
**  FORMAL PARAMETERS:
**
**      charset_as_subclass	sym_k_..._charset literal naming charset
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
**      sym_k...charset name for charset
**
**  SIDE EFFECTS:
**
**      none
**
**--
**/

int	sem_map_subclass_to_charset(charset_as_subclass)

int	charset_as_subclass;

{
    switch (charset_as_subclass)
    {

    case lex_k_fontlist_default_tag:
    case sym_k_fontlist_default_tag:
      return sym_k_fontlist_default_tag;
    case lex_k_default_charset:
	return uil_sym_default_charset;
    case lex_k_userdefined_charset:
	return sym_k_userdefined_charset;
    default:
	_assert (charset_as_subclass!=0, "charset code==0");
	_assert (charset_as_subclass<=uil_max_charset, "unknown charset");
	return charset_as_subclass;
    }
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function verifies the character set indicated by the user.
**
**  FORMAL PARAMETERS:
**
**      current_frame       current stack frame
**      charset_frame       stack frame of CHARSET token
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
**      none
**
**--
**/

void    sar_charset_verify ( charset_frame )

yystype           *charset_frame;

{
    key_keytable_entry_type	*az_keyword_entry;

    _assert(((charset_frame->b_tag == sar_k_token_frame) ||
	     (charset_frame->b_tag == sar_k_value_frame)),
	    "token or value frame missing" );

    az_keyword_entry = charset_frame->value.az_keyword_entry;



    /*
    **	Store the current charset so it can be used by LEX to processes a
    **	string literal (if the CHARSET_NAME is used to prefix a string literal)
    */
    Uil_lex_l_literal_charset = az_keyword_entry->b_subclass;
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function converts a random NAME into a CHARSET_NAME 
**	with the default charset.
**
**  FORMAL PARAMETERS:
**
**      name_frame       current stack frame containing NAME
**
**  IMPLICIT INPUTS:
**
**      none
**
**  IMPLICIT OUTPUTS:
**
**      modified name_frame
**
**  FUNCTION VALUE:
**
**      none
**
**  SIDE EFFECTS:
**
**      name_frame converted from NAME to CHARSET_NAME
**
**--
**/

void sar_make_fallback_charset(name_frame)
     yystype           *name_frame;
{
  sym_name_entry_type			*symbol_entry;
  static key_keytable_entry_type	*az_keyword_entry = NULL;

  _assert(((name_frame->b_tag == sar_k_token_frame) ||
	   (name_frame->b_tag == sar_k_value_frame)),
	  "token or value frame missing" );

  /* Get symbol and check if already used as charset. */
  symbol_entry = (sym_name_entry_type *)name_frame->value.az_symbol_entry;
  
  if ((symbol_entry->b_flags & sym_m_charset) == 0)
    {
      symbol_entry->b_flags |= sym_m_charset;
      diag_issue_diagnostic(d_default_charset,
			    _sar_source_pos2(symbol_entry),
			    symbol_entry->c_text,
			    DEFAULT_TAG);
    }
  
  /* Get the default charset keyword entry. */
  if (az_keyword_entry == NULL)
    az_keyword_entry = key_find_keyword(strlen(DEFAULT_TAG), DEFAULT_TAG);
  
  _assert((az_keyword_entry !=NULL), "default charset keyword missing");

  /* Change NAME to CHARSET_NAME */
  name_frame->value.az_keyword_entry = az_keyword_entry;
  name_frame ->b_type = az_keyword_entry->b_token;

  /*
   **	Store the current charset so it can be used by LEX to processes a
   **	string literal (if the NAME is used to prefix a string literal)
   */
    Uil_lex_l_literal_charset = az_keyword_entry->b_subclass;
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function checks the attributes of CHARACTER_SET function.
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

void	sar_chk_charset_attr( target_frame, value_frame, prior_value_frame )

yystype	    *target_frame;
yystype	    *value_frame;
yystype	    *prior_value_frame;
{

    /*
    **	Set up not specified values in the target frame.
    **	    b_type will hold the sixteen_bit property
    */

    switch (prior_value_frame->b_tag)
    {
    case sar_k_null_frame:
	/*
	** no prior values
	*/

	target_frame->b_tag = sar_k_token_frame;
	target_frame->b_direction = NOSTRING_DIRECTION;
	target_frame->b_charset = uil_sym_default_charset;
	target_frame->b_type = 0;
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
	break;
    default:
	_assert( FALSE, "prior value frame missing from stack" );
    }

    /*
    **	Case on the keyword for the attribute given
    */

    switch (value_frame->b_type)
    {
    case RIGHT_TO_LEFT:
    {
	sym_value_entry_type	*value_entry;

	value_entry =
		(sym_value_entry_type *) value_frame->value.az_symbol_entry;

	/*
	**  If the value is a boolean, then just set the corresponding mask 
	**  accordingly.
	*/
	if (value_entry->b_type == sym_k_bool_value)
	    if (value_entry->value.l_integer == TRUE)
		target_frame->b_direction = XmSTRING_DIRECTION_R_TO_L;
	    else
		target_frame->b_direction = XmSTRING_DIRECTION_L_TO_R;

	break;
    }

    case SIXTEEN_BIT:
    {
	sym_value_entry_type	*value_entry;

	value_entry =
		(sym_value_entry_type *) value_frame->value.az_symbol_entry;

	/*
	**  If the value is a boolean, then just set the corresponding mask 
	**  accordingly.
	*/
	if (value_entry->b_type == sym_k_bool_value)
	    if (value_entry->value.l_integer == TRUE)
		target_frame->b_type |= sym_m_sixteen_bit; 
	    else
		target_frame->b_type &= ~sym_m_sixteen_bit; 

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
**      This function makes a CHARACTER_SET and sets the properties
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

void	sar_make_charset (target_frame, value_frame, attr_frame, keyword_frame)

yystype	    *target_frame;
yystype	    *value_frame;
yystype	    *attr_frame;
yystype	    *keyword_frame;
{
    sym_value_entry_type    *value_entry;

    _assert( value_frame->b_tag == sar_k_value_frame,
	     "value frame missing from stack" );


    /*
    **  Set the character set information into the symbol table
    **  entry for the char_8 string that indentifies the name of this
    **  userdefined character set.
    */
    value_entry =
	(sym_value_entry_type *) value_frame->value.az_symbol_entry;
    value_entry->b_charset = sym_k_userdefined_charset;
 
/* BEGIN HaL fix CR 5547 */
    sem_evaluate_value (value_entry); 
    if (value_entry->b_type != sym_k_char_8_value)
        diag_issue_diagnostic
            (d_wrong_type,
             _sar_source_pos2( value_entry ),
             diag_value_text( value_entry->b_type ),
             "null-terminated string");
/* END HaL fix CR 5547 */
  
 
    /*
    **  If the attr_frame is not null, it must be a value frame with contains
    **  a pointer to the attributes frame for this userdefined charset.
    */
    if (attr_frame->b_tag == sar_k_token_frame)
	{
	/*
	**  Set the attributes of the string, as specified by the options
	**  to the CHARACTER_SET function, without disturbing any
	**  existing bits.
	*/
	value_entry->b_direction = attr_frame->b_direction;
	value_entry->b_aux_flags = (attr_frame->b_type & sym_m_sixteen_bit);
	}


    /*
    ** initialize the target frame
    */

    _sar_move_source_info( target_frame, keyword_frame );

    target_frame->b_tag = sar_k_value_frame;
    target_frame->b_type = sym_k_char_8_value;
    target_frame->b_flags = sym_m_private;
    target_frame->value.az_symbol_entry = value_frame->value.az_symbol_entry;


    /*
    **  Store the current charset so it can be used by LEX to processes a
    **  string literal (if the CHARACTER_SET function is used to prefix a
    **  string literal)
    */
    Uil_lex_l_literal_charset = lex_k_userdefined_charset;
    Uil_lex_az_literal_charset = value_entry;
}


