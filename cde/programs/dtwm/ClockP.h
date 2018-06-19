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
/**---------------------------------------------------------------------
***	
***	file:		ClockP.h
***
***	project:	MotifPlus Widgets
***
***	description:	Private include file for DtClock class.
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


#ifndef _DtClockP_h
#define _DtClockP_h

#include <Xm/XmP.h>
#include <Dt/ControlP.h>
#include "Clock.h"

#define SEG_BUFF_SIZE		128
#define ASCII_TIME_BUFLEN	32	/* big enough for 26 plus slop */



/*-------------------------------------------------------------
**	Class Structure
*/

/*	Class Part
*/
typedef struct _DtClockClassPart
{
	XmCacheClassPartPtr	cache_part;
	caddr_t			extension;
} DtClockClassPart;

/*	Full Class Record
*/
typedef struct _DtClockClassRec
{
	RectObjClassPart	rect_class;
	XmGadgetClassPart	gadget_class;
	DtIconClassPart	icon_class;
	DtControlClassPart	control_class;
	DtClockClassPart	clock_class;
} DtClockClassRec;

/*	Actual Class
*/
extern DtClockClassRec dtClockClassRec;



/*-------------------------------------------------------------
**	Instance Structure
*/

/*	Instance Part
*/
typedef struct _DtClockPart
{
	int		update;		/* update frequence */
	Dimension 	radius;		/* radius factor */
	int		backing_store;	/* backing store type */
	Boolean 	reverse_video;
	Boolean 	chime;
	Boolean 	beeped;
	Dimension 	tick_spacing;
	Dimension 	minute_hand_length;
	Dimension 	hour_hand_length;
	Dimension 	hand_width;
	Position 	centerX;
	Position 	centerY;
	int		numseg;
	XPoint		segbuff[SEG_BUFF_SIZE];
	XPoint		*segbuffptr;
	XPoint		*hour, *sec;
	struct 		tm  otm ;

	XtIntervalId 	interval_id;
	char 		prev_time_string[ASCII_TIME_BUFLEN];
	int		clock_interval;

	GC		clock_hand_gc;
	GC		clock_background_gc;

	Dimension	left_inset;
	Dimension	right_inset;
	Dimension	top_inset;
	Dimension	bottom_inset;
	Dimension	clock_width;
	Dimension	clock_height;
} DtClockPart;

/*	Full Instance Record
*/
typedef struct _DtClockRec
{
	ObjectPart	object;
	RectObjPart	rectangle;
	XmGadgetPart	gadget;
	DtIconPart	icon;
	DtControlPart	control;
	DtClockPart	clock;
} DtClockRec;



/*-------------------------------------------------------------
**	Class and Instance Macros
*/

/*	DtClock Class Macros
*/

/*	DtClock Instance Macros
*/
#define G_ClockInterval(g)	(g -> clock.clock_interval)
#define G_ClockHandGC(g)	(g -> clock.clock_hand_gc)
#define G_ClockBackgroundGC(g)	(g -> clock.clock_background_gc)
#define G_ClockHandPixel(g)	(g -> icon.pixmap_foreground)
#define G_ClockBackground(g)	(g -> icon.pixmap_background)
#define G_ClockWidth(g)		(g -> clock.clock_width)
#define G_ClockHeight(g)	(g -> clock.clock_height)
#define G_LeftInset(g)		(g -> clock.left_inset)
#define G_RightInset(g)		(g -> clock.right_inset)
#define G_TopInset(g)		(g -> clock.top_inset)
#define G_BottomInset(g)	(g -> clock.bottom_inset)


#endif /* _DtClockP_h */


/* DON'T ADD ANYTHING AFTER THIS #endif */
