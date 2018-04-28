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
/* $XConsortium: Print.c /main/16 1996/11/11 10:56:18 drk $ */

#include "PrintDemo.h"

/*
 * static function declarations
 */
static void PrintCB(Widget, XtPointer, XtPointer);
static void DoPrint(Widget widget, AppPrintData * p) ;
static void StartJobCB(Widget, XtPointer, XtPointer);
static void Print(AppPrintData *p);
static void PrintCloseDisplayCB(Widget, XtPointer, XtPointer);
static void PageSetupCB(Widget, XtPointer, XtPointer);
static void PdmNotifyCB(Widget, XtPointer, XtPointer);
static void PrintSetupCB(Widget, XtPointer, XtPointer);
static void FinishPrintToFile(Display*, XPContext, XPGetDocStatus, XPointer);
static void CreatePrintShell(Widget, AppPrintData*);

/*
 * ------------------------------------------------------------------------
 * Name: PdmNotifyCB
 *
 * Description:
 *
 *     Called when the PDM is up, or down.
 *
 */
static void 
PdmNotifyCB(Widget pr_shell, XtPointer client_data, XtPointer call_data)
{
    XmPrintShellCallbackStruct* pr_cbs = 
	(XmPrintShellCallbackStruct*) call_data;
    AppPrintData * p = (AppPrintData *) client_data ;
    
    if (pr_cbs->reason == XmCR_PDM_NONE) {
	/* put out a real message dialog */
	printf("No PDM found in the environment\n");
    } else 
    if (pr_cbs->reason == XmCR_PDM_START_ERROR) {
	/* put out a real message dialog */
	printf("Cannot start the PDM\n");
    } else 
    if (pr_cbs->reason == XmCR_PDM_START_VXAUTH) {
	/* put out a real message dialog */
	printf("PDM is not authorized to connect to Video display\n");
    } else 
    if (pr_cbs->reason == XmCR_PDM_START_PXAUTH) {
	/* put out a real message dialog */
	printf("PDM is not authorized to connect to Print display\n");
    }
 
}

/*
 * ------------------------------------------------------------------------
 * Name: PrintMenuCB
 *
 * Description:
 *
 *     Called when the user selects the "Print..." menu item.
 *
 */
void 
PrintMenuCB(Widget pr_button, XtPointer client_data, XtPointer call_data)
{
    AppPrintData* p = (AppPrintData*)client_data;
    
    /* only propose a new print job if one is not already running 
       shouldn't happen since we put the button insensitive */
    if (!p->printed_lines) {
	CreatePrintSetup(pr_button, p);

	XtManageChild(p->print_dialog); /* popup dialog each time */

    } else {
	/* real dialog here */
	printf("Print job already running\n");
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: CreatePrintShell
 *
 * Description:
 *
 *     Called when the user selects the "Print" or the "Setup..." button
 *       in the setupbox.
 *
 */

static void CreatePrintShell(Widget widget, AppPrintData* p)
{
    /*
     * create a print_shell if none available.  the print dialog callback
     * always provides valid printer context and print display initialized: 
     * XpInitContext called, attributes set.
     */

    if (!p->print_shell) {
	p->print_shell = 
	    XmPrintSetup(widget, 
			 XpGetScreenOfContext(p->print_data->print_display,
					      p->print_data->print_context),
			 "Print", NULL, 0);

	XtAddCallback(p->print_shell, XmNpageSetupCallback, 
		      PageSetupCB, (XtPointer)p);
	XtAddCallback(p->print_shell, XmNpdmNotificationCallback, 
		      PdmNotifyCB, (XtPointer)p);
    }
}


 /*
 * ------------------------------------------------------------------------
 * Name: PrintSetupCB
 *
 * Description:
 *
 *     Called when the user presses the setup box "Setup..." button.
 *
 */
static void 
PrintSetupCB(Widget print_dialog, XtPointer client_data, XtPointer call_data)
{
    AppPrintData *p = (AppPrintData*)client_data;
    DtPrintSetupCallbackStruct *pbs = (DtPrintSetupCallbackStruct*)call_data;
    int copies ;

    XtVaGetValues(print_dialog, DtNcopies, &copies, NULL);
    if (copies == 3) {
	String attr ;
	Display * pdpy = pbs->print_data->print_display ;
	XPContext pcontext = pbs->print_data->print_context ;

	attr = XpGetAttributes (pdpy, pcontext,  XPPageAttr);
	if (attr) printf ("XPPageAttr:\n%s\n----------------------\n", attr);
	attr = XpGetAttributes (pdpy, pcontext,  XPDocAttr);
	if (attr) printf ("XPDocAttr:\n%s\n----------------------\n", attr);
	attr = XpGetAttributes (pdpy, pcontext, XPJobAttr);
	if (attr) printf ("XPJobAttr:\n%s\n----------------------\n", attr);
	attr = XpGetAttributes (pdpy, pcontext,  XPPrinterAttr);
	if (attr) printf ("XPPrinterAttr:\n%s\n----------------------\n", attr);
	attr = XpGetAttributes (pdpy, pcontext,  XPServerAttr);
	if (attr) printf ("XPServerAttr:\n%s\n----------------------\n", attr);
	
	return ;
    }

   /* copy the setup data into our space */
    if (p->print_data->print_display != NULL)
	DtPrintFreeSetupData(p->print_data);
    DtPrintCopySetupData(p->print_data, pbs->print_data);
 
    /* create a print shell if not already done */
    CreatePrintShell(print_dialog, p);

    /* pop up the PDM */
    if (XmPrintPopupPDM(p->print_shell, XtParent(print_dialog)) 
	!= XmPDM_NOTIFY_SUCCESS) {
	/* post a message error dialog */
	printf("XmPrintPopupPDM failed\n");
    }

    /* Free the setup data - use fresh data when Print button pressed. */
    DtPrintFreeSetupData(p->print_data);
}

static void 
CancelCB(Widget print_dialog, XtPointer client_data, XtPointer call_data)
{
    AppPrintData *p = (AppPrintData*)client_data;
    DtPrintSetupCallbackStruct *pbs = (DtPrintSetupCallbackStruct*)call_data;

    /* mostly to try it out */
    XtDestroyWidget(print_dialog); p->print_dialog = NULL ; 
}

/*
 * ------------------------------------------------------------------------
 * Name: CreatePrintSetup
 *
 * Description:
 *
 *     Creates a DtPrintSetupBox dialog.
 *
 */
void
CreatePrintSetup(Widget parent, AppPrintData* p)
{
    /*
     * only create one PrintSetupBox 
     */
    if(!p->print_dialog)
    {
	Cardinal n = 0;
	Arg args[5];

	/* can be called when print_only is up, which means no need
	   for a dialog */
	if(XtIsApplicationShell(parent))
	    p->print_dialog =
		DtCreatePrintSetupBox(parent, "PrintSetup", NULL, 0);
	else
	{
	    XmString title = XmStringCreateLocalized("Print");
	    XtSetArg(args[n], XmNdialogTitle, title); n++;
	    p->print_dialog =
		DtCreatePrintSetupDialog(parent, "PrintSetup", args, n);
	    XmStringFree(title);
	}
	/*
	 * allow the application to customize the print setup box
	 */
	AppObject_customizePrintSetupBox(p->app_object, p->print_dialog);
        /*
         * add typically used callbacks
         */
        XtAddCallback(p->print_dialog, DtNclosePrintDisplayCallback,
                      PrintCloseDisplayCB, p);
        XtAddCallback(p->print_dialog, DtNsetupCallback,
                      PrintSetupCB, p);
         XtAddCallback(p->print_dialog, DtNprintCallback,
                      PrintCB, p);
        XtAddCallback(p->print_dialog, DtNcancelCallback,
                      CancelCB, p);
	/*
	 * other callbacks, for attributes management, are available 
	 */
    }
}


/*
 * ------------------------------------------------------------------------
 * Name: PrintCB
 *
 * Description:
 *
 *     Called when the user presses the setup box "Print" button.
 *
 */
static void 
PrintCB(Widget print_dialog, XtPointer client_data, XtPointer call_data)
{
    AppPrintData *p = (AppPrintData*)client_data;
    DtPrintSetupCallbackStruct *pbs = (DtPrintSetupCallbackStruct*)call_data;

    /*
     * get the new printer data from the DtPrintSetupBox, and copy it
     * into our AppPrint data
     */
    if (p->print_data->print_display != NULL)
	 DtPrintFreeSetupData(p->print_data);
    DtPrintCopySetupData(p->print_data, pbs->print_data);
 
    DoPrint(print_dialog, p);
}

/*
 * ------------------------------------------------------------------------
 * Name: QuickPrintCB
 *
 * Description:
 *
 *     Called when the user hits "Print" quick button.
 */
void 
QuickPrintCB(Widget pr_button, XtPointer client_data, XtPointer call_data)
{
    AppPrintData *p = (AppPrintData*)client_data;

    CreatePrintSetup(pr_button, p);

    /*
     * check if the DtPrintSetupBox ("Print...") has been called yet 
     */
    if(p->print_data->print_display == NULL)
    {
        /*
         * first time thru print setup, so get default data 
         */
        if (DtPrintFillSetupData(p->print_dialog, p->print_data)
	    != DtPRINT_SUCCESS) {
	    /* post some message error dialog */
	    printf("DtPrintFillSetupData failed\n");
	    return ;
	}
    }
 
    DoPrint(pr_button, p) ;
}

/*
 * ------------------------------------------------------------------------
 * Name: FinishPrintToFile
 *
 * Description:
 *
 *     App-specific print data holder allocate function.
 *
 */
static void FinishPrintToFile(Display *display,
			      XPContext context,
			      XPGetDocStatus status,
			      XPointer client_data)
{
    if (status != XPGetDocFinished)
	/* put out a real message dialog */
	printf("Something went wrong with XmPrintToFile...\n");
    else 
	printf("XmPrintToFile completed OK\n");
}


/*
 * ------------------------------------------------------------------------
 * Name: DoPrint
 *
 * Description:
 *     
 *     Routine used from the "Print" button and from the OK button of the
 *     "Print..." dialog.
 *
 */
static void 
DoPrint(Widget widget, AppPrintData * p) 
{
    int save_data = XPSpool;

    /* create print shell, if not done yet */
    CreatePrintShell(widget, p);

    if (p->print_data->destination == DtPRINT_TO_FILE)
	save_data = XPGetData;

    /* start job must precede XpGetDocumentData in XmPrintToFile */
    XpStartJob(XtDisplay(p->print_shell), save_data);  

    /* setup print to file */
    if (p->print_data->destination == DtPRINT_TO_FILE)
    {
        if (!XmPrintToFile(XtDisplay(p->print_shell), 
			   p->print_data->dest_info, FinishPrintToFile, NULL))
	{
	    /* Add real error message here. */
	    printf("XmPrintToFile: Unable to print to file %s\n",
		   p->print_data->dest_info);

	    XpCancelJob(XtDisplay(p->print_shell), False);

	    /* we can go back to the event loop as if we had never
	       printed */
	    return;
	}
    }

    XtSetSensitive(p->pr_button, False);
}

/*
 * ------------------------------------------------------------------------
 * Name: PageSetupCB
 *
 * Description:
 *
 *     Called when the print shell receives the XP events.
 *
 */
static void 
PageSetupCB(Widget widget, XtPointer client_data, XtPointer call_data)
{
    Widget pshell = widget ;
    XmPrintShellCallbackStruct* pr_cbs = 
	(XmPrintShellCallbackStruct*) call_data;
    AppPrintData * p = (AppPrintData *) client_data ;

    /* could have real indicator of progress here */
    printf("Printed Lines %d\n", p->printed_lines);

    /* the first time around, create a print text widget and get
       line info - equivalent for testing for first page*/
    if (!pr_cbs->last_page && !p->printed_lines) {

	/* create the widgets once */
	if (!p->pform) {
	    /* create a form widget with some fixed margins */
	    p->pform = XtVaCreateManagedWidget("pform", xmFormWidgetClass, 
					       pshell, NULL);
	    /* create a text widget */
	    p->ptext = XtVaCreateManagedWidget("ptext", xmTextWidgetClass, 
					       p->pform, NULL);
	}
	/* transfer value from file buffer to print text widget */
	XmTextSetString(p->ptext, p->app_object->file_buffer );

	/* get lines per page and total lines */
	XtVaGetValues(p->ptext, XmNrows, &(p->lines_per_page), 
		      XmNtotalLines, &(p->total_lines), NULL);

	p->printed_lines += p->lines_per_page ;

	/* If I'm already done: fit in one page, set last_page up */
	if (p->printed_lines >= p->total_lines) 
	    pr_cbs->last_page = True ;
	/* that will have for effect in the shell to start a page, end it, 
	   and then end the job */
	return ;
    }

    /* if not the first page - see previous test, and not the last
       scroll for next page */
    if (!pr_cbs->last_page) {
	XmTextScroll(p->ptext, p->lines_per_page);
	p->printed_lines += p->lines_per_page ;
	/* if last page, say it */
	if (p->printed_lines >= p->total_lines) pr_cbs->last_page = True ;
    } else {
	/* job done. reset our counter, and keep print shell around
	   for next print job, just pop it down 
	   reset the Print... button sensitive */
	XtPopdown(pshell);
	p->printed_lines = 0 ;	
	XtSetSensitive(p->pr_button, True);

    }

    
}

/*
 * ------------------------------------------------------------------------
 * Name: PrintCloseDisplayCB
 *
 * Description:
 *
 *     Called when the print setup box is about to close the print
 *     display (in response to a new printer on a different display, or
 *     when the setup box is destroyed, or from DtPrintResetConnection).
 */
static void
PrintCloseDisplayCB(
		    Widget widget,
		    XtPointer client_data,
		    XtPointer call_data)
{
    DtPrintSetupCallbackStruct *pbs = (DtPrintSetupCallbackStruct*)call_data;
    AppPrintData *p = (AppPrintData*)client_data;

    if (p->print_shell)
    {
        XtDestroyWidget(p->print_shell);
        p->print_shell = (Widget)NULL ;
	
	/* must remember that the children are gone, as well */
	p->ptext = p->pform = NULL;
    }

    DtPrintFreeSetupData(p->print_data); 
    /* that nulls out p->print_data->print_display */
}

/*
 * ------------------------------------------------------------------------
 * Name: AppPrintData_new
 *
 * Description:
 *
 *     App-specific print data holder allocate function.
 *
 */
AppPrintData*
AppPrintData_new()
{
    AppPrintData* p = (AppPrintData*)XtCalloc(1, sizeof(AppPrintData));
    p->print_data = (DtPrintSetupData*)XtCalloc(1, sizeof(DtPrintSetupData));

    return p;
}

