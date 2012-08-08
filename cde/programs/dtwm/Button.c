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
/* $XConsortium: Button.c /main/7 1995/11/14 10:03:27 rswiston $ */
#ifdef REV_INFO
#ifndef lint
static char SCCSID[] = "OSF/Motif: @(#)Button.c	1.19 95/05/01";
#endif /* lint */
#endif /* REV_INFO */
/******************************************************************************
*******************************************************************************
*
*  (c) Copyright 1992 HEWLETT-PACKARD COMPANY
*  ALL RIGHTS RESERVED
*  
*******************************************************************************
******************************************************************************/
#include <stdio.h>
#include <X11/cursorfont.h>
#include "ButtonP.h"
#include <Xm/ManagerP.h>
#include <Xm/DrawP.h>
#include <Dt/Control.h>
#include <Dt/MacrosP.h>
#include <Dt/DtStrDefs.h>

#define DELAY_DEFAULT 100	

static void Initialize( 
                        Widget request_w,
                        Widget new_w) ;
static void GetGCs( 
                        Widget w) ;
static void UpdateGCs( 
			Widget w) ;
static void Redisplay( 
                        Widget w,
                        XEvent *event,
                        Region region) ;
static Boolean SetValues( 
                        Widget current_w,
                        Widget request_w,
                        Widget new_w) ;
static void Arm( 
                        Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
static void Disarm( 
                        Widget w,
                        XEvent *event,
			String *params,
        		Cardinal *num_params) ;
static void Activate( 
                        Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
static void ArmAndActivate( 
                        Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
static void InputDispatch(
                        Widget w,
                        XButtonEvent *event,
                        Mask event_mask) ;
static Boolean VisualChange(
                        Widget w,
                        Widget current_w,
                        Widget new_w) ;
static void ArmTimeout( 
                        Widget w,
                        XtIntervalId *id) ;
static void Enter( 
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
static void Leave( 
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
static void CallCallback( 
                        DtButtonGadget g,
                        XtCallbackList cb,
                        int reason,
                        XEvent *event) ;
static void DefaultSelectColor(
			Widget widget,
			int offset,
			XrmValue *value) ;

/*  Resource list for button  */

#define R_Offset(field) \
	XtOffset (DtButtonGadget, button.field)
static XtResource resources[] = 
{
	{
		XmNcursorFont,
		XmCCursorFont, XmRInt, sizeof (int),
		R_Offset (cursor_font),
		XmRImmediate, (XtPointer) 1
	},
	{
		XmNarmColor,
		XmCArmColor, XmRPixel, sizeof (Pixel),
		R_Offset (arm_color),
		XmRCallProc, (XtPointer) DefaultSelectColor
	},
	{
		XmNsubpanel,
		XmCSubpanel, XmRWidget, sizeof (Widget),
		R_Offset (subpanel),
		XmRImmediate, (XtPointer) NULL
	},
	{
		XmNpushFunction,
		XmCPushFunction, XmRFunction, sizeof (XtPointer),
		R_Offset (push_function),
		XmRImmediate, (XtPointer) NULL
	},
	{
		XmNpushArgument,
		XmCPushArgument, XmRPointer, sizeof (XtPointer),
		R_Offset (push_argument),
		XmRImmediate, (XtPointer) NULL
	},
	{
		XmNimageName,
		XmCString, XmRString, sizeof (String),
		R_Offset (image_name),
		XmRImmediate, (XtPointer) ""
	},

   	{
     		XmNcallback,
		XmCCallback, XmRCallback, sizeof(XtCallbackList),
     		R_Offset (callback),
     		XmRImmediate, (caddr_t) NULL
   	},

	{
     		XmNmultiClick,
     		XmCMultiClick, XmRMultiClick, sizeof (unsigned char),
     		R_Offset (multiClick),
     		XmRImmediate, (XtPointer) XmMULTICLICK_KEEP
   	}
};


/*  The button class record definition  */

externaldef (dtbuttongadgetclassrec)
        DtButtonGadgetClassRec dtButtonGadgetClassRec=
{
   {
      (WidgetClass) &xmGadgetClassRec,  /* superclass            */	
      "DtButton",                       /* class_name	         */	
      sizeof(DtButtonGadgetRec),        /* widget_size	         */	
      NULL,                             /* class_initialize      */    
      NULL,                             /* class_part_initialize */
      FALSE,                            /* class_inited          */	
      (XtInitProc) Initialize,          /* initialize	         */	
      NULL,                             /* initialize_hook       */
      NULL,                             /* realize	         */	
      NULL,                             /* actions               */	
      0,                                /* num_actions    	 */	
      resources,                        /* resources	         */	
      XtNumber(resources),              /* num_resources         */	
      NULLQUARK,                        /* xrm_class	         */	
      TRUE,                             /* compress_motion       */	
      XtExposeCompressMaximal,          /* compress_exposure     */	
      TRUE,                             /* compress_enterleave   */
      FALSE,                            /* visible_interest      */	
      NULL,                             /* destroy               */	
      NULL,                             /* resize                */
      (XtExposeProc) Redisplay,         /* expose                */	
      (XtSetValuesFunc) SetValues,      /* set_values	         */	
      NULL,                             /* set_values_hook       */
      XtInheritSetValuesAlmost,         /* set_values_almost     */
      NULL,                             /* get_values_hook       */
      NULL,                             /* accept_focus	         */	
      XtVersion,                        /* version               */
      NULL,                             /* callback private      */
      NULL,                             /* tm_table              */
      NULL,                             /* query_geometry        */
      NULL,				/* display_accelerator   */
      NULL,				/* extension             */
   },

   /*      XmGadget Part     */
   {
      (XtWidgetProc) _XtInherit,   	   /* Gadget border_highlight   */
      (XtWidgetProc) _XtInherit,  	   /* Gadget border_unhighlight */
      ArmAndActivate,    		   /* arm_and_activate          */
      (XmWidgetDispatchProc)InputDispatch, /* input_dispatch       	*/
      VisualChange,                        /* visual_change        	*/
      NULL,				   /* get resources      	*/
      0,				   /* num get_resources  	*/
      NULL,                                /* class_cache_part     	*/
      NULL,         			   /* extension                 */
   }

};

externaldef(dtbuttonwidgetclass) WidgetClass dtButtonGadgetClass =
			  (WidgetClass) &dtButtonGadgetClassRec;


/************************************************************************
 *
 *  Initialize
 *     The main widget instance initialization routine.
 *
 ************************************************************************/

static void 
Initialize(
        Widget request_w,
        Widget new_w )
{
   DtButtonGadget request = (DtButtonGadget) request_w ;
   DtButtonGadget new_g = (DtButtonGadget) new_w ;
   XmManagerWidget mw = (XmManagerWidget) XtParent(new_w) ;

   new_g->button.timer = 0;
   new_g->button.click_count = 0;
   new_g->button.armTimeStamp = 0;
   new_g->button.activateTimeStamp = 0;

   B_Armed (new_g) = False;

/* Set the input mask for events handled by Manager.
*/
   G_EventMask (new_g) = (XmARM_EVENT | XmACTIVATE_EVENT |
                        XmMULTI_ARM_EVENT | XmMULTI_ACTIVATE_EVENT |
                        XmHELP_EVENT | XmFOCUS_IN_EVENT |
                        XmFOCUS_OUT_EVENT | XmENTER_EVENT | XmLEAVE_EVENT);

/*	Get pixmap and mask, get pixmap geometry, get gcs.
 */
   if (B_ImageName (new_g) != NULL)
   {
       String str = B_ImageName (new_g);
       B_ImageName (new_g) = XtMalloc (strlen (str) + 1);
       strcpy (B_ImageName (new_g), str);

       B_Pixmap (new_g) =
		   XmGetPixmap (XtScreen (new_g), B_ImageName (new_g),
				   M_BottomShadowColor (mw),
				   M_TopShadowColor (mw));
       B_Mask (new_g) =
		   _DtGetMask (XtScreen (new_g), B_ImageName (new_g));

       if (B_Pixmap (new_g) != XmUNSPECIFIED_PIXMAP)
       {
	   Window		root;
	   int		int_x = 0, int_y = 0;
	   unsigned int	int_w = 0, int_h = 0,
			   int_bw, depth;

	   XGetGeometry (XtDisplay (new_g), B_Pixmap (new_g),
			   &root, &int_x, &int_y, &int_w, &int_h,
			   &int_bw, &depth);

	   B_PixmapWidth (new_g) = (Dimension) int_w;
	   B_PixmapHeight (new_g) = (Dimension) int_h;
       }
       else
       {
	   B_PixmapWidth (new_g) = 0;
	   B_PixmapHeight (new_g) = 0;
       }
   }
   else
   {
       B_Pixmap (new_g) = XmUNSPECIFIED_PIXMAP;
       B_Mask (new_g) = XmUNSPECIFIED_PIXMAP;
       B_PixmapWidth (new_g) = 0;
       B_PixmapHeight (new_g) = 0;
   }

   GetGCs (new_w);

/*	Set widget geometry.
 */
   new_g -> gadget.shadow_thickness = 1;
   new_g -> gadget.highlight_thickness = 1;
   if (G_Width(request) == 0)
       {
       if (B_PixmapWidth (new_g) > 0)
	   G_Width(new_g) = B_PixmapWidth (new_g) + 4;
       else
	   G_Width(new_g) = 10;
       }
   if (G_Height(request) == 0)
       {
       if (B_PixmapHeight (new_g) > 0)
	   G_Height(new_g) = B_PixmapHeight (new_g) + 4;
       else
	   G_Height(new_g) = 10;
       }
}




/************************************************************************
 *
 *  GetGCs
 *     Get the graphics context used for drawing the button.
 *
 ************************************************************************/

static void 
GetGCs(
        Widget w )
{
DtButtonGadget bg = (DtButtonGadget) w ;
XmManagerWidget mw = (XmManagerWidget) XtParent(w) ;
XGCValues values;
XtGCMask  valueMask;

valueMask = GCForeground | GCBackground;

values.foreground = M_Foreground(mw);
values.background = M_Background(mw);
bg -> button.gc_normal = XtGetGC ((Widget) mw, valueMask, &values);

values.foreground = B_ArmColor (bg);
bg -> button.gc_armed_bg = XtGetGC ((Widget) mw, valueMask, &values);

values.foreground = M_Background(mw);
values.background = M_Foreground(mw);
bg -> button.gc_background = XtGetGC ((Widget) mw, valueMask, &values);

/*	Get Clip GC
 */
if (B_Mask(bg) != XmUNSPECIFIED_PIXMAP) 
    {
    valueMask = GCForeground | GCBackground | GCClipMask;
    values.clip_mask = B_Mask(bg);
    values.foreground = M_Foreground (mw);
    values.background = M_Background (mw);
    bg->button.gc_clip = XtGetGC ((Widget) mw, valueMask, &values);
    }
else
    bg->button.gc_clip = NULL;
}


static void 
UpdateGCs(
        Widget w )
{
   DtButtonGadget bg = (DtButtonGadget) w ;
   XmManagerWidget mw = (XmManagerWidget) XtParent(w) ;
   XGCValues values;

   XtReleaseGC ((Widget) mw, bg->button.gc_normal);
   XtReleaseGC ((Widget) mw, bg->button.gc_background);
   XtReleaseGC ((Widget) mw, bg->button.gc_armed_bg);

   if (bg->button.gc_clip != NULL) 
      XtReleaseGC ((Widget) mw, bg->button.gc_clip);

   GetGCs (w);
}


/************************************************************************
 *
 *  Redisplay
 *     General redisplay function called on exposure events.
 *
 ************************************************************************/
/* ARGSUSED */
static void 
Redisplay(
        Widget w,
        XEvent *event,
        Region region )
{
DtButtonGadget bg = (DtButtonGadget) w;
XmManagerWidget mw = (XmManagerWidget) XtParent(w);
Dimension	s_t = bg -> gadget.shadow_thickness;
Dimension	h_t = bg -> gadget.highlight_thickness;
Position	x;
Pixmap	pix;
GC	gc;

gc = (B_Armed (bg)) ? bg -> button.gc_armed_bg : bg -> button.gc_background;
XFillRectangle (XtDisplay (bg), XtWindow (bg),
                gc, G_X(bg) +s_t + h_t, G_Y(bg) + s_t + h_t,
		G_Width(bg) - 2 * (s_t + h_t),
		G_Height(bg) - 2 * (s_t + h_t));

if (s_t > 0)
    {
    GC	gc1, gc2;
    if (B_Armed (bg))
	{
	gc1 = XmParentBottomShadowGC(w);
	gc2 = XmParentTopShadowGC(w);
	}
    else
	{
	gc1 = bg -> button.gc_background;
	gc2 = bg -> button.gc_background;
	}
    XmeDrawShadows (XtDisplay(bg), XtWindow (bg), gc1, gc2, 
                          G_X(bg) + h_t, G_Y(bg) + h_t,
			  G_Width(bg) - 2 * h_t, G_Height(bg) - 2 * h_t,
			  s_t,XmSHADOW_OUT);
    }

x = G_X(bg) + ((Position)(G_Width(bg) - B_PixmapWidth (bg))/2);
if (x % 2 == 1)
    x++;

if (B_Mask (bg) != XmUNSPECIFIED_PIXMAP)
    {
    gc = bg->button.gc_clip;
    XSetClipMask (XtDisplay(bg), gc, B_Mask (bg));
    XSetClipOrigin (XtDisplay(bg), gc,
		x, G_Y(bg) + (Position)(G_Height(bg) - B_PixmapHeight (bg))/2);
    }
else
    gc = bg->button.gc_normal;

pix = B_Pixmap (bg);
if ((gc != NULL) && (pix != XmUNSPECIFIED_PIXMAP))
    {   
    XCopyArea (XtDisplay ((Widget) bg), pix, XtWindow ((Widget) bg), gc, 0, 0,
		B_PixmapWidth (bg), B_PixmapHeight (bg),
		x, G_Y(bg) + (Position)(G_Height(bg) - B_PixmapHeight (bg))/2);
    }

if (bg -> gadget.highlighted)
   (*(xmGadgetClassRec.gadget_class.border_highlight))((Widget) bg);

}


/************************************************************************
 *
 *  Destroy
 *	Clean up allocated resources when the widget is destroyed.
 *
 ************************************************************************/
#if 0
static void 
Destroy(
        Widget w )
{
DtButtonGadget bg = (DtButtonGadget) w ;

if (bg->button.timer)
    XtRemoveTimeOut (bg->button.timer);

XtReleaseGC (w, bg -> button.gc_normal);
XtReleaseGC (w, bg -> button.gc_background);
XtReleaseGC (w, bg -> button.gc_armed_bg);
if (bg -> button.gc_clip != NULL) 
    XtReleaseGC (w, bg -> button.gc_clip);

XtRemoveAllCallbacks (w, "callback");
}
#endif /* 0 */



/************************************************************************
 *
 *  SetValues
 *	Note:  The only implementation within this function is to
 *	support the resetting of the image, cursor, and functions
 *	as needed by the front panel.
 *
 ************************************************************************/

/* ARGSUSED */
static Boolean 
SetValues(
        Widget current_w,
        Widget request_w,
        Widget new_w )

{
   DtButtonGadget  current = (DtButtonGadget) current_w;
   DtButtonGadget  request = (DtButtonGadget) request_w;
   DtButtonGadget  new_g = (DtButtonGadget) new_w;
   XmManagerWidget mw = (XmManagerWidget) XtParent(new_w);
   Boolean          returnFlag = FALSE;
   Cursor           cursor;

   G_EventMask (new_g) = (XmARM_EVENT | XmACTIVATE_EVENT |
                        XmMULTI_ARM_EVENT | XmMULTI_ACTIVATE_EVENT |
                        XmHELP_EVENT | XmFOCUS_IN_EVENT |
                        XmFOCUS_OUT_EVENT | XmENTER_EVENT | XmLEAVE_EVENT);

   /*  Change the image  */

   if (strcmp (B_ImageName (current), B_ImageName (new_g)) != 0)
   {
      String str = B_ImageName (new_g);

      XtFree (B_ImageName (current));
      B_ImageName (new_g) = XtMalloc (strlen (str) + 1);
      strcpy (B_ImageName (new_g), str);

      B_Pixmap (new_g) =
         XmGetPixmap (XtScreen (new_g), B_ImageName (new_g),
                      M_BottomShadowColor (mw),
                      M_TopShadowColor (mw));
      B_Mask (new_g) =
         _DtGetMask (XtScreen (new_g), B_ImageName (new_g));

      if (B_Pixmap (new_g) != XmUNSPECIFIED_PIXMAP)
      {
         Window	      root;
         int          int_x = 0, int_y = 0;
         unsigned int int_w = 0, int_h = 0,
                      int_bw, depth;

         XGetGeometry (XtDisplay (new_g), B_Pixmap (new_g),
                       &root, &int_x, &int_y, &int_w, &int_h, &int_bw, &depth);

         B_PixmapWidth (new_g) = (Dimension) int_w;
         B_PixmapHeight (new_g) = (Dimension) int_h;
      }
      else
      {
         B_PixmapWidth (new_g) = 0;
         B_PixmapHeight (new_g) = 0;
      }

      UpdateGCs(new_w);

      returnFlag = TRUE;
   }
   else
   {
      if (B_ImageName (new_g) != B_ImageName (current))
      {
         String str = B_ImageName (new_g);

         XtFree (B_ImageName (current));
         B_ImageName (new_g) = XtMalloc (strlen (str) + 1);
         strcpy (B_ImageName (new_g), str);
      }
   }

   return (returnFlag);
}




/************************************************************************
 *
 *  arm
 *     This function processes button 1 down occuring on the button.
 *
 ************************************************************************/

static void 
Arm(
        Widget w,
        XEvent *event,
        String *params,
        Cardinal *num_params )
{
   DtButtonGadget bg = (DtButtonGadget) w ;

   (void) XmProcessTraversal((Widget) bg, XmTRAVERSE_CURRENT);

   B_Armed (bg) = True;

   if ((event->xbutton.time - bg->button.armTimeStamp) >
       XtGetMultiClickTime(XtDisplay(bg)))
   {
      bg -> button.armTimeStamp = event->xbutton.time;
      B_Expose ( w, event, NULL);
   }
}


/************************************************************************
 *
 *  disarm
 *     This function processes button 1 up occuring on the button.
 *
 ************************************************************************/

static void 
Disarm(
        Widget w,
        XEvent *event,
	String *params,
        Cardinal *num_params )
{
   DtButtonGadget bg = (DtButtonGadget) w ;

   B_Armed (bg) = False;

   B_Expose (w, event, NULL);
}


/************************************************************************
 *
 *  activate
 *     This function processes button 1 up occuring on the button.
 *     If the button 1 up occurred inside the button the activate
 *     callbacks are called.
 *
 ************************************************************************/

static void 
Activate(
        Widget w,
        XEvent *event,
	String *params,
        Cardinal *num_params )
{
   DtButtonGadget bg = (DtButtonGadget) w ;
   DtButtonCallbackStruct call_value;
   Dimension bw = G_BorderWidth(bg);

   if (!B_Armed (bg))
      return;

   if ((event->xbutton.time - bg->button.activateTimeStamp) >
       XtGetMultiClickTime(XtDisplay(bg)))
   {
      bg -> button.activateTimeStamp = event->xbutton.time;
      bg->button.click_count = 1;
   }
   else
      bg->button.click_count++;

   B_Armed (bg) = False;

   if (bg->button.callback)
   {
      call_value.reason = XmCR_ACTIVATE;
      call_value.event = event;
      call_value.click_count = bg->button.click_count;

      if ((bg->button.multiClick == XmMULTICLICK_DISCARD) &&
	  (call_value.click_count > 1)) { 
	  return;
      }

      XFlush(XtDisplay(bg));

      CallCallback (bg, B_Callback (bg), XmCR_ACTIVATE, event);
   }
}


/************************************************************************
 *
 *     ArmAndActivate
 *
 ************************************************************************/

static void 
ArmAndActivate(
        Widget w,
        XEvent *event,
	String *params,
        Cardinal *num_params )
{
   DtButtonCallbackStruct call_value;
   DtButtonGadget bg = (DtButtonGadget) w ;

   B_Armed (bg) = True;
   Redisplay ( w, event, FALSE); 

   XFlush (XtDisplay (bg));

   call_value.reason = XmCR_ACTIVATE;
   call_value.event = event;
   call_value.click_count = 1;  /* always 1 in kselect */

   if (bg->button.callback)
   {
      XFlush (XtDisplay (bg));
      CallCallback (bg, B_Callback (bg), XmCR_ACTIVATE, event);
   }
   B_Armed (bg) = False;

   /* If the button is still around, show it released, after a short delay */

   if (bg->object.being_destroyed == False)
   {
      bg->button.timer = XtAppAddTimeOut(
			XtWidgetToApplicationContext((Widget)bg),
                        (unsigned long) DELAY_DEFAULT,
                        (XtTimerCallbackProc)ArmTimeout,
                        (caddr_t)bg);
   }
}

/* ARGSUSED */
static void 
ArmTimeout(
        Widget w,
        XtIntervalId *id )
{
    DtButtonGadget bg = (DtButtonGadget) w ;

    bg -> button.timer = 0;
    if (XtIsRealized ((Widget)bg) && XtIsManaged ((Widget)bg)) {
       Redisplay ( w, NULL, FALSE); 
       XFlush (XtDisplay (bg));
    }
    return;
}


/*-------------------------------------------------------------
**      InputDispatch
**              Process event dispatched from parent or event handler.
*/
static void
InputDispatch(
        Widget w,
        XButtonEvent *event,
        Mask event_mask )
{
        DtButtonGadget bg = (DtButtonGadget) w ;

        if (event_mask & XmARM_EVENT ||
            event_mask & XmMULTI_ARM_EVENT)
        {
           if (event->button == Button1)
             Arm (w, (XEvent*) event, (String *)NULL, (Cardinal)0);
        }
        else if (event_mask & XmACTIVATE_EVENT ||
                 event_mask & XmMULTI_ACTIVATE_EVENT)
        {
                if (event->button == Button1)
                {
                   if (event->x >= G_X (bg) &&
                         event->x <= (Position)(G_X (bg) + G_Width (bg)) &&
                         event->y >= G_Y (bg) &&
                         event->y <= (Position)(G_Y (bg) + G_Height (bg)))
                   {
                        Activate (w, (XEvent*) event,
                                  (String *)NULL, (Cardinal)0);
                        Disarm (w, (XEvent*) event,
				  (String *)NULL, (Cardinal)0);
                   }
                   else
                        Disarm (w, (XEvent*) event,
				  (String *)NULL, (Cardinal)0);
                }
        }
        else if (event_mask & XmHELP_EVENT)
                _XmSocorro (w, (XEvent *)event,
                                     (String *)NULL,(Cardinal)0);
        else if (event_mask & XmENTER_EVENT)
                Enter (w, (XEvent *)event, (String *)NULL,(Cardinal)0);
        else if (event_mask & XmLEAVE_EVENT)
                Leave (w, (XEvent *)event, (String *)NULL,(Cardinal)0);
        else if (event_mask & XmFOCUS_IN_EVENT)
                _XmFocusInGadget (w, (XEvent *)event,
                                    (String *)NULL,(Cardinal)0);
        else if (event_mask & XmFOCUS_OUT_EVENT)
                _XmFocusOutGadget (w, (XEvent *)event,
                                    (String *)NULL,(Cardinal)0);
}


/*-------------------------------------------------------------
**      VisualChange
**              Update GCs when parent visuals change.
*/
static Boolean
VisualChange(
        Widget w,
        Widget current_w,
        Widget new_w )
{
    XmManagerWidget  current = (XmManagerWidget) current_w;
    XmManagerWidget  new_m = (XmManagerWidget) new_w;
    DtButtonGadget   bg = (DtButtonGadget) w ;
    Boolean          update = False;

    /* If the parent foreground or background has changed,
     * then update gcs and pixmap.
     */
    if (M_Foreground (current) != M_Foreground (new_m) ||
        M_Background (current) != M_Background (new_m))
    {
       UpdateGCs(w);
       update = True;
    }

    if (update)
    {
       if (B_ImageName (bg) != NULL)
       {
	     if (B_Mask(bg) != XmUNSPECIFIED_PIXMAP)
		XmDestroyPixmap (XtScreen(bg), B_Mask(bg));

	     if (B_Pixmap(bg) != XmUNSPECIFIED_PIXMAP)
		XmDestroyPixmap (XtScreen(w), B_Pixmap (bg));

	     B_Pixmap (bg) = XmGetPixmap (XtScreen (bg), B_ImageName (bg),
					 M_TopShadowColor (new_m),
					 M_BottomShadowColor (new_m));

	     if (B_Pixmap (bg) != XmUNSPECIFIED_PIXMAP)
	       B_Mask(bg) = (Pixmap)_DtGetMask(XtScreen(bg), B_ImageName(bg));

	     return (True);
       }
       else
	 return (False);
    }
}

/************************************************************************
 *
 *  Enter
 *
 ************************************************************************/

static void 
Enter(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params )
{
   DtButtonGadget bg = (DtButtonGadget) wid ;

   _XmEnterGadget (wid, (XEvent *)event, (String *)NULL,(Cardinal *)0);

   if (B_Armed (bg))
      B_Expose (wid, event, NULL);
}




/************************************************************************
 *
 *  Leave
 *
 ************************************************************************/

static void 
Leave(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params )
{
   DtButtonGadget bg = (DtButtonGadget) wid ;

   _XmLeaveGadget (wid, (XEvent *)event, (String *)NULL,  (Cardinal *)0);

   if (B_Armed (bg))
   {
      B_Armed (bg) = False;
      B_Expose (wid, event, NULL);
      B_Armed (bg) = True;
   }
}


/*-------------------------------------------------------------
**	CallCallback
**		Call callback, if any, with reason and event.
*/
static void 
CallCallback(
        DtButtonGadget w,
        XtCallbackList cb,
        int reason,
        XEvent *event )
{
    DtControlCallbackStruct	cb_data;

    if (cb != NULL)
    {
       cb_data.reason = reason;
       cb_data.event = event;
       cb_data.control_type = XmCONTROL_BUTTON;
       cb_data.set = False;
       cb_data.subpanel = B_Subpanel (w);
       cb_data.push_function = B_PushFunction (w);
       cb_data.push_argument = B_PushArgument (w);
       cb_data.file_size = 0;
       XtCallCallbackList ((Widget) w, cb, &cb_data);
    }
}


/*-------------------------------------------------------------
**	DefaultSelectColor
**		Resource proc. to determine the default select color.
**		(Formerly _XmSelectColorDefault).
*/
static void 
DefaultSelectColor(
        Widget widget,
        int offset,
        XrmValue *value )
{
   XmeGetDefaultPixel (widget, XmSELECT, offset, value);
}


/************************************************************************
 *
 *  DtCreateButtonGadget
 *	Create an instance of an button and return the widget id.
 *
 ************************************************************************/

Widget 
DtCreateButtonGadget(
        Widget parent,
        char *name,
        ArgList arglist,
        Cardinal argcount )
{
   return (XtCreateWidget (name, dtButtonGadgetClass, 
                           parent, arglist, argcount));
}

