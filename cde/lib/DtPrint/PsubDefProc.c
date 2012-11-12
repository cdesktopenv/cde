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
/* $XConsortium: PsubDefProc.c /main/14 1996/12/02 10:51:20 rswiston $ */
/*
 * DtPrint/PsubDefProc.c
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
/*
 * ------------------------------------------------------------------------
 * Include Files
 *
 */
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#include <Xm/XmAll.h>

#include <Dt/HelpDialog.h>
#include <Dt/DtNlUtils.h>
#include <Dt/HourGlass.h>

/*
 * PrintSetupBP.h is included only for access to the DtPrintDefProcData
 * member of the widget instance structure.
 */
#include <Dt/PrintSetupBP.h>

#include <Dt/PsubUtilI.h>

/*
 * ------------------------------------------------------------------------
 * Constant Definitions
 *
 */
#define HELP_VOLUME "LibDtPrint"
#define PRINTER_INFO_HELP_ID "PrinterInfo"
#define SELECT_PRINTER_HELP_ID "SelectPrinter"
#define SELECT_FILE_HELP_ID "SelectFile"

/*
 * ------------------------------------------------------------------------
 * Static Function Declarations
 *
 */
static XtEnum BuildPrinterLists(
				Widget w,
				DtPrintDefaultProcData* dpd,
				int* item_count);
static XmStringTable BuildPrinterSelectionItems(
						Display* display,
						DtPrintDefaultProcData* dpd,
						int item_count,
						String initial_printer,
						XmStringTag tag);
static void CloseSelectPrinterInfoConnection(
					     DtPrintDefaultProcData* dpd);
static int CompareSelectPrinterRecs(const void*,
				    const void*);
static String CompoundTextToString(
				   Display* display,
				   unsigned char* compound_text);
static XtEnum CreateFileSelectionBox(
				     Widget parent,
				     Widget psub,
				     DtPrintDefaultProcData* dpd);
static XtEnum CreatePrinterInfoBox(
				   Widget parent,
				   Widget psub,
				   DtPrintDefaultProcData* dpd);
static XtEnum CreatePrinterSelectionBox(
					Widget parent,
					Widget psub,
					DtPrintDefaultProcData* dpd);
static void DestroyWidgetCB(
			    Widget w,
			    XtPointer client_data,
			    XtPointer call_data);
static void DtPrintDefProcDestroyCB(Widget w,
				    XtPointer client_data,
				    XtPointer call_data);
static void ErrorMessageDestroyCB(
				  Widget w,
				  XtPointer client_data,
				  XtPointer call_data);
static String FindSelectedPrinter(
				  Display* display,
				  DtPrintDefaultProcData* dpd);
static void FreeSelectPrinterData(
				  DtPrintDefaultProcData* dpd);
static Widget GetWMShellAncestor(
				 Widget w);
static void HelpDialogDestroyCB(
				Widget w,
				XtPointer client_data,
				XtPointer call_data);
static void InfoBoxLayoutCB(
			    Widget widget,
			    XtPointer client_data,
			    XtPointer call_data);
static void ParseFileNameSpec(
			      const char* file_name,
			      XmString* pattern,
			      char** name_only);
static void PresentErrorDialog(
			       Widget w,
			       String title,
			       String message,
			       ...);
static void PresentHelp(
			Widget w,
			const char* help_volume,
			const char* location_id);
static void PresentVerifyError(
			       Widget w,
			       XtEnum status,
			       String printer_spec);
static void PrinterInfoDestroyCB(
				 Widget w,
				 XtPointer client_data,
				 XtPointer call_data);
static void PrinterInfoHelpCB(
			      Widget w,
			      XtPointer client_data,
			      XtPointer call_data);
static void SelectFileDestroyCB(
				Widget w,
				XtPointer client_data,
				XtPointer call_data);
static void SelectFileHelpCB(
			     Widget w,
			     XtPointer client_data,
			     XtPointer call_data);
static void SelectPrinterCB(
			    Widget w,
			    XtPointer client_data,
			    XtPointer call_data);
static void SelectPrinterDestroyCB(
				   Widget w,
				   XtPointer client_data,
				   XtPointer call_data);
static void SelectPrinterHelpCB(
				Widget w,
				XtPointer client_data,
				XtPointer call_data);
static void SelectPrinterInfoCB(
				Widget w,
				XtPointer client_data,
				XtPointer call_data);
static void SelectPrinterItemCB(
				Widget w,
				XtPointer client_data,
				XtPointer call_data);
static void SetListBoxSelection(
				Widget list_box,
				int position);
static void UpdateFileNameCB(
			     Widget w,
			     XtPointer client_data,
			     XtPointer call_data);

/*
 * ------------------------------------------------------------------------
 * Name: BuildPrinterLists
 *
 * Description:
 *
 *     Retrieves lists of printers from the Xp server found in the
 *     XpServerList resource or XPSERVERLIST env var.
 *
 * Return value:
 *
 *
 *
 */
static XtEnum
BuildPrinterLists(
		  Widget w,
		  DtPrintDefaultProcData* dpd,
		  int* item_count)
{
    String* server_list;
    int server_count;
    int i, j;
    Display* display;
    int error_base;
    int event_base;
    XPPrinterList xp_printer_list;
    DtPrintSelectPrinterList printer_list;
    /*
     * clean up previous lists if needed
     */
    FreeSelectPrinterData(dpd);
    /*
     * get the list of servers
     */
    server_list = _DtPrintGetXpServerList(w);
    if((String*)NULL == server_list)
	return DtPRINT_FAILURE;
    /*
     * get the printer list for each valid Xp server
     */
    for(server_count = 0;
	(String)NULL != server_list[server_count];
	server_count++);
    dpd->xp_server_list =
	(String*)XtCalloc(server_count, sizeof(String));
    dpd->printer_lists = (DtPrintSelectPrinterList*)
	XtCalloc(server_count, sizeof(DtPrintSelectPrinterList));
    dpd->printer_counts =
	(int*)XtCalloc(server_count, sizeof(int));
    for(i = 0, *item_count = 0; i < server_count; i++)
    {
	/*
	 * ensure the server is a valid Xp server
	 */
	display = XOpenDisplay(server_list[i]);
	if((Display*)NULL == display)
	    continue;
	if(!XpQueryExtension(display, &event_base, &error_base))
	{
	    XCloseDisplay(display);
	    continue;
	}
	/*
	 * add the server to the xp server list
	 */
	dpd->xp_server_list[dpd->xp_server_count] =
	    XtNewString(server_list[i]);
	/*
	 * get the printer list for the server
	 */
	xp_printer_list =
	    XpGetPrinterList(display, (char*)NULL,
			     &dpd->printer_counts[dpd->xp_server_count]);
	/*
	 * save a copy of the compound text printer name and
	 * string versions of the name and description for
	 * eventual use in the printer selection list.
	 */
	dpd->printer_lists[dpd->xp_server_count] = (DtPrintSelectPrinterList)
	    XtCalloc(dpd->printer_counts[dpd->xp_server_count],
		     sizeof(DtPrintSelectPrinterRec));
	printer_list = dpd->printer_lists[dpd->xp_server_count];
	for(j = 0; j < dpd->printer_counts[dpd->xp_server_count]; j++)
	{
	    printer_list[j].printer_name_ct =
		XtNewString(xp_printer_list[j].name);
	    printer_list[j].printer_name =
		CompoundTextToString(display,
				     (unsigned char*)xp_printer_list[j].name);
	    printer_list[j].description =
		CompoundTextToString(display,
				     (unsigned char*)xp_printer_list[j].desc);
	}
	XpFreePrinterList(xp_printer_list);
	/*
	 * sort the printer list
	 */
	if(0 < dpd->printer_counts[dpd->xp_server_count])
	    qsort((void*)printer_list,
		  dpd->printer_counts[dpd->xp_server_count],
		  sizeof(DtPrintSelectPrinterRec), CompareSelectPrinterRecs);

	*item_count += dpd->printer_counts[dpd->xp_server_count];
	++dpd->xp_server_count;
	XCloseDisplay(display);
    }
    _DtPrintFreeStringList(server_list);
    if(0 == *item_count)
    {
	FreeSelectPrinterData(dpd);
	return DtPRINT_FAILURE;
    }
    else
	return DtPRINT_SUCCESS;
}

/*
 * ------------------------------------------------------------------------
 * Name: BuildPrinterSelectionItems
 *
 * Description:
 *
 *     Builds the list of XmString items (printer name + desc) to be set
 *     in the printer selection list box.
 *
 * Return value:
 *
 *     None.
 *
 */
static XmStringTable
BuildPrinterSelectionItems(
			   Display* display,
			   DtPrintDefaultProcData* dpd,
			   int item_count,
			   String initial_printer,
			   XmStringTag tag)
{
    XmStringTable items;
    int i, j;
    char* buf = (char*)NULL;
    Cardinal buf_size = 0;
    Cardinal new_size;
    int current_item;
    String name, full_name, desc;
    char* server_name;
    int server_len;
    
    items = (XmStringTable)XtCalloc(item_count, sizeof(XmString));
    for(i = 0, current_item = 0; i < dpd->xp_server_count; i++)
    {
	server_name = dpd->xp_server_list[i];
	server_len = strlen(server_name);
	
	for(j = 0; j < dpd->printer_counts[i]; j++, current_item++)
	{
	    /*
	     * build a fully qualified X printer specifier
	     */
	    name = (dpd->printer_lists[i])[j].printer_name;
	    (dpd->printer_lists[i])[j].printer_name = (String)NULL;
	    full_name =
		_DtPrintCreateXPrinterSpecifier(name, server_name,
						DtPRINT_NET_UNSPECIFIED,
						-1, -1);
	    XtFree(name);
	    /*
	     * check to see if this should be the initially selected printer
	     */
	    if(dpd->selected_printer == 0
	       && (String)NULL != initial_printer)
		if(strcmp(initial_printer, full_name) == 0)
		    dpd->selected_printer = current_item + 1;
	    /*
	     * convert the printer description
	     */
	    desc = (dpd->printer_lists[i])[j].description;
	    (dpd->printer_lists[i])[j].description = (String)NULL;
	    if((String)NULL != desc && '\0' != *desc)
	    {
		/*
		 * chop the description after the 1st line
		 */
		char* ptr = Dt_strchr(desc, '\n');
		if((char*)NULL != ptr)
		    *ptr = '\0';
	    }
	    /*
	     * ensure the format buffer is large enough to contain
	     * the formatted list item
	     */
	    new_size = 2;
	    new_size += full_name ? strlen(full_name) : 0;
	    new_size += desc ? strlen(desc) : 0;
	    if(new_size > buf_size)
	    {
		buf_size = new_size;
		buf = XtRealloc(buf, buf_size);
	    }
	    /*
	     * format the item, and add it to the item list
	     */
	    sprintf(buf, "%s\t%s",
		    full_name ? full_name : "",
		    desc ? desc : "");
	    XtFree(desc);
	    XtFree(full_name);
	    items[current_item] =
		XmStringGenerate((XtPointer)buf, (XmStringTag)NULL,
				 XmMULTIBYTE_TEXT, tag);
	}
    }
    XtFree(buf);
    return items;
}

/*
 * ------------------------------------------------------------------------
 * Name: CloseSelectPrinterInfoConnection
 *
 * Description:
 *
 *     Close the X printer connection maintained for the Select Printer
 *     dialog's Printer Information dialog.
 *
 * Return value:
 *
 *     None.
 *
 */
static void
CloseSelectPrinterInfoConnection(
				 DtPrintDefaultProcData* dpd)
{
    if((Display*)NULL != dpd->select_printer_info_display)
    {
	if((XPContext)None != dpd->select_printer_info_context)
	{
	    XpDestroyContext(dpd->select_printer_info_display,
			     dpd->select_printer_info_context);
	    dpd->select_printer_info_context = (XPContext)None;
	}
	XCloseDisplay(dpd->select_printer_info_display);
	dpd->select_printer_info_display = (Display*)NULL;
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: CompareSelectPrinterRecs
 *
 * Description:
 *
 *     Compares the printer names in two DtPrintSelectPrinterRecs.
 *
 * Return value:
 *
 *     Returns an integer greater than, equal to, or less than zero,
 *     according to whether the printer name in spr1 is greater
 *     than, equal to, or less than the printer name in spr2.
 *
 */
static int
CompareSelectPrinterRecs(const void* spr1,
			 const void* spr2)
{
    return strcoll(((DtPrintSelectPrinterRec*)spr1)->printer_name,
		   ((DtPrintSelectPrinterRec*)spr2)->printer_name);
}

/*
 * ------------------------------------------------------------------------
 * Name: CompoundTextToString
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     None.
 *
 */
static String
CompoundTextToString(
		     Display* display,
		     unsigned char* compound_text)
{
    String str = (String)NULL;

    if((unsigned char*)NULL != compound_text)
    {
	XTextProperty text_prop;
	char** list;
	int count;

	text_prop.encoding = XInternAtom(display, "COMPOUND_TEXT", False);
	text_prop.format = 8;
	text_prop.value = compound_text;
	text_prop.nitems = strlen((char*)text_prop.value);
	if(Success ==
	   XmbTextPropertyToTextList(display, &text_prop, &list, &count))
	{
	    if(count > 0)
		str = XtNewString(list[0]);
	    XFreeStringList(list);
	}
    }
    return str;
}

/*
 * ------------------------------------------------------------------------
 * Name: CreateFileSelectionBox
 *
 * Description:
 *
 *     Creates the file selection dialog box.
 *
 * Return value:
 *
 *     None.
 *
 */
static XtEnum
CreateFileSelectionBox(
		       Widget parent,
		       Widget psub,
		       DtPrintDefaultProcData* dpd)
{
    Arg args[15];
    Cardinal n;
    XmString title_xmstr;

    title_xmstr = XmStringCreateLocalized(SELECT_FILE_TITLE);
    
    n = 0;
    /*
     * dialog resources
     */
    XtSetArg(args[n], XmNdialogTitle, title_xmstr); n++;
    XtSetArg(args[n], XmNdialogStyle,
	     XmDIALOG_PRIMARY_APPLICATION_MODAL); n++;
    XtSetArg(args[n], XmNdeleteResponse, XmUNMAP); n++;
    /*
     * file selection box resources
     */
    XtSetArg(args[n], XmNautoUnmanage, True); n++;
    /*
     * create the file selection box dialog
     */
    dpd->file_selection_box =
	XmCreateFileSelectionDialog(parent, "_PsubDefProcFileSelectionBox",
				    args, n);
    XmStringFree(title_xmstr);
    if(dpd->file_selection_box == (Widget)NULL)
	return DtPRINT_FAILURE;
    /*
     * add callbacks
     */
    XtAddCallback(dpd->file_selection_box, XmNokCallback,
		  UpdateFileNameCB, (XtPointer)psub);
    XtAddCallback(dpd->file_selection_box, XmNhelpCallback,
		  SelectFileHelpCB, (XtPointer)psub);
    XtAddCallback(dpd->file_selection_box, XmNdestroyCallback,
		  SelectFileDestroyCB, (XtPointer)psub);
    /*
     * return
     */
    return DtPRINT_SUCCESS;
}

/*
 * ------------------------------------------------------------------------
 * Name: CreatePrinterInfoBox
 *
 * Description:
 *
 *     Creates the printer information dialog box.
 *
 * Return value:
 *
 *     None.
 *
 */
static XtEnum
CreatePrinterInfoBox(
		     Widget parent,
		     Widget psub,
		     DtPrintDefaultProcData* dpd)
{
    Arg args[15];
    Cardinal n;
    XmString title_xmstr;
    Widget manager;
    Widget description_label, description;
    Widget name_label, name;
    Widget format_label, format;
    Widget model_label, model;
    XmString label;
    
    title_xmstr = XmStringCreateLocalized(PRINTER_INFO_TITLE);
    
    n = 0;
    /*
     * dialog resources
     */
    XtSetArg(args[n], XmNdialogTitle, title_xmstr); n++;
    XtSetArg(args[n], XmNdialogStyle,
	     XmDIALOG_PRIMARY_APPLICATION_MODAL); n++;
    XtSetArg(args[n], XmNdeleteResponse, XmUNMAP); n++;
    /*
     * printer info box resources
     */
    XtSetArg(args[n], XmNautoUnmanage, True); n++;
    /*
     * create the dialog
     */
    dpd->printer_info_box =
	XmCreateInformationDialog(parent, "_PsubDefProcPrinterInfoBox",
				  args, n);
    XmStringFree(title_xmstr);
    if(dpd->printer_info_box == (Widget)NULL)
	return DtPRINT_FAILURE;
    /*
     * add callbacks
     */
    XtAddCallback(dpd->printer_info_box, XmNhelpCallback,
		  PrinterInfoHelpCB, (XtPointer)psub);
    XtAddCallback(dpd->printer_info_box, XmNdestroyCallback,
		  PrinterInfoDestroyCB, (XtPointer)psub);
    XtAddCallback(dpd->printer_info_box, XmNmapCallback,
		  InfoBoxLayoutCB, (XtPointer)dpd);
    /*
     * unmanage unwanted children
     */
    XtUnmanageChild(XtNameToWidget(dpd->printer_info_box, "Cancel"));
    XtUnmanageChild(XtNameToWidget(dpd->printer_info_box, "Message"));
    XtUnmanageChild(XtNameToWidget(dpd->printer_info_box, "Symbol"));
    /*
     * create the control manager
     */
    manager =
	XtVaCreateManagedWidget("PrinterInfoForm", xmFormWidgetClass,
				dpd->printer_info_box,
				XmNallowOverlap, False,
				NULL);
    /*
     * printer description
     */
    label = XmStringCreateLocalized(DESCRIPTION_LABEL);
    description_label =
	XtVaCreateManagedWidget("DescriptionLabel",
				xmLabelWidgetClass,
				manager,
				XmNlabelString, label,
				XmNleftAttachment,   XmATTACH_FORM,
				XmNtopAttachment,    XmATTACH_FORM,
				XmNtopOffset,        5,
				NULL);
    XmStringFree(label);
    description = 
	XtVaCreateManagedWidget("Description",
				xmLabelWidgetClass,
				manager,
				XmNalignment,        XmALIGNMENT_BEGINNING,
				XmNleftAttachment,   XmATTACH_WIDGET,
				XmNleftWidget,       description_label,
				XmNleftOffset,       10,
				XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET,
				XmNtopWidget,        description_label,
				NULL);
    /*
     * printer name
     */
    label = XmStringCreateLocalized(PRINTER_NAME_LABEL);
    name_label =
	XtVaCreateManagedWidget("NameLabel",
				xmLabelWidgetClass,
				manager,
				XmNlabelString, label,
				XmNleftAttachment,   XmATTACH_FORM,
				XmNtopAttachment,    XmATTACH_WIDGET,
				XmNtopWidget,        description,
				XmNtopOffset,        5,
				NULL);
    XmStringFree(label);
    name = 
	XtVaCreateManagedWidget("Name",
				xmLabelWidgetClass,
				manager,
				XmNleftAttachment,   XmATTACH_WIDGET,
				XmNleftWidget,       name_label,
				XmNleftOffset,       10,
				XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET,
				XmNtopWidget,        name_label,
				NULL);
    /*
     * printer format
     */
    label = XmStringCreateLocalized(FORMAT_LABEL);
    format_label =
	XtVaCreateManagedWidget("FormatLabel",
				xmLabelWidgetClass,
				manager,
				XmNlabelString, label,
				XmNleftAttachment,   XmATTACH_FORM,
				XmNtopAttachment,    XmATTACH_WIDGET,
				XmNtopWidget,        name_label,
				XmNtopOffset,        5,
				NULL);
    XmStringFree(label);
    format = 
	XtVaCreateManagedWidget("Format",
				xmLabelWidgetClass,
				manager,
				XmNleftAttachment,   XmATTACH_WIDGET,
				XmNleftWidget,       format_label,
				XmNleftOffset,       10,
				XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET,
				XmNtopWidget,        format_label,
				NULL);
    /*
     * printer model
     */
    label = XmStringCreateLocalized(MODEL_LABEL);
    model_label =
	XtVaCreateManagedWidget("ModelLabel",
				xmLabelWidgetClass,
				manager,
				XmNlabelString, label,
				XmNleftAttachment,   XmATTACH_FORM,
				XmNtopAttachment,    XmATTACH_WIDGET,
				XmNtopWidget,        format_label,
				XmNtopOffset,        5,
				NULL);
    XmStringFree(label);
    model = 
	XtVaCreateManagedWidget("Model",
				xmLabelWidgetClass,
				manager,
				XmNalignment,        XmALIGNMENT_BEGINNING,
				XmNleftAttachment,   XmATTACH_WIDGET,
				XmNleftWidget,       model_label,
				XmNleftOffset,       10,
				XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET,
				XmNtopWidget,        model_label,
				NULL);
    /*
     * return
     */
    return DtPRINT_SUCCESS;
}

/*
 * ------------------------------------------------------------------------
 * Name: CreatePrinterSelectionBox
 *
 * Description:
 *
 *     Creates the printer selection dialog box.
 *
 * Return value:
 *
 *     None.
 *
 */
static XtEnum
CreatePrinterSelectionBox(
			  Widget parent,
			  Widget psub,
			  DtPrintDefaultProcData* dpd)
{
    Arg args[15];
    Cardinal n;
    Widget control;
    XmString list_label;
    XmString title_xmstr;
    XmString info_label;

    title_xmstr = XmStringCreateLocalized(MORE_PRINTERS_TITLE);
    list_label = XmStringCreateLocalized(PRINTER_LIST_LABEL);
    info_label = XmStringCreateLocalized(PRINTER_INFO_LABEL);
    
    n = 0;
    /*
     * dialog resources
     */
    XtSetArg(args[n], XmNdialogTitle, title_xmstr); n++;
    XtSetArg(args[n], XmNdialogStyle,
	     XmDIALOG_PRIMARY_APPLICATION_MODAL); n++;
    XtSetArg(args[n], XmNdeleteResponse, XmDESTROY); n++;
    /*
     * selection box resources
     */
    XtSetArg(args[n], XmNautoUnmanage, True); n++;
    XtSetArg(args[n], XmNlistLabelString, list_label); n++;
    XtSetArg(args[n], XmNapplyLabelString, info_label); n++;
    /*
     * create the dialog
     */
    dpd->printer_selection_box =
	XmCreateSelectionDialog(parent, "_PsubDefProcPrinterSelectionBox",
				args, n);
    XmStringFree(list_label);
    XmStringFree(info_label);
    XmStringFree(title_xmstr);
    if(dpd->printer_selection_box == (Widget)NULL)
	return DtPRINT_FAILURE;
    /*
     * add callbacks
     */
    XtAddCallback(dpd->printer_selection_box, XmNokCallback,
		  SelectPrinterCB, (XtPointer)psub);
    XtAddCallback(dpd->printer_selection_box, XmNapplyCallback,
		  SelectPrinterInfoCB, (XtPointer)psub);
    XtAddCallback(dpd->printer_selection_box, XmNcancelCallback,
		  SelectPrinterCB, (XtPointer)psub);
    XtAddCallback(dpd->printer_selection_box, XmNhelpCallback,
		  SelectPrinterHelpCB, (XtPointer)psub);
    XtAddCallback(dpd->printer_selection_box, XmNdestroyCallback,
		  SelectPrinterDestroyCB, (XtPointer)psub);
    /*
     * get the list box widget ID
     */
    dpd->printer_list_box =
	XtNameToWidget(dpd->printer_selection_box, "*ItemsList");
    XtAddCallback(dpd->printer_list_box, XmNbrowseSelectionCallback,
		  SelectPrinterItemCB, (XtPointer)psub);
    /*
     * unmanaged unwanted children
     */
    control = XtNameToWidget(dpd->printer_selection_box, "*Selection");
    if(control != (Widget)NULL)
	XtUnmanageChild(control);
    control = XtNameToWidget(dpd->printer_selection_box, "*Text");
    if(control != (Widget)NULL)
	XtUnmanageChild(control);
    /*
     * return
     */
    return DtPRINT_SUCCESS;
}

/*
 * ------------------------------------------------------------------------
 * Name: DestroyWidgetCB
 *
 * Description:
 *
 *     Destroys the Widget passed as client_data.
 *
 * Return value:
 *
 *     None.
 *
 */
static void 
DestroyWidgetCB(
		Widget w,
		XtPointer client_data,
		XtPointer call_data)
{
    XtDestroyWidget((Widget)client_data);
}

/*
 * ------------------------------------------------------------------------
 * Name: DtPrintDefProcDestroyCB
 *
 * Description:
 *
 *     Free resources allocated for the default procedure data structure
 *     in response to the destruction of the DtPrintSetupBox.
 *
 *     Calls destroy callbacks for dialogs *directly* because the dialogs
 *     aren't children of the DtPrintSetupBox, but instead are more like
 *     siblings or cousins of the PrintSetupBox whose common ancestor is
 *     the shell parent of the PrintSetupBox. As such the dialogs may or
 *     may not otherwise be destroyed when the PrintSetupBox is
 *     destroyed. Furthermore the 2nd phase of the XtDestroyWidget calls
 *     made in this routine isn't reached until after the PrintSetupBox
 *     instance record has been destroyed (as part of the destroy phase
 *     of which *this* call is a part).
 *
 *     The main reason this is a concern is because data associated with
 *     these dialogs is stored in the PrintSetupBox widget instance
 *     structure.
 *
 * Return value:
 *
 *     None.
 *
 */
static void
DtPrintDefProcDestroyCB(Widget w,
			XtPointer client_data,
			XtPointer call_data)
{
    DtPrintDefaultProcData* dpd = &PSUB_DefaultProcData(w);

    if(dpd->error_message_box)
    {
	XtRemoveCallback(dpd->error_message_box, XmNdestroyCallback,
			 ErrorMessageDestroyCB, (XtPointer)w);
	XtDestroyWidget(dpd->error_message_box);
	ErrorMessageDestroyCB(dpd->error_message_box,
			      (XtPointer)w, (XtPointer)NULL);
    }
    if(dpd->help_dialog)
    {
	XtRemoveCallback(dpd->help_dialog, XmNdestroyCallback,
			 HelpDialogDestroyCB, (XtPointer)w);
	XtDestroyWidget(dpd->help_dialog);
	HelpDialogDestroyCB(dpd->help_dialog,
			    (XtPointer)w, (XtPointer)NULL);
    }
    if(dpd->file_selection_box)
    {
	XtRemoveCallback(dpd->file_selection_box, XmNdestroyCallback,
			 SelectFileDestroyCB, (XtPointer)w);
	XtDestroyWidget(dpd->file_selection_box);
	SelectFileDestroyCB(dpd->file_selection_box,
			    (XtPointer)w, (XtPointer)NULL);
    }
    if(dpd->printer_selection_box)
    {
	XtRemoveCallback(dpd->printer_selection_box, XmNdestroyCallback,
			 SelectPrinterDestroyCB, (XtPointer)w);
	XtDestroyWidget(dpd->printer_selection_box);
	SelectPrinterDestroyCB(dpd->printer_selection_box,
			       (XtPointer)w, (XtPointer)NULL);
    }
    if(dpd->printer_info_box)
    {
	XtRemoveCallback(dpd->printer_info_box, XmNdestroyCallback,
			 PrinterInfoDestroyCB, (XtPointer)w);
	XtDestroyWidget(dpd->printer_info_box);
	PrinterInfoDestroyCB(dpd->printer_info_box,
			     (XtPointer)w, (XtPointer)NULL);
    }
    
}

/*
 * ------------------------------------------------------------------------
 * Name: ErrorMessageDestroyCB
 *
 * Description:
 *
 *     Reset the error message widget ID instance data when the message
 *     box is destroyed.
 *
 *     This callback is considered part of the default resource
 *     procedures and not part of the setup box widget proper.
 *
 * Return value:
 *
 *     None.
 *
 */
static void 
ErrorMessageDestroyCB(
		      Widget w,
		      XtPointer client_data,
		      XtPointer call_data)
{
    DtPrintDefaultProcData* dpd = &PSUB_DefaultProcData(client_data);
    
    dpd->error_message_box = (Widget)NULL;
}

/*
 * ------------------------------------------------------------------------
 * Name: FindSelectedPrinter
 *
 * Description:
 *
 *     Build a full printer name by locating the currently selected
 *     printer in the server list and printer list.
 *
 *
 * Return value:
 *
 *     A newly allocated full printer name. It is the caller's
 *     responsibility to free the returned String by calling XtFree.
 *
 */
static String
FindSelectedPrinter(
		    Display* display,
		    DtPrintDefaultProcData* dpd)
{
    int printer_count;
    int i;
    int printer_index;

    /*
     * return if any of the structures are not in place
     */
    if(dpd->xp_server_list == (String*)NULL
       ||
       dpd->printer_counts == (int*)NULL
       ||
       dpd->printer_lists == (DtPrintSelectPrinterList*)NULL)
    {
	return (String)NULL;
    }
    /*
     * find the printer list containing the selected printer
     */
    if(0 == dpd->selected_printer)
	return (String)NULL;
    else
	printer_index = dpd->selected_printer - 1;
    
    for(i = 0, printer_count = 0; i < dpd->xp_server_count; i++)
    {
	if(printer_index < printer_count + (dpd->printer_counts)[i])
	    break;
	else
	    printer_count += (dpd->printer_counts)[i];
    }
    if(i < dpd->xp_server_count)
    {
	/*
	 * build and return the printer name
	 */
	DtPrintSelectPrinterList printer_list;
	String printer_spec;
	String printer_name;
	char* printer_name_ct;
	char* display_spec;

	display_spec = (dpd->xp_server_list)[i];
	printer_list = (dpd->printer_lists)[i];
	printer_name_ct =
	    printer_list[printer_index - printer_count].printer_name_ct;
	printer_name =
	    CompoundTextToString(display, (unsigned char*)printer_name_ct);
	if((String)NULL != printer_name)
	{
	    printer_spec =
		_DtPrintCreateXPrinterSpecifier(printer_name,
						display_spec,
						DtPRINT_NET_UNSPECIFIED,
						-1, -1);
	    XtFree(printer_name);
	    return printer_spec;
	}
    }
    return (String)NULL;
}

/*
 * ------------------------------------------------------------------------
 * Name: FreeSelectPrinterData
 *
 * Description:
 *
 *     Deallocate and reset data items associated with the printer
 *     selection dialog.
 *
 * Return value:
 *
 *     None.
 */
static void
FreeSelectPrinterData(
		      DtPrintDefaultProcData* dpd)
{
    int i, j;
    DtPrintSelectPrinterList printer_list;
    
    for(i = 0; i < dpd->xp_server_count; i++)
    {
	printer_list = dpd->printer_lists[i];
	for(j = 0; j < dpd->printer_counts[i]; j++)
	{
	    XtFree(printer_list[j].printer_name_ct);
	}
	XtFree((char*)printer_list);
    }
    XtFree((char*)dpd->printer_lists);
    dpd->printer_lists = (DtPrintSelectPrinterList*)NULL;
    XtFree((char*)dpd->xp_server_list);
    dpd->xp_server_list = (String*)NULL;
    dpd->xp_server_count = 0;
    dpd->selected_printer = 0;
}

/*
 * ------------------------------------------------------------------------
 * Name: GetWMShellAncestor
 *
 * Description:
 *
 *     Obtains the widget ID of the closest WMShell ancestor for
 *     the passed widget.
 *
 * Return value:
 *
 *     The widget ID of the closest WMShell ancestor.
 *
 */
static Widget
GetWMShellAncestor(Widget w)
{
    Widget wmshell_ancestor = XtParent(w);
    while(wmshell_ancestor != (Widget)NULL)
    {
	if(XtIsWMShell(wmshell_ancestor))
	    break;
	else
	    wmshell_ancestor = XtParent(wmshell_ancestor);
    }
    return wmshell_ancestor;
}

/*
 * ------------------------------------------------------------------------
 * Name: HelpDialogDestroyCB
 *
 * Description:
 *
 *     Reset the help dialog widget ID instance data when the widget
 *     is destroyed.
 *
 *     This callback is considered part of the default resource
 *     procedures and not part of the setup box widget proper.
 *
 * Return value:
 *
 *     None.
 *
 */
static void 
HelpDialogDestroyCB(
		    Widget w,
		    XtPointer client_data,
		    XtPointer call_data)
{
    DtPrintDefaultProcData* dpd = &PSUB_DefaultProcData(client_data);
    
    dpd->help_dialog = (Widget)NULL;
}

/*
 * ------------------------------------------------------------------------
 * Name: InfoBoxLayoutCB
 *
 * Description:
 *
 *     Callback to right justify printer information dialog labels at
 *     mapping time.
 *
 * Return value:
 *
 *     None.
 *
 *
 */
static void
InfoBoxLayoutCB(
		Widget widget,
		XtPointer client_data,
		XtPointer call_data)
{
    DtPrintDefaultProcData* dpd = (DtPrintDefaultProcData*)client_data;
    Widget w[4];
    int i, widest;
    Dimension width, max_width;
    /*
     * only need to do this once after the widget is created
     */
    XtRemoveCallback(widget, XmNmapCallback, InfoBoxLayoutCB, client_data);
    /*
     * get the label widget ids
     */
    w[0] = XtNameToWidget(dpd->printer_info_box,
			  "PrinterInfoForm.DescriptionLabel");
    w[1] = XtNameToWidget(dpd->printer_info_box,
			  "PrinterInfoForm.NameLabel");
    w[2] = XtNameToWidget(dpd->printer_info_box,
			  "PrinterInfoForm.FormatLabel");
    w[3] = XtNameToWidget(dpd->printer_info_box,
			  "PrinterInfoForm.ModelLabel");
    /*
     * find the widest label
     */
    for(i = 0, widest = 0, max_width = 0; i < 4; i++)
    {
	XtVaGetValues(w[i], XmNwidth, &width, NULL);
	if(width > max_width)
	{
	    widest = i;
	    max_width = width;
	}
    }
    /*
     * sever the top attachment on the widest label in order to avoid
     * circular dependencies in form children; apparently one of the form
     * widget's many talents is the inability to manage the vertical and
     * horizontal attachments independently...
     */
    XtVaSetValues(w[widest], XmNtopAttachment, XmATTACH_SELF, NULL);
    /*
     * attach the right side of the smaller labels to the right side of
     * the widest label
     */
    for(i = 0; i < 4; i++)
	if(i != widest)
	    XtVaSetValues(w[i],
			  XmNleftAttachment, XmATTACH_NONE,
			  XmNrightAttachment, XmATTACH_OPPOSITE_WIDGET,
			  XmNrightWidget, w[widest],
			  NULL);
}

/*
 * ------------------------------------------------------------------------
 * Name: ParseFileNameSpec
 *
 * Description:
 *
 *     Parse the passed file name to create a file selection box filter
 *     pattern and the name only portion of the full name spec.
 *
 *     It is the caller's responsibility to free the allocated memory
 *     returned in 'pattern' and 'name_only' using XmStringFree and
 *     XtFree respectively.
 *
 * Return value:
 *
 *     None.
 *
 *
 */
static void
ParseFileNameSpec(const char* file_name,
		  XmString* pattern_xmstr,
		  char** name_only)
{
    String ptr;
    String pattern;
    /*
     * start off with a copy of the file name, ensuring there is
     * enough space for the added "*" and the string terminator.
     */
    pattern = XtMalloc(strlen(file_name)+2);
    strcpy(pattern, file_name);
    /*
     * find the last slash
     */
    ptr = DtStrrchr(pattern, '/');
    if(ptr != (String)NULL)
    {
	/*
	 * grab the name portion of the file name
	 */
	*name_only = XtNewString(ptr+1);
	/*
	 * set the wildcard character immediately following the last
	 * slash
	 */
	strcpy(ptr+1, "*");
	*pattern_xmstr = XmStringCreateLocalized(pattern);
    }
    else
    {
	/*
	 * no slash found; use the default pattern
	 */
	*pattern_xmstr = (XmString)NULL;
	*name_only = XtNewString(file_name);
    }
    XtFree(pattern);
}

/*
 * ------------------------------------------------------------------------
 * Name: PresentErrorDialog
 *
 * Description:
 *
 *     This is a utility function that will present an Error Dialog. It
 *     takes a title, a message, and a variable list of Strings to
 *     include in the message. The message parm is treated like a
 *     printf-style format string, except that only the "%s" directive is
 *     supported.
 *
 *     This function is to be used exclusively by the default
 *     resource procedures (e.g. _DtPrintSetupBoxVerifyXPrinterProc).
 *
 *     The variable list of String parms must be terminated by
 *     (String)NULL.
 *
 * Return Value:
 *
 *     None.
 *
 */
static void
PresentErrorDialog(
		   Widget w,
		   String title,
		   String message,
		   ...)
{
    Arg args[15];
    Cardinal n;
    String expanded_message;
    String str_n;
    XmString message_xmstr;
    XmString title_xmstr;
    int message_len;
    va_list arg_marker;
    DtPrintDefaultProcData* dpd = &PSUB_DefaultProcData(w);
    Widget wmshell_ancestor;
    WidgetList wmshell_ancestors_children = (WidgetList)NULL;
    
    wmshell_ancestor = GetWMShellAncestor(w);

    if (wmshell_ancestor)
    {
	n = 0;
	XtSetArg(args[n], XmNchildren, &wmshell_ancestors_children); n++;
	XtGetValues(wmshell_ancestor, args, n);
    }

    if(dpd->error_message_box != (Widget)NULL)
    {
	/*
	 * present only one message box
	 */
	XtDestroyWidget(dpd->error_message_box);
    }
    
    title_xmstr = XmStringCreateLocalized(title);
    /*
     * determine length of expanded message
     */
    message_len = strlen(message);
    va_start(arg_marker, message);
    while((str_n = va_arg(arg_marker, String)) != (String)NULL)
    {
	message_len += strlen(str_n);
    }
    va_end(arg_marker);
    ++message_len;
    /*
     * expand the message
     */
    expanded_message = XtMalloc(message_len);
    va_start(arg_marker, message);
    vsprintf(expanded_message, message, arg_marker);
    va_end(arg_marker);
    /*
     * convert message to XmString
     */
    message_xmstr = XmStringCreateLocalized(expanded_message);
    XtFree(expanded_message);
    /*
     * create the error dialog
     */
    n = 0;
    XtSetArg(args[n], XmNdialogTitle, title_xmstr); n++;
    XtSetArg(args[n], XmNmessageString, message_xmstr); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_PRIMARY_APPLICATION_MODAL); n++;
    XtSetArg(args[n], XmNdeleteResponse, XmDESTROY); n++;
    dpd->error_message_box = 
	XmCreateErrorDialog(wmshell_ancestors_children ?
			    wmshell_ancestors_children[0] :
			    wmshell_ancestor,
			    "_PsubDefProcErrorMsg", args, n);
    XmStringFree(title_xmstr);
    XmStringFree(message_xmstr);

    XtUnmanageChild(XtNameToWidget(dpd->error_message_box, "Cancel"));
    XtUnmanageChild(XtNameToWidget(dpd->error_message_box, "Help"));

    XtAddCallback(dpd->error_message_box, XmNokCallback,
		  DestroyWidgetCB, (XtPointer)dpd->error_message_box);
    XtAddCallback(dpd->error_message_box, XmNcancelCallback,
		  DestroyWidgetCB, (XtPointer)dpd->error_message_box);
    XtAddCallback(dpd->error_message_box, XmNdestroyCallback,
		  ErrorMessageDestroyCB, (XtPointer)w);
    /*
     * manage the message box according to the hint
     */
    if(DtPRINT_HINT_MESSAGES_OK == dpd->messages_hint)
	XtManageChild(dpd->error_message_box);

#ifdef XXX_JUNGLE_REMOVE
    /*
     * manage the message box only if the psub is mapped
     * and the focus is not in the printer name field
     */
    {
	XWindowAttributes attr;
	Status status;
	Window window = XtWindow(w);
	if(window)
	{
	    status = XGetWindowAttributes(XtDisplay(w), window, &attr);
	    if(status == 0 || attr.map_state == IsViewable)
	    {
		Widget name_w = XtNameToWidget(w, "Name");
		Widget child;
		
		for(child = XmGetFocusWidget(w);
		    child != wmshell_ancestor && child != name_w
		    && child != (Widget)NULL;
		    child = XtParent(child));

		if(child != name_w && child != (Widget)NULL)
		    XtManageChild(dpd->error_message_box);
	    }
	}
    }
#endif /* XXX_JUNGLE_REMOVE */
}

/*
 * ------------------------------------------------------------------------
 * Name: PresentHelp
 *
 * Description:
 *
 *     Presents a help dialog whose widget instance may be shared between
 *     the default dialogs.
 *
 * Return value:
 *
 *     None.
 *
 */
static void PresentHelp(
			Widget w,
			const char* help_volume,
			const char* location_id)
{
    DtPrintDefaultProcData* dpd = &PSUB_DefaultProcData(w);
    Widget wmshell_ancestor;
    Widget dialog_shell;
    
    wmshell_ancestor = GetWMShellAncestor(w);

    if((Widget)NULL == dpd->help_dialog)
    {
	Arg args[5];
	Cardinal n;
	XmString title;
	
	n = 0;
	title = XmStringCreateLocalized(HELP_DLG_TITLE);
	XtSetArg(args[n], XmNdialogTitle, title); n++;
	dpd->help_dialog =
	    DtCreateHelpDialog(wmshell_ancestor, "_PsubDefProcHelpDialog",
			       args, n);
	XmStringFree(title);
	XtAddCallback(dpd->help_dialog, XmNdestroyCallback,
		      HelpDialogDestroyCB, (XtPointer)w);
    }
    /*
     * set the help volume and location
     */
    XtVaSetValues(dpd->help_dialog,
		  DtNhelpVolume, help_volume,
		  DtNlocationId, location_id,
		  DtNhelpType, DtHELP_TYPE_TOPIC,
		  NULL);
    /*
     * pop up the help dialog
     */
    XtManageChild(dpd->help_dialog);
    dialog_shell = XtParent(dpd->help_dialog);
    if(None != XtWindow(dialog_shell))
	XRaiseWindow(XtDisplay(dialog_shell), XtWindow(dialog_shell));
}

/*
 * ------------------------------------------------------------------------
 * Name: PresentVerifyError
 *
 * Description:
 *
 *     Presents a error dialog appropriate for an error status returned
 *     by _DtPrintVerifyXPrinter.
 *
 * Return value:
 *
 *     None.
 *
 */
static void
PresentVerifyError(
		   Widget w,
		   XtEnum status,
		   String printer_spec)
{
    String printer_name;
    String display_spec;
    
    _DtPrintParseXPrinterSpecifier(printer_spec,
				   &printer_name,
				   &display_spec);
    switch(status)
    {
    case DtPRINT_PRINTER_MISSING:
	PresentErrorDialog(w, INVALID_PRINTER_TITLE,
			   PRINTER_MISSING_MESSAGE,
			   printer_spec,
			   (String)NULL);
	break;

    case DtPRINT_NO_DEFAULT:
	PresentErrorDialog(w, INVALID_PRINTER_TITLE,
			   NO_DEFAULT_MESSAGE,
			   (String)NULL);
	break;

    case DtPRINT_NO_DEFAULT_DISPLAY:
	PresentErrorDialog(w, INVALID_PRINTER_TITLE,
			   NO_DEFAULT_DISPLAY_MESSAGE,
			   printer_name,
			   (String)NULL);
	break;
	
    case DtPRINT_NO_PRINTER:
	PresentErrorDialog(w, INVALID_PRINTER_TITLE,
			   INVALID_PRINTER_MESSAGE,
			   printer_name, display_spec,
			   (String)NULL);
	break;

    case DtPRINT_NOT_XP_DISPLAY:
	PresentErrorDialog(w, INVALID_PRINTER_TITLE,
			   NOT_XP_DISPLAY_MESSAGE,
			   display_spec,
			   (String)NULL);
	break;

    case DtPRINT_INVALID_DISPLAY:
	PresentErrorDialog(w, INVALID_PRINTER_TITLE,
			   INVALID_DISPLAY_MESSAGE,
			   display_spec,
			   (String)NULL);
	break;
    }
    XtFree(printer_name);
    XtFree(display_spec);
}


/*
 * ------------------------------------------------------------------------
 * Name: PrinterInfoDestroyCB
 *
 * Description:
 *
 *     Reset data items used in conjunction with the
 *     printer information selection dialog.
 *
 * Return value:
 *
 *     None.
 *
 */
static void
PrinterInfoDestroyCB(
		    Widget w,
		    XtPointer client_data,
		    XtPointer call_data)
{
    DtPrintDefaultProcData* dpd = &PSUB_DefaultProcData(client_data);

    dpd->printer_info_box = (Widget)NULL;
}

/*
 * ------------------------------------------------------------------------
 * Name: PrinterInfoHelpCB
 *
 * Description:
 *
 *     Display context sensitive help.
 *
 * Return value:
 *
 *     None.
 *
 */
static void
PrinterInfoHelpCB(
		  Widget w,
		  XtPointer client_data,
		  XtPointer call_data)
{
    PresentHelp((Widget)client_data, HELP_VOLUME, PRINTER_INFO_HELP_ID);
}

/*
 * ------------------------------------------------------------------------
 * Name: SelectFileDestroyCB
 *
 * Description:
 *
 *     Reset data items used in conjunction with the
 *     file selection dialog.
 *
 * Return value:
 *
 *     None.
 *
 */
static void
SelectFileDestroyCB(
		    Widget w,
		    XtPointer client_data,
		    XtPointer call_data)
{
    DtPrintDefaultProcData* dpd = &PSUB_DefaultProcData(client_data);

    dpd->file_selection_box = (Widget)NULL;
}

/*
 * ------------------------------------------------------------------------
 * Name: SelectFileHelpCB
 *
 * Description:
 *
 *     Display context sensitive help.
 *
 * Return value:
 *
 *     None.
 *
 */
static void
SelectFileHelpCB(
		 Widget w,
		 XtPointer client_data,
		 XtPointer call_data)
{
    PresentHelp((Widget)client_data, HELP_VOLUME, SELECT_FILE_HELP_ID);
}

/*
 * ------------------------------------------------------------------------
 * Name: SelectPrinterCB
 *
 * Description:
 *
 *     Callback to handle the printer selection box OK and Cancel
 *     buttons.
 *
 * Return value:
 *
 *     None.
 *
 */
static void
SelectPrinterCB(
		Widget w,
		XtPointer client_data,
		XtPointer call_data)
{
    XmSelectionBoxCallbackStruct* cbs =
	(XmSelectionBoxCallbackStruct*)call_data;
    Widget psub = (Widget)client_data;
    DtPrintDefaultProcData* dpd = &PSUB_DefaultProcData(psub);
    String printer_spec;

    /*
     * close the info dialog display connection if it's been opened
     */
    CloseSelectPrinterInfoConnection(dpd);

    switch(cbs->reason)
    {
    case XmCR_OK:
	/*
	 * set the selected printer name in the PrintSetupBox
	 */
	printer_spec = FindSelectedPrinter(XtDisplay(psub), dpd);
	if((String)NULL != printer_spec)
	{
	    XtVaSetValues(psub, DtNprinterName, printer_spec, NULL);
	    XtFree(printer_spec);
	}
	/*
	 * no break
	 */

    case XmCR_CANCEL:
	/*
	 * deallocate and reset the lists
	 */
	FreeSelectPrinterData(dpd);
	if(dpd->printer_list_box != (Widget)NULL)
	    XmListDeleteAllItems(dpd->printer_list_box);
	break;
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: SelectPrinterDestroyCB
 *
 * Description:
 *
 *     Deallocate and reset data items used in conjunction with the
 *     printer selection dialog.
 *
 * Return value:
 *
 *     None.
 *
 */
static void
SelectPrinterDestroyCB(
		       Widget w,
		       XtPointer client_data,
		       XtPointer call_data)
{
    DtPrintDefaultProcData* dpd = &PSUB_DefaultProcData(client_data);

    dpd->printer_selection_box = (Widget)NULL;
    dpd->printer_list_box = (Widget)NULL;
    CloseSelectPrinterInfoConnection(dpd);
    FreeSelectPrinterData(dpd);
}

/*
 * ------------------------------------------------------------------------
 * Name: SelectPrinterHelpCB
 *
 * Description:
 *
 *     Display context sensitive help.
 *
 * Return value:
 *
 *     None.
 *
 */
static void
SelectPrinterHelpCB(
		    Widget w,
		    XtPointer client_data,
		    XtPointer call_data)
{
    PresentHelp((Widget)client_data, HELP_VOLUME, SELECT_PRINTER_HELP_ID);
}

/*
 * ------------------------------------------------------------------------
 * Name: SelectPrinterInfoCB
 *
 * Description:
 *
 *     Callback attached to the printer selection box "Info..."
 *     button. The "Info..." button is intended to behave just like the
 *     PrintSetupBox "Info..." button, so it is implemented to call the
 *     procedure set for the PrintSetupBox DtNprinterInfoProc resource.
 *     As such, this function needs to open a new display connection to
 *     the print server indicated by the currently selected printer in
 *     the list. The connection is not closed until a selection is made
 *     in the list, or the printer selection dialog is dismissed. This is
 *     because no assumptions about the Printer Info dialog's widget
 *     heirarchy can be made.
 *
 * Return value:
 *
 *     None.
 *
 */
static void
SelectPrinterInfoCB(
		    Widget w,
		    XtPointer client_data,
		    XtPointer call_data)
{
    Widget psub = (Widget)client_data;
    DtPrintDefaultProcData* dpd = &PSUB_DefaultProcData(psub);
    DtPrintSetupProc info_proc;

    CloseSelectPrinterInfoConnection(dpd);
    XtVaGetValues(psub, DtNprinterInfoProc, &info_proc, NULL);
    if((DtPrintSetupProc)NULL != info_proc)
    {
	String printer_spec;
	
	printer_spec = FindSelectedPrinter(XtDisplay(psub), dpd);
	if((String)NULL != printer_spec)
	{
	    String new_printer_spec; /*
				      * will always be set to NULL by
				      * _DtPrintVerifyXPrinter when
				      * called from within this routine
				      */
	    XtEnum status;
	    
	    /*
	     * open a connection to the X printer
	     */
	    status =
		_DtPrintVerifyXPrinter(psub,
				       printer_spec,
				       &new_printer_spec,
				       &dpd->select_printer_info_display,
				       &dpd->select_printer_info_context);
	    if(status == DtPRINT_SUCCESS)
	    {
		DtPrintSetupData psd;

		memset(&psd, 0, sizeof(DtPrintSetupData));
		psd.printer_name = printer_spec;
		psd.print_display = dpd->select_printer_info_display;
		psd.print_context = dpd->select_printer_info_context;
		(*info_proc)(psub, &psd);
	    }
	    else
	    {
		/*
		 * this should only happen if, while a user is viewing
		 * the printer selection dialog, a server rehash is
		 * performed that deletes the currently selected printer
		 */
		PresentVerifyError(w, status, printer_spec);
	    }
	    XtFree(printer_spec);
	}
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: SelectPrinterItemCB
 *
 * Description:
 *
 *     Callback to handle the a selection in the printer selection list
 *     box.
 *
 * Return value:
 *
 *     None.
 *
 */
static void
SelectPrinterItemCB(
		    Widget w,
		    XtPointer client_data,
		    XtPointer call_data)
{
    XmListCallbackStruct* cbs =
	(XmListCallbackStruct*)call_data;
    Widget psub = (Widget)client_data;
    DtPrintDefaultProcData* dpd = &PSUB_DefaultProcData(psub);
    Widget info_button;

    if(dpd->selected_printer != cbs->item_position)
    {
	dpd->selected_printer = cbs->item_position;
	info_button = XtNameToWidget(dpd->printer_selection_box, "*Apply");
	if(0 == dpd->selected_printer)
	    XtSetSensitive(info_button, False);
	else
	    XtSetSensitive(info_button, True);
	/*
	 * update the default printer info dialog if it is visible
	 */
	if(dpd->printer_info_box && XtIsManaged(dpd->printer_info_box))
	    SelectPrinterInfoCB(w, client_data, (XtPointer)NULL);
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: SetListBoxSelection
 *
 * Description:
 *
 *
 * Return value:
 *
 *     None.
 *
 */
static void
SetListBoxSelection(Widget list_box, int position)
{
    int visible_item_count;
    int item_count;
    int middle_offset;
    int first_visible_pos;
    /*
     * Scroll the list, making the item at the indicated position
     * visible in the center of the list box, and make
     * it the initial selection.
     */
    XtVaGetValues(list_box,
		  XmNitemCount, &item_count,
		  XmNvisibleItemCount, &visible_item_count,
		  NULL);

    if(item_count > visible_item_count)
    {
	middle_offset = (visible_item_count+1) / 2;
	if(position > middle_offset)
	    if(position > item_count - middle_offset)
		first_visible_pos = item_count - visible_item_count +1;
	    else
		first_visible_pos = position - middle_offset + 1;
	else
	    first_visible_pos = 1;
	XmListSetPos(list_box, first_visible_pos);
    }

    if(position > 0)
	XmListSelectPos(list_box, position, True);
    else
	XmListDeselectAllItems(list_box);
}

/*
 * ------------------------------------------------------------------------
 * Name: UpdateFileNameCB
 *
 * Description:
 *
 *     Updates the PrintSetupBox fileName resource based on the selection
 *     make in the file selection box.
 *
 * Return value:
 *
 *     None.
 *
 */
static void 
UpdateFileNameCB(
		 Widget w,
		 XtPointer client_data,
		 XtPointer call_data)
{
    Widget text_field;
    Widget psub = (Widget)client_data;
    String file_name;
    /*
     * get the file name from the file selection box text field
     */
    text_field = XtNameToWidget(w, "Text");
    if(text_field)
    {
	XtVaGetValues(text_field, XmNvalue, &file_name, NULL);
	if(file_name)
	{
	    /*
	     * set the file name in the print setup box
	     */
	    XtVaSetValues(psub, DtNfileName, file_name, NULL);
	    /*
	     * free the file name retrieved from the text field
	     */
	    XtFree(file_name);
	}
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: _DtPrintDefProcInitialize
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *
 */
void
_DtPrintDefProcInitialize(Widget w)
{
    DtPrintDefaultProcData* dpd = &PSUB_DefaultProcData(w);
    
    dpd->error_message_box = (Widget)NULL;
    dpd->messages_hint = DtPRINT_HINT_MESSAGES_OK;
    dpd->help_dialog = (Widget)NULL;
    dpd->file_selection_box = (Widget)NULL;
    dpd->printer_selection_box = (Widget)NULL;
    dpd->printer_list_box = (Widget)NULL;
    dpd->xp_server_list = (String*)NULL;
    dpd->xp_server_count = 0;
    dpd->printer_lists = (DtPrintSelectPrinterList*)NULL;
    dpd->selected_printer = 0;
    dpd->select_printer_info_display = (Display*)NULL;
    dpd->select_printer_info_context = (XPContext)None;
    dpd->printer_info_box = (Widget)NULL;

    XtAddCallback(w, XmNdestroyCallback,
		  DtPrintDefProcDestroyCB, (XtPointer)NULL);
}

/*
 * ------------------------------------------------------------------------
 * Name: _DtPrintDefProcManageErrorBox
 *
 * Description:
 *
 *     Manage the Error Message Box if it exists.
 *
 * Return value:
 *
 *
 */
void
_DtPrintDefProcManageErrorBox(
			      DtPrintDefaultProcData* dpd)
{
    if(dpd->error_message_box != (Widget)NULL)
	XtManageChild(dpd->error_message_box);
}

/*
 * ------------------------------------------------------------------------
 * Name: _DtPrintSetupBoxXPrinterInfoProc
 *
 * Description:
 *
 *     Default Xp mode function for the DtNprinterInfoProc resource.
 *
 * Return value:
 *
 *
 */
XtEnum
_DtPrintSetupBoxXPrinterInfoProc(
				 Widget w,
				 DtPrintSetupData* print_data)
{
    DtPrintDefaultProcData* dpd = &PSUB_DefaultProcData(w);
    Widget wmshell_ancestor;
    Widget ctl;
    XmString label;
    char* attr_value_ct;
    String attr_value;
    XmString empty_label;
    Widget name_label_top;
    Position max_y, y;
    Dimension height;
    Widget manager = NULL;
    Widget dialog_shell;
    
    wmshell_ancestor = GetWMShellAncestor(w);
    _DtTurnOnHourGlass(wmshell_ancestor);
    empty_label = XmStringCreateLocalized("");
    /*
     * if needed, create the printer information dialog
     */
    if(dpd->printer_info_box == (Widget)NULL)
    {
	XtEnum status = CreatePrinterInfoBox(wmshell_ancestor, w, dpd);
	if(status != DtPRINT_SUCCESS)
	{
	    _DtTurnOffHourGlass(wmshell_ancestor);
	    return status;
	}
	dialog_shell = XtParent(dpd->printer_info_box);
    }
    else
    {
	/*
	 * hide the widget during re-layout
	 */
	manager = XtNameToWidget(dpd->printer_info_box, "*PrinterInfoForm");
	XtUnmapWidget(dpd->printer_info_box);
	dialog_shell = XtParent(dpd->printer_info_box);
	XtVaSetValues(dialog_shell, XmNallowShellResize, False, NULL);
	XtUnmanageChild(manager);
    }
    /*
     * description
     */
    ctl = XtNameToWidget(dpd->printer_info_box, "*Description");
    XtVaSetValues(ctl, XmNlabelString, empty_label, NULL);
    attr_value_ct = XpGetOneAttribute(print_data->print_display,
				      print_data->print_context,
				      XPPrinterAttr,
				      "descriptor");
    if((char*)NULL != attr_value_ct)
    {
	attr_value =
	    CompoundTextToString(XtDisplay(w), (unsigned char*)attr_value_ct);
	XFree(attr_value_ct);
	if((String)NULL != attr_value)
	{
	    label = XmStringGenerate((XtPointer)attr_value, (XmStringTag)NULL,
				     XmMULTIBYTE_TEXT, (XmStringTag)NULL);
	    XtFree(attr_value);
	    XtVaSetValues(ctl, XmNlabelString, label, NULL);
	    XmStringFree(label);
	}
    }
    /*
     * set the top attachment for the name label
     */
    XtVaGetValues(ctl, XmNy, &y, XmNheight, &height, NULL);
    max_y = y + height;
    name_label_top = ctl;
    ctl = XtNameToWidget(dpd->printer_info_box, "*DescriptionLabel");
    XtVaGetValues(ctl, XmNy, &y, XmNheight, &height, NULL);
    if(y+height > max_y)
	name_label_top = ctl;
    ctl = XtNameToWidget(dpd->printer_info_box, "*NameLabel");
    XtVaSetValues(ctl, XmNtopWidget, name_label_top, NULL);
    /*
     * printer name
     */
    ctl = XtNameToWidget(dpd->printer_info_box, "*Name");
    label = XmStringCreateLocalized(print_data->printer_name);
    XtVaSetValues(ctl, XmNlabelString, label, NULL);
    XmStringFree(label);
    /*
     * document format
     */
    ctl = XtNameToWidget(dpd->printer_info_box, "*Format");
    XtVaSetValues(ctl, XmNlabelString, empty_label, NULL);
    attr_value = XpGetOneAttribute(print_data->print_display,
				   print_data->print_context,
				   XPDocAttr,
				   "document-format");
    if((String)NULL != attr_value)
    {
	char* format_start;
	format_start = strchr(attr_value, '{');
	if((char*)NULL != format_start)
	{
	    char* format_end;
	    ++format_start;
	    format_end = strchr(format_start, '}');
	    if((char*)NULL != format_end)
	    {
		*format_end = '\0';
		label = XmStringCreateLocalized(format_start);
		XtVaSetValues(ctl, XmNlabelString, label, NULL);
		XmStringFree(label);
	    }
	}
	XFree(attr_value);
    }
    /*
     * printer model
     */
    ctl = XtNameToWidget(dpd->printer_info_box, "*Model");
    XtVaSetValues(ctl, XmNlabelString, empty_label, NULL);
    attr_value_ct = XpGetOneAttribute(print_data->print_display,
				      print_data->print_context,
				      XPPrinterAttr,
				      "printer-model");
    if((char*)NULL != attr_value_ct)
    {
	attr_value =
	    CompoundTextToString(XtDisplay(w), (unsigned char*)attr_value_ct);
	XFree(attr_value_ct);
	if((String)NULL != attr_value)
	{
	    label = XmStringGenerate((XtPointer)attr_value, (XmStringTag)NULL,
				     XmCHARSET_TEXT, (XmStringTag)NULL);
	    XtFree(attr_value);
	    XtVaSetValues(ctl, XmNlabelString, label, NULL);
	    XmStringFree(label);
	}
    }
    /*
     * pop up the printer info dialog and return
     */
    XmStringFree(empty_label);
    if(manager)
    {
	XtVaSetValues(dialog_shell, XmNallowShellResize, True, NULL);
	XtManageChild(manager);
	XtMapWidget(dpd->printer_info_box);
    }
    XtManageChild(dpd->printer_info_box);
    if(None != XtWindow(dialog_shell))
	XRaiseWindow(XtDisplay(dialog_shell), XtWindow(dialog_shell));
    _DtTurnOffHourGlass(wmshell_ancestor);
    return DtPRINT_SUCCESS;
}

/*
 * ------------------------------------------------------------------------
 * Name: _DtPrintSetupBoxSelectFileProc
 *
 * Description:
 *
 *     Default function for the DtNselectFileProc resource.
 *
 * Return value:
 *
 *
 */
XtEnum
_DtPrintSetupBoxSelectFileProc(
			       Widget w,
			       DtPrintSetupData* print_data)
{
    DtPrintDefaultProcData* dpd = &PSUB_DefaultProcData(w);
    String name_only;
    String file_name;
    XmString pattern;
    Widget file_list;
    Widget text_field;
    Widget wmshell_ancestor;
    
    wmshell_ancestor = GetWMShellAncestor(w);
    _DtTurnOnHourGlass(wmshell_ancestor);
    /*
     * if needed, create the file selection dialog
     */
    if(dpd->file_selection_box == (Widget)NULL)
    {
	XtEnum status = CreateFileSelectionBox(wmshell_ancestor, w, dpd);
	if(status != DtPRINT_SUCCESS)
	{
	    _DtTurnOffHourGlass(wmshell_ancestor);
	    return status;
	}
    }
    /*
     * get the file name passed from the PrintSetupBox
     */
    file_name = print_data->dest_info;
    /*
     * parse the file name to create the filter pattern and the name only
     * portion of the full name spec
     */
    ParseFileNameSpec(file_name, &pattern, &name_only);
    /*
     * set the filter pattern in the file selection box
     */
    XtVaSetValues(dpd->file_selection_box, XmNpattern, pattern, NULL);
    if(pattern != (XmString)NULL)
	XmStringFree(pattern);
    /*
     * select the current file name in the file names list
     */
    file_list = XtNameToWidget(dpd->file_selection_box, "*ItemsList");
    if(file_list != (Widget)NULL)
    {
	XmString file_name_xmstr;
	int position;

	text_field = XtNameToWidget(dpd->file_selection_box, "Text");
	if(text_field != (Widget)NULL)
	{
	    String dir_spec;
	    /*
	     * recreate the file name from the name_only and the current
	     * dir spec as determined by the file selection box
	     *
	     * Note: the char* value retrieved directly from the text
	     * field is used, because forming the file name from a
	     * concatenation of XmStrings causes XmListItemPos to fail
	     * even though the file name is actually in the list.
	     */
	    XtVaGetValues(text_field, XmNvalue, &dir_spec, NULL);
	    file_name = XtMalloc(strlen(dir_spec)+strlen(name_only)+1);
	    strcpy(file_name, dir_spec);
	    strcat(file_name, name_only);
	    file_name_xmstr = XmStringCreateLocalized(file_name);
	    XtFree(dir_spec);
	    XtFree(file_name);
	}
	else
	    file_name_xmstr = XmStringCreateLocalized(file_name);
	/*
	 * find the position of the file name in the list
	 */
	position = XmListItemPos(file_list, file_name_xmstr);
	SetListBoxSelection(file_list, position);
	if(position == 0)
	{
	    /*
	     * The file name is not in the list. Manually set in the
	     * "Selection" text field.
	     */
	    XtVaSetValues(dpd->file_selection_box,
			  XmNdirSpec, file_name_xmstr,
			  NULL);
	}
	XmStringFree(file_name_xmstr);
    }
    XtFree(name_only);
    /*
     * pop up the file selection dialog and return
     */
    XtManageChild(dpd->file_selection_box);
    _DtTurnOffHourGlass(wmshell_ancestor);
    return DtPRINT_SUCCESS;
}

/*
 * ------------------------------------------------------------------------
 * Name: _DtPrintSetupBoxSelectXPrinterProc
 *
 * Description:
 *
 *     Default Xp mode function for the DtNselectPrinterProc resource.
 *
 * Return value:
 *
 *
 */
XtEnum
_DtPrintSetupBoxSelectXPrinterProc(
				   Widget w,
				   DtPrintSetupData* print_data)
{
    DtPrintDefaultProcData* dpd = &PSUB_DefaultProcData(w);
    Widget wmshell_ancestor;
    XmStringTable items;
    int item_count;
    int i;
    XmTabList tab_list;
    XmRenderTable render_table, new_render_table;
    int count;
    XmStringTag* tags;
    XmRendition rendition;
    Arg args[2];
    Cardinal n;

    wmshell_ancestor = GetWMShellAncestor(w);
    _DtTurnOnHourGlass(wmshell_ancestor);
    /*
     * build the list of servers and printers
     */
    if(DtPRINT_SUCCESS != BuildPrinterLists(w, dpd, &item_count))
    {
	PresentErrorDialog(w, PRLIST_ERROR_TITLE,
			   NO_PRINTERS_MESSAGE,
			   (String)NULL);
	_DtTurnOffHourGlass(wmshell_ancestor);
	return DtPRINT_FAILURE;
    }
    /*
     * create the dialog if it doesn't exist
     */
    if(dpd->printer_selection_box == (Widget)NULL)
    {
	XtEnum status = CreatePrinterSelectionBox(wmshell_ancestor, w, dpd);
	if(status != DtPRINT_SUCCESS)
	{
	    _DtTurnOffHourGlass(wmshell_ancestor);
	    return status;
	}
    }
    /*
     * get a copy of the list widget's render table
     */
    XtVaGetValues(dpd->printer_list_box,
		  XmNrenderTable, &render_table,
		  NULL);
    render_table = XmRenderTableCopy(render_table, (XmStringTag*)NULL, 0);
    /*
     * build the item list for the list box
     */
    count = XmRenderTableGetTags(render_table, &tags);
    items = BuildPrinterSelectionItems(XtDisplay(w), dpd, item_count,
				       print_data->printer_name, tags[0]);
    /*
     * generate a tab list for the items
     */
    tab_list = XmStringTableProposeTablist(items, item_count,
					   dpd->printer_list_box,
					   20, XmRELATIVE);
    /*
     * get a copy of the first rendition in the render table
     */
    rendition = XmRenderTableGetRendition(render_table, tags[0]);
    for(i = 0; i < count; i++)
	XtFree(tags[i]);
    XtFree((char*)tags);
    /*
     * update the copy with the new tab list
     */
    n = 0;
    XtSetArg(args[n], XmNtabList, tab_list); n++;
    XmRenditionUpdate(rendition, args, n);
    XmTabListFree(tab_list);
    /*
     * create a new render table replacing the first entry with the
     * updated rendition
     */
    new_render_table = XmRenderTableAddRenditions(render_table, &rendition,
						  1, XmMERGE_REPLACE);
    XmRenditionFree(rendition);
    /*
     * set the new render table and list items in the list box
     */
    XtVaSetValues(dpd->printer_list_box,
		  XmNrenderTable, new_render_table,
		  XmNitems, items,
		  XmNitemCount, item_count,
		  NULL);
    XmRenderTableFree(new_render_table);
    for(i = 0; i < item_count; i++)
	XmStringFree(items[i]);
    XtFree((char*)items);
    SetListBoxSelection(dpd->printer_list_box, dpd->selected_printer);
    /*
     * disable the info button if nothing is selected
     */
    if(0 == dpd->selected_printer)
	XtSetSensitive(XtNameToWidget(dpd->printer_selection_box, "*Apply"),
		       False);
    /*
     * manage the dialog and return
     */
    XtManageChild(dpd->printer_selection_box);
    _DtTurnOffHourGlass(wmshell_ancestor);
    return DtPRINT_SUCCESS;
}

/*
 * ------------------------------------------------------------------------
 * Name: _DtPrintSetupBoxVerifyXPrinterProc
 *
 * Description:
 *
 *     Default function for the DtNverifyPrinterProc resource when the
 *     setup mode is XP.
 *
 *     (this rest of this description outlines the responsibilities of a
 *     DtNverifyPrinterProc in general)
 *
 *     This function verifies the printer name passed in the
 *     PrintSetupData structure.
 *
 *     If needed, the DtNprintSetupMode resource may be obtained via a
 *     GetValues call in order to determine the current setup mode.
 *
 *     It is the responsibility of this proc to return a valid print
 *     Display handle and print context by setting the print_display and
 *     print_context in the PrintSetupData. Conceptually, this is a
 *     side-effect of this proc, but since it will be necessary to open a
 *     display connection in order to properly verify the X printer,
 *     there's no point in doing it twice.
 *
 *     If the passed printer name is incomplete, this proc may attempt to
 *     determine a default or fully-qualified name (e.g. fill in a
 *     missing display spec for an X Printer Specifier). The new default
 *     or fully-qualified printer name should be set in the PrintSetupBox
 *     by updating the DtNprinterName resource via a SetValues call. If a
 *     fully-qualified name cannot be determined, this proc simply
 *     returns DtPRINT_FAILURE.
 *
 *     If the printer cannot be verified, it is the responsibiliy of this
 *     function to present an appropriate message to the user.
 *
 * Return value:
 *
 *     DtPRINT_SUCCESS
 *         if the printer name has been successfully verified.
 *     
 *     DtPRINT_FAILURE
 *         if the printer name is invalid.
 *
 *
 */
XtEnum
_DtPrintSetupBoxVerifyXPrinterProc(
				   Widget w,
				   DtPrintSetupData* psd)
{
    DtPrintDefaultProcData* dpd = &PSUB_DefaultProcData(w);
    XtEnum status;
    String new_printer_spec;
    Display* new_display;
    XPContext new_context;
    Widget wmshell_ancestor;
    
    wmshell_ancestor = GetWMShellAncestor(w);
    _DtTurnOnHourGlass(wmshell_ancestor);
    /*
     * verify the printer
     */
    status = _DtPrintVerifyXPrinter(w,
				    psd->printer_name,
				    &new_printer_spec,
				    &new_display,
				    &new_context);
    if(status == DtPRINT_SUCCESS)
    {
	/*
	 * update the passed print setup data with the new display and
	 * context
	 */
	psd->print_display = new_display;
	psd->print_context = new_context;
    }
    else
    {
	/*
	 * unable to open the printer; present a message to the user
	 * according to the hint set in the setup data
	 */
	dpd->messages_hint = psd->messages_hint;
	PresentVerifyError(w, status,
			   new_printer_spec
			   ? new_printer_spec : psd->printer_name);
	/*
	 * reset the hint since the error dialog is also used
	 * by the other default procs
	 */
	dpd->messages_hint = DtPRINT_HINT_MESSAGES_OK;
	/*
	 * this function only indicates success or failure
	 */
	status = DtPRINT_FAILURE;
    }
    if(new_printer_spec)
    {
	/*
	 * set the new printer spec, even if the verify failed
	 */
	XtVaSetValues(w,
		      DtNprinterName, new_printer_spec,
		      NULL);
	XtFree(new_printer_spec);
    }
    /*
     * return
     */
    _DtTurnOffHourGlass(wmshell_ancestor);
    return status;
}
