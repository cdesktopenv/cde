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
static char rcsid[] = "$XConsortium: UilSarObj.c /main/14 1995/07/14 09:37:30 drk $"
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
**      This module contains the semantic action routines for
**	object definitions in the UIL.
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
**  DEFINE and MACRO DEFINITIONS
**
**/

/*
**  This is a fast algorithm for mapping an integer (_size) to
**	    0	_size <= 8
**	    1	8 < _size <= 16
**	    2	16 < _size <= 32
**	    3	32 < _size <= 64
**	    4	64 < _size 
**  The algorithm is based on the notion that the floating pt representation
**  of an integer has an exponent that is the log_base2( int ).
**
**  This algorithm is specific to the VAX.  An alternate layout of the
**  internal represention of a floating pt number could be supplied for
**  other architectures.
*/

#define _compute_node_index( _size, _index ) \
    {   unsigned short j, k; \
        j = (_size); \
        if (j <= 8) k = 0; \
        else if (j <= 16) k = 1; \
        else if (j <= 32) k = 2; \
        else if (j <= 64) k = 3; \
        else k = 4; \
        (_index) = k; \
    }
	  
	

/*
**
**  EXTERNAL VARIABLE DECLARATIONS
**
**/

extern yystype			gz_yynullval;
extern yystype			yylval;


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
**      This routine associates the latest comment block
**	with the current object frame.    RAP
**
**  FORMAL PARAMETERS:
**
**      object_frame		address of the parse stack frame for this
**				object.
**
**	
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
**      the object frame contains the comment for this object.
**
**--
**/

void		sar_assoc_comment( object )

sym_obj_entry_type    *object;
{

  object->obj_header.az_comment = (char *)_get_memory(strlen(comment_text)+1);
  strcpy(object->obj_header.az_comment, comment_text);
  comment_text[0] = '\0';

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This routine creates the symbol node for the object, and
**	saves it in the object frame on the parse stack.
**
**  FORMAL PARAMETERS:
**
**      object_frame		address of the parse stack frame for this
**				object.
**
**	object_type		type literal for this object.
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
**      the object frame contains the symbol node for this object.
**
**--
**/

void		sar_create_object
  	( yystype *object_frame, unsigned char object_type )

{

    sym_name_entry_type		* name_entry;
    sym_obj_entry_type		* obj_entry;
    int				node_size;
    yystype			* source_frame;

    source_frame = & yylval;

    if (object_frame->b_tag != sar_k_null_frame)
	{

	/* 
	 ** First we check on the name to see if it has been previously used.
	 ** This function returns NULL if name cannot be used.
	 */
	name_entry = (sym_name_entry_type *) sem_dcl_name (object_frame);
	}
    else
	{
	name_entry = NULL;
	}

/*    Determine the size of the symbol node to allocate.    */

    switch (object_type)
	{
    	case sym_k_gadget_entry:
    	case sym_k_widget_entry:
    	    node_size = sym_k_widget_entry_size;
    	    break;

    	case sym_k_list_entry:
    	    node_size = sym_k_list_entry_size;
    	    break;

    	default:
    	    _assert (FALSE, "unexpected object type");
    	    break;
	}

/*
 * Allocate the symbol node, connect it to its name, and save source info
 */
    obj_entry = (sym_obj_entry_type *)
	sem_allocate_node (object_type, node_size);
    if (name_entry != NULL)
	{
	name_entry->az_object = (sym_entry_type *)obj_entry;
	obj_entry->obj_header.az_name = (sym_name_entry_type *)name_entry;
	}
    _sar_save_source_pos (&obj_entry->header, source_frame );
    sar_assoc_comment(obj_entry);  /* preserve comments */
/*
 * Set the definition in progress bit.
 */
    obj_entry->obj_header.b_flags |= sym_m_def_in_progress;

/*
 * Save the symbol node in the object frame.
 */
    object_frame->b_tag = sar_k_object_frame;
    object_frame->b_type = object_type;
    object_frame->value.az_symbol_entry = (sym_entry_type *)obj_entry;

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This routine creates the symbol node for the child, and
**	saves it in the object frame on the parse stack.
**
**  FORMAL PARAMETERS:
**
**      object_frame		address of the parse stack frame for this
**				object.
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
**      the object frame contains the symbol node for this child.
**
**--
**/

void		sar_create_child
  	( yystype *object_frame )

{
    sym_obj_entry_type		* obj_entry;
    yystype			* source_frame;

    source_frame = & yylval;

/*
 * Allocate the symbol node, set its type, and save source info
 */
    obj_entry = (sym_obj_entry_type *)
	sem_allocate_node (sym_k_child_entry, sym_k_widget_entry_size);
    obj_entry->header.b_type = 
      object_frame->value.az_keyword_entry->b_subclass;

    _sar_save_source_pos (&obj_entry->header, source_frame );
    sar_assoc_comment(obj_entry);  /* preserve comments */
/*
 * Indicate in compress table that this child type is used.
 */
    uil_child_compr[obj_entry->header.b_type] = 1;
    
/*
 * Set the definition in progress bit.
 */
    obj_entry->obj_header.b_flags |= sym_m_def_in_progress;

/*
 * Save the symbol node in the object frame.
 */
    object_frame->b_tag = sar_k_object_frame;
    object_frame->b_type = sym_k_child_entry;
    object_frame->value.az_symbol_entry = (sym_entry_type *)obj_entry;

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine creates and links a section node into the symbol table
**
**  FORMAL PARAMETERS:
**
**	id_frame	the token frame with the id for this entry.
**
**  IMPLICIT INPUTS:
**
**	sym_az_current_section_entry	global pointer to the "current" section
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
**--
**/

void	sar_link_section ( id_frame )

yystype			* id_frame;

{
sym_section_entry_type	* section_entry;

/*
 * Allocate a section entry. Link this entry of of the current section list
 */
section_entry = (sym_section_entry_type *) sem_allocate_node
    (sym_k_section_entry, sym_k_section_entry_size);
section_entry->next = (sym_entry_type *)
    sym_az_current_section_entry->entries;
sym_az_current_section_entry->entries = (sym_entry_type *) section_entry;
section_entry->entries = id_frame->value.az_symbol_entry;

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine saves the source information about where this
**	semi-colon entry ends.
**
**  FORMAL PARAMETERS:
**
**	semi_frame	the token frame for the terminating semi-colon
**			for this entry.
**
**  IMPLICIT INPUTS:
**
**	sym_az_current_section_entry	global pointer to the
**		"current" section list
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
**	none
**
**--
**/

void	sar_save_src_semicolon_pos (semi_frame)

yystype			* semi_frame;

{
sym_section_entry_type	* section_entry;

section_entry = (sym_section_entry_type *)
    sym_az_current_section_entry->entries;
_sar_save_source_pos (&section_entry->entries->header, semi_frame);

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This function saves the title end source for lists (i.e. "ARGUMENTS",
**	"CALLBACKS", "PROCEDURES", and "CONTROLS").  The source saved here
**	should be "}" or posibly an id_ref.
**
**  PARAMETERS:
**
**	close_frame	ptr to token frame for the closing source
**
**  IMPLICIT INPUTS:
**
**	the "current" list on the frame stack as returned by sem_find_object
**
**  IMPLICIT OUTPUTS:
**
**	none
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**	none
**--
**/

void	sar_save_list_end (close_frame)

yystype	    *close_frame;

{
    sym_list_entry_type		* list_entry;
    yystype			* list_frame;

    /*
    ** Search the syntax stack for the object frame.
    */

    list_frame = sem_find_object (close_frame - 1);
    list_entry = (sym_list_entry_type *) list_frame->value.az_symbol_entry;

    _sar_save_source_pos ( &list_entry->header , close_frame );

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This function saves the end source for just about any type
**	of entry.
**
**  PARAMETERS:
**
**	close_frame	ptr to token frame for the closing source (probably
**			a semi-colon).
**
**  IMPLICIT INPUTS:
**
**	the "current" list on the frame stack as returned by sem_find_object
**
**  IMPLICIT OUTPUTS:
**
**	none
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**	none
**--
**/

void	sar_save_src_entry_end (close_frame, entry_frame)

yystype    *close_frame;
yystype    *entry_frame;

{
    sym_entry_type		* entry;

    /*
    ** Extract the entry from the frame.
    */

    entry = (sym_entry_type *) entry_frame->value.az_symbol_entry;

    /*
    ** Case on the type of entry (source gets put in a different spot for
    ** control entries).
    */

    if (entry->header.b_tag == sym_k_control_entry)
	{
	sym_control_entry_type *control_entry = (sym_control_entry_type *)entry;

	_sar_save_source_pos (&control_entry->az_con_obj->header, close_frame);
	}

    /*
    ** Save the source info in the default place
    */

    _sar_save_source_pos ( &entry->header , close_frame );

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This routine sets flags in the stack entry for the object.
**
**  FORMAL PARAMETERS:
**
**	current_frame		address of the current syntax stack frame
**
**      mask			mask of flags to be set.
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

void	sar_set_object_flags 

	(yystype *current_frame, unsigned char mask )

{

    sym_obj_entry_type		* obj_entry;
    yystype			* object_frame;

/*    Search the syntax stack for the object frame.    */

    object_frame = sem_find_object (current_frame - 1);
    obj_entry = (sym_obj_entry_type *) object_frame->value.az_symbol_entry;

/*    Set the flags for the object entry.	*/

    obj_entry->obj_header.b_flags |= mask;

/*    If this is an exported or private object and it has a name, 
**    make an external entry for it.	
*/

    if ((mask & (sym_m_exported | sym_m_private)) &&
  	(obj_entry->obj_header.az_name != NULL))
	{
	sym_make_external_def (obj_entry->obj_header.az_name);
	}

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This routine unsets flags in the stack entry for the object.
**
**  FORMAL PARAMETERS:
**
**	current_frame		address of the current syntax stack frame
**
**      mask			mask of flags to be unset.
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

void	sar_unset_object_flags 

	(yystype *current_frame, unsigned char mask )

{
    yystype			* object_frame;

/*    Search the syntax stack for the object frame.    */

    object_frame = sem_find_object (current_frame - 1);

/*    Unset the flags for the object entry.	*/

    object_frame->b_flags &= ~mask;

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This routine sets the type in the stack entry for the (list) object.
**
**  FORMAL PARAMETERS:
**
**	current_frame		address of the current syntax stack frame
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

void		sar_set_list_type
			( current_frame )

yystype		* current_frame;

{

    sym_obj_entry_type		* obj_entry;
    yystype			* list_frame;

/*    Search the syntax stack for the list frame.    */
    list_frame = sem_find_object (current_frame-1);
    obj_entry = (sym_obj_entry_type *) list_frame->value.az_symbol_entry;

/*    Set the type for the list entry.	*/
    obj_entry->header.b_type = current_frame->b_type;

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This routine sets the type in the stack entry for the (widget) object.
**
**  FORMAL PARAMETERS:
**
**	current_frame		address of the current syntax stack frame
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

void		sar_set_object_class
			( current_frame )

yystype		* current_frame;

{

    sym_obj_entry_type		* obj_entry;
    yystype			* object_frame;

/*    Search the syntax stack for the object frame.    */
    object_frame = sem_find_object (current_frame-1);
    obj_entry = (sym_obj_entry_type *) object_frame->value.az_symbol_entry;

/*    Set the type for the object entry.	*/
    obj_entry->header.b_type =
	current_frame->value.az_keyword_entry->b_subclass;

/*    
**  Indicate in compression table that this object type is used.
**  Note that user defined widgets don't get compression code entires.
**  We always identify user defined widgets as MrmwcUnknown.
*/
    if ( obj_entry->header.b_type != uil_sym_user_defined_object )
        uil_widget_compr[obj_entry->header.b_type] = 1;

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This routine sets the variant in the stack entry for the object.
**
**  FORMAL PARAMETERS:
**
**	current_frame		address of the current syntax stack frame
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

void		sar_set_object_variant
			( current_frame )

yystype		* current_frame;

{

    sym_obj_entry_type		* obj_entry;
    yystype			* object_frame;

/*    Search the syntax stack for the object frame.    */

    object_frame = sem_find_object (current_frame - 1);
    obj_entry = (sym_obj_entry_type *) object_frame->value.az_symbol_entry;

/*    Set the variant for the object entry.	*/

    switch (current_frame->b_type)
	{

/*    Use the module default for this object type.   */
	case 0:
	    {
	    unsigned int	obj_type;

	    /*
	     * Pick up gadget variant (or widget variant) as specified
	     * by the module tables and the gadget variants
	     */
	    obj_type = obj_entry->header.b_type;
	    if (uil_urm_variant[obj_type] == sym_k_gadget_entry)
		{
		obj_entry->obj_header.b_flags |= sym_m_obj_is_gadget;
		obj_entry->header.b_type = uil_gadget_variants [obj_type];
		}

	    break;
	    }

	case sym_k_widget_entry:
	    break;

	case sym_k_gadget_entry:
	    {
	    unsigned int	obj_type;

	    /*
	     * Check if gadgets are supported for this object type.
	     * If so, change the object type to the matching code for
	     * the widget class which is the gadget.
	     */
	    obj_type = obj_entry->header.b_type;
	    if (uil_gadget_variants[obj_type] == 0)
		{
		yystype		* source_frame;

		source_frame = & yylval;
		diag_issue_diagnostic
		    (d_gadget_not_sup,
		     _sar_source_position (source_frame ),
		     diag_object_text(obj_type),
		     diag_object_text(obj_type) );
		}
	    else
		{
		obj_entry->obj_header.b_flags |= sym_m_obj_is_gadget;
		obj_entry->header.b_type = uil_gadget_variants [obj_type];
		}

	    break;
	    }

	default:
    	    _assert (FALSE, "unexpected variant type");
	    break;
	}

    /*
    ** If this object is a gadget, mark that gadgets of this type have been
    ** used so we can later assign it a compression code. This is a safety
    ** set against the actual widget class.
    */

    if ((obj_entry->obj_header.b_flags & sym_m_obj_is_gadget) != 0)
	uil_widget_compr[obj_entry->header.b_type] = 1;

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This routine finds the object frame on the parse stack.
**
**  FORMAL PARAMETERS:
**
**      current_frame		address of the current parse stack frame.
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
**      address of the parse stack frame for this object.
**
**  SIDE EFFECTS:
**
**      none
**
**--
**/

yystype		* sem_find_object ( current_frame )

yystype 	* current_frame;

{

    yystype 	* object_frame;

    object_frame = current_frame;

/*    Search the syntax stack for the object frame.    */

    while ( (object_frame->b_tag != sar_k_object_frame) &&
	    (object_frame->b_tag != sar_k_module_frame) )
	object_frame--;

    if (object_frame->b_tag != sar_k_object_frame)
        _assert (FALSE, "missing object frame on the parser stack");

   return (object_frame);

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This routine processes a reference to an object in the
**	UIL.  The reference may be a forward reference.
**
**  FORMAL PARAMETERS:
**
**      ref_frame		address of the parse stack frame for
**				the object reference.
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

void		sar_object_reference ( ref_frame )
yystype 	* ref_frame;

{
    yystype			* obj_frame;
    sym_obj_entry_type		* obj_entry;
    sym_name_entry_type		* ref_name;
    sym_obj_entry_type		* ref_entry;
    sym_value_entry_type	* ref_value;
    boolean			invalid_ref;

    yystype			* source_frame;

    source_frame = & yylval;

/*    Search the syntax stack for the object frame.    */

    obj_frame = sem_find_object (ref_frame - 1);
    obj_entry = (sym_obj_entry_type *) obj_frame->value.az_symbol_entry;
    ref_name = (sym_name_entry_type *) ref_frame->value.az_symbol_entry;
    ref_value = (sym_value_entry_type *) ref_name->az_object;
    ref_entry = (sym_obj_entry_type *) ref_name->az_object;

/*    Check if this name was previously defined for a different usage.	*/

    if (ref_entry != NULL)
	{
	if ( ref_entry->header.b_tag==sym_k_widget_entry ||
	     ref_entry->header.b_tag==sym_k_gadget_entry ||
	     ref_entry->header.b_tag==sym_k_child_entry )
	    invalid_ref =
		(ref_entry->header.b_tag!=obj_entry->header.b_tag) ||
		((ref_entry->header.b_type!=obj_entry->header.b_type) &&
		 (uil_gadget_variants[ref_entry->header.b_type]!=
		  obj_entry->header.b_type) &&
		 (uil_gadget_variants[obj_entry->header.b_type]!=
		  ref_entry->header.b_type));
	else
	    invalid_ref =
		(ref_entry->header.b_tag!=obj_entry->header.b_tag) ||
		(ref_entry->header.b_type!=obj_entry->header.b_type);
	
	if ( invalid_ref )
	    {

	    char	* expected_type, * found_type;

	    if (ref_entry->header.b_tag == sym_k_list_entry)
		found_type = diag_tag_text (ref_entry->header.b_type);
	    else if (ref_entry->header.b_tag == sym_k_widget_entry)
		found_type = diag_object_text (ref_entry->header.b_type);
	    else if (ref_entry->header.b_tag == sym_k_gadget_entry)
		found_type = diag_object_text (ref_entry->header.b_type);
	    else if (ref_entry->header.b_tag == sym_k_value_entry)
		found_type = diag_value_text
		    (((sym_value_entry_type *) ref_entry)->b_type);
	    else
		found_type = "";

	    if (obj_entry->header.b_tag == sym_k_list_entry)
		expected_type =
		    diag_tag_text (obj_entry->header.b_type);
	    else
		expected_type =
		    diag_object_text (obj_entry->header.b_type);

	    diag_issue_diagnostic
		( d_obj_type,
		  _sar_source_position ( source_frame ),
		  found_type,
		  diag_tag_text (ref_entry->header.b_tag),
		  expected_type,
		  diag_tag_text (obj_entry->header.b_tag) );

    	    obj_entry->header.b_tag = sym_k_error_entry;

            return;
	    }
	}

    switch (obj_entry->header.b_tag)
	{

	case sym_k_list_entry:
	    {

    /*   Add this entry to the list.  A copy of the list will be made.    */

	    if ((ref_value != 0) && 
		((ref_value->obj_header.b_flags & sym_m_forward_ref) == 0))
		{
	        ref_frame->value.az_symbol_entry = (sym_entry_type *)ref_entry;
	        sar_add_list_entry (ref_frame);
		}
	    else
		sar_add_forward_list_entry (ref_frame);
	    
	    break;
	    }

	case sym_k_gadget_entry:
	case sym_k_widget_entry:
	    {
	    int		make_fwd_ref;

    /*    Mark the widget as referenced.	*/

	    ref_name->b_flags |= sym_m_referenced;

    /*    Mark the referencing object       */

        obj_entry->obj_header.b_flags |= sym_m_obj_is_reference;

    /*    Forward references are allowed for widgets or gadgets.  */

	    if (ref_entry == NULL)
		make_fwd_ref = TRUE;
	    else
		{

    /*   A widget can reference itself; treat it as a forward reference. */

		if (ref_entry->obj_header.b_flags & sym_m_def_in_progress)
		    make_fwd_ref = TRUE;
		else
		    make_fwd_ref = FALSE;
		}

	    if (make_fwd_ref)
		{
		/*    Add forward reference entry for this widget.	*/

		sym_make_forward_ref
		    (ref_frame,
		     obj_entry->header.b_type,
		     (char*)& obj_entry->obj_header.az_reference );
		}
	    else
		{
		/*   Save this reference in the widget.	*/
		
		obj_entry->obj_header.az_reference = (sym_entry_type *)ref_entry;
		}
	    
	    break;
	    }

	default:
	    {
	    _assert (FALSE, "unexpected object reference type");
	    break;
	    }
	}

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This routine updates the parent list of every object in the controls
**      list(s) for this object.  Parent lists are required in order to check
**      constraint arguments.
**
**  FORMAL PARAMETERS:
**
**      control_list_frame		address of the parse stack frame for
**				                the control list.
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

void		sar_update_parent_list
			( control_list_frame )

yystype 	* control_list_frame;

{
    yystype			* widget_frame;
    sym_widget_entry_type	* widget_entry;
    sym_list_entry_type	* control_list_entry;

/* Search the syntax stack for the widget frame. */

    widget_frame = sem_find_object (control_list_frame - 1);
    widget_entry = (sym_widget_entry_type *)
			widget_frame->value.az_symbol_entry;

    _assert (widget_entry->header.b_tag == sym_k_widget_entry ||
	     widget_entry->header.b_tag == sym_k_gadget_entry ||
	     widget_entry->header.b_tag == sym_k_child_entry,
	     "widget missing from the stack");

/* Get the control_list entry from the widget */

    control_list_entry = (sym_list_entry_type *) 
			control_list_frame->value.az_symbol_entry;

    _assert ((control_list_entry->header.b_tag == sym_k_list_entry ||
	      control_list_entry->header.b_tag == sym_k_error_entry),
	     "list entry missing");

/* The control list contains control list entries as well as nested lists,
** which in turn contain list entries and nested lists.
** We need to call a recursive routine to traverse all the entries.
*/

    parent_list_traverse(widget_entry, control_list_entry);

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine recursively traverses a control_list.  Control lists
**	may contain control list entries as well as nested control lists.
**	
**      This routine also updates the parent list of every object in the 
**	controls list(s) for this object.  Parent lists are required in order 
**	to check constraint arguments.
**
**  FORMAL PARAMETERS:
**
**	widget_entry			the widget to be entered in lists
**      control_list_entry		A control_list or nested control list
**				        
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

void parent_list_traverse (widget_entry, control_list_entry)
    sym_widget_entry_type	*widget_entry;
    sym_list_entry_type 	*control_list_entry;

{
sym_obj_entry_type		*control_list_member;
sym_control_entry_type		*control_entry;
sym_nested_list_entry_type	*nested_control_list_entry;
sym_widget_entry_type		*control_widget;
int				found;
sym_forward_ref_entry_type	*fwd_ref_entry;
sym_parent_list_type		*parent_node;
sym_parent_list_type		*parent_ptr;


    for (control_list_member = (sym_obj_entry_type *)control_list_entry ->
					obj_header.az_next;
	control_list_member != NULL;
	control_list_member = (sym_obj_entry_type *)control_list_member ->
					obj_header.az_next)
	{
	switch (control_list_member->header.b_tag)
	    {
	    case sym_k_nested_list_entry:
		nested_control_list_entry = (sym_nested_list_entry_type *)
			control_list_member;
                /* Begin fixing DTS 9497 */
                if(nested_control_list_entry->az_list)
		       parent_list_traverse (widget_entry,
				      nested_control_list_entry->az_list);
                /* End fixing DTS 9497 */
		break;
	    case sym_k_control_entry:
		control_entry = (sym_control_entry_type *) control_list_member;

/*  Get a pointer to one of the actual widgets in the control list */

		control_widget =  control_entry->az_con_obj;

/*
**  If it's a widget reference, go find it.  If you can't find it, it must
**  be a forward reference.  If so, find the forward reference entry for it
**  and update it with a pointer to its parent.
*/
    
		if ( control_widget->
			obj_header.b_flags & sym_m_obj_is_reference)
        	    if ( control_widget->obj_header.az_reference == NULL )
            		{

/*  Forward reference. Update forward reference entry. */

			found = FALSE;
			for (fwd_ref_entry = sym_az_forward_ref_chain;
			    ((fwd_ref_entry != NULL) && (found == FALSE));
			    fwd_ref_entry = fwd_ref_entry->az_next_ref)
			    {
			    if (fwd_ref_entry->a_update_location ==
				(char *) & control_widget->
				obj_header.az_reference)
				{
				found = TRUE;
				fwd_ref_entry->parent = widget_entry;
				}
			    }            
			}
		    else
			{
/*  A widget reference, but already defined.  Go update its entry. */

			control_widget = (sym_widget_entry_type *)
			    control_widget->obj_header.az_reference;
			found = FALSE;
			for (parent_ptr = control_widget->parent_list;
			    ((parent_ptr != NULL) && (found == FALSE));
			    parent_ptr = parent_ptr->next)
			    {
			    if (parent_ptr->parent == widget_entry)
				found = TRUE;
			    }
			if (found == FALSE)
			    {
			    parent_node = (sym_parent_list_type *) 
				sem_allocate_node (sym_k_parent_list_entry, 
						   sym_k_parent_list_size);
			    parent_node->next = control_widget->parent_list;
			    control_widget->parent_list = parent_node;
			    parent_node->parent = widget_entry;
			    }
			}
		else
		    {
/*  An inline widget definition.  Go update its entry. */

		    found = FALSE;
		    for (parent_ptr = control_widget->parent_list;
			((parent_ptr != NULL) && (found == FALSE));
			parent_ptr = parent_ptr->next)
			{
			if (parent_ptr->parent == widget_entry)
			    found = TRUE;
			}
		    if (found == FALSE)
			{
			parent_node = (sym_parent_list_type *) 
			    sem_allocate_node (sym_k_parent_list_entry,
					       sym_k_parent_list_size);
			parent_node->next = control_widget->parent_list;
			control_widget->parent_list = parent_node;
			parent_node->parent = widget_entry;
			}
		    }
		}
	    }

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This routine saves a widget feature in the widget symbol node.
**
**  FORMAL PARAMETERS:
**
**      feature_frame		address of the parse stack frame for
**				the widget feature.
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

void		sar_save_feature
			( feature_frame )

yystype 	* feature_frame;

{
    yystype			* widget_frame;
    sym_widget_entry_type	* widget_entry;
    sym_entry_type		* feature_entry;
    sym_entry_type		* * ptr;

    yystype			* source_frame;

    source_frame = & yylval;

/*    Search the syntax stack for the widget frame.    */

    widget_frame = sem_find_object (feature_frame - 1);
    widget_entry = (sym_widget_entry_type *)
			widget_frame->value.az_symbol_entry;

    _assert (widget_entry->header.b_tag == sym_k_widget_entry ||
	     widget_entry->header.b_tag == sym_k_gadget_entry ||
	     widget_entry->header.b_tag == sym_k_child_entry,
	     "widget missing from the stack");

    feature_entry = feature_frame->value.az_symbol_entry;

    _assert ((feature_entry->header.b_tag == sym_k_list_entry ||
	      feature_entry->header.b_tag == sym_k_error_entry),
	     "list entry missing");

    switch (feature_entry->header.b_type)
	{

	case sym_k_argument_entry:
	    ptr = (sym_entry_type * *) & widget_entry->az_arguments;
	    break;

	case sym_k_control_entry:
	    ptr = (sym_entry_type * *) & widget_entry->az_controls;
	    break;

	case sym_k_callback_entry:
	    ptr = (sym_entry_type * *) & widget_entry->az_callbacks;
	    break;

	case sym_k_error_entry:
	    return;

	default:
	    _assert (FALSE, "unexpected widget feature");
	    break;
	}

/*	Check for duplicate features.	*/

    if (* ptr != NULL)
	{
	diag_issue_diagnostic
		( d_dup_list,
		  _sar_source_position ( source_frame ),
		  diag_tag_text (feature_entry->header.b_type),
		  diag_tag_text (feature_entry->header.b_tag),
		  diag_object_text (widget_entry->header.b_type),
		  diag_tag_text (widget_entry->header.b_tag) );

	return;
	}

/*	Save the feature in the widget.	*/

    (* ptr) = feature_entry;

/*	Clear the feature frame from the stack.	*/

    feature_frame->b_tag = sar_k_null_frame;

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This routine processes an argument pair for an object.
**
**  FORMAL PARAMETERS:
**
**      argument_frame		address of the parse stack frame for
**				the argument reference.
**
**      value_frame		address of the parse stack frame for
**				the argument value.
**
**	equals_frame		address of the parse stack frame for the
**				equals sign.
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
void		sar_save_argument_pair
  		  ( argument_frame, value_frame, equals_frame)

yystype 	* argument_frame;
yystype 	* value_frame;
yystype		* equals_frame;

{

    yystype			* object_frame;
    sym_argument_entry_type	* arg_entry;
    sym_list_entry_type		* list_entry;
    sym_value_entry_type	* val_value_entry;
    sym_value_entry_type	* arg_value_entry;
    unsigned char		actual_tag;

    yystype			* source_frame;


    source_frame = & yylval;

/*    Search the syntax stack for the object frame.    */

    object_frame = sem_find_object (argument_frame - 1);

    list_entry = (sym_list_entry_type *)
			object_frame->value.az_symbol_entry;

    _assert (list_entry->header.b_tag == sym_k_list_entry,
	     "list entry missing");

    arg_value_entry =
	(sym_value_entry_type *) argument_frame->value.az_symbol_entry;

    _assert (arg_value_entry->header.b_tag == sym_k_value_entry,
	     "argument value entry missing");

    /*
    **  Save the source information (?)
    */

    _sar_save_source_info ( &arg_value_entry->header , argument_frame , 
	argument_frame );

    val_value_entry = (sym_value_entry_type *) value_frame->value.az_symbol_entry;
    actual_tag = val_value_entry->header.b_tag;

/*    Create and fill in the argument node.    */

    arg_entry = (sym_argument_entry_type *) sem_allocate_node (
			sym_k_argument_entry, sym_k_argument_entry_size);

    /*
    ** If the argument is a forward reference, we'll patch in the
    ** address of the the referenced arg between passes.  Otherwise,
    ** just point to the referenced arg node.
    */

    if ((argument_frame->b_flags & sym_m_forward_ref) != 0)
	sym_make_value_forward_ref (argument_frame, 
	(char*)&(arg_entry->az_arg_name), sym_k_patch_add);
    else
        arg_entry->az_arg_name =
	    (sym_value_entry_type *) argument_frame->value.az_symbol_entry;

    /*
    ** If the argument value is a forward reference, we'll patch in the
    ** address of the the referenced arg value between passes.  Otherwise,
    ** just point to the referenced arg value node.
    */

    if ((value_frame->b_flags & sym_m_forward_ref) != 0)
	sym_make_value_forward_ref (value_frame, 
	(char*)&(arg_entry->az_arg_value), sym_k_patch_add);
    else
        arg_entry->az_arg_value = val_value_entry;

    argument_frame->b_tag = sar_k_null_frame;
    argument_frame->value.az_symbol_entry = (sym_entry_type *) arg_entry;


}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**  This routine processes a reason to procedure or procedure list binding 
**  for a callback object in UIL.
**
**  FORMAL PARAMETERS:
**
**      reason_frame		    address of the parse stack frame for
**				                the reason reference.
**
**      proc_ref_frame		    address of the parse stack frame for
**				                the procedure reference.
**
**	equals_frame		    address if the parse stack frame for
**						the equals sign.
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

void		sar_save_reason_binding
  		  	( reason_frame, proc_ref_frame, equals_frame )

yystype		* reason_frame;
yystype 	* proc_ref_frame;
yystype 	* equals_frame;

{

    yystype			* object_frame;
    sym_callback_entry_type	* callback_entry;
    sym_list_entry_type		* list_entry;
    yystype			* source_frame;

    source_frame = & yylval;

/*    Search the syntax stack for the object frame.    */

    object_frame = sem_find_object (reason_frame - 1);

    list_entry = (sym_list_entry_type *) object_frame->value.az_symbol_entry;

    _assert (list_entry->header.b_tag == sym_k_list_entry,
	     "list entry missing");

    /*
    ** Create and fill in the callback node.
    */

    callback_entry = (sym_callback_entry_type *) sem_allocate_node (
			sym_k_callback_entry, sym_k_callback_entry_size);

    /*
    ** If the reason is a forward reference, we'll patch in the
    ** address of the the referenced reason between passes.  Otherwise,
    ** just point to the referenced reason node.
    */

    if ((reason_frame->b_flags & sym_m_forward_ref) != 0)
	sym_make_value_forward_ref (reason_frame,
	(char*)&(callback_entry->az_call_reason_name), sym_k_patch_add);
    else
        callback_entry->az_call_reason_name =
	    (sym_value_entry_type *) reason_frame->value.az_symbol_entry;


    /*
    ** Save source information
    */
/*    _sar_save_source_info ( &reason_value_entry->header , reason_frame , 
**	reason_frame );
*/

    /*
    **    Note that proc_ref_frame may point to either a procedure reference
    **    or to a list of procedure reference nodes
    */

    if (  proc_ref_frame->b_type == sym_k_list_entry)
        callback_entry->az_call_proc_ref_list =
        	(sym_list_entry_type *) proc_ref_frame->value.az_symbol_entry;
    else
        callback_entry->az_call_proc_ref =
      	    (sym_proc_ref_entry_type *) proc_ref_frame->value.az_symbol_entry;

    reason_frame->b_tag = sar_k_null_frame;
    reason_frame->value.az_symbol_entry = (sym_entry_type *) callback_entry;

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**    This routine processes a control clause.
**
**  FORMAL PARAMETERS:
**
**   managed_frame		address of the parse stack frame for
**				the managed flag for this control.
**
**   item_frame			address of the parse stack frame for
**				the control item object.
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

void		sar_save_control_item
			( managed_frame, item_frame )

yystype		* managed_frame;
yystype		* item_frame;

{

    yystype			* object_frame;
    sym_control_entry_type	* control_entry;
    sym_list_entry_type		* list_entry;
    yystype			* source_frame;

    source_frame = & yylval;

/*    Search the syntax stack for the object frame.    */

    object_frame = sem_find_object (managed_frame - 1);

    list_entry =
	(sym_list_entry_type *) object_frame->value.az_symbol_entry;

    _assert (list_entry->header.b_tag == sym_k_list_entry,
	     "list entry missing");

/*	Verify that this type of item is allowed on this list.	*/

    if (list_entry->header.b_type != sym_k_control_entry)
	{
	diag_issue_diagnostic
	    ( d_list_item,
	     _sar_source_position ( source_frame ),
	     diag_tag_text (sym_k_control_entry),
	     diag_tag_text (list_entry->header.b_type),
	     diag_tag_text (list_entry->header.b_tag) );
	
	return;
	}

/*    Create and fill in the control node.    */

    control_entry = (sym_control_entry_type *) sem_allocate_node (
			sym_k_control_entry, sym_k_control_entry_size);

    control_entry->az_con_obj =
	(sym_widget_entry_type *) item_frame->value.az_symbol_entry;

    control_entry->obj_header.b_flags = ( item_frame->b_flags |
					      managed_frame->b_flags );

    managed_frame->b_tag =
    item_frame->b_tag = sar_k_null_frame;

    managed_frame->value.az_symbol_entry = (sym_entry_type *) control_entry;

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**    This routine processes a control clause when an id is created in that
**    clause.
**
**  FORMAL PARAMETERS:
**
**   control_frame		address of the parse stack frame for
**				the control list.
**
**   item_frame			address of the parse stack frame for
**				the control item id.
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

void		sar_save_control_widget
			( control_frame, item_frame )

yystype		* control_frame;
yystype		* item_frame;

{

    yystype			* object_frame;
    sym_control_entry_type	* control_entry;
    sym_list_entry_type		* list_entry;
    yystype			* source_frame;
    yystype			temp_frame;

    /*
    **	move the item_frame to the control_frame and
    **  the control_frame to the null_frame. This is done
    **  because the item_frame needs to be second in the list
    */

    temp_frame = *item_frame;
    *item_frame = *control_frame;
    *control_frame = temp_frame;

    source_frame = & yylval;

/*    Search the syntax stack for the object frame.    */

    object_frame = sem_find_object (control_frame - 1);

    list_entry =
	(sym_list_entry_type *) object_frame->value.az_symbol_entry;

    _assert (list_entry->header.b_tag == sym_k_list_entry,
	     "list entry missing");

/*	Verify that this type of item is allowed on this list.	*/

    if (list_entry->header.b_type != sym_k_control_entry)
	{
	diag_issue_diagnostic
	    ( d_list_item,
	     _sar_source_position ( source_frame ),
	     diag_tag_text (sym_k_control_entry),
	     diag_tag_text (list_entry->header.b_type),
	     diag_tag_text (list_entry->header.b_tag) );
	
	return;
	}

/*    Create and fill in the control node.    */

    control_entry = (sym_control_entry_type *) sem_allocate_node
	(sym_k_control_entry, sym_k_control_entry_size);

    control_entry->az_con_obj =
	(sym_widget_entry_type *) item_frame->value.az_symbol_entry;

    control_entry->obj_header.b_flags = item_frame->b_flags;

    control_frame->b_tag =
    item_frame->b_tag = sar_k_null_frame;

    control_frame->value.az_symbol_entry = (sym_entry_type *) control_entry;

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This routine saves the source for a user defined create procedure
**
**  FORMAL PARAMETERS:
**
**	procedure_frame		address of the parse stack frame for
**				the text "PROCEDURE".
**
**      proc_id_frame		address of the parse stack frame for
**				the procedure reference.
**
**      proc_arg_frame		address of the parse stack frame for
**				the procedure argument value.
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

void		sar_save_user_proc_ref_src
			( procedure_frame, proc_id_frame, proc_arg_frame)

yystype		* procedure_frame;
yystype 	* proc_id_frame;
yystype 	* proc_arg_frame;

{
    sym_proc_ref_entry_type	* proc_ref_entry;

    proc_ref_entry = (sym_proc_ref_entry_type *)proc_id_frame->value.az_symbol_entry;

    /*
    ** If the parameter arg clause was ommitted the source info should be null.
    ** We want to save the source for the "args" if it is there.
    */
    _sar_save_source_info (& proc_ref_entry->header, procedure_frame, proc_arg_frame );

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This routine processes a procedure reference.
**
**  FORMAL PARAMETERS:
**
**      proc_id_frame		address of the parse stack frame for
**				the procedure reference.
**
**      proc_arg_frame		address of the parse stack frame for
**				the procedure argument value.
**
**	context			indicates whether this is a callback
**				or a user-defined procedure reference.
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

void		sar_process_proc_ref
			( proc_id_frame, proc_arg_frame, context )

yystype 	* proc_id_frame;
yystype 	* proc_arg_frame;
int		context;

{

/*    Call the common routine to get the procedure reference node, and
      return it in the stack frame.	*/

    proc_id_frame->value.az_symbol_entry = (sym_entry_type *)
		sem_reference_procedure (
			proc_id_frame, proc_arg_frame,
			context );

/*    If this is the create proc for a user_defined widget, save it
      in the object node.	*/

    if (context == sym_k_object_proc)
	{
	yystype			* widget_frame;
	sym_widget_entry_type	* widget_entry;
	
/*    Search the syntax stack for the widget frame.  NOTE: gadgets can
	      not have creation procedures; the grammar enforces this.     */

	widget_frame = sem_find_object (proc_id_frame - 1);
	widget_entry =
	    (sym_widget_entry_type *) widget_frame->value.az_symbol_entry;
	
	_assert (widget_entry->header.b_tag == sym_k_widget_entry,
		 "widget missing from the stack");
	
	if (widget_entry->header.b_type != uil_sym_user_defined_object)
	    {
	    yystype		* source_frame;
	    
	    source_frame = & yylval;
	    diag_issue_diagnostic
		(d_create_proc,
		 _sar_source_position ( source_frame ),
		 diag_object_text (widget_entry->header.b_type) );
	    
	    return;
	    }
	else
	    {
	    widget_entry->az_create_proc =
		(sym_proc_ref_entry_type *) proc_id_frame->value.az_symbol_entry;
	    }
	
	}

    return;    

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This routine adds an entry to a list.
**
**  FORMAL PARAMETERS:
**
**      entry_frame		address of the parse stack frame for
**				the entry to be added to the list.
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

void		sar_add_list_entry
  		  	( entry_frame )

yystype		* entry_frame;

{
    yystype			* list_frame;
    sym_list_entry_type		* list_entry;
    sym_obj_entry_type		* entry_entry;

    yystype			* source_frame;

    source_frame = & yylval;

/*    Search the syntax stack for the list frame.    */

    list_frame = sem_find_object (entry_frame - 1);
    list_entry = (sym_list_entry_type *) list_frame->value.az_symbol_entry;

    _assert (list_entry->header.b_tag == sym_k_list_entry,
	     "list entry missing");

    entry_entry = (sym_obj_entry_type *) entry_frame->value.az_symbol_entry;

    /*
    ** If we are including a list within a list, put a nested list entry
    ** in the list, and point it to the actual list.
    */

    if 	(entry_entry->header.b_tag == sym_k_list_entry)
	{
	sym_nested_list_entry_type	*nested_entry;

	/*
	** If this list is a reference to a previously defined list,
	** then use the previously defined list.
	*/
	if (entry_entry->obj_header.az_reference != NULL)
	    {
	    entry_entry = (sym_obj_entry_type *)
		entry_entry->obj_header.az_reference;
	    _assert (entry_entry->header.b_tag == sym_k_list_entry,
		     "entry list entry missing");
	    }

	/*
	** Create a nested list entry to reference the nested list. This
	** becomes the entry which will be added to the current list.
	*/
	nested_entry = (sym_nested_list_entry_type *)
	    sem_allocate_node (sym_k_nested_list_entry,
			       sym_k_nested_list_entry_size);
	nested_entry->header.b_type = entry_entry->header.b_type;
	nested_entry->az_list = (sym_list_entry_type *) entry_entry;
	entry_entry = (sym_obj_entry_type *) nested_entry;
	}
    else
        if (entry_entry->header.b_tag == sym_k_name_entry)
	    {
	    sym_nested_list_entry_type	*nested_entry;
	    /*
	    ** This is a forward reference to a named, nested list.
	    */
	    nested_entry = (sym_nested_list_entry_type *)
	        sem_allocate_node (sym_k_nested_list_entry,
				   sym_k_nested_list_entry_size);

	    sym_make_value_forward_ref (entry_frame,
		(char*)&(nested_entry->az_list),
		sym_k_patch_list_add);

	    entry_entry = (sym_obj_entry_type *) nested_entry;
	    }

    /*
    ** Add the entry to front of the list
    ** The nested entry created above is included in this processing.
    */
    entry_entry->obj_header.az_next =
	(sym_entry_type *) list_entry->obj_header.az_next;
    list_entry->obj_header.az_next =
	(sym_entry_type *) entry_entry;
    list_entry->w_count++;

    entry_frame->b_tag = sar_k_null_frame;

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This routine adds a forward referenced list entry to a list.
**	
**
**  FORMAL PARAMETERS:
**
**      entry_frame		address of the parse stack frame for
**				the entry to be added to the list.
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

void		sar_add_forward_list_entry
  		  	( entry_frame )

yystype		* entry_frame;

{
    yystype			* list_frame;
    sym_list_entry_type		* list_entry;
    sym_obj_entry_type		* entry_entry;
    sym_name_entry_type		* name_entry;
    yystype			* source_frame;
    sym_nested_list_entry_type	*nested_entry;

    source_frame = & yylval;

/*    Search the syntax stack for the list frame.    */

    list_frame = sem_find_object (entry_frame - 1);
    list_entry = (sym_list_entry_type *) list_frame->value.az_symbol_entry;

    _assert (list_entry->header.b_tag == sym_k_list_entry,
	     "list entry missing");

    name_entry = (sym_name_entry_type *) entry_frame->value.az_symbol_entry;

    nested_entry = (sym_nested_list_entry_type *)
	sem_allocate_node (sym_k_nested_list_entry,
			   sym_k_nested_list_entry_size);

    sym_make_value_forward_ref (entry_frame,
	(char*)&(nested_entry->az_list),
	sym_k_patch_list_add);

    entry_entry = (sym_obj_entry_type *) nested_entry;

    /*
    ** Add the entry to front of the list
    ** The nested entry created above is included in this processing.
    */
    entry_entry->obj_header.az_next =
	(sym_entry_type *) list_entry->obj_header.az_next;
    list_entry->obj_header.az_next =
	(sym_entry_type *) entry_entry;
    list_entry->w_count++;

    entry_frame->b_tag = sar_k_null_frame;

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This routine verifies that the list or widget has been defined
**	correctly. Virtually all such validation is actually done in pass 2.
**
**  FORMAL PARAMETERS:
**
**	current_frame		address of the current syntax stack frame
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

void	sar_verify_object ( current_frame )

yystype			* current_frame;

{
yystype				* obj_frame;
sym_widget_entry_type		* widget_entry;
unsigned int			widget_type;
sym_obj_entry_type		* obj_entry;
yystype				* source_frame;



/*
 * Search the syntax stack for the object frame.
 */
source_frame = & yylval;
obj_frame = sem_find_object (current_frame - 1);
obj_entry = (sym_obj_entry_type *) obj_frame->value.az_symbol_entry;

switch (obj_entry->header.b_tag)
    {
    case sym_k_gadget_entry:
    case sym_k_widget_entry:
    
        /*
	 * Clear the definition in progress bit.
	 */
        _assert (obj_entry->obj_header.b_flags & sym_m_def_in_progress,
		 "widget definition not in progress");
	obj_entry->obj_header.b_flags &= (~ sym_m_def_in_progress);
	break;
    case sym_k_list_entry:
	/*
	 * Clear the definition in progress bit and return.
	 */
	_assert (obj_entry->obj_header.b_flags & sym_m_def_in_progress,
		 "list definition not in progress");
	obj_entry->obj_header.b_flags &= (~ sym_m_def_in_progress);
	return;

    case sym_k_error_entry:
	return;

    default:
	_assert (FALSE, "list or widget missing from the stack");
	break;
    }


/*
 * If this is a user_defined widget, be sure the create proc was
 * specified if this is a declaration, and not specified if it
 * is a reference.
 */
widget_entry = (sym_widget_entry_type *) obj_entry;
widget_type = widget_entry->header.b_type;
if (widget_type == uil_sym_user_defined_object)
    {
    if ((widget_entry->obj_header.b_flags & sym_m_obj_is_reference) != 0)
	{
	if (widget_entry->az_create_proc != NULL)
	    {
	    diag_issue_diagnostic
		(d_create_proc_inv,
		 _sar_source_pos2(widget_entry),
		 diag_object_text (widget_type) );
	    widget_entry->header.b_type = sym_k_error_object;
	    }
	}
    else
	{
	if (widget_entry->az_create_proc == NULL)
	    {
	    diag_issue_diagnostic
		(d_create_proc_req,
		 _sar_source_pos2(widget_entry),
		 diag_object_text (widget_type) );
	    widget_entry->header.b_type = sym_k_error_object;
	    }
	}
    }

}



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This routine allocates a symbol node of the specified size
**	and type.
**
**  FORMAL PARAMETERS:
**
**	node_tag 	tag of node to allocate
**	node_size	size of node to allocate
**
**  IMPLICIT INPUTS:
**
**
**  IMPLICIT OUTPUTS:
**
**
**  FUNCTION VALUE:
**
**      the address of the allocated node
**
**  SIDE EFFECTS:
**
**      The node is saved in the allocated node list
**
**--
**/

sym_entry_type	* sem_allocate_node
	(unsigned char node_tag, unsigned short node_size )

{

    sym_entry_type		* node_ptr;

    node_ptr = (sym_entry_type *) XtCalloc (1, node_size);
    node_ptr->header.w_node_size = node_size;
    node_ptr->header.b_tag = node_tag;
    UrmPlistAppendPointer (sym_az_allocated_nodes, (XtPointer)node_ptr);

    return node_ptr;

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This routine puts a symbol node on the free node list.
**
**  FORMAL PARAMETERS:
**
**	node_ptr	address of node to put on the free list
**
**  IMPLICIT INPUTS:
**
**
**  IMPLICIT OUTPUTS:
**
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**--
**/

void	sem_free_node ( node_ptr )

sym_entry_type		* node_ptr;

{

    UrmPlistAppendPointer (sym_az_freed_nodes, (XtPointer)node_ptr);
    
}
   
