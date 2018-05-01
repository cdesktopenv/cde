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
 *      $XConsortium: prop_items.c /main/4 1996/08/07 19:50:46 mustafa $
 *
 * @(#)prop_items.c	1.31 11 Aug 1995 cde_app_builder/src/ab
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
 *****************************************************************
 * prop_items.c - Implements all common property sheet functionality
 * 		  for Item-editing
 *
 *****************************************************************
 */
#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 1
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
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
#include <Xm/TextF.h>
#include <Xm/ToggleB.h>
#include <Dt/ComboBox.h>
#include <Dt/SpinBox.h>
#include <ab_private/trav.h>
#include <ab_private/obj_notify.h>
#include <ab_private/objxm.h>
#include <ab_private/ab.h>
#include <ab_private/prop.h>
#include <ab_private/propP.h>
#include <ab_private/abobjP.h> 
#include <ab_private/abobj_set.h> 
#include <ab_private/ui_util.h>
#include <ab_private/x_util.h>

#define ITEM_NAME_MAX	512

#define HasItems(o) (obj_is_choice(o) || obj_is_list(o) || \
        obj_is_menubar(o) || obj_is_menu(o) || \
	obj_is_combo_box(o) || obj_is_spin_box(o))

#define PointerToRealItem(o) (o)->ui_handle

#define HasActiveState(t) ((t) == AB_ITEM_FOR_CHOICE || \
	(t) == AB_ITEM_FOR_MENUBAR || (t) == AB_ITEM_FOR_MENU)

#define HasSelectedState(t) ((t) == AB_ITEM_FOR_CHOICE || \
	(t) == AB_ITEM_FOR_LIST || (t) == AB_ITEM_FOR_COMBO_BOX || \
	(t) == AB_ITEM_FOR_SPIN_BOX)

#define HasLabelType(t) ((t) == AB_ITEM_FOR_CHOICE || \
	(t) == AB_ITEM_FOR_MENUBAR || (t) == AB_ITEM_FOR_MENU)

#define HasHelpState(t) ((t) == AB_ITEM_FOR_MENUBAR)

#define HasSubmenu(t) ((t) == AB_ITEM_FOR_MENUBAR || (t) == AB_ITEM_FOR_MENU)

#define HasMnemonic(t) ((t) == AB_ITEM_FOR_MENUBAR || (t) == AB_ITEM_FOR_MENU)

#define HasAccelerator(t) ((t) == AB_ITEM_FOR_MENU)

#define HasLabelLineStyle(t) ((t) == AB_ITEM_FOR_MENU)

#define RequiresSelection(p) (p->item_type == AB_ITEM_FOR_COMBO_BOX || \
	p->item_type == AB_ITEM_FOR_SPIN_BOX || \
        (p->item_type == AB_ITEM_FOR_CHOICE && \
	  obj_get_choice_type(*(p->current_obj_ptr)) != AB_CHOICE_NONEXCLUSIVE))
	  


static const char       *SeparatorNamebase ={"separator"};

/*************************************************************************
**                                                                      **
**       Private Function Declarations                                  **
**                                                                      **
*************************************************************************/

static void	activate_item_settings(
		    PropItemsSetting	pis,
		    BOOL		active
		);
static void     activate_menu_item_settings( 
                    PropItemsSetting     pis, 
                    BOOL                active
                ); 
static void	copy_item_to_clipboard(
		    ABObj		iobj,
		    ABObj		*item_clipboard
		);
static int	copy_item_fields(
		    ABObj		iobj,
		    ABObj		iobj_copy
		);
static BOOL	delete_selected_item(
		    PropItemsSetting	pis
		);
static int	get_current_item_pos( 
    		    PropItemsSetting    pis 
		); 
static ABObj	get_selected_item_copy(
		    Widget		itemlist,
		    int			*ppos
		);
static STRING	get_unique_default_label(
    		    Widget      	itemlist,
    		    STRING      	base
		);
static STRING	get_graphic_namebase(
		    STRING		graphic_path
		);
static ABObj	init_new_item(
    		    PropItemsSetting    pis
		);
static void	load_item(
    		    PropItemsSetting    pis,
    		    ABObj               iobj
		);
static void	setup_item_label_setting(
		    PropItemsSetting	pis,
		    AB_LABEL_TYPE	label_type,
		    STRING		label,
		    AB_LINE_TYPE	label_line_style
		);
static void	verify_and_set_item_selected(
    		    PropItemsSetting    pis,
    		    BOOL                selected
		);
/*
 * Callbacks
 */
static void     add_itemCB(
                    Widget      widget, 
                    XtPointer   client_data, 
                    XtPointer   call_data 
                ); 
static void     edit_itemCB(
                    Widget      widget,
                    XtPointer   client_data,
                    XtPointer   call_data
                );
static void	field_activateCB(
		    Widget      widget,
                    XtPointer   client_data,
                    XtPointer   call_data
                );
static void     select_itemCB(
                    Widget      widget,
                    XtPointer   client_data,
                    XtPointer   call_data
                );
static void	set_edit_menu_stateCB(
		    Widget	widget,
                    XtPointer   client_data,
                    XtPointer   call_data
                );

/*************************************************************************
**                                                                      **
**       Data Declarations                                              **
**                                                                      **
*************************************************************************/


/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
*************************************************************************/

/* 
 * Initialize the mechanism to do Prop Sheet Item Editing
 */
void
prop_item_editor_init(
    PropItemsSetting	pis,
    AB_ITEM_TYPE	type,
    Widget		list,
    Widget		changebar,
    Widget		add_button,
    int			add_count,
    WidgetList		add_item,
    int			*add_enum,
    int			edit_count,
    WidgetList		edit_item,
    int			*edit_enum,
    PropFieldSetting    label_pfs, 
    PropOptionsSetting  label_type_pos, 
    Widget		label_graphic_hint,
    PropFieldSetting    mnemonic_pfs, 
    PropFieldSetting    accel_pfs,
    PropOptionsSetting  line_style_pos,
    PropCheckboxSetting state_pcs,
    PropMenunameSetting	menuname_pms,
    ABObj		*current_obj_ptr
)
{
    Widget		  menu_shell;
    int			  i;
   
    pis->item_type = type;
    pis->item_list = list;
    pis->item_menu = XtParent(add_item[0]);
    pis->item_label_pfs = label_pfs;
    pis->item_label_type_pos = label_type_pos;
    pis->item_mnemonic_pfs = mnemonic_pfs;
    pis->item_accel_pfs = accel_pfs;
    pis->item_line_style_pos = line_style_pos;
    pis->item_state_pcs = state_pcs;
    pis->item_menuname_pms = menuname_pms;
    pis->changebar = changebar;
    pis->current_item = NULL;
    pis->clipboard_item = NULL;
    pis->current_obj_ptr = current_obj_ptr;

    propP_changebar_init(changebar, list);

    XtVaSetValues(pis->item_list,
		XmNselectionPolicy, 	XmBROWSE_SELECT,
    		XmNautomaticSelection,	TRUE,
		NULL);

    XtVaSetValues(XtParent(pis->item_list),
                XmNrightAttachment,     XmATTACH_FORM,
                XmNrightOffset,         100,
		NULL);

    XtAddCallback(pis->item_list, XmNbrowseSelectionCallback, 
			select_itemCB, (XtPointer)pis);

    XtAddCallback(pis->item_label_pfs->field, XmNactivateCallback,
		field_activateCB, (XtPointer)pis);

    XtVaSetValues(pis->item_label_pfs->label, 
                XmNuserData, (XtArgVal)label_graphic_hint,
                XmNrecomputeSize, False,
                NULL);

    /* Set Up Add Button */
    XtVaSetValues(add_button, XmNuserData, (XtArgVal)INSERT_AFTER, NULL);
    XtAddCallback(add_button, XmNactivateCallback, add_itemCB, (XtPointer)pis);

    /* Set Up Edit MenuButton */
    for(i = 0; i < add_count; i++)
    {
	XtVaSetValues(add_item[i], XmNuserData, (XtArgVal)add_enum[i], NULL);
	XtAddCallback(add_item[i], XmNactivateCallback,
		add_itemCB, (XtPointer)pis);
    }
    for(i=0; i < edit_count; i++) 
    { 
        XtVaSetValues(edit_item[i], XmNuserData, (XtArgVal)edit_enum[i], NULL); 
        XtAddCallback(edit_item[i], XmNactivateCallback, 
                edit_itemCB, (XtPointer)pis); 
    }
  
    /* Register a popup callback on the menubutton's menushell so that
     * menuitems can be made active/inactive depending on the state
     * of the item list at the time the menu is invoked.
     */
    menu_shell = ui_get_ancestor_shell(add_item[0]);
    XtAddCallback(menu_shell, XtNpopupCallback, set_edit_menu_stateCB, 
		(XtPointer)pis);

}

void
prop_item_editor_load(
    PropItemsSetting    pis,
    ABObj       	obj
)
{
    AB_TRAVERSAL trav;
    ABObj       pobj, iobj;
    ABObj       *iobj_list;
    int         num_items;
    STRING	list_ident;
    int         i;

    if (!HasItems(obj))
	return;

    /* Clear any current items from list */
    prop_item_editor_clear(pis);

    /* Load new obj's items into list */
    pobj = objxm_comp_get_subobj(obj, AB_CFG_PARENT_OBJ);

    if ((num_items = obj_get_num_items(pobj)) > 0)
    {
    	/* Store an array of copies of the items in the
     	 * list's userData
     	 */  
    	iobj_list = (ABObj *)util_malloc(num_items*sizeof(ABObj));
	activate_item_settings(pis, True);

    	for (trav_open(&trav, pobj, AB_TRAV_ITEMS_FOR_OBJ), i = 0;
            (iobj = trav_next(&trav)) != NULL; i++)
    	{
            /* Make a copy of the item's obj, so changes can
             * be made to it without affecting the real item obj
             */
            iobj_list[i] = obj_create(iobj->type, NULL);
            copy_item_fields(iobj, iobj_list[i]);

	    /* Keep track of Copy's real Item object 
	     */
	    PointerToRealItem(iobj_list[i]) = (XtPointer)iobj;

	    list_ident = prop_item_get_namebase(iobj);

            ui_list_add_item(pis->item_list, list_ident, 0);
    	}
    	trav_close(&trav);
    	XtVaSetValues(pis->item_list, XmNuserData, iobj_list, NULL);
    	XmListSelectPos(pis->item_list, 1, TRUE);
    }
    else /* No Items to load */
    	activate_item_settings(pis, False);
}

void
prop_item_editor_clear(
    PropItemsSetting	pis
)
{
    ABObj       *iobj_list;
    int         num_items;
    int         i;

    /* Destroy dup'd array of item objs */
    XtVaGetValues(pis->item_list,
                XmNuserData,  &iobj_list,
                XmNitemCount, &num_items,
                NULL);

    if (num_items > 0) /* Clear List & Array of Item copies...*/ 
    {
    	for (i=0; i < num_items; i++)
	{
	    PointerToRealItem(iobj_list[i]) = NULL;
            obj_destroy(iobj_list[i]);
	}

        util_free(iobj_list);
	XtVaSetValues(pis->item_list, XmNuserData, (XtArgVal)NULL, NULL);
        XmListDeleteAllItems(pis->item_list);
    }

    /* Clear Item Settings */
    prop_field_set_value(pis->item_label_pfs, "", False);

    if (HasLabelType(pis->item_type))
    	prop_options_set_value(pis->item_label_type_pos, (XtPointer)AB_LABEL_STRING, False);

    if (HasMnemonic(pis->item_type))
	prop_field_set_value(pis->item_mnemonic_pfs, "", False);

    if (HasAccelerator(pis->item_type))
	prop_field_set_value(pis->item_accel_pfs, "", False);

    if (HasLabelLineStyle(pis->item_type))
	prop_options_set_value(pis->item_line_style_pos, (XtPointer)AB_LINE_ETCHED_OUT, False);

    if (HasSubmenu(pis->item_type))
	prop_menuname_set_value(pis->item_menuname_pms, "", False);

    if (HasActiveState(pis->item_type))
    	prop_checkbox_set_value(pis->item_state_pcs, AB_STATE_ACTIVE , True, False);

    if (HasSelectedState(pis->item_type))
    	prop_checkbox_set_value(pis->item_state_pcs, AB_STATE_SELECTED, False, False);

    if (HasHelpState(pis->item_type))
	prop_checkbox_set_value(pis->item_state_pcs, HELP_ITEM_KEY, False, False);

    pis->current_item = NULL;

    if (pis->clipboard_item != NULL)
    {
	PointerToRealItem(pis->clipboard_item) = NULL;
    	obj_destroy(pis->clipboard_item);
    	pis->clipboard_item = NULL;
    }

}

/*
 * Transfer all changes made to item-copies inside the Item Editor
 * to the actual item objects they correspond to
 */
void
prop_item_editor_apply(
    PropItemsSetting	pis
)
{
    ABObj	current_obj;
    AB_TRAVERSAL trav;
    ABObj       *iobj_list;
    ABObj       pobj, oobj, iobj;
    Widget	*real_widget = NULL;
    int		*real_pos = NULL;
    ABObj	*real_iobj = NULL;
    BOOL	item_transfer = False;
    Widget	parent;
    STRING      item_name;
    STRING	name_base;
    int         new_num_items;
    int         num_items;
    int		current_item_pos = 0;
    int		del_count = 0;
    int         i, j;

    if ((current_obj = *(pis->current_obj_ptr)) == NULL)
	return;

    XtVaGetValues(pis->item_list,
                XmNuserData,    &iobj_list,
                XmNitemCount,   &new_num_items,
                NULL);

    pobj = objxm_comp_get_subobj(current_obj, AB_CFG_PARENT_OBJ);
    oobj = objxm_comp_get_subobj(current_obj, AB_CFG_OBJECT_OBJ);
    num_items = obj_get_num_items(pobj);

    parent = objxm_get_widget(pobj);
    if (pis->item_type == AB_ITEM_FOR_COMBO_BOX)
	parent = ui_combobox_get_list_widget(parent);

    /* Store pointers to the Live Item objects & their associated widgets
     * into linear arrays, so we can easily re-arrange them to match
     * applied changes (inserts, deletes, moves)
     */
    if (num_items > 0)
    {
    	real_iobj = (ABObj *)util_malloc(num_items*sizeof(ABObj));

	/* NOTE: don't need to store widgets for Menu Items since they 
	 *       are not instantiated during build-mode
	 */
    	switch(pis->item_type)
    	{
	    case AB_ITEM_FOR_LIST:
	    case AB_ITEM_FOR_COMBO_BOX:
	    case AB_ITEM_FOR_SPIN_BOX:
		real_pos = (int*)util_malloc(num_items*sizeof(int));
		break;
	    case AB_ITEM_FOR_MENUBAR:
	    case AB_ITEM_FOR_CHOICE:
		real_widget = (Widget*)util_malloc(num_items*sizeof(Widget));
	}
    }

    trav_open(&trav, pobj, AB_TRAV_ITEMS_FOR_OBJ);
    for (i=0; i < num_items; i++)
    {
	real_iobj[i] = trav_next(&trav);
	if (real_widget)
	{
	    /* Detach widget from item-object */
	    real_widget[i] = objxm_get_widget(real_iobj[i]);
	    abobjP_disable_build_actions(real_iobj[i], real_widget[i]);
	    objxm_set_widget(real_iobj[i], NULL);
	    objxm_free_obj_and_actions(real_widget[i]);
	}
	else if (real_pos)
	    real_pos[i] = i + 1;
    }
    trav_close(&trav);

    /* Now that we've stored them in an array, unparent them */
    for (i=0; i < num_items; i++)
	obj_unparent(real_iobj[i]);

    for (i=0; i < new_num_items; i++)
    {
	iobj = NULL;
	item_transfer = False;

	if (PointerToRealItem(iobj_list[i]) != NULL)
	{
	    /* Find real item object which corresponds to Copy */
	    for (j=0; iobj == NULL && j < num_items; j++)
	    	if (PointerToRealItem(iobj_list[i]) == real_iobj[j])
	    	{
	    	    iobj = real_iobj[j];
		    real_iobj[j] = NULL; /* Consume */
		    if (j != i)
			item_transfer = True;
	        }
	}

	if (iobj == NULL) /* none found ->We have a New object here! */
	{
            iobj = obj_create(AB_TYPE_ITEM, NULL);
            iobj->info.item.type = pis->item_type;
            PointerToRealItem(iobj_list[i]) = iobj; 
	    item_transfer = True;
  	}
	obj_append_child(pobj, iobj);

	/* 
	 * Now Transfer all attribute values of Copy to Real item obj...
  	 */

        /* Must reset Item's name in case Obj's name changed */
        name_base = prop_item_get_namebase(iobj_list[i]);
        item_name = abobj_construct_item_name(obj_get_name(current_obj),
                         name_base, "item");
 
        obj_set_name(iobj_list[i], item_name);
        obj_set_unique_name(iobj, obj_get_name(iobj_list[i]));
 
        objxm_obj_configure(iobj, OBJXM_CONFIG_BUILD, TRUE);
 
        abobj_set_label(iobj, iobj_list[i]->label_type, obj_get_label(iobj_list[i]));
        abobj_set_accelerator(iobj, obj_get_accelerator(iobj_list[i]));
        abobj_set_mnemonic(iobj, obj_get_mnemonic(iobj_list[i]));
        abobj_set_line_style(iobj, obj_get_line_style(iobj_list[i]));
 
        abobj_set_visible(iobj, obj_is_initially_visible(iobj_list[i]));
        abobj_set_menu_name(iobj, obj_get_menu_name(iobj_list[i]));
        obj_set_is_help_item(iobj, obj_is_help_item(iobj_list[i]));
        abobj_set_selected(iobj, obj_is_initially_selected(iobj_list[i]));
        abobj_set_active(iobj, obj_is_initially_active(iobj_list[i]));
 
        /* Color should be the same as the object parenting the Item */
        abobj_set_background_color(iobj, obj_get_bg_color(current_obj));
        abobj_set_foreground_color(iobj, obj_get_fg_color(current_obj));

	/* Trick to avoid sorting the actual widgets...
	 * we re-use the existing widget order, just re-assign them to
	 * new item objects (SetValues will take care of setting correct
	 * attributes).
	 */
	if (i < num_items)
	{
	    if (real_widget)
	    {
		/* Transfer widget to item object */
	        objxm_set_widget(iobj, real_widget[i]);
		objxm_store_obj_and_actions(real_widget[i], iobj);
                abobjP_enable_build_actions(iobj, real_widget[i]);

		if (item_transfer)
		{
                    /* Resources may be set differently for iobj than they were
                     * set for the previous iobj which owned this widget
                     */
		    objxm_comp_set_ui_args(iobj, OBJXM_CONFIG_BUILD, True);
		    obj_set_flag(iobj, AttrChangedFlag);
		}
	        real_widget[i] = NULL; /* Consume */
	    }
	    else if (real_pos) /* Items are not actually widgets */
	    {
		if (item_transfer)
		{
		    XmString xmitem = XmStringCreateLocalized(obj_get_label(iobj));

		    switch(pis->item_type)
		    {
		        case AB_ITEM_FOR_COMBO_BOX:
		    	case AB_ITEM_FOR_LIST:
			    if (parent)
                            	XmListReplacePositions(parent, &(real_pos[i]), &xmitem, 1);
			    else
				util_dprintf(0, 
				   "prop_item_editor_apply: ERROR parent for item %s is NULL\n",
					util_strsafe(obj_get_label(iobj)));
		  	    break;
		    	case AB_ITEM_FOR_SPIN_BOX:
                    	    DtSpinBoxDeletePos(parent, real_pos[i]);
                    	    DtSpinBoxAddItem(parent, xmitem, real_pos[i]);
			    break;
                    }
                    XmStringFree(xmitem);
		}
		real_pos[i] = 0; /* Consume */
	    }
	    obj_set_flag(iobj, InstantiatedFlag);
	}
	else /* no existing widget for Item object's position */
	    obj_clear_flag(iobj, InstantiatedFlag);

    }
    /* Do garbage collection...anything that was not consumed above
     * should be destroyed...
     */
    parent = objxm_get_widget(oobj);;

    for (i=0; i < num_items; i++) 
    {
	if (real_iobj && real_iobj[i] != NULL)
	    obj_destroy(real_iobj[i]);

	if (real_widget && real_widget[i] != NULL)
	    XtDestroyWidget(real_widget[i]);
	else if (real_pos && real_pos[i] != 0)
 	{
            if (parent && pis->item_type == AB_ITEM_FOR_LIST)
                XmListDeletePos(parent, real_pos[i - del_count]);

            else if (parent && pis->item_type == AB_ITEM_FOR_COMBO_BOX)
            	DtComboBoxDeletePos(parent, real_pos[i - del_count]);

            else if (parent && pis->item_type == AB_ITEM_FOR_SPIN_BOX)
            	DtSpinBoxDeletePos(parent, real_pos[i - del_count]);
	    del_count++;
	}
    }
    if (num_items > 0)
    {
    	util_free(real_iobj);
    	util_free(real_widget);
	util_free(real_pos);
    }

    /* ComboBox doesn't handle dynamic changes well, so re-instantiate */
    if (obj_is_combo_box(current_obj))
	obj_tree_clear_flag(current_obj, InstantiatedFlag);

    /* Ensure Item is selected in ItemList */
    current_item_pos = get_current_item_pos(pis);
    ui_list_select_pos(pis->item_list, current_item_pos, False);

}

void 
prop_item_change(  
    PropItemsSetting    pis,
    BOOL		select_next	
)  
{  
    ABObj	 current_obj;
    STRING	 newlabel = NULL, basename = NULL, graphic_path;
    BOOL	 label_type_chg = False;
    AB_LABEL_TYPE new_label_type;
    XmString	 xmitem;
    int		 current_item_pos = 0;
    ABObj	 *iobj_list;
    int		 num_items;
    int		 next_pos;

    if ((current_obj = *(pis->current_obj_ptr)) == NULL)
        return;

    if (pis->current_item)
    {
    	XtVaGetValues(pis->item_list,
                XmNuserData,    &iobj_list,
                XmNitemCount,   &num_items,
                NULL);
	current_item_pos = get_current_item_pos(pis);

	if (current_item_pos == 0)
	{
	    util_dprintf(3, "prop_item_change: could not find item in List\n");
	    return;
	}

	if (HasLabelType(pis->item_type))
	{
	    new_label_type =
		(AB_LABEL_TYPE)prop_options_get_value(pis->item_label_type_pos);
	    label_type_chg = (new_label_type != pis->current_item->label_type);
	    pis->current_item->label_type = new_label_type;
	}

	if (HasMnemonic(pis->item_type))
	    obj_set_mnemonic(pis->current_item, prop_field_get_value(pis->item_mnemonic_pfs));

	if (HasAccelerator(pis->item_type))
	    obj_set_accelerator(pis->current_item, prop_field_get_value(pis->item_accel_pfs));

	if (HasSelectedState(pis->item_type))
	    verify_and_set_item_selected(pis, 
		prop_checkbox_get_value(pis->item_state_pcs, AB_STATE_SELECTED));

	if (HasActiveState(pis->item_type))
	    obj_set_is_initially_active(pis->current_item,
		prop_checkbox_get_value(pis->item_state_pcs, AB_STATE_ACTIVE));

	if (HasHelpState(pis->item_type))
	{
	    BOOL help_item;

            if ((help_item = prop_checkbox_get_value(pis->item_state_pcs, HELP_ITEM_KEY)) == True)
            {
            	if (!prop_help_item_ok(pis->item_list, pis->current_item))
            	{
                    prop_checkbox_set_value(pis->item_state_pcs, HELP_ITEM_KEY, False, False);
                    help_item = FALSE;
            	}   
	     }
             obj_set_is_help_item(pis->current_item, help_item);
	}
	if (HasSubmenu(pis->item_type))
	{
	    STRING menuname;

            /* If valid Sub-menu, set menu_name */
            if (prop_submenu_name_ok(pis->item_menuname_pms->field, current_obj))
            {
            	menuname = ui_field_get_string(pis->item_menuname_pms->field);
            	obj_set_menu_name(pis->current_item,
                	(util_strcmp(menuname, "") == 0? NULL : menuname));
            	util_free(menuname);
            }
	}

	switch(pis->current_item->label_type)
	{
	    case AB_LABEL_SEPARATOR:
	    	/* 
	     	 * Separator Menu-items have a NULL label string, but they still
	     	 * require a string for identification in the itemlist as well
	     	 * as to base the item name
	     	 */
		newlabel = NULL;
	    	obj_set_line_style(pis->current_item,
                        (AB_LINE_TYPE)prop_options_get_value(pis->item_line_style_pos));
	    	prop_field_set_value(pis->item_label_pfs, "", False);
	    	basename = get_unique_default_label(pis->item_list, (STRING)SeparatorNamebase);
		break;

	    case AB_LABEL_GLYPH:
		if (!prop_graphic_filename_ok(pis->item_label_pfs->field, False))
		    /* Couldn't convert to Pixmap, so reset back to type String */
                    pis->current_item->label_type = AB_LABEL_STRING;
		else
		{
		    /* Strip off graphic filename path for item name */
		    graphic_path = newlabel = prop_field_get_value(pis->item_label_pfs);
		    basename = get_graphic_namebase(graphic_path);
		}
		break;
	}
	if (pis->current_item->label_type == AB_LABEL_STRING)
	    /* memory alloc'd in call below */
	    basename = newlabel = prop_field_get_value(pis->item_label_pfs);

        if (label_type_chg ||
	    !util_streq(obj_get_label(pis->current_item), newlabel))
        {
            obj_set_label(pis->current_item, newlabel);
            abobj_set_item_name(pis->current_item, obj_get_module(current_obj),
                        obj_get_name(current_obj), basename);
        
	    xmitem = XmStringCreateLocalized(basename);
            XmListReplacePositions(pis->item_list, &current_item_pos, &xmitem, 1);
            XmStringFree(xmitem);
	}

	if (pis->current_item->label_type != AB_LABEL_SEPARATOR)
            util_free(newlabel);

	/* Select the "next" item in the list */
	if (select_next)
	{
	    next_pos = current_item_pos == num_items? 1 : current_item_pos + 1;
    	    pis->current_item = iobj_list[next_pos - 1];
	    ui_list_select_pos(pis->item_list, next_pos, False);
    	    load_item(pis, iobj_list[next_pos - 1]);
	}
    }
}
  
void
prop_item_edit(
    PropItemsSetting	pis,
    AB_EDIT_TYPE        etype
)
{
    ABObj       iobj_copy;
    int         pos;
    int		new_item_pos;
 
    iobj_copy = get_selected_item_copy(pis->item_list, &pos);
 
    switch(etype)
    {
        case AB_EDIT_CUT:
            copy_item_to_clipboard(iobj_copy, &(pis->clipboard_item));
	    PointerToRealItem(pis->clipboard_item) = PointerToRealItem(iobj_copy);
            delete_selected_item(pis);
            break;
        case AB_EDIT_COPY:
            copy_item_to_clipboard(iobj_copy, &(pis->clipboard_item));
	    /* NOTE: Don't copy PointerToRealItem for a "Copy" */
            break;
        case AB_EDIT_PASTE:
            if (pis->clipboard_item != NULL)
            {
                iobj_copy = obj_create(pis->clipboard_item->type, NULL);
                copy_item_fields(pis->clipboard_item, iobj_copy);
		PointerToRealItem(iobj_copy) = PointerToRealItem(pis->clipboard_item);

                new_item_pos = prop_item_insert(pis, INSERT_AFTER, iobj_copy);
		if (new_item_pos != -1)
                    ui_list_select_pos(pis->item_list, new_item_pos, True);
		else
		{
		    PointerToRealItem(iobj_copy) = NULL;
		    obj_destroy(iobj_copy);
		    util_dprintf(3, "prop_item_edit: could not add new item to List\n");
		}
            }
            break;
        case AB_EDIT_DELETE:
            delete_selected_item(pis);
            break;
        default:
            /* catch-all case to avoid compiler warnings */
        break;
    }
}

STRING
prop_item_get_namebase(
    ABObj	iobj
)
{
    STRING	namebase = NULL;
    static char namebuf[ITEM_NAME_MAX];
    STRING      starts, graphic_path;

    switch(iobj->label_type)
    {
 	case AB_LABEL_SEPARATOR:
            snprintf(namebuf, sizeof(namebuf), "%s", obj_get_name(iobj));
            starts = strstr(namebuf, SeparatorNamebase);
            namebase = strtok(starts, "_");
	    break;
	case AB_LABEL_GLYPH:
	    graphic_path = obj_get_label(iobj);
	    namebase = get_graphic_namebase(graphic_path);
	    break;
	case AB_LABEL_STRING:
	default:
            namebase = obj_get_label(iobj);
	    break;
    }
    return namebase;
}


int
prop_item_insert(
    PropItemsSetting	pis,
    INSERT_TYPE         itype,
    ABObj       	iobj
)
{
    ABObj       *iobj_list,
                *new_iobj_list;
    int         num_items;
    int         select_pos;
    int         pos = 0;
    int         i, j;

    XtVaGetValues(pis->item_list,
                XmNitemCount, &num_items,
                XmNuserData,  &iobj_list,
                NULL);

    select_pos = ui_list_get_selected_pos(pis->item_list);
    if (select_pos != -1)
    {
        switch(itype)
        {
            case INSERT_AFTER:
                pos = select_pos + 1;
                break;
            case INSERT_BEFORE:
                pos = select_pos;
                break;
            case INSERT_TOP:
                pos = 1;
                break;
            case INSERT_BOTTOM:
                pos = num_items + 1;
                break;
            default:
                /* catch-all case to avoid compiler warnings */
            break;
        }
    }    
    else /* First Item being added to List...*/
    {
        pos = 1;
	if (RequiresSelection(pis))
	    obj_set_is_initially_selected(iobj, True);

	activate_item_settings(pis, True);
    }

    ui_list_add_item(pis->item_list, obj_get_label(iobj), pos);
    num_items++;

    new_iobj_list = (ABObj *)util_malloc(num_items * sizeof(ABObj));

    for (i = j = 0; i < num_items; i++)
    {
        if (i == (pos - 1))
            new_iobj_list[i] = iobj;
        else
            new_iobj_list[i] = iobj_list[j++];
    }
    XtVaSetValues(pis->item_list, XmNuserData, new_iobj_list, NULL);

    util_free(iobj_list);

    return (pos);

}


/*
 ****************************************************************************
 * Private Support Routines
 ****************************************************************************
 */

static int
copy_item_fields(
    ABObj	iobj,
    ABObj	iobj_copy
)
{
    if (iobj != NULL && iobj_copy != NULL)
    {
    	obj_set_subtype(iobj_copy, obj_get_subtype(iobj));
    	obj_set_name(iobj_copy, obj_get_name(iobj));
    	obj_set_label(iobj_copy, obj_get_label(iobj));
    	obj_set_is_help_item(iobj_copy, obj_is_help_item(iobj));
    	iobj_copy->label_type = iobj->label_type;
	obj_set_is_initially_visible(iobj_copy, obj_is_initially_visible(iobj));
	obj_set_is_initially_active(iobj_copy, obj_is_initially_active(iobj));
	obj_set_is_initially_selected(iobj_copy, 
			obj_is_initially_selected(iobj));
        obj_set_menu_name(iobj_copy, obj_get_menu_name(iobj));
        obj_set_accelerator(iobj_copy, obj_get_accelerator(iobj));
        obj_set_mnemonic(iobj_copy, obj_get_mnemonic(iobj));
        obj_set_line_style(iobj_copy, obj_get_line_style(iobj));
        obj_set_bg_color(iobj_copy, obj_get_bg_color(iobj));
        obj_set_fg_color(iobj_copy, obj_get_fg_color(iobj));

    	return 0;
    }
    return -1;
}

static void
copy_item_to_clipboard(
    ABObj	iobj,
    ABObj	*item_clipboard
)
{
    ABObj	old_selection;
    ABObj	new_selection;

    old_selection = *item_clipboard;

    new_selection = obj_create(iobj->type, NULL);
    copy_item_fields(iobj, new_selection); 
    *item_clipboard = new_selection;

    if (old_selection != NULL)
    {
	PointerToRealItem(old_selection) = NULL;
	obj_destroy(old_selection);
    }

}

static BOOL
delete_selected_item(
    PropItemsSetting  pis
)
{
    ABObj	current_obj;
    ABObj       *iobj_list,
		*new_iobj_list;
    int		num_items;
    int		select_pos;
    int		i, j;

    current_obj = *(pis->current_obj_ptr);
 
    XtVaGetValues(pis->item_list,
                XmNuserData,    &iobj_list,
                NULL);

    select_pos = ui_list_get_selected_pos(pis->item_list);
 
    if (select_pos != -1)
    {
      	XtVaGetValues(pis->item_list, XmNitemCount, &num_items, NULL);
	if (num_items > 1)
	{
            new_iobj_list = (ABObj *)util_malloc((num_items - 1) * sizeof(ABObj));
	    if (new_iobj_list == NULL)
	    {
		util_dprintf(1, "delete_selected_item: malloc failed\n");
		return False;
	    }
	}

        for (i = j = 0; i < num_items; i++)
        {
            if (i == (select_pos - 1)) /* Found item-object being deleted */
	    {
		/* If the item-object being deleted is the one which is selected and
		 * it's not a Checkbox-item, then choose another item-object to
		 * be selected.
		 */
		if (num_items > 1 && 
		    obj_is_initially_selected(iobj_list[i]) &&
		    RequiresSelection(pis))
		{
		    if (i + 1 < num_items)
			obj_set_is_initially_selected(iobj_list[i+1], True);
		    else
			obj_set_is_initially_selected(iobj_list[0], True);
		}

		PointerToRealItem(iobj_list[i]) = NULL;
		obj_destroy(iobj_list[i]);
	    }
	    else
            	new_iobj_list[j++] = iobj_list[i];
    	}
    	XtVaSetValues(pis->item_list, XmNuserData, new_iobj_list, NULL);

        XmListDeletePos(pis->item_list, select_pos);
	if (num_items > 1)
	    XmListSelectPos(pis->item_list, 
		select_pos == num_items? select_pos - 1 : select_pos, TRUE);

	else /* No items left in list */
	    activate_item_settings(pis, False);

    	util_free(iobj_list);
	return TRUE;
    }   
    return FALSE;

}

static int
get_current_item_pos(
    PropItemsSetting	pis
)
{
    ABObj	*iobj_list;
    int		num_items = 0;
    int		i;
    int		current_pos = 0;

    XtVaGetValues(pis->item_list,
                XmNuserData,    &iobj_list,
                XmNitemCount,   &num_items,
                NULL);

    for(i=0; i < num_items; i++)
        if (iobj_list[i] == pis->current_item)
        {
            current_pos = i + 1;
            break;
        }

    return current_pos;
}

ABObj
get_selected_item_copy(
    Widget      list,
    int         *ppos
)
{
    ABObj       *iobj_list;
    int		select_pos;

    select_pos = ui_list_get_selected_pos(list);
    XtVaGetValues(list,
                XmNuserData,    &iobj_list,
                NULL);

    if (select_pos != -1)
    {
        *ppos = select_pos;
        return(iobj_list[(*ppos - 1)]);
    }
    return NULL;

}

static STRING
get_graphic_namebase(
    STRING      graphic_path
)
{
    static char namebase_buf[ITEM_NAME_MAX];

    if (util_strempty(graphic_path))
        return NULL;

    util_get_file_name_from_path(graphic_path, namebase_buf, ITEM_NAME_MAX);
    strcat(namebase_buf, "_graphic");

    return namebase_buf;
}

static STRING
get_unique_default_label(
    Widget	itemlist,
    STRING	base
)
{
    static char   label_buf[256];
    BOOL   	  unique = False;
    int	   	  i = 1;
    int		  pos;

    while (!unique)
    {
        sprintf(label_buf, "%s%d", base, i++);
	if (ui_list_find_item(itemlist, label_buf, &pos) == ERR_NOT_FOUND)
	    unique = True;
    }
    return label_buf;
}

static ABObj
init_new_item(
    PropItemsSetting	pis
)
{
    ABObj current_obj;
    ABObj new_iobj;
    STRING newlabel;

    if ((current_obj = *(pis->current_obj_ptr)) == NULL)
        return NULL;

    new_iobj = obj_create(AB_TYPE_ITEM, NULL);
    obj_set_subtype(new_iobj, pis->item_type);

    new_iobj->label_type = AB_LABEL_STRING;
    newlabel = get_unique_default_label(pis->item_list,
        catgets(Dtb_project_catd, 100, 268, "Item"));
    obj_set_label(new_iobj, newlabel);

    abobj_set_item_name(new_iobj, obj_get_module(current_obj),
                obj_get_name(current_obj), newlabel);

    if (HasSelectedState(pis->item_type))
            obj_set_is_initially_selected(new_iobj, False);
    if (HasActiveState(pis->item_type))
            obj_set_is_initially_active(new_iobj, True);
    if (HasHelpState(pis->item_type))
        obj_set_is_help_item(new_iobj, False);

    obj_set_bg_color(new_iobj, obj_get_bg_color(current_obj));
    obj_set_fg_color(new_iobj, obj_get_fg_color(current_obj));

    return(new_iobj);
}


/*
 * Load the Item obj into the item editing fields
 * (if iobj == NULL then perform a clear)
 */
static void
load_item(
    PropItemsSetting	pis,
    ABObj		iobj
)
{
    AB_LABEL_TYPE	label_type = AB_LABEL_STRING;
    STRING 		label = NULL;

    if (iobj != NULL && !obj_is_item(iobj))
	return;

    /* Load Item Label Type */
    if (HasLabelType(pis->item_type))
    {
	label_type = iobj? iobj->label_type : AB_LABEL_STRING;
    	prop_options_set_value(pis->item_label_type_pos,
                               (XtPointer)label_type, False);
    }

    /* Load Item Label (make selected & with focus) */
    if (label_type != AB_LABEL_SEPARATOR)
    {
    	label = iobj? obj_get_label(iobj) : "";
	setup_item_label_setting(pis, iobj->label_type, label, AB_LINE_UNDEF);
	ui_field_select_string(pis->item_label_pfs->field, True);
    }
    else  /* AB_LABEL_SEPARATOR */
	setup_item_label_setting(pis, iobj->label_type, 
		"", iobj? obj_get_line_style(iobj) : AB_LINE_NONE); 

    /* Load Mnenonic/Accelerator */
    if (HasMnemonic(pis->item_type))
	prop_field_set_value(pis->item_mnemonic_pfs, obj_get_mnemonic(iobj), False);

    if (HasAccelerator(pis->item_type))
	prop_field_set_value(pis->item_accel_pfs, obj_get_accelerator(iobj), False);

    /* Load Item State */
    if (HasActiveState(pis->item_type))
    	prop_checkbox_set_value(pis->item_state_pcs, AB_STATE_ACTIVE, 
		iobj? obj_is_initially_active(iobj) : True, False);

    if (HasSelectedState(pis->item_type))
    	prop_checkbox_set_value(pis->item_state_pcs, AB_STATE_SELECTED, 
		iobj? obj_is_initially_selected(iobj) : False, False);

    if (HasHelpState(pis->item_type))
	prop_checkbox_set_value(pis->item_state_pcs, HELP_ITEM_KEY, 
		iobj? obj_is_help_item(iobj) : False, False);

    /* Load Item Submenu */
    if (HasSubmenu(pis->item_type))
	prop_menuname_set_value(pis->item_menuname_pms, 
		iobj? obj_get_menu_name(iobj) : "", False);

}

static void
activate_item_settings(
    PropItemsSetting	pis,
    BOOL		active
)
{
    Widget	graphic_hint = NULL;

    ui_set_active(pis->item_label_pfs->label, active);
    if (!active)
	ui_field_set_string(pis->item_label_pfs->field, "");
    ui_field_set_editable(pis->item_label_pfs->field, active);

    if (HasLabelType(pis->item_type))
    {
	if (pis->item_label_type_pos->label)
            ui_set_active(pis->item_label_type_pos->label, active);

        ui_set_active(pis->item_label_type_pos->optionbox, active);

	XtVaGetValues(pis->item_label_pfs->label,
		XmNuserData,	&graphic_hint,
		NULL);
	if (graphic_hint)
	    ui_set_active(graphic_hint, active);
    }

    if (HasLabelLineStyle(pis->item_type))
    {
        ui_set_active(pis->item_line_style_pos->label, active);
        ui_set_active(pis->item_line_style_pos->optionbox, active);
    }

    if (pis->item_state_pcs != NULL)
    {
	if (pis->item_state_pcs->label)
	    ui_set_active(pis->item_state_pcs->label, active);
	ui_set_active(pis->item_state_pcs->checkbox, active);
    }

    activate_menu_item_settings(pis, active);
}

static void
activate_menu_item_settings(
    PropItemsSetting    pis,
    BOOL                active
)
{
    if (HasMnemonic(pis->item_type))
    {
        ui_set_active(pis->item_mnemonic_pfs->label, active);
        ui_set_active(pis->item_mnemonic_pfs->field, active);
    }
    if (HasAccelerator(pis->item_type))
    {
        ui_set_active(pis->item_accel_pfs->label, active);
        ui_set_active(pis->item_accel_pfs->field, active);
    }
    if (HasSubmenu(pis->item_type))
    {
        ui_set_active(pis->item_menuname_pms->label, active);
        ui_set_active(pis->item_menuname_pms->field, active);
        ui_set_active(pis->item_menuname_pms->menubutton, active);
    }
}


static void
setup_item_label_setting(
    PropItemsSetting	pis,
    AB_LABEL_TYPE	label_type,
    STRING		label,
    AB_LINE_TYPE	label_line_style
)
{
    BOOL	no_sep_label = True;

    prop_setup_label_field(pis->item_label_pfs, pis->item_line_style_pos,
                label_type, label, label_line_style);

    no_sep_label = (label_type != AB_LABEL_SEPARATOR);

    XtVaSetValues(pis->item_label_pfs->field, 
		XmNcursorPositionVisible, no_sep_label,
		NULL);

    activate_menu_item_settings(pis, no_sep_label);
}

static void
verify_and_set_item_selected(
    PropItemsSetting	pis,
    BOOL		selected
)
{
    ABObj	current_obj;
    ABObj	*iobj_list;
    int		num_items;
    int		i;

    XtVaGetValues(pis->item_list,
                XmNuserData,  &iobj_list,
                XmNitemCount, &num_items,
                NULL);

    current_obj = *(pis->current_obj_ptr);

    switch(pis->item_type)
    {
	case AB_ITEM_FOR_CHOICE:
	    if (obj_get_subtype(current_obj) == AB_CHOICE_NONEXCLUSIVE)
		break;
	    /* else fall through...*/
	case AB_ITEM_FOR_COMBO_BOX:
	case AB_ITEM_FOR_SPIN_BOX:
	case AB_ITEM_FOR_LIST:
	    if (selected)	
	    {
		if (pis->item_type != AB_ITEM_FOR_LIST ||
		    ((obj_get_selection_mode(current_obj) == AB_SELECT_SINGLE) ||
		     (obj_get_selection_mode(current_obj) == AB_SELECT_BROWSE)))
	  	{
		    /* make sure only 1 Item is selected at a time...*/
	    	    for (i=0; i < num_items; i++)
		   	if (iobj_list[i] != pis->current_item &&
				obj_is_initially_selected(iobj_list[i]) == True)
		           obj_set_is_initially_selected(iobj_list[i], False);
	    	}
	    }
	    break;
	default:
	    break;
		
    }
    obj_set_is_initially_selected(pis->current_item, selected);

}

/*
 ****************************************************************************
 * Callbacks
 ****************************************************************************
 */

static void
edit_itemCB(
    Widget	widget,
    XtPointer	client_data,
    XtPointer   call_data
)
{
    PropItemsSetting    pis = (PropItemsSetting)client_data; 
    int			current_item_pos = 0;
    AB_EDIT_TYPE 	etype;

    XtVaGetValues(widget, XmNuserData, &etype, NULL);

    if (etype == EDIT_CHANGE)
    {
	current_item_pos = get_current_item_pos(pis);
  	prop_item_change(pis, False);
	ui_list_select_pos(pis->item_list, current_item_pos, False);
    }
    else
    {
        prop_item_edit(pis, etype);
    	ui_field_select_string(pis->item_label_pfs->field, False);
    }
    prop_set_changebar(pis->changebar, PROP_CB_ON);
    XmProcessTraversal(pis->item_label_pfs->field, XmTRAVERSE_CURRENT);

}
static void
field_activateCB(
    Widget      widget,
    XtPointer   client_data,
    XtPointer   call_data
)
{
    PropItemsSetting	pis = (PropItemsSetting)client_data;

    prop_item_change(pis, True);

}
static void
add_itemCB(
    Widget	widget,
    XtPointer	client_data,
    XtPointer	call_data
)
{
    PropItemsSetting    pis = (PropItemsSetting)client_data;
    ABObj	 iobj_copy;
    INSERT_TYPE  itype;
    int		 new_item_pos;

    if (*(pis->current_obj_ptr) == NULL)
	return;

    XtVaGetValues(widget, XmNuserData, &itype, NULL);

    iobj_copy = init_new_item(pis);
    new_item_pos = prop_item_insert(pis, itype, iobj_copy);
    if (new_item_pos != -1)
    {
    	ui_list_select_pos(pis->item_list, new_item_pos, True);
    	pis->current_item = iobj_copy;
    	prop_set_changebar(pis->changebar, PROP_CB_ON);

    	ui_field_select_string(pis->item_label_pfs->field, True);
    }
    else /* Error in adding item to list */
    {
	PointerToRealItem(iobj_copy) = NULL;
	obj_destroy(iobj_copy);
	util_dprintf(3, "add_itemCB: could not add new item to List\n");
    }

}


static void
select_itemCB(
    Widget 	widget,
    XtPointer 	client_data,
    XtPointer   call_data
)
{
    XmListCallbackStruct *listdata = (XmListCallbackStruct *)call_data;
    PropItemsSetting	pis = (PropItemsSetting)client_data;
    ABObj	 	iobj_copy;
    int		 	pos;

    iobj_copy = get_selected_item_copy(pis->item_list, &pos);

    if (pis->current_item != NULL &&
	pis->current_item != iobj_copy)
    {
        /* Need to Save previous item's edits... */
	prop_item_change(pis, False);
    }
    load_item(pis, iobj_copy);
    pis->current_item = iobj_copy;
 
}

static void
set_edit_menu_stateCB(
    Widget      widget,
    XtPointer   client_data,
    XtPointer   call_data
)
{
    PropItemsSetting    pis = (PropItemsSetting)client_data;
    BOOL		listitem_exists = False;
    BOOL		clipboard_full = False;
    WidgetList		menu_items;
    int			num_menu_items;
    int			item_key;
    int			i;

    listitem_exists = (pis->current_item != NULL);
    clipboard_full = (pis->clipboard_item != NULL);

    XtVaGetValues(pis->item_menu,
		XmNnumChildren,	&num_menu_items,
		XmNchildren,	&menu_items,
		NULL);

    /* Loop through the Edit menuitems, setting their sensitivity 
     * according to the state of the Item editor
     */
    for (i = 0; i < num_menu_items; i++)
    {
	XtVaGetValues(menu_items[i], XmNuserData, &item_key, NULL);

	switch(item_key)
  	{
	    case AB_EDIT_CUT:
	    case AB_EDIT_COPY:
	    case AB_EDIT_DELETE:
	    case EDIT_CHANGE:
		ui_set_active(menu_items[i], listitem_exists);
		break;
	    case AB_EDIT_PASTE:
		ui_set_active(menu_items[i], clipboard_full);
		break;
	    case INSERT_AFTER:
	    case INSERT_BEFORE:
	    default:
		break;
		/* do nothing */
	}
    }
}

void
prop_item_labeltypeCB(
    Widget   item,
    XtPointer clientdata,
    XtPointer call_data
)
{
    AB_LABEL_TYPE                value = AB_LABEL_UNDEF;
    PropItemsSetting    	 pis = (PropItemsSetting)clientdata;

    XtVaGetValues(item, XmNuserData, &value, NULL);

    setup_item_label_setting(pis, (AB_LABEL_TYPE)value, NULL, AB_LINE_UNDEF);

    if (value != AB_LABEL_SEPARATOR)
	ui_field_select_string(pis->item_label_pfs->field, True);

}
