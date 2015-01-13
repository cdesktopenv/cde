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
/* $TOG: icon.c /main/5 1998/01/09 15:29:53 cshi $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#define   NO_RES_DISPLAY        0
#define   LOW_RES_DISPLAY       1
#define   VGA_RES_DISPLAY       2
#define   MED_RES_DISPLAY       3
#define   HIGH_RES_DISPLAY      4

#define  DTCM_STD_ICON	"IcCM"
#define  DTCM_INV_ICON	"IcCMinv"

#include <stdlib.h>
#include <stdio.h>
#include <Xm/Xm.h>
#include "calendar.h"

/* XXX: Defined in Xm/IconFile.h but the file isn't installed??? */ 
/*
 * values for icon magnitude
 */
#define XmUNSPECIFIED_ICON_SIZE 0
#define XmLARGE_ICON_SIZE       1
#define XmMEDIUM_ICON_SIZE      2
#define XmSMALL_ICON_SIZE       3
#define XmTINY_ICON_SIZE        4
extern String XmGetIconFileName( 
                        Screen *screen,
                        String imageInstanceName,
                        String imageClassName,
                        String hostPrefix,
                        unsigned int size) ;

/* XXX: Private routines, but what are my other choices? */
extern Pixmap _DtGetMask(Screen *screen, char *image_name);
extern int _DtGetDisplayResolution(Display *disp, int screen);

static Boolean load_icon(Calendar *c, IconInfo	*icon, char *icon_name, int size, Pixel, Pixel);

static int
wm_icon_preference(
	Display		*dpy,
	Screen		*screen,
	int		 size)
{
	int		 icon_size_count,
			 cal_icon_size,
			 i;
	XIconSize       *icon_sizes;

	if (XGetIconSizes(dpy, XRootWindowOfScreen(screen),
			  &icon_sizes, &icon_size_count) == 0)
		return (size);

	/* Calendar icons are either 32x32 or 48x48 */
	if (size == XmLARGE_ICON_SIZE)
		cal_icon_size = 48;
	else
		cal_icon_size = 32;

	for (i = 0; i < icon_size_count; i++) {
		if (icon_sizes[i].min_width <= cal_icon_size &&
		    icon_sizes[i].max_width >= cal_icon_size &&
		    icon_sizes[i].min_height <= cal_icon_size &&
		    icon_sizes[i].max_height >= cal_icon_size) {

			XFree(icon_sizes);
			return (size);
		}
	}
	/* If we are here then WM doesn't want us to use this icon
	 * size.  If we are using 48x48 then we can see if 32x32 will
	 * work.
	 */
	if (size == XmLARGE_ICON_SIZE)
		cal_icon_size = 32;

	for (i = 0; i < icon_size_count; i++) {
		if (icon_sizes[i].min_width <= cal_icon_size &&
		    icon_sizes[i].max_width >= cal_icon_size &&
		    icon_sizes[i].min_height <= cal_icon_size &&
		    icon_sizes[i].max_height >= cal_icon_size) {

			XFree(icon_sizes);
			return (XmMEDIUM_ICON_SIZE);
		}
	}
	XFree(icon_sizes);
	return (0);
}

/*
 * We load two icons.  The main icon used when the tool is iconic and an
 * inverted icon that is used during flashing when a reminder occurs.
 */

static Boolean
load_icons(
	Calendar	*c)
{
	Pixel		 fg, bg;
	int		 resolution,
			 size;
	Screen		*screen = XtScreen(c->frame);

	if (!c->icon)
		c->icon = (IconInfo *) ckalloc(sizeof(IconInfo));
	if (!c->icon_inverted)
		c->icon_inverted = (IconInfo *) ckalloc(sizeof(IconInfo));

	XtVaGetValues(c->canvas, 
			XmNforeground,  &fg, 
			XmNbackground,  &bg,
			NULL);

	resolution = _DtGetDisplayResolution(XtDisplay(c->frame), 
				XScreenNumberOfScreen(screen));

	/*
	 * The CDE spec says we use a large icon in high and medium
	 * resolution.  We use a medium size icon in low res.
	 */
	switch (resolution) {
	    case HIGH_RES_DISPLAY:
	    case MED_RES_DISPLAY:
		size = XmLARGE_ICON_SIZE;
		break;
	    case LOW_RES_DISPLAY:
	    default:
		size = XmMEDIUM_ICON_SIZE;
		break;
	}

	/* See if the resolution size we have chosen is acceptable by the WM */
	if (!(size = wm_icon_preference(XtDisplay(c->frame), screen, size))) {
		c->icon->icon = 0;
		c->icon_inverted->icon = 0;
		return (FALSE);
	}

	if (!load_icon(c, c->icon, DTCM_STD_ICON, size, fg, bg))
		return (FALSE);

	/* 
	 * If we are color we load a color icon that has been inverted
	 * by a graphic's artist.  If we are mono or reduced color we take 
	 * our one bit deep pixmap and invert it ourself by switching fg and
	 * bg.
	 */
	if (c->icon->icon_depth >= 8) {
		if (!load_icon(c, c->icon_inverted, DTCM_INV_ICON, size, 
			       fg, bg))
			return (FALSE);
	} else {
		/* Reverse fg/bg on STD_ICON to get inverted icon. */
		/* XXX: This probably won't work on 1 < depth < 8 visuals */
		if (!load_icon(c, c->icon_inverted, DTCM_STD_ICON, size, 
			       bg, fg))
			return (FALSE);
	}
	return (TRUE);
}

static Boolean
load_icon(
	Calendar	*c,
	IconInfo	*icon,
	char		*icon_name,
	int		 size,
	Pixel		 fg,
	Pixel		 bg)
{
	char		*icon_filename;
	int		 x, y;
	Window		 root;
	unsigned int	 border_width;
	Screen		*screen = XtScreen(c->frame);

	if (!(icon_filename = XmGetIconFileName(screen, NULL, icon_name, 
								NULL, size))) {
		icon->icon = 0;
		return (FALSE);
	}

	if ((icon->icon = XmGetPixmap(screen, icon_filename, fg, bg)) ==
							XmUNSPECIFIED_PIXMAP) {
		free(icon_filename);
		icon->icon = 0;
		return (FALSE);
	}

	if ((icon->icon_mask = _DtGetMask(screen, icon_filename)) ==
							XmUNSPECIFIED_PIXMAP) {
		free(icon_filename);
		icon->icon = 0;
		icon->icon_mask = 0;
		return (FALSE);
	}

	/* Store away the size of the icon */
        XGetGeometry(XtDisplay(c->frame),
                     icon->icon,
                     &root,
                     &x, &y,
                     &icon->icon_width, &icon->icon_height,
                     &border_width, &icon->icon_depth);

	XtFree(icon_filename);
	return (TRUE);
}

static void
free_icon(
	Calendar	*c,
	IconInfo	*icon)
{
	if (!icon)
		return;

	if (icon->icon)
		XmDestroyPixmap(XtScreen(c->frame), icon->icon);

	icon->icon = 0;

	if (icon->icon_mask)
		XmDestroyPixmap(XtScreen(c->frame), icon->icon_mask);

	icon->icon_mask = 0;
}

static void
paint_icon_date(
	Calendar 	*c)
{
        Tick 		 current_time;
        char 		 buf[25];
        Cal_Font 	*font = c->fonts->iconfont;
        new_XContext 	*xc;
	int		 len, 
			 x, y, 
			 width, height, 
			 nop;
	Screen		*s = XtScreen(c->canvas);
	Pixel		 fg, bg;
	GC		 gc;
	XGCValues        gc_vals;
	Display		*dpy;

	if (!(c->icon && c->icon->icon && c->icon->icon_mask))
		return;

	dpy = XtDisplayOfObject(c->frame);

        current_time = now();
        XtVaSetValues(c->frame, XmNiconName, months[month(current_time)], NULL);
        xc = c->xcontext;

	if (c->icon->icon_depth == 1) {
		XtVaGetValues(c->canvas, 
			XmNforeground,  &fg, 
			XmNbackground,  &bg,
			NULL);
	} else {
		fg = BlackPixelOfScreen(s);
		bg = WhitePixelOfScreen(s);
	}

	gc_vals.foreground = fg;
	gc_vals.function = GXcopy;
	gc_vals.fill_style = FillSolid;
	gc = XCreateGC(xc->display, c->icon->icon, GCForeground | GCFunction |
						   GCFillStyle, &gc_vals);

	/* painting new date */
        /* day of the month */
        (void) sprintf(buf, "%d", dom(current_time));
	len = cm_strlen(buf);
	CalTextExtents(font, buf, len, &nop, &nop, &width, &height);
	x = c->icon->icon_width/2 - width/2 - 1;
	y = c->icon->icon_height - 2 * (c->icon->icon_height - height) / 3;

	CalDrawString(dpy, c->icon->icon, font, gc, x, y, buf, len);

	gc_vals.foreground = bg;
	XChangeGC(xc->display, gc, GCForeground, &gc_vals);
	if (c->icon_inverted && c->icon_inverted->icon 
			     && c->icon_inverted->icon_mask)
		CalDrawString(dpy, c->icon_inverted->icon, font, 
				   gc, x, y, buf, len);

	XFreeGC(xc->display, gc);
	XFlush(xc->display);
}
 
void
paint_icon(Calendar *c)
{

	/* Free the old pixmaps since they have the wrong date in them. */
	if (c->icon && c->icon->icon)
		free_icon(c, c->icon);
	if (c->icon_inverted && c->icon_inverted->icon)
		free_icon(c, c->icon_inverted);

	/* Load the icon, mask and related info into calendar->icon */
	load_icons(calendar);

        paint_icon_date(c);

        XtVaSetValues(c->frame, XmNiconPixmap, NULL, NULL);

	if (c->icon && c->icon->icon && c->icon->icon_mask)
        	XtVaSetValues(c->frame, 
			XmNiconPixmap, c->icon->icon,
			XmNiconMask, c->icon->icon_mask,
			NULL);
}
