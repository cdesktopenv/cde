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
static char rcsid[] = "$XConsortium: UilDiags.c /main/15 1996/10/21 11:06:46 cde-osf $"
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
**      This module contains the procedures for issuing diagnostics
**	for the UIL compiler.
**
**
**--
**/


/*
**
**  INCLUDE FILES
**
**/

#include <stdio.h>
#include <signal.h>
#include <setjmp.h>
#include <X11/Intrinsic.h>
#include <X11/Xos.h>
#include <Xm/Xm.h>

#include <stdarg.h>

#include "UilDefI.h"
#include "UilMessTab.h"

/*
**
**  EXTERNAL Storage
**
*/

externaldef(uil_comp_glbl)	int	Uil_message_count[uil_k_max_status+1];

/*
**
**  OWN Storage
**
*/

static	boolean	issuing_diagnostic;
static	int	Uil_diag_status_delay_count;

/*
 * Fix for CR 5534 - static storage for old signal handlers
 */
static	void 	(*bus_handler)();
static	void 	(*sys_handler)();
static	void 	(*fpe_handler)();

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      diag_store_handlers saves the signal handlers from the calling
**	application in global variables.
**
**  FORMAL PARAMETERS:
**
**	none
**
**  IMPLICIT INPUTS:
**
**	none
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
**      the global signal handler variables are changed.
**
**--
**/

void    diag_store_handlers
            ( void )
{
/*
 * Fix for CR 5534 - set the file variables for holding previous bus handlers
 *                   to NULL and then store the old handlers (if any) when
 *                   calling signal for the Uil handlers
 */
    bus_handler = signal( SIGBUS, diag_handler );       /* access violations */
#ifdef SIGSYS
    sys_handler = signal( SIGSYS, diag_handler );       /* bad arguments to sys calls */
#endif
    fpe_handler = signal( SIGFPE, diag_handler );       /* overflows */
/*
 * End Fix for CR 5534
 */

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      diag_issue_diagnostic emits a diagnostic to the listing file if
**	there is one and also sends an error to standard error file.
**
**  FORMAL PARAMETERS:
**
**      d_message_number:  index of the error to issue.  The indices are
**			   defined in message.h
**	a_src_rec:	   pointer to the source record to issue the
**			   diagnostic against.  This data structure has
**	l_start_column:	   position within the source line for diagnostic.
**			   A negative value indicates no position.
**	args:		   a variable length parameter list that holds the
**			   arguments to be substituted in the diagnositic
**
**  IMPLICIT INPUTS:
**
**      diag_rz_msg_table: table of messages and their severities defined
**			   in message_table.h
**	diag_rl_external_code:  Possible OS-specific external code
**
**  IMPLICIT OUTPUTS:
**
**      messages emitted
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**      message counts updated
**
**--
**/

#define buf_size (src_k_max_source_line_length + 1)

void	diag_issue_diagnostic
	    ( int d_message_number, src_source_record_type *az_src_rec, 
	      int l_start_column, ...)

{
    va_list	ap;			/* ptr to variable length parameter */
    int		severity;		/* severity of message */
    int		message_number;		/* message number */
    char	msg_buffer[132];	/* buffer to construct message */
    char	ptr_buffer[buf_size];	/* buffer to construct pointer */
    char	loc_buffer[132];	/* buffer to construct location */
    char	src_buffer[buf_size];	/* buffer to hold source line */

    /*
    **	check if we are in a loop issuing errors
    */

    if (issuing_diagnostic)
    {
	_debug_output( "nested diagnostics issued" );
	Uil_message_count[ uil_k_severe_status ]++;
	uil_exit( uil_k_severe_status );
    }

    issuing_diagnostic = TRUE;

    /*
    **	determine the severity of the error.  For d_submit_spr we issue
    **	the fix previous error diagnostic, if we encountered prior errors;
    **  otherwise we let it thru.
    */

    message_number = d_message_number;

    if (message_number == d_submit_spr)
	if (Uil_message_count[ uil_k_error_status ] > 0)
	    message_number = d_prev_error;

    severity = diag_rz_msg_table[ message_number ].l_severity;

    /*
    **	check if messages of this severity are to be reported.
    */

    switch (severity)
    {
    case uil_k_info_status:
	if (Uil_cmd_z_command.v_report_info_msg)
	    break;

	issuing_diagnostic = FALSE;
	return;

    case uil_k_warning_status:
	if (Uil_cmd_z_command.v_report_warn_msg)
	    break;

	issuing_diagnostic = FALSE;
	return;

    default:
	;
    }

    Uil_message_count[ severity ]++;
    if (severity > uil_l_compile_status)
	uil_l_compile_status = severity;

    /*
    **	Diagnostic format varies considerably
    **	   1) no source
    **		message
    **	   2) source but no column
    **		source line
    **		message
    **		location in source message
    **	   3) source and column
    **		source line
    **		column pointer
    **		message
    **		location in source message
    **	   4) source and column but no access key 
    **		message
    **		location in source message
    */

    /* 
    **	substitute any parameters into the error message placing the
    **	resultant string in msg_buffer
    */

    va_start(ap, l_start_column);

#ifndef NO_MESSAGE_CATALOG
    vsprintf( msg_buffer, 
	      catgets(uil_catd, UIL_SET1, msg_cat_table[ message_number ],
		      diag_rz_msg_table[ message_number ].ac_text), 
	     ap );
#else
    vsprintf( msg_buffer, 
	      diag_rz_msg_table[ message_number ].ac_text, 
	      ap );
#endif
    va_end(ap);

    src_buffer[ 0 ] = 0;
    loc_buffer[ 0 ] = 0;
    ptr_buffer[ 0 ] = 0;

    if (az_src_rec != diag_k_no_source) 
    {
	if ( !_src_null_access_key(az_src_rec->z_access_key) )
	{
	    /*
	    **	create the location line line
	    */

#ifndef NO_MESSAGE_CATALOG
	    sprintf( loc_buffer,
		     catgets(uil_catd, UIL_SET_MISC,
			     UIL_MISC_0, "\t\t line: %d  file: %s"),
		     az_src_rec->w_line_number,
		     src_get_file_name( az_src_rec ) );
#else
	    sprintf( loc_buffer,
		     "\t\t line: %d  file: %s",
		     az_src_rec->w_line_number,
		     src_get_file_name( az_src_rec ) );
#endif

	    /*
	    **	retrieve the source line
	    */

	    src_buffer[ 0 ] = '\t';
	    src_retrieve_source( az_src_rec, &src_buffer[ 1 ] );

	    /*
	    **	filter the standard unprintable characters.
	    */

	    lex_filter_unprintable_chars
		    ( (unsigned char *)src_buffer, strlen( src_buffer ), 0 );

	    /*
	    **	create the column pointer if a source position was given
	    */

	    if (l_start_column != diag_k_no_column)
	    {
		int	i;

		for (i=0;  i < l_start_column+1;  i++)
		{
		    if (src_buffer[ i ] == '\t')
			ptr_buffer[ i ] = '\t';
		    else
			ptr_buffer[ i ] = ' ';
		 }

		ptr_buffer[ i++ ] = '*';
		ptr_buffer[ i ]   = 0;
	    }
	}
	else	/* no access key */
	{
	    /*
	    **	create the location line line
	    */

	    if (l_start_column != diag_k_no_column)
#ifndef NO_MESSAGE_CATALOG
	      sprintf(loc_buffer,
		      catgets(uil_catd, UIL_SET_MISC,
			      UIL_MISC_1, 
			      "\t\t line: %d  position: %d  file: %s"),
		      az_src_rec->w_line_number,
		      l_start_column + 1,
		      src_get_file_name( az_src_rec ) );
#else
	      sprintf(loc_buffer,
		      "\t\t line: %d  position: %d  file: %s",
		      az_src_rec->w_line_number,
		      l_start_column + 1,
		      src_get_file_name( az_src_rec ) );
#endif
	    else
#ifndef NO_MESSAGE_CATALOG
		sprintf( loc_buffer, catgets(uil_catd, UIL_SET_MISC,
					     UIL_MISC_0,
					     "\t\t line: %d  file: %s"),
			 az_src_rec->w_line_number,
			 src_get_file_name( az_src_rec ) );
#else
		sprintf( loc_buffer,
			 "\t\t line: %d  file: %s",
			 az_src_rec->w_line_number,
			 src_get_file_name( az_src_rec ) );
#endif
	}
    }

    /*
    **  issue the error to standard error file (system specific)
    */

    write_msg_to_standard_error
	( message_number, src_buffer, ptr_buffer, msg_buffer, loc_buffer );

    /* 
    **	if we have a listing, place message in the source structure
    */

    if (Uil_cmd_z_command.v_listing_file)
	src_append_diag_info
	    ( az_src_rec, l_start_column, msg_buffer, message_number );

    issuing_diagnostic = FALSE;

    /* 
    **	if there are fatal errors, print the listing file and exit.
    */

    if (Uil_message_count[ uil_k_severe_status] > 0)
    {
	lst_output_listing();
	uil_exit( uil_k_severe_status );
    }

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      diag_issue_summary emits a diagnostic summary if any diagnostics
**	were issued during the compilation
**
**  FORMAL PARAMETERS:
**
**      void
**
**  IMPLICIT INPUTS:
**
**      Uil_message_count	   table of messages issued
**
**  IMPLICIT OUTPUTS:
**
**      messages optionally emitted
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

void	diag_issue_summary()

{
    
    if (uil_l_compile_status == uil_k_success_status)
	return;

    Uil_cmd_z_command.v_report_info_msg = TRUE;

    diag_issue_diagnostic
	( d_summary,
	  diag_k_no_source, diag_k_no_column,
	  Uil_message_count[ uil_k_error_status ],
	  Uil_message_count[ uil_k_warning_status ],
	  Uil_message_count[ uil_k_info_status ] );

    return;
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      The major constructs in the UIL language each have a data structure
**	and a tag value in the first byte of that data structure.  Given
**	a tag value, this routine will return a string that can be substituted
**      in an error message to describe that construct.
**
**  FORMAL PARAMETERS:
**
**      b_tag
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
**      char *	 text corresponding to tag
**
**  SIDE EFFECTS:
**
**      none
**
**--
**/

char	*diag_tag_text( b_tag )

int XmConst   b_tag;

{

    switch (b_tag)
    {
    case sym_k_value_entry:
	return "value";
    case sym_k_widget_entry:
	return "widget";
    case sym_k_gadget_entry:
	return "gadget";
    case sym_k_child_entry:
	return "auto child";
    case sym_k_module_entry:
	return "module";
    case sym_k_proc_def_entry:
    case sym_k_proc_ref_entry:
	return "procedure";
    case sym_k_identifier_entry:
	return "identifier";

/*   These are used by the symbol table dumper.    */

    case sym_k_argument_entry:
	return "argument";
    case sym_k_callback_entry:
	return "callback";
    case sym_k_control_entry:
	return "control";
    case sym_k_name_entry:
	return "name";
    case sym_k_forward_ref_entry:
	return "forward ref";
    case sym_k_external_def_entry:
	return "external def";
    case sym_k_list_entry:
	return "list";
    case sym_k_root_entry:
	return "root";
    case sym_k_include_file_entry:
	return "include file";
    case sym_k_def_obj_entry:
	return "default variant";
    case sym_k_section_entry:
	return "section";

    default:
#ifndef NO_MESSAGE_CATALOG
	return (catgets(uil_catd, UIL_VALUES, UIL_VAL_1, "** unknown **"));
#else
	return "** unknown **";
#endif
    }

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      Each widget in the UIL language has a integer value.  Given this
**	value, this routine will return a string that can be substituted
**      in an error message to describe that widget.
**
**  FORMAL PARAMETERS:
**
**      b_type		integer rep of the widget type
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
**      char *	 text corresponding to type
**
**  SIDE EFFECTS:
**
**      none
**
**--
**/

char	*diag_object_text( b_type )

int XmConst   b_type;

{
    if ( b_type <= sym_k_error_object )
#ifndef NO_MESSAGE_CATALOG
	return (catgets(uil_catd, UIL_VALUES, UIL_VAL_0, "** error **"));
#else
	return "** error **";
#endif
    if ( b_type <= uil_max_object )
	return uil_widget_names[b_type];
#ifndef NO_MESSAGE_CATALOG
    return (catgets(uil_catd, UIL_VALUES, UIL_VAL_1, "** unknown **"));
#else
    return "** unknown **";
#endif

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      Each value in the UIL language has a integer value.  Given this
**	value, this routine will return a string that can be substituted
**      in an error message to describe that value.
**
**  FORMAL PARAMETERS:
**
**      b_type		integer rep of the value type
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
**      char *	 text corresponding to type
**
**  SIDE EFFECTS:
**
**      none
**
**--
**/

char	*diag_value_text( b_type )

int XmConst   b_type;

{

    if ( b_type <= sym_k_error_value )
#ifndef NO_MESSAGE_CATALOG
	return (catgets(uil_catd, UIL_VALUES, UIL_VAL_0, "** error **"));
#else
	return "** error **";
#endif
    if ( b_type <= sym_k_max_value )
	return uil_datatype_names[b_type];
#ifndef NO_MESSAGE_CATALOG
    return (catgets(uil_catd, UIL_VALUES, UIL_VAL_1, "** unknown **"));
#else
    return "** unknown **";
#endif

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      Each charset supported by UIL has a integer value.  Given this
**	value, this routine will return a string that can be substituted
**      in an error message to describe that charset.
**
**  FORMAL PARAMETERS:
**
**      b_type		integer rep of the charset
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
**      char *	 text corresponding to type
**
**  SIDE EFFECTS:
**
**      none
**
**--
**/

char	*diag_charset_text( b_type )

int XmConst   b_type;

{
    
    if ( b_type <= sym_k_error_charset )
#ifndef NO_MESSAGE_CATALOG
	return (catgets(uil_catd, UIL_VALUES, UIL_VAL_0, "** error **"));
#else
	return "** error **";
#endif
    if ( b_type <= uil_max_charset )
	return uil_charset_names[b_type];
#ifndef NO_MESSAGE_CATALOG
    return (catgets(uil_catd, UIL_VALUES, UIL_VAL_1, "** unknown **"));
#else
    return "** unknown **";
#endif

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This routine initializes the diagnostic system.  This includes
**	establishes a handler for catastrophic errors and initializing
**	error tables.
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
**      Uil_message_count
**	issuing_diagnostic
**	Uil_diag_status_delay_count
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**      error handler is supplied for SIGBUS, SIGFPE and SIGSYS errors
**
**--
**/

void	diag_initialize_diagnostics()

{

    int	    i;

    /* 
    **	Set up a handler to be invoked if access violations or
    **	bad arguments to sys calls occur.
    **  Other errors should be processed as is standard for the OS.
    */
/*
 * Fix for CR 5534 - call diag_store_handlers to save the old signal handlers.
 */
    diag_store_handlers();
/*
 * End Fix for CR 5534
 */

    signal( SIGBUS, diag_handler );	/* access violations */
#ifdef SIGSYS
    signal( SIGSYS, diag_handler );	/* bad arguments to sys calls */
#endif
    signal( SIGFPE, diag_handler );	/* overflows */


    /*
    **  Reset the message counts to zero.
    */
    for (i=0;  i <= uil_k_max_status;  i++)
	Uil_message_count[ i ] = 0;


    /*
    **  Clear the flag that we used to detect recursive error reporinting.
    */
    issuing_diagnostic = FALSE;


    /*
    **  Make sure there is no delay before starting to report status.
    */
    Uil_diag_status_delay_count = 0;
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This routine resets the handler for detecting overflows.
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
**      void
**
**  SIDE EFFECTS:
**
**      error handler is supplied for SIGFPE 
**
**--
**/

void	diag_reset_overflow_handler()

{

    signal( SIGFPE, diag_handler );	/* overflows */

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This routine is the handler for internal errors.
**
**  FORMAL PARAMETERS:
**
**      l_error	    type of signal being raised
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
**      image will exit (done by diag_issue_diagnostic)
**
**--
**/

void	diag_handler( l_error )

int	l_error;

{
    /* 
    **	This handler is invoked for access violations, oeverflows or bad 
    **  arguments to sys calls.  Other errors are processed as is standard 
    **	for the OS.  The handler checks on overflow to see if we are trying
    **	to catch them at the moment.  Otherwise it issues an internal error.
    */

    if (l_error == SIGFPE)
    {
	if (uil_az_error_env_valid)
	{
	    longjmp( uil_az_error_env_block, 1);
	}
    }

#if debug_version
    {
      char	*error_text;
      
      switch (l_error)
	{
	case SIGBUS:
	  error_text = "Access Violation";
	  break;
#ifdef SIGSYS
	case SIGSYS:
	  error_text = "Bad system call";
	  break;
#endif
	case SIGFPE:
	  error_text = "Overflow";
	  break;
	default:
	  error_text = "Unknown reason";
	}

      diag_issue_internal_error( error_text );
    }
    
#else

    diag_issue_internal_error( NULL );

#endif

    /* we don't expect to come back */

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This routine issues an internal error.
**
**  FORMAL PARAMETERS:
**
**      error_text	In the debug version this parameter holds a
**			description of the internal error.  In the
**			non debug version it is omitted.
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
**      image will exit (done by diag_issue_diagnostic)
**
**--
**/

void	diag_issue_internal_error( error_text )

char	* error_text;

{

    /* 
    **	This routine is a focal point for issuing internal errors.
    **  In DEBUG mode it takes an argument that gives more information
    **	about the failure.
    */

#if debug_version

    diag_issue_diagnostic
	( d_bug_check, diag_k_no_source, diag_k_no_column , error_text );

#else

    diag_issue_diagnostic( d_submit_spr, diag_k_no_source, diag_k_no_column );

#endif

    /* we don't expect to come back */

}




/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      this routine emits a diagnostic to stderr
**
**  FORMAL PARAMETERS:
**
**	msg_number:	message number 
**      src_buffer:	ptr to source buffer for the error
**      ptr_buffer:	ptr to column buffer locator for the error
**      msg_buffer:	ptr to message buffer for the error
**      loc_buffer:	ptr to location buffer for the error
**
**  IMPLICIT INPUTS:
**
**      none
**
**  IMPLICIT OUTPUTS:
**
**      messages emitted
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


static char	XmConst success_text[1]  = "";
static char	XmConst info_text[7]     = "Info: ";
static char	XmConst warning_text[10] = "Warning: ";
static char	XmConst error_text[8]    = "Error: ";
static char	XmConst severe_text[9]   = "Severe: ";

static char	XmConst *severity_table [] =
    { success_text, info_text, warning_text, error_text, severe_text, };

void	write_msg_to_standard_error
		(message_number, src_buffer, ptr_buffer, msg_buffer, loc_buffer)

XmConst int   message_number;
XmConst char  *src_buffer;
XmConst char  *ptr_buffer;
XmConst char  *msg_buffer;
XmConst char  *loc_buffer;

{

    /*
    **  If message callback was supplied, call it with the description of the
    **  error instead of writing it to standard output.
    */
    if (Uil_cmd_z_command.message_cb != (Uil_continue_type(*)())NULL) 
    {
	Uil_status_type return_status;
/*
 * Fix for 5534 - restore the old signal handers before calling message_cb
 *                and restore the Uil signal handlers immediately afterwards
 */
        diag_restore_diagnostics();
	return_status = (*Uil_cmd_z_command.message_cb)(
			    Uil_cmd_z_command.message_data,
			    message_number,
			    diag_rz_msg_table[ message_number ].l_severity,
			    msg_buffer,
			    src_buffer,
			    ptr_buffer,
			    loc_buffer,
			    Uil_message_count);
         diag_store_handlers();
/*
 * End Fix for 5534
 */

	/*
	**  If request is for termination, then longjmp back to main routine
	**  and set the return status to reflect user abort.
	*/
	if (return_status == Uil_k_terminate)
	    uil_exit (uil_k_error_status);	    
	else
	    return;
    }



    /* print source line */

    fprintf ( stderr, "\n" );

    if (src_buffer[ 0 ] != 0)
    {
	fprintf ( stderr, "%s\n", src_buffer );
     }

    /* print the source column locator line */

    if (ptr_buffer[ 0 ] != 0)
    {
	fprintf ( stderr, "%s\n", ptr_buffer );
     }

    /* print message line */

#ifndef NO_MESSAGE_CATALOG
    fprintf ( stderr, "%s%s\n", 
	      catgets(uil_catd, UIL_SET_MISC,
		      diag_rz_msg_table[ message_number ].l_severity, 
		      severity_table
		      [ diag_rz_msg_table[ message_number ].l_severity ]),
	      msg_buffer );
#else
    fprintf ( stderr, "%s%s\n", 
	      severity_table
		[ diag_rz_msg_table[ message_number ].l_severity ],
	      msg_buffer );
#endif

    /* print location line */

    if (loc_buffer[ 0 ] != 0)
    {
	fprintf ( stderr, "%s\n", loc_buffer );
     }

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      function to return abbreviation for a message number
**
**  FORMAL PARAMETERS:
**
**      d_message_number	message number to look up
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
**      ptr to text for message number
**
**  SIDE EFFECTS:
**
**      none
**
**--
**/

char XmConst *diag_get_message_abbrev( d_message_number )

int	d_message_number;

{
    return
	severity_table[ diag_rz_msg_table[ d_message_number ].l_severity ];
}





/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	Routine to invoke the user-supplied status callback routine.  If none
**	is supplied this routine returns without error.  Otherwise, the user's
**	delay information is processed and if the requested criteria is met the
**	user's routine is invoked.  with parameters to describe the progress of
**	the compilation.  
**
**  FORMAL PARAMETERS:
**
**      none
**
**  IMPLICIT INPUTS:
**
**      Uil_cmd_z_command_line
**
**  IMPLICIT OUTPUTS:
**
**      Uil_diag_status_delay_count
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

void	diag_report_status ( )

{
    Uil_continue_type	return_status;

    /*
    **  If no status callback was supplied, just return.
    */
    if (Uil_cmd_z_command.status_cb == (Uil_continue_type(*)())NULL) return;

    /*
    **	If delay is used up (less than or equal to zero) then invoke the
    **	callback, and reset the delay count.
    */
    if (Uil_diag_status_delay_count <= 0)
    {
	Uil_diag_status_delay_count = Uil_cmd_z_command.status_update_delay;
/*
 * Fix for CR 5534 - restore the application signal handlers before calling
 *                   status_cb and then return the Uil signal handlers 
 *                   immediately after.
 */
        diag_restore_diagnostics();
	return_status = (*Uil_cmd_z_command.status_cb)(
			    Uil_cmd_z_command.status_data,
			    Uil_percent_complete,
			    Uil_lines_processed,
			    Uil_current_file,
			    Uil_message_count);
        diag_store_handlers();
/*
 * End Fix for CR 5534
 */

	/*
	**  If request is for termination, then longjmp back to main routine
	**  and set the return status to reflect user abort.
	*/
	if (return_status == Uil_k_terminate)
	{
	    uil_exit (uil_k_error_status);	    
	}
    }	

    /*
    **  Delay not used-up, so decrement by one.
    */
    else
	Uil_diag_status_delay_count--;

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This routine restores the old signal handlers that were replaced by 
**	Uil.  This routine is only called by the callable Uil.
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
**	none
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**      error handlers for SIGBUS, SIGFPE and SIGSYS errors are restored to
**	pre-Uil condition.
**
**--
**/

void
diag_restore_diagnostics(void)
{

/*
 * Fix for CR 5534 - restore the old signal handlers
 */
    signal( SIGBUS, bus_handler );
#ifdef SIGSYS
    signal( SIGSYS, sys_handler );
#endif
    signal( SIGFPE, fpe_handler );
/*
 * End Fix for CR 5534
 */
}

