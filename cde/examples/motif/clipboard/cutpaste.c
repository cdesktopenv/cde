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
/* $XConsortium: cutpaste.c /main/4 1995/10/27 10:41:39 rswiston $ */
/*
 * (c) Copyright 1989, 1990, 1991, 1992 OPEN SOFTWARE FOUNDATION, INC. 
 * ALL RIGHTS RESERVED 
 */
/*
 * Motif Release 1.2
 */

/******************************************************************************
 * cutpaste.c
 *
 *
 *****************************************************************************/

#include <stdio.h>
#include <Xm/Xm.h>
#include <Xm/CutPaste.h>
#include <Mrm/MrmPublic.h>


static Boolean CopyToClipboard(
	Time time);
static Boolean PasteClipboard(
	Time time );
static void PopupHandler(
	Widget w,
	Widget pw,
	XEvent *event,
	Boolean *ctd );
static void ManageCb(
	Widget w,
	String id,
	XtPointer cb );
static void InitPopupCb(
	Widget w,
	String id,
	XtPointer cb );
static void CutCb(
	Widget w,
	XtPointer cd,
	XmPushButtonCallbackStruct *cb );
static void CopyCb(
	Widget w,
	XtPointer cd,
	XmPushButtonCallbackStruct *cb );
static void PasteCb(
	Widget w,
	XtPointer cd,
	XmPushButtonCallbackStruct *cb );
static void ExitCb(
	Widget w,
	XtPointer cd,
	XtPointer cb );


static MrmHierarchy mrm_id;
static char *mrm_vec[]={"cutpaste.uid"};
static MrmCode mrm_class;
static MRMRegisterArg mrm_names[] = {
        {"InitPopupCb", (XtPointer)InitPopupCb },
        {"ManageCb", (XtPointer)ManageCb },
        {"CutCb", (XtPointer)CutCb },
        {"CopyCb", (XtPointer)CopyCb },
        {"PasteCb", (XtPointer)PasteCb },
        {"ExitCb", (XtPointer)ExitCb }
};

#define GraphFormat "MY_GRAPH_DATA"

#define MIN(x,y)  ((x) > (y) ? (y) : (x))

static XtAppContext  appContext;
static Widget shell;
static Widget *bars;
static Cardinal nbars;

int
main(
    int argc,
    char *argv[] )
{
    Widget app_main;
    Display *display;
    Arg args[2];
    Widget *children;

    XtToolkitInitialize();
    MrmInitialize ();
    appContext = XtCreateApplicationContext();
    display = XtOpenDisplay(appContext, NULL, argv[0], "Cutpaste",
			NULL, 0, &argc, argv);
    if (display == NULL) {
	fprintf(stderr, "%s:  Can't open display\n", argv[0]);
	exit(1);
    }
    shell = XtAppCreateShell(argv[0], NULL, applicationShellWidgetClass,
			  display, NULL, 0);
    if (MrmOpenHierarchy (1, mrm_vec, NULL, &mrm_id) != MrmSUCCESS) exit(0);
    MrmRegisterNames(mrm_names, XtNumber(mrm_names));
    MrmFetchWidget (mrm_id, "appMain", shell, &app_main, &mrm_class);
    XtManageChild(app_main);

    /* define graph data format */

    while (XmClipboardRegisterFormat (display, GraphFormat, 32) ==
					XmClipboardLocked)  { }

    XtSetArg (args[0], XmNchildren, &children);
    XtSetArg (args[1], XmNnumChildren, &nbars);
    XtGetValues (XtNameToWidget (shell, "*graphForm"), args, 2);
    bars = (Widget *) XtMalloc (nbars * sizeof(Widget));
    memcpy (bars, children, nbars * sizeof(Widget));

    XtRealizeWidget(shell);

    XtAppMainLoop(appContext);
}

static void
ManageCb(
    Widget w,
    String id,
    XtPointer cb )

{
    XtManageChild (XtNameToWidget (shell, id));
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
 * PopupMenu support
 *
 *****************************************************************/

static void
InitPopupCb(
    Widget w,
    String id,
    XtPointer cb )
{
    Widget popupWindow = XtNameToWidget (shell, id);

    XtAddEventHandler (popupWindow, ButtonPressMask, False,
		(XtEventHandler)PopupHandler, (XtPointer) w);
}

static void
PopupHandler (
    Widget w,
    Widget pw,
    XEvent *event,
    Boolean *ctd )
{
    if (((XButtonEvent *)event)->button != Button3) return;

    XmMenuPosition((Widget) pw, (XButtonEvent *)event);
    XtManageChild ((Widget) pw);
}

/*****************************************************************
 *
 * Clipboard support
 *
 *****************************************************************/

static void
CutCb(
    Widget w,
    XtPointer cd,
    XmPushButtonCallbackStruct *cb )
{
    XButtonEvent *be = (XButtonEvent *)cb->event;
    Arg args[1];
    int i;

    if (CopyToClipboard(be->time) == True) {

	/* clear graph data */

	XtSetArg (args[0], XmNvalue, 0);
	for (i=0; i < nbars; i++)
	    XtSetValues (bars[i], args, 1);
    }
    else {
	XBell (XtDisplay(w), 0);
    }
}

static void
CopyCb(
    Widget w,
    XtPointer cd,
    XmPushButtonCallbackStruct *cb )
{
    XButtonEvent *be = (XButtonEvent *)cb->event;

    if (CopyToClipboard(be->time) != True) {
	XBell (XtDisplay(w), 0);
    }
}

static void
PasteCb(
    Widget w,
    XtPointer cd,
    XmPushButtonCallbackStruct *cb )
{
    XButtonEvent *be = (XButtonEvent *)cb->event;

    if (PasteClipboard(be->time) != True) {
	XBell (XtDisplay(w), 0);
    }
}

static Boolean
CopyToClipboard(
    Time time )
{
    Display *dpy = XtDisplay (shell);
    Window window = XtWindow (shell);
    long itemId = 0;
    XmString clipLabel;
    long *graphData;
    int value;
    int i;
    Arg args[1];

    if (XmClipboardLock (dpy, window) == XmClipboardLocked)
	return (False);

    clipLabel = XmStringCreateLtoR ("cutpaste", XmFONTLIST_DEFAULT_TAG);

    XmClipboardStartCopy ( dpy, window, clipLabel, time, NULL, NULL, &itemId);

    /* copy graph data */

    graphData = (long *)XtMalloc (nbars * sizeof (long));
    XtSetArg (args[0], XmNvalue, &value);
    for (i=0; i < nbars; i++) {
	XtGetValues (bars[i], args, 1);
	graphData[i] = (long)value;
    }

    XmClipboardCopy (dpy, window, itemId, GraphFormat, (XtPointer)graphData,
			(nbars * sizeof(long)), 0, NULL);

    XmClipboardEndCopy (dpy, window, itemId);

    XmClipboardUnlock (dpy, window, False);

    XtFree ((char *)clipLabel);
    XtFree ((char *)graphData);

    return (True);
}

static Boolean
PasteClipboard(
    Time time )
{
    Display *dpy = XtDisplay (shell);
    Window window = XtWindow (shell);
    unsigned long length = 0;
    long *graphData;
    int i;
    int setBars;
    Arg args[1];

    if (XmClipboardStartRetrieve (dpy, window, time) == XmClipboardLocked)
	return (False);

    XmClipboardInquireLength(dpy, window, GraphFormat, &length);

    if (length == 0) {
	XmClipboardEndRetrieve (dpy, window);
	return (False);
    }

    graphData = (long *)XtMalloc (length * sizeof (long));

    XmClipboardRetrieve(dpy, window, GraphFormat, graphData, length,
			NULL, NULL);

    XmClipboardEndRetrieve(dpy, window);

    setBars = MIN (nbars, length);
    for (i = 0; i < setBars; i++) {
	XtSetArg (args[0], XmNvalue, graphData[i]);
	XtSetValues (bars[i], args, 1);
    }

    XtFree ((char *)graphData);

    return (True);
}

