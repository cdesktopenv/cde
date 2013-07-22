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
#ifndef lint
#ifdef  VERBOSE_REV_INFO
static char rcs_id[] = "$XConsortium: TermView.c /main/1 1996/04/21 19:20:13 drk $";
#endif  /* VERBOSE_REV_INFO */
#endif  /* lint */

/*                                                                      *
 * (c) Copyright 1993, 1994, 1996 Hewlett-Packard Company               *
 * (c) Copyright 1993, 1994, 1996 International Business Machines Corp. *
 * (c) Copyright 1993, 1994, 1996 Sun Microsystems, Inc.                *
 * (c) Copyright 1993, 1994, 1996 Novell, Inc.                          *
 * (c) Copyright 1996 Digital Equipment Corporation.			*
 * (c) Copyright 1996 FUJITSU LIMITED.					*
 * (c) Copyright 1996 Hitachi.						*
 */

#include "TermHeader.h"
#include <X11/X.h>
#include "TermViewP.h"
#include <Xm/ScrolledW.h>
#include <Xm/ScrollBar.h>
#include <Xm/DrawP.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/Frame.h>
#include <Xm/RowColumn.h>
#include "Term.h"
#include "TermViewMenu.h"
#include "TermPrimDebug.h"
#include <time.h>
#include <signal.h>
#include <Xm/RepType.h>
#include "TermPrimRepType.h"
#include "TermViewHelp.h"
#include "TermPrimMessageCatI.h"

#define	widgetHeight(w)		((w)->core.height + 2 * (w)->core.border_width)
#define	widgetWidth(w)		((w)->core.width + 2 * (w)->core.border_width)
#define	ourHeightOffset(w)	((w)->manager.shadow_thickness + \
				(w)->termview.marginHeight)
#define	ourWidthOffset(w)	((w)->manager.shadow_thickness + \
				(w)->termview.marginWidth)
#define	ourHeightAdjustment(w) \
				(2 * ((w)->manager.shadow_thickness + \
				(w)->termview.marginHeight))
#define	ourWidthAdjustment(w) \
				(2 * ((w)->manager.shadow_thickness + \
				(w)->termview.marginWidth))
#define	ourWidth(w)		((w)->core.width - ourWidthAdjustment(w))
#define	ourHeight(w)		((w)->core.height - ourHeightAdjustment(w))

static void ClassInitialize(void);
static void ClassPartInitialize(WidgetClass wc);
static void Initialize(Widget rw, Widget nw, Arg *args, Cardinal *num_args);
static void Resize(Widget w);
static void Redisplay(Widget w, XEvent *event, Region region);
static Boolean SetValues(Widget cw, Widget rw, Widget nw, ArgList args,
	Cardinal *num_args);
static void Realize(Widget w, XtValueMask *p_valueMask,
	XSetWindowAttributes *attributes);
static void Destroy(Widget w);
static XtGeometryResult GeometryManager(Widget w, XtWidgetGeometry *desired,
	XtWidgetGeometry *allowed);
static void ChangeManaged(Widget w);
static void CreateScrollBar(Widget w);
static void SubprocessTerminationCallback(Widget w, XtPointer client_data,
	XtPointer call_data);
#ifdef	NOTUSED
static void GetDesiredChildWidth(Widget w, Widget child, int *childX,
	Dimension *childWidth);
#endif	/* NOTUSED */
static void handleEnterLeaveEvents(Widget w, XtPointer closure, XEvent *event,
	Boolean *cont);
static void handleProcessStructureNotifyEvent(Widget w, XtPointer eventData,
	XEvent *event, Boolean *cont);

char _DtTermViewMenuDefaultFonts[] = "\
-dt-interface user-medium-r-normal-xxs*-*-*-*-*-m-*:\n\
-dt-interface user-medium-r-normal-xs*-*-*-*-*-m-*:\n\
-dt-interface user-medium-r-normal-s*-*-*-*-*-m-*:\n\
-dt-interface user-medium-r-normal-m*-*-*-*-*-m-*:\n\
-dt-interface user-medium-r-normal-l*-*-*-*-*-m-*:\n\
-dt-interface user-medium-r-normal-xl*-*-*-*-*-m-*:\n\
-dt-interface user-medium-r-normal-xxl*-*-*-*-*-m-*:\n\
";

static void
GetBlinkRate
(
    Widget		  w,
    int			  resource_offset,
    XtArgVal		 *value
)
{
    DtTermViewWidget	  tw = (DtTermViewWidget) w;
    Arg			  al[1];
    int			  ac;
    int			  blinkRate;

    ac = 0;
    (void) XtSetArg(al[ac], DtNblinkRate, &blinkRate);			ac++;
    (void) XtGetValues(tw->termview.term, al, ac);
    *value = (XtArgVal) blinkRate;
}

static void
GetCharCursorStyle
(
    Widget		  w,
    int			  resource_offset,
    XtArgVal		 *value
)
{
    DtTermViewWidget	  tw = (DtTermViewWidget) w;
    Arg			  al[1];
    int			  ac;
    unsigned char	  charCursorStyle;

    ac = 0;
    (void) XtSetArg(al[ac], DtNcharCursorStyle, &charCursorStyle);	ac++;
    (void) XtGetValues(tw->termview.term, al, ac);
    *value = (XtArgVal) charCursorStyle;
}

static void
GetVisualBell
(
    Widget		  w,
    int			  resource_offset,
    XtArgVal		 *value
)
{
    DtTermViewWidget	  tw = (DtTermViewWidget) w;
    Arg			  al[1];
    int			  ac;
    Boolean		  visualBell;

    ac = 0;
    (void) XtSetArg(al[ac], DtNvisualBell, &visualBell);		ac++;
    (void) XtGetValues(tw->termview.term, al, ac);
    *value = (XtArgVal) visualBell;
}

static void
GetMarginBell
(
    Widget		  w,
    int			  resource_offset,
    XtArgVal		 *value
)
{
    DtTermViewWidget	  tw = (DtTermViewWidget) w;
    Arg			  al[1];
    int			  ac;
    Boolean		  marginBell;

    ac = 0;
    (void) XtSetArg(al[ac], DtNmarginBell, &marginBell);		ac++;
    (void) XtGetValues(tw->termview.term, al, ac);
    *value = (XtArgVal) marginBell;
}

static void
GetNMarginBell
(
    Widget		  w,
    int			  resource_offset,
    XtArgVal		 *value
)
{
    DtTermViewWidget	  tw = (DtTermViewWidget) w;
    Arg			  al[1];
    int			  ac;
    int			  nMarginBell;

    ac = 0;
    (void) XtSetArg(al[ac], DtNnMarginBell, &nMarginBell);		ac++;
    (void) XtGetValues(tw->termview.term, al, ac);
    *value = (XtArgVal) nMarginBell;
}

static void
GetJumpScroll
(
    Widget		  w,
    int			  resource_offset,
    XtArgVal		 *value
)
{
    DtTermViewWidget	  tw = (DtTermViewWidget) w;
    Arg			  al[1];
    int			  ac;
    Boolean		  jumpScroll;

    ac = 0;
    (void) XtSetArg(al[ac], DtNjumpScroll, &jumpScroll);		ac++;
    (void) XtGetValues(tw->termview.term, al, ac);
    *value = (XtArgVal) jumpScroll;
}

static void
GetRows
(
    Widget		  w,
    int			  resource_offset,
    XtArgVal		 *value
)
{
    DtTermViewWidget	  tw = (DtTermViewWidget) w;
    Arg			  al[1];
    int			  ac;
    short		  rows;

    ac = 0;
    (void) XtSetArg(al[ac], DtNrows, &rows);				ac++;
    (void) XtGetValues(tw->termview.term, al, ac);
    *value = (XtArgVal) rows;
}

static void
GetColumns
(
    Widget		  w,
    int			  resource_offset,
    XtArgVal		 *value
)
{
    DtTermViewWidget	  tw = (DtTermViewWidget) w;
    Arg			  al[1];
    int			  ac;
    short		  columns;

    ac = 0;
    (void) XtSetArg(al[ac], DtNcolumns, &columns);			ac++;
    (void) XtGetValues(tw->termview.term, al, ac);
    *value = (XtArgVal) columns;
}

#ifdef	NOTDEF
static XmImportOperator
SetRows
(
    Widget		  w,
    int			  resource_offset,
    XtArgVal		 *value
)
{
    DtTermViewWidget	  tw = (DtTermViewWidget) w;
    Arg			  al[1];
    int			  ac;
    short		  rows;

    rows = (* (short *) value);
    (void) XtSetArg(al[ac], DtNrows, rows);				ac++;
    (void) XtGetValues(tw->termview.term, al, ac);
    return (XmSYNTHETIC_NONE);
}

static XmImportOperator
SetColumns
(
    Widget		  w,
    int			  resource_offset,
    XtArgVal		 *value
)
{
    DtTermViewWidget	  tw = (DtTermViewWidget) w;
    Arg			  al[1];
    int			  ac;
    short		  columns;

    columns = (* (short *) value);
    (void) XtSetArg(al[ac], DtNcolumns, columns);			ac++;
    (void) XtGetValues(tw->termview.term, al, ac);
    return (XmSYNTHETIC_NONE);
}
#endif	/* NOTDEF */

/* the resource list for DtTerm... */
static XmSyntheticResource synResources[] =
{
    {
	DtNblinkRate, sizeof(int),
	XtOffsetOf(struct _DtTermViewRec, termview.blinkRate),
	GetBlinkRate, NULL
    },
    {
	DtNcharCursorStyle, sizeof(unsigned char),
	XtOffsetOf(struct _DtTermViewRec, termview.charCursorStyle),
	GetCharCursorStyle, NULL
    },
    {
	DtNvisualBell, sizeof(Boolean),
	XtOffsetOf(struct _DtTermViewRec, termview.visualBell),
	GetVisualBell, NULL
    },
    {
	DtNmarginBell, sizeof(Boolean),
	XtOffsetOf(struct _DtTermViewRec, termview.marginBell),
	GetMarginBell, NULL
    },
    {
	DtNnMarginBell, sizeof(int),
	XtOffsetOf(struct _DtTermViewRec, termview.nMarginBell),
	GetNMarginBell, NULL
    },
    {
	DtNjumpScroll, sizeof(Boolean),
	XtOffsetOf(struct _DtTermViewRec, termview.jumpScroll),
	GetJumpScroll, NULL
    },
    {
	DtNrows, sizeof(short),
	XtOffsetOf(struct _DtTermViewRec, termview.rows),
	GetRows, /*SetRows*/ NULL
    },
    {
	DtNcolumns, sizeof(short),
	XtOffsetOf(struct _DtTermViewRec, termview.columns),
	GetColumns, /*SetColumns*/ NULL
    },
};


/* the resource list for dtterm... */
static XtResource resources[] =
{
    {
	DtNemulationId, DtCEmulationId, XmRString, sizeof(char *),
	XtOffsetOf( struct _DtTermViewRec, termview.emulationId),
	XtRImmediate, (XtPointer) "dtterm"
    },
    {
	DtNcharCursorStyle, DtCCharCursorStyle, DtRDtTermCharCursorStyle,
	sizeof(unsigned char),
	XtOffsetOf(struct _DtTermViewRec, termview.charCursorStyle),
	XmRImmediate, (XtPointer) DtTERM_CHAR_CURSOR_BOX
    },
    {
	DtNblinkRate, DtCBlinkRate, XmRInt, sizeof(int),
	XtOffsetOf(struct _DtTermViewRec, termview.blinkRate),
	XmRImmediate, (XtPointer) 250
    },
    {
	DtNvisualBell, DtCVisualBell, XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(struct _DtTermViewRec, termview.visualBell),
	XmRImmediate, (XtPointer) False
    },
    {
	DtNmarginBell, DtCMarginBell, XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(struct _DtTermViewRec, termview.marginBell),
	XmRImmediate, (XtPointer) False
    },
    {
	DtNnMarginBell, DtCNMarginBell, XmRInt,
	sizeof(int),
	XtOffsetOf(struct _DtTermViewRec, termview.nMarginBell),
	XmRImmediate, (XtPointer) 10
    },
    {
	DtNjumpScroll, DtCJumpScroll, XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(struct _DtTermViewRec, termview.jumpScroll),
	XmRImmediate, (XtPointer) True
    },
    {
	DtNbaseWidth, DtCBaseWidth, XmRInt, sizeof(int),
	XtOffsetOf(struct _DtTermViewRec, termview.baseWidth),
	XmRImmediate, (XtPointer) 0
    },
    {
	DtNbaseHeight, DtCBaseHeight, XmRInt, sizeof(int),
	XtOffsetOf(struct _DtTermViewRec, termview.baseHeight),
	XmRImmediate, (XtPointer) 0
    },
    {
	DtNwidthInc, DtCWidthInc, XmRInt, sizeof(int),
	XtOffsetOf(struct _DtTermViewRec, termview.widthInc),
	XmRImmediate, (XtPointer) 0
    },
    {
	DtNheightInc, DtCHeightInc, XmRInt, sizeof(int),
	XtOffsetOf(struct _DtTermViewRec, termview.heightInc),
	XmRImmediate, (XtPointer) 0
    },
    {
	DtNtermDevice, DtCTermDevice, XmRInt, sizeof(int),
	XtOffsetOf(struct _DtTermViewRec, termview.pty),
	XmRImmediate, (XtPointer) -1
    },
    {
	DtNtermDeviceAllocate, DtCTermDeviceAllocate, XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(struct _DtTermViewRec, termview.ptyAllocate),
	XmRImmediate, (XtPointer) True
    },
    {
	DtNtermSlaveName, DtCTermSlaveName, XmRString, sizeof(char *),
	XtOffsetOf(struct _DtTermViewRec, termview.ptySlaveName),
	XmRImmediate, (XtPointer) NULL
    },
    {
	DtNrows, DtCRows, XmRShort, sizeof(short),
	XtOffsetOf(struct _DtTermViewRec, termview.rows),
	XmRImmediate, (XtPointer) 0
    },
    {
	DtNcolumns, DtCColumns, XmRShort, sizeof(short),
	XtOffsetOf(struct _DtTermViewRec, termview.columns),
	XmRImmediate, (XtPointer) 0
    },
    {
	XmNtraversalOn, XmCTraversalOn, XmRBoolean, sizeof(Boolean),
	XtOffsetOf(struct _DtTermViewRec, manager.traversal_on),
	XtRImmediate, (XtPointer) True
    },
    {
	DtNshadowType, DtCShadowType, XmRShadowType, sizeof (unsigned char),
	XtOffsetOf( struct _DtTermViewRec, termview.shadowType),
	XmRImmediate, (XtPointer) DtSHADOW_OUT
    },
    {
	XmNshadowThickness, XmCShadowThickness, XmRHorizontalDimension,
	sizeof (Dimension),
	XtOffsetOf( struct _DtTermViewRec, manager.shadow_thickness),
	XmRImmediate, (XtPointer) 1
    },
    {
	DtNmarginWidth, DtCMarginWidth, XmRHorizontalDimension,
	sizeof (Dimension),
	XtOffsetOf( struct _DtTermViewRec, termview.marginWidth),
	XmRImmediate, (XtPointer) 7
    },
    {
	DtNmarginHeight, DtCMarginHeight, XmRVerticalDimension,
	sizeof (Dimension),
	XtOffsetOf( struct _DtTermViewRec, termview.marginHeight),
	XmRImmediate, (XtPointer) 7
    },
    {
	DtNspacing, DtCSpacing, XmRVerticalDimension,
	sizeof (Dimension),
	XtOffsetOf( struct _DtTermViewRec, termview.spacing),
	XmRImmediate, (XtPointer) 4
    },
    {
	DtNloginShell, DtCLoginShell, XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf( struct _DtTermViewRec, termview.subprocessLoginShell),
	XtRImmediate, (XtPointer) False
    },
    {
	DtNsubprocessTerminationCallback, DtCCallback, XmRCallback,
	sizeof(XtCallbackList),
	XtOffsetOf( struct _DtTermViewRec, termview.subprocessTerminationCallback),
	XmRPointer, (XtPointer) NULL
    },
    {
	DtNnewCallback, DtCCallback, XmRCallback,
	sizeof(XtCallbackList),
	XtOffsetOf( struct _DtTermViewRec, termview.newCallback),
	XmRPointer, (XtPointer) NULL
    },
    {
	DtNsubprocessPid, DtCSubprocessPid, XmRInt, sizeof(int),
	XtOffsetOf( struct _DtTermViewRec, termview.subprocessPid),
	XtRImmediate, (XtPointer) -1
    },
    {
	DtNsubprocessExec, DtCSubprocessExec, XmRBoolean, sizeof(Boolean),
	XtOffsetOf( struct _DtTermViewRec, termview.subprocessExec),
	XmRImmediate, (XtPointer) True
    },
    {
	DtNsubprocessTerminationCatch, DtCSubprocessTerminationCatch,
	XmRBoolean, sizeof(Boolean),
	XtOffsetOf( struct _DtTermViewRec, termview.subprocessTerminationCatch),
	XmRImmediate, (XtPointer) True
    },
    {
	DtNsubprocessCmd, DtCSubprocessCmd, XmRString, sizeof(char *),
	XtOffsetOf( struct _DtTermViewRec, termview.subprocessCmd),
	XtRImmediate, (XtPointer) NULL
    },
    {
	DtNsubprocessArgv, DtCSubprocessArgv, XtRStringArray, sizeof(String *),
	XtOffsetOf( struct _DtTermViewRec, termview.subprocessArgv),
	XmRImmediate, (XtPointer) NULL
    },
    {
	DtNscrollBar, DtCScrollBar, XmRBoolean, sizeof(Boolean),
	XtOffsetOf( struct _DtTermViewRec, termview.scrollBarVisible),
	XmRImmediate, (XtPointer) True
    },
    {
	DtNmenuBar, DtCMenuBar, XmRBoolean, sizeof(Boolean),
	XtOffsetOf( struct _DtTermViewRec, termview.menuBarVisible),
	XmRImmediate, (XtPointer) True
    },
    {
	DtNmenuPopup, DtCMenuPopup, XmRBoolean, sizeof(Boolean),
	XtOffsetOf( struct _DtTermViewRec, termview.menuPopupVisible),
	XmRImmediate, (XtPointer) True
    },
    {
	DtNsizeList, DtCSizeList, DtRDtTermTerminalSizeList,
	sizeof(DtTermTerminalSizeList),
	XtOffsetOf( struct _DtTermViewRec, termview.sizeList),
	XmRString, (XtPointer) "80x24 132x24"
    },
    {
	DtNuserFont, DtCUserFont, XmRFontList,
	sizeof(XmFontList),
	XtOffsetOf( struct _DtTermViewRec, termview.fontList),
	XmRImmediate, (XtPointer) NULL
    },
    {
	DtNuserFontList, DtCUserFontList, XmRString, sizeof(char *),
	XtOffsetOf( struct _DtTermViewRec, termview.userFontList),
	XmRImmediate, (XtPointer) _DtTermViewMenuDefaultFonts
    },
};

/* global class record for instances of class: DtTermView
 */

externaldef(termclassrec) DtTermViewClassRec dtTermViewClassRec =
{
    /* core class record */
    {
	/* superclass		*/	(WidgetClass) &xmBulletinBoardClassRec,
	/* class_name		*/	"DtTermView",
	/* widget_size		*/	sizeof(DtTermViewRec),
	/* class_initialize	*/	ClassInitialize,
	/* class_part_init	*/	ClassPartInitialize,
	/* class_inited		*/	FALSE,
	/* initialize		*/	Initialize,
	/* initialize_hook	*/	(XtArgsProc) NULL,
	/* realize		*/	Realize,
	/* actions		*/	NULL, /*actionsList,*/
	/* num_actions		*/	0, /*XtNumber(actionsList),*/
	/* resources		*/	resources,
	/* num_resources	*/	XtNumber(resources),
	/* xrm_class		*/	NULLQUARK,
	/* compress_motion	*/	TRUE,
	/* compress_exposure	*/	FALSE,
	/* compress_enterlv	*/	TRUE,
	/* visible_interest	*/	TRUE,
	/* destroy		*/	Destroy,
	/* resize		*/	Resize,
	/* expose		*/	Redisplay,
	/* set_values		*/	SetValues,
	/* set_values_hook	*/	(XtArgsFunc) NULL,
	/* set_values_almost	*/	XtInheritSetValuesAlmost,
	/* get_values_hook	*/	(XtArgsProc) NULL,
	/* accept_focus		*/	(XtAcceptFocusProc) NULL,
	/* version		*/	XtVersion,
	/* callback_private	*/	(XtPointer) NULL,
	/* tm_table		*/	XtInheritTranslations,
	/* query_geometry	*/	(XtGeometryHandler) NULL,
	/* display_accelerator	*/	(XtStringProc) NULL,
	/* extension		*/	(XtPointer) NULL,
    },

    /* composite class rec */
    {
	/* geometry_manager		*/ GeometryManager,
	/* change_managed		*/ ChangeManaged,
	/* insert_child			*/ XtInheritInsertChild,
	/* delete_child			*/ XtInheritDeleteChild,
	/* Extension			*/ NULL,
    },

    /* constraint class Init */
    {
	NULL,
	0,
	0,
	NULL,
	NULL,
	NULL,
	NULL,
    },

    /* manager class */
    {
	/* translations			*/ XtInheritTranslations,
	/* get_resources		*/ synResources,
	/* num_get_resources		*/ XtNumber(synResources),
	/* get_cont_resources		*/ NULL,
	/* num_get_cont_resources	*/ 0,
	/* parent_process		*/ XmInheritParentProcess,
	/* extension			*/ NULL,
    },

    /* bulletin board class */
    {
        /* always_install_accelerators  */ FALSE,
        /* geom_matrix_create           */ (XmGeoCreateProc) NULL,
        /* focus_moved_proc             */ XmInheritFocusMovedProc,
        /* extension                    */ NULL,
    },

    /* termview class record */
    {
	0
    }
};

externaldef(dttermviewwidgetclass) WidgetClass dtTermViewWidgetClass =
	(WidgetClass)&dtTermViewClassRec;

/* Class Initialize...
 */
static void
ClassInitialize(void)
{
    /* register our resource converters... */
    (void) _DtTermPrimInitRepTypes();

    return;
}

/*ARGSUSED*/
static void
ClassPartInitialize(WidgetClass w_class)
{
}

static Boolean
SetSizeInfo(Widget w, int width, int height)
{
    DtTermViewWidget tw = (DtTermViewWidget) w;
    Arg arglist[20];
    Boolean change = False;
    int i;

    short oldRows = tw->termview.rows;
    short oldColumns = tw->termview.columns;
    int oldWidthInc = tw->termview.widthInc;
    int oldHeightInc = tw->termview.heightInc;
    int oldBaseWidth = tw->termview.baseWidth;
    int oldBaseHeight = tw->termview.baseHeight;
    Dimension shellHeight;

#ifdef	THIS_BREAKS_DASH_GEOMETRY
    if (!XtIsRealized(tw->termview.term)) {
	return;
    }
#endif	/* THIS_BREAKS_DASH_GEOMETRY */

    if (width <= 0)
	width = tw->core.width;
    if (height <= 0)
	height = tw->core.height;

    if ((width <= 0) || (height <= 0))
	return(change);

    /* get the widthInc, heightInc, rows, and cols...
     */
    i = 0;
    (void) XtSetArg(arglist[i], DtNrows, &tw->termview.rows); i++;
    (void) XtSetArg(arglist[i], DtNcolumns, &tw->termview.columns); i++;
    (void) XtSetArg(arglist[i], DtNwidthInc, &tw->termview.widthInc); i++;
    (void) XtSetArg(arglist[i], DtNheightInc, &tw->termview.heightInc); i++;
    (void) XtGetValues(tw->termview.term, arglist, i);
    /* validate the data... */
    if (tw->termview.rows < 1)
	tw->termview.rows = 1;
    if (tw->termview.columns < 1)
	tw->termview.columns = 1;
    if (tw->termview.widthInc < 1)
	tw->termview.widthInc = 1;
    if (tw->termview.heightInc < 1)
	tw->termview.heightInc = 1;

    /* from the rows, columns, heightInc and widthInc we can calculate
     * our base width and base height...
     */
    if (width > 0) {
	tw->termview.baseWidth = width -
		tw->termview.columns * tw->termview.widthInc;
    }
    /* sanity check on baseWidth... */
    if ((width <= 0) || (tw->termview.baseWidth < 0)) {
	tw->termview.baseWidth = 0;
    }

    if (XtIsShell(XtParent(w))) {
	i = 0;
	(void) XtSetArg(arglist[i], XmNheight, &shellHeight); i++;
	XtGetValues(XtParent(w),arglist,i);
	if (shellHeight > 0) {
	    tw->termview.baseHeight = shellHeight -
		tw->termview.rows * tw->termview.heightInc;
	}
	/* sanity check on baseHeight... */
	if ((shellHeight <= 0) || (tw->termview.baseHeight < 0)) {
	    tw->termview.baseHeight = 0;
	}
    } else {
      	if (height > 0) {
	    tw->termview.baseHeight = height -
		tw->termview.rows * tw->termview.heightInc;
    	}
        /* sanity check on baseHeight... */
    	if ((height <= 0) || (tw->termview.baseHeight < 0)) {
	    tw->termview.baseHeight = 0;
    	}
    }

    if ((oldRows != tw->termview.rows) ||
	    (oldColumns != tw->termview.columns) ||
	    (oldWidthInc != tw->termview.widthInc) ||
	    (oldHeightInc != tw->termview.heightInc) ||
	    (oldBaseWidth != tw->termview.baseWidth) ||
	    (oldBaseHeight != tw->termview.baseHeight))
	change = True;

    if (change && XtIsShell(XtParent(w))) {
	i = 0;
	(void) XtSetArg(arglist[i], DtNwidthInc, tw->termview.widthInc); i++;
	(void) XtSetArg(arglist[i], DtNheightInc, tw->termview.heightInc); i++;
	(void) XtSetArg(arglist[i], DtNbaseWidth, tw->termview.baseWidth); i++;
	(void) XtSetArg(arglist[i], DtNbaseHeight, tw->termview.baseHeight); i++;
	(void) XtSetValues(XtParent(w), arglist, i);
    }

    return(change);
}

static Boolean
ResetSizeInfo(Widget w, int width, int height)
{
    DtTermViewWidget tw = (DtTermViewWidget) w;
    Arg arglist[20];
    Boolean change = False;
    int i;
    int oldWidthInc = tw->termview.widthInc;
    int oldHeightInc = tw->termview.heightInc;
    int oldBaseWidth = tw->termview.baseWidth;
    int oldBaseHeight = tw->termview.baseHeight;
    Dimension shellHeight;

    if (width <= 0)
	width = tw->core.width;
    if (height <= 0)
	height = tw->core.height;

    if ((width <= 0) || (height <= 0))
	return(change);

    /* get the widthInc, heightInc...
     */
    i = 0;
    (void) XtSetArg(arglist[i], DtNwidthInc, &tw->termview.widthInc); i++;
    (void) XtSetArg(arglist[i], DtNheightInc, &tw->termview.heightInc); i++;
    (void) XtGetValues(tw->termview.term, arglist, i);
    /* validate the data... */
    if (tw->termview.widthInc < 1)
	tw->termview.widthInc = 1;
    if (tw->termview.heightInc < 1)
	tw->termview.heightInc = 1;

    /* from the rows, columns, heightInc and widthInc we can calculate
     * our base width and base height...
     */
    if (width > 0) {
	tw->termview.baseWidth = width % tw->termview.widthInc;
    }
    /* sanity check on baseWidth... */
    if ((width <= 0) || (tw->termview.baseWidth < 0)) {
	tw->termview.baseWidth = 0;
    }

    if (height > 0) {
    	tw->termview.baseHeight = height % tw->termview.heightInc;
    }
    /* sanity check on baseHeight... */
    if ((height <= 0) || (tw->termview.baseHeight < 0)) {
    	tw->termview.baseHeight = 0;
    }
    if (XtIsShell(XtParent(w))) {
        i = 0;
        (void) XtSetArg(arglist[i], XmNheight, &shellHeight); i++;
        XtGetValues(XtParent(w),arglist,i);
	tw->termview.baseHeight += shellHeight - tw->core.height;
    }
    if ((oldWidthInc != tw->termview.widthInc) ||
	    (oldHeightInc != tw->termview.heightInc) ||
	    (oldBaseWidth != tw->termview.baseWidth) ||
	    (oldBaseHeight != tw->termview.baseHeight))
	change = True;

    if (change && XtIsShell(XtParent(w))) {
	i = 0;
	(void) XtSetArg(arglist[i], DtNwidthInc, tw->termview.widthInc); i++;
	(void) XtSetArg(arglist[i], DtNheightInc, tw->termview.heightInc); i++;
	(void) XtSetArg(arglist[i], DtNbaseWidth, tw->termview.baseWidth); i++;
	(void) XtSetArg(arglist[i], DtNbaseHeight, tw->termview.baseHeight);
		i++;
	(void) XtSetValues(XtParent(w), arglist, i);
    }

    return(change);
}


/* Initialize...
 */

/*ARGSUSED*/
static void
Initialize(Widget rw, Widget w, Arg *args, Cardinal *num_args)
{
    DtTermViewWidget tw = (DtTermViewWidget) w;
    int i;
    Arg arglist[20];
    XmRepTypeId shadowTypeID;

    Debug('T', timeStamp("DtTermView Initialize() starting"));

    /* we need to insure that everthing is initialized.  Let's do a
     * checklist...
     */
    /* Widget term			created below			*/
    /* Widget scrolledWindowFrame	created below			*/
    /* Widget scrolledWindow		created below			*/
    /* widgetInfo scrollBar		created below or set to null	*/
    /* widgetInfo menuBar		created below or set to null	*/
    /* Widget helpWidget */
    tw->termview.helpWidget = (Widget) 0;

    /* Widget globalOptionsDialog */
    tw->termview.globalOptionsDialog = (Widget) 0;

    /* Widget terminalOptionsDialog */
    tw->termview.terminalOptionsDialog = (Widget) 0;

    /* char emulationId			set via setvalues		*/
    /* unsigned char charCursorStyle	set via setvalues		*/
    /* int blinkRate			set via setvalues		*/
    /* Boolean visualBell		set via setvalues		*/
    /* Boolean marginBell		set via setvalues		*/
    /* int nMarginBell			set via setvalues		*/
    /* unsigned char shadowType		set via setvalues		*/
    shadowTypeID = XmRepTypeGetId(XmRShadowType);
    if (shadowTypeID != XmREP_TYPE_INVALID) {
	if (!XmRepTypeValidValue(shadowTypeID, tw->termview.shadowType,
	    (Widget) tw))
	tw->termview.shadowType = DtSHADOW_OUT;
    }

    /* XmFontList fontList		fetched from Term below		*/
    /* XmFontList defaultFontList */
    tw->termview.defaultFontList = (XmFontList) 0;

    /* short rows			set via setvalues		*/
    /* short columns			set via setvalues		*/

    if ((tw->termview.rows <= 0) || (tw->termview.columns <= 0)) {
	if (XtIsShell(XtParent(w))) {
	    char *geometry;
	    XSizeHints hints;
	    int flags;
	    int x;
	    int y;
	    int width;
	    int height;
	    int winGravity;

	    i = 0;
	    (void) XtSetArg(arglist[i], XmNgeometry, &geometry); i++;
	    (void) XtGetValues(XtParent(w), arglist, i);

	    if (geometry && *geometry) {
		hints.flags = 0;

		flags = XWMGeometry(XtDisplay(w),
			XScreenNumberOfScreen(XtScreen(w)),
			geometry,
			"80x24",
			0,
			&hints,
			&x,
			&y,
			&width,
			&height,
			&winGravity);
		if (tw->termview.rows <= 0) {
		    tw->termview.rows = height;
		}
		if (tw->termview.columns <= 0) {
		    tw->termview.columns = width;
		}
	    }
	}

	if (tw->termview.rows <= 0) {
	    tw->termview.rows = 24;
	}
	if (tw->termview.columns <= 0) {
	    tw->termview.columns = 80;
	}
    }

    /* Dimension marginHeight		set via setvalues		*/
    /* Dimension marginWidth		set via setvalues		*/
    /* Dimension spacing		set via setvalues		*/
    /* int baseWidth			computed by termview		*/
    /* int baseHeight			computed by termview		*/
    /* int widthInc			set below based on term values	*/
    /* int heightInc			set below based on term values	*/
    /* int pty				set via setvalues or below	*/
    /* char *ptySlaveName		set via setvalues or below	*/
    /* int subprocessPid		DKS: not yet set below		*/
    /* char *subprocessCmd		set via setvalues		*/
    /* char **subprocessArgv		set via setvalues		*/
    /* Boolean subprocessLoginShell	set via setvalues		*/
    /* Boolean subprocessTerminationCatch
					set via setvalues		*/
    /* Boolean subprocessExec		set via setvalues		*/
    /* Boolean scrollBarVisible		set via setvalues		*/
    /* Boolean menuBarVisible		set via setvalues		*/
    /* Boolean capsLock */
    tw->termview.capsLock = False;

    /* Boolean stop */
    tw->termview.stop = False;

    /* DtTermInsertCharMode insertCharMode */
    tw->termview.insertCharMode = DtTERM_INSERT_CHAR_OFF;

    /* XtCallbackList subprocessTerminationCallback
					set via setvalues		*/
    /* XtCallbackList newCallback	set via setvalues		*/

    /* DtTermTerminalSize sizeDefault */
    tw->termview.sizeDefault.rows = 0;
    tw->termview.sizeDefault.columns = 0;

    /* DtTermTerminalSizeList sizeList	set via setvalues		*/

    /* char *userFontName */
    tw->termview.userFontName = NULL;

    /* int currentFontToggleButtonIndex */
    tw->termview.currentFontToggleButtonIndex = -1;

    /* Boolean sized */
    tw->termview.sized = False;

    /* and from Manager... */
    /* Dimension manager.shadow_thickness */
    /* BulletinBoard was really nice to us and set this to 1 if it
     * was originally 0 and we are a top level shell.  Let's reset it
     * to its old value...
     */
    tw->manager.shadow_thickness =
	    ((DtTermViewWidget) (rw))->manager.shadow_thickness;

    /* create the MenuBar...
     */
    if (tw->termview.menuBarVisible) {
	Debug('T',
		timeStamp( "DtTermView Initialize() menubar create starting"));

	i = 0;
	tw->termview.menuBar.widget =
		_DtTermViewCreatePulldownMenu(w, w, arglist, i);

	Debug('T',
		timeStamp( "DtTermView Initialize() managing the menubar"));

	(void) XtManageChild(tw->termview.menuBar.widget);
	tw->termview.menuBar.managed = True;

	Debug('T',
		timeStamp("DtTermView Initialize() menubar create finished"));
    } else {
	tw->termview.menuBar.widget = (Widget) 0;
	tw->termview.menuBar.managed = False;
    }

    Debug('T', timeStamp(
	    "DtTermView Initialize() scrolledWindowFrame create starting"));
    i = 0;
    tw->termview.scrolledWindowFrame =
	    XtCreateManagedWidget("dtTermScrolledWindowFrame",
	    xmFrameWidgetClass, w, arglist, i);
    Debug('T', timeStamp(
	    "DtTermView Initialize() scrolledWindowFrame create finished"));

    Debug('T', timeStamp(
	    "DtTermView Initialize() scrolledWindow create starting"));
    /* create the ScrolledWindow...
     */
    i = 0;
    (void) XtSetArg(arglist[i], XmNscrollingPolicy, XmAPPLICATION_DEFINED); i++;
    (void) XtSetArg(arglist[i], XmNvisualPolicy, XmVARIABLE); i++;
    (void) XtSetArg(arglist[i], XmNscrollBarDisplayPolicy, XmSTATIC); i++;
    tw->termview.scrolledWindow =
	    XtCreateManagedWidget("dtTermScrolledWindow",
	    xmScrolledWindowWidgetClass,
	    tw->termview.scrolledWindowFrame, arglist, i);

    Debug('T', timeStamp(
	    "DtTermView Initialize() scrolledWindow create finished"));

    /* create the Term widget...
     */
    i = 0;
    /* Checklist.  Let's make sure we are setting everthing that term
     * needs...
     */

    /* widgetInfo menuBar		not needed			*/
    /* Widget scrolledWindowFrame	not needed			*/
    /* Widget scrolledWindow		not needed			*/
    /* widgetInfo scrollBar		not needed			*/
    /* Widget term			not needed			*/
    /* char *emulationId						*/
    (void) XtSetArg(arglist[i], DtNemulationId,
	    tw->termview.emulationId); i++;

    /* unsigned char charCursorStyle					*/
    (void) XtSetArg(arglist[i], DtNcharCursorStyle,
	    tw->termview.charCursorStyle); i++;

    /* int blinkRate							*/
    (void) XtSetArg(arglist[i], DtNblinkRate, tw->termview.blinkRate); i++;

    /* Boolean visualBell		set via setvalues		*/
    (void) XtSetArg(arglist[i], DtNvisualBell, tw->termview.visualBell); i++;

    /* Boolean marginBell		set via setvalues		*/
    (void) XtSetArg(arglist[i], DtNmarginBell, tw->termview.marginBell); i++;

    /* Boolean nMarginBell		set via setvalues		*/
    (void) XtSetArg(arglist[i], DtNnMarginBell, tw->termview.nMarginBell);
	    i++;

    /* Boolean jumpScroll		set via setvalues		*/
    (void) XtSetArg(arglist[i], DtNjumpScroll, tw->termview.jumpScroll); i++;

    /* unsigned char shadowType		defaulted by child		*/
    /* XmFontList fontList		fetched from Term below		*/
    /* short rows							*/
    if (tw->termview.rows > 0) {
	(void) XtSetArg(arglist[i], DtNrows, tw->termview.rows); i++;
    }

    /* short columns							*/
    if (tw->termview.columns > 0) {
	(void) XtSetArg(arglist[i], DtNcolumns, tw->termview.columns); i++;
    }

    /* Dimension marginHeight		applies to DtTermView only	*/
    /* Dimension marginWidth		applies to DtTermView only	*/
    /* Dimension spacing		applies to DtTermView only	*/
    /* int baseWidth			computed by DtTermView and xterm*/
    /* int baseHeight			computed by DtTermView and xterm*/
    /* int widthInc			fetched from Term		*/
    /* int heightInc			fetched from Term		*/
    /* int pty								*/
    if (tw->termview.pty >= 0) {
	(void) XtSetArg(arglist[i], DtNtermDevice, tw->termview.pty); i++;
    }

    /* Boolean ptyAllocate						*/
    (void) XtSetArg(arglist[i], DtNtermDeviceAllocate,
	    tw->termview.ptyAllocate); i++;

    /* char *ptySlaveName						*/
    if (tw->termview.ptySlaveName) {
	(void) XtSetArg(arglist[i], DtNtermSlaveName,
		tw->termview.ptySlaveName); i++;
    }

    /* int subprocessPid		fetched from Term		*/
    /* char *subprocessCmd						*/
    if (tw->termview.subprocessCmd) {
	(void) XtSetArg(arglist[i], DtNsubprocessCmd,
		tw->termview.subprocessCmd); i++;
    }

    /* char **subprocessArgv						*/
    if (tw->termview.subprocessArgv) {
	(void) XtSetArg(arglist[i], DtNsubprocessArgv,
		tw->termview.subprocessArgv); i++;
    }

    /* Boolean subprocessLoginShell					*/
    (void) XtSetArg(arglist[i], DtNsubprocessLoginShell,
	    tw->termview.subprocessLoginShell); i++;

    /* Boolean subprocessTerminationCatch				*/
    (void) XtSetArg(arglist[i], DtNsubprocessTerminationCatch,
	    tw->termview.subprocessTerminationCatch); i++;

    /* Boolean subprocessExec						*/
    (void) XtSetArg(arglist[i], DtNsubprocessExec,
	    tw->termview.subprocessExec); i++;

    /* Boolean scrollBarVisible		applies to TermView only	*/
    /* Boolean menuBarVisible		applies to TermView only	*/
    /* XtCallbackList subprocessTerminationCallback
     *					called by Term's callback
     *					(see below)
     */

    Debug('T', timeStamp("DtTermView Initialize() calling DtCreateTerm()"));
    tw->termview.term = DtCreateTerm(tw->termview.scrolledWindow,
	    "dtTerm", arglist, i);
    Debug('T', timeStamp("DtTermView Initialize() DtCreateTerm() finished"));
    XtManageChild(tw->termview.term);
    /* have the term's subprocessTerminationCallback call us and we will
     * kick off our callback list...
     */
    Debug('T', timeStamp("DtTermView Initialize() DtTerm widget managed"));

    (void) XtAddCallback(tw->termview.term, DtNsubprocessTerminationCallback,
	    SubprocessTerminationCallback, tw);

    /* create the popup menu...
     */
    if (tw->termview.menuPopupVisible) {
	Debug('T',
		timeStamp("DtTermView Initialize() popupmenu create started"));
	i = 0;
	(void) _DtTermViewCreatePopupMenu(w, tw->termview.term, arglist, i);
	Debug('T',
		timeStamp("DtTermView Initialize() popupmenu create finished"));
    }

#ifdef	THIS_BREAKS_DASH_GEOMETRY
    /* fill in rows, columns, widthInc, heightInc, baseWidth, baseHeight... */
    (void) SetSizeInfo(w, -1, -1);
#endif	/* THIS_BREAKS_DASH_GEOMETRY */

    /* get our fontList from the term widget... */
    i = 0;
    (void) XtSetArg(arglist[i], DtNuserFont, &tw->termview.fontList);
	    i++;
    (void) XtGetValues(tw->termview.term, arglist, i);
    tw->termview.defaultFontList = tw->termview.fontList;

    /* create the scrollBar...
     */
    if (tw->termview.scrollBarVisible) {
	Debug('T', timeStamp(
		"DtTermView Initialize() scrollBar create starting"));
	(void) CreateScrollBar(w);
	Debug('T', timeStamp(
		"DtTermView Initialize() scrollBar create finished"));
    } else {
	tw->termview.scrollBar.widget = (Widget) 0;
	tw->termview.scrollBar.managed = False;
    }

    /* create the function key form...
     */

    i = 0;
    /* Add an event handler for enter and leave events to allow us to
     * modify the "CAPS LOCK" indicator...
     */
    (void) XtAddEventHandler(w,
	    (EventMask) EnterWindowMask | LeaveWindowMask,
	    False, handleEnterLeaveEvents, (Opaque) NULL);

    /* Add a substructure notify handler so that the first time we are
     * mapped, we can save away our default size...
     */
    {
	Widget sw;

	for (sw = w; !XtIsShell(sw); sw = XtParent(sw))
	    ;
	(void) XtAddEventHandler(sw, (EventMask) StructureNotifyMask,
		False, handleProcessStructureNotifyEvent, (Opaque) w);
    }

    Debug('T', timeStamp("DtTermView Initialize() finished"));
    return;
}

static void
Resize(Widget w)
{
    DtTermViewWidget tw = (DtTermViewWidget) w;
    Dimension menuBarHeight = 0;
    int configWidth;
    int configHeight;

    if (!XtIsRealized(w)) {
	return;
    }

    /* We have 4 widgets to worry about:
     *	-the scrolled window that holds the term widget,
     *	-the menu
     *  -the status line
     *
     * The menubar will be left at whatever height it was.
     * The scrolled window (the term widget) will be given whatever
     * is left.
     */

    /* update all our managed flags... */
    if (tw->termview.menuBar.widget)
	tw->termview.menuBar.managed = XtIsManaged(tw->termview.menuBar.widget);
    if (tw->termview.scrollBar.widget)
	tw->termview.scrollBar.managed = XtIsManaged(tw->termview.scrollBar.widget);

    /* the term window will start at 0,0 and be the width and height of
     * our window except:
     *
     *	- we will subtract the height of the menubar,
     *	- we will change y to put the window below the menubar.
     */

    /* the menu bar gets the width of the window.  We will query the
     * menu bar to see what it thinks it's height should be.
     */
    if (tw->termview.menuBar.widget && (tw->termview.menuBar.managed =
	    XtIsManaged(tw->termview.menuBar.widget))) {
	XtWidgetGeometry desired;
	XtWidgetGeometry preferred;

	desired.x = 0;
	desired.y = 0;
	desired.border_width = tw->termview.menuBar.widget->core.border_width;
	desired.width = tw->core.width -
		2 * tw->termview.menuBar.widget->core.border_width;
	desired.height = tw->termview.menuBar.widget->core.height;
	desired.request_mode = CWWidth;
	if (XtQueryGeometry(tw->termview.menuBar.widget, &desired, &preferred) !=
		XtGeometryYes) {
	    /* set the border width and the height to that returned by
	     * the widget XtQueryGeometry() request...
	     */
	    desired.height = preferred.height;
	    desired.border_width = preferred.border_width;
	}
	
	/* change the width of the menubar to the width of the window... */
	(void) XtResizeWidget(tw->termview.menuBar.widget,
		desired.width,
		desired.height,
		desired.border_width);

	menuBarHeight = widgetHeight(tw->termview.menuBar.widget);
    }

    /* now, the terminal widget gets the height that is left and
     * the full width (i.e., goes from the left border to the right
     * border)...
     */
    configWidth = ourWidth(tw) - 2 *
	    tw->termview.scrolledWindowFrame->core.border_width;
    configHeight = ourHeight(tw) - 2 *
	    tw->termview.scrolledWindowFrame->core.border_width -
	    menuBarHeight;
    if (configWidth <= 0) {
	configWidth = 1;
    }
    if (configHeight <= 0) {
	configHeight = 1;
    }

    (void) XtConfigureWidget(tw->termview.scrolledWindowFrame,
						/* Widget		*/
	    ourWidthOffset(tw),			/* x			*/
	    ourHeightOffset(tw) + menuBarHeight,
						/* y			*/
	    (Dimension) configWidth,		/* width		*/
	    (Dimension) configHeight,		/* height		*/
	    tw->termview.scrolledWindowFrame->core.border_width);
						/* border width		*/

    /* update our rows, columns, widthInc, heightInc, baseWidth,
     * baseHeight...
     */
    (void) SetSizeInfo(w, -1, -1);

    /* we have been sized... */
    tw->termview.sized = True;

    return;
}

/*ARGSUSED*/
static void
Redisplay(Widget w, XEvent *event, Region region)
{
    DtTermViewWidget tw = (DtTermViewWidget) w;

    if (XtIsRealized(w)) {
	/* before we do anything, if this is our first time, we need to
	 * be sized.
	 */
	if (!tw->termview.sized) {
	    Resize(w);
	}

	(void) XClearArea(XtDisplay(w), XtWindow(w), 0, 0, tw->core.width,
		tw->core.height, False);
	if (tw->termview.menuBar.managed) {
	    (void) XmeDrawShadows(XtDisplay(w), XtWindow(w),
		    tw->manager.top_shadow_GC,
		    tw->manager.bottom_shadow_GC,
		    0,
		    widgetHeight(tw->termview.menuBar.widget),
		    tw->core.width,
		    tw->core.height - widgetHeight(tw->termview.menuBar.widget),
		    tw->manager.shadow_thickness,
		    tw->termview.shadowType);
	} else {
	    (void) XmeDrawShadows(XtDisplay(w), XtWindow(w),
		    tw->manager.top_shadow_GC,
		    tw->manager.bottom_shadow_GC,
		    0,
		    0,
		    tw->core.width,
		    tw->core.height,
		    tw->manager.shadow_thickness,
		    tw->termview.shadowType);
	}
    }
}

/* SetValues...
 */
/*ARGSUSED*/
static Boolean
SetValues(Widget cur_w, Widget ref_w, Widget w, ArgList args,
	Cardinal *num_args)
{
    DtTermViewWidget tw = (DtTermViewWidget) w;
    DtTermViewWidget cur_tw = (DtTermViewWidget) cur_w;
    Boolean flag = False;	/* return value... */
    Arg al[20];
    int ac = 0;

    /* check scrollbar visible... */
    if (tw->termview.scrollBarVisible != cur_tw->termview.scrollBarVisible) {
	if (tw->termview.scrollBarVisible) {
	    /* make it visible... */
	    if (!tw->termview.scrollBar.widget) {
		/* need to create it first... */
		(void) CreateScrollBar(w);
	    }
	    (void) XtManageChild(tw->termview.scrollBar.widget);
	    tw->termview.scrollBar.managed = True;
	} else {
	    /* make it invisible... */
	    if (tw->termview.scrollBar.widget) {
		/* unmanage it... */
		(void) XtUnmanageChild(tw->termview.scrollBar.widget);
	    }
	    tw->termview.scrollBar.managed = False;
	}
    }

    /* check menubar visible... */
    if (tw->termview.menuBarVisible != cur_tw->termview.menuBarVisible) {
	if (tw->termview.menuBarVisible) {
	    /* make it visible... */
	    if (!tw->termview.menuBar.widget) {
		/* need to create it first... */
		tw->termview.menuBar.widget =
			_DtTermViewCreatePulldownMenu(w, w, NULL, 0);
		(void) XtManageChild(tw->termview.menuBar.widget);
		tw->termview.menuBar.managed = True;
	    }
	    (void) XtManageChild(tw->termview.menuBar.widget);
	} else {
	    /* make it invisible... */
	    if (tw->termview.menuBar.widget) {
		/* unmanage it... */
		(void) XtUnmanageChild(tw->termview.menuBar.widget);
		tw->termview.menuBar.managed = False;
	    }
	}
    }

    /* blinkRate... */
    if (tw->termview.blinkRate != cur_tw->termview.blinkRate) {
	(void) XtSetArg(al[ac], DtNblinkRate, tw->termview.blinkRate);
									ac++;
	tw->termview.blinkRate = -1;
    }

    /* charCursorStyle... */
    if (tw->termview.charCursorStyle != cur_tw->termview.charCursorStyle) {
	(void) XtSetArg(al[ac], DtNcharCursorStyle,
		tw->termview.charCursorStyle);				ac++;
	tw->termview.charCursorStyle = (unsigned char) -1;
    }

    /* visualBell... */
    if (tw->termview.visualBell != cur_tw->termview.visualBell) {
	(void) XtSetArg(al[ac], DtNvisualBell, tw->termview.visualBell);
									ac++;
	tw->termview.visualBell = (Boolean) -1;
    }

    /* marginBell... */
    if (tw->termview.marginBell != cur_tw->termview.marginBell) {
	(void) XtSetArg(al[ac], DtNmarginBell, tw->termview.marginBell);
									ac++;
	tw->termview.marginBell = (Boolean) -1;
    }

    /* nMarginBell... */
    if (tw->termview.nMarginBell != cur_tw->termview.nMarginBell) {
	(void) XtSetArg(al[ac], DtNnMarginBell, tw->termview.nMarginBell);
									ac++;
	tw->termview.nMarginBell = -1;
    }

    /* jump scroll... */
    if (tw->termview.jumpScroll != cur_tw->termview.jumpScroll) {
	(void) XtSetArg(al[ac], DtNjumpScroll, tw->termview.jumpScroll);
									ac++;
	tw->termview.jumpScroll = (Boolean) -1;
    }

    /* check rows and columns... */
    if ((tw->termview.rows != cur_tw->termview.rows) ||
	    (tw->termview.columns != cur_tw->termview.columns)) {
	/* save away our default width and height... */
	if (tw->termview.sizeDefault.rows <= 0) {
	    tw->termview.sizeDefault.rows = cur_tw->termview.rows;
	}
	if (tw->termview.sizeDefault.columns <= 0) {
	    tw->termview.sizeDefault.columns = cur_tw->termview.columns;
	}

	(void) XtSetArg(al[ac], DtNcolumns, tw->termview.columns); ac++;
	(void) XtSetArg(al[ac], DtNrows, tw->termview.rows); ac++;

	/* Let's set the columns and rows back at this point.  They will
	 * be set for real when and if our size actually changes...
	 */
	tw->termview.columns = cur_tw->termview.columns;
	tw->termview.rows = cur_tw->termview.rows;
    }

    if (tw->termview.fontList != cur_tw->termview.fontList) {
	(void) XtSetArg(al[ac], DtNuserFont, tw->termview.fontList); ac++;

	/*DKS: we should change the font for the status line here as well...
	 */
    }

    if (ac > 0) {
	(void) XtSetValues(tw->termview.term, al, ac);
    }

    return(flag);
}

/* Realize...
 */
static void
Realize(Widget w, XtValueMask *p_valueMask, XSetWindowAttributes *attributes)
{
    Mask valueMask = *p_valueMask;

    Debug('T', timeStamp("DtTermView Realize() starting"));
#ifdef	NOTDEF
    /* I don't think that we want to do this yet, since I don't think we
     * know enough to do it at this time...
     */
#endif	/* NOTDEF */
    /* update our rows, columns, widthInc, heightInc, baseWidth,
     * baseHeight...
     */
    (void) SetSizeInfo(w, -1, -1);

    /*	Make sure height and width are not zero.
     */
    if (!XtWidth(w)) XtWidth(w) = 1 ;
    if (!XtHeight(w)) XtHeight(w) = 1 ;
    
    valueMask |= CWBitGravity | CWDontPropagate;
    attributes->bit_gravity = NorthWestGravity;
    attributes->do_not_propagate_mask =
	    ButtonPressMask | ButtonReleaseMask |
	    KeyPressMask | KeyReleaseMask | PointerMotionMask;

    XtCreateWindow (w, InputOutput, CopyFromParent, valueMask, attributes);
    Debug('T', timeStamp("DtTermView realize finished"));
}

/* Destroy...
 */
static void
Destroy(Widget w)
{
    DtTermViewWidget tw = (DtTermViewWidget) w;

    if (tw->termview.userFontName)
        XtFree(tw->termview.userFontName);
}

Widget
DtCreateTermView(Widget parent, char *name, ArgList arglist, Cardinal argcount)
{
    Widget w;

    Debug('T', timeStamp("DtCreateTermView() starting"));
    w = XtCreateWidget(name, dtTermViewWidgetClass, parent, arglist, argcount);
    Debug('T', (void) timeStamp("DtCreateTermView() started"));
    return(w);
}

static XtGeometryResult
GeometryManager(Widget child, XtWidgetGeometry *desired,
	XtWidgetGeometry *allowed)
{
    DtTermViewWidget tw;
    Dimension width;
    Dimension height;
    Dimension widthReturn;
    Dimension heightReturn;
    Dimension ignoreReturn;
    char *c = NULL;
    XtGeometryResult result;
    Boolean callResize = False;

    /* get our parent DtTermView widget... */
    for (tw = (DtTermViewWidget) XtParent(child);
	    !XtIsSubclass((Widget) tw, dtTermViewWidgetClass);
	    tw = (DtTermViewWidget) XtParent((Widget) tw))
	/*EMPTY*/
	;

    /* initialize the allowed structure to the desired one... */
    (void) memcpy((char *) allowed, (char *) desired, sizeof(*desired));

    /* update all our managed flags... */
    if (tw->termview.menuBar.widget)
	tw->termview.menuBar.managed = XtIsManaged(tw->termview.menuBar.widget);
    if (tw->termview.scrollBar.widget)
	tw->termview.scrollBar.managed = XtIsManaged(tw->termview.scrollBar.widget);

    if (child == tw->termview.scrolledWindowFrame) {
	/* handle geometry changes from the scrolledWindow...
	 */
	result = XtGeometryYes;

	/* don't allow changes in x or y... */
	if ((desired->request_mode & CWX) && (desired->x != child->core.x)) {
	    allowed->x = child->core.x;
	    result = XtGeometryAlmost;
	}
	if ((desired->request_mode & CWY) && (desired->y != child->core.y)) {
	    allowed->y = child->core.y;
	    result = XtGeometryAlmost;
	}

	/* if we are still in XtGeometryYes mode, then make width and height
	 * changes...
	 * DKS: we should probably confirm this with our parent first!!!...
	 */
	if ((result == XtGeometryYes) &&
		!(desired->request_mode & XtCWQueryOnly)) {
	    /* calculate our new size... */
	    width = tw->core.width;
	    if (desired->request_mode & CWWidth) {
		width = desired->width + 2 * child->core.border_width;
	    }

	    height = widgetHeight(child);
	    if (desired->request_mode & CWHeight) {
		height = desired->height + 2 * child->core.border_width;
	    }
	    if (tw->termview.menuBar.widget && tw->termview.menuBar.managed)
		height += widgetHeight(tw->termview.menuBar.widget);

	    /* we need to update our widthInc, heightInc, baseWidth, and
	     * baseHeight before we change our size so that the window
	     * manager will honour them...
	     */
	    width += ourWidthAdjustment(tw);
	    height += ourHeightAdjustment(tw);
	    (void) ResetSizeInfo((Widget) tw, width, height);

	    switch (XtMakeResizeRequest((Widget) tw, width, height,
		    &widthReturn, &heightReturn)) {
	    case XtGeometryYes:
		callResize = True;
		break;

	    case XtGeometryAlmost:
		/* take what we were able to get... */
		(void) XtMakeResizeRequest((Widget) tw, widthReturn,
			heightReturn, &ignoreReturn, &ignoreReturn);

		/* we aren't going to be allowed to resize the DtTermView
		 * window to the extent that we need to, so let's figure
		 * out what we can allow...
		 */
		if ((desired->request_mode & CWHeight) &&
			(height != heightReturn)) {
		    /* height request was denied.  Back the allowed height
		     * out...
		     */
		    if (tw->termview.menuBar.managed)
			heightReturn -= widgetHeight(tw->termview.menuBar.widget);
		    allowed->height = heightReturn - ourHeightAdjustment(tw);
		    result = XtGeometryAlmost;
		}

		if ((desired->request_mode & CWWidth) &&
			(width != widthReturn)) {
		    allowed->width = widthReturn - ourWidthAdjustment(tw);
		    result = XtGeometryAlmost;
		}
		break;
	    
	    case XtGeometryNo:
		result = XtGeometryNo;
		break;
	    }

	    /* set the window manager hints now...
	     */
	    (void) SetSizeInfo((Widget) tw, -1, -1);

	    /* if we are still in XtGeometryYes mode, it is safe to make
	     * the actual changes...
	     */
		    
	    if (result == XtGeometryYes) {
		/* make any requested changes... */
		if (desired->request_mode & CWWidth) {
		    child->core.width = desired->width;
		}
		if (desired->request_mode & CWHeight) {
		    child->core.height = desired->height;
		}
		if (desired->request_mode & CWX) {
		    child->core.x = desired->x;
		}
		if (desired->request_mode & CWY) {
		    child->core.y = desired->y;
		}
	    }
	}

	/* if we had ourself resized and get a XtGeometryYes reply, we need to
	 * call our resize method...
	 */
	if (callResize) {
	    (void) Resize((Widget) tw);
	}

	return(result);
    }
	
    if (child == tw->termview.menuBar.widget) {
	c = "menuBar";
    } else {
	c = "<unknown>";
    }

    (void) printf("GeometryManager: %s widget (0x%lx) requesting geo change\n",
	    c, child);

    return(XtGeometryNo);
}

static void
ChangeManaged(Widget w)
{
    DtTermViewWidget tw = (DtTermViewWidget) w;
    Dimension width;
    Dimension height;
    Dimension widthReturn;
    Dimension heightReturn;

    /* we have 3 widgets to worry about:
     *	-the scrolled window that holds the term widget,
     *	-the menu
     */

    /* update all our managed flags... */
    if (tw->termview.menuBar.widget)
	tw->termview.menuBar.managed = XtIsManaged(tw->termview.menuBar.widget);
    if (tw->termview.scrollBar.widget)
	tw->termview.scrollBar.managed = XtIsManaged(tw->termview.scrollBar.widget);

    height = 0;
    width = widgetWidth(tw->termview.scrolledWindowFrame);

    if (tw->termview.menuBar.widget && tw->termview.menuBar.managed) {
	(void) XtConfigureWidget(tw->termview.menuBar.widget,
		0,
		0,
		width + ourWidthAdjustment(tw) - 2 *
			tw->termview.menuBar.widget->core.border_width,
		tw->termview.menuBar.widget->core.height,
		tw->termview.menuBar.widget->core.border_width);
	height += widgetHeight(tw->termview.menuBar.widget);
    }

    (void) XtConfigureWidget(tw->termview.scrolledWindowFrame,
	    ourWidthOffset(tw),
	    ourHeightOffset(tw) + height,
	    tw->termview.scrolledWindowFrame->core.width,
	    tw->termview.scrolledWindowFrame->core.height,
	    tw->termview.scrolledWindowFrame->core.border_width);

    height += widgetHeight(tw->termview.scrolledWindowFrame);

    /* we need to update our widthInc, heightInc, baseWidth, and baseHeight
     * before we change our size so that the window manager will honour them...
     */
    width += ourWidthAdjustment(tw);
    height += ourHeightAdjustment(tw);
    (void) ResetSizeInfo(w, width, height);

    switch (XtMakeResizeRequest(w, width, height, &widthReturn,
	    &heightReturn)) {
	case XtGeometryAlmost:
	    /* we didn't get our requested size, so we mey need to recalculate
	     * lines and or columns...
	     */
	    (void) XtMakeResizeRequest(w, widthReturn, heightReturn,
		    (Dimension *) 0, (Dimension *) 0);
	    break;
    }

    /* set the window manager hints now...
     */
    (void) SetSizeInfo(w, -1, -1);

    return;
}

static void
CreateScrollBar(Widget w)
{
    DtTermViewWidget tw = (DtTermViewWidget) w;
    Arg arglist[20];
    int i;

    i = 0;
    (void) XtSetArg(arglist[i], XmNorientation, XmVERTICAL);
    tw->termview.scrollBar.widget = XtCreateManagedWidget("dtTermScrollBar",
	    xmScrollBarWidgetClass, tw->termview.scrolledWindow,
	    arglist, i);
    (void) XmScrolledWindowSetAreas(tw->termview.scrolledWindow,
	    (Widget) 0, tw->termview.scrollBar.widget, tw->termview.term);
    tw->termview.scrollBar.managed = True;

    i = 0;
    (void) XtSetArg(arglist[i], DtNverticalScrollBar,
	    tw->termview.scrollBar.widget); i++;
    (void) XtSetValues(tw->termview.term, arglist, i);
    return;
}

#ifdef	NOTUSED
static void
GetDesiredChildWidth(Widget w, Widget child, int *childX,
	Dimension *childWidth)
{
    DtTermViewWidget tw = (DtTermViewWidget) w;
    Dimension scrolledWindowSpacing;
    unsigned char scrollBarPlacement;
    Arg arglist[10];
    int i;

    *childX = 0;
    *childWidth = widgetWidth(tw->termview.scrolledWindow) -
	    2 * child->core.border_width;

    if (tw->termview.scrollBar.widget && tw->termview.scrollBar.managed) {
	/* get the spacing and placement...
	 */
	i = 0;
	(void) XtSetArg(arglist[i], DtNscrollBarPlacement,
		&scrollBarPlacement); i++;
	(void) XtSetArg(arglist[i], DtNspacing, &scrolledWindowSpacing); i++;
	(void) XtGetValues(tw->termview.scrolledWindow, arglist, i);
	*childWidth -= scrolledWindowSpacing +
		widgetWidth(tw->termview.scrollBar.widget);
	if ((XmTOP_LEFT == scrollBarPlacement) || (XmBOTTOM_LEFT ==
		scrollBarPlacement)) {
	    *childX = widgetWidth(tw->termview.scrolledWindow) -
		    *childWidth - 2 * child->core.border_width;
	}
    }

    return;
}
#endif	/* NOTUSED */

/* the DtTermView widget to apply this callback to is passed via the
 * client_data rather than the wid paramater...
 */
/*ARGSUSED*/
static void
SubprocessTerminationCallback(Widget wid, XtPointer client_data,
	XtPointer call_data)
{
    DtTermViewWidget tw = (DtTermViewWidget) client_data;
    DtTermSubprocessTerminationCallbackStruct *termCB =
	    (DtTermSubprocessTerminationCallbackStruct *) call_data;
    DtTermViewCallbackStruct termviewCB;

    if (tw->termview.subprocessTerminationCallback) {
	(void) memset(&termviewCB, '\0', sizeof(termviewCB));
	/* termviewCB.reason = ?; DKS: this needs to be implemented some day... */
	termviewCB.pid = termCB->pid;
	termviewCB.status = termCB->status;

	(void) XtCallCallbackList((Widget) tw,
		tw->termview.subprocessTerminationCallback, &termviewCB);
    }
}

/*ARGSUSED*/
static void
handleEnterLeaveEvents(Widget w, XtPointer closure, XEvent *event,
	Boolean *cont)
{
    switch (event->type) {
	case EnterNotify:
	case LeaveNotify:
	    /* let's update the caps lock indicator... */
	    break;
    }
}

Boolean
DtTermViewGetCloneEnabled(Widget w)
{
    DtTermViewWidget tw = (DtTermViewWidget) w;

    if (tw->termview.newCallback) {
	return(True);
    }
    return(False);
}

/*ARGSUSED*/
void
DtTermViewCloneCallback(Widget w, XtPointer client_data, XtPointer call_data)
{
    DtTermViewWidget tw = (DtTermViewWidget) w;
    XmPushButtonCallbackStruct *pbCB = (XmPushButtonCallbackStruct *) call_data;
    DtTermViewCallbackStruct _termCB;
    DtTermViewCallbackStruct *termCB = &_termCB;
    Arg arglist[40];
    int i;

    termCB->reason = 0;
    termCB->event = pbCB->event;

    i = 0;
    /* let's use a checklist to insure that we are cloning all our
     * resources...
     */
    /* widgetInfo menuBar		set by new widget		*/
    /* Widget scrolledWindowFrame	set by new widget		*/
    /* Widget scrolledWindow		set by new widget		*/
    /* widgetInfo scrollBar		set by new widget		*/
    /* Widget term			set by new widget		*/

    /* unsigned char charCursorStyle */
    (void) XtSetArg(arglist[i], DtNcharCursorStyle, tw->termview.charCursorStyle);
	    i++;

    /* int blinkRate */
    (void) XtSetArg(arglist[i], DtNblinkRate, tw->termview.blinkRate); i++;

    /* unsigned char shadowType */
    (void) XtSetArg(arglist[i], DtNshadowType, tw->termview.shadowType); i++;

    /* XmFontList fontList */
    (void) XtSetArg(arglist[i], XmNfontList, tw->termview.fontList); i++;

    /* short rows */
    (void) XtSetArg(arglist[i], DtNrows, tw->termview.rows); i++;

    /* short columns */
    (void) XtSetArg(arglist[i], DtNcolumns, tw->termview.columns); i++;

    /* Dimension marginHeight */
    (void) XtSetArg(arglist[i], DtNmarginHeight, tw->termview.marginHeight); i++;

    /* Dimension marginWidth */
    (void) XtSetArg(arglist[i], DtNmarginWidth, tw->termview.marginWidth); i++;

    /* Dimension spacing */
    (void) XtSetArg(arglist[i], XmNspacing, tw->termview.spacing); i++;

    /* int baseWidth			set by new widget		*/
    /* int baseHeight			set by new widget		*/
    /* int widthInc			set by new widget		*/
    /* int heightInc			set by new widget		*/
    /* int pty				set by new widget		*/
    /* int ptyAllocate							*/
    (void) XtSetArg(arglist[i], DtNtermDeviceAllocate, True);

    /* char *ptySlaveName		set by new widget		*/
    /* int subprocessPid		set by new widget		*/

    /* char *subprocessCmd */
    (void) XtSetArg(arglist[i], DtNsubprocessCmd, tw->termview.subprocessCmd);
	    i++;

    /* char **subprocessArgv */
    (void) XtSetArg(arglist[i], DtNsubprocessArgv, tw->termview.subprocessArgv);
	    i++;

    /* Boolean subprocessLoginShell */
    (void) XtSetArg(arglist[i], DtNsubprocessLoginShell,
	    tw->termview.subprocessLoginShell); i++;

    /* Boolean subprocessTerminationCatch */
    (void) XtSetArg(arglist[i], DtNsubprocessTerminationCatch,
	    tw->termview.subprocessTerminationCatch); i++;

    /* Boolean subprocessExec */
    (void) XtSetArg(arglist[i], DtNsubprocessExec, tw->termview.subprocessExec);
	    i++;

    /* Boolean scrollBarVisible */
    (void) XtSetArg(arglist[i], DtNscrollBar,
	    tw->termview.scrollBarVisible); i++;

    /* Boolean menuBarVisible */
    (void) XtSetArg(arglist[i], DtNmenuBar, tw->termview.menuBarVisible); i++;

    termCB->arglist = arglist;
    termCB->argcount = i;

    if (tw->termview.newCallback) {
	(void) XtCallCallbackList(w, tw->termview.newCallback,
		(XtPointer) termCB);
    }
}

/*ARGSUSED*/
static void
closeHelp(Widget w, XtPointer client_data, XtPointer call_data)
{
    (void) XtUnmanageChild(w);
}

static void
destroyHelp
(
    Widget		  w,
    XtPointer		  client_data,
    XtPointer		  call_data
)
{
    (void) XtDestroyWidget(w);
}

static void
hyperlinkCallback
(
    Widget		  w,
    XtPointer		  clientData,
    XtPointer		  callData
)
{
    Arg			  al[10];
    int			  ac;
    Widget		  quickHelpDialog;
    DtHelpDialogCallbackStruct
			 *hyperData = (DtHelpDialogCallbackStruct *) callData;
    
    switch ((int) hyperData->hyperType) {
    case DtHELP_LINK_TOPIC:
	break;

    case DtHELP_LINK_MAN_PAGE:
	ac = 0;
	(void) XtSetArg(al[ac], DtNhelpType, DtHELP_TYPE_MAN_PAGE);	ac++;
	(void) XtSetArg(al[ac], DtNmanPage, hyperData->specification);	ac++;
	(void) XtSetArg(al[ac], DtNcolumns, 80);			ac++;
	(void) XtSetArg(al[ac], DtNrows, 20);				ac++;
	(void) XtSetArg(al[ac], XmNtitle, GETMESSAGE(NL_SETN_View, 4, "Terminal - Man Page"));ac++;
	quickHelpDialog = DtCreateHelpQuickDialog(w, "helpDialog", al, ac);
#ifdef	HPVUE
	(void) XtAddCallback(quickHelpDialog, XmNokCallback,
		destroyHelp, (XtPointer) 0);
#else	/* HPVUE */
	(void) XtAddCallback(quickHelpDialog, DtNcloseCallback,
		destroyHelp, (XtPointer) 0);
#endif	/* HPVUE */
	(void) XtManageChild(quickHelpDialog);
	break;

    case DtHELP_LINK_APP_DEFINE:
	break;
    }
}

    

void
_DtTermViewMapHelp(Widget w, char *volume, char *id)
{
    DtTermViewWidget tw = (DtTermViewWidget) w;
    Widget sw;
    Arg arglist[10];
    int i;

    i = 0;
    (void) XtSetArg(arglist[i], DtNhelpType, DtHELP_TYPE_TOPIC); i++;
    (void) XtSetArg(arglist[i], DtNlocationId, id); i++;
    (void) XtSetArg(arglist[i], DtNhelpVolume, volume); i++;
    (void) XtSetArg(arglist[i], XmNtitle, GETMESSAGE(NL_SETN_View, 3,"Terminal - Help")); i++;
    if (tw->termview.helpWidget) {
	(void) XtSetValues(tw->termview.helpWidget, arglist, i);
    } else {
	tw->termview.helpWidget = DtCreateHelpDialog(w, "helpDialog",
		arglist, i);
#ifdef	BOGUS
#ifdef	HPVUE
	(void) XtAddCallback(tw->termview.helpWidget, XmNokCallback,
		closeHelp, (XtPointer) 0);
#else	/* HPVUE */
	(void) XtAddCallback(tw->termview.helpWidget, DtNcloseCallback,
		closeHelp, (XtPointer) 0);
#endif	/* HPVUE */
#endif	/* BOGUS */
	(void) XtAddCallback(tw->termview.helpWidget, DtNcloseCallback,
		closeHelp, (XtPointer) 0);
	(void) XtAddCallback(tw->termview.helpWidget, DtNhyperLinkCallback,
		hyperlinkCallback, (XtPointer) 0);
    }
    (void) XtManageChild(tw->termview.helpWidget);

    for (sw = tw->termview.helpWidget; !XtIsShell(sw); sw = XtParent(sw))
	;
    if (sw) {
	(void) XRaiseWindow(XtDisplay(sw), XtWindow(sw));
    }
}

Widget
_DtTermViewGetChild(Widget w, _DtTermViewChildType child)
{
    DtTermViewWidget tw = (DtTermViewWidget) w;

    switch (child) {
    case DtTermTERM_WIDGET:
	return(tw->termview.term);
	/*NOTREACHED*/
	break;

    case DtTermMENUBAR_WIDGET:
	return(tw->termview.menuBar.widget);
	/*NOTREACHED*/
	break;

    case DtTermSCROLLBAR_WIDGET:
	return(tw->termview.scrollBar.widget);
	/*NOTREACHED*/
	break;
    }

    return((Widget) 0);
}

static void
handleProcessStructureNotifyEvent(Widget w, XtPointer eventData, XEvent *event,
	Boolean *cont)
{
    DtTermViewWidget tw = (DtTermViewWidget) eventData;

    switch(event->type) {
    case MapNotify:
	if (tw->termview.sizeDefault.rows <= 0) {
	    tw->termview.sizeDefault.rows = tw->termview.rows;
	}
	if (tw->termview.sizeDefault.columns <= 0) {
	    tw->termview.sizeDefault.columns = tw->termview.columns;
	}
	break;
    }
}
