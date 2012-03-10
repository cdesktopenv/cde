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
static char rcsid[] = "$XConsortium: UilCmd.c /main/11 1995/07/14 09:32:29 drk $"
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
**      Command line interpreter for the
**
**--
**/

/*
**
**  INCLUDE FILES
**
**/


#include "UilDefI.h"
#include "UilCmdDef.h"


/*
**
**  GLOBAL DECLARATIONS
**
**/

externaldef(uil_comp_glbl) cmd_command_line_type Uil_cmd_z_command;



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This procedure parses the command line and places the
**	results of the parse in the global structure "Uil_cmd_z_command".
**
**  FORMAL PARAMETERS:
**
**      l_arg_count:	number of command arguments
**	rac_arg_value:	array of pointers to null terminated character strings
**			each of which is one of the command line arguments
**
**  IMPLICIT INPUTS:
**
**      none
**
**  IMPLICIT OUTPUTS:
**
**      Uil_cmd_z_command:	respository for command line info
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**      Uil_cmd_z_command is set
**
**--
**/
void	cmd_decode_command_line( l_arg_count, rac_arg_value )
int 	l_arg_count;
char 	*rac_arg_value[ ];

{
    static char	    *include_list	[Uil_k_max_include_dir_count];
    int	i;

    Uil_cmd_z_command.ac_source_file = NULL;
    Uil_cmd_z_command.ac_resource_file = NULL;
    Uil_cmd_z_command.ac_listing_file = NULL;
    Uil_cmd_z_command.include_dir_count = 0;
    Uil_cmd_z_command.ac_include_dir = (char **)include_list;
    Uil_cmd_z_command.v_resource_file = TRUE;
    Uil_cmd_z_command.v_listing_file = FALSE;
    Uil_cmd_z_command.v_show_machine_code = FALSE;
    Uil_cmd_z_command.v_report_info_msg = TRUE;
    Uil_cmd_z_command.v_report_warn_msg = TRUE;
    Uil_cmd_z_command.v_parse_tree = FALSE;
    Uil_cmd_z_command.v_use_setlocale = FALSE;
    Uil_cmd_z_command.status_update_delay = 0;
    Uil_cmd_z_command.message_cb = (Uil_continue_type(*)())NULL;
    Uil_cmd_z_command.status_cb = (Uil_continue_type(*)())NULL;
    Uil_cmd_z_command.ac_database = NULL;
    Uil_cmd_z_command.v_database = FALSE;

#if debug_version
    uil_v_dump_tokens = FALSE;
    uil_v_dump_symbols = FALSE;
#endif

    /* traverse the options on the command line */

    for (i = 1;  i < l_arg_count;  i++)
    {
	/* check for an output file  */

	if ( strcmp("-o", rac_arg_value[ i ]) == 0 )
	{
	    /* the next argument is the output file name  */

	    /* check next field is not an option */

            if (((i+1) >= l_arg_count) ||
                ( '-' == rac_arg_value[ i+1 ][ 0 ] ))
	    {
		diag_issue_diagnostic
			( d_miss_opt_arg, 
			  diag_k_no_source, diag_k_no_column,
			  rac_arg_value[ i ],
			  "output file"
			);
		continue;
	    }

	    if (Uil_cmd_z_command.ac_resource_file == NULL)
	        Uil_cmd_z_command.ac_resource_file = rac_arg_value[ i+1 ];
	    else
		diag_issue_diagnostic
		    ( d_dupl_opt, 
		      diag_k_no_source, diag_k_no_column,
		      rac_arg_value[ i ]
		    );
	    i = i + 1;
	}
	    
	/* check for a binary database file */

	else if ( strcmp("-wmd", rac_arg_value[ i ]) == 0 )
	{
	    /* the next argument is the binary database file name  */

	    /* check next field is not an option */

            if (((i+1) >= l_arg_count) ||
                ( '-' == rac_arg_value[ i+1 ][ 0 ] ))
	    {
		diag_issue_diagnostic
			( d_miss_opt_arg, 
			  diag_k_no_source, diag_k_no_column,
			  rac_arg_value[ i ],
			  "binary database file"
			);
		continue;
	    }

	    if (!Uil_cmd_z_command.v_database)
	    {
	        Uil_cmd_z_command.v_database = TRUE;
	        Uil_cmd_z_command.ac_database = rac_arg_value[ i+1 ];
	    }
	    else
		diag_issue_diagnostic
		    ( d_dupl_opt, 
		      diag_k_no_source, diag_k_no_column,
		      rac_arg_value[ i ]
		    );
	    i = i + 1;
	}

	/* check for an listing file */

	else if ( strcmp("-v", rac_arg_value[ i ]) == 0 )
	{
	    /* the next argument is the listing file name  */

	    /* check next field is not an option */

            if (((i+1) >= l_arg_count) ||
                ( '-' == rac_arg_value[ i+1 ][ 0 ] ))
	    {
		diag_issue_diagnostic
			( d_miss_opt_arg, 
			  diag_k_no_source, diag_k_no_column,
			  rac_arg_value[ i ],
			  "listing file"
			);
		continue;
	    }

	    if (!Uil_cmd_z_command.v_listing_file)
	    {
	        Uil_cmd_z_command.v_listing_file = TRUE;
	        Uil_cmd_z_command.ac_listing_file = rac_arg_value[ i+1 ];
	    }
	    else
		diag_issue_diagnostic
		    ( d_dupl_opt, 
		      diag_k_no_source, diag_k_no_column,
		      rac_arg_value[ i ]
		    );
	    i = i + 1;
	}
	    
	/* check for the machine code option */

	else if ( strcmp("-m", rac_arg_value[ i ]) == 0 )
	{
	        Uil_cmd_z_command.v_show_machine_code = TRUE;
	}
	    
	/* check if warnings are to be supressed */

	else if ( strcmp("-w", rac_arg_value[ i ]) == 0 )
	{
	        Uil_cmd_z_command.v_report_info_msg = FALSE;
	        Uil_cmd_z_command.v_report_warn_msg = FALSE;
	}

	/* check if setlocale is to be enabled */

	else if ( strcmp("-s", rac_arg_value[ i ]) == 0 )
	{
	  Uil_cmd_z_command.v_use_setlocale = TRUE;
	}
	
	/* check for an unexpected option */

	else if ( '-' == rac_arg_value[ i ][ 0 ] )
	{

	/* check for an include directory */

	    if ( 'I' == rac_arg_value[ i ][ 1 ] )
	    {
		if (Uil_cmd_z_command.include_dir_count < Uil_k_max_include_dir_count)
		    
		    include_list[Uil_cmd_z_command.include_dir_count++] = 
			& rac_arg_value[i] [2];
		else
		    diag_issue_diagnostic
		        ( d_too_many_dirs, 
		          diag_k_no_source, diag_k_no_column,
		          rac_arg_value[ i ], Uil_k_max_include_dir_count
		        );
	    } else
	    {
		diag_issue_diagnostic
		    ( d_unknown_opt, 
		      diag_k_no_source, diag_k_no_column,
		      rac_arg_value[ i ]
		    );
	    }
	}

	/* assume it is a UIL source file specification
	 * validation of the file spec is done when file is opened */

	else
	{
	    if (Uil_cmd_z_command.ac_source_file == NULL)
	       Uil_cmd_z_command.ac_source_file = rac_arg_value[ i ];
	    else
		diag_issue_diagnostic
		    ( d_add_source, 
		      diag_k_no_source, diag_k_no_column,
		      rac_arg_value[ i ]
		    );
	}
    }

    /*
    **	Check for a source file - otherwise issue a diagnostic.
    */

    if (Uil_cmd_z_command.ac_source_file == NULL)
	diag_issue_diagnostic
	    ( d_no_source, diag_k_no_source, diag_k_no_column );

    if (Uil_cmd_z_command.ac_resource_file == NULL)
	{
        Uil_cmd_z_command.ac_resource_file = XtMalloc (strlen ("a.uid") + 1);
        strcpy (Uil_cmd_z_command.ac_resource_file,"a.uid");
        }

    /*
    **	Machine code listing only makes sense if listing is set.
    */
    
    Uil_cmd_z_command.v_show_machine_code =
        ( Uil_cmd_z_command.v_listing_file & 
	  Uil_cmd_z_command.v_show_machine_code);
}



