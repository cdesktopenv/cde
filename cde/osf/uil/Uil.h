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
/*   $XConsortium: Uil.h /main/11 1995/07/14 09:32:19 drk $ */

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
**      This include file defines the set of definitions for use with
**	UIL compiler callable interface.
**
**--
**/

#ifndef Uil_h
#define Uil_h

/*
**
**  INCLUDE FILES
**
**/



/*
**
**  Definition of Compiler Severity Statuses
**
*/

typedef unsigned int	Uil_status_type;
#define Uil_k_min_status	0
#define Uil_k_success_status	0
#define Uil_k_info_status	1
#define Uil_k_warning_status	2
#define Uil_k_error_status	3
#define Uil_k_severe_status	4
#define Uil_k_max_status	4

/*
**
*/

typedef char (*string_array)[];
#define CEIL(a,b) ((a) < (b) ? (a) : (b))


/*
**  Uil_command_type -- Input which describes how/what to compile.
*/

typedef struct  _Uil_command_type
{
    char	    *source_file;	    /* single source to compile	    */
    char	    *resource_file;	    /* name of output file	    */
    char	    *listing_file;	    /* name of listing file	    */
    unsigned int    include_dir_count;	    /* number of directories in	    */
					    /* include_dir array	    */
    char	    **include_dir;	   /* directory to search for	    */
					    /* includes files		    */
    unsigned	    listing_file_flag: 1;   /* produce a listing	    */
    unsigned	    resource_file_flag: 1;  /* generate UID output	    */
    unsigned	    machine_code_flag : 1;  /* generate machine code	    */
    unsigned	    report_info_msg_flag: 1;/* report info messages	    */
    unsigned	    report_warn_msg_flag: 1;/* report warnings		    */
    unsigned	    parse_tree_flag: 1;	    /* generate parse tree	    */
    unsigned        issue_summary: 1;       /* issue diagnostics summary    */
    
    unsigned int    status_update_delay;    /* Number of times a status	    */
		    			    /* point is passed before	    */
					    /* calling statusCB routine	    */
					    /* 0 means called every time    */
    char	    *database;		    /* name of database file 	    */
    unsigned	    database_flag: 1; 	    /* read a new database file     */
    unsigned	    use_setlocale_flag: 1;  /* Enable calls to setlocale    */
} Uil_command_type;



/*
** Uil_compile_desc_type -- Output information about the compilation including
** the compiler_version, data_structure_version, parse tree, and error counts.
*/

typedef struct _Uil_comp_desc
{
    unsigned int	compiler_version;   /* version number of Compiler */
    unsigned int	data_version;	    /* version number of structures */
    char		*parse_tree_root;   /* parse tree output */
    unsigned int	message_count[Uil_k_max_status+1];    
					    /* array of severity counts */
} Uil_compile_desc_type;    




/*
**  Uil_continue_type -- A value returned from a Uil callback routine which
**  allows the application to specify whether to terminate or continue the
**  compilation.
*/

typedef unsigned int	Uil_continue_type;
#define Uil_k_terminate		0
#define Uil_k_continue		1



/*
**
**  Entry Points
**
*/

#ifndef _ARGUMENTS
#define _ARGUMENTS(arglist) arglist
#endif

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

extern Uil_status_type Uil
		    _ARGUMENTS((
			Uil_command_type *command_desc ,
			Uil_compile_desc_type *compile_desc ,
			Uil_continue_type (*message_cb )(),
			char *message_data ,
			Uil_continue_type (*status_cb )(),
			char *status_data ));


#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#undef _ARGUMENTS


#endif /* Uil_h */
/* DON'T ADD STUFF AFTER THIS #endif */
