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
/* $XConsortium: IconP.h /main/4 1995/11/06 09:42:03 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

/*****************************************************************************
*
*  IconP.H - widget private header file
*  
******************************************************************************/

#ifndef _IconP_h
#define _IconP_h

#include <Xm/XmP.h>
#include <Xm/PrimitiveP.h>

#include "Icon.h"

#ifdef __cplusplus
extern "C" {
#endif

#define Max(x, y) (((x) > (y)) ? (x) : (y))

typedef struct _IconClassPart {
    int reserved;
} IconClassPart;

typedef struct _IconClassRec {
    CoreClassPart core_class;
    XmPrimitiveClassPart primitive_class;
    IconClassPart icon_class;
} IconClassRec;

extern IconClassRec iconClassRec;

typedef struct _IconPart {
    /* Public Resources */
    XmString		label;
    XmString		top_label;
    XmString		bottom_label;
    Pixmap		pixmap;
    Pixmap		mask;
    Pixmap		state_pixmap;
    Pixmap		state_mask;
    unsigned char	state_gravity;
    Pixel		select_color;
    XmFontList		font;
    unsigned char	alignment;
    unsigned char	string_direction;
    unsigned char	icon_placement;
    unsigned char	icon_shadow_type;
    Boolean		active;
    Boolean		select_color_persistent;
    Boolean		shrink_outline;
    Boolean		word_wrap;
    Boolean		selected;
    Boolean		show_selected_pixmap;
    Boolean		resize_width;
    Boolean		resize_height;
    Dimension		icon_shadow_thickness;
    Dimension		icon_margin_thickness;   /* margin around widget */
    XtCallbackList	single_click_callback;
    XtCallbackList	double_click_callback;
    GuiIconFields	fields;

    /* Private stuff */
    GC		gc;
    GC		stipple_gc;
    GC		selected_fg_gc;
    GC		selected_bg_gc;
    GC		mask_gc;
    GC		state_mask_gc;
    GC		mask_stipple_gc;
    Pixel	select_label_color;
    XmString	wrapped_label;
    XmRegion	shadow_region;
    XmRegion	highlight_region;
    Position	pixmap_x;
    Position	pixmap_y;
    Position	label_x;
    Position	label_y;
    Position	top_label_x;
    Position	top_label_y;
    Position	bottom_label_x;
    Position	bottom_label_y;
    Position	state_pixmap_x;
    Position	state_pixmap_y;
    Dimension	pixmap_width;
    Dimension	pixmap_height;
    Dimension	top_label_width;
    Dimension	top_label_height;
    Dimension	bottom_label_width;
    Dimension	bottom_label_height;
    Dimension	label_width;
    Dimension	label_height;
    Dimension	state_pixmap_width;
    Dimension	state_pixmap_height;
    Time	old_time;
    Boolean	old_shrink_outline;
} IconPart;

typedef struct _IconRec {
    CorePart core;
    XmPrimitivePart primitive;
    IconPart icon;
} IconRec;

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _IconP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
