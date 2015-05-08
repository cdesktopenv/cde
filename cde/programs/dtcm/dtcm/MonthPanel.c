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
/*********************************************************************************
**  MonthPanel.c
**
**  $XConsortium: MonthPanel.c /main/7 1996/11/21 19:41:55 drk $
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

/*
 * Month Panel widget implementation (class XmMonthPanel)
 *
 * The XmMonthPanel widget is rigged with the Motif widget binary
 * compatibility mechanism.  All Motif-specific changes for this mechanism
 * are preceded by a comment including the string "MotifBc"
 *
 * For a description of the Motif widget binary compatibility mechanism
 * see the reference manual entry on XmResolvePartOffsets().
 *
 */
#include <EUSCompat.h>
#include <stdio.h>
#include <stdint.h>
#include <Xm/ManagerP.h>
#include <Xm/PushBG.h>
#include <Xm/SeparatoG.h>
#include <Xm/LabelG.h>
#include "MonthPanelP.h"
#include "timeops.h"
#include "misc.h"

#define XOS_USE_XT_LOCKING
#define X_INCLUDE_TIME_H
#if defined(linux)
#undef SVR4
#endif
#include <X11/Xos_r.h>

/*
 * MotifBc - index value for this class is superclass' value + 1
 */
#ifdef MOTIF_BC
#define XmMonthPanelIndex (XmManagerIndex + 1)
#endif /* MOTIF_BC */

/*
 * MotifBc - dynamic offset tables
 */
#ifdef MOTIF_BC
static XmOffsetPtr ipot;	/* Instance part offset table */
static XmOffsetPtr cpot;	/* Constraint part offset table */
#endif /* MOTIF_BC */

/*
 * MotifBc - macros for accessing instance and constraint fields
 */
#ifdef MOTIF_BC
#define Year(w)		XmField(w, ipot, XmMonthPanel, year, int)
#define Month(w)	XmField(w, ipot, XmMonthPanel, month, int)
#define Callback(w)	XmField(w, ipot, XmMonthPanel, callback, XtCallbackList)
#define TitleFormat(w)	XmField(w, ipot, XmMonthPanel, title_format, String)
#define Header(w)	XmField(w, ipot, XmMonthPanel, header, Widget)
#define DayLabels(w)	XmField(w, ipot, XmMonthPanel, day_labels, Widget *)
#define Days(w)		XmField(w, ipot, XmMonthPanel, days, Widget *)
#endif /* MOTIF_BC */


/********    Static Function Declarations    ********/
static void scale_components(XmMonthPanelWidget);

static void ClassPartInitialize(WidgetClass) ;
static void Initialize(Widget, Widget, ArgList, Cardinal *) ;
static void Redisplay(Widget, XEvent *, Region);
static void DoLayout(XmMonthPanelWidget) ;
static void Resize(Widget) ;
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *) ;
static XtGeometryResult QueryProc(Widget, XtWidgetGeometry*, XtWidgetGeometry*);
static XtGeometryResult GeometryManager( Widget, XtWidgetGeometry *, XtWidgetGeometry *);
static void DayCallback(Widget, XtPointer, XtPointer);
static void MonthCallback(Widget, XtPointer, XtPointer);
static void set_header_string(XmMonthPanelWidget);
static Boolean clipped(Widget);
/********    End Static Function Declarations    ********/


/*
 * These arrays are implemented in calendarA.c.  They are arrays
 * of the local strings for day names and month names.  The day
 * arrays start from 0, the month array starts from 1; no particular
 * reason for this difference that I can see.
 */
extern char *days3[];		/* eg: "S"   */
extern char *months[];		/* eg: "January" */

/*
 * widget instance names for column header labels in panel
 */
static char *col_hdr[] = { "sunday", "monday", "tuesday", "wednesday",
                           "thursday", "friday", "saturday" };


/************************************************************************
 *									*
 * Month Panel Resources						*
 *									*
 ************************************************************************/

static XtResource resources[] = 
{
    { XmNyear, XmCYear, XmRInt, sizeof(int),
        XtOffsetOf( struct _XmMonthPanelRec, month_panel.year),
	XmRImmediate, (XtPointer)NULL },
    { XmNmonth, XmCMonth, XmRInt, sizeof(int),
        XtOffsetOf( struct _XmMonthPanelRec, month_panel.month),
	XmRImmediate, (XtPointer)NULL },
    { XmNactivateCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
        XtOffsetOf( struct _XmMonthPanelRec, month_panel.callback),
        XmRImmediate, (XtPointer)NULL },
    { XmNtitleFormat, XmCTitleFormat, XmRString, sizeof(char *),
	XtOffsetOf( struct _XmMonthPanelRec, month_panel.title_format),
	XtRString, (XtPointer)"%B %Y" },
    { XmNactiveHeader, XmCActiveHeader, XmRBoolean, sizeof(Boolean),
	XtOffsetOf( struct _XmMonthPanelRec, month_panel.active_header),
	XtRImmediate, (XtPointer)False },
    { XmNactiveDays, XmCActiveDays, XmRBoolean, sizeof(Boolean),
	XtOffsetOf( struct _XmMonthPanelRec, month_panel.active_days),
	XtRImmediate, (XtPointer)False },
    { XmNshowPanelHeader, XmCShowPanelHeader, XmRBoolean, sizeof(Boolean),
	XtOffsetOf( struct _XmMonthPanelRec, month_panel.show_panel_header),
	XtRImmediate, (XtPointer)True },
    { XmNshowColumnHeaders, XmCShowColumnHeaders, XmRBoolean, sizeof(Boolean),
	XtOffsetOf( struct _XmMonthPanelRec, month_panel.show_column_headers),
	XtRImmediate, (XtPointer)True },
    { XmNshowSeparator, XmCShowSeparator, XmRBoolean, sizeof(Boolean),
	XtOffsetOf( struct _XmMonthPanelRec, month_panel.show_separator),
	XtRImmediate, (XtPointer)True }
};



/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

externaldef(xmmonthpanelclassrec) XmMonthPanelClassRec
             xmMonthPanelClassRec = {
  {
/* core_class fields      */
    /* superclass         */    (WidgetClass) &xmManagerClassRec,
    /* class_name         */    "XmMonthPanel",
    /* widget_size        */    sizeof(XmMonthPanelRec),
    /* class_initialize   */    NULL,
    /* class_partinit     */    ClassPartInitialize,
    /* class_inited       */	FALSE,
    /* initialize         */    Initialize,
    /* Init hook	  */    NULL,
    /* realize            */    XtInheritRealize,
    /* actions		  */	NULL,
    /* num_actions	  */	0,
    /* resources          */    resources,
    /* num_resources      */    XtNumber(resources),
    /* xrm_class          */    NULLQUARK,
    /* compress_motion	  */	TRUE,
    /* compress_exposure  */	XtExposeCompressMaximal,
    /* compress_enterleave*/	TRUE,
    /* visible_interest   */    FALSE,
    /* destroy            */    NULL,
    /* resize             */    Resize,
    /* expose             */    Redisplay,
    /* set_values         */    SetValues,
    /* set values hook    */    NULL,
    /* set values almost  */    XtInheritSetValuesAlmost,
    /* get values hook    */    NULL,
    /* accept_focus       */    NULL,
    /* Version            */    XtVersion,
    /* PRIVATE cb list    */    NULL,
    /* tm_table		  */    XtInheritTranslations,
    /* query_geometry     */    XtInheritQueryGeometry,
    /* display_accelerator*/    NULL,
    /* extension          */    NULL,
  },
  {
/* composite_class fields */
    /* geometry_manager   */    GeometryManager,
    /* change_managed     */    XtInheritChangeManaged,
    /* insert_child	  */	XtInheritInsertChild,
    /* delete_child	  */	XtInheritDeleteChild,	/* Inherit from superclass */
    /* Extension          */    NULL,
  },{
/* Constraint class Init */
    NULL,
    0,
    0,
    NULL,
    NULL,
    NULL,
    NULL
      
  },
/* Manager Class */
   {		
      XtInheritTranslations,    		/* translations        */    
      NULL,					/* get resources      	  */
      0,					/* num get_resources 	  */
      NULL,					/* get_cont_resources     */
      0,					/* num_get_cont_resources */
      XmInheritParentProcess,                   /* parent_process         */
      NULL,					/* extension           */    
   },

 {
/* Month Panel class - none */     
     /* mumble */               0
 }	
};

externaldef(xmmonthpanelwidgetclass) WidgetClass
             xmMonthPanelWidgetClass = (WidgetClass)&xmMonthPanelClassRec;




/************************************************************************
 *									*
 *  ClassPartInitialize - Set up the fast subclassing.			*
 *									*
 ************************************************************************/
static void 
ClassPartInitialize( WidgetClass wc )
{
/*
   _XmFastSubclassInit (wc, XmMONTH_PANEL_BIT);
*/
}


/************************************************************************
 *									*
 *  Initialize								*
 *									*
 ************************************************************************/
/* ARGSUSED */
static void 
Initialize( Widget rw, Widget nw, ArgList args, Cardinal *num_args )
{
	int i=0;
	Arg wargs[5];
	Widget header;
	XmString str;
	struct tm *tm_ret;
	time_t timer;
	Tick tmptick;
	_Xltimeparams localtime_buf;
	
        XmMonthPanelWidget request = (XmMonthPanelWidget) rw ;
        XmMonthPanelWidget new_w = (XmMonthPanelWidget) nw ;

        char buf[BUFSIZ];

	new_w->month_panel.display_rows = 6;  /* minimum to show all days */

/*
 * create and cache panel header ( eg. "January") (depends on format resource)
 *
 * Whether XmLabels or XmPushButton gadgets are used is selectable via
 * the boolean "activeDays" and "activeHeader" resource. True gets you buttons.
 */

	if (new_w->month_panel.active_header) {
	   new_w->month_panel.header =
		XmCreatePushButtonGadget((Widget)new_w, "header", NULL, 0);
	   XtAddCallback(new_w->month_panel.header, XmNactivateCallback,
		MonthCallback, NULL);
	   new_w->month_panel.separator = (Widget) NULL;
	}
	else {
	   new_w->month_panel.header =
		XmCreateLabelGadget((Widget)new_w, "header", NULL, 0);
	   new_w->month_panel.separator =
		XmCreateSeparatorGadget((Widget)new_w, "separator", NULL, 0); 
	   /* Managing of separator is switchable */
	   if (new_w->month_panel.show_separator) {
	      XtManageChild(new_w->month_panel.separator);
              /* separator takes another row to display */
              (new_w->month_panel.display_rows)++;
           }
	}

	/* set header label */

/*
**  Problem in set_header_string.  It references month_panel.month
** and month_panel.year, which aren't initialized in when it is called
** in the Initialize function.  Set them to today.
*/
	time (&timer);
	tm_ret = _XLocaltime(&timer, localtime_buf);
	new_w->month_panel.month = tm_ret->tm_mon + 1;
	new_w->month_panel.year = 1900 + tm_ret->tm_year;

	set_header_string(new_w);

	/* Managing of header is switchable */
	if (new_w->month_panel.show_panel_header) {
 	   XtManageChild(new_w->month_panel.header);
           /* header takes another row to display */
           (new_w->month_panel.display_rows)++;
        }

/*
 * create and cache id's of, column header day names
 */
	new_w->month_panel.day_labels =
		(Widget *) XtMalloc( 7 * sizeof(Widget) );
        for (i=0; i < 7; i++) {
           XmString str;
           Widget label;

	   /* I18N:next line: wrap the string & fetch from catalog */
           str = XmStringCreateLocalized(days3[i]);
           XtSetArg(wargs[0], XmNlabelString, str);
           label = XmCreateLabelGadget((Widget) new_w, col_hdr[i], wargs, 1);
	   /* Managing of column headers is switchable */
	   if (new_w->month_panel.show_column_headers)
              XtManageChild(label);
           new_w->month_panel.day_labels[i] = label; /* cache in panel inst. */
           XmStringFree(str);
        }

        /* column headers take another row to display */
        if (new_w->month_panel.show_column_headers)
           (new_w->month_panel.display_rows)++;

/*
 * create and cache id's of, 31 buttons for days
 */
	new_w->month_panel.days = (Widget *) XtMalloc( 31 * sizeof(Widget) );
        for (i=0; i < 31; i++) {
           char buf[BUFSIZ];
           XmString str;
           Widget btn;

           sprintf(buf, "%d", i+1);
           str = XmStringCreateLocalized(buf);
           XtSetArg(wargs[0], XmNlabelString, str);
           sprintf(buf, "day%d", i+1);

/* choose between buttons or labels for day objects */
	   if (new_w->month_panel.active_days) {
		btn = XmCreatePushButtonGadget((Widget)new_w, buf, wargs, 1);
		XtAddCallback(btn, XmNactivateCallback, DayCallback,
			(XtPointer) (intptr_t) (i+1));
	   }
	   else {
		btn = XmCreateLabelGadget((Widget) new_w, buf, wargs, 1);
	   }
	   new_w->month_panel.days[i] = btn;/* cache id in panel instance */
	   XmStringFree(str);
	}
	XtManageChildren((WidgetList)new_w->month_panel.days, 31);

/*
 * Set all these components to the appropriate size for the container
 * Then position them correctly.
 */
	scale_components(new_w);

	DoLayout(new_w);
}

static void
Redisplay(Widget w, XEvent *ev, Region region)
{
	XmMonthPanelWidget panel = (XmMonthPanelWidget) w;

	XmeRedisplayGadgets( w, ev, region) ;
}


/************************************************************************
 *									*
 * DoLayout - Layout the month panel.					*
 *                                                                      *
 ************************************************************************/
static void 
DoLayout( XmMonthPanelWidget mw )
{
	XmMonthPanelPart *mpp = &(mw->month_panel);
	Dimension w = mw->core.width;
	Dimension h = mw->core.height;
	int col_w = (int) w/7;
	int row_h;
	Boolean tall_header;
	Position x=0;
	Position y=0;
	int days_in_month = monthlength(monthdayyear(mpp->month,1,mpp->year));
	int first = fdom(monthdayyear(mpp->month,1,mpp->year));
	Widget this_widget;
	int i=0;

   	if (!mw->month_panel.active_days && mw->month_panel.active_header) {
		tall_header = True;
		row_h = (int) h/(mpp->display_rows + 1);
	}
	else {
		tall_header = False;
		row_h = (int) h/(mpp->display_rows);
	}

/* 
 * Make sure the variable buttons are correctly [un]managed.
 * (the "variable" buttons are those for 29th, 30th and 31st)
 */
	for (i=28; i < 31; i++) {
		Widget this_widget = (mpp->days)[i];
		if (days_in_month > i) {
			if (!XtIsManaged(this_widget))
				XtManageChild(this_widget);
		}
		else {
			if (XtIsManaged(this_widget))
				XtUnmanageChild(this_widget);
		}
	}

/*
 * position the panel header
 */
	XtMoveWidget(mpp->header, 0, 0);

/*
 * position the separator, if any
 */
	if (mpp->separator != NULL) {
		if (tall_header)
			XtMoveWidget(mpp->separator,
				(Position) w/8, (Position) 2 * row_h);
		else
			XtMoveWidget(mpp->separator,
				(Position) w/8, (Position)  row_h);
	}

/*
 * position the day labels
 */
	x = (Position) 0;
	y = (Position) (mw->month_panel.display_rows -
			(tall_header ? 6 : 7)) * row_h;

	i=0;
        while (i < 7) {
		this_widget = mw->month_panel.day_labels[i];
		XtMoveWidget(this_widget, x, y);
		x += col_w;
                i++;
        }

/*
 * position the day buttons
 */
	x = (Position) first * col_w;
	y = (Position) (mw->month_panel.display_rows -
			(tall_header ? 5 : 6)) * row_h;

	i=0;
	while (i < 31){
		this_widget = mw->month_panel.days[i];

		/* don't bother with unmanaged buttons */
		if (XtIsManaged(this_widget))
			XtMoveWidget(this_widget, x, y);
		i++;

		/* take new row after each 7 buttons */
		if ( ( (i+first) % 7 ) == 0 ) {
			x = 0;
			y += row_h;
		}
		else
			x += col_w;
	}

}


/************************************************************************
 *
 * scale_components - Make all the labels, buttons correct size for this
 *                    size space.
 *
 *                    [4/12/94] - Check whether new size causes labels
 *                                to be clipped.  If it does, set labelType
 *                                to PIXMAP.  A suitable labelPixmap should be
 *                                set to indicate "label too small".
 ************************************************************************/

static void
scale_components(XmMonthPanelWidget mw) {

	XmMonthPanelPart *mpp = &(mw->month_panel);
	Dimension w=mw->core.width;
	Dimension h=mw->core.height;
	int row_h;
	Boolean tall_header;
	int col_w = (int) w/7;
	int i=0;
	Widget this_widget;
	unsigned char ltype;
	Pixel fg, bg;
	Pixmap head_pm, col_head_pm, btn_pm;

   	if (!mw->month_panel.active_days && mw->month_panel.active_header) {
		tall_header = True;
		row_h = (int) h/(mpp->display_rows + 1);
	}
	else {
		tall_header = False;
		row_h = (int) h/(mpp->display_rows);
	}

	/******************************************************************
	 * get pixmap in colors needed - used to replace text when clipped.
	 *
	 * NOTE1: Don't need to release the pixmaps, because they come from
	 * the Motif-owned image cache.
	 * 
	 * NOTE2: setting the pixmaps should be in redisplay rather than
	 * here - only done here because this is where labelType is toggled.
	 ******************************************************************/
	XtVaGetValues(XtParent(mw), XmNforeground, &fg, XmNbackground, &bg, NULL);
	head_pm = XmGetPixmap(XtScreen(mw), "slant_right", fg, bg);
	col_head_pm = XmGetPixmap(XtScreen(mw), "slant_left", fg, bg);
	btn_pm = XmGetPixmap(XtScreen(mw), "50_foreground", fg, bg);


	/*********************************************************************
	 * This algorithm allocates equal header, label and button heights;
	 * equal label and button widths; header spans container width.
	 *********************************************************************/

	/**************************
	 * set panel header size
	 **************************/
	
	if (tall_header)
		XtResizeWidget(mpp->header, w, 2 * row_h, mpp->header->core.border_width);
	else
		XtResizeWidget(mpp->header, w, row_h, mpp->header->core.border_width);

	if (clipped(mpp->header)) {
		XtVaSetValues(mpp->header,
			XmNlabelType, XmPIXMAP,
			XmNlabelPixmap, head_pm,
			NULL);
	}
	else
		XtVaSetValues(mpp->header,
			XmNlabelType, XmSTRING,
			NULL);

	/**************************
	 * separator
	 **************************/
	if (mpp->separator != NULL) {
		XtResizeWidget(mpp->separator, 3*(int)w/4, row_h, 0);
	}
	
	/**************************
	 * set column-heading label sizes
	 **************************/
	for (i=0; i<7; i++) {
		this_widget = (mpp->day_labels)[i];
		XtResizeWidget(this_widget, col_w, row_h, 0);
	}	
	/* display pixmap if clipped, string otherwise */
	this_widget = (mpp->day_labels)[6];
	XtVaGetValues(this_widget, XmNlabelType, &ltype, NULL);
	if (clipped(this_widget)) {
	   if (ltype == XmSTRING) {
		for (i=0; i<7; i++) {
		   XtVaSetValues((mpp->day_labels)[i],
			XmNlabelType, XmPIXMAP,
			XmNlabelPixmap, col_head_pm,
			NULL);
		}	
	   }
	}
	else {
	   if (ltype == XmPIXMAP) {
		for (i=0; i<7; i++) {
		   XtVaSetValues((mpp->day_labels)[i], XmNlabelType, XmSTRING, NULL);
		}	
	   }
	}

	/**************************
	 * Day buttons
	 **************************/
	for (i=0; i<31; i++) {
		this_widget = (mpp->days)[i];
		XtResizeWidget(this_widget, col_w, row_h, 0);
	}	

	/*************************************************************
	 * If any of the button labels are clipped, replace the
	 * entire set with their pixmap equivalents.  The labelPixmap
	 * resource should be set to something that means "not big enough"
	 * (like a dot!)
	 *   This check should really be done in above loop, but it
	 * slows things down to catch the edge case (different fonts
	 * in different labels, so this is thought "good enough".
	 *   28th button picked at random for test - at least it's always a
	 * 2-digit label.
	 *************************************************************/
	this_widget = (mpp->days)[27];
	XtVaGetValues(this_widget, XmNlabelType, &ltype, NULL);
	if (clipped(this_widget)) {
	   if (ltype == XmSTRING) {
		for (i=0; i<31; i++) {
		   XtVaSetValues((mpp->days)[i],
			XmNlabelType, XmPIXMAP,
			XmNlabelPixmap, btn_pm,
			NULL);
		}	
	   }
	}
	else {
	   if (ltype == XmPIXMAP) {
		for (i=0; i<31; i++) {
		   XtVaSetValues((mpp->days)[i], XmNlabelType, XmSTRING, NULL);
		}	
	   }
	}
}

/*
 * Determine whether labelString requires more space
 * than its widget currently has.  This information
 * can be used to display clipped information differently
 */
static Boolean
clipped(Widget w) 
{
	XmString str;
	XmFontList fl;
	Dimension req_w, req_h, act_w, act_h, shd_w, hlt_w;

	return(False);

	XtVaGetValues(w,
		XmNlabelString, &str,
		XmNfontList, &fl,
		XmNhighlightThickness, &hlt_w,
		XmNshadowThickness, &shd_w,
		XmNwidth, &act_w,
		XmNheight, &act_h,
		NULL);

	XmStringExtent(fl, str, &req_w, &req_h);

	/* adjust actual width/height available for shadow and highlight */
	act_w -= ( (2*hlt_w) + (2*shd_w) );
	act_h -= ( (2*hlt_w) + (2*shd_w) );

	if ((req_w > act_w) || (req_h > act_h))
		return (True);
	else
		return(False);
}

/************************************************************************
 *                                                                      *
 *  Recompute the size of the month panel.				* 
 *									*
 ************************************************************************/
static void 
Resize( Widget wid )
{
    XmMonthPanelWidget mw = (XmMonthPanelWidget) wid ;
    XmManagerWidgetClass super = (XmManagerWidgetClass) xmManagerWidgetClass;

    scale_components(mw);
    DoLayout(mw);
    (*super->core_class.resize)((Widget) mw);
}

/***************************************************************************
 *									   *
 *  Geometry Manager
 *									   *
 ***************************************************************************/
static XtGeometryResult
GeometryManager( Widget w, XtWidgetGeometry *request, XtWidgetGeometry *reply )
{
        return (XtGeometryYes);
}

/***************************************************************************
 *									   *
 *  QueryProc (stub for now)						   *
 *									   *
 ***************************************************************************/
static XtGeometryResult 
QueryProc( Widget w, XtWidgetGeometry *request, XtWidgetGeometry *reply )
{
/*    XmMonthPanelWidget mw = (XmMonthPanelWidget) w;*/
    return(XtGeometryYes);
}


#if 0
/***************************************************************************
 *									   *
 *  CalcSize (stub for now- to be called by QueryProc)			   *
 *									   *
 ***************************************************************************/
static void
CalcSize(XmMonthPanelWidget mw, Dimension *replyWidth , Dimension *replyHeight)
{
    *replyWidth = mw->core.width;
    *replyHeight = mw->core.height;

}
#endif

/************************************************************************
 *									*
 *  SetValues								*
 *									*
 ************************************************************************/
static Boolean 
SetValues( Widget cw, Widget rw, Widget nw, ArgList args, Cardinal *num_args )
{
        XmMonthPanelWidget current = (XmMonthPanelWidget) cw ;
        XmMonthPanelWidget request = (XmMonthPanelWidget) rw ;
        XmMonthPanelWidget new_w = (XmMonthPanelWidget) nw ;
    Boolean relayout = FALSE;
    Boolean new_date = FALSE;

    if (request->month_panel.show_panel_header != current->month_panel.show_panel_header) {
       if (request->month_panel.show_panel_header) {
          XtManageChild(request->month_panel.header);
          (request->month_panel.display_rows)++;
       }
       else {
          XtUnmanageChild(request->month_panel.header);
          (request->month_panel.display_rows)--;
       }
       relayout = TRUE;
    }

    if (request->month_panel.show_column_headers != current->month_panel.show_column_headers) {
       if (request->month_panel.show_column_headers) {
          int col=0;
          while (col < 7) {
             XtManageChild(request->month_panel.day_labels[col]);
             col++;
          }
          (request->month_panel.display_rows)++;
       }
       else {
          int col=0;
          while (col < 7) {
             XtUnmanageChild(request->month_panel.day_labels[col]);
             col++;
          }
          (request->month_panel.display_rows)--;
       }
       relayout = TRUE;
    }
       
    if (request->month_panel.show_separator != current->month_panel.show_separator) {
       if (request->month_panel.show_separator) {
          XtManageChild(request->month_panel.separator);
          (request->month_panel.display_rows)++;
       }
       else {
          XtUnmanageChild(request->month_panel.separator);
          (request->month_panel.display_rows)--;
       }
       relayout = TRUE;
    }

    if (request->month_panel.month != current->month_panel.month) {

	/* range check - quietly force to nearest valid value */
	if (request->month_panel.month < 1)
		new_w->month_panel.month = 1;
	else if (request->month_panel.month > 12)
		new_w->month_panel.month = 12;

	new_date = TRUE;
    }

    if (request->month_panel.year != current->month_panel.year) {

	/* range check - quietly force to nearest valid value */
	if (request->month_panel.year < 1)
		new_w->month_panel.year = 1970;
	else if (request->month_panel.year > 2037)
		new_w->month_panel.year = 2037;
 
	new_date = TRUE;
    }

    if ((request->core.width != current->core.width) ||
	(request->core.height != current->core.height)) {
		relayout = TRUE;
    }

    if (strcmp(request->month_panel.title_format,
		new_w->month_panel.title_format) != 0) {
	set_header_string(new_w);
    }

    if (new_date) {
	set_header_string(new_w);
	relayout = TRUE;
    }

    if (relayout)
	DoLayout(new_w);

    return (relayout);
}

/*
 * set_header_string
 *
 * Sets label in month button of panel, according to the
 * setting of the titleFormat resource, which is assumed to
 * be a valid format string for stftime(3)
 */
static void
set_header_string(XmMonthPanelWidget mw)
{
	XmString str;
	struct tm *tm_ret;
	Tick tmptick;
	Arg wargs[3];
	char buf[BUFSIZ];
	_Xltimeparams localtime_buf;

	tmptick =
	   monthdayyear(mw->month_panel.month, 1 ,mw->month_panel.year);
	tm_ret = _XLocaltime(&tmptick, localtime_buf);
	(void) strftime(buf, BUFSIZ, mw->month_panel.title_format, tm_ret);

	str = XmStringCreateLocalized(buf);
	XtSetArg(wargs[0], XmNlabelString, str);
	XtSetValues(mw->month_panel.header, wargs, 1);
	XmStringFree(str);
}


/************************************************************************
 * DayCallback is invoked from day button children.
 * It invokes MonthPanel's own callbac list, setting the type in the
 * callback struct to DAY_SELECTION, and the day field to the index of the
 * button that was pressed.
 *
 ************************************************************************/
static void DayCallback(Widget w, XtPointer client, XtPointer call)
{
   XmMonthPanelWidget mp = (XmMonthPanelWidget) XtParent(w);
   XmMonthPanelCallbackStruct data;

   data.type = DAY_SELECTION;
   data.day = (intptr_t) client;

   XtCallCallbackList((Widget) mp, mp->month_panel.callback, &data);
}

/************************************************************************
 * MonthCallback is invoked from the header button.
 * It invokes MonthPanel's own callback list, setting the type in the
 * callback struct to MONTH_SELECTION, and zeroing out the day field.
 ************************************************************************/
static void MonthCallback(Widget w, XtPointer client, XtPointer call)
{
   XmMonthPanelWidget mp = (XmMonthPanelWidget) XtParent(w);
   XmMonthPanelCallbackStruct data;

   data.type = MONTH_SELECTION;
   data.day = 0;

   XtCallCallbackList((Widget) mp, mp->month_panel.callback, &data);
}

/************************************************************************
 *									*
 * XmCreateMonthPanel - convenience interface to XtCreateWidget.	*
 *									*
 ************************************************************************/
Widget 
XmCreateMonthPanel( Widget parent, char *name, ArgList args, Cardinal argCount )
{

    return ( XtCreateWidget( name, 
			     xmMonthPanelWidgetClass, 
			     parent, 
			     args, 
			     argCount ) );
}
