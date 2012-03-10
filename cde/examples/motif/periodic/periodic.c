/* 
 * (c) Copyright 1989, 1990, 1991, 1992, 1993 OPEN SOFTWARE FOUNDATION, INC. 
 * ALL RIGHTS RESERVED 
*/ 
/* 
 * Motif Release 1.2.2
*/ 
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: periodic.c /main/4 1995/10/27 10:44:10 rswiston $"
#endif
#endif

/******************************************************************************
 * periodic.c
 *
 * Copy and rename the file periodic.ad to Periodic in your home directory
 * or app-defaults directory, or merge it with your .Xdefaults file.
 *
 * It provides useful default values for Periodic fonts and colors
 *
 *****************************************************************************/

#include <stdio.h>
#include <Xm/Xm.h>
#include <Xm/Scale.h>
#include <Xm/ToggleB.h>
#include <Mrm/MrmPublic.h>


typedef struct _DrawData {
	GC gc;
	Position drawX;
	Position drawY;
	Dimension drawWidth;
	Dimension drawHeight;
} DrawData;


static GC GetGC(
	Widget w );
static void ConfigureDrawData(
	Widget w,
	DrawData *data );
static void DrawButton(
	Widget w );
static void DrawArea(
	Widget w );
static void PopupHandler(
        Widget w,
        Widget pw,
        XEvent *event,
        Boolean *ctd );

static void ManageCb(
        Widget w,
        String id,
        XmToggleButtonCallbackStruct *cb );
static void UnmapCb(
        Widget w,
        XtPointer cd,
        XtPointer cb );
static void UnmanageCb(
        Widget w,
        String id,
        XtPointer cb );
static void InitPopupCb(
        Widget w,
        String id,
        XtPointer cb );
static void PopdownCb(
        Widget w,
        XtPointer cd,
        XtPointer cb );
static void DaExposeCb(
        Widget w,
        XtPointer cd,
        XtPointer cb );
static void DaResizeCb(
        Widget w,
        XtPointer cd,
        XtPointer cb );
static void DbExposeCb(
        Widget w,
        XtPointer cd,
        XtPointer cb );
static void DbResizeCb(
        Widget w,
        XtPointer cd,
        XtPointer cb );
static void ScaleCb(
        Widget w,
        XtPointer cd,
        XtPointer cb );
static void SetScaleCb(
        Widget w,
        int *value,
        XmToggleButtonCallbackStruct *cb );
static void ViewCb(
        Widget w,
        XtPointer cd,
        XmToggleButtonCallbackStruct *cb );
static void LayoutCb(
        Widget w,
        XtPointer cd,
        XmToggleButtonCallbackStruct *cb );
static void ToggleLightsCb(
        Widget w,
        XtPointer cd,
        XmToggleButtonCallbackStruct *cb );
static void ShowCb(
        Widget w,
        String id,
        XtPointer cb );
static void ExitCb(
        Widget w,
        XtPointer cd,
        XtPointer cb );

static MrmHierarchy mrmId;
static char *mrmFile[]={"periodic.uid"};
static MrmCode mrmClass;
static MRMRegisterArg mrmNames[] = {
        {"InitPopupCb", (XtPointer)InitPopupCb },
        {"PopdownCb", (XtPointer)PopdownCb },
        {"UnmapCb", (XtPointer)UnmapCb },
        {"UnmanageCb", (XtPointer)UnmanageCb },
        {"ManageCb", (XtPointer)ManageCb },
        {"DaExposeCb", (XtPointer)DaExposeCb },
        {"DaResizeCb", (XtPointer)DaResizeCb },
        {"DbExposeCb", (XtPointer)DbExposeCb },
        {"DbResizeCb", (XtPointer)DbResizeCb },
        {"ScaleCb", (XtPointer)ScaleCb },
        {"SetScaleCb", (XtPointer)SetScaleCb },
        {"ViewCb", (XtPointer)ViewCb },
        {"LayoutCb", (XtPointer)LayoutCb },
        {"ToggleLightsCb", (XtPointer)ToggleLightsCb },
        {"ShowCb", (XtPointer)ShowCb },
        {"ExitCb", (XtPointer)ExitCb }
};

static String fallbackResources[] = {
"periodic*XmText.columns: 10",
"periodic*XmTextField.columns: 10",
"periodic*scaleFrame*XmScale.width: 50",
"periodic*scrollFrame*XmScrollBar.width: 50",

"periodic*fontList: *-*-*-medium-r-*-*-*-100-*-*-p-*-*-*",

"periodic*titleLabel.fontList: *-*-*-bold-r-*-*-*-180-*-*-p-*-*-*",
"periodic*subtitleLabel.fontList: *-*-*-bold-r-*-*-*-140-*-*-p-*-*-*",
"periodic*labelLabel.fontList: *-*-*-bold-r-*-*-*-180-*-*-p-*-*-*",
"periodic*menuBar*fontList: *-*-*-medium-r-*-*-*-120-*-*-p-*-*-*",
"periodic*popupMenu*fontList: *-*-*-medium-r-*-*-*-120-*-*-p-*-*-*",
"periodic*XmMessageBox*fontList: *-*-*-medium-r-*-*-*-120-*-*-p-*-*-*",
"periodic*fileDialog*fontList: *-*-*-medium-r-*-*-*-120-*-*-p-*-*-*",
"periodic*selectDialog*fontList: *-*-*-medium-r-*-*-*-120-*-*-p-*-*-*",
"periodic*promptDialog*fontList: *-*-*-medium-r-*-*-*-120-*-*-p-*-*-*",
NULL
};

static XtAppContext  appContext;
static Widget shell;

int
main(
    int argc,
    char *argv[] )
{
    Widget appMain;
    Display *display;
    Arg args[2];

    XtToolkitInitialize();
    MrmInitialize ();
    appContext = XtCreateApplicationContext();
    XtAppSetFallbackResources (appContext, fallbackResources);
    display = XtOpenDisplay(appContext, NULL, "periodic", "Periodic",
			NULL, 0, &argc, argv);
    if (display == NULL) {
	fprintf(stderr, "%s:  Can't open display\n", argv[0]);
	exit(1);
    }
    XtSetArg (args[0], XmNargc, argc);
    XtSetArg (args[1], XmNargv, argv);
    shell = XtAppCreateShell("periodic", "Periodic", applicationShellWidgetClass,
			  display, args, 2);
    if (MrmOpenHierarchy (1, mrmFile, NULL, &mrmId) != MrmSUCCESS) exit(0);
    MrmRegisterNames(mrmNames, XtNumber(mrmNames));
    MrmFetchWidget (mrmId, "appMain", shell, &appMain, &mrmClass);
    XtManageChild(appMain);
    XtRealizeWidget(shell);
    XtAppMainLoop(appContext);
}

static void
ExitCb(
    Widget w,
    XtPointer cd,
    XtPointer cb )
{
    exit(0);
}


/*****************************************************************
 *
 * Display selected Dialog widget
 *
 *****************************************************************/

static Widget managedToggle = NULL;

static void
ManageCb(
    Widget w,
    String id,
    XmToggleButtonCallbackStruct *cb )
{
    Widget dialog = XtNameToWidget (shell, id);

    if (cb->set) {
	if (managedToggle != NULL)
	    XmToggleButtonSetState (managedToggle, False, True);
	managedToggle = w;
	XtManageChild (dialog);
    }
    else {
	XtUnmanageChild (dialog);
	managedToggle = NULL;
    }
}

static void
UnmapCb(
    Widget w,
    XtPointer cd,
    XtPointer cb )
{
    XmToggleButtonSetState (managedToggle, False, True);
}

static void
UnmanageCb(
    Widget w,
    String id,
    XtPointer cb )
{
    XtUnmanageChild (XtNameToWidget (shell, id));
}

static void
ShowCb(
    Widget w,
    String id,
    XtPointer cb )
{
    static Widget tb = NULL;
    static Widget sc = NULL;
    int value;

    if (tb == NULL) tb = XtNameToWidget (shell, "*toggleButton");
    if (sc == NULL) sc = XtNameToWidget (shell, "*valueScale");

    XmScaleGetValue (sc, &value);
    if (XmToggleButtonGetState(tb) == True && value == 1012)
	XtManageChild (XtNameToWidget (shell, id));
}


/*****************************************************************
 *
 * Provide RadioBox behavior inside a PulldownMenu
 *
 *****************************************************************/

static void
ViewCb(
    Widget w,
    XtPointer cd,
    XmToggleButtonCallbackStruct *cb )
{
    static Widget viewToggle = NULL;

    if (cb->set) {
	if (viewToggle) XmToggleButtonSetState (viewToggle, False, False);
	viewToggle = w;
    }
    else {
	if (w == viewToggle) XmToggleButtonSetState (w, True, False);
    }
}

static void
LayoutCb(
    Widget w,
    XtPointer cd,
    XmToggleButtonCallbackStruct *cb )
{
    static Widget layoutToggle = NULL;

    if (cb->set) {
	if (layoutToggle) XmToggleButtonSetState (layoutToggle, False, False);
	layoutToggle = w;
    }
    else {
	if (w == layoutToggle) XmToggleButtonSetState (w, True, False);
    }
}


/*****************************************************************
 *
 * PopupMenu support
 *
 *****************************************************************/

static Time popupLastEventTime = 0;

static void
InitPopupCb(
    Widget w,
    String id,
    XtPointer cb )
{
    Widget popupWindow = XtNameToWidget (shell, id);

    XtAddEventHandler (popupWindow, ButtonPressMask, False,
			(XtEventHandler)PopupHandler, (XtPointer)w);
}

static void
PopupHandler (
    Widget w,
    Widget pw,
    XEvent *event,
    Boolean *ctd )
{
    if (((XButtonEvent *)event)->button != Button3) return;
    if (((XButtonEvent *)event)->time <= popupLastEventTime) return;

    XmMenuPosition((Widget) pw, (XButtonEvent *)event);
    XtManageChild ((Widget) pw);
}

/* By default, cancelling a popup menu with Button 3 will cause the
 * popup to be reposted at the location of the cancelling click.
 *
 * To switch off this behavior, remember when the menu was popped down.
 * In PopupHandler, don't repost the menu if the posting click just
 * cancelled a popup menu.
 */
static void
PopdownCb(
    Widget w,
    XtPointer cd,
    XtPointer cb )
{
    popupLastEventTime = XtLastTimestampProcessed (XtDisplay(w));
}


/*****************************************************************
 *
 * Draw utilities
 *
 *****************************************************************/

static DrawData *drawData = NULL;
static DrawData *buttonData = NULL;

static GC
GetGC(
    Widget w )
{
    Arg args[2];
    XGCValues gcv;
    Pixel fg;
    Pixel bg;
    GC gc;

    XtSetArg (args[0], XmNforeground, &fg);
    XtSetArg (args[1], XmNbackground, &bg);
    XtGetValues (w, args, 2);
    gcv.foreground = fg;
    gcv.background = bg;
    gcv.line_width = 1;
    gc = XtGetGC (w, GCForeground | GCBackground | GCLineWidth, &gcv);

    return (gc);
}

static void
ConfigureDrawData(
    Widget w,
    DrawData *data )
{
    Arg args[6];
    Dimension width, height, st, ht, mw, mh;
    Dimension totalMarginWidth;
    Dimension totalMarginHeight;

    width = height = st = ht = mw = mh = 0;
    XtSetArg (args[0], XmNwidth, &width);
    XtSetArg (args[1], XmNheight, &height);
    XtSetArg (args[2], XmNshadowThickness, &st);
    XtSetArg (args[3], XmNhighlightThickness, &ht);
    XtSetArg (args[4], XmNmarginWidth, &mw);
    XtSetArg (args[5], XmNmarginHeight, &mh);
    XtGetValues (w, args, 6);

    totalMarginWidth = st + ht + mw;
    totalMarginHeight = st + ht + mh;

    if (2u * totalMarginWidth < width && 2u * totalMarginHeight < height) {
	data->drawX = totalMarginWidth;
	data->drawY = totalMarginHeight;
	data->drawWidth = width - 2 * totalMarginWidth;
	data->drawHeight = height - 2 * totalMarginHeight;
    }
    else {
	data->drawWidth = 0;
	data->drawHeight = 0;
    }
}

/*****************************************************************
 *
 * DrawingArea display code
 *
 *****************************************************************/

static void
DaResizeCb(
    Widget w,
    XtPointer cd,
    XtPointer cb )
{
    if (drawData == NULL) return;

    ConfigureDrawData (w, drawData);
    XClearArea (XtDisplay(w), XtWindow(w),
		drawData->drawX, drawData->drawY,
		drawData->drawWidth, drawData->drawHeight,
		False);
    DrawArea (w);
}

static void
DaExposeCb(
    Widget w,
    XtPointer cd,
    XtPointer cb )
{
    if (drawData == NULL) {
	drawData = (DrawData *)XtMalloc (sizeof(DrawData));
	drawData->gc = GetGC (w);
	ConfigureDrawData (w, drawData);
    }
    DrawArea(w);
}

#define NPOINTS 40u

static void
DrawArea(
    Widget w )
{
    int i, x, y;
    unsigned int m;

    XPoint p[NPOINTS];

    if (drawData->drawWidth == 0) return;

    XClearArea (XtDisplay(w), XtWindow(w),
		drawData->drawX, drawData->drawY,
		drawData->drawWidth, drawData->drawHeight,
		False);
    XDrawRectangle (XtDisplay(w), XtWindow(w), drawData->gc,
		drawData->drawX, drawData->drawY,
		drawData->drawWidth, drawData->drawHeight);
    XDrawLine (XtDisplay(w), XtWindow(w), drawData->gc,
		drawData->drawX, drawData->drawY + drawData->drawHeight/2,
		drawData->drawX + drawData->drawWidth,
		drawData->drawY + drawData->drawHeight/2);

    m = 20 * drawData->drawHeight / 100u;
    p[0].x = drawData->drawX;
    p[0].y = drawData->drawY + drawData->drawHeight/2;
    for (i = 1; i < NPOINTS-1; i++) {
	p[i].x = drawData->drawX + (i * drawData->drawWidth)/NPOINTS;
	p[i].y = drawData->drawY + m/2u + (rand() % (drawData->drawHeight - m));
    }
    p[NPOINTS-1].x = drawData->drawX + drawData->drawWidth;
    p[NPOINTS-1].y = drawData->drawY + drawData->drawHeight/2;

    XDrawLines (XtDisplay(w), XtWindow(w), drawData->gc,
		p, NPOINTS, CoordModeOrigin);
}

static void
ScaleCb(
    Widget w,
    XtPointer cd,
    XtPointer cb )
{
    static Widget da = NULL;

    if (drawData == NULL) return;

    if (da == NULL) da = XtNameToWidget (shell, "*drawArea");

    DrawArea (da);
}

static void
SetScaleCb(
    Widget w,
    int *value,
    XmToggleButtonCallbackStruct *cb )
{
    static Widget da = NULL;
    static Widget sc = NULL;

    if (drawData == NULL) return;

    if (da == NULL) da = XtNameToWidget (shell, "*drawArea");
    if (sc == NULL) sc = XtNameToWidget (shell, "*valueScale");

    XmScaleSetValue (sc, *value);

    DrawArea (da);
}

/*****************************************************************
 *
 * DrawnButton display code
 *
 *****************************************************************/

static Boolean lightsOn = False;

static void
DbResizeCb(
    Widget w,
    XtPointer cd,
    XtPointer cb )
{
    if (buttonData == NULL) return;

    ConfigureDrawData (w, buttonData);
    XClearArea (XtDisplay(w), XtWindow(w),
		buttonData->drawX, buttonData->drawY,
		buttonData->drawWidth, buttonData->drawHeight,
		False);
    DrawButton (w);
}

static void
DbExposeCb(
    Widget w,
    XtPointer cd,
    XtPointer cb )
{
    if (buttonData == NULL) {
	buttonData = (DrawData *)XtMalloc (sizeof(DrawData));
	buttonData->gc = GetGC (w);
	ConfigureDrawData (w, buttonData);
    }
    DrawButton(w);
}

#define NARCS 6u

static void
DrawButton(
    Widget w )
{
    int i, x, y, incX, incY;
    XArc a[NARCS];

    if (buttonData->drawWidth == 0 || !lightsOn) return;

    a[0].x = buttonData->drawX + (buttonData->drawWidth - 1)/2u;
    a[0].y = buttonData->drawY + (buttonData->drawHeight - 1)/2u;
    a[0].width = 1;
    a[0].height = 1;
    a[0].angle1 = 0;
    a[0].angle2 = 360*64;
    incX = (buttonData->drawWidth - 1)/(2u * NARCS);
    incY = (buttonData->drawHeight - 1)/(2u * NARCS);

    for (i = 1; i < NARCS; i++) {
	a[i].x = a[i-1].x - incX;
	a[i].y = a[i-1].y - incY;
	a[i].width = a[i-1].width + 2 * incX;
	a[i].height = a[i-1].height + 2 * incY;
#ifndef BROKEN_SERVER_ARCS
	a[i].angle1 = 0;
	a[i].angle2 = 360 * 64;
#else
	XDrawRectangle (XtDisplay(w), XtWindow(w), buttonData->gc,
			a[i].x, a[i].y, a[i].width, a[i].height);
#endif
    }

#ifndef BROKEN_SERVER_ARCS
    XDrawArcs (XtDisplay(w), XtWindow(w), buttonData->gc, a, NARCS);
#endif
}

static void
ToggleLightsCb(
    Widget w,
    XtPointer cd,
    XmToggleButtonCallbackStruct *cb )
{
    static Widget db = NULL;

    if (buttonData == NULL) return;

    if (db == NULL) db = XtNameToWidget (shell, "*drawnButton");

    lightsOn = cb->set;

    if (lightsOn)
	DrawButton (db);
    else
	XClearArea (XtDisplay(db), XtWindow(db),
		buttonData->drawX, buttonData->drawY,
		buttonData->drawWidth, buttonData->drawHeight,
		False);
}

