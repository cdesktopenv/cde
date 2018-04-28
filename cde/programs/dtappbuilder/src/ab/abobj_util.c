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
 *      $XConsortium: abobj_util.c /main/3 1995/11/06 17:17:50 rswiston $
 *
 * @(#)abobj_util.c	1.20 16 Feb 1994      cde_app_builder/src/ab
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
 * abobj_util.c - miscellaneous AB object utility functions
 *	
 *
 ***********************************************************************
 */
#include <stdio.h>
#include <sys/param.h>
#include <X11/Intrinsic.h>
#include <X11/Composite.h>
#include <X11/Shell.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/ScrolledW.h>
#include <ab_private/trav.h>
#include <ab_private/objxm.h>
#include <ab_private/ab.h>
#include <ab_private/proj.h>
#include <ab_private/prop.h>
#include <ab_private/brws.h>
#include <ab_private/x_util.h>
#include "abobjP.h"
#include <ab_private/conn.h>
#include <ab_private/obj_notify.h>


/*************************************************************************
**
**
**       Private Function Declarations
**
**
**
**************************************************************************/

/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/

/*
 * XmConfigure, Instantiate & Set Build-actions on a
 * UI object hierarchy
 */
int
abobj_instantiate_tree(
    ABObj	root,
    BOOL	manage_last
)
{
    int iRet = 0;
 
    if (root == NULL)
        iRet = -1;
 
    if (iRet == 0 && !obj_has_flag(root, XmConfiguredFlag))
        iRet = objxm_tree_configure(root, OBJXM_CONFIG_BUILD);
 
    if (iRet == 0 && !obj_has_flag(root, InstantiatedFlag))
    {
        iRet = objxm_tree_instantiate(root, manage_last);

	/*
	 * Set event handlers for dynamic centering behaviour
	 */
        abobj_tree_set_centering_handler(root);
    }   
    return iRet;

}

/*
 * XmConfigure, Instantiate, Set Build-actions & Map
 * a UI object hierarchy
 */
int
abobj_show_tree(
    ABObj	root,
    BOOL	manage_last
)
{
    int iRet = 0;

    iRet = abobj_instantiate_tree(root, manage_last);

    if (iRet == 0)
    {
        abobj_tree_set_build_actions(root);
	iRet = objxm_tree_map(root, TRUE);
    }

    obj_tree_update_clients(root);

    return iRet;

}

/*
 * Unmap a UI obj hierarchy
 */
int
abobj_hide_tree(
    ABObj	root
)
{
    int iRet = 0;

    if (root == NULL) 
        iRet = -1; 

    if (iRet == 0 && obj_has_flag(root, MappedFlag))
	iRet = objxm_tree_map(root, FALSE);

    obj_tree_update_clients(root);

    return iRet;

}


/*
 * Get the actual width of the widget associated with object
 */
int
abobj_get_actual_width(
    ABObj	obj
)
{
    Dimension 	width = 0;
    Widget	widget;

    widget = objxm_get_widget(obj);

    if (widget != NULL)
        XtVaGetValues(widget, XmNwidth, &width, NULL);

    return((int)width);
}

/*
 * Get the actual height of the widget associated with object
 */
int
abobj_get_actual_height(
    ABObj	obj
)
{
    Dimension 	height = 0;
    Widget      widget;
 
    widget = objxm_get_widget(obj); 
 
    if (widget != NULL) 
        XtVaGetValues(widget, XmNheight, &height, NULL);

    return((int)height);
}

/*
 * Get the Actual width of the Composite Object
 */
int
abobj_get_comp_width(
    ABObj	obj
)
{
    ABObj	szObj;
    int		width;

    if ((width = obj_get_width(obj)) == -1)
    {
	szObj = objxm_comp_get_subobj(obj, AB_CFG_SIZE_OBJ);
	width = abobj_get_actual_width(szObj);
    }
    return width;
}

/* 
 * Get the Actual height of the Composite Object 
 */ 
int 
abobj_get_comp_height(
    ABObj       obj 
) 
{
    ABObj       szObj;
    int         height;
 
    if ((height = obj_get_height(obj)) == -1) 
    { 
        szObj = objxm_comp_get_subobj(obj, AB_CFG_SIZE_OBJ); 
        height = abobj_get_actual_height(szObj); 
    }   
    return height; 
}


/*
 * Get the actual x position of the widget since x
 * can be -1 for immovable objects.
 */
int
abobj_get_x(
    ABObj       obj
)
{
    ABObj	xyObj = obj;

    if (obj->x == -1)
	xyObj = objxm_comp_get_subobj(obj, AB_CFG_POSITION_OBJ);

    return(abobj_get_actual_x(xyObj));
}

/* 
 * Get the logical x position of the object - precedence
 * goes to the value of the attribute (if not -1) over the 
 * actual x position of the widget because this routine can get
 * called before the attribute value has actually been applied
 * to the widget.
 */ 
int
abobj_get_comp_x(
    ABObj	obj
)
{
    int		x;
    ABObj	xyObj;
    ABObj	pobj = obj_get_root(obj_get_parent(obj));

    /* If obj is a layered pane, then return the position of its 
     * parent (the layers obj). 
     */ 
    if (obj_is_layers(pobj))
	obj = pobj;

    if ((x = obj_get_x(obj)) == -1)
    {
        xyObj = objxm_comp_get_subobj(obj, AB_CFG_POSITION_OBJ);
        x = abobj_get_actual_x(xyObj);
    }   
    return x;
}
/*
 * Get the x position of the widget since x attribute
 * can be -1 for immovable objects.
 */
int
abobj_get_actual_x(
    ABObj	obj
)
{
    Position    w_x = 0;
    Widget	widget;

    widget = objxm_get_widget(obj);
 
    if (widget != NULL)
        XtVaGetValues(widget, XmNx, &w_x, NULL);
    else
        util_dprintf(1, "abobj_get_actual_x: %s : null widget\n",
                obj_get_name(obj));
 
    return ((int)w_x);
}

/* 
 * Get the y position of the widget since the y attribute
 * can be -1 for immovable objects.
 */
int 
abobj_get_y(
    ABObj       obj 
) 
{ 
    ABObj 	xyObj = obj;
 
    if (obj->y == -1) 
	xyObj = objxm_comp_get_subobj(obj, AB_CFG_POSITION_OBJ);
 
    return(abobj_get_actual_y(xyObj));
} 

/*
 * Get the logical y position of the object - precedence
 * goes to the value of the attribute (if not -1) over the
 * actual y position of the widget because this routine can get
 * called before the attribute value has actually been applied
 * to the widget.
 */
int
abobj_get_comp_y(
    ABObj       obj
)
{
    int         y;
    ABObj       xyObj;
    ABObj       pobj = obj_get_root(obj_get_parent(obj));

    /* If obj is a layered pane, then return the position of its
     * parent (the layers obj).
     */
    if (obj_is_layers(pobj))
        obj = pobj;

    if ((y = obj_get_y(obj)) == -1)
    {
        xyObj = objxm_comp_get_subobj(obj, AB_CFG_POSITION_OBJ);
        y = abobj_get_actual_y(xyObj);
    }
    return y;
}

int 
abobj_get_actual_y( 
    ABObj       obj
)
{
    Position    w_y = 0;
    Widget      widget;

    widget = objxm_get_widget(obj);

    if (widget != NULL)
        XtVaGetValues(widget, XmNy, &w_y, NULL);
    else
        util_dprintf(1, "abobj_get_actual_y: %s : null widget\n",
                obj_get_name(obj));
 
    return ((int)w_y);
}

/*
 * Get the label width of an object.
 * For objects without labels, it is the width of the object.
 * For groups, it is the width to the end of the first label.
 */
int
abobj_get_label_width(
    ABObj	obj
)
{
    Dimension width;
    ABObj     l_obj;

    l_obj = (ABObj)objxm_comp_get_subobj(obj, AB_CFG_LABEL_OBJ);

    if (obj_is_group(obj))
    {
	ABObj pobj, child;
        pobj = (ABObj)objxm_comp_get_subobj(obj, AB_CFG_PARENT_OBJ);
	child = obj_get_child(pobj, 0);
	if (child != NULL)
	    width = abobj_get_label_width(child);
	else
	    width = 0;
    }
    else if (l_obj != obj)
    {
	if (l_obj != NULL)
	    XtVaGetValues(objxm_get_widget(l_obj),
			XmNwidth,  &width,
			NULL);
	else /* No label */
	    width = 0;
    }
    else 
	XtVaGetValues(objxm_get_widget(obj),
			XmNwidth,  &width,
			NULL);

    return ((int)width);
}

void
abobj_get_rect_for_objects(
    ABObj      *list,
    int         count,
    XRectangle *j_rect
)
{
    XRectangle r1;
    int        i;

    j_rect->width = 0;
    j_rect->height = 0;

    for (i=0; i < count; i++)
    {
        x_get_widget_rect(objxm_get_widget(list[i]), &r1);
	x_get_rect_bounding(j_rect, &r1);
    }
}


/*
 * Find the greatest width, height in a list of objects. Also return
 * the object with the greatest width and the one with the greatest
 * height. Assumes that the list contains only objects that are
 * of valid type (basically AB_CFG_CONFIG_OBJ).
 */
void
abobj_get_greatest_size(
    ABObj      *list,
    int         count,
    int		*width,
    int		*height,
    ABObj	*tallest_obj,
    ABObj	*widest_obj
)
{
    XRectangle  w_rect;
    int         i;
    int		previous_width, previous_height;

    x_get_widget_rect(objxm_get_widget(list[0]), &w_rect);

    *width = w_rect.width;
    previous_width = *width;

    *height = w_rect.height;
    previous_height = *height;

    if (tallest_obj != NULL)
        *tallest_obj = list[0];

    if (widest_obj != NULL)
    	*widest_obj = list[0];

    for (i=0; i < count; i++)
    {
        x_get_widget_rect(objxm_get_widget(list[i]), &w_rect);

        *width = max((int) w_rect.width, (int) *width); 
	if (widest_obj != NULL && *width > previous_width)
		*widest_obj = list[i];

        *height = max((int) w_rect.height, (int)*height);
	if (tallest_obj != NULL && *height > previous_height)
		*tallest_obj = list[i];
    }
}

int
abobj_get_value_x(
	ABObj	obj
)
{
    ABObj o_obj;
    Dimension  x, valuex, width;

    o_obj = (ABObj)objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);

    if (o_obj != obj)
    {
	XtVaGetValues(objxm_get_widget(obj),
			XmNx,      &x,
			NULL);
	XtVaGetValues(objxm_get_widget(o_obj),
			XmNx,   &valuex,
			NULL);
	x += valuex;	
    }
    else 
    {
	XtVaGetValues(objxm_get_widget(obj),
			XmNx,      &x,
			XmNwidth,  &width,
			NULL);
  	x += width;	
    }

    return ((int)x);
}

BOOL
abobj_has_attached_label(
    ABObj		obj
)
{
    BOOL	has_label;

    switch(obj_get_type(obj))
    {
	case AB_TYPE_CHOICE:
	case AB_TYPE_COMBO_BOX:
	case AB_TYPE_SPIN_BOX:
	case AB_TYPE_TEXT_FIELD:
	case AB_TYPE_LIST:
	case AB_TYPE_SCALE:
	    has_label = obj_has_label(obj);
	    break;
	default:
	    has_label = False;
    }
    return has_label;
}


/*
 * WORKAROUND: Motif XmForm bug won't honor child resize requests,
 * 	       so we have to force it by changing the size of the
 *	       form momentarily.  UGLY!!!
 */
void
abobj_force_dang_form_resize(
    ABObj  obj
)
{
/*
REMIND: aim,1/12/95 - I believe this is no longer necessary, however
 	I am leaving this in (commented out) for awhile to be sure.

    Dimension p_width, p_height;
    ABObj     parentobj;
    Widget    parent;

    if (objxm_get_widget(obj)== NULL)
	return;

    parentobj = obj_get_parent(obj);
    parent    = objxm_get_widget(parentobj);
    
    if (XtIsSubclass(parent, xmFormWidgetClass))
    {
    	XtVaGetValues(parent, XmNwidth, &p_width, XmNheight, &p_height, NULL);
    	XtVaSetValues(parent, XmNwidth, p_width+1,XmNheight, p_height+1,NULL);
    	XtVaSetValues(parent, XmNwidth, p_width,  XmNheight, p_height,  NULL);
    }
*/

}

/*
 * Alloc & return a STRING of the form: 
 * 	"modulename :: objectname"
 */
STRING
abobj_alloc_moduled_name(
    STRING	mod_name,
    STRING	obj_name
)
{
    static char full_name[BUFSIZ];
    STRING	moduled_name;

    if (mod_name == NULL)
	strcpy(full_name, "**unnamed module**");
    else
	strcpy(full_name, mod_name);

    strcat(full_name, " :: "); 

    if (obj_name == NULL)
	strcat(full_name, "**unnamed object**");
    else
	strcat(full_name, obj_name);

    moduled_name = XtNewString(full_name);
 
    return(moduled_name);
}
    

/*
 * Given an object, return a STRING in the form:
 * 	"modulename :: objectname"
 * or, for a project:
 *	"Application :: projectname"
 */
STRING
abobj_get_moduled_name(
    ABObj       obj
)
{
    ABObj	module;
    STRING      obj_name;
    STRING      mod_name;
    STRING	moduled_name;

    if (obj_is_project(obj))
	mod_name = "Application";
    else if ((module = obj_get_module(obj)) == NULL)
	mod_name = "**null module**";
    else
	mod_name = obj_get_name(module);

    if (obj == NULL)
	obj_name = "**null object**";
    else
	obj_name = obj_get_name(obj);

    moduled_name = abobj_alloc_moduled_name(mod_name, obj_name);
 
    return(moduled_name);
}

/*
 * Given a STRING of the form "modulename :: objectname",
 * return the corresponding Module & Object 
 */
int
abobj_moduled_name_extract(
    STRING	m_o_name,
    ABObj	*module,
    ABObj	*obj
)
{
    STRING	modobjname = XtNewString(m_o_name);
    STRING	modname;
    STRING	objname;
    int		iRet = 0;

    if (modobjname == NULL)
    {
	if (util_get_verbosity() > 0)
	    fprintf(stderr,"abobj_moduled_name_extract: NULL module-object name passed\n");
	iRet = -1;
    }
    else
    {
    	modname = strtok(modobjname, " ::");
	strtok(NULL, " ");
    	objname = strtok(NULL, "");

    	if (modname != NULL && objname != NULL)
    	{
    	    if ((*module = obj_find_module_by_name(proj_get_project(), modname)) != NULL)
    	    	*obj = obj_scoped_find_by_name(*module, objname);
	    else
	    {
		if (util_get_verbosity() > 0)
		    fprintf(stderr,"abobj_moduled_name_extract: couldn't find module: %s\n",
			modname);
		iRet = -1;
	    }
	}
	else
	{
	    if (util_get_verbosity() > 0) 
		fprintf(stderr,"abobj_moduled_name_extract: couldn't extract names for module or object\n");
	    iRet = -1;
	}
    }
    if (iRet == -1)
	*module = *obj = NULL;

    XtFree(modobjname);
    return iRet;

}

/*
 * Rename all SubObjs within an Object based on New object-name
 */
int
abobj_comp_rename(
    ABObj	root,
    STRING	oldname,
    STRING	newname
)
{
    AB_TRAVERSAL trav;
    ABObj        subobj;
    STRING	 old_subname;
    STRING	 new_subname;
    char	 *sfx;
    int		 sfx_index;
 
    /* Change name-base of all subobj to be new obj name */
    for (trav_open(&trav, root, AB_TRAV_COMP_SUBOBJS);
        (subobj = trav_next(&trav)) != NULL; )
    {
	if (subobj != root)
	{
	    old_subname = obj_get_name(subobj);
	    sfx_index = strlen(oldname) + 1;
	    if (sfx_index < strlen(old_subname)) 
	    {
	    	sfx = &(old_subname[sfx_index]);
	    	new_subname = ab_ident_from_name_and_label(newname, sfx);
#ifdef DEBUG
	    	util_dprintf(5,"changing subobj \"%s\" to \"%s\"\n",
			old_subname, new_subname);
#endif
	    	obj_set_name(subobj, new_subname);
	    }
	}
    }
    trav_close(&trav);
    return 0;
}

int
abobj_update_module_name(
    ABObj       modobj
)
{
    int		num_win, i;
    ABObj       winobj, statobj;
    XmString    xmname;
    String	modname = obj_get_name(modobj);

    if (!obj_is_module(modobj))
        return -1;

    num_win = obj_get_num_children(modobj);

    for(i = 0; i < num_win; i++)
    {
	winobj = obj_get_child(modobj, i);
    	statobj = objxm_comp_get_subobj(winobj, AB_CFG_STATUS_OBJ);
    	if (statobj && objxm_get_widget(statobj) != NULL)
    	{
            xmname = XmStringCreateLocalized(modname);
            XtVaSetValues(objxm_get_widget(statobj),
            	XmNlabelString,     xmname,
            	NULL);
            XmStringFree(xmname);
        }
    }
 
    return 0;
}

int
abobj_update_proj_name(
    ABObj       proj_obj
)
{ 
    STRING	new_title = NULL;
    STRING	proj_win_title = NULL;
    STRING	proj_name = NULL;
 
    if( !obj_is_project(proj_obj) ) 
        return -1;  
 
    if( AB_proj_window == NULL )
    {	
	return 0;
    }

    /* Update the project window title */
    proj_name = obj_get_name(proj_obj);
    if (!util_strempty(proj_name))
    {
    	proj_win_title = (STRING) util_malloc(strlen(proj_name) +
			strlen(".bip") + 1); 
    	strcpy(proj_win_title, proj_name);
    }
    else
    {
	proj_win_title = (STRING) util_malloc(
		strlen(catgets(Dtb_project_catd, 100, 104, "NoName")) 
		+ strlen(".bip") +1);
    	strcpy(proj_win_title, 
		catgets(Dtb_project_catd, 100, 104, "NoName"));
    }

    strcat(proj_win_title, ".bip");
    new_title = (STRING) util_malloc(
		strlen(catgets(Dtb_project_catd, 10, 1, "Project Organizer"))
		+ strlen(" - ") + strlen(proj_win_title) +1);

    strcpy(new_title, 
	catgets(Dtb_project_catd, 10, 1, "Project Organizer"));
    strcat(new_title, " - "); 
    strcat(new_title, proj_win_title); 
    util_free(proj_win_title);

    XtVaSetValues(XtParent(AB_proj_window), XmNtitle,  new_title, NULL);
    
    /*
     * The project directory is always the current directory
     */
    proj_update_stat_region(PROJ_STATUS_PROJ_PATH, ab_get_cur_dir());

    return 0;
}

int
abobj_update_palette_title(
    ABObj       proj_obj
)
{ 
    STRING      	new_title = NULL;
    STRING      	proj_win_title = NULL;
    STRING      	proj_name = NULL;
    AB_TRAVERSAL	trav;
    BOOL		SaveNeeded = FALSE;
    ABObj		mod;
 
    if( !obj_is_project(proj_obj) ) 
        return -1;  

    proj_name = obj_get_name(proj_obj); 
    if (!util_strempty(proj_name))
    {
        proj_win_title = (STRING) util_malloc(strlen(proj_name) +
                        strlen(".bip") + 1);
        strcpy(proj_win_title, proj_name);
    }
    else
    {
        proj_win_title = (STRING) util_malloc(
                strlen(catgets(Dtb_project_catd, 100, 104, "NoName"))
                + strlen(".bip") +1);
        strcpy(proj_win_title,
                catgets(Dtb_project_catd, 100, 104, "NoName"));
    }
    strcat(proj_win_title, ".bip");
 
    for (trav_open(&trav, proj_obj, AB_TRAV_MODULES);
	((mod = trav_next(&trav)) != NULL) && !SaveNeeded; )
    {
	if (obj_has_flag(mod, SaveNeededFlag))
	   SaveNeeded = TRUE;
    }
    if (obj_has_flag(proj_obj, SaveNeededFlag))
	SaveNeeded = TRUE;

    new_title = (STRING) util_malloc(
                strlen(catgets(Dtb_project_catd, 10, 5, "Application Builder"))
                + strlen(" - ") + strlen(proj_win_title) + 
		+ strlen(catgets(Dtb_project_catd, 100, 244, " (Save Needed)")) 
		+1);

    strcpy(new_title, 
	catgets(Dtb_project_catd, 10, 5, "Application Builder"));
    strcat(new_title, " - ");
    strcat(new_title, proj_win_title); 
    util_free(proj_win_title);

    if (SaveNeeded)
    {
	strcat(new_title, 
	    catgets(Dtb_project_catd, 100, 244, " (Save Needed)"));
    }

    XtVaSetValues(AB_toplevel, XmNtitle,  new_title, NULL); 
    return 0;
}

/*
 * Duplicates one ABObj.
 * Copies the attachments, and resets the flags. Makes the object 
 * somewhat ready for pasting.
 */
ABObj
abobj_dup(ABObj obj)
{
    ABObj	newObj= obj_dup(obj);
    STRING	name;

    if (newObj == NULL)
    {
	return NULL;
    }

    if (obj->attachments)
    {
        obj_init_attachments(newObj);
	*(newObj->attachments) = *(obj->attachments);
    }

    obj_tree_clear_flag(newObj, 
	AttrChangedFlag | 
	XmConfiguredFlag |
	InstantiatedFlag | 
	MappedFlag | 
	BuildActionsFlag);

    /*
    name = obj_get_unique_name(obj, obj_get_module(obj), 
                    obj_get_name(obj), -1);

    obj_set_name(newObj, name);
    */

    return newObj;
}

/*
 * Duplicates an ABObj, and it's salient children.
 * Copies the attachments, and resets the flags. Makes the object 
 * somewhat ready for pasting.
 */
ABObj
abobj_dup_tree(ABObj obj)
{
    AB_TRAVERSAL	trav;
    ABObj		newObj = NULL,
			child,
			newChild;
    
    if (!obj || !obj_is_salient(obj))
	return (newObj);

    newObj = abobj_dup(obj);

    for (trav_open(&trav, obj, AB_TRAV_SALIENT_CHILDREN);
        (child = trav_next(&trav)) != NULL; )
    {
	newChild = abobj_dup_tree(child);

	obj_append_child(newObj, newChild);
    }

    trav_close(&trav);

    return (newObj);
}

/*
 * Determines the Positioning attributes for a NEWLY created Pane object
 */
int
abobj_init_pane_position(
    ABObj	paneobj
)
{
    Dimension	p_width, p_height;
    ABObj	parent = obj_get_parent(paneobj);

    if (obj_is_layers(obj_get_root(parent)))
    {
        obj_set_attachment(paneobj, AB_CP_NORTH, AB_ATTACH_GRIDLINE, 0, 0);
        obj_set_attachment(paneobj, AB_CP_WEST,  AB_ATTACH_GRIDLINE, 0, 0);
        obj_set_attachment(paneobj, AB_CP_EAST,  AB_ATTACH_GRIDLINE, (void*)100, 0);
        obj_set_attachment(paneobj, AB_CP_SOUTH, AB_ATTACH_GRIDLINE, (void*)100, 0);
    }
    else
    {   
        /* Set up Default Layout for Control Panel */
        XtVaGetValues(objxm_get_widget(parent),
                XmNwidth,       &p_width,
                XmNheight,      &p_height,
                NULL);

        /* Snap x,y to edge of parent if already close */
        if (paneobj->x <= AB_grid_size)
            paneobj->x = 0;
        else if ((paneobj->x + paneobj->width) >= ((int)p_width - AB_grid_size))
            paneobj->x = (int)p_width - (paneobj->width + 1);
        if (paneobj->y <= AB_grid_size)
            paneobj->y = 0;
        else if ((paneobj->y + paneobj->height) >= ((int)p_height - AB_grid_size))
            paneobj->y = (int)p_height - (paneobj->height + 1);

        obj_set_attachment(paneobj, AB_CP_NORTH, AB_ATTACH_POINT, NULL, paneobj->y);
        obj_set_attachment(paneobj, AB_CP_WEST,  AB_ATTACH_POINT, NULL, paneobj->x);
    }

    return 0;

}

static int
reset_widget_colors(
    Widget	widget,
    BOOL	reset_bg,
    BOOL	reset_fg
)
{
    static Widget	dummy_shell = NULL;
    Widget		dummy,
			dummy_parent;
    Pixel		default_bg,
			default_fg;
    Arg			args[2];
    int			n = 0;

    if (widget == NULL)
	return -1;

    dummy_parent = XtParent(widget);

    if (XtIsShell(dummy_parent)) /* shell can only have 1 child */
    {
	/* Create and cache a dummy shell widget to keep around
	 * for cases where the widget parent is already a shell
	 * and cannot be re-used.
	 */
        if (dummy_shell == NULL)
            dummy_shell = XtCreatePopupShell("dummy_shell",
                       transientShellWidgetClass, AB_toplevel, NULL, 0);
        dummy_parent = dummy_shell;
    }
 
    /* REMIND: aim - revisit when have more time
     * To get default color, temporarily create a Widget to
     * extract the default color from....this is not optimal and
     * should be replaced with a scheme to cache default colors.
     */
    dummy = XtCreateWidget("dummy", XtClass(widget),
                                dummy_parent, NULL, 0);
    XtVaGetValues(dummy,
                        XmNbackground, &default_bg,
                        XmNforeground, &default_fg,
                        NULL);
    if (reset_bg)
    {
	XtSetArg(args[n], XmNbackground, default_bg); n++;
    }
    if (reset_fg)
    {
	XtSetArg(args[n], XmNforeground, default_fg); n++;
    }

    XtSetValues(widget, args, n);

    XtDestroyWidget(dummy);

    return 0;
}

int
abobj_reset_colors(
    ABObj       obj,
    BOOL        reset_bg,
    BOOL        reset_fg
)
{
    AB_TRAVERSAL 	trav;
    ABObj       	subobj;
    Widget      	widget;

    /* Reset color for entire Composite Object */
    for (trav_open(&trav, obj, AB_TRAV_COMP_SUBOBJS);
         (subobj= trav_next(&trav)) != NULL; )
    {
        if ((widget = objxm_get_widget(subobj)) != NULL)
	    reset_widget_colors(widget, reset_bg, reset_fg);
    }
    trav_close(&trav);

    return OK;
}


/*
 * Traverse tree and set the mode of behavior ("build" or "test")
 * for each object with a valid ui_handle
 */
int
abobj_tree_set_centering_handler(
    ABObj    	root
)
{
    AB_TRAVERSAL    trav;
    ABObj       obj;
 
    if (root == NULL)
	return ERROR;

    for (trav_open(&trav, root, AB_TRAV_UI);
        (obj = trav_next(&trav)) != NULL; )
    {
	Widget		w;

	w = objxm_get_widget(obj);

	if (w != NULL)
	{
	    DTB_CENTERING_TYPES	type;

	    type = abobj_get_centering_type(obj);

	    if (type != DTB_CENTER_NONE)
		dtb_center(w, type);
       	}

    }
    return OK;
}

DTB_CENTERING_TYPES
abobj_get_centering_type(
    ABObj obj
)
{
    AB_ATTACH_TYPE	left_attach_type,
			top_attach_type;

    if (!obj)
	return(DTB_CENTER_NONE);
    
    left_attach_type = obj_get_attach_type(obj, AB_CP_WEST);
    top_attach_type = obj_get_attach_type(obj, AB_CP_NORTH);

    if ((left_attach_type == AB_ATTACH_CENTER_GRIDLINE) && 
	(top_attach_type != AB_ATTACH_CENTER_GRIDLINE))
	return(DTB_CENTER_POSITION_VERT);

    if ((left_attach_type != AB_ATTACH_CENTER_GRIDLINE) && 
	(top_attach_type == AB_ATTACH_CENTER_GRIDLINE))
	return(DTB_CENTER_POSITION_HORIZ);

    if ((left_attach_type == AB_ATTACH_CENTER_GRIDLINE) && 
	(top_attach_type == AB_ATTACH_CENTER_GRIDLINE))
	return(DTB_CENTER_POSITION_BOTH);

    return(DTB_CENTER_NONE);
}

/*
 * Builds an array of all menus inside a project or module
 *
 * Note: the caller is responsible for freeing the memory allocated
 *       by this function
 */
int
abobj_build_menus_array(
    ABObj	root,
    ABObj       **menus_ptr,
    int         *menu_count_ptr
)
{
    AB_TRAVERSAL        trav;
    ABObj		modules[256];
    int			num_modules = 0;
    ABObj               module, obj;
    int                 num_children;
    int                 i, j, k;

    *menu_count_ptr = 0;

    if (obj_is_project(root)) /* Getting all menus in project */
    {
	/* Load modules into an array */
    	for (trav_open(&trav, root, AB_TRAV_MODULES);
             (module= trav_next(&trav)) != NULL; )
	    modules[num_modules++] = module;

	trav_close(&trav);
    }
    else if (obj_is_module(root)) /* Getting menus in 1 module */
	modules[num_modules++] = root;
    else
	return -1;

    for (i = 0; i < num_modules; i++)
    {
	/* Count number of menus to determine the array size */
        num_children = obj_get_num_children(modules[i]);
        for (j = 0; j < num_children; j++)
        {
            obj = obj_get_child(modules[i], j);
            if (obj_is_menu(obj))
                (*menu_count_ptr)++;
        }
    }

    if (*menu_count_ptr > 0)
    {
        /* Store Menus in an array */
        *menus_ptr = (ABObj*)util_malloc(*menu_count_ptr*sizeof(ABObj));
        k = 0;

	for(i = 0; i < num_modules; i++)
  	{
            num_children = obj_get_num_children(modules[i]);
            for (j = 0; j < num_children; j++)
            {
                obj = obj_get_child(modules[i], j);
                if (obj_is_menu(obj))
                    (*menus_ptr)[k++] = obj;
            }
        }
    }
    return 0;
}

/*
 * Given a STRING of the form "Application :: projectname",
 * return the corresponding Object 
 */
int
abobj_project_name_extract(
    STRING	app_name,
    ABObj	*obj
)
{
    STRING	appobjname = XtNewString(app_name);
    STRING	appname = NULL;
    STRING	objname = NULL;
    int		iRet = 0;

    if (appobjname == NULL)
    {
	if (util_get_verbosity() > 0)
	    fprintf(stderr,"abobj_project_name_extract: NULL project-object name passed\n");
	iRet = -1;
    }
    else
    {
    	appname = strtok(appobjname, " ::");
	strtok(NULL, " ");
    	objname = strtok(NULL, "");

    	if (objname != NULL)
    	{
	    if (strcmp(obj_get_name(proj_get_project()), objname) == 0)
		*obj = proj_get_project();
	    else
		iRet = -1;
	}
	else 
	    iRet = -1;
    }
    if (iRet == -1)
	*obj = NULL;

    XtFree(appobjname);
    return iRet;
}
