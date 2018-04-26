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
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */

/*
 *	$XConsortium: cgen.h /main/3 1995/11/06 17:22:21 rswiston $
 *
 * @(#)cgen.h	1.14 14 Nov 1994	cde_app_builder/src/ab
 *
 * 	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 */

#ifndef _CGEN_H_
#define _CGEN_H_

/*
 * cgen.h - Code Generator interface
 */

#include <X11/Intrinsic.h>
#include <ab_private/obj.h>
#include <ab_private/obj_notify.h>

/*
 * The final goal the user wants to reach.  May need to issue a series of
 * (system) commands to achieve this goal.
 */
typedef enum
{
    CG_GOAL_UNDEF = 0,          /* CG = codegen */
    CG_GOAL_GEN_CODE,
    CG_GOAL_MAKE,
    CG_GOAL_RUN,
    CG_GOAL_MAKE_AND_RUN,
    CG_GOAL_NUM_VALUES
}		CG_GOAL;
 
typedef enum
{
    CG_CMD_UNDEF = 0,
    CG_CMD_GEN_CODE, 
    CG_CMD_MAKE,
    CG_CMD_RUN,
    CG_SUBCOMMAND_NUM_VALUES
}                   CG_SUBCOMMAND;


typedef enum
{
    CG_VERBOSITY_UNDEF = 0,
    CG_VERBOSITY_NORMAL,
    CG_VERBOSITY_SILENT,
    CG_VERBOSITY_VERBOSE,
    CB_VERBOSITY_NUM_VALUES
}		CG_VERBOSITY;


typedef enum
{
    CG_GEN_FLAG_UNDEF = 0,
    CG_GEN_PROJ_FLAG,
    CG_GEN_MAIN_FLAG,
    CG_GEN_SPECIFIC_FILES_FLAG,
    CG_GEN_SPECIFIC_FILES_AND_MAIN_FLAG,
    CG_GEN_FLAG_NUM_VALUES
}		CG_GEN_FLAG;

typedef struct
{
    CG_GEN_FLAG		cmd_flag;
    Boolean		no_merge;
    CG_VERBOSITY	verbosity;
    StringList		module_list;
    STRING		make_args;
    STRING		run_args;
}		CGenOptions;

extern CGenOptions	CodeGenOptions;
extern StringList	user_env_vars;
extern StringList	module_list;

/*
extern void	cgen_show_dialog(
			    Widget	widget,
			    XtPointer	client_data,
			    XtPointer	call_data
			);
*/

extern void 	cgen_show_codegen_win(
			    void 
		);

extern void 	cgen_gen_code( 
		    CG_SUBCOMMAND       cmd 
		); 

extern void 	cgen_make( 
		    CG_SUBCOMMAND       cmd 
		); 

extern void 	cgen_run( 
		    CG_SUBCOMMAND       cmd 
		); 

extern void	cgen_make_run(
		    CG_SUBCOMMAND       cmd
                );

extern void	cgenP_init_props_module_list(
		    Widget      mod_list
		);

extern void 	cgenP_prop_init(void);

extern void 	cgenP_update_mod_listCB(
		    Widget widget,
		    XtPointer clientData,
		    XtPointer callData
		);

/*
 * Updates project directory, title, ...
 */
extern int	cgen_notify_new_directory(STRING directory);
extern int	cgen_notify_new_project(ABObj project);
extern int 	cgen_notify_props_new_proj(ABObj project);
extern int 	cgenP_sync_up_dir(void);

/*
 * Aborts whatever command is running (if any)
 */
extern int	cgen_abort(void);


#endif /* _CGEN_H_ */
