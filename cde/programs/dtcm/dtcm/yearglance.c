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
**  yearglance.c
**
**  $XConsortium: yearglance.c /main/8 1996/11/21 19:43:58 drk $
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
static char sccsid[] = "@(#)yearglance.c 1.37 95/07/27 Copyr 1991 Sun Microsystems, Inc.";
#endif

#include <EUSCompat.h>
#include <stdio.h>
#include <stdlib.h>
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
#include <rpc/rpc.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <X11/Xlib.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/Form.h>
#include <Xm/ToggleBG.h>
#include "MonthPanel.h"
#include "util.h"
#include "timeops.h"
#include "select.h"
#include "editor.h"
#include "x_graphics.h"
#include "dayglance.h"
#include "weekglance.h"
#include "monthglance.h"
#include "yearglance.h"
#include "calendar.h"
#include "help.h"

#define XOS_USE_XT_LOCKING
#define X_INCLUDE_TIME_H
#if defined(__linux__)
#undef SVR4
#endif
#include <X11/Xos_r.h>

/* if positioning of months in year view changes this must change too!
This specifies row and col for easy selection in year view */
int month_row_col[12][2] =
{
  0, 0,
  0, 1,
  0, 2,
  1, 0,
  1, 1,
  1, 2,
  2, 0,
  2, 1,
  2, 2,
  3, 0,
  3, 1,
  3, 2,
};

extern int prolog_found;

extern char *months[];
extern char *months2[];

static void 	create_month_panels(Calendar *);
static void	year_btn_cb(Widget , XtPointer , XtPointer );
static void	update_year(Calendar *);
static void	allocator(Calendar *);
static void	deallocator(Calendar *);
static Boolean  allocated(Calendar *);


/*
 * check whether year view is allocated yet
 */
static Boolean
allocated(Calendar *c)
{
	return (c->view->year_info != NULL);
}


/*
 * allocate memory used for year view
 */
static void
allocator(Calendar *c)
{
        Year *y;

	c->view->year_info = (caddr_t) ckalloc(sizeof(Year));

	y = (Year *)c->view->year_info;
        y->month_panels =
                (Widget *) ckalloc(12 * sizeof(Widget));

	/* create the month panel widgets */
	create_month_panels(c);
}


/*
 * release memory used for year view
 */
static void
deallocator(Calendar *c)
{
        Year *y = (Year *)c->view->year_info;
/*
	int i;
*/

	XtDestroyWidget(y->form);
	XtDestroyWidget(y->label_form);

	/* Destroy Monthpanels and free space for caching them */
/*
	for (i=0; i<12; i++)
		XtDestroyWidget(y->month_panels[i]);
*/
 
        free(y->month_panels); 

	/* free memory stored in calendar for year stuff */
	free(y);
	c->view->year_info = NULL;

}

extern CSA_return_code
paint_year(Calendar *c)
{
        Year *y;	/* can't initialize this - may not be alloc'd yet! */

	if (!c->cal_handle)
		return(CSA_E_INVALID_PARAMETER);

	XmToggleButtonGadgetSetState(c->year_scope, True, False);
	
/*
 * Do memory allocation if necessary
 */
	if (!allocated(c))
		allocator(c);

	y = (Year *)c->view->year_info;
/*
 * update year displayed if it's not 'current'
 */
	if (y->year_shown != year(c->view->date))
		update_year(c);

/*
 * swap in year form if it's not already there
 */
	XtUnmapWidget(c->canvas);
	XtManageChild(y->label_form);
	XtMapWidget(y->label_form);
	XtManageChild(y->form);
	XtMapWidget(y->form);

	return(CSA_SUCCESS);
}

/*
 * handler for button to switch to year view.
 */
void
year_button (Widget widget, XtPointer data, XtPointer cbs)
{
        Calendar *c = calendar;

	if (c->view->glance == yearGlance)
		return;

	XtUnmapWidget(c->canvas);

	switch (c->view->glance) {
		case dayGlance:
			c->view->glance = yearGlance;
			cleanup_after_dayview(c);
			break;
		case weekGlance:
			c->view->glance = yearGlance;
			cleanup_after_weekview(c);
			break;
		case monthGlance:
			c->view->glance = yearGlance;
			cleanup_after_monthview(c);
			break;
		default:
			break;
	}
        paint_year(c);

}

extern void
print_std_year_range(int start_year, int end_year)
{
	void *xp;
        int n, year;
	Calendar *c = calendar;

	if ((xp = x_open_file(c)) == (void *)NULL)
	  return;

        n = end_year - start_year + 1;

	year = start_year;
        if (n <= 0)
                n = 1;

        for (; n > 0; n--)
        {
	  x_init_printer(xp, PORTRAIT);
	  x_init_std_year(xp);

	  /* print the year at top */
	  x_std_year_name(xp, year);

	  x_print_year(xp, year);
	  x_finish_printer(xp);
	  year++;
        }

	x_print_file(xp, c);
}

/*
 * Set the panels to the right year/month display
 */
static void
update_year(Calendar *c)
{
	XmString str;
	char buf[BUFSIZ];
        Year *y = (Year *)c->view->year_info;
        int year_num = year(c->view->date);
	int i;
	_Xltimeparams localtime_buf;

	/* change year label in yearview */
	/* NL_COMMENT
	   Attention Translator:

	   This string is used in the calendar year view.  In the C locale
	   it has the form:

		1995

	   strftime conversion string: "%Y" is used.

	   Use the appropriate strftime conversion for your locale.
	*/
	strftime(buf, 100, catgets(c->DT_catd, 1, 1092, "%Y"), 
		 _XLocaltime(&c->view->date, localtime_buf));
	XtVaGetValues(y->year_label, XmNlabelString, &str, NULL);
	XmStringFree(str);
	str = XmStringCreateLocalized(buf);
	XtVaSetValues(y->year_label, XmNlabelString, str, NULL);

	/* update each panel */
	for (i=0; i<12; i++) {
		XtVaSetValues(y->month_panels[i], XmNyear, year_num,
				XmNmonth, i+1, NULL);
	}

	/* record the year that's been displayed */
	y->year_shown = year_num;
}


/*
 * this creates the 3x4 grid of MonthPanel widgets that
 * make up the year view.
 */
static void
create_month_panels(Calendar *c) {
        Year *y = (Year *)c->view->year_info;
	Arg al[20];
	int ac=0, i=0;

/*
 * This array holds the relative positioning for panels
 * in the year view.  Each row below is a set of 4 numbers:
 * top, right, bottom, left.  The numbers are used as the
 * appropriate <side>Position constraints on the MonthPanel
 * children of the Form.  Relies on Form's fractionBase resource
 * being set up as 100, which is is at create time.
 */
static int anchors[] = {
	0,	33,	24,	1,
	0,	66,	24,	34,
	0,	99,	24,	67,
	25,	33,	49,	1,
	25,	66,	49,	34,
	25,	99,	49,	67,
	50,	33,	74,	1,
	50,	66,	74,	34,
	50,	99,	74,	67,
	75,	33,	100,	1,
	75,	66,	100,	34,
	75,	99,	100,	67
}; 
 /*
static int anchors[] = {
	0,	24,	33,	1,
	0,	49,	33,	26,
	0,	74,	33,	51,
	0,	99,	33,	76,
	34,	24,	66,	1,
	34,	49,	66,	26,
	34,	74,	66,	51,
	34,	99,	66,	76,
	67,	24,	100,	1,
	67,	49,	100,	26,
	67,	74,	100,	51,
	67,	99,	100,	76
};  */


/*
 * Create a container to replace the old canvas
 */
	ac=0;
	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNtopWidget, c->previous); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftOffset, 1); ac++;
	y->label_form = XmCreateForm(c->form, "labelForm", al, ac);

	ac=0;
	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNtopWidget, y->label_form); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftOffset, 1); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_POSITION); ac++;
	XtSetArg(al[ac], XmNbottomPosition, 95); ac++;
	y->form = XmCreateForm(c->form, "yearForm", al, ac);
	XtVaSetValues(c->message_text, XmNtopWidget, y->form, NULL);
	XtManageChild(y->label_form);
	XtUnmapWidget(y->label_form);
	XtManageChild(y->form);
	XtUnmapWidget(y->form);

	XtAddCallback(y->form, XmNhelpCallback, (XtCallbackProc)help_cb, 
			(XtPointer) CALENDAR_HELP_YEAR_WINDOW);

/*
 * Label displaying year for this view
 */
	ac=0;
	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	y->year_label = XmCreateLabel(y->label_form, "yearLabel", al, ac);
	XtManageChild(y->year_label);

/*
 * Create 12 monthPanels
 */
	/* fixed part of arglist */
	ac=0;
	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_POSITION); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_POSITION); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_POSITION); 

	for (i=0; i<12; i++) {
		char name[BUFSIZ];

		sprintf(name, "panel%d", i+1);
		ac=4;		/* start adding after fixed part */
		XtSetArg(al[ac], XmNtopPosition, anchors[4*i]); ac++;
		XtSetArg(al[ac], XmNrightPosition, anchors[4*i+1]); ac++;
		XtSetArg(al[ac], XmNbottomPosition, anchors[4*i+2]); ac++;
		XtSetArg(al[ac], XmNleftPosition, anchors[4*i+3]); ac++;
		XtSetArg(al[ac], XmNwidth, 150); ac++;
		XtSetArg(al[ac], XmNheight, 100); ac++;
		y->month_panels[i] = XmCreateMonthPanel(y->form, name, al, ac);
		XtAddCallback(y->month_panels[i], XmNactivateCallback,
				year_btn_cb, (XtPointer) c);
	}
	XtManageChildren(y->month_panels, 12);
}

/*
 * This is the callback for all the buttons on the month
 * panels.  It figures out what was pressed, and invokes
 * the appropriate function.
 */
static void
year_btn_cb(Widget w, XtPointer client, XtPointer call)
{
        XmMonthPanelCallbackStruct *cbs = (XmMonthPanelCallbackStruct *) call;
	Calendar *c = (Calendar *)client;
	int monthno, year;
 
	XtVaGetValues(w, XmNmonth, &monthno, XmNyear, &year, NULL);

        if (cbs->type == MONTH_SELECTION) {
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
		cleanup_after_yearview(c);
		c->view->glance = monthGlance;
		prepare_to_paint_monthview(c, NULL);
		XtMapWidget(c->canvas);
	}
        else {	/* type == DAY_SELECTION */
		c->view->olddate = c->view->date;
                c->view->date = monthdayyear(monthno, cbs->day, year);
		cleanup_after_yearview(c);

		c->view->glance = dayGlance;
        	init_mo(c);
        	(void)init_dayview(c);

		XtMapWidget(c->canvas);
		paint_day(c);
	}
}

extern void
cleanup_after_yearview(Calendar *c)
{
        Year *y = (Year *)c->view->year_info;

	if (!y) return;

	XtUnmapWidget(y->form);
	XtUnmapWidget(y->label_form);

	/* Unmanaging the form is necessary to keep the stupid 
	   traversal order right */

	XtUnmanageChild(y->form);
	XtUnmanageChild(y->label_form);
       	XmProcessTraversal(c->appt_btn, XmTRAVERSE_CURRENT); 

	XmToggleButtonGadgetSetState(c->year_scope, False, False);

	/*
	 * Reclaim year view memory.  You should comment this out
	 * if rendering performance is more important than
	 * minimizing memory usage.  It saves 400 *dget instances
	 * lying around when they may never be needed again, but
	 * at the expense of recreating them if they are needed.
	 */
/*
	deallocator(c);
*/
}

