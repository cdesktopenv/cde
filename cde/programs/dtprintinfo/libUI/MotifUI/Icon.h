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
/* $XConsortium: Icon.h /main/3 1995/11/06 09:41:27 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

/*****************************************************************************
*
*  Icon.h - widget public header file
*  
******************************************************************************/

#ifndef _Icon_h
#define _Icon_h

#ifdef __cplusplus
extern "C" {
#endif

externalref WidgetClass iconWidgetClass;

typedef struct _IconClassRec *IconWidgetClass;
typedef struct _IconRec *IconWidget;

enum {
     GuiSINGLE_CLICK, 
     GuiDOUBLE_CLICK
};

enum {
     GuiPIXMAP_TOP, 
     GuiPIXMAP_BOTTOM, 
     GuiPIXMAP_LEFT, 
     GuiPIXMAP_RIGHT
};

enum {
    GuiNORTHWEST_GRAVITY, 
    GuiNORTH_GRAVITY, 
    GuiNORTHEAST_GRAVITY, 
    GuiWEST_GRAVITY, 
    GuiCENTER_GRAVITY, 
    GuiEAST_GRAVITY, 
    GuiSOUTHWEST_GRAVITY, 
    GuiSOUTH_GRAVITY, 
    GuiSOUTHEAST_GRAVITY
};

typedef struct
{
    int		reason;
    XEvent	*event;
    Boolean	prev_selected;
    XmString	string;
    int		field_index;
    XmString	field_string;
} GuiIconCallbackStruct, *GuiIconCallback;

typedef struct
{
    Boolean		free_data;
    Dimension   	name_width;
    int			n_fields;
    int			field_spacing;
    XmString		*fields;
    Dimension   	*widths;
    unsigned char	*alignments;
    Boolean		*draw_fields;
    Boolean		*selected;
    Boolean		*active;
} GuiIconFieldsStruct, *GuiIconFields, **GuiIconFieldsList;

#define GuiIsIcon(w) XtIsSubclass((w), iconWidgetClass)

/* Icon Resources */

extern const char gui_icon_strings[];

#define GuiNtopLabelString ((char*)&gui_icon_strings[0])
#define GuiNbottomLabelString ((char*)&gui_icon_strings[15])
#define GuiNpixmapPlacement ((char*)&gui_icon_strings[33])
#define GuiCPixmapPlacement ((char*)&gui_icon_strings[49])
#define GuiRPixmapPlacement ((char*)&gui_icon_strings[65])
#define GuiNdoubleClickCallback ((char*)&gui_icon_strings[81])
#define GuiNsingleClickCallback ((char*)&gui_icon_strings[101])
#define GuiNselected ((char*)&gui_icon_strings[121])
#define GuiCSelected ((char*)&gui_icon_strings[130])
#define GuiNiconShadowType ((char*)&gui_icon_strings[139])
#define GuiCIconShadowType ((char*)&gui_icon_strings[154])
#define GuiNiconShadowThickness ((char*)&gui_icon_strings[169])
#define GuiCIconShadowThickness ((char*)&gui_icon_strings[189])
#define GuiNtextSelectColor ((char*)&gui_icon_strings[209])
#define GuiCTextSelectColor ((char*)&gui_icon_strings[225])
#define GuiNselectColor ((char*)&gui_icon_strings[241])
#define GuiCSelectColor ((char*)&gui_icon_strings[253])
#define GuiNselectColorPersistent ((char*)&gui_icon_strings[265])
#define GuiCSelectColorPersistent ((char*)&gui_icon_strings[287])
#define GuiNshowSelectedPixmap ((char*)&gui_icon_strings[309])
#define GuiCShowSelectedPixmap ((char*)&gui_icon_strings[328])
#define GuiNiconMarginThickness ((char*)&gui_icon_strings[347])
#define GuiCIconMarginThickness ((char*)&gui_icon_strings[367])
#define GuiNactive ((char*)&gui_icon_strings[387])
#define GuiCActive ((char*)&gui_icon_strings[394])
#define GuiNiconMask ((char*)&gui_icon_strings[401])
#define GuiCIconMask ((char*)&gui_icon_strings[410])
#define GuiNshrinkOutline ((char*)&gui_icon_strings[419])
#define GuiCShrinkOutline ((char*)&gui_icon_strings[433])
#define GuiNfields ((char *)&gui_icon_strings[447])
#define GuiCFields ((char *)&gui_icon_strings[454])
#define GuiNstatePixmap ((char *)&gui_icon_strings[461])
#define GuiNstateIconMask ((char *)&gui_icon_strings[473])
#define GuiCStateIconMask ((char *)&gui_icon_strings[487])
#define GuiNstateGravity ((char *)&gui_icon_strings[501])
#define GuiCStateGravity ((char *)&gui_icon_strings[514])
#define GuiRStateGravity ((char *)&gui_icon_strings[514])

/* Public functions */

extern void GuiIconSetFieldNameWidth(
    Widget,			/* GuiIconWidget */
    Dimension name_width);

extern Dimension GuiIconGetFieldNameWidth(Widget);

extern void GuiIconSetField(
    Widget,			/* GuiIconWidget */
    int index,
    XmString new_string,	/* NULL does not change value */
    Dimension new_width,		/* 0 does not change value */
    unsigned char alignments,
    Boolean draw_field,
    Boolean selected,
    Boolean active);

extern void GuiIconGetField(
    Widget,
    int index,
    XmString *string,
    Dimension *width,
    unsigned char *alignments,
    Boolean *draw_field,
    Boolean *selected,
    Boolean *active);

extern void GuiIconGetRects(
    Widget,		/* GuiIconWidget */
    XRectangle *,	/* Pixmap Rectangle Return */
    XRectangle *	/* Label Rectangle Return */
    );

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _Icon_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
