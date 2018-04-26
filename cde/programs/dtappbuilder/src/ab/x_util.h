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
 *      $XConsortium: x_util.h /main/3 1995/11/06 17:57:58 rswiston $
 *
 * @(#)x_util.h	1.16 21 Apr 1994      cde_app_builder/src/ab
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
 * ab_x_util.h 
 */
#ifndef _X_UTIL_H_
#define _X_UTIL_H_

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#ifndef min
#define min(a,b) ((a) < (b)? (a):(b))
#endif

#ifndef max
#define max(a,b) ((a) > (b)? (a):(b))
#endif

extern int  	x_load_cursor_bitmaps(
           	    Widget 	    w
            	);
extern void 	x_get_widget_rect(
                    Widget      w, 
                    XRectangle *r
            	);
extern void 	x_get_rect_bounding(
		    XRectangle *r1, 
		    XRectangle *r2
		);
extern void 	x_graphics_op(
            	    Display     *dpy,
            	    Drawable    dest,
            	    GC          gc,
            	    int         x,
            	    int         y,
            	    int         width,
            	    int      height,
            	    Pixmap   src
            	);
extern void 	x_get_widget_rect(
            	    Widget     widget, 
            	    XRectangle *rect
            	);
extern void 	x_adjust_rect_margin(
            	    XRectangle *r, 
            	    int 	   m
            	);                     
extern void 	x_box(
            	    Widget     widget, 
            	    Drawable   win, 
            	    int 	   x0, 
            	    int 	   y0, 
            	    int 	   x1, 
            	    int 	   y1
            	);
extern void 	x_box_r(
            	    Widget 	 widget, 
            	    XRectangle  *r
            	);
extern void 	x_dashed_box_r(
            	    Widget 	widget, 
            	    Drawable    drawable, 
            	    XRectangle  *r
             	);
extern void 	x_fullscreen_box(
            	    Widget 	    widget,
            	    Drawable        rootwin,
                    int 	    x0,
                    int 	    y0,
                    int 	    x1,
                    int 	    y1
            	);
extern void 	x_fullscreen_preview_box(
            	    Widget 	    widget,
            	    Drawable        rootwin,
                    int 	    x0,
                    int 	    y0,
                    int 	    x1,
                    int 	    y1
            	);
extern void 	x_conn_fullscreen_init(
		    Widget	    w,
		    Drawable    root_win
	        );
extern void	x_conn_fullscreen_chord(
    		    Widget    w,
    		    Drawable rootwin,
    		    int    x0,
    		    int    y0,
    		    int    x1,
    		    int    y1
		);
extern void     x_fullscreen_chord(
            	    Widget 	    widget,
            	    Drawable    rootwin,
                    int 	    x0,
                    int 	    y0,
                    int 	    x1,
                    int 	    y1
            	);
extern void 	x_conn_fullscreen_cleanup(
		    Widget	    w
	        );
extern unsigned int    
		x_contrast_color(
            	    Widget 	    w
            	);
extern Boolean	x_widget_translate_xy(
            	    Widget 	    src, 
            	    XtPointer   dst, 
            	    int 	    src_x, 
            	    int 	    src_y,
            	    int 	    *p_dst_x, 
            	    int 	    *p_dst_y
            	);
extern Boolean  x_rootxy_inside_widget(
            	    Widget 	    w, 
            	    int 	    rootx, 
            	    int 	    rooty
              	);
extern Window 	x_xwin_at_rootxy(
            	    Widget      widget,
                    int         x,
                    int         y,
                    int         *p_wx,
                    int         *p_wy
            	);
extern Cursor 	x_create_cursor(
		    Widget		widget,
		    Window		root_win,		 
		    unsigned char	bits[],
		    unsigned short	width,
		    unsigned short	height,
		    unsigned int	xhot,
		    unsigned int	yhot
	        );

extern Cursor	x_create_stencil_cursor(
    	  	    Widget widget, 
    	  	    Pixmap pixmap, 
    	  	    unsigned short width, 
    	  	    unsigned short height,
    	  	    unsigned int xhot, 
    	  	    unsigned int yhot
	  	);

extern int	x_get_num_pending_expose_events(Display *display);


/*************************************************************************
**                                                                      **
**       Macros								**
**                                                                      **
**************************************************************************/
#define rect_right(r)   (short)((r)->x + (short)(r)->width)
#define rect_bottom(r)  (short)((r)->y + (short)(r)->height)

#define rect_includespoint(r,p_x,p_y) \
        ((p_x) >= (r)->x && (p_y) >= (r)->y && \
         (p_x)< (short)((r)->x+(short)(r)->width) && \
         (p_y)< (short)((r)->y+(short)(r)->height))

#define rect_includesrect(r1, r2) \
       	((r2)->x >= (r1)->x && (r2)->y >= (r1)->y && \
	 ((r2)->x + (short)(r2)->width)  <= ((r1)->x + (short)(r1)->width) && \
	 ((r2)->y + (short)(r2)->height) <= ((r1)->y + (short)(r1)->height))

#define rect_equal(r1,r2) \
        ((r1)->x==(r2)->x && (r1)->width==(r2)->width && \
         (r1)->y==(r2)->y && (r1)->height==(r2)->height)

#define rect_zero_out(r) \
    { (r)->x=0;(r)->y=0;(r)->width=0;(r)->height=0; }

#define rect_isnull(r) \
    ((r)->width==0 && (r)->height==0)

/*************************************************************************
**                                                                      **
**       Extern Data							**
**                                                                      **
**************************************************************************/
extern Pixmap    	AB_cp_cursor_pixmap, AB_mv_cursor_pixmap;
extern unsigned short   AB_cp_cursor_height, AB_cp_cursor_width;
extern unsigned short 	AB_mv_cursor_height, AB_mv_cursor_width;

#endif /* _X_UTIL_H_ */

