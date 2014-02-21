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
/* static char rcsid[] = 
	"$XConsortium: Clock.c /main/8 1996/10/14 10:58:15 pascale $";
*/
/**---------------------------------------------------------------------
***	
***	file:		Clock.c
***
***	project:	MotifPlus Widgets
***
***	description:	Source code for DtClock class.
***			Portions adapted from the Xaw Clock widget.
***	
***
***			(c) Copyright 1990 by Hewlett-Packard Company.
***
***
Copyright 1987, 1988 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.
***
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
#include <time.h>
#include <Xm/GadgetP.h>
#include <Xm/ManagerP.h>
#include "ClockP.h"
#include <Dt/Control.h>


/*-------------------------------------------------------------
**	Public Interface
**-------------------------------------------------------------
*/

WidgetClass	dtClockGadgetClass;


/*-------------------------------------------------------------
*/

#define SIZE_DEFAULT		32
#define	DELAY_DEFAULT		250
#define VERTICES_IN_HANDS	6	/* to draw triangle */
#define PI			3.14159265358979
#define TWOPI			(2. * PI)

#define SMALL_TICK_FRACT	100
#define SMALL_MINUTE_HAND_FRACT	95
#define SMALL_HOUR_HAND_FRACT	68
#define SMALL_HAND_WIDTH_FRACT	12

#define TICK_FRACT		90
#define MINUTE_HAND_FRACT	70
#define HOUR_HAND_FRACT		40
#define HAND_WIDTH_FRACT	7

#define max(a,b)	((a) > (b) ? (a) : (b))
#define min(a,b)	((a) < (b) ? (a) : (b))
#define abs(a)		((a) < 0 ? - (a) : (a))

#define G_Width(r)              (r -> rectangle.width)
#define G_Height(r)             (r -> rectangle.height)
#define M_BackgroundGC(m)       (m -> manager.background_GC)
#define G_X(r)                  (r -> rectangle.x)
#define G_Y(r)                  (r -> rectangle.y)


/* Is this still needed, or does <time.h> always declare time()? */
extern time_t		time ( time_t * );


/********    Public Function Declarations    ********/

extern Widget DtCreateClock( 
                        Widget parent,
                        String name,
                        ArgList arglist,
                        Cardinal argcount) ;

/********    End Public Function Declarations    ********/

/********    Static Function Declarations    ********/

static void ClockTick( 
                        XtPointer client_data,
                        XtIntervalId *id) ;
static void DrawLine( 
                        DtClockGadget w,
                        Dimension blank_length,
                        Dimension length,
                        double fraction_of_a_circle) ;
static void DrawHand( 
                        DtClockGadget w,
                        Dimension length,
                        Dimension width,
                        double fraction_of_a_circle) ;
static void SetSegment( 
                        DtClockGadget w,
                        int x1,
                        int y1,
                        int x2,
                        int y2) ;
static int round( 
                        double x) ;
static void DrawClockFace( 
                        DtClockGadget g) ;
static void Initialize( 
                        Widget request_w,
                        Widget new_w) ;
static void Destroy( 
                        Widget w) ;
static void Resize( 
                        Widget widget) ;
static void Draw( 
                        DtClockGadget g,
                        Drawable drawable,
                        Position x,
                        Position y,
                        Dimension w,
                        Dimension h,
                        Dimension h_t,
                        Dimension s_t,
                        unsigned char s_type,
                        unsigned char fill_mode) ;
static void UpdateGCs( 
                        DtClockGadget g) ;
static void EraseHands ( DtClockGadget	w, struct tm *	tm );

/********    End Static Function Declarations    ********/


/*-------------------------------------------------------------
**	Resource List
*/
#define R_Offset(field) \
	XtOffset (DtClockGadget, clock.field)

static XtResource resources[] = 
{
	{
		XmNclockInterval,
		XmCInterval, XmRInt, sizeof (int),
		R_Offset (clock_interval), XmRImmediate, (XtPointer) 60
	},
	{
		XmNchime,
		XmCBoolean, XmRBoolean, sizeof (Boolean),
		R_Offset (chime), XmRImmediate, (XtPointer) FALSE
	},
	{
		XmNleftInset,
		XmCSpacing, XmRHorizontalDimension, sizeof (Dimension),
		R_Offset (left_inset), XmRImmediate, (caddr_t) 2
	},
	{
		XmNrightInset,
		XmCSpacing, XmRHorizontalDimension, sizeof (Dimension),
		R_Offset (right_inset), XmRImmediate, (caddr_t) 2
	},
	{
		XmNtopInset,
		XmCSpacing, XmRVerticalDimension, sizeof (Dimension),
		R_Offset (top_inset), XmRImmediate, (caddr_t) 2
	},
	{
		XmNbottomInset,
		XmCSpacing, XmRVerticalDimension, sizeof (Dimension),
		R_Offset (bottom_inset), XmRImmediate, (caddr_t) 2
	}
};
#undef	R_Offset



/*-------------------------------------------------------------
**	Class Record
*/
DtClockClassRec dtClockClassRec =
{
	/*	Core Part
	*/
	{	
		(WidgetClass) &dtControlClassRec, /* superclass	*/
		"Clock",			/* class_name		*/
		sizeof (DtClockRec),		/* widget_size		*/
		NULL,				/* class_initialize	*/
		NULL,				/* class_part_initialize*/
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
		Destroy,			/* destroy		*/	
		Resize,				/* resize		*/
		XtInheritExpose,		/* expose		*/	
		NULL,				/* set_values		*/	
		NULL,				/* set_values_hook	*/
		XtInheritSetValuesAlmost,	/* set_values_almost	*/
		NULL,				/* get_values_hook	*/
		NULL,				/* accept_focus		*/	
		XtVersion,			/* version		*/
		NULL,				/* callback private	*/
		NULL,				/* tm_table		*/
		NULL,				/* query_geometry	*/
		NULL,				/* display_accelerator	*/
		NULL,				/* extension		*/
	},

	/*	XmGadget Part
	*/
	{
		XmInheritBorderHighlight,	/* border_highlight	*/
		XmInheritBorderUnhighlight,	/* border_unhighlight	*/
(XtActionProc)	XmInheritArmAndActivate,	/* arm_and_activate	*/
(XmWidgetDispatchProc)	XmInheritInputDispatch,	/* input_dispatch	*/
		XmInheritVisualChange,		/* visual_change	*/
		NULL,				/* get_resources	*/
		0,				/* num_get_resources	*/
		XmInheritCachePart,		/* class_cache_part	*/
		NULL,				/* extension		*/
	},

	/*	DtIcon Part
	*/
	{
		DtInheritGetSize,		/* get_size		*/
		DtInheritGetPositions,		/* get_positions	*/
		(DrawProc) Draw,		/* draw			*/
		DtInheritCallCallback,		/* call_callback	*/
(UpdateGCsProc)	UpdateGCs,			/* update_gcs		*/
		False,				/* optimize_redraw	*/
		NULL,				/* class_cache_part	*/
		NULL,				/* extension		*/
	},

	/*	DtClock Part
	*/
	{
		NULL,				/* class_cache_part	*/
		NULL,				/* extension		*/
	},

	/*	DtControl Part
	*/
	{
		NULL,				/* class_cache_part	*/
		NULL,				/* extension		*/
	}
};


WidgetClass dtClockGadgetClass = (WidgetClass) &dtClockClassRec;



/*-------------------------------------------------------------
**	Private Functions
**-------------------------------------------------------------
*/

/*-------------------------------------------------------------
**	ClockTick
**		Clock timeout.
*/
static void 
ClockTick(
        XtPointer client_data,
        XtIntervalId *id )
{
	DtClockGadget		w = 	(DtClockGadget)client_data;	
	struct tm *	localtime ();
	struct tm 	tm; 
	time_t		time_value;
	char *		time_ptr;
	register Display *	dpy = XtDisplay (w);
	register Window		win = XtWindow (w);

	if (id || !w->clock.interval_id)
		w->clock.interval_id =
			XtAppAddTimeOut (
				XtWidgetToApplicationContext ((Widget) w),
				G_ClockInterval (w)*1000, ClockTick,
				(XtPointer)w );
	(void) time (&time_value);
	tm = *localtime (&time_value);

	if (! XtIsManaged ((Widget)w))
	{
		w->clock.otm = tm;
		return;
	}

/*	Beep on the half hour; double-beep on the hour.
*/
	if (w->clock.chime == TRUE)
	{
		if (w->clock.beeped && (tm.tm_min != 30) &&
		    (tm.tm_min != 0))
			w->clock.beeped = FALSE;
		if (((tm.tm_min == 30) || (tm.tm_min == 0)) 
		    && (!w->clock.beeped))
		{
			w->clock.beeped = TRUE;
			XBell (dpy, 50);	
			if (tm.tm_min == 0)
				XBell (dpy, 50);
		}
	}

/* remove this when clipping added to gcs */
	if ((G_Width (w) <
                   (Dimension)((G_ClockWidth (w) / 2) + w->clock.centerX)) ||
	    (G_Height (w) <
                   (Dimension)((G_ClockHeight (w) / 2) + w->clock.centerY)))
		return;

/*	The second (or minute) hand is sec (or min) sixtieths around the
*	clock face. The hour hand is (hour + min/60) twelfths of the way
*	around the clock-face.
*/
	if (tm.tm_hour > 12)
		tm.tm_hour -= 12;

	EraseHands (w, &tm);

	w->clock.segbuffptr = w->clock.segbuff;
	w->clock.numseg = 0;

/*	Calculate the hour hand, fill it in with its color.
*/
	DrawHand (w, w->clock.minute_hand_length, w->clock.hand_width,
				((double) tm.tm_min)/60.0);
	XDrawLines (dpy, win, G_ClockHandGC (w), w->clock.segbuff,
			VERTICES_IN_HANDS, CoordModeOrigin);
	XFillPolygon (dpy, win, G_ClockHandGC (w), w->clock.segbuff,
			VERTICES_IN_HANDS, Convex, CoordModeOrigin);

	w->clock.hour = w->clock.segbuffptr;

	DrawHand (w, w->clock.hour_hand_length, w->clock.hand_width,
			 ((((double)tm.tm_hour) + 
				(((double)tm.tm_min)/60.0)) / 12.0));
	XFillPolygon (dpy, win, G_ClockHandGC (w), w->clock.hour,
			VERTICES_IN_HANDS, Convex, CoordModeOrigin);
	XDrawLines (dpy, win, G_ClockHandGC (w), w->clock.hour,
			VERTICES_IN_HANDS, CoordModeOrigin);

	w->clock.sec = w->clock.segbuffptr;
	w->clock.otm = tm;
}
	


/*-------------------------------------------------------------
**	EraseHands
**		Erase clock hands.
*/
static void
EraseHands (
	DtClockGadget	w,
	struct tm *	tm )
{
	XmManagerWidget	mgr =	(XmManagerWidget) XtParent (w);
	Display	*	dpy =		XtDisplay (w);
	Window		win =		XtWindow (w);
	unsigned char	behavior =	G_Behavior (w);
	GC		gc;

	if (! XtIsManaged ((Widget)w) || w->clock.numseg <= 0)
		return;

	gc = G_ClockBackgroundGC (w);

/*	Erase old hands.
*/
	if (!tm || tm->tm_min != w->clock.otm.tm_min ||
		  tm->tm_hour != w->clock.otm.tm_hour)
 	{ 
		XDrawLines (dpy, win, gc, w->clock.segbuff,
				VERTICES_IN_HANDS, CoordModeOrigin);
		XDrawLines (dpy, win, gc, w->clock.hour,
				VERTICES_IN_HANDS, CoordModeOrigin);
		XFillPolygon (dpy, win, gc, w->clock.segbuff,
				VERTICES_IN_HANDS, Convex, CoordModeOrigin);
		XFillPolygon (dpy, win, gc, w->clock.hour,
				VERTICES_IN_HANDS, Convex, CoordModeOrigin);
	}
}



/*-------------------------------------------------------------
**	DrawLine
**		Draw a line.
 * blank_length is the distance from the center which the line begins.
 * length is the maximum length of the hand.
 * Fraction_of_a_circle is a fraction between 0 and 1 (inclusive) indicating
 * how far around the circle (clockwise) from high noon.
 *
 * The blank_length feature is because I wanted to draw tick-marks around the
 * circle (for seconds).  The obvious means of drawing lines from the center
 * to the perimeter, then erasing all but the outside most pixels doesn't
 * work because of round-off error (sigh).
 */
static void
DrawLine (
	DtClockGadget w,
	Dimension blank_length,
	Dimension length,
	double fraction_of_a_circle )
{
	double dblank_length = (double)blank_length, dlength = (double)length;
	double angle, cosangle, sinangle;
	double cos ();
	double sin ();
	int cx = w->clock.centerX, cy = w->clock.centerY, x1, y1, x2, y2;

	/*
	 *  A full circle is 2 PI radians.
	 *  Angles are measured from 12 o'clock, clockwise increasing.
	 *  Since in X, +x is to the right and +y is downward:
	 *
	 *	x = x0 + r * sin (theta)
	 *	y = y0 - r * cos (theta)
	 *
	 */
	angle = TWOPI * fraction_of_a_circle;
	cosangle = cos (angle);
	sinangle = sin (angle);

	/* break this out so that stupid compilers can cope */
	x1 = cx + (int) (dblank_length * sinangle);
	y1 = cy - (int) (dblank_length * cosangle);
	x2 = cx + (int) (dlength * sinangle);
	y2 = cy - (int) (dlength * cosangle);
	SetSegment (w, x1, y1, x2, y2);
}



/*-------------------------------------------------------------
**	DrawHand
**		Draw a hand.
 *
 * length is the maximum length of the hand.
 * width is the half-width of the hand.
 * Fraction_of_a_circle is a fraction between 0 and 1 (inclusive) indicating
 * how far around the circle (clockwise) from high noon.
 *
 */
static void 
DrawHand(
        DtClockGadget w,
        Dimension length,
        Dimension width,
        double fraction_of_a_circle )
{
	register double angle, cosangle, sinangle;
	register double ws, wc;
	Position x, y, x1, y1, x2, y2;
	double cos ();
	double sin ();

	/*  A full circle is 2 PI radians.
	 *  Angles are measured from 12 o'clock, clockwise increasing.
	 *  Since in X, +x is to the right and +y is downward:
	 *
	 *	x = x0 + r * sin (theta)
	 *	y = y0 - r * cos (theta)
	 */
	angle = TWOPI * fraction_of_a_circle;
	cosangle = cos (angle);
	sinangle = sin (angle);

	/* Order of points when drawing the hand.
	 *		    1,4
	 *		    / \
	 *		   /   \
	 *		  /	 \
	 *		2 ------- 3
	 */
	wc = width * cosangle;
	ws = width * sinangle;
	SetSegment (w,
		   x = w->clock.centerX + round (length * sinangle),
		   y = w->clock.centerY - round (length * cosangle),
		   x1 = w->clock.centerX - round (ws + wc), 
		   y1 = w->clock.centerY + round (wc - ws));  /* 1 ---- 2 */
	SetSegment (w, x1, y1, 
		   x2 = w->clock.centerX - round (ws - wc), 
		   y2 = w->clock.centerY + round (wc + ws));  /* 2 ----- 3 */
	SetSegment (w, x2, y2, x, y);	/* 3 ----- 1 (4) */
}



/*-------------------------------------------------------------
**	SetSegment
**		Set segment values.
*/
static void 
SetSegment(
        DtClockGadget w,
        int x1,
        int y1,
        int x2,
        int y2 )
{
	w->clock.segbuffptr->x = x1 + G_X (w);
	w->clock.segbuffptr++->y = y1 + G_Y (w);
	w->clock.segbuffptr->x = x2 + G_X (w);
	w->clock.segbuffptr++->y = y2 + G_Y (w);
	w->clock.numseg += 2;
}


/*-------------------------------------------------------------
**	round
**		round integer.
*/
static int 
round(
        double x )
{
	return (x >= 0.0 ? (int) (x + .5) : (int) (x - .5));
}



/*-------------------------------------------------------------
**	DrawClockFace
 *
 *	Draw the clock face (every fifth tick-mark is longer
 *	than the others).
 */
static void 
DrawClockFace(
        DtClockGadget g )
{
	Boolean	draw_minute_ticks =
			 ((G_ClockWidth (g) > (Dimension) (2 * SIZE_DEFAULT)) &&
			  (G_ClockHeight (g) > (Dimension) (2 * SIZE_DEFAULT)));
	register int i;
	register int delta =
			(int)(g->clock.radius - g->clock.tick_spacing) / 3;

	if (! XtIsManaged ((Widget)g))
		return;

	g->clock.segbuffptr = g->clock.segbuff;
	g->clock.numseg = 0;

/*	Set segments.
*/
	for (i = 0; i < 60; i++)
	{
		if (draw_minute_ticks)
		{
			if ((i % 5) == 0)
				DrawLine (g, g->clock.tick_spacing,
					 g->clock.radius, ((double) i)/60.);
			else
				DrawLine (g, g->clock.radius - delta,
					 g->clock.radius, ((double) i)/60.);
		}
		else
			if ((i % 15) == 0)
				DrawLine (g, g->clock.radius - 1,
					 g->clock.radius, ((double) i)/60.);
	}

/*	Draw clock face.
*/
	XDrawSegments (XtDisplay (g), XtWindow (g), G_ClockHandGC (g),
			(XSegment *) & (g->clock.segbuff[0]),
			g->clock.numseg/2);
	
	g->clock.segbuffptr = g->clock.segbuff;
	g->clock.numseg = 0;
}



/*-------------------------------------------------------------
**	Action Procs
**-------------------------------------------------------------
*/

/*-------------------------------------------------------------
**	Core Procs
**-------------------------------------------------------------
*/


/*-------------------------------------------------------------
**	Initialize
**		Initialize a new gadget instance.
*/
/* ARGSUSED */
static void 
Initialize(
        Widget request_w,
        Widget new_w )
{
	DtClockGadget	request =	 (DtClockGadget) request_w,
			new =		 (DtClockGadget) new_w;
	Dimension	w, h,
			h_t =		G_HighlightThickness (new),
			s_t =		G_ShadowThickness (new);
	XmManagerWidget	mw = (XmManagerWidget) XtParent (new);
	EventMask	mask;
	String		name = NULL;

/*	Set width and height.
*/
	if ((G_Width (request) == 0) ||
	    (G_Height (request) == 0))
	{
		G_ClockWidth (new) = SIZE_DEFAULT;
		G_ClockHeight (new) = SIZE_DEFAULT;
		G_GetSize ((DtIconGadget)new, &w, &h);
		if (G_Width (request) == 0)
			G_Width (new) = w;
	 	if (G_Height (request) == 0)
			G_Height (new) = h;
	}

	Resize ((Widget)new);

	if (G_ClockInterval (new) <= 0)
		G_ClockInterval (new) = 60;	/* make invalid update's use a default */
	new->clock.interval_id = 0;
	new->clock.numseg = 0;
}



/*-------------------------------------------------------------
**	Destroy
**		Release resources allocated for gadget.
*/
static void 
Destroy(
        Widget w )
{
	DtClockGadget	g =	 (DtClockGadget) w;
	XmManagerWidget mw = (XmManagerWidget) XtParent (g);

	if (g->clock.interval_id) XtRemoveTimeOut (g->clock.interval_id);

	XtReleaseGC ((Widget) mw, G_ClockHandGC (g));
}



/*-------------------------------------------------------------
**	Resize
**		Set positions of string and pixmap.
*/
static void 
Resize(
        Widget widget )
{
	DtClockGadget	g =	 (DtClockGadget) widget;
	Position	pix_x, pix_y, str_x, str_y;
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
			spacing = G_Spacing (g),
			w = G_Width (g),
			h = G_Height (g);
	int		radius;

	G_ClockWidth (g) = p_w - G_LeftInset (g) - G_RightInset (g);
	G_ClockHeight (g) = p_h - G_TopInset (g) - G_BottomInset (g);
	
        G_ClockWidth (g) = min (G_ClockWidth (g), G_ClockHeight (g));
        G_ClockHeight (g) = G_ClockWidth (g);

        G_GetPositions ((DtIconGadget)g, w, h, h_t, s_t,
			 &pix_x, &pix_y, &str_x, &str_y);

/*	Compute clock size factors.
*	(need signed radius value since Dimension is unsigned)
*/
	radius = ((int) min (G_ClockWidth (g), G_ClockHeight (g))/2);
	g->clock.radius = (Dimension) max (radius, 1);

	if (G_ClockWidth (g) < (Dimension)(SIZE_DEFAULT / 2))
	{
		g->clock.tick_spacing =
		   ((Dimension)(TICK_FRACT * g->clock.radius)/100);
		g->clock.minute_hand_length =
		   ((Dimension)(MINUTE_HAND_FRACT * g->clock.radius)/100);
		g->clock.hour_hand_length =
		   ((Dimension)(HOUR_HAND_FRACT * g->clock.radius)/100);
		g->clock.hand_width =
		   ((Dimension)(HAND_WIDTH_FRACT * g->clock.radius)/100);
	}
	else
	{
		g->clock.tick_spacing =
		   ((Dimension)(SMALL_TICK_FRACT * g->clock.radius)/100);
		g->clock.minute_hand_length =
		   ((Dimension)(SMALL_MINUTE_HAND_FRACT * g->clock.radius)/100);
		g->clock.hour_hand_length =
		   ((Dimension)(SMALL_HOUR_HAND_FRACT * g->clock.radius)/100);
		g->clock.hand_width =
		   ((Dimension)(SMALL_HAND_WIDTH_FRACT * g->clock.radius)/100);
	}

/*	Compute clock position factors.
*/
        g->clock.centerX = pix_x + (p_w / 2);
        g->clock.centerY = pix_y + (p_h / 2);
}



/*-------------------------------------------------------------
**	SetValues
**		
*/
#if 0
/* ARGSUSED */
static Boolean 
SetValues(
        Widget current_w,
        Widget request_w,
        Widget new_w )
{
	DtClockGadget	request =	 (DtClockGadget) request_w,
			current =	 (DtClockGadget) current_w,
			new =		 (DtClockGadget) new_w;
	Boolean		redraw_flag = False;

	if (G_ClockInterval (new) != current->clock.update)
	{
		if (current->clock.interval_id)
			XtRemoveTimeOut (current->clock.interval_id);
		if (XtIsRealized (new))
			new->clock.interval_id =
				XtAppAddTimeOut (
					XtWidgetToApplicationContext (new_w),
					G_ClockInterval (new)*1000,
					(XtTimerCallbackProc) ClockTick, 
					(XtPointer)new_w);
	}

/*	Update clock face and hand color.
*/
	if ((G_ClockBackground (new) != G_ClockBackground (current)) ||
	    (G_ClockHandPixel (new) != G_ClockHandPixel (current)))
	{
		G_UpdateGCs ((DtIconGadget)new);
		redraw_flag = True;
	}

/*	Update size factors if no resize.
*/
	if ((G_Width (new) == G_Width (current) &&
	     G_Height (new) == G_Height (current)) &&
	    (G_MarginWidth (new) != G_MarginWidth (current) ||
	     G_MarginHeight (new) != G_MarginHeight (current) ||
	     G_Spacing (new) != G_Spacing (current) ||
	     G_ShadowThickness (new) != G_ShadowThickness (current) ||
	     G_HighlightThickness (new) != G_HighlightThickness (current)))
		Resize (new_w);

	return (redraw_flag);
}
#endif /* 0 */


/*-------------------------------------------------------------
**	Gadget Procs
**-------------------------------------------------------------
*/


/*-------------------------------------------------------------
**	Icon Procs
**-------------------------------------------------------------
*/

/*-------------------------------------------------------------
**	Draw
**		Draw gadget to drawable.
*/
static void 
Draw(
        DtClockGadget g,
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
   XmManagerWidget mgr = (XmManagerWidget) XtParent (g);
   Display * d = XtDisplay (g);
   GC gc;
   XRectangle clip;
   Position p_x, p_y, s_x, s_y;
   unsigned char behavior =	G_Behavior (g);
   Dimension m_w = G_MarginWidth (g);
   Dimension m_h = G_MarginHeight (g);
   Dimension h_pad = h_t + s_t + m_w;
   Dimension v_pad = h_t + s_t + m_h;
   Dimension width, height;
   Pixmap pix;
   Pixmap mask;
   Boolean bMono;

   bMono = ( ((G_Foreground (g) == BlackPixelOfScreen (XtScreen (g))) ||
	      (G_Foreground (g) == WhitePixelOfScreen (XtScreen (g)))) &&
	     ((G_Background (g) == BlackPixelOfScreen (XtScreen (g))) ||
	      (G_Background (g) == WhitePixelOfScreen (XtScreen (g)))) );

   if ((G_Armed (g)) && (fill_mode != XmFILL_PARENT))
       gc = G_ArmedBackgroundGC (g);
   else
       gc = M_BackgroundGC (mgr);

   if ((fill_mode != XmFILL_NONE) && (fill_mode != XmFILL_TRANSPARENT))
       XFillRectangle (d, drawable, gc, x + h_t, y + h_t,
				   w - 2 * h_t, h - 2 * h_t);

   G_GetPositions ((DtIconGadget)g, w, h, h_t, s_t, &p_x, &p_y, &s_x, &s_y);

   if (G_Pixmap (g))
       {
       width = ((Dimension)(p_x + s_t + h_t) >= G_Width (g))
			   ? 0 : min (G_PixmapWidth (g),
				   (Dimension)(G_Width (g) - p_x - s_t - h_t));
       height = ((Dimension)(p_y + s_t + h_t) >= G_Height (g))
			   ? 0 : min (G_PixmapHeight (g),
				   (Dimension)(G_Height (g) - p_y - s_t - h_t));
   /*	Use normal image.
    */
       pix = G_Pixmap (g);
       mask = G_Mask (g);

   /*	Update clip gc.
    */
       if (mask != XmUNSPECIFIED_PIXMAP)
	   {
	   gc = G_ClipGC (g);
	   XSetClipMask (XtDisplay(g), gc, mask);
	   XSetClipOrigin (XtDisplay(g), gc, x + p_x, y + p_y);
	   }
       else
	   gc = G_NormalGC (g);
	   
   /*	Paint pixmap.
    */
       if ((gc != NULL) && (pix != XmUNSPECIFIED_PIXMAP))
	   XCopyArea (d, pix, drawable, gc, 0, 0,
			   width, height, x + p_x, y + p_y);
       }

   /*	Fill clock face region and draw clock.
   */
	   if (g->clock.numseg != 0)
		   EraseHands (g, (struct tm *) 0);

	   ClockTick ((XtPointer)g, (XtIntervalId)0);

   /*	Draw shadow.
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

   /*	Draw string.
   */
	   if (G_String (g))
	   {
		   clip.x = x + h_t + s_x;
		   clip.y = y + h_t + s_y;
		   clip.width = G_Width (g) - (2 * (h_t + s_t));
		   clip.height = G_Height (g) - (2 * (h_t + s_t));
		   if (clip.width > 0 && clip.height > 0)
                   {
                      if (bMono)
                      {
			 if ((s_x - 2) >= (Position)(h_t + s_t))
                            XFillRectangle (d, drawable,
					    G_ArmedBackgroundGC (g),
                                	    x + s_x - 2, y + s_y,
                                	    2, G_StringHeight (g));
			 XmStringDrawImage (d, drawable, G_FontList (g),
				            G_String (g), G_NormalGC (g),
					    x + s_x, y + s_y,
				            clip.width, XmALIGNMENT_BEGINNING,
				            XmSTRING_DIRECTION_L_TO_R, &clip);
                         if ((s_x + (Position)(G_StringWidth (g) + 2))
			     <= (Position)(G_Width (g) - h_t - s_t))
                	    XFillRectangle (d, drawable,
					    G_ArmedBackgroundGC (g),
                                            x + s_x + G_StringWidth (g),
					    y + s_y, 2, G_StringHeight (g));
                      }
		      else
                      {
			 if (G_UseEmbossedText (g))
                            XmStringDraw (d, drawable, G_FontList (g),
					  G_String (g), G_BottomShadowGC (g),
					  x + s_x + 1, y + s_y + 1,
                                          clip.width, XmALIGNMENT_BEGINNING,
                                          XmSTRING_DIRECTION_L_TO_R, &clip);
                         XmStringDraw (d, drawable, G_FontList (g),
				       G_String (g), G_NormalGC (g),
				       x + s_x, y + s_y,
                                       clip.width, XmALIGNMENT_BEGINNING,
                                       XmSTRING_DIRECTION_L_TO_R, &clip);

                      }
                   }
	   }

}



/*-------------------------------------------------------------
**	UpdateGCs
**		Get normal and background graphics contexts.
**		Use standard mask to maximize caching opportunities.
*/
static void 
UpdateGCs(
        DtClockGadget g )
{
XGCValues	values;
XtGCMask	value_mask;
XmManagerWidget	mw = (XmManagerWidget) XtParent(g);
XFontStruct *	font;
Boolean		font_rtn;
DtIconGadgetClass
		super = (DtIconGadgetClass) XtSuperclass (g);
static Boolean	first = True;

/*	Let superclass do most of the work.
*/
(* C_UpdateGCs (super)) ((DtIconGadget)g);

if (first)
	first = False;
else if (G_ClockHandGC (g))
	XtReleaseGC ((Widget)mw, G_ClockHandGC (g));

/*	Get clock hand GC.
*/

value_mask = GCForeground | GCFillStyle;

if (XmeRenderTableGetDefaultFont (G_FontList (g), &font)) {
  value_mask |= GCFont;
  values.font = font->fid;
}

if (((G_PixmapForeground (g) == WhitePixelOfScreen (XtScreen (g))) &&
     (G_PixmapBackground (g) == BlackPixelOfScreen (XtScreen (g)))) ||
    ((G_PixmapForeground (g) == BlackPixelOfScreen (XtScreen (g))) &&
     (G_PixmapBackground (g) == WhitePixelOfScreen (XtScreen (g)))))
    values.foreground = G_PixmapBackground (g);
else
    values.foreground = mw->manager.top_shadow_color;
values.fill_style = FillSolid;


G_ClockHandGC (g) = XtGetGC ((Widget) mw, value_mask, &values);

/*	Get clock background GC
*/

value_mask |= GCBackground;

if (((G_PixmapForeground (g) == WhitePixelOfScreen (XtScreen (g))) &&
     (G_PixmapBackground (g) == BlackPixelOfScreen (XtScreen (g)))) ||
    ((G_PixmapForeground (g) == BlackPixelOfScreen (XtScreen (g))) &&
     (G_PixmapBackground (g) == WhitePixelOfScreen (XtScreen (g)))))
    values.foreground = G_PixmapForeground (g);
else
    values.foreground = G_ArmColor (g);
values.background = G_PixmapBackground (g);

G_ClockBackgroundGC (g) = XtGetGC ((Widget) mw, value_mask, &values);
}



/*-------------------------------------------------------------
**	Public Entry Points
**-------------------------------------------------------------
*/

/*-------------------------------------------------------------
**	DtCreateClock
**		Create a new gadget instance.
**-------------------------------------------------------------
*/
Widget 
DtCreateClock(
        Widget parent,
        String name,
        ArgList arglist,
        Cardinal argcount )
{
	return (XtCreateWidget (name, dtClockGadgetClass, 
			parent, arglist, argcount));
}
