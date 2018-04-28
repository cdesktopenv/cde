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
 *      $XConsortium: pal_custdlg.c /main/5 1996/08/08 17:59:53 mustafa $
 *
 * @(#)pal_custdlg.c	1.40 15 Feb 1994      cde_app_builder/src/ab
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
 * pal_custdlg.c - Implements Palette CustomDialog object functionality
 */
#include <stdint.h>
#include <stdio.h>
#include <Xm/Xm.h>
#include <ab_private/util.h>
#include <ab_private/ab.h>
#include <ab_private/pal.h>
#include <ab_private/proj.h>
#include <ab_private/prop.h>
#include <ab_private/abobj.h>
#include <ab_private/abobj_set.h>
#include <ab_private/ui_util.h>
#include "custdlg_ui.h"


const int    custdlg_init_height = 175;
const int    custdlg_init_width  = 380;

typedef struct  PROP_CUSTDLG_SETTINGS
{
    Widget                      prop_sheet;
    PropObjOptionsSettingRec    win_parent;
    PropFieldSettingRec         name;
    PropFieldSettingRec         title;
    PropRadioSettingRec         size_policy;
    PropGeometrySettingRec      geometry;
    PropRadioSettingRec         resize_mode;
    PropCheckboxSettingRec      areas;
    PropObjOptionsSettingRec	default_but;
    PropObjOptionsSettingRec	help_but;
    PropCheckboxSettingRec      init_state;
    PropColorSettingRec         bg_color;
    PropColorSettingRec         fg_color;
    ABObj                       current_obj;
} PropCustdlgSettingsRec, *PropCustdlgSettings;

/*************************************************************************
**                                                                      **
**       Private Function Declarations                                  **
**                                                                      **
**************************************************************************/
/*
 * Methods
 */
static int	custdlg_initialize(
                    ABObj   obj
                );
static Widget   custdlg_prop_init(
                    Widget  parent,
                    AB_PROP_TYPE type
                );
static int	custdlg_prop_activate(
		    AB_PROP_TYPE type,
		    BOOL 	 active
		);
static int	custdlg_prop_clear(
                    AB_PROP_TYPE type
                );
static int      custdlg_prop_load(
                    ABObj   	 obj,
                    AB_PROP_TYPE type,
 		    unsigned long loadkey
                );
static int	custdlg_prop_apply(
		    AB_PROP_TYPE type
		);
static BOOL     custdlg_prop_pending(
                    AB_PROP_TYPE type
                );

static BOOL	verify_props(
		    AB_PROP_TYPE type
		);
static void	turnoff_changebars(
		    AB_PROP_TYPE type
		);

static void	custdlg_create_area(
		    ABObj		obj,
		    AB_CONTAINER_TYPE	area_type
		);
static void	custdlg_create_buttons(
		    ABObj		obj,
		    ABObj		area
		);
static void	custdlg_remove_area(
                    ABObj               obj,
		    ABObj		area
                );
static BOOL	button_test_func(
		    ABObj		obj
		);
static BOOL	winparent_test_func(
		    ABObj		obj
		);



/*************************************************************************
**                                                                      **
**       Data                                                   	**
**                                                                      **
**************************************************************************/

PalItemInfo custdlg_palitem_rec = {

    /* type             */  AB_TYPE_DIALOG,
    /* name             */  "Custom Dialog",
    /* animation pixmaps*/  NULL,
    /* number of pixmaps*/  0,
    /* rev_prop_frame   */  NULL,
    /* fix_prop_dialog  */  NULL,
    /* initialize       */  custdlg_initialize,
    /* is_a_test        */  obj_is_popup_win,
    /* prop_initialize  */  custdlg_prop_init,
    /* prop_activate	*/  custdlg_prop_activate,
    /* prop_clear	*/  custdlg_prop_clear,
    /* prop_load        */  custdlg_prop_load,
    /* prop_apply	*/  custdlg_prop_apply,
    /* prop_pending	*/  custdlg_prop_pending

};

PalItemInfo *ab_custdlg_palitem = &custdlg_palitem_rec;
PropCustdlgSettingsRec prop_custdlg_settings_rec[AB_PROP_TYPE_NUM_VALUES];


/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/
static int
custdlg_initialize(
    ABObj     obj
)
{
    ABObj	module = obj_get_module(obj);
    ABObj	win;
    ABObj	apobj;
    int		num_wins;
    int		i;

    /* Find MainWindow to be Custom Dialog's Motif parent.
     * If none exist, window-parent will remain NULL
     */
    num_wins = obj_get_num_children(module);
    for (i = 0; i < num_wins; i++)
    {
	win = obj_get_child(module, i);
	if (obj_is_base_win(win))
	{
	    obj_set_win_parent(obj, win);
	    break;
	}
    }
    obj_set_unique_name(obj, "dialog");

    obj_set_label(obj, catgets(Dtb_project_catd, 100, 256, "Custom Dialog"));
    obj_set_bg_color(obj, "white");
    obj_set_is_initially_visible(obj, False);
    obj_set_is_initially_active(obj, True);
    obj_set_width(obj, custdlg_init_width);
    obj_set_height(obj, custdlg_init_height);

    /* Create Default Button Panel */
    apobj = obj_create(AB_TYPE_CONTAINER, obj);
    obj_set_subtype(apobj, AB_CONT_BUTTON_PANEL);
    pal_initialize_obj(apobj);

    /* Create Default Buttons for Button Panel */
    custdlg_create_buttons(obj, apobj);

    return OK;

}

static Widget
custdlg_prop_init(
    Widget parent,
    AB_PROP_TYPE type
)
{
    DtbCustdlgPropDialogInfoRec	rev_custdlg_prop_dialog; /* Revolving Props */
    DtbCustdlgPropDialogInfo    cgen = &dtb_custdlg_prop_dialog; /* Codegen structure */
    DtbRevolvPropDialogInfo	rpd = &(dtb_revolv_prop_dialog);
    PropCustdlgSettingsRec    	*pcs = &(prop_custdlg_settings_rec[type]);
    Widget            		item[6];
    int            		item_val[6];
    int            		i, n;

    if (type == AB_PROP_REVOLVING)
    {
	/* Cloning Trick:
	 * Only the Attributes ControlPanel needs to be created within
	 * the existing Revolving Prop dialog, so fill out all other
	 * fields with the Revolving Prop dialog equivelents, so the
	 * dtb initialize proc will skip those non-NULL fields...
	 */
        dtbCustdlgPropDialogInfo_clear(&rev_custdlg_prop_dialog);

	cgen = &(rev_custdlg_prop_dialog);
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
    	cgen = &dtb_custdlg_prop_dialog;

    if (dtb_custdlg_prop_dialog_initialize(cgen, parent) == 0)
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
            prop_fixed_dialog_init(ab_custdlg_palitem,
			cgen->prop_dialog_shellform, cgen->objlist);
            prop_activate_panel_init(type, ab_custdlg_palitem,
                        cgen->ok_button, cgen->apply_button,
                        cgen->reset_button, cgen->cancel_button,
                        cgen->help_button);
	}

        /* Alternate Editor Buttons */
        prop_editors_panel_init(type, ab_custdlg_palitem,
            NULL, cgen->conn_button, cgen->helptxt_button);

        /*
          * Prop Sheet Settings....
         */

        /* Name */
        prop_field_init(&(pcs->name), cgen->name_field_label,
                cgen->name_field, cgen->name_cb);

        /* Window Parent */
        n = 0;
        item[n] = cgen->winparent_opmenu_items.None_item;
        item_val[n] = 0; n++;
        prop_obj_options_init(&(pcs->win_parent), cgen->winparent_opmenu_label,
                cgen->winparent_opmenu, cgen->winparent_opmenu_menu,
                n, item, (XtPointer*)item_val,
                cgen->winparent_cb, True, /* Display module */
                &(pcs->current_obj), winparent_test_func);

        /* Title */
        prop_field_init(&(pcs->title), cgen->title_field_label, cgen->title_field,
                cgen->title_cb);

        /* Resize Mode */
        n = 0;
        item[n] = cgen->resizemode_rbox_items.Adjustable_item;
        item_val[n] = True; n++;
        item[n] = cgen->resizemode_rbox_items.Fixed_item;
        item_val[n] = False; n++;
        prop_radiobox_init(&(pcs->resize_mode), cgen->resizemode_rbox_label,
                cgen->resizemode_rbox, n, item, (XtPointer*)item_val,
                cgen->resizemode_cb);

        /* Window Areas */
        n = 0;
        n = 0;
        item[n] = cgen->areas_ckbox_items.Button_Panel_item;
        item_val[n] = AB_CONT_BUTTON_PANEL; n++;
        item[n] = cgen->areas_ckbox_items.Footer_item;
        item_val[n] = AB_CONT_FOOTER; n++;
  	prop_checkbox_init(&(pcs->areas), cgen->areas_ckbox_label,
		cgen->areas_ckbox, n, item, item_val,
		cgen->areas_cb);

 	/* Default Button */
        n = 0;
        item[n] = cgen->defaultb_opmenu_items.None_item;
        item_val[n] = 0; n++;
	prop_obj_options_init(&(pcs->default_but), cgen->defaultb_opmenu_label,
		cgen->defaultb_opmenu, cgen->defaultb_opmenu_menu,
		n, item, (XtPointer*)item_val,
		cgen->default_button_cb, False, /* don't display module */
		&(pcs->current_obj), button_test_func);

 	/* Help Button */
        n = 0;
        item[n] = cgen->helpb_opmenu_items.None_item;
        item_val[n] = 0; n++;
	prop_obj_options_init(&(pcs->help_but), cgen->helpb_opmenu_label,
		cgen->helpb_opmenu, cgen->helpb_opmenu_menu,
		n, item, (XtPointer*)item_val,
		cgen->help_button_cb, False, /* don't display module */
		&(pcs->current_obj), button_test_func);

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

        /* Geometry */
        prop_geomfield_init(&(pcs->geometry), cgen->geom_label,
		NULL, NULL, NULL, NULL,
                cgen->width_field_label, cgen->width_field,
                cgen->height_field_label, cgen->height_field,
                cgen->geom_cb);

        /* Initial State */
        n = 0;
        item[n] = cgen->istate_ckbox_items.Visible_item;
        item_val[n] = AB_STATE_VISIBLE; n++;
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

        prop_changebars_cleared(pcs->prop_sheet);

	return (cgen->prop_dialog_shellform);
    }
    else
    	return NULL;

}

static int
custdlg_prop_activate(
    AB_PROP_TYPE type,
    BOOL	 active
)
{
     ui_set_active(prop_custdlg_settings_rec[type].prop_sheet, active);

    return OK;
}


static int
custdlg_prop_clear(
    AB_PROP_TYPE type
)
{
    PropCustdlgSettingsRec	*pcs = &(prop_custdlg_settings_rec[type]);

    if (pcs->current_obj == NULL)
	return OK;

    /* Clear Name */
    prop_field_set_value(&(pcs->name), "", False);

    /* Clear Window Parent */
    prop_obj_options_load(&(pcs->win_parent), NULL);

    /* Clear Title */
    prop_field_set_value(&(pcs->title), "", False);

    /* Clear Resize Mode */
    prop_radiobox_set_value(&(pcs->resize_mode), (XtPointer)True, False);

    /* Clear Window Areas */
    prop_checkbox_set_value(&(pcs->areas), AB_CONT_BUTTON_PANEL, False, False);
    prop_checkbox_set_value(&(pcs->areas), AB_CONT_FOOTER, False, False);

    /* Clear Default Button */
    prop_obj_options_load(&(pcs->default_but), NULL);

    /* Clear Help Button */
    prop_obj_options_load(&(pcs->help_but), NULL);

    /* Clear Size Policy */
    prop_radiobox_set_value(&(pcs->size_policy), (XtPointer)SIZE_FIXED_KEY, False);

    /* Clear Size */
    prop_geomfield_clear(&(pcs->geometry), GEOM_WIDTH);
    prop_geomfield_clear(&(pcs->geometry), GEOM_HEIGHT);

    /* Clear Initial State */
    prop_checkbox_set_value(&(pcs->init_state), AB_STATE_VISIBLE, True, False);

    /* Clear Color */
    prop_colorfield_set_value(&(pcs->bg_color), "", False);
    prop_colorfield_set_value(&(pcs->fg_color), "", False);

    pcs->current_obj = NULL;

    turnoff_changebars(type);

    return OK;

}

static int
custdlg_prop_load(
    ABObj    	 obj,
    AB_PROP_TYPE type,
    unsigned long loadkey
)
{
    ABObj			area;
    PropCustdlgSettingsRec 	*pcs = &(prop_custdlg_settings_rec[type]);
    BOOL                        load_all = (loadkey & LoadAll);

    if (obj == NULL)
    {
        if (pcs->current_obj != NULL)
            obj = pcs->current_obj;
        else
            return ERROR;
    }
    else if (!obj_is_popup_win(obj))
        return ERROR;
    else
        pcs->current_obj = obj;

    /* Load Name */
    if (load_all || loadkey & LoadName)
    	prop_field_set_value(&(pcs->name), obj_get_name(obj), False);

    if (load_all)
    {
	/* Load Window Parent */
	prop_obj_options_load(&(pcs->win_parent), proj_get_project());
	prop_obj_options_set_value(&(pcs->win_parent),
	obj_get_win_parent(obj), False);

	/* Load Title */
	prop_field_set_value(&(pcs->title), obj_get_label(obj), False);

	/* Load Resize Mode */
	prop_radiobox_set_value(&(pcs->resize_mode),
                                (XtPointer)(intptr_t) obj_get_resizable(obj), False);

	/* Load Window Areas */
	area = objxm_comp_custdlg_get_area(obj, AB_CONT_BUTTON_PANEL);
	prop_checkbox_set_value(&(pcs->areas), AB_CONT_BUTTON_PANEL,
	area != NULL, False);

	area = objxm_comp_custdlg_get_area(obj, AB_CONT_FOOTER);
	prop_checkbox_set_value(&(pcs->areas), AB_CONT_FOOTER,
		area != NULL, False);

	/* Load Default Button */
	prop_obj_options_load(&(pcs->default_but),
	objxm_comp_custdlg_get_area(obj, AB_CONT_BUTTON_PANEL));
	prop_obj_options_set_value(&(pcs->default_but),
	obj_get_default_act_button(obj), False);

	/* Load Help Button */
	prop_obj_options_load(&(pcs->help_but),
	objxm_comp_custdlg_get_area(obj, AB_CONT_BUTTON_PANEL));
	prop_obj_options_set_value(&(pcs->help_but),
	obj_get_help_act_button(obj), False);

	/* Load Size Policy */
	prop_radiobox_set_value(&(pcs->size_policy),
				abobj_width_resizable(obj)?
                                (XtPointer)SIZE_FIXED_KEY :
                                (XtPointer)SIZE_OF_CONTENTS_KEY, False);

	/* Load Initial State */
	prop_checkbox_set_value(&(pcs->init_state), AB_STATE_VISIBLE,
		obj_is_initially_visible(obj), False);

	/* Load Color */
	prop_colorfield_set_value(&(pcs->bg_color), obj_get_bg_color(obj), False);
	prop_colorfield_set_value(&(pcs->fg_color), obj_get_fg_color(obj), False);

    	turnoff_changebars(type);
    }

    /* Load Size */
    if (load_all || loadkey & LoadSize)
        prop_load_obj_size(obj, &(pcs->geometry));

    return OK;
}

int
custdlg_prop_apply(
    AB_PROP_TYPE   type
)
{
    PropCustdlgSettingsRec 	*pcs = &(prop_custdlg_settings_rec[type]);
    ABObj			module;
    ABObj			area;
    STRING			value;
    BOOL			area_set;
    BOOL			reset_bg = False;
    BOOL			reset_fg = False;
    BOOL			size_chg = False;
    int				new_w, new_h;

    if (!verify_props(type))
        return ERROR;

    if (prop_changed(pcs->name.changebar))
    {
        value = prop_field_get_value(&(pcs->name));
        abobj_set_name(pcs->current_obj, value);
        util_free(value);
    }
    if (prop_changed(pcs->win_parent.options.changebar))
    {
	ABObj	win_parent = (ABObj) NULL;

	win_parent = prop_obj_options_get_value(&(pcs->win_parent));
	abobj_set_win_parent(pcs->current_obj, win_parent);
    }
    if (prop_changed(pcs->title.changebar))
    {
        value = prop_field_get_value(&(pcs->title));
        abobj_set_label(pcs->current_obj, pcs->current_obj->label_type, value);
        util_free(value);
    }
    if (prop_changed(pcs->resize_mode.changebar))
    {
        abobj_set_resize_mode(pcs->current_obj,
		(BOOL)prop_radiobox_get_value(&(pcs->resize_mode)));
    }
    if (prop_changed(pcs->areas.changebar))
    {
	module = obj_get_module(pcs->current_obj);

	/* Button Panel */
	area = objxm_comp_custdlg_get_area(pcs->current_obj, AB_CONT_BUTTON_PANEL);
	area_set = prop_checkbox_get_value(&(pcs->areas), AB_CONT_BUTTON_PANEL);

	if (area != NULL && !area_set) /* Remove Button Panel */
	    custdlg_remove_area(pcs->current_obj, area);

	else if (area == NULL && area_set) /* Add Button Panel */
	    custdlg_create_area(pcs->current_obj, AB_CONT_BUTTON_PANEL);

        /* Footer */
        area = objxm_comp_custdlg_get_area(pcs->current_obj, AB_CONT_FOOTER);
        area_set = prop_checkbox_get_value(&(pcs->areas), AB_CONT_FOOTER);

        if (area != NULL && !area_set) /* Remove Footer */
            custdlg_remove_area(pcs->current_obj, area);

        else if (area == NULL && area_set) /* Add Footer */
            custdlg_create_area(pcs->current_obj, AB_CONT_FOOTER);
    }
    if (prop_changed(pcs->default_but.options.changebar))
    {
	abobj_set_default_act_button(pcs->current_obj,
	    prop_obj_options_get_value(&(pcs->default_but)));

    }
    if (prop_changed(pcs->help_but.options.changebar))
    {
	abobj_set_help_act_button(pcs->current_obj,
	    prop_obj_options_get_value(&(pcs->help_but)));

    }
    if (prop_changed(pcs->size_policy.changebar))
    {
	abobj_set_size_policy(pcs->current_obj,
		prop_radiobox_get_value(&(pcs->size_policy)) == SIZE_FIXED_KEY);

	size_chg = True;
    }
    if (prop_changed(pcs->geometry.changebar))
    {
    	if (abobj_width_resizable(pcs->current_obj))
	{
	    new_w = prop_geomfield_get_value(&(pcs->geometry), GEOM_WIDTH);
	    abobj_set_pixel_width(pcs->current_obj, new_w, 0);
	}
	if (abobj_height_resizable(pcs->current_obj))
	{
	    new_h = prop_geomfield_get_value(&(pcs->geometry), GEOM_HEIGHT);
	    abobj_set_pixel_height(pcs->current_obj, new_h, 0);
	}
    }
    if (prop_changed(pcs->init_state.changebar))
    {
        abobj_set_visible(pcs->current_obj,
		prop_checkbox_get_value(&(pcs->init_state), AB_STATE_VISIBLE));
    }
    if (prop_changed(pcs->fg_color.changebar))
    {
        value = prop_colorfield_get_value(&(pcs->fg_color));
        abobj_set_foreground_color(pcs->current_obj, value);
	if (util_strempty(value))
	    reset_fg = True;
        util_free(value);
        prop_colorfield_set_value(&(pcs->fg_color), obj_get_fg_color(pcs->current_obj), False);
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
    abobj_tree_instantiate_changes(pcs->current_obj);

    if (reset_bg || reset_fg) /* Set back to No Color */
        abobj_reset_colors(pcs->current_obj, reset_bg, reset_fg);

    turnoff_changebars(type);

    return OK;

}

static BOOL
custdlg_prop_pending(
    AB_PROP_TYPE type
)
{
    return(prop_changebars_pending(prop_custdlg_settings_rec[type].prop_sheet));
}

static BOOL
verify_props(
    AB_PROP_TYPE type
)
{
    PropCustdlgSettingsRec 	*pcs = &(prop_custdlg_settings_rec[type]);
    ABObj		   	area;
    BOOL		   	area_set;

    if (prop_changed(pcs->name.changebar) && !prop_name_ok(pcs->current_obj, pcs->name.field))
        return False;

    if (prop_changed(pcs->areas.changebar))
    {
   	DTB_MODAL_ANSWER	answer = DTB_ANSWER_NONE;
    	ABObj			pobj;

	/* Check Button Panel */
        area = objxm_comp_custdlg_get_area(pcs->current_obj, AB_CONT_BUTTON_PANEL);
        area_set = prop_checkbox_get_value(&(pcs->areas), AB_CONT_BUTTON_PANEL);
        if (area != NULL && !area_set) /* Remove Menubar */
        {
	    pobj = objxm_comp_get_subobj(area, AB_CFG_PARENT_OBJ);

	    if (obj_get_num_children(pobj) > 0)
	    {
                /* Popup Modal Message and wait for answer */
		dtb_custdlg_remove_bpanel_msg_initialize(
			&dtb_custdlg_remove_bpanel_msg);
                answer = dtb_show_modal_message(pcs->areas.checkbox,
				&dtb_custdlg_remove_bpanel_msg,
				NULL, NULL, NULL);
                if (answer == DTB_ANSWER_CANCEL)
                    return False;
	    }
	}

        area = objxm_comp_custdlg_get_area(pcs->current_obj, AB_CONT_FOOTER);
        area_set = prop_checkbox_get_value(&(pcs->areas), AB_CONT_FOOTER);
        if (area != NULL && !area_set) /* Remove Footer */
        {
            pobj = objxm_comp_get_subobj(area, AB_CFG_PARENT_OBJ);

            if (obj_get_num_children(pobj) > 0)
            {
                /* Popup Modal Message and wait for answer */
		dtb_custdlg_remove_footer_msg_initialize(
			&dtb_custdlg_remove_footer_msg);
                answer = dtb_show_modal_message(pcs->areas.checkbox,
				&dtb_custdlg_remove_footer_msg,
				NULL, NULL, NULL);
                if (answer == DTB_ANSWER_CANCEL)
                    return False;
	    }
        }
    }

    if (prop_changed(pcs->geometry.changebar) &&
         (!prop_number_ok(pcs->geometry.w_field, (STRING)WFieldStr, 1, SHRT_MAX) ||
         !prop_number_ok(pcs->geometry.h_field, (STRING)HFieldStr, 1, SHRT_MAX)))
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
    PropCustdlgSettingsRec *pcs = &(prop_custdlg_settings_rec[type]);

    prop_set_changebar(pcs->name.changebar,    		PROP_CB_OFF);
    prop_set_changebar(pcs->win_parent.options.changebar,PROP_CB_OFF);
    prop_set_changebar(pcs->title.changebar,     	PROP_CB_OFF);
    prop_set_changebar(pcs->resize_mode.changebar,     	PROP_CB_OFF);
    prop_set_changebar(pcs->areas.changebar,     	PROP_CB_OFF);
    prop_set_changebar(pcs->default_but.options.changebar,PROP_CB_OFF);
    prop_set_changebar(pcs->help_but.options.changebar, PROP_CB_OFF);
    prop_set_changebar(pcs->size_policy.changebar,     	PROP_CB_OFF);
    prop_set_changebar(pcs->geometry.changebar,     	PROP_CB_OFF);
    prop_set_changebar(pcs->init_state.changebar,	PROP_CB_OFF);
    prop_set_changebar(pcs->bg_color.changebar,  	PROP_CB_OFF);
    prop_set_changebar(pcs->fg_color.changebar,  	PROP_CB_OFF);

    prop_changebars_cleared(pcs->prop_sheet);

}

static void
custdlg_create_area(
    ABObj		obj,
    AB_CONTAINER_TYPE	area_type
)
{
    ABObj	pwobj;
    ABObj	label, area;
    ABObj       workobj;
    Dimension   height;

    pwobj = objxm_comp_get_subobj(obj, AB_CFG_WINDOW_PW_OBJ);
    workobj = objxm_comp_get_subobj(obj, AB_CFG_PARENT_OBJ);

    area = obj_create(AB_TYPE_CONTAINER, pwobj);
    obj_set_subtype(area, area_type);
    pal_initialize_obj(area);

    if (area_type == AB_CONT_FOOTER)
    {
	label = obj_create(AB_TYPE_LABEL, area);
        obj_set_x(label, 1);
        obj_set_y(label, 1);
	pal_initialize_obj(label);
	obj_set_label(label, catgets(Dtb_project_catd, 100, 264, "footer message"));
	obj_set_label_alignment(label, AB_ALIGN_LEFT);
	obj_set_unique_name(label,
		ab_ident_from_name_and_label(obj_get_name(obj), "label"));

	/*
	 * Workaround part1: MainWindow bug that causes MainWindow to shrink
	 * when a MessageWindow area is added
	 */
	XtVaGetValues(objxm_get_widget(workobj),
		XmNheight,	&height,
		NULL);
    }
    else if (area_type == AB_CONT_BUTTON_PANEL)
	custdlg_create_buttons(obj, area);

    abobj_show_tree(area, True);

    /* Workaround part2 */
    if (area_type == AB_CONT_FOOTER)
	XtVaSetValues(objxm_get_widget(workobj),
		XmNheight,	height,
		NULL);

    abobj_set_save_needed(obj_get_module(obj), True);

}

static void
custdlg_create_buttons(
    ABObj	obj,
    ABObj	area
)
{
    ABObj	bobj;
    char    *button_names[] = {"button1","button2","button3"};
    char    *button_labels[3];
    int     startpos, endpos;
    int		i;

    button_labels[0] = catgets(Dtb_project_catd, 100, 265, "Button1");
    button_labels[1] = catgets(Dtb_project_catd, 100, 266, "Button2");
    button_labels[2] = catgets(Dtb_project_catd, 100, 267, "Button3");

    endpos = 0;
    for(i=0; i < XtNumber(button_names); i++)
    {
        bobj = obj_create(AB_TYPE_BUTTON, area);
        obj_set_subtype(bobj, AB_BUT_PUSH);
        obj_set_unique_name(bobj,
            ab_ident_from_name_and_label(obj_get_name(obj),button_names[i]));
        obj_set_label(bobj, button_labels[i]);
        obj_set_is_initially_visible(bobj, True);
        obj_set_is_initially_active(bobj, True);
        startpos = endpos + 10;
        obj_set_attachment(bobj, AB_CP_WEST, AB_ATTACH_GRIDLINE, (void*)(intptr_t) startpos, 0);
        endpos = startpos + 20;
        obj_set_attachment(bobj, AB_CP_EAST, AB_ATTACH_GRIDLINE, (void*)(intptr_t) endpos, 0);
	obj_set_attachment(bobj, AB_CP_NORTH, AB_ATTACH_POINT, NULL, 5);
    }

}

static void
custdlg_remove_area(
    ABObj       obj,
    ABObj	area
)
{
    objxm_tree_destroy(area);
    abobj_set_save_needed(obj_get_module(obj), True);
}

/*
 * Test whether a Button should be a candidate to be the dialog's
 * "Default" button or not
 */
static BOOL
button_test_func(
    ABObj	obj
)
{
    ABObj	parent = obj_get_root(obj_get_parent(obj));

    if (obj_is_button(obj))
    {
    	while (obj_is_group(parent))
            parent = obj_get_root(obj_get_parent(parent));

	if (obj_get_container_type(parent) == AB_CONT_BUTTON_PANEL)
	    return True;
    }
    return False;
}

/*
 * Test whether an object can be the "Window Parent" of the dialog
 */
static BOOL
winparent_test_func(
    ABObj       obj
)
{
    return(obj_is_base_win(obj));
}
