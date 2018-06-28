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
 *      $XConsortium: pal_panedwin.c /main/3 1995/11/06 17:41:05 rswiston $
 *
 *      @(#)pal_panedwin.c	1.10 01 May 1995
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


/*
 * panedwin.c - functions for Paned Window prop sheet.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <sys/param.h>		/* MAXPATHLEN */
#include <Xm/Xm.h>
#include <Xm/List.h>
#include <ab/util_types.h>
#include <ab_private/ab.h>
#include <ab_private/obj.h>
#include <ab_private/pal.h>
#include <ab_private/prop.h>
#include <ab_private/proj.h>
#include <ab_private/brws.h>
#include <ab_private/abobj.h>
#include <ab_private/abobj_set.h>
#include <ab_private/abobj_list.h>
#include <ab_private/ui_util.h>
#include <ab_private/obj_notify.h>
#include <ab_private/objxm.h>
#include "panedwin_ed_ui.h"

typedef struct  PROP_PANEDWIN_SETTINGS
{
    Widget                      prop_sheet;
    PropFieldSettingRec         name;
    PropCheckboxSettingRec      init_state;
    Widget			panelist;
    Widget      		width;
    Widget      		height;
    PropGeometrySettingRec      min_max_geo;
    ABObj                       current_pw_obj;
    ABObj                       current_pane_obj;
} PropPanedWinSettingsRec, *PropPanedWinSettings;


/*************************************************************************
**                                                                      **
**       Private Function Declarations                                  **
**                                                                      **
**************************************************************************/

static int 	panedwin_initialize(
		    ABObj    obj
		);
static Widget   panedwin_prop_init(
                    Widget  	parent,
                    AB_PROP_TYPE type
                );
static int	panedwin_prop_load(
		    ABObj	 obj,
                    AB_PROP_TYPE type,
		    unsigned long loadkey
                );
static int 	panedwin_load_panes(
		    ABObj       obj,
		    AB_PROP_TYPE type
		);
static int	panedwin_prop_clear(
                    AB_PROP_TYPE type
		);
static int 	panedwin_prop_clear_geom(
                    AB_PROP_TYPE type
		);
static int	panedwin_prop_activate(
                    AB_PROP_TYPE type,
		    BOOL 	 active
		);
static int	panedwin_prop_apply(
                    AB_PROP_TYPE type
		);
static BOOL     panedwin_prop_pending(
                    AB_PROP_TYPE type
		);
static void 	panedwinEdP_init();
static BOOL 	pw_child_test_func(
		    ABObj test_obj
		);
static BOOL	verify_props(
                    AB_PROP_TYPE type
		);
static void	turnoff_changebars(
                    AB_PROP_TYPE type
		);

static int 	position_compare(
		    const void *leftEntry,
		    const void *rightEntry
		);

static int 	east_compare(
		    const void *leftEntry,
		    const void *rightEntry
		);

static int      west_compare(
                    const void *leftEntry,
                    const void *rightEntry
                );

/*
 * Callbacks
 */
static int 	pw_obj_reparentedOCB(
		    ObjEvReparentInfo	info
		);
static int      pw_obj_destroyedOCB(
                    ObjEvDestroyInfo    info
                );
static int      pw_obj_renamedOCB(
                    ObjEvAttChangeInfo	info
                );
static void 	pw_panelist_selectCB(
		    Widget      	widget,
		    XtPointer   	client_data,
		    XmListCallbackStruct *listdata
		);

/*************************************************************************
**                                                                      **
**       Data 		                                                **
**                                                                      **
**************************************************************************/

PalItemInfo panedwin_palitem_rec = {
    /* type             */  AB_TYPE_CONTAINER,
    /* name             */  "Paned Window",
    /* animation pixmaps*/  NULL,
    /* number of pixmaps*/  0,
    /* rev_prop_frame   */  NULL,
    /* fix_prop_dialog  */  NULL,
    /* initialize       */  panedwin_initialize,
    /* is_a_test        */  obj_is_paned_win,
    /* prop_initialize  */  panedwin_prop_init,
    /* prop_active      */  panedwin_prop_activate,
    /* prop_clear       */  panedwin_prop_clear,
    /* prop_load        */  panedwin_prop_load,
    /* prop_apply       */  panedwin_prop_apply,
    /* prop_pending     */  panedwin_prop_pending
};

PalItemInfo *ab_panedwin_palitem = &panedwin_palitem_rec;
PropPanedWinSettingsRec    prop_pw_settings_rec[AB_PROP_TYPE_NUM_VALUES];

/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/
static int
panedwin_initialize(
    ABObj    obj
)
{
    if (!obj_is_paned_win(obj))
	return ERROR;

    obj_set_unique_name(obj, "panedwin"); 
    obj_set_is_initially_active(obj, True);

    return OK;
}

static Widget
panedwin_prop_init(
    Widget		parent,
    AB_PROP_TYPE	type
)
{
    DtbPanedwinEdDialogInfoRec	rev_panedwin_prop_dialog;
    DtbPanedwinEdDialogInfo	cgen = &dtb_panedwin_ed_dialog; /* Codegen structure */
    DtbRevolvPropDialogInfo     rpd = &(dtb_revolv_prop_dialog);
    PropPanedWinSettingsRec   	*pws = &(prop_pw_settings_rec[type]);
    Widget                      item[2];
    int                         item_val[2];
    int				n = 0;

    if (type == AB_PROP_REVOLVING)
    {
        /* Cloning Trick:
         * Only the Attributes ControlPanel needs to be created within
         * the existing Revolving Prop dialog, so fill out all other
         * fields with the Revolving Prop dialog equivelents, so the
         * dtb initialize proc will skip those non-NULL fields...
         */
        dtbPanedwinEdDialogInfo_clear(&rev_panedwin_prop_dialog);
 
        cgen = &(rev_panedwin_prop_dialog);
        cgen->dialog = rpd->prop_dialog;
        cgen->dialog_shellform = rpd->prop_dialog_shellform;
        cgen->dialog_panedwin = rpd->prop_dialog_panedwin;
        cgen->dialog_form = rpd->prop_dialog_form;
        cgen->objlist_panel = rpd->objlist_panel;
        cgen->objlist_label = rpd->objlist_label2;
        cgen->objlist_scrolledwin = rpd->objlist_scrolledwin;
        cgen->objlist = rpd->objlist;
        cgen->attrs_ctrlpanel_frame = rpd->attrs_ctrlpanel_frame;
        cgen->activate_panel = rpd->activate_panel;
        cgen->apply_button = rpd->apply_button;
        cgen->ok_button = rpd->ok_button;
        cgen->cancel_button = rpd->cancel_button;
        cgen->reset_button = rpd->reset_button;
        cgen->help_button = rpd->help_button;
    }
    else /* AB_PROP_FIXED */
        cgen = &dtb_panedwin_ed_dialog;

    if (dtb_panedwin_ed_dialog_initialize(cgen, parent) == 0)
    {
	pws->prop_sheet = cgen->attrs_ctrlpanel;
	pws->panelist = cgen->panelist;
        pws->current_pw_obj = NULL;
        pws->current_pane_obj = NULL;
   	pws->width = cgen->width_value;
   	pws->height = cgen->height_value;

        /* Add Update, Rename, and Destroy object callbacks */
        panedwinEdP_init();

        /* Add Callback to load pane children values when a pane 
	 * is selected from the "Panes" list. 
	 */
        XtAddCallback(cgen->panelist,
                XmNbrowseSelectionCallback,
                (XtCallbackProc)pw_panelist_selectCB, (XtPointer)type);

        if (type == AB_PROP_REVOLVING)
                XtVaSetValues(parent,
                        XmNuserData, pws->current_pw_obj,
                        NULL);

        /* Dialog/Object List */
        if (type == AB_PROP_FIXED)
        {
            prop_fixed_dialog_init(ab_panedwin_palitem,
                        cgen->dialog_shellform, cgen->objlist);
            prop_activate_panel_init(type, ab_panedwin_palitem,
                        cgen->ok_button, cgen->apply_button,
                        cgen->reset_button, cgen->cancel_button,
                        cgen->help_button);
        }

        /* Alternate Editor Buttons */
        /* Alternate Editor Buttons */
        prop_editors_panel_init(type, ab_panedwin_palitem,
            cgen->attach_button, cgen->conn_button, cgen->helptxt_button);
 
        /*
         * Prop Sheet Settings....
         */

	/* Name Field */
	prop_field_init(&(pws->name), cgen->name_field_label,
			    cgen->name_field, cgen->name_cb);

	/* Initial State */
        n = 0;
        item[n] = cgen->init_state_cbox_items.Visible_item;
        item_val[n] = AB_STATE_VISIBLE; n++;
        item[n] = cgen->init_state_cbox_items.Active_item;
        item_val[n] = AB_STATE_ACTIVE; n++;
        prop_checkbox_init(&(pws->init_state),cgen->init_state_cbox_label,
                cgen->init_state_cbox, n, item, item_val,
                cgen->init_state_cb);

        /* Pane Minimum/Maximum Setting */
        prop_geomfield_init(&(pws->min_max_geo), cgen->pane_height_lbl, 
		NULL, NULL, NULL, NULL,
		cgen->min_height_field_label, cgen->min_height_field,
		cgen->max_height_field_label, cgen->max_height_field,
		cgen->height_cb); 

	prop_changebars_cleared(pws->prop_sheet);

	return (cgen->dialog_shellform);
    }
    else
	return NULL;
}  

static int
panedwin_prop_activate(
    AB_PROP_TYPE type,
    BOOL	 active
)
{
    ui_set_active(prop_pw_settings_rec[type].prop_sheet, active);
    return OK;
}


static int
panedwin_prop_clear(
    AB_PROP_TYPE type
)
{
    PropPanedWinSettingsRec   *pws = &(prop_pw_settings_rec[type]);

    /* Clear Name Field */
    prop_field_set_value(&(pws->name), "", False);

    /* Clear Initial State */
    prop_checkbox_set_value(&(pws->init_state), AB_STATE_VISIBLE, 
				True, False);
    prop_checkbox_set_value(&(pws->init_state), AB_STATE_ACTIVE, 
				True, False);

    /* Clear pane information */
    panedwin_prop_clear_geom(type);
 
    pws->current_pw_obj = NULL;
    pws->current_pane_obj = NULL;

    turnoff_changebars(type);

    return OK;
}

static int
panedwin_prop_clear_geom(
    AB_PROP_TYPE type
)
{
    PropPanedWinSettingsRec   *pws = &(prop_pw_settings_rec[type]);

    /* Clear Pane Geometry Setting */
    XtVaSetValues(pws->width,
                XtVaTypedArg, XmNlabelString, XtRString,
                    "   0", strlen("   0")+1,
                NULL);
    XtVaSetValues(pws->height,
                XtVaTypedArg, XmNlabelString, XtRString,
                    "   0", strlen("   0")+1,
                NULL);

    /* Clear Pane Minimum/Maximum Setting */
    prop_geomfield_clear(&(pws->min_max_geo), GEOM_WIDTH);
    prop_geomfield_clear(&(pws->min_max_geo), GEOM_HEIGHT);

    return OK;
}

static int
panedwin_prop_load(
    ABObj        obj,
    AB_PROP_TYPE type,
    unsigned long loadkey
)
{
    PropPanedWinSettingsRec     *pws = &(prop_pw_settings_rec[type]);
    BOOL			load_all = (loadkey & LoadAll);

    if (obj == NULL)
    {
        if (pws->current_pw_obj != NULL)
            obj = pws->current_pw_obj;
        else
            return ERROR;
    }
    else if (!obj_is_paned_win(obj))
        return ERROR;
    else
        pws->current_pw_obj = obj;


    /* Load  Name of paned window object */
    if (load_all || loadkey & LoadName)
    	prop_field_set_value(&(pws->name), obj_get_name(obj), False);

    if (load_all)
    {
	/* Load Initial State */
	prop_checkbox_set_value(&(pws->init_state), AB_STATE_VISIBLE,
				obj_is_initially_visible(obj), False);
	prop_checkbox_set_value(&(pws->init_state), AB_STATE_ACTIVE,
				obj_is_initially_active(obj), False);

	/* Load children of the paned window */
	panedwin_load_panes(obj, type);

    	turnoff_changebars(type);
    }

    return OK;
}

static int
panedwin_load_panes(
    ABObj       obj,
    AB_PROP_TYPE type
)
{
    PropPanedWinSettingsRec     *pws = &(prop_pw_settings_rec[type]);

    if (obj == NULL || !obj_is_paned_win(obj))
	return ERROR;

    /* Clear out the panes list first */
    XmListDeleteAllItems(pws->panelist);
         
    /* Populate the panes list in the prop sheet */
    abobj_list_update(pws->panelist, obj, pw_child_test_func);      

    return OK;
}

static int
panedwin_pane_prop_load(
    ABObj    	obj,
    AB_PROP_TYPE type
)
{
    PropPanedWinSettingsRec	*pws = &(prop_pw_settings_rec[type]);
    char			width[MAXPATHLEN], 
				height[MAXPATHLEN];
    *width = 0;
    *height = 0;

    if (obj == NULL)
    {
        if (pws->current_pane_obj != NULL)
            obj = pws->current_pane_obj;
        else
            return ERROR;
    }
    else if (!obj_is_pane(obj) && !obj_is_layers(obj))
        return ERROR;
    else
        pws->current_pane_obj = obj;

    /* Pane Geometry Setting */
    sprintf(width, "%d", abobj_get_actual_width(obj));
    sprintf(height,"%d", abobj_get_actual_height(obj));
    XtVaSetValues(pws->width, 
		XtVaTypedArg, XmNlabelString, XtRString,
		    width, strlen(width)+1,
		NULL);
    XtVaSetValues(pws->height, 
		XtVaTypedArg, XmNlabelString, XtRString,
		    height, strlen(height)+1,
		NULL);

    /* Pane Minimum/Maximum Setting */
    prop_geomfield_set_value(&(pws->min_max_geo), GEOM_WIDTH, 
			obj_get_pane_min(obj),False);
    prop_geomfield_set_value(&(pws->min_max_geo), GEOM_HEIGHT, 
			obj_get_pane_max(obj),False);

    turnoff_changebars(type);

    return OK;
}

/* This routine is called when the "Apply" button is
 * pressed.  It sets all the values of attributes that
 * have changed to their new values.
 */
int
panedwin_prop_apply(
    AB_PROP_TYPE   type
)
{
    PropPanedWinSettingsRec	*pws = &(prop_pw_settings_rec[type]);
    STRING			value;
    int				geom;

    if (!verify_props(type))
        return ERROR;

    if (prop_changed(pws->name.changebar))
    {
        value = prop_field_get_value(&(pws->name));   
        abobj_set_name(pws->current_pw_obj, value);
        util_free(value);
    }
    if (prop_changed(pws->init_state.changebar))
    {
        abobj_set_visible(pws->current_pw_obj,
                prop_checkbox_get_value(&(pws->init_state), AB_STATE_VISIBLE));
        abobj_set_active(pws->current_pw_obj,
                prop_checkbox_get_value(&(pws->init_state), AB_STATE_ACTIVE));
    }
    if (prop_changed(pws->min_max_geo.changebar))
    {
	geom = prop_geomfield_get_value(&(pws->min_max_geo), GEOM_WIDTH);
	abobj_set_pane_min(pws->current_pane_obj, geom);
	geom = prop_geomfield_get_value(&(pws->min_max_geo), GEOM_HEIGHT);
	abobj_set_pane_max(pws->current_pane_obj, geom);
    }

    if (pws->current_pw_obj != NULL)
	abobj_instantiate_changes(pws->current_pw_obj);

    if (pws->current_pane_obj != NULL)
	abobj_instantiate_changes(pws->current_pane_obj);

    turnoff_changebars(type);

    return OK;
}

static BOOL
panedwin_prop_pending(
    AB_PROP_TYPE type
)
{
    return(prop_changebars_pending(prop_pw_settings_rec[type].prop_sheet));
}

static BOOL
verify_props(
    AB_PROP_TYPE type
)
{
    PropPanedWinSettingsRec   *pws = &(prop_pw_settings_rec[type]);

    if (prop_changed(pws->name.changebar) && 
	!prop_name_ok(pws->current_pw_obj, pws->name.field))
        return False;

    if (prop_changed(pws->min_max_geo.changebar) &&
        (!prop_number_ok(pws->min_max_geo.w_field, "Pane Height Min Field", 1, SHRT_MAX) ||
         !prop_number_ok(pws->min_max_geo.h_field, "Pane Height Max Field", 1, SHRT_MAX)))
        return False; 

    return True;
}

static void
turnoff_changebars(
    AB_PROP_TYPE type
)
{
    PropPanedWinSettingsRec   *pws = &(prop_pw_settings_rec[type]);

    prop_set_changebar(pws->name.changebar,    		PROP_CB_OFF);
    prop_set_changebar(pws->init_state.changebar,	PROP_CB_OFF);
    prop_set_changebar(pws->min_max_geo.changebar,  	PROP_CB_OFF);

    prop_changebars_cleared(pws->prop_sheet);
}


/* This routine is called when an item in the "Panes" list is
 * selected.  It takes care of loading in the pane child width/
 * height and pane min/max values.
 */
static void
pw_panelist_selectCB(
    Widget      widget,
    XtPointer   client_data,
    XmListCallbackStruct *listdata
)
{
    ABObj               module = NULL;
    ABObj               selected_obj = NULL;
    STRING              name = NULL;
    int			ret = 0;
    AB_PROP_TYPE	type;
    PropPanedWinSettingsRec   *pws;

    type = (AB_PROP_TYPE) client_data;
    pws = &(prop_pw_settings_rec[type]);
    name = objxm_xmstr_to_str(listdata->item);
    if (name)
    {
	ret = abobj_moduled_name_extract(name, &module, &selected_obj);
	pws->current_pane_obj = selected_obj;
	panedwin_pane_prop_load(selected_obj, type);
	util_free(name);
    }
}

/* Test whether the object is a module and if so test
 * if it is mapped (showing).
 */
static BOOL
module_test_func(
    ABObj test_obj
)
{
    if (!obj_is_module(test_obj))
        return(False);

    if (obj_has_flag(test_obj, MappedFlag))
    {
        return(True);
    }
    else
        return(False);
}

/*
 * Test whether an object should be loaded into the
 * list of panes in the Paned Window Editor.
 */
static BOOL
pw_child_test_func(
    ABObj test_obj
)
{
    ABObj       module = NULL;
    ABObj	parent = NULL;

    if ((module = obj_get_module(test_obj)) == NULL)
        return(False);

    parent = obj_get_parent(test_obj);
    if ( (obj_is_pane(test_obj) || obj_is_layers(test_obj))
	&& obj_is_paned_win(parent) && obj_has_flag(module, MappedFlag))
    {
        return(True);
    }
    else
        return(False);
}

/*
 * obj-callback: object is being destroyed - remove from Paned
 *		 Window Editor object list.
 */
static int
pw_obj_destroyedOCB(
    ObjEvDestroyInfo    info
)
{
    ABObj	parent = NULL;
    int         ret = 0, i;
    PropPanedWinSettingsRec   *pws;
                     
    if ( !obj_is_pane(info->obj) && !obj_is_layers(info->obj) )
            return 0;
 
    parent = obj_get_parent(info->obj);
    if ( ((parent = obj_get_parent(info->obj)) != NULL) &&
	obj_is_paned_win(parent))
    {
	for (i = 0; i < AB_PROP_TYPE_NUM_VALUES; ++i)
	{
	    pws = &(prop_pw_settings_rec[i]);
		
	    if (pws->current_pane_obj == info->obj)
	    {
		panedwin_prop_clear_geom((AB_PROP_TYPE) i);
		pws->current_pane_obj = NULL;
	    }
	    ret = abobj_list_obj_destroyed(pws->panelist, info->obj, 
			pw_child_test_func);
	}
    }
    return (ret);
}

static int
pw_obj_renamedOCB(
    ObjEvAttChangeInfo    info
)
{
    ABObj       parent = NULL;
    STRING	mod_name = NULL;
    int         ret = 0, i;
    PropPanedWinSettingsRec   *pws;

    if (!obj_is_module(info->obj) && !obj_is_pane(info->obj) 
	&& !obj_is_layers(info->obj))
	return 0;

    if (obj_is_module(info->obj))
    {
	mod_name = obj_get_name(info->obj);
	if (mod_name == NULL)
	    return -1;
     
	if (info->old_name != NULL)
	{
	    /* Change the module prefix in the panes list */
	    for (i = 0; i < AB_PROP_TYPE_NUM_VALUES; ++i)
	    {
		pws = &(prop_pw_settings_rec[i]);
		abobj_list_obj_renamed(pws->panelist, info->obj, 
		istr_string(info->old_name), module_test_func);
	    }
	}
    }
    else	/* obj is a pane OR a layer */
    {
	if (info->old_name == NULL)
	{
	    /* This is a new pane dropped on an existing
	     * paned window object OR this is a new layer
	     * being created.  To make sure, check the 
	     * obj's parent.
	     */  
	    parent = obj_get_parent(info->obj);
	    if ( (parent != NULL) && obj_is_paned_win(parent) )
	    {
		for (i = 0; i < AB_PROP_TYPE_NUM_VALUES; ++i)
		{
		    pws = &(prop_pw_settings_rec[i]);
		    if (parent == pws->current_pw_obj)
		    {
			ret = abobj_list_update(pws->panelist, parent,
				pw_child_test_func);
		    }
		}
	    }
	}
	/* The pane's name has changed */
	else
	    for (i = 0; i < AB_PROP_TYPE_NUM_VALUES; ++i)
	    {
		pws = &(prop_pw_settings_rec[i]);
		ret = abobj_list_obj_renamed(pws->panelist, info->obj,
		istr_string(info->old_name), pw_child_test_func);
	    }
    }
    return (ret);
}

/* This callback gets called when a pane child is being
 * parented to a different object, as when "Unmake Paned
 * Window" or "Make Paned Window" is chosen from the floating 
 * menu.
 */
static int
pw_obj_reparentedOCB(
    ObjEvReparentInfo     info
)
{
    ABObj	parent = NULL;
    int		ret = 0, i;
    PropPanedWinSettingsRec   *pws;

    if (!obj_is_pane(info->obj) && !obj_is_layers(info->obj))
	return 0;

    /* If the pane name is NULL, then that means it is a 
     * new pane, one which was dragged from the palette 
     * onto an existing paned window. In that case, let 
     * the pw_obj_renamedOCB callback handle it (i.e. add 
     * it to the pane list).
     */
    parent = obj_get_parent(info->obj);
    if (obj_get_name(info->obj) != NULL)
    {
	/* Either a new paned window obj was created (via
	 * the "Make Paned Window" popup menu OR a paned
	 * window obj is being destroyed (via the "Unmake
	 * Paned Window" popup menu) and therefore its
	 * children are being reparented to the paned window's
	 * parent.  If a new paned window was created, we don't
	 * need to update the panes list because that will happen
	 * when the new paned window obj is selected. 
	 */
	if ((info->old_parent != NULL) && 
	    obj_is_paned_win(info->old_parent))
	{
	    for (i = 0; i < AB_PROP_TYPE_NUM_VALUES; ++i)
	    {
		pws = &(prop_pw_settings_rec[i]);
		if (pws->current_pane_obj == info->obj)
		    panedwin_prop_clear_geom((AB_PROP_TYPE) i);
		    ret = abobj_list_obj_reparented(pws->panelist, info,
				pw_child_test_func);
	    }
	}
    }
    return (ret);
}

static void
panedwinEdP_init(void)
{
    obj_add_reparent_callback(pw_obj_reparentedOCB, "panedwinEdP_init");
    obj_add_rename_callback(pw_obj_renamedOCB, "panedwinEdP_init");
    obj_add_destroy_callback(pw_obj_destroyedOCB, "panedwinEdP_init");
}


/*
 * Create a paned window out of the selected group of objects.
 */
void
abobj_make_panedwin(void)
{
    ABObj         	project = proj_get_project();
    ABObj         	obj = (ABObj) NULL;
    ABObj         	pw_obj = (ABObj) NULL;
    ABObj         	obj_parent = (ABObj) NULL;
    ABSelectedRec 	sel;
    int		  	i, xpos = 0, ypos = 0;
    AB_ATTACH_TYPE	attach_type = AB_ATTACH_UNDEF,
			next_attach_type = AB_ATTACH_UNDEF;
    void		*attach_val = NULL,
			*next_attach_val = NULL;
    int			attach_offset = 0;
    BOOL		AttachTypesEqual;

    /* Creation may take awhile, so set busy cursor */
    ab_set_busy_cursor(True);

    /* Get handle to the selected objects */
    abobj_get_selected(project, False, False, &sel);

    /* Get the parent for all the selected objects */
    obj_parent = obj_get_parent(sel.list[0]);

    /* Set the SaveNeeded flag on the module */
    abobj_set_save_needed(obj_get_module(obj_parent), TRUE);

    /* Deselect any objects that happen to be selected */
    abobj_deselect_all(project);
    aob_deselect_all_objects(project);

    /* Create a new panedWindow obj as a child of the
     * parent of the selected objects.
     */
    pw_obj = obj_create(AB_TYPE_CONTAINER, obj_parent);
    obj_set_subtype(pw_obj, AB_CONT_PANED);

    pal_initialize_obj(pw_obj);

    /* Sort the selected object list in the order in
     * which the panes will be placed once they are part
     * of a panedWindow (order is based on y coordinate).
     */
    qsort(sel.list, sel.count, sizeof(ABObj *), position_compare);

    /* Set the paned window object's NORTH and SOUTH attachments */
    attach_type = obj_get_attach_type(sel.list[0], AB_CP_NORTH);
    attach_val = obj_get_attach_value(sel.list[0], AB_CP_NORTH);
    attach_offset = obj_get_attach_offset(sel.list[0], AB_CP_NORTH);
    obj_set_attachment(pw_obj,
                        AB_CP_NORTH,
                        attach_type,
                        attach_val,
                        attach_offset);
 
    attach_type = obj_get_attach_type(sel.list[sel.count-1], AB_CP_SOUTH);
    attach_val = obj_get_attach_value(sel.list[sel.count-1], AB_CP_SOUTH);
    attach_offset = obj_get_attach_offset(sel.list[sel.count-1], AB_CP_SOUTH);
    obj_set_attachment(pw_obj,
                        AB_CP_SOUTH,
                        attach_type,
                        attach_val,
                        attach_offset);

    /* Reparent the selected objects to be children of the
     * new panedWindow obj, in the *correct* order.
     */
    for (i = 0; i < sel.count; i++)
    {
        obj = sel.list[i];
        obj_reparent(obj, pw_obj);
    }

    /* Sort the selected object list from rightmost object
     * to leftmost object.
     */
    qsort(sel.list, sel.count, sizeof(ABObj *), east_compare);

    /* Set the paned window object's EAST attachments */
    attach_type = obj_get_attach_type(sel.list[sel.count-1], AB_CP_EAST);
    attach_val = obj_get_attach_value(sel.list[sel.count-1], AB_CP_EAST);
    attach_offset = obj_get_attach_offset(sel.list[sel.count-1], AB_CP_EAST);
    obj_set_attachment(pw_obj,
                        AB_CP_EAST,
                        attach_type,
                        attach_val,
                        attach_offset);

    /* Sort the selected object list from leftmost object
     * to rightmost object. Have to do this because it is
     * possible that the leftmost and rightmost objects are
     * in fact one and the same (if it spans the entire window,
     * for example and the other pane does not).
     */
    qsort(sel.list, sel.count, sizeof(ABObj *), west_compare);

    /* Set the paned window object's WEST attachment */
    attach_type = obj_get_attach_type(sel.list[0],AB_CP_WEST);
    attach_val = obj_get_attach_value(sel.list[0],AB_CP_WEST);
    attach_offset = obj_get_attach_offset(sel.list[0],AB_CP_WEST);
    obj_set_attachment(pw_obj,
                        AB_CP_WEST,
                        attach_type,
                        attach_val,
                        attach_offset);

    /* Check whether or not the paned window's children's
     * EAST/WEST attachment types are equal.  If not, then post
     * a message saying that the attachments may not be correct
     * and that the user should use the Attachments Editor
     * to correct them. 
     */
    AttachTypesEqual = TRUE;
    attach_type = obj_get_attach_type(sel.list[0], AB_CP_EAST);
    for (i = 1; ((i < sel.count) && AttachTypesEqual); i++)
    {
	next_attach_type = obj_get_attach_type(sel.list[i], AB_CP_EAST);
	if (attach_type != next_attach_type)
	{
	    AttachTypesEqual = FALSE;
	    /* Post Message */
	    dtb_panedwin_ed_pw_east_attach_msg_initialize(
			&dtb_panedwin_ed_pw_east_attach_msg);
	    dtb_show_message((Widget)obj_parent->ui_handle,
			&dtb_panedwin_ed_pw_east_attach_msg, NULL,NULL);
	}
    }
    /* If all of the paned window's children EAST attachment
     * types are equal, then check the EAST attachment values.
     */
    if (AttachTypesEqual)
    {
	attach_val = obj_get_attach_value(sel.list[0], AB_CP_EAST);
	for (i = 1; i < sel.count; i++)
	{
	    next_attach_val = obj_get_attach_value(sel.list[i], AB_CP_EAST);
	    if (attach_val != next_attach_val)
            {
		/* Post Message */
	        dtb_panedwin_ed_pw_east_attach_msg_initialize(
			&dtb_panedwin_ed_pw_east_attach_msg);
		dtb_show_message((Widget)obj_parent->ui_handle,
			&dtb_panedwin_ed_pw_east_attach_msg, NULL,NULL);
		break;
            }
	}
    }

    AttachTypesEqual = TRUE;
    attach_type = obj_get_attach_type(sel.list[0], AB_CP_WEST);
    for (i = 1; ((i < sel.count) && AttachTypesEqual); i++)
    {
        next_attach_type = obj_get_attach_type(sel.list[i], AB_CP_WEST);
        if (attach_type != next_attach_type)
        {
	    AttachTypesEqual = FALSE;
            /* Post Message */
	    dtb_panedwin_ed_pw_west_attach_msg_initialize(
			&dtb_panedwin_ed_pw_west_attach_msg);
            dtb_show_message((Widget)obj_parent->ui_handle,
                        &dtb_panedwin_ed_pw_west_attach_msg, NULL,NULL); 
        }
    }  
    /* If all of the paned window's children WEST attachment
     * types are equal, then check the WEST attachment values.
     */
    if (AttachTypesEqual)
    {  
        attach_val = obj_get_attach_value(sel.list[0], AB_CP_WEST);
        for (i = 1; i < sel.count; i++) 
        {
            next_attach_val = obj_get_attach_value(sel.list[i], AB_CP_WEST);
            if (attach_val != next_attach_val)
            {
                /* Post Message */ 
	        dtb_panedwin_ed_pw_west_attach_msg_initialize(
			&dtb_panedwin_ed_pw_west_attach_msg);
		dtb_show_message((Widget)obj_parent->ui_handle,
                        &dtb_panedwin_ed_pw_west_attach_msg, NULL,NULL); 
                break;
            } 
        }
    }

    obj_tree_clear_flag(pw_obj, InstantiatedFlag);
    abobj_show_tree(pw_obj, True);
 
    /* Make the new obj selected */
    abobj_select(pw_obj);

    ab_set_busy_cursor(False);
}

static int
position_compare(
    const void *leftEntry,
    const void *rightEntry
)
{
    ABObj *left_obj = (ABObj *)leftEntry;
    ABObj *right_obj = (ABObj *)rightEntry;

    int	  left_xpos = obj_get_x(*left_obj);
    int   left_ypos = obj_get_y(*left_obj);
    int	  right_xpos = obj_get_x(*right_obj);
    int   right_ypos = obj_get_y(*right_obj);

    if (left_ypos - right_ypos != 0)
	return (left_ypos - right_ypos);
    else
	return (left_xpos - right_xpos);
}

static int
east_compare(
    const void *leftEntry,
    const void *rightEntry
)
{
    ABObj *left_obj = (ABObj *)leftEntry;
    ABObj *right_obj = (ABObj *)rightEntry;
 
    int   left_xpos = obj_get_x(*left_obj) + 
			abobj_get_actual_width(*left_obj);
    int   right_xpos = obj_get_x(*right_obj) +
			abobj_get_actual_width(*right_obj);
    return (left_xpos - right_xpos);
}

static int
west_compare(
    const void *leftEntry,
    const void *rightEntry
)
{
    ABObj *left_obj = (ABObj *)leftEntry;
    ABObj *right_obj = (ABObj *)rightEntry;

    int   left_xpos = obj_get_x(*left_obj);
    int   right_xpos = obj_get_x(*right_obj);

    return (left_xpos - right_xpos);
}

/*
 * Take the selected group of objects out of the
 * paned window and delete the paned window object.
 */
void
abobj_unmake_panedwin(void)
{
    ABObj		project = proj_get_project();
    ABObj		pw_obj = NULL;
    ABObj		obj_parent = NULL;
    ABObj         	obj = NULL;
    ABObj         	*list = NULL;
    ABSelectedRec 	sel;
    int           	i, x_orig = 0, y_orig = 0, ypos = 0;
    int		  	j, numChildren = 0, tree_pos = 0;
    int           	list_items = 0; 
    Boolean		parent_exists = True;
 
    /* Undo may take awhile, so set busy cursor */
    ab_set_busy_cursor(True);

    /* Set the SaveNeeded flag */
    abobj_set_save_needed(project, TRUE);

    /* Get handle to the paned window objects */
    abobj_get_selected(project, False, False, &sel);

    /* Go through each paned window object and move
     * its children to the paned window's parent.
     */
    for (i = 0; i < sel.count; i++)
    {
	pw_obj = sel.list[i];

	/* Deselect the paned window */
	abobj_deselect(pw_obj);

	/* Get the parent for all the selected objects */
	obj_parent = obj_get_parent(pw_obj);

	/* Set the SaveNeeded flag on the module */
	abobj_set_save_needed(obj_get_module(pw_obj), TRUE);

	/* Get the child position of the paned window
	 * object in its parent hierarchy.
	 */ 
	tree_pos = obj_get_child_num(pw_obj);

 	/* Get the x,y position of the paned window */
	x_orig = obj_get_x(pw_obj);
	y_orig = obj_get_y(pw_obj);
	ypos = y_orig;

	numChildren = obj_get_num_salient_children(pw_obj);
	for (j = 0; j < numChildren; j++, tree_pos++)
	{
	    /* The first child really is the next
	     * child because the previous child has
	     * been reparented to the paned window's
	     * parent.
	     */
	    obj = obj_get_salient_child(pw_obj, 0);
	    obj_unparent(obj);
	    obj_insert_child(obj_parent, obj, tree_pos);

	    abobj_set_xy(obj, x_orig, ypos);
	    ypos = ypos + abobj_get_actual_height(obj);

	    objxm_tree_uninstantiate(obj, True);
	    abobj_instantiate_changes(obj);
	}
	
	/* Destroy the paned window object */
	obj_destroy(pw_obj);

	/* REMIND: Not sure if we need this code.
	 * 	Paned windows should all have the
	 * 	same parent, given the constraint
	 *	that multiple paned windows cannot
	 * 	be selected across different modules.
	 */
	/*                   
         * Keep a list of parents that need to be 
	 * updated after all paned windows are unpaned.
         */
        if (list == NULL)
        { 
	    list = (ABObj*)XtMalloc(sizeof(ABObj));
            list[0] = obj_parent;
            list_items = 1;
        }
        else
        for (j = 0; j < list_items; j++)
        {
            if (list[j] == obj_parent)
            {
		parent_exists = True;
		break;
            }
        }
        if (parent_exists == False)
        {
            XtRealloc((char*)list, sizeof(ABObj));
            list[list_items++] = obj_parent;
        }
        parent_exists = False;
    }

    ab_set_busy_cursor(False);
} 
