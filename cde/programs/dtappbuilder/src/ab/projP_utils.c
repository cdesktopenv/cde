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
 *	$XConsortium: projP_utils.c /main/3 1995/11/06 17:46:28 rswiston $
 *
 * @(#)projP_utils.c	1.6 18 Jan 1994 cde_app_builder/src/ab
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
 * File: projP_utils.c - private functions for the project module
 */

#include <stdio.h>
#include <errno.h>
#include <sys/param.h>
#include <Xm/Xm.h>
#include <Xm/FileSB.h>
#include <Xm/TextF.h>
#include <ab_private/ab.h>
#include <ab_private/proj.h>
#include <ab_private/projP.h>
#include <ab_private/obj.h>
#include <ab_private/objxm.h>
#include <ab_private/abobj_set.h>
#include <ab_private/ui_util.h>
#include <ab_private/ab_utils.h>
#include <ab_private/abio.h>
#include <ab_private/bil.h>
#include <ab_private/pal.h>
#include "proj_ui.h"

/*************************************************************************
**                                                                      **
**       Private Function Declarations                                  **
**                                                                      **
**************************************************************************/

static void 	save_as_bil_okCB(
		    Widget              widget,
		    XtPointer           client_data,
		    XmSelectionBoxCallbackStruct *call_data
		);

static void     no_module_selected(
                    DtbProjProjMainInfo proj_d
                );

static void     one_module_selected(
                    DtbProjProjMainInfo proj_d
                );

static void     mult_module_selected(
                    DtbProjProjMainInfo proj_d
                );

/*************************************************************************
**                                                                      **
**       Data     	                                                **
**                                                                      **
**************************************************************************/

#if !defined(linux) && !defined(CSRG_BASED)
extern char	*sys_errlist[];
#endif
char		Buf[MAXPATHLEN];	/* Work buffer */

/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/

/************************************************************
 *  This routine is called by the XmNactivateCallback for the
 *  Module->[Save/Save As] menu item from the Project Organizer.
 *  It creates a file chooser if one does not already exist
 *  and pops it up.
 ***********************************************************/
void
projP_show_save_as_bil_chooser(
    Widget      widget,
    ABObj       obj
)
{
    Widget      	textf;
    char        	init_name[MAXPATHLEN];
    ChooserInfo		info = NULL;
    XmString		ok_label;
    char		*title;
 
    XtCallbackRec ok_callback[] = {
        {(XtCallbackProc)save_as_bil_okCB, (XtPointer) NULL},
        {(XtCallbackProc) NULL, (XtPointer) NULL}
    };
    XtCallbackRec cancel_callback[] = {
        {(XtCallbackProc)proj_cancelCB, (XtPointer) NULL},
        {(XtCallbackProc) NULL, (XtPointer) NULL}
    };
    XtCallbackRec popdown_callback[] = {
        {(XtCallbackProc)proj_unmap_chooserCB, (XtPointer) NULL},
        {(XtCallbackProc) NULL, (XtPointer) NULL}
    };
 
    /* Create the file chooser if it hasn't already
     * been created by anyone else, then set the
     * appropriate resources.
     */  
    if (AB_generic_chooser == NULL)
    {
        AB_generic_chooser = (Widget) XmCreateFileSelectionDialog(
                widget,
                "AB_generic_chooser",
                (ArgList) NULL,
                (Cardinal) 0);

        info = (ChooserInfo)calloc(sizeof(ChooserInfoRec), 1);
        XtVaSetValues(AB_generic_chooser,
                        XmNuserData, (XtPointer) info,
                        NULL);
    }
    else if (XtIsManaged(AB_generic_chooser))
    {
        ui_win_front(AB_generic_chooser);
        return;
    }
 
    XtVaGetValues(AB_generic_chooser,
                        XmNuserData, &info,
                        NULL);
    info->child = (Widget) NULL;
    info->chooser_type = AB_SAVE_MOD_AS_CHOOSER;
 
    /* Pass along the client_data to the okCallback */
    ok_callback[0].closure = (XtPointer) obj;

    ok_label = XmStringCreateLocalized(catgets(Dtb_project_catd, 100, 237, "Save"));
    XtVaSetValues(AB_generic_chooser,
                XmNautoUnmanage, FALSE,
                XtVaTypedArg, XmNpattern,
                        XtRString, "*.bil",
                        sizeof("*.bil"),
                XmNokLabelString, ok_label,
                XmNokCallback, &ok_callback,
                XmNcancelCallback, &cancel_callback,
                NULL);
    XmStringFree(ok_label);

    title = XtNewString(catgets(Dtb_project_catd, 100, 242, "Save BIL File"));
    XtVaSetValues(XtParent(AB_generic_chooser),
                XmNtitle, title,
                XmNpopdownCallback, &popdown_callback,
                NULL);
    XtFree(title);
    textf = XmFileSelectionBoxGetChild(AB_generic_chooser,
                        XmDIALOG_TEXT);
    strcpy(init_name, (char *)obj_get_name(obj));
    strcat(init_name, ".bil");
    XmTextFieldSetString(textf, init_name);
    XtManageChild(AB_generic_chooser);
    ui_win_front(AB_generic_chooser);

    /* Set File and Project Organizer Project and Module menu
     * items inactive, until this file chooser is dismissed.
     */
    pal_set_File_menu(AB_SAVE_MOD_AS_CHOOSER, FALSE);
    proj_set_menus(AB_SAVE_MOD_AS_CHOOSER, FALSE);
}

/*************************************************************
 *  This routine is the XmNokCallback for the Project Organizer
 *  Module->Save As file chooser. It is passed the appropriate
 *  module obj, the one the user wants to save, via client data.
 *************************************************************/
static void
save_as_bil_okCB(
    Widget              widget,
    XtPointer           client_data,
    XmSelectionBoxCallbackStruct *call_data
)
{
    STRING              fullpath = NULL, msg = NULL;
    char		mod_name[MAXPATHLEN];
    char		mod_dir[MAXPATHLEN];
    ABObj               module_obj = (ABObj) client_data;
    XmString            xm_buf = (XmString) NULL;
    BOOL		read_OK = FALSE, write_OK = FALSE;
    int			iRet = 0;
    DtbObjectHelpData	help_data = NULL;

    *mod_name = 0;
    *mod_dir = 0;
    fullpath = (STRING) objxm_xmstr_to_str(call_data->value);

    /*
     * If the file chooser selection text field is empty,
     * return, leaving the chooser up to give the user 
     * another try.
     */
    if (util_strempty(fullpath))
    {
	dtb_proj_no_name_msg_initialize(&dtb_proj_no_name_msg);
	(void)dtb_show_modal_message(dtb_get_toplevel_widget(),
                        &dtb_proj_no_name_msg, NULL, NULL, NULL);
        return;
    }
    if (util_file_is_directory(fullpath))
    {
        sprintf(Buf, catgets(Dtb_project_catd, 100, 73,
                "Cannot save the module to %s.\n%s is a directory not a file."), fullpath, fullpath);
        xm_buf = XmStringCreateLocalized(Buf);
        dtb_proj_error_msg_initialize(&dtb_proj_error_msg);

	help_data = (DtbObjectHelpData) util_malloc(sizeof(DtbObjectHelpDataRec));
	help_data->help_text = catgets(Dtb_project_catd, 100, 87,
	    "The file name you specified for saving the\nmodule is a directory. Type another file name\nfor the module.");
	help_data->help_volume = "";
	help_data->help_locationID = "";

        (void)dtb_show_modal_message(widget,
                        &dtb_proj_error_msg, xm_buf, help_data, NULL);

	util_free(help_data);
        XmStringFree(xm_buf);
        return;
    }

    /* Check if we're allowed to write to the directory
     * specified for the module file.
     */                  
    util_get_dir_name_from_path(fullpath, mod_dir, MAXPATHLEN);
    iRet = abio_access_file(mod_dir, &read_OK, &write_OK);
    if (iRet == 0)
    {
	if (write_OK)
	{
	    util_derive_name_from_path(fullpath, mod_name);
	    if (proj_verify_name(mod_name, TRUE,
		&dtb_proj_save_as_mod_msg, widget, module_obj))
    	    {
		projP_save_export_bil(module_obj, fullpath, FALSE);
		abobj_set_save_needed(module_obj, FALSE);
    	    }
	}
	else
        {
	    sprintf(Buf, catgets(Dtb_project_catd, 100, 76,
		    "Cannot save module to \"%s\".\n\"%s\" does not have write permission."), mod_dir, mod_dir);
	    xm_buf = XmStringCreateLocalized(Buf);
	    dtb_proj_error_msg_initialize(&dtb_proj_error_msg);

            help_data = (DtbObjectHelpData) util_malloc(sizeof(DtbObjectHelpDataRec));
            help_data->help_text = catgets(Dtb_project_catd, 100, 88, 
		"The directory you specified for saving is not\nwritable by you. Therefore, the module cannot\nbe saved. Try saving the module to another\ndirectory.");
            help_data->help_volume = ""; 
            help_data->help_locationID = ""; 

	    (void)dtb_show_modal_message(widget,
                        &dtb_proj_error_msg, xm_buf, help_data, NULL);

	    util_free(help_data);
	    XmStringFree(xm_buf);
	}
    }  
    else
    {
	util_dprintf(1, "Error in accessing \"%s\".", mod_dir);
    }
}

int
projP_save_export_bil(
    ABObj       module_obj,
    STRING      fullpath,
    BOOL        Exporting
)
{
    STRING              old_filename = NULL;
    char                new_filename[MAXPATHLEN];
    DTB_MODAL_ANSWER    answer = DTB_ANSWER_NONE;
    BOOL		read_OK, write_OK;
    int			iRet = 0;
    XmString		xm_buf = (XmString) NULL;

    *new_filename = 0;

    if (util_strempty(fullpath))
        return -1;

    /* Set the busy cursor while writing the bil file to disk */
    ab_set_busy_cursor(TRUE);

    abio_expand_file(fullpath, new_filename);
    old_filename = obj_get_file(module_obj);

    /* If the module has already been saved to a file and it's being
     * saved to the same filename, just overwrite the previous file.
     * Don't bring up an overwrite dialog.
     */  
    if (!Exporting && 
	util_paths_are_same_file(old_filename, new_filename))
    {
	/* Check that the specified file has write permission */
	abio_access_file(old_filename, &read_OK, &write_OK);
	if (write_OK)
	{
            iRet = projP_write_out_bil_file(module_obj, new_filename,
					    NULL, Exporting);
	    if (iRet >= 0)
                obj_clear_flag(module_obj, SaveNeededFlag);
	    XtUnmanageChild(AB_generic_chooser);
	}
	else
	{
	    sprintf(Buf, catgets(Dtb_project_catd, 100, 7,
		    "%s is a read-only file."), old_filename);

	    util_set_help_data(catgets(Dtb_project_catd, 100, 81,
		"The changes that were made cannot be saved\nbecause you do not have permission to write\nto the file."), NULL, NULL);
	    util_printf_err(Buf);
	}

	ab_set_busy_cursor(FALSE);
        return (iRet);
    }
 
    /* A module is either being exported OR being saved to
     * different file. 
     */
    if (!util_file_exists(new_filename))
    {
        /* The file does not exist, so go ahead and write out
         * the new file.
         */
        if (errno == ENOENT)
        {
            iRet = projP_write_out_bil_file(module_obj, new_filename,
					    NULL, Exporting);
            if ((iRet >= 0) && !Exporting)
                obj_clear_flag(module_obj, SaveNeededFlag);
        }
        else  /* An error occurred with stat */
        {
            sprintf(Buf, "%s: %s", new_filename, sys_errlist[errno]);
	    util_printf_err(Buf);
        }
        XtUnmanageChild(AB_generic_chooser);
    }
    else
    {
	/* The file exists. Post an overwrite dialog. */
	iRet = proj_overwrite_existing_file(new_filename, module_obj, 
			FALSE, Exporting, FALSE, NULL, &answer);
 
	/* Pop down the file chooser if the user has chosen to
	 * overwrite the existing file.
	 */
	if (answer == DTB_ANSWER_ACTION1)
	    XtUnmanageChild(AB_generic_chooser);
    }
 
    ab_set_busy_cursor(FALSE);
    return (iRet);
}
 

int
projP_write_out_bil_file(
    ABObj       obj,
    char        *new_filename,
    STRING	old_proj_dir,
    BOOL        Exporting
)
{
    int			rc = 0;			/* return code */
    char                name[MAXPATHLEN];
    STRING              rel_path = NULL;
    STRING              old_file = NULL;
    STRING              old_name = NULL;

    *name = 0;

    if (!obj_is_module(obj))
        return -1;

    if (obj_get_name(obj) != NULL)
	old_name = (STRING) strdup(obj_get_name(obj));

    util_derive_name_from_path(new_filename, name);

    if (!Exporting)
    {
        if (obj_get_file(obj) != NULL)
            old_file =(STRING) strdup(obj_get_file(obj));

	rel_path = proj_cvt_mod_file_to_rel_path(new_filename,
			obj_get_file(proj_get_project()));
 
	if (util_strempty(rel_path) || util_strempty(name)
	    || util_strempty(old_name))
	{
	    rc = -1;
	}
	else
	{
	    /* Update UI components, e.g. project window, browser,
	     * all the window footers in the module and the main
	     * palette's status region.
	     */
	    proj_rename_module(obj, name);

	    rc = bil_save_tree(obj, rel_path, old_proj_dir,
			BIL_SAVE_FILE_PER_MODULE);
	    if (rc < 0)
            {
		util_print_error(rc, rel_path);
	        proj_rename_module(obj, old_name);
            }
	    else
	    {
		obj_set_file(obj, rel_path);
            	obj_clear_flag(obj, SaveNeededFlag);

		/* Set the SaveNeededFlag on the project. Either the
             	 * directory where the module was stored or its name
             	 * (or both) has changed.
             	 */   
	    	if (!util_paths_are_same_file(old_file, rel_path))
		    abobj_set_save_needed(obj_get_project(obj), TRUE);
	    }
	}

	if (old_file != NULL) util_free(old_file);
	if (rel_path != NULL) util_free(rel_path);
    }
    else        /* We're exporting the file, not saving it. */
    {
	if (util_strempty(name) || util_strempty(old_name))
	{
	    rc = -1;
	}
	else
	{
	    /* Using the obj_set_name routine instead of
	     * the abobj_set_name routine because the module
	     * is being exported (not saved), which means
	     * that we don't care if there's another module
	     * in the project with the exported name; the
	     * project doesn't need to be dirtied, etc. We 
	     * don't need all that stuff that abobj_set_name
	     * does for you.
	     */
	    abobj_disable_save_needed();
	    obj_set_name(obj, name);
            rc = bil_save_tree(obj, new_filename, old_proj_dir, 
			BIL_SAVE_FILE_PER_MODULE);
	    obj_set_name(obj, old_name);
	    abobj_enable_save_needed();
	    if (rc < 0)
		util_print_error(rc, new_filename);
	}
    }

    if (old_name != NULL) util_free(old_name);
    return rc;
}

/*
 * Update the sensitivity of menu items depending on
 * the old/new count of modules selected.
 *
 * NOTE: Currently, this proc does not use the old_count
 * param. It will, in future use it for optimization.
 */
void
projP_update_menu_items(
    int         old_count,
    int         new_count
)
{
    if (new_count == 0)
    {
        no_module_selected(&dtb_proj_proj_main);
        return;
    }
 
    if (new_count == 1)
    {
        one_module_selected(&dtb_proj_proj_main);
        return;
    }
 
    mult_module_selected(&dtb_proj_proj_main);
}

/*
 * no_module_selected()
 * No module is currently selected, so desensitize all
 * relevant menu items
 */
static void
no_module_selected(
    DtbProjProjMainInfo proj_w
)
{
    ProjModMenubuttonMenuItems  menu_rec;

    if (!proj_w)
        return;

    menu_rec = &(proj_w->menubar_Module_item_mod_menubutton_menu_items); 
    ui_set_active(menu_rec->Save_item, FALSE);
    ui_set_active(menu_rec->Save_As_item, FALSE);
    ui_set_active(menu_rec->Show_item, FALSE);
    ui_set_active(menu_rec->Hide_item, FALSE);
    ui_set_active(menu_rec->Browse_item, FALSE);
    ui_set_active(menu_rec->Export_item, FALSE);
    ui_set_active(menu_rec->Remove_item, FALSE);
}

/*
 * one_module_selected()
 * One module is currently selected, so make all
 * menu items sensitive
 */
static void
one_module_selected(
    DtbProjProjMainInfo proj_w
)
{
    ProjModMenubuttonMenuItems  menu_rec;
    ChooserInfo                 info = NULL;
 
    if (!proj_w) return;

    menu_rec = &(proj_w->menubar_Module_item_mod_menubutton_menu_items); 
    ui_set_active(menu_rec->Show_item, TRUE);
    ui_set_active(menu_rec->Hide_item, TRUE);
    ui_set_active(menu_rec->Browse_item, TRUE);
    ui_set_active(menu_rec->Remove_item, TRUE);
     
    /* If the FSB has not been created yet or if it has
     * been created but it is not displayed, then make
     * the menu items active.
     */
    if ((AB_generic_chooser == NULL) ||
        !XtIsManaged(AB_generic_chooser))
    {
        ui_set_active(menu_rec->Save_item, TRUE);
        ui_set_active(menu_rec->Save_As_item, TRUE);
        ui_set_active(menu_rec->Import_item, TRUE);
        ui_set_active(menu_rec->Export_item, TRUE);
    }
    else if (XtIsManaged(AB_generic_chooser))
    {
        XtVaGetValues(AB_generic_chooser, XmNuserData, &info, NULL);
        if (info->chooser_type == AB_EXPORT_CHOOSER)
        {
            ui_set_active(menu_rec->Export_item, TRUE);
        }
        else if (info->chooser_type == AB_SAVE_PROJ_AS_CHOOSER)
        {
            ui_set_active(menu_rec->Save_item, TRUE);
            ui_set_active(menu_rec->Save_As_item, TRUE);
        }
        else if (info->chooser_type == AB_IMPORT_CHOOSER)
        {
            ui_set_active(menu_rec->Import_item, TRUE);
        }
    }
}

/*
 * mult_module_selected()
 * Multiple modules are currently selected.
 * We desensitize the Save/Save As/Export buttons
 * because the choossers cannot handle multiple
 * saves at the moment...
 */
static void
mult_module_selected(
    DtbProjProjMainInfo proj_w
)
{
    ProjModMenubuttonMenuItems  menu_rec;
 
    if (!proj_w)
        return;

    menu_rec = &(proj_w->menubar_Module_item_mod_menubutton_menu_items); 
    ui_set_active(menu_rec->Save_item, FALSE);
    ui_set_active(menu_rec->Save_As_item, FALSE);
    ui_set_active(menu_rec->Export_item, FALSE);
    ui_set_active(menu_rec->Show_item, TRUE);
    ui_set_active(menu_rec->Hide_item, TRUE);
    ui_set_active(menu_rec->Browse_item, TRUE);
    ui_set_active(menu_rec->Remove_item, TRUE);
}
