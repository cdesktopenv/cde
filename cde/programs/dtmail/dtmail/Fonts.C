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
/*  $TOG: Fonts.C /main/8 1997/09/04 08:06:26 mgreess $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 *
 * This code was borrowed from dtcm (Calendar Manager).
 * Its purpose is to find a font in a particular family that is
 * the same size as the user font being used.
 */

#include <X11/Xlib.h>
#include <Xm/Xm.h>
#include <Xm/AtomMgr.h>
#include <stdio.h>
#include <stdlib.h>
#include "Fonts.h"


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
    XtPointer	 	 fl_entry_font = NULL, font_to_use = NULL;
    char		*fl_entry_font_tag;
    Boolean		 found_font_set = False,
			 found_font_struct = False,
			 do_break = False;

    *type_return = XmFONT_IS_FONT;

    if (!XmFontListInitFontContext(&fl_context, font_list))
      		return (XtPointer)NULL;

    do
    {
	fl_entry = XmFontListNextEntry(fl_context);
	if (fl_entry)
	{
	    fl_entry_font = XmFontListEntryGetFont(fl_entry, type_return);
	    if (*type_return == XmFONT_IS_FONTSET)
	    {
		fl_entry_font_tag = XmFontListEntryGetTag(fl_entry);

		/* 
		 * Save the first font set found in-
		 * case the default tag is not set.
		 */
		if (!found_font_set)
		{
		    font_to_use = fl_entry_font;
		    found_font_set = True;
		    found_font_struct = True;

		    if (!strcmp(XmFONTLIST_DEFAULT_TAG, fl_entry_font_tag))
		      do_break = True;
		}
		else if (!strcmp(XmFONTLIST_DEFAULT_TAG, fl_entry_font_tag))
		{
		    /* Found right font set */
		    font_to_use = fl_entry_font;
		    do_break = True;
		}

		XtFree((char*) fl_entry_font_tag);

		if (do_break)
		  break;
	    }
	    else if (!found_font_struct)
	    {
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

/*
 * Strip the font out of a give fontlist.
 */
Boolean
fontlist_to_font(
	XmFontList	 font_list,
	FontType	*new_font)
{
	XmFontType	 type_return;
	XtPointer	 font_data;

	if (!font_list) return False;

	font_data = get_font(font_list, &type_return);

	if (!font_data) 
		return False;

	new_font->cf_type = type_return;

	if (type_return == XmFONT_IS_FONTSET)
		new_font->f.cf_fontset = (XFontSet)font_data;
	else
		new_font->f.cf_font = (XFontStruct *)font_data;

	return True;
}

/*
 * Determine the pixel size of the user font.  Try to match a symbol
 * font to that size.  If one can't be found, return NULL, calling
 * function will probably default to the user font.
 */
void
load_app_font(
	Widget		w, 
	FontType	*userfont,
	char		**fontname)
{
	unsigned long	 pixel_size;
	Display		*dpy = XtDisplay(w);
	char		 font_name[128],
			*font_name_ptr = font_name,
		       **font_names;
	int		 nnames;
	Atom		 pixel_atom = XmInternAtom(dpy, "PIXEL_SIZE", FALSE);

	/* First get the pixel size from the User Font */
	if (userfont->cf_type == XmFONT_IS_FONT) {
		/* If we can't get the pixel size from the user font we
		 * default to a 12 pixel font.
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
			if (!XGetFontProperty(font_struct_list[0],
					      pixel_atom, 
					      &pixel_size))
				pixel_size = 12;
		}
	}

	/* See if the font exists */
	sprintf (font_name,
		"-dt-application-medium-r-normal--%ld-*-*-*-*-*-dtsymbol-*",
		pixel_size);
	font_names = XListFonts(dpy, font_name, 1, &nnames);

	if (!nnames) {
	    /* Try again */
	    sprintf (font_name,
		"-*-symbol-medium-r-normal--%ld-*-*-*-*-*", pixel_size);
	    font_names = XListFonts(dpy, font_name, 1, &nnames);
	}
	if (!nnames) {
	    /* Try again with the default size */
	    pixel_size = 12;
	    sprintf (font_name,
		"-dt-application-medium-r-normal--%ld-*-*-*-*-*-dtsymbol-*",
		pixel_size);
	    font_names = XListFonts(dpy, font_name, 1, &nnames);
	}
	if (!nnames) {
	    *fontname = NULL;
	    return;
	}

	XFreeFontNames(font_names);
	*fontname = XtNewString (font_name);
}

