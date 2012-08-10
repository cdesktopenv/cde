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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*
 *	$XConsortium: prop.c /main/5 1996/08/08 17:57:58 mustafa $
 *
 *	@(#)prop.c	1.89 26 Feb 1994
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
 *****************************************************************
 * prop.c - Implements all common property sheet functionality
 *
 *****************************************************************
 */
#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 1
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <Dt/ComboBox.h>
#include <Xm/BulletinB.h>
#include <Xm/CascadeB.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/List.h>
#include <Xm/MessageB.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/Separator.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/ToggleB.h>
#include <ab_private/trav.h>
#include <ab_private/obj_notify.h>
#include <ab_private/objxm.h>
#include <ab_private/ab.h>
#include <ab_private/prop.h>
#include <ab_private/propP.h>
#include <ab_private/proj.h>
#include <ab_private/abobj.h>
#include <ab_private/abobj_set.h>
#include <ab_private/abobj_list.h>
#include <ab_private/ui_util.h>
#include <ab_private/x_util.h>
#include <ab_private/conn.h>
#include <ab_private/help.h>
#include <ab_private/color_chooser.h>
#include <ab_private/attch_ed.h>
#include "dtbuilder.h"
#include "revolv_ui.h"

#if defined(CSRG_BASED)
#define MAXINT INT_MAX
#endif

char   		*LabelForString = NULL;
char   		*LabelForGraphic = NULL;
char		*NoneItem = NULL;

char		*XFieldStr = NULL;
char		*YFieldStr = NULL;
char		*WFieldStr = NULL;
char		*HFieldStr = NULL;
char		*OffsetFieldStr = NULL;
char		*PercentageFieldStr = NULL;
char		*RowColFieldStr = NULL;
char		*VertSpacingFieldStr = NULL;
char		*HorizSpacingFieldStr = NULL;

static char	*menu_strs[4];
static char	*menu_names[] = {"none_item", "new_menu_item", "menus_item", "edit_item"};

static const char    	*Name_ok_test="_";

typedef struct _PROP_STATE_INFO {
    PalItemInfo *palitem;
    Widget	objlist;
    ABObj	loaded_obj;
} PropStateInfo;



/*************************************************************************
**                                                                      **
**       Private Function Declarations                                  **
**                                                                      **
*************************************************************************/
		/* Revolving Dialog Functions
		 */
static void	revolv_dialog_init(
		    Widget	parent
		);
static void	revolv_objectspane_init(
		    DtbRevolvPropDialogInfo	cgen
		);
static void	revolv_basicframe_init(
		    DtbRevolvPropDialogInfo     cgen
                );
static PalItemInfo *
		revolv_set_prop_type(
            	    PalItemInfo*     	palitem,
		    ABObj		loadobj
        	);
static void	revolv_change_prop_frame(
		    PalItemInfo		*palitem
		);
static PalItemInfo *
		revolv_invoke_props(
		    PalItemInfo		*palitem,
		    ABObj		loadobj
		);


		/* Fixed Dialog Functions
		 */
static void
                fixed_invoke_props(
                    PalItemInfo         *palitem
                );

		/* General Prop Manipulation Functions
		 */
static int	apply_props(
		    AB_PROP_TYPE	ptype,
		    Widget		dialog
		);
static int	apply_prop_changes(
		    AB_PROP_TYPE	ptype,
		    ABObj		obj,
		    PalItemInfo		*palitem
		);
static DTB_MODAL_ANSWER
		handle_auto_apply(
                    AB_PROP_TYPE        ptype,
		    PropStateInfo	*pstate,
		    PalItemInfo		*new_palitem,
                    ABObj               new_obj,
		    PalItemInfo		**result
		);
static PropStateInfo *
		get_prop_state_info(
		    Widget		widget
		);
static void	init_prop_state_info(
		    Widget		dialog,
		    PalItemInfo		*palitem,
		    Widget		objlist,
		    ABObj		loaded_obj
		);
static void	load_props(
                    AB_PROP_TYPE        ptype,
		    PropStateInfo	*pstate,
		    ABObj		obj
		);
static void	create_obj_option_item(
		    PropObjOptionsSetting	pos,
		    Widget			item,
    		    ABObj       		obj
		);

		/* Object List Manipulation Functions
		 */
static int	objlist_delete_obj(
    		    AB_PROP_TYPE        type,
    		    PalItemInfo         *palitem,
		    ObjEvDestroyInfo	info
		);
static void	objlist_ensure_selection(
    		    Widget      	objlist
		);
static void	objlist_load(
                    AB_PROP_TYPE        type,
		    PropStateInfo	*pstate
		);
static int      objlist_rename_obj(
                    AB_PROP_TYPE        type,
                    PalItemInfo         *palitem,
    		    ObjEvAttChangeInfo  info
		);
static int	objlist_show_or_hide_obj(
                    AB_PROP_TYPE        type,
                    PalItemInfo         *palitem,
                    ObjEvUpdateInfo  	info
                );
static BOOL	objlist_test_func(
		    ABObj	test_obj
		);


/*
 * Object-Callbacks
 */
static int	obj_renamedOCB(
		    ObjEvAttChangeInfo  info
		);
static int	obj_shown_or_hiddenOCB(
		    ObjEvUpdateInfo	info
		);
static int	obj_geom_changedOCB(
		    ObjEvAttChangeInfo	info
		);
static int	obj_destroyedOCB(
    		    ObjEvDestroyInfo    info
		);

/*
 * Xt-Callbacks
 */
		/* General Prop Callbacks
		 */
static void    	apply_propsCB(
            	    Widget 	w,
            	    XtPointer 	clientdata,
            	    XtPointer 	calldata
        	);
static void    	cancel_propsCB(
            	    Widget 	w,
            	    XtPointer 	clientdata,
            	    XtPointer 	calldata
        	);
static void    	change_objecttypeCB(
            	    Widget 	w,
            	    XtPointer 	clientdata,
            	    XtPointer 	calldata
        	);
static void	close_propsCB(
                    Widget      w,
                    XtPointer   clientdata,
                    XtPointer   calldata
                );
static void	invoke_attach_editorCB(
                    Widget      w,
                    XtPointer   cldata,
                    XtPointer   calldata
                );
static void    	invoke_connectionsCB(
            	    Widget 	w,
            	    XtPointer 	cldata,
            	    XtPointer 	calldata
        	);
static void	invoke_help_editorCB(
                    Widget      w,
                    XtPointer   cldata,
                    XtPointer   calldata
                );
static void	objlist_selectCB(
    		    Widget      widget,
    		    XtPointer   client_data,
    		    XmListCallbackStruct *listdata
		);
static void     ok_propsCB(
                    Widget      w,
                    XtPointer   clientdata,
                    XtPointer   calldata
                );
static void    	reset_propsCB(
            	    Widget 	w,
            	    XtPointer 	clientdata,
            	    XtPointer 	calldata
        	);
static void    	tearoff_propsCB(
            	    Widget 	w,
            	    XtPointer 	clientdata,
            	    XtPointer 	calldata
        	);

		/* Color Field Callbacks
		 */
static void	color_chooserCB(
                    Widget   widget,
                    XtPointer clientdata,
                    XtPointer calldata
                );
static void	color_noneCB(
                    Widget   widget,
                    XtPointer clientdata,
                    XtPointer calldata
                );

		/* Menu Name Setting Callbacks
		 */
static void	menu_newCB(
		    Widget	widget,
		    XtPointer	client_data,
		    XtPointer	call_data
		);
static void	menu_editCB(
		    Widget	widget,
		    XtPointer   client_data,
                    XtPointer   call_data
                );
static void	menu_edit_set_stateCB(
		    Widget	widget,
		    XtPointer   client_data,
                    XtPointer   call_data
                );
static void     menulist_buildCB(
                    Widget      widget,
                    XtPointer   client_data,
                    XtPointer   call_data
                );
static void	menulist_destroyCB(
                    Widget      widget,
                    XtPointer   client_data,
                    XtPointer   call_data
                );
static void	menuname_clearCB(
                    Widget   widget,
                    XtPointer clientdata,
                    XtPointer calldata
                );
static void     menuname_setCB(
                    Widget      widget,
                    XtPointer   client_data,
                    XtPointer   call_data
                );
static void	menu_field_chgCB(
                    Widget      widget,
                    XtPointer   client_data,
                    XtPointer   call_data
                );

static void     obj_options_buildCB(
                    Widget      widget,
                    XtPointer   client_data,
                    XtPointer   call_data
                );

/*
 * Misc Functions
 */
static void	find_submenu_owners(
    		    ABObj       submenu,
    		    ABObj       *menulist,
    		    int         menu_count,
    		    ABObj       *i_menulist,
    		    int         *i_count_ptr
		);
static BOOL	is_submenu_of(
    		    STRING      submenu_name,
    		    ABObj       menu
		);
static BOOL	menu_in_list(
   		    ABObj        *menulist,
   		    int          menu_count,
   		    ABObj        target_menu
		);

static void strings_init(
		);


/*************************************************************************
**                                                                      **
**       Data Declarations                                              **
**                                                                      **
*************************************************************************/
extern PalItemInfo*    	palette_item[];
extern int       	palette_item_cnt;
extern PalItemInfo	*ab_menu_palitem;
extern PalItemInfo	*ab_group_palitem;

static PalItemInfo      *cur_list_palitem = NULL;
static Widget    	revolv_current_frame = NULL;
static Widget    	revolv_optionmenu   = NULL;
static BOOL		apply_in_progress = False;

static char      Buf[512];

/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
*************************************************************************/

/*
 * Initialize Props:
 * 	Create Revolving Prop dialog
 * 	Add callbacks for object rename & destroy
 */
void
prop_init(
    Widget	toplevel
)
{
    strings_init();

    revolv_dialog_init(toplevel);

    obj_add_rename_callback(obj_renamedOCB, "PROPS");
    obj_add_update_callback(obj_shown_or_hiddenOCB, "PROPS");
    obj_add_geometry_change_callback(obj_geom_changedOCB, "PROPS");
    obj_add_destroy_callback(obj_destroyedOCB, "PROPS");
}

/*
 * Popup the Revolving Prop dialog.
 * If an object is currently selected,  Set the
 * corresponding revolving prop-type and Load the object
 */
void
prop_show_dialog(
    Widget	widget,
    XtPointer	client_data,
    XtPointer	call_data
)
{
    ABSelectedRec sel;
    PalItemInfo	  *palitem = NULL;
    PropStateInfo *pstate;

    ab_set_busy_cursor(TRUE);

    abobj_get_selected(proj_get_project(), FALSE, FALSE, &sel);
    if (sel.count > 0)
    {
	palitem = pal_get_item_info(sel.list[0]);
	util_free(sel.list);
    }
    /* If no objects selected, set to First type */
    if (palitem == NULL)
	palitem = palette_item[0];

    revolv_invoke_props(palitem, NULL);
    pstate = get_prop_state_info(AB_rev_prop_dialog);
    objlist_ensure_selection(pstate->objlist);

    ab_set_busy_cursor(FALSE);
}

/*
 * Popup the Fixed Prop dialog for the specified
 * Palette Item type.
 */
void
prop_show_fixed(
    PalItemInfo *palitem
)
{
    PropStateInfo *pstate;

    ab_set_busy_cursor(TRUE);

    fixed_invoke_props(palitem);
    pstate = get_prop_state_info(palitem->fix_prop_dialog);
    objlist_ensure_selection(pstate->objlist);

    ab_set_busy_cursor(FALSE);
}

/*
 * Popup the Fixed Prop dialog for Menus
 */
void
prop_show_menu_props(
    Widget	w,
    XtPointer	client_data,
    XtPointer   calldata
)
{
    prop_show_fixed(ab_menu_palitem);
}

/*
 * Popup the Fixed Group dialog for menus.
 */
void
prop_show_group_props(
    Widget	w,
    XtPointer	client_data,
    XtPointer   calldata
)
{

    prop_show_fixed(ab_group_palitem);

}

/*
 * Load an object into the Prop dialog of the
 * specified prop type (Revolving or Fixed).
 */
void
prop_load_obj(
    ABObj	obj,
    AB_PROP_TYPE ptype
)
{
    PalItemInfo *palitem;
    PalItemInfo *viz_palitem;
    Widget      dialog;
    PropStateInfo *pstate;
    STRING	modname = NULL;

    if (obj_is_module(obj) || obj_is_project(obj))
	return;

    ab_set_busy_cursor(TRUE);

    if (obj_is_item(obj))
	obj = obj_get_parent(obj);

    obj = obj_get_root(obj);

    if ((viz_palitem = palitem = pal_get_item_info(obj)) != NULL)
    {
    	if (ptype == AB_PROP_REVOLVING)
    	{
	    viz_palitem = revolv_invoke_props(palitem, obj);
	    dialog = AB_rev_prop_dialog;
    	}
    	else /* AB_PROP_FIXED */
    	{
	    fixed_invoke_props(palitem);
	    dialog = palitem->fix_prop_dialog;
    	}

	pstate = get_prop_state_info(dialog);
	modname = abobj_get_moduled_name(obj);
    }
    else
	util_dprintf(1,"prop_load_obj: %s : could not get Palette info\n",
		util_strsafe(obj_get_name(obj)));

    ab_set_busy_cursor(FALSE);

    if (dialog != NULL && modname != NULL && viz_palitem == palitem)
    {
	ui_list_select_item(pstate->objlist, modname, TRUE);
	util_free(modname);
    }

}
void
prop_fixed_dialog_init(
    PalItemInfo *palitem,
    Widget	dialog,
    Widget	objlist
)
{
    Widget	shell = XtParent(dialog);

    init_prop_state_info(dialog, palitem, objlist, NULL);

    /* The initial position of a Fixed Prop dialog is dependent on
     * how it is invoked and whether the Revolving Prop editor is up,
     * so don't specify an initial position relative to the palette.
     */
    ab_register_window(dialog, AB_WIN_DIALOG, WindowHidden,
		NULL, AB_WPOS_UNSPECIFIED,
		close_propsCB, (XtPointer)AB_PROP_FIXED);

    /* Store current-object in List (init to NULL)  */
    XtVaSetValues(objlist,
                XmNselectionPolicy,     XmBROWSE_SELECT,
                XmNuserData,            NULL,
                NULL);

    XtAddCallback(objlist, XmNbrowseSelectionCallback,
            (XtCallbackProc)objlist_selectCB, (XtPointer)AB_PROP_FIXED);

}

void
prop_activate_panel_init(
    AB_PROP_TYPE type,
    PalItemInfo *palitem,
    Widget	ok_button,
    Widget	apply_button,
    Widget	reset_button,
    Widget	cancel_button,
    Widget	help_button
)
{
    Widget	dialog;

    /* Ensure "Apply" is default button */
    dialog = ui_get_ancestor_dialog(apply_button);
    XtVaSetValues(dialog, XmNdefaultButton, apply_button, NULL);

    XtVaSetValues(apply_button,
		XmNshowAsDefault, True,
		XmNdefaultButtonShadowThickness, 1,
		NULL);
    XtVaSetValues(ok_button,
                XmNdefaultButtonShadowThickness, 1,
                NULL);
    XtVaSetValues(reset_button,
                XmNdefaultButtonShadowThickness, 1,
                NULL);
    XtVaSetValues(cancel_button,
                XmNdefaultButtonShadowThickness, 1,
                NULL);
    if (help_button != NULL)
    	XtVaSetValues(help_button,
                XmNdefaultButtonShadowThickness, 1,
                NULL);

    XtAddCallback(apply_button, XmNactivateCallback, apply_propsCB, (XtPointer)type);
    XtAddCallback(reset_button, XmNactivateCallback, reset_propsCB, (XtPointer)type);
    XtAddCallback(ok_button,    XmNactivateCallback, ok_propsCB,    (XtPointer)type);
    XtAddCallback(cancel_button,XmNactivateCallback, cancel_propsCB,(XtPointer)type);

}

void
prop_editors_panel_init(
    AB_PROP_TYPE type,
    PalItemInfo *palitem,
    Widget	attach_ed_button,
    Widget	conn_ed_button,
    Widget	help_ed_button
)
{
    if (attach_ed_button)
    {
    	XtAddCallback(attach_ed_button, XmNactivateCallback,
			invoke_attach_editorCB, (XtPointer)type);

    }
    if (conn_ed_button)
    {
    	XtAddCallback(conn_ed_button, XmNactivateCallback,
			invoke_connectionsCB, (XtPointer)type);

    }
    if (help_ed_button)
    {
    	XtAddCallback(help_ed_button, XmNactivateCallback,
			invoke_help_editorCB, (XtPointer)type);

    }

}


void
prop_checkbox_init(
    PropCheckboxSetting	pcs,
    Widget		label,
    Widget		checkbox,
    int			num_items,
    WidgetList		items,
    int			*item_keys,
    Widget		changebar
)
{
    int i;

    /* Store Widget-IDs */
    pcs->label = label;
    pcs->checkbox = checkbox;
    pcs->changebar = changebar;

    propP_changebar_init(changebar, checkbox);

    for (i=0; i < num_items; i++)
    {
	XtVaSetValues(items[i], XmNuserData, (XtArgVal)item_keys[i], NULL);
        XtAddCallback(items[i], XmNvalueChangedCallback,
                        propP_setting_chgCB, (XtPointer)changebar);
    }
}

int
prop_checkbox_set_value(
    PropCheckboxSetting	pcs,
    int			itemkey,
    BOOL		value,
    BOOL		trip_changebar
)
{
    int		num_children = 0;
    WidgetList  children = NULL;
    XtArgVal	childkey;
    int		i;

    XtVaGetValues(pcs->checkbox,
	XtNnumChildren,	&num_children,
	XtNchildren,	&children,
	NULL);

    for (i=0; i < num_children; i++)
    {
	XtVaGetValues(children[i], XmNuserData, &childkey, NULL);
	if (childkey == itemkey) /* Found it */
	{
	    XmToggleButtonSetState(children[i], value, trip_changebar? TRUE : FALSE);
	    return OK;
	}
    }
    /* Item not found in checkbox */
    util_dprintf(1,"prop_checkbox_set_value: invalid item key\n");
    return ERROR;
}

BOOL
prop_checkbox_get_value(
    PropCheckboxSetting	pcs,
    int			itemkey
)
{
    int         num_children = 0;
    WidgetList  children = NULL;
    XtArgVal    childkey;
    int         i;

    XtVaGetValues(pcs->checkbox,
        XtNnumChildren, &num_children,
        XtNchildren,    &children,
        NULL);

    for (i=0; i < num_children; i++)
    {
        XtVaGetValues(children[i], XmNuserData, &childkey, NULL);
        if (childkey == itemkey) /* Found it */
            return((BOOL)XmToggleButtonGetState(children[i]));
    }

    /* itemkey was not found */
    util_dprintf(1, "prop_checkbox_get_value: invalid item key\n");
    return FALSE;
}

void
prop_colorfield_init(
    PropColorSetting	pcs,
    Widget		menubutton,
    Widget		none_item,
    Widget		chooser_item,
    Widget		swatch,
    Widget		field,
    Widget		changebar
)
{
    Pixel	contrast_pix, default_pix;
    char	swatch_name[128];

    /* Store Widget-IDs */
    pcs->menubutton = menubutton;
    pcs->swatch = swatch;
    pcs->field = field;
    pcs->changebar = changebar;

    XtAddCallback(chooser_item, XmNactivateCallback,
        color_chooserCB, (XtPointer)pcs);

    XtAddCallback(none_item, XmNactivateCallback,
	color_noneCB, (XtPointer)pcs);

    /* Determine whether this is a foreground or background color field
     * and store the corresponding color-default in user-data.
     */
    strcpy(swatch_name, XtName(swatch));
    if (strstr(swatch_name, "bg"))
	default_pix = WhitePixelOfScreen(XtScreen(swatch));
    else
	default_pix = BlackPixelOfScreen(XtScreen(swatch));

    /* Ensure Swatch's border is contrast to prop sheet */
    XtVaGetValues(XtParent(changebar),
                        XmNforeground,  &contrast_pix,
                        NULL);
    XtVaSetValues(swatch,
		XmNborderColor,	  (XtArgVal)contrast_pix,
		XmNborderWidth,   (XtArgVal)1,
		XmNuserData,	  (XtArgVal)default_pix,
		XmNrecomputeSize, (XtArgVal)False,
		NULL);

    strcpy(swatch_name, XtName(swatch));

    /* Setup Prop Sheet changebar mechanism */
    propP_changebar_init(changebar, menubutton);

    XtAddCallback(field, XmNvalueChangedCallback,
                propP_field_chgCB, (XtPointer)changebar);

}

STRING
prop_colorfield_get_value(
    PropColorSetting    pcs
)
{
    STRING	value;

    value = ui_field_get_string(pcs->field);

    return value;
}

int
prop_colorfield_set_value(
    PropColorSetting	pcs,
    STRING		colorname,
    BOOL		trip_changebar
)
{
    Pixel 		pixel;

    if (util_strempty(colorname)) /* no color - swatch is 'invisible' */
        XtVaGetValues(XtParent(pcs->swatch), XmNbackground, &pixel, NULL);

    else if (objxm_name_to_pixel(AB_toplevel, colorname, &pixel) != OK)
    {
	if (objxm_color_exists(colorname)) /* color couldn't be allocated */
	{
	    /* Set swatch color to default black or white */
	    XtVaGetValues(pcs->swatch, XmNuserData, &pixel, NULL);
	}
	else /* Invalid color name - this should not happen */
	{
	    util_dprintf(1, "prop_colorfield_set_value: \"%s\" is not an existing color name.\n",
		colorname);
	    return ERR;
	}
    }
    XtVaSetValues(pcs->swatch,
	XmNbackground, 	(XtArgVal)pixel,
	NULL);

    if (!trip_changebar)
        /* Set state so changebar is not triggered */
        XtVaSetValues(pcs->field, XmNuserData, (XtArgVal)PROP_LOAD, NULL);

    /* Set field value */
    ui_field_set_string(pcs->field, colorname);

    if (!trip_changebar)
        /* Reset state */
        XtVaSetValues(pcs->field, XmNuserData, (XtArgVal)PROP_EDIT, NULL);

    return OK;

}

void
prop_obj_combobox_init(
    PropObjComboboxSetting pcs,
    Widget              label,
    Widget              combobox,
    Widget              changebar,
    ABObj		*current_obj_ptr
)
{

    /* Store Widget-IDs */
    pcs->label = label;
    pcs->combobox = combobox;
    pcs->changebar = changebar;
    pcs->list = ui_combobox_get_list_widget(combobox);
    pcs->current_obj_ptr = current_obj_ptr;

    /* Setup Prop Sheet changebar mechanism */
    propP_changebar_init(changebar, combobox);

    /* Load "None" Item */
    prop_obj_combobox_load(pcs, NULL, NULL);

    XtAddCallback(combobox, DtNselectionCallback, propP_combobox_chgCB,
		(XtPointer)pcs);

}

ABObj
prop_obj_combobox_get_value(
    PropObjComboboxSetting pcs
)
{
    ABObj	 	obj;
    STRING	 	name;
    XmStringTable 	selected;
    int			select_count = 0;

    XtVaGetValues(pcs->list,
	XmNselectedItems, &selected,
	XmNselectedItemCount, &select_count,
	NULL);

    if (select_count == 0)
    {
	util_dprintf(1, "prop_obj_combobox_get_value: No items select in List\n");
	return NULL;
    }
    name = objxm_xmstr_to_str(selected[0]);

    if (util_strcmp(name, NoneItem) != 0)
        obj = obj_find_by_name(obj_get_module(*pcs->current_obj_ptr), name);
    else
	obj = NULL; /* "None" was selected */

    util_free(name);

    return obj;
}

int
prop_obj_combobox_set_value(
    PropObjComboboxSetting pcs,
    ABObj		   obj,
    BOOL		   trip_changebar
)
{
    ui_combobox_select_item(pcs->combobox,
		(STRING)(obj? obj_get_name(obj) : NoneItem));
    prop_set_changebar(pcs->changebar, trip_changebar? PROP_CB_ON : PROP_CB_OFF);

    return 0;
}

void
prop_obj_combobox_load(
    PropObjComboboxSetting pcs,
    ABObj		   root,
    ABObjTestFunc	   obj_test_func
)
{
    AB_TRAVERSAL	trav;
    ABObj		obj;

    ui_combobox_clear(pcs->combobox);
    ui_combobox_add_item(pcs->combobox, (STRING)NoneItem, 0, False);

    if (root != NULL)
    {
    	for (trav_open(&trav, root, AB_TRAV_SALIENT_UI);
            (obj= trav_next(&trav)) != NULL; )
	    if ((*obj_test_func)(obj) == True)
	    	ui_combobox_add_item(pcs->combobox, obj_get_name(obj), 0, False);

        trav_close(&trav);
    }
}

void
prop_field_init(
    PropFieldSetting	pfs,
    Widget		label,
    Widget		field,
    Widget		changebar
)
{
    /* Store Widget-IDs */
    pfs->label = label;
    pfs->field = field;
    pfs->changebar = changebar;

    /* Setup Prop Sheet changebar mechanism */
    XtVaSetValues(field, XmNuserData, (XtArgVal)PROP_EDIT, NULL);
    propP_changebar_init(changebar, field);

    XtAddCallback(field, XmNvalueChangedCallback,
		propP_field_chgCB, (XtPointer)changebar);

}

STRING
prop_field_get_value(
    PropFieldSetting    pfs
)
{
    STRING      value;

    value = ui_field_get_string(pfs->field);

    return value;
}

int
prop_field_set_value(
    PropFieldSetting	pfs,
    STRING		value,
    BOOL		trip_changebar
)
{
    return(propP_field_set_value(pfs->field, value, trip_changebar));
}

int
prop_field_get_numeric_value(
    PropFieldSetting	pfs
)
{
    STRING      value;
    int		num;

    value = ui_field_get_string(pfs->field);
    num = prop_str_to_int(value);
    util_free(value);

    return num;
}

int
prop_field_set_numeric_value(
    PropFieldSetting	pfs,
    int			value,
    BOOL		trip_changebar
)
{
    char valstr[8];

    if (!trip_changebar)
    	/* Set state so changebar is not triggered */
    	XtVaSetValues(pfs->field, XmNuserData, (XtArgVal)PROP_LOAD, NULL);

    /* Convert int to string */
    sprintf(valstr, "%d", value);
    ui_field_set_string(pfs->field, valstr);

    if (!trip_changebar)
    	/* Reset state */
    	XtVaSetValues(pfs->field, XmNuserData, (XtArgVal)PROP_EDIT, NULL);

    return OK;
}

void
prop_footer_init(
    PropFooterSetting    pfs,
    Widget              label,
    Widget		checkbox,
    int			num_items,
    WidgetList		items,
    int			*item_keys,
    Widget              field,
    Widget              changebar
)
{
    int i;

    /* Store Widget-IDs */
    pfs->label = label;
    pfs->checkbox = checkbox;
    pfs->field = field;
    pfs->changebar = changebar;

    /* Setup Prop Sheet changebar mechanism */
    XtVaSetValues(field, XmNuserData, (XtArgVal)PROP_EDIT, NULL);
    propP_changebar_init(changebar, checkbox);

    XtAddCallback(field, XmNvalueChangedCallback,
                propP_field_chgCB, (XtPointer)changebar);

    for (i=0; i < num_items; i++)
    {
        XtVaSetValues(items[i], XmNuserData, (XtArgVal)item_keys[i], NULL);
        XtAddCallback(items[i], XmNvalueChangedCallback,
                        propP_setting_chgCB, (XtPointer)changebar);
    }

}

void
prop_geomfield_init(
    PropGeometrySetting	pgs,
    Widget		label,
    Widget		x_label,
    Widget		x_field,
    Widget		y_label,
    Widget		y_field,
    Widget		w_label,
    Widget		w_field,
    Widget		h_label,
    Widget		h_field,
    Widget		changebar
)
{
    /* Store Widget-IDs */
    pgs->label = label;
    pgs->x_label = x_label;
    pgs->x_field = x_field;
    pgs->y_label = y_label;
    pgs->y_field = y_field;
    pgs->w_label = w_label;
    pgs->w_field = w_field;
    pgs->h_label = h_label;
    pgs->h_field = h_field;
    pgs->changebar = changebar;

    /* Setup Prop Sheet changebar mechanism */
    propP_changebar_init(changebar,
	x_field? x_field : (y_field? y_field : (w_field? w_field : h_field)));

    if (x_field != NULL)
    {
    	XtVaSetValues(x_field, XmNuserData, (XtArgVal)PROP_EDIT, NULL);
    	XtAddCallback(x_field, XmNvalueChangedCallback,
			propP_field_chgCB, (XtPointer)changebar);
    }
    if (y_field != NULL)
    {
    	XtVaSetValues(y_field, XmNuserData, (XtArgVal)PROP_EDIT, NULL);
    	XtAddCallback(y_field, XmNvalueChangedCallback,
			propP_field_chgCB, (XtPointer)changebar);
    }
    if (h_field != NULL)
    {
    	XtVaSetValues(h_field, XmNuserData, (XtArgVal)PROP_EDIT, NULL);
    	XtAddCallback(h_field, XmNvalueChangedCallback,
			propP_field_chgCB, (XtPointer)changebar);
    }
    if (w_field != NULL)
    {
    	XtVaSetValues(w_field, XmNuserData, (XtArgVal)PROP_EDIT, NULL);
    	XtAddCallback(w_field, XmNvalueChangedCallback,
			propP_field_chgCB, (XtPointer)changebar);
    }

}

int
prop_geomfield_get_value(
    PropGeometrySetting pgs,
    GEOM_KEY		gkey
)
{
    Widget	field;
    STRING 	value;
    int		gvalue = 0;

    switch(gkey)
    {
	case GEOM_X:
	    field = pgs->x_field;
	    break;
	case GEOM_Y:
	    field = pgs->y_field;
	    break;
	case GEOM_WIDTH:
	    field = pgs->w_field;
	    break;
	case GEOM_HEIGHT:
	    field = pgs->h_field;
	    break;
	default:
	    field = NULL;
    }
    if (field)
    {
	value = ui_field_get_string(field);
	gvalue = prop_str_to_int(value);
	util_free(value);
    }
    return gvalue;
}

int
prop_geomfield_set_value(
    PropGeometrySetting pgs,
    GEOM_KEY		gkey,
    int			gvalue,
    BOOL		trip_changebar
)
{
    Widget      field;

    switch(gkey)
    {
        case GEOM_X:
            field = pgs->x_field;
            break;
        case GEOM_Y:
            field = pgs->y_field;
            break;
        case GEOM_WIDTH:
            field = pgs->w_field;
            break;
        case GEOM_HEIGHT:
            field = pgs->h_field;
            break;
        default:
            field = NULL;
    }
    if (field)
	propP_field_set_numeric_value(field, gvalue, trip_changebar);

    return OK;
}

void
prop_load_obj_position(
    ABObj		obj,
    PropGeometrySetting pgs
)
{
    BOOL	movable = abobj_is_movable(obj);

    prop_geomfield_set_value(pgs, GEOM_X, abobj_get_comp_x(obj), False);
    prop_geomfield_set_value(pgs, GEOM_Y, abobj_get_comp_y(obj), False);

    ui_set_active(pgs->x_field, movable);
    ui_set_active(pgs->x_label, movable);
    ui_set_active(pgs->y_field, movable);
    ui_set_active(pgs->y_label, movable);
}

void
prop_load_obj_size(
    ABObj		obj,
    PropGeometrySetting pgs
)
{
    BOOL resizable;

    if (pgs->w_field)
    {
    	prop_geomfield_set_value(pgs, GEOM_WIDTH, abobj_get_comp_width(obj), False);
    	resizable =  abobj_width_resizable(obj);
    	ui_set_active(pgs->w_field, resizable);
    	ui_set_active(pgs->w_label, resizable);
    }
    if (pgs->h_field)
    {
    	prop_geomfield_set_value(pgs, GEOM_HEIGHT, abobj_get_comp_height(obj), False);
    	resizable =  abobj_height_resizable(obj);
    	ui_set_active(pgs->h_field, resizable);
    	ui_set_active(pgs->h_label, resizable);
    }
}

void
prop_size_policyCB(
    Widget	w,
    XtPointer	clientdata,
    XtPointer	calldata
)
{
    XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct*)calldata;
    PropGeometrySetting 	pgs = (PropGeometrySetting)clientdata;
    XtArgVal			value;
    BOOL			active = False;

    if (state->set)
    {
    	XtVaGetValues(w, XmNuserData, &value, NULL);
	active = (value == SIZE_FIXED_KEY);
	if (pgs->w_label)
            ui_set_active(pgs->w_label, active);
	if (pgs->w_field)
            ui_set_active(pgs->w_field, active);
	if (pgs->h_label)
            ui_set_active(pgs->h_label, active);
	if (pgs->h_field)
            ui_set_active(pgs->h_field, active);
    }
}

#define NO_MENU		0
#define	NEW_MENU	1
#define MENUS		2
#define EDIT_MENU	3

void
prop_menuname_init(
    PropMenunameSetting	pms,
    AB_PROP_TYPE	ptype,
    Widget		label,
    Widget		menubutton,
    Widget		field,
    Widget		owner_name_field,
    Widget		changebar,
    PropFieldSetting	menu_title_pfs,
    ABObj		*current_obj_ptr,
    BOOL		new_item_ok
)
{
    Widget      submenu;
    Widget	menu_items[4];
    int		i;

    pms->prop_type = ptype;
    pms->label = label;
    pms->menubutton = menubutton;
    pms->field = field;
    pms->owner_name_field = owner_name_field;
    pms->changebar = changebar;
    pms->menu_title_pfs = menu_title_pfs;
    pms->current_obj_ptr = current_obj_ptr;

    /* Setup Prop Sheet changebar mechanism */
    XtVaSetValues(field, XmNuserData, (XtArgVal)PROP_EDIT, NULL);
    propP_changebar_init(changebar, field);

    XtAddCallback(field, XmNvalueChangedCallback,
                propP_field_chgCB, (XtPointer)changebar);

    /* Build Menu and Setup Callbacks */
    XtVaGetValues(menubutton, XmNsubMenuId, &submenu, NULL);

    for (i=0; i < XtNumber(menu_items); i++)
    {
    	menu_items[i] = XtVaCreateManagedWidget(menu_names[i],
                        xmCascadeButtonWidgetClass,
                        submenu,
                        XtVaTypedArg, XmNlabelString, XtRString,
                                menu_strs[i], strlen(menu_strs[i])+1,
                        XmNuserData,    pms,
                        NULL);
    }

    XtAddCallback(menu_items[NO_MENU], XmNactivateCallback, menuname_clearCB,
			(XtPointer)pms);

    if (!new_item_ok)
	XtDestroyWidget(menu_items[NEW_MENU]);
    else
        XtAddCallback(menu_items[NEW_MENU], XmNactivateCallback, menu_newCB,
                        (XtPointer)pms);

    /* Set up callback to dynamically build "Menus" submenu */
    XtAddCallback(menubutton, XmNcascadingCallback, menulist_buildCB,
                        (XtPointer)menu_items[MENUS]);
    XtVaGetValues(menubutton, XmNsubMenuId, &submenu, NULL);
    XtAddCallback(XtParent(submenu), XtNpopdownCallback, menulist_destroyCB,
                        (XtPointer)submenu);

    /* Set up "Edit Current" menu item */
    XtAddCallback(menubutton, XmNcascadingCallback, menu_edit_set_stateCB,
                        (XtPointer)menu_items[EDIT_MENU]);
    XtAddCallback(menu_items[EDIT_MENU], XmNactivateCallback, menu_editCB,
                        (XtPointer)pms);

    /* Menu Title field should not be active until a Menu is connected */
    if (menu_title_pfs)
    {
    	XtAddCallback(field, XmNvalueChangedCallback,
                menu_field_chgCB, (XtPointer)pms);
	ui_set_active(menu_title_pfs->label, False);
	ui_set_active(menu_title_pfs->field, False);
    }

}

STRING
prop_menuname_get_value(
    PropMenunameSetting    pms
)
{
    STRING      value;

    value = ui_field_get_string(pms->field);

    return value;
}

int
prop_menuname_set_value(
    PropMenunameSetting pms,
    STRING              value,
    BOOL                trip_changebar
)
{
    return(propP_field_set_value(pms->field, value, trip_changebar));
}

void
prop_options_init(
    PropOptionsSetting	pos,
    Widget		label,
    Widget		optionbox,
    Widget		menu,
    int			num_items,
    WidgetList		items,
    XtPointer		*item_values,
    Widget		changebar
)
{
    int i;

    /* Store Widget-IDs */
    pos->label = label;
    pos->optionbox = optionbox;
    pos->menu = menu;
    pos->changebar = changebar;

    /* Setup Prop Sheet changebar mechanism */
    propP_changebar_init(changebar, optionbox);

    for (i=0; i < num_items; i++)
    {
	XtVaSetValues(items[i], XmNuserData, (XtArgVal)item_values[i], NULL);
        XtAddCallback(items[i], XmNactivateCallback,
                        propP_setting_chgCB, (XtPointer)changebar);
        XtAddCallback(items[i], XmNactivateCallback,
                        propP_options_itemCB, (XtPointer)menu);

    }
    /* Initialize Current-value to First Item in Option Menu */
    if (num_items > 0)
    	prop_options_set_value(pos, (XtPointer)item_values[0], False);
}

XtPointer
prop_options_get_value(PropOptionsSetting pos)
{
    XtArgVal value = -1;

    XtVaGetValues(pos->menu, XmNuserData, &value, NULL);

    return ((XtPointer)value);
}

int
prop_options_set_value(
    PropOptionsSetting	pos,
    XtPointer         	value,
    BOOL		trip_changebar
)
{
    int            num_children;
    WidgetList     children;
    XtArgVal       child_val;
    int            i;

    XtVaGetValues(pos->menu,
        XtNnumChildren,    &num_children,
        XtNchildren,       &children,
        NULL);

    for (i = 0; i < num_children; i++)
    {
        XtVaGetValues(children[i],
                XmNuserData, 	 &child_val,
                NULL);

        if (child_val == (XtArgVal) value)
        {
	    XtVaSetValues(pos->optionbox, XmNmenuHistory, children[i], NULL);
            XtVaSetValues(pos->menu, XmNuserData, (XtArgVal)child_val, NULL);

	    if (trip_changebar)
		prop_set_changebar(pos->changebar, PROP_CB_ON);

            return OK;
        }
    }
    /* value doesn't exist as an option */
    util_dprintf(1, "prop_optionmenu_set_value: invalid option\n");
    return ERROR;
}

/*
 * prop_options_remove_value()
 * Unmanages item corresponding to 'value'
 */
int
prop_options_remove_value(
    PropOptionsSetting	pos,
    XtPointer         	value,
    BOOL		trip_changebar
)
{
    int            num_children;
    WidgetList     children;
    XtArgVal       child_val,
		   reset_val;
    int            i;

    XtVaGetValues(pos->menu,
        XtNnumChildren,    &num_children,
        XtNchildren,       &children,
        NULL);

    for (i = 0; i < num_children; i++)
    {
        XtVaGetValues(children[i],
                XmNuserData, 	 &child_val,
                NULL);

        if (child_val == (XtArgVal)value)
        {
	    XtArgVal	cur_val;

	    cur_val = (XtArgVal)prop_options_get_value(pos);

	    /*
	     * Found value to remove. We need to do:
	     *	- if current setting value is the same as the one we are
	     *	  deleting, set the setting value to be the previous item
	     *	  (if that is not the deleted one)
	     *	- unmanage item widget
	     */

	    if (cur_val == (XtArgVal)value)
	    {
		if ((i == 0) && (num_children != 1))
		{
                    XtVaGetValues(children[1],
                        XmNuserData, 	 &reset_val,
                        NULL);
		    prop_options_set_value(pos, (XtPointer)reset_val, trip_changebar);
		}
		else
		    if (i != 0)
		        prop_options_set_value(pos, (XtPointer)reset_val, trip_changebar);
	    }

	    XtUnmanageChild(children[i]);

	    if (trip_changebar)
		prop_set_changebar(pos->changebar, PROP_CB_ON);

            return OK;
        }
	else
	    /*
	     * Remember previous child value
	     */
	    reset_val = child_val;
    }

    /* value doesn't exist as an option */
    util_dprintf(1, "prop_optionmenu_set_value: invalid option\n");
    return ERROR;
}

void
prop_obj_options_init(
    PropObjOptionsSetting  pos,
    Widget              label,
    Widget              optionbox,
    Widget              menu,
    int                 num_items,
    WidgetList          items,
    XtPointer          *item_values,
    Widget              changebar,
    BOOL		display_module,
    ABObj		*current_obj_ptr,
    ABObjTestFunc	obj_test_func
)
{
    Widget	shell;

    prop_options_init(&(pos->options), label, optionbox, menu,
		num_items, items, item_values, changebar);
    pos->display_module = display_module;
    pos->current_obj_ptr = current_obj_ptr;
    pos->obj_test_func = obj_test_func;
    pos->search_root = NULL;

    /* Setup callback to dynamically build Object list when Menu is popped up */
    shell = ui_get_ancestor_shell(menu);
    XtAddCallback(shell, XtNpopupCallback, obj_options_buildCB, (XtPointer)pos);

}

ABObj
prop_obj_options_get_value(
    PropObjOptionsSetting  pos
)
{
    return((ABObj)prop_options_get_value(&(pos->options)));
}

int
prop_obj_options_set_value(
    PropObjOptionsSetting	pos,
    ABObj			obj,
    BOOL			trip_changebar
)
{
    return(prop_options_set_value(&(pos->options), (XtPointer)obj, trip_changebar));
}

void
prop_obj_options_load(
    PropObjOptionsSetting  pos,
    ABObj                  search_root
)
{
    AB_TRAVERSAL        trav;
    ABObj               obj;
    int			i, num_items;
    WidgetList		items;

    /* Clear Object OptionMenu */
    XtVaGetValues(pos->options.menu,
		XmNchildren, 	&items,
		XmNnumChildren, &num_items,
		NULL);

    /* Unmanage all but the First ("None") items */
    if (num_items > 1)
    	XtUnmanageChildren(&(items[1]), num_items - 1);

    i = 1;

    /* Load OptionMenu with objects passing "test" */
    pos->search_root = search_root;
    if (pos->search_root != NULL)
    {
        for (trav_open(&trav, pos->search_root, AB_TRAV_SALIENT_UI);
            (obj= trav_next(&trav)) != NULL; )
            if ((*(pos->obj_test_func))(obj) == True)
	    {
		/* Either create new item or use existing one */
		create_obj_option_item(pos, i < num_items? items[i] : NULL, obj);
		i++;
	    }
        trav_close(&trav);
    }
    XtVaGetValues(pos->options.menu,
                XmNchildren, 	&items,
                NULL);

    /* Manage all eligible items */
    if (i > 1)
    	XtManageChildren(&(items[1]), i - 1);
}

void
prop_radiobox_init(
    PropRadioSetting	prs,
    Widget		label,
    Widget		radiobox,
    int			num_items,
    WidgetList		items,
    XtPointer		*item_values,
    Widget		changebar
)
{
    int i;

    /* Store Widget-IDs */
    prs->label = label;
    prs->radiobox = radiobox;
    prs->changebar = changebar;

    /* Setup Prop Sheet changebar mechanism */
    propP_changebar_init(changebar, radiobox);

    for (i=0; i < num_items; i++)
    {
	XtVaSetValues(items[i], XmNuserData, (XtArgVal)item_values[i], NULL);
	XtAddCallback(items[i], XmNvalueChangedCallback,
			propP_setting_chgCB, (XtPointer*)changebar);
	XtAddCallback(items[i], XmNvalueChangedCallback,
			propP_radiobox_itemCB, (XtPointer)item_values[i]);

	/* Ensure Radiobox has a default Value set */
	if (i == 0)
	{
	    XtVaSetValues(items[i], XmNset, True, NULL);
	    XtVaSetValues(radiobox, XmNuserData, (XtArgVal)item_values[i], NULL);
	}
    }

}

int
prop_radiobox_get_value(
    PropRadioSetting	prs
)
{
    XtArgVal value;

    XtVaGetValues(prs->radiobox, XmNuserData, &value, NULL);

    return((int)value);
}

int
prop_radiobox_set_value(
    PropRadioSetting prs,
    XtPointer  	     value,
    BOOL	     trip_changebar
)
{
    int         num_children = 0;
    WidgetList  children = NULL;
    XtArgVal    childval = 0;
    Boolean     found = FALSE;
    int         i = 0;

    XtVaGetValues(prs->radiobox,
        XtNnumChildren,    &num_children,
        XtNchildren,       &children,
        NULL);

    for (i = 0; i < num_children; i++)
    {
        XtVaGetValues(children[i], XmNuserData, &childval, NULL);
        XmToggleButtonSetState(children[i], childval == (XtArgVal)value? TRUE : FALSE, FALSE);
        if (childval == (XtArgVal)value)
        {
            found = TRUE;
            XtVaSetValues(prs->radiobox, XmNuserData, (XtArgVal)value, NULL);
	    if (trip_changebar)
		prop_set_changebar(prs->changebar, PROP_CB_ON);
        }
    }
    if (!found)
    {
        util_dprintf(1,"prop_radio_set_value: invalid value\n");
        return ERROR;
    }
    return OK;
}

void
prop_label_field_init(
    PropFieldSetting	label_pfs,
    Widget		graphic_hint,
    WidgetList		labeltype_items,
    int			num_items
)
{
    int		i;

    if (label_pfs->label != NULL && graphic_hint != NULL)
    	XtVaSetValues(label_pfs->label,
		XmNuserData, (XtArgVal)graphic_hint,
		XmNrecomputeSize, False,
		NULL);

    for(i=0; i < num_items; i++)
        XtAddCallback(labeltype_items[i], XmNactivateCallback,
                (XtCallbackProc)propP_labeltypeCB, (XtPointer)label_pfs);

}

void
prop_setup_label_field(
    PropFieldSetting	label_pfs,
    PropOptionsSetting	label_line_style_pos,
    AB_LABEL_TYPE 	ltype,
    STRING		field_val,
    AB_LINE_TYPE	line_style
)
{
    Widget	graphic_hint = NULL;

    XtVaGetValues(label_pfs->label, XmNuserData, &graphic_hint, NULL);

    switch(ltype)
    {
	case AB_LABEL_STRING:
	case AB_LABEL_GLYPH:
           ui_set_label_string(label_pfs->label,
		(STRING)(ltype == AB_LABEL_STRING? LabelForString : LabelForGraphic));
           if (graphic_hint)
                ui_set_active(graphic_hint, (ltype == AB_LABEL_GLYPH));
	   ui_set_active(label_pfs->label, True);
           ui_field_set_editable(label_pfs->field, True);

           if (label_line_style_pos)
           {
                ui_set_active(label_line_style_pos->label, False);
                ui_set_active(label_line_style_pos->optionbox, False);
           }
	   break;
	case AB_LABEL_SEPARATOR:
	    ui_set_active(label_pfs->label, False);
	    ui_field_set_string(label_pfs->field, "");
	    ui_field_set_editable(label_pfs->field, False);
	    ui_set_active(label_line_style_pos->label, True);
	    ui_set_active(label_line_style_pos->optionbox, True);
	    if (graphic_hint)
		ui_set_active(graphic_hint, False);
	    break;
    }
    if (field_val != NULL)
    {
    	XtVaSetValues(label_pfs->field, XmNuserData, (XtArgVal)PROP_LOAD, NULL);
    	ui_field_set_string(label_pfs->field, field_val);
    	XtVaSetValues(label_pfs->field, XmNuserData, (XtArgVal)PROP_EDIT, NULL);
    }
    if (line_style != AB_LINE_UNDEF)
	prop_options_set_value(label_line_style_pos, (XtPointer)line_style, False);

    /* WORKAROUND: for Motif bug */
    XtVaSetValues(label_pfs->label, XmNalignment, XmALIGNMENT_END, NULL);

}

/*
 * return TRUE if changebar is "on"
 */
BOOL
prop_changed(
    Widget changebar
)
{
    XtArgVal     cb_state;

    XtVaGetValues(changebar, XmNuserData, &cb_state, NULL);

    if (cb_state == PROP_CB_ON)
        return(TRUE);
    else
        return(FALSE);

}

/*
 * Get the running-count of changebars that are "ON"
 */
BOOL
prop_changebars_pending(
    Widget	prop_sheet
)
{
    XtArgVal	   change_count;

    XtVaGetValues(prop_sheet, XmNuserData, &change_count, NULL);

    if (change_count > 0)
	return True;
    else
	return False;

}

/*
 * Set the running-count of changebars that are "ON" to 0
 */
void
prop_changebars_cleared(
    Widget	prop_sheet
)
{
     XtVaSetValues(prop_sheet, XmNuserData, (XtArgVal)0, NULL);
}

/*
 * Set changebar to desired state: on or off
 * Also keep a running count of the number of changebars
 * that are "ON" in the userData of the changebars' parent
 */
void
prop_set_changebar(
    Widget    changebar,
    int        mode
)
{
    Widget prop_sheet;
    Pixel  bar_pixel;

    prop_sheet = XtParent(changebar);

    if (mode == PROP_CB_ON)
    {
/*
 REMIND: aim - workaround for Motif problem with Prop textfield
         input being lost...

	if (!prop_changed(changebar))
	{
*/
	    XtVaGetValues(XtParent(changebar),
			XmNforeground,	&bar_pixel,
			NULL);

            XtVaSetValues(changebar,
		 	XmNbackground,	bar_pixel,
                        XmNforeground,  bar_pixel,
                        XmNuserData,    (XtArgVal)PROP_CB_ON,
                        NULL);

	    /* Set dirty-bit */
	    XtVaSetValues(prop_sheet, XmNuserData, (XtArgVal)1, NULL);
/*
	}
*/
    }
    else /* PROP_CB_OFF */
    {
/*
        if (prop_changed(changebar))
	{
*/
            XtVaGetValues(XtParent(changebar), XmNbackground, &bar_pixel, NULL);
            XtVaSetValues(changebar,
			XmNbackground,  bar_pixel,
                        XmNforeground,  bar_pixel,
                        XmNuserData,    (XtArgVal)PROP_CB_OFF,
                        NULL);
/*
	}
*/
    }
}

/*
 * Verify that the colorname entered is valid
 */
BOOL
prop_color_ok(
    Widget    field
)
{
    STRING colorname;
    BOOL   valid = TRUE;

    colorname = ui_field_get_string(field);

    /* No color is OK */
    if (util_strempty(colorname))
	return valid;

    if (!objxm_color_exists(colorname))
    {
        sprintf(Buf,
            catgets(Dtb_project_catd, 100, 112, "%s is not a valid color."),
                        colorname);
	util_set_help_data(catgets(Dtb_project_catd, 100, 113,
	    	    "The color that was specified is not recognized\nas a valid color name."), NULL, NULL);
	valid = FALSE;

    	propP_popup_message(field, Buf, False);
    }
    util_free(colorname);
    return valid;

}

/*
 * Verify that the Glyph filename entered is valid
 */
BOOL
prop_graphic_filename_ok(
    Widget	field,
    BOOL	empty_ok
)
{
    char   filebase[512];
    STRING filename;
    STRING ext;
    Pixmap pixmap;
    int	   status;
    BOOL   valid = False;

    filename = ui_field_get_string(field);

    if (util_strempty(filename))
    {
	if (empty_ok)
	    valid = True; /* No Filename is valid */
	else
	{
	    util_set_help_data(catgets(Dtb_project_catd, 100, 115,
		"If Graphic is specified as the Label Type for\nthe object whose properties are being modified,\nthen a pixmap (.pm, .xpm) or bitmap (.bm, .xbm)\nfile name must be specified in the Graphic Filename\nfield."), NULL, NULL);

	    propP_popup_message(field,
		catgets(Dtb_project_catd, 100, 114,
		   "The \"Graphic Filename\" field cannot be empty."), False);
	}
    }
    else
    {
	strcpy(filebase, filename);
	/* Look to see if user typed in the full filename for the Graphic.
	 * If so, strip off the extension and mark stripped to be True.
	 */
	if (util_file_name_has_extension(filename, "pm") ||
	    util_file_name_has_extension(filename, "xpm") ||
	    util_file_name_has_extension(filename, "bm") ||
	    util_file_name_has_extension(filename, "xbm"))
	{
	    ext = strrchr(filebase, '.');
	    ext[0] = '\0'; /* strip off extenstion */

            util_set_help_data(catgets(Dtb_project_catd, 100, 117,
		"The graphic file name extension should not be included\nwhen it is specified in the Graphic Filename field."), NULL, NULL);

	    propP_popup_message(field,
		catgets(Dtb_project_catd, 100, 116,
		    "\"Graphic Filename\" field expects the filename\nbase only (no extensions: .pm .xpm .bm .xbm).\nStripping off the extension."), True);
	    ui_field_set_string(field, filebase);
	}
	status = objxm_filebase_to_pixmap(field, filebase, &pixmap);
	if (status == OK)
	{
	    valid = TRUE;
    	    XmDestroyPixmap(XtScreen(field), pixmap);
    	}
    	else
	    propP_popup_message(field,
		objxm_pixmap_conversion_error_msg(NULL, filebase, status), False);

    }
    util_free(filename);

    return valid;
}

BOOL
prop_help_item_ok(
    Widget	list,
    ABObj	item_obj
)
{
    int		num_items;
    ABObj	*iobj_list;
    BOOL	exists = FALSE;
    int		i;
    char	Buf[256];


    XtVaGetValues(list,
                XmNitemCount, &num_items,
                XmNuserData,  &iobj_list,
                NULL);

    /* Search through cascades, looking for another configured
     * to be the "Help" cascade
     */
    for (i = 0; i < num_items; i++)
    {
	if (obj_is_help_item(iobj_list[i]) == TRUE &&
	    item_obj != iobj_list[i])
	{
	    exists = TRUE;
	    break;
	}
    }
    if (exists)
    {
	util_set_help_data(catgets(Dtb_project_catd, 100, 119,
	    "Two Menubar items cannot both be specified as the\nHelp cascade. To specify a different Menubar item,\nyou must first clear the item that is currently set\nto be the Help cascade, then select the new Menubar\nitem to be the Help cascade."), NULL, NULL);

        sprintf(Buf, catgets(Dtb_project_catd, 100, 118,
                "There can only be one Help cascade per Menubar.Item\n\"%s\" is already configured to be the Help cascade,\ntherefore Item \"%s\" cannot also be the Help cascade."),
		obj_get_label(iobj_list[i]), obj_get_label(item_obj));

        propP_popup_message(list, Buf, False);
	return FALSE;
    }

    return TRUE;

}

/*
 * Verify that identifier string only contains valid chars
 */
BOOL
prop_identifier_ok(
    Widget    field
)
{
    return(prop_string_ok(field, Name_ok_test, TRUE));
}



/*
 * verify that the name entered is valid & unique
 */
BOOL
prop_name_ok(
    ABObj    obj,
    Widget    field
)
{
    ABObj  module = obj_get_module(obj);
    ABObj  other = NULL;
    char   Buf[512];
    BOOL   valid = TRUE;
    STRING newname;
    STRING oldname;

    newname = ui_field_get_string(field);
    oldname = obj_get_name(obj);

    if (!newname || !*newname)
    {
	util_set_help_data(catgets(Dtb_project_catd, 100, 121,
	    "A string must be entered in the Object Name field."),
	    NULL, NULL);
    	propP_popup_message( field, catgets(Dtb_project_catd, 100, 120,
		"A name is required."), False);
	valid = FALSE;
    }
    else if (!prop_identifier_ok(field))
    	valid = FALSE;
    else if (!oldname)
    	oldname = "";

    if (valid &&
        ((other = obj_find_by_name(module, newname)) != NULL))
    {
	if (other != obj)
	{
	    util_set_help_data(catgets(Dtb_project_catd, 100, 123,
		"The name specified in the Object Name field is not\na unique name in the module. You must specify a unique\nname for the object."),
		NULL, NULL);

	    sprintf(Buf, catgets(Dtb_project_catd, 100, 122,
		"Another object in Module \"%s\"\nhas the name \"%s\".\nPlease enter a unique name."), util_strsafe(obj_get_name(module)),newname);
	    propP_popup_message(field, Buf, False);
	    valid = FALSE;
	}
    }
    util_free(newname);

    return valid;

}

BOOL
prop_number_ok(
    Widget    	field,
    STRING	field_name,
    int		min_val,
    int		max_val
)
{
    BOOL      	valid = TRUE;
    char      	*p;
    STRING    	string;
    char      	*s;
    int		value;

    string = ui_field_get_string(field);
    s = string;

    if ((s  != NULL) && (*s != '\0'))
    {
    	(void)strtol(s, &p, 10);

    	if((s == p) || ((s + strlen(s)) != p))
    	{
	    util_set_help_data(catgets(Dtb_project_catd, 100, 125,
		"The field only accepts integers. You must enter\nan integer value."), NULL, NULL);
            sprintf(Buf, catgets(Dtb_project_catd, 100, 124,
		"\"%s\" must contain an integer."),
                field_name);
            propP_popup_message(field, Buf, False);
	    valid = FALSE;
    	}
    }
    if (valid) /* is an integer */
    {
    	value = prop_str_to_int(string);
	if (value < min_val || value > max_val)
	{
            util_set_help_data(catgets(Dtb_project_catd, 100, 127,
		"The numerical value entered into the field is not valid. Specify a value in the given range."), NULL, NULL);
	    sprintf(Buf, catgets(Dtb_project_catd, 100, 126,
		"\"%s\" value (%d) out of\nvalid range [%d - %d]."),
		field_name, value, min_val, max_val);
	    propP_popup_message(field, Buf, False);
	    valid = False;
	}
    }
    util_free(string);

    return(valid);
}

/*
 * Search the tree beginning at "root" for an object of the specified
 * type with the name currently stored in "field"
 * Return TRUE if found (or name is "") and FALSE otherwise
 */
BOOL
prop_obj_name_ok(
    Widget	field,
    ABObj	root,
    AB_OBJECT_TYPE objtype,
    STRING	objtype_name
)
{
    ABObj	target;
    STRING	objname;
    BOOL	valid = FALSE;
    char	msgbuf[256];

    objname = ui_field_get_string(field);

    /* ObjName can be blank OR the name of an existing obj */
    if (util_strcmp(objname, "") == 0)
        valid = TRUE;
    else if ((target = obj_scoped_find_by_name(root, objname)) != NULL)
    {
	if (target->type == objtype)
            valid = TRUE;
	else
	{
	    util_set_help_data(catgets(Dtb_project_catd, 100, 129,
		"The object specified is not of the correct type.\nFor example, it is an error to specify a button\nas the Popup Menu for a Control Pane."),
		NULL, NULL);
	    sprintf(msgbuf, catgets(Dtb_project_catd, 100, 128,
		"\"%s\" is not an object of type %s."),
		objname, objtype_name);
	}
    }
    else
    {
	util_set_help_data(catgets(Dtb_project_catd, 100, 136,
	    "The specified object does not exist or it is\nnot of the correct type."), NULL, NULL);

        sprintf(msgbuf, catgets(Dtb_project_catd, 100, 130,
		"\"%s\" is not the name of an existing %s."),
                objname, objtype_name);
    }

    if (valid == FALSE)
        propP_popup_message(field, msgbuf, False);

    util_free(objname);

    return valid;

}

BOOL
prop_submenu_name_ok(
    Widget      field,
    ABObj	owner
)
{
    BOOL	valid = False;
    ABObj	module = obj_get_module(owner);
    char	msgbuf[256];

    /* Check to see if Submenu exists first */
    if (prop_obj_name_ok(field, module, AB_TYPE_MENU, "Menu"))
    {
	valid = True;

	/* Check that the submenu name specified will not cause a circular
	 * menu (if the submenu is one that is in the owner's ancestory)
	 */
	if (obj_is_menu(owner))
	{
	    ABObj	menu, *menus, *ineligible_menus;
     	    int		menu_count = 0;
     	    int		ineligible_count = 0;
    	    STRING	menu_name;

    	    menu_name = ui_field_get_string(field);
    	    abobj_build_menus_array(module, &menus, &menu_count);

	    /* Build list of any menus in the ancestory of "owner" menu
	     * since these are not valid submenus
	     */
            ineligible_menus = (ABObj*)util_malloc(menu_count*sizeof(ABObj));
            ineligible_menus[ineligible_count++] = owner;
            find_submenu_owners(owner, menus, menu_count,
                	ineligible_menus, &ineligible_count);

	    /* Check to see if the specified submenu is in the ineligible list */
	    menu = obj_scoped_find_by_name(module, menu_name);
            if (menu_in_list(ineligible_menus, ineligible_count, menu))
	    {
		STRING	fmtStr = NULL;
		STRING	help_buf = NULL;

		valid = False;

		fmtStr = XtNewString(catgets(Dtb_project_catd,
			100, 132, "Menu \"%s\" is attached as a sub-menu\nto \"%s\". You cannot create a circular\nreference within menus."));

		help_buf = (STRING) util_malloc(strlen(fmtStr) +
		    strlen(obj_get_name(owner)) + strlen(menu_name) +1);
		sprintf(help_buf, fmtStr, obj_get_name(owner), menu_name);

		util_set_help_data(help_buf, NULL, NULL);
		sprintf(msgbuf, catgets(Dtb_project_catd, 100, 131,
		    "Menu \"%s\" is an ancestor of \"%s\"\ntherefore it cannot be attached as a Sub-menu."), menu_name, obj_get_name(owner));
        	propP_popup_message(field, msgbuf, False);

		XtFree(fmtStr);
		util_free(help_buf);
	    }
	    util_free(menus);
	    util_free(ineligible_menus);
	    util_free(menu_name);
        }
    }
    return valid;
}


int
prop_str_to_int(
    STRING      str
)
{
    int    val;
    char   *s = str;
    char   *p;

    if ((s != NULL) && (*s != '\0'))
    {
        val = (int)strtol(s, &p, 10);

        if ((s == p) || ((s + strlen(s)) != p))
        {
            return(ERR_ATOI);
        }
        return(val);
    }
    return(ERR_ATOI);
}

/*
 * verify that a string contains only valid characters
 */
BOOL
prop_string_ok(
    Widget    field,
    const char    *chars,
    BOOL    display_notice
)
{
    STRING string;
    char   *s;

    string = ui_field_get_string(field);
    s = string;

    while(s && *s != '\0')
    {
    	if (!isalnum(*s) && (strchr(chars, *s) == NULL))
    	{
            if (display_notice)
            {
		util_set_help_data(catgets(Dtb_project_catd, 10, 69,
		    "Object names are used by the code generator to\ncreate C identifier names. C identifiers must\nbe composed of letters, digits, or underscores.\nTherefore, object names in App Builder must also\nfollow that rule."),
		     NULL, NULL);
             	sprintf(Buf, catgets(Dtb_project_catd, 100, 133,
		    "Only letters, digits, and [%s] allowed."), chars);
                propP_popup_message(field, Buf, False);
            }
	    util_free(string);
            return(FALSE);
        }
        s++;
    }
    util_free(string);
    return(TRUE);
}


/*
 *****************************************************************
 * Module Private Functions
 *****************************************************************
 */

/*
 * Set Changebar's geometry so that it is aligned with and equal in height to,
 * it's corresponding setting.  This must be done dynamically because fonts
 * and localization could cause these metrics to change at dtbuilder invocation.
 */
static void
set_changebar_size(
    Widget	shell,
    XtPointer	client_data,
    XEvent	*event,
    Boolean	*cont_dispatch
)
{
    Widget	setting, changebar;
    Widget	setting_p, changebar_p;
    Position	setting_y, delta_y;
    Dimension	setting_h;
    Widget	*wlist = (Widget*)client_data;

    if (event->type == MapNotify)
    {
    	changebar = wlist[0];
    	setting = wlist[1];

	XtVaGetValues(setting,
		XmNheight, &setting_h,
		XmNy,	   &setting_y,
		NULL);

	changebar_p = XtParent(changebar);
	setting_p = setting;

	/* Calculate the setting's overall y position relative to the
	 * entire property sheet by walking up the setting's ancestry
	 * and accumulating the relative y offsets.
	 */
    	while (setting_p && XtParent(setting_p) != changebar_p)
	{
            setting_p = XtParent(setting_p);
	    XtVaGetValues(setting_p, XmNy, &delta_y, NULL);
	    setting_y += delta_y;
	}

	/* Align the changebar up with the setting */
	XtVaSetValues(changebar,
		XmNheight, 	setting_h - 2,
		XmNy,	   	setting_y + 1,
		XmNtopOffset, 	setting_y + 1,
		NULL);

	/* Only need to do this once per changebar! */
    	XtRemoveEventHandler(shell, StructureNotifyMask, False,
		set_changebar_size, (XtPointer)wlist);

	util_free(wlist);
    }
}

/*
 * Initialize the changebar
 */
void
propP_changebar_init(
    Widget	changebar,
    Widget	setting
)
{
    Widget	shell = ui_get_ancestor_shell(changebar);
    Widget	*wlist;
    unsigned char changebar_top_att;

    XtVaSetValues(changebar, XmNuserData, (XtArgVal)PROP_CB_ON, NULL);
    prop_set_changebar(changebar, PROP_CB_OFF);

    /* If changebar already has attachment, let's not mess with it */
    XtVaGetValues(changebar, XmNtopAttachment, &changebar_top_att, NULL);
    if (changebar_top_att != XmATTACH_FORM)
	return;

    wlist = (Widget*)util_malloc(2*sizeof(Widget));
    if (wlist == NULL)
	return;

    wlist[0] = changebar;
    wlist[1] = setting;

    /* Once the shell is mapped we'll need to calculate the appropriate
     * changebar size/position to match its corrsponding setting
     * (which could change dynamically due to fonts/localization)
     */
    XtAddEventHandler(shell, StructureNotifyMask, False,
	set_changebar_size, (XtPointer)wlist);
}

/*
 * Callback: textfield value has changed...turnon changebar
 *	     if operation is not a "load"
 */

void
propP_combobox_chgCB(
    Widget widget,
     XtPointer clientdata,
     XtPointer calldata
)
{
    PropObjComboboxSetting pcs = (PropObjComboboxSetting)clientdata;

    prop_set_changebar(pcs->changebar, PROP_CB_ON);
}

void
propP_field_chgCB(
     Widget widget,
     XtPointer clientdata,
     XtPointer calldata
)
{
     Widget      changebar = (Widget)clientdata;
     XtArgVal    field_mode, cb_state;

     XtVaGetValues(widget, XmNuserData, &field_mode, NULL);
/*
 REMIND: aim - workaround for Motif problem with Prop textfield
	 input being lost...

     XtVaGetValues(changebar, XmNuserData, &cb_state,  NULL);

     if (field_mode == PROP_EDIT && cb_state == PROP_CB_OFF)
*/
     if (field_mode == PROP_EDIT)
    	prop_set_changebar(changebar, PROP_CB_ON);

}

int
propP_field_set_value(
    Widget	field,
    STRING	value,
    BOOL	trip_changebar
)
{
    if (!trip_changebar)
    	/* Set state so changebar is not triggered */
    	XtVaSetValues(field, XmNuserData, (XtArgVal)PROP_LOAD, NULL);

    /* Set field value */
    ui_field_set_string(field, value);

    if (!trip_changebar)
    	/* Reset state */
   	XtVaSetValues(field, XmNuserData, (XtArgVal)PROP_EDIT, NULL);

    return OK;
}

int
propP_field_set_numeric_value(
    Widget      field,
    int         val,
    BOOL	trip_changebar
)
{
    char valstr[8];

    if (!trip_changebar)
        /* Set state so changebar is not triggered */
    	XtVaSetValues(field, XmNuserData, (XtArgVal)PROP_LOAD, NULL);

    /* Convert int to string */
    sprintf(valstr, "%d", val);
    ui_field_set_string(field, valstr);

    if (!trip_changebar)
        /* Reset state */
    	XtVaSetValues(field, XmNuserData, (XtArgVal)PROP_EDIT, NULL);

    return 0;

}

void
propP_labeltypeCB(
    Widget    item,
    XtPointer clientdata,
    XtPointer call_data
)
{
    AB_LABEL_TYPE       label_type = AB_LABEL_UNDEF;
    PropFieldSetting    label_pfs = (PropFieldSetting)clientdata;

    XtVaGetValues(item, XmNuserData, &label_type, NULL);
    prop_setup_label_field(label_pfs, NULL, label_type, NULL, AB_LINE_UNDEF);
    ui_field_select_string(label_pfs->field, True);
}

void
propP_options_itemCB(
    Widget      item,
    XtPointer   client_data,
    XtPointer   call_data
)
{
    Widget      optionmenu = (Widget)client_data;
    XtArgVal	value;

    XtVaGetValues(item, XmNuserData, &value, NULL);

    XtVaSetValues(optionmenu, XmNuserData, (XtArgVal)value, NULL);

}

/*
 * popup Error Dialog displaying passed in message
 */
void
propP_popup_message(
    Widget    	parent,
    STRING    	msg,
    BOOL	modal
)
{
    DtbObjectHelpDataRec    help_data;
    STRING                  help_text = NULL;
    STRING                  help_vol = NULL;
    STRING                  help_loc = NULL;
    XmString	xm_buf = XmStringCreateLocalized(msg);

    dtb_revolv_wrn_msg_initialize(&dtb_revolv_wrn_msg);

    util_get_help_data(&help_text, &help_vol, &help_loc);
    if (!util_strempty(help_text))
    {
	help_data.help_text = help_text ? (STRING) XtNewString(help_text) : NULL;
	help_data.help_volume = help_vol ? (STRING) XtNewString(help_vol) : NULL;
	help_data.help_locationID = help_loc ? (STRING) XtNewString(help_loc) : NULL;
    }
    else
    {
            /* If there is no help text to display,
             * then remove the Help button.
             */
            dtb_revolv_wrn_msg.help_button = False;
    }

    if (!util_strempty(help_text))
    {
	dtb_show_message(parent,
                &dtb_revolv_wrn_msg, xm_buf, &help_data);

	/*
	 * Free help data strings
	 */
	if (help_data.help_text);
	    XtFree(help_data.help_text);
	if (help_data.help_volume);
	    XtFree(help_data.help_volume);
	if (help_data.help_locationID);
	    XtFree(help_data.help_locationID);
    }
    else
	dtb_show_message(parent,
		&dtb_revolv_wrn_msg, xm_buf, NULL);

    XmStringFree(xm_buf);
}

/*
 * Callback: choice value has changed...turnon changebar
 */
void
propP_setting_chgCB(
     Widget widget,
     XtPointer clientdata,
     XtPointer calldata
)
{
     Widget changebar = (Widget)clientdata;
     XtArgVal   cb_state;

     XtVaGetValues(changebar, XmNuserData, &cb_state,  NULL);

     if (cb_state == PROP_CB_OFF)
        prop_set_changebar(changebar, PROP_CB_ON);

}

/*
 * Callback: item in exclusive choice has been set...store it's
 *	     value in the parent radiobox
 */
void
propP_radiobox_itemCB(
    Widget item,
    XtPointer clientdata,
    XtPointer calldata
)
{
    XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct*)calldata;
    XtArgVal  value = (XtArgVal)clientdata;
    Widget excl_setting;

    if (state->set)
    {
    	excl_setting = XtParent(item);
    	XtVaSetValues(excl_setting, XmNuserData, value, NULL);
    }
}


/*
 *****************************************************************
 * Private Functions
 *****************************************************************
 */

static void
revolv_dialog_init(
    Widget	parent
)
{
    DtbRevolvPropDialogInfo     cgen = &dtb_revolv_prop_dialog; /* Codegen structure */

    if (dtb_revolv_prop_dialog_initialize(cgen, parent) == 0)
    {
    	AB_rev_prop_dialog = cgen->prop_dialog_shellform;

	XtVaSetValues(cgen->prop_dialog_form,
		XmNresizePolicy,	XmRESIZE_ANY,
		XmNallowResize,		True,
		XmNautoUnmanage,	False,
		NULL);

    	revolv_objectspane_init(cgen);
	revolv_basicframe_init(cgen);
	prop_activate_panel_init(AB_PROP_REVOLVING, NULL,
		cgen->ok_button, cgen->apply_button,
		cgen->reset_button, cgen->cancel_button,
		cgen->help_button);

	init_prop_state_info(AB_rev_prop_dialog, NULL, cgen->objlist, NULL);

        ab_register_window(AB_rev_prop_dialog, AB_WIN_DIALOG,
                WindowHidden, AB_toplevel, AB_WPOS_TILE_HORIZONTAL,
		close_propsCB, (XtPointer)AB_PROP_REVOLVING);
    }
}

/*
 * In order to get the automatically generated dtbuilder
 * resource file to work with our Revolving Props cloning
 * mechanism, we need to name each Revolving prop sheet
 * frame to be the SAME as the dialog shellform within
 * the Fixed version (to ensure the attribute settings
 * resources are picked up in the Revolving version).
 */
static String
get_prop_frame_name(
    PalItemInfo	*palitem
)
{
    switch (palitem->type)
    {
	case AB_TYPE_BASE_WINDOW:
	    return "dtb_mainwin_prop_dialog_shellform";
	case AB_TYPE_DIALOG:
	    return "dtb_custdlg_prop_dialog_shellform";
	case AB_TYPE_FILE_CHOOSER:
	    return "dtb_fchooser_prop_dialog_shellform";
	case AB_TYPE_CONTAINER:
	    if (util_streq(palitem->name, "Control Pane"))
		return "dtb_cpanel_prop_dialog_shellform";
	    else if (util_streq(palitem->name, "Group"))
		return "dtb_group_prop_dialog_shellform";
	    else if (util_streq(palitem->name, "Menubar"))
		return "dtb_menubar_prop_dialog_shellform";
	    else
		return "dtb_panedwin_ed_prop_dialog_shellform";
	case AB_TYPE_DRAWING_AREA:
	    return "dtb_drawp_prop_dialog_shellform";
	case AB_TYPE_TEXT_PANE:
            return "dtb_textp_prop_dialog_shellform";
	case AB_TYPE_TERM_PANE:
            return "dtb_termp_prop_dialog_shellform";
	case AB_TYPE_BUTTON:
            return "dtb_button_prop_dialog_shellform";
	case AB_TYPE_CHOICE:
            return "dtb_choice_prop_dialog_shellform";
	case AB_TYPE_COMBO_BOX:
            return "dtb_combobox_prop_dialog_shellform";
	case AB_TYPE_LABEL:
            return "dtb_label_prop_dialog_shellform";
	case AB_TYPE_LIST:
            return "dtb_list_prop_dialog_shellform";
	case AB_TYPE_MENU:
            return "dtb_menu_prop_dialog_shellform";
	case AB_TYPE_SCALE:
            return "dtb_scale_prop_dialog_shellform";
	case AB_TYPE_SEPARATOR:
            return "dtb_sep_prop_dialog_shellform";
	case AB_TYPE_SPIN_BOX:
            return "dtb_spinbox_prop_dialog_shellform";
	case AB_TYPE_TEXT_FIELD:
            return "dtb_textf_prop_dialog_shellform";
	default:
	    return "revolv_prop_frame";
    }
}

static void
revolv_basicframe_init(
    DtbRevolvPropDialogInfo	cgen
)
{
    int     	i;
    Widget	rev_form;
    Widget	rev_frame;

    /* Destroy dummy frame & controlpanel */
    XtDestroyWidget(cgen->attrs_ctrlpanel_frame);

    /* Create Form to manage all revolving frames */
    rev_form = XtVaCreateManagedWidget("revolving_dialog_form",
		xmFormWidgetClass,
		cgen->prop_dialog_form,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		cgen->objlist_panel,
		XmNrightAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNbottomAttachment,	XmATTACH_FORM,
		XmNresizePolicy,	XmRESIZE_ANY,
		XmNheight,		600,
		NULL);

    /*
     * Create frame for each object on the palette...
     */
    for (i = 0; i < palette_item_cnt; i++)
    {
    	rev_frame = cgen->attrs_ctrlpanel_frame =
                XtVaCreateWidget(get_prop_frame_name(palette_item[i]),
                xmFrameWidgetClass,
                rev_form,
		XmNshadowType,		XmSHADOW_IN,
                XmNtopAttachment,       XmATTACH_FORM,
                XmNrightAttachment,     XmATTACH_FORM,
                XmNleftAttachment,      XmATTACH_FORM,
                XmNbottomAttachment,    XmATTACH_FORM,
                NULL);

        (*(palette_item[i]->prop_initialize))(rev_frame, AB_PROP_REVOLVING);
        palette_item[i]->rev_prop_frame = rev_frame;

	/* Make sure Frame is not initially managed */
	XtUnmanageChild(rev_frame);
    }
}

static BOOL
editable_obj_test(
    PalEditableObjInfo *ed_obj_info
)
{
    return (ed_obj_info->palitem != NULL);
}

static void
revolv_objectspane_init(
   DtbRevolvPropDialogInfo	cgen
)
{

    revolv_optionmenu = cgen->objtype_opmenu;

    /* Destroy dummy Option menu item */
    XtDestroyWidget(cgen->objtype_opmenu_items.object_type_item);

    /* Build up Object Type Option Menu */
    pal_add_editable_obj_menu_items(cgen->objtype_opmenu_menu,
	change_objecttypeCB, editable_obj_test);

    /*
     * Configure Revolving Behavior
     */
    XtAddCallback(cgen->tearoff_button, XmNactivateCallback,
		tearoff_propsCB, NULL);

    XtAddCallback(cgen->objlist, XmNbrowseSelectionCallback,
            (XtCallbackProc)objlist_selectCB,
		(XtPointer)AB_PROP_REVOLVING);

    /* Setup Object List Label */
    XtVaSetValues(cgen->objlist_label2,
            XmNalignment,        XmALIGNMENT_END,
            XmNrightAttachment,  XmATTACH_WIDGET,
            XmNrightWidget,    	 cgen->objlist_scrolledwin,
	    XmNrightOffset,	 4,
            XmNleftAttachment,   XmATTACH_POSITION,
            XmNleftPosition,     0,
	    XmNbottomAttachment, XmATTACH_NONE,
	    NULL);

}

/*
 * Switch the Revolving-prop frame to display the pane
 * matching the specified Palette Item type.
 */
static void
revolv_change_prop_frame(
    PalItemInfo	*palitem
)
{
    Widget 		newframe = NULL;
    PropStateInfo 	*pstate;

    if (palitem->rev_prop_frame != NULL)
    {
        newframe = palitem->rev_prop_frame;
        XtManageChild(newframe);
    }
    if (newframe != NULL)
    {
        if (revolv_current_frame != NULL)
            XtUnmanageChild(revolv_current_frame);
        revolv_current_frame = newframe;
    }
    pstate = get_prop_state_info(AB_rev_prop_dialog);
    pstate->palitem = palitem;
    objlist_load(AB_PROP_REVOLVING, pstate);

}

/*
 * Attempt to change the Type of Props being edited
 * in the Revolving Prop dialog to the specified
 * Palette Item type.  IF there are currently un-applied
 * edits pending, handle "auto apply".
 */
static PalItemInfo *
revolv_set_prop_type(
    PalItemInfo *new_palitem,
    ABObj	load_obj
)
{
    PalItemInfo 	*viz_palitem = new_palitem;
    PropStateInfo 	*pstate;

    if (revolv_current_frame != new_palitem->rev_prop_frame)
    {
	pstate = get_prop_state_info(AB_rev_prop_dialog);

	if (pstate->palitem &&
	    (*pstate->palitem->prop_pending)(AB_PROP_REVOLVING) == TRUE)
	    handle_auto_apply(AB_PROP_REVOLVING, pstate,
				new_palitem, load_obj, &viz_palitem);
	else
	    revolv_change_prop_frame(new_palitem);
    }

    /* Return current visible Palette Item type (might be different
     * than one requested if user canceled load operation during
     * auto-apply
     */
    return(viz_palitem);

}

/*
 * Invoke Revolving Property Dialog (create if it doesn't exist)
 * and set it's revolving-type to the Palette Item type
 * corrsponding to the loadobj's type (if loadobj == NULL, then
 * no specific object is being loaded).
 */
static PalItemInfo *
revolv_invoke_props(
   PalItemInfo   *palitem,
   ABObj	 load_obj
)
{
    PalItemInfo *viz_palitem;

    if (!AB_rev_prop_dialog)
	revolv_dialog_init(AB_toplevel);

    viz_palitem = revolv_set_prop_type(palitem, load_obj);

    ab_show_window(AB_rev_prop_dialog);

    ui_optionmenu_change_label(revolv_optionmenu, viz_palitem->name);

    return(viz_palitem);

}

static void
fixed_invoke_props(
    PalItemInfo   *palitem
)
{
    Widget      	prop_dialog;
    PropStateInfo 	*pstate;
    char        	title[64];
    static BOOL		first_time = True;

    if (palitem->fix_prop_dialog == NULL)
    {
        prop_dialog = (*(palitem->prop_initialize))(AB_toplevel, AB_PROP_FIXED);

	if (prop_dialog != NULL)
	{
	    palitem->fix_prop_dialog = prop_dialog;
	    pstate = get_prop_state_info(palitem->fix_prop_dialog);

            /* Load all current objects of this type into the list */
            objlist_load(AB_PROP_FIXED, pstate);
	}
    }
    else
	prop_dialog = palitem->fix_prop_dialog;

    if (prop_dialog != NULL)
    {
	if (first_time)
	{
	    ab_position_window(prop_dialog,
			ab_window_is_open(AB_rev_prop_dialog)?
				AB_rev_prop_dialog : AB_toplevel,
			    	AB_WPOS_TILE_HORIZONTAL);
	    first_time = False;
	}

	ab_show_window(prop_dialog);
    }
    else
       util_dprintf(1, "fixed_invoke_props: could not initialize Properties for %s\n",
                palitem->name);

}

static int
apply_props(
    AB_PROP_TYPE	type,
    Widget		dialog
)
{
    PropStateInfo	*pstate;

    pstate = get_prop_state_info(dialog);

    if (pstate->loaded_obj == NULL) /* no object currently loaded */
        return OK;

    return(apply_prop_changes(type, pstate->loaded_obj, pstate->palitem));

}

/*
 * Apply Prop changes to an object
 */
static int
apply_prop_changes(
    AB_PROP_TYPE type,
    ABObj	obj,
    PalItemInfo	*palitem
)
{
    Widget 	 	alt_dialog = NULL;
    AB_PROP_TYPE 	alt_type;
    PropStateInfo	*alt_pstate;
    int			result;

    apply_in_progress = True;
    result = (*(palitem->prop_apply))(type);
    apply_in_progress = False;

    /* If same object is also loaded into alternate prop dialog,
     * propagate the changes to the alternate dialog.
     */
    if (type == AB_PROP_REVOLVING && palitem->fix_prop_dialog != NULL)
    {
	alt_type   = AB_PROP_FIXED;
	alt_dialog = palitem->fix_prop_dialog;
    }
    else if (type == AB_PROP_FIXED &&
		palitem->rev_prop_frame == revolv_current_frame)
    {
	alt_type   = AB_PROP_REVOLVING;
	alt_dialog = AB_rev_prop_dialog;
    }

    if (alt_dialog)
    {
	alt_pstate = get_prop_state_info(alt_dialog);
	if (alt_pstate->loaded_obj == obj)
	    (*(palitem->prop_load))(obj, alt_type, LoadAll);
    }

    return result;
}

/*
 * Handle Auto-apply:
 * Another object is being loaded into a Prop dialog
 * however another object has edits pending;
 * Post a Modal Message asking the user to Apply the
 * changes or Cancel the Load operation.
 * Return the resulting Palette Item type which is
 * showing after the user makes a choice.
 */
static DTB_MODAL_ANSWER
handle_auto_apply(
    AB_PROP_TYPE 	prop_type,
    PropStateInfo	*pstate,
    PalItemInfo         *new_palitem,
    ABObj        	new_obj,
    PalItemInfo		**result
)
{
    DTB_MODAL_ANSWER 	answer = DTB_ANSWER_NONE;
    char	   	Buf[256];
    BOOL	   	changing_types = FALSE;
    STRING	   	modname;
    DtbObjectHelpData   help_data = NULL;

    if (pstate->loaded_obj == NULL) /* something's wrong */
    {
	util_dprintf(1,"handle_auto_apply: NULL previous obj!??\n");
	*result = NULL;
	return answer;
    }

    if (new_palitem != pstate->palitem)
	changing_types = TRUE;


    help_data = (DtbObjectHelpData) util_malloc(sizeof(DtbObjectHelpDataRec));
    help_data->help_volume = NULL;
    help_data->help_locationID = NULL;

    if (dtb_app_resource_rec.implied_apply == True)
    {
	answer = DTB_ANSWER_ACTION1;
    }
    else
    {
	XmString xm_buf = (XmString) NULL;
	STRING   loadedObjName = obj_get_name(pstate->loaded_obj);

	if (new_obj == NULL)
	{
	    if (changing_types)
	    {
		sprintf(Buf, catgets(Dtb_project_catd, 100, 34,
		"Properties for \"%s\" have been modified but not Applied.\
		\nApply Changes or Cancel Change-ObjectType operation."),
		loadedObjName);

		help_data->help_text = catgets(Dtb_project_catd, 100,95,
		    "Click Apply Changes to apply the changes to the\ncurrent object and display the new object type.\n\nClick Cancel if you don't want to apply the\nchanges to the current object. You can then\nclick Reset to undo the changes before changing\nto a different object type.");
	    }
	    else
	    {
		sprintf(Buf, catgets(Dtb_project_catd, 100, 35,
		"Properties for \"%s\" have been modified but not Applied.\
		\nApply Changes or Cancel Close operation."),
		loadedObjName);

		help_data->help_text = catgets(Dtb_project_catd, 100,96,
		    "Click Apply Changes to apply the changes to the\ncurrent object and close the Property Editor.\n\nClick Cancel if you don't want to apply the\nchanges to the current object and want the\nProperty Editor to remain displayed. You can\nthen click Reset to undo the changes before\nclosing the Property Editor.");
	    }
	}
	else
	{
		STRING   newObjName = obj_get_name(new_obj);

		sprintf(Buf, catgets(Dtb_project_catd, 100, 36,
		"Properties for \"%s\" have been modified but not Applied.\
		\nApply Changes or Cancel Load operation for \"%s\"."),
		 loadedObjName, newObjName);

		help_data->help_text = catgets(Dtb_project_catd, 100,93,
		    "Click Apply Changes to apply the changes to the\ncurrent object and load the selected object.\n\nClick Cancel if you don't want to apply the\nchanges to the current object. You can then\nclick Reset to undo the changes before loading\nthe selected object.");
	}

	/* Popup Modal Message and wait for answer */
	xm_buf = XmStringCreateLocalized(Buf);
	dtb_revolv_prop_pend_msg_initialize(&dtb_revolv_prop_pend_msg);
	answer = dtb_show_modal_message(pstate->objlist,
		&dtb_revolv_prop_pend_msg, xm_buf, help_data, NULL);

	util_free(help_data);
	XmStringFree(xm_buf);
    }

    switch(answer)
    {
	case DTB_ANSWER_ACTION1: /* Apply Changes */
		/*
		 * If Apply was not successful (value verification failed), then
		 * do not change to the new Prop sheet.
		 */
                if (apply_prop_changes(prop_type, pstate->loaded_obj, pstate->palitem) != OK)
		{
		    *result = pstate->palitem;
		    answer = DTB_ANSWER_CANCEL;
		}
	        else
		{
                    if (!changing_types)
                    	load_props(prop_type, pstate, new_obj);
                    else
                    	revolv_change_prop_frame(new_palitem);

		    *result = new_palitem;
		}
		break;

	case DTB_ANSWER_CANCEL: /* Cancel Load */
                if (changing_types)
		    ui_optionmenu_change_label(revolv_optionmenu, pstate->palitem->name);
                modname = abobj_get_moduled_name(pstate->loaded_obj);
                ui_list_select_item(pstate->objlist, modname, FALSE);
                util_free(modname);
		*result = pstate->palitem;
    }

    return answer;
}

static PropStateInfo *
get_prop_state_info(
    Widget	widget
)
{
    Widget	  dialog;
    PropStateInfo *pstate;

    dialog = ui_get_ancestor_dialog(widget);

    XtVaGetValues(dialog, XmNuserData, &pstate, NULL);

    return pstate;

}


static void
init_prop_state_info(
    Widget              dialog,
    PalItemInfo         *palitem,
    Widget              objlist,
    ABObj               loaded_obj
)
{
    PropStateInfo *pstate;

    pstate = (PropStateInfo*)util_malloc(sizeof(PropStateInfo));

    pstate->palitem = palitem;
    pstate->objlist = objlist;
    pstate->loaded_obj = loaded_obj;

    XtVaSetValues(dialog, XmNuserData, (XtArgVal)pstate, NULL);

}


/*
 * Load an object into the Revolving or Fixed Prop dialog
 */
static void
load_props(
    AB_PROP_TYPE 	type,
    PropStateInfo	*pstate,
    ABObj		obj
)
{
    pstate->loaded_obj = obj;

    /* Load obj's props */
    (*(pstate->palitem->prop_load))(obj, type, LoadAll);
}

static int
objlist_delete_obj(
    AB_PROP_TYPE	type,
    PalItemInfo		*palitem,
    ObjEvDestroyInfo    info
)
{
    Widget		dialog;
    PropStateInfo	*pstate;
    int			num_items;
    int			first_viz, last_viz;

    if (type == AB_PROP_REVOLVING)
	dialog = AB_rev_prop_dialog;
    else
	dialog = palitem->fix_prop_dialog;

    pstate = get_prop_state_info(dialog);

    cur_list_palitem = palitem;
    abobj_list_obj_destroyed(pstate->objlist, info->obj, objlist_test_func);

    ui_list_get_info(pstate->objlist, &num_items, &first_viz, &last_viz);

    if (num_items == 0) /* Deleted Last one...*/
    {
         /* Clear Prop Sheet & Make it Inactive */
        (*(palitem->prop_clear))(type);
        (*(palitem->prop_activate))(type, FALSE);
	pstate->loaded_obj = NULL;
    }
    else if (info->obj == pstate->loaded_obj) /* Deleted "Current" one...*/
    {
	/* If there are pending edits on the deleted object, first clear
	 * those edits via a reset before making another object "current"
	 * in props.
	 */
	if ((*(palitem->prop_pending))(type) == True)
    	    (*(palitem->prop_load))(NULL, type, LoadAll);

	ui_list_select_pos(pstate->objlist, 1, TRUE);
    }
    return 0;
}

/*
 * Ensure an item is selected in the Prop dialog list
 */
static void
objlist_ensure_selection(
    Widget	objlist
)
{
    int		num_items = 0;
    int		num_selected = 0;

    XtVaGetValues(objlist,
		XmNitemCount, 	      &num_items,
		XmNselectedItemCount, &num_selected,
		NULL);

    if (num_selected == 0 && num_items > 0)
	ui_list_select_pos(objlist, 1, TRUE);

}

/*
 * Traverse the project and load all objects matching the
 * Palette Item type into the Prop dialog list
 */
static void
objlist_load(
    AB_PROP_TYPE	type,
    PropStateInfo	*pstate
)
{
    ABObj           proj = proj_get_project();
    int		    num_items = 0;

    if (proj == NULL || pstate->objlist == NULL)
        return;

    /* Reset Loaded-obj to NULL */
    pstate->loaded_obj = NULL;

    cur_list_palitem = pstate->palitem;
    num_items = abobj_list_load(pstate->objlist, proj, objlist_test_func);

    /* If there are items in the list, activate the Props,
     * else de-activate them.
     */
    ((*pstate->palitem->prop_activate)(type, num_items > 0? TRUE : FALSE));

}

static int
objlist_rename_obj(
    AB_PROP_TYPE	type,
    PalItemInfo		*palitem,
    ObjEvAttChangeInfo    info
)
{
    Widget		dialog;
    PropStateInfo	*pstate;

    if (type == AB_PROP_REVOLVING)
	dialog = AB_rev_prop_dialog;
    else
	dialog = palitem->fix_prop_dialog;

    pstate = get_prop_state_info(dialog);

    cur_list_palitem = palitem;
    abobj_list_obj_renamed(pstate->objlist, info->obj,
		istr_string(info->old_name),
                objlist_test_func);

    return 0;

}

static int
objlist_show_or_hide_obj(
    AB_PROP_TYPE	type,
    PalItemInfo		*palitem,
    ObjEvUpdateInfo     info
)
{
    Widget		dialog;
    PropStateInfo	*pstate;
    int			orig_num_items = 0;
    int			num_items = 0;
    int			first_vis, last_vis;

    if (type == AB_PROP_REVOLVING)
	dialog = AB_rev_prop_dialog;
    else
	dialog = palitem->fix_prop_dialog;

    pstate = get_prop_state_info(dialog);

    ui_list_get_info(pstate->objlist, &orig_num_items, &first_vis, &last_vis);

    cur_list_palitem = palitem;
    abobj_list_obj_updated(pstate->objlist, info, objlist_test_func);

    ui_list_get_info(pstate->objlist, &num_items, &first_vis, &last_vis);

    if (orig_num_items == 0 && num_items > 0)
	(*(palitem->prop_activate))(type, TRUE);

    if (num_items == 0) /* Hid Last one...*/
    {
         /* Clear Prop Sheet & Make it Inactive */
        (*(palitem->prop_clear))(type);
        (*(palitem->prop_activate))(type, FALSE);
	pstate->loaded_obj = NULL;
    }
    else if (!(pstate->loaded_obj != NULL &&
		obj_has_flag(obj_get_module(pstate->loaded_obj), MappedFlag)) ||
        num_items == 1)
	/* Make sure 1 is selected */
        ui_list_select_pos(pstate->objlist, 1, True);

    return 0;
}

/*
 * Test whether an object should be loaded into the
 * Prop Dialog object list
 */
static BOOL
objlist_test_func(
        ABObj test_obj
)
{
    if ((obj_get_module(test_obj) != NULL) &&
	 obj_is_defined(test_obj) &&
	 obj_is_salient_ui(test_obj) &&
         obj_has_flag(test_obj, MappedFlag))
    {
	if (cur_list_palitem != NULL)
	    return((*cur_list_palitem->is_a_test)(test_obj));
        return(True);
    }
    return(False);
}

static void
create_obj_option_item(
    PropObjOptionsSetting	pos,
    Widget			item,
    ABObj       		obj
)
{
    Widget      newitem;
    STRING	modname = NULL;
    STRING	name = NULL;
    XmString    xmitem;

    if (obj == NULL)
	name = (String)NoneItem;
    else if (pos->display_module)
	modname = abobj_get_moduled_name(obj);
    else
	name = obj_get_name(obj);

    xmitem = XmStringCreateLocalized(modname? modname : name);

    if (item == NULL)
    {
	/* Create new item */
    	newitem = XtVaCreateWidget(modname? modname : name,
                xmPushButtonWidgetClass,
                pos->options.menu,
                XmNlabelString, xmitem,
                XmNuserData,    (XtArgVal)obj,
                NULL);

    	XtAddCallback(newitem, XmNactivateCallback,
                        propP_setting_chgCB, (XtPointer)pos->options.changebar);
    	XtAddCallback(newitem, XmNactivateCallback,
                        propP_options_itemCB, (XtPointer)pos->options.menu);
    }
    else /* Use existing item */
	XtVaSetValues(item,
		XmNlabelString,	xmitem,
		XmNuserData,    (XtArgVal)obj,
		NULL);

    XmStringFree(xmitem);
    util_free(modname);

}


/*
 ******************************************************************
 * Object Callbacks
 ******************************************************************
 */

/*
 * obj-callback: object name has changed - update Prop Dialog lists
 */
static int
obj_renamedOCB(
    ObjEvAttChangeInfo    info
)
{
    PalItemInfo 	*palitem;
    int			i;

    if (!obj_is_salient(info->obj) ||
	obj_get_parent(info->obj) == NULL ||
	obj_is_item(info->obj))
        return 0;

    util_dprintf(3, "obj_renamedOCB: from %s to %s\n", istr_string_safe(info->old_name),
	obj_get_name(info->obj));

    if (obj_is_module(info->obj)) /* Module name changed */
    {
    	/* Need to update ALL Prop Dialog object lists...*/
    	for (i = 0; i < palette_item_cnt; i++)
    	{
	    palitem = cur_list_palitem = palette_item[i];

	    if (palitem->rev_prop_frame == revolv_current_frame)
		objlist_rename_obj(AB_PROP_REVOLVING, palitem, info);

	    if (palitem->fix_prop_dialog != NULL)
		objlist_rename_obj(AB_PROP_FIXED, palitem, info);
	}
    }
    else if ((palitem = pal_get_item_info(info->obj)) != NULL) /* Object name changed */
    {
	/* If obj's type is currently loaded in Revolving props, update the list */
    	if (AB_rev_prop_dialog != NULL && revolv_current_frame == palitem->rev_prop_frame)
	    objlist_rename_obj(AB_PROP_REVOLVING, palitem, info);

    	/* If Fixed Props for obj's type exists, update its list */
    	if (palitem->fix_prop_dialog != NULL)
            objlist_rename_obj(AB_PROP_FIXED, palitem, info);
    }
    return 0;
}

/*
 * obj-callback: object geometry has changed - update Prop sheet if necessary
 */
static int
obj_geom_changedOCB(
    ObjEvAttChangeInfo    info
)
{
    PalItemInfo         *palitem;
    PropStateInfo 	*pstate;
    unsigned long	geomflag = 0;

    /* If "apply_in_progress" is True, then this update is being called
     * during the apply of geometry changes within a prop sheet, and we
     * DO not want to update prop fields in this case because it could
     * interfere with the apply.
     */
    if (!obj_is_salient(info->obj) ||
        obj_get_parent(info->obj) == NULL ||
        obj_is_item(info->obj) ||
	apply_in_progress)
        return 0;

    /* If a layers's position changes, then we need to update the
     * position attrs of any of its panes which are currently loaded
     * in props (since, in props, their x,y position is displayed as the
     * entire layers's position, even though the panes' *real* position
     * within the layers object is 0,0).
     */
    if (info->atts & OBJEV_ATT_POSITION &&
	obj_is_layers(info->obj))
    {
        ABObj 	layers = info->obj;
 	ABObj	pane;
	int 	num_panes = obj_get_num_children(layers);
        int	i;

	for (i=0; i < num_panes; i++)
	{
	    /* Use recursion to call this geometry callback for each
	     * pane in the layers object
	     */
	    pane = obj_get_child(layers, i);
	    info->obj = pane;
	    obj_geom_changedOCB(info);
	}
	info->obj = layers; /* reset back to original obj */
    }

    if ((palitem = pal_get_item_info(info->obj)) != NULL)
    {
	util_dprintf(3, "obj_geom_changedOCB: %s's geometry changed to %d,%d %dx%d\n",
		obj_get_name(info->obj), obj_get_x(info->obj), obj_get_y(info->obj),
		obj_get_width(info->obj), obj_get_height(info->obj));

	if (info->atts & OBJEV_ATT_POSITION)
	    geomflag |= LoadPosition;
	if (info->atts & OBJEV_ATT_SIZE)
	    geomflag |= LoadSize;

 	/* If modified obj is currently loaded in the Revolving prop editor, update
	 * only its geometry attributes in the prop sheet
	 */
        if (AB_rev_prop_dialog != NULL && revolv_current_frame == palitem->rev_prop_frame)
    	{
	    pstate = get_prop_state_info(AB_rev_prop_dialog);
	    if (pstate->loaded_obj == info->obj)
                (*(palitem->prop_load))(NULL, AB_PROP_REVOLVING, geomflag);
    	}

	/* If modified obj is currently loaded in its Fixed prop editor, update
         * only its geometry attributes in the prop sheet
         */
        if (palitem->fix_prop_dialog != NULL)
	{
	    pstate = get_prop_state_info(palitem->fix_prop_dialog);
	    if (pstate->loaded_obj == info->obj)
                (*(palitem->prop_load))(NULL, AB_PROP_FIXED, geomflag);
	}
    }
    return 0;
}

/*
 * obj-callback: object is being destroyed - remove from Prop dialog lists
 */
static int
obj_destroyedOCB(
    ObjEvDestroyInfo    info
)
{
    PalItemInfo	*palitem;

    if (!obj_is_salient(info->obj) ||
	 obj_get_parent(info->obj) == NULL ||
	 obj_is_item(info->obj))
        return 0;

#ifdef DEBUG
    {
	char	buf[1024];
        util_dprintf(3, "obj_destroyedOCB: %s\n",
            obj_get_safe_name(info->obj, buf, 1024));
    }
#endif /* DEBUG */

    if ((palitem = pal_get_item_info(info->obj)) != NULL) /* Object being destroyed */
    {
        /* If obj's type is currently loaded in Revolving props, update the list */
        if (AB_rev_prop_dialog != NULL && revolv_current_frame == palitem->rev_prop_frame)
	    objlist_delete_obj(AB_PROP_REVOLVING, palitem, info);

        /* If Fixed Props for obj's type exists, update its list */
        if (palitem->fix_prop_dialog != NULL)
	    objlist_delete_obj(AB_PROP_FIXED, palitem, info);
    }
    return 0;
}

/*
 * obj-callback: object being shown or hidden - modify Prop dialog lists
 */
static int
obj_shown_or_hiddenOCB(
    ObjEvUpdateInfo	info
)
{
    PalItemInfo *palitem;
    int		i;

    if (!obj_is_salient(info->obj) ||
	 obj_get_parent(info->obj) == NULL ||
	 obj_is_item(info->obj))
        return 0;

    util_dprintf(3, "obj_shown_or_hiddenOCB: %s\n",
        obj_get_name(info->obj));

    if (obj_is_module(info->obj))
    {
        /* Need to update ALL Prop Dialog object lists...*/
        for (i = 0; i < palette_item_cnt; i++)
        {
            palitem = palette_item[i];

            if (palitem->rev_prop_frame == revolv_current_frame)
		objlist_show_or_hide_obj(AB_PROP_REVOLVING, palitem, info);

            if (palitem->fix_prop_dialog != NULL)
                objlist_show_or_hide_obj(AB_PROP_FIXED, palitem, info);
        }
    }
    else if ((palitem = pal_get_item_info(info->obj)) != NULL) /* Object being Updated */
    {
        cur_list_palitem = palitem;

        /* If obj's type is currently loaded in Revolving props, update the list */
        if (AB_rev_prop_dialog != NULL && revolv_current_frame == palitem->rev_prop_frame)
	    objlist_show_or_hide_obj(AB_PROP_REVOLVING, palitem, info);

        /* If Fixed Props for obj's type exists, update its list */
        if (palitem->fix_prop_dialog != NULL)
            objlist_show_or_hide_obj(AB_PROP_FIXED, palitem, info);
    }
    return 0;
}

/*
 ******************************************************************
 * Xt Callbacks
 ******************************************************************
 */

/*
 * Callback: apply properties for current object
 */
static void
apply_propsCB(
    Widget      widget,
    XtPointer   client_data,
    XtPointer   calldata
)
{
    AB_PROP_TYPE 	type = (AB_PROP_TYPE)client_data;
    PropStateInfo	*pstate;
    Widget	 	dialog;

    if (type == AB_PROP_REVOLVING)
        dialog = AB_rev_prop_dialog;
    else /* AB_PROP_FIXED */
    {
	pstate = get_prop_state_info(widget);
        dialog = pstate->palitem->fix_prop_dialog;
    }

    apply_props(type, dialog);

}

/*
 * Callback:
 */
static void
cancel_propsCB(
    Widget      widget,
    XtPointer   client_data,
    XtPointer   calldata
)
{
    AB_PROP_TYPE 	type = (AB_PROP_TYPE)client_data;
    PropStateInfo	*pstate;
    Widget       	dialog;

    pstate = get_prop_state_info(widget);

    if (type == AB_PROP_REVOLVING)
	dialog = AB_rev_prop_dialog;

    else /* AB_PROP_FIXED */
        dialog = pstate->palitem->fix_prop_dialog;

    (*(pstate->palitem->prop_load))(NULL,type,LoadAll);

    ui_win_show(dialog, False, XtGrabNone);

}

/*
 * Callback: a new object has been selected off object menu
 */
static void
change_objecttypeCB(
    Widget      widget,
    XtPointer   client_data,
    XtPointer   calldata
)
{
    PalEditableObjInfo	*ed_obj_info = (PalEditableObjInfo*)client_data;
    PalItemInfo		*viz_palitem;
    PropStateInfo	*pstate;

    viz_palitem = revolv_set_prop_type(ed_obj_info->palitem, NULL);

    if (viz_palitem == ed_obj_info->palitem)
    {
	/* Object-type change was successful */
    	pstate = get_prop_state_info(AB_rev_prop_dialog);
    	objlist_ensure_selection(pstate->objlist);
    }
    else
	/* Object-type change rejected; reset object-type */
    	ui_optionmenu_change_label(revolv_optionmenu, viz_palitem->name);


}

/*
 * Callback: Prop dialog has been closed -- check for pending changes
 */
static void
close_propsCB(
    Widget      widget,
    XtPointer   client_data,
    XtPointer   calldata
)
{
    AB_PROP_TYPE 	type = (AB_PROP_TYPE)client_data;
    DTB_MODAL_ANSWER	answer;
    PalItemInfo		*palitem = NULL;
    PropStateInfo	*pstate;
    WidgetList		shell_child;
    Widget		dialog;

    /* Get Immediate child of Shell */
    XtVaGetValues(widget,
		XmNchildren,	&shell_child,
		NULL);

    dialog = shell_child[0];
    pstate = get_prop_state_info(dialog);

    if ((*(pstate->palitem->prop_pending))(type) == TRUE)
        /* An Object is loaded with pending changes...*/
        answer = handle_auto_apply(type, pstate, pstate->palitem, NULL, &palitem);

    if (answer != DTB_ANSWER_CANCEL)
    	XtPopdown(widget);

}

/*
 * Callback: popup Attachment Editor
 */
static void
invoke_attach_editorCB(
    Widget      widget,
    XtPointer   client_data,
    XtPointer   calldata
)
{
    AB_PROP_TYPE type = (AB_PROP_TYPE)client_data;
    PropStateInfo	*pstate;

    pstate = get_prop_state_info(widget);

    if (pstate->loaded_obj == NULL)
	return;

    attch_ed_show_dialog(pstate->loaded_obj);
}

/*
 * Callback: popup Connections Editor
 */
static void
invoke_connectionsCB(
    Widget      widget,
    XtPointer   client_data,
    XtPointer   calldata
)
{
    AB_PROP_TYPE 	type = (AB_PROP_TYPE)client_data;
    PropStateInfo	*pstate;

    pstate = get_prop_state_info(widget);

    if (pstate->loaded_obj == NULL)
	return;

    conn_set_source(pstate->loaded_obj);
    conn_set_target(NULL);
    conn_popup_dialog(widget, (XtPointer)0, NULL);
}

/*
 * Callback: popup Help Editor Dialog
 */
static void
invoke_help_editorCB(
    Widget	widget,
    XtPointer	client_data,
    XtPointer	calldata
)
{
    AB_PROP_TYPE 	type = (AB_PROP_TYPE)client_data;
    PropStateInfo	*pstate;

    pstate = get_prop_state_info(widget);

    if (pstate->loaded_obj == NULL)
	return;

    ab_set_help_obj(pstate->loaded_obj);
    ab_popup_help(widget,(XtPointer)0,NULL);
}

static void
objlist_selectCB(
    Widget	widget,
    XtPointer	client_data,
    XmListCallbackStruct *listdata
)
{
    AB_PROP_TYPE 	type = (AB_PROP_TYPE)client_data;
    PropStateInfo	*pstate;
    PalItemInfo		*palitem = NULL;
    ABObj		module;
    ABObj        	selected_obj;
    STRING       	name;

    pstate = get_prop_state_info(widget);

    name = objxm_xmstr_to_str(listdata->item);
    if (name)
    {
        abobj_moduled_name_extract(name, &module, &selected_obj);
        util_free(name);

	/* If the object selected in the list is the one already
	 * loaded, then don't do anything.
	 */
        if (selected_obj && selected_obj != pstate->loaded_obj)
        {
            if ((*(pstate->palitem->prop_pending))(type) == TRUE)
                /* An Object is loaded with pending changes...*/
                handle_auto_apply(type, pstate, pstate->palitem, selected_obj, &palitem);
            else
                load_props(type, pstate, selected_obj);
            return;
        }
    }
}

/*
 * Callback: apply properties for current object & take down dialog
 */
static void
ok_propsCB(
    Widget      widget,
    XtPointer   client_data,
    XtPointer   calldata
)
{
    AB_PROP_TYPE 	type = (AB_PROP_TYPE)client_data;
    PropStateInfo	*pstate;
    Widget       	dialog;

    if (type == AB_PROP_REVOLVING)
        dialog = AB_rev_prop_dialog;

    else /* AB_PROP_FIXED */
    {
	pstate = get_prop_state_info(widget);
        dialog = pstate->palitem->fix_prop_dialog;
    }

    if (apply_props(type, dialog) == OK)
	ui_win_show(dialog, False, XtGrabNone);

}

/*
 * Callback: reset properties for current object
 */
static void
reset_propsCB(
    Widget      widget,
    XtPointer   client_data,
    XtPointer   calldata
)
{
    AB_PROP_TYPE 	type = (AB_PROP_TYPE)client_data;
    PropStateInfo	*pstate;

    pstate = get_prop_state_info(widget);

    (*(pstate->palitem->prop_load))(NULL,type,LoadAll);

}

/*
 * Callback:
 */
static void
tearoff_propsCB(
    Widget      widget,
    XtPointer   client_data,
    XtPointer   calldata
)
{
    PropStateInfo	*pstate;

    pstate = get_prop_state_info(AB_rev_prop_dialog);

    if (pstate->loaded_obj != NULL)
        prop_load_obj(pstate->loaded_obj, AB_PROP_FIXED);
    else
	fixed_invoke_props(pstate->palitem);

}

static void
color_chooserCB(
    Widget   w,
    XtPointer clientdata,
    XtPointer calldata
)
{
    PropColorSetting	pcs = (PropColorSetting)clientdata;
    String 		color_choice;
    Pixel 		c;
    int	  		status;

    color_choice = display_color_chooser();

    if (color_choice[0] != 0)
    {
	/* This should never error since the chooser enforces choosing
	 * a valid & available color, but check just to be sure.
	 */
        if ((status = objxm_name_to_pixel(w, color_choice, &c)) == OK)
	{
             XtVaSetValues(pcs->swatch,
             	XmNbackground, c,
             	NULL);
             XmTextSetString(pcs->field, color_choice);
	}
    }
}

static void
color_noneCB(
    Widget   w,
    XtPointer clientdata,
    XtPointer calldata
)
{
    PropColorSetting    pcs = (PropColorSetting)clientdata;

    /* Reset Swatch to appear transparent */
    prop_colorfield_set_value(pcs, "", True);

}

static void
menu_newCB(
    Widget      w,
    XtPointer   client_data,
    XtPointer   call_data
)
{
    PropMenunameSetting pms = (PropMenunameSetting)client_data;
    PropStateInfo 	*pstate;
    AB_PROP_TYPE	alt_prop_type = AB_PROP_FIXED;
    ABObj		module;
    Widget		prop_dialog;
    STRING      	base = NULL;
    STRING      	unique_name;
    STRING      	menu_name;
    STRING      	name;

    /* Build a menu-name based on the menu-owner's name or label */
    if (pms->owner_name_field != NULL)
    {
        /* Use owner's name as prefix */
        base = ui_field_get_string(pms->owner_name_field);
    }

    module = obj_get_module(*(pms->current_obj_ptr));
    name = (base? ab_ident_from_name_and_label(base, "menu") : "menu");
    menu_name = strdup(name);
    unique_name = obj_alloc_unique_name_for_child(module, menu_name, -1);

    /* We want to create/load the new menu in a separate editor if
     * possible so that the user can view both the new menu AND the
     * object which owns it simultaneously.  However, if we are currently
     * in the Fixed Menu Props dialog and there is an object loaded in
     * the Revolving Prop dialog which has *pending* edits, don't try
     * to use it (suddently changing it's type & object will probably
     * confuse the user!).
     */
    if (pms->prop_type == AB_PROP_FIXED)
    {
	prop_dialog = ui_get_ancestor_dialog(pms->owner_name_field);

	/* Try using the Revolving Prop Editor if either we are currently
	 * in the Menu Fixed Props, or if the Menu Fixed Props has
	 * edits pending...
	 */
	if (prop_dialog == ab_menu_palitem->fix_prop_dialog ||
            (ab_menu_palitem->fix_prop_dialog &&
             (*ab_menu_palitem->prop_pending)(AB_PROP_FIXED) == True))
	{
            pstate = get_prop_state_info(AB_rev_prop_dialog);

            if (pstate->palitem &&
               (*pstate->palitem->prop_pending)(AB_PROP_REVOLVING) == False)
	    {
	    	alt_prop_type = AB_PROP_REVOLVING;
	    	revolv_invoke_props(ab_menu_palitem, NULL);
	    }
	}
    }

    if (alt_prop_type == AB_PROP_FIXED)
	prop_show_fixed(ab_menu_palitem);

    /* Insert the new menu name into the owner's field, UNLESS we
     * happen to be inside the Fixed Menu Prop dialog AND are also
     * creating the new menu in the Fixed Menu Prop dialog.
     */
    if (pal_create_menu(alt_prop_type, module, unique_name, NULL) == 0 &&
	!(prop_dialog == ab_menu_palitem->fix_prop_dialog &&
	  alt_prop_type == AB_PROP_FIXED))
        ui_field_set_string(pms->field, unique_name);

    util_free(base);
    util_free(menu_name);
    util_free(unique_name);
}

static BOOL
is_submenu_of(
    STRING	submenu_name,
    ABObj	menu
)
{
    ABObj	item;
    AB_TRAVERSAL trav;
    BOOL	is_sub = False;

    for(trav_open(&trav, menu, AB_TRAV_ITEMS);
       (item = trav_next(&trav)) != NULL; )
    {
        if (util_strcmp(obj_get_menu_name(item), submenu_name) == 0)
        {
	   is_sub = True;
           break;
        }
    }
    trav_close(&trav);
    return is_sub;
}

static void
find_submenu_owners(
    ABObj	submenu,
    ABObj	*menulist,
    int		menu_count,
    ABObj	*i_menulist,
    int		*i_count_ptr
)
{
    STRING	submenu_name;
    int		i;

    submenu_name = obj_get_name(submenu);
    for(i = 0; i < menu_count; i++)
    {
        if (submenu != menulist[i] &&
	    is_submenu_of(submenu_name, menulist[i]))
	{
            i_menulist[(*i_count_ptr)++] = menulist[i];
	    find_submenu_owners(menulist[i], menulist, menu_count,
		i_menulist, i_count_ptr);
	}
    }
}

static BOOL
menu_in_list(
   ABObj	*menulist,
   int		menu_count,
   ABObj	target_menu
)
{
    int i;

    for(i = 0; i < menu_count; i++)
	if (menulist[i] == target_menu)
	    return True;

    return False;
}

static void
menulist_buildCB(
    Widget      widget,
    XtPointer   client_data,
    XtPointer   call_data
)
{
    PropMenunameSetting	pms;
    ABObj	current_obj;
    ABObj	*menus;
    ABObj	*ineligible_menus = NULL;
    int		num_menus = 0;
    int		ineligible_count = 0;
    Widget      submenu;
    Widget      menus_item = (Widget)client_data;
    Widget      menus_menu = NULL;
    Widget      menus_menuitem;
    Widget      menu_field;
    STRING      menu_name;
    int         num_children;
    int         i, j;

    XtVaGetValues(widget, XmNsubMenuId, &submenu, NULL);
    XtVaGetValues(menus_item, XmNuserData, &pms, NULL);

    current_obj = *(pms->current_obj_ptr);

    abobj_build_menus_array(obj_get_module(current_obj), &menus, &num_menus);

    if (num_menus > 0)
    {
	if (obj_is_menu(current_obj)) /* Need to prevent circular menus */
	{
	    /* Build up a list of menus that belong to the current menu's
	     * ancestory (if it's a submenu) - these menus will not be eligible
	     * for selection as a submenu off the current menu.
	     */
	    ineligible_menus = (ABObj*)util_malloc(num_menus*sizeof(ABObj));
	    ineligible_menus[ineligible_count++] = current_obj;
	    find_submenu_owners(current_obj, menus, num_menus,
		ineligible_menus, &ineligible_count);
	}

	for (j = 0; j < num_menus; j++)
	{
            if (menus_menu == NULL)
                menus_menu = XmCreatePulldownMenu(submenu, "menus_menu", NULL, 0);

            menu_name = obj_get_name(menus[j]);

            menus_menuitem = XtVaCreateManagedWidget("menus_menuitem",
                        xmCascadeButtonWidgetClass,
                        menus_menu,
                        XtVaTypedArg, XmNlabelString, XtRString,
                                menu_name, strlen(menu_name)+1,
                        NULL);

	    /* If Menu is ineligible to be a Submenu, make it inactive */
	    if (menu_in_list(ineligible_menus, ineligible_count, menus[j]))
		ui_set_active(menus_menuitem, False);

            XtAddCallback(menus_menuitem, XmNactivateCallback,
                        menuname_setCB, (XtPointer)pms->field);
        }

        ui_set_active(menus_item, TRUE);
        XtVaSetValues(menus_item, XmNsubMenuId, menus_menu, NULL);
        XtVaSetValues(submenu, XmNuserData, (XtArgVal)menus_menu, NULL);

	util_free(menus);
	util_free(ineligible_menus);
    }
    else /* No Menus - inactivate the item */
    {
        ui_set_active(menus_item, FALSE);
        XtVaSetValues(submenu, XmNuserData, (XtArgVal)NULL, NULL);
    }
}

/*
 * Callback: destroy the Menu->Names submenu after it pops down
 */
static void
menulist_destroyCB(
    Widget	w,
    XtPointer   client_data,
    XtPointer   calldata
)
{
    Widget	submenu = (Widget)client_data;
    Widget	menus_menu;

    XtVaGetValues(submenu, XmNuserData, &menus_menu, NULL);

    if (menus_menu != NULL)
	XtDestroyWidget(menus_menu);
}

static void
menu_edit_set_stateCB(
    Widget      widget,
    XtPointer   client_data,
    XtPointer   call_data
)
{
    PropMenunameSetting 	pms;
    Widget			edit_item = (Widget)client_data;
    STRING			current_menuname = NULL;

    /* Set the "Edit Current" item to be active ONLY if there is a
     * menu name in the field
     */
    XtVaGetValues(edit_item, XmNuserData, &pms, NULL);
    current_menuname = ui_field_get_string(pms->field);
    ui_set_active(edit_item, !util_strempty(current_menuname));
    util_free(current_menuname);
}


/*
 * Callback: Clear Menu Name field
 */
static void
menuname_clearCB(
    Widget      w,
    XtPointer   client_data,
    XtPointer   calldata
)
{
    PropMenunameSetting	pms = (PropMenunameSetting)client_data;
    ui_field_set_string(pms->field, "");

}

/*
 * Callback: an item off the Menu->Names submenu was selected:
 * Set the value of the menu textfield to its label
 */
static void
menuname_setCB(
    Widget      w,
    XtPointer   client_data,
    XtPointer   calldata
)
{
    Widget	menu_field = (Widget)client_data;
    XmString	xm_menuname;
    STRING	menu_name;

    XtVaGetValues(w, XmNlabelString, &xm_menuname, NULL);
    menu_name = objxm_xmstr_to_str(xm_menuname);
    ui_field_set_string(menu_field, menu_name);
    util_free(menu_name);
}

static void
menu_editCB(
    Widget      w,
    XtPointer   client_data,
    XtPointer   call_data
)
{
    PropMenunameSetting pms = (PropMenunameSetting)client_data;
    STRING		current_menuname = NULL;
    ABObj		module;
    ABObj		menu;

    current_menuname = ui_field_get_string(pms->field);
    module = obj_get_module(*(pms->current_obj_ptr));

    if (!util_strempty(current_menuname))
    {
	if ((menu = obj_find_by_name(module, current_menuname)) != NULL)
	    prop_load_obj(menu, pms->prop_type == AB_PROP_FIXED?
		AB_PROP_REVOLVING : AB_PROP_FIXED);
	else
	{
            STRING  fmtStr = NULL;
            STRING  help_buf = NULL;
	    char Buf[256];

            fmtStr = XtNewString(catgets(Dtb_project_catd, 100, 135,
		"The menu with the name \"%s\" does not exist in\nthe module \"%s\". Specify a valid menu name."));

            help_buf = (STRING) util_malloc(strlen(fmtStr) +
		strlen(current_menuname) + strlen(obj_get_name(module)) +1);
            sprintf(help_buf, fmtStr, current_menuname, obj_get_name(module));

            util_set_help_data(help_buf, NULL, NULL);
            sprintf(Buf, catgets(Dtb_project_catd, 100, 134,
            	"Could not find menu \"%s\" in module \"%s\"."),
		current_menuname, obj_get_name(module));
            propP_popup_message(pms->field, Buf, False);

	    XtFree(fmtStr);
	    util_free(help_buf);
	}
    }
    util_free(current_menuname);
}

static void
menu_field_chgCB(
    Widget      w,
    XtPointer   client_data,
    XtPointer   call_data
)
{
    PropMenunameSetting pms = (PropMenunameSetting)client_data;
    STRING		value;
    BOOL		menu_there = False;

    if (pms->menu_title_pfs)
    {
	/* If a non-empty string in in Menu field, make Title active */
	value = ui_field_get_string(pms->field);
	menu_there = (!util_strempty(value));
	ui_set_active(pms->menu_title_pfs->label, menu_there);
	ui_set_active(pms->menu_title_pfs->field, menu_there);
   	util_free(value);
    }
}


int
prop_geomfield_clear(
    PropGeometrySetting pgs,
    GEOM_KEY            gkey
)
{
    Widget      field = NULL;

    switch(gkey)
    {
        case GEOM_X:
            field = pgs->x_field;
            break;
        case GEOM_Y:
            field = pgs->y_field;
            break;
        case GEOM_WIDTH:
            field = pgs->w_field;
            break;
        case GEOM_HEIGHT:
            field = pgs->h_field;
            break;
        default:
            field = NULL;
    }

    if (field)
    {
	/* Set state so changebar is not triggered */
	XtVaSetValues(field, XmNuserData, (XtArgVal)PROP_LOAD,NULL);

	/* Convert int to string */
    	ui_field_set_string(field, NULL);

	/* Reset state */
	XtVaSetValues(field, XmNuserData, (XtArgVal)PROP_EDIT,NULL);
    }

    return OK;
}

static void
obj_options_buildCB(
    Widget      widget,
    XtPointer   client_data,
    XtPointer   call_data
)
{
    PropObjOptionsSetting pos = (PropObjOptionsSetting)client_data;

    prop_obj_options_load(pos, pos->search_root);
}

void
strings_init(
)
{
    LabelForString =
	XtNewString(catgets(Dtb_project_catd, 100, 215, "Label:"));
    LabelForGraphic =
	XtNewString(catgets(Dtb_project_catd, 100, 216, "Graphic Filename:"));
    NoneItem =
	XtNewString(catgets(Dtb_project_catd, 100, 217, "None"));
    XFieldStr =
	XtNewString(catgets(Dtb_project_catd, 100, 218, "X Field"));
    YFieldStr =
	XtNewString(catgets(Dtb_project_catd, 100, 219, "Y Field"));
    WFieldStr =
	XtNewString(catgets(Dtb_project_catd, 100, 220, "Width Field"));
    HFieldStr =
	XtNewString(catgets(Dtb_project_catd, 100, 221, "Height Field"));
    OffsetFieldStr =
	XtNewString(catgets(Dtb_project_catd, 100, 222, "Offset Field"));
    PercentageFieldStr =
	XtNewString(catgets(Dtb_project_catd, 100, 223, "Percentage Field"));
    menu_strs[0] =
	XtNewString(catgets(Dtb_project_catd, 100, 224, "None"));
    menu_strs[1] =
	XtNewString(catgets(Dtb_project_catd, 100, 225, "Create New Menu..."));
    menu_strs[2] =
	XtNewString(catgets(Dtb_project_catd, 100, 226, "Menus"));
    menu_strs[3] =
	XtNewString(catgets(Dtb_project_catd, 100, 227, "Edit Current..."));
    RowColFieldStr =
	XtNewString(catgets(Dtb_project_catd, 100, 250, "Rows/Columns Field"));
    VertSpacingFieldStr =
        XtNewString(catgets(Dtb_project_catd, 100, 251, "Vertical Spacing Field"));
    HorizSpacingFieldStr =
	XtNewString(catgets(Dtb_project_catd, 100, 252, "Horizontal Spacing Field"));
}
