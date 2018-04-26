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
 *      $XConsortium: pal_button.c /main/5 1996/08/08 17:50:37 mustafa $
 *
 * @(#)pal_button.c	1.82 11 Aug 1995      cde_app_builder/src/ab
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
 * pal_button.c - Implements Palette Button object functionality
 */
#include <stdio.h>
#include <Xm/Xm.h>
#include <ab_private/ab.h>
#include <ab_private/pal.h>
#include <ab_private/prop.h>
#include <ab_private/abobj.h>
#include <ab_private/abobj_set.h>
#include <ab_private/ui_util.h>
#include "button_ui.h"

#define	LABEL_TYPE_STRING	0
#define LABEL_TYPE_GLYPH	1
#define LABEL_TYPE_ARROW	2

typedef struct  PROP_BUTTON_SETTINGS
{
    Widget                      prop_sheet;
    PropFieldSettingRec         name;
    PropRadioSettingRec         button_type;
    PropOptionsSettingRec       label_type;
    Widget                      string_item;
    Widget                      glyph_item;
    Widget                      arrow_item;
    Widget			graphic_hint;
    PropFieldSettingRec         label;
    PropOptionsSettingRec       arrow_dir;
    PropOptionsSettingRec       label_align;
    PropMenunameSettingRec      menuname;
    PropRadioSettingRec         size_policy;
    PropGeometrySettingRec      geometry;
    PropCheckboxSettingRec      init_state;
    PropColorSettingRec         bg_color;
    PropColorSettingRec         fg_color;
    ABObj                       current_obj;
} PropButtonSettingsRec, *PropButtonSettings;

/*************************************************************************
**                                                                      **
**       Private Function Declarations                                  **
**                                                                      **
**************************************************************************/
/*
 * Methods
 */
static int      button_initialize(
                    ABObj   obj
                );
static Widget   button_prop_init(
                    Widget  parent,
		    AB_PROP_TYPE type
                );
static int	button_prop_activate(
		    AB_PROP_TYPE type,
		    BOOL 	 active
		);
static int	button_prop_clear(
		    AB_PROP_TYPE type
		);
static int      button_prop_load(
                    ABObj   	 obj,
		    AB_PROP_TYPE type,
 		    unsigned long loadkey
                );
static int	button_prop_apply(
		    AB_PROP_TYPE type
		);
static BOOL	button_prop_pending(
		    AB_PROP_TYPE type
		);

/*
 * General routines
 */
static BOOL	verify_props(
		    AB_PROP_TYPE 	type
		);
static void	turnoff_changebars(
		    AB_PROP_TYPE 	type
		);
static void	setup_labeltype_setting(
		    AB_PROP_TYPE 	type,
		    AB_BUTTON_TYPE 	btype
		);
static void	setup_label_field(
		    AB_PROP_TYPE 	type,
		    AB_LABEL_TYPE 	ltype
		);
/*
 * Callbacks
 */
static void     typeCB(
                    Widget   	widget,
                    XtPointer 	clientdata,
                    XmToggleButtonCallbackStruct *state
                );
static void	labeltypeCB(
		    Widget   	widget,
		    XtPointer 	clientdata,
		    XtPointer 	calldata
		);
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
PalItemInfo button_palitem_rec = {

    /* type             */  AB_TYPE_BUTTON,
    /* name             */  "Button",
    /* animation pixmaps*/  NULL,
    /* number of pixmaps*/  0,
    /* rev_prop_frame   */  NULL,
    /* fix_prop_dialog  */  NULL,
    /* initialize       */  button_initialize,
    /* is_a_test        */  obj_is_button,
    /* prop_initialize  */  button_prop_init,
    /* prop_active 	*/  button_prop_activate,
    /* prop_clear	*/  button_prop_clear,
    /* prop_load        */  button_prop_load,
    /* prop_apply	*/  button_prop_apply,
    /* prop_pending	*/  button_prop_pending

};

PalItemInfo *ab_button_palitem = &button_palitem_rec;
PropButtonSettingsRec prop_button_settings_rec[AB_PROP_TYPE_NUM_VALUES];

/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/
static int
button_initialize(
    ABObj    obj
)
{
    if (obj_get_subtype(obj) == AB_BUT_MENU)
    {
	obj_set_unique_name(obj, "menubutton");
	obj_set_label(obj, catgets(Dtb_project_catd, 6, 66, "Menu Button"));
    }
    else /* Push or Drawn */
    {
    	obj_set_unique_name(obj, "button");
	obj_set_label(obj, catgets(Dtb_project_catd, 6, 65, "Button"));
    }
    obj_set_is_initially_visible(obj, True);
    obj_set_is_initially_active(obj, True);

    obj_set_attachment(obj, AB_CP_NORTH, AB_ATTACH_POINT, NULL, obj->y);
    obj_set_attachment(obj, AB_CP_WEST,  AB_ATTACH_POINT, NULL, obj->x);

    return OK;

}

static Widget
button_prop_init(
    Widget    parent,
    AB_PROP_TYPE type
)
{
    DtbButtonPropDialogInfoRec	rev_button_prop_dialog; /* Revolving Props */
    DtbButtonPropDialogInfo	cgen;
    DtbRevolvPropDialogInfo	rpd = &(dtb_revolv_prop_dialog);
    PropButtonSettingsRec      *pbs = &(prop_button_settings_rec[type]);
    Widget                      item[6];
    int                         item_val[6];
    int                         n;
    int				i;

    if (type == AB_PROP_REVOLVING)
    {
	/* Cloning Trick:
	 * Only the Attributes ControlPanel needs to be created within
	 * the existing Revolving Prop dialog, so fill out all other
	 * fields with the Revolving Prop dialog equivelents, so the
	 * dtb initialize proc will skip those non-NULL fields...
	 */
        dtbButtonPropDialogInfo_clear(&rev_button_prop_dialog);

	cgen = &(rev_button_prop_dialog);
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
	cgen->reset_button = rpd->reset_button;
	cgen->ok_button = rpd->ok_button;
	cgen->cancel_button = rpd->cancel_button;
	cgen->help_button = rpd->help_button;

    	XtVaSetValues(parent,
			XmNuserData, pbs->current_obj,
			NULL);
    }
    else /* AB_PROP_FIXED */
    	cgen = &dtb_button_prop_dialog;

    if (dtb_button_prop_dialog_initialize(cgen, parent) == 0)
    {
        pbs->prop_sheet = cgen->attrs_ctrlpanel;
        pbs->current_obj = NULL;

	if (type == AB_PROP_REVOLVING)
        	XtVaSetValues(parent,
                        XmNuserData, pbs->current_obj,
                        NULL);

        /* Dialog/Object List */
	if (type == AB_PROP_FIXED)
	{
            prop_fixed_dialog_init(ab_button_palitem,
			cgen->prop_dialog_shellform, cgen->objlist);
            prop_activate_panel_init(type, ab_button_palitem,
                        cgen->ok_button, cgen->apply_button,
			cgen->reset_button, cgen->cancel_button,
			cgen->help_button);
	}

        /* Alternate Editor Buttons */
        prop_editors_panel_init(type, ab_button_palitem,
            cgen->attach_button, cgen->conn_button, cgen->helptxt_button);

        /*
          * Prop Sheet Settings....
         */

        /* Name */
        prop_field_init(&(pbs->name), cgen->name_field_label,
                cgen->name_field, cgen->name_cb);

        /* Button Type */
        n = 0;
        item[n] = cgen->buttontype_rbox_items.Push_item;
        item_val[n] = AB_BUT_PUSH; n++;
        item[n] = cgen->buttontype_rbox_items.Drawn_item;
        item_val[n] = AB_BUT_DRAWN; n++;
        item[n] = cgen->buttontype_rbox_items.Menu_item;
        item_val[n] = AB_BUT_MENU; n++;
        prop_radiobox_init(&(pbs->button_type), cgen->buttontype_rbox_label,
                                cgen->buttontype_rbox, n, item, (XtPointer*)item_val,
                                cgen->buttontype_cb);
        for(i=0; i < n; i++)
            XtAddCallback(item[i], XmNvalueChangedCallback,
                        (XtCallbackProc)typeCB, (XtPointer)type);

        /* Label Type */
        n = 0;
        item[n] = cgen->labeltype_rbox_items.String_item;
        item_val[n] = AB_LABEL_STRING; n++;
        item[n] = cgen->labeltype_rbox_items.Graphic_item;
        item_val[n] = AB_LABEL_GLYPH; n++;
        item[n] = cgen->labeltype_rbox_items.Arrow_item;
        item_val[n] = AB_LABEL_ARROW_DOWN; n++;
        prop_options_init(&(pbs->label_type), cgen->labeltype_rbox_label,
                        cgen->labeltype_rbox, cgen->labeltype_rbox_menu,
			n, item, (XtPointer*)item_val,
                        cgen->labeltype_cb);
        for(i=0; i < n; i++)
            XtAddCallback(item[i], XmNactivateCallback,
                        (XtCallbackProc)labeltypeCB, (XtPointer)type);

        /* special setup required for Button Type/Label Type */
        pbs->string_item = cgen->labeltype_rbox_items.String_item;
        pbs->glyph_item = cgen->labeltype_rbox_items.Graphic_item;
        pbs->arrow_item = cgen->labeltype_rbox_items.Arrow_item;
	pbs->graphic_hint = cgen->graphic_hint;

        /* Label */
        prop_field_init(&(pbs->label), cgen->label_field_label,
                cgen->label_field, cgen->label_cb);

	/* NOTE: we cannot use prop_label_field_init() because there are
	 * other label-types (arrow) which need to be handled and so
	 * we need to use a private labeltype callback (not propP_labeltypeCB).
	 * The following needs to be set in order for prop_setup_label_field()
	 * to work correctly.
	 */
	XtVaSetValues(cgen->label_field_label,
		XmNrecomputeSize, False,
		XmNuserData,	  (XtArgVal)cgen->graphic_hint,
		NULL);

        /* Label Align */
        n = 0;
        item[n] = cgen->labelalign_opmenu_items.Left_item;
        item_val[n] = AB_ALIGN_LEFT; n++;
        item[n] = cgen->labelalign_opmenu_items.Centered_item;
        item_val[n] = AB_ALIGN_CENTER; n++;
        item[n] = cgen->labelalign_opmenu_items.Right_item;
        item_val[n] = AB_ALIGN_RIGHT; n++;
        prop_options_init(&(pbs->label_align), cgen->labelalign_opmenu_label,
                cgen->labelalign_opmenu, cgen->labelalign_opmenu_menu,
                n, item, (XtPointer*)item_val,
                cgen->labelalign_cb);

        /* Arrow Direction */
        n = 0;
        item[n] = cgen->arrowdir_opmenu_items.Up_item;
        item_val[n] = AB_LABEL_ARROW_UP; n++;
        item[n] = cgen->arrowdir_opmenu_items.Down_item;
        item_val[n] = AB_LABEL_ARROW_DOWN; n++;
        item[n] = cgen->arrowdir_opmenu_items.Right_item;
        item_val[n] = AB_LABEL_ARROW_RIGHT; n++;
        item[n] = cgen->arrowdir_opmenu_items.Left_item;
        item_val[n] = AB_LABEL_ARROW_LEFT; n++;
        prop_options_init(&(pbs->arrow_dir), cgen->arrowdir_opmenu_label,
                cgen->arrowdir_opmenu, cgen->arrowdir_opmenu_menu,
                n, item, (XtPointer*)item_val,
                cgen->arrowdir_cb);

        /* Menu Name Setting */
        prop_menuname_init(&(pbs->menuname), type, cgen->menu_label,
			cgen->menu_mbutton, cgen->menu_field,
                        cgen->name_field, cgen->menu_cb, NULL/*title*/,
                        &(pbs->current_obj), True);

        /* Size Policy */
        n = 0;
        item[n] = cgen->sizepolicy_rbox_items.Size_of_Label_item;
        item_val[n] = SIZE_OF_CONTENTS_KEY; n++;
        item[n] = cgen->sizepolicy_rbox_items.Fixed_item;
        item_val[n] = SIZE_FIXED_KEY; n++;
        prop_radiobox_init(&(pbs->size_policy), cgen->sizepolicy_rbox_label,
                cgen->sizepolicy_rbox, n, item, (XtPointer*)item_val,
                cgen->sizepolicy_cb);

        for(i=0; i < n; i++)
            XtAddCallback(item[i], XmNvalueChangedCallback,
                        (XtCallbackProc)prop_size_policyCB, (XtPointer)&(pbs->geometry));

        /* Geometry */
        prop_geomfield_init(&(pbs->geometry), cgen->geom_label,
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
        prop_checkbox_init(&(pbs->init_state), cgen->istate_ckbox_label,
                cgen->istate_ckbox, n, item, item_val,
                cgen->istate_cb);

        /* Color */
        prop_colorfield_init(&(pbs->bg_color), cgen->bg_mbutton,
		cgen->bg_mbutton_bg_mbutton_menu_items.None_item,
		cgen->bg_mbutton_bg_mbutton_menu_items.Color_Chooser_item,
                cgen->bg_swatch, cgen->bg_field, cgen->bg_cb);

        prop_colorfield_init(&(pbs->fg_color), cgen->fg_mbutton,
                cgen->fg_mbutton_fg_mbutton_menu_items.None_item,
                cgen->fg_mbutton_fg_mbutton_menu_items.Color_Chooser_item,
                cgen->fg_swatch, cgen->fg_field, cgen->fg_cb);

        prop_changebars_cleared(pbs->prop_sheet);

	return (cgen->prop_dialog_shellform);
    }
    else
    	return NULL;

}

static int
button_prop_activate(
    AB_PROP_TYPE type,
    BOOL	 active
)
{
    ui_set_active(prop_button_settings_rec[type].prop_sheet, active);

    return OK;
}


static int
button_prop_clear(
    AB_PROP_TYPE type
)
{
    PropButtonSettingsRec	*pbs = &(prop_button_settings_rec[type]);

    if (pbs->current_obj == NULL)
	return OK;

    /* Clear Name */
    prop_field_set_value(&(pbs->name), "", False);

    /* Clear Button Type*/
    prop_radiobox_set_value(&(pbs->button_type), (XtPointer)AB_BUT_PUSH, False);

    /* Clear LabelType */
    prop_options_set_value(&(pbs->label_type), (XtPointer)AB_LABEL_STRING, False);
    ui_set_active(pbs->label.field, True);
    ui_set_active(pbs->label.label, True);

    /* Clear Label */
    ui_set_label_string(pbs->label.label, (STRING)LabelForString);
    prop_field_set_value(&(pbs->label), "", False);

    /* Clear Arrow Direction */
    prop_options_set_value(&(pbs->arrow_dir), (XtPointer)AB_LABEL_ARROW_DOWN, False);

    /* Clear Label Alignment */
    prop_options_set_value(&(pbs->label_align), (XtPointer)AB_ALIGN_CENTER, False);

    /* Clear Menu Name */
    prop_menuname_set_value(&(pbs->menuname), "", False);

    /* Clear Size Policy */
    prop_radiobox_set_value(&(pbs->size_policy),
                            (XtPointer)SIZE_OF_CONTENTS_KEY, False);

    /* Clear Geometry */
    prop_geomfield_clear(&(pbs->geometry), GEOM_X);
    prop_geomfield_clear(&(pbs->geometry), GEOM_Y);
    prop_geomfield_clear(&(pbs->geometry), GEOM_WIDTH);
    prop_geomfield_clear(&(pbs->geometry), GEOM_HEIGHT);

    /* Clear Initial State */
    prop_checkbox_set_value(&(pbs->init_state), AB_STATE_VISIBLE, True, False);
    prop_checkbox_set_value(&(pbs->init_state), AB_STATE_ACTIVE, True, False);

    /* Clear Color */
    prop_colorfield_set_value(&(pbs->bg_color), "", False);
    prop_colorfield_set_value(&(pbs->fg_color), "", False);

    pbs->current_obj = NULL;

    turnoff_changebars(type);

    return OK;
}

static int
button_prop_load(
    ABObjPtr 	  obj,
    AB_PROP_TYPE  type,
    unsigned long loadkey
)
{
    AB_BUTTON_TYPE 	   	btype;
    PropButtonSettingsRec 	*pbs = &(prop_button_settings_rec[type]);
    BOOL                        load_all = (loadkey & LoadAll);

    if (obj == NULL)
    {
        if (pbs->current_obj != NULL)
            obj = pbs->current_obj;
        else
            return ERROR;
    }
    else if (!obj_is_button(obj))
        return ERROR;
    else
        pbs->current_obj = obj;

    /* Load Name */
    if (load_all || loadkey & LoadName)
    	prop_field_set_value(&(pbs->name), obj_get_name(obj), False);

    if (load_all)
    {
    	/* Load Button Type */
    	btype = obj_get_button_type(obj);
    	prop_radiobox_set_value(&(pbs->button_type), (XtPointer)btype, False);

	/* Load Label Type */
	switch (obj_get_label_type(obj))
	{
		case AB_LABEL_ARROW_DOWN:
		case AB_LABEL_ARROW_UP:
		case AB_LABEL_ARROW_RIGHT:
		case AB_LABEL_ARROW_LEFT:
		/* AB_LABEL_ARROW_* types are mapped into AB_LABEL_ARROW_DOWN */
                    prop_options_set_value(&(pbs->label_type), (XtPointer)AB_LABEL_ARROW_DOWN, False);
			break;
		default:
			prop_options_set_value(&(pbs->label_type), (XtPointer)obj_get_label_type(obj), False);
			break;
	}
	setup_labeltype_setting(type, btype);

	/* Load Label */
	prop_setup_label_field(&(pbs->label), NULL,
				obj->label_type, obj_get_label(obj), AB_LINE_UNDEF);

	/* Load Label Alignment */
	prop_options_set_value(&(pbs->label_align),
		(XtPointer)obj_get_label_alignment(obj), False);

	/* Load Menu Name */
	prop_menuname_set_value(&(pbs->menuname),
		obj_get_menu_name(obj), False);

	/* Load Size Policy */
	prop_radiobox_set_value(&(pbs->size_policy),
                                abobj_width_resizable(obj)?
                                (XtPointer)SIZE_FIXED_KEY :
                                (XtPointer)SIZE_OF_CONTENTS_KEY, False);

	/* Load Initial State */
	prop_checkbox_set_value(&(pbs->init_state), AB_STATE_VISIBLE,
				obj_is_initially_visible(obj), False);
	prop_checkbox_set_value(&(pbs->init_state), AB_STATE_ACTIVE,
				obj_is_initially_active(obj), False);

	/* Load Color */
	prop_colorfield_set_value(&(pbs->bg_color), obj_get_bg_color(obj), False);
	prop_colorfield_set_value(&(pbs->fg_color), obj_get_fg_color(obj), False);

	turnoff_changebars(type);
    }

    /* Load Geometry */
    if (load_all || loadkey & LoadPosition)
        prop_load_obj_position(obj, &(pbs->geometry));

    if (load_all || loadkey & LoadSize)
	prop_load_obj_size(obj, &(pbs->geometry));

    return OK;

}

int
button_prop_apply(
    AB_PROP_TYPE   type
)
{
    PropButtonSettingsRec 	*pbs = &(prop_button_settings_rec[type]);
    STRING	    		value;
    BOOL	    		size_chg = False;
    BOOL			reset_bg = False;
    BOOL			reset_fg = False;
    int				new_w, new_h;

    if (!verify_props(type))
        return ERROR;

    if (prop_changed(pbs->name.changebar))
    {
	value = prop_field_get_value(&(pbs->name));
        abobj_set_name(pbs->current_obj, value);
        util_free(value);
    }
    if (prop_changed(pbs->button_type.changebar))
    {
        abobj_set_button_type(pbs->current_obj,
            (AB_BUTTON_TYPE)prop_radiobox_get_value(&(pbs->button_type)));
        size_chg = True;
    }
    if (prop_changed(pbs->label.changebar) ||
        prop_changed(pbs->label_type.changebar) ||
        prop_changed(pbs->arrow_dir.changebar))
    {
        AB_LABEL_TYPE ltype =
            (AB_LABEL_TYPE)prop_options_get_value(&(pbs->label_type));

        switch (ltype)
        {
            case AB_LABEL_ARROW_DOWN:
            case AB_LABEL_ARROW_UP:
            case AB_LABEL_ARROW_RIGHT:
            case AB_LABEL_ARROW_LEFT:
                abobj_set_label(pbs->current_obj,
                    (AB_LABEL_TYPE)prop_options_get_value(&(pbs->arrow_dir)),
                    NULL);
                break;
            default:
        	value = prop_field_get_value(&(pbs->label));
                abobj_set_label(pbs->current_obj, ltype, value);
                util_free(value);
                break;
        }
        size_chg = True;
    }
    if (prop_changed(pbs->label_align.changebar))
    {
        abobj_set_label_alignment(pbs->current_obj,
                (AB_ALIGNMENT)prop_options_get_value(&(pbs->label_align)));
        size_chg = True;
    }
    if (prop_changed(pbs->size_policy.changebar))
    {
	BOOL fixed_size = prop_radiobox_get_value(
		&(pbs->size_policy)) == SIZE_FIXED_KEY;

        /* If the button type, label, or label type hasn't
	 * changed and the button's size was set to "Fixed"
	 * and now is being changed to "Size of Label", then
	 * we have to reinstantiate the button along with
	 * setting the XmNrecomputeSize resource back to True,
	 * so that the button will resize itself correctly.
	 * Otherwise, the resize won't take place.
         */
	if (!prop_changed(pbs->label.changebar) &&
	    !prop_changed(pbs->label_type.changebar) &&
	    !prop_changed(pbs->button_type.changebar) &&
	    (obj_get_width(pbs->current_obj) != -1) && !fixed_size)
	{
	    obj_clear_flag(pbs->current_obj, InstantiatedFlag);
	}

	abobj_set_size_policy(pbs->current_obj, fixed_size);
	size_chg = True;
    }
    if (prop_changed(pbs->geometry.changebar))
    {
	if (abobj_width_resizable(pbs->current_obj))
	{
	    new_w = prop_geomfield_get_value(&(pbs->geometry), GEOM_WIDTH);
	    abobj_set_pixel_width(pbs->current_obj, new_w, 0);
	}
	if (abobj_height_resizable(pbs->current_obj))
	{
	    new_h = prop_geomfield_get_value(&(pbs->geometry), GEOM_HEIGHT);
	    abobj_set_pixel_height(pbs->current_obj, new_h, 0);
	}
	if (abobj_is_movable(pbs->current_obj))
	    abobj_set_xy(pbs->current_obj,
	     	prop_geomfield_get_value(&(pbs->geometry), GEOM_X),
	     	prop_geomfield_get_value(&(pbs->geometry), GEOM_Y));
	size_chg = True;

    }
    if (prop_changed(pbs->init_state.changebar))
    {
        abobj_set_visible(pbs->current_obj,
		prop_checkbox_get_value(&(pbs->init_state), AB_STATE_VISIBLE));
        abobj_set_active(pbs->current_obj,
		prop_checkbox_get_value(&(pbs->init_state), AB_STATE_ACTIVE));
    }
    if (prop_changed(pbs->fg_color.changebar))
    {
        value = prop_colorfield_get_value(&(pbs->fg_color));
        abobj_set_foreground_color(pbs->current_obj, value);
	if (util_strempty(value))
	    reset_fg = True;
        util_free(value);
        prop_colorfield_set_value(&(pbs->fg_color),
		obj_get_fg_color(pbs->current_obj), False);
    }
    if (prop_changed(pbs->bg_color.changebar))
    {
        value = prop_colorfield_get_value(&(pbs->bg_color));
        abobj_set_background_color(pbs->current_obj, value);
	if (util_strempty(value))
	    reset_bg = True;
        util_free(value);
        prop_colorfield_set_value(&(pbs->bg_color),
		obj_get_bg_color(pbs->current_obj), False);
    }
    if (prop_changed(pbs->menuname.changebar))
    {
        value = prop_menuname_get_value(&(pbs->menuname));
        abobj_set_menu_name(pbs->current_obj, value);
        util_free(value);
        prop_menuname_set_value(&(pbs->menuname),
		obj_get_menu_name(pbs->current_obj), False);
    }

    abobj_instantiate_changes(pbs->current_obj);

    if (reset_bg || reset_fg) /* Set back to No Color */
        abobj_reset_colors(pbs->current_obj, reset_bg, reset_fg);
    if (size_chg)
	abobj_force_dang_form_resize(pbs->current_obj);

    /* Button may have changed Size as a result...*/
    prop_geomfield_set_value(&(pbs->geometry), GEOM_WIDTH,
		abobj_get_comp_width(pbs->current_obj), False);
    prop_geomfield_set_value(&(pbs->geometry), GEOM_HEIGHT,
		abobj_get_comp_height(pbs->current_obj), False);

    turnoff_changebars(type);

    return OK;
}

static BOOL
button_prop_pending(
    AB_PROP_TYPE type
)
{
    return(prop_changebars_pending(prop_button_settings_rec[type].prop_sheet));
}

static BOOL
verify_props(
    AB_PROP_TYPE type
)
{
    PropButtonSettingsRec *pbs = &(prop_button_settings_rec[type]);

    if (prop_changed(pbs->name.changebar) && !prop_name_ok(pbs->current_obj, pbs->name.field))
        return False;

    if ((prop_changed(pbs->label_type.changebar) || prop_changed(pbs->label.changebar)) &&
	(AB_BUILTIN_ACTION)prop_options_get_value(&(pbs->label_type)) == AB_LABEL_GLYPH &&
        !prop_graphic_filename_ok(pbs->label.field, False))
        return False;

    if (prop_changed(pbs->geometry.changebar) &&
        (!prop_number_ok(pbs->geometry.x_field, (STRING)XFieldStr, -SHRT_MAX, SHRT_MAX) ||
         !prop_number_ok(pbs->geometry.y_field, (STRING)YFieldStr, -SHRT_MAX, SHRT_MAX) ||
         !prop_number_ok(pbs->geometry.w_field, (STRING)WFieldStr, 1, SHRT_MAX) ||
         !prop_number_ok(pbs->geometry.h_field, (STRING)HFieldStr, 1, SHRT_MAX)))
        return False;

    if (prop_changed(pbs->fg_color.changebar) && !prop_color_ok(pbs->fg_color.field))
        return False;

    if (prop_changed(pbs->bg_color.changebar) && !prop_color_ok(pbs->bg_color.field))
        return False;

    if (prop_changed(pbs->menuname.changebar) && !prop_obj_name_ok(pbs->menuname.field,
        obj_get_module(pbs->current_obj), AB_TYPE_MENU, "Menu"))
	return False;

    return True;
}

static void
turnoff_changebars(
    AB_PROP_TYPE type
)
{
    PropButtonSettingsRec *pbs = &(prop_button_settings_rec[type]);

    prop_set_changebar(pbs->name.changebar,    		PROP_CB_OFF);
    prop_set_changebar(pbs->button_type.changebar,    	PROP_CB_OFF);
    prop_set_changebar(pbs->label_type.changebar,	PROP_CB_OFF);
    prop_set_changebar(pbs->label.changebar,     	PROP_CB_OFF);
    prop_set_changebar(pbs->arrow_dir.changebar,	PROP_CB_OFF);
    prop_set_changebar(pbs->label_align.changebar,	PROP_CB_OFF);
    prop_set_changebar(pbs->geometry.changebar,		PROP_CB_OFF);
    prop_set_changebar(pbs->size_policy.changebar,	PROP_CB_OFF);
    prop_set_changebar(pbs->init_state.changebar,	PROP_CB_OFF);
    prop_set_changebar(pbs->init_state.changebar,	PROP_CB_OFF);
    prop_set_changebar(pbs->bg_color.changebar,  	PROP_CB_OFF);
    prop_set_changebar(pbs->fg_color.changebar,  	PROP_CB_OFF);
    prop_set_changebar(pbs->menuname.changebar,  	PROP_CB_OFF);

    prop_changebars_cleared(pbs->prop_sheet);

}

static void
typeCB(
    Widget   item,
    XtPointer clientdata,
    XmToggleButtonCallbackStruct *state
)
{
    AB_PROP_TYPE	type = (AB_PROP_TYPE)clientdata;
    int    		value;

    if (state->set)
    {
	PropButtonSettingsRec *pbs = &(prop_button_settings_rec[type]);
	AB_BUTTON_TYPE btype;

        XtVaGetValues(item, XmNuserData, &value, NULL);
	btype = (AB_BUTTON_TYPE)value;

	if ((btype == AB_BUT_DRAWN) || (btype == AB_BUT_MENU))
	{
	    switch((AB_LABEL_TYPE)prop_options_get_value(&(pbs->label_type)))
	    {
	        case AB_LABEL_ARROW_DOWN:
	        case AB_LABEL_ARROW_UP:
	        case AB_LABEL_ARROW_LEFT:
	        case AB_LABEL_ARROW_RIGHT:
		    prop_options_set_value(&(pbs->label_type), (XtPointer)AB_LABEL_STRING, True);
		    break;
	        default:
		    break;
	    }
	}
	setup_labeltype_setting(type, btype);
    }
}

static void
setup_labeltype_setting(
    AB_PROP_TYPE type,
    AB_BUTTON_TYPE btype
)
{
    PropButtonSettingsRec *pbs = &(prop_button_settings_rec[type]);

    ui_set_active(pbs->string_item, True);
    ui_set_active(pbs->glyph_item, True);

    switch(btype)
    {
        case AB_BUT_PUSH:
            ui_set_active(pbs->arrow_item, True);
            ui_set_active(pbs->menuname.label, False);
            ui_set_active(pbs->menuname.menubutton, False);
            ui_set_active(pbs->menuname.field,   False);
            break;
        case AB_BUT_DRAWN:
            ui_set_active(pbs->arrow_item, False);
            ui_set_active(pbs->menuname.label, False);
            ui_set_active(pbs->menuname.menubutton, False);
            ui_set_active(pbs->menuname.field,   False);
            break;
        case AB_BUT_MENU:
            ui_set_active(pbs->arrow_item, False);
            ui_set_active(pbs->menuname.label, True);
            ui_set_active(pbs->menuname.menubutton, True);
            ui_set_active(pbs->menuname.field,   True);
            break;
    }
    /*
      HACK: when arrow_item is set inactive, ie grayed out for AB_BUT_DRAWN
      and AB_BUT_MENU, the label type option menu switchs to arrow_item.
      Setting the label type to its current value, fixes it.

      Why? setting the sensitivity on a option menu item, shouldn't change
      the option menu to that item.
    */
    prop_options_set_value(&(pbs->label_type), prop_options_get_value(&(pbs->label_type)), False);
    setup_label_field(type, (AB_LABEL_TYPE)prop_options_get_value(&(pbs->label_type)));
}

static void
setup_label_field(
    AB_PROP_TYPE type,
    AB_LABEL_TYPE ltype
)
{
    PropButtonSettingsRec *pbs = &(prop_button_settings_rec[type]);

    switch(ltype)
    {
        case AB_LABEL_STRING:
           ui_set_label_string(pbs->label.label, (STRING)LabelForString);
           ui_set_active(pbs->arrow_dir.label, False);
           ui_set_active(pbs->arrow_dir.optionbox, False);
           ui_set_active(pbs->label.label, True);
           ui_set_active(pbs->label.field, True);
	   ui_set_active(pbs->label_align.label, True);
	   ui_set_active(pbs->label_align.optionbox, True);
	   ui_set_active(pbs->graphic_hint, False);
           break;
        case AB_LABEL_GLYPH:
           ui_set_label_string(pbs->label.label, (STRING)LabelForGraphic);
           ui_set_active(pbs->arrow_dir.label, False);
           ui_set_active(pbs->arrow_dir.optionbox, False);
           ui_set_active(pbs->label.field, True);
	   ui_set_active(pbs->label.label, True);
           ui_set_active(pbs->label_align.label, True);
           ui_set_active(pbs->label_align.optionbox, True);
	   ui_set_active(pbs->graphic_hint, True);
           break;
        case AB_LABEL_ARROW_DOWN:
        case AB_LABEL_ARROW_UP:
        case AB_LABEL_ARROW_LEFT:
        case AB_LABEL_ARROW_RIGHT:
           ui_set_active(pbs->label.label, False);
           ui_set_active(pbs->label.field, False);
           ui_set_active(pbs->label_align.label, False);
           ui_set_active(pbs->label_align.optionbox, False);
           ui_set_active(pbs->arrow_dir.label, True);
           ui_set_active(pbs->arrow_dir.optionbox, True);
	   ui_set_active(pbs->graphic_hint, False);
           break;
    }
    /* WORKAROUND: for Motif bug */
    XtVaSetValues(pbs->label.label, XmNalignment, XmALIGNMENT_END, NULL);

}

static void
labeltypeCB(
    Widget   item,
    XtPointer clientdata,
    XtPointer	calldata
)
{
    AB_PROP_TYPE	  type = (AB_PROP_TYPE)clientdata;
    PropButtonSettingsRec *pbs = &(prop_button_settings_rec[type]);
    AB_LABEL_TYPE	  value = AB_LABEL_UNDEF;

    XtVaGetValues(item, XmNuserData, &value, NULL);
    setup_label_field(type, value);

    if (value != AB_LABEL_ARROW_DOWN)
        ui_field_select_string(pbs->label.field, True);

}

static void
size_policyCB(
    Widget	w,
    XtPointer	clientdata,
    XmToggleButtonCallbackStruct *state
)
{
    AB_PROP_TYPE        type = (AB_PROP_TYPE)clientdata;
    PropButtonSettingsRec *pbs = &(prop_button_settings_rec[type]);
    XtArgVal		value;

    /* Width/Height fields should ONLY be editable if Size Policy
     * is "Fixed"
     */
    if (state->set)
    {
    	XtVaGetValues(w, XmNuserData, &value, NULL);
        ui_field_set_editable(pbs->geometry.w_field, value == SIZE_FIXED_KEY);
        ui_field_set_editable(pbs->geometry.h_field, value == SIZE_FIXED_KEY);
    }
}
