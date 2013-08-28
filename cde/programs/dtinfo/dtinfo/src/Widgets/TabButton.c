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
/*
 * $XConsortium: TabButton.c /main/4 1996/06/11 16:46:03 cde-hal $
 *
 * Copyright (c) 1992 HaL Computer Systems, Inc.  All rights reserved.
 * UNPUBLISHED -- rights reserved under the Copyright Laws of the United
 * States.  Use of a copyright notice is precautionary only and does not
 * imply publication or disclosure.
 * 
 * This software contains confidential information and trade secrets of HaL
 * Computer Systems, Inc.  Use, disclosure, or reproduction is prohibited
 * without the prior express written permission of HaL Computer Systems, Inc.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions as set forth in subparagraph (c)(l)(ii) of the Rights in
 * Technical Data and Computer Software clause at DFARS 252.227-7013.
 *                        HaL Computer Systems, Inc.
 *                  1315 Dell Avenue, Campbell, CA  95008
 * 
 */

#include "TabButtonP.h"
#include <stdio.h>
#include <stdlib.h>
#include <Xm/Xm.h>
#include <Xm/IconButtonP.h>

/* **************************************************************
 * constant and type declarations
 * ************************************************************** */

enum { XmINVALID_MULTICLICK = 255, DEFAULT_DISARM_DELAY = 100 };

char _XtSCdisarmDelay[] = "disarmDelay";
char _XtSCDisarmDelay[] = "DisarmDelay";
char _XtSCradius[] = "radius";
char _XtSCRadius[] = "Radius";

#define XyzTAB_BUTTON_BIT 203

static XyzTabButtonCallbackStruct call_value;
static Boolean internal_expose;

/* Can't use XRectangle because we care about negative values. */
typedef struct
{
  int x, y, width, height;
} rect_t;

/* **************************************************************
 * macro definitions
 * ************************************************************** */

/* Cast macro. */
#define TabWidget(W) ((XyzTabButtonWidget)W)
#define TOP_GC ((tabw->tab.armed) ? \
   tabw->primitive.bottom_shadow_GC : tabw->primitive.top_shadow_GC)
#define BOTTOM_GC ((tabw->tab.armed) ? \
   tabw->primitive.top_shadow_GC : tabw->primitive.bottom_shadow_GC)

/* Casted widget macro to avoid excess local variables. */
#define nw              w
#define tabw		TabWidget(w)
#define tab_part	tabw->tab

/* **************************************************************
 * Function declarations.  Hope you have an ANSI C compiler.
 * ************************************************************** */

/* -------- Action procudures. -------- */

#define ACTION_PROC(PROC) \
  static void PROC (Widget w, XEvent *event, \
		    String *params, Cardinal *num_params)

ACTION_PROC (Arm);		ACTION_PROC (MultiArm);
ACTION_PROC (Activate); 	ACTION_PROC (MultiActivate);
ACTION_PROC (ActivateCommon);	ACTION_PROC (ArmAndActivate);
ACTION_PROC (Disarm);
ACTION_PROC (Enter);            ACTION_PROC (Leave);

/* -------- Assorted internal routines. -------- */

static void calc_min_size (Widget);
static void check_radius (Widget);
static void arm_timeout (XtPointer, XtIntervalId *);
static void get_arm_gc (Widget);
static void get_background_gc (Widget);
static void get_parent_bg_gc (Widget);
static void draw_background (Widget);
static void draw_label (Widget, XEvent *, Region);
static void draw_shadow (Widget w);
static void get_label_rect (Widget, rect_t *);
static void circle_to_arc (Position center_x, Position center_y,
			   Dimension radius, int start_angle, int sweep_angle,
			   Dimension pen_width, XArc *arc);
static void draw_highlight (Widget);
static void undraw_highlight (Widget);
static void draw_rounded_shadow (Display *, Drawable,
				GC top, GC bottom,
				Position x, Position y,
				Dimension width, Dimension height,
				Dimension thickness, Dimension radius,
				unsigned int type);

/* -------- Standard widget functions. -------- */

static void Initialize (Widget req, Widget new_w, ArgList, Cardinal *);
static Boolean SetValues (Widget current, Widget req, Widget new_w,
			  ArgList, Cardinal *);
static void Resize (Widget);
static void Destroy (Widget);
static void Redisplay (Widget, XEvent *, Region);

/* **************************************************************
 * Translation tables
 * ************************************************************** */

static char translations[] = "\
<Btn1Down>:		Arm() \n\
<Btn1Down>,<Btn1Up>:	Activate() Disarm() \n\
<Btn1Down>(2+):		MultiArm() \n\
<Btn1Up>(2+):		MultiActivate() \n\
<Btn1Up>:		Activate() Disarm() \n\
<Btn2Down>:		ProcessDrag() \n\
<EnterWindow>:		Enter() \n\
<LeaveWindow>:		Leave() \n\
<Key>osfActivate:	PrimitiveParentActivate() \n\
<Key>osfCancel:		PrimitiveParentCancel() \n\
<Key>osfSelect:		ArmAndActivate() \n\
<Key>osfHelp:		Help() \n\
~s ~m ~a <Key>Return:	PrimitiveParentActivate() \n\
~s ~m ~a <Key>space:	ArmAndActivate()";


/* **************************************************************
 * Action table
 * ************************************************************** */

static XtActionsRec actions[] =
{
  { "Arm", 		Arm		},
  { "MultiArm", 	MultiArm 	},
  { "Activate", 	Activate	},
  { "MultiActivate", 	MultiActivate	},
  { "ArmAndActivate", 	ArmAndActivate	},
  { "Disarm", 		Disarm		},
  { "Enter", 		Enter		},
  { "Leave",		Leave		},
};



/* **************************************************************
 * Resources
 * ************************************************************** */

extern void _XmSelectColorDefault(Widget,int,XrmValue*);

static XtResource resources[] = 
{
  /* -------- Specify tab resources here. -------- */

  { XmNfillOnArm, XmCFillOnArm, XmRBoolean, sizeof (Boolean),
    XtOffsetOf (struct _XyzTabButtonRec, tab.fill_on_arm),
    XmRImmediate, (XtPointer) True  },
  { XmNarmColor, XmCArmColor, XmRPixel, sizeof (Pixel),
    XtOffsetOf (struct _XyzTabButtonRec, tab.arm_color),
    XmRCallProc, (XtPointer)(size_t) _XmSelectColorDefault },
  { XmNarmPixmap, XmCArmPixmap, XmRPrimForegroundPixmap, sizeof (Pixmap),
    XtOffsetOf (struct _XyzTabButtonRec, tab.arm_pixmap),
    XmRImmediate, (XtPointer) XmUNSPECIFIED_PIXMAP },

  { XmNactivateCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
    XtOffsetOf (struct _XyzTabButtonRec, tab.activate_callback),
    XmRPointer, (XtPointer) NULL },

  { XmNarmCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
    XtOffsetOf (struct _XyzTabButtonRec, tab.arm_callback),
    XmRPointer, (XtPointer) NULL },

  { XmNdisarmCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
    XtOffsetOf (struct _XyzTabButtonRec, tab.disarm_callback),
    XmRPointer, (XtPointer) NULL },

  { XmNmultiClick, XmCMultiClick, XmRMultiClick, sizeof (unsigned char),
    XtOffsetOf (struct _XyzTabButtonRec, tab.multi_click),
    XmRImmediate, (XtPointer) XmINVALID_MULTICLICK },

  { XtNdisarmDelay, XtCDisarmDelay, XtRInt, sizeof (int),
    XtOffsetOf (struct _XyzTabButtonRec, tab.disarm_delay),
    XmRImmediate, (XtPointer) DEFAULT_DISARM_DELAY },

  { XtNradius, XtCRadius, XmRHorizontalDimension, sizeof(Dimension),
    XtOffsetOf (struct _XyzTabButtonRec, tab.radius),
    XmRImmediate, (XtPointer) 12 },

   /* -------- Override some Primitive resources here. -------- */
       
  { XmNshadowThickness, XmCShadowThickness, XmRHorizontalDimension,
    sizeof(Dimension),
    XtOffsetOf (struct _XyzTabButtonRec, primitive.shadow_thickness),
    XmRImmediate, (XtPointer) 2 },

  { XmNtraversalOn, XmCTraversalOn, XmRBoolean, sizeof(Boolean),
    XtOffsetOf (struct _XmPrimitiveRec, primitive.traversal_on),
    XmRImmediate, (XtPointer) True },

  { XmNhighlightThickness,  XmCHighlightThickness,  XmRHorizontalDimension,
    sizeof (Dimension),
    XtOffsetOf (struct _XmPrimitiveRec, primitive.highlight_thickness),
    XmRImmediate, (XtPointer) 2 },
};


XmPrimitiveClassExtRec tabButtonPrimClassExtRec =
{
  NULL,
  NULLQUARK,
  XmPrimitiveClassExtVersion,
  sizeof (XmPrimitiveClassExtRec),
  XmInheritBaselineProc,                  /* widget_baseline */
  XmInheritDisplayRectProc,               /* widget_display_rect */
};

XyzTabButtonClassRec xyzTabButtonClassRec =
{
  {
    /* -------- Core class record. -------- */
    /* superclass	  */	(WidgetClass) &xmLabelClassRec,
    /* class_name	  */	"XyzTabButton",
    /* widget_size	  */	sizeof (XyzTabButtonRec),
    /* class_initialize   */    NULL,
    /* class_part_init    */    NULL,
    /* class_inited       */	False,
    /* initialize	  */	Initialize,
    /* initialize_hook    */    NULL,
    /* realize		  */	XtInheritRealize,
    /* actions		  */	actions,
    /* num_actions	  */	XtNumber (actions),
    /* resources	  */	resources,
    /* num_resources	  */	XtNumber (resources),
    /* xrm_class	  */	NULLQUARK,
    /* compress_motion	  */	True,
    /* compress_exposure  */	XtExposeCompressMaximal,
    /* compress_enterlv   */    True,
    /* visible_interest	  */	False,
    /* destroy		  */	Destroy,
    /* resize		  */	Resize,
    /* expose		  */	Redisplay,
    /* set_values	  */	SetValues,
    /* set_values_hook    */    NULL,
    /* set_values_almost  */    XtInheritSetValuesAlmost,
    /* get_values_hook    */	NULL,
    /* accept_focus	  */	NULL,
    /* version            */	XtVersion,
    /* callback_private   */    NULL,
    /* tm_table           */    translations,
    /* query_geometry     */	XtInheritQueryGeometry, 
    /* display_accelerator */   NULL,
    /* extension record   */    NULL,
  },

  {
    /* -------- Primitive class record. -------- */
    /* Primitive border_highlight   */	draw_highlight,
    /* Primitive border_unhighlight */	undraw_highlight,
    /* translations		    */  XtInheritTranslations,
    /* arm_and_activate		    */  ArmAndActivate,
    /* get resources		    */  NULL,
    /* num get_resources	    */  0,
    /* extension		    */  (XtPointer)&tabButtonPrimClassExtRec,
  },

  {
    /* -------- Label class record. -------- */
    /* setOverrideCallback     */	   XmInheritWidgetProc,
    /* menu procedures         */	   XmInheritMenuProc,
    /* menu traversal xlation  */	   XtInheritTranslations,
    /* extension	       */	   NULL,
  }
};

/* -------- Class record declaration.  -------- */

WidgetClass xyzTabButtonWidgetClass = (WidgetClass)&xyzTabButtonClassRec;
  
/* **************************************************************
 * Arm - Arm button and call arm callbacks
 * ************************************************************** */

static void
Arm (Widget w, XEvent *event,  String *params, Cardinal *num_params)
{
  /* Don't traverse to here if no event was provided on the assumption
     that it was a program, rather than user, initiated action. */
  if (!tabw->primitive.have_traversal && event != NULL)
    XmProcessTraversal (w, XmTRAVERSE_CURRENT);

  tabw->tab.armed = True;
  if (event != NULL && event->type == ButtonPress)
    tabw->tab.last_arm_time = event->xbutton.time;
  else
    tabw->tab.last_arm_time = 0;

  /* -------- Fake an expose to draw tab in armed state. -------- */
  internal_expose = True;
  (*(XtClass(w)->core_class.expose)) (w, event, (Region) NULL);

  if (tabw->tab.arm_callback)
    {
      XFlush(XtDisplay (w));
      call_value.reason = XmCR_ARM;
      call_value.event = event;
      XtCallCallbackList (w, tabw->tab.arm_callback, &call_value);
    }
}


/* **************************************************************
 * MultiArm - Action called for multiple clicks
 * ************************************************************** */

static void
MultiArm (Widget w, XEvent *event, String *params, Cardinal *num_params)
{
  if (tabw->tab.multi_click == XmMULTICLICK_KEEP)
    Arm (w, event, NULL, NULL);
}


/* **************************************************************
 * Activate - Wonder Tab power, activate!
 * ************************************************************** */

static void 
Activate (Widget w, XEvent *event, String *params, Cardinal *num_params)
{
  /* NOTE: Should print warning in this case. */
  if (!tabw->tab.armed)
    return;

  tabw->tab.click_count = 1;
  ActivateCommon (w, event, params, num_params);
}


/* **************************************************************
 * MultiActicate - Activate for multiple clicks
 * ************************************************************** */

static void
MultiActivate (Widget w, XEvent *event, String *params, Cardinal *num_params)
{
  /* NOTE: Should print warning in this case. */
  if (event->type != ButtonRelease)
    return;

  if (tabw->tab.multi_click == XmMULTICLICK_KEEP)
    {
      tabw->tab.click_count++;
      ActivateCommon (w, event, params, num_params);
      Disarm (w, event, params, num_params);
    }
}


/* **************************************************************
 * ActivateCommon - Activate and MultiActivate common code.
 * ************************************************************** */

static void 
ActivateCommon (Widget w, XEvent *event, String *params, Cardinal *num_params)
{
  Dimension bw = tabw->core.border_width;
  
  tabw->tab.armed = False;

  internal_expose = True;
  (*(XtClass(w))->core_class.expose) (w, event, (Region) NULL);
  
  /* If the event was a button event, make sure it occured within
     the tab.  (The user could have released outside the tab.) */
  if (event->type != ButtonRelease ||
      (event->xbutton.x >= (int) -bw &&
       event->xbutton.y >= (int) -bw &&
       event->xbutton.x <  (int) (tabw->core.width + bw) &&
       event->xbutton.y <  (int) (tabw->core.height + bw)))
    {
      /* Bail out if this is a multiple click and we're ignoring them. */
      if (tabw->tab.click_count > 1 &&
	  tabw->tab.multi_click == XmMULTICLICK_DISCARD)
	return;

      if (tabw->tab.activate_callback)
	{
	  call_value.reason = XmCR_ACTIVATE;
	  call_value.event = event;
	  call_value.click_count = tabw->tab.click_count;
	  XFlush (XtDisplay (tabw));
	  XtCallCallbackList (w, tabw->tab.activate_callback, &call_value);
	}
    }
}


/* **************************************************************
 * ArmAndActivate
 * ************************************************************** */

static void 
ArmAndActivate (Widget w, XEvent *event, String *params, Cardinal *num_params)
{  
  Boolean already_armed = tabw->tab.armed;

  tabw->tab.armed = True;
  internal_expose = True;
  (*XtClass(w)->core_class.expose) (w, event, (Region) NULL);
  XFlush (XtDisplay (w));

  if (tabw->tab.arm_callback && !already_armed)
   {
      call_value.reason = XmCR_ARM;
      call_value.event = event;
      XtCallCallbackList(w, tabw->tab.arm_callback, &call_value);
   }

   call_value.event = event;
   call_value.click_count = 1;

   if (tabw->tab.activate_callback)
     {
       call_value.reason = XmCR_ACTIVATE;
       XFlush (XtDisplay (w));
       XtCallCallbackList (w, tabw->tab.activate_callback, &call_value);
     }

   tabw->tab.armed = False;
   
   if (tabw->tab.disarm_callback)
     {
       XFlush (XtDisplay (w));
       call_value.reason = XmCR_DISARM;
       XtCallCallbackList (w, tabw->tab.disarm_callback, &call_value);
     }

  /* Set up a timout to show the widget disarming, but only if it's
     not going to be destroyed and there's not already one pending */

  if (tabw->core.being_destroyed == False && tabw->tab.timer == 0)
    tabw->tab.timer = XtAppAddTimeOut (XtWidgetToApplicationContext(w),
				       tabw->tab.disarm_delay, arm_timeout,
				       (XtPointer) w);
}


/* **************************************************************
 * arm_timeout - Draw the tab disarming.
 * ************************************************************** */

static void
arm_timeout (XtPointer data, XtIntervalId *id)
{ 
  Widget w = (Widget) data;

  tabw->tab.timer = 0;

  if (XtIsRealized (w) && XtIsManaged (w))
    {
      internal_expose = True;
      (*XtClass(w)->core_class.expose)(w, NULL, (Region) NULL);
    }

  XFlush (XtDisplay (w));
}


/* **************************************************************
 * Disarm - Disarm and call appropriate callbacks.
 * ************************************************************** */

static void 
Disarm (Widget w, XEvent *event, String *params, Cardinal *num_params)
{
  /* Only redisplay if the tab is armed. */
  if (tabw->tab.armed == True)
    {
      tabw->tab.armed = False;
      internal_expose = True;
      (*XtClass(w)->core_class.expose) (w, NULL, (Region) NULL);
    }

  if (tabw->tab.disarm_callback)
    {
      call_value.reason = XmCR_DISARM;
      call_value.event = event;
      XtCallCallbackList (w, tabw->tab.disarm_callback, &call_value);
    }
}


/* **************************************************************
 * Enter
 * ************************************************************** */

static void 
Enter (Widget w, XEvent *event, String *params, Cardinal *num_params)
{
  _XmPrimitiveEnter (w, event, NULL, NULL);

  if (tabw->tab.armed)
    {
      internal_expose = True;
      (*XtClass(w)->core_class.expose) (w, event, (Region) NULL);
    }
}


/* **************************************************************
 * Leave
 * ************************************************************** */

static void 
Leave (Widget w, XEvent *event, String *params, Cardinal *num_params)
{
  _XmPrimitiveLeave (w, event, NULL, NULL);

  if (tabw->tab.armed)
    {
      tabw->tab.armed = False;
      internal_expose = True;
      (*XtClass(w)->core_class.expose) (w, event, (Region) NULL);
      tabw->tab.armed = True;
    }
}


/* **************************************************************
 * Initialize
 * ************************************************************** */

static void 
Initialize (Widget rw, Widget nw, ArgList args, Cardinal *num_args)
{
  if (tabw->tab.multi_click == XmINVALID_MULTICLICK)
    tabw->tab.multi_click = XmMULTICLICK_KEEP;

  tabw->tab.armed = False;
  tabw->tab.timer = 0;

  /* NOTE: really, screw with margins instead.  Write a proc
     to calculate label margins. */

  /* If we have an arm pixmap, but no unarm, use it for both. */
  if (tabw->label.pixmap == XmUNSPECIFIED_PIXMAP &&
      tabw->tab.arm_pixmap != XmUNSPECIFIED_PIXMAP)
    {
      tabw->label.pixmap = tabw->tab.arm_pixmap;
      if (XtWidth (rw) == 0)
	XtWidth (nw) = 0;
      if (XtHeight (rw) == 0)
	XtHeight (nw) = 0;

      _XmCalcLabelDimensions (nw);
      Resize (nw);
    }

  tabw->tab.unarm_pixmap = tabw->label.pixmap;

  tabw->tab.background_gc = tabw->tab.arm_gc = tabw->tab.parent_bg_gc = NULL;
  
  get_arm_gc (nw);
  get_background_gc (nw);
  get_parent_bg_gc (w);
}


/* **************************************************************
 * get_arm_gc
 * ************************************************************** */

static void 
get_arm_gc (Widget w)
{
  XGCValues values;
  XtGCMask  value_mask;

  value_mask = GCForeground | GCFillStyle;

  values.foreground = tabw -> tab.arm_color;
  values.fill_style = FillSolid;

  if (tabw->tab.arm_gc)
    XtReleaseGC (w, tabw->tab.arm_gc);
  tabw->tab.arm_gc = XtGetGC (w, value_mask, &values);
}


/* **************************************************************
 * get_background_gc
 * ************************************************************** */

static void 
get_background_gc (Widget w)
{
  XGCValues        values;
  XtGCMask         value_mask;
  short            myindex;
  XFontStruct     *fs;

  value_mask = GCForeground | GCBackground | GCGraphicsExposures;
			  
  _XmFontListSearch(tabw->label.font, XmFONTLIST_DEFAULT_TAG, &myindex, &fs);
  values.foreground = tabw->core.background_pixel;
  values.background = tabw->primitive.foreground;
  values.graphics_exposures = False;

  if (fs != NULL)
    {
      value_mask |= GCFont;
      values.font = fs->fid;
    }

  if (tabw->tab.background_gc)
    XtReleaseGC (w, tabw->tab.background_gc);
  tabw->tab.background_gc = XtGetGC (w, value_mask, &values);
}


/* **************************************************************
 * get_parent_bg_gc
 * ************************************************************** */

static void
get_parent_bg_gc (Widget w)
{
  Widget    parent;
  XGCValues values;
  XtGCMask  value_mask;

  parent = XtParent(w);

  value_mask = GCForeground /* | GCBackground */ | GCGraphicsExposures;
  values.foreground = parent->core.background_pixel;
  values.graphics_exposures = False;

  if (parent->core.background_pixmap != XtUnspecifiedPixmap)
    {
      value_mask |= GCFillStyle | GCTile;
      values.fill_style = FillTiled;
      values.tile = parent->core.background_pixmap;
    }

  if (tabw->tab.parent_bg_gc)
    XtReleaseGC (w, tabw->tab.parent_bg_gc);
  tabw->tab.parent_bg_gc = XtGetGC (parent, value_mask, &values);
}


/* **************************************************************
 * calc_min_size
 * ************************************************************** */

static void
calc_min_size (Widget w)
{
  /* Should really be double this, but it seems to work... */
  /* I just don't have time to analyze all drawing requests, but
     we would like to see something when it's small... */
  tabw->tab.min_size = (
    tabw->primitive.highlight_thickness +
    tabw->primitive.shadow_thickness +
    2 /* label area */);
}


/* **************************************************************
 * check_radius - clip radius internally if necessary
 * ************************************************************** */

/* Radius checking depends on user radius setting, shadow thickness,
   and widget width and height. */

void check_radius (Widget w)
{
  Position   offset         =  2 * tabw->primitive.highlight_thickness;
  Dimension  sthickness     =  tabw->primitive.shadow_thickness;
  Dimension  width          =  XtWidth(w) - offset;
  Dimension  height         =  XtHeight(w) - offset;
  Dimension  radius         =  tabw->tab.radius;

  if ((radius * 2) > width)
    radius = (width / 2);

  /* Radius must never, ever be negative! */
  if (radius + sthickness > height)
    radius = (sthickness > height) ? 0 : height - sthickness;

  /* Minimum radius is 1/2 shadow thickness. */
  if (radius < (sthickness - 1) / 2)
    radius = (sthickness - 1) / 2;

  tabw->tab.current_radius = radius;
}


/* **************************************************************
 * Resize
 * ************************************************************** */

static void
Resize (Widget w)
{
  (*xmLabelClassRec.core_class.resize) (w);

  calc_min_size (w);
  check_radius (w);
}

/* **************************************************************
 * SetValues
 * ************************************************************** */

static Boolean 
SetValues (Widget cw, Widget rw, Widget nw, ArgList args, Cardinal *num_args)
{
   XyzTabButtonWidget current = (XyzTabButtonWidget) cw;
   Boolean redisplay = False;

   if (tabw->tab.arm_pixmap != current->tab.arm_pixmap &&
       tabw->label.label_type == XmPIXMAP && tabw->tab.armed) 
     redisplay = True;
      
   if (tabw->label.pixmap == XmUNSPECIFIED_PIXMAP &&
       tabw->tab.arm_pixmap != XmUNSPECIFIED_PIXMAP)
     {
       tabw->label.pixmap = tabw->tab.arm_pixmap;
       if (tabw->label.recompute_size)
	 {
	   if (XtWidth (rw) == XtWidth (cw))
	     XtWidth (nw) = 0;
	   if (XtHeight (rw) == XtHeight (cw))
	     XtHeight (nw) = 0;
	 }

       _XmCalcLabelDimensions (nw);
       Resize (w);
     }

   if (tabw->label.pixmap != current->label.pixmap &&
       tabw->label.label_type == XmPIXMAP && !tabw->tab.armed)
     {
       tabw->tab.unarm_pixmap = tabw->label.pixmap;
       redisplay = True;
     }

   if (tabw->tab.arm_color != current->tab.arm_color)
     {
       get_arm_gc (nw);
       if (tabw->tab.armed)
	 redisplay = True;
     }

   if (tabw->core.background_pixel != current->core.background_pixel)
     {
       get_background_gc (nw);
       if (!tabw->tab.armed)
	 redisplay = True;
     }

   if (tabw->tab.fill_on_arm != current->tab.fill_on_arm && tabw->tab.armed)
     redisplay = True;

   if (tabw->primitive.shadow_thickness !=
         current->primitive.shadow_thickness ||
       tabw->primitive.highlight_thickness !=
         current->primitive.highlight_thickness)
     {
       calc_min_size (nw);
       check_radius (nw);
       redisplay = True;
     }

   if (tabw->tab.radius != current->tab.radius)
     {
       calc_min_size (nw);
       check_radius (nw);
       redisplay = True;
     }

   return (redisplay);
}


/* **************************************************************
 * Destroy
 * ************************************************************** */

static void 
Destroy (Widget w)
{
  if (tabw->tab.timer)
    XtRemoveTimeOut (tabw->tab.timer);

  XtReleaseGC (w, tabw->tab.arm_gc);
  XtReleaseGC (w, tabw->tab.background_gc);
  if (tabw->tab.parent_bg_gc != NULL)
    XtReleaseGC (w, tabw->tab.parent_bg_gc);

  XtRemoveAllCallbacks (w, XmNactivateCallback);
  XtRemoveAllCallbacks (w, XmNarmCallback);
  XtRemoveAllCallbacks (w, XmNdisarmCallback);
}


/* **************************************************************
 * XyzCreateTabButton
 * ************************************************************** */

Widget 
XyzCreateTabButton (Widget parent, const char *name,
		    ArgList arglist, Cardinal argcount)
{
  return (XtCreateWidget (name, xyzTabButtonWidgetClass, 
			  parent, arglist, argcount));
}

/* **************************************************************
 * Redisplay
 * ************************************************************** */

static void 
Redisplay (Widget w, XEvent *event, Region region)
{
  if (!XtIsRealized (w))
    return;

  /* Don't attempt to draw the widget if it's too small. */
  if (XtWidth(w) < tabw->tab.min_size || XtHeight(w) < tabw->tab.min_size)
    return;

  draw_background (w);
  draw_label (w, event, region);
  draw_shadow (w);
  internal_expose = False;
}


/* **************************************************************
 * draw_background
 * ************************************************************** */

static void 
draw_background (Widget w)
{
  GC         tmp_gc;
  static rect_t lab;
  static XRectangle rects[2];
  static XArc       arcs[2];
  int radius = tabw->tab.current_radius - tabw->primitive.shadow_thickness;
  int r = 0;
  Boolean different_bg = _XmDifferentBackground (w, XtParent (w));

  get_label_rect (w, &lab);
  if (lab.height <= 0 || lab.width <= 0)
    return;

  if (different_bg && !internal_expose)
    {
      rects[0].x = 0;
      rects[1].x = XtWidth(w) - tabw->primitive.highlight_thickness -
	           tabw->tab.current_radius;
      rects[0].y = rects[1].y = 0;
      rects[0].width = rects[1].width =
      rects[0].height = rects[1].height =
        tabw->primitive.highlight_thickness + tabw->tab.current_radius;

      XFillRectangles (XtDisplay (w), XtWindow (w), tabw->tab.parent_bg_gc,
		       rects, 2);
      /* Take care of internal label border when bg matches shadow color.
         The arc case is handled in the computation of label_rect later.
	 (Technically this should be in the foreground color of this
	 widget, but it's a pain to draw the 1 pixel-wide arc, so we use
	 the background color of our parent instead.) */
      if (tabw->tab.arm_color == tabw->primitive.top_shadow_color ||
	  tabw->tab.arm_color == tabw->primitive.bottom_shadow_color)
	{
	  XSegment lines[3];
	  /* side edges */
	  lines[0].x1 = lines[0].x2 = lab.x - 1;
	  lines[1].x1 = lines[1].x2 = lab.x + lab.width;
	  lines[0].y1 = lines[1].y1 = lab.y + lab.height;
	  lines[0].y2 = lines[1].y2 = lab.y + radius - 1;
	  /* top edge */
	  lines[2].y1 = lines[2].y2 = lab.y - 1;
	  lines[2].x1 = lab.x + radius - 1;
	  lines[2].x2 = lab.x + lab.width - radius;
	  XDrawSegments (XtDisplay(w), XtWindow(w),
			 tabw->tab.parent_bg_gc, lines, 3);
	}
    }

  /* External expose and not armed means background already correct,
     unless there's a different bg, then we must to draw over extra
     parts of the rectangles from above. */
  if (!different_bg && !internal_expose && !tabw->tab.armed)
    return;

  /* Now draw the background of the button area. */
  if (tabw->tab.armed && tabw->tab.fill_on_arm)
    tmp_gc = tabw->tab.arm_gc;
  else
    tmp_gc = tabw->tab.background_gc;

  if (radius < 0)
    return;

  if (radius > 0)
    {
      /* Filled arcs need radius + 1 because they don't overlap rect bounds */
      circle_to_arc (lab.x + radius, lab.y + radius,
		     radius + 1, 90, 180, 0, &arcs[0]);
      circle_to_arc (lab.x + lab.width - radius - 1, lab.y + radius,
		     radius + 1, 0, 90, 0, &arcs[1]);
      XFillArcs (XtDisplay (w), XtWindow (w), tmp_gc, arcs, 2);

      /* Don't need to draw other internal stuff, just the corners, if
	 we're not armed, because background is already correct. */
      if (!internal_expose && !tabw->tab.armed)
	return;

      rects[r].x = lab.x + radius;
      rects[r].y = lab.y;
      rects[r].width = lab.width - (radius * 2);
      rects[r].height = radius;
      r++;
    }

  rects[r].x = lab.x;
  rects[r].y = lab.y + radius;
  rects[r].width = lab.width;
  rects[r].height = lab.height - radius;
  r++;

  XFillRectangles (XtDisplay (w), XtWindow (w), tmp_gc, rects, r);
}


/* **************************************************************
 * draw_label
 * ************************************************************** */

static void 
draw_label (Widget w, XEvent *event, Region region)
{  
  GC tmp_gc = NULL;
  Boolean swap_gcs = False;

  /* If the arm color is the same as the foreground,
     swap in a new gc so that text will be visible. */
  if (tabw->tab.fill_on_arm && tabw->label.label_type == XmSTRING &&
      tabw->tab.armed && tabw->tab.arm_color == tabw->primitive.foreground)
    {
      tmp_gc = tabw->label.normal_GC;
      tabw->label.normal_GC = tabw->tab.background_gc;
      swap_gcs = True;
    }

  /* Pick the right pixmap to display based on the button state. */
  if (tabw->label.label_type == XmPIXMAP)
    {
      if (tabw->tab.armed)
	if (tabw->tab.arm_pixmap != XmUNSPECIFIED_PIXMAP)
	  tabw->label.pixmap = tabw->tab.arm_pixmap;
	else
	  tabw->label.pixmap = tabw->tab.unarm_pixmap;
      else
	tabw->label.pixmap = tabw->tab.unarm_pixmap;
    }

  (*xmLabelClassRec.core_class.expose) (w, event, region);

  /* Swap the GCs back if necessary. */
  if (swap_gcs)
    tabw->label.normal_GC = tmp_gc;
}


/* **************************************************************
 * draw_shadow
 * ************************************************************** */

static void 
draw_shadow (Widget w)
{
  if (tabw->primitive.shadow_thickness > 0)
    {
      int offset = tabw->primitive.highlight_thickness;

      draw_rounded_shadow (XtDisplay (w), XtWindow (w),
			   TOP_GC, BOTTOM_GC,
			   offset, offset,
			   XtWidth (w) - 2*offset, XtHeight (w) - 2*offset,
			   tabw->primitive.shadow_thickness,
			   tabw->tab.current_radius, XmSHADOW_OUT);
    }
}


/* **************************************************************
 * get_label_rect
 * ************************************************************** */

static void
get_label_rect (Widget w, rect_t *rect)
{
  int dx;
  short fill = 0;

  /* Make some room between shadows and label area if the colors are
     the same to avoid bad effects in monochrome. */
  if (tabw->tab.arm_color == tabw->primitive.top_shadow_color ||
      tabw->tab.arm_color == tabw->primitive.bottom_shadow_color)
    fill = 1;

  dx = tabw->primitive.highlight_thickness +
       tabw->primitive.shadow_thickness + fill;

  rect->x = dx;
  rect->y = dx;
  rect->width = tabw->core.width - dx - dx;
  rect->height = tabw->core.height - dx - dx;
}


/* **************************************************************
 * draw_arcs - Draw the arc parts of the shadow
 * ************************************************************** */

static void
draw_arcs (Display *display, Drawable drawable,
	   GC top_gc, GC bottom_gc,
	   Position x, Position y, Dimension width, Dimension height,
	   Dimension thickness, Dimension radius,
	   unsigned int type)
{
  static XArc arcs[3];

  circle_to_arc (x + radius, y + radius, radius, 90, 180, thickness, &arcs[0]);
  circle_to_arc (x + width - radius - 1, y + radius,
		 radius, 45, 90, thickness, &arcs[1]);
  circle_to_arc (x + width - radius - 1, y + radius,
		 radius, 0, 45, thickness, &arcs[2]);

  XSetLineAttributes (display, top_gc,
		      thickness, LineSolid, CapButt, JoinMiter);
  XSetLineAttributes (display, bottom_gc,
		      thickness, LineSolid, CapButt, JoinMiter);
  XDrawArcs (display, drawable, top_gc, &arcs[0], 2);
  XDrawArcs (display, drawable, bottom_gc, &arcs[2], 1);
  XSetLineAttributes (display, top_gc,
		      0, LineSolid, CapButt, JoinMiter);
  XSetLineAttributes (display, bottom_gc,
		      0, LineSolid, CapButt, JoinMiter);
}


/* **************************************************************
 * draw_rounded_shadow
 * ************************************************************** */

static void
draw_rounded_shadow (Display *display, Drawable drawable,
 		     GC top_gc, GC bottom_gc,
	 	     Position x, Position y, Dimension width, Dimension height,
		     Dimension thickness, Dimension radius,
		     unsigned int type)
{
  static XSegment *segs= NULL;
  static int seg_count = 0;
  int i, segs_needed = thickness * 4;
  /* precompute array indicies */ 
  int top =    thickness * 0,
      left   = thickness * 1,
      bottom = thickness * 2,
      right =  thickness * 3;

  if (segs_needed > seg_count)
    segs = (XSegment *)
      XtRealloc ((char *) segs, sizeof (XSegment) * segs_needed);

  for (i = 0; i < thickness; i++)
    {
      segs[i + top].x1 = x + radius;
      segs[i + top].x2 = x + width - radius;
      segs[i + top].y1 = segs[i + top].y2 = y + i;

      segs[i + left].x1 = segs[i + left].x2 = x + i;
      segs[i + left].y1 = y + radius;
      segs[i + left].y2 = y + height - i - 1;

      segs[i + right].x1 = segs[i + right].x2 = x + width - i - 1;
      segs[i + right].y1 = y + radius;
      segs[i + right].y2 = y + height - thickness - 1;
      
      segs[i + bottom].x1 = x + i + 1;
      segs[i + bottom].x2 = x + width - 1;
      segs[i + bottom].y1 = segs[i + bottom].y2 = y + height - i - 1;
    }


  XDrawSegments (display, drawable, top_gc, &segs[0], bottom);
  XDrawSegments (display, drawable, bottom_gc, &segs[bottom], bottom);

  draw_arcs (display, drawable, top_gc, bottom_gc,
	     x, y, width, height, thickness, radius, type);
}


/* **************************************************************
 * draw_highlight
 * ************************************************************** */

static void
draw_highlight (Widget w)
{
  Dimension thickness = tabw->primitive.highlight_thickness;

  tabw->primitive.highlighted = True;
  tabw->primitive.highlight_drawn = True;

  if (XtWidth(w) == 0 || XtHeight(w) == 0 ||
      tabw->primitive.highlight_thickness == 0)
    return;

  draw_rounded_shadow (XtDisplay(w), XtWindow(w),
 		       tabw->primitive.highlight_GC,
	 	       tabw->primitive.highlight_GC,
		       0, 0, XtWidth(w), XtHeight(w),
		       thickness, tabw->tab.current_radius + thickness,
		       XmSHADOW_OUT);
}


/* **************************************************************
 * undraw_highlight
 * ************************************************************** */

static
void undraw_highlight (Widget w)
{
  Dimension thickness=  tabw->primitive.highlight_thickness;

  tabw->primitive.highlighted = False;
  tabw->primitive.highlight_drawn = False;

  if (XtWidth(w) == 0 || XtHeight(w) == 0 ||
      tabw->primitive.highlight_thickness == 0)
    return;

  draw_rounded_shadow (XtDisplay(w), XtWindow(w),
		       tabw->tab.parent_bg_gc,
		       tabw->tab.parent_bg_gc,
		       0, 0, XtWidth(w), XtHeight(w),
		       thickness, tabw->tab.current_radius + thickness,
		       XmSHADOW_OUT);
}


/* **************************************************************
 * circle_to_arc - given circle parmeters, compute arc
 * ************************************************************** */

static void
circle_to_arc (Position center_x, Position center_y, Dimension radius,
	       int start_angle, int end_angle, Dimension pen_width,
	       XArc *arc)
{
  Dimension arc_radius;
  Dimension half_pen_width = (pen_width - 1) / 2;

  /* X draws with it's pen centered on the given radius, but we want
     our arcs to be filled with the pen inside the radius.  */
  /* Set the arc radius so that the pen will just hit our desired radius. */
  arc_radius = radius - half_pen_width;

  /* XArc is specified as bounding box. */
  arc->x = center_x - arc_radius;
  arc->y = center_y - arc_radius;
  arc->width = arc_radius * 2;
  arc->height = arc_radius * 2;
  arc->angle1 = start_angle * 64;
  arc->angle2 = (end_angle - start_angle) * 64;
  /* Correct for X drawing 1 pixel on rect border at these two angles. */
  if (end_angle == 90 || end_angle == 270)
    arc->angle2--;

  if (arc->width > 4000 || arc->height > 4000)
    {
      fprintf (stderr, "Internal tab widget error.  Aborting to prevent X crash.\n");
      fprintf (stderr, "radius = %d, pen_width = %d, half_pen_width = %d\n",
	       radius, pen_width, half_pen_width);
      abort();
    }
}
