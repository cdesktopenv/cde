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
 *      $XConsortium: objxm_inst.c /main/8 1996/10/02 15:26:06 drk $
 *
 *      @(#)objxm_inst.c	1.114 19 May 1995
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
 * objxm_inst.c - instantiates AB objects into motif widgets
 *    
 *
 ***********************************************************************
 */

#include <stdio.h>
#include <ab_private/XmAll.h>
#include <Dt/TermPrim.h>
#include <Dt/Term.h>
#include <Dt/MenuButton.h>
#include <Dt/SpinBox.h>
#include <Dt/ComboBox.h>
#include <ab_private/trav.h>
#include "objxmP.h"

#define	MAX_FORMS  	24

char * const _applicationShell = "applicationShellWidgetClass";
char * const _dtComboBox =       "dtComboBoxWidgetClass";
char * const _dtMenuButton =     "dtMenuButtonWidgetClass";
char * const _dtSpinBox =	 "dtSpinBoxWidgetClass";
char * const _dtTerm =		 "dtTermWidgetClass";
char * const _group =		 "groupWidgetClass"; 
char * const _topLevelShell =	 "topLevelShellWidgetClass"; 
char * const _xmArrowButton =	 "xmArrowButtonWidgetClass";
char * const _xmBulletinBoard =	 "xmBulletinBoardWidgetClass";
char * const _xmCascadeButton =	 "xmCascadeButtonWidgetClass";
char * const _xmCommand =	 "xmCommandWidgetClass";
char * const _xmDialogShell =	 "xmDialogShellWidgetClass";
char * const _xmDrawingArea =	 "xmDrawingAreaWidgetClass";
char * const _xmDrawnButton =	 "xmDrawnButtonWidgetClass";
char * const _xmFileSelectionBox = "xmFileSelectionBoxWidgetClass";
/* This is a psuedo widget class */
char * const _xmFileSelectionDialog = "xmFileSelectionDialogWidgetClass";
char * const _xmForm =		"xmFormWidgetClass";
char * const _xmFrame =		"xmFrameWidgetClass";
char * const _xmLabel = 	"xmLabelWidgetClass";
char * const _xmLabelGadget =	"xmLabelGadgetWidgetClass";
char * const _xmList =		"xmListWidgetClass";
char * const _xmMainWindow =	"xmMainWindowWidgetClass";
char * const _xmManager =	"xmManagerWidgetClass";
char * const _xmMenuShell =	"xmMenuShellWidgetClass";
char * const _xmMessageBox =	"xmMessageBoxWidgetClass";
/* This is a psuedo widget class */
char * const _xmMessageDialog =	"xmMessageDialogWidgetClass";
char * const _xmPanedWindow = 	"xmPanedWindowWidgetClass";
/* xmPopupMenuWidgetClass is a pseudo class name but is supported by UIL */
char * const _xmPopupMenu = 	"xmPopupMenuWidgetClass"; 
char * const _xmPrimitive =	"xmPrimitiveWidgetClass";
/* xmPulldownMenuWidgetClass is a pseudo class name but is supported by UIL */
char * const _xmPulldownMenu =	"xmPulldownMenuWidgetClass";
char * const _xmPushButton =	"xmPushButtonWidgetClass";
char * const _xmRowColumn =	"xmRowColumnWidgetClass";
char * const _xmScale =		"xmScaleWidgetClass";
char * const _xmScrollBar =	"xmScrollBarWidgetClass";
/* xmScrolledListWidgetClass is a pseudo class name but is supported by UIL */
char * const _xmScrolledList =	"xmScrolledListWidgetClass";
char * const _xmScrolledWindow ="xmScrolledWindowWidgetClass";
char * const _xmSelectionBox =	"xmSelectionBoxWidgetClass";
char * const _xmSeparator =	"xmSeparatorWidgetClass";
char * const _xmSeparatorGadget="xmSeparatorGadgetWidgetClass";
char * const _xmText =		"xmTextWidgetClass";
char * const _xmTextField =	"xmTextFieldWidgetClass";
char * const _xmToggleButton =	"xmToggleButtonWidgetClass";
char * const _xmToggleButtonGadget = "xmToggleButtonGadgetWidgetClass";

/*************************************************************************
**                                                                      **
**       Private Function Declarations 					**
**                                                                      **
**************************************************************************/
static int	xm_instantiate_tree(
    		    ABObj    root,
		    BOOL     manage
		);
static int	xm_instantiate_obj(
    		    ABObj    obj,
		    BOOL     manage
		);
static int      post_instantiate_tree(
                    ABObj    root
                );
static int      post_instantiate_obj(
                    ABObj    obj
		); 
static int	xm_manage_tree(
		    ABObj    obj
		);
static Widget   instantiate_menu(
                    ABObj       obj,
		    ABObj	menu_ref,
                    Widget      owner,
                    Widget      parent
                );
static Widget	instantiate_widget(
		    ABObj	obj,
		    Widget	parent,
		    BOOL	manage
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

/*
 * Actions 
 */
static void	popup_menu(
    		    Widget      widget,
    		    XEvent      *event,
    		    String      *params,
    		    int         num_params
		);


/*************************************************************************
**                                                                      **
**       Global  Data                                                   **
**                                                                      **
**************************************************************************/

static    Widget form_list[MAX_FORMS];
static 	  int	 form_list_count = 0;

static 	  XtTranslations popup_menu_trans = NULL;

static XtActionsRec menu_actions[] = {
    {"PopupMenu",     (XtActionProc)popup_menu }
}; 
 
static String menu_trans =
     "<Btn3Down>:        PopupMenu()"; 

/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/

/*
 * Instantiate (Create widgets) for all objects in a tree
 * starting at root
 */
int
objxm_tree_instantiate(
    ABObj    root,
    BOOL     manage_last
)
{
    /* Create all widgets 
     */
    xm_instantiate_tree(root, manage_last? FALSE : TRUE);

    /* Set resources that must be set AFTER all widgets
     * are created.
     */
    post_instantiate_tree(root);

    /* Now that ALL resources have been set (attachments)
     * manage the widgets
     */
    if (manage_last)
    	xm_manage_tree(root);

    /* WORKAROUND for Motif XmForm bug (it ignores child resize request
     * if x,y have not also changed). So, we have to force it.
     */
    formlist_force_resize();

    return OK;
}

/*
 * Map/Unmap (display/undisplay on Screen) all objects in a tree
 * starting at root
 */
int
objxm_tree_map(
    ABObj	root,
    BOOL	map
)
{
    AB_TRAVERSAL	trav;
    ABObj		child = NULL;

    if (root == NULL)
	return -1;

    if (map)
      	xm_map_obj(root);
    else
	xm_unmap_obj(root);

    for (trav_open(&trav, root, AB_TRAV_CHILDREN);
        (child = trav_next(&trav)) != NULL; )
	objxm_tree_map(child, map);

    trav_close(&trav);
    return 0;
}

static int
xm_map_obj(
    ABObj	obj
)
{
    Widget	widget;

    if (!obj_is_ui(obj) && !obj_is_module(obj)) 
	return 0; 

    if (!obj_has_flag(obj, MappedFlag))
    {
        if (obj_is_module(obj) || obj_is_project(obj))
        {
            util_dprintf(2,"MAPPING %s: %s\n",
                       obj_is_project(obj)?"project":"module",
                        util_strsafe(obj_get_name(obj)));
        }
    	else if ((widget = (Widget)obj->ui_handle) != NULL)
    	{
	    util_dprintf(2,"MAPPING widget: %s\n", util_strsafe(obj_get_name(obj)));

            if (XtIsSubclass(widget, applicationShellWidgetClass) ||
		XtIsSubclass(widget, topLevelShellWidgetClass))
            	XtPopup(widget, XtGrabNone);
  	    else if (XtIsSubclass(widget, xmDialogShellWidgetClass))
	    {
	    	XtManageChild(objxm_get_widget(obj_get_child(obj, 0)));
	    	XtPopup(widget, XtGrabNone);
	    }
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

    if (!obj_is_ui(obj) && !obj_is_module(obj))
        return 0;  

    if (obj_has_flag(obj, MappedFlag)) 
    { 
        if (obj_is_module(obj) || obj_is_project(obj))
        { 
            util_dprintf(2,"UN-MAPPING %s: %s\n",
                       obj_is_project(obj)?"project":"module",
			util_strsafe(obj_get_name(obj)));
        } 
        else if ((widget = (Widget)obj->ui_handle) != NULL)
        { 
            util_dprintf(2,"UN-MAPPING widget: %s\n", 
				util_strsafe(obj_get_name(obj)));

            if (XtIsSubclass(widget, applicationShellWidgetClass) ||
		XtIsSubclass(widget, topLevelShellWidgetClass) ||
		XtIsSubclass(widget, xmDialogShellWidgetClass)) 
                XtPopdown(widget); 
        }
        obj_clear_flag(obj, MappedFlag); 
    } 
    return 0; 
} 

static int
xm_instantiate_tree(
    ABObj    root,
    BOOL     manage
)
{
    AB_TRAVERSAL    trav;
    ABObj           child;

    if (root == NULL)
        return -1;
    
    /* Top-down traversal is crucial */
    xm_instantiate_obj(root, manage);
    for (trav_open(&trav, root, AB_TRAV_CHILDREN);
        (child = trav_next(&trav)) != NULL; )
    {
    	xm_instantiate_tree(child, manage);
    }
    trav_close(&trav);

    return 0;

}

static int
post_instantiate_tree(
    ABObj    root
)
{
    AB_TRAVERSAL    trav;
    ABObj           child;
 
    if (root == NULL)
        return -1;

    post_instantiate_obj(root);
    
    /* Bottom-up traversal is crucial*/
    for (trav_open(&trav, root, AB_TRAV_CHILDREN);
	(child = trav_next(&trav)) != NULL; )
	if (obj_is_ui(child))
	    post_instantiate_tree(child);

    trav_close(&trav);
 
    return 0;
}

static int
xm_manage_tree(
    ABObj	root
)
{
    AB_TRAVERSAL    trav;
    static ABObj    manage_root = NULL;
    ABObj	    child;
    ABObj	    childobj;
    Widget	    widget;
    WidgetList	    wchildren;
    int		    num_children;
    int		    num_wchildren = 0;
    int		    i;

    if (root == NULL)
        return -1;

    if (manage_root == NULL)
	manage_root = root;
   
    /* Bottom-up traversal is crucial*/
    for (trav_open(&trav, root, AB_TRAV_CHILDREN);
        (child = trav_next(&trav)) != NULL; )
    {
        xm_manage_tree(child);
    }
    trav_close(&trav);

    widget = (Widget)root->ui_handle;

    /* If this widget has children, manage them all at once.
     */
    if (widget != NULL && XtIsSubclass(widget, compositeWidgetClass))
    {
	/* Build list of valid widget children */
	num_children = obj_get_num_children(root);
	wchildren = (WidgetList)util_malloc(num_children*sizeof(Widget));
	for (i=0; i < num_children; i++)
	{
	    childobj = obj_get_child(root, i);
	    if (childobj->ui_handle != NULL &&
	        !objxm_is_menu_widget(objxm_get_widget(childobj)))
		wchildren[num_wchildren++] = (Widget)childobj->ui_handle;
	}
	if (num_wchildren > 0 && !XtIsSubclass(widget, xmDialogShellWidgetClass))
	{
	    XtManageChildren(wchildren, num_wchildren);
            if (util_get_verbosity() > 3)
            {
                Dimension w,h;
                fprintf(stderr,"MANAGING Children:");
                for (i=0; i < num_wchildren ; i++)
                {
                    XtVaGetValues(wchildren[i], XmNwidth, &w, XmNheight, &h, NULL);
                    fprintf(stderr,"%s(%dx%d) ", XtName(wchildren[i]),w,h);
                }
                fprintf(stderr,".\n");
            }
	}
	util_free(wchildren);
    }
    /* We are back at the Top of the tree being managed:
     * MUST manage the top one by hand since this function
     * was not called on its parent...
     */
    if (manage_root == root)
    {
	if (widget != NULL && !XtIsSubclass(widget, shellWidgetClass) &&
	    !XtIsSubclass(XtParent(widget), xmDialogShellWidgetClass))
	{
	    XtManageChild(widget);
	    if (util_get_verbosity() > 3)
	    {
		Dimension w,h;
		XtVaGetValues(widget, XmNwidth, &w, XmNheight, &h, NULL);
            	fprintf(stderr,"MANAGING root: %s(%dx%d)\n", 
			util_strsafe(obj_get_name(root)),w,h);
	    }
	}
	manage_root = NULL;
    }
    return 0;
}
static int
xm_instantiate_obj(
    ABObj    obj,
    BOOL     manage
)
{
    ABObj	   parentobj, temp_parent;
    Widget	   widget;
    Widget	   parent;
    
    if (!obj_has_flag(obj, XmConfiguredFlag) ||
	obj_has_flag(obj, InstantiatedFlag) ||
	obj_has_flag(obj, BeingDestroyedFlag))
        return OK; /* not considered an error */

    if (obj_is_virtual(obj) || obj_is_menu_ref(obj))
    {
        objxm_obj_remove_all_ui_args(obj, OBJXM_CONFIG_BUILD);
        obj_clear_flag(obj, CreateAttrsFlag);
        obj_set_flag(obj,   InstantiatedFlag);
        obj_clear_flag(obj, AttrChangedFlag);
	return OK;
    }

    /* List,ComboBox & SpinBox Items are not separate widgets */
    if (obj_is_combo_box_item(obj) || 
	obj_is_list_item(obj) ||
	obj_is_spin_box_item(obj))
    { 
	AB_ITEM_TYPE 	itype;
	XmString 	xmitem;
	char		*obj_name;

        if ((parent = objxm_get_widget(obj_get_parent(obj))) == NULL)
        {
	    obj_name = util_strsafe(obj_get_name(obj));
            util_dprintf(1, "Instantiation ERROR: %s: widget-ID for parent is NULL\n", obj_name); 
            return ERROR;
    	}
	itype =  obj_get_item_type(obj);
        xmitem = XmStringCreateLocalized(obj_get_label(obj));

	switch(itype)
	{
	    case AB_ITEM_FOR_LIST:
	    	XmListAddItem(parent, xmitem, 0);
            	obj_set_flag(obj, InstantiatedFlag);
		break;
	    case AB_ITEM_FOR_COMBO_BOX:
	    	DtComboBoxAddItem(parent, xmitem, 0, False);
	    	obj_set_flag(obj, InstantiatedFlag);
		break;
	    case AB_ITEM_FOR_SPIN_BOX:
		DtSpinBoxAddItem(parent, xmitem, 0);
		obj_set_flag(obj, InstantiatedFlag);
		break;
	    default:
		return ERROR;
	}
	XmStringFree(xmitem);
	if (obj_has_flag(obj, InstantiatedFlag))
	    return OK;
    }
    /* Determine parent.. */
    /* If obj is a window, parent it off the AB toplevel during
     * build-mode to ensure it's independence during UI editing
     */
    if (obj_is_window(obj))
        parent = ObjxmP_toplevel;
    else
    {
        parentobj = obj_get_parent(obj);
 
        while (parentobj != NULL && obj_is_virtual(parentobj))
        {
            temp_parent = obj_get_parent(parentobj);
            if (temp_parent == NULL)
                break;

            parentobj = temp_parent;
        }
 
        parent = (Widget)parentobj->ui_handle;
    }

    widget = instantiate_widget(obj, parent, manage);

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
	    ABObj	robj = obj_get_root(obj);
	    if (obj_is_choice(robj)) 
	    {
		ABObj	o_obj = objxm_comp_get_subobj(robj, AB_CFG_OBJECT_OBJ);
		objxm_obj_set_ui_arg(o_obj, AB_ARG_WIDGET, XmNsubMenuId, widget);
	    }
	}

	/* if widget is being replaced, destroy old one */
	if (obj->ui_handle != NULL)
	    objxm_tree_uninstantiate(obj, TRUE);

	/* Store widget-id in the obj */
	obj->ui_handle = (void*)widget;

	/* Store the ABObj & the original translations in the widget's
	 * userData
	 */
	objxm_store_obj_and_actions(widget, obj);

	objxm_obj_remove_all_ui_args(obj, OBJXM_CONFIG_BUILD);
	obj_clear_flag(obj, CreateAttrsFlag);
	obj_set_flag(obj,   InstantiatedFlag);
 	obj_clear_flag(obj, AttrChangedFlag);

        return OK;
    }
    return ERROR;

}


static Widget
instantiate_widget(
    ABObj	obj,
    Widget	parent,
    BOOL	manage
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
        util_dprintf(1, "Instantiation ERROR: Object must have a valid name\n");
        return NULL;
    }
    if (parent == NULL)
    {
        char *obj_name = util_strsafe(obj_get_name(obj));
        util_dprintf(1, "Instantiation ERROR: %s: widget-ID for parent is NULL\n", obj_name);
        return NULL;
    }
    if ((classname = obj_get_class_name(obj)) == NULL)
    {
	classname = objxm_obj_get_default_motif_class(obj);
	obj_set_class_name(obj, classname);
    }

    if (!obj_has_flag(obj, CreateAttrsFlag)) /* Make sure Resources are set */
        objxm_comp_set_ui_args(obj_get_root(obj), OBJXM_CONFIG_BUILD, TRUE);

    /* Instantiate Widget using class & arglist
     */
    if (util_streq(obj_get_class_name(obj), _applicationShell))
    {
	all_args = (ArgList)obj->ui_args;
	num_args = objxm_get_num_args(all_args);

        if (util_get_verbosity() > 3)
        {
            util_dprintf(2,"INSTANTIATING(Shell): %s part_of(%s), parent=%s, class=%s.\n",
                name, obj->part_of? util_strsafe(obj_get_name(obj->part_of)) : name,
                XtName(parent), obj_get_class_name(obj));
            objxm_dump_arglist(obj, all_args, num_args);
        }

    	widget = XtCreatePopupShell(name,
        		topLevelShellWidgetClass,
        		parent,
        		all_args,
        		num_args);
	XtVaSetValues(widget, XtNwindowGroup, XtWindow(ObjxmP_toplevel), NULL);

    }
    else  /* Non TopLevel */
    {
	all_args = (ArgList)obj->ui_args;
        num_args = objxm_get_num_args(all_args); 

	if (util_streq(obj_get_class_name(obj), _xmFrame) ||
	    XtIsSubclass(parent, xmFrameWidgetClass))
	{
	    xmclass  = objxm_get_class_ptr(obj_get_class_name(obj));
	    widget = XtCreateManagedWidget(name, xmclass, parent, all_args, num_args);
	}
	else
	{
            xmcreate_func = objxm_obj_get_xmcreate_func(obj);

	    if (xmcreate_func != NULL)
            	widget = (*xmcreate_func)(parent, name, all_args, num_args);
            else
	    {
	    	xmclass  = objxm_get_class_ptr(obj_get_class_name(obj));
	    	if (xmclass != NULL)
                   widget = XtCreateWidget(name, xmclass, parent, all_args, num_args);
	    	else
	    	{
		    char *obj_name = util_strsafe(obj_get_name(obj));
                    util_dprintf(1, "Instantiation ERROR: %s: Object must have widget class\n", obj_name);
		    return NULL;
	    	}
	    }
	}
        if (util_get_verbosity() > 3)
        {
            fprintf(stderr,"INSTANTIATING: %s=%p part_of(%s), parent=%s, class=%s.\n",
                name, widget, obj->part_of? util_strsafe(obj_get_name(obj->part_of)) : name,
                XtName(parent), obj_get_class_name(obj));
            objxm_dump_arglist(obj, all_args, num_args);
        }

	if (manage && !objxm_is_menu_widget(widget))
	{
	    XtManageChild(widget);
	    if (util_get_verbosity() > 3)
	    {
		Dimension w,h;
		XtVaGetValues(widget, XmNwidth, &w, XmNheight, &h, NULL);
            	fprintf(stderr,"MANAGING: %s(%dx%d)\n", XtName(widget), w,h);
	    }
	}

    }
    return widget;

} 

static int
post_instantiate_obj(
    ABObj	obj
)
{
    Widget	widget;
    ABObj	parent_obj;
    ArgList	args;
    int		num_args;

    widget = (Widget)obj->ui_handle;

    if (ObjxmP_post_instantiate_cb != NULL)
	ObjxmP_post_instantiate_cb(obj);

    objxm_obj_set_post_inst_args(obj, OBJXM_CONFIG_BUILD);

    /* If there is no widget associated with obj, then we are done */
    if (widget == NULL)
        return 0;

    if (!obj_is_window(obj))
    {
	parent_obj = obj_get_parent(obj);
	
	/* This is a workaround for an extremely strange problem where [sometimes]
	 * Control Panes do not resize large enough to display all of their children.
	 */
	if (XtIsSubclass(objxm_get_widget(parent_obj), xmFormWidgetClass))
	    formlist_append(objxm_get_widget(parent_obj));
    }

    if (obj_has_flag(obj,AttrChangedFlag))
    {
	args = (ArgList)obj->ui_args;
        num_args = objxm_get_num_args(args);

	if (util_get_verbosity() > 3)
	{
	    fprintf(stderr,"POST-INSTANTIATE SetValues: %s\n", util_strsafe(obj_get_name(obj)));
	    objxm_dump_arglist(obj, args, num_args); 
	}
        /*
          if (!XtIsManaged(widget))
          XtManageChild(widget);
          */
        XtSetValues(widget, args, num_args);
        obj_clear_flag(obj, AttrChangedFlag);
        objxm_obj_remove_all_ui_args(obj, OBJXM_CONFIG_BUILD);
    }
    return 0;

}

/*
 * OBJ-callback: called when an object is destroyed so
 * we can destroy the widget associated with it
 */
int
objxmP_destroy( 
    ObjEvDestroyInfo    info 
) 
{
    ABObj	obj    = info->obj;
    Widget	widget = (Widget)info->obj->ui_handle;

    if (widget != NULL)
    {
	objxm_free_obj_and_actions(widget);
	/*
	 * WORKAROUND UNTIL XtDestroyWidget(combobox) bug
	 * is fixed. Isa 2/22/94
	 *
	 * WORKAROUND Also for XmMainWindow bug - destroying
	 * the messageWindow widget causes it to corecump. aim 7/14/94
	 */
	if (obj_is_combo_box(obj) ||
	   (obj_is_control_panel(obj) && obj_get_subtype(obj) == AB_CONT_FOOTER))
	{
	    XtUnrealizeWidget(widget);
	}
	else
	{
            util_dprintf(2,"WIDGET DESTROY(CB): %s=%p part_of(%s)\n",
                	util_strsafe(obj_get_name(obj)), widget,
                	obj->part_of? util_strsafe(obj_get_name(obj->part_of)) :
                	util_strsafe(obj_get_name(obj)));

	    XtDestroyWidget(widget);
	}
	obj->ui_handle = NULL;
    }
    /* Free the arglist if it exists */
    if (obj->ui_args != NULL)
    {
	util_dprintf(2, "FREEING ARGLIST: %s\n", obj_get_name(obj));
	util_free(obj->ui_args);
    }
    return 0;

}

/*
 * Destroy Widgets AND objects in a tree starting at root
 */
int
objxm_tree_destroy(
    ABObj	root
)
{

    if (root == NULL)
	return -1;

    /* Destroy any existing widgets */
    objxm_tree_uninstantiate(root, TRUE);

    /* Destroy ABObj tree */
    obj_destroy(root);

    return 0;
}

/*
 * Destroy Widgets in a tree starting at root
 * (sets ui_handle to NULL)
 */
int
objxm_tree_uninstantiate(
    ABObj	root,
    BOOL	topmost
)
{
    Widget	    widget;
    AB_TRAVERSAL    trav;
    ABObj           child;

    /*
     * Fix for Bug Ids 1189359 and 1200072
     *
     * Clear the flags first because list, combo box, and spin box items
     * don't actually have a ui_handle associated with them.
     */
    obj_clear_flag(root, BuildActionsFlag);
    obj_clear_flag(root, InstantiatedFlag);
    obj_clear_flag(root, MappedFlag);

    widget = (Widget)root->ui_handle;

    if (widget == NULL)
        return -1;

    for (trav_open(&trav, root, AB_TRAV_CHILDREN);
        (child = trav_next(&trav)) != NULL; )
    {
        objxm_tree_uninstantiate(child, FALSE);
    }

    objxm_free_obj_and_actions(widget);

    /*
     * WORKAROUND UNTIL XtDestroyWidget(combobox) bug
     * is fixed. Isa 2/22/94
     * WORKAROUND Also for XmMainWindow bug - destroying
     * the messageWindow widget causes it to corecump. aim 7/14/94
     */
    if (obj_is_combo_box(root) ||
           (obj_is_control_panel(root) && obj_get_subtype(root) == AB_CONT_FOOTER))
    {
	XtUnrealizeWidget(widget);
    }
    else
    {
        util_dprintf(2,"WIDGET DESTROY: %s=%p part_of(%s)\n",
           	util_strsafe(obj_get_name(root)), widget,
           	root->part_of? util_strsafe(obj_get_name(root->part_of)) :
                util_strsafe(obj_get_name(root)));

        XtDestroyWidget(widget);
    }
    root->ui_handle = NULL;

    trav_close(&trav);

    return OK;
 
}

/*
 * Instantiate any changes made to an object tree
 * starting at root
 */
int
objxm_tree_instantiate_changes(
    ABObj	root,
    BOOL	*new_widgets
)
{
    static ABObj    chg_root = NULL;
    AB_TRAVERSAL    trav;
    ABObj           child;
    static BOOL	    new_w = FALSE;

    if (root == NULL)
        return -1;

    if (chg_root == NULL)
	chg_root = root;

    /* Instantiate changes TOP-DOWN */
    objxm_comp_instantiate_changes(root, new_widgets);
    if (*new_widgets == TRUE)
        new_w = TRUE;

    for (trav_open(&trav, root, AB_TRAV_SALIENT_CHILDREN);
        (child = trav_next(&trav)) != NULL; )
    {
        objxm_tree_instantiate_changes(child, new_widgets);
    }

    trav_close(&trav);
    if (chg_root == root) /* We are done */
    {
	chg_root = NULL;
	*new_widgets = new_w;
	new_w = FALSE;
    }
    return 0;
}

/*
 * For a Composite Object, Instantiate any changes made
 * to it's tree using recursion.
 */
int
objxm_comp_instantiate_changes(
    ABObj	obj,
    BOOL	*new_widgets /* return if new widgets created */
)
{
    static ABObj comp_root = NULL; /* Composite RootObj */
    static BOOL  args_set = FALSE;
    static BOOL  w_created = FALSE;
    AB_TRAVERSAL trav;
    ABObj        sub_obj;
    ArgList      args;
    int          num_args;
    Widget       widget;
    BOOL	 new_w;
 
    if (comp_root == NULL) /* Keep track of the Root */
        comp_root = obj;

    widget = (Widget)obj->ui_handle;

    /* Re-instantiate obj (if it's not being-destroyed)
     */
    if (!obj_has_flag(obj, InstantiatedFlag) && 
	!obj_has_flag(obj, BeingDestroyedFlag))
    {
        if (!args_set) /* Set Args ONCE */
        {
            objxm_comp_set_ui_args(obj_get_root(obj),
			OBJXM_CONFIG_BUILD, TRUE);
            args_set = TRUE;
        }
	if (widget != NULL) /* object being re-instantiated */
	    XtUnmanageChild(widget);

        objxm_tree_instantiate(obj, /*False*/True);
	w_created = TRUE;
    }
    /* Change obj's resources (if it's not being destroyed)
     */
    else if (widget != NULL &&
            obj_has_flag(obj, AttrChangedFlag) && 
	    !obj_has_flag(obj, BeingDestroyedFlag))
    {
        args = (ArgList)obj->ui_args;
        num_args = objxm_get_num_args(args);

        if (util_get_verbosity() > 3) 
        {
            fprintf(stderr,"CHANGE-ATTRS SetValues: %s\n",
			util_strsafe(obj_get_name(obj)));
            objxm_dump_arglist(obj, args, num_args);
        }
        XtSetValues(widget, args, num_args);

/*
        if (XtIsSubclass(XtParent(widget), xmFormWidgetClass))
            formlist_append(XtParent(widget));
*/

	/* If XmNbackground is being set, we must call the Motif
	 * routine which re-calculates/sets all other colors in
	 * widget (shadow, etc).
	 */
/*
REMIND: aim,2/8/94 - revisit why this prevents foreground from being
        changed later...

	for (i = 0; i < num_args; i++)
	{
	    if (args[i].name == XmNbackground)
		XmChangeColor(widget, args[i].value);
	}
*/

        obj_clear_flag(obj, AttrChangedFlag);
        objxm_obj_remove_all_ui_args(obj, OBJXM_CONFIG_BUILD);
    }

    if (obj_has_flag(obj, BeingDestroyedFlag))
        objxm_tree_destroy(obj);
    else
    {
        /* Use recursion to Instantiate changes for any 
	 * SubObjs or Items of this obj
 	 * (except for Menu-reference SubObjs)
         */
        for (trav_open(&trav, obj, AB_TRAV_CHILDREN);
                (sub_obj = trav_next(&trav)) != NULL; )
        {
            if (sub_obj->part_of == comp_root &&
                !obj_is_menu_ref(sub_obj))
                objxm_comp_instantiate_changes(sub_obj, &new_w);
        }
        trav_close(&trav);
    }
    if (obj == comp_root) /* Recursed back up to Root */
    {
	/* Ensure any Non-widget/OptionMenu type items get "selected" if necessary */
        if (obj_is_item(obj)) 
            objxm_comp_set_item_selection(obj);

        formlist_force_resize();
	*new_widgets = w_created;
        comp_root = NULL;
        args_set = FALSE;
	w_created = FALSE;
    }
    return 0;
}

/*
 * Makes a non-widget type item (List, ComboBox, SpinBox) selected/unselected
 * and makes an OptionMenu item the "selected" item
 */
int
objxm_comp_set_item_selection(
    ABObj	obj
)
{
    ABObj		pobj;
    Widget		parent;
    XmString		xmitem;
    AB_ITEM_TYPE	itype;
    BOOL		selected;
    
    pobj = obj_get_parent(obj);
    parent = objxm_get_widget(pobj);
 
    if (!obj_is_item(obj) || parent == NULL)
	return -1;
    
    selected = obj_is_initially_selected(obj);
    itype = obj_get_item_type(obj);
    
    if (itype == AB_ITEM_FOR_CHOICE && selected &&
	(obj_get_choice_type(obj_get_root(pobj)) == AB_CHOICE_OPTION_MENU))
    {
	Widget optionbox =
		objxm_get_widget(objxm_comp_get_subobj(obj_get_root(pobj), AB_CFG_OBJECT_OBJ));
	Widget item_w = objxm_get_widget(obj);

	if (optionbox && item_w) /* Make OptionMenu item selected */
	    XtVaSetValues(optionbox, XmNmenuHistory, item_w, NULL);
    }
    else if (itype == AB_ITEM_FOR_LIST ||
	itype == AB_ITEM_FOR_COMBO_BOX ||
	itype == AB_ITEM_FOR_SPIN_BOX)
    {
	Widget combobox;
	int pos = obj_get_child_num(obj);

	xmitem = XmStringCreateLocalized(obj_get_label(obj));

	switch(itype)
	{
            case AB_ITEM_FOR_COMBO_BOX:
                if (selected)
                    DtComboBoxSetItem(parent, xmitem);
		combobox = parent;
                parent = XtNameToWidget(combobox, "*List");
                /* fall through...*/
            case AB_ITEM_FOR_LIST:
                if (selected)
                    XmListSelectPos(parent, ++pos, True);
                else
                    XmListDeselectPos(parent, ++pos);
                break;
            case AB_ITEM_FOR_SPIN_BOX:
                if (selected)
                    DtSpinBoxSetItem(parent, xmitem);
                break;
	    default:
		break;
	}
	XmStringFree(xmitem);
    }
    return 0;
}
	    

/*
 * Store the corresponding ABObj & widget Translations in the
 * userData for the widget
 */
int      
objxm_store_obj_and_actions( 
    Widget      w,
    ABObj       obj 
)
{
    XtTranslations  trans;
    ObjxmWidgetInfo *winfo = NULL;

    if (w == NULL || obj == NULL)
	return -1;

    winfo = (ObjxmWidgetInfo*)util_malloc(sizeof(ObjxmWidgetInfo));

    /* Store ABObj & Widget translations in userData of widget 
     */
    winfo->obj = obj;

    XtVaGetValues(w, XtNtranslations, &trans, NULL);
    winfo->translations = trans;

    XtVaSetValues(w, XmNuserData, winfo, NULL);

    return 0;

}

/*
 * Free the data structure used to store the ABObj &
 * original translations for a widget
 */
int
objxm_free_obj_and_actions(
    Widget	w
)
{
    ObjxmWidgetInfo *winfo = NULL;

    if (w == NULL)
	return -1;

    XtVaGetValues(w, XmNuserData, &winfo, NULL);

    util_free(winfo);

    return 0;

}
 
/*
 * Retrieve the ABObj corresponding to a widget
 */
ABObj    
objxm_get_obj_from_widget( 
    Widget      w
)
{
    ObjxmWidgetInfo *winfo = NULL;

    if (w == NULL)
	return NULL;

    XtVaGetValues(w, XmNuserData, &winfo, NULL);

    if (winfo != NULL)
    	return(winfo->obj);

    return NULL;
}
 
/*
 * Retrieve the original translations corresponding to a widget
 */
XtTranslations 
objxm_get_actions_from_widget( 
    Widget       w 
)
{
    ObjxmWidgetInfo *winfo = NULL; 

    if (w == NULL) 
        return NULL;
 
    XtVaGetValues(w, XmNuserData, &winfo, NULL); 

    if (winfo != NULL)
    	return(winfo->translations); 
 
    return NULL;

}

WidgetClass
objxm_get_class_ptr(
    STRING	class_name
)
{
    ISTRING     class_istr = istr_create(class_name);
    WidgetClass xmclass;
            
    xmclass  = xm_get_class(class_istr);

    istr_destroy(class_istr);
    return xmclass;
}

static WidgetClass
xm_get_class(
    ISTRING    class_name
)
{
    WidgetClass    wclass;

    if (istr_equal(class_name, istr_const(_applicationShell)))
        wclass = applicationShellWidgetClass;
    else if (istr_equal(class_name, istr_const(_topLevelShell)))
        wclass = topLevelShellWidgetClass;
    else if (istr_equal(class_name, istr_const(_dtTerm)))
	wclass = dtTermWidgetClass;
    else if (istr_equal(class_name, istr_const(_xmArrowButton)))
        wclass = xmArrowButtonWidgetClass;
    else if (istr_equal(class_name, istr_const(_xmBulletinBoard))) 
        wclass = xmBulletinBoardWidgetClass; 
    else if (istr_equal(class_name, istr_const(_xmCascadeButton))) 
        wclass = xmCascadeButtonWidgetClass; 
    else if (istr_equal(class_name, istr_const(_dtComboBox))) 
        wclass = dtComboBoxWidgetClass; 
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
    else if (istr_equal(class_name, istr_const(_dtMenuButton))) 
        wclass = dtMenuButtonWidgetClass; 
    else if (istr_equal(class_name, istr_const(_xmMenuShell))) 
        wclass = xmMenuShellWidgetClass; 
    else if (istr_equal(class_name, istr_const(_xmMessageBox))) 
        wclass = xmMessageBoxWidgetClass; 
    else if (istr_equal(class_name, istr_const(_xmPanedWindow))) 
        wclass = xmPanedWindowWidgetClass; 
    else if (istr_equal(class_name, istr_const(_xmPrimitive))) 
        wclass = xmPrimitiveWidgetClass; 
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
    else if (istr_equal(class_name, istr_const(_dtSpinBox))) 
        wclass = dtSpinBoxWidgetClass; 
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
objxm_obj_get_xmcreate_func(
    ABObj    obj
)
{
    ABObj	   robj = obj_get_root(obj);
    ISTRING        class_name = istr_create(obj_get_class_name(obj));
    XmCreateFunc   xmcreate_func;

    if (class_name == NULL)
        return NULL;

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
    else if (istr_equal(class_name, istr_const(_xmFileSelectionDialog))) 
        xmcreate_func = XmCreateFileSelectionDialog; 
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
    else if (istr_equal(class_name, istr_const(_dtMenuButton))) 
        xmcreate_func = DtCreateMenuButton; 
    else if (istr_equal(class_name, istr_const(_xmMenuShell))) 
        xmcreate_func = XmCreateMenuShell; 
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

	    if (objxm_comp_get_subobj(robj, AB_CFG_OBJECT_OBJ) == obj)
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

    istr_destroy(class_name);
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
	util_dprintf(3,"formlist_append: form_list FULL\n");
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
formlist_force_resize(void)
{
    Dimension 	f_width, f_height;
    int 	i;
    Widget	phantom;
    Pixel	bgpixel;
    XmString	xmlabel;

    util_dprintf(2,"formlist_force_resize: ");

    for (i= form_list_count-1; i >= 0; i--)
    {
	/* This is an unfortunate workaround for a strange problem where
	 * the XmForm widget does not expand to a size large enough to
	 * display all its children.  
	 *
	 * Creating a temporary, invisible widget seems to wake up the
	 * Form's geometry manager into resizing itself to the correct
	 * geometry.
	 *
	 * (I agree, this makes little sense, but trust me, it works).
	 */
        XtVaGetValues(form_list[i], 
		XmNbackground, &bgpixel,
		NULL);

	xmlabel = XmStringCreateLocalized(" ");
	util_dprintf(2,"\n Creating phantom child of %s.\n", XtName(form_list[i]));
	phantom = XtVaCreateManagedWidget("phantom", xmLabelWidgetClass,
		form_list[i], 
		XmNlabelString,	xmlabel,
		XmNbackground,	bgpixel,
		NULL);
	XmStringFree(xmlabel);
	XtDestroyWidget(phantom);
	form_list[i] = NULL;
    }
    form_list_count = 0;
    util_dprintf(2, "\n");

    return 0;
}

int
objxm_create_popup_menu_title(
    ABObj	menu_ref,
    OBJXM_CONFIG_TYPE ctype,
    STRING	title
)
{
    ABObj	tobj;
    ABObj	menu;

    if (menu_ref == NULL || !obj_is_menu_ref(menu_ref))
	return -1;

    menu = obj_get_actual_obj(menu_ref);
    if (menu == NULL)
	return -1;

    tobj = obj_create(AB_TYPE_LABEL, NULL);
    obj_insert_child(menu_ref, tobj, 0);
    obj_set_unique_name(tobj,
        ab_ident_from_name_and_label(obj_get_name(menu), "title"));
    obj_set_label(tobj, title);
    objxm_obj_configure(tobj, ctype, True);

    if (ctype == OBJXM_CONFIG_BUILD)
    	tobj->ui_handle = instantiate_widget(tobj, 
		objxm_get_widget(menu_ref), True);
 
    tobj = obj_create(AB_TYPE_SEPARATOR, NULL);
    obj_insert_child(menu_ref, tobj, 1);
    obj_set_unique_name(tobj,
        ab_ident_from_name_and_label(obj_get_name(menu),
		"title_sep"));
    obj_set_line_style(tobj, AB_LINE_DOUBLE_LINE);
    objxm_obj_configure(tobj, ctype, True);

    if (ctype == OBJXM_CONFIG_BUILD)
    	tobj->ui_handle = instantiate_widget(tobj, 
		objxm_get_widget(menu_ref), True);

    return 0;
}

/*
 * Instantiate (create Widgets) for all Menus connected to
 * objects inside a tree starting at root.
 */
int
objxm_instantiate_menus(
    ABObj	root
)
{
    AB_TRAVERSAL    trav;
    ABObj           obj;
    ABObj	    m_parent;
    ABObj	    menu_ref;
 
    if (root == NULL)
        return -1;
   
    for (trav_open(&trav, root, AB_TRAV_UI);
        (obj = trav_next(&trav)) != NULL; )
  	/* Instantiate a menu for the object if one exists.
	 * NOTE: SubMenus (menus off of other menu-items) are 
	 * actually instantiated using recursion in instantiate_menu(),
	 * thus we do not call instantiate_menu() for them directly.
	 */
    	if (obj_is_salient(obj) &&
	    obj_get_menu_name(obj) != NULL && !obj_is_menu_item(obj))
    	{
	    m_parent = objxm_comp_get_subobj(obj, AB_CFG_MENU_PARENT_OBJ);
	    menu_ref = objxm_comp_get_subobj(obj, AB_CFG_MENU_OBJ);
	    instantiate_menu(obj, menu_ref, (Widget)obj->ui_handle, 
				(Widget)m_parent->ui_handle);
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
 * to the object which "owns" a copy of the menu. Also, in order to store
 * the widget-IDs of all menu-items & sub-menus, the menu obj hierarchy is
 * temporarily created underneath the Menu-ref obj (this hierarchy is destroyed
 * when the the menus are destroyed).
 *
 * Both "owner" & "parent" are required because for casacdes, Motif requires a 
 * menu to be parented off the *parent* of the cascade...
 *
 */
static Widget 
instantiate_menu(
    ABObj	obj,
    ABObj	menu_ref,
    Widget	owner, 
    Widget	parent
)
{
    AB_TRAVERSAL trav;
    AB_MENU_TYPE m_type;
    ABObj	sub_menu_ref;
    ABObj	menu_p;
    ABObj	menu;
    ABObj	item;
    ABObj	tmp_itemobj;
    ABObj	tmp_menuref;
    Widget	menu_w = NULL;
    Widget	submenu_w = NULL;
    Widget	item_w;

    if (menu_ref != NULL)
    {
	/* Get Virtual Menu & object to parent menu off of */
	menu = obj_get_actual_obj(menu_ref);

    	if (menu != NULL && parent != NULL)
    	{
	    if (obj_is_button(obj) && obj_get_subtype(obj) == AB_BUT_MENU)
	    {
		/* MenuButtons already have their Menu created */
		XtVaGetValues((Widget)obj->ui_handle, XmNsubMenuId, &menu_w, NULL);
		menu_ref->ui_handle = (void*)menu_w;
	    }
	    else
	    {
	    	/* Set Pseudo-class for Menu based on the type
	     	 * of object that owns it
	     	 */
	    	if (obj_is_menu_item(obj) || obj_is_menubar_item(obj))
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
            	menu_ref->ui_handle = (void*)menu_w;

	        if (m_type == AB_MENU_PULLDOWN)
	    	    XtVaSetValues(owner, XmNsubMenuId, menu_w, NULL);

	    	else /* PopupMenu */
		{
		    STRING title;

		    XtInsertEventHandler(parent, ButtonPressMask, False, xm_popup_menu, 
			(XtPointer)obj, XtListHead);
/*
 REMIND: aim, 10/7/94 - In trying to migrate to using translations, it
	 messed up the event handling when we returned to BUILD mode.
	 For now, going back to event-handling.
	
		    ABObj	tobj;

		    if (popup_menu_trans == NULL)
		    {
			XtAppAddActions(XtWidgetToApplicationContext(parent), 
				menu_actions, XtNumber(menu_actions));
			popup_menu_trans = XtParseTranslationTable(menu_trans);
		    }
		    menu_p = objxm_get_obj_from_widget(parent);
		    objxm_store_obj_and_actions(parent, menu_p);
		    XtOverrideTranslations(parent, popup_menu_trans);
*/

		    if (!util_strempty((title = obj_get_menu_title(obj))))
			objxm_create_popup_menu_title(menu_ref, OBJXM_CONFIG_BUILD,
				title);
		}
	    }

	    /* Instantiate Menu Items */
	    for (trav_open(&trav, menu, AB_TRAV_CHILDREN);
            (item = trav_next(&trav)) != NULL; )
	    {
		/* Create TEMP copy of Item obj under Menu-ref */
                tmp_itemobj = obj_dup(item);
                obj_append_child(menu_ref, tmp_itemobj);
	    	item_w = instantiate_widget(item, menu_w, TRUE);
		tmp_itemobj->ui_handle = (void*)item_w;

		/* If SubMenu exists, instantiate it */
		if ((sub_menu_ref = objxm_comp_get_subobj(item, AB_CFG_MENU_OBJ)) != NULL)
		{
		    /* Create TEMP copy of submenu(Menu-ref) under temp Item obj */
		    tmp_menuref = 
			obj_create_ref(obj_get_actual_obj(sub_menu_ref));
                    tmp_menuref->part_of = tmp_itemobj;
                    obj_append_child(tmp_itemobj, tmp_menuref);
		    submenu_w = instantiate_menu(
					item, tmp_menuref, item_w, menu_w);
		}
	    }
	    trav_close(&trav);
	}
	else 
	{
	    char *obj_name = util_strsafe(obj_get_name(obj));
	    util_dprintf(1, "Instantiation ERROR: %s: could not instantiate menu\n", obj_name);
	    return NULL;
	}
    }
    return menu_w;
}

/*
 * Destroy widgets for all Menus connected to
 * objects inside a tree starting at root.
 */
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
    ABObj	    *item_array;
    int		    num_items;
    int		    i;
 
    if (root == NULL)
        return -1;
   
    for (trav_open(&trav, root, AB_TRAV_UI);
        (obj = trav_next(&trav)) != NULL; )
        if (obj_is_salient(obj) && !obj_is_menu_item(obj) &&
	   (menu_ref = objxm_comp_get_subobj(obj, AB_CFG_MENU_OBJ)) != NULL)
        {
	    if (menu_ref->ui_handle != NULL)
	    {
		menu = obj_get_actual_obj(menu_ref);

		/* Destroy Temporary Item objs created ONLY for Test-mode */
		num_items = obj_get_num_children(menu_ref);
		item_array = (ABObj*)util_malloc(num_items*sizeof(ABObj));
		for (i=0; i < num_items; i++)
		    item_array[i] = obj_get_child(menu_ref, i);

		for (i=0; i < num_items; i++)
		    obj_destroy(item_array[i]);

		util_free(item_array);

		/* Unless the obj is a MenuButton, disconnect & Destroy the Menu 
		 */
		if (!(obj_is_button(obj) && obj_get_subtype(obj) == AB_BUT_MENU))
		{
	    	    if (obj_is_item(obj) &&
                    	(obj->info.item.type == AB_ITEM_FOR_MENU ||
                     	obj->info.item.type == AB_ITEM_FOR_MENUBAR))
                    	m_type = AB_MENU_PULLDOWN;
            	    else
                    	m_type = AB_MENU_POPUP;

		    if (m_type == AB_MENU_POPUP) /* Remove EventHandler that pops up menu */
		    {
			Widget		parent;
			XtTranslations 	orig_trans;

		    	menu_p = objxm_comp_get_subobj(obj, AB_CFG_MENU_PARENT_OBJ);
		    	XtRemoveEventHandler((Widget)menu_p->ui_handle, ButtonPressMask, 
				False, xm_popup_menu, (XtPointer)obj);
/*
 REMIND: aim, 10/7/94 - In trying to migrate to using translations, it
         messed up the event handling when we returned to BUILD mode.
         For now, going back to event-handling.

			parent = objxm_get_widget(menu_p);
			orig_trans = objxm_get_actions_from_widget(parent);
			XtVaSetValues(parent, XtNtranslations, orig_trans, NULL);
*/
			
		    }
		    XtDestroyWidget((Widget)menu_ref->ui_handle);
		}
		menu_ref->ui_handle = (void*)NULL;
	    }
        }
    trav_close(&trav);
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
    	menu_ref = objxm_comp_get_subobj(obj, AB_CFG_MENU_OBJ);

    	if (menu_ref != NULL && (menu = (Widget)menu_ref->ui_handle) != NULL)
    	{
	    XmMenuPosition(menu, (XButtonPressedEvent*)bevent);
	    XtManageChild(menu);
	}
    }
}

static void
popup_menu(
    Widget      widget,
    XEvent      *event,
    String      *params,
    int         num_params
)
{
    XButtonEvent	*bevent;
    ABObj		obj, menu_ref;
    Widget		menu;

    if (event->type == ButtonPress)
    {
	bevent = (XButtonEvent*)event;
	obj = objxm_get_obj_from_widget(widget); 

	menu_ref = objxm_comp_get_subobj(obj, AB_CFG_MENU_OBJ);

        if (menu_ref != NULL && (menu = (Widget)menu_ref->ui_handle) != NULL)
        {
            XmMenuPosition(menu, (XButtonPressedEvent*)bevent);
            XtManageChild(menu);
        }
    }    
}
