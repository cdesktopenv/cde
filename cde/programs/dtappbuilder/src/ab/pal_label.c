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
 *      $XConsortium: pal_label.c /main/5 1996/08/08 18:01:43 mustafa $
 *
 * @(#)pal_label.c	1.74 11 Aug 1995      cde_app_builder/src/ab
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
 * pal_label.c - Implements Palette Label object functionality
 */
#include <stdio.h>
#include <Xm/Xm.h>
#include <ab_private/ab.h>
#include <ab_private/abobj.h>
#include <ab_private/abobj_set.h>
#include <ab_private/pal.h>
#include <ab_private/prop.h>
#include <ab_private/ui_util.h>
#include "label_ui.h"


typedef struct  PROP_LABEL_SETTINGS
{
    Widget                      prop_sheet;
    PropFieldSettingRec         name;
    PropOptionsSettingRec       label_type;
    PropFieldSettingRec         label;
    PropOptionsSettingRec       label_align;
    PropRadioSettingRec       	size_policy;
    PropGeometrySettingRec      geometry;
    PropCheckboxSettingRec      init_state;
    PropColorSettingRec         bg_color;
    PropColorSettingRec         fg_color;
    ABObj                       current_obj;
} PropLabelSettingsRec, *PropLabelSettings;

/*************************************************************************
**                                                                      **
**       Private Function Declarations                                  **
**                                                                      **
**************************************************************************/
/*
 * Methods
 */
static int      label_initialize(
                    ABObj   obj
                );
static Widget   label_prop_init(
                    Widget  parent,
                    AB_PROP_TYPE type
                );
static int	label_prop_activate(
		    AB_PROP_TYPE type,
		    BOOL 	 active
		);
static int	label_prop_clear(
		    AB_PROP_TYPE type
		);
static int      label_prop_load(
                    ABObj   	 obj,
                    AB_PROP_TYPE type,
		    unsigned long loadkey
                );
static int	label_prop_apply(
		    AB_PROP_TYPE type
		);
static BOOL     label_prop_pending(
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
static void	size_policyCB(
		    Widget   	widget,
                    XtPointer 	clientdata,
                    XmToggleButtonCallbackStruct *state
                );


/*************************************************************************
**                                                                      **
**       Data  		                                                 **
**                                                                      **
**************************************************************************/
PalItemInfo label_palitem_rec = {

    /* type             */  AB_TYPE_LABEL,
    /* name             */  "Label",
    /* animation pixmaps*/  NULL,
    /* number of pixmaps*/  0,
    /* rev_prop_frame   */  NULL,
    /* fix_prop_dialog  */  NULL,
    /* initialize       */  label_initialize,
    /* is_a_test        */  obj_is_label,
    /* prop_initialize  */  label_prop_init,
    /* prop_activate	*/  label_prop_activate,
    /* prop_clear	*/  label_prop_clear,
    /* prop_load        */  label_prop_load,
    /* prop_apply	*/  label_prop_apply,
    /* prop_pending	*/  label_prop_pending

};

PalItemInfo *ab_label_palitem = &label_palitem_rec;
PropLabelSettingsRec prop_label_settings_rec[AB_PROP_TYPE_NUM_VALUES];


/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/
static int
label_initialize(
    ABObj    obj
)
{
    obj_set_unique_name(obj, "label");

    obj_set_label(obj, catgets(Dtb_project_catd, 6, 69, "Label"));
    obj_set_is_initially_visible(obj, True);
    obj_set_is_initially_active(obj, True);

    obj_set_attachment(obj, AB_CP_NORTH, AB_ATTACH_POINT, NULL, obj->y);
    obj_set_attachment(obj, AB_CP_WEST,  AB_ATTACH_POINT, NULL, obj->x);

    return OK;

}

static Widget
label_prop_init(
    Widget parent,
    AB_PROP_TYPE type
)
{
    DtbLabelPropDialogInfoRec	rev_label_prop_dialog; /* Revolving Props */
    DtbLabelPropDialogInfo     	cgen = &dtb_label_prop_dialog; /* Codegen structure */
    DtbRevolvPropDialogInfo	rpd = &(dtb_revolv_prop_dialog);
    PropLabelSettingsRec    	*pls = &(prop_label_settings_rec[type]);
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
        dtbLabelPropDialogInfo_clear(&rev_label_prop_dialog);

	cgen = &(rev_label_prop_dialog);
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
    else /* AB_PROP_FIXED - create entire dialog */
    	cgen = &dtb_label_prop_dialog;


    if (dtb_label_prop_dialog_initialize(cgen, parent) == 0)
    {
        pls->prop_sheet = cgen->attrs_ctrlpanel;
        pls->current_obj = NULL;

	if (type == AB_PROP_REVOLVING)
        	XtVaSetValues(parent,
                        XmNuserData, pls->current_obj,
                        NULL);

        /* Dialog/Object List */
	if (type == AB_PROP_FIXED)
	{
            prop_fixed_dialog_init(ab_label_palitem,
			cgen->prop_dialog_shellform, cgen->objlist);
            prop_activate_panel_init(type, ab_label_palitem,
                        cgen->ok_button, cgen->apply_button,
                        cgen->reset_button, cgen->cancel_button,
                        cgen->help_button);
	}

        /* Alternate Editor Buttons */
        prop_editors_panel_init(type, ab_label_palitem,
            cgen->attach_button, cgen->conn_button, cgen->helptxt_button);

        /*
          * Prop Sheet Settings....
         */

        /* Name */
        prop_field_init(&(pls->name), cgen->name_field_label,
                cgen->name_field, cgen->name_cb);

        /* Label Type */
        n = 0;
        item[n] = cgen->labeltype_rbox_items.String_item;
        item_val[n] = AB_LABEL_STRING; n++;
        item[n] = cgen->labeltype_rbox_items.Graphic_item;
        item_val[n] = AB_LABEL_GLYPH; n++;
        prop_options_init(&(pls->label_type), cgen->labeltype_rbox_label,
                        cgen->labeltype_rbox, cgen->labeltype_rbox_menu,
			n, item, (XtPointer*)item_val,
                        cgen->labeltype_cb);

        /* Label */
        prop_field_init(&(pls->label), cgen->label_label, cgen->label_textpane,
                cgen->label_cb);

	prop_label_field_init(&(pls->label), cgen->graphic_hint, item, n);

        /* Label Align */
        n = 0;
        item[n] = cgen->labelalign_opmenu_items.Left_item;
        item_val[n] = AB_ALIGN_LEFT; n++;
        item[n] = cgen->labelalign_opmenu_items.Centered_item;
        item_val[n] = AB_ALIGN_CENTER; n++;
        item[n] = cgen->labelalign_opmenu_items.Right_item;
        item_val[n] = AB_ALIGN_RIGHT; n++;
        prop_options_init(&(pls->label_align), cgen->labelalign_opmenu_label,
                cgen->labelalign_opmenu, cgen->labelalign_opmenu_menu,
                n, item, (XtPointer*)item_val,
                cgen->labelalign_cb);

        /* Size Policy */
        n = 0;
        item[n] = cgen->szpolicy_rbox_items.Size_of_Label_item;
        item_val[n] = SIZE_OF_CONTENTS_KEY; n++;
        item[n] = cgen->szpolicy_rbox_items.Fixed_item;
        item_val[n] = SIZE_FIXED_KEY; n++;
        prop_radiobox_init(&(pls->size_policy), cgen->szpolicy_rbox_label,
                cgen->szpolicy_rbox, n, item, (XtPointer*)item_val,
                cgen->szpolicy_cb);

        for(i=0; i < n; i++)
            XtAddCallback(item[i], XmNvalueChangedCallback,
                        (XtCallbackProc)prop_size_policyCB, (XtPointer)&(pls->geometry));


        /* Geometry */
        prop_geomfield_init(&(pls->geometry), cgen->geom_label,
                cgen->x_field_label, cgen->x_field,
                cgen->y_field_label, cgen->y_field,
                cgen->width_field_label, cgen->width_field,
                cgen->height_field_label, cgen->height_field,
                cgen->geom_cb);

        /* Initial State */
        n = 0;
        item[n] = cgen->istate_ckbox_items.Visible_item;
        item_val[n] = AB_STATE_VISIBLE; n++;
        item[n] = cgen->istate_ckbox_items.Active_item;
        item_val[n] = AB_STATE_ACTIVE; n++;
        prop_checkbox_init(&(pls->init_state), cgen->istate_ckbox_label,
                cgen->istate_ckbox, n, item, item_val,
                cgen->istate_cb);

        /* Background */
        prop_colorfield_init(&(pls->bg_color), cgen->bg_mbutton,
		cgen->bg_mbutton_bg_mbutton_menu_items.None_item,
		cgen->bg_mbutton_bg_mbutton_menu_items.Color_Chooser_item,
                cgen->bg_swatch, cgen->bg_field, cgen->bg_cb);

        /* Foreground */
        prop_colorfield_init(&(pls->fg_color), cgen->fg_mbutton,
                cgen->fg_mbutton_fg_mbutton_menu_items.None_item,
                cgen->fg_mbutton_fg_mbutton_menu_items.Color_Chooser_item,
                cgen->fg_swatch, cgen->fg_field, cgen->fg_cb);

        prop_changebars_cleared(pls->prop_sheet);

	return (cgen->prop_dialog_shellform);
    }
    else
    	return NULL;

}

static int
label_prop_activate(
    AB_PROP_TYPE type,
    BOOL	 active
)
{
    ui_set_active(prop_label_settings_rec[type].prop_sheet, active);

    return OK;
}


static int
label_prop_clear(
    AB_PROP_TYPE type
)
{
    PropLabelSettingsRec	*pls = &(prop_label_settings_rec[type]);

    if (pls->current_obj == NULL)
	return OK;

    /* Clear Name */
    prop_field_set_value(&(pls->name), "", False);

    /* Clear LabelType */
    prop_options_set_value(&(pls->label_type), (XtPointer)AB_LABEL_STRING, False);
    ui_set_active(pls->label.field, True);
    ui_set_active(pls->label.label, True);

    /* Clear Label */
    ui_set_label_string(pls->label.label, (STRING)LabelForString);
    prop_field_set_value(&(pls->label), "", False);

    /* Clear Label Alignment */
    prop_options_set_value(&(pls->label_align), (XtPointer)AB_ALIGN_CENTER, False);

    /* Clear Geometry */
    prop_geomfield_clear(&(pls->geometry), GEOM_X);
    prop_geomfield_clear(&(pls->geometry), GEOM_Y);
    prop_geomfield_clear(&(pls->geometry), GEOM_WIDTH);
    prop_geomfield_clear(&(pls->geometry), GEOM_HEIGHT);

    /* Clear Size Policy */
    prop_radiobox_set_value(&(pls->size_policy),
                            (XtPointer)SIZE_OF_CONTENTS_KEY, False);

    /* Clear Initial State */
    prop_checkbox_set_value(&(pls->init_state), AB_STATE_VISIBLE, True, False);
    prop_checkbox_set_value(&(pls->init_state), AB_STATE_ACTIVE, True, False);

    /* Clear Background Color */
    prop_colorfield_set_value(&(pls->bg_color), "", False);

    /* Clear Foreground Color */
    prop_colorfield_set_value(&(pls->fg_color), "", False);

    pls->current_obj = NULL;

    turnoff_changebars(type);

    return OK;

}

static int
label_prop_load(
    ABObjPtr 	 obj,
    AB_PROP_TYPE type,
    unsigned long loadkey
)
{
    PropLabelSettingsRec 	*pls = &(prop_label_settings_rec[type]);
    BOOL			load_all = (loadkey & LoadAll);

    if (obj == NULL)
    {
        if (pls->current_obj != NULL)
            obj = pls->current_obj;
        else
            return ERROR;
    }
    else if (!obj_is_label(obj))
        return ERROR;
    else
        pls->current_obj = obj;

    /* Load Name */
    if (load_all || loadkey & LoadName)
    	prop_field_set_value(&(pls->name), obj_get_name(obj), False);

    if (load_all)
    {
	/* Load Label */
	prop_options_set_value(&(pls->label_type), (XtPointer)obj->label_type, False);
	prop_setup_label_field(&(pls->label), NULL,
				obj->label_type, obj_get_label(obj), AB_LINE_UNDEF);

	/* Load Label Alignment */
	prop_options_set_value(&(pls->label_align), (XtPointer)obj_get_label_alignment(obj), False);

	/* Load Size Policy */
	prop_radiobox_set_value(&(pls->size_policy),
				abobj_width_resizable(obj)?
                                (XtPointer)SIZE_FIXED_KEY :
                                (XtPointer)SIZE_OF_CONTENTS_KEY, False);

	/* Load Initial State */
	prop_checkbox_set_value(&(pls->init_state), AB_STATE_VISIBLE,
		obj_is_initially_visible(obj), False);
	prop_checkbox_set_value(&(pls->init_state), AB_STATE_ACTIVE,
		obj_is_initially_active(obj), False);

	/* Load Color */
	prop_colorfield_set_value(&(pls->bg_color), obj_get_bg_color(obj), False);
	prop_colorfield_set_value(&(pls->fg_color), obj_get_fg_color(obj), False);

	turnoff_changebars(type);
    }

    /* Load Geometry */
    if (load_all || loadkey & LoadPosition)
        prop_load_obj_position(obj, &(pls->geometry));

    if (load_all || loadkey & LoadSize)
        prop_load_obj_size(obj, &(pls->geometry));

    return OK;
}

int
label_prop_apply(
    AB_PROP_TYPE   type
)
{
    PropLabelSettingsRec 	*pls = &(prop_label_settings_rec[type]);
    STRING			value;
    BOOL			size_chg = False;
    BOOL			reset_bg = False;
    BOOL			reset_fg = False;
    int				new_w, new_h;

    if (!verify_props(type))
        return ERROR;

    if (prop_changed(pls->name.changebar))
    {
        value = prop_field_get_value(&(pls->name));
        abobj_set_name(pls->current_obj, value);
        util_free(value);
    }
    if (prop_changed(pls->label.changebar) || prop_changed(pls->label_type.changebar))
    {
        value = prop_field_get_value(&(pls->label));
        abobj_set_label(pls->current_obj,
	    (AB_LABEL_TYPE)prop_options_get_value(&(pls->label_type)),
            value);
        util_free(value);
        size_chg = True;
    }
    if (prop_changed(pls->size_policy.changebar))
    {
        BOOL    fixed_size = prop_radiobox_get_value(
                                &(pls->size_policy)) == SIZE_FIXED_KEY;
        /* If the label or label type hasn't changed and the
         * label's size was set to "Fixed" and now is being
         * changed to "Size of Label", then we have to re-
         * instantiate the label along with setting the
         * XmNrecomputeSize resource back to True, so that the
	 * label will resize itself correctly.  Otherwise, the
	 * resize won't take place.
         */
        if (!prop_changed(pls->label.changebar) &&
            !prop_changed(pls->label_type.changebar) &&
            (obj_get_width(pls->current_obj) != -1) && !fixed_size)
        {
	    obj_clear_flag(pls->current_obj, InstantiatedFlag);
	}

	abobj_set_size_policy(pls->current_obj, fixed_size);
	size_chg = True;
    }
    if (prop_changed(pls->geometry.changebar))
    {
	if (abobj_width_resizable(pls->current_obj))
	{
	    new_w = prop_geomfield_get_value(&(pls->geometry), GEOM_WIDTH);
	    abobj_set_pixel_width(pls->current_obj, new_w, 0);
	}
	if (abobj_height_resizable(pls->current_obj))
	{
	    new_h = prop_geomfield_get_value(&(pls->geometry), GEOM_HEIGHT);
	    abobj_set_pixel_height(pls->current_obj, new_h, 0);
	}
	if (abobj_is_movable(pls->current_obj))
	    abobj_set_xy(pls->current_obj,
	     	prop_geomfield_get_value(&(pls->geometry), GEOM_X),
	     	prop_geomfield_get_value(&(pls->geometry), GEOM_Y));

	size_chg = True;
    }
    if (prop_changed(pls->init_state.changebar))
    {
        abobj_set_visible(pls->current_obj,
		prop_checkbox_get_value(&(pls->init_state), AB_STATE_VISIBLE));
        abobj_set_active(pls->current_obj,
		prop_checkbox_get_value(&(pls->init_state), AB_STATE_ACTIVE));
    }
    if (prop_changed(pls->label_align.changebar))
    {
        abobj_set_label_alignment(pls->current_obj,
                (AB_ALIGNMENT)prop_options_get_value(&(pls->label_align)));
        size_chg = True;
    }
    if (prop_changed(pls->fg_color.changebar))
    {
        value = prop_colorfield_get_value(&(pls->fg_color));
        abobj_set_foreground_color(pls->current_obj, value);
	if (util_strempty(value))
	    reset_fg = True;
        util_free(value);
        prop_colorfield_set_value(&(pls->fg_color), obj_get_fg_color(pls->current_obj), False);
    }
    if (prop_changed(pls->bg_color.changebar))
    {
        value = prop_colorfield_get_value(&(pls->bg_color));
        abobj_set_background_color(pls->current_obj, value);
	if (util_strempty(value))
	    reset_bg = True;
        util_free(value);
        prop_colorfield_set_value(&(pls->bg_color), obj_get_bg_color(pls->current_obj), False);
    }

    abobj_instantiate_changes(pls->current_obj);

    if (reset_bg || reset_fg) /* Set back to No Color */
        abobj_reset_colors(pls->current_obj, reset_bg, reset_fg);
    if (size_chg)
	abobj_force_dang_form_resize(pls->current_obj);

    /* Label may have changed Size as a result...*/
    prop_geomfield_set_value(&(pls->geometry), GEOM_WIDTH,
		abobj_get_comp_width(pls->current_obj), False);
    prop_geomfield_set_value(&(pls->geometry), GEOM_HEIGHT,
		abobj_get_comp_height(pls->current_obj), False);

    turnoff_changebars(type);

    return OK;

}

static BOOL
label_prop_pending(
    AB_PROP_TYPE type
)
{
    return(prop_changebars_pending(prop_label_settings_rec[type].prop_sheet));
}

static BOOL
verify_props(
    AB_PROP_TYPE type
)
{
    PropLabelSettingsRec *pls = &(prop_label_settings_rec[type]);

    if (prop_changed(pls->name.changebar) && !prop_name_ok(pls->current_obj, pls->name.field))
        return False;

    if ((prop_changed(pls->label_type.changebar) || prop_changed(pls->label.changebar)) &&
	prop_options_get_value(&(pls->label_type)) == (XtPointer)AB_LABEL_GLYPH &&
        !prop_graphic_filename_ok(pls->label.field, False))
        return False;

    if (prop_changed(pls->geometry.changebar) &&
        (!prop_number_ok(pls->geometry.x_field, (STRING)XFieldStr,-SHRT_MAX, SHRT_MAX) ||
         !prop_number_ok(pls->geometry.y_field, (STRING)YFieldStr,-SHRT_MAX, SHRT_MAX) ||
         !prop_number_ok(pls->geometry.w_field, (STRING)WFieldStr, 1, SHRT_MAX) ||
         !prop_number_ok(pls->geometry.h_field, (STRING)HFieldStr, 1, SHRT_MAX)))
        return False;

    if (prop_changed(pls->fg_color.changebar) && !prop_color_ok(pls->fg_color.field))
        return False;

    if (prop_changed(pls->bg_color.changebar) && !prop_color_ok(pls->bg_color.field))
        return False;

    return True;
}

static void
turnoff_changebars(
    AB_PROP_TYPE type
)
{
    PropLabelSettingsRec *pls = &(prop_label_settings_rec[type]);

    prop_set_changebar(pls->name.changebar,    	PROP_CB_OFF);
    prop_set_changebar(pls->label_type.changebar,PROP_CB_OFF);
    prop_set_changebar(pls->label.changebar,     PROP_CB_OFF);
    prop_set_changebar(pls->label_align.changebar,PROP_CB_OFF);
    prop_set_changebar(pls->geometry.changebar,		PROP_CB_OFF);
    prop_set_changebar(pls->size_policy.changebar,	PROP_CB_OFF);
    prop_set_changebar(pls->init_state.changebar,PROP_CB_OFF);
    prop_set_changebar(pls->bg_color.changebar,  PROP_CB_OFF);
    prop_set_changebar(pls->fg_color.changebar,  PROP_CB_OFF);

    prop_changebars_cleared(pls->prop_sheet);

}

static void
size_policyCB(
    Widget	w,
    XtPointer	clientdata,
    XmToggleButtonCallbackStruct *state
)
{
    AB_PROP_TYPE        	type = (AB_PROP_TYPE)clientdata;
    PropLabelSettingsRec        *pls = &(prop_label_settings_rec[type]);
    XtArgVal			value;

    /* Width/Height fields should ONLY be editable if Size Policy
     * is "Fixed"
     */
    if (state->set)
    {
    	XtVaGetValues(w, XmNuserData, &value, NULL);
    	ui_field_set_editable(pls->geometry.w_field, value == SIZE_FIXED_KEY);
    	ui_field_set_editable(pls->geometry.h_field, value == SIZE_FIXED_KEY);
    }
}
