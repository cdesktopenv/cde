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
 *	$XConsortium: pal_sep.c /main/5 1996/08/08 18:03:31 mustafa $
 *
 *	@(#)pal_separator.c	1.6 22 Feb 1994
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
 * pal_separator.c - Implements Palette Separator object functionality
 */
#include <stdio.h>
#include <Xm/Xm.h>
#include <ab_private/pal.h>
#include <ab_private/ab.h>
#include <ab_private/prop.h>
#include <ab_private/abobj.h>
#include <ab_private/abobj_set.h>
#include <ab_private/x_util.h>
#include <ab_private/ui_util.h>
#include "sep_ui.h"

const    int    separator_init_width  = 130;
const    int    separator_init_height = 10;


typedef struct  PROP_SEP_SETTINGS
{
    Widget                      prop_sheet;
    PropFieldSettingRec         name;
    PropGeometrySettingRec      geometry;
    PropOptionsSettingRec       line_style;
    PropRadioSettingRec         orient;
    PropCheckboxSettingRec      init_state;
    PropColorSettingRec         bg_color;
    PropColorSettingRec         fg_color;
    ABObj                       current_obj;
} PropSepSettingsRec, *PropSepSettings;

/*************************************************************************
**                                                                      **
**       Private Function Declarations                                  **
**                                                                      **
**************************************************************************/
/*
 * Methods
 */
static int      separator_initialize(
                    ABObj   obj
                );
static Widget   separator_prop_init(
                    Widget  parent,
                    AB_PROP_TYPE type
                );
static int	separator_prop_activate(
		    AB_PROP_TYPE type,
		    BOOL 	 active
		);
static int	separator_prop_clear(
		    AB_PROP_TYPE type
		);
static int      separator_prop_load(
                    ABObj   	 obj,
                    AB_PROP_TYPE type,
		    unsigned long loadkey
                );
static int	separator_prop_apply(
		    AB_PROP_TYPE type
		);
static BOOL     separator_prop_pending(
                    AB_PROP_TYPE type
                );

static BOOL	verify_props(
		    AB_PROP_TYPE type
		);
static void	turnoff_changebars(
		    AB_PROP_TYPE type
		);

/*
 * Callbacks
 */

/*************************************************************************
**                                                                      **
**       Data  		                                                 **
**                                                                      **
**************************************************************************/
PalItemInfo separator_palitem_rec = {

    /* type             */  AB_TYPE_SEPARATOR,
    /* name             */  "Separator",
    /* animation pixmaps*/  NULL,
    /* number of pixmaps*/  0,
    /* rev_prop_frame   */  NULL,
    /* fix_prop_dialog  */  NULL,
    /* initialize       */  separator_initialize,
    /* is_a_test        */  obj_is_separator,
    /* prop_initialize  */  separator_prop_init,
    /* prop_activate	*/  separator_prop_activate,
    /* prop_clear	*/  separator_prop_clear,
    /* prop_load        */  separator_prop_load,
    /* prop_apply	*/  separator_prop_apply,
    /* prop_pending	*/  separator_prop_pending

};

PalItemInfo *ab_separator_palitem = &separator_palitem_rec;
PropSepSettingsRec prop_sep_settings_rec[AB_PROP_TYPE_NUM_VALUES];


/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/
static int
separator_initialize(
    ABObj    obj
)
{
    obj_set_unique_name(obj, "separator");

    obj_set_label(obj, catgets(Dtb_project_catd, 22, 46, "Separator"));
    obj_set_line_style(obj, AB_LINE_ETCHED_IN);
    obj_set_is_initially_visible(obj, True);
    obj_set_is_initially_active(obj, True);

    obj_set_width(obj, separator_init_width);
    obj_set_height(obj, separator_init_height);

    obj_set_attachment(obj, AB_CP_NORTH, AB_ATTACH_POINT, NULL, obj->y);
    obj_set_attachment(obj, AB_CP_WEST,  AB_ATTACH_POINT, NULL, obj->x);

    return OK;

}

static Widget
separator_prop_init(
    Widget parent,
    AB_PROP_TYPE type
)
{
    DtbSepPropDialogInfoRec	rev_sep_prop_dialog; /* Revolving Props */
    DtbSepPropDialogInfo     	cgen = &dtb_sep_prop_dialog; /* Codegen structure */
    DtbRevolvPropDialogInfo	rpd = &(dtb_revolv_prop_dialog);
    PropSepSettingsRec    	*pss = &(prop_sep_settings_rec[type]);
    Widget            		item[14];
    int            		item_val[14];
    int            		n;

    if (type == AB_PROP_REVOLVING)
    {
	/* Cloning Trick:
	 * Only the Attributes ControlPanel needs to be created within
	 * the existing Revolving Prop dialog, so fill out all other
	 * fields with the Revolving Prop dialog equivelents, so the
	 * dtb initialize proc will skip those non-NULL fields...
	 */
        dtbSepPropDialogInfo_clear(&rev_sep_prop_dialog);

	cgen = &(rev_sep_prop_dialog);
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
    	cgen = &dtb_sep_prop_dialog;

    if (dtb_sep_prop_dialog_initialize(cgen, parent) == 0)
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
            prop_fixed_dialog_init(ab_separator_palitem,
			cgen->prop_dialog_shellform, cgen->objlist);
            prop_activate_panel_init(type, ab_separator_palitem,
                        cgen->ok_button, cgen->apply_button,
                        cgen->reset_button, cgen->cancel_button,
                        cgen->help_button);
	}

        /* Alternate Editor Buttons */
        prop_editors_panel_init(type, ab_separator_palitem,
            cgen->attach_button, cgen->conn_button, cgen->helptxt_button);

        /*
          * Prop Sheet Settings....
         */

        /* Name */
        prop_field_init(&(pss->name), cgen->name_field_label,
                cgen->name_field, cgen->name_cb);

        /* Orientation */
        n = 0;
        item[n] = cgen->orient_rbox_items.Horizontal_item;
        item_val[n] = AB_ORIENT_HORIZONTAL; n++;
        item[n] = cgen->orient_rbox_items.Vertical_item;
        item_val[n] = AB_ORIENT_VERTICAL; n++;
        prop_radiobox_init(&(pss->orient), cgen->orient_rbox_label,
                                cgen->orient_rbox, n, item, (XtPointer*)item_val,
                                cgen->orient_cb);

        /* Line Style */
        n = 0;
        item[n] = cgen->linestyle_opmenu_items.None_item;
        item_val[n] = AB_LINE_NONE; n++;
        item[n] = cgen->linestyle_opmenu_items.Etched_In_item;
        item_val[n] = AB_LINE_ETCHED_IN; n++;
        item[n] = cgen->linestyle_opmenu_items.Etched_Out_item;
        item_val[n] = AB_LINE_ETCHED_OUT; n++;
        item[n] = cgen->linestyle_opmenu_items.Etched_In_Dash_item;
        item_val[n] = AB_LINE_ETCHED_IN_DASH; n++;
        item[n] = cgen->linestyle_opmenu_items.Etched_Out_Dash_item;
        item_val[n] = AB_LINE_ETCHED_OUT_DASH; n++;
        item[n] = cgen->linestyle_opmenu_items.Single_Line_item;
        item_val[n] = AB_LINE_SINGLE_LINE; n++;
        item[n] = cgen->linestyle_opmenu_items.Double_Line_item;
        item_val[n] = AB_LINE_DOUBLE_LINE; n++;
        item[n] = cgen->linestyle_opmenu_items.Single_Dashed_Line_item;
        item_val[n] = AB_LINE_SINGLE_DASHED_LINE; n++;
        item[n] = cgen->linestyle_opmenu_items.Double_Dashed_Line_item;
        item_val[n] = AB_LINE_DOUBLE_DASHED_LINE; n++;
        prop_options_init(&(pss->line_style), cgen->linestyle_opmenu_label,
                cgen->linestyle_opmenu, cgen->linestyle_opmenu_menu,
                n, item, (XtPointer*)item_val,
                cgen->linestyle_cb);

        /* Geometry */
        prop_geomfield_init(&(pss->geometry), cgen->geom_label,
                cgen->x_field_label, cgen->x_field,
                cgen->y_field_label, cgen->y_field,
                cgen->width_field_label, cgen->width_field,
                cgen->height_field_label, cgen->height_field,
                cgen->geom_cb);

        /* Initial State */
        n = 0;
        item[n] = cgen->istate_ckbox_items.Visible_item;
        item_val[n] = AB_STATE_VISIBLE; n++;
        prop_checkbox_init(&(pss->init_state), cgen->istate_ckbox_label,
                cgen->istate_ckbox, n, item, item_val,
                cgen->istate_cb);

        /* Background */
        prop_colorfield_init(&(pss->bg_color), cgen->bg_mbutton,
		cgen->bg_mbutton_bg_mbutton_menu_items.None_item,
		cgen->bg_mbutton_bg_mbutton_menu_items.Color_Chooser_item,
                cgen->bg_swatch, cgen->bg_field, cgen->bg_cb);

        /* Foreground */
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
separator_prop_activate(
    AB_PROP_TYPE type,
    BOOL	 active
)
{
    ui_set_active(prop_sep_settings_rec[type].prop_sheet, active);

    return OK;
}


static int
separator_prop_clear(
    AB_PROP_TYPE type
)
{
    PropSepSettingsRec	*pss = &(prop_sep_settings_rec[type]);

    if (pss->current_obj == NULL)
	return OK;

    /* Clear Name */
    prop_field_set_value(&(pss->name), "", False);

    /* Clear Orientation */
    prop_radiobox_set_value(&(pss->orient),
                            (XtPointer)AB_ORIENT_HORIZONTAL, False);

    /* Clear Line Style */
    prop_options_set_value(&(pss->line_style), (XtPointer)AB_LINE_ETCHED_OUT, False);

    /* Clear Geometry */
    prop_geomfield_clear(&(pss->geometry), GEOM_X);
    prop_geomfield_clear(&(pss->geometry), GEOM_Y);
    prop_geomfield_clear(&(pss->geometry), GEOM_WIDTH);
    prop_geomfield_clear(&(pss->geometry), GEOM_HEIGHT);

    /* Clear Initial State */
    prop_checkbox_set_value(&(pss->init_state), AB_STATE_VISIBLE, True, False);

    /* Clear Background Color */
    prop_colorfield_set_value(&(pss->bg_color), "", False);

    /* Clear Foreground Color */
    prop_colorfield_set_value(&(pss->fg_color), "", False);

    pss->current_obj = NULL;

    turnoff_changebars(type);

    return OK;

}

static int
separator_prop_load(
    ABObjPtr 	 obj,
    AB_PROP_TYPE type,
    unsigned long loadkey
)
{
    PropSepSettingsRec *pss = &(prop_sep_settings_rec[type]);
    BOOL		load_all = (loadkey & LoadAll);

    if (obj == NULL)
    {
        if (pss->current_obj != NULL)
            obj = pss->current_obj;
        else
            return ERROR;
    }
    else if (!obj_is_separator(obj))
        return ERROR;
    else
        pss->current_obj = obj;

    /* Load Name */
    if (load_all || loadkey & LoadName)
    	prop_field_set_value(&(pss->name), obj_get_name(obj), False);

    if (load_all)
    {
	/* Load Orientation */
	prop_radiobox_set_value(&(pss->orient),
                                (XtPointer)obj_get_orientation(obj), False);

	/* Load Line Style */
	prop_options_set_value(&(pss->line_style), (XtPointer)obj_get_line_style(obj), False);

	/* Load Initial State */
	prop_checkbox_set_value(&(pss->init_state), AB_STATE_VISIBLE,
		obj_is_initially_visible(obj), False);

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
separator_prop_apply(
    AB_PROP_TYPE   type
)
{
    PropSepSettingsRec 	*pss = &(prop_sep_settings_rec[type]);
    STRING		value;
    AB_ORIENTATION	orient;
    BOOL		size_chg = False;
    BOOL		orient_chg = False;
    BOOL		style_chg = False;
    BOOL		reset_bg = False;
    BOOL		reset_fg = False;
    int			new_w, new_h;

    if (!verify_props(type))
        return ERROR;

    if (prop_changed(pss->name.changebar))
    {
        value = prop_field_get_value(&(pss->name));
        abobj_set_name(pss->current_obj, value);
        util_free(value);
    }
    if (prop_changed(pss->orient.changebar))
    {
        abobj_set_orientation(pss->current_obj,
		(AB_ORIENTATION)prop_radiobox_get_value(&(pss->orient)));
        orient_chg = True;
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
    }
    if (prop_changed(pss->line_style.changebar))
    {
        abobj_set_line_style(pss->current_obj,
                (AB_LINE_TYPE)prop_options_get_value(&(pss->line_style)));
	style_chg = True;
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

    if (orient_chg)
    {
	/* The x,y attributes have changed and the width/height
	 * attributes have been swapped, so update the prop fields
	 * with these new values.
	 */
	prop_geomfield_set_value(&(pss->geometry), GEOM_X,
		abobj_get_comp_x(pss->current_obj), False);
	prop_geomfield_set_value(&(pss->geometry), GEOM_Y,
		abobj_get_comp_y(pss->current_obj), False);
        prop_geomfield_set_value(&(pss->geometry), GEOM_WIDTH,
                abobj_get_comp_width(pss->current_obj), False);
        prop_geomfield_set_value(&(pss->geometry), GEOM_HEIGHT,
                abobj_get_comp_height(pss->current_obj), False);
    }
    if (style_chg)
    {
	/* Workaround for XmSeparator bug where setting its XmNseparatorType
	 * resource changes its thickness; we want to maintain the specified
	 * width/height instead
	 */
	orient = obj_get_orientation(pss->current_obj);
	if (orient == AB_ORIENT_HORIZONTAL)
	    XtVaSetValues(objxm_get_widget(pss->current_obj),
		XmNheight, prop_geomfield_get_value(&(pss->geometry), GEOM_HEIGHT),
		NULL);
	else /* AB_ORIENT_VERTICAL */
	    XtVaSetValues(objxm_get_widget(pss->current_obj),
                XmNwidth, prop_geomfield_get_value(&(pss->geometry), GEOM_WIDTH),
                NULL);
    }
    turnoff_changebars(type);

    return OK;

}

static BOOL
separator_prop_pending(
    AB_PROP_TYPE type
)
{
    return(prop_changebars_pending(prop_sep_settings_rec[type].prop_sheet));
}

static BOOL
verify_props(
    AB_PROP_TYPE type
)
{
    PropSepSettingsRec *pss = &(prop_sep_settings_rec[type]);

    if (prop_changed(pss->name.changebar) && !prop_name_ok(pss->current_obj, pss->name.field))
        return False;

    if (prop_changed(pss->geometry.changebar) &&
        (!prop_number_ok(pss->geometry.x_field, (STRING)XFieldStr, -SHRT_MAX, SHRT_MAX) ||
         !prop_number_ok(pss->geometry.y_field, (STRING)YFieldStr, -SHRT_MAX, SHRT_MAX) ||
         !prop_number_ok(pss->geometry.w_field, (STRING)WFieldStr, 1, SHRT_MAX) ||
         !prop_number_ok(pss->geometry.h_field, (STRING)HFieldStr, 1, SHRT_MAX)))
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
    PropSepSettingsRec *pss = &(prop_sep_settings_rec[type]);

    prop_set_changebar(pss->name.changebar,    	 PROP_CB_OFF);
    prop_set_changebar(pss->orient.changebar,    PROP_CB_OFF);
    prop_set_changebar(pss->line_style.changebar,PROP_CB_OFF);
    prop_set_changebar(pss->geometry.changebar,  PROP_CB_OFF);
    prop_set_changebar(pss->init_state.changebar,PROP_CB_OFF);
    prop_set_changebar(pss->bg_color.changebar,  PROP_CB_OFF);
    prop_set_changebar(pss->fg_color.changebar,  PROP_CB_OFF);

    prop_changebars_cleared(pss->prop_sheet);

}
