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
 *      $XConsortium: pal_drawp.c /main/5 1996/08/08 18:00:35 mustafa $
 *
 * @(#)pal_graphp.c	1.32 14 Feb 1994      cde_app_builder/src/ab
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
 * pal_graphp.c - Implements Palette Drawing Area Pane object functionality
 */
#include <stdio.h>
#include <Xm/Xm.h>
#include <ab_private/util.h>
#include <ab_private/ab.h>
#include <ab_private/pal.h>
#include <ab_private/prop.h>
#include <ab_private/abobj.h> 
#include <ab_private/abobj_set.h> 
#include <ab_private/ui_util.h>
#include "drawp_ui.h"

const    int    drawp_init_height = 80;
const    int    drawp_init_width  = 80;

typedef struct  PROP_DRAWP_SETTINGS
{
    Widget                      prop_sheet;
    PropFieldSettingRec         name;
    PropRadioSettingRec         scrolling;
    PropGeometrySettingRec      geom;
    PropGeometrySettingRec      size;
    PropCheckboxSettingRec      init_state;
    PropOptionsSettingRec       frame;
    PropColorSettingRec         bg_color;
    PropColorSettingRec         fg_color;
    PropMenunameSettingRec      menuname;
    PropFieldSettingRec         menu_title;
    ABObj                       current_obj;
} PropDrawpSettingsRec, *PropDrawpSettings;

/*************************************************************************
**                                                                      **
**       Private Function Declarations                                  **
**                                                                      **
**************************************************************************/
/*
 * Methods
 */
static int	drawp_initialize(
                    ABObj   obj
                );
static Widget   drawp_prop_init(
                    Widget  parent,
                    AB_PROP_TYPE type
                );
static int	drawp_prop_activate(
		    AB_PROP_TYPE type,
		    BOOL 	 active
		);
static int 	drawp_prop_clear(
                    AB_PROP_TYPE type
                );	
static int      drawp_prop_load(
                    ABObj   	 obj,
                    AB_PROP_TYPE type,
 		    unsigned long loadkey
                );
static int	drawp_prop_apply(
		    AB_PROP_TYPE type
		);
static BOOL     drawp_prop_pending(
                    AB_PROP_TYPE type
                );

static BOOL	verify_props(
		    AB_PROP_TYPE type
		);
static void	turnoff_changebars(
		    AB_PROP_TYPE type
		);

static void	setup_scroll_settings(
		    AB_PROP_TYPE 	type,
		    AB_SCROLLBAR_POLICY	sb_policy
		);
/* 
 * Callbacks
 */
static void	scroll_stateCB(
		    Widget	widget,
		    XtPointer   client_data,
		    XtPointer   call_data
		);

/*************************************************************************
**                                                                      **
**       Data 		                                                **
**                                                                      **
**************************************************************************/
PalItemInfo drawp_palitem_rec = {

    /* type             */  AB_TYPE_DRAWING_AREA,
    /* name             */  "Draw Area Pane",
    /* animation pixmaps*/  NULL,
    /* number of pixmaps*/  0,
    /* rev_prop_frame   */  NULL,
    /* fix_prop_dialog  */  NULL,
    /* initialize       */  drawp_initialize,
    /* is_a_test        */  obj_is_drawing_area,
    /* prop_initialize  */  drawp_prop_init,
    /* prop_activate	*/  drawp_prop_activate,
    /* prop_clear	*/  drawp_prop_clear,
    /* prop_load        */  drawp_prop_load,
    /* prop_apply	*/  drawp_prop_apply,
    /* prop_pending	*/  drawp_prop_pending

};

PalItemInfo *ab_drawp_palitem = &drawp_palitem_rec;
PropDrawpSettingsRec prop_drawp_settings_rec[AB_PROP_TYPE_NUM_VALUES];


/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/

static int
drawp_initialize(
    ABObj     obj
)
{
    ABObj	parent = obj_get_root(obj_get_parent(obj));

    obj_set_unique_name(obj, "drawpane");
    obj_set_bg_color(obj, "white");
    
    if (!obj_is_layers(parent)) 
    {
    	obj_set_width(obj, drawp_init_width);
	obj_set_height(obj, drawp_init_height);
    }
    obj_set_hscrollbar_policy(obj, AB_SCROLLBAR_WHEN_NEEDED);
    obj_set_vscrollbar_policy(obj, AB_SCROLLBAR_WHEN_NEEDED);

    /* Actual DrawingArea Size - only valid w/scrolling */
    obj_set_drawarea_width(obj, 400);
    obj_set_drawarea_height(obj, 400);

    abobj_init_pane_position(obj);

    return OK;

}

static Widget
drawp_prop_init(
    Widget parent,
    AB_PROP_TYPE type
)
{
    DtbDrawpPropDialogInfoRec	rev_drawp_prop_dialog; /* Revolving Props */
    DtbDrawpPropDialogInfo	cgen = &dtb_drawp_prop_dialog; /* Codegen structure */
    DtbRevolvPropDialogInfo	rpd = &(dtb_revolv_prop_dialog);
    PropDrawpSettingsRec    	*pds = &(prop_drawp_settings_rec[type]);
    Widget			item[10];
    int				item_val[10];
    int				n, i;

    if (type == AB_PROP_REVOLVING)
    {
	/* Cloning Trick:
	 * Only the Attributes ControlPanel needs to be created within
	 * the existing Revolving Prop dialog, so fill out all other
	 * fields with the Revolving Prop dialog equivelents, so the
	 * dtb initialize proc will skip those non-NULL fields...
	 */
        dtbDrawpPropDialogInfo_clear(&rev_drawp_prop_dialog);

	cgen = &(rev_drawp_prop_dialog);
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
    	cgen = &dtb_drawp_prop_dialog; 

    if (dtb_drawp_prop_dialog_initialize(cgen, parent) == 0)
    {
        pds->prop_sheet = cgen->attrs_ctrlpanel;
        pds->current_obj = NULL;

	if (type == AB_PROP_REVOLVING)
        	XtVaSetValues(parent,
                        XmNuserData, pds->current_obj,
                        NULL);

        /* Dialog/Object List */
	if (type == AB_PROP_FIXED)
	{
            prop_fixed_dialog_init(ab_drawp_palitem,
			cgen->prop_dialog_shellform, cgen->objlist);
            prop_activate_panel_init(type, ab_drawp_palitem, 
                        cgen->ok_button, cgen->apply_button, 
                        cgen->reset_button, cgen->cancel_button,
                        cgen->help_button);
	}

	/* Alternate Editor Buttons */
	prop_editors_panel_init(type, ab_drawp_palitem,
		    cgen->attach_button, cgen->conn_button, cgen->helptxt_button);

	/*
	  * Prop Sheet Settings....
	 */

	/* Name */
	prop_field_init(&(pds->name), cgen->name_field_label,
		    cgen->name_field, cgen->name_cb);

	/* Scrolling */
        n = 0;
        item[n] = cgen->scrolldisp_rbox_items.Never_item;
        item_val[n] = AB_SCROLLBAR_NEVER; n++;
        item[n] = cgen->scrolldisp_rbox_items.Always_item;
        item_val[n] = AB_SCROLLBAR_ALWAYS; n++;
        item[n] = cgen->scrolldisp_rbox_items.When_Needed_item;
        item_val[n] = AB_SCROLLBAR_WHEN_NEEDED; n++;
        prop_radiobox_init(&(pds->scrolling), cgen->scrolldisp_rbox_label,
                cgen->scrolldisp_rbox, n, item, (XtPointer*)item_val,
                cgen->scroll_cb);

        for(i=0; i < n; i++)
            XtAddCallback(item[i], XmNvalueChangedCallback,
                        (XtCallbackProc)scroll_stateCB, (XtPointer)type);

        /* Border Frame */
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
        prop_options_init(&(pds->frame), cgen->bframe_opmenu_label,
                            cgen->bframe_opmenu, cgen->bframe_opmenu_menu,
                            n, item, (XtPointer*)item_val,
                            cgen->bframe_cb);

	/* Menu Title */
        prop_field_init(&(pds->menu_title), cgen->menutitle_field_label,
                            cgen->menutitle_field, cgen->menutitle_cb);

        /* Menu Name */
        prop_menuname_init(&(pds->menuname), type, cgen->menu_label,
			cgen->menu_mbutton,
			cgen->menu_field,
                        cgen->name_field, cgen->menu_cb,
			&(pds->menu_title),
                        &(pds->current_obj), True);

	/* Size */
        prop_geomfield_init(&(pds->geom), cgen->geom_label,
			    cgen->x_field_label, cgen->x_field, 
			    cgen->y_field_label, cgen->y_field,
                            cgen->width_field_label, cgen->width_field,
                            cgen->height_field_label, cgen->height_field,
                            cgen->geom_cb);

        /* Canvas Size */ 
        prop_geomfield_init(&(pds->size), cgen->tsize_label, 
                            NULL, NULL, NULL, NULL,
                            cgen->twidth_field_label, cgen->twidth_field, 
                            cgen->theight_field_label, cgen->theight_field, 
                            cgen->tsize_cb);


	/* Initial State */
	n = 0;
	item[n] = cgen->istate_ckbox_items.Visible_item;
	item_val[n] = AB_STATE_VISIBLE; n++;
	item[n] = cgen->istate_ckbox_items.Active_item;
	item_val[n] = AB_STATE_ACTIVE; n++;
	prop_checkbox_init(&(pds->init_state), cgen->istate_ckbox_label,
			    cgen->istate_ckbox, n, item, item_val,
			    cgen->istate_cb);

	/* Color */
	prop_colorfield_init(&(pds->bg_color), cgen->bg_mbutton,
                cgen->bg_mbutton_bg_mbutton_menu_items.None_item,
                cgen->bg_mbutton_bg_mbutton_menu_items.Color_Chooser_item,
		cgen->bg_swatch, cgen->bg_field, cgen->bg_cb);

	prop_colorfield_init(&(pds->fg_color), cgen->fg_mbutton, 
                cgen->fg_mbutton_fg_mbutton_menu_items.None_item, 
                cgen->fg_mbutton_fg_mbutton_menu_items.Color_Chooser_item, 
                cgen->fg_swatch, cgen->fg_field, cgen->fg_cb); 

	prop_changebars_cleared(pds->prop_sheet);

	return (cgen->prop_dialog_shellform);
    }
    else
	return NULL;
}

static int
drawp_prop_activate(
    AB_PROP_TYPE type,
    BOOL	 active
)
{
    ui_set_active(prop_drawp_settings_rec[type].prop_sheet, active);

    return OK;
}


static int
drawp_prop_clear(
    AB_PROP_TYPE type
)
{
    PropDrawpSettingsRec	*pds = &(prop_drawp_settings_rec[type]);

    if (pds->current_obj == NULL)
	return OK;

    /* Clear Name */
    prop_field_set_value(&(pds->name), "", False);

    /* Clear Scrolling */
    prop_radiobox_set_value(&(pds->scrolling),
                            (XtPointer)AB_SCROLLBAR_WHEN_NEEDED, False); 

    /* Clear Border Frame */
    prop_options_set_value(&(pds->frame), (XtPointer)AB_LINE_NONE, False);

    /* Clear Menu Name/Title */
    prop_menuname_set_value(&(pds->menuname), "", False);
    prop_field_set_value(&(pds->menu_title), "", False);

    /* Clear Geometry */
    prop_geomfield_clear(&(pds->geom), GEOM_X);
    prop_geomfield_clear(&(pds->geom), GEOM_Y);
    prop_geomfield_clear(&(pds->geom), GEOM_WIDTH);
    prop_geomfield_clear(&(pds->geom), GEOM_HEIGHT);

    /* Clear Total Size */
    prop_geomfield_clear(&(pds->size), GEOM_WIDTH); 
    prop_geomfield_clear(&(pds->size), GEOM_HEIGHT); 
 
    /* Clear Initial State */
    prop_checkbox_set_value(&(pds->init_state), AB_STATE_VISIBLE, True, False);
    prop_checkbox_set_value(&(pds->init_state), AB_STATE_ACTIVE, True, False);

    /* Clear Color */
    prop_colorfield_set_value(&(pds->bg_color), "", False);
    prop_colorfield_set_value(&(pds->fg_color), "", False);

    pds->current_obj = NULL;

    turnoff_changebars(type);

    return OK;
}

static int
drawp_prop_load(
    ABObj    	 obj,
    AB_PROP_TYPE type,
    unsigned long loadkey
)
{
    PropDrawpSettingsRec 	*pds = &(prop_drawp_settings_rec[type]);
    AB_SCROLLBAR_POLICY		sb_policy = AB_SCROLLBAR_UNDEF;
    BOOL			load_all = (loadkey & LoadAll);
 
    if (obj == NULL)
    {
        if (pds->current_obj != NULL)
            obj = pds->current_obj;
        else
            return ERROR;
    }
    else if (!obj_is_drawing_area(obj))
        return ERROR;
    else
        pds->current_obj = obj;

    /* Load Name */
    if (load_all || loadkey & LoadName)
    	prop_field_set_value(&(pds->name), obj_get_name(obj), False);

    if (load_all)
    {
	/* Load Scrolling*/
	sb_policy = obj_get_vscrollbar_policy(obj);
	prop_radiobox_set_value(&(pds->scrolling),
                                (XtPointer)sb_policy, False);
	setup_scroll_settings(type, sb_policy);

	/* Load Border Frame */
	prop_options_set_value(&(pds->frame), (XtPointer)obj_get_border_frame(obj), False);

	/* Load Menu Name/Title */
	prop_menuname_set_value(&(pds->menuname), obj_get_menu_name(obj), False);
	prop_field_set_value(&(pds->menu_title), obj_get_menu_title(obj), False);
 
	/* Load Initial State */
	prop_checkbox_set_value(&(pds->init_state), AB_STATE_VISIBLE,
		obj_is_initially_visible(obj), False);
	prop_checkbox_set_value(&(pds->init_state), AB_STATE_ACTIVE,
		obj_is_initially_active(obj), False);

	/* Load Color */
	prop_colorfield_set_value(&(pds->bg_color), obj_get_bg_color(obj), False); 
	prop_colorfield_set_value(&(pds->fg_color), obj_get_fg_color(obj), False);

	turnoff_changebars(type);
    }

    /* Load Geometry */
    if (load_all || loadkey & LoadPosition)
        prop_load_obj_position(obj, &(pds->geom)); 

    if (load_all || loadkey & LoadSize)
    {
        prop_load_obj_size(obj, &(pds->geom)); 
 
        /* Load Total Size */
        if (sb_policy != AB_SCROLLBAR_NEVER)
        {
            prop_geomfield_set_value(&(pds->size), GEOM_WIDTH,
            	obj_get_drawarea_width(obj), False);
            prop_geomfield_set_value(&(pds->size), GEOM_HEIGHT,
            	obj_get_drawarea_height(obj), False);
        }
    }

    return OK;
}

int
drawp_prop_apply(
    AB_PROP_TYPE   type
)
{
    PropDrawpSettingsRec 	*pds = &(prop_drawp_settings_rec[type]);
    ABObj			chg_root = pds->current_obj;
    STRING			value;
    BOOL			size_chg = False;
    int				wth, hgt;
    BOOL			reset_bg = False;
    BOOL			reset_fg = False;

    if (!verify_props(type))
        return ERROR;

    if (prop_changed(pds->name.changebar))
    {
        value = prop_field_get_value(&(pds->name));   
        abobj_set_name(pds->current_obj, value);
        util_free(value);
    }
    if (prop_changed(pds->scrolling.changebar))
    {
        abobj_set_scrollbar_state(pds->current_obj,
            (AB_SCROLLBAR_POLICY)prop_radiobox_get_value(&(pds->scrolling)));

        size_chg = True;
    }
    if (prop_changed(pds->frame.changebar))
    {
        abobj_set_border_frame(pds->current_obj,
                (AB_LINE_TYPE)prop_options_get_value(&(pds->frame)));
    }
    if (prop_changed(pds->menuname.changebar))
    {
        value = prop_menuname_get_value(&(pds->menuname));
        abobj_set_menu_name(pds->current_obj, value);
        util_free(value);
        prop_menuname_set_value(&(pds->menuname),
		obj_get_menu_name(pds->current_obj), False);
    }
    if (prop_changed(pds->menu_title.changebar))
    {
        value = prop_field_get_value(&(pds->menu_title));
        abobj_set_menu_title(pds->current_obj, value);
        util_free(value);
    }
    if (prop_changed(pds->geom.changebar))
    {
     	ABObj	pobj;
        int 	border;

	pobj = obj_get_root(obj_get_parent(pds->current_obj));
	wth = prop_geomfield_get_value(&(pds->geom), GEOM_WIDTH);
	hgt = prop_geomfield_get_value(&(pds->geom), GEOM_HEIGHT);

        border = /* assumption! */
            ((prop_options_get_value(&(pds->frame)) != (XtPointer)AB_LINE_NONE)? 4 : 0);

	if (obj_is_layers(pobj))
	{
	    abobj_layer_set_size(pobj, wth, hgt);
	    chg_root = pobj;
	}
	else 
	{
            abobj_set_pixel_width(pds->current_obj, wth, border);
            abobj_set_pixel_height(pds->current_obj, hgt, border);
	}

	/* Note: if drawpane is a layer, then position must be set on
	 * the layer (not the drawpane).
	 */
	if (abobj_is_movable(pds->current_obj))
	    abobj_set_xy(chg_root,
	     	prop_geomfield_get_value(&(pds->geom), GEOM_X),
	     	prop_geomfield_get_value(&(pds->geom), GEOM_Y));
 
        size_chg = True;
    }
    if (prop_changed(pds->size.changebar))
    {
        wth = prop_geomfield_get_value(&(pds->size), GEOM_WIDTH); 
        hgt = prop_geomfield_get_value(&(pds->size), GEOM_HEIGHT);
        abobj_set_drawarea_size(pds->current_obj, wth, hgt);

        size_chg = True;
    }
    if (prop_changed(pds->init_state.changebar))
    {
        abobj_set_visible(pds->current_obj, 
		prop_checkbox_get_value(&(pds->init_state), AB_STATE_VISIBLE));
        abobj_set_active(pds->current_obj,
		prop_checkbox_get_value(&(pds->init_state), AB_STATE_ACTIVE));
    }
    if (prop_changed(pds->fg_color.changebar))
    {
        value = prop_colorfield_get_value(&(pds->fg_color));   
        abobj_set_foreground_color(pds->current_obj, value);
	if (util_strempty(value))
	    reset_fg = True;
        util_free(value);
        prop_colorfield_set_value(&(pds->fg_color), obj_get_fg_color(pds->current_obj), False);
    }
    if (prop_changed(pds->bg_color.changebar))
    {
        value = prop_colorfield_get_value(&(pds->bg_color));   
        abobj_set_background_color(pds->current_obj, value);
	if (util_strempty(value))
	    reset_bg = True;
        util_free(value);
        prop_colorfield_set_value(&(pds->bg_color), obj_get_bg_color(pds->current_obj), False);
    }
    abobj_instantiate_changes(pds->current_obj);
    if (chg_root != pds->current_obj) 
        abobj_tree_instantiate_changes(chg_root);

    if (reset_bg || reset_fg) /* Set back to No Color */ 
        abobj_reset_colors(pds->current_obj, reset_bg, reset_fg); 
    if (size_chg)
        abobj_force_dang_form_resize(pds->current_obj);

    turnoff_changebars(type);

    return OK;

}

static BOOL
drawp_prop_pending(
    AB_PROP_TYPE type
)
{
    return(prop_changebars_pending(prop_drawp_settings_rec[type].prop_sheet));
}

static BOOL
verify_props(
    AB_PROP_TYPE type
)
{
    PropDrawpSettingsRec *pds = &(prop_drawp_settings_rec[type]);

    if (prop_changed(pds->name.changebar) && 
	!prop_name_ok(pds->current_obj, pds->name.field))
        return False;

    if (prop_changed(pds->geom.changebar) &&
        (!prop_number_ok(pds->geom.x_field,(STRING)XFieldStr, -SHRT_MAX, SHRT_MAX) ||
         !prop_number_ok(pds->geom.y_field,(STRING)YFieldStr, -SHRT_MAX, SHRT_MAX) ||
         !prop_number_ok(pds->geom.w_field,(STRING)WFieldStr, 1, SHRT_MAX) ||
         !prop_number_ok(pds->geom.h_field,(STRING)HFieldStr, 1, SHRT_MAX)))
        return False;

    if (prop_changed(pds->size.changebar) &&
        (!prop_number_ok(pds->size.w_field, "DrawArea Width Field", 1, SHRT_MAX) || 
	 !prop_number_ok(pds->size.h_field, "DrawArea Height Field", 1, SHRT_MAX)))
        return False;
 
    if (prop_changed(pds->fg_color.changebar) && !prop_color_ok(pds->fg_color.field))
        return False;
 
    if (prop_changed(pds->bg_color.changebar) && !prop_color_ok(pds->bg_color.field))
        return False;

    if (prop_changed(pds->menuname.changebar) && !prop_obj_name_ok(pds->menuname.field,
			obj_get_module(pds->current_obj), AB_TYPE_MENU, "Menu"))
	return False;

    return True;
}

static void
turnoff_changebars(
    AB_PROP_TYPE type
)
{
    PropDrawpSettingsRec *pds = &(prop_drawp_settings_rec[type]);

    prop_set_changebar(pds->name.changebar,    	PROP_CB_OFF);
    prop_set_changebar(pds->scrolling.changebar,PROP_CB_OFF);
    prop_set_changebar(pds->size.changebar,    	PROP_CB_OFF);
    prop_set_changebar(pds->geom.changebar, 	PROP_CB_OFF);
    prop_set_changebar(pds->init_state.changebar,PROP_CB_OFF);
    prop_set_changebar(pds->frame.changebar,	PROP_CB_OFF);
    prop_set_changebar(pds->bg_color.changebar, PROP_CB_OFF);
    prop_set_changebar(pds->fg_color.changebar, PROP_CB_OFF);
    prop_set_changebar(pds->menuname.changebar, PROP_CB_OFF);
    prop_set_changebar(pds->menu_title.changebar,PROP_CB_OFF);

    prop_changebars_cleared(pds->prop_sheet);

}

static void
setup_scroll_settings(
    AB_PROP_TYPE 	type,
    AB_SCROLLBAR_POLICY	sb_policy
)
{
    PropDrawpSettingsRec *pds = &(prop_drawp_settings_rec[type]);
    BOOL		scrolling;

    scrolling = (sb_policy != AB_SCROLLBAR_NEVER);
    
    ui_set_active(pds->size.label,   scrolling);
    ui_set_active(pds->size.w_label, scrolling);
    ui_set_active(pds->size.h_label, scrolling);
    ui_set_active(pds->size.w_field, scrolling);
    ui_set_active(pds->size.h_field, scrolling);

}

static void
scroll_stateCB(
    Widget	widget,
    XtPointer	client_data,
    XtPointer   call_data
)
{
    XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *)call_data;
    AB_PROP_TYPE type = (AB_PROP_TYPE)client_data;
    XtPointer	value;

    if (state->set)
    {
	XtVaGetValues(widget, XmNuserData, &value, NULL);
	setup_scroll_settings(type, (AB_SCROLLBAR_POLICY)value);

    }
}
