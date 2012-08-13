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
/* $XConsortium: Filter.c /main/8 1996/10/30 11:10:48 drk $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 *   FILE:           Filter.c
 *
 *   COMPONENT_NAME: Desktop File Manager (dtfile)
 *
 *   Description:    Source file for the file filter dialog.
 *
 *   FUNCTIONS: ApplyCallback
 *		Create
 *		DefaultCallback
 *		DeselectAllFilter
 *		Destroy
 *		FileTypeAscending
 *		FreeValues
 *		GetDefaultValues
 *		GetFilterValues
 *		GetResourceValues
 *		GetValues
 *		InstallChange
 *		InstallClose
 *		OkCallback
 *		ReadInFiletypes
 *		SelectAllFilter
 *		SetValues
 *		ShowHiddenFiles
 *		UpdateFilterAfterDBReread
 *		WriteResourceValues
 *		filterTypeChanged
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *
 ****************************************************************************
 ************************************<+>*************************************/


#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include <Xm/XmP.h>
#include <Xm/Xm.h>
#include <Xm/DialogS.h>
#include <Xm/DrawingA.h>
#include <Xm/Form.h>
#include <Xm/LabelG.h>
#include <Xm/List.h>
#include <Xm/MessageB.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/TextF.h>
#include <Xm/ToggleBG.h>
#include <Xm/ScrollBar.h>
#include <Xm/ScrolledW.h>
#include <Xm/SeparatoG.h>
#include <Xm/VendorSEP.h>
#include <Xm/MwmUtil.h>
#include <Xm/Protocols.h>

#include <Dt/TitleBox.h>
#include <Dt/Icon.h>
#include <Dt/IconP.h>
#include <Dt/IconFile.h>

#include <X11/ShellP.h>
#include <X11/Shell.h>
#include <X11/Xatom.h>

#include <Dt/DtP.h>
#include <Dt/DtNlUtils.h>

#include "Encaps.h"
#include "SharedProcs.h"

#include "FileMgr.h"
#include "Desktop.h"
#include "Main.h"
#include "Common.h"
#include "Filter.h"
#include "Help.h"

char **DtDtsDataTypeNames(void);

static char * FILE_FILTER = "FileFilter";


static DialogResource resources[] =
{
   { "matchFilter", XmRBoolean, sizeof(Boolean),
     XtOffset(FilterDataPtr, match_flag),
     (XtPointer) True, _DtBooleanToString },

   { "name", XmRString, sizeof(String),
     XtOffset(FilterDataPtr, filter),
     (XtPointer) "", _DtStringToString },

   { "showHidden", XmRBoolean, sizeof(Boolean),
     XtOffset(FilterDataPtr, show_hidden),
     (XtPointer) False, _DtBooleanToString },

   { "filetypesFilteredCount", XmRInt, sizeof(int),
     XtOffset(FilterDataPtr, filetypesFilteredCount),
     (XtPointer) 3, _DtIntToString },

   { "filetypesFiltered", XmRString, sizeof(String),
     XtOffset(FilterDataPtr, filetypesFiltered),
     (XtPointer) DEFAULT_FILTER, _DtStringToString },
};


#define FM_TYPE_LABEL  "TYPE_LABEL"


/********    Static Function Declarations    ********/

static void Create(
                        Display *display,
                        Widget parent,
                        Widget *return_widget,
                        XtPointer *dialog) ;
static void InstallChange(
                        FilterRec *filter_rec,
                        XtCallbackProc callback,
                        XtPointer client_data) ;
static void InstallClose(
                        FilterRec *filter_rec,
                        XtCallbackProc callback,
                        XtPointer client_data) ;
static void Destroy(
                        FilterRec *filter_rec) ;
static XtPointer GetValues(
                        FilterRec *filter_rec) ;
static XtPointer GetDefaultValues( void ) ;
static XtPointer GetResourceValues(
                        XrmDatabase data_base,
                        char **name_list) ;
static void SetValues(
                        FilterRec *filter_rec,
                        FilterData *filter_data) ;
static void WriteResourceValues(
                        DialogData *values,
                        int fd,
                        char **name_list) ;
static void FreeValues(
                        FilterData *filter_data) ;
static void DefaultCallback(
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void CancelCallback(
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void OkCallback(
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void ApplyCallback(
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static Boolean GetFilterValues(
                        FilterRec *filter_rec,
                        FilterData *filter_data) ;
static void filterTypeChanged(
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data ) ;
static int FileTypeAscending(
                       FTData **t1,
                       FTData **t2 );
static void SelectAllFilter(
                       Widget w,
                       XEvent *event,
                       String *params,
                       Cardinal *num_params );
static void DeselectAllFilter(
                       Widget w,
                       XEvent *event,
                       String *params,
                       Cardinal *num_params );
/********    End Static Function Declarations    ********/




static DialogClass filterClassRec =
{
   resources,
   XtNumber(resources),
   Create,
   (DialogInstallChangeProc) InstallChange,
   (DialogInstallCloseProc) InstallClose,
   (DialogDestroyProc) Destroy,
   (DialogGetValuesProc) GetValues,
   GetDefaultValues,
   GetResourceValues,
   (DialogSetValuesProc) SetValues,
   WriteResourceValues,
   (DialogFreeValuesProc) FreeValues,
   (DialogMapWindowProc) _DtGenericMapWindow,
   NULL,
};

DialogClass * filterClass = (DialogClass *) &filterClassRec;

static XtActionsRec FilterAction[] = {
{ "SelectAllFilter", SelectAllFilter },
{ "DeselectAllFilter", DeselectAllFilter }
};

static char FilterTranslationString[] = "\
Ctrl<Key>slash:SelectAllFilter(@)\n\
Ctrl<Key>backslash:DeselectAllFilter(@)";

/************************************************************************
 *
 *  Create
 *
 ************************************************************************/

static void
Create(
        Display *display,
        Widget parent,
        Widget *return_widget,
        XtPointer *dialog )
{
   FilterRec * filter_rec;
   Widget shell, form, fileNameFilter;
   Widget tog_frame, tog_form, fileTypeScrollWin, file_window;
   Widget labelPt2, hidden, separator, sel_form;
   Widget ok, apply, reset, close, help;
   Widget selectAll, unselectAll;
   Widget hiddenPulldown;
   XmString label_string;
   XtTranslations filterTranslationTable;
   int flag=FLAG_RESET;

   Arg args[20];
   int n;
   char * tmpStr, * resolvedFilterTranslationString;

   /*  Allocate the file filter dialog instance record.  */
   filter_rec = (FilterRec *) XtMalloc (sizeof (FilterRec));

   /*  Create the shell and form used for the dialog.  */
   n = 0;
   XtSetArg (args[n], XmNmwmFunctions, MWM_FUNC_MOVE |
             MWM_FUNC_CLOSE);                                           ++n;
   XtSetArg (args[n], XmNmwmDecorations, MWM_DECOR_BORDER |
             MWM_DECOR_TITLE);                                          ++n;
   XtSetArg (args[n], XmNallowShellResize, True);                       ++n;
   shell = XmCreateDialogShell (parent, "filter_files", args, n);

   /* Set the useAsyncGeo on the shell */
   XtSetArg (args[0], XmNuseAsyncGeometry, True);
   XtSetValues (XtParent(shell), args, 1);

   n = 0;
   XtSetArg (args[n], XmNmarginWidth, 1);                               n++;
   XtSetArg (args[n], XmNmarginHeight, 1);                              n++;
   XtSetArg (args[n], XmNshadowThickness, 1);                           n++;
   XtSetArg (args[n], XmNshadowType, XmSHADOW_OUT);                     n++;
   XtSetArg (args[n], XmNautoUnmanage, False);                          n++;
   form = XmCreateForm (shell, "form", args, n);
   XtAddCallback(form, XmNhelpCallback, HelpRequestCB,
                 HELP_FILTER_DIALOG_STR);

   XtAppAddActions( XtWidgetToApplicationContext(form),
                    FilterAction,
                    XtNumber(FilterAction));
   resolvedFilterTranslationString = ResolveTranslationString(
                               FilterTranslationString,
                               (char *)filter_rec );
   filterTranslationTable = XtParseTranslationTable(
                               resolvedFilterTranslationString );
   XtFree( resolvedFilterTranslationString );
    resolvedFilterTranslationString  = NULL;
   XtOverrideTranslations(form, filterTranslationTable);

   hiddenPulldown = XmCreatePulldownMenu(form, "hiddenPD", args, 0);

   label_string = XmStringCreateLocalized ((GETMESSAGE(13,27, "Shown")));
   XtSetArg(args[0], XmNmarginHeight, 1);
   XtSetArg(args[1], XmNmarginWidth, 1);
   XtSetArg(args[2], XmNlabelString, label_string);
   XtSetArg(args[3], XmNuserData, SHOWN);
   filter_rec->hidArray[SHOWN] = 
                XmCreatePushButtonGadget(hiddenPulldown, "shown", args, 4);
   XmStringFree(label_string);
   XtAddCallback(filter_rec->hidArray[SHOWN], XmNactivateCallback, 
                                    filterTypeChanged, (XtPointer) filter_rec);

   label_string = XmStringCreateLocalized ((GETMESSAGE(13,28, "Hidden")));
   XtSetArg(args[2], XmNlabelString, label_string);
   XtSetArg(args[3], XmNuserData, HIDDEN);
   filter_rec->hidArray[HIDDEN] = 
                XmCreatePushButtonGadget(hiddenPulldown, "hidden", args, 4);
   XmStringFree(label_string);
   XtAddCallback(filter_rec->hidArray[HIDDEN], XmNactivateCallback, 
                                    filterTypeChanged, (XtPointer) filter_rec);
   XtManageChildren(filter_rec->hidArray, 2);

   /* create the Option Menu and attach it to the Pulldown MenuPane */
   label_string = XmStringCreateLocalized ((GETMESSAGE(13,31,
                          "Select Datatypes to be:")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);                  n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);              n++;
   XtSetArg (args[n], XmNleftOffset, 5);                              n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);               n++;
   XtSetArg (args[n], XmNtopOffset, 10);                              n++;
   XtSetArg (args[n], XmNsubMenuId, hiddenPulldown);                  n++;
   XtSetArg (args[n], XmNmenuHistory, filter_rec->hidArray[HIDDEN]);  n++;
   hidden = XmCreateOptionMenu(form, "file_name_label", args, n);
   XtManageChild (hidden);
   XmStringFree (label_string);
   XtAddCallback(hidden, XmNhelpCallback, HelpRequestCB,
                 HELP_FILTER_DIALOG_STR);

   /* Create the Scrolled Window For the filetype display area   */
   /* and set the scrollbars colors correctly                    */
   n = 0;
   XtSetArg (args[n], XmNspacing, 2);                                   n++;
   XtSetArg (args[n], XmNscrollingPolicy, XmAUTOMATIC);                 n++;
   XtSetArg (args[n], XmNscrollBarDisplayPolicy, XmAS_NEEDED);          n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);                n++;
   XtSetArg (args[n], XmNleftOffset, 10);                               n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);               n++;
   XtSetArg (args[n], XmNrightOffset, 10);                              n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);               n++;
   XtSetArg (args[n], XmNtopWidget, hidden);                            n++;
   XtSetArg (args[n], XmNtopOffset, 5);                                 n++;
   fileTypeScrollWin = XmCreateScrolledWindow(form, "filetypeSWin", args, n);
   XtManageChild (fileTypeScrollWin);
   XtAddCallback(fileTypeScrollWin, XmNhelpCallback, HelpRequestCB,
                 HELP_FILTER_DIALOG_STR);

   /*  Create a drawing area as a child of the scrolled window  */

   n = 0;
   XtSetArg (args[n], XmNmarginWidth, 4);                               n++;
   XtSetArg (args[n], XmNmarginHeight, 4);                              n++;
   XtSetArg (args[n], XmNresizePolicy, XmRESIZE_ANY);                   n++;
   file_window = XmCreateDrawingArea (fileTypeScrollWin, "file_window",
                                                                     args, n);
   XtManageChild (file_window);

   n = 0;
   XtSetArg (args[n], XmNmarginWidth, 1);                               n++;
   XtSetArg (args[n], XmNmarginHeight, 1);                              n++;
   XtSetArg (args[n], XmNshadowThickness, 0);                           n++;
   XtSetArg (args[n], XmNautoUnmanage, False);                          n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);                n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);               n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);               n++;
   XtSetArg (args[n], XmNtopWidget, fileTypeScrollWin);                 n++;
   XtSetArg (args[n], XmNtopOffset, 10);                                n++;
   sel_form = XmCreateForm (form, "sel_form", args, n);
   XtManageChild(sel_form);

   label_string = XmStringCreateLocalized ((GETMESSAGE(13,21, "Select All")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);                 n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);         n++;
   XtSetArg (args[n], XmNleftPosition, 15);                          n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);              n++;
   XtSetArg (args[n], XmNtopOffset, 5);                              n++;
   XtSetArg (args[n], XmNmarginHeight, 4);                           n++;
   XtSetArg (args[n], XmNmarginWidth, 10);                           n++;
   selectAll = XmCreatePushButtonGadget (sel_form, "selectAll", args, n);
   XtManageChild (selectAll);
   XtAddCallback (selectAll, XmNactivateCallback, SelectAllFileTypes,
                                                 (XtPointer) filter_rec);
   XtAddCallback(selectAll, XmNhelpCallback, HelpRequestCB,
                 HELP_FILTER_DIALOG_STR);
   XmStringFree (label_string);
   XtSetArg (args[0], XmNdefaultButton, selectAll);
   XtSetValues (sel_form, args, 1);

   label_string = XmStringCreateLocalized ((GETMESSAGE(13,22, "Deselect All")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);                    n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);            n++;
   XtSetArg (args[n], XmNleftPosition, 55);                             n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);                 n++;
   XtSetArg (args[n], XmNtopOffset, 5);                                 n++;
   XtSetArg (args[n], XmNmarginHeight, 4);                              n++;
   XtSetArg (args[n], XmNmarginWidth, 10);                              n++;
   unselectAll = XmCreatePushButtonGadget (sel_form, "deselectAll", args, n);
   XtManageChild (unselectAll);
   XtAddCallback (unselectAll, XmNactivateCallback, UnselectAllFileTypes,
                                                 (XtPointer) filter_rec);
   XtAddCallback(unselectAll, XmNhelpCallback, HelpRequestCB,
                 HELP_FILTER_DIALOG_STR);
   XmStringFree (label_string);

   /*  Create the file name filter label and text fields.  */
   label_string = XmStringCreateLocalized (((char *)GETMESSAGE(13,26,
                    "Also hide (Optional): ")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);                    n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);                n++;
   XtSetArg (args[n], XmNleftOffset, 5);                                n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);               n++;
   XtSetArg (args[n], XmNtopWidget, sel_form);                          n++;
   XtSetArg (args[n], XmNtopOffset, 18);                                n++;
   XtSetArg (args[n], XmNtraversalOn, False);                           n++;
   labelPt2 = XmCreateLabelGadget (form, "file_name_label", args, n);
   XtManageChild (labelPt2);
   XmStringFree (label_string);
   XtAddCallback(labelPt2, XmNhelpCallback, HelpRequestCB,
                 HELP_FILTER_DIALOG_STR);


   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET);              n++;
   XtSetArg (args[n], XmNleftWidget, labelPt2);                         n++;
   XtSetArg (args[n], XmNleftOffset, 0);                                n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);               n++;
   XtSetArg (args[n], XmNtopWidget, sel_form);                          n++;
   XtSetArg (args[n], XmNtopOffset, 10);                                n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);               n++;
   XtSetArg (args[n], XmNrightOffset, 10);                              n++;
   fileNameFilter = XmCreateTextField (form, "file_name_text", args, n);
   XtManageChild (fileNameFilter);
   XtAddCallback(fileNameFilter, XmNhelpCallback, HelpRequestCB,
                 HELP_FILTER_DIALOG_STR);

   /*  Create a separator between the buttons  */

   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);                n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);               n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);               n++;
   XtSetArg (args[n], XmNtopWidget, fileNameFilter);                    n++;
   XtSetArg (args[n], XmNtopOffset, 10);                                n++;
   separator =  XmCreateSeparatorGadget (form, "separator", args, n);
   XtManageChild (separator);


   /*  Create the ok, apply, reset, cancel and help buttons  */

   n = 0;
   XtSetArg (args[n], XmNlabelString, okXmString);                      n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);            n++;
   XtSetArg (args[n], XmNleftPosition, 1);                              n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);           n++;
   XtSetArg (args[n], XmNrightPosition, 19);                            n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);               n++;
   XtSetArg (args[n], XmNtopWidget, separator);                         n++;
   XtSetArg (args[n], XmNtopOffset, 5);                                 n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);              n++;
   XtSetArg (args[n], XmNbottomOffset, 5);                              n++;
   XtSetArg (args[n], XmNmarginWidth, 4);                               n++;
   XtSetArg (args[n], XmNmarginHeight, 4);                              n++;
   XtSetArg (args[n], XmNmultiClick, XmMULTICLICK_DISCARD);             n++;
   XtSetArg (args[n], XmNuserData, flag);                        n++;
   ok = XmCreatePushButtonGadget (form, "ok", args, n);
   XtManageChild (ok);
   XtAddCallback(ok, XmNhelpCallback, HelpRequestCB,
                 HELP_FILTER_DIALOG_STR);

   n = 0;
   XtSetArg (args[n], XmNlabelString, applyXmString);                   n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);            n++;
   XtSetArg (args[n], XmNleftPosition, 21);                             n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);           n++;
   XtSetArg (args[n], XmNrightPosition, 39);                            n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);               n++;
   XtSetArg (args[n], XmNtopWidget, separator);                         n++;
   XtSetArg (args[n], XmNtopOffset, 5);                                 n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);              n++;
   XtSetArg (args[n], XmNbottomOffset, 5);                              n++;
   XtSetArg (args[n], XmNmarginWidth, 4);                               n++;
   XtSetArg (args[n], XmNmarginHeight, 4);                              n++;
   apply = XmCreatePushButtonGadget (form, "apply", args, n);
   XtManageChild (apply);
   XtAddCallback(apply, XmNhelpCallback, HelpRequestCB,
                 HELP_FILTER_DIALOG_STR);

   XtSetArg (args[0], XmNdefaultButton, apply);
   XtSetValues (form, args, 1);

   label_string = XmStringCreateLocalized (((char *)GETMESSAGE(13, 11, "Defaults")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);                    n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);            n++;
   XtSetArg (args[n], XmNleftPosition, 41);                             n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);           n++;
   XtSetArg (args[n], XmNrightPosition, 59);                            n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);               n++;
   XtSetArg (args[n], XmNtopWidget, separator);                         n++;
   XtSetArg (args[n], XmNtopOffset, 5);                                 n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);              n++;
   XtSetArg (args[n], XmNbottomOffset, 5);                              n++;
   XtSetArg (args[n], XmNmarginWidth, 4);                               n++;
   XtSetArg (args[n], XmNmarginHeight, 4);                              n++;
   reset = XmCreatePushButtonGadget (form, "reset", args, n);
   XtManageChild (reset);
   XtAddCallback (reset, XmNactivateCallback, DefaultCallback, (XtPointer) filter_rec);
   XtAddCallback(reset, XmNhelpCallback, HelpRequestCB,
                 HELP_FILTER_DIALOG_STR);
   XmStringFree (label_string);


   n = 0;
   XtSetArg (args[n], XmNlabelString, cancelXmString);                  n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);            n++;
   XtSetArg (args[n], XmNleftPosition, 61);                             n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);           n++;
   XtSetArg (args[n], XmNrightPosition, 79);                            n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);               n++;
   XtSetArg (args[n], XmNtopWidget, separator);                         n++;
   XtSetArg (args[n], XmNtopOffset, 5);                                 n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);              n++;
   XtSetArg (args[n], XmNbottomOffset, 5);                              n++;
   XtSetArg (args[n], XmNmarginWidth, 4);                               n++;
   XtSetArg (args[n], XmNmarginHeight, 4);                              n++;
   XtSetArg (args[n], XmNmultiClick, XmMULTICLICK_DISCARD);             n++;
   XtSetArg (args[n], XmNuserData, flag );                              n++;
   close = XmCreatePushButtonGadget (form, "close", args, n);
   XtAddCallback (close, XmNactivateCallback, CancelCallback, (XtPointer) filter_rec);
   XtManageChild (close);
   XtAddCallback(close, XmNhelpCallback, HelpRequestCB,
                 HELP_FILTER_DIALOG_STR);


   n = 0;
   XtSetArg (args[n], XmNlabelString, helpXmString);                    n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);            n++;
   XtSetArg (args[n], XmNleftPosition, 81);                             n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);           n++;
   XtSetArg (args[n], XmNrightPosition, 99);                            n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);               n++;
   XtSetArg (args[n], XmNtopWidget, separator);                         n++;
   XtSetArg (args[n], XmNtopOffset, 5);                                 n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);              n++;
   XtSetArg (args[n], XmNbottomOffset, 5);                              n++;
   XtSetArg (args[n], XmNmarginWidth, 4);                               n++;
   XtSetArg (args[n], XmNmarginHeight, 4);                              n++;
   help = XmCreatePushButtonGadget (form, "help", args, n);
   XtManageChild (help);
   XtAddCallback(help, XmNactivateCallback, HelpRequestCB,
                 HELP_FILTER_DIALOG_STR);
   XtAddCallback(help, XmNhelpCallback, HelpRequestCB,
                 HELP_FILTER_DIALOG_STR);


   XtSetArg (args[0], XmNcancelButton, close);
   XtSetValues (form, args, 1);

   /* Fill in our instance structure */

   filter_rec->shell = shell;
   filter_rec->form = form;
   filter_rec->fileNameFilter = fileNameFilter;
   filter_rec->hidden = hidden;
   filter_rec->scrolled_window = fileTypeScrollWin;
   filter_rec->file_window = file_window;
   filter_rec->select_all = selectAll;
   filter_rec->unselect_all = unselectAll;
   filter_rec->label = labelPt2;
   filter_rec->cancel = 0x0;
   filter_rec->previous_filter_data = NULL;

   filter_rec->ok = ok;
   filter_rec->apply = apply;
   filter_rec->reset = reset;
   filter_rec->close = close;
   filter_rec->help = help;

   /*  Set the return values for the dialog widget and dialog instance.  */

   *return_widget = form;
   *dialog = (XtPointer) filter_rec;
}




/************************************************************************
 *
 *  InstallChange
 *
 ************************************************************************/

static void
InstallChange(
        FilterRec *filter_rec,
        XtCallbackProc callback,
        XtPointer client_data )
{
   FilterApply * apply_data;


   /*  Setup the callback data to be sent to the Ok callback.   */
   /*  This contains the encapsulation callback to invoke upon  */
   /*  the data within the dialog being changed.                */

   apply_data = (FilterApply *) XtMalloc (sizeof (FilterApply));
   apply_data->callback = callback;
   apply_data->client_data = client_data;
   apply_data->filter_rec = (XtPointer) filter_rec;
   filter_rec->apply_data = apply_data;


   XtAddCallback (filter_rec->apply, XmNactivateCallback,
                  ApplyCallback, (XtPointer) apply_data);

   XtAddCallback (filter_rec->ok, XmNactivateCallback,
                  OkCallback, (XtPointer) apply_data);
}




/************************************************************************
 *
 *  InstallClose
 *
 ************************************************************************/

static void
InstallClose(
        FilterRec *filter_rec,
        XtCallbackProc callback,
        XtPointer client_data )
{
   Atom delete_window_atom;

   XtAddCallback(filter_rec->close, XmNactivateCallback, callback, client_data);

   delete_window_atom = XmInternAtom (XtDisplay(filter_rec->shell), "WM_DELETE_WINDOW", True);
   XmRemoveWMProtocols( filter_rec->shell, &delete_window_atom, 1 );
   XmAddWMProtocolCallback( filter_rec->shell, delete_window_atom, callback, 
                            (XtPointer) client_data );
}




/************************************************************************
 *
 *  Destroy
 *
 ************************************************************************/

static void
Destroy(
        FilterRec *filter_rec )
{
   XtDestroyWidget (filter_rec->shell);
   XtFree ((char *) filter_rec->apply_data);
   XtFree ((char *) filter_rec);
}




/************************************************************************
 *
 *  GetValues
 *
 ************************************************************************/

static XtPointer
GetValues(
        FilterRec *filter_rec )
{
   FilterData * filter_data;

  if( filter_rec->cancel
      && filter_rec->previous_filter_data )
  {
    
    filter_data = filter_rec->previous_filter_data;
    filter_rec->previous_filter_data = NULL;
  }
  else
  {
    Arg args[4];

    /*  Allocate and initialize the file filter dialog data.  */

    filter_data = (FilterData *) XtMalloc (sizeof (FilterData));

    filter_data->displayed = True;

    XtSetArg (args[0], XmNx, &filter_data->x);
    XtSetArg (args[1], XmNy, &filter_data->y);
    XtSetArg (args[2], XmNwidth, &filter_data->width);
    XtSetArg (args[3], XmNheight, &filter_data->height);
    XtGetValues (filter_rec->shell, args, 4);

    (void) GetFilterValues (filter_rec, filter_data);
  }

  filter_rec->cancel = 0x0;

  return ((XtPointer) filter_data);
}




/************************************************************************
 *
 *  GetDefaultValues
 *
 ************************************************************************/

static XtPointer
GetDefaultValues( void )
{
   FilterData * filter_data;

   /*  Allocate and initialize the default file filter dialog data.  */

   filter_data = (FilterData *) XtMalloc (sizeof (FilterData));

   filter_data->displayed = False;
   filter_data->x = 0;
   filter_data->y = 0;
   filter_data->height = 0;
   filter_data->width = 0;
   filter_data->match_flag = True;
   filter_data->filter = XtNewString(".*");
   filter_data->show_hidden = False;
   filter_data->filetypesFilteredCount = 3;
   filter_data->filetypesFiltered = XtNewString(DEFAULT_FILTER);
   filter_data->user_data = NULL;
   filter_data->count = ReadInFiletypes(filter_data);

   return ((XtPointer) filter_data);
}




/************************************************************************
 *
 *  GetResourceValues
 *
 ************************************************************************/

static XtPointer
GetResourceValues(
        XrmDatabase data_base,
        char **name_list )
{
   FilterData * filter_data;
   static Boolean convertersAdded = False;


   /*  Allocate and get the resources for file filter dialog data.  */

   filter_data = (FilterData *) XtMalloc (sizeof (FilterData));
   filter_data->user_data = NULL;

   _DtDialogGetResources (data_base, name_list, FILE_FILTER, (char *)filter_data,
                       resources, filterClass->resource_count);


   /* Create new strings for any string value we read in from our
    * resourceDB.
    */

   filter_data->filter = XtNewString(filter_data->filter);
   filter_data->filetypesFiltered = XtNewString(filter_data->filetypesFiltered);

   filter_data->count = ReadInFiletypes(filter_data);

   return ((XtPointer) filter_data);
}


static FilterData *
CopyFilterData(
        FilterData *filter_data )
{
  FilterData * CopiedFilterData;
  int i;

  CopiedFilterData = (FilterData *) XtMalloc (sizeof (FilterData));
  CopiedFilterData->displayed = filter_data->displayed;
  CopiedFilterData->show_hidden = filter_data->show_hidden;
  CopiedFilterData->match_flag = filter_data->match_flag;
  CopiedFilterData->filter = XtNewString( filter_data->filter );
  CopiedFilterData->filetypesFilteredCount = filter_data->filetypesFilteredCount;
  CopiedFilterData->filetypesFiltered = XtNewString( filter_data->filetypesFiltered );
  CopiedFilterData->count = filter_data->count;
  CopiedFilterData->user_data = (FTData **) XtMalloc(sizeof(FTData *) *
                                                       filter_data->count);
  for( i = 0; i < filter_data->count; ++i )
  {
    CopiedFilterData->user_data[i] = (FTData *)XtMalloc(sizeof(FTData));
    CopiedFilterData->user_data[i]->filetype =
      XtNewString( filter_data->user_data[i]->filetype );
    CopiedFilterData->user_data[i]->index =
      filter_data->user_data[i]->index;
    CopiedFilterData->user_data[i]->selected =
      filter_data->user_data[i]->selected;
  }
  return CopiedFilterData;
}

/************************************************************************
 *
 *  SetValues
 *
 ************************************************************************/

static void
SetValues(
        FilterRec *filter_rec,
        FilterData *filter_data )
{
   Arg args[20];
   int n, i, count;
   XmManagerWidget mgr = (XmManagerWidget)filter_rec->file_window;
   int num_children = mgr->composite.num_children;
   Pixel  background_color;
   Pixel  foreground_color;
   Pixel  pixmap_background;
   Pixel  pixmap_foreground;

   XmString icon_label;
   Widget icon_widget;
   Position x;
   Dimension max = 0;
   XtWidgetGeometry  desired, preferred;
   FTData *user_data;
   PixmapData *pixmapData;

   filter_rec->cancel = 0x0;
   if( filter_rec->previous_filter_data )
     FreeValues( filter_rec->previous_filter_data );
   filter_rec->previous_filter_data = CopyFilterData( filter_data );

   XmTextFieldSetString (filter_rec->fileNameFilter, filter_data->filter);

   if(XmToggleButtonGadgetGetState (*showHiddenMenu))
      filter_data->show_hidden = True;
   else
      filter_data->show_hidden = False;

   if(filter_data->match_flag)
      XtSetArg (args[0], XmNmenuHistory, filter_rec->hidArray[HIDDEN]);
   else
      XtSetArg (args[0], XmNmenuHistory, filter_rec->hidArray[SHOWN]);
   XtSetValues(filter_rec->hidden, args, 1);

   XtSetArg (args[0], XmNbackground, &background_color);
   XtSetArg (args[1], XmNforeground, &foreground_color);
   XtSetArg (args[2], XmNtopShadowColor, &pixmap_background);
   XtSetArg (args[3], XmNbottomShadowColor, &pixmap_foreground);
   XtGetValues((Widget)mgr, args, 4);

   for (i = 0, x = 5, count = 0; i < filter_data->count; i++)
   {
      char *ptr;

      user_data = filter_data->user_data[i];

      if (ptr = (char *)DtDtsDataTypeToAttributeValue(user_data->filetype,
                                                      FM_TYPE_LABEL,
                                                      NULL))
      {
         icon_label = XmStringCreateLocalized(ptr);
         DtDtsFreeAttributeValue(ptr);
      }
      else
      {
         icon_label = XmStringCreateLocalized(user_data->filetype);
      }

      pixmapData = _DtRetrievePixmapData(user_data->filetype,
                                         NULL,
                                         NULL,
                                         filter_rec->file_window,
                                         LARGE);

      n = 0;
      XtSetArg (args[n], XmNstring, icon_label);                        n++;
      if (pixmapData)
        XtSetArg (args[n], XmNimageName, pixmapData->iconFileName);
      else
        XtSetArg (args[n], XmNimageName, NULL);                         n++;
      XtSetArg (args[n], XmNunderline, False);                          n++;

      XtSetArg (args[n], XmNpixmapPosition, XmPIXMAP_TOP);              n++;
      XtSetArg (args[n], XmNbehavior, XmICON_BUTTON);                   n++;
      XtSetArg (args[n], XmNfillMode, XmFILL_PARENT);              n++;

      XtSetArg (args[n], XmNforeground, foreground_color);           n++;
      XtSetArg (args[n], XmNarmColor, white_pixel);                  n++;
      if (background_color == white_pixel)
      {
         XtSetArg (args[n], XmNbackground, white_pixel);             n++;
         XtSetArg (args[n], XmNpixmapBackground, white_pixel);       n++;
         XtSetArg (args[n], XmNpixmapForeground, black_pixel);       n++;
      }
      else if (background_color == black_pixel)
      {
         XtSetArg (args[n], XmNbackground, black_pixel);             n++;
         XtSetArg (args[n], XmNpixmapBackground, white_pixel);       n++;
         XtSetArg (args[n], XmNpixmapForeground, black_pixel);       n++;
      }
      else
      {
         XtSetArg (args[n], XmNbackground, background_color);        n++;
         XtSetArg (args[n], XmNpixmapBackground, pixmap_background); n++;
         XtSetArg (args[n], XmNpixmapForeground, pixmap_foreground); n++;
      }

      if (i - count < num_children)
      {
         icon_widget = mgr->composite.children[i - count];
         XtSetArg (args[n], XmNx, x);                                   n++;
         XtSetArg (args[n], XmNy, 5);                                   n++;
         XtSetArg (args[n], XmNuserData, user_data);                    n++;
         XtSetValues(icon_widget, args, n);
         XtRemoveAllCallbacks (icon_widget, XmNcallback);

      }
      else
      {
         XtSetArg (args[n], XmNx, x);                                   n++;
         XtSetArg (args[n], XmNy, 5);                                   n++;
         XtSetArg (args[n], XmNshadowThickness, 2);                     n++;
         XtSetArg (args[n], XmNfontList, user_font);                    n++;
         if(keybdFocusPolicy == XmEXPLICIT)
         {
            XtSetArg (args[n], XmNtraversalOn, True);                   n++;
         }
         else
         {
            XtSetArg (args[n], XmNtraversalOn, False);                  n++;
            XtSetArg (args[n], XmNhighlightThickness, 0);               n++;
         }
         XtSetArg (args[n], XmNborderType, DtNON_RECTANGLE);           n++;
         if (mgr->composite.num_children == 0)
         {
            XtSetArg (args[n], XmNuserData, user_data);                 n++;
            icon_widget= _DtCreateIcon ((Widget)mgr, "icon", args, n);
         }
         else
         {
            icon_widget = _DtDuplicateIcon((Widget)mgr,
                          mgr->composite.children[0],
                          (XmString)args[0].value, (String)args[1].value,
                          (XtPointer)user_data, (Boolean)args[2].value);
            n = 0;
            XtSetArg (args[n], XmNx, x);                                n++;
            XtSetArg (args[n], XmNy, 5);                                n++;
            XtSetValues(icon_widget, args, n);
         }
         XtAddCallback(icon_widget, XmNhelpCallback,
                       (XtCallbackProc)HelpRequestCB, NULL);
      }
      XtAddCallback (icon_widget, XmNcallback, NewFileTypeSelected,
                                                    (XtPointer)filter_rec);

      _DtCheckAndFreePixmapData(user_data->filetype,
                                filter_rec->file_window,
                                (DtIconGadget) icon_widget,
                                pixmapData);

      XtManageChild(icon_widget);
      x += 5 + icon_widget->core.width;
      if (icon_widget->core.height > max)
         max = icon_widget->core.height;

      XmStringFree(icon_label);
   }

   for (i = 0, count = 0; i < filter_data->count; i++)
   {

      user_data = filter_data->user_data[i];

      icon_widget = mgr->composite.children[i - count];

      if(filter_data->user_data[i]->selected == True)
      {
         n = 0;
         /* Draw the selected icon as selected */
         if (background_color == white_pixel)
         {
            XtSetArg (args[n], XmNbackground, black_pixel);             n++;
            XtSetArg (args[n], XmNforeground, white_pixel);             n++;
         }
         else
         {
            XtSetArg (args[n], XmNbackground, white_pixel);             n++;
            XtSetArg (args[n], XmNforeground, black_pixel);             n++;
         }

         XtSetArg (args[n], XmNpixmapBackground, white_pixel);          n++;
         XtSetArg (args[n], XmNpixmapForeground, black_pixel);          n++;
         XtSetArg (args[n], XmNarmColor, white_pixel);                  n++;
         XtSetValues(icon_widget, args, n);
      }
   }

   i -= count;
   /* Unmanage any icons left over */
   while (i < num_children)
   {
      XtUnmanageChild(mgr->composite.children[i]);
      i++;
   }

   /* Line up the titles in all of the icons */
   num_children = mgr->composite.num_children;
   for (i = 0; i < num_children; i++)
   {
      if (XtIsManaged(mgr->composite.children[i]))
      {
         if (mgr->composite.children[i]->core.height != max)
         {
            n = 0;
            XtSetArg(args[n], XmNy,
                     5 + (max - mgr->composite.children[i]->core.height)); n++;
            XtSetValues(mgr->composite.children[i], args, n);
         }
      }
   }

   /* Set the height of the scrolled window */
   desired.x = filter_rec->scrolled_window->core.x;
   desired.y = filter_rec->scrolled_window->core.y;
   desired.width = filter_rec->scrolled_window->core.width;
   desired.request_mode = (CWWidth);
   XtQueryGeometry(filter_rec->scrolled_window, &desired, &preferred);
   n = 0;
   if (preferred.request_mode & CWWidth)
   {
       XtSetArg(args[n], XmNwidth, (XtArgVal) preferred.width);
       n++;
   }
   if (preferred.request_mode & CWHeight)
   {
       XtSetArg(args[n], XmNheight, (XtArgVal) preferred.height);
       n++;
   }
   XtSetValues(filter_rec->scrolled_window, args, n);

}




/************************************************************************
 *
 *  WriteResourceValues
 *
 ************************************************************************/

static void
WriteResourceValues(
        DialogData *values,
        int fd,
        char **name_list )
{
   FilterData * filter_data = (FilterData *) values->data;
   FilterRec  * filter_rec;
   Arg args[2];


   /*  If the dialog is currently displayed, update the geometry  */
   /*  fields to their current values.                            */

   if (filter_data->displayed == True)
   {
      _DtGenericUpdateWindowPosition(values);
      filter_rec = (FilterRec *) _DtGetDialogInstance (values);
   }

   _DtDialogPutResources (fd, name_list, FILE_FILTER, values->data,
                       resources, filterClass->resource_count);
}




/************************************************************************
 *
 *  FreeValues
 *
 ************************************************************************/

static void
FreeValues(
        FilterData *filter_data )
{
   int i;

   for( i = 0; i < filter_data->count; i++)
   {
      if(filter_data->user_data[i]->filetype)
        XtFree ((char *)filter_data->user_data[i]->filetype);
      XtFree ((char *)filter_data->user_data[i]);
   }
   XtFree ((char *)filter_data->user_data);
   XtFree ((char *) filter_data->filter);
   XtFree ((char *) filter_data->filetypesFiltered);
   XtFree ((char *) filter_data);
}



/************************************************************************
 *
 *  DefaultCallback
 *
 ************************************************************************/

static void
DefaultCallback(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
   FilterRec * filter_rec = (FilterRec *) client_data;
   DialogData * dialog_data;
   FilterData * filter_data;
   int i;
   Arg args[1];


   /*  Get the instance and data for the dialog  */

   dialog_data = _DtGetInstanceData ((XtPointer)filter_rec);
   filter_data = (FilterData *) dialog_data->data;

   filter_data->show_hidden = False;

   XtSetArg (args[0], XmNmenuHistory, filter_rec->hidArray[HIDDEN]);
   XtSetValues(filter_rec->hidden, args, 1);
   filter_data->match_flag = True;

   XmToggleButtonGadgetSetState(*showHiddenMenu, False, False);

   XtFree( filter_data->filter );
   filter_data->filter = XtNewString(".*");

   filter_data->filetypesFilteredCount = 3;
   XtFree( filter_data->filetypesFiltered );
   filter_data->filetypesFiltered = XtNewString( DEFAULT_FILTER );

   filter_data->count = ReadInFiletypes(filter_data);

   /* Display the new type  */
   SetValues (filter_rec, filter_data);
}

/************************************************************************
 *
 *  CancelCallback
 *
 ************************************************************************/

static void
CancelCallback(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
  FilterRec * filter_rec = (FilterRec *) client_data;

  filter_rec->cancel = 0x1;
}

/************************************************************************
 *
 *  OkCallback
 *      This is a callback function called when the (Ok) button
 *      is pressed.  It saves the new file filter data internally and
 *      then calls the encapsulations callback function. It then closes
 *      the dialog.
 *
 ************************************************************************/

static void
OkCallback(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
   FilterRec  * filter_rec = (FilterRec *) ((FilterApply *)client_data)->filter_rec;
   DialogData * dialog_data;
   Widget close = filter_rec->close;

   if(RecheckFlag(NULL,w))
     return;
   else if(RecheckFlag(NULL,close))
     return;

   ApplyCallback(w, client_data, call_data);
   dialog_data = _DtGetInstanceData ((XtPointer)filter_rec);
   ResetFlag(NULL,close);
   XtCallCallbacks(filter_rec->close, XmNactivateCallback,
                                                 (XtPointer)dialog_data);
}


/************************************************************************
 *
 *  ApplyCallback
 *      This is a callback function called when the (Apply) button
 *      is pressed.  It saves the new file filter data internally and
 *      then calls the encapsulations callback function.
 *
 ************************************************************************/

static void
ApplyCallback(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
   FilterRec  * filter_rec;
   DialogData * dialog_data;
   FilterData * filter_data;
   FilterData * new_filter_data;
   XmManagerWidget mgr;
   int num_children;
   int i, count;
   Arg args[2];


   filter_rec = (FilterRec *) ((FilterApply *) client_data)->filter_rec;
   dialog_data = _DtGetInstanceData ((XtPointer)filter_rec);
   filter_data = (FilterData *) dialog_data->data;
   new_filter_data = (FilterData *) XtMalloc (sizeof (FilterData));
   mgr = (XmManagerWidget)filter_rec->file_window;
   num_children = mgr->composite.num_children;

   GetFilterValues (filter_rec, new_filter_data);
   FreeValues (filter_data);
   dialog_data->data = (XtPointer) new_filter_data;

   if( filter_rec->previous_filter_data )
     FreeValues( filter_rec->previous_filter_data );
   filter_rec->previous_filter_data = CopyFilterData( new_filter_data );

   /* Need to update 'userData' for each of the icons */
   for (i = 0, count = 0; i < num_children &&
                          i + count < new_filter_data->count; i++)
   {
      XtSetArg (args[0], XmNuserData, new_filter_data->user_data[i + count]);
      XtSetValues(mgr->composite.children[i], args, 1);
   }
   (*(((FilterApply *)client_data)->callback)) (w, ((FilterApply *)client_data)->client_data, NULL);
}



/************************************************************************
 *
 *  GetFilterValues
 *      Update the current file filter values within the data structure
 *      from the current toggle button values.
 *
 ************************************************************************/

static Boolean
GetFilterValues(
        FilterRec *filter_rec,
        FilterData *filter_data )
{
   int i;
   Widget menuHistory;
   Arg args[2];
   DialogData * dialog_data;
   FilterData * old_filter_data;

   filter_data->displayed = True;

   filter_data->filter = XmTextFieldGetString (filter_rec->fileNameFilter);

   XmToggleButtonGadgetSetState(*showHiddenMenu, False, False);
   filter_data->show_hidden = False;

   XtSetArg (args[0], XmNmenuHistory, &menuHistory);
   XtGetValues (filter_rec->hidden, args, 1);

   if(menuHistory == filter_rec->hidArray[HIDDEN])
      filter_data->match_flag = True;
   else
      filter_data->match_flag = False;

   filter_data->filetypesFiltered = NULL;
   filter_data->filetypesFilteredCount = 0;
   dialog_data = _DtGetInstanceData ((XtPointer)filter_rec);
   if(dialog_data != NULL)
   {
      old_filter_data = (FilterData *) dialog_data->data;
      filter_data->count = old_filter_data->count;
      filter_data->user_data = (FTData **) XtMalloc(sizeof(FTData *) *
                                                       filter_data->count);

      for( i = 0; i < filter_data->count; i++)
      {
         /* Copy current value from the old dialog values */
         filter_data->user_data[i] = (FTData *)XtMalloc(sizeof(FTData));
         filter_data->user_data[i]->filetype =
              XtNewString(old_filter_data->user_data[i]->filetype);
         filter_data->user_data[i]->index =
              old_filter_data->user_data[i]->index;
         filter_data->user_data[i]->selected =
              old_filter_data->user_data[i]->selected;

         if((filter_data->user_data[i]->selected == True && 
                                                filter_data->match_flag) ||
            (filter_data->user_data[i]->selected == False && 
                                                !filter_data->match_flag))
         {
           if(filter_data->filetypesFiltered == NULL)
           {
              filter_data->filetypesFiltered =
                XtNewString(filter_data->user_data[i]->filetype);
           }
           else
           {
              filter_data->filetypesFiltered = (char *)XtRealloc(
                               (char *)filter_data->filetypesFiltered,
                               strlen(filter_data->filetypesFiltered) +
                               strlen(filter_data->user_data[i]->filetype) + 2);

              strcat(filter_data->filetypesFiltered, ",");
              strcat(filter_data->filetypesFiltered,
                     filter_data->user_data[i]->filetype);
           }
           filter_data->filetypesFilteredCount++;
        }
      }

      return (True);
    }
    return(False);
}


void
ShowHiddenFiles (
   Widget  w,
   XtPointer client_data,
   XtPointer callback)

{
   FilterData *filter_data;
   FileMgrData *file_mgr_data;
   FileMgrRec *file_mgr_rec;
   FilterRec *filter_rec;
   DialogData *dialog_data;
   Widget mbar;
   Arg args[1];

   if ((int)client_data == FM_POPUP)
     mbar = XtParent(w);
   else
     mbar = (Widget)XmGetPostedFromWidget(XtParent(w));
   XmUpdateDisplay (w);

   XtSetArg(args[0], XmNuserData, &file_mgr_rec);
   XtGetValues(mbar, args, 1);

   dialog_data = _DtGetInstanceData ((XtPointer)file_mgr_rec);
   file_mgr_data = (FileMgrData *) dialog_data->data;

   filter_data = (FilterData *)(file_mgr_data->filter_active->data);
   if (file_mgr_data->show_hid_enabled)
   {
      filter_data->show_hidden = False;
      file_mgr_data->show_hid_enabled = False;
   }
   else
   {
      filter_data->show_hidden = True;
      file_mgr_data->show_hid_enabled = True;
   }

   filter_rec = (FilterRec *)_DtGetDialogInstance( file_mgr_data->filter_edit );

   /* Refilter the contents of the current directory */

   FileMgrRedisplayFiles (file_mgr_rec, file_mgr_data, False);

}


int
ReadInFiletypes(
     FilterData *filter_data)
{
   char **file_types;
   int i;
   int count = 0;
   int count_size = 0;
   FTData *user_data;
   char *filtered_types, *file_type;
   int j;

   if (filter_data->user_data)
   {
      for(i=0; i<filter_data->count; i++)
      {
         XtFree(filter_data->user_data[i]->filetype);
         XtFree((char *)filter_data->user_data[i]);
      }
      XtFree((char *)filter_data->user_data);
      filter_data->user_data = NULL;
      filter_data->count = 0;
   }

   file_types = (char **) DtDtsDataTypeNames();
   i = 0;
   while(file_types[i])
   {
      /* filter out invisible types and actions */
      if((_DtCheckForDataTypeProperty(file_types[i], "invisible")) ||
         (DtDtsDataTypeIsAction(file_types[i])))
      {
         i++;
         continue;
      }

      if (count == count_size)
      {
         count_size += 10;

         filter_data->user_data =
            (FTData **) XtRealloc ((char *)filter_data->user_data,
                                        sizeof (FTData *) * count_size);
      }

      user_data = (FTData *)XtMalloc(sizeof(FTData));
      user_data->index = count;
      user_data->selected = False;
      user_data->filetype = XtNewString(file_types[i]);

      filter_data->user_data[count] = user_data;
      count++;
      i++;
   }

   DtDtsFreeDataTypeNames(file_types);

   qsort( filter_data->user_data, count, sizeof( FTData *), (int (*)())FileTypeAscending );

   filtered_types = filter_data->filetypesFiltered;
   for(i = 0; i < filter_data->filetypesFilteredCount; i++)
   {
      file_type = filtered_types;
      filtered_types = DtStrchr(filtered_types, ',');

      if(filter_data->filetypesFilteredCount - 1 != i)
         *filtered_types = '\0';

      for(j = 0; j < count; j++)
      {
          if(strcmp(file_type, filter_data->user_data[j]->filetype) == 0) 
          {
             filter_data->user_data[j]->selected = True;
             break;
          }
      }

      if(filter_data->filetypesFilteredCount - 1 != i)
      {
         *filtered_types = ',';
         filtered_types++;
      }
   }
   if(!filter_data->match_flag)
   {
      for(j = 0; j < count; j++)
      {
          if(filter_data->user_data[j]->selected == True)
             filter_data->user_data[j]->selected = False;
          else
             filter_data->user_data[j]->selected = True;
      }
   }

   return(count);
}


/*
 * Whenever the database files are reread, we must update the filter
 * values, since the indices of the filetypes may have changed.
 */

void
UpdateFilterAfterDBReread (
   DialogData * dialog_data )

{
   int i, count;
   int num_children;
   XmManagerWidget mgr;
   Arg args[2];
   FilterData * new_filter_data;
   FilterData * filter_data;
   FilterRec * filter_rec;
   Boolean result;

   filter_data = (FilterData *) dialog_data->data;
   new_filter_data = (FilterData *) XtMalloc (sizeof (FilterData));

   if (filter_rec = (FilterRec *) _DtGetDialogInstance(dialog_data))
   {
      /* Dialog is posted */
      result = GetFilterValues (filter_rec, new_filter_data);
      if( result )
      {
         XtFree(new_filter_data->filetypesFiltered);
         new_filter_data->filetypesFiltered =
                  XtNewString(filter_data->filetypesFiltered);
         new_filter_data->filetypesFilteredCount =
                  filter_data->filetypesFilteredCount;
         FreeValues (filter_data);
         dialog_data->data = (XtPointer) new_filter_data;
         filter_data = new_filter_data;
      }
   }

   /* Reload the filetype information */
   filter_data->count = ReadInFiletypes(filter_data);

   if (filter_rec && result)
   {
      /* Need to update 'userData' for each of the icons */
      mgr = (XmManagerWidget)filter_rec->file_window;
      num_children = mgr->composite.num_children;
      for (i = 0, count = 0; i < num_children &&
                             i + count < filter_data->count; i++)
      {
         XtSetArg (args[0], XmNuserData, new_filter_data->user_data[i]);
         XtSetValues(mgr->composite.children[i], args, 1);
      }
      /* Display the new type  */
      SetValues (filter_rec, filter_data);
   }
}

static void
filterTypeChanged(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
   Widget menuHistory;
   FilterRec * filter_rec = (FilterRec *) client_data;
   DialogData * dialog_data;
   FilterData * filter_data;
   Arg args[2];
   XmString label_string;

   /*  Get the instance and data for the dialog  */
   dialog_data = _DtGetInstanceData ((XtPointer)filter_rec);
   filter_data = (FilterData *) dialog_data->data;

   XtSetArg (args[0], XmNmenuHistory, &menuHistory);
   XtGetValues (filter_rec->hidden, args, 1);

   if(menuHistory == filter_rec->hidArray[HIDDEN])
   {
      filter_data->match_flag = True;
      label_string = XmStringCreateLocalized (((char *)GETMESSAGE(13,26,
                                         "Also hide (Optional): ")));
   }
   else
   {
      filter_data->match_flag = False;
      label_string = XmStringCreateLocalized (((char *)GETMESSAGE(13,29,
                                         "Also show (Optional): ")));
   }
   XtSetArg (args[0], XmNlabelString, label_string);
   XtSetValues( filter_rec->label, args, 1 );

   XmStringFree( label_string );
   XmUpdateDisplay( filter_rec->label );
}

static int
FileTypeAscending(
        FTData **t1,
        FTData **t2 )
{
  return strcmp( (*t1)->filetype, (*t2)->filetype );
}

static void
SelectAllFilter(
        Widget w,
        XEvent *event,
        String *params,
        Cardinal *num_params )
{
  FilterRec * filterRec;

  sscanf( params[0], "%p", &filterRec );
  SelectAllFileTypes( w, (XtPointer)filterRec, NULL );
  return ;
}

static void
DeselectAllFilter(
        Widget w,
        XEvent *event,
        String *params,
        Cardinal *num_params )
{
  FilterRec * filterRec;

  sscanf( params[0], "%p", &filterRec );
  UnselectAllFileTypes( w, (XtPointer)filterRec, NULL );
  return ;
}
