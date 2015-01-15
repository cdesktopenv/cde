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
/* $XConsortium: print.c /main/18 1996/11/25 10:23:05 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#include <EUSCompat.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <csa.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/LabelG.h>
#include <Xm/DialogS.h>
#include <Xm/Print.h>
#include <Xm/Protocols.h>
#include <Xm/PushBG.h>
#include <Xm/SeparatoG.h>
#include <Xm/SpinB.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>
#include <Xm/SashP.h>
#include <Xm/SSpinB.h>
#include <Xm/RowColumn.h>
#include <Dt/HourGlass.h>
#include "util.h"
#include "misc.h"
#include "props.h"
#include "props_pu.h"
#include "getdate.h"
#include "datefield.h"
#include "timeops.h"
#include "calendar.h"
#include "print.h"
#include "todo.h"
#include "help.h"

/* needed for view-specific print routines */
#include "dayglance.h"
#include "weekglance.h"
#include "monthglance.h"
#include "yearglance.h"

static char *printErrorTitle = "Calendar : Error - Print";
static char *setupErrorTitle = "Calendar : Error - Print Setup";
static char *pdmErrorText = "Print Dialog Manager error - setup failed.";

typedef struct {
   /* widget handles */
   Widget pdb;
   Widget form;
   Widget report_type_option;
   Widget from_label;
   Widget from_spin;
   Widget from_day;
   Widget from_month;
   Widget from_year;
   Widget to_label;
   Widget to_spin;
   Widget to_day;
   Widget to_month;
   Widget to_year;
   Widget more_opts;
#ifdef GR_DEBUG
   Widget debugToggle;
#endif

   int    report_type;
   Boolean setupDataValid;
   DtPrintSetupData setupData;
   Widget printShell;
   Boolean badAllocError;
} _DtCmPrintData;

/*
 * private function declarations
 */
static void print_cb(Widget, XtPointer, XtPointer);
static void print_setup_cb(Widget, XtPointer, XtPointer);
static void close_print_display_cb(Widget, XtPointer, XtPointer);
static void cancel_cb(Widget, XtPointer, XtPointer);
static void pdm_notify_cb(Widget, XtPointer, XtPointer);
static void report_option_cb(Widget, XtPointer, XtPointer);
static void from_modify_verify_cb(Widget, XtPointer, XtPointer);
static void to_modify_verify_cb(Widget, XtPointer, XtPointer);
static void spin_field_changed_cb(Widget, XtPointer, XtPointer);
static void more_opts_cb(Widget, XtPointer, XtPointer);
static Tick pd_modify_day(Calendar *, int, Boolean);
static Tick pd_modify_month(Calendar *, int, Boolean);
static Tick pd_modify_year(Calendar *, int, Boolean);
static Boolean pd_set_start_date(Calendar *, Tick);
static Boolean pd_set_end_date(Calendar *, Tick);
static Boolean pd_set_max_start_date(Calendar *, Tick);
static Boolean pd_set_min_end_date(Calendar *, Tick);
static Boolean pd_set_report_type(Calendar *, int);
static Boolean pd_set_report_managed(Calendar *, int);
static Boolean pd_get_start_positions(Calendar *, int *, int *, int *);
static Boolean pd_get_end_positions(Calendar *, int *, int *, int *);
static void clearSetupData(_DtCmPrintData *);
static void createPrintShell(Calendar *);
static void report_error(Calendar *, char *, char *);

#ifdef GR_DEBUG
Boolean
inDebugMode(Calendar *c)
{
  _DtCmPrintData *pd = (_DtCmPrintData *)c->print_data;
  if (pd != (_DtCmPrintData *)NULL)
    return XmToggleButtonGadgetGetState(pd->debugToggle);

  return False;
}
#endif

static Boolean
pd_set_start_date(Calendar *c, Tick tick)
{
  _DtCmPrintData *pd = (_DtCmPrintData *)c->print_data;
  Props *p = (Props *)c->properties;
  OrderingType ot = get_int_prop(p, CP_DATEORDERING);
  SeparatorType st = get_int_prop(p, CP_DATESEPARATOR);
  Arg args[5];
  int nargs;

  if (pd == (_DtCmPrintData *)NULL)
    return False;

  if (tick > get_eot())
    tick = get_eot();

  if (pd_get_report_type(c) == PR_WEEK_VIEW)
  {
    /* Week view - start at Monday. */
    tick = first_dow(tick);
  }

  if (tick < get_bot())
    tick = get_bot();

  if (XtIsManaged(pd->from_day))
  {
    nargs = 0;
    XtSetArg(args[nargs], XmNposition, dom(tick)); nargs++;
    XtSetArg(args[nargs], XmNmaximumValue, monthlength(tick)); nargs++;
    XtSetValues(pd->from_day, args, nargs);
  }
  if (XtIsManaged(pd->from_month))
  {
    XtSetArg(args[0], XmNposition, month(tick) - 1);
    XtSetValues(pd->from_month, args, 1);
  }
  XtSetArg(args[0], XmNposition, year(tick));
  XtSetValues(pd->from_year, args, 1);

  return True;
}

static Boolean
pd_set_end_date(Calendar *c, Tick tick)
{
  _DtCmPrintData *pd = (_DtCmPrintData *)c->print_data;
  Props *p = (Props *)c->properties;
  OrderingType ot = get_int_prop(p, CP_DATEORDERING);
  SeparatorType st = get_int_prop(p, CP_DATESEPARATOR);
  Arg args[5];
  int nargs;

  if (pd == (_DtCmPrintData *)NULL)
    return False;

  if (tick < get_bot())
    tick = get_bot();

  if (pd_get_report_type(c) == PR_WEEK_VIEW)
  {
    /* Week view - end on Sunday. */
    tick = last_dow(tick);
  }

  if (tick > get_eot())
    tick = get_eot();

  if (XtIsManaged(pd->to_day))
  {
    nargs = 0;
    XtSetArg(args[nargs], XmNposition, dom(tick)); nargs++;
    XtSetArg(args[nargs], XmNmaximumValue, monthlength(tick)); nargs++;
    XtSetValues(pd->to_day, args, nargs);
  }
  if (XtIsManaged(pd->to_month))
  {
    XtSetArg(args[0], XmNposition, month(tick) - 1);
    XtSetValues(pd->to_month, args, 1);
  }
  XtSetArg(args[0], XmNposition, year(tick));
  XtSetValues(pd->to_year, args, 1);

  return True;
}

static Boolean
pd_set_max_start_date(Calendar *c, Tick tick)
{
  Tick curTick = pd_get_start_date(c);

  return (tick < curTick) ? pd_set_start_date(c, tick) : True;
}

static Boolean
pd_set_min_end_date(Calendar *c, Tick tick)
{
  Tick curTick = pd_get_end_date(c);

  return (tick > curTick) ? pd_set_end_date(c, tick) : True;
}

static Boolean
pd_set_report_type(Calendar *c, int reportType)
{
  _DtCmPrintData *pd = (_DtCmPrintData *)c->print_data;
  Widget optionMenu = (Widget)NULL;
  Arg args[10];
  int nargs;

  if (pd == (_DtCmPrintData *)NULL)
    return False;

  if (reportType == pd->report_type)
    return True;

  nargs = 0;
  XtSetArg(args[nargs], XmNsubMenuId, &optionMenu); nargs++;
  XtGetValues(pd->report_type_option, args, nargs);

  if (optionMenu != (Widget)NULL)
  {
    WidgetList optionChildren;
    int nChildren = 0;

    nargs = 0;
    XtSetArg(args[nargs], XmNchildren, &optionChildren); nargs++;
    XtSetArg(args[nargs], XmNnumChildren, &nChildren); nargs++;
    XtGetValues(optionMenu, args, nargs);

    if (nChildren > reportType)
    {
      nargs = 0;
      XtSetArg(args[nargs], XmNmenuHistory,
	       optionChildren[reportType]); nargs++;
      XtSetValues(pd->report_type_option, args, nargs);

      return pd_set_report_managed(c, reportType);
    }
  }

  return False;
}

static Boolean
pd_set_report_managed(Calendar *c, int reportType)
{
  _DtCmPrintData *pd = (_DtCmPrintData *)c->print_data;
  Arg args[10];
  int nargs;

  if (pd == (_DtCmPrintData *)NULL)
    return False;

  if (reportType == pd->report_type)
    return True;

  pd->report_type = reportType;

  switch (reportType)
  {
  case PR_YEAR_VIEW:
    XtManageChild(pd->from_year);
    XtManageChild(pd->to_year);

    XtUnmanageChild(pd->from_day);
    XtUnmanageChild(pd->from_month);
    XtUnmanageChild(pd->to_day);
    XtUnmanageChild(pd->to_month);
    break;

  case PR_WEEK_VIEW:
    XtManageChild(pd->from_year);
    XtManageChild(pd->from_month);
    XtManageChild(pd->from_day);
    XtManageChild(pd->to_year);
    XtManageChild(pd->to_month);
    XtManageChild(pd->to_day);

    nargs = 0;
    XtSetArg(args[nargs], XmNeditable, False); nargs++;
    XtSetArg(args[nargs], XmNcursorPositionVisible, False); nargs++;
    XtSetValues(pd->from_day, args, nargs);
    XtSetValues(pd->to_day, args, nargs);
    break;

  case PR_DAY_VIEW:
  case PR_APPT_LIST:
  case PR_TODO_LIST:
    XtManageChild(pd->from_year);
    XtManageChild(pd->from_month);
    XtManageChild(pd->from_day);
    XtManageChild(pd->to_year);
    XtManageChild(pd->to_month);
    XtManageChild(pd->to_day);

    nargs = 0;
    XtSetArg(args[nargs], XmNeditable, True); nargs++;
    XtSetArg(args[nargs], XmNcursorPositionVisible, True); nargs++;
    XtSetValues(pd->from_day, args, nargs);
    XtSetValues(pd->to_day, args, nargs);
    break;

  default:
  case PR_MONTH_VIEW:
    XtManageChild(pd->from_year);
    XtManageChild(pd->from_month);
    XtManageChild(pd->to_year);
    XtManageChild(pd->to_month);

    XtUnmanageChild(pd->from_day);
    XtUnmanageChild(pd->to_day);
    break;
  }

  return True;
}

static Boolean
pd_get_start_positions(Calendar *c, int *m, int *d, int *y)
{
  _DtCmPrintData *pd = (_DtCmPrintData *)c->print_data;
  int dayPos = 1, monthPos = 0, yearPos;

  if (pd == (_DtCmPrintData *)NULL)
    return False;

  if (XtIsManaged(pd->from_day))
    XtVaGetValues(pd->from_day, XmNposition, &dayPos, NULL);
  if (XtIsManaged(pd->from_month))
    XtVaGetValues(pd->from_month, XmNposition, &monthPos, NULL);
  XtVaGetValues(pd->from_year, XmNposition, &yearPos, NULL);

  *m = monthPos;
  *d = dayPos;
  *y = yearPos;

  return True;
}

static Boolean
pd_get_end_positions(Calendar *c, int *m, int *d, int *y)
{
  _DtCmPrintData *pd = (_DtCmPrintData *)c->print_data;
  int dayPos = 1, monthPos = 0, yearPos;

  if (pd == (_DtCmPrintData *)NULL)
    return False;

  if (XtIsManaged(pd->to_day))
    XtVaGetValues(pd->to_day, XmNposition, &dayPos, NULL);
  if (XtIsManaged(pd->to_month))
    XtVaGetValues(pd->to_month, XmNposition, &monthPos, NULL);
  XtVaGetValues(pd->to_year, XmNposition, &yearPos, NULL);

  *m = monthPos;
  *d = dayPos;
  *y = yearPos;

  return True;
}

static void
from_modify_verify_cb(Widget w, XtPointer uDataP, XtPointer cbDataP)
{
  Calendar *c = (Calendar *)uDataP;
  XmSpinBoxCallbackStruct *cbStruct = (XmSpinBoxCallbackStruct *)cbDataP;
  _DtCmPrintData *pd = (_DtCmPrintData *)c->print_data;
  int newPos;
  Tick newTick;

  cbStruct->doit = False;

  if (cbStruct->widget == pd->from_day)
  {
    newTick = pd_modify_day(c, cbStruct->reason, False);
  }
  else if (cbStruct->widget == pd->from_month)
  {
    newTick = pd_modify_month(c, cbStruct->reason, False);
  }
  else if (cbStruct->widget == pd->from_year)
  {
    newTick = pd_modify_year(c, cbStruct->reason, False);
  }
  else return;

  if (pd_set_start_date(c, newTick) && pd_set_min_end_date(c, newTick))
  {
    int monthPos, dayPos, yearPos;

    pd_get_start_positions(c, &monthPos, &dayPos, &yearPos);
    if (cbStruct->widget == pd->from_day)
      cbStruct->position = dayPos;
    else if (cbStruct->widget == pd->from_month)
      cbStruct->position = monthPos;
    else cbStruct->position = yearPos;
    cbStruct->doit = True;
  }
}

static void
to_modify_verify_cb(Widget w, XtPointer uDataP, XtPointer cbDataP)
{
  Calendar *c = (Calendar *)uDataP;
  XmSpinBoxCallbackStruct *cbStruct = (XmSpinBoxCallbackStruct *)cbDataP;
  _DtCmPrintData *pd = (_DtCmPrintData *)c->print_data;
  Tick newTick;

  cbStruct->doit = False;

  if (cbStruct->widget == pd->to_day)
  {
    newTick = pd_modify_day(c, cbStruct->reason, True);
  }
  else if (cbStruct->widget == pd->to_month)
  {
    newTick = pd_modify_month(c, cbStruct->reason, True);
  }
  else if (cbStruct->widget == pd->to_year)
  {
    newTick = pd_modify_year(c, cbStruct->reason, True);
  }
  else return;

  if (pd_set_end_date(c, newTick) && pd_set_max_start_date(c, newTick))
  {
    int monthPos, dayPos, yearPos;

    pd_get_end_positions(c, &monthPos, &dayPos, &yearPos);
    if (cbStruct->widget == pd->to_day)
      cbStruct->position = dayPos;
    else if (cbStruct->widget == pd->to_month)
      cbStruct->position = monthPos;
    else cbStruct->position = yearPos;
    cbStruct->doit = True;
  }
}

static void
spin_field_changed_cb(Widget field, XtPointer uData, XtPointer cbData)
{
  char *newStr;
  int curPos;
  int newPos;
  unsigned char sbcType;
  int minValue;
  int maxValue;
  int nValues;
  XmStringTable strings;
  int cursorPosition;
  Arg args[20];
  int nargs;

  nargs = 0;
  XtSetArg(args[nargs], XmNspinBoxChildType, &sbcType); nargs++;
  XtSetArg(args[nargs], XmNmaximumValue, &maxValue); nargs++;
  XtSetArg(args[nargs], XmNminimumValue, &minValue); nargs++;
  XtSetArg(args[nargs], XmNnumValues, &nValues); nargs++;
  XtSetArg(args[nargs], XmNposition, &curPos); nargs++;
  XtSetArg(args[nargs], XmNvalues, &strings); nargs++;
  XtSetArg(args[nargs], XmNvalue, &newStr); nargs++;
  XtSetArg(args[nargs], XmNcursorPosition, &cursorPosition); nargs++;
  XtGetValues(field, args, nargs);

  newPos = curPos;
  if (sbcType == XmSTRING)
  {
    XmString xmString = XmStringCreateLocalized(newStr);

    for (newPos = 0; newPos < nValues; newPos++)
    {
      if (XmStringCompare(xmString, strings[newPos]))
	break;
    }

    if (newPos >= nValues)
      newPos = curPos;

    XmStringFree(xmString);
  }
  else if (sbcType == XmNUMERIC)
  {
    newPos = atoi(newStr);

    if ((newPos < minValue) ||
	(newPos > maxValue))
      newPos = curPos;
  }

  if (newPos != curPos)
  {
    nargs = 0;
    XtSetArg(args[nargs], XmNposition, newPos); nargs++;
    XtSetValues(field, args, nargs);

    /* After the above does its thing, restore original cursor position. */
    nargs = 0;
    XtSetArg(args[nargs], XmNcursorPosition, cursorPosition); nargs++;
    XtSetValues(field, args, nargs);
  }

  XtFree(newStr);
}

static void
more_opts_cb(Widget w, XtPointer uData, XtPointer cbData)
{
  Calendar *c = (Calendar *)uData;
  _DtCmPrintData *pd = (_DtCmPrintData *)c->print_data;

  _DtTurnOnHourGlass(c->frame);
  _DtTurnOnHourGlass(pd->pdb);

  show_print_props_pu(c);

  _DtTurnOffHourGlass(pd->pdb);
  _DtTurnOffHourGlass(c->frame);
}

static Tick
pd_modify_day(Calendar *c, int reason, Boolean isTo)
{
  int monthPos, dayPos, yearPos;
  int maxDay;
  Tick tmpTick;
  Tick curTick;
  Tick newTick;
  int reportType = pd_get_report_type(c);

  if (isTo)
  {
    if (!pd_get_end_positions(c, &monthPos, &dayPos, &yearPos))
      return 0;
  }
  else
  {
    if (!pd_get_start_positions(c, &monthPos, &dayPos, &yearPos))
      return 0;
  }

  switch (reason)
  {
  case XmCR_SPIN_NEXT:
    tmpTick = monthdayyear(monthPos + 1, 1, yearPos);
    maxDay = monthlength(tmpTick);
    if ((dayPos == 1) || (dayPos > maxDay))
      dayPos = maxDay;
    else dayPos--;
    curTick = monthdayyear(monthPos + 1, dayPos, yearPos);
    newTick = (reportType == PR_WEEK_VIEW) ?
      nextweek(curTick) : nextday(curTick);
    break;

  case XmCR_SPIN_PRIOR:
    tmpTick = monthdayyear(monthPos + 1, 1, yearPos);
    maxDay = monthlength(tmpTick);
    if (dayPos >= maxDay)
      dayPos = 1;
    else dayPos++;
    curTick = monthdayyear(monthPos + 1, dayPos, yearPos);
    newTick = (reportType == PR_WEEK_VIEW) ?
      prevweek(curTick) : prevday(curTick);
    break;

  default:
  case XmCR_SPIN_FIRST:
    newTick = monthdayyear(monthPos + 1, 1, yearPos);
    if (reportType == PR_WEEK_VIEW)
    {
      if (isTo)
      {
	/* We want the first Sunday in the month. */
	newTick = last_dow(newTick);
      }
      else
      {
	/* We want the first Monday in the month. */
	int dayOffset = (dow(newTick) + 6) % 7;

	if (dayOffset > 0)
	  newTick = next_ndays(newTick, 7 - dayOffset);
      }
    }
    break;

  case XmCR_SPIN_LAST:
    curTick = monthdayyear(monthPos + 1, 1, yearPos);
    newTick = last_dom(curTick);
    if (reportType == PR_WEEK_VIEW)
    {
      if (isTo)
      {
	/* We want the last Sunday in the month. */
	int dayOffset = dow(newTick);

	if (dayOffset > 0)
	  newTick = last_ndays(newTick, dayOffset);
      }
      else
      {
	/* We want the last Monday in the month. */
	newTick = first_dow(newTick);
      }
    }
    break;
  }

  return newTick;
}

static Tick
pd_modify_month(Calendar *c, int reason, Boolean isTo)
{
  int monthPos, dayPos, yearPos;
  int maxDay;
  Tick tmpTick;
  Tick curTick;
  Tick newTick;
  int reportType = pd_get_report_type(c);

  if (isTo)
  {
    if (!pd_get_end_positions(c, &monthPos, &dayPos, &yearPos))
      return 0;
  }
  else
  {
    if (!pd_get_start_positions(c, &monthPos, &dayPos, &yearPos))
      return 0;
  }

  switch (reason)
  {
  case XmCR_SPIN_NEXT:
    if (monthPos == 0)
      monthPos = 11;
    else monthPos--;

    curTick = monthdayyear(monthPos + 1, dayPos, yearPos);
    if (reportType == PR_WEEK_VIEW)
      newTick = next_ndays(curTick, 4 * 7);
    else
    {
      tmpTick = nextmonth(curTick);
      maxDay = monthlength(tmpTick);
      if (dayPos > maxDay)
	dayPos = maxDay;
      newTick = monthdayyear(month(tmpTick), dayPos, year(tmpTick));
    }
    break;

  case XmCR_SPIN_PRIOR:
    if (monthPos == 11)
      monthPos = 0;
    else monthPos++;

    curTick = monthdayyear(monthPos + 1, dayPos, yearPos);
    if (reportType == PR_WEEK_VIEW)
      newTick = last_ndays(curTick, 4 * 7);
    else
    {
      /* ... There is no prevmonth()! ... */
      tmpTick = prev_nmonth(curTick, 1);
      maxDay = monthlength(tmpTick);
      if (dayPos > maxDay)
	dayPos = maxDay;
      newTick = monthdayyear(month(tmpTick), dayPos, year(tmpTick));
    }
    break;

  default:
  case XmCR_SPIN_FIRST:
    newTick = monthdayyear(1, 1, yearPos);
    if (reportType == PR_WEEK_VIEW)
    {
      if (isTo)
      {
	/* We want the first Sunday in the month. */
	newTick = last_dow(newTick);
      }
      else
      {
	/* We want the first Monday in the month. */
	int dayOffset = (dow(newTick) + 6) % 7;

	if (dayOffset > 0)
	  newTick = next_ndays(newTick, 7 - dayOffset);
      }
    }
    break;

  case XmCR_SPIN_LAST:
    newTick = monthdayyear(12, 31, yearPos);
    if (reportType == PR_WEEK_VIEW)
    {
      if (isTo)
      {
	/* We want the last Sunday in the month. */
	int dayOffset = dow(newTick);

	if (dayOffset > 0)
	  newTick = last_ndays(newTick, dayOffset);
      }
      else
      {
	/* We want the last Monday in the month. */
	newTick = first_dow(newTick);
      }
    }
    break;
  }

  return newTick;
}

static Tick
pd_modify_year(Calendar *c, int reason, Boolean isTo)
{
  int monthPos, dayPos, yearPos;
  int maxDay;
  Tick tmpTick;
  Tick curTick;
  Tick newTick;
  int reportType = pd_get_report_type(c);

  if (isTo)
  {
    if (!pd_get_end_positions(c, &monthPos, &dayPos, &yearPos))
      return 0;
  }
  else
  {
    if (!pd_get_start_positions(c, &monthPos, &dayPos, &yearPos))
      return 0;
  }

  switch (reason)
  {
  case XmCR_SPIN_NEXT:
    yearPos--;

    curTick = monthdayyear(monthPos + 1, dayPos, yearPos);
    if (reportType == PR_WEEK_VIEW)
      newTick = next_ndays(curTick, 52 * 7);
    else
    {
      tmpTick = monthdayyear(monthPos + 1, 1, yearPos + 1);
      maxDay = monthlength(tmpTick);
      if (dayPos > maxDay)
	dayPos = maxDay;
      newTick = monthdayyear(monthPos + 1, dayPos, yearPos + 1);
    }
    break;

  case XmCR_SPIN_PRIOR:
    yearPos++;

    curTick = monthdayyear(monthPos + 1, dayPos, yearPos);
    if (reportType == PR_WEEK_VIEW)
      newTick = last_ndays(curTick, 52 * 7);
    else
    {
      tmpTick = monthdayyear(monthPos + 1, 1, yearPos - 1);
      maxDay = monthlength(tmpTick);
      if (dayPos > maxDay)
	dayPos = maxDay;
      newTick = monthdayyear(monthPos + 1, dayPos, yearPos - 1);
    }
    break;

  default:
  case XmCR_SPIN_FIRST:
    yearPos = year(get_bot());
    tmpTick = monthdayyear(monthPos + 1, 1, yearPos);
    maxDay = monthlength(tmpTick);
    if (dayPos > maxDay)
      dayPos = maxDay;
    newTick = monthdayyear(monthPos + 1, dayPos, yearPos);
    break;

  case XmCR_SPIN_LAST:
    yearPos = year(get_eot());
    tmpTick = monthdayyear(monthPos + 1, 1, yearPos);
    maxDay = monthlength(tmpTick);
    if (dayPos > maxDay)
      dayPos = maxDay;
    newTick = monthdayyear(monthPos + 1, dayPos, yearPos);
    break;
  }

  return newTick;
}

/*
 * post_print_dialog()
 *
 * Create and display the Print dialog.
 */
void
post_print_dialog(
	Calendar *c)
{
  _DtCmPrintData *pd;

  create_print_dialog(c);

  /* display the dialog on-screen */
  if ((pd = (_DtCmPrintData *)c->print_data) != (_DtCmPrintData *)NULL)
  {
    if (!XtIsManaged(pd->pdb))
      XtManageChild(pd->pdb);
    else XRaiseWindow(XtDisplay(pd->pdb),
		      XtWindow(XtParent(pd->pdb)));
  }
}

static void
clearSetupData(_DtCmPrintData *pd)
{
  if (pd->setupDataValid)
  {
    DtPrintFreeSetupData(&pd->setupData);
    pd->setupDataValid = False;
  }

  memset(&pd->setupData, 0, sizeof(DtPrintSetupData));
}

static void
createPrintShell(Calendar *c)
{
#if defined(PRINTING_SUPPORTED)
  _DtCmPrintData *pd = (_DtCmPrintData *)c->print_data;

#ifdef GR_DEBUG
  if (inDebugMode(c))
  {
    if (pd->printShell != (Widget)NULL)
      XtDestroyWidget(pd->printShell);

    pd->printShell = XmCreateDialogShell(c->frame, "Print",
					 0, NULL);
  }
  else
#endif
  if (pd->printShell == (Widget)NULL)
  {
    pd->printShell =
      XmPrintSetup(pd->pdb,
		   XpGetScreenOfContext(pd->setupData.print_display,
					pd->setupData.print_context),
		   "Print", NULL, 0);

    XtAddCallback(pd->printShell, XmNpdmNotificationCallback,
		  pdm_notify_cb, (XtPointer)c);
  }
#endif  /* PRINTING_SUPPORTED */
}

void
create_print_dialog(Calendar *c)
{
	XmString 	 xmstr, day_view, week_view, month_view,
			 year_view, appt_list, todo_list, view;
	_DtCmPrintData 	*pd;
	char 		 fnamebuf[BUFSIZ];
	char		 *print_file;
	char		 *print_dir;
	char		 *title;
	Arg              args[20];
	int              nargs;
	XmString	 abbrevMonths[12];
	int		 maxMonthLen;
	int		 i;
	int		 reportType;
	Widget		 helpButton;

	if ((pd = (_DtCmPrintData *)c->print_data) == NULL)
	{
	  c->print_data = (caddr_t)XtMalloc(sizeof(_DtCmPrintData));
	  pd = (_DtCmPrintData *) c->print_data;

	  /* Initialize filename to dir+filename from props */
	  print_file = get_char_prop((Props *)c->properties,CP_PRINTFILENAME);
	  if (!print_file)
	    print_file = "";
	  print_dir = get_char_prop((Props *)c->properties,CP_PRINTDIRNAME);
	  if (!print_dir)
	    print_dir = "";
	  sprintf(fnamebuf, "%s/%s", print_dir, print_file);

	  title = XtNewString(catgets(c->DT_catd, 1, 728, "Calendar : Print"));
	  nargs = 0;
	  XtSetArg(args[nargs], XmNtitle, title); nargs++;
	  XtSetArg(args[nargs], XmNdeleteResponse, XmUNMAP); nargs++;
	  XtSetArg(args[nargs], DtNfileName, fnamebuf); nargs++;
	  XtSetArg(args[nargs], XmNautoUnmanage, False); nargs++;
	  XtSetArg(args[nargs], XmNdeleteResponse, XmDO_NOTHING); nargs++;
	  pd->pdb = DtCreatePrintSetupDialog(c->frame, "Calendar - Print",
					     args, nargs);
	  XtFree(title);

	  setup_quit_handler(XtParent(pd->pdb), cancel_cb, (XtPointer)c);

	  XtAddCallback(pd->pdb, DtNcancelCallback,
			cancel_cb, (XtPointer)c);
	  XtAddCallback(pd->pdb, DtNprintCallback,
			print_cb, (XtPointer)c);
	  XtAddCallback(pd->pdb, DtNclosePrintDisplayCallback,
			close_print_display_cb, (XtPointer)c);
	  XtAddCallback(pd->pdb, DtNsetupCallback,
			print_setup_cb, (XtPointer)c);

	  helpButton = XtNameToWidget(pd->pdb, "Help");
	  if (helpButton != (Widget)NULL)
	    XtAddCallback(helpButton, XmNactivateCallback,
			  help_cb, (XtPointer)PRINT_HELP_BUTTON);
	  XtAddCallback(pd->pdb, XmNhelpCallback,
			help_cb, (XtPointer)PRINT_HELP_BUTTON);

	  nargs = 0;
	  XtSetArg(args[nargs], XmNfractionBase, 3); nargs++;
	  pd->form = XmCreateForm(pd->pdb, "RangeForm", args, nargs);

	  view = XmStringCreateLocalized(
			catgets(c->DT_catd, 1, 976, "Report Type:"));
	  day_view = XmStringCreateLocalized(
			catgets(c->DT_catd, 1, 977, "Day View"));
	  week_view = XmStringCreateLocalized(
			catgets(c->DT_catd, 1, 978, "Week View"));
	  month_view = XmStringCreateLocalized(
			catgets(c->DT_catd, 1, 979, "Month View"));
	  year_view = XmStringCreateLocalized(
			catgets(c->DT_catd, 1, 980, "Year View"));
	  appt_list = XmStringCreateLocalized(
			catgets(c->DT_catd, 1, 981, "Appointment List"));
	  todo_list = XmStringCreateLocalized(
			catgets(c->DT_catd, 1, 982, "To Do List"));

	  /*
	   * remember - this returns a RowColumn widget!
	   */
	  pd->report_type = PR_DAY_VIEW;
	  pd->report_type_option = XmVaCreateSimpleOptionMenu(pd->form,
		"TypeOptionMenu", view, 0,
		pd->report_type, report_option_cb,
		XmVaPUSHBUTTON, 	day_view, NULL, NULL, NULL,
		XmVaPUSHBUTTON, 	week_view, NULL, NULL, NULL,
		XmVaPUSHBUTTON, 	month_view, NULL, NULL, NULL,
		XmVaPUSHBUTTON, 	year_view, NULL, NULL, NULL,
		XmVaPUSHBUTTON, 	appt_list, NULL, NULL, NULL,
		XmVaPUSHBUTTON, 	todo_list, NULL, NULL, NULL,
		XmNorientation, 	XmVERTICAL,
		XmNtopAttachment, 	XmATTACH_FORM,
		XmNtopOffset, 		10,
		XmNleftAttachment, 	XmATTACH_POSITION,
		XmNleftPosition, 	1,
		XmNleftOffset,		5,
		XmNmarginWidth,		0,
		XmNnavigationType, 	XmTAB_GROUP,
		NULL);
	  XtManageChild(pd->report_type_option);

	  XmStringFree(day_view);
	  XmStringFree(week_view);
	  XmStringFree(month_view);
	  XmStringFree(year_view);
	  XmStringFree(appt_list);
	  XmStringFree(todo_list);
	  XmStringFree(view);

	  xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1,
						  731, "From:"));
	  pd->from_label = XtVaCreateWidget("FromLabel",
		xmLabelGadgetClass,
		pd->form,
		XmNlabelString, 	xmstr,
		XmNleftAttachment, 	XmATTACH_POSITION,
		XmNleftPosition, 	0,
		XmNleftOffset,		10,
		XmNtopAttachment,	XmATTACH_OPPOSITE_WIDGET,
		XmNtopWidget,		pd->report_type_option,
		XmNtopOffset,		0,
		NULL);
	  XmStringFree(xmstr);
	  XtManageChild(pd->from_label);

	  nargs = 0;
	  XtSetArg(args[nargs], XmNleftAttachment,
		   XmATTACH_OPPOSITE_WIDGET); nargs++;
	  XtSetArg(args[nargs], XmNleftWidget, pd->from_label); nargs++;
	  XtSetArg(args[nargs], XmNleftOffset, 0); nargs++;
	  XtSetArg(args[nargs], XmNtopAttachment, XmATTACH_WIDGET); nargs++;
	  XtSetArg(args[nargs], XmNtopWidget, pd->from_label); nargs++;
	  XtSetArg(args[nargs], XmNtopOffset, 4); nargs++;
	  XtSetArg(args[nargs], XmNshadowThickness, 1); nargs++;
	  pd->from_spin = XmCreateSpinBox(pd->form, "FromSpinBox",
					  args, nargs);
	  XtAddCallback(pd->from_spin, XmNmodifyVerifyCallback,
			from_modify_verify_cb, (XtPointer)c);

	  nargs = 0;
	  XtSetArg(args[nargs], XmNspinBoxChildType, XmNUMERIC); nargs++;
	  XtSetArg(args[nargs], XmNminimumValue, 1); nargs++;
	  XtSetArg(args[nargs], XmNmaximumValue, 31); nargs++;
	  XtSetArg(args[nargs], XmNcolumns, 2); nargs++;
	  XtSetArg(args[nargs], XmNmaxLength, 2); nargs++;
	  XtSetArg(args[nargs], XmNposition, 1); nargs++;
	  pd->from_day = XmCreateTextField(pd->from_spin, "FromDay",
					   args, nargs);
	  XtAddCallback(pd->from_day, XmNvalueChangedCallback,
			spin_field_changed_cb, (XtPointer)NULL);
	  XtManageChild(pd->from_day);

	  maxMonthLen = 0;
	  for (i = 0; i < 12; i++)
	  {
	    abbrevMonths[i] = XmStringCreateLocalized(months2[i + 1]);
	    if (cm_strlen(months2[i + 1]) > maxMonthLen)
	      maxMonthLen = cm_strlen(months2[i + 1]);
	  }
	  nargs = 0;
	  XtSetArg(args[nargs], XmNspinBoxChildType, XmSTRING); nargs++;
	  XtSetArg(args[nargs], XmNnumValues, 12); nargs++;
	  XtSetArg(args[nargs], XmNvalues, abbrevMonths); nargs++;
	  XtSetArg(args[nargs], XmNcolumns, maxMonthLen); nargs++;
	  XtSetArg(args[nargs], XmNmaxLength, maxMonthLen); nargs++;
	  XtSetArg(args[nargs], XmNposition, 0); nargs++;
	  pd->from_month = XmCreateTextField(pd->from_spin, "FromMonth",
					     args, nargs);
	  XtAddCallback(pd->from_month, XmNvalueChangedCallback,
			spin_field_changed_cb, (XtPointer)NULL);
	  XtManageChild(pd->from_month);

	  nargs = 0;
	  XtSetArg(args[nargs], XmNspinBoxChildType, XmNUMERIC); nargs++;
	  XtSetArg(args[nargs], XmNminimumValue,
		   year(get_bot())); nargs++;
	  XtSetArg(args[nargs], XmNmaximumValue,
		   year(get_eot())); nargs++;
	  XtSetArg(args[nargs], XmNcolumns, 4); nargs++;
	  XtSetArg(args[nargs], XmNmaxLength, 4); nargs++;
	  XtSetArg(args[nargs], XmNposition,
		   year(get_bot())); nargs++;
	  XtSetArg(args[nargs], XmNwrap, False); nargs++;
	  pd->from_year = XmCreateTextField(pd->from_spin, "FromYear",
					    args, nargs);
	  XtAddCallback(pd->from_year, XmNvalueChangedCallback,
			spin_field_changed_cb, (XtPointer)NULL);
	  XtManageChild(pd->from_year);
	  XtManageChild(pd->from_spin);

	  xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1,
						  732, "To:"));
	  pd->to_label = XtVaCreateWidget("ToLabel",
		xmLabelGadgetClass,
		pd->form,
		XmNlabelString, 	xmstr,
		XmNleftAttachment, 	XmATTACH_POSITION,
		XmNleftPosition, 	2,
		XmNleftOffset,		10,
		XmNtopAttachment,	XmATTACH_OPPOSITE_WIDGET,
		XmNtopWidget,		pd->report_type_option,
		XmNtopOffset,		0,
		NULL);
	  XmStringFree(xmstr);
	  XtManageChild(pd->to_label);

	  nargs = 0;
	  XtSetArg(args[nargs], XmNleftAttachment,
		   XmATTACH_OPPOSITE_WIDGET); nargs++;
	  XtSetArg(args[nargs], XmNleftWidget, pd->to_label); nargs++;
	  XtSetArg(args[nargs], XmNleftOffset, 0); nargs++;
	  XtSetArg(args[nargs], XmNtopAttachment, XmATTACH_WIDGET); nargs++;
	  XtSetArg(args[nargs], XmNtopWidget, pd->to_label); nargs++;
	  XtSetArg(args[nargs], XmNtopOffset, 4); nargs++;
	  XtSetArg(args[nargs], XmNshadowThickness, 1); nargs++;
	  pd->to_spin = XmCreateSpinBox(pd->form, "ToSpinBox",
					args, nargs);
	  XtAddCallback(pd->to_spin, XmNmodifyVerifyCallback,
			to_modify_verify_cb, (XtPointer)c);

	  nargs = 0;
	  XtSetArg(args[nargs], XmNspinBoxChildType, XmNUMERIC); nargs++;
	  XtSetArg(args[nargs], XmNminimumValue, 1); nargs++;
	  XtSetArg(args[nargs], XmNmaximumValue, 31); nargs++;
	  XtSetArg(args[nargs], XmNcolumns, 2); nargs++;
	  XtSetArg(args[nargs], XmNmaxLength, 2); nargs++;
	  XtSetArg(args[nargs], XmNposition, 1); nargs++;
	  pd->to_day = XmCreateTextField(pd->to_spin, "ToDay",
					   args, nargs);
	  XtAddCallback(pd->to_day, XmNvalueChangedCallback,
			spin_field_changed_cb, (XtPointer)NULL);
	  XtManageChild(pd->to_day);

	  nargs = 0;
	  XtSetArg(args[nargs], XmNspinBoxChildType, XmSTRING); nargs++;
	  XtSetArg(args[nargs], XmNnumValues, 12); nargs++;
	  XtSetArg(args[nargs], XmNvalues, abbrevMonths); nargs++;
	  XtSetArg(args[nargs], XmNcolumns, maxMonthLen); nargs++;
	  XtSetArg(args[nargs], XmNmaxLength, maxMonthLen); nargs++;
	  XtSetArg(args[nargs], XmNposition, 0); nargs++;
	  pd->to_month = XmCreateTextField(pd->to_spin, "ToMonth",
					   args, nargs);
	  XtAddCallback(pd->to_month, XmNvalueChangedCallback,
			spin_field_changed_cb, (XtPointer)NULL);
	  XtManageChild(pd->to_month);
	  for (i = 0; i < 12; i++)
	    XmStringFree(abbrevMonths[i]);

	  nargs = 0;
	  XtSetArg(args[nargs], XmNspinBoxChildType, XmNUMERIC); nargs++;
	  XtSetArg(args[nargs], XmNminimumValue,
		   year(get_bot())); nargs++;
	  XtSetArg(args[nargs], XmNmaximumValue,
		   year(get_eot())); nargs++;
	  XtSetArg(args[nargs], XmNcolumns, 4); nargs++;
	  XtSetArg(args[nargs], XmNmaxLength, 4); nargs++;
	  XtSetArg(args[nargs], XmNposition,
		   year(get_bot())); nargs++;
	  XtSetArg(args[nargs], XmNwrap, False); nargs++;
	  pd->to_year = XmCreateTextField(pd->to_spin, "ToYear",
					  args, nargs);
	  XtAddCallback(pd->to_year, XmNvalueChangedCallback,
			spin_field_changed_cb, (XtPointer)NULL);
	  XtManageChild(pd->to_year);
	  XtManageChild(pd->to_spin);

	  xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1,
						  679, "More..."));
	  nargs = 0;
	  XtSetArg(args[nargs], XmNlabelString, xmstr); nargs++;
	  XtSetArg(args[nargs], XmNrightAttachment, XmATTACH_FORM); nargs++;
	  XtSetArg(args[nargs], XmNrightOffset, 10); nargs++;
	  XtSetArg(args[nargs], XmNtopAttachment, XmATTACH_WIDGET); nargs++;
	  XtSetArg(args[nargs], XmNtopWidget, pd->to_spin); nargs++;
	  XtSetArg(args[nargs], XmNtopOffset, 10); nargs++;
	  pd->more_opts = XmCreatePushButtonGadget(pd->form, "MoreOpts",
						   args, nargs);
	  XtAddCallback(pd->more_opts, XmNactivateCallback,
			more_opts_cb, (XtPointer)c);
	  XtManageChild(pd->more_opts);
	  XmStringFree(xmstr);

#ifdef GR_DEBUG
	  nargs = 0;
	  XtSetArg(args[nargs], XmNleftAttachment, XmATTACH_FORM); nargs++;
	  XtSetArg(args[nargs], XmNleftOffset, 10); nargs++;
	  XtSetArg(args[nargs], XmNtopAttachment,
		   XmATTACH_OPPOSITE_WIDGET); nargs++;
	  XtSetArg(args[nargs], XmNtopWidget, pd->more_opts); nargs++;
	  XtSetArg(args[nargs], XmNtopOffset, 10); nargs++;
	  pd->debugToggle = XmCreateToggleButtonGadget(pd->form, "Debug Mode",
						       args, nargs);
	  XtManageChild(pd->debugToggle);
#endif

	  XtManageChild(pd->form);

	  pd->setupDataValid = False;
	  clearSetupData(pd);
	  pd->printShell = (Widget)NULL;
	  pd->badAllocError = False;
	}

	/* Set report type and dates according to current view/day. */
	switch (c->view->glance) {
	case dayGlance : reportType = PR_DAY_VIEW; break;
	case weekGlance : reportType = PR_WEEK_VIEW; break;
	case yearGlance : reportType = PR_YEAR_VIEW; break;
	default:
	case monthGlance : reportType = PR_MONTH_VIEW; break;
	}

	pd_set_report_type(c, reportType);

	pd_set_start_date(c, c->view->date);
	pd_set_end_date(c, c->view->date);
}

/*
 * Close dialog
 */
static void
cancel_cb(Widget w, XtPointer data, XtPointer ignore)
{
  Calendar *c = (Calendar *)data;
  _DtCmPrintData *pd = (_DtCmPrintData *)c->print_data;

  XtUnmanageChild(pd->pdb);
}

static void
report_error(Calendar *c, char *title, char *errText)
{
  Props_pu *pu = (Props_pu *)c->properties_pu;
  char *label;

  label = XtNewString(catgets(c->DT_catd, 1, 95, "Continue"));

  dialog_popup(c->frame,
	       DIALOG_TITLE, title,
	       DIALOG_TEXT, errText,
	       BUTTON_IDENT, 1, label,
	       DIALOG_IMAGE, pu->xm_error_pixmap,
	       NULL);

  XtFree(label);
}

static void
pdm_notify_cb(Widget w, XtPointer uData, XtPointer cbData)
{
#if defined(PRINTING_SUPPORTED)
  XmPrintShellCallbackStruct *cbStruct =
    (XmPrintShellCallbackStruct *)cbData;
  Calendar *c = (Calendar *)uData;
  char *errText = (char *)NULL;
  char *title;

  switch (cbStruct->reason)
  {
  case XmCR_PDM_NONE:
  case XmCR_PDM_START_ERROR:
  case XmCR_PDM_EXIT_ERROR:
    errText = XtNewString(catgets(c->DT_catd, 1, 1112, pdmErrorText));
    break;

  default:
    break;
  }

  if (errText)
  {
    title = XtNewString(catgets(c->DT_catd, 1, 1111, setupErrorTitle));

    report_error(c, title, errText);

    XtFree(title);
    XtFree(errText);
  }
#endif  /* PRINTING_SUPPORTED */
}

/*
 * Called when print dialog box's print display is closed.
 */
static void
close_print_display_cb(Widget w, XtPointer uData, XtPointer cbData)
{
  Calendar *c = (Calendar *)uData;
  _DtCmPrintData *pd = (_DtCmPrintData *)c->print_data;

  if (pd != (_DtCmPrintData *)NULL)
  {
    /* Destroy printShell associated with Display, if any. */
    if (pd->printShell != (Widget)NULL)
    {
      XtDestroyWidget(pd->printShell);
      pd->printShell = (Widget)NULL;
    }

    clearSetupData(pd);
  }
}

/*
 * Print callback: do the deed!
 */
static void
print_cb(Widget w, XtPointer data, XtPointer cbDataP)
{
   Calendar *c = (Calendar *)data;
   _DtCmPrintData *pd = (_DtCmPrintData *)c->print_data;
   DtPrintSetupCallbackStruct *cbStruct =
     (DtPrintSetupCallbackStruct *)cbDataP;

   clearSetupData(pd);
   DtPrintCopySetupData(&pd->setupData, cbStruct->print_data);
   pd->setupDataValid = True;

   _DtTurnOnHourGlass(c->frame);
   print_report(c);
   _DtTurnOffHourGlass(c->frame);

   /* Leave setupDataValid=True for quick print! */
}

static void
print_setup_cb(Widget w, XtPointer uData, XtPointer cbData)
{
#if defined(PRINTING_SUPPORTED)
  Calendar *c = (Calendar *)uData;
  _DtCmPrintData *pd = (_DtCmPrintData *)c->print_data;
  DtPrintSetupCallbackStruct *cbStruct =
    (DtPrintSetupCallbackStruct *)cbData;

  clearSetupData(pd);
  DtPrintCopySetupData(&pd->setupData, cbStruct->print_data);
  pd->setupDataValid = True;

#ifdef GR_DEBUG
  /* Force debug mode off. */
  if (inDebugMode(c))
    XmToggleButtonGadgetSetState(pd->debugToggle, False, False);
#endif

  createPrintShell(c);

  if (XmPrintPopupPDM(pd->printShell, w) != XmPDM_NOTIFY_SUCCESS)
  {
    char *errText = XtNewString(catgets(c->DT_catd, 1, 1112,
					pdmErrorText));
    char *title = XtNewString(catgets(c->DT_catd, 1, 1111, setupErrorTitle));

    report_error(c, title, errText);

    XtFree(title);
    XtFree(errText);
  }

  clearSetupData(pd);
#endif  /* PRINTING_SUPPORTED */
}

/*
 * print_report: dispatch the view-specific output routine
 */
void
print_report(Calendar *c)
{
  _DtCmPrintData *pd = (_DtCmPrintData *)c->print_data;
  Props 	*p = (Props *) c->properties;
  Props_pu	*pu = (Props_pu *) c->properties_pu;
  int 		r = pd_get_report_type(c);
  OrderingType 	ot = get_int_prop(p, CP_DATEORDERING);
  SeparatorType st = get_int_prop(p, CP_DATESEPARATOR);
  Tick		start_date = pd_get_start_date(c);
  Tick		end_date = pd_get_end_date(c);
  char *text = (char *)NULL;

  if ((start_date == DATE_BBOT) || (end_date == DATE_BBOT) || 
      (start_date == DATE_AEOT) || (end_date == DATE_BBOT))
  {
    text = XtNewString(catgets(c->DT_catd, 1, 892,
	"The dates for printing must be between 1969 and 2038."));
  }
  else if (start_date <= 0)
  {
    text = XtNewString(catgets(c->DT_catd, 1, 894,
			       "Malformed \"From\" date"));
  }
  else if (end_date <= 0)
  {
    text = XtNewString(catgets(c->DT_catd, 1, 896,
			       "Malformed \"To\" date"));
  }
  else if (start_date > end_date)
  {
    text = XtNewString(catgets(c->DT_catd, 1, 898,
"The \"To\" date for printing must be after the \"From\" date for printing"));
  }

  if (text)
  {
    char *title = XtNewString(catgets(c->DT_catd, 1, 736, printErrorTitle));

    report_error(c, title, text);

    XtFree(text);
    XtFree(title);
    return;
  }

  if (pd == (_DtCmPrintData *)NULL)
    return;

  if (!pd->setupDataValid)
  {
    if (DtPrintFillSetupData(pd->pdb, &pd->setupData) != DtPRINT_SUCCESS)
    {
      /*
       * NOTE: DtPrintFillSetupData() displays an error dialog if
       * it is unsuccessful; no need to display our own.
       */
      return;
    }

    pd->setupDataValid = True;
  }

  if (XtIsManaged(pd->pdb))
    _DtTurnOnHourGlass(XtParent(pd->pdb));

  createPrintShell(c);

  switch(r)
  {
  case PR_YEAR_VIEW:
    print_std_year_range(year(start_date), year(end_date));
    break;
  default:
  case PR_MONTH_VIEW:
    print_month_range(c, start_date, end_date);
    break;
  case PR_WEEK_VIEW:
    print_week_range(c, start_date, end_date);
    break;
  case PR_DAY_VIEW:
    print_day_range(c, start_date, end_date);
    break;
  case PR_APPT_LIST:
    x_print_list_range(c, CSA_TYPE_EVENT, (int) c->view->glance,
		       start_date, end_date);
    break;
  case PR_TODO_LIST:
    x_print_list_range(c, CSA_TYPE_TODO, (int) VIEW_ALL,
		       start_date, end_date);
    break;
  }

#ifdef GR_DEBUG
  if (inDebugMode(c))
  {
    XtDestroyWidget(pd->printShell);
    pd->printShell = (Widget)NULL;
  }
#endif

  if (XtIsManaged(pd->pdb))
  {
    _DtTurnOffHourGlass(XtParent(pd->pdb));
    XtUnmanageChild(pd->pdb);
  }

  /* Keep setupDataValid=True for future QuickPrint operations. */
}

/*
 * report_option_cb
 *
 * callback for report-type option menu.
 * Here we just store the button number of the selected
 * button in the print_data structure, to make life easy
 * when the print callback is invoked.
 */
static void
report_option_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
  int choice = (int) (intptr_t) client_data;
  Calendar *c = calendar;

  pd_set_report_managed(c, choice);

  pd_set_start_date(c, pd_get_start_date(c));
  pd_set_end_date(c, pd_get_end_date(c));
}

char *
pd_get_printer_name(Calendar *c)
{
  _DtCmPrintData *pd = (_DtCmPrintData *)c->print_data;

  if ((pd == (_DtCmPrintData *)NULL) ||
      (!pd->setupDataValid) ||
      pd_print_to_file(c))
    return (char *)NULL;

  return pd->setupData.printer_name ?
    XtNewString(pd->setupData.printer_name) : (char *)NULL;
}

int
pd_get_copies(Calendar *c)
{
  _DtCmPrintData *pd = (_DtCmPrintData *)c->print_data;
  int nCopies = 0;

  if (pd == (_DtCmPrintData *)NULL)
    return 0;

  XtVaGetValues(pd->pdb, DtNcopies, &nCopies, NULL);
  return nCopies;
}

Tick
pd_get_start_date(Calendar *c)
{
  int dayPos, monthPos, yearPos;

  if (!pd_get_start_positions(c, &monthPos, &dayPos, &yearPos))
    return 0;

  return monthdayyear(monthPos + 1, dayPos, yearPos);
}

Tick
pd_get_end_date(Calendar *c)
{
  int dayPos, monthPos, yearPos;

  if (!pd_get_end_positions(c, &monthPos, &dayPos, &yearPos))
    return 0;

  return monthdayyear(monthPos + 1, dayPos, yearPos);
}

char *
pd_get_print_options(Calendar *c)
{
  return XtNewString("");
}

Boolean
pd_print_to_file(Calendar *c)
{
  _DtCmPrintData *pd = (_DtCmPrintData *)c->print_data;

  if ((pd == (_DtCmPrintData *)NULL) ||
      (!pd->setupDataValid))
    return False;

  return pd->setupData.destination == DtPRINT_TO_FILE;
}

char *
pd_get_file_name(Calendar *c)
{
  _DtCmPrintData *pd = (_DtCmPrintData *)c->print_data;

  if ((pd == (_DtCmPrintData *)NULL) ||
      (!pd->setupDataValid) ||
      !pd_print_to_file(c))
    return (char *)NULL;

  return pd->setupData.dest_info ?
    XtNewString(pd->setupData.dest_info) : (char *)NULL;
}

int
pd_get_report_type(Calendar *c)
{
  _DtCmPrintData *pd = (_DtCmPrintData *)c->print_data;

  if (pd == (_DtCmPrintData *)NULL)
    return PR_MONTH_VIEW;

  return pd->report_type;
}

Widget
pd_get_print_shell(Calendar *c)
{
  _DtCmPrintData *pd = (_DtCmPrintData *)c->print_data;

  if (pd == (_DtCmPrintData *)NULL)
    return (Widget)NULL;

  return pd->printShell;
}

Display *
pd_get_print_display(Calendar *c)
{
  Widget printShell = pd_get_print_shell(c);

  if (printShell == (Widget)NULL)
      return (Display *)NULL;

  return XtDisplay(printShell);
}

void
pd_set_bad_alloc_error(Calendar *c, Boolean errorOn)
{
  _DtCmPrintData *pd = (_DtCmPrintData *)c->print_data;

  if (pd != (_DtCmPrintData *)NULL)
      pd->badAllocError = errorOn;
}

Boolean
pd_get_bad_alloc_error(Calendar *c)
{
  _DtCmPrintData *pd = (_DtCmPrintData *)c->print_data;

  if (pd != (_DtCmPrintData *)NULL)
      return pd->badAllocError;
      
  return False;
}
