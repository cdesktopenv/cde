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
static char rcsid[] = "$TOG: UilSarMod.c /main/13 1997/03/12 15:21:36 dbl $"
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
**      This module contain the routines for processing the module construct.
**
**--
**/


/*
**
**  INCLUDE FILES
**
**/

#include "UilDefI.h"

/*
**
**  TABLE OF CONTENTS
**
*/

/*
**
**  DEFINE and MACRO DEFINITIONS
**
**/



/*
**
**  EXTERNAL VARIABLE DECLARATIONS
**
**/

extern   yystype			yylval;


/*
**
**  GLOBAL VARIABLE DECLARATIONS
**
**/

externaldef(uil_comp_glbl) src_source_record_type
	*src_az_module_source_record;
externaldef(uil_comp_glbl) unsigned short int	
	*uil_urm_variant = NULL;
externaldef(uil_comp_glbl) unsigned short int
	*uil_arg_compr = NULL;
externaldef(uil_comp_glbl) unsigned short int
	*uil_reas_compr = NULL;
externaldef(uil_comp_glbl) unsigned short int
	*uil_widget_compr = NULL;
externaldef(uil_comp_glbl) unsigned short int
	*uil_child_compr = NULL;

/*
**
**  OWN VARIABLE DECLARATIONS
**
**/

unsigned int	module_clauses;

#define m_version_clause	(1<<0)
#define m_names_clause		(1<<1)
#define m_charset_clause	(1<<2)
#define m_objects_clause	(1<<3)

#define m_after_names		(m_charset_clause)


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This function initializes all static data structure for the semantic
**	action routines.
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
**      uil_urm_variant
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**      global variables are initialized 
**
**--
**/

void	sar_initialize ()

{
int		i;	/* loop index */

/* BEGIN OSF Fix CR 5443 */
/* Initialize uil_sym_default_charset based on XmFALLBACK_CHARSET */
if (strcmp(XmFALLBACK_CHARSET, "ISO8859-1") != 0) /* Most common case. */
  for (i = 0; i < (int)charset_lang_table_max; i++)
    if (strcmp(XmFALLBACK_CHARSET, charset_lang_names_table[i]) == 0)
      {
	uil_sym_default_charset = charset_lang_codes_table[i];
	break;
      }
/* END OSF Fix CR 5443 */

/*
 * Allocate vectors for the variant and usage vectors if they are NULL,
 */
if ( uil_urm_variant == NULL )
    uil_urm_variant = (unsigned short int *)
	XtMalloc(sizeof(unsigned short int)*(uil_max_object+1));
if ( uil_arg_compr == NULL )
    uil_arg_compr = (unsigned short int *)
	XtMalloc(sizeof(unsigned short int)*(uil_max_arg+1));
if ( uil_reas_compr == NULL )
    uil_reas_compr = (unsigned short int *)
	XtMalloc(sizeof(unsigned short int)*(uil_max_reason+1));
if ( uil_widget_compr == NULL )
    uil_widget_compr = (unsigned short int *)
	XtMalloc(sizeof(unsigned short int)*(uil_max_object+1));
if ( uil_child_compr == NULL )
    uil_child_compr = (unsigned short int *)
	XtMalloc(sizeof(unsigned short int)*(uil_max_child+1));
/*
 **	Loop throught the array of object variants and set them all to NULL.
 */
for (i = 0; i<uil_max_object+1; i++)
    uil_urm_variant[i] = 0;   

/*
 ** Initialize all compression vectors
 */
for ( i=0 ; i<uil_max_arg+1 ; i++ )
    uil_arg_compr[i] = 0;
for ( i=0 ; i<uil_max_reason+1 ; i++ )
    uil_reas_compr[i] = 0;
for ( i=0 ; i<uil_max_object+1 ; i++ )
    uil_widget_compr[i] = 0;
for ( i=0 ; i<uil_max_child+1 ; i++ )
    uil_child_compr[i] = 0;

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function creates the root entry node for the compilation.
**	The "root" entry is the root node of the symbol tree and is
**	what's passed back to the caller of the compiler.
**
**  FORMAL PARAMETERS:
**
**      root_frame	ptr to root frame that will remain on the stack
**			throughout the compilation
**
**  IMPLICIT INPUTS:
**
**      none
**
**  IMPLICIT OUTPUTS:
**
**	sym_az_root_entry		global pointer used thoughout the compilation
**	sym_az_curent_section_entry	global pointer used thoughout the compilation
**	src_az_first_source_record	global pointer to the source record list
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**	none
**
**--
**/

void	sar_create_root (root_frame)

yystype	    *root_frame;

{
    /*
    ** Allocate the symbol nodes
    */

    sym_az_root_entry = (sym_root_entry_type *)
		sem_allocate_node (sym_k_root_entry, sym_k_root_entry_size);

    /*
    ** Create a place holder entry (tail) and change the "current" section list to it.
    */

    sym_az_current_section_entry = (sym_section_entry_type *) sem_allocate_node
			    ( sym_k_section_entry, sym_k_section_entry_size );

    sym_az_current_section_entry->header.b_type = sym_k_section_tail;

    sym_az_root_entry->sections = sym_az_current_section_entry;
    sym_az_root_entry->src_record_list = src_az_first_source_record;

    /*
    ** Save the file name and the expanded version of it.
    */

    strcpy (sym_az_root_entry->file_name, Uil_cmd_z_command.ac_source_file);
    strcpy (sym_az_root_entry->full_file_name, src_az_source_file_table[0]->expanded_name);

    /*
    ** Save the symbol node in the root frame.
    */

    root_frame->b_tag = sar_k_root_frame;
    root_frame->b_type = sym_k_root_entry;
    root_frame->value.az_symbol_entry = (sym_entry_type *)sym_az_root_entry;

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function create the module entry symbol node for the compilation.
**
**  FORMAL PARAMETERS:
**
**      target_frame	ptr to module frame that will remain on the stack
**			throughout the compilation
**	id_frame	ptr to token frame for the module name
**
**  IMPLICIT INPUTS:
**
**	sym_az_root_entry	global that points to the root entry
**
**  IMPLICIT OUTPUTS:
**
**      sym_az_module_entry	global that points to module entry
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**      set second line of the listing title
**
**--
**/

void	sar_create_module(target_frame, id_frame, module_frame)

yystype	    *target_frame;
yystype	    *id_frame;
yystype	    *module_frame;

{
    sym_name_entry_type	    *name_entry;

    /*
    **  Call standard routine to check name entry for id_frame.
    **	This routine handles font name, color names, etc used as ids
    */

    name_entry = (sym_name_entry_type *) sem_dcl_name( id_frame );

    /*
    **	Allocate the module entry and fill it in
    */

    sym_az_module_entry = (sym_module_entry_type *)
	sem_allocate_node (sym_k_module_entry, sym_k_module_entry_size);
    sym_az_module_entry->obj_header.az_name = name_entry;
    _sar_save_source_pos (&sym_az_module_entry->header, module_frame);

    /* preserve module header comments */
    sar_assoc_comment ((sym_obj_entry_type *)sym_az_module_entry);  

    /*
    ** Hang the module entry off the root entry
    */

    sym_az_root_entry->module_hdr = sym_az_module_entry;

    /*
    **	Have name entry point to the module entry too.  This stops the
    **	name from being reused to name another construct.
    */

    name_entry->az_object = (sym_entry_type *) sym_az_module_entry;

    /*
    ** Save the source information about module name identifier
    */

    _sar_save_source_info ( &name_entry->header , module_frame , id_frame );

    /*
    **	Set up target frame
    */

    target_frame->b_tag = sar_k_module_frame;

    /*
    **	Set up listing title
    */

    if (Uil_cmd_z_command.v_listing_file)
	sprintf(Uil_lst_c_title2, 
		"Module: %s", 
		name_entry->c_text );

    /*
    **	Set mask to no clauses seen
    */

    module_clauses = 0;
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function sets a version string for the module.
**
**  FORMAL PARAMETERS:
**
**      value_frame	ptr to value frame for version string
**
**  IMPLICIT INPUTS:
**
**	none
**
**  IMPLICIT OUTPUTS:
**
**      sym_az_module_entry	global that point to module entry
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**      set up second line of the listing title
**
**--
**/

void	sar_process_module_version (value_frame, start_frame)

yystype	    *value_frame;
yystype	    *start_frame;

{
    sym_value_entry_type    *value_entry;

    _assert( value_frame->b_tag == sar_k_value_frame, "value frame missing" );

    if ((module_clauses & m_version_clause) != 0)
	diag_issue_diagnostic
	    ( d_single_occur,
	      _sar_source_position( value_frame ),
	      "UIL", "module", "version", "clause"
	    );

    /*
    ** FORWARD REFERENCING OF VERSION NO LONGER ALLOWED
    ** If it's not a forward reference, verify its length and stick a pointer
    ** to the value node of the version in the module entry.
    */

    if ((value_frame->b_flags & sym_m_forward_ref) != 0)
      diag_issue_diagnostic(d_illegal_forward_ref,
			    _sar_source_position(value_frame),
			    "Module Version"
	    );

    else
	{
	value_entry = (sym_value_entry_type *) 
	    value_frame->value.az_symbol_entry;

	if (value_entry->w_length > 31)
	{
	    diag_issue_diagnostic
		( d_out_range,
		  _sar_source_position( value_frame ),
		  "version string",
		  "0..31 characters"
		);

    	    value_entry->w_length = 31;
	}

    sym_az_module_entry->az_version = value_entry;
	}

    /*
    ** Save source info
    */

    _sar_save_source_info ( &sym_az_module_entry->az_version->header , start_frame , value_frame);

    /*
    **	Set up listing title
    */

    if (Uil_cmd_z_command.v_listing_file)
	sprintf(Uil_lst_c_title2, 
		"Module: %s \t Version: %s", 
		sym_az_module_entry->obj_header.az_name->c_text,
		value_entry->value.c_value );

    module_clauses |= m_version_clause;
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function sets the case sensitivity of names for the module.
**
**  FORMAL PARAMETERS:
**
**      token_frame	ptr to token frame for keyword sensitive or insensitive
**
**  IMPLICIT INPUTS:
**
**	sym_az_module_entry	global which points to module entry
**
**  IMPLICIT OUTPUTS:
**
**      uil_v_case_sensitive	global which control case sensitivity of names
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

void	sar_process_module_sensitivity (token_frame, start_frame)

yystype	    *token_frame;
yystype	    *start_frame;

{
    _assert( token_frame->b_tag == sar_k_token_frame, "token frame missing" );

    if ((module_clauses & m_names_clause) != 0)
	diag_issue_diagnostic
	    ( d_single_occur,
	      _sar_source_position( token_frame ),
	      "UIL", "module", "names", "clause"
	    );

    if ((module_clauses & m_after_names) != 0)
	diag_issue_diagnostic
	    ( d_names,
	      _sar_source_position( token_frame )
	    );

    uil_v_case_sensitive = (token_frame->b_type == CASE_SENSITIVE);

    sym_az_module_entry->az_case_sense = (sym_value_entry_type *)
					 sem_allocate_node (sym_k_value_entry, sym_k_value_entry_size);

    sym_az_module_entry->az_case_sense->header.b_type = uil_v_case_sensitive;

    /*
    ** Save source info
    */

    _sar_save_source_info ( &sym_az_module_entry->az_case_sense->header , start_frame , token_frame);

    /* let the keyword table know of the sensitivity change */

    key_initialize();


    /*
    **	The default for name sensitivity is SENSITIVE.
    **	At the point that this semantic routine is called, the only valid
    **  name seen so far is the module name.
    */

    module_clauses |= m_names_clause;
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function sets the default charset for the module.
**
**  FORMAL PARAMETERS:
**
**      token_frame	ptr to token frame for charset
**
**  IMPLICIT INPUTS:
**
**	sym_az_module_entry	global pointer to the module entry
**
**  IMPLICIT OUTPUTS:
**
**      Uil_lex_l_user_default_charset
**	Uil_lex_az_charset_entry
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

void	sar_process_module_charset(token_frame , start_frame)

yystype	    *token_frame;
yystype	    *start_frame;

{
    sym_value_entry_type	*value_entry;

    _assert( (token_frame->b_tag == sar_k_token_frame) ||
	     (token_frame->b_tag == sar_k_value_frame), "token or value frame missing" );

    if ((module_clauses & m_charset_clause) != 0)
	diag_issue_diagnostic
	    ( d_single_occur,
	      _sar_source_position( token_frame ),
	      "UIL", "module", "character_set", "clause"
	    );

    /*
    **  There are two different ways that the charset info may be specified.
    **  If the charset_frame is a token frame, then we can just grab the
    **  token class and map it into a charset value.  If it is a value frame
    **  the it is the result of the CHARACTER_SET function and is a string
    **  value representing the character set.
    */
    switch (token_frame->b_tag)
    {
	case sar_k_token_frame:
	    {
	    key_keytable_entry_type	*az_keyword_entry;
	    az_keyword_entry = token_frame->value.az_keyword_entry;

	    Uil_lex_l_user_default_charset = az_keyword_entry->b_subclass;

	    value_entry = sem_create_value_entry (
				 token_frame->value.az_keyword_entry->at_name,
				 token_frame->value.az_keyword_entry->b_length, sym_k_char_8_value );
	    break;
	    }
     
	case sar_k_value_frame:
	    {
	    Uil_lex_l_user_default_charset = lex_k_userdefined_charset;
	    Uil_lex_az_charset_entry = (sym_value_entry_type *)token_frame->value.az_symbol_entry;
	    value_entry = (sym_value_entry_type *)token_frame->value.az_symbol_entry;
	    break;
	    }
    }

    /* If charset specified for module, then localized strings not allowed */
    Uil_lex_l_localized = FALSE;
    
    module_clauses |= m_charset_clause;
    sym_az_module_entry->az_character_set = value_entry;

    /*
    ** Save source info
    */

    _sar_save_source_info ( &value_entry->header , start_frame , token_frame);

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function saves the source record for the module header
**	for possible later use in the machine code listing.
**
**  FORMAL PARAMETERS:
**
**      none
**
**  IMPLICIT INPUTS:
**
**      yylval		Current token information from the lexical analyzer
**
**  IMPLICIT OUTPUTS:
**
**	src_az_module_source_record	source record for the module header
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

void	sar_save_module_source ()

{

    src_az_module_source_record = yylval.az_source_record;

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine creates and saves a default object specification
**	for the module entry itself.
**
**  FORMAL PARAMETERS:
**
**      object_frame	ptr to token frame for "OBJECT" 
**
**  IMPLICIT INPUTS:
**
**      none
**
**  IMPLICIT OUTPUTS:
**
**	sym_az_module_entry	global pointer to the module entry
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

void	sar_make_def_obj (object_frame)

yystype	    *object_frame;

{
sym_def_obj_entry_type	*def_obj_entry;

/*
 * Make def_obj entry and link into the chain headed in the module
 */
def_obj_entry = (sym_def_obj_entry_type *) sem_allocate_node
    (sym_k_def_obj_entry, sym_k_def_obj_entry_size);
_sar_save_source_pos (&def_obj_entry->header, object_frame);
def_obj_entry->next = sym_az_module_entry->az_def_obj;
sym_az_module_entry->az_def_obj = def_obj_entry;

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function sets the default variants for objects which
**	are defined in the module.
**
**  FORMAL PARAMETERS:
**
**      type_frame	ptr to token frame for object type
**      variant_frame	ptr to token frame for variant
**
**  IMPLICIT INPUTS:
**
**      uil_gadget_variants	table to see if the gadget variant is supported
**      uil_urm_variant		table to see if the object type has been
**				specified previously
**	sym_az_module_entry	global pointing to the module entry
**
**  IMPLICIT OUTPUTS:
**
**      uil_urm_variant		table to contain the default variant for this
**				object type.
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

void	sar_process_module_variant  (obj_type_frame, variant_frame)

yystype	    *obj_type_frame;
yystype	    *variant_frame;

{

    unsigned int		obj_type, obj_variant;
    yystype			*source_frame;
    sym_def_obj_entry_type	*def_obj_entry;

    source_frame = & yylval;

    obj_type = obj_type_frame->value.az_keyword_entry->b_subclass;
    obj_variant = variant_frame->b_type;

/*  See if this object type has been specified before.  */

    if ( uil_urm_variant[obj_type] != 0 )
	{
	diag_issue_diagnostic
	    (d_supersede,
	     _sar_source_position ( source_frame ),
	     diag_object_text (obj_type),
	     diag_tag_text (obj_variant),
	     diag_tag_text (sym_k_module_entry),
	     "" );
	}

/*  See if this object type supports gadgets.  */

    if ( obj_variant == sym_k_gadget_entry )
	{
	if ( uil_gadget_variants[obj_type] == 0 )
	    {
	    diag_issue_diagnostic
		(d_gadget_not_sup,
		 _sar_source_position ( source_frame ),
		 diag_object_text (obj_type),
		 diag_object_text (obj_type) );
	    obj_variant = sym_k_widget_entry;
	    }
	}

/*  Save the default variant information  */
    uil_urm_variant[obj_type] = obj_variant;

/*
** get the latest def_obj entry and fill in
*/
    def_obj_entry = sym_az_module_entry->az_def_obj->next;
    def_obj_entry->b_object_info = obj_type;
    def_obj_entry->b_variant_info = obj_variant;

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This procedure saves source info for the various sections declaration
**	lists in the Uil file (i.e. value, identifier, procedure, object, and
**	list).
**
**  FORMAL PARAMETERS:
**
**      header_frame	ptr to token frame for the section declaration
**	section_type	integer describing what section this is
**
**  IMPLICIT INPUTS:
**
**      none
**
**  IMPLICIT OUTPUTS:
**
**	sym_az_root_entry	global pointer to the root entry
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**	none
**
**--
*/

void	sar_save_section_source (header_frame, section_type)

yystype	    *header_frame;
int	    section_type;

{
    sym_section_entry_type	*section_entry;

    section_entry = (sym_section_entry_type *) sem_allocate_node
			( sym_k_section_entry, sym_k_section_entry_size );

    section_entry->header.b_type = section_type;

    /*
    ** Save source info
    */

    _sar_save_source_info ( &section_entry->header , header_frame, header_frame);

    /*
    ** Link this section into the current section list.
    */

    section_entry->next = (sym_entry_type *) sym_az_current_section_entry;
    section_entry->prev_section = sym_az_current_section_entry->prev_section;
    sym_az_current_section_entry = section_entry;

}
