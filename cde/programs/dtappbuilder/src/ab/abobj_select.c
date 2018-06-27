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
 *      $XConsortium: abobj_select.c /main/3 1995/11/06 17:17:19 rswiston $
 *
 * @(#)abobj_select.c	1.37 15 Feb 1994      cde_app_builder/src/ab
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
 * ab_select.c - Implements "select" behavior of a UI object
 *        
 *
 ***********************************************************************
 */
#include <stdint.h>
#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/cursorfont.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>
#include <Xm/ScrolledW.h>
#include <ab_private/trav.h>
#include <ab_private/ui_util.h>
#include <ab_private/x_util.h>
#include <ab_private/objxm.h>
#include <ab_private/proj.h>
#include "abobjP.h"

const int AB_selected_rect_size = 7;

/*************************************************************************
**                                                                      **
**       Private Function Declarations                                  **
**                                                                      **
**************************************************************************/

static void    	select_feedback(
               	    ABObj 	selObj,
		    BOOL  	on
        	);
static void	select_footer(
		    ABObj 	obj,
		    BOOL  	on
		);
static void     turnon_select_feedback( 
		    ABObj 	selObj,
		    BOOL	resizable
		); 

static void     turnoff_select_feedback(
		    ABObj 	selObj,
		    BOOL	resizable
		);

static void    	draw_select_feedback(
		    ABObj 	selObj,
		    BOOL	resizable
        	);
/*
 * Event Handlers
 */
static void    	monitor_cursor(
		    Widget     	w,
		    XtPointer 	clientdata,
		    XEvent     	*event,
		    Boolean 	*cont_dispatch
        	);

static void    	select_feedback_redraw(
		    Widget 	w,
		    XtPointer 	clientdata,
		    XEvent     	*event,
		    Boolean 	*cont_dispatch
        	);

/*************************************************************************
**		    		    		    		          **
**       Private Data		    		    		       **
**		    		    		    		          **
**************************************************************************/
static GC 	  AB_grab_handle_gc;
static XRectangle rb_rect;
static Widget     rb_widget; 
static BOOL	  first_time = TRUE;


/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/

/*
 * cause the object to appear "selected"
 */
void
abobj_select(
    ABObj obj
)
{
    AB_CONTAINER_TYPE	cont_type;
    ABObj		rootObj = obj_get_root(obj);
    ABObj    		selObj;

    /* Make sure the window is on screen */
    if (obj_is_window(rootObj))
        ui_win_front(objxm_get_widget(rootObj));

    proj_set_cur_module(obj_get_module(obj));
    
    if (obj_is_selected(rootObj))
        return;

    obj_set_is_selected(rootObj, TRUE);

    if (obj_is_module(rootObj))
	return;

    if (obj_is_item(obj))
    {
	rootObj = obj_get_root(obj_get_parent(obj));
	obj_set_is_selected(rootObj, TRUE);
    }

    selObj = objxm_comp_get_subobj(rootObj, AB_CFG_SELECT_OBJ);

    if (selObj == NULL)
    {
        if (util_get_verbosity() > 0)
            fprintf(stderr, "abobj_select: %s :no selection object\n",
		obj_get_name(rootObj));
        return;
    }

    if (obj_is_window(rootObj))
	select_footer(selObj, True);
    else
	turnon_select_feedback(selObj, abobj_is_directly_resizable(rootObj));

}


/*
 * remove "select" feedback from object
 */
void
abobj_deselect(
    ABObj obj
)
{
    AB_CONTAINER_TYPE	cont_type;
    ABObj		rootObj = obj_get_root(obj);
    ABObj    		selObj;

    if (!obj_is_selected(rootObj))
        return;

    obj_set_is_selected(rootObj, FALSE);

    if (obj_is_module(obj))
	return;

    if (obj_is_item(obj)) 
    {
        rootObj = obj_get_root(obj_get_parent(obj)); 
	obj_set_is_selected(rootObj, FALSE);
    } 

    selObj = objxm_comp_get_subobj(rootObj, AB_CFG_SELECT_OBJ);

    if (selObj == NULL)
    {
        if (util_get_verbosity() > 0)
            fprintf(stderr, "abobj_deselect: %s :no selection object\n",
		obj_get_name(rootObj));
        return;
    }

    if (obj_is_window(rootObj))
        select_footer(selObj, False);
    else
        turnoff_select_feedback(selObj, abobj_is_directly_resizable(rootObj));

}

void
abobj_deselect_all(
    ABObj	root	
)
{
    ABObj           obj = root;
    AB_TRAVERSAL    trav;
 
    if (obj == NULL)
        return;
 
    for (trav_open(&trav, obj, AB_TRAV_UI);
        (obj = trav_next(&trav)) != NULL; )
    {
        if (obj_is_selected(obj))
            abobj_deselect(obj);
    }
    trav_close(&trav);

    /*
     * The browser displays the module as well,
     * so we must deselect it when abobj_deselect_all() is
     * called
     */
    if (obj_is_project(root))
    {
        for (trav_open(&trav, root, AB_TRAV_MODULES);
            (obj = trav_next(&trav)) != NULL; )
        {
            if (obj_is_selected(obj))
                abobj_deselect(obj);
        }
        trav_close(&trav);
    }
    else
    {
	ABObj	module = obj_get_module(root);

        if (obj_is_selected(module))
            abobj_deselect(module);
    }

}


/*
 * abobj_get_selected- Return an array containing ptrs to
 * all selected ABObjs in a given
 */
int
abobj_get_selected(
    ABObj root,
    BOOL  include_root,
    BOOL  include_items,
    ABSelectedRec *sel
)
{
    ABObj     	    obj = root;
    AB_TRAVERSAL    trav;

    sel->count = 0;
    sel->list  = NULL;

    if (obj == NULL)
        return -1;

    /* First count up all selected objs */
    for (trav_open(&trav, obj, AB_TRAV_SALIENT_UI);
        (obj = trav_next(&trav)) != NULL; )
    {
    	if (obj_is_selected(obj) && 
	    !(obj_is_item(obj) && !include_items) &&
	    !(obj == root && !include_root))
            (sel->count)++;
    }
    trav_close(&trav);

    /* Now alloc & fill array with selected objs */
    obj = root;
    if (sel->count > 0)
    {
        int i = 0;
	sel->list = (ABObj*)XtMalloc(sel->count * sizeof(ABObj));

        for (trav_open(&trav, obj, AB_TRAV_SALIENT_UI);
	    (obj = trav_next(&trav)) != NULL; )
	{
	    if (obj_is_selected(obj) && 
		!(obj_is_item(obj) && !include_items) &&
                !(obj == root && !include_root))
		sel->list[i++] = obj;
	}
    }
    return 0;
}


/*
 * Sort list of selected objects 
 */
void
abobj_sort_sel_list(
    ABObj    *sel_list,
    int       sel_count,
    int       sort
)
{
    int		i, j;
    int		x1, y1, x2, y2;
    ABObj	tmp_obj;

    /*
     * Sort the list of objects based on x and y values
     */
    for (i = 0; i < sel_count; i++) 
    {
	for (j = i; j < sel_count; j++) 
	{
	    x1 = obj_get_x(sel_list[i]);
	    y1 = obj_get_y(sel_list[i]);
	    x2 = obj_get_x(sel_list[j]);
	    y2 = obj_get_y(sel_list[j]);

	    if (sort == XSORT) 
	    {
		if (x2 < x1 || (x2 == x1 && y2 < y1)) 
		{
		    tmp_obj = sel_list[i];
		    sel_list[i] = sel_list[j];
		    sel_list[j] = tmp_obj;
		}
	    } else 
	    {
		if (y2 < y1 || (y2 == y1 && x2 < x1)) 
		{
		    tmp_obj = sel_list[i];
		    sel_list[i] = sel_list[j];
		    sel_list[j] = tmp_obj;
		}
	    }
	}
    }
}
/*
 * render select feedback for objects which do not have grab handles
 */
static void
select_feedback(
    ABObj selObj,
    BOOL  on
)
{
    Widget  selWidget;
    Pixel   contrast;

    selWidget = (Widget)selObj->ui_handle;

    if (selWidget == NULL)
    {
        if (util_get_verbosity() > 0)
            fprintf(stderr, "select_feedback: %s :no selection widget\n",
		obj_get_name(selObj));
        return;
    }

    if (on)
    {
	contrast = x_contrast_color(selWidget);
        XtVaSetValues(selWidget, 
			XmNborderWidth,   (XtArgVal)2, 
			XmNborderColor,	  contrast,
			NULL);
    }
    else
        XtVaSetValues(selWidget, XmNborderWidth,   (XtArgVal)0, NULL);
 
}

static void
select_footer(
    ABObj	selObj,
    BOOL	on
)
{
    Widget	selWidget;
    Pixel	contrast;
    XmString	xmstr;

    if (!(selWidget = (Widget)selObj->ui_handle))
	return;

    if (on)
    {
	contrast = x_contrast_color(selWidget);

	xmstr = XmStringCreateLocalized("Window SELECTED ");
	XtVaSetValues(selWidget,
		XmNlabelString,		xmstr,
		XmNforeground,		contrast,
		NULL);
	XmStringFree(xmstr);
    }
    else /* off */
    {
        xmstr = XmStringCreateLocalized("         "); 
        XtVaSetValues(selWidget, 
                XmNlabelString,         xmstr, 
                NULL);
        XmStringFree(xmstr); 
    }

}
    

/*
 * set event handlers to draw the grab handles on
 * the object's selection widget
 */
static void
turnon_select_feedback(
    ABObj selObj,
    BOOL  resizable
)
{
    Widget  selWidget;

    selWidget = (Widget)selObj->ui_handle;

    if (selWidget == NULL)
        return;

    draw_select_feedback(selObj, resizable);

    XtAddEventHandler(selWidget, StructureNotifyMask | ExposureMask, FALSE,
        	select_feedback_redraw, (XtPointer)(uintptr_t) resizable);

    XtAddEventHandler(selWidget, PointerMotionMask, FALSE,
                monitor_cursor, (XtPointer)selObj);

}

/*
 * remove the expose event handlers and clear the
 * grab handles off the widget
 */
static void
turnoff_select_feedback(
    ABObj selObj,
    BOOL  resizable
)
{
    XRectangle w_rect;
    Widget     selWidget;
 
    selWidget = (Widget)selObj->ui_handle;
 
    if (selWidget == NULL)
        return;

    x_get_widget_rect(selWidget, &w_rect);

    XtRemoveEventHandler(selWidget, StructureNotifyMask | ExposureMask, FALSE,
                select_feedback_redraw, (XtPointer)(uintptr_t) resizable);

    XtRemoveEventHandler(selWidget, PointerMotionMask, FALSE,
                monitor_cursor, (XtPointer)selObj);
	
    /* Make sure cursor is reset to normal */
    monitor_cursor(selWidget, selObj, NULL, NULL); 

    ui_refresh_widget_tree(selWidget);

}

/*
 * Draw the grabhandles on the selection-widget for the object
 */
static void
draw_select_feedback(
    ABObj selObj,
    BOOL  resizable
)
{
    ABObj	  obj;
    Widget     	  selWidget;
    Dimension     width = 0;
    Dimension     height = 0;
    int  x0,y0,x1,y1;
    int		  border_w;
    Display       *display;
    Drawable      drawable;
    XGCValues     gc_val;
    unsigned long bg, color_contrast;
    int		  grabbox_size;

    obj = obj_get_root(selObj);

    selWidget = (Widget)selObj->ui_handle;    

    if ( (obj_is_drawing_area(obj) || obj_is_text_pane(obj)) &&
	(obj_has_hscrollbar(obj) || obj_has_vscrollbar(obj)) )
    {
	ABObj	swobj = objxm_comp_get_subobj(obj, AB_CFG_SIZE_OBJ);
	Widget	cwidget;

	XtVaGetValues((Widget)swobj->ui_handle,
		XmNclipWindow,	&cwidget,
		NULL);

	if (cwidget)
	{
	    XtVaGetValues(cwidget,
                XtNwidth,     &width,
                XtNheight,    &height,
		NULL);

	    XtVaGetValues(selWidget,
		XtNbackground, &bg,
		NULL);
	}
    }

    if (height == 0 || width == 0)
    	XtVaGetValues(selWidget, 
            	XtNwidth,     &width, 
            	XtNheight,    &height, 
            	XtNbackground,&bg,
            	NULL);

/*
    if (obj_is_control(obj))
	grabbox_size = (AB_selected_rect_size / 2) + 1;
    else
*/
	grabbox_size = AB_selected_rect_size;

    if (obj_is_control(obj))
	border_w = 1;
    else
	border_w = 2;

    x0 = y0 = border_w - 1;
    x1 = width - grabbox_size;
    y1 = height - grabbox_size;

    color_contrast = x_contrast_color(selWidget);
    
    display  = (Display *) XtDisplay(selWidget);
    drawable = (Drawable)  XtWindow(selWidget);

    if (!AB_grab_handle_gc)
    {
        gc_val.foreground = color_contrast;
	gc_val.line_width = border_w;
        gc_val.function      = GXcopy;
                gc_val.subwindow_mode = IncludeInferiors;
        AB_grab_handle_gc = XCreateGC(display, drawable,
                 (GCFunction | GCForeground | GCLineWidth | GCSubwindowMode), 
		 &gc_val);
    }
    else
    {
        XSetForeground(display, AB_grab_handle_gc, color_contrast);
	XSetLineAttributes(display, AB_grab_handle_gc, border_w,
		LineSolid, CapButt, JoinMiter);
    }

    /* Draw Select Rectangle */
    XDrawRectangle(display, drawable, AB_grab_handle_gc, 
		   x0, y0,
		   width-border_w, height-border_w);

    /* If Obj is Resizable, Draw Resize handles */
    if (resizable)
    {
	/* North-West */
    	XFillRectangle(display, drawable, AB_grab_handle_gc, x0, y0,
               grabbox_size, grabbox_size);
	/* South-West */
    	XFillRectangle(display, drawable, AB_grab_handle_gc, x0, y1,
               grabbox_size, grabbox_size);
	/* North-East */
    	XFillRectangle(display, drawable, AB_grab_handle_gc, x1, y0,
               grabbox_size, grabbox_size);
	/* South-East */
    	XFillRectangle(display, drawable, AB_grab_handle_gc, x1, y1,
               grabbox_size, grabbox_size);
	/* North */
    	XFillRectangle(display, drawable, AB_grab_handle_gc, (x0+x1)/2, y0,
               grabbox_size, grabbox_size);
	/* South */
    	XFillRectangle(display, drawable, AB_grab_handle_gc, (x0+x1)/2, y1,
               grabbox_size, grabbox_size);
	/* West */
    	XFillRectangle(display, drawable, AB_grab_handle_gc, x0, (y0+y1)/2,
               grabbox_size, grabbox_size);
	/* East */
    	XFillRectangle(display, drawable, AB_grab_handle_gc, x1, (y0+y1)/2,
               grabbox_size, grabbox_size);
    }
    XFlush(display);

}

/*
 * EventHandler: detect when cursor moves over grab handles
 */
static void
monitor_cursor(
    Widget    selWidget,
    XtPointer clientdata,
    XEvent    *event,
    Boolean   *cont_dispatch
)
{
    ABObj	     selObj = (ABObj)clientdata;
    XMotionEvent     *mevent;
    Cursor           resize_cursor;
    RESIZE_DIR       dir;
    static Boolean   cursor_changed = FALSE;
    Window           win;
    Display          *dpy;


    if (event != NULL)
    {
    	if (event->type != MotionNotify)
            return;
    	else
            mevent = (XMotionEvent*)event;

    	dir = abobjP_find_resize_direction(selObj, selWidget, event);
    }
    else
	dir = NONE;

    dpy = XtDisplay(selWidget);
    win = XtWindow(selWidget);

    if (dir == NONE)
    {
        if (cursor_changed)
        {
            XUndefineCursor(dpy, win);
            cursor_changed = FALSE;
        }
    }
    else
    {
	if ((resize_cursor = abobjP_get_resize_cursor(selWidget, dir)) != 0)
	{
    	    XDefineCursor(dpy, win, resize_cursor);
    	    cursor_changed = TRUE;
	}
    }

}

/*
 * EventHandler: render grab handles on widget for Expose events
 */
static void
select_feedback_redraw(
    Widget       selWidget, 
    XtPointer clientdata, 
    XEvent    *event, 
    Boolean   *cont_dispatch
)
{
    ABObj	selObj = NULL;
    BOOL	resizable = (BOOL)((uintptr_t) clientdata);
    Boolean     redraw = FALSE;

    selObj = objxm_get_obj_from_widget(selWidget);

    if (event->type == ConfigureNotify)
    {
        XRectangle    w_rect;
        x_get_widget_rect(selWidget, &w_rect);

        /* ClearArea is roundtrip, should use XOR..*/
	ui_refresh_widget_tree(selWidget);
        redraw = TRUE;
    }
    else if (event->type == Expose)
        redraw = TRUE;

    if (redraw)
        draw_select_feedback(selObj, resizable);
}

