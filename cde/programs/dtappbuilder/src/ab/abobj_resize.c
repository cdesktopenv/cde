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
 *      $XConsortium: abobj_resize.c /main/4 1996/10/29 16:43:13 mustafa $
 *
 * @(#)abobj_resize.c	1.28 15 Feb 1994      cde_app_builder/src/ab
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
 *
 * abobj_resize.c - Implements resizing of UI objects
 *         
 *
 ***********************************************************************
 */
#include <stdio.h>
#include <X11/cursorfont.h>
#include <Xm/ScrolledW.h>
#include <ab_private/objxm.h>
#include <ab_private/prop.h>
#include <ab_private/x_util.h>
#include <ab_private/ui_util.h>
#include <ab_private/abobj_edit.h>
#include "abobjP.h"

/*************************************************************************
**                                                                      **
**       Private Function Declarations                                  **
**                                                                      **
**************************************************************************/
static void    	make_rect(
		    XRectangle  *new_r, 
		    XRectangle  *r, 
		    int        	x, 
		    int        	y, 
		    RESIZE_DIR  dir
        	);

static void    	undo_resize(
		    ABUndoRec	undo_rec
		);
void	resize_in_pixels(
ABObj	obj,
int		width,
int		height,
int		border_w
    );

static void	subtract_attached_label(
		    ABObj	obj,
		    XRectangle	*r_rect
		);

/*************************************************************************
**                                                                      **
**       Private Data                                                   **
**                                                                      **
**************************************************************************/
static Boolean       first_move = TRUE;
static XRectangle    resize_rect;
static ABObj	     xy_obj = NULL;
static Widget	     xy_widget = NULL;
static int	     border_w = 0;

/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/

/*
 *    Return resize-direction corresponding to cursor x,y position    
 */
RESIZE_DIR
abobjP_find_resize_direction (
    ABObj  obj,
    Widget widget, 
    XEvent *event
)
{
    ABObj		rootObj;
    XRectangle    	w_rect;
    XRectangle    	temp;
    int        		half_handle;
    int        		half_width;
    int        		half_height;
    int        		x, y;
    int			orig_x, orig_y;
    int		  	grabbox_size;

    if (event->type == MotionNotify)
    {
        orig_x = x = ((XMotionEvent*)event)->x;
        orig_y = y = ((XMotionEvent*)event)->y;
    }
    else if (event->type == ButtonPress)
    {
        orig_x = x = ((XButtonEvent*)event)->x;
        orig_y = y = ((XButtonEvent*)event)->y;
    }
    else
	return NONE;

    if (obj_is_item(obj))
	obj = obj_get_parent(obj);

    rootObj = obj_get_root(obj);

    rect_zero_out(&w_rect);

    if ((obj_is_drawing_area(obj) || obj_is_text_pane(obj)) &&
        (obj_has_hscrollbar(obj) || obj_has_vscrollbar(obj)) )
    {
        ABObj   swobj = objxm_comp_get_subobj(obj, AB_CFG_SIZE_OBJ);
        Widget  cwidget;

        XtVaGetValues((Widget)swobj->ui_handle,
                XmNclipWindow,  &cwidget,
                NULL);

	if (cwidget)
	    x_get_widget_rect(cwidget, &w_rect);
    }
    else if (rootObj != obj)
    {
	Window	win;

        XTranslateCoordinates(XtDisplay(widget), XtWindow(widget),
            XtWindow(objxm_get_widget(rootObj)), orig_x , orig_y, &x, &y,
            &win);

	widget = objxm_get_widget(rootObj);
    }

    if (w_rect.width == 0 || w_rect.height == 0)
        x_get_widget_rect(widget, &w_rect);

/*
    if (obj_is_control(obj))
	grabbox_size = (AB_selected_rect_size / 2) + 1;
    else
*/
    grabbox_size = AB_selected_rect_size;

    /* Readjust coordinates to remove offset within the parent window.
    */
    w_rect.x = 0;
    w_rect.y = 0;

    /* Exit if we are not near the border.
     */
    temp = w_rect;
    x_adjust_rect_margin(&temp, 0 - grabbox_size);
    if (obj_is_control_panel(rootObj) && rect_includespoint (&temp, x, y))
        return (NONE);

    if (!abobj_is_directly_resizable(rootObj))
	return (MOVE);

    half_handle = grabbox_size/ 2 + 1;
    half_width  = w_rect.width / 2;
    half_height = w_rect.height / 2;

    temp.width = temp.height = grabbox_size + 2;

    /********************************
    *  SOUTH_EAST
    *  Most common ?
    *********************************/
    temp.x = w_rect.width - grabbox_size;
    temp.y = w_rect.height - grabbox_size;

    if (rect_includespoint (&temp, x, y))
        return (SOUTH_EAST);

    /********************************
    *  SOUTH_WEST
    *********************************/
    temp.x = 0;
    temp.y = w_rect.height - grabbox_size;

    if (rect_includespoint (&temp, x, y))
        return (SOUTH_WEST);

    /********************************
    *  NORTH
    *********************************/
    temp.x = half_width - half_handle;
    temp.y = 0;

    if (rect_includespoint (&temp, x, y))
        return (NORTH);


    /********************************
    *  SOUTH
    *********************************/
    temp.x = half_width - half_handle;
    temp.y = w_rect.height - grabbox_size;

    if (rect_includespoint (&temp, x, y))
        return (SOUTH);

    /********************************
    *  EAST
    *********************************/
    temp.x = w_rect.width - grabbox_size;
    temp.y = half_height - half_handle;

    if (rect_includespoint (&temp, x, y))
        return (EAST);

    /********************************
    *  WEST
    *********************************/
    temp.x = 0;
    temp.y = half_height - half_handle;

    if (rect_includespoint (&temp, x, y))
        return (WEST);

    /********************************
    *  NORTH_EAST
    *********************************/
    temp.x = w_rect.width - grabbox_size;
    temp.y = 0;

    if (rect_includespoint (&temp, x, y))
        return (NORTH_EAST);

    /********************************
    *  NORTH_WEST
    *********************************/
    temp.x = 0;
    temp.y = 0;

    if (rect_includespoint (&temp, x, y))
        return (NORTH_WEST);

    /********************************
    *  Must be a move.
    *********************************/
    return (MOVE);
}


/*
 *    Return correct font cursor for given Resize-direction    
 */
Cursor
abobjP_get_resize_cursor(
    Widget     w, 
    RESIZE_DIR dir
)
{
    static Cursor    resize_cursor[10] = 
                	{ 0, 0, 0, 0, 0, 
                          0, 0, 0, 0, 0 };
    unsigned int     shape;

    if (!resize_cursor[dir])
    {
	if (dir == MOVE)
	{
            resize_cursor[MOVE] = x_create_stencil_cursor(w,
                AB_mv_cursor_pixmap, AB_mv_cursor_width,
                AB_mv_cursor_height, 0, 0);
	}
	else
	{
            switch(dir)
            {
	        case SOUTH_EAST:
	    	    shape = XC_bottom_right_corner;
	    	    break;
	        case SOUTH_WEST:
	    	    shape = XC_bottom_left_corner; 
	    	    break;
	        case NORTH_WEST:
	    	    shape = XC_top_left_corner;
	    	    break;
	        case NORTH_EAST:
	    	    shape = XC_top_right_corner;
	    	    break;
	        case SOUTH:
	    	    shape = XC_bottom_side;
	    	    break;
	        case NORTH:
	    	    shape = XC_top_side;
	    	    break;
	        case EAST:
		    shape = XC_right_side;
		    break;
	    	case WEST:
		    shape = XC_left_side;
		    break;
	    	case NONE:
                default:
		    return 0;
	    }
            resize_cursor[dir] = XCreateFontCursor(XtDisplay(w), shape);
	}
    }
    return(resize_cursor[dir]);

}

/*
 *    Draw rubberbanding outline for resizing action    
 */
int
abobjP_resize_object_outline(
    ABObj        obj,
    XEvent       *event,
    RESIZE_DIR   dir
)
{
    ABObj	       s_obj;
    static Widget      parent;
    static Window      rootwin;
    static Display     *dpy;
    static XRectangle  orig_r, r;
    static int         last_x, last_y;
    int                x,y;
    char               buf[80];

    if (event->type == MotionNotify)
    {
    	x = ((XMotionEvent*)event)->x_root;
    	y = ((XMotionEvent*)event)->y_root;
    }
    else if (event->type == ButtonPress)
    {
        x = ((XButtonEvent*)event)->x_root;
        y = ((XButtonEvent*)event)->y_root;
    } 
    else
	return -1;

    if (first_move)
    {
        Window     win;
        int    orig_x, orig_y;
        int    trans_x, trans_y;

	if (obj_is_item(obj))
	    obj = obj_get_parent(obj);

	obj  = obj_get_root(obj);
	xy_obj = objxm_comp_get_subobj(obj, AB_CFG_POSITION_OBJ);
	xy_widget = (Widget)xy_obj->ui_handle;
 
        if (xy_widget == NULL)
        {
            if (util_get_verbosity() > 0)
                fprintf(stderr,"abobjP_resize_object_outline: %s :no SIZE widget\n",
			util_strsafe(obj_get_name(obj)));
            return ERROR;
        }
 
        parent  = XtParent(xy_widget);
        dpy     = XtDisplay(xy_widget);
        rootwin = RootWindowOfScreen(XtScreen(xy_widget));

        x_get_widget_rect(xy_widget, &orig_r);
	if (obj_has_border_frame(obj)) /* We have a border-frame to deal with */
	{
	    XRectangle pane_r;

            s_obj = objxm_comp_get_subobj(obj, AB_CFG_SIZE_OBJ);

	    /* Determine width of border */
	    x_get_widget_rect(objxm_get_widget(s_obj), &pane_r);
	    border_w = ((int)(orig_r.width - pane_r.width))/2;
	}
	else
	    border_w = 0;

	orig_r.width--;
	orig_r.height--;

        r = orig_r;

        orig_x = (int)orig_r.x;
        orig_y = (int)orig_r.y;

        XTranslateCoordinates(dpy, XtWindow(parent),
            rootwin, orig_x , orig_y, &trans_x, &trans_y,
            &win);

        r.x = (short)trans_x;
        r.y = (short)trans_y;

        first_move = FALSE;
    }
    else     /* erase previous outline */    
    {
        make_rect(&resize_rect, &r, last_x, last_y, dir);
        x_fullscreen_box(xy_widget, rootwin, 
                resize_rect.x, resize_rect.y,
                rect_right(&resize_rect), 
                rect_bottom(&resize_rect));

    }

    make_rect(&resize_rect, &r, x, y, dir);
    x_fullscreen_box(xy_widget, rootwin, 
                resize_rect.x, resize_rect.y,
                                rect_right(&resize_rect), 
                rect_bottom(&resize_rect));

    sprintf(buf, "%3.3dx%3.3d", resize_rect.width,
                                resize_rect.height);

    /* REMIND: aim, update status region on ab palette win */

    last_x = x;
    last_y = y;

    return OK;

}

/*
 *    Resize object according to info in last resize rubberbanding    
 */
int
abobj_resize(
    ABObj     obj, 
    XEvent     *event
)
{
    Widget       parent;
    Display      *dpy;
    XRectangle   p_rect;
    XRectangle	 adj_rect;
    Window       rootwin;
    int          orig_x, orig_y;
    int          trans_x, trans_y;
    ABObj	 moveobj;
    ABObj	 pobj;

    if (!first_move) /* resize occurred */
    {
        Window     win;

	if (obj_is_item(obj))
	    obj = obj_get_parent(obj);

	obj     = obj_get_root(obj);
        parent  = XtParent(xy_widget);
        dpy     = XtDisplay(xy_widget);

        orig_x = (int)resize_rect.x;
        orig_y = (int)resize_rect.y;

        first_move = TRUE;

        x_get_widget_rect(parent, &p_rect);

        rootwin = RootWindowOfScreen(XtScreen(xy_widget));

        /* erase last rect outline 
         */
        x_fullscreen_box(xy_widget, rootwin, 
        	resize_rect.x, resize_rect.y,
                rect_right(&resize_rect), 
                rect_bottom(&resize_rect));

        /* translate from root coordinates to parent's 
         */
        XTranslateCoordinates(dpy, rootwin, XtWindow(parent),
                        orig_x , orig_y, &trans_x, &trans_y, &win);

	resize_rect.width++;
	resize_rect.height++;

        /* Ensure new geometry fits within parent
         */
        if (trans_x < 0)
        {
            resize_rect.x = 0;
            resize_rect.width += trans_x;

	    if (obj_is_pane(obj) || obj_is_separator(obj)) /* If a pane, attach to parent's edge */
		obj_set_attachment(xy_obj, AB_CP_WEST, AB_ATTACH_OBJ, obj_get_parent(xy_obj), 0);
        }
        else    
	{
            resize_rect.x = trans_x;

            /* Break edge-attachment */
            if ((obj_is_pane(obj) || obj_is_separator(obj)) &&
                 (obj_get_attach_type(xy_obj, AB_CP_WEST) == AB_ATTACH_OBJ &&
                  obj_get_attach_value(xy_obj, AB_CP_WEST) == (void*)obj_get_parent(xy_obj)))
                obj_set_attachment(xy_obj, AB_CP_WEST, AB_ATTACH_POINT, (void*)0, trans_x);
        }

        if (trans_y < 0)
        {
            resize_rect.y = 0;
            resize_rect.height += trans_y;

            if (obj_is_pane(obj) || obj_is_separator(obj)) /* If a pane, attach to parent's edge */
                obj_set_attachment(xy_obj, AB_CP_NORTH, AB_ATTACH_OBJ, obj_get_parent(xy_obj), 0);
        }
        else
	{
            resize_rect.y = trans_y;

	    /* Break edge-attachment */
            if ((obj_is_pane(obj) || obj_is_separator(obj)) &&
		 (obj_get_attach_type(xy_obj, AB_CP_NORTH) == AB_ATTACH_OBJ &&
		  obj_get_attach_value(xy_obj, AB_CP_NORTH) == (void*)obj_get_parent(xy_obj))) 
                obj_set_attachment(xy_obj, AB_CP_NORTH, AB_ATTACH_POINT, (void*)0, trans_y);
	}

        if (resize_rect.x + (short)resize_rect.width >= (short)p_rect.width)
        {
            resize_rect.width = (short)p_rect.width - resize_rect.x - 1;

	    if (obj_is_pane(obj) || obj_is_separator(obj)) /* If a pane, attach to parent's edge */ 
		obj_set_attachment(xy_obj, AB_CP_EAST, AB_ATTACH_OBJ, obj_get_parent(xy_obj), 0);
	}
	else
	{
            /* Break edge-attachment */ 
	    if ((obj_is_pane(obj) || obj_is_separator(obj)) &&
                 (obj_get_attach_type(xy_obj, AB_CP_EAST) == AB_ATTACH_OBJ &&
                  obj_get_attach_value(xy_obj, AB_CP_EAST) == (void*)obj_get_parent(xy_obj))) 
                obj_set_attachment(xy_obj, AB_CP_EAST, AB_ATTACH_NONE, (void*)0, 0); 
        } 

        if (resize_rect.y + (short)resize_rect.height > (short)p_rect.height)
	{
            resize_rect.height = (short)p_rect.height - resize_rect.y - 1;

            if (obj_is_pane(obj) || obj_is_separator(obj)) /* If a pane, attach to parent's edge */
                obj_set_attachment(xy_obj, AB_CP_SOUTH, AB_ATTACH_OBJ, obj_get_parent(xy_obj), 0); 
	}
        else 
        {
            /* Break edge-attachment */ 
            if ((obj_is_pane(obj) || obj_is_separator(obj)) && 
                 (obj_get_attach_type(xy_obj, AB_CP_SOUTH) == AB_ATTACH_OBJ && 
                  obj_get_attach_value(xy_obj, AB_CP_SOUTH) == (void*)obj_get_parent(xy_obj)))  
                obj_set_attachment(xy_obj, AB_CP_SOUTH, AB_ATTACH_NONE, (void*)0, 0);  
        } 

	(void)abobj_set_undo(&obj, 1, undo_resize, AB_UNDO_RESIZE);

	pobj = obj_get_parent(obj);
 	if (obj_is_layers(pobj))
	    moveobj = pobj;
	else
	    moveobj = obj;

	adj_rect = resize_rect;
	if (abobj_has_attached_label(obj))
	    subtract_attached_label(obj, &adj_rect);

        if (obj_is_drawing_area(obj) && obj_has_scrollbar(obj))
        {
            int new_d_w = -1;
            int new_d_h = -1;
            int d_w = obj_get_drawarea_width(obj);
            int d_h = obj_get_drawarea_height(obj);
 
            /* If the overall DrawArea dimension is now larger than the internal
             * canvas dimension, resize the canvas dimension to be at least as large
             * as the overall DrawArea.
             */
            if ((int)resize_rect.width != obj_get_width(obj) &&
                (int)resize_rect.width > d_w)
                new_d_w = (int)resize_rect.width;
 
            if ((int)resize_rect.height != obj_get_height(obj) &&
                (int)resize_rect.height > d_h)
                new_d_h = (int)resize_rect.height;
 
            if (new_d_w != -1 || new_d_h != -1)
                abobj_set_drawarea_size(obj,
                        new_d_w != -1? new_d_w : d_w, new_d_h != -1? new_d_h : d_h);
        }

        /* 
 	 * Resize the object !
         */
	if (obj_is_layers(pobj))
	    abobj_layer_set_size(pobj, resize_rect.width, resize_rect.height);

        else if ((obj_is_text(obj) || obj_is_term_pane(obj)) &&
		  obj_get_num_columns(obj) != -1)
	    /* TextPane, TermPane, TextField with Character-based Sizing */
	    abobj_set_text_size(obj, (int)adj_rect.width, (int)adj_rect.height);

	else if (obj_is_list(obj) && obj_get_num_rows(obj) != -1)
	{
	    /* List with Character-based height */
	    abobj_set_text_size(obj, (int)adj_rect.width, (int)adj_rect.height);
	    abobj_set_pixel_width(obj, (int)adj_rect.width, border_w);
	}
	else /* else size is Pixel-based...*/
	    resize_in_pixels(obj, (int)adj_rect.width, (int)adj_rect.height, border_w);

        abobj_set_xy(moveobj, resize_rect.x, resize_rect.y);

	if (xy_obj->attachments)
	    abobj_calculate_new_layout(xy_obj, resize_rect.x, resize_rect.y,
		resize_rect.width, resize_rect.height);

	/* Change obj size BEFORE changing attachments...*/
	abobj_instantiate_changes(obj);
	abobj_tree_instantiate_changes(moveobj);

	/* WORKAROUND for Motif XmForm bug (it ignores child resize request 
	 * if x,y have not also changed). So, we have to force it.
	 */
	abobj_force_dang_form_resize(xy_obj);

	if (util_get_verbosity() > 3)
	    objxm_dump_widget_geometry(xy_widget);

        return OK;
    }
    return ERROR;

}

static void
subtract_attached_label(
    ABObj	obj,
    XRectangle	*r_rect
)
{
    ABObj	lblObj;
    Widget	lbl_widget;
    Dimension   lbl_w = 0;
    Dimension   lbl_h = 0;
    Dimension	margin = 0;
    Dimension	spacing = 0;
    Dimension	width, height;

    if (!abobj_has_attached_label(obj))
	return;

    lblObj = objxm_comp_get_subobj(obj, AB_CFG_LABEL_OBJ);
    lbl_widget = objxm_get_widget(lblObj);

    if (lblObj != NULL && lbl_widget != NULL)
    {
	switch(obj_get_label_position(obj))
	{
	    case AB_CP_WEST:
		XtVaGetValues(lbl_widget, XmNwidth, &lbl_w, NULL);
		XtVaGetValues(XtParent(lbl_widget), /* RowColumn */
			XmNmarginWidth, &margin,
			XmNspacing,	&spacing,
			NULL);

		width = r_rect->width - lbl_w - (2*margin) - spacing;
		r_rect->width = width;
		break;
	    case AB_CP_NORTH:
                XtVaGetValues(lbl_widget, XmNheight, &lbl_h, NULL);
                XtVaGetValues(XtParent(lbl_widget), /* RowColumn */
                        XmNmarginHeight, &margin,
                        XmNspacing,      &spacing, 
                        NULL); 
 
                height = r_rect->height - lbl_h - (2*margin) - spacing; 
                r_rect->height = height; 
                break; 
	    default:
		break;
	}
    }
}

void
resize_in_pixels(
    ABObj	obj,
    int		width,
    int		height,
    int		border_w
)
{
    BOOL w_resizable = abobj_width_resizable(obj);
    BOOL h_resizable = abobj_height_resizable(obj);

    if (w_resizable && h_resizable)
        abobj_set_pixel_size(obj, width, height, border_w);

    else if (w_resizable)
        abobj_set_pixel_width(obj, width, border_w);

    else if (h_resizable)
        abobj_set_pixel_height(obj, height, border_w);

}

void
abobjP_track_external_resizes(
    Widget	widget,
    XtPointer   client_data,
    XEvent 	*event,
    Boolean	cont_to_dispatch
)
{
    ABObj     	obj = (ABObj)client_data;
    Widget    	parent = XtParent(widget);
    int	      	new_value;
    int	      	new_h = -1; 
    int	      	new_w = -1;
    int		old_w, old_h;	
    BOOL      	resized = False;

    if (event->type == ConfigureNotify)
    {
        XConfigureEvent *cfg_event = (XConfigureEvent *)&event->xconfigure;

	obj_get_size(obj, &old_w, &old_h);

        if (old_w != -1)
	    new_w = (int)cfg_event->width + (2*cfg_event->border_width);

        if (old_h != -1)
            new_h = (int)cfg_event->height + (2*cfg_event->border_width);

	if (new_w != old_w || new_h != old_h)
	{
	    if (obj && obj_is_group(obj) && 
		(obj_get_group_type(obj) != AB_GROUP_IGNORE))
	    {
		return;
	    }

	    abobj_set_save_needed(obj_get_module(obj), TRUE);
	    obj_resize(obj, new_w, new_h);
	}
    }
}


/*
 * calculate resize rect based on resize-direction & obj dimensions    
 */
static void
make_rect (
    XRectangle    *new_r, 
    XRectangle    *r, 
    int        	  x, 
    int        	  y, 
    RESIZE_DIR    dir
)
{
    switch (dir)
    {
        case NORTH:
            new_r->x = r->x;
            new_r->y = y;
            new_r->width = r->width;
            new_r->height = (r->y + r->height) - y;
            break;
        case SOUTH:
            new_r->x = r->x;
            new_r->y = r->y;
            new_r->width = r->width;
            new_r->height = y - r->y;
            break;
        case EAST:
            new_r->x = r->x;
            new_r->y = r->y;
            new_r->width = x - r->x;
            new_r->height = r->height;
            break;
        case WEST:
            new_r->x = x;
            new_r->y = r->y;
            new_r->width = (r->x + r->width) - x;
            new_r->height = r->height;
            break;
        case NORTH_EAST:
            new_r->x = r->x;
            new_r->y = y;
            new_r->width = x - r->x;
            new_r->height = (r->y + r->height) - y;
            break;
        case NORTH_WEST:
            new_r->x = x;
            new_r->y = y;
            new_r->width = (r->x + r->width) - x;
            new_r->height = (r->y + r->height) - y;
            break;
        case SOUTH_EAST:
            new_r->x = r->x;
            new_r->y = r->y;
            new_r->width = x - r->x;
            new_r->height = y - r->y;
            break;
        case SOUTH_WEST:
            new_r->x = x;
            new_r->y = r->y;
            new_r->width = (r->x + r->width) - x;
            new_r->height = y - r->y;
	    break;
	}
}

/*
 * Function for undoing RESIZE
 */
static void
undo_resize(
    ABUndoRec	undo_rec
)
{

    Position	x,
		y;
    int		width, 
    		height,
		i;
    ABObj       obj;

    if (!undo_rec)
	return;

    /*
     * Set undo to undo this resize (undo of undo)
     */
    (void)abobj_set_undo(undo_rec->list, undo_rec->count, 
		undo_resize, AB_UNDO_RESIZE);

    /*
     * For each object that was resized
     */
    for (i = 0; i < undo_rec->count; ++i)
    {
	BOOL	w_resizable, h_resizable;
	/*
	 * If undo record is not the right type, something is WRONG !!
	 */
	if (undo_rec->info_list[i].type != AB_UNDO_RESIZE)
	    continue;

        obj = undo_rec->list[i];
	w_resizable = abobj_width_resizable(obj);
	h_resizable = abobj_height_resizable(obj);

	/*
	 * Get current x,y position
	 */
	if (obj->ui_handle)
	{
            XtVaGetValues(objxm_get_widget(obj),
			XmNx,      &x,
			XmNy,      &y,
			NULL);
	}
	else
	{
	    x = (Position)obj_get_x(obj);
	    y = (Position)obj_get_y(obj);
	}

	/*
	 * Get previous width/height
	 */
        width = undo_rec->info_list[i].info.resize.width;
        height = undo_rec->info_list[i].info.resize.height;

        /* 
         * Resize the object !
         */
	resize_in_pixels(obj, (int)width, (int)height, border_w);
        abobj_set_xy(obj, (int)x, (int)y);

        if (xy_obj->attachments)
            abobj_calculate_new_layout(xy_obj, (int)x, (int)y,
                    width, height);

        abobj_instantiate_changes(obj);

        /* WORKAROUND for Motif XmForm bug (it ignores child resize request 
         * if x,y have not also changed). So, we have to force it.
         */
        abobj_force_dang_form_resize(xy_obj);

        if (util_get_verbosity() > 3)
            objxm_dump_widget_geometry(xy_widget);
    }
}

/*
 * Can the object be resized using direct-manipulation?
 */
BOOL
abobj_is_directly_resizable(
    ABObj       obj
)
{
    BOOL        resizable = True;

    switch(obj_get_type(obj))
    {
        case AB_TYPE_BUTTON:
        case AB_TYPE_LABEL:
        case AB_TYPE_COMBO_BOX:
	case AB_TYPE_LIST:
            if (obj_get_width(obj) == -1)
                resizable = False;
            break;
	case AB_TYPE_CHOICE:
	    resizable = False;
	    break;
        case AB_TYPE_CONTAINER:
	{
            AB_CONTAINER_TYPE cont_type;
            cont_type = obj_get_container_type(obj);
            if ((cont_type == AB_CONT_MENU_BAR ||
                 cont_type == AB_CONT_TOOL_BAR ||
                 cont_type == AB_CONT_FOOTER ||
		 cont_type == AB_CONT_BUTTON_PANEL ||
		 cont_type == AB_CONT_GROUP) ||
		obj_get_width(obj) == -1)
                resizable = False;
            break;
        }
        default:
            break;
    }
    return resizable;
}

/*
 * Can the object's height be changed (from Prop sheet)?
 */
BOOL
abobj_height_resizable(
    ABObj	obj
)
{
    BOOL 	resizable = True;

    switch(obj_get_type(obj))
    {
        case AB_TYPE_COMBO_BOX:
	case AB_TYPE_TEXT_FIELD:
	case AB_TYPE_SPIN_BOX:
        case AB_TYPE_CHOICE:
            resizable = False;
	    break;
	case AB_TYPE_BUTTON:
	case AB_TYPE_LABEL:
	case AB_TYPE_BASE_WINDOW:
	case AB_TYPE_DIALOG:
	    if (obj_get_height(obj) == -1)
		resizable = False;
	    break;
	case AB_TYPE_SCALE:
	    if (obj_get_orientation(obj) == AB_ORIENT_HORIZONTAL)
		resizable = False;
	    break;
        case AB_TYPE_CONTAINER:
        {
            AB_CONTAINER_TYPE cont_type;
            cont_type = obj_get_container_type(obj);
            if (cont_type == AB_CONT_MENU_BAR ||
		(obj_is_control_panel(obj) && obj_get_height(obj) == -1))
		resizable = False;
	    break;
        }
        default:
            break;
    }
    return resizable;
}

/* 
 * Can the object's width be changed (from Prop sheet)?
 */ 
BOOL
abobj_width_resizable(
    ABObj       obj
)
{
    BOOL        resizable = True;
 
    switch(obj_get_type(obj))
    {
        case AB_TYPE_CHOICE:
            resizable = False;
            break; 
	case AB_TYPE_SPIN_BOX:
	case AB_TYPE_COMBO_BOX:
	case AB_TYPE_BUTTON:
	case AB_TYPE_LABEL:
	case AB_TYPE_LIST:
        case AB_TYPE_BASE_WINDOW: 
        case AB_TYPE_DIALOG:
	    if (obj_get_width(obj) == -1)
		resizable = False;
	    break;
        case AB_TYPE_SCALE: 
            if (obj_get_orientation(obj) == AB_ORIENT_VERTICAL) 
                resizable = False; 
            break; 
        case AB_TYPE_CONTAINER: 
        { 
            AB_CONTAINER_TYPE cont_type; 
            cont_type = obj_get_container_type(obj); 
            if (cont_type == AB_CONT_MENU_BAR ||
                cont_type == AB_CONT_TOOL_BAR ||
                cont_type == AB_CONT_FOOTER ||
                cont_type == AB_CONT_BUTTON_PANEL ||
                cont_type == AB_CONT_GROUP ||
		(obj_is_control_panel(obj) && obj_get_width(obj) == -1))
                resizable = False;
            break; 
        }
        default:
            break;
    }
    return resizable;
}
