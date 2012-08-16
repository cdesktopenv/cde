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
 *	$XConsortium: proj_utils.c /main/3 1995/11/06 17:47:04 rswiston $
 *
 *      @(#)proj_utils.c	1.70 28 Apr 1995
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
 * proj_utils.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/param.h>
#include <errno.h>
#include <X11/Xlib.h>
#include <Xm/Xm.h>
#include <Xm/FileSB.h>
#include <Xm/TextF.h>
#include <Xm/ToggleB.h>
#include <ab_private/ab.h>
#include <ab_private/obj.h>
#include <ab_private/brws.h>
#include <ab_private/x_util.h>
#include <ab_private/istr.h>
#include <ab_private/projP.h>
#include <ab_private/proj.h>
#include <ab_private/objxm.h>
#include <ab_private/util.h>
#include <ab_private/ui_util.h>
#include <ab_private/abio.h>
#include <ab_private/trav.h>
#include <ab_private/abobj.h>
#include <ab_private/abobj_set.h>
#include <ab_private/bil.h>
#include <ab_private/abuil_print.h>
#include <ab_private/pal.h>

#include "dtbuilder.h"
#include "palette_ui.h"
#include "proj_ui.h"


const char 	mod_path[] = "*proj_mod_path";
const char 	proj_path[] = "*proj_proj_path";
char		Buf[MAXPATHLEN];	/* Work buffer */

/* Whether we are to export a module as BIL (default) or UIL */
static BOOL ExportAsUIL = FALSE;

/*************************************************************************
**                                                                      **
**       Private Function Declarations                                  **
**                                                                      **
**************************************************************************/
static int      select_fn(
                    VNode       module
                );

static void 	save_proj_as_okCB(
		    Widget              widget,
		    XtPointer           client_data,
		    XmSelectionBoxCallbackStruct *call_data
		);

static void 	export_bil_okCB(
		    Widget                      widget,
		    XtPointer                   client_data,
		    XmSelectionBoxCallbackStruct *call_data
		);

static void 	name_module( 
		    ABObj       new_module, 
		    STRING      new_module_name 
		);

static Boolean 	verify_name(
		    Widget      prompt_text,
		    XtPointer   client_data
		);

static Widget 	create_save_encap_cbox(
		    Widget      parent,
		    BOOL	*EncapProj
		);

static Widget 	create_export_format_cbox(
		    Widget      parent,
		    ABObj	module_obj
		);

static void 	save_as_encap(
		    Widget widget,
		    XtPointer clientData,
		    XtPointer callData
		);

static void 	export_format_changed(
		    Widget widget,
		    XtPointer clientData,
		    XtPointer callData
		);

static int 	save_proj_as_bip(
		    STRING      file
		);

static int 	save_proj_as_bix(
		    STRING      file
		);

static int 	proj_get_file_basename(
		    STRING filename, 
		    STRING basename
		);

static Boolean export_uil_file( 
		    STRING fullpath, 
		    ABObj module_obj
		);

static int 	overwrite_bip_file(
		    ABObj       proj,
		    STRING      file,
		    STRING	old_proj_dir
		);


static void	notify_proj_save_as_cancelCB(
    			Widget          widget,
    			XtPointer       client_data,
    			XtPointer	call_data
		);

static void	notify_proj_save_as_okCB(
    			Widget          widget,
    			XtPointer       client_data,
    			XtPointer	call_data
		);

/*************************************************************************
**                                                                      **
**       Data                                                           **
**                                                                      **
**************************************************************************/

#if !defined(linux) && !defined(__FreeBSD__)
extern char             *sys_errlist[];
#endif

/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/

void
proj_select(
    AB_OBJ	*module
)
{
    VNode	selected_mod;
    Vwr		v;
    VMethods	m;

    if (!module)
	return;
    
    if (!(selected_mod = (VNode)module->projwin_data))
	return;

    if (!(v = selected_mod->browser))
	return;

    if (!(m = v->methods))
	return;
    
    BRWS_NODE_SET_STATE(selected_mod, BRWS_NODE_SELECTED);

    (*m->render_node)(selected_mod, TRUE);
}


void
proj_deselect(
    AB_OBJ	*module
)
{
    VNode	selected_mod;
    Vwr		v;
    VMethods	m;

    if (!module)
	return;
    
    if (!(selected_mod = (VNode)module->projwin_data))
	return;

    if (!(v = selected_mod->browser))
	return;

    if (!(m = v->methods))
	return;
    
    BRWS_NODE_UNSET_STATE(selected_mod, BRWS_NODE_SELECTED);

    (*m->render_node)(selected_mod, FALSE);
}


void
proj_toggle_select(
    AB_OBJ	*module
)
{
    VNode	selected_mod;
    Vwr		v;
    VMethods	m;

    if (!module)
	return;
    
    if (!(selected_mod = (VNode)module->projwin_data))
	return;

    if (!(v = selected_mod->browser))
	return;

    if (!(m = v->methods))
	return;
    
    if (BRWS_NODE_STATE_IS_SET(selected_mod, BRWS_NODE_SELECTED))
    {
        BRWS_NODE_UNSET_STATE(selected_mod, BRWS_NODE_SELECTED);
        (*m->render_node)(selected_mod, FALSE);
    }
    else
    {
        BRWS_NODE_SET_STATE(selected_mod, BRWS_NODE_SELECTED);
        (*m->render_node)(selected_mod, TRUE);
    }
}

void
proj_update_node(
    ABObj	obj
)
{
    Vwr		v;
    VNode	vnode;

    if (!obj)
	return;
    
    if (!(vnode = (VNode)obj->projwin_data))
	return;

    if (!(v = vnode->browser))
	return;

    vwr_init_elements(vnode);

    erase_viewer(v);
    draw_viewer(v);
}

int
proj_update_stat_region(
    PROJ_STATUS_INFO    type,
    STRING              value
)
{
    Vwr				proj_vwr = NULL;
    DtbProjProjMainInfoRec	*projInfo = NULL;
    BrowserUiObj		ui;
    static Widget       projwin_mod_path = NULL;
    static Widget       projwin_proj_path = NULL;
    Widget              path_value = NULL;
    String              nullstr = "        ";

    util_dprintf(2, "proj_update_stat_region:\n");

    if (dtb_proj_proj_main.module_drawarea == NULL)
    {
	return 0;
    }
   
    XtVaGetValues(dtb_proj_proj_main.module_drawarea, XmNuserData, &proj_vwr, NULL);
    
    if (!proj_vwr)
	return(-1);

    ui = (BrowserUiObj)proj_vwr->ui_handle;

    projInfo = (DtbProjProjMainInfoRec *)ui->ip;

    switch (type)
    {
        case PROJ_STATUS_MOD_PATH:
            path_value = projInfo->module_path;
	    util_dprintf(2, "\tcase PROJ_STATUS_MOD_PATH\n");
            break;
        case PROJ_STATUS_PROJ_PATH:
            path_value = projInfo->project_path;
	    util_dprintf(2, "\tcase PROJ_STATUS_PROJ_PATH\n");
            break;
    }
    if (value == NULL)
        value = nullstr;
 
    if (path_value != NULL)
    {
	util_dprintf(2, "\tsetting the path value to %s\n", value);
        XtVaSetValues(path_value,
                XtVaTypedArg, XmNlabelString, XtRString,
                value, strlen(value)+1,
                NULL);
    }

    return 0;          
}

void
proj_show_save_proj_as_chooser(
    Widget              	widget,
    ProjSaveCallbackFunc	saveDoneCB
)
{
    Widget      	textf = NULL,
			encap_cbox = NULL;
    char        	init_name[MAXPATHLEN];
    ChooserInfo		info = NULL;
    XmString		ok_label;
    char		*title;
 
    XtCallbackRec ok_callback[] = {
        {(XtCallbackProc)save_proj_as_okCB, (XtPointer) NULL},
        {(XtCallbackProc) NULL, (XtPointer) NULL},
        {(XtCallbackProc) NULL, (XtPointer) NULL}
    };
    XtCallbackRec cancel_callback[] = {
        {(XtCallbackProc)proj_cancelCB, (XtPointer) NULL},
        {(XtCallbackProc) NULL, (XtPointer) NULL},
        {(XtCallbackProc) NULL, (XtPointer) NULL}
    };

    XtCallbackRec popdown_callback[] = {
        {(XtCallbackProc)proj_unmap_chooserCB, (XtPointer) NULL},
        {(XtCallbackProc) NULL, (XtPointer) NULL}
    };


    /* Add callback to call saveDone, if necessary */
    ok_callback[1].callback = NULL;
    ok_callback[1].closure = NULL;
    if (saveDoneCB != NULL)
    {
	ok_callback[1].callback = notify_proj_save_as_okCB;
	ok_callback[1].closure = (XtPointer)saveDoneCB;
    }

    /* Add callback to call saveDone, if necessary */
    cancel_callback[1].callback = NULL;
    cancel_callback[1].closure = NULL;
    if (saveDoneCB != NULL)
    {
	cancel_callback[1].callback = notify_proj_save_as_cancelCB;
	cancel_callback[1].closure = (XtPointer)saveDoneCB;
    }
 
    /* Create the file chooser if it hasn't already
     * been created by anyone else, then set the
     * appropriate resources.
     */  
    if (AB_generic_chooser == NULL)
    {
        AB_generic_chooser = XmCreateFileSelectionDialog(
                widget,
                "AB_generic_chooser",
                (ArgList) NULL,
                (Cardinal) 0);

        info = (ChooserInfo) calloc(sizeof(ChooserInfoRec), 1);
        XtVaSetValues(AB_generic_chooser,
                        XmNuserData, (XtPointer) info,
                        NULL);
    }
    else if (XtIsManaged(AB_generic_chooser))
    {
        ui_win_front(AB_generic_chooser);
        return;
    }

    /*
     * Reset necessary fields
     */
    XtVaGetValues(AB_generic_chooser,
                        XmNuserData, &info,
                        NULL);
    info->writeEncapsulated = FALSE;

    /* Create the "Save As Encapsulated Project" checkbox */
    encap_cbox = create_save_encap_cbox(
			AB_generic_chooser, &(info->writeEncapsulated));

    info->child = (Widget) encap_cbox;
    info->chooser_type = AB_SAVE_PROJ_AS_CHOOSER;

    ok_label = XmStringCreateLocalized(catgets(Dtb_project_catd, 100, 237, "Save"));
    XtVaSetValues(AB_generic_chooser,
                XmNautoUnmanage, FALSE,
                XtVaTypedArg, XmNpattern,
                        XtRString, "*.bip",
                        sizeof("*.bip"),
                XmNokLabelString, ok_label,
                XmNokCallback, &ok_callback,
                XmNcancelCallback, &cancel_callback,
                NULL);
    XmStringFree(ok_label);

    title = XtNewString(catgets(Dtb_project_catd, 100, 238, "Save Project"));
    XtVaSetValues(XtParent(AB_generic_chooser),
                XmNtitle, title,
                XmNpopdownCallback, &popdown_callback,
                NULL);
    XtFree(title);
    textf = XmFileSelectionBoxGetChild(AB_generic_chooser,
                        XmDIALOG_TEXT);
    strcpy(init_name, (char *)obj_get_name(proj_get_project()));
    strcat(init_name, ".bip");
    XmTextFieldSetString(textf, init_name);
    XtManageChild(AB_generic_chooser);
    ui_win_front(AB_generic_chooser);

    /* Set File and Project Organizer Project and Module menu
     * items inactive, until this file chooser is dismissed.
     */
    pal_set_File_menu(AB_SAVE_PROJ_AS_CHOOSER, FALSE);
    proj_set_menus(AB_SAVE_PROJ_AS_CHOOSER, FALSE);
}


static void
notify_proj_save_as_cancelCB(
    Widget              widget,
    XtPointer           client_data,
    XtPointer		call_data
)
{
    ProjSaveCallbackFunc	saveDoneCB = (ProjSaveCallbackFunc)client_data;
    saveDoneCB(AB_SAVE_CANCELLED);
}


static void
notify_proj_save_as_okCB(
    Widget              widget,
    XtPointer           client_data,
    XtPointer		call_data
)
{
    ProjSaveCallbackFunc	saveDoneCB = (ProjSaveCallbackFunc)client_data;
    saveDoneCB(AB_SAVE_SAVE_AS);
}


/*************************************************************
 *  This is the okCallback proc for the "Save Project As"
 *  file chooser. It writes out the project file as well as
 *  all the module files that belong to the project. This
 *  is done via a call to proj_save_exploded().
 *************************************************************/
static void
save_proj_as_okCB(
    Widget              widget,
    XtPointer           client_data,
    XmSelectionBoxCallbackStruct *call_data
)
{
    STRING              proj_filename = NULL;
    char		proj_name[MAXPATHLEN];
    char		proj_dir[MAXPATHLEN];
    char		help_buf[MAXPATHLEN];
    int			iRet = 0; 
    ChooserInfo		chooserInfo = NULL;
    XmString            xm_buf = (XmString) NULL;
    BOOL		read_OK = FALSE, write_OK = FALSE;
    DtbObjectHelpData	help_data = NULL;

    *proj_name = 0;
    *proj_dir = 0;
    *help_buf = 0;
    proj_filename = (STRING)objxm_xmstr_to_str(call_data->value);

    /* 
    ** If the file chooser selection text field is empty, return,
    ** leaving the chooser up to give the user another try.
    */
    if (util_strempty(proj_filename))
    {
	dtb_proj_no_name_msg_initialize(&dtb_proj_no_name_msg);
	(void)dtb_show_modal_message(widget, 
			&dtb_proj_no_name_msg, NULL, NULL, NULL);
	return;
    }
    if (util_file_is_directory(proj_filename))
    {
        sprintf(Buf, catgets(Dtb_project_catd, 100, 71,
                "Cannot save the project to %s.\n%s is a directory not a file."), proj_filename, proj_filename);
        xm_buf = XmStringCreateLocalized(Buf);

	dtb_proj_error_msg_initialize(&dtb_proj_error_msg);

        help_data = (DtbObjectHelpData) util_malloc(sizeof(DtbObjectHelpDataRec));
        help_data->help_text = catgets(Dtb_project_catd, 100, 85,
                "The file name you specified for saving the project\nis a directory. Type another file name for the project.");
        help_data->help_volume = ""; 
        help_data->help_locationID = ""; 

	(void)dtb_show_modal_message(widget,
                        &dtb_proj_error_msg, xm_buf, help_data, NULL);

	util_free(help_data);
        XmStringFree(xm_buf);
	return;
    }

    /* Check if we're allowed to write to the directory
     * specified for the project file.
     */
    util_get_dir_name_from_path(proj_filename, proj_dir, MAXPATHLEN);
    iRet = abio_access_file(proj_dir, &read_OK, &write_OK);
    if (iRet == 0)
    {
	if (write_OK)
	{
	    util_derive_name_from_path(proj_filename, proj_name);
    	    if (ab_c_ident_is_ok(proj_name))
    	    {
		XtVaGetValues(widget, XmNuserData, &chooserInfo, NULL);
		if (chooserInfo->writeEncapsulated)
		    iRet = save_proj_as_bix(proj_filename);
		else
		    iRet = save_proj_as_bip(proj_filename);
	    }
    	    else
    	    {
		/* If the project name is illegal (i.e. x.y.z), then
	 	 * post a message informing the user and leave the
	 	 * chooser up to give him/her another try.
	 	 */
		dtb_proj_save_proj_as_msg_initialize(&dtb_proj_save_proj_as_msg);	
		(void)dtb_show_modal_message(widget, 
			&dtb_proj_save_proj_as_msg, NULL, NULL, NULL);
    	    }
	}
	else
        {
	    XtVaGetValues(widget, XmNuserData, &chooserInfo, NULL);
	    if (chooserInfo->writeEncapsulated)
	    {
		sprintf(Buf, catgets(Dtb_project_catd, 100, 77,
		    "Cannot save encapsulated project to \"%s\".\n\"%s\" does not have write permission."), proj_dir, proj_dir);
	    }
	    else
	    {
		sprintf(Buf, catgets(Dtb_project_catd, 100, 74,
		    "Cannot save project to \"%s\".\n\"%s\" does not have write permission."), proj_dir, proj_dir);
	    }

	    util_set_help_data(catgets(Dtb_project_catd, 100, 83,
		"The directory you specified for saving is not\nwritable by you. Therefore, the project cannot\nbe saved.  Try saving the project to another directory."), NULL, NULL);

            util_printf_err(Buf);
        }
    }
    else
    {
	util_dprintf(1, "Error in accessing \"%s\".", proj_dir);
    }
}

static int
save_proj_as_bip(
    STRING	file
)
{
    AB_TRAVERSAL        trav;
    ABObj               module= NULL;
    ABObj               project = proj_get_project();
    char                new_filename[MAXPATHLEN];
    char                new_proj_dir[MAXPATHLEN];
    char		old_proj_dir[MAXPATHLEN]; 
    BOOL                changed_dir= FALSE;
    BOOL		EncapProj = FALSE;
    BOOL		Edited = FALSE;
    BOOL                ProjHasBeenSaved = (obj_get_file(project) != NULL);
    int			iRet = 0;
    char		msg[MAXPATHLEN];
    DTB_MODAL_ANSWER	answer = DTB_ANSWER_ACTION1;
    DtbObjectHelpDataRec	help_data;

#define	MSG "You are trying to save the new project file (%s)\nin the same directory as the old project file (%s).\nThis will cause the same modules to be used in both the old\nproject and in the new project. Click OK to continue with\nthis option.\n\nTo create a separate version of the project %s and its\nmodules, you must save to a different directory. Click\nCancel now, then select a different directory in the file\nselection dialog."

#define HLP_MSG "When choosing Save Project As, you must save the project\nfile in a new directory if you want to save a separate\nversion of the project file (.bip) and each module file\n(.bil). Otherwise, the modules from the old version will\nbe overwritten with any changes from the new version. Both\nproject files will reference the same module files.\n\nApp Builder recommends one project per directory. When code\nis generated for the project, a Makefile is produced. There\ncan be only one Makefile per directory. If there are multiple\nprojects in one directory, the Makefile will be overwritten\neach time code is generated for a different project."


    *new_filename = 0;
    *new_proj_dir = 0;
    *old_proj_dir = 0;

    /* Save the old project directory before chdir'ing 
     * to the new one.
     */
    strcpy(old_proj_dir, ab_get_cur_dir());

    /* 
     * Compose the project file name.
     */
    strcpy(new_filename, file);
    abio_expand_bil_proj_path(new_filename);

    /*
     * cd to the directory specified by the Save As
     */
    *new_proj_dir= 0;
    util_get_dir_name_from_path(new_filename, new_proj_dir, MAXPATHLEN);
    changed_dir = !ab_is_cur_dir(new_proj_dir);
    if (ab_change_dir(new_proj_dir) < 0)
    {
        perror(new_proj_dir);
    }
 
    /* Check if the project was created by opening an
     * encapsulated file.
     */
    if (!proj_is_exploded(project))
    {
	EncapProj = TRUE;
	proj_set_files_exploded(project);
    }
	
    /* If the project has been saved before and the
     * user has not changed directory in the FSB and
     * the user isn't saving the project as a .bix
     * file, then pop up the save warning.
     */
    if (ProjHasBeenSaved && !changed_dir && !EncapProj)
    {
	int		new_name_len = 0;
	int		old_name_len = 0;
	int		msg_len = 0;
	char		old_file[MAXPATHLEN];
	char		new_file[MAXPATHLEN];
	STRING		msg = NULL;
  	XmString	xm_msg = NULL;

   	*old_file = 0;
   	*new_file = 0;
	util_get_file_name_from_path(obj_get_file(project), 
			old_file, MAXPATHLEN);
	util_get_file_name_from_path(new_filename, 
			new_file, MAXPATHLEN);
	new_name_len = strlen(new_file);
	old_name_len = strlen(old_file);
 	msg_len = strlen(catgets(Dtb_project_catd, 100, 101, MSG));
	msg = (STRING)util_malloc((2*old_name_len)+new_name_len+msg_len +1);

	help_data.help_text = catgets(Dtb_project_catd, 100, 102, HLP_MSG);
	help_data.help_volume = NULL;
	help_data.help_locationID = NULL;
	if (msg)
	{
	    sprintf(msg, catgets(Dtb_project_catd, 100, 101, MSG),
			new_file, old_file, old_file);
	    xm_msg = XmStringCreateLocalized(msg);
	    answer = dtb_show_modal_message(AB_generic_chooser,
			&dtb_proj_wrn_msg, xm_msg, &help_data, NULL);
	    util_free(msg);
	    XmStringFree(xm_msg);
	}
	else
	{
	    util_dprintf(1, "save_proj_as_bip: could not malloc space for message.");
	    answer = DTB_ANSWER_CANCEL;
	}
    }

    if (answer == DTB_ANSWER_ACTION1)
    {
	for (trav_open(&trav, project, AB_TRAV_MODULES);
            (module= trav_next(&trav)) != NULL; )
    	{
	    if (obj_has_flag(module, SaveNeededFlag))
	    	Edited = TRUE;
	    /*
	     * Since we may have changed directories, we need to
	     * write out all the bil files, as well.
	     */
	    if (changed_dir)
            	obj_set_flag(module, SaveNeededFlag);
    	}
    	trav_close(&trav);
 
	/**************************************************** 
	* Save out the project.				*
	****************************************************/
	
	/* Set the busy cursor while writing the project 
	 * to disk. 
	 */
	ab_set_busy_cursor(TRUE);

	iRet = proj_save_exploded(project, new_filename, 
		    TRUE, EncapProj);

	ab_set_busy_cursor(FALSE);
	
	/* If the save was successful, clear the SaveNeeded
	 * flag for the project. If it was not successful,
	 * then change directories back to the old project
	 * directory and make sure to set the Proj. Org.
	 * Project Path field back to the old directory.
	 */
	if (iRet >= 0)
	{
	    if (!Edited)
	    {
	    	for (trav_open(&trav, project, AB_TRAV_MODULES);
		    (module= trav_next(&trav)) != NULL; )
            	{
		    obj_clear_flag(module, SaveNeededFlag);
            	}
            	trav_close(&trav);
	    }
	    abobj_set_save_needed(project, FALSE);
    	}
	else if (changed_dir)
	{
	    ab_change_dir(old_proj_dir);
	    abobj_update_proj_name(project);
	}

	/* Popdown the file chooser. */
	XtUnmanageChild(AB_generic_chooser);
    }

    return (iRet);
}

void
proj_show_name_dlg(
    ABObj       module,
    Widget      parent
)
{
    STRING              init_name = (STRING) NULL;
    STRING              seln_str = (STRING) NULL;
    UI_MODAL_ANSWER     answer = UI_ANSWER_NONE;
    Widget              dlg = (Widget) NULL;
    int                 i, num_cascades = 0;
    WidgetList          cascades = (WidgetList) NULL;
    DtbObjectHelpData   help_data = NULL;
    ABObj		project = proj_get_project();
    STRING		dialog_title = NULL;
    STRING		textf_lbl = NULL;
    STRING		btn1_lbl = NULL;
    STRING		btn2_lbl = NULL;
    STRING		btn3_lbl = NULL;

    /* Post the name dialog */
    if (module)
    {
	obj_set_unique_name(module, "module");
	init_name = obj_get_name(module);
    }
    else
    {
	init_name = obj_alloc_unique_name_for_child(project,
		"module", -1);
    }

    /* We have to explicitly make the palette inactive if the
     * parent is not AB_toplevel.  This occurs when the first
     * window is dragged off the palette and the name dialog
     * pops up.  The parent is the window that was dragged out.
     * The reason is that ui_popup_modal_prompt() creates a
     * prompt dialog which is PRIMARY_APPLICATION_MODAL.  So,
     * the user won't be able to interact with the window, but
     * still would be able to interact with the palette.  That
     * is why it must be set to inactive explicitly.
     */
    if (parent != AB_toplevel)
    {  
        XtVaGetValues(dtb_palette_ab_palette_main.palette_menubar,
                XmNnumChildren, &num_cascades,
                XmNchildren,    &cascades,
                NULL);

        for (i = 0; i < num_cascades - 1; i++)
  	{
            XtSetSensitive(cascades[i], FALSE);
	}
        XtSetSensitive(dtb_palette_ab_palette_main.build_test_choice1, FALSE);
        ab_palette_set_active(FALSE);
    }
 

    help_data = (DtbObjectHelpData) util_malloc(sizeof(DtbObjectHelpDataRec));
    help_data->help_text = XtNewString(catgets(Dtb_project_catd, 100, 41,
                        "Type in a name for the new module. A valid name\ncan consist of letters, underscore (_), and digits,\nhowever the name must begin with either a letter or\nan underscore."));
    help_data->help_volume = "";
    help_data->help_locationID = "";

    dialog_title = XtNewString(catgets(Dtb_project_catd, 100, 40, 
	"Application Builder"));
    textf_lbl = XtNewString(catgets(Dtb_project_catd, 100, 111, "Module Name:"));
    btn1_lbl = XtNewString(catgets(Dtb_project_catd, 100, 108, "Apply"));
    btn2_lbl = XtNewString(catgets(Dtb_project_catd, 100, 109, "Cancel"));
    btn3_lbl = XtNewString(catgets(Dtb_project_catd, 100, 110, "Help"));

    answer = ui_popup_modal_prompt( parent, 
		dialog_title, textf_lbl, init_name, btn1_lbl, btn2_lbl, 
		btn3_lbl, help_data, XmDIALOG_OK_BUTTON, &seln_str,
		verify_name, (XtPointer) module);
    XtFree(dialog_title);
    XtFree(textf_lbl);
    XtFree(btn1_lbl);
    XtFree(btn2_lbl);
    XtFree(btn3_lbl);
    XtFree(help_data->help_text);
    util_free(help_data);
 
    switch (answer)
    {
	case UI_ANSWER_OK:
	case UI_ANSWER_CANCEL:
            if (parent != AB_toplevel)
            {
		for (i = 0; i < num_cascades - 1; i++)
		{
                    XtSetSensitive(cascades[i], TRUE);
		}
		XtSetSensitive(dtb_palette_ab_palette_main.build_test_choice1, TRUE);
		ab_palette_set_active(TRUE);	   
            }
	    if (answer == UI_ANSWER_OK)
	    {
		name_module(module, seln_str);
	    }
	    break;
    }    
}

void
proj_show_export_bil_chooser(
    Widget      widget,
    ABObj       obj
)
{
    Widget      	textf;
    Widget		format_cbox = (Widget)NULL;
    char        	init_name[MAXPATHLEN];
    ChooserInfo		info = NULL;
    XmString		ok_label;
    char		*title;

    XtCallbackRec ok_callback[] = {
        {(XtCallbackProc)export_bil_okCB, (XtPointer) NULL},
        {(XtCallbackProc) NULL, (XtPointer) NULL},
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
        AB_generic_chooser = XmCreateFileSelectionDialog(
                widget,
                "AB_generic_chooser",
                (ArgList) NULL,
                (Cardinal) 0);

        info = (ChooserInfo) util_malloc(sizeof(ChooserInfoRec));
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

    /* Force 'export as BIL' default */
    ExportAsUIL = FALSE;

    /* Create the "Export Module" checkbox */
    format_cbox = create_export_format_cbox(AB_generic_chooser, obj);

    info->child = format_cbox;
    info->chooser_type = AB_EXPORT_CHOOSER;

    /* Pass along the client_data to the okCallback */
    ok_callback[0].closure = (XtPointer) obj;

    ok_label = XmStringCreateLocalized(catgets(Dtb_project_catd, 100, 243, "Export"));
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

    title = XtNewString(catgets(Dtb_project_catd, 100, 239, "Export Module"));
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
    pal_set_File_menu(AB_EXPORT_CHOOSER, FALSE);
    proj_set_menus(AB_EXPORT_CHOOSER, FALSE);
}

static void
export_bil_okCB(
    Widget                      widget,
    XtPointer                   client_data,
    XmSelectionBoxCallbackStruct *call_data
)
{
    STRING	fullpath = NULL;
    ABObj	module_obj = (ABObj) client_data;
    XmString	xm_buf = (XmString) NULL;
    char        mod_name[MAXPATHLEN];
    char        mod_dir[MAXPATHLEN];
    BOOL        read_OK = FALSE, write_OK = FALSE;
    int		iRet = 0;
    DtbObjectHelpData	help_data = NULL;

    *mod_name = 0;
    *mod_dir = 0;
    fullpath = (STRING) objxm_xmstr_to_str(call_data->value);
    /* 
    ** If the file chooser selection text field is empty, return,
    ** but leave the choser up so the user gets another try.
    */
    if (util_strempty(fullpath)) 
    {
	dtb_proj_no_name_msg_initialize(&dtb_proj_no_name_msg);
	(void)dtb_show_modal_message(widget, 
			&dtb_proj_no_name_msg, NULL, NULL, NULL);
	return;
    }
    if (util_file_is_directory(fullpath))
    {
        sprintf(Buf, catgets(Dtb_project_catd, 100, 72,
                "Cannot export the module to %s.\n%s is a directory not a file."), fullpath, fullpath);
        xm_buf = XmStringCreateLocalized(Buf);
        dtb_proj_error_msg_initialize(&dtb_proj_error_msg);

	help_data = (DtbObjectHelpData) util_malloc(sizeof(DtbObjectHelpDataRec));
	help_data->help_text = catgets(Dtb_project_catd, 100, 84,
		"The file name you specified for exporting the\nmodule is a directory. Type another file name\nfor the module.");
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
	    if (ExportAsUIL) 
    	    {
		if (export_uil_file(fullpath,module_obj)) 
		{
		    /* Export succeeded.  Take down the chooser */
		    XtUnmanageChild(AB_generic_chooser);
		}
    	    }
	    else 
    	    {
		util_derive_name_from_path(fullpath, mod_name);
		if (ab_c_ident_is_ok(mod_name))
		{
		    /* This does the export and unmanages 
		     * the chooser.
		     */
		    projP_save_export_bil(module_obj, fullpath, TRUE);
		}
		else
		{
		  /* If the module name is illegal (i.e.  x.y.z), then
		   * post a message informing the user and leave the
		   * chooser up to give him/her another try.
		   */
		    dtb_proj_save_as_mod_msg_initialize(
				&dtb_proj_save_as_mod_msg);
		    (void) dtb_show_modal_message(widget,
                        	&dtb_proj_save_as_mod_msg,
                        	NULL, NULL, NULL);
		}
    	    }
	}
	else
        {
	    sprintf(Buf, catgets(Dtb_project_catd, 100, 78,
                    "Cannot export module to \"%s\".\n\"%s\" does not have write permission."), mod_dir, mod_dir);
            xm_buf = XmStringCreateLocalized(Buf);
	    dtb_proj_error_msg_initialize(&dtb_proj_error_msg);

            help_data = (DtbObjectHelpData) util_malloc(sizeof(DtbObjectHelpDataRec));
            help_data->help_text = catgets(Dtb_project_catd, 100, 86,
		"The directory you specified for exporting is not\nwritable by you. Therefore, the module cannot be\nexported. Try exporting the module to another\ndirectory.");
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

/*
 * Puts up a modal message dialog and returns info on which
 * button the user chooses (i.e. overwrite, cancel, or help).
 */
int
proj_overwrite_existing_file(
    char    		*file,
    ABObj   		obj,
    BOOL    		encapsulating,
    BOOL    		exporting,
    BOOL    		changed_dir,
    STRING		old_proj_dir,
    DTB_MODAL_ANSWER	*answer
)
{
    BOOL	read_OK = FALSE, write_OK = FALSE;
    int		iRet = 0;
    XmString	xm_buf = (XmString) NULL;

    abio_access_file(file, &read_OK, &write_OK);
    if (!write_OK)
    {
	if (!encapsulating)
	{
	    sprintf(Buf, catgets(Dtb_project_catd, 100, 3, 
			"File \"%s\" exists and\nis a read-only file. You can save your\nwork by saving to a different filename."), file);
	}
	else
	{
	    sprintf(Buf, catgets(Dtb_project_catd, 100, 53, 
			"File \"%s\" exists and\nis a read-only file. To save the encapsulated file,\nyou must save it to a different file name."), file);
	}

	util_set_help_data(catgets(Dtb_project_catd, 100, 79,
		"The file name you specified for saving already exists\nand cannot be overwritten, since it is not writable by\nyou. Your edits can be saved if you type a different\nfile name."), NULL, NULL);

	util_error(Buf);
	return (-1);
    }
    sprintf(Buf, catgets(Dtb_project_catd, 100, 4, 
	    "File \"%s\" exists.  You can:"), file);
    xm_buf = XmStringCreateLocalized(Buf); 

    dtb_proj_overwrite_msg_initialize(&dtb_proj_overwrite_msg);
    *answer = dtb_show_modal_message(dtb_get_toplevel_widget(),
		&dtb_proj_overwrite_msg, xm_buf, NULL, NULL);

    switch (*answer)
    {
	/* The user wants to overwrite an existing file */

        case DTB_ANSWER_ACTION1:
            /* If the obj is a project, then update the project
             * window title with the project's new name.
             */
            if (obj_is_project(obj))
            {  
		if (encapsulating)
		    iRet = proj_save_encapsulated(obj, file);
		else
                    iRet = overwrite_bip_file(obj, file, old_proj_dir);
            }
            else
                iRet = projP_write_out_bil_file(obj, file, old_proj_dir,
					exporting);
            break;
 
        case DTB_ANSWER_CANCEL:
 	    iRet = -1;
 	    break;

        case DTB_ANSWER_HELP:
            break;
    }

    XmStringFree(xm_buf);
    return (iRet);
}


void
proj_save_needed(
    ProjSaveCallbackFunc saveDoneCB
)
{
    ABObj		project = proj_get_project();
    DTB_MODAL_ANSWER	answer = DTB_ANSWER_NONE;
    BOOL		projHasBeenSaved = (obj_get_file(project) != NULL);
    BOOL		encapFile = FALSE;
    int			iRet = 0;
    XmString		xm_buf = (XmString) NULL;
    STRING		projName = obj_get_name(project);
    STRING		projFile = obj_get_file(project);
    char		proj_dir[MAXPATHLEN];
    BOOL		read_OK = FALSE, write_OK = FALSE;

    *proj_dir = 0;
    if (!proj_is_exploded(project))
    {
	sprintf(Buf, catgets(Dtb_project_catd, 100, 5,
	"Project %s was loaded from an encapsulated file (%s).\n\
	It will be saved as a normal project, with one file per module."),
	    projName,
	    projFile);
	xm_buf = XmStringCreateLocalized(Buf);

	encapFile = TRUE;
	dtb_proj_bix_save_msg_initialize(&dtb_proj_bix_save_msg);
	answer = dtb_show_modal_message(dtb_get_toplevel_widget(),
			&dtb_proj_bix_save_msg, xm_buf, NULL, NULL);
	XmStringFree(xm_buf);

	if (answer != DTB_ANSWER_ACTION1)
	{
	    return;
	}
	proj_set_files_exploded(project);
    }

    if (!projHasBeenSaved || encapFile) 
    {
	/* This eventually calls proj_save_exploded() and
	 * sets the SaveNeeded flag accordingly.
	 */
	proj_show_save_proj_as_chooser(dtb_get_toplevel_widget(), saveDoneCB);
    }
    else
    {
	/* Check if we're allowed to write to the directory
	 * specified for the project file.
	 */
	util_get_dir_name_from_path(projFile, proj_dir, MAXPATHLEN);
	iRet = abio_access_file(proj_dir, &read_OK, &write_OK);
	if (iRet == 0)
	{
            if (write_OK)
            {
		iRet = proj_save_exploded(project, projFile, 
				  FALSE, encapFile);
		if (iRet >= 0)
		    abobj_set_save_needed(project, FALSE);

		if (saveDoneCB != NULL)
		    saveDoneCB(iRet>=0? AB_SAVE_NORMAL:iRet);
	    }
            else
            {
		sprintf(Buf, catgets(Dtb_project_catd, 100, 74,
		    "Cannot save project to \"%s\".\n\"%s\" does not have write permission."), proj_dir, proj_dir);

		util_set_help_data(catgets(Dtb_project_catd, 100, 83,
		     "The directory you specified for saving is not\nwritable by you. Therefore, the project cannot\nbe saved. Try saving the project to another directory."), NULL, NULL);

		util_printf_err(Buf);
            }
    	}
	else
	{
	    util_dprintf(1, "Error in accessing \"%s\".", proj_dir);
	}
    }
}


/*
 * Makes sure each module is in its own file.
 *
 * Returns <0 on err, or # of file names changed
 */
int
proj_set_files_exploded(ABObj project)
{
    AB_TRAVERSAL	trav;
    ABObj		module = NULL;
    STRING		projFileName = obj_get_file(project);
    int			numFilesChanged = 0;
    char		newFileName[1024];
    *newFileName = 0;

    for (trav_open(&trav, project, AB_TRAV_MODULES);
	(module = trav_next(&trav)) != NULL; )
    {
	if (util_streq(projFileName, obj_get_file(module)))
	{
	    obj_set_file(module, NULL);
	    ++numFilesChanged;
	    abobj_set_save_needed(module, TRUE);
	}
    }
    trav_close(&trav);

    if (numFilesChanged > 0)
    {
	obj_set_file(project, NULL);
	abobj_set_save_needed(project, TRUE);
    }

    return numFilesChanged;
}

int
proj_unset_files_exploded(ABObj project)
{
    AB_TRAVERSAL        trav;
    ABObj               module = NULL;
    STRING              projFileName = obj_get_file(project);
 
    if (projFileName == NULL)
        return 0;
 
    for (trav_open(&trav, project, AB_TRAV_MODULES);
        (module = trav_next(&trav)) != NULL; )
    {
	obj_set_file(module, projFileName);
    }
    trav_close(&trav);
     
    return 0;
}
 

BOOL
proj_is_exploded(ABObj project)
{
    AB_TRAVERSAL	trav;
    ABObj		module = NULL;
    STRING		projFileName = obj_get_file(project);
    BOOL		isExploded = TRUE;

    if (projFileName == NULL)
    {
	return TRUE;
    }

    for (trav_open(&trav, project, AB_TRAV_MODULES);
	(module = trav_next(&trav)) != NULL; )
    {
	if (util_streq(projFileName, obj_get_file(module)))
	{
	    isExploded = FALSE;
	    break;
	}
    }
    trav_close(&trav);

    return isExploded;
}


int
proj_save_encapsulated(
    ABObj	obj,
    STRING	file_name
)
{
    int		rc = 0;		/* return code */
    char	proj_dir[MAXPATHLEN];
    BOOL	read_OK = FALSE, write_OK = FALSE;

    *proj_dir = 0;

    /* Check if we're allowed to write to the directory
     * specified for the project file.
     */                  
    util_get_dir_name_from_path(file_name, proj_dir, MAXPATHLEN);
    rc = abio_access_file(proj_dir, &read_OK, &write_OK);
    if (rc == 0)
    {
	if (write_OK)
	{
	    rc = bil_save_tree(obj, file_name, NULL,
				BIL_SAVE_ENCAPSULATED);
	    if (rc < 0)
		util_print_error(rc, file_name);
	}
	else
        {
            sprintf(Buf, catgets(Dtb_project_catd, 100, 77,
              "Cannot save encapsulated project to \"%s\".\n\"%s\" does not have write permission."), proj_dir, proj_dir);

	    util_set_help_data(catgets(Dtb_project_catd, 100, 83,
		 "The directory you specified for saving is not\nwritable by you. Therefore, the project cannot\nbe saved. Try saving the project to another directory."), NULL, NULL);

            util_printf_err(Buf);
	    rc = ERR;
        }
    }
    else
    {
	util_dprintf(1, "Error in accessing \"%s\".", proj_dir);
    }

    return rc;
}


int
proj_save_exploded(
    ABObj       project,
    char        *new_filename,
    BOOL        save_as,
    BOOL        encap_file
)
{
    int                 rc = 0;                 /* return code */
    char                name[MAXPATHLEN];
    STRING              old_name = NULL;
    STRING              old_file = NULL;
    DTB_MODAL_ANSWER    answer = DTB_ANSWER_NONE;
    BOOL                DiffFile = TRUE;
    char                tmp_path[MAXPATHLEN],
                        mod_exp_file[MAXPATHLEN];
    ABObj               module;
    AB_TRAVERSAL        trav;
    int                 iRet = 0;
    XmString            xm_buf = (XmString) NULL;
    STRING              old_proj_dir = NULL;
 
    *name = 0;
    *tmp_path = 0;
    *mod_exp_file = 0;
 
    if ( !obj_is_project(project) )
        return ERR;
 
    /* Save the old name and filename for the project.
     * If something goes wrong with saving it out, then
     * we'll set the name back to the old value. The old
     * file name is used to check if we're saving to
     * the same file or not.
     */
    if (obj_get_name(project) != NULL)
    {  
        old_name = (STRING) util_malloc(strlen(obj_get_name(project))+1);
        strcpy( old_name, obj_get_name(project) );
    }
    if (obj_get_file(project) != NULL)
    {
        old_file = (STRING) util_malloc(strlen(obj_get_file(project))+1);
        strcpy( old_file, obj_get_file(project) );
        old_proj_dir = (STRING) util_malloc(strlen(obj_get_file(project)));
        util_get_dir_name_from_path(obj_get_file(project),
                old_proj_dir, strlen(obj_get_file(project)));
    }

    util_derive_name_from_path(new_filename, name);
    obj_set_name(project, name);
 
    /****************************************************
     * Save the module (.bil) files out first.          *
     ****************************************************/
 
    /* If there are unsaved modules, save them out in the
     * same directory as the project file.
     */
    for (trav_open_cond(&trav, project, AB_TRAV_MODULES, obj_is_defined_module);
	(module = trav_next(&trav)) != NULL; )
    {
        /* Check if the file has never been saved or
         * if we're trying to save an encapsulated file
         * to individual files.
         */
        if ((obj_get_file(module) == NULL) || encap_file)
        {
            strcpy(tmp_path, obj_get_name(module));
            abio_expand_file(tmp_path, mod_exp_file);
            if ( !util_file_exists(mod_exp_file) )
            {
                if ( errno == ENOENT )
                {
                    rc = bil_save_tree(
                                module, mod_exp_file, old_proj_dir,
                                BIL_SAVE_FILE_PER_MODULE);
                    if (rc < 0)
                    {
			obj_set_name(project, old_name);
			util_print_error(rc, mod_exp_file);
    			if (old_name != NULL) util_free(old_name);
    			if (old_file != NULL) util_free(old_file);
    			if (old_proj_dir != NULL) util_free(old_proj_dir);
                        return rc;
                    }
                    obj_clear_flag(module, SaveNeededFlag);
                    obj_set_file(module, mod_exp_file);
 
                    /* Update all the UI components, such as the
                     * the project window and  browser.
                     */
                    proj_rename_module(module, obj_get_name(module));
                }
                else /* An error occurred with stat */
                {
                    obj_set_name(project, old_name);
                    sprintf(Buf, "%s: %s", mod_exp_file, sys_errlist[errno]);
		    util_printf_err(Buf);
    		    if (old_name != NULL) util_free(old_name);
		    if (old_file != NULL) util_free(old_file);
		    if (old_proj_dir != NULL) util_free(old_proj_dir);
                    return -1;
                }
            }
            /* A file named <module name>.bil already exists in the
             * directory where the project is going to be stored.
             * If the user chooses not to overwrite it or if there
             * was a problem in overwriting the file, then we have
             * to dismiss the "Save As Project" file chooser and
             * return  without trying to save the project.
             */
            else
            {  
                iRet = proj_overwrite_existing_file(mod_exp_file,
                                module, FALSE, FALSE, FALSE, old_proj_dir,
                                &answer);
                if ((iRet < 0) || (answer != DTB_ANSWER_ACTION1))
                {                
                    obj_set_name(project, old_name);
                    if (encap_file)
                    {
                        proj_unset_files_exploded(project);
                    }
                    XtUnmanageChild(AB_generic_chooser);
		    if (old_name != NULL) util_free(old_name);
		    if (old_file != NULL) util_free(old_file);
		    if (old_proj_dir != NULL) util_free(old_proj_dir);
                    return (iRet);
                }
                else obj_set_file(module, mod_exp_file);
            }    
        }       /* End if module has never been saved. */
        else /* The module has been previously saved to a file. */
        {
	    STRING	rel_path = NULL;

            if (obj_has_flag(module, SaveNeededFlag) || save_as)
            {
                if (save_as)
                {  
		    char	dir[MAXPATHLEN];
		    *dir = 0;

		    /* We need to get the relative path of the bil file
                     * because the project directory (and thus the current
                     * directory) may have changed. Therefore the file field
                     * can change for the module obj.
                     */
		    rel_path = proj_cvt_mod_file_to_rel_path(
				obj_get_file(module), new_filename);

                    /* If the module was stored to a subdirectory,
                     * make the same subdirectory hierarchy underneath
                     * the new project directory, if necessary.
                     */
                    util_get_dir_name_from_path( obj_get_file(module),
                        dir, MAXPATHLEN );
                    if (!util_path_is_absolute(dir) && !ab_is_cur_dir(dir))
                    {
                        if ((iRet = util_mkdir_hier(dir)) < 0)
                        {
		    	    obj_set_name(project, old_name);
			    sprintf(Buf, "%s: %s", dir, sys_errlist[errno]);
			    util_printf_err(Buf);
			    if (rel_path) util_free(rel_path);
			    if (old_name != NULL) util_free(old_name);
			    if (old_file != NULL) util_free(old_file);
			    if (old_proj_dir != NULL) util_free(old_proj_dir);
                            return (iRet);
                        }
                    }
		    if (util_file_exists(rel_path))
		    {
			iRet = proj_overwrite_existing_file(rel_path,
                                module, FALSE, FALSE, TRUE, 
				old_proj_dir, &answer);
			if ((iRet < 0) || (answer != DTB_ANSWER_ACTION1))
			{
                    	    obj_set_name(project, old_name);
                    	    XtUnmanageChild(AB_generic_chooser);
			    if (rel_path) util_free(rel_path);
			    if (old_name != NULL) util_free(old_name);
			    if (old_file != NULL) util_free(old_file);
			    if (old_proj_dir != NULL) util_free(old_proj_dir);
                    	    return (iRet);
			}
			else 
			{
			    obj_set_file(module, rel_path);
			    if (rel_path) util_free(rel_path);
		            obj_clear_flag(module, SaveNeededFlag);
			}
		    }
		    else
		    {
			rc = bil_save_tree(module, rel_path, 
				old_proj_dir, BIL_SAVE_FILE_PER_MODULE);
			if (rc < 0)
			{
			    obj_set_name(project, old_name);
			    util_print_error(rc, rel_path);
			    if (rel_path) util_free(rel_path);
			    if (old_name != NULL) util_free(old_name);
			    if (old_file != NULL) util_free(old_file);
			    if (old_proj_dir != NULL) util_free(old_proj_dir);
			    return rc;
			}      
			if (rel_path)
			{
                    	    obj_set_file(module, rel_path);
                    	    util_free(rel_path);
			}
		        obj_clear_flag(module, SaveNeededFlag);
		    }
                }
	  	else	/* Save modules in Save Project case */
		{
		    rel_path = obj_get_file(module);

                    rc = bil_save_tree(module, rel_path,
			old_proj_dir, BIL_SAVE_FILE_PER_MODULE); 
		    if (rc < 0)
                    {
		    	obj_set_name(project, old_name);
		    	util_print_error(rc, rel_path);
			if (old_name != NULL) util_free(old_name);
			if (old_file != NULL) util_free(old_file);
			if (old_proj_dir != NULL) util_free(old_proj_dir);
                    	return rc;
                    }
		    if (rel_path)
		    	obj_set_file(module, rel_path);

		    obj_clear_flag(module, SaveNeededFlag);
		}
            }
	    else
	    {
		/* We still need to get the relative path for
		 * the module (relative to the project directory)
		 * even if the module doesn't need saving because
		 * it could have been saved out first to the current
		 * directory and thereby given an absolute path.
		 */
		 rel_path = proj_cvt_mod_file_to_rel_path(
				obj_get_file(module), new_filename);
		obj_set_file(module, rel_path);
		util_free(rel_path);
	    }
        }
    }   /* End for loop */
    trav_close(&trav);
         
    /****************************************************
     * Save the project (.bip) file.                    *
     ****************************************************/

    /* If the project file needs to be saved or if we're saving
     * it to a different file, then write it out. Otherwise don't
     * do anything.
     */
    if (obj_has_flag(project, SaveNeededFlag) || save_as)
    {
        /* The file is being saved to a different name and
         * a file already exists with that name. Popup the
         * overwrite dialog.
         */
        if (!util_paths_are_same_file(old_file, new_filename) &&
            util_file_exists(new_filename))
        {
           /* If the user chose to overwrite the file, but
            * something went wrong with writing it out or if
            * the user decided not to overwrite, then dismiss
            * the file chooser.
            */
            iRet = proj_overwrite_existing_file(new_filename, project,
			FALSE, FALSE, FALSE, old_proj_dir, &answer);
            if ((iRet < 0) || (answer != DTB_ANSWER_ACTION1))
            {
                obj_set_name(project, old_name);
                if (encap_file)
                {
                    proj_unset_files_exploded(project);
                }
                XtUnmanageChild(AB_generic_chooser);
            }
            else obj_set_file(project, new_filename);
        }
        else 
        {
            rc = bil_save_tree(project, new_filename, old_proj_dir,
                                   BIL_SAVE_FILE_PER_MODULE);

            /* If there is an error saving out the project file,
             * post an error dialog.
             */
            if (rc < 0)
            { 
		util_print_error(rc, new_filename);
		obj_set_name(project, old_name);
    if (old_name != NULL) util_free(old_name);
    if (old_file != NULL) util_free(old_file);
    if (old_proj_dir != NULL) util_free(old_proj_dir);
		return rc;
            }
            obj_set_file(project, new_filename);
            obj_clear_flag(project, SaveNeededFlag);
        }
    }
       
    /* If the project directory has changed, update the
     * Project Organizer's Project Path field.
     */
    if (save_as)
        abobj_update_proj_name(project);
 
    if (old_name != NULL) util_free(old_name);
    if (old_file != NULL) util_free(old_file);
    if (old_proj_dir != NULL) util_free(old_proj_dir);
 
    return (iRet);
}

BOOL
proj_check_unsaved_edits(
    ABObj       root
)
{
    ABObj               obj = NULL;
    AB_TRAVERSAL        trav;
    BOOL                NeedsSaving = False;
 
    if ( obj_has_flag(root, SaveNeededFlag) )
	NeedsSaving = True;

    /* Need to check if any of the modules in the old project
     * has unsaved edits.  If so,  set NeedsSaving to true.
     */  
    for (trav_open(&trav, root, AB_TRAV_MODULES);
        ((obj = trav_next(&trav)) != NULL) && !NeedsSaving; )
    {
        if ( obj_has_flag(obj, SaveNeededFlag) )
            NeedsSaving = True;
    }
    trav_close(&trav);
 
    /* If there's nothing to save, clear the flag */
/*
    if (obj_get_num_children(root) == 0)
        obj_clear_flag(root, SaveNeededFlag);
*/
 
    return (NeedsSaving);
}

static void
name_module(
    ABObj       new_module,
    STRING      new_module_name
)
{
    ABObj       project = proj_get_project();

    if (new_module == NULL)
    {
        new_module = obj_create(AB_TYPE_MODULE, project);
        abobj_set_name(new_module, new_module_name);
        obj_ensure_unique_name(new_module, obj_get_project(new_module), -1);
        proj_set_cur_module(new_module);
        abobj_show_tree(new_module, TRUE);
    }
    else
    {   
        /* If this is a module that's being renamed (when you
         * drag out the first window, the module is initially
         * name "module") , clear out the file field (it's
         * still set to <oldname>.bil). If the module was created
         * when the user dragged out a window, the file field
         * will already be NULL.
         */
        obj_set_file(new_module, NULL);

        if (strcmp(new_module_name, obj_get_name(new_module)) != 0) 
            proj_rename_module(new_module, new_module_name);
    }
    abobj_set_save_needed(project, TRUE);
}

void
proj_create_new_proj(
    STRING      proj_name
)
{
    ABObj       new_proj = NULL;

    /* Create the new project and module.
     * Set the new module to be the current one.
     */  
    new_proj = obj_create(AB_TYPE_PROJECT, NULL);
    if (proj_name != NULL)
        obj_set_name(new_proj, proj_name);
    else
        obj_set_name(new_proj, "project");

   /* Initialize the project to the one just created.
    * This also sets the current module to NULL, since
    * a module has not been created and shown yet.
    */
    proj_set_project(new_proj);
    abobj_show_tree(new_proj, TRUE);

    /*
     * Update project window and main palette title.
     */  
    abobj_update_proj_name(new_proj);
    abobj_update_palette_title(new_proj);
    obj_tree_update_clients(new_proj);
}

int
proj_show_proj_dir(void)
{
    ABObj       proj = proj_get_project();
    STRING      projPath = NULL;
    char        dir_buf[MAXPATHLEN+1] = "";

    if (proj == NULL)
    {
        return -1;
    }
    projPath = obj_get_file(proj);
    if (projPath == NULL)
    {
        return -1;
    }
    if (util_get_dir_name_from_path(projPath, dir_buf, MAXPATHLEN) == NULL)    {
        return -1;
    }
    return ab_change_dir(dir_buf);
}

void
proj_name_proj()
{
    STRING              proj_name = NULL;
    UI_MODAL_ANSWER     answer = UI_ANSWER_NONE;
    DtbObjectHelpData	help_data = NULL;
    STRING		dialog_title = NULL;
    STRING		textf_lbl = NULL;
    STRING		def_name = NULL;
    STRING		btn1_lbl = NULL;
    STRING		btn2_lbl = NULL;
    STRING		btn3_lbl = NULL;

    help_data = (DtbObjectHelpData) util_malloc(sizeof(DtbObjectHelpDataRec));
    help_data->help_text = XtNewString(catgets(Dtb_project_catd, 100, 42,
                        "Type in a name for the new project. A valid\nname can consist of letters, underscore (_),\nand digits, however the name must begin with\neither a letter or an underscore."));
    help_data->help_volume = "";
    help_data->help_locationID = "";

    dialog_title = XtNewString(catgets(Dtb_project_catd, 100, 40, 
	"Application Builder"));
    textf_lbl = XtNewString(catgets(Dtb_project_catd, 100, 107, "Project Name:"));
    def_name = XtNewString("project");
    btn1_lbl = XtNewString(catgets(Dtb_project_catd, 100, 108, "Apply"));
    btn2_lbl = XtNewString(catgets(Dtb_project_catd, 100, 109, "Cancel"));
    btn3_lbl = XtNewString(catgets(Dtb_project_catd, 100, 110, "Help"));

    /* Popup the name dialog for the project */
    answer = ui_popup_modal_prompt( dtb_get_toplevel_widget(), 
		dialog_title, textf_lbl, def_name, btn1_lbl, 
		btn2_lbl, btn3_lbl, help_data, XmDIALOG_OK_BUTTON, 
		&proj_name, verify_name, (XtPointer) proj_get_project());
    XtFree(dialog_title);
    XtFree(textf_lbl);
    XtFree(def_name);
    XtFree(btn1_lbl);
    XtFree(btn2_lbl);
    XtFree(btn3_lbl);
    XtFree(help_data->help_text);
    util_free(help_data);
    switch (answer)
    {
        case UI_ANSWER_OK:
            proj_create_new_proj(proj_name);
            break;

        case UI_ANSWER_CANCEL:
        default:
            break;
    }
}    


/* This routine is called by the okCallback for the modal name dialog
 * via ui_popup_modal_prompt().  It checks that the named entered is
 * a valid one.  If not it posts a warning dialog and returns False.
 */
static Boolean
verify_name(
    Widget      prompt_text,
    XtPointer   client_data
)
{
    STRING              seln_str = NULL;
    char                new_name[MAXPATHLEN];
    char                title[MAXPATHLEN];
    Boolean             retval = True;
    Prompt_Info_Rec     *prompt_info;
    Boolean             ObjIsModule;
    ABObj               obj = NULL, newObj = NULL;

    prompt_info = (Prompt_Info_Rec  *) client_data;
    newObj = (ABObj)prompt_info->client_data;

    /* This routine is called by both the name project and
     * name module prompt dialogs.  When it is called by
     * proj_name_proj(), the client_data passed in is the
     * current project. When it is called by proj_show_name_dlg()
     * the client_data can be either NULL or a valid module.
     * It is NULL if the user selects "New Module". It is not
     * NULL if proj_show_name_dlg() gets called as a result of 
     * the first window getting dragged out (see create_obj_
     * action() and locate_obj_parent() in pal_create.c).
     */
    if (newObj == NULL)
	ObjIsModule = TRUE;
    else
	ObjIsModule = obj_is_module((ABObj)newObj);

    seln_str = XmTextFieldGetString(prompt_text);
    if( util_strempty(seln_str) )
    {
        if ( ObjIsModule )
        {
	    dtb_proj_vrfy_mod_name_msg_initialize(
			&dtb_proj_vrfy_mod_name_msg);
	    dtb_show_message(prompt_text, 
			&dtb_proj_vrfy_mod_name_msg, NULL, NULL);
        }
        else
        {

	    dtb_proj_vrfy_proj_name_msg_initialize(
			&dtb_proj_vrfy_proj_name_msg);
	    dtb_show_message(prompt_text, 
			&dtb_proj_vrfy_proj_name_msg, NULL, NULL);
        }
        retval = False;
    }
    else
    {
        /* Check if the user entered in <name>.bil (or .bip).
         * If so, util_check_name() will strip off the extension
         * and the revised name will be returned in new_name.
         * If the name was modified by util_check_name(), then
         * copy it to prompt_info.  The module or project will
         * then be created with that modified name.
         */
        util_check_name(seln_str, new_name);
        if (!util_streq(seln_str, new_name))
        {
            *(prompt_info->seln_str) = (STRING) XtMalloc(strlen(new_name) + 1);
            strcpy( *(prompt_info->seln_str), new_name );
        }
 
  	retval = proj_verify_name(new_name, ObjIsModule,
		&dtb_proj_vrfy_c_ident_msg, prompt_text, newObj);
    }
    return retval;
}

static Widget
create_save_encap_cbox(
    Widget      parent,
    BOOL	*EncapProj
)
{
    Widget      encap_toggle = NULL;
    XmString    label_xmstring = NULL;

    label_xmstring = XmStringCreateLocalized(
	catgets(Dtb_project_catd, 100, 240, "Save As Encapsulated Project"));
    encap_toggle = XtVaCreateManagedWidget("encap_toggle",
                        xmToggleButtonWidgetClass,
                        parent,
                        XmNlabelString, label_xmstring,
			XmNspacing, 0,
			XmNuserData, (XtPointer) parent,
                        NULL);
    XtAddCallback(encap_toggle, 
		XmNvalueChangedCallback, save_as_encap,
		(XtPointer) EncapProj);
    XmStringFree(label_xmstring);
    label_xmstring = NULL;

    return (encap_toggle);
}

static Widget
create_export_format_cbox(
    Widget      parent,
    ABObj	obj
)
{
    Widget      format_toggle = NULL;
    XmString    label_xmstring = NULL;

    label_xmstring = XmStringCreateLocalized(
	catgets(Dtb_project_catd, 100, 241, "Save As UIL"));
    format_toggle = XtVaCreateManagedWidget("format_toggle",
                        xmToggleButtonWidgetClass,
                        parent,
                        XmNlabelString, label_xmstring,
			XmNspacing, 0,
			XmNuserData, (XtPointer) obj,
                        NULL);
    XtAddCallback(format_toggle, 
		XmNvalueChangedCallback, export_format_changed,
		(XtPointer) parent);
    XmStringFree(label_xmstring);
    label_xmstring = NULL;

    return (format_toggle);
}

static void
save_as_encap(
    Widget widget,
    XtPointer clientData,
    XtPointer callData
)
{
    XmToggleButtonCallbackStruct *state =
                (XmToggleButtonCallbackStruct *)callData;
    BOOL        *EncapProj = (BOOL *) clientData;
    Widget	fchooser = NULL,
		textf = NULL;
    char        init_name[MAXPATHLEN];
    STRING	filename = NULL;	
 
    XtVaGetValues(widget, XmNuserData, &fchooser, NULL);
    textf = XmFileSelectionBoxGetChild(fchooser, XmDIALOG_TEXT);
    filename = XmTextFieldGetString(textf);    

    if (util_strempty(filename))
	strcpy(init_name, (char *)obj_get_name(proj_get_project()));
    else
	util_check_name(filename, init_name);

    if (state->set)
    {
	strcat(init_name, ".bix");
	XmTextFieldSetString(textf, init_name);
	*EncapProj = TRUE;
    }
    else
    {
	strcat(init_name, ".bip");
	XmTextFieldSetString(textf, init_name);
	*EncapProj = FALSE;
    }
}

static void
export_format_changed(
    Widget widget,
    XtPointer clientData,
    XtPointer callData
)
{
    XmToggleButtonCallbackStruct *state =
                (XmToggleButtonCallbackStruct *)callData;
    Widget	fchooser = NULL,
		textf = NULL;
    char        init_name[MAXPATHLEN];
    STRING	filename = NULL;	
    ABObj	module_obj = (ABObj) NULL;
 
    /* REMIND: Shouldn't this just be AB_generic_chooser? */
    fchooser = (Widget) clientData;
    textf = XmFileSelectionBoxGetChild(fchooser, XmDIALOG_TEXT);
    filename = XmTextFieldGetString(textf);    

    /* Get target module object out of userData */
    XtVaGetValues(widget,XmNuserData,&module_obj,NULL);

    /* 
    ** If a filename is not specified, use the name of the module to be 
    ** exported.  If a filename is specified, it might be either .bil or .uil.
    */
    if (util_strempty(filename))
	strcpy(init_name, (char *)obj_get_name(module_obj));
    else
	proj_get_file_basename(filename, init_name);

    if (state->set)
    {
	strcat(init_name, ".uil");
	XmTextFieldSetString(textf, init_name);
	ExportAsUIL = TRUE;
    }
    else
    {
	strcat(init_name, ".bil");
	XmTextFieldSetString(textf, init_name);
	ExportAsUIL = FALSE;
    }
}

static int
save_proj_as_bix(
    STRING	file
)
{
    char                bix_file[MAXPATHLEN];
    STRING              errmsg = (STRING) NULL;
    DTB_MODAL_ANSWER    answer = DTB_ANSWER_NONE;
    int			iRet = 0;
    XmString		xm_buf = (XmString) NULL;

    /* Set the busy cursor while writing the bix file to disk */
    ab_set_busy_cursor(TRUE);

    *bix_file = 0;
    strcpy(bix_file, file);
    if( abio_expand_bil_encapsulated_path(bix_file) != -1 )
    {   
        if( !util_file_exists(bix_file) )
        {
            if ( errno == ENOENT )
            {
                /* Pop down the file chooser */
                XtUnmanageChild(AB_generic_chooser);
                iRet = proj_save_encapsulated(proj_get_project(), 
					bix_file);
            }
            else /* An error occurred with stat */
            {
		sprintf(Buf, "%s: %s", bix_file, sys_errlist[errno]);
		xm_buf = XmStringCreateLocalized(Buf);
		dtb_proj_error_msg_initialize(&dtb_proj_error_msg);
		(void)dtb_show_modal_message(dtb_get_toplevel_widget(), 
			&dtb_proj_error_msg, xm_buf, NULL, NULL);
		XmStringFree(xm_buf);
            }
        }
        else /* The file already exists. Post an overwrite dialog. */
        {
            iRet = proj_overwrite_existing_file(bix_file,
			proj_get_project(), TRUE, TRUE, FALSE, NULL, 
			&answer);
 
            /* Popdown the file chooser if the user chooses to 
             * overwrite the file.  Otherwise keep it up.
             */
            if (answer == DTB_ANSWER_ACTION1)
	    {
                XtUnmanageChild(AB_generic_chooser);
	    }
        }
    }

    ab_set_busy_cursor(FALSE);
    return (iRet);
}

static int
proj_get_file_basename(
    STRING filename,
    STRING basename
)
{
    char	*slashPtr, *dotPtr;

    slashPtr = strrchr(filename, '/');
    if(slashPtr == NULL) {
	strcpy(basename,filename);
    }
    else {
	strcpy(basename,slashPtr+1);
    }

    dotPtr = strrchr(basename, '.');
    if(dotPtr != NULL) {
	if( (strncmp(dotPtr, ".bi", 3) == 0) ||
	    (strncmp(dotPtr, ".uil",3) == 0)    ) {
		*dotPtr = 0;
        }
    }
    return(0);
}

/* 
** Write out a module as a UIL file.  This is very similar to the code
** used to do save-as .bi[px] I/O.
*/
static Boolean
export_uil_file(
    STRING fullpath,
    ABObj module_obj
)
{
    FILE 		*fp;
    STRING		errmsg = NULL;
    Boolean		retval = FALSE;
    DTB_MODAL_ANSWER	answer = DTB_ANSWER_NONE;
    XmString            xm_buf = (XmString) NULL;

    /* Set the busy cursor while writing the file to disk */
    ab_set_busy_cursor(TRUE);

    /* See if the file exists */
    if( !util_file_exists(fullpath) )
    {
	if (errno == ENOENT) 
	{
	    /* Nope, the file doesn't exist, so write it out */
	    if( (fp=util_fopen_locked(fullpath,"w")) != (FILE *)NULL) 
	    {
	    	abuil_obj_print_uil(fp,module_obj);
		fclose(fp);
		retval = TRUE;
	    }
	}
	else 
	{
	    /* An error occurred in stat (inside util_file_exists()) */
	    sprintf(Buf, "%s: %s", fullpath, sys_errlist[errno]);
	    xm_buf = XmStringCreateLocalized(Buf);
	    dtb_proj_error_msg_initialize(&dtb_proj_error_msg);
	    (void)dtb_show_modal_message(dtb_get_toplevel_widget(), 
			&dtb_proj_error_msg, xm_buf, NULL, NULL);
	    XmStringFree(xm_buf);
	    retval = FALSE;
	}
    }
    else 
    {
	/* 
	** The file already exists, so post an overwrite dialog and
	** see what the user wants to do.
	*/
	sprintf(Buf, catgets(Dtb_project_catd, 100, 4,
		"File \"%s\" exists.  You can:"), fullpath);
	xm_buf = XmStringCreateLocalized(Buf);
	dtb_proj_overwrite_msg_initialize(&dtb_proj_overwrite_msg);
	answer = dtb_show_modal_message(dtb_get_toplevel_widget(),
			&dtb_proj_overwrite_msg, xm_buf, NULL, NULL);
	XmStringFree(xm_buf);

	if (answer == DTB_ANSWER_ACTION1)
	{
	    /* Yes, we should overwrite the existing file */
	    if( (fp=util_fopen_locked(fullpath,"w")) != (FILE *)NULL) 
	    {
	    	abuil_obj_print_uil(fp,module_obj);
		fclose(fp);
		retval = TRUE;
	    }
	}
	else 
	{
	    /* No, don't overwrite, try again via the chooser */
	    retval = FALSE;
	}
    }

    /* Turn off busy cursor and return whether we did the export or not */
    ab_set_busy_cursor(FALSE);
    return(retval);
}

static int
overwrite_bip_file(
    ABObj	proj,
    STRING	file,
    STRING	old_proj_dir
)
{
    int		rc = 0;		/* return code */

    rc = bil_save_tree(proj, file, old_proj_dir, BIL_SAVE_FILE_PER_MODULE);
    if (rc < 0)
	util_print_error(rc, file);
    return rc;
}

/* This routine takes the fully expanded paths (includes
 * directory and project or module name.bil/p) of the
 * module and project file fields and returns a string
 * that can be used in a call to obj_set_file().
 */
STRING
proj_cvt_mod_file_to_rel_path(
    STRING	mod_path,
    STRING	proj_path
)
{
    char                mod_name[MAXPATHLEN];
    char                mod_dir[MAXPATHLEN];
    char                proj_dir[MAXPATHLEN];
    char                rel_dir[MAXPATHLEN];
    STRING		rel_path = NULL;
    int			iRet = 0;

    *mod_name = 0;
    *mod_dir = 0;
    *proj_dir = 0;
    *rel_dir = 0;

    if (mod_path == NULL)
    {
	return NULL;
    }

    iRet = util_derive_name_from_path(mod_path, mod_name);
    util_get_dir_name_from_path(mod_path, mod_dir, MAXPATHLEN);

    if (proj_path != NULL)
    {
	util_get_dir_name_from_path(proj_path, 
			proj_dir, MAXPATHLEN);
	iRet = util_cvt_path_to_relative( mod_dir,
                        proj_dir, rel_dir, MAXPATHLEN);
        if (iRet == ERR)
        {
	    return NULL;
        }
    }
    else
    {
	/* If the project file field is NULL, then
       	 * we want to store the absolute path of the
	 * module.
	 */
	strcpy(rel_dir, mod_dir);
    }
 
    if (util_streq(rel_dir, "."))
    {
	rel_path = (char *) util_malloc(strlen(mod_name) + 5);
	sprintf(rel_path, "%s.bil", mod_name);
    }
    else if (util_streq(rel_dir, "/"))
    {
	rel_path = (char *) util_malloc(strlen(mod_name) +
				strlen(rel_dir) + 5);
	sprintf(rel_path, "%s%s.bil", rel_dir, mod_name);
    }
    else
    {
	rel_path = (char *) util_malloc(strlen(mod_name) +
				strlen(rel_dir) + 6);
	sprintf(rel_path, "%s/%s.bil", rel_dir, mod_name);
    }

    return (rel_path);
}

void
proj_set_menus(
    AB_CHOOSER_TYPE	chooser_type,
    BOOL		active
)
{
    Vwr		proj_vwr = NULL;
    VNode       *selected_nodes = NULL;
    int         num_selected = 0;
    Widget	open_proj_item = dtb_proj_proj_main.menubar_Project_item_proj_menubutton_menu_items.Open_item;
    Widget	save_proj_as_item = dtb_proj_proj_main.menubar_Project_item_proj_menubutton_menu_items.Save_As_item;
    Widget	save_proj_item = dtb_proj_proj_main.menubar_Project_item_proj_menubutton_menu_items.Save_item;
    Widget	import_item = dtb_proj_proj_main.menubar_Module_item_mod_menubutton_menu_items.Import_item;
    Widget	save_mod_as_item = dtb_proj_proj_main.menubar_Module_item_mod_menubutton_menu_items.Save_As_item;
    Widget	save_mod_item = dtb_proj_proj_main.menubar_Module_item_mod_menubutton_menu_items.Save_item;
    Widget	export_item = dtb_proj_proj_main.menubar_Module_item_mod_menubutton_menu_items.Export_item;

    if (AB_proj_window == NULL)
    {
	return;
    }

    /* Get the handle to the project viewer, so we can tell
     * if any module icons in the Project Organizer are selected.
     */
    XtVaGetValues(open_proj_item,
			XmNuserData, &proj_vwr,
			NULL);
    if (!proj_vwr)
	return;

    /*
     * Get nodes that are selected
     */
    vwr_get_cond(proj_vwr->current_tree, &selected_nodes,
                        &num_selected, select_fn);

    switch (chooser_type)
    {
        case AB_OPEN_PROJ_CHOOSER: 
	    XtSetSensitive(save_proj_as_item, active);
	    XtSetSensitive(save_proj_item, active);
	    XtSetSensitive(import_item, active);

	    /* Only set Module-> Save, Save As, and Export menu
	     * items active if there is exactly one module icon
	     * selected in the Project Organizer.
	     */
	    if (active && (num_selected == 1))
	    {
		XtSetSensitive(save_mod_as_item, active);
		XtSetSensitive(save_mod_item, active);
		XtSetSensitive(export_item, active);
	    }
	    else if (!active)
	    {
		XtSetSensitive(save_mod_as_item, active);
		XtSetSensitive(save_mod_item, active);
		XtSetSensitive(export_item, active);
	    }
            break; 
        case AB_SAVE_PROJ_AS_CHOOSER: 
	    XtSetSensitive(open_proj_item, active);
	    XtSetSensitive(import_item, active);

            /* Only set Module-> Save, Save As, and Export menu
             * items active if there is exactly one module icon 
             * selected in the Project Organizer. 
             */ 
            if (active && (num_selected == 1)) 
            {  
                XtSetSensitive(save_mod_as_item, active);
                XtSetSensitive(save_mod_item, active);
                XtSetSensitive(export_item, active);
            }    
            else if (!active) 
            {  
                XtSetSensitive(save_mod_as_item, active);
                XtSetSensitive(save_mod_item, active);
                XtSetSensitive(export_item, active);
            }
            break;
        case AB_IMPORT_CHOOSER: 
	    XtSetSensitive(open_proj_item, active);
	    XtSetSensitive(save_proj_as_item, active);
	    XtSetSensitive(save_proj_item, active);

            /* Only set Module-> Save, Save As, and Export menu
             * items active if there is exactly one module icon 
             * selected in the Project Organizer. 
             */ 
            if (active && (num_selected == 1)) 
            {  
                XtSetSensitive(save_mod_as_item, active);
                XtSetSensitive(save_mod_item, active);
                XtSetSensitive(export_item, active);
            }    
            else if (!active) 
            {  
                XtSetSensitive(save_mod_as_item, active);
                XtSetSensitive(save_mod_item, active);
                XtSetSensitive(export_item, active);
            }
            break;
        case AB_EXPORT_CHOOSER:
	    XtSetSensitive(open_proj_item, active);
	    XtSetSensitive(save_proj_as_item, active);
	    XtSetSensitive(save_proj_item, active);
	    XtSetSensitive(import_item, active);

            /* Only set Module-> Save, Save As, and Export menu
             * items active if there is exactly one module icon
             * selected in the Project Organizer. 
             */  
            if (active && (num_selected == 1))
            {  
                XtSetSensitive(save_mod_as_item, active);
                XtSetSensitive(save_mod_item, active);
                XtSetSensitive(export_item, active);
            }    
            else if (!active)    
            {    
                XtSetSensitive(save_mod_as_item, active);
                XtSetSensitive(save_mod_item, active);
                XtSetSensitive(export_item, active);
            }
            break;
        case AB_SAVE_MOD_AS_CHOOSER:
	    XtSetSensitive(open_proj_item, active);
	    XtSetSensitive(save_proj_as_item, active);
	    XtSetSensitive(save_proj_item, active);
	    XtSetSensitive(import_item, active);

            /* Only set Module-> Save, Save As, and Export menu
             * items active if there is exactly one module icon
             * selected in the Project Organizer.
             */
            if (active && (num_selected == 1))
            {  
                XtSetSensitive(save_mod_as_item, active);
                XtSetSensitive(save_mod_item, active);
                XtSetSensitive(export_item, active);
            }
            else if (!active)
            {
                XtSetSensitive(save_mod_as_item, active);
                XtSetSensitive(save_mod_item, active);
                XtSetSensitive(export_item, active);
            }
            break;
    }
}

static int
select_fn(
    VNode       module
)
{
    if (BRWS_NODE_STATE_IS_SET(module, BRWS_NODE_SELECTED))
        return (1);

    return (0);
}


Boolean
proj_verify_name(
    STRING		new_name,
    BOOL		ObjIsModule,
    DtbMessageData	msgData,
    Widget		parent,
    ABObj		newObj
)
{
    Boolean	retval = TRUE;
    XmString    xm_buf = (XmString) NULL;
    ABObj	obj = NULL;

    if (!ab_c_ident_is_ok(new_name))
    {
	dtb_show_message(parent, msgData, NULL, NULL);
        retval = False;
    }
    else /* The name is legal, but need to check if it's unique */
    {
        if (ObjIsModule)
        {
	    obj = obj_find_by_name_and_type(proj_get_project(),
		new_name, AB_TYPE_MODULE);

	    /* If the module that was found is the same one
	     * as the one we're naming, then don't post the
	     * "not unique name" message.  This will happen
	     * when the user first drags out a window and
	     * names the module the default name, "module".
	     */
            if ((obj != NULL) && (obj != newObj))
            {
		sprintf(Buf, catgets(Dtb_project_catd, 100, 6,
		    "%s is not a unique name for the module."), 
		    new_name );
		xm_buf = XmStringCreateLocalized(Buf);
		dtb_show_message(parent, &dtb_proj_unique_name_msg, 
			xm_buf, NULL); 
		XmStringFree(xm_buf);
                retval = False;
	    }
	}
    }

    return retval;
}
