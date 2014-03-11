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
/* $XConsortium: x_graphics.c /main/15 1996/11/25 10:29:30 rswiston $ */
/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

#include <EUSCompat.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include <locale.h>
#ifdef SVR4
#include <unistd.h>
#endif /* SVR4 */
#include <sys/stat.h>
#include <sys/param.h>
#include "util.h"
#include "calendar.h"
#include "timeops.h"
#include "datefield.h"
#include "props.h"
#include "props_pu.h"
#include "x_graphics.h"
#include "todo.h"
#include "cm_i18n.h"
#include "dayglance.h"
#include "format.h"
#include "deskset.h"
#include "print.h"

#include <X11/Intrinsic.h>
#include <X11/extensions/Print.h>

#include <Xm/Xm.h>
#include <Xm/DialogS.h>
#include <Xm/DrawingA.h>
#include <Xm/Print.h>

#define XOS_USE_XT_LOCKING
#define X_INCLUDE_TIME_H
#if defined(linux)
#undef SVR4
#endif
#include <X11/Xos_r.h>

/*
 *  There is currently a defect in the X Print Server which
 *  causes the print shell to be created with dimensions 1x1
 *  instead of the dimensions appropriate to the X Print Server.
 *  Until this is corrected, defining SHELL_SIZE_BUG will
 *  cause dtcm to size the print shell to 8.5"x11.0".
 */
#define SHELL_SIZE_BUG 1

#define BUFFERSIZE 512
#define DAYS_IN_WEEK 7
#define WEEK_DAYS 5
#define WEEKEND_DAYS 2
#define INTERVALS_PER_DAY 96 /* 4 15-minute intervals per hour * 24 hours */

#define VMARGIN(gInfo) (2 * gInfo->normalWd)
#define HMARGIN(gInfo) (4 * gInfo->normalWd)
#define TODO_HINDENT 10
#define TODO_VINDENT 10

/* Day View fonts */
enum
{
  DV_TITLE_FONT = 0,
  DV_HEADER_FONT,
  DV_APPT_FONT,
  DV_TIME_FONT,
  DV_NUM_FONTS
};
static String dvFonts[DV_NUM_FONTS];

/* Week View fonts */
enum
{
  WV_TITLE_FONT = 0,
  WV_HEADER_FONT,
  WV_APPT_FONT,
  WV_DAY_FONT,
  WV_BOX_FONT,
  WV_NUM_FONTS
};
static String wvFonts[WV_NUM_FONTS];

/* Month View fonts */
enum
{
  MV_TITLE_FONT = 0,
  MV_HEADER_FONT,
  MV_APPT_FONT,
  MV_DAY_FONT,
  MV_DATE_FONT,
  MV_TINY_TITLE_FONT,
  MV_TINY_DAY_FONT,
  MV_TINY_DATE_FONT,
  MV_NUM_FONTS
};
static String mvFonts[MV_NUM_FONTS];

/* Year View fonts */
enum
{
  YV_TITLE_FONT = 0,
  YV_HEADER_FONT,
  YV_TINY_TITLE_FONT,
  YV_TINY_DAY_FONT,
  YV_TINY_DATE_FONT,
  YV_NUM_FONTS
};
static String yvFonts[YV_NUM_FONTS];

/* Appt View fonts */
enum
{
  AV_TITLE_FONT = 0,
  AV_HEADER_FONT,
  AV_APPT_FONT,
  AV_ITEM_FONT,
  AV_TINY_TITLE_FONT,
  AV_TINY_DAY_FONT,
  AV_TINY_DATE_FONT,
  AV_NUM_FONTS
};
static String avFonts[AV_NUM_FONTS];

/* ToDo View fonts */
enum
{
  TV_TITLE_FONT = 0,
  TV_HEADER_FONT,
  TV_APPT_FONT,
  TV_ITEM_FONT,
  TV_TINY_TITLE_FONT,
  TV_TINY_DAY_FONT,
  TV_TINY_DATE_FONT,
  TV_NUM_FONTS
};
static String tvFonts[TV_NUM_FONTS];

#ifdef GR_DEBUG
typedef struct _PrintResources
{
  Pixel lightGrayPixel;
  Pixel darkGrayPixel;
} PrintResources;
#endif

/*
 *  Unfortunately, the dt-application fonts don't support all the point
 *  sizes we'd like.  We use the closest approximation.
 */
#ifdef GR_DEBUG
static char bold40PtFont[] =
  "-*-times-bold-r-normal-*-0-[80 0 0 40]-0-0-p-0-*-*:";
static char bold30PtFont[] =
  "-*-times-bold-r-normal-*-0-300-0-0-p-0-*-*:";
static char bold20PtFont[] =
  "-*-times-bold-r-normal-*-0-200-0-0-p-0-*-*:";
static char bold16PtFont[] =
  "-*-times-bold-r-normal-*-0-160-0-0-p-0-*-*:";
static char bold14PtFont[] =
  "-*-times-bold-r-normal-*-0-140-0-0-p-0-*-*:";
static char bold12PtFont[] =
  "-*-times-bold-r-normal-*-0-120-0-0-p-0-*-*:";
static char bold10PtFont[] =
  "-*-times-bold-r-normal-*-0-100-0-0-p-0-*-*:";
static char bold9PtFont[] =
  "-*-times-bold-r-normal-*-0-90-0-0-p-0-*-*:";
static char bold7PtFont[] =
  "-*-times-bold-r-normal-*-0-70-0-0-p-0-*-*:";

static char helv10PtFont[] =
  "-*-helvetica-medium-r-normal-*-0-100-0-0-p-0-*-*:";
static char helv9PtFont[] =
  "-*-helvetica-medium-r-normal-*-0-90-0-0-p-0-*-*:";
static char helv8PtFont[] =
  "-*-helvetica-medium-r-normal-*-0-80-0-0-p-0-*-*:";

static char cour12PtFont[] =
  "-*-courier-medium-r-normal-*-0-120-0-0-m-0-*-*:";
static char cour7PtFont[] =
  "-*-courier-medium-r-normal-*-0-70-0-0-m-0-*-*:";
#else
static char bold40PtFont[] =
  "-dt-application-bold-r-normal-serif-0-[80 0 0 40]-0-0-p-0-*-*:";
static char bold30PtFont[] =
  "-dt-application-bold-r-normal-serif-0-300-0-0-p-0-*-*:";
static char bold20PtFont[] =
  "-dt-application-bold-r-normal-serif-0-200-0-0-p-0-*-*:";
static char bold16PtFont[] =
  "-dt-application-bold-r-normal-serif-0-160-0-0-p-0-*-*:";
static char bold14PtFont[] =
  "-dt-application-bold-r-normal-serif-0-140-0-0-p-0-*-*:";
static char bold12PtFont[] =
  "-dt-application-bold-r-normal-serif-0-120-0-0-p-0-*-*:";
static char bold10PtFont[] =
  "-dt-application-bold-r-normal-serif-0-100-0-0-p-0-*-*:";
static char bold9PtFont[] =
  "-dt-application-bold-r-normal-serif-0-90-0-0-p-0-*-*:";
static char bold7PtFont[] =
  "-dt-application-bold-r-normal-serif-0-70-0-0-p-0-*-*:";

static char helv10PtFont[] =
  "-dt-application-medium-r-normal-sans-0-100-0-0-p-0-*-*:";
static char helv9PtFont[] =
  "-dt-application-medium-r-normal-sans-0-90-0-0-p-0-*-*:";
static char helv8PtFont[] =
  "-dt-application-medium-r-normal-sans-0-80-0-0-p-0-*-*:";

static char cour12PtFont[] =
  "-dt-application-medium-r-normal-serif-0-120-0-0-m-0-*-*:";
static char cour7PtFont[] =
  "-dt-application-medium-r-normal-serif-0-70-0-0-m-0-*-*:";
#endif

static XtResource dvDefaultResources[] =
{
  {"dvpTitleFont", "PrintTitleFont", XmRString, sizeof(String),
     sizeof(String) * DV_TITLE_FONT, XmRImmediate,
     (XtPointer)bold16PtFont},
  {"dvpHeaderFont", "PrintHeaderFont", XmRString, sizeof(String),
     sizeof(String) * DV_HEADER_FONT, XmRImmediate,
     (XtPointer)bold9PtFont},
  {"dvpApptFont", "PrintApptFont", XmRString, sizeof(String),
     sizeof(String) * DV_APPT_FONT, XmRImmediate,
     (XtPointer)helv10PtFont},
  {"dvpTimeFont", "PrintTimeFont", XmRString, sizeof(String),
     sizeof(String) * DV_TIME_FONT, XmRImmediate,
     (XtPointer)bold10PtFont}
};

static XtResource wvDefaultResources[] =
{
  {"wvpTitleFont", "PrintTitleFont", XmRString, sizeof(String),
     sizeof(String) * WV_TITLE_FONT, XmRImmediate,
     (XtPointer)bold16PtFont},
  {"wvpHeaderFont", "PrintHeaderFont", XmRString, sizeof(String),
     sizeof(String) * WV_HEADER_FONT, XmRImmediate,
     (XtPointer)bold9PtFont},
  {"wvpApptFont", "PrintApptFont", XmRString, sizeof(String),
     sizeof(String) * WV_APPT_FONT, XmRImmediate,
     (XtPointer)helv9PtFont},
  {"wvpDayFont", "PrintDayFont", XmRString, sizeof(String),
     sizeof(String) * WV_DAY_FONT, XmRImmediate,
     (XtPointer)bold12PtFont},
  {"wvpBoxFont", "PrintBoxFont", XmRString, sizeof(String),
     sizeof(String) * WV_BOX_FONT, XmRImmediate,
     (XtPointer)bold12PtFont}
};

static XtResource mvDefaultResources[] =
{
  {"mvpTitleFont", "PrintTitleFont", XmRString, sizeof(String),
     sizeof(String) * MV_TITLE_FONT, XmRImmediate,
     (XtPointer)bold30PtFont},
  {"mvpHeaderFont", "PrintHeaderFont", XmRString, sizeof(String),
     sizeof(String) * MV_HEADER_FONT, XmRImmediate,
     (XtPointer)bold9PtFont},
  {"mvpApptFont", "PrintApptFont", XmRString, sizeof(String),
     sizeof(String) * MV_APPT_FONT, XmRImmediate,
     (XtPointer)helv8PtFont},
  {"mvpDayFont", "PrintDayFont", XmRString, sizeof(String),
     sizeof(String) * MV_DAY_FONT, XmRImmediate,
     (XtPointer)bold14PtFont},
  {"mvpDateFont", "PrintDateFont", XmRString, sizeof(String),
     sizeof(String) * MV_DATE_FONT, XmRImmediate,
     (XtPointer)bold20PtFont},
  {"mvpTinyTitleFont", "PrintTinyTitleFont", XmRString, sizeof(String),
     sizeof(String) * MV_TINY_TITLE_FONT, XmRImmediate,
     (XtPointer)bold10PtFont},
  {"mvpTinyDayFont", "PrintTinyDayFont", XmRString, sizeof(String),
     sizeof(String) * MV_TINY_DAY_FONT, XmRImmediate,
     (XtPointer)cour7PtFont},
  {"mvpTinyDateFont", "PrintTinyDateFont", XmRString, sizeof(String),
     sizeof(String) * MV_TINY_DATE_FONT, XmRImmediate,
     (XtPointer)bold7PtFont}
};

static XtResource yvDefaultResources[] =
{
  {"yvpTitleFont", "PrintTitleFont", XmRString, sizeof(String),
     sizeof(String) * YV_TITLE_FONT, XmRImmediate,
     (XtPointer)bold40PtFont},
  {"yvpHeaderFont", "PrintHeaderFont", XmRString, sizeof(String),
     sizeof(String) * YV_HEADER_FONT, XmRImmediate,
     (XtPointer)bold9PtFont},
  {"yvpTinyTitleFont", "PrintTinyTitleFont", XmRString, sizeof(String),
     sizeof(String) * YV_TINY_TITLE_FONT, XmRImmediate,
     (XtPointer)bold16PtFont},
  {"yvpTinyDayFont", "PrintTinyDayFont", XmRString, sizeof(String),
     sizeof(String) * YV_TINY_DAY_FONT, XmRImmediate,
     (XtPointer)cour12PtFont},
  {"yvpTinyDateFont", "PrintTinyDateFont", XmRString, sizeof(String),
     sizeof(String) * YV_TINY_DATE_FONT, XmRImmediate,
     (XtPointer)bold12PtFont}
};

static XtResource avDefaultResources[] =
{
  {"avpTitleFont", "PrintTitleFont", XmRString, sizeof(String),
     sizeof(String) * AV_TITLE_FONT, XmRImmediate,
     (XtPointer)bold16PtFont},
  {"avpHeaderFont", "PrintHeaderFont", XmRString, sizeof(String),
     sizeof(String) * AV_HEADER_FONT, XmRImmediate,
     (XtPointer)bold9PtFont},
  {"avpApptFont", "PrintApptFont", XmRString, sizeof(String),
     sizeof(String) * AV_APPT_FONT, XmRImmediate,
     (XtPointer)helv10PtFont},
  {"avpItemFont", "PrintItemFont", XmRString, sizeof(String),
     sizeof(String) * AV_ITEM_FONT, XmRImmediate,
     (XtPointer)bold10PtFont},
  {"avpTinyTitleFont", "PrintTinyTitleFont", XmRString, sizeof(String),
     sizeof(String) * AV_TINY_TITLE_FONT, XmRImmediate,
     (XtPointer)bold16PtFont},
  {"avpTinyDayFont", "PrintTinyDayFont", XmRString, sizeof(String),
     sizeof(String) * AV_TINY_DAY_FONT, XmRImmediate,
     (XtPointer)cour12PtFont},
  {"avpTinyDateFont", "PrintTinyDateFont", XmRString, sizeof(String),
     sizeof(String) * AV_TINY_DATE_FONT, XmRImmediate,
     (XtPointer)bold12PtFont}
};

static XtResource tvDefaultResources[] =
{
  {"tvpTitleFont", "PrintTitleFont", XmRString, sizeof(String),
     sizeof(String) * TV_TITLE_FONT, XmRImmediate,
     (XtPointer)bold16PtFont},
  {"tvpHeaderFont", "PrintHeaderFont", XmRString, sizeof(String),
     sizeof(String) * TV_HEADER_FONT, XmRImmediate,
     (XtPointer)bold9PtFont},
  {"tvpApptFont", "PrintApptFont", XmRString, sizeof(String),
     sizeof(String) * TV_APPT_FONT, XmRImmediate,
     (XtPointer)helv10PtFont},
  {"tvpItemFont", "PrintItemFont", XmRString, sizeof(String),
     sizeof(String) * TV_ITEM_FONT, XmRImmediate,
     (XtPointer)bold10PtFont},
  {"tvpTinyTitleFont", "PrintTinyTitleFont", XmRString, sizeof(String),
     sizeof(String) * TV_TINY_TITLE_FONT, XmRImmediate,
     (XtPointer)bold16PtFont},
  {"tvpTinyDayFont", "PrintTinyDayFont", XmRString, sizeof(String),
     sizeof(String) * TV_TINY_DAY_FONT, XmRImmediate,
     (XtPointer)cour12PtFont},
  {"tvpTinyDateFont", "PrintTinyDateFont", XmRString, sizeof(String),
     sizeof(String) * TV_TINY_DATE_FONT, XmRImmediate,
     (XtPointer)bold12PtFont}
};

#ifdef GR_DEBUG
static XtResource displayResources[] =
{
  {"lightGrayPixel", "LightGrayPixel", XmRPixel, sizeof(Pixel),
     XtOffsetOf(PrintResources, lightGrayPixel), XmRString,
     "yellow"},
  {"darkGrayPixel", "DarkGrayPixel", XmRPixel, sizeof(Pixel),
     XtOffsetOf(PrintResources, darkGrayPixel), XmRString,
     "red"}
};

static PrintResources printResources;
#endif

typedef int CMFontEnum;

enum {
  LEFT_AT_X = 1,
  CENTER_AT_X = 2,
  RIGHT_AT_X = 4,
  TOP_AT_Y = 8,
  CENTER_AT_Y = 16,
  BOTTOM_AT_Y = 32
};

typedef struct _CMFontInfo
{
  CMFontEnum fontEnum;
  XmFontList fontList;
} CMFontInfo;

typedef struct _CMDayInfo
{
  int curX1, curY1;
  int curX2, curY2;
  int tab1;
  int boxHt;
  int boxWd;
  int line1;
} CMDayInfo;

typedef struct _CMToDoInfo
{
  int outerX1, outerY1;
  int innerX1, innerY1;
  int innerX2, innerY2;
  int outerX2, outerY2;
  int boxX1, boxY1;
  int boxX2, boxY2;
  int lineX, lineY;
} CMToDoInfo;

typedef struct _CMWeekInfo
{
  int curX1, curY1;
  int curX2, curY2;
  int tab1;
  int boxHt;
  int boxWd;
  int lineX;
  int lineY;
  int lineMaxY;
  int schedBoxX1, schedBoxY1;
  int schedBoxX2, schedBoxY2;
  char schedBucket[INTERVALS_PER_DAY];
} CMWeekInfo;

typedef struct _CMMonthInfo
{
  int curX1, curY1;
  int boxWd, boxHt;
  int tab1;
  int nRows;
  int lineX, lineY;
  int lastX;
  int lineMaxX, lineMaxY;
} CMMonthInfo;

typedef struct _CMGraphicsInfo
{
  Widget drawingArea;
  GC gc;
  Calendar *c;
  int reportType;

  char timestamp[BUFFERSIZE];
  Boolean reverseVideo;
  Pixel fg, bg;
  Pixmap lightPixmap, darkPixmap;

  double hscale;
  double vscale;

  /* Scaled line thicknesses */
  int thinWd;
  int normalWd;
  int thickWd;

  int leftMarginX;
  int rightMarginX;
  int topMarginY;
  int bottomMarginY;

  CMFontInfo *fontInfo;
  int nFonts;
  CMFontInfo *curFont;

  union
  {
      CMDayInfo dayInfo;
      CMToDoInfo toDoInfo;
      CMWeekInfo weekInfo;
      CMMonthInfo monthInfo;
  } u;

} CMGraphicsInfo;

static void local_dayname(Calendar *, char **, int);
static void local_dayname3(Calendar *, char **, int);
static char *get_report_type_string(CMGraphicsInfo *);
static void filePrintDoneCB(Display *, XPContext, XPGetDocStatus,
			    XPointer);
static void filePrintReportStatus(Calendar *, Boolean);
static void showBadAllocError(Calendar *);

/* X Graphics stuff. */
static void x_print_string(CMGraphicsInfo *, char *, int, int, int);
static void x_fill_light_box(CMGraphicsInfo *, int, int, int, int, int);
static void x_fill_dark_box(CMGraphicsInfo *, int, int, int, int, int);
static void x_fill_black_box(CMGraphicsInfo *, int, int, int, int);
static void x_fill_box(CMGraphicsInfo *, int, int, int, int, Pixmap, int);
static void x_draw_box(CMGraphicsInfo *, int, int, int, int, int);
static void x_draw_line(CMGraphicsInfo *, int, int, int, int, int);
static void x_set_clip_box(CMGraphicsInfo *, int, int, int, int);
static void x_unset_clip_box(CMGraphicsInfo *);

/* X Font stuff. */
static Boolean x_load_font(CMGraphicsInfo *, CMFontInfo *, char *);

/* CM Font stuff. */
static Boolean cm_load_font(CMGraphicsInfo *, CMFontEnum, char *);
static void cm_load_font_error(CMGraphicsInfo *, char *);
static void cm_release_fonts(CMGraphicsInfo *);
static Boolean cm_set_font(CMGraphicsInfo *, CMFontEnum);
static CMFontInfo *cm_find_font(CMGraphicsInfo *, CMFontEnum);
static void cm_font_extents(CMGraphicsInfo *, CMFontInfo *, int *, int *);
static int cm_font_height(CMGraphicsInfo *, CMFontInfo *);
static int cm_font_width(CMGraphicsInfo *, CMFontInfo *);
static int cm_string_width(CMGraphicsInfo *, CMFontInfo *, char *);
static void cm_reverse_video(CMGraphicsInfo *, Boolean);

static CMFontEnum cm_get_title_enum(CMGraphicsInfo *);
static CMFontEnum cm_get_header_enum(CMGraphicsInfo *);
static CMFontEnum cm_get_tiny_title_enum(CMGraphicsInfo *);
static CMFontEnum cm_get_tiny_day_enum(CMGraphicsInfo *);
static CMFontEnum cm_get_tiny_date_enum(CMGraphicsInfo *);
static void cm_adjust_margins(CMGraphicsInfo *);
static int cm_cvt_margin(CMGraphicsInfo *, Props_op, int);
static void cm_print_header_string(CMGraphicsInfo *, Props_op,
				   char *, char *,
				   char *, char *);

/* Day/Week View stuff */
static void cm_print_time(CMGraphicsInfo *, char *, Glance);
static void cm_print_appt_text(CMGraphicsInfo *, char *, int, Glance);
static void cm_print_text(CMGraphicsInfo *, char *, int, Glance);
static void cm_week_sched_update(CMGraphicsInfo *,
				 Dtcm_appointment *, Props *);
static int cm_week_sched_box_indent(CMGraphicsInfo *);

/* ToDo/Appt. View stuff */
static void cm_set_todo_text_clipping(CMGraphicsInfo *);
static void cm_unset_todo_text_clipping(CMGraphicsInfo *);
static void cm_print_todo_text(CMGraphicsInfo *, char *, CSA_sint32,
			       CSA_sint32, int);
static int cm_tiny_month_height(CMGraphicsInfo *, int);
static void cm_print_todo_months(CMGraphicsInfo *, time_t);
static int cm_count_appts(CMGraphicsInfo *, CSA_entry_handle *,
			  int, CSA_sint32, int);
static void cm_get_todo_lines_per_page(CMGraphicsInfo *, CSA_sint32,
				       int *, int *);
static void cm_todo_outline(CMGraphicsInfo *, Boolean);

/* Month View stuff */
static void cm_print_month_time(CMGraphicsInfo *, char *);
static void cm_print_month_text(CMGraphicsInfo *, char *, int);
static void cm_std_month_dates(CMGraphicsInfo *, int, int, int,
			       int, int, int, int);
static void cm_std_month_weekdays(CMGraphicsInfo *, int, int, int, int);

#ifdef GR_DEBUG
static void tmpFn(Widget, XtPointer, XtPointer);
static void tmpSpin(Widget);
static Boolean tmpFlag;

static void
tmpSpin(Widget w)
{
  XEvent event;
  XtAppContext appContext;

  tmpFlag = False;
  appContext = XtWidgetToApplicationContext(w);
  while (!tmpFlag)
  {
    XtAppNextEvent(appContext, &event);
    XtDispatchEvent(&event);
  }
}

static void
tmpFn(Widget w, XtPointer uData, XtPointer cbData)
{
  XmDrawingAreaCallbackStruct *cbStruct =
    (XmDrawingAreaCallbackStruct *)cbData;

  if (cbStruct->reason == XmCR_EXPOSE)
  {
    if (cbStruct->event->xexpose.count != 0)
      return;
  }
  else
  {
    if (cbStruct->event->type != ButtonPress)
      return;
  }

  tmpFlag = True;
}
#endif

static void
x_print_string(CMGraphicsInfo *gInfo, char *str, int x, int y, int flags)
{
  Widget w = gInfo->drawingArea;
  GC gc = gInfo->gc;
  XmFontList fontList = (XmFontList)NULL;
  XmString xmString;
  Dimension textWd, textHt;
  int textX, textY;
  int xFlags, yFlags;
  XmStringDirection xmStringDirection;
  Arg args[10];
  int nargs;

  nargs = 0;
  XtSetArg(args[nargs], XmNstringDirection, &xmStringDirection); nargs++;
  XtGetValues(w, args, nargs);

  if ((gInfo->nFonts == 0) ||
      (gInfo->curFont == (CMFontInfo *)NULL))
  {
    nargs = 0;
    XtSetArg(args[nargs], XmNlabelFontList, &fontList); nargs++;
    XtGetValues(XtParent(w), args, nargs);
  }
  else
  {
    fontList = gInfo->curFont->fontList;
  }

  xmString = XmStringCreateLocalized(str);
  XmStringExtent(fontList, xmString, &textWd, &textHt);

  xFlags = (flags & (LEFT_AT_X | CENTER_AT_X | RIGHT_AT_X));
  yFlags = (flags & (TOP_AT_Y | CENTER_AT_Y | BOTTOM_AT_Y));

  switch (xFlags)
  {
  default:
  case LEFT_AT_X:
    textX = x;
    break;

  case CENTER_AT_X:
    textX = x - (textWd/2);
    break;

  case RIGHT_AT_X:
    textX = x - textWd + 1;
    break;
  }
  if (xmStringDirection == XmSTRING_DIRECTION_R_TO_L)
    textX += textWd;

  switch (yFlags)
  {
  default:
  case TOP_AT_Y:
    textY = y;
    break;

  case CENTER_AT_Y:
    textY = y - (textHt/2);
    break;

  case BOTTOM_AT_Y:
    textY = y - textHt;
    break;
  }

  XSetForeground(XtDisplay(w), gc, gInfo->fg);
  XmStringDraw(XtDisplay(w), XtWindow(w), fontList, xmString,
	       gc, textX, textY, textWd, XmALIGNMENT_BEGINNING,
	       xmStringDirection, NULL);
  XmStringFree(xmString);
}

static void
x_fill_light_box(CMGraphicsInfo *gInfo, int x1, int y1, int x2, int y2,
		 int lineWidth)
{
#ifdef GR_DEBUG
  Widget w = gInfo->drawingArea;
  GC gc = gInfo->gc;
  int fillX, fillY;
  int fillWd, fillHt;

  XSetForeground(XtDisplay(w), gc, printResources.lightGrayPixel);

  fillX = x1;
  fillY = y1;
  fillWd = x2 - x1 + 1;
  fillHt = y2 - y1 + 1;
  XFillRectangle(XtDisplay(w), XtWindow(w), gc, fillX, fillY, fillWd, fillHt);

  if (lineWidth > 0)
    x_draw_box(gInfo, x1, y1, x2, y2, lineWidth);
#else
  static char lightStippleData[] = {0x10, 0x00, 0x01, 0x00};

  if (gInfo->lightPixmap == None)
  {
    Widget w = gInfo->drawingArea;
    Arg args[10];
    int nargs;
    unsigned long fg, bg;
    int depth;
    int lightStippleWd = 8;
    int lightStippleHt = 4;

    if (gInfo->reverseVideo)
    {
      fg = gInfo->bg;
      bg = gInfo->fg;
    }
    else
    {
      fg = gInfo->fg;
      bg = gInfo->bg;
    }

    nargs = 0;
    XtSetArg(args[nargs], XmNdepth, &depth); nargs++;
    XtGetValues(w, args, nargs);

    gInfo->lightPixmap =
      XCreatePixmapFromBitmapData(XtDisplay(w), XtWindow(w),
				  lightStippleData,
				  lightStippleWd, lightStippleHt,
				  fg, bg, depth);
  }

  x_fill_box(gInfo, x1, y1, x2, y2, gInfo->lightPixmap, lineWidth);
#endif
}

static void
x_fill_dark_box(CMGraphicsInfo *gInfo, int x1, int y1, int x2, int y2,
		int lineWidth)
{
#ifdef GR_DEBUG
  Widget w = gInfo->drawingArea;
  GC gc = gInfo->gc;
  int fillX, fillY;
  int fillWd, fillHt;

  XSetForeground(XtDisplay(w), gc, printResources.darkGrayPixel);

  fillX = x1;
  fillY = y1;
  fillWd = x2 - x1 + 1;
  fillHt = y2 - y1 + 1;
  XFillRectangle(XtDisplay(w), XtWindow(w), gc, fillX, fillY, fillWd, fillHt);

  if (lineWidth > 0)
    x_draw_box(gInfo, x1, y1, x2, y2, lineWidth);
#else
  static char darkStippleData[] = {0x11, 0xaa, 0x44, 0xaa};

  if (gInfo->darkPixmap == None)
  {
    Widget w = gInfo->drawingArea;
    Arg args[10];
    int nargs;
    unsigned long fg, bg;
    int depth;
    int darkStippleWd = 8;
    int darkStippleHt = 4;

    if (gInfo->reverseVideo)
    {
      fg = gInfo->bg;
      bg = gInfo->fg;
    }
    else
    {
      fg = gInfo->fg;
      bg = gInfo->bg;
    }

    nargs = 0;
    XtSetArg(args[nargs], XmNdepth, &depth); nargs++;
    XtGetValues(w, args, nargs);

    gInfo->darkPixmap =
      XCreatePixmapFromBitmapData(XtDisplay(w), XtWindow(w),
				  darkStippleData,
				  darkStippleWd, darkStippleHt,
				  fg, bg, depth);
  }

  x_fill_box(gInfo, x1, y1, x2, y2, gInfo->darkPixmap, lineWidth);
#endif
}

static void
x_fill_black_box(CMGraphicsInfo *gInfo, int x1, int y1, int x2, int y2)
{
  x_fill_box(gInfo, x1, y1, x2, y2, None, 0);
}

static void
x_fill_box(CMGraphicsInfo *gInfo, int x1, int y1,
	   int x2, int y2, Pixmap stipple, int lineWidth)
{
  Widget w = gInfo->drawingArea;
  GC gc = gInfo->gc;
  int fillX, fillY;
  int fillWd, fillHt;

  XSetForeground(XtDisplay(w), gc, gInfo->fg);
  XSetBackground(XtDisplay(w), gc, gInfo->bg);

  if (stipple != None)
  {
    XSetTile(XtDisplay(w), gc, stipple);
    XSetFillStyle(XtDisplay(w), gc, FillTiled);
  }

  fillX = x1;
  fillY = y1;
  fillWd = x2 - x1 + 1;
  fillHt = y2 - y1 + 1;
  XFillRectangle(XtDisplay(w), XtWindow(w), gc, fillX, fillY, fillWd, fillHt);

  if (stipple != None)
    XSetFillStyle(XtDisplay(w), gc, FillSolid);

  if (lineWidth > 0)
    x_draw_box(gInfo, x1, y1, x2, y2, lineWidth);
}

static void
x_draw_box(CMGraphicsInfo *gInfo, int x1, int y1,
	   int x2, int y2, int lineWidth)
{
  Widget w = gInfo->drawingArea;
  GC gc = gInfo->gc;
  XGCValues newGCValues;

  if (lineWidth == 0)
    lineWidth = 1;

  newGCValues.line_width = lineWidth;
  newGCValues.foreground = gInfo->fg;
  XChangeGC(XtDisplay(w), gc, GCForeground | GCLineWidth, &newGCValues);

#ifdef NEVER
  /* X centers thick lines; we want them wholly within specified rectangle. */
  if (lineWidth > 1)
  {
    int ulOffset = lineWidth / 2;
    int lrOffset = (lineWidth - 1) / 2;

    x1 += ulOffset;
    y1 += ulOffset;
    x2 -= lrOffset;
    y2 -= lrOffset;
  }
#endif

  XDrawRectangle(XtDisplay(w), XtWindow(w),
		 gc, x1, y1, x2 - x1, y2 - y1);
}

static void
x_draw_line(CMGraphicsInfo *gInfo, int x1, int y1,
	    int x2, int y2, int lineWidth)
{
  Widget w = gInfo->drawingArea;
  GC gc = gInfo->gc;
  XGCValues newGCValues;

  if (lineWidth == 0)
    lineWidth = 1;

  newGCValues.line_width = lineWidth;
  newGCValues.foreground = gInfo->fg;
  XChangeGC(XtDisplay(w), gc, GCForeground | GCLineWidth, &newGCValues);

  XDrawLine(XtDisplay(w), XtWindow(w),
	    gc, x1, y1, x2, y2);
}

static void
x_set_clip_box(CMGraphicsInfo *gInfo, int x1, int y1, int x2, int y2)
{
  XRectangle rect;
  Widget w = gInfo->drawingArea;

  rect.x = x1;
  rect.y = y1;
  rect.width = x2 - x1;
  rect.height = y2 - y1;

  XSetClipRectangles(XtDisplay(w), gInfo->gc, 0, 0, &rect, 1, YXBanded);
}

static void
x_unset_clip_box(CMGraphicsInfo *gInfo)
{
  Widget w = gInfo->drawingArea;

  XSetClipMask(XtDisplay(w), gInfo->gc, None);
}

static Boolean
x_load_font(CMGraphicsInfo *gInfo, CMFontInfo *fInfo, char *fontPattern)
{
  Widget w = gInfo->drawingArea;
  XrmValue fromValue;
  XrmValue toValue;
  XmFontList fontList = (XmFontList)NULL;

  if (fInfo->fontList != (XmFontList)NULL)
      return False;

  fromValue.addr = (XPointer)fontPattern;
  fromValue.size = strlen(fontPattern);

  toValue.addr = (XPointer)&fontList;
  toValue.size = sizeof(XmFontList);

#ifdef GR_DEBUG
  {
    char *tmpPtr = XtNewString(fontPattern);
    int i;

    for (i = 0; tmpPtr[i] != '\0'; i++)
    {
      if ((i > 0) &&
	  (tmpPtr[i] == '0') &&
	  (tmpPtr[i - 1] == '-') &&
	  (tmpPtr[i + 1] == '-'))
	tmpPtr[i] = '*';
    }
    fromValue.addr = (XPointer)tmpPtr;
  }
#endif /* GR_DEBUG */

  /*
   * :( XtConvertAndStore() doesn't return False even when the specified
   * font cannot be found; Motif plugs in a default font and issues a
   * warning to stderr.
   */
  XtConvertAndStore(w, XmRString, &fromValue, XmRFontList, &toValue);

#ifdef GR_DEBUG
  XtFree((char *)fromValue.addr);
#endif

  fInfo->fontList = fontList;
  return True;
}

static Boolean
cm_load_font(CMGraphicsInfo *gInfo, CMFontEnum fontEnum,
	     char *fontPattern)
{
  int i;
  CMFontInfo *fInfo = cm_find_font(gInfo, fontEnum);

  if (fInfo != (CMFontInfo *)NULL)
    /* It's a no-no to try to load a font more than once! */
    return False;

  fInfo = (CMFontInfo *)XtRealloc((char *)gInfo->fontInfo,
				  sizeof(CMFontInfo) * (gInfo->nFonts + 1));
  if (fInfo == (CMFontInfo *)NULL)
    return False;
  gInfo->fontInfo = fInfo;

  fInfo = &(gInfo->fontInfo[gInfo->nFonts]);
  fInfo->fontEnum = fontEnum;
  fInfo->fontList = (XmFontList)NULL;

  if (!x_load_font(gInfo, fInfo, fontPattern))
  {
    cm_load_font_error(gInfo, fontPattern);
    return False;
  }

  gInfo->nFonts++;
  return True;
}

static void
cm_load_font_error(CMGraphicsInfo *gInfo, char *fontPattern)
{
  Calendar *c = gInfo->c;
  Props_pu *pu = (Props_pu *)c->properties_pu;
  char *errFmt;
  char *errText;
  char *title;
  char *label;

  errFmt = catgets(c->DT_catd, 1, 1119,
		   "Warning - Unable to load font %s.");
  errText = (char *)XtMalloc((strlen(errFmt) + strlen(fontPattern) + 1) *
			     sizeof(char));
  sprintf(errText, errFmt, fontPattern);
  label = XtNewString(catgets(c->DT_catd, 1, 95, "Continue"));
  title = XtNewString(catgets(c->DT_catd, 1, 1118,
			      "Calendar : Warning - Print"));

  dialog_popup(c->frame,
	       DIALOG_TITLE, title,
	       DIALOG_TEXT, errText,
	       BUTTON_IDENT, 1, label,
	       DIALOG_IMAGE, pu->xm_warning_pixmap,
	       NULL);

  XtFree(label);
  XtFree(title);
  XtFree(errText);
}

static void
cm_release_fonts(CMGraphicsInfo *gInfo)
{
  XtFree((char *)gInfo->fontInfo);
  gInfo->fontInfo = (CMFontInfo *)NULL;
  gInfo->nFonts = 0;
}

static Boolean
cm_set_font(CMGraphicsInfo *gInfo, CMFontEnum fontEnum)
{
  CMFontInfo *fInfo;

  if ((gInfo->curFont == (CMFontInfo *)NULL) ||
      (gInfo->curFont->fontEnum != fontEnum))
  {
    if ((fInfo = cm_find_font(gInfo, fontEnum)) == (CMFontInfo *)NULL)
      return False;

    gInfo->curFont = fInfo;
  }

  return True;
}

static CMFontInfo *
cm_find_font(CMGraphicsInfo *gInfo, CMFontEnum fontEnum)
{
  int i;

  for (i = 0; i < gInfo->nFonts; i++)
  {
    if (gInfo->fontInfo[i].fontEnum == fontEnum)
      return &(gInfo->fontInfo[i]);
  }

  return (CMFontInfo *)NULL;
}

static void
cm_font_extents(CMGraphicsInfo *gInfo, CMFontInfo *fInfo, int *wd, int *ht)
{
  int maxHt = 0;
  int maxWd = 0;

  if ((gInfo != (CMGraphicsInfo *)NULL) &&
      (fInfo != (CMFontInfo *)NULL))
  {
    XmFontContext fontContext;

    if (XmFontListInitFontContext(&fontContext, fInfo->fontList))
    {
      XmFontListEntry fontListEntry;
      XFontSet fontSet;
      XFontStruct *fontStruct;
      XtPointer fontEntryFont;
      XmFontType fontType;
      XFontSetExtents *fontSetExtents;
      int thisWd, thisHt;

      while ((fontListEntry = XmFontListNextEntry(fontContext))
	     != (XmFontListEntry)NULL)
      {
	fontEntryFont = XmFontListEntryGetFont(fontListEntry, &fontType);
	if (fontType == XmFONT_IS_FONT)
	{
	  fontStruct = (XFontStruct *)fontEntryFont;
	  thisWd = fontStruct->max_bounds.rbearing -
	    fontStruct->min_bounds.lbearing;
	  thisHt = fontStruct->ascent + fontStruct->descent;
	}
	else if (fontType == XmFONT_IS_FONTSET)
	{
	  fontSet = (XFontSet)fontEntryFont;
	  fontSetExtents = XExtentsOfFontSet(fontSet);
	  thisWd = fontSetExtents->max_logical_extent.width;
	  thisHt = fontSetExtents->max_logical_extent.height;
	}
	else continue;

	if (thisWd > maxWd)
	  maxWd = thisWd;
	if (thisHt > maxHt)
	  maxHt = thisHt;
      }

      XmFontListFreeFontContext(fontContext);
    }
  }

  *wd = (maxWd > 0) ? maxWd : 8;
  *ht = (maxHt > 0) ? maxHt : 13;
}

static int
cm_font_height(CMGraphicsInfo *gInfo, CMFontInfo *fInfo)
{
  int wd, ht;

  cm_font_extents(gInfo, fInfo, &wd, &ht);
  return ht;
}

static int
cm_font_width(CMGraphicsInfo *gInfo, CMFontInfo *fInfo)
{
  int wd, ht;

  cm_font_extents(gInfo, fInfo, &wd, &ht);
  return wd;
}

static int
cm_string_width(CMGraphicsInfo *gInfo, CMFontInfo *fInfo, char *str)
{
  if ((gInfo != (CMGraphicsInfo *)NULL) &&
      (fInfo != (CMFontInfo *)NULL) &&
      (str != (char *)NULL))
  {
    XmString xmStr;
    Dimension textWd;
    Dimension textHt;
    XmFontList fontList = fInfo->fontList;

    xmStr = XmStringCreateLocalized(str);
    XmStringExtent(fontList, xmStr, &textWd, &textHt);
    XmStringFree(xmStr);

    return textWd;
  }

  return 0;
}

static void
cm_reverse_video(CMGraphicsInfo *gInfo, Boolean enabled)
{
  if (gInfo->reverseVideo != enabled)
  {
    Pixel tmp;

    tmp = gInfo->fg;
    gInfo->fg = gInfo->bg;
    gInfo->bg = tmp;

    gInfo->reverseVideo = enabled;
  }
}

static CMFontEnum
cm_get_title_enum(CMGraphicsInfo *gInfo)
{
  switch (gInfo->reportType)
  {
  case PR_DAY_VIEW:
    return DV_TITLE_FONT;

  case PR_WEEK_VIEW:
    return WV_TITLE_FONT;

  default:
  case PR_MONTH_VIEW:
    return MV_TITLE_FONT;

  case PR_YEAR_VIEW:
    return YV_TITLE_FONT;

  case PR_APPT_LIST:
    return AV_TITLE_FONT;

  case PR_TODO_LIST:
    return TV_TITLE_FONT;
  }
}

static CMFontEnum
cm_get_header_enum(CMGraphicsInfo *gInfo)
{
  switch (gInfo->reportType)
  {
  case PR_DAY_VIEW:
    return DV_HEADER_FONT;

  case PR_WEEK_VIEW:
    return WV_HEADER_FONT;

  default:
  case PR_MONTH_VIEW:
    return MV_HEADER_FONT;

  case PR_YEAR_VIEW:
    return YV_HEADER_FONT;

  case PR_APPT_LIST:
    return AV_HEADER_FONT;

  case PR_TODO_LIST:
    return TV_HEADER_FONT;
  }
}

static CMFontEnum
cm_get_tiny_title_enum(CMGraphicsInfo *gInfo)
{
  switch (gInfo->reportType)
  {
  default:
  case PR_MONTH_VIEW:
    return MV_TINY_TITLE_FONT;

  case PR_YEAR_VIEW:
    return YV_TINY_TITLE_FONT;

  case PR_APPT_LIST:
    return AV_TINY_TITLE_FONT;

  case PR_TODO_LIST:
    return TV_TINY_TITLE_FONT;
  }
}

static CMFontEnum
cm_get_tiny_day_enum(CMGraphicsInfo *gInfo)
{
  switch (gInfo->reportType)
  {
  default:
  case PR_MONTH_VIEW:
    return MV_TINY_DAY_FONT;

  case PR_YEAR_VIEW:
    return YV_TINY_DAY_FONT;

  case PR_APPT_LIST:
    return AV_TINY_DAY_FONT;

  case PR_TODO_LIST:
    return TV_TINY_DAY_FONT;
  }
}

static CMFontEnum
cm_get_tiny_date_enum(CMGraphicsInfo *gInfo)
{
  switch (gInfo->reportType)
  {
  default:
  case PR_MONTH_VIEW:
    return MV_TINY_DATE_FONT;

  case PR_YEAR_VIEW:
    return YV_TINY_DATE_FONT;

  case PR_APPT_LIST:
    return AV_TINY_DATE_FONT;

  case PR_TODO_LIST:
    return TV_TINY_DATE_FONT;
  }
}

static void
cm_adjust_margins(CMGraphicsInfo *gInfo)
{
  CMFontInfo *fInfo;
  CMFontEnum titleFont, headerFont;

  titleFont = cm_get_title_enum(gInfo);
  headerFont = cm_get_header_enum(gInfo);

  /*
   * Assume MonthFont and StampFont have been
   * set as required for header and footer.
   */
  if ((fInfo = cm_find_font(gInfo, titleFont)) != (CMFontInfo *)NULL)
    gInfo->topMarginY += cm_font_height(gInfo, fInfo);

  if ((fInfo = cm_find_font(gInfo, headerFont)) != (CMFontInfo *)NULL)
    gInfo->bottomMarginY -= VMARGIN(gInfo) + cm_font_height(gInfo, fInfo);
}

static int
cm_cvt_margin(CMGraphicsInfo *gInfo, Props_op prop, int defaultVal)
{
  Widget w = gInfo->drawingArea;
  Props *p = (Props *) gInfo->c->properties;
  XtEnum badCvt;
  int returnVal;

  /* PostScript dimensions are the same vertically as horizontally. */
  returnVal = XmConvertStringToUnits(XtScreen(w),
				     get_char_prop(p, prop),
				     XmHORIZONTAL,
				     XmPIXELS,
				     &badCvt);

  return badCvt ? defaultVal : returnVal;
}

static void
cm_print_time(CMGraphicsInfo *gInfo, char *str, Glance view)
{
  cm_print_text(gInfo, str, (view == dayGlance) ? 0 : 2, view);
}

static void
cm_print_appt_text(CMGraphicsInfo *gInfo, char *str,
		   int indentAmt, Glance view)
{
  char *to = NULL;
  unsigned long to_len = 0;
  unsigned long _len;

  _len = strlen(str);
  _converter_( str, _len, &to, &to_len );
  if ( ( to != NULL ) && ( to_len != 0 ) )
    str = to;

  cm_print_text(gInfo, str, indentAmt, view);
}

static void
cm_print_text(CMGraphicsInfo *gInfo, char *str,
	      int indentAmt, Glance view)
{
  int textX, textY;
  int clipX1, clipX2;

  switch (view)
  {
  case dayGlance:
    cm_set_font(gInfo, DV_APPT_FONT);

    /* Make sure the text fits in the box. */
    if (gInfo->u.dayInfo.line1 + cm_font_height(gInfo, gInfo->curFont) +
	VMARGIN(gInfo) <= gInfo->u.dayInfo.curY1 - (gInfo->normalWd / 2))
    {
      textX = gInfo->u.dayInfo.curX1 + gInfo->u.dayInfo.tab1 + HMARGIN(gInfo) +
	(indentAmt * cm_font_width(gInfo, gInfo->curFont));
      textY = gInfo->u.dayInfo.line1;
      clipX1 = textX;
      clipX2 = gInfo->u.dayInfo.curX1 + gInfo->u.dayInfo.boxWd -
	((gInfo->normalWd / 2) + 1) - HMARGIN(gInfo);

      gInfo->u.dayInfo.line1 += cm_font_height(gInfo, gInfo->curFont);
      break;
    }
    return;

  case weekGlance:
    cm_set_font(gInfo, WV_APPT_FONT);

    /* Make sure the text fits in the box. */
    if (gInfo->u.weekInfo.lineY + cm_font_height(gInfo, gInfo->curFont) +
	VMARGIN(gInfo) <= gInfo->u.weekInfo.lineMaxY - (gInfo->thickWd / 2))
    {
      textX = gInfo->u.weekInfo.lineX +
	(gInfo->normalWd / 2) + HMARGIN(gInfo) +
	(indentAmt * cm_font_width(gInfo, gInfo->curFont));
      textY = gInfo->u.weekInfo.lineY;
      clipX1 = textX;
      clipX2 = gInfo->u.weekInfo.lineX +
	(gInfo->u.weekInfo.boxWd / WEEK_DAYS) -
	  ((gInfo->normalWd / 2) + 1) - HMARGIN(gInfo);

      gInfo->u.weekInfo.lineY += cm_font_height(gInfo, gInfo->curFont);
      break;
    }
    return;

  default:
    return;
  }

  if (clipX2 > clipX1)
  {
    x_set_clip_box(gInfo, clipX1, gInfo->topMarginY,
		   clipX2, gInfo->bottomMarginY - 1);
    x_print_string(gInfo, str, textX, textY, LEFT_AT_X | TOP_AT_Y);
    x_unset_clip_box(gInfo);
  }
}

static void
cm_week_sched_update(CMGraphicsInfo *gInfo, Dtcm_appointment *appt, Props *p)
{
  /*
   * In order to draw appts in the sched box,  all appts for a day are
   * mapped into a array, and then drawn later.  The array has
   * 96 elements, and represents the 96 15-minute segements
   * available between the day boundaries
   */
  int begin_t = get_int_prop(p, CP_DAYBEGIN);
  int end_t = get_int_prop(p, CP_DAYEND);
  Tick	tick;
  Tick	end_tick = 0;
  int i, start, end;
  int end_slider;

  _csa_iso8601_to_tick(appt->time->value->item.date_time_value, &tick);
  if (appt->end_time)
    _csa_iso8601_to_tick(appt->end_time->value->item.date_time_value,
			 &end_tick);

  /*
   * Determine the last element in the sched_buckets array which
   * corresponds to this time range.
   */

  end_slider = (end_t - begin_t) * 4;

  /* 
   * Figure where the begin and end times should be in the array.
   */
  start = ((hour(tick) - begin_t) * 4) + (minute(tick) / 15);
  if (end_tick)
    end = start + ((end_tick - tick) * 4 / 3600);
  else
    end = start;

  /*
   * Make sure that the appointment starts/ends within the visible
   * time range.
   */
  if (start < 0) start = 0;
  if (end < 0) end = 0;
  if (start > end_slider) start = end_slider;
  if (end > end_slider) end = end_slider;

  /* 
   * Only map if some portion of time is between day boundaries.
   */
  if ((start < 0 && end < 0) || 
      (start >= end_slider && end >= end_slider))
    return;

  /* 
   * Mark the blocks of time affected in the array.
   */
  for (i = start; i < end; i++)
    gInfo->u.weekInfo.schedBucket[i]++;
}

static int
cm_week_sched_box_indent(CMGraphicsInfo *gInfo)
{
  CMFontInfo *fInfo = cm_find_font(gInfo, WV_DAY_FONT);
  static char *baselineStr = "24";

  return cm_string_width(gInfo, fInfo, baselineStr) + HMARGIN(gInfo) +
    gInfo->thickWd;
}

static void
cm_set_todo_text_clipping(CMGraphicsInfo *gInfo)
{
  x_set_clip_box(gInfo,
		 gInfo->u.toDoInfo.innerX1,
		 gInfo->u.toDoInfo.innerY1,
		 gInfo->u.toDoInfo.boxX2 -
		 ((gInfo->thickWd / 2) + 1) - HMARGIN(gInfo),
		 gInfo->u.toDoInfo.boxY2 -
		 ((gInfo->thickWd / 2) + 1));
}

static void
cm_unset_todo_text_clipping(CMGraphicsInfo *gInfo)
{
  x_unset_clip_box(gInfo);
}

static void
cm_print_todo_text(CMGraphicsInfo *gInfo, char *str1, CSA_sint32 as,
		   CSA_sint32 appt_type, int items)
{
  char buf[20];
  char *str;
  int textX, textY;
  int fontHt;
  int i, digitWd, maxDigitWd;
  int indent;
  Boolean isTodo = (appt_type == CSA_TYPE_TODO);

  cm_set_font(gInfo, isTodo ? TV_ITEM_FONT : AV_ITEM_FONT);
  maxDigitWd = 0;
  for (i = 0; i <= 9; i++)
  {
    sprintf(buf, "%d", i);
    if ((digitWd = cm_string_width(gInfo, gInfo->curFont, buf)) > maxDigitWd)
      maxDigitWd = digitWd;
  }

  sprintf(buf, "%d.", items);

  indent = strlen(buf);
  if (indent < 4)
    indent = 4;
  textX = gInfo->u.toDoInfo.lineX + (indent * maxDigitWd);
  textY = gInfo->u.toDoInfo.lineY;
  x_print_string(gInfo, buf, textX, textY,
		 RIGHT_AT_X | TOP_AT_Y);
  textX += maxDigitWd + HMARGIN(gInfo);

  cm_set_font(gInfo, isTodo ? TV_APPT_FONT : AV_APPT_FONT);
  fontHt = cm_font_height(gInfo, gInfo->curFont);

  if (isTodo)
  {
    /* draw check box */
    x_draw_box(gInfo,
	       textX + (gInfo->normalWd / 2) + 1,
	       textY + (gInfo->normalWd / 2) + 1,
	       textX + fontHt - ((gInfo->normalWd + 1) / 2),
	       textY + fontHt - ((gInfo->normalWd + 1) / 2),
	       gInfo->normalWd);

    /* add check to box */
    if (as == CSA_STATUS_COMPLETED)
    {
      x_draw_line(gInfo,
		  textX + (fontHt / 4),
		  textY + (fontHt / 2),
		  textX + (fontHt / 2),
		  textY + fontHt - ((gInfo->normalWd + 3) / 2),
		  gInfo->normalWd);
      x_draw_line(gInfo,
		  textX + (fontHt / 2),
		  textY + fontHt - ((gInfo->normalWd + 3) / 2),
		  textX + ((5 * fontHt) / 4),
		  textY + (gInfo->normalWd / 2) + 1,
		  gInfo->normalWd);
    }
    textX += ((5 * fontHt) / 4) + maxDigitWd + HMARGIN(gInfo);
  }

  {
    char *to = NULL;
    unsigned long to_len = 0;
    unsigned long _len;

    _len = strlen(str1);
    _converter_( str1, _len, &to, &to_len );
    if ( ( to != NULL ) && ( to_len != 0 ) )
      str1 = to;
  }

  str = euc_to_octal(str1);

  x_print_string(gInfo, str, textX, textY, LEFT_AT_X | TOP_AT_Y);
  gInfo->u.toDoInfo.lineY += fontHt;
}

static int
cm_tiny_month_height(CMGraphicsInfo *gInfo, int spacing)
{
  int monthBoxHt;
  CMFontInfo *fInfo;

  /*
   * Month boxes need to be tall enough for:
   *  1 MonthFont, 1 DayFont, 6 DateFonts
   */
  fInfo = cm_find_font(gInfo, cm_get_tiny_title_enum(gInfo));
  monthBoxHt = cm_font_height(gInfo, fInfo) + (2 * spacing);

  fInfo = cm_find_font(gInfo, cm_get_tiny_day_enum(gInfo));
  monthBoxHt += spacing + cm_font_height(gInfo, fInfo);

  fInfo = cm_find_font(gInfo, cm_get_tiny_date_enum(gInfo));
  monthBoxHt += 6 * (spacing + cm_font_height(gInfo, fInfo));

  return monthBoxHt;
}

static void
cm_print_todo_months(CMGraphicsInfo *gInfo, time_t end_time)
{
  int x1, y1, x2, y2, lastX, mon, yr;
  int monthBoxWd;
  int tlOff = (gInfo->thickWd + 1) / 2;
  int brOff = (gInfo->thickWd / 2) + 1;

  mon = month(end_time);
  yr = year(end_time);

  x1 = gInfo->u.toDoInfo.boxX1;
  lastX = gInfo->u.toDoInfo.boxX2;
  monthBoxWd = lastX - x1;
  y1 = gInfo->u.toDoInfo.boxY1;
  x2 = x1 + (monthBoxWd / 3);
  y2 = gInfo->u.toDoInfo.boxY2;

  x_draw_line(gInfo, x2, y1, x2, y2, gInfo->thickWd);
  if ((mon - 1) == 0)
    x_print_month((void *)gInfo, 12, yr - 1,
		  x1 + tlOff, y1 + tlOff,
		  x2 - brOff, y2 - brOff, VMARGIN(gInfo));
  else
    x_print_month((void *)gInfo, mon - 1, yr,
		  x1 + tlOff, y1 + tlOff,
		  x2 - brOff, y2 - brOff, VMARGIN(gInfo));

  x1 = x2;
  x2 = gInfo->u.toDoInfo.boxX1 + ((2 * monthBoxWd) / 3);
  x_draw_line(gInfo, x2, y1, x2, y2, gInfo->thickWd);
  x_print_month((void *)gInfo, mon, yr,
		x1 + tlOff, y1 + tlOff,
		x2 - brOff, y2 - brOff, VMARGIN(gInfo));

  x1 = x2;
  x2 = lastX;
  if ((mon + 1) > 12)
    x_print_month((void *)gInfo, 1, yr + 1,
		  x1 + tlOff, y1 + tlOff,
		  x2 - brOff, y2 - brOff, VMARGIN(gInfo));
  else
    x_print_month((void *)gInfo, mon + 1, yr,
		  x1 + tlOff, y1 + tlOff,
		  x2 - brOff, y2 - brOff, VMARGIN(gInfo));
}

static int
cm_count_appts(CMGraphicsInfo *gInfo, CSA_entry_handle *list, int a_total,
	       CSA_sint32 appt_type, int vf)
{
  int      		i, meoval;
  boolean_t		skip_appt;
  CSA_return_code	stat;
  Dtcm_appointment	*appt;
  Calendar		*c = gInfo->c;
  Props			*p = (Props*)c->properties;
  Tick			start_tick;
  int			apptCount = 0;

  meoval = get_int_prop(p, CP_PRINTPRIVACY);

  appt = allocate_appt_struct(appt_read,
			      c->general->version,
			      CSA_ENTRY_ATTR_CLASSIFICATION_I,
			      CSA_ENTRY_ATTR_TYPE_I,
			      CSA_ENTRY_ATTR_STATUS_I,
			      CSA_ENTRY_ATTR_START_DATE_I,
			      CSA_ENTRY_ATTR_SUMMARY_I,
			      CSA_X_DT_ENTRY_ATTR_SHOWTIME_I,
			      CSA_ENTRY_ATTR_END_DATE_I,
			      NULL);
  for (i = 0; i < a_total; i++)
  {
    stat = query_appt_struct(c->cal_handle, list[i], appt);
    if (stat != CSA_SUCCESS)
    {
      free_appt_struct(&appt);
      return;
    }
    skip_appt = B_FALSE;

    if ((privacy_set(appt) == CSA_CLASS_PUBLIC) &&
	!(meoval & PRINT_PUBLIC))
      skip_appt = B_TRUE;
    else if ((privacy_set(appt) == CSA_CLASS_CONFIDENTIAL) &&
	     !(meoval & PRINT_SEMIPRIVATE))
      skip_appt = B_TRUE;
    else if ((privacy_set(appt) == CSA_CLASS_PRIVATE) &&
	     !(meoval & PRINT_PRIVATE))
      skip_appt = B_TRUE;

    if ((appt_type == CSA_TYPE_TODO) &&
	((appt->type->value->item.sint32_value != CSA_TYPE_TODO)
	 ||
	 ((vf == VIEW_PENDING) &&
	  (appt->state->value->item.sint32_value ==
	   CSA_STATUS_COMPLETED)) ||
	 ((vf == VIEW_COMPLETED) &&
	  (appt->state->value->item.sint32_value ==
	   CSA_X_DT_STATUS_ACTIVE))))
      skip_appt = B_TRUE;

    if ((appt_type == CSA_TYPE_EVENT) &&
	(appt->type->value->item.sint32_value != CSA_TYPE_EVENT))
      skip_appt = B_TRUE;

    if (!skip_appt)
      apptCount++;
  }

  return apptCount;
}

static void
cm_get_todo_lines_per_page(CMGraphicsInfo *gInfo, CSA_sint32 apptType,
			   int *linesFullPageP,
			   int *linesPartialPageP)
{
  CMFontInfo *fInfo =
    cm_find_font(gInfo, (apptType == CSA_TYPE_TODO) ?
		 TV_APPT_FONT : AV_APPT_FONT);
  int fontHt = cm_font_height(gInfo, fInfo);

  *linesFullPageP = (gInfo->u.toDoInfo.boxY2 - gInfo->u.toDoInfo.innerY1 -
		     gInfo->thickWd - (2 * VMARGIN(gInfo))) / fontHt;
  *linesPartialPageP = (gInfo->u.toDoInfo.innerY2 - gInfo->u.toDoInfo.innerY1 -
			gInfo->thickWd - (2 * VMARGIN(gInfo))) / fontHt;
}

static void
cm_todo_outline(CMGraphicsInfo *gInfo, Boolean lastPage)
{
	x_fill_dark_box(gInfo, gInfo->u.toDoInfo.outerX1,
			gInfo->u.toDoInfo.outerY1,
			gInfo->u.toDoInfo.outerX2,
			gInfo->u.toDoInfo.outerY2,
			gInfo->thickWd);

	if (lastPage)
	{
	  /* Appts box. */
	  x_fill_light_box(gInfo,
			   gInfo->u.toDoInfo.innerX1,
			   gInfo->u.toDoInfo.innerY1,
			   gInfo->u.toDoInfo.innerX2,
			   gInfo->u.toDoInfo.innerY2,
			   gInfo->thickWd);

	  /* Months box. */
	  x_fill_light_box(gInfo,
			   gInfo->u.toDoInfo.boxX1,
			   gInfo->u.toDoInfo.boxY1,
			   gInfo->u.toDoInfo.boxX2,
			   gInfo->u.toDoInfo.boxY2,
			   gInfo->thickWd);
	}
	else
	{
	  /* Appts box. */
	  x_fill_light_box(gInfo,
			   gInfo->u.toDoInfo.innerX1,
			   gInfo->u.toDoInfo.innerY1,
			   gInfo->u.toDoInfo.boxX2,
			   gInfo->u.toDoInfo.boxY2,
			   gInfo->thickWd);
	}
}

static void
cm_print_month_time(CMGraphicsInfo *gInfo, char *str)
{
  int textX, textY;
  int clipX1, clipX2;
  static char *baselineTime = "24:00";

  cm_set_font(gInfo, MV_APPT_FONT);

  /* Make sure the text fits in the box. */
  if (gInfo->u.monthInfo.lineY + cm_font_height(gInfo, gInfo->curFont) +
      VMARGIN(gInfo) <= gInfo->u.monthInfo.lineMaxY - (gInfo->normalWd / 2))
  {
    /* Time is right-justified. */
    textX = gInfo->u.monthInfo.lineX + (gInfo->normalWd / 2) +
      HMARGIN(gInfo) + cm_string_width(gInfo, gInfo->curFont, baselineTime);
    gInfo->u.monthInfo.lastX = textX;
    textY = gInfo->u.monthInfo.lineY;
    clipX1 = gInfo->u.monthInfo.lineX + (gInfo->normalWd / 2) + HMARGIN(gInfo);
    clipX2 = gInfo->u.monthInfo.lineMaxX -
      (gInfo->normalWd / 2) - HMARGIN(gInfo);

    if (clipX2 > clipX1)
    {
      x_set_clip_box(gInfo, clipX1, gInfo->u.monthInfo.lineY,
		     clipX2, gInfo->u.monthInfo.lineMaxY);
      x_print_string(gInfo, str, textX, textY, RIGHT_AT_X | TOP_AT_Y);
      x_unset_clip_box(gInfo);
    }
  }
}

static void
cm_print_month_text(CMGraphicsInfo *gInfo, char *str, int indentAmt)
{
  int textX, textY;
  int clipX1, clipX2;

  cm_set_font(gInfo, MV_APPT_FONT);

  /* Make sure the text fits in the box. */
  if (gInfo->u.monthInfo.lineY + cm_font_height(gInfo, gInfo->curFont) +
      VMARGIN(gInfo) <= gInfo->u.monthInfo.lineMaxY - (gInfo->normalWd / 2))
  {
    textX = gInfo->u.monthInfo.lastX + (gInfo->normalWd / 2) + HMARGIN(gInfo);
    textY = gInfo->u.monthInfo.lineY;
    clipX1 = textX;
    clipX2 = gInfo->u.monthInfo.lineMaxX -
      (gInfo->normalWd / 2) - HMARGIN(gInfo);

    if (clipX2 > clipX1)
    {
      x_set_clip_box(gInfo, clipX1, gInfo->u.monthInfo.lineY,
		     clipX2, gInfo->u.monthInfo.lineMaxY);
      x_print_string(gInfo, str, textX, textY, LEFT_AT_X | TOP_AT_Y);
      x_unset_clip_box(gInfo);
    }

    gInfo->u.monthInfo.lineY += cm_font_height(gInfo, gInfo->curFont);
  }
}

static void
cm_std_month_weekdays(CMGraphicsInfo *gInfo, int colWd,
		      int x1, int y1, int x2)
{
  char *day_of_week[DAYS_IN_WEEK];
  int extraWd;
  int boxWd;
  int i;
  int indent;

  for (i = 0; i < DAYS_IN_WEEK; i++)
    local_dayname(gInfo->c, &day_of_week[i], i);

  x1 += HMARGIN(gInfo);
  x2 -= HMARGIN(gInfo);
  boxWd = x2 - x1;
  extraWd = boxWd - (DAYS_IN_WEEK * colWd);
  if (extraWd < 0)
  {
    extraWd = 0;
    colWd = boxWd / DAYS_IN_WEEK;
  }

  /* --- print std month weekdays --- */
  cm_set_font(gInfo, cm_get_tiny_day_enum(gInfo));
  for (i = 0; i < DAYS_IN_WEEK; i++)
  {
    indent = i + 1;
    x_print_string(gInfo, day_of_week[i],
		   x1 + (indent * colWd) + ((indent * extraWd) /
					    (DAYS_IN_WEEK + 1)),
		   y1, RIGHT_AT_X | TOP_AT_Y);
  }

  for (i = 0; i < DAYS_IN_WEEK; i++)
    free(day_of_week[i]);
}

static void
cm_std_month_dates(CMGraphicsInfo *gInfo, int first_dom, int monlen,
		   int colWd, int x1, int y1, int x2, int spacing)
{
  int i;
  int dateX, dateY;
  int extraWd;
  int boxWd;
  int indent;
  char dateStr[3];
  int fontHt;

  cm_set_font(gInfo, cm_get_tiny_date_enum(gInfo));
  fontHt = cm_font_height(gInfo, gInfo->curFont);

  x1 += HMARGIN(gInfo);
  x2 -= HMARGIN(gInfo);
  boxWd = x2 - x1;
  extraWd = boxWd - (DAYS_IN_WEEK * colWd);
  if (extraWd < 0)
  {
    extraWd = 0;
    colWd = boxWd / DAYS_IN_WEEK;
  }

  for (i = 0; i < monlen; i++, first_dom++)
  {
    indent = (first_dom % DAYS_IN_WEEK) + 1;
    dateX = x1 + (indent * colWd) + ((indent * extraWd) /
				     (DAYS_IN_WEEK + 1));
    dateY = y1 + ((first_dom / DAYS_IN_WEEK) * (spacing + fontHt));
    sprintf(dateStr, "%d", i + 1);
    x_print_string(gInfo, dateStr,
		   dateX, dateY, RIGHT_AT_X | TOP_AT_Y);
  }
}

static void
cm_print_header_string(CMGraphicsInfo *gInfo, Props_op printOp,
		       char *timeStamp, char *userId,
		       char *pageStr, char *reportTypeStr)
{
  Props *p = (Props *)gInfo->c->properties;
  int xPos, yPos, posFlags;
  int hdrType;
  char *str;

  hdrType = get_int_prop(p, printOp);

  switch (hdrType)
  {
  case POP_HDR_DATE:
    str = timeStamp;
    break;

  case POP_HDR_USER_ID:
    str = userId;
    break;

  case POP_HDR_PAGE_NUMBER:
    str = pageStr;
    break;

  case POP_HDR_REPORT_TYPE:
    str = reportTypeStr;
    break;

  default:
    /* Nothing to print. */
    return;
  }

  switch (printOp)
  {
  case CP_PRINTLHEADER:
    xPos = gInfo->leftMarginX;
    yPos = gInfo->topMarginY;
    posFlags = LEFT_AT_X | TOP_AT_Y;
    break;

  case CP_PRINTRHEADER:
    xPos = gInfo->rightMarginX - 1;
    yPos = gInfo->topMarginY;
    posFlags = RIGHT_AT_X | TOP_AT_Y;
    break;

  case CP_PRINTLFOOTER:
    xPos = gInfo->leftMarginX;
    if (gInfo->reportType == PR_WEEK_VIEW)
      xPos += cm_week_sched_box_indent(gInfo);
    yPos = gInfo->bottomMarginY + VMARGIN(gInfo);
    posFlags = LEFT_AT_X | TOP_AT_Y;
    break;

  case CP_PRINTRFOOTER:
    xPos = gInfo->rightMarginX - 1;
    yPos = gInfo->bottomMarginY + VMARGIN(gInfo);
    posFlags = RIGHT_AT_X | TOP_AT_Y;
    break;

  default:
    return;
  }

  cm_set_font(gInfo, cm_get_header_enum(gInfo));
  x_print_string(gInfo, str, xPos, yPos, posFlags);
}

static void
local_dayname(Calendar *c, char **array_place, int dayNum)
{
  char *source;
  char *str;
  char *to = NULL;
  unsigned long to_len = 0;
  unsigned long _len;
  static char *defaultDays[DAYS_IN_WEEK] =
  {
    "S", "M", "T", "W", "T", "F", "S"
  };
  static int dayCatIndex[DAYS_IN_WEEK] =
  {
    497, 491, 492, 493, 494, 495, 496
  };

  source = catgets(c->DT_catd, 1,
		   dayCatIndex[dayNum], defaultDays[dayNum]);

  _len = strlen( source );
  _converter_( source, _len, &to, &to_len );
  if ( ( to != NULL ) && ( to_len != 0 ) ) {
    str = euc_to_octal(to);
  } else {
    str = euc_to_octal(source);
  }

  *array_place = (char *)malloc(sizeof(char) * (cm_strlen(str) + 1));
  cm_strcpy(*array_place, str);
}

static void
local_dayname3(Calendar *c, char **array_place, int dayNum)
{
  char *source;
  char *str;
  char *to = NULL;
  unsigned long to_len = 0;
  unsigned long _len;
  static char *defaultDays[DAYS_IN_WEEK] =
  {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
  };
  static int dayCatIndex[DAYS_IN_WEEK] =
  {
    480, 481, 482, 483, 484, 485, 486
  };

  source = catgets(c->DT_catd, 1,
		   dayCatIndex[dayNum], defaultDays[dayNum]);

  _len = strlen( source );
  _converter_( source, _len, &to, &to_len );
  if ( ( to != NULL ) && ( to_len != 0 ) ) {
    str = euc_to_octal(to);
  } else {
    str = euc_to_octal(source);
  }

  *array_place = (char *)malloc(sizeof(char) * (cm_strlen(str) + 1));
  cm_strcpy(*array_place, str);
}

static char *
get_report_type_string(CMGraphicsInfo *gInfo)
{
  Calendar *c = gInfo->c;
  int reportType = gInfo->reportType;
  static char *defaultStrs[] =
  {
    "Day view by Calendar Manager",
    "Week view by Calendar Manager",
    "Month view by Calendar Manager",
    "Year view by Calendar Manager",
    "Appt List by Calendar Manager",
    "To Do List by Calendar Manager"
  };
  static int catIndex[] =
  {
    469, 490, 477, 507, 474, 473
  };
  static char *reportStrs[] =
  {
    NULL, NULL, NULL, NULL, NULL, NULL
  };

  if ((reportType < 0) || (reportType >= XtNumber(defaultStrs)))
    return "";

  if (reportStrs[reportType] == (char *)NULL)
  {
    char *str;
    char *to = NULL;
    unsigned long to_len = 0;
    unsigned long _len;

    str = catgets(c->DT_catd, 1, catIndex[reportType],
		  defaultStrs[reportType]);

    _len = strlen( str );
    _converter_( str, _len, &to, &to_len );
    if ( ( to != NULL ) && ( to_len != 0 ) ) {
      str = to;
    }
    str = euc_to_octal(str);

    reportStrs[reportType] = cm_strdup(str);
  }

  return reportStrs[reportType];
}

static void
filePrintDoneCB(Display *dsp, XPContext context, XPGetDocStatus status,
		XPointer uData)
{
  Calendar *c = (Calendar *)uData;

  if (pd_get_bad_alloc_error(c))
  {
      showBadAllocError(c);
      pd_set_bad_alloc_error(c, False);
  }
  else
      filePrintReportStatus(c, (status == XPGetDocFinished));
}

static void
filePrintReportStatus(Calendar *c, Boolean ok)
{
  Props_pu *pu = (Props_pu *)c->properties_pu;
  char *errText;
  char *title;
  char *label;
  Pixmap pixmap;

  label = XtNewString(catgets(c->DT_catd, 1, 95, "Continue"));
  if (!ok)
  {
    errText = XtNewString(catgets(c->DT_catd, 1, 1115,
				  "Error - unable to print to file."));
    title = XtNewString(catgets(c->DT_catd, 1, 1114,
				"Calendar : Error - Print To File"));
    pixmap = pu->xm_error_pixmap;
  }
  else
  {
    errText = XtNewString(catgets(c->DT_catd, 1, 1117,
				  "Print to file has completed."));
    title = XtNewString(catgets(c->DT_catd, 1, 1116,
				"Calendar : Print To File"));
    pixmap = pu->xm_info_pixmap;
  }

  dialog_popup(c->frame,
	       DIALOG_TITLE, title,
	       DIALOG_TEXT, errText,
	       BUTTON_IDENT, 1, label,
	       DIALOG_IMAGE, pixmap,
	       NULL);

  XtFree(label);
  XtFree(title);
  XtFree(errText);
}

static void
showBadAllocError(Calendar *c)
{
  Props_pu *pu = (Props_pu *)c->properties_pu;
  char *errText;
  char *title;
  char *label;
  Pixmap pixmap;

  label = XtNewString(catgets(c->DT_catd, 1, 95, "Continue"));
  errText = XtNewString(catgets(c->DT_catd, 1, 1122,
				"Print job failed.\n\n\
The X Print Server is temporarily out of resources."));
  title = XtNewString(catgets(c->DT_catd, 1, 1121,
				"Calendar : Print Server Error"));
  pixmap = pu->xm_error_pixmap;

  dialog_popup(c->frame,
	       DIALOG_TITLE, title,
	       DIALOG_TEXT, errText,
	       BUTTON_IDENT, 1, label,
	       DIALOG_IMAGE, pixmap,
	       NULL);

  XtFree(label);
  XtFree(title);
  XtFree(errText);
}

void *
x_open_file(Calendar *c)
{
  CMGraphicsInfo	*gInfo;
  Widget		drawingArea;
  Tick		t;
  struct tm	*tm;
  Arg		args[10];
  int		nargs;
  unsigned int	gcMask = GCFont | GCForeground | GCBackground |
    GCTile | GCFillStyle | GCLineWidth;
  Widget printShell;
  WidgetList shellChildren;
#ifdef SHELL_SIZE_BUG
  Dimension shellWd = 0, shellHt = 0;
#endif /* SHELL_SIZE_BUG */
  int nChildren;
  static Boolean getResources = True;
  _Xltimeparams localtime_buf;

  if (getResources)
  {
    XtGetApplicationResources(c->frame, (XtPointer)&dvFonts,
			      dvDefaultResources, XtNumber(dvDefaultResources),
			      NULL, 0);
    XtGetApplicationResources(c->frame, (XtPointer)&wvFonts,
			      wvDefaultResources, XtNumber(wvDefaultResources),
			      NULL, 0);
    XtGetApplicationResources(c->frame, (XtPointer)&mvFonts,
			      mvDefaultResources, XtNumber(mvDefaultResources),
			      NULL, 0);
    XtGetApplicationResources(c->frame, (XtPointer)&yvFonts,
			      yvDefaultResources, XtNumber(yvDefaultResources),
			      NULL, 0);
    XtGetApplicationResources(c->frame, (XtPointer)&avFonts,
			      avDefaultResources, XtNumber(avDefaultResources),
			      NULL, 0);
    XtGetApplicationResources(c->frame, (XtPointer)&tvFonts,
			      tvDefaultResources, XtNumber(tvDefaultResources),
			      NULL, 0);
    getResources = False;
  }

  if ((printShell = pd_get_print_shell(c)) == (Widget)NULL)
    return (void *)NULL;

  if ((gInfo = (CMGraphicsInfo *)XtMalloc(sizeof(CMGraphicsInfo)))
      == (CMGraphicsInfo *)NULL)
    return (void *)NULL;

#ifdef GR_DEBUG
  XtGetApplicationResources(printShell, (XtPointer)&printResources,
			    displayResources, XtNumber(displayResources),
			    NULL, 0);
#endif

#if defined(PRINTING_SUPPORTED)

#ifdef GR_DEBUG
  if (!inDebugMode(c))
  {
#endif
    if (pd_print_to_file(c))
    {
      char *fileName = pd_get_file_name(c);

      /* Protocol says XpStartJob() MUST be called before XmPrintToFile() */
      XpStartJob(XtDisplay(printShell), XPGetData);

      XFlush(XtDisplay(printShell));

      if (!XmPrintToFile(XtDisplay(printShell), fileName,
			 filePrintDoneCB, (XtPointer)c))
      {
	  XpCancelJob(XtDisplay(printShell), False);

	  XtFree(fileName);
	  XtFree((char *)gInfo);

	  filePrintReportStatus(c, False);

	  return (void *)NULL;
      }
      XtFree(fileName);
    }
    else
    {
      XpStartJob(XtDisplay(printShell), XPSpool);
    }
#ifdef GR_DEBUG
  }
#endif
#endif  /* PRINTING_SUPPORTED */


  nargs = 0;
#ifdef SHELL_SIZE_BUG
  XtSetArg(args[nargs], XmNwidth, &shellWd); nargs++;
  XtSetArg(args[nargs], XmNheight, &shellHt); nargs++;
#endif /* SHELL_SIZE_BUG */
  XtSetArg(args[nargs], XmNchildren, &shellChildren); nargs++;
  XtSetArg(args[nargs], XmNnumChildren, &nChildren); nargs++;
  XtGetValues(printShell, args, nargs);

#ifdef SHELL_SIZE_BUG
  if ((shellWd < 10) || (shellHt < 10))
  {
    shellWd = XmConvertUnits(printShell, XmHORIZONTAL, Xm1000TH_INCHES,
			     PAGE_WIDTH, XmPIXELS);
    shellHt = XmConvertUnits(printShell, XmVERTICAL, Xm1000TH_INCHES,
			     PAGE_HEIGHT, XmPIXELS);

    nargs = 0;
    XtSetArg(args[nargs], XmNwidth, shellWd); nargs++;
    XtSetArg(args[nargs], XmNheight, shellHt); nargs++;
    XtSetValues(printShell, args, nargs);
  }
#endif /* SHELL_SIZE_BUG */

  if (nChildren == 0)
  {
    nargs = 0;
    XtSetArg(args[nargs], XmNunitType, XmPIXELS); nargs++;
#ifdef GR_DEBUG
    if (!inDebugMode(c))
    {
#endif
      XtSetArg(args[nargs], XmNbackground,
	       WhitePixelOfScreen(XtScreen(printShell))); nargs++;
#ifdef GR_DEBUG
    }
#endif
    drawingArea = XmCreateDrawingArea(printShell, "PrintArea",
				      args, nargs);
  }
  else
    drawingArea = shellChildren[0];

#ifdef GR_DEBUG
  if (inDebugMode(c))
    XtAddCallback(drawingArea, XmNinputCallback,
		  tmpFn, (XtPointer)NULL);
#endif

  gInfo->drawingArea = drawingArea;
  gInfo->gc = XtAllocateGC(drawingArea, 0, 0, NULL, gcMask, 0);
  gInfo->c = c;
  gInfo->reportType = PR_MONTH_VIEW; /* Set properly in x_init_*() */

  gInfo->fontInfo = gInfo->curFont = (CMFontInfo *)NULL;
  gInfo->nFonts = 0;

  gInfo->reverseVideo = False;
  gInfo->fg = BlackPixelOfScreen(XtScreen(drawingArea));
  gInfo->bg = WhitePixelOfScreen(XtScreen(drawingArea));
#ifdef GR_DEBUG
  nargs = 0;
  XtSetArg(args[nargs], XmNforeground, &(gInfo->fg)); nargs++;
  XtSetArg(args[nargs], XmNbackground, &(gInfo->bg)); nargs++;
  XtGetValues(drawingArea, args, nargs);
#endif
  gInfo->lightPixmap = None;
  gInfo->darkPixmap = None;

  /* set up time stamp */
  /* STRING_EXTRACTION SUNW_DESKSET_CM_MSG
   * The following string is the date/time format used in printing out
   * calendar views.  This is the locale's date and time.  If this string
   * is to be translated, please refer to the man pages for strftime()
   * for various format strings.
   */
  t = now();
  tm = _XLocaltime(&t, localtime_buf);
  strftime(gInfo->timestamp, BUFFERSIZE, "%x %I:%M %p", tm);

  {
    char *to = NULL;
    unsigned long to_len = 0;
    unsigned long _len;

    _len = strlen( gInfo->timestamp );
    _converter_( gInfo->timestamp, _len, &to, &to_len );
    if ( ( to != NULL ) && ( to_len != 0 ) )
      strncpy( gInfo->timestamp, to,
	      ( BUFFERSIZE > to_len ) ? to_len : BUFFERSIZE - 1 );
  }

  return (void *)gInfo;
}

void
x_print_file(void *gInfoP, Calendar *c)
{
  CMGraphicsInfo *gInfo = (CMGraphicsInfo *)gInfoP;
  Display *dsp = XtDisplay(gInfo->drawingArea);

#ifdef GR_DEBUG
  if (!inDebugMode(c))
#endif
  {
    XpEndJob(dsp);

    /* Make sure we know about a BadAlloc if it happens. */
    XSync(XtDisplay(gInfo->drawingArea), FALSE);
  }

  if (pd_get_bad_alloc_error(c) &&
      (!pd_print_to_file(c)))
  {
    showBadAllocError(c);
    pd_set_bad_alloc_error(c, False);
  }

  if (gInfo->lightPixmap != None)
    XFreePixmap(dsp, gInfo->lightPixmap);
  if (gInfo->darkPixmap != None)
    XFreePixmap(dsp, gInfo->darkPixmap);

  XtReleaseGC(gInfo->drawingArea, gInfo->gc);

  cm_release_fonts(gInfo);

  XtFree((char *)gInfo);
}

void
x_init_printer(void *gInfoP, short orientation)
{
  CMGraphicsInfo *gInfo = (CMGraphicsInfo *)gInfoP;
  Widget w = gInfo->drawingArea;
  Arg args[10];
  int nargs;
  unsigned int pageWd, pageHt;
  int baseWd, baseHt;
  int rMargin, bMargin;
  int inchWd;
  Dimension daWd, daHt;

#ifdef GR_DEBUG
  if (inDebugMode(gInfo->c))
  {
    XtAddCallback(w, XmNexposeCallback,
		  tmpFn, (XtPointer)NULL);
  }
  else
#endif
    XpStartPage(XtDisplay(w), XtWindow(XtParent(w)));

  /* XpStartPage() sets the shell width/height correctly. */
  nargs = 0;
  XtSetArg(args[nargs], XmNwidth, &daWd); nargs++;
  XtSetArg(args[nargs], XmNheight, &daHt); nargs++;
  XtGetValues(XtParent(w), args, nargs);

  if (XtIsRealized(w))
  {
    XClearArea(XtDisplay(w), XtWindow(w), 0, 0, 0, 0,
#ifdef GR_DEBUG
	       inDebugMode(gInfo->c)
#else
	       True
#endif
	       );
  }
  else
  {
    nargs = 0;
    XtSetArg(args[nargs], XmNwidth, daWd); nargs++;
    XtSetArg(args[nargs], XmNheight, daHt); nargs++;
    XtSetValues(w, args, nargs);
    XtManageChild(w);
  }

#ifdef GR_DEBUG
  if (inDebugMode(gInfo->c))
  {
    tmpSpin(w);

    XtRemoveCallback(w, XmNexposeCallback,
		     tmpFn, (XtPointer)NULL);

    /* Now that window is displayed, get its actual size, */
    /* since the Window Manager may have modified it. */
    nargs = 0;
    XtSetArg(args[nargs], XmNwidth, &daWd); nargs++;
    XtSetArg(args[nargs], XmNheight, &daHt); nargs++;
    XtGetValues(XtParent(w), args, nargs);
  }
#endif

  /* Get margins from properties. */
  inchWd = XmConvertUnits(w, XmHORIZONTAL, Xm1000TH_INCHES,
			  XINCH, XmPIXELS);
  gInfo->leftMarginX = cm_cvt_margin(gInfo, CP_PRINTLMARGIN, inchWd);
  gInfo->topMarginY = cm_cvt_margin(gInfo, CP_PRINTTMARGIN, inchWd);
  rMargin = cm_cvt_margin(gInfo, CP_PRINTRMARGIN, inchWd);
  bMargin = cm_cvt_margin(gInfo, CP_PRINTBMARGIN, inchWd);

  gInfo->rightMarginX = daWd - rMargin;
  gInfo->bottomMarginY = daHt - bMargin;

  pageWd = XmConvertUnits(w, XmHORIZONTAL, XmPIXELS,
			  daWd, Xm1000TH_INCHES);
  pageHt = XmConvertUnits(w, XmVERTICAL, XmPIXELS,
			  daHt, Xm1000TH_INCHES);
  if (orientation == PORTRAIT)
  {
    baseWd = PAGE_WIDTH;
    baseHt = PAGE_HEIGHT;
  }
  else
  {
    baseWd = PAGE_HEIGHT;
    baseHt = PAGE_WIDTH;
  }

  gInfo->hscale = (double)pageWd / (double)baseWd;
  gInfo->vscale = (double)pageHt / (double)baseHt;

  /* Scaled line widths: .25pt, 1pt, 2pt. */
  gInfo->thinWd = XmConvertUnits(w, XmHORIZONTAL, Xm100TH_POINTS,
				 25, XmPIXELS);
  if (gInfo->thinWd < 1)
    gInfo->thinWd = 1;
  gInfo->normalWd = XmConvertUnits(w, XmHORIZONTAL, Xm100TH_POINTS,
				   100, XmPIXELS);
  if (gInfo->normalWd < 1)
    gInfo->normalWd = 1;
  gInfo->thickWd = XmConvertUnits(w, XmHORIZONTAL, Xm100TH_POINTS,
				  200, XmPIXELS);
  if (gInfo->thickWd < 1)
    gInfo->thickWd = 1;
}

void
x_init_day(void *gInfoP, int timeslots)
{
  CMGraphicsInfo *gInfo = (CMGraphicsInfo *)gInfoP;
  CMFontInfo *fInfo;

  gInfo->reportType = PR_DAY_VIEW;

  /* Load day fonts */
  if (gInfo->nFonts == 0)
  {
    int i;

    for (i = 0; i < DV_NUM_FONTS; i++)
      cm_load_font(gInfo, i, dvFonts[i]);

    /* Set curFont to first loaded font. */
    gInfo->curFont = gInfo->fontInfo;
  }

  cm_adjust_margins(gInfo);

  fInfo = cm_find_font(gInfo, DV_TIME_FONT);
  gInfo->u.dayInfo.tab1 = (cm_font_height(gInfo, fInfo) * 2) + gInfo->normalWd;

  gInfo->u.dayInfo.curX1 = gInfo->leftMarginX + (gInfo->normalWd / 2);
  gInfo->u.dayInfo.curX2 = (gInfo->leftMarginX + gInfo->rightMarginX) / 2;
  fInfo = cm_find_font(gInfo, DV_HEADER_FONT);
  gInfo->u.dayInfo.curY1 =
    gInfo->topMarginY + (gInfo->normalWd / 2) + cm_font_height(gInfo, fInfo) +
      VMARGIN(gInfo) + gInfo->u.dayInfo.tab1;
  gInfo->u.dayInfo.curY2 = gInfo->u.dayInfo.curY1;

  gInfo->u.dayInfo.boxHt =
    (gInfo->bottomMarginY - ((gInfo->normalWd + 1) / 2) -
     gInfo->u.dayInfo.curY1) / timeslots;
  gInfo->u.dayInfo.boxWd =
    gInfo->u.dayInfo.curX2 - gInfo->u.dayInfo.curX1;

  gInfo->u.dayInfo.line1 = 0;
}

int
x_get_day_lines_per_page(void *gInfoP)
{
  CMGraphicsInfo *gInfo = (CMGraphicsInfo *)gInfoP;
  CMFontInfo *fInfo = cm_find_font(gInfo, DV_APPT_FONT);

  return (gInfo->u.dayInfo.boxHt - gInfo->normalWd - (2 * VMARGIN(gInfo))) /
    cm_font_height(gInfo, fInfo);
}

int
x_get_week_lines_per_page(void *gInfoP)
{
  CMGraphicsInfo *gInfo = (CMGraphicsInfo *)gInfoP;
  CMFontInfo *fInfo = cm_find_font(gInfo, DV_APPT_FONT);

  return (gInfo->u.weekInfo.boxHt - gInfo->u.weekInfo.tab1 -
	  ((gInfo->normalWd + 1) / 2) -
	  (gInfo->thickWd / 2) - (2 * VMARGIN(gInfo))) /
	    cm_font_height(gInfo, fInfo);
}

void
x_print_header(void *gInfoP, char *buf, int pageNum, int numPages)
{
  CMGraphicsInfo *gInfo = (CMGraphicsInfo *)gInfoP;
  Calendar *c = gInfo->c;
  char *str;
  char str2[BUFFERSIZE];
  char pageStr[BUFFERSIZE];
  char *timeStamp;
  char *userId;
  char *reportTypeStr;

  /* --- print header info at top --- */
  if (buf != (char *)NULL)
  {
    cm_set_font(gInfo, cm_get_title_enum(gInfo));
    {
      char *to = NULL;
      unsigned long to_len = 0;
      unsigned long _len;

      _len = strlen( buf );
      _converter_( buf, _len, &to, &to_len );
      if ( ( to != NULL ) && ( to_len != 0 ) ) {
	str = euc_to_octal(to);
      } else {
	str = euc_to_octal(buf);
      }
    }
    x_print_string(gInfo, str,
		   (gInfo->rightMarginX + gInfo->leftMarginX) / 2,
		   gInfo->topMarginY,
		   CENTER_AT_X | BOTTOM_AT_Y);
  }

  /* Get header/footer strings ready. */
  timeStamp = gInfo->timestamp;
  userId = c->view->current_calendar;
  reportTypeStr = get_report_type_string(gInfo);

  if (numPages == 0)
    numPages++;

  /* print creation notice at bottom */ 
  cm_strcpy(str2, catgets(c->DT_catd, 1, 468, "Page"));
  {
    char *to = NULL;
    unsigned long to_len = 0;
    unsigned long _len;

    _len = strlen( str2 );
    _converter_( str2, _len, &to, &to_len );
    if ( ( to != NULL ) && ( to_len != 0 ) ) {
      strncpy( str2, to,
	      ( BUFFERSIZE > to_len ) ? to_len : BUFFERSIZE );
    }
  }
  cm_strcpy(str2, euc_to_octal(str2));

  str = catgets(c->DT_catd, 1, 476, "of");
  {
    char *to = NULL;
    unsigned long to_len = 0;
    unsigned long _len;

    _len = strlen( str );
    _converter_( str, _len, &to, &to_len );
    if ( ( to != NULL ) && ( to_len != 0 ) ) {
      str = to;
    }
  }

  sprintf(pageStr, "%s %d %s %d", str2, pageNum, str, numPages);

  /* Header/footer strings are all set - print 'em! */
  cm_print_header_string(gInfo, CP_PRINTLHEADER,
			 timeStamp, userId, pageStr, reportTypeStr);
  cm_print_header_string(gInfo, CP_PRINTRHEADER,
			 timeStamp, userId, pageStr, reportTypeStr);
  cm_print_header_string(gInfo, CP_PRINTLFOOTER,
			 timeStamp, userId, pageStr, reportTypeStr);
  cm_print_header_string(gInfo, CP_PRINTRFOOTER,
			 timeStamp, userId, pageStr, reportTypeStr);
}

void
x_day_header (void *gInfoP)
{
  CMGraphicsInfo *gInfo = (CMGraphicsInfo *)gInfoP;
  CMFontInfo *fInfo;
  char *str;
  Calendar *c = gInfo->c;
  int dayBoxVOffset;
  int midX;

  fInfo = cm_find_font(gInfo, DV_HEADER_FONT);
  dayBoxVOffset = (gInfo->normalWd / 2) +
    cm_font_height(gInfo, fInfo) + VMARGIN(gInfo);

  /* --- print Morning/Afternoon boxes --- */
  cm_set_font(gInfo, DV_TIME_FONT);

  x_fill_light_box(gInfo, gInfo->u.dayInfo.curX1,
		   gInfo->topMarginY + dayBoxVOffset,
		   gInfo->rightMarginX - ((gInfo->normalWd + 1) / 2),
		   gInfo->topMarginY + dayBoxVOffset + gInfo->u.dayInfo.tab1,
		   gInfo->normalWd);

  x_fill_dark_box(gInfo, gInfo->u.dayInfo.curX1,
		  gInfo->topMarginY + dayBoxVOffset,
		  gInfo->u.dayInfo.curX1 + gInfo->u.dayInfo.tab1,
		  gInfo->topMarginY + dayBoxVOffset + gInfo->u.dayInfo.tab1,
		  gInfo->normalWd);

  midX = gInfo->u.dayInfo.curX2;
  x_fill_dark_box(gInfo, midX,
		  gInfo->topMarginY + dayBoxVOffset,
		  midX + gInfo->u.dayInfo.tab1,
		  gInfo->topMarginY + dayBoxVOffset + gInfo->u.dayInfo.tab1,
		  gInfo->normalWd);

  str = catgets(c->DT_catd, 1, 470, "Morning");

  {
    char *to = NULL;
    unsigned long to_len = 0;
    unsigned long _len;

    _len = strlen( str );
    _converter_( str, _len, &to, &to_len );
    if ( ( to != NULL ) && ( to_len != 0 ) ) {
      str = to;
    }
  }

  x_print_string(gInfo, str,
		 (gInfo->u.dayInfo.curX1 + gInfo->u.dayInfo.tab1 + midX) / 2,
		 gInfo->topMarginY + dayBoxVOffset +
		 (gInfo->u.dayInfo.tab1 / 2),
		 CENTER_AT_X | CENTER_AT_Y);

  str = catgets(c->DT_catd, 1, 471, "Afternoon");

  {
    char *to = NULL;
    unsigned long to_len = 0;
    unsigned long _len;

    _len = strlen( str );
    _converter_( str, _len, &to, &to_len );
    if ( ( to != NULL ) && ( to_len != 0 ) ) {
      str = to;
    }
  }

  str = euc_to_octal(str);
  x_print_string(gInfo, str,
		 (midX + gInfo->u.dayInfo.tab1 + gInfo->rightMarginX
		  - ((gInfo->normalWd + 1) / 2)) / 2,
		 gInfo->topMarginY + dayBoxVOffset +
		 (gInfo->u.dayInfo.tab1 / 2),
		 CENTER_AT_X | CENTER_AT_Y);
}

void
x_day_timeslots(void *gInfoP, int i, Boolean more)
{
  CMGraphicsInfo *gInfo = (CMGraphicsInfo *)gInfoP;

  char hourbuf[6];
  char modbuf[6];
  Calendar *c = gInfo->c;

  sprintf (hourbuf, "%d", (morning(i) || (i == 12)) ? i : (i - 12) );
  sprintf (modbuf, "%s", morning(i) ? catgets(c->DT_catd, 1, 4, "am") :
	   catgets(c->DT_catd, 1, 3, "pm"));

  /* --- print hourly boxes for appt entries --- */
  if (i == 12)
  {
    gInfo->u.dayInfo.curX1 = gInfo->u.dayInfo.curX2;
    gInfo->u.dayInfo.curY1 = gInfo->u.dayInfo.curY2;

    /* boxWd may be off by a pixel - set here to ensure correctness. */
    gInfo->u.dayInfo.boxWd =
      gInfo->rightMarginX - ((gInfo->normalWd + 1) / 2) -
	gInfo->u.dayInfo.curX1;
  }

  x_draw_box(gInfo, gInfo->u.dayInfo.curX1, gInfo->u.dayInfo.curY1,
	     gInfo->u.dayInfo.curX1 + gInfo->u.dayInfo.boxWd,
	     gInfo->u.dayInfo.curY1 + gInfo->u.dayInfo.boxHt,
	     gInfo->normalWd);
  x_fill_light_box(gInfo, gInfo->u.dayInfo.curX1,
		   gInfo->u.dayInfo.curY1,
		   gInfo->u.dayInfo.curX1 + gInfo->u.dayInfo.tab1,
		   gInfo->u.dayInfo.curY1 + gInfo->u.dayInfo.boxHt,
		   gInfo->normalWd);

  cm_set_font(gInfo, DV_TIME_FONT);

  x_print_string(gInfo, hourbuf,
		 gInfo->u.dayInfo.curX1 + (gInfo->u.dayInfo.tab1 / 2),
		 gInfo->u.dayInfo.curY1 + (gInfo->u.dayInfo.boxHt / 2) -
		 (VMARGIN(gInfo) / 2),
		 CENTER_AT_X | BOTTOM_AT_Y);

  x_print_string(gInfo, modbuf,
		 gInfo->u.dayInfo.curX1 + (gInfo->u.dayInfo.tab1 / 2),
		 gInfo->u.dayInfo.curY1 + (gInfo->u.dayInfo.boxHt / 2) +
		 (VMARGIN(gInfo) / 2),
		 CENTER_AT_X | TOP_AT_Y);

  if (more)
    x_print_string(gInfo, "*",
		   gInfo->u.dayInfo.curX1 + (gInfo->u.dayInfo.tab1 / 2),
		   gInfo->u.dayInfo.curY1 + (gInfo->u.dayInfo.boxHt / 2) +
		   cm_font_height(gInfo, gInfo->curFont) + VMARGIN(gInfo),
		   CENTER_AT_X | TOP_AT_Y);

  gInfo->u.dayInfo.line1 = gInfo->u.dayInfo.curY1 +
    ((gInfo->normalWd + 1) / 2) + VMARGIN(gInfo);
  gInfo->u.dayInfo.curY1 += gInfo->u.dayInfo.boxHt;
}

Boolean
x_print_multi_appts(void *gInfoP,
	CSA_entry_handle *list,
	int a_total,
        int num_page,
        Glance view)
{
  /*
   * This routine is used to print appointments for day and week
   * views.  The parm "view" is used to differentiate
   * who is printing, and personalize the print for that view.
   */

  CMGraphicsInfo *gInfo = (CMGraphicsInfo *)gInfoP;
  int		indented, indentAmt, multlines=TRUE;
  Lines		*lines, *lp;    
  char		buf1[128], buf2[257];
  Calendar	*c = gInfo->c;  
  Props		*pr = (Props*)c->properties;
  int		meoval = get_int_prop(pr, CP_PRINTPRIVACY);
  int		dt = get_int_prop(pr, CP_DEFAULTDISP);
  int		i, j, start, maxlines, pos = 1, line_counter = 0;
  Boolean	new_appt = FALSE;
  CSA_return_code	stat;
  Dtcm_appointment	*appt;

  if (view == weekGlance)
    maxlines = x_get_week_lines_per_page(gInfoP);
  else if (view == dayGlance)
    maxlines = x_get_day_lines_per_page(gInfoP);
  else maxlines = 1;

  start = ((num_page - 1) * maxlines) + 1;

  appt = allocate_appt_struct(appt_read,
			      c->general->version,
			      CSA_ENTRY_ATTR_CLASSIFICATION_I,
			      CSA_ENTRY_ATTR_START_DATE_I,
			      CSA_ENTRY_ATTR_SUMMARY_I,
			      CSA_X_DT_ENTRY_ATTR_SHOWTIME_I,
			      CSA_ENTRY_ATTR_END_DATE_I,
			      NULL);
  for (j = 0; j < a_total; j++) {
    stat = query_appt_struct(c->cal_handle, list[j], appt);
    if (stat != CSA_SUCCESS) {
      free_appt_struct(&appt);
      return False;
    }

    if ((privacy_set(appt) == CSA_CLASS_PUBLIC) && !(meoval & PRINT_PUBLIC))
      continue;
    if ((privacy_set(appt) == CSA_CLASS_CONFIDENTIAL) &&
	!(meoval & PRINT_SEMIPRIVATE))
      continue;
    if ((privacy_set(appt) == CSA_CLASS_PRIVATE) &&
	!(meoval & PRINT_PRIVATE))
      continue;

    lp = lines = text_to_lines(appt->what->value->item.string_value,
			       multlines ? 10 : 1);
    new_appt = TRUE;

    /* skip past lines already printed */
    if (pos < start) {
      if (showtime_set(appt)) {
	if (new_appt)
	  for (i = 1; i <= num_page; i++)
	    if (pos == (maxlines * i))
	      start--;
	pos++;
      }
      while ((lines != NULL) && (pos < start)) {
	pos++;
	new_appt = FALSE;
	lines = lines->next;
      }
      if (pos < start) {
	continue;
      }
    }

    /* skip last line if it's a time */
    if (showtime_set(appt)) {
      if (line_counter == (maxlines - 1)) {
	free_appt_struct(&appt);
	return(FALSE);
      }
    }

    /* HH:MM xm - HH:MM xm format */
    format_line2(appt, buf1, buf2, dt);

    indented = (*buf1 != NULL);
    indentAmt = (view == dayGlance) ? 0 : 1;
    if (indented)
      indentAmt += 2;

    /* only print if appt text found */
    if (lines != NULL && lines->s != NULL) {
      if ((indented) && (new_appt)) {
	line_counter++;
	if ((line_counter > maxlines) && (lines != NULL)) {
	  free_appt_struct(&appt);
	  return(FALSE);
	}

	cm_print_time (gInfo, buf1, view);
      }

      cm_print_appt_text (gInfo, lines->s, indentAmt, view);
      line_counter++;
      if ((line_counter > maxlines) && (lines != NULL)) {
	free_appt_struct(&appt);
	return(FALSE);
      }

      lines = lines->next;
      while (lines != NULL) {
	line_counter++;
	if ((line_counter > maxlines) && (lines != NULL)) {
	  free_appt_struct(&appt);
	  return(FALSE);
	}

	cm_print_appt_text (gInfo, lines->s, indentAmt, view);
	lines = lines->next;
      }
    }
    destroy_lines(lp);
    if (view == weekGlance)
      cm_week_sched_update(gInfo, appt, pr);
    new_appt = FALSE;
  }
  free_appt_struct(&appt);
  return(TRUE);
}

void
x_finish_printer(void *gInfoP)
{
  CMGraphicsInfo *gInfo = (CMGraphicsInfo *)gInfoP;
  Widget w = gInfo->drawingArea;

#ifdef GR_DEBUG
  if (inDebugMode(gInfo->c))
    tmpSpin(w);
  else
#endif
    XpEndPage(XtDisplay(w));
}

void
x_print_list_range(Calendar *c, CSA_sint32 appt_type, int item_data,
		   Tick start_tick, Tick end_tick)
{
	char		buf[MAXNAMELEN], buf2[MAXNAMELEN];
	void		*xp;
	Props		*p = (Props *)c->properties;
	Glance		glance;
	todo_view_op	todo_view = VIEW_ALL;
	OrderingType	ot = get_int_prop(p, CP_DATEORDERING);
	time_t 		start, end;
        CSA_return_code stat;
        CSA_entry_handle *list;
        CSA_attribute *range_attrs;
	CSA_enum *ops;
        int i;
	CSA_uint32 a_total;

	if ((xp = x_open_file(c)) == (void *)NULL)
	  return;

	if (appt_type ==  CSA_TYPE_TODO) {
		todo_view = (todo_view_op)item_data;
		glance = c->view->glance;
	} else
		glance = (Glance)item_data;

	x_init_printer(xp, PORTRAIT);
	x_init_list(xp);

	memset(buf, '\0', MAXNAMELEN);
	memset(buf2, '\0', MAXNAMELEN);

	start = lowerbound(start_tick);
	end = nextday(end_tick) - 1;

	format_date(start + 1, ot, buf2, 1, 0, 0);

	if (appt_type == CSA_TYPE_TODO)
		sprintf(buf, catgets(c->DT_catd, 1, 899,
				     "To Do List Beginning: %s"), buf2);
	else
		sprintf(buf, catgets(c->DT_catd, 1, 900,
				     "Appt List Beginning: %s"), buf2);

	setup_range(&range_attrs, &ops, &i, start, end, appt_type,
		    NULL, B_FALSE, c->general->version);
	stat = csa_list_entries(c->cal_handle, i, range_attrs,
				ops, &a_total, &list, NULL);
	if (stat != CSA_SUCCESS) {
		return;
	}

	x_print_todo(xp, list, a_total, appt_type,
		     (start_tick != end_tick), todo_view, end, buf);

	csa_free(list);

	x_finish_printer(xp);
	x_print_file(xp, c);
}

void
x_init_list(void *gInfoP)
{
  CMGraphicsInfo *gInfo = (CMGraphicsInfo *)gInfoP;
  CMFontInfo *fInfo;
  int hIndent;
  int vIndent;

  /* We can ONLY get here printing via dialog - printData is valid. */
  gInfo->reportType = pd_get_report_type(gInfo->c);

  /* Load list fonts */
  if (gInfo->nFonts == 0)
  {
    int i;

    if (gInfo->reportType == PR_APPT_LIST)
    {
      for (i = 0; i < AV_NUM_FONTS; i++)
	cm_load_font(gInfo, i, avFonts[i]);
    }
    else
    {
      for (i = 0; i < TV_NUM_FONTS; i++)
	cm_load_font(gInfo, i, tvFonts[i]);
    }

    /* Set curFont to first loaded font. */
    gInfo->curFont = gInfo->fontInfo;
  }

  cm_adjust_margins(gInfo);

  cm_set_font(gInfo, cm_get_header_enum(gInfo));

  hIndent = TODO_HINDENT * gInfo->hscale * gInfo->normalWd;
  vIndent = TODO_VINDENT * gInfo->vscale * gInfo->normalWd;
  gInfo->u.toDoInfo.outerX1 = gInfo->leftMarginX + (gInfo->thickWd / 2);
  gInfo->u.toDoInfo.outerY1 = gInfo->topMarginY + (gInfo->thickWd / 2) +
    cm_font_height(gInfo, gInfo->curFont) + VMARGIN(gInfo);
  gInfo->u.toDoInfo.innerX1 = gInfo->u.toDoInfo.outerX1 + hIndent;
  gInfo->u.toDoInfo.innerY1 = gInfo->u.toDoInfo.outerY1 + vIndent;
  gInfo->u.toDoInfo.outerX2 =
    gInfo->rightMarginX - ((gInfo->thickWd + 1) / 2);
  gInfo->u.toDoInfo.outerY2 =
    gInfo->bottomMarginY - ((gInfo->thickWd + 1) / 2);
  gInfo->u.toDoInfo.boxX2 = gInfo->u.toDoInfo.outerX2 - hIndent;
  gInfo->u.toDoInfo.boxY2 = gInfo->u.toDoInfo.outerY2 - vIndent;
  gInfo->u.toDoInfo.boxX1 = gInfo->u.toDoInfo.innerX1;
  gInfo->u.toDoInfo.boxY1 = gInfo->u.toDoInfo.boxY2 -
    cm_tiny_month_height(gInfo, VMARGIN(gInfo)) - gInfo->thickWd;
  gInfo->u.toDoInfo.innerX2 = gInfo->u.toDoInfo.boxX2;
  gInfo->u.toDoInfo.innerY2 = gInfo->u.toDoInfo.boxY1 - vIndent;

  gInfo->u.toDoInfo.lineX =
    gInfo->u.toDoInfo.innerX1 + ((gInfo->thickWd + 1) / 2) + HMARGIN(gInfo);
  gInfo->u.toDoInfo.lineY =
    gInfo->u.toDoInfo.innerY1 + ((gInfo->thickWd + 1) / 2) + VMARGIN(gInfo);
}

void
x_print_todo(void *gInfoP, CSA_entry_handle *list, int a_total, 
	     CSA_sint32 appt_type, Boolean showDate, int vf,
	     time_t end_time, char *buf)
{
	CMGraphicsInfo  *gInfo = (CMGraphicsInfo *)gInfoP;
	int      	i, items_per_page = 0, meoval;
	char            buf3[BUFFERSIZE], buf2[BUFFERSIZE], buf1[BUFFERSIZE];
	int		total_items = 0;
	boolean_t	skip_appt;
	CSA_return_code	stat;
	Dtcm_appointment *appt;
	Calendar	*c = gInfo->c;
	Props		*p = (Props*)c->properties;
	DisplayType	dt = get_int_prop(p, CP_DEFAULTDISP);
	OrderingType	ot = get_int_prop(p, CP_DATEORDERING);
	SeparatorType	st = get_int_prop(p, CP_DATESEPARATOR);
	Tick		start_tick;
	int		linesFullPage, linesPartialPage;
	int		numAppts, numPages, curPage;

	curPage = 1;
	cm_get_todo_lines_per_page(gInfo, appt_type, &linesFullPage,
				   &linesPartialPage);
	numAppts = cm_count_appts(gInfo, list, a_total, appt_type, vf);
	for (numPages = 1, numAppts -= linesPartialPage;
	     numAppts > 0;
	     numPages++)
	  numAppts -= linesFullPage;
	x_print_header(gInfoP, buf, curPage, numPages);
	cm_todo_outline(gInfo, curPage == numPages);
	cm_set_todo_text_clipping(gInfo);

	meoval = get_int_prop(p, CP_PRINTPRIVACY);
	buf1[0] = buf2[0] = buf3[0] = '\0';

        appt = allocate_appt_struct(appt_read,
				    c->general->version,
				    CSA_ENTRY_ATTR_CLASSIFICATION_I,
				    CSA_ENTRY_ATTR_TYPE_I,
				    CSA_ENTRY_ATTR_STATUS_I,
				    CSA_ENTRY_ATTR_START_DATE_I,
				    CSA_ENTRY_ATTR_SUMMARY_I,
				    CSA_X_DT_ENTRY_ATTR_SHOWTIME_I,
				    CSA_ENTRY_ATTR_END_DATE_I,
				    NULL);
        for (i = 0; i < a_total; i++)
	{
	  stat = query_appt_struct(c->cal_handle, list[i], appt);
	  if (stat != CSA_SUCCESS)
	  {
	    free_appt_struct(&appt);
	    return;
	  }
	  skip_appt = B_FALSE;

	  if ((privacy_set(appt) == CSA_CLASS_PUBLIC) &&
	      !(meoval & PRINT_PUBLIC))
	    skip_appt = B_TRUE;
	  else if ((privacy_set(appt) == CSA_CLASS_CONFIDENTIAL) &&
		   !(meoval & PRINT_SEMIPRIVATE))
	    skip_appt = B_TRUE;
	  else if ((privacy_set(appt) == CSA_CLASS_PRIVATE) &&
		   !(meoval & PRINT_PRIVATE))
	    skip_appt = B_TRUE;

	  if ((appt_type == CSA_TYPE_TODO) &&
	      ((appt->type->value->item.sint32_value != CSA_TYPE_TODO)
	       ||
	       ((vf == VIEW_PENDING) &&
		(appt->state->value->item.sint32_value ==
		 CSA_STATUS_COMPLETED)) ||
	       ((vf == VIEW_COMPLETED) &&
		(appt->state->value->item.sint32_value ==
		 CSA_X_DT_STATUS_ACTIVE))))
	    skip_appt = B_TRUE;

	  if ((appt_type == CSA_TYPE_EVENT) &&
	      (appt->type->value->item.sint32_value != CSA_TYPE_EVENT))
	    skip_appt = B_TRUE;

	  if (skip_appt)
	    continue;

	  format_maxchars(appt, buf1, BUFFERSIZE - 5, dt);
	  if (showDate)
	  {
	    _csa_iso8601_to_tick(appt->time->value->item.date_time_value,
				 &start_tick);
	    format_date3(start_tick, ot, st, buf2);
	    sprintf(buf3, "%s  %s", buf2, buf1);
	  } else
	    cm_strcpy(buf3, buf1);

	  total_items++;
	  cm_print_todo_text (gInfo, buf3,
			      appt->state->value->item.sint32_value,
			      appt_type, total_items);
	  if ((++items_per_page % linesFullPage) == 0)
	  {
	    cm_unset_todo_text_clipping(gInfo);
	    x_finish_printer(gInfoP);
	    x_init_printer(gInfoP, PORTRAIT);
	    x_init_list(gInfoP);
	    x_print_header(gInfoP, buf, ++curPage, numPages);
	    cm_todo_outline(gInfo, curPage == numPages);
	    cm_set_todo_text_clipping(gInfo);
	    items_per_page = 0;
	  }
	} /* end for stmt */

	free_appt_struct(&appt);
	cm_unset_todo_text_clipping(gInfo);

	if (items_per_page > linesPartialPage) {
	  x_finish_printer(gInfoP);
	  x_init_printer(gInfoP, PORTRAIT);
	  x_init_list(gInfoP);
	  x_print_header(gInfoP, buf, ++curPage, numPages);
	  cm_todo_outline(gInfo, curPage == numPages);
	}

	cm_print_todo_months(gInfo, end_time);
}

void
x_print_month(void *gInfoP, int mon, int yr, int x1,
	      int y1, int x2, int y2, int spacing)
{
  CMGraphicsInfo *gInfo = (CMGraphicsInfo *)gInfoP;
  CMFontInfo *fInfo;
  Tick j = monthdayyear(mon, 1, yr);        /* starting date Jan 1, y */
  int i, colWd, dateWd, monLen;
  char str[BUFFERSIZE];

  if (mon > 12)
    mon = (mon % 13) + 1;
  else if (mon < 1)
    mon = mon + 12;

  x_set_clip_box(gInfo, x1, y1, x2, y2);

  cm_set_font(gInfo, cm_get_tiny_title_enum(gInfo));
  y1 += spacing;

  cm_strcpy(str, months[mon]);

  {
    char *to = NULL;
    unsigned long to_len = 0;
    unsigned long _len;

    _len = strlen(str);
    _converter_( str, _len, &to, &to_len );
    if ( ( to != NULL ) && ( to_len != 0 ) ) {
      cm_strcpy(str, to);
    }
  }

  cm_strcpy(str, euc_to_octal(str));
  x_fill_light_box(gInfo, x1 + HMARGIN(gInfo), y1,
		   x2 - HMARGIN(gInfo),
		   y1 + cm_font_height(gInfo, gInfo->curFont),
		   0);
  x_print_string(gInfo, str,
		 (x1 + x2) / 2, y1,
		 CENTER_AT_X | TOP_AT_Y);
  y1 += spacing + cm_font_height(gInfo, gInfo->curFont);

  fInfo = cm_find_font(gInfo, cm_get_tiny_date_enum(gInfo));
  monLen = monthlength(j);
  colWd = 0;
  for (i = 0; i < monLen; i++)
  {
    sprintf(str, "%d", i + 1);
    if ((dateWd = cm_string_width(gInfo, fInfo, str)) > colWd)
      colWd = dateWd;
  }

  cm_std_month_weekdays(gInfo, colWd, x1, y1, x2);
  y1 += spacing + cm_font_height(gInfo, gInfo->curFont);

  cm_std_month_dates(gInfo, fdom(j), monLen, colWd,
		     x1, y1, x2, spacing);

  x_unset_clip_box(gInfo);
}

void
x_init_std_year(void *gInfoP)
{
  CMGraphicsInfo *gInfo = (CMGraphicsInfo *)gInfoP;
  CMFontInfo *fInfo;

  gInfo->reportType = PR_YEAR_VIEW;

  /* Load year fonts */
  if (gInfo->nFonts == 0)
  {
    int i;

    for (i = 0; i < YV_NUM_FONTS; i++)
      cm_load_font(gInfo, i, yvFonts[i]);

    /* Set curFont to first loaded font. */
    gInfo->curFont = gInfo->fontInfo;
  }

  cm_adjust_margins(gInfo);
}

void
x_std_year_name(void *gInfoP, int yr)
{
  CMGraphicsInfo *gInfo = (CMGraphicsInfo *)gInfoP;
  char yearBuf[BUFFERSIZE];

  /* --- print year centered at top --- */
  sprintf(yearBuf, "%d", yr);
  x_print_header(gInfoP, yearBuf, 1, 1);
}

void
x_print_year(void *gInfoP, int yr)
{
  CMGraphicsInfo *gInfo = (CMGraphicsInfo *)gInfoP;
  CMFontInfo *fInfo;
  int i;
  int leftX, topY;
  int x1, y1;
  int x2, y2;
  int row, col;
  int pageWd, pageHt;
  int monthHt;
  int extraSpace;

  fInfo = cm_find_font(gInfo, YV_HEADER_FONT);
  monthHt = cm_tiny_month_height(gInfo, VMARGIN(gInfo));

  leftX = gInfo->leftMarginX;
  topY = gInfo->topMarginY + cm_font_height(gInfo, fInfo) + VMARGIN(gInfo);

  pageWd = gInfo->rightMarginX - leftX;
  pageHt = gInfo->bottomMarginY - topY;
  extraSpace = pageHt - (4 * monthHt);

  for (i = 0; i < 12; i++)
  {
    row = i / 3;
    col = i % 3;

    x1 = leftX + ((col * pageWd) / 3);
    x2 = leftX + (((col + 1) * pageWd) / 3);

    if (extraSpace > 0)
    {
      y1 = topY + (((row + 1) * extraSpace) / 5) + (row * monthHt);
      y2 = topY + (((row + 2) * extraSpace) / 5) + ((row + 1) * monthHt);
    }
    else
    {
      y1 = topY + ((row * pageHt) / 4);
      y2 = topY + (((row + 1) * pageHt) / 4);
    }

    x_print_month(gInfoP, i + 1, yr, x1, y1, x2, y2, VMARGIN(gInfo));
  }
}

void
x_init_week(void *gInfoP)
{
  CMGraphicsInfo *gInfo = (CMGraphicsInfo *)gInfoP;
  CMFontInfo *fInfo;

  gInfo->reportType = PR_WEEK_VIEW;

  /* Load week fonts */
  if (gInfo->nFonts == 0)
  {
    int i;

    for (i = 0; i < WV_NUM_FONTS; i++)
      cm_load_font(gInfo, i, wvFonts[i]);

    /* Set curFont to first loaded font. */
    gInfo->curFont = gInfo->fontInfo;
  }

  cm_adjust_margins(gInfo);

  fInfo = cm_find_font(gInfo, WV_HEADER_FONT);
  gInfo->u.weekInfo.curX1 = gInfo->leftMarginX + (gInfo->thickWd / 2);
  gInfo->u.weekInfo.curY1 = gInfo->topMarginY + cm_font_height(gInfo, fInfo) +
    VMARGIN(gInfo) + (gInfo->thickWd / 2);
  gInfo->u.weekInfo.boxWd =
    gInfo->rightMarginX - gInfo->leftMarginX - gInfo->thickWd;
  gInfo->u.weekInfo.boxHt =
    (gInfo->bottomMarginY - ((gInfo->thickWd + 1) / 2) -
     gInfo->u.weekInfo.curY1) / 2;
  gInfo->u.weekInfo.curY2 =
    gInfo->u.weekInfo.curY1 + gInfo->u.weekInfo.boxHt;

  fInfo = cm_find_font(gInfo, WV_DAY_FONT);
  gInfo->u.weekInfo.tab1 =
    ((3 * cm_font_height(gInfo, fInfo)) +
     gInfo->thickWd + gInfo->normalWd) / 2;
}

void
x_week_appt_boxes(void *gInfoP)
{
  CMGraphicsInfo *gInfo = (CMGraphicsInfo *)gInfoP;
  int x1, y1, x2, y2;
  int innerWd, innerOffset;
  int i;

  /* Draw weekdays box. */
  x1 = gInfo->u.weekInfo.curX1;
  y1 = gInfo->u.weekInfo.curY1;
  x2 = x1 + gInfo->u.weekInfo.boxWd;
  y2 = y1 + gInfo->u.weekInfo.tab1;

  x_fill_light_box(gInfo, x1, y1, x2, y2, gInfo->normalWd);

  y2 = gInfo->u.weekInfo.curY2;
  x_draw_box(gInfo, x1, y1, x2, y2, gInfo->thickWd);

  /* Draw weekdays vertical lines. */
  innerWd = gInfo->u.weekInfo.boxWd - gInfo->thickWd + gInfo->normalWd;
  innerOffset = ((gInfo->thickWd - 1) / 2) - ((gInfo->normalWd - 1) / 2);
  for (i = 1; i < WEEK_DAYS; i++)
  {
    x1 = gInfo->u.weekInfo.curX1 + innerOffset + ((i * innerWd) / WEEK_DAYS);
    x_draw_line(gInfo, x1, y1, x1, y2, gInfo->normalWd);
  }

  /* Draw weekends box. */
  x1 = gInfo->u.weekInfo.curX1 +
    (((WEEK_DAYS - WEEKEND_DAYS) * gInfo->u.weekInfo.boxWd) / WEEK_DAYS);
  y1 = y2;
  y2 = y1 + gInfo->u.weekInfo.tab1;
  x_fill_light_box(gInfo, x1, y1, x2, y2, gInfo->normalWd);

  y2 = gInfo->bottomMarginY - ((gInfo->thickWd + 1) / 2);
  x_draw_box(gInfo, x1, y1, x2, y2, gInfo->thickWd);

  /* Draw weekends vertical lines. */
  for (i = WEEK_DAYS - WEEKEND_DAYS + 1; i < WEEK_DAYS; i++)
  {
    x1 = gInfo->u.weekInfo.curX1 + innerOffset + ((i * innerWd) / WEEK_DAYS);
    x_draw_line(gInfo, x1, y1, x1, y2, gInfo->normalWd);
  }
}

void
x_week_sched_boxes(void *gInfoP)
{
  CMGraphicsInfo *gInfo = (CMGraphicsInfo *)gInfoP;
  char *day_of_week[DAYS_IN_WEEK];
  char hourBuf[BUFFERSIZE];
  Props *p = (Props *)gInfo->c->properties;
  int begin = get_int_prop(p, CP_DAYBEGIN);
  int end = get_int_prop(p, CP_DAYEND);
  DisplayType dt = get_int_prop(p, CP_DEFAULTDISP);
  int thisHour;
  int num_hours;
  int boxIndent;
  int boxX1, boxY1, boxX2, boxY2, boxWd, boxHt;
  int x1, y1, x2, y2;
  int i;
  int ulOff = gInfo->thickWd / 2;
  int brOff = (gInfo->thickWd - 1) / 2;

  /* --- print week sched boxes --- */

  /* figure out number of partitions in sched box */
  num_hours = end - begin;

  /* determine offset of sched box from left margin */
  cm_set_font(gInfo, WV_DAY_FONT);
  boxIndent = cm_week_sched_box_indent(gInfo);
  boxX1 = gInfo->u.weekInfo.curX1 + boxIndent;
  boxX2 = gInfo->u.weekInfo.curX1 +
    ((3 * gInfo->u.weekInfo.boxWd) / WEEK_DAYS) - boxIndent;
  boxY1 = gInfo->u.weekInfo.curY2 +
    (2 * cm_font_height(gInfo, gInfo->curFont)) +
      gInfo->thickWd;
  boxY2 = gInfo->bottomMarginY - 1 - brOff;

  x_draw_box(gInfo, boxX1, boxY1, boxX2, boxY2, gInfo->thickWd);

  boxX1 += brOff - ((gInfo->normalWd - 1) / 2);
  boxX2 -= ulOff - (gInfo->normalWd / 2);
  boxY1 += brOff - ((gInfo->thinWd - 1) / 2);
  boxY2 -= ulOff - (gInfo->thinWd / 2);
  boxWd = boxX2 - boxX1;
  boxHt = boxY2 - boxY1;

  for (i = 0; i < DAYS_IN_WEEK; i++)
    local_dayname(gInfo->c, &day_of_week[i], (i + 1) % DAYS_IN_WEEK);

  /* print abbreviated weekdays on top and draw vertical lines. */
  for (i = 0; i < DAYS_IN_WEEK; i++)
  {
    x1 = boxX1 + ((i * boxWd) / DAYS_IN_WEEK);
    x2 = boxX1 + (((i + 1) * boxWd) / DAYS_IN_WEEK);

    x_print_string(gInfo, day_of_week[i],
		   (x1 + x2) / 2, boxY1 -
		   gInfo->thickWd - (2 * VMARGIN(gInfo)),
		   CENTER_AT_X | BOTTOM_AT_Y);
    free(day_of_week[i]);
    if (i > 0)
      x_draw_line(gInfo, x1, boxY1, x1, boxY2, gInfo->normalWd);
  }

  /* Print hours and horizontal lines. */
  for (thisHour = begin, i = 0; i <= num_hours; i++, thisHour++)
  {
    if ( dt == HOUR12 )
    {
      if (thisHour > 12)
	thisHour %= 12;
    }
    else if ( dt == HOUR24 )
    {
      if (thisHour > 24)
	thisHour %= 24;
    }
    sprintf(hourBuf, "%d", thisHour);
    y1 = boxY1 + ((i * boxHt) / num_hours);

    x_print_string(gInfo, hourBuf,
		   boxX1 - gInfo->thickWd - HMARGIN(gInfo), y1,
		   RIGHT_AT_X | CENTER_AT_Y);

    if ((i > 0) && (i < num_hours))
      x_draw_line(gInfo, boxX1, y1, boxX2, y1, gInfo->thinWd);
  }

  gInfo->u.weekInfo.schedBoxX1 = boxX1;
  gInfo->u.weekInfo.schedBoxY1 = boxY1;
  gInfo->u.weekInfo.schedBoxX2 = boxX2;
  gInfo->u.weekInfo.schedBoxY2 = boxY2;
}

void
x_week_daynames(void *gInfoP, char *dayName, int dayIndex, Boolean more)
{
  CMGraphicsInfo *gInfo = (CMGraphicsInfo *)gInfoP;
  Calendar *c = gInfo->c;
  int x1, y1, x2, y2;
  int innerOffset, innerWd;
  char dayBuf[BUFFERSIZE];

  y1 = (dayIndex < WEEK_DAYS) ? gInfo->u.weekInfo.curY1 :
    gInfo->u.weekInfo.curY2;
  y2 = y1 + gInfo->u.weekInfo.tab1;

  cm_set_font(gInfo, WV_DAY_FONT);

  innerWd = gInfo->u.weekInfo.boxWd - gInfo->thickWd + gInfo->normalWd;
  innerOffset = ((gInfo->thickWd - 1) / 2) - ((gInfo->normalWd - 1) / 2);

  if (dayIndex >= WEEK_DAYS)
    dayIndex -= WEEKEND_DAYS;
  x1 = gInfo->u.weekInfo.curX1 + innerOffset +
    ((dayIndex * innerWd) / WEEK_DAYS);
  x2 = gInfo->u.weekInfo.curX1 + innerOffset +
    (((dayIndex + 1) * innerWd) / WEEK_DAYS);

  {
    char *to = NULL;
    unsigned long to_len = 0;
    unsigned long _len;

    _len = strlen(dayName);
    _converter_( dayName, _len, &to, &to_len );
    if ( ( to != NULL ) && ( to_len != 0 ) ) {
      dayName = to;
    }
  }

  sprintf(dayBuf, more ? "%s *" : "%s", euc_to_octal(dayName));
  x_print_string(gInfo, dayBuf, (x1 + x2) / 2, (y1 + y2) / 2,
		 CENTER_AT_X | CENTER_AT_Y);

  gInfo->u.weekInfo.lineX = x1;
  gInfo->u.weekInfo.lineY = y2 + ((gInfo->normalWd + 1) / 2) + VMARGIN(gInfo);
  gInfo->u.weekInfo.lineMaxY = y1 + gInfo->u.weekInfo.boxHt;
}

void
x_week_sched_init(void *gInfoP)
{
  CMGraphicsInfo *gInfo = (CMGraphicsInfo *)gInfoP;

  memset(gInfo->u.weekInfo.schedBucket, 0,
	 sizeof(gInfo->u.weekInfo.schedBucket));
}

void
x_week_sched_draw(void *gInfoP, int dayIndex)
{
  CMGraphicsInfo *gInfo = (CMGraphicsInfo *)gInfoP;
  int i;
  Props *p = (Props *)gInfo->c->properties;
  int begin = get_int_prop(p, CP_DAYBEGIN);
  int end = get_int_prop(p, CP_DAYEND);
  int num_hours = end - begin;
  int nIntervals = num_hours * 4;
  int x1, y1, x2, y2;
  int boxWd = gInfo->u.weekInfo.schedBoxX2 - gInfo->u.weekInfo.schedBoxX1;
  int boxHt = gInfo->u.weekInfo.schedBoxY2 - gInfo->u.weekInfo.schedBoxY1;
  int currColor, lastColor;
  int tOffset, bOffset;

  x1 = gInfo->u.weekInfo.schedBoxX1 +
    ((dayIndex * boxWd) / DAYS_IN_WEEK) + ((gInfo->normalWd + 1) / 2);
  x2 = gInfo->u.weekInfo.schedBoxX1 +
    (((dayIndex + 1) * boxWd) / DAYS_IN_WEEK) - (gInfo->normalWd / 2) - 1;

  lastColor = 0;
  for (i = 0; i < nIntervals; i++)
  {
    y1 = gInfo->u.weekInfo.schedBoxY1 + ((i * boxHt) / nIntervals);
    y2 = gInfo->u.weekInfo.schedBoxY1 + (((i + 1) * boxHt) / nIntervals);
    switch (gInfo->u.weekInfo.schedBucket[i])
    {
    case 0:
      currColor = 0;
      break;

    case 1:
      currColor = 1;
      tOffset = (i % 4) ? 0 : ((gInfo->thinWd + 1) / 2);
      bOffset = ((i + 1) % 4) ? 1 : (gInfo->thinWd / 2) + 1;
      x_fill_dark_box(gInfo, x1, y1 + tOffset, x2,
		      y2 - bOffset, 0);
      break;

    default:
      currColor = 2;
      x_fill_black_box(gInfo, x1, y1, x2, y2);
      break;
    }

    if (currColor != lastColor)
    {
      if ((currColor != 2) && (lastColor != 2))
	x_draw_line(gInfo, x1, y1, x2 + 1, y1, gInfo->thinWd);
      lastColor = currColor;
    }
  }
}

void
x_init_month(void *gInfoP, int nRows)
{
  CMGraphicsInfo *gInfo = (CMGraphicsInfo *)gInfoP;
  CMFontInfo *fInfo;

  gInfo->reportType = PR_MONTH_VIEW;

  /* Load month fonts */
  if (gInfo->nFonts == 0)
  {
    int i;

    for (i = 0; i < MV_NUM_FONTS; i++)
      cm_load_font(gInfo, i, mvFonts[i]);

    /* Set curFont to first loaded font. */
    gInfo->curFont = gInfo->fontInfo;
  }

  cm_adjust_margins(gInfo);

  /* --- init for month view print --- */
  fInfo = cm_find_font(gInfo, MV_HEADER_FONT);
  gInfo->u.monthInfo.curX1 = gInfo->leftMarginX + (gInfo->normalWd / 2);
  gInfo->u.monthInfo.curY1 =
    gInfo->topMarginY + cm_font_height(gInfo, fInfo) +
      VMARGIN(gInfo) + (gInfo->normalWd / 2);
  gInfo->u.monthInfo.boxWd =
    gInfo->rightMarginX - gInfo->leftMarginX - gInfo->normalWd;
  gInfo->u.monthInfo.boxHt =
    gInfo->bottomMarginY - ((gInfo->normalWd + 1) / 2) -
      gInfo->u.monthInfo.curY1;
  fInfo = cm_find_font(gInfo, MV_DAY_FONT);
  gInfo->u.monthInfo.tab1 = 2 * cm_font_height(gInfo, fInfo);
  gInfo->u.monthInfo.nRows = nRows;
}

int
x_get_month_lines_per_page(void *gInfoP)
{
  CMGraphicsInfo *gInfo = (CMGraphicsInfo *)gInfoP;
  CMFontInfo *fInfo = cm_find_font(gInfo, MV_APPT_FONT);
  CMFontInfo *dateFInfo = cm_find_font(gInfo, MV_DATE_FONT);
  int dateSz = HMARGIN(gInfo) + cm_font_height(gInfo, dateFInfo);

  return (((gInfo->u.monthInfo.boxHt - gInfo->u.monthInfo.tab1) /
	   gInfo->u.monthInfo.nRows) -
	  gInfo->normalWd - dateSz - (2 * VMARGIN(gInfo))) /
    cm_font_height(gInfo, fInfo);
}

void
x_month_daynames(void *gInfoP, int nRows)
{
  CMGraphicsInfo *gInfo = (CMGraphicsInfo *)gInfoP;
  char *day_of_week[DAYS_IN_WEEK];
  int i;
  Calendar *c = gInfo->c;
  int x1, y1, x2, y2;
  int dayBoxHt;

  for (i = 0; i < DAYS_IN_WEEK; i++)
    local_dayname3(gInfo->c, &(day_of_week[i]), i);

  /* Draw outer box. */
  x1 = gInfo->u.monthInfo.curX1;
  y1 = gInfo->u.monthInfo.curY1;
  x2 = x1 + gInfo->u.monthInfo.boxWd;
  y2 = y1 + gInfo->u.monthInfo.boxHt;
  x_draw_box(gInfo, x1, y1, x2, y2, gInfo->normalWd);

  /* Draw box around days. */
  y2 = y1 + gInfo->u.monthInfo.tab1;
  x_fill_dark_box(gInfo, x1, y1, x2, y2, gInfo->normalWd);

  cm_set_font(gInfo, MV_DAY_FONT);
  for (i = 0; i < DAYS_IN_WEEK; i++)
  {
    x1 = gInfo->u.monthInfo.curX1 +
      ((i * gInfo->u.monthInfo.boxWd) / DAYS_IN_WEEK);
    x2 = gInfo->u.monthInfo.curX1 +
      (((i + 1) * gInfo->u.monthInfo.boxWd) / DAYS_IN_WEEK);

    cm_reverse_video(gInfo, True);
    x_print_string(gInfo, day_of_week[i],
		   (x1 + x2) / 2, (y1 + y2) / 2,
		   CENTER_AT_X | CENTER_AT_Y);
    free(day_of_week[i]);

    cm_reverse_video(gInfo, False);
    if (i != 0)
      x_draw_line(gInfo, x1, y1, x1, y1 + gInfo->u.monthInfo.boxHt,
		  gInfo->normalWd);
  }

  x1 = gInfo->u.monthInfo.curX1;
  x2 = x1 + gInfo->u.monthInfo.boxWd;
  dayBoxHt = gInfo->u.monthInfo.boxHt - gInfo->u.monthInfo.tab1;

  /* Draw horizontal lines. */
  for (i = 1; i < nRows; i++)
  {
    y1 = gInfo->u.monthInfo.curY1 + gInfo->u.monthInfo.tab1 +
      ((i * dayBoxHt) / nRows);
    x_draw_line(gInfo, x1, y1, x2, y1, gInfo->normalWd);
  }
}

void
x_print_little_months(void *gInfoP, Tick tick)
{
  CMGraphicsInfo *gInfo = (CMGraphicsInfo *)gInfoP;
  int m = month(tick);		/* current month just printed */
  int y = year(tick);		/* year of month just printed */
  int fday = fdom(tick);	/* first day of month, 0=Sun ... */

  int nm, nmy, pm, pmy;

  int ndays = monthlength(tick);
  int nrows = gInfo->u.monthInfo.nRows;
  int pmIndex;
  int i, nBoxes;
  int x1, y1, x2, y2;
  int boxX1, boxY1, boxWd, boxHt;
  int ulOffset, brOffset;
  int dayRow, dayCol;
  int spacing;

  /* 
   * Print out miniature prev & next month on month grid.
   * Check if there is enough room at end;  if not then
   * print at beginning of grid.
   */
  nBoxes = nrows * DAYS_IN_WEEK;
  if ((fday + ndays + 2) <= nBoxes)
  {
    pmIndex = nBoxes - 2;
  }
  else
  {
    pmIndex = 0;
  }

  if (m == 12)
  {
    nm = 1;
    nmy = y + 1;
    pm = m - 1;
    pmy = y;
  }
  else if (m == 1)
  {
    nm = m + 1;
    nmy = y;
    pm = 12;
    pmy = y - 1;
  }
  else
  {
    nm = m + 1;
    pm = m - 1;
    nmy = pmy = y;
  }

  boxX1 = gInfo->u.monthInfo.curX1;
  boxY1 = gInfo->u.monthInfo.curY1 + gInfo->u.monthInfo.tab1;
  boxWd = gInfo->u.monthInfo.boxWd;
  boxHt = gInfo->u.monthInfo.boxHt - gInfo->u.monthInfo.tab1;
  ulOffset = (gInfo->normalWd + 1) / 2;
  brOffset = (gInfo->normalWd / 2) + 1;

  /* Determine spacing for drawing little months.  Can be negative. */
  for (spacing = VMARGIN(gInfo);
       cm_tiny_month_height(gInfo, spacing) >
       (boxHt / nrows) - gInfo->normalWd;
       spacing--)
    ;

  /* Gray out extra boxes. */
  y1 = boxY1 + ulOffset;
  y2 = boxY1 - brOffset + (boxHt / nrows);
  for (i = 0; i < fday; i++)
  {
    x1 = boxX1 + ((i * boxWd) / DAYS_IN_WEEK) + ulOffset;
    x2 = boxX1 + (((i + 1) * boxWd) / DAYS_IN_WEEK) - brOffset;

    x_fill_light_box(gInfo, x1, y1, x2, y2, 0);

    if (i == pmIndex)
      x_print_month(gInfoP, pm, pmy, x1, y1, x2, y2, spacing);
    else if (i == (pmIndex + 1))
      x_print_month(gInfoP, nm, nmy, x1, y1, x2, y2, spacing);
  }

  dayRow = (nBoxes - 1) / DAYS_IN_WEEK;
  y1 = boxY1 + ((dayRow * boxHt) / nrows) + ulOffset;
  y2 = boxY1 + (((dayRow + 1) * boxHt) / nrows) - brOffset;
  for (i = fday + ndays; i < nBoxes; i++)
  {
    dayCol = i % DAYS_IN_WEEK;
    x1 = boxX1 + ((dayCol * boxWd) / DAYS_IN_WEEK) + ulOffset;
    x2 = boxX1 + (((dayCol + 1) * boxWd) / DAYS_IN_WEEK) - brOffset;

    x_fill_light_box(gInfo, x1, y1, x2, y2, 0);

    if (i == pmIndex)
      x_print_month(gInfoP, pm, pmy, x1, y1, x2, y2, spacing);
    else if (i == (pmIndex + 1))
      x_print_month(gInfoP, nm, nmy, x1, y1, x2, y2, spacing);
  }
}

void
x_month_timeslots(void *gInfoP, Tick tick, Boolean more)
{
  CMGraphicsInfo *gInfo = (CMGraphicsInfo *)gInfoP;
  int fday = fdom(tick);	/* first day of month, 0=Sun ... */
  int thisDay = dom(tick);
  int nrows = gInfo->u.monthInfo.nRows;
  int x1, y1;
  int boxX1, boxY1, boxWd, boxHt;
  int ulOffset, brOffset;
  int dayRow, dayCol;
  char dateBuf[BUFFERSIZE];

  boxX1 = gInfo->u.monthInfo.curX1;
  boxY1 = gInfo->u.monthInfo.curY1 + gInfo->u.monthInfo.tab1;
  boxWd = gInfo->u.monthInfo.boxWd;
  boxHt = gInfo->u.monthInfo.boxHt - gInfo->u.monthInfo.tab1;
  ulOffset = (gInfo->normalWd + 1) / 2;
  brOffset = (gInfo->normalWd / 2) + 1;

  /* Write date into appropriate box. */
  cm_set_font(gInfo, MV_DATE_FONT);
  dayRow = (thisDay + fday - 1) / DAYS_IN_WEEK;
  dayCol = (thisDay + fday - 1) % DAYS_IN_WEEK;

  gInfo->u.monthInfo.lineY = boxY1 + ((dayRow * boxHt) / nrows);
  gInfo->u.monthInfo.lineX = boxX1 + ((dayCol * boxWd) / DAYS_IN_WEEK);
  gInfo->u.monthInfo.lastX = gInfo->u.monthInfo.lineX + ulOffset;
  gInfo->u.monthInfo.lineMaxY = boxY1 + (((dayRow + 1) * boxHt) / nrows);
  gInfo->u.monthInfo.lineMaxX = boxX1 + (((dayCol + 1) * boxWd) /
					 DAYS_IN_WEEK);
  y1 = gInfo->u.monthInfo.lineY + ulOffset + HMARGIN(gInfo);
  x1 = gInfo->u.monthInfo.lineX + ulOffset + VMARGIN(gInfo);

  sprintf(dateBuf, more ? "%d *" : "%d", thisDay);
  x_print_string(gInfo, dateBuf, x1, y1, LEFT_AT_X | TOP_AT_Y);

  gInfo->u.monthInfo.lineY =
    y1 + cm_font_height(gInfo, gInfo->curFont) + VMARGIN(gInfo);
}

Boolean
x_print_month_appts(void *gInfoP, CSA_entry_handle *list,
		    int a_total, int num_page,
		    time_t hi_hour, int lines_per_box)
{
  CMGraphicsInfo *gInfo = (CMGraphicsInfo *)gInfoP;
  int indented;
  Lines *lines;
  char buf1[BUFFERSIZE];
  Calendar *c = gInfo->c;
  Props *pr = (Props *)c->properties;
  int meoval = get_int_prop(pr, CP_PRINTPRIVACY);
  int i, start, pos = 1, line_counter = 0;	
  CSA_return_code stat;
  Dtcm_appointment *appt;
  Tick start_tick;

  start = ((num_page - 1) * lines_per_box) + 1;

  appt = allocate_appt_struct(appt_read,
			      c->general->version,
			      CSA_ENTRY_ATTR_CLASSIFICATION_I,
			      CSA_ENTRY_ATTR_START_DATE_I,
			      CSA_ENTRY_ATTR_SUMMARY_I,
			      CSA_X_DT_ENTRY_ATTR_SHOWTIME_I,
			      NULL);
  for (i = 0; i < a_total; i++)
  {
    stat = query_appt_struct(c->cal_handle, list[i], appt);
    if (stat != CSA_SUCCESS)
    {
      free_appt_struct(&appt);
      return False;
    }

    _csa_iso8601_to_tick(appt->time->value->item.date_time_value, &start_tick);
    if (start_tick >= hi_hour)
    {
      free_appt_struct(&appt);
      return True;
    }
 
    if ((privacy_set(appt) == CSA_CLASS_PUBLIC) &&
	!(meoval & PRINT_PUBLIC))
      continue;
    if ((privacy_set(appt) == CSA_CLASS_CONFIDENTIAL) &&
	!(meoval & PRINT_SEMIPRIVATE))
      continue;
    if ((privacy_set(appt) == CSA_CLASS_PRIVATE) &&
	!(meoval & PRINT_PRIVATE))
      continue;

    if (pos < start)
    {
      pos++;
    }
    else
    {
      get_time_str(appt, buf1);

      indented = (*buf1 != NULL);

      lines = text_to_lines(appt->what->value->item.string_value, 1);
      line_counter++;
      if ((line_counter > lines_per_box) && (lines != NULL))
      {
	free_appt_struct(&appt);
	return(False);
      }

      /* only print if appt text found */ 
      if ((lines != NULL) && (lines->s != NULL))
      {
	if (indented)  	/* time found so print it */
	  cm_print_month_time(gInfo, buf1);
	cm_print_month_text(gInfo, lines->s, indented ? 2 : 0);
      }

      destroy_lines(lines);
    }
  }

  free_appt_struct(&appt);
  return(True);
}
