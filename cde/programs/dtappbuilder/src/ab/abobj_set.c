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
 *      $XConsortium: abobj_set.c /main/5 1996/10/29 16:44:11 mustafa $
 *
 *	@(#)abobj_set.c	1.132 19 May 1995
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
 ***********************************************************************
 * abobj_set.c - implements setting AB object properties
 *
 *
 ***********************************************************************
 */
#include <stdio.h>
#if !defined(CSRG_BASED)
#include <values.h>
#endif
#include <Xm/Xm.h>
#include <Xm/List.h>
#include <Dt/SpinBox.h>
#include <Dt/ComboBox.h>
#include <ab_private/obj.h>
#include <ab_private/trav.h>
#include <ab_private/objxm.h>
#include <ab_private/ab.h>
#include <ab_private/abobj.h>
#include <ab_private/abobj_set.h>
#include <ab_private/proj.h>
#include <ab_private/prop.h>
#include <ab_private/ui_util.h>
#include "dtb_utils.h"
#include <ab_private/x_util.h>

static BOOL	save_needed_enabled = TRUE;

/*************************************************************************
**                                                                      **
**       Private Function Declarations 					**
**                                                                      **
**************************************************************************/

static void	get_size_params(
    		    ABObj       obj,
    		    ABObj       *szObjPtr,
    		    ABObj       *bdObjPtr,
    		    Dimension	*border_w_ptr
		);
static void     set_height_attrs(
                    ABObj       obj,
                    ABObj       szObj,
                    ABObj       bdObj,
                    int         border_frame_w,
                    Dimension   border_w
                );
static void	set_width_attrs(
    		    ABObj       obj,
    		    ABObj       szObj,
    		    ABObj       bdObj,
    		    int         border_frame_w,
    		    Dimension   border_w
		);

/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/

/*
 * Instantiate Changes for a Composite Object hierarchy
 */
void
abobj_instantiate_changes(
    ABObj 	obj
)
{
    BOOL	selected = FALSE;
    static BOOL	new_widgets;

    if (obj_is_selected(obj)) /* Temporarily turn-off selection */
    {
        selected = TRUE;
        abobj_deselect(obj);
    }
    /* Tell Motif to recognize changes */
    objxm_comp_instantiate_changes(obj, &new_widgets);

    /* If objects were Re-instantiated, we must
     * make sure build-actions are enabled
     */
    if (new_widgets)
    {
    	abobj_tree_set_build_actions(obj);
	objxm_tree_map(obj, TRUE);
    }

    if (selected) /* Re-select obj */
	abobj_select(obj);
}

/*
 * Instantiate changes for an Object tree starting at root
 */
void
abobj_tree_instantiate_changes(
    ABObj	root
)
{
    ABSelectedRec sel;
    static BOOL	new_widgets;
    int	        i;

    abobj_get_selected(root, TRUE, TRUE, &sel);
    for(i=0; i < sel.count; i++)
    {
	if (!obj_has_flag(sel.list[i], BeingDestroyedFlag))
	    abobj_deselect(sel.list[i]);
	else
	    sel.list[i] = NULL;
    }
    objxm_tree_instantiate_changes(root, &new_widgets);

    if (new_widgets)
    {
	abobj_tree_set_build_actions(root);
	objxm_tree_map(root, TRUE);
    }

    /* Reselect previously selected objects */
    for (i=0; i < sel.count; i++)
	if (sel.list[i] != NULL)
	    abobj_select(sel.list[i]);

}
void
abobj_set_name(
    ABObj	obj,
    STRING	name
)
{
    AB_TRAVERSAL 	trav;
    ABObj		project, module, wp_module, ui_obj;
    STRING		oldname_copy = NULL;
    STRING		oldname = obj_get_name(obj);

    project = proj_get_project();
    module = obj_get_module(obj);

    if (util_strcmp(name, oldname) != 0)
    {
	if (obj_is_module(obj))
	{
	    if (obj_find_by_name_and_type(project, name, AB_TYPE_MODULE) != NULL)
	    {
		util_dprintf(1, "abobj_set_name: %s: another module already has name \"%s\"\n",
			obj_get_name(obj), name);
	    	return;
	    }
	}
	else if (obj_find_by_name(module, name) != NULL)
	{
	    util_dprintf(1, "abobj_set_name: %s: another object already has name \"%s\"\n",
                        obj_get_name(obj), name);
	    return;
 	}

	if (oldname)
	    oldname_copy = strdup(oldname);
	obj_set_name(obj, name);
        abobj_set_save_needed(module, TRUE);

        /* If we're changing the name of a module (bil file)
	 * or we are changing the name of the application's root-window,
         * we need to set the dirty bit on the project.
         */
        if (obj_is_module(obj) ||
	    obj_get_root_window(project) == obj)
          abobj_set_save_needed(project, TRUE);

	if (obj_is_module(obj))
	{
	    /* Since the window-parent relationship between a dialog and a
	     * main-window is stored in the main-window's module (as a list of
	     * dialog names in the form "module.dialog"), we need to check all
	     * the dialogs in this module to see if any of their window-parent's
	     * modules should also be marked as dirty.
	     */
	    for (trav_open(&trav, obj, AB_TRAV_WINDOWS);
                (ui_obj= trav_next(&trav)) != NULL; )
		if (obj_is_popup_win(ui_obj))
		{
		    wp_module = obj_get_module(obj_get_win_parent(ui_obj));
		    if (wp_module != NULL && wp_module != obj)
		        abobj_set_save_needed(wp_module, TRUE);
		}
            trav_close(&trav);
	}
	else if (obj_is_popup_win(obj))
	{
	    /* If this dialog's window-parent is in a different module than
	     * this dialog, then mark that module as dirty also.
	     */
	    wp_module = obj_get_module(obj_get_win_parent(obj));
            if (wp_module != NULL && wp_module != module)
                abobj_set_save_needed(wp_module, TRUE);
	}
	else if (obj_is_menu(obj)) /* We must change menu-name field for all owners */
	{
            AB_TRAVERSAL trav;
            ABObj 	 ui_obj;

            for (trav_open(&trav, module, AB_TRAV_SALIENT);
                (ui_obj= trav_next(&trav)) != NULL; )
		if (util_strcmp(obj_get_menu_name(ui_obj), oldname_copy) == 0)
		    obj_set_menu_name(ui_obj, name);

            trav_close(&trav);
	}
	else if (obj_is_choice(obj) || obj_is_list(obj) ||
	    obj_is_menubar(obj) || obj_is_menu(obj))
        {
	    AB_TRAVERSAL trav;
	    ABObj	 iobj;

	    /* Change name-base of all items to be new obj name */
 	    for (trav_open(&trav, obj, AB_TRAV_ITEMS_FOR_OBJ);
		(iobj = trav_next(&trav)) != NULL; )
		if (obj_is_item(iobj))
		{
		    STRING	namebase = prop_item_get_namebase(iobj);

		    abobj_set_item_name(iobj, module,
				obj_get_name(obj), namebase);
		}

	    trav_close(&trav);
	}
	/* Make sure all SubObjs also get renamed...*/
	if (oldname_copy != NULL)
	{
 	    abobj_comp_rename(obj, oldname_copy, obj_get_name(obj));
	    util_free(oldname_copy);
	}
    }
}

STRING
abobj_construct_item_name(
    STRING	prefix,
    STRING	namebase,
    STRING	suffix
)
{
    STRING name;
    STRING itemname;
    STRING full_itemname;

    name = ab_ident_from_name_and_label(prefix, namebase);

    /* Check to make sure resulting name is NOT the same as "prefix"
     * which happens when namebase is composed of all non alpha-numeric
     * characters (i.e.  prefix="checkbox", namebase=" ")
     */
    if (util_strcmp(name, prefix) == 0)
        name = ab_ident_from_name_and_label(prefix, "nolabel");

    if (suffix != NULL)
    {
     	itemname = strdup(name);
	full_itemname = ab_ident_from_name_and_label(itemname, suffix);
	util_free(itemname);
    }
    else
	full_itemname = name;

    return full_itemname;
}

void
abobj_set_item_name(
    ABObj       iobj,
    ABObj       module,
    STRING      prefix,
    STRING      namebase
)
{
    STRING	itemName = NULL;

    itemName = abobj_construct_item_name(prefix, namebase, "item");
    obj_set_unique_name(iobj, itemName);
}

void
abobj_set_choice_type(
    ABObj	obj,
    AB_CHOICE_TYPE type
)
{
    AB_CHOICE_TYPE oldtype;
    ABObj 	subObj, pObj, iobj;
    Boolean 	radio;
    int		indtype;
    int     	i;

    oldtype = obj_get_choice_type(obj);

    if (type != oldtype)
    {
        /* A change has occurred in the module so set the save flag */
        abobj_set_save_needed( obj_get_module(obj), TRUE);

	if (oldtype == AB_CHOICE_OPTION_MENU ||
	       type == AB_CHOICE_OPTION_MENU)
	{
	    objxm_tree_unconfigure(obj);
            obj_set_subtype(obj, type);
            objxm_tree_configure(obj, OBJXM_CONFIG_BUILD); /* Sets all args */
            obj_tree_clear_flag(obj, InstantiatedFlag);
        }
	else  /* EXCLUSIVE || NON_EXCLUSIVE */
	{

	    subObj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);
	    pObj = objxm_comp_get_subobj(obj, AB_CFG_PARENT_OBJ);

	    obj_set_subtype(obj, type);

	    if (type == AB_CHOICE_EXCLUSIVE)
	    {
		radio = TRUE;
		indtype = XmONE_OF_MANY;
	    }
	    else
	    {
		radio = FALSE;
		indtype = XmN_OF_MANY;
	    }
	    objxm_obj_set_ui_arg(subObj, AB_ARG_BOOLEAN, XmNradioBehavior, radio);
            obj_set_flag(subObj, AttrChangedFlag);

            for (i=0; i < obj_get_num_children(pObj); i++)
            {
            	iobj = obj_get_child(pObj, i);
            	objxm_obj_set_ui_arg(iobj, AB_ARG_LITERAL, XmNindicatorType, indtype);
            	obj_set_flag(iobj, AttrChangedFlag);

	    }
	}
    }
}

void
abobj_set_menu_name(
    ABObj	obj,
    STRING	menuname
)
{
    ABObj	menu_ref = NULL;
    ABObj	subObj;
    STRING	old_menuname = obj_get_menu_name(obj);
    STRING	new_menuname = NULL;

    new_menuname = (util_strempty(menuname)? NULL : menuname);

    if (util_strcmp(old_menuname, new_menuname) != 0)
    {
        /* A change has occurred in the module so set the save flag */
        abobj_set_save_needed( obj_get_module(obj), TRUE);

	obj_set_menu_name(obj, new_menuname);

	subObj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);

	/* If Menu being deleted or changed, destroy old Menu-reference */
	if (old_menuname != NULL)
	{
	    menu_ref = objxm_comp_get_subobj(obj, AB_CFG_MENU_OBJ);
	    if (obj_is_ref(menu_ref))
	    {
	        obj_destroy(menu_ref);
	    }
	}
	else if (obj_is_menu_item(obj))
	    /* Menu-items w/submenus need to be CascadeButtons */
	    obj_set_class_name(subObj, _xmCascadeButton);

	if (new_menuname != NULL)
	{
	    /* Menu-Refs ALWAYS parented off RootObj */
	    /* Create new Menu-reference */
	    if (objxm_comp_config_menu_ref(obj) == NULL)
		if (util_get_verbosity() > 2)
	    	    fprintf(stderr,"abobj_set_menu_name: couldn't find menu obj: %s\n", new_menuname);
	}
	else if (obj_is_menu_item(obj))
	    /* Menu-items without submenus need to be PushButtons */
	    obj_set_class_name(subObj, _xmPushButton);
    }
}

void
abobj_set_menu_title(
    ABObj	obj,
    STRING	menu_title
)
{
    if (util_strcmp(menu_title, obj_get_menu_title(obj)) != 0)
    {
        abobj_set_save_needed(obj_get_module(obj), TRUE);

        obj_set_menu_title(obj, menu_title);

	/* Since Menus arn't instantiated until Test-mode,
	 * there is no need to do anything else here
	 */
    }
}

void
abobj_set_accelerator(
    ABObj	obj,
    STRING	accel
)
{
    ABObj       subObj;
    STRING      new_accel = NULL;

    new_accel = (util_strempty(accel)? NULL : accel);

    if (util_strcmp(obj_get_accelerator(obj), new_accel) != 0)
    {
        abobj_set_save_needed( obj_get_module(obj), TRUE);

        obj_set_accelerator(obj, new_accel);
        subObj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);

        objxm_obj_set_ui_arg(subObj, AB_ARG_STRING, XmNaccelerator, new_accel);
        /* REMIND: acceleratorText should be converted to different format */
        objxm_obj_set_ui_arg(subObj, AB_ARG_XMSTRING, XmNacceleratorText,
                new_accel? (XtArgVal)XmStringCreateLocalized(accel) : (XtArgVal)NULL);
	obj_set_flag(subObj, AttrChangedFlag);
    }
}
void
abobj_set_mnemonic(
    ABObj	obj,
    STRING	mnemonic
)
{
    ABObj	subObj;
    STRING	new_mnemonic = NULL;

    new_mnemonic = (util_strempty(mnemonic)? NULL : mnemonic);

    if (util_strcmp(obj_get_mnemonic(obj), new_mnemonic) != 0)
    {
        abobj_set_save_needed( obj_get_module(obj), TRUE);

	obj_set_mnemonic(obj, new_mnemonic);
	subObj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);

	objxm_obj_set_ui_arg(subObj, AB_ARG_MNEMONIC, XmNmnemonic,
		new_mnemonic? (XtArgVal)XStringToKeysym(new_mnemonic) : (XtArgVal)NULL);
	obj_set_flag(subObj, AttrChangedFlag);
    }
}

void
abobj_set_group_type(
    ABObj	obj,
    AB_GROUP_TYPE type
)
{
    if (obj_get_group_type(obj) != type)
    {
        abobj_set_save_needed( obj_get_module(obj), TRUE);
        obj_set_group_type(obj, type);
	obj_set_flag(obj_get_module(obj), SaveNeededFlag);
    }
}


void
abobj_set_row_alignment(
    ABObj	 obj,
    AB_ALIGNMENT align
)
{
    if (obj_get_row_align(obj) != align)
    {
        abobj_set_save_needed( obj_get_module(obj), TRUE);
        obj_set_row_align(obj, align);
	obj_set_flag(obj_get_module(obj), SaveNeededFlag);
    }
}

void
abobj_set_col_alignment(
    ABObj	 obj,
    AB_ALIGNMENT align
)
{
    if (obj_get_col_align(obj) != align)
    {
        abobj_set_save_needed( obj_get_module(obj), TRUE);
        obj_set_col_align(obj, align);
	obj_set_flag(obj_get_module(obj), SaveNeededFlag);
    }
}

void
abobj_set_row_attach_type(
    ABObj	   obj,
    AB_ATTACH_TYPE type
)
{
    if (obj_get_hattach_type(obj) != type)
    {
        abobj_set_save_needed( obj_get_module(obj), TRUE);
        obj_set_hattach_type(obj, type);
	obj_set_flag(obj_get_module(obj), SaveNeededFlag);
    }
}


void
abobj_set_col_attach_type(
    ABObj	   obj,
    AB_ATTACH_TYPE type
)
{
    if (obj_get_vattach_type(obj) != type)
    {
        abobj_set_save_needed( obj_get_module(obj), TRUE);
        obj_set_vattach_type(obj, type);
	obj_set_flag(obj_get_module(obj), SaveNeededFlag);
    }
}


void
abobj_set_row_offset(
    ABObj	obj,
    int		offset
)
{
    if (obj_get_hoffset(obj) != offset)
    {
        abobj_set_save_needed( obj_get_module(obj), TRUE);
        obj_set_hoffset(obj, offset);
	objxm_obj_set_attachment_args(obj, OBJXM_CONFIG_BUILD);
	obj_set_flag(obj_get_module(obj), SaveNeededFlag);
    }
}

void
abobj_set_col_offset(
    ABObj	obj,
    int		offset
)
{
    if (obj_get_voffset(obj) != offset)
    {
        abobj_set_save_needed( obj_get_module(obj), TRUE);
        obj_set_voffset(obj, offset);
	objxm_obj_set_attachment_args(obj, OBJXM_CONFIG_BUILD);
	obj_set_flag(obj_get_module(obj), SaveNeededFlag);
    }
}

void
abobj_set_button_type(
    ABObj	obj,
    AB_BUTTON_TYPE type
)
{
    AB_BUTTON_TYPE old_type;
    ABObj	   subObj;

    old_type = obj_get_button_type(obj);
    subObj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);

    if (old_type != type)
    {
        abobj_set_save_needed( obj_get_module(obj), TRUE);
	obj_set_subtype(obj, type);

        if (old_type == AB_BUT_MENU) /* Clear Menu field */
	    abobj_set_menu_name(obj, NULL);

	switch (type)
	{
	    case AB_BUT_PUSH:
		obj_set_class_name(subObj, _xmPushButton);
		break;

	    case AB_BUT_DRAWN:
                obj_set_class_name(subObj, _xmDrawnButton);
                break;

	    case AB_BUT_MENU:
		obj_set_class_name(subObj, _dtMenuButton);
		break;
	}
	obj_clear_flag(subObj, InstantiatedFlag);

    }
}

void
abobj_set_label_alignment(
    ABObj	obj,
    AB_ALIGNMENT align
)
{

    if (obj_get_label_alignment(obj) != align)
    {
        obj_set_label_alignment(obj, align);
        objxm_comp_set_lbl_align_args(obj, OBJXM_CONFIG_BUILD);
	obj_set_flag(obj_get_module(obj), SaveNeededFlag);
    }
}

void
abobj_set_label_position(
    ABObj	obj,
    AB_COMPASS_POINT pos
)
{
    if (obj_get_label_position(obj) != pos)
    {
        obj_set_label_position(obj, pos);
        objxm_comp_set_lbl_pos_args(obj, OBJXM_CONFIG_BUILD);
	obj_set_flag(obj_get_module(obj), SaveNeededFlag);
    }
}

void
abobj_set_label(
    ABObj	  obj,
    AB_LABEL_TYPE ltype,
    STRING	  label
)
{
    ABObj	  lblObj  = NULL;  /* Label object */
    STRING	  old_label = XtNewString(obj_get_label(obj));
    AB_LABEL_TYPE old_ltype  = obj->label_type;
    BOOL	  type_change = FALSE;

    /* Label-setting code depends on label_type being set correctly
     * so set it first if changed.
     */
    if (ltype != old_ltype)
    {
	obj->label_type = ltype;
	type_change = TRUE;
    }

    if (util_strcmp(label, old_label) != 0 || type_change)
    {
	/* A change has occurred in the module so set the save flag */
        abobj_set_save_needed( obj_get_module(obj), TRUE);

	/* List, ComboBox & SpinBox Items are not actually widgets
	 * so we must use the widget's API for configuring items.
	 */
	if (obj_is_list_item(obj) ||
	    obj_is_combo_box_item(obj) ||
	    obj_is_spin_box_item(obj))
	{
	    ABObj		p_obj = obj_get_parent(obj);
	    Widget		parent = objxm_get_widget(p_obj);
	    AB_ITEM_TYPE 	itype = (AB_ITEM_TYPE)obj_get_subtype(obj);
	    int			pos;
	    int			num_items;
	    XmString		xmitem;

	    obj_set_label(obj, label? label : "");

	    if (parent != NULL)
	    {
		xmitem = XmStringCreateLocalized(obj_get_label(obj));
	    	pos = obj_get_child_num(obj);
		pos++; /* XmList starts at 1 */

		if (obj_is_combo_box_item(obj))
		    parent = ui_combobox_get_list_widget(parent);

		if (obj_is_list_item(obj) || obj_is_combo_box_item(obj))
		    XtVaGetValues(parent, XmNitemCount, &num_items, NULL);
		else if (obj_is_spin_box_item(obj))
		    XtVaGetValues(parent, DtNnumValues, &num_items, NULL);

		if (pos <= num_items)
		{
		    if (obj_is_list_item(obj) || obj_is_combo_box_item(obj))
		    	XmListReplacePositions(parent, &pos, &xmitem, 1);
		    else
		    {
			DtSpinBoxDeletePos(parent, pos);
			DtSpinBoxAddItem(parent, xmitem, pos);
		    }
		}
		XmStringFree(xmitem);
	    }
	}
	/* If a Label is being Added or Removed from a Menu,List,Choice or
	 * ComboBox, SpinBox, Scale or TextField, ReConfigure the Obj
	 */
	else if ((obj_is_menu(obj) || obj_is_list(obj) ||
		 obj_is_choice(obj) || obj_is_text_field(obj) ||
		 obj_is_combo_box(obj) || obj_is_spin_box(obj) ||
		 obj_is_scale(obj)) &&
	         (util_strempty(old_label) || util_strempty(label)))
	{
            objxm_obj_unconfigure(obj);
            obj_set_label(obj, label);
            objxm_obj_configure(obj, OBJXM_CONFIG_BUILD, TRUE); /* Takes care of setting label args */
	    obj_tree_clear_flag(obj, InstantiatedFlag);
        }
	else
	{
	    obj_set_label(obj, label? label : "");
	    objxm_comp_set_label_args(obj, OBJXM_CONFIG_BUILD);

	    /* If label-type changes for buttons, might need to Re-instantiate with
	     * new class
	     */
	    if (obj_is_button(obj) && type_change)
	    {
		lblObj = objxm_comp_get_subobj(obj, AB_CFG_LABEL_OBJ);

        	if ((ltype == AB_LABEL_STRING || ltype == AB_LABEL_GLYPH) &&
                     ab_label_is_arrow(old_ltype))
                {
                    obj_set_class_name(lblObj, obj_get_subtype(obj) == AB_BUT_DRAWN?
					_xmDrawnButton : _xmPushButton);
                    obj_clear_flag(lblObj, InstantiatedFlag);
               	}
        	else if (ab_label_is_arrow(ltype) && !ab_label_is_arrow(old_ltype))
        	{
                    obj_set_class_name(lblObj, _xmArrowButton);
                    obj_clear_flag(lblObj, InstantiatedFlag);
                }
            }
	    if (obj_is_menu_item(obj) && type_change)
	    {
		if (old_ltype == AB_LABEL_SEPARATOR)
		{
		    lblObj = objxm_comp_get_subobj(obj, AB_CFG_LABEL_OBJ);
		    obj_set_class_name(lblObj, _xmCascadeButton);
		}
	    }
	}
    }
    XtFree(old_label);
}

void
abobj_set_border_frame(
    ABObj	obj,
    AB_LINE_TYPE btype
)
{
    AB_LINE_TYPE old_btype = obj_get_border_frame(obj);

    if (old_btype != btype)
    {
	/* A change has occurred in the module so set the save flag */
        abobj_set_save_needed( obj_get_module(obj), TRUE);

	if  (old_btype == AB_LINE_NONE ||
	    btype == AB_LINE_NONE) /* Adding or Removing border */
	{
            objxm_obj_unconfigure(obj);
            obj_set_border_frame(obj, btype);
            objxm_obj_configure(obj, OBJXM_CONFIG_BUILD, TRUE); /* Takes care of setting border args */
            obj_tree_clear_flag(obj, InstantiatedFlag);
	}
	else /* Changing Border style */
	{
	    obj_set_border_frame(obj, btype);
	    objxm_comp_set_border_args(obj, OBJXM_CONFIG_BUILD);
	}
    }
}

void
abobj_set_line_style(
    ABObj	obj,
    AB_LINE_TYPE type
)
{
    ABObj	subObj;

    if (obj_get_line_style(obj) != type)
    {
	/* A change has occurred in the module so set the save flag */
        abobj_set_save_needed( obj_get_module(obj), TRUE);

        obj_set_line_style(obj, type);

	subObj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);
        objxm_obj_set_line_style_arg(subObj, OBJXM_CONFIG_BUILD);
    }
}


void
abobj_set_arrow_style(
    ABObj	obj,
    AB_ARROW_STYLE astyle
)
{
    ABObj	subObj;

    if (obj_get_arrow_style(obj) != astyle)
    {
	/* A change has occurred in the module so set the save flag */
        abobj_set_save_needed( obj_get_module(obj), TRUE);

        obj_set_arrow_style(obj, astyle);

	subObj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);
        objxm_obj_set_arrow_style_arg(subObj, OBJXM_CONFIG_BUILD);
    }
}

void
abobj_set_icon(
    ABObj       obj,
    STRING	icon,
    STRING	icon_mask,
    STRING      icon_label
)
{
    BOOL	chg = False;

    if (util_strcmp(icon_label, obj_get_icon_label(obj)) != 0)
    {
        abobj_set_save_needed(obj_get_module(obj), TRUE);
	chg = True;

	obj_set_icon_label(obj, icon_label);
    }
    if (util_strcmp(icon_mask, obj_get_icon_mask(obj)) != 0)
    {
        abobj_set_save_needed(obj_get_module(obj), TRUE);
        chg = True;

        obj_set_icon_mask(obj, util_strempty(icon_mask)? NULL : icon_mask);
    }
    if (util_strcmp(icon, obj_get_icon(obj)) != 0)
    {
        abobj_set_save_needed(obj_get_module(obj), TRUE);
	chg = True;

        obj_set_icon(obj, util_strempty(icon)? NULL : icon);
    }

    if (chg)
	objxm_comp_set_icon_args(obj, OBJXM_CONFIG_BUILD);

}


/*
 * This routine is used in exactly one place, in pal_group.c. It shouldn't
 * be used at all, but we will handle the one special case, since we
 * can't change pal_group.c, due to CRT.
 */
void
abobj_set_initial_state(
    ABObj	obj,
    AB_OBJECT_STATE	istate
)
{
#ifdef DEBUG
    util_dprintf(1, "OBSOLETE ROUTINE abobj_set_initial_state() called!\n");
    util_dprintf(1, "    (This works, for now, but it should be removed!)\n");
#endif /* DEBUG */

    if (istate == AB_STATE_ACTIVE)
    {
	obj_set_is_initially_active(obj, TRUE);
    }
}

void
abobj_set_initial_value(
    ABObj	obj,
    STRING	strval,
    int		intval
)
{
    ABObj       subObj;  /* object */
    STRING	old_strval;
    STRING	new_strval = NULL;

    new_strval = (util_strempty(strval)? NULL : strval);

    if (obj_is_text(obj))
    {
    	old_strval = obj_get_initial_value_string(obj);

    	if (util_strcmp(old_strval, new_strval) != 0)
    	{
	    /* A change has occurred in the module so set the save flag */
	    abobj_set_save_needed( obj_get_module(obj), TRUE);
	    obj_set_initial_value_string(obj, new_strval);

       	    subObj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);
            objxm_obj_set_ui_arg(subObj, AB_ARG_STRING, XmNvalue,
		obj_get_initial_value_string(obj));
            obj_set_flag(subObj, AttrChangedFlag);
	}
    }
    else if (obj_is_spin_box(obj) || obj_is_scale(obj))
    {
	if (obj_get_initial_value_int(obj) != intval)
	{
            abobj_set_save_needed( obj_get_module(obj), TRUE);
	    obj_set_initial_value_int(obj, intval);

            subObj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);
	    if (obj_is_spin_box(obj))
                objxm_obj_set_ui_arg(subObj, AB_ARG_INT, DtNposition, intval);
	    else
		objxm_obj_set_ui_arg(subObj, AB_ARG_INT, XmNvalue, intval);
            obj_set_flag(subObj, AttrChangedFlag);
        }
    }
}

void
abobj_set_orientation(
    ABObj	obj,
    AB_ORIENTATION orient
)
{
    ABObj 	subObj;
    ABObj	lblObj;

    if (obj_get_orientation(obj) != orient)
    {
        /* A change has occurred in the module so set the save flag */
        abobj_set_save_needed( obj_get_module(obj), TRUE);

	obj_set_orientation(obj, orient);

        subObj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);
	objxm_obj_set_orientation_arg(subObj, OBJXM_CONFIG_BUILD);

	if (obj_is_choice(obj))
	{
	    lblObj = objxm_comp_get_subobj(obj, AB_CFG_LABEL_OBJ);

	    if (lblObj && orient == AB_ORIENT_VERTICAL)
	    {
                objxm_obj_set_ui_arg(subObj, AB_ARG_LITERAL, XmNentryVerticalAlignment,
                                                XmALIGNMENT_BASELINE_BOTTOM);
                objxm_obj_set_ui_arg(lblObj, AB_ARG_INT, XmNmarginHeight, 8);
		obj_set_flag(lblObj, AttrChangedFlag);
	    }
	}
	else if (obj_is_separator(obj) || obj_is_scale(obj))
 	{
            XRectangle  rect;
            int     	old_x, old_y, new_x, new_y, new_width, new_height;
	    Widget  	widget = objxm_get_widget(subObj);

	    if (widget != NULL)
	    {
            	x_get_widget_rect(widget, &rect);

            	new_height = (int)rect.width;
            	new_width = (int)rect.height;
            	new_x = old_x = (int)rect.x;
            	new_y = old_y = (int)rect.y;

            	if (orient == AB_ORIENT_VERTICAL)
            	{
            	    new_x += (int)rect.width/2;
            	    new_y -= (int)rect.width/2;
            	}
            	else /* AB_ORIENT_HORIZONTAL */
            	{
            	    new_x -= (int)rect.height/2;
            	    new_y += (int)rect.height/2;
            	}
	    	abobj_set_pixel_width(obj, (int)new_width, 0);
	    	abobj_set_pixel_height(obj, (int)new_height, 0);
            	abobj_set_xy(obj, new_x, new_y);

	    	if (obj_is_scale(obj))
            	    obj_tree_clear_flag(obj, InstantiatedFlag);
	    }
	}
    }
}

void
abobj_set_num_columns(
    ABObj	obj,
    int		num_cols
)
{
    ABObj       subObj;  /* object */
    ABObj	pObj;
    ABObj	lblObj;
    int		old_num_cols;

    old_num_cols = obj_get_num_columns(obj);

    if (num_cols != old_num_cols)
    {
        /* A change has occurred in the module so set the save flag */
        abobj_set_save_needed( obj_get_module(obj), TRUE);

	subObj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);

        obj_set_num_columns(obj, num_cols);

	if (num_cols != -1 &&
	    (obj_is_text(obj) || obj_is_term_pane(obj)))
	{
	    objxm_obj_set_ui_arg(subObj, AB_ARG_INT, XmNcolumns, num_cols);
/*
            obj_clear_flag(obj, InstantiatedFlag);
*/
            obj_clear_flag(subObj, InstantiatedFlag);
	}
	else if (obj_is_choice(obj))
        {
            pObj = objxm_comp_get_subobj(obj, AB_CFG_PARENT_OBJ);

	    objxm_obj_set_ui_arg(pObj, AB_ARG_INT, XmNnumColumns, num_cols);
            obj_set_flag(pObj, AttrChangedFlag);

	    if (num_cols > 1)
		objxm_obj_set_literal_ui_arg(subObj, OBJXM_CONFIG_BUILD, XmNpacking, XmPACK_COLUMN);
	    else
		objxm_obj_set_literal_ui_arg(subObj, OBJXM_CONFIG_BUILD, XmNpacking, XmPACK_TIGHT);
	    obj_set_flag(subObj, AttrChangedFlag);

	    lblObj = objxm_comp_get_subobj(obj, AB_CFG_LABEL_OBJ);
	    if (lblObj && num_cols > 1)
	    {
                objxm_obj_set_ui_arg(subObj, AB_ARG_LITERAL, XmNentryVerticalAlignment,
                                                XmALIGNMENT_BASELINE_BOTTOM);
                objxm_obj_set_ui_arg(lblObj, AB_ARG_INT, XmNmarginHeight, 8);
                obj_set_flag(lblObj, AttrChangedFlag);
            }
       	}
    }
}

void
abobj_set_num_rows(
    ABObj	obj,
    int		num_rows
)
{
    ABObj       subObj;  /* object */
    int         old_num_rows;

    old_num_rows = obj_get_num_rows(obj);

    if (num_rows != old_num_rows)
    {
        /* A change has occurred in the module so set the save flag */
        abobj_set_save_needed( obj_get_module(obj), TRUE);

        subObj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);
        obj_set_num_rows(obj, num_rows);

	if (num_rows != -1)
    	{
	    if (obj_is_list(obj))
            {
            	objxm_obj_set_ui_arg(subObj, AB_ARG_INT, XmNvisibleItemCount, num_rows);
            	obj_set_flag(subObj, AttrChangedFlag);
	    }
	    else if (obj_is_text_pane(obj) || obj_is_term_pane(obj))
	    {
	    	objxm_obj_set_ui_arg(subObj, AB_ARG_INT, XmNrows, num_rows);
	    	obj_clear_flag(subObj, InstantiatedFlag);
	    }
	}
    }
}

void
abobj_set_scrollbar_state(
    ABObj		obj,
    AB_SCROLLBAR_POLICY	scrolling
)
{
    int 		value;
    BOOL		scroll_flag = False;
    BOOL		changed = False;
    AB_SCROLLBAR_POLICY	hsb_policy = AB_SCROLLBAR_UNDEF,
			vsb_policy = AB_SCROLLBAR_UNDEF;

/* REMIND: (mcv) This routine does not allow setting the
 * horizontal and vertical scrollbar values separately.
 * This needs to be fixed.  The prop sheets don't yet allow
 * setting separate values for horizontal and vertical scrollbars
 * either.
 */
    /* TextPane only allows 2 scrollbar policies: Always,Never */
    if (scrolling != obj_get_vscrollbar_policy(obj))
    {
	changed = True;
        /* A change has occurred in the module so set the save flag */
        abobj_set_save_needed( obj_get_module(obj), TRUE);

	/* Scrolling is being turned off */
	objxm_obj_unconfigure(obj);
	obj_set_vscrollbar_policy(obj, scrolling);
	scroll_flag = (scrolling == AB_SCROLLBAR_ALWAYS ||
			scrolling == AB_SCROLLBAR_WHEN_NEEDED );

	/* If text wordwrap is ON, then NO Horizontal scrollbar is necessary */
        if (obj_is_text_pane(obj) && scroll_flag && obj->info.text.word_wrap)
            obj_set_hscrollbar_policy(obj, AB_SCROLLBAR_NEVER);

        else if (!obj_is_term_pane(obj))
            obj_set_hscrollbar_policy(obj, scrolling);

	objxm_obj_configure(obj, OBJXM_CONFIG_BUILD, TRUE);
	obj_tree_clear_flag(obj, InstantiatedFlag);
    }

    /* DrawingArea allows all 3 types of scrollbar display policies */
    if (obj_is_drawing_area(obj) && changed)
    {
	ABObj swObj = objxm_comp_get_subobj(obj, AB_CFG_POSITION_OBJ);
	hsb_policy = obj_get_hscrollbar_policy(obj);
	vsb_policy = obj_get_vscrollbar_policy(obj);

        /* A change has occurred in the module so set the save flag */
        abobj_set_save_needed( obj_get_module(obj), TRUE);

	if (hsb_policy == AB_SCROLLBAR_ALWAYS ||
		vsb_policy == AB_SCROLLBAR_ALWAYS)
	    value = XmSTATIC;
	else
	    value = XmAS_NEEDED;

	objxm_obj_set_ui_arg(swObj, AB_ARG_LITERAL, XmNscrollBarDisplayPolicy, value);
	obj_set_flag(swObj, AttrChangedFlag);
    }
}

void
abobj_set_show_value(
    ABObj	obj,
    BOOL	show_val
)
{
    ABObj	subObj;

    if (obj_is_scale(obj) &&
	obj_get_show_value(obj) != show_val)
    {
	obj_set_show_value(obj, show_val);

	subObj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);
	objxm_obj_set_ui_arg(subObj, AB_ARG_BOOLEAN, XmNshowValue, show_val);
	obj_set_flag(subObj, AttrChangedFlag);
    }
}

void
abobj_set_size_policy(
    ABObj	obj,
    BOOL	fixed_size
)
{
    BOOL	selected = False;
    ABObj	subObj;
    int		init_width, init_height;

    if (obj_get_width(obj) != -1 && !fixed_size ||
	 obj_get_width(obj) == -1 && fixed_size)
    {
        abobj_set_save_needed(obj_get_module(obj), TRUE);

	subObj = objxm_comp_get_subobj(obj, AB_CFG_SIZE_OBJ);

	if (!fixed_size) /* size NOT set by user */
	{
	    /* Setting size to not be "fixed", causes object
	     * select grab-handles to be removed (so the object cannot be
	     * resized by direct manipulation).  It is necessary to
 	     * remove the grab-handle event handling HERE (before size
	     * gets set to -1), else grab-handles are never removed.
	     */
    	    if (obj_is_selected(obj)) /* Temporarily turn-off selection */
    	    {
        	selected = True;
        	abobj_deselect(obj);
    	    }
	    obj_set_width(obj, -1);

	    if (!obj_is_list(obj) && !obj_is_combo_box(obj))
	    	obj_set_height(obj, -1);

	    if (selected)
		abobj_select(obj);
	}
	else /* fixed_size - size directly set by user */
	{
	    init_width = abobj_get_comp_width(obj);
	    init_height = abobj_get_comp_height(obj);

	    obj_set_width(obj, init_width);
	    if (!obj_is_list(obj) && !obj_is_combo_box(obj))
	    	obj_set_height(obj, init_height);
	}

	switch(obj_get_type(obj))
	{
	    case AB_TYPE_BASE_WINDOW:
	    case AB_TYPE_DIALOG:
		objxm_obj_set_literal_ui_arg(subObj, OBJXM_CONFIG_BUILD,
			XmNresizePolicy, fixed_size? XmRESIZE_GROW : XmRESIZE_ANY);
		/* fall through..*/
	    case AB_TYPE_CONTAINER:
		/* If being changed to shrink-to-fit, must re-instantiate in
	  	 * order for it to size around it's contents
		 */
		if (!fixed_size)
		    obj_clear_flag(obj, InstantiatedFlag);
		break;
	    case AB_TYPE_LABEL:
	    case AB_TYPE_BUTTON:
            	objxm_obj_set_ui_arg(subObj, AB_ARG_BOOLEAN,
                        XmNrecomputeSize, !fixed_size);
		break;
	    case AB_TYPE_LIST:
		/* XmNlistSizePolicy is Create-time-only resource */
		objxm_obj_set_literal_ui_arg(subObj, OBJXM_CONFIG_BUILD,
			XmNlistSizePolicy, fixed_size? XmCONSTANT : XmVARIABLE);
		obj_tree_clear_flag(subObj, InstantiatedFlag);
		break;
	    case AB_TYPE_COMBO_BOX:
		/* Combobox must be re-instantiated to force new sizing */
		if (!fixed_size)
		    obj_tree_clear_flag(subObj, InstantiatedFlag);
		break;
	    default:
		break;
	}
        obj_set_flag(subObj, AttrChangedFlag);
    }
}

void
abobj_set_max_length(
    ABObj       obj,
    int		len
)
{
    ABObj       subObj;  /* Text object */

    if (obj_is_text(obj) &&
	len != obj_get_max_length(obj))
    {
        /* A change has occurred in the module so set the save flag */
        abobj_set_save_needed( obj_get_module(obj), TRUE);

        subObj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);
	obj_set_max_length(obj, len);

        objxm_obj_set_ui_arg(subObj, AB_ARG_INT, XmNmaxLength, len);
        obj_set_flag(subObj, AttrChangedFlag);

    }

}

void
abobj_set_background_color(
    ABObj	obj,
    STRING	colorname
)
{
    STRING 	oldcolor = obj_get_bg_color(obj);
    STRING 	newcolor = NULL;

    newcolor = (util_strempty(colorname)? NULL : colorname);

    if (util_strcmp(oldcolor, newcolor) != 0)
    {
        /* A change has occurred in the module so set the save flag */
        abobj_set_save_needed( obj_get_module(obj), TRUE);

        obj_set_bg_color(obj, newcolor);
	objxm_comp_set_color_args(obj, OBJXM_CONFIG_BUILD, ObjxmBackground);

	/* If Obj has Item children, propogage color change */
	if (obj_is_menubar(obj) || obj_is_choice(obj) ||
	    obj_is_menu(obj) )
	{
	    AB_TRAVERSAL trav;
	    ABObj item;

	    for (trav_open(&trav, obj, AB_TRAV_ITEMS_FOR_OBJ);
                (item= trav_next(&trav)) != NULL; )
		abobj_set_background_color(item, newcolor);
	    trav_close(&trav);
        }
    }
}

void
abobj_set_foreground_color(
    ABObj       obj,
    STRING      colorname
)
{
    STRING oldcolor = obj_get_fg_color(obj);
    STRING newcolor = NULL;

    newcolor = (util_strempty(colorname)? NULL : colorname);

    if (util_strcmp(oldcolor, newcolor) != 0)
    {
        /* A change has occurred in the module so set the save flag */
        abobj_set_save_needed( obj_get_module(obj), TRUE);

        obj_set_fg_color(obj, newcolor);
	objxm_comp_set_color_args(obj, OBJXM_CONFIG_BUILD, ObjxmForeground);

        /* If Obj has Item children, propogage color change */
        if (obj_is_menubar(obj) || obj_is_choice(obj) ||
	    obj_is_menu(obj))
        {
            AB_TRAVERSAL trav;
            ABObj item;

            for (trav_open(&trav, obj, AB_TRAV_ITEMS_FOR_OBJ);
                (item= trav_next(&trav)) != NULL; )
                abobj_set_foreground_color(item, newcolor);
            trav_close(&trav);
        }
    }
}

void
abobj_set_read_only(
    ABObj	obj,
    BOOL	readonly
)
{

    if (obj_get_read_only(obj) != readonly)
    {
        /* A change has occurred in the module so set the save flag */
        abobj_set_save_needed( obj_get_module(obj), TRUE);

	obj_set_read_only(obj, readonly);
	objxm_comp_set_read_only_args(obj, OBJXM_CONFIG_BUILD);

	/* ComboBox needs to be re-instantiated because the Build
	 * actions have to be configured on the inner TextField
	 */
	if (obj_is_combo_box(obj) || obj_is_scale(obj))
	    obj_tree_clear_flag(obj, InstantiatedFlag);

	if (obj_is_combo_box(obj))
	{
	    if (readonly)
		obj_set_width(obj, -1); /* Not resizable */
	    else
		obj_set_width(obj, abobj_get_comp_width(obj));
	}
    }
}

void
abobj_set_resize_mode(
    ABObj	obj,
    BOOL	resizable
)
{
    if (obj_get_resizable(obj) != resizable)
    {
        abobj_set_save_needed(obj_get_module(obj), TRUE);
        obj_set_resizable(obj, resizable);
    }
}

static void
get_size_params(
    ABObj	obj,
    ABObj	*szObjPtr,
    ABObj	*bdObjPtr,
    Dimension	*border_w_ptr
)
{
    Widget	widget;

    *szObjPtr = objxm_comp_get_subobj(obj, AB_CFG_SIZE_OBJ);
    *bdObjPtr = objxm_comp_get_subobj(obj, AB_CFG_BORDER_OBJ);
    widget = objxm_get_widget(*szObjPtr);

    /* If widget has an Xt border, account for it */
    if (*szObjPtr != NULL && widget != NULL)
    	XtVaGetValues(widget,
                XmNborderWidth, border_w_ptr,
                NULL);
}

static void
set_width_attrs(
    ABObj	obj,
    ABObj	szObj,
    ABObj	bdObj,
    int		border_frame_w,
    Dimension	border_w
)
{
    int width = obj_get_width(obj);

    if (width != -1)
    {
        objxm_obj_set_ui_arg(szObj, AB_ARG_INT, XmNwidth,
            width - (2*border_w) - (2*border_frame_w));
        obj_set_flag(szObj, AttrChangedFlag);

        if (bdObj)
        {
            objxm_obj_set_ui_arg(bdObj, AB_ARG_INT, XmNwidth, width);
            obj_set_flag(bdObj, AttrChangedFlag);
        }
    }
}

void
abobj_set_pixel_width(
    ABObj       obj,
    int         width,
    int 	border_frame_w
)
{
    ABObj       szObj;  /* Size object */
    ABObj       bdObj = NULL;
    Dimension   border_w;

    if (obj_get_width(obj) != width)
    {
        /* A change has occurred in the module so set the save flag */
        abobj_set_save_needed( obj_get_module(obj), TRUE);

	obj_resize(obj, width, obj_get_height(obj));
	get_size_params(obj, &szObj, &bdObj, &border_w);
	set_width_attrs(obj, szObj, bdObj, border_frame_w, border_w);
    }
}

static void
set_height_attrs(
    ABObj	obj,
    ABObj	szObj,
    ABObj	bdObj,
    int		border_frame_w,
    Dimension	border_w
)
{
    int height = obj_get_height(obj);

    if (height != -1)
    {
	objxm_obj_set_ui_arg(szObj, AB_ARG_INT, XmNheight,
	        height - (2*border_w) - (2*border_frame_w));
	obj_set_flag(szObj, AttrChangedFlag);

	if (bdObj)
	{
	    objxm_obj_set_ui_arg(bdObj, AB_ARG_INT, XmNheight, height);
	    obj_set_flag(bdObj, AttrChangedFlag);
	}
	if (obj_is_control_panel(obj) &&
	   (obj_get_container_type(obj) == AB_CONT_BUTTON_PANEL ||
	    obj_get_container_type(obj) == AB_CONT_FOOTER))
	{
	    objxm_obj_set_ui_arg(bdObj? bdObj : szObj,
	        AB_ARG_INT, XmNpaneMinimum, height);
	    objxm_obj_set_ui_arg(bdObj? bdObj : szObj,
	        AB_ARG_INT, XmNpaneMaximum, height);
	}
    }
}

void
abobj_set_pixel_height(
    ABObj       obj,
    int         height,
    int         border_frame_w
)
{
    ABObj       szObj;  /* Size object */
    ABObj       bdObj = NULL;
    Dimension   border_w;

    if (obj_get_height(obj) != height)
    {
        /* A change has occurred in the module so set the save flag */
        abobj_set_save_needed( obj_get_module(obj), TRUE);

	obj_resize(obj, obj_get_width(obj), height);
	get_size_params(obj, &szObj, &bdObj, &border_w);
	set_height_attrs(obj, szObj, bdObj, border_frame_w, border_w);
    }
}

void
abobj_set_pixel_size(
    ABObj       obj,
    int         width,
    int		height,
    int         border_frame_w
)
{
    ABObj       szObj;  /* Size object */
    ABObj       bdObj = NULL;
    Dimension   border_w;

    if (obj_get_width(obj) != width || obj_get_height(obj) != height)
    {
        abobj_set_save_needed(obj_get_module(obj), TRUE);

        obj_resize(obj, width, height);
	get_size_params(obj, &szObj, &bdObj, &border_w);
        set_width_attrs(obj, szObj, bdObj, border_frame_w, border_w);
        set_height_attrs(obj, szObj, bdObj, border_frame_w, border_w);
    }
}

void
abobj_set_text_size(
    ABObj       obj,
    int		width,
    int		height
)
{
    ABObj       tObj;
    int         num_cols, num_rows;

    tObj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);

    if (obj_get_textpane_width(obj) != width)
    {
	obj_set_textpane_width(obj, width);
        objxm_obj_set_ui_arg(obj, AB_ARG_INT, XmNwidth, width);

	obj_set_textpane_width(tObj, width);
        objxm_obj_set_ui_arg(tObj, AB_ARG_INT, XmNwidth, width);

        obj_set_flag(obj, AttrChangedFlag);
    }

    if (obj_get_textpane_height(obj) != height)
    {
	obj_set_textpane_height(obj, height);
        objxm_obj_set_ui_arg(obj, AB_ARG_INT, XmNheight, height);

	obj_set_textpane_height(tObj, height);
        objxm_obj_set_ui_arg(tObj, AB_ARG_INT, XmNheight, height);
        obj_set_flag(obj, AttrChangedFlag);
    }

    ui_size_to_row_col(objxm_get_widget(tObj), width,
                       height, &num_rows, &num_cols);

    if (!obj_is_list(obj))
    {
        abobj_set_num_columns(tObj, num_cols);
        abobj_set_num_columns(obj, num_cols);
    }

    if (!obj_is_text_field(obj))
    {
        abobj_set_num_rows(tObj, num_rows);
        abobj_set_num_rows(obj, num_rows);
    }

    /*
      resize_in_pixels(obj, width, height, 2);
      */
}

/*
 * Change the Draw-Area size
 * NOTE: Regular width/height setting effects total VISIBLE
 * 	 size of DrawArea (if scrolling, size of ScrolledWindow)
 * 	 The "DrawArea" width/height are the underlying size
 *	 of the TOTAL DrawArea (including parts that arn't visible).
 *	 If there is no scrolling, width==drawarea_width &
 *	 height==drawarea_height.
 */
void
abobj_set_drawarea_size(
    ABObj	obj,
    int		 width,
    int		 height
)
{
    ABObj       daObj;   /* DrawArea object */

    if (!obj_is_drawing_area(obj))
	return;

    daObj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);

    if (obj_get_drawarea_width(obj) != width)
    {
        /* A change has occurred in the module so set the save flag */
        abobj_set_save_needed( obj_get_module(obj), TRUE);

	obj_set_drawarea_width(obj, width);
        objxm_obj_set_ui_arg(daObj, AB_ARG_INT, XmNwidth, width);
        obj_set_flag(daObj, AttrChangedFlag);
    }
    if (obj_get_drawarea_height(obj) != height)
    {
        /* A change has occurred in the module so set the save flag */
        abobj_set_save_needed( obj_get_module(obj), TRUE);

	obj_set_drawarea_height(obj, height);
        objxm_obj_set_ui_arg(daObj, AB_ARG_INT, XmNheight, height);
        obj_set_flag(daObj, AttrChangedFlag);
    }
}

/*
 * Set win-parent field for obj (if different from current)
 */
void
abobj_set_win_parent(
    ABObj	obj,
    ABObj	win_parent
)
{
    ABObj	old_win_parent;
    ABObj	old_mod, new_mod;

    if ((old_win_parent = obj_get_win_parent(obj)) != win_parent)
    {
	obj_set_win_parent(obj, win_parent);
	old_mod = obj_get_module(old_win_parent);
	new_mod = obj_get_module(win_parent);

	/* If either win_parent value was NULL, then the module will be NULL
	 */
	if (old_mod)
	    abobj_set_save_needed(old_mod, TRUE);
	if (new_mod)
	    abobj_set_save_needed(new_mod, TRUE);
    }
}

void
abobj_set_word_wrap(
    ABObj	obj,
    BOOL	wrap
)
{
    ABObj	subObj, swObj;

    if (obj_is_text_pane(obj) && obj->info.text.word_wrap != wrap)
    {
        /* A change has occurred in the module so set the save flag */
        abobj_set_save_needed( obj_get_module(obj), TRUE);

	obj->info.text.word_wrap = wrap;

	subObj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);

	/* If Scrolling is enabled and wrapping is turned OFF, then
	 * a horizontal scrollbar should be added.
	 */
	if (obj_get_vscrollbar_policy(obj) != AB_SCROLLBAR_NEVER )
	{
	    swObj = objxm_comp_get_subobj(obj, AB_CFG_POSITION_OBJ);

	    obj_set_hscrollbar_policy(obj,
			wrap? AB_SCROLLBAR_NEVER : AB_SCROLLBAR_ALWAYS);

	    /* Must re-instantiate both ScrolledWindow & Text */
	    obj_clear_flag(subObj, InstantiatedFlag);
	    obj_clear_flag(swObj,InstantiatedFlag);
	}
	else /* No Scrolling */
	{
	    objxm_obj_set_ui_arg(subObj, AB_ARG_BOOLEAN, XmNwordWrap, wrap);
	    obj_set_flag(subObj, AttrChangedFlag);
	}
    }
}


void
abobj_set_xy(
    ABObj	obj,
    int		x,
    int		y
)
{
    ABObj	posObj; /* Position Object */
    BOOL	move = False;

    posObj = objxm_comp_get_subobj(obj, AB_CFG_POSITION_OBJ);

    if (obj->x != x)
    {
        /* A change has occurred in the module so set the save flag */
        abobj_set_save_needed( obj_get_module(obj), TRUE);

	if (x != -1 && obj->attachments == NULL)
	{
	    objxm_obj_set_ui_arg(posObj, AB_ARG_INT, XmNx, x);
	    obj_set_flag(posObj, AttrChangedFlag);
	}
	move = True;
    }
    if (obj->y != y)
    {
        /* A change has occurred in the module so set the save flag */
        abobj_set_save_needed( obj_get_module(obj), TRUE);

	if (y != -1 && obj->attachments == NULL)
	{
            objxm_obj_set_ui_arg(posObj, AB_ARG_INT, XmNy, y);
            obj_set_flag(posObj, AttrChangedFlag);
	}
	move = True;
    }

    if (x != -1 && y != -1 && obj->attachments != NULL)
    {
        Dimension	obj_width;
        Dimension	obj_height;
    	Widget		widget = objxm_get_widget(obj);

	if (widget != NULL)
	{
            XtVaGetValues(widget,
                        XmNwidth,       &obj_width,
                        XmNheight,      &obj_height,
                        NULL);

	    abobj_calculate_new_layout(obj, x, y,
		obj->width < 0 ? obj_width : obj->width,
		obj->height < 0 ? obj_height : obj->height);
	}
    }

    if (move)
	obj_move(obj, x, y);
}

void
abobj_set_active(
    ABObj       obj,
    BOOL        active
)
{
    if (obj_is_initially_active(obj) != active)
    {
        /* A change has occurred in the module so set the save flag */
        abobj_set_save_needed( obj_get_module(obj), TRUE);

        obj_set_is_initially_active(obj, active);
    }
}

void
abobj_set_visible(
    ABObj       obj,
    BOOL        visible
)
{
    if (obj_is_initially_visible(obj) != visible)
    {
        /* A change has occurred in the module so set the save flag */
        abobj_set_save_needed( obj_get_module(obj), TRUE);

        obj_set_is_initially_visible(obj, visible);
    }
}

void
abobj_set_iconic(
    ABObj       obj,
    BOOL        iconic
)
{
    if (obj_is_initially_iconic(obj) != iconic)
    {
        /* A change has occurred in the module so set the save flag */
        abobj_set_save_needed( obj_get_module(obj), TRUE);

        obj_set_is_initially_iconic(obj, iconic);
    }
}

void
abobj_set_selected(
    ABObj       obj,
    BOOL     	selected
)
{
    if (obj_is_initially_selected(obj) != selected)
    {
        /* A change has occurred in the module so set the save flag */
        abobj_set_save_needed( obj_get_module(obj), TRUE);

        obj_set_is_initially_selected(obj, selected);

        if (obj_is_choice_item(obj))
        {
  	    ABObj	chobj = obj_get_root(obj_get_parent(obj));

	    if (obj_get_choice_type(chobj) != AB_CHOICE_OPTION_MENU)
	    {
                objxm_obj_set_ui_arg(obj, AB_ARG_BOOLEAN, XmNset, selected);
		obj_set_flag(obj, AttrChangedFlag);
	    }
        }
    }
}

void
abobj_set_selection_mode(
    ABObj       obj,
    AB_SELECT_TYPE select
)
{
    ABObj       subObj;

    if (obj_is_list(obj) && obj_get_selection_mode(obj) != select)
    {
	obj_set_selection_mode(obj, select);

        subObj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);
	objxm_obj_set_selection_arg(subObj, OBJXM_CONFIG_BUILD);

        /* A change has occurred in the module so set the save flag */
        abobj_set_save_needed( obj_get_module(obj), TRUE);
    }
}

void
abobj_set_tearoff(
    ABObj       obj,
    BOOL        tearoff
)
{
    if (obj_get_tearoff(obj) != tearoff)
    {
        /* A change has occurred in the module so set the save flag */
        abobj_set_save_needed( obj_get_module(obj), TRUE);

        obj_set_tearoff(obj, tearoff);
    }
}

void
abobj_set_text_type(
    ABObj		obj,
    AB_TEXT_TYPE	ttype
)
{
    ABObj	subObj;

    if (obj_get_text_type(obj) != ttype)
    {
	/* A change has occurred in the module so set the save flag */
        abobj_set_save_needed( obj_get_module(obj), TRUE);

	obj_set_text_type(obj, ttype);

	subObj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);

	if (obj_is_spin_box(obj))
	{
    	    objxm_obj_set_literal_ui_arg(subObj, OBJXM_CONFIG_BUILD,
		DtNspinBoxChildType, ttype == AB_TEXT_NUMERIC?
			DtNUMERIC : XmSTRING);

	    /* DtNspinBoxChildType can only be set at create time */
	    obj_clear_flag(subObj, InstantiatedFlag);
   	}
    }
}

void
abobj_set_decimal_points(
    ABObj	obj,
    int		dec_points
)
{
    ABObj       subObj;

    if (dec_points != obj_get_decimal_points(obj) &&
	(obj_is_scale(obj) || obj_is_spin_box(obj)))
    {
        /* A change has occurred in the module so set the save flag */
        abobj_set_save_needed( obj_get_module(obj), TRUE);

        obj_set_decimal_points(obj, dec_points);
        subObj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);

        objxm_obj_set_ui_arg(subObj, AB_ARG_INT, XmNdecimalPoints, dec_points);
        obj_set_flag(subObj, AttrChangedFlag);
    }
}

void
abobj_set_default_act_button(
    ABObj	obj,
    ABObj	button
)
{
    if (obj_get_default_act_button(obj) != button)
    {
        abobj_set_save_needed(obj_get_module(obj), TRUE);

	obj_set_default_act_button(obj, button);
	objxm_comp_set_default_button_args(obj, OBJXM_CONFIG_BUILD);
    }
}

void
abobj_set_direction(
    ABObj       obj,
    AB_DIRECTION dir
)
{
    ABObj       subObj;

    if (dir != obj_get_direction(obj) &&
        obj_is_scale(obj))
    {
        /* A change has occurred in the module so set the save flag */
        abobj_set_save_needed( obj_get_module(obj), TRUE);

        obj_set_direction(obj, dir);
        subObj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);
	objxm_obj_set_direction_arg(subObj, OBJXM_CONFIG_BUILD);
    }
}

void
abobj_set_help_act_button(
    ABObj	obj,
    ABObj	button
)
{
    if (obj_get_help_act_button(obj) != button)
    {
        abobj_set_save_needed(obj_get_module(obj), TRUE);

	obj_set_help_act_button(obj, button);
    }
}

void
abobj_set_increment(
    ABObj       obj,
    int         incr
)
{
    ABObj	subObj;

    if (incr != obj_get_increment(obj) &&
	(obj_is_scale(obj) || obj_is_spin_box(obj)))
    {
        /* A change has occurred in the module so set the save flag */
        abobj_set_save_needed( obj_get_module(obj), TRUE);

        subObj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);
        obj_set_increment(obj, incr);

	if (obj_is_spin_box(obj))
            objxm_obj_set_ui_arg(subObj, AB_ARG_INT, DtNincrementValue, incr);
	else if (obj_is_scale(obj))
	    objxm_obj_set_ui_arg(subObj, AB_ARG_INT, XmNscaleMultiple, incr);

	obj_set_flag(subObj, AttrChangedFlag);
    }
}

void
abobj_set_min_max_values(
    ABObj       obj,
    int         min,
    int		max
)
{
    ABObj       subObj;

    if (min != obj_get_min_value(obj) ||
	max != obj_get_max_value(obj) &&
        (obj_is_scale(obj) || obj_is_spin_box(obj)))
    {
        /* A change has occurred in the module so set the save flag */
        abobj_set_save_needed( obj_get_module(obj), TRUE);

        subObj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);
        obj_set_min_value(obj, min);
        obj_set_max_value(obj, max);

	if (obj_is_spin_box(obj))
  	{
	    objxm_obj_set_ui_arg(subObj, AB_ARG_INT, DtNminimumValue,
                obj_get_min_value(obj));
            objxm_obj_set_ui_arg(subObj, AB_ARG_INT, DtNmaximumValue,
                obj_get_max_value(obj));
	}
	else if (obj_is_scale(obj))
	{
            objxm_obj_set_ui_arg(subObj, AB_ARG_INT, XmNminimum, min);
            objxm_obj_set_ui_arg(subObj, AB_ARG_INT, XmNmaximum, max);
	}
        obj_set_flag(subObj, AttrChangedFlag);
    }
}

int
abobj_set_save_needed(
    ABObj	obj,
    BOOL	set
)
{
    ABObj	module = NULL;
    if ((AB_builder_mode != MODE_BUILD) || !abobj_save_needed_enabled())
	return 0;

    if (set)
	obj_set_flag(obj, SaveNeededFlag);
    else
	obj_clear_flag(obj, SaveNeededFlag);

    /* make sure we save the module */
    if (set)
    {
        module = obj_get_module(obj);
        if ((module != NULL) && (module != obj))
        {
	    abobj_set_save_needed(module, set);
        }
    }

/* REMIND: This call should change to update some other area
 *	   on the palette.
 */
    /*
     * Only update the palette if the module or project changes.
     * The module always gets set to dirty if any of its objects becomes
     * dirty (see above).
     */
    if (obj_is_module(obj) || obj_is_project(obj))
    {
        if (abobj_update_palette_title(proj_get_project()) == -1)
	    return -1;
    }

    return 0;
}

void
abobj_disable_save_needed(
)
{
    save_needed_enabled = FALSE;
}

void
abobj_enable_save_needed(
)
{
    save_needed_enabled = TRUE;
}

BOOL
abobj_save_needed_enabled(
)
{
    return (save_needed_enabled);
}

void
abobj_set_pattern_type(
    ABObj		obj,
    AB_FILE_TYPE_MASK 	fmtype
)
{
    ABObj			subObj;
    unsigned char		value;
    AB_FILE_TYPE_MASK old_fmtype = obj_get_file_type_mask(obj);

    if (old_fmtype != fmtype)
    {
	/* A change has occurred in the module so set the save flag */
        abobj_set_save_needed( obj_get_module(obj), TRUE);

	obj_set_file_type_mask(obj, fmtype);

        subObj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);

        switch (fmtype)
        {
            case AB_FILE_REGULAR:
                value = XmFILE_REGULAR;
                break;
            case AB_FILE_DIRECTORY:
                value = XmFILE_DIRECTORY;
                break;
            case AB_FILE_ANY:
                value = XmFILE_ANY_TYPE;
                break;
        }
        objxm_obj_set_ui_arg(subObj, AB_ARG_LITERAL, XmNfileTypeMask, value);
        obj_set_flag(subObj, AttrChangedFlag);
    }
}

void
abobj_set_directory(
    ABObj       obj,
    STRING      val
)
{
    ABObj       subObj;  /* object */
    STRING      oldval;
    STRING	newval = NULL;

    newval = (util_strempty(val)? NULL : val);

    if (obj_is_file_chooser(obj))
    {
        oldval = obj_get_directory(obj);

        if (!util_streq(oldval, newval) )
        {
            /* A change has occurred in the module so set the save flag */
            abobj_set_save_needed( obj_get_module(obj), TRUE);

            subObj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);

            obj_set_directory(obj, newval);

            objxm_obj_set_ui_arg(subObj, AB_ARG_XMSTRING, XmNdirectory,
		newval? (XtArgVal)XmStringCreateLocalized(newval) : (XtArgVal)NULL);

            obj_set_flag(subObj, AttrChangedFlag);
        }
    }
}

void
abobj_set_filter_pattern(
    ABObj       obj,
    STRING      val
)
{
    ABObj       subObj;  /* object */
    STRING      oldval;
    STRING      newval = NULL;

    newval = (util_strempty(val)? NULL : val);

    if (obj_is_file_chooser(obj))
    {
        oldval = obj_get_filter_pattern(obj);

        if (!util_streq(oldval, newval) )
        {
            /* A change has occurred in the module so set the save flag */
            abobj_set_save_needed( obj_get_module(obj), TRUE);

            subObj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);

            obj_set_filter_pattern(obj, newval);

	    objxm_obj_set_ui_arg(subObj, AB_ARG_XMSTRING, XmNpattern,
		newval? (XtArgVal)XmStringCreateLocalized(newval) : (XtArgVal)NULL);
            obj_set_flag(subObj, AttrChangedFlag);
        }
    }
}

void
abobj_set_ok_label(
    ABObj       obj,
    STRING      val
)
{
    ABObj       subObj;  /* object */
    STRING      oldval;
    STRING      newval = NULL;

    newval = (util_strempty(val)? NULL : val);

    if (obj_is_file_chooser(obj))
    {
        oldval = obj_get_ok_label(obj);

        if (!util_streq(oldval, newval) )
        {
            /* A change has occurred in the module so set the save flag */
            abobj_set_save_needed( obj_get_module(obj), TRUE);

            subObj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);

            obj_set_ok_label(obj, newval);

	    objxm_obj_set_ui_arg(subObj, AB_ARG_XMSTRING, XmNokLabelString,
                newval? (XtArgVal)XmStringCreateLocalized(newval) : (XtArgVal)NULL);
            obj_set_flag(subObj, AttrChangedFlag);
        }
    }
}

void
abobj_set_auto_dismiss(
    ABObj       obj,
    BOOL        val
)
{
    ABObj       subObj;  /* object */
    BOOL      	oldval;

    if (obj_is_file_chooser(obj))
    {
        oldval = obj_get_auto_dismiss(obj);

        if (val != oldval )
        {
            /* A change has occurred in the module so set the save flag */
            abobj_set_save_needed( obj_get_module(obj), TRUE);

            subObj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);

            obj_set_auto_dismiss(obj, val);

	    objxm_obj_set_ui_arg(subObj, AB_ARG_BOOLEAN, XmNautoUnmanage, val);

            obj_set_flag(subObj, AttrChangedFlag);
        }
    }
}

void
abobj_set_sessioning_method(
    ABObj	proj,
    AB_SESSIONING_METHOD ss_mthd
)
{

    if (!proj || !obj_is_project(proj))
	return;

    if (obj_get_sessioning_method(proj) != ss_mthd)
    {
        obj_set_sessioning_method(proj, ss_mthd);
        abobj_set_save_needed(proj, TRUE);
    }
}

void
abobj_set_attachment(
    ABObj		obj,
    AB_COMPASS_POINT	dir,
    ABAttachment	*attach
)
{
    AB_ATTACH_TYPE	old_attach_type;

    if (!obj || !attach)
	return;

    old_attach_type = obj_get_attach_type(obj, dir);

    /*
     * Unset centering event handler
     */
    if ( ((old_attach_type == AB_ATTACH_CENTER_GRIDLINE) ||
		(attach->type == AB_ATTACH_CENTER_GRIDLINE)) &&
	(old_attach_type != attach->type) )
    {
	Widget			w;

	w = objxm_get_widget(obj);

	if (w)
	{
            DTB_CENTERING_TYPES	centering_type;

            centering_type = abobj_get_centering_type(obj);
	    dtb_uncenter(w, centering_type);
	}
    }

    obj_set_attachment(obj, dir, attach->type,
			attach->value, attach->offset);
    objxm_obj_set_attachment_args(obj, OBJXM_CONFIG_BUILD);

    /*
     * Set centering event handler
     */
    if ( ((old_attach_type == AB_ATTACH_CENTER_GRIDLINE) ||
		(attach->type == AB_ATTACH_CENTER_GRIDLINE)) &&
	(old_attach_type != attach->type) )
    {
	Widget			w;

	w = objxm_get_widget(obj);

	if (w)
	{
            DTB_CENTERING_TYPES	centering_type;

            centering_type = abobj_get_centering_type(obj);
	    dtb_center(w, centering_type);
	}
    }

    abobj_set_save_needed(obj_get_module(obj), TRUE);
}

void
abobj_set_pane_min(
    ABObj	obj,
    int		value
)
{
    ABObj	xyObj = NULL;

    if (obj_get_pane_min(obj) != value)
    {
	/* A change has occurred in the module so set the save flag */
	abobj_set_save_needed( obj_get_module(obj), TRUE);

	xyObj = objxm_comp_get_subobj(obj, AB_CFG_POSITION_OBJ);
	obj_set_pane_min(obj, value);
	objxm_obj_set_ui_arg(xyObj, AB_ARG_INT, XmNpaneMinimum, value);
	obj_set_flag(xyObj, AttrChangedFlag);
    }
}

void
abobj_set_pane_max(
    ABObj       obj,
    int         value
)
{
    ABObj       xyObj = NULL;

    if (obj_get_pane_max(obj) != value)
    {
	/* A change has occurred in the module so set the save flag */
	abobj_set_save_needed( obj_get_module(obj), TRUE);

	xyObj = objxm_comp_get_subobj(obj, AB_CFG_POSITION_OBJ);
	obj_set_pane_max(obj, value);
	objxm_obj_set_ui_arg(xyObj, AB_ARG_INT, XmNpaneMaximum, value);
	obj_set_flag(xyObj, AttrChangedFlag);
    }
}

void
abobj_set_i18n_enabled(
    ABObj	proj,
    BOOL	i18n_enabled
)
{

    if (!proj || !obj_is_project(proj))
	return;

    if (obj_get_i18n_enabled(proj) != i18n_enabled)
    {
        obj_set_i18n_enabled(proj, i18n_enabled);
        abobj_set_save_needed(proj, TRUE);
    }
}
