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
/* $XConsortium: Prefs.c /main/7 1996/10/23 17:04:25 mustafa $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 *   FILE:           Prefs.c
 *
 *   COMPONENT_NAME: Desktop File Manager (dtfile)
 *
 *   Description:    Source file for the change directory dialog.
 *
 *   FUNCTIONS: ApplyCallback
 *		Create
 *		Destroy
 *		DirectionToString
 *		FreeValues
 *		GetDefaultValues
 *		GetPreferencesValues
 *		GetResourceValues
 *		GetValues
 *		InstallChange
 *		InstallClose
 *		OkCallback
 *		OrderToString
 *		RandomToString
 *		ResetCallback
 *		SetValues
 *		ShowTypeToString
 *		StringToDirection
 *		StringToOrder
 *		StringToRandom
 *		StringToShowType
 *		StringToTreeFiles
 *		StringToView
 *		ToggleCallback
 *		TreeFilesToString
 *		ViewToString
 *		WriteResourceValues
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *
 ****************************************************************************
 ************************************<+>*************************************/


#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


#include <Xm/XmP.h>
#include <Xm/Xm.h>
#include <Xm/DialogS.h>
#include <Xm/MessageB.h>
#include <Xm/BulletinB.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/LabelG.h>
#include <Xm/PushBG.h>
#include <Xm/ToggleBG.h>
#include <Xm/SeparatoG.h>
#include <Xm/VendorSEP.h>
#include <Xm/MwmUtil.h>
#include <Xm/Protocols.h>

#include <Dt/TitleBox.h>

#include <Dt/DtNlUtils.h>
#include "Encaps.h"
#include "SharedProcs.h"

#include <X11/ShellP.h>
#include <X11/Shell.h>
#include <X11/Xatom.h>

#include "FileMgr.h"
#include "Desktop.h"
#include "Main.h"
#include "Prefs.h"
#include "Help.h"

/********    Static Function Declarations    ********/

static void Create(
                        Display *display,
                        Widget parent,
                        Widget *return_widget,
                        XtPointer *dialog) ;
static void InstallChange(
                        PreferencesRec *preferences_rec,
                        XtCallbackProc callback,
                        XtPointer client_data) ;
static void InstallClose(
                        PreferencesRec *preferences_rec,
                        XtCallbackProc callback,
                        XtPointer client_data) ;
static void Destroy(
                        PreferencesRec *preferences_rec) ;
static XtPointer GetValues(
                        PreferencesRec *preferences_rec) ;
static XtPointer GetDefaultValues( void ) ;
static XtPointer GetResourceValues(
                        XrmDatabase data_base,
                        char **name_list) ;
static void SetValues(
                        PreferencesRec *preferences_rec,
                        PreferencesData *preferences_data) ;
static void WriteResourceValues(
                        DialogData *values,
                        int fd,
                        char **name_list) ;
static void FreeValues(
                        PreferencesData *preferences_data) ;
static void ResetCallback(
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void ToggleCallback(
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void OkCallback(
                        Widget w,
                        PreferencesApply *apply_data,
                        XtPointer call_data) ;
static void ApplyCallback(
                        Widget w,
                        PreferencesApply *apply_data,
                        XtPointer call_data) ;
static void GetPreferencesValues(
                        PreferencesRec *preferences_rec,
                        PreferencesData *preferences_data) ;

/********    End Static Function Declarations    ********/


static char * Order_by_file_type = "order_by_file_type";
static char * Order_by_alphabetical = "order_by_alphabetical";
static char * Order_by_date = "order_by_date";
static char * Order_by_size = "order_by_size";
static char * By_name = "by_name";
static char * By_name_and_icon = "by_name_and_icon";
static char * By_name_and_small_icon = "by_name_and_small_icon";
static char * By_attributes = "by_attributes";
static char * Direction_descending = "direction_descending";
static char * Direction_ascending = "direction_ascending";
static char * Single_directory = "single_directory";
static char * DirectoryTree = "directory_tree";
static char * Tree_files_never = "never";
static char * Tree_files_choose = "choose";
static char * Tree_files_always = "always";
static char * PreferencesName = "Preferences";
static char * Random_on = "random_on";
static char * Random_off = "random_off";


/*  Resource definitions for the preferences dialog  */

static DialogResource resources[] =
{
   { "showType", SHOW_TYPE, sizeof(unsigned char),
     XtOffset(PreferencesDataPtr, show_type),
     (XtPointer) SINGLE_DIRECTORY, ShowTypeToString },

   { "treeFiles", TREE_FILES, sizeof(unsigned char),
     XtOffset(PreferencesDataPtr, tree_files),
     (XtPointer) TREE_FILES_NEVER, TreeFilesToString },

   { "view_single", VIEW, sizeof(unsigned char),
     XtOffset(PreferencesDataPtr, view_single),
     (XtPointer) BY_NAME_AND_ICON, ViewToString },

   { "view_tree", VIEW, sizeof(unsigned char),
     XtOffset(PreferencesDataPtr, view_tree),
     (XtPointer) BY_NAME_AND_SMALL_ICON, ViewToString },

   { "order", ORDER, sizeof(unsigned char), 
     XtOffset(PreferencesDataPtr, order),
     (XtPointer) ORDER_BY_ALPHABETICAL, OrderToString },

   { "direction", DIRECTION_RESRC, sizeof(unsigned char), 
     XtOffset(PreferencesDataPtr, direction),
     (XtPointer) DIRECTION_ASCENDING, DirectionToString },

   { "positionEnabled", RANDOM, sizeof(unsigned char), 
     XtOffset(PreferencesDataPtr, positionEnabled),
     (XtPointer) RANDOM_OFF, RandomToString },
};

/*  Callback function definitions  */

/*
 *  The Dialog Class structure.
 */
static DialogClass preferencesClassRec =
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

DialogClass * preferencesClass = (DialogClass *) &preferencesClassRec;

int showType;


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
   PreferencesRec * preferences_rec;
   Widget shell;
   Widget form;
   Widget show_frame;
   Widget view_frame;
   Widget order_frame;
   Widget direction_frame;
   Widget group_form;
   Widget separator;
   Widget temp;
   Widget headers_frame;
   Widget position_frame;
   Widget titleWidget;
   XmString label_string;
   Arg args[15];
   Dimension offset, screenHeight;
   int n;
   char * tmpStr;

   /*  Allocate the change directory dialog instance record.  */

   preferences_rec = (PreferencesRec *) XtMalloc (sizeof (PreferencesRec));

   screenHeight = HeightOfScreen( XtScreen( parent ) );
   /* We are supporting 3 sizes. They are: 1200, 1024, 480
      For the sake of simplicity, process two of them.
   */
   if( screenHeight >= (Dimension) 1024 )
     offset = (Dimension)5;
   else
     offset = (Dimension)1;

   /*  Create the shell, bulletin board and form used for the dialog.  */

   n = 0;
   XtSetArg (args[n], XmNmwmFunctions, MWM_FUNC_MOVE
             | MWM_FUNC_CLOSE);                                 ++n;
   XtSetArg (args[n], XmNmwmDecorations, MWM_DECOR_BORDER
             | MWM_DECOR_TITLE);                                ++n;
   XtSetArg (args[n], XmNallowShellResize, False);              ++n;
   preferences_rec->shell = shell =
                 XmCreateDialogShell (parent, "preferences", args, n);


   /* Set the useAsyncGeo on the shell */
   XtSetArg (args[0], XmNuseAsyncGeometry, True);
   XtSetValues (XtParent(shell), args, 1);

   n = 0;
   XtSetArg (args[n], XmNmarginWidth, 1);                       n++;
   XtSetArg (args[n], XmNmarginHeight, 1);                      n++;
   XtSetArg (args[n], XmNshadowThickness, 1);                   n++;
   XtSetArg (args[n], XmNshadowType, XmSHADOW_OUT);             n++;
   XtSetArg (args[n], XmNautoUnmanage, False);                  n++;
   form = XmCreateForm (shell, "form", args, n);
   XtAddCallback(form, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PREFERENCES_DIALOG_STR);


   /* Create the headers status line */
   label_string = XmStringCreateLocalized ((GETMESSAGE(23,23, "Headers")));
   n = 0;
   XtSetArg (args[n], XmNtitleString, label_string);            n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);         n++;
   XtSetArg (args[n], XmNtopOffset, offset);                    n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);       n++;
   XtSetArg (args[n], XmNrightOffset, offset);                  n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);        n++;
   XtSetArg (args[n], XmNleftOffset, offset);                   n++;
   XtSetArg (args[n], XmNmarginWidth, offset);                  n++;
   XtSetArg (args[n], XmNmarginHeight, offset);                 n++;
   headers_frame = _DtCreateTitleBox(form, "headers_frame", args, n);
   XtManageChild (headers_frame);
   XtAddCallback(headers_frame, XmNhelpCallback,
                 (XtCallbackProc)HelpRequestCB,
                 HELP_PREFERENCES_HEADERS_STR);
   XmStringFree(label_string);

   titleWidget = _DtTitleBoxGetTitleArea(headers_frame);

   XtSetArg (args[0], XmNtraversalOn, False);
   XtSetValues (titleWidget, args, 1);


   group_form = XmCreateForm (headers_frame, "group_form", args, 0);
   XtManageChild (group_form);

   label_string = XmStringCreateLocalized ((GETMESSAGE(23,24, "Iconic Path")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);		n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
   preferences_rec->show_iconic_path = temp =
      XmCreateToggleButtonGadget (group_form, "show_iconic_path", args, n);
   XtManageChild (temp);
   XmStringFree (label_string);
   XtAddCallback (temp, XmNvalueChangedCallback, ToggleCallback,
                  (XtPointer) preferences_rec);
   XtAddCallback(temp, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_PREFERENCES_HEADERS_STR);

   label_string = XmStringCreateLocalized ((GETMESSAGE(23,36, "Text Path")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);                    n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);                 n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET);              n++;
   XtSetArg (args[n], XmNleftWidget, preferences_rec->show_iconic_path);n++;
   XtSetArg (args[n], XmNleftOffset, 10);                               n++;
   preferences_rec->show_current_dir = temp =
      XmCreateToggleButtonGadget (group_form, "show_current_dir", args, n);
   XtManageChild (temp);
   XmStringFree (label_string);
   XtAddCallback (temp, XmNvalueChangedCallback, ToggleCallback, (XtPointer) preferences_rec);
   XtAddCallback(temp, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_PREFERENCES_HEADERS_STR);

   label_string = XmStringCreateLocalized ((GETMESSAGE(23,37, "Message Line")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);                    n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);                 n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET);              n++;
   XtSetArg (args[n], XmNleftWidget, preferences_rec->show_current_dir);n++;
   XtSetArg (args[n], XmNleftOffset, 10);                               n++;
   preferences_rec->show_status_line = temp =
      XmCreateToggleButtonGadget (group_form, "show_status_line", args, n);
   XtManageChild (temp);
   XmStringFree (label_string);
   XtAddCallback (temp, XmNvalueChangedCallback, ToggleCallback, (XtPointer) preferences_rec);
   XtAddCallback(temp, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_PREFERENCES_HEADERS_STR);


   /* Create the positioning status line */
   label_string = XmStringCreateLocalized ((GETMESSAGE(23,18, "Placement")));
   n = 0;
   XtSetArg (args[n], XmNtitleString, label_string);         n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);    n++;
   XtSetArg (args[n], XmNtopWidget, headers_frame);          n++;
   XtSetArg (args[n], XmNtopOffset, offset);                 n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);    n++;
   XtSetArg (args[n], XmNrightOffset, offset);               n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);     n++;
   XtSetArg (args[n], XmNleftOffset, offset);                n++;
   XtSetArg (args[n], XmNmarginWidth, offset);               n++;
   XtSetArg (args[n], XmNmarginHeight, offset);              n++;
   position_frame = _DtCreateTitleBox(form, "position_frame", args, n);
   XtManageChild (position_frame);
   XtAddCallback(position_frame, XmNhelpCallback, 
                 (XtCallbackProc)HelpRequestCB, 
                 HELP_PREFERENCES_PLACEMENT_STR);
   XmStringFree(label_string);

   titleWidget = _DtTitleBoxGetTitleArea(position_frame);

   XtSetArg (args[0], XmNtraversalOn, False);
   XtSetValues (titleWidget, args, 1);


   group_form = XmCreateForm (position_frame, "group_form", args, 0);
   XtManageChild (group_form);

   label_string = XmStringCreateLocalized ((GETMESSAGE(23,20, "As Placed")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);		n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
   XtSetArg (args[n], XmNindicatorType, XmONE_OF_MANY);		n++;
   preferences_rec->random_on = temp =
      XmCreateToggleButtonGadget (group_form, "random_on", args, n);
   XtManageChild (temp);
   XmStringFree (label_string);
   XtAddCallback (temp, XmNvalueChangedCallback, ToggleCallback, 
                  (XtPointer) preferences_rec);
   XtAddCallback(temp, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PREFERENCES_PLACEMENT_STR);

   label_string = XmStringCreateLocalized ((GETMESSAGE(23,35, "Rows and Columns")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);              n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);           n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET);        n++;
   XtSetArg (args[n], XmNleftWidget, preferences_rec->random_on); n++;
   XtSetArg (args[n], XmNleftOffset, 10);                         n++;
   XtSetArg (args[n], XmNindicatorType, XmONE_OF_MANY);           n++;
   preferences_rec->random_off = temp =
      XmCreateToggleButtonGadget (group_form, "random_off", args, n);
   XtManageChild (temp);
   XmStringFree (label_string);
   XtAddCallback (temp, XmNvalueChangedCallback, ToggleCallback, (XtPointer) preferences_rec);
   XtAddCallback(temp, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PREFERENCES_PLACEMENT_STR);


   /*  Create the show container and widgets  */

   label_string = XmStringCreateLocalized (((char *)GETMESSAGE(23, 7, "Show")));
   n = 0;
   XtSetArg (args[n], XmNtitleString, label_string);            n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, position_frame);            n++;
   XtSetArg (args[n], XmNtopOffset, offset);                    n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);        n++;
   XtSetArg (args[n], XmNleftOffset, offset);                   n++;
   XtSetArg (args[n], XmNmarginWidth, offset);                  n++;
   XtSetArg (args[n], XmNmarginHeight, offset);                 n++;
   show_frame = _DtCreateTitleBox (form, "show_frame", args, n);
   XtManageChild (show_frame);
   XmStringFree (label_string);
   XtAddCallback(show_frame, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PREFERENCES_SHOW_STR);

   titleWidget = _DtTitleBoxGetTitleArea(show_frame);

   XtSetArg (args[0], XmNtraversalOn, False);
   XtSetValues (titleWidget, args, 1);

   group_form = XmCreateForm (show_frame, "show_form", args, 0);
   XtManageChild (group_form);

   label_string = XmStringCreateLocalized (((char *)GETMESSAGE(23, 8, "By Single Folder")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);		n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
   XtSetArg (args[n], XmNindicatorType, XmONE_OF_MANY);		n++;
   preferences_rec->show_single = temp =
      XmCreateToggleButtonGadget (group_form, "by_single_directory", args, n);
   XtManageChild (temp);
   XmStringFree (label_string);
   XtAddCallback (temp, XmNvalueChangedCallback, ToggleCallback, 
                  (XtPointer) preferences_rec);
   XtAddCallback(temp, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PREFERENCES_SHOW_STR);


   label_string = XmStringCreateLocalized (((char *)GETMESSAGE(23, 9, "By Tree")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);            n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, temp);                      n++;
   XtSetArg (args[n], XmNtopOffset, offset);                    n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);        n++;
   XtSetArg (args[n], XmNindicatorType, XmONE_OF_MANY);         n++;
   preferences_rec->show_multiple = temp =
      XmCreateToggleButtonGadget (group_form, "by_directory_tree", args, n);
   XtManageChild (temp);
   XmStringFree (label_string);
   XtAddCallback (temp, XmNvalueChangedCallback, ToggleCallback,
                  (XtPointer) preferences_rec);
   XtAddCallback(temp, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_PREFERENCES_SHOW_STR);

   label_string = XmStringCreateLocalized (((char *)GETMESSAGE(23, 27, "Folders only")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);            n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, temp);                      n++;
   XtSetArg (args[n], XmNtopOffset, offset);                    n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);        n++;
   XtSetArg (args[n], XmNleftOffset, 25);                       n++;
   XtSetArg (args[n], XmNindicatorType, XmONE_OF_MANY);         n++;
   preferences_rec->tree_files_never = temp =
      XmCreateToggleButtonGadget (group_form, "tree_files_never", args, n);
   XtManageChild (temp);
   XmStringFree (label_string);
   XtAddCallback (temp, XmNvalueChangedCallback, ToggleCallback,
                  (XtPointer) preferences_rec);
   XtAddCallback(temp, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_PREFERENCES_SHOW_STR);

   label_string = XmStringCreateLocalized (((char *)GETMESSAGE(23, 28, "Folders, then Files")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);            n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, temp);                      n++;
   XtSetArg (args[n], XmNtopOffset, offset);                    n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);        n++;
   XtSetArg (args[n], XmNleftOffset, 25);                       n++;
   XtSetArg (args[n], XmNindicatorType, XmONE_OF_MANY);         n++;
   preferences_rec->tree_files_choose = temp =
      XmCreateToggleButtonGadget (group_form, "tree_files_choose", args, n);
   XtManageChild (temp);
   XmStringFree (label_string);
   XtAddCallback (temp, XmNvalueChangedCallback, ToggleCallback,
                  (XtPointer) preferences_rec);
   XtAddCallback(temp, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_PREFERENCES_SHOW_STR);

   label_string = XmStringCreateLocalized (((char *)GETMESSAGE(23, 29, "Folders and Files")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);            n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, temp);                      n++;
   XtSetArg (args[n], XmNtopOffset, offset);                    n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);        n++;
   XtSetArg (args[n], XmNleftOffset, 25);                       n++;
   XtSetArg (args[n], XmNindicatorType, XmONE_OF_MANY);         n++;
   preferences_rec->tree_files_always = temp =
      XmCreateToggleButtonGadget (group_form, "tree_files_always", args, n);
   XtManageChild (temp);
   XmStringFree (label_string);
   XtAddCallback (temp, XmNvalueChangedCallback, ToggleCallback,
                  (XtPointer) preferences_rec);
   XtAddCallback(temp, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PREFERENCES_SHOW_STR);


   /*  Create the view container and widgets  */

   label_string = XmStringCreateLocalized (((char *)GETMESSAGE(23,31, "Representation")));
   n = 0;
   XtSetArg (args[n], XmNtitleString, label_string);            n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, position_frame);            n++;
   XtSetArg (args[n], XmNtopOffset, offset);                    n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET);      n++;
   XtSetArg (args[n], XmNleftOffset, 10);                       n++;
   XtSetArg (args[n], XmNleftWidget, show_frame);               n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);       n++;
   XtSetArg (args[n], XmNrightOffset, offset);                  n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET);	n++;
   XtSetArg (args[n], XmNbottomWidget, show_frame);             n++;
   XtSetArg (args[n], XmNmarginWidth, offset);                  n++;
   XtSetArg (args[n], XmNmarginHeight, offset);                 n++;
   view_frame = _DtCreateTitleBox(form, "view_frame", args, n);
   XtManageChild (view_frame);
   XmStringFree (label_string);
   XtAddCallback(view_frame, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_PREFERENCES_VIEW_STR);

   titleWidget = _DtTitleBoxGetTitleArea(view_frame);

   XtSetArg (args[0], XmNtraversalOn, False);
   XtSetValues (titleWidget, args, 1);

   group_form = XmCreateForm (view_frame, "view_form", args, 0);
   XtManageChild (group_form);

   label_string = XmStringCreateLocalized (((char *)GETMESSAGE(23,32, "By Name Only")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);		n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
   XtSetArg (args[n], XmNindicatorType, XmONE_OF_MANY);		n++;
   preferences_rec->by_name = temp =
      XmCreateToggleButtonGadget (group_form, "by_name", args, n);
   XtManageChild (temp);
   XmStringFree (label_string);
   XtAddCallback (temp, XmNvalueChangedCallback, ToggleCallback,
                  (XtPointer) preferences_rec);
   XtAddCallback(temp, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_PREFERENCES_VIEW_STR);


   label_string = XmStringCreateLocalized (((char *)GETMESSAGE(23,33, "By Large Icons")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);            n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, temp);                      n++;
   XtSetArg (args[n], XmNtopOffset, offset);                    n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);        n++;
   XtSetArg (args[n], XmNindicatorType, XmONE_OF_MANY);         n++;
   preferences_rec->by_name_and_icon = temp =
      XmCreateToggleButtonGadget (group_form, "by_name_and_icon", args, n);
   XtManageChild (temp);
   XmStringFree (label_string);
   XtAddCallback (temp, XmNvalueChangedCallback, ToggleCallback,
                  (XtPointer) preferences_rec);
   XtAddCallback(temp, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_PREFERENCES_VIEW_STR);


   label_string = XmStringCreateLocalized (((char *)GETMESSAGE(23,34, "By Small Icons")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);            n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, temp);                      n++;
   XtSetArg (args[n], XmNtopOffset, offset);                    n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);        n++;
   XtSetArg (args[n], XmNindicatorType, XmONE_OF_MANY);         n++;
   preferences_rec->by_name_and_small_icon = temp =
     XmCreateToggleButtonGadget (group_form, "by_name_and_small_icon", args, n);
   XtManageChild (temp);
   XmStringFree (label_string);
   XtAddCallback (temp, XmNvalueChangedCallback, ToggleCallback,
                  (XtPointer) preferences_rec);
   XtAddCallback(temp, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_PREFERENCES_VIEW_STR);



   label_string = XmStringCreateLocalized (((char *)GETMESSAGE(23,6, "By Name, date, size ...")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);            n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, temp);                      n++;
   XtSetArg (args[n], XmNtopOffset, offset);                    n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);        n++;
   XtSetArg (args[n], XmNindicatorType, XmONE_OF_MANY);         n++;
   preferences_rec->by_attributes = temp =
      XmCreateToggleButtonGadget (group_form, "by_attributes", args, n);
   XtManageChild (temp);
   XmStringFree (label_string);
   XtAddCallback (temp, XmNvalueChangedCallback, ToggleCallback,
                  (XtPointer) preferences_rec);
   XtAddCallback(temp, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_PREFERENCES_VIEW_STR);


   /*  Create the file order container and widgets  */

   label_string = XmStringCreateLocalized (((char *)GETMESSAGE(23, 10, "Order")));
   n = 0;
   XtSetArg (args[n], XmNtitleString, label_string);            n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, view_frame);                n++;
   XtSetArg (args[n], XmNtopOffset, offset);                    n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);        n++;
   XtSetArg (args[n], XmNleftOffset, offset);                   n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_OPPOSITE_WIDGET);	n++;
   XtSetArg (args[n], XmNrightWidget, show_frame);              n++;
   XtSetArg (args[n], XmNmarginWidth, offset);                  n++;
   XtSetArg (args[n], XmNmarginHeight, offset);                 n++;
   order_frame = _DtCreateTitleBox (form, "order_frame", args, n);
   XtManageChild (order_frame);
   XmStringFree (label_string);
   XtAddCallback(order_frame, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PREFERENCES_ORDER_STR);

   titleWidget = _DtTitleBoxGetTitleArea(order_frame);

   XtSetArg (args[0], XmNtraversalOn, False);
   XtSetValues (titleWidget, args, 1);

   group_form = XmCreateForm (order_frame, "order_form", args, 0);
   XtManageChild (group_form);

   label_string = XmStringCreateLocalized (((char *)GETMESSAGE(23, 12, "Alphabetically")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);            n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);         n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);        n++;
   XtSetArg (args[n], XmNindicatorType, XmONE_OF_MANY);         n++;
   preferences_rec->order_alphabetical = temp =
      XmCreateToggleButtonGadget (group_form, "alphabetically", args, n);
   XtManageChild (temp);
   XmStringFree (label_string);
   XtAddCallback (temp, XmNvalueChangedCallback, ToggleCallback, 
                  (XtPointer) preferences_rec);
   XtAddCallback(temp, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PREFERENCES_ORDER_STR);


   label_string = XmStringCreateLocalized (((char *)GETMESSAGE(23, 11, "By File Type")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);            n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, temp);                      n++;
   XtSetArg (args[n], XmNtopOffset, offset);                    n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);        n++;
   XtSetArg (args[n], XmNindicatorType, XmONE_OF_MANY);         n++;
   preferences_rec->order_file_type = temp =
      XmCreateToggleButtonGadget (group_form, "by_file_type", args, n);
   XtManageChild (temp);
   XmStringFree (label_string);
   XtAddCallback (temp, XmNvalueChangedCallback, ToggleCallback, 
                  (XtPointer) preferences_rec);
   XtAddCallback(temp, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PREFERENCES_ORDER_STR);


   label_string = XmStringCreateLocalized (((char *)GETMESSAGE(23, 13, "By Date")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);            n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, temp);                      n++;
   XtSetArg (args[n], XmNtopOffset, offset);                    n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);        n++;
   XtSetArg (args[n], XmNindicatorType, XmONE_OF_MANY);         n++;
   preferences_rec->order_date = temp =
      XmCreateToggleButtonGadget (group_form, "by_date", args, n);
   XtManageChild (temp);
   XmStringFree (label_string);
   XtAddCallback (temp, XmNvalueChangedCallback, ToggleCallback, 
                  (XtPointer) preferences_rec);
   XtAddCallback(temp, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PREFERENCES_ORDER_STR);


   label_string = XmStringCreateLocalized (((char *)GETMESSAGE(23, 14, "By Size")));

   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);            n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, temp);                      n++;
   XtSetArg (args[n], XmNtopOffset, offset);                    n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);        n++;
   XtSetArg (args[n], XmNindicatorType, XmONE_OF_MANY);         n++;
   preferences_rec->order_size = temp =
      XmCreateToggleButtonGadget (group_form, "by_size", args, n);
   XtManageChild (temp);
   XmStringFree (label_string);
   XtAddCallback (temp, XmNvalueChangedCallback, ToggleCallback, 
                  (XtPointer) preferences_rec);
   XtAddCallback(temp, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PREFERENCES_ORDER_STR);


   /*  Create the file direction container and widgets  */

   label_string = XmStringCreateLocalized (((char *)GETMESSAGE(23, 15, "Direction")));
   n = 0;
   XtSetArg (args[n], XmNtitleString, label_string);            n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, view_frame);                n++;
   XtSetArg (args[n], XmNtopOffset, offset);                    n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET);	n++;
   XtSetArg (args[n], XmNbottomWidget, order_frame);            n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET);	n++;
   XtSetArg (args[n], XmNleftWidget, view_frame);               n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);       n++;
   XtSetArg (args[n], XmNrightOffset, offset);                  n++;
   XtSetArg (args[n], XmNmarginWidth, offset);                  n++;
   XtSetArg (args[n], XmNmarginHeight, offset);                 n++;
   direction_frame = _DtCreateTitleBox (form, "direction_frame", args, n);
   XtManageChild (direction_frame);
   XmStringFree (label_string);
   XtAddCallback(direction_frame, XmNhelpCallback, 
                 (XtCallbackProc)HelpRequestCB, 
                 HELP_PREFERENCES_DIRECTION_STR);

   titleWidget = _DtTitleBoxGetTitleArea(direction_frame);

   XtSetArg (args[0], XmNtraversalOn, False);
   XtSetValues (titleWidget, args, 1);

   group_form = XmCreateForm (direction_frame, "direction_form", args, 0);
   XtManageChild (group_form);

   label_string = XmStringCreateLocalized (((char *)GETMESSAGE(23, 16, "Ascending")));

   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);		n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
   XtSetArg (args[n], XmNindicatorType, XmONE_OF_MANY);		n++;
   preferences_rec->direction_ascending = temp =
      XmCreateToggleButtonGadget (group_form, "ascending", args, n);
   XtManageChild (temp);
   XmStringFree (label_string);
   XtAddCallback (temp, XmNvalueChangedCallback, ToggleCallback, 
                  (XtPointer) preferences_rec);
   XtAddCallback(temp, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PREFERENCES_DIRECTION_STR);


   label_string = XmStringCreateLocalized (((char *)GETMESSAGE(23, 17, "Descending")));

   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);            n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, temp);                      n++;
   XtSetArg (args[n], XmNtopOffset, offset);                    n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);        n++;
   XtSetArg (args[n], XmNindicatorType, XmONE_OF_MANY);         n++;
   preferences_rec->direction_descending = temp =
      XmCreateToggleButtonGadget (group_form, "descending", args, n);
   XtManageChild (temp);
   XmStringFree (label_string);
   XtAddCallback (temp, XmNvalueChangedCallback, ToggleCallback, 
                  (XtPointer) preferences_rec);
   XtAddCallback(temp, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PREFERENCES_DIRECTION_STR);


   /*  Create a separator between the buttons  */

   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);        n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);       n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, order_frame);               n++;
   XtSetArg (args[n], XmNtopOffset, offset);                    n++;
   separator =  XmCreateSeparatorGadget (form, "separator", args, n);
   XtManageChild (separator);


   /*  Create the ok, apply, reset, cancel and help buttons  */

   n = 0;
   XtSetArg (args[n], XmNlabelString, okXmString);              n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);    n++;
   XtSetArg (args[n], XmNleftPosition, 1);                      n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);   n++;
   XtSetArg (args[n], XmNrightPosition, 19);                    n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, separator);                 n++;
   XtSetArg (args[n], XmNtopOffset, offset);                    n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);      n++;
   XtSetArg (args[n], XmNbottomOffset, offset);                 n++;
   XtSetArg (args[n], XmNmarginWidth, 4);                       n++;
   XtSetArg (args[n], XmNmarginHeight, 4);                      n++;
   preferences_rec->ok = temp =
      XmCreatePushButtonGadget (form, "ok", args, n);
   XtManageChild (temp);
   XtAddCallback(temp, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PREFERENCES_DIALOG_STR);

   n = 0;
   XtSetArg (args[n], XmNlabelString, applyXmString);           n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);    n++;
   XtSetArg (args[n], XmNleftPosition, 21);                     n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);   n++;
   XtSetArg (args[n], XmNrightPosition, 39);                    n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, separator);                 n++;
   XtSetArg (args[n], XmNtopOffset, offset);                    n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);      n++;
   XtSetArg (args[n], XmNbottomOffset, offset);                 n++;
   XtSetArg (args[n], XmNmarginWidth, 4);                       n++;
   XtSetArg (args[n], XmNmarginHeight, 4);                      n++;
   preferences_rec->apply = temp =
      XmCreatePushButtonGadget (form, "apply", args, n);
   XtManageChild (temp);
   XtAddCallback(temp, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PREFERENCES_DIALOG_STR);


   label_string = XmStringCreateLocalized ((GETMESSAGE(23,30, "Defaults")));

   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);            n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);    n++;
   XtSetArg (args[n], XmNleftPosition, 41);                     n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);   n++;
   XtSetArg (args[n], XmNrightPosition, 59);                    n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, separator);                 n++;
   XtSetArg (args[n], XmNtopOffset, offset);                    n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);      n++;
   XtSetArg (args[n], XmNbottomOffset, offset);                 n++;
   XtSetArg (args[n], XmNmarginWidth, 4);                       n++;
   XtSetArg (args[n], XmNmarginHeight, 4);                      n++;
   preferences_rec->reset = temp = 
      XmCreatePushButtonGadget (form, "defaults", args, n);
   XmStringFree (label_string);
   XtManageChild (temp);
   XtAddCallback(temp, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PREFERENCES_DIALOG_STR);
   XtAddCallback (temp, XmNactivateCallback, ResetCallback, 
                  (XtPointer) preferences_rec);


   n = 0;
   XtSetArg (args[n], XmNlabelString, cancelXmString);          n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);    n++;
   XtSetArg (args[n], XmNleftPosition, 61);                     n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);   n++;
   XtSetArg (args[n], XmNrightPosition, 79);                    n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, separator);                 n++;
   XtSetArg (args[n], XmNtopOffset, offset);                    n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);      n++;
   XtSetArg (args[n], XmNbottomOffset, offset);                 n++;
   XtSetArg (args[n], XmNmarginWidth, 4);                       n++;
   XtSetArg (args[n], XmNmarginHeight, 4);                      n++;
   preferences_rec->close = temp =
      XmCreatePushButtonGadget (form, "close", args, n);
   XtManageChild (temp);
   XtAddCallback(temp, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PREFERENCES_DIALOG_STR);


   n = 0;
   XtSetArg (args[n], XmNlabelString, helpXmString);            n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);    n++;
   XtSetArg (args[n], XmNleftPosition, 81);                     n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);   n++;
   XtSetArg (args[n], XmNrightPosition, 99);                    n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, separator);                 n++;
   XtSetArg (args[n], XmNtopOffset, offset);                    n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);      n++;
   XtSetArg (args[n], XmNbottomOffset, offset);                 n++;
   XtSetArg (args[n], XmNmarginWidth, 4);                       n++;
   XtSetArg (args[n], XmNmarginHeight, 4);                      n++;
   preferences_rec->help = temp =
      XmCreatePushButtonGadget (form, "help", args, n);
   XtManageChild (temp);
   XtAddCallback(temp, XmNactivateCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PREFERENCES_DIALOG_STR);
   XtAddCallback(temp, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PREFERENCES_DIALOG_STR);


   XtSetArg (args[0], XmNcancelButton, preferences_rec->close);
   XtSetArg (args[1], XmNdefaultButton, preferences_rec->ok);
   XtSetValues (form, args, 2);


   /*  Set the return values for the dialog widget and dialog instance.  */

   *return_widget = form;
   *dialog = (XtPointer) preferences_rec;
}




/************************************************************************
 *
 *  InstallChange
 *
 ************************************************************************/

static void
InstallChange(
        PreferencesRec *preferences_rec,
        XtCallbackProc callback,
        XtPointer client_data )
{
   PreferencesApply * apply_data;


   /*  Setup the callback data to be sent to the Ok callback.   */
   /*  This contains the encapsulation callback to invoke upon  */
   /*  the data within the dialog being changed.                */

   apply_data = (PreferencesApply *) XtMalloc (sizeof (PreferencesApply));
   apply_data->callback = callback;
   apply_data->client_data = client_data;
   apply_data->preferences_rec = (XtPointer) preferences_rec;
   preferences_rec->apply_data = apply_data;


   /*  Add the callbacks for list item insertion.  */

   XtAddCallback (preferences_rec->apply, XmNactivateCallback, 
                  (XtCallbackProc)ApplyCallback, (XtPointer) apply_data);

   XtAddCallback (preferences_rec->ok, XmNactivateCallback, 
                  (XtCallbackProc)OkCallback, (XtPointer) apply_data);
}




/************************************************************************
 *
 *  InstallClose
 *
 ************************************************************************/

static void
InstallClose(
        PreferencesRec *preferences_rec,
        XtCallbackProc callback,
        XtPointer client_data )
{
   Atom delete_window_atom;

   XtAddCallback (preferences_rec->close,
                  XmNactivateCallback, callback, client_data);

   delete_window_atom = XmInternAtom (XtDisplay(preferences_rec->shell),
                                      "WM_DELETE_WINDOW", True);
   XmRemoveWMProtocols( preferences_rec->shell, &delete_window_atom, 1 );
   XmAddWMProtocolCallback( preferences_rec->shell, delete_window_atom,
                            callback, (XtPointer) client_data );
}




/************************************************************************
 *
 *  Destroy
 *
 ************************************************************************/

static void
Destroy(
        PreferencesRec *preferences_rec )
{
   XtDestroyWidget (preferences_rec->shell);
   XtFree ((char *) preferences_rec->apply_data);
   XtFree ((char *) preferences_rec);
}




/************************************************************************
 *
 *  GetValues
 *
 ************************************************************************/

static XtPointer
GetValues(
        PreferencesRec *preferences_rec )
{
   PreferencesData * preferences_data;
   Arg args[4];


   /*  Allocate and initialize the change dir dialog data.  */

   preferences_data = (PreferencesData *) XtMalloc (sizeof (PreferencesData));

   preferences_data->displayed = True;

   XtSetArg (args[0], XmNx, &preferences_data->x);
   XtSetArg (args[1], XmNy, &preferences_data->y);
   XtSetArg (args[2], XmNwidth, &preferences_data->width);
   XtSetArg (args[3], XmNheight, &preferences_data->height);
   XtGetValues (preferences_rec->shell, args, 4);

   GetPreferencesValues (preferences_rec, preferences_data);

   return ((XtPointer) preferences_data);
}




/************************************************************************
 *
 *  GetDefaultValues
 *
 ************************************************************************/

static XtPointer
GetDefaultValues( void )
{
   PreferencesData * preferences_data;

   /*  Allocate and initialize the default change dir dialog data.  */

   preferences_data = (PreferencesData *) XtMalloc (sizeof (PreferencesData));

   preferences_data->displayed = False;
   preferences_data->x = 0;
   preferences_data->y = 0;
   preferences_data->height = 0;
   preferences_data->width = 0;

   preferences_data->show_type = SINGLE_DIRECTORY;
   preferences_data->tree_files = TREE_FILES_NEVER;
   preferences_data->view_single = BY_NAME_AND_ICON;
   preferences_data->view_tree = BY_NAME_AND_SMALL_ICON;
   preferences_data->order = ORDER_BY_ALPHABETICAL;
   preferences_data->direction = DIRECTION_ASCENDING;
   preferences_data->positionEnabled = RANDOM_OFF;

   return ((XtPointer) preferences_data);
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
   PreferencesData * preferences_data;


   /*  Allocate and get the resources for change dir dialog data.  */

   preferences_data = (PreferencesData *) XtMalloc (sizeof (PreferencesData));

   _DtDialogGetResources (data_base, name_list, PreferencesName, 
                       (char *)preferences_data, resources, 
                       preferencesClass->resource_count);

   return ((XtPointer) preferences_data);
}




/************************************************************************
 *
 *  SetValues
 *
 ************************************************************************/

static void
SetValues(
        PreferencesRec *preferences_rec,
        PreferencesData *preferences_data )
{
   Arg true_args[1];
   Arg false_args[1];
   unsigned char *viewP;


   XtSetArg (true_args[0], XmNset, True);
   XtSetArg (false_args[0], XmNset, False);

   if (preferences_data->show_type == SINGLE_DIRECTORY)
   {
      XtSetValues (preferences_rec->show_single, true_args, 1);
      XtSetValues (preferences_rec->show_multiple, false_args, 1);
      if( trashFileMgrData
          &&(PreferencesData *)trashFileMgrData->preferences->data ==
                                                         preferences_data)
         XtSetSensitive (XtParent (preferences_rec->show_multiple), False);
      else
         XtSetSensitive (XtParent (preferences_rec->show_multiple), True);

      XtSetSensitive (preferences_rec->tree_files_never, False);
      XtSetSensitive (preferences_rec->tree_files_choose, False);
      XtSetSensitive (preferences_rec->tree_files_always, False);
      viewP = &preferences_data->view_single;

      if (*viewP == BY_ATTRIBUTES)
         XtSetSensitive (XtParent (preferences_rec->random_on), False);
      else
         XtSetSensitive (XtParent (preferences_rec->random_on), True);

   }
   else
   {
      XtSetValues (preferences_rec->show_single, false_args, 1);
      XtSetValues (preferences_rec->show_multiple, true_args, 1);
      XtSetSensitive (XtParent (preferences_rec->random_on), False);
      XtSetSensitive (preferences_rec->tree_files_never, True);
      XtSetSensitive (preferences_rec->tree_files_choose, True);
      XtSetSensitive (preferences_rec->tree_files_always, True);
      viewP = &preferences_data->view_tree;
   }

   if (preferences_data->tree_files == TREE_FILES_NEVER)
   {
      XtSetValues (preferences_rec->tree_files_never, true_args, 1);
      XtSetValues (preferences_rec->tree_files_choose, false_args, 1);
      XtSetValues (preferences_rec->tree_files_always, false_args, 1);
   }
   else if (preferences_data->tree_files == TREE_FILES_CHOOSE)
   {
      XtSetValues (preferences_rec->tree_files_never, false_args, 1);
      XtSetValues (preferences_rec->tree_files_choose, true_args, 1);
      XtSetValues (preferences_rec->tree_files_always, false_args, 1);
   }
   else /* preferences_data->tree_files == TREE_FILES_ALWAYS */
   {
      XtSetValues (preferences_rec->tree_files_never, false_args, 1);
      XtSetValues (preferences_rec->tree_files_choose, false_args, 1);
      XtSetValues (preferences_rec->tree_files_always, true_args, 1);
   }

   preferences_rec->view_single = preferences_data->view_single;
   preferences_rec->view_tree = preferences_data->view_tree;

   if (*viewP == BY_NAME)
   {
      XtSetValues (preferences_rec->by_name, true_args, 1);
      XtSetValues (preferences_rec->by_name_and_icon, false_args, 1);
      XtSetValues (preferences_rec->by_name_and_small_icon, false_args, 1);
      XtSetValues (preferences_rec->by_attributes, false_args, 1);
   }
   else if (*viewP == BY_NAME_AND_ICON)
   {
      XtSetValues (preferences_rec->by_name, false_args, 1);
      XtSetValues (preferences_rec->by_name_and_icon, true_args, 1);
      XtSetValues (preferences_rec->by_name_and_small_icon, false_args, 1);
      XtSetValues (preferences_rec->by_attributes, false_args, 1);
   }
   else if (*viewP == BY_NAME_AND_SMALL_ICON)
   {
      XtSetValues (preferences_rec->by_name, false_args, 1);
      XtSetValues (preferences_rec->by_name_and_icon, false_args, 1);
      XtSetValues (preferences_rec->by_name_and_small_icon, true_args, 1);
      XtSetValues (preferences_rec->by_attributes, false_args, 1);
   }
   else if (*viewP == BY_ATTRIBUTES)
   {
      XtSetValues (preferences_rec->by_name, false_args, 1);
      XtSetValues (preferences_rec->by_name_and_icon, false_args, 1);
      XtSetValues (preferences_rec->by_name_and_small_icon, false_args, 1);
      XtSetValues (preferences_rec->by_attributes, true_args, 1);
   }

   if (preferences_data->order == ORDER_BY_FILE_TYPE)
   {
      XtSetValues (preferences_rec->order_file_type, true_args, 1);
      XtSetValues (preferences_rec->order_alphabetical, false_args, 1);
      XtSetValues (preferences_rec->order_date, false_args, 1);
      XtSetValues (preferences_rec->order_size, false_args, 1);
   }
   else if (preferences_data->order == ORDER_BY_ALPHABETICAL)
   {
      XtSetValues (preferences_rec->order_file_type, false_args, 1);
      XtSetValues (preferences_rec->order_alphabetical, true_args, 1);
      XtSetValues (preferences_rec->order_date, false_args, 1);
      XtSetValues (preferences_rec->order_size, false_args, 1);
   }
   else if (preferences_data->order == ORDER_BY_DATE)
   {
      XtSetValues (preferences_rec->order_file_type, false_args, 1);
      XtSetValues (preferences_rec->order_alphabetical, false_args, 1);
      XtSetValues (preferences_rec->order_date, true_args, 1);
      XtSetValues (preferences_rec->order_size, false_args, 1);
   }
   else if (preferences_data->order == ORDER_BY_SIZE)
   {
      XtSetValues (preferences_rec->order_file_type, false_args, 1);
      XtSetValues (preferences_rec->order_alphabetical, false_args, 1);
      XtSetValues (preferences_rec->order_date, false_args, 1);
      XtSetValues (preferences_rec->order_size, true_args, 1);
   }

   if (preferences_data->direction == DIRECTION_ASCENDING)
   {
      XtSetValues (preferences_rec->direction_ascending, true_args, 1);
      XtSetValues (preferences_rec->direction_descending, false_args, 1);
   }
   if (preferences_data->direction == DIRECTION_DESCENDING)
   {
      XtSetValues (preferences_rec->direction_ascending, false_args, 1);
      XtSetValues (preferences_rec->direction_descending, true_args, 1);
   }

   if (preferences_data->positionEnabled == RANDOM_ON)
   {
      XtSetValues (preferences_rec->random_on, true_args, 1);
      XtSetValues (preferences_rec->random_off, false_args, 1);
   }
   if (preferences_data->positionEnabled == RANDOM_OFF)
   {
      XtSetValues (preferences_rec->random_off, true_args, 1);
      XtSetValues (preferences_rec->random_on, false_args, 1);
   }

   if( trashFileMgrData
       &&(PreferencesData *)trashFileMgrData->preferences->data ==
          preferences_data )
   {
     XtSetSensitive( preferences_rec->show_iconic_path, False );
     XtSetSensitive( preferences_rec->show_current_dir, False );
     XtSetSensitive( preferences_rec->show_status_line, False );
     XtSetSensitive( preferences_rec->random_on, False );
   }
   else if( showFilesystem )
   {
     XtSetSensitive( preferences_rec->show_iconic_path, True );
     XtSetSensitive( preferences_rec->show_current_dir, True );
     XtSetSensitive( preferences_rec->show_status_line, True );
     XtSetSensitive( preferences_rec->random_on, True );


     if (preferences_data->show_iconic_path)
       XtSetValues (preferences_rec->show_iconic_path, true_args, 1);
     else
       XtSetValues (preferences_rec->show_iconic_path, false_args, 1);

     if (preferences_data->show_current_dir)
       XtSetValues (preferences_rec->show_current_dir, true_args, 1);
     else
       XtSetValues (preferences_rec->show_current_dir, false_args, 1);

     if (preferences_data->show_status_line)
       XtSetValues (preferences_rec->show_status_line, true_args, 1);
     else
       XtSetValues (preferences_rec->show_status_line, false_args, 1);
   }
   else
   {
     XtSetSensitive( preferences_rec->show_iconic_path, False );
     XtSetSensitive( preferences_rec->show_current_dir, False );
     XtSetSensitive( preferences_rec->show_status_line, False );
     XtSetSensitive( preferences_rec->random_on, True );
   }
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
   PreferencesData * preferences_data = (PreferencesData *) values->data;
   PreferencesRec * preferences_rec;
   Arg args[2];


   /*  If the dialog is currently displayed, update the geometry  */
   /*  fields to their current values.                            */

   if (preferences_data->displayed == True)
   {
      _DtGenericUpdateWindowPosition(values);
      preferences_rec = (PreferencesRec *) _DtGetDialogInstance (values);
   }

   _DtDialogPutResources (fd, name_list, PreferencesName, values->data, 
                       resources, preferencesClass->resource_count);
}




/************************************************************************
 *
 *  FreeValues
 *
 ************************************************************************/

static void
FreeValues(
        PreferencesData *preferences_data )
{
   XtFree ((char *) preferences_data);
}




/************************************************************************
 *
 *  ResetCallback
 *
 ************************************************************************/

static void
ResetCallback(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
   PreferencesRec * preferences_rec = (PreferencesRec *) client_data;
   DialogData * dialog_data;
   PreferencesData * preferences_data;

   dialog_data = _DtGetInstanceData ((XtPointer)preferences_rec);
   preferences_data = (PreferencesData *) dialog_data->data;

   preferences_data->show_type = SINGLE_DIRECTORY;
   preferences_data->tree_files = TREE_FILES_NEVER;
   preferences_data->view_single = BY_NAME_AND_ICON;
   preferences_data->view_tree = BY_NAME_AND_SMALL_ICON;
   preferences_data->order = ORDER_BY_ALPHABETICAL;
   preferences_data->direction = DIRECTION_ASCENDING;
   preferences_data->positionEnabled = RANDOM_OFF;
   preferences_data->show_iconic_path = True;
   preferences_data->show_current_dir = True;
   preferences_data->show_status_line = True;

   /*  Get the current data for the dialog and redisplay.  */

   SetValues  (preferences_rec, (PreferencesData *) dialog_data->data);
}




/************************************************************************
 *
 *  ToggleCallback
 *	Handle the one of many processing for the toggles.
 *
 ************************************************************************/

static void
ToggleCallback(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
   PreferencesRec * preferences_rec;
   DialogData * dialog_data;
   PreferencesData * preferences_data;
   Boolean set;
   unsigned char *viewP;
   Arg args[1];
   Arg false_args[1];
   Arg true_args[1];
   int tmp_view;


   preferences_rec = (PreferencesRec *) client_data;
   dialog_data = _DtGetInstanceData ((XtPointer)preferences_rec);
   preferences_data = (PreferencesData *) dialog_data->data;

   XtSetArg (false_args[0], XmNset, False);
   XtSetArg (true_args[0], XmNset, True);

   XtSetArg (args[0], XmNset, &set);
   XtGetValues (preferences_rec->show_multiple, args, 1);
   if (set)
     viewP = &preferences_rec->view_tree;
   else
     viewP = &preferences_rec->view_single;

   if (w == preferences_rec->show_single)
   {
      XtSetValues (preferences_rec->show_multiple, false_args, 1);
      XtSetValues (preferences_rec->show_single, true_args, 1);

      XtSetSensitive (XtParent (preferences_rec->random_on), True);
      XtSetSensitive (preferences_rec->tree_files_never, False);
      XtSetSensitive (preferences_rec->tree_files_choose, False);
      XtSetSensitive (preferences_rec->tree_files_always, False);

      if (preferences_rec->view_single == BY_NAME)
         w = preferences_rec->by_name;
      else if (preferences_rec->view_single == BY_NAME_AND_ICON)
         w = preferences_rec->by_name_and_icon;
      else if (preferences_rec->view_single == BY_NAME_AND_SMALL_ICON)
         w = preferences_rec->by_name_and_small_icon;
      else
      {
         w = preferences_rec->by_attributes;
         XtSetSensitive (XtParent (preferences_rec->random_on), False);
      }
      showType = SINGLE_DIRECTORY;
      ToggleCallback (w, client_data, call_data);
   }
   else if (w == preferences_rec->show_multiple)
   {
      XtSetValues (preferences_rec->show_single, false_args, 1);
      XtSetValues (preferences_rec->show_multiple, true_args, 1);

      XtSetSensitive (XtParent (preferences_rec->random_on), False);
      XtSetSensitive (preferences_rec->tree_files_never, True);
      XtSetSensitive (preferences_rec->tree_files_choose, True);
      XtSetSensitive (preferences_rec->tree_files_always, True);

      if (preferences_rec->view_tree == BY_NAME)
         w = preferences_rec->by_name;
      else if (preferences_rec->view_tree == BY_NAME_AND_ICON)
         w = preferences_rec->by_name_and_icon;
      else if (preferences_rec->view_tree == BY_NAME_AND_SMALL_ICON)
         w = preferences_rec->by_name_and_small_icon;
      else
         w = preferences_rec->by_attributes;

      showType = MULTIPLE_DIRECTORY;
      ToggleCallback (w, client_data, call_data);
   }
   else if (w == preferences_rec->tree_files_never)
   {
      XtSetValues (preferences_rec->tree_files_choose, false_args, 1);
      XtSetValues (preferences_rec->tree_files_always, false_args, 1);
      XtSetValues (preferences_rec->tree_files_never, true_args, 1);
   }
   else if (w == preferences_rec->tree_files_choose)
   {
      XtSetValues (preferences_rec->tree_files_never, false_args, 1);
      XtSetValues (preferences_rec->tree_files_always, false_args, 1);
      XtSetValues (preferences_rec->tree_files_choose, true_args, 1);
   }
   else if (w == preferences_rec->tree_files_always)
   {
      XtSetValues (preferences_rec->tree_files_never, false_args, 1);
      XtSetValues (preferences_rec->tree_files_choose, false_args, 1);
      XtSetValues (preferences_rec->tree_files_always, true_args, 1);
   }
   else if (w == preferences_rec->by_name)
   {
      if (showType == SINGLE_DIRECTORY)
         XtSetSensitive (XtParent (preferences_rec->random_on), True);
      XtSetValues (preferences_rec->by_name_and_icon, false_args, 1);
      XtSetValues (preferences_rec->by_name_and_small_icon, false_args, 1);
      XtSetValues (preferences_rec->by_attributes, false_args, 1);
      XtSetValues (preferences_rec->by_name, true_args, 1);
      *viewP = BY_NAME;
   }
   else if (w == preferences_rec->by_name_and_icon)
   {
      if (showType == SINGLE_DIRECTORY)
         XtSetSensitive (XtParent (preferences_rec->random_on), True);
      XtSetValues (preferences_rec->by_name, false_args, 1);
      XtSetValues (preferences_rec->by_name_and_small_icon, false_args, 1);
      XtSetValues (preferences_rec->by_attributes, false_args, 1);
      XtSetValues (preferences_rec->by_name_and_icon, true_args, 1);
      *viewP = BY_NAME_AND_ICON;
   }
   else if (w == preferences_rec->by_name_and_small_icon)
   {
      if (showType == SINGLE_DIRECTORY)
         XtSetSensitive (XtParent (preferences_rec->random_on), True);
      XtSetValues (preferences_rec->by_name, false_args, 1);
      XtSetValues (preferences_rec->by_attributes, false_args, 1);
      XtSetValues (preferences_rec->by_name_and_icon, false_args, 1);
      XtSetValues (preferences_rec->by_name_and_small_icon, true_args, 1);
      *viewP = BY_NAME_AND_SMALL_ICON;
   }
   else if (w == preferences_rec->by_attributes)
   {
      XtSetSensitive (XtParent (preferences_rec->random_on), False);
      XtSetValues (preferences_rec->by_name, false_args, 1);
      XtSetValues (preferences_rec->by_name_and_icon, false_args, 1);
      XtSetValues (preferences_rec->by_name_and_small_icon, false_args, 1);
      XtSetValues (preferences_rec->by_attributes, true_args, 1);
      *viewP = BY_ATTRIBUTES;
   }
   else if (w == preferences_rec->order_file_type)
   {
      XtSetValues (preferences_rec->order_alphabetical, false_args, 1);
      XtSetValues (preferences_rec->order_date, false_args, 1);
      XtSetValues (preferences_rec->order_size, false_args, 1);
      XtSetValues (preferences_rec->order_file_type, true_args, 1);
   }
   else if (w == preferences_rec->order_alphabetical)
   {
      XtSetValues (preferences_rec->order_file_type, false_args, 1);
      XtSetValues (preferences_rec->order_date, false_args, 1);
      XtSetValues (preferences_rec->order_size, false_args, 1);
      XtSetValues (preferences_rec->order_alphabetical, true_args, 1);
   }
   else if (w == preferences_rec->order_date)
   {
      XtSetValues (preferences_rec->order_file_type, false_args, 1);
      XtSetValues (preferences_rec->order_alphabetical, false_args, 1);
      XtSetValues (preferences_rec->order_size, false_args, 1);
      XtSetValues (preferences_rec->order_date, true_args, 1);
   }
   else if (w == preferences_rec->order_size)
   {
      XtSetValues (preferences_rec->order_file_type, false_args, 1);
      XtSetValues (preferences_rec->order_alphabetical, false_args, 1);
      XtSetValues (preferences_rec->order_date, false_args, 1);
      XtSetValues (preferences_rec->order_size, true_args, 1);
   }
   else if (w == preferences_rec->direction_ascending)
   {
      XtSetValues (preferences_rec->direction_descending, false_args, 1);
      XtSetValues (preferences_rec->direction_ascending, true_args, 1);
   }
   else if (w == preferences_rec->direction_descending)
   {
      XtSetValues (preferences_rec->direction_ascending, false_args, 1);
      XtSetValues (preferences_rec->direction_descending, true_args, 1);
   }
   else if (w == preferences_rec->random_on)
   {
      XtSetValues (preferences_rec->random_off, false_args, 1);
      XtSetValues (preferences_rec->random_on, true_args, 1);
   }
   else if (w == preferences_rec->random_off)
   {
      XtSetValues (preferences_rec->random_on, false_args, 1);
      XtSetValues (preferences_rec->random_off, true_args, 1);
   }
}


/************************************************************************
 *
 *  OkCallback
 *      This is a callback function called when the (Ok) button
 *      is pressed.  It saves the new preverences data itnernally and
 *      then calls the encapsulations callback function.
 *
 ************************************************************************/

static void
OkCallback(
        Widget w,
        PreferencesApply *apply_data,
        XtPointer call_data )
{
   PreferencesRec * preferences_rec;
   DialogData * dialog_data;

   ApplyCallback( w, apply_data, call_data);

   preferences_rec = (PreferencesRec *) apply_data->preferences_rec;
   dialog_data = _DtGetInstanceData ((XtPointer)preferences_rec);

   XtCallCallbacks(preferences_rec->close, XmNactivateCallback,
                                                 (XtPointer)dialog_data);
}


/************************************************************************
 *
 *  ApplyCallback
 *	This is a callback function called when the (Apply) button
 *	is pressed.  It saves the new preverences data itnernally and
 *	then calls the encapsulations callback funtion.
 *
 ************************************************************************/

static void
ApplyCallback(
        Widget w,
        PreferencesApply *apply_data,
        XtPointer call_data )
{
   (*(apply_data->callback)) (w, apply_data->client_data, NULL);
}




/************************************************************************
 *
 *  GetPreferencesValues
 *	Update the current preferences values within the data structure
 *	from the current toggle button values.
 *
 ************************************************************************/

static void
GetPreferencesValues(
        PreferencesRec *preferences_rec,
        PreferencesData *preferences_data )
{
   Boolean set;
   Arg args[2];

   XtSetArg (args[0], XmNset, &set);

   XtGetValues (preferences_rec->show_single, args, 1);
   if (set) preferences_data->show_type = SINGLE_DIRECTORY;
   XtGetValues (preferences_rec->show_multiple, args, 1);
   if (set) preferences_data->show_type = MULTIPLE_DIRECTORY;

   preferences_data->view_single = preferences_rec->view_single;
   preferences_data->view_tree = preferences_rec->view_tree;

   XtGetValues (preferences_rec->tree_files_never, args, 1);
   if (set) preferences_data->tree_files = TREE_FILES_NEVER;
   XtGetValues (preferences_rec->tree_files_choose, args, 1);
   if (set) preferences_data->tree_files = TREE_FILES_CHOOSE;
   XtGetValues (preferences_rec->tree_files_always, args, 1);
   if (set) preferences_data->tree_files = TREE_FILES_ALWAYS;

   XtGetValues (preferences_rec->order_file_type, args, 1);
   if (set) preferences_data->order = ORDER_BY_FILE_TYPE;
   XtGetValues (preferences_rec->order_alphabetical, args, 1);
   if (set) preferences_data->order = ORDER_BY_ALPHABETICAL;
   XtGetValues (preferences_rec->order_date, args, 1);
   if (set) preferences_data->order = ORDER_BY_DATE;
   XtGetValues (preferences_rec->order_size, args, 1);
   if (set) preferences_data->order = ORDER_BY_SIZE;

   XtGetValues (preferences_rec->direction_ascending, args, 1);
   if (set) preferences_data->direction = DIRECTION_ASCENDING;
   XtGetValues (preferences_rec->direction_descending, args, 1);
   if (set) preferences_data->direction = DIRECTION_DESCENDING;

   XtGetValues (preferences_rec->random_on, args, 1);
   if (set) preferences_data->positionEnabled = RANDOM_ON;
   XtGetValues (preferences_rec->random_off, args, 1);
   if (set) preferences_data->positionEnabled = RANDOM_OFF;

   XtGetValues (preferences_rec->show_iconic_path, args, 1);
   preferences_data->show_iconic_path = set;
   XtGetValues (preferences_rec->show_current_dir, args, 1);
   preferences_data->show_current_dir = set;
   XtGetValues (preferences_rec->show_status_line, args, 1);
   preferences_data->show_status_line = set;
}




/************************************************************************
 *
 *  ShowTypeToString
 *	Convert the show type to a string and write it out.
 *
 ***********************************************************************/

void
ShowTypeToString(
        int fd,
        unsigned char *value,
        char *out_buf )
{
   char * buf = NULL;

   if (*value == SINGLE_DIRECTORY)
      buf = Single_directory;
   else if (*value == MULTIPLE_DIRECTORY)
      buf = DirectoryTree;

   _DtStringToString(fd, (char **)&buf, out_buf);
}




/************************************************************************
 *
 *  StringToShowType
 *	Convert string to a show type.
 *
 ************************************************************************/

void
StringToShowType(
        XrmValuePtr args,
        Cardinal *num_args,
        XrmValue *from_val,
        XrmValue *to_val )
{
   char * in_str = (char *)(from_val->addr);
   static unsigned char i;

   to_val->size = sizeof (unsigned char);
   to_val->addr = (XtPointer) &i;

   if (_DtStringsAreEquivalent (in_str, Single_directory))
      i = SINGLE_DIRECTORY;
   else if (_DtStringsAreEquivalent (in_str, DirectoryTree))
      i = MULTIPLE_DIRECTORY;
   else
   {
      to_val->size = 0;
      to_val->addr = NULL;
      XtStringConversionWarning ((char *)from_val->addr, SHOW_TYPE);
   }
}




/************************************************************************
 *
 *  TreeFilesToString
 *	Convert the tree-files option to a string and write it out.
 *
 ***********************************************************************/

void
TreeFilesToString(
        int fd,
        unsigned char *value,
        char *out_buf )
{
   char * buf = NULL;

   if (*value == TREE_FILES_NEVER)
      buf = Tree_files_never;
   else if (*value == TREE_FILES_CHOOSE)
      buf = Tree_files_choose;
   else if (*value == TREE_FILES_ALWAYS)
      buf = Tree_files_always;

   _DtStringToString(fd, (char **)&buf, out_buf);
}




/************************************************************************
 *
 *  StringToTreeFiles
 *	Convert string to a tree-files option.
 *
 ************************************************************************/

void
StringToTreeFiles(
        XrmValuePtr args,
        Cardinal *num_args,
        XrmValue *from_val,
        XrmValue *to_val )
{
   char * in_str = (char *)(from_val->addr);
   static unsigned char i;

   to_val->size = sizeof (unsigned char);
   to_val->addr = (XtPointer) &i;

   if (_DtStringsAreEquivalent (in_str, Tree_files_never))
      i = TREE_FILES_NEVER;
   else if (_DtStringsAreEquivalent (in_str, Tree_files_choose))
      i = TREE_FILES_CHOOSE;
   else if (_DtStringsAreEquivalent (in_str, Tree_files_always))
      i = TREE_FILES_ALWAYS;
   else
   {
      to_val->size = 0;
      to_val->addr = NULL;
      XtStringConversionWarning ((char *)from_val->addr, TREE_FILES);
   }
}




/************************************************************************
 *
 *  ViewToString
 *	Convert the view mode to a string and write it out.
 *
 ***********************************************************************/

void
ViewToString(
        int fd,
        unsigned char *value,
        char *out_buf )
{
   char * buf = NULL;

   if (*value == BY_NAME)
      buf = By_name;
   else if (*value == BY_NAME_AND_ICON)
      buf = By_name_and_icon;
   else if (*value == BY_NAME_AND_SMALL_ICON)
      buf = By_name_and_small_icon;
   else if (*value == BY_ATTRIBUTES)
      buf = By_attributes;

   _DtStringToString(fd, (char **)&buf, out_buf);
}




/************************************************************************
 *
 *  StringToView
 *	Convert string to a viewing mode. 
 *
 ************************************************************************/

void
StringToView(
        XrmValuePtr args,
        Cardinal *num_args,
        XrmValue *from_val,
        XrmValue *to_val )
{
   char * in_str = (char *)(from_val->addr);
   static unsigned char i;

   to_val->size = sizeof (unsigned char);
   to_val->addr = (XtPointer) &i;

   if (_DtStringsAreEquivalent (in_str, By_name)) 
      i = BY_NAME;
   else if (_DtStringsAreEquivalent (in_str, By_name_and_icon)) 
      i = BY_NAME_AND_ICON;
   else if (_DtStringsAreEquivalent (in_str, By_name_and_small_icon)) 
      i = BY_NAME_AND_SMALL_ICON;
   else if (_DtStringsAreEquivalent (in_str, By_attributes)) 
      i = BY_ATTRIBUTES;
   else
   {
      to_val->size = 0;
      to_val->addr = NULL;
      XtStringConversionWarning ((char *)from_val->addr, VIEW);
   }
}





/************************************************************************
 *
 *  OrderToString
 *	Convert the viewing order to a string and write it out.
 *
 ***********************************************************************/

void
OrderToString(
        int fd,
        unsigned char *value,
        char *out_buf )
{
   char * buf = NULL;

   if (*value == ORDER_BY_FILE_TYPE)
      buf = Order_by_file_type;
   else if (*value == ORDER_BY_ALPHABETICAL)
      buf = Order_by_alphabetical;
   else if (*value == ORDER_BY_DATE)
      buf = Order_by_date;
   else if (*value == ORDER_BY_SIZE)
      buf = Order_by_size;

   _DtStringToString(fd, (char **)&buf, out_buf);
}




/************************************************************************
 *
 *  StringToOrder
 *	Convert string to a view order.
 *
 ************************************************************************/

void
StringToOrder(
        XrmValuePtr args,
        Cardinal *num_args,
        XrmValue *from_val,
        XrmValue *to_val )
{
   char * in_str = (char *)(from_val->addr);
   static unsigned char i;

   to_val->size = sizeof (unsigned char);
   to_val->addr = (XtPointer) &i;

   if (_DtStringsAreEquivalent (in_str, Order_by_file_type)) 
      i = ORDER_BY_FILE_TYPE;
   else if (_DtStringsAreEquivalent (in_str, Order_by_alphabetical)) 
      i = ORDER_BY_ALPHABETICAL;
   else if (_DtStringsAreEquivalent (in_str, Order_by_date)) 
      i = ORDER_BY_DATE;
   else if (_DtStringsAreEquivalent (in_str, Order_by_size)) 
      i = ORDER_BY_SIZE;
   else
   {
      to_val->size = 0;
      to_val->addr = NULL;
      XtStringConversionWarning ((char *)from_val->addr, DIRECTION_RESRC);
   }
}




/************************************************************************
 *
 *  DirectionToString
 *	Convert the ordering direction to a string and write it out.
 *
 ***********************************************************************/

void
DirectionToString(
        int fd,
        unsigned char *value,
        char *out_buf )
{
   char * buf = NULL;

   if (*value == DIRECTION_ASCENDING)
      buf = Direction_ascending;
   else if (*value == DIRECTION_DESCENDING)
      buf = Direction_descending;

   _DtStringToString(fd, (char **)&buf, out_buf);
}


/************************************************************************
 *
 *  RandomToString
 *      Convert the random placement to a string and write it out.
 *
 ***********************************************************************/

void
RandomToString(
        int fd,
        unsigned char *value,
        char *out_buf )
{
   char * buf = NULL;

   if (*value == RANDOM_ON)
      buf = Random_on;
   else if (*value == RANDOM_OFF)
      buf = Random_off;

   _DtStringToString(fd, (char **)&buf, out_buf);
}


/************************************************************************
 *
 *  StringToDirection
 *	Convert string to a view ordering direction.
 *
 ************************************************************************/

void
StringToDirection(
        XrmValuePtr args,
        Cardinal *num_args,
        XrmValue *from_val,
        XrmValue *to_val )
{
   char * in_str = (char *)(from_val->addr);
   static unsigned char i;

   to_val->size = sizeof (unsigned char);
   to_val->addr = (XtPointer) &i;

   if (_DtStringsAreEquivalent (in_str, Direction_ascending)) 
      i = DIRECTION_ASCENDING;
   else if (_DtStringsAreEquivalent (in_str, Direction_descending)) 
      i = DIRECTION_DESCENDING;
   else
   {
      to_val->size = 0;
      to_val->addr = NULL;
      XtStringConversionWarning ((char *)from_val->addr, DIRECTION_RESRC);
   }
}


/************************************************************************
 *
 *  StringToRandom
 *      Convert string to either random placement on or off  
 *
 ************************************************************************/

void
StringToRandom(
        XrmValuePtr args,
        Cardinal *num_args,
        XrmValue *from_val,
        XrmValue *to_val )
{
   char * in_str = (char *)(from_val->addr);
   static unsigned char i;

   to_val->size = sizeof (unsigned char);
   to_val->addr = (XtPointer) &i;

   if (_DtStringsAreEquivalent (in_str, Random_on))
      i = RANDOM_ON;
   else if (_DtStringsAreEquivalent (in_str, Random_off))
      i = RANDOM_OFF;
   else
   {
      to_val->size = 0;
      to_val->addr = NULL;
      XtStringConversionWarning ((char *)from_val->addr, RANDOM);
   }
}
