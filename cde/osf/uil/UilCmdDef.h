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
/*   $XConsortium: UilCmdDef.h /main/10 1995/07/14 09:32:45 drk $ */

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
**      This include file defines the interface to the UIL command line
**	processing.
**
**--
**/

#ifndef UilCmdDef_h
#define UilCmdDef_h

/*
**
**  INCLUDE FILES
**
**/


typedef struct  
{
    char	    *ac_source_file;
    char	    *ac_resource_file;
    char	    *ac_listing_file;
    unsigned int    include_dir_count;	    /* number of directories in	    */
					    /* ac_include_dir array	    */
    char	    **ac_include_dir;    /* pointer to array of	    */
					    /* directories to search for    */
					    /* includes in		    */
    unsigned	    v_listing_file: 1;
    unsigned	    v_resource_file: 1;
    unsigned	    v_show_machine_code: 1;
    unsigned	    v_report_info_msg: 1;
    unsigned	    v_report_warn_msg: 1;
    unsigned	    v_parse_tree: 1;	    /* generate parse tree	    */
    unsigned        v_issue_summary: 1;
    
    unsigned int    status_update_delay;    /* Number of times a status	    */
					    /* point is passed before	    */
					    /* calling statusCB routine	    */
					    /* 0 means called every time    */
    Uil_continue_type	(*message_cb)();
    char		*message_data;
    Uil_continue_type	(*status_cb)();
    char		*status_data;
    unsigned	    	v_use_setlocale: 1;    /* Allow localized strings */
    char		*ac_database;
    unsigned 		v_database: 1;
} cmd_command_line_type;

#endif /* UilCmdDef_h */
/* DON'T ADD STUFF AFTER THIS #endif */
