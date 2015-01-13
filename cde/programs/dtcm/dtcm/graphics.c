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
/*******************************************************************************
**
**  graphics.c
**
**  $XConsortium: graphics.c /main/3 1995/11/03 10:27:52 rswiston $
**
**  RESTRICTED CONFIDENTIAL INFORMATION:
**
**  The information in this document is subject to special
**  restrictions in a confidential disclosure agreement between
**  HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
**  document outside HP, IBM, Sun, USL, SCO, or Univel without
**  Sun's specific written approval.  This document and all copies
**  and derivative works thereof must be returned or destroyed at
**  Sun's request.
**
**  Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
**
*******************************************************************************/

/*									*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc. 				*
 */

#ifndef lint
static  char sccsid[] = "@(#)graphics.c 1.23 94/11/29 Copyr 1994 Sun Microsystems, Inc.";
#endif

#include <EUSCompat.h>
#include <stdio.h>
#include <sys/stat.h>
#if !defined(CSRG_BASED)
#include <sys/sysmacros.h>
#endif
#include <sys/param.h> /* MAXPATHLEN defined here */
#ifdef SVR4
#include <sys/utsname.h> /* SYS_NMLN */
#endif /* SVR4 */
#include <limits.h>
#include <dirent.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <rpc/rpc.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <X11/Xlib.h>
#include <Xm/DrawingA.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/ManagerP.h>
#include <Xm/GadgetP.h>
#include "util.h"
#include "misc.h"
#include "timeops.h"
#include "datefield.h"
#include "props.h"
#include "props_pu.h"
/*
#include "select.h"
*/
/*
#include "editor.h"
#include "graphics.h"
*/
#include "browser.h"
#include "blist.h"
/*
#include "alarm.h"
#include "dayglance.h"
#include "yearglance.h"
#include "weekglance.h"
*/
#include "calendar.h"
/*
#include "todo.h"
*/
#include "find.h"
#include "goto.h"
#include "tempbr.h"
/*
#include "gettext.h"
#include "namesvc.h"
#include "monthglance.h"
*/

#define gray_data_75_width      8
#define gray_data_75_height     8
#define gray_data_50_width      8
#define gray_data_50_height     8
#define gray_data_25_width      8
#define gray_data_25_height     8
#define black_data_width        8
#define black_data_height       8

#define solid_list_length               2
#define short_dotted_list_length        2
#define dotted_list_length              2
#define dot_dashed_list_length          4
#define short_dashed_list_length        2
#define long_dashed_list_length         2
#define odd_dashed_list_length          3
static unsigned char black_data[] = {
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF
};
static unsigned char gray_data_75[] = {
        0xDD,
        0xBB,
        0xEE,
        0xF7,
        0xDD,
        0xBB,
        0xEE,
        0xF7
};

static unsigned char gray_data_50[] = {
        0xAA,
        0x55,
        0xAA,
        0x55,
        0xAA,
        0x55,
        0xAA,
        0x55
};

static unsigned char gray_data_25[] = {
        0x88,
        0x22,
        0x44,
        0x11,
        0x88,
        0x22,
        0x44,
        0x11
};

static Pixmap black_data_pixmap = NULL;
static Pixmap gray_data_75_pixmap = NULL;
static Pixmap gray_data_50_pixmap = NULL;
static Pixmap gray_data_25_pixmap = NULL;
 
static unsigned char solid[solid_list_length] = {1, 0};
static unsigned char short_dotted[short_dotted_list_length] = {1, 1};
static unsigned char dotted[dotted_list_length] = {3, 1};
static unsigned char dot_dashed[dot_dashed_list_length] = {3, 4, 3, 1};
static unsigned char short_dashed[short_dashed_list_length] = {4, 4};
static unsigned char long_dashed[long_dashed_list_length] = {4, 7};
static unsigned char odd_dashed[odd_dashed_list_length] = {1, 2, 3};

static unsigned char *dash_list[] = {
        solid,
        short_dotted,
        dotted,
        dot_dashed,
        short_dashed,
        long_dashed,
        odd_dashed,
};

extern void
gr_clear_area(new_XContext *xc, int x, int y, Dimension w, Dimension h) {

        XFillRectangle(xc->display, xc->xid, xc->clear_gc, x, y, w, h);
/*
	XClearArea(xc->display, xc->xid, x, y, w, h, False);
*/
}

extern void
gr_clear_box (new_XContext *xc, int x, int y, int w, int h) {

        x+=1; y+=1; w-=1; h-=1;

        XFillRectangle(xc->display, xc->xid, xc->clear_gc, x, y, w, h);
/*
	XClearArea(xc->display, xc->xid, x, y, w, h, False);
*/
}

extern void
gr_make_grayshade(new_XContext *xc, int x, int y, int w, int h, int shade) {

        XColor colorcell_del, rgb_db_ref;
 
	XSetForeground((Display*)xc->display, (GC)xc->gc, (unsigned long)xc->colorcell_del[shade].pixel);
 
        XSetFillStyle(xc->display, xc->gc, FillSolid);
        XFillRectangle(xc->display, xc->xid, xc->gc, x, y, w, h);
}

extern void
gr_make_gray(new_XContext *xc, int x, int y, int w, int h, int percent) {

        /* Set up gray stipple */
        switch(percent) {
        case 25:
                XSetStipple(xc->display, xc->gc, gray_data_25_pixmap);
                break;
        case 50:
                XSetStipple(xc->display, xc->gc, gray_data_50_pixmap);
                break;
        case 75:
                XSetStipple(xc->display, xc->gc, gray_data_75_pixmap);
                break;
        default:
                XSetStipple(xc->display, xc->gc, gray_data_25_pixmap);
                break;
        }
 
        XSetForeground(xc->display, xc->gc, xc->foreground);
        XSetFillStyle(xc->display, xc->gc, FillOpaqueStippled);
        XFillRectangle(xc->display, xc->xid, xc->gc, x, y, w, h);
}

extern void
gr_make_rgbcolor(new_XContext *xc, Colormap cms, int x, int y, int w, int h, 
                 int r, int g, int b) {

        XColor colorcell_del;

        colorcell_del.red = (unsigned short)(r<<8);
        colorcell_del.green = (unsigned short)(g<<8);
        colorcell_del.blue = (unsigned short)(b<<8);
        XAllocColor(xc->display, cms, &colorcell_del);

        XSetForeground((Display*)xc->display, (GC)xc->gc,
                       (unsigned long)colorcell_del.pixel);
        XSetFillStyle(xc->display, xc->gc, FillSolid);
        XFillRectangle(xc->display, xc->xid, xc->gc, x, y, w, h);
}

extern int
gr_nchars(int area, char *str, Cal_Font *font) {

        char *buf;
        int i, l, w=0, n=0;
        int first = 1;
        int len = cm_strlen(str);
	int nop, width;

	CalTextExtents(font, str, len, &nop, &nop, &width, &nop);
        if (width <= area)
                return(strlen(str));

        for (i=0; i<len; i++) {
                if ( first ) {
                        buf = (char *)cm_mbchar(str);
                        first = 0;
                } else {
                        buf = (char *)cm_mbchar((char *)NULL);
                }
		l = cm_strlen(buf);
		CalTextExtents(font, buf, l, &nop, &nop, &width, &nop);
                w += width;
                if (w <= area)
                        n += mblen(buf, MB_LEN_MAX);
                else break;
        }
        return(n);
}

void
gr_draw_rgb_box(
	new_XContext	*xc,
	int		x,
	int		y,
	int		w,
	int		h,
	int		thickness,
	int		shade,
	Colormap	cms)

{
        XColor colorcell_del, rgb_db_ref;
        unsigned long pixel;
        int     gc_changed = 0;

        switch(shade) {
                case RED:
                        /*
                        XGetGCValues(xc->display, xc->gc,
                        GCForeground|GCFillStyle, xc->gcvals);
                        */
                        pixel = xc->colorcell_del[RED].pixel;
                break;
		default:
			pixel = shade;
		break;
        }

        XSetForeground((Display*)xc->display, (GC)xc->gc, pixel);
        XSetFillStyle(xc->display, xc->gc, FillSolid);

	for (;thickness > 0; thickness--) {
        	XDrawRectangle(xc->display, xc->xid, xc->gc, x, y, w, h);
		x++; y++;
		w -= 2; h -=2;
	}

	return;
}

void
gr_dissolve_box(
	new_XContext	*xc,
	int		x,
	int		y,
	int		w,
	int		h,
	int		thickness)
{
	for (;thickness > 0; thickness--) {
        	XDrawRectangle(xc->display, xc->xid, xc->clear_gc, x, y, w, h);
		x++; y++;
		w -= 2; h -=2;
	}
}

/*  given an area of a certain length (in pixels), compute
    where to lay down a string such that it's centered */
 
extern int
gr_center(int area, char *str, Cal_Font *font) {

        int i, strl, l, w, first = 1;
	int nop, width;
        char *buf;

        w = 0;
        strl = cm_strlen(str);
        for (i=0; i<strl; i++) {
                if ( first ) {
                        buf = (char*)cm_mbchar(str);
                        first = 0;
                } else {
                        buf = (char*)cm_mbchar((char *)NULL);
                }
                l = cm_strlen(buf);
		CalTextExtents(font, buf, l, &nop, &nop, &width, &nop);
                w += width;
        }
        return ((area - w)/2);
}        

extern void
gr_text(new_XContext *xc, int x, int y, Cal_Font *font, char *str, 
        XRectangle *rect) {
 
        int len = cm_strlen(str);
	int x2, y2, w, h;
	XRectangle overall_ink_return;
 
        if (rect != NULL) {
		CalTextExtents(font, str, len, &x2, &y2, &w, &h);

		overall_ink_return.x = x2 + rect->x;
		overall_ink_return.y = y2 + rect->y;
		overall_ink_return.width = w;
		overall_ink_return.height = h;

                if (!myrect_intersectsrect(rect, &overall_ink_return))
                        return;
        }
        CalDrawString(xc->display, xc->xid, font, xc->draw_gc, x, y,
                      str, cm_strlen(str));
}
 
extern void
gr_text_rgb(new_XContext *xc, int x, int y, Cal_Font *font, char *str, 
            Pixel shade, Colormap cms, XRectangle *rect) {

	XRectangle overall_ink_return;
	int x2, y2, w, h;
 
        if (rect != NULL) {
		CalTextExtents(font, str, cm_strlen(str), &x2, &y2, &w, &h);

		overall_ink_return.x = x2 + rect->x;
		overall_ink_return.y = y2 + rect->y;
		overall_ink_return.width = w;
		overall_ink_return.height = h;

                if (!myrect_intersectsrect(rect, &overall_ink_return))
                        return;
        }

        XSetForeground((Display*)xc->display, (GC)xc->gc,
			(unsigned long)shade);

        CalDrawString(xc->display, xc->xid, font, xc->gc, x, y, 
			str, cm_strlen(str));
}

extern void 
gr_draw_line(new_XContext *xc, int x1, int y1, int x2, int y2, 
             GR_Line_Style style, XRectangle *rect) {

        int dash_offset = 0;

        if (rect != NULL) {
                XRectangle    gr_rect;

                gr_rect.x = x1;
                gr_rect.y = y1;
                gr_rect.width = x2-x1;
                gr_rect.height = y2-y1;

                if (!myrect_intersectsrect(rect, &gr_rect))
                        return;
        }
        if (style==gr_solid) {
                XDrawLine(xc->display, xc->xid, xc->draw_gc, x1, y1, x2,
y2);
        }
        else {
                /* Set up and paint */
                XSetForeground(xc->display, xc->gc, xc->foreground);
                XSetDashes(xc->display, xc->gc, dash_offset,
                        (char *)dash_list[style],
                        short_dotted_list_length);
                XSetLineAttributes(xc->display, xc->gc, 0,
                        LineOnOffDash, CapNotLast, JoinMiter);
                XDrawLine(xc->display, xc->xid, xc->gc, x1, y1, x2, y2);
        }
 
}

extern void 
gr_draw_box(new_XContext *xc, int x, int y, int w, int h, XRectangle *rect) {

        if (rect != NULL) {
                XRectangle    gr_rect;
 
                gr_rect.x = x;
                gr_rect.y = y;
                gr_rect.width = w;
                gr_rect.height = h;

                if (!myrect_intersectsrect(rect, &gr_rect))
                        return;
        }
        XDrawRectangle(xc->display, xc->xid, xc->draw_gc, x, y, w, h);
}        

extern void 
gr_draw_glyph(new_XContext *src_xc, new_XContext *dst_xc, Pixmap pixmap, 
              int x, int y, int w, int h) {

        XSetStipple(src_xc->display, src_xc->gc, pixmap);
        XSetTSOrigin(src_xc->display, src_xc->gc, x, y);
        XFillRectangle(src_xc->display, dst_xc->xid, src_xc->gc, x, y, w, h);
}
 
static void
saturate(XColor *xclr)

{
	unsigned short n;

	n = MAX(xclr->red, xclr->green);
	n = MAX(n, xclr->blue);

	n = USHRT_MAX - n;

	xclr->red += n;
	xclr->blue += n;
	xclr->green += n;

	return;
}
 
/*
 * Allocate and initialize an XContext
 */
new_XContext *
gr_create_xcontext(Calendar *c, Widget widget, GR_depth depth, XtAppContext app)
{

        new_XContext	*xc;
        Colormap	cms = NULL;
        XGCValues	gc_vals, tmp_vals;
	GC		hilight_gc;
	XColor		exact_color;
	XColor		def_color;
 
        /* X Drawing Stuff */
        if ((xc = (new_XContext *) ckalloc(sizeof(new_XContext))) == NULL)
                return NULL;
 
        xc->display = (Display *) XtDisplay(widget);
        xc->app = app;
 
        if (depth == gr_mono) {
                xc->foreground = 1;
                xc->background = 0;
		xc->hilight_color = 1;
        } else {
		XtVaGetValues(widget,
			XmNbackground, &xc->background,
			XmNforeground, &xc->foreground,
			NULL);
		/*
		 * XXX EEEEEEK!  We use a private interface to get the
		 * hilight color.  We do this because there is no public
		 * interface.
		 */
		hilight_gc = XmParentHighlightGC(widget);
		XGetGCValues(xc->display, hilight_gc, GCForeground, &tmp_vals);
		xc->hilight_color = tmp_vals.foreground;
        }
 
        xc->xid = XtWindowOfObject(widget);
        xc->screen_depth = DefaultDepthOfScreen(XtScreen(widget));

        /*                                                      
         * Create general purpose gc.  This gc is changed as needed by
         * the drawing routines.
         */
        xc->gcvals = (XGCValues *) ckalloc(sizeof(XGCValues));
        xc->gcvals->foreground = xc->foreground;
        xc->gcvals->background = xc->background;

        xc->gcvals->fill_style = FillOpaqueStippled;
        xc->gc = XCreateGC(
                xc->display,
                xc->xid,
                GCForeground|GCBackground|GCFillStyle,
                xc->gcvals);

        /*
         * Specialized GCs. We create a couple of specialized GCs to increase   
         * the speed of common operations.  This way we don't need to change
         * the GC for every operation.
         */

        /* GC used for clearing */
        gc_vals.foreground = xc->background;
        xc->clear_gc = XCreateGC(xc->display, xc->xid, GCForeground, &gc_vals);
	
        /* Create GC used for inverting */
        gc_vals.foreground = xc->background;
        gc_vals.function = GXinvert;
        xc->invert_gc = XCreateGC(xc->display, xc->xid,
                                 GCForeground | GCFunction, &gc_vals);
	XSetSubwindowMode(xc->display, xc->invert_gc,  IncludeInferiors);

        /* Create GC used for drawing */
        gc_vals.function = GXcopy;
        gc_vals.foreground = xc->foreground;
        xc->draw_gc = XCreateGC(xc->display, xc->xid, GCForeground | GCFunction,
                                &gc_vals);
        XSetLineAttributes(xc->display, xc->draw_gc, 0,
                           LineSolid, CapNotLast, JoinMiter);

        return xc;                                            
}
 
 
/*
 * Set the clip mask for all gcs the graphics package uses
 */

extern void 
gr_set_clip_rectangles(new_XContext *xc, int x, int y, XRectangle *rectangles, 
                       int n, int ordering) {

        XSetClipRectangles(xc->display, xc->gc, x, y, rectangles, n, ordering);
        XSetClipRectangles(xc->display, xc->draw_gc,
                           x, y, rectangles, n, ordering);
        XSetClipRectangles(xc->display, xc->clear_gc,
                           x, y, rectangles, n, ordering);
        XSetClipRectangles(xc->display, xc->invert_gc,
                           x, y, rectangles, n, ordering);
        return;
}

extern void 
gr_clear_clip_rectangles(new_XContext *xc) {

        XSetClipMask(xc->display, xc->gc, None);
        XSetClipMask(xc->display, xc->draw_gc, None);
        XSetClipMask(xc->display, xc->clear_gc, None);
        XSetClipMask(xc->display, xc->invert_gc, None);
}

extern Boolean
gr_create_stipple(new_XContext *xc, unsigned char *data, int datawidth, int dataheight, 
                  Pixmap *stipple, unsigned int *width, unsigned int *height) {

 
        Boolean ok = True;  
 
        if ((*stipple = XCreateBitmapFromData(xc->display,
                xc->xid,
        /*      RootWindow(xc->display, DefaultScreen(xc->display)),  */
                (char *)data, datawidth, dataheight)) == 0) {
                        ok = False;
                }
        else {
                *width  = datawidth;
                *height = dataheight;
        }
        return(ok);
}        
 
extern Boolean
gr_init(new_XContext *xc, Widget canvas) {

        unsigned int width, height;
        Boolean ok = True;
        XColor  rgb_db_ref;
 
        Display *dpy = XtDisplay(canvas);
        int scr = DefaultScreen(dpy);
        Colormap cmap = DefaultColormap(dpy, scr);

        ok = gr_create_stipple(xc, black_data,     
                black_data_width, black_data_height,
                &black_data_pixmap, &width, &height);
        ok = gr_create_stipple(xc, gray_data_75,
                gray_data_75_width, gray_data_75_height,
                &gray_data_75_pixmap, &width, &height);
        ok = gr_create_stipple(xc, gray_data_50,
                gray_data_50_width, gray_data_50_height,
                &gray_data_50_pixmap, &width, &height);
        ok = gr_create_stipple(xc, gray_data_25,
                gray_data_25_width, gray_data_25_height,
                &gray_data_25_pixmap, &width, &height);

        ok = XAllocNamedColor(dpy, cmap,                
                "dark slate grey", &xc->colorcell_del[DARKGREY], &rgb_db_ref);
        ok = XAllocNamedColor(dpy, cmap,
                "dim grey", &xc->colorcell_del[DIMGREY], &rgb_db_ref);
        ok = XAllocNamedColor(dpy, cmap,
                "grey", &xc->colorcell_del[GREY], &rgb_db_ref);
        ok = XAllocNamedColor(dpy, cmap,
                "light grey", &xc->colorcell_del[LIGHTGREY], &rgb_db_ref);
        ok = XAllocNamedColor(dpy, cmap,
                "red", &xc->colorcell_del[RED], &rgb_db_ref);

        return(ok);                                           
}
