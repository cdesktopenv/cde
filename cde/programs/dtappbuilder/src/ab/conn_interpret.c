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
 *      $XConsortium: conn_interpret.c /main/3 1995/11/06 17:25:58 rswiston $
 *
 * @(#)conn_interpret.c	1.58 12 Apr 1995 cde_app_builder/src/ab
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
#include <Dt/SpinBox.h>
#include <ab_private/XmAll.h>
#include <ab_private/obj.h>
#include <ab_private/trav.h>
#include <ab_private/istr.h>
#include <ab_private/ab.h>
#include <ab_private/abobj.h>
#include <ab_private/abobj_set.h>
#include <ab_private/connP.h>
#include <ab_private/proj.h>
#include <ab/util_types.h>
#include <ab_private/util_ds.h>
#include <ab_private/help.h>
#include <ab_private/ui_util.h>
#include <ab_private/tmodeP.h>

typedef	void	(*INTERPRET_HANDLER)(
		    AB_ACTION_INFO	*,
		    XtCallbackProc	when_cb
		);
typedef	void	(*ACTION_HANDLER)(
		    AB_ACTION_INFO	*
		);

static XtCallbackProc		get_when_callback(AB_WHEN when);
static INTERPRET_HANDLER	get_when_attach_handler(AB_WHEN	when);
static INTERPRET_HANDLER	get_when_detach_handler(AB_WHEN	when);
static ACTION_HANDLER		get_std_action_handler(
				    AB_BUILTIN_ACTION std_act
				);

static void	attach_interpretation(
    ABObj	action
);
static void	attach_for_single_refs(
		    BOOL		for_from,
		    ABObj		*item_refs,
		    AB_ACTION_INFO	*info,
		    INTERPRET_HANDLER	handler,
		    XtCallbackProc	when_callback
		);
static void	attach_for_both_refs(
		    ABObj		*from_refs,
		    ABObj		*to_refs,
		    AB_ACTION_INFO	*info,
		    INTERPRET_HANDLER	handler,
		    XtCallbackProc	when_callback
		);
static void	detach_interpretation(
                    ABObj	action
                );
static void	tree_set_initial_state(
		    ABObj	obj,
		    BOOL	ignore_win_visibility
		);
static void	tree_set_obj_initial_state(
		    ABObj	obj
		);
static void	tree_restore_build_state(
		    ABObj	obj
		);
static void	set_obj_initial_state(
		    ABObj	obj
		);
static void	set_initial_state(
		    ABObj	trg_obj,
		    BOOL	ignore_win_visibility
		);
static void	restore_build_state(
		    ABObj	trg_obj
		);
static int	setup_window_for_test_mode(
		    ABObj	win
		);
static void	deiconify_wins(
		    void	*cl_data
		);
static void	make_win_iconic(
		    ABObj	win,
		    BOOL	to_iconify
		);
static void	remove_win_close(
		    void	*cl_data
		);
static void	destroy_menus(
		    void	*cl_data
		);
static void	sensitize_objects(
		    void	*cl_data
		);
static void	free_infos(
		    void	*cl_data
		);
static void	reset_layer(
		    void	*cl_data
		);
static void	manage_objs(
		    void	*cl_data
		);

static ACTION_HANDLER	get_action_handler(
		    AB_ACTION_INFO	*notify_info
		);

static void	activate_attach_handler(
		    AB_ACTION_INFO	*notify_info,
		    XtCallbackProc	when_cb
		);
static void	activate_detach_handler(
		    AB_ACTION_INFO	*notify_info,
		    XtCallbackProc	when_cb
		);
static void	post_menu_attach_handler(
		    AB_ACTION_INFO	*notify_info,
		    XtCallbackProc	when_cb
		);
static void	post_menu_detach_handler(
		    AB_ACTION_INFO	*notify_info,
		    XtCallbackProc	when_cb
		);

static void	activate_when_cb(
		    Widget	w,
		    XtPointer	client_data,
		    XtPointer	call_data
		);
static void	post_menu_when_cb(
		    Widget	w,
		    XtPointer	client_data,
		    XtPointer	call_data
		);

static void	user_def_fn_handler(
		    AB_ACTION_INFO	*action_info
		);
static void	code_frag_handler(
		    AB_ACTION_INFO	*action_info
		);
static void 	on_item_help_handler( 
		    AB_ACTION_INFO      *action_info 
		);
static void 	help_volume_handler( 
		    AB_ACTION_INFO      *action_info 
		);
static void	show_action_handler(
		    AB_ACTION_INFO	*action_info
		);
static void	hide_action_handler(
		    AB_ACTION_INFO	*action_info
		);
static void	enable_action_handler(
		    AB_ACTION_INFO	*action_info
		);
static void	disable_action_handler(
		    AB_ACTION_INFO	*action_info
		);
static void	set_text_action_handler(
		    AB_ACTION_INFO	*action_info
		);
static void	set_value_action_handler(
		    AB_ACTION_INFO	*action_info
		);
static void	set_label_action_handler(
		    AB_ACTION_INFO	*action_info
		);
static void	show_help_action_handler(
		    AB_ACTION_INFO	*action_info
		);


static void	tree_enable_help(
		    ABObj root
		);

static void	tree_disable_help(
		    ABObj root
		);

static void     win_closeCB(
                    Widget      w,
                    XtPointer   client_data,
                    XtPointer   call_data
                );


static XtCallbackProc
get_when_callback(AB_WHEN	when)
{
    switch (when)
    {
      case AB_WHEN_UNDEF:		return (XtCallbackProc)NULL;
      case AB_WHEN_ACTIVATED:		return activate_when_cb;
      case AB_WHEN_DESTROYED:		return (XtCallbackProc)NULL;
      case AB_WHEN_DROPPED_ON:		return (XtCallbackProc)NULL;
      case AB_WHEN_DOUBLE_CLICKED_ON:	return (XtCallbackProc)NULL;
      case AB_WHEN_ITEM_SELECTED:	return (XtCallbackProc)NULL;
      case AB_WHEN_POPPED_DOWN:		return (XtCallbackProc)NULL;
      case AB_WHEN_POPPED_UP:		return (XtCallbackProc)NULL;
      case AB_WHEN_REPAINT_NEEDED:	return (XtCallbackProc)NULL;
      case AB_WHEN_NUM_VALUES:		return (XtCallbackProc)NULL;
    }
    return (XtCallbackProc)NULL;
}

static INTERPRET_HANDLER
get_when_attach_handler(AB_WHEN	when)
{
    switch (when)
    {
      case AB_WHEN_UNDEF:		return (INTERPRET_HANDLER)NULL;
      case AB_WHEN_ACTIVATED:		return activate_attach_handler;
      case AB_WHEN_AFTER_CREATED:	return (INTERPRET_HANDLER)NULL;
      case AB_WHEN_DESTROYED:		return (INTERPRET_HANDLER)NULL;
      case AB_WHEN_DROPPED_ON:		return (INTERPRET_HANDLER)NULL;
      case AB_WHEN_DOUBLE_CLICKED_ON:	return (INTERPRET_HANDLER)NULL;
      case AB_WHEN_ITEM_SELECTED:	return (INTERPRET_HANDLER)NULL;
      case AB_WHEN_POPPED_DOWN:		return (INTERPRET_HANDLER)NULL;
      case AB_WHEN_POPPED_UP:		return (INTERPRET_HANDLER)NULL;
      case AB_WHEN_REPAINT_NEEDED:	return (INTERPRET_HANDLER)NULL;
      case AB_WHEN_NUM_VALUES:		return (INTERPRET_HANDLER)NULL;
    }
    return (INTERPRET_HANDLER)NULL;
}

static INTERPRET_HANDLER
get_when_detach_handler(AB_WHEN	when)
{
    switch (when)
    {
      case AB_WHEN_UNDEF:		return (INTERPRET_HANDLER)NULL;
      case AB_WHEN_ACTIVATED:		return activate_detach_handler;
      case AB_WHEN_AFTER_CREATED:	return (INTERPRET_HANDLER)NULL;
      case AB_WHEN_DESTROYED:		return (INTERPRET_HANDLER)NULL;
      case AB_WHEN_DROPPED_ON:		return (INTERPRET_HANDLER)NULL;
      case AB_WHEN_DOUBLE_CLICKED_ON:	return (INTERPRET_HANDLER)NULL;
      case AB_WHEN_ITEM_SELECTED:	return (INTERPRET_HANDLER)NULL;
      case AB_WHEN_POPPED_DOWN:		return (INTERPRET_HANDLER)NULL;
      case AB_WHEN_POPPED_UP:		return (INTERPRET_HANDLER)NULL;
      case AB_WHEN_REPAINT_NEEDED:	return (INTERPRET_HANDLER)NULL;
      case AB_WHEN_NUM_VALUES:		return (INTERPRET_HANDLER)NULL;
    }
    return (INTERPRET_HANDLER)NULL;
}

static ACTION_HANDLER
get_std_action_handler(AB_BUILTIN_ACTION std_act)
{
    switch (std_act)
    {
      case AB_STDACT_UNDEF:		return (ACTION_HANDLER)NULL;
      case AB_STDACT_DISABLE:		return disable_action_handler;
      case AB_STDACT_ENABLE:		return enable_action_handler;
      case AB_STDACT_HIDE:		return hide_action_handler;
      case AB_STDACT_SET_LABEL:		return set_label_action_handler;
      case AB_STDACT_SET_TEXT:		return set_text_action_handler;
      case AB_STDACT_SET_VALUE:		return set_value_action_handler;
      case AB_STDACT_SHOW:		return show_action_handler;
      case AB_BUILTIN_ACTION_NUM_VALUES:return (ACTION_HANDLER)NULL;
    }
    return (ACTION_HANDLER)NULL;
}


static BOOL		is_enabled = FALSE;

static ABObj	*	get_menu_item_refs(
			    ABObj	menu_item
			);

/*
 * These variables are used to track information to correct when transitioning
 * from test mode back to build mode.
 *
 * conn_test_mode_initialize() and conn_enable_action_interpret() initialize
 * the lists.
 *
 * conn_test_mode_cleanup() and conn_disable_action_interpret() revert what
 * was done during test mode and free the lists.
 */

	       /* conn_{enable,disable}_action_interpret() */
static ASet		ConnP_manage; /* objs to manage - initially visible & hidden */
static ASet		ConnP_senses;	       /* objs to senitize */
static LList		ConnP_menu_item_infos; /* objs to free item infos from */

		/* conn_test_mode_{initialize,cleanup}() */
static ASet		ConnP_win_close;       /* dialogs, FSB(s) to rm close cb from */
static ASet		ConnP_modules_disable; /* modules to disable */
static ASet		ConnP_wins_deiconify;  /* windows to deiconify */
static ASet		ConnP_layer;	       /* layers that were shown or hidden */

/*
 * Final entry into test mode.
 *
 * Windows need to be mapped/unmapped, visibility set/unset, sensitivity
 * set/unset, connection inperpretations setup, help setup, window close
 * callbacks added, and ? (think that's it).
 */
extern void
conn_enable_action_interpret(
    ABObj	project,
    BOOL	to_test_project
)
{
    if (project == NULL || !obj_is_project(project))
	return;

    ConnP_manage          = util_aset_create();
    ConnP_senses          = util_aset_create();
    ConnP_menu_item_infos = util_llist_create();

    if (to_test_project)
	tree_set_initial_state(project, FALSE);
    else
    {
        ABObj		mod;
        AB_TRAVERSAL	trav;

        for (trav_open(&trav, project, AB_TRAV_MODULES);
             (mod = trav_next(&trav)) != NULL; )
            if (obj_is_module(mod) && obj_has_flag(mod, MappedFlag))
            	tree_set_initial_state(mod, TRUE);
        trav_close(&trav);
    }
    is_enabled = TRUE;
}

/*
 * Cleanup on exit from Test mode
 */
extern void
conn_disable_action_interpret(
    ABObj	project
)
{
    ABObj		obj;
    AB_TRAVERSAL	trav;

    if (!is_enabled)
	return;

    if (project == NULL || !obj_is_project(project))
	return;

    tree_restore_build_state(project);

    util_aset_iterate(ConnP_manage, manage_objs);
    util_aset_iterate(ConnP_senses, sensitize_objects);
    util_llist_iterate(ConnP_menu_item_infos, free_infos);

    util_llist_destroy(ConnP_menu_item_infos);
    util_aset_destroy(ConnP_manage);
    util_aset_destroy(ConnP_senses);

    is_enabled = FALSE;
}

/*
 * recurse throught the tree, setting the initial state of the objects
 */
static void
tree_set_initial_state(
    ABObj	obj,
    BOOL	ignore_win_visibility
)
{
    ABObj		child;
    AB_TRAVERSAL	trav;

    if (obj_is_menu_item(obj))
    {
	ABObj	*ref_list = get_menu_item_refs(obj);
	ABObj	*cur_obj;

	if (ref_list != NULL)
	{
	    for (cur_obj = ref_list; *cur_obj != NULL; cur_obj++)
		set_initial_state(*cur_obj, ignore_win_visibility);
	    XtFree((char *)ref_list);
	}
    }
    else if (!obj_is_virtual(obj))
	set_initial_state(obj, ignore_win_visibility);

    for (trav_open(&trav, obj, AB_TRAV_SALIENT_CHILDREN);
	 (child = trav_next(&trav)) != NULL; )
	tree_set_initial_state(child, ignore_win_visibility);
    trav_close(&trav);
}

/*
 * recurse throught the tree, setting the initial state of the objects
 */
static void
tree_set_obj_initial_state(
    ABObj	obj
)
{
    ABObj		child;
    AB_TRAVERSAL	trav;

    if (obj_is_menu_item(obj))
    {
	ABObj	*ref_list = get_menu_item_refs(obj);
	ABObj	*cur_obj;

	if (ref_list != NULL)
	{
	    for (cur_obj = ref_list; *cur_obj != NULL; cur_obj++)
		set_obj_initial_state(*cur_obj);
	    XtFree((char *)ref_list);
	}
    }
    else if (!obj_is_virtual(obj))
	set_obj_initial_state(obj);

    for (trav_open(&trav, obj, AB_TRAV_SALIENT_CHILDREN);
	 (child = trav_next(&trav)) != NULL; )
	tree_set_obj_initial_state(child);
    trav_close(&trav);
}

static void
tree_restore_build_state(
    ABObj	obj
)
{
    ABObj		child;
    AB_TRAVERSAL	trav;

    if (!obj_is_virtual(obj))
	restore_build_state(obj);

    for (trav_open(&trav, obj, AB_TRAV_SALIENT_CHILDREN);
	 (child = trav_next(&trav)) != NULL; )
	tree_restore_build_state(child);
    trav_close(&trav);
}

static void
set_obj_initial_state(
    ABObj	obj
)
{
    if (!obj || !objxm_get_widget(obj))
	return;
    
    if (!obj_is_initially_active(obj))
    {
	XtSetSensitive(objxm_get_widget(obj), FALSE);

	/* apply to the actual XmList also */
	if (obj_is_list(obj))
	    XtSetSensitive(objxm_get_widget(
		               objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ)),
			   FALSE);
	
	if (!obj_is_menu_item(obj))
	    util_llist_insert_after((LList)ConnP_senses, obj);
    }

    if (!obj_is_window(obj) && !obj_is_initially_visible(obj))
    {
	ui_set_visible(objxm_get_widget(obj), FALSE);
	util_aset_add(ConnP_manage, obj);
    }
}

static void
set_initial_state(
    ABObj	trg_obj,
    BOOL	ignore_win_visibility
)
{
    /*
     * Basic Algorithm:
     *  trg_obj is a window (e.g. BASE_WINDOW, DIALOG or FILE_CHOOSER):
     *				Initial state
     *			Visible				Not Visible
     *
     *		Shown	  Do nothing			  Unmap
     *
     *		Hidden	XmConfig module if not already	  Do nothing
     *			done, and map window/dialog
     *
     *	all other ui elements:
     *		if window/dialog it belongs to is mapped,
     *			do the active/inactive setting
     *                  set the visibility
     *		else
     *			do nothing.
     *
     * This applies directly if we are going to TestProject mode (in which
     * case ignore_win_visibility will be FALSE).  If we are going to
     * TestShowModules mode (ignore_win_visibility is TRUE) then we modify
     * the above algorithm so that it ignores the current state but is able
     * to reset to it when we return to Build mode (in case any connections
     * have hidden/shown something).
     */
    AB_OBJECT_TYPE	trg_type = obj_get_type(trg_obj);

    if (obj_is_window(trg_obj))
    {
	BOOL is_visible = obj_is_initially_visible(trg_obj);

	/* Are we going to TestShownModules mode? */
	if(ignore_win_visibility)
	{
            if (setup_window_for_test_mode(trg_obj) == -1) goto cret;

	    /* 
	    ** For TestShownModules, if the object is currently invisible
	    ** show it.
	    */
	    if (!obj_has_flag(trg_obj, MappedFlag))
	    {
		if (objxm_tree_map(trg_obj, TRUE) == -1) goto cret;
	    }

	    /* 
	    ** If the object is supposed to be iconic (and visible) 
	    ** make it iconic, but remember that we have to deiconify
	    ** it we go back to Build mode
	    */
	    if (obj_is_initially_iconic(trg_obj))
	    {
		make_win_iconic(trg_obj, TRUE);
		util_llist_insert_after(ConnP_wins_deiconify, trg_obj);
	    }
	}
	else
	{
	    /* 
	    ** No, we're going to TestProject mode, so we must honor
	    ** initial state (but remember current state)
	    */
	    if (is_visible) 		/* The object should be visible */
	    {
                if (setup_window_for_test_mode(trg_obj) == -1) goto cret;

	        /* 
	        ** If the object is currently invisible show it.
	        */
	        if (!obj_has_flag(trg_obj, MappedFlag))
	        {
		    if (objxm_tree_map(trg_obj, TRUE) == -1) goto cret;
	        }

	        /* 
	        ** If the object is supposed to be iconic (and visible) 
		** make it iconic, but remember that we have to deiconify
		** it we go back to Build mode
	        */
	        if (obj_is_initially_iconic(trg_obj))
	        {
		    make_win_iconic(trg_obj, TRUE);
                    util_llist_insert_after(ConnP_wins_deiconify, trg_obj);
	        }
	    }
            else /* !is_visible */
            {
		/* 
		** Object is supposed to be invisible.  If it is currently
		** visible and is a real widget, make invisible.
		*/
                if (obj_has_flag(trg_obj, MappedFlag) && objxm_get_widget(trg_obj))
		    objxm_tree_map(trg_obj, FALSE);
            }
	}
    }
    
    set_obj_initial_state(trg_obj);
    
cret:
    return;
}

static void
restore_build_state(
    ABObj	trg_obj
)
{
    Widget	trg_w;

    if ((trg_w = objxm_get_widget(trg_obj)) == NULL)
	return;

    /* Going back to Build mode, we need to ensure all windows
     * are resizable (have resize decorations) again.
     */  
    if (obj_is_window(trg_obj) && !obj_get_resizable(trg_obj))
        ui_win_set_resizable(trg_w, True, obj_has_flag(trg_obj, MappedFlag));
}

static int
setup_window_for_test_mode(
    ABObj	win
)
{
    int		ret_val = -1;
    ABObj	module = obj_get_module(win);

    if (module == NULL)
        goto cret;

    /* Make sure the module is instantiated */
    if (!obj_has_flag(module, InstantiatedFlag))
    {
        if (abobj_instantiate_tree(module, TRUE) == -1)
            goto cret;
    }
    
    /* 
    ** If this module isn't already in the list of modules we've set up,
    ** add connection interpretation and help and add it to the list.
    */
    if (!util_llist_find(ConnP_modules_disable, module))
    {
        AB_TRAVERSAL	trav;
        ABObj		action;

        objxm_instantiate_menus(module);

        util_aset_add(ConnP_modules_disable, module);

        /* Setup connections */
        for (trav_open(&trav, obj_get_project(module), AB_TRAV_ACTIONS);
             (action = trav_next(&trav)) != NULL; )
        {
            AB_ACTION_INFO	*info = &(action->info.action);

            if (info == NULL || info->from == NULL ||
                obj_get_module(info->from) != module)
                continue;
            
            attach_interpretation(action);
        }
        trav_close(&trav);

	/* Enable test-mode help support on this module's children */
	tree_enable_help(module);
    }

    /*
    ** Now, we can setup the window.
    */
    if(obj_is_base_win(win) || obj_is_popup_win(win) || obj_is_file_chooser(win)) 
    {
	/* Windows, Dialogs and FSB's need a window-close callback to
	 * handle "manual" close as if it were a 'hide' action.
	 */
	ui_add_window_close_callback(objxm_get_widget(win),
				     win_closeCB, (XtPointer)win, XmUNMAP);
	util_aset_add(ConnP_win_close, win);


	/* In Build mode, ALL windows are resizable, therefore we need
	 * to remove the resize decorations from any window whose
	 * "User Resize" attribute is "Fixed".
	 */
	if (obj_get_resizable(win) == False)
	{
	    ui_win_set_resizable(objxm_get_widget(win), False, 
				 obj_has_flag(win, MappedFlag));
	    /*
	     * Since we need to unmap/remap the window in order to get
	     * the decorations to change, we need to mark this window
	     * object so that when the iconify_tracker event handler is
	     * called, it doesn't interpret the Unmap event as an
	     * iconify. (this is an unfortunate hack to get around
	     * the mysterious problem of the Unmap event occurring
	     * AFTER we return to BUILD mode).
	     */
	    obj_set_flag(win, DecorChangedFlag);
	}    
    }
    
    ret_val = 0;
cret:
    return(ret_val);
}

static void
disable_modules(
    void	*cl_data
)
{
    ABObj		mod = (ABObj)cl_data;
    ABObj		action;
    AB_TRAVERSAL	trav;

    if (mod == NULL || !obj_is_module(mod))
        goto cret;
    
    /* Remove connections */
    for (trav_open(&trav, obj_get_project(mod), AB_TRAV_ACTIONS);
         (action = trav_next(&trav)) != NULL; )
    {
        AB_ACTION_INFO	*info = &(action->info.action);
        
        if (info == NULL || info->from == NULL ||
            obj_get_module(info->from) != mod)
            continue;
        
        detach_interpretation(action);
    }
    trav_close(&trav);
    
    /* Disable test-mode help support */
    tree_disable_help(mod);

    objxm_destroy_menus(mod);
cret:
    return;
}

static void
deiconify_wins(
    void	*cl_data
)
{
    ABObj	obj = (ABObj)cl_data;

    make_win_iconic(obj, FALSE);
}

static void
sensitize_objects(
    void	*cl_data
)
{
    ABObj	obj = (ABObj)cl_data;

    if (obj != NULL &&
	!tmodeP_obj_has_flags(obj_get_window(obj), TestModeFlagResizedWindow) &&
	objxm_get_widget(obj) != NULL)
    {
	XtSetSensitive(objxm_get_widget(obj), TRUE);
	
	/* apply to the actual XmList also */
	if (obj_is_list(obj))
	    XtSetSensitive(objxm_get_widget(
		               objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ)),
			   TRUE);
    }
}

static void
free_infos(
    void	*cl_data
)
{
    AB_ACTION_INFO	*info = (AB_ACTION_INFO *)cl_data;

    if (info != NULL)
    {
	INTERPRET_HANDLER	handler;
	AB_WHEN			c_when;

	c_when = info->when;
	handler = get_when_detach_handler(c_when);
	if (handler != NULL && info->from != NULL)
	    (*handler)(info, get_when_callback(c_when));

	XtFree((char *)info);
    }
}

static void
reset_layer(
    void	*cl_data
)
{
    ABObj	obj = (ABObj)cl_data;

    if (obj != NULL)
    {
	if (obj_is_layers(obj))
	    abobj_layer_manage_visible(obj);
    }
}

/* manage non-window objects */
static void
manage_objs(
    void	*cl_data
)
{
    ABObj	obj    = (ABObj)cl_data;

    /*
    ** If the window was resized, there is no need to set the
    ** visibility. The widget will be re-instantiated.
    */
    if (obj && !obj_is_window(obj) && objxm_get_widget(obj) &&
	!tmodeP_obj_has_flags(obj_get_window(obj), TestModeFlagResizedWindow))
    {
	ui_set_visible(objxm_get_widget(obj), TRUE);
    }
}

static void
attach_interpretation(
    ABObj	action
)
{
    AB_ACTION_INFO	*info = &(action->info.action);
    INTERPRET_HANDLER	handler;
    AB_WHEN		c_when;
    BOOL		needs_from_refs = FALSE;
    BOOL		needs_to_refs = FALSE;

    c_when = info->when;
    handler = get_when_attach_handler(c_when);
    if (handler == NULL || info->from == NULL)
	return;

    /* if source is a menu item */
    if (obj_is_menu_item(info->from))
	needs_from_refs = TRUE;

    if (info->func_type == AB_FUNC_BUILTIN)
    { /* connection is a builtin one and needs a target */
	/* if target is a menu item */
	if (info->to != NULL && obj_is_menu_item(info->to))
	    needs_to_refs = TRUE;
    }

    if (needs_from_refs == TRUE)
    {
	ABObj	*from_refs = get_menu_item_refs(info->from);

	if (from_refs == NULL)
	    return;

	if (needs_to_refs == TRUE)
	{
	    ABObj	*to_refs = get_menu_item_refs(info->to);

	    if (to_refs == NULL)
	    {
		XtFree((char *)from_refs);
		return;
	    }
	    attach_for_both_refs(from_refs, to_refs, info,
					handler, get_when_callback(c_when));
	    XtFree((char *)to_refs);
	}
	else
	    attach_for_single_refs(TRUE, from_refs, info,
					handler, get_when_callback(c_when));
	XtFree((char *)from_refs);
    }
    else if (needs_to_refs == TRUE)
    {
	ABObj	*to_refs = get_menu_item_refs(info->to);

	if (to_refs == NULL)
	    return;

	attach_for_single_refs(FALSE, to_refs, info,
				    handler, get_when_callback(c_when));
	XtFree((char *)to_refs);
    }
    else
	(*handler)(info, get_when_callback(c_when));
}

static void
attach_for_both_refs(
    ABObj		*from_refs,
    ABObj		*to_refs,
    AB_ACTION_INFO	*info,
    INTERPRET_HANDLER	handler,
    XtCallbackProc	when_callback
)
{
    ABObj	*trav_from;

    for (trav_from = from_refs; *trav_from != NULL; trav_from++)
    {
	ABObj	*trav_to;

	for (trav_to = to_refs; *trav_to != NULL; trav_to++)
	{
	    AB_ACTION_INFO	*new_info;

	    /* create new info structure and copy old one */
	    new_info = (AB_ACTION_INFO *)
				XtMalloc(sizeof(AB_ACTION_INFO));
	    memmove((void *)new_info, (void *)info,
					sizeof(AB_ACTION_INFO));

	    /* assign reference instead of item to from and to */
	    new_info->from = *trav_from;
	    new_info->to   = *trav_to;

	    /* add info to list */
	    util_llist_insert_after(ConnP_menu_item_infos, new_info);
	    /* call the handler on this info record */
	    (*handler)(new_info, when_callback);
	}
    }
}

static void
attach_for_single_refs(
    BOOL		for_from,
    ABObj		*item_refs,
    AB_ACTION_INFO	*info,
    INTERPRET_HANDLER	handler,
    XtCallbackProc	when_callback
)
{
    ABObj	*trav_item;

    for (trav_item = item_refs; *trav_item != NULL; trav_item++)
    {
	AB_ACTION_INFO	*new_info;

	/* create new info structure and copy old one */
	new_info = (AB_ACTION_INFO *)
			    XtMalloc(sizeof(AB_ACTION_INFO));
	memmove((void *)new_info, (void *)info,
				    sizeof(AB_ACTION_INFO));

	/* assign reference instead of item to from or to */
	if (for_from)
	    new_info->from = *trav_item;
	else
	    new_info->to = *trav_item;

	/* add info to list */
	util_llist_insert_after(ConnP_menu_item_infos, new_info);
	/* call the handler on this info record */
	(*handler)(new_info, when_callback);
    }
}

static void
detach_interpretation(
    ABObj	action
)
{
    AB_ACTION_INFO	*info = &(action->info.action);
    INTERPRET_HANDLER	handler;
    AB_WHEN		c_when;

    c_when = info->when;
    handler = get_when_detach_handler(c_when);
    if (handler == NULL || info->from == NULL)
	return;

    (*handler)(info, get_when_callback(c_when));
}

static ACTION_HANDLER
get_action_handler(
    AB_ACTION_INFO	*notify_info
)
{
    ACTION_HANDLER	handler = (ACTION_HANDLER)NULL;

    switch (notify_info->func_type)
    {
      case AB_FUNC_BUILTIN:
	/*
	 * case target is not a window or dialog:
	 * case target is a window or dialog and !show and !hide:
	 *	if window/dialog unmapped, return NULL
	 * case target is a window or dialog and show or hide:
	 *	return handler (xmconfig processing in handlers)
	 *
	 */
	{
	    ABObj		trg = notify_info->to;
	    AB_OBJECT_TYPE	trg_type;

	    if (trg == NULL)
		goto cret;

	    trg_type = obj_get_type(trg);
	    /* Used to just check for BASE_WINDOW and DIALOG */
	    if (obj_is_window(trg))
	    {
		AB_BUILTIN_ACTION std_act = notify_info->func_value.builtin;

		switch (std_act)
		{
		  case AB_STDACT_SHOW:
		  case AB_STDACT_HIDE:
		    handler = get_std_action_handler(
				    notify_info->func_value.builtin);
		    break;
		  default:
		    if (!obj_has_flag(trg, MappedFlag))
			goto cret;
		    else
			handler = get_std_action_handler(
					notify_info->func_value.builtin);
		    break;
		}
	    }
	    else
	    {
		ABObj	win = obj_get_window(trg);

		if (win == NULL)
		    goto cret;
		if (!obj_has_flag(win, MappedFlag))
		    goto cret;
		handler = get_std_action_handler(
				notify_info->func_value.builtin);
	    }
	    break;
	}
      case AB_FUNC_USER_DEF:
	handler = user_def_fn_handler;
	break;
      case AB_FUNC_CODE_FRAG:
	handler = code_frag_handler;
	break;
      case AB_FUNC_ON_ITEM_HELP:
	handler = on_item_help_handler;
	break;
      case AB_FUNC_HELP_VOLUME:
	handler = help_volume_handler;
	break;
    }
cret:
    return(handler);
}


static void
activate_attach_handler(
    AB_ACTION_INFO	*notify_info,
    XtCallbackProc	when_cb
)
{
    ABObj	c_from	= notify_info->from;
    ABObj	c_to	= notify_info->to;
    Widget	attach_w;
    AB_ITEM_TYPE	item_type;

    if (when_cb == NULL)
	return;

    switch(obj_get_type(c_from))
    {
      case AB_TYPE_ITEM:
	item_type = (AB_ITEM_TYPE)obj_get_subtype(c_from);
	switch(item_type)
	{
	  case AB_ITEM_FOR_MENU:
	    if (obj_has_menu(c_from))
	    {
		if (objxm_get_widget(c_from) != NULL)
		    XtAddCallback(objxm_get_widget(c_from),
				XmNcascadingCallback,
				when_cb, (XtPointer)notify_info);
	    }
	    else
	    {
		if (objxm_get_widget(c_from)!= NULL)
		    XtAddCallback(objxm_get_widget(c_from),
				XmNactivateCallback,
				when_cb, (XtPointer)notify_info);
	    }
	    break;

	  case AB_ITEM_FOR_CHOICE:
	    attach_w = objxm_get_widget(c_from);
	    if (attach_w != NULL)
	    {
		if (obj_is_option_menu(
			obj_get_root(obj_get_parent(c_from))))
		    XtAddCallback(attach_w, XmNactivateCallback,
				    when_cb, (XtPointer)notify_info);
		else
		    XtAddCallback(attach_w, XmNvalueChangedCallback,
				    when_cb, (XtPointer)notify_info);
	    }
	    break;
	}
	break;
      case AB_TYPE_BUTTON:
	attach_w = objxm_get_widget(c_from);
	if (attach_w != NULL)
	    XtAddCallback(attach_w, XmNactivateCallback,
			    when_cb, (XtPointer)notify_info);
	break;
      default:
	break;
    }
}

static void
post_menu_attach_handler(
    AB_ACTION_INFO	*notify_info,
    XtCallbackProc	when_cb
)
{
    ABObj	c_from	= notify_info->from;
    ABObj	c_to	= notify_info->to;
    Widget	attach_w;
    AB_ITEM_TYPE	item_type;

    if (when_cb == NULL)
	return;

    switch(obj_get_type(c_from))
    {
      case AB_TYPE_ITEM:
	item_type = (AB_ITEM_TYPE)obj_get_subtype(c_from);
	switch(item_type)
	{
	  case AB_ITEM_FOR_MENU:
	    {
		if (obj_has_menu(c_from))
		{
		    if (objxm_get_widget(c_from) != NULL)
			XtAddCallback(objxm_get_widget(c_from),
				    XmNcascadingCallback,
				    when_cb, (XtPointer)notify_info);
		}
		else
		{
		    if (objxm_get_widget(c_from) != NULL)
			XtAddCallback(objxm_get_widget(c_from),
				    XmNactivateCallback,
				    when_cb, (XtPointer)notify_info);
		}
	    }
	    break;
	}
	break;
      default:
	break;
    }
}

static void
activate_detach_handler(
    AB_ACTION_INFO	*notify_info,
    XtCallbackProc	when_cb
)
{
    ABObj	c_from	= notify_info->from;
    Widget	attach_w;
    AB_ITEM_TYPE	item_type;

    if (when_cb == NULL)
	return;

    switch(obj_get_type(c_from))
    {
      case AB_TYPE_ITEM:
	item_type = (AB_ITEM_TYPE)obj_get_subtype(c_from);
	switch(item_type)
	{
	  case AB_ITEM_FOR_MENU:
	    {
		if (obj_has_menu(c_from))
		{
		    if (objxm_get_widget(c_from) != NULL)
			XtRemoveCallback(objxm_get_widget(c_from),
				    XmNcascadingCallback,
				    when_cb, (XtPointer)notify_info);
		}
		else
		{
		    if (objxm_get_widget(c_from) != NULL)
			XtRemoveCallback(objxm_get_widget(c_from),
				    XmNactivateCallback,
				    when_cb, (XtPointer)notify_info);
		}
	    }
	    break;

	  case AB_ITEM_FOR_CHOICE:
	    attach_w = objxm_get_widget(c_from);
	    if (attach_w != NULL)
	    {
		if (obj_is_option_menu(
			obj_get_root(obj_get_parent(c_from))))
		    XtRemoveCallback(attach_w, XmNactivateCallback,
				    when_cb, (XtPointer)notify_info);
		else
		    XtRemoveCallback(attach_w, XmNvalueChangedCallback,
				    when_cb, (XtPointer)notify_info);
	    }
	    break;
	}
	break;
      case AB_TYPE_BUTTON:
	attach_w = objxm_get_widget(c_from);
	if (attach_w != NULL)
	    XtRemoveCallback(attach_w, XmNactivateCallback,
				when_cb, (XtPointer)notify_info);
	break;
      default:
	break;
    }
}

static void
post_menu_detach_handler(
    AB_ACTION_INFO	*notify_info,
    XtCallbackProc	when_cb
)
{
    ABObj	c_from	= notify_info->from;
    Widget	attach_w;
    AB_ITEM_TYPE	item_type;

    if (when_cb == NULL)
	return;

    switch(obj_get_type(c_from))
    {
      case AB_TYPE_ITEM:
	item_type = (AB_ITEM_TYPE)obj_get_subtype(c_from);
	switch(item_type)
	{
	  case AB_ITEM_FOR_MENU:
	    {
		if (obj_has_menu(c_from))
		{
		    if (objxm_get_widget(c_from) != NULL)
			XtRemoveCallback(objxm_get_widget(c_from),
				    XmNcascadingCallback,
				    when_cb, (XtPointer)notify_info);
		}
		else
		{
		    if (objxm_get_widget(c_from) != NULL)
			XtRemoveCallback(objxm_get_widget(c_from),
				    XmNactivateCallback,
				    when_cb, (XtPointer)notify_info);
		}
	    }
	    break;
	}
	break;
      default:
	break;
    }
}

static void
activate_when_cb(
    Widget	w,
    XtPointer	client_data,
    XtPointer	call_data
)
{
    AB_ACTION_INFO	*notify_info = (AB_ACTION_INFO *)client_data;
    ACTION_HANDLER	handler;

    if (notify_info == NULL)
	return;

    switch (obj_get_type(notify_info->from))
    {
      case AB_TYPE_ITEM:
	if (obj_get_subtype(notify_info->from) == (int)AB_ITEM_FOR_CHOICE)
	{
	    if (!obj_is_option_menu(
		    obj_get_root(obj_get_parent(notify_info->from))))
	    {
		XmToggleButtonCallbackStruct *tog_data =
			(XmToggleButtonCallbackStruct *)call_data;

		if (!tog_data || tog_data->reason != XmCR_VALUE_CHANGED ||
		    !tog_data->set)
		    return;
	    }
	}
	break;
      default:
	break;
    }

    handler = get_action_handler(notify_info);
    if (handler != NULL)
	(*handler)(notify_info);
}

static void
post_menu_when_cb(
    Widget	w,
    XtPointer	client_data,
    XtPointer	call_data
)
{
    AB_ACTION_INFO	*notify_info = (AB_ACTION_INFO *)client_data;
    ACTION_HANDLER	handler;

    if (notify_info == NULL)
	return;

    handler = get_action_handler(notify_info);

    if (handler != NULL)
	(*handler)(notify_info);
}

static void
user_def_fn_handler(
    AB_ACTION_INFO	*action_info
)
{
    if (action_info->func_type != AB_FUNC_USER_DEF)
	return;

    if (action_info->func_value.func_name != (ISTRING)NULL)
    printf("Call user-defined function: \"%s\"\n",
		istr_string(action_info->func_value.func_name));
}

static void
code_frag_handler(
    AB_ACTION_INFO	*action_info
)
{
    if (action_info->func_type != AB_FUNC_CODE_FRAG)
	return;

    printf("Execute user code\n");
}

static void
on_item_help_handler(
    AB_ACTION_INFO      *action_info
)
{
    if (action_info->func_type != AB_FUNC_ON_ITEM_HELP)
        return;
    dtb_do_onitem_help();
}

static void 
help_volume_handler( 
    AB_ACTION_INFO      *action_info 
) 
{ 
    if (action_info->func_type != AB_FUNC_HELP_VOLUME) 
        return;    

    
    dtb_show_help_volume_info(istr_string(action_info->volume_id),
		istr_string(action_info->location)); 
} 

/*
** Handle a 'show-help' connection by dispatching the XmNhelpCallback on the
** target object (if it has one).
*/
static void
show_help_action_handler(
    AB_ACTION_INFO	*action_info
)
{
    ABObj		c_to	= action_info->to;
    ABObj		root_obj, help_obj;
    
    if (c_to == NULL || objxm_get_widget(c_to) == NULL)
	return;

    if(obj_has_help_data(c_to) == True) {
	help_obj = objxm_comp_get_subobj(c_to,AB_CFG_HELP_OBJ);
	if(help_obj != NULL) {
		XtCallCallbacks(objxm_get_widget(help_obj),XmNhelpCallback,
			(XtPointer)NULL);
	}
    }
}

static void
show_action_handler(
    AB_ACTION_INFO	*action_info
)
{
    ABObj		c_to	= action_info->to;
    ABObj		layer;
    
    /* If target object doesn't exist, just return */
    if (c_to == NULL) return;

    if(obj_is_window(c_to))
    {
	if (setup_window_for_test_mode(c_to) == -1)
	    goto cret;
	if (obj_has_flag(c_to, MappedFlag))
	    goto cret;
	tree_set_obj_initial_state(c_to);
	if (objxm_tree_map(c_to, TRUE) == -1)
	    goto cret;
    }
    else {
	/* Don't do anything with an unrealized non-window object */
	if(objxm_get_widget(c_to) && !XtIsManaged(objxm_get_widget(c_to)))
	{
	    if (!obj_is_menu_item(c_to)) {
		/* Layers */
		if (obj_is_layers(obj_get_parent(c_to)))
		{
		    layer = obj_get_parent(c_to);
		    /*
		     * If there is only one pane, this one must be it.
		     * Thus, do nothing; else manage it.
		     */
		    if (obj_get_num_salient_children(layer) > 1)
		    {
			ui_set_visible(objxm_get_widget(c_to), TRUE);

			/* add layer to clean up when we go back to Build Mode */
			util_aset_add(ConnP_layer, layer);
		    }
		}
		else
		{
		    ui_set_visible(objxm_get_widget(c_to), TRUE);
		}
	    }
	}
    }
cret:
    return;
}


static void
hide_action_handler(
    AB_ACTION_INFO	*action_info
)
{
    ABObj		c_to	= action_info->to;
    ABObj		layer;
    
    if (!c_to || !objxm_get_widget(c_to))
	return;

    if(obj_is_window(c_to))
    {
        ABObj	module = obj_get_module(c_to);

        if (!module)
	    goto cret;
        if (obj_has_flag(c_to, MappedFlag))
        {
            if (objxm_tree_map(c_to, FALSE) == -1)
		goto cret;
	}
    }
    else
    {
	if (XtIsManaged(objxm_get_widget(c_to)))
	{
	    if (!obj_is_menu_item(c_to)) {
		/* Layers */
		if (obj_is_layers(obj_get_parent(c_to)))
		{
		    layer = obj_get_parent(c_to);
		    /*
		     * If there is only one pane, this one must be it.
		     * Thus, do nothing; else unmanage it.
		     */
		    if (obj_get_num_salient_children(layer) > 1)
		    {
			ui_set_visible(objxm_get_widget(c_to), FALSE);

			/* add layer to clean up when we go back to Build Mode? */
			util_aset_add(ConnP_layer, layer);
		    }
		}
		else
		    ui_set_visible(objxm_get_widget(c_to), FALSE);

		/* Show this object when we go back to Build mode */
		util_aset_add(ConnP_manage, c_to);
	    }
	}
    }
cret:
    return;
}

static void
enable_action_handler(
    AB_ACTION_INFO	*action_info
)
{
    ABObj	c_to	= action_info->to;
    
    if (c_to == NULL || objxm_get_widget(c_to) == NULL)
	return;

    ui_set_active(objxm_get_widget(c_to), TRUE);
	
    if (!obj_is_menu_item(c_to))
	util_aset_remove(ConnP_senses, c_to);
}

static void
disable_action_handler(
    AB_ACTION_INFO	*action_info
)
{
    ABObj	c_to	= action_info->to;
    
    if (c_to == NULL || objxm_get_widget(c_to) == NULL)
	return;

    ui_set_active(objxm_get_widget(c_to), FALSE);
	
    if (!obj_is_menu_item(c_to))
	util_aset_add(ConnP_senses, c_to);
}

static void
set_text_action_handler(
    AB_ACTION_INFO	*action_info
)
{
    ABObj	c_to	= action_info->to;
    
    if (c_to == NULL)
	return;

    if (obj_is_text(c_to))
    {
	ABObj   subObj   = NULL;
	ISTRING	txt_istr = NULL;
	STRING	txt_str  = NULL;

	/* obtain the actual text object */
	subObj = objxm_comp_get_subobj(c_to, AB_CFG_OBJECT_OBJ);
	if (subObj == NULL || objxm_get_widget(subObj) == NULL)
	    return;

	/* obtain the string */
	txt_istr = action_info->arg_value.sval;
	if (txt_istr != (ISTRING)NULL)
	    txt_str = istr_string(txt_istr);
	else
	    txt_str = Util_empty_string;
	    
	/* set the string on the widget */
	ui_field_set_string(objxm_get_widget(subObj), txt_str);

	/* mark this obj's window as dirty */
	tmodeP_obj_set_flags(obj_get_window(c_to), TestModeFlagDirtyWindow);
    }
}

static void
set_value_action_handler(
    AB_ACTION_INFO	*action_info
)
{
    ABObj	c_to	= action_info->to;
    
    if (c_to == NULL)
	return;

    if(obj_is_spin_box(c_to) || obj_is_scale(c_to))
    {
	ABObj   subObj  = NULL;
	int     val_int = 0;
	
	/* get the actual object */
	subObj = objxm_comp_get_subobj(c_to, AB_CFG_OBJECT_OBJ);
	if (subObj == NULL || objxm_get_widget(subObj) == NULL)
	    return;
	
	/* get the value */
	val_int = action_info->arg_value.ival;
    
	/* set the value */
	if (obj_is_spin_box(c_to))
	    XtVaSetValues(objxm_get_widget(subObj), DtNposition, val_int, NULL);
	else
	    XtVaSetValues(objxm_get_widget(subObj), XmNvalue, val_int, NULL);
	
	/* mark this obj's window as dirty */
	tmodeP_obj_set_flags(obj_get_window(c_to), TestModeFlagDirtyWindow);
    }
}

static void
set_label_action_handler(
    AB_ACTION_INFO	*action_info
)
{
    ABObj	c_to	 = action_info->to;
    ISTRING	lab_istr = NULL;
    STRING	lab_str  = NULL;
    
    if (c_to == NULL)
	return;

    /* obtain the label */
    lab_istr = action_info->arg_value.sval;
    if (lab_istr != (ISTRING)NULL)
	lab_str = istr_string(lab_istr);
    else
	lab_str = Util_empty_string;
	
    /* set the label */
    ui_obj_set_label(c_to, lab_str);

    /* mark this obj's window as dirty */
    tmodeP_obj_set_flags(obj_get_window(c_to), TestModeFlagDirtyWindow);
}

static ABObj	*
get_menu_item_refs(
    ABObj	menu_item
)
{
    ABObj		parent_menu;
    ABObj		this_module;
    ABObj		child;
    AB_TRAVERSAL	trav;
    int			num_refs;
    int			list_size;
    ABObj		*item_ref_list;
    AB_TRAVERSAL	item_trav;
    ABObj		item_child;
    int			i;
    int			item_index;

    if (menu_item == NULL || !obj_is_menu_item(menu_item))
	return(NULL);

    /* Get parent menu of item */
    parent_menu = obj_get_parent(menu_item);

    if (parent_menu == NULL || obj_get_type(parent_menu) != AB_TYPE_MENU)
	return(NULL);

    if ((this_module = obj_get_module(parent_menu)) == NULL)
	return(NULL);

    item_index = -1;
    for (i = 0, trav_open(&trav, parent_menu, AB_TRAV_CHILDREN);
	 (item_child = trav_next(&trav)) != NULL; i++)
    {
	if (obj_get_type(item_child) == AB_TYPE_ITEM &&
	    util_streq(obj_get_name(item_child), obj_get_name(menu_item)))
	{
	    item_index = i;
	    break;
	}
    }
    trav_close(&trav);

    if (item_index == -1)	/* should never happen */
	return(NULL);

    num_refs = 0; list_size = 1;
    item_ref_list = (ABObj *) XtMalloc(sizeof(ABObj) * (list_size + 1));

    /* Traverse module for all menu-references pointing to this menu */
    for (trav_open(&trav, this_module, AB_TRAV_ALL);
	 (child = trav_next(&trav)) != NULL; )
    {
	if (obj_is_menu_ref(child) && obj_get_actual_obj(child) == parent_menu)
	{
	    ABObj		item_ref;

	    /* Find our item by index in each tree rooted at a menu-reference */

	    item_child = obj_get_item(child, item_index);
	    if (item_child != (ABObj)NULL)
	    {
		if (num_refs == list_size)
		{
		    list_size += 2;
		    item_ref_list = (ABObj *) XtRealloc(
					(char *)item_ref_list,
					sizeof(ABObj) * (list_size + 1));
		}
		/* Add item to return list */
		item_ref_list[num_refs++] = item_child;
	    }
	}
    }
    trav_close(&trav);

    if (num_refs > 0)
	item_ref_list[num_refs] = NULL;	/* NULL-terminate the array */
    else
    {
	XtFree((char *)item_ref_list);
	item_ref_list = NULL;
    }
    return(item_ref_list);
}

/*
 * initialization for test mode.
 */
extern void
conn_test_mode_initialize(
    ABObj	project
)
{
    AB_TRAVERSAL	trav;
    ABObj		obj;

    ConnP_wins_deiconify  = util_llist_create();
    
    ConnP_modules_disable = util_aset_create();
    ConnP_win_close       = util_aset_create();
    ConnP_layer           = util_aset_create();
}

/*
 * cleanup for test mode
 */
extern void
conn_test_mode_cleanup(
    ABObj	project
)
{
    util_aset_iterate(ConnP_modules_disable, disable_modules);
    util_aset_iterate(ConnP_win_close, remove_win_close);
    util_aset_iterate(ConnP_wins_deiconify, deiconify_wins);
    util_aset_iterate(ConnP_layer, reset_layer);

    util_aset_destroy(ConnP_modules_disable);
    util_aset_destroy(ConnP_win_close);
    util_aset_destroy(ConnP_wins_deiconify);
    util_aset_destroy(ConnP_layer);
}

static void
make_win_iconic(
    ABObj	win,
    BOOL	to_iconify
)
{
    XEvent	event;
    Widget	top_level;
    Window	root;

    if (!obj_is_window(win))
	goto cret;

    top_level = objxm_get_widget(win);
    root = XRootWindowOfScreen(XtScreen(top_level));

    event.xclient.type = ClientMessage;
    event.xclient.display = XtDisplay(top_level);
    event.xclient.window = XtWindow(top_level);
    event.xclient.message_type = XInternAtom (XtDisplay(top_level),
						"WM_CHANGE_STATE", False);
    event.xclient.format = 32;
    event.xclient.data.l[0] = to_iconify?IconicState:NormalState;

    XSendEvent(XtDisplay(top_level), root, False,
		SubstructureRedirectMask|SubstructureNotifyMask,
		&event);

    XFlush(XtDisplay(top_level));

cret:
    return;
}

/* Enable test-mode help support on an object and its children */
static void
tree_enable_help(ABObj obj)
{
    ABObj		child;
    AB_TRAVERSAL	trav;

    if (!obj_is_virtual(obj)) help_test_enable(obj);

    for (trav_open(&trav, obj, AB_TRAV_SALIENT_CHILDREN);
	 (child = trav_next(&trav)) != NULL; ) {
	    /* Recursively descend through all children */
	    tree_enable_help(child);
    }
    trav_close(&trav);
}

/* Disable test-mode help support on an object and its children */
static void
tree_disable_help(ABObj obj)
{
    ABObj		child;
    AB_TRAVERSAL	trav;

    if (!obj_is_virtual(obj)) help_test_disable(obj);

    for (trav_open(&trav, obj, AB_TRAV_SALIENT_CHILDREN);
	 (child = trav_next(&trav)) != NULL; ) {
	    /* Recursively descend through all children */
	    tree_disable_help(child);
    }
    trav_close(&trav);
}

/* 
** This function is called when the user manually closes a dialog or
** file selection box while in test mode.  It handles the necessary
** bookkeeping as if the window had been hidden via a connection.
*/
static void     
win_closeCB(
	Widget      w,
	XtPointer   client_data,
	XtPointer   call_data
)
{
    ABObj	obj = (ABObj)client_data;
    ABObj	project = proj_get_project();

    /*
     * If we are in Test Project mode and the user has just Closed
     * the Root Window of the project, then we want to simulate an
     * application close (quit) and take down all windows.
     */
    if (AB_builder_mode == MODE_TEST_PROJ &&
	obj_get_root_window(project) == obj)
    {
        ABObj           win;
        AB_TRAVERSAL    trav;

        for (trav_open(&trav, project, AB_TRAV_WINDOWS);
             (win = trav_next(&trav)) != NULL; )
	{
	    if (obj_has_flag(win, MappedFlag))
	    	objxm_tree_map(win, False);
	}
    }
    obj_clear_flag(obj,MappedFlag);

    /*
    ** We're going to let the window manager 'hide' this object.
    */
}

/* 
** Routine used to remove the window close callback on those dialogs 
** and file choosers that had one added (and which therefore appear on the
** ConnP_win_close list). 
*/
static void
remove_win_close(
    void	*cl_data
)
{
    ABObj		obj = (ABObj)cl_data;

    ui_remove_window_close_callback(objxm_get_widget(obj),
			win_closeCB,(XtPointer)obj);
}

