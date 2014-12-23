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
/* $TOG: Icon.c /main/20 1999/09/08 10:44:52 mgreess $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1990,1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

/**---------------------------------------------------------------------
***	
***	file:		Icon.c
***
***	project:	MotifPlus Widgets
***
***	description:	Source code for DtIconGadget class.
***	
***-------------------------------------------------------------------*/


/*-------------------------------------------------------------
**	Include Files
*/

#include <stdio.h>
#ifdef __apollo
#include <sys/types.h>
#endif
#include <sys/stat.h>
#include <X11/Xatom.h>
#include <Xm/XmP.h>
#include <Xm/CacheP.h>
#include <Xm/ManagerP.h>
#include <Xm/PrimitiveP.h>
#include <Xm/DrawP.h>
#include <Dt/IconP.h>
#include <Dt/MacrosP.h>
#include <Dt/DtMsgsP.h>
#include <Dt/Dnd.h>
#include "DtWidgetI.h"
#include "DtSvcInternal.h"

#include <Xm/XmPrivate.h>  /* Motif _XmEnterGadget and friends */

/*-------------------------------------------------------------
**	Public Interface
**-------------------------------------------------------------
*/

WidgetClass	dtIconGadgetClass;

#define Min(x, y)    (((x) < (y)) ? (x) : (y))
#define Max(x, y)    (((x) > (y)) ? (x) : (y))
#define Limit(x, lim)  (((lim) == 0 || (x) <= (lim))? (x): (lim))

/*-------------------------------------------------------------
**	Forward Declarations
*/

extern void 	_DtRegisterNewConverters ( void );
extern char *	_XmExtObjAlloc(int size);
extern void	_XmExtObjFree(XtPointer element);
extern void	_XmSelectColorDefault ( Widget, int, XrmValue * );

#define SPACING_DEFAULT		2
#define MARGIN_DEFAULT		2
#define UNSPECIFIED_DIMENSION	9999
#define UNSPECIFIED_CHAR	255

#define WARN_ALIGNMENT		_DtMsgIcon_0000
#define WARN_BEHAVIOR		_DtMsgIcon_0001
#define WARN_FILL_MODE		_DtMsgIcon_0002
#define WARN_PIXMAP_POSITION	_DtMsgIcon_0003
#define WARN_MARGIN		_DtMsgIcon_0004
#define WARN_SHADOW_TYPE	_DtMsgIcon_0005


#define MgrParent(g) ((XmManagerWidget) (XmIsManager(XtParent(g)) ? (XtParent(g)) : (XtParent(XtParent(g)))))

/********    Private Function Declarations    ********/

extern void _DtIconRegisterDropsite(
			Widget w) ;

/********    End Private Function Declarations    ********/


/********    Static Function Declarations    ********/

static int IconCacheCompare( 
                        XtPointer A,
                        XtPointer B) ;

static void ReCacheIcon_r( 
			DtIconCacheObjPart *cache,
                        DtIconGadget g);

static void GetDefaultBackground( 
                        Widget g,
                        int offset,
                        XrmValue *value) ;
static void GetDefaultForeground( 
                        Widget g,
                        int offset,
                        XrmValue *value) ;
static void GetDefaultFillMode( 
                        Widget w,
                        int offset,
                        XrmValue *value) ;
static void GetSpacing( 
                        Widget w,
                        int offset,
                        XtArgVal *value) ;
static void GetString( 
                        Widget w,
                        int offset,
                        XtArgVal *value) ;
static void IconEventHandler( 
                        Widget w,
                        XtPointer client_data,
                        XButtonEvent *event) ;
static void ClickTimeout( 
                        Widget w,
                        XtIntervalId *id) ;
static void IconArm( 
                        Widget w,
                        XEvent *event) ;
static void IconDisarm( 
                        Widget w,
                        XEvent *event) ;
static void IconActivate( 
                        Widget w,
                        XEvent *event) ;
static void IconDrag( 
                        Widget w,
                        XEvent *event) ;
static void IconPopup( 
                        Widget w,
                        XEvent *event) ;
static void IconEnter( 
                        Widget w,
                        XEvent *event) ;
static void IconLeave( 
                        Widget w,
                        XEvent *event) ;
static void ClassInitialize( void ) ;
static void SecondaryObjectCreate( 
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
static void InitializePosthook( 
                        Widget req,
                        Widget new,
                        ArgList args,
                        Cardinal *num_args) ;
static Boolean SetValuesPrehook( 
                        Widget oldParent,
                        Widget refParent,
                        Widget newParent,
                        ArgList args,
                        Cardinal *num_args) ;
static void GetValuesPrehook( 
                        Widget newParent,
                        ArgList args,
                        Cardinal *num_args) ;
static void GetValuesPosthook( 
                        Widget new,
                        ArgList args,
                        Cardinal *num_args) ;
static Boolean SetValuesPosthook( 
                        Widget current,
                        Widget req,
                        Widget new,
                        ArgList args,
                        Cardinal *num_args) ;
static void QualifyIconLocalCache( 
                        DtIconGadget g,
			DtIconCacheObjPart *cache);
static void Initialize( 
                        Widget request_w,
                        Widget new_w) ;
static void Destroy( 
                        Widget w) ;
static void Resize( 
                        Widget w) ;
static void Redisplay( 
                        Widget w,
                        XEvent *event,
                        Region region) ;
static Boolean SetValues( 
                        Widget current_w,
                        Widget request_w,
                        Widget new_w) ;
static void BorderHighlight(
                        DtIconGadget g) ;
static void BorderUnhighlight(
                        DtIconGadget g) ;
static void ArmAndActivate( 
                        Widget w,
                        XEvent *event) ;
static void InputDispatch( 
                        Widget w,
                        XButtonEvent *event,
                        Mask event_mask) ;
static Boolean VisualChange( 
                        Widget w,
                        Widget current_w,
                        Widget new_w) ;
static void GetSize( 
                        DtIconGadget g,
                        Dimension *w,
                        Dimension *h) ;
static void GetPositions( 
                        DtIconGadget g,
                        Position w,
                        Position h,
                        Dimension h_t,
                        Dimension s_t,
                        Position *pix_x,
                        Position *pix_y,
                        Position *str_x,
                        Position *str_y) ;
static void Draw( 
                        DtIconGadget g,
                        Drawable drawable,
                        Position x,
                        Position y,
                        Dimension w,
                        Dimension h,
                        Dimension h_t,
                        Dimension s_t,
                        unsigned char s_type,
                        unsigned char fill_mode) ;
static void CallCallback( 
                        DtIconGadget g,
                        XtCallbackList cb,
                        int reason,
                        XEvent *event) ;
static void UpdateGCs( 
                        DtIconGadget g) ;
static Cardinal GetIconClassSecResData( 
                        WidgetClass class,
                        XmSecondaryResourceData **data_rtn) ;
static XtPointer GetIconClassResBase( 
                        Widget widget,
                        XtPointer client_data) ;
static Boolean LoadPixmap( 
                        DtIconGadget new,
                        String pixmap) ;
static void ClassPartInitialize ( WidgetClass	wc);
static void HighlightBorder( Widget w );
static void UnhighlightBorder( Widget w );

/********    End Static Function Declarations    ********/

/*      External Cache Procs
*/
extern void _XmCacheDelete( XtPointer data );
extern void _XmCacheCopy( XtPointer src, XtPointer dest, size_t size );
extern Cardinal _XmSecondaryResourceData( XmBaseClassExt, XmSecondaryResourceData **, 
			  XtPointer, String, String, XmResourceBaseProc);
extern XtPointer _XmCachePart( XmCacheClassPartPtr cp, XtPointer cpart, size_t  size );


/*-------------------------------------------------------------
**	Resource List
*/
#define I_Offset(field) \
	XtOffset (DtIconGadget, icon.field)

#define I_Cache_Offset(field) \
	XtOffset (DtIconCacheObject, icon_cache.field)

static XtResource resources[] = 
{
	{
		XmNset,
		XmCSet, XmRBoolean, sizeof (Boolean),
		I_Offset (set), XmRImmediate, (XtPointer) False
	},
	{
		XmNshadowType,
		XmCShadowType, XmRShadowType, sizeof (unsigned char),
		I_Offset (shadow_type),
		XmRImmediate, (XtPointer) XmSHADOW_ETCHED_OUT
	},
	{
		XmNborderType,
		XmCBorderType, XmRBorderType, sizeof (unsigned char),
		I_Offset (border_type),
		XmRImmediate, (XtPointer) DtRECTANGLE
	},
	{
		XmNcallback,
		XmCCallback, XmRCallback, sizeof (XtCallbackList),
		I_Offset (callback), XmRImmediate, (XtPointer) NULL
	},
	{
		XmNfontList,
		XmCFontList, XmRFontList, sizeof (XmFontList),
		I_Offset (font_list), XmRString, "Fixed"
	},
	{
		XmNimageName,
		XmCString, XmRString, sizeof (XmString),
		I_Offset (image_name), XmRImmediate, (XtPointer) NULL
	},
	{
		XmNpixmap,
		XmCPixmap, XmRPixmap, sizeof (Pixmap),
		I_Offset (pixmap),
		XmRImmediate, (XtPointer) XmUNSPECIFIED_PIXMAP
	},
	{
		XmNstring,
		XmCXmString, XmRXmString, sizeof (XmString),
		I_Offset (string),
		XmRImmediate, (XtPointer) XmUNSPECIFIED_STRING
	},
	{
		XmNpixmapForeground,
		XmCForeground, XmRPixel, sizeof (Pixel),
		I_Offset (pixmap_foreground),
		XmRCallProc, (XtPointer) GetDefaultForeground
	},
	{
		XmNpixmapBackground,
		XmCBackground, XmRPixel, sizeof (Pixel),
		I_Offset (pixmap_background),
		XmRCallProc, (XtPointer) GetDefaultBackground
	},
	{
		XmNmaxPixmapWidth,
		XmCMaxWidth, XmRHorizontalDimension, sizeof (Dimension),
		I_Offset (max_pixmap_width), XmRImmediate, (XtPointer) 0
	},
	{
		XmNmaxPixmapHeight,
		XmCMaxHeight, XmRVerticalDimension, sizeof (Dimension),
		I_Offset (max_pixmap_height), XmRImmediate, (XtPointer) 0
	},
	{
		XmNunderline,
		XmCUnderline, XmRBoolean, sizeof (Boolean),
		I_Offset (underline), XmRImmediate, (XtPointer) False
        },
	{
		XmNdropSiteOperations, XmCDropSiteOperations,
                XmRDropSiteOperations, sizeof(unsigned char),
                I_Offset(operations), XmRImmediate, (XtPointer) XmDROP_NOOP
	},
	{
		XmNdropCallback,
		XmCDropCallback, XmRCallback, sizeof (XtCallbackList),
		I_Offset (drop_callback), XmRImmediate, (XtPointer) NULL
	}
};


static XtResource cache_resources[] =
{
	{
		XmNbehavior,
		XmCBehavior, XmRBehavior, sizeof (unsigned char),
		I_Cache_Offset (behavior),
		XmRImmediate, (XtPointer) XmICON_BUTTON
	},
	{
		XmNrecomputeSize,
		XmCRecomputeSize, XmRBoolean, sizeof (Boolean),
		I_Cache_Offset (recompute_size), XmRImmediate, (XtPointer) True
	},
	{
		"drawShadow",
		"DrawShadow", XmRBoolean, sizeof (Boolean),
		I_Cache_Offset (draw_shadow), XmRImmediate, (XtPointer) False
	},
	{
		XmNfillOnArm,
		XmCFillOnArm, XmRBoolean, sizeof (Boolean),
		I_Cache_Offset (fill_on_arm), XmRImmediate, (XtPointer) True
	},
	{
		XmNforeground,
		XmCForeground, XmRPixel, sizeof (Pixel),
		I_Cache_Offset (foreground),
		XmRCallProc, (XtPointer) GetDefaultForeground
	},
	{
		XmNbackground,
		XmCBackground, XmRPixel, sizeof (Pixel),
		I_Cache_Offset (background),
		XmRCallProc, (XtPointer) GetDefaultBackground
	},
	{
		XmNarmColor,
		XmCArmColor, XmRPixel, sizeof (Pixel),
		I_Cache_Offset (arm_color),
		XmRCallProc, (XtPointer) _XmSelectColorDefault
	},
	{
		XmNspacing,
		XmCSpacing, XmRHorizontalDimension, sizeof (Dimension),
		I_Cache_Offset (spacing),
		XmRImmediate, (XtPointer) UNSPECIFIED_DIMENSION
	},
	{
		XmNmarginHeight,
		XmCMarginHeight, XmRVerticalDimension, sizeof (Dimension),
		I_Cache_Offset (margin_height),
		XmRImmediate, (XtPointer) UNSPECIFIED_DIMENSION
	},
	{
		XmNmarginWidth,
		XmCMarginWidth, XmRHorizontalDimension, sizeof (Dimension),
		I_Cache_Offset (margin_width),
		XmRImmediate, (XtPointer) UNSPECIFIED_DIMENSION
	},
	{
		XmNpixmapPosition,
		XmCPixmapPosition, XmRPixmapPosition, sizeof (unsigned char),
		I_Cache_Offset (pixmap_position),
		XmRImmediate, (XtPointer) UNSPECIFIED_CHAR
	},
	{
		XmNstringPosition,
		XmCStringPosition, XmRStringPosition, sizeof (unsigned char),
		I_Cache_Offset (string_position),
		XmRImmediate, (XtPointer) UNSPECIFIED_CHAR
	},
	{
		XmNalignment,
		XmCAlignment, XmRAlignment, sizeof (unsigned char),
		I_Cache_Offset (alignment),
		XmRImmediate, (XtPointer) XmALIGNMENT_BEGINNING
	},
	{
		XmNfillMode,
		XmCFillMode, XmRFillMode, sizeof (unsigned char),
		I_Cache_Offset (fill_mode),
		XmRCallProc, (XtPointer) GetDefaultFillMode
	}
};


static XmSyntheticResource syn_resources[] =
{
	{
		XmNstring, sizeof (XmString),
		I_Offset (string), GetString,
		(XmImportProc) NULL
	},
};


static XmSyntheticResource cache_syn_resources[] =
{
	{
		XmNspacing, sizeof (Dimension),
		I_Cache_Offset (spacing), GetSpacing,
		(XmImportProc) NULL
	},
	{
		XmNmarginWidth, sizeof (Dimension),
		I_Cache_Offset (margin_width),
		XmeFromHorizontalPixels, 
                (XmImportProc)XmeToHorizontalPixels
	},
	{
		XmNmarginHeight, sizeof (Dimension),
		I_Cache_Offset (margin_height),
		XmeFromVerticalPixels, 
                (XmImportProc)XmeToVerticalPixels, 
	}
};

#undef	I_Offset
#undef	I_Cache_Offset
	

/*-------------------------------------------------------------
**	Cache Class Record
*/

static XmCacheClassPart IconClassCachePart = {
        {NULL, 0, 0},            /* head of class cache list */
        _XmCacheCopy,           /* Copy routine         */
        _XmCacheDelete,         /* Delete routine       */
        IconCacheCompare,       /* Comparison routine   */
};

/*-------------------------------------------------------------
**	Base Class Extension Record
*/
static XmBaseClassExtRec       iconBaseClassExtRec = {
    NULL,                                     /* Next extension       */
    NULLQUARK,                                /* record type XmQmotif */
    XmBaseClassExtVersion,                    /* version              */
    sizeof(XmBaseClassExtRec),                /* size                 */
    XmInheritInitializePrehook,               /* initialize prehook   */
    SetValuesPrehook,                         /* set_values prehook   */
    InitializePosthook,                       /* initialize posthook  */
    SetValuesPosthook,                        /* set_values posthook  */
    (WidgetClass)&dtIconCacheObjClassRec,     /* secondary class      */
    (XtInitProc)SecondaryObjectCreate,        /* creation proc        */
    (XmGetSecResDataFunc) GetIconClassSecResData,    /* getSecResData */
    {0},                                      /* fast subclass        */
    GetValuesPrehook,                         /* get_values prehook   */
    GetValuesPosthook,                        /* get_values posthook  */
    NULL,                                     /* classPartInitPrehook */
    NULL,                                     /* classPartInitPosthook*/
    NULL,                                     /* ext_resources        */
    NULL,                                     /* compiled_ext_resources*/
    0,                                        /* num_ext_resources    */
    FALSE,                                    /* use_sub_resources    */
    XmInheritWidgetNavigable,                 /* widgetNavigable      */
    XmInheritFocusChange,                     /* focusChange          */
};

/*-------------------------------------------------------------
**	Icon Cache Object Class Record
*/
externaldef (dticoncacheobjclassrec)
DtIconCacheObjClassRec dtIconCacheObjClassRec =
{
  {
      /* superclass         */    (WidgetClass) &xmExtClassRec,
      /* class_name         */    "DtIcon",
      /* widget_size        */    sizeof(DtIconCacheObjRec),
      /* class_initialize   */    NULL,
      /* chained class init */    NULL,
      /* class_inited       */    False,
      /* initialize         */    NULL,
      /* initialize hook    */    NULL,
      /* realize            */    NULL,
      /* actions            */    NULL,
      /* num_actions        */    0,
      /* resources          */    cache_resources,
      /* num_resources      */    XtNumber(cache_resources),
      /* xrm_class          */    NULLQUARK,
      /* compress_motion    */    False,
      /* compress_exposure  */    False,
      /* compress enter/exit*/    False,
      /* visible_interest   */    False,
      /* destroy            */    NULL,
      /* resize             */    NULL,
      /* expose             */    NULL,
      /* set_values         */    NULL,
      /* set values hook    */    NULL,
      /* set values almost  */    NULL,
      /* get values hook    */    NULL,
      /* accept_focus       */    NULL,
      /* version            */    XtVersion,
      /* callback offsetlst */    NULL,
      /* default trans      */    NULL,
      /* query geo proc     */    NULL,
      /* display accelerator*/    NULL,
      /* extension record   */    NULL,
   },

   {
      /* synthetic resources */   cache_syn_resources,
      /* num_syn_resources   */   XtNumber(cache_syn_resources),
      /* extension           */   NULL,
   },
};

/*-------------------------------------------------------------
**	Class Record
*/
externaldef (dticonclassrec)
DtIconClassRec dtIconClassRec =
{
	/*	Core Part
	*/
	{	
		(WidgetClass) &xmGadgetClassRec, /* superclass		*/
		"DtIcon",			/* class_name		*/
		sizeof (DtIconRec),		/* widget_size		*/
		ClassInitialize,		/* class_initialize	*/
		ClassPartInitialize,		/* class_part_initialize*/
		False,				/* class_inited		*/
		(XtInitProc) Initialize,	/* initialize		*/
		NULL,				/* initialize_hook	*/
		NULL,				/* realize		*/
		NULL,				/* actions		*/
		0,				/* num_actions		*/
		resources,			/* resources		*/
		XtNumber (resources),		/* num_resources	*/
		NULLQUARK,			/* xrm_class		*/
		True,				/* compress_motion	*/
		True,				/* compress_exposure	*/
		True,				/* compress_enterleave	*/
		False,				/* visible_interest	*/	
		(XtWidgetProc) Destroy, 	/* destroy		*/	
		(XtWidgetProc) Resize,		/* resize		*/
		(XtExposeProc) Redisplay,	/* expose		*/	
		(XtSetValuesFunc) SetValues,	/* set_values		*/	
		NULL,				/* set_values_hook	*/
		XtInheritSetValuesAlmost,	/* set_values_almost	*/
		NULL,				/* get_values_hook	*/
		NULL,				/* accept_focus		*/	
		XtVersion,			/* version		*/
		NULL,				/* callback private	*/
		NULL,				/* tm_table		*/
		NULL,				/* query_geometry	*/
		NULL,				/* display_accelerator	*/
		(XtPointer)&iconBaseClassExtRec,/* extension		*/
	},

	/*	XmGadget Part
	*/
	{
	(XtWidgetProc)	BorderHighlight,	/* border_highlight	*/
	(XtWidgetProc)	BorderUnhighlight,	/* border_unhighlight	*/
	(XtActionProc)	ArmAndActivate,		/* arm_and_activate	*/
	(XmWidgetDispatchProc)	InputDispatch,	/* input_dispatch	*/
	(XmVisualChangeProc)	VisualChange,	/* visual_change	*/
		syn_resources,			/* get_resources	*/
		XtNumber (syn_resources),	/* num_get_resources	*/
		&IconClassCachePart,		/* class_cache_part	*/
		NULL,	 			/* extension		*/
	},

	/*	DtIconGadget Part
	*/
	{
		GetSize,			/* get_size		*/
		GetPositions,			/* get_positions	*/
		Draw,				/* draw			*/
		CallCallback,			/* call_callback	*/
		UpdateGCs,			/* update_gcs		*/
		True,				/* optimize_redraw	*/
		NULL,				/* class_cache_part	*/
		NULL,				/* extension		*/
	}
};


externaldef (dticongadgetclass) WidgetClass dtIconGadgetClass = (WidgetClass) &dtIconClassRec;


/*-------------------------------------------------------------
**	Private Functions
**-------------------------------------------------------------
*/

/************************************************************************
 *
 *  The border highlighting and unhighlighting routines.
 *
 *  These routines were originally in Obsolete.c but can not depend
 *  on these routines to live forever. Therefore, copied into my
 *  own space.
 *
 ************************************************************************/

static void
HighlightBorder(
        Widget w )
{
  XtWidgetProc border_highlight;

  if (XmIsPrimitive(w)) {
    _DtProcessLock();
    border_highlight = xmPrimitiveClassRec.primitive_class.border_highlight;
    _DtProcessUnlock();
    (*border_highlight) (w);
  } else if (XmIsGadget(w)) {
    _DtProcessLock();
    border_highlight = xmGadgetClassRec.gadget_class.border_highlight;
    _DtProcessUnlock();
    (*border_highlight) (w);
  }
}

static void
UnhighlightBorder(
        Widget w )
{
  XtWidgetProc border_unhighlight;

  if (XmIsPrimitive(w)) {
    _DtProcessLock();
    border_unhighlight = xmPrimitiveClassRec.primitive_class.border_unhighlight;
    _DtProcessUnlock();
    (*border_unhighlight) (w);
  } else if (XmIsGadget(w)) {
    _DtProcessLock();
    border_unhighlight = xmGadgetClassRec.gadget_class.border_unhighlight;
    _DtProcessUnlock();
    (*border_unhighlight) (w);
  }
}



/*-------------------------------------------------------------
**	GetMaskGC
**		Get normal and background graphics contexts.
*/
static GC
GetMaskGC(
        DtIconGadget g,
	  Position x, 
	  Position y)
{
    if (G_Mask(g) != XmUNSPECIFIED_PIXMAP) {

	XSetClipOrigin(XtDisplay(g),
		       G_ClipGC(g),
		       x, y);
	return G_ClipGC(g);
    }
    else {
	return G_NormalGC(g);
    }
}

/*-------------------------------------------------------------
**	GetDefaultBackground
**		Copy background pixel from Manager parent.
*/
/* ARGSUSED */
static void 
GetDefaultBackground(
        Widget g,
        int offset,
        XrmValue *value )
{
	static Pixel		pixel;
	XmManagerWidget		parent = (XmManagerWidget) XtParent (g);

	value->addr = (XtPointer) &pixel;
	value->size = sizeof (Pixel);

	if (XmIsManager ((Widget) parent))
		pixel = M_Background (parent);
	else
		XmeGetDefaultPixel (g, XmBACKGROUND, offset, value);
}


/*-------------------------------------------------------------
**	GetDefaultForeground
**		Copy foreground pixel from Manager parent.
*/
static void 
GetDefaultForeground(
        Widget g,
        int offset,
        XrmValue *value )
{
	static Pixel		pixel;
	XmManagerWidget		parent = (XmManagerWidget) XtParent (g);

	value->addr = (XtPointer) &pixel;
	value->size = sizeof (Pixel);

	if (XmIsManager ((Widget) parent))
		pixel = M_Foreground (parent);
	else
		XmeGetDefaultPixel (g, XmFOREGROUND, offset, value);
}



/*-------------------------------------------------------------
**	GetDefaultFillMode
**		Get default fill mode.
*/
/* ARGSUSED */
static void 
GetDefaultFillMode(
        Widget w,
        int offset,
        XrmValue *value )
{
	static unsigned char	fill_mode;
	DtIconGadget	g =	(DtIconGadget) w;

	value->addr = (XtPointer) &fill_mode;
	value->size = sizeof (unsigned char);

	if (G_ShadowThickness (g) == 0)
		fill_mode = XmFILL_PARENT;
	else
		fill_mode = XmFILL_SELF;
}


/*-------------------------------------------------------------
**	GetSpacing
**		Convert from pixels to horizontal or vertical units.
*/
static void 
GetSpacing(
        Widget w,
        int offset,
        XtArgVal *value )
{
	DtIconCacheObject icon_co = (DtIconCacheObject) w;

        if (G_CachePixmapPosition (icon_co) == XmPIXMAP_TOP ||
            G_CachePixmapPosition (icon_co) == XmPIXMAP_BOTTOM ||
            G_CachePixmapPosition (icon_co) == XmPIXMAP_MIDDLE)
                XmeFromVerticalPixels ((Widget)icon_co, offset, value);
        else
                XmeFromHorizontalPixels ((Widget)icon_co, offset, value);
}



/*-------------------------------------------------------------
**	GetString
**		Convert string from internal to external form.
*/
/* ARGSUSED */
static void 
GetString(
        Widget w,
        int offset,
        XtArgVal *value )
{
	DtIconGadget	g =	(DtIconGadget) w;
	XmString	string;

	string = XmStringCopy (G_String (g));

	*value = (XtArgVal) string;
}


/*-------------------------------------------------------------
**	IconEventHandler
**		Event handler for middle button events.
*/
/* ARGSUSED */
static void 
IconEventHandler(
        Widget w,
        XtPointer client_data,
        XButtonEvent *event )
{
	DtIconGadget		g =	NULL;

	g = (DtIconGadget) XmObjectAtPoint (w, event->x, event->y);
	
	if (!g || !DtIsIcon ((Widget)g))
		return;

	if (event->button == Button2 || event->button == Button3)
	{
		if (event->type == ButtonPress)
			InputDispatch ((Widget) g, event, XmARM_EVENT);
		else if (event->type == ButtonRelease)
			InputDispatch ((Widget) g, event, XmACTIVATE_EVENT);
	}
}



/*-------------------------------------------------------------
**	ClickTimeout
**		Clear Click flags.
*/
/* ARGSUSED */
static void 
ClickTimeout(
        Widget w,
        XtIntervalId *id )
{
	DtIconGadget	g = 	(DtIconGadget) w;
	Time		last_time, end_time;

	if (! G_Armed (g))
	{
		G_ClickTimerID (g) = 0;
		XtFree ((char *)G_ClickEvent (g));
		G_ClickEvent (g) = NULL;
		return;
	}

	last_time = XtLastTimestampProcessed (XtDisplay (g));
		/*
  		 * fix for bug# 4504
		 */
	if( G_ClickEvent (g) == NULL) return;

	end_time = G_ClickEvent (g) -> time + (Time)
				XtGetMultiClickTime (XtDisplay (g)); 

/*	Sync and reset timer if server interval may not have elapsed.
*/
	if ((last_time < end_time) && G_Sync (g))
	{
		G_Sync (g) = False;
		XSync (XtDisplay (g), False);
		G_ClickTimerID (g) = 
			XtAppAddTimeOut (XtWidgetToApplicationContext ((Widget)g),
					(unsigned long) 50, 
					(XtTimerCallbackProc)ClickTimeout, 
					(XtPointer) g);
	}
/*	Handle Select action.
*/
	else
	{
		CallCallbackProc call_callback;
		XtExposeProc expose;

		_DtProcessLock();
		expose = XtCoreProc(w, expose);
		call_callback = C_CallCallback(XtClass(w));
		_DtProcessUnlock();

		G_ClickTimerID (g) = 0;
		G_Armed (g) = False;
		(*call_callback) (g, G_Callback (g), XmCR_SELECT,
				  (XEvent *)G_ClickEvent (g));

	        if ((G_Behavior (g) == XmICON_DRAG) &&
                    (G_ShadowThickness (g) == 0))
                {
                   /* Do nothing */
                }
                else
		   (*expose) ((Widget)g, (XEvent*)G_ClickEvent (g), NULL);
		XtFree ((char *)G_ClickEvent (g));
		G_ClickEvent (g) = NULL;
	}
}



/*-------------------------------------------------------------
**	Action Procs
**-------------------------------------------------------------
*/

/*-------------------------------------------------------------
**	IconArm
**		Handle Arm action.
*/
static void 
IconArm(
        Widget w,
        XEvent *event )
{
	DtIconGadget	g = 	(DtIconGadget) w;

	if (G_Armed (g) || G_Behavior (g) == XmICON_LABEL)
		return;

	G_Armed (g) = True;

	if (G_Behavior (g) == XmICON_DRAG)
	  {
	    CallCallbackProc call_callback;

	    _DtProcessLock();
	    call_callback = C_CallCallback(XtClass(g));
	    _DtProcessUnlock();
	    (*call_callback) (g, G_Callback (g), XmCR_ARM, event);
	  }

	if ((G_Behavior (g) == XmICON_DRAG) &&
            (G_ShadowThickness (g) == 0))
        {
           /* Do nothing */
        }
        else
	  {
	    XtExposeProc expose;

	    _DtProcessLock();
	    expose = XtCoreProc(w, expose);
	    _DtProcessUnlock();
	    (*expose) ((Widget)g, event, NULL);
	  }
}



/*-------------------------------------------------------------
**	IconDisarm
**		Handle Disarm action.
*/
static void 
IconDisarm(
        Widget w,
        XEvent *event )
{
	DtIconGadget	g = 	(DtIconGadget) w;

	if (! G_Armed (g) || G_Behavior (g) == XmICON_LABEL)
		return;

	G_Armed (g) = False;

if (G_Behavior (g) == XmICON_DRAG)
	{
	  CallCallbackProc call_callback;
	  XtExposeProc expose;

	  _DtProcessLock();
	  call_callback = C_CallCallback(XtClass(g));
	  expose = XtCoreProc(w, expose);
	  _DtProcessUnlock();
	  (*call_callback) (g, G_Callback (g), XmCR_DISARM, event);
	  (*expose) ((Widget)g, event, NULL);
	}
}



/*-------------------------------------------------------------
**	IconActivate
**		Handle Activate action.
*/
static void 
IconActivate(
        Widget w,
        XEvent *event )
{
	DtIconGadget		g = 	(DtIconGadget) w;
	unsigned long		delay;
	XButtonEvent *		b_event = (XButtonEvent *) event;
	CallCallbackProc	call_callback;
	XtExposeProc		expose;

	if (! G_Armed (g))
		return;

	_DtProcessLock();
	call_callback = C_CallCallback(XtClass(g));
	expose = XtCoreProc(w, expose);
	_DtProcessUnlock();
	if (G_Behavior (g) == XmICON_BUTTON)
	{
		G_Armed (g) = False;
		(*call_callback) (g, G_Callback (g), XmCR_ACTIVATE, event);
		(*expose) ((Widget)g, event, NULL);
	}

	else if (G_Behavior (g) == XmICON_TOGGLE)
	{
		G_Armed (g) = False;
		G_Set (g) = ! G_Set (g);
		(*call_callback) (g, G_Callback (g), XmCR_VALUE_CHANGED, event);
	}

	else if (G_Behavior (g) == XmICON_DRAG)
	{
		if (G_ClickTimerID (g))
		{
			G_ClickTimerID (g) = 0;
			XtFree ((char *)G_ClickEvent (g));
			G_ClickEvent (g) = NULL;
			G_Armed (g) = False;
			(*call_callback) (g, G_Callback (g),
					XmCR_DEFAULT_ACTION, event);
		}
		else
		{
			delay = (unsigned long)
				XtGetMultiClickTime (XtDisplay (g)); 
			G_ClickEvent (g) = (XButtonEvent *)
				XtMalloc (sizeof (XButtonEvent));
			*(G_ClickEvent (g)) = *b_event;
			G_Sync (g) = True;
			G_ClickTimerID (g) = 
				XtAppAddTimeOut (
					XtWidgetToApplicationContext ((Widget)g),
					delay, (XtTimerCallbackProc)ClickTimeout, 
					(XtPointer) g);
		}

                if (G_ShadowThickness (g) > 0)
		   (*expose) ((Widget)g, event, NULL);
	}
}



/*-------------------------------------------------------------
**	IconDrag
**		Handle Drag action.
*/
static void 
IconDrag(
        Widget w,
        XEvent *event )
{
	DtIconGadget	g = 	(DtIconGadget) w;

	if (G_Behavior (g) == XmICON_DRAG)
	  {
	    CallCallbackProc call_callback;

	    _DtProcessLock();
	    call_callback = C_CallCallback(XtClass(w));
	    _DtProcessUnlock();
	    (*call_callback) (g, G_Callback (g), XmCR_DRAG, event);
	  }
}


/*-------------------------------------------------------------
**      IconPopup
**            Handle button 3 - popup's
*/
static void
IconPopup(
        Widget w,
        XEvent *event )
{
        DtIconGadget   g =     (DtIconGadget) w;
	CallCallbackProc	call_callback;

	_DtProcessLock();
	call_callback = C_CallCallback(XtClass(w));
	_DtProcessUnlock();
        (*call_callback) (g, G_Callback (g), XmCR_POPUP, event);
}



/*-------------------------------------------------------------
**	IconEnter
**		Handle Enter action.
*/
static void 
IconEnter(
        Widget w,
        XEvent *event )
{
	DtIconGadget	g = 	(DtIconGadget) w;

        _XmEnterGadget (w, (XEvent *)event,
                                 (String *)NULL,(Cardinal *)0);

	if (G_Armed (g))
		{
		if ((G_Behavior (g) == XmICON_BUTTON) ||
		    (G_Behavior (g) == XmICON_TOGGLE))
		  {
		    XtExposeProc expose;
		    _DtProcessLock();
		    expose = XtCoreProc(w, expose);
		    _DtProcessUnlock();
		    (*expose) ((Widget)g, event, NULL);
		  }
		}
}


/*-------------------------------------------------------------
**	IconLeave
**		Handle Leave action.
*/
static void 
IconLeave(
        Widget w,
        XEvent *event )
{
	DtIconGadget	g = 	(DtIconGadget) w;

	_XmLeaveGadget (w, (XEvent *)event,
                               (String *)NULL,  (Cardinal *)0);
	if (G_Armed (g))
	{
	if ((G_Behavior (g) == XmICON_BUTTON) ||
	    (G_Behavior (g) == XmICON_TOGGLE))
		{
		  XtExposeProc expose;

		  _DtProcessLock();
		  expose = XtCoreProc(w, expose);
		  _DtProcessUnlock();

		  G_Armed (g) = False;
		  (*expose) ((Widget)g, event, NULL);
		  G_Armed (g) = True;
		}
	}
}



/*-------------------------------------------------------------
**	Core Procs
**-------------------------------------------------------------
*/

/*-------------------------------------------------------------
**	ClassInitialize
**		Initialize gadget class.
*/
static void 
ClassInitialize( void )
{
	_DtRegisterNewConverters ();

	iconBaseClassExtRec.record_type = XmQmotif;
}

/*-------------------------------------------------------------
**	ClassPartInitialize
**		Initialize gadget class part.
*/
static void
ClassPartInitialize (
	WidgetClass	wc)
{
	DtIconGadgetClass ic =	(DtIconGadgetClass) wc;
	DtIconGadgetClass super = (DtIconGadgetClass) ic->rect_class.superclass;

	if (C_GetSize (ic) == DtInheritGetSize)
		C_GetSize (ic) = C_GetSize (super);
	if (C_GetPositions (ic) == DtInheritGetPositions)
		C_GetPositions (ic) = C_GetPositions (super);
	if (C_Draw (ic) == DtInheritDraw)
		C_Draw (ic) = C_Draw (super);
	if (C_CallCallback (ic) == DtInheritCallCallback)
		C_CallCallback (ic) = C_CallCallback (super);
	if (C_UpdateGCs (ic) == DtInheritUpdateGCs)
		C_UpdateGCs (ic) = C_UpdateGCs (super);
}

/*-------------------------------------------------------------
**	Cache Procs
**-------------------------------------------------------------
*/

/*-------------------------------------------------------------
**      IconCacheCompare
**
*/
static int 
IconCacheCompare(
        XtPointer A,
        XtPointer B )
{
   DtIconCacheObjPart *icon_inst = (DtIconCacheObjPart *) A ;
   DtIconCacheObjPart *icon_cache_inst = (DtIconCacheObjPart *) B ;

   if ((icon_inst->fill_on_arm == icon_cache_inst->fill_on_arm) &&
       (icon_inst->recompute_size== icon_cache_inst->recompute_size) &&
       (icon_inst->pixmap_position== icon_cache_inst->pixmap_position) &&
       (icon_inst->string_position== icon_cache_inst->string_position) &&
       (icon_inst->alignment == icon_cache_inst->alignment) &&
       (icon_inst->behavior == icon_cache_inst->behavior) &&
       (icon_inst->draw_shadow == icon_cache_inst->draw_shadow) &&
       (icon_inst->fill_mode == icon_cache_inst->fill_mode) &&
       (icon_inst->margin_width== icon_cache_inst->margin_width) &&
       (icon_inst->margin_height== icon_cache_inst->margin_height) &&
       (icon_inst->string_height== icon_cache_inst->string_height) &&
       (icon_inst->spacing== icon_cache_inst->spacing) &&
       (icon_inst->foreground== icon_cache_inst->foreground) &&
       (icon_inst->background== icon_cache_inst->background) &&
       (icon_inst->arm_color== icon_cache_inst->arm_color))
       return 1;
   else
       return 0;

}


/*-------------------------------------------------------------
**      SecondaryObjectCreate
**
*/
static void 
SecondaryObjectCreate(
        Widget req,
        Widget new_w,
        ArgList args,
        Cardinal *num_args )
{
    XmBaseClassExt              *cePtr;
    XmWidgetExtData             extData;
    WidgetClass                 wc;
    Cardinal                    size;
    XtPointer                   newSec, reqSec;
    XtResourceList		resources;
    Cardinal			num_resources;

    _DtProcessLock();
    cePtr = _XmGetBaseClassExtPtr(XtClass(new_w), XmQmotif);
    wc = (*cePtr)->secondaryObjectClass;
    if (NULL == wc) return;
    size = wc->core_class.widget_size;
    resources = wc->core_class.resources;
    num_resources = wc->core_class.num_resources;

    newSec = _XmExtObjAlloc(size);
    reqSec = _XmExtObjAlloc(size);
    _DtProcessUnlock();

    /*
     * fetch the resources in superclass to subclass order
     */

    XtGetSubresources(new_w, newSec, NULL, NULL, 
		      resources, num_resources, args, *num_args );

    extData = (XmWidgetExtData) XtCalloc(sizeof(XmWidgetExtDataRec), 1);
    extData->widget = (Widget)newSec;
    extData->reqWidget = (Widget)reqSec;

    ((DtIconCacheObject)newSec)->ext.extensionType = XmCACHE_EXTENSION;
    ((DtIconCacheObject)newSec)->ext.logicalParent = new_w;

    _XmPushWidgetExtData(new_w, extData,
                         ((DtIconCacheObject)newSec)->ext.extensionType);
    memcpy(reqSec, newSec, size);

    /*
     * fill out cache pointers
     */
    Icon_Cache(new_w) = &(((DtIconCacheObject)extData->widget)->icon_cache);
    Icon_Cache(req) = &(((DtIconCacheObject)extData->reqWidget)->icon_cache);

}


/*-------------------------------------------------------------
**      InitializePostHook
**
*/
/* ARGSUSED */
static void 
InitializePosthook(
        Widget req,
        Widget new,
        ArgList args,
        Cardinal *num_args )
{
    XmWidgetExtData     ext;
    DtIconGadget lw = (DtIconGadget)new;

    /*
     * - register parts in cache.
     * - update cache pointers
     * - and free req
     */

    _DtProcessLock();
    Icon_Cache(lw) = (DtIconCacheObjPart *)
           _XmCachePart( Icon_ClassCachePart(lw),
                         (XtPointer) Icon_Cache(lw),
                         sizeof(DtIconCacheObjPart));

    /*
     * might want to break up into per-class work that gets explicitly
     * chained. For right now, each class has to replicate all
     * superclass logic in hook routine
     */

    /*     * free the req subobject used for comparisons
     */
    _XmPopWidgetExtData((Widget) lw, &ext, XmCACHE_EXTENSION);
    _XmExtObjFree((XtPointer)ext->widget);
    _XmExtObjFree((XtPointer)ext->reqWidget);
    _DtProcessUnlock();
    XtFree( (char *) ext);
}

/*-------------------------------------------------------------
**	SetValuesPrehook
**
*/
/* ARGSUSED */
static Boolean 
SetValuesPrehook(
        Widget oldParent,
        Widget refParent,
        Widget newParent,
        ArgList args,
        Cardinal *num_args )
{
    XmWidgetExtData             extData;
    XmBaseClassExt              *cePtr;
    WidgetClass                 ec;
    DtIconCacheObject          newSec, reqSec;
    Cardinal                    size;
    XtResourceList		resources;
    Cardinal			num_resources;

    _DtProcessLock();
    cePtr = _XmGetBaseClassExtPtr(XtClass(newParent), XmQmotif);
    ec = (*cePtr)->secondaryObjectClass;
    size = ec->core_class.widget_size;
    resources = ec->core_class.resources;
    num_resources = ec->core_class.num_resources;

    /* allocate copies and fill from cache */
    newSec = (DtIconCacheObject) _XmExtObjAlloc(size);
    reqSec = (DtIconCacheObject) _XmExtObjAlloc(size);
    _DtProcessUnlock();

    newSec->object.self = (Widget)newSec;
    newSec->object.widget_class = ec;
    newSec->object.parent = XtParent(newParent);
    newSec->object.xrm_name = newParent->core.xrm_name;
    newSec->object.being_destroyed = False;
    newSec->object.destroy_callbacks = NULL;
    newSec->object.constraints = NULL;

    newSec->ext.logicalParent = newParent;
    newSec->ext.extensionType = XmCACHE_EXTENSION;

    memcpy(&(newSec->icon_cache),
           Icon_Cache(newParent),
           sizeof(DtIconCacheObjPart));

    extData = (XmWidgetExtData) XtCalloc(sizeof(XmWidgetExtDataRec), 1);
    extData->widget = (Widget)newSec;
    extData->reqWidget = (Widget)reqSec;
    _XmPushWidgetExtData(newParent, extData, XmCACHE_EXTENSION);

    _XmGadgetImportSecondaryArgs(newParent, args, num_args);
    XtSetSubvalues((XtPointer)newSec, resources, num_resources,
		   args, *num_args);

    _XmExtImportArgs((Widget)newSec, args, num_args);

    memcpy((XtPointer)reqSec, (XtPointer)newSec, size);

    Icon_Cache(newParent) = &(((DtIconCacheObject)newSec)->icon_cache);
    Icon_Cache(refParent) =
			&(((DtIconCacheObject)extData->reqWidget)->icon_cache);

    return FALSE;
}


/*-------------------------------------------------------------
**	GetValuesPrehook
**
*/
static void 
GetValuesPrehook(
        Widget newParent,
        ArgList args,
        Cardinal *num_args )
{
    XmWidgetExtData             extData;
    XmBaseClassExt              *cePtr;
    WidgetClass                 ec;
    DtIconCacheObject          newSec;
    Cardinal                    size;
    XtResourceList		resources;
    Cardinal			num_resources;

    _DtProcessLock();
    cePtr = _XmGetBaseClassExtPtr(XtClass(newParent), XmQmotif);
    ec = (*cePtr)->secondaryObjectClass;
    size = ec->core_class.widget_size;
    resources = ec->core_class.resources;
    num_resources = ec->core_class.num_resources;

    newSec = (DtIconCacheObject)_XmExtObjAlloc(size);
    _DtProcessUnlock();

    newSec->object.self = (Widget)newSec;
    newSec->object.widget_class = ec;
    newSec->object.parent = XtParent(newParent);
    newSec->object.xrm_name = newParent->core.xrm_name;
    newSec->object.being_destroyed = False;
    newSec->object.destroy_callbacks = NULL;
    newSec->object.constraints = NULL;

    newSec->ext.logicalParent = newParent;
    newSec->ext.extensionType = XmCACHE_EXTENSION;

    memcpy( &(newSec->icon_cache),
            Icon_Cache(newParent),
            sizeof(DtIconCacheObjPart));

    extData = (XmWidgetExtData) XtCalloc(sizeof(XmWidgetExtDataRec), 1);
    extData->widget = (Widget)newSec;
    _XmPushWidgetExtData(newParent, extData, XmCACHE_EXTENSION);

    XtGetSubvalues((XtPointer)newSec, resources, num_resources,
                   args, *num_args);

    _XmExtGetValuesHook((Widget)newSec, args, num_args);
}



/*-------------------------------------------------------------
**	GetValuesPosthook
**
*/
/* ARGSUSED */
static void 
GetValuesPosthook(
        Widget new,
        ArgList args,
        Cardinal *num_args )
{
 XmWidgetExtData             ext;

 _XmPopWidgetExtData(new, &ext, XmCACHE_EXTENSION);

 _DtProcessLock();
 _XmExtObjFree((XtPointer)ext->widget);
 _DtProcessUnlock();

 XtFree((char *)ext);
}



/*-------------------------------------------------------------
**	SetValuesPosthook
**
*/
/* ARGSUSED */
static Boolean 
SetValuesPosthook(
        Widget current,
        Widget req,
        Widget new,
        ArgList args,
        Cardinal *num_args )
{
    XmWidgetExtData             ext;

    /*
     * - register parts in cache.
     * - update cache pointers
     * - and free req
     */


      /* assign if changed! */
      _DtProcessLock();
      if (!IconCacheCompare((XtPointer)Icon_Cache(new),
			    (XtPointer)Icon_Cache(current)))

      {
         _XmCacheDelete((XtPointer) Icon_Cache(current)); /* delete the old one */
          Icon_Cache(new) = (DtIconCacheObjPart *)
              _XmCachePart(Icon_ClassCachePart(new),
                           (XtPointer) Icon_Cache(new),
                           sizeof(DtIconCacheObjPart));
      } else
          Icon_Cache(new) = Icon_Cache(current);

      _XmPopWidgetExtData(new, &ext, XmCACHE_EXTENSION);

      _XmExtObjFree((XtPointer)ext->widget);
      _XmExtObjFree((XtPointer)ext->reqWidget);
      _DtProcessUnlock();

      XtFree((char *)ext);

      return FALSE;
}


/*--------------------------------------------------------------------------
**	QualifyIconLocalCache
**		Checks to see if local cache is set up
*/
static void 
QualifyIconLocalCache(DtIconGadget g, DtIconCacheObjPart *local_cache)
{
  _DtProcessLock();
  ClassCacheCopy(Icon_ClassCachePart(g))((XtPointer) Icon_Cache(g),
					 (XtPointer) local_cache,
					 sizeof(DtIconCacheObjPart));
  _DtProcessUnlock();
}

/************************************************************************
 *
 * ReCacheIcon_r()
 * Check to see if ReCaching is necessary as a result of fields having
 * been set by a mananger widget. This routine is called by the
 * manager widget in their SetValues after a change is made to any
 * of Icon's cached fields.
 *
 ************************************************************************/

static void 
ReCacheIcon_r(DtIconCacheObjPart *local_cache, DtIconGadget g)
{
  if (!IconCacheCompare( (XtPointer)local_cache, (XtPointer)Icon_Cache(g)))
    {
      _DtProcessLock();
      _XmCacheDelete( (XtPointer) Icon_Cache(g));   /* delete the old one */
      Icon_Cache(g) = (DtIconCacheObjPart *)
	_XmCachePart(Icon_ClassCachePart(g), 
		     (XtPointer) local_cache,
		     sizeof(DtIconCacheObjPart)); 
      _DtProcessUnlock();
    }
}


/*********************************************************************
 *
 *  GetParentBackgroundGC
 *      Get the graphics context used for erasing their highlight border.
 *
 *********************************************************************/
static void
GetParentBackgroundGC(
        DtIconGadget g )
{
   XGCValues values;
   XtGCMask  valueMask;
   Widget    parent = XtParent((Widget)g);

   valueMask = GCForeground | GCBackground;
   values.foreground = parent->core.background_pixel;

   if (XmIsManager(parent))

      values.background = ((XmManagerWidget) parent)->manager.foreground;
   else
      values.background = ((XmPrimitiveWidget) parent)->primitive.foreground;

   if ((parent->core.background_pixmap != None) &&
       (parent->core.background_pixmap != XmUNSPECIFIED_PIXMAP))
   {
      valueMask |= GCFillStyle | GCTile;
      values.fill_style = FillTiled;
      values.tile = parent->core.background_pixmap;
   }

   G_SavedParentBG(g) = parent->core.background_pixel;

   G_ParentBackgroundGC(g) = XtGetGC (parent, valueMask, &values);
}


/*-------------------------------------------------------------
**	Initialize
**		Initialize a new gadget instance.
*/
static void 
Initialize(
        Widget request_w,
        Widget new_w )
{
	DtIconGadget	request =	(DtIconGadget) request_w,
			new =		(DtIconGadget) new_w;
	Window		root;
	int		int_x = 0, int_y = 0;
	unsigned int	int_w = 0, int_h = 0,
			int_bw, depth;
	Dimension	w, h;
	EventMask	mask;
	String		name = NULL;
	UpdateGCsProc	update_gcs;

	G_Sync (new) = False;

/*	Validate behavior.
*/
	if (G_Behavior (new) != XmICON_LABEL &&
	    G_Behavior (new) != XmICON_BUTTON &&
	    G_Behavior (new) != XmICON_TOGGLE &&
	    G_Behavior (new) != XmICON_DRAG)
	{
		XmeWarning ((Widget)new, WARN_BEHAVIOR);
		G_Behavior (new) = XmICON_BUTTON;
	}

/*	Set the input mask for events handled by Manager.
*/
	G_EventMask (new) = (XmARM_EVENT | XmACTIVATE_EVENT |
			XmMULTI_ARM_EVENT | XmMULTI_ACTIVATE_EVENT |
			XmHELP_EVENT | XmFOCUS_IN_EVENT | XmKEY_EVENT |
			XmFOCUS_OUT_EVENT | XmENTER_EVENT | XmLEAVE_EVENT);

/*	Add event handler for icon events.
*/
	if (G_Behavior (new) == XmICON_DRAG)
	{
		mask = ButtonPressMask|ButtonReleaseMask;
		XtAddEventHandler (XtParent (new), mask, False,
				(XtEventHandler) IconEventHandler, 0);
	}

	G_ClickTimerID (new) = 0;
	G_ClickEvent (new) = NULL;

	G_Armed (new) = False;

	G_Mask (new) = None;

	if (G_Pixmap (new) == XmUNSPECIFIED_PIXMAP)
		G_Pixmap (new) = None;

	if (G_ImageName (new) || G_Pixmap (new))
	{
		if (G_ImageName (new))
		{

/*	Try to get pixmap from image name.
*/
			G_Pixmap (new) = 
				XmGetPixmap (XtScreen (new), G_ImageName (new),
					G_PixmapForeground (new),
					G_PixmapBackground (new));
			if (G_Pixmap (new) != XmUNSPECIFIED_PIXMAP) 
			  {
			      name = G_ImageName (new);
			      G_Mask (new) = 
				_DtGetMask(XtScreen (new), G_ImageName
					   (new));
			  }
			else
			{
/* warning? */				
				name = NULL;
				G_Pixmap (new) = None;
			}
		}

/*	Update width and height; copy image name.
*/
		if (G_Pixmap (new))
		{
			XGetGeometry (XtDisplay (new), G_Pixmap (new),
				&root, &int_x, &int_y, &int_w, &int_h,
				&int_bw, &depth);
		}
		if (name)
		{
			G_ImageName (new) = XtNewString(name);
		}
		else
			G_ImageName (new) = NULL;
	}
	G_PixmapWidth(new) = Limit((Dimension) int_w, G_MaxPixmapWidth(new));
	G_PixmapHeight(new) = Limit((Dimension) int_h, G_MaxPixmapHeight(new));

/*	Validate fill mode.
*/
	if (G_FillMode (new) != XmFILL_NONE &&
	    G_FillMode (new) != XmFILL_PARENT &&
	    G_FillMode (new) != XmFILL_TRANSPARENT &&
	    G_FillMode (new) != XmFILL_SELF)
	{
		XmeWarning ((Widget)new, WARN_FILL_MODE);
		if (G_ShadowThickness (new) > 0)
			G_FillMode (new) = XmFILL_SELF;
		else
			G_FillMode (new) = XmFILL_PARENT;
	}

/*	Validate pixmap position.
*/
	if (G_StringPosition (new) != UNSPECIFIED_CHAR)
		G_PixmapPosition (new) = G_StringPosition (new);

	if (G_PixmapPosition (new) == UNSPECIFIED_CHAR)
		G_PixmapPosition (new) = XmPIXMAP_LEFT;
	else if (G_PixmapPosition (new) != XmPIXMAP_LEFT &&
		 G_PixmapPosition (new) != XmPIXMAP_RIGHT &&
		 G_PixmapPosition (new) != XmPIXMAP_TOP &&
		 G_PixmapPosition (new) != XmPIXMAP_BOTTOM &&
		 G_PixmapPosition (new) != XmPIXMAP_MIDDLE)
	{
		XmeWarning ((Widget)new, WARN_PIXMAP_POSITION);
		G_PixmapPosition (new) = XmPIXMAP_LEFT;
	}
	G_StringPosition (new) = G_PixmapPosition (new);

/*	Validate alignment.
*/
	if (G_Alignment (new) != XmALIGNMENT_BEGINNING &&
	    G_Alignment (new) != XmALIGNMENT_CENTER &&
	    G_Alignment (new) != XmALIGNMENT_END)
	{
		XmeWarning ((Widget)new, WARN_ALIGNMENT);
		G_Alignment (new) = XmALIGNMENT_BEGINNING;
	}

/*	Validate shadow type.
*/
	if (G_ShadowType (new) != XmSHADOW_IN &&
	    G_ShadowType (new) != XmSHADOW_OUT &&
	    G_ShadowType (new) != XmSHADOW_ETCHED_IN &&
	    G_ShadowType (new) != XmSHADOW_ETCHED_OUT)
	{
		XmeWarning ((Widget)new, WARN_SHADOW_TYPE);
		if (G_Behavior (new) == XmICON_BUTTON)
			G_ShadowType (new) = XmSHADOW_OUT;
		else if (G_Behavior (new) == XmICON_TOGGLE)
			G_ShadowType (new) = (G_Set (new))
				? XmSHADOW_ETCHED_IN : XmSHADOW_ETCHED_OUT;
	}

/*	Copy fontlist.
*/
	if (G_FontList (new) == NULL)
		G_FontList (new) =
			XmeGetDefaultRenderTable ((Widget)new, XmBUTTON_FONTLIST);
	G_FontList (new) = XmFontListCopy (G_FontList (new));

	if (G_String (new) == XmUNSPECIFIED_STRING)
		G_String (new) = (_XmString) NULL;
       
	if (G_String (new))
	{
		G_String (new) = XmStringCopy (G_String (new));
		XmStringExtent (G_FontList (new), G_String (new),
					&w, &h);
                if (G_Underline(new))
                   h++;
	}
	else
		w = h = 0;

	G_StringWidth (new) = w;
	G_StringHeight (new) = h;

/*	Convert margins to pixel units.
*/
	if (G_UnitType (new) != XmPIXELS)
	{
		G_MarginWidth (new) = 
			XmeToHorizontalPixels ((Widget)new, G_UnitType (new),
					(XtArgVal *)G_MarginWidth (new));
		G_MarginHeight (new) = 
			XmeToVerticalPixels ((Widget)new, G_UnitType (new),
					(XtArgVal *)G_MarginHeight (new));
	}

/*	Check for unspecified margins.
*/
	if (G_MarginWidth (request) == UNSPECIFIED_DIMENSION)
		G_MarginWidth (new) = MARGIN_DEFAULT;
	if (G_MarginHeight (request) == UNSPECIFIED_DIMENSION)
		G_MarginHeight (new) = MARGIN_DEFAULT;

/*	Convert spacing.
*/
	if (G_Spacing (new) == UNSPECIFIED_DIMENSION)
	{
		G_Spacing (new) = G_StringHeight (new) / 5;
		if (G_Spacing (new) < SPACING_DEFAULT)
			G_Spacing (new) = SPACING_DEFAULT;
	}
	else if (G_Spacing (new) && G_UnitType (new) != XmPIXELS)
	{
		G_Spacing (new) = 
			(G_PixmapPosition (new) == XmPIXMAP_LEFT ||
			 G_PixmapPosition (new) == XmPIXMAP_RIGHT)
			? XmeToHorizontalPixels ((Widget)new, G_UnitType (new),
					(XtArgVal *)G_Spacing (new))
			: XmeToVerticalPixels ((Widget)new, G_UnitType (new),
					(XtArgVal *)G_Spacing (new));
	}

/*	Set width and height.
*/
	if (G_Width (request) == 0 || G_Height (request) == 0)
	{
	  	GetSizeProc get_size;

		_DtProcessLock();
		get_size = C_GetSize(XtClass(new));
		_DtProcessUnlock();
		(*get_size) (new, &w, &h);

		if (G_Width (request) == 0)
			G_Width (new) = w;
		if (G_Height (request) == 0)
			G_Height (new) = h;
	}

/*  	Get graphics contexts.
*/
	G_NormalGC (new) = NULL;
	G_ClipGC (new) = NULL;
	G_BackgroundGC (new) = NULL;
	G_ArmedGC (new) = NULL;
	G_ArmedBackgroundGC (new) = NULL;
	_DtProcessLock();
	update_gcs = C_UpdateGCs(XtClass(new));
	_DtProcessUnlock();
	(*update_gcs) (new);

        GetParentBackgroundGC(new);

        if (G_Operations(new) != XmDROP_NOOP) {
	   _DtIconRegisterDropsite(new_w);
        }
}



/*-------------------------------------------------------------
**	Destroy
**		Release resources allocated for gadget.
*/
static void 
Destroy(
        Widget w )
{
	DtIconGadget	g =	(DtIconGadget) w;
	XmManagerWidget mw = (XmManagerWidget) XtParent(g);

	if (G_ClickTimerID (g))
		XtRemoveTimeOut (G_ClickTimerID (g));

	XtFree ((char *)G_ClickEvent (g));

	if (G_String (g) != NULL)
		XmStringFree (G_String (g));

	if (G_ImageName (g) != NULL)
	{
		XtFree (G_ImageName (g));
		if (G_Mask (g) != XmUNSPECIFIED_PIXMAP)
		  XmDestroyPixmap (XtScreen(w),G_Mask (g));
		XmDestroyPixmap (XtScreen(w),G_Pixmap (g));
	}

	XmFontListFree (G_FontList (g));

	_DtProcessLock();
	_XmCacheDelete((XtPointer) Icon_Cache(w));
	_DtProcessUnlock();

	XtReleaseGC ((Widget)mw, G_NormalGC (g));
	XtReleaseGC ((Widget)mw, G_ClipGC (g));
	XtReleaseGC ((Widget)mw, G_BackgroundGC (g));
	XtReleaseGC ((Widget)mw, G_ArmedGC (g));
	XtReleaseGC ((Widget)mw, G_ArmedBackgroundGC (g));

/* remove event handler if last Icon in parent? */
}



/*-------------------------------------------------------------
**	Resize
**		Set clip rect?
*/
/* ARGSUSED */
static void 
Resize(
        Widget w )
{
}


/*-------------------------------------------------------------
**	Redisplay
**		Redisplay gadget.
*/
/* ARGSUSED */
static void 
Redisplay(
        Widget w,
        XEvent *event,
        Region region )
{
	DtIconGadget	g =		(DtIconGadget) w;
	Dimension	s_t =		G_ShadowThickness (g);
	unsigned char	fill_mode =	G_FillMode (g);
	DrawProc	draw;

	if (! XtIsManaged (w))
		return;

/*	Draw gadget to window.
*/
	_DtProcessLock();
	draw = C_Draw(XtClass(g));
	_DtProcessUnlock();
	(*draw) (g, XtWindow (g), G_X (g), G_Y (g), G_Width (g), G_Height (g),
		 G_HighlightThickness (g), s_t, G_ShadowType (g), fill_mode);

/*	Draw highlight if highlighted.
*/
	if (G_Highlighted (g))
                BorderHighlight( (DtIconGadget)g );
}



/*-------------------------------------------------------------
**	SetValues
**		
*/
/* ARGSUSED */
static Boolean 
SetValues(
        Widget current_w,
        Widget request_w,
        Widget new_w )
{
	DtIconGadget	current =	(DtIconGadget) current_w,
			new =		(DtIconGadget) new_w;

	Window		root;
	int		int_x = 0, int_y = 0;
	unsigned int	int_w = 0, int_h = 0,
			int_bw, depth;
	Dimension	w, h;
	Boolean		new_image_name = False,
			redraw_flag = False,
			draw_pixmap = False,
			draw_string = False,
			draw_shadow = False;
	Dimension	h_t = G_HighlightThickness (new),
			s_t = G_ShadowThickness (new),
			p_x, p_y, s_x, s_y;
	String		name = NULL;
	Boolean		optimize_redraw = False;

/*	If unchanged, reuse old image name
*/
	if (G_ImageName (current) != G_ImageName (new) &&
	    G_ImageName (new) &&
	    G_ImageName (current) &&
	    strcmp(G_ImageName (current), G_ImageName (new)) == 0)
	{
		G_ImageName (new) = G_ImageName (current);
	}

/*	Validate behavior
*/
	if (G_Behavior (new) != G_Behavior (current))
	{
		if (G_Behavior (new) != XmICON_LABEL &&
		    G_Behavior (new) != XmICON_BUTTON &&
		    G_Behavior (new) != XmICON_TOGGLE &&
		    G_Behavior (new) != XmICON_DRAG)
		{
			XmeWarning ((Widget)new, WARN_BEHAVIOR);
			G_Behavior (new) = G_Behavior (current);
		}

		if (G_Behavior (new) == XmICON_DRAG)
		{
			EventMask	mask;

			mask = ButtonPressMask|ButtonReleaseMask;
			XtAddEventHandler (XtParent (new), mask, False,
					(XtEventHandler) IconEventHandler, 0);
		}
	}

/*	Reset the interesting input types.
*/
	G_EventMask (new) |= (XmARM_EVENT | XmACTIVATE_EVENT |
			XmMULTI_ARM_EVENT | XmMULTI_ACTIVATE_EVENT |
			XmHELP_EVENT | XmFOCUS_IN_EVENT | XmKEY_EVENT |
			XmFOCUS_OUT_EVENT | XmENTER_EVENT | XmLEAVE_EVENT);

/*	Check for new image name.
*/
	if (G_ImageName (new) && (G_ImageName (current) != G_ImageName (new)))
		new_image_name = True;

/*	Validate shadow type.
*/
	if ((G_ShadowType (new) != G_ShadowType (current)) ||
	    (G_Behavior (new) == XmICON_TOGGLE &&
	     G_Set (new) != G_Set (current)))
	{
		if (G_ShadowType (new) != XmSHADOW_IN &&
		    G_ShadowType (new) != XmSHADOW_OUT &&
		    G_ShadowType (new) != XmSHADOW_ETCHED_IN &&
		    G_ShadowType (new) != XmSHADOW_ETCHED_OUT)
		{
			XmeWarning ((Widget)new, WARN_SHADOW_TYPE);
			G_ShadowType (new) = G_ShadowType (current); 
		}

/*	Disallow change if conflict with set or armed state.
*/
		else if (((G_Behavior (new) == XmICON_TOGGLE) &&
			  ((G_Set (new) && ! G_Armed (new)) ||
			   (! G_Set (new) && G_Armed (new)))) ||
			 ((G_Behavior (new) == XmICON_BUTTON) &&
			  (G_Armed (new))))
		{
			if (G_ShadowType (new) == XmSHADOW_OUT)
				G_ShadowType (new) = XmSHADOW_IN;
			else if (G_ShadowType (new) == XmSHADOW_ETCHED_OUT)
				G_ShadowType (new) = XmSHADOW_ETCHED_IN;
		}
		else if (((G_Behavior (new) == XmICON_TOGGLE) &&
			  ((G_Set (new) && G_Armed (new)) ||
			   (! G_Set (new) && ! G_Armed (new)))) ||
			 ((G_Behavior (new) == XmICON_BUTTON) &&
			  (! G_Armed (new))))
		{
			if (G_ShadowType (new) == XmSHADOW_IN)
				G_ShadowType (new) = XmSHADOW_OUT;
			else if (G_ShadowType (new) == XmSHADOW_ETCHED_IN)
				G_ShadowType (new) = XmSHADOW_ETCHED_OUT;
		}

		if (G_ShadowType (new) != G_ShadowType (current))
			draw_shadow = True;
	}

/*	Validate alignment.
*/
	if (G_Alignment (new) != G_Alignment (current))
	{
		if (G_Alignment (new) != XmALIGNMENT_BEGINNING &&
		    G_Alignment (new) != XmALIGNMENT_CENTER &&
		    G_Alignment (new) != XmALIGNMENT_END)
		{
			XmeWarning ((Widget)new, WARN_ALIGNMENT);
			G_Alignment (new) = G_Alignment (current);
		}
		else
			redraw_flag = True;
	}


/*	Validate fill mode.
*/
	if (G_FillMode (new) != G_FillMode (current))
	{
		if (G_FillMode (new) != XmFILL_NONE &&
		    G_FillMode (new) != XmFILL_PARENT &&
		    G_FillMode (new) != XmFILL_TRANSPARENT &&
		    G_FillMode (new) != XmFILL_SELF)
		{
			XmeWarning ((Widget)new, WARN_FILL_MODE);
			G_FillMode (new) = G_FillMode (current);
		}
	}

/*	Validate pixmap position.
*/
	if (G_StringPosition (new) != G_StringPosition (current))
		G_PixmapPosition (new) = G_StringPosition (new);

	if (G_PixmapPosition (new) != G_PixmapPosition (current))
	{
		if (G_PixmapPosition (new) != XmPIXMAP_LEFT &&
		    G_PixmapPosition (new) != XmPIXMAP_RIGHT &&
		    G_PixmapPosition (new) != XmPIXMAP_TOP &&
		    G_PixmapPosition (new) != XmPIXMAP_BOTTOM &&
		    G_PixmapPosition (new) != XmPIXMAP_MIDDLE)
		{
			XmeWarning ((Widget)new, WARN_PIXMAP_POSITION);
			G_PixmapPosition (new) = G_PixmapPosition (current); 
		}
		else
			redraw_flag = True;

		G_StringPosition (new) = G_PixmapPosition (new);
	}

/*	Update pixmap if pixmap foreground or background changed.
*/
	if (G_PixmapForeground (current) != G_PixmapForeground (new) ||
	    G_PixmapBackground (current) != G_PixmapBackground (new))
	{
		if (G_Pixmap (current) == G_Pixmap (new) &&
		    (G_ImageName (new) != NULL) &&
		    (! new_image_name))
		{
			draw_pixmap = True;
			if (G_Mask(new) != XmUNSPECIFIED_PIXMAP) 
			  XmDestroyPixmap( XtScreen(new), G_Mask(current));
			XmDestroyPixmap (XtScreen(new),G_Pixmap (current));
			G_Pixmap (new) = 
				XmGetPixmap (XtScreen (new), G_ImageName (new),
					G_PixmapForeground (new),
					G_PixmapBackground (new));
			if (G_Pixmap(new) != XmUNSPECIFIED_PIXMAP) 
			  G_Mask (new) = 
			    _DtGetMask(XtScreen (new), G_ImageName (new));
		}
	}


/*	Check for change in image name.
*/
	if (new_image_name)
	{

/*	Try to get pixmap from image name.
*/
		if (G_ImageName (current) != NULL) 
		  {
		      if (G_Mask(new) != XmUNSPECIFIED_PIXMAP)
			XmDestroyPixmap (XtScreen(new),G_Mask(current));
		      XmDestroyPixmap (XtScreen(new),G_Pixmap (current));
		  }
		G_Pixmap (new) = 
			XmGetPixmap (XtScreen (new), G_ImageName (new),
				G_PixmapForeground (new),
				G_PixmapBackground (new));

		if (G_Pixmap (new) != XmUNSPECIFIED_PIXMAP)
		{
    		    G_Mask(new) = (Pixmap)_DtGetMask(XtScreen(new), G_ImageName(new));
		    XGetGeometry (XtDisplay (new), G_Pixmap (new),
				  &root, &int_x, &int_y, &int_w, &int_h,
				  &int_bw, &depth);
		    name = G_ImageName (new);
		    w = Limit((Dimension) int_w, G_MaxPixmapWidth(new));
		    h = Limit((Dimension) int_h, G_MaxPixmapHeight(new));
		}
		else
		{
			name = NULL;
			G_Pixmap (new) = None;
			w = 0;
			h = 0;
		}

/*	If resetting to current image name, then reuse old copy.
*/
		if (name && G_ImageName (current)
		    && (! strcmp (G_ImageName (new), G_ImageName (current))))
		{
			G_ImageName (new) = G_ImageName (current);
			name = G_ImageName (current);
		}
		else 
		{
		    if (name)
		      G_ImageName (new) = XtNewString(name);
		    else
		      G_ImageName (new) = NULL;
		    if (G_ImageName (current))
		      XtFree (G_ImageName (current));
		}

		if (G_Pixmap (new) != G_Pixmap (current))
		{
			if ((G_Pixmap (new) != None) &&
			    (G_PixmapWidth (new) == w) &&
			    (G_PixmapHeight (new) == h))
			{
				draw_pixmap = True;
			}
			else
			{
				redraw_flag = True;
				G_PixmapWidth (new) = w;
				G_PixmapHeight (new) = h;
			}
		}
	}

/*	Release image name and pixmap if name set to null.
*/
	else if (G_Pixmap (new) == G_Pixmap (current))
	{
		if ((G_ImageName (current) != NULL) && 
		    (G_ImageName (new) == NULL))
		{
			redraw_flag = True;
			XtFree (G_ImageName (current));
			if (G_Mask(new) != XmUNSPECIFIED_PIXMAP)
			  XmDestroyPixmap (XtScreen(new),G_Mask (current));
			XmDestroyPixmap (XtScreen(new),G_Pixmap (current));
			G_Pixmap (new) = None;			
			G_PixmapWidth (new) = 0;
			G_PixmapHeight (new) = 0;
		}
	}

/*	Process change in pixmap.
*/
	else if (G_Pixmap (new) != G_Pixmap (current)) 
	{
		if (G_Pixmap (new))
		{
			XGetGeometry (XtDisplay (new), G_Pixmap (new), &root,
					&int_x, &int_y, &int_w, &int_h,
					&int_bw, &depth);
			w = Limit((Dimension) int_w, G_MaxPixmapWidth(new));
			h = Limit((Dimension) int_h, G_MaxPixmapHeight(new));
		}
		else
		{
			if (G_ImageName (current) != NULL)
			{
				XtFree (G_ImageName (current));
				if (G_Mask(new) != XmUNSPECIFIED_PIXMAP)
				  XmDestroyPixmap (XtScreen(new),G_Mask (current));
				XmDestroyPixmap (XtScreen(new),G_Pixmap (current));
				G_ImageName (new) = NULL;
			}
			w = h = 0;
		}

		if (G_Pixmap (new) &&
		    (G_PixmapWidth (new) == w) &&
		    (G_PixmapHeight (new) == h))
		{
			draw_pixmap = True;
		}
		else
		{
			redraw_flag = True;
			G_PixmapWidth (new) = w;
			G_PixmapHeight (new) = h;
		}
	}

        if( ( G_MaxPixmapWidth(new) != G_MaxPixmapWidth(current)) ||
             (G_MaxPixmapHeight(new) != G_MaxPixmapHeight(current)) )
        {
                if (G_Pixmap (new))
                {
                        XGetGeometry (XtDisplay (new), G_Pixmap (new), &root,
                                        &int_x, &int_y, &int_w, &int_h,
                                        &int_bw, &depth);
                        w = Limit((Dimension) int_w, G_MaxPixmapWidth(new));
                        h = Limit((Dimension) int_h, G_MaxPixmapHeight(new));
                }
                else
                {
                        w = h = 0;
                }

                if (G_Pixmap (new) &&
                    (G_PixmapWidth (new) == w) &&
                    (G_PixmapHeight (new) == h))
                {
                        draw_pixmap = True;
                }
                else
                {
                        redraw_flag = True;
                        G_PixmapWidth (new) = w;
                        G_PixmapHeight (new) = h;
                }
        }

			
/*	Update GCs if foreground, background or mask changed.
*/
	if (G_Foreground (current) != G_Foreground (new) ||
	    G_Background (current) != G_Background (new) ||
	    ((G_Mask (current) != G_Mask(new)) &&
	     (G_Pixmap (current) != G_Pixmap (new))) ||
	    G_ArmColor (current) != G_ArmColor (new))
	{
		UpdateGCsProc update_gcs;
		if (G_ShadowThickness (new) > 0 &&
		    G_Behavior(new) != XmICON_DRAG &&
		    G_Background (current) != G_Background (new))
			redraw_flag = True;
		else
			draw_string = True;
		_DtProcessLock();
		update_gcs = C_UpdateGCs(XtClass(new));
		_DtProcessUnlock();
		(*update_gcs) (new);
	}

/*	Convert dimensions to pixel units.
*/
	if (G_UnitType (new) != XmPIXELS)
	{
		G_MarginWidth (new) = 
			XmeToHorizontalPixels ((Widget)new, G_UnitType (new),
					(XtArgVal *)G_MarginWidth (new));
		G_MarginHeight (new) = 
			XmeToVerticalPixels ((Widget)new, G_UnitType (new),
					(XtArgVal *)G_MarginHeight (new));
	}

/*	Convert spacing.
*/
	if (G_UnitType (new) != G_UnitType (current) &&
	    G_UnitType (new) != XmPIXELS)
	{
		G_Spacing (new) = 
			(G_PixmapPosition (new) == XmPIXMAP_LEFT ||
			 G_PixmapPosition (new) == XmPIXMAP_RIGHT)
			? XmeToHorizontalPixels ((Widget)new, G_UnitType (new),
					(XtArgVal *)G_Spacing (new))
			: XmeToVerticalPixels ((Widget)new, G_UnitType (new),
					(XtArgVal *)G_Spacing (new));
	}

/*	Process change in string or font list.
*/

	if (G_String (new) != G_String (current) ||
	    G_FontList (new) != G_FontList (current) ||
	    G_Underline (new) != G_Underline (current))
	{
		if (G_FontList (new) != G_FontList (current))
		{
			if (G_FontList (new) == NULL)
				G_FontList (new) = G_FontList (current);
			else
			{
				XmFontListFree (G_FontList (current));
				G_FontList (new) =
					XmFontListCopy (G_FontList (new));
			}
		}
		if (G_String (new))
		{
			if (G_String (new) != G_String (current))
			{
				if (G_String (current))
					XmStringFree (G_String (current));
				G_String (new) =
					XmStringCopy (G_String (new));
			}

			XmStringExtent (G_FontList (new), G_String (new),
					&w, &h);
                        if (G_Underline(new))
                           h++;
		}
		else
			w = h = 0;

		G_StringWidth (new) = w;
		G_StringHeight (new) = h;

		G_Spacing (new) = (Dimension) G_StringHeight (new) / 5;
		if (G_Spacing (new) < SPACING_DEFAULT)
			G_Spacing (new) = SPACING_DEFAULT;

		if ((G_String (new) != NULL) &&
		    (G_StringWidth (new) == G_StringWidth (current)) &&
		    (G_StringHeight (new) == G_StringHeight (current)))
			draw_string = True;
		else
			redraw_flag = True;
	}

/*	Check for other changes requiring redraw.
*/
	if (G_HighlightThickness (new) != G_HighlightThickness (current) ||
	    G_ShadowThickness (new) != G_ShadowThickness (current) ||
	    G_MarginWidth (new) != G_MarginWidth (current) ||
	    G_MarginHeight (new) != G_MarginHeight (current) ||
	    G_Spacing (new) != G_Spacing (current))
	{
		redraw_flag = True;
	}

/*	Update size.
*/
	if (!(redraw_flag ||
	      (G_RecomputeSize (new) && ! G_RecomputeSize (current))))
	  {
	    _DtProcessLock();
	    optimize_redraw = C_OptimizeRedraw(XtClass(new));
	    _DtProcessUnlock();
	  }

	if (redraw_flag ||
	    (G_RecomputeSize (new) && ! G_RecomputeSize (current)))
	{
		if (G_RecomputeSize (new) &&
		    (G_Width (current) == G_Width (new) ||
		     G_Height (current) == G_Height (new)))
		{
		  	GetSizeProc get_size;

			_DtProcessLock();
			get_size = C_GetSize(XtClass(new));
			_DtProcessUnlock();
			(*get_size) (new, &w, &h);

			if (G_Width (current) == G_Width (new))
				G_Width (new) = w;
			if (G_Height (current) == G_Height (new))
				G_Height (new) = h;
		}
	}

/*	Set redraw flag if this class doesn't optimize redraw.
*/
	else if (! optimize_redraw)
	{
		if (draw_shadow || draw_pixmap || draw_string)
			redraw_flag = True;
	}	

/*	Optimize redraw if managed.
*/
	else if (XtIsManaged (new_w) && XtIsRealized(new_w))
	{
/*	Get string and pixmap positions if necessary.
*/
		if ((draw_pixmap && G_Pixmap (new)) ||
		    (draw_string && G_String (new)))
		{
			GetPositionProc get_positions;

			_DtProcessLock();
			get_positions = C_GetPositions(XtClass(new));
			_DtProcessUnlock();
			(*get_positions) (new, G_Width (new), G_Height (new),
				h_t, G_ShadowThickness (new),
				(Position *)&p_x, (Position *)&p_y, 
				(Position *)&s_x, (Position *)&s_y);
		}
/*	Copy pixmap, clip if necessary.
*/
		if (draw_pixmap && G_Pixmap (new) &&
		    G_Pixmap (new) != XmUNSPECIFIED_PIXMAP)
		  {
		      w = ((unsigned) (p_x + s_t + h_t) >= G_Width (new))
			? 0 : Min ((unsigned)G_PixmapWidth (new),
				   G_Width (new) - p_x - s_t - h_t);
		      h = ((unsigned) (p_y + s_t + h_t) >= G_Height (new))
			? 0 : Min ((unsigned)G_PixmapHeight (new),
				   G_Height (new) - p_y - s_t - h_t);
		      if (w > 0 && h > 0) {
			  XCopyArea 
			    (XtDisplay (new), G_Pixmap (new), XtWindow (new),
			     GetMaskGC(new, G_X(new) + p_x, G_Y(new) + p_y),
			     0, 0,
			     w, h, G_X (new) + p_x, G_Y (new) + p_y);
		      }
		}
/*	Draw string with normal or armed background; clip if necessary.
*/
		if (draw_string && G_String (new))
		{
			GC		gc;
			XRectangle	clip;
			unsigned char	behavior = G_Behavior (new);

			if ((behavior == XmICON_BUTTON ||
			     behavior == XmICON_DRAG) &&
			    G_FillOnArm (new) && G_Armed (new))
				gc = G_ArmedGC (new);
			else if (behavior == XmICON_TOGGLE &&
				 G_FillOnArm (new) &&
				 ((G_Armed (new) && !G_Set (new)) ||
				  (!G_Armed (new) && G_Set (new))))
				gc = G_ArmedGC (new);
			else
				gc = G_NormalGC (new);

			clip.x = G_X (new) + s_x;
			clip.y = G_Y (new) + s_y;
			clip.width = (s_x + s_t + h_t >= 
				      (unsigned)G_Width (new))
				? 0 : Min ((unsigned)G_StringWidth (new),
					   G_Width (new) - s_x - s_t - h_t);
			clip.height = (s_y + s_t + h_t >= 
				       (unsigned)G_Height (new))
				? 0 : Min ((unsigned)G_StringHeight (new),
					   G_Height (new) - s_y - s_t - h_t);
			if (clip.width > 0 && clip.height > 0)
                        {
				XmStringDrawImage (XtDisplay (new),
					 XtWindow (new), G_FontList (new),
					G_String (new), gc,
					G_X (new) + s_x, G_Y (new) + s_y,
					clip.width, XmALIGNMENT_CENTER,
					XmSTRING_DIRECTION_L_TO_R, &clip);

				if (G_Underline(new))
				{
				   XmStringDrawUnderline (XtDisplay (new),
					    XtWindow (new), G_FontList (new),
					   G_String (new), gc,
					   G_X (new) + s_x, G_Y (new) + s_y,
					   clip.width, XmALIGNMENT_CENTER,
					   XmSTRING_DIRECTION_L_TO_R, &clip,
                                           G_String(new));
				}
                        }
		}
/*	Draw shadow.
*/
		if (draw_shadow && G_DrawShadow(new))
		{
                   if(G_BorderType(new) == DtRECTANGLE || !G_Pixmap(new))
		     XmeDrawShadows(XtDisplay(new), XtWindow(new),
				    M_TopShadowGC(MgrParent(new)),
				    M_BottomShadowGC(MgrParent(new)),
				    G_X(new) + h_t, G_Y(new) + h_t,
				    G_Width(new) - 2*h_t, G_Height(new) - 2*h_t,
				    G_ShadowThickness(new), G_ShadowType(new));
                   else
		     {
		       CallCallbackProc call_callback;
		       _DtProcessLock();
		       call_callback = C_CallCallback(XtClass(new));
		       _DtProcessUnlock();
		       (*call_callback) (new, G_Callback (new),
                                                    XmCR_SHADOW, NULL);
		     }
		}
	}

        if (G_Operations(current) != G_Operations(new)) {
           if (G_Operations(current) == XmDROP_NOOP){
	      _DtIconRegisterDropsite(new_w);
           } else {
              if (G_Operations(new) == XmDROP_NOOP)
	         DtDndDropUnregister(new_w);
              else {
                 Arg args[1];

	         XtSetArg(args[0], XmNdropSiteOperations, G_Operations(new));
	         XmDropSiteUpdate(new_w, args, 1);
              }
           }
        }

	return (redraw_flag);
}



/*-------------------------------------------------------------
**	Gadget Procs
**-------------------------------------------------------------
*/

/*-------------------------------------------------------------
**	BorderHighlight
**
*/
static void 
BorderHighlight( DtIconGadget g)
{
   register int width;
   register int height;
   CallCallbackProc call_callback;

   width = g->rectangle.width;
   height = g->rectangle.height;

   if (width == 0 || height == 0) return;

   if (g->gadget.highlight_thickness == 0) return;

   g->gadget.highlighted = True;
   g->gadget.highlight_drawn = True;

   if(G_BorderType(g) == DtRECTANGLE || !G_Pixmap(g))
      HighlightBorder ((Widget)g);

   _DtProcessLock();
   call_callback = C_CallCallback(XtClass(g));
   _DtProcessUnlock();
   (*call_callback) (g, G_Callback (g), XmCR_HIGHLIGHT, NULL);
}

/*-------------------------------------------------------------
**	BorderUnhighlight
**
*/
static void 
BorderUnhighlight( DtIconGadget g)
{

   register int window_width;
   register int window_height;
   register int highlight_width;
   CallCallbackProc	call_callback;

   window_width = g->rectangle.width;
   window_height = g->rectangle.height;

   if (window_width == 0 || window_height == 0) return;

   highlight_width = g->gadget.highlight_thickness;
   if (highlight_width == 0) return;

   g->gadget.highlighted = False;
   g->gadget.highlight_drawn = False;

   if(G_BorderType(g) == DtRECTANGLE || !G_Pixmap(g))
      UnhighlightBorder ((Widget)g);

   _DtProcessLock();
   call_callback = C_CallCallback(XtClass(g));
   _DtProcessUnlock();
   (*call_callback) (g, G_Callback (g), XmCR_UNHIGHLIGHT, NULL);
}

/*-------------------------------------------------------------
**	ArmAndActivate
**		Invoke Activate.
*/
static void 
ArmAndActivate(
        Widget w,
        XEvent *event )
{
	IconArm (w, event);
	IconActivate (w, event);
}



/*-------------------------------------------------------------
**	InputDispatch
**		Process event dispatched from parent or event handler.
*/
static void 
InputDispatch(
        Widget w,
        XButtonEvent *event,
        Mask event_mask )
{
	DtIconGadget	g =	(DtIconGadget) w;

	if (event_mask & XmARM_EVENT || event_mask & XmMULTI_ARM_EVENT)
		if (event->button == Button2)
			IconDrag (w, (XEvent*) event);	
                else if (event->button == Button3)
                        IconPopup (w, (XEvent*) event);
		else
			IconArm (w, (XEvent*) event);
	else if (event_mask & XmACTIVATE_EVENT ||
		 event_mask & XmMULTI_ACTIVATE_EVENT)
	{
                if (event->button == Button3)
                        ;
		else if (event->x >= G_X (g) &&
			 event->x <= G_X (g) + (int)G_Width (g) &&
			 event->y >= G_Y (g) &&
			 event->y <= G_Y (g) + (int)G_Height (g))
			IconActivate (w, (XEvent*) event);
		else
			IconDisarm (w, (XEvent*) event);
	}
	else if (event_mask & XmHELP_EVENT)
		_XmSocorro (w, (XEvent *)event,
                                     (String *)NULL,(Cardinal)0);
	else if (event_mask & XmENTER_EVENT)
		IconEnter (w, (XEvent *)event);
	else if (event_mask & XmLEAVE_EVENT)
		IconLeave (w, (XEvent *)event);
	else if (event_mask & XmFOCUS_IN_EVENT)
		_XmFocusInGadget (w, (XEvent *)event,
                                    (String *)NULL,(Cardinal)0);
	else if (event_mask & XmFOCUS_OUT_EVENT)
		_XmFocusOutGadget (w, (XEvent *)event,
                                    (String *)NULL,(Cardinal)0);
}



/*-------------------------------------------------------------
**	VisualChange
**		Update GCs when parent visuals change.
*/
static Boolean 
VisualChange(
        Widget w,
        Widget current_w,
        Widget new_w )
{
    XmManagerWidget		current =	(XmManagerWidget) current_w;
    XmManagerWidget		new = 		(XmManagerWidget) new_w;
    DtIconGadget		g = 		(DtIconGadget) w;
    Boolean			update = 	False;
    DtIconCacheObjPart		local_cache;
    UpdateGCsProc		update_gcs;
    
    QualifyIconLocalCache(g, &local_cache);
    /*	If foreground or background was the same as the parent, and parent
     **	foreground or background has changed, then update gcs and pixmap.
     */
    /* (can't really tell if explicitly set to be same as parent!
     **  -- could add flags set in dynamic default procs for fg and bg)
     */
    if (G_Foreground (g) == M_Foreground (current) &&
	M_Foreground (current) != M_Foreground (new))
      {
	  local_cache.foreground = M_Foreground (new);
	  update = True;
      }
    
    if (G_Background (g) == M_Background (current) &&
	M_Background (current) != M_Background (new))
      {
	  local_cache.background = M_Background (new);
	  update = True;
      }
    
    if (G_PixmapForeground (g) == M_Foreground (current) &&
	M_Foreground (current) != M_Foreground (new))
      {
	  G_PixmapForeground(g) =  M_Foreground (new);
	  update = True;
      }
    
    if (G_PixmapBackground (g) == M_Background (current) &&
	M_Background (current) != M_Background (new))
      {
	  G_PixmapBackground(g) = M_Background (new);
	  update = True;
      }
    
    if (update)
      {
	  ReCacheIcon_r(&local_cache, g);
	  _DtProcessLock();
	  update_gcs = C_UpdateGCs(XtClass(g));
	  _DtProcessUnlock();
	  (*update_gcs) (g);
	  
	  if (G_ImageName (g) != NULL)
	    {
		if (G_Mask(g) != XmUNSPECIFIED_PIXMAP)
		  XmDestroyPixmap (XtScreen(g),G_Mask(g));
		XmDestroyPixmap (XtScreen(w),G_Pixmap (g));
		G_Pixmap (g) = 
		  XmGetPixmap (XtScreen (g), G_ImageName (g),
			       G_PixmapForeground (g),
			       G_PixmapBackground (g));
		if (G_Pixmap (g) != XmUNSPECIFIED_PIXMAP)
		  G_Mask(g) = (Pixmap)_DtGetMask(XtScreen(g), G_ImageName(g));
		return (True);
	    }
	  else
	    return (False);
      }
      return (False);
}


/*-------------------------------------------------------------
**	Icon Procs
**-------------------------------------------------------------
*/

/*-------------------------------------------------------------
**	GetSize
**		Compute size.
*/
static void 
GetSize(
        DtIconGadget g,
        Dimension *w,
        Dimension *h )
{
	Dimension	s_t = G_ShadowThickness (g),
			h_t = G_HighlightThickness (g),
			p_w = G_PixmapWidth (g),
			p_h = G_PixmapHeight (g),
			m_w = G_MarginWidth (g),
			m_h = G_MarginHeight (g),
			s_w = G_StringWidth (g),
			s_h = G_StringHeight (g),
			v_pad = 2 * (s_t + h_t + m_h),
			h_pad = 2 * (s_t + h_t + m_w),
			spacing = G_Spacing (g);
	
	if (((p_w == 0) && (p_h == 0)) || ((s_w == 0) && (s_h == 0)))
		spacing = 0;

/*	Get width and height.
*/
	switch ((int) G_PixmapPosition (g))
	{
		case XmPIXMAP_TOP:
		case XmPIXMAP_BOTTOM:
			*w = Max (p_w, s_w) + h_pad;
			*h = p_h + s_h + v_pad + spacing;
			break;
		case XmPIXMAP_LEFT:
		case XmPIXMAP_RIGHT:
			*w = p_w + s_w + h_pad + spacing;
			*h = Max (p_h, s_h) + v_pad;
			break;
		case XmPIXMAP_MIDDLE:
			*w = Max (p_w, s_w) + h_pad;
			*h = Max (p_h, s_h) + v_pad;
			break;
		default:
			break;
	}
}



/*-------------------------------------------------------------
**	GetPositions
**		Get positions of string and pixmap.
*/
static void 
GetPositions(
        DtIconGadget g,
        Position w,
        Position h,
        Dimension h_t,
        Dimension s_t,
        Position *pix_x,
        Position *pix_y,
        Position *str_x,
        Position *str_y )
{
	Dimension	p_w =		G_PixmapWidth (g),
			p_h =		G_PixmapHeight (g),
			s_w =		G_StringWidth (g),
			s_h =		G_StringHeight (g),
			m_w =		G_MarginWidth (g),
			m_h =		G_MarginHeight (g),
			spacing =	G_Spacing (g),
			h_pad =		s_t + h_t + m_w,
			v_pad =		s_t + h_t + m_h,
			width =		w - 2 * h_pad,
			height =	h - 2 * v_pad;
	Position	p_x =		h_pad,
			p_y =		v_pad,
			s_x =		h_pad,
			s_y =		v_pad;
	unsigned char	align =		G_Alignment (g);

	if (((p_w == 0) && (p_h == 0)) || ((s_w == 0) && (s_h == 0)))
		spacing = 0;

/*	Set positions
*/
	switch ((int) G_PixmapPosition (g))
	{
		case XmPIXMAP_TOP:
			if (align == XmALIGNMENT_CENTER)
			{
				if (p_w && width > p_w)
					p_x += (width - p_w)/2U;
				if (s_w && width > s_w)
					s_x += (width - s_w)/2U;
			}
			else if (align == XmALIGNMENT_END)
			{
				if (p_w && width > p_w)
					p_x += width - p_w;
				if (s_w && width > s_w)
					s_x += width - s_w;
			}
			if (p_h && ((unsigned)height > p_h + s_h + spacing))
				p_y += (height - p_h - s_h - spacing)/2U;
			if (p_h)
				s_y = p_y + p_h + spacing;
			else
				s_y += (height - s_h)/2U;
			break;
		case XmPIXMAP_BOTTOM:
			if (align == XmALIGNMENT_CENTER)
			{
				if (p_w && width > p_w)
					p_x += (width - p_w)/2U;
				if (s_w && width > s_w)
					s_x += (width - s_w)/2U;
			}
			else if (align == XmALIGNMENT_END)
			{
				if (p_w && width > p_w)
					p_x += width - p_w;
				if (s_w && width > s_w)
					s_x += width - s_w;
			}
			if (s_h && ((unsigned)height > p_h + s_h + spacing))
				s_y += (height - p_h - s_h - spacing)/2U;
			if (s_h)
				p_y = s_y + s_h + spacing;
			else
				p_y += (height - s_h)/2U;
			break;
		case XmPIXMAP_LEFT:
			if (p_h && height > p_h)
				p_y += (height - p_h)/2U;
			s_x += p_w + spacing;
			if (s_h && height > s_h)
				s_y += (height - s_h)/2U;
			break;
		case XmPIXMAP_RIGHT:
			if (s_h && height > s_h)
				s_y += (height - s_h)/2U;
			p_x += s_w + spacing;
			if (p_h && height > p_h)
				p_y += (height - p_h)/2U;
			break;
		case XmPIXMAP_MIDDLE:
			if (p_w && width > p_w)
		 	   p_x += (width - p_w)/2U;
			if (s_w && width > s_w)
			   s_x += (width - s_w)/2U;
			if (s_h && height > s_h)
				s_y += (height - s_h)/2U;
			if (p_h && height > p_h)
				p_y += (height - p_h)/2U;
			break;
		default:
			break;
	}

	*pix_x = p_x;
	*pix_y = p_y;
	*str_x = s_x;
	*str_y = s_y;
}



/*-------------------------------------------------------------
**	Draw
**		Draw gadget to drawable.
*/
/* ARGSUSED */
static void 
Draw(
        DtIconGadget g,
        Drawable drawable,
        Position x,
        Position y,
        Dimension w,
        Dimension h,
        Dimension h_t,
        Dimension s_t,
        unsigned char s_type,
        unsigned char fill_mode )
{
	XmManagerWidget	mgr =	(XmManagerWidget) XtParent (g);
	Display *	d = 	XtDisplay (g);
	GC		gc;
	XRectangle	clip;
	Position	p_x, p_y, s_x, s_y;
	Dimension	width, height;
	unsigned char	behavior =	G_Behavior (g);
        Position        adj_x, adj_y;
	int		rec_width=0,begin=0,diff=0;
	GetPositionProc get_positions;

/*	Fill with icon or manager background or arm color.
*/
        if (G_SavedParentBG(g) != XtParent(g)->core.background_pixel) {
         XtReleaseGC((Widget)mgr, G_ParentBackgroundGC(g));
         GetParentBackgroundGC(g);
        }

	if (fill_mode == XmFILL_SELF)
	{
		if ((behavior == XmICON_BUTTON || behavior == XmICON_DRAG) &&
		     G_FillOnArm (g) && G_Armed (g))
			gc = G_ArmedBackgroundGC (g);
		else if (behavior == XmICON_TOGGLE && G_FillOnArm (g) &&
			 ((G_Armed (g) && !G_Set (g)) ||
			  (!G_Armed (g) && G_Set (g))))
			gc = G_ArmedBackgroundGC (g);
		else
			gc = G_BackgroundGC (g);
	}
	else if (fill_mode == XmFILL_PARENT)
		gc = G_ParentBackgroundGC (g);

	if ((fill_mode == XmFILL_SELF) || (fill_mode == XmFILL_PARENT))
		XFillRectangle (d, drawable, gc, x + h_t, y + h_t,
				w - 2 * h_t, h - 2 * h_t);

/*	Get pixmap and string positions.
*/
	_DtProcessLock();
	get_positions = C_GetPositions(XtClass(g));
	_DtProcessUnlock();
	(*get_positions) (g, w, h, h_t, s_t, &p_x, &p_y, &s_x, &s_y);

/*	Copy pixmap.
*/
	if (G_Pixmap (g))
	{
		width = (p_x + s_t + h_t >= (unsigned)G_Width (g))
			? 0 : Min ((unsigned)G_PixmapWidth (g),
				   G_Width (g) - p_x - s_t - h_t);
		height = (p_y + s_t + h_t >= (unsigned)G_Height (g))
			? 0 : Min ((unsigned)G_PixmapHeight (g),
				   G_Height (g) - p_y - s_t - h_t);
		if (width > 0 && height > 0)
		  {
		      if (fill_mode == XmFILL_TRANSPARENT)
                        {
			    adj_x = s_t + h_t + G_MarginWidth(g);
			    adj_y = s_t + h_t + G_MarginHeight(g);
                            switch (G_PixmapPosition(g))
                            {
                               case XmPIXMAP_TOP:
                               case XmPIXMAP_BOTTOM:
			          XFillRectangle(d, drawable, 
                                                 G_ParentBackgroundGC(g),
					         x + p_x - adj_x, 
                                                 y + p_y - adj_y,
					         width + (2 * adj_y),
        				         height + (2 * adj_x) -
                                                                 (s_t + h_t));
                                  break;
                               case XmPIXMAP_LEFT:
                               case XmPIXMAP_RIGHT:
			          XFillRectangle(d, drawable, 
                                                 G_ParentBackgroundGC(g),
					         x + p_x - adj_x, 
                                                 y + p_y - adj_y,
					         width + (2 * adj_y) -
                                                                 (s_t + h_t),
        				         height + (2 * adj_x));
                                  break;
                               case XmPIXMAP_MIDDLE:
			          XFillRectangle(d, drawable, 
                                                 G_ParentBackgroundGC(g),
					         x + p_x - adj_x, 
                                                 y + p_y - adj_y,
					         width + (2 * adj_y),
        				         height + (2 * adj_x));
                                  break;
                            }
                        }
		      XCopyArea (d, G_Pixmap (g), drawable, 
				 GetMaskGC(g, x + p_x, y + p_y),
				 0, 0, width, height, x + p_x, y + p_y);
		  }
	}

/*	Draw string.
*/
	if ((behavior == XmICON_BUTTON || behavior == XmICON_DRAG) &&
	     G_FillOnArm (g) && G_Armed (g))
		gc = G_ArmedGC (g);
	else if (behavior == XmICON_TOGGLE && G_FillOnArm (g) &&
		 ((G_Armed (g) && !G_Set (g)) ||
		  (!G_Armed (g) && G_Set (g))))
		gc = G_ArmedGC (g);
	else
		gc = G_NormalGC (g);

	if (G_String (g))
	{
		clip.x = x + s_x;
		clip.y = y + s_y;
                switch (G_PixmapPosition(g))
                {
                   case XmPIXMAP_TOP:
                   case XmPIXMAP_BOTTOM:
		      clip.width = (s_x + s_t + h_t >= (unsigned)G_Width (g))
			      ? 0 : Min ((unsigned)G_StringWidth (g),
					 G_Width (g) - s_x);
		      clip.height = (s_y + s_t + h_t >= (unsigned)G_Height (g))
		      	      ? 0 : Min ((unsigned)G_StringHeight (g),
					 G_Height (g) - s_y - s_t - h_t);
                      break;
                   case XmPIXMAP_LEFT:
                   case XmPIXMAP_RIGHT:
		      clip.width = (s_x + s_t + h_t >= (unsigned)G_Width (g))
			      ? 0 : Min ((unsigned)G_StringWidth (g),
					 G_Width (g) - s_x - s_t - h_t);
		      clip.height = (s_y + s_t + h_t >= (unsigned)G_Height (g))
		      	      ? 0 : Min ((unsigned)G_StringHeight (g),
					 G_Height (g) - s_y);
                      break;
                   case XmPIXMAP_MIDDLE:
		      clip.width = (s_x + s_t + h_t >= (unsigned)G_Width (g))
			      ? 0 : Min ((unsigned)G_StringWidth (g),
					 G_Width (g) - s_x);
		      clip.height = (s_y + s_t + h_t >= (unsigned)G_Height (g))
		      	      ? 0 : Min ((unsigned)G_StringHeight (g),
					 G_Height (g) - s_y);
                      break;
                   default:
                     /* Unknown alignment */
                     clip.width = 0;
                     clip.height = 0;
                 }
		if (clip.width > 0 && clip.height > 0)
                {
                        if (fill_mode == XmFILL_TRANSPARENT)
                        {
                           adj_x = s_t + h_t + G_MarginWidth(g);
                           adj_y = s_t + h_t + G_MarginHeight(g);
                           switch (G_PixmapPosition(g))
                           {
                              case XmPIXMAP_TOP:
                              case XmPIXMAP_BOTTOM:
                                 XFillRectangle(d, drawable, 
                                                G_ParentBackgroundGC(g),
                                                clip.x - adj_x, 
                                                clip.y - adj_y + s_t + h_t,
                                                clip.width + (2 * adj_y),
                                                clip.height + (2 * adj_x) - 
                                                                (s_t + h_t));
                                 break;
                              case XmPIXMAP_RIGHT:
                              case XmPIXMAP_LEFT:
				begin = clip.x - adj_x + s_t + h_t;
                                rec_width = clip.width + (2 * adj_y) 
							     -(s_t + h_t);
				if (G_PixmapPosition(g) == XmPIXMAP_LEFT &&
				    begin > (int) (x + p_x) && 
				    begin < (int) (x + p_x + width))
                                {
                                   /*
                                    * XmPIXMAP_LEFT -- the rectangle starts
				    * inside the pixmap
                                    */
                                        diff = x+p_x+width - begin;
                                        begin+=diff;
                                        rec_width-=diff;
                                }
                                else if(G_PixmapPosition(g) == XmPIXMAP_RIGHT &&
				    (rec_width+begin) > (x+p_x))
                                {
                                    /*
                                     * PIXMAP_RIGHT -- rectangle drawn into
                                     * the pixmap
                                     */
                                        diff = ( rec_width + begin) - (x+p_x);
                                        rec_width-=diff;
                                }

                                 XFillRectangle(d, drawable, 
                                                G_ParentBackgroundGC(g),
                                                begin, 
                                                clip.y - adj_y,
                                                rec_width,
                                                clip.height + (2 * adj_x));
                                 break;
                              case XmPIXMAP_MIDDLE:
                                 XFillRectangle(d, drawable, 
                                                G_ParentBackgroundGC(g),
                                                clip.x - adj_x,
                                                clip.y - adj_y,
                                                clip.width + (2 * adj_y),
                                                clip.height + (2 * adj_x));
                                 break;
                            }
                        }

			XmStringDrawImage (d, drawable, G_FontList (g),
				G_String (g), gc, x + s_x, y + s_y,
				clip.width, XmALIGNMENT_BEGINNING,
				XmSTRING_DIRECTION_L_TO_R, &clip);

			if (G_Underline(g))
			{
			   XmStringDrawUnderline (d, drawable, G_FontList (g),
				G_String (g), gc, x + s_x, y + s_y,
				clip.width, XmALIGNMENT_BEGINNING,
				XmSTRING_DIRECTION_L_TO_R, &clip,
                                G_String(g));
			}
                }
	}

        /* Potentially fill the area between the label and the pixmap */
        if ((fill_mode == XmFILL_TRANSPARENT) && G_Pixmap(g) && G_String(g) &&
           (height > 0) && (width > 0) && (clip.width > 0) && (clip.height > 0))
        {
           switch (G_PixmapPosition(g))
           {
              case XmPIXMAP_TOP:
                      XFillRectangle(d, drawable, G_ParentBackgroundGC(g),
                                     x + Max(s_x, p_x), y + p_y + height,
                                     Min(clip.width, width),
                                     s_y - (p_y + height));
                      break;

              case XmPIXMAP_BOTTOM:
                      XFillRectangle(d, drawable, G_ParentBackgroundGC(g),
                                     x + Max(s_x, p_x), y + s_y + clip.height,
                                     Min(clip.width, width),
                                     p_y - (s_y + clip.height));
                      break;

              case XmPIXMAP_RIGHT:
                      XFillRectangle(d, drawable, G_ParentBackgroundGC(g),
                                     x + s_x + clip.width, y + Max(s_y, p_y),
                                     p_x - (s_x + clip.width),
                                     Min(clip.height, height));
                      break;
              case XmPIXMAP_LEFT:
                      XFillRectangle(d, drawable, G_ParentBackgroundGC(g),
                                     x + p_x + width, y + Max(s_y, p_y),
                                     s_x - (p_x + width),
                                     Min(clip.height, height));
                      break;
              case XmPIXMAP_MIDDLE:
                      XFillRectangle(d, drawable, G_ParentBackgroundGC(g),
                                     x + Max(s_x, p_x), y + Max(s_y, p_y),
                                     Min(clip.width, width),
                                     Min(clip.height, height));
                      break;
           }
        }


/*	Draw shadow.
*/
	if (G_ShadowThickness (g) > 0 && G_DrawShadow(g)) 
           if(G_BorderType(g) == DtRECTANGLE || !G_Pixmap(g))
		{
		  unsigned char shadow_type;

		  if (((G_Behavior (g) == XmICON_BUTTON) && G_Armed (g)) ||
		      ((G_Behavior (g) == XmICON_TOGGLE) &&
		       ((!G_Set (g) && G_Armed (g)) ||
		       (G_Set (g) && !G_Armed (g)))))
		    shadow_type = XmSHADOW_IN;
		  else
		    shadow_type = XmSHADOW_OUT;

		  XmeDrawShadows(d, drawable, 
				 M_TopShadowGC(MgrParent(g)),
				 M_BottomShadowGC(MgrParent(g)),
				 x + h_t, y + h_t, w - 2*h_t, h - 2*h_t,
				 s_t, shadow_type);
		}
           else
	     {
	       CallCallbackProc call_callback;
	       _DtProcessLock();
	       call_callback = C_CallCallback(XtClass(g));
	       _DtProcessUnlock();
	       (*call_callback) (g, G_Callback (g), XmCR_SHADOW, NULL);
	     }
}



/*-------------------------------------------------------------
**	CallCallback
**		Call callback, if any, with reason and event.
*/
static void 
CallCallback(
        DtIconGadget g,
        XtCallbackList cb,
        int reason,
        XEvent *event )
{
	DtIconCallbackStruct	cb_data;

	if (cb != NULL)
	{
		cb_data.reason = reason;
		cb_data.event = event;
		cb_data.set = G_Set (g);
		XtCallCallbackList ((Widget) g, cb, &cb_data);
	}
}





/*-------------------------------------------------------------
**	UpdateGCs
**		Get normal and background graphics contexts.
**		Use standard mask to maximize caching opportunities.
*/
static void 
UpdateGCs(
        DtIconGadget g )
{
	XGCValues	values;
	XtGCMask	value_mask;
	XmManagerWidget	mw = (XmManagerWidget) XtParent(g);
	XFontStruct *	font;

	if (G_NormalGC (g))
		XtReleaseGC ((Widget)mw, G_NormalGC (g));
	if (G_ClipGC (g))
		XtReleaseGC ((Widget)mw, G_ClipGC (g));
	if (G_BackgroundGC (g))
		XtReleaseGC ((Widget)mw, G_BackgroundGC (g));
	if (G_ArmedGC (g))
		XtReleaseGC ((Widget)mw, G_ArmedGC (g));
	if (G_ArmedBackgroundGC (g))
		XtReleaseGC ((Widget)mw, G_ArmedBackgroundGC (g));

/*	Get normal GC.
*/
	value_mask = GCForeground | GCBackground | GCFillStyle;
	values.foreground = G_Foreground (g);
	values.background = G_Background (g);
	values.fill_style = FillSolid;
	if (XmeRenderTableGetDefaultFont(G_FontList (g), &font)) {
	   value_mask |= GCFont;
	   values.font = font->fid;
        }
	G_NormalGC (g) = XtGetGC ((Widget)mw, value_mask, &values);

/*	Get background GC.
*/
	values.foreground = G_Background (g);
	values.background = G_Foreground (g);
	G_BackgroundGC (g) = XtGetGC ((Widget)mw, value_mask, &values);

/*	Get armed GC.
*/
	values.foreground = G_Foreground (g);
	values.background = G_ArmColor (g);
	G_ArmedGC (g) = XtGetGC ((Widget)mw, value_mask, &values);

/*	Get armed background GC.
*/
	values.foreground = G_ArmColor (g);
	values.background = G_Background (g);
	G_ArmedBackgroundGC (g) = XtGetGC ((Widget)mw, value_mask, &values);

	if (G_Mask(g) != XmUNSPECIFIED_PIXMAP) {
	    value_mask |= GCClipMask;
	    values.clip_mask = G_Mask(g);
	    values.foreground = G_Foreground (g);
	    values.background = G_Background (g);
	    G_ClipGC (g) = XtGetGC ((Widget)mw, value_mask, &values);
	}
	else
	  G_ClipGC (g) = NULL;
}


/*-------------------------------------------------------------
**	GetIconClassSecResData ( )
**		Class function to be called to copy secondary resource
**		for external use.  i.e. copy the cached resources and
**		send it back.
**-------------------------------------------------------------
*/
/* ARGSUSED */
static Cardinal 
GetIconClassSecResData(
        WidgetClass class,
        XmSecondaryResourceData **data_rtn )
{   int arrayCount = 0;
    XmBaseClassExt  bcePtr;
    String  resource_class, resource_name;
    XtPointer  client_data;

    _DtProcessLock();
    bcePtr = &( iconBaseClassExtRec);
    client_data = NULL;
    resource_class = NULL;
    resource_name = NULL;
    arrayCount =
      _XmSecondaryResourceData ( bcePtr, data_rtn, client_data,
                                resource_name, resource_class,
                            (XmResourceBaseProc) (GetIconClassResBase));
    _DtProcessUnlock();

    return (arrayCount);
}


/*-------------------------------------------------------------
**	GetIconClassResBase ()
**		return the address of the base of resources.
**		- Not yet implemented.
**-------------------------------------------------------------
*/
/* ARGSUSED */
static XtPointer 
GetIconClassResBase(
        Widget widget,
        XtPointer client_data )
{   XtPointer  widgetSecdataPtr;
    int  icon_cache_size = sizeof (DtIconCacheObjPart);
    char *cp;

    widgetSecdataPtr = (XtPointer) (XtMalloc ( icon_cache_size +1));

    if (widgetSecdataPtr)
      { 
	cp = (char *) widgetSecdataPtr;

#ifndef SVR4
        bcopy ( (char *) ( Icon_Cache(widget)), (char *) cp, icon_cache_size);
#else
        memmove ( (char *)cp , (char *) ( Icon_Cache(widget)), icon_cache_size);
#endif
      }

    return (widgetSecdataPtr);
}



/*-------------------------------------------------------------
**	Public Entry Points
**-------------------------------------------------------------
*/

/*-------------------------------------------------------------
**	_DtCreateIcon
**		Create a new gadget instance.
**-------------------------------------------------------------
*/
Widget 
_DtCreateIcon(
        Widget parent,
        String name,
        ArgList arglist,
        Cardinal argcount )
{
	return (XtCreateWidget (name, dtIconGadgetClass, 
			parent, arglist, argcount));
}



/*-------------------------------------------------------------
**	_DtIconGetState
**		Return state of Icon.
**-------------------------------------------------------------
*/
Boolean 
_DtIconGetState(
        Widget w )
{
	DtIconGadget g =	(DtIconGadget) w;

	return (G_Set (g));
}


/*-------------------------------------------------------------
**	_DtIconSetState
**		Set state of Icon.
**-------------------------------------------------------------
*/
void 
_DtIconSetState(
        Widget w,
        Boolean state,
        Boolean notify )
{
	DtIconGadget g =	(DtIconGadget) w;
	CallCallbackProc	call_callback;
	XtExposeProc		expose;

	if (G_Behavior (g) != XmICON_TOGGLE || state == G_Set (g))
		return;

	_DtProcessLock();
	call_callback = C_CallCallback(XtClass(g));
	expose = XtCoreProc(w, expose);
	_DtProcessUnlock();

	G_Set (g) = state;
	(*expose) ((Widget)g, NULL, NULL);

	if (notify)
	  {
	    (*call_callback) (g, G_Callback (g), XmCR_VALUE_CHANGED, NULL);
	  }
}



/*-------------------------------------------------------------
**	_DtIconDraw
**		Render gadget to drawable without highlight.
**-------------------------------------------------------------
*/
Drawable 
_DtIconDraw(
        Widget widget,
        Drawable drawable,
        Position x,
        Position y,
        Boolean fill )
{
	DtIconGadget	g =		(DtIconGadget) widget;
	Dimension	h_t =		G_HighlightThickness (g),
			w =		G_Width (g) - 2 * h_t,
			h =		G_Height (g) - 2 * h_t;
	unsigned char	fill_mode;
	DrawProc	draw;

	if (!drawable || drawable == XmUNSPECIFIED_PIXMAP)
		drawable = (Drawable)
			XCreatePixmap (XtDisplay (g),
				RootWindowOfScreen (XtScreen (g)),
				w, h, DefaultDepthOfScreen (XtScreen (g)));

	fill_mode = (fill) ? XmFILL_SELF : XmFILL_PARENT;
	
	_DtProcessLock();
	draw = C_Draw(XtClass(g));
	_DtProcessUnlock();
	(*draw) (g, drawable, x, y, w, h,
		 0, G_ShadowThickness (g), G_ShadowType (g), fill_mode);

	return (drawable);
}



/***************************************************************************/


/*
 * Load the specified pixmap.
 */

static Boolean 
LoadPixmap(
        DtIconGadget new,
        String pixmap )

{
   unsigned int int_h, int_w;
   Screen *s = XtScreen(new);
   Pixmap pm = XmGetPixmap(s, pixmap, G_PixmapForeground(new),
                           G_PixmapBackground(new));
   Pixmap mask;

   if (pm == XmUNSPECIFIED_PIXMAP)
      return(True);

   mask = XmeGetMask(s, pixmap);

   G_Pixmap(new) = pm;
   G_Mask(new) = mask;
   G_ImageName(new) = XtNewString(pixmap);

   XmeGetPixmapData(s, pm, NULL, NULL, NULL, NULL, NULL, NULL, &int_w, &int_h);
   G_PixmapWidth(new) = Limit((Dimension)int_w, G_MaxPixmapWidth(new));
   G_PixmapHeight(new) = Limit((Dimension)int_h, G_MaxPixmapWidth(new));
   return(False);
}



Widget 
_DtDuplicateIcon(
        Widget parent,
        Widget widget,
        XmString string,
        String pixmap,
        XtPointer user_data,
        Boolean underline )
{
   DtIconGadget gadget;
   int size;
   DtIconGadget new;
   Dimension h, w;
   DtIconCacheObjPart local_cache;
   XtWidgetProc insert_child;
   GetSizeProc get_size;

   /* Create the new instance structure */
   gadget = (DtIconGadget) widget;
   _DtProcessLock();
   size = XtClass(gadget)->core_class.widget_size;
   _DtProcessUnlock();
   new = (DtIconGadget)XtMalloc(size);

   /* Copy the master into the duplicate */

#ifndef       SVR4
   bcopy((char *)gadget, (char *)new, (int)size);
#else
   memmove((char *)new, (char *)gadget, (int)size);
#endif

   _DtProcessLock();
   insert_child = 
     ((CompositeWidgetClass)XtClass(parent))->composite_class.insert_child;
   get_size = C_GetSize(XtClass(new));
   Icon_Cache(new) = (DtIconCacheObjPart *)
                       _XmCachePart(Icon_ClassCachePart(new),
                                    (XtPointer) Icon_Cache(new),
                                    sizeof(DtIconCacheObjPart));
   _DtProcessUnlock();

   /* Certain fields need to be updated */
   new->object.parent = parent;
   new->object.self = (Widget)new;
   G_FontList(new) = XmFontListCopy(G_FontList(gadget));

   /* Certain fields should not be inherited by the clone */
   new->object.destroy_callbacks = NULL;
   new->object.constraints = NULL;
   new->gadget.help_callback = NULL;
   new->icon.drop_callback = NULL;
   new->rectangle.managed = False;
   G_Callback(new) = NULL;

   /* Set the user_data field */
   new->gadget.user_data = user_data;

   /* Process the optional pixmap name */
   if ((pixmap == NULL) || LoadPixmap(new, pixmap))
   {
      /* No pixmap to load */
      G_ImageName(new) = NULL;
      G_Pixmap(new) = None;
      G_PixmapWidth(new) = 0;
      G_PixmapHeight(new) = 0;
   }

   /* Process the required label string */
   G_String(new) = XmStringCopy(string);
   XmStringExtent(G_FontList(new), G_String(new), &w, &h);
   G_Underline(new) = underline;
   if (G_Underline(new))
      h++;
   G_StringWidth(new) = w;
   QualifyIconLocalCache(new, &local_cache);
   local_cache.string_height = h;

   ReCacheIcon_r(&local_cache, new);

   /* Get copies of the GC's */
   G_NormalGC(new) = NULL;
   G_BackgroundGC(new) = NULL;
   G_ArmedGC(new) = NULL;
   G_ArmedBackgroundGC(new) = NULL;
   G_ClipGC(new) = NULL;
   UpdateGCs(new);

   /* Size the gadget */
   (*get_size) (new, &w, &h);
   G_Width(new) = w;
   G_Height(new) = h;

   /* Insert the duplicate into the parent's child list */
   (*insert_child) ((Widget)new);

   return ((Widget) new);
}


Boolean 
_DtIconSelectInTitle(
        Widget widget,
        Position pt_x,
        Position pt_y )

{
   DtIconGadget	g = 	(DtIconGadget) widget;
   Position	x, y;
   Dimension	w, h, h_t, s_t;
   XRectangle	clip;
   Position	p_x, p_y, s_x, s_y;
   GetPositionProc	get_positions;

   h_t = 0;
   s_t = G_ShadowThickness(g);
   x = G_X(g);
   y = G_Y(g);
   w = G_Width (g); 
   h = G_Height (g);
   _DtProcessLock();
   get_positions = C_GetPositions(XtClass(g));
   _DtProcessUnlock();
   (*get_positions) (g, w, h, h_t, s_t, &p_x, &p_y, &s_x, &s_y);

   if (G_String (g))
   {
      clip.x = x + s_x;
      clip.y = y + s_y;
      clip.width = (s_x + s_t + h_t >= (unsigned)G_Width (g))
         ? 0 : Min ((unsigned)G_StringWidth (g), 
		    G_Width (g) - s_x - s_t - h_t);
      clip.height = (s_y + s_t + h_t >= (unsigned)G_Height (g))
         ? 0 : Min ((unsigned)G_StringHeight (g), 
		    G_Height (g) - s_y - s_t - h_t);
      if (clip.width <= 0 || clip.height <= 0)
         return(False);
      else
      {
         if ((pt_x >= clip.x) && 
	     (pt_y >= clip.y) &&
             ((unsigned)pt_x <= clip.x + clip.width) && 
	     ((unsigned)pt_y <= clip.y + clip.height))
            return(True);
         else
            return(False);
      }
   }
   else
      return(False);
}


/*
 * Thread-safe variant of _DtIconGetTextExtent.
 */
void 
_DtIconGetTextExtent_r(Widget widget,
		       XRectangle *clip)
{
   DtIconGadget	g = 	(DtIconGadget) widget;
   Position	x, y;
   Dimension	w, h, h_t, s_t;
   Position	p_x, p_y, s_x, s_y;
   GetPositionProc	get_positions;

   h_t = 0;
   s_t = G_ShadowThickness(g);
   x = G_X(g);
   y = G_Y(g);
   w = G_Width (g); 
   h = G_Height (g);
   _DtProcessLock();
   get_positions = C_GetPositions(XtClass(g));
   _DtProcessUnlock();
   (*get_positions) (g, w, h, h_t, s_t, &p_x, &p_y, &s_x, &s_y);

   if (G_String (g))
   {
      clip->x = x + s_x;
      clip->y = y + s_y;
      clip->width = (s_x + s_t + h_t >= (unsigned)G_Width (g))
         ? 0 : Min ((unsigned)G_StringWidth (g), 
		    G_Width (g) - s_x - s_t - h_t);
      clip->height = (s_y + s_t + h_t >= (unsigned)G_Height (g))
         ? 0 : Min ((unsigned)G_StringHeight (g), 
		    G_Height (g) - s_y - s_t - h_t);

      if (clip->width <= 0)
         clip->width = 0;

      if (clip->height <= 0)
         clip->height = 0;
   }
   else
   {
      clip->x = 0;
      clip->y = 0;
      clip->height = 0;
      clip->width = 0;
   }
}


/*
 * Returns a pointer to a static storage area; must not be freed.
 * This interface is deprecated in favor of _DtIconGetTextExtent_r.
 */

XRectangle * 
_DtIconGetTextExtent(
        Widget widget )

{
   static XRectangle	clip;

   _DtIconGetTextExtent_r(widget, &clip);

   return(&clip);
}

/*-------------------------------------------------------------
**	_DtIconGetIconRects
**		Returns rects occupied by label and pixmap
*/
void 
_DtIconGetIconRects(
        DtIconGadget g,
        unsigned char *flags,
        XRectangle *rect1,
        XRectangle *rect2 )

{
   Position	p_x, p_y, s_x, s_y;
   Dimension	width, height;
   Position     adj_x, adj_y;
   Dimension    h_t, s_t;
   GetPositionProc	get_positions;
   
   h_t = G_HighlightThickness(g);
   s_t = G_ShadowThickness(g);

   adj_x = G_MarginWidth(g);
   adj_y = G_MarginHeight(g);
   _DtProcessLock();
   get_positions = C_GetPositions(XtClass(g));
   _DtProcessUnlock();
   (*get_positions) (g, G_Width(g), G_Height(g), h_t, s_t, &p_x, &p_y, &s_x, &s_y);
   *flags = 0;

   if (G_Pixmap (g))
   {
      width = (p_x + s_t + h_t >= (unsigned)G_Width (g)) ? 0 : 
                 Min ((unsigned)G_PixmapWidth (g), 
		      G_Width (g) - p_x - s_t - h_t);
      height = (p_y + s_t + h_t >= (unsigned)G_Height (g)) ? 0 : 
                 Min ((unsigned)G_PixmapHeight (g), 
		      G_Height (g) - p_y - s_t - h_t);
      if (width > 0 && height > 0)
      {
         rect1->x = G_X(g) + p_x - adj_x; 
         rect1->y = G_Y(g) + p_y - adj_y;
         rect1->width = width + (2 * adj_y);
         rect1->height = height + (2 * adj_x);
         *flags |= XmPIXMAP_RECT;
      }
   }

   if (G_String(g))
   {
      width = (s_x + s_t + h_t >= (unsigned)G_Width (g)) ? 0 : 
	Min ((unsigned)G_StringWidth (g), G_Width (g) - s_x - s_t - h_t);
      height = (s_y + s_t + h_t >= (unsigned)G_Height (g)) ? 0 : 
	Min ((unsigned)G_StringHeight (g), G_Height (g) - s_y - s_t - h_t);
      if (width > 0 && height > 0)
      {
         rect2->x = G_X(g) + s_x - adj_x;
         rect2->y = G_Y(g) + s_y - adj_y;
         rect2->width = width + (2 * adj_y);
         rect2->height = height + (2 * adj_x);
         *flags |= XmLABEL_RECT;
      }
   }
}

/* ARGSUSED */
/* Do animation when everything is completed.
 * Note: DropDestroy callback is the only notification after the melt has
 * been completed.
 */
static void
AnimateCallback(
    Widget      w,
    XtPointer   clientData,
    XtPointer   callData )
{
  DtIconGadget g = (DtIconGadget) w;

  if (G_DropCallback(g)) {
     XtCallCallbackList(w, G_DropCallback(g), callData);
  }
}

/* ARGSUSED */
static void
TransferCallback(
        Widget w,
        XtPointer clientData,
        XtPointer callData )
{
  DtDndTransferCallback call_data = (DtDndTransferCallback) callData;
  DtIconGadget g = (DtIconGadget) w;

  call_data->x += G_X(g);
  call_data->y += G_Y(g);

  if (G_DropCallback(g)) {
     XtCallCallbackList(w, G_DropCallback(g), callData);
  }
}

/*-------------------------------------------------------------
**	_DtIconRegisterDropsite
**		Registers the Icon as a dropsite.
*/
void 
_DtIconRegisterDropsite(
        Widget w)

{
    XtCallbackRec transferCB[] = { {TransferCallback, NULL}, {NULL, NULL} };
    XtCallbackRec animateCB[] = { {AnimateCallback, NULL}, {NULL, NULL} };
    DtIconGadget g =	(DtIconGadget) w;
    XRectangle rects[2];
    unsigned char flags;
    int numRects = 0;
    Arg args[5];
    Cardinal n;

    _DtIconGetIconRects(g, &flags, &rects[0], &rects[1]);

    if (flags & XmPIXMAP_RECT) {
	rects[0].x -= G_X(g);
	rects[0].y -= G_Y(g);
	numRects++;
    }

    if (flags & XmLABEL_RECT) {
	rects[1].x -= G_X(g);
	rects[1].y -= G_Y(g);
       if (!numRects) rects[0] = rects[1];
       numRects++;
    }

    n = 0;
    if (numRects) {
	XtSetArg(args[n], XmNdropRectangles, rects); n++;
    	XtSetArg(args[n], XmNnumDropRectangles, numRects); n++;
    }
    XtSetArg(args[n], XmNanimationStyle, XmDRAG_UNDER_SHADOW_IN); n++;
    XtSetArg(args[n], DtNtextIsBuffer, True); n++;
    XtSetArg(args[n], DtNdropAnimateCallback, animateCB); n++;

    DtDndDropRegister(w, DtDND_FILENAME_TRANSFER|DtDND_BUFFER_TRANSFER,
	G_Operations(g), transferCB,
	args, n);
}
