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
**  calendarA.c
**
**  $TOG: calendarA.c /main/22 1999/09/20 10:29:08 mgreess $
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
static  char sccsid[] = "@(#)calendarA.c 1.196 95/04/12 Copyr 1991 Sun Microsystems, Inc.";
#endif

#include <EUSCompat.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/param.h> /* MAXPATHLEN defined here */
#ifdef SVR4
#include <sys/utsname.h> /* SYS_NMLN */
#endif /* SVR4 specific includes */
#include <locale.h>
#include <nl_types.h>
#include <dirent.h>
#include <ctype.h>
#include <string.h>
#include <sys/time.h>
#include <sys/signal.h>
#include <signal.h>
#include <rpc/rpc.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <X11/Xlib.h>
#include <Xm/Xm.h>
#include <Xm/ArrowBG.h>
#include <Xm/DrawingA.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/LabelG.h>
#include <Xm/List.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/ToggleBG.h>
#include <Xm/RowColumn.h>
#include <Xm/SeparatoG.h>
#include <Xm/Text.h>
#include <Xm/CascadeBG.h>
#include <Xm/CascadeB.h>
#include <Dt/HourGlass.h>
#include <Dt/Dt.h>
#include <Xm/RepType.h>
#include <Xm/Protocols.h>
#include <Dt/EnvControlP.h>
#include <Tt/tt_c.h>
#include <LocaleXlate.h>	/* under DtHelp */
#include "calendar.h"
#include "revision.h"
#include "help.h"
#include "datefield.h"
#include "props.h"
#include "props_pu.h"
#include "misc.h"
#include "deskset.h"
#include "select.h"
#include "session.h"
#include "editor.h"
#include "group_editor.h"
#include "todo.h"
#include "format.h"
#include "getdate.h"
#include "print.h"
#include "util.h"
#include "browser.h"
#include "blist.h"
#include "alarm.h"
#include "dayglance.h"
#include "yearglance.h"
#include "monthglance.h"
#include "weekglance.h"
#include "find.h"
#include "timezone.h"
#include "goto.h"
#include "tempbr.h"
#include "dnd.h"
#include "fallback.h"
#include "rfp.h"
#ifdef FNS
#include "dtfns.h"
#include "cmfns.h"
#include "cmtt.h"
#endif

#if defined(linux)
#undef SVR4
#endif
#include <X11/Xos.h>
#include <errno.h>
#ifdef X_NOT_STDC_ENV
extern int errno;
#endif

#define	ITIMER_NULL ((struct itimerval *) 0)
#define VIEWMARGIN		15
#define NUM_MITEMS_PERCOL 	27
#define ICON_FG			18
#define ICON_BG			14

#if !defined(NL_CAT_LOCALE)
#define NL_CAT_LOCALE       0
#endif


extern time_t timelocal();	/* LINT */
extern char * getlogin();	/* LINT */
extern void(*sigset())();
extern int cmtt_init(char *, Calendar *, XtAppContext, Widget);
extern void show_about(Widget, XtPointer, XtPointer);


static struct itimerval timer;
extern u_long gettransient();
extern void xtcallback(XtPointer, int *, XtInputId *);
extern int getfdset(fd_set);
extern Tick prevnday_exacttime();
extern void quit_handler(Widget, XtPointer, XtPointer);
static void sig_int_handler(int sig);
extern void find_cb(Widget, XtPointer, XtPointer);
extern void goto_cb(Widget, XtPointer, XtPointer);
#ifdef __osf__
extern void sigchld_handler(int);
#endif /* __osf__ */
extern void timezone_cb(Widget, XtPointer, XtPointer);
extern void print_cb(Widget, XtPointer, XtPointer);

extern int cm_register_calendar(const char *username, const char *location);
extern void paint_icon(Calendar *c);
extern void load_app_font(Calendar *, Font_Weight, Cal_Font *, Cal_Font *);
extern void repaint_damaged_month(Calendar *c, XRectangle *rect);

/* routine to get common locale/charset name */
extern void _DtI18NXlateOpToStdLocale(
     		char       *operation,
     		char       *opLocale,
     		char       **ret_stdLocale,
     		char       **ret_stdLang,
     		char       **ret_stdSet);

static void resize_proc(Widget, XtPointer, XtPointer);
static void update_handler (CSA_session_handle, CSA_flags, CSA_buffer, CSA_buffer, CSA_extension *);
static Boolean init_fonts(Calendar *);
static XFontSet get_bold_font(Widget, String);
static void logon_retry(XtPointer data, XtIntervalId *dummy);
static int newXErrorHandler(Display *, XErrorEvent *);

static int (*oldXErrorHandler)(Display *, XErrorEvent *);

void init_strings();

int		child;
int		yyylineno;
int		debug=0;
int		expert=0;

int		cmicon=3;
Calendar        *calendar;

int		gargc;		/* saved for tooltalk initialization */
char		**gargv;

Display *dpy;
XtAppContext    app;

extern void
destroy_paint_cache(Paint_cache *cache, int num_entries) {

	int i;

	if (cache == NULL)
		return;

	for (i = 0; i < num_entries; i++) {
		if (cache[i].summary)
			free(cache[i].summary);
	}

	free(cache);
}

extern void
invalidate_cache(Calendar *c) {

	Browselist      *bl = (Browselist *)c->browselist;
	BlistData       *bd;
	int		i;

	if (c->paint_cache) {
		destroy_paint_cache(c->paint_cache, c->paint_cache_size);
		c->paint_cache = NULL;
		c->paint_cache_size = 0;
	}

	if (bl) {
		i = 1;
		while (bd = (BlistData *)CmDataListGetData(bl->blist_data, i)) {
			if (bd->cal_handle == c->cal_handle) {
                		destroy_paint_cache(bd->cache, bd->cache_size);
                		bd->cache = NULL;
                		bd->cache_size = 0;
				break;
			}
			i++;
        	}

	}
}

CSA_return_code
allocate_paint_cache(CSA_entry_handle *list, int num_entries, Paint_cache **return_cache) {
	Dtcm_appointment 	*appt;
        Calendar 		*c=calendar;
	CSA_return_code 	stat = CSA_SUCCESS;
	Paint_cache	*cache = (Paint_cache *) calloc(sizeof(Paint_cache), num_entries);
	int 			i;


	appt = allocate_appt_struct(appt_read,
				    c->general->version,
				    CSA_ENTRY_ATTR_START_DATE_I,
				    CSA_ENTRY_ATTR_END_DATE_I,
				    CSA_ENTRY_ATTR_SUMMARY_I,
				    CSA_X_DT_ENTRY_ATTR_SHOWTIME_I,
				    NULL);

	/* loop thru the list of appointments twice, first displaying 
	   the no time appointments, and then the appointments with times 
	   associated. */

	for (i = 0; i < num_entries; i++) {
		
		stat = query_appt_struct(c->cal_handle, list[i], appt);
		if (stat != CSA_SUCCESS) {
			destroy_paint_cache(cache, i);
			*return_cache = NULL;
			free_appt_struct(&appt);
			return(stat);
		}
		_csa_iso8601_to_tick(appt->time->value->item.date_time_value,
			&cache[i].start_time);
		if (appt->end_time)
			_csa_iso8601_to_tick(appt->end_time->value->item.\
				date_time_value, &cache[i].end_time);
		else
			cache[i].end_time = 0;

		cache[i].summary = strdup(appt->what->value->item.string_value);
		cache[i].show_time = showtime_set(appt);
	}

	free_appt_struct(&appt);
	*return_cache = cache;
	return(stat);
}

extern void
cache_dims(Calendar *c, Dimension w, Dimension h) 
{
	Glance glance;
	Props *p = (Props*)c->properties;
	int begin = get_int_prop(p, CP_DAYBEGIN);
	int end = get_int_prop(p, CP_DAYEND);
	int margin;
	XFontSetExtents regfontextents, boldfontextents;
        
	CalFontExtents(c->fonts->viewfont, &regfontextents);
	CalFontExtents(c->fonts->boldfont, &boldfontextents);

	c->view->winw = w;
	c->view->winh = h;

	margin = c->view->outside_margin;
	glance = c->view->glance;
	switch(glance) {
		case weekGlance:
			c->view->boxw = (int)(w-2*margin)/5;
			break;
		case monthGlance:
			c->view->boxw = (int)(w-2*margin)/7;
			c->view->topoffset =  boldfontextents.max_logical_extent.height + 30;
			c->view->boxh = (int)(h-c->view->topoffset-
						(margin + boldfontextents.max_logical_extent.height + 6))/c->view->nwks;
			break;
		case yearGlance:
			/* revisit for i18n  - is max_bounds correct? */
			c->view->boxw = 20 * (short)
					regfontextents.max_logical_extent.width
					+ margin;
			c->view->boxh = 7 * (short)
					regfontextents.max_ink_extent.height 
					+ 2 * (short)
					boldfontextents.max_ink_extent.height;
			c->view->topoffset=20;
			break;
		case dayGlance:
			c->view->topoffset = boldfontextents.max_logical_extent.height + 15;
			c->view->boxw = APPT_AREA_WIDTH;
			c->view->boxh = (int)(h - c->view->topoffset)
			 	/ (end - begin + 1); 
			break;
		default:
			break;
	}
}

static void
resize_proc(
	Widget 		 w, 
	XtPointer 	 client, 
	XtPointer 	 call)
{
	Dimension 	 width, 
			 height;
	Calendar 	*c = calendar;
        Glance 		 glance = c->view->glance;

	XtVaGetValues(w, XmNwidth, &width, XmNheight, &height, NULL);
	cache_dims(c, width, height);

	/* If the weekGlance has been initialized then we need to update
	 * its size information reguardless of the current view.
	 */
	if (glance != weekGlance && c->view->week_info != NULL) {
		Boundary	b;
                resize_weekview(c, &b);
	}

        switch(glance) {
        case monthGlance:
		XtUnmapWidget(c->canvas);
                prepare_to_paint_monthview(c, NULL);
		XtMapWidget(c->canvas);
                break;
        case yearGlance:
		return;
        case weekGlance: {
		Boundary	b;
		XtUnmapWidget(c->canvas);
                resize_weekview(c, &b);
		XtMapWidget(c->canvas);
                break;
	}
        case dayGlance:
		/*
		 * If the resize is caused by a Window Manager maximize
		 * or minimize, many of the dayview's lengths need to
		 * be updated.
		 */
		init_dayview (c);
                break;
        default:
                paint_grid(c, NULL);
                break;
        }
}

/*
 * repaint_proc: the XmNexposeCallback handler for the main canvas.
 *
 * Does nothing till the last expose in a possible sequence, then
 * calls the canvas update procedure paint_canvas.
 *
 * This could be optimized to only refresh the damaged areas, by
 * maintaining a smallest-enclosing-rectangle in the Calendar structure
 * until ev.xexpose.count becomes 0, then setting clip accordingly.
 * For now, crude but effective will suffice
 */

static XRectangle new_clip;
static Boolean clip_set = False;

void
repaint_proc(Widget canvas, XtPointer unused, XtPointer data)
{
	Calendar 	*c = calendar;
	XmDrawingAreaCallbackStruct *cbs = (XmDrawingAreaCallbackStruct *) data;
	XEvent *ev = cbs->event;
	int		max_height, old_abs_height, new_abs_height;
	int		max_width, old_abs_width, new_abs_width;
	unsigned long 	valuemask;
	XSetWindowAttributes attrs;
	static short	set_bit_gravity = True;	

	if (set_bit_gravity) {
		set_bit_gravity = False;
		/* 
	 	 * Set the bit gravity on the canvas to ForgetGravity to 
		 * insure we receive an exposure event when the canvas is 
		 * resized smaller.
	 	 */
		valuemask = CWBitGravity;
		attrs.bit_gravity = ForgetGravity;;
		XChangeWindowAttributes(calendar->xcontext->display, 
				XtWindow(calendar->canvas), valuemask, &attrs);
	}

	if (clip_set == True) {
		old_abs_height = new_clip.y + new_clip.height;
		new_abs_height = ev->xexpose.height + ev->xexpose.y;
		max_height = new_abs_height > old_abs_height ? new_abs_height : old_abs_height;
		old_abs_width = new_clip.x + new_clip.width;
		new_abs_width = ev->xexpose.width + ev->xexpose.x;
		max_width = new_abs_width > old_abs_width ? new_abs_width : old_abs_width;
	
        	if (ev->xexpose.x < new_clip.x)
                	new_clip.x = ev->xexpose.x;
        	if (ev->xexpose.y < new_clip.y)
                	new_clip.y = ev->xexpose.y;
	
		new_clip.height = max_height - new_clip.y;
		new_clip.width = max_width - new_clip.x;
	}
	else {
		new_clip.x = ev->xexpose.x;
		new_clip.y = ev->xexpose.y;
		new_clip.height = ev->xexpose.height;
		new_clip.width = ev->xexpose.width;

		clip_set = True;
	}

	if (ev->xexpose.count == 0) {
		XRectangle	clip;
		Dimension	w, h;
		new_XContext 	*xc;

		if ((xc = c->xcontext)==NULL)
			return;

		XtVaGetValues(canvas, XmNwidth, &w, XmNheight, &h, NULL);

                gr_set_clip_rectangles(calendar->xcontext, 0, 0, 
				       &new_clip, 1, Unsorted);

                paint_canvas(c, &new_clip, RENDER_REPAINT);

		clip.x = 0; clip.y = 0; clip.width = w; clip.height = h;
		gr_set_clip_rectangles(calendar->xcontext, 0, 0, &clip, 1, Unsorted);

		
		clip_set = False;

	}

}

static void
show_tempbr(Calendar *c)
{
	Tempbr *t;

	t = (Tempbr*)c->tempbr; 

	if (t == NULL || t->frame == NULL) {
                c->tempbr = (caddr_t)make_tempbr(c);
		t = (Tempbr*)c->tempbr; 
        }
	else {
		XtManageChild(t->form);
		XtPopup(t->frame, XtGrabNone);
	}

        XmProcessTraversal(t->name_text, XmTRAVERSE_CURRENT);
        XtVaSetValues(t->form, XmNinitialFocus, t->name_text, NULL);
}

static void
show_browser(Calendar *c) {
	extern void	set_entry_date(Calendar *);
	Browser		*b;
        Props           *p = (Props*) c->properties;
        OrderingType    ord_t = get_int_prop(p, CP_DATEORDERING);
        SeparatorType   sep_t = get_int_prop(p, CP_DATESEPARATOR);
	char		buf[BUFSIZ];


	if (c->browser == NULL)
                make_browser(c);
	b = (Browser *)c->browser; 


	b->date = c->view->date;
        format_tick(b->date, ord_t, sep_t, buf);
	XmTextSetString(b->datetext, buf);

	set_entry_date(c);

	XtPopup(b->frame, XtGrabNone);
}

static void
show_goto(Calendar *c)
{
	Goto *g;

	g = (Goto*)c->goTo; 

	if (g == NULL || g->frame == NULL) {
                c->goTo = (caddr_t)make_goto(c);
		g = (Goto*)c->goTo; 
        }
	else {
		XtManageChild(g->form);
		XtPopup(g->frame, XtGrabNone);
	}
	
        XmProcessTraversal(g->datetext, XmTRAVERSE_CURRENT);
        XtVaSetValues(g->form, XmNinitialFocus, g->datetext, NULL);

	set_message(g->goto_message, "\0");
}

static void
show_timezone(Calendar *c)
{
	Timezone *t;

	t = (Timezone*)c->timezone; 

	if (t == NULL || t->frame == NULL) {
                c->timezone = (caddr_t)make_timezone(c);
		t = (Timezone*)c->timezone; 
        }
	else {
                refresh_timezone(t);
		XtManageChild(t->form);
		XtPopup(t->frame, XtGrabNone);
	}
}

static void
show_find(Calendar *c)
{
        Find *f;

        f = (Find*)c->find;

        if (f == NULL || f->frame == NULL) {
                c->find = (caddr_t)make_find(c);
                f = (Find*)c->find;
        }
        else {
                XtManageChild(f->form);
                XtPopup(f->frame, XtGrabNone);
        }

        XmProcessTraversal(f->apptstr, XmTRAVERSE_CURRENT);
        XtVaSetValues(f->form, XmNinitialFocus, f->apptstr, NULL);

	set_message(f->find_message, "\0");
}

/* ARGSUSED */
/* revisit - this will have to be hacked for motif
static void
event_proc(Notify_client     canvas,
    Event		  *event,
    Notify_arg        arg,
    Notify_event_type when)
{
	Calendar *c;
	Props *p;
	Glance glance;
	Find *f;

	c = (Calendar *) xv_get(canvas, WIN_CLIENT_DATA); 
	XtVaGetValues(canvas, XmNuserData, c, NULL);
	p = (Props *) c->properties;
	glance = c->view->glance;

	switch(event_action(event)) {
		case ACTION_PROPS:
			(void)p_show_proc(p->frame);
			break;
		case ACTION_SELECT:
		case ACTION_MENU:
		case LOC_DRAG:
			switch(glance) {
				case monthGlance:
					month_event(c, event);
					break;
				case weekGlance:
					week_event(c, event);
					break;
				case dayGlance:
					day_event(c, event);
					break;
				case yearGlance:
					year_event(c, event);
					break;
			}
			c->general->last_canvas_touched = main_win;
			break;
		case ACTION_FIND_FORWARD:
		case ACTION_FIND_BACKWARD:
        		f = (Find*)c->find;
        		if (f == NULL || f->frame == NULL) { 
        	        	c->find = (caddr_t)make_find(c);
                		f = (Find*)c->find;
			}
        		(void)xv_set(f->frame, XV_SHOW, TRUE, NULL);
			XtVaSetValues(bl->frame, XmNmappedWhenManaged, TRUE, NULL);
			break;
		default:
			break;
	}
	
	if (event_action(event) == ACTION_HELP && event_is_up(event))
	{
		switch(glance) {
			case monthGlance:
				xv_help_show(canvas, "cm:MonthView", event);
				break;
			case weekGlance:
				xv_help_show(canvas, "cm:WeekView", event);
				break;
			case dayGlance:
				xv_help_show(canvas, "cm:DayView", event);
				break;
			case yearGlance:
				xv_help_show(canvas, "cm:YearView", event);
				break;
		}
		return(NOTIFY_DONE);
	} else
		return (notify_next_event_func(canvas, (Notify_event)event, arg, when));
}
*/

void
prev_button(Widget w, XtPointer client_data, XtPointer cbs)
{
	Tick date;
	Calendar *c = calendar;
	new_XContext *xc;
	Editor *e = (Editor *) calendar->editor;

	invalidate_cache(c);

	xc = c->xcontext;
	switch(c->view->glance) {
		case weekGlance:
			date = last_ndays(c->view->date, 7);
			if (timeok(date) ||
			    ((date = get_bot()) != c->view->date)) {
				c->view->olddate = c->view->date;
				c->view->date = date;
				c->view->nwks = numwks(c->view->date);
				paint_weekview(c, NULL);
			}
			break;
		case monthGlance:
			date = previousmonth(c->view->date);
			if (timeok(date)) {
				XtUnmapWidget(c->canvas);
				c->view->nwks = numwks(date);
				c->view->olddate = c->view->date;
				c->view->date = date;
				c->view->nwks = numwks(c->view->date);
				(void)prepare_to_paint_monthview(c, NULL);
				XtMapWidget(c->canvas);
			}
			break;
		case yearGlance:
			date = lastjan1(c->view->date);
			if (timeok(date)) {
				c->view->olddate = c->view->date;
				c->view->date = date;
				c->view->nwks = numwks(c->view->date); 
/*
 * Since we know year view is already showing, just directly force a new
 * year to be displayed without first managing all the year stuff.
 */
				paint_year(c);
				calendar_select(c, monthSelect, NULL);
			}
			break;
		case dayGlance:
			date = last_ndays(c->view->date, 1);
			if (timeok(date)) {
				c->view->olddate = c->view->date;
				c->view->date = date;
				c->view->nwks = numwks(c->view->date);
				calendar_deselect(c);
				if (c->view->date <
				    lowerbound(((Day *)(c->view->day_info))->month1)) { 
        				((Day *)(c->view->day_info))->month3
						= c->view->date;
					((Day *)(c->view->day_info))->month2
						= previousmonth(c->view->date);
					((Day *)(c->view->day_info))->month1
						= previousmonth(((Day *)(c->view->day_info))->month2);
					init_dayview(c);
					paint_dayview(c, True, NULL, False);
				} else { 
					monthbox_deselect(c);
					monthbox_datetoxy(c);
					paint_dayview(c, False, NULL, False);
					monthbox_select(c);
				}
				calendar_select(c, hourSelect, NULL);
			}
			break;
		default:
			break;
	}
}

void
next_button(Widget w, XtPointer client_data, XtPointer cbs)
{
	Tick date;
	Calendar *c = calendar;
	new_XContext *xc;
	Editor *e = (Editor *) calendar->editor;

	invalidate_cache(c);

	xc = c->xcontext;
	switch(c->view->glance) {
		case weekGlance:
			date = next_ndays(c->view->date, 7);
			if (timeok(date) ||
			    ((date = get_eot()) != c->view->date)) {
				c->view->olddate = c->view->date;
				c->view->date = date;
				c->view->nwks = numwks(c->view->date);
				paint_weekview(c, NULL);
			}
			break;
		case monthGlance:
			date = nextmonth(c->view->date);
			if (timeok(date)) {
				XtUnmapWidget(c->canvas);
				c->view->olddate = c->view->date;
				c->view->date = date;
				c->view->nwks = numwks(date);
				(void)prepare_to_paint_monthview(c, NULL);
				XtMapWidget(c->canvas);
			}
			break;
		case yearGlance:
			date = nextjan1(c->view->date);
			if (timeok(date)) {
				c->view->olddate = c->view->date;
				c->view->date = date;
				c->view->nwks = numwks(date);
/*
 * Since we know year view is already showing, just directly force a new
 * year to be displayed without first managing all the year stuff.
 */
				paint_year(c);
				calendar_select(c, monthSelect, NULL);
			}
			break;
		case dayGlance:
			date = next_ndays(c->view->date, 1);
			if (timeok(date)) {
				c->view->olddate = c->view->date;
				c->view->date = date;
				c->view->nwks = numwks(c->view->date);
				/* beyond month range displayed */
				if (c->view->date > upperbound(last_dom(
				    (((Day *)(c->view->day_info))->month3)))) {
					((Day *)(c->view->day_info))->month1
						= c->view->date;
        				((Day *)(c->view->day_info))->month2
						= nextmonth(c->view->date);
					((Day *)(c->view->day_info))->month3
						= nextmonth(((Day *)(c->view->day_info))->month2);
					calendar_deselect(c);
					init_dayview(c);
					paint_dayview(c, True, NULL, False);
					calendar_select(c, hourSelect, NULL);
				} else {
					calendar_deselect(c);
					monthbox_deselect(c);
					monthbox_datetoxy(c);
					paint_dayview(c, False, NULL, False);
					calendar_select(c, hourSelect, NULL);
					monthbox_select(c);
				}
			}
			break;
		default:
			break;
	}
}

void
today_button(Widget w, XtPointer client_data, XtPointer cbs)
{
	Tick today, date;
	Calendar *c = calendar;
	new_XContext *xc;
	int mo_da, mo_to;
	pr_pos xy;
	Editor *e = (Editor *) calendar->editor;

	invalidate_cache(c);

	date = c->view->date;
	c->view->olddate = c->view->date;
	c->view->date = today = now();
	mo_da = month(date);
	mo_to = month(today);
	c->view->nwks = numwks(today);
	calendar_deselect(c);
	switch(c->view ->glance) {
	case weekGlance:
		if (mo_da != mo_to || dom(date) != dom(today) ||
			year(date) != year(today)) {
			paint_weekview(c, NULL);
		}
		calendar_select(c, weekdaySelect, NULL);
		break;
	case monthGlance:
		if (mo_da != mo_to || year(date) != year(today)) {
			XtUnmapWidget(c->canvas);
			xc = c->xcontext;
			(void)prepare_to_paint_monthview(c, NULL);
			XtMapWidget(c->canvas);
		}
		calendar_select(c, daySelect, (caddr_t)NULL);
		break;
	case yearGlance:
/*
 * Since we know year view is already showing, just directly force a new
 * year to be displayed without first managing all the year stuff.
 */
		if (year(date) != year(today)) 
			paint_year(c);
		else calendar_deselect(c); 
		xy.y = month_row_col[mo_to-1][ROW];
		xy.x = month_row_col[mo_to-1][COL];
		calendar_select(c, monthSelect, (caddr_t)&xy);
		break;
	case dayGlance:
		if (mo_da != mo_to || dom(date) != dom(today) ||
				year(date) != year(today)) {
			if (mo_to < month(((Day *)(c->view->day_info))->month1) ||
				mo_to > month(((Day *)(c->view->day_info))->month3)) {
                		calendar_deselect(c);
				init_mo(c);
				init_dayview(c);
				paint_dayview(c, True, NULL, True);
			}
			else {
				calendar_deselect(c);
				monthbox_deselect(c);
				monthbox_datetoxy(c);
				paint_dayview(c, False, NULL, True);
				monthbox_select(c);
			}
		}
		calendar_select(c, hourSelect,(caddr_t)NULL);
		break;
	default:
		break;
	}
}

/*
static Menu
gen_tzmenu(item, op)
	Menu_item item;
	Menu_generate op;
{
	char *path;
	static Menu pullright;

	switch(op) {
	case MENU_DISPLAY	: 
		if (pullright == NULL) {
*/
/*
			path = (char *) xv_get(item, MENU_CLIENT_DATA);
*/
/*
			XtVaGetArgs(item, XmNuserData, path, NULL);
			pullright = make_tzmenu(path);
			break;
		}
	case MENU_DISPLAY_DONE :
	case MENU_NOTIFY	:
	case MENU_NOTIFY_DONE	: break ;
	}
	return(pullright);
}
*/

static void
set_default_view(Calendar *c)
{
	/* See if the view is set via a command line option */
	if (calendar->app_data->default_view) {
		if (!strcmp(calendar->app_data->default_view, "year"))
			calendar->view->glance = yearGlance;
		else if (!strcmp(calendar->app_data->default_view, "month"))
			calendar->view->glance = monthGlance;
		else if (!strcmp(calendar->app_data->default_view, "week"))
			calendar->view->glance = weekGlance;
		else if (!strcmp(calendar->app_data->default_view, "day")) 
			calendar->view->glance = dayGlance;
		else
			calendar->view->glance = dayGlance;
	} else
		calendar->view->glance = dayGlance;
}

/*
 * Changed the code that calls these to directly call the callbacks
 * called from here... not sure why there was this indirection.
 * It should be safe to remove these.
 */
void view_day_cb(Widget w, XtPointer data, XtPointer cbs)
{
	if ((intptr_t) data == 0)
		day_button(w, data, cbs);
}

void view_week_cb(Widget w, XtPointer data, XtPointer cbs)
{
	if ((intptr_t) data == 0)
		week_button(w, data, cbs);
}

void view_month_cb(Widget w, XtPointer data, XtPointer cbs)
{
	if ((intptr_t) data == 0)
		month_button(w, data, cbs);
}

void view_year_cb( Widget w, XtPointer data, XtPointer cbs)
{
	if ((intptr_t) data == 0)
		(void) year_button(w, data, cbs);
}

static void
make_view_menu(Calendar *c)
{
	void view_appt_cb(), view_todo_cb();
        Widget view_menu, day, week, month, year, appt_list, tz,
		view_btn, sep, find, go_to;
        XmString day_str, week_str, month_str, year_str, tz_str, apptlist_str,
		todo_str, view_str, findappts, gotodate;
	Arg al[10];
	int ac;

        day_str = XmStringCreateLocalized(catgets(c->DT_catd, 1, 33, "Day"));
        week_str = XmStringCreateLocalized(catgets(c->DT_catd, 1, 34, "Week"));
        month_str = XmStringCreateLocalized(
			catgets(c->DT_catd, 1, 35, "Month"));
        year_str = XmStringCreateLocalized(catgets(c->DT_catd, 1, 36, "Year"));
        tz_str = XmStringCreateLocalized(
			catgets(c->DT_catd, 1, 37, "Time Zone..."));
	apptlist_str = XmStringCreateLocalized(
			catgets(c->DT_catd, 1, 38, "Appointment List..."));
	todo_str = XmStringCreateLocalized(
			catgets(c->DT_catd, 1, 965, "To Do List..."));
	view_str = XmStringCreateLocalized(catgets(c->DT_catd, 1, 40, "View"));
	findappts = XmStringCreateLocalized(
			catgets(c->DT_catd, 1, 41, "Find..."));
	gotodate = XmStringCreateLocalized(
			catgets(c->DT_catd, 1, 42, "Go to Date..."));

        view_menu = XmCreatePulldownMenu(c->menu_bar, "viewMenu", NULL, 0);

        ac = 0;
        XtSetArg(al[ac], XmNlabelString, day_str); ac++;
        day = XmCreatePushButtonGadget(view_menu, "day", al, ac);
        XtAddCallback(day, XmNactivateCallback, day_button, NULL);

        ac = 0;
        XtSetArg(al[ac], XmNlabelString, week_str); ac++;
        week = XmCreatePushButtonGadget(view_menu, "week", al, ac);
        XtAddCallback(week, XmNactivateCallback, week_button, NULL);

        ac = 0;
        XtSetArg(al[ac], XmNlabelString, month_str); ac++;
        month = XmCreatePushButtonGadget(view_menu, "month", al, ac);
        XtAddCallback(month, XmNactivateCallback, month_button, NULL);

        ac = 0;
        XtSetArg(al[ac], XmNlabelString, year_str); ac++;
        year = XmCreatePushButtonGadget(view_menu, "year", al, ac);
        XtAddCallback(year, XmNactivateCallback, year_button, NULL);

	ac = 0;
	sep = XmCreateSeparatorGadget(view_menu, "separator1", al, ac);

        ac = 0;
        XtSetArg(al[ac], XmNlabelString, apptlist_str); ac++;
        appt_list = XmCreatePushButtonGadget(view_menu, "apptList", al, ac);
        XtAddCallback(appt_list, XmNactivateCallback, view_appt_cb, NULL);

        ac = 0;
        XtSetArg(al[ac], XmNlabelString, todo_str); ac++;
        c->todo_view_menu = XmCreatePushButtonGadget(view_menu, "toDoList", 
			al, ac);
        XtAddCallback(c->todo_view_menu, XmNactivateCallback, 
			view_todo_cb, NULL);

	ac = 0;
	sep = XmCreateSeparatorGadget(view_menu, "separator2", al, ac);

	ac = 0;
	XtSetArg(al[ac], XmNlabelString, findappts); ac++;
	find = XmCreatePushButtonGadget(view_menu, "find", al, ac);
        XtAddCallback(find, XmNactivateCallback, find_cb, NULL);

	ac = 0;
	XtSetArg(al[ac], XmNlabelString, gotodate); ac++;
	go_to = XmCreatePushButtonGadget(view_menu, "goTo", al, ac);
        XtAddCallback(go_to, XmNactivateCallback, goto_cb, NULL);

        ac = 0;
        XtSetArg(al[ac], XmNlabelString, tz_str); ac++;
        tz = XmCreatePushButtonGadget(view_menu, "timeZone", al, ac);
        XtAddCallback(tz, XmNactivateCallback, timezone_cb, NULL);

	ManageChildren(view_menu);

	XtSetArg(al[ac], XmNsubMenuId, view_menu); ac++;
	XtSetArg(al[ac], XmNlabelString, view_str); ac++;
	view_btn = (Widget) XmCreateCascadeButton(c->menu_bar, "view", al, ac);
        XtAddCallback(view_btn, XmNhelpCallback, (XtCallbackProc)help_cb, 
			(XtPointer) CALENDAR_HELP_VIEW_BUTTON);

        XmStringFree(day_str);
        XmStringFree(week_str);
        XmStringFree(month_str);
        XmStringFree(year_str);
        XmStringFree(tz_str);
        XmStringFree(apptlist_str);
        XmStringFree(todo_str);
        XmStringFree(view_str);
	XmStringFree(findappts);
	XmStringFree(gotodate);
}

extern void
print_cb(Widget ignore1, XtPointer ignore2, XtPointer ignore3)
{
  _DtTurnOnHourGlass(calendar->frame);
  post_print_dialog(calendar);
  _DtTurnOffHourGlass(calendar->frame);
}

extern void
print_current(Widget widget, XtPointer client_data, XtPointer reason)
{
  _DtTurnOnHourGlass(calendar->frame);
  create_print_dialog(calendar);
  print_report(calendar);
  _DtTurnOffHourGlass(calendar->frame);
}

void 
edit_cb( Widget w, XtPointer item_no, XtPointer cbs)
{
	Calendar *c = calendar;

	_DtTurnOnHourGlass(c->frame);
	if ((intptr_t) item_no == 1)
		show_props_pu(c);
	else if ((intptr_t) item_no == 3)
		make_find(c);
	_DtTurnOffHourGlass(c->frame);
}

void 
find_cb(Widget w, XtPointer item_no, XtPointer cbs)
{
	Calendar *c = calendar;

	_DtTurnOnHourGlass(c->frame);
        show_find(calendar);
	_DtTurnOffHourGlass(c->frame);
}

void 
goto_cb(Widget w, XtPointer item_no, XtPointer cbs)
{
	Calendar *c = calendar;

	_DtTurnOnHourGlass(c->frame);
        show_goto(calendar);
	_DtTurnOffHourGlass(c->frame);
}

void 
timezone_cb(Widget w, XtPointer item_no, XtPointer cbs)
{
	Calendar *c = calendar;

	_DtTurnOnHourGlass(c->frame);
        show_timezone(calendar);
	_DtTurnOffHourGlass(c->frame);
}

void
view_appt_cb(Widget w, XtPointer item_no, XtPointer cbs)
{
	Calendar	*c = calendar;

	_DtTurnOnHourGlass(c->frame);
	show_editor_view(c, c->view->glance);
	_DtTurnOffHourGlass(c->frame);
}

void
view_todo_cb(Widget w, XtPointer item_no, XtPointer cbs)
{
	Calendar	*c = calendar;

	_DtTurnOnHourGlass(c->frame);
	show_todo_view(c, VIEW_PENDING);
	_DtTurnOffHourGlass(c->frame);
}

void 
appt_cb(Widget w, XtPointer item_no, XtPointer cbs)
{
	Calendar *c = calendar;

	_DtTurnOnHourGlass(c->frame);
	switch((intptr_t) item_no) {
	case 0:
		show_editor(c, 0, 0, False);
		break;
	case 1:
		show_geditor(c, 0, 0);
		break;
	case 2:
		show_todo(c);
		break;
	default:
		break;
	}
	_DtTurnOffHourGlass(c->frame);
}

static void 
show_other_cb(Widget w, XtPointer cl_data, XtPointer cbs)
{
	Calendar *c = calendar;

	_DtTurnOnHourGlass(c->frame);
	show_tempbr(c);
	_DtTurnOffHourGlass(c->frame);
}

static void 
compare_cb(Widget w, XtPointer cl_data, XtPointer cbs)
{
	Calendar *c = calendar;

	_DtTurnOnHourGlass(c->frame);
	show_browser(c);
	_DtTurnOffHourGlass(c->frame);
}

static void 
edit_menu_cb(Widget w, XtPointer cl_data, XtPointer cbs)
{
	Calendar	*c = (Calendar *)cl_data;

	_DtTurnOnHourGlass(c->frame);
	show_browselist(c);
	_DtTurnOffHourGlass(c->frame);
}

static void 
browse_cb(Widget w, XtPointer client_data, XtPointer cbs) {
	int		rcode, idx = (int) (intptr_t) client_data;
	char		buf[MAXNAMELEN];
	Calendar	*c = calendar;
	Props_pu	*p = (Props_pu *)c->properties_pu;
	BlistData	*bd;
	Browselist	*bl = (Browselist *)c->browselist;

	if (!(bd = (BlistData *)CmDataListGetData(bl->blist_data, idx))) {
	  	char *title = XtNewString(catgets(c->DT_catd, 1, 1072,
				"Calendar : Error - Browse"));
		char *text = XtNewString(catgets(c->DT_catd, 1, 610,
				"Internal error retrieving calendar name."));
		char *ident = XtNewString(catgets(c->DT_catd, 1, 95, "Continue"));
		dialog_popup(c->frame,
			DIALOG_TITLE, title,
			DIALOG_TEXT, text,
			BUTTON_IDENT, 1, ident,
			DIALOG_IMAGE, p->xm_error_pixmap,
			NULL);
		XtFree(ident);
		XtFree(text);
		XtFree(title);
		return;
	}

	_DtTurnOnHourGlass(c->frame);
#ifdef FNS
	if (cmfns_use_fns((Props *)c->properties)) {
		rcode = cmfns_lookup_calendar(bd->name, buf, sizeof(buf));
	} else {
		rcode = -1;
	}

	if (rcode > 0)
		switch_it(c, buf, main_win);
	else
#endif
		switch_it(c, bd->name, main_win);
	_DtTurnOffHourGlass(c->frame);
}

static void
make_file_menu(Calendar *c)
{
	Widget file_menu, print, pr_current, opts, exit, file_btn, sep;
	XmString xmstr;
	Arg al[10];
	int ac;

	file_menu = XmCreatePulldownMenu(c->menu_bar, "fileMenu", NULL, 0);

	ac = 0;
	xmstr = XmStringCreateLocalized(
			catgets(c->DT_catd, 1, 68, "Print Current View"));
	XtSetArg(al[ac], XmNlabelString, xmstr); ac++;
	pr_current = XmCreatePushButtonGadget(file_menu, "printCurrent", 
					      al, ac);
        XtAddCallback(pr_current, XmNactivateCallback, print_current, NULL);
	XmStringFree(xmstr);

	ac = 0;
	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 69, "Print..."));
	XtSetArg(al[ac], XmNlabelString, xmstr); ac++;
	print = XmCreatePushButtonGadget(file_menu, "print", al, ac);
        XtAddCallback(print, XmNactivateCallback, print_cb, NULL);
	XmStringFree(xmstr);

	ac = 0;
	sep = XmCreateSeparatorGadget(file_menu, "separator1", al, ac);

	ac = 0;
	xmstr = XmStringCreateLocalized(
				catgets(c->DT_catd, 1, 72, "Options..."));
	XtSetArg(al[ac], XmNlabelString, xmstr); ac++;
	opts = XmCreatePushButtonGadget(file_menu, "options", al, ac);
	XtAddCallback(opts, XmNactivateCallback, edit_cb, (XtPointer) 1);
	XmStringFree(xmstr);

	ac = 0;
	sep = XmCreateSeparatorGadget(file_menu, "separator2", al, ac);

	ac = 0;
	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 73, "Exit"));
	XtSetArg(al[ac], XmNlabelString, xmstr); ac++;
	exit = XmCreatePushButtonGadget(file_menu, "exit", al, ac);
	XtAddCallback(exit, XmNactivateCallback, quit_handler, (XtPointer) c);
	XmStringFree(xmstr);

	ManageChildren(file_menu);

	ac = 0;
	XtSetArg(al[ac], XmNsubMenuId, file_menu); ac++;
	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 65, "File"));
	XtSetArg(al[ac], XmNlabelString, xmstr); ac++;
	file_btn = (Widget) XmCreateCascadeButton(c->menu_bar, "file", al, ac);
        XtAddCallback(file_btn, XmNhelpCallback, (XtCallbackProc)help_cb, 
					(XtPointer) CALENDAR_HELP_FILE_BUTTON);
	XmStringFree(xmstr);
}

static void
make_edit_menu(Calendar *c)
{
	Widget edit_menu, edit_button;
	Widget appt, props;
	XmString edit_str, appt_str, todo_str;
	Arg al[10];
	int ac;

	appt_str = XmStringCreateLocalized(
			catgets(c->DT_catd, 1, 74, "Appointment..."));
        todo_str = XmStringCreateLocalized(
			catgets(c->DT_catd, 1, 966, "To Do..."));
        edit_str = XmStringCreateLocalized(
			catgets(c->DT_catd, 1, 76, "Edit"));

	edit_menu = XmCreatePulldownMenu(c->menu_bar, "editMenu", NULL, 0);

	ac = 0;
	XtSetArg(al[ac], XmNlabelString, appt_str); ac++;
	appt = XmCreatePushButtonGadget(edit_menu, "appt", al, ac);
	XtAddCallback(appt, XmNactivateCallback, appt_cb, (XtPointer) 0);

	ac = 0;
	XtSetArg(al[ac], XmNlabelString, todo_str); ac++;
	c->todo_edit_menu = XmCreatePushButtonGadget(edit_menu, "toDo", al, ac);
	XtAddCallback(c->todo_edit_menu, XmNactivateCallback, 
			appt_cb, (XtPointer) 2);

	ManageChildren(edit_menu);

	ac = 0;
	XtSetArg(al[ac], XmNsubMenuId, edit_menu); ac++;
	XtSetArg(al[ac], XmNlabelString, edit_str); ac++;
	edit_button = XmCreateCascadeButton(c->menu_bar, "edit", al, ac);
        XtAddCallback(edit_button, XmNhelpCallback, (XtCallbackProc)help_cb,
			(XtPointer) CALENDAR_HELP_EDIT_BUTTON);

	XmStringFree(appt_str);
	XmStringFree(todo_str);
	XmStringFree(edit_str);
}

static void
make_help_menu(Calendar *c)
{
	Arg al[10];
	int ac;
	XmString help, overview, tasks, ref, onitem, using, about;
	Widget	cascade, help_menu, w_overview, w_tasks, w_ref, w_onitem, 
		w_using, w_about, w_sep;

        help = XmStringCreateLocalized(catgets(c->DT_catd, 1, 77, "Help"));
        overview = XmStringCreateLocalized(
			catgets(c->DT_catd, 1, 612, "Overview..."));
        tasks = XmStringCreateLocalized(catgets(c->DT_catd, 1, 79, "Tasks..."));
        ref = XmStringCreateLocalized(
			catgets(c->DT_catd, 1, 80, "Reference..."));
        onitem = XmStringCreateLocalized(
			catgets(c->DT_catd, 1, 964, "On Item"));
        using = XmStringCreateLocalized(
			catgets(c->DT_catd, 1, 82, "Using Help..."));
        about = XmStringCreateLocalized(
			catgets(c->DT_catd, 1, 613, "About Calendar..."));   

        help_menu = XmCreatePulldownMenu(c->menu_bar, "helpMenu", NULL, 0);

        ac = 0;
        XtSetArg(al[ac], XmNlabelString, overview); ac++;
        w_overview = XmCreatePushButtonGadget(help_menu, "overview", al, ac);
	XtAddCallback(w_overview, XmNactivateCallback, show_main_help, 
			(XtPointer)HELP_OVERVIEW); 

	ac = 0;
	w_sep = XmCreateSeparatorGadget(help_menu, "separator1", al, ac);

        ac = 0;
        XtSetArg(al[ac], XmNlabelString, tasks); ac++;
        w_tasks = XmCreatePushButtonGadget(help_menu, "tasks", al, ac);
	XtAddCallback(w_tasks, XmNactivateCallback, show_main_help, 
			(XtPointer)HELP_TASKS);

        ac = 0;
        XtSetArg(al[ac], XmNlabelString, ref); ac++;
        w_ref = XmCreatePushButtonGadget(help_menu, "reference", al, ac);
	XtAddCallback(w_ref, XmNactivateCallback, show_main_help, 
			(XtPointer)HELP_REFERENCE);

        ac = 0;
        XtSetArg(al[ac], XmNlabelString, onitem); ac++;
        w_onitem = XmCreatePushButtonGadget(help_menu, "onItem", al, ac);
	XtAddCallback(w_onitem, XmNactivateCallback, show_main_help, 
			(XtPointer)HELP_ONITEM);

	ac = 0;
	w_sep = XmCreateSeparatorGadget(help_menu, "separator2", al, ac);

        ac = 0;
        XtSetArg(al[ac], XmNlabelString, using); ac++;
        w_using = XmCreatePushButtonGadget(help_menu, "using", al, ac);
	XtAddCallback(w_using, XmNactivateCallback, show_main_help, 
			(XtPointer)HELP_USINGHELP);

	ac = 0;
	w_sep = XmCreateSeparatorGadget(help_menu, "separator2", al, ac);

        ac = 0;
        XtSetArg(al[ac], XmNlabelString, about); ac++;
        w_about = XmCreatePushButtonGadget(help_menu, "about", al, ac);
        XtAddCallback(w_about, XmNactivateCallback, show_about, 
			(XtPointer)NULL);
        XtAddCallback(w_about, XmNactivateCallback, show_main_help, 
			(XtPointer)HELP_ABOUTCALENDAR);

	ManageChildren(help_menu);

	ac = 0;
	XtSetArg(al[ac], XmNlabelString, help); ac++;
	XtSetArg(al[ac], XmNsubMenuId, help_menu); ac++;
	cascade = (Widget)XmCreateCascadeButton(c->menu_bar, "help", al, ac);
        XtAddCallback(cascade, XmNhelpCallback, (XtCallbackProc)help_cb, 
			(XtPointer) CALENDAR_HELP_HELP_BUTTON);

	ac = 0;
	XtSetArg(al[ac], XmNmenuHelpWidget, cascade); ac++;
	XtSetValues(c->menu_bar, al, ac);

        XmStringFree(help);
        XmStringFree(overview);
        XmStringFree(tasks);
        XmStringFree(ref);
        XmStringFree(onitem);
        XmStringFree(using);
        XmStringFree(about);
}

static void
map_browse_menu(Widget menu, XtPointer client_data, XtPointer call_data)

{
	Position y;
	Dimension h, w;
	short	maxcols, newcols, columns;
	int	screenheight = HeightOfScreen(XtScreen(menu));
	int	fudgefact = 20; /* to allow for decorations on menu */

	XtVaGetValues(menu,
	XmNheight, &h,
	XmNwidth, &w,
	XmNy, &y,
	XmNnumColumns, &columns,
	NULL);


	if ((int) (h + fudgefact) > ((int) screenheight / 2)) {

	/* the menu is taller than half the screen.  We need to find out how 
	   many more columns to specify for the menu to make it fit. */

		newcols = (columns * (int) ((int) (h + fudgefact)/(int) (screenheight/2))) + 1;
		maxcols = WidthOfScreen(XtScreen(menu))/(int) ((int)w/(int)columns);

		if (newcols > maxcols)
			newcols = maxcols;

		XtVaSetValues(menu, XmNnumColumns, newcols, NULL);
	}

}


/*
 * This a seperate routine so that it may be called when a new name is added
 * from the edit list
 */
extern void
update_browse_menu_names(Calendar *c) {
	int		i, ac;
	Arg		al[10];
	char		*str;
	Widget		item;
	XmString	xmstr;
	BlistData	*bd;
	Browselist	*bl = (Browselist *)c->browselist;
 
	c->browse_menu =
		XmCreatePulldownMenu(c->menu_bar, "browseMenu", NULL, 0);
	XtVaSetValues(c->browse_menu, 
		XmNnumColumns, 1, 
                XmNpacking, XmPACK_COLUMN,
                XmNorientation, XmVERTICAL,
		NULL);

	ac = 0;
	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 84,
						"Show Other Calendar..."));
	XtSetArg(al[ac], XmNlabelString, xmstr); ac++;
	item = XmCreatePushButtonGadget(c->browse_menu, "showOther", al, ac);
        XtAddCallback(item, XmNactivateCallback, show_other_cb, NULL);
	XmStringFree(xmstr);

	ac = 0;
	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 85,
						"Compare Calendars..."));
	XtSetArg(al[ac], XmNlabelString, xmstr); ac++;
	item = XmCreatePushButtonGadget(c->browse_menu, "compare", al, ac);
        XtAddCallback(item, XmNactivateCallback, compare_cb, NULL);
	XmStringFree(xmstr);

	ac = 0;
	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 86,
						"Menu Editor..."));
	XtSetArg(al[ac], XmNlabelString, xmstr); ac++;
	item = XmCreatePushButtonGadget(c->browse_menu, "editMenu", al, ac);
        XtAddCallback(item, XmNactivateCallback, edit_menu_cb, (XtPointer)c);
	XmStringFree(xmstr);

	ac = 0;
	item = XmCreateSeparatorGadget(c->browse_menu, "separator", al, ac);

	blist_clean(bl, TRUE);
	blist_init_names(c);
	for (i = 1; i <= bl->blist_data->count; i++) {
		bd = (BlistData *)CmDataListGetData(bl->blist_data, i);
		if (!bd || blank_buf(bd->name) || bd->tag != BLIST_ACTIVE)
			continue;

		ac = 0;
		xmstr = XmStringCreateLocalized(bd->name);
		XtSetArg(al[ac], XmNlabelString, xmstr); ac++;
		str = (!strcmp(bd->name, c->calname)) ? "calName" : "myName";
		item = XmCreatePushButtonGadget(c->browse_menu, str, al, ac);
		XtAddCallback(item, XmNactivateCallback, browse_cb,
			      (XtPointer) (intptr_t) i);
		XmStringFree(xmstr);
	}

	XtAddCallback(c->browse_menu, XmNmapCallback, map_browse_menu, NULL); 
	ManageChildren(c->browse_menu);
}


static void
make_browse_menu(Calendar *c) {
	Arg		al[10];
	int		ac = 0;
	XmString	xmstr;

	/*
	 * This needs to be a seperate routine so it can be called when the
	 * edit list changes the browse menu
	 */
	update_browse_menu_names(c);

	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 87, "Browse"));
        XtSetArg(al[ac], XmNsubMenuId, c->browse_menu); ac++;
        XtSetArg(al[ac], XmNlabelString, xmstr); ac++;
        c->browse_button = XmCreateCascadeButton(c->menu_bar, "browse", al, ac);
        XtAddCallback(c->browse_button, XmNhelpCallback,(XtCallbackProc)help_cb,
		      (XtPointer)CALENDAR_HELP_BROWSE_BUTTON);
	XtManageChild(c->browse_button);
	XmStringFree(xmstr);
}

/*
 * Create the menu bar and all the pulldowns it manages
 * Menu bar is associated with its MainWindow parent after this returns
 */
static void
make_menus(Calendar *c)
{
	int	ac = 0;
	Arg	al[10];

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	c->menu_bar = XmCreateMenuBar(c->form, "menuBar", al, ac);

	make_file_menu(c);
	make_edit_menu(c);
	make_view_menu(c);
	make_browse_menu(c);
	make_help_menu(c);
	ManageChildren(c->menu_bar);
}

/*
 * Create the toolbar in the main window
 */
static void
make_buttons(Calendar *c)
{
	XmString	today_str;
	Arg		al[10];
	int		ac = 0;
	Dimension	today_width;
	Widget		toolform;
	Props_pu	*pu = (Props_pu *)c->properties_pu;
	Pixel           fg, bg;
	Screen          *s;

        today_str = XmStringCreateLocalized(
			catgets(c->DT_catd, 1, 88, "Today"));
        ac=0;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
        XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
        XtSetArg(al[ac], XmNtopWidget, c->menu_bar); ac++;
	calendar->toolbar = XmCreateFrame(c->form, "toolbar", al, ac);

	s = XtScreen(calendar->toolbar);
	XtVaGetValues(calendar->toolbar, 
			XmNforeground, 	&fg, 
			XmNbackground, 	&bg,
			NULL);

	pu->appt_button_xbm = XmGetPixmap(s, "DtCMapt.s.pm", fg, bg);
	pu->todo_button_xbm = XmGetPixmap(s, "DtCMtdo.s.pm", fg, bg);
	pu->year_button_xbm = XmGetPixmap(s, "DtCMyr.s.pm", fg, bg);
	pu->month_button_xbm = XmGetPixmap(s, "DtCMmth.s.pm", fg, bg);
	pu->week_button_xbm = XmGetPixmap(s, "DtCMwk.s.pm", fg, bg);
	pu->day_button_xbm = XmGetPixmap(s, "DtCMday.s.pm", fg, bg);

        ac=0;
	XtSetArg(al[ac], XmNfractionBase, 100); ac++;
	toolform = XmCreateForm(c->toolbar, "toolform", al, ac);

	ac=0;
	XtSetArg(al[ac], XmNlabelString, today_str); ac++;
        XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
        XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
        XtSetArg(al[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
        XtSetArg(al[ac], XmNleftPosition, 50); ac++;
        XtSetArg(al[ac], XmNnavigationType, XmTAB_GROUP); ac++;
	c->today = XmCreatePushButton(toolform, "today", al, ac);
	XmStringFree(today_str);
        XtAddCallback(calendar->today, XmNactivateCallback, today_button, NULL);
        XtAddCallback(c->today, XmNhelpCallback, (XtCallbackProc)help_cb, 
			(XtPointer) CALENDAR_HELP_TODAY_BUTTON);

	/*
	 * Need to centre the today button, as it shrink-wraps its
	 * label, and will now be out of position.  Unpleasant, but
	 * necessary because XmForm doesn't provide direct means of centring.
	 */
	XtVaGetValues(c->today, XmNwidth, &today_width, NULL);
	XtVaSetValues(c->today, XmNleftOffset, (int) today_width/-2, NULL);

        ac=0;
        XtSetArg(al[ac], XmNarrowDirection, XmARROW_LEFT); ac++;
        XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
        XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
        XtSetArg(al[ac], XmNrightAttachment, XmATTACH_WIDGET); ac++;
        XtSetArg(al[ac], XmNrightWidget, c->today); ac++;
        XtSetArg(al[ac], XmNnavigationType, XmTAB_GROUP); ac++;
        c->previous = XmCreateArrowButtonGadget(toolform, "previous", al, ac);
        XtAddCallback(c->previous, XmNactivateCallback, prev_button, NULL);
        XtAddCallback(c->previous, XmNhelpCallback, (XtCallbackProc)help_cb, 
			(XtPointer) CALENDAR_HELP_PREV_BUTTON);

        ac=0;
        XtSetArg(al[ac], XmNarrowDirection, XmARROW_RIGHT); ac++;
        XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
        XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
        XtSetArg(al[ac], XmNleftAttachment, XmATTACH_WIDGET); ac++;
        XtSetArg(al[ac], XmNleftWidget, c->today); ac++;
        XtSetArg(al[ac], XmNnavigationType, XmTAB_GROUP); ac++;
        c->next = XmCreateArrowButtonGadget(toolform, "next", al, ac);
        XtAddCallback(c->next, XmNactivateCallback, next_button, NULL);
        XtAddCallback(c->next, XmNhelpCallback, (XtCallbackProc)help_cb, 
			(XtPointer) CALENDAR_HELP_NEXT_BUTTON);

	/* toolbar "editor" buttons */
        ac=0;
        XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
        XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
        XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
        XtSetArg(al[ac], XmNlabelType, XmPIXMAP); ac++;
        XtSetArg(al[ac], XmNlabelPixmap, pu->appt_button_xbm); ac++;
        XtSetArg(al[ac], XmNnavigationType, XmTAB_GROUP); ac++;
        c->appt_btn = XmCreatePushButtonGadget(toolform, "appt", al, ac);
	XtAddCallback(c->appt_btn, XmNactivateCallback, appt_cb, (XtPointer) 0);
        XtAddCallback(c->appt_btn, XmNhelpCallback, (XtCallbackProc)help_cb, 
			(XtPointer) CALENDAR_HELP_APPT_BUTTON);

        ac=0;
        XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
        XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
        XtSetArg(al[ac], XmNleftAttachment, XmATTACH_WIDGET); ac++;
        XtSetArg(al[ac], XmNleftWidget, c->appt_btn); ac++;
        XtSetArg(al[ac], XmNlabelType, XmPIXMAP); ac++;
        XtSetArg(al[ac], XmNlabelPixmap, pu->todo_button_xbm); ac++;
        XtSetArg(al[ac], XmNnavigationType, XmTAB_GROUP); ac++;
        c->todo_btn = XmCreatePushButtonGadget(toolform, "todo", al, ac);
	XtAddCallback(c->todo_btn, XmNactivateCallback, appt_cb, (XtPointer) 2);
        XtAddCallback(c->todo_btn, XmNhelpCallback, (XtCallbackProc)help_cb, 
			(XtPointer) CALENDAR_HELP_TODO_BUTTON);

	ac=0;
        XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
        XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
        XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
        XtSetArg(al[ac], XmNorientation, XmHORIZONTAL); ac++;
	c->radio = XmCreateRadioBox(toolform, "radio", al, ac);

	/* toolbar "scope navigation" buttons */
        ac=0;
        XtSetArg(al[ac], XmNlabelType, XmPIXMAP); ac++;
        XtSetArg(al[ac], XmNlabelPixmap, pu->day_button_xbm); ac++;
        XtSetArg(al[ac], XmNfillOnSelect, True); ac++;
        XtSetArg(al[ac], XmNindicatorOn, False); ac++;
        XtSetArg(al[ac], XmNshadowThickness, 1); ac++;
        XtSetArg(al[ac], XmNnavigationType, XmTAB_GROUP); ac++;
        c->day_scope = XmCreateToggleButtonGadget(c->radio, "day", al, ac);
        XtAddCallback(c->day_scope, XmNvalueChangedCallback, day_button, NULL);
        XtAddCallback(c->day_scope, XmNhelpCallback, (XtCallbackProc)help_cb, 
			(XtPointer) CALENDAR_HELP_DAY_BUTTON);

        ac=0;
        XtSetArg(al[ac], XmNlabelType, XmPIXMAP); ac++;
        XtSetArg(al[ac], XmNlabelPixmap, pu->week_button_xbm); ac++;
        XtSetArg(al[ac], XmNfillOnSelect, True); ac++;
        XtSetArg(al[ac], XmNindicatorOn, False); ac++;
        XtSetArg(al[ac], XmNshadowThickness, 1); ac++;
        XtSetArg(al[ac], XmNnavigationType, XmTAB_GROUP); ac++;
        c->week_scope = XmCreateToggleButtonGadget(c->radio, "week", al, ac);
        XtAddCallback(c->week_scope, XmNvalueChangedCallback, 
						week_button, NULL);
        XtAddCallback(c->week_scope, XmNhelpCallback, (XtCallbackProc)help_cb, 
			(XtPointer) CALENDAR_HELP_WEEK_BUTTON);

        ac=0;
        XtSetArg(al[ac], XmNlabelType, XmPIXMAP); ac++;
        XtSetArg(al[ac], XmNlabelPixmap, pu->month_button_xbm); ac++;
        XtSetArg(al[ac], XmNfillOnSelect, True); ac++;
        XtSetArg(al[ac], XmNindicatorOn, False); ac++;
        XtSetArg(al[ac], XmNshadowThickness, 1); ac++;
        XtSetArg(al[ac], XmNnavigationType, XmTAB_GROUP); ac++;
        c->month_scope = XmCreateToggleButtonGadget(c->radio, "month", al, ac);
        XtAddCallback(c->month_scope, XmNvalueChangedCallback, 
			month_button, NULL);
        XtAddCallback(c->month_scope, XmNhelpCallback, (XtCallbackProc)help_cb,
			(XtPointer) CALENDAR_HELP_MONTH_BUTTON);

        ac=0;
        XtSetArg(al[ac], XmNlabelType, XmPIXMAP); ac++;
        XtSetArg(al[ac], XmNlabelPixmap, pu->year_button_xbm); ac++;
        XtSetArg(al[ac], XmNfillOnSelect, True); ac++;
        XtSetArg(al[ac], XmNindicatorOn, False); ac++;
        XtSetArg(al[ac], XmNshadowThickness, 1); ac++;
        XtSetArg(al[ac], XmNnavigationType, XmTAB_GROUP); ac++;
        c->year_scope = XmCreateToggleButtonGadget(c->radio, "year", al, ac);
        XtAddCallback(c->year_scope, XmNvalueChangedCallback, 
			year_button, NULL);
        XtAddCallback(c->year_scope, XmNhelpCallback, (XtCallbackProc)help_cb, 
			(XtPointer) CALENDAR_HELP_YEAR_BUTTON);

	ManageChildren(c->radio);
	ManageChildren(toolform);
	XtManageChild(toolform);

}

/*
 * Ask the server for the list of reminders
 */
static CSA_return_code
get_next_reminder(Calendar *c, time_t tick, CSA_uint32 *num_reminders, CSA_reminder_reference **reminders) {
	CSA_return_code	stat;
	char	buffer[BUFSIZ];

	_csa_tick_to_iso8601(tick, buffer);

	stat = csa_read_next_reminder(c->my_cal_handle, 0, NULL, buffer, num_reminders, reminders, NULL);

	return stat;
}

static char *daystring[31] = {"1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","16","17","18","19","20","21","22","23","24","25","26","27","28","29","30","31"};


/*
**  Reset the reminders
*/
extern void
reset_alarm(Calendar *c)
{
	if (c->view->next_alarm != NULL) {
		csa_free(c->view->next_alarm);
		c->view->next_alarm = NULL;
	}
	reset_timer(c);
}

/*
**  Set the timer to call the reminder_driver for the next reminder.
*/
extern CSA_return_code
reset_timer(Calendar *c) {
	time_t			tick;
	CSA_reminder_reference	*last_r = c->view->next_alarm;
	CSA_return_code		stat;
	CSA_reminder_reference	*reminders;
	CSA_uint32		num_reminders;
	Tick			reminder_tick;

	/*
	 * set the reference time to now() or the time the
	 * last reminder is to happen whichever is earlier
	 */
	tick = now();
	if (last_r)
		_csa_iso8601_to_tick(last_r->run_time, &reminder_tick);
	
	if (last_r != NULL) {
		if (tick > reminder_tick)
			tick = reminder_tick;

		csa_free(last_r);
		c->view->next_alarm = NULL;
	}

	stat = get_next_reminder(c, tick, &num_reminders, &reminders);
	if (stat == SUCCESS && num_reminders > 0) {
		if (c->view->next_alarm_id)
			XtRemoveTimeOut(c->view->next_alarm_id);
		c->view->next_alarm = reminders;
		c->view->next_alarm_count = num_reminders;
		_csa_iso8601_to_tick(reminders[0].run_time, &reminder_tick);
		c->view->next_alarm_id = XtAppAddTimeOut(c->xcontext->app,
			(max(reminder_tick - now(), 1) * 1000), reminder_driver, c);
	}

	return stat;
}

void
center_today_button(Calendar *c)
{
	char buf[BUFSIZ];
	XmString today_buf;
	Dimension width;

	/*
	 * Need to re-centre the today button, as it shrink-wraps its
	 * label, and will now be out of position.  Unpleasant, but
	 * necessary because XmForm doesn't provide direct means of centring.
	 */
	XtVaGetValues(c->today, XmNwidth, &width, NULL);
	XtVaSetValues(c->today, XmNleftOffset, (int) width/-2, NULL);
}

void
move_to_new_day(XtPointer data, XtIntervalId *dummy)
{
        Tick midnight;
        unsigned long tomorrow;
	Calendar	*c = (Calendar *) data;

        if (c->form==NULL) return;

	/* figure out time for next timeout invocation */
        midnight = next_ndays(now(), 1)+30;
        tomorrow = (max(midnight-now(), 1));
	tomorrow = tomorrow * 1000;

	/* if date displayed is yesterday, update it to today */
	if (next_ndays(c->view->date, 1) == next_ndays(now()-daysec, 1)) {
		c->view->olddate = c->view->date;
		c->view->date = next_ndays(c->view->olddate, 1);
		invalidate_cache(c);
		paint_canvas(c, NULL, RENDER_UNMAP);
	}

	/* update the icon */
        paint_icon(c);

	/* reinstall this function as timeout handler for tomorrow */
        XtAppAddTimeOut(c->xcontext->app, tomorrow, move_to_new_day, c);
}

static void
setup_new_day_handler(c)
	Calendar *c;
{
	Tick midnight;
        unsigned long next_day;

        if (c->form==NULL) return;
        midnight = next_ndays(now(), 1)+30;
        next_day = (max(midnight-now(), 1));
	next_day = next_day * 1000;
        XtAppAddTimeOut(c->xcontext->app, next_day, move_to_new_day, c);
}
	

/* ARGSUSED */
/* Revisit ...
static Notify_value
wait3_handler(frame, pid, status, rusage)
        Notify_client frame;
        int pid;
        int status;
        struct rusage *rusage;
{
        exit(1);
	return NOTIFY_DONE;
}
*/

/*
 * Miscellaneous cleanup before the application exits.
 * Unregister current calendar.
 *
 * This is used as the activateCallback handler form the File->Exit
 * menu item, and overloaded as the handler from termination by Motif/Xt as
 * the XmAddWMProtocolCallback handler for delete_window.  
 */
void
quit_handler(Widget w, XtPointer cdata, XtPointer cbs) {
	Calendar	*c = (Calendar *)cdata;

	if (debug)
	  fprintf(stderr, "%s", catgets(c->DT_catd, 1, 89, "in quit_handler\n"));

	if (editor_showing((Editor*) c->editor) ||
	    todo_showing((ToDo*) c->todo))
	{
	    if (editor_showing((Editor*) c->editor))
	      show_editor(c, 0, 0, False);
	    if (todo_showing((ToDo*) c->todo))
	      show_todo(c);
	    if (NULL != c->frame)
	      XBell(XtDisplayOfObject(c->frame), 50);
	    return;
	} 

	if (c->view->next_alarm)
	  csa_free(c->view->next_alarm);

	props_clean_up((Props *)c->properties);
	if (c->cal_handle != c->my_cal_handle)
	  csa_logoff(c->cal_handle, NULL);
	csa_logoff(c->my_cal_handle, NULL);

	exit(0);
}

static void
sig_int_handler(
	int	sig)
{
	if (!calendar) exit(1);

	if (calendar->cal_handle != calendar->my_cal_handle)
		csa_logoff(calendar->cal_handle, NULL);

	if (calendar->my_cal_handle)
		csa_logoff(calendar->my_cal_handle, NULL);

	exit(0);
}

static int myabort(dpy, event)
        Display *dpy;
        XErrorEvent  *event;
{
/*
char buffer[MAXNAMELEN];
  char mesg[MAXNAMELEN];
  char number[32];
  char *mtype = "XlibMessage";
  FILE *fp = stdout;

  XGetErrorText( dpy, event->error_code, buffer, MAXNAMELEN );
  XGetErrorDatabaseText( dpy, mtype, "XError", "X Error (intercepted)",
                         mesg, MAXNAMELEN );
  ( void )fprintf( fp, "%s:  %s\n  ", mesg, buffer );
  XGetErrorDatabaseText( dpy, mtype, "MajorCode", "Request Major code %d",
                         mesg, MAXNAMELEN );
  ( void )fprintf( fp, mesg, event->request_code );
  sprintf( number, "%d", event->request_code );
  XGetErrorDatabaseText( dpy, "XRequest", number, "", buffer, MAXNAMELEN );
 ( void )fprintf(fp, " (%s)", buffer );
  fputs("\n  ", fp );
  XGetErrorDatabaseText( dpy, mtype, "MinorCode", "Request Minor code",
                         mesg, MAXNAMELEN );
  ( void )fprintf( fp, mesg, event->minor_code );
  fputs("\n  ", fp );
  XGetErrorDatabaseText( dpy, mtype, "ResourceID", "ResourceID 0x%x",
                         mesg, MAXNAMELEN );
  ( void )fprintf(fp, mesg, event->resourceid );
  fputs("\n  ", fp );
  XGetErrorDatabaseText( dpy, mtype, "ErrorSerial", "Error Serial #%d",
                         mesg, MAXNAMELEN );
  ( void )fprintf( fp, mesg, event->serial );
   fputs("\n  ", fp );
  XGetErrorDatabaseText( dpy, mtype, "CurrentSerial", "Current Serial #%d",
                         mesg, MAXNAMELEN );
  ( void )fprintf( fp, mesg, NextRequest(dpy)-1 );
fputs( "\n", fp );
        abort();
*/
return(0);
}

static void
error_open(Calendar *c) {
	char		buf[MAXNAMELEN], buf2[MAXNAMELEN];
	char		*name, *host;
	Props_pu	*p = (Props_pu *)c->properties_pu;

	char *nl_user = XtNewString(catgets(c->DT_catd, 1, 92, "User name"));
	char *nl_host = XtNewString(catgets(c->DT_catd, 1, 93, "Host"));
	char *ident = XtNewString(catgets(c->DT_catd, 1, 95, "Continue"));
	char *title = 
	  XtNewString(catgets(c->DT_catd, 1, 94, "Calendar : Error"));

	name = cm_target2name(c->calname);
	host = cm_target2host(c->calname);

	sprintf(buf, "%s %s", catgets(c->DT_catd, 1, 90,
			"Error opening Calendar file"), c->calname);
	set_message(c->message_text, buf);

	sprintf(buf, "%s\n%s: %s, %s: %s", catgets(c->DT_catd, 1, 91,
				   		"rpc.cmsd is not responding for your user name.\nMake sure the inetd process is running and the entry\nin inetd.conf for rpc.cmsd is correct for your host."),
		nl_user, name,
		nl_host, host);

	dialog_popup(c->frame,
		DIALOG_TITLE, title,
		DIALOG_TEXT, buf,
		BUTTON_IDENT, 1, ident,
		BUTTON_HELP, CMSD_ERROR_HELP,
		DIALOG_IMAGE, p->xm_error_pixmap,
		NULL);

	XtFree(title);
	XtFree(ident);
	XtFree(nl_host);
	XtFree(nl_user);

	free(name);
	free(host);
}

static void
error_noloc(Calendar *c, char *name) 
{
        char            buf[BUFSIZ];
        Props_pu        *p = (Props_pu *)c->properties_pu;

	char *ident = XtNewString(catgets(c->DT_catd, 1, 95, "Continue"));
	char *title = 
	  XtNewString(catgets(c->DT_catd, 1, 94, "Calendar : Error"));

        sprintf(buf, "%s %s",
		catgets(c->DT_catd, 1, 108, "No Calendar Location specified for"),
		name);
        set_message(c->message_text, buf);

	sprintf(buf, "%s", catgets(c->DT_catd, 1, 109, "No location specified; add a hostname to the\nInitial Calendar View in Properties/Display Settings.  If\nyou ran Calendar (dtcm) with the -c option, verify you specified a hostname."));
        dialog_popup(c->frame,
                DIALOG_TITLE, title,
                DIALOG_TEXT, buf,
                BUTTON_IDENT, 1, ident,
		BUTTON_HELP, CAL_LOCATION_ERROR_HELP,
                DIALOG_IMAGE, p->xm_error_pixmap,
                NULL);
	XtFree(title);
	XtFree(ident);
}

/*
 * this routine is used to logon to user's own calendar.
 * If the calendar does not exist, it will create it and logon to it.
 * It then registers callback for updates.
 * If it fails to logon due to server timeout and retry is True,
 * it will set a timer to retry.
 */
static Boolean
open_user_calendar(Calendar *c, Boolean retry)
{
	char		buf[MAXNAMELEN], *loc, *user;
	CSA_return_code	status;
	CSA_calendar_user csa_user;
	CSA_flags	flags;
	CSA_extension	cb_ext;
	CSA_extension	logon_ext;
	CSA_attribute	attr;
	CSA_attribute_value val;
	Boolean		viewother;

	c->general->version = 0;
	if ((loc = cm_target2location(c->calname)) == NULL) {
		error_noloc(c, c->calname);
		return False;
	}
	if ((user = cm_target2name(c->calname)) == NULL) {
		error_noloc(c, c->calname);
		free(loc);
		return False;
	}

#ifdef FNS
	if (cmfns_use_fns((Props *)c->properties)) {
		cm_register_calendar(c->calname, loc);
	}
#endif

	csa_user.user_name = c->calname;
	csa_user.user_type = 0;
	csa_user.calendar_user_extensions = NULL;
	csa_user.calendar_address = c->calname;

	logon_ext.item_code = CSA_X_DT_GET_USER_ACCESS_EXT;
	logon_ext.item_data = 0;
	logon_ext.item_reference = NULL;
	logon_ext.extension_flags = CSA_EXT_LAST_ELEMENT;

	status = csa_logon(NULL, &csa_user, NULL, NULL, NULL,
			&c->my_cal_handle, &logon_ext);

	if (status == CSA_E_CALENDAR_NOT_EXIST) {
		/* get common locale/charset name */
		attr.name = CSA_CAL_ATTR_CHARACTER_SET;
		attr.value = &val;
		attr.attribute_extensions = NULL;
		val.type = CSA_VALUE_STRING;
		val.item.string_value = NULL;
		_DtI18NXlateOpToStdLocale(DtLCX_OPER_SETLOCALE,
			setlocale(LC_CTYPE, NULL), &val.item.string_value,
			NULL, NULL);

		/* create the calendar */
		if ((status = csa_add_calendar(0, &csa_user, 1, &attr, NULL))
		    == CSA_E_UNSUPPORTED_ATTRIBUTE) {
			/* server with version 4 or less does not support
			 * the CSA_CAL_ATTR_CHARACTER_SET cal attribute
			 */
			status = csa_add_calendar(0, &csa_user, 0, NULL,
					NULL);
		}
		if (val.item.string_value) free(val.item.string_value);

		if (status == CSA_SUCCESS)
			status = csa_logon(NULL, &csa_user, NULL, NULL, NULL,
					&c->my_cal_handle, &logon_ext);
	}

	free(loc);
	free(user);

	viewother = strcmp(c->calname, c->view->current_calendar);

	if (status != CSA_SUCCESS) {
		if (status == CSA_X_DT_E_SERVER_TIMEOUT && retry) {
			XtAppAddTimeOut(c->xcontext->app,
				(viewother ? 15*1000 : 0), logon_retry, c);
			return False;
		} else {
			error_open(c);
			return False;
		}
	} else
		c->my_access = logon_ext.item_data;

	c->my_cal_version = get_data_version(c->my_cal_handle);

	flags = CSA_CB_ENTRY_ADDED | CSA_CB_ENTRY_DELETED |
		CSA_CB_ENTRY_UPDATED;
	cb_ext.item_code = CSA_X_XT_APP_CONTEXT_EXT;
	cb_ext.item_data = (CSA_uint32)c->xcontext->app;
	cb_ext.extension_flags = CSA_EXT_LAST_ELEMENT;
	csa_register_callback(c->my_cal_handle, flags, update_handler,
			NULL, &cb_ext);

	if (c->my_cal_version < DTCM_DATA_VER)
		set_message(c->message_text, catgets(c->DT_catd, 1, 186,
		       "Calendar (dtcm) and rpc.cmsd versions are different."));

	if (!viewother) {
		/* NL_COMMENT

		   Message 113 : ``Calendar'' is used in the main titlebar 
		   of the tool: Calendar : user@host.
		 */
		sprintf(buf, "%s : %s", catgets(c->DT_catd, 1, 113, "Calendar"),
					c->calname);
        	XtVaSetValues(c->frame, XmNtitle, buf, NULL);
	}

	return True;
}

static Boolean
open_initial_calendar(
	Calendar 	*c,
	CSA_return_code *status)
{
	char		buf[MAXNAMELEN], *loc, *user;
	CSA_flags	flags = NULL;
	CSA_extension	cb_ext;
	CSA_extension	logon_ext;
	CSA_calendar_user csa_user;

	if ((user = cm_target2name(c->view->current_calendar)) == NULL) {
		error_noloc(c, c->view->current_calendar);
		return False;
	}

	if ((loc = cm_target2location(c->view->current_calendar)) == NULL) {
		error_noloc(c, c->view->current_calendar);
		return False;
	}

	csa_user.user_name = c->view->current_calendar;
	csa_user.user_type = 0;
	csa_user.calendar_user_extensions = NULL;
	csa_user.calendar_address = c->view->current_calendar;

	logon_ext.item_code = CSA_X_DT_GET_USER_ACCESS_EXT;
	logon_ext.item_data = 0;
	logon_ext.item_reference = NULL;
	logon_ext.extension_flags = CSA_EXT_LAST_ELEMENT;

	*status = csa_logon(NULL, &csa_user, NULL, NULL, NULL, &c->cal_handle,
			&logon_ext);
	free(loc);
	free(user);
	if (*status != CSA_SUCCESS)
		return False; 
	else
		c->user_access = logon_ext.item_data;

	/*
	 * Get the version number of the new calendar
	 */

	c->general->version = get_data_version(c->cal_handle);

	if (c->general->version < DTCM_DATA_VER)
		set_message(c->message_text, catgets(c->DT_catd, 1, 186,
		       "Calendar (dtcm) and rpc.cmsd versions are different."));


	flags = CSA_CB_ENTRY_ADDED | CSA_CB_ENTRY_DELETED |
		CSA_CB_ENTRY_UPDATED;
	cb_ext.item_code = CSA_X_XT_APP_CONTEXT_EXT;
	cb_ext.item_data = (CSA_uint32)c->xcontext->app;
	cb_ext.extension_flags = CSA_EXT_LAST_ELEMENT;
	csa_register_callback(c->cal_handle, flags,
			update_handler, NULL, &cb_ext);

	sprintf(buf, "%s : %s", catgets(c->DT_catd, 1, 113, "Calendar"),
					c->view->current_calendar);
        XtVaSetValues(c->frame, XmNtitle, buf, NULL);

	return True;
}

/*
 * try to logon to user's own calendar
 */
static void
logon_retry(XtPointer data, XtIntervalId *dummy)
{
	Calendar	*c = (Calendar *)data;
	/* retry once */
	if (c->my_cal_handle == 0 && (open_user_calendar(c, False) == True))
	{
		_DtTurnOnHourGlass(c->frame);
		reset_timer(c);
		if (!strcmp(c->calname, c->view->current_calendar)) {
			c->cal_handle = c->my_cal_handle;
			c->user_access = c->my_access;
			c->general->version = c->my_cal_version;
			invalidate_cache(c);
			paint_canvas((Calendar *)data, NULL, RENDER_UNMAP);
		}
		_DtTurnOffHourGlass(c->frame);
	}
}

static void
view_event(Widget w, XtPointer client_data, XtPointer cbs)
{
        Calendar *c = calendar;
	XEvent	*event = (XEvent *) client_data;

        if (event == NULL)
                return;
        if (c->view->glance == monthGlance)
                month_event(event);
        else if (c->view->glance == weekGlance)
                week_event(event);
        else if (c->view->glance == dayGlance)
                day_event(event);
/* REVISIT - dac
        else if (c->view->glance == yearGlance)
                year_event(event);
*/
}

static void
cm_usage()
{
	(void)fprintf(stderr, "Usage: dtcm [ -c calendar ] [-v view ]");
	(void)fprintf(stderr, " [ -p printer ] [ generic-tool-arguments ]\n" );
	if (child != 0)
		(void)kill(child, SIGKILL);
	exit(1);
}

/*
 * Calculation of default calendar name for the application
 * Must return srting value in val_ret->addr
 * Invoked by the XtRCallProc resource initialization mechanism.
 */
static void
def_cal(Widget w, int offset, XrmValue *val_ret)
{
        val_ret->addr = cm_strdup(get_char_prop((Props *)calendar->properties, 
								CP_DEFAULTCAL));
}

static void
def_view(Widget w, int offset, XrmValue *val_ret)
{
	ViewType value_p;

	value_p = get_int_prop((Props *)calendar->properties, CP_DEFAULTVIEW);
	switch (value_p) {
		case YEAR_VIEW:
		 	val_ret->addr = cm_strdup("year");
			break;
		case MONTH_VIEW:
		 	val_ret->addr = cm_strdup("month");
			break;
		case WEEK_VIEW:
		 	val_ret->addr = cm_strdup("week");
			break;
		case DAY_VIEW:
		 	val_ret->addr = cm_strdup("day");
			break;
	}
}

/*
 * Command line options definition
 */
static XrmOptionDescRec options[] = {
	{ "-v", "__defaultView", XrmoptionSepArg, NULL },
	{ "-c", "__defaultCalendar", XrmoptionSepArg, NULL },
	{ "-p", "__defaultPrinter", XrmoptionSepArg, NULL },
	{ "-trace", "__trace", XrmoptionNoArg, (caddr_t)"FALSE" },
	{ "-session", "__session", XrmoptionSepArg, NULL }
};

/*
 * Application-defined resources
 */
static XtResource resources[] = {
/* initial view */
	{ "__defaultView", "__DefaultView", XtRString, sizeof(String),
	XtOffset(DtCmAppResourcesPtr, default_view), XtRCallProc, 
	(XtPointer)def_view},
/* default calendar */
	{ "__defaultCalendar", "__DefaultCalendar", XtRString, sizeof(String),
	XtOffset(DtCmAppResourcesPtr, default_calendar), XtRCallProc,
	(XtPointer)def_cal},
/* default printer */
	{ "__defaultPrinter", "__DefaultPrinter", XtRString, sizeof(String),
	XtOffset(DtCmAppResourcesPtr, default_printer), XtRString,
	(XtPointer)NULL},
/* label font */
	{ "labelFont", "LabelFont", XmRFontList, sizeof(XmFontList),
	XtOffset(DtCmAppResourcesPtr, labelfontlist), XtRString,
	NULL},
	{ "labelFont", "LabelFont", XtRString, sizeof(String),
	XtOffset(DtCmAppResourcesPtr, labelfont), XtRString,
	NULL},
/* view font */
	{ "viewFont", "ViewFont", XmRFontList, sizeof(XmFontList),
	XtOffset(DtCmAppResourcesPtr, viewfontlist), XtRString,
	NULL},
/* user font (backup for view font */
	{ "userFont", "UserFont", XtRString, sizeof(String),
	XtOffset(DtCmAppResourcesPtr, userfont), XtRString,
	"-b&h-lucidatypewriter-medium-r-*-sans-*-120-*-*-*-*-*-*:"},
	{ "userFont", "UserFont", XmRFontList, sizeof(XmFontList),
	XtOffset(DtCmAppResourcesPtr, userfontlist), XtRString,
	"-b&h-lucidatypewriter-medium-r-*-sans-*-120-*-*-*-*-*-*:"},
/* system font (backup for label font */
	{ "systemFont", "SystemFont", XmRFontList, sizeof(XmFontList),
	XtOffset(DtCmAppResourcesPtr, systemfontlist), XtRString,
	"-b&h-lucida-medium-r-*-sans-*-120-*-*-*-*-*-*:"},
	{ "systemFont", "SystemFont", XtRString, sizeof(String),
	XtOffset(DtCmAppResourcesPtr, systemfont), XtRString,
	"-b&h-lucida-medium-r-*-sans-*-120-*-*-*-*-*-*:"},
/* bold font */
	{ "boldFont", "BoldFont", XmRFontList, sizeof(XmFontList),
	XtOffset(DtCmAppResourcesPtr, boldfontlist), XtRString,
	NULL},
/* font for icon */
        { "iconFont", "IconFont", XmRFontList, sizeof(XmFontList), 
        XtOffset(DtCmAppResourcesPtr, iconfontlist), XtRString, 
        NULL}, 
/* application font family */
	{ "applicationFontFamily", "ApplicationFontFamily", XtRString, 
	sizeof(XmString), XtOffset(DtCmAppResourcesPtr, app_font_family), 
	XtRString, "application"},
/* Session file from -session option */
	{ "__session", "__Session", XtRString, 
	sizeof(XmString), XtOffset(DtCmAppResourcesPtr, session_file), 
	XtRString, NULL},
/* trace flag */
	{ "__trace", "__trace", XtRBoolean, 
	sizeof(Boolean), XtOffset(DtCmAppResourcesPtr, debug), 
	XtRBoolean, FALSE},
};

/*
 * This routine returns the user's calendar in the format, user@location.
 * "user" is the user's login name.  If the calendar location is not
 * specified in the "user calendar location" option, "location" defaults
 * to the local host.
 */
char *
get_user_calendar()
{
	char	*name, *uname, *loc;
	Boolean	needfree = False;

	uname = cm_get_uname();
	if ((loc = get_char_prop((Props *)calendar->properties, CP_CALLOC)) &&
	    *loc != '\0') {
		loc = strdup(loc);
		needfree = True;
	} else
		loc = cm_get_local_host();

	name = malloc(strlen(uname) + strlen(loc) + 2);
	sprintf(name, "%s@%s", uname, loc);

	if (needfree) free(loc);

	return (name);
}

static void
init_calendar(argc, argv)
	int argc; char **argv;
{
	int start, stop;
	register int i;
	register char *s_ptr, *d_ptr;
	Props *p;
	Tick today;
	char bind_home[MAXPATHLEN];
	XRectangle clip;
	Dimension w, h, label_width;
        String translations =
        	"<Btn1Down>: view_event()               \n\
         	<Btn1Motion>: view_event()";
        static XtActionsRec action_table[] = {
                {(String) "view_event", (XtActionProc) view_event},
		{(String) "TranslationDragStart", 
		   TranslationDragStart},
		{"dtcm-process-press",
		   DtcmProcessPress}
        };
	DtCmAppResources app_data;
	Atom xa_WM_SAVE_YOURSELF;
	unsigned long valuemask;
	XSetWindowAttributes attrs;
	Arg al[20];
	int ac=0;
	Tt_status status;

	init_time();
	today = now();
	calendar = (Calendar*) ckalloc(sizeof(Calendar));
	calendar->view = (View*) ckalloc(sizeof(View));
	calendar->general = (General*) ckalloc(sizeof(General));
	calendar->view->nwks = numwks(today);
	calendar->app_data = (DtCmAppResourcesPtr)
		ckalloc(sizeof(DtCmAppResources));
	calendar->editor = (caddr_t)ckalloc(sizeof(Editor));
	calendar->geditor = (caddr_t)ckalloc(sizeof(GEditor));
	calendar->todo = (caddr_t)ckalloc(sizeof(ToDo));
	calendar->browselist = (caddr_t)ckalloc(sizeof(Browselist));
	calendar->fonts = (Cal_Fonts *) ckalloc(sizeof(Cal_Fonts));
	calendar->fonts->labelfont = (Cal_Font *) ckalloc(sizeof(Cal_Font));
	calendar->fonts->viewfont = (Cal_Font *) ckalloc(sizeof(Cal_Font));
	calendar->fonts->boldfont = (Cal_Font *) ckalloc(sizeof(Cal_Font));
	calendar->fonts->iconfont = (Cal_Font *) ckalloc(sizeof(Cal_Font));
	calendar->fonts->userfont = (Cal_Font *) ckalloc(sizeof(Cal_Font));

	/*
	 * Save the argv list into a WM_COMMAND format string for
	 * later use when the application is asked to save itself.  
	 * The format is a buffer with a series of null terminated 
	 * strings within it, one for each string in the argv list.  
	 * Thus the total length is that of the arguments plus a null 
	 * byte for each argument.
	 */
	for (i = 0, calendar->view->wm_cmdstrlen = 0; i < argc; i++) {
		/* Do not record the -session option as we add that
		 * automatically when we save the session.
		 */
		if (!strcmp("-session", argv[i])) {
			i++;
			continue;
		}
		calendar->view->wm_cmdstrlen += strlen(argv[i]) + 1;
	}

	d_ptr = calendar->view->wm_cmdstr =
					malloc(calendar->view->wm_cmdstrlen);
	for (i = 0; i < argc; i++)
	{
		char *s_ptr = argv[i];

		/* Do not record the -session option as we add that
		 * automatically when we save the session.
		 */
		if (!strcmp("-session", argv[i])) {
			i++;
			continue;
		}
		
		while (*s_ptr)
			*d_ptr++ = *s_ptr++;
		*d_ptr++ = '\0';
	}
	d_ptr = NULL;

	p = (Props *)ckalloc(sizeof(Props));
	calendar->properties = (caddr_t)p;
	calendar->properties_pu = (caddr_t) ckalloc(sizeof(Props_pu));
	calendar->items = (Items *) ckalloc(sizeof(Items));
	read_props(p);
	cal_convert_cmrc(p);
	if ((start = get_int_prop(p, CP_DAYBEGIN)) < 0)
		start = 0;
	else if (start > 23)
		start = 23;
	if ((stop = get_int_prop(p, CP_DAYEND)) <= start)
		stop = start + 1;
	else if (stop > 24)
		stop = 24;
	set_int_prop(p, CP_DAYBEGIN, start);
	set_int_prop(p, CP_DAYEND, stop);

        calendar->frame = XtVaAppInitialize(&app, "Dtcm",
				options, XtNumber(options), &argc, argv,
				fallback_resources,
				XmNwidth, 650, XmNheight, 730, 
				XmNmappedWhenManaged, False,
				NULL);

	if (argc > 1)
		cm_usage();

	dpy = XtDisplayOfObject(calendar->frame); 

	/*
	 * Initialize DtSvc for drag and drop stuff
	 */
	DtInitialize(dpy, calendar->frame, argv[0], "Dtcm");

	/*
	 * Add X error handler to handle BadAlloc errors on the print server.
	 */
	oldXErrorHandler = XSetErrorHandler(newXErrorHandler);

	/*
	 * Load the application resources and set the quit handler for the main
	 * frame.
	 */
	XtGetApplicationResources(calendar->frame, calendar->app_data,
		resources, XtNumber(resources), NULL, 0);

	/* Handle the -session option */
	if (calendar->app_data->session_file)
		GetSessionInfo(calendar);

	if (!init_fonts(calendar)) {
		fprintf (stderr, "%s: Failed to find required fonts ", argv[0]);
		fprintf (stderr, "(``fixed'' and ``variable'')...exiting.\n"); 
		exit(-1);
	}

	setup_quit_handler(calendar->frame, quit_handler, (caddr_t)calendar);

	/*
	 * Setup the handlers for the saving of session information
	 */
	xa_WM_SAVE_YOURSELF = XInternAtom(dpy, "WM_SAVE_YOURSELF", False);
	XmAddWMProtocolCallback(calendar->frame, xa_WM_SAVE_YOURSELF,
                                CMSaveSessionCB, NULL);

	/*
	 * Must call this to cause Motif to recognise tear_off_enabled in a
	 * resource file.  Mmm... :-)
	 */
	XmRepTypeInstallTearOffModelConverter();

	calendar->view->date = today;
	calendar->view->outside_margin = VIEWMARGIN;
	calendar->view->current_selection = (caddr_t)ckalloc(sizeof(Selection));
	calendar->user = cm_get_credentials();

#if 0 	/* use this when we go with mapped names */
	cm_get_yptarget(cm_get_uname(), &calendar->calname);
	if (calendar->calname == NULL) 
		/* No mapping in NIS+ db */
		calendar->calname = cm_get_deftarget();
#endif

	calendar->calname = get_user_calendar();
	calendar->view->current_calendar =
		cm_strdup(calendar->app_data->default_calendar);

 	/* Open the message catalog for internationalization */
	calendar->DT_catd = catopen(DTCM_CAT, NL_CAT_LOCALE);

        ac=0;
	XtSetArg(al[ac], XmNfractionBase, 100); ac++;
	XtSetArg(al[ac], XmNmarginWidth, 0); ac++;
	calendar->form = XmCreateForm(calendar->frame, "form", al, ac);

	make_menus(calendar);
	create_all_pixmaps((Props_pu *)calendar->properties_pu,
			   calendar->menu_bar);
	make_buttons(calendar);

	ac=0;
	XtSetArg(al[ac], XmNtranslations, 
			XtParseTranslationTable(translations)); ac++;
	XtSetArg(al[ac], XmNresizePolicy, XmRESIZE_ANY); ac++;
	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNtopWidget, calendar->previous); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftOffset, 1); ac++;
      	calendar->canvas = XmCreateDrawingArea(calendar->form, "canvas",
				al, ac);
        XtAddCallback(calendar->canvas, XmNhelpCallback, 
					(XtCallbackProc)help_view_cb, NULL);
	ac=0;
	XtSetArg(al[ac], XmNalignment, XmALIGNMENT_BEGINNING); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNmarginHeight, 0); ac++;
	calendar->message_text = XmCreateLabelGadget(calendar->form, "message",
						     al, ac);
	set_message(calendar->message_text, " ");

	XtVaSetValues(calendar->canvas,
			XmNbottomAttachment, XmATTACH_WIDGET,
			XmNbottomWidget, calendar->message_text,
			NULL);

        XtAppAddActions(app, (XtActionList) &action_table,
			XtNumber(action_table));

	ManageChildren(calendar->form);
        XtManageChild(calendar->form);

/*
 * Enforce a "reasonable" size.  Things are unreadable and ugly any
 * smaller than 400x400, including the grids.  (The text will likely
 * be unreadable even at this size, but it's all a matter of giving
 * enough rope.  No particular reason for the numbers used for maximums.
 */
	XtRealizeWidget(calendar->frame);

        XtVaGetValues(calendar->frame, XmNwidth, &w, XmNheight, &h, NULL);
	XtVaSetValues(calendar->frame,
		XmNminWidth, min(400,w),
		XmNminHeight, min(450,h),
		NULL);

	XtMapWidget(calendar->frame);

        XtAddCallback(calendar->canvas, XmNresizeCallback, resize_proc, NULL);
        XtAddCallback(calendar->canvas, XmNinputCallback, view_event, NULL);
        XtAddCallback(calendar->canvas, XmNexposeCallback, repaint_proc, NULL);

	cm_register_drop_site(calendar, calendar->form);

	calendar->xcontext = gr_create_xcontext(calendar, calendar->canvas,
						gr_color, app);

	gr_init(calendar->xcontext, calendar->canvas);
	set_default_view(calendar);

	init_strings();   /* strings in timeops.c */

	calendar->view->day_info = (caddr_t) ckalloc(sizeof(Day));
	init_mo(calendar);

#ifdef FNS
	/* Initialized FNS */
	dtfns_init();
#endif

	status = cmtt_init("Calendar", calendar, app, calendar->frame);
	if (TT_OK != status) {
	    char *errfmt;
    	    errfmt = catgets(calendar->DT_catd, 2, 2,
			"Could not connect to ToolTalk:\n%s\n");
            DieFromToolTalkError( calendar, errfmt, status );
	}

	/* 
	 * This needs to be done before we open the calendar because
	 * if we fail to open the calendar an error dialog is displayed.
	 * This causes the main canvas to be painted but this doesn't
	 * happen correctly if we haven't cached the diminsions.
	 */
	XtVaGetValues(calendar->canvas, XmNwidth, &w, XmNheight, &h, NULL);
	cache_dims(calendar, w, h);

	/* 
	 * Paint the icon before we log into the calendar to avoid the
	 * user seeing the default icon while the logon process is
	 * occurring.
	 */
        paint_icon(calendar);

	/*
	 * set version of cms and register client
	 */
	_DtTurnOnHourGlass(calendar->frame);

	if (open_user_calendar(calendar, True) == True)
		reset_timer(calendar);

	if (strcmp(calendar->calname, calendar->view->current_calendar)) {
		CSA_return_code 	status = CSA_SUCCESS;

		if (open_initial_calendar(calendar, &status) == False) {
			/* failed to logon to the initial calendar,
			 * display user's own calendar instead
			 */
			free(calendar->view->current_calendar);
			calendar->view->current_calendar =
				cm_strdup(calendar->calname);

			if (calendar->my_cal_handle) {
				char	buf[MAXNAMELEN];

				calendar->cal_handle = calendar->my_cal_handle;
				calendar->user_access = calendar->my_access;
				calendar->general->version =
					calendar->my_cal_version;
				sprintf(buf, "%s : %s", 
					catgets(calendar->DT_catd, 1, 113, 
								"Calendar"),
					calendar->calname);
        			XtVaSetValues(calendar->frame, XmNtitle, buf,
					NULL);
			}
			backend_err_msg(calendar->frame, 
				calendar->view->current_calendar, status,
				((Props_pu *) 
				     calendar->properties_pu)->xm_error_pixmap);
		}
	} else {
		calendar->cal_handle = calendar->my_cal_handle;
		calendar->user_access = calendar->my_access;
		calendar->general->version = calendar->my_cal_version;
	}

	signal(SIGINT, sig_int_handler);

	/* A cache will be allocated if error dialog has been displayed
	 * this call is to clear the cache so that the main view
	 * can be properly updated
	 */
	invalidate_cache(calendar);

	_DtTurnOffHourGlass(calendar->frame);

	setup_new_day_handler(calendar);
	center_today_button(calendar);

	/* Twiddle controls for old versions */

	if (calendar->general->version && calendar->general->version < DATAVER2)
	{
		if (todo_showing((ToDo *)calendar->todo))
			XtPopdown(((ToDo *)calendar->todo)->frame);
		XtUnmanageChild(calendar->todo_btn);
		XtSetSensitive(calendar->todo_edit_menu, False);
		XtSetSensitive(calendar->todo_view_menu, False);
	}
	else {
		XtManageChild(calendar->todo_btn);
		XtSetSensitive(calendar->todo_edit_menu, True);
		XtSetSensitive(calendar->todo_view_menu, True);
	}
}

#ifdef __osf__
extern void
sigchld_handler(int signo)      /* Do not use the arg signo at the moment */
{
        pid_t   pid;
        int     stat_loc;

        pid = waitpid(-1, &stat_loc, WNOHANG);
        /* Child exit handling code follows, if any */
}
#endif /* __osf__ */

static int
newXErrorHandler(Display *dsp, XErrorEvent *event)
{
    if ((event->error_code == BadAlloc) &&
	(dsp == pd_get_print_display(calendar)))
    {
	pd_set_bad_alloc_error(calendar, True);
	return 0;
    }

    return (*oldXErrorHandler)(dsp, event);
}

int
main(int argc, char **argv) {
/* Handle SIGCHLD for dtcm */

        struct sigaction sa, osa;

#ifdef __osf__
        sa.sa_handler = sigchld_handler;
        sa.sa_flags   =  0;
#else
	sa.sa_handler = SIG_IGN;
# ifdef SA_NOCLDWAIT
	sa.sa_flags   =  SA_NOCLDWAIT;
# else
        sa.sa_flags   =  0;
# endif
#endif
        sigemptyset(&sa.sa_mask);

        sigaction(SIGCHLD, &sa, &osa);

	XtSetLanguageProc(NULL, NULL, NULL);
	_DtEnvControl(DT_ENV_SET); /* set up environment variables */

        init_calendar(argc, argv);

        XtAppMainLoop (app);

        exit(0);

        return(0);
}

extern CSA_return_code
paint_canvas(
	Calendar 	*c, 
	XRectangle 	*rect,
	Render_Type	 render_type)
{
/* REPAINT, CLEAR_FIRST, UNMAP */ 
        Glance glance = c->view->glance;
	CSA_return_code stat = CSA_SUCCESS;

        switch(glance) {
        case monthGlance:
		switch (render_type) {
		case RENDER_REPAINT:
                	repaint_damaged_month(c, rect);
			break;
		case RENDER_CLEAR_FIRST:
			gr_clear_area(c->xcontext, 0, 0, 
				      c->view->winw, c->view->winh);
                	repaint_damaged_month(c, rect);
			break;
		case RENDER_UNMAP:
			XtUnmapWidget(c->canvas);
                	prepare_to_paint_monthview(c, rect);
			XtMapWidget(c->canvas);
			break;
		}
                break;
        case yearGlance:
                stat = paint_year(c);
                break;
        case weekGlance:
                paint_weekview(c, rect);
                break;
        case dayGlance:
		paint_day(c);
                break;
        default:
		stat = CSA_SUCCESS;
                paint_grid(c, rect);
                break;
        }
	return stat;
}

#ifdef notdef
extern char*
cm_get_relname()
{
        char s[BUFSIZ];
        static char *CM_name;

        /* we now make a composite name for the tool, combining
         * "Calendar Manager" with a release identifier
         */
	if (CM_name == NULL) {
                sprintf(s, "%s", catgets(calendar->DT_catd, 1, 113, "Calendar"));
        	CM_name = (char*)ckalloc(cm_strlen(s) + 2);
	 
        	sprintf(CM_name, "%s", s);
	}
 
        return (char*)CM_name;
}
#endif 

extern Boolean
in_range(time_t start, time_t stop, time_t tick) {
        if (tick >= start && tick <= stop)
                return True;
        return False;
}

extern Boolean
today_inrange(Calendar *c, time_t day_in_range) {
	time_t	start, stop;
	Boolean	inrange = False;

	/* is today in range of current view? */
        get_range(c->view->glance, day_in_range, &start, &stop);
        inrange = in_range(start, stop, time(0));
	return inrange;
}
void
init_strings()
{
	char *display_lang="C";

	months[1] = strdup(catgets(calendar->DT_catd, 1, 114, "January"));
	months[2] = strdup(catgets(calendar->DT_catd, 1, 115, "February"));
	months[3] = strdup(catgets(calendar->DT_catd, 1, 116, "March"));
	months[4] = strdup(catgets(calendar->DT_catd, 1, 117, "April"));
	months[5] = strdup(catgets(calendar->DT_catd, 1, 118, "May"));
	months[6] = strdup(catgets(calendar->DT_catd, 1, 119, "June"));
	months[7] = strdup(catgets(calendar->DT_catd, 1, 120, "July"));
	months[8] = strdup(catgets(calendar->DT_catd, 1, 121, "August"));
	months[9] = strdup(catgets(calendar->DT_catd, 1, 122, "September"));
	months[10] = strdup(catgets(calendar->DT_catd, 1, 123, "October"));
	months[11] = strdup(catgets(calendar->DT_catd, 1, 124, "November"));
	months[12] = strdup(catgets(calendar->DT_catd, 1, 125, "December"));

	months2[1] = strdup(catgets(calendar->DT_catd, 1, 126, "Jan"));
	months2[2] = strdup(catgets(calendar->DT_catd, 1, 127, "Feb"));
	months2[3] = strdup(catgets(calendar->DT_catd, 1, 128, "Mar"));
	months2[4] = strdup(catgets(calendar->DT_catd, 1, 129, "Apr"));
	months2[5] = strdup(catgets(calendar->DT_catd, 1, 130, "May"));
	months2[6] = strdup(catgets(calendar->DT_catd, 1, 131, "Jun"));
	months2[7] = strdup(catgets(calendar->DT_catd, 1, 132, "Jul"));
	months2[8] = strdup(catgets(calendar->DT_catd, 1, 133, "Aug"));
	months2[9] = strdup(catgets(calendar->DT_catd, 1, 134, "Sep"));
	months2[10] = strdup(catgets(calendar->DT_catd, 1, 135, "Oct"));
	months2[11] = strdup(catgets(calendar->DT_catd, 1, 136, "Nov"));
	months2[12] = strdup(catgets(calendar->DT_catd, 1, 137, "Dec"));
	
	/* NL_COMMENT
	   Attention Translator:

	   The strings (message number 138-144) are abbreviations to
	   the days of the week:

		Sun --> Sunday
		Mon --> Monday
		Tue --> Tuesday
		Wed --> Wednesday
		Thu --> Thursday
		Fri --> Friday
		Sat --> Saturday
		Sun --> Sunday

	*/
	days[0] = strdup(catgets(calendar->DT_catd, 1, 138, "Sun"));
	days[1] = strdup(catgets(calendar->DT_catd, 1, 139, "Mon"));
	days[2] = strdup(catgets(calendar->DT_catd, 1, 140, "Tue"));
	days[3] = strdup(catgets(calendar->DT_catd, 1, 141, "Wed"));
	days[4] = strdup(catgets(calendar->DT_catd, 1, 142, "Thu"));
	days[5] = strdup(catgets(calendar->DT_catd, 1, 143, "Fri"));
	days[6] = strdup(catgets(calendar->DT_catd, 1, 144, "Sat"));
	days[7] = strdup(catgets(calendar->DT_catd, 1, 138, "Sun"));

	days2[0] = strdup(catgets(calendar->DT_catd, 1, 146, "Sunday"));
	days2[1] = strdup(catgets(calendar->DT_catd, 1, 147, "Monday"));
	days2[2] = strdup(catgets(calendar->DT_catd, 1, 148, "Tuesday"));
	days2[3] = strdup(catgets(calendar->DT_catd, 1, 149, "Wednesday"));
	days2[4] = strdup(catgets(calendar->DT_catd, 1, 150, "Thursday"));
	days2[5] = strdup(catgets(calendar->DT_catd, 1, 151, "Friday"));
	days2[6] = strdup(catgets(calendar->DT_catd, 1, 152, "Saturday"));
	days2[7] = strdup(catgets(calendar->DT_catd, 1, 146, "Sunday"));

	/* NL_COMMENT
	   Attention Translator:

	   The strings (message number 154-160, 168) are one letter 
	   abbreviations to the days of the week:

		S --> Sunday (message 154)
		M --> Monday
		T --> Tuesday
		W --> Wednesday
		T --> Thursday
		F --> Friday
		S --> Saturday (message 160)

	*/
	days3[0] = strdup(catgets(calendar->DT_catd, 1, 154, "S"));
	days3[1] = strdup(catgets(calendar->DT_catd, 1, 155, "M"));
	days3[2] = strdup(catgets(calendar->DT_catd, 1, 156, "T"));
	days3[3] = strdup(catgets(calendar->DT_catd, 1, 157, "W"));
	days3[4] = strdup(catgets(calendar->DT_catd, 1, 158, "T"));
	days3[5] = strdup(catgets(calendar->DT_catd, 1, 159, "F"));
	days3[6] = strdup(catgets(calendar->DT_catd, 1, 160, "S"));
	days3[7] = strdup(catgets(calendar->DT_catd, 1, 154, "S"));

	/* NL_COMMENT
	   Attention Translator:

	   The strings (message number 170-176) are abbreviations to
	   the days of the week:

		SUN --> Sunday
		MON --> Monday
		TUE --> Tuesday
		WED --> Wednesday
		THU --> Thursday
		FRI --> Friday
		SAT --> Saturday

	*/
	days4[0] = strdup(catgets(calendar->DT_catd, 1, 170, "SUN"));
	days4[1] = strdup(catgets(calendar->DT_catd, 1, 171, "MON"));
	days4[2] = strdup(catgets(calendar->DT_catd, 1, 172, "TUE"));
	days4[3] = strdup(catgets(calendar->DT_catd, 1, 173, "WED"));
	days4[4] = strdup(catgets(calendar->DT_catd, 1, 174, "THU"));
	days4[5] = strdup(catgets(calendar->DT_catd, 1, 175, "FRI"));
	days4[6] = strdup(catgets(calendar->DT_catd, 1, 176, "SAT"));
	days4[7] = strdup(catgets(calendar->DT_catd, 1, 170, "SUN"));
}


void
switch_it(Calendar *c, char *new_calendar, WindowType win)
{
	int			new_version;
	char			buf[MAXNAMELEN], *loc, *user;
	Tempbr			*tb = (Tempbr*)c->tempbr;
	CSA_return_code		status;
	CSA_session_handle	new_cal_handle;
	Dtcm_calendar		*cal;
	CSA_calendar_user	csa_user;
	CSA_extension		logon_ext;

	set_message(c->message_text, "\0");
 
        /*
	 * Check to see if we're already browsing the requested calendar ...
	 * If we have a valid calendar handle, we can return otherwise
	 * try logon again
	 */
        if (strcmp(new_calendar, c->view->current_calendar) == 0 &&
	    c->cal_handle) {
		sprintf(buf, catgets(c->DT_catd, 1, 178,
			"You Are Already Browsing %s"), new_calendar);
		set_message(c->message_text, buf);
		if (tb && tb->show_message)
			set_message(tb->show_message, buf);
                return;
        }

	invalidate_cache(c);

        /*
	 * Open the new calendar for browsing (or use my_cal_handle if we're
	 * switching to our calendar).
	 */
	if (strcmp(new_calendar, c->calname) == 0) {
		if (c->my_cal_handle == 0) {
			if (open_user_calendar(c, False) == True)
				reset_timer(c);
			else
				return;
		}
		new_cal_handle = c->my_cal_handle;
		c->user_access = c->my_access;
	} else {
		if ((user = cm_target2name(new_calendar)) == NULL) {
			if (!strcmp(new_calendar, ""))
				sprintf(buf, "%s", catgets(c->DT_catd, 1, 619, "Please enter a calendar name in the format: <user>@<hostname>"));
			else
				sprintf(buf, catgets(c->DT_catd, 1, 620,
		"Unknown calendar. Calendar name needed: <name>%s"),
				new_calendar);
			set_message(c->message_text, buf);
			if (win == tempbrowser)
				set_message(tb->show_message, buf);
			return;
		}

		if ((loc = cm_target2location(new_calendar)) == NULL) {
			if (!strcmp(new_calendar, ""))
				sprintf(buf, "%s", catgets(c->DT_catd, 1, 619, "Please enter a calendar name in the format: <user>@<hostname>"));
			else
				sprintf(buf, catgets(c->DT_catd, 1, 622,
	"Unknown calendar. Hostname needed: %s@<hostname>"),
				user);
			set_message(c->message_text, buf);
			if (win == tempbrowser)
				set_message(tb->show_message, buf);
			free(user);
			return;
		}

		/*
		 * Note this assumes your calendar has already been opened;
		 * this should have been done at start-up by open_user_calendar.
		 */

		csa_user.user_name = new_calendar;
		csa_user.user_type = 0;
		csa_user.calendar_user_extensions = NULL;
		csa_user.calendar_address = new_calendar;

		logon_ext.item_code = CSA_X_DT_GET_USER_ACCESS_EXT;
		logon_ext.item_data = 0;
		logon_ext.item_reference = NULL;
		logon_ext.extension_flags = CSA_EXT_LAST_ELEMENT;

		status = csa_logon(NULL, &csa_user, NULL, NULL, NULL,
				&new_cal_handle, &logon_ext);
		free(loc);
		free(user);

		if (status != CSA_SUCCESS) {
			backend_err_msg(c->frame, new_calendar, status,
				((Props_pu *)c->properties_pu)->xm_error_pixmap);
			if (tb && tb->show_message)
				set_message(tb->show_message, "\0");
			return;
		} else
			c->user_access = logon_ext.item_data;
	}

        set_message(((Editor *)c->editor)->message_text, " ");      
        set_message(((GEditor *)c->geditor)->message_text, " ");
        set_message(((ToDo *)c->todo)->message_text, " ");

	/*
	 * Get the version number of the new calendar
	 */

	new_version = get_data_version(new_cal_handle);

	if (new_version < DTCM_DATA_VER)
		set_message(c->message_text, catgets(c->DT_catd, 1, 186,
		       "Calendar (dtcm) and rpc.cmsd versions are different."));

	/*
	 * Close the calendar we were currently browsing (note we don't ever
	 * close our calendar) and set the callback for the new calendar.
	 */
	if (c->cal_handle != c->my_cal_handle)
		csa_logoff(c->cal_handle, NULL);

	if (new_cal_handle != c->my_cal_handle) {


		CSA_flags	flags = NULL;
		CSA_extension	cb_ext;

		flags = CSA_CB_ENTRY_ADDED | CSA_CB_ENTRY_DELETED |
			CSA_CB_ENTRY_UPDATED;
		cb_ext.item_code = CSA_X_XT_APP_CONTEXT_EXT;
		cb_ext.item_data = (CSA_uint32)c->xcontext->app;
		cb_ext.extension_flags = CSA_EXT_LAST_ELEMENT;
		csa_register_callback(new_cal_handle, flags,
				update_handler, NULL, &cb_ext);

	}

	/*
	 * Set variables in the Calendar structure for the new calendar and
	 * repaint.
	 */
	c->cal_handle = new_cal_handle;
	c->general->version = new_version;
        if (c->view->current_calendar != NULL)
                free(c->view->current_calendar);
        c->view->current_calendar = cm_strdup(new_calendar);
        paint_canvas(c, NULL, RENDER_CLEAR_FIRST);
 
	/*
	 * Set the title bars on all the windows!
	 */
	sprintf(buf, "%s : %s", catgets(c->DT_catd, 1, 113, "Calendar"),
				new_calendar);
	XtVaSetValues(c->frame, XmNtitle, buf, NULL);
 
	set_editor_title((Editor *)c->editor, new_calendar);
	add_all_appt((Editor *)c->editor);
	set_geditor_title((GEditor *)c->geditor, new_calendar);
	add_all_gappt((GEditor *)c->geditor);
	set_todo_title((ToDo *)c->todo, new_calendar);
	add_all_todo((ToDo *)c->todo);

	/* set up the repeat menus for all the editors. */

	if (c->editor && (((Editor *)c->editor)->rfp.repeat_menu))
		rfp_set_repeat_values(&((Editor *)c->editor)->rfp);

	if (c->geditor && (((GEditor *)c->geditor)->rfp.repeat_menu))
		rfp_set_repeat_values(&((GEditor *)c->geditor)->rfp);

	if (c->todo && (((ToDo *)c->todo)->rfp.repeat_menu))
		rfp_set_repeat_values(&((ToDo *)c->todo)->rfp);

	if (c->general->version < DATAVER2) {
		if (todo_showing((ToDo *)calendar->todo))
			XtPopdown(((ToDo *)calendar->todo)->frame);
		XtUnmanageChild(c->todo_btn);
		XtSetSensitive(c->todo_edit_menu, False);
		XtSetSensitive(c->todo_view_menu, False);
	}
	else {
		XtManageChild(c->todo_btn);
		XtSetSensitive(c->todo_edit_menu, True);
		XtSetSensitive(c->todo_view_menu, True);
	}

	if (tb && tb->show_message) {
		char	buf[128];
		char *calendar = 
		  XtNewString(catgets(c->DT_catd, 1, 919, "Calendar"));

		sprintf (buf, "%s %s %s",
			calendar,
			new_calendar,
			catgets(c->DT_catd, 1, 920, "displayed."));
		XtFree(calendar);
		set_message(tb->show_message, buf);
	}
}

/*
 * calendar callback function
 *
 * CSA callback that handles main canvas.
 * A separate one is used for the browser canvas (browser.c/mb_update_handler)
 * They get invoked from libcsa when there are changes to calendars we
 * have registered them on.
 *
 * This is a simple callback routine: it doesn't care what the
 * reason is for the call... it always refreshes all data & display.
 * An optimization would be to only refresh the canvas region affected
 * by the data changed.
 */
static void
update_handler(CSA_session_handle cal, CSA_flags reason,
	       CSA_buffer call_data, CSA_buffer client_data, CSA_extension *ext)
{
	Calendar	*c = calendar;

	/* sync canvas */
	invalidate_cache(c);
	paint_canvas(c, NULL, RENDER_CLEAR_FIRST);
	reset_alarm(c);

	/* sync editors */
	if (editor_showing((Editor *)c->editor))
		add_all_appt((Editor *)c->editor);
	if (todo_showing((ToDo *)calendar->todo))
 		add_all_todo((ToDo *)calendar->todo);

}

static Boolean
init_fonts(
	Calendar *cal)
{
	XrmDatabase	 db;
	Cal_Font	 systemfont,
			 userfont;

	/*
	 * Some of the scrolling lists contain formatted text and thus
	 * require a fixed width font to display the text correctly.
	 * We want to use the fixed width font defined by *.userFont as
	 * it will be localized to the system calendar is running on. 
	 */
	db = XtScreenDatabase(XtScreen(cal->frame));
	if (db) {
		XrmPutStringResource(&db, "*find_list.fontList", 
						       cal->app_data->userfont);
		XrmPutStringResource(&db, "*view_list.fontList",
						       cal->app_data->userfont);
		XrmPutStringResource(&db, "*todo_list.fontList",
						       cal->app_data->userfont);
		XrmPutStringResource(&db, "*ge_appt_list.fontList",
						       cal->app_data->userfont);
		XrmPutStringResource(&db, "*access_list.fontList",
						       cal->app_data->userfont);
		XrmPutStringResource(&db, "*browseList.fontList",
						       cal->app_data->userfont);
		XrmPutStringResource(&db, "Dtcm*canvas*week2day.fontList",
						     cal->app_data->systemfont);
		if (cal->app_data->labelfont)
			XrmPutStringResource(&db, "*monthLabel.fontList",
						      cal->app_data->labelfont);
		else
			XrmPutStringResource(&db, "*monthLabel.fontList",
						     cal->app_data->systemfont);
		if (cal->app_data->labelfont)
			XrmPutStringResource(&db, "*yearLabel.fontList",
						      cal->app_data->labelfont);
		else
			XrmPutStringResource(&db, "*yearLabel.fontList",
						     cal->app_data->systemfont);
	}

	if (!fontlist_to_font(cal->app_data->systemfontlist, &systemfont)) {
		/* Couldn't convert the system fontlist to a font - bad news. */
		if (!(systemfont.f.cf_font = 
			     XLoadQueryFont(XtDisplay(cal->frame), "variable")))
			return False;
		else
			systemfont.cf_type = XmFONT_IS_FONT; 
	}
	if (!fontlist_to_font(cal->app_data->userfontlist, &userfont)) {
		/* Couldn't convert the user fontlist to a font - bad news. */
		if (!(userfont.f.cf_font = 
			     XLoadQueryFont(XtDisplay(cal->frame), "fixed")))
			return False;
		else
			userfont.cf_type = XmFONT_IS_FONT; 
	}

	*cal->fonts->userfont =  userfont;

	/*
	 * If the application resources for the view, label, icon or bold
	 * font are not set, default to the system/user font or application
	 * font for the views.
	 */
	if (cal->app_data->viewfontlist) {
		if (!fontlist_to_font(cal->app_data->viewfontlist, 
				      cal->fonts->viewfont))
			*cal->fonts->viewfont = userfont;
	} else {
			load_app_font(cal, MEDIUM, &userfont, 
				      cal->fonts->viewfont);
	}

	if (cal->app_data->labelfontlist) {
		if (!fontlist_to_font(cal->app_data->labelfontlist, 
				      cal->fonts->labelfont))
			*cal->fonts->labelfont = systemfont;
	} else {
			*cal->fonts->labelfont = systemfont;
	}

	if (cal->app_data->iconfontlist) {
		if (!fontlist_to_font(cal->app_data->iconfontlist, 
				      cal->fonts->iconfont))
			*cal->fonts->iconfont = systemfont;
	} else {
			*cal->fonts->iconfont = systemfont;
	}

	if (cal->app_data->boldfontlist) {
		if (!fontlist_to_font(cal->app_data->boldfontlist, 
				      cal->fonts->boldfont))
			*cal->fonts->boldfont = userfont;
	} else {
			load_app_font(cal, BOLD, &userfont, 
				      cal->fonts->boldfont);
	}

	return True;
}
