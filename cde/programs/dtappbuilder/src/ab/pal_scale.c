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
 *      $XConsortium: pal_scale.c /main/5 1996/08/08 18:03:06 mustafa $
 *
 * @(#)pal_scale.c	1.26 11 Aug 1995      cde_app_builder/src/ab
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
 * pal_scale.c - Implements Palette Scale/Gauge object functionality
 */
#include <stdint.h>
#include <stdio.h>
#include <Xm/Xm.h>
#include <ab_private/ab.h>
#include <ab_private/pal.h>
#include <ab_private/prop.h>
#include <ab_private/abobj.h> 
#include <ab_private/abobj_set.h> 
#include <ab_private/ui_util.h>
#include "scale_ui.h"


typedef struct  PROP_SCALE_SETTINGS
{
    Widget                      prop_sheet;
    PropFieldSettingRec         name;
    PropRadioSettingRec         scale_type;
    PropGeometrySettingRec      geometry;
    PropRadioSettingRec         orient;
    PropOptionsSettingRec       dir;
    Widget			dir_items[4];
    PropOptionsSettingRec       label_type;
    PropOptionsSettingRec       label_pos;
    PropFieldSettingRec         label;
    PropFieldSettingRec         min;
    PropFieldSettingRec         max;
    PropFieldSettingRec         incr;
    PropFieldSettingRec         decimal;
    PropFieldSettingRec         ivalue;
    PropCheckboxSettingRec      show_value;
    PropCheckboxSettingRec      init_state;
    PropColorSettingRec         bg_color;
    PropColorSettingRec         fg_color;
    ABObj                       current_obj;
} PropScaleSettingsRec, *PropScaleSettings;

/*************************************************************************
**                                                                      **
**       Private Function Declarations                                  **
**                                                                      **
**************************************************************************/
/*
 * Methods
 */
static int      scale_initialize(
                    ABObj   obj
                );
static Widget   scale_prop_init(
                    Widget  parent,
		    AB_PROP_TYPE type
                );
static int	scale_prop_activate(
		    AB_PROP_TYPE type,
		    BOOL 	 active
		);
static int	scale_prop_clear(
		    AB_PROP_TYPE type
		);
static int      scale_prop_load(
                    ABObj   	 obj,
		    AB_PROP_TYPE type,
		    unsigned long loadkey
                );
static int	scale_prop_apply(
		    AB_PROP_TYPE type
		);
static BOOL	scale_prop_pending(
		    AB_PROP_TYPE type
		);

/*
 * General routines
 */
static BOOL	verify_props(
		    AB_PROP_TYPE type
		);
static void	turnoff_changebars(
		    AB_PROP_TYPE type
		);
static void	setup_direction_setting(
		    AB_PROP_TYPE type,
		    AB_ORIENTATION orient
		);

/*
 * Xt Callbacks
 */
static void     orientCB(
                    Widget   widget,
                    XtPointer clientdata,
                    XmToggleButtonCallbackStruct *state
                );

/*************************************************************************
**                                                                      **
**       Data  		                                                 **
**                                                                      **
**************************************************************************/
PalItemInfo scale_palitem_rec = {

    /* type             */  AB_TYPE_SCALE,
    /* name             */  "Scale",
    /* animation pixmaps*/  NULL,
    /* number of pixmaps*/  0,
    /* rev_prop_frame   */  NULL,
    /* fix_prop_dialog  */  NULL,
    /* initialize       */  scale_initialize,
    /* is_a_test        */  obj_is_scale,
    /* prop_initialize  */  scale_prop_init,
    /* prop_active 	*/  scale_prop_activate,
    /* prop_clear	*/  scale_prop_clear,
    /* prop_load        */  scale_prop_load,
    /* prop_apply	*/  scale_prop_apply,
    /* prop_pending	*/  scale_prop_pending
};

PalItemInfo *ab_scale_palitem = &scale_palitem_rec;
PropScaleSettingsRec prop_scale_settings_rec[AB_PROP_TYPE_NUM_VALUES];


/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/
static int
scale_initialize(
    ABObj    obj
)
{
    if (obj_get_read_only(obj) == False)
    {
        obj_set_unique_name(obj, "scale");
        obj_set_label(obj, catgets(Dtb_project_catd, 100, 260, "Scale:"));
    }
    else /* Gauge */
    {
    	obj_set_unique_name(obj, "gauge");
        obj_set_label(obj, catgets(Dtb_project_catd, 100, 261, "Gauge:"));
    }
    obj_set_label_position(obj, AB_CP_NORTH);

    obj_set_orientation(obj, AB_ORIENT_HORIZONTAL);
    obj_set_direction(obj, AB_DIR_LEFT_TO_RIGHT);
    obj_set_min_value(obj, 0);
    obj_set_max_value(obj, 100);
    obj_set_initial_value_int(obj, 50);
    obj_set_increment(obj, 1);

    obj_set_is_initially_visible(obj, True);
    obj_set_is_initially_active(obj, True);

    /* REMIND:  Hack to get around Motif XmNtitleString bug (when set to NULL) */
    obj_set_width(obj, 90);
    obj_set_height(obj, 18);

    obj_set_attachment(obj, AB_CP_NORTH, AB_ATTACH_POINT, NULL, obj->y);
    obj_set_attachment(obj, AB_CP_WEST,  AB_ATTACH_POINT, NULL, obj->x);

    return OK;

}

static Widget
scale_prop_init(
    Widget    parent,
    AB_PROP_TYPE type
)
{
    DtbScalePropDialogInfoRec	rev_scale_prop_dialog; /* Revolving Props */
    DtbScalePropDialogInfo     	cgen = &dtb_scale_prop_dialog; /* Codegen structure */
    DtbRevolvPropDialogInfo	rpd = &(dtb_revolv_prop_dialog);
    PropScaleSettingsRec    	*pss = &(prop_scale_settings_rec[type]);
    Widget            		item[6];
    int            		item_val[6];
    int            		n;
    int				i;

    if (type == AB_PROP_REVOLVING)
    {
	/* Cloning Trick:
	 * Only the Attributes ControlPanel needs to be created within
	 * the existing Revolving Prop dialog, so fill out all other
	 * fields with the Revolving Prop dialog equivelents, so the
	 * dtb initialize proc will skip those non-NULL fields...
	 */
        dtbScalePropDialogInfo_clear(&rev_scale_prop_dialog);

	cgen = &(rev_scale_prop_dialog);
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
    	cgen = &dtb_scale_prop_dialog; 


    if (dtb_scale_prop_dialog_initialize(cgen, parent) == 0)
    {
        pss->prop_sheet = cgen->attrs_ctrlpanel;
        pss->current_obj = NULL;

	if (type == AB_PROP_REVOLVING)
        	XtVaSetValues(parent,
                        XmNuserData, pss->current_obj,
                        NULL);

        /* Dialog/Object List */
	if (type == AB_PROP_FIXED)
	{
            prop_fixed_dialog_init(ab_scale_palitem,
			cgen->prop_dialog_shellform, cgen->objlist);
            prop_activate_panel_init(type, ab_scale_palitem, 
                        cgen->ok_button, cgen->apply_button, 
                        cgen->reset_button, cgen->cancel_button,
                        cgen->help_button);
	}

        /* Alternate Editor Buttons */
        prop_editors_panel_init(type, ab_scale_palitem,
            cgen->attach_button, cgen->conn_button, cgen->helptxt_button);

        /*
          * Prop Sheet Settings....
         */

        /* Name */
        prop_field_init(&(pss->name), cgen->name_field_label,
                cgen->name_field, cgen->name_cb);

        /* Label, Type, Position */
        n = 0;
        item[n] = cgen->labeltype_opmenu_items.String_item;
        item_val[n] = AB_LABEL_STRING; n++;
        item[n] = cgen->labeltype_opmenu_items.Graphic_item;
        item_val[n] = AB_LABEL_GLYPH; n++;
        prop_options_init(&(pss->label_type), cgen->labeltype_opmenu_label,
                        cgen->labeltype_opmenu,
                        cgen->labeltype_opmenu_menu,
                        n, item, (XtPointer*)item_val,
                        cgen->labeltype_cb);
 
        prop_field_init(&(pss->label), cgen->label_field_label, 
                cgen->label_field, cgen->label_cb); 
 
        prop_label_field_init(&(pss->label), cgen->graphic_hint, item, n); 
 
        n = 0;
        item[n] = cgen->labelpos_opmenu_items.Left_item;
        item_val[n] = AB_CP_WEST; n++;
        item[n] = cgen->labelpos_opmenu_items.Above_item;
        item_val[n] = AB_CP_NORTH; n++;
        prop_options_init(&(pss->label_pos), cgen->labelpos_opmenu_label,
                        cgen->labelpos_opmenu,
                        cgen->labelpos_opmenu_menu,
                        n, item, (XtPointer*)item_val,
                        cgen->labeltype_cb);

        /* Scale Type */
        n = 0;
        item[n] = cgen->scaletype_rbox_items.Scale_item;
        item_val[n] = False; n++;
        item[n] = cgen->scaletype_rbox_items.Gauge_item;
        item_val[n] = True; n++;
        prop_radiobox_init(&(pss->scale_type), cgen->scaletype_rbox_label,
                                cgen->scaletype_rbox, n, item, (XtPointer*)item_val,
                                cgen->scaletype_cb);

        /* Geometry */
        prop_geomfield_init(&(pss->geometry), cgen->geom_label,
                cgen->x_field_label, cgen->x_field,
                cgen->y_field_label, cgen->y_field,
                cgen->width_field_label, cgen->width_field,
                cgen->height_field_label, cgen->height_field,
                cgen->geom_cb);

        /* Orientation */
        n = 0;
        item[n] = cgen->orient_rbox_items.Horizontal_item;
        item_val[n] = AB_ORIENT_HORIZONTAL; n++;
        item[n] = cgen->orient_rbox_items.Vertical_item;
        item_val[n] = AB_ORIENT_VERTICAL; n++;
        prop_radiobox_init(&(pss->orient), cgen->orient_rbox_label,
                                cgen->orient_rbox, n, item, (XtPointer*)item_val,
                                cgen->orient_cb);

        for(i=0; i < n; i++)
            XtAddCallback(item[i], XmNvalueChangedCallback,
                        (XtCallbackProc)orientCB, (XtPointer)type);

        /* Direction */
        n = 0;
        item[n] = cgen->dir_opmenu_items.Left_to_Right_item;
        item_val[n] = AB_DIR_LEFT_TO_RIGHT; n++;
        item[n] = cgen->dir_opmenu_items.Right_to_Left_item;
        item_val[n] = AB_DIR_RIGHT_TO_LEFT; n++;
        item[n] = cgen->dir_opmenu_items.Top_to_Bottom_item;
        item_val[n] = AB_DIR_TOP_TO_BOTTOM; n++;
        item[n] = cgen->dir_opmenu_items.Bottom_to_Top_item;
        item_val[n] = AB_DIR_BOTTOM_TO_TOP; n++;
        prop_options_init(&(pss->dir), cgen->dir_opmenu_label,
                cgen->dir_opmenu, cgen->dir_opmenu_menu,
                n, item, (XtPointer*)item_val,
                cgen->dir_cb);

	for (i=0; i < n; i++)
	    pss->dir_items[i] = item[i];

        /* Min & Max & Increment */
        prop_field_init(&(pss->min), cgen->min_field_label,
                cgen->min_field, cgen->valrange_cb);
        prop_field_init(&(pss->max), cgen->max_field_label,
                cgen->max_field, cgen->valrange_cb);
	prop_field_init(&(pss->incr), cgen->incr_field_label,
		cgen->incr_field, cgen->valrange_cb);

	/* Decimal Points */
	prop_field_init(&(pss->decimal), cgen->decimal_field_label,
		cgen->decimal_field, cgen->dec_cb);

	/* Initial Value */
        prop_field_init(&(pss->ivalue), cgen->ivalue_field_label, 
                cgen->ivalue_field, cgen->ivalue_cb);
        n = 0;
        item[n] = cgen->showval_ckbox_items.Show_Value_item;
        item_val[n] = SHOW_VALUE_KEY; n++;
        prop_checkbox_init(&(pss->show_value), NULL,
                cgen->showval_ckbox, n, item, item_val,
                cgen->ivalue_cb);

        /* Initial State */
        n = 0;
        item[n] = cgen->istate_ckbox_items.Visible_item;
        item_val[n] = AB_STATE_VISIBLE; n++;
        item[n] = cgen->istate_ckbox_items.Active_item;
        item_val[n] = AB_STATE_ACTIVE; n++;
        prop_checkbox_init(&(pss->init_state), cgen->istate_ckbox_label,
                cgen->istate_ckbox, n, item, item_val,
                cgen->istate_cb);

        /* Color */
        prop_colorfield_init(&(pss->bg_color), cgen->bg_mbutton, 
		cgen->bg_mbutton_bg_mbutton_menu_items.None_item,
		cgen->bg_mbutton_bg_mbutton_menu_items.Color_Chooser_item,
                cgen->bg_swatch, cgen->bg_field, cgen->bg_cb);

        prop_colorfield_init(&(pss->fg_color), cgen->fg_mbutton,
                cgen->fg_mbutton_fg_mbutton_menu_items.None_item,  
                cgen->fg_mbutton_fg_mbutton_menu_items.Color_Chooser_item, 
                cgen->fg_swatch, cgen->fg_field, cgen->fg_cb); 

        prop_changebars_cleared(pss->prop_sheet);

	return (cgen->prop_dialog_shellform);
    }
    else
    	return NULL;

}

static int
scale_prop_activate(
    AB_PROP_TYPE type,
    BOOL	 active
)
{
    ui_set_active(prop_scale_settings_rec[type].prop_sheet, active);

    return OK;
}


static int
scale_prop_clear(
    AB_PROP_TYPE type
)
{
    PropScaleSettingsRec	*pss = &(prop_scale_settings_rec[type]);

    if (pss->current_obj == NULL)
	return OK;

    /* Clear Name */
    prop_field_set_value(&(pss->name), "", False);

    /* Clear Label Type/Position */
    prop_options_set_value(&(pss->label_type), (XtPointer)AB_LABEL_STRING, False);
    prop_options_set_value(&(pss->label_pos), (XtPointer)AB_CP_WEST, False);

    /* Clear Label */
    ui_set_label_string(pss->label.label, "Label:");
    prop_field_set_value(&(pss->label), "", False);

    /* Clear Scale Type*/ 
    prop_radiobox_set_value(&(pss->scale_type), (XtPointer)False, False); 

    /* Clear Orientation */
    prop_radiobox_set_value(&(pss->orient),
                            (XtPointer)AB_ORIENT_HORIZONTAL, False);

    /* Clear Direction */
    prop_options_set_value(&(pss->dir), (XtPointer)AB_DIR_LEFT_TO_RIGHT, False);
 
    /* Clear Value Range */
    prop_field_set_value(&(pss->min), "", False);
    prop_field_set_value(&(pss->max), "", False);
    prop_field_set_value(&(pss->incr), "", False);

    /* Clear Decimal Points */
    prop_field_set_value(&(pss->decimal), "", False);

    /* Clear Initial Value/Show Value */ 
    prop_field_set_value(&(pss->ivalue), "", False); 
    prop_checkbox_set_value(&(pss->show_value), True, False, False);

    /* Clear Geometry */
    prop_geomfield_clear(&(pss->geometry), GEOM_X);
    prop_geomfield_clear(&(pss->geometry), GEOM_Y);
    prop_geomfield_clear(&(pss->geometry), GEOM_WIDTH);
    prop_geomfield_clear(&(pss->geometry), GEOM_HEIGHT);
 
    /* Clear Initial State */
    prop_checkbox_set_value(&(pss->init_state), AB_STATE_VISIBLE, True, False);
    prop_checkbox_set_value(&(pss->init_state), AB_STATE_ACTIVE, True, False);

    /* Clear Color */
    prop_colorfield_set_value(&(pss->bg_color), "", False);
    prop_colorfield_set_value(&(pss->fg_color), "", False);

    pss->current_obj = NULL;

    turnoff_changebars(type);

    return OK;
}

static int
scale_prop_load(
    ABObjPtr 	 obj,
    AB_PROP_TYPE type,
    unsigned long loadkey
)
{
    PropScaleSettingsRec 	*pss = &(prop_scale_settings_rec[type]);
    AB_ORIENTATION		orient;
    BOOL			load_all = (loadkey & LoadAll);
 
    if (obj == NULL)
    {
        if (pss->current_obj != NULL)
            obj = pss->current_obj;
        else
            return ERROR;
    }
    else if (!obj_is_scale(obj))
        return ERROR;
    else
        pss->current_obj = obj;

    /* Load Name */
    if (load_all || loadkey & LoadName)
    	prop_field_set_value(&(pss->name), obj_get_name(obj), False);

    if (load_all)
    {
	/* Load Label Type/Position */
	prop_options_set_value(&(pss->label_type), (XtPointer)obj->label_type, False);
	prop_options_set_value(&(pss->label_pos), (XtPointer)obj_get_label_position(obj), False);

	/* Load Label */
	prop_setup_label_field(&(pss->label), NULL,
				obj->label_type, obj_get_label(obj), AB_LINE_UNDEF);

	/* Load Scale Type */
	prop_radiobox_set_value(&(pss->scale_type),
                                (XtPointer)(intptr_t) obj_get_read_only(obj), False);

	/* Load Orientation */
	orient = obj_get_orientation(obj);
	prop_radiobox_set_value(&(pss->orient), (XtPointer)orient, False); 
	setup_direction_setting(type, orient);

	/* Load Direction */
	prop_options_set_value(&(pss->dir), (XtPointer)obj_get_direction(obj), False);

	/* Load Value Range */
	prop_field_set_numeric_value(&(pss->min), obj_get_min_value(obj), False);
	prop_field_set_numeric_value(&(pss->max), obj_get_max_value(obj), False);
	prop_field_set_numeric_value(&(pss->incr), obj_get_increment(obj), False);

	/* Load Decimal Points */
	prop_field_set_numeric_value(&(pss->decimal), 
		obj_get_decimal_points(obj), False);

	/* Load Initial Value/Show Value */
	prop_field_set_numeric_value(&(pss->ivalue), 
		obj_get_initial_value_int(obj), False);
	prop_checkbox_set_value(&(pss->show_value), SHOW_VALUE_KEY,
		obj_get_show_value(obj), False);

	/* Load Initial State */
	prop_checkbox_set_value(&(pss->init_state), AB_STATE_VISIBLE,
		obj_is_initially_visible(obj), False);
    	prop_checkbox_set_value(&(pss->init_state), AB_STATE_ACTIVE,
		obj_is_initially_active(obj), False);

    	/* Load Color */
    	prop_colorfield_set_value(&(pss->bg_color), obj_get_bg_color(obj), False); 
    	prop_colorfield_set_value(&(pss->fg_color), obj_get_fg_color(obj), False);
 
    	turnoff_changebars(type);
    }

    /* Load Geometry */
    if (load_all || loadkey & LoadPosition)
        prop_load_obj_position(obj, &(pss->geometry)); 

    if (load_all || loadkey & LoadSize)
        prop_load_obj_size(obj, &(pss->geometry)); 

    return OK;
}

int
scale_prop_apply(
    AB_PROP_TYPE   type
)
{
    PropScaleSettingsRec 	*pss = &(prop_scale_settings_rec[type]);
    STRING	    		value;
    int				new_w, new_h;
    BOOL	    		size_chg = False;
    BOOL			reset_bg = False;
    BOOL			reset_fg = False;

    if (!verify_props(type))
        return ERROR;

    if (prop_changed(pss->name.changebar))
    {
        value = prop_field_get_value(&(pss->name));     
        abobj_set_name(pss->current_obj, value);
        util_free(value);
    }
    if (prop_changed(pss->label.changebar) || 
	prop_changed(pss->label_type.changebar))
    {
        value = prop_field_get_value(&(pss->label));
        abobj_set_label(pss->current_obj,
            (AB_LABEL_TYPE)prop_options_get_value(&(pss->label_type)),
                        value);
        util_free(value);

        abobj_set_label_position(pss->current_obj,
                (AB_COMPASS_POINT)prop_options_get_value(&(pss->label_pos)));
        size_chg = True;
    }
    if (prop_changed(pss->scale_type.changebar))
    {
        abobj_set_read_only(pss->current_obj,
            (BOOL)prop_radiobox_get_value(&(pss->scale_type)));
        size_chg = True;
    }   
    if (prop_changed(pss->orient.changebar))
    {
        abobj_set_orientation(pss->current_obj,
            (AB_ORIENTATION)prop_radiobox_get_value(&(pss->orient)));
    }
    if (prop_changed(pss->dir.changebar))
    {
	abobj_set_direction(pss->current_obj,
		(AB_DIRECTION)prop_options_get_value(&(pss->dir)));
    }
    if (prop_changed(pss->min.changebar))
    {
        abobj_set_min_max_values(pss->current_obj, 
		prop_field_get_numeric_value(&(pss->min)),
		prop_field_get_numeric_value(&(pss->max)));
        abobj_set_increment(pss->current_obj,
                prop_field_get_numeric_value(&(pss->incr)));
    }   
    if (prop_changed(pss->decimal.changebar))
    {
	abobj_set_decimal_points(pss->current_obj,
		prop_field_get_numeric_value(&(pss->decimal)));
    }
    if (prop_changed(pss->ivalue.changebar))
    {
	abobj_set_initial_value(pss->current_obj, NULL,
		prop_field_get_numeric_value(&(pss->ivalue)));
	abobj_set_show_value(pss->current_obj,
	        prop_checkbox_get_value(&(pss->show_value), SHOW_VALUE_KEY));

    }
    if (prop_changed(pss->geometry.changebar))
    {
	if (abobj_width_resizable(pss->current_obj))
	{
	    new_w = prop_geomfield_get_value(&(pss->geometry), GEOM_WIDTH);
	    abobj_set_pixel_width(pss->current_obj, new_w, 0);
	}
        if (abobj_height_resizable(pss->current_obj))
        {
	    new_h = prop_geomfield_get_value(&(pss->geometry), GEOM_HEIGHT);
	    abobj_set_pixel_height(pss->current_obj, new_h, 0);
	}
	if (abobj_is_movable(pss->current_obj))
	    abobj_set_xy(pss->current_obj,
	     	prop_geomfield_get_value(&(pss->geometry), GEOM_X),
	     	prop_geomfield_get_value(&(pss->geometry), GEOM_Y));
	size_chg = True;

    }
    if (prop_changed(pss->init_state.changebar))
    {
        abobj_set_visible(pss->current_obj, 
		prop_checkbox_get_value(&(pss->init_state), AB_STATE_VISIBLE));
        abobj_set_active(pss->current_obj,
		prop_checkbox_get_value(&(pss->init_state), AB_STATE_ACTIVE));
    }
    if (prop_changed(pss->fg_color.changebar))
    {
	value = prop_colorfield_get_value(&(pss->fg_color));
        abobj_set_foreground_color(pss->current_obj, value);
	if (util_strempty(value))
	    reset_fg = True;
        util_free(value);
        prop_colorfield_set_value(&(pss->fg_color), 
		obj_get_fg_color(pss->current_obj), False);
    }
    if (prop_changed(pss->bg_color.changebar))
    {
	value = prop_colorfield_get_value(&(pss->bg_color));
        abobj_set_background_color(pss->current_obj, value);
	if (util_strempty(value))
	    reset_bg = True;
        util_free(value);
        prop_colorfield_set_value(&(pss->bg_color), 
		obj_get_bg_color(pss->current_obj), False);
    }

    abobj_instantiate_changes(pss->current_obj);

    if (reset_bg || reset_fg) /* Set back to No Color */ 
        abobj_reset_colors(pss->current_obj, reset_bg, reset_fg); 
    if (size_chg)
	abobj_force_dang_form_resize(pss->current_obj);
 
    turnoff_changebars(type);

    return OK;
}

static BOOL
scale_prop_pending(
    AB_PROP_TYPE type
)
{
    return(prop_changebars_pending(prop_scale_settings_rec[type].prop_sheet));
}

static BOOL
verify_props(
    AB_PROP_TYPE type
)
{
    PropScaleSettingsRec *pss = &(prop_scale_settings_rec[type]);
    int			  min, max;

    if (prop_changed(pss->name.changebar) && 
	!prop_name_ok(pss->current_obj, pss->name.field))
        return False;

    if ((prop_changed(pss->label_type.changebar) || prop_changed(pss->label.changebar)) &&
	prop_options_get_value(&(pss->label_type)) == (XtPointer)AB_LABEL_GLYPH &&
        !prop_graphic_filename_ok(pss->label.field, False))
        return False;

    if (!prop_number_ok(pss->min.field, "Min Field", INT_MIN, INT_MAX))
        return False;

    min = prop_field_get_numeric_value(&(pss->min));
    if (!prop_number_ok(pss->max.field, "Max Field", min+1, INT_MAX))
        return False;
 
    max = prop_field_get_numeric_value(&(pss->max));
    if (!prop_number_ok(pss->incr.field, "Increment Field", 1, max - min))
        return False;
 
    if (prop_changed(pss->ivalue.changebar) &&
        !prop_number_ok(pss->ivalue.field, "Initial Value Field", min, max))
	return False;

    if (prop_changed(pss->decimal.changebar) &&
         !prop_number_ok(pss->decimal.field, "Decimal Field", 0, 99))
        return False;
         
    if (prop_changed(pss->geometry.changebar) &&
        (!prop_number_ok(pss->geometry.x_field,(STRING)XFieldStr, -SHRT_MAX, SHRT_MAX) ||
         !prop_number_ok(pss->geometry.y_field,(STRING)YFieldStr, -SHRT_MAX, SHRT_MAX) ||
         !prop_number_ok(pss->geometry.h_field,(STRING)HFieldStr, 1, SHRT_MAX) ||
         !prop_number_ok(pss->geometry.w_field,(STRING)WFieldStr, 1, SHRT_MAX)))
        return False;

    if (prop_changed(pss->fg_color.changebar) && !prop_color_ok(pss->fg_color.field))
        return False;
 
    if (prop_changed(pss->bg_color.changebar) && !prop_color_ok(pss->bg_color.field))
        return False;

    return True;
}

static void
turnoff_changebars(
    AB_PROP_TYPE type
)
{
    PropScaleSettingsRec *pss = &(prop_scale_settings_rec[type]);

    prop_set_changebar(pss->name.changebar,    		PROP_CB_OFF);
    prop_set_changebar(pss->scale_type.changebar,	PROP_CB_OFF);
    prop_set_changebar(pss->orient.changebar,		PROP_CB_OFF);
    prop_set_changebar(pss->dir.changebar,		PROP_CB_OFF);
    prop_set_changebar(pss->label_type.changebar,    	PROP_CB_OFF);
    prop_set_changebar(pss->label.changebar,    	PROP_CB_OFF);
    prop_set_changebar(pss->min.changebar,		PROP_CB_OFF);
    prop_set_changebar(pss->decimal.changebar,		PROP_CB_OFF);
    prop_set_changebar(pss->geometry.changebar,		PROP_CB_OFF);
    prop_set_changebar(pss->ivalue.changebar,		PROP_CB_OFF);
    prop_set_changebar(pss->init_state.changebar,	PROP_CB_OFF);
    prop_set_changebar(pss->bg_color.changebar,  	PROP_CB_OFF);
    prop_set_changebar(pss->fg_color.changebar,  	PROP_CB_OFF);

    prop_changebars_cleared(pss->prop_sheet);

}

static void
orientCB(
    Widget   	item,
    XtPointer 	clientdata,
    XmToggleButtonCallbackStruct *state
)
{
    AB_PROP_TYPE	type = (AB_PROP_TYPE)clientdata;
    PropScaleSettingsRec *pss = &(prop_scale_settings_rec[type]);
    int    		value;

    if (state->set)
    {
        XtVaGetValues(item, XmNuserData, &value, NULL); 
	setup_direction_setting(type, (AB_ORIENTATION)value);

	if (value == AB_ORIENT_HORIZONTAL)
	    prop_options_set_value(&(pss->dir), 
		(XtPointer)AB_DIR_LEFT_TO_RIGHT, True);
	else
	    prop_options_set_value(&(pss->dir),
		(XtPointer)AB_DIR_BOTTOM_TO_TOP, True);
    }
}

static void
setup_direction_setting(
    AB_PROP_TYPE	type,
    AB_ORIENTATION	orient
)
{
    PropScaleSettingsRec *pss = &(prop_scale_settings_rec[type]);

    ui_set_active(pss->dir_items[0], 
		orient == AB_ORIENT_HORIZONTAL? True : False);
    ui_set_active(pss->dir_items[1], 
		orient == AB_ORIENT_HORIZONTAL? True : False);
    ui_set_active(pss->dir_items[2], 
		orient == AB_ORIENT_VERTICAL? True : False);
    ui_set_active(pss->dir_items[3], 
		orient == AB_ORIENT_VERTICAL? True : False);
}
