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
static char rcsid[] = "$XConsortium: UilP2Reslv.c /main/11 1995/07/14 09:36:35 drk $"
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
**      This module contain the second pass routines for resolving forward
**	references.
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
**      This function processes forward references from the first pass.
**
**  FORMAL PARAMETERS:
**
**      none
**
**  IMPLICIT INPUTS:
**
**      sym_az_forward_ref_chain
**	sym_az_val_forward_ref_chain
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

void	sem_resolve_forward_refs()

{
    sym_forward_ref_entry_type		* fwd_entry;
    sym_forward_ref_entry_type		* next_fwd_entry;
    sym_val_forward_ref_entry_type	* fwd_val_entry;
    sym_val_forward_ref_entry_type	* next_fwd_val_entry;
    sym_widget_entry_type		** target_obj_entry;
    sym_value_entry_type		** target_val_entry;
    sym_parent_list_type        	* parent_node;
    sym_parent_list_type        	* parent_ptr;
    int                         	found;

    /* 
    **	Forward references are placed on a chain by the first pass of
    **	the compiler.  This routine walks the chain checking that
    **	    1) name is now defined
    **	    2) name points to the correct type of object
    */

    for (fwd_entry = sym_az_forward_ref_chain;  
	 fwd_entry != NULL;  
	 fwd_entry = next_fwd_entry)
    {
	sym_name_entry_type	* name_entry;
	sym_widget_entry_type	* object_entry;
	unsigned short int	object_type;


	/*
	**  Save the pointer to the next forward entry so we can free the current 
	**  entry after it is processed.
	*/
	next_fwd_entry = fwd_entry->az_next_ref;


	/*
	**  Call the Status callback routine to report our progress.
	*/
	/* %COMPLETE */
	Uil_percent_complete = 60;
	if (Uil_cmd_z_command.status_cb != (Uil_continue_type(*)())NULL)
	    diag_report_status();    


	object_type = fwd_entry->header.b_type;
	name_entry = fwd_entry->az_name;
	object_entry = (sym_widget_entry_type *) name_entry->az_object;

	if (object_entry == NULL)
	{
	    diag_issue_diagnostic
		( d_never_def,
		  _sar_source_pos2 (fwd_entry),
		  diag_object_text( object_type ),
		  name_entry->c_text );
	    continue;
	}

	/*
	** Make sure object references are to correct type of object.
	** A forward reference naming a widget class may be correctly
	** resolved by the corresponding gadget class, and vice versa.
	*/
	if ((object_entry->header.b_type!=object_type) &&
	    (uil_gadget_variants[object_entry->header.b_type]!=object_type) &&
	    (uil_gadget_variants[object_type]!=object_entry->header.b_type))
	    {
	    diag_issue_diagnostic
		(d_ctx_req,
		 _sar_source_pos2(fwd_entry),
		 diag_object_text(object_type),
		 diag_object_text(object_entry->header.b_type));
	    continue;
	    }

	target_obj_entry =
		(sym_widget_entry_type * *) fwd_entry->a_update_location;
	*target_obj_entry = object_entry;

	/*
	** Update objects on forward refernce chain so that their parent_lists point
	** to the objects which reference them
	*/
	if (fwd_entry -> parent != NULL)
	    {
	    found = FALSE;
	    for (parent_ptr = object_entry -> parent_list;
		((parent_ptr != NULL) && (found == FALSE));
		parent_ptr = parent_ptr -> next)
		{
		if (parent_ptr -> parent == fwd_entry -> parent)
		    found = TRUE;
		}
	    if (found == FALSE)
		{
		parent_node = (sym_parent_list_type *) 
		    sem_allocate_node (sym_k_parent_list_entry, 
		    sym_k_parent_list_size);
		parent_node -> next = object_entry -> parent_list;
		object_entry -> parent_list = parent_node;
		parent_node -> parent = fwd_entry -> parent;
		}
	    }


	/*
	**  Free the Forward reference entry now that it is no longer needed
	*/    
	sem_free_node(( sym_entry_type *)fwd_entry);

    }


    /*
    **  Now resolve the forward references to values
    **/

    /* 
    **	Forward references are placed on a chain by the first pass of
    **	the compiler.  This routine walks the chain checking that
    **	    1) name is now defined
    **	    2) name points to the correct type of value
    */

    for (fwd_val_entry = sym_az_val_forward_ref_chain;  
	 fwd_val_entry != NULL;  
	 fwd_val_entry = next_fwd_val_entry)
    {
	sym_name_entry_type	* name_entry;
	sym_value_entry_type	* value_entry;
	sym_obj_entry_type	* obj_entry;


	/*
	**  Save the pointer to the next forward entry so we can free the current 
	**  entry after it is processed.
	*/
	next_fwd_val_entry = fwd_val_entry->az_next_ref;


	/*
	**  Call the Status callback routine to report our progress.
	*/
	/* %COMPLETE */
	Uil_percent_complete = 60;
	if (Uil_cmd_z_command.status_cb != (Uil_continue_type(*)())NULL)
	    diag_report_status();    

	name_entry = fwd_val_entry->az_name;
	value_entry = (sym_value_entry_type *) name_entry->az_object;
	obj_entry = (sym_obj_entry_type *) name_entry->az_object;

	if (value_entry == NULL)
	{
	    diag_issue_diagnostic
		( d_never_def,
		  _sar_source_pos2 (fwd_val_entry),
		  "value",
		  name_entry->c_text );
	    continue;
	}

	switch (fwd_val_entry->fwd_ref_flags)
	    {
	    case (sym_k_patch_add):
	    case (sym_k_patch_list_add):
		{
    	        target_val_entry =
		        (sym_value_entry_type * *) fwd_val_entry->a_update_location;
	        *target_val_entry = value_entry;
	        break;
	        }
	    default:
		_assert(FALSE, "Illegal forward reference");
            }


	/*
	**  Free the Forward reference entry now that it is no longer needed
	*/    
	sem_free_node(( sym_entry_type *)fwd_val_entry);

    }




}
