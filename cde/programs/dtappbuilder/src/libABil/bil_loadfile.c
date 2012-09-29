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
 * $TOG: bil_loadfile.c /main/4 1998/04/06 13:12:27 mgreess $
 * 
 * @(#)bil_loadfile.c	1.53 02 Apr 1995
 * 
 * RESTRICTED CONFIDENTIAL INFORMATION:
 * 
 * The information in this document is subject to special restrictions in a
 * confidential disclosure agreement between HP, IBM, Sun, USL, SCO and
 * Univel.  Do not distribute this document outside HP, IBM, Sun, USL, SCO,
 * or Univel without Sun's specific written approval.  This document and all
 * copies and derivative works thereof must be returned or destroyed at Sun's
 * request.
 * 
 * Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 * 
 */


/*
 * Routines to load in GUIDE intermediate language (BIL) files.
 */

#include <sys/param.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ab_private/util.h>
#include <ab_private/abio.h>
#include <ab_private/trav.h>
#include <ab_private/bil.h>
#include "../libABobj/obj_utils.h"
#include "load.h"
#include "bilP.h"

BIL_LOAD_INFO	bilP_load;
char		Buf[MAXPATHLEN];
#if !defined(linux) && !defined(CSRG_BASED)
extern char     *sys_errlist[];
#endif

/*
 * Read a BIL file into memory. Returns a project node.
 * 
 * Any modules found in the BIL file are put into the project that is passed in.
 * Any project objects in the BIL file are ignored. This allows modules to be
 * imported from an encapsulated bil file.  The project passed in will be the
 * one returned.
 * 
 * If the project passed in is NULL, then it is assumed that the first object
 * encountered will be a project.  This project will be used to store modules
 * (if any) that are also found in the file.
 * 
 * 
 * File name extensions ===================== All BIL files are identical in
 * format. No file name extensions are enforced, but the standard extensions
 * are as follows:
 * 
 * .bil = bil module file, contains one module
 * 
 * .bip = bil project file. contains one project definition and references to
 * constituent .bil files
 * 
 * .bix = encapsulated bil file.  contains all project and module definitions
 * for one project.
 * 
 */
ABObj
bil_load_file(
    char 	*bil_file,
    FILE	*inFile,
    ABObj 	project,
    ABObjList	*compObjsOutPtr
)
{
#define compObjsOut (*compObjsOutPtr)
    ABObj               obj = NULL;
    STRING              errmsg = NULL;
    AB_FILE_TYPE        file_type = AB_FILE_UNDEF;
    int                 success = OK;
    int                 i;
    int                 lineNumber = 0;
    BOOL                unknownObjects = FALSE;
    BOOL		LoadErr = FALSE;
    float		ver = 0.0;

    if (util_strempty(bil_file) && inFile == NULL)
    {
	return NULL;
    }

    if (util_be_verbose() && (!util_strempty(bil_file)))
    {
	util_printf("reading %s\n", bil_file);
    }

    /*
     * Reset everything
     */
    bilP_load.project = NULL;
    bilP_load.module = NULL;
    bilP_load.compObjsRead = NULL;
    bilP_load.obj = NULL;
    bilP_load.objType = AB_TYPE_UNDEF;
    bilP_load.fileName = NULL;
    abil_loadmsg_clear();
    abil_loadmsg_set_line_number_callback(bilP_load_get_line_number);

    /*
     * set up for load
     */
    bilP_load.fileName = istr_create(bil_file);
    bilP_load.project = project;
    bilP_load.compObjsRead = objlist_create();
    abil_loadmsg_set_file(istr_string(bilP_load.fileName));

    /* If the FILE * is NULL, then call abio_open_bil_input().
     * inFile will be NULL if this routine is being called
     * by dtcodegen (abmf.c) to load a project or module file.
     */
    if (inFile == NULL)
    {
	/* Open the input file.  */
	errmsg = abio_open_bil_input(bil_file, &lineNumber, &AByyin);
	if (errmsg != NULL)
	{
	    LoadErr = TRUE;
	    goto epilogue;
        }
    }
    else
    {
	/* Read in header information */
	ver = abio_get_bil_version(inFile, &lineNumber);
	errmsg = abio_check_bil_version(ver, bil_file);
	if (errmsg != NULL)
	{
	    LoadErr = TRUE;
	    goto epilogue;
        }
	else
	    AByyin = inFile;
    }
    ++lineNumber;

    /*
     * Parse the file.
     */
    bilP_load_reset();
    bilP_load_set_line_number(lineNumber);
    if ((success = AByyparse()) != 0)
    {
	abio_close_input(inFile);
	LoadErr = TRUE;
	goto epilogue;
    }

#ifdef BOGUS	/* the commented out util_error() below makes this loop
		   unnecessary -dunn */
    /*
     * make sure there are no unknown object types in the tree.
     */
    unknownObjects = FALSE;
    {
	AB_TRAVERSAL        trav;
	for (trav_open(&trav, bilP_load.project, AB_TRAV_ALL), i = 0;
	     (obj = trav_next(&trav)) != NULL; ++i)
	{
	    if (obj->type == AB_TYPE_UNKNOWN)
	    {
		unknownObjects = TRUE;
		break;
	    }
	}
	trav_close(&trav);
	if (unknownObjects)
	{
	    /* util_error("unknown type objects after load."); */
	}
    }
#endif /* BOGUS */

    /* Close the input file. */
    abio_close_input(AByyin);

    if (bilP_load.project == NULL)
    {
	LoadErr = TRUE;
	sprintf(Buf, catgets(ABIL_MESSAGE_CATD, ABIL_MESSAGE_SET, 39,
		"Unable to load project."));
	errmsg = Buf;
	goto epilogue;
    }


    /*
     * Update the returned list of composite objects
     */
    if (compObjsOutPtr != NULL)
    {
	int	numObjs = objlist_get_num_objs(bilP_load.compObjsRead);
	int	i;

	if (compObjsOut == NULL)
	{
	    compObjsOut = objlist_create();
	}

	for (i = 0; i < numObjs; ++i)
	{
	    objlist_add_obj(compObjsOut, 
			objlist_get_obj(bilP_load.compObjsRead, i, NULL), NULL);
	}
    }


epilogue:
    objlist_destroy(bilP_load.compObjsRead);
    if (LoadErr)
    {
	if (errmsg != NULL)
	{
	    util_printf_err("%s\n", errmsg);
	}
	else 	
	{
	    /* It was a yacc generated error, thus it gets
	     * printed out via yyerror() (see bil_lex.l),
	     * so reset the printed boolean (see load.c).
	     */
	    abil_loadmsg_set_err_printed(FALSE);
	}
	
	/* There was an error reading in the project/module
	 * file.  If this routine was called to read in a
	 * module file, then bilP_load.module will not be NULL,
	 * and it will be destroyed here. If this routine was
	 * called to read in a project file, then it will
	 * simply return an error value, and the project will
	 * be destroyed in bil_load_file_and_resolve_all().
	 * Therefore, if a corrupt bip file is read in, the 
	 * project will NOT be loaded into dtbuilder.  If a 
	 * corrupt bil file is imported, only the module created 
	 * for the corrupt bil file will be destroyed. If the 
	 * corrupt bil file was part of an existing project, then 
	 * the project referencing it will be destroyed in 
	 * bil_load_file_and_resolve_all().
	 *
	 * Note:  It is possible that bilP_load.module will not
	 * 	  be NULL when reading in a bip file and that is
	 *	  if the ending ')' for ":project" is not in the
	 *	  bip file for some reason.  So, we should check
	 * 	  for that case, otherwise we'll be destroying
	 *	  the same object, the project, twice - not good. 
	 */
	if ((bilP_load.module != NULL) && (bilP_load.module != bilP_load.project))
	{
	    obj_destroy(bilP_load.module);
	    bilP_load.module = NULL;
	} 
	return NULL;
    }
    else
    {
	return bilP_load.project;
    }
#undef compObjsOut
}				/* bil_load_file */

/*
 * Loads in a new project, and returns a pointer to it.  Chdirs to the
 * directory where the project is. Loads in all files referenced by the
 * project
 */
int
bil_load_file_and_resolve_all(
    STRING	path,
    FILE	*inFile,
    ABObj	*newProjectPtr
)
{
#define newProject (*newProjectPtr)
    int			return_value = 0;
    AB_TRAVERSAL        trav;
    ABObj               file = NULL;
    ABObj		ret = NULL;
    char                dirName[MAXPATHLEN];
    char		fileName[MAXPATHLEN];
    STRING		modfile = NULL;
    *dirName = 0;
    *fileName = 0;
    newProject = NULL;

    /* If the path is NULL then that means that a 
     * data buffer was dropped (i.e. dtmail attachment)
     * on dtbuilder and therefore, there is no file. 
     */ 
    if (!util_strempty(path))
    {
	util_get_dir_name_from_path(path, dirName, MAXPATHLEN);
	util_get_file_name_from_path(path, fileName, MAXPATHLEN);
	if (chdir(dirName) != 0)
	{
	    util_printf_err("%s: %s\n", dirName, sys_errlist[errno]);
	    return ERR;
	}
    }

    newProject = bil_load_file(fileName, inFile, NULL, NULL);
    /* If bil_load_file returns NULL, then something went wrong
     * with reading in the bip file, so destroy the project.
     */
    if (newProject == NULL)
    {
	obj_destroy(bilP_load.project);
	bilP_load.project = NULL;
	return_value = ERR;
	goto epilogue;
    }

    /*
     * Find any references to other files in the project and load them
     */
    for (trav_open(&trav, newProject, AB_TRAV_FILES);
	 (file = trav_next(&trav)) != NULL;)
    {
        modfile = obj_get_file(file);

	/* Pass in NULL for the FILE * argument, because we
	 * haven't opened (util_fopen_locked) the module files yet.
 	 */
	ret = bil_load_file(modfile, NULL, newProject, NULL);

	/* If the return value from bil_load_file is NULL,
	 * then something went wrong with reading in the
	 * bil file (i.e. it was a corrupt bil file).  In this
	 * case, don't load the project, destroy it.
	 */
	if (ret == NULL)
	{
	    return_value = -1;
	    obj_destroy(bilP_load.project);
	    bilP_load.project = NULL;
	    newProject = NULL;
	    break;
	}
	else
	{
	    obj_set_flag(file, BeingDestroyedFlag);
	}
    }
    trav_close(&trav);

epilogue:
    if (ret)
    {
	/* Destroy filename holders */
	obj_tree_destroy_flagged(newProject);
    }

    return return_value;

#undef newProject
}

/******************************************************************
 *
 * Callbacks for parser
 */

int
bilP_load_end_of_file()
{
    bilP_reset_token_text();
    return 0;
}


int
bilP_load_end_of_data()
{
    fseek(AByyin, 0, SEEK_END);		/* move to end of file */
    return 0;
}

