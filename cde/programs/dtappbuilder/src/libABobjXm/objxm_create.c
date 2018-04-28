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
 *      $XConsortium: objxm_create.c /main/4 1995/11/06 18:45:52 rswiston $
 *
 * @(#)objxm_create.c	1.45 18 Jan 1994      cde_app_builder/src/libABobjXm
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
 * objxm_create.c - instantiates AB objects into motif widgets
 *    
 *
 ***********************************************************************
 */

#include <stdio.h>
#include <ab_private/XmAll.h>
#include <Dt/TermPrim.h>
#include <Dt/Term.h>
#include <ab_private/trav.h>
#include "objxmP.h"

#define	MAX_FORMS  	24


/*************************************************************************
**                                                                      **
**       Private Function Declarations 					**
**                                                                      **
**************************************************************************/
static int	xm_instantiate_tree(
    		    ABObj    root,
    		    BOOL     map 
		);
static int	xm_instantiate_obj(
    		    ABObj    obj,
    		    BOOL     map
		);
static int	xm_instantiate_menu(
		    ABObj	obj,
		    Widget	owner,
		    Widget	parent,
		    BOOL	submenu
		);
static int      post_instantiate_tree(
                    ABObj    root,
                    BOOL     map 
                );
static int      post_instantiate_obj(
                    ABObj    obj, 
                    BOOL     map
		); 
static Widget	instantiate_widget(
		    ABObj	obj,
		    Widget	parent,
		    BOOL	map
		);
static int	xm_map_obj(
		    ABObj	obj
		);
static int	xm_unmap_obj(
		    ABObj	obj
		);
static WidgetClass
		xm_get_class(
    		    ISTRING    class_name
		);

static int	formlist_append(
    		    Widget      newform
		);

static int	formlist_force_resize(
		);

/*
 * Event Handlers 
 */
static void	xm_popup_menu(
		    Widget	w,
		    XtPointer	client_data,
		    XEvent	*ev,
		    Boolean	*cont
		);


/*************************************************************************
**                                                                      **
**       Global  Data                                                   **
**                                                                      **
**************************************************************************/

static    Widget form_list[MAX_FORMS];
static 	  int	 form_list_count = 0;

/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/

int
objxm_fully_instantiate_tree(
    ABObj    root,
    BOOL     map 
)
{

    /* Instantiate all widgets 
     */
    xm_instantiate_tree(root, FALSE);

    /* Set resources that must be set AFTER all widgets
     * are instantiated.
     */
    post_instantiate_tree(root, map);

    /* WORKAROUND for Motif XmForm bug (it ignores child resize request
     * if x,y have not also changed). So, we have to force it.
     */
    formlist_force_resize();

    return OK;
}

int
objxm_map_tree(
    ABObj	root,
    BOOL	map
)
{
    AB_TRAVERSAL trav;
    ABObj	 obj;

    if (root == NULL)
	return -1;

/*
    for (trav_open(&trav, root, AB_TRAV_CHILDREN);
             (obj = trav_next(&trav)) != NULL; )
	objxm_map_tree(obj, map);
*/

    if (map)
	xm_map_obj(root);
    else
	xm_unmap_obj(root);

    return 0;

}

static int
xm_map_obj(
    ABObj	obj
)
{
    Widget	widget;

    if (!obj_has_flag(obj, InstantiatedFlag))
    {
	if (util_get_verbosity() > 2)
	    fprintf(stderr,"xm_map_obj: %s must be instantiated first\n",
		obj_get_name(obj));
	return -1;
    }
    if (!obj_has_flag(obj, MappedFlag))
    {
	/* If the object has an associated widget and is NOT a menu-pane,
	 * then Manage it...
	 */
	if ((widget = (Widget)obj->ui_handle) != NULL) 
	{
            if (XtIsSubclass(widget, topLevelShellWidgetClass))
            	XtPopup(widget, XtGrabNone);

	    /* Manage everything but menushells & menupanes */
	    else if (!objxm_is_menu_widget(widget))
		XtSetMappedWhenManaged(widget, TRUE);
	}
	obj_set_flag(obj, MappedFlag);
    }
    return 0;

}

static int
xm_unmap_obj(
    ABObj	obj
)
{
    Widget	widget;

    if (obj_has_flag(obj, MappedFlag)) 
    { 
        if ((widget = (Widget)obj->ui_handle) != NULL)
        { 
            if (XtIsSubclass(widget, topLevelShellWidgetClass)) 
                XtPopdown(widget); 
            else if (!objxm_is_menu_widget(widget))
		XtSetMappedWhenManaged(widget, FALSE);
        }
        obj_clear_flag(obj, MappedFlag); 
    } 
    return 0; 
 
} 

static int
xm_instantiate_tree(
    ABObj    root,
    BOOL     map 
)
{
    AB_TRAVERSAL    trav;
    ABObj           child;

    if (root == NULL)
        return -1;
    
    /* Top-down traversal is crucial */
    xm_instantiate_obj(root, map);
    for (trav_open(&trav, root, AB_TRAV_CHILDREN);
        (child = trav_next(&trav)) != NULL; )
    {
    	xm_instantiate_tree(child, map);
    }
    trav_close(&trav);

    return 0;

}

static int
post_instantiate_tree(
    ABObj    root,
    BOOL     map 
)
{
    AB_TRAVERSAL    trav;
    ABObj           child;
 
    if (root == NULL)
        return -1;
    
    /* Bottom-up traversal is crucial*/
    for (trav_open(&trav, root, AB_TRAV_UI);
	(child = trav_next(&trav)) != NULL; )
	post_instantiate_obj(child, map);

    trav_close(&trav);
 
    return 0;
}

static int
xm_instantiate_obj(
    ABObj    obj,
    BOOL     map 
)
{
    ABObj	parentobj;
    Widget	widget;
    Widget	parent;
    
    if (!obj_has_flag(obj, XmConfiguredFlag) ||
	obj_has_flag(obj, InstantiatedFlag) ||
	obj_has_flag(obj, BeingDestroyedFlag))
        return OK; /* not considered an error */

    if (obj_is_menu_ref(obj))
	return OK;

    if (obj_is_virtual(obj))
    {
	obj_set_flag(obj, InstantiatedFlag);
	if (map)
	    xm_map_obj(obj);
	return OK;
    }

    /* List Items are not separate widgets */
    if (obj_is_item(obj) && obj->info.item.type == AB_ITEM_FOR_LIST)
    {
	XmString xmitem;

	if ((parent = (Widget)(obj_get_parent(obj)->ui_handle)) != NULL)
	{
	    xmitem = XmStringCreateLocalized(obj_get_label(obj));
	    XmListAddItem(parent, xmitem, 0);
	    XmStringFree(xmitem);

	    obj_set_flag(obj, InstantiatedFlag);
            if (map) 
            	xm_map_obj(obj); 
	    return OK;
	}
	else
	    return ERROR;
    }
    /* Determine parent.. */
    /* If obj is a window, parent it off the AB toplevel during
     * build-mode to ensure it's independence during UI editing
     */
    if (obj_is_window(obj))
        parent = Objxm_toplevel;
    else
    {
        parentobj = obj_get_parent(obj);
 
        while (obj_is_virtual(parentobj))
            parentobj = obj_get_parent(parentobj);
 
        parent = (Widget)parentobj->ui_handle;
    }
    if (parent == NULL)
    {
        if (util_get_verbosity() > 2)
        fprintf(stderr,"xm_instantiate_obj: %s: ui_handle for parent is NULL\n",
                    istr_string_safe(obj->name));
	return ERROR;
    }

    widget = instantiate_widget(obj, parent, map);

    if (widget != NULL)
    {

	/* If we have an OptionMenu menupane, we need to hook it
	 * up to the XmOptionMenu (XmRowColumn) in time for the
	 * XmOptionMenu's instantiation...ugly, but haven't found
	 * a better place to do this yet...
	 */
	if (objxm_is_menu_widget(widget) && 
	    XtIsSubclass(widget, xmRowColumnWidgetClass))
	{
	    ABObj	robj = obj_get_comp_rootobj(obj);
	    if (obj_is_choice(robj)) 
	    {
		ABObj	o_obj = objxm_get_config_obj(robj, AB_CFG_OBJECT_OBJ);
		objxm_set_ui_arg(o_obj, AB_ARG_WIDGET, XmNsubMenuId, widget);
	    }
	}

	/* if widget is being replaced, destroy old one */
	if (obj->ui_handle != NULL)
	    objxm_uninstantiate_tree(obj, TRUE);

	/* obj & widget point to each other...*/
        XtVaSetValues(widget, XmNuserData, obj, NULL);
	obj->ui_handle = (void*)widget;

	objxm_remove_all_ui_args(obj);
	obj_set_flag(obj,   InstantiatedFlag);
 	obj_clear_flag(obj, AttrChangedFlag);

        return OK;
    }

    return ERROR;

}

int
objxm_instantiate_menus(
    ABObj	root
)
{
    AB_TRAVERSAL    trav;
    ABObj           obj;
    ABObj	    m_parent;
 
    if (root == NULL)
        return -1;
   
    for (trav_open(&trav, root, AB_TRAV_UI);
        (obj = trav_next(&trav)) != NULL; )
  	/* Instantiate a menu for the object if one exists.
	 * NOTE: SubMenus (menus off of other menu-items) are 
	 * actually instantiated using recursion in xm_instantiate_menu(),
	 * thus we do not call xm_instantiate_menu() for them directly.
	 */
    	if (obj_is_salient(obj) &&
	    obj_get_menu_name(obj) != NULL && !obj_is_menu_item(obj))
    	{
	    m_parent = objxm_get_config_obj(obj, AB_CFG_MENU_PARENT_OBJ);
	    xm_instantiate_menu(obj, (Widget)obj->ui_handle, 
				(Widget)m_parent->ui_handle, FALSE);
        }

    trav_close(&trav);
    return 0;

}

/* 
 * Since we support the concept of virtual "Sharable" menus (the same Menu
 * object can be attached to multiple objects) and Motif does NOT, we must
 * use special handling when instantiating menus.  Although the virtual or
 * "shared" Menu obj is used to determine the structure of the menu, the actual
 * widget ID of the menu is stored in the Menu-Reference obj attached directly
 * to the object which "owns" a copy of the menu. 
 *
 * Both "owner" & "parent" are required because for casacdes, Motif requires a 
 * menu to be parented off the *parent* of the cascade...
 *
 * NOTE: for menus containing cascading sub-menus, ONLY the TOP-MOST
 * 	 menu is stored in the Menu-Reference obj.
 */
static int 
xm_instantiate_menu(
    ABObj	obj,
    Widget	owner, 
    Widget	parent,
    BOOL	submenu
)
{
    AB_TRAVERSAL trav;
    AB_MENU_TYPE m_type;
    ABObj	menu_ref;
    ABObj	menu;
    ABObj	item;
    Widget	menu_w;
    Widget	item_w;

    if ((menu_ref = objxm_get_config_obj(obj, AB_CFG_MENU_OBJ)) != NULL)
    {
	/* Get Virtual Menu & object to parent menu off of */
	menu = obj_get_ref_menu(menu_ref);

    	if (menu != NULL && parent != NULL)
    	{
	    /* Set Pseudo-class for Menu based on the type
	     * of object that owns it
	     */
	    if (obj_is_item(obj) && 
		(obj->info.item.type == AB_ITEM_FOR_MENU ||
		 obj->info.item.type == AB_ITEM_FOR_MENUBAR))
	    {
		m_type = AB_MENU_PULLDOWN;
		obj_set_class_name(menu, _xmPulldownMenu);
	    }
	    else
	    {
		m_type = AB_MENU_POPUP;
		obj_set_class_name(menu, _xmPopupMenu);
	    }
	    menu_w = instantiate_widget(menu, parent, TRUE);

	    if (m_type == AB_MENU_PULLDOWN)
	    	XtVaSetValues(owner, XmNsubMenuId, menu_w, NULL);
	    else /* PopupMenu */
		XtAddEventHandler(parent, ButtonPressMask, False, xm_popup_menu, obj);

	    /* Instantiate Menu Items */
	    for (trav_open(&trav, menu, AB_TRAV_CHILDREN);
            (item = trav_next(&trav)) != NULL; )
	    {
	    	item_w = instantiate_widget(item, menu_w, TRUE);

		/* If SubMenu exists, instantiate it */
		if (obj_get_menu_name(item) != NULL)
		    xm_instantiate_menu(item, item_w, menu_w, TRUE);
	    }
	    trav_close(&trav);

	    /* ONLY store widget-ids for top-most menus */
	    if (!submenu)
	    	menu_ref->ui_handle = (void*)menu_w;
	}
	else 
	{
	    if (util_get_verbosity() > 2)
		fprintf(stderr,"xm_instantiate_menu: could not instantiate menu for %s\n",
			istr_string_safe(obj->name));
	    return -1;
	}
    }
    return 0;
}


int
objxm_destroy_menus(
    ABObj	root
)
{
    AB_TRAVERSAL    trav;
    ABObj           obj;
    ABObj	    menu;
    AB_MENU_TYPE    m_type;
    ABObj           menu_ref;
    ABObj	    menu_p;
 
    if (root == NULL)
        return -1;
   
    for (trav_open(&trav, root, AB_TRAV_UI);
        (obj = trav_next(&trav)) != NULL; )
        if (obj_is_salient(obj) && !obj_is_menu_item(obj) &&
	   (menu_ref = objxm_get_config_obj(obj, AB_CFG_MENU_OBJ)) != NULL)
        {
	    if (menu_ref->ui_handle != NULL)
	    {
		menu = obj_get_ref_menu(menu_ref);

	    	if (obj_is_item(obj) &&
                    (obj->info.item.type == AB_ITEM_FOR_MENU ||
                     obj->info.item.type == AB_ITEM_FOR_MENUBAR))
                    m_type = AB_MENU_PULLDOWN;
            	else
                    m_type = AB_MENU_POPUP;

		if (m_type == AB_MENU_POPUP) /* Remove EventHandler that pops up menu */
		{
		    menu_p = objxm_get_config_obj(obj, AB_CFG_MENU_PARENT_OBJ);
		    XtRemoveEventHandler((Widget)menu_p->ui_handle, ButtonPressMask, 
			False, xm_popup_menu, obj);
		}
/*
fprintf(stderr,"DESTROY_MENU: %s\n", XtName(menu_ref->ui_handle));
*/
		XtDestroyWidget((Widget)menu_ref->ui_handle);
		menu_ref->ui_handle = (void*)NULL;
	    }
        }
 
    trav_close(&trav);
    return 0;
 
}

static Widget
instantiate_widget(
    ABObj	obj,
    Widget	parent,
    BOOL	map
)
{
    Widget       widget = NULL;
    WidgetClass  xmclass;
    XmCreateFunc xmcreate_func;
    STRING	 classname;
    String       name;
    int        	 num_args;
    ArgList      all_args = NULL;

    name = (String)obj_get_name(obj);
    if (name == NULL)
    {
        if (util_get_verbosity() > 2)
            fprintf(stderr,"instantiate_widget: object must have name\n");
        return NULL;
    }

    if ((classname = obj_get_class_name(obj)) == NULL)
    {
	classname = objxm_get_default_motif_class(obj);
	obj_set_class_name(obj, classname);
    }

    if (obj->ui_args == NULL) /* Make sure Resources are set */
        objxm_set_obj_ui_args(obj, OBJXM_CONFIG_BUILD, TRUE);

    /* Instantiate Widget using class & arglist
     */
    if (istr_equal(obj->class_name, istr_const(_topLevelShell)))
    {
	all_args = objxmP_merge_arglists((ArgList)obj->ui_args, 
				objxm_get_num_args((ArgList)obj->ui_args),
				(ArgList)obj->tk_args,
				objxm_get_num_args((ArgList)obj->tk_args));
	num_args = objxm_get_num_args(all_args);

    	widget = XtCreatePopupShell(name,
        		topLevelShellWidgetClass,
        		parent,
        		all_args,
        		num_args);
    }
    else  /* Non TopLevel */
    {
	objxm_set_ui_arg(obj, AB_ARG_BOOLEAN, XtNmappedWhenManaged, map);

        all_args = objxmP_merge_arglists((Arg *)obj->ui_args, 
                	objxm_get_num_args((Arg *)obj->ui_args),
                	(Arg *)obj->tk_args, 
                	objxm_get_num_args((Arg *)obj->tk_args)); 

        num_args = objxm_get_num_args(all_args); 

/*
fprintf(stderr,"INSTANTIATING: %s part_of(%s), class=%s\n", name, 
obj->part_of? istr_string_safe(obj->part_of->name) : "null",obj_get_class_name(obj));
objxm_dump_arglist(obj, all_args, num_args);
*/

        xmcreate_func = objxm_get_xmcreate_func(obj);

        if (xmcreate_func != NULL)
            widget = (*xmcreate_func)(parent, name, all_args, num_args);
    
        else
	{
	    xmclass  = (WidgetClass)xm_get_class(obj->class_name);
            widget = XtCreateWidget(name, xmclass, parent, all_args, num_args);
	}

	if (!objxm_is_menu_widget(widget))
	    XtManageChild(widget);

    }
    XtFree((char *)all_args);

    return widget;

} 

static int
post_instantiate_obj(
    ABObj	obj,
    BOOL	map
)
{
    Widget	widget;
    ABObj	parent_obj;
    ArgList	args;
    int		num_args;
    Dimension   w,h;

    widget = (Widget)obj->ui_handle;

    if (widget == NULL)
	return 0;

    if (obj_is_salient(obj))
    	objxmP_type_post_instantiate(obj);

    if (obj->attachments != NULL)
    	objxm_set_attachment_args(obj);

    if (!obj_is_window(obj))
    {
	parent_obj = obj_get_parent(obj);

        if (XtIsSubclass((Widget)parent_obj->ui_handle, xmFormWidgetClass))
	    formlist_append((Widget)parent_obj->ui_handle);
    }

    if (obj_has_flag(obj,AttrChangedFlag))
    {
	args = (ArgList)obj->ui_args;
        num_args = objxm_get_num_args(args);

/*
fprintf(stderr,"(PI) CHANGE_ATTRS: %s\n", istr_string_safe(obj->name));
objxm_dump_arglist(obj, args, num_args); 
*/

        XtSetValues(widget, args, num_args);
        obj_clear_flag(obj, AttrChangedFlag);
        objxm_remove_all_ui_args(obj);
    }
    if (map)
	xm_map_obj(obj);

    return 0;

}

int
objxmP_destroy( 
    ObjEvDestroyInfo    info 
) 
{
    Widget	widget = (Widget)info->obj->ui_handle;

    if (widget != NULL)
    {
/*
fprintf(stderr,"DESTROY: %s\n", istr_string_safe(info->obj->name));
*/
	XtDestroyWidget(widget);
	info->obj->ui_handle = NULL;
    }
    return 0;

}

int
objxm_destroy_tree(
    ABObj	root
)
{

    if (root == NULL)
	return -1;

    /* Destroy any existing widgets */
    objxm_uninstantiate_tree(root, TRUE);

    /* Destroy ABObj tree */
    obj_destroy(root);

    return 0;
}

int
objxm_uninstantiate_tree(
    ABObj	root,
    BOOL	topmost
)
{
    AB_TRAVERSAL    trav;
    ABObj           child;

    if (root == NULL || root->ui_handle == NULL)
        return -1;

    if (topmost)
{
/*
fprintf(stderr,"DESTROY2: %s\n", istr_string_safe(root->name));
*/
	XtDestroyWidget((Widget)root->ui_handle);
}

    for (trav_open(&trav, root, AB_TRAV_CHILDREN);
        (child = trav_next(&trav)) != NULL; )
    {
        objxm_uninstantiate_tree(child, FALSE);
    }
    root->ui_handle = NULL;
    obj_clear_flag(root, InstantiatedFlag);
    obj_clear_flag(root, MappedFlag);

    trav_close(&trav);

    return OK;
 
}


static WidgetClass
xm_get_class(
    ISTRING    class_name
)
{
    WidgetClass    wclass;

        /* REMIND: aim,8/25/93 - change to a switch statement? */

    if (istr_equal(class_name, istr_const(_topLevelShell)))
        wclass = topLevelShellWidgetClass;
    else if (istr_equal(class_name, istr_const(_dtTerm)))
	wclass = dtTermWidgetClass;
    else if (istr_equal(class_name, istr_const(_xmArrowButton)))
        wclass = xmArrowButtonWidgetClass;
    else if (istr_equal(class_name, istr_const(_xmBulletinBoard))) 
        wclass = xmBulletinBoardWidgetClass; 
    else if (istr_equal(class_name, istr_const(_xmCascadeButton))) 
        wclass = xmCascadeButtonWidgetClass; 
    else if (istr_equal(class_name, istr_const(_xmCommand))) 
        wclass = xmCommandWidgetClass; 
    else if (istr_equal(class_name, istr_const(_xmDialogShell))) 
        wclass = xmDialogShellWidgetClass; 
    else if (istr_equal(class_name, istr_const(_xmDrawingArea))) 
        wclass = xmDrawingAreaWidgetClass; 
    else if (istr_equal(class_name, istr_const(_xmDrawnButton)))
	wclass = xmDrawnButtonWidgetClass;
    else if (istr_equal(class_name, istr_const(_xmFileSelectionBox))) 
        wclass = xmFileSelectionBoxWidgetClass; 
    else if (istr_equal(class_name, istr_const(_xmForm))) 
        wclass = xmFormWidgetClass; 
    else if (istr_equal(class_name, istr_const(_xmFrame))) 
        wclass = xmFrameWidgetClass; 
    else if (istr_equal(class_name, istr_const(_xmLabel))) 
        wclass = xmLabelWidgetClass; 
    else if (istr_equal(class_name, istr_const(_xmLabelGadget))) 
	wclass = xmLabelGadgetClass; 
    else if (istr_equal(class_name, istr_const(_xmList))) 
        wclass = xmListWidgetClass; 
    else if (istr_equal(class_name, istr_const(_xmMainWindow))) 
        wclass = xmMainWindowWidgetClass; 
    else if (istr_equal(class_name, istr_const(_xmManager))) 
        wclass = xmManagerWidgetClass; 
    else if (istr_equal(class_name, istr_const(_xmMenuShell))) 
        wclass = xmMenuShellWidgetClass; 
    else if (istr_equal(class_name, istr_const(_xmMessageBox))) 
        wclass = xmMessageBoxWidgetClass; 
    else if (istr_equal(class_name, istr_const(_xmPanedWindow))) 
        wclass = xmPanedWindowWidgetClass; 
    else if (istr_equal(class_name, istr_const(_xmPopupMenu)))
	wclass = xmMenuShellWidgetClass;
    else if (istr_equal(class_name, istr_const(_xmPrimitive))) 
        wclass = xmPrimitiveWidgetClass; 
    else if (istr_equal(class_name, istr_const(_xmPulldownMenu)))
	wclass = xmMenuShellWidgetClass;
    else if (istr_equal(class_name, istr_const(_xmPushButton))) 
        wclass = xmPushButtonWidgetClass; 
    else if (istr_equal(class_name, istr_const(_xmRowColumn))) 
        wclass = xmRowColumnWidgetClass; 
    else if (istr_equal(class_name, istr_const(_xmScale))) 
	wclass = xmScaleWidgetClass; 
    else if (istr_equal(class_name, istr_const(_xmScrollBar))) 
        wclass = xmScrollBarWidgetClass; 
    else if (istr_equal(class_name, istr_const(_xmScrolledWindow))) 
        wclass = xmScrolledWindowWidgetClass; 
    else if (istr_equal(class_name, istr_const(_xmSelectionBox))) 
        wclass = xmSelectionBoxWidgetClass; 
    else if (istr_equal(class_name, istr_const(_xmSeparator))) 
        wclass = xmSeparatorWidgetClass; 
    else if (istr_equal(class_name, istr_const(_xmSeparatorGadget))) 
        wclass = xmSeparatorGadgetClass; 
    else if (istr_equal(class_name, istr_const(_xmText))) 
        wclass = xmTextWidgetClass; 
    else if (istr_equal(class_name, istr_const(_xmTextField))) 
        wclass = xmTextFieldWidgetClass; 
    else if (istr_equal(class_name, istr_const(_xmToggleButton))) 
        wclass = xmToggleButtonWidgetClass; 
    else if (istr_equal(class_name, istr_const(_xmToggleButtonGadget))) 
        wclass = xmToggleButtonGadgetClass; 

    else    /* Unknown Widget class */
	wclass = NULL;

    return(wclass);
}

XmCreateFunc
objxm_get_xmcreate_func(
    ABObj    obj
)
{
    ABObj	   robj = obj_get_comp_rootobj(obj);
    ISTRING        class_name = obj->class_name;
    XmCreateFunc   xmcreate_func;

    if (class_name == NULL)
        return NULL;

    /* REMIND: aim,8/25/93 - change to a switch statement? */

    if (istr_equal(class_name, istr_const(_xmArrowButton)))
	xmcreate_func = XmCreateArrowButton;
    else if (istr_equal(class_name, istr_const(_xmBulletinBoard))) 
        xmcreate_func = XmCreateBulletinBoard; 
    else if (istr_equal(class_name, istr_const(_xmCascadeButton))) 
        xmcreate_func = XmCreateCascadeButton; 
    else if (istr_equal(class_name, istr_const(_xmCommand))) 
        xmcreate_func = XmCreateCommand; 
    else if (istr_equal(class_name, istr_const(_xmDialogShell))) 
        xmcreate_func = XmCreateDialogShell; 
    else if (istr_equal(class_name, istr_const(_xmDrawingArea))) 
        xmcreate_func = XmCreateDrawingArea; 
    else if (istr_equal(class_name, istr_const(_xmDrawnButton)))
	xmcreate_func = XmCreateDrawnButton;
    else if (istr_equal(class_name, istr_const(_xmFileSelectionBox))) 
        xmcreate_func = XmCreateFileSelectionBox; 
    else if (istr_equal(class_name, istr_const(_xmForm))) 
        xmcreate_func = XmCreateForm; 
    else if (istr_equal(class_name, istr_const(_xmFrame))) 
        xmcreate_func = XmCreateFrame; 
    else if (istr_equal(class_name, istr_const(_xmLabel))) 
        xmcreate_func = XmCreateLabel; 
    else if (istr_equal(class_name, istr_const(_xmLabelGadget))) 
        xmcreate_func = XmCreateLabelGadget; 
    else if (istr_equal(class_name, istr_const(_xmList))) 
        xmcreate_func = XmCreateList; 
    else if (istr_equal(class_name, istr_const(_xmMainWindow))) 
        xmcreate_func = XmCreateMainWindow; 
    else if (istr_equal(class_name, istr_const(_xmMenuShell))) 
        xmcreate_func = XmCreateMenuShell; 
    else if (istr_equal(class_name, istr_const(_xmMessageBox))) 
    {
        AB_COMPOUND_INFO *info = (AB_COMPOUND_INFO*)&(obj->info.compound);
        switch(info->type)
        {
        case AB_COMP_INFO_BOX:
            xmcreate_func = XmCreateInformationDialog;
            break;
        case AB_COMP_ERROR_BOX:
            xmcreate_func = XmCreateErrorDialog;
            break;
        case AB_COMP_QUESTION_BOX:
            xmcreate_func = XmCreateQuestionDialog;
            break;
        case AB_COMP_TEMPLATE_BOX:
            xmcreate_func = XmCreateTemplateDialog;
            break;
                case AB_COMP_WARNING_BOX: 
                        xmcreate_func = XmCreateWarningDialog; 
                        break; 
                case AB_COMP_WORKING_BOX: 
                        xmcreate_func = XmCreateWorkingDialog; 
                        break; 
                case AB_COMP_MESSAGE_BOX: 
                        xmcreate_func = XmCreateMessageDialog;
            break;
        default:
                    xmcreate_func = XmCreateMessageBox; 
         }
    }
    else if (istr_equal(class_name, istr_const(_xmPanedWindow))) 
        xmcreate_func = XmCreatePanedWindow; 
    else if (istr_equal(class_name, istr_const(_xmPopupMenu)))
	xmcreate_func = XmCreatePopupMenu;
    else if (istr_equal(class_name, istr_const(_xmPulldownMenu)))
	xmcreate_func = XmCreatePulldownMenu;
    else if (istr_equal(class_name, istr_const(_xmPushButton))) 
        xmcreate_func = XmCreatePushButton; 
    else if (istr_equal(class_name, istr_const(_xmRowColumn))) 
    {
        if (obj_is_menubar(obj))
            xmcreate_func = XmCreateMenuBar;

        else if (obj_is_choice(obj))
        {
            AB_CHOICE_INFO *info = (AB_CHOICE_INFO *)&(robj->info.choice);

	    if (objxm_get_config_obj(robj, AB_CFG_OBJECT_OBJ) == obj)
	    {
            	switch(info->type)
            	{
            	    case AB_CHOICE_EXCLUSIVE:
                    	xmcreate_func = XmCreateRadioBox;
                	break;
             	    case AB_CHOICE_NONEXCLUSIVE:
                	xmcreate_func = XmCreateSimpleCheckBox;
                	break;
	    	    case AB_CHOICE_OPTION_MENU:
			xmcreate_func = XmCreateOptionMenu;
			break;
            	    default:
                	xmcreate_func = XmCreateRowColumn;
		}
            }
	    else
		xmcreate_func = XmCreateRowColumn;
        }
        else
            xmcreate_func = XmCreateRowColumn;
    }

    else if (istr_equal(class_name, istr_const(_xmScale))) 
        xmcreate_func = XmCreateScale; 
    else if (istr_equal(class_name, istr_const(_xmScrollBar))) 
        xmcreate_func = XmCreateScrollBar; 
    else if (istr_equal(class_name, istr_const(_xmScrolledList)))
	xmcreate_func = XmCreateScrolledList;
    else if (istr_equal(class_name, istr_const(_xmScrolledWindow))) 
        xmcreate_func = XmCreateScrolledWindow; 
    else if (istr_equal(class_name, istr_const(_xmSelectionBox))) 
        xmcreate_func = XmCreateSelectionBox; 
    else if (istr_equal(class_name, istr_const(_xmSeparator))) 
        xmcreate_func = XmCreateSeparator; 
    else if (istr_equal(class_name, istr_const(_xmSeparatorGadget))) 
        xmcreate_func = XmCreateSeparatorGadget; 
    else if (istr_equal(class_name, istr_const(_xmText))) 
        xmcreate_func = XmCreateText; 
    else if (istr_equal(class_name, istr_const(_xmTextField))) 
        xmcreate_func = XmCreateTextField; 
    else if (istr_equal(class_name, istr_const(_xmToggleButton))) 
        xmcreate_func = XmCreateToggleButton; 
    else if (istr_equal(class_name, istr_const(_xmToggleButtonGadget))) 
        xmcreate_func = XmCreateToggleButtonGadget; 

    else    /* Unknown Widget class */
	xmcreate_func = NULL;

    return(xmcreate_func);
}

/* 
 * Store widget-id of XmForm widget which contains children
 * which have changed size
 */
static int 
formlist_append(
    Widget	newform
)
{
    int iRet = 0;
    int i;

    /* If form is already on list, return */
    for (i=0; i < form_list_count; i++)
	if (form_list[i] == newform)
	    return iRet;

    if (form_list_count < MAX_FORMS)
        form_list[form_list_count++] = newform;
    else 
    {
	if (util_get_verbosity() > 0)
	    fprintf(stderr,"formlist_append: form_list FULL\n");
	iRet = -1;
    }
    return iRet;

}

/*
 * For each XmForm on the form_list, force it to grant it's
 * children's resize requests by 'faking' a geometry change
 * on the XmForm itself.
 *
 * Yes, this is ugly, but is currently the only workaround
 * for the Motif bug where the XmForm refuses childrens'
 * resize requests after they've been already managed.
 */
static int
formlist_force_resize(
)
{
    Dimension f_width, f_height;
    int i;

    for (i= form_list_count-1; i >= 0; i--)
    {
        /* UGLY! */
        XtVaGetValues(form_list[i], XmNwidth, &f_width, XmNheight, &f_height, NULL);
        XtVaSetValues(form_list[i], XmNwidth, f_width+1,XmNheight, f_height+1,NULL);
        XtVaSetValues(form_list[i], XmNwidth, f_width,  XmNheight, f_height,  NULL);
	form_list[i] = NULL;
    }
    form_list_count = 0;

    return 0;

}


static void
xm_popup_menu(
    Widget 	widget,
    XtPointer 	client_data,
    XEvent 	*event,
    Boolean 	*cont
)
{
    XButtonEvent  *bevent = (XButtonEvent*)event;
    ABObj         obj     = (ABObj)client_data;
    ABObj	  menu_ref;
    Widget	  menu;

    if (bevent->button == 3)
    {
    	menu_ref = objxm_get_config_obj(obj, AB_CFG_MENU_OBJ);

    	if (menu_ref != NULL && (menu = (Widget)menu_ref->ui_handle) != NULL)
    	{
	    XmMenuPosition(menu, (XButtonPressedEvent*)bevent);
	    XtManageChild(menu);
	}
    }
}
