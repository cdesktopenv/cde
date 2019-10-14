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
/*******************************************************************************
**
**  select.c
**
**  $TOG: select.c /main/4 1998/08/18 17:32:33 samborn $
**
**  RESTRICTED CONFIDENTIAL INFORMATION:
**
**  The information in this document is subject to special
**  restrictions in a confidential disclosure agreement between
**  HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
**  document outside HP, IBM, Sun, USL, SCO, or Univel without
**  Sun's specific written approval.  This document and all copies
**  and derivative works thereof must be returned or destroyed at
**  Sun's request.
**
**  Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
**
*******************************************************************************/

/*									*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc. 				*
 */

#ifndef lint
static  char sccsid[] = "@(#)select.c 1.24 95/02/21 Copyr 1994 Sun Microsystems, Inc.";
#endif

#include <EUSCompat.h>
#include <stdio.h>
#include <Xm/Xm.h>
#include "util.h"
#include "select.h"
/*
#include "graphics.h"
*/
#include "calendar.h"
#include "timeops.h"
#include "browser.h"
#include "todo.h"
#include "dayglance.h"
#include "weekglance.h"
#include "monthglance.h"
#include "yearglance.h"
#include "datefield.h"
#include "props.h"

static void select_weekhotbox();

/* Highlight thickness  - mghis */
#define	STDTTT		3	/* default */
#define WEEKLTTT	2	/* of hour slots in week glance view */

extern int
selection_active (Selection *s)
{
	return (s->active);
}

extern void
deactivate_selection (Selection *s)
{
	s->active=0;
}

extern void
activate_selection (Selection *s)
{
	s->active=1;
}

extern void
weekchart_deselect(Calendar *c)
{
	int h, i, j, k, index, wi, x, y, chart_x, chart_y;
	Week *w = (Week *)c->view->week_info;
	Selection *sel	= (Selection *) w->current_selection;
	XFontSetExtents fontextents;
	int	char_height;
	Colormap cms;

	CalFontExtents(w->font, &fontextents);
	char_height = fontextents.max_ink_extent.height;

        XtVaGetValues(c->canvas, XmNcolormap, &cms, NULL);

	if (!selection_active (sel)) return;

	j = sel->col;
	k = sel->row;

	/* dissolves box around weekday letter over chart */
	chart_x = w->chart_x + (j * w->chart_day_width);
	chart_y = w->chart_y - char_height - 4;
	gr_dissolve_box(c->xcontext, chart_x, chart_y - 6,
                                w->chart_day_width, char_height + 6, 2);

	x =  (j*w->chart_day_width) +  w->chart_x + 1;
	y =  (k * (w->chart_hour_height + w->add_pixels)) +  w->chart_y + 1;
	h =  (w->chart_hour_height/BOX_SEG);
	wi =  w->chart_day_width-2;

	gr_clear_area(c->xcontext, x, y, w->chart_day_width-1, 
			w->chart_hour_height-1 + w->add_pixels);
	gr_draw_box(c->xcontext, x - 1, y - 1, w->chart_day_width, 
			w->chart_hour_height + w->add_pixels, NULL);
	index = j * (w->segs_in_array / 7) + (BOX_SEG * k);
	for (i = index;  i < (index + BOX_SEG); i++) {
		/* compensate for the added pixel for displaying chart */
		if ((i+1) == (index + BOX_SEG)) h += w->add_pixels;
		if (w->time_array[i] == 1) {
			if ((c->xcontext->screen_depth < 8)  || FAKE_MONOCHROME)
				gr_make_gray(c->xcontext, x, y, wi+1, h, 25);
			else 
				gr_make_grayshade(c->xcontext, x, y, wi+1, h, 
						LIGHTGREY);
		}
		else if (w->time_array[i] == 2) {
			if ((c->xcontext->screen_depth < 8)  || FAKE_MONOCHROME)
				gr_make_gray(c->xcontext, x, y, 
					wi+1, h, 50);
			else
				gr_make_rgbcolor(c->xcontext, cms, x, y, wi+1, h, 
						MIDGREY, MIDGREY, MIDGREY);
		}
		else if (w->time_array[i] >= 3) {
			if ((c->xcontext->screen_depth < 8)  || FAKE_MONOCHROME)
				gr_make_gray(c->xcontext, x, y, wi+1, h, 75);
			else 
				gr_make_grayshade(c->xcontext, x, y, wi+1, h, 
						DIMGREY);
		}
		/* compensate for grid line pixel */ 
		if (i == (index+BOX_SEG-2))
			y += (h-1);
		else
			y += h;
	}
	deactivate_selection (sel);
}

/* selects day in chart */
extern void
weekchart_select(Calendar *c)
{
	int i, j, chart_x, chart_y;
	Week *w = (Week *)c->view->week_info;
	Selection *sel	= (Selection *) w->current_selection;
	Colormap cms;
	Pixel background_pixel;
	XFontSetExtents fontextents;
	int	char_height;

	CalFontExtents(w->font, &fontextents);
	char_height = fontextents.max_ink_extent.height;

        XtVaGetValues(c->canvas, XmNcolormap, &cms, NULL);
	/*
        XtVaGetValues(c->frame, XmNbackground, &background_pixel, NULL);
	*/
	background_pixel = c->xcontext->hilight_color;

	i = sel->col;
	j = sel->row;
	if (j >= 0) {
        	chart_x = w->chart_x + (i * w->chart_day_width);
        	chart_y = w->chart_y - char_height - 4;
		if (c->xcontext->screen_depth < 8) {
			gr_draw_box(c->xcontext, (i * w->chart_day_width) +
			 	w->chart_x + 1, (j * w->chart_hour_height) + 
				(j * w->add_pixels) + w->chart_y + 1, 
				w->chart_day_width - 2, 
				w->chart_hour_height - 2 + w->add_pixels, NULL);
			gr_draw_box(c->xcontext, chart_x, chart_y, 
				w->chart_day_width, char_height + 1, NULL);
		}
		else  { 
			/* Select time slot */
			gr_draw_rgb_box(c->xcontext, (i * w->chart_day_width)
				+ w->chart_x, (j * w->chart_hour_height) 
				+ (j * w->add_pixels) + w->chart_y, 
				w->chart_day_width, w->chart_hour_height
				+ w->add_pixels, WEEKLTTT,  background_pixel,
				cms);

			/* Select weekday letter over chart */
			gr_draw_rgb_box(c->xcontext, chart_x, chart_y - 6,
                                w->chart_day_width, char_height + 6, WEEKLTTT,
				background_pixel, cms);
		}
		sel->nunits = 1;
		activate_selection(sel);
	}
}

/* selects day is main boxes: not chart */
static void
select_weekday(Calendar *c, Boolean select)
{
        int     n, x, y;
        new_XContext*xc     = c->xcontext;
	Week 	*w = (Week *)c->view->week_info;
	Pixel foreground_pixel;


	foreground_pixel = xc->hilight_color;

        /* Draw selection feedback on week view */
        (n = dow(c->view->date)) == 0 ? n = 6 : n--;

        if (n < 5) {
                x = w->x + n * w->day_width + 2;
                y = w->y + w->label_height + 2;
        } else {
                n -= (5 - 3);
                x = w->x + n * w->day_width + 2;
                y = w->y + w->day_height + w->label_height + 2;
        }
 
        if (select) {
                if (c->xcontext->screen_depth < 8) {
                        gr_draw_box(xc, x, y, w->day_width - 2,
                        w->day_height - w->label_height - 2, NULL);
		}
                else {
			Colormap cms;

        		XtVaGetValues(c->canvas, XmNcolormap, &cms, NULL);
                        gr_draw_rgb_box(xc, x - 2, y - 2, w->day_width,
                                        w->day_height - w->label_height, STDTTT,
				 	foreground_pixel, cms);
		}
                weekchart_select(c);
        } else {
                gr_draw_box(xc, x - 2, y - 2, w->day_width,
                       	    w->day_height - w->label_height, NULL);
                gr_dissolve_box(xc, x - 1, y - 1, w->day_width - 2,
				 w->day_height - w->label_height - 2, 2);
                weekchart_deselect(c);
        }
 
}

/*	selection service for all views.  ref is a client_data
	field which is cast depending on the selection unit.  if
	it's a daySelect, ref contains the number of weeks in the
	month.  if it's a monthSelect, ref contains a point to x,y
	coordinates.  ref is NULL on a weekSelect.		*/

extern void
calendar_select (Calendar *c, Selection_unit unit, caddr_t ref)
{
	int i, j;
	int xpos, boxw, boxh, margin, topoff, date;
	Selection *sel;
	new_XContext *xc;
	Colormap cms;
	Pixel foreground_pixel;
	int dayname_height;

	date	= c->view->date;
	sel	= (Selection *) c->view->current_selection;
	boxw	= c->view->boxw;
	boxh	= c->view->boxh;
	margin	= c->view->outside_margin;
	topoff	= c->view->topoffset;
	xc	= c->xcontext;

        XtVaGetValues(c->canvas, XmNcolormap, &cms, NULL);
	/*
        XtVaGetValues(c->canvas, XmNforeground, &foreground_pixel, NULL);
	*/
	foreground_pixel = xc->hilight_color;
	switch (unit) {
		/* day selection on month glance */
		case daySelect:
			dayname_height = ((Month *) c->view->month_info)->dayname_height;
			i=dow(date);
			j=wom(date);
			if (j >= 0) {
				sel->row=j-1;
				sel->col=i;
				sel->nunits=1;
				if (c->xcontext->screen_depth < 8) 
					gr_draw_box(xc, (i*boxw)+margin+
					1, (sel->row*boxh)+topoff+
					dayname_height+1, boxw-2, boxh-2, NULL);
				else {
					gr_draw_rgb_box(xc, (i*boxw)
						+margin, 
						(sel->row*boxh)+topoff+
                                        	dayname_height, boxw, 
						boxh, STDTTT,
						foreground_pixel, cms);
				}
				activate_selection (sel);

			}
			break;
		/* week selection on month glance view */
		case weekSelect:
#ifdef NEVER
			i=0; j=7;
			sel->row=(int)ref;
			sel->col=i;
			sel->nunits=j-i;
			while (i < j) {
				if (c->xcontext->screen_depth < 8) 
					gr_draw_box(xc, i*boxw+margin+1,
					(int)ref*boxh+margin+topoff+1, 
					boxw-2, boxh-2, NULL);
				else {
					gr_draw_rgb_box(xc, i*boxw+margin+2,
					(int)ref*boxh+margin+topoff+2, 
					boxw-4, boxh-4, STDTTT,
					foreground_pixel, cms);
					gr_draw_rgb_box(xc, i*boxw+margin+1,
					(int)ref*boxh+margin+topoff+1, 
					boxw-2, boxh-2, 1,
					foreground_pixel, cms);
				}
				i++;
			}
			activate_selection (sel);
#endif
			break;
		/* month selection on year glance view */
		case monthSelect:
			if (ref != NULL) {
				i=((pr_pos *)ref)->x;
				j=((pr_pos *)ref)->y;
			}
			else {
				i = sel->col;
				j = sel->row;
			}
			if (c->xcontext->screen_depth < 8) 
				gr_draw_box(xc, i*boxw+margin+10,
					j*boxh+topoff-4, boxw-4, boxh-4, NULL);
			else {
				if (i == 0)
                                       xpos = margin-5;
                                else if (i == 1)
                                        xpos = i*(boxw-18);
                                else
                                        xpos = i*(boxw-24);
/*
                                gr_draw_rgb_box(xc, xpos,
                                        j*boxh+topoff+10, boxw-2, boxh-2, 1,
                                        foreground_pixel, cms);
*/
                                gr_draw_rgb_box(xc, xpos,
                                        j*boxh+topoff+10, boxw-56, boxh+2, STDTTT,
                                        foreground_pixel, cms);
			}
			sel->row=j;
			sel->col=i;
			sel->nunits=1;
			activate_selection (sel);
			break;
		/* hour box selection day glance view */
		case hourSelect:
			if (ref != NULL)
				j=((pr_pos *)ref)->y;
			else
				j = sel->row;
			if (c->xcontext->screen_depth < 8) 
				gr_draw_box(xc, MOBOX_AREA_WIDTH+5,
					j*boxh+topoff+1, c->view->boxw-4,
					c->view->boxh-2, NULL);
			else {
				gr_draw_rgb_box(xc, MOBOX_AREA_WIDTH+3,
					j*boxh+topoff, 
					c->view->boxw - 2,
					c->view->boxh,
					STDTTT,
					foreground_pixel, cms);
			}
			sel->row=j;
			sel->col=MOBOX_AREA_WIDTH+4;
			sel->nunits=1;
			activate_selection(sel);
			break;
		/* day selection on week glance */
		case weekdaySelect:
			select_weekday(c, True);
			activate_selection (sel);
			break;
		/* hotbox selection on week glance */
		case weekhotboxSelect:
			select_weekhotbox(c);
			activate_selection (sel);
			break;
		default:
			break;
	}
/*
	common_update_lists(c);
*/
}
	
extern void
calendar_deselect (Calendar *c)
{
	int 		i, j, k, xpos;
	int		x, y;
	int 		boxh = c->view->boxh;
	int 		boxw = c->view->boxw;
	int 		margin = c->view->outside_margin;
	int 		topoff = c->view->topoffset;
	Selection 	*s = (Selection *) c->view->current_selection;
	int 		nunits = s->nunits;
	new_XContext 	*xc = c->xcontext;
	int 		dayname_height;
        int		nop;
	Cal_Font 	*pf = c->fonts->boldfont;
	Props 		*p = (Props*)c->properties;
	DisplayType	disp_t = get_int_prop(p, CP_DEFAULTDISP);

	if (selection_active(s)) {
		j=s->col;
		k=s->row;
		switch(c->view->glance) {
		case weekGlance:
			select_weekday(c, False);
			deactivate_selection (s);
			break;
		case dayGlance: {
        		int	hrbox_margin;
			int	num_hrs = get_int_prop(p, CP_DAYEND) -
					  get_int_prop(p, CP_DAYBEGIN) + 1;

			for (i=0; i < nunits; i++) {
				gr_dissolve_box(xc, MOBOX_AREA_WIDTH+3,
					 k*boxh+topoff + 1, boxw-3, boxh-2, 3);
				x = MOBOX_AREA_WIDTH + 2;
				y = k * boxh + topoff;

				gr_draw_line(xc, x, y, x + boxw, y,
						gr_solid, NULL);
				y += boxh;
				gr_draw_line(xc, x, y, x + boxw, y, 
						gr_solid, NULL);
			}

        		if (disp_t == HOUR12)
                		CalTextExtents(pf, "12pm", 4, &nop, &nop, 
							   &hrbox_margin, &nop);
        		else
                		CalTextExtents(pf, "24 ", 3, &nop, &nop, 
							   &hrbox_margin, &nop);

			/* draw vertical line */
        		gr_draw_line(xc, MOBOX_AREA_WIDTH+2+hrbox_margin,
			c->view->topoffset, MOBOX_AREA_WIDTH+2+hrbox_margin,
			c->view->topoffset + num_hrs * c->view->boxh, 
			gr_solid, NULL);
			break;
		}
		case monthGlance:
			dayname_height = 
				((Month *) c->view->month_info)->dayname_height;

			for (i=0; i<nunits; i++) {
				gr_dissolve_box(xc,
					(j * boxw) + margin + 1, 
					(k * boxh) + topoff + dayname_height + 1,
                                        boxw - 2, boxh - 2, 2);
				gr_draw_box(xc,
					(j * boxw) + margin, 
					(k * boxh) + topoff + dayname_height,
                                        boxw, boxh, NULL);
				j++;
			}
			break;
		case yearGlance:
			for (i=0; i<nunits; i++) {
                                if (j == 0)
                                       xpos = margin-5;
                                else if (j == 1)
                                        xpos = j*(boxw-18);
                                else
                                        xpos = j*(boxw-24);

/*
				gr_dissolve_box(xc, (j*boxw)+margin+10,
					k*boxh+topoff-5,
					boxw-2, boxh-2, 1);
*/
				gr_dissolve_box(xc, xpos, 
                                   k*boxh+topoff+10, boxw-56, boxh+2, 1);

/*
				gr_dissolve_box(xc, (j*boxw)+margin+11,
					k*boxh+topoff-4,
					boxw-2, boxh-2, 1);
*/
				j++;
			}
			break;
		}
		deactivate_selection (s);
	}
}
extern void
monthbox_deselect(Calendar *c)
{
	char buf[3];
	Day *day_info = (Day *)c->view->day_info;
	int x, y;

	if (day_info->day_selected == -1)
		return;

	x = day_info->day_selected_x + 2;
	y = day_info->day_selected_y + 2;

	if (c->xcontext->screen_depth < 8)
		gr_make_gray(c->xcontext, 
			x,
			y,
			day_info->col_w+1,
			day_info->row_h-1, 25);
	else
		gr_make_grayshade(c->xcontext, 
			x,
			y,
			day_info->col_w+1,
			day_info->row_h-1, LIGHTGREY);
	buf [0] = '\0';
	sprintf(buf, "%d", day_info->day_selected);
	gr_text(c->xcontext, day_info->day_selected_x2+2, 
			day_info->day_selected_y2, 
			c->fonts->viewfont, buf, NULL);
}
extern void
monthbox_select(Calendar *c)
{
	char buf[3];
	Day *day_info = (Day *)c->view->day_info;
	int x, y;

	x = day_info->day_selected_x + 2;
	y = day_info->day_selected_y + 2;

	gr_clear_box(c->xcontext, 
			x,
			y,
			day_info->col_w,
			day_info->row_h-3);
	gr_draw_box(c->xcontext,
			x,
			y,
			day_info->col_w,
			day_info->row_h-3, NULL);
	buf [0] = '\0';
	sprintf(buf, "%d", day_info->day_selected);
	gr_text(c->xcontext, day_info->day_selected_x2+2, 
			day_info->day_selected_y2, 
			c->fonts->viewfont, buf, NULL);
}

extern void
browser_deselect (Calendar *c, Browser *b)
{
	int h, i, j, k, index, w, x, y;
	Selection *sel	= (Selection *) b->current_selection;
	Colormap cms;

        XtVaGetValues(c->canvas, XmNcolormap, &cms, NULL);
	if (!selection_active (sel))
		return;
	j = sel->col;
	k = sel->row;
	x =  (j*b->boxw) +  b->chart_x + 1;
	y =  (k*b->boxh) + b->chart_y + 1;
	h =  (b->boxh/BOX_SEG);
	w =  b->boxw-2;
	gr_clear_area(b->xcontext, x, y, b->boxw-1, b->boxh-1);
	index = j * (b->segs_in_array / 7) + (BOX_SEG * k);
	for (i = index;  i < (index + BOX_SEG); i++) {
		if (b->multi_array[i] == 1) {
			if ((c->xcontext->screen_depth < 8)  || FAKE_MONOCHROME)
				gr_make_gray(b->xcontext, x, y, w+1, 
							h, 25);
			else 
				gr_make_grayshade(b->xcontext, x, y, 
					w+1, h, LIGHTGREY);
		}
		else if (b->multi_array[i] == 2) {
			if ((c->xcontext->screen_depth < 8)  || FAKE_MONOCHROME)
				gr_make_gray(b->xcontext, x, y, 
					w+1, h, 50);
			else
				gr_make_rgbcolor(b->xcontext, cms, x, y, 
					w+1, h, MIDGREY, MIDGREY, MIDGREY);
		}
		else if (b->multi_array[i] >= 3) {
			if ((c->xcontext->screen_depth < 8)  || FAKE_MONOCHROME)
				gr_make_gray(b->xcontext, x, y, 
					w+1, h, 75);
			else 
				gr_make_grayshade(b->xcontext, x, y, 
					w+1, h, DIMGREY);
		}
		/* compensate for grid line pixel */ 
		if (i == (index+BOX_SEG-2))
			y += (h-1);
		else
			y += h;
	}
	deactivate_selection (sel);
}


extern void
browser_select(Calendar *c, Browser *b, pr_pos *xy)
{
	int i, j;
	Selection *sel	= (Selection *) b->current_selection;
	Colormap cms;
	Pixel pixel;

	if (xy != NULL) {
		i = xy->x;
		j = xy->y;
	}
	else {
		i = sel->col;
		j = sel->row;
	}
	if (j >= 0) {
		if (c->xcontext->screen_depth < 8) {
			gr_draw_box(b->xcontext, (i * b->boxw) + 
				b->chart_x + 2, (j * b->boxh) + b->chart_y 
				+ 2, b->boxw - 4, b->boxh - 4, NULL);
		}
		else {
			pixel = c->xcontext->hilight_color;
        		XtVaGetValues(b->canvas, XmNcolormap, &cms, NULL);
			gr_draw_rgb_box(b->xcontext, (i * b->boxw) + 
				b->chart_x + 2, (j * b->boxh) + b->chart_y 
				+ 2, b->boxw - 4, b->boxh - 4, STDTTT,
				pixel, cms);
			gr_draw_rgb_box(b->xcontext, (i * b->boxw) + 
				b->chart_x + 1, (j * b->boxh) + b->chart_y 
				+ 1, b->boxw - 2, b->boxh - 2, STDTTT,
				pixel, cms);
		}
		sel->row = j;
		sel->col = i;
		sel->nunits = 1;
		activate_selection(sel);
	}
}

static void
select_weekhotbox(Calendar *c)
{
        int     n, x, y;
        new_XContext*xc     = c->xcontext;
	Week *w = (Week *)c->view->week_info;
        long    date    = c->view->date;
	Colormap 	cms;
	Pixel background_pixel;

        XtVaGetValues(c->canvas, XmNcolormap, &cms, NULL);
        XtVaGetValues(c->frame, XmNbackground, &background_pixel, NULL);

        /* Draw selection feedback on week view */
        if ((n = dow(date)) == 0)
                n = 6;
        else
                n--;

        if (n < 5) {
                x = w->x + n * w->day_width + 2;
                y = w->y + 2;
        }
	else {
                n -= (5 - 3);
                x = w->x + n * w->day_width + 2;
                y = w->y + w->day_height + 2;
        }
 
	if (c->xcontext->screen_depth < 8)
		gr_draw_box(xc, x, y, w->day_width - 4, w->label_height - 4, NULL);
	else {
		gr_draw_rgb_box(xc, x, y,
				w->day_width - 4, w->label_height - 4, STDTTT,
				background_pixel, cms);
		gr_draw_rgb_box(xc, x-1, y-1,
				w->day_width-2, w->label_height - 2, STDTTT,
				background_pixel, cms);
	}
}

extern void
paint_selection(Calendar *c)
{
        Props  *p = (Props*)c->properties;
        Selection *sel;
        int d, mo, beg = get_int_prop(p, CP_DAYBEGIN);
        Week *w = (Week *)c->view->week_info;
        pr_pos xy;

        switch ((Glance)c->view->glance) {
                case monthGlance:
                        calendar_select(c, daySelect, (caddr_t)NULL);
                        break;
                case dayGlance:
                        sel = (Selection *) c->view->current_selection;
                        sel->row = hour(c->view->date) - beg + 1;
                        calendar_select(c, hourSelect, (caddr_t)NULL);
                        break;
                case weekGlance:
                        sel = (Selection*)w->current_selection;
                        sel->row = hour(c->view->date) - beg;
                        sel->col =  (d = dow(c->view->date)) == 0 ? 6 : --d;                        	calendar_select(c, weekdaySelect, (caddr_t)NULL);
                        break;
                case yearGlance:
                        mo = month(c->view->date);
                        xy.y = month_row_col[mo-1][ROW];
                        xy.x = month_row_col[mo-1][COL];
                        calendar_select(c, monthSelect, (caddr_t)&xy);
                        break;
        }
}
