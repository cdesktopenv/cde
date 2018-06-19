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
/******************************************************************************
*******************************************************************************
*
*  (c) Copyright 1992 HEWLETT-PACKARD COMPANY
*  ALL RIGHTS RESERVED
*
*******************************************************************************
******************************************************************************/
#ifndef _DtButtonP_h
#define _DtButtonP_h

#include <Xm/XmP.h>
#include "Button.h"
#include <Xm/GadgetP.h>
#include <Xm/ManagerP.h>

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*  Arrow class structure  */

typedef struct _DtButtonGadgetClassPart
{
   caddr_t extension;
} DtButtonGadgetClassPart;


/*  Full class record declaration for Arrow class  */

typedef struct _DtButtonGadgetClassRec
{
   RectObjClassPart             rect_class;
   XmGadgetClassPart      	gadget_class;
   DtButtonGadgetClassPart   	button_class;
} DtButtonGadgetClassRec;

externalref DtButtonGadgetClassRec dtButtonGadgetClassRec;


/*  The button instance record  */

typedef struct _DtButtonGadgetPart
{
   XtCallbackList callback;

   Boolean armed;

   GC      gc_normal;
   GC      gc_clip;
   GC      gc_background;
   GC      gc_armed_bg;

   XtIntervalId     timer;	
   unsigned char    multiClick;         /* KEEP/DISCARD resource */
   int              click_count;
   Time		    armTimeStamp;
   Time		    activateTimeStamp;

	Widget		subpanel;
	XtPointer	push_function;
	XtPointer	push_argument;

	String		image_name;
	Pixmap		pixmap;
	Pixmap		mask;
	Dimension	pixmap_width;
	Dimension	pixmap_height;

	int		cursor_font;
	Pixel		arm_color;
} DtButtonGadgetPart;


/*  Full instance record declaration  */

typedef struct _DtButtonGadgetRec
{
   ObjectPart            object;
   RectObjPart           rectangle;
   XmGadgetPart          gadget;
   DtButtonGadgetPart    button;
} DtButtonGadgetRec;

#define B_Expose(w,e,r) \
	(w -> core.widget_class->core_class.expose)(w,e,r)
#define B_ArmColor(w)		(w -> button.arm_color)
#define B_CursorFont(w)		(w -> button.cursor_font)
#define B_Armed(w)		(w -> button.armed)
#define B_PushFunction(w)	(w -> button.push_function)
#define B_PushArgument(w)	(w -> button.push_argument)
#define B_Subpanel(w)		(w -> button.subpanel)
#define B_Callback(w)		(w -> button.callback)
#define B_PixmapWidth(w)	(w -> button.pixmap_width)
#define B_PixmapHeight(w)	(w -> button.pixmap_height)
#define B_ImageName(w)		(w -> button.image_name)
#define B_Mask(w)		(w -> button.mask)
#define B_Pixmap(w)		(w -> button.pixmap)
#define M_TopShadowColor(w)	(w -> manager.top_shadow_color)
#define M_BottomShadowColor(w)	(w -> manager.bottom_shadow_color)

#if defined(__cplusplus) || defined(c_plusplus)
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _DtButtonP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
