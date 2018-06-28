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
 *	$XConsortium: pal_combobox.c /main/5 1996/08/08 17:52:08 mustafa $
 *
 *	@(#)pal_combobox.c	1.1 23 Feb 1994	
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
 * pal_combobox.c - Implements Palette ComboBox object functionality
 */
#include <stdint.h>
#include <stdio.h>
#include <Xm/Xm.h>
#include <ab_private/obj_notify.h>
#include <ab_private/ab.h>
#include <ab_private/pal.h>
#include <ab_private/prop.h>
#include <ab_private/abobj.h>
#include <ab_private/abobj_set.h>
#include <ab_private/ui_util.h>
#include "combobox_ui.h"

const    int    combo_box_init_width  = 100;

static const	int input_rows = 3;

typedef struct  PROP_COMBOBOX_SETTINGS
{
    Widget                      prop_sheet;
    PropFieldSettingRec         name;
    PropRadioSettingRec         combobox_type;
    PropGeometrySettingRec      pos;
    PropRadioSettingRec         wth_policy;
    PropGeometrySettingRec      wth;
    PropOptionsSettingRec       label_type;
    PropFieldSettingRec         label;
    PropOptionsSettingRec       label_pos;
    PropCheckboxSettingRec      init_state;
    PropColorSettingRec         bg_color;
    PropColorSettingRec         fg_color;
    PropFieldSettingRec         item_label;
    PropCheckboxSettingRec	item_state;
    PropItemsSettingRec         items;
    ABObj                       current_obj;
} PropComboboxSettingsRec, *PropComboboxSettings;

/*************************************************************************
**                                                                      **
**       Private Function Declarations                                  **
**                                                                      **
**************************************************************************/
/*
 * Methods
 */
static int      combo_box_initialize(
                    ABObj   obj
                );
static Widget   combo_box_prop_init(
                    Widget  parent,
                    AB_PROP_TYPE type
                );
static int	combo_box_prop_activate(
		    AB_PROP_TYPE type,
		    BOOL 	 active
		);
static int	combo_box_prop_clear(
		    AB_PROP_TYPE type
		);
static int      combo_box_prop_load(
                    ABObj   	 obj,
                    AB_PROP_TYPE type,
  		    unsigned long loadkey
                );
static int	combo_box_prop_apply(
		    AB_PROP_TYPE type
		);
static BOOL     combo_box_prop_pending(
                    AB_PROP_TYPE type
                );

static BOOL	verify_props(
		    AB_PROP_TYPE type
		);
static void	turnoff_changebars(
		    AB_PROP_TYPE type
		);

/*
 * ABObj Callbacks
 */
static int	combobox_install_obj_destroy_CB(void);

static int	combobox_obj_destroy_CB(
		    ObjEvDestroyInfo destroyInfo
		);

/*
 * Xt Callbacks
 */
static void	wth_policyCB(
		    Widget   	widget,
                    XtPointer 	clientdata,
                    XmToggleButtonCallbackStruct *state
                );

/*************************************************************************
**                                                                      **
**       Data  		                                                 **
**                                                                      **
**************************************************************************/
PalItemInfo combo_box_palitem_rec = {

    /* type             */  AB_TYPE_COMBO_BOX,
    /* name             */  "Combo Box",
    /* animation pixmaps*/  NULL,
    /* number of pixmaps*/  0,
    /* rev_prop_frame   */  NULL,
    /* fix_prop_dialog  */  NULL,
    /* initialize       */  combo_box_initialize,
    /* is_a_test        */  obj_is_combo_box,
    /* prop_initialize  */  combo_box_prop_init,
    /* prop_activate	*/  combo_box_prop_activate,
    /* prop_clear	*/  combo_box_prop_clear,
    /* prop_load        */  combo_box_prop_load,
    /* prop_apply	*/  combo_box_prop_apply,
    /* prop_pending	*/  combo_box_prop_pending

};

PalItemInfo *ab_combo_box_palitem = &combo_box_palitem_rec;
PropComboboxSettingsRec prop_combobox_settings_rec[AB_PROP_TYPE_NUM_VALUES];


/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/
static int
combo_box_initialize(
    ABObj    obj
)
{
    ABObj	iobj;
    String  items[2];
    int         i;

    obj_set_unique_name(obj, "combobox");

    obj_set_label(obj, catgets(Dtb_project_catd, 100, 255, "ComboBox:"));
    obj_set_label_position(obj, AB_CP_WEST);
    obj_set_height(obj, -1);

    /* Add initial items to list */
    items[0] = catgets(Dtb_project_catd, 6, 70, "itemA");
    items[1] = catgets(Dtb_project_catd, 6, 71, "itemB");
    for (i=0; i < XtNumber(items); i++)
    {
        iobj = obj_create(AB_TYPE_ITEM, NULL);
        obj_set_subtype(iobj, AB_ITEM_FOR_COMBO_BOX);
        obj_append_child(obj, iobj);
        abobj_set_item_name(iobj, obj_get_module(obj), obj_get_name(obj), items[i]);
        obj_set_label(iobj, items[i]);
        obj_set_is_initially_selected(iobj, i==0? True : False);
    }
    obj_set_attachment(obj, AB_CP_NORTH, AB_ATTACH_POINT, NULL, obj->y);
    obj_set_attachment(obj, AB_CP_WEST,  AB_ATTACH_POINT, NULL, obj->x);

    return OK;
}

static Widget
combo_box_prop_init(
    Widget parent,
    AB_PROP_TYPE type
)
{
    DtbComboboxPropDialogInfoRec rev_combobox_prop_dialog; /* Revolving Props */
    DtbComboboxPropDialogInfo   cgen = &dtb_combobox_prop_dialog; /* Codegen structure */
    DtbRevolvPropDialogInfo	rpd = &(dtb_revolv_prop_dialog);
    PropComboboxSettingsRec    	*pcs = &(prop_combobox_settings_rec[type]);
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
        dtbComboboxPropDialogInfo_clear(&rev_combobox_prop_dialog);

	cgen = &(rev_combobox_prop_dialog);
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
        cgen->ok_button = rpd->ok_button;
        cgen->cancel_button = rpd->cancel_button;
	cgen->apply_button = rpd->apply_button; 
	cgen->reset_button = rpd->reset_button; 
	cgen->help_button = rpd->help_button; 

    	/*
     	 * Get notification of object destruction!
     	 */  
    	combobox_install_obj_destroy_CB();

    }
    else /* AB_PROP_FIXED */
    	cgen = &dtb_combobox_prop_dialog; 

    if (dtb_combobox_prop_dialog_initialize(cgen, parent) == 0)
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
            prop_fixed_dialog_init(ab_combo_box_palitem,
			cgen->prop_dialog_shellform, cgen->objlist);
            prop_activate_panel_init(type, ab_combo_box_palitem, 
                        cgen->ok_button, cgen->apply_button, 
                        cgen->reset_button, cgen->cancel_button,
                        cgen->help_button);
	}

        /* Alternate Editor Buttons */
        prop_editors_panel_init(type, ab_combo_box_palitem,
            cgen->attach_button, cgen->conn_button, cgen->helptxt_button);

        /*
          * Prop Sheet Settings....
         */

        /* Name */
        prop_field_init(&(pcs->name), cgen->name_field_label,
                cgen->name_field, cgen->name_cb);

        /* Label, Type, Position */
        n = 0;
        item[n] = cgen->labeltype_rbox_items.String_item;
        item_val[n] = AB_LABEL_STRING; n++;
        item[n] = cgen->labeltype_rbox_items.Graphic_item;
        item_val[n] = AB_LABEL_GLYPH; n++;
        prop_options_init(&(pcs->label_type), cgen->labeltype_rbox_label,
                       	cgen->labeltype_rbox, cgen->labeltype_rbox_menu,
			n, item, (XtPointer*)item_val,
                        cgen->labeltype_cb);

        prop_field_init(&(pcs->label), cgen->label_field_label,
                cgen->label_field, cgen->label_cb);

	prop_label_field_init(&(pcs->label), cgen->graphic_hint, item, n);

        n = 0;
        item[n] = cgen->labelpos_rbox_items.Above_item;
        item_val[n] = AB_CP_NORTH; n++;
        item[n] = cgen->labelpos_rbox_items.Left_item;
        item_val[n] = AB_CP_WEST; n++;
        prop_options_init(&(pcs->label_pos), cgen->labelpos_rbox_label,
                	cgen->labelpos_rbox, cgen->labelpos_rbox_menu,
                	n, item, (XtPointer*)item_val,
                	cgen->labeltype_cb);

        /* Read Only */
        n = 0;
        item[n] = cgen->comboboxtype_rbox_items.Static_item;
        item_val[n] = True; n++;
        item[n] = cgen->comboboxtype_rbox_items.Editable_item;
        item_val[n] = False; n++;
        prop_radiobox_init(&(pcs->combobox_type), cgen->comboboxtype_rbox_label,
                       cgen->comboboxtype_rbox, n, item, (XtPointer*)item_val,
                       cgen->comboboxtype_cb);

        /* Position */
        prop_geomfield_init(&(pcs->pos), cgen->pos_label,
                cgen->x_field_label, cgen->x_field,
                cgen->y_field_label, cgen->y_field,
                NULL, NULL,
                NULL, NULL,
                cgen->pos_cb);

	/* Width */
        n = 0;
        item[n] = cgen->wpolicy_rbox_items.Longest_Item_item;
        item_val[n] = SIZE_OF_CONTENTS_KEY; n++;
        item[n] = cgen->wpolicy_rbox_items.Fixed_item;
        item_val[n] = SIZE_FIXED_KEY; n++;
        prop_radiobox_init(&(pcs->wth_policy), cgen->wpolicy_rbox_label,
                cgen->wpolicy_rbox, n, item, (XtPointer*)item_val,
                cgen->wpolicy_cb);

        for(i=0; i < n; i++)
            XtAddCallback(item[i], XmNvalueChangedCallback,
                        (XtCallbackProc)prop_size_policyCB, (XtPointer)&(pcs->wth));

        prop_geomfield_init(&(pcs->wth), cgen->wpolicy_rbox_label,
                            NULL, NULL, NULL, NULL,
                            cgen->w_field_label, cgen->w_field,
                            NULL, NULL,
                            cgen->wpolicy_cb);

        /* Initial State */
        n = 0;
        item[n] = cgen->istate_ckbox_items.Visible_item;
        item_val[n] = AB_STATE_VISIBLE; n++;
        item[n] = cgen->istate_ckbox_items.Active_item;
        item_val[n] = AB_STATE_ACTIVE; n++;
        prop_checkbox_init(&(pcs->init_state), cgen->istate_ckbox_label,
                cgen->istate_ckbox, n, item, item_val,
                cgen->istate_cb);

        /* Color */
        prop_colorfield_init(&(pcs->bg_color), cgen->bg_mbutton, 
		cgen->bg_mbutton_bg_mbutton_menu_items.None_item,
		cgen->bg_mbutton_bg_mbutton_menu_items.Color_Chooser_item,
                cgen->bg_swatch, cgen->bg_field, cgen->bg_cb);

        prop_colorfield_init(&(pcs->fg_color), cgen->fg_mbutton,
                cgen->fg_mbutton_fg_mbutton_menu_items.None_item,  
                cgen->fg_mbutton_fg_mbutton_menu_items.Color_Chooser_item, 
                cgen->fg_swatch, cgen->fg_field, cgen->fg_cb); 

	/* Item Editor....*/

	/* Item Label */
        prop_field_init(&(pcs->item_label), cgen->itemlabel_label,
                cgen->itemlabel_field, cgen->itemlist_cb);

        /* Item State */
        n = 0;
        item[n] = cgen->item_state_ckbox_items.Selected_item;
        item_val[n] = AB_STATE_SELECTED; n++;
        prop_checkbox_init(&(pcs->item_state), NULL,
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

	prop_item_editor_init(&(pcs->items), AB_ITEM_FOR_COMBO_BOX,
		cgen->itemlist, cgen->itemlist_cb, 
		cgen->item_add_button,
		n, item, item_val, 
		j, item2, item2_val,
		&(pcs->item_label), NULL/*label_type*/, NULL/*graphic_hint*/,
		NULL/*mnemonic*/, NULL/*accel*/, NULL/*line_style*/,
		&(pcs->item_state), NULL,
		&(pcs->current_obj));

        prop_changebars_cleared(pcs->prop_sheet);

	return (cgen->prop_dialog_shellform);
    }
    else
    	return NULL;
}

static int
combo_box_prop_activate(
    AB_PROP_TYPE type,
    BOOL	 active
)
{
    ui_set_active(prop_combobox_settings_rec[type].prop_sheet, active);

    return OK;
}


static int
combo_box_prop_clear(
    AB_PROP_TYPE type
)
{
    PropComboboxSettingsRec	*pcs = &(prop_combobox_settings_rec[type]);

    if (pcs->current_obj == NULL)
	return OK;

    /* Clear Name */
    prop_field_set_value(&(pcs->name), "", False);

    /* Clear Label Type/Position */
    prop_options_set_value(&(pcs->label_type), (XtPointer)AB_LABEL_STRING, False);
    prop_options_set_value(&(pcs->label_pos), (XtPointer)AB_CP_WEST, False);

    /* Clear Label */
    ui_set_label_string(pcs->label.label, "Label:");
    prop_field_set_value(&(pcs->label), "", False);

    /* Clear Read Only */ 
    prop_radiobox_set_value(&(pcs->combobox_type), (XtPointer)True, False);

    /* Clear Position */
    prop_geomfield_clear(&(pcs->pos), GEOM_X);
    prop_geomfield_clear(&(pcs->pos), GEOM_Y);

    /* Clear Width */
    prop_radiobox_set_value(&(pcs->wth_policy), (XtPointer)SIZE_OF_CONTENTS_KEY, False);
    prop_geomfield_clear(&(pcs->wth), GEOM_WIDTH);

    /* Clear Initial State */
    prop_checkbox_set_value(&(pcs->init_state), AB_STATE_VISIBLE, True, False);
    prop_checkbox_set_value(&(pcs->init_state), AB_STATE_ACTIVE, True, False);

    /* Clear Color */
    prop_colorfield_set_value(&(pcs->bg_color), "", False);
    prop_colorfield_set_value(&(pcs->fg_color), "", False);

    /* Clear Items */
    prop_item_editor_clear(&(pcs->items));

    pcs->current_obj = NULL;

    turnoff_changebars(type);

    return OK;
}

static int
combo_box_prop_load(
    ABObjPtr 	 obj,
    AB_PROP_TYPE type,
    unsigned long loadkey
)
{
    PropComboboxSettingsRec 	*pcs = &(prop_combobox_settings_rec[type]);
    BOOL		    	editable;
    BOOL                        load_all = (loadkey & LoadAll);
 
    if (obj == NULL)
    {
        if (pcs->current_obj != NULL)
            obj = pcs->current_obj;
        else
            return ERROR;
    }
    else if (!obj_is_combo_box(obj))
        return ERROR;
    else
        pcs->current_obj = obj;

    /* Load Name */
    if (load_all || loadkey & LoadName)
    	prop_field_set_value(&(pcs->name), obj_get_name(obj), False);

    if (load_all)
    {
	/* Load Label Type/Position */
	prop_options_set_value(&(pcs->label_type), (XtPointer)obj->label_type, False);
	prop_options_set_value(&(pcs->label_pos), (XtPointer)obj_get_label_position(obj), False);

	/* Load Label */
	prop_setup_label_field(&(pcs->label), NULL,
				obj->label_type, obj_get_label(obj), AB_LINE_UNDEF);

	/* Load Read Only */
	prop_radiobox_set_value(&(pcs->combobox_type),
				(XtPointer)(intptr_t) obj_get_read_only(obj), False);

	/* Load Initial State */
	prop_checkbox_set_value(&(pcs->init_state), AB_STATE_VISIBLE,
		obj_is_initially_visible(obj), False);
	prop_checkbox_set_value(&(pcs->init_state), AB_STATE_ACTIVE,
		obj_is_initially_active(obj), False);

    	/* Load Color */
    	prop_colorfield_set_value(&(pcs->bg_color), obj_get_bg_color(obj), False); 
    	prop_colorfield_set_value(&(pcs->fg_color), obj_get_fg_color(obj), False);

    	/* Load Items */
    	prop_item_editor_load(&(pcs->items), obj);
 
    	turnoff_changebars(type);
    }

    /* Load Position */
    if (load_all || loadkey & LoadPosition)
        prop_load_obj_position(obj, &(pcs->pos)); 

    /* Load Width */
    if (load_all || loadkey & LoadSize)
    {
        editable = abobj_width_resizable(obj);
        prop_radiobox_set_value(&(pcs->wth_policy),
                                editable? (XtPointer)SIZE_FIXED_KEY :
                                (XtPointer)SIZE_OF_CONTENTS_KEY, False);

        ui_set_active(pcs->wth.w_field, editable);
        ui_set_active(pcs->wth.w_label, editable);
        prop_geomfield_set_value(&(pcs->wth), GEOM_WIDTH,
                abobj_get_comp_width(obj), False);
    }

    return OK;
}

int
combo_box_prop_apply(
    AB_PROP_TYPE   type
)
{
    PropComboboxSettingsRec 	*pcs = &(prop_combobox_settings_rec[type]);
    STRING			value;
    int				width_policy, new_w;
    BOOL			size_chg = False;
    BOOL	    		reset_bg = False;
    BOOL	    		reset_fg = False;

    if (!verify_props(type))
        return ERROR;

    if (prop_changed(pcs->name.changebar))
    {
        value = prop_field_get_value(&(pcs->name));   
        abobj_set_name(pcs->current_obj, value);
        util_free(value);
    }
    if (prop_changed(pcs->label.changebar) || 
	prop_changed(pcs->label_type.changebar))
    {
        value = prop_field_get_value(&(pcs->label));

        abobj_set_label(pcs->current_obj,
            (AB_LABEL_TYPE)prop_options_get_value(&(pcs->label_type)),
                        value);
        util_free(value);

        abobj_set_label_position(pcs->current_obj,
                (AB_COMPASS_POINT)prop_options_get_value(&(pcs->label_pos)));

	size_chg = True;
    }
    if (prop_changed(pcs->combobox_type.changebar))
    {
        abobj_set_read_only(pcs->current_obj,
            (BOOL)prop_radiobox_get_value(&(pcs->combobox_type)));
    }
    if (prop_changed(pcs->pos.changebar))
    {
	if (abobj_is_movable(pcs->current_obj))
	    abobj_set_xy(pcs->current_obj,
	     	prop_geomfield_get_value(&(pcs->pos), GEOM_X),
	     	prop_geomfield_get_value(&(pcs->pos), GEOM_Y));
    }
    if (prop_changed(pcs->wth_policy.changebar))
    {
	width_policy = prop_radiobox_get_value(&(pcs->wth_policy));
	abobj_set_size_policy(pcs->current_obj, width_policy == SIZE_FIXED_KEY);

	if (width_policy == SIZE_FIXED_KEY)
	{
	    new_w = prop_geomfield_get_value(&(pcs->wth), GEOM_WIDTH);
	    abobj_set_pixel_width(pcs->current_obj, new_w, 0);
	}
	size_chg = True;
    }
    if (prop_changed(pcs->init_state.changebar))
    {
        abobj_set_visible(pcs->current_obj, 
		prop_checkbox_get_value(&(pcs->init_state), AB_STATE_VISIBLE));
        abobj_set_active(pcs->current_obj,
		prop_checkbox_get_value(&(pcs->init_state), AB_STATE_ACTIVE));
    }
    if (prop_changed(pcs->fg_color.changebar))
    {
        value = prop_colorfield_get_value(&(pcs->fg_color));   
        abobj_set_foreground_color(pcs->current_obj, value);
	if (util_strempty(value))
	    reset_fg = True;
        util_free(value);
        prop_colorfield_set_value(&(pcs->fg_color), obj_get_fg_color(pcs->current_obj),
False);
    }
    if (prop_changed(pcs->bg_color.changebar))
    {
        value = prop_colorfield_get_value(&(pcs->bg_color));   
        abobj_set_background_color(pcs->current_obj, value);
	if (util_strempty(value))
	    reset_bg = True;
        util_free(value);
        prop_colorfield_set_value(&(pcs->bg_color), obj_get_bg_color(pcs->current_obj),
False);
    }
    if (prop_changed(pcs->items.changebar))
    {
        prop_item_change(&(pcs->items), False);
        prop_item_editor_apply(&(pcs->items));
        size_chg = True;
    }
    abobj_tree_instantiate_changes(pcs->current_obj);

    if (reset_bg || reset_fg) /* Set back to No Color */ 
        abobj_reset_colors(pcs->current_obj, reset_bg, reset_fg); 
    if (size_chg)
	abobj_force_dang_form_resize(pcs->current_obj);

    turnoff_changebars(type);

    return OK;
}

static BOOL
combo_box_prop_pending(
    AB_PROP_TYPE type
)
{
    return(prop_changebars_pending(prop_combobox_settings_rec[type].prop_sheet));
}

static BOOL
verify_props(
    AB_PROP_TYPE type
)
{
    PropComboboxSettingsRec *pcs = &(prop_combobox_settings_rec[type]);

    if (prop_changed(pcs->name.changebar) && !prop_name_ok(pcs->current_obj,
pcs->name.field))
        return False;

    if ((prop_changed(pcs->label_type.changebar) || prop_changed(pcs->label.changebar)) &&
	prop_options_get_value(&(pcs->label_type)) == (XtPointer)AB_LABEL_GLYPH &&
        !prop_graphic_filename_ok(pcs->label.field, False))
        return False;
 
    if (prop_changed(pcs->pos.changebar) &&
        (!prop_number_ok(pcs->pos.x_field, (STRING)XFieldStr, -SHRT_MAX, SHRT_MAX) ||
         !prop_number_ok(pcs->pos.y_field, (STRING)YFieldStr, -SHRT_MAX, SHRT_MAX)))
	return False;

    if (prop_changed(pcs->wth.changebar) &&
        !prop_number_ok(pcs->wth.w_field, (STRING)WFieldStr, 1, SHRT_MAX))
        return False;

    if (prop_changed(pcs->fg_color.changebar) && !prop_color_ok(pcs->fg_color.field))
        return False;
 
    if (prop_changed(pcs->bg_color.changebar) && !prop_color_ok(pcs->bg_color.field))
        return False;

    return True;
}

static void
turnoff_changebars(
    AB_PROP_TYPE type
)
{
    PropComboboxSettingsRec *pcs = &(prop_combobox_settings_rec[type]);

    prop_set_changebar(pcs->name.changebar,    		PROP_CB_OFF);
    prop_set_changebar(pcs->combobox_type.changebar,	PROP_CB_OFF);
    prop_set_changebar(pcs->pos.changebar,	 	PROP_CB_OFF);
    prop_set_changebar(pcs->wth_policy.changebar,	PROP_CB_OFF);
    prop_set_changebar(pcs->label_type.changebar,	PROP_CB_OFF);
    prop_set_changebar(pcs->label.changebar,     	PROP_CB_OFF);
    prop_set_changebar(pcs->init_state.changebar,	PROP_CB_OFF);
    prop_set_changebar(pcs->bg_color.changebar,  	PROP_CB_OFF);
    prop_set_changebar(pcs->fg_color.changebar,  	PROP_CB_OFF);
    prop_set_changebar(pcs->items.changebar,  	 	PROP_CB_OFF);

    prop_changebars_cleared(pcs->prop_sheet);
}

static int
combobox_install_obj_destroy_CB(void)
{
    static BOOL	callback_installed = False;

    if (callback_installed)
	return 0;

    obj_add_destroy_callback(combobox_obj_destroy_CB, "ComboBox Props");
    return 0;
}


static int
combobox_obj_destroy_CB(ObjEvDestroyInfo destroyInfo)
{
    int			i;
    ABObj		doomedObj = destroyInfo->obj;
    PropComboboxSettingsRec *pcs;

    if (!obj_is_combo_box_item(doomedObj))
	return 0;

    for (i = 0; i < AB_PROP_TYPE_NUM_VALUES; ++i)
    {
	pcs = &(prop_combobox_settings_rec[i]);

	if (pcs->current_obj == doomedObj)
	    pcs->current_obj = NULL;
	if (pcs->items.current_item == doomedObj)
	    pcs->items.current_item = NULL;
	if (pcs->items.clipboard_item == doomedObj)
	    pcs->items.clipboard_item = NULL;
    }
    return 0;
}

static void
wth_policyCB(
    Widget	w,
    XtPointer	clientdata,
    XmToggleButtonCallbackStruct *state
)
{
    AB_PROP_TYPE        	type = (AB_PROP_TYPE)clientdata;
    PropComboboxSettingsRec     *pcs = &(prop_combobox_settings_rec[type]);
    XtArgVal			value;

    /* Width field should ONLY be editable if Size Policy
     * is "Fixed"
     */
    if (state->set)
    {
    	XtVaGetValues(w, XmNuserData, &value, NULL);
    	ui_field_set_editable(pcs->wth.w_field, value == SIZE_FIXED_KEY);
    }
}

