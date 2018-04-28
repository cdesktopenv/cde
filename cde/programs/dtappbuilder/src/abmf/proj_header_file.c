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
 *	$XConsortium: proj_header_file.c /main/3 1995/11/06 18:12:47 rswiston $
 *
 * @(#)proj_header_file.c	3.64 23 Nov 1994	cde_app_builder/src/abmf
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


/*
 * Create a file containing the main() for the given project.  This
 * file is generated when -P or -m flag is used.
 */

#include <sys/param.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <ab/util_types.h>
#include <ab_private/abio.h>
#include "abmfP.h"
#include "cdefsP.h"
#include "ui_header_fileP.h"
#include "instancesP.h"
#include "motifdefsP.h"
#include "utilsP.h"
#include "obj_namesP.h"
#include "write_cP.h"

/*
 * Local functions
 */
static int write_includes(GenCodeInfo genCodeInfo);
static int write_connection_decls(GenCodeInfo genCodeInfo, ABObj module);
static int write_callback_decls(GenCodeInfo genCodeInfo, ABObj module);
static int write_user_or_auto_decls(
			GenCodeInfo	genCodeInfo, 
			ABObj		project, 
			BOOL		auto_named
		);
static int	write_app_resource_struct(GenCodeInfo genCodeInfo, ABObj project);
static int	write_app_resource_var_extern(GenCodeInfo genCodeInfo, ABObj project);
static int write_i18n_declarations(GenCodeInfo genCodeInfo, ABObj project);

/*
 * Main header file includes for i18n: may not be same as main C file
 */
static char        *I18n_Includes[] =
{
    "<nl_types.h>",
    NULL,
};

/*
 * Write the project.h file.
 */
int
abmfP_write_project_header_file(
		GenCodeInfo	genCodeInfo,
		ABObj		project,
		STRING		codeFileName
)
{
    File		codeFile = genCodeInfo->code_file;
    char		projectName[1024];

    /*
     * Write file header.
     */
    abmfP_write_user_header_seg(genCodeInfo);
    abio_puts(codeFile, nlstr);

    sprintf(projectName, "project %s", obj_get_name(project));
    abmfP_write_file_header(
		genCodeInfo, 
		codeFileName, 
		TRUE,
		projectName,
		util_get_program_name(), 
		ABMF_MODIFY_USER_SEGS,
	   " * Contains: object data structures and callback declarations"
		);

    write_includes(genCodeInfo);

    /*
     * Write declarations for i18n.
     * These are needed only if i18n is enabled.
     */
    if (genCodeInfo->i18n_method == ABMF_I18N_XPG4_API)
        write_i18n_declarations(genCodeInfo, project);
    
    write_app_resource_struct(genCodeInfo, project);
    write_connection_decls(genCodeInfo, project);
    write_callback_decls(genCodeInfo, project);
    write_app_resource_var_extern(genCodeInfo, project);

    abmfP_write_user_long_seg(genCodeInfo,
	"Add types, macros, and externs here");

    abmfP_write_file_footer(genCodeInfo, codeFileName, TRUE);

    return OK;
}


static int
write_includes(GenCodeInfo genCodeInfo)
{
    abmfP_write_c_system_include(genCodeInfo, "stdlib.h");
    abmfP_write_c_system_include(genCodeInfo, "X11/Intrinsic.h");

    return 0;
}


/*
 * Traverse object list and write out callback decls, for those
 * connections with the proper value of auto_named.
 */
static int
write_user_or_auto_decls(
			GenCodeInfo genCodeInfo, 
			ABObj project, 
			BOOL auto_named)
{
    AB_TRAVERSAL        trav;
    ABObj               action = NULL;
    AB_ACTION_INFO     *actinfo = NULL;
    STRING              func_name = NULL;

    for (trav_open(&trav, project, AB_TRAV_ACTIONS);
	 (action = trav_next(&trav)) != NULL;)
    {
	/* If action is not a cross-module connection AND it
	 * is not a shared connection, continue.
	 */
	if ( !obj_is_cross_module(action) &&
	     ( mfobj_has_flags(action, CGenFlagIsDuplicateDef) ||
	       (!mfobj_has_flags(action, CGenFlagWriteDefToProjFile))
	     )
	   )
	{
	    continue;
	}

	actinfo = &(action->info.action);
	func_name = abmfP_get_action_name(action);
	if (!util_xor(actinfo->auto_named, auto_named))
	{
	    abmfP_write_action_func_decl(genCodeInfo, action);
	    abio_puts(genCodeInfo->code_file, nlstr);
	}
    }
    trav_close(&trav);
    return 0;
}


/*
 * Traverse object list and write out connection decls. (Functions
 * that have been automatically generated and named)
 */
static int
write_connection_decls(GenCodeInfo genCodeInfo, ABObj project)
{
    return write_user_or_auto_decls(genCodeInfo, project, TRUE);
}


/*
 * Traverse object list and write out callback decls. (Functions that
 * have been named by the user)
 */
static int
write_callback_decls(GenCodeInfo genCodeInfo, ABObj project)
{
    return write_user_or_auto_decls(genCodeInfo, project, FALSE);
}


static int
write_app_resource_struct(
    GenCodeInfo	genCodeInfo, 
    ABObj	project
)
{
    File	codeFile;

    if (!genCodeInfo || !project)
	return (0);

    codeFile = genCodeInfo->code_file;

    abio_puts(codeFile,"\n");
    abmfP_write_c_comment(genCodeInfo, 
	FALSE, "Structure to store values for Application Resources");

    abio_puts(codeFile,"typedef struct {\n");
    abio_indent(codeFile);
    abio_puts(codeFile,"char	*session_file;\n");
    abio_puts(codeFile,"\n");
    abmfP_write_user_struct_fields_seg(genCodeInfo);
    abio_outdent(codeFile);
    abio_puts(codeFile, "} DtbAppResourceRec;\n");

    abio_puts(codeFile,"\n");

    return (0);
}

static int
write_app_resource_var_extern(
    GenCodeInfo	genCodeInfo, 
    ABObj	project
)
{
    File	codeFile;

    if (!genCodeInfo || !project)
	return (0);

    codeFile = genCodeInfo->code_file;

    abio_puts(codeFile,"\n");
    abio_puts(codeFile,"extern DtbAppResourceRec\tdtb_app_resource_rec;\n");

    abio_puts(codeFile,"\n");

    return (0);
}

static int
write_i18n_declarations(
    GenCodeInfo	genCodeInfo,
    ABObj	project
    )
{
    File	codeFile;
    STRING      *p = NULL;
    int		ret_val = 0;

    if (genCodeInfo == NULL)
        goto cret;

    codeFile = genCodeInfo->code_file;

    /*
     * Write out the include directives
     */
    for (p = I18n_Includes; *p; p++)
    {
	abio_printf(codeFile, "#include %s\n", *p);
    }

    /*
     * Write out the macro for the name of the catalog
     */
    abio_printf(codeFile, "#define DTB_PROJECT_CATALOG\t\"%s\"\n",
                obj_get_name(project));

    abio_puts(codeFile,
              "/* Handle for standard message catalog for the project */\n");
    
    abio_puts(codeFile, "extern nl_catd\tDtb_project_catd;\n");
    
cret:
    return(ret_val);
}
