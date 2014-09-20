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
 *      $XConsortium: x_util.c /main/3 1995/11/06 17:57:42 rswiston $
 *
 * @(#)x_util.c	1.21 21 Nov 1994      cde_app_builder/src/ab
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
 * x_util.c - X11 & Xt support functions
 *	
 *
 ***********************************************************************
 */

#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <ab_private/util.h>
#include <ab_private/x_util.h>
#include <ab_private/ab.h>
#include "bitmaps/copy_cursor.xbm"
#include "bitmaps/move_cursor.xbm"


/*************************************************************************
**                                                                      **
**       Private Function Declarations                                  **
**                                                                      **
**************************************************************************/
static void 	box_gc_init(
            	    Widget w, 
            	    Display *display, 
            	    Drawable drawable
            	);
static void    	fullscreen_gc_init(
            	    Widget w, 
            	    Display *display, 
            	    Drawable drawable
            	);
static Bool	expose_count_predicate(
		    Display *display, 
		    XEvent *event, 
		    XPointer arg
		);

/*************************************************************************
**                                                                      **
**       Private Data                                                   **
**                                                                      **
**************************************************************************/
static GC      box_gc;
static GC      fullscreen_gc;
static GC      grid_gc;


/*************************************************************************
**                                                                      **
**       Public Data                                                    **
**                                                                      **
**************************************************************************/
Pixmap           AB_cp_cursor_pixmap, AB_mv_cursor_pixmap;
unsigned short   AB_cp_cursor_height, AB_cp_cursor_width;
unsigned short   AB_mv_cursor_height, AB_mv_cursor_width;


/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/

/*
 * Load all bitmaps to be used for cursors
 */
int
x_load_cursor_bitmaps(
    Widget widget
)
{

    if (!XtIsRealized(widget))
    {
        if (util_get_verbosity() > 0)
            fprintf(stderr,"x_load_cursor_bitmaps: widget must be realized\n");
        return ERROR;
    }
 
    AB_cp_cursor_height = copy_cursor_bm_height;
    AB_cp_cursor_width  = copy_cursor_bm_width;
    AB_cp_cursor_pixmap = XCreatePixmapFromBitmapData(XtDisplay(widget),
                                XtWindow(widget),
                                (char*)copy_cursor_bm_bits,
                                AB_cp_cursor_width,
                                AB_cp_cursor_height,
                                BlackPixelOfScreen(XtScreen(widget)),
                                WhitePixelOfScreen(XtScreen(widget)),
                                1);

    AB_mv_cursor_height = move_cursor_bm_height; 
    AB_mv_cursor_width  = move_cursor_bm_width;
    AB_mv_cursor_pixmap = XCreatePixmapFromBitmapData(XtDisplay(widget), 
                                XtWindow(widget), 
                                (char*)move_cursor_bm_bits,
                                AB_mv_cursor_width,
                                AB_mv_cursor_height,
                                BlackPixelOfScreen(XtScreen(widget)),
                                WhitePixelOfScreen(XtScreen(widget)),
                                1); 

    return OK;


}

/*
 * return a cursor which is the image of the passed-in pixmap with
 * a stencil around it
 */
Cursor
x_create_stencil_cursor(
    Widget widget, 
    Pixmap pixmap, 
    unsigned short width, 
    unsigned short height,
    unsigned int xhot, 
    unsigned int yhot
)
{
    Display        	*dpy;
    Window        	win;
    Cursor        	cursor;
    Pixmap        	cursor_pixmap, mask_pixmap;
    unsigned short    	c_width, c_height;
    unsigned int	c_xhot, c_yhot;
    unsigned int	max_width, max_height;
    static GC       	c_gc = NULL;
    static XColor    	fg_color, bg_color;
    int        		i, j;

    if (!XtIsRealized(widget))
    {
        if (util_get_verbosity() > 0)
            fprintf(stderr,"x_create_stencil_cursor: widget must be realized\n");
        return 0;
    }

    dpy     = XtDisplay(widget);
    win     = XtWindow(widget);

    c_width  = width + 2;
    c_height = height+ 2;
    c_xhot   = xhot;
    c_yhot   = yhot;

    XQueryBestCursor(dpy, win, c_width, c_height, &max_width, &max_height);

    if ( max_width < c_width)
        c_width  = max_width - 2;
    if ( max_height < c_height)
        c_height = max_height - 2;

    if ((max_width - 2) < c_xhot)
        c_xhot = max_width - 2;
    if ((max_height - 2) < c_yhot)
        c_yhot = max_height - 2;

    cursor_pixmap = XCreatePixmap(dpy, win, c_width, c_height, 1);
    mask_pixmap   = XCreatePixmap(dpy, win, c_width, c_height, 1);

    if (!c_gc)
    {
    	XGCValues       values;
	XrmValue        src, dst;
        Pixel          fg_pixel, bg_pixel;

	/* For the mask, we set foreground to 1 directly */
	values.foreground = 1;
	values.background = 0;
        c_gc = XCreateGC(dpy, cursor_pixmap, GCForeground|GCBackground, &values);

        fg_pixel = BlackPixelOfScreen(XtScreen(widget));
        bg_pixel = WhitePixelOfScreen(XtScreen(widget));

        src.size = sizeof(Pixel);
        src.addr = (XPointer)(&fg_pixel);
        dst.size = sizeof(XColor);
        dst.addr = (XPointer)&fg_color;
        XtConvertAndStore(widget, XtRPixel, &src, XtRColor, &dst);

        src.addr = (XPointer)&bg_pixel;
        dst.addr = (XPointer)&bg_color;
        XtConvertAndStore(widget, XtRPixel, &src, XtRColor, &dst);
    }

    XSetFunction(dpy, c_gc, GXclear);
    XFillRectangle(dpy, mask_pixmap,   c_gc, 0, 0, c_width, c_height);
    XFillRectangle(dpy, cursor_pixmap, c_gc, 0, 0, c_width, c_height);

    XSetFunction(dpy, c_gc, GXcopy);
    XCopyArea(dpy, pixmap, cursor_pixmap, c_gc, 0, 0, c_width, c_height, 1, 1);

    XSetFunction(dpy, c_gc, GXor);
    /* Build a cursor mask_pixmap that is a stencil around the image */
    for (i = -1; i <= 1; i++)
    {
        for (j = -1; j <= 1; j++)
        {
            x_graphics_op(dpy, mask_pixmap, c_gc, i, j,
                                        c_width, c_height,
                                        cursor_pixmap);
        }
    }
 
    cursor = XCreatePixmapCursor(dpy, cursor_pixmap, mask_pixmap,
                        &fg_color, &bg_color, c_xhot, c_yhot);

    XFreePixmap(dpy, mask_pixmap);
    XFreePixmap(dpy, cursor_pixmap);

    return(cursor);

}

void
x_graphics_op(
    Display *dpy, 
    Drawable dest, 
    GC gc, 
    int x, 
    int y,
    int width, 
    int height, 
    Pixmap src 
)
{
    XGCValues       orig, changes;
    unsigned long   changes_mask =  GCTileStipXOrigin |
                    GCTileStipYOrigin |
                    GCFillStyle;

    /* Save original GC state */
    XGetGCValues(dpy, gc, changes_mask, &orig);

    changes.ts_x_origin = x;
    changes.ts_y_origin = y;
    changes.stipple = src;
    changes.fill_style = FillOpaqueStippled;
 
    XChangeGC(dpy, gc, changes_mask | GCStipple, &changes);
    XFillRectangle(dpy, dest, gc, x, y, width, height);
 
    /* Restore original GC state (all but stipple) */
    XChangeGC(dpy, gc, changes_mask, &orig);
}


void
x_get_widget_rect(
    Widget widget, 
    XRectangle *rect
)
{

    XtVaGetValues(widget,
            XtNwidth,       (XtArgVal)&(rect->width),
            XtNheight,      (XtArgVal)&(rect->height),
            XtNx,           (XtArgVal)&(rect->x),
            XtNy,           (XtArgVal)&(rect->y),
            NULL);
}

/*
 * Figure out the total area occupied by two rects.
 */
void
x_get_rect_bounding(XRectangle *r1, XRectangle *r2)
{
    XRectangle     r;

    if (rect_isnull(r1))
	r = *r2;
    else if (rect_isnull(r2))
	r = *r1;
    else {
	r.x = min(r1->x, r2->x);
	r.y = min(r1->y, r2->y);
	r.width = max(r1->x + (short)r1->width, r2->x + (short)r2->width)
	    - r.x;
	r.height = max(r1->y + (short)r1->height, r2->y + (short)r2->height)
	    - r.y;
    }
    *r1 = r;
}

void
x_adjust_rect_margin(
    XRectangle *r, 
    int m
)            
{
    (r->x)-=m;
    (r->y)-=m;         
    (r->width)+=(m+m);
    (r->height)+=(m+m);
}

/*
 *  x_box
 */
void
x_box(
    Widget widget, 
    Drawable drawable, 
    int x0, 
    int y0, 
    int x1, 
    int y1
)
{
    Display     *display;
    int    tmp;

    /* Get display and drawable */
    display = (Display *)XtDisplay(widget);

    /* Initialize gc before use */
    box_gc_init(widget, display, drawable);

    /*
     * Transpose the coordinates if either the
     * width or height of the rectangle is negative.
     */
    if (x1 - x0 < 0)
    {
    	tmp = x0;
    	x0 = x1;
    	x1 = tmp;
    }
    if (y1 - y0 < 0)
    {
    	tmp = y0;
    	y0 = y1;
    	y1 = tmp;
    }

    XDrawRectangle(display, drawable, box_gc, x0, y0, x1-x0, y1-y0);
}

/*
 *  x_box_r
 */
void
x_box_r(
    Widget widget, 
    XRectangle *r
)
{
    x_box(widget, XtWindow(widget), r->x, r->y, rect_right(r), rect_bottom(r));
}

/*
 * x_dashed_box_r
 */
void
x_dashed_box_r(
    Widget widget, 
    Drawable drawable, 
    XRectangle *r
)
{
    Display     *display;

    /* Get display and drawable */
    display = (Display *)XtDisplay(widget);

    /* Initialize gc before use */
    box_gc_init(widget, display, drawable);

    /* Modify gc to used dashed lines */
    XSetLineAttributes(display, box_gc,
        0, LineOnOffDash, CapButt, JoinMiter);

    XDrawRectangle(display, drawable, box_gc,
        r->x, r->y, r->width -1, r->height -1);

    /* Restore back to solid lines */
    XSetLineAttributes(display, box_gc,
        0, LineSolid, CapButt, JoinMiter);
}

void
x_fullscreen_box(
    Widget    w,
    Drawable rootwin,
    int    x0,
    int    y0,
    int    x1,
    int    y1
)
{
    Display         *display;
    int        tmp;

    display = XtDisplay(w);
    fullscreen_gc_init(w, display, rootwin);

    /*
     * Transpose the coordinates if either the
     * width or height of the rectangle is negative.
     */
    if (x1 - x0 < 0)
    {
        tmp = x0;
        x0 = x1;
        x1 = tmp;
    }
    if (y1 - y0 < 0)
    {
        tmp = y0;
        y0 = y1;
        y1 = tmp;
    }

    XDrawRectangle(display, rootwin, fullscreen_gc,
        x0, y0, x1 - x0, y1 - y0);
}

void
x_fullscreen_preview_box(
    Widget    w,
    Drawable rootwin,
    int    x0,
    int    y0,
    int    x1,
    int    y1
)
{
    Display         *display;
    int        tmp;

    display = XtDisplay(w);

    /*
     * Transpose the coordinates if either the
     * width or height of the rectangle is negative.
     */
    if (x1 - x0 < 0)
    {
        tmp = x0;
        x0 = x1;
        x1 = tmp;
    }
    if (y1 - y0 < 0)
    {
        tmp = y0;
        y0 = y1;
        y1 = tmp;
    }

    XDrawRectangle(display, rootwin, fullscreen_gc,
        x0 - 1, y0 - 1, x1 - x0 + 2, y1 - y0 + 2);
}


/*
 * Return black or white depending on the intensity of the background
 * color.  Intensity is calculated as I = 0.3*R + 0.59*G + 0.12*B.
 */
unsigned int 
x_contrast_color(
    Widget widget
)
{
    XrmValue    src, dst;
    float       intensity;
    unsigned int    bg_pixel;
    XColor    bg_color;

    XtVaGetValues(widget, XtNbackground, &bg_pixel, NULL);

    src.size = sizeof(Pixel);
    src.addr = (XPointer)&bg_pixel;
    dst.size = sizeof(XColor);
    dst.addr = (XPointer)&bg_color;
    XtConvertAndStore(widget, XtRPixel, &src, XtRColor, &dst);

    intensity = ((float) bg_color.red * 0.3 + (float) bg_color.green * 0.59 +
        (float) bg_color.blue * 0.12) / 255.0;
/*
fprintf(stderr,"x_contrast_color: %s : red(%d) green(%d) blue(%d) = intensity(%f)\n",
XtName(widget),bg_color.red, bg_color.green, bg_color.blue, intensity);
*/

    if (intensity >= 90)
        return BlackPixelOfScreen(XtScreen(widget));
    else
    	return WhitePixelOfScreen(XtScreen(widget));

}

Boolean
x_widget_translate_xy(
    Widget src, 
    XtPointer dst, 
    int src_x, 
    int src_y,
    int *p_dst_x, 
    int *p_dst_y
)
{
    Window src_win = XtWindow(src);
    Window dst_win = dst? XtWindow((Widget)dst) : RootWindowOfScreen(XtScreen(src));
    Window child;

    return(XTranslateCoordinates(XtDisplay(src), src_win, dst_win,
            src_x, src_y, p_dst_x, p_dst_y, &child));
}

Boolean
x_rootxy_inside_widget(
    Widget w, 
    int rootx, 
    int rooty
)
{
    Window        root_win = RootWindowOfScreen(XtScreen(w));
    XRectangle     w_rect;
    int         w_x, w_y;
    Window        win_below;

    if (XtIsRealized(w))
    {
        x_get_widget_rect(w, &w_rect); 
  
        XTranslateCoordinates(XtDisplay(w), XtWindow(w),
        root_win, 0, 0, &w_x, &w_y, &win_below); 

        w_rect.x = (short)w_x;
        w_rect.y = (short)w_y;
        if (rect_includespoint(&w_rect, rootx, rooty))
       	    return TRUE;
    }
    return FALSE;
}

/*
 * box_gc_init
 */
static void
box_gc_init(
    Widget widget, 
    Display *display, 
    Drawable drawable
)
{
    XGCValues       gc_val;
    unsigned long    cc_pixel = x_contrast_color(widget);
    unsigned long   bg_pixel;

    XtVaGetValues(widget, XtNbackground, &bg_pixel, NULL);

    /* If gc doesn't exist yet, create it */
    if (!box_gc) 
    {
        gc_val.foreground = bg_pixel ^ cc_pixel; 
        gc_val.function = GXxor;
        box_gc = XCreateGC(display, drawable,
                (GCFunction | GCForeground), &gc_val);
    } 
    else 
    {
        XSetForeground(display, box_gc, bg_pixel ^ cc_pixel);
    }
}
/*
 * fullscreen_gc_init
 */
static void
fullscreen_gc_init(
    Widget widget, 
    Display *display, 
    Drawable drawable
)
{
    XGCValues       gc_val;
    unsigned long   cc_pixel = x_contrast_color(widget);
    unsigned long   bg_pixel;

    XtVaGetValues(widget, XtNbackground, &bg_pixel, NULL);

    /* If gc doesn't exist yet, create it */
    if (!fullscreen_gc) 
    {
        gc_val.foreground = bg_pixel ^ cc_pixel; 
        gc_val.function = GXxor;
        gc_val.subwindow_mode = IncludeInferiors;
        fullscreen_gc = XCreateGC(display, drawable,
            (GCFunction|GCForeground|GCSubwindowMode), &gc_val);
    } 
    else 
    {
        XSetForeground(display, fullscreen_gc, bg_pixel ^ cc_pixel);
    }
}

/*
 * Return the X Window which is directly under the x,y position
 * on the RootWindow
 */
Window
x_xwin_at_rootxy(
    Widget    widget,
    int     x,
    int     y,
    int     *dstx,
    int     *dsty
)
{
    Display *dpy;
    Window  child;
    Window  rootwin, srcwin, dstwin;

    dpy     = XtDisplay(widget);
    rootwin = RootWindowOfScreen(XtScreen(widget));

    if (XTranslateCoordinates(dpy, rootwin, rootwin, x, y,
                  dstx, dsty, &child) == 0)
        return ((Window) 0);

    /*
     * child is the XID of a child window of rootwin at 
     * root cordinates (x,y)
     * If child is NULL, we dropped on the root window
     */
    if (!child)
        return ((Window) rootwin);
 
    srcwin = rootwin;
    x      = *dstx;
    y      = *dsty;
    dstwin = child;
 
    for (;;)
    {
        if (XTranslateCoordinates(dpy, srcwin, dstwin, x, y,
                  dstx, dsty, &child) == 0)
            return ((Window) 0);

        if (!child)
            break;
        else
        {
            srcwin  = dstwin;
            dstwin  = child;
            x       = *dstx;
            y       = *dsty;
        }
    }

    return (dstwin);

}

void
x_conn_fullscreen_init(
    Widget	w,
    Drawable	root_win
)
{
    Display	*display;

    display = XtDisplay(w);
    fullscreen_gc_init(w, display, root_win);

    /* Modify gc to used thick lines */
    XSetLineAttributes(display, fullscreen_gc,
        2, LineSolid, CapButt, JoinMiter);
}

/*
 * Draw a line on the root window using fullscreen_gc
 */
void
x_conn_fullscreen_chord(
    Widget    w,
    Drawable rootwin,
    int    x0,
    int    y0,
    int    x1,
    int    y1
)
{
    Display    *display;
    int        tmp;

    display = XtDisplay(w);
    XDrawLine(display, rootwin, fullscreen_gc, x0, y0, x1, y1);
}

void
x_conn_fullscreen_cleanup(
    Widget	w
)
{
    /* Restore back to solid lines */
    XSetLineAttributes(XtDisplay(w), fullscreen_gc,
	    0, LineSolid, CapButt, JoinMiter);
}

Cursor
x_create_cursor(
    Widget		widget,
    Window		root_win,		 
    unsigned char	bits[],
    unsigned short	width,
    unsigned short	height,
    unsigned int	xhot,
    unsigned int	yhot
)
{
    Pixmap	curs_pix;
    Cursor	cursor;
    XColor	fg_color;
    XColor	bg_color;

    curs_pix = XCreateBitmapFromData(XtDisplay(widget), root_win, (char*)bits,
				     width, height);
    if (curs_pix == None)
	return(None);

    {
	XrmValue       src, dst;
        Pixel          fg_pixel, bg_pixel;

        fg_pixel = BlackPixelOfScreen(XtScreen(widget));
        bg_pixel = WhitePixelOfScreen(XtScreen(widget));

        src.size = sizeof(Pixel);
        dst.size = sizeof(XColor);

        src.addr = (XPointer)&fg_pixel;
        dst.addr = (XPointer)&fg_color;
        XtConvertAndStore(widget, XtRPixel, &src, XtRColor, &dst);

        src.addr = (XPointer)&bg_pixel;
        dst.addr = (XPointer)&bg_color;
        XtConvertAndStore(widget, XtRPixel, &src, XtRColor, &dst);
    }

    cursor = XCreatePixmapCursor(XtDisplay(widget), curs_pix, curs_pix,
				 &fg_color, &bg_color, xhot, yhot);
    XFreePixmap(XtDisplay(widget), curs_pix);

    return(cursor);

}


/*
 * Counts the number of expose events or events that could cause
 * exposures currently waiting in the queue.
 *
 * note: THIS DOES NOT FLUSH THE OUTPUT QUEUE, SO OTHER EVENTS MAY
 * BE PENDING ON THE CLIENT SIDE (USE XFlush() TO AVOID THIS)
 */
int		
x_get_num_pending_expose_events(Display *display)
{
    int 	num_expose_events = 0;
    XEvent	event;

    XCheckIfEvent(display, &event,
	expose_count_predicate, (XPointer)&num_expose_events);
    return num_expose_events;
}


/*
 * If we return True from this predicate function, the event will
 * be removed from the queue.  We don't want this to happen, so we
 * will *always* return False, but increment the counter (the arg ptr)
 * for each expose event.
 */
static Bool		
expose_count_predicate(Display *display, XEvent *event, XPointer arg)
{
#define expose_count (*expose_count_out)
    int		*expose_count_out = (int *)arg;

    switch (event->type)
    {
	case CreateNotify:
	case DestroyNotify:
	case Expose:
	case GraphicsExpose:
	case MapNotify:
	case MapRequest:
	case NoExpose:
	case UnmapNotify:
	case VisibilityNotify:
	    ++expose_count;
	break;
    }

    return False;
#undef expose_count
}

