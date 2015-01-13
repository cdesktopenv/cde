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
/*******************************************************************************
**
**  dayglance.c
**
**  $XConsortium: dayglance.c /main/10 1996/11/21 19:42:19 drk $
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
static char sccsid[] = "@(#)dayglance.c 1.76 95/04/24 Copyr 1991 Sun Microsystems, Inc.";
#endif

#include <EUSCompat.h>
#include <stdio.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/Text.h>
#include <Xm/ToggleBG.h>
#include <Dt/HourGlass.h>
#include "util.h"
#include "stdlib.h"
#include "calendar.h"
#include "timeops.h"
#include "datefield.h"
#include "props.h"
#include "help.h"
#include "x_graphics.h"
#include "format.h"
#include "weekglance.h"
#include "monthglance.h"
#include "yearglance.h"
#include "MonthPanel.h"
#include "getdate.h"
#include "select.h"
#include "dayglance.h"
#include "editor.h"
#include "todo.h"
#include "print.h"
#include "group_editor.h"

#define XOS_USE_XT_LOCKING
#define X_INCLUDE_TIME_H
#if defined(linux)
#undef SVR4
#endif
#include <X11/Xos_r.h>

extern int debug;


/* private function prototypes */
static Boolean _print_day(Calendar*, int, void *,
			  Tick, Props*, Boolean);
static void day_btn_cb(Widget, XtPointer, XtPointer);
static void create_month_panels(Calendar *);
static void display_monthpanels(Calendar *);
static void update_quarter(Calendar *);
static Boolean in_moboxes(Calendar *, int, int);

#define INSIDE_MARGIN 6

extern void
paint_day_header(Calendar *c, Tick date, void *rect)
{
	Props *p = (Props*)c->properties;
	OrderingType ot = get_int_prop(p, CP_DATEORDERING);
	int pfy, x;
	char buf[100];
	Boolean inrange = False;
	Colormap cmap;
	Pixel foreground_pixel;
	XFontSetExtents fontextents;
	struct tm *tm;
	_Xltimeparams localtime_buf;

	tm = _XLocaltime(&date, localtime_buf);

	CalFontExtents(c->fonts->labelfont, &fontextents);
	pfy = fontextents.max_logical_extent.height;
        XtVaGetValues(c->canvas, XmNcolormap, &cmap, NULL);
        XtVaGetValues(c->canvas, XmNforeground, &foreground_pixel, NULL);
        buf [0] = '\0';
	/* NL_COMMENT
	   Attention Translator:

	   This string is used in the calendar day view.  In the C locale
	   it has the form:

		Monday, January 16, 1995

	   strftime conversion string: "%A, %B %e, %Y" is used.  

	   Use the appropriate strftime conversion for your locale.
	*/
	strftime(buf, 100, catgets(c->DT_catd, 1, 992, "%A, %B %e, %Y"), tm);
	inrange = today_inrange(c, date);
	x = gr_center(c->view->winw-(int)MOBOX_AREA_WIDTH, buf,
			c->fonts->labelfont) + (int)MOBOX_AREA_WIDTH;
	if (c->xcontext->screen_depth >= 8 && inrange) 
        	gr_text_rgb(c->xcontext, x, c->view->topoffset - (pfy/2),
                        c->fonts->labelfont, buf, foreground_pixel, 
			cmap, rect);
	else
        	gr_text(c->xcontext, x, c->view->topoffset - (pfy/2),
                        c->fonts->labelfont, buf, rect);
}
	
extern int
morning(int hr)
{
	return(hr<12);
}

static void
paint_dayview_appts(Calendar *c, Paint_cache *cache, int a_total, void *rect)
{
	int w = c->view->boxw;
	int h = c->view->boxh;
	int begin_time, end_time;
	int x, x2, y, y2, num_hrs, i, last_hr, hr, x_off;
	Cal_Font *pf = c->fonts->boldfont;
	Cal_Font *pf2 = c->fonts->viewfont;
	XFontSetExtents fontextents;
	XFontSetExtents fontextents2;
	Props *p = (Props*)c->properties;
	Boolean am = True;
	char buf[5], *appt_str;
	int pfy, curr_line, maxlines;
	Lines *lines = NULL, *headlines = NULL;
	DisplayType disp_t;
	Colormap cmap;
	Pixel fg;
	Tick start_tick, end_tick;
	int	nop, hrbox_margin;

	CalFontExtents(pf, &fontextents);
	CalFontExtents(pf2, &fontextents2);


	XtVaGetValues(c->canvas, XmNcolormap, &cmap, XmNforeground, &fg, NULL);

	/* draw horizontal lines */
	begin_time = get_int_prop(p, CP_DAYBEGIN);
	end_time = get_int_prop(p, CP_DAYEND);
	disp_t = get_int_prop(p, CP_DEFAULTDISP);
	num_hrs = end_time - begin_time + 1;

	if (disp_t == HOUR12)
		CalTextExtents(pf, "12pm", 4, &nop, &nop, &hrbox_margin, &nop);
	else
		CalTextExtents(pf, "24 ", 3, &nop, &nop, &hrbox_margin, &nop);

	x = MOBOX_AREA_WIDTH+2;
	x2 = x + w;
	y = c->view->topoffset;
	for (i = 0; i <= num_hrs; i++) {
		gr_draw_line(c->xcontext, x, y, x2, y, gr_solid, rect);
		y += h;
	}
	/* draw vertical line */
	y = c->view->topoffset;
	y2 = y + num_hrs * h;
	x += hrbox_margin;
	gr_draw_line(c->xcontext, x, y, x, y2, gr_solid, rect);

	x = MOBOX_AREA_WIDTH+3;
	y += h/2+4;

	/* draw in hours */
	for (i = begin_time - 1; i < end_time; i++) {
		hr = i;
		if (i < begin_time)
			buf[0] = '\0';
		else if (disp_t == HOUR12) {
			am = adjust_hour(&hr);
			(void) sprintf(buf, "%d%s", hr, am ? "a" : "p");
		}
		else
			(void) sprintf(buf, "%02d", hr);
		x_off = gr_center(hrbox_margin, buf, pf); 

/* REVISIT: unclear why we're still distinguishing between gr_text[_rgb]
		if (c->xcontext->screen_depth >= 8) 
			gr_text_rgb(c->xcontext, x+x_off, y, pf,
				buf, fg, cmap, rect);
		else
*/
			gr_text(c->xcontext, x+x_off, y, pf, buf, rect);

		y += h;
	}

	/* draw in appointments */

	x = MOBOX_AREA_WIDTH + hrbox_margin + 6;
	pfy = fontextents2.max_logical_extent.height;

	maxlines = (h - 6) / pfy;
	curr_line = last_hr = 0;

	/* loop thru, getting out the "no time" appointments */

        for (i = 0; i < a_total; i++) {
		if (i != a_total)
			last_hr = hr;
		hr = begin_time;
		if (cache[i].show_time == 0) {
			if (last_hr != hr) curr_line = 0;
			y = c->view->topoffset + 2 + pfy;
			if (curr_line < maxlines) {
				y += (curr_line * pfy) + h * (hr - begin_time);
				headlines = lines = text_to_lines(cache[i].summary, 4);

				start_tick = cache[i].start_time;
				end_tick = cache[i].end_time;
				if (lines != NULL && lines->s != NULL) { 
					appt_str = ckalloc(cm_strlen(lines->s)+18);
					format_line(start_tick, lines->s, 
						appt_str, end_tick, 
						cache[i].show_time, disp_t);
					lines = lines->next;
				}
				else {
					appt_str = ckalloc(15);
					format_line(start_tick, (char*)NULL, 
						appt_str, end_tick, 
						cache[i].show_time, disp_t);
				}
				appt_str[cm_strlen(appt_str)] = '\0';

/* REVISIT: unclear why we're still distinguishing between gr_text[_rgb]
				if (c->xcontext->screen_depth >= 8) 
					gr_text_rgb(c->xcontext, x, y,
					   pf2, appt_str, fg, cmap, rect);
				else
*/
					gr_text(c->xcontext, x, y,
					   pf2, appt_str, rect);

				free(appt_str); appt_str = NULL;
				curr_line++;
				if (curr_line < maxlines && lines != NULL) {
				 	appt_str = ckalloc(324);
					cm_strcpy(appt_str, "    ");
					while (lines != NULL) { 
						if (lines->s != NULL) 
							cm_strcat(appt_str, lines->s);
						lines = lines->next;
						if (lines != NULL && lines->s != NULL)
							cm_strcat(appt_str, " - ");
					}
					y += pfy;

/* REVISIT: unclear why we're still distinguishing between gr_text[_rgb]

					if (c->xcontext->screen_depth >= 8) 
						gr_text_rgb(c->xcontext, x, y,
						   pf2, appt_str, fg,
						   cmap, rect);
					else
*/
						gr_text(c->xcontext, x, y,
						   pf2, appt_str, rect);

					curr_line++;
					free(appt_str); appt_str = NULL;
				}
				destroy_lines(headlines); lines=NULL;
			}
		}
	}

        for (i = 0; i < a_total; i++) {
		if (i != a_total)
			last_hr = hr;

		start_tick = cache[i].start_time;
		end_tick = cache[i].end_time;
		hr = hour(start_tick);
		if (hr >= begin_time && hr < end_time && (cache[i].show_time && !magic_time(start_tick))) {
			if (last_hr != hr) curr_line = 0;
			y = c->view->topoffset + 2 + pfy;
			if (curr_line < maxlines) {
				y += (curr_line * pfy) + h * (hr - begin_time + 1);
				headlines = lines = text_to_lines(cache[i].summary, 4);
				if (lines != NULL && lines->s != NULL) { 
					appt_str = ckalloc(cm_strlen(lines->s)+18);
					format_line(start_tick, lines->s, 
						appt_str, end_tick, 
						cache[i].show_time, disp_t);
					lines = lines->next;
				}
				else {
					appt_str = ckalloc(15);
					format_line(start_tick, (char*)NULL, 
						appt_str, end_tick, 
						cache[i].show_time, disp_t);
				}
				appt_str[cm_strlen(appt_str)] = '\0';

/* REVISIT: unclear why we're still distinguishing between gr_text[_rgb]
				if (c->xcontext->screen_depth >= 8) 
					gr_text_rgb(c->xcontext, x, y,
					   pf2, appt_str, fg, cmap, rect);
				else
*/
					gr_text(c->xcontext, x, y,
					   pf2, appt_str, rect);

				free(appt_str); appt_str = NULL;
				curr_line++;
				if (curr_line < maxlines && lines != NULL) {
				 	appt_str = ckalloc(324);
					cm_strcpy(appt_str, "    ");
					while (lines != NULL) { 
						if (lines->s != NULL) 
							cm_strcat(appt_str, lines->s);
						lines = lines->next;
						if (lines != NULL && lines->s != NULL)
							cm_strcat(appt_str, " - ");
					}
					y += pfy;

/* REVISIT: unclear why we're still distinguishing between gr_text[_rgb]

					if (c->xcontext->screen_depth >= 8) 
						gr_text_rgb(c->xcontext, x, y,
						   pf2, appt_str, fg,
						   cmap, rect);
					else 
*/
						gr_text(c->xcontext, x, y,
						   pf2, appt_str, rect);

					curr_line++;
					free(appt_str); appt_str = NULL;
				}
				destroy_lines(headlines); lines=NULL;
			}
		}
	}
}

extern void
init_mo(Calendar *c)
{
	Day *day_info = (Day *)c->view->day_info;
	day_info->month1 = previousmonth(c->view->date);
	day_info->month2 = c->view->date;
	day_info->month3 = nextmonth(c->view->date);
}

extern void 
init_dayview(Calendar *c)
{
        int  tot_rows, wks_1, wks_2, wks_3;
	Dimension w, h;
	Day *day_info = (Day *) c->view->day_info;

	/*
	 * Create month panels if they're not already there
	 */
	if (day_info->month_panels == (Widget *) NULL) {
		day_info->month_panels = (Widget *)ckalloc(3 * sizeof(Widget));

		create_month_panels(c);
	}


        XtVaGetValues(c->canvas, XmNwidth, &w, XmNheight, &h, NULL);
	(void)cache_dims(c, w, h);

	day_info->day_selected = -1;
	day_info->mobox_width = (int)MOBOX_AREA_WIDTH - 
			2*c->view->outside_margin;
	/* col width of day number in month boxes */
        day_info->col_w = 
		(day_info->mobox_width-INSIDE_MARGIN*2)/7;
	/* width of all of the month boxes */
        day_info->mobox_width = 7 * day_info->col_w + 
			2 * INSIDE_MARGIN;

	wks_1 = numwks(day_info->month1);
	wks_2 = numwks(day_info->month2);
	wks_3 = numwks(day_info->month3); 
	/* total rows in three months */ 
	tot_rows = wks_1 + wks_2 + wks_3 + 3; 

	/* row height of day number in month boxes */
        day_info->row_h = (c->view->winh - 3*c->view->topoffset-c->view->outside_margin) 
			/ tot_rows;

	/* height of 1st month */
        day_info->mobox_height1 = day_info->row_h * 
			(wks_1+1)+1;
	/* height of 2nd month */
        day_info->mobox_height2 = day_info->row_h * 
			(wks_2+1)+1;
	/* height of 2rd month */
        day_info->mobox_height3 = day_info->row_h * 
			(wks_3+1)+1;

	day_info->month1_y = c->view->topoffset;
	day_info->month2_y = 2*c->view->topoffset + 
			day_info->mobox_height1; 
	day_info->month3_y = 3*c->view->topoffset + 
			day_info->mobox_height1 +
			day_info->mobox_height2;
	((Selection*)(c->view->current_selection))->row = 0;
}

extern void
monthbox_xytodate(Calendar *c, int x, int y)
{
	char str[5];
	Day *day_info = (Day *)c->view->day_info;
	XFontSetExtents fontextents;
	int pfy;
	int col_w = day_info->col_w;
        int row_h = day_info->row_h;
	int row, col, x_off;
	int day_selected, tmpx;

	CalFontExtents(c->fonts->labelfont, &fontextents);
	pfy = fontextents.max_ink_extent.height;

	col = (x-c->view->outside_margin-INSIDE_MARGIN) / col_w;
	if (col < 0) return;
	tmpx = c->view->outside_margin + INSIDE_MARGIN + col * col_w;

	if (y < (day_info->month1_y + 
			day_info->mobox_height1)) {
		row = (y-day_info->month1_y-row_h) / row_h;
        	day_selected = (7 * (row+1)) - fdom(day_info->month1)
				 - (6 - col);
		if (day_selected <= 0 || day_selected >
			 monthlength(day_info->month1)) 
				return;
		day_info->day_selected = day_selected;
		day_info->day_selected_y = 
			day_info->month1_y + (row+1)*row_h; 
		c->view->olddate = c->view->date;
		c->view->date = next_ndays(first_dom(day_info->month1),
			 day_info->day_selected);
	}
	else if (y < (day_info->month2_y +
                        day_info->mobox_height2)) {
		row = (y-day_info->month2_y-row_h) / row_h;
        	day_selected = (7 * (row+1)) - fdom(day_info->month2)
				 - (6 - col);
		if (day_selected <= 0 || day_selected >
			 monthlength(day_info->month2)) 
				return;
		day_info->day_selected = day_selected;
		day_info->day_selected_y = 
			day_info->month2_y + (row+1)*row_h; 
		c->view->olddate = c->view->date;
		c->view->date = next_ndays(first_dom(day_info->month2),
			 day_info->day_selected);
	}
	else if (y < (day_info->month3_y +
                        day_info->mobox_height3)) {
		row = (y-day_info->month3_y-row_h) / row_h;
        	day_selected = (7 * (row+1)) - fdom(day_info->month3) 
				- (6 - col);
		if (day_selected <= 0 || day_selected > 
				monthlength(day_info->month3)) 
				return;
		day_info->day_selected = day_selected;
		day_info->day_selected_y = 
			day_info->month3_y + (row+1)*row_h; 
		c->view->olddate = c->view->date;
		c->view->date = next_ndays(first_dom(day_info->month3),
			 day_info->day_selected);
	}
	day_info->day_selected_x = tmpx;
	sprintf(str, "%d", day_info->day_selected);
	x_off = gr_center(col_w, str, c->fonts->labelfont);
	day_info->day_selected_x2 =
		day_info->day_selected_x+x_off;
	day_info->day_selected_y2 =
		day_info->day_selected_y + pfy;

}

extern void
monthbox_datetoxy(Calendar *c)
{
	char str[5];
	int week, x_off, dayw, daym, mo;
	Day *day_info = (Day *) c->view->day_info;
	XFontSetExtents fontextents;
	int pfy;
	int col_w = day_info->col_w;
	int row_h = day_info->row_h;
	struct tm tm;
	_Xltimeparams localtime_buf;

	CalFontExtents(c->fonts->labelfont, &fontextents);
	pfy = fontextents.max_logical_extent.height;

	tm = *_XLocaltime(&c->view->date, localtime_buf);
	mo = tm.tm_mon+1;
	dayw = tm.tm_wday;
	daym = tm.tm_mday;
	week = (12+tm.tm_mday-tm.tm_wday)/7;
	day_info->day_selected_x = c->view->outside_margin + 
			INSIDE_MARGIN + col_w*dayw;

	if (mo == month(day_info->month1)) 
		day_info->day_selected_y = 
			day_info->month1_y + 
			row_h*week;
	else if (mo == month(day_info->month2))  
		day_info->day_selected_y = 
			day_info->month2_y + 
			row_h*week;
	else if (mo == month(day_info->month3)) 
		day_info->day_selected_y = 
			day_info->month3_y + 
			row_h*week;
	sprintf(str, "%d", daym);
	x_off = gr_center(col_w, str, c->fonts->labelfont);
	day_info->day_selected_x2 = 
			day_info->day_selected_x+x_off;
	day_info->day_selected_y2 = 
			day_info->day_selected_y + pfy;
	day_info->day_selected = daym;
}

static Boolean
in_moboxes(Calendar *c, int x, int y)
{
	int margin = c->view->outside_margin;
	int topoff = c->view->topoffset;
	Boolean in_mobox = False;
	Day *day_info = (Day *)c->view->day_info;
	int row_h = day_info->row_h;

	if (x < (MOBOX_AREA_WIDTH-margin-2*INSIDE_MARGIN) && 
	   	x > margin && y > topoff && 
		( (y < (day_info->month3_y+
			day_info->mobox_height3) &&
			y > (day_info->month3_y+row_h)) || 
		  (y < (day_info->month2_y+
                	day_info->mobox_height2) &&
			y > (day_info->month2_y+row_h)) ||
		  (y < (day_info->month1_y+
			day_info->mobox_height1) &&
			y > day_info->month1_y+row_h) ) )
		in_mobox = True;

	return in_mobox;
}

extern void
paint_dayview(Calendar *c, Boolean repaint, XRectangle *rect, Boolean update_months)
{
	Props 		*p = (Props*)c->properties;
	int 		num_hrs;
	int 		beg = get_int_prop(p, CP_DAYBEGIN);
	int 		end = get_int_prop(p, CP_DAYEND);
	time_t 		start, stop;
	CSA_attribute 		*range_attrs;
        CSA_entry_handle 	*list;
	CSA_enum 	*ops;
	int 		i, j;
	CSA_uint32 	a_total;
	int		panel0_year, panel0_month;
	int		panel1_year, panel1_month;
	int		panel2_year, panel2_month;
	int		year_num, month_num;
	Boolean		day_not_on_panel = False;
	Day 		*d = (Day *)c->view->day_info;
	int		top_panel = 0;
	int		bottom_panel = 2;

	num_hrs = end - beg + 1;
	if (c->paint_cache == NULL) {
		start = (time_t) lower_bound(0, c->view->date);
        	stop = (time_t) next_nhours(start, end+1) - 1;
		setup_range(&range_attrs, &ops, &j, start, stop, CSA_TYPE_EVENT, 0,
		    	B_FALSE, c->general->version);
        	csa_list_entries(c->cal_handle, j, range_attrs, ops, &a_total, &list, NULL);
	
		free_range(&range_attrs, &ops, j);
		allocate_paint_cache(list, a_total, &c->paint_cache);
		c->paint_cache_size = a_total;
		csa_free(list);
	}

	year_num = year(c->view->date);
	month_num = month(c->view->date);
	XtVaGetValues(d->month_panels[1],
		XmNyear, &panel1_year,
		XmNmonth, &panel1_month,
		NULL);

	if ((panel1_year == year(get_bot())) && (panel1_month == 1))
		top_panel = 1;
	else if ((panel1_year == year(get_eot())) && (panel1_month == 12))
		bottom_panel = 1;

	XtVaGetValues(d->month_panels[top_panel],
		XmNyear, &panel0_year,
		XmNmonth, &panel0_month,
		NULL);

	if ((year_num < panel0_year) || 
	    ((year_num == panel0_year) && (month_num < panel0_month)))
		day_not_on_panel = True;
	
	XtVaGetValues(d->month_panels[bottom_panel],
		XmNyear, &panel2_year,
		XmNmonth, &panel2_month,
		NULL);

	if ((year_num > panel2_year) || 
	    ((year_num == panel2_year) && (month_num > panel2_month)))
		day_not_on_panel = True;

	/* set up month panels */
	if (update_months || day_not_on_panel)
		update_quarter(c);

	/* no need to do this on a damage event */

	if (!rect)
		display_monthpanels(c);

	/* repaint appointment area */
	if (repaint) {
		int	line_length;

		gr_clear_area(c->xcontext, 0, 0, 
			c->view->winw, c->view->winh);
		line_length = c->view->topoffset + 
			      ((end - beg + 1) * c->view->boxh);
		/* draw line separating mo. boxes and appts. */
        	gr_draw_line(c->xcontext, (int)MOBOX_AREA_WIDTH+1,
			 0, (int)MOBOX_AREA_WIDTH+1,
                         line_length, gr_solid, rect);
        	gr_draw_line(c->xcontext, (int)MOBOX_AREA_WIDTH+2, 
			 0, (int)MOBOX_AREA_WIDTH+2,
                         line_length, gr_solid, rect);
        	gr_draw_line(c->xcontext, (int)MOBOX_AREA_WIDTH+2, 
			 c->view->topoffset-1, c->view->winw,
                         c->view->topoffset-1,  gr_solid, rect);
        	gr_draw_line(c->xcontext, (int)MOBOX_AREA_WIDTH+2, 
			 c->view->topoffset, c->view->winw,
                         c->view->topoffset, gr_solid, rect);
		paint_dayview_appts(c, c->paint_cache, c->paint_cache_size, rect);
	}
	/* just repaint schedule area */
	else {
		gr_clear_area(c->xcontext, 
				(int)MOBOX_AREA_WIDTH+4, 0, 
				c->view->winw - (int)MOBOX_AREA_WIDTH+4,
				c->view->winh);
        	gr_draw_line(c->xcontext, (int)MOBOX_AREA_WIDTH+2, 
			 c->view->topoffset, c->view->winw,
                         c->view->topoffset,  gr_solid, rect);
        	gr_draw_line(c->xcontext, (int)MOBOX_AREA_WIDTH+2, 
			 c->view->topoffset+1, c->view->winw,
                         c->view->topoffset+1, gr_solid, rect);
		paint_dayview_appts(c, c->paint_cache, c->paint_cache_size, rect);
	}

	paint_day_header(c, c->view->date, rect);
}

extern void
paint_day(Calendar *c)
{
	Day *day_info = (Day *) c->view->day_info;

        c->view->glance = dayGlance;
	XmToggleButtonGadgetSetState(c->day_scope, True, False);
	gr_clear_area(c->xcontext, 0, 0, c->view->winw, c->view->winh);
	if (day_info->month_panels == (Widget *) NULL)
		(void)init_dayview(c);
	paint_dayview(c, True, NULL, True); 
	calendar_select(c, hourSelect, (caddr_t)NULL);
}
	
/*
 * handler for button to switch to day view.
 */
extern void
day_button (Widget widget, XtPointer data, XtPointer cbs)
{
        Calendar *c = calendar;

	if (c->view->glance == dayGlance)
		return;

	XtUnmapWidget(c->canvas);
	invalidate_cache(c);

	switch (c->view->glance) {
		case weekGlance:
			c->view->glance = dayGlance;
			cleanup_after_weekview(c);
			break;
		case yearGlance:
			c->view->glance = dayGlance;
			cleanup_after_yearview(c);
			break;
		case monthGlance:
			c->view->glance = dayGlance;
			cleanup_after_monthview(c);
			break;
		default:
			break;	
	}

	init_mo(c);
	(void)init_dayview(c);

	XtMapWidget(c->canvas);
}

extern void
print_day_range(Calendar *c, Tick start_tick, Tick end_tick)
{
	int 		n;
	register Tick 	first_date = start_tick;
        Boolean 	done = False, first = True;
        int 		num_page = 1;
	Props 		*pr = (Props*)c->properties;
	void *xp = (void *)NULL;

	n = (end_tick - start_tick)/daysec + 1;

	if (n <= 0) n = 1;

	if ((xp = x_open_file(c)) == (void *)NULL)
	  return;

	for (; n > 0; n--) {
	  while (!done) {
	    done = _print_day(c, num_page, xp,
			      first_date, pr, first);
	    num_page++;
	    first = False;
	  }
	  done = False;
	  num_page = 1;
	}

	x_print_file(xp, c);
}

static int
count_day_pages(Calendar *c, int lines_per_page, Tick tick)
{
	int	n, i, j, timeslots, num_appts, pages, max = 0; 
	Props *p = (Props *)c->properties;
	int       daybegin = get_int_prop(p, CP_DAYBEGIN);
	int       dayend   = get_int_prop(p, CP_DAYEND);
        char 	*location;
	time_t 	start, end;
        CSA_return_code stat;
        CSA_entry_handle *list;
        CSA_attribute *range_attrs;
	CSA_enum *ops;
	CSA_uint32 a_total;

	for (i=daybegin; i < dayend; i++) {
		start = (time_t) lower_bound(i, tick);
		end = (time_t) next_nhours(start+1, 1) - 1;
		setup_range(&range_attrs, &ops, &j, start, end,
			    CSA_TYPE_EVENT, 0, B_FALSE, c->general->version);
        	csa_list_entries(c->cal_handle, j, range_attrs, ops, &a_total, &list, NULL);
		free_range(&range_attrs, &ops, j);

                num_appts = count_multi_appts(list, a_total, c);

		if (num_appts > max)
			max = num_appts;

                csa_free(list);
	}
 
       	pages = max / lines_per_page;
        if ((max % lines_per_page) > 0)
                pages++;
 
        return(pages);
}

static Boolean
_print_day(Calendar *c,
    int num_page,
    void *xp,
    Tick first_date,
    Props *p,
    Boolean first)
{
    char buf[100];
    int n, i, j, timeslots, num_appts, pages;
    int max = 0;
    int daybegin = get_int_prop(p, CP_DAYBEGIN);
    int dayend   = get_int_prop(p, CP_DAYEND);
    OrderingType ord_t = get_int_prop(p, CP_DATEORDERING);
    Boolean more, done = False, all_done = True;
    char *location;
    CSA_return_code stat;
    CSA_entry_handle *list;
    CSA_attribute *range_attrs;
    CSA_enum *ops;
    CSA_uint32 a_total;
    time_t start, stop;
    int lines_per_page;
    static Tick tick = 0;
    static int total_pages = 0;

    /*
     * Need to find the max number of timeslots which will be shown
     * in one column, for later calculation of box height.
     */  
    if ((!morning(daybegin)) || dayend <= 12) 
    	timeslots = dayend - daybegin;
    else
    	timeslots = ((12-daybegin) > (dayend-12)) ?
    		     (12-daybegin) : (dayend-12);

    x_init_printer(xp, PORTRAIT);
    x_init_day(xp, timeslots);
    lines_per_page = x_get_day_lines_per_page(xp);

    if (first)
      tick = first_date;

    if (num_page > 1)
      tick = prevday(tick);
    else
      total_pages = (lines_per_page > 0) ?
	count_day_pages(c, lines_per_page, tick) : 1;

    format_date(tick, ord_t, buf, 1, 0, 0);

    x_print_header(xp, buf, num_page, total_pages);
    x_day_header(xp);

    for (i=daybegin; i < dayend; i++) {
      start = (time_t) lower_bound(i, tick);
      stop = (time_t) next_nhours(start+1, 1) - 1;
      setup_range(&range_attrs, &ops, &j, start, stop,
		  CSA_TYPE_EVENT, 0, B_FALSE, c->general->version);

      csa_list_entries(c->cal_handle, j, range_attrs,
		       ops, &a_total, &list, NULL);
      free_range(&range_attrs, &ops, j);
 
      num_appts = count_multi_appts(list, a_total, c);

      if ((lines_per_page > 0) && (num_appts > (lines_per_page * num_page)))
	more = True;
      else
	more = False;

      x_day_timeslots (xp, i, more);

      if (lines_per_page > 0)
	done = x_print_multi_appts (xp, list, a_total,
				    num_page, dayGlance);
      else done = True;

      if (!done)
	all_done = False;
      csa_free(list);
    }
 
    x_finish_printer(xp);
    tick = nextday(tick); 

    return(all_done);
}

static int
day_xytoclock(Calendar *c, int x, int y, Tick t)
{
	int daybegin, hr, val;
	char buf[10];
	struct tm tm;
	Props *p;
	_Xltimeparams localtime_buf;

	p	= (Props *)c->properties;
	daybegin = get_int_prop(p, CP_DAYBEGIN);
	tm	= *_XLocaltime(&t, localtime_buf);

	hr = (x == 1) ? (12 + y) : (y + daybegin - 1);

	/* 
	 * If y == 0 then the user is clicking on the no-time area.  There
	 * is no hour associated with no-time events.
	 */
	if (y == 0) hr = 0;

	(void)sprintf(buf, "%d/%d/%d", tm.tm_mon+1, tm.tm_mday, tm.tm_year+1900);
	val	=cm_getdate(buf, NULL);
	val	= val+(hr*(int)hrsec);
	adjust_dst(t, val);
	return(val);
}

extern void
day_event(XEvent *event)
{
        static int lastcol, lastrow;
        static XEvent lastevent;
        pr_pos xy;
        int x, y, i, j;
        int boxw, boxh, margin, id;
        Calendar *c = calendar;
        Tick date = c->view->date;
        Boolean in_mbox = False; /* in month boxes ? */
        Editor *e = (Editor*)c->editor;
        ToDo *t = (ToDo*)c->todo;
        GEditor *ge = (GEditor*)c->geditor;
        Day *day_info = (Day *)c->view->day_info;
 
        boxw    = c->view->boxw;
        boxh    = c->view->boxh;
        margin  = c->view->outside_margin;
        x       = event->xbutton.x;
        y       = event->xbutton.y;
 
        /* boundary conditions */
        if ((!(in_mbox = in_moboxes(c, x, y)) && x < MOBOX_AREA_WIDTH+2)
||
                (x > MOBOX_AREA_WIDTH+2 && y < c->view->topoffset)) {
                lastcol=0; lastrow=0;
                return;
        }
        if (in_mbox) {
                xy.x = (x - margin)/ day_info->col_w;
                xy.y = (x - c->view->topoffset)/ day_info->row_h;
        }
        else {
                xy.x    = boxw;
                xy.y    = (y - c->view->topoffset)/boxh;
        }
        switch(event->type) {
        case MotionNotify:
                if (!in_mbox) {
                        if (xy.x !=lastcol || xy.y !=lastrow) {
                                calendar_deselect(c);
                                j = day_xytoclock(c, xy.x, xy.y, date);
                                if (j >= 0) {
                                        c->view->olddate = c->view->date;                                        c->view->date = j;
                                        calendar_select(c, hourSelect, (caddr_t)&xy);
                                }
                                lastcol=xy.x;
                                lastrow=xy.y;
                        }
                }
                break;
        case ButtonPress:
		if (ds_is_double_click(&lastevent, event)) {
			_DtTurnOnHourGlass(c->frame);
			j = day_xytoclock(c, xy.x, xy.y, date);
			if (lastcol == xy.x && lastrow == xy.y)
				show_editor(c, j, next_nhours(j, 1), xy.y == 0 ? True : False);
			if (in_mbox) {
				monthbox_deselect(c);
				monthbox_xytodate(c, x, y);
				monthbox_select(c);
			} else
				show_editor(c, j, next_nhours(j, 1), xy.y == 0 ? True : False);
			_DtTurnOffHourGlass(c->frame);
		} else {
			if (in_mbox) {
				monthbox_deselect(c);
				monthbox_xytodate(c, x, y);
				paint_dayview(c, False, NULL, True);
				calendar_select(c, hourSelect, (caddr_t)NULL);
				monthbox_select(c);
				j = day_xytoclock(c, xy.x, xy.y, date);
				if (editor_showing(e)) {
					set_editor_defaults(e, j,
							    next_nhours(j, 1), False);
					add_all_appt(e);
				}
				if (todo_showing(t)) {
					set_todo_defaults(t);
					add_all_todo(t);
				}
				if (geditor_showing(ge)) {
					set_geditor_defaults(ge, j,
							     next_nhours(j, 1));
					add_all_gappt(ge);
				}
			} else {
				calendar_deselect(c);
				j = day_xytoclock(c, xy.x, xy.y, date);
				if (j >= 0) {
					c->view->olddate = c->view->date;
					c->view->date = j;
					calendar_select(c, hourSelect,
							(caddr_t)&xy);
					if (editor_showing(e)) {
						set_editor_defaults(e, j,
							next_nhours(j, 1), xy.y == 0 ? True : False);
						add_all_appt(e);
					}
					if (todo_showing(t)) {
						set_todo_defaults(t);
						add_all_todo(t);
					}
					if (geditor_showing(ge)) {
						set_geditor_defaults(ge, j,
							next_nhours(j, 1));
						add_all_gappt(ge);
					}
				}
			}
		}
		lastcol=xy.x;
		lastrow=xy.y;
                break;
        default:
        	break;
        };            /* switch */
        lastevent = *event;
}

static void
display_monthpanels(Calendar *c)
{
	Day 	*d = (Day *)c->view->day_info;
	char 	buf[BUFSIZ];
	int	lastyear = year(get_eot());
	int	firstyear = year(get_bot());
	int	panel_year;
	int	panel_month;

	XtManageChild(d->panel_form);

	XtVaGetValues(d->month_panels[1],
		XmNyear, &panel_year,
		XmNmonth, &panel_month,
		NULL);

	if ((panel_year == firstyear) && (panel_month == 1)) {
		XtUnmapWidget(d->month_panels[0]);
		XtMapWidget(d->month_panels[2]);
		sprintf(buf, "%s", catgets(c->DT_catd, 1, 623, "Calendar does not display dates prior to January 1, 1970"));
		set_message(c->message_text, buf);
	}
	else if ((panel_year == lastyear) && (panel_month == 12)) {
		XtMapWidget(d->month_panels[0]);
		XtUnmapWidget(d->month_panels[2]);
		sprintf(buf, "%s", catgets(c->DT_catd, 1, 624, "Calendar does not display dates after December 31, 2037"));
		set_message(c->message_text, buf);
	}
	else {
		XtMapWidget(d->month_panels[0]);
		XtMapWidget(d->month_panels[2]);
		set_message(c->message_text, "");
	}

	/* switch canvas to attach using its XmNleftPosition value */
/* THIS IS THE RIGHT THIS TO DO, BUT ONLY AFTER REMOVING THE
   CODE THAT DRAWS THE OLD MO_BOXES.  FOR NOW, WE JUST MANAGE THE
   MONTHPANELS ON TOP OF THAT PART OF THE CANVAS - dac
	XtVaSetValues(c->canvas,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNleftPosition, 40,
		NULL);
*/

}

extern void
cleanup_after_dayview(Calendar *c)
{
	Day *d = (Day *)c->view->day_info;

	invalidate_cache(c);

	XtUnmanageChild(d->panel_form);

	XmToggleButtonGadgetSetState(c->day_scope, False, False);

	/* make canvas attach to form, and ignore its XmNleftPosition */
/* DITTO ABOVE: THIS ISN'T NEEDED TILL WE ACTUALLY DO RESIZE THE
   CANVAS TO ADJOIN THE MONTHPANELS WHILE DAY VIEW IS UP
	XtVaSetValues(c->canvas, XmNleftAttachment, XmATTACH_FORM, NULL);
*/

	gr_clear_area(calendar->xcontext, 0, 0, calendar->view->winw,
		       calendar->view->winh);

	set_message(c->message_text, "");
}

static void
create_month_panels(Calendar *c)
{
	Day *d = (Day *)c->view->day_info;
	Arg al[20];
	int ac;

	ac=0;
	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET); ac++;
        XtSetArg(al[ac], XmNtopWidget, c->canvas); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
        XtSetArg(al[ac], XmNrightPosition, 40); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
        XtSetArg(al[ac], XmNleftOffset, 2); ac++;
        XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_POSITION); ac++;
        XtSetArg(al[ac], XmNbottomPosition, 95); ac++;
        XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	d->panel_form = XmCreateForm(c->form, "quarter", al, ac);


	ac=0;
        XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
        XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_POSITION); ac++;
        XtSetArg(al[ac], XmNbottomPosition, 33); ac++;
        XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNmonth, month(previousmonth(c->view->date))); ac++;
	XtSetArg(al[ac], XmNyear, year(previousmonth(c->view->date))); ac++;
        d->month_panels[0] = XmCreateMonthPanel(d->panel_form, "lastMonth", al, ac);
        XtAddCallback(d->month_panels[0], XmNactivateCallback,
                        day_btn_cb, (XtPointer) c);
	XtAddCallback(d->month_panels[0], XmNhelpCallback, 
			(XtCallbackProc)help_view_cb, NULL);

	ac=0;
        XtSetArg(al[ac], XmNtopAttachment, XmATTACH_POSITION); ac++;
	XtSetArg(al[ac], XmNtopPosition, 33); ac++;
        XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_POSITION); ac++;
        XtSetArg(al[ac], XmNbottomPosition, 66); ac++;
        XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNmonth, month(c->view->date)); ac++;
	XtSetArg(al[ac], XmNyear, year(c->view->date)); ac++;
        d->month_panels[1] = XmCreateMonthPanel(d->panel_form, "thisMonth", al, ac);
        XtAddCallback(d->month_panels[1], XmNactivateCallback,
                        day_btn_cb, (XtPointer) c);
	XtAddCallback(d->month_panels[1], XmNhelpCallback, 
			(XtCallbackProc)help_view_cb, NULL);

	ac=0;
        XtSetArg(al[ac], XmNtopAttachment, XmATTACH_POSITION); ac++;
        XtSetArg(al[ac], XmNtopPosition, 66); ac++;
        XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
        XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
        XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNmonth, month(nextmonth(c->view->date))); ac++;
	XtSetArg(al[ac], XmNyear, year(nextmonth(c->view->date))); ac++;
        d->month_panels[2] = XmCreateMonthPanel(d->panel_form, "nextMonth", al, ac);
        XtAddCallback(d->month_panels[2], XmNactivateCallback,
                        day_btn_cb, (XtPointer) c);
	XtAddCallback(d->month_panels[2], XmNhelpCallback, 
			(XtCallbackProc)help_view_cb, NULL);

	ManageChildren(d->panel_form);

}

/*
 * This redisplays the 3 month panels 2 show the correct 3-month
 * window, centred around the month containing the "current" date.
 */
static void
update_quarter(Calendar *c)
{
	Day *d = (Day *)c->view->day_info;
        int year_num, month_num;

	/* previous month */
	year_num = year(previousmonth(c->view->date));
	month_num = month(previousmonth(c->view->date));
	XtVaSetValues(d->month_panels[0],
		XmNyear, year_num,
		XmNmonth, month_num,
		NULL);
	
	/* current month */
	year_num = year(c->view->date);
	month_num = month(c->view->date);
	XtVaSetValues(d->month_panels[1],
		XmNyear, year_num,
		XmNmonth, month_num,
		NULL);
	
	/* next month */
	year_num = year(nextmonth(c->view->date));
	month_num = month(nextmonth(c->view->date));
	XtVaSetValues(d->month_panels[2],
		XmNyear, year_num,
		XmNmonth, month_num,
		NULL);
	
}

static void
day_btn_cb(Widget w, XtPointer client, XtPointer call)
{
        XmMonthPanelCallbackStruct *cbs = (XmMonthPanelCallbackStruct *) call;
        Calendar *c = (Calendar *)client;
        int monthno, year;
        int date = c->view->date;
 
	invalidate_cache(c);

        XtVaGetValues(w, XmNmonth, &monthno, XmNyear, &year, NULL);
 
        if (cbs->type == MONTH_SELECTION) {

		XtUnmapWidget(c->canvas);

		/*
		 * Date update:
		 *	if month chosen is not current month
		 *	set date to 1st of new month, otherwise
		 *	don't change date.
		 */
		if (month(c->view->date) != monthno) {
			calendar_deselect (c);
			c->view->olddate = c->view->date;
			c->view->date = monthdayyear(monthno, 1, year);
			c->view->nwks = numwks(c->view->date);
			calendar_select (c, monthSelect, NULL);
		}
 
                /* switch to month view */
                c->view->glance = monthGlance;
                cleanup_after_dayview(c);
                prepare_to_paint_monthview(c, NULL);

		XtMapWidget(c->canvas);
        }
        else {	/* type == DAY_SELECTION */
                c->view->olddate = c->view->date;
                c->view->date = monthdayyear(monthno, cbs->day, year);
		gr_clear_area(c->xcontext, 0, 0, c->view->winw, c->view->winh);
		paint_dayview(c, True, NULL, False); 
		calendar_select(c, hourSelect, (caddr_t)NULL);
        }
}
