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
 *      $XConsortium: pal_menu.c /main/5 1996/08/08 18:02:46 mustafa $
 *
 * @(#)pal_menu.c	1.38 15 Feb 1994      cde_app_builder/src/ab
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
 * pal_menu.c - Implements Palette Menu object functionality
 * 		NOTE: menu not actually ON the main palette
 */
#include <stdint.h>
#include <stdio.h>
#include <Xm/Xm.h>
#include <ab_private/obj_notify.h>
#include <ab_private/trav.h>
#include <ab_private/objxm.h>
#include <ab_private/ab.h>
#include <ab_private/pal.h>
#include <ab_private/proj.h>
#include <ab_private/prop.h>
#include <ab_private/abobj.h>
#include <ab_private/abobj_set.h>
#include <ab_private/abobj_edit.h>
#include <ab_private/ui_util.h>
#include "menu_ui.h"


typedef struct  PROP_MENU_SETTINGS
{
    Widget                      prop_sheet;
    PropFieldSettingRec         name;
    PropRadioSettingRec         tearoff;
    Widget			color_label;
    PropColorSettingRec         bg_color;
    PropColorSettingRec         fg_color;
    Widget			item_list_label;
    Widget			item_list_button_group;
    PropFieldSettingRec         item_label;
    PropOptionsSettingRec       item_label_type;
    PropFieldSettingRec         item_mnemonic;
    PropFieldSettingRec         item_accel;
    PropOptionsSettingRec       item_line_style;
    PropCheckboxSettingRec      item_state;
    PropMenunameSettingRec      item_menuname;
    PropItemsSettingRec         items;
    Widget			button_group;
    Widget			edit_menu;
    ABObj                       current_obj;
    ABObj			clipboard_menu;
} PropMenuSettingsRec, *PropMenuSettings;

/*************************************************************************
**                                                                      **
**       Private Function Declarations                                  **
**                                                                      **
**************************************************************************/
/*
 * Methods
 */
static int      menu_initialize(
                    ABObj   obj
                );
static Widget   menu_prop_init(
                    Widget  parent,
		    AB_PROP_TYPE type
                );
static int	menu_prop_activate(
		    AB_PROP_TYPE type,
		    BOOL 	 active
		);
static int      menu_prop_clear(
                    AB_PROP_TYPE type
                );
static int      menu_prop_load(
                    ABObj   	 obj,
		    AB_PROP_TYPE type,
  		    unsigned long loadkey
                );
static int	menu_prop_apply(
		    AB_PROP_TYPE type
		);
static BOOL	menu_prop_pending(
		    AB_PROP_TYPE type
		);

static BOOL	verify_props(
		    AB_PROP_TYPE type
		);
static void	turnoff_changebars(
		    AB_PROP_TYPE type
		);

/*
 * Menu Clipboard Functions
 */
static void	edit_item_init(
    		    Widget       widget,
    		    AB_PROP_TYPE ptype,
    		    AB_EDIT_TYPE etype
		);
static void	copy_menu_to_clipboard(
    		    ABObj       mobj,
    		    ABObj       *menu_clipboard
		);

/*
 * ABObj callbacks
 */
static int	prop_menu_obj_destroy_CB(
		    ObjEvDestroyInfo destroyInfo
		);
static int 	prop_menu_install_obj_destroy_CB(void);

/*
 * Xt Callbacks
 */
static void	menu_createCB(
                    Widget      widget,
                    XtPointer   client_data,
                    XtPointer   call_data
                );
static void	menu_editCB(
                    Widget      widget,
                    XtPointer   client_data,
                    XtPointer   call_data
                );
static void	set_edit_menu_stateCB(
                    Widget      widget,
                    XtPointer   client_data,
                    XtPointer   call_data
                );

/*************************************************************************
**                                                                      **
**       Data  		                                                 **
**                                                                      **
**************************************************************************/
PalItemInfo menu_palitem_rec = {

    /* type             */  AB_TYPE_MENU,
    /* name             */  "Menu",
    /* animation pixmaps*/  NULL,
    /* number of pixmaps*/  0,
    /* rev_prop_frame   */  NULL,
    /* fix_prop_dialog  */  NULL,
    /* initialize       */  menu_initialize,
    /* is_a_test        */  obj_is_menu,
    /* prop_initialize  */  menu_prop_init,
    /* prop_activate	*/  menu_prop_activate,
    /* prop_clear	*/  menu_prop_clear,
    /* prop_load        */  menu_prop_load,
    /* prop_apply	*/  menu_prop_apply,
    /* prop_pending	*/  menu_prop_pending

};

PalItemInfo *ab_menu_palitem = &menu_palitem_rec;
PropMenuSettingsRec prop_menu_settings_rec[AB_PROP_TYPE_NUM_VALUES];


/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/
static int
menu_initialize(
    ABObj    obj
)
{
    STRING	items[2];
    ABObj	iobj;
    int		i;

    if (util_strempty(obj_get_name(obj)))
    {
    	obj_set_unique_name(obj, "menu");
    }

    obj_set_is_initially_active(obj, True);

    /* Add initial items to Choice */
    items[0] = catgets(Dtb_project_catd,   6, 136, "Item1");
    items[1] = catgets(Dtb_project_catd, 100, 259, "Item2");
    for (i=0; i < XtNumber(items); i++)
    {
        iobj = obj_create(AB_TYPE_ITEM, NULL);
        obj_append_child(obj, iobj);
        obj_set_subtype(iobj, AB_ITEM_FOR_MENU);
        iobj->label_type = AB_LABEL_STRING;
        obj_set_is_initially_active(iobj, True);
        abobj_set_item_name(iobj, obj_get_module(obj), obj_get_name(obj), items[i]);
        obj_set_label(iobj, items[i]);
    }

    return OK;
}

static Widget
menu_prop_init(
    Widget    parent,
    AB_PROP_TYPE type
)
{
    DtbMenuPropDialogInfoRec	rev_menu_prop_dialog; /* Revolving Props */
    DtbMenuPropDialogInfo	cgen = &dtb_menu_prop_dialog; /* Codegen structure */
    DtbRevolvPropDialogInfo	rpd = &(dtb_revolv_prop_dialog);
    PropMenuSettingsRec    	*pms = &(prop_menu_settings_rec[type]);
    Widget            		item[12];
    int            		item_val[12];
    Widget			item2[12];
    int				item2_val[12];
    int            		i, n, j;

    if (type == AB_PROP_REVOLVING)
    {
	/* Cloning Trick:
	 * Only the Attributes ControlPanel needs to be created within
	 * the existing Revolving Prop dialog, so fill out all other
	 * fields with the Revolving Prop dialog equivelents, so the
	 * dtb initialize proc will skip those non-NULL fields...
	 */
        dtbMenuPropDialogInfo_clear(&rev_menu_prop_dialog);

	cgen = &(rev_menu_prop_dialog);
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
    	prop_menu_install_obj_destroy_CB();

    }
    else /* AB_PROP_FIXED */
    	cgen = &dtb_menu_prop_dialog;

    if (dtb_menu_prop_dialog_initialize(cgen, parent) == 0)
    {
        pms->prop_sheet = cgen->attrs_ctrlpanel;
        pms->current_obj = NULL;
	pms->clipboard_menu = NULL;

	if (type == AB_PROP_REVOLVING)
            XtVaSetValues(parent,
                        XmNuserData, pms->current_obj,
                        NULL);

        /* Dialog/Object List */
	if (type == AB_PROP_FIXED)
	{
            prop_fixed_dialog_init(ab_menu_palitem,
			cgen->prop_dialog_shellform, cgen->objlist);
            prop_activate_panel_init(type, ab_menu_palitem,
                        cgen->ok_button, cgen->apply_button,
                        cgen->reset_button, cgen->cancel_button,
                        cgen->help_button);
	}

        /* Alternate Editor Buttons */
        prop_editors_panel_init(type, ab_menu_palitem,
            NULL, cgen->conn_button, cgen->helptxt_button);

        /*
          * Prop Sheet Settings....
         */
	/* New & Edit Buttons */
        XtAddCallback(cgen->newbutton, XmNactivateCallback,
		menu_createCB, (XtPointer)type);

	edit_item_init(cgen->menuedit_mbutton_menu_editmenu_items.Cut_item,
		type, AB_EDIT_CUT);
	edit_item_init(cgen->menuedit_mbutton_menu_editmenu_items.Copy_item,
		type, AB_EDIT_COPY);
	edit_item_init(cgen->menuedit_mbutton_menu_editmenu_items.Paste_item,
		type, AB_EDIT_PASTE);
	edit_item_init(cgen->menuedit_mbutton_menu_editmenu_items.Delete_item,
		type, AB_EDIT_DELETE);

	pms->edit_menu = cgen->menuedit_mbutton_menu_editmenu;
    	XtAddCallback(XtParent(pms->edit_menu), XtNpopupCallback,
		set_edit_menu_stateCB, (XtPointer)type);

        /* Name */
        prop_field_init(&(pms->name), cgen->name_field_label,
                cgen->name_field, cgen->name_cb);

        /* Tear-off */
        n = 0;
        item[n] = cgen->tearoff_rbox_items.Enabled_item;
        item_val[n] = True; n++;
        item[n] = cgen->tearoff_rbox_items.Disabled_item;
        item_val[n] = False; n++;
        prop_radiobox_init(&(pms->tearoff), cgen->tearoff_rbox_label,
                cgen->tearoff_rbox, n, item, (XtPointer*)item_val,
                cgen->tearoff_cb);

        /* Color */
        prop_colorfield_init(&(pms->bg_color), cgen->bg_mbutton,
		cgen->bg_mbutton_bg_mbutton_menu_items.None_item,
		cgen->bg_mbutton_bg_mbutton_menu_items.Color_Chooser_item,
                cgen->bg_swatch, cgen->bg_field, cgen->bg_cb);

        prop_colorfield_init(&(pms->fg_color), cgen->fg_mbutton,
                cgen->fg_mbutton_fg_mbutton_menu_items.None_item,
                cgen->fg_mbutton_fg_mbutton_menu_items.Color_Chooser_item,
                cgen->fg_swatch, cgen->fg_field, cgen->fg_cb);
	pms->color_label = cgen->color_label;

	/* Item Editor....*/

	pms->item_list_label = cgen->itemlist_label;
	pms->item_list_button_group = cgen->item_button_group;

	/* Item Label */
        prop_field_init(&(pms->item_label), cgen->itemlabel_field_label,
                cgen->itemlabel_field, cgen->itemlist_cb);
        n = 0;
        item[n] = cgen->itemlabeltype_opmenu_items.String_item;
        item_val[n] = AB_LABEL_STRING; n++;
        item[n] = cgen->itemlabeltype_opmenu_items.Graphic_item;
        item_val[n] = AB_LABEL_GLYPH; n++;
	item[n] = cgen->itemlabeltype_opmenu_items.Separator_item;
        item_val[n] = AB_LABEL_SEPARATOR; n++;
        prop_options_init(&(pms->item_label_type), cgen->itemlabeltype_opmenu_label,
                cgen->itemlabeltype_opmenu, cgen->itemlabeltype_opmenu_menu,
                n, item, (XtPointer*)item_val, cgen->itemlist_cb);

        for(i=0; i < n; i++)
            XtAddCallback(item[i], XmNactivateCallback,
                  (XtCallbackProc)prop_item_labeltypeCB, (XtPointer)&(pms->items));

	/* Item Mnemonic */
        prop_field_init(&(pms->item_mnemonic), cgen->mnemonic_field_label,
                cgen->mnemonic_field, cgen->itemlist_cb);

	/* Item Accelerator */
        prop_field_init(&(pms->item_accel), cgen->accel_field_label,
                cgen->accel_field, cgen->itemlist_cb);

	/* Item Line Style */
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
        prop_options_init(&(pms->item_line_style), cgen->linestyle_opmenu_label,
                cgen->linestyle_opmenu, cgen->linestyle_opmenu_menu,
                n, item, (XtPointer*)item_val,
                cgen->itemlist_cb);
 	prop_options_set_value(&(pms->item_line_style), (XtPointer)AB_LINE_ETCHED_OUT, False);

	/* Item State */
        n = 0;
        item[n] = cgen->itemstate_ckbox_items.Active_item;
        item_val[n] = AB_STATE_ACTIVE; n++;
        prop_checkbox_init(&(pms->item_state), cgen->itemstate_ckbox_label,
                cgen->itemstate_ckbox, n, item, item_val,
                cgen->itemlist_cb);

	/* Item Menu Name */
        prop_menuname_init(&(pms->item_menuname), type, cgen->label,
			cgen->item_menu_mbutton,
			cgen->item_menu_field,
                        cgen->itemlabel_field, cgen->itemlist_cb, NULL/*title*/,
			&(pms->current_obj), True);

	/* Store Items->Insert menu items in array */
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

	/* Hook up Item Editing mechanism to Item List*/
	prop_item_editor_init(&(pms->items), AB_ITEM_FOR_MENU,
		cgen->itemlist, cgen->itemlist_cb,
		cgen->item_add_button,
		n, item, item_val, /* Insert */
		j, item2, item2_val,/* Edit */
		&(pms->item_label), &(pms->item_label_type), cgen->graphic_hint,
		&(pms->item_mnemonic), &(pms->item_accel), &(pms->item_line_style),
		&(pms->item_state), &(pms->item_menuname),
		&(pms->current_obj));

 	pms->button_group = cgen->button_group;

        prop_changebars_cleared(pms->prop_sheet);

	return (cgen->prop_dialog_shellform);
    }
    else
    	return NULL;
}

static int
menu_prop_activate(
    AB_PROP_TYPE type,
    BOOL	 active
)
{
    /*
     * Since the "Add Menu" button must remain active at all times, we cannot
     * make the entire prop sheet inactive;  Therefore, we set sensitivity
     * on more specific controls in the prop sheet (not the entire Form).
     */

    ui_set_active(prop_menu_settings_rec[type].name.label, active);
    ui_set_active(prop_menu_settings_rec[type].name.field, active);

    ui_set_active(prop_menu_settings_rec[type].tearoff.label, active);
    ui_set_active(prop_menu_settings_rec[type].tearoff.radiobox, active);

    ui_set_active(prop_menu_settings_rec[type].item_list_label, active);
    ui_set_active(prop_menu_settings_rec[type].item_list_button_group, active);
    ui_set_active(prop_menu_settings_rec[type].item_label.label, active);
    ui_set_active(prop_menu_settings_rec[type].item_label.field, active);
    ui_set_active(prop_menu_settings_rec[type].item_label_type.label, active);
    ui_set_active(prop_menu_settings_rec[type].item_label_type.optionbox, active);
    ui_set_active(prop_menu_settings_rec[type].item_mnemonic.label, active);
    ui_set_active(prop_menu_settings_rec[type].item_mnemonic.field, active);
    ui_set_active(prop_menu_settings_rec[type].item_accel.label, active);
    ui_set_active(prop_menu_settings_rec[type].item_accel.field, active);
    ui_set_active(prop_menu_settings_rec[type].item_line_style.label, active);
    ui_set_active(prop_menu_settings_rec[type].item_line_style.optionbox, active);
    ui_set_active(prop_menu_settings_rec[type].item_state.label, active);
    ui_set_active(prop_menu_settings_rec[type].item_state.checkbox, active);
    ui_set_active(prop_menu_settings_rec[type].item_menuname.label, active);
    ui_set_active(prop_menu_settings_rec[type].item_menuname.menubutton, active);
    ui_set_active(prop_menu_settings_rec[type].item_menuname.field, active);
    ui_set_active(prop_menu_settings_rec[type].items.item_list, active);

    ui_set_active(prop_menu_settings_rec[type].color_label, active);
    ui_set_active(prop_menu_settings_rec[type].bg_color.menubutton, active);
    ui_set_active(prop_menu_settings_rec[type].bg_color.field, active);
    ui_set_active(prop_menu_settings_rec[type].fg_color.menubutton, active);
    ui_set_active(prop_menu_settings_rec[type].fg_color.field, active);

    ui_set_active(prop_menu_settings_rec[type].button_group, active);

    return OK;
}


static int
menu_prop_clear(
    AB_PROP_TYPE type
)
{
    PropMenuSettingsRec	*pms = &(prop_menu_settings_rec[type]);

    /* Clear Name */
    prop_field_set_value(&(pms->name), "", False);

    /* Clear Tearoff */
    prop_radiobox_set_value(&(pms->tearoff),  (XtPointer)False, False);

    /* Clear Background Color */
    prop_colorfield_set_value(&(pms->bg_color), "", False);

    /* Clear Foreground Color */
    prop_colorfield_set_value(&(pms->fg_color), "", False);

    /* Clear Items */
    prop_item_editor_clear(&(pms->items));

    pms->current_obj = NULL;

    turnoff_changebars(type);

    return OK;

}

static int
menu_prop_load(
    ABObjPtr 	 obj,
    AB_PROP_TYPE type,
    unsigned long loadkey
)
{
    PropMenuSettingsRec *pms = &(prop_menu_settings_rec[type]);
    BOOL		load_all = (loadkey & LoadAll);

    if (obj == NULL)
    {
        if (pms->current_obj != NULL)
            obj = pms->current_obj;
        else
            return ERROR;
    }
    else if (!obj_is_menu(obj))
        return ERROR;
    else
        pms->current_obj = obj;

    /* Load Name */
    if (load_all || loadkey & LoadName)
    	prop_field_set_value(&(pms->name), obj_get_name(obj), False);

    if (load_all)
    {
	/* Load Tearoff */
	prop_radiobox_set_value(&(pms->tearoff),
		(XtPointer)(intptr_t) (obj->info.menu.tear_off), False);

	/* Load Color */
	prop_colorfield_set_value(&(pms->bg_color), obj_get_bg_color(obj), False);
	prop_colorfield_set_value(&(pms->fg_color), obj_get_fg_color(obj), False);

	/* Load Items */
	prop_item_editor_load(&(pms->items), obj);

	turnoff_changebars(type);
    }

    return OK;
}

static int
menu_prop_apply(
    AB_PROP_TYPE   type
)
{
    PropMenuSettingsRec 	*pms = &(prop_menu_settings_rec[type]);
    STRING	    		value;
    BOOL	    		reset_bg = False;
    BOOL	    		reset_fg = False;

    if (!verify_props(type))
        return ERROR;

    if (prop_changed(pms->name.changebar))
    {
        value = prop_field_get_value(&(pms->name));
        abobj_set_name(pms->current_obj, value);
        util_free(value);
    }
    if (prop_changed(pms->tearoff.changebar))
    {
        abobj_set_tearoff(pms->current_obj, prop_radiobox_get_value(&(pms->tearoff)));
    }
    if (prop_changed(pms->fg_color.changebar))
    {
        value = prop_colorfield_get_value(&(pms->fg_color));
        abobj_set_foreground_color(pms->current_obj, value);
	if (util_strempty(value))
	    reset_fg = True;
        util_free(value);
        prop_colorfield_set_value(&(pms->fg_color),
		obj_get_fg_color(pms->current_obj), False);
    }
    if (prop_changed(pms->bg_color.changebar))
    {
        value = prop_colorfield_get_value(&(pms->bg_color));
        abobj_set_background_color(pms->current_obj, value);
	if (util_strempty(value))
	    reset_bg = True;
        util_free(value);
        prop_colorfield_set_value(&(pms->bg_color),
		obj_get_bg_color(pms->current_obj), False);
    }
    if (prop_changed(pms->items.changebar))
    {
        /* Ensure edits to current item are saved before apply */
        prop_item_change(&(pms->items), False);
        prop_item_editor_apply(&(pms->items));
    }
   /*
    * Menus are a SPECIAL CASE:
    * Menu objects are NOT instantiated during build-mode, therefore DO NOT
    * call abobj_instantiate_changes()
    */
    turnoff_changebars(type);

    return OK;
}

static BOOL
menu_prop_pending(
    AB_PROP_TYPE type
)
{
    return(prop_changebars_pending(prop_menu_settings_rec[type].prop_sheet));
}

static BOOL
verify_props(
    AB_PROP_TYPE type
)
{
    PropMenuSettingsRec *pms = &(prop_menu_settings_rec[type]);

    if (prop_changed(pms->name.changebar) &&
	!prop_name_ok(pms->current_obj, pms->name.field))
        return False;

    if (prop_changed(pms->fg_color.changebar) && !prop_color_ok(pms->fg_color.field))
        return False;

    if (prop_changed(pms->bg_color.changebar) && !prop_color_ok(pms->bg_color.field))
        return False;

    return True;
}


static void
turnoff_changebars(
    AB_PROP_TYPE type
)
{
    PropMenuSettingsRec *pms = &(prop_menu_settings_rec[type]);

    prop_set_changebar(pms->name.changebar,    		PROP_CB_OFF);
    prop_set_changebar(pms->tearoff.changebar,		PROP_CB_OFF);
    prop_set_changebar(pms->bg_color.changebar,  	PROP_CB_OFF);
    prop_set_changebar(pms->fg_color.changebar,  	PROP_CB_OFF);
    prop_set_changebar(pms->items.changebar,  	 	PROP_CB_OFF);

    prop_changebars_cleared(pms->prop_sheet);

}

static void
edit_item_init(
    Widget	 widget,
    AB_PROP_TYPE ptype,
    AB_EDIT_TYPE etype
)
{
    XtVaSetValues(widget, XmNuserData, (XtArgVal)etype, NULL);
    XtAddCallback(widget, XmNactivateCallback, menu_editCB, (XtPointer)ptype);
}

static void
set_edit_menu_stateCB(
    Widget      widget,
    XtPointer   client_data,
    XtPointer   call_data
)
{
    AB_PROP_TYPE	type = (AB_PROP_TYPE)client_data;
    PropMenuSettingsRec *pms = &(prop_menu_settings_rec[type]);
    BOOL		menu_exists = False;
    BOOL		clipboard_full = False;
    WidgetList		menu_items;
    int			num_menu_items;
    int			value;
    AB_EDIT_TYPE	item_key;
    int			i;

    menu_exists = (pms->current_obj != NULL);
    clipboard_full = (pms->clipboard_menu != NULL);

    XtVaGetValues(pms->edit_menu,
		XmNnumChildren,	&num_menu_items,
		XmNchildren,	&menu_items,
		NULL);

    /* Loop through the Edit menuitems, setting their sensitivity
     * according to the state of the Menu Props editor
     */
    for (i = 0; i < num_menu_items; i++)
    {
	XtVaGetValues(menu_items[i], XmNuserData, &value, NULL);
	item_key = (AB_EDIT_TYPE)value;

	switch(item_key)
  	{
	    case AB_EDIT_CUT:
	    case AB_EDIT_COPY:
	    case AB_EDIT_DELETE:
		ui_set_active(menu_items[i], menu_exists);
		break;
	    case AB_EDIT_PASTE:
		ui_set_active(menu_items[i], clipboard_full);
		break;
	    default:
		break;
		/* do nothing */
	}
    }
}


static void
copy_menu_to_clipboard(
    ABObj	mobj,
    ABObj	*menu_clipboard
)
{
    ABObj	old_selection;
    ABObj	new_selection;

    old_selection = *menu_clipboard;
    new_selection = obj_tree_dup(mobj);

    /* We must clear the MappedFlag so that the editors do not
     * think this menu (or immediately after a "paste", the menu copy)
     * is "shown".
     */
    obj_tree_clear_flag(new_selection, MappedFlag);

    *menu_clipboard = new_selection;

    if (old_selection != NULL)
	obj_destroy(old_selection);
}


int
pal_create_menu(
    AB_PROP_TYPE	type,
    ABObj		module,
    STRING		menu_name,
    STRING		menu_title
)
{
    PropMenuSettingsRec *pms = &(prop_menu_settings_rec[type]);
    ABObj        newmenu;

    if (module == NULL) /* No module created yet */
    {
	ABObj	proj = proj_get_project();

        module = obj_create(AB_TYPE_MODULE, proj);
    	abobj_set_save_needed(proj, True);
        obj_set_name(module, "module");
        abobj_show_tree(module, FALSE);
        proj_set_cur_module(module);
        proj_show_name_dlg(module, pms->prop_sheet);
    }
    newmenu = obj_create(AB_TYPE_MENU, module);
    abobj_set_save_needed(module, True);

    /* menu_name MUST already be unique! */
    obj_set_name(newmenu, menu_name);

    pal_initialize_obj(newmenu);
    abobj_show_tree(newmenu, False);

    prop_load_obj(newmenu, type);

    /* Set Focus directly to Item Label field */
    ui_field_select_string(pms->item_label.field, True);

    return 0;
}

static void
menu_createCB(
    Widget      widget,
    XtPointer   client_data,
    XtPointer   call_data
)
{
    AB_PROP_TYPE type = (AB_PROP_TYPE)client_data;
    STRING	name = NULL;
    char	nameBuf[5001] = "";

    name = obj_get_unique_name_for_child(
		proj_get_cur_module(), "menu", 5000, nameBuf);
    pal_create_menu(type, proj_get_cur_module(), name, "Menu");

}

static void
menu_editCB(
    Widget	widget,
    XtPointer   client_data,
    XtPointer   call_data
)
{
    AB_PROP_TYPE 	type = (AB_PROP_TYPE)client_data;
    PropMenuSettingsRec *pms = &(prop_menu_settings_rec[type]);
    char		namebuf[512];
    STRING		name;
    AB_EDIT_TYPE 	etype;
    ABObj		module;
    ABObj		newmenu;
    ABObj		delmenu;

    module = proj_get_cur_module();

    XtVaGetValues(widget, XmNuserData, &etype, NULL);

    switch(etype)
    {
	case AB_EDIT_CUT:
    	    if (pms->current_obj != NULL)
	    {
            	copy_menu_to_clipboard(pms->current_obj, &(pms->clipboard_menu));
	    	delmenu = pms->current_obj;
	    	obj_destroy(delmenu);
	    }
            break;
        case AB_EDIT_COPY:
	    if (pms->current_obj != NULL)
            	copy_menu_to_clipboard(pms->current_obj, &(pms->clipboard_menu));
            break;
        case AB_EDIT_PASTE:
            if (pms->clipboard_menu != NULL)
            {
		/* We need to use some trickery here in order to give the
		 * new copy of the menu an incremented/unique name based
		 * on the clipboard menu's name.  We must temporarily re-name
		 * the clipboard menu so that the dup'd copy won't get an
		 * identicle name as the clipboard menu (which in turn will
		 * cause the prop sheet mechanism to think the clipboard menu
		 * is being renamed, rather than a new menu being added).
		 * Trust me - this works.
		 */

		/* store off original clipboard menu name and temporarily
		 * set it to a dummy string.
		 */
		snprintf(namebuf, sizeof(namebuf), "%s", obj_get_name(pms->clipboard_menu));
		obj_set_name(pms->clipboard_menu, "MENU_DUMMY");

		/* duplicate clipboard menu & menu-items */
		newmenu = obj_tree_dup(pms->clipboard_menu);
		obj_append_child(proj_get_cur_module(), newmenu);

		/* Change name to be based on clipboard menu name instead
		 * of "MENU_DUMMY"; Note that abobj_set_name() will also
		 * take care of updating the menu-item names.
		 */
		name = obj_alloc_unique_name(newmenu, namebuf, -1);
		abobj_set_name(newmenu, name);

		/* reset the clipboard menu name back to original */
		obj_set_name(pms->clipboard_menu, namebuf);
		util_free(name);

    		abobj_show_tree(newmenu, False);
    		prop_load_obj(newmenu, type);
	    }
	    break;
        case AB_EDIT_DELETE:
	    if (pms->current_obj != NULL)
	    {
	    	delmenu = pms->current_obj;
            	obj_destroy(delmenu);
	    }
            break;
        default:
            /* catch-all case to avoid compiler warnings */
	    break;
    }
}


/*
 * Object destroy callback
 */
static int
prop_menu_install_obj_destroy_CB(void)
{
    static BOOL	callback_installed = False;

    if (callback_installed)
    {
	return 0;
    }
    obj_add_destroy_callback(prop_menu_obj_destroy_CB, "Menu Props");
    return 0;
}

static int
prop_menu_obj_destroy_CB(ObjEvDestroyInfo destroyInfo)
{
    PropMenuSettingsRec *pms;
    int			i;
    ABObj		doomedObj = destroyInfo->obj;
    ABObj		obj, item;
    AB_TRAVERSAL	trav;
    STRING		doomed_menu_name;

    if (!obj_is_menu_item(doomedObj) &&
	!obj_is_menu(doomedObj))
    {
	return 0;
    }

    for (i = 1; i < AB_PROP_TYPE_NUM_VALUES; i++)
    {
	pms = &(prop_menu_settings_rec[i]);

	if (pms->current_obj == doomedObj)
	    pms->current_obj = NULL;
	if (pms->items.current_item == doomedObj)
	    pms->items.current_item = NULL;
	if (pms->items.clipboard_item == doomedObj)
	    pms->items.clipboard_item = NULL;
    }

    if (obj_is_menu(doomedObj) && obj_get_parent(doomedObj) != NULL)
    {
	doomed_menu_name = obj_get_name(doomedObj);

	/* Look through the module to see if any objects refer to
	 * the menu being destroyed.  If so, set their menu-name attributes
	 * to NULL.
	 */
        for (trav_open(&trav, obj_get_module(doomedObj), AB_TRAV_SALIENT);
            (obj= trav_next(&trav)) != NULL; )
            if (util_strcmp(obj_get_menu_name(obj), doomed_menu_name) == 0)
	    	abobj_set_menu_name(obj, "");

        trav_close(&trav);

	for (i = 1; i < AB_PROP_TYPE_NUM_VALUES; i++)
	{
	    pms = &(prop_menu_settings_rec[i]);

	    /* Look to see if any of the clipboard menu item's refer to
	     * the menu being destroyed.  If so, set their menu-name attributes
	     * to NULL.
	     */
	    if (pms->clipboard_menu != NULL)
	    {
		for (trav_open(&trav, pms->clipboard_menu, AB_TRAV_ITEMS);
		    (item = trav_next(&trav)) != NULL; )
		    if (util_strcmp(obj_get_menu_name(item), doomed_menu_name) == 0)
			obj_set_menu_name(item, NULL);
	    }
	    trav_close(&trav);
	}
    }
    return 0;
}
