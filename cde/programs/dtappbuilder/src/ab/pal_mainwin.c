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
 *      $XConsortium: pal_mainwin.c /main/5 1996/08/08 18:02:22 mustafa $
 *
 * @(#)pal_mainwin.c	1.67 11 Aug 1995      cde_app_builder/src/ab
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
 * pal_mainwin.c - Implements Palette Main Window object functionality
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
#include "mainwin_ui.h"

const int    mainwin_init_height = 175;
const int    mainwin_init_width  = 400;

typedef struct  PROP_MAINWIN_SETTINGS
{
    Widget                      prop_sheet;
    PropFieldSettingRec         name;
    PropFieldSettingRec         title;
    PropRadioSettingRec         size_policy;
    PropGeometrySettingRec      geometry;
    PropRadioSettingRec         resize_mode;
    PropFieldSettingRec         icon;
    PropFieldSettingRec         icon_mask;
    PropFieldSettingRec         icon_label;
    PropCheckboxSettingRec      areas;
    PropCheckboxSettingRec      init_state;
    PropColorSettingRec         bg_color;
    PropColorSettingRec         fg_color;
    ABObj                       current_obj;
} PropMainwinSettingsRec, *PropMainwinSettings;

/*************************************************************************
**                                                                      **
**       Private Function Declarations                                  **
**                                                                      **
**************************************************************************/
/*
 * Methods
 */
static int	mainwin_initialize(
                    ABObj   obj
                );
static Widget   mainwin_prop_init(
                    Widget  parent,
		    AB_PROP_TYPE type
                );
static int	mainwin_prop_activate(
		    AB_PROP_TYPE type,
		    BOOL 	 active
		);
static int	mainwin_prop_clear(
		    AB_PROP_TYPE type
		);
static int      mainwin_prop_load(
                    ABObj   	 obj,
		    AB_PROP_TYPE type,
		    unsigned long loadkey
                );
static int	mainwin_prop_apply(
		    AB_PROP_TYPE type
		);

static BOOL	mainwin_prop_pending(
		    AB_PROP_TYPE type
		);

static BOOL	verify_props(
		    AB_PROP_TYPE type
		);
static void	turnoff_changebars(
		    AB_PROP_TYPE type
		);

static void	mainwin_create_area(
		    ABObj		obj,
		    AB_CONTAINER_TYPE	area_type
		);

static void	mainwin_remove_area(
                    ABObj               obj,
		    ABObj		area
                );


/*************************************************************************
**                                                                      **
**       Data                                                   	**
**                                                                      **
**************************************************************************/

PalItemInfo mainwin_palitem_rec = {

    /* type             */  AB_TYPE_BASE_WINDOW,
    /* name             */  "Main Window",
    /* animation pixmaps*/  NULL,
    /* number of pixmaps*/  0,
    /* rev_prop_frame   */  NULL,
    /* fix_prop_dialog  */  NULL,
    /* initialize       */  mainwin_initialize,
    /* is_a_test        */  obj_is_base_win,
    /* prop_initialize  */  mainwin_prop_init,
    /* prop_active	*/  mainwin_prop_activate,
    /* prop_clear	*/  mainwin_prop_clear,
    /* prop_load        */  mainwin_prop_load,
    /* prop_apply	*/  mainwin_prop_apply,
    /* prop_pending	*/  mainwin_prop_pending

};

PalItemInfo *ab_mainwin_palitem = &mainwin_palitem_rec;
PropMainwinSettingsRec prop_mainwin_settings_rec[AB_PROP_TYPE_NUM_VALUES];

/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/
static int
mainwin_initialize(
    ABObj     obj
)
{
    ABObj	project = proj_get_project();

    obj_set_unique_name(obj, "mainwindow");

    obj_set_label(obj, catgets(Dtb_project_catd, 100, 258, "Main Window"));
    obj_set_bg_color(obj, "white");
    obj_set_is_initially_visible(obj, True);
    obj_set_is_initially_iconic(obj, False);
    obj_set_is_initially_active(obj, True);
    obj->width       	= mainwin_init_width;
    obj->height       	= mainwin_init_height;

    /* If no Main Windows are currently designated as the
     * "Primary" MainWindow (root window), then set this
     * one to be it
     */
    if (obj_get_root_window(project) == NULL)
	obj_set_root_window(project, obj);

    return OK;
}

static Widget
mainwin_prop_init(
    Widget       parent,
    AB_PROP_TYPE type
)
{
    DtbMainwinPropDialogInfoRec	rev_mainwin_prop_dialog; /* Revolving Props */
    DtbMainwinPropDialogInfo	cgen;
    DtbRevolvPropDialogInfo	rpd = &(dtb_revolv_prop_dialog);
    PropMainwinSettingsRec      *pms = &(prop_mainwin_settings_rec[type]);
    Widget                      item[6];
    int                         item_val[6];
    int                         i, n;

    if (type == AB_PROP_REVOLVING)
    {
	/* Cloning Trick:
	 * Only the Attributes ControlPanel needs to be created within
	 * the existing Revolving Prop dialog, so fill out all other
	 * fields with the Revolving Prop dialog equivelents, so the
	 * dtb initialize proc will skip those non-NULL fields...
	 */
        dtbMainwinPropDialogInfo_clear(&rev_mainwin_prop_dialog);

	cgen = &(rev_mainwin_prop_dialog);
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

    }
    else /* AB_PROP_FIXED */
    	cgen = &dtb_mainwin_prop_dialog;

    if (dtb_mainwin_prop_dialog_initialize(cgen, parent) == 0)
    {
        pms->prop_sheet = cgen->attrs_ctrlpanel;
        pms->current_obj = NULL;

	if (type == AB_PROP_REVOLVING)
        	XtVaSetValues(parent,
                        XmNuserData, pms->current_obj,
                        NULL);

        /* Dialog/Object List */
	if (type == AB_PROP_FIXED)
	{
            prop_fixed_dialog_init(ab_mainwin_palitem,
			cgen->prop_dialog_shellform, cgen->objlist);
            prop_activate_panel_init(type, ab_mainwin_palitem,
                        cgen->ok_button, cgen->apply_button,
                        cgen->reset_button, cgen->cancel_button,
                        cgen->help_button);
	}
        /* Alternate Editor Buttons */
        prop_editors_panel_init(type, ab_mainwin_palitem,
            NULL, cgen->conn_button, cgen->helptxt_button);

        /*
          * Prop Sheet Settings....
         */

        /* Name */
        prop_field_init(&(pms->name), cgen->name_field_label,
                cgen->name_field, cgen->name_cb);

        /* Title */
        prop_field_init(&(pms->title), cgen->title_field_label,
		cgen->title_field,
                cgen->title_cb);

        /* Resize Mode */
        n = 0;
        item[n] = cgen->resizemode_rbox_items.Adjustable_item;
        item_val[n] = True; n++;
        item[n] = cgen->resizemode_rbox_items.Fixed_item;
        item_val[n] = False; n++;
        prop_radiobox_init(&(pms->resize_mode), cgen->resizemode_rbox_label,
                cgen->resizemode_rbox, n, item, (XtPointer*)item_val,
                cgen->resizemode_cb);

        /* Icon Filename */
        prop_field_init(&(pms->icon), cgen->icon_field_label,
                cgen->icon_field, cgen->icon_cb);

        /* Icon Mask Filename */
        prop_field_init(&(pms->icon_mask), cgen->iconmask_field_label,
                cgen->iconmask_field, cgen->iconmask_cb);

        /* Icon Label */
        prop_field_init(&(pms->icon_label), cgen->iconlabel_field_label,
                cgen->iconlabel_field, cgen->iconlabel_cb);

        /* Window Areas */
        n = 0;
        item[n] = cgen->areas_ckbox_items.Menubar_item;
        item_val[n] = AB_CONT_MENU_BAR; n++;
        item[n] = cgen->areas_ckbox_items.Toolbar_item;
        item_val[n] = AB_CONT_TOOL_BAR; n++;
        item[n] = cgen->areas_ckbox_items.Footer_item;
        item_val[n] = AB_CONT_FOOTER; n++;
  	prop_checkbox_init(&(pms->areas), cgen->areas_ckbox_label,
		cgen->areas_ckbox, n, item, item_val,
		cgen->areas_cb);

        /* Size Policy */
        n = 0;
        item[n] = cgen->size_policy_rbox_items.Fit_Contents_item;
        item_val[n] = SIZE_OF_CONTENTS_KEY; n++;
        item[n] = cgen->size_policy_rbox_items.Fixed_item;
        item_val[n] = SIZE_FIXED_KEY; n++;
        prop_radiobox_init(&(pms->size_policy), cgen->size_policy_rbox_label,
                cgen->size_policy_rbox, n, item, (XtPointer*)item_val,
                cgen->size_policy_cb);

        for(i=0; i < n; i++)
            XtAddCallback(item[i], XmNvalueChangedCallback,
                  prop_size_policyCB, (XtPointer)&(pms->geometry));

        /* Geometry */
        prop_geomfield_init(&(pms->geometry), cgen->geom_label,
		NULL, NULL, NULL, NULL,
                cgen->width_field_label, cgen->width_field,
                cgen->height_field_label, cgen->height_field,
                cgen->geom_cb);

        /* Initial State */
        n = 0;
        item[n] = cgen->istate_ckbox_items.Visible_item;
        item_val[n] = AB_STATE_VISIBLE; n++;
        item[n] = cgen->istate_ckbox_items.Iconic_item;
        item_val[n] = AB_STATE_ICONIC; n++;
        prop_checkbox_init(&(pms->init_state), cgen->istate_ckbox_label,
                cgen->istate_ckbox, n, item, item_val,
                cgen->istate_cb);

        /* Background Setting */
        prop_colorfield_init(&(pms->bg_color), cgen->bg_mbutton,
		cgen->bg_mbutton_bg_mbutton_menu_items.None_item,
		cgen->bg_mbutton_bg_mbutton_menu_items.Color_Chooser_item,
                cgen->bg_swatch, cgen->bg_field, cgen->bg_cb);

        /* Foreground Setting */
        prop_colorfield_init(&(pms->fg_color), cgen->fg_mbutton,
                cgen->fg_mbutton_fg_mbutton_menu_items.None_item,
                cgen->fg_mbutton_fg_mbutton_menu_items.Color_Chooser_item,
                cgen->fg_swatch, cgen->fg_field, cgen->fg_cb);

        prop_changebars_cleared(pms->prop_sheet);

	return (cgen->prop_dialog_shellform);
    }
    else
    	return NULL;

}

static int
mainwin_prop_activate(
    AB_PROP_TYPE type,
    BOOL	 active
)
{
     ui_set_active(prop_mainwin_settings_rec[type].prop_sheet, active);
     return OK;
}

static int
mainwin_prop_clear(
    AB_PROP_TYPE type
)
{
    PropMainwinSettingsRec	*pms = &(prop_mainwin_settings_rec[type]);

    if (pms->current_obj == NULL)
	return OK;

    /* Clear Name */
    prop_field_set_value(&(pms->name), "", False);

    /* Clear Title */
    prop_field_set_value(&(pms->title), "", False);

    /* Clear Resize Mode */
    prop_radiobox_set_value(&(pms->resize_mode), (XtPointer)True, False);

    /* Clear Icon Filename */
    prop_field_set_value(&(pms->icon), "", False);

    /* Clear Icon Mask Filename */
    prop_field_set_value(&(pms->icon_mask), "", False);

    /* Clear Icon Label */
    prop_field_set_value(&(pms->icon_label), "", False);

    /* Clear Areas */
    prop_checkbox_set_value(&(pms->areas), AB_CONT_MENU_BAR, False, False);
    prop_checkbox_set_value(&(pms->areas), AB_CONT_TOOL_BAR, False, False);
    prop_checkbox_set_value(&(pms->areas), AB_CONT_FOOTER, False, False);

    /* Clear Size Policy */
    prop_radiobox_set_value(&(pms->size_policy),
                            (XtPointer)SIZE_FIXED_KEY, False);

    /* Clear Size */
    prop_geomfield_clear(&(pms->geometry), GEOM_WIDTH);
    prop_geomfield_clear(&(pms->geometry), GEOM_HEIGHT);

    /* Clear Initial State */
    prop_checkbox_set_value(&(pms->init_state), AB_STATE_VISIBLE, True, False);
    prop_checkbox_set_value(&(pms->init_state), AB_STATE_ICONIC, False, False);

    /* Clear Color */
    prop_colorfield_set_value(&(pms->bg_color), "", False);
    prop_colorfield_set_value(&(pms->fg_color), "", False);

    pms->current_obj = NULL;

    turnoff_changebars(type);

    return OK;
}

static int
mainwin_prop_load(
    ABObj        obj,
    AB_PROP_TYPE type,
    unsigned long loadkey
)
{
    PropMainwinSettingsRec 	*pms = &(prop_mainwin_settings_rec[type]);
    ABObj		   	area;
    BOOL			load_all = (loadkey & LoadAll);

    if (obj == NULL)
    {
        if (pms->current_obj != NULL)
            obj = pms->current_obj;
        else
            return ERROR;
    }
    else if (!obj_is_base_win(obj))
        return ERROR;
    else
        pms->current_obj = obj;

    /* Load Name */
    if (load_all || loadkey & LoadName)
    	prop_field_set_value(&(pms->name), obj_get_name(obj), False);

    if (load_all)
    {
	/* Load Title */
	prop_field_set_value(&(pms->title), obj_get_label(obj), False);

	/* Load Resize Mode */
	prop_radiobox_set_value(&(pms->resize_mode),
                                (XtPointer)(intptr_t) obj_get_resizable(obj), False);

	/* Load Icon Filename */
	prop_field_set_value(&(pms->icon), obj_get_icon(obj), False);

	/* Load Icon Mask Filename */
	prop_field_set_value(&(pms->icon_mask), obj_get_icon_mask(obj), False);

	/* Load Icon Label */
	prop_field_set_value(&(pms->icon_label), obj_get_icon_label(obj), False);

	/* Load Window Areas */
	area = objxm_comp_mainwin_get_area(obj, AB_CONT_MENU_BAR);
	prop_checkbox_set_value(&(pms->areas), AB_CONT_MENU_BAR,
	area != NULL, False);

	area = objxm_comp_mainwin_get_area(obj, AB_CONT_TOOL_BAR);
	prop_checkbox_set_value(&(pms->areas), AB_CONT_TOOL_BAR,
		area != NULL, False);

	area = objxm_comp_mainwin_get_area(obj, AB_CONT_FOOTER);
	prop_checkbox_set_value(&(pms->areas), AB_CONT_FOOTER,
		area != NULL, False);

	/* Load Size Policy */
	prop_radiobox_set_value(&(pms->size_policy),
				abobj_width_resizable(obj)?
                                (XtPointer)SIZE_FIXED_KEY :
                                (XtPointer)SIZE_OF_CONTENTS_KEY, False);

	/* Load Initial State */
	prop_checkbox_set_value(&(pms->init_state), AB_STATE_VISIBLE,
		obj_is_initially_visible(obj), False);
	prop_checkbox_set_value(&(pms->init_state), AB_STATE_ICONIC,
		obj_is_initially_iconic(obj), False);

	/* Load Color */
	prop_colorfield_set_value(&(pms->bg_color), obj_get_bg_color(obj), False);
	prop_colorfield_set_value(&(pms->fg_color), obj_get_fg_color(obj), False);

	turnoff_changebars(type);
    }

    /* Load Size */
    if (load_all || loadkey & LoadSize)
        prop_load_obj_size(obj, &(pms->geometry));

    return OK;
}

int
mainwin_prop_apply(
    AB_PROP_TYPE   type
)
{
    PropMainwinSettingsRec 	*pms = &(prop_mainwin_settings_rec[type]);
    ABObj			module;
    ABObj			area;
    BOOL			area_set;
    STRING			value, icon, icon_mask, icon_label;
    BOOL			reset_bg = False;
    BOOL			reset_fg = False;
    BOOL                        size_chg = False;
    int				new_w, new_h;

    if (!verify_props(type))
        return ERROR;

    if (prop_changed(pms->name.changebar))
    {
        value = prop_field_get_value(&(pms->name));
        abobj_set_name(pms->current_obj, value);
        util_free(value);
    }
    if (prop_changed(pms->title.changebar))
    {
        value = prop_field_get_value(&(pms->title));
        abobj_set_label(pms->current_obj, pms->current_obj->label_type, value);
        util_free(value);
    }
    if (prop_changed(pms->resize_mode.changebar))
    {
        abobj_set_resize_mode(pms->current_obj,
		(BOOL)prop_radiobox_get_value(&(pms->resize_mode)));
    }
    if (prop_changed(pms->icon.changebar) ||
	prop_changed(pms->icon_mask.changebar) ||
	prop_changed(pms->icon_label.changebar))
    {
        icon = prop_field_get_value(&(pms->icon));
	icon_mask = prop_field_get_value(&(pms->icon_mask));
	icon_label = prop_field_get_value(&(pms->icon_label));
        abobj_set_icon(pms->current_obj, icon, icon_mask, icon_label);
        util_free(icon);
        util_free(icon_mask);
        util_free(icon_label);
    }
    if (prop_changed(pms->areas.changebar))
    {
	module = obj_get_module(pms->current_obj);

	/* Menubar */
	area = objxm_comp_mainwin_get_area(pms->current_obj, AB_CONT_MENU_BAR);
	area_set = prop_checkbox_get_value(&(pms->areas), AB_CONT_MENU_BAR);

	if (area != NULL && !area_set) /* Remove Menubar */
	    mainwin_remove_area(pms->current_obj, area);

	else if (area == NULL && area_set) /* Add Menubar */
	    mainwin_create_area(pms->current_obj, AB_CONT_MENU_BAR);

        /* Toolbar */
        area = objxm_comp_mainwin_get_area(pms->current_obj, AB_CONT_TOOL_BAR);
        area_set = prop_checkbox_get_value(&(pms->areas), AB_CONT_TOOL_BAR);

        if (area != NULL && !area_set) /* Remove Toolbar */
            mainwin_remove_area(pms->current_obj, area);

        else if (area == NULL && area_set) /* Add Toolbar */
            mainwin_create_area(pms->current_obj, AB_CONT_TOOL_BAR);

        /* Footer */
        area = objxm_comp_mainwin_get_area(pms->current_obj, AB_CONT_FOOTER);
        area_set = prop_checkbox_get_value(&(pms->areas), AB_CONT_FOOTER);

        if (area != NULL && !area_set) /* Remove Footer */
            mainwin_remove_area(pms->current_obj, area);

        else if (area == NULL && area_set) /* Add Footer */
            mainwin_create_area(pms->current_obj, AB_CONT_FOOTER);
    }
    if (prop_changed(pms->size_policy.changebar))
    {
	abobj_set_size_policy(pms->current_obj,
		prop_radiobox_get_value(&(pms->size_policy)) == SIZE_FIXED_KEY);

	size_chg = True;
    }
    if (prop_changed(pms->geometry.changebar))
    {
    	if (abobj_width_resizable(pms->current_obj))
	{
	    new_w = prop_geomfield_get_value(&(pms->geometry), GEOM_WIDTH);
	    abobj_set_pixel_width(pms->current_obj, new_w, 0);
	}
	if (abobj_height_resizable(pms->current_obj))
	{
	    new_h = prop_geomfield_get_value(&(pms->geometry), GEOM_HEIGHT);
	    abobj_set_pixel_height(pms->current_obj, new_h, 0);
	}
    }
    if (prop_changed(pms->init_state.changebar))
    {
        abobj_set_visible(pms->current_obj,
		prop_checkbox_get_value(&(pms->init_state), AB_STATE_VISIBLE));
        abobj_set_iconic(pms->current_obj,
		prop_checkbox_get_value(&(pms->init_state), AB_STATE_ICONIC));
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
    abobj_instantiate_changes(pms->current_obj);

    if (reset_bg || reset_fg) /* Set back to No Color */
        abobj_reset_colors(pms->current_obj, reset_bg, reset_fg);

    turnoff_changebars(type);

    return OK;

}

static BOOL
mainwin_prop_pending(
    AB_PROP_TYPE type
)
{
    return(prop_changebars_pending(prop_mainwin_settings_rec[type].prop_sheet));
}

static BOOL
verify_props(
    AB_PROP_TYPE type
)
{
    PropMainwinSettingsRec *pms = &(prop_mainwin_settings_rec[type]);
    ABObj		   area;
    BOOL		   area_set;

    if (prop_changed(pms->name.changebar) && !prop_name_ok(pms->current_obj, pms->name.field))
        return False;

    if (prop_changed(pms->areas.changebar))
    {
   	DTB_MODAL_ANSWER	answer;
    	ABObj			pobj;

        area = objxm_comp_mainwin_get_area(pms->current_obj, AB_CONT_MENU_BAR);
        area_set = prop_checkbox_get_value(&(pms->areas), AB_CONT_MENU_BAR);
        if (area != NULL && !area_set) /* Remove Menubar */
        {
	    pobj = objxm_comp_get_subobj(area, AB_CFG_PARENT_OBJ);

	    if (obj_get_num_children(pobj) > 0)
	    {
                /* Popup Modal Message and wait for answer */
		dtb_mainwin_remove_mbar_msg_initialize(
			&dtb_mainwin_remove_mbar_msg);
                answer = dtb_show_modal_message(pms->areas.checkbox,
				&dtb_mainwin_remove_mbar_msg,
				NULL, NULL, NULL);
                if (answer == DTB_ANSWER_CANCEL)
                    return False;
	    }
	}

        area = objxm_comp_mainwin_get_area(pms->current_obj, AB_CONT_TOOL_BAR);
        area_set = prop_checkbox_get_value(&(pms->areas), AB_CONT_TOOL_BAR);
        if (area != NULL && !area_set) /* Remove Toolbar */
        {
            pobj = objxm_comp_get_subobj(area, AB_CFG_PARENT_OBJ);

            if (obj_get_num_children(pobj) > 0)
            {
                /* Popup Modal Message and wait for answer */
		dtb_mainwin_remove_tbar_msg_initialize(
			&dtb_mainwin_remove_tbar_msg);
                answer = dtb_show_modal_message(pms->areas.checkbox,
				&dtb_mainwin_remove_tbar_msg,
				NULL, NULL, NULL);
                if (answer == DTB_ANSWER_CANCEL)
                    return False;
	    }
        }

        area = objxm_comp_mainwin_get_area(pms->current_obj, AB_CONT_FOOTER);
        area_set = prop_checkbox_get_value(&(pms->areas), AB_CONT_FOOTER);
        if (area != NULL && !area_set) /* Remove Footer */
        {
            pobj = objxm_comp_get_subobj(area, AB_CFG_PARENT_OBJ);

            if (obj_get_num_children(pobj) > 0)
            {
                /* Popup Modal Message and wait for answer */
		dtb_mainwin_remove_footer_msg_initialize(
			&dtb_mainwin_remove_footer_msg);
                answer = dtb_show_modal_message(pms->areas.checkbox,
				&dtb_mainwin_remove_footer_msg,
				NULL, NULL, NULL);
                if (answer == DTB_ANSWER_CANCEL)
                    return False;
	    }
        }
    }
    if (prop_changed(pms->icon.changebar) &&
	!prop_graphic_filename_ok(pms->icon.field, True))
	return False;

    if (prop_changed(pms->icon_mask.changebar) &&
	!prop_graphic_filename_ok(pms->icon_mask.field, True))
	return False;

    if (prop_changed(pms->geometry.changebar) &&
	 (!prop_number_ok(pms->geometry.w_field, (STRING)WFieldStr, 1, SHRT_MAX) ||
	 !prop_number_ok(pms->geometry.h_field, (STRING)HFieldStr, 1, SHRT_MAX)))
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
    PropMainwinSettingsRec *pms = &(prop_mainwin_settings_rec[type]);

    prop_set_changebar(pms->name.changebar,    		PROP_CB_OFF);
    prop_set_changebar(pms->title.changebar,     	PROP_CB_OFF);
    prop_set_changebar(pms->resize_mode.changebar,     	PROP_CB_OFF);
    prop_set_changebar(pms->icon.changebar,    		PROP_CB_OFF);
    prop_set_changebar(pms->icon_mask.changebar,    	PROP_CB_OFF);
    prop_set_changebar(pms->icon_label.changebar,     	PROP_CB_OFF);
    prop_set_changebar(pms->areas.changebar,     	PROP_CB_OFF);
    prop_set_changebar(pms->size_policy.changebar,     	PROP_CB_OFF);
    prop_set_changebar(pms->geometry.changebar,     	PROP_CB_OFF);
    prop_set_changebar(pms->init_state.changebar,	PROP_CB_OFF);
    prop_set_changebar(pms->bg_color.changebar,  	PROP_CB_OFF);
    prop_set_changebar(pms->fg_color.changebar,  	PROP_CB_OFF);

    prop_changebars_cleared(pms->prop_sheet);

}

static void
mainwin_create_area(
    ABObj	obj,
    AB_CONTAINER_TYPE	area_type
)
{
    ABObj	mwobj;
    ABObj	label, area;
    ABObj       workobj;
    Dimension   height;

    mwobj = objxm_comp_get_subobj(obj, AB_CFG_WINDOW_MW_OBJ);
    workobj = objxm_comp_get_subobj(obj, AB_CFG_PARENT_OBJ);

    area = obj_create(AB_TYPE_CONTAINER, mwobj);
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
    abobj_show_tree(area, True);

    /* Workaround part2 */
    if (area_type == AB_CONT_FOOTER)
	XtVaSetValues(objxm_get_widget(workobj),
		XmNheight,	height,
		NULL);

    abobj_set_save_needed(obj_get_module(obj), True);

}

static void
mainwin_remove_area(
    ABObj       obj,
    ABObj	area
)
{
/*
    XtUnmanageChild(objxm_get_widget(area));
*/
    objxm_tree_destroy(area);

    abobj_set_save_needed(obj_get_module(obj), True);
}
