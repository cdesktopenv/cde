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
 *      $XConsortium: conn_drag.c /main/3 1995/11/06 17:25:49 rswiston $
 *
 * @(#)conn_drag.c	1.20 15 Feb 1994	cde_app_builder/src/ab
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


#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <Xm/Xm.h>
#include <Xm/PushB.h>

#include <ab_private/obj.h>
#include <ab_private/trav.h>

#include <ab_private/conn.h>
#include <ab_private/connP.h>
#include <ab_private/brws.h>
#include <ab_private/proj.h>
#include <ab_private/x_util.h>

#include <ab_private/objxm.h>

/*
 * The cursors used for drag 'n link.
 */
#include "bitmaps/left_up_link.xbm"
#include "bitmaps/left_down_link.xbm"
#include "bitmaps/right_up_link.xbm"
#include "bitmaps/right_down_link.xbm"

extern Widget     AB_toplevel;

/*
 * Connection drag directions.
 */
typedef enum {
    NORTH_EAST = 0x00,
    NORTH_WEST = 0x01,
    SOUTH_EAST = 0x10,
    SOUTH_WEST = 0x11,
    NONE       = 0x22
} DRAG_DIR;

static Cursor	curs_nw	= (Cursor)None;
static Cursor	curs_ne	= (Cursor)None;
static Cursor	curs_se	= (Cursor)None;
static Cursor	curs_sw	= (Cursor)None;

static Boolean	in_browser;

static int      start_x;
static int      start_y;

static int      cur_x;
static int      cur_y;

static ABObj	source_obj;
static ABObj	target_obj;

static Window	source_win;
static Window	target_win;


static void		process_chord_extend(
			    Widget		widget
			);
static void		setup_drag(
			    Widget		widget,
			    XButtonEvent	*button_ev
			);
static void		cleanup_drag(
			    Widget		widget,
			    XButtonEvent	*button_ev
			);
static DRAG_DIR		find_drag_dirn(
			    int		start_x,
			    int		start_y,
			    int		new_x,
			    int		new_y
			);
static ABObj		get_obj_at_point(
			    ABObj	project,
			    Boolean	in_browser,
			    Window	window,
			    int		x,
			    int		y
			);
static ABObj		ui_get_obj_at_point(
			    ABObj	project,
			    Window	window,
			    int		x,
			    int		y
			);
static Boolean		is_win_decor(
			    Widget	win_wid,
			    Window	window
			);
static Boolean		find_win_match(
			    Display	*display,
			    Window	top_win,
			    Window	target_win,
			    Window	limit_win
			);
static void		preview_object(
			    ABObj	obj,
			    Boolean	in_browser,
			    Window	win
			);
static void		ui_preview_object(
			    ABObj	obj
			);


extern void
conn_drag_chord(
    Widget	widget,
    XEvent	*event,
    String	*params,
    Cardinal	num_params
)
{
    ABObj		project = proj_get_project();
    Boolean		done;
    XButtonEvent	*button_ev;
    Display		*display = XtDisplay(widget);
    Window		window = XtWindow(widget);
    Window		root_win = RootWindow(display, DefaultScreen(display));
    Window		child_win;
    int			win_x;
    int			win_y;
    XEvent		new_event;

    if (event->type != ButtonPress)
	return;
    else
	button_ev = (XButtonEvent *)event;

    if (!(button_ev->state & ControlMask))
	return;

    XTranslateCoordinates(display, window, root_win,
		button_ev->x, button_ev->y, &start_x, &start_y, &child_win);

    /*
     * Identify source object
     */
    source_win = x_xwin_at_rootxy(widget, start_x, start_y, &win_x, &win_y);

    in_browser = aob_is_browser_win(project, source_win);
    source_obj = get_obj_at_point(project, in_browser,
					source_win, win_x, win_y);
    if (source_obj == NULL)
    {
	XBell(display, 0);
	return;
    }

    setup_drag(widget, button_ev);
    x_conn_fullscreen_init(widget, root_win);

    preview_object(source_obj, in_browser, source_win);

    cur_x = start_x; cur_y = start_y;
    target_win = source_win; target_obj = source_obj;
    x_conn_fullscreen_chord(widget, root_win, start_x, start_y, cur_x, cur_y);

    done = FALSE;
    while (!done)
    {
	XEvent		extra;
	Window		new_win;

	XNextEvent(display, &new_event);

	switch (new_event.type)
	{
	  case ButtonRelease:
	    done = TRUE;
	    break;

	  case MotionNotify:
	    if (new_event.xmotion.root != root_win ||
		(XEventsQueued(display, QueuedAfterReading) > 0 &&
		 (XPeekEvent(display, &extra), extra.type == MotionNotify)))
		break;
	    else
		process_chord_extend(widget);
	    break;
	}
    }

    process_chord_extend(widget);

    x_conn_fullscreen_chord(widget, root_win, start_x, start_y, cur_x, cur_y);

    /*
     * Free cursors and disable grab
     */
    cleanup_drag(widget, (XButtonEvent *)&new_event);

    if (target_obj != NULL)
    {
	preview_object(target_obj, in_browser, target_win);

	conn_set_source(source_obj);

  	if (obj_is_message(target_obj) || obj_is_menu(target_obj))
	    conn_set_target(NULL);
	else
	     conn_set_target(target_obj);
	conn_popup_dialog(AB_toplevel, (XtPointer)0, NULL);
    }
    else
    {
	XBell(display, 0);
	fprintf(stderr, "Connection chord not dropped on a valid target.\n");
    }
    x_conn_fullscreen_cleanup(widget);
}

static void
process_chord_extend(
    Widget		widget
)
{
    ABObj		project = proj_get_project();
    Display	        *display = XtDisplay(widget);
    Window	        window = XtWindow(widget);
    Window	        root_win = RootWindow(display, DefaultScreen(display));

    DRAG_DIR		new_dir;
    Window		dum_ret;
    Window		new_win;
    int			new_x;
    int			new_y;
    int			win_x;
    int			win_y;
    int			dum_x;
    int			dum_y;
    unsigned int	mask_ret;
    Cursor		cur_cursor;


    x_conn_fullscreen_chord(widget, root_win,
	    start_x, start_y, cur_x, cur_y);

    if (!XQueryPointer(display, root_win, &dum_ret, &dum_ret,
	    &new_x, &new_y, &dum_x, &dum_y, &mask_ret))
	return;

    new_dir = find_drag_dirn(start_x, start_y, new_x, new_y);
    if (new_dir != find_drag_dirn(start_x, start_y, cur_x, cur_y))
    {
	switch(new_dir)
	{
	  case NORTH_WEST:
	    cur_cursor = curs_nw;
	    break;
	  case NORTH_EAST:
	    cur_cursor = curs_ne;
	    break;
	  case SOUTH_EAST:
	    cur_cursor = curs_se;
	    break;
	  case SOUTH_WEST:
	    cur_cursor = curs_sw;
	    break;
	  default:
	    /* catch-all to avoid compiler warnings */
	    break;
	}

	if (XGrabPointer(display, root_win, True,
		    ButtonReleaseMask|PointerMotionMask,
		    GrabModeAsync, GrabModeAsync, root_win,
		    cur_cursor, CurrentTime) !=
	    GrabSuccess)
	{
	    fprintf(stderr, "Pointer grab failed.\n");
	    XUngrabKeyboard(display, CurrentTime);
	    XUngrabServer(display);
	    return;
	}
    }
    x_conn_fullscreen_chord(widget, root_win,
	    start_x, start_y, new_x, new_y);
    
    new_win = x_xwin_at_rootxy(widget, new_x, new_y,
				&win_x, &win_y);

    if (in_browser)
    {
	if (new_win != target_win)
	{
	    preview_object(target_obj, in_browser, target_win);

	    target_win = new_win;
	    in_browser = aob_is_browser_win(project, target_win);
	    target_obj = get_obj_at_point(project, in_browser,
				target_win, win_x, win_y);

	    preview_object(target_obj, in_browser, target_win);
	}
	else
	{
	    ABObj	new_obj;

	    new_obj = get_obj_at_point(project, in_browser,
				target_win, win_x, win_y);
	    if (new_obj != target_obj)
	    {
		preview_object(target_obj, in_browser, target_win);

		target_obj = new_obj;

		preview_object(target_obj, in_browser, target_win);
	    }
	}
    }
    else if (new_win != target_win)
    {
	ABObj	new_obj;
	Boolean	save_browser = in_browser;

	in_browser = aob_is_browser_win(project, new_win);
	new_obj = get_obj_at_point(project, in_browser,
			    new_win, win_x, win_y);
	if (new_obj != target_obj)
	{
	    preview_object(target_obj, save_browser, target_win);

	    target_obj = new_obj;

	    preview_object(target_obj, in_browser, target_win);
	}
	target_win = new_win;
    }
    cur_x = new_x;
    cur_y = new_y;
}

static void
setup_drag(
    Widget		widget,
    XButtonEvent	*button_ev
)
{
    Display	*display	= XtDisplay(widget);
    Window	window		= XtWindow(widget);
    Window	root_win	= RootWindow(display, DefaultScreen(display));
    Cursor	cur_cursor;

    curs_nw = x_create_cursor(widget, root_win, left_up_link_bits,
			       left_up_link_width, left_up_link_height,
			       left_up_link_x_hot, left_up_link_y_hot);
    curs_ne = x_create_cursor(widget, root_win, right_up_link_bits,
			       right_up_link_width, right_up_link_height,
			       right_up_link_x_hot, right_up_link_y_hot);
    curs_se = x_create_cursor(widget, root_win, right_down_link_bits,
			       right_down_link_width, right_down_link_height,
			       right_down_link_x_hot, right_down_link_y_hot);
    curs_sw = x_create_cursor(widget, root_win, left_down_link_bits,
			       left_down_link_width, left_down_link_height,
			       left_down_link_x_hot, left_down_link_y_hot);
    cur_cursor = curs_ne;


    XSynchronize(display, True);
    if (XGrabPointer(display, root_win, True,
		ButtonReleaseMask|PointerMotionMask, GrabModeAsync,
		GrabModeAsync, root_win, cur_cursor, button_ev->time) !=
	GrabSuccess)
    {
	fprintf(stderr, "Pointer grab failed.\n");
	return;
    }
    if (XGrabKeyboard(display, root_win, False,
		    GrabModeAsync, GrabModeAsync,
		    button_ev->time) !=
	GrabSuccess)
    {
	fprintf(stderr, "Keyboard grab failed (non-fatal).\n");
    }
    XGrabServer(display);
}

static void
cleanup_drag(
    Widget		widget,
    XButtonEvent	*button_ev
)
{
    Display	*display	= XtDisplay(widget);

    XUngrabPointer(display, button_ev->time);
    XUngrabKeyboard(display, button_ev->time);
    XUngrabServer(display);
    XSynchronize(display, False);

    XFreeCursor(display, curs_nw);
    XFreeCursor(display, curs_ne);
    XFreeCursor(display, curs_se);
    XFreeCursor(display, curs_sw);
}

static DRAG_DIR
find_drag_dirn(
    int		start_x,
    int		start_y,
    int		new_x,
    int		new_y
)
{
    int	drag_val	= 0x00;

    if (start_x > new_x)	/* West */
	drag_val |= 0x01;
    if (start_y < new_y)	/* South */
	drag_val |= 0x10;
    
    /*
    switch ((DRAG_DIR)drag_val)
    {
      case NORTH_EAST:
	fprintf(stderr, "North East");
	break;
      case NORTH_WEST:
	fprintf(stderr, "North West");
	break;
      case SOUTH_EAST:
	fprintf(stderr, "South East");
	break;
      case SOUTH_WEST:
	fprintf(stderr, "South West");
	break;
    }
    fprintf(stderr, "\n");
    */
    return((DRAG_DIR)drag_val);
}

/*
 * Identify the target object.
 */
static ABObj
get_obj_at_point(
    ABObj	project,
    Boolean	in_browser,
    Window	window,
    int		x,
    int		y
)
{
    ABObj	ret_val;    

    if (in_browser)
    {
	ret_val = aob_get_object_from_xy(project, window, x, y);

	if (ret_val != NULL)
	{
	    ret_val = obj_get_root(ret_val);
	    if (!connP_obj_enabled(obj_get_type(ret_val),
				    obj_get_subtype(ret_val)))
		ret_val = NULL;
	}
    }
    else
    {
	ret_val = ui_get_obj_at_point(project, window, x, y);
	if (ret_val != NULL)
	{
	    ret_val = obj_get_root(ret_val);
	}
    }

    return(ret_val);
}

/*
 * Look for the target object among the realized widgets
 */
static ABObj
ui_get_obj_at_point(
    ABObj	project,
    Window	window,
    int		x,
    int		y
)
{
    ABObj		ui_obj = NULL;
    AB_TRAVERSAL	trav;

    for (trav_open(&trav, project, AB_TRAV_UI);
         (ui_obj = trav_next(&trav)) != NULL; )
    {
	Widget	win_wid = (Widget)ui_obj->ui_handle;

	if ((win_wid != (Widget)NULL) && (XtWindow(win_wid) == window))
	    break;
    }
    trav_close(&trav);

    if (ui_obj == NULL)
    {
	for (trav_open(&trav, project, AB_TRAV_WINDOWS);
	     (ui_obj = trav_next(&trav)) != NULL; )
	{
	    Widget	win_wid = objxm_get_widget(ui_obj);
	    
	    if ((win_wid != NULL) && is_win_decor(win_wid, window))
	    {
		return ui_obj;
	    }
	}
	trav_close(&trav);
    }

    return ui_obj;

#ifdef BOGUS /* this is old Guide stuff */
    /*
     * Translate the supplied x, y to the window's local coordinates.

    win_translate_xy(window, XtWindow(ui_obj->ui_handle), x, y, &newx, &newy);


     * Check to see if the pointer is in a List or Choice item
    for (obj = G_tail_fn(G_itf); (obj && obj != win); obj = obj->previous)
    {
	if (obj->owner_obj == win &&
		!G_isgroup(obj) &&
		!G_isstack_obj(obj) &&
		newx >= obj->x &&
		newx <= (obj->x + obj->width) &&
		newy >= obj->y &&
		newy <= (obj->y + obj->height))
	{
	     * If it's a setting, also identify which particular setting item
	     * it's pointing at.
	    if (obj->G_type == G_SETTING &&
		    obj->setting_type != G_SETTING_STACK)
	    {
		*nth_item = identify_setting_item(obj->handle,
						  newx, newy);
	    }

	    return obj;
	}
    }
     */

    /*
     * Check to see if it's in a group.
    for (obj = win; obj; obj = obj->next)
    {
	if (obj->owner_obj == win &&
		G_isgroup(obj) &&
		newx >= obj->x &&
		newx <= (obj->x + obj->width) &&
		newy >= obj->y &&
		newy <= (obj->y + obj->height))
	    return obj;
    }

     */
#endif /* BOGUS */
}


static Boolean
is_win_decor(
    Widget	win_wid,
    Window	window
)
{
    Boolean	ret_val;
    Window	query_win	= XtWindow(win_wid);
    Display	*display	= XtDisplay(win_wid);

    Window	qroot;
    Window	parent;
    Window	*children;
    unsigned int	num_children;

    /*
     * If there is a Window Manager running, the top level
     * window has been reparented into a window hierarchy
     * which holds the decor windows as well.
     * In this loop, we find the top of this hierarchy that
     * is the actual child of the root window
     *
     */
    for (;
	 XQueryTree(display, query_win, &qroot,
		    &parent, &children, &num_children) &&
	 parent != qroot;
	 query_win = parent)
    {
	XFree((XPointer)children);
    }
    XFree((XPointer)children);

    if (parent != qroot)
	return(FALSE);

    return(find_win_match(display, query_win, window, XtWindow(win_wid)));
}

static Boolean
find_win_match(
    Display	*display,
    Window	top_win,
    Window	target_win,
    Window	limit_win
)
{
    Window	qroot;
    Window	parent;
    Window	*children;
    unsigned int	num_children;
    Boolean	ret_val;
    
    if (top_win == target_win)
	return(TRUE);
    if (top_win == limit_win)
	return(FALSE);

    ret_val = FALSE;
    if (XQueryTree(display, top_win, &qroot,
		    &parent, &children, &num_children))
    {
	unsigned int	i;
	
	for (i = 0; i < num_children; i++)
	{
	    if ((ret_val = find_win_match(display, children[i], target_win,
					  limit_win)) == TRUE)
		break;
	}
	XFree((XPointer)children);
    }
    return(ret_val);
}

static void
preview_object(
    ABObj	obj,
    Boolean	in_browser,
    Window	win
)
{
    if (obj == NULL)
	return;

    if (in_browser)
	aob_preview(obj, win);
    else
	ui_preview_object(obj);
}

static void
ui_preview_object(
    ABObj	obj
)
{
    Window	rootwin;
    Display	*dpy;
    XRectangle	p_rect;
    ABObj	gobj;
    Widget	gwidget;
    Widget	parent;
    int		trans_x;
    int		trans_y;
    Window	win;
    
    
    if (obj_is_item(obj))
	obj = obj_get_parent(obj);

    obj  = obj_get_root(obj);
    gobj = objxm_comp_get_subobj(obj, AB_CFG_SIZE_OBJ);
 
    gwidget = (Widget)gobj->ui_handle;
 
    if (gwidget == NULL)
    {
	fprintf(stderr,"ui_preview_object: no geometry widget\n");
	return;
    }
    parent  = XtParent(gwidget);
    dpy     = XtDisplay(gwidget);
    rootwin = RootWindowOfScreen(XtScreen(gwidget));

    x_get_widget_rect(gwidget, &p_rect);
    XTranslateCoordinates(dpy, XtWindow(parent),
            rootwin,  p_rect.x, p_rect.y, &trans_x, &trans_y,
            &win);
    p_rect.x = trans_x;
    p_rect.y = trans_y;
    x_fullscreen_preview_box(gwidget, rootwin, 
		      p_rect.x, p_rect.y,
		      rect_right(&p_rect), 
		      rect_bottom(&p_rect));
}
