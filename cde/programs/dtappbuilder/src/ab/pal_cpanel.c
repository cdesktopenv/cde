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
 *      $XConsortium: pal_cpanel.c /main/5 1996/08/08 17:52:51 mustafa $
 *
 * @(#)pal_cpanel.c	1.36 14 Feb 1994      cde_app_builder/src/ab
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
 * pal_cpanel.c - Implements Palette ControlPane object functionality
 */
#include <stdio.h>
#include <Xm/Xm.h>
#include <ab_private/pal.h>
#include <ab_private/ab.h>
#include <ab_private/prop.h>
#include <ab_private/abobj.h>
#include <ab_private/abobj_set.h> 
#include <ab_private/ui_util.h>
#include "cpanel_ui.h"

const    int    cpanel_init_height = 120;
const    int    cpanel_init_width  = 200;

typedef struct  PROP_CPANEL_SETTINGS
{
    Widget                      prop_sheet;
    PropFieldSettingRec         name;
    PropRadioSettingRec         size_policy;
    PropGeometrySettingRec      geometry;
    PropCheckboxSettingRec      init_state;
    PropOptionsSettingRec       frame;
    PropColorSettingRec         bg_color;
    PropMenunameSettingRec      menuname;
    PropFieldSettingRec         menu_title;
    ABObj                       current_obj;
} PropCPanelSettingsRec, *PropCPanelSettings;

/*************************************************************************
**                                                                      **
**       Private Function Declarations                                  **
**                                                                      **
**************************************************************************/
/*
 * Methods
 */
static int	cpanel_initialize(
                    ABObj   obj
                );
static Widget   cpanel_prop_init(
                    Widget  parent,
                    AB_PROP_TYPE type
                );
static int	cpanel_prop_activate(
		    AB_PROP_TYPE type,
		    BOOL 	 active
		);
static int	cpanel_prop_clear(
                    AB_PROP_TYPE type
		);
static int      cpanel_prop_load(
                    ABObj   	 obj,
                    AB_PROP_TYPE type,
 		    unsigned long loadkey
                );
static int	cpanel_prop_apply(
		    AB_PROP_TYPE type
		);
static BOOL     cpanel_prop_pending(
                    AB_PROP_TYPE type
                );

static BOOL	verify_props(
		    AB_PROP_TYPE type
		);
static void	turnoff_changebars(
		    AB_PROP_TYPE type
		);



/*************************************************************************
**                                                                      **
**       Data 		                                                **
**                                                                      **
**************************************************************************/
PalItemInfo cpanel_palitem_rec = {

    /* type             */  AB_TYPE_CONTAINER,
    /* name             */  "Control Pane",
    /* animation pixmaps*/  NULL,
    /* number of pixmaps*/  0,
    /* rev_prop_frame   */  NULL,
    /* fix_prop_dialog  */  NULL,
    /* initialize       */  cpanel_initialize,
    /* is_a_test        */  obj_is_control_panel,
    /* prop_initialize  */  cpanel_prop_init,
    /* prop_activate	*/  cpanel_prop_activate,
    /* prop_clear	*/  cpanel_prop_clear,
    /* prop_load        */  cpanel_prop_load,
    /* prop_apply	*/  cpanel_prop_apply,
    /* prop_pending	*/  cpanel_prop_pending
};

PalItemInfo *ab_cpanel_palitem = &cpanel_palitem_rec;
PropCPanelSettingsRec prop_cpanel_settings_rec[AB_PROP_TYPE_NUM_VALUES];


/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/

static int
cpanel_initialize(
    ABObj     obj
)
{
    ABObj		parent;
    AB_CONTAINER_TYPE	cont_type;
    STRING		name;

    parent = obj_get_root(obj_get_parent(obj));
    cont_type = obj_get_container_type(obj);

    switch(cont_type)
    {
	case AB_CONT_BUTTON_PANEL:
	    name = ab_ident_from_name_and_label(obj_get_name(parent),
			"button_panel");
	    obj_set_height(obj, 48);
	    break;
	case AB_CONT_FOOTER:
	    name = ab_ident_from_name_and_label(obj_get_name(parent),
			"footer");
	    obj_set_height(obj, 28);
	    break;
	case AB_CONT_TOOL_BAR:
	    name = ab_ident_from_name_and_label(obj_get_name(parent),
			"toolbar");
	    obj_set_height(obj, 48);
	    break;
	default: /* Standard Control Pane */
	    abobj_init_pane_position(obj);
	    name = "controlpane";

	    /* If control pane was added as a layer, the size has already been
	     * set by the size of the layer.
             */
	    if (!obj_is_layers(parent))
	    {
	    	obj_set_width(obj, cpanel_init_width);
	    	obj_set_height(obj, cpanel_init_height);
	    }
	    break;
    }
    obj_set_unique_name(obj, name);

    obj_set_is_initially_visible(obj, True);
    obj_set_is_initially_active(obj, True);

    return OK;

}

static Widget
cpanel_prop_init(
    Widget parent,
    AB_PROP_TYPE type
)
{
    DtbCpanelPropDialogInfoRec	rev_cpanel_prop_dialog; /* Revolving Props */
    DtbCpanelPropDialogInfo	cgen = &dtb_cpanel_prop_dialog; /* Codegen structure */
    DtbRevolvPropDialogInfo	rpd = &(dtb_revolv_prop_dialog);
    PropCPanelSettingsRec    	*pcs = &(prop_cpanel_settings_rec[type]);
    Widget			item[10];
    int				item_val[10];
    int				i, n;

    if (type == AB_PROP_REVOLVING)
    {
	/* Cloning Trick:
	 * Only the Attributes ControlPanel needs to be created within
	 * the existing Revolving Prop dialog, so fill out all other
	 * fields with the Revolving Prop dialog equivelents, so the
	 * dtb initialize proc will skip those non-NULL fields...
	 */
        dtbCpanelPropDialogInfo_clear(&rev_cpanel_prop_dialog);

	cgen = &(rev_cpanel_prop_dialog);
	cgen->prop_dialog = rpd->prop_dialog;
        cgen->prop_dialog_shellform = rpd->prop_dialog_shellform; 
	cgen->prop_dialog_panedwin = rpd->prop_dialog_panedwin;
	cgen->prop_dialog_form = rpd->prop_dialog_form;
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
    	cgen = &dtb_cpanel_prop_dialog; 

    if (dtb_cpanel_prop_dialog_initialize(cgen, parent) == 0)
    {
	pcs->prop_sheet = cgen->attrs_ctrlpanel;
        pcs->current_obj = NULL;

	if (type == AB_PROP_REVOLVING)
        	XtVaSetValues(parent,
                        XmNuserData, pcs->current_obj,
                        NULL);

        /* Dialog/Object List */
	if (type == AB_PROP_FIXED)
	{
            prop_fixed_dialog_init(ab_cpanel_palitem,
			cgen->prop_dialog_shellform, cgen->objlist);
            prop_activate_panel_init(type, ab_cpanel_palitem, 
                        cgen->ok_button, cgen->apply_button, 
                        cgen->reset_button, cgen->cancel_button,
                        cgen->help_button);
	}
	/* Alternate Editor Buttons */
	prop_editors_panel_init(type, ab_cpanel_palitem,
		    cgen->attach_button, cgen->conn_button, cgen->helptxt_button);

	/*
	  * Prop Sheet Settings....
	 */

	/* Name Field */
	prop_field_init(&(pcs->name), cgen->name_field_label,
			    cgen->name_field, cgen->name_cb);

        /* Border Frame Setting */
        n = 0;
        item[n] = cgen->bframe_opmenu_items.None_item;
        item_val[n] = AB_LINE_NONE; n++;
        item[n] = cgen->bframe_opmenu_items.Shadow_Out_item;
        item_val[n] = AB_LINE_SHADOW_OUT; n++;
        item[n] = cgen->bframe_opmenu_items.Shadow_In_item;
        item_val[n] = AB_LINE_SHADOW_IN; n++;
        item[n] = cgen->bframe_opmenu_items.Etched_Out_item;
        item_val[n] = AB_LINE_ETCHED_OUT; n++;
        item[n] = cgen->bframe_opmenu_items.Etched_In_item;
        item_val[n] = AB_LINE_ETCHED_IN; n++;
        prop_options_init(&(pcs->frame), cgen->bframe_opmenu_label,
                            cgen->bframe_opmenu, cgen->bframe_opmenu_menu,
                            n, item, (XtPointer*)item_val,
                            cgen->bframe_cb);

	/* Menu Title */
        prop_field_init(&(pcs->menu_title), cgen->menutitle_field_label,
                            cgen->menutitle_field, cgen->menutitle_cb);

        /* Menu Name Setting */
        prop_menuname_init(&(pcs->menuname), type, cgen->menu_label,
			cgen->menu_mbutton, cgen->menu_field,
                        cgen->name_field, cgen->menu_cb,
			&(pcs->menu_title),
                        &(pcs->current_obj), True);

        /* Size Policy */
        n = 0;
        item[n] = cgen->size_policy_rbox_items.Fit_Contents_item;
        item_val[n] = SIZE_OF_CONTENTS_KEY; n++;
        item[n] = cgen->size_policy_rbox_items.Fixed_item;
        item_val[n] = SIZE_FIXED_KEY; n++;
        prop_radiobox_init(&(pcs->size_policy), cgen->size_policy_rbox_label,
                cgen->size_policy_rbox, n, item, (XtPointer*)item_val,
                cgen->size_policy_cb);

        for(i=0; i < n; i++)
            XtAddCallback(item[i], XmNvalueChangedCallback,
                        (XtCallbackProc)prop_size_policyCB, (XtPointer)&(pcs->geometry));

	/* Geometry Setting */
	prop_geomfield_init(&(pcs->geometry), cgen->geom_label,
			    cgen->x_field_label, cgen->x_field,
			    cgen->y_field_label, cgen->y_field,
			    cgen->width_field_label, cgen->width_field,
			    cgen->height_field_label, cgen->height_field,
			    cgen->geom_cb);

	/* Initial State Setting */
	n = 0;
	item[n] = cgen->istate_ckbox_items.Visible_item;
	item_val[n] = AB_STATE_VISIBLE; n++;
	item[n] = cgen->istate_ckbox_items.Active_item;
	item_val[n] = AB_STATE_ACTIVE; n++;
	prop_checkbox_init(&(pcs->init_state), cgen->istate_ckbox_label,
			    cgen->istate_ckbox, n, item, item_val,
			    cgen->istate_cb);

	/* Background Setting */
	prop_colorfield_init(&(pcs->bg_color), cgen->bg_mbutton,
                cgen->bg_mbutton_bg_mbutton_menu_items.None_item,
                cgen->bg_mbutton_bg_mbutton_menu_items.Color_Chooser_item,
		cgen->bg_swatch, cgen->bg_field, cgen->bg_cb);

	prop_changebars_cleared(pcs->prop_sheet);

	return (cgen->prop_dialog_shellform);
    }
    else
	return NULL;

}  

static int
cpanel_prop_activate(
    AB_PROP_TYPE type,
    BOOL	 active
)
{
    ui_set_active(prop_cpanel_settings_rec[type].prop_sheet, active);

    return OK;
}


static int
cpanel_prop_clear(
    AB_PROP_TYPE type
)
{
    PropCPanelSettingsRec	*pcs = &(prop_cpanel_settings_rec[type]);

    if (pcs->current_obj == NULL)
	return OK;

    /* Clear Name */
    prop_field_set_value(&(pcs->name), "", False);

    /* Clear Border Frame */
    prop_options_set_value(&(pcs->frame), (XtPointer)AB_LINE_NONE, False);

    /* Clear Menu Name/Title  */
    prop_menuname_set_value(&(pcs->menuname), "", False);
    prop_field_set_value(&(pcs->menu_title), "", False);

    /* Clear Size Policy */
    prop_radiobox_set_value(&(pcs->size_policy), (XtPointer)SIZE_FIXED_KEY, False);

    /* Clear Geometry */
    prop_geomfield_clear(&(pcs->geometry), GEOM_X);
    prop_geomfield_clear(&(pcs->geometry), GEOM_Y);
    prop_geomfield_clear(&(pcs->geometry), GEOM_WIDTH);
    prop_geomfield_clear(&(pcs->geometry), GEOM_HEIGHT);
 
    /* Clear Initial State */
    prop_checkbox_set_value(&(pcs->init_state), AB_STATE_VISIBLE, True, False);
    prop_checkbox_set_value(&(pcs->init_state), AB_STATE_ACTIVE, True, False);

    /* Clear Color */
    prop_colorfield_set_value(&(pcs->bg_color), "", False);

    pcs->current_obj = NULL;

    turnoff_changebars(type);

    return OK;
}


static int
cpanel_prop_load(
    ABObj    	 obj,
    AB_PROP_TYPE type,
    unsigned long loadkey
)
{
    PropCPanelSettingsRec 	*pcs = &(prop_cpanel_settings_rec[type]);
    BOOL		   	editable;
    BOOL                        load_all = (loadkey & LoadAll);
 
    if (obj == NULL)
    {
        if (pcs->current_obj != NULL)
            obj = pcs->current_obj;
        else
            return ERROR;
    }
    else if (!obj_is_control_panel(obj))
        return ERROR;
    else
        pcs->current_obj = obj;

    /* Load Name */
    if (load_all || loadkey & LoadName)
    	prop_field_set_value(&(pcs->name), obj_get_name(obj), False);

    if (load_all)
    {
	/* Load Border Frame */
	prop_options_set_value(&(pcs->frame), (XtPointer)obj_get_border_frame(obj), False);

	/* Load Menu Name/Title */
	prop_menuname_set_value(&(pcs->menuname), obj_get_menu_name(obj), False);
	prop_field_set_value(&(pcs->menu_title), obj_get_menu_title(obj), False);

	/* Load Size Policy */
	prop_radiobox_set_value(&(pcs->size_policy),
                                abobj_width_resizable(obj)? 
                                (XtPointer)SIZE_FIXED_KEY :
                                (XtPointer)SIZE_OF_CONTENTS_KEY, False);

	/* For now we will not allow the user to manipulate the "Size Policy"
	 * of specialized Control Panels; they will be "Fit Contents".
	 */
	switch(obj_get_container_type(obj)) 
	{
		case AB_CONT_BUTTON_PANEL:
		case AB_CONT_FOOTER:
		case AB_CONT_TOOL_BAR:
		editable = False;
			break;
		default:
		editable = True;
	}
   	ui_set_active(pcs->size_policy.label, editable);
   	ui_set_active(pcs->size_policy.radiobox, editable);

	/* Load Initial State */
	prop_checkbox_set_value(&(pcs->init_state), AB_STATE_VISIBLE,
		obj_is_initially_visible(obj), False);
	prop_checkbox_set_value(&(pcs->init_state), AB_STATE_ACTIVE,
		obj_is_initially_active(obj), False);

	/* Load Color */
	prop_colorfield_set_value(&(pcs->bg_color), obj_get_bg_color(obj), False); 

        turnoff_changebars(type);
    }

    /* Load Geometry */
    if (load_all || loadkey & LoadPosition)
        prop_load_obj_position(obj, &(pcs->geometry)); 

    if (load_all || loadkey & LoadSize)
        prop_load_obj_size(obj, &(pcs->geometry)); 

    return OK;
}

int
cpanel_prop_apply(
    AB_PROP_TYPE   type
)
{
    ABObj			pobj;
    PropCPanelSettingsRec 	*pcs = &(prop_cpanel_settings_rec[type]);
    ABObj			chg_root = pcs->current_obj;
    STRING			value;
    BOOL			reset_bg = False;
    BOOL			size_chg = False;
    int				new_w = -1;
    int				new_h = -1;

    if (!verify_props(type))
        return ERROR;

    if (prop_changed(pcs->name.changebar))
    {
        value = prop_field_get_value(&(pcs->name));   
        abobj_set_name(pcs->current_obj, value);
        util_free(value);
    }
    if (prop_changed(pcs->frame.changebar))
    {
        abobj_set_border_frame(pcs->current_obj,
                (AB_LINE_TYPE)prop_options_get_value(&(pcs->frame)));
    }
    if (prop_changed(pcs->menuname.changebar))
    {
        value = prop_menuname_get_value(&(pcs->menuname));
        abobj_set_menu_name(pcs->current_obj, value);
        util_free(value);
        prop_menuname_set_value(&(pcs->menuname), obj_get_menu_name(pcs->current_obj),
		False);
    }
    if (prop_changed(pcs->menu_title.changebar))
    {
        value = prop_field_get_value(&(pcs->menu_title));
        abobj_set_menu_title(pcs->current_obj, value);
        util_free(value);
    }
    if (prop_changed(pcs->size_policy.changebar))
    {
	abobj_set_size_policy(pcs->current_obj,
		prop_radiobox_get_value(&(pcs->size_policy)) == SIZE_FIXED_KEY);

	size_chg = True;
    }
    if (prop_changed(pcs->geometry.changebar))
    {
	pobj = obj_get_root(obj_get_parent(pcs->current_obj));
	if (obj_is_layers(pobj))
	    chg_root = pobj;

    	if (abobj_width_resizable(pcs->current_obj))
	    new_w = prop_geomfield_get_value(&(pcs->geometry), GEOM_WIDTH);

	if (abobj_height_resizable(pcs->current_obj))
	    new_h = prop_geomfield_get_value(&(pcs->geometry), GEOM_HEIGHT);

	if (obj_is_layers(pobj) && 
	   (new_h != -1 || new_w != -1))
	    abobj_layer_set_size(pobj, new_w, new_h);
	else 
	{
	    if (new_w != -1)
		abobj_set_pixel_width(pcs->current_obj, new_w, 0);
	    if (new_h != -1)
		abobj_set_pixel_height(pcs->current_obj, new_h, 0);
	}

	if (abobj_is_movable(pcs->current_obj))
	    abobj_set_xy(chg_root,
	     	prop_geomfield_get_value(&(pcs->geometry), GEOM_X),
	     	prop_geomfield_get_value(&(pcs->geometry), GEOM_Y));
    }
    if (prop_changed(pcs->init_state.changebar))
    {
        abobj_set_visible(pcs->current_obj, 
		prop_checkbox_get_value(&(pcs->init_state), AB_STATE_VISIBLE));
        abobj_set_active(pcs->current_obj,
		prop_checkbox_get_value(&(pcs->init_state), AB_STATE_ACTIVE));
    }
    if (prop_changed(pcs->bg_color.changebar))
    {
        value = prop_colorfield_get_value(&(pcs->bg_color));   
        abobj_set_background_color(pcs->current_obj, value);
	if (util_strempty(value))
	    reset_bg = True;
        util_free(value);
        prop_colorfield_set_value(&(pcs->bg_color), obj_get_bg_color(pcs->current_obj), False);
    }
    abobj_instantiate_changes(pcs->current_obj);
    if (chg_root != pcs->current_obj)
    	abobj_tree_instantiate_changes(chg_root);

    if (reset_bg) /* Set back to No Color */ 
        abobj_reset_colors(pcs->current_obj, reset_bg, False); 

    turnoff_changebars(type);

    return OK;

}

static BOOL
cpanel_prop_pending(
    AB_PROP_TYPE type
)
{
    return(prop_changebars_pending(prop_cpanel_settings_rec[type].prop_sheet));
}

static BOOL
verify_props(
    AB_PROP_TYPE type
)
{
    PropCPanelSettingsRec *pcs = &(prop_cpanel_settings_rec[type]);

    if (prop_changed(pcs->name.changebar) && !prop_name_ok(pcs->current_obj, pcs->name.field))
        return False;

    if (prop_changed(pcs->geometry.changebar) &&
        (!prop_number_ok(pcs->geometry.x_field, (STRING)XFieldStr, -SHRT_MAX, SHRT_MAX) ||
         !prop_number_ok(pcs->geometry.y_field, (STRING)YFieldStr, -SHRT_MAX, SHRT_MAX) ||
         !prop_number_ok(pcs->geometry.w_field, (STRING)WFieldStr, 1, SHRT_MAX) ||
         !prop_number_ok(pcs->geometry.h_field, (STRING)HFieldStr, 1, SHRT_MAX)))
        return False;
 
    if (prop_changed(pcs->bg_color.changebar) && !prop_color_ok(pcs->bg_color.field))
        return False;

    if (prop_changed(pcs->menuname.changebar) && !prop_obj_name_ok(pcs->menuname.field,
			obj_get_module(pcs->current_obj), AB_TYPE_MENU, "Menu"))
	return False;

    return True;
}

static void
turnoff_changebars(
    AB_PROP_TYPE type
)
{
    PropCPanelSettingsRec *pcs = &(prop_cpanel_settings_rec[type]);

    prop_set_changebar(pcs->name.changebar,    		PROP_CB_OFF);
    prop_set_changebar(pcs->init_state.changebar,	PROP_CB_OFF);
    prop_set_changebar(pcs->frame.changebar,		PROP_CB_OFF);
    prop_set_changebar(pcs->size_policy.changebar,	PROP_CB_OFF);
    prop_set_changebar(pcs->geometry.changebar,		PROP_CB_OFF);
    prop_set_changebar(pcs->bg_color.changebar,  	PROP_CB_OFF);
    prop_set_changebar(pcs->menuname.changebar,  	PROP_CB_OFF);
    prop_set_changebar(pcs->menu_title.changebar,  	PROP_CB_OFF);

    prop_changebars_cleared(pcs->prop_sheet);

}
