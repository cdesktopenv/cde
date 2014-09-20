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
 *      $XConsortium: proj.c /main/3 1995/11/06 17:45:35 rswiston $
 *
 *      @(#)proj.c	1.50 17 May 1994
 *
 *      RESTRICTED CONFIDENTIAL INFORMATION:
 *
 *      The information in this document is subject to special
 *      restrictions in a confidential disclosure agreement between
 *      HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *      document outside HP, IBM, Sun, USL, SCO, or Univel without
 *      Sun's specific written approval.  This document and all copies
 *      and derivative works thereof must be returned or destroyed at
 *      Sun's request.
 *
 *      Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 */


#include <stdio.h>
#include <sys/param.h>
#include <errno.h>
#include <Xm/Xm.h>
#include <Xm/BulletinB.h>
#include <Xm/SelectioB.h>
#include <ab_private/ab.h> 
#include <ab_private/bil.h> 
#include <ab_private/abobj.h> 
#include <ab_private/abobj_set.h> 
#include <ab_private/appfw.h> 
#include <ab_private/objxm.h>
#include <ab_private/brwsP.h>
#include <ab_private/projP.h> 
#include <ab_private/cgen.h>
#include <ab_private/ui_util.h>
#include <ab_private/x_util.h>
#include <ab_private/trav.h>
#include <ab_private/abio.h>
#include <ab_private/ab_bil.h>
#include <ab_private/proj.h>
#include <ab_private/obj.h>
#include <ab_private/abuil_load.h>
#include <ab_private/pal.h>
#include "proj_ui.h"
#include "dtbuilder.h"

/*************************************************************************
**                                                                      **
**       Private Function Declarations                                  **
**                                                                      **
**************************************************************************/
static Vwr	create_proj_struct();

static void	create_proj_ui_handles(
		    Vwr		v
		);

/*
 * Action Procs for project window.
 */
static void    	select_module(
		    Widget w, 
		    XEvent *ev, 
		    String *params, 
		    int nparams
        	);

static void    	toggle_select_module(
		    Widget w, 
		    XEvent *ev, 
		    String *params, 
		    int nparams
        	);

static void    	show_module(
		    Widget w, 
		    XEvent *ev, 
		    String *params, 
		    int nparams
        	);

static void	enable_proj_actions(
		    Widget widget
		);

static void	disable_proj_actions(
		    Widget widget
		);

static void	no_module_selected(
		    DtbProjProjMainInfo	proj_d
		);

static void	one_module_selected(
		    DtbProjProjMainInfo	proj_d
		);

static void	mult_module_selected(
		    DtbProjProjMainInfo	proj_d
		);

static void	update_menu_items(
		    int		old_count,
		    int		new_count
		);

static void 	pointer_motion_proc(
		    Widget widget,
		    XEvent *event,
		    String *params,
		    int num_params
		);
		    
static void	popdown_proj_window(
		    Widget	widget,
		    XtPointer   client_data,
		    XtPointer   call_data
		);

static void 	import_uil(
		    STRING      uil_file_name
		);

/*
 * Object notification callbacks
 */
static int	projP_obj_renameOCB(
		    ObjEvAttChangeInfo	info
		);

static int	projP_obj_destroyOCB(
		    ObjEvDestroyInfo	info
		);

static int	projP_obj_updateOCB(
		    ObjEvUpdateInfo	info
		);

/*
 * Functions for supporting rubber banding
 */
static void	project_bpress(
		    Widget widget, 
		    XEvent *event, 
		    String *params, 
		    int num_params
		);

static int	project_select_rband(
		    VNode       vnode
		);

static void	projP_drawarea_button_drag(
		    Widget widget,
		    XtPointer client_data,
		    XEvent *event,
		    Boolean *cont_dispatch
		);

static void	project_rband(
		    Widget	widget,
		    XEvent	*event,
		    XRectangle	*rb_rect,
		    XtPointer	client_data
		);

/*************************************************************************
**                                                                      **
**       Data     	                                                **
**                                                                      **
**************************************************************************/

/*
 * Project window Actions
 */
static XtActionsRec projwin_actions[] = {
    {"ModButtonPress",   (XtActionProc)project_bpress },
    {"ModSelect",        (XtActionProc)select_module },
    {"ModToggleSelect",  (XtActionProc)toggle_select_module },
    {"ModShowModule",    (XtActionProc)show_module },
    {"ModPointerMotion", (XtActionProc)pointer_motion_proc}
};

/* 
 * Translations for module subwindow of Project Organizer 
 */
static String base_translations =
     "Shift <Btn1Down>:		ModButtonPress() \n\
     <Btn1Down>:		ModButtonPress() \n\
      Shift <Btn1Down>,<Btn1Up>: ModToggleSelect() \n\
     <Btn1Down>,<Btn1Up>:	ModSelect() \n\
     <Btn1Up>(2):		ModShowModule() \n\
     <Motion>:			ModPointerMotion() ";

static String btn2_adjust_translations =
    "\n<Btn2Down>:                ModButtonPress() \n\
       <Btn2Down>,<Btn2Up>:       ModToggleSelect() ";

static XtTranslations proj_transtbl = NULL;


/*
 * Static variables used for rubber banding
 */
static XRectangle       *rband_rect = NULL;
static Boolean          mselect_adjust = False;

char			Buf[MAXPATHLEN];	/* Work buffer */

/*************************************************************************
**
**       Function Definitions
**
**************************************************************************/
static int	select_fn(
		    VNode	module
		);

/*
 * Project Window Xt callbacks
 */

/*
 * Popup the project window by managing the container child of the
 * Dialog shell.
 */
void
proj_show_dialog()
{
    ABObj		proj = proj_get_project();
    BrowserUiObj 	ui;
    ChooserInfo		info = NULL;
 
    if (AB_proj_window == NULL)
    {
	if (!Proj_viewer)
            Proj_viewer = create_proj_struct();

	create_proj_ui_handles(Proj_viewer);
        proj_add_objects(proj);

	/* Check if the shared file chooser is already
	 * up when the Project Organizer is displayed.
	 * If it is, then find out which one is up and
	 * set the appropriate menu items inactive.
	 */
	if ((AB_generic_chooser != NULL) &&
	    XtIsManaged(AB_generic_chooser))
 	{
	    XtVaGetValues(AB_generic_chooser,
				XmNuserData, &info,
				NULL);
	    proj_set_menus(info->chooser_type, FALSE);
	}
	/* When the Project Organizer is first brought
	 * up, no modules are selected so certain menu
	 * items (like Module ->Save/Save As) should be
	 * made inactive.
	 */
	update_menu_items(0, 0);
    }
    ab_show_window(AB_proj_window);
    abobj_update_proj_name( proj );
}

/*
 * projP_store_viewer
 * Create callback for project window widgets
 */
void
projP_store_viewer(
    Widget      widget,
    XtPointer   client_data,
    XtPointer   call_data
)
{
    char	*tmp_str = (char *)client_data;

    /*
    widget_str = strtok(tmp_str, "+");
    */

    /*
     * If project viewer structure has not been initialized,
     * do it now
     */
    if (!Proj_viewer)
    {
	Proj_viewer = create_proj_struct();
    }

    XtVaSetValues(widget, XmNuserData, (XtPointer)Proj_viewer, NULL);

}

/*
 * Create viewer data structure for project window
 */
static Vwr
create_proj_struct()
{
    Vwr			v;
    extern VMethods	projP_methods;

    v = vwr_create(projP_methods);

    return(v);
}

static void
popdown_proj_window(
    Widget	widget,
    XtPointer   client_data,
    XtPointer   call_data
)
{
    ui_win_show(widget, False, XtGrabNone);
}

static void
create_proj_ui_handles(
    Vwr		v
)
{
    Widget	draw_area = NULL;
    BrowserUiObj ui;


    dtbProjProjMainInfo_clear(&dtb_proj_proj_main);

    /*
     * Create ui handle struct for project window
     */
    ui = aob_create_ui_obj();

    (void)dtb_proj_proj_main_initialize(&dtb_proj_proj_main, AB_toplevel);
    ui->shell = dtb_proj_proj_main.proj_main;
    AB_proj_window = dtb_proj_proj_main.proj_main_mainwin;

    ab_register_window(AB_proj_window, AB_WIN_WINDOW, 
	WindowHidden, AB_toplevel, AB_WPOS_TILE_VERTICAL,
	popdown_proj_window, NULL);

    /*
     * WORKAROUND FOR BUG
     * Until the generated code sets XmNmenuHelpWidget properly,
     * this is a workaround.
     */
    XtVaSetValues(dtb_proj_proj_main.menubar,
        XmNmenuHelpWidget, dtb_proj_proj_main.menubar_items.Help_item,
        NULL);

    XtRealizeWidget(ui->shell);

    ui->ip = (void *)&dtb_proj_proj_main;
	
    v->ui_handle = (void *)ui;

    if (!(draw_area = brws_draw_area(v)))  
    {
        fprintf(stderr, "Can't find draw area widget for project\n");
        return;
    }

    /*
     * Set event handler on draw area for dragging/rubber banding
     */
    XtAddEventHandler(draw_area, 
                Button1MotionMask | Button2MotionMask, False,
                projP_drawarea_button_drag, (XtPointer)NULL);

    brwsP_make_drawarea_snap(v, draw_area);

    setup_vwr_graphics(v);

    enable_proj_actions(draw_area);
}

void
proj_add_objects
(
    AB_OBJ	*obj
)
{
    AB_OBJ		*project;
    Vwr			viewer;
    ViewerMethods	*m;

    if (!Proj_viewer)
	return;
/*
        Proj_viewer = create_proj_struct();
*/

    /*
     * Get project object
     */
    project = obj_get_project(obj);

    viewer = Proj_viewer;

    m = viewer->methods;
    (*m->insert_tree)(viewer, obj);

    /*
     * Erase/redraw viewer
     */
    if (viewer->ui_handle)
    {
	/*
	 * Don't need to erase viewer as a new node is 
	 * added to end of the array of nodes in project window
        erase_viewer(viewer);
	 */
        draw_viewer(viewer);
    }
}

void
proj_delete_objects
(
    AB_OBJ	*obj
)
{
    AB_OBJ		*project;
    Vwr			viewer;
    ViewerMethods	*m;

    if (!Proj_viewer)
	return;

    /*
     * Get project object
     */
    project = obj_get_project(obj);

    viewer = Proj_viewer;

    m = viewer->methods;
    (*m->remove_tree)(viewer, obj);

    /*
     * Erase/redraw viewer
     */
    if (viewer->ui_handle)
    {
        erase_viewer(viewer);
        draw_viewer(viewer);
    }
}

/*
 * Register project actions with Xt
 */
void
proj_register_actions(
    XtAppContext app
)
{
    XtAppAddActions(app, projwin_actions, XtNumber(projwin_actions));
}

/*
 * Enable project window actions
 */
static void
enable_proj_actions(
    Widget widget
)
{
    if (proj_transtbl == NULL)
    {
	String	   translations;
        int        len;

	len = strlen(base_translations) + 1;

	if (AB_btn1_transfer == True) /* Button2 can be used for ADJUST */
            len += strlen(btn2_adjust_translations);

        translations = (String)util_malloc(len*sizeof(char));
        if (translations == NULL)
            return; /* yikes */

	strcpy(translations, base_translations);
        if (AB_btn1_transfer == True) /* Button2 can be used for ADJUST */ 
	    strcat(translations, btn2_adjust_translations);

	proj_transtbl = XtParseTranslationTable(translations);

	util_free(translations);
    }

    /* enable build mode behavior */
    XtVaSetValues(widget, 
		XtNtranslations, proj_transtbl,
        	NULL);
}

/*
 * Disable project window actions
 */
static void
disable_proj_actions(
    Widget widget
)
{
}

/*
 * Action: Select module in project window
 */
static void
select_module(
    Widget widget, 
    XEvent *event, 
    String *params, 
    int num_params
)
{
    Vwr		v = NULL;
    VNode	selected_node;
    VMethods	m;
    AB_OBJ	*obj;

    XtVaGetValues(widget, XmNuserData, &v, NULL); 

    if (!v)
	return;

    selected_node = vwr_locate_node(v, 
		        event->xbutton.x, event->xbutton.y);
    
    if (selected_node)
    {
	VNode	*selected_nodes = NULL;
	int	num_selected = 0;

	if (!(m = v->methods))
	    return;
	
	/* USE METHODS !! */
	obj = (AB_OBJ *)selected_node->obj_data;

	if (!obj)
	    return;

	/*
	 * Deselect all selected module nodes
	 * Select the module node
	 */
	aob_deselect_all_nodes(v, TRUE);
	proj_select(obj);

        vwr_get_cond(v->current_tree, &selected_nodes, 
			&num_selected, select_fn);
	
	if (selected_nodes)
	    free((char *)selected_nodes);

	update_menu_items(0, num_selected);
    }
}

/*
 * Action: Toggle select module in project window
 */
static void
toggle_select_module(
    Widget widget, 
    XEvent *event, 
    String *params, 
    int num_params
)
{
    Vwr		v = NULL;
    VNode	selected_node;
    VMethods	m;
    AB_OBJ	*obj;

    XtVaGetValues(widget, XmNuserData, &v, NULL); 

    if (!v)
	return;

    selected_node = vwr_locate_node(v, 
		        event->xbutton.x, event->xbutton.y);
    
    if (selected_node)
    {
	VNode	*selected_nodes = NULL;
	int	num_selected = 0;

	if (!(m = v->methods))
	    return;
	
	/* USE METHODS !! */
	obj = (AB_OBJ *)selected_node->obj_data;

	if (!obj)
	    return;

	/*
	 * Toggle select the module node
	 */
        if (BRWS_NODE_STATE_IS_SET(selected_node, BRWS_NODE_SELECTED))
	    proj_deselect(obj);
	else
	    proj_select(obj);

        vwr_get_cond(v->current_tree, &selected_nodes, 
			&num_selected, select_fn);
	
	if (selected_nodes)
	    free((char *)selected_nodes);

	update_menu_items(0, num_selected);
    }
}

/*
 * Action: Show the module when double-clicked on.
 */
static void
show_module(
    Widget widget, 
    XEvent *event, 
    String *params, 
    int num_params
)
{
    Vwr         v = NULL;
    VNode       selected_node;
    VMethods    m;
    ABObj       obj;
    ABObj       winobj;
    AB_TRAVERSAL trav;
 
    XtVaGetValues(widget, XmNuserData, &v, NULL);

    if (!v)
        return;

    selected_node = vwr_locate_node(v,
                        event->xbutton.x, event->xbutton.y);

    if (selected_node)
    {
        VNode   *selected_nodes = NULL;
        int     num_selected = 0;
 
        if (!(m = v->methods))
            return;

        /* USE METHODS !! */
        obj = (ABObj) selected_node->obj_data;
 
        if (!obj)
            return;

        /*
         * Set busy cursor before this potentially
         * lengthy operation
         */
        ab_set_busy_cursor(TRUE);

        /* If the module is already showing, then
         * make all of its windows come to the fore-
         * ground.
         */
        if (obj_has_flag(obj, MappedFlag))
        {
            for (trav_open(&trav, obj, AB_TRAV_WINDOWS);
                (winobj = trav_next(&trav)) != NULL; )
            {
                if (obj_has_flag(winobj, MappedFlag))
                    ui_win_front(objxm_get_widget(winobj));
            }
            trav_close(&trav);
            proj_set_cur_module(obj);
        }    
        else if( abobj_show_tree(obj, TRUE) == -1 )
        {
           if (util_get_verbosity() > 0)
             fprintf(stderr,"show_module: error in abobj_show_tree\n");
	}
        else 
        {
            proj_set_cur_module(obj);
        }

        vwr_get_cond(v->current_tree, &selected_nodes,
                        &num_selected, select_fn);

        if (selected_nodes)                        
            free((char *)selected_nodes);
 
        update_menu_items(0, num_selected);

        /*
         * Unset busy cursor
         */
        ab_set_busy_cursor(FALSE);

    }
}

static int
select_fn(
    VNode	module
)
{
    if (BRWS_NODE_STATE_IS_SET(module, BRWS_NODE_SELECTED))
	return (1);

    return (0);
}

static int
project_select_rband(
    VNode       vnode
)
{

    if (!vnode || !rband_rect)
	return (0);

    if (((int)rband_rect->x <= vnode->x) && 
	((int)(rband_rect->x + (short)rband_rect->width) >= (vnode->x + vnode->width)) &&
	((int)rband_rect->y <= vnode->y) &&
	((int)(rband_rect->y + (short)rband_rect->height) >= vnode->y + vnode->height))
        return (1);
		 
    return (0);
}

/*
 * Update the sensitivity of menu items depending on
 * the old/new count of modules selected.
 * 
 * NOTE: Currently, this proc does not use the old_count 
 * param. It will, in future use it for optimization.
 */
static void
update_menu_items(
    int		old_count,
    int		new_count
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
    DtbProjProjMainInfo	proj_w
)
{
    ProjModMenubuttonMenuItems	menu_rec;

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
    DtbProjProjMainInfo	proj_w
)
{
    ProjModMenubuttonMenuItems	menu_rec;
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
    DtbProjProjMainInfo	proj_w
)
{
    ProjModMenubuttonMenuItems	menu_rec;

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

int
proj_init(
)
{
    obj_add_rename_callback(projP_obj_renameOCB, "PROJ");
    obj_add_destroy_callback(projP_obj_destroyOCB, "PROJ");
    obj_add_update_callback(projP_obj_updateOCB, "PROJ");

    AB_project = obj_create(AB_TYPE_PROJECT, NULL);
    obj_set_name(AB_project, "project");
    abobj_show_tree(AB_project, TRUE);

    return 0;
}

/* This destroys the current project and creates
 * a new default project named "project" since
 * dtbuilder can never have a NULL project.
 */
int
proj_destroy_project(
    ABObj	proj
)
{
    obj_set_flag(proj, BeingDestroyedFlag);
    obj_destroy(proj);
    proj_create_new_proj(NULL);
    return OK;
}

int
proj_set_project(
    ABObj       proj
)
{
    if (proj != NULL)
    {
	/*
	 * Update project window and browsers
	 */
	AB_project = proj;

	/* Initialize the current module to NULL,
	 * since no modules are showing yet.
	 */
	proj_set_cur_module(NULL);

	cgen_notify_new_project(AB_project);
	appfw_notify_new_project(AB_project);

	return OK;
    }
    return -1;
}

ABObj
proj_get_project(
)
{
    return AB_project;
}

int
proj_set_cur_module(
    ABObj	module
)
{
    if ( (module != NULL) && (obj_get_type(module) != AB_TYPE_MODULE) )
	return -1;

    AB_cur_module = module;
    if (module != NULL)
	ab_update_stat_region(AB_STATUS_CUR_MODULE, obj_get_name(module));
    else
	ab_update_stat_region(AB_STATUS_CUR_MODULE, NULL);
	
    return OK;
}

ABObj
proj_get_cur_module(
)
{
    return AB_cur_module;
}

int
proj_rename_module(
    ABObj    module,
    STRING   name
)
{
    if ( !obj_is_module(module) )
	return -1;

    abobj_set_name(module, name);

    if (abobj_update_module_name(module) == -1)
	return -1;

    /* Update the browser */
    brws_update_node(module);

    /* Update the project window */
    proj_update_node(module);

    if (module == proj_get_cur_module())
    {
	if (ab_update_stat_region(AB_STATUS_CUR_MODULE, name) == -1)
	    return -1;
    }
    return 0;
}

/*
 * Action: Update "Module Path:" in the project window
 *	   whenever the user moves the pointer over a
 *	   module icon.
 */
static void
pointer_motion_proc(
    Widget widget, 
    XEvent *event, 
    String *params, 
    int num_params
)
{
    Vwr			v = NULL;
    VNode		node_under_pointer = NULL;
    ABObj		obj = NULL;
    XMotionEvent	*mevent = (XMotionEvent*)event;
    char		mod_path[MAXPATHLEN] = "";

    util_dprintf(2, "pointer_motion_proc:\n");
    XtVaGetValues(widget, XmNuserData, &v, NULL); 

    if (!v)
	return;

    node_under_pointer = vwr_locate_node(v, 
		        mevent->x, mevent->y);
    if (node_under_pointer != NULL)
    {
	/* USE METHODS !! */
	obj = (AB_OBJ *)node_under_pointer->obj_data;
	
	if (!obj)
	    return;

	util_dprintf(2, "\tmodule name is %s\n", obj_get_name(obj));

	/* If the module has not been saved, file will be NULL */
	if (obj_get_file(obj) != NULL)
	{
	    util_get_dir_name_from_path(obj_get_file(obj), mod_path,
			MAXPATHLEN);
	}
	else
	{
	    strcpy(mod_path, ".");
	}

	util_dprintf(2, "\tmodule path is %s\n", mod_path);

	if (!util_strempty(mod_path))
	{
	    util_dprintf(2, "\tupdating the project window status region\n");
	    proj_update_stat_region(PROJ_STATUS_MOD_PATH, mod_path);
	}
    }
    else
    {
	/* If we're not over a node, clear out the module
	 * path status label in the project window.
	 */
	util_dprintf(2, "\tpointer is not positioned over a node\n");
	proj_update_stat_region(PROJ_STATUS_MOD_PATH, NULL);
    }
}


static void
import_uil(
    STRING      uil_file_name
)
{
    ABObj       project = proj_get_project();
    ABObjPtr    uil_interface;
 
    if ((uil_interface = abuil_get_uil_file(uil_file_name, project))
            == (ABObjPtr) NULL)
    {
        fprintf(stderr, "Failed to import %s.\n", uil_file_name);
        return;
    }
    else
        fprintf(stderr, "Succesfully imported %s.\n", uil_file_name);
 
    obj_set_flag(project, SaveNeededFlag);
 
    /*
     * Realize the tree
     */
    abobj_show_tree(uil_interface, TRUE);
    abobj_set_save_needed(project, TRUE);
}

void
proj_open_proj_okCB(
    Widget      widget,
    XtPointer   client_data,
    XmSelectionBoxCallbackStruct *call_data
)
{
    STRING      	proj_filename = (STRING) NULL;
    XmString		xm_buf = (XmString) NULL;
    DtbObjectHelpData	help_data = NULL;

    proj_filename = (STRING)objxm_xmstr_to_str(call_data->value);

    /* If the file chooser selection text field is empty, return */
    if ( util_strempty(proj_filename) )
    {
	dtb_proj_no_name_msg_initialize(&dtb_proj_no_name_msg);
	(void)dtb_show_modal_message(widget,
                        &dtb_proj_no_name_msg, NULL, NULL, NULL);
    }
    else if (!util_file_exists(proj_filename))
    {
        sprintf(Buf, catgets(Dtb_project_catd, 100, 8,
		"The file %s does not exist."), proj_filename);
	util_printf_err(Buf);
    }
    else if (!util_file_is_regular_file(proj_filename))
    {
	sprintf(Buf, catgets(Dtb_project_catd, 100, 69,
                "Cannot open %s.\n%s is not a regular file."),
		proj_filename, proj_filename);
        xm_buf = XmStringCreateLocalized(Buf);
        dtb_proj_error_msg_initialize(&dtb_proj_error_msg);

	help_data = (DtbObjectHelpData) util_malloc(sizeof(DtbObjectHelpDataRec));
	help_data->help_text = catgets(Dtb_project_catd, 100, 89,
	    "The file you specified is a directory or\nanother special file.");
	help_data->help_volume = "";
	help_data->help_locationID = "";

        (void)dtb_show_modal_message(widget,
                        &dtb_proj_error_msg, xm_buf, help_data, NULL);

	util_free(help_data);
        XmStringFree(xm_buf);
    }
    else
    {   
        XtUnmanageChild(widget);	/* pop down the chooser */
	ab_check_and_open_bip(proj_filename); 
    }    
}

void
proj_cancelCB(
    Widget              widget,
    XtPointer           client_data,
    XmSelectionBoxCallbackStruct *call_data
)
{
    /* Pop down the file chooser.  This must be done
     * explicitly because the file chooser autoUnmanage
     * resource was set to FALSE.
     */
    XtUnmanageChild(widget);
}


void
proj_import_okCB(
    Widget              		widget,
    XtPointer				client_data,
    XmSelectionBoxCallbackStruct	*call_data
)
{
    STRING 	file_name = (STRING) NULL;
    XmString	xm_buf = (XmString) NULL;
    BOOL	read_OK, write_OK;
    DtbObjectHelpData	help_data = NULL;

    if (call_data->reason != XmCR_OK)
    {
        return;
    }

    file_name = (STRING)objxm_xmstr_to_str(call_data->value);

    /* If the file chooser selection text field is empty,
     * return, leaving the chooser up to give the user 
     * another try.
     */
    if (util_strempty(file_name))
    {
	dtb_proj_no_name_msg_initialize(&dtb_proj_no_name_msg);
	(void)dtb_show_modal_message(widget,
                        &dtb_proj_no_name_msg, NULL, NULL, NULL);
    }
    else if (!util_file_exists(file_name))
    {
	/* If the file specified does not exist, keep up the file
	 * chooser so that the user can specify another file.
	 */
	sprintf(Buf, catgets(Dtb_project_catd, 100, 8,
		"The file %s does not exist."), file_name);
	util_printf_err(Buf);
    }
    else if (!util_file_is_regular_file(file_name))
    {
        sprintf(Buf, catgets(Dtb_project_catd, 100, 70,
                "Cannot import %s.\n%s is not a regular file."),
		file_name, file_name);
        xm_buf = XmStringCreateLocalized(Buf);
        dtb_proj_error_msg_initialize(&dtb_proj_error_msg);

        help_data = (DtbObjectHelpData) util_malloc(sizeof(DtbObjectHelpDataRec));
        help_data->help_text = catgets(Dtb_project_catd, 100, 89, 
            "The file you specified is a directory or\nanother special file.");
        help_data->help_volume = ""; 
        help_data->help_locationID = ""; 
 
	(void)dtb_show_modal_message(widget,
                        &dtb_proj_error_msg, xm_buf, help_data, NULL);

	util_free(help_data);
        XmStringFree(xm_buf);
    }
    else
    {
	ChooserInfo	info = NULL;

 	XtVaGetValues(AB_generic_chooser,
			XmNuserData, &info,
			NULL);
	
	if (info->ImportAsBil)
	{
	    /* Pop down the file chooser */
	    XtUnmanageChild(widget);
	    ab_check_and_import_bil(file_name, info->ImportByCopy);
	}
	else 	/* Import as UIL */
	{
            abio_access_file(file_name, &read_OK, &write_OK);
            if (read_OK)
            {
		/* Pop down the file chooser */
		XtUnmanageChild(widget);
		import_uil(file_name);
            }
            else
            {
		sprintf(Buf, catgets(Dtb_project_catd, 100, 9,
			"%s does not have read permission."),
			file_name);
		util_printf_err(Buf);
            }
	}
    }
}

/*
 * Object rename callback
 * Update project organizer when module is renamed.
 */
static int
projP_obj_renameOCB(
    ObjEvAttChangeInfo    info
)
{
    ABObj	obj = info->obj;
    char	*name;

    /*
     * We only care about modules and projects here
     */
    if (!obj || (!obj_is_module(obj) && !obj_is_project(obj)))
	return (0);

    /*
     * Update the module name only if the old name is not NULL.
     *
     * If the old name is NULL that means this is a new object.
     * and we let the update callback handle that.
     */
    if (info->old_name != NULL)
        proj_update_node(obj);


    return (0);
}

/*
 * Object destroy callback
 * Remove viewer nodes from project organizer when module is destroyed.
 */
static int
projP_obj_destroyOCB(
    ObjEvDestroyInfo    info
)
{
    ABObj	obj = info->obj;

    if (!obj || (!obj_is_module(obj) && !obj_is_project(obj)))
	return (0);

    if (obj->projwin_data)
    {
	ABObj	proj = obj_get_project(obj);

	if (!proj)
	    return (0);

	/*
	 * If the project is being destroyed, skip if the object passed in is
	 * not the project itself. We do this so that proj_delete_objects() is
	 * not called for every module in the project. Instead, it is called
	 * just once for the project.
	 */
	if (obj_has_flag(proj, BeingDestroyedFlag) && !obj_is_project(obj))
	    return (0);

        proj_delete_objects(obj);
    }

    return (0);
}

/*
 * projP_obj_updateOCB
 * Update project organizer when new modules are added
 */
static int
projP_obj_updateOCB(
    ObjEvUpdateInfo	info
)
{
    ABObj	obj = info->obj;


    /*
     * We should only care about modules and projects
     * here.
     */
    if (!obj || (!obj_is_module(obj) && !obj_is_project(obj)))
	return (0);

    proj_add_objects(obj);

    return (0);
}

/*
 * Action: mouse button down on browser
 */
static void
project_bpress(
    Widget widget, 
    XEvent *event, 
    String *params, 
    int num_params
)
{
    XButtonEvent        *bevent;

    if (event->type == ButtonPress)
    {
        bevent = (XButtonEvent*)event;

        if (bevent->state == 0 && bevent->button == 1)/* RUBBERBAND SELECT */
        {
            if (ui_initiate_rubberband(widget, True, 
			project_rband, (XtPointer)NULL) == ERROR)
            {
                if (util_get_verbosity() > 0)
                    fprintf(stderr,"project organizer: couldn't begin rubberbanding\n");
            }
	    else
		mselect_adjust = False;
	}
        else if (bevent->button == 2 || /* RUBBERBAND ADJUST SELECT */
                 (bevent->button == 1 && (bevent->state & ShiftMask)))
	{
            if (ui_initiate_rubberband(widget, True, 
			project_rband, (XtPointer)NULL) == ERROR)
            {
                if (util_get_verbosity() > 0)
                    fprintf(stderr,"project organizer: couldn't begin rubberbanding\n");
            }
	    else
		mselect_adjust = True;
        }
    }
}

/*
 * EventHandler: project organizer draw area drag action ...
 */
static void
projP_drawarea_button_drag(
    Widget widget,
    XtPointer client_data,
    XEvent *event,
    Boolean *cont_dispatch
)
{
    if (event->type == MotionNotify)
    {
        if (((XMotionEvent*)event)->state & Button1Mask) /* RUBBERBAND SELECT */
	{
            ui_button_drag(widget, event, client_data);
	}
        else if (((XMotionEvent*)event)->state & Button2Mask) /* RUBBERBAND ADJUST */
        {
            ui_button_drag(widget, event, client_data);
        }
    }
}


static void
project_rband(
    Widget	widget,
    XEvent	*event,
    XRectangle	*rb_rect,
    XtPointer	client_data
)
{
    Vwr		v = NULL;
    VNode	*selected_nodes = NULL;
    VMethods	m;
    ABSelectedRec	old_sel, 
			new_sel;
    XRectangle	tmp_rect;
    int		num_selected = 0,
		num_cur_selected = 0,
		i;

    XtVaGetValues(widget, XmNuserData, &v, NULL); 

    if (!v)
	return;

    if (!(m = v->methods))
	return;
    
    /* If rubberband was drawn from lower-right to upper-left,
     * translate rect so that x,y is upper-left point in rectangle.
     */
    if (rect_right(rb_rect) < rb_rect->x ||
	rect_bottom(rb_rect) < rb_rect->y)
    {
	tmp_rect.x = rect_right(rb_rect);
	tmp_rect.y = rect_bottom(rb_rect);
	tmp_rect.width = rb_rect->x - tmp_rect.x;
	tmp_rect.height = rb_rect->y - tmp_rect.y;
	rb_rect = &tmp_rect;
    }

    /*
     * Important: set rband_rect so that
     * the function 'project_select_rband' can see it.
     */
    rband_rect = rb_rect;

    /*
     * Get list of currently selected modules in project
     * organizer
     */
    vwr_get_cond(v->current_tree, &selected_nodes, 
			&num_selected, select_fn);

    if (mselect_adjust)
	/*
	 * If select adjust, remember how many modules are currently
	 * selected
	 */
	num_cur_selected = num_selected;
    else
    {
	/*
	 * If not select adjust, the current select count is reset
	 */
	num_cur_selected = 0;

	/*
	 * Deselect all currently selected nodes
	 */
        for (i=0; i < num_selected; ++i)
	{
	    ABObj	obj;

	    /*
	     * Get ABObj
	     */
	    obj = (ABObj)selected_nodes[i]->obj_data;

	    if (!obj)
	        continue;

	    proj_deselect(obj);
	}
    }

    /*
     * Free up node list if it contained anything
     */
    if (selected_nodes)
        util_free(selected_nodes);

    /*
     * Get nodes in browser that are within the
     * rubber band rectangle.
     */
    vwr_get_cond(v->current_tree, &selected_nodes, 
			&num_selected, project_select_rband);
    
    /*
     * Update (activate/deactivate) project organizer menu items
     */
    update_menu_items(0, num_cur_selected + num_selected);

    /*
     * Return if no selected nodes
     */
    if (num_selected == 0)
	return;

    /*
     * For each object enclosed in rubber band rectangle
     */
    for (i=0; i < num_selected; ++i)
    {
	ABObj	obj;
	int	j;

	/*
	 * Get ABObj
	 */
	obj = (ABObj)selected_nodes[i]->obj_data;

	if (!obj)
	    continue;

	proj_select(obj);
    }

    /*
     * Unset rband_rect
     */
    rband_rect = NULL;

    /*
     * Free up node list if it contained anything
     */
    if (selected_nodes)
	util_free(selected_nodes);
}

void
proj_unmap_chooserCB(
    Widget	chooser,
    XtPointer	clientData,
    XtPointer 	callData
)
{
    ChooserInfo	info = NULL;

    XtVaGetValues(AB_generic_chooser, XmNuserData, &info, NULL);

    /* Destroy the "extra" file chooser child if one
     * exists (i.e. Save Project As, Import Module, etc.)
     */
    if (info->child != NULL)
    {
	XtDestroyWidget(info->child);
    }

    /* Set the appropriate File menu items and Project
     * Organizer Project and Module menu items active.
     */
    pal_set_File_menu(info->chooser_type, TRUE);
    proj_set_menus(info->chooser_type, TRUE);
}
