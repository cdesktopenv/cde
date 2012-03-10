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
static char rcsid[] = "$XConsortium: UilSarProc.c /main/12 1995/07/14 09:37:43 drk $"
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
**      This module contain the routines for processing procedures.
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


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function create the procedure definition entry symbol 
**	node a procedure declaration.
**
**  FORMAL PARAMETERS:
**
**	id_frame	ptr to token frame for the procedure name
**	param_frame	ptr to token frame or null frame holding the
**			type of the argument
**	class_frame	ptr to frame whose b_flags holds private, etc. info
**
**  IMPLICIT INPUTS:
**
**	sym_az_current_section_entry	the "current" section list
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

void
sar_create_procedure(XmConst yystype *id_frame, 
                     XmConst yystype *param_frame, 
                     XmConst yystype *class_frame, 
                     XmConst yystype *semi_frame)
{
    sym_name_entry_type	    *name_entry;
    sym_proc_def_entry_type *proc_def_entry;
    sym_section_entry_type  *section_entry;
    sym_obj_entry_type	    *obj_entry;

    /*
    **  Call standard routine to check name entry for id_frame.
    **	This routine handles font name, color names, etc used as ids
    */

    name_entry = (sym_name_entry_type *) sem_dcl_name( id_frame );

    if (name_entry == NULL)
	return;

    /*
    **	Allocate the procedure definition entry and fill it in
    */

    proc_def_entry = (sym_proc_def_entry_type *)
	sem_allocate_node (sym_k_proc_def_entry, sym_k_proc_def_entry_size);

    proc_def_entry->b_widget_type = uil_max_object + 1;
    proc_def_entry->obj_header.az_name = (sym_name_entry_type *) name_entry;
    name_entry->az_object = (sym_entry_type *) proc_def_entry;

    /* 
    **	Parameter frame has 4 cases:
    **	    1) no argument checking desired
    **	       syntax: PROCEDURE id
    **	    2) argument checking desired - no argument
    **	       syntax: PROCEDURE id( )
    **	    3) argument checking desired - single argument
    **	       syntax: PROCEDURE id( type )
    **	    4) argument checking desired - single typed widget argument
    **	       syntax: PROCEDURE id( CLASS_NAME )
    **  These cases are distinguished as follows:
    **	    1) tag = null  type = 0
    **	    2) tag = null  type = sar_k_no_value
    **	    3) tag = token type = argument type
    **	    4) tag = object type = widget type
    */

    proc_def_entry->v_arg_checking = TRUE;

    switch (param_frame->b_tag)
    {
    case sar_k_null_frame:
	if (param_frame->b_type == sym_k_no_value )
	{
	    proc_def_entry->b_arg_count = 0;
	    proc_def_entry->b_arg_type = sym_k_no_value;
	}
	else
	    proc_def_entry->v_arg_checking = FALSE;
	    
	break;

    case sar_k_token_frame:
	proc_def_entry->b_arg_type = param_frame->b_type;
	proc_def_entry->b_arg_count = 1;
	break;

    case sar_k_object_frame:
	_assert((param_frame->b_type == sym_k_widget_entry),
		"object frame not widget entry");
	
	obj_entry = 
	  (sym_obj_entry_type *)param_frame->value.az_symbol_entry;

	proc_def_entry->b_arg_type = sym_k_widget_ref_value;
	proc_def_entry->b_arg_count = 1;
	proc_def_entry->b_widget_type = obj_entry->header.b_type;
	break;
	
    default:
	_assert( FALSE, "param frame in error" );

    }

    /*
    **	Process the class clause
    */

    switch (class_frame->b_flags)
    {
    case sym_m_exported:
	sym_make_external_def( name_entry );

    case sym_m_private:
    case sym_m_imported:
	break;

    default:
	_assert( FALSE, "class frame in error" );

    }

    proc_def_entry->obj_header.b_flags = class_frame->b_flags;

    /*
    ** save the source file info for this procedure entry
    */
    _sar_save_source_info (&proc_def_entry->header, id_frame, semi_frame );
    sar_assoc_comment((sym_obj_entry_type *)proc_def_entry);       /* preserve comments */

    /*
    ** allocate a section entry to link the proc_def entry into the structure
    */
    section_entry = (sym_section_entry_type *) sem_allocate_node
			(sym_k_section_entry, sym_k_section_entry_size);

    /*
    ** Link this entry off of the current section list
    */
    section_entry->next = (sym_entry_type *) sym_az_current_section_entry->entries;
    sym_az_current_section_entry->entries = (sym_entry_type *) section_entry;

    section_entry->entries = (sym_entry_type *) proc_def_entry;

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function processes a reference to a procedure.
**
**  FORMAL PARAMETERS:
**
**	id_frame	ptr to token frame for the procedure name
**	value_frame	ptr to token frame or null frame holding the
**			value of the argument to the procedure
**	context		value indicating how the procedure is being used
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
**      a procedure reference entry / NULL in case of an illegal reference
**
**  SIDE EFFECTS:
**
**      errors may be issued 
**
**--
**/

sym_proc_ref_entry_type
*sem_reference_procedure( yystype         *id_frame, 
                          XmConst yystype *value_frame, 
                          XmConst int     context )
{
    sym_value_entry_type    *value_entry;
    sym_proc_def_entry_type *proc_def_entry;
    sym_proc_ref_entry_type *proc_ref_entry;

    /*
    **  Call standard routine to check name entry for id_frame.
    **	This routine handles font name, color names, etc used as ids
    */

    proc_def_entry =
	(sym_proc_def_entry_type *)
	    sem_ref_name( id_frame, sym_k_proc_def_entry );

    switch (value_frame->b_tag)
    {
    case sar_k_null_frame:
	value_entry = NULL;
	break;

    case sar_k_value_frame:
	if ((value_frame->b_flags & sym_m_forward_ref) != 0)
	    value_entry = NULL;
	else
	    value_entry = (sym_value_entry_type *) 
			  value_frame->value.az_symbol_entry;
	break;

    case sar_k_object_frame:
	value_entry =
		(sym_value_entry_type *) value_frame->value.az_symbol_entry;
	break;

    default:
	_assert( FALSE, "actual arg in error" );
    }

    /*
    **	Allocate the procedure reference entry and fill it in
    */

    proc_ref_entry = (sym_proc_ref_entry_type *)
	sem_allocate_node (sym_k_proc_ref_entry, sym_k_proc_ref_entry_size);

    if ((id_frame->b_flags & sym_m_forward_ref) != 0)
        sym_make_value_forward_ref (id_frame, 
	(char*)&(proc_ref_entry->az_proc_def), sym_k_patch_list_add);
    else 
	proc_ref_entry->az_proc_def = proc_def_entry;

    if ((value_frame->b_flags & sym_m_forward_ref) != 0)
        sym_make_value_forward_ref (value_frame, 
	(char*)&(proc_ref_entry->az_arg_value), sym_k_patch_add);
    else
	proc_ref_entry->az_arg_value = value_entry;

    /*
    **	Apply context constraints
    **
    **	If this is a procedure being used as a user object,
    **	it should not have any arguments.  The arguments to such
    **	a procedure are always a parent widget id and an argument list.
    **	This constraint is currently inforced by the grammar.
    **
    **	At this time the compiler permits all types of values for callback
    **  arguments.  This may be limited shortly when we see if it is
    **  reasonable to pass fonts, colors, reasons, etc.
    */

    return proc_ref_entry;

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function checks to see if a object is defined with the name
**	corresponding to the id given in the first parameter.
**
**  FORMAL PARAMETERS:
**
**      id_frame	ptr to a token frame on the parse stack holding the name
**	tag		the type of construct needed
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
**      ptr to a symbol entry for construct or NULL
**
**  SIDE EFFECTS:
**
**	error message if the name is undefined or for a different construct
**	forward_ref bit may be turned on in id_frame
**--
**/

sym_entry_type
*sem_ref_name(yystype     *id_frame, 
              XmConst int tag)
{
    sym_name_entry_type	    *name_entry;
    sym_entry_type	    *symbol_entry;

    _assert( id_frame->b_tag == sar_k_token_frame, "arg1 not id frame" );

    /* 
    ** The id frame may hold a name or the keyword for a font name, color
    ** name, reason name etc.  If it is one of these special name, then
    ** we must see if the symbol table holds a name for the special type.
    */

    if (id_frame->b_type != NAME)
    {
	name_entry = 
	    sym_find_name
		( id_frame->value.az_keyword_entry->b_length,
		  id_frame->value.az_keyword_entry->at_name );

	if (name_entry == NULL)
	{
	    diag_issue_diagnostic
		( d_undefined,
		  _sar_source_position( id_frame ),
		  diag_tag_text( sym_k_proc_def_entry ),
		  id_frame->value.az_keyword_entry->at_name );

	    return NULL;
	}

    }
    else
	name_entry =
		(sym_name_entry_type *) id_frame->value.az_symbol_entry;

    /*
    ** If the name entry already has no object linked from it, we are
    ** referencing an undefined object.
    */

    symbol_entry = name_entry->az_object;

    if (symbol_entry == NULL )
	{
	id_frame->b_flags |= sym_m_forward_ref;
	return NULL;
	}
    /*
    ** If the name entry has the wrong type of object, this is also
    ** an error.
    */

    if (symbol_entry->header.b_tag != tag )
    {
	diag_issue_diagnostic
	    ( d_ctx_req,
	      _sar_source_position( id_frame ),
	      diag_tag_text( tag ),
	      diag_tag_text( symbol_entry->header.b_tag ) );

	return NULL;
    }

    return symbol_entry;

}
