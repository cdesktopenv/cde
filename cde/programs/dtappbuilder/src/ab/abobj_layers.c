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
 *      $XConsortium: abobj_layers.c /main/3 1995/11/06 17:16:01 rswiston $
 *
 * @(#)abobj_layers.c	1.18 02 Feb 1995      cde_app_builder/src/ab
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
 * abobj_layers.c - Implements Layered Panes 
 *
 ***********************************************************************
 */
#include <stdio.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <ab_private/trav.h>
#include <ab_private/obj_notify.h>
#include <ab_private/objxm.h>
#include <ab_private/abobj.h>
#include <ab_private/abobj_set.h>
#include <ab_private/abobj_edit.h>
#include <ab_private/x_util.h>


/*************************************************************************
**                                                                      **
**       Private Function Declarations                                  **
**                                                                      **
**************************************************************************/

static void	set_pane_attachments(
		    ABObj	pane
		);

static int	obj_destroyedOCB(
    		    ObjEvDestroyInfo    info
		);

/*************************************************************************
**                                                                      **
**       Private Data                                                   **
**                                                                      **
**************************************************************************/

static AB_COMPASS_POINT attach_dirs[] = { AB_CP_NORTH, AB_CP_WEST,
					AB_CP_EAST, AB_CP_SOUTH };

/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/

ABObj
abobj_handle_layered_pane(
    ABObj	newobj,
    ABObj	pane	
)
{
    static BOOL 	first_time = True;
    ABObj		pane_parent;
    ABObj		pobj;
    ABObj		layers;
    AB_ATTACH_TYPE 	atype;
    XRectangle 		p_rect;
    void		*aval;
    int			aoffset;
    int			i, pos;

    if (first_time)
    {
       	obj_add_destroy_callback(obj_destroyedOCB, "LAYERS");
	first_time = False;
    }

    pane = obj_get_root(pane);
    pane_parent = obj_get_parent(pane);

    x_get_widget_rect(objxm_get_widget(pane), &p_rect);

    if (!obj_is_layers(pane_parent)) /* Create New Layered pane */
    {

	pobj = objxm_comp_get_subobj(obj_get_root(pane_parent),
			AB_CFG_PARENT_OBJ);

	/* Create Layers Object to hold all layered panes.
	 * Insert the layer obj in the same position as its
	 * child.
	 */
	pos = obj_get_child_num(pane);
	layers = obj_create(AB_TYPE_LAYERS, NULL);
	obj_insert_child(pobj, layers, pos);
	
	obj_set_unique_name(layers, "layers");

	obj_set_width(layers, (int)p_rect.width);
	obj_set_height(layers, (int)p_rect.height);

	/* If the layer is being parented off of a paned window
	 * then the paned window will take care of positioning
	 * the layer object, so we don't need to set the x,y
	 * values. Same applies to the attachments for the new
	 * layer.
	 */
	if (!obj_is_paned_win(pane_parent))
	{
	    obj_set_x(layers, obj_get_x(pane));
	    obj_set_y(layers, obj_get_y(pane));

	    /* Copy original Pane's attachments to the Layers object */
	    for (i=0; i < XtNumber(attach_dirs); i++)
	    {
		atype   = obj_get_attach_type(pane, attach_dirs[i]);
		aval    = obj_get_attach_value(pane,attach_dirs[i]);
		aoffset = obj_get_attach_offset(pane, attach_dirs[i]);
		obj_set_attachment(layers, attach_dirs[i], atype, aval, aoffset);
	    }
	}
	else
	{
	    /* Set the correct position for the layer object,
	     * so that when it is instantiated, it ends up in
	     * the right position in the paned window.
	     */
	    objxm_obj_set_ui_arg(layers, AB_ARG_INT, 
			XmNpositionIndex, (short) pos); 
	}

	/* Reset pane's attachment's to be to all sides of the Layers object */
	set_pane_attachments(pane);

	obj_reparent(pane, layers);
	obj_clear_flag(pane, InstantiatedFlag);
	abobj_show_tree(layers, FALSE);
    }
    else /* Layers already exist */
	layers = pane_parent;

    obj_set_width(newobj, (int)p_rect.width);
    obj_set_height(newobj, (int)p_rect.height);
    obj_set_x(newobj, 0);
    obj_set_y(newobj, 0);
    set_pane_attachments(newobj);

    obj_clear_flag(pane, VisibleFlag);
    XtUnmanageChild(objxm_get_widget(pane));
    obj_set_flag(newobj, VisibleFlag);

    return layers;
}

int
abobj_layer_show_next(
    ABObj	layers
)
{
    AB_TRAVERSAL	trav;
    ABObj		pane;
    ABObj		old_top = NULL;
    ABObj		new_top = NULL;

    if (layers == NULL || !obj_is_layers(layers))
	return -1;

    /* If there is only one pane, do nothing */
    if (obj_get_num_salient_children(layers) <= 1)
	return 0;

    for (trav_open(&trav, layers, AB_TRAV_SALIENT_CHILDREN);
	(pane = trav_next(&trav)) != NULL; )
    {
	/* Search for currently visible pane and mark it */
	if (obj_has_flag(pane, VisibleFlag))
	    old_top = pane;
	else if (old_top != NULL) /* previous pane is visible */
	{
	    new_top = pane;
	    break;
	}
    }
    trav_close(&trav);

    if (new_top == NULL) /* wraparound to first pane */
	new_top = obj_get_salient_child(layers, 0);

    if (old_top)
    	abobj_deselect(old_top);

    XtManageChild(objxm_get_widget(new_top));
    obj_set_flag(new_top, VisibleFlag);

    if (old_top)
    {
    	XtUnmanageChild(objxm_get_widget(old_top));
    	obj_clear_flag(old_top, VisibleFlag);
    }
    abobj_select(new_top); 

    return 0;
}

void
abobj_layer_set_size(
    ABObj	layers,
    int		new_width,
    int		new_height
)
{
    ABObj		pane;
    ABObj		szObj, xyObj;
    Widget		szWidget, xyWidget;
    XRectangle		sz_rect, xy_rect;
    int			border_w;
    AB_TRAVERSAL        trav;

    util_dprintf(3,"abobj_layer_set_size: %s: %d x %d\n",
	obj_get_name(layers), new_width, new_height);

    /* If a dimension is -1, then that dimension is not being modified */
    if (new_width == -1)
	new_width = obj_get_width(layers);
    if (new_height == -1)
	new_height = obj_get_height(layers);


    abobj_set_pixel_size(layers, new_width, new_height, 0);

    for (trav_open(&trav, layers, AB_TRAV_SALIENT_CHILDREN);
        (pane = trav_next(&trav)) != NULL; )
    {
	if (obj_has_border_frame(pane))
	{
	    szObj = objxm_comp_get_subobj(pane, AB_CFG_SIZE_OBJ);
	    xyObj = objxm_comp_get_subobj(pane, AB_CFG_POSITION_OBJ);

	    szWidget = objxm_get_widget(szObj);
	    xyWidget = objxm_get_widget(xyObj);

            x_get_widget_rect(szWidget, &sz_rect);
            x_get_widget_rect(xyWidget, &xy_rect);
            border_w = ((int)(xy_rect.width - sz_rect.width))/2;
        }
        else
            border_w = 0;

        if ((obj_is_text(pane) || obj_is_term_pane(pane)) &&
                   obj_get_num_columns(pane) != -1)
            /* TextPane, TermPane with Character-based Sizing */
            abobj_set_text_size(pane, new_width, new_height);

	/* If Pane has "Fit Contents" size-policy, then don't set new size */
	else if (obj_get_width(pane) != -1 && obj_get_height(pane) != -1)
	    abobj_set_pixel_size(pane, new_width, new_height, border_w);
    }    
    trav_close(&trav);
}

/*
 * Manage the visible pane in the layer, and unmanage the rest
 */
int
abobj_layer_manage_visible(
    ABObj	layer
)
{
    AB_TRAVERSAL	trav;
    ABObj		pane;

    if (layer == NULL || !obj_is_layers(layer))
	return -1;

    /* manage the visible pane, unmanage the rest */
    for (trav_open(&trav, layer, AB_TRAV_SALIENT_CHILDREN);
	(pane = trav_next(&trav)) != NULL; )
    {
	if (obj_has_flag(pane, VisibleFlag))
	{
	    if (!XtIsManaged(objxm_get_widget(pane)))
		XtManageChild(objxm_get_widget(pane));
	}
	else
	{
	    if (XtIsManaged(objxm_get_widget(pane)))
		XtUnmanageChild(objxm_get_widget(pane));
	}
    }
    trav_close(&trav);

    return 0;
}


/*
 * Make all 4 sides of pane attached to Layers object
 */
static void
set_pane_attachments(
    ABObj	pane
)
{
    obj_set_attachment(pane, AB_CP_NORTH, AB_ATTACH_GRIDLINE, 0, 0);
    obj_set_attachment(pane, AB_CP_WEST, AB_ATTACH_GRIDLINE, 0, 0);
    obj_set_attachment(pane, AB_CP_EAST, AB_ATTACH_GRIDLINE, (void*)100, 0);
    obj_set_attachment(pane, AB_CP_SOUTH, AB_ATTACH_GRIDLINE, (void*)100, 0);

}

/*
 * obj-callback: object is being destroyed - remove from Prop dialog lists
 */
static int
obj_destroyedOCB(
    ObjEvDestroyInfo    info
)
{
    static ABObj	pane = NULL;
    ABObj		layers;
    ABObj		last_pane;
    AB_ATTACH_TYPE      atype;
    void                *aval;
    int                 aoffset;
    int			i;

    if (obj_get_parent(info->obj) == NULL)
	return 0;

    layers = obj_get_root(obj_get_parent(info->obj));

    if (obj_is_layers(layers))
    {
	/* A Pane inside the Layers has been destroyed...*/
	pane = info->obj;

	/* If it was the one currently visible, then show the next one
	 */
	if (obj_has_flag(pane, VisibleFlag))
    	    abobj_layer_show_next(layers);

    	if (obj_get_num_salient_children(layers) == 1)
	{
	    /* Since there is only 1 pane left, we no longer need the layers.
	     * So, reparent the last pane to the layer's parent and destroy
	     * the layers object.
	     */
	    last_pane = obj_get_child(layers, 0);

	    /*
	     * Update undo buffer to depend on this last pane for undo
	     * instead of the parent (layer) which is about to be nuked
	     */
	    abobj_setup_undo_cut_layer(layers, last_pane);

	    /* Reparenting the last layer causes the obj_notify function to
	     * flush its queue, which in turn causes this function to be
	     * called again with the same object to destroy.  Therefore we
	     * need to track when we are in the reparent phase so that we
	     * don't get into an infinite loop...
	     */
            obj_unparent(last_pane);
	    obj_append_child(obj_get_parent(layers), last_pane);

	    /* Copy layers object's attachments back to the last pane
	     * so that it remains positioned in the same spot.
	     */
            for (i=0; i < XtNumber(attach_dirs); i++)
            {
                atype   = obj_get_attach_type(layers, attach_dirs[i]);
                aval    = obj_get_attach_value(layers,attach_dirs[i]);
                aoffset = obj_get_attach_offset(layers, attach_dirs[i]);
                obj_set_attachment(last_pane, attach_dirs[i], atype, aval, aoffset);
            }
            obj_destroy(layers);
            obj_clear_flag(last_pane, InstantiatedFlag);
            abobj_show_tree(last_pane, FALSE);
	}
    }

    return 0;
}
