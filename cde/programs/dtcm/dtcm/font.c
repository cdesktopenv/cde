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
/* $XConsortium: font.c /main/4 1995/11/09 12:59:38 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#include <X11/Xlib.h>
#include <Xm/Xm.h>
#include <Xm/AtomMgr.h>
#include <stdio.h>
#include <stdlib.h>
#include "calendar.h"

#ifndef ABS
#define ABS(x) (((x) > 0) ? (x) : (-(x)))
#endif /* ABS */

/*
 * Walk a font_list looking for a FontSet with the 
 * XmFONTLIST_DEFAULT_TAG set.  If we fail to find a FontSet with this tag,
 * return the first FontSet found.  If we fail to find a FontSet return
 * the first font found.
 *
 * This function returns either a XFontStruct or a XFontSet.  The type can
 * be determined by the value of type_return which will equal either
 * XmFONT_IS_FONTSET or XmFONT_IS_FONT.
 *
 * The XFontStruct or XFontSet that is returned is not a copy and should
 * not be freed.
 */
static XtPointer
get_font(
	XmFontList	 font_list,
	XmFontType	*type_return)
{
	XmFontContext	 fl_context;
	XmFontListEntry	 fl_entry;
	XtPointer	 fl_entry_font,
			 font_to_use;
	char		*fl_entry_font_tag;
	Boolean		 found_font_set = False,
			 found_font_struct = False;

	*type_return = XmFONT_IS_FONT;

	if (!XmFontListInitFontContext(&fl_context, font_list))
		return (XtPointer)NULL;

	do {
		fl_entry = XmFontListNextEntry(fl_context);
		if (fl_entry) {
			fl_entry_font = 
				XmFontListEntryGetFont(fl_entry, type_return);
			if (*type_return == XmFONT_IS_FONTSET) {
				fl_entry_font_tag = 
						XmFontListEntryGetTag(fl_entry);
					/* 
					 * Save the first font set found in-
					 * case the default tag is not set.
					 */
				if (!found_font_set) {
					font_to_use = fl_entry_font;
					found_font_set = True;
					found_font_struct = True;

					if (!strcmp(XmFONTLIST_DEFAULT_TAG,
						    fl_entry_font_tag))
						break;
				} else if (!strcmp(XmFONTLIST_DEFAULT_TAG, 
						    fl_entry_font_tag)) {
					/* Found right font set */
					font_to_use = fl_entry_font;
					break;
				}
			} else if (!found_font_struct) {
				font_to_use = fl_entry_font;
				found_font_struct = True;
			}
		}
	} while (fl_entry != NULL);

	XmFontListFreeFontContext(fl_context);

	if (!found_font_set && !found_font_struct)
		return (XtPointer)NULL;

	return (XtPointer)font_to_use;
}

extern Boolean
fontlist_to_font(
	XmFontList	 font_list,
	Cal_Font	*cal_font)
{
	XmFontType	 type_return;
	XtPointer	 font_data;

	if (!font_list) return False;

	font_data = get_font(font_list, &type_return);

	if (!font_data) 
		return False;

	cal_font->cf_type = type_return;

	if (type_return == XmFONT_IS_FONTSET)
		cal_font->f.cf_fontset = (XFontSet)font_data;
	else
		cal_font->f.cf_font = (XFontStruct *)font_data;

	return True;
}

/*
 * Look for a font that is closest to the requested point size and resolution
 * using the weight and family name specified.
 */
static Boolean
ProbeForFont(
	Calendar	*cal, 
	Font_Weight	 weight, 
	unsigned long	 target_pixel_size, 
	int		 with_foundry,
	char	        *font_name)
{
	char	       **font_names,
			 notused[48];
	int		 i, 
			 nnames,
			 size = 0,
			 closest_size_diff = 100000,
			 closest_size,
			 closest_index = -1;

	if (!strcmp(cal->app_data->app_font_family, "application")) {
		sprintf (font_name, "-%s-%s-%s-r-normal-sans-*-*-*-*-p-*",
				(with_foundry)?"dt":"*",
				cal->app_data->app_font_family,
				(weight == BOLD)?"bold":"medium");
	} else {
		sprintf (font_name, "-%s-%s-%s-r-normal--*-*-*-*-*-*",
				(with_foundry)?"dt":"*",
				cal->app_data->app_font_family,
				(weight == BOLD)?"bold":"medium");
	}

	/* See if the font exists */
	font_names = XListFonts(XtDisplay(cal->frame), font_name, 80, &nnames);
	if (!nnames) {
		if (with_foundry)
			return ProbeForFont(cal, weight, target_pixel_size, 
							     FALSE, font_name);
		else
			return FALSE;
	}

	/* For the fonts that match, get their pixel size and 
	 * look for the one with the pixel size closest to the size we are 
	 * looking for.
	 */
	for (i = 0; i < nnames; i++) {
		sscanf(font_names[i], 
		       "-%[^-]-%[^-]-%[^-]-%[^-]-%[^-]-%[^-]-%d-%s",
			notused, notused, notused, notused, notused, notused,
			&size, notused);

		if (!size)
			sscanf(font_names[i], 
			    "-%[^-]-%[^-]-%[^-]-%[^-]-%[^-]--%d-%s",
			     notused, notused, notused, notused, notused,
			     &size, notused);

		if (size) {
			if ((ABS((int)(target_pixel_size - size)) <= 
							closest_size_diff)) {
				closest_size_diff = 
					ABS((int)(target_pixel_size - size));
				closest_size = size;
				closest_index = i;
				if (closest_size_diff == 0)
					break;
			}
			size = 0;
		}
	}

	if (closest_index == -1)
		return FALSE;

	/* This one is the closest in size */
	sprintf (font_name, "-%s-%s-%s-r-normal-%s-%d-*-*-*-*-*",
		 (with_foundry)?"dt":"*",
		 cal->app_data->app_font_family,
		 (weight == BOLD)?"bold":"medium",
		 (!strcmp(cal->app_data->app_font_family, "application")) ?
								"sans": "",
		 closest_size);
		
	XFreeFontNames(font_names);

	return TRUE;
}

extern void
load_app_font(
	Calendar	*cal, 
	Font_Weight	 weight,
	Cal_Font	*userfont,
	Cal_Font	*return_font)
{
	unsigned long	 pixel_size;
	Display		*dpy = XtDisplay(cal->frame);
	char		 font_name[128],
			*font_name_ptr = font_name,
		       **font_names;
	int		 nnames;
	XrmValue	 in_font;
	XrmValue	 out_fontlist;
	XmFontList	 font_list;
	Atom		 pixel_atom = XmInternAtom(dpy, "PIXEL_SIZE", FALSE);

	/* First get the pixel size from the User Font */
	if (userfont->cf_type == XmFONT_IS_FONT) {
		/* If we can't get the pixel size from the user font we
		 * defaults to a 12 pixel font.
		 */
		if (!XGetFontProperty(userfont->f.cf_font, pixel_atom, 
							&pixel_size))
			pixel_size = 12;
	} else {
		XFontStruct 	**font_struct_list;
		char		**font_name_list;
		int		  list_size;

		if (!(list_size = XFontsOfFontSet(userfont->f.cf_fontset,
					    &font_struct_list,
					    &font_name_list))) {
			pixel_size = 12;
		} else {
			int i;
			if (!XGetFontProperty(font_struct_list[0],
					      pixel_atom, 
					      &pixel_size))
				pixel_size = 12;
		}
	}

	/* If the font family is not ``application'' then it is probably
	 * multibyte so we can't assume there is an add-style or a proportional
	 * version available.
	 */
	if (!strcmp(cal->app_data->app_font_family, "application")) {
		sprintf (font_name, "-dt-%s-%s-r-normal-sans-%lu-*-*-*-p-*",
				cal->app_data->app_font_family,
				(weight == BOLD)?"bold":"medium",
				pixel_size);
	} else {
		sprintf (font_name, "-dt-%s-%s-r-normal--%lu-*-*-*-*-*",
				cal->app_data->app_font_family,
				(weight == BOLD)?"bold":"medium",
				pixel_size);
	}

	/* See if the font exists */
	font_names = XListFonts(dpy, font_name, 1, &nnames);
	if (!nnames) {
		if (!ProbeForFont(cal, weight, pixel_size, TRUE, 
							font_name_ptr)) {
			/* We didn't find anything */
			*return_font = *userfont;
			return;
		}
	} else
		XFreeFontNames(font_names);

	strcat(font_name, ":");
	in_font.size = strlen(font_name);
	in_font.addr = font_name;
	out_fontlist.size = sizeof(XmFontList);
	out_fontlist.addr = (XPointer)&font_list;

	/* Convert font string to a font list. */
	if (!XtConvertAndStore(cal->frame, XtRString, &in_font, XmRFontList,
				&out_fontlist)) {
		*return_font = *userfont;
	}

	if (!fontlist_to_font(font_list, return_font)) {
		*return_font = *userfont;
	}
}

/*
 * Given a font or fontset, return the extents of the text.
 */
void
CalTextExtents(
	Cal_Font	*font,
	char		*string,
	int		 nbytes,
	int		*x_return,
	int		*y_return,
	int		*width_return,
	int		*height_return)
{
	
	if (!font) return;

	if (font->cf_type == XmFONT_IS_FONT) {
		int		direction,
				ascent,
				descent;
		XCharStruct	overall;

		XTextExtents(font->f.cf_font, string, nbytes,
				&direction, &ascent, &descent, &overall);

		*x_return = overall.lbearing;
		*y_return = overall.ascent;
		*width_return = overall.width;
		*height_return = overall.ascent + overall.descent;
	} else {
		XRectangle	ink,
				logical;

		XmbTextExtents(font->f.cf_fontset, string, nbytes,
				&ink, &logical);

		*x_return = logical.x;
		*y_return = logical.y;
		*width_return = logical.width;
		*height_return = logical.height;
	}
}

/*
 * Given a font or fontset, render the text.
 */
void
CalDrawString(
	Display		*dpy,
	Drawable	 draw,
	Cal_Font	*font,
	GC		 gc,
	int		 x,
	int		 y,
	char		*string,
	int		 length)
{

	if (font->cf_type == XmFONT_IS_FONT) {
		XSetFont(dpy, gc, font->f.cf_font->fid);
		XDrawString(dpy, draw, gc, x, y, string, length);
	} else {
		XmbDrawString(dpy, draw, font->f.cf_fontset, gc, x, y, 
							string, length);
	}
}

void
CalFontExtents(
	Cal_Font	*font,
	XFontSetExtents *fse)
{
	if (font->cf_type == XmFONT_IS_FONT) {
		/*
		 * These computations are stolen from _XsimlCreateFontSet()
		 * in Xlib.
		 */
		fse->max_ink_extent.x = font->f.cf_font->min_bounds.lbearing;
		fse->max_ink_extent.y = - font->f.cf_font->max_bounds.ascent;
		fse->max_ink_extent.width = font->f.cf_font->min_bounds.width;
		fse->max_ink_extent.height = 
					 font->f.cf_font->max_bounds.ascent +
					 font->f.cf_font->max_bounds.descent;

		fse->max_logical_extent.x = 0;
		fse->max_logical_extent.y = - font->f.cf_font->ascent;
		fse->max_logical_extent.width = 
					 font->f.cf_font->max_bounds.width;
		fse->max_logical_extent.height = 
					 font->f.cf_font->ascent +
					 font->f.cf_font->descent;
	} else {
		*fse = *XExtentsOfFontSet(font->f.cf_fontset);
	}
}
