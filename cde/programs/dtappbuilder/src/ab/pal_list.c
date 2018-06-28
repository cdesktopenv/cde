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
 *      $XConsortium: pal_list.c /main/5 1996/08/08 18:02:00 mustafa $
 *
 * @(#)pal_list.c	1.41 15 Feb 1994      cde_app_builder/src/ab
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
 * pal_list.c - Implements Palette List object functionality
 */
#include <stdint.h>
#include <stdio.h>
#include <Xm/Xm.h>
#include <ab_private/obj_notify.h>
#include <ab_private/objxm.h>
#include <ab_private/ab.h>
#include <ab_private/pal.h>
#include <ab_private/prop.h>
#include <ab_private/abobj.h>
#include <ab_private/abobj_set.h>
#include <ab_private/abobj_edit.h>
#include <ab_private/ui_util.h>
#include "list_ui.h"

const    int    list_init_width  = 130;

typedef struct  PROP_LIST_SETTINGS
{
    Widget                      prop_sheet;
    PropFieldSettingRec         name;
    PropOptionsSettingRec       sel_mode;
    PropGeometrySettingRec      pos;
    PropRadioSettingRec       	wth_policy;
    PropGeometrySettingRec      wth;
    PropOptionsSettingRec       hgt_metric;
    PropGeometrySettingRec      hgt;
    PropOptionsSettingRec       label_type;
    PropFieldSettingRec         label;
    PropOptionsSettingRec       label_pos;
    PropCheckboxSettingRec      init_state;
    PropColorSettingRec         bg_color;
    PropColorSettingRec         fg_color;
    PropMenunameSettingRec      menuname;
    PropFieldSettingRec         menu_title;
    PropFieldSettingRec         item_label;
    PropCheckboxSettingRec      item_state;
    PropItemsSettingRec         items;
    ABObj                       current_obj;
} PropListSettingsRec, *PropListSettings;

/*************************************************************************
**                                                                      **
**       Private Function Declarations                                  **
**                                                                      **
**************************************************************************/
/*
 * Methods
 */
static int      list_initialize(
                    ABObj   obj
                );
static Widget   list_prop_init(
                    Widget  parent,
		    AB_PROP_TYPE type
                );
static int	list_prop_activate(
		    AB_PROP_TYPE type,
		    BOOL 	 active
		);
static int	list_prop_clear(
                    AB_PROP_TYPE type
                );
static int      list_prop_load(
                    ABObj   	 obj,
		    AB_PROP_TYPE type,
		    unsigned long loadkey
                );
static int	list_prop_apply(
		    AB_PROP_TYPE type
		);
static BOOL     list_prop_pending(
                    AB_PROP_TYPE type
                );
static BOOL	verify_props(
		    AB_PROP_TYPE type
		);
static void	turnoff_changebars(
		    AB_PROP_TYPE type
		);

/*
 * ABObj callbacks
 */
static int	prop_list_install_obj_destroy_CB(void);

static int	prop_list_obj_destroy_CB(
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
PalItemInfo list_palitem_rec = {

    /* type             */  AB_TYPE_LIST,
    /* name             */  "List",
    /* animation pixmaps*/  NULL,
    /* number of pixmaps*/  0,
    /* rev_prop_frame   */  NULL,
    /* fix_prop_dialog  */  NULL,
    /* initialize       */  list_initialize,
    /* is_a_test        */  obj_is_list,
    /* prop_initialize  */  list_prop_init,
    /* prop_activate	*/  list_prop_activate,
    /* prop_clear	*/  list_prop_clear,
    /* prop_load        */  list_prop_load,
    /* prop_apply	*/  list_prop_apply,
    /* prop_pending	*/  list_prop_pending

};

PalItemInfo *ab_list_palitem = &list_palitem_rec;
PropListSettingsRec prop_list_settings_rec[AB_PROP_TYPE_NUM_VALUES];


/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/
static int
list_initialize(
    ABObj    obj
)
{
    AB_LIST_INFO *info = &(obj->info.list);
    ABObj	module = obj_get_module(obj);
    ABObj	iobj;
    String  items[2];
    int		i;

    obj_set_unique_name(obj, "list");

    obj_set_num_rows(obj, 4);
    obj_set_height(obj, -1);
    obj_set_width(obj, list_init_width);
    obj_set_selection_mode(obj, AB_SELECT_BROWSE);
    obj_set_is_initially_visible(obj, True);
    obj_set_is_initially_active(obj, True);

    /* Add initial items to list */
    items[0] = catgets(Dtb_project_catd, 6, 70, "itemA");
    items[1] = catgets(Dtb_project_catd, 6, 71, "itemB");
    for (i=0; i < XtNumber(items); i++)
    {
        iobj = obj_create(AB_TYPE_ITEM, NULL);
	obj_set_subtype(iobj, AB_ITEM_FOR_LIST);
        obj_append_child(obj, iobj);
	abobj_set_item_name(iobj, obj_get_module(obj), obj_get_name(obj), items[i]);
        obj_set_label(iobj, items[i]);
	obj_set_is_initially_selected(iobj, False);
    }
    obj_set_attachment(obj, AB_CP_NORTH, AB_ATTACH_POINT, NULL, obj->y);
    obj_set_attachment(obj, AB_CP_WEST,  AB_ATTACH_POINT, NULL, obj->x);

    return OK;

}


static Widget
list_prop_init(
    Widget    parent,
    AB_PROP_TYPE type
)
{
    DtbListPropDialogInfoRec	rev_list_prop_dialog; /* Revolving Props */
    DtbListPropDialogInfo       cgen = &dtb_list_prop_dialog; /* Codegen structure */
    DtbRevolvPropDialogInfo	rpd = &(dtb_revolv_prop_dialog);
    PropListSettingsRec    	*pls = &(prop_list_settings_rec[type]);
    Widget            		item[8];
    int            		item_val[8];
    Widget			item2[8];
    int				item2_val[8];
    int            		n, j;
    int				i;

    if (type == AB_PROP_REVOLVING)
    {
	/* Cloning Trick:
	 * Only the Attributes ControlPanel needs to be created within
	 * the existing Revolving Prop dialog, so fill out all other
	 * fields with the Revolving Prop dialog equivelents, so the
	 * dtb initialize proc will skip those non-NULL fields...
	 */
        dtbListPropDialogInfo_clear(&rev_list_prop_dialog);

	cgen = &(rev_list_prop_dialog);
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
    	prop_list_install_obj_destroy_CB();

    }
    else /* AB_PROP_FIXED - create entire dialog*/
    	cgen = &dtb_list_prop_dialog;

    if (dtb_list_prop_dialog_initialize(cgen, parent) == 0)
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
            prop_fixed_dialog_init(ab_list_palitem,
			cgen->prop_dialog_shellform, cgen->objlist);
            prop_activate_panel_init(type, ab_list_palitem,
                        cgen->ok_button, cgen->apply_button,
                        cgen->reset_button, cgen->cancel_button,
                        cgen->help_button);
	}

        /* Alternate Editor Buttons */
        prop_editors_panel_init(type, ab_list_palitem,
            cgen->attach_button, cgen->conn_button, cgen->helptxt_button);

        /*
          * Prop Sheet Settings....
         */

        /* Name */
        prop_field_init(&(pls->name), cgen->name_field_label,
                cgen->name_field, cgen->name_cb);

        /* Label, Type, Position */
        n = 0;
        item[n] = cgen->labeltype_rbox_items.String_item;
        item_val[n] = AB_LABEL_STRING; n++;
        item[n] = cgen->labeltype_rbox_items.Graphic_item;
        item_val[n] = AB_LABEL_GLYPH; n++;
        prop_options_init(&(pls->label_type), cgen->labeltype_rbox_label,
                cgen->labeltype_rbox, cgen->labeltype_rbox_menu,
		n, item, (XtPointer*)item_val,
                cgen->labeltype_cb);

        prop_field_init(&(pls->label), cgen->label_field_label,
                cgen->label_field, cgen->label_cb);

	prop_label_field_init(&(pls->label), cgen->graphic_hint, item, n);

        n = 0;
        item[n] = cgen->labelpos_rbox_items.Above_item;
        item_val[n] = AB_CP_NORTH; n++;
        item[n] = cgen->labelpos_rbox_items.Left_item;
        item_val[n] = AB_CP_WEST; n++;
        prop_options_init(&(pls->label_pos), cgen->labelpos_rbox_label,
                cgen->labelpos_rbox, cgen->labelpos_rbox_menu,
                n, item, (XtPointer*)item_val,
                cgen->labeltype_cb);

        /* Selection Mode */
        n = 0;
        item[n] = cgen->selmode_rbox_items.Single_Select_item;
        item_val[n] = AB_SELECT_SINGLE; n++;
        item[n] = cgen->selmode_rbox_items.Browse_Select_item;
        item_val[n] = AB_SELECT_BROWSE; n++;
        item[n] = cgen->selmode_rbox_items.Multiple_Select_item;
        item_val[n] = AB_SELECT_MULTIPLE; n++;
        item[n] = cgen->selmode_rbox_items.Browse_Multiple_Select_item;
        item_val[n] = AB_SELECT_BROWSE_MULTIPLE; n++;
        prop_options_init(&(pls->sel_mode), cgen->selmode_rbox_label,
                                cgen->selmode_rbox, cgen->selmode_rbox_menu,
                                n, item, (XtPointer*)item_val,
                                cgen->selmode_cb);

	/* Position */
	prop_geomfield_init(&(pls->pos), cgen->pos_label,
			    cgen->x_field_label, cgen->x_field,
			    cgen->y_field_label, cgen->y_field,
			    NULL, NULL, NULL, NULL,
			    cgen->pos_cb);

	/* Width */
        n = 0;
        item[n] = cgen->wpolicy_rbox_items.Longest_Item_item;
        item_val[n] = SIZE_OF_CONTENTS_KEY; n++;
        item[n] = cgen->wpolicy_rbox_items.Fixed_item;
        item_val[n] = SIZE_FIXED_KEY; n++;
        prop_radiobox_init(&(pls->wth_policy), cgen->wpolicy_rbox_label,
                cgen->wpolicy_rbox, n, item, (XtPointer*)item_val,
                cgen->wpolicy_cb);

        for(i=0; i < n; i++)
            XtAddCallback(item[i], XmNvalueChangedCallback,
                        (XtCallbackProc)prop_size_policyCB, (XtPointer)&(pls->wth));

        prop_geomfield_init(&(pls->wth), cgen->wpolicy_rbox_label,
                            NULL, NULL, NULL, NULL,
                            cgen->width_field_label, cgen->width_field,
                            NULL, NULL,
                            cgen->wpolicy_cb);

        /* Height */
        prop_geomfield_init(&(pls->hgt), cgen->hgt_opmenu_label,
			    NULL, NULL, NULL, NULL,
                            NULL, NULL,
                            cgen->height_field_label, cgen->height_field,
                            cgen->hgt_cb);

	n = 0;
	item[n] = cgen->hgt_opmenu_items.Lines_item;
	item_val[n] = SIZE_IN_CHARS_KEY; n++;
	item[n] = cgen->hgt_opmenu_items.Pixels_item;
	item_val[n] = SIZE_IN_PIXELS_KEY; n++;
	prop_options_init(&(pls->hgt_metric), cgen->hgt_opmenu_label,
			   cgen->hgt_opmenu,
			   cgen->hgt_opmenu_menu,
			   n, item, (XtPointer*)item_val,
			   cgen->hgt_cb);

        /* Initial State */
        n = 0;
        item[n] = cgen->istate_ckbox_items.Visible_item;
        item_val[n] = AB_STATE_VISIBLE; n++;
        item[n] = cgen->istate_ckbox_items.Active_item;
        item_val[n] = AB_STATE_ACTIVE; n++;
        prop_checkbox_init(&(pls->init_state), cgen->istate_ckbox_label,
                cgen->istate_ckbox, n, item, item_val,
                cgen->istate_cb);

        /* Color */
        prop_colorfield_init(&(pls->bg_color), cgen->bg_mbutton,
		cgen->bg_mbutton_bg_mbutton_menu_items.None_item,
		cgen->bg_mbutton_bg_mbutton_menu_items.Color_Chooser_item,
                cgen->bg_swatch, cgen->bg_field, cgen->bg_cb);

        prop_colorfield_init(&(pls->fg_color), cgen->fg_mbutton,
                cgen->fg_mbutton_fg_mbutton_menu_items.None_item,
                cgen->fg_mbutton_fg_mbutton_menu_items.Color_Chooser_item,
                cgen->fg_swatch, cgen->fg_field, cgen->fg_cb);

	/* Menu Title */
        prop_field_init(&(pls->menu_title), cgen->menutitle_field_label,
                            cgen->menutitle_field, cgen->menutitle_cb);

        /* Menu Name Setting */
        prop_menuname_init(&(pls->menuname), type, cgen->menu_label,
			cgen->menu_mbutton, cgen->menu_field,
                        cgen->name_field, cgen->menu_cb,
			&(pls->menu_title),
			&(pls->current_obj), True);

        /* Item Editor....*/

        /* Item Label */
        prop_field_init(&(pls->item_label), cgen->itemlabel_label,
                cgen->itemlabel_field, cgen->itemlist_cb);

        /* Item State */
        n = 0;
        item[n] = cgen->itemstate_ckbox_items.Selected_item;
        item_val[n] = AB_STATE_SELECTED; n++;
        prop_checkbox_init(&(pls->item_state), NULL,
                cgen->itemstate_ckbox, n, item, item_val,
                cgen->itemlist_cb);

	/* Store Items->Insert menu items in array */
	n = 0;
        item[n] = cgen->item_edit_mbutton_editmenu_items.Add_Before_item;
        item_val[n] = INSERT_BEFORE; n++;
        item[n] = cgen->item_edit_mbutton_editmenu_items.Add_After_item;
        item_val[n] = INSERT_AFTER; n++;

	/* Store Items->Edit menu_items.items in array */
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

	/* Hook up Item Editing mechanism to Item List */
	prop_item_editor_init(&(pls->items), AB_ITEM_FOR_LIST,
		cgen->itemlist, cgen->itemlist_cb,
		cgen->item_add_button,
		n, item, item_val, /* Insert */
		j, item2, item2_val,/* Edit */
		&(pls->item_label), NULL/*label_type*/, NULL/*graphic_hint*/,
		NULL/*mnemonic*/, NULL/*accel*/, NULL/*line_style*/,
		&(pls->item_state), NULL/*submenu*/,
		&(pls->current_obj));

        prop_changebars_cleared(pls->prop_sheet);

	return (cgen->prop_dialog_shellform);
    }
    else
    	return NULL;

}

static int
list_prop_activate(
    AB_PROP_TYPE type,
    BOOL	 active
)
{
    ui_set_active(prop_list_settings_rec[type].prop_sheet, active);

    return OK;

}

static int
list_prop_clear(
    AB_PROP_TYPE type
)
{
    PropListSettingsRec	*pls = &(prop_list_settings_rec[type]);

    if (pls->current_obj == NULL)
	return OK;

    /* Clear Name */
    prop_field_set_value(&(pls->name), "", False);

    /* Clear Label Type/Position */
    prop_options_set_value(&(pls->label_type), (XtPointer)AB_LABEL_STRING, False);
    prop_options_set_value(&(pls->label_pos), (XtPointer)AB_CP_WEST, False);

    /* Clear Label */
    ui_set_label_string(pls->label.label, "Label:");
    prop_field_set_value(&(pls->label), "", False);

    /* Clear Selection Mode */
    prop_options_set_value(&(pls->sel_mode), (XtPointer)AB_SELECT_SINGLE, False);

    /* Clear Position */
    prop_geomfield_clear(&(pls->pos), GEOM_X);
    prop_geomfield_clear(&(pls->pos), GEOM_Y);

    /* Clear Width */
    prop_radiobox_set_value(&(pls->wth_policy),
                            (XtPointer)SIZE_FIXED_KEY, False);
    prop_geomfield_clear(&(pls->wth), GEOM_WIDTH);

    /* Clear Height */
    prop_geomfield_clear(&(pls->hgt), GEOM_HEIGHT);

    /* Clear Initial State */
    prop_checkbox_set_value(&(pls->init_state), AB_STATE_VISIBLE, True, False);
    prop_checkbox_set_value(&(pls->init_state), AB_STATE_ACTIVE, True, False);

    /* Clear Color */
    prop_colorfield_set_value(&(pls->bg_color), "", False);
    prop_colorfield_set_value(&(pls->fg_color), "", False);

    /* Clear Menu Name/Title */
    prop_menuname_set_value(&(pls->menuname), "", False);
    prop_field_set_value(&(pls->menu_title), "", False);

    /* Clear Items */
    prop_item_editor_clear(&(pls->items));

    pls->current_obj = NULL;

    turnoff_changebars(type);

    return OK;
}


static int
list_prop_load(
    ABObjPtr 	 obj,
    AB_PROP_TYPE type,
    unsigned long loadkey
)
{
    PropListSettingsRec *pls = &(prop_list_settings_rec[type]);
    int			height;
    BOOL		load_all = (loadkey & LoadAll);
    BOOL		editable;

    if (obj == NULL)
    {
        if (pls->current_obj != NULL)
            obj = pls->current_obj;
        else
            return ERROR;
    }
    else if (!obj_is_list(obj))
        return ERROR;
    else
        pls->current_obj = obj;

    /* Load Name */
    if (load_all || loadkey & LoadName)
    	prop_field_set_value(&(pls->name), obj_get_name(obj), False);

    if (load_all)
    {
	/* Load Label Type/Position */
	prop_options_set_value(&(pls->label_type), (XtPointer)obj->label_type, False);
	prop_options_set_value(&(pls->label_pos), (XtPointer)obj_get_label_position(obj), False);

	/* Load Label */
	prop_setup_label_field(&(pls->label), NULL,
				obj->label_type, obj_get_label(obj), AB_LINE_UNDEF);

	/* Load Selection Mode */
	prop_options_set_value(&(pls->sel_mode),
		(XtPointer)((AB_SELECT_TYPE)obj_get_selection_mode(obj)), False);

	/* Load Initial State */
	prop_checkbox_set_value(&(pls->init_state), AB_STATE_VISIBLE,
		obj_is_initially_visible(obj), False);
	prop_checkbox_set_value(&(pls->init_state), AB_STATE_ACTIVE,
		obj_is_initially_active(obj), False);

	/* Load Color */
	prop_colorfield_set_value(&(pls->bg_color), obj_get_bg_color(obj), False);
	prop_colorfield_set_value(&(pls->fg_color), obj_get_fg_color(obj), False);

	/* Load Menu Name/Title */
	prop_menuname_set_value(&(pls->menuname), obj_get_menu_name(obj), False);
	prop_field_set_value(&(pls->menu_title), obj_get_menu_title(obj), False);

	/* Load Items */
	prop_item_editor_load(&(pls->items), obj);

	turnoff_changebars(type);
    }

    /* Load Position */
    if (load_all || loadkey & LoadPosition)
        prop_load_obj_position(obj, &(pls->pos));

    /* Load Size */
    if (load_all || loadkey & LoadSize)
    {
	/* Load Width */
        editable = abobj_width_resizable(obj);
        prop_radiobox_set_value(&(pls->wth_policy), editable?
                                (XtPointer)SIZE_FIXED_KEY :
                                (XtPointer)SIZE_OF_CONTENTS_KEY, False);

        ui_set_active(pls->wth.w_field, editable);
        ui_set_active(pls->wth.w_label, editable);

        prop_geomfield_set_value(&(pls->wth), GEOM_WIDTH,
                abobj_get_comp_width(obj), False);

        /* Load Height */
        if (obj_get_num_rows(obj) == -1) /* Size in Pixels */
        {
            prop_options_set_value(&(pls->hgt_metric), (XtPointer)SIZE_IN_PIXELS_KEY, False);
            height = abobj_get_comp_height(obj);
        }
        else /* Size in Chars */
        {
            prop_options_set_value(&(pls->hgt_metric), (XtPointer)SIZE_IN_CHARS_KEY, False);
            height = obj_get_num_rows(obj);
        }
        prop_geomfield_set_value(&(pls->hgt), GEOM_HEIGHT, height, False);
    }

    return OK;

}

static int
list_prop_apply(
    AB_PROP_TYPE   type
)
{
    PropListSettingsRec 	*pls = &(prop_list_settings_rec[type]);
    ABObj			szObj;
    STRING	    		value;
    int				width_policy, metric, new_w, new_h;
    BOOL	    		size_chg = False;
    BOOL	    		reset_bg = False;
    BOOL	    		reset_fg = False;

    if (!verify_props(type))
        return ERROR;

    if (prop_changed(pls->name.changebar))
    {
        value = prop_field_get_value(&(pls->name));
        abobj_set_name(pls->current_obj, value);
        util_free(value);
    }
    if (prop_changed(pls->label.changebar) ||
	prop_changed(pls->label_type.changebar))
    {
        value = prop_field_get_value(&(pls->label));

        abobj_set_label(pls->current_obj,
            (AB_LABEL_TYPE)prop_options_get_value(&(pls->label_type)),
                        value);
        util_free(value);

        abobj_set_label_position(pls->current_obj,
                (AB_COMPASS_POINT)prop_options_get_value(&(pls->label_pos)));

	size_chg = True;
    }
    if (prop_changed(pls->sel_mode.changebar))
    {
        abobj_set_selection_mode(pls->current_obj,
            (AB_SELECT_TYPE)prop_options_get_value(&(pls->sel_mode)));
    }
    if (prop_changed(pls->pos.changebar))
    {
	if (abobj_is_movable(pls->current_obj))
	    abobj_set_xy(pls->current_obj,
	     	prop_geomfield_get_value(&(pls->pos), GEOM_X),
	     	prop_geomfield_get_value(&(pls->pos), GEOM_Y));
    }
    if (prop_changed(pls->wth_policy.changebar))
    {
	width_policy = prop_radiobox_get_value(&(pls->wth_policy));
	abobj_set_size_policy(pls->current_obj, width_policy == SIZE_FIXED_KEY);

	if (width_policy == SIZE_FIXED_KEY)
	{
	    new_w = prop_geomfield_get_value(&(pls->wth), GEOM_WIDTH);
	    abobj_set_pixel_width(pls->current_obj, new_w, 0);
	}
	size_chg = True;
    }
    if (prop_changed(pls->hgt.changebar))
    {
	metric = (int)(intptr_t) prop_options_get_value(&(pls->hgt_metric));
	new_h = prop_geomfield_get_value(&(pls->hgt), GEOM_HEIGHT);

        abobj_set_num_rows(pls->current_obj,
		metric == SIZE_IN_CHARS_KEY? new_h : -1);

	abobj_set_pixel_height(pls->current_obj,
		metric == SIZE_IN_PIXELS_KEY? new_h : -1, 0);

        size_chg = True;
    }
    if (prop_changed(pls->init_state.changebar))
    {
        abobj_set_visible(pls->current_obj,
		prop_checkbox_get_value(&(pls->init_state), AB_STATE_VISIBLE));
        abobj_set_active(pls->current_obj,
		prop_checkbox_get_value(&(pls->init_state), AB_STATE_ACTIVE));
    }
    if (prop_changed(pls->fg_color.changebar))
    {
        value = prop_colorfield_get_value(&(pls->fg_color));
        abobj_set_foreground_color(pls->current_obj, value);
	if (util_strempty(value))
	    reset_fg = True;
        util_free(value);
        prop_colorfield_set_value(&(pls->fg_color),
		obj_get_fg_color(pls->current_obj), False);
    }
    if (prop_changed(pls->bg_color.changebar))
    {
        value = prop_colorfield_get_value(&(pls->bg_color));
        abobj_set_background_color(pls->current_obj, value);
	if (util_strempty(value))
	    reset_bg = True;
        util_free(value);
        prop_colorfield_set_value(&(pls->bg_color),
		obj_get_bg_color(pls->current_obj), False);
    }
    if (prop_changed(pls->menuname.changebar))
    {
        value = prop_menuname_get_value(&(pls->menuname));
        abobj_set_menu_name(pls->current_obj, value);
        util_free(value);
        prop_menuname_set_value(&(pls->menuname),
		obj_get_menu_name(pls->current_obj), False);
    }
    if (prop_changed(pls->menu_title.changebar))
    {
        value = prop_field_get_value(&(pls->menu_title));
        abobj_set_menu_title(pls->current_obj, value);
        util_free(value);
    }
    if (prop_changed(pls->items.changebar))
    {
        /* Ensure edits to current item are saved before apply */
        prop_item_change(&(pls->items), False);
        prop_item_editor_apply(&(pls->items));
        size_chg = True;
    }

    abobj_tree_instantiate_changes(pls->current_obj);

    if (reset_bg || reset_fg) /* Set back to No Color */
        abobj_reset_colors(pls->current_obj, reset_bg, reset_fg);

    if (size_chg)
    {
	/* Need to force XmList to be fixed width, since often it's
	 * finicky and snaps back to the width of the longest item...
	 */
	if (obj_get_width(pls->current_obj) != -1)
	{
            szObj = objxm_comp_get_subobj(pls->current_obj, AB_CFG_SIZE_OBJ);
            XtVaSetValues(objxm_get_widget(szObj),
                XmNwidth, obj_get_width(pls->current_obj),
                NULL);
	}
	abobj_force_dang_form_resize(pls->current_obj);
    }
    turnoff_changebars(type);

    return OK;
}

static BOOL
list_prop_pending(
    AB_PROP_TYPE type
)
{
    return(prop_changebars_pending(prop_list_settings_rec[type].prop_sheet));
}

static BOOL
verify_props(
    AB_PROP_TYPE type
)
{
    PropListSettingsRec *pls = &(prop_list_settings_rec[type]);

    if (prop_changed(pls->name.changebar) &&
	!prop_name_ok(pls->current_obj, pls->name.field))
        return False;

    if ((prop_changed(pls->label_type.changebar) || prop_changed(pls->label.changebar)) &&
	prop_options_get_value(&(pls->label_type)) == (XtPointer)AB_LABEL_GLYPH &&
        !prop_graphic_filename_ok(pls->label.field, False))
        return False;

    if (prop_changed(pls->pos.changebar) &&
        (!prop_number_ok(pls->pos.x_field, (STRING)XFieldStr, -SHRT_MAX, SHRT_MAX) ||
         !prop_number_ok(pls->pos.y_field, (STRING)YFieldStr, -SHRT_MAX, SHRT_MAX)))
        return False;

    if (prop_changed(pls->wth.changebar) &&
        !prop_number_ok(pls->wth.w_field, (STRING)WFieldStr, 1, SHRT_MAX))
        return False;

    if (prop_changed(pls->hgt.changebar) &&
        !prop_number_ok(pls->hgt.h_field, (STRING)HFieldStr, 1, SHRT_MAX))
        return False;

    if (prop_changed(pls->fg_color.changebar) && !prop_color_ok(pls->fg_color.field))
        return False;

    if (prop_changed(pls->bg_color.changebar) && !prop_color_ok(pls->bg_color.field))
        return False;

    if (prop_changed(pls->menuname.changebar) && !prop_obj_name_ok(pls->menuname.field,
                        obj_get_module(pls->current_obj), AB_TYPE_MENU, "Menu"))
        return False;

    return True;
}

static void
turnoff_changebars(
    AB_PROP_TYPE type
)
{
    PropListSettingsRec *pls = &(prop_list_settings_rec[type]);

    prop_set_changebar(pls->name.changebar,    	PROP_CB_OFF);
    prop_set_changebar(pls->sel_mode.changebar,	PROP_CB_OFF);
    prop_set_changebar(pls->wth_policy.changebar,PROP_CB_OFF);
    prop_set_changebar(pls->hgt.changebar,	 PROP_CB_OFF);
    prop_set_changebar(pls->pos.changebar,	 PROP_CB_OFF);
    prop_set_changebar(pls->label_type.changebar,PROP_CB_OFF);
    prop_set_changebar(pls->label.changebar,     PROP_CB_OFF);
    prop_set_changebar(pls->init_state.changebar,PROP_CB_OFF);
    prop_set_changebar(pls->bg_color.changebar,  PROP_CB_OFF);
    prop_set_changebar(pls->fg_color.changebar,  PROP_CB_OFF);
    prop_set_changebar(pls->menuname.changebar,  PROP_CB_OFF);
    prop_set_changebar(pls->menu_title.changebar,  PROP_CB_OFF);
    prop_set_changebar(pls->items.changebar,  	 PROP_CB_OFF);

    prop_changebars_cleared(pls->prop_sheet);

}

/*
 * Object destroy callback
 */
static int
prop_list_install_obj_destroy_CB(void)
{
    static BOOL	callback_installed = False;

    if (callback_installed)
    {
	return 0;
    }
    obj_add_destroy_callback(prop_list_obj_destroy_CB, "List Props");
    return 0;
}


static int
prop_list_obj_destroy_CB(ObjEvDestroyInfo destroyInfo)
{
    int			i;
    ABObj		doomedObj = destroyInfo->obj;
    PropListSettingsRec *pls;

    if (!obj_is_list_item(doomedObj))
    {
	return 0;
    }

    for (i = 0; i < AB_PROP_TYPE_NUM_VALUES; ++i)
    {
	pls = &(prop_list_settings_rec[i]);

	if (pls->current_obj == doomedObj)
	    pls->current_obj = NULL;
	if (pls->items.current_item == doomedObj)
	    pls->items.current_item = NULL;
	if (pls->items.clipboard_item == doomedObj)
	    pls->items.clipboard_item = NULL;
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
    PropListSettingsRec         *pls = &(prop_list_settings_rec[type]);
    XtArgVal			value;

    /* Width field should ONLY be editable if Size Policy
     * is "Fixed"
     */
    if (state->set)
    {
    	XtVaGetValues(w, XmNuserData, &value, NULL);
    	ui_field_set_editable(pls->wth.w_field, value == SIZE_FIXED_KEY);
    }
}
