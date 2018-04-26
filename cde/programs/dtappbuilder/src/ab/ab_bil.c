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
 *	$XConsortium: ab_bil.c /main/3 1995/11/06 17:12:37 rswiston $
 *
 * @(#)ab_bil.c	1.85 22 May 1995
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
 * File: ab_bil.c - functions dealing with bil files
 */
#include <stdio.h>
#include <sys/times.h>
#include <time.h>
#include <sys/param.h>		/* MAXPATHLEN */
#include <errno.h>
#include <ab_private/obj.h>
#include <ab_private/trav.h>
#include <ab_private/bil.h>
#include <ab_private/objxm.h>
#include <ab_private/abobj.h>
#include <ab_private/abobj_set.h>
#include <ab_private/brws.h>
#include <ab_private/proj.h>
#include <ab_private/conn.h>
#include <ab_private/ui_util.h>
#include <ab_private/ab.h>
#include <ab_private/ab_bil.h>
#include "../libABobj/objP.h"
#include "palette_ui.h"

/* REMIND: #ifdef DEBUG */
typedef struct
{
    double	startSeconds;
    double	endSeconds;
    double	elapsedSeconds;
} ABTimedIntervalRec, *ABTimedInterval;

typedef struct
{
    ABTimedIntervalRec	realTime;
    ABTimedIntervalRec	CPUTime;
} ABProfiledIntervalRec, *ABProfiledInterval;

static int get_cur_times(double *realTimeOut, double *cpuTimeOut);
static int get_start_times(ABProfiledInterval);
static int get_end_times(ABProfiledInterval);
static int calc_elapsed_times(ABProfiledInterval);
static int print_load_stats(
		ABObj			tree,
		ABProfiledInterval	totalTime,
		ABProfiledInterval	loadTime,
		ABProfiledInterval	configTime
		);

char	Buf[MAXPATHLEN];	/* Work buffer */

/*
 * Loads in a new project, replacing the current project.
 * Chdirs to the directory where the project is. Called 
 * from the 'File->Open Project' and 'Project->Open'
 * (Project Organizer) menus, and the cmd-line (dtbuilder.c).
 */
int 
ab_load_bil_file(
    STRING	fileName,
    FILE	*inFile,
    BOOL	BufferDrop
)
{
    int				rc = 0;		/* return code */
    ABObj			newProject = NULL;
    ABObj			mod = NULL;
    STRING			init_msg = NULL;
    STRING			modfile = NULL;
    int				pLinesRead = 0;
    ABProfiledIntervalRec	totalTime;
    ABProfiledIntervalRec	loadTime;
    ABProfiledIntervalRec	configTime;
    XmString			xm_buf = (XmString) NULL;
    int                 	NoWrite = 0;
    int          		len = 0;
    STRING       		errmsg = NULL;
    BOOL			read_OK, write_OK;
    AB_TRAVERSAL		trav;

    get_start_times(&totalTime);

    get_start_times(&loadTime);
    rc = bil_load_file_and_resolve_all(fileName, inFile, &newProject);
    if (newProject == NULL)
    {
        return rc;
    }

    /* If BufferDrop is TRUE, then that means that
     * ab_load_bil_file() is being called due to
     * a buffer drop (i.e. a .bix dtmail attachment).
     * In that case, we don't need to check whether
     * the modules are read-only, since there aren't
     * .bil files associated with the modules.
     */
    if (!BufferDrop)
    {
        /* Check if any of the modules are read-only. If so, post
         * a message.
         */
	for (trav_open(&trav, newProject, AB_TRAV_MODULES);
	    (mod = trav_next(&trav)) != NULL; )
	{
	    if (!obj_is_defined(mod))
	    {
	       continue;
	    }
	
    	    modfile = obj_get_file(mod);
            abio_access_file(modfile, &read_OK, &write_OK);
            if (!write_OK && !util_strempty(modfile))
            {
	        obj_set_read_only(mod, TRUE);

	        len = len + strlen(modfile) + 2;
                /*
                ** Is this the first read-only file we've encountered? 
		** If so, initialize the notice message.
                */
                if (NoWrite == 0)
                {
		    /* The first thing in the message is 
		     * some general text. 
		     */
		    init_msg = catgets(Dtb_project_catd, 100, 38, "The following modules are read-only.\nIf you edit these modules, you\nwill not be able to save your edits.");

		    /* If we have an old buffer lying around, free it */
		    if (errmsg != (STRING) NULL) 
		        util_free(errmsg);
 
		    /* Now create a new buffer of the proper size */
		    len = strlen(init_msg) + strlen(modfile) + 3;
		    errmsg = (STRING) util_malloc(len);
 
		    /*
		    ** Put the general text plus name of the first 
		    ** read-only file into the notice message.
		    */
		    sprintf(errmsg, "%s\n\n%s", init_msg, modfile);
	        }
	        else
                {
		    errmsg = (STRING) realloc(errmsg, len);
		    strcat(errmsg, "\n");
		    strcat(errmsg, modfile);
	        }
	        NoWrite++;
	    }
	}
    }

    if (!util_strempty(errmsg))
    {
	util_set_help_data(catgets(Dtb_project_catd, 100, 92,
	    "Any changes made to a read-only module cannot be saved,\nunless you save the module to a different file name."),
	    NULL, NULL);
	util_puts(errmsg);
	util_free(errmsg);
    }
    get_end_times(&loadTime);

    if( obj_get_num_children(newProject) == 0 )
    {
	sprintf(Buf, catgets(Dtb_project_catd, 100, 23,
		"%s: Empty project file loaded."), fileName);
	xm_buf = XmStringCreateLocalized(Buf);
	dtb_palette_empty_proj_msg_initialize(&dtb_palette_empty_proj_msg);
	(void)dtb_show_modal_message(dtb_get_toplevel_widget(),
		&dtb_palette_empty_proj_msg, xm_buf, NULL, NULL);
	XmStringFree(xm_buf);
    }

    if (util_get_verbosity() >= 5)
        obj_tree_print(newProject);

    /* Destroy the old project, first. Then initialize
     * the project to be the one just created.  This
     * also sets the current module to NULL, since a
     * module has not been shown yet.
     */
    proj_destroy_project(proj_get_project());
    proj_set_project(newProject);

/* REMIND: Check this out later */

    get_start_times(&configTime);
    objxm_tree_configure(newProject, OBJXM_CONFIG_BUILD);
    get_end_times(&configTime);

    abobj_update_proj_name(newProject);
    abobj_update_palette_title(newProject);
    obj_tree_update_clients(newProject);

#ifdef DEBUG
    get_end_times(&totalTime);
    print_load_stats(newProject, &totalTime, &loadTime, &configTime);
#endif /* DEBUG */


    return 0;
}


int 
ab_import_bil_file(
    STRING	fileName,
    FILE	*inFile,
    BOOL	ImportByCopy
)
{
    int			return_value = 0;
    AB_TRAVERSAL	trav;
    ABObj		project = proj_get_project();
    ABObj		module = NULL;
    ABObjList		loaded_comp_objs = NULL;
    ABObj		loaded_module = NULL;
    STRING		file = NULL;

    /*
     * XmConfigured flags is not getting set properly.  If it's already
     * here, we'll assume it's configured.
     */
    for (trav_open(&trav, project, AB_TRAV_MODULES);
	(module= trav_next(&trav)) != NULL; )
    {
	obj_set_flag(module, XmConfiguredFlag);
    }
    trav_close(&trav);

    if (bil_load_file(fileName, inFile, project, &loaded_comp_objs)
	== NULL)
    {
	return_value = -1;
	goto epilogue;
    }

    /* loaded_module will point to the module that was
     * created.
     */
    loaded_module = NULL;
    if (objlist_get_num_objs(loaded_comp_objs) >= 1)
    {
        loaded_module = objlist_get_obj(loaded_comp_objs, 0, NULL);
    }

    if (loaded_module != NULL)
    {
	if (ImportByCopy)
	{
	    obj_set_file(loaded_module, (String) NULL);
	}
	else	/* Import by reference */
	{
	    /* Convert the imported module's file field to a
	     * path that is relative to the directory in which
	     * the project is stored.
	     */
	    file = proj_cvt_mod_file_to_rel_path(fileName,
			obj_get_file(project));
	    obj_set_file(loaded_module, file);
	    util_free(file);
	}

	/*
	 * configure anything that's not marked as configured
	 */
	abobj_show_tree(loaded_module, TRUE);
        obj_tree_update_clients(loaded_module);
        proj_set_cur_module(loaded_module);
    }

epilogue:
    objlist_destroy(loaded_comp_objs);
    return return_value;
}


static int
get_start_times(ABProfiledInterval interval)
{
    return get_cur_times(&(interval->realTime.startSeconds),
		  	 &(interval->CPUTime.startSeconds));
}


static int
get_end_times(ABProfiledInterval interval)
{
    return get_cur_times(&(interval->realTime.endSeconds),
		  	 &(interval->CPUTime.endSeconds));
}


static int
calc_elapsed_times(ABProfiledInterval interval)
{
    interval->realTime.elapsedSeconds =
	interval->realTime.endSeconds - interval->realTime.startSeconds;
    interval->CPUTime.elapsedSeconds =
	interval->CPUTime.endSeconds - interval->CPUTime.startSeconds;
    return 0;
}


/* REMIND: #ifdef DEBUG */
static int
print_load_stats(
	ABObj			tree,
	ABProfiledInterval	totalTime,
	ABProfiledInterval	loadTime,
	ABProfiledInterval	configTime
)
{
    ABProfiledIntervalRec	otherTimeRec;
    ABProfiledInterval		otherTime = &otherTimeRec;

    /*
     * Print out statistics about load
     */
    calc_elapsed_times(totalTime);
    calc_elapsed_times(loadTime);
    calc_elapsed_times(configTime);
    otherTime->realTime.elapsedSeconds = 
		(totalTime->realTime.elapsedSeconds
			- loadTime->realTime.elapsedSeconds
			- configTime->realTime.elapsedSeconds);
    otherTime->CPUTime.elapsedSeconds = 
		(totalTime->CPUTime.elapsedSeconds
			- loadTime->CPUTime.elapsedSeconds
			- configTime->CPUTime.elapsedSeconds);

    fprintf(stderr, "\nLoad Times (Real/CPU) seconds:\n");
    fprintf(stderr, "          Total: (%lg/%lg)\n", 
	totalTime->realTime.elapsedSeconds,
	totalTime->CPUTime.elapsedSeconds);
    fprintf(stderr, "    Actual load: (%lg/%lg)\n",
	loadTime->realTime.elapsedSeconds,
	loadTime->CPUTime.elapsedSeconds);
    fprintf(stderr, "         Config: (%lg/%lg)\n",
	configTime->realTime.elapsedSeconds,
	configTime->CPUTime.elapsedSeconds);
    fprintf(stderr, "          Other: (%lg/%lg)\n",
	otherTime->realTime.elapsedSeconds,
	otherTime->CPUTime.elapsedSeconds);

    fprintf(stderr, "Number of objects loaded - salient:%d   total:%d\n",
	trav_count(tree, AB_TRAV_SALIENT),
	trav_count(tree, AB_TRAV_ALL));

    return 0;
}
/* REMIND: #endif DEBUG */


static int
get_cur_times(double *realTimeOut, double *cpuTimeOut)
{
    static BOOL	initialized = FALSE;
    static long	ticks_per_second = 1;
    struct tms  timeInfo;
    double      realTime;
    double      cpuTime;
    if (!initialized)
    {
	initialized = TRUE;
        ticks_per_second = sysconf(_SC_CLK_TCK);
    }

    realTime = times(&timeInfo);
    cpuTime = timeInfo.tms_utime + timeInfo.tms_stime
                        + timeInfo.tms_cutime + timeInfo.tms_cstime;
    *realTimeOut  = realTime / ticks_per_second;
    *cpuTimeOut = cpuTime / ticks_per_second;

    return 0;
}


/* Called by the 'File->Open Project' callback and the 
 * 'Project->Open' callback.  NOT called when a project
 * is loaded from the command-line.
 */
int
ab_check_and_open_bip(
    STRING	fileName
)
{
    DTB_MODAL_ANSWER    answer = DTB_ANSWER_NONE;
    BOOL                read_OK, write_OK;
    int			iRet = 0;
    XmString		xm_buf = (XmString) NULL;
    DtbObjectHelpData   help_data = NULL;

    abio_access_file(fileName, &read_OK, &write_OK);
    if (read_OK)
    {
	if (write_OK)
	{
	    iRet = ab_load_project(fileName, NULL, FALSE);
	}
	else	/* Read-only file */
	{
    	    /* You can read it but you can't write to it. */
            sprintf(Buf, catgets(Dtb_project_catd, 100, 17,
		"The file %s is a read-only file.\nYou may open the project\nor cancel the operation."), fileName);
	    xm_buf = XmStringCreateLocalized(Buf);
	    dtb_palette_open_ro_proj_msg_initialize(
			&dtb_palette_open_ro_proj_msg);

            help_data = (DtbObjectHelpData) util_malloc(sizeof(DtbObjectHelpDataRec));
            help_data->help_text = catgets(Dtb_project_catd, 100, 90,
		"Opening a read-only project means that any changes that\naffect the project (.bip) file, such as creation of a\nnew module or creation of a cross-module connection,\ncannot be saved, unless you save the project to a\ndifferent file name. You have the option of continuing\nwith the open operation or cancelling it.");
            help_data->help_volume = "";
            help_data->help_locationID = "";
	    
            answer = dtb_show_modal_message(dtb_get_toplevel_widget(),
                &dtb_palette_open_ro_proj_msg, xm_buf, help_data, NULL);

	    util_free(help_data);
	    XmStringFree(xm_buf);
	    switch (answer)
	    {
		    case DTB_ANSWER_ACTION1:
			iRet = ab_load_project(fileName, NULL, FALSE);
			break;

        	    case DTB_ANSWER_CANCEL:
			break;

        	    case DTB_ANSWER_HELP:
			break;
	    }
	}
    }
    else 
    {
	sprintf(Buf, catgets(Dtb_project_catd, 100, 20,
		"%s does not have read permission."), fileName);
        xm_buf = XmStringCreateLocalized(Buf);
        dtb_palette_error_msg_initialize(&dtb_palette_error_msg);
	(void)dtb_show_modal_message(dtb_get_toplevel_widget(),
			&dtb_palette_error_msg, xm_buf, NULL, NULL);
	XmStringFree(xm_buf);
	iRet = -1;
    }

    return (iRet); 
}

int
ab_load_project(
    STRING	fileName,
    FILE	*inFile,
    BOOL	BufferDrop
)
{
    int		ret = 0;

    ab_set_busy_cursor(TRUE);
    ui_sync_display_of_widget(AB_toplevel);
    
    if ((ret = ab_load_bil_file(fileName, inFile, BufferDrop)) != -1)
    {
	proj_show_proj_dir();
	objxm_tree_configure(proj_get_project(), OBJXM_CONFIG_BUILD);
	/* Popup the Project Organizer */
	proj_show_dialog();
    }

    ab_set_busy_cursor(FALSE);

    return (ret);
}

/* Calls ab_import_bil_file().  Called by the File->Import->Module
 * callback and the Module->Import callback.  NOT called when
 * a project is loaded from the command-line.
 */
int
ab_check_and_import_bil(
    STRING	fileName,
    BOOL	ImportByCopy
)
{
    BOOL                read_OK, write_OK;
    DTB_MODAL_ANSWER    answer = DTB_ANSWER_NONE;
    ABObj       	project = proj_get_project();
    int			iRet = 0;
    XmString		xm_buf = (XmString) NULL;
    DtbObjectHelpData	help_data = NULL;

    abio_access_file(fileName, &read_OK, &write_OK);
    if (read_OK)
    {
	if (ImportByCopy)
	{
		iRet = ab_import_module(fileName, NULL, TRUE);
	}
	else	/* Importing by Reference. Have to check permissions. */
	{
	    if (write_OK)
	    {
		/* The file is readable and writable, can be opened, 
		 * and is a valid module file, so import it.
		 */
		iRet = ab_import_module(fileName, NULL, FALSE);
	    }
	    else
	    {
        	/* You can read it but you can't write to it. */
        	sprintf(Buf, catgets(Dtb_project_catd, 100, 21,
			    "The file %s is a read-only file.\n\
			    You may import the module or cancel\n\
			    the operation."), fileName);
		xm_buf = XmStringCreateLocalized(Buf);
		dtb_palette_import_ro_msg_initialize(
				&dtb_palette_import_ro_msg);

                help_data = (DtbObjectHelpData) util_malloc(sizeof(DtbObjectHelpDataRec));
                help_data->help_text = catgets(Dtb_project_catd, 100, 91,
		    "Importing a read-only module means that any changes\nthat affect the module cannot be saved, unless you\nsave the module to a different file name. You have\nthe option of continuing with the import operation\nor cancelling it.");

                help_data->help_volume = "";
                help_data->help_locationID = "";

        	answer = dtb_show_modal_message(dtb_get_toplevel_widget(),
			&dtb_palette_import_ro_msg, xm_buf, help_data, NULL);

		util_free(help_data);
		XmStringFree(xm_buf);
		switch (answer)
		{
		    case DTB_ANSWER_ACTION1:
			iRet = ab_import_module(fileName, NULL, ImportByCopy);
			break;

        	    case DTB_ANSWER_CANCEL:
		    case DTB_ANSWER_HELP:
			break;
		}
	    }
	}
    }
    else
    {
	sprintf(Buf, catgets(Dtb_project_catd, 100, 9,
		"%s does not have read permission."), fileName);
        xm_buf = XmStringCreateLocalized(Buf);  
        dtb_palette_error_msg_initialize(&dtb_palette_error_msg);
	(void)dtb_show_modal_message(dtb_get_toplevel_widget(),
			&dtb_palette_error_msg, xm_buf, NULL, NULL);
        XmStringFree(xm_buf);
	iRet = -1;
    }

    return (iRet);
}

int
ab_import_module(
    STRING	fileName,
    FILE	*inFile,
    BOOL	ImportByCopy
)
{
    int		iRet = 0;

    ab_set_busy_cursor(TRUE);
    ui_sync_display_of_widget(AB_toplevel);

    iRet = ab_import_bil_file(fileName, inFile, ImportByCopy);
    if (iRet != -1)
    {
        abobj_set_save_needed(proj_get_project(), TRUE);
    }
    ab_set_busy_cursor(FALSE);

    return (iRet);
}
