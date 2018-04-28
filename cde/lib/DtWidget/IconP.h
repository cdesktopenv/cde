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
/* $XConsortium: IconP.h /main/7 1996/03/29 11:33:23 drk $ */
/**---------------------------------------------------------------------
***	
***	file:		IconP.h
***
***	project:	MotifPlus Widgets
***
***	description:	Private include file for DtIcon gadget class.
***	
***	
***			(c) Copyright 1990 by Hewlett-Packard Company.
***
***
***-------------------------------------------------------------------*/


#ifndef _DtIconP_h
#define _DtIconP_h

#include <Xm/ExtObjectP.h>
#include <Xm/XmP.h>
#include <Xm/GadgetP.h>
#include <Dt/Icon.h>


/*-------------------------------------------------------------
**	Cache Class Structure
*/

/*	Cache Class Part
*/
typedef struct _DtIconCacheObjClassPart
{
    int foo;
} DtIconCacheObjClassPart;

/*	Cache Full Class Record
*/
typedef struct _DtIconCacheObjClassRec     /* label cache class record */
{
    ObjectClassPart                     object_class;
    XmExtClassPart                      ext_class;
    DtIconCacheObjClassPart            icon_class_cache;
} DtIconCacheObjClassRec;

/*	Cache Actual Class
*/
externalref DtIconCacheObjClassRec dtIconCacheObjClassRec;


/*-------------------------------------------------------------
**	Cache Instance Structure
*/

/*	Cache Instance Part
*/
typedef struct _DtIconCacheObjPart
{
	Dimension	margin_width;
	Dimension	margin_height;
	Dimension	string_height;
	Dimension	spacing;
	Pixel		foreground;
	Pixel		background;
	Pixel		arm_color;
	Boolean		fill_on_arm;
	Boolean		recompute_size;
	Boolean		draw_shadow;
	unsigned char	pixmap_position;
	unsigned char	string_position;
	unsigned char	alignment;
	unsigned char	behavior;
	unsigned char	fill_mode;
} DtIconCacheObjPart;

typedef struct _DtIconCacheObjRec
{
    ObjectPart               object;
    XmExtPart                ext;
    DtIconCacheObjPart      icon_cache;
} DtIconCacheObjRec;


typedef void (*GetPositionProc)(
	DtIconGadget,
	Position,
	Position,
	Dimension,
	Dimension,
	Position *,
	Position *,
	Position *,
	Position *
);
typedef void (*GetSizeProc)(
	DtIconGadget,
	Dimension *,
	Dimension *
);
typedef void (*DrawProc)(
	DtIconGadget,
	Drawable,
	Position,
	Position,
	Dimension,
	Dimension,
	Dimension,
	Dimension,
	unsigned char,
	unsigned char
);
typedef void (*CallCallbackProc)(
	DtIconGadget,
	XtCallbackList,
	int,
	XEvent *
);
typedef void (*UpdateGCsProc)(
	DtIconGadget
);
/*-------------------------------------------------------------
**	Class Structure
*/

/*	Class Part
*/
typedef struct _DtIconClassPart
{
	GetSizeProc		get_size;
	GetPositionProc		get_positions;
	DrawProc		draw;
	CallCallbackProc	call_callback;
	UpdateGCsProc		update_gcs;
	Boolean			optimize_redraw;
	XmCacheClassPartPtr	cache_part;
	caddr_t			extension;
} DtIconClassPart;

/*	Full Class Record
*/
typedef struct _DtIconClassRec
{
	RectObjClassPart	rect_class;
	XmGadgetClassPart	gadget_class;
	DtIconClassPart	icon_class;
} DtIconClassRec;

/*	Actual Class
*/
externalref DtIconClassRec dtIconClassRec;


/*-------------------------------------------------------------
**	Instance Structure
*/

/*	Instance Part
*/
typedef struct _DtIconPart
{
	Boolean		set;
	Boolean		armed;
	Boolean		sync;
	Boolean		underline;
	unsigned char	shadow_type;
	unsigned char	border_type;
	XtCallbackList	callback;
	XtIntervalId	click_timer_id;
	XButtonEvent *	click_event;
	String		image_name;
	Pixmap		pixmap;
	Pixmap		mask;
	Pixel		pixmap_foreground;
	Pixel		pixmap_background;
	Dimension	max_pixmap_width;
	Dimension	max_pixmap_height;
	XmFontList	font_list;
	XmString	string;
	Dimension	string_width;
	Dimension	pixmap_width;
	Dimension	pixmap_height;
	GC		clip_gc;
	GC		normal_gc;
	GC		background_gc;
	GC		armed_gc;
	GC		armed_background_gc;
	GC		parent_background_gc;
	Pixel		saved_parent_background;
	DtIconCacheObjPart *cache;
	unsigned char	operations;
	XtCallbackList	drop_callback;
} DtIconPart;

/*	Full Instance Record
*/
typedef struct _DtIconRec
{
	ObjectPart	object;
	RectObjPart	rectangle;
	XmGadgetPart	gadget;
	DtIconPart	icon;
} DtIconRec;


/*-------------------------------------------------------------
**	Class and Instance Macros
*/

/*	DtIcon Class Macros
*/	
#define DtInheritGetSize	((GetSizeProc) _XtInherit)
#define DtInheritGetPositions	((GetPositionProc) _XtInherit)
#define DtInheritDraw		((DrawProc) _XtInherit)
#define DtInheritCallCallback	((CallCallbackProc) _XtInherit)
#define DtInheritUpdateGCs	((UpdateGCsProc) _XtInherit)

/*** WARNING: These macros are not thread-safe! ***/
#define C_GetSize(wc)		\
	(((DtIconGadgetClass)(wc)) -> icon_class.get_size)
#define C_GetPositions(wc)	\
	(((DtIconGadgetClass)(wc)) -> icon_class.get_positions)
#define C_Draw(wc)		\
	(((DtIconGadgetClass)(wc)) -> icon_class.draw)
#define C_CallCallback(wc)	\
	(((DtIconGadgetClass)(wc)) -> icon_class.call_callback)
#define C_OptimizeRedraw(wc)	\
	(((DtIconGadgetClass)(wc)) -> icon_class.optimize_redraw)
#define C_UpdateGCs(wc)		\
	(((DtIconGadgetClass)(wc)) -> icon_class.update_gcs)

/*	DtIcon Macros
*/

/*** WARNING: These macros are not thread-safe! ***/
#define G_GetSize(g,w,h) \
  (((DtIconClassRec *)g -> object.widget_class) -> icon_class.get_size) \
	(g,w,h)
#define G_GetPositions(g,w,h,h_t,s_t,p_x,p_y,s_x,s_y) \
  (((DtIconClassRec *)g -> object.widget_class) -> icon_class.get_positions) \
	(g,w,h,h_t,s_t,p_x,p_y,s_x,s_y)
#define G_Draw(g,d,x,y,w,h,h_t,s_t,s_type,fill) \
  (((DtIconClassRec *)g -> object.widget_class) -> icon_class.draw) \
	(g,d,x,y,w,h,h_t,s_t,s_type,fill)
#define G_CallCallback(g,cb,r,e) \
  (((DtIconClassRec *)g -> object.widget_class) -> icon_class.call_callback) \
	(g,cb,r,e)
#define G_UpdateGCs(g) \
  (((DtIconClassRec *)g -> object.widget_class) -> icon_class.update_gcs) \
	(g)

/*	Cached Instance Field Macros
*/
#define G_CachePixmapPosition(co)	(((DtIconCacheObject)(co)) -> \
				  	icon_cache.pixmap_position)
#define G_FillOnArm(g)		(((DtIconGadget)(g)) -> \
				  icon.cache -> fill_on_arm)
#define G_RecomputeSize(g)	(((DtIconGadget)(g)) -> \
				  icon.cache -> recompute_size)
#define G_DrawShadow(g)		(((DtIconGadget)(g)) -> \
				  icon.cache -> draw_shadow)
#define G_PixmapPosition(g)	(((DtIconGadget)(g)) -> \
				  icon.cache -> pixmap_position)
#define G_StringPosition(g)	(((DtIconGadget)(g)) -> \
				  icon.cache -> string_position)
#define G_Alignment(g)		(((DtIconGadget)(g)) -> \
				  icon.cache -> alignment)
#define G_Behavior(g)		(((DtIconGadget)(g)) -> \
				  icon.cache -> behavior)
#define G_FillMode(g)		(((DtIconGadget)(g)) -> \
				  icon.cache -> fill_mode)
#define G_MarginWidth(g)	(((DtIconGadget)(g)) -> \
				  icon.cache -> margin_width)
#define G_MarginHeight(g)	(((DtIconGadget)(g)) -> \
				  icon.cache -> margin_height)
#define G_StringHeight(g)	(((DtIconGadget)(g)) -> \
				  icon.cache -> string_height)
#define G_Spacing(g)		(((DtIconGadget)(g)) -> \
				  icon.cache -> spacing)
#define G_Foreground(g)		(((DtIconGadget)(g)) -> \
				  icon.cache -> foreground)
#define G_Background(g)		(((DtIconGadget)(g)) -> \
				  icon.cache -> background)
#define G_ArmColor(g)		(((DtIconGadget)(g)) -> \
				  icon.cache -> arm_color)

/*	Non-Cached Instance Field Macros
*/
#define G_Armed(g)		(g -> icon.armed)
#define G_Set(g)		(g -> icon.set)
#define G_Sync(g)		(g -> icon.sync)
#define G_Callback(g)		(g -> icon.callback)
#define G_ClickTimerID(g)	(g -> icon.click_timer_id)
#define G_ClickInterval(g)	(g -> icon.click_interval)
#define G_ClickEvent(g)		(g -> icon.click_event)
#define G_ShadowType(g)		(g -> icon.shadow_type)
#define G_BorderType(g)		(g -> icon.border_type)
#define G_Pixmap(g)		(g -> icon.pixmap)
#define G_Mask(g)		(g -> icon.mask)
#define G_PixmapForeground(g)	(g -> icon.pixmap_foreground)
#define G_PixmapBackground(g)	(g -> icon.pixmap_background)
#define G_MaxPixmapWidth(g)	(g -> icon.max_pixmap_width)
#define G_MaxPixmapHeight(g)	(g -> icon.max_pixmap_height)
#define G_String(g)		(g -> icon.string)
#define G_FontList(g)		(g -> icon.font_list)
#define G_ImageName(g)		(g -> icon.image_name)
#define G_StringWidth(g)	(g -> icon.string_width)
#define G_PixmapWidth(g)	(g -> icon.pixmap_width)
#define G_PixmapHeight(g)	(g -> icon.pixmap_height)
#define G_BackgroundGC(g)	(g -> icon.background_gc)
#define G_ArmedGC(g)		(g -> icon.armed_gc)
#define G_ArmedBackgroundGC(g)	(g -> icon.armed_background_gc)
#define G_NormalGC(g)		(g -> icon.normal_gc)
#define G_ClipGC(g)		(g -> icon.clip_gc)
#define G_Underline(g)		(g -> icon.underline)
#define G_ParentBackgroundGC(g)	(g -> icon.parent_background_gc)
#define G_SavedParentBG(g)	(g -> icon.saved_parent_background)
#define G_Operations(g)		(g -> icon.operations)
#define G_DropCallback(g)	(g -> icon.drop_callback)

#define Icon_Cache(w)                   (((DtIconGadget)(w))-> \
                                           icon.cache)

#define Icon_ClassCachePart(w) \
        (((DtIconGadgetClass)dtIconGadgetClass)->gadget_class.cache_part)


#endif /* _DtIconP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
