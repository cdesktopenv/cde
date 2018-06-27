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
/* $XConsortium: wsinfo.c /main/3 1995/10/27 10:41:27 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company	
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 */

/* Demonstrate the CDE Workspace API. */

/*  
 * include files  
 */

#include <stdio.h>
#include <Xm/XmAll.h>
#include <Dt/Wsm.h>

/*  
 * functions
 */
void main();
static void quitCB(); 
static void updateCB(); 
static void ShowCurrentWorkspaceInfo();
static DtWsmWsChangeProc wschangecb();
static void InstallXErrorHandler();

/*  
 * global variables  
 */
static Widget toplevel;
static Widget wChildren[20];

/* 
 * button assignments 
 */
#define QUIT_BUTTON	(wChildren[0])
#define UPDATE_BUTTON	(wChildren[1])
#define SEPARATOR	(wChildren[2])
#define WS_LABEL	(wChildren[3])
#define WS_VALUE	(wChildren[4])
#define TITLE_LABEL	(wChildren[5])
#define TITLE_VALUE	(wChildren[6])
#define BACKDROP_LABEL	(wChildren[7])
#define BACKDROP_VALUE	(wChildren[8])
#define COLORSET_LABEL	(wChildren[9])
#define COLORSET_VALUE	(wChildren[10])
#define SEPARATOR2	(wChildren[11])
#define NUM_CHILDREN	12

/*
 * main - main logic for program
 */
void main (argc,argv)
    int argc;
    char **argv;
{
    Widget frame;
    Widget form;
    Widget  top;
    XtAppContext app_context;
    Arg	args[10];
    int n;
    int i;
    XmString xms, xmsBlank;

    /*  
     * initialize toolkit  
     */
    n = 0;
    XtSetArg (args[n], XmNallowShellResize, True);  n++;
    toplevel = XtAppInitialize (&app_context, "Dtwsinfo", NULL, 0, &argc, argv,
								NULL, args, n);

    /* 
     * Set up X error handling
     */
    InstallXErrorHandler ();

    n = 0;
    XtSetArg (args[n], XmNmarginWidth, 10);  n++;
    XtSetArg (args[n], XmNmarginHeight, 10);  n++;
    frame = XmCreateFrame (toplevel, "frame", args, n);
    XtManageChild (frame);

    /*  
     * manager widget 
     */
    n = 0;
    form = XmCreateForm (frame, "form", args, n);
    XtManageChild (form);

    /*  
     * create compound string for blank labels
     */
    xmsBlank = XmStringCreateLocalized("   ");

    /*******************************************
     * QUIT BUTTON
     ******************************************/

    /*  
     * create compound string for the button text  
     */
    xms = XmStringCreateLocalized("Quit");

    /*  
     * set up arglist  
     */
    n = 0;
    XtSetArg (args[n], XmNlabelType, XmSTRING);  n++;
    XtSetArg (args[n], XmNlabelString, xms);  n++;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    /*  
     * create button  
     */
    top = QUIT_BUTTON = XtCreateWidget ("button", xmPushButtonWidgetClass,
	  form, args, n);
    XmStringFree(xms);

    /*  
     * add callback  
     */
    XtAddCallback (QUIT_BUTTON, XmNactivateCallback, quitCB, NULL);

    /*******************************************
     * UPDATE WORKSPACE INFO BUTTON
     ******************************************/

    /*  
     * create compound string for the button text  
     */
    xms = XmStringCreateLocalized("Get workspace info");

    /*  
     * set up arglist  
     */
    n = 0;
    XtSetArg (args[n], XmNlabelType, XmSTRING);  n++;
    XtSetArg (args[n], XmNlabelString, xms);  n++;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg (args[n], XmNtopWidget, top); n++;
    /*  
     * create button  
     */
    top = UPDATE_BUTTON = XtCreateWidget ("button", xmPushButtonWidgetClass,
	  form, args, n);
    XmStringFree(xms);

    /*  
     * add callback  
     */
    XtAddCallback (UPDATE_BUTTON, XmNactivateCallback, updateCB, NULL);

    /*******************************************
     * SEPARATOR
     ******************************************/

    /*  
     * set up arglist  
     */
    n = 0;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg (args[n], XmNtopWidget, top); n++;
    /*  
     * create separator  
     */
    top = SEPARATOR = (Widget) 
	    XmCreateSeparatorGadget (form, "separator", args, n);

    /*******************************************
     * LABEL FOR CURRENT WORKSPACE
     ******************************************/

    /*
     * NOTE: We assume this is the longest label and lay out the
     *       other fields based on this assumption.
     */

    /*  
     * create compound string for the label text  
     */
    xms = XmStringCreateLocalized("Current workspace: ");

    /*  
     * set up arglist  
     */
    n = 0;
    XtSetArg (args[n], XmNlabelType, XmSTRING);  n++;
    XtSetArg (args[n], XmNlabelString, xms);  n++;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg (args[n], XmNtopWidget, top); n++;
    /*  
     * create label  
     */
    WS_LABEL = XtCreateWidget ("label", xmLabelWidgetClass,
	  form, args, n);
    XmStringFree (xms);

    /*******************************************
     * VALUE FOR CURRENT WORKSPACE
     ******************************************/

    /*  
     * set up arglist  
     */
    n = 0;
    XtSetArg (args[n], XmNlabelType, XmSTRING);  n++;
    XtSetArg (args[n], XmNlabelString, xmsBlank);  n++;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
    XtSetArg (args[n], XmNleftWidget, WS_LABEL); n++;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg (args[n], XmNtopWidget, top); n++;
    /*  
     * create value
     */
    top = WS_VALUE = XtCreateWidget ("label", xmLabelWidgetClass,
	  form, args, n);

    /*******************************************
     * LABEL FOR WORKSPACE TITLE
     ******************************************/

    /*  
     * create compound string for the label text  
     */
    xms = XmStringCreateLocalized("    title: ");

    /*  
     * set up arglist  
     */
    n = 0;
    XtSetArg (args[n], XmNlabelType, XmSTRING);  n++;
    XtSetArg (args[n], XmNlabelString, xms);  n++;
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg (args[n], XmNrightWidget, WS_LABEL); n++;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg (args[n], XmNtopWidget, top); n++;
    /*  
     * create label  
     */
    TITLE_LABEL = XtCreateWidget ("label", xmLabelWidgetClass,
	  form, args, n);
    XmStringFree(xms);

    /*******************************************
     * VALUE FOR WORKSPACE TITLE
     ******************************************/

    /*  
     * set up arglist  
     */
    n = 0;
    XtSetArg (args[n], XmNlabelType, XmSTRING);  n++;
    XtSetArg (args[n], XmNlabelString, xmsBlank);  n++;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
    XtSetArg (args[n], XmNleftWidget, TITLE_LABEL); n++;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg (args[n], XmNtopWidget, top); n++;
    /*  
     * create value
     */
    top = TITLE_VALUE = XtCreateWidget ("label", xmLabelWidgetClass,
	  form, args, n);

    /*******************************************
     * LABEL FOR WORKSPACE BACKDROP
     ******************************************/

    /*  
     * create compound string for the label text  
     */
    xms = XmStringCreateLocalized("    backdrop: ");

    /*  
     * set up arglist  
     */
    n = 0;
    XtSetArg (args[n], XmNlabelType, XmSTRING);  n++;
    XtSetArg (args[n], XmNlabelString, xms);  n++;
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg (args[n], XmNrightWidget, WS_LABEL); n++;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg (args[n], XmNtopWidget, top); n++;
    /*  
     * create label  
     */
    BACKDROP_LABEL = XtCreateWidget ("label", xmLabelWidgetClass,
	  form, args, n);
    XmStringFree (xms);

    /*******************************************
     * VALUE FOR WORKSPACE BACKDROP
     ******************************************/

    /*  
     * set up arglist  
     */
    n = 0;
    XtSetArg (args[n], XmNlabelType, XmSTRING);  n++;
    XtSetArg (args[n], XmNlabelString, xmsBlank);  n++;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
    XtSetArg (args[n], XmNleftWidget, BACKDROP_LABEL); n++;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg (args[n], XmNtopWidget, top); n++;
    /*  
     * create value
     */
    top = BACKDROP_VALUE = XtCreateWidget ("label", xmLabelWidgetClass,
	  form, args, n);

    /*******************************************
     * LABEL FOR WORKSPACE COLORSET ID
     ******************************************/

    /*  
     * create compound string for the label text  
     */
    xms = XmStringCreateLocalized("    color set id: ");

    /*  
     * set up arglist  
     */
    n = 0;
    XtSetArg (args[n], XmNlabelType, XmSTRING);  n++;
    XtSetArg (args[n], XmNlabelString, xms);  n++;
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg (args[n], XmNrightWidget, WS_LABEL); n++;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg (args[n], XmNtopWidget, top); n++;
    /*  
     * create label  
     */
    COLORSET_LABEL = XtCreateWidget ("label", xmLabelWidgetClass,
	  form, args, n);
    XmStringFree (xms);

    /*******************************************
     * VALUE FOR WORKSPACE COLORSET ID
     ******************************************/

    /*  
     * set up arglist  
     */
    n = 0;
    XtSetArg (args[n], XmNlabelType, XmSTRING);  n++;
    XtSetArg (args[n], XmNlabelString, xmsBlank);  n++;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
    XtSetArg (args[n], XmNleftWidget, COLORSET_LABEL); n++;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg (args[n], XmNtopWidget, top); n++;
    /*  
     * create value
     */
    top = COLORSET_VALUE = XtCreateWidget ("label", xmLabelWidgetClass,
	  form, args, n);

    /*******************************************
     * SEPARATOR
     ******************************************/

    /*  
     * set up arglist  
     */
    n = 0;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg (args[n], XmNtopWidget, top); n++;
    /*  
     * create separator  
     */
    top = SEPARATOR2 = (Widget) 
	    XmCreateSeparatorGadget (form, "separator", args, n);

    /*  
     * manage children
     */
    XtManageChildren (wChildren, NUM_CHILDREN);

    /* 
     * Show info on current workspace
     */
    ShowCurrentWorkspaceInfo ();

    /*  
     * realize widgets  
     */
    XtRealizeWidget (toplevel);

    /*
     * Do these calls after the shell is realized.  
     * They need a window created for the top level shell.
     *
     * HINT: See XtSetMappedWhenManaged(1) if you want to 
     *       realize a widget to get a window, but need to
     *       control when the map of the window occurs.
     */
    (void) DtWsmAddCurrentWorkspaceCallback (toplevel,
		(DtWsmWsChangeProc) wschangecb, NULL);

    DtWsmOccupyAllWorkspaces (XtDisplay(toplevel), XtWindow(toplevel));

    /*  
     * process events  
     */
    XtAppMainLoop (app_context);
}


/*-------------------------------------------------------------
 *	Update the information for the current workspace
 */
static void ShowCurrentWorkspaceInfo ()
{
    Arg	args[10]; /*  arg list		*/
    int n;	/*  arg count		*/
    Atom aWs;
    int rval;
    Display *dpy;
    Window root;
    DtWsmWorkspaceInfo *pWsInfo;
    XmString xms;
    char pch[512];
    char *pchName;

    dpy = XtDisplay(toplevel);
    root = XDefaultRootWindow(dpy);

    rval = DtWsmGetCurrentWorkspace(dpy, root, &aWs);

    if (rval == Success)
    {
	rval = DtWsmGetWorkspaceInfo (dpy, root, aWs, &pWsInfo);

	if (rval == Success)
	{
	    /* workspace name */
	    pchName = XmGetAtomName (dpy, pWsInfo->workspace);
	    xms = XmStringCreateLocalized(pchName);

	    n = 0;
	    XtSetArg (args[n], XmNlabelString, xms);  n++;
	    XtSetValues (WS_VALUE, args, n);

	    XmStringFree (xms);
	    XtFree (pchName);

	    /* workspace title */
	    xms = XmStringCreateLocalized(pWsInfo->pchTitle);

	    n = 0;
	    XtSetArg (args[n], XmNlabelString, xms);  n++;
	    XtSetValues (TITLE_VALUE, args, n);

	    XmStringFree (xms);

	    /* workspace backdrop name */
	    if (pWsInfo->backdropName != None)
		pchName = XmGetAtomName (dpy, pWsInfo->backdropName);
	    else
		pchName = "<failed to load!>";
	    xms = XmStringCreateLocalized(pchName);

	    n = 0;
	    XtSetArg (args[n], XmNlabelString, xms);  n++;
	    XtSetValues (BACKDROP_VALUE, args, n);

	    XmStringFree (xms);
	    if (pWsInfo->backdropName != None)
		XtFree (pchName);

	    /* workspace colorset id */
	    sprintf (pch, "%d", pWsInfo->colorSetId);
	    xms = XmStringCreateLocalized(pch);

	    n = 0;
	    XtSetArg (args[n], XmNlabelString, xms);  n++;
	    XtSetValues (COLORSET_VALUE, args, n);

	    XmStringFree (xms);

	    DtWsmFreeWorkspaceInfo (pWsInfo);
	}
    }

    if (rval != Success)
    {
	fprintf (stderr, "Failed to get workspace information.\n");
	fprintf (stderr, "Is dtwm running?\n");
	exit (1);
    }
}

/*-------------------------------------------------------------
 *	quitCB - callback for button
 */
static void quitCB (w, client_data, call_data) 
Widget	w;		/*  widget id		*/
XtPointer	client_data;	/*  data from application   */
XtPointer	call_data;	/*  data from widget class  */
{
    exit (0);
}


/*-------------------------------------------------------------
 *	updateCB - callback for show current workspace info
 */
static void updateCB (w, client_data, call_data) 
Widget	w;		/*  widget id		*/
XtPointer	client_data;	/*  data from application   */
XtPointer	call_data;	/*  data from widget class  */
{

    ShowCurrentWorkspaceInfo ();

}

/*-------------------------------------------------------------
**      wschangecb - callback for workspace switch
*/
static DtWsmWsChangeProc
wschangecb (w, atom, client_data)
Widget  w;
Atom    atom;
XtPointer client_data;
{
    ShowCurrentWorkspaceInfo ();
}



/*-------------------------------------------------------------
**      data for X error handler
*/
#define E_MAJOR_CODE            0
#define E_MINOR_CODE            1
#define E_RESOURCE_ID           2
#define E_ERROR_SERIAL          3
#define E_CURRENT_SERIAL        4

#define NUM_E_STRINGS           5

static char *pchErrorFormatNames [NUM_E_STRINGS] = {
    "MajorCode",
    "MinorCode",
    "ResourceID",
    "ErrorSerial",
    "CurrentSerial"
};

static char *pchErrorFormat [NUM_E_STRINGS];


/*-------------------------------------------------------------
**      HandleXErrors - catch X protocol errors
*/
static int
_HandleXErrors (display, errorEvent)
Display  *display;
XErrorEvent *errorEvent;
{
    char buffer[BUFSIZ];
    char message[BUFSIZ];

    XGetErrorText (display, errorEvent->error_code, buffer, BUFSIZ);
    fprintf (stderr, "\n*** X error occurred ***\n");
    fprintf (stderr, "Description = '%s'\n  ", buffer);

    fprintf (stderr, pchErrorFormat[E_MAJOR_CODE], errorEvent->request_code);
    sprintf(message, "%d", errorEvent->request_code);
    XGetErrorDatabaseText (display, "XRequest", message, " ", buffer, BUFSIZ);
    fprintf (stderr, " (%s)\n  ", buffer);
    fprintf (stderr, pchErrorFormat[E_MINOR_CODE], errorEvent->minor_code);
    fprintf (stderr, "\n  ");
    fprintf (stderr, pchErrorFormat[E_RESOURCE_ID], errorEvent->resourceid);
    fprintf (stderr, "\n  ");
    fprintf (stderr, pchErrorFormat[E_ERROR_SERIAL], errorEvent->serial);
    fprintf (stderr, "\n  ");
    fprintf (stderr, pchErrorFormat[E_CURRENT_SERIAL],
                        LastKnownRequestProcessed(display));
    fprintf (stderr, "\n");

    return (0);
}

/*-------------------------------------------------------------
**	Setup the X error handler
*/
static void
InstallXErrorHandler ( display )
Display *display;
{
    char buffer[BUFSIZ];
    int i;

    /*
     * Fetch the X error format strings from XErrorDB
     */
    for (i = 0; i< NUM_E_STRINGS; i++)
    {
        XGetErrorDatabaseText (display, "XlibMessage",
                    pchErrorFormatNames[i], " %d ", buffer, BUFSIZ);

        pchErrorFormat[i] = (char *) XtMalloc (1+strlen(buffer));

        strcpy(pchErrorFormat[i], buffer);
    }

    XSetErrorHandler (_HandleXErrors);
}
