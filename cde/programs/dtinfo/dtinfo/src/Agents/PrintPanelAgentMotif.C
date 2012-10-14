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
/*
 *  $TOG: PrintPanelAgentMotif.C /main/35 1997/08/01 15:17:10 samborn $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 *
 * Copyright (c) 1992 HAL Computer Systems International, Ltd.
 * All rights reserved.  Unpublished -- rights reserved under
 * the Copyright Laws of the United States.  USE OF A COPYRIGHT
 * NOTICE IS PRECAUTIONARY ONLY AND DOES NOT IMPLY PUBLICATION
 * OR DISCLOSURE.
 * 
 * THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE
 * SECRETS OF HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.  USE,
 * DISCLOSURE, OR REPRODUCTION IS PROHIBITED WITHOUT THE
 * PRIOR EXPRESS WRITTEN PERMISSION OF HAL COMPUTER SYSTEMS
 * INTERNATIONAL, LTD. 
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject
 * to the restrictions as set forth in subparagraph (c)(l)(ii)
 * of the Rights in Technical Data and Computer Software clause
 * at DFARS 252.227-7013.
 *
 *          HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.

 *                  1315 Dell Avenue
 *                  Campbell, CA  95008
 * 
 */

//------------- Includes -----------------------------------

#define C_xList
#define L_Support

#define C_TOC_Element
#define C_NodeViewInfo
#define L_Basic

#define C_Printgr
#define C_PrintMgr
#define C_PrefMgr
#define C_MessageMgr
#define L_Managers

#define C_PrintPanelAgent
#define C_HelpAgent
#define L_Agents

#include <X11/IntrinsicP.h>
#include <X11/CoreP.h>

#include <Xm/MessageB.h>
#include <Xm/PushB.h>
#include <Xm/Scale.h>

#include <DtI/DisplayAreaP.h>
#include <DtI/DisplayAreaI.h>

#include <Prelude.h>

#include "Other/XmStringLocalized.hh"
#include "Managers/CatMgr.hh"

#include "Registration.hh"

#include <WWL/WXmDialogShell.h>
#include <WWL/WXmForm.h>
#include <WWL/WXmFrame.h>
#include <WWL/WXmPushButton.h>
#include <WWL/WXmSeparator.h>
#include <sstream>
using namespace std;

#define CLASS PrintPanelAgent

#include "create_macros.hh"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>


#define AM WAutoManage
#define WXmToggleBtn WXmToggleButton
#define WXmPushBtn WXmPushButton

#if 0
#  define RCS_DEBUG(statement) cerr << statement << endl
#else
#  define RCS_DEBUG(statement)
#endif

//-------- External References ---------------------------------

extern Widget footer_book_name;
extern Widget footer_page_no;
extern DtHelpDispAreaStruct *gHelpDisplayArea;

//------------ Global Variables -----------------------------

Widget f_print_shell = NULL;
Widget f_scrollbar = NULL;
Widget f_dialog = NULL;
const char *f_printOrientation = NULL;
Boolean print_server_error = False;

//------------ Private Variables -----------------------------

//  Prefixed with an l to distinguish from f_AppPrintData in 
//  AppPrintData class.  
AppPrintData *	l_AppPrintData; 

static  WXmForm		        f_print_panel;
static  WXmLabel		f_selected_field;
static  WXmLabel		f_to_print_field;
static  WXmToggleButton	        f_print_nodes;
static  WXmToggleButton	        f_print_hierarchy;

static Boolean print_hierarchy;  // keep track of hierarchy vs section

static void PrintEverything(AppPrintData *p);
static void PrintOneUASCommon(UAS_Pointer<UAS_Common> &doc, Widget pshell, int *cur_pageP);

PrintPanelAgent::PrintPanelAgent()
{
    RCS_DEBUG("PrintPanelAgent constructor entered.\n");

    l_AppPrintData = new AppPrintData();
    cout << "overloaded PrintPanelAgenct constructor called.\n";   

    RCS_DEBUG("PrintPanelAgent constructor exiting.\n");   
}

PrintPanelAgent::PrintPanelAgent(AppPrintData *p)
{
    l_AppPrintData = p;
    cout << "overloaded PrintPanelAgenct constructor called.\n";   
}

PrintPanelAgent::~PrintPanelAgent()
{
    cout << "PrintPanelAgenct destructor called.\n";
}

/*
 * ------------------------------------------------------------------------
 * Name:  reset_ui
 *
 * Description:
 *     Resets the number of nodes and what to print buttons.
 *
 * Return value:
 *     None.
 *
 */
void
reset_ui(AppPrintData *p)
{
   RCS_DEBUG("reset_ui called.\n");   

    xList<UAS_Pointer<UAS_Common> > &print_list = *(p->f_print_list);
    Xassert (print_list.length() > 0);
    
    // Set up the "Number of Nodes" labels. 
    static char buffer[24];
    
    sprintf (buffer, "%d", print_list.length());
    f_selected_field.LabelString (WXmString (buffer));
    f_to_print_field.LabelString (WXmString (buffer));
    
    // Set up "What to print" buttons based on command line 
    // value set in Window System 

    print_hierarchy = window_system().videoShell()->hierarchy;

    f_print_hierarchy.Set(print_hierarchy);
    f_print_nodes.Set(!print_hierarchy);
    
    Boolean hierarchy_sensitive = False;
    
    List_Iterator<UAS_Pointer<UAS_Common> > l (print_list);

    // NOTE: Could fail if toc() is NULL. 
    UAS_List<UAS_Common> kids = l.item()->children();
    if ((!strncmp(l.item()->locator(), "mmdb:", 5)) && print_list.length() == 1 && kids.length() > 0) {
	hierarchy_sensitive = True;
    }
    
    // If one is insensitive, the other one must be as well. 
    f_print_nodes.SetSensitive (hierarchy_sensitive);
    f_print_hierarchy.SetSensitive (hierarchy_sensitive);

}


/*
 * ------------------------------------------------------------------------
 * Name: customizePrintSetupBox
 *
 * Description:
 *     Adds application specific items to the passed print setup box.
 *
 * Return value:
 *
 *     None.
 *
 */
void
customizePrintSetupBox(AppPrintData *p)
{
    XmString label;

    Widget print_dialog = p->f_print_dialog;
    RCS_DEBUG("customizePrintSetupBox called.\n");   

    Widget row, how_many_frame, how_many_title, how_many_form, selected_label, print_panel;
    Widget to_print_label, what_frame, what_title, what_form, selected_field, to_print_field;
    Widget print_nodes;

    //  Create the app-specific top work area

    f_print_panel = print_panel = XtVaCreateManagedWidget("workarea",
					    xmFormWidgetClass,
					    print_dialog,
					    XmNorientation, XmHORIZONTAL,
					    NULL);

    // How many?
    
    how_many_frame = XtVaCreateManagedWidget("how_many_frame",
					     xmFrameWidgetClass,
					     print_panel, 
					     XmNleftAttachment, XmATTACH_FORM,
					     XmNrightAttachment, XmATTACH_POSITION,
					     XmNtopAttachment, XmATTACH_FORM,
					     XmNrightPosition, 49,
					     XmNtopOffset, 2,
					     XmNleftOffset, 5,
					     XmNmarginHeight, 2,
					     NULL);

    label = XmStringGenerate(CATGETS(Set_AgentLabel, 205, "Number of Sections"), 
			     NULL, XmCHARSET_TEXT, NULL);

    how_many_title = XtVaCreateManagedWidget("how_many_title",
					     xmLabelWidgetClass,
					     how_many_frame,
					     XmNchildType, XmFRAME_TITLE_CHILD,
					     XmNlabelString, label,
					     NULL);

    XmStringFree(label);

    how_many_form = XtVaCreateManagedWidget("how_many_frame",
					    xmFormWidgetClass,
					    how_many_frame,
					    NULL);

    label = XmStringGenerate(CATGETS(Set_AgentLabel, 206, "Selected:"),
			     NULL, XmCHARSET_TEXT, NULL);

    selected_label = XtVaCreateManagedWidget("selected_label", 
					     xmLabelWidgetClass, 
					     how_many_form, 
					     XmNlabelString, label,
					     XmNtopAttachment, XmATTACH_FORM,
					     XmNleftAttachment, XmATTACH_FORM,
					     XmNleftOffset, 4,
					     XmNtopOffset, 2,
					     NULL);
    XmStringFree(label);

    f_selected_field = selected_field = XtVaCreateManagedWidget("selected_field",
					       xmLabelWidgetClass, 
					       how_many_form, 
					       XmNlabelString, label,
					       XmNalignment, XmALIGNMENT_END,
					       XmNrightAttachment, XmATTACH_FORM,
					       XmNrightOffset, 2,
					       XmNtopAttachment, XmATTACH_FORM,
					       XmNtopOffset, 2,
					       NULL);

    label = XmStringGenerate(CATGETS(Set_AgentLabel, 207, "To Be Printed:"),
			     NULL, XmCHARSET_TEXT, NULL);
    
    to_print_label = XtVaCreateManagedWidget("to_print_label",
					     xmLabelWidgetClass,
					     how_many_form, 
					     XmNlabelString, label,
					     XmNtopAttachment, XmATTACH_WIDGET,
					     XmNtopWidget, selected_label,
					     XmNleftAttachment, XmATTACH_FORM,
					     XmNbottomAttachment, XmATTACH_FORM,
					     XmNleftOffset, 4,
					     XmNbottomOffset, 5,
					     XmNtopOffset, 8, 
					     NULL);
    XmStringFree(label);

    f_to_print_field = to_print_field = XtVaCreateManagedWidget("to_print_field",
					       xmLabelWidgetClass,
					       how_many_form, 
					       XmNalignment, XmALIGNMENT_END,
					       XmNtopAttachment, XmATTACH_WIDGET,
					       XmNtopWidget, selected_field,
					       XmNtopOffset, 8,
					       XmNrightAttachment, XmATTACH_FORM,
					       XmNrightOffset, 2,
					       XmNbottomAttachment, XmATTACH_FORM,
 					       XmNbottomOffset, 5,
					       NULL);

    // What?

    what_frame = XtVaCreateManagedWidget("what_frame",
					 xmFrameWidgetClass,
					 print_panel,
					 XmNleftAttachment, XmATTACH_POSITION,
					 XmNleftPosition, 51,
					 XmNrightAttachment, XmATTACH_FORM,
					 XmNtopAttachment, XmATTACH_FORM,
					 XmNtopOffset, 2,
					 XmNrightOffset, 5,
					 NULL);

    label = XmStringGenerate(CATGETS(Set_AgentLabel, 209, "What to Print"),
			     NULL, XmCHARSET_TEXT, NULL);

    what_title = XtVaCreateManagedWidget("what_title",
					 xmLabelWidgetClass,
					 what_frame,
					 XmNlabelString, label,
					 XmNchildType, XmFRAME_TITLE_CHILD,
					 NULL);
    XmStringFree(label);
    
    what_form = XtVaCreateManagedWidget("what_form",
					xmFormWidgetClass,
					what_frame,
					NULL);

    label = XmStringGenerate(CATGETS(Set_AgentLabel, 210, "Sections"),
			     NULL, XmCHARSET_TEXT, NULL);

    f_print_nodes = print_nodes = XtVaCreateManagedWidget("print_nodes",
					    xmToggleButtonWidgetClass,
					    what_form,
					    XmNlabelString, label,
					    XmNset, TRUE,
					    XmNalignment, XmALIGNMENT_BEGINNING,
					    XmNindicatorType, XmONE_OF_MANY,
					    XmNleftAttachment, XmATTACH_FORM,
					    XmNrightAttachment, XmATTACH_FORM,
					    XmNtopAttachment, XmATTACH_FORM,
					    XmNleftOffset, 4,
					    XmNrightOffset, 4,
					    NULL);

    XmStringFree(label);
    XtAddCallback(print_nodes, XmNvalueChangedCallback, ToggleWhatCB, p);

    label = XmStringGenerate(CATGETS(Set_AgentLabel, 211, "Hierarchy"),
			     NULL, XmCHARSET_TEXT, NULL);
        
    f_print_hierarchy = XtVaCreateManagedWidget("print_hierarchy",
						xmToggleButtonWidgetClass,
						what_form,
						XmNlabelString, label,
						XmNalignment, XmALIGNMENT_BEGINNING,
						XmNindicatorType, XmONE_OF_MANY,
						XmNtopAttachment, XmATTACH_WIDGET,
						XmNtopWidget, print_nodes,
						XmNbottomAttachment, XmATTACH_FORM,
						XmNbottomOffset, 3,
						XmNleftAttachment, XmATTACH_FORM,
						XmNrightAttachment, XmATTACH_FORM,
						XmNleftOffset, 4,
						XmNrightOffset, 4,
						NULL);

    XtAddCallback(f_print_hierarchy, XmNvalueChangedCallback, ToggleWhatCB, p);

    XmStringFree(label);

    RCS_DEBUG("customizePrintSetupBox exiting.\n");   

}

/*
 * ------------------------------------------------------------------------
 * Name: PdmNotifyCB
 *
 * Description:
 *
 *     Called when the PDM is up, or down.
 *
 */
void 
PdmNotifyCB(Widget pr_shell, XtPointer client_data, XtPointer call_data)
{
    RCS_DEBUG("PdmNotifyCB called.\n");   
    char *msg;

    XmPrintShellCallbackStruct* pr_cbs = 
	(XmPrintShellCallbackStruct*) call_data;
    AppPrintData * p = (AppPrintData *) client_data ;
    
    if ((pr_cbs->reason == XmCR_PDM_NONE) ||
	(pr_cbs->reason == XmCR_PDM_START_ERROR) ||
	(pr_cbs->reason == XmCR_PDM_START_VXAUTH) ||
	(pr_cbs->reason == XmCR_PDM_START_PXAUTH))
    {
	msg = CATGETS(Set_PrintPanelAgent, 12, "Print Dialog Manager error - setup failed.");
	message_mgr().error_dialog(msg);
    }
 
    RCS_DEBUG("PdmNotifyCB exiting.\n");   

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
void 
CreatePrintShell(Widget widget, AppPrintData* p)
{
    char buf[BUFSIZ];
    
    RCS_DEBUG("CreatePrintShell called.\n");   

    /*
     * create a print_shell if none available.  the print dialog callback
     * always provides valid printer context and print display initialized: 
     * XpInitContext called, attributes set.
     */
    if (!p->f_print_shell) {
	
	p->f_print_shell = XmPrintSetup(widget, 
					XpGetScreenOfContext(p->f_print_data->print_display,
							     p->f_print_data->print_context),
					(char*)"PrintShell", NULL, 0);


	//  set the global variable to this value so that the create_ui function in 
	//  NodePrintAgent will know what to parent it's form to
	f_print_shell = p->f_print_shell;

	//  set the print display in the window manager so the fontcache can connect
	//  to the correct display
	window_system().setPrintDisplay(f_print_shell);
	
	// XtAddCallback(p->f_print_shell, XmNpageSetupCallback, PageSetupCB, (XtPointer)p);
	XtAddCallback(p->f_print_shell, XmNpdmNotificationCallback, PdmNotifyCB, (XtPointer)p);
	
	// set default print medium if specified on command line
	
	if (window_system().videoShell()->paper_size != NULL) {
	    sprintf(buf, "*default-medium: %s\n", window_system().videoShell()->paper_size);
	    XpSetAttributes(
		p->f_print_data->print_display, 
		p->f_print_data->print_context, 
		XPDocAttr, 
		buf, 
		XPAttrMerge);
	}

    }

    RCS_DEBUG("CreatePrintShell exiting.\n");   

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
void 
PrintSetupCB(Widget print_dialog, XtPointer client_data, XtPointer call_data)
{
    char *msg;

    RCS_DEBUG("PrintSetupCB called.\n");   


    AppPrintData *p = (AppPrintData*)client_data;
    DtPrintSetupCallbackStruct *pbs = (DtPrintSetupCallbackStruct*)call_data;

    // copy the setup data into our space 

    if (p->f_print_data->print_display != NULL)
	DtPrintFreeSetupData(p->f_print_data);
    DtPrintCopySetupData(p->f_print_data, pbs->print_data);

    // create a print shell if not already done 

    CreatePrintShell(print_dialog, p);

    // pop up the PDM 
    if (XmPrintPopupPDM(p->f_print_shell, print_dialog) 
	!= XmPDM_NOTIFY_SUCCESS) {
	/* post a message error dialog */
	msg = CATGETS(Set_PrintPanelAgent, 6, "Unable to display the Print Dialog Manager");
	message_mgr().error_dialog(msg);
	RCS_DEBUG("XmPrintPopupPDM failed\n");
    }	

    // Free the setup data - use fresh data when Print button pressed.
    DtPrintFreeSetupData(p->f_print_data);

    RCS_DEBUG("PrintSetupCB exiting.\n");   
}

/*
 * ------------------------------------------------------------------------
 * Name: CancelCB
 *
 * Description:
 *
 *     Called when the user cancels out of the Print Setup dialog.
 *     If gui-less printing, causes the program to exit.
 *
 */
static void 
CancelCB(Widget print_dialog, XtPointer client_data, XtPointer call_data)
{
	
    // if gui-less command line printing, exit now that job is canceled
    
    if (window_system().videoShell()->print_only) {
	_exit(1);
    }

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
    RCS_DEBUG("CreatePrintSetup called.\n");   

    //  Only create one PrintSetupBox 

    if (p->f_print_dialog == NULL)
    {
	Cardinal n = 0;
	Arg args[10];

	// resources common to silent and noisy printing cases

	XtSetArg(args[n], (char*)DtNcopies, window_system().videoShell()->copies); n++;
	
	// set print to file if file name given 
	if ((char *)window_system().videoShell()->file_name != NULL) {
	    XtSetArg(args[n], (char*)DtNfileName, (char *)window_system().videoShell()->file_name); n++;
	    XtSetArg(args[n], (char*)DtNprintDestination, DtPRINT_TO_FILE); n++;
	}

	// printer name
	if ((char *)window_system().videoShell()->printer != NULL) {
	    XtSetArg(args[n], (char*)DtNprinterName, (char *)window_system().videoShell()->printer); n++;
	}

	// if silent printing we won't need a dialog

	if (window_system().videoShell()->silent) {

	    // if parent widget is an application shell, use it, otherwise use
	    // the grandparent

	    if (XtIsApplicationShell(parent)) {
		p->f_print_dialog =
		    DtCreatePrintSetupBox(parent, (char*)"PrintSetup", args, n);
	    }
	    else {	
		p->f_print_dialog =
		    DtCreatePrintSetupBox(XtParent(parent), (char*)"PrintSetup", args, n);
	    }
	}    

	//  not silent printing so we will need a dialog

	else
	{
	    // additional dialog resources

	    XmString title = XmStringCreateLocalized(
		CATGETS(Set_PrintPanelAgent, 1, "Dtinfo: Print"));

	    XtSetArg(args[n], XmNdialogTitle, title); n++;
	    XtSetArg(args[n], (char*)DtNworkAreaLocation, DtWORK_AREA_BOTTOM); n++;
 
	    // if parent widget is an application shell, use it, otherwise use
	    // the grandparent

	    if (XtIsApplicationShell(parent)) {
		p->f_print_dialog =
		    DtCreatePrintSetupDialog(parent, (char*)"PrintSetup", args, n);
	    }
	    else {
		p->f_print_dialog =
		    DtCreatePrintSetupDialog(XtParent(parent), (char*)"PrintSetup", args, n);
	    }
//	    XmStringFree(title);
	}

	// customize the print setup box

	customizePrintSetupBox(p);
        
	// add  callbacks

        XtAddCallback(p->f_print_dialog, DtNclosePrintDisplayCallback, PrintCloseDisplayCB, 
		      (XtPointer)p);

        XtAddCallback(p->f_print_dialog, DtNsetupCallback,
                      PrintSetupCB, 
		      (XtPointer)p);

        XtAddCallback(p->f_print_dialog, DtNprintCallback, PrintCB, (XtPointer)p);
        XtAddCallback(p->f_print_dialog, DtNcancelCallback, CancelCB, (XtPointer)p);

	// add context help for dialog

	help_agent().add_activate_help(p->f_print_dialog, (char*)"print_panel_help");
    }

    //  update message area
    reset_ui(p);

    RCS_DEBUG("CreatePrintSetup exiting.\n");   

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
void 
PrintCB(Widget print_dialog, XtPointer client_data, XtPointer call_data)
{
    RCS_DEBUG("PrintCB called.\n");   

    AppPrintData *p = (AppPrintData*)client_data;
    DtPrintSetupCallbackStruct *pbs = (DtPrintSetupCallbackStruct*)call_data;

    /*
     * get the new printer data from the DtPrintSetupBox, and copy it
     * into our AppPrint data
     */
    if (p->f_print_data->print_display != NULL)
	 DtPrintFreeSetupData(p->f_print_data);
    DtPrintCopySetupData(p->f_print_data, pbs->print_data);
   
    DoPrint(print_dialog, p);

    RCS_DEBUG("PrintCB exiting.\n");   
}

/*
 * ------------------------------------------------------------------------
 * Name: QuickPrintCB
 *
 * Description:
 *
 *     Called when the user hits "Print" quick button.
 *     not sure of this functions role in the "new order" ???? rCs
 */
void 
QuickPrintCB(Widget pr_button, XtPointer client_data, XtPointer call_data)
{
    char *msg;

    RCS_DEBUG("QuickPrintCB called.\n");   

    AppPrintData *p = (AppPrintData*)client_data;

    CreatePrintSetup(pr_button, p);


    // check if the DtPrintSetupBox ("Print...") has been called yet 

    if(p->f_print_data->print_display == (Display*)NULL)
    {
	
	// first time thru print setup, so get default data 
	
        if (DtPrintFillSetupData(p->f_print_dialog, p->f_print_data)
	    != DtPRINT_SUCCESS) {
	    // NOTE: DtPrintFillSetupData() already posts an error
	    // dialog on failure - no need to post our own.
	    RCS_DEBUG("DtPrintFillSetupData failed\n");
	    return ;
	}
    }
 
    DoPrint(pr_button, p) ;

    RCS_DEBUG("QuickPrintCB exiting.\n");   

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
void 
FinishPrintToFile(Display *display,
		  XPContext context,
		  XPGetDocStatus status,
		  XPointer client_data)
{
    char *msg;

    RCS_DEBUG("FinishPrintToFile called.\n");   


    if (status != XPGetDocFinished) {

	msg = CATGETS(Set_PrintPanelAgent, 8, "Error printing to file");
	message_mgr().error_dialog(msg);

	RCS_DEBUG("Something went wrong with XmPrintToFile...\n");
    }
    else {

	msg = CATGETS(Set_PrintPanelAgent, 9, "Print to file completed.");
	message_mgr().info_dialog(msg);

	RCS_DEBUG("XmPrintToFile completed OK\n");
    }
    RCS_DEBUG("FinishPrintToFile exiting.\n");   

    return;

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
void 
DoPrint(Widget widget, AppPrintData * p) 
{
    int save_data = XPSpool;
    char *msg;

    xList<UAS_Pointer<UAS_Common> > &print_list = *(p->f_print_list);
    Xassert (print_list.length() > 0);    

    RCS_DEBUG("DoPrint called.\n");   

    // create print shell, if not done yet 
    CreatePrintShell(widget, p);

    if (p->f_print_data->destination == DtPRINT_TO_FILE)
    {
	save_data = XPGetData;
    }

    // start job must precede XpGetDocumentData in XmPrintToFile 

    XpStartJob(XtDisplay(p->f_print_shell), save_data);  

    // maintain the sequence
    // between startjob and getdocument 

    XFlush(XtDisplay(p->f_print_shell));  

    // setup print to file 

    if (p->f_print_data->destination == DtPRINT_TO_FILE)
    {
        if (!XmPrintToFile(XtDisplay(p->f_print_shell), 
			   p->f_print_data->dest_info, 
			   (XPFinishProc)FinishPrintToFile, 
			   NULL))
	{
	    // Add real error message here.
	    // printf("XmPrintToFile: Unable to print to file %s\n",
	    //        p->print_data->dest_info);
	    
	    XpCancelJob(XtDisplay(p->f_print_shell), False);
	    
	    // we can go back to the event loop as if we had never printed
	    return;
	}
    }
    
    if (!(window_system().videoShell()->silent)) {

	XtRealizeWidget(p->f_print_shell);
	XtPopup(p->f_print_shell, XtGrabNone);
    }

    PrintEverything(p);

    XpEndJob(XtDisplay(p->f_print_shell));

    // synch the print display (i am hoping this will get
    // the xlib error handler called, if need be

    XSync(XtDisplay(p->f_print_shell), False);

    // if a print server error occured then notify the user

    if (print_server_error) {

	print_server_error = False; // reset for next time

	// Post an error dialog 

	msg = CATGETS(Set_PrintPanelAgent, 
		      11, 
		      "Print Job Failed.\n\nThe X Print Server is Temporarily Out of Resources.");
	message_mgr().error_dialog(msg);
	
    }

    if (!(window_system().videoShell()->silent)) {
	XtPopdown(p->f_print_shell);
    }

    // if we are in print only mode flush the output
    // buffer to force the print and then exit

    if (window_system().videoShell()->print_only) {

	XSync(XtDisplay(p->f_print_shell), False);

	// call this function to avoid invoking destructors
	_exit(0);
    }

    //  have the print manager destroy the print agent
    //  used to print this job

    print_mgr().destroy_agent();

    // destroy print shell as well
    
    if (p->f_print_shell)
    {
        XtDestroyWidget(p->f_print_shell);
        p->f_print_shell = (Widget)NULL ;
	f_print_shell = (Widget)NULL ;

    }

    RCS_DEBUG("DoPrint exiting.\n");   

}

/*
 * ------------------------------------------------------------------------
 * Name: PrintEverything
 *
 * Description:
 *
 *     Called when the print shell receives the XP events. Contains the 
 *     logic for printing all topics.
 *
 */
static void 
PrintEverything(AppPrintData *p)
{
    int cur_page = 0;
    
    RCS_DEBUG("PrintEverything called.\n");

    xList<UAS_Pointer<UAS_Common> > &print_list = *(p->f_print_list);
    Xassert (print_list.length() > 0);

    l_AppPrintData = p;
    Widget pshell = p->f_print_shell;

    List_Iterator<UAS_Pointer<UAS_Common> > l (print_list);

    while (l)
    {
	PrintOneUASCommon(l.item(), pshell, &cur_page);
	l++;
    }

    RCS_DEBUG("PrintEverything exiting.\n");
}

static void
PrintOneUASCommon(UAS_Pointer<UAS_Common> &doc, Widget pshell, int *cur_pageP)
{
    Arg	args[15];
    int n;
    XmString label;
    char buf[BUFSIZ];

    // Retrieve the topic from the doc
    // The retrieve function will send a UAS_DocumentRetrievedMsg to the
    // print manager which will render the canvas with the appropriate topic

    doc->retrieve((void *)1);

    // set print orientation to either landscape or portrait (if set)

    if (f_printOrientation != NULL) {
	sprintf(buf, "*content-orientation: %s\n", f_printOrientation);
	XpSetAttributes(XtDisplay(pshell), XpGetContext(XtDisplay(pshell)), XPPageAttr, buf, XPAttrMerge);
    }
    
    for (gHelpDisplayArea->firstVisible = gHelpDisplayArea->nextNonVisible = 0;
	 gHelpDisplayArea->nextNonVisible >= 0;
	 gHelpDisplayArea->firstVisible = gHelpDisplayArea->nextNonVisible + 1)
    {
	(*cur_pageP)++;

	XpStartPage(XtDisplay(pshell), XtWindow(pshell));

	sprintf(buf, "%d", *cur_pageP);
	label = XmStringCreateLocalized(buf);

	n = 0;
	XtSetArg(args[n], XmNlabelString, label); n++;
	XtSetValues(footer_page_no, args, n);
	XmStringFree(label);

	// Process exposures so label is updated.
	XmUpdateDisplay(footer_page_no);

	// _DtHelpCleanAndDrawWholeCanvas((XtPointer)gHelpDisplayArea);

	XpEndPage(XtDisplay(pshell));
    }

    // Print chidren if appropriate
    if (print_hierarchy)
    {
	int i;
	UAS_List<UAS_Common> kids = doc->children();

	for (i = 0; i < kids.length(); i++)
	{
	    PrintOneUASCommon(kids[i], pshell, cur_pageP);
	}
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
 *     when the setup box is destroyed).
 */
void
PrintCloseDisplayCB(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    RCS_DEBUG("PrintCloseDisplayCB called.\n");   

    DtPrintSetupCallbackStruct *pbs = (DtPrintSetupCallbackStruct*)call_data;
    AppPrintData *p = (AppPrintData*)client_data;

    if (p->f_print_shell)
    {
        XtDestroyWidget(p->f_print_shell);
        p->f_print_shell = (Widget)NULL ;
	f_print_shell = (Widget)NULL ;

    }

    DtPrintFreeSetupData(p->f_print_data);

    // reset gHelpDisplayArea so that it will be recreated.
    // ??? should i also destroy the canvas here?
    gHelpDisplayArea = 0;
    
    RCS_DEBUG("PrintCloseDisplayCB exiting.\n");   

}


// /////////////////////////////////////////////////////////////////
// ToggleWhatCB
// /////////////////////////////////////////////////////////////////

void
ToggleWhatCB(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data)
{
    RCS_DEBUG("ToggleWhatCB called.\n");   

    XmToggleButtonCallbackStruct * cbs = (XmToggleButtonCallbackStruct *) call_data;
    AppPrintData * p = (AppPrintData *)client_data;
    
    xList<UAS_Pointer<UAS_Common> > &print_list = *(p->f_print_list);
    
    static char buffer[24];
    WXmToggleButton other (NULL), active(widget);
    
    ON_DEBUG (printf ("PrintPanel::ToggleWhatCB (%p)\n", widget));
    
    Xassert ( (widget == (Widget)f_print_nodes) || (widget == (Widget)f_print_hierarchy));
    
    // Figure out which one is the other one (ie: not clicked on). 
    if (active == f_print_nodes) 
    {
	other = f_print_hierarchy;
    }
    else 
    {
	other = f_print_nodes;
    }
    
    active.Set (True);
    other.Set (False);
    
    if (active == f_print_nodes)
    {
	sprintf (buffer, "%d", print_list.length());
	print_hierarchy = False;
    }
    else // active == f_print_hierarchy 
    {
	List_Iterator<UAS_Pointer<UAS_Common> > l (print_list);
	int subtree_size = l.item()->subtree_size ();

	if (subtree_size != 0)
	{
	    sprintf (buffer, "%d", subtree_size);
	}
	else
	{
	    strcpy (buffer, "?");
	}
	print_hierarchy = True;
    }
    
    if (*buffer == '0') 
    {
	*buffer = '?';
    }
    f_to_print_field.LabelString (WXmString (buffer));

    RCS_DEBUG("ToggleWhatCBB exiting.\n");   

}
