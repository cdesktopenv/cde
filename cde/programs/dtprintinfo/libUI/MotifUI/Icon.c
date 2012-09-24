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
/* $XConsortium: Icon.c /main/10 1996/11/20 15:35:35 drk $ */
#include <stdio.h>

#include "IconP.h"

#include <X11/Intrinsic.h>
#include <Xm/RepType.h>

/* Copied from Xm/RegionI.h */
extern XmRegion _XmRegionCreate( void ) ;
extern void _XmRegionDestroy( 
                        XmRegion r) ;
extern void _XmRegionClear(
			XmRegion r ) ;
extern void _XmRegionUnionRectWithRegion( 
                        XRectangle *rect,
                        XmRegion source,
                        XmRegion dest) ;
extern void _XmRegionDrawShadow(
			Display	*display,
			Drawable d,
			GC top_gc,
			GC bottom_gc,
			XmRegion region,
#if NeedWidePrototypes
			int border_thick,
			int shadow_thick,
#else
			Dimension border_thick,
			Dimension shadow_thick,
#endif /* NeedWidePrototypes */
			unsigned int shadow_type ) ;
extern Boolean _XmRegionPointInRegion( 
                        XmRegion pRegion,
                        int x,
                        int y) ;

/* Resource Strings */
 
const char gui_icon_strings[] =
{
't','o','p','L','a','b','e','l','S','t','r','i','n','g',0,
'b','o','t','t','o','m','L','a','b','e','l','S','t','r','i','n','g',0,
'p','i','x','m','a','p','P','l','a','c','e','m','e','n','t',0,
'P','i','x','m','a','p','P','l','a','c','e','m','e','n','t',0,
'P','i','x','m','a','p','P','l','a','c','e','m','e','n','t',0,
'd','o','u','b','l','e','C','l','i','c','k','C','a','l','l','b','a','c','k',0,
's','i','n','g','l','e','C','l','i','c','k','C','a','l','l','b','a','c','k',0,
's','e','l','e','c','t','e','d',0,
'S','e','l','e','c','t','e','d',0,
'i','c','o','n','S','h','a','d','o','w','T','y','p','e',0,
'I','c','o','n','S','h','a','d','o','w','T','y','p','e',0,
'i','c','o','n','S','h','a','d','o','w','T','h','i','c','k','n','e','s','s',0,
'I','c','o','n','S','h','a','d','o','w','T','h','i','c','k','n','e','s','s',0,
't','e','x','t','S','e','l','e','c','t','C','o','l','o','r',0,
'T','e','x','t','S','e','l','e','c','t','C','o','l','o','r',0,
's','e','l','e','c','t','C','o','l','o','r',0,
'S','e','l','e','c','t','C','o','l','o','r',0,
's','e','l','e','c','t','C','o','l','o','r','P','e','r','s','i','s','t','e','n','t',0,
'S','e','l','e','c','t','C','o','l','o','r','P','e','r','s','i','s','t','e','n','t',0,
's','h','o','w','S','e','l','e','c','t','e','d','P','i','x','m','a','p',0,
'S','h','o','w','S','e','l','e','c','t','e','d','P','i','x','m','a','p',0,
'i','c','o','n','M','a','r','g','i','n','T','h','i','c','k','n','e','s','s',0,
'I','c','o','n','M','a','r','g','i','n','T','h','i','c','k','n','e','s','s',0,
'a','c','t','i','v','e',0,
'A','c','t','i','v','e',0,
'i','c','o','n','M','a','s','k',0,
'I','c','o','n','M','a','s','k',0,
's','h','r','i','n','k','O','u','t','l','i','n','e',0,
'S','h','r','i','n','k','O','u','t','l','i','n','e',0,
'f','i','e','l','d','s',0,
'F','i','e','l','d','s',0,
's','t','a','t','e','P','i','x','m','a','p',0,
's','t','a','t','e','I','c','o','n','M','a','s','k',0,
'S','t','a','t','e','I','c','o','n','M','a','s','k',0,
's','t','a','t','e','G','r','a','v','i','t','y',0,
'S','t','a','t','e','G','r','a','v','i','t','y',0
};

/* Icon resources Macros */
#define NormalGC(w) (((IconWidget)(w))->icon.gc)
#define StippleGC(w) (((IconWidget)(w))->icon.stipple_gc)
#define SelectFGGC(w) (((IconWidget)(w))->icon.selected_fg_gc)
#define SelectBGGC(w) (((IconWidget)(w))->icon.selected_bg_gc)
#define MaskGC(w) (((IconWidget)(w))->icon.mask_gc)
#define StateMaskGC(w) (((IconWidget)(w))->icon.state_mask_gc)
#define MaskStippleGC(w) (((IconWidget)(w))->icon.mask_stipple_gc)
#define SelectColor(w) (((IconWidget)(w))->icon.select_color)
#define SelectColorPersistent(w) \
    (((IconWidget)(w))->icon.select_color_persistent)
#define SelectLabelColor(w) (((IconWidget)(w))->icon.select_label_color)
#define Mask(w) (((IconWidget)(w))->icon.mask)
#define Font(w) (((IconWidget)(w))->icon.font)
#define MarginThickness(w) (((IconWidget)(w))->icon.icon_margin_thickness)
#define StatePixmap(w) (((IconWidget)(w))->icon.state_pixmap)
#define StateMask(w) (((IconWidget)(w))->icon.state_mask)
#define StateGravity(w) (((IconWidget)(w))->icon.state_gravity)
#define StatePixmapX(w) (((IconWidget)(w))->icon.state_pixmap_x)
#define StatePixmapY(w) (((IconWidget)(w))->icon.state_pixmap_y)
#define StatePixmapWidth(w) (((IconWidget)(w))->icon.state_pixmap_width)
#define StatePixmapHeight(w) (((IconWidget)(w))->icon.state_pixmap_height)
#define Pixmap(w) (((IconWidget)(w))->icon.pixmap)
#define PixmapX(w) (((IconWidget)(w))->icon.pixmap_x)
#define PixmapY(w) (((IconWidget)(w))->icon.pixmap_y)
#define PixmapWidth(w) (((IconWidget)(w))->icon.pixmap_width)
#define PixmapHeight(w) (((IconWidget)(w))->icon.pixmap_height)
#define Label(w) (((IconWidget)(w))->icon.label)
#define TopLabel(w) (((IconWidget)(w))->icon.top_label)
#define BottomLabel(w) (((IconWidget)(w))->icon.bottom_label)
#define LabelX(w) (((IconWidget)(w))->icon.label_x)
#define LabelY(w) (((IconWidget)(w))->icon.label_y)
#define LabelWidth(w) (((IconWidget)(w))->icon.label_width)
#define LabelHeight(w) (((IconWidget)(w))->icon.label_height)
#define TopLabelX(w) (((IconWidget)(w))->icon.top_label_x)
#define TopLabelY(w) (((IconWidget)(w))->icon.top_label_y)
#define TopLabelWidth(w) (((IconWidget)(w))->icon.top_label_width)
#define TopLabelHeight(w) (((IconWidget)(w))->icon.top_label_height)
#define BottomLabelX(w) (((IconWidget)(w))->icon.bottom_label_x)
#define BottomLabelY(w) (((IconWidget)(w))->icon.bottom_label_y)
#define BottomLabelWidth(w) (((IconWidget)(w))->icon.bottom_label_width)
#define BottomLabelHeight(w) (((IconWidget)(w))->icon.bottom_label_height)
#define ResizeHeight(w) (((IconWidget)(w))->icon.resize_height)
#define ResizeWidth(w) (((IconWidget)(w))->icon.resize_width)
#define Fields(w) (((IconWidget)(w))->icon.fields)

#define Alignment(w) (((IconWidget)(w))->icon.alignment)
#define StringDirection(w) (((IconWidget)(w))->icon.string_direction)
#define IconPlacement(w) (((IconWidget)(w))->icon.icon_placement)
#define Selected(w) (((IconWidget)(w))->icon.selected)
#define ShowSelectedPixmap(w) (((IconWidget)(w))->icon.show_selected_pixmap)
#define Active(w) (((IconWidget)(w))->icon.active)
#define ShrinkOutline(w) (((IconWidget)(w))->icon.shrink_outline)
#define OldShrinkOutline(w) (((IconWidget)(w))->icon.old_shrink_outline)
#define IconShadowType(w) (((IconWidget)(w))->icon.icon_shadow_type)
#define IconShadowThickness(w) (((IconWidget)(w))->icon.icon_shadow_thickness)
#define ShadowRegion(w) (((IconWidget)(w))->icon.shadow_region)
#define HighlightRegion(w) (((IconWidget)(w))->icon.highlight_region)

/* XmPrimitive resources Macros */
#define HighlightThickness(w) (((IconWidget)(w))->primitive.highlight_thickness)
#define TopShadowGC(w) (((IconWidget)(w))->primitive.top_shadow_GC)
#define BottomShadowGC(w) (((IconWidget)(w))->primitive.bottom_shadow_GC)
#define HighlightColor(w) (((IconWidget)(w))->primitive.highlight_color)
#define Foreground(w) (((IconWidget)(w))->primitive.foreground)
#define Highlighted(w) (((IconWidget)(w))->primitive.highlighted)

/* Core resources Macros */
#define BackgroundPixel(w) (((IconWidget)(w))->core.background_pixel)
#define Width(w) (((IconWidget)(w))->core.width)
#define Height(w) (((IconWidget)(w))->core.height)
#define BorderWidth(w) (((IconWidget)w)->core.border_width)


static void ClassInitialize(void);
static void GetLabelString(Widget, int, XtArgVal *);
static void GetTopLabelString(Widget, int, XtArgVal *);
static void GetBottomLabelString(Widget, int, XtArgVal *);
static void HighlightBorder(Widget);
static void UnhighlightBorder(Widget);
static void Initialize(IconWidget, IconWidget, ArgList, Cardinal *);
static void Redisplay(IconWidget, XEvent *, Region);
static void Resize(IconWidget);
static Dimension CalculateSize(IconWidget, Boolean);
static void CalculateNewField(Widget w, Dimension name_width,
   int index, Dimension field_width);
static void Destroy(IconWidget);
static Boolean SetValues(Widget, Widget, IconWidget, ArgList, Cardinal *);
static void CreateGC(IconWidget);
static void CreateMaskGC(IconWidget);
static void CreateStateMaskGC(IconWidget);
static void SetRegions(Widget, int, int, int, int);
static XtGeometryResult QueryGeometry(IconWidget, XtWidgetGeometry *,
    XtWidgetGeometry *);
static void SingleClickCB(Widget, XEvent *, String *, Cardinal *);
static void DoubleClickCB(Widget, XEvent *, String *, Cardinal *);
static void ForegroundColorDefault(Widget, int, XrmValue *);
static void SelectColorDefault(Widget, int, XrmValue *);

static char defaultTranslations[] =
"<Btn1Down>:           SingleClickCB()\n\
    <Key>osfActivate:     SingleClickCB()\n\
    <Key>osfSelect:       SingleClickCB()\n\
    <Key>space:           SingleClickCB()\n\
    <Btn1Down>(2+):       DoubleClickCB()";

static XtActionsRec actionsList[] = 
{
    {
    "SingleClickCB", (XtActionProc) SingleClickCB
    },
    {
    "DoubleClickCB", (XtActionProc) DoubleClickCB
    },

};

static XmRepTypeId GuiRID_PIXMAP_PLACEMENT;
static char *PixmapPlacement[] = 
{
    "pixmap_top", "pixmap_bottom", "pixmap_left", "pixmap_right"
};

static XmRepTypeId GuiRID_STATE_PIXMAP_PLACEMENT;
static char *StatePixmapPlacement[] =
{
    "northwest_gravity", "north_gravity", "northeast_gravity", 
    "west_gravity", "center_gravity", "east_gravity", "southwest_gravity", 
    "south_gravity", "southeast_gravity"
};

static XtResource resources[] =
{
    {
    XmNlabelString, XmCXmString, XmRXmString, sizeof(XmString),
    XtOffsetOf(struct _IconRec, icon.label), XmRImmediate,
    (XtPointer) NULL
    },
    {
    GuiNtopLabelString, XmCXmString, XmRXmString, sizeof(XmString),
    XtOffsetOf(struct _IconRec, icon.top_label), XmRImmediate,
    (XtPointer) NULL
    },
    {
    GuiNbottomLabelString, XmCXmString, XmRXmString, sizeof(XmString),
    XtOffsetOf(struct _IconRec, icon.bottom_label), XmRImmediate,
    (XtPointer) NULL
    },
    {
    GuiNtextSelectColor, GuiCTextSelectColor, XmRPixel,
    sizeof(Pixel), XtOffsetOf(struct _IconRec, icon.select_label_color),
    XmRImmediate, (XtPointer) ForegroundColorDefault
    },
    {
    GuiNselectColor, GuiCSelectColor, XmRPixel,
    sizeof(Pixel), XtOffsetOf(struct _IconRec, icon.select_color),
    XmRImmediate, (XtPointer) SelectColorDefault
    },
    {
    XmNfontList, XmCFontList, XmRFontList, sizeof(XmFontList),
    XtOffsetOf(struct _IconRec, icon.font), XmRImmediate,
    (XtPointer) NULL
    },
    {
    XmNlabelPixmap, XmCLabelPixmap, XmRPrimForegroundPixmap,
    sizeof(Pixmap), XtOffsetOf(struct _IconRec, icon.pixmap),
    XmRImmediate, (XtPointer) XmUNSPECIFIED_PIXMAP
    },
    {
    GuiNiconMask, GuiCIconMask, XmRPrimForegroundPixmap,
    sizeof(Pixmap), XtOffsetOf(struct _IconRec, icon.mask),
    XmRImmediate, (XtPointer) XmUNSPECIFIED_PIXMAP
    },
    {
    GuiNstatePixmap, XmCLabelPixmap, XmRPrimForegroundPixmap,
    sizeof(Pixmap), XtOffsetOf(struct _IconRec, icon.state_pixmap),
    XmRImmediate, (XtPointer) XmUNSPECIFIED_PIXMAP
    },
    {
    GuiNstateIconMask, GuiCStateIconMask, XmRPrimForegroundPixmap,
    sizeof(Pixmap), XtOffsetOf(struct _IconRec, icon.state_mask),
    XmRImmediate, (XtPointer) XmUNSPECIFIED_PIXMAP
    },
    {
    GuiNstateGravity, GuiCStateGravity, GuiRStateGravity, sizeof(unsigned char),
    XtOffsetOf(struct _IconRec, icon.state_gravity), 
    XmRImmediate, (XtPointer) GuiSOUTHEAST_GRAVITY
    }, 
    {
    GuiNsingleClickCallback, XtCCallback, XtRCallback, sizeof(caddr_t),
    XtOffsetOf(struct _IconRec, icon.single_click_callback),
    XtRCallback, NULL
    },
    {
    GuiNdoubleClickCallback, XtCCallback, XtRCallback, sizeof(caddr_t),
    XtOffsetOf(struct _IconRec, icon.double_click_callback),
    XtRCallback, NULL
    },
    {
    GuiNiconShadowType, GuiCIconShadowType, XmRShadowType,
    sizeof(unsigned char),
    XtOffsetOf(struct _IconRec, icon.icon_shadow_type),
    XmRImmediate, (XtPointer) XmSHADOW_OUT
    },
    {
    GuiNiconShadowThickness, GuiCIconShadowThickness, XmRHorizontalDimension,
    sizeof(Dimension),
    XtOffsetOf(struct _IconRec, icon.icon_shadow_thickness),
    XmRImmediate, (XtPointer) 0
    },
    {
    XmNalignment, XmCAlignment, XmRAlignment, sizeof(unsigned char),
    XtOffsetOf(struct _IconRec,icon.alignment),
    XmRImmediate, (XtPointer) XmALIGNMENT_CENTER
    },
    {
    GuiNpixmapPlacement, GuiCPixmapPlacement, GuiRPixmapPlacement,
    sizeof(unsigned char), XtOffsetOf(struct _IconRec,icon.icon_placement),
    XmRImmediate, (XtPointer) GuiPIXMAP_TOP
    },
    {
    GuiNiconMarginThickness, GuiCIconMarginThickness, XmRHorizontalDimension,
    sizeof(Dimension),
    XtOffsetOf(struct _IconRec, icon.icon_margin_thickness),
    XmRImmediate, (XtPointer) 2
    },
    {
    GuiNselectColorPersistent, GuiCSelectColorPersistent, XmRBoolean,
    sizeof(Boolean),
    XtOffsetOf(struct _IconRec,icon.select_color_persistent),
    XmRImmediate, (XtPointer) False
    },
    {
    XmNresizeHeight, XmCResizeHeight, XmRBoolean, sizeof(Boolean),
    XtOffsetOf(struct _IconRec,icon.resize_height),
    XmRImmediate, (XtPointer) True
    },
    {
    XmNresizeWidth, XmCResizeWidth, XmRBoolean, sizeof(Boolean),
    XtOffsetOf(struct _IconRec,icon.resize_width),
    XmRImmediate, (XtPointer) True
    },
    {
    GuiNshowSelectedPixmap, GuiCShowSelectedPixmap, XmRBoolean,
    sizeof(Boolean), XtOffsetOf(struct _IconRec,icon.show_selected_pixmap),
    XmRImmediate, (XtPointer) False
    },
    {
    XmNwordWrap, XmCWordWrap, XmRBoolean, sizeof(Boolean),
    XtOffsetOf(struct _IconRec,icon.word_wrap),
    XmRImmediate, (XtPointer) False
    },
    {
    GuiNselected, GuiCSelected, XmRBoolean, sizeof(Boolean),
    XtOffsetOf(struct _IconRec,icon.selected),
    XmRImmediate, (XtPointer) False
    },
    {
    GuiNshrinkOutline, GuiCShrinkOutline, XmRBoolean, sizeof(Boolean),
    XtOffsetOf(struct _IconRec,icon.shrink_outline),
    XmRImmediate, (XtPointer) True
    },
    {
    GuiNactive, GuiCActive, XmRBoolean, sizeof(Boolean),
    XtOffsetOf(struct _IconRec,icon.active),
    XmRImmediate, (XtPointer) True
    },
    {
    XmNstringDirection, XmCStringDirection, XmRStringDirection,
    sizeof(unsigned char),
    XtOffsetOf(struct _IconRec,icon.string_direction), XmRImmediate,
    (XtPointer) XmSTRING_DIRECTION_DEFAULT
    },
    {
    XmNtraversalOn, XmCTraversalOn, XmRBoolean, sizeof(Boolean),
    XtOffsetOf(struct _XmPrimitiveRec, primitive.traversal_on),
    XmRImmediate, (XtPointer) True
    },
    {
    XmNhighlightThickness, XmCHighlightThickness, XmRHorizontalDimension,
    sizeof(Dimension),
    XtOffsetOf(struct _XmPrimitiveRec, primitive.highlight_thickness),
    XmRImmediate, (XtPointer) 2
    },
    {
    GuiNfields, GuiCFields, XmRPointer, sizeof(XtPointer),
    XtOffsetOf(struct _IconRec,icon.fields), XmRImmediate, (XtPointer) NULL
    }
};

static XmSyntheticResource syn_resources[] =
{
    {
    XmNlabelString, sizeof(XmString),
    XtOffsetOf(struct _IconRec, icon.label), GetLabelString, NULL
    },
    {
    GuiNtopLabelString, sizeof(XmString),
    XtOffsetOf(struct _IconRec, icon.top_label), GetTopLabelString, NULL
    },
    {
    GuiNbottomLabelString, sizeof(XmString),
    XtOffsetOf(struct _IconRec, icon.bottom_label), GetBottomLabelString, NULL
    }
};

IconClassRec iconClassRec = 
{
    {
    /* core_class fields    */
    (WidgetClass) &xmPrimitiveClassRec, /* superclass           */
    "Icon",                             /* class_name           */
    sizeof(IconRec),                    /* widget_size          */
    ClassInitialize,                    /* class_initialize     */
    NULL,                               /* class_part_initialize*/
    False,                              /* class_inited         */
    (XtInitProc)Initialize,             /* initialize           */
    NULL,                               /* initialize_notify    */
    XtInheritRealize,                   /* realize              */
    actionsList,                        /* actions              */
    XtNumber(actionsList),              /* num_actions          */
    resources,                          /* resources            */
    XtNumber(resources),                /* num_resources        */
    NULLQUARK,                          /* xrm_class            */
    True,                               /* compress_motion      */
    True,                               /* compress_exposure    */
    True,                               /* compress_enterleave  */
    False,                              /* visible_interest     */
    (XtWidgetProc)Destroy,              /* destroy              */
    (XtWidgetProc)Resize,               /* resize               */
    (XtExposeProc)Redisplay,            /* expose               */
    (XtSetValuesFunc)SetValues,         /* set_values           */
    NULL,                               /* set_values_hook      */
    XtInheritSetValuesAlmost,           /* set_values_almost    */
    NULL,                               /* get_values_hook      */
    NULL,                               /* accept_focus         */
    XtVersionDontCheck,                 /* version              */
    NULL,                               /* callback_private     */
    defaultTranslations,                /* tm_table             */
    (XtGeometryHandler)QueryGeometry,   /* query_geometry       */
    NULL,                               /* disp accelerator     */
    NULL                                /* extension            */
    },
    {
    /* primitive_class record */
    HighlightBorder,                    /* border_highlight     */
    UnhighlightBorder,                  /* border_unhighlight   */
    XtInheritTranslations,              /* translations         */
    NULL,                               /* arm_and_activate     */
    syn_resources,                      /* syn resources        */
    XtNumber(syn_resources),            /* num syn_resources    */
    NULL,                               /* extension            */
    },
    {
    /* icon_class record     */
    0,                                  /* extension            */
    }
};

externaldef(iconwidgetclass) WidgetClass iconWidgetClass =
   (WidgetClass) &iconClassRec;
   
/**********************************************************************
 *
 * Public functions
 *
 *********************************************************************/
   
/*
 *   NAME:     GuiIconSetFieldNameWidth
 *   FUNCTION:
 *   RETURNS:
 */
void
GuiIconSetFieldNameWidth(
    Widget w,
    Dimension name_width
    )
{
    GuiIconFields fields;
    if (!GuiIsIcon(w))
      {
        XmeWarning(w, "Cannot set field name width of non-subclass of GuiIcon");
        return;
      }
    if ((fields = Fields(w)) && name_width > 0)
	CalculateNewField(w, name_width, -1, 0);
}

/*
 *   NAME:     GuiIconGetFieldNameWidth
 *   FUNCTION:
 *   RETURNS:
 */
Dimension
GuiIconGetFieldNameWidth(
    Widget w
    )
{
    GuiIconFields fields;
    if (!GuiIsIcon(w))
      {
        XmeWarning(w, "Cannot get field name width of non-subclass of GuiIcon");
        return 0;
      }
    if (fields = Fields(w))
	return fields->name_width;
    else
	return 0;
}

/*
 *   NAME:     GuiIconSetField
 *   FUNCTION:
 *   RETURNS:
 */
void
GuiIconSetField(
    Widget w,                   /* GuiIconWidget */
    int index, 
    XmString new_string,        /* NULL does not change value */
    Dimension new_width,        /* 0 does not change value */
    unsigned char alignment,
    Boolean draw_field, 
    Boolean selected, 
    Boolean active)
{
    GuiIconFields fields;
    if (!GuiIsIcon(w))
      {
        XmeWarning(w, "Cannot set icon fields of non-subclass of GuiIcon");
        return;
      }
    if (fields = Fields(w))
      {
	if (index >= 0 && index < fields->n_fields)
	  {
	    Boolean refresh = False;

	    if (new_string)
	      {
		if (!XmStringCompare(new_string, fields->fields[index]))
		    refresh = True;
		XmStringFree(fields->fields[index]);
		fields->fields[index] = new_string;
	      }
	    if (XmRepTypeValidValue(XmRepTypeGetId(XmRAlignment), alignment, w))
	      {
		if (fields->alignments[index] != alignment)
		  {
		    fields->alignments[index] = alignment;
		    refresh = True;
		  }
	      }
	    if (fields->draw_fields)
		fields->draw_fields[index] = draw_field;
	    if (fields->selected)
	      {
		if (fields->selected[index] != selected)
		  {
		    fields->selected[index] = selected;
		    refresh = True;
		  }
	      }
	    if (fields->active)
	      {
		if (fields->active[index] != active)
		  {
		    fields->active[index] = active;
		    refresh = True;
		  }
	      }
	    CalculateNewField(w, 0, index, new_width);
	    if (XtIsRealized(w) && XtIsManaged(w) && refresh)
		XClearArea(XtDisplay(w), XtWindow(w), 0, 0, 0, 0, TRUE);
	  }
      }

}

/*
 *   NAME:     GuiIconGetField
 *   FUNCTION:
 *   RETURNS:
 */
void
GuiIconGetField(
    Widget w,
    int index, 
    XmString *string,
    Dimension *width,
    unsigned char *alignments, 
    Boolean *draw_field, 
    Boolean *selected, 
    Boolean *active)
{
    GuiIconFields fields;
    if (!GuiIsIcon(w))
      {
        XmeWarning(w, "Cannot get icon fields of non-subclass of GuiIcon");
        return;
      }
    if (fields = Fields(w))
      {
	if (index >= 0 && index < fields->n_fields)
	  {
	    *string = fields->fields[index];
	    *width = fields->widths[index];
	    *alignments = fields->alignments[index];
	    if (fields->draw_fields)
		*draw_field = fields->draw_fields[index];
	    if (fields->selected)
		*selected = fields->selected[index];
	    if (fields->active)
		*active = fields->active[index];
	  }
      }
}

/*
 *   NAME:     GuiIconGetRects
 *   FUNCTION:
 *   RETURNS:
 */
void
GuiIconGetRects(
   Widget      w,
   XRectangle  *pixmap_rect,
   XRectangle  *label_rect
   )
{
    if (!GuiIsIcon(w))
      {
        XmeWarning(w, "Cannot get icon rectangles of non-subclass of GuiIcon");
        return;
      }
    if (ShrinkOutline(w) == True)
      {
        pixmap_rect->x = (short) PixmapX(w);
        pixmap_rect->y = (short) PixmapY(w);
        pixmap_rect->width = (short) PixmapWidth(w);
        pixmap_rect->height = (short) PixmapHeight(w);
        label_rect->x = (short) LabelX(w);
        label_rect->y = (short) LabelY(w);
        label_rect->width = (short) LabelWidth(w);
        label_rect->height = (short) LabelHeight(w);
      }
    else
      {
        switch (IconPlacement(w))
          {
        case GuiPIXMAP_TOP:
        case GuiPIXMAP_BOTTOM:
            pixmap_rect->x = (LabelX(w) < PixmapX(w) ? LabelX(w) : PixmapX(w));
            pixmap_rect->y = (short) PixmapY(w);
            pixmap_rect->width = (LabelWidth(w) > PixmapWidth(w) ?
                LabelWidth(w) : PixmapWidth(w));
            pixmap_rect->height = (short) PixmapHeight(w);
            label_rect->x = pixmap_rect->x;
            label_rect->y = (short) LabelY(w);
            label_rect->width = pixmap_rect->width;
            label_rect->height = (short) LabelHeight(w);
            break;

        case GuiPIXMAP_LEFT:
        case GuiPIXMAP_RIGHT:
            pixmap_rect->x = (short) PixmapX(w);
            pixmap_rect->y = (LabelY(w) < PixmapY(w) ? LabelY(w) : PixmapY(w));
            pixmap_rect->width = (short) PixmapWidth(w);
            pixmap_rect->height = (LabelHeight(w) > PixmapHeight(w) ?
                LabelHeight(w) : PixmapHeight(w));
            label_rect->x = (short) LabelX(w);
            label_rect->y = pixmap_rect->y;
            label_rect->width = (short) LabelWidth(w);
            label_rect->height = pixmap_rect->height;
            break;
          }
      }
    pixmap_rect->x -= IconShadowThickness(w);
    pixmap_rect->y -= IconShadowThickness(w);
    pixmap_rect->width += 2 * IconShadowThickness(w);
    pixmap_rect->height += 2 * IconShadowThickness(w);
    label_rect->x -= IconShadowThickness(w);
    label_rect->y -= IconShadowThickness(w);
    label_rect->width += 2 * IconShadowThickness(w);
    label_rect->height += 2 * IconShadowThickness(w);
}

/**********************************************************************
 *
 * Class methods
 *
 *********************************************************************/

/*
 *   NAME:     ClassInitialize
 *   FUNCTION:
 *   RETURNS:
 */
static void
ClassInitialize(
   void
   )
{
    GuiRID_PIXMAP_PLACEMENT = XmRepTypeRegister(GuiRPixmapPlacement,
        PixmapPlacement, NULL, sizeof(PixmapPlacement) / sizeof(char *));
    GuiRID_STATE_PIXMAP_PLACEMENT = XmRepTypeRegister(GuiRStateGravity, 
	StatePixmapPlacement, NULL,sizeof(StatePixmapPlacement)/sizeof(char *));
}

/*
 *   NAME:     Initialize
 *   FUNCTION:
 *   RETURNS:
 */
static void
Initialize(
   IconWidget request,
   IconWidget new,
   ArgList args,
   Cardinal *num_args
   )
{
    unsigned char       stringDirection;
    Arg                 new_args[1];
    int                 n;

    StateMaskGC(new) = 0L;

    if (!XmRepTypeValidValue(XmRepTypeGetId(XmRAlignment), Alignment(new),
	    (Widget) new))
      {
        Alignment(new) = XmALIGNMENT_CENTER;
      }
    if (!XmRepTypeValidValue(XmRepTypeGetId(XmRShadowType), IconShadowType(new),
        (Widget) new))
      {
        IconShadowType(new) = XmSHADOW_OUT;
      }
    if (StringDirection(new) == XmSTRING_DIRECTION_DEFAULT)
      {
        if (XmIsManager(XtParent(new)))
          {
            n = 0;
            XtSetArg(new_args[n], XmNstringDirection, &stringDirection);
            n++;
            XtGetValues(XtParent(new), new_args, n);
            StringDirection(new) = stringDirection;
          }
        else
            StringDirection(new) = XmSTRING_DIRECTION_L_TO_R;
      }

    if(!XmRepTypeValidValue(XmRepTypeGetId(XmRStringDirection),
	StringDirection(new), (Widget) new))
      {
        StringDirection(new) = XmSTRING_DIRECTION_L_TO_R;
      }

    if (Font(new) == NULL)
      {
        XmFontList defaultFont;

        defaultFont = XmeGetDefaultRenderTable((Widget) new, XmLABEL_FONTLIST);
        Font(new) = XmFontListCopy(defaultFont);
      }
    else
        Font(new) = XmFontListCopy(Font(new));

    if (Pixmap(new) == XmUNSPECIFIED_PIXMAP && Label(new) == NULL)
      {
        XmString string;
        string = (XmString)XmeGetLocalizedString((char *)NULL, 
	    (Widget) new, XmNlabelString, new->core.name);
        Label(new) = XmStringCopy(string);
        XmStringFree (string);
      }
    else if (Label(new) != NULL)
      {
        if (XmeStringIsValid((XmString) Label(new)))
            Label(new) = XmStringCopy((XmString) Label(new));
        else
          {
            XmString string;
            string = (XmString)XmeGetLocalizedString((char *)NULL,
		(Widget) new, XmNlabelString, new->core.name);
            Label(new) = XmStringCopy(string);
            XmStringFree (string);
          }
      }
    if (TopLabel(new) && XmeStringIsValid((XmString) TopLabel(new)))
	TopLabel(new) = XmStringCopy((XmString) TopLabel(new));
    else
	TopLabel(new) = NULL;
    if (BottomLabel(new) && XmeStringIsValid((XmString) BottomLabel(new)))
	BottomLabel(new) = XmStringCopy((XmString) BottomLabel(new));
    else
	BottomLabel(new) = NULL;
    if (!(Selected(new) == True || Selected(new) == False))
        Selected(new) = False;

    if (!XmRepTypeValidValue(GuiRID_PIXMAP_PLACEMENT, IconPlacement(new),
        (Widget) new))
      {
        IconPlacement(new) = GuiPIXMAP_TOP;
      }
    if (!XmRepTypeValidValue(GuiRID_STATE_PIXMAP_PLACEMENT, StateGravity(new), 
	(Widget) new))
      {
        StateGravity(new) = GuiSOUTHWEST_GRAVITY;
      }
    if (!(ResizeHeight(new) == True || ResizeHeight(new) == False))
        ResizeHeight(new) = False;
    if (!(ResizeWidth(new) == True || ResizeWidth(new) == False))
        ResizeWidth(new) = False;
    if (!(Active(new) == True || Active(new) == False))
        Active(new) = True;
    if (!(SelectColorPersistent(new) == True ||
        SelectColorPersistent(new) == False))
      {
        SelectColorPersistent(new) = False;
      }
    if (SelectColorPersistent(new) == False)
        XmGetColors(new->core.screen, new->core.colormap,
            BackgroundPixel(new), NULL, NULL, NULL, &SelectColor(new));
    if (SelectLabelColor(new) == SelectColor(new))
        SelectLabelColor(new) = Foreground(new);
    new->icon.old_time = 0;
    CreateGC(new);
    ShadowRegion(new) = _XmRegionCreate();
    HighlightRegion(new) = _XmRegionCreate();
    OldShrinkOutline(new) = ShrinkOutline(new);
    Resize(new);
}

/*
 *   NAME:     CreateGC
 *   FUNCTION:
 *   RETURNS:
 */
static void
CreateGC(
   IconWidget w
   )
{
    XGCValues   values;
    XtGCMask    valueMask;
    XFontStruct *fs = (XFontStruct *) NULL;

    valueMask = GCForeground | GCBackground | GCFont | GCGraphicsExposures;
    values.foreground = Foreground(w);
    values.background = BackgroundPixel(w);
    values.graphics_exposures = False;
    if (XmeRenderTableGetDefaultFont(Font(w), &fs))
        values.font = fs->fid;
    else
        valueMask &= ~GCFont;

    /*****  Create Normal GC *****/
    NormalGC(w) = XtGetGC((Widget)w, valueMask, &values);

    /*****  Create Selected Background GC (bg for text and pixmap) *****/
    values.foreground = SelectColor(w);
    SelectBGGC(w) = XtGetGC((Widget)w, valueMask, &values);

    /*****  Create Selected Foreground GC (text foreground) *****/
    values.foreground = SelectLabelColor(w);
    SelectFGGC(w) = XtGetGC((Widget)w, valueMask, &values);

    /*****  Create Stippled GC (grey out text)  *****/
    values.foreground = SelectLabelColor(w);
    valueMask |= GCFillStyle | GCTile;
    values.fill_style = FillTiled;
    values.tile = XmGetPixmapByDepth(XtScreen((Widget)(w)),
        "50_foreground", BackgroundPixel(w), Foreground(w),
        w->core.depth);
    StippleGC(w) = XtGetGC((Widget)w, valueMask, &values);

    if (StatePixmap(w) != XmUNSPECIFIED_PIXMAP)
	CreateStateMaskGC(w);
    CreateMaskGC(w);
}

/*
 *   NAME:     CreateStateMaskGC
 *   FUNCTION:
 *   RETURNS:
 */
static void
CreateStateMaskGC(
    IconWidget w
    )
{
    XGCValues   values;
    XtGCMask    valueMask;

    /*****  Create State Mask GC (copy state pixmap) *****/
    valueMask = GCForeground | GCBackground | GCGraphicsExposures;
    values.foreground = Foreground(w);
    values.background = BackgroundPixel(w);
    values.graphics_exposures = False;
    if (StateMask(w) != XmUNSPECIFIED_PIXMAP)
      {
	values.clip_mask = StateMask(w);
	valueMask |= GCClipMask;
      }
    StateMaskGC(w) = XtGetGC((Widget)w, valueMask, &values);
}

/*
 *   NAME:     CreateMaskGC
 *   FUNCTION:
 *   RETURNS:
 */
static void
CreateMaskGC(
   IconWidget w
   )
{
    XGCValues   values;
    XtGCMask    valueMask;

    /*****  Create Normal Mask GC (copy normal pixmap) *****/
    valueMask = GCForeground | GCBackground | GCGraphicsExposures;
    values.foreground = Foreground(w);
    values.background = BackgroundPixel(w);
    values.graphics_exposures = False;
    if (Mask(w) != XmUNSPECIFIED_PIXMAP)
      {
        values.clip_mask = Mask(w);
        valueMask |= GCClipMask;
      }
    MaskGC(w) = XtGetGC((Widget)w, valueMask, &values);

    /*****  Create Masked Stippled GC (grey out pixmap) *****/
    valueMask &= ~GCTile;
    valueMask |= GCFillStyle | GCStipple;
    values.foreground = BackgroundPixel(w);
    values.background = Foreground(w);
    values.fill_style = FillStippled;
    values.stipple = XmGetPixmapByDepth(XtScreen((Widget)(w)),
        "50_foreground", BlackPixelOfScreen(XtScreen(w)),
        WhitePixelOfScreen(XtScreen(w)), 1);
    MaskStippleGC(w) = XtGetGC((Widget)w, valueMask, &values);
}

/*
 *   NAME:     Destroy
 *   FUNCTION:
 *   RETURNS:
 */
static void
Destroy(
   IconWidget w
   )
{
    if (Label(w))
        XmStringFree(Label(w));
    if (TopLabel(w))
        XmStringFree(TopLabel(w));
    if (BottomLabel(w))
        XmStringFree(BottomLabel(w));
    if (Font(w))
        XmFontListFree(Font(w));
    if (Fields(w))
      {
	int i;
	for (i = 0; i < Fields(w)->n_fields; i++)
	   XmStringFree(Fields(w)->fields[i]);
	free(Fields(w)->widths);
	free(Fields(w)->alignments);
	free(Fields(w)->fields);
	if (Fields(w)->draw_fields)
	    free(Fields(w)->draw_fields);
	if (Fields(w)->selected)
	    free(Fields(w)->selected);
	if (Fields(w)->active)
	    free(Fields(w)->active);
	free(Fields(w));
	Fields(w) = NULL;
      }
    XtReleaseGC((Widget)w, NormalGC(w));
    XtReleaseGC((Widget)w, StippleGC(w));
    XtReleaseGC((Widget)w, SelectFGGC(w));
    XtReleaseGC((Widget)w, SelectBGGC(w));
    if (StateMaskGC(w))
	XtReleaseGC((Widget)w, StateMaskGC(w));
    XtReleaseGC((Widget)w, MaskGC(w));
    XtReleaseGC((Widget)w, MaskStippleGC(w));
    _XmRegionDestroy(ShadowRegion(w));
    _XmRegionDestroy(HighlightRegion(w));
    XtRemoveAllCallbacks((Widget)w, GuiNdoubleClickCallback);
    XtRemoveAllCallbacks((Widget)w, GuiNsingleClickCallback);
}

/*
 *   NAME:     Resize
 *   FUNCTION:
 *   RETURNS:
 */
static void
Resize(
   IconWidget w
   )
{
    (void) CalculateSize(w, True);
}

/*
 *   NAME:     CalculateNewField
 *   FUNCTION:
 *   RETURNS:
 */
static void
CalculateNewField(
   Widget w, 
   Dimension name_width,
   int index,
   Dimension field_width
   )
{
    Dimension width;

    if (index >= 0)
      {
	if (field_width > 0)
	    Fields(w)->widths[index] = field_width;
      }
    else
      {
	if (name_width > 0)
	    Fields(w)->name_width = name_width;
      }
    if ((width = CalculateSize((IconWidget) w, False)) != Width(w))
      {
	Arg args[1];
	XtSetArg(args[0], XmNwidth, (int)width);
	XtSetValues(w, args, 1);
      }
}

/*
 *   NAME:     CalculateSize
 *   FUNCTION:
 *   RETURNS:
 */
static Dimension
CalculateSize(
   IconWidget w,
   Boolean query_geometry
   )
{
    unsigned int width, height, junk;
    Window junkwin;
    int x, y;
    Boolean show_fields = False;
    Dimension _width;

    if (query_geometry)
      {
	if (Pixmap(w) != XmUNSPECIFIED_PIXMAP)
	  {
            XGetGeometry(XtDisplay(w), Pixmap(w), &junkwin,
		(int *) &junk, (int *) &junk, &width, &height, &junk, &junk);
            PixmapWidth(w) = (Dimension) width;
            PixmapHeight(w) = (Dimension) height;
	  }
	else
	  {
            PixmapWidth(w) = 0;
            PixmapHeight(w) = 0;
	  }
      }
    if (Label(w) != NULL)
      {
        XmStringExtent(Font(w), Label(w), &LabelWidth(w), &LabelHeight(w));
        LabelWidth(w) += 4;
        LabelHeight(w) += 4;
      }
    else
      {
        LabelWidth(w) = 0;
        LabelHeight(w) = 0;
      }
    if (PixmapWidth(w) % 2)
	PixmapWidth(w) = PixmapWidth(w) + 1;
    if (LabelWidth(w) % 2)
	LabelWidth(w) = LabelWidth(w) + 1;

    if (IconPlacement(w) == GuiPIXMAP_TOP ||
        IconPlacement(w) == GuiPIXMAP_BOTTOM)
      {
        if (LabelWidth(w) < PixmapWidth(w))
          {
            OldShrinkOutline(w) = ShrinkOutline(w);
            ShrinkOutline(w) = False;
          }
        else
          {
            ShrinkOutline(w) = OldShrinkOutline(w);
          }
        if (ResizeWidth(w) == True)
            _width = Max(PixmapWidth(w), LabelWidth(w)) +
                2 * (IconShadowThickness(w) +
                MarginThickness(w) + HighlightThickness(w));
        if (ResizeHeight(w) == True)
            Height(w) = PixmapHeight(w) + LabelHeight(w) +
                2 * (MarginThickness(w) + HighlightThickness(w) +
                IconShadowThickness(w));
      }
    else if (IconPlacement(w) == GuiPIXMAP_LEFT ||
        IconPlacement(w) == GuiPIXMAP_RIGHT)
      {
        if (ResizeWidth(w) == True)
            _width = PixmapWidth(w) + LabelWidth(w) +
                2 * (MarginThickness(w) + HighlightThickness(w) +
                IconShadowThickness(w));
        if (ResizeHeight(w) == True)
            Height(w) = Max(PixmapHeight(w), LabelHeight(w)) +
                2 * (IconShadowThickness(w) +
                MarginThickness(w) + HighlightThickness(w));
      }

    switch (IconPlacement(w))
      {
    case GuiPIXMAP_TOP:
    case GuiPIXMAP_BOTTOM:
        if (IconPlacement(w) == GuiPIXMAP_TOP)
          {
            PixmapY(w) = MarginThickness(w) +
                IconShadowThickness(w) + HighlightThickness(w);
            LabelY(w) = PixmapHeight(w) + PixmapY(w);
          }
        else
          {
            LabelY(w) = MarginThickness(w) +
                IconShadowThickness(w) + HighlightThickness(w);
            PixmapY(w) = LabelHeight(w) + LabelY(w);
          }
        switch (Alignment(w))
          {
        case XmALIGNMENT_BEGINNING:
            LabelX(w) = MarginThickness(w) + IconShadowThickness(w) +
                HighlightThickness(w);
            PixmapX(w) = LabelX(w);
            break;
        case XmALIGNMENT_CENTER:
            PixmapX(w) = (Position)(_width - PixmapWidth(w)) / 2;
            LabelX(w) = (Position)(_width - LabelWidth(w)) / 2;
            break;
        case XmALIGNMENT_END:
            LabelX(w) = _width - (MarginThickness(w) +
                IconShadowThickness(w) + HighlightThickness(w) + LabelWidth(w));
            PixmapX(w) = _width - (MarginThickness(w) +
                IconShadowThickness(w) + HighlightThickness(w) +
                PixmapWidth(w));
            break;
          }
        break;

    case GuiPIXMAP_LEFT:
    case GuiPIXMAP_RIGHT:
        if (IconPlacement(w) == GuiPIXMAP_LEFT)
          {
            PixmapX(w) = MarginThickness(w) + IconShadowThickness(w) +
                HighlightThickness(w);
            LabelX(w) = PixmapX(w) + PixmapWidth(w);
          }
        else
          {
            LabelX(w) = MarginThickness(w) + IconShadowThickness(w) +
                HighlightThickness(w);
            PixmapX(w) = LabelX(w) + LabelWidth(w);
            if (PixmapX(w) % 2)
                PixmapX(w) = PixmapX(w) - 1;
          }
        LabelY(w) = (Position)(Height(w) - LabelHeight(w)) / 2;
        PixmapY(w) = (Position)(Height(w) - PixmapHeight(w)) / 2;
        break;
      }

    if (TopLabel(w) || BottomLabel(w))
      {
	int offset;
	Dimension max_width;

        if (TopLabel(w))
	  {
            XmStringExtent(Font(w), TopLabel(w), &TopLabelWidth(w),
	        &TopLabelHeight(w));
	    TopLabelWidth(w) += 4;
	  }
	else
	  {
	    TopLabelHeight(w) = 0;
	    TopLabelWidth(w) = 0;
	  }
        if (BottomLabel(w))
	  {
            XmStringExtent(Font(w), BottomLabel(w), &BottomLabelWidth(w),
	        &BottomLabelHeight(w));
	    BottomLabelWidth(w) += 4;
	  }
	else
	  {
	    BottomLabelHeight(w) = 0;
	    BottomLabelWidth(w) = 0;
	  }
        TopLabelY(w) = 0;
	Height(w) += TopLabelHeight(w) + BottomLabelHeight(w);
	BottomLabelY(w) = Height(w) - MarginThickness(w) -
	   IconShadowThickness(w) - HighlightThickness(w) -
	   BottomLabelHeight(w);
	LabelY(w) += TopLabelHeight(w);
	PixmapY(w) += TopLabelHeight(w);
        if (BottomLabelWidth(w) % 2)
	    BottomLabelWidth(w) = BottomLabelWidth(w) + 1;
        if (TopLabelWidth(w) % 2)
	    TopLabelWidth(w) = TopLabelWidth(w) + 1;
	max_width = Max(TopLabelWidth(w), BottomLabelWidth(w));
        offset = max_width - _width;
        if (offset > 0)
	    _width = max_width;
        switch (Alignment(w))
          {
        case XmALIGNMENT_BEGINNING:
            TopLabelX(w) = 0;
            BottomLabelX(w) = 0;
            break;
        case XmALIGNMENT_CENTER:
            TopLabelX(w) = (Position)(_width - TopLabelWidth(w)) / 2;
            BottomLabelX(w) = (Position)(_width - BottomLabelWidth(w)) / 2;
	    if (offset > 0)
	      {
		offset /= 2;
                PixmapX(w) += offset;
                LabelX(w) += offset;
	      }
            break;
        case XmALIGNMENT_END:
            TopLabelX(w) = _width - TopLabelWidth(w);
            BottomLabelX(w) = _width - BottomLabelWidth(w);
	    if (offset > 0)
	      {
                PixmapX(w) += offset;
                LabelX(w) += offset;
	      }
            break;
          }
      }
    else
      {
	BottomLabelHeight(w) = 0;
	BottomLabelWidth(w) = 0;
	TopLabelHeight(w) = 0;
	TopLabelWidth(w) = 0;
      }
    if (Fields(w))
     {
	int i;
	Boolean draw_field;

	if (!Fields(w)->draw_fields)
	    show_fields = True;
	else
	    for (i = 0; i < Fields(w)->n_fields; i++)
		if (Fields(w)->draw_fields[i])
		  {
		    show_fields = True;
		    break;
		  }
	if (show_fields)
	  {
            _width -= LabelWidth(w);
            _width += Fields(w)->name_width + 4;
	    for (i = 0; i < Fields(w)->n_fields; i++)
	      {
		if (Fields(w)->draw_fields)
		    draw_field = Fields(w)->draw_fields[i];
		else
		    draw_field = True;
		if (draw_field)
		    _width += Fields(w)->widths[i] + Fields(w)->field_spacing;
	      }
	  }
     }
    _XmRegionClear(ShadowRegion(w));
    _XmRegionClear(HighlightRegion(w));

    /* Create the rectangles sorted by y coordinate first,
       and then by x coordinate */
    if (ShrinkOutline(w) == True)
      {
        switch (IconPlacement(w))
          {
        case GuiPIXMAP_TOP:
        case GuiPIXMAP_LEFT:
	    SetRegions((Widget)w, PixmapX(w), PixmapY(w), PixmapWidth(w), 
		PixmapHeight(w));
	    if (show_fields)
	      {
		int wid = _width - LabelX(w) - 2 - (MarginThickness(w) +
		    HighlightThickness(w) + IconShadowThickness(w));
		SetRegions((Widget)w, LabelX(w), LabelY(w), 
		    wid, LabelHeight(w));
	      }
	    else
	      {
		SetRegions((Widget)w, LabelX(w), LabelY(w), 
		    LabelWidth(w), LabelHeight(w));
	      }
            break;

        case GuiPIXMAP_BOTTOM:
        case GuiPIXMAP_RIGHT:
	    SetRegions((Widget)w, LabelX(w), 
		LabelY(w), LabelWidth(w), LabelHeight(w));
	    SetRegions((Widget)w, PixmapX(w), PixmapY(w), PixmapWidth(w), 
		PixmapHeight(w));
            break;
          }
      }
    else
      {
        switch (IconPlacement(w))
          {
        case GuiPIXMAP_TOP:
        case GuiPIXMAP_BOTTOM:
            width = (LabelWidth(w) > PixmapWidth(w) ?
                LabelWidth(w) : PixmapWidth(w));
            x = (LabelX(w) < PixmapX(w) ? LabelX(w) : PixmapX(w));
	    SetRegions((Widget)w, x, PixmapY(w), (int) width, PixmapHeight(w));
	    SetRegions((Widget)w, x, LabelY(w), (int) width, LabelHeight(w));
            break;

        case GuiPIXMAP_RIGHT:
        case GuiPIXMAP_LEFT:
            height = (LabelHeight(w) > PixmapHeight(w) ?
                LabelHeight(w) : PixmapHeight(w));
            y = (LabelY(w) < PixmapY(w) ? LabelY(w) : PixmapY(w));
	    if (show_fields)
	      {
		int wid = _width - 2 - 2 * (MarginThickness(w) +
		    HighlightThickness(w) + IconShadowThickness(w));
		SetRegions((Widget)w, PixmapX(w), y, wid, (int) height);
	      }
	    else
	      {
		SetRegions((Widget)w, LabelX(w), y, LabelWidth(w),
		    (int) height);
		SetRegions((Widget)w, PixmapX(w), y, PixmapWidth(w),
		    (int) height);
	      }
            break;
          }
      }

    XSetClipOrigin(XtDisplay(w), MaskGC(w), PixmapX(w), PixmapY(w));
    XSetClipOrigin(XtDisplay(w), MaskStippleGC(w), PixmapX(w), PixmapY(w));
    if (StatePixmap(w) != XmUNSPECIFIED_PIXMAP)
      {
	Dimension x_offset, y_offset;

        XGetGeometry(XtDisplay(w), StatePixmap(w), &junkwin,
            (int *) &junk, (int *) &junk, &width, &height, &junk, &junk);
        StatePixmapWidth(w) = (Dimension) width;
        StatePixmapHeight(w) = (Dimension) height;
	switch(StateGravity(w))
	{
	case GuiNORTHWEST_GRAVITY:
	    x_offset = 0;
	    y_offset = 0;
	    break;
	case GuiNORTH_GRAVITY:
	    x_offset = (Dimension)(int)(PixmapWidth(w) -
		StatePixmapWidth(w)) / 2;
	    y_offset = 0;
	    break;
	case GuiNORTHEAST_GRAVITY:
	    x_offset = (PixmapWidth(w) - StatePixmapWidth(w));
	    y_offset = 0;
	    break;
	case GuiWEST_GRAVITY:
	    x_offset = 0;
	    y_offset = (Dimension)((int)(PixmapHeight(w) -
		StatePixmapHeight(w)) / 2);
	    break;
	case GuiCENTER_GRAVITY:
	    x_offset = (Dimension)((int)(PixmapWidth(w) -
		StatePixmapWidth(w)) / 2);
	    y_offset = (Dimension)((int)(PixmapHeight(w) -
		StatePixmapHeight(w)) / 2);
	    break;
	case GuiEAST_GRAVITY:
	    x_offset = (PixmapWidth(w) - StatePixmapWidth(w));
	    y_offset = (Dimension)((int)(PixmapHeight(w) -
		StatePixmapHeight(w)) / 2);
	    break;
	case GuiSOUTHWEST_GRAVITY:
	    x_offset = 0;
	    y_offset = (PixmapHeight(w) - StatePixmapHeight(w));
	    break;
	case GuiSOUTH_GRAVITY:
	    x_offset = (Dimension)((int)(PixmapWidth(w) -
		StatePixmapWidth(w)) / 2);
	    y_offset = (PixmapHeight(w) - StatePixmapHeight(w));
	    break;
	case GuiSOUTHEAST_GRAVITY:
	    x_offset = (PixmapWidth(w) - StatePixmapWidth(w));
	    y_offset = (PixmapHeight(w) - StatePixmapHeight(w));
	    break;
	}
	StatePixmapX(w) = PixmapX(w) + x_offset;
	StatePixmapY(w) = PixmapY(w) + y_offset;
	XSetClipOrigin(XtDisplay(w), StateMaskGC(w), StatePixmapX(w),
	    StatePixmapY(w));
      }
   if (query_geometry)
	Width(w) = _width;
   return _width;
}

/*
 *   NAME:     SetRegions
 *   FUNCTION:
 *   RETURNS:
 */
static void
SetRegions(
   Widget      w,
   int         x,
   int         y,
   int         width,
   int         height
   )
{
    XRectangle rect;

    rect.x = x - IconShadowThickness(w);
    rect.y = y - IconShadowThickness(w);
    rect.width = width - 1 + 2 * IconShadowThickness(w);
    rect.height = height - 1 + 2 * IconShadowThickness(w);
    _XmRegionUnionRectWithRegion(&rect, ShadowRegion(w), ShadowRegion(w));
    rect.x -= (MarginThickness(w) + HighlightThickness(w));
    rect.y -= (MarginThickness(w) + HighlightThickness(w));
    rect.width += 2 * (MarginThickness(w) + HighlightThickness(w));
    rect.height += 2 * (MarginThickness(w) + HighlightThickness(w));
    _XmRegionUnionRectWithRegion(&rect, HighlightRegion(w), HighlightRegion(w));
}

/*
 *   NAME:     Redisplay
 *   FUNCTION:
 *   RETURNS:
 */
static void
Redisplay(
   IconWidget w,
   XEvent *event,
   Region region
   )
{
    XRectangle  pixmap_rect;
    XRectangle  label_rect;
    GC		label_gc;

    if (XtIsRealized((Widget)w) && XtIsManaged((Widget)w))
      {
        if (ShrinkOutline(w) == False)
            GuiIconGetRects((Widget)w, &pixmap_rect, &label_rect);
	if (Selected(w) == True)
	    label_gc = SelectFGGC(w);
	else
	    label_gc = NormalGC(w);
        if ((w->core.sensitive) && (w->core.ancestor_sensitive))
          {
	    if (Fields(w))
	      {
		int i;
		Position x;
		Boolean draw_field;

                if (Selected(w) == True)
                  {
		    int wid = Width(w) - LabelX(w) - 2 - MarginThickness(w) -
			IconShadowThickness(w) - HighlightThickness(w);
                    if (ShrinkOutline(w) == True)
                      {
                        XFillRectangle(XtDisplay(w), XtWindow(w),
                            SelectBGGC(w), LabelX(w), LabelY(w),
                            wid, LabelHeight(w));
                      }
                    else
                      {
                        XFillRectangle(XtDisplay(w), XtWindow(w),
                            SelectBGGC(w), (int)label_rect.x, (int)label_rect.y,
                            wid, (int)label_rect.height);
                      }
                  }
		x = LabelX(w) + 2 + Fields(w)->name_width +
		    Fields(w)->field_spacing;
		for (i = 0; i < Fields(w)->n_fields; i++)
		  {
		    if (Fields(w)->draw_fields)
			draw_field = Fields(w)->draw_fields[i];
		    else
			draw_field = True;
		    if (draw_field)
		      {
			XmStringDraw(XtDisplay(w), XtWindow(w), Font(w),
			    Fields(w)->fields[i], label_gc, x, LabelY(w) + 2,
			    Fields(w)->widths[i], Fields(w)->alignments[i],
			    StringDirection(w), NULL);
		        x += Fields(w)->widths[i] + Fields(w)->field_spacing;
		      }
		  }
	      }
            if (Label(w) != NULL)
              {
                if (Selected(w) == True && !Fields(w))
                  {
                    if (ShrinkOutline(w) == True)
                      {
                        XFillRectangle(XtDisplay(w), XtWindow(w),
                            SelectBGGC(w), LabelX(w), LabelY(w),
                            LabelWidth(w), LabelHeight(w));
                      }
                    else
                      {
                        XFillRectangle(XtDisplay(w), XtWindow(w),
                            SelectBGGC(w), (int)label_rect.x, (int)label_rect.y,
                            (int)label_rect.width, (int)label_rect.height);
                      }
                  }
		XmStringDraw(XtDisplay(w), XtWindow(w), Font(w), Label(w),
		   label_gc, LabelX(w) + 2, LabelY(w) + 2, LabelWidth(w) - 4,
		   Alignment(w), StringDirection(w), NULL);
              }
            if (TopLabel(w) != NULL)
              {
		XmStringDraw(XtDisplay(w), XtWindow(w), Font(w), TopLabel(w),
		    NormalGC(w), TopLabelX(w) + 2, TopLabelY(w),
		    TopLabelWidth(w) - 4, Alignment(w),
		    StringDirection(w), NULL);
	      }
            if (BottomLabel(w) != NULL)
              {
		XmStringDraw(XtDisplay(w), XtWindow(w), Font(w),
		    BottomLabel(w), NormalGC(w),
		    BottomLabelX(w) + 2, BottomLabelY(w),
		    BottomLabelWidth(w) - 4, Alignment(w),
		    StringDirection(w), NULL);
	      }
            if (Pixmap(w) != XmUNSPECIFIED_PIXMAP)
              {
                if (ShowSelectedPixmap(w) == True && Selected(w) == True)
                  {
                    if (ShrinkOutline(w) == True)
                      {
                        XFillRectangle(XtDisplay(w), XtWindow(w),
                            SelectBGGC(w),
                            PixmapX(w), PixmapY(w),
                            PixmapWidth(w), PixmapHeight(w));
                      }
                    else
                      {
                        XFillRectangle(XtDisplay(w), XtWindow(w),
                            SelectBGGC(w),
                            (int)pixmap_rect.x, (int)pixmap_rect.y,
                            (int)pixmap_rect.width, (int)pixmap_rect.height);
                      }
                    XCopyArea(XtDisplay(w), Pixmap(w), XtWindow(w), MaskGC(w),
                        0, 0, PixmapWidth(w), PixmapHeight(w),
                        PixmapX(w), PixmapY(w));
                  }
                else
                  {
                    XCopyArea(XtDisplay(w), Pixmap(w), XtWindow(w),
                        MaskGC(w), 0, 0, PixmapWidth(w), PixmapHeight(w),
                        PixmapX(w), PixmapY(w));
                  }
                if (Active(w) == False)
                    XFillRectangle(XtDisplay(w), XtWindow(w),
                        MaskStippleGC(w),
                        PixmapX(w), PixmapY(w),
                        PixmapWidth(w), PixmapHeight(w));
              }
          }
        else /* Widget is insensitive */
          {
	    if (Fields(w))
	      {
		int i;
		Position x;
		Boolean draw_field;

                if (Selected(w) == True)
                  {
		    int wid = Width(w) - LabelX(w) - 2 - MarginThickness(w) -
			IconShadowThickness(w) - HighlightThickness(w);
                    if (ShrinkOutline(w) == True)
                      {
                        XFillRectangle(XtDisplay(w), XtWindow(w),
                            SelectBGGC(w), LabelX(w), LabelY(w),
                            wid, LabelHeight(w));
                      }
                    else
                      {
                        XFillRectangle(XtDisplay(w), XtWindow(w),
                            SelectBGGC(w), (int)label_rect.x, (int)label_rect.y,
                            wid, (int)label_rect.height);
                      }
                  }
		x = LabelX(w) + 2 + Fields(w)->name_width +
		    Fields(w)->field_spacing;
		for (i = 0; i < Fields(w)->n_fields; i++)
		  {
		    if (Fields(w)->draw_fields)
			draw_field = Fields(w)->draw_fields[i];
		    else
			draw_field = True;
		    if (draw_field)
		      {
			XmStringDraw(XtDisplay(w), XtWindow(w), Font(w),
			    Fields(w)->fields[i], StippleGC(w), x,
			    LabelY(w) + 2, Fields(w)->widths[i],
			    Fields(w)->alignments[i], StringDirection(w), NULL);
		        x += Fields(w)->widths[i] + Fields(w)->field_spacing;
		      }
		  }
	      }
            if (Label(w) != NULL)
              {
                if (Selected(w) == True && !Fields(w))
                  {
                    if (ShrinkOutline(w) == True)
                      {
                        XFillRectangle(XtDisplay(w), XtWindow(w), SelectBGGC(w),
                            LabelX(w), LabelY(w),
                            LabelWidth(w), LabelHeight(w));
                      }
                    else
                      {
                        XFillRectangle(XtDisplay(w), XtWindow(w), SelectBGGC(w),
                            (int)label_rect.x, (int)label_rect.y,
                            (int)label_rect.width, (int)label_rect.height);
                      }
                  }
                XmStringDraw(XtDisplay(w), XtWindow(w), Font(w), Label(w),
                    StippleGC(w), LabelX(w) + 2, LabelY(w) + 2,
                    LabelWidth(w) - 4, Alignment(w),
                    StringDirection(w), NULL);
              }
            if (TopLabel(w) != NULL)
              {
                XmStringDraw(XtDisplay(w), XtWindow(w), Font(w), TopLabel(w),
                    StippleGC(w), TopLabelX(w) + 2, TopLabelY(w),
                    TopLabelWidth(w) - 4, Alignment(w),
                    StringDirection(w), NULL);
              }
            if (BottomLabel(w) != NULL)
              {
                XmStringDraw(XtDisplay(w), XtWindow(w), Font(w),
		    BottomLabel(w), StippleGC(w),
                    BottomLabelX(w) + 2, BottomLabelY(w),
                    BottomLabelWidth(w) - 4, Alignment(w),
                    StringDirection(w), NULL);
              }
            if (Pixmap(w) != XmUNSPECIFIED_PIXMAP)
              {
                if (ShowSelectedPixmap(w) == True && Selected(w) == True)
                  {
                    if (ShrinkOutline(w) == True)
                      {
                        XFillRectangle(XtDisplay(w), XtWindow(w),
                            SelectBGGC(w),
                            PixmapX(w), PixmapY(w),
                            PixmapWidth(w), PixmapHeight(w));
                      }
                    else
                      {
                        XFillRectangle(XtDisplay(w), XtWindow(w),
                            SelectBGGC(w),
                            (int)pixmap_rect.x, (int)pixmap_rect.y,
                            (int)pixmap_rect.width, (int)pixmap_rect.height);
                      }
                    XCopyArea(XtDisplay(w), Pixmap(w), XtWindow(w),
                        MaskGC(w), 0, 0, PixmapWidth(w), PixmapHeight(w),
                        PixmapX(w), PixmapY(w));
                  }
                else
                  {
                    XCopyArea(XtDisplay(w), Pixmap(w), XtWindow(w),
                        MaskGC(w), 0, 0, PixmapWidth(w), PixmapHeight(w),
                        PixmapX(w), PixmapY(w));
                  }
                XFillRectangle(XtDisplay(w), XtWindow(w),
                    MaskStippleGC(w),
                    PixmapX(w), PixmapY(w),
                    PixmapWidth(w), PixmapHeight(w));
              }
          }
	if (StatePixmap(w) && StatePixmap(w) != XmUNSPECIFIED_PIXMAP)
	  {
	    XCopyArea(XtDisplay(w), StatePixmap(w), XtWindow(w), 
		StateMaskGC(w), 0, 0, StatePixmapWidth(w), StatePixmapHeight(w),
		StatePixmapX(w), StatePixmapY(w));
	  }
        if (IconShadowThickness(w))
          {
            _XmRegionDrawShadow(XtDisplay(w), XtWindow(w),
                TopShadowGC(w), BottomShadowGC(w), ShadowRegion(w),
                0, IconShadowThickness(w), IconShadowType(w));
          }
        if (Highlighted(w))
          {
            (* ((XmPrimitiveWidgetClass) XtClass(w))
                ->primitive_class.border_highlight) ((Widget) w);
          }
        else 
          {
            (* ((XmPrimitiveWidgetClass) XtClass(w))
                ->primitive_class.border_unhighlight) ((Widget) w);
          }
      }
}

/*
 *   NAME:     SetValues
 *   FUNCTION:
 *   RETURNS:
 */
static Boolean
SetValues(
   Widget cur,
   Widget req,
   IconWidget new,
   ArgList args,
   Cardinal *num_args
   )
{
    Boolean flag = False;
    Boolean newstring = False;
    Boolean CleanupFontFlag = False;
    Boolean Call_SetSize = False;

    /*  If the label has changed, make a copy of the new label,  */
    /*  and free the old label.                                  */

    if (Label(new) != Label(cur))
      {
        newstring = True;
        if (Pixmap(new) == XmUNSPECIFIED_PIXMAP && Label(new) == NULL)
          {
            XmString string;
            string = (XmString)XmeGetLocalizedString((char *) NULL,
		(Widget) new, XmNlabelString, new->core.name);
            Label(new) = XmStringCopy(string);
            XmStringFree(string);
          }
        else if (Label(new) != NULL)
          {
            if (XmeStringIsValid((XmString) Label(new)))
                Label(new) = XmStringCopy((XmString) Label(new));
            else
              {
                XmString string;
                string = (XmString)XmeGetLocalizedString((char *) NULL,
		    (Widget) new, XmNlabelString, new->core.name);
                Label(new) = XmStringCopy(string);
                XmStringFree(string);
              }
          }

        XmStringFree(Label(cur));
        Label(cur) = NULL;
        Label(req) = NULL;
      }

    if (TopLabel(new) != TopLabel(cur))
      {
        newstring = True;
	if (TopLabel(new) && XmeStringIsValid((XmString) TopLabel(new)))
	    TopLabel(new) = XmStringCopy((XmString) TopLabel(new));
	else
	    TopLabel(new) = NULL;
        XmStringFree(TopLabel(cur));
        TopLabel(cur) = NULL;
        TopLabel(req) = NULL;
      }
    if (BottomLabel(new) != BottomLabel(cur))
      {
        newstring = True;
	if (BottomLabel(new) && XmeStringIsValid((XmString)BottomLabel(new)))
	    BottomLabel(new) = XmStringCopy((XmString) BottomLabel(new));
	else
	    BottomLabel(new) = NULL;
        XmStringFree(BottomLabel(cur));
        BottomLabel(cur) = NULL;
        BottomLabel(req) = NULL;
      }
    if (Font(new) != Font(cur))
      {
        CleanupFontFlag = True;
        if (Font(new) == NULL)
            Font(new) = XmeGetDefaultRenderTable((Widget) new, XmLABEL_FONTLIST);
        Font(new) = XmFontListCopy(Font(new));
      }

    /* ValidateInputs(new); */

    if (new->core.sensitive != cur->core.sensitive ||
        new->core.ancestor_sensitive != cur->core.ancestor_sensitive)
      {
        flag = True;
      }

    if (Mask(new) != Mask(cur))
      {
        XtReleaseGC((Widget) cur, MaskGC(cur));
        XtReleaseGC((Widget) cur, MaskStippleGC(cur));
        CreateMaskGC(new);
	Call_SetSize = True;
	flag = True;
      }

    if (StateMask(new) != StateMask(cur))
      {
        XtReleaseGC((Widget) cur, StateMaskGC(cur));
        CreateStateMaskGC(new);
	Call_SetSize = True;
	flag = True;
      }

    if (newstring || Font(new) != Font(cur) || Pixmap(new) != Pixmap(cur) ||
	ShowSelectedPixmap(new) != ShowSelectedPixmap(cur) ||
	StatePixmap(new) != StatePixmap(cur))
      {
        Call_SetSize = True;
        flag = True;
      }

    if (Alignment(new) != Alignment(cur))
      {
        if(!XmRepTypeValidValue(XmRepTypeGetId(XmRAlignment), Alignment(new),
	    (Widget) new))
	  {
            Alignment(new) = Alignment(cur);
	  }
        flag = True;
        Call_SetSize = True;
      }
    if (StringDirection(new) != StringDirection(cur))
      {
        if(!XmRepTypeValidValue(XmRepTypeGetId(XmRStringDirection),
	    StringDirection(new), (Widget) new))
	  {
            StringDirection(new) = StringDirection(cur);
	  }
        flag = True;
      }
    if (ShrinkOutline(new) != ShrinkOutline(cur))
      {
        OldShrinkOutline(new) = ShrinkOutline(new);
        flag = True;
        Call_SetSize = True;
      }

    if (SelectColorPersistent(new) != SelectColorPersistent(cur))
      {
        if (!(SelectColorPersistent(new) == True ||
            SelectColorPersistent(new) == False))
            SelectColorPersistent(new) = False;
      }
    if (Selected(new) != Selected(cur))
      {
        if (!(Selected(new) == True || Selected(new) == False))
            Selected(new) = Selected(cur);
        flag = True;
      }
    if (IconShadowType(new) != IconShadowType(cur))
      {
        if (!XmRepTypeValidValue(XmRepTypeGetId(XmRShadowType),
            IconShadowType(new), (Widget) new))
	  {
            IconShadowType(new) = IconShadowType(cur);
	  }
        flag = True;
      }
    if (IconPlacement(new) != IconPlacement(cur))
      {
        if (!XmRepTypeValidValue(GuiRID_PIXMAP_PLACEMENT, IconPlacement(new),
            (Widget) new))
	  {
            IconPlacement(new) = IconPlacement(cur);
	  }
        Call_SetSize = True;
        flag = True;
      }
    if (StateGravity(new) != StateGravity(cur))
      {
	if (!XmRepTypeValidValue(GuiRID_STATE_PIXMAP_PLACEMENT, 
		StateGravity(new), (Widget) new))
	  {
            StateGravity(new) = GuiSOUTHWEST_GRAVITY;
	  }
        Call_SetSize = True;
        flag = True;
      }
    if (ResizeHeight(new) != ResizeHeight(cur))
      {
        if (!(ResizeHeight(new) == True || ResizeHeight(new) == False))
            ResizeHeight(new) = ResizeHeight(cur);
      }
    if (ResizeWidth(new) != ResizeWidth(cur))
      {
        if (!(ResizeWidth(new) == True || ResizeWidth(new) == False))
            ResizeWidth(new) = ResizeWidth(cur);
      }
    if (Active(new) != Active(cur))
      {
        if (!(Active(new) == True || Active(new) == False))
            Active(new) = Active(cur);
        flag = True;
      }
    if ((SelectColor(new) != SelectColor(cur)) && Selected(new))
        flag = True;

    if (MarginThickness(new) != MarginThickness(cur) ||
        IconShadowThickness(new) != IconShadowThickness(cur) ||
        HighlightThickness(new) != HighlightThickness(cur) ||
        Width(new) <= 0 ||
        Height(new) <= 0)
      {
        Call_SetSize = True;
        flag = True;
      }

    if ((new->core.sensitive != cur->core.sensitive) ||
        (new->core.ancestor_sensitive != cur->core.ancestor_sensitive))
        flag = True;

    if (Foreground(new) != Foreground(cur) ||
        BackgroundPixel(new) != BackgroundPixel(cur) ||
        SelectColor(new) != SelectColor(cur) ||
        SelectLabelColor(new) != SelectLabelColor(cur) ||
        Font(new) != Font(cur))
      {
        if (BackgroundPixel(new) != BackgroundPixel(cur))
          {
            Pixel top_shadow_color, bottom_shadow_color, select_color;
            Arg gc_args[2];

            XmGetColors(new->core.screen, new->core.colormap,
                BackgroundPixel(new), NULL, &top_shadow_color,
                &bottom_shadow_color, &select_color);
            if (!SelectColorPersistent(new))
                SelectColor(new) = select_color;
            XtSetArg(gc_args[0], XmNtopShadowColor, top_shadow_color);
            XtSetArg(gc_args[1], XmNbottomShadowColor, bottom_shadow_color);
            XtSetValues((Widget)new, gc_args, 2);
          }
        if (SelectColor(new) != SelectColor(cur))
          {
            if (SelectColor(new) == SelectLabelColor(new))
                SelectLabelColor(new) = Foreground(new);
          }
        XtReleaseGC((Widget) cur, NormalGC(cur));
        XtReleaseGC((Widget) cur, StippleGC(cur));
        XtReleaseGC((Widget) cur, SelectFGGC(cur));
        XtReleaseGC((Widget) cur, SelectBGGC(cur));
        XtReleaseGC((Widget) cur, MaskGC(cur));
        XtReleaseGC((Widget) cur, MaskStippleGC(cur));
	if (StateMaskGC(cur))
            XtReleaseGC((Widget) cur, StateMaskGC(cur));
        CreateGC(new);
        Call_SetSize = True;
        flag = True;
      }

    if (CleanupFontFlag && Font(cur))
        XmFontListFree(Font(cur));

    if (Call_SetSize == True)
        (*(new->core.widget_class->core_class.resize)) ((Widget) new);

    return(flag);
}

/*
 *   NAME:     QueryGeometry
 *   FUNCTION:
 *   RETURNS:
 */
static XtGeometryResult
QueryGeometry(
   IconWidget w,
   XtWidgetGeometry *intended,
   XtWidgetGeometry *desired
   )
{
    Dimension width, width1, height, height1;
    Boolean show_fields = False;

    width1 = Max(TopLabelWidth(w), BottomLabelWidth(w));
    height1 = TopLabelHeight(w) + BottomLabelHeight(w);

    switch (IconPlacement(w))
      {
    case GuiPIXMAP_TOP:
    case GuiPIXMAP_BOTTOM:
	width = Max(PixmapWidth(w), LabelWidth(w));
	height = PixmapHeight(w) + LabelHeight(w);
        break;
    case GuiPIXMAP_LEFT:
    case GuiPIXMAP_RIGHT:
	width = PixmapWidth(w) + LabelWidth(w);
	height = Max(PixmapHeight(w), LabelHeight(w));
        break;
      }
    if (Fields(w))
      {
	int i;
	if (Fields(w)->draw_fields)
	    for (i = 0; i < Fields(w)->n_fields; i++)
		if (Fields(w)->draw_fields[i])
		  {
		    show_fields = True;
		    break;
		  }
	else
	    show_fields = True;
      }
    if (ResizeWidth(w) == False || show_fields)
	desired->width = XtWidth(w);
    else
	desired->width = (Dimension) Max(width, width1) +
	    2 * (IconShadowThickness(w) +
	    MarginThickness(w) + HighlightThickness(w));
    if (ResizeHeight(w) == False || show_fields)
	desired->height = XtHeight(w);
    else
	desired->height = height +  height1 +
	    2 * (IconShadowThickness(w) +
	    MarginThickness(w) + HighlightThickness(w));
    if (desired->width == 0)
	desired->width = 1;
    if (desired->height == 0)
	desired->height = 1;
}

/*
 *   NAME:     SingleClickCB
 *   FUNCTION:
 *   RETURNS:
 */
static void
SingleClickCB(
   Widget      widget,
   XEvent      *event,
   String      *params,
   Cardinal    *num_params
   )
{
    IconWidget                  w = (IconWidget) widget;
    GuiIconCallbackStruct       call_value;
    int                         reason = GuiSINGLE_CLICK;
    XtCallbackList              callback = w->icon.single_click_callback;
    Arg                         args[1];

    if (event->type == KeyPress)
      {
        if ((event->xkey.time - w->icon.old_time) <=
            XtGetMultiClickTime(XtDisplay(w)))
          {
            reason = GuiDOUBLE_CLICK;
            callback = w->icon.double_click_callback;
          }
        w->icon.old_time = event->xkey.time;
      }
    else /* event->type == ButtonPress */
      {
	if (Fields(w))
	 {
            if (_XmRegionPointInRegion(HighlightRegion(w),
                event->xbutton.x, event->xbutton.y) == False)
	      {
                w->icon.old_time = 0;
                return;
	      }
	 }
        else if (_XmRegionPointInRegion(ShadowRegion(w),
                 event->xbutton.x, event->xbutton.y) == False)
          {
            w->icon.old_time = 0;
            return;
          }
        w->icon.old_time = event->xbutton.time;
      }

    if (callback)
      {
        (void) XmProcessTraversal(widget, XmTRAVERSE_CURRENT);
        XFlush(XtDisplay(w));
        call_value.reason = reason;
        call_value.prev_selected = Selected(widget);
        call_value.event = event;
        XtSetArg(args[0], XmNlabelString, &call_value.string);
        XtGetValues(widget, args, 1);
        Selected(widget) = True;
        Redisplay((IconWidget)widget, NULL, NULL);
        XtCallCallbackList((Widget) w, callback, &call_value);
        XmStringFree(call_value.string);
      }
}

/*
 *   NAME:     DoubleClickCB
 *   FUNCTION:
 *   RETURNS:
 */
static void
DoubleClickCB(
   Widget      widget,
   XEvent      *event,
   String      *params,
   Cardinal    *num_params
   )
{
    IconWidget                  w = (IconWidget) widget;
    GuiIconCallbackStruct       call_value;
    Arg                         args[1];

    if (event->type == ButtonPress)
      {
	if (event->xkey.time - w->icon.old_time >
	    XtGetMultiClickTime(XtDisplay(w)))
	  {
	    return;
	  }
	else if (Fields(w))
	  {
            if (_XmRegionPointInRegion(HighlightRegion(w),
                event->xbutton.x, event->xbutton.y) == False)
	      {
                return;
	      }
	  }
	else if (_XmRegionPointInRegion(ShadowRegion(w),
                 event->xbutton.x, event->xbutton.y) == False)
          {
            return;
          }
      }

    if (w->icon.double_click_callback)
      {
        (void) XmProcessTraversal(widget, XmTRAVERSE_CURRENT);
        XFlush(XtDisplay(w));
        call_value.reason = GuiDOUBLE_CLICK;
        call_value.prev_selected = Selected(widget);
        call_value.event = event;
        XtSetArg(args[0], XmNlabelString, &call_value.string);
        XtGetValues(widget, args, 1);
        Selected(widget) = True;
        Redisplay((IconWidget)widget, NULL, NULL);
        XtCallCallbackList((Widget) w, w->icon.double_click_callback,
            &call_value);
        XmStringFree(call_value.string);
      }
}

/*
 *   NAME:     HighlightBorder
 *   FUNCTION:
 *   RETURNS:
 */
static void
HighlightBorder(
   Widget w
   )
{
    XmPrimitiveWidget pw;

    pw = (XmPrimitiveWidget) w;

    if (Width(w) == 0 || Height(pw) == 0 || HighlightThickness(w) == 0)
        return;

    pw->primitive.highlighted = True;
    pw->primitive.highlight_drawn = True;

    XSetForeground(XtDisplay(w), NormalGC(w), HighlightColor(w));
    _XmRegionDrawShadow(XtDisplay(w), XtWindow(w),
        NormalGC(w), NormalGC(w), HighlightRegion(w),
        0, HighlightThickness(w),
        IconShadowType(w));
    XSetForeground(XtDisplay(w), NormalGC(w), Foreground(w));
}

/*
 *   NAME:     UnhighlightBorder
 *   FUNCTION:
 *   RETURNS:
 */
static void
UnhighlightBorder(
   Widget w
   )
{
    XmPrimitiveWidget pw;

    pw = (XmPrimitiveWidget) w;

    pw->primitive.highlighted = False;
    pw->primitive.highlight_drawn = False;

    if (Width(w) == 0 || Height(w) == 0 || HighlightThickness(w) == 0)
        return;

    XSetForeground(XtDisplay(w), NormalGC(w), BackgroundPixel(w));
    _XmRegionDrawShadow(XtDisplay(w), XtWindow(w),
        NormalGC(w), NormalGC(w), HighlightRegion(w),
        0, HighlightThickness(w),
        IconShadowType(w));
    XSetForeground(XtDisplay(w), NormalGC(w), Foreground(w));
}

/*
 *   NAME:     GetLabelString
 *   FUNCTION:
 *   RETURNS:
 */
static void
GetLabelString(
   Widget      w,
   int         offset,
   XtArgVal    *value
   )
{
    XmString string;

    string = XmStringCopy(Label(w));

    *value = (XtArgVal) string;
}

/*
 *   NAME:     GetTopLabelString
 *   FUNCTION:
 *   RETURNS:
 */
static void
GetTopLabelString(
   Widget      w,
   int         offset,
   XtArgVal    *value
   )
{
    XmString string;

    string = XmStringCopy(TopLabel(w));

    *value = (XtArgVal) string;
}

/*
 *   NAME:     GetBottomLabelString
 *   FUNCTION:
 *   RETURNS:
 */
static void
GetBottomLabelString(
   Widget      w,
   int         offset,
   XtArgVal    *value
   )
{
    XmString string;

    string = XmStringCopy(BottomLabel(w));

    *value = (XtArgVal) string;
}

/************************************************************************
 *
 *  Dynamic defaulting color functions
 *
 ************************************************************************/

static void 
ForegroundColorDefault(
   Widget widget,
   int offset,
   XrmValue *value 
   )
{
    XmeGetDefaultPixel(widget, XmFOREGROUND, offset, value);
}

static void 
SelectColorDefault(
   Widget widget,
   int offset,
   XrmValue *value 
   )
{
    XmeGetDefaultPixel(widget, XmSELECT, offset, value);
}
