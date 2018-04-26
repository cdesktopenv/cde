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
 *	$XConsortium: pal_fchooser.c /main/5 1996/08/08 18:01:05 mustafa $
 *
 *	@(#)pal_fchooser.c	1.4 07 Jun 1994
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
 * pal_fchooser.c - Implements Palette FileChooser object functionality
 */
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
#include "fchooser_ui.h"

typedef struct  PROP_FCHOOSER_SETTINGS
{
    Widget                      prop_sheet;
    PropFieldSettingRec         win_parent;
    PropFieldSettingRec         name;
    PropFieldSettingRec         title;
    PropFieldSettingRec         init_dir;
    PropFieldSettingRec         pattern;
    PropFieldSettingRec         ok_label;
    PropRadioSettingRec   	pattern_type;
    PropCheckboxSettingRec      init_state;
    PropCheckboxSettingRec      auto_dismiss;
    PropColorSettingRec         bg_color;
    PropColorSettingRec         fg_color;
    ABObj                       current_obj;
} PropFchooserSettingsRec, *PropFchooserSettings;

/*************************************************************************
**                                                                      **
**       Private Function Declarations                                  **
**                                                                      **
**************************************************************************/
/*
 * Methods
 */
static int	fchooser_initialize(
                    ABObj   obj
                );
static Widget   fchooser_prop_init(
                    Widget  parent,
                    AB_PROP_TYPE type
                );
static int	fchooser_prop_activate(
		    AB_PROP_TYPE type,
		    BOOL 	 active
		);
static int	fchooser_prop_clear(
                    AB_PROP_TYPE type
                );
static int      fchooser_prop_load(
                    ABObj   	 obj,
                    AB_PROP_TYPE type,
		    unsigned long loadkey
                );
static int	fchooser_prop_apply(
		    AB_PROP_TYPE type
		);
static BOOL     fchooser_prop_pending(
                    AB_PROP_TYPE type
                );

static BOOL	verify_props(
		    AB_PROP_TYPE type
		);
static void	turnoff_changebars(
		    AB_PROP_TYPE type
		);


/*************************************************************************
**                                                                      **
**       Data                                                   	**
**                                                                      **
**************************************************************************/

PalItemInfo fchooser_palitem_rec = {

    /* type             */  AB_TYPE_FILE_CHOOSER,
    /* name             */  "File Selection",
    /* animation pixmaps*/  NULL,
    /* number of pixmaps*/  0,
    /* rev_prop_frame   */  NULL,
    /* fix_prop_dialog  */  NULL,
    /* initialize       */  fchooser_initialize,
    /* is_a_test        */  obj_is_file_chooser,
    /* prop_initialize  */  fchooser_prop_init,
    /* prop_activate	*/  fchooser_prop_activate,
    /* prop_clear	*/  fchooser_prop_clear,
    /* prop_load        */  fchooser_prop_load,
    /* prop_apply	*/  fchooser_prop_apply,
    /* prop_pending	*/  fchooser_prop_pending

};

PalItemInfo *ab_fchooser_palitem = &fchooser_palitem_rec;
PropFchooserSettingsRec prop_fchooser_settings_rec[AB_PROP_TYPE_NUM_VALUES];


/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/
static int
fchooser_initialize(
    ABObj     obj
)
{
    STRING	name;
    ABObj	module = obj_get_module(obj);
    ABObj	win;
    ABObj	bobj;
    int		num_wins;
    int		i;
    int		startpos, endpos;
    char        nameBuf[5001] = "";

    /* Find BaseWindow to be Custom Dialog's Motif parent.
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
    name = obj_get_unique_name(obj, "file_seln", 5000, nameBuf);
    obj_set_name(obj, name);

    obj_set_label(obj, catgets(Dtb_project_catd, 100, 257, "File Selection Dialog"));
    obj_set_ok_label(obj, catgets(Dtb_project_catd, 100, 228, "Ok"));
    obj_set_is_initially_visible(obj, False);
    obj_set_is_initially_active(obj, True);

    return OK;
}

static Widget
fchooser_prop_init(
    Widget parent,
    AB_PROP_TYPE type
)
{
    DtbFchooserPropDialogInfoRec rev_fchooser_prop_dialog; /* Revolving Props */
    DtbFchooserPropDialogInfo    cgen = &dtb_fchooser_prop_dialog; /* Codegen structure */
    DtbRevolvPropDialogInfo	rpd = &(dtb_revolv_prop_dialog);
    PropFchooserSettingsRec    	*pcs = &(prop_fchooser_settings_rec[type]);
    Widget            		item[6];
    int            		item_val[6];
    int            		n;

    if (type == AB_PROP_REVOLVING)
    {
	/* Cloning Trick:
	 * Only the Attributes ControlPanel needs to be created within
	 * the existing Revolving Prop dialog, so fill out all other
	 * fields with the Revolving Prop dialog equivalents, so the
	 * dtb initialize proc will skip those non-NULL fields...
	 */
        dtbFchooserPropDialogInfo_clear(&rev_fchooser_prop_dialog);

	cgen = &(rev_fchooser_prop_dialog);
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
    	cgen = &dtb_fchooser_prop_dialog;

    if (dtb_fchooser_prop_dialog_initialize(cgen, parent) == 0)
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
            prop_fixed_dialog_init(ab_fchooser_palitem,
			cgen->prop_dialog_shellform, cgen->objlist);
            prop_activate_panel_init(type, ab_fchooser_palitem,
                        cgen->ok_button, cgen->apply_button,
                        cgen->reset_button, cgen->cancel_button,
                        cgen->help_button);
	}

        /* Alternate Editor Buttons */
        prop_editors_panel_init(type, ab_fchooser_palitem,
            NULL, cgen->conn_button, cgen->help_txt_button);

        /*
          * Prop Sheet Settings....
         */

        /* Name */
        prop_field_init(&(pcs->name), cgen->name_field_label,
                cgen->name_field, cgen->name_cb);

        /* Window Parent */
        prop_field_init(&(pcs->win_parent), cgen->winparent_field_label,
                cgen->winparent_field, cgen->winparent_cb);

        /* Title */
        prop_field_init(&(pcs->title), cgen->title_field_label,
		cgen->title_field, cgen->title_cb);

        /* Initial Directory */
        prop_field_init(&(pcs->init_dir), cgen->init_dir_field_label,
		cgen->init_dir_field, cgen->init_dir_cb);

        /* Search Pattern Type */
        n = 0;
        item[n] = cgen->pattern_type_rbox_items.Files_item;
        item_val[n] = AB_FILE_REGULAR; n++;
        item[n] = cgen->pattern_type_rbox_items.Directories_item;
        item_val[n] = AB_FILE_DIRECTORY; n++;
        item[n] = cgen->pattern_type_rbox_items.Both_item;
        item_val[n] = AB_FILE_ANY; n++;
        prop_radiobox_init(&(pcs->pattern_type), cgen->pattern_type_rbox_label,
                cgen->pattern_type_rbox, n, item, (XtPointer*)item_val,
                cgen->pattern_type_cb);

        /* Search Pattern */
	prop_field_init(&(pcs->pattern), cgen->pattern_field_label,
                cgen->pattern_field, cgen->pattern_cb);

        /* Ok Button Label */
	prop_field_init(&(pcs->ok_label), cgen->ok_btn_lbl_field_label,
                cgen->ok_btn_lbl_field, cgen->ok_btn_lbl_cb);

        /* Initial State */
        n = 0;
        item[n] = cgen->istate_ckbox_items.Visible_item;
        item_val[n] = AB_STATE_VISIBLE; n++;
        prop_checkbox_init(&(pcs->init_state), cgen->istate_ckbox_label,
                cgen->istate_ckbox, n, item, item_val, cgen->istate_cb);

        /* Popdown Behavior */
        n = 0;
        item[n] = cgen->popdown_checkbox_items.AutomaticallyDismiss_item;
        item_val[n] = DISMISS_ITEM_KEY; n++;
        prop_checkbox_init(&(pcs->auto_dismiss), cgen->popdown_checkbox_label,
                cgen->popdown_checkbox, n, item, item_val, cgen->popdown_cb);

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
fchooser_prop_activate(
    AB_PROP_TYPE type,
    BOOL	 active
)
{
     ui_set_active(prop_fchooser_settings_rec[type].prop_sheet, active);

    return OK;
}


static int
fchooser_prop_clear(
    AB_PROP_TYPE type
)
{
    PropFchooserSettingsRec	*pcs = &(prop_fchooser_settings_rec[type]);

    if (pcs->current_obj == NULL)
	return OK;

    /* Clear Name */
    prop_field_set_value(&(pcs->name), "", False);

    /* Clear Window Parent */
    prop_field_set_value(&(pcs->win_parent), "", False);

    /* Clear Title */
    prop_field_set_value(&(pcs->title), "", False);

    /* Clear Initial Directory */
    prop_field_set_value(&(pcs->init_dir), "", False);

    /* Clear Search Pattern */
    prop_field_set_value(&(pcs->pattern), "", False);

    /* Clear Ok Button Label */
    prop_field_set_value(&(pcs->ok_label), "", False);

    /* Clear Search Pattern Type */
    prop_radiobox_set_value(&(pcs->pattern_type), (XtPointer)True, False);

    /* Clear Initial State */
    prop_checkbox_set_value(&(pcs->init_state), AB_STATE_VISIBLE, True, False);

    /* Clear Popdown Behavior */
    prop_checkbox_set_value(&(pcs->auto_dismiss), DISMISS_ITEM_KEY, True, False);

    /* Clear Color */
    prop_colorfield_set_value(&(pcs->bg_color), "", False);
    prop_colorfield_set_value(&(pcs->fg_color), "", False);

    pcs->current_obj = NULL;

    turnoff_changebars(type);

    return OK;
}

static int
fchooser_prop_load(
    ABObj    	 obj,
    AB_PROP_TYPE type,
    unsigned long loadkey
)
{
    ABObj	   		win_parent;
    STRING	   		name;
    PropFchooserSettingsRec 	*pcs = &(prop_fchooser_settings_rec[type]);
    BOOL                        load_all = (loadkey & LoadAll);

    if (obj == NULL)
    {
        if (pcs->current_obj != NULL)
            obj = pcs->current_obj;
        else
            return ERROR;
    }
    else if (!obj_is_file_chooser(obj))
        return ERROR;
    else
        pcs->current_obj = obj;

    /* Load Name */
    if (load_all || loadkey & LoadName)
    	prop_field_set_value(&(pcs->name), obj_get_name(obj), False);

    if (load_all)
    {
	/* Load Window Parent */
	if ((win_parent = obj_get_win_parent(obj)) != NULL)
	{
		name = abobj_get_moduled_name(win_parent);
		prop_field_set_value(&(pcs->win_parent), name, False);
		util_free(name);
	}
	else
		prop_field_set_value(&(pcs->win_parent), "", False);

	/* Load Title */
	prop_field_set_value(&(pcs->title), obj_get_label(obj), False);

	/* Load Initial Directory */
	prop_field_set_value(&(pcs->init_dir), obj_get_directory(obj), False);

	/* Load Search Pattern */
	prop_field_set_value(&(pcs->pattern), obj_get_filter_pattern(obj), False);

	/* Load Ok Button Label */
	prop_field_set_value(&(pcs->ok_label), obj_get_ok_label(obj), False);

	/* Load Search Pattern Type */
	prop_radiobox_set_value(&(pcs->pattern_type),
			(XtPointer)obj_get_file_type_mask(obj), False);

	/* Load Initial State */
	prop_checkbox_set_value(&(pcs->init_state), AB_STATE_VISIBLE,
		obj_is_initially_visible(obj), False);

	/* Load Popdown Behavior */
	prop_checkbox_set_value(&(pcs->auto_dismiss), DISMISS_ITEM_KEY,
		obj_get_auto_dismiss(obj), False);

	/* Load Color */
	prop_colorfield_set_value(&(pcs->bg_color), obj_get_bg_color(obj), False);
	prop_colorfield_set_value(&(pcs->fg_color), obj_get_fg_color(obj), False);

	turnoff_changebars(type);
    }

    return OK;
}

int
fchooser_prop_apply(
    AB_PROP_TYPE   type
)
{
    PropFchooserSettingsRec 	*pcs = &(prop_fchooser_settings_rec[type]);
    STRING			value;
    BOOL			reset_bg = False;
    BOOL			reset_fg = False;

    if (!verify_props(type))
        return ERROR;

    if (prop_changed(pcs->name.changebar))
    {
        value = prop_field_get_value(&(pcs->name));
        abobj_set_name(pcs->current_obj, value);
        util_free(value);
    }
    if (prop_changed(pcs->win_parent.changebar))
    {
        value = prop_field_get_value(&(pcs->win_parent));
        if (util_strcmp(value, "") == 0)
        {
            abobj_set_win_parent(pcs->current_obj, NULL);
            util_free(value);
        }
        else
        {
            abobj_set_win_parent(pcs->current_obj,
                obj_scoped_find_by_name(proj_get_project(), (STRING)value));
            util_free(value);

            /* Ensure that the object is displayed in the form "module :: obj" */
            value = abobj_get_moduled_name(obj_get_win_parent(pcs->current_obj));
            prop_field_set_value(&(pcs->win_parent), value, False);
            util_free(value);
        }
    }
    if (prop_changed(pcs->title.changebar))
    {
        value = prop_field_get_value(&(pcs->title));
        abobj_set_label(pcs->current_obj, pcs->current_obj->label_type, value);
        util_free(value);
    }
    if (prop_changed(pcs->init_dir.changebar))
    {
        value = prop_field_get_value(&(pcs->init_dir));
        abobj_set_directory(pcs->current_obj, value);
        util_free(value);
    }
    if (prop_changed(pcs->pattern.changebar))
    {
        value = prop_field_get_value(&(pcs->pattern));
        abobj_set_filter_pattern(pcs->current_obj, value);
        util_free(value);
    }
    if (prop_changed(pcs->ok_label.changebar))
    {
        value = prop_field_get_value(&(pcs->ok_label));
        abobj_set_ok_label(pcs->current_obj, value);
        util_free(value);
    }
    if (prop_changed(pcs->pattern_type.changebar))
    {
        abobj_set_pattern_type(pcs->current_obj,
	    (AB_FILE_TYPE_MASK)prop_radiobox_get_value(&(pcs->pattern_type)));
    }
    if (prop_changed(pcs->init_state.changebar))
    {
        abobj_set_visible(pcs->current_obj,
		prop_checkbox_get_value(&(pcs->init_state), AB_STATE_VISIBLE));
    }
    if (prop_changed(pcs->auto_dismiss.changebar))
    {
        abobj_set_auto_dismiss(pcs->current_obj,
		prop_checkbox_get_value(&(pcs->auto_dismiss), DISMISS_ITEM_KEY));
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
    abobj_instantiate_changes(pcs->current_obj);

    if (reset_bg || reset_fg) /* Set back to No Color */
        abobj_reset_colors(pcs->current_obj, reset_bg, reset_fg);

    turnoff_changebars(type);

    return OK;

}

static BOOL
fchooser_prop_pending(
    AB_PROP_TYPE type
)
{
    return(prop_changebars_pending(prop_fchooser_settings_rec[type].prop_sheet));
}

static BOOL
verify_props(
    AB_PROP_TYPE type
)
{
    PropFchooserSettingsRec *pcs = &(prop_fchooser_settings_rec[type]);

    if (prop_changed(pcs->name.changebar) && !prop_name_ok(pcs->current_obj, pcs->name.field))
        return False;

    if (prop_changed(pcs->win_parent.changebar) && !prop_obj_name_ok(pcs->win_parent.field,
                proj_get_project(), AB_TYPE_BASE_WINDOW, "Main Window"))
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
    PropFchooserSettingsRec *pcs = &(prop_fchooser_settings_rec[type]);

    prop_set_changebar(pcs->name.changebar,    		PROP_CB_OFF);
    prop_set_changebar(pcs->win_parent.changebar,    	PROP_CB_OFF);
    prop_set_changebar(pcs->title.changebar,     	PROP_CB_OFF);
    prop_set_changebar(pcs->init_dir.changebar,     	PROP_CB_OFF);
    prop_set_changebar(pcs->pattern_type.changebar,     PROP_CB_OFF);
    prop_set_changebar(pcs->pattern.changebar,     	PROP_CB_OFF);
    prop_set_changebar(pcs->ok_label.changebar,     	PROP_CB_OFF);
    prop_set_changebar(pcs->init_state.changebar,	PROP_CB_OFF);
    prop_set_changebar(pcs->auto_dismiss.changebar,	PROP_CB_OFF);
    prop_set_changebar(pcs->bg_color.changebar,  	PROP_CB_OFF);
    prop_set_changebar(pcs->fg_color.changebar,  	PROP_CB_OFF);

    prop_changebars_cleared(pcs->prop_sheet);

}
