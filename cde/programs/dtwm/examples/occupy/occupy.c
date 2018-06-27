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
/* $XConsortium: occupy.c /main/3 1996/11/05 12:17:48 mustafa $ */
/***********************************************************************
 *  CDE Developer's Conference Sample Program
 *
 *  File: occupy.c
 *
 *  Purpose:
 *      Demonstrate the CDE Workspace API. This file uses:
 *
 *	    DtWsmGetWorkspacesOccupied
 *	    DtWsmSetWorkspacesOccupied
 *	    DtWsmOccupyAllWorkspaces
 *
 *
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company	
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 *  (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of
 *      Novell, Inc.
 *  ALL RIGHTS RESERVED
 *
 ***********************************************************************/

/*  
 * include files  
 */

#include <stdio.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/TextF.h>

#include <Dt/Wsm.h>


/*  
 * functions defined in this program  
 */
void main();
void quitCB(); 
void setCB(); 
void showCB(); 
void wsCB(); 
void allWsCB(); 
void SetupWorkspaceButtons ();
void ShowWorkspaceOccupancy();
void SetWorkspaceOccupancy();


/*  
 * global variables  
 */
XmString xms;	/* button label pointer for compound string */
Widget	toplevel; /*  Shell widget	*/
Widget wChildren [20];
Widget *wWs;
Atom *paWsSet = NULL;
Boolean *pbOccupy;
unsigned long numWorkspaces;
Atom *paWs;

/* 
 * button assignments 
 */
#define QUIT_BUTTON	(wChildren[0])
#define SEPARATOR	(wChildren[1])
#define SET_BUTTON      (wChildren[2])
#define SHOW_BUTTON     (wChildren[3])
#define ALL_WS_BUTTON   (wChildren[4])
#define NUM_CHILDREN	5

/*-------------------------------------------------------------
 *	main - main logic for program
 */
void main (argc,argv)
int argc;
char **argv;
{
    Widget	form;
    Widget  top;
    XtAppContext app_context;
    Arg	args[10];
    int n;
    int i;

    /*  
     * initialize toolkit  
     */
    n = 0;
    XtSetArg (args[n], XmNallowShellResize, True);  n++;
    toplevel = XtAppInitialize (&app_context, "XMDemos", NULL, 0, 
	    &argc, argv, NULL, args, n);

    /*  
     * manager widget 
     */
    n = 0;
    form = XmCreateForm (toplevel, "form", args, n);
    XtManageChild (form);

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
    /*  
     * add callback  
     */
    XtAddCallback (QUIT_BUTTON, XmNactivateCallback, quitCB, NULL);

    /*  
     * create compound string for the button text  
     */
    xms = XmStringCreateLocalized("Set Occupancy");

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
    top = SET_BUTTON = XtCreateWidget ("button", xmPushButtonWidgetClass,
	  form, args, n);
    /*  
     * add callback  
     */
    XtAddCallback (SET_BUTTON, XmNactivateCallback, setCB, NULL);

    /*  
     * create compound string for the button text  
     */
    xms = XmStringCreateLocalized("Occupy All Workspaces");

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
    top = ALL_WS_BUTTON = XtCreateWidget ("button", xmPushButtonWidgetClass,
	  form, args, n);
    /*  
     * add callback  
     */
    XtAddCallback (ALL_WS_BUTTON, XmNactivateCallback, allWsCB, NULL);

    /*  
     * create compound string for the button text  
     */
    xms = XmStringCreateLocalized("Show Occupancy");

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
    top = SHOW_BUTTON = XtCreateWidget ("button", xmPushButtonWidgetClass,
      form, args, n);
    /*  
     * add callback  
     */
    XtAddCallback (SHOW_BUTTON, XmNactivateCallback, showCB, NULL);

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

    /* 
     * set up workspace buttons 
     */
    SetupWorkspaceButtons (form, top);

    /*  
     * manage children
     */
    XtManageChildren (wChildren, NUM_CHILDREN);
    XtManageChildren (wWs, numWorkspaces);

    /*  
     * realize widgets  
     */
    XtRealizeWidget (toplevel);

    /* 
     * set up initial workspace occupancy info 
     */
    pbOccupy = (Boolean *) XtMalloc (numWorkspaces * sizeof (Boolean));
    for (i=0; i<numWorkspaces; i++)
    {
	pbOccupy[i] = False;
    }
    ShowWorkspaceOccupancy ();

    /*  
     * process events  
     */
    XtAppMainLoop (app_context);
}

/*-------------------------------------------------------------
 *	Create a set of buttons for the current set of workspaces
 */
void SetupWorkspaceButtons (form, top)
Widget form;
Widget top;
{
    Window root;
    Arg	args[10]; /*  arg list		*/
    int n;	/*  arg count		*/
    int i;
    DtWsmWorkspaceInfo *pWsInfo;
    char *pchWs;
    Widget left = None;
    Widget *pWid;

    /*  
     * get workspace information 
     */
    root = XRootWindowOfScreen(XtScreen(form));
    if (DtWsmGetWorkspaceList (XtDisplay(form), root, 
		    &paWs, (int *)&numWorkspaces) == Success)
    {
	wWs = (Widget *) XtMalloc (numWorkspaces * sizeof (Widget));
	pWid = wWs;
	for (i=0; i<numWorkspaces; i++, pWid++)
	{
	    DtWsmGetWorkspaceInfo (XtDisplay(form), root, paWs[i],
		&pWsInfo);

	    pchWs = (char *) XmGetAtomName (XtDisplay(form), 
					    pWsInfo->workspace);
	    /*  
	     * create compound string for the button text  
	     */
	    xms = XmStringCreateLocalized(pchWs);

	    /*  
	     * set up arglist  
	     */
	    n = 0;
	    XtSetArg (args[n], XmNlabelType, XmSTRING);  n++;
	    XtSetArg (args[n], XmNlabelString, xms);  n++;
	    XtSetArg (args[n], XmNindicatorOn, True);  n++;
	    XtSetArg (args[n], XmNindicatorType, XmN_OF_MANY);  n++;
	    if (left == None)
	    {
		XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	    }
	    else
	    {
		XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
		XtSetArg (args[n], XmNleftWidget, left); n++;
	    }
	    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	    XtSetArg (args[n], XmNtopWidget, top); n++;
	    /*  
	     * create button  
	     */
	    *pWid = XtCreateWidget ("button", xmToggleButtonWidgetClass,
	      form, args, n);
	    /*  
	     * add callback  
	     */
	    XtAddCallback (*pWid, XmNvalueChangedCallback, wsCB, 
				    (XtPointer) i);
	    left = *pWid;
	}
    }
    else
    {
	fprintf(stderr, "Failed to get workspace info -- dtwm not running!\n");
	exit (1);
    }
}

/*-------------------------------------------------------------
 *	Update the workspace buttons with where we are now.
 */
void ShowWorkspaceOccupancy ()
{
    Arg	args[10]; /*  arg list		*/
    int n;	/*  arg count		*/
    int i,j;
    Atom *paWsIn = NULL;
    unsigned long numWsIn;
    int rval;

    rval = DtWsmGetWorkspacesOccupied(XtDisplay(toplevel), 
		XtWindow(toplevel), &paWsIn, &numWsIn );

    if (rval == Success)
    {

	for (i = 0; i<numWorkspaces; i++)
	{
	    pbOccupy[i] = False;
	    for (j=0; j<numWsIn; j++)
	    {
		if (paWs[i] == paWsIn[j])
		{
		    pbOccupy[i] = True;
		    break;
		}
	    }
	}

	for (i = 0; i<numWorkspaces; i++)
	{
	    n = 0;
	    if (pbOccupy[i])
	    {
		XtSetArg (args[n], XmNset, True);  n++;
	    }
	    else
	    {
		XtSetArg (args[n], XmNset, False);  n++;
	    }
	    XtSetValues (wWs[i], args, n);
	}

	if (paWsIn) XFree ((char *) paWsIn);
    }
}

/*-------------------------------------------------------------
 *	quitCB - callback for button
 */
void quitCB (w, client_data, call_data) 
Widget	w;		/*  widget id		*/
caddr_t	client_data;	/*  data from application   */
caddr_t	call_data;	/*  data from widget class  */
{
    exit (0);
}


/*-------------------------------------------------------------
 *	showCB - callback for show occupancy button
 */
void showCB (w, client_data, call_data) 
Widget	w;		/*  widget id		*/
caddr_t	client_data;	/*  data from application   */
caddr_t	call_data;	/*  data from widget class  */
{

    ShowWorkspaceOccupancy();

}

/*-------------------------------------------------------------
 *	setCB - callback for set occupancy button
 */
void setCB (w, client_data, call_data) 
Widget	w;		/*  widget id		*/
caddr_t	client_data;	/*  data from application   */
caddr_t	call_data;	/*  data from widget class  */
{
    int numSet;
    int i;

    if (!paWsSet)
    {
	paWsSet = (Atom *) XtMalloc (numWorkspaces * sizeof(Atom));
    }

    for (i=0, numSet=0; i<numWorkspaces; i++)
    {
	if (pbOccupy[i])
	{
	    paWsSet[numSet++] = paWs[i];
	}
    }

    DtWsmSetWorkspacesOccupied (XtDisplay(toplevel),
			    XtWindow(toplevel), paWsSet, numSet);

}

/*-------------------------------------------------------------
 *	wsCB - callback for set occupancy button
 */
void wsCB (w, client_data, call_data) 
Widget	w;		/*  widget id		*/
caddr_t	client_data;	/*  data from application   */
caddr_t	call_data;	/*  data from widget class  */
{
    int i = (Atom) client_data; /* ws array index */
    XmToggleButtonCallbackStruct *ptb = 
		(XmToggleButtonCallbackStruct *) call_data;

    if (ptb->reason == XmCR_VALUE_CHANGED)
    {
	if (ptb->set)
	{
	    pbOccupy[i] = True;
	}
	else
	{
	    pbOccupy[i] = False;
	}
    }
}

/*-------------------------------------------------------------
 *	allWsCB - callback for occupy all button
 */
void allWsCB (w, client_data, call_data) 
Widget	w;		/*  widget id		*/
caddr_t	client_data;	/*  data from application   */
caddr_t	call_data;	/*  data from widget class  */
{
    DtWsmOccupyAllWorkspaces (XtDisplay(toplevel), XtWindow(toplevel));
}
