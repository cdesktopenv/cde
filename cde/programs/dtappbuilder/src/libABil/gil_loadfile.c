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
 *	$XConsortium: gil_loadfile.c /main/3 1995/11/06 18:29:14 rswiston $
 */

/*
 * Routines to load in GUIDE intermediate language (GIL) files.
 */

#include <sys/param.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ab_private/util.h>
#include <ab_private/abio.h>
#include <ab_private/trav.h>
#include <ab_private/gil.h>
#include "loadP.h"
#include "load.h"
#include "../libABobj/obj_utils.h"
#include "gilP.h"

/*
 * Read a GIL file into memory.
 */
ABObj
gil_load_file(
              STRING gil_file,
              FILE * inFile,
              ABObj project
)
{
    int                 return_value = 0;
    int                 rc = 0; /* r turn code */
    STRING              errmsg = NULL;
    ABObj               obj = NULL;
    ABObj               interface = NULL;
    AB_FILE_TYPE        file_type = AB_FILE_UNDEF;
    FILE               *gilInFile = NULL;

    /* printf("gil_load_file(%s)\n", gil_file); */

    if (gil_file == NULL)
    {
        return 0;
    }

    abil_loadmsg_clear();
    abil_loadmsg_set_line_number_callback(NULL);	/*don't report line#*/
    abil_loadmsg_set_file(gil_file);

    /*
     * Create the root node for this beastie.
     */
    interface = obj_scoped_find_or_create_undef(project, 
			ab_ident_from_file_name(gil_file), AB_TYPE_MODULE);
    if (interface == NULL)
    {
        abil_print_load_err(ERR_NO_MEMORY);
        goto epilogue;
    }

    obj_set_file(interface, gil_file);
    obj_set_is_defined(interface, TRUE);

    /*
     * Open the input file.
     */
    if (inFile != NULL)
    {
        gilInFile = inFile;
    }
    else
    {
        if ((errmsg = abio_open_gil_input(gil_file, &gilInFile)) != NULL)
        {
            util_error(errmsg);
            return_value = -1;
            goto epilogue;
        }
        if (!util_be_silent())
            fprintf(stderr, "%s: reading %s\n",
                    util_get_program_name(), gil_file);
    }

    /*
     * Find start of first object (should be interface)
     */
    if (abio_get_eof(gilInFile))
    {
        abil_print_load_err(ERR_EOF);
	return_value = -1;
        goto epilogue;
    }
    if (!abio_gil_get_object_begin(gilInFile))
    {
	abil_print_load_err(ERR_WANT_OBJECT);
	return_value = -1;
	goto epilogue;
    }

    /*
     * Loop reading the input file until there are no more objects.
     */
    while (!abio_get_file_end(gilInFile))
    {
        if (abio_get_eof(gilInFile))
        {
            abil_print_load_err(ERR_EOF);
            goto epilogue;
        }

        if (!abio_gil_get_object_begin(gilInFile))
        {
            abil_print_load_err(ERR_WANT_OBJECT);
            goto epilogue;
        }

        if ((rc = gilP_load_object(gilInFile, interface)) < 0)
        {
            return_value = rc;
            goto epilogue;
        }
    }

    if (inFile == NULL)         /* don't close passed-in stream */
    {
        abio_close_input(gilInFile);
    }

epilogue:
    if (return_value < 0)
    {
        if (interface != NULL)
        {
            obj_destroy(interface);
            interface = NULL;
        }
    }
    if (interface != NULL)
    {
	obj_set_is_defined(interface, TRUE);
    }
    return interface;
}                               /* gil_load_file */


/*
 * Loads in a GIL project file - DOES NOT LOAD IN THE INTERFACES IN
 * THE PROJECT FILE.
 */
int
gil_load_project_file(
                      STRING project_file,
                      FILE * inFile,
                      ABObj *projectOutPtr
)
{
    int                 return_value = 0;
    int                 rc = 0; /* r turn code */
    STRING              errmsg = NULL;
    ABObj               project = obj_create(AB_TYPE_PROJECT, NULL);
    FILE               *gilInFile = NULL;

    util_dprintf(1,
                 "gil_load_project_file(%s)\n", util_strsafe(project_file));

    abil_loadmsg_clear();
    abil_loadmsg_set_line_number_callback(NULL);	/*don't report line#*/
    abil_loadmsg_set_file(project_file);

    if (project == NULL)
    {
        abil_print_load_err(ERR_NO_MEMORY);
        goto epilogue;
    }

    project->info.project.is_default = FALSE;   /* spec'd by user */
    obj_set_file(project, strdup(project_file));
    obj_set_name(project, ab_ident_from_file_name(project_file));

    /*
     * Some bonehead decided that a project file should consist of only one
     * object and not have the surrounding parens like a GIL file.  So, we
     * can't use load_interface.
     */
    if (inFile != NULL)
    {
        gilInFile = inFile;
    }
    else
    {
        if ((errmsg = abio_open_gil_input(project_file, &gilInFile)) != NULL)
        {
            util_error(errmsg);
            return_value = -1;
            goto epilogue;
        }
        if (!util_be_silent())
        {
            fprintf(stderr, "%s: reading %s\n",
                    util_get_program_name(), project_file);
        }
    }

    /*
     * Find start of project object
     */
    if (abio_get_eof(gilInFile))
    {
        abil_print_load_err(ERR_EOF);
	return_value = -1;
        goto epilogue;
    }
    if (!abio_gil_get_object_begin(gilInFile))
    {
	abil_print_load_err(ERR_WANT_OBJECT);
	return_value = -1;
	goto epilogue;
    }

    /*
     * The project is it's own parent...
     */
    if ((rc = gilP_load_object2(gilInFile, project, project)) < 0)
    {
        return_value = rc;
        goto epilogue;
    }
    if (inFile == NULL)
    {
        abio_close_input(gilInFile);
    }

epilogue:
    if (return_value < 0)
    {
        obj_destroy(project);
        project = NULL;
    }

    *projectOutPtr = project;
    return return_value;
}


int
gil_load_project_file_and_resolve_all(
                                      STRING project_file,
                                      FILE * inFile,
                                      ABObj *projectOutPtr
)
{
    int                 return_value = 0;
    int                 rc = 0;
    ABObj		project = NULL;
    int			i = 0;
    ABObj		interface = NULL;
    AB_TRAVERSAL	trav;

    if ((rc = gil_load_project_file(project_file, inFile, &project)) < 0)
    {
        return_value = rc;
	goto epilogue;
    }

    /*
     * Load in the interfaces referenced in the project file
     */
    for (trav_open(&trav, project, AB_TRAV_MODULES | AB_TRAV_MOD_SAFE);
	(interface = trav_next(&trav)) != NULL; )
    {
	if (!obj_is_defined(interface))
	{
            if (gil_load_file(obj_get_file(interface), NULL, project) == NULL)
            {
                return_value = -1;
                goto epilogue;
            }
	}
    }
    trav_close(&trav);

epilogue:
    if (return_value < 0)
    {
	obj_destroy(project);
    }
    (*projectOutPtr) = project;
    return return_value;
}

