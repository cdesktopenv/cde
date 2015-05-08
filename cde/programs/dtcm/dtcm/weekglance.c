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
/* $XConsortium: weekglance.c /main/13 1996/11/21 19:43:24 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#include <EUSCompat.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/param.h> /* MAXPATHLEN defined here */
#ifdef SVR4
#include <sys/utsname.h> /* SYS_NMLN */
#endif /* SVR4 */
#include <dirent.h>
#include <ctype.h>
#include <string.h>
#include <sys/time.h>
#if defined(sun) && defined(_XOPEN_SOURCE)
#include <time.h>
#endif
#include <rpc/rpc.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <X11/Xlib.h>
#include <Xm/Xm.h>
#include <Xm/PushB.h>
#include <Xm/Text.h>
#include <Xm/ToggleBG.h>
#include <Dt/HourGlass.h>
#include "calendar.h"
#include "util.h"
#include "timeops.h"
#include "format.h"
#include "datefield.h"
#include "x_graphics.h"
#include "props.h"
#include "select.h"
#include "editor.h"
#include "group_editor.h"
#include "browser.h"
#include "blist.h"
#include "dayglance.h"
#include "monthglance.h"
#include "yearglance.h"
#include "weekglance.h"
#include "todo.h"
#include "find.h"
#include "goto.h"
#include "tempbr.h"

#define XOS_USE_XT_LOCKING
#define X_INCLUDE_TIME_H
#if defined(linux)
#undef SVR4
#endif
#include <X11/Xos_r.h>

static int week_xytoclock(Week *w, int x, int y);
static int week_xytohour(Week *w, int x, int y);
static void fill_day();
static void draw_week();
static void draw_chart();
static void quick_button_cb(Widget, XtPointer, XtPointer);
static void display_hot_btn(Calendar *, int, int);
static void clear_hot_btn(Calendar *, int);
static void allocator(Calendar *);
static void deallocator(Calendar *);
static Boolean print_week (Calendar *c,
			   int num_page, 
			   void *xp,
			   Tick first_date, 
			   Props *p, 
			   Boolean first);


#define inchart(w, x, y) \
                ((x >= w->chart_x && x <= (w->chart_x + w->chart_width) && \
                y >= w->chart_y - w->label_height && \
                y <= (w->chart_y + w->chart_height-1)))

#define inweek(w, x, y) \
                ((x >= w->x && x <= w->x + w->width && \
                y >= w->y && y <= w->y + w->day_height) || \
                (x >= w->x + 3 * w->day_width && x <= w->x + w->width && \
                y >= w->y + w->day_height && y <= w->y + w->height))

extern void
format_week_header(Tick date, OrderingType order, char *buf)
{
        Calendar *c = calendar;
	struct tm *tm;
	_Xltimeparams localtime_buf;

	tm = _XLocaltime(&date, localtime_buf);

	/* NL_COMMENT
	   Attention Translator:

	   This string is used in the calendar week view.  In the C locale
	   it has the form: 

			Monday, January 16, 1995

	   strftime conversion string: "%A, %B %e, %Y" is used.  The string
	   will be used in a label that looks like this:

			Week Starting Monday, January 16, 1995

	   Use the appropriate strftime conversion for your locale.
	*/
	strftime(buf, 80, 
		catgets(c->DT_catd, 1, 993, "Week Starting %A, %B %e, %Y"), tm);
}
 
static int
count_week_pages (Calendar *c, int lines_per_page, Tick start_date)
{
	time_t start, stop;
        CSA_return_code stat;
        CSA_entry_handle *list;
        CSA_attribute *range_attrs;
	CSA_enum *ops;
	CSA_uint32 a_total;
        int num_appts, i, j, max = 0, pages;

        /* count the times and text of appts */
        for (i = 1; i <= 7; i++)
        {
                /* setup a time limit for appts searched */
                start = (time_t) lowerbound (start_date);
                stop = (time_t) next_ndays(start_date, 1) - 1;
		setup_range(&range_attrs, &ops, &j, start, stop,
			    CSA_TYPE_EVENT, 0, B_FALSE, c->general->version);
		csa_list_entries(c->cal_handle, j, range_attrs, ops, &a_total, &list, NULL);
		free_range(&range_attrs, &ops, j);

                num_appts = count_multi_appts(list, a_total, c);
                if (num_appts > max)
                        max = num_appts;
 
                start_date = nextday(start_date);
                csa_free(list);
        }
 
        pages = max / lines_per_page;
        if ((max % lines_per_page) > 0)
                pages++;

        return(pages);
}

static Boolean
print_week (Calendar *c, 
	int num_page, 
	void *xp,
	Tick first_date, 
	Props *p, 
	Boolean first)
{
        Boolean more, done = False, all_done = True;
        int num_appts, day_of_week;
        char    buf[128];
        int     i, j;
	OrderingType ot = get_int_prop(p, CP_DATEORDERING);
	time_t start, stop;
        CSA_return_code stat;
        CSA_entry_handle *list;
        CSA_attribute *range_attrs;
	CSA_enum *ops;
        CSA_uint32 a_total;
	int lines_per_page;
        static Tick start_date = 0;
	static int total_pages;
 
        static char *days[] = {
                (char *)NULL, (char *)NULL, (char *)NULL,
                (char *)NULL, (char *)NULL, (char *)NULL, (char *)NULL
        };

	if (days[0] == (char *)NULL)
	{
	  days[0] = XtNewString(catgets(c->DT_catd, 1, 596, "Monday %d"));
	  days[1] = XtNewString(catgets(c->DT_catd, 1, 597, "Tuesday %d"));
	  days[2] = XtNewString(catgets(c->DT_catd, 1, 598, "Wednesday %d"));
	  days[3] = XtNewString(catgets(c->DT_catd, 1, 599, "Thursday %d"));
	  days[4] = XtNewString(catgets(c->DT_catd, 1, 600, "Friday %d"));
	  days[5] = XtNewString(catgets(c->DT_catd, 1, 601, "Saturday %d"));
	  days[6] = XtNewString(catgets(c->DT_catd, 1, 602, "Sunday %d"));
	}

	x_init_printer(xp, LANDSCAPE);
	x_init_week(xp);
	lines_per_page = x_get_week_lines_per_page(xp);

	if (first)
	  start_date = first_date;

	if (num_page > 1)
	{
	  start_date = prevweek(start_date);
	  if (!timeok(start_date))
	    start_date = get_bot();
	}
	else
	  total_pages = (lines_per_page > 0) ?
	    count_week_pages(c, lines_per_page, start_date) : 1;

	format_week_header(start_date, ot, buf);

	x_print_header(xp, buf, num_page, total_pages);
	x_week_appt_boxes(xp);
	x_week_sched_boxes(xp);

	/* print the times and text of appts */
	for (i = (dow(start_date) + 6) % 7; i < 7; i++)
	{
	  /* print <Weekday DD> centered at top of appt box */
	  x_week_sched_init(xp);

	  sprintf(buf, days[i], dom(start_date));

	  /* setup a time limit for appts searched */
	  start = (time_t) lowerbound (start_date);
	  stop = (time_t) next_ndays(start_date, 1) - 1;
	  setup_range(&range_attrs, &ops, &j, start, stop,
		      CSA_TYPE_EVENT, 0, B_FALSE, c->general->version);
	  csa_list_entries(c->cal_handle, j, range_attrs,
			   ops, &a_total, &list, NULL);
	  free_range(&range_attrs, &ops, j);

	  num_appts = count_multi_appts(list, a_total, c);

	  if ((lines_per_page > 0) &&
	      (num_appts > (lines_per_page * num_page)))
	    more = True;
	  else
	    more = False;

	  x_week_daynames(xp, buf, i, more);

	  /* print out times and appts */
	  if (lines_per_page > 0)
	    done = x_print_multi_appts(xp, list, a_total,
				       num_page, weekGlance);
	  else done = True;

	  if (!done)
	    all_done = False;

	  x_week_sched_draw(xp, i);

	  start_date = nextday(start_date);
	  csa_free(list);
	}

	x_finish_printer(xp);

        return(all_done);
}

extern void
print_week_range(Calendar * c, Tick start_tick, Tick end_tick)
{

        Props 		*p = (Props *)c->properties;
        register Tick 	first_date = start_tick;
        int 		num_weeks;
        Boolean 	done = False, first = True;
        int 		num_page = 1;
	void *xp = (void *)NULL;

        /* get number of weeks needed to print */

	num_weeks = ((end_tick - start_tick)/wksec) + 1;

        if (num_weeks <= 0)
                num_weeks = 1;

	first_date = first_dow(first_date);
	if (!timeok(first_date))
	  first_date = get_bot();

	if ((xp = x_open_file(c)) == (void *)NULL)
	  return;

        for (; num_weeks > 0; num_weeks--) {
	  while (!done) {
	    done = print_week(c, num_page, xp, first_date, p, first);
	    num_page++;
	    first = False;
	  }
	  done = False;
	  num_page = 1;
        }

	x_print_file(xp, c);
}

extern int
count_multi_appts(CSA_entry_handle *list, int num_entries, Calendar *c)
{
  CSA_return_code stat;
  Dtcm_appointment *appt;
  int count = 0, i, meoval;
  Props *pr = (Props*)c->properties;
  Lines *lines, *l_ptr;
 
  meoval = get_int_prop(pr, CP_PRINTPRIVACY);

  appt = allocate_appt_struct(appt_read,
			      c->general->version,
			      CSA_ENTRY_ATTR_CLASSIFICATION_I,
			      CSA_X_DT_ENTRY_ATTR_SHOWTIME_I,
			      CSA_ENTRY_ATTR_SUMMARY_I,
			      NULL);
  for (i = 0; i < num_entries; i++) {

    stat = query_appt_struct(c->cal_handle, list[i], appt);

    if (stat != CSA_SUCCESS) {
      free_appt_struct(&appt);
      return 0;
    }

    if ((privacy_set(appt) == CSA_CLASS_PUBLIC) && !(meoval & PRINT_PUBLIC))
      continue;
    else if ((privacy_set(appt) == CSA_CLASS_CONFIDENTIAL) &&
	     !(meoval & PRINT_SEMIPRIVATE))
      continue;
    else if ((privacy_set(appt) == CSA_CLASS_PRIVATE) &&
	     !(meoval & PRINT_PRIVATE))
      continue;

    if (showtime_set(appt)) {
      count++;
    }
    l_ptr = lines = text_to_lines(appt->what->value->item.string_value, 10);
    while (lines != NULL) {
      count++;
      lines = lines->next;
    }
    destroy_lines(l_ptr);
  }        
  free_appt_struct(&appt);
  return(count);
}        
 
/*
 * handler for week view menu item.
 */
void
week_button (Widget widget, XtPointer data, XtPointer cbs)
{
        Calendar *c = calendar;

	if (c->view->glance == weekGlance)
		return;

	switch (c->view->glance) {
		case dayGlance:
			c->view->glance = weekGlance;
			cleanup_after_dayview(c);
        		paint_weekview(c, NULL);
			break;
		case yearGlance:
			c->view->glance = weekGlance;
			cleanup_after_yearview(c);
			XtMapWidget(c->canvas);
			break;
		case monthGlance:
			c->view->glance = weekGlance;
			cleanup_after_monthview(c);
			break;
		default:
			break;
	}
}

/*
 * Clean up anything left around that won't be removed on
 * switching to another view, such as the hot buttons for
 * navigation to day view.
 */
extern void
cleanup_after_weekview(Calendar *c)
{
	int n;
        Week *w = (Week *)c->view->week_info;

	invalidate_cache(c);
	set_message(c->message_text, "");

	XtUnmanageChildren(w->hot_button, 7);

	XmToggleButtonGadgetSetState(c->week_scope, False, False);

	/*
	 * Deallocating everything causes all this view's memory to be
	 * freed (at least all we have allocated in this application)
	 * at the expense of rebuilding the view next time it is used.
	 * Commenting it out improves usability and avoids any risk
	 * of memory leakage from Xt and Motif.
	 */
/*
	deallocator(c);
*/
}

/*
 * allocate storage & subwidgets used by week view
 */
static void 
allocator(Calendar *c)
{
	int n;
        Week *w;
	Arg args[1];

	/* main storage for other week data */
	calendar->view->week_info = (caddr_t) ckalloc(sizeof(Week));
	w = (Week *)c->view->week_info;

	/* navigation buttons to day view */
	w->hot_button = (Widget *) ckalloc(7 * sizeof(Widget));

	/* Set the recomputeSize of the PushButtons to False, to prevent a loop 
	   where a SetValues is done on the PushButton's width from the 
	   exposeCallback of the DrawingArea */
	XtSetArg(args[0], XmNrecomputeSize, False); 	

	for (n=0; n<7; n++) {
		w->hot_button[n] =
			XmCreatePushButton(c->canvas, "week2day", args, 1);
		XtAddCallback(w->hot_button[n],XmNactivateCallback, 
			quick_button_cb, (XtPointer) (intptr_t) n);
	}

	/* selection info (and init its permanent attributes) */
	w->current_selection = ckalloc(sizeof(Selection));
	((Selection*)w->current_selection)->row = 0;
	((Selection*)w->current_selection)->nunits = 1;
}

/*
 * allocate storage & subwidgets used by week view
 */
static void 
deallocator(Calendar *c)
{
        Week *w = (Week *)c->view->week_info;
	int n;

	/* hot buttons */
	for (n=0; n<7; n++)
		XtDestroyWidget(w->hot_button[n]);

	/* array that held navigation buttons */
	free(w->hot_button);

	/* selection info */
	free(w->current_selection);

	/* allocated in init_week */
        if (w->time_array != NULL)
                free(w->time_array);

	/* structure holding week information */
	free(w);
	c->view->week_info = NULL;
}

/*
 * Set up data needed to draw this particular week
 */
static void
init_week(Calendar *c, Boundary *boundary)
{
        Week *w = (Week *)c->view->week_info;
        int     char_width, char_height;
        Props   *p;
        int     num_hrs,        day_of_week;
        int     empty_space, day_box;
	int	skip_days = 0;
	XFontSetExtents regfontextents, boldfontextents;

	*boundary = okay;

        /*
         * The week view starts on Monday.  Map Sunday to the last day of the
         * week
         */
        if ((day_of_week = dow(c->view->date)) == 0)
                day_of_week = 6;
        else
                day_of_week--;

        ((Selection*)w->current_selection)->col = day_of_week;

        w->start_date = lowerbound(c->view->date - (day_of_week * daysec));
	/* make sure date is within bounds */
	if (w->start_date == -1) {
		if (year(c->view->date) == year(get_bot())) {
			w->start_date = get_bot();
			*boundary = lower;
		}
	}
	else if (year(next_ndays(w->start_date, 7)) > year(get_eot())) {
			*boundary = upper;
	}

        /*
         * Set up a bunch of variables which are needed to draw and fill
         * the week at a glance screen
         */
	XtVaGetValues(c->canvas, 
			XmNwidth, &w->canvas_w, 
			XmNheight, &w->canvas_h, 
			NULL);

        w->font = c->fonts->labelfont;
        w->small_font = c->fonts->viewfont;
        w->small_bold_font = c->fonts->boldfont;
	CalFontExtents(w->font, &regfontextents);
	CalFontExtents(w->small_bold_font, &boldfontextents);

        w->x = c->view->outside_margin;
        w->y = 2 * (int) boldfontextents.max_logical_extent.height;

	char_height = regfontextents.max_logical_extent.height;
        char_width = regfontextents.max_logical_extent.width;
        w->label_height = char_height * 2;
        w->day_width = ((int) (w->canvas_w - 2 * w->x)) / 5;
        /* height of box with label */
        w->day_height = ((int) (w->canvas_h - 2 * w->y)) / 2;
        /*
         * We compute week dimensions from day dimensions to remove rounding
         * errors
         */
        w->width = w->day_width * 5;
        /* height from top of box to bottom of weekend boxes */
        w->height = w->day_height * 2;
 
        p = (Props *)c->properties;
        w->begin_hour = get_int_prop(p, CP_DAYBEGIN);
        w->end_hour = get_int_prop(p, CP_DAYEND);
 
        /* width of a column in chart */
        w->chart_day_width = (3 * w->day_width - 3 * char_width) / 7;
        /* width of chart */
        w->chart_width = w->chart_day_width * 7;
        num_hrs = w->end_hour - w->begin_hour;
 
        /* height of box without label */
        day_box = w->day_height - w->label_height;
 
        /* height of an hour in chart */
        w->chart_hour_height = day_box / num_hrs;
        /* chart_hour_height must be evenly divisble by BOX_SEG */
        w->chart_hour_height -= (w->chart_hour_height % BOX_SEG);
        w->chart_height = w->chart_hour_height * num_hrs;
 
        /* x point of upper left corner of chart */
        w->chart_x = w->x + 2 * boldfontextents.max_logical_extent.width;
        /* y point of upper left corner of chart */
        w->chart_y = w->y + w->height - w->chart_height;
 
        /* left over empty space above chart after round off error */
        empty_space = day_box - w->chart_height;
        /* add pixels to the height of each hour box in chart to fill gap*/
        if (w->add_pixels = ((double)empty_space / (double)num_hrs)) {
                w->chart_y -= w->add_pixels * num_hrs;
                w->chart_height += w->add_pixels * num_hrs;
        }
 
        w->segs_in_array = BOX_SEG * num_hrs * 7;
        if (w->time_array != NULL)
                free(w->time_array);
        w->time_array = (char*)ckalloc(w->segs_in_array);
 
        c->view->outside_margin = w->x;
        c->view->topoffset = w->y;
}

extern void
paint_weekview(
	Calendar 	*c, 
	XRectangle 	*rect)
{
	Boundary 	 boundary;

	XmToggleButtonGadgetSetState(c->week_scope, True, False);
        c->view->glance = weekGlance;

	/* allocate weekview storage if it's never been used before */
	if (c->view->week_info == NULL) {
		allocator(c);
		resize_weekview(c, &boundary);
	} else 
        	init_week(c, &boundary);

        draw_week(c, rect, boundary);
        calendar_select(c, weekdaySelect, NULL);
}

static void
cm_update_segs(Week *w, 
	Tick tick, 
	Tick dur, 
	int *start_index, 
	int *end_index, 
	Boolean addto)
{
        int     num_segs, i, start, start_hour, duration, nday;
 
        start_hour = hour(tick);
       
        if (start_hour >= w->end_hour) {
                *start_index = -1;
                *end_index = -1;
                return;
        }
 
        if (start_hour < w->begin_hour) {
                start = 0;
                duration = dur - ((w->begin_hour -
                 (start_hour + (double)minute(tick)/(double)60))
                        * hrsec);
        } else{
                start = ((start_hour - w->begin_hour) * 60 + minute(tick));
                duration = dur;
        }
 
        if (duration <= 0) {
                *start_index = -1;
                *end_index = -1;
                return;
        }

        nday = (nday=dow(tick))==0? 6: nday-1;
        num_segs = (double)start / (double)MINS_IN_SEG;
        *start_index = (double)start / (double)MINS_IN_SEG + (nday * (w->segs_in_array/7));
        if (start - (num_segs * MINS_IN_SEG) > 7)
                (*start_index)++;
        num_segs = ((double)duration / (double)60 / (double)MINS_IN_SEG);
        *end_index = num_segs + *start_index;
        if (((double)duration/(double)60-MINS_IN_SEG*num_segs) > 7)
                (*end_index)++;

        if (*end_index > (i = ((nday + 1) * (w->segs_in_array / 7))) )
                *end_index = i;

        for (i = *start_index; i < *end_index; i++)
                if (addto)
                        w->time_array[i]++;
                else      
                        w->time_array[i]--;
}        

static void
add_extra_pixels(int i, int num_pixels, int *h)
{
        if (((i+1) % BOX_SEG) == 0)
                *h += num_pixels;
}

static void
chart_draw_appts(Week *w, int start, int end)
{
        int x, y, h, i, segs_in_col, no_boxes;
        Calendar *c = calendar;
        int col_remainder, boxseg_h;
	Colormap cms;

        XtVaGetValues(c->canvas, XmNcolormap, &cms, NULL);
        h = boxseg_h = (double)w->chart_hour_height / (double)BOX_SEG;
        segs_in_col = (w->segs_in_array / 7);
        /* find starting segment in column */
        col_remainder =  (start % segs_in_col);

        no_boxes = (double)(col_remainder+1) / (double)BOX_SEG;
        y = w->chart_y + ((double)col_remainder * (double)boxseg_h) +
                        ((double)w->add_pixels * (double)no_boxes) + 1;
        x = w->chart_x + ((double)start / (double)segs_in_col *
                        (double)w->chart_day_width);
 
        for (i = start; i < end; i++) {
                if (w->time_array[i] == 0) {
                        /* batch up repaints */
                        if ( (i+1) < w->segs_in_array &&
                                w->time_array[i+1] == 0 &&
                                ((i+1) % segs_in_col) != 0 ) {
                                h += boxseg_h;
                                add_extra_pixels(i, w->add_pixels, &h);
                                continue;
                        }
                        add_extra_pixels(i, w->add_pixels, &h);
                        gr_clear_area(c->xcontext, x, y, w->chart_day_width, h);
                }
                else if (w->time_array[i] == 1) {
                        /* batch up for one repaint */
                        if ( (i+1) < w->segs_in_array
                                 && w->time_array[i+1] == 1 &&
                                 ((i+1) % segs_in_col) != 0 ) {
                                h += boxseg_h;
                                add_extra_pixels(i, w->add_pixels, &h);
                                continue;
                        }
                        add_extra_pixels(i, w->add_pixels, &h);
                        if ((c->xcontext->screen_depth < 8) || FAKE_MONOCHROME)
                                gr_make_gray(c->xcontext, x, y,
                                        w->chart_day_width, h, 25);
                        else
                                gr_make_grayshade(c->xcontext, x, y,
                                        w->chart_day_width, h, LIGHTGREY);
                }
                else if (w->time_array[i] == 2) {
                        /* batch up for one repaint */
                        if ( (i+1) < w->segs_in_array
                                 && w->time_array[i+1] == 2 &&
                                 ((i+1) % segs_in_col) != 0 ) {
                                h += boxseg_h;
                                add_extra_pixels(i, w->add_pixels, &h);
                                continue;
                        }
                        add_extra_pixels(i, w->add_pixels, &h);
                        if ((c->xcontext->screen_depth < 8) || FAKE_MONOCHROME)
                                gr_make_gray(c->xcontext, x, y,
                                        w->chart_day_width, h, 50);
                        else
                                gr_make_rgbcolor(c->xcontext, cms, x, y,
                                        w->chart_day_width, h,
                                        MIDGREY, MIDGREY, MIDGREY);
                }
                else if (w->time_array[i] >= 3) {
                        /* batch up for one repaint */
                        if ( (i+1) < w->segs_in_array
                                && w->time_array[i+1] >= 3 &&
                                ((i+1) % segs_in_col) != 0 ) {
                                h += boxseg_h;
                                add_extra_pixels(i, w->add_pixels, &h);
                                continue;
                        }
                        add_extra_pixels(i, w->add_pixels, &h);
                        if ((c->xcontext->screen_depth < 8) || FAKE_MONOCHROME)
                                gr_make_gray(c->xcontext, x, y,
                                        w->chart_day_width, h, 75);
                        else
                                gr_make_grayshade(c->xcontext, x, y,
                                        w->chart_day_width, h, DIMGREY);
                }
                if (i != 0 && (((i+1) % segs_in_col) == 0)) {
                        x += w->chart_day_width;
                        y = w->chart_y + 1;
                        h = ((double)w->chart_hour_height/(double)BOX_SEG);
                }
                else {
                        y += h;
                        h = boxseg_h;
                }
        }
}

extern void
resize_weekview(
	Calendar	*c,
	Boundary	*boundary)
{
        register int     x, y;
	register int	 n;
	Week 		*w = (Week *)c->view->week_info;

	/* allocate weekview storage if it's never been used before */
	if (c->view->week_info == NULL) {
		allocator(c);
		w = (Week *)c->view->week_info;
	}

        init_week(c, boundary);

	y = w->y;
	x = w->x;

        for (n = 0; n < 7; n++)  {

                if (n == 5) {
                        y += w->day_height + 1;
                        x = w->x + 3 * w->day_width;
                }

		XtVaSetValues(w->hot_button[n],
				XmNx,		x + 2,
				XmNy,		y + 1,
				XmNwidth,	w->day_width - 3,
				XmNheight,	w->label_height - 2,
				NULL);
                x += w->day_width;
        }
}

static void
draw_week(Calendar *c, XRectangle *rect, Boundary boundary)
{
        Week *w = (Week *)c->view->week_info;
        register int    current_day;
        register int    n;
        register int    x, y;
        int             char_height;
        int             start_date;
        char            **day_names;
        char            label[80];
	char		buf[MAXNAMELEN];
	char		*footer_message = NULL;
        int             start_ind, end_ind;
        int             today_dom, day_om;
        new_XContext        *xc;
        Props           *p = (Props*)c->properties;
        XRectangle      chartrect;
	OrderingType	ot = get_int_prop(p, CP_DATEORDERING);
	Tick		start_tick, end_tick;
	time_t start, stop;
        CSA_return_code stat;
        CSA_entry_handle *list;
        CSA_attribute *range_attrs;
	CSA_enum *ops;
	CSA_uint32 a_total;
        int i, lower_bound = 0, upper_bound = 0;
	XFontSetExtents regfontextents, boldfontextents;
	int	notused, width1, width2, width3;
        
	CalFontExtents(w->font, &regfontextents);
	char_height = regfontextents.max_logical_extent.height;
        start_date      = w->start_date;
        xc              = c->xcontext;

       	start = (time_t) lowerbound(start_date);
       	stop = (time_t) next_ndays(start, 7) - 1;

        if (c->paint_cache == NULL) {
		setup_range(&range_attrs, &ops, &i, start, stop, CSA_TYPE_EVENT,
	    		0, B_FALSE, c->general->version);
		csa_list_entries(c->cal_handle, i, range_attrs, ops, 
				 &a_total, &list, NULL);
		free_range(&range_attrs, &ops, i);
                allocate_paint_cache(list, a_total, &c->paint_cache);
                c->paint_cache_size = a_total;
		csa_free(list);
        }


        gr_clear_box(xc, 0, 0, w->canvas_w, w->canvas_h);
 
	CalTextExtents(w->font, days2[3], cm_strlen(days2[3]), 
		       &notused, &notused, &width1, &notused);
	CalTextExtents(w->font, "  00", cm_strlen("  00"),
		       &notused, &notused, &width2, &notused);
	CalTextExtents(w->font, "Wed 00", cm_strlen("Wed 00"), 
		       &notused, &notused, &width3, &notused);
        if (width1 + width2 <= w->day_width - 2)
                day_names = days2;
        else if (width3 <= w->day_width - 2)
                day_names = days;
        else
                day_names = days3;
        x = w->x;
        y = w->y;
        
        format_week_header(start_date, ot, label);
        gr_text(xc, x, y - char_height / 2, w->font, label, rect);
 
        /*
         * Draw bold box around first 5 days
         */
        gr_draw_box(xc, x, y, w->width, w->day_height, rect);
        gr_draw_box(xc, x - 1, y - 1, w->width + 2, w->day_height + 2, rect);
        gr_draw_line(xc, x, y + w->label_height, x + w->width,
                y + w->label_height, gr_solid, rect);
 
        /*
         * Draw bold box around last 2 days
         */
        x += 3 * w->day_width;
        y += w->day_height;
 
        gr_draw_box(xc, x, y, 2 * w->day_width, w->day_height, rect);
        gr_draw_box(xc, x - 1, y, 2 * w->day_width + 2, w->day_height + 1,rect);
        gr_draw_line(xc, x, y + w->label_height, x + 2 * w->day_width, 
		     y + w->label_height, gr_solid, rect);
        y = w->y;
        x = w->x + w->day_width;
        for (n = 0; n < 4; n++) {
                if (n < 3) {
                        gr_draw_line(xc, x, y, x, y + w->day_height, 
				     gr_solid, rect);
                } else {
                        gr_draw_line(xc, x, y, x, y + 2 * w->day_height,
				     gr_solid, rect);
                }
                x += w->day_width;
        }
       /*
         * Fill in week with appointments
         */
        x = w->x;
        y = w->y;
        current_day = start_date;
        today_dom = dom(time(0));
 
	/* Crock alert!!!!  The obscure code below is doing something 
	   really nasty.  The variable boundary indicates whether the 
	   week being displayed falls across a boundary with the 
	   beginning or the end of time.  In the case of the beginning 
	   of time, the code then assumes that the first 2 days in the 
	   week need to be unbuttoned, and the rest buttoned and painted.  
	   Likewise, with the end of time case, the code assumes the last 
	   3 days of the week need similar treatment. */

        for (n = 0; n < 7; n++)  {

                if (n == 5) {
                        y += w->day_height;
                        x = w->x + 3 * w->day_width;
                }
 
		if (boundary == okay) {
                	day_om = dom(current_day);
			display_hot_btn(c, n, day_om);
                	fill_day(c, w, x, y, current_day, 
				 c->paint_cache, c->paint_cache_size, rect);
                	current_day += daysec;
			if (lower_bound > 0) {
				sprintf(buf, "%s", catgets(c->DT_catd, 1, 623, "Calendar does not display dates prior to January 1, 1970"));
				footer_message = buf;
			}
			else
				footer_message = NULL;
		}
		else if (boundary == lower) {
			/* skip days before Jan 1, 1970 */
			clear_hot_btn(c, n);
			if (lower_bound++ == 2)
				boundary = okay;
		}
		else if (boundary == upper) {
                	day_om = dom(current_day);
			if (++upper_bound <= 4)
				display_hot_btn(c, n, day_om);
                	fill_day(c, w, x, y, current_day, 
				 c->paint_cache, c->paint_cache_size, rect);
                	current_day += daysec;
			sprintf(buf, "%s", catgets(c->DT_catd, 1, 624, "Calendar does not display dates after December 31, 2037"));
			footer_message = buf; 
			if (upper_bound > 4)
				clear_hot_btn(c, n); 
		}
                x += w->day_width;
        }
        if (rect != NULL) {
		CalFontExtents(w->small_bold_font, &boldfontextents);

                chartrect.x = w->x;
                chartrect.y = w->chart_y - w->label_height;
                chartrect.width = w->chart_width +
                        3*boldfontextents.max_logical_extent.width;
                chartrect.height = w->chart_height + 2*w->label_height;
	}

       if (rect == NULL || myrect_intersectsrect(rect,  &chartrect)) {
		for (i = 0; i < c->paint_cache_size; i++) {
			start_tick = (c->paint_cache)[i].start_time;
			end_tick = (c->paint_cache)[i].end_time;
                        cm_update_segs(w, start_tick,
				(end_tick ? (end_tick - start_tick) : 0),
                                &start_ind, &end_ind, True);
		}
                chart_draw_appts(w, 0, w->segs_in_array);
                draw_chart(c, w, NULL);
        }

	/* do not repaint the footer message in a damage display.  
	   For some reason this causes the damage routine to get called 
	   again, resulting in a recursive dsaster. */

	if (footer_message && !rect)
		set_message(c->message_text, footer_message);
}
/*
 *  Format 2 lines of appt data
 */
static void
format_entry(Paint_cache *cache_entry, char *buf1, char *buf2,
	     DisplayType display) {
        Tick    tick, end_tick = 0;
        int     hour1, min1, hour2, min2;
        Lines   *lines;
        char    *s1, *s2;
        struct tm *tm;
	_Xltimeparams localtime_buf;


	tick = cache_entry->start_time;
	end_tick = cache_entry->end_time;
 
        /*
         * Extract an appointment and format it into 2 lines of no more
         * then maxchars
         */
        *buf1 = *buf2 = '\0';
        if (cache_entry == NULL || cache_entry->summary == NULL) return;
        tm = _XLocaltime(&tick, localtime_buf);
        hour1 = tm->tm_hour;
        min1  = tm->tm_min;
 
        if (!cache_entry->show_time || magic_time(tick)) {
                lines = (Lines *) text_to_lines(cache_entry->summary, 1); 
                if (lines==NULL) return;
                strncpy(buf2, lines->s, 256);
                destroy_lines(lines);
                return;
        }
 
        s1 = s2 = "am";
        if (display == HOUR12 && !adjust_hour(&hour1))
                s1="pm";
	if (end_tick) {
        	hour2 = hour(end_tick);
        	min2 = minute(end_tick);
        	if (display == HOUR12 && !adjust_hour(&hour2))
                        s2="pm";
	}

        if (end_tick == 0 ||
	    (hour1 == hour2 && min1 == min2 && (strcmp(s1, s2) == 0))) {
                if (display == HOUR24)
                        sprintf(buf1, "%02d%.2d", hour1, min1);
                else
                        sprintf(buf1, "%d:%.2d%s", hour1, min1, s1);
        }
        else {
                if (display == HOUR12)
                        sprintf(buf1, "%d:%.2d%s-%d:%.2d%s", hour1, min1, s1,
                                 hour2, min2, s2);
                else
                        sprintf(buf1, "%02d%02d-%02d%02d", hour1, min1,
                                 hour2, min2);
        }

          
        lines = (Lines *) text_to_lines(cache_entry->summary, 1);
         
        if (lines == NULL || lines->s == NULL ||                        
                (cm_strlen(lines->s) == 1 && lines->s[0] == ' '))
                buf2[0] = '\0';
        else
                sprintf(buf2, " %s", lines->s);
        destroy_lines(lines);
}

static int
paint_entry(Calendar *c, int x, int y, int maxchars, Paint_cache *cache_entry, XRectangle *rect)
{
	XFontSetExtents	fontextents;
        Props *p = (Props*)c->properties;
        int             nlines = 0, dt = get_int_prop(p, CP_DEFAULTDISP);
        new_XContext        *xc = c->xcontext;
        char            buf1[50], buf2[WHAT_LEN+1];
        Week            *w = (Week *)c->view->week_info;
	Tick		tick;
 
        /*
         * Write an appointment entry into a day
         */
 
        if (maxchars >= 40)             /* maxed out possible=40 */
                maxchars = 40;
                 
        buf1[0] = '\0'; buf2[0] = '\0';
 
        format_entry(cache_entry, buf1, buf2, dt);

	tick = cache_entry->start_time;
 
        if (cache_entry->show_time && !magic_time(tick) && (buf1[0] != '\0')) {
                maxchars = gr_nchars(w->day_width - 5, buf1,c->fonts->boldfont); 
                buf1[min(cm_strlen(buf1), maxchars)] = '\0';
                gr_text(xc, x, y, c->fonts->boldfont, buf1, rect);
                nlines++;
		CalFontExtents(c->fonts->boldfont, &fontextents);
		y += fontextents.max_logical_extent.height;;
        }
        if (buf2[0] != '\0') {
                maxchars = gr_nchars(w->day_width - 5, buf2, 
						c->fonts->viewfont);
		buf2[min(cm_strlen(buf2), maxchars)] = '\0';
                gr_text(xc, x, y, c->fonts->viewfont, buf2, rect);
                nlines++;
        }
 
        return(nlines);
}

static void
fill_day(Calendar *c, Week *w, int x, int y, int day, 
		Paint_cache *cache, int a_total, XRectangle *rect)
{
	CSA_return_code stat;
	Dtcm_appointment *appt;
        register int    lower = (int)lowerbound(day);
        register int    upper = (int)next_ndays(day, 1);
        register int    i, loop, n;
        register int    nlines = 0;
	XFontSetExtents fontextents;
        int	char_width;
	int 	char_height;
        int     maxlines;
        int     maxchars;
	Tick	tick;
 
	CalFontExtents(w->small_font, &fontextents);
        char_width = fontextents.max_logical_extent.width;
	char_height = fontextents.max_logical_extent.height;
        maxlines = ((w->day_height - w->label_height) / char_height)- 1;
        maxchars = (w->day_width / char_width);
#if 0
        x += char_width;
#endif
        x += 3;
        y += (w->label_height + char_height);
 
        /*
         * Fill in a day with appointments
         */

	/* loop thru twice, first displaying "no time" appointments, 
	   and then the others. */

	for (loop = 0; loop < 2; loop++) {
		for (i = 0; i < a_total; i++) {
	
			if ((cache[i].start_time < lower) || (cache[i].start_time >= upper))
				continue;
	
			if (cache[i].show_time != loop)
				continue;
	
	                if (nlines < maxlines) {
	                        n = paint_entry(c, x, y, maxchars, &cache[i], rect);
	                        y += n * char_height;
	                        nlines += n;
	                }
	        }        
	}
} 
 
static void
draw_chart(Calendar *c, register Week *w, XRectangle *rect)
{
        register int    x, y;
        int     n;
	XFontSetExtents fontextents;
	int 	char_height;
        char    label[5];
        new_XContext *xc = c->xcontext;
        Props *p = (Props*)c->properties;
	DisplayType dt = get_int_prop(p, CP_DEFAULTDISP);

	CalFontExtents(w->font, &fontextents);
	char_height = fontextents.max_logical_extent.height;
 
        /*
         * Draw chart. We first draw all the lines, then the labels
         * so that Xlib can batch the lines into 1 X request
         */
 
        /* Draw horizontal lines for time */
        x = w->chart_x;
        y = w->chart_y;
        for (n = w->begin_hour; n <= w->end_hour; n++) {
                gr_draw_line(xc, x, y, x + w->chart_width, y, gr_solid, rect);                y += w->chart_hour_height + w->add_pixels;
        }
 
        /* Draw vertical lines for days */
        y = w->chart_y;
        for (n = 0; n < 7; n++) {
                gr_draw_line(xc, w->chart_x + (w->chart_day_width * n),
                        y, w->chart_x + (w->chart_day_width * n),
                        y + w->chart_height, gr_solid, rect);
        }
 
        /*
         * Draw box around the whole thing.
         */
        gr_draw_box(xc, w->chart_x, w->chart_y, w->chart_width,
                    w->chart_height, rect);
        gr_draw_box(xc, w->chart_x - 1, w->chart_y - 1,
                    w->chart_width + 2, w->chart_height + 2, rect);
        /* Label horizontal lines with time of day */
        x = w->chart_x;
        y = w->chart_y;
        for (n = w->begin_hour; n <= w->end_hour; n++) {
                if (dt == HOUR12)
                        sprintf(label, "%2d", n > 12 ? n - 12 : n);
                else
                        sprintf(label, "%2d", n);
                gr_text(xc, w->x - 8, y+3,
                        w->small_bold_font, label, rect);
                y += w->chart_hour_height + w->add_pixels;
        }
 
        /* Label  vertical lines with day labels */
        y = w->chart_y;
        for (n = 0; n < 7; n++) {
                x = gr_center(w->chart_day_width, days3[n+1], w->font);
                gr_text(xc, w->chart_x + (w->chart_day_width * n) + x,
                        y - char_height / 2, w->font, days3[n+1], rect);
        }
 
}

extern void
week_event(XEvent *event)
{
	Calendar *c = calendar;
        Props *p = (Props*)c->properties;
        static int lastdate;
        static XEvent lastevent;
        int x, y, i, j, hr, id;
        Week    *w = (Week *)c->view->week_info;
        Selection *wsel;
        Editor *e = (Editor *)c->editor;
        ToDo *t = (ToDo*)c->todo;
        GEditor *ge = (GEditor*)c->geditor;
        static int lastrow, lastcol;
        int row, col;

	x 	= event->xbutton.x;
	y 	= event->xbutton.y;
        wsel    = (Selection *)w->current_selection;

        switch(event->type) {
        case MotionNotify:
                j = week_xytoclock(w, x, y);
                (col = dow(j)) == 0 ? col = 6 : col--;
                if (inchart(w, x, y))
                        row = (double)(y - w->chart_y) /
                                (double)(w->chart_hour_height+ w->add_pixels);
                else
                        row = wsel->row;
                if (j != lastdate || lastcol != col || lastrow != row) {
                        calendar_deselect(c);
                        wsel->row = row;
                        wsel->col = col;
                        if (j > 0) {
                                c->view->olddate = c->view->date;
                                c->view->date = j;
                                calendar_select(c, weekdaySelect, NULL);
                        }
                }
                lastcol = wsel->col;
                lastrow = wsel->row;
                lastdate = c->view->date;
                break;
     case ButtonPress:
		j = week_xytoclock(w, x, y);
		if (j == -1)
			return;
		hr = (inchart(w, x, y)) ? week_xytohour(w, x, y) : (wsel->row + w->begin_hour);

		if (ds_is_double_click(&lastevent, event)) {
			_DtTurnOnHourGlass(c->frame);
			if (j == lastdate) {
				show_editor(c, next_nhours(j, hr), next_nhours(j, hr + 1), False);
			}
			else if (editor_showing(e)) {
				set_editor_defaults(e, next_nhours(j, hr), next_nhours(j, hr + 1), False);
				add_all_appt(e);
			}
			_DtTurnOffHourGlass(c->frame);
		}
		else {
			calendar_deselect(c);
			(wsel->col = dow(j)) == 0 ?
				wsel->col = 6 : wsel->col--;
			if (inchart(w, x, y))
				wsel->row = (double)(y - w->chart_y) /
				(double)(w->chart_hour_height + w->add_pixels);
			if (j > 0) {
				c->view->olddate = c->view->date;
				c->view->date = j;
				calendar_select(c, weekdaySelect, NULL);
			}
			if (editor_showing(e)) {
				set_editor_defaults(e, next_nhours(j, hr), next_nhours(j, hr + 1), False);
				add_all_appt(e);
			}
			if (todo_showing(t)) {
				set_todo_defaults(t);
				add_all_todo(t);
			}
			if (geditor_showing(ge)) {
				set_geditor_defaults(ge, 0, 0);
				add_all_gappt(ge);
			}
		}
		lastdate = c->view->date;
		lastcol = wsel->col;
		lastrow = wsel->row;
                break;
        default: 
                break;
        };             /* switch */
        lastevent = *event;
}

static int
week_xytohour(Week *w, int x, int y)
{
        if (!inchart(w, x, y))
                return(-1);
        y -= w->chart_y;
        return(w->begin_hour + ((double)y /
                (double)(w->chart_hour_height + w->add_pixels)));
}

static int
week_xytoclock(Week *w, int x, int y)
{
        int     dow;

        /*
         * Convert the x and y location on the week view to a date
         */
        if (inchart(w, x, y)) {
                dow = (double)(x - w->chart_x)/(double)w->chart_day_width;
        } else if (inweek(w, x, y)) {
                if (y < w->y + w->day_height)
                        dow = (x - w->x)/w->day_width;
                else
                        dow = (x - w->x - 3 * w->day_width)/w->day_width
+ 5;
        } else
                return(0);

	if (w->start_date == get_bot()) {
		if (dow < 3)
			return(-1);
		else
			dow = dow - 3;
	}
	else if (w->start_date == (time_t)last_ndays(get_eot(), 3)) {
		if (dow > 3) {
			return(-1);
		}
	}

        return(w->start_date + dow * daysec);
}


/*
 * Handler for "hot" buttons to navigate to day view
 */
static void
quick_button_cb(Widget widget, XtPointer client, XtPointer call)
{
        Calendar *c = calendar;
        Week    *w = (Week *)c->view->week_info;
	int dow = (int) (intptr_t) client;
	char buf[BUFSIZ];

	if (c->view->date != get_bot()) {
		c->view->olddate = c->view->date;
		c->view->date = w->start_date + dow * daysec;
	}
	calendar_select(c, weekhotboxSelect, NULL);
 
	cleanup_after_weekview(c);

	c->view->glance = dayGlance;
        init_mo(c);
        (void)init_dayview(c);

        paint_day(c);

}

/*
 * Handle labeling, positioning and managing of a given hotbutton
 */
static void
display_hot_btn(
	Calendar 	*c,
	int 		 n,	/* which button (0..6) */
	int 		 date)	/* what day in month */
{
	Week 		*w = (Week *)c->view->week_info;
	Widget 		 btn = w->hot_button[n];
	XmString 	 str;
	char 		 buf[BUFSIZ];

	/* REVISIT: I18N wrap the string for day name */
	sprintf(buf, "%s %d", n == 6 ? days[0] : days[n + 1], date);

	str = XmStringCreateLocalized(buf);
	XtVaSetValues(btn, XmNlabelString, str, NULL);
	XmStringFree(str);

	if (!XtIsManaged(btn)) {
		XtManageChild(btn);
	}
}

/*
 * Clear previous hot buttons because we can't display dates
 *  prior to Jan 1, 1970
 */
static void
clear_hot_btn(Calendar *c,
                int n)                  /* which button (0..6) */
{
        Week *w = (Week *)c->view->week_info;
        Widget btn = w->hot_button[n];
 
        if (XtIsManaged(btn)) {
                XtUnmanageChild(btn);
        }
}

