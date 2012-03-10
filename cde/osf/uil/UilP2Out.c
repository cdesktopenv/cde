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
static char rcsid[] = "$TOG: UilP2Out.c /main/15 1997/03/12 15:17:24 dbl $"
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
**      This module contain the routines for creating the UID file.
**
**--
**/


/*
**
**  INCLUDE FILES
**
**/

#ifdef DXM_V11
#include <DXm/DXmHelpB.h>
#endif

#include "UilDefI.h"
#include "UilSymGen.h"

/*
**
**  DEFINE and MACRO DEFINITIONS
**
**/

#ifdef WORD64
#define _shift 3
#else
#define _shift 2
#endif

#define initial_context_size 2048
/* alpha port note:
   the original define makes some assumptions on the sizes of the
   structures when calculating this value. I 'think' I have devined
   the intent of this and have replaced it with a more generic
   calculation.
   */
#define out_k_last_offset \
	(((src_k_max_source_line_length +1) / sizeof(sym_entry_type *)))

/* #define out_k_last_offset ((sizeof( src_source_buffer_type )>>_shift)-2) */

typedef	struct	_out_queue_type
{
    struct _out_queue_type  *az_prior_queue;
    sym_entry_type	    *entry[out_k_last_offset + 1];
} out_queue_type;


/*
**
**  EXTERNAL VARIABLE DECLARATIONS
**
**/

char	*_get_memory();
void	_free_memory();

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

externaldef(uil_comp_glbl) IDBFile		out_az_idbfile_id;

static  URMResourceContext	*out_az_context;
static	out_queue_type		*out_az_queue;
static	int			out_l_next_offset;
static UidCompressionTable     	*extern_arg_compr;
static UidCompressionTable      *extern_class_compr;



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function begins the process of creating the UID file.  It
**	handles:
**	    1) creating the UID file and closing it
**	    2) initializing the queue of other objects to be processed
**	    3) creating the module widget 
**	    4) emitting exported literals and procedures
**	    5) creating the compression code table for this UID file
**
**  FORMAL PARAMETERS:
**
**      none
**
**  IMPLICIT INPUTS:
**
**      sym_az_external_def_chain   list of exported object
**	src_az_avail_source_buffer  use source buffers for queue of objects
**				    requiring further attention
**
**  IMPLICIT OUTPUTS:
**
**      src_az_avail_source_buffer
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**      UID file is created
**
**--
**/

void	sem_output_uid_file()

{
    sym_external_def_entry_type	*ext_entry;
    char			*module_version;
    char			*module_name;
    status			urm_status;
    sym_entry_type		*symbol_entry;
    int				topmost_widget_count;
    int				topmost_index;
    struct
    {   MrmOsOpenParam	os_param;
	char		result_file[256];
    } uid_fcb;


    /*
    **  No UID file is created if any error severity message has
    **	been emitted.
    */

    if (uil_l_compile_status >= uil_k_error_status)
    {
	diag_issue_diagnostic( 
	    d_no_uid,
	    diag_k_no_source,
	    diag_k_no_column );

	return;
    }

    /*
    **	Request URM hold error message and report via status rather
    **	that sending them to stdout.  This permits the compiler to
    **	deal with the messages as it does with others.
    */

    urm_status =
    Urm__UT_SetErrorReport( URMErrOutMemory );
    if( urm_status != MrmSUCCESS)
	issue_urm_error( "allocating context" );

    /*
    **	Open the UID file
    */
    uid_fcb.os_param.version = MrmOsOpenParamVersion;


    /* clobber flag lets a.uid replace an existing a.uid */
    uid_fcb.os_param.nam_flg.clobber_flg = TRUE;

    module_version = "";
    module_name = sym_az_module_entry->obj_header.az_name->c_text;

    if (sym_az_module_entry->az_version != NULL)
	module_version = sym_az_module_entry->az_version->value.c_value;

    urm_status = UrmIdbOpenFileWrite
		  ( Uil_cmd_z_command.ac_resource_file,
		    & uid_fcb.os_param,
		    _host_compiler,
		    _compiler_version,
		    module_name,
		    module_version,
		    &out_az_idbfile_id,
		    uid_fcb.result_file );

    if (urm_status != MrmSUCCESS)
    {
	diag_issue_diagnostic( 
	    d_uid_open,
	    diag_k_no_source,
	    diag_k_no_column,
	    uid_fcb.result_file );

	return;
    }


    /*
    **	Set the name of the file we are accessing as the current file and call
    **	the Status callback routine to report our progress.
    */
    Uil_current_file =  uid_fcb.result_file;
    if (Uil_cmd_z_command.status_cb != (Uil_continue_type(*)())NULL)
	diag_report_status();    


    /*
    **	Create a context
    */

    urm_status =
    	UrmGetResourceContext ( _get_memory, _free_memory, 
	initial_context_size, &out_az_context );
    if( urm_status != MrmSUCCESS)
	issue_urm_error( "allocating context" );

    /*
    **	Initialize queue of objects to be processed.
    */

    out_l_next_offset = 0;
    out_az_queue = (out_queue_type *)src_az_avail_source_buffer;
    out_az_queue->az_prior_queue = NULL;
    src_az_avail_source_buffer = 
	src_az_avail_source_buffer->az_prior_source_buffer;

    /*
    **	Count the number of topmost widgets by scanning the external list.
    **	A topmost widget is one that is not referenced.
    */

    topmost_widget_count = 0;

    for (ext_entry = sym_az_external_def_chain;  
	 ext_entry != NULL;  
	 ext_entry = ext_entry->az_next_object)
    {
	symbol_entry = ext_entry->az_name->az_object;

	if (symbol_entry->header.b_tag == sym_k_widget_entry)
	    if ((((sym_widget_entry_type *)symbol_entry)->
		    obj_header.az_name->b_flags & sym_m_referenced) == 0)
		topmost_widget_count++;

    }

    /*
    **	Initialize the context to build the interface module.
    */

    urm_status =
    UrmIFMInitModule
	( out_az_context, topmost_widget_count, URMaPublic, FALSE );
    if( urm_status != MrmSUCCESS)
	issue_urm_error( "initializing module" );

    /*
    **  Create the compression code table for use with this UID file.
    */

    create_int_compression_codes();

    /* 
    **	Exported objects are on a chain that we will now walk.
    **	They can be of 3 types: widgets, gadgets and values.
    **	Values, gadgets and widgets are pushed on LIFO queue and processed 
    **	shortly.
    **	Each widget on the list is top most if it is not referenced.  Topmost
    **	widgets are added to the current context for the interface module.
    */

    topmost_index = 0;

    for (ext_entry = sym_az_external_def_chain;  
	 ext_entry != NULL;  
	 ext_entry = ext_entry->az_next_object)
    {
	/*
	**  Call the Status callback routine to report our progress.
	*/
      /* %COMPLETE */
      Uil_percent_complete=CEIL(
	    80+	(.20 *((float)topmost_index/(float)(topmost_widget_count+.5)))*100, 80);

	if (Uil_cmd_z_command.status_cb != (Uil_continue_type(*)())NULL)
	    diag_report_status();    

	symbol_entry = ext_entry->az_name->az_object;

	switch (symbol_entry->header.b_tag)
	{

	case sym_k_value_entry:
	{
	    sym_value_entry_type   *value_entry;

	    value_entry = (sym_value_entry_type *)symbol_entry;
	
	    value_entry->output_state = sym_k_queued;

	    push( (sym_entry_type *)value_entry );

	    break;
	}

	case sym_k_gadget_entry:
	case sym_k_widget_entry:
	{
	    sym_widget_entry_type   *widget_entry;

	    widget_entry = (sym_widget_entry_type *)symbol_entry;
	
	    if ((widget_entry->obj_header.az_name->b_flags & sym_m_referenced)
		== 0)
	    {
		widget_entry->output_state = sym_k_queued;

		push((sym_entry_type *) widget_entry );

		urm_status = 
		UrmIFMSetTopmost
		    ( out_az_context, 
		      topmost_index, 
		      widget_entry->obj_header.az_name->c_text );
		if( urm_status != MrmSUCCESS)
		    issue_urm_error( "adding topmost widget" );

		topmost_index++;
	    }

	    break;
	}

	default:
	    _assert( FALSE, "unexpected entry on external chain");
	    break;
	}
    }

    /*
    **	Emit the Interface Module
    */

    urm_status =
    UrmIFMPutModule
	( out_az_idbfile_id, module_name, out_az_context );
    if( urm_status != MrmSUCCESS)
	{
	if (urm_status == MrmEOF)
	    diag_issue_diagnostic ( d_uid_write, diag_k_no_source, 
				    diag_k_no_column, Uil_current_file );
	else
	    issue_urm_error( "emitting module" );
	}

    if (Uil_cmd_z_command.v_show_machine_code) {
	save_module_machine_code 
	    ( src_az_module_source_record, out_az_context );
    }

    /*
    **	Start to process the widgets that have been pushed on
    **	the queue.
    */

    for (symbol_entry = pop();  symbol_entry != NULL;  symbol_entry = pop() )
    {
	/*
	**  Call the Status callback routine to report our progress.
	*/
	if (Uil_cmd_z_command.status_cb != (Uil_continue_type(*)())NULL)
	    diag_report_status();    

	switch (symbol_entry->header.b_tag)
	{

	case sym_k_value_entry:
	    out_emit_value(( sym_value_entry_type *) symbol_entry );
	    break;

	case sym_k_widget_entry:
	case sym_k_gadget_entry:
	case sym_k_child_entry:
	    out_emit_widget(( sym_widget_entry_type *) symbol_entry );
	    break;

	default:
	    _assert( FALSE, "unexpected entry popped during output");
	    break;
	}
    }

    create_ext_compression_codes ();

    /*
    **	Close the UID file - if there are errors don't keep the file
    */

    urm_status = (uil_l_compile_status < uil_k_error_status);

    if (!urm_status)
    {
	diag_issue_diagnostic
	    ( d_no_uid, diag_k_no_source, diag_k_no_column );
    }

    urm_status =
    UrmIdbCloseFile( out_az_idbfile_id, urm_status );
	out_az_idbfile_id = NULL;
    if( urm_status != MrmSUCCESS)
      diag_issue_diagnostic
	( d_uid_write, diag_k_no_source, diag_k_no_column, Uil_current_file );


    /*
    **	Free the context 
    */

    urm_status =
    UrmFreeResourceContext( out_az_context );
    if( urm_status != MrmSUCCESS)
	issue_urm_error( "freeing context" );

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function pushes a symbol table entry into a LIFO queue.
**
**  FORMAL PARAMETERS:
**
**      sym_entry		pointer to symbol table to push
**
**  IMPLICIT INPUTS:
**
**      out_l_next_offset	next offset in current queue
**	out_az_queue		current queue
**	src_az_avail_source_buffer
**				next available queue buffer
**
**  IMPLICIT OUTPUTS:
**
**      same as inputs
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**      argument is placed on a queue
**
**--
**/

void	push(sym_entry)

sym_entry_type	*sym_entry;

{

    out_queue_type  *next_queue;

    /*
    **	We reuse source buffers for the output queues.
    */

    if (out_l_next_offset > out_k_last_offset)
    {
	if (src_az_avail_source_buffer == NULL)
	{
	    src_az_avail_source_buffer = 
		(src_source_buffer_type *)
		    _get_memory( sizeof( src_source_buffer_type ) );
	    src_az_avail_source_buffer->az_prior_source_buffer = NULL;
	}

	next_queue = (out_queue_type *)src_az_avail_source_buffer;
	src_az_avail_source_buffer = 
	    src_az_avail_source_buffer->az_prior_source_buffer;

	next_queue->az_prior_queue = out_az_queue;
	out_az_queue = next_queue;
	out_l_next_offset = 0;
    }

    out_az_queue->entry[out_l_next_offset] = sym_entry;

    out_l_next_offset++;

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function pops a symbol table entry from a LIFO queue.
**
**  FORMAL PARAMETERS:
**
**      none
**
**  IMPLICIT INPUTS:
**
**      out_l_next_offset	next offset in current queue
**	out_az_queue		current queue
**	src_az_avail_source_buffer
**				next available queue buffer
**
**  IMPLICIT OUTPUTS:
**
**      same as inputs
**
**  FUNCTION VALUE:
**
**      sym_entry		pointer to symbol table popped
**	NULL			when no more to pop
**
**  SIDE EFFECTS:
**
**      none
**
**--
**/

sym_entry_type	*pop()

{

    src_source_buffer_type  *avail_buffer;

    out_l_next_offset--;

    if (out_l_next_offset < 0)
    {
	avail_buffer = (src_source_buffer_type *)out_az_queue;
	out_az_queue = out_az_queue->az_prior_queue;
	avail_buffer->az_prior_source_buffer = src_az_avail_source_buffer;
	src_az_avail_source_buffer = avail_buffer;

	if (out_az_queue == NULL)
	    return NULL;

	out_l_next_offset = out_k_last_offset;
    }

    return out_az_queue->entry[out_l_next_offset];

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function builds the URM record for a widget.
**
**  FORMAL PARAMETERS:
**
**      widget_entry	    symbol table pointer for a widget
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
**      write a widget record to UID file
**      push other objects on the queue to be processed
**
**--
**/

void	out_emit_widget( widget_entry )

sym_widget_entry_type	*widget_entry;

{

char				buffer[32];
char				*widget_name;
char				*widget_class_name;
unsigned int			widget_class;
unsigned long			widget_variety;
int				arg_count;
int				related_arg_count;
int				arglist_index;
sym_list_entry_type		*list_entry;
MrmCode				access_code;
status				urm_status;
MrmCode				subtree_code;
sym_control_entry_type		*subtree_control;


_assert( (widget_entry->header.b_tag == sym_k_widget_entry) ||
	 (widget_entry->header.b_tag == sym_k_gadget_entry) ||
	 (widget_entry->header.b_tag == sym_k_child_entry),
	 "object to be emitted is not an object" );

_assert( (widget_entry->obj_header.b_flags & 
	 (sym_m_exported | sym_m_private)),
	 "object being emitted is not exported or private" );

if (widget_entry->header.b_tag == sym_k_child_entry)
  widget_variety = UilMrmAutoChildVariety;
else widget_variety = UilMrmWidgetVariety;

/*
* Each real widget needs a name.  Automatic children just get an
* empty string since the name is stored in the compression tables.
* For real widgets, we use the user provided name
* if there is one; otherwise widgetfile#-line#-col# 
* For example, widget-1-341-111 was defined in file=1, line=341
* and column=11
*/
if (widget_variety == UilMrmAutoChildVariety)
  widget_name = "";
else if (widget_entry->obj_header.az_name == NULL)
  {
    sprintf(buffer, "widget-%d-%d-%d", 
	    widget_entry->header.az_src_rec->b_file_number,
	    widget_entry->header.az_src_rec->w_line_number,
	    widget_entry->header.b_src_pos);
    widget_name = buffer;
  }
else
    widget_name = widget_entry->obj_header.az_name->c_text;

access_code = URMaPublic;
if (widget_entry->obj_header.b_flags & sym_m_private)
    access_code = URMaPrivate;

urm_status = UrmCWRInit (out_az_context, widget_name, access_code, FALSE);
if( urm_status != MrmSUCCESS)
    issue_urm_error( "initializing context" );

    /*
    **	Set the class of the widget. 
    */

    widget_class_name = NULL;

    arg_count = 0;
    related_arg_count = 0;
    subtree_control = NULL;

    /*
    ** Special processing 1: User defined widgets have the class as
    ** creation routine
    */
    if ( widget_entry->header.b_type == uil_sym_user_defined_object )
	{
	widget_class_name = 
	    widget_entry->az_create_proc->az_proc_def->obj_header.az_name->c_text;
	}

    /*
    ** Special processing 2. Widgets which map their (single) control to
    ** a (subtree) resource. This handles the convention (originally invented
    ** for the pulldown menu which is the child of a cascade button) that
    ** subtrees which must be instantiated to serve as a resource value are
    ** treated as children rather than as widget references. Multiple
    ** children issue a diagnostic.
    */
    subtree_code = uil_urm_subtree_resource[widget_entry->header.b_type];
    if ( subtree_code != 0 )
	{
	int			    count;

	list_entry = widget_entry->az_controls;
	count = 0;
	extract_subtree_control (list_entry, &subtree_control, &count);
	switch ( count )
	    {
	    case 0:
	        break;
	    case 1:
		arg_count += 1;
		break;
	    default:
		arg_count += 1;
	        diag_issue_diagnostic
		    (d_single_control,
		     _sar_source_pos2(subtree_control),
		     diag_object_text(widget_entry->header.b_type));
		break;
	    }
      }


    /*
     * Set the class in the widget record
     */
    if (widget_variety == UilMrmAutoChildVariety)
      widget_class = uil_child_compr[widget_entry->header.b_type];
    else widget_class = uil_widget_compr[widget_entry->header.b_type];      


    /*
     * User defined widgets don't get compressed.  
     */

    if (widget_entry->header.b_type == uil_sym_user_defined_object)
	widget_class = MrmwcUnknown;

    urm_status =
    UrmCWRSetClass( out_az_context, 
		    widget_class,
		    widget_class_name,
		    widget_variety );
    if( urm_status != MrmSUCCESS)
	issue_urm_error( "setting class" );

    /*
    **	Check the callback list for the creation reason and process it.
    **	Do this first since it affects arg_count.
    */
    list_entry = widget_entry->az_callbacks;
    if (list_entry != NULL)
	{
	sym_callback_entry_type	    *callback_entry;

	arg_count += compute_list_size (list_entry, sym_k_callback_entry);
	callback_entry = NULL;
	extract_create_callback (list_entry, &callback_entry);
	if ( callback_entry != NULL )
	    {
	    arglist_index = 0;
	    emit_callback (callback_entry, &arglist_index, TRUE);
	    arg_count -= 1;
	    }
	}

    /*
    **	Output the list of arguments.  Arguments are either callbacks
    **	or arguments.
    */

    if (widget_entry->az_arguments != NULL)
	arg_count += compute_list_size
	    (widget_entry->az_arguments, sym_k_argument_entry);

    if (arg_count > 0)
	{
	urm_status =
	UrmCWRInitArglist( out_az_context, arg_count );
	if( urm_status != MrmSUCCESS)
	    issue_urm_error( "initializing arglist" );
	arglist_index = arg_count - 1;

	/*
	**	Process the callbacks, then the arguments
	*/
	process_all_callbacks
	    (widget_entry->az_callbacks, &arglist_index);
	process_all_arguments
	    (widget_entry->az_arguments, &arglist_index, &related_arg_count);

	/*
	** Process a control which is to be entered as a subtree resource. Mark
	** the control so it won't be processed again.
	*/
	if (subtree_control != NULL)
	    {
	    MrmCode	    widget_access;
	    MrmCode	    widget_form;
	    char	    *widget_index;
	    MrmResource_id  widget_id;
	    
	    urm_status =
	    UrmCWRSetCompressedArgTag
		(out_az_context, arglist_index,
		uil_arg_compr[subtree_code], 0);
	    if( urm_status != MrmSUCCESS)
		issue_urm_error( "setting compressed arg" );

	    widget_form = 
		ref_control( subtree_control, 
			    &widget_access, &widget_index, &widget_id );
	    urm_status =
		UrmCWRSetArgResourceRef
		    (out_az_context,
		     arglist_index,
		     widget_access,
		     URMgWidget,
		     RGMwrTypeSubTree,
		     widget_form,
		     widget_index,
		     widget_id );
    	    if( urm_status != MrmSUCCESS)
		issue_urm_error( "setting arg reference" );
	    subtree_control->header.b_tag = sym_k_error_entry;

	    arglist_index++;
	    
	    }
	}

    /*
    **	Process controls
    */

    list_entry = widget_entry->az_controls;

    if (list_entry != NULL)
	{
	int			    widget_index;

	/*
	**  The list of controls is in reverse order.  To correct for
	**  this, controls are placed in the list from bottom to top.
	**  Thus widget_index represent the last slot in the list used.
	*/

	widget_index = compute_list_size (list_entry, sym_k_control_entry);
	if (widget_index > 0)
	    {
	    urm_status =
		UrmCWRInitChildren (out_az_context, widget_index );
	    if ( urm_status != MrmSUCCESS)
		issue_urm_error( "initializing children" );
	    process_all_controls (list_entry, &widget_index);
	    }
	}

    /*
    **	If we have any related arguments, report the number.
    */

    if (related_arg_count > 0)
	UrmCWRSetExtraArgs( out_az_context, related_arg_count );

    /*
    **	Emit the widget record to UID file
    **	All widgets are indexed as long as they are named.  This is so
    **	they can be included in module interface structure.
    */

    if (widget_entry->obj_header.az_name == NULL)
	{
	if (widget_entry->resource_id == 0 )
	    {
	    urm_status =
	    UrmIdbGetResourceId
		( out_az_idbfile_id, &(widget_entry->resource_id) );
	    if( urm_status != MrmSUCCESS)
		issue_urm_error( "obtaining resource id" );
	    }

        urm_status =
	    UrmPutRIDWidget
	    (out_az_idbfile_id,
	     widget_entry->resource_id,
	     out_az_context );
	}
    else
        urm_status =
	    UrmPutIndexedWidget
	    (out_az_idbfile_id,
	     widget_name,
	     out_az_context );

    if( urm_status != MrmSUCCESS)
	{
	if (urm_status == MrmEOF)
	    diag_issue_diagnostic ( d_uid_write, diag_k_no_source, 
				    diag_k_no_column, Uil_current_file );
	else
	    issue_urm_error( "emitting widget" );
	}

    if (Uil_cmd_z_command.v_show_machine_code)
	{
	save_widget_machine_code (widget_entry, out_az_context);
	}

    widget_entry->output_state = sym_k_emitted;
    }



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine extracts the controlled widget (top-level) from
**	a controls list for subtree resources (those whose subtree must
**	be instantiated as part of parent creation, but which are
**	implemented as resources, not children). It returns the count of
**	the number of such entries found, and a pointer to the last one
**	encountered.
**
**  FORMAL PARAMETERS:
**
**      list_entry		the list to be (recursively) searched
**	menu_entry		to return a ponter to the pulldown menu
**	count			counts the number of entries found
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
**--
**/

void extract_subtree_control (list_entry, menu_entry, count)
    sym_list_entry_type		*list_entry;
    sym_control_entry_type	**menu_entry;
    int				*count;
    
{

/*
 * Local variables
 */
sym_obj_entry_type		*list_member;
sym_nested_list_entry_type	*nested_list_entry;
sym_control_entry_type		*control_entry;


/*
 * Process the list elements, recursing on nested lists. Ignore error entries.
 */
if ( list_entry == NULL ) return;
for (list_member=(sym_obj_entry_type *)list_entry->obj_header.az_next;
     list_member!=NULL;
     list_member=(sym_obj_entry_type *)list_member->obj_header.az_next)
    switch ( list_member->header.b_tag )
	{
	case sym_k_nested_list_entry:
	    nested_list_entry = (sym_nested_list_entry_type *) list_member;
	    extract_subtree_control
		(nested_list_entry->az_list, menu_entry, count);
	    break;
        case sym_k_control_entry:
	    control_entry = (sym_control_entry_type *) list_member;
	    *count += 1;
	    *menu_entry = control_entry;
	    
	}

}



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine searches a callbacks list (and any nested lists)
**	for the create callback.
**
**  FORMAL PARAMETERS:
**
**      list_entry		the list to be (recursively) searched
**	create_entry		to return a pointer to the create entry
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
**--
**/

void extract_create_callback (list_entry, create_entry)
    sym_list_entry_type		*list_entry;
    sym_callback_entry_type	**create_entry;
    
{

/*
 * Local variables
 */
sym_obj_entry_type		*list_member;
sym_nested_list_entry_type	*nested_list_entry;
sym_callback_entry_type		*callback_entry;
sym_value_entry_type		*value_entry;
key_keytable_entry_type		*key_entry;


/*
 * Process the list elements, recursing on nested lists. Ignore error entries.
 */
if ( list_entry == NULL ) return;
for (list_member=(sym_obj_entry_type *)list_entry->obj_header.az_next;
     list_member!=NULL;
     list_member=(sym_obj_entry_type *)list_member->obj_header.az_next)
    switch ( list_member->header.b_tag )
	{
	case sym_k_nested_list_entry:
	    nested_list_entry = (sym_nested_list_entry_type *) list_member;
	    extract_create_callback
		(nested_list_entry->az_list, create_entry);
	    break;
	case sym_k_callback_entry:
	    callback_entry = (sym_callback_entry_type *) list_member;
	    value_entry = callback_entry->az_call_reason_name;
	    if (value_entry->obj_header.b_flags & sym_m_builtin)
		{
		key_entry = 
		    (key_keytable_entry_type *)value_entry->value.l_integer;
		if ( strcmp(uil_reason_toolkit_names[key_entry->b_subclass],
			    MrmNcreateCallback) == 0 )
		    {
		    *create_entry = callback_entry;
		    return;
		    }
		}
	    break;
	}

}



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine processes all valid callbacks in an callbacks list,
**	recursing on nested lists. Each valid callback is emitted to
**	the output.
**
**  FORMAL PARAMETERS:
**
**      list_entry		the list to be (recursively) processed
**	arglist_index		the modifiable index of arguments in the
**				output argslist.
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
**--
**/

void process_all_callbacks (list_entry, arglist_index)
    sym_list_entry_type		*list_entry;
    int				*arglist_index;
    
{

/*
 * Local variables
 */
sym_obj_entry_type		*list_member;
sym_nested_list_entry_type	*nested_list_entry;
sym_callback_entry_type		*callback_entry;


/*
 * Process the list elements, recursing on nested lists. Ignore error entries.
 */
if ( list_entry == NULL ) return;
for (list_member=(sym_obj_entry_type *)list_entry->obj_header.az_next;
     list_member!=NULL;
     list_member=(sym_obj_entry_type *)list_member->obj_header.az_next)
    switch ( list_member->header.b_tag )
	{
	case sym_k_nested_list_entry:
	    nested_list_entry = (sym_nested_list_entry_type *) list_member;
	    process_all_callbacks (nested_list_entry->az_list, arglist_index);
	    break;
	case sym_k_callback_entry:
	    callback_entry = (sym_callback_entry_type *) list_member;
	    emit_callback (callback_entry, arglist_index, FALSE);
	    break;
	case sym_k_error_entry:
	    break;
	default:
	    _assert (FALSE, "unknown entry in callback list");
	    break;
	}

}



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine processes all valid arguments in an arguments list,
**	recursing on nested lists. Each valid argument is emitted to
**	the output.
**
**  FORMAL PARAMETERS:
**
**      list_entry		the list to be (recursively) processed
**	arglist_index		the modifiable index of arguments in the
**				output argslist.
**	related_count		the modifiable count of related args
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
**--
**/

void process_all_arguments (list_entry, arglist_index, related_count)
    sym_list_entry_type		*list_entry;
    int				*arglist_index;
    int				*related_count;
    
{

/*
 * Local variables
 */
sym_obj_entry_type		*list_member;
sym_nested_list_entry_type	*nested_list_entry;
sym_argument_entry_type		*argument_entry;


/*
 * Process the list elements, recursing on nested lists. Ignore error entries.
 */
if ( list_entry == NULL ) return;
for (list_member=(sym_obj_entry_type *)list_entry->obj_header.az_next;
     list_member!=NULL;
     list_member=(sym_obj_entry_type *)list_member->obj_header.az_next)
    switch ( list_member->header.b_tag )
	{
	case sym_k_nested_list_entry:
	    nested_list_entry = (sym_nested_list_entry_type *) list_member;
	    process_all_arguments
		(nested_list_entry->az_list, arglist_index, related_count);
	    break;
	case sym_k_argument_entry:
	    argument_entry = (sym_argument_entry_type *) list_member;
	    emit_argument (argument_entry, *arglist_index, related_count);
	    *arglist_index -= 1;
	    break;
	case sym_k_error_entry:
	    break;
	default:
	    _assert (FALSE, "unknown entry in argument list");
	    break;
	}

}



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine processes all valid controls in an controls list,
**	recursing on nested lists. Each valid control is emitted to
**	the output.
**
**  FORMAL PARAMETERS:
**
**      list_entry		the list to be (recursively) processed
**	widget_index		the modifiable index of children
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
**--
**/

void process_all_controls (list_entry, widget_index)
    sym_list_entry_type		*list_entry;
    int				*widget_index;
    
{

/*
 * Local variables
 */
sym_obj_entry_type		*list_member;
sym_nested_list_entry_type	*nested_list_entry;
sym_control_entry_type		*control_entry;


/*
 * Process the list elements, recursing on nested lists. Ignore error entries.
 */
if ( list_entry == NULL ) return;
for (list_member=(sym_obj_entry_type *)list_entry->obj_header.az_next;
     list_member!=NULL;
     list_member=(sym_obj_entry_type *)list_member->obj_header.az_next)
    switch ( list_member->header.b_tag )
	{
	case sym_k_nested_list_entry:
	    nested_list_entry = (sym_nested_list_entry_type *) list_member;
	    process_all_controls (nested_list_entry->az_list, widget_index);
	    break;
	case sym_k_control_entry:
	    control_entry = (sym_control_entry_type *) list_member;
	    *widget_index -= 1;
	    emit_control (control_entry, *widget_index);
	    break;
	case sym_k_error_entry:
	    break;
	default:
	    _assert (FALSE, "unknown entry in control list");
	    break;
	}

}



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function builds the URM record for a value.
**
**  FORMAL PARAMETERS:
**
**      value_entry	    symbol table pointer for a value
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
**      write a value record to UID file
**
**--
**/

void	out_emit_value( value_entry )

sym_value_entry_type	*value_entry;

{
    MrmCode	    access;
    int		    value_size;
    MrmType	    value_type;
    int		    value_count;    
    char	    *buffer;
    status	    urm_status;
    XmString	    tmp_str;
    
    _assert( value_entry->header.b_tag == sym_k_value_entry,
	     "object to be emitted is not a value" );

    _assert( (value_entry->obj_header.b_flags & (sym_m_exported | sym_m_private)),
	     "value being emitted is not exported or private" );

    access = URMaPublic;

    if (value_entry->obj_header.b_flags & sym_m_private)
    {
	if (value_entry->resource_id == 0 )
	{
	    urm_status =
	    UrmIdbGetResourceId
		( out_az_idbfile_id, &(value_entry->resource_id) );
	    if( urm_status != MrmSUCCESS)
		issue_urm_error( "obtaining resource id" );
	}

	access = URMaPrivate;
    }

    /* 
    **	Case on the type of literal.
    */

    value_type = Urm_code_from_uil_type( value_entry->b_type );
    switch (value_entry->b_type)
	{
	case sym_k_bool_value:
	case sym_k_integer_value:
	    _assert(access == URMaPublic, 
		    "private value should not get resource ids");
	    value_size = sizeof(long);
	    break;

	case sym_k_horizontal_integer_value:
	case sym_k_vertical_integer_value:
	    value_size = sizeof(RGMUnitsInteger);
	    break;

	case sym_k_float_value:
	    _assert(access == URMaPublic, 
		    "private floats should not get resource ids");
	    value_size = sizeof (double);
	    break;

	case sym_k_horizontal_float_value:
	case sym_k_vertical_float_value:
	    value_size = sizeof(RGMUnitsFloat);
	    break;

	case sym_k_single_float_value:
	    _assert(access == URMaPublic,
		    "private single floats should not get resource ids");
	    value_size = sizeof(float);
	    break;

	case sym_k_compound_string_value:
	    tmp_str = value_entry->value.xms_value;
	    value_size = 
	      XmCvtXmStringToByteStream(tmp_str, 
				(unsigned char **)&(value_entry->value.c_value));
	    XmStringFree(tmp_str);
	    break;

	case sym_k_font_value:
	case sym_k_fontset_value:
	    {
	    /*
	     * Size is FontItem plus size of charset name string and
	     * size of font name string.
	     */
	     value_size = sizeof(RGMFontItem)
	       + strlen(sem_charset_name(value_entry->b_charset,
					 value_entry->az_charset_value)) 
		 + 1 + strlen(value_entry->value.c_value) + 1;
	    break;
	    }
	case sym_k_color_value:
	    /* null on end of color name accounted for in RGMColorDesc */
	    value_size = sizeof( RGMColorDesc ) + value_entry->w_length;
	    break;

	case sym_k_rgb_value:
	    {
	    sym_value_entry_type    *value_segment;

	    value_size = sizeof( RGMIntegerVector );
	    value_count = 0;
	    for (value_segment = value_entry->az_first_table_value;
		 value_segment != NULL;
		 value_segment =  value_segment->az_next_table_value)
		{
		value_size += sizeof(long);
		value_count++;
		}
	    break;
	    }

	case sym_k_color_table_value:
	    value_size = compute_color_table_size( value_entry );
	    break;

	case sym_k_icon_value:
	    value_size = compute_icon_size( value_entry );
	    break;

	case sym_k_char_8_value:
	case sym_k_reason_value:
	case sym_k_argument_value:
	case sym_k_class_rec_name_value:
	case sym_k_xbitmapfile_value:
	case sym_k_keysym_value:
	case sym_k_localized_string_value:
	    /* BEGIN OSF Fix CR 4859 */
/* END OSF Fix CR 4859 */
	    value_size = value_entry->w_length + 1;  /* +1 for the null */
	    break;

/* BEGIN OSF Fix CR 4859 */
	case sym_k_wchar_string_value:
	    value_size = sizeof(RGMWCharEntry) +
	      value_entry->az_first_table_value->w_length;  
	    break;
/* END OSF Fix CR 4859 */
	case sym_k_identifier_value:
	    value_size = value_entry->w_length;  /* includes the null */
	    break;

	case sym_k_string_table_value:
	    {
	    sym_value_entry_type	*value_segment;
	    
	    /* value_size accounts for header and null at end of the index */
	    value_size = sizeof( RGMTextVector ); 
	    value_count = 0;

	    /* 
	     **  Determine the size of the string table by adding together
	     **  the lengths of component strings.  Before the string is a
	     **  table of words.  The first word is the number of component
	     **  string.  Next comes the word offsets of each of the strings
	     **  from the start of the buffer.
	     */
	    for (value_segment=value_entry->az_first_table_value;
		 value_segment != NULL;  
		 value_segment = value_segment->az_next_table_value)
		{
		value_count++;

		value_size += 
		  XmCvtXmStringToByteStream(value_segment->value.xms_value, NULL) +
		    sizeof( RGMTextEntry );
		}
	    break;
	    }


	case sym_k_asciz_table_value:
	    {
	    sym_value_entry_type	*value_segment;

	    /* value_size accounts for header and null at end of the index */
	    value_size = sizeof( RGMTextVector ); 
	    value_count = 0;

	    /* 
	     **  Determine the size of the string table by adding together
	     **  the lengths of component strings.  Before the string is a
	     **  table of words.  The first word is the number of component
	     **  string.  Next comes the word offsets of each of the strings
	     **  from the start of the buffer.
	     */

	    for (value_segment=value_entry->az_first_table_value;
		 value_segment != NULL;  
		 value_segment = value_segment->az_next_table_value)
		{
		value_count++;
		value_size +=
		    value_segment->w_length + sizeof(RGMTextEntry) + 1;
		}
	    break;
	    }

	case sym_k_integer_table_value:
	    {
	    sym_value_entry_type	*value_segment;
	    
	    /*
	     **  The size needed for the vector is the size of the header
	     **  information followed by the the list of integers.  Add in
	     **  the header size here.
	     */
	    value_size = sizeof( RGMIntegerVector ); 
	    value_count = 0;

	    /* 
	     **  Determine the size of the integer table by adding together
	     **  the lengths of component integers to the header.
	     */

	    for (value_segment = value_entry->az_first_table_value;  
		 value_segment != NULL;  
		 value_segment = value_segment->az_next_table_value)
		{
		value_size += sizeof(long);
		value_count++;
		}
	    break;
	    }


	case sym_k_font_table_value:
	    {
	    sym_value_entry_type	*font_value;

	    /*
	     * Size is size of basic table, plus strings for all FontItems.
	     * We allocate one fewer FontItems than specified in the structure
	     * definition.
	     */
	    /* value_size accounts for header and null at end of the index */
	    value_size = sizeof(RGMFontList) - sizeof(RGMFontItem); 
	    value_count = 0;
	    
	    /* 
	     **  Determine the size of the font list by adding together
	     **  the lengths of component fonts.  Each component is a FontItem
	     **  in the list, plus space for the charset name and font name.
	     */
	    for (font_value = value_entry->az_first_table_value;  
		 font_value != NULL;  
		 font_value = font_value->az_next_table_value)
		{
		/* Fix for CR 5266 Part 2a -- Pull az_charset_value off of
		 *                    font_value, rather than value_entry.
		 */
		value_count += 1;
		value_size += sizeof(RGMFontItem)
		    + strlen(sem_charset_name(font_value->b_charset,
					      font_value->az_charset_value))
		      + 1 + strlen(font_value->value.c_value) + 1;
		}
	    break;
	    }

	case sym_k_trans_table_value:
	    {
	    sym_value_entry_type	*value_segment;
	    
	    value_size = 0;

	    /*
	     **  Determine the length of the translation table by adding
	     **  together the length of the component strings.
	     */
	    for (value_segment = value_entry->az_first_table_value;  
		 value_segment != NULL;  
		 value_segment = value_segment->az_next_table_value)
		value_size += value_segment->w_length + 1;
	    break;
	    }
	
	default:
	    _assert( FALSE, "unexpected value type" );
	}

    /*
    **	Check that the context is large enough to hold the value
    */

    if ((int)(UrmRCSize( out_az_context ) ) < value_size)
	{
	if( MrmSUCCESS != 
	   UrmResizeResourceContext( out_az_context, value_size ))
	    issue_urm_error( "allocating context" );
	urm_status = UrmResizeResourceContext( out_az_context, value_size );
	if ( urm_status != MrmSUCCESS)
	    {
	    if (urm_status == MrmTOO_MANY)
		{
		diag_issue_diagnostic
		    ( d_value_too_large,
		     (src_source_record_type *)value_entry->header.az_src_rec,
		     diag_k_no_column,
		     value_entry->obj_header.az_name->c_text );
		}
	    else
		issue_urm_error( "allocating context" );
	    }
	}
    
    
    /*
    **	Move the literal to the context.
    */

    UrmRCSetGroup( out_az_context, URMgLiteral );
    UrmRCSetType( out_az_context, value_type );
    UrmRCSetAccess( out_az_context, access );
    UrmRCSetLock( out_az_context, FALSE );
    UrmRCSetSize( out_az_context, value_size );

    buffer = (char *) UrmRCBuffer( out_az_context );

    bzero( buffer, value_size );

    switch (value_entry->b_type)
	{
	case sym_k_bool_value:
	case sym_k_integer_value:
	case sym_k_float_value:
	case sym_k_reason_value:
	case sym_k_argument_value:
	    _move( buffer, &value_entry->value.l_integer, value_size );
	    break;

	case sym_k_single_float_value:
	    _move( buffer, &value_entry->value.single_float, value_size);
	    break;

	case sym_k_char_8_value:
	case sym_k_localized_string_value:
	case sym_k_identifier_value:
	case sym_k_class_rec_name_value:
	case sym_k_xbitmapfile_value:
	case sym_k_keysym_value:
	case sym_k_compound_string_value:
	    _move( buffer, value_entry->value.c_value, value_size );
	    break;

/* BEGIN OSF Fix CR 4859 */
        case sym_k_wchar_string_value:
          {
            RGMWCharEntryPtr  wcharentry;
            
            wcharentry = (RGMWCharEntryPtr)buffer;
            
            wcharentry->wchar_item.count = value_size;
            
            _move(wcharentry->wchar_item.bytes,
                  value_entry->az_first_table_value->value.c_value, value_size);
            break;
          }
/* END OSF Fix CR 4859 */
          
	case sym_k_font_value:
	case sym_k_fontset_value:
	    {
	    RGMFontItemPtr	fontitem;
	    MrmOffset		textoffs;
	    char		*textptr;
	    char		*charset_name;
	    int			text_len;
	    
	    fontitem = (RGMFontItemPtr) buffer;
	    textoffs = (MrmOffset) sizeof(RGMFontItem);
	    textptr = (char *)fontitem+textoffs;
	    
	    charset_name = sem_charset_name (value_entry->b_charset, 
					     value_entry->az_charset_value);
	    text_len = strlen(charset_name) + 1;

	    fontitem->type = Urm_code_from_uil_type(value_entry->b_type);
	    fontitem->cset.cs_offs = textoffs;
	    strcpy (textptr, charset_name);
	    textoffs += text_len;
	    textptr += text_len;
	    fontitem->font.font_offs = textoffs;
	    strcpy (textptr, value_entry->value.c_value);
	    
	    break;
	    }

	case sym_k_color_value:
	    {
	    RGMColorDesc	*color_buffer;
	    
	    color_buffer = (RGMColorDesc *)buffer;
	    
	    switch (value_entry->b_arg_type)
		{
		case sym_k_unspecified_color:
		    color_buffer->mono_state = URMColorMonochromeUnspecified;
		    break;
		case sym_k_foreground_color:
		    color_buffer->mono_state = URMColorMonochromeForeground;
		    break;
		case sym_k_background_color:
		    color_buffer->mono_state = URMColorMonochromeBackground;
		    break;
		}

	    color_buffer->desc_type = URMColorDescTypeName;

	    _move( color_buffer->desc.name, 
		  value_entry->value.c_value, 
		  value_entry->w_length + 1 );
	    
	    break;
	    }

	case sym_k_rgb_value:
	    {
	    sym_value_entry_type	*value_segment;
	    RGMColorDesc 		*color_buffer;
	    int				color_vector[3];
	    int				index;

	    color_buffer = (RGMColorDesc *)buffer;
	    index = value_count;
	    
	    index--;
	    for (value_segment=value_entry->az_first_table_value;
		 value_segment != NULL;
		 value_segment = value_segment->az_next_table_value)
		{
		color_vector[index] = (long) value_segment->value.l_integer;
		index--;
		}
	    
	    color_buffer->desc_type = URMColorDescTypeRGB;
	    color_buffer->desc.rgb.red = color_vector[0];
	    color_buffer->desc.rgb.green = color_vector[1];
	    color_buffer->desc.rgb.blue = color_vector[2];
	    color_buffer->mono_state = URMColorMonochromeUnspecified;

	    break;
	    }


	case sym_k_color_table_value:
	    create_color_table( value_entry, buffer );
	    break;

	case sym_k_icon_value:
	    create_icon( value_entry, buffer );
	    break;

	case sym_k_string_table_value:
	    {
	    sym_value_entry_type	*value_segment;
	    int				text_offset;
	    int				index;
	    int				segment_size;
	    RGMTextVector		*string_vector;
	    
	    /*
	     **  Value entries are reversed.  Need to fill the buffer
	     **  from the end to front.
	     */
	    text_offset = value_size;
	    string_vector = (RGMTextVector *)buffer;
	    string_vector->validation = URMTextVectorValid;
	    string_vector->count = value_count;
	    index = value_count;
	    string_vector->item[index].pointer = NULL;
	    
	    for (value_segment=value_entry->az_first_table_value;
		 value_segment != NULL;  
		 value_segment = value_segment->az_next_table_value)
		{
		  tmp_str = value_segment->value.xms_value;
		  segment_size = 
		    XmCvtXmStringToByteStream(tmp_str, 
				      (unsigned char **)&(value_segment->value.c_value));
		  XmStringFree(tmp_str);
		  text_offset -= segment_size;
		  _move(&(buffer[text_offset]), 
			value_segment->value.c_value, segment_size);
		  index--;
		  string_vector->item[index].text_item.offset = text_offset;
		  string_vector->item[index].text_item.rep_type =
		    MrmRtypeCString;
		}
	    break;
	    }

	case sym_k_asciz_table_value:
	    {
	    sym_value_entry_type	*value_segment;
	    int				text_offset;
	    int				index;
	    int				segment_size;
	    RGMTextVector		*string_vector;
	    
	    /*
	     **  Value entries are reversed.  Need to fill the buffer
	     **  from the end to front.
	     */
	    text_offset = value_size;
	    string_vector = (RGMTextVector *)buffer;
	    string_vector->validation = URMTextVectorValid;
	    string_vector->count = value_count;
	    index = value_count;
	    string_vector->item[index].pointer = NULL;

	    for (value_segment=value_entry->az_first_table_value;
		 value_segment != NULL;  
		 value_segment = value_segment->az_next_table_value)
		{
		segment_size = value_segment->w_length + 1;
		buffer[text_offset-1] = '\0';
		text_offset -= segment_size;
		_move( &(buffer[text_offset]), 
		      value_segment->value.c_value, segment_size - 1);
		index--;
		string_vector->item[index].text_item.offset = text_offset;
		string_vector->item[index].text_item.rep_type = MrmRtypeChar8;
		}
	    break;
	    }

	case sym_k_integer_table_value:
	    {
	    sym_value_entry_type	*value_segment;
	    int				index;
	    RGMIntegerVector		*integer_vector;

	    /*
	     **  Fill in the header information
	     */
	    integer_vector = (RGMIntegerVector *)buffer;
	    integer_vector->validation = URMIntegerVectorValid;
	    integer_vector->count = value_count;

	    /*
	     **  Value entries are reversed.  Need to fill the buffer
	     **  from the end to front.
	     */
	    index = value_count - 1;
	    for (value_segment=value_entry->az_first_table_value;
		 value_segment != NULL;  
		 value_segment = value_segment->az_next_table_value)
		{
		integer_vector->item [index] =
		    (long) value_segment->value.l_integer;
		index--;
		}
	    break;
	    }

	case sym_k_font_table_value:
	    {
	    RGMFontListPtr		fontlist;
	    RGMFontItemPtr		fontitem;
	    sym_value_entry_type	*font_value;
	    int				textoffs;
	    char			*textptr;
	    int				index;
	    char			*charset_name;
	    int				text_len;
	    
	    /*
	     **  Font items are in correct order.
	     */
	    fontlist = (RGMFontList *)buffer;
	    fontlist->validation = URMFontListValid;
	    fontlist->count = value_count;

	    /*
	     **  textoffs need to be the offset just beyond the last
	     **  FontItem in the list. One FontItem is already allocated,
	     **  so account for it in sizing.
	     */
	    textoffs = sizeof (RGMFontList) +
		sizeof(RGMFontItem)*(value_count-1);
	    textptr = (char *)fontlist+textoffs;
	    
	    for (index = 0,
		 font_value = value_entry->az_first_table_value;  
		 font_value != NULL;  
		 index++, font_value = font_value->az_next_table_value)
		{
		fontitem = &fontlist->item[index];
                /*
                 * Fix for CR 5266 Part 2b -- Pull az_charset_value off of
                 *                    font_value, rather than value_entry.
                 */
		charset_name = 
		  sem_charset_name (font_value->b_charset, 
				    font_value->az_charset_value);

		fontitem->type = Urm_code_from_uil_type(font_value->b_type);
		fontitem->cset.cs_offs = textoffs;
		strcpy (textptr, charset_name);
		text_len = strlen(charset_name) + 1;
		textoffs += text_len;
		textptr += text_len;
		fontitem->font.font_offs = textoffs;
		strcpy (textptr, font_value->value.c_value);
		text_len = strlen(font_value->value.c_value) + 1;
		textoffs += text_len;
		textptr += text_len;
		}
	    break;
	    }

	case sym_k_trans_table_value:
	    {
	    sym_value_entry_type	*value_segment;
	    int				offset;
	    int				segment_size;
	    
	    /*
	     **  Value entries are reversed.  Need to fill the buffer
	     **  from the end to front.
	     */

	    offset = value_size;
	    for (value_segment = value_entry->az_first_table_value;  
		 value_segment != NULL;  
		 value_segment = value_segment->az_next_table_value)
		{
		buffer[offset - 1] = '\n';
		segment_size = value_segment->w_length + 1;
		offset -= segment_size;
		_move( &(buffer[offset]), 
		      value_segment->value.c_value, segment_size-1 );
		}
	    buffer[value_size - 1] = 0;
	    break;
	    }

	case sym_k_horizontal_integer_value:
	case sym_k_vertical_integer_value:
	    {
	    RGMUnitsIntegerPtr uiptr;

	    uiptr = (RGMUnitsIntegerPtr) buffer;
	    uiptr->value = value_entry->value.l_integer;
	    uiptr->units = value_entry->b_arg_type;
	    break;
	    }

	case sym_k_horizontal_float_value:
	case sym_k_vertical_float_value:
	    {
	    RGMUnitsFloatPtr ufptr;

	    ufptr = (RGMUnitsFloatPtr) buffer;
	    *((double *)(&ufptr->value[0])) = value_entry->value.d_real;
	    ufptr->units = value_entry->b_arg_type;
	    break;
	    }
	}

    /*
    **	Output the literal
    */
    if (access == URMaPublic)
        urm_status = UrmPutIndexedLiteral
	    (out_az_idbfile_id,
	     value_entry->obj_header.az_name->c_text,
	     out_az_context);
    else
        urm_status = UrmPutRIDLiteral
	    (out_az_idbfile_id, value_entry->resource_id, out_az_context);

    if( urm_status != MrmSUCCESS)
	{
	if (urm_status == MrmEOF)
	    diag_issue_diagnostic ( d_uid_write, diag_k_no_source, 
				    diag_k_no_column, Uil_current_file );
	else
	    issue_urm_error( "emitting literal" );
	}
    
    if (Uil_cmd_z_command.v_show_machine_code)
	save_value_machine_code (value_entry, out_az_context);
    
    value_entry->output_state = sym_k_emitted;
    
    }


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function builds the callback argument for a widget record
**
**  FORMAL PARAMETERS:
**
**      callback_entry	    symbol table pointer for the callback
**	arglist_index	    index in arglist to place callback
**			    (this is an in/out argument)
**	emit_create	    true: emit a create reason
**			    false: skip create reason
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
**      callback data is added to out_az_context
**
**--
**/

void	emit_callback

	(sym_callback_entry_type *callback_entry,
	int *arglist_index,
	boolean emit_create)

{
sym_value_entry_type		*reason_entry;
sym_proc_ref_entry_type		*proc_ref_entry_next;
int				proc_ref_index;
int				proc_count;
boolean				qcreate = FALSE;
MrmOffset			callback_offset;
status				urm_status;

/*
 * Count number of entries in callback list
 */
if ( callback_entry->az_call_proc_ref != 0 )
    proc_count = 1;
else
    proc_count = count_proc (callback_entry->az_call_proc_ref_list, 0);

/*
** Reasons can take several forms:
**    1) create reason: value is builtin - keyword Urm value is "created"
**	this need special handling
**    2) builtin reasons: value is builtin - keyword Urm value as URM
**	compressed equivalent argument
**    3) non-builtin private: value is not builtin but private -
**	use an uncompressed argument tag
**    4) non-builtin public: value is not builtin and public -
**	not supported yet
*/

reason_entry = callback_entry->az_call_reason_name;
if ( reason_entry->obj_header.b_flags & sym_m_builtin )
    {
    key_keytable_entry_type *key_entry;
    
    key_entry = (key_keytable_entry_type *) reason_entry->value.l_integer;
    qcreate =
	(strcmp(uil_reason_toolkit_names[key_entry->b_subclass],
		MrmNcreateCallback) == 0);
    if ( qcreate )
	{
	/*
	 **	case 1: create reason - return if we are not to emit
	 **	otherwise use special routine to describe
	 */
	if ( !emit_create ) return;
	urm_status = UrmCWRSetCreationCallback
	    (out_az_context,
	     proc_count,
	     &callback_offset);
	if( urm_status != MrmSUCCESS)
	    {
	    if (urm_status == MrmEOF)
		diag_issue_diagnostic ( d_uid_write, diag_k_no_source, 
					diag_k_no_column, Uil_current_file );
	    else
		issue_urm_error ("emitting creation callback");
	    }

	}
    else
	{
	/*
	 **	case 2: builtin case - use a compressed argument
	 */
	urm_status = UrmCWRSetCompressedArgTag
	    (out_az_context,
	     *arglist_index,
	     uil_reas_compr[key_entry->b_subclass],
	     0);	
	    if( urm_status != MrmSUCCESS)
		issue_urm_error( "setting compressed arg" );
	}
    }
else
    {
    /*
     **  Non private reasons and arguments are not supported
     */
    if ( reason_entry->obj_header.b_flags & (sym_m_imported|sym_m_exported) )
	{
	diag_issue_diagnostic
	    (d_not_impl, diag_k_no_source, diag_k_no_column,
	     "EXPORTED and IMPORTED arguments and reasons" );
	return;
	}
    
    /*
     **  case 3: private, non-builtin case - use an uncompressed argument
     */
    urm_status = UrmCWRSetUncompressedArgTag
	(out_az_context,
	 *arglist_index,
	 reason_entry->value.c_value);
    if( urm_status != MrmSUCCESS)
	issue_urm_error( "setting uncompressed arg" );
    }

/*
 * Create the callback value (this is not done for the create callback)
 */
if ( ! qcreate )
    {
    urm_status = UrmCWRSetArgCallback
	(out_az_context, *arglist_index, proc_count, &callback_offset);
    if( urm_status != MrmSUCCESS)
	issue_urm_error ("setting callback arg");
    }

/*
 * Create the callback procedures
 */
if (callback_entry->az_call_proc_ref != 0)
    {
    proc_ref_index = 0;
    proc_ref_entry_next = callback_entry->az_call_proc_ref;
    }
else
    {
    proc_ref_index = proc_count - 1;
    proc_ref_entry_next = 
	(sym_proc_ref_entry_type *) callback_entry->
	    az_call_proc_ref_list->obj_header.az_next;
    }
emit_callback_procedures
    (proc_ref_entry_next, &proc_ref_index, callback_offset);
*arglist_index = *arglist_index - 1;

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function counts the number of procedures in a procedure list
**	including nested procedure lists.
**
**  FORMAL PARAMETERS:
**
**      proc_list	    list of procedures (and nested list entries)
**	count		    count of procedures encountered so far
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
**      count of procedures in procedure list
**
**  SIDE EFFECTS:
**
**      none
**
**--
**/

int	count_proc(proc_list, count)
sym_list_entry_type	*proc_list;
int			count;

{
    sym_obj_entry_type		*proc_list_next;

    for (proc_list_next = (sym_obj_entry_type *)proc_list->obj_header.az_next;
			    proc_list_next != 0;
			    proc_list_next = (sym_obj_entry_type *)
				proc_list_next->obj_header.az_next)
	{
	switch (proc_list_next->header.b_tag)
	    {
	    case sym_k_nested_list_entry:
		count = count_proc(((sym_nested_list_entry_type *)
			proc_list_next)->
		        az_list, 
			count);
		break;
	    case sym_k_proc_ref_entry:
		count++;
		break;
	    default:
		_assert(FALSE, "unknown entry in procedures list");
	    }
	}
	return (count);
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function output a procedure list referenced by a callback entry
**
**  FORMAL PARAMETERS:
**
**	proc_ref_entry_next next procedure reference entry
**	proc_ref_index	    index of procedure in procedure list (for Mrm)
**	callback_offset	    offset of callback (for Mrm)
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

/*
 * Fix for CR 4772 - Change the proc_ref_index entry from an integer to an 
 *                   integer pointer to allow for proper construction of 
 *                   internal callback arrays.
 */
void	emit_callback_procedures

	(sym_proc_ref_entry_type *proc_ref_entry_next,
	int *proc_ref_index,
	MrmOffset callback_offset)

{
    sym_proc_def_entry_type *proc_def_entry;
    sym_value_entry_type    *proc_arg_entry;
    MrmCode		    arg_access;
    MrmCode		    arg_form;
    char		    *arg_index;
    MrmResource_id	    arg_id;
    MrmCode		    arg_type, arg_group;
    long		    arg_value;
    sym_nested_list_entry_type	*nested_proc_list_entry;
    sym_list_entry_type	    *proc_list_entry;
    status		    urm_status;

    for (
        ;
        proc_ref_entry_next != 0;
        proc_ref_entry_next = 
            (sym_proc_ref_entry_type *) proc_ref_entry_next->
            obj_header.az_next)

        {
	switch (proc_ref_entry_next->header.b_tag)
	    {
	    case sym_k_nested_list_entry:
		nested_proc_list_entry = (sym_nested_list_entry_type *)
		    proc_ref_entry_next;
		proc_list_entry = nested_proc_list_entry->az_list;
		emit_callback_procedures (( sym_proc_ref_entry_type *)proc_list_entry->obj_header.az_next, 
		    proc_ref_index,		
		    callback_offset);
		break;
	    case sym_k_proc_ref_entry:
                proc_def_entry = proc_ref_entry_next->az_proc_def;
                proc_arg_entry = proc_ref_entry_next->az_arg_value;

                if (proc_arg_entry == NULL)
                    {
        	        arg_type = MrmRtypeNull;
        	        arg_value = 0L;
        	        arg_form = URMrImmediate;
                    }
                else
                    {
        	        arg_form = ref_value
        		            ( proc_arg_entry, 
        		              &arg_type, &arg_value, &arg_access, &arg_index,
        		              &arg_id, &arg_group );
                    }
    
                if (arg_form == URMrImmediate)
                    urm_status =
    	        UrmCWRSetCallbackItem
    	            ( out_az_context, callback_offset, *proc_ref_index,
    	              proc_def_entry->obj_header.az_name->c_text,
    	              arg_type,
    	              arg_value );
                else
                    urm_status =
    	        UrmCWRSetCallbackItemRes
    	            ( out_az_context, callback_offset, *proc_ref_index,
    	              proc_def_entry->obj_header.az_name->c_text,
    	              arg_group,
    	              arg_access,
    	              arg_type,
    	              arg_form,
    	              arg_index,
    	              arg_id );
    
                if( urm_status != MrmSUCCESS)
		    issue_urm_error( "setting callback proc" );
        	*proc_ref_index = *proc_ref_index - 1;
                break;

	    case sym_k_error_entry:
		break;
	    default:
		_assert (FALSE, "unknown entry in procedures list");
		break;
	    }
        }
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function builds an argument for a widget record
**
**  FORMAL PARAMETERS:
**
**      argument_entry	    symbol table pointer for the argument
**	arglist_index	    index in arglist to place argument
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
**      argument data is added to out_az_context
**
**--
**/

void	emit_argument( argument_entry, arglist_index, related_arg_count )

sym_argument_entry_type	*argument_entry;
int			arglist_index;
int			*related_arg_count;

{
sym_value_entry_type		*arg_name_entry;
sym_value_entry_type		*arg_value_entry;
MrmCode				arg_access;
MrmCode				arg_form;
char				*arg_index;
MrmResource_id			arg_id;
MrmCode				arg_type, arg_group;
long				arg_value;
unsigned char			expected_type;
status				urm_status;


/*
 *	For an argument, we must:
 *	    1) create the argument 
 *	    2) create the argument value
 */

arg_name_entry = argument_entry->az_arg_name;
if (arg_name_entry->obj_header.b_flags & sym_m_builtin)
    {
    key_keytable_entry_type *key_entry;
    
    key_entry = (key_keytable_entry_type *)arg_name_entry->value.l_integer;
    
    urm_status = UrmCWRSetCompressedArgTag
	(out_az_context,
	 arglist_index,
	 uil_arg_compr[key_entry->b_subclass],
	 uil_arg_compr[related_argument_table[key_entry->b_subclass]]); 
    if ( related_argument_table[key_entry->b_subclass] != 0 )
	*related_arg_count += 1;
    if( urm_status != MrmSUCCESS)
	issue_urm_error( "setting compressed arg" );
    }
else
    {
    /*
     **  Non private reasons and arguments are not supported
     */
    
    if ( arg_name_entry->obj_header.b_flags & (sym_m_imported|sym_m_exported) )
	{
	diag_issue_diagnostic
	    (d_not_impl, diag_k_no_source, diag_k_no_column,
	     "EXPORTED and IMPORTED arguments and reasons" );
	    return;
	}

    /*
     **  case 3: private, non-builtin case - use an uncompressed argument
     */
	urm_status = UrmCWRSetUncompressedArgTag
	    (out_az_context,
	     arglist_index,
	     arg_name_entry->value.c_value);
    if( urm_status != MrmSUCCESS)
	issue_urm_error( "setting uncompressed arg" );
    }

/*
 * Acquire the argument parameters. If it is an immediate value, set it.
 * Else set it up as a literal or widget reference. For literal references,
 * the expected type must be set up in order to enable coercion in Mrm.
 */
arg_value_entry = argument_entry->az_arg_value;
arg_form = ref_value
    (arg_value_entry,
     &arg_type,
     &arg_value,
     &arg_access,
     &arg_index,
     &arg_id,
     &arg_group);
if (arg_form == URMrImmediate)
    urm_status = UrmCWRSetArgValue
	(out_az_context,
	 arglist_index,
	 arg_type,
	 arg_value );
else
    {
    switch ( arg_group )
	{
	case URMgLiteral:
	    if ( argument_entry->az_arg_name->obj_header.b_flags &
		sym_m_builtin )
		{
		key_keytable_entry_type		* keytable_entry;
	    
		keytable_entry = (key_keytable_entry_type *)
		    argument_entry->az_arg_name->value.l_integer;
		_assert (keytable_entry->b_class == tkn_k_class_argument,
			 "name is not an argument");
		expected_type =
		    argument_type_table[keytable_entry->b_subclass];
		}
	    else
		expected_type = argument_entry->az_arg_name->b_arg_type;
	    urm_status = UrmCWRSetArgResourceRef
		(out_az_context,
		 arglist_index,
		 arg_access,
		 arg_group,
		 Urm_code_from_uil_type(expected_type),
		 arg_form,
		 arg_index,
		 arg_id );
	    break;
	case URMgWidget:
	    urm_status = UrmCWRSetArgResourceRef
		(out_az_context,
		 arglist_index,
		 arg_access,
		 arg_group,
		 RGMwrTypeReference,
		 arg_form,
		 arg_index,
		 arg_id );
	    break;
	}
    }
    
if( urm_status != MrmSUCCESS)
    issue_urm_error ("setting arg value");

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function builds a control for a widget record
**
**  FORMAL PARAMETERS:
**
**      control_entry	    symbol table pointer for the control
**	control_offset	    offset of object in the control list
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
**      control data is added to out_az_context
**
**--
**/

void	emit_control( control_entry, control_offset )

sym_control_entry_type	*control_entry;
int			control_offset;

{
    MrmCode		    access;
    MrmCode		    form;
    char		    *index;
    MrmResource_id	    id;
    status		    urm_status;
    sym_widget_entry_type   *widget_entry;
    Boolean		    managed;

    /*
    **  1) Process the object reference
    **  2) set object as a child of current context
    */

    form = ref_control( control_entry, &access, &index, &id );

    /* Truly gross hack.  Fix in WML before beta */
#ifndef sym_k_XmRenderTable_object
#define sym_k_XmRenderTable_object 0
#endif
#ifndef sym_k_XmRendition_object
#define sym_k_XmRendition_object 0
#endif
#ifndef sym_k_XmTabList_object
#define sym_k_XmTabList_object 0
#endif
    widget_entry = control_entry->az_con_obj;

    while (widget_entry->obj_header.az_reference != NULL)
      widget_entry = 
	(sym_widget_entry_type *)widget_entry->obj_header.az_reference;

    managed = ((widget_entry->header.b_type != sym_k_XmRenderTable_object) &&
	       (widget_entry->header.b_type != sym_k_XmRendition_object) &&
	       (widget_entry->header.b_type != sym_k_XmTabList_object) &&
	       ((control_entry->obj_header.b_flags & sym_m_managed) != 0));
    
    /*
    **  Add the object as a child.
    */

    urm_status =
    UrmCWRSetChild
	( out_az_context,
	  control_offset,
	  managed,
	  access,
	  form,
	  index,
	  id );
    
    if( urm_status != MrmSUCCESS)
	issue_urm_error( "setting child" );

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function process a reference to a value.
**
**  FORMAL PARAMETERS:
**
**      arg_value_entry	    (in)  value entry to process
**	arg_type	    (out) URM argument type
**	arg_value	    (out) argument value if immediate
**	arg_access	    (out) private or public
**	arg_index	    (out) index if value is an index
**	arg_id		    (out) resource id if value is a resource id
**	arg_group	    (out) URM group (widget or literal)
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
**      URMrIndex, URMrRID, URMrImmediate (class of value)
**	defines which of the output parameters have meaning
**
**  SIDE EFFECTS:
**
**      none
**
**--
**/

MrmCode	ref_value(value_entry, 
		  arg_type, arg_value, arg_access, arg_index, arg_id, arg_group)

sym_value_entry_type	*value_entry;
MrmCode			*arg_type;
long			*arg_value;
MrmCode			*arg_access;
char			**arg_index;
MrmResource_id		*arg_id;
MrmCode			*arg_group;

{

    status	urm_status;

    *arg_value = 0L;
    *arg_index = NULL;
    *arg_id = 0;
    *arg_group = URMgLiteral;

    /*  This value may actually be a widget reference, so check for this
        case first.    */

    if (value_entry->header.b_tag == sym_k_widget_entry)
	{
	
	/*  Set up a dummy control entry, and process the widget reference.  */

	sym_control_entry_type	control_entry;
	sym_widget_entry_type	* widget_entry;
	
	widget_entry = (sym_widget_entry_type *)value_entry;
	control_entry.header.b_tag = sym_k_control_entry;
	control_entry.az_con_obj = widget_entry;
	
	*arg_group = URMgWidget;
	*arg_type = RGMwrTypeReference;
	
	return ref_control (&control_entry, arg_access, arg_index, arg_id);
	}

    *arg_type = Urm_code_from_uil_type( value_entry->b_type );

    if (value_entry->obj_header.b_flags & sym_m_private)
	{
	*arg_access = URMaPrivate;

	switch (value_entry->b_type)
	    {
	    case sym_k_bool_value:
	    case sym_k_integer_value:
	        *arg_value = value_entry->value.l_integer;
		return URMrImmediate;

	    case sym_k_float_value:
		*arg_value = (long)(&(value_entry->value.d_real));
		return URMrImmediate;

	    case sym_k_single_float_value:
		*arg_value = (long)(value_entry->value.single_float);
		return URMrImmediate;

	    case sym_k_char_8_value:
	    case sym_k_font_value:
	    case sym_k_fontset_value:
	    case sym_k_color_value:
	    case sym_k_reason_value:
	    case sym_k_argument_value:
	    case sym_k_trans_table_value:
	    case sym_k_asciz_table_value:
	    case sym_k_integer_table_value:
	    case sym_k_string_table_value:
	    case sym_k_color_table_value:
	    case sym_k_icon_value:
	    case sym_k_font_table_value:
	    case sym_k_compound_string_value:
	    case sym_k_identifier_value:
	    case sym_k_class_rec_name_value:
	    case sym_k_xbitmapfile_value:
	    case sym_k_keysym_value:
	    case sym_k_rgb_value:
	    case sym_k_wchar_string_value:
	    case sym_k_localized_string_value:
 	    case sym_k_horizontal_integer_value:
 	    case sym_k_vertical_integer_value:
 	    case sym_k_horizontal_float_value:
 	    case sym_k_vertical_float_value:
		if (value_entry->resource_id == 0 )
		    {
		    urm_status =
			UrmIdbGetResourceId
			    (out_az_idbfile_id, &(value_entry->resource_id) );
		    if ( urm_status != MrmSUCCESS )
			issue_urm_error( "obtaining resource id" );
		    }

		if (value_entry->output_state == sym_k_not_processed)
		    {
		    value_entry->output_state = sym_k_queued;
		    push((sym_entry_type *) value_entry );
		    }
		
		*arg_id = value_entry->resource_id;
		return URMrRID;
		
	    default:
		_assert( FALSE, "unexpected value type" );
		return URMrImmediate;
	    }
	
	}

    /*
    **	Only Imported and Exported Values reach this point
    */

    *arg_access = URMaPublic;
    *arg_index = (char *)(value_entry->obj_header.az_name->c_text);

    if ((value_entry->obj_header.b_flags & sym_m_exported) &&
	(value_entry->output_state == sym_k_not_processed))
	{
	value_entry->output_state = sym_k_queued;
	push((sym_entry_type *) value_entry );
	}

    return URMrIndex;

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function process a reference to a control (widget or gadget)
**
**  FORMAL PARAMETERS:
**
**      control_entry	    (in)  control entry for widget reference
**	access		    (out) private or public
**	index		    (out) index if widget is an index
**	id		    (out) resource id if widget is a resource id
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
**      URMrIndex, URMrRID (class of control)
**	defines which of the output parameters have meaning
**
**  SIDE EFFECTS:
**
**      none
**
**--
**/

MrmCode	ref_control(control_entry, access, index, id)

sym_control_entry_type	*control_entry;
MrmCode			*access;
char			**index;
MrmResource_id		*id;

{

    sym_widget_entry_type   *widget_entry;
    MrmCode		    form;
    status		    urm_status;

    _assert( control_entry->header.b_tag == sym_k_control_entry,
	     "expecting a control entry" );

    /*
    **	For a control, we must:
    **	    1) determine if this is a definition or a reference
    **	    2) queue the widget to be created if it isn't yet
    **	    3) get a resource id for the control if unnamed
    */

    widget_entry = control_entry->az_con_obj;

    /*
    **	If the reference field is set, this is a reference to a control
    **	defined elsewhere.  Otherwise it is an inline definition.
    */

    while (widget_entry->obj_header.az_reference != NULL)
	widget_entry = 
	    (sym_widget_entry_type *)widget_entry->obj_header.az_reference;

    /*
    **	Queue the control to be processed if it has not already been
    **	emitted or queued for processing.
    */

    if ((widget_entry->obj_header.b_flags & (sym_m_exported | sym_m_private))
	&& 
	(widget_entry->output_state == sym_k_not_processed)
       )
    {
	widget_entry->output_state = sym_k_queued;

	push((sym_entry_type *) widget_entry );
    }

    if (widget_entry->obj_header.az_name == NULL)
    {
	/*
	**  Need a resource id.
	*/

	if (widget_entry->resource_id == 0 )
	{
	    urm_status =
	    UrmIdbGetResourceId
		( out_az_idbfile_id, &(widget_entry->resource_id) );
	    if( urm_status != MrmSUCCESS)
	    	issue_urm_error( "obtaining resource id" );
	}

        form = URMrRID;
	*id = widget_entry->resource_id;
	*index = NULL;
    }
    else
    {
	/*
	**  Need an index
	*/

        form = URMrIndex;
	*index = widget_entry->obj_header.az_name->c_text;
	*id = 0 ;
    }

    *access = URMaPublic;

    if (widget_entry->obj_header.b_flags & sym_m_private)
        *access = URMaPrivate;

    return form;

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function issue a diagnostic for an error detected by URM.
**
**  FORMAL PARAMETERS:
**
**      problem		string indicating what p2_output was trying to do
**
**  IMPLICIT INPUTS:
**
**      out_az_context	context in error (hold further info about error)
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
**      diagnostic is issued - compilation stops
**
**--
**/

void	issue_urm_error( problem )

char	*problem;

{
    char    buffer[132];

    sprintf(buffer, "while %s encountered %s", 
	    problem, 
	    Urm__UT_LatestErrorMessage());

    diag_issue_internal_error( buffer );
}

/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This procedure maps uil literal type to Urm equivalent types
 *
 *  FORMAL PARAMETERS:
 *
 *	uil_type 	 uil types sym_k_..._value
 *
 *  IMPLICIT INPUTS:
 *
 *      none
 *
 *  IMPLICIT OUTPUTS:
 *
 *      none
 *
 *  FUNCTION VALUE:
 *
 *	corresponding RGMrType... code
 *
 *  SIDE EFFECTS:
 *
 *      none
 *
 *--
 */

MrmCode Urm_code_from_uil_type( uil_type )

int	uil_type;

{

    switch (uil_type)
    {
    case sym_k_integer_value:		    return MrmRtypeInteger;
    case sym_k_horizontal_integer_value:    return MrmRtypeHorizontalInteger;
    case sym_k_vertical_integer_value:	    return MrmRtypeVerticalInteger;
    case sym_k_bool_value:		    return MrmRtypeBoolean;
    case sym_k_char_8_value:		    return MrmRtypeChar8;
    case sym_k_localized_string_value:	    return MrmRtypeChar8;
    case sym_k_argument_value:		    return MrmRtypeChar8;
    case sym_k_reason_value:		    return MrmRtypeChar8;
    case sym_k_trans_table_value:	    return MrmRtypeTransTable;
    case sym_k_asciz_table_value:	    return MrmRtypeChar8Vector;
    case sym_k_string_table_value:	    return MrmRtypeCStringVector;
    case sym_k_compound_string_value:	    return MrmRtypeCString;
    case sym_k_wchar_string_value:	    return MrmRtypeWideCharacter; 
    case sym_k_integer_table_value:	    return MrmRtypeIntegerVector;
    case sym_k_color_value:		    return MrmRtypeColor;
    case sym_k_color_table_value:	    return MrmRtypeColorTable;
    case sym_k_icon_value:		    return MrmRtypeIconImage;
    case sym_k_float_value:		    return MrmRtypeFloat;
    case sym_k_horizontal_float_value:	    return MrmRtypeHorizontalFloat;
    case sym_k_vertical_float_value:	    return MrmRtypeVerticalFloat;
    case sym_k_font_value:		    return MrmRtypeFont;
    case sym_k_fontset_value:		    return MrmRtypeFontSet;
    case sym_k_font_table_value:	    return MrmRtypeFontList;
    case sym_k_identifier_value:	    return MrmRtypeAddrName;
    case sym_k_class_rec_name_value:        return MrmRtypeClassRecName;
    case sym_k_xbitmapfile_value:	    return MrmRtypeXBitmapFile;
    case sym_k_widget_ref_value:	    return MrmRtypeAny;
    case sym_k_pixmap_value:		    return MrmRtypeIconImage;
    case sym_k_any_value:		    return MrmRtypeAny;
    case sym_k_keysym_value:                return MrmRtypeKeysym;    
    case sym_k_single_float_value:          return MrmRtypeSingleFloat;
    case sym_k_rgb_value:                   return MrmRtypeColor;

    default:
	_assert( FALSE, "unknown value type" );
	return 0;
    }

}



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function computes the size of a color table.
**
**  FORMAL PARAMETERS:
**
**      table_entry	    value entry for the color table
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
**      size of the color table
**
**  SIDE EFFECTS:
**
**      colors within the color table may be allocated
**
**--
**/

int	compute_color_table_size(table_entry)

sym_value_entry_type	*table_entry;

{
    sym_value_entry_type    *color_entry;
    int			    size;
    int			    i;
    MrmCode		    arg_type;
    long		    arg_value;
    MrmCode		    arg_access;
    char		    *arg_index;
    MrmResource_id	    arg_id;
    MrmCode		    arg_group;

    /*
    **	Compute the size of the ColorTable plus the size of the Color
    **	table entries (one per color including fore and back ground).
    **	Multiply entry size by max_index rather than max_index + 1
    **	since RGMColorTable includes 1 entry. Note that descriptors
    **  are sized to consume fullword-aligned blocks of memory in
    **  to preserve alignment for processors requiring such alignment.
    */

    size = sizeof(RGMColorTable) +
	sizeof(RGMColorTableEntry)*table_entry->b_max_index;
    size = _FULLWORD (size);

    /*
    **	Compute space needed for resource descriptors for the colors.
    */

    for (i = 0;  i < (int)table_entry->b_table_count;  i++)
    {
	color_entry = table_entry->value.z_color[i].az_color;

	/*
	**  Default colors have az_color set to 0=back and 1=fore.
	**  These require ColorTableEntries but no resource descriptors.
	*/

	if ((long)color_entry > 1)
	{
	    /*
	    **	Call ref_value for each of the colors in the color table.
	    **	This will cause them to be created if necessary and also
	    **	classify the type of resource needed.
	    */

	    table_entry->value.z_color[i].w_desc_offset = size;

	    switch (ref_value( color_entry,
			       & arg_type, & arg_value, & arg_access,
			       & arg_index, & arg_id, & arg_group ) )
	    {
	    case URMrRID:
		size += sizeof (RGMResourceDesc);
		size = _FULLWORD (size);
		break;
	    case URMrIndex:
		size += sizeof(RGMResourceDesc) - sizeof(MrmResource_id) +
			strlen(arg_index)+1;
		size = _FULLWORD (size);
		break;
	    default:
		_assert( FALSE, "immediate color values not supported" );
	    }
	}
    }

    table_entry->w_length = size;

    return size;

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function creates a color table in a context.
**
**  FORMAL PARAMETERS:
**
**      table_entry	    value entry for the color table
**      buffer		    pointer to a context buffer
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

void	create_color_table(table_entry, buffer)

sym_value_entry_type	*table_entry;
char			*buffer;

{
    RGMColorTable	    *table;
    RGMColorTableEntry	    *item;
    RGMResourceDesc	    *desc;
    int			    i;
    MrmCode		    arg_form;
    MrmCode		    arg_type;
    long		    arg_value;
    MrmCode		    arg_access;
    char		    *arg_index;
    MrmResource_id	    arg_id;
    MrmCode		    arg_group;

    /*
    **	Fill in the Color Table fields
    */

    table = (RGMColorTable *)buffer;

    table->validation = URMColorTableValid;
    table->count = table_entry->b_max_index + 1;

    /*
    **	Loop thru the colors in the table setting up both the index
    **	of offset for the colors and their resource descriptors.
    */

    item = table->item;

    for (i = 0;  i < (int)table_entry->b_table_count;  i++)
    {
	int	index;

	index = table_entry->value.z_color[i].b_index;
	table->item[index].color_item.coffs = 
	    table_entry->value.z_color[i].w_desc_offset;
	desc = (RGMResourceDesc *)
		    (buffer + table_entry->value.z_color[i].w_desc_offset);

	/*
	**  Default colors have b_index set to 0=back and 1=fore.
	**  These require ColorTableEntries but no resource descriptors.
	*/

	if (index > 1)
	{
	    table->item[index].type = MrmRtypeResource;

	    /*
	    **	Call ref_value for each of the colors in the color table.
	    **	This provide the necessary info to fill in the resource
	    **	descriptor
	    */

	    arg_form = ref_value( table_entry->value.z_color[i].az_color,
			          & arg_type, & arg_value, & arg_access,
				  & arg_index, & arg_id, & arg_group );

	    desc->access = arg_access;
	    desc->type = arg_form;
	    desc->res_group = arg_group;
	    desc->cvt_type = arg_type;

	    switch (arg_form)
	    {
	    case URMrRID:
		desc->size = sizeof( RGMResourceDesc );
		desc->key.id = arg_id;
		break;
	    case URMrIndex:
		desc->size = strlen( arg_index ) + 1;
		_move( desc->key.index, arg_index, desc->size );
		desc->size += sizeof( RGMResourceDesc ) - 
			      sizeof( MrmResource_id );
		break;
	    default:
		_assert( FALSE, "immediate color values not supported" );
	    }
	}
    }

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function computes the size of an icon.
**
**  FORMAL PARAMETERS:
**
**      icon_entry	    value entry for the icon
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
**      size of the icon
**
**  SIDE EFFECTS:
**
**      color table within the icon may be allocated
**
**--
**/

int	compute_icon_size(icon_entry)

sym_value_entry_type	*icon_entry;

{
    int			    size;
    int			    pixel_type;
    MrmCode		    arg_type;
    long		    arg_value;
    MrmCode		    arg_access;
    char		    *arg_index;
    MrmResource_id	    arg_id;
    MrmCode		    arg_group;

    /*
    **	The size of the icon consist of the size of the RGMIconImage
    **	structure + the size of the color table resource descriptor
    **	+ the actual data.
    */

    size = sizeof( RGMIconImage );

    /*
    **	Compute space needed for the color table resource descriptor
    **
    **	Call ref_value.
    **	This will cause the table to be created if necessary and also
    **	classify the type of resource needed.
    */

    switch (ref_value( icon_entry->value.z_icon->az_color_table,
		       & arg_type, & arg_value, & arg_access,
		       & arg_index, & arg_id, & arg_group ) )
    {
    case URMrRID:
	size += sizeof( RGMResourceDesc );
	break;
    case URMrIndex:
	size += sizeof( RGMResourceDesc ) - sizeof( MrmResource_id ) +
		strlen( arg_index ) + 1;
	break;
    default:
	_assert( FALSE, "immediate color table values not supported" );
    }

    /*
    **	Save the offset of the data for later.
    */

    icon_entry->b_data_offset = size;

    /*
    **	Bits per pixel is based on the number of colors used.
    **	Pixel_type:
    **		0	for 1 bit pixels
    **		1	for 2 bit pixels
    **		2	for 4 bit pixels
    **		3	for 8 bit pixels
    **	URM's pixels size encoding is pixel_type + 1
    **	Pixel_size = 1 << pixel_type
    */

    pixel_type = icon_entry->value.z_icon->az_color_table->b_max_index;

    if (pixel_type < 2)
	pixel_type = 0;
    else if (pixel_type < 4)
	pixel_type = 1;
    else if (pixel_type < 16)
	pixel_type = 2;
    else
	pixel_type = 3;

    icon_entry->b_pixel_type = pixel_type;

    /*
    **	Size is width * height - each row must be an even number of bytes
    */

    size += ((int)((icon_entry->value.z_icon->w_width << pixel_type) + 7) / 8)
	     * icon_entry->value.z_icon->w_height;

    icon_entry->w_length = size;

    return size;

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function creates an icon in a context buffer
**
**  FORMAL PARAMETERS:
**
**      icon_entry	    value entry for the icon
**      buffer		    pointer to context buffer
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
**      buffer is filled in
**
**--
**/

void	create_icon(icon_entry,buffer)

sym_value_entry_type	*icon_entry;
char			*buffer;

{
    sym_value_entry_type    *row_entry;
    RGMIconImage	    *icon;
    RGMResourceDesc	    *desc;
    MrmCode		    arg_form;
    MrmCode		    arg_type;
    long		    arg_value;
    MrmCode		    arg_access;
    char		    *arg_index;
    MrmResource_id	    arg_id;
    MrmCode		    arg_group;

    unsigned char	    *sbyte;
    unsigned char	    *tbyte;
    int			    w_len;
    int			    p_len;
    int			    i;
    int			    j;
    char		    pixel_type;
    char		    pixel_size;
    char		    pixel_per_byte;

    /*
    **	Fill in the fixed location fields of the IconImage.
    */

    icon = (RGMIconImage *)buffer;

    icon->validation = URMIconImageValid;
    pixel_type = icon_entry->b_pixel_type;
    icon->pixel_size = pixel_type + 1;
    icon->width = icon_entry->value.z_icon->w_width;
    icon->height = icon_entry->value.z_icon->w_height;
    icon->ct_type = MrmRtypeResource;
    icon->color_table.ctoff = sizeof( RGMIconImage );
    icon->pixel_data.pdoff = icon_entry->b_data_offset;

    /*
    **	Place color table resource descriptor in the context.
    **
    **	Call ref_value which will return all info need to complete
    **	the description.
    */

    arg_form = ref_value( icon_entry->value.z_icon->az_color_table,
			  & arg_type, & arg_value, & arg_access,
			  & arg_index, & arg_id, & arg_group );

    desc = (RGMResourceDesc *)(buffer + sizeof( RGMIconImage ));

    desc->access = arg_access;
    desc->type = arg_form;
    desc->res_group = arg_group;
    desc->cvt_type = MrmRtypeResource;

    switch (arg_form)
    {
    case URMrRID:
	desc->size = sizeof( RGMResourceDesc );
	desc->key.id = arg_id;
	break;
    case URMrIndex:
	desc->size = strlen( arg_index ) + 1;
	_move( desc->key.index, arg_index, desc->size );
	desc->size += sizeof( RGMResourceDesc ) - 
		      sizeof( MrmResource_id );
	break;
    default:
	_assert( FALSE, "immediate color values not supported" );
    }

    /*
    **	Now move the pixels into the buffer
    **	Variable usage:
    **	    sbyte:  base of source byte stream
    **	    tbyte:  current position in target byte stream
    **	    w_len:  # of pixels that will go into integral # of bytes
    **	    p_len:  # of pixels that will go into final byte
    */

    pixel_per_byte = 8 >> pixel_type;
    pixel_size = 1 << pixel_type;

    tbyte = (unsigned char *)(buffer + icon_entry->b_data_offset);

    for (row_entry = icon_entry->value.z_icon->az_rows,
	 w_len = ((int)row_entry->w_length / (int)pixel_per_byte) * pixel_per_byte,
	 p_len = row_entry->w_length - w_len;

	 row_entry != NULL;  

	 row_entry = row_entry->az_next_table_value)
    {
	sbyte = (unsigned char *)row_entry->value.c_value;

	for (i = 0;  i < w_len; tbyte++)
	{
	    for (*tbyte = 0, j = 0;  
		 j < 8;  
		 j += pixel_size )
	    {
		unsigned char	t;

		t = sbyte[i++] << j;
		*tbyte |= t;
	    }
	}

	if (p_len > 0)
	{
	    for ( *tbyte = 0, j = 0;
		  j < (p_len * pixel_size);
		  j += pixel_size  )
	    {
		unsigned char	t;

		t = sbyte[i++] << j;
		*tbyte |= t;
	    }
	    tbyte++;
	}
    }
}



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function counts the number of valid, usable entries in a
**	list. It simply accumulates all the nodes in the list (recursively)
**	which match the given node type.
**
**  FORMAL PARAMETERS:
**
**      list_entry		the list to be counted
**	type			the node type to match
**
**  IMPLICIT INPUTS:
**
**  IMPLICIT OUTPUTS:
**
**  FUNCTION VALUE:
**
**      the number of nodes which matched the type
**
**  SIDE EFFECTS:
**
**--
**/

int compute_list_size (list_entry, type)
    sym_list_entry_type		*list_entry;
    int				type;

{

/*
 * Local variables
 */
sym_obj_entry_type		*list_member;
sym_nested_list_entry_type	*nested_list_entry;
int				count = 0;

/*
 * loop down the list
 */
if ( list_entry == NULL ) return 0;
for (list_member=(sym_obj_entry_type *)list_entry->obj_header.az_next;
     list_member!=NULL;
     list_member=(sym_obj_entry_type *)list_member->obj_header.az_next)
    switch ( list_member->header.b_tag )
	{
	case sym_k_nested_list_entry:
	    nested_list_entry = (sym_nested_list_entry_type *) list_member;
	    count += compute_list_size (nested_list_entry->az_list, type);
	    break;
	default:
	    if ( list_member->header.b_tag == (char)type )
		count += 1;
	    break;
	}

return count;

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine creates the internal compression code tables.  
**
**      Upon calling this routine, the internal compression code tables
**      (uil_arg_compr, uil_reas_compr, and uil_widget_compr) have zero 
**	entries for resources which have not been referenced in this UIL 
**	module and have one entries for resrources which have been referenced.  
**
**      This routine assigns increasing integers to each non-zero entry in the
**      internal compression code tables.
**
**      The internal compression code tables are indexed by subclass to yield
**      the external compression code values which are written to the UID file. 
**
**  FORMAL PARAMETERS:
**
**      none
**
**  IMPLICIT INPUTS:
**
**	uil_arg_compr
**	uil_reas_compr
**	uil_widget_compr
**
**  IMPLICIT OUTPUTS:
**
**	uil_arg_compr
**	uil_reas_compr
**	uil_widget_compr
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**--
**/

void create_int_compression_codes ()
{

/*
 * Local variables
 */
int	i;
int	compression_code = 2;

    /*
    ** Request compression codes for all subtree resources.
    */

    for ( i=0 ; i<uil_max_object ; i++ )
	if ( uil_urm_subtree_resource[i] != 0 )
	    uil_arg_compr[uil_urm_subtree_resource[i]] = 1;

    /*
    ** Create compression code tables for object classes. This include
    ** both widgets and gadgets, since both have class literals.
    **
    */

    for (i = 0 ; i <= uil_max_object; i++)
	{
	if (uil_widget_compr[i] == 1)
	    uil_widget_compr[i] = compression_code++;
	}

    /*
    ** Create compression code tables for arguments
    **
    */

    compression_code = 2;
    for (i = 0 ; i <= uil_max_arg ; i++)
	{
	if (uil_arg_compr[i] == 1)
	    uil_arg_compr[i] = compression_code++;
	}

    /*
    ** Create compression code tables for reasons.
    ** Note that the numbering continues from where we left off with args.
    */

    for (i = 0;
	i <= uil_max_reason;
	i++)
	{
	if (uil_reas_compr[i] == 1)
	    uil_reas_compr[i] = compression_code++;
	}

    /*
    ** Create compression code tables for automatic children.
    ** Note that numbering continues where we left off with reasons.
    */
    for (i = 0; i <= uil_max_child; i++)
	{
	if (uil_child_compr[i] == 1)
	    uil_child_compr[i] = compression_code++;
	}

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine creates the external compression code tables.  
**
**
**      This routine writes the corresponding toolkit name to the external
**      compression code table for each non-zero entry in the internal
**      compression code table.
**
**      The internal compression code tables are indexed by subclass to yield
**      the external compression code values which are written to the UID file. 
**      The external compression codes are used as an index to the external
**      compression code tables so that MRM can map the compression code into
**      the corresponding toolkit name.
**
**  FORMAL PARAMETERS:
**
**      none
**
**  IMPLICIT INPUTS:
**
**	uil_arg_compr
**	uil_reas_compr
**	uil_widget_compr
**
**  IMPLICIT OUTPUTS:
**
**	extern_args_compr
**	extern_widget_compr
**	%ArgCmpr (index for argument compression table in UID file)
**	%ReasCmpr (index for reason compression table in UID file)
**	%ClassCmpr (index for class compression table in UID file)
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**--
**/

void create_ext_compression_codes ()
{

/*
 * Local variables
 */
int	i;
int	comp_code;
int	next_code;
int	text_offset;
int	arg_value_count;
int	arg_value_size;
char	*arg_buffer;
int	class_value_count;
int	class_value_size;
char	*class_buffer;
status	urm_status;

    /*
    ** Create compression code tables for arguments
    **
    ** Determine number of elements in external compression table 
    ** ( extern_arg_compr[] ) and size of external compression table.
    */

    arg_value_size = sizeof (UidCompressionTable);
    arg_value_count = UilMrmReservedCodeCount;
    next_code = UilMrmMinValidCode;
    for (i = 0;
	i <= uil_max_arg;
	i++)
	{
	if (uil_arg_compr[i] != 0)
	    {
	    arg_value_count++;
	    next_code++;
	    if (uil_argument_toolkit_names[i] == NULL)
		{
		_assert (FALSE, "unknown argument")
		}
	    else
	        arg_value_size += strlen(uil_argument_toolkit_names[i]) + 1;
	    }
	}

    /*
    ** Add compression codes for reasons
    */

    for (i = 0;
	i <= uil_max_reason;
	i++)
	{
	if (uil_reas_compr[i] != 0)
	    {
	    arg_value_count++;
	    next_code++;
	    if (uil_reason_toolkit_names[i] == NULL)
		{
		_assert (FALSE, "unknown argument")
		}
	    else
	        arg_value_size += strlen(uil_reason_toolkit_names[i]) + 1;
	    }
	}

    /*
    ** Add compression codes for automatic children
    */

    for (i = 0; i <= uil_max_child; i++)
      {
	if (uil_child_compr[i] != 0)
	  {
	    arg_value_count++;
	    next_code++;
	    arg_value_size += strlen(uil_child_names[i]) + 1;
	  }
      }

    /*
    ** Add the space for the table's vector entries (the next code counts
    ** one more space than we need, but as a zero-based code has the
    ** correct number in it).
    */
    arg_value_size += sizeof(UidCTableEntry) * next_code;


    /*
    ** Check that the resource context is large enough to hold the value
    */

    if ( (int)(UrmRCSize( out_az_context )) < arg_value_size )
	{
	if( MrmSUCCESS != 
	   UrmResizeResourceContext( out_az_context, arg_value_size ))
	    issue_urm_error( "allocating context" );
	}

    /*
    ** Set up the resource context and point extern_arg_compr to the resource
    ** context buffer.
    */

    UrmRCSetGroup( out_az_context, URMgLiteral );
    UrmRCSetType( out_az_context, sym_k_asciz_table_value );
    UrmRCSetAccess( out_az_context, URMaPublic );
    UrmRCSetLock( out_az_context, FALSE );
    UrmRCSetSize( out_az_context, arg_value_size );

    arg_buffer = (char *) UrmRCBuffer( out_az_context );

    extern_arg_compr = (UidCompressionTable *)arg_buffer;
    bzero (arg_buffer, arg_value_size);

    /*
    ** Now fill in the actual value of the external compresion code
    ** table ( extern_arg_compr[] ).  
    */

    extern_arg_compr->validation = UidCompressionTableValid;
    extern_arg_compr->num_entries = arg_value_count;
#ifdef WORD64
    text_offset = ((int)&extern_arg_compr->entry[arg_value_count]
		   - (int)extern_arg_compr) * sizeof(int);
#else
    text_offset = (long)&extern_arg_compr->entry[arg_value_count]
	- (long)extern_arg_compr;
#endif
    comp_code = UilMrmMinValidCode;
    for ( i = 0 ; i<=uil_max_arg ; i++ )
	{
	if (uil_arg_compr[i] != 0)
	    {
	    _move( &(arg_buffer[text_offset]), 
		  uil_argument_toolkit_names[i], 
		  strlen(uil_argument_toolkit_names[i]) + 1);
	    extern_arg_compr->entry[comp_code].stoffset = text_offset;
	    text_offset += (strlen(uil_argument_toolkit_names[i]) + 1);
	    comp_code++;
	    }
	}

    for ( i = 0 ; i<=uil_max_reason ; i++ )
	{
	if (uil_reas_compr[i] != 0)
	    {
	    _move( &(arg_buffer[text_offset]), 
		  uil_reason_toolkit_names[i], 
		  strlen(uil_reason_toolkit_names[i]) + 1);
	    extern_arg_compr->entry[comp_code].stoffset = 
		text_offset;
	    text_offset += (strlen(uil_reason_toolkit_names[i]) + 1);
	    comp_code++;
	    }
	}

    for ( i = 0 ; i<=uil_max_child ; i++ )
      {
	if (uil_child_compr[i] != 0)
	  {
	    char *name;
	    
	    if (strncmp(uil_child_names[i], AUTO_CHILD_PREFIX, 
			strlen(AUTO_CHILD_PREFIX)) == 0)
	      name = (uil_child_names[i] + strlen(AUTO_CHILD_PREFIX));
	    else name = uil_child_names[i];

	    _move( &(arg_buffer[text_offset]), name, strlen(name) + 1);
	    extern_arg_compr->entry[comp_code].stoffset = text_offset;
	    text_offset += (strlen(name) + 1);
	    comp_code++;
	  }
      }

    /*
    ** Finally write the argument compression code table out to the UID file
    */
    urm_status = 
      UrmPutIndexedLiteral (out_az_idbfile_id, 
			    UilMrmResourceTableIndex, out_az_context);
    if (urm_status != MrmSUCCESS)
	{
	if (urm_status == MrmEOF)
	    diag_issue_diagnostic ( d_uid_write, diag_k_no_source, 
				    diag_k_no_column, Uil_current_file );
	else
	    issue_urm_error("emitting literal");
	}


    /*
    ** Create compression code tables for classes
    **
    ** Determine number of elements in external compression table 
    ** ( extern_class_compr[] ) and size of external 
    ** compression table.
    ** PROBABL ERROR: WHAT ABOUT GADGETS???
    */

    class_value_size = sizeof (UidCompressionTable);
    class_value_count = UilMrmReservedCodeCount;
    next_code = UilMrmMinValidCode;
    for (i = 0;
	i <= uil_max_object;
	i++)
	if (uil_widget_compr[i] != 0)
	    {
	    class_value_count++;
	    next_code++;
	    if (uil_widget_funcs[i] == NULL)
		{
		_assert (FALSE, "unknown class")
		}
	    else
	        class_value_size += strlen(uil_widget_funcs[i]) + 1;
	    }

    /*
    ** Again, compute the additional size for the vector.
    */

    class_value_size += sizeof(UidCTableEntry) * next_code;

    /*
    ** Check that the resource context is large enough to hold the value
    */

    if ( (int)(UrmRCSize(out_az_context)) < class_value_size )
	{
	if( MrmSUCCESS != 
	   UrmResizeResourceContext( out_az_context, class_value_size ))
	    issue_urm_error( "allocating context" );
	}

    /*
    ** Set up the resource context and point extern_class_compr to the resource
    ** context buffer.
    */

    UrmRCSetGroup( out_az_context, URMgLiteral );
    UrmRCSetType( out_az_context, sym_k_asciz_table_value );
    UrmRCSetAccess( out_az_context, URMaPublic );
    UrmRCSetLock( out_az_context, FALSE );
    UrmRCSetSize( out_az_context, class_value_size );

    class_buffer = (char *) UrmRCBuffer( out_az_context );

    extern_class_compr = (UidCompressionTable *)class_buffer;
    bzero (class_buffer, class_value_size);

    /*
    ** Now fill in the actual value of the external compresion code
    ** table ( extern_class_compr[] ).  
    */

    extern_class_compr->validation = UidCompressionTableValid;
    extern_class_compr->num_entries = class_value_count;
#ifdef WORD64
    text_offset = ((int)&extern_class_compr->entry[class_value_count]
		   - (int)extern_class_compr) * sizeof(int);
#else
    text_offset = (long)&extern_class_compr->entry[class_value_count]
	- (long)extern_class_compr;
#endif
    comp_code = UilMrmMinValidCode;
    for ( i = 0;
	i <= uil_max_object;
	i++)
	{
	if (uil_widget_compr[i] != 0)
	    {
	    _move( &(class_buffer[text_offset]), 
	        uil_widget_funcs[i], 
	        strlen(uil_widget_funcs[i]) + 1);
	    extern_class_compr->entry[comp_code].stoffset = 
		text_offset;
	    text_offset += (strlen(uil_widget_funcs[i]) + 1);
	    comp_code++;
	    }
	}

    /*
    ** Finally write the class compression code table out to the UID file
    */
    urm_status = 
      UrmPutIndexedLiteral (out_az_idbfile_id, UilMrmClassTableIndex, 
			    out_az_context);
    if (urm_status != MrmSUCCESS)
	{
	if (urm_status == MrmEOF)
	    diag_issue_diagnostic ( d_uid_write, diag_k_no_source, 
				    diag_k_no_column, Uil_current_file );
	else
	    issue_urm_error("emitting literal");
	}
}
