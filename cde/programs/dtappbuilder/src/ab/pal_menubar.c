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
 *      $XConsortium: pal_menubar.c /main/4 1996/08/07 19:49:33 mustafa $
 *
 * @(#)pal_menubar.c	1.36 15 Feb 1994      cde_app_builder/src/ab
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
 * pal_menubar.c - Implements Palette Menubar object functionality
 */
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
#include "menubar_ui.h"


typedef struct  PROP_MENUBAR_SETTINGS
{
    Widget                      prop_sheet;
    PropFieldSettingRec         name;
    PropCheckboxSettingRec      init_state;
    PropColorSettingRec         bg_color;
    PropColorSettingRec         fg_color;
    PropFieldSettingRec         item_label;
    PropOptionsSettingRec       item_label_type;
    PropFieldSettingRec         item_mnemonic;
    PropCheckboxSettingRec      item_state;
    PropMenunameSettingRec      item_menuname;
    PropItemsSettingRec         items;
    ABObj                       current_obj;
} PropMenubarSettingsRec, *PropMenubarSettings;

/*************************************************************************
**                                                                      **
**       Private Function Declarations                                  **
**                                                                      **
**************************************************************************/
/*
 * Methods
 */
static int      menubar_initialize(
                    ABObj   obj
                );
static Widget   menubar_prop_init(
                    Widget  parent,
		    AB_PROP_TYPE type
                );
static int	menubar_prop_activate(
		    AB_PROP_TYPE type,
		    BOOL 	 active
		);
static int      menubar_prop_load(
                    ABObj   	 obj,
		    AB_PROP_TYPE type,
		    unsigned long loadkey
                );
static int      menubar_prop_clear(
                    AB_PROP_TYPE type
                );
static int	menubar_prop_apply(
		    AB_PROP_TYPE type
		);
static BOOL	menubar_prop_pending(
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
static int	prop_menubar_obj_destroy_CB(
		    ObjEvDestroyInfo destroyInfo
		);
static int 	prop_menubar_install_obj_destroy_CB(void);


/*
 * Xt Callbacks
 */

/*************************************************************************
**                                                                      **
**       Data  		                                                 **
**                                                                      **
**************************************************************************/
PalItemInfo menubar_palitem_rec = {

    /* type             */  AB_TYPE_CONTAINER,
    /* name             */  "Menubar",
    /* animation pixmaps*/  NULL,
    /* number of pixmaps*/  0,
    /* rev_prop_frame   */  NULL,
    /* fix_prop_dialog  */  NULL,
    /* initialize       */  menubar_initialize,
    /* is_a_test        */  obj_is_menubar,
    /* prop_initialize  */  menubar_prop_init,
    /* prop_activate	*/  menubar_prop_activate,
    /* prop_clear	*/  menubar_prop_clear,
    /* prop_load        */  menubar_prop_load,
    /* prop_apply	*/  menubar_prop_apply,
    /* prop_pending	*/  menubar_prop_pending

};

PalItemInfo *ab_menubar_palitem = &menubar_palitem_rec;
PropMenubarSettingsRec prop_menubar_settings_rec[AB_PROP_TYPE_NUM_VALUES];


/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/
static int
menubar_initialize(
    ABObj    obj
)
{
    ABObj	iobj;
    ABObj	module = obj_get_module(obj);
    String  cascades[3];
    int		i;

    obj_set_unique_name(obj, "menubar");

    obj->x = 0;
    obj->y = 0;
    obj_set_is_initially_visible(obj, True);
    obj_set_is_initially_active(obj, True);

    /* Add initial cascades to Choice */
    cascades[0] = catgets(Dtb_project_catd, 6, 20, "File");
    cascades[1] = catgets(Dtb_project_catd, 6, 21, "Edit");
    cascades[2] = catgets(Dtb_project_catd, 6, 25, "Help");
    for (i=0; i < XtNumber(cascades); i++)
    {
        iobj = obj_create(AB_TYPE_ITEM, NULL);
	obj_set_subtype(iobj, AB_ITEM_FOR_MENUBAR);
	obj_append_child(obj, iobj);
        iobj->label_type = AB_LABEL_STRING;
	obj_set_is_initially_active(iobj, True);
	obj_set_is_initially_visible(iobj, True);
	abobj_set_item_name(iobj, obj_get_module(obj), obj_get_name(obj), cascades[i]);
        obj_set_label(iobj, cascades[i]);
	if (i == 2)
	    obj_set_is_help_item(iobj, True);

    }
    return OK;

}

static Widget
menubar_prop_init(
    Widget    parent,
    AB_PROP_TYPE type
)
{
    DtbMenubarPropDialogInfoRec	rev_menubar_prop_dialog; /* Revolving Props */
    DtbMenubarPropDialogInfo	cgen = &dtb_menubar_prop_dialog; /* Codegen structure */
    DtbRevolvPropDialogInfo	rpd = &(dtb_revolv_prop_dialog);
    PropMenubarSettingsRec    	*pms = &(prop_menubar_settings_rec[type]);
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
        dtbMenubarPropDialogInfo_clear(&rev_menubar_prop_dialog);

	cgen = &(rev_menubar_prop_dialog);
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
    	prop_menubar_install_obj_destroy_CB();

    }
    else /* AB_PROP_FIXED */
    	cgen = &dtb_menubar_prop_dialog; 

    if (dtb_menubar_prop_dialog_initialize(cgen, parent) == 0)
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
            prop_fixed_dialog_init(ab_menubar_palitem,
			cgen->prop_dialog_shellform, cgen->objlist);
            prop_activate_panel_init(type, ab_menubar_palitem, 
                        cgen->ok_button, cgen->apply_button, 
                        cgen->reset_button, cgen->cancel_button,
                        cgen->help_button);
	}

        /* Alternate Editor Buttons */
        prop_editors_panel_init(type, ab_menubar_palitem,
            NULL, cgen->conn_button, cgen->helptxt_button);

        /*
          * Prop Sheet Settings....
         */

        /* Name */
        prop_field_init(&(pms->name), cgen->name_field_label,
                cgen->name_field, cgen->name_cb);

        /* Initial State */
        n = 0;
        item[n] = cgen->istate_ckbox_items.Active_item;
        item_val[n] = AB_STATE_ACTIVE; n++;
        prop_checkbox_init(&(pms->init_state), cgen->istate_ckbox_label,
                cgen->istate_ckbox, n, item, item_val,
                cgen->istate_cb);

        /* Background */
        prop_colorfield_init(&(pms->bg_color), cgen->bg_mbutton, 
		cgen->bg_mbutton_bg_mbutton_menu_items.None_item,
		cgen->bg_mbutton_bg_mbutton_menu_items.Color_Chooser_item,
                cgen->bg_swatch, cgen->bg_field, cgen->bg_cb);

        /* Foreground */
        prop_colorfield_init(&(pms->fg_color), cgen->fg_mbutton,
                cgen->fg_mbutton_fg_mbutton_menu_items.None_item,  
                cgen->fg_mbutton_fg_mbutton_menu_items.Color_Chooser_item, 
                cgen->fg_swatch, cgen->fg_field, cgen->fg_cb); 

	/* Item Editor....*/

	/* Item Label */
        prop_field_init(&(pms->item_label), cgen->item_label_label,
                cgen->itemlabel_field, cgen->itemlist_cb);
        n = 0;
        item[n] = cgen->itemlabel_opmenu_items.String_item;
        item_val[n] = AB_LABEL_STRING; n++;
        item[n] = cgen->itemlabel_opmenu_items.Graphic_item;
        item_val[n] = AB_LABEL_GLYPH; n++;
        prop_options_init(&(pms->item_label_type), cgen->item_labeltype_label,
                cgen->itemlabel_opmenu, cgen->itemlabel_opmenu_menu,
                n, item, (XtPointer*)item_val, cgen->itemlist_cb);

        for(i=0; i < n; i++)
            XtAddCallback(item[i], XmNactivateCallback,
                  (XtCallbackProc)prop_item_labeltypeCB, (XtPointer)&(pms->items));

	/* Item Mnemonic */
        prop_field_init(&(pms->item_mnemonic), cgen->mnemonic_label,
                cgen->mnemonic_field, cgen->itemlist_cb);

	/* Item State */
        n = 0;
        item[n] = cgen->itemstate_ckbox_items.Active_item;
        item_val[n] = AB_STATE_ACTIVE; n++;
	item[n] = cgen->itemstate_ckbox_items.Is_Help_Item_item;
        item_val[n] = HELP_ITEM_KEY; n++;
        prop_checkbox_init(&(pms->item_state), cgen->itemstate_label,
                cgen->itemstate_ckbox, n, item, item_val,
                cgen->itemlist_cb);

	/* Item Menu Name */
        prop_menuname_init(&(pms->item_menuname), type, cgen->menu_label,
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

	/* Hook up Item Editing mechanism to Item List */
	prop_item_editor_init(&(pms->items), AB_ITEM_FOR_MENUBAR,
		cgen->itemlist, cgen->itemlist_cb, 
		cgen->item_add_button,
		n, item, item_val, /* Insert */
		j, item2, item2_val,/* Edit */
		&(pms->item_label), &(pms->item_label_type), cgen->graphic_hint,
		&(pms->item_mnemonic), NULL/*accel*/, NULL/*line_style*/,
		&(pms->item_state), &(pms->item_menuname),
		&(pms->current_obj));

        prop_changebars_cleared(pms->prop_sheet);

	return (cgen->prop_dialog_shellform);
    }
    else
    	return NULL;

}

static int
menubar_prop_activate(
    AB_PROP_TYPE type,
    BOOL	 active
)
{
    ui_set_active(prop_menubar_settings_rec[type].prop_sheet, active);

    return OK;
}


static int
menubar_prop_clear(
    AB_PROP_TYPE type
)
{
    PropMenubarSettingsRec	*pms = &(prop_menubar_settings_rec[type]);

    if (pms->current_obj == NULL)
	return OK;

    /* Clear Name */
    prop_field_set_value(&(pms->name), "", False);

    /* Clear Initial State */
    prop_checkbox_set_value(&(pms->init_state), AB_STATE_ACTIVE, True, False);

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
menubar_prop_load(
    ABObjPtr 	 obj,
    AB_PROP_TYPE type,
    unsigned long loadkey
)
{
    PropMenubarSettingsRec 	*pms = &(prop_menubar_settings_rec[type]);
    BOOL			load_all = (loadkey & LoadAll);
 
    if (obj == NULL)
    {
        if (pms->current_obj != NULL)
            obj = pms->current_obj;
        else
            return ERROR;
    }
    else if (!obj_is_menubar(obj))
        return ERROR;
    else
        pms->current_obj = obj;

    /* Load Name */
    if (load_all || loadkey & LoadName)
    	prop_field_set_value(&(pms->name), obj_get_name(obj), False);

    if (load_all)
    {
	/* Load Initial State */
	prop_checkbox_set_value(&(pms->init_state), AB_STATE_ACTIVE,
		obj_is_initially_active(obj), False);

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
menubar_prop_apply(
    AB_PROP_TYPE   type
)
{
    PropMenubarSettingsRec 	*pms = &(prop_menubar_settings_rec[type]);
    STRING	    		value;
    ABObj	    		mb_obj;
    ABObj	    		help_item;
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
    if (prop_changed(pms->init_state.changebar))
    {
        abobj_set_active(pms->current_obj,
		prop_checkbox_get_value(&(pms->init_state), AB_STATE_ACTIVE));
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

    abobj_tree_instantiate_changes(pms->current_obj);

    if (reset_bg || reset_fg) /* Set back to No Color */ 
        abobj_reset_colors(pms->current_obj, reset_bg, reset_fg); 

    /* We must set the Help Cascade AFTER changes have been
     * instantiated because we require the live Widget-ID of
     * Help-cascade and this is the only order which guarantees it.
     */  
    help_item = obj_get_menubar_help_item(pms->current_obj);
    mb_obj = objxm_comp_get_subobj(pms->current_obj, AB_CFG_OBJECT_OBJ);

    XtVaSetValues(objxm_get_widget(mb_obj),
                XmNmenuHelpWidget, help_item? objxm_get_widget(help_item): NULL,
                NULL);

    turnoff_changebars(type);

    return OK;
}

static BOOL
menubar_prop_pending(
    AB_PROP_TYPE type
)
{
    return(prop_changebars_pending(prop_menubar_settings_rec[type].prop_sheet));
}

static BOOL
verify_props(
    AB_PROP_TYPE type
)
{
    PropMenubarSettingsRec *pms = &(prop_menubar_settings_rec[type]);

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
    PropMenubarSettingsRec *pms = &(prop_menubar_settings_rec[type]);

    prop_set_changebar(pms->name.changebar,    	PROP_CB_OFF);
    prop_set_changebar(pms->init_state.changebar,PROP_CB_OFF);
    prop_set_changebar(pms->bg_color.changebar,  PROP_CB_OFF);
    prop_set_changebar(pms->fg_color.changebar,  PROP_CB_OFF);
    prop_set_changebar(pms->items.changebar,  	 PROP_CB_OFF);

    prop_changebars_cleared(pms->prop_sheet);

}

/*
 * Object destroy callback
 */
static int
prop_menubar_install_obj_destroy_CB(void)
{
    static BOOL	callback_installed = False;

    if (callback_installed)
    {
	return 0;
    }
    obj_add_destroy_callback(prop_menubar_obj_destroy_CB, "Menubar Props");
    return 0;
}

static int
prop_menubar_obj_destroy_CB(ObjEvDestroyInfo destroyInfo)
{
    int			i;
    ABObj		doomedObj = destroyInfo->obj;
    PropMenubarSettingsRec *pms;


    if (!   (obj_is_item(doomedObj) && 
	    (obj_get_item_type(doomedObj) == AB_ITEM_FOR_MENUBAR)) )
    {
	return 0;
    }

    for (i = 0; i < AB_PROP_TYPE_NUM_VALUES; ++i)
    {
	pms = &(prop_menubar_settings_rec[i]);

	if (pms->current_obj == doomedObj)
	    pms->current_obj = NULL;
	if (pms->items.current_item == doomedObj)
	    pms->items.current_item = NULL;
	if (pms->items.clipboard_item == doomedObj)
	    pms->items.clipboard_item = NULL;
    }

    return 0;
}
