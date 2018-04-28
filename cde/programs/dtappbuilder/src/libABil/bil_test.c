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
 *	$XConsortium: bil_test.c /main/3 1995/11/06 18:26:50 rswiston $
 *
 *      @(#)bil_test.c	1.16 11 Feb 1994 cde_app_builder/src/libABil
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


/****************************************************************
*                                                           	*
* test_bil.c - BIL test driver					*
*                                                           	*
*****************************************************************/

#include <stdio.h>
#include <string.h>
#include <nl_types.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ab_private/util.h>
#include <ab_private/obj.h>
#include <ab_private/trav.h>
#include <ab_private/bil.h>
#include "loadP.h"

nl_catd	Dtb_project_catd = (nl_catd)NULL; /* normally, defined in dtcodegen */

typedef struct
{
    int		verbosity;
    BOOL	dump_tree;
    char	*fileName;
    BOOL	import;
    BOOL	save;
    BOOL	save_encapsulated;
} PARAMETERS;

static int parse_args(int argc, char *argv[], PARAMETERS *params);
static int	usage(STRING msg);

int
main (int argc, char *argv[])
{
    PARAMETERS	params;
    ABObj	newProject= NULL;
    AB_TRAVERSAL	trav;
    ABObj	file= NULL;
    int		iRC = 0;		/* return code */
    STRING	errmsg = NULL;
    ABObj	module = NULL;
    AB_TRAVERSAL	moduleTrav;
    ABObjList		fileObjList = NULL;
    
    util_init(&argc, &argv);
    parse_args(argc, argv, &params);

    util_set_verbosity(params.verbosity);
    if (params.import)
    {
	newProject= obj_create(AB_TYPE_PROJECT, NULL);
	obj_set_name(newProject, "untitled");
	bil_load_file(params.fileName, NULL, newProject, &fileObjList);
    }
    else
    {
        newProject= bil_load_file(params.fileName, NULL, NULL, &fileObjList);
        if (newProject != NULL)
        {
            for (trav_open(&trav, newProject, AB_TRAV_FILES);
	        (file= trav_next(&trav)) != NULL; )
            {
	        bil_load_file(
		    obj_get_name(file), (FILE*)NULL, newProject, &fileObjList);
	        obj_set_flag(file, BeingDestroyedFlag);
            }
	    trav_close(&trav);
            obj_tree_destroy_flagged(newProject);
        }
    }

    iRC = obj_tree_verify(newProject);
    util_printf("verify: %d\n", iRC);
    if (iRC < 0)
    {
	exit(1);
    }

    if (params.dump_tree)
    {
        obj_tree_print(newProject);
    }

    if (params.save)
    {
        mkdir("tmp", S_IRWXU| S_IRGRP|S_IXGRP | S_IROTH|S_IXOTH);
	if (chdir("tmp") != 0)
	{
	    util_printf_err("Could not create and cd to ./tmp\n");
	    exit(1);
	}

	if (params.save_encapsulated)
	{
	    /* encapsulated save */
	    STRING	outFileName = "test.bix";
	    util_printf("Saving project [ENCAPSULATED] to ./tmp\n");
	    unlink(outFileName);
	    iRC = bil_save_tree(newProject, outFileName,
						BIL_SAVE_ENCAPSULATED);
	}
	else
	{
	    /* "ordinary" save */
  	    util_printf("Saving project to ./tmp\n");
	    if (obj_get_file(newProject))
	    {
		unlink(obj_get_file(newProject));
	    }
	    iRC = bil_save_tree(newProject, obj_get_file(newProject), 
					BIL_SAVE_FILE_PER_MODULE);
	    if (iRC >= 0)
	    {
	        for (trav_open(&moduleTrav, newProject, AB_TRAV_MODULES);
		    (module = trav_next(&moduleTrav)) != NULL; )
	        {
	    	    if (obj_get_file(module))
	    	    {
		        unlink(obj_get_file(module));
	    	    }
		    iRC = bil_save_tree(module, NULL, BIL_SAVE_FILE_PER_MODULE);
		}
	    }
	    trav_close(&moduleTrav);
	}

	chdir("..");
    }


    objlist_destroy(fileObjList);
    return 0;
}


static int
usage(STRING msg)
{
    fprintf(stderr, "%s",
"-d = dump\n"
"-v[vvv...] = verbosity\n"
"-h = help\n"
"-i = import\n"
"-s = save\n"
"-x = save encapsulated\n"
);

    if (msg != NULL)
    {
	fprintf(stderr, "\n%s\n", msg);
    }

    exit(0);
    return 0;
}

static int
parse_args(int argc, char *argv[], PARAMETERS *params)
{
    int iArg;
    BOOL	verbosity_seen= FALSE;
    BOOL	dump_tree_seen = FALSE;

    if (argc < 2)
    {
	usage("No filename specified");
    }

    params->fileName= NULL;
    params->verbosity= 3;
    params->dump_tree = FALSE;
    params->import= FALSE;
    params->save = FALSE;
    params->save_encapsulated = FALSE;

    for (iArg= 1; iArg < argc; ++iArg)
    {
	char	*szArg= argv[iArg];
	int	iArgLen= strlen(szArg);

        if (szArg[0] == '-')
        {
	    int iFlag;
	    for (iFlag= 0; iFlag < iArgLen; ++iFlag)
	    {
		char cFlag= szArg[iFlag];
		switch (cFlag)
		{
			case 'd':
			    params->dump_tree = TRUE;
			break;

			case 'v': 
			    if (!verbosity_seen)
			    {
				params->verbosity= 1;
			    }
			    else
			    {
				++(params->verbosity);
			    }
			break;

			case 'h':
			    usage(NULL);
			    break;

			case 'i':	/* import */
			   params->import= TRUE;
			   break;

			case 's':
			    params->save = TRUE;
			    break;

			case 'x':
			    params->save = TRUE;
			    params->save_encapsulated = TRUE;
			    break;
		}
	    }
	}
	else
	{
		/*
		 * not a flag arg
		 */
		params->fileName= szArg;
	}
    }

    if (! (   params->import 
	   || params->dump_tree
	   || params->save)
       )
    {
	/* no action specified - default to dump */
	params->dump_tree = TRUE;
    }

    return 0;
}


