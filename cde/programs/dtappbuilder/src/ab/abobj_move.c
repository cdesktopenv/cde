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
 *      $XConsortium: abobj_move.c /main/3 1995/11/06 17:17:00 rswiston $
 *
 * @(#)abobj_move.c	1.25 15 Feb 1994      cde_app_builder/src/ab
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
 * ab_move.c - Implements moving UI objects around
 *        
 *
 ***********************************************************************
 */
#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <ab_private/x_util.h>
#include <ab_private/objxm.h>
#include <ab_private/abobj_edit.h>
#include "abobjP.h"

/* XRectangle Transformation macros used for passing rects up/down embedded
 * coordinate systems.
 */
#define	rect_passtoparent(a, b, rect) \
	{(rect)->x = (unsigned short)(rect)->x + (a); (rect)->y = (unsigned short)(rect)->y + (b);}

#define	rect_passtochild(a, b, rect) \
	{(rect)->x = (rect)->x - (a); (rect)->y = (rect)->y - (b);}

const int    AB_drag_threshold = 4;


static void	undo_move(
		    ABUndoRec	undo_rec
		);

/*************************************************************************
**                                                                      **
**       Private Data                                                   **
**                                                                      **
**************************************************************************/
static XRectangle drag_init_rect, orig_rect, move_rect;
static Boolean    first_move = True;
static ABObj      xy_obj;
static Widget     xy_widget;
static Widget	  parent;
static ABSelectedRec sel; 

/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/
int
abobjP_move_object_outline(
    ABObj         	obj, 
    XMotionEvent        *mevent
)
{
    static XRectangle parent_rect;
    static XRectangle last_rect;
    static int        x_offset, y_offset;
    static Dimension  border_w;
    static Display    *dpy;
    XRectangle	      widget_rect;
    int               trans_x, trans_y;

    /* First time: set up initial move variables */
    if (first_move)
    {
	if (obj_is_item(obj))
	    obj = obj_get_parent(obj);

	obj    = obj_get_root(obj);
	
        /* Multiple objects might be selected...*/ 
        if (obj_is_control(obj) || 
	    obj_is_group(obj) ||
	    obj_is_pane(obj)) 
        { 
            abobj_get_selected(obj_get_root(obj_get_parent(obj)), False, False, &sel);
        } 
        else
	{
            sel.count = 1; 
	    sel.list = (ABObj*)util_malloc(sizeof(ABObj));
	    sel.list[0] = obj;
	}

	xy_obj = objxm_comp_get_subobj(obj, AB_CFG_POSITION_OBJ);
        xy_widget = (Widget)xy_obj->ui_handle;

        if (xy_widget == NULL)
        {
            if (util_get_verbosity() > 2)
                fprintf(stderr,"abobjP_move_object_outline: %s :no POSITION widget\n",
			util_strsafe(obj_get_name(obj)));
            return ERROR;
        }

        dpy    = XtDisplay(xy_widget);
        parent = XtParent(xy_widget);

	x_get_widget_rect(xy_widget, &widget_rect);
        x_get_widget_rect(parent, &parent_rect);

	if (sel.count > 1)
	{
            abobj_get_rect_for_objects(sel.list, sel.count, &orig_rect);
	} 
	else
	{
	    orig_rect = widget_rect;
	    XtVaGetValues(xy_widget, XtNborderWidth, &border_w, NULL);
	    orig_rect.width  += (2*border_w);
	    orig_rect.height += (2*border_w);
	    orig_rect.width--;
	    orig_rect.height--;
	}
	move_rect = orig_rect;

        drag_init_rect.x     = mevent->x - AB_drag_threshold;
        drag_init_rect.y     = mevent->y - AB_drag_threshold;
        drag_init_rect.width = drag_init_rect.height = 2 * AB_drag_threshold;

	x_offset = widget_rect.x - orig_rect.x + mevent->x;
	y_offset = widget_rect.y - orig_rect.y + mevent->y;

        first_move  = False;

        rect_zero_out(&last_rect);
    }
    /* Don't begin rendering move outline until pointer is out of
     * the drag_init bounding box
     */
    else if (!rect_includespoint(&drag_init_rect, mevent->x, mevent->y))
    {    
        Window win;

        /* event coords are relative to widget-must translate to parent */
        XTranslateCoordinates(dpy, XtWindow(xy_widget), XtWindow(parent),
            mevent->x, mevent->y, &trans_x, &trans_y, &win);

        move_rect.x = trans_x - x_offset;
        move_rect.y = trans_y - y_offset;

        /* Ensure move outline is within the parent's rect */
        if (move_rect.x < 0)
            move_rect.x = 0;
        else if ((move_rect.x + (short)move_rect.width + 1) >= (short)parent_rect.width)
            move_rect.x = parent_rect.width - (move_rect.width + 1);

        if (move_rect.y < 0)
            move_rect.y = 0;
        else if ((move_rect.y + (short)move_rect.height + 1) >= (short)parent_rect.height)
            move_rect.y = parent_rect.height - (move_rect.height + 1);

        /* If cursor has moved since last event, erase previous outline
         * and render new one (using XOR function)
         */
        if (!rect_equal(&move_rect, &last_rect))
        {
            if (!rect_isnull(&last_rect))
                x_box_r(parent, &last_rect);
            x_box_r(parent, &move_rect);
            last_rect = move_rect;
        }
    }
    return OK;
}

/*
 * Move the object to the location of the last drag rect
 */
int
abobj_move(
    ABObj     obj, 
    XEvent    *event
)
{
    short	x_delta = 0;
    short	y_delta = 0;

    if (!first_move)
    {
        first_move = True;

	/* erase last box */
        x_box_r(parent, &move_rect);

	x_delta = move_rect.x - orig_rect.x;
	y_delta = move_rect.y - orig_rect.y;

	/*
	 * Before the move is performed, record undo information
	 */
	(void)abobj_set_undo(sel.list, sel.count, undo_move, AB_UNDO_MOVE);

	abobj_nudge_selected(sel.list, sel.count, x_delta, y_delta, True);

	util_free(sel.list);
        return OK;
    }
    return ERROR;

}
void
abobj_nudge_selected(
    ABObj	*sel_list,
    int		sel_count,
    short	x_delta,
    short	y_delta,
    BOOL	reselect
)
{
    ABObj	moveobj;
    ABObj	xyobj;
    XRectangle  new_rect;
    int		i;

    for (i=0; i < sel_count; i++)
    {
        if (obj_is_layers(obj_get_parent(sel_list[i])))
            moveobj = obj_get_parent(sel_list[i]);
        else
            moveobj = sel_list[i];
 
        xyobj = objxm_comp_get_subobj(moveobj, AB_CFG_POSITION_OBJ);
         
        x_get_widget_rect(objxm_get_widget(xyobj), &new_rect);
        new_rect.x += x_delta;
        new_rect.y += y_delta;
 
        /* Move object */
        abobj_set_xy(moveobj, new_rect.x, new_rect.y);

/*
        if (xyobj->attachments)
            abobj_calculate_new_layout(xyobj, new_rect.x, new_rect.y,
                        new_rect.width, new_rect.height);
*/
 
        abobj_tree_instantiate_changes(moveobj);

	/* 
	 * UGLY WORKAROUND for Motif bug which prevents a 1 pixel move
	 * to the Left from working if only 1 object is being moved...
	 */
	if (sel_count == 1 && x_delta == -1) 
            abobj_force_dang_form_resize(moveobj);	

	if (reselect)
	    abobj_select(sel_list[i]);
   }

}


void	
abobj_move_selected(
    ABObj       *sel_list, 
    int          sel_count,
    XRectangle  *start, 
    XRectangle  *stop
)
{
	ABObj		obj;
	XRectangle	obj_rect;
	int		i;

	/*
	 * Loop through all selected objects with the same owner.  Only
	 * move selected objects, groups without anchors, and anchored groups
	 * if the anchor is also selected.
	 */
	 /* andy, revisit to add in all the group and anchor stuff */
	for (i = 0; i < sel_count; i++) 
	{
	    obj = sel_list[i];
	    x_get_widget_rect(objxm_get_widget(obj), &obj_rect);
	    rect_passtochild(start->x, start->y, &obj_rect);
	    rect_passtoparent(stop->x, stop->y, &obj_rect);
	    /*
	    undo_record_move(obj);
	    */
	    abobj_set_xy(obj, (int)obj_rect.x, (int)obj_rect.y);
	    abobj_instantiate_changes(obj); 
	}
}

/*
 * Undo function for move
 * NOTE: may need to change when moving multiple objects is
 * implemented.
 */
static void
undo_move(
    ABUndoRec	undo_rec
)
{
    int		i, 
    		x, 
		y;
    ABObj	obj;

    if (!undo_rec)
	return;

    /*
     * Set undo to undo this move (undo of undo)
     */
    (void)abobj_set_undo(undo_rec->list, undo_rec->count, 
		undo_move, AB_UNDO_MOVE);

    /*
     * For each object that was moved previously
     */
    for (i = 0; i < undo_rec->count; ++i)
    {
	/*
	 * If undo record is not the right type, something is WRONG !!
	 */
	if (undo_rec->info_list[i].type != AB_UNDO_MOVE)
	    continue;

	/*
	 * Get previous x,y and object taht was moved
	 */
        x = undo_rec->info_list[i].info.move.x;
        y = undo_rec->info_list[i].info.move.y;
        obj = undo_rec->list[i];

        /* Move object */
        abobj_set_xy(obj, x, y); 

	if (xy_obj->attachments)
	    abobj_calculate_new_layout(xy_obj, x, y,
		move_rect.width, move_rect.height);

        abobj_instantiate_changes(obj); 
    }
}

BOOL
abobj_is_movable(
    ABObj       obj
)
{
    BOOL        	movable = True;
    ABObj		pobj;
    AB_CONTAINER_TYPE	cont_type;

    /* If object is a child of a Group and the group has a
     * defined layout type, then it is not movable
     */
    pobj = obj_get_root(obj_get_parent(obj));
    if (obj_is_group(pobj) &&
        obj_get_group_type(pobj) != AB_GROUP_IGNORE)
        movable = False;

    switch(obj_get_type(obj))
    {
	case AB_TYPE_TEXT_PANE:
	case AB_TYPE_TERM_PANE:
	case AB_TYPE_DRAWING_AREA:
        case AB_TYPE_CONTAINER:
        {
	    /* If object is child of PanedWindow, it cannot be moved */
	    if (obj_is_paned_win(pobj))
		movable = False;
	    else if (obj_is_container(obj))
	    {
            	cont_type = obj_get_container_type(obj);
            	if (cont_type == AB_CONT_MENU_BAR ||
                    cont_type == AB_CONT_TOOL_BAR ||
                    cont_type == AB_CONT_FOOTER ||
                    cont_type == AB_CONT_BUTTON_PANEL)
                    movable = False;
	    }
            break;
        }
        default:
            break;
    }
    return movable;
}
