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
/* $TOG: Find.c /main/10 1999/12/09 13:06:33 mgreess $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 *   FILE:           Find.c
 *
 *   COMPONENT_NAME: Desktop File Manager (dtfile)
 *
 *   Description:    Source file for the find file dialog.
 *
 *   FUNCTIONS: AlternateInputHandler
 *		AlternateInputHandler
 *		AlternateInputHandler
 *		Create
 *		Destroy
 *		EnterStopBttn
 *		ExecuteFind
 *		ExecuteGrep
 *		ExtractDirectory
 *		FindProcessStarted
 *		FindPutOnDesktop
 *		FreeMatchInfo
 *		FreeValues
 *		GetDefaultValues
 *		GetFileName
 *		GetFindValues
 *		GetResourceValues
 *		GetValues
 *		GrowBuffer
 *		InstallChange
 *		InstallClose
 *		InvalidFindMessage
 *		LeaveStopBttn
 *		MakeAbsolute
 *		NewView
 *		SetActiveItem
 *		SetFocus
 *		SetValues
 *		StartCallback
 *		StartSearch
 *		StopCallback
 *		StopSearch
 *		WriteResourceValues
 *		findpopen
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *
 ****************************************************************************
 ************************************<+>*************************************/


#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <signal.h>


#ifdef __hpux
#include <sys/getaccess.h>
#endif /* __hpux */
#include <stdlib.h>

#include <string.h>

#include <Xm/XmP.h>
#include <Xm/DialogS.h>
#include <Xm/Form.h>
#include <Xm/LabelG.h>
#include <Xm/List.h>
#include <Xm/Frame.h>
#include <Xm/MessageB.h>
#include <Xm/PushBG.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/ScrolledW.h>
#include <Xm/TextF.h>
#include <Xm/ToggleBG.h>
#include <Xm/SeparatoG.h>
#include <Xm/VendorSEP.h>
#include <Xm/MwmUtil.h>
#include <Xm/Protocols.h>

#include <Dt/TitleBox.h>

#include <X11/ShellP.h>
#include <X11/Xatom.h>

#include <Dt/Connect.h>
#include <Dt/DtNlUtils.h>
#include <Dt/HourGlass.h>

#include <Tt/tttk.h>

#include "Encaps.h"
#include "SharedProcs.h"
#include "FileMgr.h"
#include "Desktop.h"
#include "Main.h"
#include "Common.h"
#include "Find.h"
#include "Help.h"
#include "SharedMsgs.h"


typedef struct _Dummy {
   Boolean       displayed;
   Position      x;
   Position      y;
   Dimension     width;
   Dimension     height;

   String string;
} Dummy, *DummyPtr;


/* Error message defines */

#define BAD_DIR_NAME               0
#define NO_DIR_ACCESS              1
#define NO_EXISTANCE               2
#define NO_FILE_OR_FOLDER_ARG      3


/* More string defines */
static char * PRINT_OPTION = " -print ";
static char * FIND_COMMAND = "find ";
static char * GREP_COMMAND = "grep -i -l ";
static char * NAME_OPTION =  " -name ";
#if defined(__hpux) || defined(sun)
static char * FOLLOW_OPTION = " -follow";
#endif /* __hpux */
static char * REDIRECTOR =   " 2>&-";
static char * TYPEDIR = " -type d";
static char * FIND_FILE = "FindFile";

#define NEW_VIEW     0
#define CURRENT_VIEW 1



/*  Resource definitions for the find file dialog  */

static DialogResource resources[] =
{
   { "folders", XmRString, sizeof(String),
     XtOffset(FindDataPtr, directories),
     (XtPointer) NULL, _DtStringToString },

   { "name", XmRString, sizeof(String),
     XtOffset(FindDataPtr, filter),
     (XtPointer) NULL, _DtStringToString },

   { "selectedItem", XmRInt, sizeof(int),
     XtOffset(FindDataPtr, selected_item),
     (XtPointer) -1, _DtIntToString },

   { "content", XmRString, sizeof(String),
     XtOffset(FindDataPtr, content),
     (XtPointer) NULL, _DtStringToString },
};


static DialogResource match_resources[] =
{
   { "matchData", XmRString, sizeof(String),
     XtOffset(DummyPtr, string),
     (XtPointer) NULL, _DtStringToString },
};

/********    Static Function Declarations    ********/

static void Create(
                        Display *display,
                        Widget parent,
                        Widget *return_widget,
                        XtPointer *dialog) ;
static void InstallChange(
                        FindRec *find_rec,
                        XtCallbackProc callback,
                        XtPointer client_data) ;
static void InstallClose(
                        FindRec *find_rec,
                        XtCallbackProc callback,
                        XtPointer client_data) ;
static void Destroy(
                        FindRec *find_rec) ;
static XtPointer GetValues(
                        FindRec *find_rec) ;
static XtPointer GetDefaultValues( void ) ;
static XtPointer GetResourceValues(
                        XrmDatabase data_base,
                        char **name_list) ;
static void SetValues(
                        FindRec *find_rec,
                        FindData *find_data) ;
static void WriteResourceValues(
                        DialogData *values,
                        int fd,
                        char **name_list) ;
static void FreeValues(
                        FindData *find_data) ;
static Boolean GetFindValues(
                        FindRec *find_rec,
                        FindData *find_data,
                        Boolean validate) ;
static void InvalidFindMessage(
                        FindRec *find_rec,
                        int messageIndex,
                        String extra_string) ;
static int FreeMatchInfo(
                        String *matches,
                        int numMatches) ;
static void LeaveStopBttn(
                        Widget w,
                        FindRec * find_rec,
                        XEvent * event) ;
static void EnterStopBttn(
                        Widget w,
                        FindRec * find_rec,
                        XEvent * event) ;
static void StopSearch(
                        Widget w,
                        FindRec *find_rec) ;
static void StartSearch(
                        Widget w,
                        FindRec *find_rec) ;
static void StartCallback(
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void StopCallback(
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static String GrowBuffer(
                        String buf,
                        int *size,
                        int extra) ;
static String MakeAbsolute(
                        String current_directory,
                        String path) ;
static Boolean FindProcessStarted(
                        FindRec *find_rec,
                        FindData *find_data) ;
static Boolean ExecuteFind(
                        FindRec *find_rec,
                        FindData *find_data,
                        FileMgrData *file_mgr_data) ;
static Boolean ExecuteGrep(
                        FindRec * find_rec) ;
static void AlternateInputHandler(
                        XtPointer client_data,
                        int *source,
                        XtInputId *id) ;
static void AlternateInputHandler2(
                        XtPointer client_data,
                        int *source,
                        XtInputId *id) ;
static void AlternateInputHandler3(
                        XtPointer client_data,
                        int *source,
                        XtInputId *id) ;
static void GetFileName(
                        Widget list,
                        int selectedItem,
                        String *host,
                        String *path,
                        FileMgrData *file_mgr_data) ;
static Boolean ExtractDirectory(
                        String host,
                        String path,
                        char **file_name) ;
static void NewView(
                        Widget widget,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void FindPutOnDesktop(
                        Widget widget,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void SetActiveItem(
                        Widget widget,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void SetFocus(
                        FindRec *find_rec,
                        FindData *find_data ) ;
FILE *findpopen(char *,char *,int *);

/********    End Static Function Declarations    ********/


/*
 *  The Dialog Class structure.
 */

static DialogClass findClassRec =
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
   (DialogSetFocusProc) SetFocus,
};

DialogClass * findClass = (DialogClass *) &findClassRec;
char *buffer;
static char *ptr;


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
   FindRec * find_rec;
   Widget shell, form, form1, form2;
   Widget newFM, outputSeparator;
   Widget headLabel, contentLabel, contentText;
   Widget filterText, filterLabel, listLabel, scrolledList, dirName, dirLabel;
#if defined(__hpux) || defined(sun)
   Widget followLink, followLinkPD;
#endif
   Widget putOnDT, separator;
   Widget start, stop, close, help;
   XmString label_string;

   Arg args[12];
   int n;
   XtTranslations trans_table;
   char * tmpStr;

   /* Initialize some global varibles */
   buffer = NULL;
   ptr = NULL;

   /*  Allocate the find file dialog instance record.  */

   find_rec = (FindRec *) XtMalloc (sizeof (FindRec));

   /*  Create the shell and form used for the dialog.  */

   n = 0;
   XtSetArg (args[n], XmNmwmFunctions, MWM_FUNC_MOVE |
             MWM_FUNC_CLOSE );                                  ++n;
   XtSetArg (args[n], XmNmwmDecorations, MWM_DECOR_BORDER |
             MWM_DECOR_TITLE);                                ++n;
   XtSetArg (args[n], XmNallowShellResize, False);              ++n;
   shell = XmCreateDialogShell (parent, "find_files", args, n);

   /* Set the useAsyncGeo on the shell */
   XtSetArg (args[0], XmNuseAsyncGeometry, True);
   XtSetValues (XtParent(shell), args, 1);

   trans_table = XtParseTranslationTable(translations_space);

   n = 0;
   XtSetArg (args[n], XmNmarginWidth, 1);				n++;
   XtSetArg (args[n], XmNmarginHeight, 1);				n++;
   XtSetArg (args[n], XmNshadowThickness, 1);			n++;
   XtSetArg (args[n], XmNshadowType, XmSHADOW_OUT);			n++;
   XtSetArg (args[n], XmNautoUnmanage, False);			n++;
   form = XmCreateForm (shell, "form", args, n);
   XtAddCallback(form, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_FIND_DIALOG_STR);

   label_string = XmStringCreateLocalized ((GETMESSAGE(15,47, "Fill in one or more fields to specify which items to find:")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);			n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNleftOffset, 5);				n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);			n++;
   XtSetArg (args[n], XmNtopOffset, 14);				n++;
   XtSetArg (args[n], XmNtraversalOn, False);				n++;
   headLabel = XmCreateLabelGadget (form, "hdlb", args, n);
   XtManageChild (headLabel);
   XmStringFree (label_string);

   label_string = XmStringCreateLocalized ((GETMESSAGE(15,31, "File or Folder Name: ")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);			n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNleftOffset, 5);				n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget, headLabel);				n++;
   XtSetArg (args[n], XmNtopOffset, 15);				n++;
   XtSetArg (args[n], XmNtraversalOn, False);				n++;
   filterLabel = XmCreateLabelGadget (form, "file_name_label", args, n);
   XtManageChild (filterLabel);
   XmStringFree (label_string);
   XtAddCallback(filterLabel, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_FIND_DIALOG_STR);

   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNleftWidget, filterLabel);			n++;
   XtSetArg (args[n], XmNleftOffset, 0);				n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget, headLabel);				n++;
   XtSetArg (args[n], XmNtopOffset, 10);				n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNrightOffset, 10);				n++;
   filterText = XmCreateTextField (form, "file_name_text", args, n);
   XtManageChild (filterText);
   XtAddCallback(filterText, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_FIND_DIALOG_STR);

   /* set up translations in the filter text edit widget */
   XtOverrideTranslations(filterText, trans_table);

   label_string = XmStringCreateLocalized ((GETMESSAGE(15,32, "File Contents:")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);			n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNleftOffset, 5);				n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget, filterText);			n++;
   XtSetArg (args[n], XmNtopOffset, 15);				n++;
   XtSetArg (args[n], XmNtraversalOn, False);				n++;
   contentLabel = XmCreateLabelGadget (form, "content_label", args, n);
   XtManageChild (contentLabel);
   XmStringFree (label_string);
   XtAddCallback(contentLabel, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_FIND_DIALOG_STR);

   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNleftWidget, contentLabel);			n++;
   XtSetArg (args[n], XmNleftOffset, 0);				n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget, filterText);			n++;
   XtSetArg (args[n], XmNtopOffset, 10);				n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNrightOffset, 10);				n++;
   contentText = XmCreateTextField (form, "content_text", args, n);
   XtManageChild (contentText);
   XtAddCallback(contentText, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_FIND_DIALOG_STR);

#if defined(__hpux) || defined(sun)
   n = 0;
   XtSetArg (args[n], XmNmarginWidth, 1);                               n++;
   XtSetArg (args[n], XmNmarginHeight, 1);                              n++;
   XtSetArg (args[n], XmNshadowThickness, 0);                           n++;
   XtSetArg (args[n], XmNautoUnmanage, False);                          n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);                n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);               n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);               n++;
   XtSetArg (args[n], XmNtopWidget, contentText);                       n++;
   XtSetArg (args[n], XmNtopOffset, 5);                                 n++;
   form1 = XmCreateForm (form, "form1", args, n);
   XtManageChild (form1);

    /* Create a Pulldown MenuPane that will contain the font sizes */
   followLinkPD = XmCreatePulldownMenu(form1, "fLinkPD", args, 0);

   label_string = XmStringCreateLocalized ((GETMESSAGE(15,33, "On")));
   XtSetArg(args[0], XmNmarginHeight, 2);
   XtSetArg(args[1], XmNmarginWidth, 12);
   XtSetArg(args[2], XmNlabelString, label_string); n++;
   find_rec->widgArry[0] =
                  XmCreatePushButtonGadget(followLinkPD, "On", args, 3);
   XmStringFree(label_string);

   label_string = XmStringCreateLocalized ((GETMESSAGE(15,34, "Off")));
   XtSetArg(args[2], XmNlabelString, label_string);
   find_rec->widgArry[1] =
                  XmCreatePushButtonGadget(followLinkPD, "Off", args, 3);
   XmStringFree(label_string);

   XtManageChildren(find_rec->widgArry, 2);

   label_string = XmStringCreateLocalized ((GETMESSAGE(15,35, "Follow Links: ")));
   /* create the Option Menu and attach it to the Pulldown MenuPane */
   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);                    n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);                n++;
   XtSetArg (args[n], XmNleftOffset, 30);                               n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);                 n++;
   XtSetArg (args[n], XmNtopOffset, 5);                                 n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);              n++;
   XtSetArg (args[n], XmNbottomOffset, 5);                              n++;
   XtSetArg(args[n], XmNsubMenuId, followLinkPD); n++;
   XtSetArg(args[n], XmNmenuHistory, find_rec->widgArry[OFF]); n++;
   followLink = XmCreateOptionMenu(form1, "fLink", args, n);
   XtManageChild (followLink);
   XtAddCallback(followLink, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_FIND_DIALOG_STR);

#endif

   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
#if defined(__hpux) || defined(sun)
   XtSetArg (args[n], XmNtopWidget, form1);		                n++;
#else
   XtSetArg (args[n], XmNtopWidget, contentText);	                n++;
#endif
   XtSetArg (args[n], XmNtopOffset, 5);					n++;
   outputSeparator = XmCreateSeparatorGadget (form, "opSeparator", args, n);
   XtManageChild (outputSeparator);

   /* Create the field for collecting the directory names to search */

   label_string = XmStringCreateLocalized (((char *)GETMESSAGE(15, 42, "Search Folder: ")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);			n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNleftOffset, 5);				n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget, outputSeparator);			n++;
   XtSetArg (args[n], XmNtopOffset, 15);				n++;
   XtSetArg (args[n], XmNtraversalOn, False);				n++;
   dirLabel = XmCreateLabelGadget (form, "folder_name_label", args, n);
   XtManageChild (dirLabel);
   XmStringFree (label_string);
   XtAddCallback(dirLabel, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_FIND_DIALOG_STR);

   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNleftWidget, dirLabel);				n++;
   XtSetArg (args[n], XmNleftOffset, 0);				n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNrightOffset, 10);				n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget, outputSeparator);			n++;
   XtSetArg (args[n], XmNtopOffset, 10);				n++;
   dirName = XmCreateTextField (form, "folder_name_text", args, n);
   XtManageChild (dirName);
   XtAddCallback(dirName, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_FIND_DIALOG_STR);

   /* set up translations in the search directory text edit widget */
   XtOverrideTranslations(dirName, trans_table);

   /* Create the widgets showing the matching files */

   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget, dirLabel);                          n++;
   XtSetArg (args[n], XmNtopOffset, 15);				n++;
   outputSeparator = XmCreateSeparatorGadget (form, "outputSeparator", args, n);
   XtManageChild (outputSeparator);

   label_string = XmStringCreateLocalized (GetSharedMessage(FILES_FOUND_LABEL));
   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);			n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNleftOffset, 5);				n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget, outputSeparator);			n++;
   XtSetArg (args[n], XmNtopOffset, 10);				n++;
   XtSetArg (args[n], XmNtraversalOn, False);				n++;
   listLabel = XmCreateLabelGadget (form, "files_found", args, n);
   XtManageChild (listLabel);
   XmStringFree (label_string);
   XtAddCallback(listLabel, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_FIND_DIALOG_STR);

   n = 0;
   XtSetArg (args[n], XmNlistSizePolicy, XmCONSTANT);			n++;
   XtSetArg (args[n], XmNscrollBarDisplayPolicy, XmSTATIC);		n++;
   XtSetArg (args[n], XmNvisibleItemCount, 5);				n++;
   scrolledList = XmCreateScrolledList (form, "file_list", args, n);
   XtManageChild (scrolledList);

   XtAddCallback (scrolledList, XmNbrowseSelectionCallback,
                                        SetActiveItem, (XtPointer) find_rec);
   XtAddCallback (scrolledList, XmNdefaultActionCallback,
                                              NewView, (XtPointer) find_rec);
   XtAddCallback(XtParent(scrolledList), XmNhelpCallback,
                 (XtCallbackProc)HelpRequestCB,
                 HELP_FIND_DIALOG_STR);


   n = 0;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget, listLabel);				n++;
   XtSetArg (args[n], XmNtopOffset, 5);					n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNleftOffset, 10);				n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNrightPosition, 10);				n++;
   XtSetValues (XtParent (scrolledList), args, n);

   n = 0;
   XtSetArg (args[n], XmNmarginWidth, 1);                               n++;
   XtSetArg (args[n], XmNmarginHeight, 1);                              n++;
   XtSetArg (args[n], XmNshadowThickness, 0);                           n++;
   XtSetArg (args[n], XmNautoUnmanage, False);                          n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget, scrolledList);			n++;
   XtSetArg (args[n], XmNtopOffset, 10);				n++;
   form2 = XmCreateForm (form, "form2", args, n);
   XtManageChild (form2);
/*
   XtAddCallback(form2, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_FIND_DIALOG_STR);
*/

   label_string = XmStringCreateLocalized ((GETMESSAGE(15,48, "Open Folder")));

   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);			n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);		n++;
   XtSetArg (args[n], XmNleftPosition, 10);				n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);			n++;
   XtSetArg (args[n], XmNtopOffset, 5);					n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);              n++;
   XtSetArg (args[n], XmNbottomOffset, 5);                              n++;
   XtSetArg (args[n], XmNmarginHeight, 4);				n++;
   XtSetArg (args[n], XmNmarginWidth, 10);				n++;
   newFM = XmCreatePushButtonGadget (form2, "new_view", args, n);
   XtManageChild (newFM);
   XtAddCallback (newFM, XmNactivateCallback, NewView, (XtPointer) find_rec);
   XtAddCallback(newFM, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_FIND_DIALOG_STR);
   XmStringFree (label_string);

   XtSetArg (args[0], XmNdefaultButton, newFM);
   XtSetValues (form2, args, 1);

   label_string = XmStringCreateLocalized ((GETMESSAGE(15,37, "Put In Workspace")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);                    n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);            n++;
   XtSetArg (args[n], XmNleftPosition, 55);                             n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);                 n++;
   XtSetArg (args[n], XmNtopOffset, 5);                                 n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);              n++;
   XtSetArg (args[n], XmNbottomOffset, 5);                              n++;
   XtSetArg (args[n], XmNmarginHeight, 4);                              n++;
   XtSetArg (args[n], XmNmarginHeight, 4);                              n++;
   XtSetArg (args[n], XmNmarginWidth, 10);                              n++;
   putOnDT = XmCreatePushButtonGadget (form2, "putInWorkspace", args, n);
   XtManageChild (putOnDT);
   XtAddCallback (putOnDT, XmNactivateCallback, FindPutOnDesktop,
                  (XtPointer) find_rec);
   XtAddCallback(putOnDT, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_FIND_DIALOG_STR);
   XmStringFree (label_string);

   /*  Create a separator between the buttons  */

   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget, form2);				n++;
   XtSetArg (args[n], XmNtopOffset, 10);				n++;
   separator =  XmCreateSeparatorGadget (form, "separator", args, n);
   XtManageChild (separator);


   /*  Create the action buttons  */

   label_string = XmStringCreateLocalized (((char *)GETMESSAGE(15, 14, "Start")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);			n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);		n++;
   XtSetArg (args[n], XmNleftPosition, 1);				n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);		n++;
   XtSetArg (args[n], XmNrightPosition, 24);				n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget, separator);				n++;
   XtSetArg (args[n], XmNtopOffset, 5);					n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNbottomOffset, 5);				n++;
   XtSetArg (args[n], XmNmarginHeight, 4);				n++;
   start = XmCreatePushButtonGadget (form, "start", args, n);
   XtAddCallback (start, XmNactivateCallback, StartCallback,
							(XtPointer) find_rec);
   XtAddCallback(start, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_FIND_DIALOG_STR);
   XtManageChild (start);
   XmStringFree (label_string);

   label_string = XmStringCreateLocalized (((char *)GETMESSAGE(15, 15, "Stop")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);			n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);		n++;
   XtSetArg (args[n], XmNleftPosition, 26);				n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);		n++;
   XtSetArg (args[n], XmNrightPosition, 49);				n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget, separator);				n++;
   XtSetArg (args[n], XmNtopOffset, 5);					n++;
   XtSetArg (args[n], XmNmarginHeight, 4);				n++;
   stop = XmCreatePushButton (form, "stop", args, n);
   XtAddCallback (stop, XmNactivateCallback, StopCallback,
						   (XtPointer) find_rec);
   XtAddCallback(stop, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_FIND_DIALOG_STR);
   XtManageChild (stop);
   XtSetSensitive (stop, False);
   XmStringFree (label_string);

   n = 0;
   XtSetArg (args[n], XmNlabelString, cancelXmString);			n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);		n++;
   XtSetArg (args[n], XmNleftPosition, 51);				n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);		n++;
   XtSetArg (args[n], XmNrightPosition, 74);				n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget, separator);				n++;
   XtSetArg (args[n], XmNtopOffset, 5);					n++;
   XtSetArg (args[n], XmNmarginHeight, 4);				n++;
   close = XmCreatePushButtonGadget (form, "close", args, n);
   XtManageChild (close);
   XtAddCallback(close, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_FIND_DIALOG_STR);


   n = 0;
   XtSetArg (args[n], XmNlabelString, helpXmString);			n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);		n++;
   XtSetArg (args[n], XmNleftPosition, 76);				n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);		n++;
   XtSetArg (args[n], XmNrightPosition, 99);				n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget, separator);				n++;
   XtSetArg (args[n], XmNtopOffset, 5);					n++;
   XtSetArg (args[n], XmNmarginHeight, 4);				n++;
   help = XmCreatePushButton (form, "help", args, n);
   XtManageChild (help);
   XtAddCallback(help, XmNactivateCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_FIND_DIALOG_STR);
   XtAddCallback(help, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_FIND_DIALOG_STR);



   XtSetArg (args[0], XmNdefaultButton, start);
   XtSetArg (args[1], XmNcancelButton, close);
   XtSetValues (form, args, 2);

   /* Fill in our instance structure */

   find_rec->shell = shell;
   find_rec->form = form;
   find_rec->fileNameFilter = filterText;
   find_rec->content = contentText;
#if defined(__hpux) || defined(sun)
   find_rec->followLink = followLink;
#else
   find_rec->followLink = NULL;
#endif
   find_rec->listLabel = listLabel;
   find_rec->matchList = scrolledList;
   find_rec->searchDirectory = dirName;
   find_rec->newFM = newFM;
   find_rec->putOnDT = putOnDT;
   find_rec->start = start;
   find_rec->stop = stop;
   find_rec->close = close;
   find_rec->help = help;

   find_rec->selectedItem = -1;
   find_rec->popenId = NULL;
   find_rec->childpid = -1;
   find_rec->alternateInputId = 0;
   find_rec->searchInProgress = False;
   find_rec->fileMgrRec = NULL;


   /*  Set the return values for the dialog widget and dialog instance.  */

   *return_widget = form;
   *dialog = (XtPointer) find_rec;
}



/************************************************************************
 *
 *  InstallChange
 *
 ************************************************************************/

static void
InstallChange(
        FindRec *find_rec,
        XtCallbackProc callback,
        XtPointer client_data )
{
   FindApply * apply_data;


   /*  Setup the callback data to be sent to the most of our */
   /*  actions callbacks.                                    */

   apply_data = (FindApply *) XtMalloc (sizeof (FindApply));
   apply_data->callback = callback;
   apply_data->client_data = client_data;
   apply_data->find_rec = (XtPointer) find_rec;
   find_rec->apply_data = apply_data;
}




/************************************************************************
 *
 *  InstallClose
 *
 ************************************************************************/

static void
InstallClose(
        FindRec *find_rec,
        XtCallbackProc callback,
        XtPointer client_data )
{
   Atom delete_window_atom;

   XtAddCallback (find_rec->close, XmNactivateCallback, callback, client_data);

   delete_window_atom = XmInternAtom (XtDisplay(find_rec->shell), "WM_DELETE_WINDOW", True);
   XmRemoveWMProtocols( find_rec->shell, &delete_window_atom, 1 );
   XmAddWMProtocolCallback( find_rec->shell, delete_window_atom, callback,
                            (XtPointer) client_data );
}




/************************************************************************
 *
 *  Destroy
 *
 ************************************************************************/

static void
Destroy(
        FindRec *find_rec )
{
   XtDestroyWidget (find_rec->shell);
   XtFree ((char *) find_rec->apply_data);
   XtFree ((char *) find_rec);
}




/************************************************************************
 *
 *  GetValues
 *
 ************************************************************************/

static XtPointer
GetValues(
        FindRec *find_rec )
{
   FindData * find_data;
   Arg args[4];


   /*  Allocate and initialize the find file dialog data.  */

   find_data = (FindData *) XtMalloc (sizeof (FindData));

   find_data->displayed = True;

   XtSetArg (args[0], XmNx, &find_data->x);
   XtSetArg (args[1], XmNy, &find_data->y);
   XtSetArg (args[2], XmNwidth, &find_data->width);
   XtSetArg (args[3], XmNheight, &find_data->height);
   XtGetValues (find_rec->shell, args, 4);

   (void) GetFindValues (find_rec, find_data, False);

   return ((XtPointer) find_data);
}




/************************************************************************
 *
 *  GetDefaultValues
 *
 ************************************************************************/

static XtPointer
GetDefaultValues( void )
{
   FindData *find_data;
   char     dirbuf[MAX_DIR_PATH_LEN];

   /*  Allocate and initialize the default find file dialog data.  */

   find_data = (FindData *) XtMalloc (sizeof (FindData));

   find_data->displayed = False;
   find_data->x = 0;
   find_data->y = 0;
   find_data->height = 0;
   find_data->width = 0;

   (char *)getcwd((char *)dirbuf, (unsigned int)MAX_DIR_PATH_LEN);
   if(restrictMode &&
           strncmp(users_home_dir, dirbuf, strlen(users_home_dir)) != 0)
   {
      find_data->directories = (char *)XtMalloc(strlen(users_home_dir) + 1);
      strcpy(find_data->directories, users_home_dir);
   }
   else
   {
      find_data->directories = (char *)XtMalloc(strlen(dirbuf) + 1);
      strcpy(find_data->directories, dirbuf);
   }

   find_data->filter = NULL;
   find_data->content = NULL;
   find_data->matches = NULL;
   find_data->num_matches = 0;
   find_data->selected_item = -1;
#if defined(__hpux) || defined(sun)
   find_data->follow_links = follow_links;
#endif

   return ((XtPointer) find_data);
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
   FindData * find_data;
   Dummy dummy;
   static Boolean convertersAdded = False;


   /*  Allocate and get the resources for find file dialog data.  */

   find_data = (FindData *) XtCalloc (1, sizeof (FindData));

   _DtDialogGetResources (data_base, name_list, FIND_FILE, (char *) find_data,
                       resources, findClass->resource_count);

   /* Create new strings for any string value we read in from our
    * resourceDB.
    */
   find_data->directories = XtNewString(find_data->directories);
   find_data->filter = XtNewString(find_data->filter);
   find_data->content = XtNewString(find_data->content);

   /*  Do a special read to get the list of matches.                 */
   /*  The information is read in as a single string, and then must  */
   /*  be converted into the internal array format.                  */

   _DtDialogGetResources (data_base, name_list, FIND_FILE, (char *) (&dummy),
                       match_resources, XtNumber(match_resources));

   CvtStringToStringList (dummy.string, &find_data->matches,
                          &find_data->num_matches);

   return ((XtPointer) find_data);
}




/************************************************************************
 *
 *  SetValues
 *
 ************************************************************************/

static void
SetValues(
        FindRec *find_rec,
        FindData *find_data )
{
   Arg args[2];
   XmString * matches;
   int i;

   XmTextFieldSetString (find_rec->fileNameFilter, find_data->filter);
   XmTextFieldSetInsertionPosition(find_rec->fileNameFilter,
                        XmTextFieldGetLastPosition(find_rec->fileNameFilter));
   XmTextFieldSetString(find_rec->content, find_data->content);
   XmTextFieldSetInsertionPosition(find_rec->content,
                        XmTextFieldGetLastPosition(find_rec->content));

   if(find_data->directories)
      XtFree(find_data->directories);
   find_data->directories =
                 XtNewString(find_data->file_mgr_data->current_directory );

   XmTextFieldSetString(find_rec->searchDirectory, find_data->directories);
   XmTextFieldSetInsertionPosition(find_rec->searchDirectory,
                        XmTextFieldGetLastPosition(find_rec->searchDirectory));
   if( find_data->file_mgr_data->restricted_directory )
   {
      XtAddCallback (find_rec->searchDirectory, XmNmodifyVerifyCallback,
                     (XtCallbackProc)TextChange,
                     (XtPointer)find_data->file_mgr_data );
      XtAddCallback (find_rec->searchDirectory, XmNmotionVerifyCallback,
                     (XtCallbackProc)TextChange,
                     (XtPointer)find_data->file_mgr_data );
   }

   /* Update the list of matches */

   if (find_data->num_matches > 0)
   {
      XtSetArg (args[0], XmNitemCount, find_data->num_matches);
      matches =
         (XmString *) XtMalloc (sizeof (XmString) * find_data->num_matches);

      for (i = 0; i < find_data->num_matches; i++)
      {
         matches[i] = XmStringCreateLocalized (find_data->matches[i]);
      }

      XtSetArg (args[1], XmNitems, matches);
      XtSetValues (find_rec->matchList, args, 2);

      for (i = 0; i < find_data->num_matches; i++)
         XmStringFree (matches[i]);

      XtFree ((char *) matches);

      if (find_data->selected_item != -1)
      {
         XmListSelectPos (find_rec->matchList,
                          find_data->selected_item + 1,False);
      }
   }
   else
   {
      XtSetArg (args[0], XmNitemCount, 0);
      XtSetValues (find_rec->matchList, args, 1);
   }

#if defined(__hpux) || defined(sun)
   /* Set up the Follow links option menu */
   if(find_data->follow_links)
      XtSetArg(args[0], XmNmenuHistory, find_rec->widgArry[ON]);
   else
      XtSetArg(args[0], XmNmenuHistory, find_rec->widgArry[OFF]);
   XtSetValues (find_rec->followLink, args, 1);
#endif

   /* Update button sensitivity, if one of the matches is selected */

   if (find_data->selected_item != -1)
   {
      XtSetSensitive (find_rec->newFM, True);
      XtSetSensitive (find_rec->putOnDT, True);
   }
   else
   {
      XtSetSensitive (find_rec->newFM, False);
      XtSetSensitive (find_rec->putOnDT, False);
   }

   find_rec->selectedItem = find_data->selected_item;

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
   FindData * find_data = (FindData *) values->data;
   FindRec  * find_rec;
   Arg args[2];
   Dummy dummy;


   /*  If the dialog is currently displayed, update the geometry  */
   /*  fields to their current values.                            */

   if (find_data->displayed == True)
   {
      _DtGenericUpdateWindowPosition(values);
      find_rec = (FindRec *) _DtGetDialogInstance (values);
      (void) GetFindValues (find_rec, find_data, False);
   }

   _DtDialogPutResources (fd, name_list, FIND_FILE, (char *) values->data,
                       resources, findClass->resource_count);


   /*  Special case for writing the array of matching files.          */
   /*  Write all of the information out as a single string, which     */
   /*  we'll parse when we read it back in at a later point in time.  */
   /*  The format for this string is:                                 */
   /*                                                                 */
   /*     <match string1>, <match string2>, <match string3>, ...      */

   if (find_data->num_matches > 0)
   {
      dummy.displayed = False;
      dummy.string = CvtStringListToString (find_data->matches,
                                            find_data->num_matches);

      _DtDialogPutResources (fd, name_list, FIND_FILE, (char *) &dummy,
                          match_resources, XtNumber (match_resources));

      XtFree ((char *) dummy.string);
      dummy.string = NULL;
   }
}




/************************************************************************
 *
 *  FreeValues
 *
 ************************************************************************/

static void
FreeValues(
        FindData *find_data )
{
   if( find_data )
   {
      if( find_data->filter )
         XtFree ((char *) find_data->filter);
      if( find_data->directories )
         XtFree ((char *) find_data->directories);
      FreeMatchInfo (find_data->matches, find_data->num_matches);
      XtFree ((char *) find_data);
   }
}


/************************************************************************
 *
 *  GetFindValues
 *	Update the current find file values within the data structure
 *	from the current values.
 *
 ************************************************************************/

static Boolean
GetFindValues(
        FindRec *find_rec,
        FindData *find_data,
        Boolean validate )
{
   Arg args[2];
   int i;
   XmString * stringTable;


   /* Get the filename to search for (e.g. Filter) */

   find_data->displayed = True;
   find_data->content = XmTextFieldGetString (find_rec->content);
   find_data->filter = (char *)_DtStripSpaces (
                              XmTextFieldGetString (find_rec->fileNameFilter));
   if (validate)
   {
     if ((strlen (find_data->filter) == 0) &&
					 (strcmp(find_data->content, "") == 0))
      {
         XtFree ((char *) find_data->filter);
         find_data->filter = NULL;
      }
   }

   /* Get the list of directories to search */
   if(find_data->directories)
      XtFree(find_data->directories);
   find_data->directories = XmTextFieldGetString (find_rec->searchDirectory);

   if (validate)
   {
      find_data->directories = (char *)_DtStripSpaces (find_data->directories);

      if (strlen (find_data->directories) == 0)
      {
         /* A directory must be supplied */

         InvalidFindMessage (find_rec, BAD_DIR_NAME, NULL);
         XtFree ((char *) find_data->filter);
         find_data->filter = NULL;
         XtFree ((char *) find_data->directories);
         find_data->directories = NULL;
         return (False);
      }
   }

   /* Make a copy of the array of matching strings */

   find_data->selected_item = find_rec->selectedItem;
   XtSetArg (args[0], XmNitemCount, &find_data->num_matches);
   XtSetArg (args[1], XmNitems, &stringTable);
   XtGetValues (find_rec->matchList, args, 2);
   if (find_data->num_matches == 0)
   {
      find_data->matches = NULL;
   }
   else
   {
      find_data->matches =
         (String *) XtMalloc (sizeof(String) * find_data->num_matches);

      for (i = 0; i < find_data->num_matches; i++)
      {
         find_data->matches[i] = (char *) _XmStringUngenerate(stringTable[i],
                                                  XmFONTLIST_DEFAULT_TAG,
                                                  XmCHARSET_TEXT, XmCHARSET_TEXT);
      }
   }

#if defined(__hpux) || defined(sun)
   {
      Widget menuHistory;

      XtSetArg (args[0], XmNmenuHistory, &menuHistory);
      XtGetValues (find_rec->followLink, args, 1);

      if(menuHistory == find_rec->widgArry[ON])
         find_data->follow_links = True;
      else
         find_data->follow_links = False;
   }
#endif

   return (True);
}




/************************************************************************
 *
 *  InvalidFindMessage
 *	Display an error message.
 *
 ************************************************************************/

static void
InvalidFindMessage(
        FindRec * find_rec,
        int messageIndex,
        String extra_string )
{
   String string;
   String new_string;
   char * title;
   static String badDirectoryNameMessage = NULL;
   static String noDirectoryAccessMessage = NULL;
   static String noSearchArgumentMessage = NULL;
   static String noExistanceMessage = NULL;
   char * tmpStr;

   if (noExistanceMessage == NULL)
   {
      tmpStr = GETMESSAGE(15,44, "Search Folder name argument is missing.\nType in the name of the folder where you want the search to begin.");
      badDirectoryNameMessage = XtNewString(tmpStr);

      tmpStr = GetSharedMessage(NO_DIR_ACCESS_ERROR);
      noDirectoryAccessMessage = XtNewString(tmpStr);

      tmpStr = GETMESSAGE(15, 50, "Search Folder name or File Content argument is missing\nType in the name of the folder where you want the search to begin.\nOr type in the string that you want to search.");
      noSearchArgumentMessage = XtNewString(tmpStr);

      tmpStr = GETMESSAGE(15,45, "The selected file no longer exists.\n\nSomeone deleted the file after the search process completed.");
      noExistanceMessage = XtNewString(tmpStr);
   }


   switch (messageIndex)
   {
      case BAD_DIR_NAME:
           string = badDirectoryNameMessage;
           break;
      case NO_DIR_ACCESS:
           string = noDirectoryAccessMessage;
           break;
      case NO_EXISTANCE:
           string = noExistanceMessage;
           break;
       case NO_FILE_OR_FOLDER_ARG:
           string = noSearchArgumentMessage;
           break;
   }

   if (extra_string)
   {
      new_string = XtMalloc (strlen(string) + strlen(extra_string) + 1);
      (void) sprintf(new_string, string, extra_string);
      tmpStr = GetSharedMessage(FIND_ERROR_TITLE);
      title = XtNewString(tmpStr);
      _DtMessage (find_rec->shell, title, new_string, NULL, HelpRequestCB);
      XtFree ((char *) new_string);
      XtFree(title);
   }
   else
   {
      tmpStr = GetSharedMessage(FIND_ERROR_TITLE);
      title = XtNewString(tmpStr);
      _DtMessage (find_rec->shell, title, string, NULL, HelpRequestCB);
      XtFree(title);
   }

   _DtTurnOffHourGlass (find_rec->shell);
}




/************************************************************************
 *
 *  FreeMatchInfo()
 *	Free up the space occupied by the array containing the match
 *      strings.
 *
 ************************************************************************/

static int
FreeMatchInfo(
        String *matches,
        int numMatches )
{
   int i;

   if (matches == NULL)
      return;

   for (i = 0; i < numMatches; i++)
      XtFree ((char *) matches[i]);


   XtFree ((char *) matches);
}



/************************************************************************
 *
 *  StopSearch()
 *	Abort an active search operation.
 *
 ************************************************************************/

static void
StopSearch(
        Widget w,
        FindRec *find_rec )
{
   Arg args[2];


   /* To avoid a race condition where the input processing routine   */
   /* detected the end of the find operation and cleaned things up,  */
   /* just as the user hit the 'stop' key, we need to check to see   */
   /* if the operation is still active.                              */

   if (find_rec->popenId != NULL)
   {
      /* Abort the find process, and remove the alternate input handler */
      (void) fclose (find_rec->popenId);
      if(find_rec->childpid > 1)  /* trying to be safe */
        kill(find_rec->childpid,SIGTERM);  /* Ignore errors */
      find_rec->popenId = NULL;
      find_rec->childpid = -1;
      XtRemoveInput (find_rec->alternateInputId);
      find_rec->alternateInputId = 0;
      find_rec->searchInProgress = False;
   }


   /* Change button sensitivities */

   XtSetSensitive (find_rec->start, True);
   XtSetSensitive (find_rec->close, True);
   XtSetSensitive (find_rec->stop, False);

   XtSetArg (args[0], XmNdefaultButton, find_rec->start);
   XtSetValues (find_rec->form, args, 1);

   /*
    */

   XtRemoveEventHandler (find_rec->stop, LeaveWindowMask, FALSE, (XtEventHandler)LeaveStopBttn, find_rec);
   XtRemoveEventHandler (find_rec->stop, EnterWindowMask, FALSE, (XtEventHandler)EnterStopBttn, find_rec);

   _DtTurnOffHourGlass (find_rec->shell);
   XmUpdateDisplay (w);
   XtFree(buffer);
   buffer = NULL;
   ptr = NULL;
}




/************************************************************************
 *
 *  StartSearch()
 *	Start an active search operation.
 *
 ************************************************************************/

static void
StartSearch(
        Widget w,
        FindRec *find_rec )
{
   FindData * find_data;
   Arg args[3];

   _DtTurnOnHourGlass (find_rec->shell);

   /* Extract current dialog values; continue only if data is valid */

   find_data = (FindData *) XtMalloc (sizeof (FindData));

   if (!GetFindValues (find_rec, find_data, True))
   {
      /* Dialog contained bogus values; abort request */

      XtFree ((char *) find_data);
      return;
   }


   /* Desensitize buttons we don't want working during a search */

   XtSetSensitive (find_rec->stop, True);
   XtSetSensitive (find_rec->start, False);
   XtSetSensitive (find_rec->close, False);
   XtSetSensitive (find_rec->newFM, False);
   XtSetSensitive (find_rec->putOnDT, False);

   XtSetArg (args[0], XmNdefaultButton, find_rec->stop);
   XtSetArg (args[1], XmNcancelButton, find_rec->close);
   XtSetValues (find_rec->form, args, 2);

   /* Clean out the match list */

   find_rec->selectedItem = -1;
   XtSetArg (args[0], XmNitemCount, 0);
   XtSetValues (find_rec->matchList, args, 1);

   XmUpdateDisplay (w);


   /* Start the find process; not much we can say if it fails! */

   if (!FindProcessStarted (find_rec, find_data))
   {
      XtSetSensitive (find_rec->close, True);
      XtSetSensitive (find_rec->newFM, False);
      XtSetSensitive (find_rec->putOnDT, False);
      XtSetSensitive (find_rec->start, True);
      XtSetSensitive (find_rec->stop, False);

      XtSetArg (args[0], XmNdefaultButton, find_rec->start);
      XtSetArg (args[1], XmNcancelButton, find_rec->close);
      XtSetValues (find_rec->form, args, 2);

      XmUpdateDisplay (w);
   }
   else
   {
      find_rec->searchInProgress = True;

      /* This event handlers will be called when user is moving his mouse
         over the Find dialog's stop button.
      */
      XtAddEventHandler( find_rec->stop, LeaveWindowMask, FALSE, (XtEventHandler)LeaveStopBttn, find_rec );
      XtAddEventHandler( find_rec->stop, EnterWindowMask, FALSE, (XtEventHandler)EnterStopBttn, find_rec );
   }

   /* Free up the dialog values we allocated */

   FreeValues (find_data);
}


/************************************************************************
 *
 *  EnterStopBttn()
 *
 ************************************************************************/

static void
EnterStopBttn(
              Widget w,
              FindRec * find_rec,
              XEvent * event )
{
  _DtTurnOffHourGlass (find_rec->shell);
}


/************************************************************************
 *
 *  LeaveStopBttn()
 *
 ************************************************************************/

static void
LeaveStopBttn(
              Widget w,
              FindRec * find_rec,
              XEvent * event )
{
  _DtTurnOnHourGlass (find_rec->shell);
}


/************************************************************************
 *
 *  StartCallback()
 *	Start a search operation.
 *
 ************************************************************************/

static void
StartCallback(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
   StartSearch (w, (FindRec *) client_data);
}




/************************************************************************
 *
 *  StopCallback
 *	Stop an active search operation.
 *
 ************************************************************************/

static void
StopCallback(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
   StopSearch (w, (FindRec *) client_data);
}




/************************************************************************
 *
 *  GrowBuffer
 *
 ************************************************************************/

static String
GrowBuffer(
        String buf,
        int *size,
        int extra )
{
   if (strlen (buf) + 1 + extra >= *size)
   {
      *size = strlen(buf) + extra + 1025;
      buf = XtRealloc (buf, *size);
   }

   return (buf);
}




/************************************************************************
 *
 *  MakeAbsolute()
 *	Change relative path to absolute one.
 *
 ************************************************************************/

static String
MakeAbsolute(
        String current_directory,
        String path )
{
   String absPath;

   absPath = XtMalloc (strlen (path) + strlen (current_directory) + 2);
   (void) strcpy (absPath, current_directory);
   (void) strcat (absPath, "/");
   (void) strcat (absPath, path);

   return (absPath);
}




/************************************************************************
 *
 *  FindProcessStarted()
 *      Determine whether to do a 'find' or a 'grep'.
 *
 ************************************************************************/

static Boolean
FindProcessStarted(
        FindRec *find_rec,
        FindData *find_data )
{
   FileMgrData * file_mgr_data;
   DialogData * dialog_data;

   dialog_data = _DtGetInstanceData ((XtPointer) (find_rec->fileMgrRec));
   file_mgr_data = (FileMgrData *) dialog_data->data;

   return(ExecuteFind(find_rec, find_data, file_mgr_data));

}

/************************************************************************
 *
 *  ExecuteFind()
 *	Create the command string for invoking the 'find' process,
 *      and then execute it.
 *
 ************************************************************************/

static Boolean
ExecuteFind(
   FindRec * find_rec,
   FindData * find_data,
   FileMgrData *file_mgr_data)
{
   int commandLen;
   String command;
   String findptr;
   String host;
   String path;
   int access_priv;
   XmString label_string;
   char *tmpStr;
   Arg args[1];
#if defined (SVR4)  || defined(_AIX) || defined(sco)
/* needed for getaccess () call */
   int save_ruid;
   int save_rgid;
#endif /* SVR4 */
   char *link_path;
   void (*oldSig)();
   Tt_status tt_status;
#ifdef __osf__
   extern void sigchld_handler(int);
#endif /* __osf__ */

   if(strcmp(find_data->content, "") == 0)
   {
      label_string = XmStringCreateLocalized (GetSharedMessage(FILES_FOUND_LABEL));
   }
   else if(strcmp(find_data->filter, "") == 0)
   {
      tmpStr = GETMESSAGE(15,38, "Files Found (by Contents):");
      label_string = XmStringCreateLocalized (tmpStr);
   }
   else
   {
      tmpStr = (GETMESSAGE(15,39, "Files Found (by Name and Contents):"));
      label_string = XmStringCreateLocalized (tmpStr);
   }
   XtSetArg (args[0], XmNlabelString, label_string);
   XtSetValues (find_rec->listLabel, args, 1);
   XmStringFree(label_string);

   if(find_data->filter == NULL )
   {
     InvalidFindMessage (find_rec, NO_FILE_OR_FOLDER_ARG, NULL);
     return( False );
   }


   /* Construct the 'find' command */

   commandLen = 1024;
   command = XtMalloc (commandLen);
   (void) strcpy (command, FIND_COMMAND);


   /* Convert directory names from external to internal (nfs) format */

   findptr = find_data->directories;


   /*  Search for the end of the directory component  */

   _DtPathFromInput(findptr, file_mgr_data->current_directory, &host, &path);

   if (path == NULL)
   {
     InvalidFindMessage (find_rec, NO_DIR_ACCESS, findptr);
     return( False );
   }

   link_path = _DtFollowLink(path);
   XtFree(path);
   path = XtNewString(link_path);

   if(path == NULL)
   {
      XtFree(command);
      return False;
   }
   /* Verify that the path exists and is accessible */
#if defined (SVR4)  || defined(_AIX) || defined(sco)
/* needed for getaccess () call */
   save_ruid = getuid();
#if !defined(SVR4) && ! defined(sco)
   setreuid(geteuid(),-1);
#else
   setuid(geteuid());
#endif
   save_rgid = getgid();
#if !defined(SVR4) && !defined(sco)
   setregid(getegid(),-1);
#else
   setgid(getegid());
#endif
   access_priv = access (path, R_OK);
#if !defined(SVR4) && !defined(sco)
   setreuid(save_ruid,-1);
   setregid(save_rgid,-1);
#else
   setuid(save_ruid);
   setgid(save_rgid);
#endif
   if (access_priv == -1 && geteuid() != root_user)
   {
#else
#  if defined(__hp_osf) || defined(__ultrix) || defined(__osf__) || defined(linux) || defined(CSRG_BASED)
   setreuid(geteuid(),-1);
   if (access ((char *) path, R_OK) == -1)
   {
#  else
#    ifdef BLS
   setresuid(geteuid(),-1,-1);
   if (access ((char *) path, R_OK) == -1)
   {
#    else
   if ((((access_priv = getaccess (path, UID_EUID, NGROUPS_EGID_SUPP,
                                      0, (void *) 0, (void *) 0)) == -1) ||
             !(access_priv & R_OK)) && (geteuid () != root_user))
   {
#    endif /* BLS */
#  endif /* Apollo & OSF */
#endif /* SVR4 */
      /* Post an error dialog, and then terminate the request */

      InvalidFindMessage (find_rec, NO_DIR_ACCESS, findptr);
      XtFree ((char *) path);
      XtFree ((char *) command);
      return (False);
   }


   /* See if the buffer needs to grow */

   command = GrowBuffer (command, &commandLen, (int) strlen (path));


   /* Add path to the command string */

   (void) strcat (command, path);
   (void) strcat (command, " ");
   XtFree ((char *) path);

   /* Add on the rest of the search constraints */

   if(strcmp(find_data->content, "") != 0)
   {
      command = GrowBuffer (command, &commandLen, (int) strlen (TYPEDIR));
      (void) strcat (command, TYPEDIR);
   }
   else
   {
       /* File name regular expression */
       if (find_data->filter)
       {
          command =
             GrowBuffer (command, &commandLen, (int)strlen (find_data->filter) +
                                                (int) strlen (NAME_OPTION) + 2);

          /* The string needs to be quoted */
          (void) strcat (command, NAME_OPTION);
          (void) strcat (command, "\"");
          (void) strcat (command, find_data->filter);
          (void) strcat (command, "\" ");
       }

   }

#if defined(__hpux) || defined(sun)
   {
      Widget menuHistory;

      XtSetArg (args[0], XmNmenuHistory, &menuHistory);
      XtGetValues (find_rec->followLink, args, 1);

      if(menuHistory == find_rec->widgArry[ON])
      {
         /* Add the option to follow a link */

         command = GrowBuffer (command, &commandLen,
                                    (int) strlen (FOLLOW_OPTION));
         (void) strcat (command, FOLLOW_OPTION);
      }
   }
#endif

   /* Add the -print to get the results of the find */

   command = GrowBuffer (command, &commandLen, (int) strlen (PRINT_OPTION));
   (void) strcat (command, PRINT_OPTION);


   /* Add the redirector for stderr, so it is disabled */

   command = GrowBuffer (command, &commandLen, (int) strlen (REDIRECTOR));
   (void) strcat (command, REDIRECTOR);


   /* Start the 'find' process */

#ifdef __osf__
   oldSig = signal(SIGCHLD, sigchld_handler);
#else
   oldSig = signal(SIGCHLD, SIG_DFL);
#endif /* __osf__ */
   find_rec->popenId = findpopen(command, "r",&(find_rec->childpid));
   signal (SIGCHLD, oldSig);

   if (find_rec->popenId == NULL)
   {
      XtFree ((char *) command);
      return (False);
   }


   /* Set up the alternate input source handler */

   if(strcmp(find_data->content, "") != 0)
   {
      find_rec->alternateInputId =
         XtAddInput (fileno (find_rec->popenId), (XtPointer)XtInputReadMask,
                  (XtInputCallbackProc)AlternateInputHandler2, find_rec);
   }
   else
   {
      find_rec->alternateInputId =
         XtAddInput (fileno (find_rec->popenId), (XtPointer)XtInputReadMask,
                  (XtInputCallbackProc)AlternateInputHandler, find_rec);
   }

   /* printf ("%s\n", command); */

   XtFree ((char *) command);

   return (True);
}


/************************************************************************
 *
 *  ExecuteGrep()
 *	Create the command string for invoking the 'grep' process,
 *      and then execute it.
 *
 ************************************************************************/
static Boolean
ExecuteGrep( FindRec * find_rec)
{
   int commandLen;
   String command;
   Arg args[1];
   char *contents;
   char *ptr2;
   int item_count;
   char * title;
   char * msg;

   /* Construct the 'grep' command */
   commandLen = 1024;
   command = XtMalloc (commandLen);
   (void) strcpy (command, GREP_COMMAND);

   contents = XmTextFieldGetString (find_rec->content);

   command = GrowBuffer (command, &commandLen, strlen(contents) + 4);

   (void) strcat (command, "\"");
   (void) strcat (command, contents);
   (void) strcat (command, "\"");
   (void) strcat (command, " ");

   if( ptr == NULL)
      ptr = buffer;

   ptr2 = DtStrchr(ptr, ',');
   if(ptr2 == NULL)
   {
      XtFree ((char *) command);
      XtFree(buffer);
      buffer = NULL;
      ptr = NULL;
      if(find_rec->popenId != NULL)
         (void) fclose (find_rec->popenId);
      find_rec->popenId = NULL;
      find_rec->alternateInputId = 0;

      find_rec->searchInProgress = False;

      /* Reset button sensitivity */

      XtSetSensitive (find_rec->close, True);
      XtSetSensitive (find_rec->start, True);
      XtSetSensitive (find_rec->stop, False);

      XtSetArg (args[0], XmNitemCount, &item_count);
      XtGetValues (find_rec->matchList, args, 1);

      XtSetArg (args[0], XmNdefaultButton, find_rec->start);
      XtSetValues (find_rec->form, args, 1);

      if (item_count == 0)
      {
         char * tmpStr;

         tmpStr = GetSharedMessage(FIND_ERROR_TITLE);
         title = XtNewString(tmpStr);
         tmpStr = GetSharedMessage(NO_FILES_FOUND_ERROR);
         msg = XtNewString(tmpStr);
         _DtMessage (find_rec->shell, title, msg, NULL, HelpRequestCB);
         XtFree(title);
         XtFree(msg);
      }
      else
      {
         XmListSelectPos(find_rec->matchList, 1, True);
         XmProcessTraversal(find_rec->matchList, XmTRAVERSE_CURRENT);
      }

      _DtTurnOffHourGlass (find_rec->shell);
      return(True);
   }
   else
   {
      *ptr2 = '\0';
      command = GrowBuffer (command, &commandLen, (int) strlen (ptr) + 3);

      /* Add buffer to the command string */

      (void) strcat (command, ptr);
      (void) strcat (command, " ");
      *ptr2 = ',';
      *ptr2++;
      ptr = ptr2;
   }

   /* Add the redirector for stderr, so it is disabled */

   command = GrowBuffer (command, &commandLen, (int) strlen (REDIRECTOR));
   (void) strcat (command, REDIRECTOR);


   /* Start the 'grep' process */

   if ((find_rec->popenId = popen (command, "r")) == NULL)
   {
      XtFree ((char *) command);
      return (False);
   }


   /* Set up the alternate input source handler */

   find_rec->alternateInputId =
      XtAddInput (fileno (find_rec->popenId), (XtPointer)XtInputReadMask,
                  (XtInputCallbackProc)AlternateInputHandler3, find_rec);

   /* printf ("%s\n", command); */

   XtFree ((char *) command);

   if(ptr == NULL)
   {
      XtFree(buffer);
      buffer = NULL;
      ptr = NULL;
   }
   return (True);
}



/************************************************************************
 *
 *  AlternateInputHandler()
 *	When a 'find' operation is taking place, this function will be
 *      invoked whenever the 'find' process has some data to send to us.
 *      The function will extract a single line from the pipe, and then
 *      add it to the list of matches, if it matches the selected file
 *      types.
 *
 ************************************************************************/

static void
AlternateInputHandler(
        XtPointer client_data,
        int *source,
        XtInputId *id )
{
   static int bufSize = 0;
   static char * buf = NULL;
   FindRec * find_rec = (FindRec *) client_data;
   FileMgrData *file_mgr_data;
   DialogData * dialog_data;
   int offset = 0;
   char next;
   XmString string;
   int count;
   Arg args[1];
   char * findptr;
   char * end;
   struct stat stat_data;
   int item_count;
   char * title;
   char * msg;
   char * file_type;

   /* Abort if the pipe has already been closed */

   if (find_rec->popenId == NULL) {
      XtSetArg (args[0], XmNdefaultButton, find_rec->start);
      XtSetValues (find_rec->form, args, 1);
      _DtTurnOffHourGlass (find_rec->shell);
      return;
   }


   /* Allocate some buffer, if this is the first time here */

   if (bufSize == 0)
   {
      bufSize = 512;
      buf = XtMalloc (bufSize);
   }

   dialog_data=_DtGetInstanceData((XtPointer)find_rec->fileMgrRec);
   file_mgr_data = (FileMgrData *) dialog_data->data;

   /* Extract the next line, upto a NewLine or EOF */

   while (1)
   {
      while ((offset < bufSize - 1) &&
          ((count = fread (&next, sizeof (char), 1, find_rec->popenId)) == 1) &&
          (next != '\n'))
      {
          buf[offset++] = next;
      }


      /* See if we broke out because the buffer needs to grow */

      if (offset >= bufSize)
      {
         bufSize += 512;
         buf = XtRealloc (buf, bufSize);
         continue;
      }


      /* Save the string we just extracted */

      if (offset > 0)
      {
         buf[offset] = NULL;
         buf = (String) DtEliminateDots (buf);

         if ((stat (buf, &stat_data) == 0) ||
             (lstat (buf, &stat_data) == 0))
         {
            findptr = buf;

            /* Strip out any invisible files */
            if (findptr)
            {
               file_type = (char *) DtDtsDataToDataType(findptr, NULL, 0, NULL,
                                                        NULL, NULL, NULL);
               if (_DtCheckForDataTypeProperty(file_type, "invisible"))
                  findptr = NULL;
            }

            /*  Add string to the scrolled list of matches  */
            /*  Add to the scrolled list of matches         */

            if (findptr &&
                    strncmp(desktop_dir, findptr, strlen(desktop_dir)) != 0)
            {
               if(file_mgr_data->restricted_directory != NULL)
                  string = XmStringCreateLocalized (findptr +
                         strlen(file_mgr_data->restricted_directory));
               else
                  string =
                         XmStringCreateLocalized (findptr);
               XmListAddItemUnselected (find_rec->matchList, string, 0);
               XmStringFree (string);
            }
         }
      }

      if (count == 0)
      {
         /* EOF; command is complete */
         /* Clean up */

         (void) fclose (find_rec->popenId);
         find_rec->popenId = NULL;
         XtRemoveInput (find_rec->alternateInputId);
         XtRemoveEventHandler (find_rec->stop, LeaveWindowMask, FALSE, (XtEventHandler)LeaveStopBttn, find_rec);
         XtRemoveEventHandler (find_rec->stop, EnterWindowMask, FALSE, (XtEventHandler)EnterStopBttn, find_rec);
         find_rec->alternateInputId = 0;

         find_rec->searchInProgress = False;

         /* Reset button sensitivity */

         XtSetSensitive (find_rec->close, True);
         XtSetSensitive (find_rec->start, True);
         XtSetSensitive (find_rec->stop, False);

         XtSetArg (args[0], XmNitemCount, &item_count);
         XtGetValues (find_rec->matchList, args, 1);

         XtSetArg (args[0], XmNdefaultButton, find_rec->start);
         XtSetValues (find_rec->form, args, 1);

         if (item_count == 0)
         {
            char * tmpStr;

            tmpStr = GetSharedMessage(FIND_ERROR_TITLE);
            title = XtNewString(tmpStr);
            tmpStr = GetSharedMessage(NO_FILES_FOUND_ERROR);
            msg = XtNewString(tmpStr);
            _DtMessage (find_rec->shell, title, msg, NULL, HelpRequestCB);
            XtFree(title);
            XtFree(msg);
         }
         else
         {
            XmListSelectPos(find_rec->matchList, 1, True);
            XmProcessTraversal(find_rec->matchList, XmTRAVERSE_CURRENT);
         }

         _DtTurnOffHourGlass (find_rec->shell);
      }
      return;
   }
}

static void
AlternateInputHandler2(
        XtPointer client_data,
        int *source,
        XtInputId *id )
{
   static int bufSize = 0;
   static char * buf = NULL;
   FindRec * find_rec = (FindRec *) client_data;
   int offset = 0;
   char next;
   int count;
   char * findptr;
   struct stat stat_data;
   char * content;

   content = XmTextFieldGetString (find_rec->fileNameFilter);
   if(strcmp(content, "") == 0)
      content = XtNewString("*");

  /* Abort if the pipe has already been closed */

   if (find_rec->popenId == NULL) {
      Arg args[1];

      _DtTurnOffHourGlass (find_rec->shell);
      XtSetArg (args[0], XmNdefaultButton, find_rec->start);
      XtSetValues (find_rec->form, args, 1);

      return;
   }

   /* Allocate some buffer, if this is the first time here */

   if (bufSize == 0)
   {
      bufSize = 512;
      buf = XtMalloc (bufSize);
   }


   /* Extract the next line, upto a NewLine or EOF */

   while (1)
   {
      while ((offset < bufSize - 1) &&
          ((count = fread (&next, sizeof (char), 1, find_rec->popenId)) == 1) &&
          (next != '\n'))
      {
          buf[offset++] = next;
      }


      /* See if we broke out because the buffer needs to grow */

      if (offset >= bufSize)
      {
         bufSize += 512;
         buf = XtRealloc (buf, bufSize);
         continue;
     }


      /* Save the string we just extracted */

      if (offset > 0)
      {
         buf[offset] = NULL;
         buf = (String) DtEliminateDots (buf);

         if ((stat (buf, &stat_data) == 0) ||
             (lstat (buf, &stat_data) == 0))
         {
            findptr = buf;

            if (findptr)
            {
               /* save it with the content, */
               if(buffer == NULL)
               {
                  buffer = (char *)XtMalloc(strlen(findptr) + strlen(content) + 3);
                  strcpy(buffer, findptr);
               }
               else
               {
                  int size;
                  size = strlen(findptr) + strlen(buffer) + strlen(content) +4;
                  buffer = (char *)XtRealloc(buffer, size);
                  strcat(buffer, findptr);
               }
               strcat(buffer, "/");
               strcat(buffer, content);
               strcat(buffer, ",");
            }
         }
      }

      if (count == 0)
      {
         /* EOF; command is complete */
         /* Clean up */

         (void) fclose (find_rec->popenId);
         find_rec->popenId = NULL;
         XtRemoveInput (find_rec->alternateInputId);
         XtRemoveEventHandler (find_rec->stop, LeaveWindowMask, FALSE, (XtEventHandler)LeaveStopBttn, find_rec);
         XtRemoveEventHandler (find_rec->stop, EnterWindowMask, FALSE, (XtEventHandler)EnterStopBttn, find_rec);
         find_rec->alternateInputId = 0;

         ExecuteGrep(find_rec);
      }

      XtFree(content);
      return;
   }
}


static void
AlternateInputHandler3(
        XtPointer client_data,
        int *source,
        XtInputId *id )
{
   static int bufSize = 0;
   static char * buf = NULL;
   FindRec * find_rec = (FindRec *) client_data;
   FileMgrData *file_mgr_data;
   DialogData * dialog_data;
   int offset = 0;
   char next;
   XmString string;
   int count;
   Arg args[1];
   char * findptr;
   struct stat stat_data;
   int item_count;
   char * title;
   char * msg;

  /* Abort if the pipe has already been closed */

   if (find_rec->popenId == NULL) {
      _DtTurnOffHourGlass (find_rec->shell);

      XtSetArg (args[0], XmNdefaultButton, find_rec->start);
      XtSetValues (find_rec->form, args, 1);
      return;
   }

   /* Allocate some buffer, if this is the first time here */

   if (bufSize == 0)
   {
      bufSize = 512;
      buf = XtMalloc (bufSize);
   }

   dialog_data=_DtGetInstanceData((XtPointer)find_rec->fileMgrRec);
   file_mgr_data = (FileMgrData *) dialog_data->data;

   /* Extract the next line, upto a NewLine or EOF */
   while (1)
   {
      while ((offset < bufSize - 1) &&
          ((count = fread (&next, sizeof (char), 1, find_rec->popenId)) == 1) &&
          (next != '\n'))
      {
          buf[offset++] = next;
      }


      /* See if we broke out because the buffer needs to grow */

      if (offset >= bufSize)
      {
         bufSize += 512;
         buf = XtRealloc (buf, bufSize);
         continue;
     }


      /* Save the string we just extracted */

      if (offset > 0)
      {
         buf[offset] = NULL;
         buf = (String) DtEliminateDots (buf);

         if ((stat (buf, &stat_data) == 0) ||
             (lstat (buf, &stat_data) == 0))
         {
            findptr = buf;

            if (findptr)
            {
               /* Make sure its nots a directory */
               if(!((stat_data.st_mode & S_IFMT) == S_IFDIR) &&
                     strncmp(desktop_dir, findptr, strlen(desktop_dir)) != 0)
               {
                  if(file_mgr_data->restricted_directory != NULL)
                     string = XmStringCreateLocalized (findptr +
                            strlen(file_mgr_data->restricted_directory));
                  else
                     string =
                        XmStringCreateLocalized (findptr);
                  XmListAddItemUnselected (find_rec->matchList, string, 0);
                  XmStringFree (string);
               }
            }
         }
      }

      if (count == 0)
      {
         /* EOF; command is complete */
         /* Clean up */

         (void) fclose (find_rec->popenId);
         find_rec->popenId = NULL;
         XtRemoveInput (find_rec->alternateInputId);
         XtRemoveEventHandler (find_rec->stop, LeaveWindowMask, FALSE, (XtEventHandler)LeaveStopBttn, find_rec);
         XtRemoveEventHandler (find_rec->stop, EnterWindowMask, FALSE, (XtEventHandler)EnterStopBttn, find_rec);
         find_rec->alternateInputId = 0;

         if(buffer != NULL)
            ExecuteGrep(find_rec);
         else
         {
            find_rec->searchInProgress = False;

            /* Reset button sensitivity */

            XtSetSensitive (find_rec->close, True);
            XtSetSensitive (find_rec->start, True);
            XtSetSensitive (find_rec->stop, False);

            XtSetArg (args[0], XmNitemCount, &item_count);
            XtGetValues (find_rec->matchList, args, 1);
            XtSetArg (args[0], XmNdefaultButton, find_rec->start);
            XtSetValues (find_rec->form, args, 1);

            if (item_count == 0)
            {
               char * tmpStr;

               tmpStr = GetSharedMessage(FIND_ERROR_TITLE);
               title = XtNewString(tmpStr);
               tmpStr = GetSharedMessage(NO_FILES_FOUND_ERROR);
               msg = XtNewString(tmpStr);
               _DtMessage (find_rec->shell, title, msg, NULL, HelpRequestCB);
               XtFree(title);
               XtFree(msg);
            }
            else
            {
               XmListSelectPos(find_rec->matchList, 1, True);
               XmProcessTraversal(find_rec->matchList, XmTRAVERSE_CURRENT);
            }

            _DtTurnOffHourGlass (find_rec->shell);
         }
      }

      return;
   }
}

/************************************************************************
 *
 *  GetFileName
 *	Get host/dir for selected match item
 *
 ************************************************************************/

static void
GetFileName(
        Widget list,
        int selectedItem,
        String *host,
        String *path,
        FileMgrData *file_mgr_data)
{
   Arg args[2];
   int count;
   XmString * items;
   String temp;
   char * tmpptr;

   XtSetArg (args[0], XmNitemCount, &count);
   XtSetArg (args[1], XmNitems, &items);
   XtGetValues (list, args, 2);

   temp =  (char *) _XmStringUngenerate(items[selectedItem],
                                        XmFONTLIST_DEFAULT_TAG,
                                        XmCHARSET_TEXT, XmCHARSET_TEXT);
   *host = NULL;
   *path = temp;
   if(file_mgr_data->restricted_directory != NULL)
   {
      tmpptr = (char *)XtMalloc(
                      strlen(file_mgr_data->restricted_directory) +
                                                     strlen(*path) + 1);
      strcpy(tmpptr, file_mgr_data->restricted_directory);
      strcat(tmpptr, *path);
      if(*host == NULL)
         XtFree(*path);
      *path = (String)tmpptr;
   }
   else if (*host != NULL)
   {
      *path = XtNewString(*path);
   }
}




/************************************************************************
 *
 *  ExtractDirectory
 *	Check if the path specifies a directory or a file.  If its a
 *      file, then extract out the directory portion, since thats all
 *      we want.
 *
 ************************************************************************/

static Boolean
ExtractDirectory(
        String host,
        String path,
        char **file_name )
{
   String realPath;
   String findptr;
   Tt_status tt_status;
   struct stat statData;

   realPath = (String) ResolveLocalPathName (host, path, NULL, home_host_name, &tt_status);
   if( TT_OK != tt_status )
     return( False );
   else
   {
     if( stat (realPath, &statData ) != 0 )
     {
       if( lstat (realPath, &statData) != 0 )
       {
         XtFree(realPath);
         return(False);
       }
     }
   }
   XtFree(realPath);

   /* Path already is a directory */

   if ((statData.st_mode & S_IFMT) == S_IFDIR)
   {
      *file_name = NULL;
      return (True);
   }

   if(findptr = strrchr (path, '/'))
   {
     *findptr = '\0';
     findptr++;
     *file_name = findptr;
     return (True);
   }
   else
   {
      *file_name = NULL;
      return (True);
   }
}




/************************************************************************
 *
 *  NewView
 *	Do the real work of viewing a directory.
 *
 ************************************************************************/

static void
NewView(
        Widget widget,
        XtPointer client_data,
        XtPointer call_data )
{
   FindRec       *find_rec;
   FileMgrData   *file_mgr_data;
   DialogData    *file_mgr_dialog_data;
   DialogData    *dialog_data;
   DirectorySet  *directory_data;
   FileViewData  *file_view_data;
   char          *file_name;
   String        path, host;
   int           j;

   find_rec = (FindRec *)client_data;
   file_mgr_dialog_data =
      (DialogData *) _DtGetInstanceData ((XtPointer) find_rec->fileMgrRec);
   file_mgr_data = (FileMgrData *) (file_mgr_dialog_data->data);

   GetFileName (find_rec->matchList, find_rec->selectedItem, &host, &path,
                                                               file_mgr_data);

   dialog_data = NULL;
   if (host == NULL)
   {
      /* No host specified, use the one associated with the view */

      if (ExtractDirectory (file_mgr_data->host, path, &file_name))
      {
         initiating_view = (XtPointer) file_mgr_data;
         if(file_mgr_data->restricted_directory == NULL)
            dialog_data = GetNewView (file_mgr_data->host, path, NULL, NULL, 0);
         else
         {
            special_view = True;
            special_treeType = file_mgr_data->show_type;
            special_viewType = file_mgr_data->view;
            special_orderType = file_mgr_data->order;
            special_directionType = file_mgr_data->direction;
            special_randomType = file_mgr_data->positionEnabled;
            special_restricted =
                   XtNewString(file_mgr_data->restricted_directory);
            if(file_mgr_data->title == NULL)
               special_title = NULL;
            else
               special_title = XtNewString(file_mgr_data->title);
            special_helpVol = XtNewString(file_mgr_data->helpVol);
            if(file_mgr_data->toolbox)
               dialog_data = GetNewView (file_mgr_data->host, path,
                              file_mgr_data->restricted_directory, NULL, 0);
            else
               dialog_data = GetNewView (file_mgr_data->host,
					 path, NULL, NULL, 0);
         }
      }
      else
        InvalidFindMessage (find_rec, NO_EXISTANCE, NULL);
   }
   else
   {
      if (ExtractDirectory (host, path, &file_name))
      {
         initiating_view = (XtPointer) file_mgr_data;
         if(file_mgr_data->restricted_directory == NULL)
            dialog_data = GetNewView (host, path, NULL, NULL, 0);
         else
         {
            special_view = True;
            special_treeType = file_mgr_data->show_type;
            special_viewType = file_mgr_data->view;
            special_orderType = file_mgr_data->order;
            special_directionType = file_mgr_data->direction;
            special_randomType = file_mgr_data->positionEnabled;
            special_restricted =
                  XtNewString(file_mgr_data->restricted_directory);
            if(file_mgr_data->title == NULL)
               special_title = NULL;
            else
               special_title = XtNewString(file_mgr_data->title);
            special_helpVol = XtNewString(file_mgr_data->helpVol);
            if(file_mgr_data->toolbox)
               dialog_data = GetNewView (file_mgr_data->host, path,
                               file_mgr_data->restricted_directory, NULL, 0);
            else
               dialog_data = GetNewView (file_mgr_data->host,
					 path, NULL, NULL, 0);
         }
      }
      else
        InvalidFindMessage  (find_rec, NO_EXISTANCE, NULL);
   }

   if(dialog_data != NULL)
   {
      file_mgr_data = (FileMgrData *) dialog_data->data;
      directory_data = file_mgr_data->directory_set[0];

      for (j = 0; j < directory_data->file_count; j++)
      {
         file_view_data = directory_data->file_view_data[j];

         if (file_view_data->filtered != True &&
            strcmp(file_name, file_view_data->file_data->file_name) == 0)
            {
               SelectFile (file_mgr_data, file_view_data);
               PositionFileView(file_view_data, file_mgr_data);
               break;
            }
      }
      if (!directory_data->file_count)
         file_mgr_data->desktop_file = XtNewString(file_name);
      else
         file_mgr_data->desktop_file = NULL;
   }
   if(file_mgr_data->selection_list[0] != NULL)
      ActivateSingleSelect ((FileMgrRec *) file_mgr_data->file_mgr_rec,
                    file_mgr_data->selection_list[0]->file_data->logical_type);

   XtFree ((char *) host);
   XtFree ((char *) path);
}



/************************************************************************
 *
 *  FindPutOnDesktop
 *      Put the found file/directory on the Desktop
 *
 ************************************************************************/

static void
FindPutOnDesktop(
        Widget widget,
        XtPointer client_data,
        XtPointer call_data )
{
   FindRec *find_rec = (FindRec *)client_data;
   FileMgrData   *file_mgr_data;
   DialogData    *file_mgr_dialog_data;
   char          *file_name;
   char          *fileptr;
   String        path, host;
   Boolean       result;
   int EndIndex  = desktop_data->numIconsUsed;

   file_mgr_dialog_data =
      (DialogData *) _DtGetInstanceData ((XtPointer) find_rec->fileMgrRec);
   file_mgr_data = (FileMgrData *) (file_mgr_dialog_data->data);

   GetFileName (find_rec->matchList, find_rec->selectedItem, &host, &path,
                                                               file_mgr_data);

   if (file_mgr_data->toolbox)
      path = _DtResolveAppManPath(path, file_mgr_data->restricted_directory);

   if( host == NULL)
       result =  ExtractDirectory (file_mgr_data->host, path, &file_name);
   else
       result = ExtractDirectory (host, path, &file_name);

   if(!result)
   {
      InvalidFindMessage (find_rec, NO_EXISTANCE, NULL);
      XtFree((char *)path);
      XtFree((char *)host);
      return;
   }

   if(file_name == NULL)
   {
      fileptr = strrchr (path, '/');
      *fileptr = '\0';
      file_name = fileptr + 1;
   }

   if( host == NULL)
   {
      SetupDesktopWindow (XtDisplay(widget), NULL,
                          (FileMgrRec *)file_mgr_data->file_mgr_rec,
                          file_name, file_mgr_data->host, path, -1, -1,
                          file_mgr_data->restricted_directory,EndIndex);
   }
   else
   {
      SetupDesktopWindow (XtDisplay(widget), NULL,
                          (FileMgrRec *)file_mgr_data->file_mgr_rec,
                          file_name, host, path, -1, -1,
                          file_mgr_data->restricted_directory,EndIndex);
   }
   XtFree((char *)path);
   XtFree((char *)host);
}

/************************************************************************
 *
 *  SetActiveItem()
 *	Saves the index of the selected item
 *
 ************************************************************************/

static void
SetActiveItem(
        Widget widget,
        XtPointer client_data,
        XtPointer call_data )
{
   FindRec * find_rec = (FindRec *) client_data;
   XmListCallbackStruct * cb = (XmListCallbackStruct *) call_data;

   find_rec->selectedItem = cb->item_position - 1;

   XtSetSensitive (find_rec->newFM, True);
   XtSetSensitive (find_rec->putOnDT, True);
}


static void
SetFocus(
        FindRec *find_rec,
        FindData *find_data )
{
   /* Force the focus to the text field */
   XmProcessTraversal(find_rec->fileNameFilter, XmTRAVERSE_CURRENT);
}

FILE *
findpopen(cmd,mode,childpid)
   char *cmd;
   char *mode;
   int *childpid;
{
   static char *pname = "findpopen";
   int     fd[2];
   register int parentside, childside;

   if(pipe(fd) < 0)
        return(NULL);
   parentside = (mode[0] == 'r')? fd[0]:fd[1];
   childside = (mode[0] == 'r')? fd[1]:fd[0];
   if((*childpid = fork()) == 0)
   {
      int     read_or_write;

      DBGFORK(("%s:  child forked, pipe %d\n", pname, childside));

      /* Child has to select the stdin or stdout based on the mode */
      read_or_write = (mode[0] == 'r')? 1:0;
      (void) close(parentside);
      /* Dup the stdin or stdout based on the mode */
      if ( read_or_write != childside )  /* If what we got is already stdin */
      {                                  /* or stdout then no need to close */
         (void) close(read_or_write);
         (void) fcntl(childside, F_DUPFD, read_or_write);
         (void) close(childside);        /* Save a file descriptor */
      }
      (void) execl(KORNSHELL, "ksh", "-c", cmd, (char *)0);
     /* Need to process the error return */

      DBGFORK(("%s:  child exiting\n", pname));
      exit(1);
   }

   if(*childpid == -1)
      return(NULL);

   DBGFORK(("%s:  forked child<%d>, pipe %d\n", pname, childpid, parentside));

   (void) close(childside);  /* We don't need child side, so close it */
   return(fdopen(parentside, mode));
}
