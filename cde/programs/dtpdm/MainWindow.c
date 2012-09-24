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
/* $XConsortium: MainWindow.c /main/7 1996/10/31 02:04:14 cde-hp $ */
/*
 * dtpdm/MainWindow.c
 */
/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */
#include <Xm/XmAll.h>
#include <Dt/HelpDialog.h>
#include <Dt/dtpdmd.h>
#include <Dt/DtNlUtils.h>

#ifdef USE_EXM_TABS
#include <Exm/TabB.h>
#endif

#include "MainWindow.h"

/*
 * PdmMainWindow-specific fallback resources
 */
static String PdmMainWinFallbackResources[] =
{
    "*Notebook.ExmTabButton.notebookChildType: XmMINOR_TAB",
    "*Notebook.XmPushButton.notebookChildType: XmMINOR_TAB",
    "*HelpDialog_popup.title: Print Setup - Help",
    "*Notebook.backPageNumber: 4",
    "*Notebook.backPagePlacement: XmTOP_RIGHT",
    "*Notebook.bindingType: XmNONE",
    "*Notebook.minorTabSpacing: 8",
    "*NotebookLabel.labelString: Setup Options:",
    "*PrinterDescriptionLabel.labelString: Printer Description:",
    "*PrinterNameLabel.labelString: Printer:",
    "*helpVolume: PrnSetup"
};

/*
 * static function declarations
 */
static void PdmMainWinCreateSetupBoxes(PdmMainWin* me);
static Widget PdmMainWinCreateWindow(PdmMainWin* me, Widget parent);
static void PdmMainWinGetAttributes(PdmMainWin* me);
static void PdmMainWinOkCB(Widget, XtPointer, XtPointer);
static void PdmMainWinCancelCB(Widget, XtPointer, XtPointer);
static void PdmMainWinHelpCB(Widget, XtPointer, XtPointer);
static void PdmMainWinHelpDestroyCB(Widget, XtPointer, XtPointer);

/*
 * ------------------------------------------------------------------------
 * Name: PdmMainWinNew
 *
 * Description:
 *
 *     Allocates a new PdmMainWin instance structure.
 *
 * Return value:
 *
 *     A pointer to the new PdmMainWin instance.
 *
 */
PdmMainWin*
PdmMainWinNew()
{
    PdmMainWin* me = (PdmMainWin*)XtCalloc(1, sizeof(PdmMainWin));
    me->pdm_xp = PdmXpNew();

    return me;
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmMainWinDelete
 *
 * Description:
 *
 *     Frees the passed PdmMainWin instance structure.
 *
 * Return value:
 *
 *     None
 *
 */
void
PdmMainWinDelete(PdmMainWin* me)
{
    PdmBoxNode* node;
    /*
     * close the print server connection
     */
    if(me->pdm_xp != (PdmXp*)NULL)
	PdmXpDelete(me->pdm_xp);
    /*
     * destroy the help dialog
     */
    if((Widget)NULL != me->help_dialog)
    {
	XtDestroyWidget(me->help_dialog);
    }
    /*
     * clean up the child setup box list
     */
    while((node = me->box_list_head) != (PdmBoxNode*)NULL)
    {
	me->box_list_head = node->next;
	XtFree((char*)node);
    }
    /*
     * clean up string members
     */
    XtFree((char*)me->print_display_spec);
    /*
     * free the instance structure
     */
    XtFree((char*)me);
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmMainWinAddSetupBox
 *
 * Description:
 *
 *     Adds a PDM setup box to the list of setup boxes managed by the
 *     main window.
 *
 * Return value:
 *
 *     The passed PdmSetupBox pointer.
 *
 */
PdmSetupBox*
PdmMainWinAddSetupBox(PdmMainWin* me, PdmSetupBox* box)
{
    /*
     * create a new setup box node for the passed setup box
     * and add it to the end of the list
     */
    PdmBoxList new_node = (PdmBoxList)XtCalloc(1, sizeof(PdmBoxNode));
    new_node->box = box;
    if(me->box_list_tail)
	me->box_list_tail = me->box_list_tail->next = new_node;
    else
	me->box_list_head = me->box_list_tail = new_node;
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmMainWinMergeFallbacks
 *
 * Description:
 *
 *     Merges the fallback resources defined by the main window and each
 *     of the setup boxes with the passed set of fallback resources.
 *
 * Return value:
 *
 *     The merged array of fallback resources. The passed set of
 *     fallback appear first in the list, followed by the main window
 *     fallbacks, followed by the fallbacks for each setup box. The last
 *     entry in the list is set to NULL. It is the caller's
 *     responsibility to free the returned list using XtFree.
 *
 */
String* PdmMainWinMergeFallbacks(PdmMainWin* me,
				 const String* app_fallbacks,
				 int count)
{
    String* new_fallbacks;
    int res_count;
    PdmBoxNode* node;
    String* ptr;
    
    res_count = count;
    res_count += XtNumber(PdmMainWinFallbackResources);
    for(node = me->box_list_head; node != (PdmBoxNode*)NULL; node = node->next)
	res_count += node->box->fallback_resources_count;
    
    new_fallbacks = (String*)XtCalloc(res_count+1, sizeof(String));
    
    ptr = new_fallbacks;
    memcpy(ptr, app_fallbacks, count*sizeof(String));
    ptr += count;
    memcpy(ptr, PdmMainWinFallbackResources,
	   XtNumber(PdmMainWinFallbackResources)*sizeof(String));
    ptr += XtNumber(PdmMainWinFallbackResources);
    for(node = me->box_list_head; node != (PdmBoxNode*)NULL; node = node->next)
    {
	memcpy(ptr, node->box->fallback_resources,
	       node->box->fallback_resources_count*sizeof(String));
	ptr += node->box->fallback_resources_count;
    }
    return new_fallbacks;
}


/*
 * ------------------------------------------------------------------------
 * Name: PdmMainWinCreateSetupBoxes
 *
 * Description:
 *
 *     For each setup box added to the setup box list,
 *     PdmMainWinCreateSetupBoxes calls the setup box's create method,
 *     passing the notebook as the parent. A tab button is created for
 *     each setup box.
 *
 * Return value:
 *
 *     None.
 *
 */
static void
PdmMainWinCreateSetupBoxes(PdmMainWin* me)
{
    PdmBoxNode* node;
    
    for(node = me->box_list_head; node != (PdmBoxNode*)NULL; node = node->next)
    {
	PdmSetupBox* box = node->box;
	/*
	 * create the setup box widget as a child of the notebook
	 */
	(*box->create_proc)(box, me->notebook);
	/*
	 * create a tab for the new setup box notebook page
	 */
#ifdef USE_EXM_TABS
	node->tab =
	    XtVaCreateManagedWidget(box->tab_name,
				    exmTabButtonWidgetClass,
				    me->notebook,
				    NULL);
#else
	node->tab =
	    XtVaCreateManagedWidget(box->tab_name,
				    xmPushButtonWidgetClass,
				    me->notebook,
				    NULL);
#endif /* USE_EXM_TABS */
    }
}


/*
 * ------------------------------------------------------------------------
 * Name: PdmMainWinGetAttributes
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     None
 *
 */
static void
PdmMainWinGetAttributes(PdmMainWin* me)
{
    PdmBoxNode* node;
    /*
     * get attributes for the main window
     */
    me->printer_descriptor = PdmXpGetStringValue(me->pdm_xp,
						 XPPrinterAttr,
						 pdmoid_att_descriptor);
    me->printer_name = PdmXpGetStringValue(me->pdm_xp,
					   XPPrinterAttr,
					   pdmoid_att_printer_name);
    /*
     * call the get attributes proc for each setup box child
     */
    for(node = me->box_list_head; node != (PdmBoxNode*)NULL; node = node->next)
    {
	PdmSetupBox* box = node->box;
	
	(*box->get_attr_proc)(box, me->pdm_xp);
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmMainWinOkCB
 *
 * Description:
 *
 *     Callback for the main window OK button. This function first calls
 *     the verify values method for each of the child setup boxes. If the
 *     values for all of the children are Ok, the new values are set into
 *     the print context, and the program exits. Otherwise the program
 *     resumes. In this case, it is assumed that the child setup box will
 *     present a message to the user indicating that its values are not
 *     Ok. This function will move the invalid child setup notebook page
 *     to the top.
 *
 * Return value:
 *
 *     None, although it usually ends the program with an exit code of
 *     PDM_EXIT_OK.
 *
 */
static void
PdmMainWinOkCB(
	       Widget w,
	       XtPointer client_data,
	       XtPointer call_data)
{
    PdmMainWin* me = (PdmMainWin*)client_data;
    PdmBoxNode* node;
    int page;
    /*
     * call the verify values proc for each setup box child
     */
    for(node = me->box_list_head, page = 1;
	node != (PdmBoxNode*)NULL;
	node = node->next, page++)
    {
	PdmSetupBox* box = node->box;

	if((*box->verify_attr_proc)(box, me->pdm_xp) != PDM_SUCCESS)
	{
	    /*
	     * Ensure this setup box is the top notebook page; the setup
	     * box is responsible for providing error messages to the
	     * user.
	     *
	     * developer hint: do not use the setup box as the parent of
	     *                 a message box; if the setup box is not the
	     *                 current notebook page, the message box
	     *                 will not be positioned properly; use the
	     *                 first shell ancestor (or even the parent)
	     *                 of the setup box instead
	     */
	    XtVaSetValues(me->notebook, XmNcurrentPageNumber, page, NULL);
	    /*
	     * stop verifying and return
	     */
	    return;
	}
    }
    /*
     * call the set attributes proc for each setup box child
     */
    for(node = me->box_list_head; node != (PdmBoxNode*)NULL; node = node->next)
    {
	PdmSetupBox* box = node->box;
	
	(*box->set_attr_proc)(box, me->pdm_xp);
    }
    /*
     * set the updated attributes into the print context
     */
    PdmXpUpdateAttributes(me->pdm_xp);
    /*
     * exit the PDM
     */
    PdmMainWinDelete(me);
    exit(PDM_EXIT_OK);
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmMainWinCancelCB
 *
 * Description:
 *
 *     Callback for the main window Cancel button. This function simply
 *     ends the PDM program.
 *
 * Return value:
 *
 *     None, although it will end the program with an exit code of
 *     PDM_EXIT_CANCEL.
 *
 */
static void
PdmMainWinCancelCB(
		   Widget w,
		   XtPointer client_data,
		   XtPointer call_data)
{
    PdmMainWin* me = (PdmMainWin*)client_data;
    PdmMainWinDelete(me);
    exit(PDM_EXIT_CANCEL);
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmMainWinHelpCB
 *
 * Description:
 *
 *
 * Return value:
 *
 *
 */
static void
PdmMainWinHelpCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    PdmMainWin* me = (PdmMainWin*)client_data;
    PdmSetupBox* box;
    struct _box_resources
    {
	String help_volume;
	String location_id;
    } box_resources;
    /*
     * create the help dialog if needed
     */
    if((Widget)NULL == me->help_dialog)
    {
	me->help_dialog =
	    DtCreateHelpDialog(XtParent(me->widget), "HelpDialog", NULL, 0);
	XtAddCallback(me->help_dialog, XmNdestroyCallback,
		      PdmMainWinHelpDestroyCB, (XtPointer)me);
    }
    /*
     * determine the current setup box
     */
    {
	XtArgVal current_page;
	int i;
	PdmBoxNode* node;
	
	XtVaGetValues(me->notebook, XmNcurrentPageNumber, &current_page, NULL);
	node = me->box_list_head;
	for(i = 1; i < (int)current_page; i++)
	    node = node->next;
	box = node->box;
    }
    /*
     * the help volume name and location id are obtained up as application
     * resources qualified for each setup box
     */
    {
	XtResource resources[2];
	/*
	 * initialize res struct for help volume
	 */
	resources[0].resource_name = DtNhelpVolume;
	resources[0].resource_class = DtCHelpVolume;
	resources[0].resource_type =  XmRString;
	resources[0].resource_size = sizeof(String);
	resources[0].resource_offset =
	    XtOffsetOf(struct _box_resources, help_volume);
	resources[0].default_type = XmRImmediate;
	resources[0].default_addr = (XtPointer)NULL;
	/*
	 * initialize res struct for location id
	 */
	resources[1].resource_name = DtNlocationId;
	resources[1].resource_class = DtCLocationId;
	resources[1].resource_type =  XmRString;
	resources[1].resource_size = sizeof(String);
	resources[1].resource_offset =
	    XtOffsetOf(struct _box_resources, location_id);
	resources[1].default_type = XmRImmediate;
	resources[1].default_addr = (XtPointer)NULL;
	/*
	 * get the resource values for the current setup box widget
	 */
	XtGetApplicationResources(box->widget, (XtPointer)&box_resources,
				  resources, XtNumber(resources),
				  (ArgList)NULL, 0);
    }
    /*
     * set the help volume and location
     */
    XtVaSetValues(me->help_dialog,
		  DtNhelpVolume, box_resources.help_volume,
		  DtNlocationId, box_resources.location_id,
		  DtNhelpType, DtHELP_TYPE_TOPIC,
		  NULL);
    /*
     * pop up the help dialog
     */
    XtManageChild(me->help_dialog);
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmMainWinHelpCB
 *
 * Description:
 *
 *     Update the main window instance structure to reflect destruction
 *     of the help dialog.
 *
 * Return value:
 *
 *     None.
 */
static void
PdmMainWinHelpDestroyCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    PdmMainWin* me = (PdmMainWin*)client_data;
    me->help_dialog = (Widget)NULL;
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmMainWinCreate
 *
 * Description:
 *
 *     Creates the PDM main window, including setup box children added
 *     via PdmMainWinAddSetupBox.
 *
 * Return value:
 *
 *     The passed PdmMainWin pointer.
 *
 */
PdmMainWin*
PdmMainWinCreate(PdmMainWin* me,
		 Widget parent,
		 String print_display_spec,
		 String print_context_str)
{
    /*
     * establish the print server connection
     */
    if(PdmXpOpen(me->pdm_xp, print_display_spec, print_context_str)
       == (Display*)NULL)
    {
	/*
	 * unable to open the print display
	 */
	PdmMainWinDelete(me);
	exit(PDM_EXIT_PXAUTH);
    }
    /*
     * make a copy of the print display spec
     */
    me->print_display_spec = XtNewString(print_display_spec);
    /*
     * get attributes for the main window and the setup box children
     */
    PdmMainWinGetAttributes(me);
    /*
     * create the main window
     */
    PdmMainWinCreateWindow(me, parent);
    /*
     * add the registered setup boxes to the notebook
     */
    PdmMainWinCreateSetupBoxes(me);
    /*
     * return
     */
    return me;
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmMainWinCreateWindow
 *
 * Description:
 *
 *     Creates the PDM main window, including setup box children added
 *     via PdmMainWinAddSetupBox.
 *
 * Return value:
 *
 *     The main window widget ID.
 *
 */
static Widget
PdmMainWinCreateWindow(PdmMainWin* me,
		       Widget parent)
{
    Widget manager;
    Widget row;
    Widget w;
    XmString label;

    /*
     * create the main window
     */
    me->widget =
	XtVaCreateManagedWidget("Main",
				xmMessageBoxWidgetClass,
				parent,
				XmNdialogType, XmDIALOG_MESSAGE,
				NULL);
    XtUnmanageChild(XtNameToWidget(me->widget, "Message"));
    XtUnmanageChild(XtNameToWidget(me->widget, "Symbol"));
    /*
     * add OK, Cancel and Help pushbutton callbacks
     */
    XtAddCallback(me->widget, XmNokCallback,
		  PdmMainWinOkCB, (XtPointer)me);
    XtAddCallback(me->widget, XmNcancelCallback,
		  PdmMainWinCancelCB, (XtPointer)me);
    XtAddCallback(me->widget, XmNhelpCallback,
		  PdmMainWinHelpCB, (XtPointer)me);
    /*
     * create the main manager widget
     */
    manager = XtVaCreateManagedWidget("Manager",
				      xmRowColumnWidgetClass,
				      me->widget,
				      NULL);
    /*
     * create the printer description row
     */
    if(me->printer_descriptor != (const char*)NULL)
    {
	char* desc;
	char* ptr;
	
	row = XtVaCreateManagedWidget(
				      "PrinterDescriptionRow",
				      xmRowColumnWidgetClass,
				      manager,
				      XmNorientation, XmHORIZONTAL,
				      NULL);
	/*
	 * create the printer description label
	 */
	w = XtVaCreateManagedWidget("PrinterDescriptionLabel",
				    xmLabelGadgetClass,
				    row,
				    NULL);
	/*
	 * create the printer description using just the 1st line
	 */
	desc = XtNewString(me->printer_descriptor);
	ptr = Dt_strchr(desc, '\n');
	if(ptr != NULL)
	    *ptr = '\0';
	label = XmStringCreateLocalized(desc);
	XtFree(desc);
	w = XtVaCreateManagedWidget("PrinterDescription",
				    xmLabelGadgetClass,
				    row,
				    XmNlabelString, label,
				    NULL);
	XmStringFree(label);
    }
    /*
     * create the printer name row
     */
    if(me->printer_name != (const char*)NULL)
    {
	char* printer_spec;
	int printer_spec_len;

	row = XtVaCreateManagedWidget(
				      "PrinterNameRow",
				      xmRowColumnWidgetClass,
				      manager,
				      XmNorientation, XmHORIZONTAL,
				      NULL);
	/*
	 * create the printer name label
	 */
	w = XtVaCreateManagedWidget("PrinterNameLabel",
				    xmLabelGadgetClass,
				    row,
				    NULL);
	/*
	 * build the X printer specifier
	 */
	printer_spec_len = strlen(me->printer_name);
	if(me->print_display_spec != (char*)NULL)
	    printer_spec_len += strlen(me->print_display_spec) + 1;
	printer_spec = XtMalloc(printer_spec_len + 1);
	strcpy(printer_spec, me->printer_name);
	if(me->print_display_spec != (char*)NULL)
	{
	    strcat(printer_spec, "@");
	    strcat(printer_spec, me->print_display_spec);
	}
	/*
	 * create the printer name
	 */
	label = XmStringCreateLocalized(printer_spec);
	XtFree(printer_spec);
	w = XtVaCreateManagedWidget("PrinterName",
				    xmLabelGadgetClass,
				    row,
				    XmNlabelString, label,
				    NULL);
	XmStringFree(label);
    }
    /*
     * top separator
     */
    w = XtVaCreateManagedWidget("TopSeparator",
				xmSeparatorGadgetClass,
				manager,
				NULL);
    /*
     * notebook label
     */
    row = XtVaCreateManagedWidget(
				  "PrinterNameRow",
				  xmRowColumnWidgetClass,
				  manager,
				  XmNorientation, XmHORIZONTAL,
				  NULL);
    w = XtVaCreateManagedWidget("NotebookLabel",
				xmLabelGadgetClass,
				row,
				NULL);
    /*
     * create the notebook
     */
    row = XtVaCreateManagedWidget(
				  "NotebookRow",
				  xmRowColumnWidgetClass,
				  manager,
				  XmNorientation, XmHORIZONTAL,
				  NULL);
    me->notebook =
	XtVaCreateManagedWidget(
				"Notebook",
				xmNotebookWidgetClass,
				row,
				NULL);
    /*
     * Create an unmanaged notebook page scroller (i.e. a widget that has
     * the XmQTnavigator trait), otherwise the notebook widget will
     * create a default page scroller when it is realized. If this
     * default page scroller is unmanaged after realization, the parent
     * manager of the notebook does not shrink in order to reclaim the
     * space previously occupied by the page scroller.
     */
    XtVaCreateWidget("DummyPageScroller",
		     xmScrollBarWidgetClass,
		     me->notebook,
		     NULL);
    /*
     * return
     */
    return me->widget;
}
