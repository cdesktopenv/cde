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
 *	$XConsortium: pal_spinbox.c /main/5 1996/08/08 18:03:54 mustafa $
 *
 *	@(#)pal_spinbox.c	1.4 22 Feb 1994
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
 * pal_spinbox.c - Implements Palette SpinBox object functionality
 */
#include <stdio.h>
#include <Xm/Xm.h>
#include <ab_private/obj_notify.h>
#include <ab_private/pal.h>
#include <ab_private/ab.h>
#include <ab_private/prop.h>
#include <ab_private/abobj.h>
#include <ab_private/abobj_set.h>
#include <ab_private/ui_util.h>
#include "spinbox_ui.h"

const    int    spinbox_init_width  = 100;

typedef struct  PROP_SPINBOX_SETTINGS
{
    Widget                      prop_sheet;
    PropFieldSettingRec         name;
    PropRadioSettingRec         spinbox_type;
    PropGeometrySettingRec      geometry;
    PropOptionsSettingRec       label_type;
    PropFieldSettingRec         label;
    PropOptionsSettingRec       label_pos;
    PropOptionsSettingRec       arrow_style;
    Widget			valrange_label;
    PropFieldSettingRec         min;
    PropFieldSettingRec         max;
    PropFieldSettingRec         incr;
    PropFieldSettingRec         decimal;
    PropFieldSettingRec		ivalue;
    PropCheckboxSettingRec      init_state;
    PropColorSettingRec         bg_color;
    PropColorSettingRec         fg_color;
    PropFieldSettingRec         item_label;
    PropCheckboxSettingRec	item_state;
    Widget			item_list_label;
    PropItemsSettingRec         items;
    Widget			item_add_button;
    Widget			item_edit_button;
    ABObj                       current_obj;
} PropSpinboxSettingsRec, *PropSpinboxSettings;

/*************************************************************************
**                                                                      **
**       Private Function Declarations                                  **
**                                                                      **
**************************************************************************/
/*
 * Methods
 */
static int      spinbox_initialize(
                    ABObj   obj
                );
static Widget   spinbox_prop_init(
                    Widget  parent,
                    AB_PROP_TYPE type
                );
static int	spinbox_prop_activate(
		    AB_PROP_TYPE type,
		    BOOL 	 active
		);
static int	spinbox_prop_clear(
		    AB_PROP_TYPE type
		);
static int      spinbox_prop_load(
                    ABObj   	 obj,
                    AB_PROP_TYPE type,
		    unsigned long loadkey
                );
static int	spinbox_prop_apply(
		    AB_PROP_TYPE type
		);
static BOOL     spinbox_prop_pending(
                    AB_PROP_TYPE type
                );

static BOOL	verify_props(
		    AB_PROP_TYPE type
		);
static void	turnoff_changebars(
		    AB_PROP_TYPE type
		);
static void	setup_content_settings(
		    AB_PROP_TYPE type,
		    AB_TEXT_TYPE spinbox_type
		);
/*
 * ABObj Callbacks
 */
static int	spinbox_install_obj_destroy_CB(void);

static int	spinbox_obj_destroy_CB(
		    ObjEvDestroyInfo destroyInfo
		);
/*
 * Xt Callbacks
 */
static void     spinbox_typeCB(
                    Widget   widget,
                    XtPointer clientdata,
                    XmToggleButtonCallbackStruct *state
                );

/*************************************************************************
**                                                                      **
**       Data  		                                                 **
**                                                                      **
**************************************************************************/
PalItemInfo spinbox_palitem_rec = {

    /* type             */  AB_TYPE_SPIN_BOX,
    /* name             */  "Spin Box",
    /* animation pixmaps*/  NULL,
    /* number of pixmaps*/  0,
    /* rev_prop_frame   */  NULL,
    /* fix_prop_dialog  */  NULL,
    /* initialize       */  spinbox_initialize,
    /* is_a_test        */  obj_is_spin_box,
    /* prop_initialize  */  spinbox_prop_init,
    /* prop_activate	*/  spinbox_prop_activate,
    /* prop_clear	*/  spinbox_prop_clear,
    /* prop_load        */  spinbox_prop_load,
    /* prop_apply	*/  spinbox_prop_apply,
    /* prop_pending	*/  spinbox_prop_pending

};

PalItemInfo *ab_spinbox_palitem = &spinbox_palitem_rec;
PropSpinboxSettingsRec prop_spinbox_settings_rec[AB_PROP_TYPE_NUM_VALUES];


/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/
static int
spinbox_initialize(
    ABObj    obj
)
{
    obj_set_unique_name(obj, "spinbox");

    obj_set_label(obj, catgets(Dtb_project_catd, 100, 262, "SpinBox:"));
    obj_set_label_position(obj, AB_CP_NORTH);
    obj_set_width(obj, spinbox_init_width);
    obj_set_text_type(obj, AB_TEXT_NUMERIC);
    obj_set_min_value(obj, 0);
    obj_set_max_value(obj, 100);
    obj_set_increment(obj, 1);
    obj_set_arrow_style(obj, AB_ARROW_END);
    obj_set_is_initially_visible(obj, True);
    obj_set_is_initially_active(obj, True);

    obj_set_attachment(obj, AB_CP_NORTH, AB_ATTACH_POINT, NULL, obj->y);
    obj_set_attachment(obj, AB_CP_WEST,  AB_ATTACH_POINT, NULL, obj->x);

    return OK;
}

static Widget
spinbox_prop_init(
    Widget parent,
    AB_PROP_TYPE type
)
{
    DtbSpinboxPropDialogInfoRec	rev_spinbox_prop_dialog; /* Revolving Props */
    DtbSpinboxPropDialogInfo    cgen = &dtb_spinbox_prop_dialog; /* Codegen structure */
    DtbRevolvPropDialogInfo	rpd = &(dtb_revolv_prop_dialog);
    PropSpinboxSettingsRec    	*pss = &(prop_spinbox_settings_rec[type]);
    Widget            		item[6];
    int            		item_val[6];
    Widget			item2[6];
    int				item2_val[6];
    int            		i, n, j;

    if (type == AB_PROP_REVOLVING)
    {
	/* Cloning Trick:
	 * Only the Attributes ControlPanel needs to be created within
	 * the existing Revolving Prop dialog, so fill out all other
	 * fields with the Revolving Prop dialog equivelents, so the
	 * dtb initialize proc will skip those non-NULL fields...
	 */
        dtbSpinboxPropDialogInfo_clear(&rev_spinbox_prop_dialog);

	cgen = &(rev_spinbox_prop_dialog);
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

    	/*
     	 * Get notification of object destruction!
     	 */
    	spinbox_install_obj_destroy_CB();

    }
    else /* AB_PROP_FIXED */
    	cgen = &dtb_spinbox_prop_dialog;

    if (dtb_spinbox_prop_dialog_initialize(cgen, parent) == 0)
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
            prop_fixed_dialog_init(ab_spinbox_palitem,
			cgen->prop_dialog_shellform, cgen->objlist);
            prop_activate_panel_init(type, ab_spinbox_palitem,
                        cgen->ok_button, cgen->apply_button,
                        cgen->reset_button, cgen->cancel_button,
                        cgen->help_button);
	}

        /* Alternate Editor Buttons */
        prop_editors_panel_init(type, ab_spinbox_palitem,
            cgen->attach_button, cgen->conn_button, cgen->helptxt_button);

        /*
          * Prop Sheet Settings....
         */

        /* Name */
        prop_field_init(&(pss->name), cgen->name_field_label,
                cgen->name_field, cgen->name_cb);

        /* Spinbox Type */
        n = 0;
        item[n] = cgen->spinboxtype_rbox_items.Numeric_item;
        item_val[n] = AB_TEXT_NUMERIC; n++;
        item[n] = cgen->spinboxtype_rbox_items.String_List_item;
        item_val[n] = AB_TEXT_DEFINED_STRING; n++;
        prop_radiobox_init(&(pss->spinbox_type), cgen->spinboxtype_rbox_label,
                       cgen->spinboxtype_rbox, n, item, (XtPointer*)item_val,
                       cgen->spinboxtype_cb);
        for(i=0; i < n; i++)
            XtAddCallback(item[i], XmNvalueChangedCallback,
                        (XtCallbackProc)spinbox_typeCB, (XtPointer)type);

        /* Label, Type, Position */
        n = 0;
        item[n] = cgen->labeltype_rbox_items.String_item;
        item_val[n] = AB_LABEL_STRING; n++;
        item[n] = cgen->labeltype_rbox_items.Graphic_item;
        item_val[n] = AB_LABEL_GLYPH; n++;
        prop_options_init(&(pss->label_type), cgen->labeltype_rbox_label,
                cgen->labeltype_rbox, cgen->labeltype_rbox_menu,
		n, item, (XtPointer*)item_val,
                cgen->labeltype_cb);

        prop_field_init(&(pss->label), cgen->label_field_label,
		cgen->label_field, cgen->label_cb);

	prop_label_field_init(&(pss->label), cgen->graphic_hint, item, n);

        n = 0;
        item[n] = cgen->labelpos_rbox_items.Above_item;
        item_val[n] = AB_CP_NORTH; n++;
        item[n] = cgen->labelpos_rbox_items.Left_item;
        item_val[n] = AB_CP_WEST; n++;
        prop_options_init(&(pss->label_pos), cgen->labeltype_rbox_label,
                cgen->labelpos_rbox, cgen->labelpos_rbox_menu,
                n, item, (XtPointer*)item_val,
                cgen->labeltype_cb);

	/* Arrow Style */
        n = 0;
        item[n] = cgen->arrowstyle_opmenu_items.Flat_Beginning_item;
        item_val[n] = AB_ARROW_FLAT_BEGIN; n++;
        item[n] = cgen->arrowstyle_opmenu_items.Flat_End_item;
        item_val[n] = AB_ARROW_FLAT_END; n++;
        item[n] = cgen->arrowstyle_opmenu_items.Beginning_item;
        item_val[n] = AB_ARROW_BEGIN; n++;
        item[n] = cgen->arrowstyle_opmenu_items.End_item;
        item_val[n] = AB_ARROW_END; n++;
        item[n] = cgen->arrowstyle_opmenu_items.Split_item;
        item_val[n] = AB_ARROW_SPLIT; n++;
	prop_options_init(&(pss->arrow_style), cgen->arrowstyle_opmenu_label,
		cgen->arrowstyle_opmenu, cgen->arrowstyle_opmenu_menu,
		n, item, (XtPointer*)item_val,
		cgen->arrowstyle_cb);

        /* Min & Max & Incr */
	pss->valrange_label = cgen->valrange_label;
        prop_field_init(&(pss->min), cgen->min_field_label,
                cgen->min_field, cgen->valrange_cb);
        prop_field_init(&(pss->max), cgen->max_field_label,
                cgen->max_field, cgen->valrange_cb);
        prop_field_init(&(pss->incr), cgen->incr_field_label,
                cgen->incr_field, cgen->valrange_cb);

	/* Initial Value/Decimal Points */
        prop_field_init(&(pss->ivalue), cgen->ivalue_label,
                cgen->ivalue_field, cgen->ivalue_cb);

	prop_field_init(&(pss->decimal), cgen->decimal_field_label,
		cgen->decimal_field, cgen->ivalue_cb);

        /* Geometry */
        prop_geomfield_init(&(pss->geometry), cgen->geom_label,
                cgen->x_field_label, cgen->x_field,
                cgen->y_field_label, cgen->y_field,
                cgen->width_field_label, cgen->width_field,
		NULL, NULL,
                cgen->geom_cb);

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

	/* Item Editor....*/

	/* Item Label */
        prop_field_init(&(pss->item_label), cgen->itemlabel_label,
                cgen->itemlabel_field, cgen->itemlist_cb);

        /* Item State */
        n = 0;
        item[n] = cgen->item_state_ckbox_items.Selected_item;
        item_val[n] = AB_STATE_SELECTED; n++;
        prop_checkbox_init(&(pss->item_state), NULL,
                cgen->item_state_ckbox, n, item, item_val,
                cgen->itemlist_cb);

        /* Store Items->Add menu items in array */
        n = 0;
        item[n] = cgen->item_edit_mbutton_editmenu_items.Add_Before_item;
        item_val[n] = INSERT_BEFORE; n++;
        item[n] = cgen->item_edit_mbutton_editmenu_items.Add_After_item;
        item_val[n] = INSERT_AFTER; n++;

	/* Store Items->Edit menu items in array */
        j = 0;
        item2[j] = cgen->item_edit_mbutton_editmenu_items.Cut_item;
        item2_val[j] = AB_EDIT_CUT; j++;
        item2[j] = cgen->item_edit_mbutton_editmenu_items.Copy_item;
        item2_val[j] = AB_EDIT_COPY; j++;
        item2[j] = cgen->item_edit_mbutton_editmenu_items.Paste_item;
        item2_val[j] = AB_EDIT_PASTE; j++;
        item2[j] = cgen->item_edit_mbutton_editmenu_items.Delete_item;
        item2_val[j] = AB_EDIT_DELETE; j++;
	item2[j] = cgen->item_edit_mbutton_editmenu_items.Change_item;
	item2_val[j] = EDIT_CHANGE; j++;

	pss->item_list_label = cgen->itemlist_label;
	pss->item_add_button = cgen->item_add_button;
	pss->item_edit_button = cgen->item_edit_mbutton;
	prop_item_editor_init(&(pss->items), AB_ITEM_FOR_SPIN_BOX,
		cgen->itemlist, cgen->itemlist_cb,
		cgen->item_add_button,
		n, item, item_val,
		j, item2, item2_val,
		&(pss->item_label), NULL/*label_type*/, NULL/*graphic_hint*/,
		NULL/*mnemonic*/, NULL/*accel*/, NULL/*line_style*/,
		&(pss->item_state), NULL/*submenu*/,
		&(pss->current_obj));

        prop_changebars_cleared(pss->prop_sheet);

	return (cgen->prop_dialog_shellform);
    }
    else
    	return NULL;

}

static int
spinbox_prop_activate(
    AB_PROP_TYPE type,
    BOOL	 active
)
{
    ui_set_active(prop_spinbox_settings_rec[type].prop_sheet, active);

    return OK;
}


static int
spinbox_prop_clear(
    AB_PROP_TYPE type
)
{
    PropSpinboxSettingsRec	*pss = &(prop_spinbox_settings_rec[type]);

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

    /* Clear Spinbox Type */
    prop_radiobox_set_value(&(pss->spinbox_type),
                            (XtPointer)AB_TEXT_NUMERIC, False);

    /* Clear Arrow Style*/
    prop_options_set_value(&(pss->arrow_style), (XtPointer)AB_ARROW_END, False);

    /* Clear Min/Max/Incr */
    prop_field_set_value(&(pss->min), "", False);
    prop_field_set_value(&(pss->max), "", False);
    prop_field_set_value(&(pss->incr), "", False);

    /* Clear Initial Value/Decimal Points */
    prop_field_set_value(&(pss->decimal), "", False);
    prop_field_set_value(&(pss->ivalue), "", False);

    /* Clear Geometry */
    prop_geomfield_clear(&(pss->geometry), GEOM_X);
    prop_geomfield_clear(&(pss->geometry), GEOM_Y);
    prop_geomfield_clear(&(pss->geometry), GEOM_WIDTH);

    /* Clear Initial State */
    prop_checkbox_set_value(&(pss->init_state), AB_STATE_VISIBLE, True, False);
    prop_checkbox_set_value(&(pss->init_state), AB_STATE_ACTIVE, True, False);

    /* Clear Color */
    prop_colorfield_set_value(&(pss->bg_color), "", False);
    prop_colorfield_set_value(&(pss->fg_color), "", False);

    /* Clear Items */
    prop_item_editor_clear(&(pss->items));

    pss->current_obj = NULL;

    turnoff_changebars(type);

    return OK;
}

static int
spinbox_prop_load(
    ABObjPtr 	 obj,
    AB_PROP_TYPE type,
    unsigned long loadkey
)
{
    PropSpinboxSettingsRec 	*pss = &(prop_spinbox_settings_rec[type]);
    AB_TEXT_TYPE	   	spinbox_type;
    BOOL			load_all = (loadkey & LoadAll);

    if (obj == NULL)
    {
        if (pss->current_obj != NULL)
            obj = pss->current_obj;
        else
            return ERROR;
    }
    else if (!obj_is_spin_box(obj))
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

	/* Load Spinbox Type */
	spinbox_type = obj_get_text_type(obj);
	prop_radiobox_set_value(&(pss->spinbox_type),
                                (XtPointer)spinbox_type, False);
	setup_content_settings(type, spinbox_type);

	/* Load Arrow Style */
	prop_options_set_value(&(pss->arrow_style), (XtPointer)obj_get_arrow_style(obj), False);

	/* Load Min/Max/Incr */
	prop_field_set_numeric_value(&(pss->min), obj_get_min_value(obj), False);
	prop_field_set_numeric_value(&(pss->max), obj_get_max_value(obj), False);
	prop_field_set_numeric_value(&(pss->incr), obj_get_increment(obj), False);

	/* Load Initial Value/Decimal Points  */
	prop_field_set_numeric_value(&(pss->ivalue),
				obj_get_initial_value_int(obj), False);
	prop_field_set_numeric_value(&(pss->decimal),
		obj_get_decimal_points(obj), False);

	/* Load Initial State */
	prop_checkbox_set_value(&(pss->init_state), AB_STATE_VISIBLE,
		obj_is_initially_visible(obj), False);
	prop_checkbox_set_value(&(pss->init_state), AB_STATE_ACTIVE,
		obj_is_initially_active(obj), False);

	/* Load Color */
	prop_colorfield_set_value(&(pss->bg_color), obj_get_bg_color(obj), False);
	prop_colorfield_set_value(&(pss->fg_color), obj_get_fg_color(obj), False);

	/* Load Items */
	prop_item_editor_load(&(pss->items), obj);

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
spinbox_prop_apply(
    AB_PROP_TYPE   type
)
{
    PropSpinboxSettingsRec 	*pss = &(prop_spinbox_settings_rec[type]);
    AB_TEXT_TYPE		spinbox_type;
    STRING			value;
    int				new_w;
    BOOL			size_chg = False;
    BOOL	    		reset_bg = False;
    BOOL	    		reset_fg = False;

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
    if (prop_changed(pss->spinbox_type.changebar))
    {
	spinbox_type = (AB_TEXT_TYPE)prop_radiobox_get_value(&(pss->spinbox_type));

	if (spinbox_type == AB_TEXT_NUMERIC &&
	    obj_get_text_type(pss->current_obj) == AB_TEXT_DEFINED_STRING)
	{
	     int		num_items;
	     DTB_MODAL_ANSWER	answer = DTB_ANSWER_NONE;

	    XtVaGetValues(pss->items.item_list, XmNitemCount, &num_items, NULL);
	    if (num_items > 0)
	    {
            	/* Popup Modal Message and wait for answer */
		dtb_spinbox_chg_type_msg_initialize(&
			dtb_spinbox_chg_type_msg);
            	answer = dtb_show_modal_message(
				pss->spinbox_type.radiobox,
				&dtb_spinbox_chg_type_msg,
				NULL, NULL, NULL);
            	if (answer == DTB_ANSWER_CANCEL)
	   	    return 0;
	    	else
		{
		    /* Destroy any existing StringList items */
    	   	    prop_item_editor_clear(&(pss->items));
		    prop_set_changebar(pss->items.changebar, PROP_CB_ON);
		}
	    }
	}
	abobj_set_text_type(pss->current_obj, spinbox_type);
    }
    if (prop_changed(pss->arrow_style.changebar))
    {
        abobj_set_arrow_style(pss->current_obj,
                (AB_ARROW_STYLE)prop_options_get_value(&(pss->arrow_style)));
    }
    if (prop_changed(pss->min.changebar))
    {
	abobj_set_min_max_values(pss->current_obj,
		prop_field_get_numeric_value(&(pss->min)),
		prop_field_get_numeric_value(&(pss->max)));
        abobj_set_increment(pss->current_obj,
                prop_field_get_numeric_value(&(pss->incr)));

    }
    if (prop_changed(pss->ivalue.changebar))
    {
	abobj_set_initial_value(pss->current_obj, NULL,
		prop_field_get_numeric_value(&(pss->ivalue)));
        abobj_set_decimal_points(pss->current_obj,
                prop_field_get_numeric_value(&(pss->decimal)));
    }
    if (prop_changed(pss->geometry.changebar))
    {
	if (abobj_width_resizable(pss->current_obj))
	{
	    new_w = prop_geomfield_get_value(&(pss->geometry), GEOM_WIDTH);
	    abobj_set_pixel_width(pss->current_obj, new_w, 0);
	    size_chg = True;
	}
	if (abobj_is_movable(pss->current_obj))
	    abobj_set_xy(pss->current_obj,
	     	prop_geomfield_get_value(&(pss->geometry), GEOM_X),
	     	prop_geomfield_get_value(&(pss->geometry), GEOM_Y));

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
    if (prop_changed(pss->items.changebar))
    {
        prop_item_change(&(pss->items), False);
        prop_item_editor_apply(&(pss->items));
        size_chg = True;
    }
    abobj_tree_instantiate_changes(pss->current_obj);

    if (reset_bg || reset_fg) /* Set back to No Color */
        abobj_reset_colors(pss->current_obj, reset_bg, reset_fg);
    if (size_chg)
	abobj_force_dang_form_resize(pss->current_obj);

    turnoff_changebars(type);

    return OK;
}

static BOOL
spinbox_prop_pending(
    AB_PROP_TYPE type
)
{
    return(prop_changebars_pending(prop_spinbox_settings_rec[type].prop_sheet));
}

static BOOL
verify_props(
    AB_PROP_TYPE type
)
{
    PropSpinboxSettingsRec *pss = &(prop_spinbox_settings_rec[type]);
    int 		    min, max;

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

    if (!prop_number_ok(pss->ivalue.field, "Initial Value Field", min, max) ||
	!prop_number_ok(pss->decimal.field, "Decimal Field", 0, 99))
	return False;

    if (prop_changed(pss->geometry.changebar) &&
        (!prop_number_ok(pss->geometry.x_field, (STRING)XFieldStr, -SHRT_MAX, SHRT_MAX) ||
         !prop_number_ok(pss->geometry.y_field, (STRING)YFieldStr, -SHRT_MAX, SHRT_MAX) ||
         !prop_number_ok(pss->geometry.w_field, (STRING)WFieldStr, 1, SHRT_MAX)))
        return False;

    if (prop_changed(pss->fg_color.changebar) &&
	!prop_color_ok(pss->fg_color.field))
        return False;

    if (prop_changed(pss->bg_color.changebar) &&
	!prop_color_ok(pss->bg_color.field))
        return False;

    return True;
}

static void
turnoff_changebars(
    AB_PROP_TYPE type
)
{
    PropSpinboxSettingsRec *pss = &(prop_spinbox_settings_rec[type]);

    prop_set_changebar(pss->name.changebar,    		PROP_CB_OFF);
    prop_set_changebar(pss->spinbox_type.changebar,	PROP_CB_OFF);
    prop_set_changebar(pss->geometry.changebar,	 	PROP_CB_OFF);
    prop_set_changebar(pss->label_type.changebar,	PROP_CB_OFF);
    prop_set_changebar(pss->label.changebar,     	PROP_CB_OFF);
    prop_set_changebar(pss->arrow_style.changebar, 	PROP_CB_OFF);
    prop_set_changebar(pss->min.changebar, 	 	PROP_CB_OFF);
    prop_set_changebar(pss->ivalue.changebar, 	 	PROP_CB_OFF);
    prop_set_changebar(pss->init_state.changebar,	PROP_CB_OFF);
    prop_set_changebar(pss->bg_color.changebar,  	PROP_CB_OFF);
    prop_set_changebar(pss->fg_color.changebar,  	PROP_CB_OFF);
    prop_set_changebar(pss->items.changebar,  	 	PROP_CB_OFF);

    prop_changebars_cleared(pss->prop_sheet);
}

static void
spinbox_typeCB(
    Widget   	item,
    XtPointer 	clientdata,
    XmToggleButtonCallbackStruct *state
)
{
    AB_PROP_TYPE	   type = (AB_PROP_TYPE)clientdata;
    PropSpinboxSettingsRec *pss = &(prop_spinbox_settings_rec[type]);
    int    		   value;
    int			   num_items;

    if (state->set)
    {
        XtVaGetValues(item, XmNuserData, &value, NULL);
	setup_content_settings(type, (AB_TEXT_TYPE)value);

	if (value == AB_TEXT_DEFINED_STRING)
	{
	    /* If type is changed to StringList and there are no items,
	     * inactivate the item settings.
	     */
            XtVaGetValues(pss->items.item_list, XmNitemCount, &num_items, NULL);
	    if (num_items < 1)
	    {
            	ui_field_set_editable(pss->item_label.field, False);
            	ui_set_active(pss->item_label.label, False);
            	ui_set_active(pss->item_state.checkbox, False);
	    }
	}
    }
}

static void
setup_content_settings(
    AB_PROP_TYPE	type,
    AB_TEXT_TYPE	spinbox_type
)
{
    PropSpinboxSettingsRec *pss = &(prop_spinbox_settings_rec[type]);

    switch(spinbox_type)
    {
	case AB_TEXT_NUMERIC:
	    ui_set_active(pss->valrange_label, True);
	    ui_set_active(pss->min.field, True);
	    ui_set_active(pss->min.label, True);
	    ui_set_active(pss->max.field, True);
	    ui_set_active(pss->max.label, True);
	    ui_set_active(pss->incr.field, True);
	    ui_set_active(pss->incr.label, True);
	    ui_set_active(pss->decimal.field, True);
	    ui_set_active(pss->decimal.label, True);
	    ui_set_active(pss->ivalue.field, True);
	    ui_set_active(pss->ivalue.label, True);

	    ui_set_active(pss->item_list_label, False);
	    ui_set_active(pss->items.item_list, False);
	    ui_set_active(pss->item_add_button, False);
	    ui_set_active(pss->item_edit_button, False);
	    ui_field_set_editable(pss->item_label.field, False);
	    ui_set_active(pss->item_label.label, False);
	    ui_set_active(pss->item_state.checkbox, False);
	    break;
	case AB_TEXT_DEFINED_STRING:
	    ui_set_active(pss->valrange_label, False);
	    ui_set_active(pss->min.field, False);
	    ui_set_active(pss->min.label, False);
	    ui_set_active(pss->max.field, False);
	    ui_set_active(pss->max.label, False);
	    ui_set_active(pss->incr.field, False);
	    ui_set_active(pss->incr.label, False);
	    ui_set_active(pss->decimal.field, False);
	    ui_set_active(pss->decimal.label, False);
            ui_set_active(pss->ivalue.field, False);
            ui_set_active(pss->ivalue.label, False);

	    ui_set_active(pss->item_list_label, True);
	    ui_set_active(pss->items.item_list, True);
	    ui_set_active(pss->item_add_button, True);
	    ui_set_active(pss->item_edit_button, True);
	    ui_field_set_editable(pss->item_label.field, True);
	    ui_set_active(pss->item_label.label, True);
	    ui_set_active(pss->item_state.checkbox, True);
	    break;
    }
}

static int
spinbox_install_obj_destroy_CB(void)
{
    static BOOL	callback_installed = False;

    if (callback_installed)
	return 0;

    obj_add_destroy_callback(spinbox_obj_destroy_CB, "SpinBox Props");
    return 0;
}


static int
spinbox_obj_destroy_CB(ObjEvDestroyInfo destroyInfo)
{
    int			i;
    ABObj		doomedObj = destroyInfo->obj;
    PropSpinboxSettingsRec *pss;

    if (!obj_is_spin_box_item(doomedObj))
	return 0;

    for (i = 0; i < AB_PROP_TYPE_NUM_VALUES; ++i)
    {
	pss = &(prop_spinbox_settings_rec[i]);

	if (pss->current_obj == doomedObj)
	    pss->current_obj = NULL;
	if (pss->items.current_item == doomedObj)
	    pss->items.current_item = NULL;
	if (pss->items.clipboard_item == doomedObj)
	    pss->items.clipboard_item = NULL;
    }
    return 0;
}

