/*  $XConsortium: Fonts.h /main/3 1996/04/21 19:52:59 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef FONTS_H
#define FONTS_H
 
typedef struct _FontType {
        XmFontType      cf_type;
        union {
                XFontStruct     *cf_font;
                XFontSet         cf_fontset;
        } f;
} FontType;

extern Boolean
fontlist_to_font(
        XmFontList       font_list,
        FontType        *new_font);

extern void
load_app_font(
        Widget          w,
        FontType        *userfont,
        char		**fontname);

#endif /* FONTS_H */
