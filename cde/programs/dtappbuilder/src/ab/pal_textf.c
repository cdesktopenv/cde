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
 *      $XConsortium: pal_textf.c /main/5 1996/08/08 18:04:36 mustafa $
 *
 * @(#)pal_textf.c	1.62 11 Aug 1995      cde_app_builder/src/ab
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
 * pal_textf.c - Implements Palette TextField object functionality
 */
#include <stdint.h>
#include <stdio.h>
#include <Xm/Xm.h>
#include <ab_private/pal.h>
#include <ab_private/ab.h>
#include <ab_private/prop.h>
#include <ab_private/abobj.h>
#include <ab_private/abobj_set.h>
#include <ab_private/ui_util.h>
#include "textf_ui.h"

typedef struct  PROP_TEXTF_SETTINGS
{
    Widget                      prop_sheet;
    PropFieldSettingRec         name;
    PropGeometrySettingRec      pos;
    PropGeometrySettingRec      size;
    PropOptionsSettingRec       size_metric;
    PropOptionsSettingRec       label_type;
    PropFieldSettingRec         label;
    PropOptionsSettingRec       label_pos;
    PropRadioSettingRec		op;
    PropFieldSettingRec         max_chars;
    PropFieldSettingRec         init_value;
    PropCheckboxSettingRec      init_state;
    PropColorSettingRec         bg_color;
    PropColorSettingRec         fg_color;
/*
    PropMenunameSettingRec      menuname;
*/
    ABObj                       current_obj;
} PropTextfSettingsRec, *PropTextfSettings;

/*************************************************************************
**                                                                      **
**       Private Function Declarations                                  **
**                                                                      **
**************************************************************************/
/*
 * Methods
 */
static int      textf_initialize(
                    ABObj   obj
                );
static Widget   textf_prop_init(
                    Widget  parent,
		    AB_PROP_TYPE type
                );
static int	textf_prop_activate(
		    AB_PROP_TYPE type,
		    BOOL 	 active
		);
static int	textf_prop_clear(
                    AB_PROP_TYPE type
                );
static int      textf_prop_load(
                    ABObj   	 obj,
		    AB_PROP_TYPE type,
  		    unsigned long loadkey
                );
static int	textf_prop_apply(
		    AB_PROP_TYPE type
		);
static BOOL	textf_prop_pending(
		    AB_PROP_TYPE type
		);

static BOOL	verify_props(
		    AB_PROP_TYPE type
		);
static void	turnoff_changebars(
		    AB_PROP_TYPE type
		);

/*
 * Xt Callbacks
 */

/*************************************************************************
**                                                                      **
**       Data  		                                                 **
**                                                                      **
**************************************************************************/
PalItemInfo textf_palitem_rec = {

    /* type             */  AB_TYPE_TEXT_FIELD,
    /* name             */  "Text Field",
    /* animation pixmaps*/  NULL,
    /* number of pixmaps*/  0,
    /* rev_prop_frame   */  NULL,
    /* fix_prop_dialog  */  NULL,
    /* initialize       */  textf_initialize,
    /* is_a_test        */  obj_is_text_field,
    /* prop_initialize  */  textf_prop_init,
    /* prop_activate	*/  textf_prop_activate,
    /* prop_clear	*/  textf_prop_clear,
    /* prop_load        */  textf_prop_load,
    /* prop_apply	*/  textf_prop_apply,
    /* prop_pending	*/  textf_prop_pending

};

PalItemInfo *ab_textf_palitem = &textf_palitem_rec;
PropTextfSettingsRec prop_textf_settings_rec[AB_PROP_TYPE_NUM_VALUES];

/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/
static int
textf_initialize(
    ABObj    obj
)
{
    AB_TEXT_INFO *info = &(obj->info.text);

    obj_set_unique_name(obj, "textfield");

    obj_set_label(obj, catgets(Dtb_project_catd, 100, 263, "TextField:"));
    obj_set_is_initially_visible(obj, True);
    obj_set_is_initially_active(obj, True);
    obj_set_num_columns(obj, 8);
    obj_set_max_length(obj, 80);

    obj_set_attachment(obj, AB_CP_NORTH, AB_ATTACH_POINT, NULL, obj->y);
    obj_set_attachment(obj, AB_CP_WEST,  AB_ATTACH_POINT, NULL, obj->x);

    return OK;

}

static Widget
textf_prop_init(
    Widget    parent,
    AB_PROP_TYPE type
)
{
    DtbTextfPropDialogInfoRec	rev_textf_prop_dialog; /* Revolving Props */
    DtbTextfPropDialogInfo	cgen = &dtb_textf_prop_dialog; /* Codegen structure */
    DtbRevolvPropDialogInfo	rpd = &(dtb_revolv_prop_dialog);
    PropTextfSettingsRec    	*pts = &(prop_textf_settings_rec[type]);
    Widget			item[10];
    int				item_val[10];
    int				n;

    if (type == AB_PROP_REVOLVING)
    {
	/* Cloning Trick:
	 * Only the Attributes ControlPanel needs to be created within
	 * the existing Revolving Prop dialog, so fill out all other
	 * fields with the Revolving Prop dialog equivelents, so the
	 * dtb initialize proc will skip those non-NULL fields...
	 */
        dtbTextfPropDialogInfo_clear(&rev_textf_prop_dialog);

	cgen = &(rev_textf_prop_dialog);
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
    	cgen = &dtb_textf_prop_dialog;

    if (dtb_textf_prop_dialog_initialize(cgen, parent) == 0)
    {
        pts->prop_sheet = cgen->attrs_ctrlpanel;
        pts->current_obj = NULL;

	if (type == AB_PROP_REVOLVING)
        	XtVaSetValues(parent,
                        XmNuserData, pts->current_obj,
                        NULL);

        /* Dialog/Object List */
	if (type == AB_PROP_FIXED)
	{
            prop_fixed_dialog_init(ab_textf_palitem,
			cgen->prop_dialog_shellform, cgen->objlist);
            prop_activate_panel_init(type, ab_textf_palitem,
                        cgen->ok_button, cgen->apply_button,
                        cgen->reset_button, cgen->cancel_button,
                        cgen->help_button);
	}

	/* Alternate Editor Buttons */
	prop_editors_panel_init(type, ab_textf_palitem,
		    cgen->attach_button, cgen->conn_button, cgen->helptxt_button);

	/*
	  * Prop Sheet Settings....
	 */

	/* Name */
	prop_field_init(&(pts->name), cgen->name_field_label,
			    cgen->name_field, cgen->name_cb);

        /* Label, Type, Position */
        n = 0;
        item[n] = cgen->labeltype_rbox_items.String_item;
        item_val[n] = AB_LABEL_STRING; n++;
        item[n] = cgen->labeltype_rbox_items.Graphic_item;
        item_val[n] = AB_LABEL_GLYPH; n++;
        prop_options_init(&(pts->label_type), cgen->labeltype_rbox_label,
                        cgen->labeltype_rbox, cgen->labeltype_rbox_menu,
			n, item, (XtPointer*)item_val,
                        cgen->labeltype_cb);

        prop_field_init(&(pts->label), cgen->label_field_label,
                cgen->label_field, cgen->label_cb);
	prop_label_field_init(&(pts->label), cgen->graphic_hint, item, n);

        n = 0;
        item[n] = cgen->labelpos_rbox_items.Left_item;
        item_val[n] = AB_CP_WEST; n++;
        item[n] = cgen->labelpos_rbox_items.Above_item;
        item_val[n] = AB_CP_NORTH; n++;
        prop_options_init(&(pts->label_pos), cgen->labelpos_rbox_label,
                        cgen->labelpos_rbox, cgen->labelpos_rbox_menu,
                        n, item, (XtPointer*)item_val,
                        cgen->labeltype_cb);

        /* Menu Name */
/*
        prop_menuname_init(&(pts->menuname), cgen->menu_mbutton, cgen->menu_field,
                        cgen->name_field, cgen->menu_cb,
                        &(pts->current_obj), True);
*/

        /* Operation */
        n = 0;
        item[n] = cgen->op_rbox_items.Editable_item;
        item_val[n] = False; n++;
        item[n] = cgen->op_rbox_items.Read_Only_item;
        item_val[n] = True; n++;
        prop_radiobox_init(&(pts->op), cgen->op_rbox_label,
                cgen->op_rbox, n, item, (XtPointer*)item_val,
                cgen->op_cb);

        /* Maximum Chars */
        prop_field_init(&(pts->max_chars), cgen->charsmax_field_label,
                            cgen->charsmax_field, cgen->charsmax_cb);

        /* Initial Value */
        prop_field_init(&(pts->init_value), cgen->ivalue_field_label,
                            cgen->ivalue_field, cgen->ivalue_cb);

	/* Position */
	prop_geomfield_init(&(pts->pos), cgen->pos_label,
			    cgen->x_field_label, cgen->x_field,
			    cgen->y_field_label, cgen->y_field,
			    NULL, NULL, NULL, NULL,
			    cgen->pos_cb);

	/* Width */
        prop_geomfield_init(&(pts->size), cgen->width_opmenu_label,
			    NULL, NULL, NULL, NULL,
                            cgen->w_field_label, cgen->w_field,
                            NULL, NULL,
                            cgen->width_cb);

	n = 0;
	item[n] = cgen->width_opmenu_items.Characters_item;
	item_val[n] = SIZE_IN_CHARS_KEY; n++;
	item[n] = cgen->width_opmenu_items.Pixels_item;
	item_val[n] = SIZE_IN_PIXELS_KEY; n++;
	prop_options_init(&(pts->size_metric), cgen->width_opmenu_label,
			   cgen->width_opmenu,
			   cgen->width_opmenu_menu,
			   n, item, (XtPointer*)item_val,
			   cgen->width_cb);

	/* Initial State */
	n = 0;
	item[n] = cgen->istate_ckbox_items.Visible_item;
	item_val[n] = AB_STATE_VISIBLE; n++;
	item[n] = cgen->istate_ckbox_items.Active_item;
	item_val[n] = AB_STATE_ACTIVE; n++;
	prop_checkbox_init(&(pts->init_state), cgen->istate_ckbox_label,
			    cgen->istate_ckbox, n, item, item_val,
			    cgen->istate_cb);

	/* Color */
	prop_colorfield_init(&(pts->bg_color), cgen->bg_mbutton,
                cgen->bg_mbutton_bg_mbutton_menu_items.None_item,
                cgen->bg_mbutton_bg_mbutton_menu_items.Color_Chooser_item,
		cgen->bg_swatch, cgen->bg_field, cgen->bg_cb);

	prop_colorfield_init(&(pts->fg_color), cgen->fg_mbutton,
                cgen->fg_mbutton_fg_mbutton_menu_items.None_item,
                cgen->fg_mbutton_fg_mbutton_menu_items.Color_Chooser_item,
                cgen->fg_swatch, cgen->fg_field, cgen->fg_cb);

	prop_changebars_cleared(pts->prop_sheet);

	return (cgen->prop_dialog_shellform);
    }
    else
	return NULL;

}

static int
textf_prop_activate(
    AB_PROP_TYPE type,
    BOOL	 active
)
{
    ui_set_active(prop_textf_settings_rec[type].prop_sheet, active);

    return OK;
}


static int
textf_prop_clear(
    AB_PROP_TYPE type
)
{
    PropTextfSettingsRec	*pts = &(prop_textf_settings_rec[type]);

    if (pts->current_obj == NULL)
	return OK;

    /* Clear Name */
    prop_field_set_value(&(pts->name), "", False);

    /* Clear Label Type/Position */
    prop_options_set_value(&(pts->label_type), (XtPointer)AB_LABEL_STRING, False);
    prop_options_set_value(&(pts->label_pos), (XtPointer)AB_CP_WEST, False);

    /* Clear Label */
    ui_set_label_string(pts->label.label, "Label:");
    prop_field_set_value(&(pts->label), "", False);

    /* Clear Menu Name */
/*
    prop_menuname_set_value(&(pts->menuname), "", False);
*/

    /* Clear Operation */
    prop_radiobox_set_value(&(pts->op), (XtPointer)False, False);

    /* Clear Maximum Chars */
    prop_field_set_value(&(pts->max_chars), "", False);

    /* Clear Initial Value*/
    prop_field_set_value(&(pts->init_value), "", False);

    /* Clear Position */
    prop_geomfield_clear(&(pts->pos), GEOM_X);
    prop_geomfield_clear(&(pts->pos), GEOM_Y);

    /* Clear Size */
    prop_geomfield_clear(&(pts->size), GEOM_WIDTH);

    /* Clear Initial State */
    prop_checkbox_set_value(&(pts->init_state), AB_STATE_VISIBLE, True, False);
    prop_checkbox_set_value(&(pts->init_state), AB_STATE_ACTIVE, True, False);

    /* Clear Color */
    prop_colorfield_set_value(&(pts->bg_color), "", False);
    prop_colorfield_set_value(&(pts->fg_color), "", False);

    pts->current_obj = NULL;

    turnoff_changebars(type);

    return OK;

}

static int
textf_prop_load(
    ABObjPtr 	 obj,
    AB_PROP_TYPE type,
    unsigned long loadkey
)
{
    PropTextfSettingsRec 	*pts = &(prop_textf_settings_rec[type]);
    int				width;
    BOOL			load_all = (loadkey & LoadAll);

    if (obj == NULL)
    {
        if (pts->current_obj != NULL)
            obj = pts->current_obj;
        else
            return ERROR;
    }
    else if (!obj_is_text_field(obj))
        return ERROR;
    else
        pts->current_obj = obj;

    /* Load Name */
    if (load_all || loadkey & LoadName)
    	prop_field_set_value(&(pts->name), obj_get_name(obj), False);

    if (load_all)
    {
	/* Load Label Type/Position */
	prop_options_set_value(&(pts->label_type), (XtPointer)obj->label_type, False);
	prop_options_set_value(&(pts->label_pos), (XtPointer)obj_get_label_position(obj), False);

	/* Load Label */
	prop_setup_label_field(&(pts->label), NULL,
				obj->label_type, obj_get_label(obj), AB_LINE_UNDEF);
	/* Load Operation */
	prop_radiobox_set_value(&(pts->op),
                                (XtPointer)(intptr_t) obj_get_read_only(obj), False);

	/* Load Maximum Chars */
	prop_field_set_numeric_value(&(pts->max_chars), obj_get_max_length(obj), False);

	/* Load Initial Value */
	prop_field_set_value(&(pts->init_value), obj_get_initial_value_string(obj), False);

	/* Load Initial State */
	prop_checkbox_set_value(&(pts->init_state), AB_STATE_VISIBLE,
		obj_is_initially_visible(obj), False);
	prop_checkbox_set_value(&(pts->init_state), AB_STATE_ACTIVE,
		obj_is_initially_active(obj), False);

    	/* Load Color */
    	prop_colorfield_set_value(&(pts->bg_color), obj_get_bg_color(obj), False);
    	prop_colorfield_set_value(&(pts->fg_color), obj_get_fg_color(obj), False);

    	turnoff_changebars(type);
    }

    /* Load Position */
    if (load_all || loadkey & LoadPosition)
        prop_load_obj_position(obj, &(pts->pos));

    /* Load Width */
    if (load_all || loadkey & LoadSize)
    {
        if (obj_get_num_columns(obj) == -1) /* Size in Pixels */
        {
            prop_options_set_value(&(pts->size_metric), (XtPointer)SIZE_IN_PIXELS_KEY, False);
            width = abobj_get_comp_width(obj);
        }
        else /* Size in Chars */
        {
            prop_options_set_value(&(pts->size_metric), (XtPointer)SIZE_IN_CHARS_KEY, False);
            width = obj_get_num_columns(obj);
        }
        prop_geomfield_set_value(&(pts->size), GEOM_WIDTH, width, False);
    }

    return OK;
}

static int
textf_prop_apply(
    AB_PROP_TYPE   type
)
{
    PropTextfSettingsRec 	*pts = &(prop_textf_settings_rec[type]);
    STRING	    		value;
    int				num, metric, new_w;
    BOOL	    		size_chg = False;
    BOOL	    		reset_bg = False;
    BOOL	    		reset_fg = False;

    if (!verify_props(type))
        return ERROR;

    if (prop_changed(pts->name.changebar))
    {
        value = prop_field_get_value(&(pts->name));
        abobj_set_name(pts->current_obj, value);
        util_free(value);
    }
    if (prop_changed(pts->label.changebar) ||
	prop_changed(pts->label_type.changebar))
    {
        value = prop_field_get_value(&(pts->label));
        abobj_set_label(pts->current_obj,
            (AB_LABEL_TYPE)prop_options_get_value(&(pts->label_type)),
                        value);
        util_free(value);

        abobj_set_label_position(pts->current_obj,
                (AB_COMPASS_POINT)prop_options_get_value(&(pts->label_pos)));
        size_chg = True;
    }
/*
    if (prop_changed(pts->menuname.changebar))
    {
        value = prop_menuname_get_value(&(pts->menuname));
        abobj_set_menu_name(pts->current_obj, value);
        util_free(value);
        prop_menuname_set_value(&(pts->menuname),
                obj_get_menu_name(pts->current_obj), False);
    }
*/
    if (prop_changed(pts->op.changebar))
    {
	abobj_set_read_only(pts->current_obj,
		prop_radiobox_get_value(&(pts->op)));

    }
    if (prop_changed(pts->max_chars.changebar))
    {
        num = prop_field_get_numeric_value(&(pts->max_chars));
        abobj_set_max_length(pts->current_obj, num);
    }
    if (prop_changed(pts->init_value.changebar))
    {
        value = prop_field_get_value(&(pts->init_value));
        abobj_set_initial_value(pts->current_obj, value, 0);
        util_free(value);
    }
    if (prop_changed(pts->pos.changebar))
    {
	if (abobj_is_movable(pts->current_obj))
	    abobj_set_xy(pts->current_obj,
	     	prop_geomfield_get_value(&(pts->pos), GEOM_X),
	     	prop_geomfield_get_value(&(pts->pos), GEOM_Y));
    }
    if (prop_changed(pts->size.changebar))
    {
	metric = (int)(intptr_t) prop_options_get_value(&(pts->size_metric));
	new_w = prop_geomfield_get_value(&(pts->size), GEOM_WIDTH);

        abobj_set_num_columns(pts->current_obj,
		metric == SIZE_IN_CHARS_KEY? new_w : -1);
	abobj_set_pixel_width(pts->current_obj,
		metric == SIZE_IN_PIXELS_KEY? new_w : -1, 0);

        size_chg = True;
    }
    if (prop_changed(pts->init_state.changebar))
    {
        abobj_set_visible(pts->current_obj,
		prop_checkbox_get_value(&(pts->init_state), AB_STATE_VISIBLE));
        abobj_set_active(pts->current_obj,
		prop_checkbox_get_value(&(pts->init_state), AB_STATE_ACTIVE));
    }
    if (prop_changed(pts->fg_color.changebar))
    {
        value = prop_colorfield_get_value(&(pts->fg_color));
        abobj_set_foreground_color(pts->current_obj, value);
	if (util_strempty(value))
	    reset_fg = True;
        util_free(value);
        prop_colorfield_set_value(&(pts->fg_color), obj_get_fg_color(pts->current_obj), False);
    }
    if (prop_changed(pts->bg_color.changebar))
    {
        value = prop_colorfield_get_value(&(pts->bg_color));
        abobj_set_background_color(pts->current_obj, value);
	if (util_strempty(value))
	    reset_bg = True;
        util_free(value);
        prop_colorfield_set_value(&(pts->bg_color),
		obj_get_bg_color(pts->current_obj), False);
    }
    abobj_instantiate_changes(pts->current_obj);

    if (reset_bg || reset_fg) /* Set back to No Color */
        abobj_reset_colors(pts->current_obj, reset_bg, reset_fg);

    if (size_chg)
        abobj_force_dang_form_resize(pts->current_obj);

    turnoff_changebars(type);

    return OK;
}

static BOOL
textf_prop_pending(
    AB_PROP_TYPE type
)
{
    return(prop_changebars_pending(prop_textf_settings_rec[type].prop_sheet));
}

static BOOL
verify_props(
    AB_PROP_TYPE type
)
{
    PropTextfSettingsRec *pts = &(prop_textf_settings_rec[type]);

    if (prop_changed(pts->name.changebar) && !prop_name_ok(pts->current_obj, pts->name.field))
        return False;

    if ((prop_changed(pts->label_type.changebar) || prop_changed(pts->label.changebar)) &&
	prop_options_get_value(&(pts->label_type)) == (XtPointer)AB_LABEL_GLYPH &&
        !prop_graphic_filename_ok(pts->label.field, False))
        return False;

    if (prop_changed(pts->pos.changebar) &&
        (!prop_number_ok(pts->pos.x_field, (STRING)XFieldStr, -SHRT_MAX, SHRT_MAX) ||
         !prop_number_ok(pts->pos.y_field, (STRING)YFieldStr, -SHRT_MAX, SHRT_MAX)))
        return False;

    if (prop_changed(pts->size.changebar) &&
        !prop_number_ok(pts->size.w_field, (STRING)WFieldStr, 1, SHRT_MAX))
        return False;

    if (prop_changed(pts->max_chars.changebar) &&
	!prop_number_ok(pts->max_chars.field, "Maximum Char Field", 1, 999))
        return False;

    if (prop_changed(pts->fg_color.changebar) && !prop_color_ok(pts->fg_color.field))
        return False;

    if (prop_changed(pts->bg_color.changebar) && !prop_color_ok(pts->bg_color.field))
        return False;

/*
    if (prop_changed(pts->menuname.changebar) && !prop_obj_name_ok(pts->menuname.field,
			obj_get_module(pts->current_obj), AB_TYPE_MENU, "Menu"))
	return False;
*/

    return True;
}

static void
turnoff_changebars(
    AB_PROP_TYPE type
)
{
    PropTextfSettingsRec *pts = &(prop_textf_settings_rec[type]);

    prop_set_changebar(pts->name.changebar,    		PROP_CB_OFF);
    prop_set_changebar(pts->pos.changebar,    		PROP_CB_OFF);
    prop_set_changebar(pts->size.changebar,    		PROP_CB_OFF);
    prop_set_changebar(pts->label_type.changebar, 	PROP_CB_OFF);
    prop_set_changebar(pts->label.changebar, 		PROP_CB_OFF);
    prop_set_changebar(pts->op.changebar, 		PROP_CB_OFF);
    prop_set_changebar(pts->max_chars.changebar, 	PROP_CB_OFF);
    prop_set_changebar(pts->init_value.changebar, 	PROP_CB_OFF);
    prop_set_changebar(pts->init_state.changebar,	PROP_CB_OFF);
    prop_set_changebar(pts->bg_color.changebar, 	PROP_CB_OFF);
    prop_set_changebar(pts->fg_color.changebar, 	PROP_CB_OFF);
/*
    prop_set_changebar(pts->menuname.changebar, 	PROP_CB_OFF);
*/

    prop_changebars_cleared(pts->prop_sheet);

}
