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
 *	$XConsortium: brws.c /main/4 1996/10/02 11:00:07 drk $
 *
 *	@(#)brws.c	1.113 22 May 1995
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
 * brws.c
 * This file contains the API/callbacks for the CDE App Builder interface browser.
 * Currently, it is a direct port of the Devguide browser. The contents of
 * this file will change soon...
 */

#include <sys/param.h>
#include <sys/types.h>

#include <stdlib.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <Xm/Xm.h>
#include <Xm/TextF.h>
#include <Xm/DialogS.h>
#include <Xm/SelectioB.h>
#include <Xm/ScrolledW.h>
#include <Xm/PushB.h>
#include <ab_private/obj.h>
#include <ab_private/obj_notify.h>
#include <ab_private/trav.h>
#include <ab_private/istr.h>
#include <ab/util_types.h>
#include <ab_private/abobj.h>
#include <ab_private/abobj_edit.h>
#include <ab_private/ui_util.h>
#include <ab_private/x_util.h>
#include <ab_private/prop.h>
#include <ab_private/proj.h>
#include <ab_private/brwsP.h>
#include <ab_private/ab.h>
#include <ab_private/message.h>
#include <ab_private/objxm.h>

/*************************************************************************
**                                                                      **
**       Private Function Declarations                                  **
**                                                                      **
**************************************************************************/

/*
 * routines called from Uil file via callbacks
 */
static void	browser_repaint(
		    Widget	widget,
		    XtPointer	client_data,
		    XtPointer	call_data
		);

static void	browser_resize(	
		    Widget	widget,
		    XtPointer	client_data,
		    XtPointer	call_data
		);

static void	browser_orientation(	
		    Widget	widget,
		    XtPointer	client_data,
		    XtPointer	call_data
		);

static void	browser_show_view_glyph(	
		    Widget	widget,
		    XtPointer	client_data,
		    XtPointer	call_data
		);

static void	browser_show_view_name(	
		    Widget	widget,
		    XtPointer	client_data,
		    XtPointer	call_data
		);

static void	browser_show_view_type(	
		    Widget	widget,
		    XtPointer	client_data,
		    XtPointer	call_data
		);

static void	browser_show_view_wclass(	
		    Widget	widget,
		    XtPointer	client_data,
		    XtPointer	call_data
		);

static void	browser_find_obj(	
		    Widget	widget,
		    XtPointer	client_data,
		    XtPointer	call_data
		);

static void	browser_tear_off(	
		    Widget	widget,
		    XtPointer	client_data,
		    XtPointer	call_data
		);

static void	browser_destroyCB(	/* Browser destroy procedure */
		    Widget	widget,
		    XtPointer	client_data,
		    XtPointer	call_data
		);

/*
 * Action Procs for module subwindow of the browser.
 */
static void    	browser_bpress(
		    Widget w, 
		    XEvent *ev, 
		    String *params, 
		    int nparams
        	);
static void    	browser_select(
		    Widget w, 
		    XEvent *ev, 
		    String *params, 
		    int nparams
        	);
static void    	browser_toggle_select(
		    Widget w, 
		    XEvent *ev, 
		    String *params, 
		    int nparams
        	);
static void    	browser_invoke_props(
		    Widget w, 
		    XEvent *ev, 
		    String *params, 
		    int nparams
        	);
static void    	browser_popup_menu(
		    Widget w, 
		    XEvent *ev, 
		    String *params, 
		    int nparams
        	);

static void	browser_rband(
		    Widget	widget,
		    XEvent	*event,
		    XRectangle	*rb_rect,
		    XtPointer	client_data
		);

/*
 * Action Procs for project subwindow of the browser.
 */
static void    	browser_proj_select(
		    Widget w, 
		    XEvent *ev, 
		    String *params, 
		    int nparams
        	);
static void    	browser_proj_toggle_select(
		    Widget w, 
		    XEvent *ev, 
		    String *params, 
		    int nparams
        	);
static void    	browser_proj_invoke_props(
		    Widget w, 
		    XEvent *ev, 
		    String *params, 
		    int nparams
        	);
static void    	browser_proj_popup_menu(
		    Widget w, 
		    XEvent *ev, 
		    String *params, 
		    int nparams
        	);

/*
 * Object notification callbacks
 */
static int	brwsP_obj_renameOCB(
		    ObjEvAttChangeInfo	info
		);

static int	brwsP_obj_destroyOCB(
		    ObjEvDestroyInfo	info
		);

static int	brwsP_obj_reparentedOCB(
		    ObjEvReparentInfo	info
		);

static int	brwsP_selectOCB(
		    ObjEvAttChangeInfo	info
		);

static int	brwsP_obj_updateOCB(
		    ObjEvUpdateInfo	info
		);

/*
 * Private functions within this file
 */
static ABBrowser	create_browser_struct();
static void		destroy_browser_struct(
                            ABBrowser	b
			);

static void		create_browser_ui_handles(
                            ABBrowser	b
                        );

static void		destroy_browser_ui_handles(
                            ABBrowser	b
                        );

static BrowserProps	aob_create_props();

static void		aob_destroy_ui_obj(
		    	    Vwr		b
			);

static void             brws_view_cascadeCB(
                            Widget      widget,
                            XtPointer   client_data,
                            XtPointer   call_data
                        );

static void		aob_destroy_props(
		    	    Vwr		b
			);

static Widget		get_main_window(
			    Widget	widget
			);

static int		select_rband(
			    VNode       vnode
			);

static int		select_fn(
			    VNode       vnode
			);

static void		brwsP_drawarea_button_drag(
			    Widget widget,
			    XtPointer client_data,
			    XEvent *event,
			    Boolean *cont_dispatch
			);

/*************************************************************************
**                                                                      **
**       Data     	                                                **
**                                                                      **
**************************************************************************/

/*
 * Browser Actions
 */
static XtActionsRec browser_actions[] = {
    {"BrowserButtonPress",        (XtActionProc)browser_bpress },
    {"BrowserSelect",        (XtActionProc)browser_select },
    {"BrowserToggleSelect",  (XtActionProc)browser_toggle_select },
    {"BrowserInvokeProps",   (XtActionProc)browser_invoke_props },
    {"BrowserPopupMenu",     (XtActionProc)browser_popup_menu },
    {"BrowserProjSelect",        (XtActionProc)browser_proj_select },
    {"BrowserProjToggleSelect",  (XtActionProc)browser_proj_toggle_select },
    {"BrowserProjInvokeProps",   (XtActionProc)browser_proj_invoke_props },
    {"BrowserProjPopupMenu",     (XtActionProc)browser_proj_popup_menu }
};

/* 
 * Translations for detailed view of browser
 */
static String base_translations =
    "Ctrl <Btn1Down>:		ObjectDragChord() \n\
     Shift <Btn1Down>:		BrowserButtonPress() \n\
     <Btn1Down>:      		BrowserButtonPress() \n\
     <Btn1Down>,<Btn1Up>:	BrowserSelect() \n\
     Shift <Btn1Down>,<Btn1Up>:	BrowserToggleSelect() \n\
     <Btn1Up>(2):		BrowserInvokeProps() \n"; /* will be appended-to */

static String btn2_adjust_translations =
    "<Btn2Down>:                BrowserButtonPress() \n\
     <Btn2Down>,<Btn2Up>:       BrowserToggleSelect() \n"; /* will be appended-to */

static String btn3_menu_translations =
    "<Btn3Down>:                BrowserPopupMenu() ";

static String btn2_menu_translations =
    "<Btn2Down>:                BrowserPopupMenu() ";

static XtTranslations browser_transtbl = NULL;

/* 
 * Translations for top level view of browser
 */
static String proj_base_translations =
    "Shift <Btn1Down>,<Btn1Up>: BrowserProjToggleSelect() \n\
     <Btn1Down>,<Btn1Up>:	BrowserProjSelect() \n\
     <Btn1Up>(2):		BrowserProjInvokeProps() \n"; /* will be appended-to */

static String proj_btn2_adjust_translations =
    "<Btn2Down>,<Btn2Up>:       BrowserProjToggleSelect() \n"; /* will be appended-to */

static XtTranslations browser_proj_transtbl = NULL;


static char	*browser_mainwindow = "mainwindow";

static char		*sm_font_name = "-*-courier-bold-r-normal--12-120-75-75-m-70-iso8859-1";

static XtTranslations	orig_draw_area_trans;
static Bool		orig_trans_set = False;
static XRectangle	*rband_rect = NULL;

static Boolean		mselect_adjust = False;

extern VMethods		brwsP_mod_methods;
extern VMethods		brwsP_proj_methods;
				

/*
 *********************************************
 * START OF ACTION PROCS FOR PROJECT SUBWINDOW
 *********************************************
 */

/*
 * Action: caused the browser object to become "selected"
 */
static void
browser_proj_select(
    Widget widget, 
    XEvent *event, 
    String *params, 
    int num_params
)
{
    ABBrowser	ab = NULL;
    Vwr		proj_b;
    Vwr		mod_b;
    VNode	proj_node;
    VNode	mod_node;
    AB_OBJ	*obj;

    XtVaGetValues(widget, XmNuserData, &ab, NULL); 

    if (!ab)
	return;

    proj_b = ab->project;
    mod_b = ab->module;

    if (!proj_b || !mod_b)
	return;

    proj_node = vwr_locate_node(proj_b, 
		        event->xbutton.x, 
			event->xbutton.y);

    if (proj_node)
    {
        ViewerMethods	*m;

        if (!(obj = (AB_OBJ *)proj_node->obj_data))
            return;

	if (!(mod_node = aob_find_bnode(obj, mod_b)))
	    return;

	/*
	 * Deselect all nodes in toplevel view
	 */
	aob_deselect_all_nodes(proj_b, TRUE);

        m = mod_b->methods;

	/*
	 * Set select state to TRUE for node
	 * Hilite node in project subwindow
	 */
        BRWS_NODE_SET_STATE(proj_node, BRWS_NODE_SELECTED);
        (*m->render_node)(proj_node, TRUE);

	/*
	 * Set shown state of node in module subwindow
	 */
        brwsP_sync_views(ab, FALSE);

	/*
	 * Erase and redraw module subwindow
	 */
        erase_viewer(mod_b);
        draw_viewer(mod_b);
    }
}

/*
 * Action: toggles the select-state of the browser object
 */
static void
browser_proj_toggle_select(
    Widget widget, 
    XEvent *event, 
    String *params, 
    int num_params
)
{
    ABBrowser	ab = NULL;
    Vwr		proj_b;
    Vwr		mod_b;
    VNode	proj_node;
    VNode	mod_node;
    AB_OBJ	*obj;

    XtVaGetValues(widget, XmNuserData, &ab, NULL); 

    if (!ab)
	return;

    proj_b = ab->project;
    mod_b = ab->module;

    if (!proj_b || !mod_b)
	return;

    proj_node = vwr_locate_node(proj_b, 
		        event->xbutton.x, 
			event->xbutton.y);

    if (proj_node)
    {
        VMethods	m;
	int		hilite;

        obj = (AB_OBJ *)proj_node->obj_data;

        if (!(obj = (AB_OBJ *)proj_node->obj_data))
            return;

	if (!(mod_node = aob_find_bnode(obj, mod_b)))
	    return;

        m = mod_b->methods;

	/*
	 * Toggle select state of node in toplevel view and 
	 * corresponding node in detail view
	 */
	if (BRWS_NODE_STATE_IS_SET(proj_node, BRWS_NODE_SELECTED))
	{
	    /*
	     * Set select state of node in toplevel view.
	     * Set hilite flag to be used later to render node
	     */
            BRWS_NODE_UNSET_STATE(proj_node, BRWS_NODE_SELECTED);
	    hilite = FALSE;

	    /*
	     * Set select state of node in detailed view
	     */
            BRWS_NODE_UNSET_STATE(mod_node, BRWS_NODE_VISIBLE);
	}
	else
	{
            BRWS_NODE_SET_STATE(proj_node, BRWS_NODE_SELECTED);
	    hilite = TRUE;

            BRWS_NODE_SET_STATE(mod_node, BRWS_NODE_VISIBLE);
	}

	/*
	 * Render node in toplevel view
	 */
        (*m->render_node)(proj_node, hilite);

	/*
	 * Render entire detailed view
	 */
        erase_viewer(mod_b);
        draw_viewer(mod_b);
    }
}

/*
 * Action: invokes the property sheet for the browser object
 */
static void
browser_proj_invoke_props(
    Widget widget, 
    XEvent *event, 
    String *params, 
    int num_params
)
{
    AB_OBJ	*obj;
    ABBrowser	ab = NULL;
    Vwr		b;
    VNode	selected_node;

    XtVaGetValues(widget, XmNuserData, &ab, NULL); 

    if (!ab)
	return;

    b = aob_proj_or_module(ab, widget);

    selected_node = vwr_locate_node(b, 
			event->xbutton.x, event->xbutton.y);

    if (selected_node)  
    {
	obj = (AB_OBJ *)selected_node->obj_data;

	if (!obj)
	    return;

	if (obj_is_message(obj))
	    msgEd_show_dialog();
	else
            prop_load_obj(obj, AB_PROP_REVOLVING);
    }
}

/*
 * Action: popup the build menu for the browser object
 */
static void
browser_proj_popup_menu(
    Widget widget, 
    XEvent *event, 
    String *params, 
    int num_params
)
{
    AB_OBJ	*obj;
    ABBrowser	ab = NULL;
    Vwr		b;
    VNode	selected_node;

    XtVaGetValues(widget, XmNuserData, &ab, NULL); 

    if (!ab)
	return;

    b = aob_proj_or_module(ab, widget);

    selected_node = vwr_locate_node(b, 
			event->xbutton.x, event->xbutton.y);

    if (selected_node)  
    {
	ABSelectedRec sel;

	obj = (AB_OBJ *)selected_node->obj_data;

	if (!obj)
	    return;

	abobj_get_selected(obj_get_project(obj), FALSE, TRUE, &sel);

	if (event->type == ButtonPress)
	    abobj_popup_menu(BROWSER_EDIT_MENU, widget, &sel, (XButtonEvent *)event);

	XtFree((char *)sel.list);
    }
}

/*
 *******************************************
 * END OF ACTION PROCS FOR PROJECT SUBWINDOW
 *******************************************
 */

/*
 ********************************************
 * START OF ACTION PROCS FOR MODULE SUBWINDOW
 ********************************************
 */
int
selected_fn(
    VNode	v
)
{
    if (BRWS_NODE_STATE_IS_SET(v, BRWS_NODE_SELECTED))
	return (1);

    return (0);
}

static int
select_rband(
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

static void
browser_rband(
    Widget	widget,
    XEvent	*event,
    XRectangle	*rb_rect,
    XtPointer	client_data
)
{
    ABBrowser	ab = NULL;
    Vwr		v = NULL;
    VNode	*selected_nodes = NULL;
    VMethods	m;
    ABSelectedRec	old_sel, 
			new_sel;
    XRectangle	tmp_rect;
    int		num_selected = 0,
		i;

    XtVaGetValues(widget, XmNuserData, &ab, NULL); 

    if (!ab)
	return;
    
    if (!(v = ab->module))
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
     * the function 'select_rband' can see it.
     */
    rband_rect = rb_rect;

    /*
     * Get nodes in browser that are within the
     * rubber band rectangle.
     */
    vwr_get_cond(v->current_tree, &selected_nodes, 
			&num_selected, select_rband);
    
    /*
     * Return if no selected nodes
     */
    if (num_selected == 0) {
	util_free(selected_nodes);
	return;
    }

    new_sel.list = (ABObj *)util_malloc(num_selected * sizeof(ABObj));
    new_sel.count = 0;

    /*
     * For each object enclosed in rubber band rectangle
     */
    for (i=0; i < num_selected; ++i)
    {
	ABObj	obj;
	int	j;

	/*
	 * Skip if browser node is not visible or collapsed
	 */
	if (!brwsP_node_is_visible(selected_nodes[i]) || 
		brwsP_node_is_collapsed(selected_nodes[i]))
	    continue;

	/*
	 * Get ABObj
	 */
	obj = (ABObj)selected_nodes[i]->obj_data;

	if (!obj)
	    continue;

	/*
	 * Get list of objects selected within the window of this
	 * object. Should optimize by caching this.
	 */
        abobj_get_selected(obj_get_window(obj), True, False, &old_sel);

	/*
	 * For each in current/old selected list (of the object's window)
	 */
	for (j = 0; j < old_sel.count; j++)
	{
	    /*
	     * Deselect object:
	     *
	     * If RUBBERBAND-SELECT, deselect all other currently selected
	     * objects.
	     * If RUBBERBAND-ADJUST, deselect only currently selected
	     * NON-sibling objects
	     */
	    if ((obj != old_sel.list[j]) &&
		(!mselect_adjust ||
		!obj_is_sibling(obj, old_sel.list[j])) )
	    {
		abobj_deselect(old_sel.list[j]);
	    }
	}

	/*
	 * Construct list of new objects to mark as selected
	 * 
	 * Mark this object as selected only if it is a sibling of any object
	 * on the same window already on the list.
	 *
	 * This can be optimized as well.
	 */
	if (new_sel.count == 0)
	{
	    /*
	     * If list is empty, just add obj to it
	     */
	    new_sel.list[new_sel.count] = obj;
	    new_sel.count++;
	}
	else
	{
	    Bool	added_to_list = False,
			window_found = False;

	    /*
	     * List is not empty.
	     * 2 things to check:
	     *	- does obj already have a sibling on the list? If yes, 
	     *	  insert obj
	     *	- is there an object on the list that is on the same window
	     *	  as obj? Non-siblings can be selected together if they are
	     *	  not on the same window.
	     */
	    for (j = 0; j < new_sel.count; ++j)
	    {
		/*
		 * Remember if an object on the same window was found
		 */
	        if (obj_get_window(obj) == obj_get_window(new_sel.list[j]))
		    window_found = True;

		/*
		 * Sibling found! Add obj to list
		 */
	        if (obj_is_sibling(obj, new_sel.list[j]))
	        {
	            new_sel.list[new_sel.count] = obj;
	            new_sel.count++;
		    added_to_list = True;
	            break;
	        }
	    }

	    /*
	     * Double check: Add obj to list if there is no object in
	     * the same window on the list yet.
	     */
	    if (!added_to_list && !window_found)
	    {
	        new_sel.list[new_sel.count] = obj;
	        new_sel.count++;
	    }
	}

	util_free(old_sel.list);
    }

    /*
     * Unset rband_rect
     */
    rband_rect = NULL;

    /*
     * Mark new objects as selected
     */
    for (i = 0; i < new_sel.count; ++i)
    {
	abobj_select(new_sel.list[i]);
    }

    /*
     * Free list of new selected objects
     */
    util_free(new_sel.list);

    /*
     * Free up node list if it contained anything
     */
    if (selected_nodes)
	util_free(selected_nodes);
}

/*
 * Action: mouse button down on browser
 */
static void
browser_bpress(
    Widget widget, 
    XEvent *event, 
    String *params, 
    int num_params
)
{
    XButtonEvent	*bevent;

    if (event->type == ButtonPress) 
    {
	bevent = (XButtonEvent*)event;

        if (bevent->state == 0 && bevent->button == 1)/* RUBBERBAND SELECT */
	{
            if (ui_initiate_rubberband(widget, True, 
			browser_rband, (XtPointer)NULL) == ERROR)
            {
                if (util_get_verbosity() > 0)
                    fprintf(stderr,"browser: couldn't begin rubberbanding\n");
            }
	    else
		mselect_adjust = False;
	}
	else if (bevent->button == 2 || /* RUBBERBAND ADJUST SELECT */
		 (bevent->button == 1 && (bevent->state & ShiftMask)))
	{
            if (ui_initiate_rubberband(widget, True, 
			browser_rband, (XtPointer)NULL) == ERROR)
            {
                if (util_get_verbosity() > 0)
                    fprintf(stderr,"browser: couldn't begin rubberbanding\n");
            }
	    else
		mselect_adjust = True;
        }
    }
}

/*
 * Action: caused the browser object to become "selected"
 */
static void
browser_select(
    Widget widget, 
    XEvent *event, 
    String *params, 
    int num_params
)
{
    ABBrowser	ab = NULL;
    Vwr		b;
    VNode	selected_node;
    AB_OBJ	*obj;
    BrowserUiObjects	*ui;

    XtVaGetValues(widget, XmNuserData, &ab, NULL); 

    if (!ab)
	return;

    b = aob_proj_or_module(ab, widget);

    ui = aob_ui_from_browser(b);

    if (!ui)
        return;

    selected_node = vwr_locate_node(b, 
		        event->xbutton.x, event->xbutton.y);

    if (selected_node)  
    {
        VMethods	m;
	unsigned long	elm_selected;

        m = b->methods;

	if (!m)
	    return;

        elm_selected = (*m->locate_elements)(selected_node, 
				event->xbutton.x, event->xbutton.y);

	obj = (AB_OBJ *)selected_node->obj_data;

	if (!obj)
	    return;

	if (util_get_verbosity() > 0)
	{
	    util_dprintf(1, "*********************\n");
	    util_dprintf(1, "Browser Node Selected - %s\n", 
			obj_get_name(obj));
	    util_dprintf(1, "Object %s is %s\n", 
		obj_get_name(obj),
		obj_is_initially_active(obj) ? 
		"initially active" : "NOT initially active");
	    if (obj->ui_handle)
	    {
	        int	num_children = 0;

	        util_dprintf(1, 
			"Ui handle - %p, Widget name: %s, class: %s\n", 
		    (objxm_get_widget(obj)),
		    XtName(objxm_get_widget(obj)),
		    obj_get_class_name(obj));

                XtVaGetValues(objxm_get_widget(obj), 
    	            XtNnumChildren,    &num_children,
    	            NULL);

	        util_dprintf(1, 
			"Widget %s has %d children\n", 
			XtName(objxm_get_widget(obj)),
			num_children);

	        objxm_dump_widget_geometry(objxm_get_widget(obj));
	    }
	    else
	    {
	        util_dprintf(1, "No ui handle\n");
	    }
	    util_dprintf(1, "*********************\n\n");

	}

	abobj_deselect_all(obj_get_project(obj));
	abobj_select(obj);
    }

}

/*
 * Action: toggles the select-state of the browser object
 */
static void
browser_toggle_select(
    Widget widget, 
    XEvent *event, 
    String *params, 
    int num_params
)
{
    AB_OBJ	*obj;
    ABBrowser	ab = NULL;
    Vwr		b;
    VNode	selected_node;

    XtVaGetValues(widget, XmNuserData, &ab, NULL); 

    if (!ab)
	return;

    b = aob_proj_or_module(ab, widget);

    selected_node = vwr_locate_node(b, 
			event->xbutton.x, event->xbutton.y);

    if (selected_node)  
    {

	obj = (AB_OBJ *)selected_node->obj_data;

	if (!obj)
	    return;

        if (obj_is_selected(obj_get_root(obj)))
	{
            abobj_deselect(obj);
	}
        else
	{
            ABSelectedRec 	sel;
	    BOOL		allow = TRUE;
	    int			i;

	    /* Only objects on the same level (siblings) may be
	     * selected at any one time, so deselect any non-sibling
	     * objects
	     */
	    abobj_get_selected(obj_get_window(obj), True, False, &sel);

	    for(i = 0; i < sel.count; i++)
	    {
	        if (!obj_is_sibling(obj, sel.list[i]))
		    allow = FALSE;
	    }
	    if (allow)
                abobj_select(obj);

	    util_free(sel.list);
	}
    }

}

/*
 * Action: invokes the property sheet for the browser object
 */
static void
browser_invoke_props(
    Widget widget, 
    XEvent *event, 
    String *params, 
    int num_params
)
{
    AB_OBJ	*obj;
    ABBrowser	ab = NULL;
    Vwr		b;
    VNode	selected_node;


    XtVaGetValues(widget, XmNuserData, &ab, NULL); 

    if (!ab)
	return;

    b = aob_proj_or_module(ab, widget);

    selected_node = vwr_locate_node(b, 
			event->xbutton.x, event->xbutton.y);

    if (selected_node)  
    {
	obj = (AB_OBJ *)selected_node->obj_data;

	if (!obj)
	    return;

	if (obj_is_message(obj))
	    msgEd_show_dialog();
	else
            prop_load_obj(obj, AB_PROP_REVOLVING);
    }
}

/*
 * Action: popup the build menu for the browser object
 */
static void
browser_popup_menu(
    Widget widget, 
    XEvent *event, 
    String *params, 
    int num_params
)
{
    AB_OBJ	*obj;
    ABBrowser	ab = NULL;
    Vwr		b;
    VNode	selected_node;
    ABSelectedRec sel;

    XtVaGetValues(widget, XmNuserData, &ab, NULL); 

    if (!ab)
	return;

    b = aob_proj_or_module(ab, widget);

    abobj_get_selected(proj_get_project(), FALSE, TRUE, &sel);

    if (event->type == ButtonPress)
        abobj_popup_menu(BROWSER_EDIT_MENU, widget, &sel, (XButtonEvent *)event);

    if (sel.list)
        XtFree((char *)sel.list);
}

/*
 ******************************************
 * END OF ACTION PROCS FOR MODULE SUBWINDOW
 ******************************************
 */

/*
 * Register build actions with Xt
 */
void
brws_register_actions(
    XtAppContext app
)
{
    XtAppAddActions(app, browser_actions, XtNumber(browser_actions));
}

/*
 * Enable browser actions for project subwindow
 */
static void
enable_proj_actions(
    Widget widget
)
{
    if (!orig_trans_set)
    {
	XtVaGetValues(widget, XtNtranslations, &orig_draw_area_trans, NULL);
	orig_trans_set = True;
    }

    if (browser_proj_transtbl == NULL)
    {
        int        len;
        String     translations, menu_translations;

        /* First time only.
         * Build up translation table based on the number of mouse buttons
         * and the value of the display resource, "enableBtn1Transfer"
         */
        menu_translations = 
		(AB_BMenu == Button3? btn3_menu_translations : btn2_menu_translations);

        len = strlen(proj_base_translations) + strlen(menu_translations) + 1;

        if (AB_BMenu == Button3 && AB_btn1_transfer == True)
            /* Button2 can be used for ADJUST */
            len += strlen(proj_btn2_adjust_translations);

        translations = (String)util_malloc(len*sizeof(char));
        if (translations == NULL)
            return; /* yikes */
 
        strcpy(translations, proj_base_translations);
        if (AB_BMenu == Button3 && AB_btn1_transfer == True)
           strcat(translations, proj_btn2_adjust_translations);
        strcat(translations, menu_translations);
	
	browser_proj_transtbl = XtParseTranslationTable(translations);

	util_free(translations);
    }

    /* enable build mode behavior */
    XtVaSetValues(widget, 
		XtNtranslations, browser_proj_transtbl,
        	NULL);
}

/*
 * Enable browser actions for module subwindow
 */
static void
enable_actions(
    Widget widget
)
{
    if (!orig_trans_set)
    {
	XtVaGetValues(widget, XtNtranslations, &orig_draw_area_trans, NULL);
	orig_trans_set = True;
    }

    if (browser_transtbl == NULL)
    {
        int        len;
        String     translations, menu_translations;

        /* First time only.
	 * Build up translation table based on the number of mouse buttons
	 * and the value of the display resource, "enableBtn1Transfer"
         */
        menu_translations = (AB_BMenu == Button3? btn3_menu_translations : btn2_menu_translations);

        len = strlen(base_translations) + strlen(menu_translations) + 1;

        if (AB_BMenu == Button3 && AB_btn1_transfer == True)
            /* Button2 can be used for ADJUST */
            len += strlen(btn2_adjust_translations);

        translations = (String)util_malloc(len*sizeof(char));
	if (translations == NULL)
	    return; /* yikes */

        strcpy(translations, base_translations);
        if (AB_BMenu == Button3 && AB_btn1_transfer == True)
           strcat(translations, btn2_adjust_translations);
        strcat(translations, menu_translations);

	browser_transtbl = XtParseTranslationTable(translations);

	util_free(translations);
    }

    /* enable build mode behavior */
    XtVaSetValues(widget, 
		XtNtranslations, browser_transtbl,
        	NULL);
}

/*
 * Disable browser actions for project subwindow
 */
static void
disable_proj_actions(
    Widget widget
)
{
    if (!orig_trans_set)
    {
	XtVaGetValues(widget, XtNtranslations, &orig_draw_area_trans, NULL);
	orig_trans_set = True;
    }

    /* enable build mode behavior */
    XtVaSetValues(widget, 
		XtNtranslations, orig_draw_area_trans,
        	NULL);
}

/*
 * Disable browser actions for module subwindow
 */
static void
disable_actions(
    Widget widget
)
{
    if (!orig_trans_set)
    {
	XtVaGetValues(widget, XtNtranslations, &orig_draw_area_trans, NULL);
	orig_trans_set = True;
    }

    /* enable build mode behavior */
    XtVaSetValues(widget, 
		XtNtranslations, orig_draw_area_trans,
        	NULL);
}

/*
 * EventHandler: browser draw area drag action ...
 */
static void
brwsP_drawarea_button_drag(
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
setup_callbacks(
    Widget	browser_widget,
    ABBrowser	browser
)
{
    BrowserUiObj	proj_ui,
			mod_ui;
    DtbBrwsMainwindowInfo	instance;

    proj_ui = (BrowserUiObj)browser->project->ui_handle;
    instance = (DtbBrwsMainwindowInfo)proj_ui->ip;

    enable_proj_actions(instance->toplevel_drawarea);
    enable_actions(instance->detailed_drawarea);

    XtVaSetValues(instance->toplevel_drawarea, 
		XmNuserData, (XtPointer)browser, NULL);
    XtVaSetValues(instance->detailed_drawarea, 
		XmNuserData, (XtPointer)browser, NULL);
    XtVaSetValues(instance->menubar_items.View_item, 
		XmNuserData, (XtPointer)browser, NULL);
    XtVaSetValues(instance->menubar_View_item_view_pulldown_items.Horizontal_item, 
		XmNuserData, (XtPointer)browser, NULL);
    XtVaSetValues(instance->menubar_View_item_view_pulldown_items.Hide_Object_Glyph_item, 
		XmNuserData, (XtPointer)browser, NULL);
    XtVaSetValues(instance->menubar_View_item_view_pulldown_items.Hide_Object_Name_item, 
		XmNuserData, (XtPointer)browser, NULL);
    XtVaSetValues(instance->menubar_View_item_view_pulldown_items.Show_Object_Type_item, 
		XmNuserData, (XtPointer)browser, NULL);
    XtVaSetValues(instance->menubar_View_item_view_pulldown_items.Collapse_item, 
		XmNuserData, (XtPointer)browser, NULL);
    XtVaSetValues(instance->menubar_View_item_view_pulldown_items.Expand_item, 
		XmNuserData, (XtPointer)browser, NULL);
    XtVaSetValues(instance->menubar_View_item_view_pulldown_items.Expand_All_item, 
		XmNuserData, (XtPointer)browser, NULL);

#ifdef BRWS_WIDGET_CLASS
    XtVaSetValues(instance->obj_wclass, 
		XmNuserData, (XtPointer)browser, NULL);
#endif

    XtVaSetValues(instance->menubar_View_item_view_pulldown_items.Find_item, 
		XmNuserData, (XtPointer)browser, NULL);
    XtVaSetValues(instance->menubar_View_item_view_pulldown_items.Tear_Off_Browser_item, 
		XmNuserData, (XtPointer)browser, NULL);

    XtVaSetValues(instance->menubar_View_item_view_pulldown_items.Module_item, 
		XmNuserData, (XtPointer)browser, NULL);

    XtVaSetValues(instance->menubar_items.Edit_item, 
		XmNuserData, (XtPointer)browser, NULL);

    XtVaSetValues(instance->menubar_View_item_view_pulldown,
		XmNuserData, (XtPointer)browser, NULL);

    XtVaSetValues(instance->view_pulldown_Module_item_view_module_pulldown2,
		XmNuserData, (XtPointer)browser, NULL);

    /*
     * Set callbacks
     */
    XtAddCallback(instance->menubar_items.Edit_item, XmNcascadingCallback, 
		brws_edit_cascadeCB, (XtPointer)NULL);
    XtAddCallback(instance->menubar_items.View_item, XmNcascadingCallback, 
		brws_view_cascadeCB, 
		(XtPointer)instance->menubar_View_item_view_pulldown_items.Module_item);

    /*
     * Set event handler on draw area for dragging/rubber banding
     */
    XtAddEventHandler(instance->detailed_drawarea, 
                Button1MotionMask | Button2MotionMask, False,
                brwsP_drawarea_button_drag, (XtPointer)NULL);

    brwsP_make_drawarea_snap(browser->module, instance->detailed_drawarea);
}

static int
select_fn(
    VNode       vnode
)
{
    if (BRWS_NODE_STATE_IS_SET(vnode, BRWS_NODE_SELECTED))
        return (1);
		 
    return (0);
}

void
brws_add_objects_to_browser
(
    ABBrowser	ab,
    AB_OBJ	*obj
)
{
    ViewerMethods	*m;
    ABObj		module;

    if (!ab)
	return;
    
    if (obj)
    {
	VNode	*selected_nodes = NULL;
	int	num_selected = 0;

        m = ab->project->methods;
        (*m->insert_tree)(ab->project, obj);

        m = ab->module->methods;
        (*m->insert_tree)(ab->module, obj);

	brwsP_sync_views(ab, TRUE);
    }

    erase_viewer(ab->project);
    draw_viewer(ab->project);

    erase_viewer(ab->module);
    draw_viewer(ab->module);
}

void
brws_delete_objects_from_browser (
    ABBrowser	ab,
    AB_OBJ	*obj
)
{
    ViewerMethods	*m;

    if (!ab)
	return;
    
    if (obj)
    {
        m = ab->project->methods;
        (*m->remove_tree)(ab->project, obj);

        m = ab->module->methods;
        (*m->remove_tree)(ab->module, obj);
    }

    /*
     * Redraw browser if it still contain objects, otherwise
     * destroy the browser
     */
    if (ab->project->tree && ab->module->tree)
    {
        erase_viewer(ab->project);
        draw_viewer(ab->project);

        erase_viewer(ab->module);
        draw_viewer(ab->module);
    }
    else
    {
        Widget	shell;

        shell = aob_ui_shell(ab->project);
	
	if (shell)
	    XtDestroyWidget(shell);
    }
}

void
brws_add_objects
(
    AB_OBJ	*obj
)
{
    AB_OBJ	*project, 
		*module;
    ABBrowser	new_b, cur_b, b_list = NULL;

    if (!obj)
	return;

    /*
     * Get project/module object
     */
    project = obj_get_project(obj);
    module = obj_get_module(obj);

    if (!project || !module)
	return;

    /*
     * Get browser list
     */
    b_list = (ABBrowser)project->info.project.browsers;

    /*
     * If no browser list, create browser for this project
     */
    if (!b_list)
	return;

    /*
     * If the object passed in was a project, we have to 
     * insert into the proper browsers the proper modules
     */
    if (obj == project)
    {
        AB_TRAVERSAL	trav;
        AB_OBJ		*cur_module;

        for (trav_open(&trav, project, AB_TRAV_MODULES);
            (cur_module = trav_next(&trav)) != NULL; )  {

            for (cur_b = b_list; cur_b; cur_b = cur_b->next)
            {
		AB_OBJ		*browsed_module;

		browsed_module = (AB_OBJ *)cur_b->project->obj_data;

		if (browsed_module == cur_module)
		{
	            brws_add_objects_to_browser(cur_b, browsed_module);
		}
	    }

        }

	return;
    }

    /*
     * Populate the object subtree with browser data for all 
     * browsers on the browser list. We do this for:
     *	module subwindow
     *	tree subwindow
     */
    for (cur_b = b_list; cur_b; cur_b = cur_b->next)
    {
        ViewerMethods	*m;

#ifdef POPULATE_EMPTY_BROWSER
	if (!cur_b->project->tree || 
	    (cur_b->project->tree && 
		((AB_OBJ *)cur_b->project->tree->obj_data == module)))
#else
	if (cur_b->project->tree && 
		((AB_OBJ *)cur_b->project->tree->obj_data == module))
#endif
	    brws_add_objects_to_browser(cur_b, obj);

    }

}

void
brws_delete_objects
(
    AB_OBJ	*obj
)
{
    AB_OBJ	*project;
    ABBrowser	cur_b, b_list = NULL;

    if (!obj)
	return;

    project = obj_get_project(obj);

    if (!project)
	return;

    b_list = (ABBrowser)project->info.project.browsers;

    for (cur_b = b_list; cur_b; cur_b = cur_b->next)
    {
        brws_delete_objects_from_browser(cur_b, obj);
    }
}

/*
 * create_browser_window - creates a new browser main window
 */
ABBrowser brws_create(void)
{
    ABBrowser	b;

    b = create_browser_struct();
    create_browser_ui_handles(b);

    return(b);
}

/*
 * brws_destroy - destroys browser
 */
void
brws_destroy(
    ABBrowser	b
)
{
    destroy_browser_ui_handles(b);
    destroy_browser_struct(b);
}

/*
 * Map browser
 */
void
brws_popup
(
    ABBrowser	b
)
{
    Widget	shell;
    WidgetList	children;

    shell = aob_ui_shell(b->module);

    if (!shell)
	return;

    XtVaGetValues(shell, XmNchildren, &children, NULL);
    ab_show_window(children[0]);
}

/*
 * Unmap browser
 */
void
brws_popdown
(
    ABBrowser	b
)
{
    Widget	shell;

    shell = aob_ui_shell(b->module);

    if (!shell)
	return;

    ui_win_show(shell, False, XtGrabNone);
}

/*
 * Create the windows of the browser
 */
static void
create_browser_ui_handles
(
    ABBrowser	b
)
{
    DtbBrwsMainwindowInfo	instance;
    BrowserUiObj	module_ui;
    BrowserUiObj	project_ui;
    Widget		browser_main = NULL;
    XmString		xmlabel;
    char		*tmp;
    char		title[100];

    if (!b || !b->module || b->module->ui_handle)
	return;
    
    project_ui = aob_create_ui_obj();
    module_ui = aob_create_ui_obj();
    project_ui->shell = module_ui->shell = NULL;

    /*
     * Create the browser widgets
     */

    instance = (DtbBrwsMainwindowInfo)malloc(sizeof(DtbBrwsMainwindowInfoRec));
    (void)dtbBrwsMainwindowInfo_clear(instance);

    (void)dtb_brws_mainwindow_initialize(instance, AB_toplevel);
    module_ui->shell = project_ui->shell = instance->mainwindow;
    XtVaSetValues(project_ui->shell, XmNmappedWhenManaged, False, NULL);

    XtVaSetValues(instance->menubar,
        XmNmenuHelpWidget, instance->menubar_items.Help_item,
        NULL);

    XtAddCallback(project_ui->shell, XtNdestroyCallback, 
		browser_destroyCB, (XtPointer)b);

    browser_main = instance->mainwindow_mainwin;

    XtVaSetValues(browser_main, XmNuserData, instance, NULL);
    project_ui->ip = module_ui->ip = (void *)instance;

    XtRealizeWidget(project_ui->shell);

    b->project->ui_handle = (void *)project_ui;
    b->module->ui_handle = (void *)module_ui;

    /*
     * Setup callbacks so that the user data passed == the browser
     * pointer
     */
    setup_callbacks(browser_main, b);

    /*
     * Register it so that it will participate in the dtbuilder
     * window protocol
     */
    ab_register_window(browser_main, AB_WIN_WINDOW, 
		WindowHidden, AB_toplevel, AB_WPOS_TILE_LEFT,
		NULL, NULL);

    /* reset the deleteResponse back to the default since a close just
     * means this browser window should be destroyed
     */
    XtVaSetValues(project_ui->shell, XmNdeleteResponse, XmDESTROY, NULL);

    /*
     * End of browser widget creation
     */

    setup_vwr_graphics(b->module);
    setup_vwr_graphics(b->project);

    /*
     **********************************
     * Put project name in window title
     **********************************
     */
    brws_set_module_name(b->module);
}

/*
 * Destroy the UI objects of the browser
 */
static void
destroy_browser_ui_handles(
    ABBrowser	b
)
{
    BrowserUiObjects	*ui;
    DtbBrwsMainwindowInfo	instance = NULL;

    if (!b)
	return;
    
    /*
     * Free ui handles for project viewer
     */
    if (b->project && b->project->ui_handle)
    {
        ui = aob_ui_from_browser(b->project);

	/*
	 * Save pointer to instance structure so that we
	 * can free it later
	 */
	instance = (DtbBrwsMainwindowInfo)ui->ip;

        aob_free_graphics(b->project);

	if (ui->shell)
	{
	    /*
	     * If the shell is already being destroyed,
	     * this will return right away
	     */
            XtDestroyWidget(ui->shell);
	    ui->shell = NULL;
	}

        aob_destroy_ui_obj(b->project);
    }

    /*
     * Free ui handles for module viewer
     */
    if (b->module && b->module->ui_handle)
    {
        ui = aob_ui_from_browser(b->module);
	instance = (DtbBrwsMainwindowInfo)ui->ip;

        aob_free_graphics(b->module);

	if (ui->find_box)
	{
	    brwsP_destroy_find_box(b->module);
	}

	if (ui->shell)
	{
            XtDestroyWidget(ui->shell);
	    ui->shell = NULL;
	}

        aob_destroy_ui_obj(b->module);
    }

    /*
     * Free the ip structure
     */
    free(instance);
}


/*
 * Setup the graphics context and stuffs like that.
 * This routine has to be called before any of
 * the browser drawing routines is called. 
 */
void
setup_vwr_graphics(
    Viewer	*b
)
{

    BrowserUiObjects	*ui_handle;
    XGCValues		gcvalues;
    Display		*dpy;
    Widget		draw_area;

    if (!b || !b->ui_handle)
	return;

    ui_handle = (BrowserUiObjects*)b->ui_handle;
    draw_area = brws_draw_area(b);
    dpy = XtDisplay(draw_area);

    if (!ui_handle->bg_font) 
    {
	Font	font = 0;

        XtVaGetValues(draw_area, XtNfont, &font, NULL);
        ui_handle->bg_font = (XFontStruct *)XQueryFont(dpy, font);
    }

    if (!ui_handle->normal_gc) 
    {

	XtVaGetValues(draw_area, 
		XtNbackground, &ui_handle->bg_color,
		XtNforeground, &ui_handle->fg_color,
		NULL);

	gcvalues.graphics_exposures = False;
	gcvalues.background = ui_handle->bg_color;
	gcvalues.foreground = ui_handle->fg_color;
	gcvalues.line_width = BRWS_ELM_BORDER_WIDTH;
	ui_handle->normal_gc = XCreateGC(dpy, XtWindow(draw_area),
		GCForeground|GCBackground|GCGraphicsExposures|GCLineWidth, &gcvalues);
    }

    if (!ui_handle->select_gc) 
    {

	gcvalues.graphics_exposures = False;
	gcvalues.background = WhitePixelOfScreen(XtScreen(draw_area));
	gcvalues.foreground = BlackPixelOfScreen(XtScreen(AB_toplevel));
	gcvalues.line_width = BRWS_ELM_BORDER_WIDTH;
	ui_handle->select_gc = XCreateGC(dpy, XtWindow(draw_area),
		GCForeground|GCBackground|GCGraphicsExposures|GCLineWidth, &gcvalues);
    }

    if (!ui_handle->line_gc) 
    {

	gcvalues.graphics_exposures = False;
	gcvalues.background = ui_handle->bg_color;
	gcvalues.foreground = ui_handle->fg_color;
	gcvalues.line_width = BRWS_LINE_WIDTH;
	ui_handle->line_gc = XCreateGC(dpy, XtWindow(draw_area),
		GCForeground|GCBackground|GCGraphicsExposures|GCLineWidth, &gcvalues);
    }

    if (!ui_handle->sm_font) 
    {
        ui_handle->sm_font = (XFontStruct *)XLoadQueryFont(dpy, sm_font_name);
        XSetFont(dpy, ui_handle->normal_gc, ui_handle->sm_font->fid);
        XSetFont(dpy, ui_handle->select_gc, ui_handle->sm_font->fid);
    }
}

/*
 * Free the graphics context, and fonts.
 */
void
aob_free_graphics(
    Viewer	*b
)
{

    BrowserUiObjects	*ui_handle;
    XGCValues		gcvalues;
    Display		*dpy;
    Widget		draw_area;

    if (!b || !b->ui_handle)
	return;

    ui_handle = (BrowserUiObjects*)b->ui_handle;
    draw_area = brws_draw_area(b);
    dpy = XtDisplay(draw_area);

    if (ui_handle->normal_gc) 
    {
	XFreeGC(dpy, ui_handle->normal_gc);
    }

    if (ui_handle->select_gc) 
    {
	XFreeGC(dpy, ui_handle->select_gc);
    }

    if (ui_handle->line_gc) 
    {
	XFreeGC(dpy, ui_handle->line_gc);
    }

    if (ui_handle->sm_font) 
    {
	XFreeFont(dpy, ui_handle->sm_font);
    }

    if (ui_handle->bg_font) 
    {
	XFreeFont(dpy, ui_handle->bg_font);
    }
}

/*
 * Allocate a big enough canvas if the current canvas is not
 * big enough.
 */
static void
reallocate_drawarea
(
    Viewer	*v,
    int		width,
    int		height
)
{
    XRectangle	w_rect;            /* widget width,height,x,y */
    Widget	draw_area;
    Boolean	resize_needed = False;
    Arg		arg[4];
    int		num_args = 0;

    if (!v)
        return;

    draw_area = brws_draw_area(v);

    if (!draw_area)
        return;

    XtVaGetValues(draw_area,
        XtNwidth,       &(w_rect.width),
        XtNheight,      &(w_rect.height), 
        NULL);
	
    if (width != w_rect.width)
    {
	XtSetArg(arg[num_args], XtNwidth, width);
	num_args++;
	resize_needed = True;
    }

    if (height != w_rect.height)
    {
	XtSetArg(arg[num_args], XtNheight, height);
	num_args++;
	resize_needed = True;
    }
    
    if (resize_needed)
    {
        BrowserProps	props;

        props = aob_browser_properties(v);

        if (props)
        {
            props->min_width = (Dimension)width;
            props->min_height = (Dimension)height;
        }

        XtSetValues(draw_area, arg, num_args);
    }
}

/*
 * Recalculates the coordinates of the node of the tree.
 */
void
recompute_viewer(
    Viewer	*v
)
{
    ViewerMethods	*m;
    int			end_y = BRWS_Y_ORIGIN,
			end_x = BRWS_X_ORIGIN,
			max_x = BRWS_X_ORIGIN,
			max_y = BRWS_Y_ORIGIN;

    if (!v)
	return;

    m = v->methods;

    (*m->compute_tree)(v, &end_x, &end_y);

    reallocate_drawarea(v, end_x, end_y);
}

/*
 * browser_show_view_elements
 * This is used by all the callbacks that toggle the hide/show
 * state of the browser node elements. This function toggles
 * the bit (determined by 'mask') in the 'elements_shown' bit 
 * vector. The bit vector is hung off the Viewer structure.
 *
 *	b		Ptr to Viewer.
 *	mask		Mask to get to the desired field in
 *			the bit vector.
 *	widget		Cascade button widget that activated the 
 *			callback.
 *	set_str		String to set if bit is set.
 *	unset_str	String to set if bit is unset.
 */
void
browser_show_view_elements(
    Viewer		*b,
    unsigned long	mask,
    Widget		widget,
    char		*set_str,
    char		*unset_str
)
{
    BrowserProperties	*props;
    XmString			xmlabel;

    if (!b )
	return;

    props = aob_browser_properties(b);

    if (!props)
	return;

    if (props->elements_shown & mask)
    {
        if (!(props->elements_shown & ~mask))
	    return;

        props->elements_shown &= ~mask;
    }
    else
        props->elements_shown |= mask;

    if (props->elements_shown & mask)
    {
        xmlabel = XmStringCreateLocalized(set_str);
    }
    else  
    {
        xmlabel = XmStringCreateLocalized(unset_str);
    }

    XtVaSetValues(widget, XmNlabelString, xmlabel, NULL);

    XmStringFree(xmlabel);

    erase_viewer(b);
    draw_viewer(b);
}



/****************************************************************************/


/*
 * aob_proj_init_state
 * Initializes the state of a project in the browser.
 * This means:
 *	Making all but the first module visible
 *	Making the first browser node in the project subwindow 
 *	selected
 */
void
aob_proj_init_state
(
    ABBrowser	ab
)
{
    Vwr			proj_b;
    Vwr			mod_b;
    VNode		top, child;
    ViewerMethods	*m;
    int			i, num_child, first_found = FALSE;

    if (!ab)
	return;


    proj_b = ab->project;
    mod_b = ab->module;

    if (!proj_b || !mod_b)
	return;

    top = proj_b->tree;
    m = proj_b->methods;

    num_child = (*m->get_num_children)(top);

    for (i=0, child = (*m->get_child)(top, 0); (i< num_child);
		child = (*m->get_child)(top, ++i))
    {
	AB_OBJ	*obj;
	VNode	module_node;

        obj = (AB_OBJ *)child->obj_data;
        module_node = aob_find_bnode(obj, mod_b);

        if (!first_found)
        {
	    if (child)
	    {
		BRWS_NODE_SET_STATE(child, BRWS_NODE_SELECTED);

		BRWS_NODE_SET_STATE(module_node, BRWS_NODE_VISIBLE);

		first_found = TRUE;
	    }
        }
	else
	{
	    if (child)
	    {
		BRWS_NODE_UNSET_STATE(module_node, BRWS_NODE_VISIBLE);
	    }
	}
    }
}

/*
 * Initialize the object browser.
 */
static ABBrowser
create_browser_struct(void)
{
    ABBrowser		b;

    /*
     * Create AB browser struct
     */
    b = (ABBrowser)malloc(sizeof(AB_BROWSER));

    /*
     * Create browser structs for project/module windows
     */
    b->project = vwr_create(brwsP_proj_methods);
    b->module = vwr_create(brwsP_mod_methods);
    b->previous = b->next = NULL;

    return b;
}

/*
 * Destroy the object browser.
 */
static void
destroy_browser_struct(
    ABBrowser		b
)
{

    VMethods	m;
    VNode	top;
    ABObj	root;

    /*
     *********************************
     * Free resources in module viewer
     *********************************
     */
    m = b->module->methods;
    top = b->module->tree;
    root = top ? (ABObj)top->obj_data : NULL;
    (*m->remove_tree)(b->module, root);
    vwr_destroy(b->module);

    /*
     **********************************
     * Free resources in project viewer
     **********************************
     */
    m = b->project->methods;
    top = b->project->tree;
    root = top ? (ABObj)top->obj_data : NULL;
    (*m->remove_tree)(b->project, root);
    vwr_destroy(b->project);

    /*
     * Free browser
     */
    free(b);
}


/*
 * Destroy the interface browser object.
 */
void
aob_destroy(Viewer *b)
{
    Widget	shell;

    shell = aob_ui_shell(b);
    XtDestroyWidget(shell);
    vwr_destroy(b);
}


/*
 * Activate an object browser.
 */
void
aob_activate(Viewer *b)
{
    Widget	shell;

    shell = aob_ui_shell(b);

    if (b)  {
        ui_win_show(shell, True, XtGrabNone);
        aob_redraw(b);
    }
}


/*
 * Redraw the browser object.  Only draw when the window
 * is visible.
 */
void
aob_redraw
(
    Viewer *b
)
{
    erase_viewer(b);
    draw_viewer(b);
}


/*
 * (Re)Draw the viewer
 */
void
draw_viewer(Viewer *v)
{
    ViewerMethods	*m;
    ViewerNode	*node;

    if (!v->current_tree || !v->tree)
        return;

    m = v->methods;

    recompute_viewer(v);

    (*m->render_tree)(v->current_tree);
}


/*
 * Create (malloc) structure to hold browser UI objects
 */
BrowserUiObj aob_create_ui_obj(void)
{
    BrowserUiObj	ui;

    ui = (BrowserUiObj)malloc(sizeof(BrowserUiObjects));

    ui->ip = NULL;
    ui->shell = NULL;
    ui->find_box = NULL;
    ui->normal_gc = NULL;
    ui->select_gc = NULL;
    ui->line_gc = NULL;
    ui->sm_font = NULL;
    ui->bg_font = NULL;
    ui->fg_color = 0;
    ui->bg_color = 0;
    
    return(ui);
}

/*
 * Create (malloc) structure to hold browser properties
 */
static BrowserProps   
aob_create_props(void)
{
    BrowserProps	props;

    props = (BrowserProps)malloc(sizeof(BrowserProperties));

    props->elements_shown = 0;
    props->initial_state = 0;
    props->min_width = 0;
    props->min_height = 0;
    props->orientation = BRWS_VERTICAL;
    props->show_mult_trees = FALSE;
    props->active = FALSE;

    return(props);
}

/*
 * Free structure that holds browser UI objects
 */
static void            
aob_destroy_ui_obj
(
    Vwr		v
)
{
    if (!v && !v->ui_handle)
	return;

    free(v->ui_handle);

    v->ui_handle = (void *)NULL;
}

/*
 * Free structure that holds browser properties
 */
static void             
aob_destroy_props
(
    Vwr		b
)
{
    if (!b && !b->properties)
	return;

    free(b->properties);
}

/*
 * browser_destroyCB - destroy callback for browser window
 * Does 2 things:
 *	Remove browser from linked list of browsers on project
 *	Destroys browser
 */
static void		
browser_destroyCB
(
    Widget	widget,
    XtPointer	client_data,
    XtPointer	call_data
)
{
    ABBrowser		b = (ABBrowser)client_data,
			prev, next;
    ABObj		project = NULL;

    if (!b || !b->module)
	return;

    project = (AB_OBJ *)b->module->obj_data;

    /*
     * If obj_data was NULL, the browser is empty.
     * Use the current project instead.
     */
    if (!project)
	project = proj_get_project();

    /*
     * Unlink this browser from the list of browsers on
     * the project
     */
    prev = b->previous;
    next = b->next;

    if (prev)
    {
	/*
	 * Link previous to next browser
	 */
        prev->next = next;
    }
    else
    {
	/*
	 * This browser is the first on the list.
	 * Update browser pointer on project
	 * Check if the browser pointers match first
	 */
	if (project && (project->info.project.browsers == b))
            project->info.project.browsers = (void *)next;
    }
	
    if (next)
    {
	/*
	 * Link next to previous browser
	 */
        next->previous = prev;
    }

    /*
     * Destroy browser
     */
    brws_destroy(b);
}

Vwr
aob_proj_or_module
(
    ABBrowser	b,
    Widget	w
)
{
    
    Widget	proj_draw_area, draw_area;

    proj_draw_area = brws_draw_area(b->project);
    draw_area = brws_draw_area(b->module);

    if (w == proj_draw_area)
	return (b->project);

    if (w == draw_area)
	return (b->module);
    
    return (NULL);
}

int
aob_is_widget_proj_browser
(
    ABBrowser	b,
    Widget	w
)
{
    Widget	proj_draw_area, draw_area;

    proj_draw_area = brws_draw_area(b->project);
    draw_area = brws_draw_area(b->module);

    if (w == proj_draw_area)
	return (TRUE);

    return (FALSE);
}

int
aob_is_proj_browser
(
    ABBrowser	ab,
    Vwr		b
)
{
    if (b == ab->project)
	return (TRUE);

    return (FALSE);
}


/*
 * Return the number of nodes selected in the current tree.
 */
int
number_of_selected
(
    VNode    tree
)
{
    ViewerMethods	*m;
    VNode		child;
    int			i, num_child, total = 0;

    if (!tree)
	return (0);

    m = BNODE_METHODS(tree);

    num_child = (*m->get_num_children)(tree);

    /*
     * Recursively count selected nodes of child subtree
     * This proc should check for (child == NULL) 
     */
    for (i=0, child = (*m->get_child)(tree, 0); 
            (i < num_child); 
            child = (*m->get_child)(tree, ++i))
    {
        total = number_of_selected(child) + total;	
    }
    return total + (BRWS_NODE_STATE_IS_SET(tree, BRWS_NODE_SELECTED) ? 
			1 : 0);
}


/*
 * Return the first node selected.
 */
VNode
node_selected
(
    VNode	tree
)
{
    ViewerMethods	*m;
    VNode		child;
    VNode		selected_node;
    int			i, num_child;
	
    if (!tree)
	return (NULL);

    m = BNODE_METHODS(tree);

    num_child = (*m->get_num_children)(tree);

    /*
     * Recursively look in child subtrees
     * This proc should check for (child == NULL) 
     */
    for (i=0, child = (*m->get_child)(tree, 0); 
            (i < num_child); 
            child = (*m->get_child)(tree, ++i))
    {
        if (selected_node = node_selected(child))
        return selected_node;
    }

    if (BRWS_NODE_STATE_IS_SET(tree, BRWS_NODE_SELECTED))
        return tree;
    else
        return NULL;
}

/*
 * aob_set_mode
 *	Hides/Disables or Shows/Enables browsers depending on
 *	the value of AB_builder_mode
 */
void
aob_set_mode
(
    AB_OBJ	*project
)
{
    ABBrowser	new_b, cur_b, b_list = NULL;

    if (!project)
	return;

    if (!obj_is_project(project))
	project = obj_get_project(project);

    if (!project)
	return;

    b_list = (ABBrowser)project->info.project.browsers;

    for (cur_b = b_list; cur_b; cur_b = cur_b->next)
    {
	Widget		proj_draw_area, draw_area;

        proj_draw_area = brws_draw_area(cur_b->project);
        draw_area = brws_draw_area(cur_b->module);

        if (AB_builder_mode == MODE_BUILD)
        {
	    /*
	     * Enable translations for browser
	     */
            enable_proj_actions(proj_draw_area);
            enable_actions(draw_area);

	    /*
	     * Map browser
	     */
	    brws_popup(cur_b);
        }
        else
        {
	    /*
	     * Disable translations for browser
	     */
            disable_proj_actions(proj_draw_area);
            disable_actions(draw_area);

	    /*
	     * Unmap browser
	     */
	    brws_popdown(cur_b);
        }
    }
}

void
brws_switch_module(
    Widget		widget,
    XtPointer		client_data,
    XtPointer		call_data
)
{
    ABBrowser		ab = NULL;
    ABObj		module = (ABObj)client_data;
    ViewerMethods	*m;
    ABSelectedRec	sel;

    XtVaGetValues(widget, XmNuserData, &ab, NULL); 

    if (!ab)
	return;

    if (ab->project->tree && ab->project->current_tree)
    {
        m = ab->project->methods;
        (*m->remove_tree)(ab->project, ab->project->tree->obj_data);
        ab->project->tree = ab->project->current_tree = NULL;
    }

    if (ab->module->tree && ab->module->current_tree)
    {
        m = ab->module->methods;
        (*m->remove_tree)(ab->module, ab->module->tree->obj_data);
        ab->module->tree = ab->module->current_tree = NULL;
    }

    if (!module && ab->project)
	brws_set_module_name(ab->project);

    brws_add_objects_to_browser(ab, module);

    /*
     * Get selected objects in this module
     */
    abobj_get_selected(module, False, False, &sel);

    /*
     * Center view on first object in sel.list
     */
    if (sel.count > 0)
    {
        brws_center_on_obj(ab, sel.list[0]);
        util_free(sel.list);
    }
}

/*
 * Cascade callback for edit cascade button
 * This function checks the clipboard as well as the number of 
 * objects that are currently selected and sets the sensitivity of the
 * edit menu items appropriately.
 */

void
brws_edit_cascadeCB(
    Widget      widget, 
    XtPointer   client_data,
    XtPointer   call_data 
)    
{
    ABObj		project = proj_get_project();
    ABSelectedRec	sel;
    ABBrowser		ab = NULL;
    DtbBrwsMainwindowInfo	instance;
    BrowserUiObj	top_ui;


    XtVaGetValues(widget, XmNuserData, &ab, NULL); 

    if (!project || !ab)
	return;

    top_ui = (BrowserUiObj)ab->project->ui_handle;
    instance = (DtbBrwsMainwindowInfo)top_ui->ip;

    abobj_get_selected(project, FALSE, FALSE, &sel);

    if (sel.count == 0)
    {
	XtVaSetValues(instance->menubar_Edit_item_edit_pulldown_items.Cut_item, XmNsensitive, False, NULL);
	XtVaSetValues(instance->menubar_Edit_item_edit_pulldown_items.Copy_item, XmNsensitive, False, NULL);
	XtVaSetValues(instance->menubar_Edit_item_edit_pulldown_items.Paste_item, XmNsensitive, False, NULL);
	XtVaSetValues(instance->menubar_Edit_item_edit_pulldown_items.Delete_item, XmNsensitive, False, NULL);
    }
    else
    {
	XtVaSetValues(instance->menubar_Edit_item_edit_pulldown_items.Cut_item, XmNsensitive, True, NULL);
	XtVaSetValues(instance->menubar_Edit_item_edit_pulldown_items.Copy_item, XmNsensitive, True, NULL);
	XtVaSetValues(instance->menubar_Edit_item_edit_pulldown_items.Delete_item, XmNsensitive, True, NULL);

	if (abobj_clipboard_is_empty())
	    XtVaSetValues(instance->menubar_Edit_item_edit_pulldown_items.Paste_item, XmNsensitive, False, NULL);
	else
	    XtVaSetValues(instance->menubar_Edit_item_edit_pulldown_items.Paste_item, XmNsensitive, True, NULL);

        XtFree((char *)sel.list);
    }

    if (abobj_undo_active())
	XtVaSetValues(instance->menubar_Edit_item_edit_pulldown_items.Undo_item, XmNsensitive, True, NULL);
    else
	XtVaSetValues(instance->menubar_Edit_item_edit_pulldown_items.Undo_item, XmNsensitive, False, NULL);

}


static Widget
get_main_window(
    Widget	widget
)
{
    Widget	browser_main = NULL,
		cur_widget = widget;

    while (cur_widget && !browser_main)
    {
	if (!strcmp(XtName(cur_widget), browser_mainwindow))
	    browser_main = cur_widget;
	else
	    cur_widget = XtParent(cur_widget);
    }

    return (browser_main);
}

static void
brws_view_cascadeCB(
    Widget      widget, 
    XtPointer   client_data,
    XtPointer   call_data 
)    
{
    ABBrowser		ab = NULL;
    BrowserUiObj	top_ui;
    DtbBrwsMainwindowInfo	instance;
    ABObj		project = proj_get_project();
    ABSelectedRec	sel;
    Widget		browse, browse_pulldown;

    browse = (Widget) client_data;
    XtVaGetValues(widget, XmNuserData, &ab, NULL); 

    if (!browse || !ab)
	return;

    /*
     * Build view -> module menu
     */
    XtVaSetValues(browse, XmNsensitive, True, NULL);
    XtVaGetValues(browse, XmNsubMenuId, &browse_pulldown, NULL);

    brws_build_module_menu(browse_pulldown, brws_switch_module);

    /*
     * Desensitize collapse/expand menu items
     */
    top_ui = (BrowserUiObj)ab->project->ui_handle;
    instance = (DtbBrwsMainwindowInfo)top_ui->ip;

    abobj_get_selected(project, FALSE, FALSE, &sel);


    if (sel.count == 0)
    {
	XtVaSetValues(instance->menubar_View_item_view_pulldown_items.Expand_item, 
		XmNsensitive, False, NULL);
	XtVaSetValues(instance->menubar_View_item_view_pulldown_items.Collapse_item, 
		XmNsensitive, False, NULL);
    }
    else
    {
	XtVaSetValues(instance->menubar_View_item_view_pulldown_items.Expand_item, 
		XmNsensitive, True, NULL);
	XtVaSetValues(instance->menubar_View_item_view_pulldown_items.Collapse_item, 
		XmNsensitive, True, NULL);

        XtFree((char *)sel.list);
    }
}

/*****************************************************************
 *  This routine creates the pushButtons for the View->Browse
 *  menu.  The pushButtons contain the modules which are 
 *  currently in the ABObj tree. This function is called by
 *  view_cascadeCB().
 *****************************************************************/
void
brws_build_module_menu(
    Widget              pulldown,
    XtCallbackProc 	callback
) 
{ 
    ABObj		project = proj_get_project();
    Widget		menu = NULL, mpb = NULL;
    WidgetList		children = NULL;
    AB_TRAVERSAL	trav;
    ABObj		module = NULL;
    ABBrowser		ab = NULL;
    STRING		name = NULL;
    XmString		label = NULL;
    Cardinal		numChildren = 0;
    int			i = 0;
    Boolean		shown_modules_exist = False;

    XtVaGetValues(pulldown, XmNuserData, &ab, NULL);

    XtVaGetValues(pulldown, 
			XmNnumChildren, &numChildren, 
			XmNchildren, &children,
			NULL);
    for (i=0; i < numChildren; i++)
    {
	XtDestroyWidget(children[i]);
    }
		
    /* Traverse ABObj tree for module nodes.
     * Populate the pulldown menu with the names
     * of the module nodes.
     */
    for (trav_open(&trav, project, AB_TRAV_MODULES);
	(module = trav_next(&trav)) != NULL; )
    {
	/* Can only browse modules that are shown
	 * and defined. Undefined modules can occur 
	 * if a module is imported which references 
	 * another module that does not exist in the 
	 * project (i.e. :win-children).
         */
	if ( !obj_has_flag(module, MappedFlag) || 
	     !obj_is_defined(module)
	   )
	{
	    continue;
	}

	/* Use the name of the mapped module for the View
	 * menu items.
	 */
	name = obj_get_name(module);
	if (!util_strempty(name))
	{
	    label = XmStringCreateLocalized(name);
	    mpb = XtVaCreateManagedWidget(name,
		xmPushButtonWidgetClass,
		pulldown,
		XmNlabelString,	label,
		XmNuserData, ab,
		NULL);
	    XtAddCallback(mpb, XmNactivateCallback, callback,(XtPointer)module);
	    XmStringFree(label);
	
	    shown_modules_exist = True;
	}
    }
    trav_close(&trav);

    if (!shown_modules_exist)
    {
	label = XmStringCreateLocalized(
	    catgets(Dtb_project_catd, 100, 229, "No modules to browse"));
	mpb = XtVaCreateManagedWidget(name,
		xmPushButtonWidgetClass,
		pulldown,
		XmNlabelString,	label,
		XmNuserData, ab,
		NULL);
	XmStringFree(label);
    }
} 

/*
 * brws_show_browser - maps the App Builder Object browser
 * The contents of the mapped browser depends on what is passed as
 * client_data. If it is NULL, an empty browser will be shown. If it
 * is not NULL, it should be a module AB_OBJ. In this case, the module
 * will be loaded into the browser.
 */
void
brws_show_browser(
    Widget		widget,
    XtPointer		client_data,
    XtPointer		call_data
)
{
    ABObj	module = (AB_OBJ *)client_data;
    ABBrowser	cur_b;

    /*
     * Search for browser containing module
     */
    cur_b = brws_get_browser_for_obj(module);

    /*
     * Center browser detailed tree view on any object
     * that was selected that is in this module
     */
    if (module)
    {
	ABSelectedRec	sel;

	/*
	 * Get selected objects in this module
	 */
	abobj_get_selected(module, False, False, &sel);

	/*
	 * Center view on first object in sel.list
	 */
	if (sel.count > 0)
	{
	    brws_center_on_obj(cur_b, sel.list[0]);
	    util_free(sel.list);
	}
    }

    /*
     * Map the browser that as found
     */
    brws_popup(cur_b);
}

void
brws_init(void)
{
    obj_add_rename_callback(brwsP_obj_renameOCB, "BRWS");
    obj_add_destroy_callback(brwsP_obj_destroyOCB, "BRWS");
    obj_add_reparent_callback(brwsP_obj_reparentedOCB, "BRWS");
    obj_add_selected_change_callback(brwsP_selectOCB, "BRWS");
    obj_add_update_callback(brwsP_obj_updateOCB, "BRWS");
}

/*
 * Object rename callback
 * Update browsers when object is renamed. This is also used for
 * adding newly created objects.
 */
static int
brwsP_obj_renameOCB(
    ObjEvAttChangeInfo    info
)
{
    ABObj	obj = info->obj;

    if (!obj)
	return (0);

    /*
     * Update the name only if the old name was not NULL
     * which means that this is a new object. We let the
     * update callback handle that.
     */
    if (info->old_name != NULL)
        brws_update_node(obj);

    return (0);
}

/*
 * Object destroy callback
 * Remove viewer nodes from browsers when object is destroyed.
 */
static int
brwsP_obj_destroyOCB(
    ObjEvDestroyInfo    info
)
{
    ABObj	obj = info->obj;

    if (!obj)
	return (0);

    if (obj->browser_data)
    {
	ABObj	project;

	if (!(project = obj_get_project(obj)))
	    return (0);

	if (obj_has_flag(project, BeingDestroyedFlag) && 
	    !(obj_is_project(obj) || obj_is_module(obj)))
	    return (0);

        brws_delete_objects(obj);
    }

    return (0);
}

/*
 * Object reparent callback
 * Resync tree in browsers.
 */
static int
brwsP_obj_reparentedOCB(
    ObjEvReparentInfo     info
)
{
    ABObj	obj = info->obj,
		old_parent = info->old_parent,
		new_parent;
    
    if (!obj | !old_parent)
	return (0);

    new_parent = obj_get_parent(obj);

    brws_add_objects(new_parent);

    return (0);
}

static int
brwsP_selectOCB(
    ObjEvAttChangeInfo	info
)
{
    ABObj	obj = info->obj;

    /*
     * If NULL object or if object does
     * not belong to a project, return
     * right away
     */
    if (!obj || !(obj_get_project(obj)))
	return (0);

    if (obj_is_selected(obj))
        brws_select(obj);
    else
        brws_deselect(obj);

    return (0);
}

static int
brwsP_obj_updateOCB(
    ObjEvUpdateInfo	info
)
{
    ABObj	obj = info->obj,
		project = NULL,
		module = NULL;
    ABBrowser   b_list = NULL,
		cur_b;

    if (!obj)
	return (0);

    brws_add_objects(obj);

    /*
     * If a module was hidden, hide the browsers
     * associated with it.
     */
    project = obj_get_project(obj);
    module = obj_get_module(obj);
    if (!project || !module)
	return (0);
    
    if (obj_has_flag(module, MappedFlag))
	return (0);

    b_list = (ABBrowser)project->info.project.browsers;

    if (!b_list)
	return (0);

    for (cur_b = b_list; cur_b; cur_b = cur_b->next)
    {
        AB_OBJ	*browsed_module;

        browsed_module = (AB_OBJ *)cur_b->project->tree->obj_data;

        if (browsed_module == module)
	{
            Widget	shell;

            shell = aob_ui_shell(cur_b->project);
	
	    /*
	     * Destroy shell widget for browser
	     * The destroy callbacks will do cleanup...
	     */
	    if (shell)
	        XtDestroyWidget(shell);
	}
    }

    return (0);
}
