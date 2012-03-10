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
static char rcsid[] = "$XConsortium: UilMain.c /main/14 1996/06/03 15:49:20 pascale $"
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
**      This module is the main procedure of the UIL Compiler.
**
**--
**/

/*
**
**  INCLUDE FILES
**
*/

#include <X11/Xlocale.h>
/* Sun's locale.h defines ON && OFF, which is also defined in UilLexPars.h */
#ifdef ON
#undef ON
#endif
#ifdef OFF
#undef OFF
#endif
#ifndef X_NOT_STDC_ENV
#include <stdlib.h>
#endif
#include "UilDefI.h"
#include <setjmp.h>

/*
**
**  TABLE OF CONTENTS
**
*/

/*
** FORWARD DECLARATIONS
*/


extern int main  _ARGUMENTS(( int l_argc , char *rac_argv []));

static void common_main  _ARGUMENTS(( void ));
static void common_cleanup  _ARGUMENTS(( void ));

#ifdef CALLABLE
static void UilWrapup _ARGUMENTS((Uil_compile_desc_type *compile_desc));
#endif	/* CALLABLE */

/*
**
**  EXTERNAL Definitions
**
*/


#ifndef NO_MESSAGE_CATALOG
#include <nl_types.h>
#if !defined(NL_CAT_LOCALE)
#define NL_CAT_LOCALE	0
#endif

externaldef(uilmsg) nl_catd uil_catd = NULL;

#endif

/*
**
**  MODULE Definitions
**
*/

static status      return_status = 0;
static jmp_buf     environment;
static unsigned    module_flags = 0;
static unsigned	   doing_exit = 0;

/*  Bit definitions for module_flags */

#define     compiler_called (1 << 0)


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This is the common part of the main routine.
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
**      compilation occurs
**
**--
**/

static void	common_main()
{
#ifndef NO_MESSAGE_CATALOG
  if (uil_catd == NULL)
    uil_catd = catopen("Uil", NL_CAT_LOCALE);
#endif

  /* Initialize the X toolkit. */
  XtToolkitInitialize(); 
  
    /* use the user supplied data base instead */
    if (Uil_cmd_z_command.v_database)
	db_incorporate();

    /* initialize the diagnostic system */
    diag_initialize_diagnostics();

    /* initialize the symbol table */
    sym_initialize_storage();

    /* initialize the source */
    src_initialize_source();

    /* open listing file if requested */
    if (Uil_cmd_z_command.v_listing_file)
	lst_open_listing();

    /* initialize the lexical analyzer */
    lex_initialize_analyzer();

    /* initialize the keyword table */
    key_initialize();

    /* initialize the sar data structures */
    sar_initialize();

    /* call YACC to parse the source file */
    /* return 0 for success, 1 for failure */
    /* Make sure the root entry sections pointer is filled in */
    if (yyparse() != 0)
    	diag_issue_diagnostic
	    (d_inv_module, diag_k_no_source, diag_k_no_column);
    sym_az_root_entry->sections = sym_az_current_section_entry;

    /* call forward reference resolver */
    sem_resolve_forward_refs();

    /* call semantic validation */
    sem_validation ();

    /* call the output phase if requested */
    if (Uil_cmd_z_command.v_resource_file)
	sem_output_uid_file();

    /* call symbol table dumper - if requested */
#if debug_version
    if (uil_v_dump_symbols == TRUE)
	sym_dump_symbols();
#endif


    /* write compilation summary */
    diag_issue_summary();


    /* write listing file if requested */
    if (Uil_cmd_z_command.v_listing_file)
	lst_output_listing();

    /* Storage is not cleaned up, since we will exit */
    uil_exit( uil_l_compile_status );

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This is the image termination procedure for the UIL compiler.
**
**  FORMAL PARAMETERS:
**
**      severity	status of compilation on exit
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
**      never return from this procedure
**
**--
**/

void	uil_exit(severity)

int	severity;

{




    /* Prevent multiple looping through this routine */
    if ( doing_exit ) return;
    doing_exit = 1;

    return_status = (severity >= uil_k_error_status);

    /* Close and remove the uid file if it's still open. */
    /* It will only be open if a severe error occurred during the output */
    /* of the uid file. */

    if (out_az_idbfile_id != NULL)
	UrmIdbCloseFile (out_az_idbfile_id, FALSE);

    /* If compiler called, return to calling program rather than */
    /* exiting process */

    if (module_flags & compiler_called)
        longjmp (environment, 1);

    /* RAP FIX for listing files */
    common_cleanup();


    exit(return_status);
}






#ifndef CALLABLE
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This is the main routine
**
**  FORMAL PARAMETERS:
**
**      l_argc:	    the number of command line arguments
**	rac_argv:   an array of pointers to character array arguments
**
**  IMPLICIT INPUTS:
**
**      none
**
**  IMPLICIT OUTPUTS:
**
**      none
**
**  COMPLETION CODES:
**
**      1 if errors; 0 otherwise
**
**  SIDE EFFECTS:
**
**	produce possibly a resource file and a listing file
**--
**/

int	main( l_argc, rac_argv )
int	l_argc;
char	*rac_argv[ ];
{
    
    setlocale(LC_ALL, "");

    /* call routine to parse the command line */

    cmd_decode_command_line( l_argc, rac_argv );

    /* call common main routine */

    common_main();

}
#endif /* !CALLABLE */



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This is the dynamic memory cleanup routine.  It calls all facility
**	provided cleanup routines so that dynamic structures can be released.
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
**      all dynamic memory is freed 
**
**--
**/

static void	common_cleanup()
{

    /* cleanup the source file information */

    Uil_src_cleanup_source();


    /* cleanup listing facility */

    Uil_lst_cleanup_listing();


    /* cleanup the lexical analyzer */

    Uil_lex_cleanup_analyzer();


    return;
}


#ifdef CALLABLE
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This is the callable entry point for the UIL Compiler. 
**
**  FORMAL PARAMETERS:
**
**      command_desc    pointer to data structure defining UIL command line
**
**	compile_desc	pointer to data structure describing the results of
**			the compilation.
**
**	message_cb	callback routine to process messages
**
**	message_data	user-data passed to the message_cb
**
**	status_cb	callback routine to process messages
**
**	status_data	user-data passed to the status_cb
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
**  COMPLETION CODES:
**
**      uil$_normal if no errors; uil$_no_output otherwise
**
**  SIDE EFFECTS:
**
**	    none
**--
**/


Uil_status_type Uil 

(Uil_command_type        *command_desc,
Uil_compile_desc_type   *compile_desc,
Uil_continue_type       (*message_cb)(),
char            *message_data,
Uil_continue_type       (*status_cb)(),
char            *status_data)


{

    /* Indicate compiler called rather than being started via command line */

    module_flags = module_flags | compiler_called;
    doing_exit = 0;


    /* Initialize command line data structure */

    Uil_cmd_z_command.ac_database = command_desc -> database;
    Uil_cmd_z_command.v_database = command_desc -> database_flag;
    Uil_cmd_z_command.ac_source_file = command_desc -> source_file;
    Uil_cmd_z_command.ac_resource_file = command_desc -> resource_file;
    Uil_cmd_z_command.ac_listing_file = command_desc -> listing_file;
    Uil_cmd_z_command.include_dir_count = command_desc -> include_dir_count;
    Uil_cmd_z_command.ac_include_dir = command_desc -> include_dir;
    Uil_cmd_z_command.v_listing_file = command_desc -> listing_file_flag;
    Uil_cmd_z_command.v_resource_file = command_desc -> resource_file_flag;
    Uil_cmd_z_command.v_show_machine_code = command_desc -> machine_code_flag;
    Uil_cmd_z_command.v_report_info_msg = command_desc -> report_info_msg_flag;
    Uil_cmd_z_command.v_report_warn_msg = command_desc -> report_warn_msg_flag;
    Uil_cmd_z_command.v_parse_tree = command_desc -> parse_tree_flag;
    Uil_cmd_z_command.v_use_setlocale = command_desc -> use_setlocale_flag;
    Uil_cmd_z_command.v_issue_summary = command_desc -> issue_summary;
    Uil_cmd_z_command.status_update_delay = command_desc -> status_update_delay;
    Uil_cmd_z_command.message_cb = message_cb;
    Uil_cmd_z_command.message_data = message_data;
    Uil_cmd_z_command.status_cb = status_cb;
    Uil_cmd_z_command.status_data = status_data;

    /* The setjmp function allows us to unwind from a fatal error setjmp is */
    /* nonzero if we are returning from a fatal error			    */

        if (setjmp(environment) == 0) {
    
	    /* use the user supplied data base instead. If no source file
	       is given (this call is only to change the database), return at
	       this point with a success. */
	    if ( Uil_cmd_z_command.v_database )
		{
		db_incorporate ();
		if ( Uil_cmd_z_command.ac_source_file == NULL )
		    uil_exit (uil_k_success_status);
		}

            /* initialize the diagnostic system */
	    uil_l_compile_status = uil_k_success_status;
            diag_initialize_diagnostics();
    
            /* initialize the symbol table */
            sym_initialize_storage();
    
            /* initialize the source */
            src_initialize_source();
    
            /* open listing file if requested */
            if (Uil_cmd_z_command.v_listing_file)
            	lst_open_listing();

            /* initialize the lexical analyzer */
            lex_initialize_analyzer();
    
            /* initialize the keyword table */
            key_initialize();
    
	    /* initialize the sar data structures */
	    sar_initialize();

	    /* call YACC to parse the source file */
            /* return 0 for success, 1 for failure */
	    /* Make sure the root entry sections pointer is filled in */
            if (yyparse() != 0)
            	diag_issue_diagnostic
        	    (d_inv_module, diag_k_no_source, diag_k_no_column);
	    sym_az_root_entry->sections = sym_az_current_section_entry;
    
            /* call forward reference resolver */
            sem_resolve_forward_refs();
        
	    /* call semantic validation */
	    sem_validation ();

            /* call the output phase if requested */
            if (Uil_cmd_z_command.v_resource_file)
            	sem_output_uid_file();

            /* call symbol table dumper - if requested */
#if debug_version
		if (uil_v_dump_symbols == TRUE)
		    sym_dump_symbols();
#endif

	    /* Perform standard wrapup processing */
	    UilWrapup (compile_desc);
    
/*
 * Fix for CR 5534 - call the routine to restore the old signal handlers
 */
            diag_restore_diagnostics();
/*
 * End Fix for CR 5534 
 */

	    /* exit with the compile status */
	    return uil_l_compile_status;

            }

	/*
	**  longjmp return from setjmp.  This is the case of a return via 
	**  uil_exit the value return_status is set by uil_exit.
	*/
        else
	    {
	    /* Do standard compiler wrapup */
	    UilWrapup (compile_desc);

	    /* return the status set by uil_exit */
	    return uil_l_compile_status;

	    }

}

/*
 * Local function to provide compiler wrapup processing. It is called both
 * from the longjmp and sequential paths in the callable compiler above.
 */
static void UilWrapup (compile_desc)
    Uil_compile_desc_type	*compile_desc;

{

    int i;  /* loop index for copying message counts		    */

    /* write compilation summary if requested */
    if ( Uil_cmd_z_command.v_issue_summary )
	diag_issue_summary();
    
    /* write listing file if requested */
    if (Uil_cmd_z_command.v_listing_file)
	lst_output_listing();

    /*
     ** fill in the "parse tree root" in the compile descriptor,
     ** and set the version for the compiler and the symbol table
     ** structure.
     */
    compile_desc->parse_tree_root = (char *)sym_az_root_entry;
    compile_desc->data_version = _data_version;
    compile_desc->compiler_version = _compiler_version_int;

    /* Fill in the message_summary array in the compile_desc */
    for (i = uil_k_min_status; i <= uil_k_max_status; i++)
	compile_desc->message_count[i] = Uil_message_count[i];

    /* If there are any error/severe messages, then don't return */
    /* a symbol table for the callable compiler - clean up here */
    if ( Uil_message_count[Uil_k_error_status]>0 ||
	 Uil_message_count[Uil_k_severe_status]>0 )
	{
	Uil_cmd_z_command.v_parse_tree = FALSE;
	compile_desc->parse_tree_root = NULL;
	}

    /* Call the cleanup routine to free dynamic memory */
    common_cleanup();
    
    /* Cleanup storage; what is cleaned up depends on whether compiler	*/
    /* was called or not						*/
    Uil_sym_cleanup_storage (Uil_cmd_z_command.v_parse_tree!=1);    

    }

#endif	/* CALLABLE */

#ifdef NO_MEMMOVE

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This is a memmove function that explicitly handles
**      overlapping memory areas.  Written in response to
**      CR 4851.
**
**  FORMAL PARAMETERS:
**
**      same as memcpy
**
**  COMPLETION CODES:
**
**      same as memcpy
**
**  SIDE EFFECTS:
**
**--
**/

char *uil_mmove(s1, s2, n)
char *s1, *s2;
int n;
{
     char *temp;

     if(s2 == s1)
         return(s2);
     if(s2 < s1 && s1 <= s2 + n){
         if(temp = (char *)malloc(n)){
             memcpy(temp, s2, n);
             memcpy(s1, temp, n);
             free(temp);
             return(s1);
         }
         printf("uil_mmove: Memory allocation failed!\n");
         exit(-1);
     }
     return((char *)memcpy(s1, s2, n));
}
#endif  /* NO_MEMMOVE */
