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
/* $TOG: ModAttr.c /main/11 1999/12/09 13:07:05 mgreess $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 *   FILE:           ModAttr.c
 *
 *   COMPONENT_NAME: Desktop File Manager (dtfile)
 *
 *   Description:    Source file for the modify file attributes dialog.
 *
 *   FUNCTIONS: Create
 *		Destroy
 *		FreeValues
 *		GetDefaultValues
 *		GetModAttrValues
 *		GetResourceValues
 *		GetValues
 *		InstallChange
 *		InstallClose
 *		InvalidModAttrMessage
 *		LoadFileAttributes
 *		OkCallback
 *		SetValues
 *		WriteResourceValues
 *		_LoadFileAttributes
 *		displayFSDialog
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *
 ****************************************************************************
 ************************************<+>*************************************/

#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <grp.h>
#include <pwd.h>

#include <Xm/XmP.h>
#include <Xm/BulletinB.h>
#include <Xm/CascadeBG.h>
#include <Xm/DialogS.h>
#include <Xm/Frame.h>
#include <Xm/Form.h>
#include <Xm/LabelG.h>
#include <Xm/MessageB.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/ScrolledW.h>
#include <Xm/TextF.h>
#include <Xm/ToggleBG.h>
#include <Xm/SeparatoG.h>
#include <Xm/VendorSEP.h>
#include <Xm/MwmUtil.h>
#include <Xm/Protocols.h>

#include <Dt/Icon.h>
#include <Dt/IconP.h>
#include <Dt/IconFile.h>
#include <Dt/TitleBox.h>
#include <Dt/Dts.h>
#include <Dt/SharedProcs.h>

#include <X11/ShellP.h>
#include <X11/Shell.h>
#include <X11/Xatom.h>

#include <Dt/Connect.h>
#include <Dt/DtNlUtils.h>

#include <Tt/tttk.h>

#include "Encaps.h"
#include "SharedProcs.h"
#include "FileMgr.h"
#include "Desktop.h"
#include "Main.h"
#include "Common.h"
#include "ModAttr.h"
#include "Help.h"

#ifndef CDE_INSTALLATION_TOP
#define CDE_INSTALLATION_TOP "/usr/dt"
#endif

/* Error message defines */

#define UNKNOWN_USER      0
#define UNKNOWN_GROUP     1


/* Field defines */
#define HOST       0x01
#define DIR        0x02
#define NAME       0x04
#define FILETYPE   0x08
#define GROUP      0x10
#define OWNER      0x20
#define BITS       0x40
#define SIZE       0x80
#define MODTIME    0x100

#define GMA_VALID           0
#define GMA_INVALID         1
#define GMA_NO_FILE         2

#define ALL_FIELDS       HOST|DIR|NAME|FILETYPE|GROUP|OWNER|BITS|SIZE|MODTIME
#define ALL_BUT_ACCESS   HOST|DIR|NAME|FILETYPE|GROUP|OWNER|SIZE|MODTIME
#define ALL_BUT_OWNER_GROUP_ACCESS    HOST|DIR|NAME|FILETYPE|SIZE|MODTIME

#define FM_TYPE_LABEL  "TYPE_LABEL"

static char * MODIFY_ATTR = "ModifyAttributes";
ModAttrApply *globalApplyData;
char *parentDir;

/*  Resource definitions for the modify file attribute dialog  */

static DialogResource resources[] =
{
   { "host", XmRString, sizeof(String), 
     XtOffset(ModAttrDataPtr, host),
     (XtPointer) NULL, _DtStringToString },

   { "directory", XmRString, sizeof(String), 
     XtOffset(ModAttrDataPtr, directory),
     (XtPointer) NULL, _DtStringToString },

   { "name", XmRString, sizeof(String), 
     XtOffset(ModAttrDataPtr, name), 
     (XtPointer) NULL, _DtStringToString },

   { "link_name", XmRString, sizeof(String), 
     XtOffset(ModAttrDataPtr, link_name), 
     (XtPointer) NULL, _DtStringToString },

   { "access", XmRInt, sizeof(int), 
     XtOffset(ModAttrDataPtr, accessBits),
     (XtPointer) 0, _DtIntToString },
};


/********    Static Function Declarations    ********/

static void Create(
                        Display *display,
                        Widget parent,
                        Widget *return_widget,
                        XtPointer *dialog) ;
static void InstallChange(
                        ModAttrRec *modAttr_rec,
                        XtCallbackProc callback,
                        XtPointer client_data) ;
static void InstallClose(
                        ModAttrRec *modAttr_rec,
                        XtCallbackProc callback,
                        XtPointer client_data) ;
static void Destroy(
                        ModAttrRec *modAttr_rec) ;
static XtPointer GetValues(
                        ModAttrRec *modAttr_rec) ;
static XtPointer GetDefaultValues( void ) ;
static XtPointer GetResourceValues(
                        XrmDatabase data_base,
                        char **name_list) ;
static void SetValues(
                        ModAttrRec *modAttr_rec,
                        ModAttrData *modAttr_data) ;
static void WriteResourceValues(
                        DialogData *values,
                        int fd,
                        char **name_list) ;
static void FreeValues(
                        ModAttrData *modAttr_data) ;
static XtCallbackProc OkCallback(
                        Widget w,
                        ModAttrApply *apply_data,
                        XtPointer call_data) ;
static short GetModAttrValues(
                        ModAttrRec *modAttr_rec,
                        ModAttrData *modAttr_data,
                        Boolean validate) ;
static void InvalidModAttrMessage(
                        Widget w,
                        int messageIndex) ;
static Boolean _LoadFileAttributes(
                        String host,
                        String dir,
                        String file,
                        ModAttrData *modAttr_data,
                        unsigned long mask) ;
static void displayFSDialog(
                            Widget                        w,
                            dtFSData                    * fileSystem,
                            XmPushButtonCallbackStruct  * cbs);
static void DontChangeCB(
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data );
static void ChangeCB(
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data );

/********    End Static Function Declarations    ********/


/*
 *  The Dialog Class structure.
 */
static DialogClass modAttrClassRec =
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

DialogClass * modAttrClass = (DialogClass *) &modAttrClassRec;

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
   ModAttrRec * modAttr_rec;
   Widget shell, form, nameLabel, name;
   Widget link_label, link_name;
   Widget fileType, ownerLabel, owner;
   Widget groupLabel, group, permissionLabel, permissionForm;
   Widget ownerPLabel, groupPLabel, otherPLabel;
   Widget readLabel, writeLabel, executeLabel;
   Widget ownerRead, ownerWrite, ownerExecute;
   Widget groupRead, groupWrite, groupExecute;
   Widget otherRead, otherWrite, otherExecute;
   Widget fsPushButton, fsWarning;
   Widget sizeLabel, size;
   Widget modLabel, mod;
   Widget groupL, ownerL;
   Widget separator, ok, cancel, help;
   XmString label_string;
   XmString empty_label_string;
   Arg args[15];
   int n;
   char * tmpStr;
   Dimension len[2];
   int flag=FLAG_RESET;


   /*  Allocate the modify file attributes dialog instance record.  */

   modAttr_rec = (ModAttrRec *) XtMalloc (sizeof (ModAttrRec));

   /*  Create the shell form used for the dialog.  */

   n = 0;
   XtSetArg (args[n], XmNmwmFunctions, MWM_FUNC_MOVE |
             MWM_FUNC_CLOSE);                                           ++n;
   XtSetArg (args[n], XmNmwmDecorations, MWM_DECOR_BORDER |
             MWM_DECOR_TITLE);                                          ++n;
   XtSetArg (args[n], XmNallowShellResize, True);                       ++n;
   shell = XmCreateDialogShell (parent, "file_properties", args, n);

   /* Set the useAsyncGeo on the shell */
   XtSetArg (args[0], XmNuseAsyncGeometry, True);
   XtSetValues (XtParent(shell), args, 1);

   n = 0;
   XtSetArg (args[n], XmNshadowThickness, 1);                   n++;
   XtSetArg (args[n], XmNshadowType, XmSHADOW_OUT);             n++;
   XtSetArg (args[n], XmNautoUnmanage, False);                  n++;
   form = XmCreateForm (shell, "form", args, n);
   XtAddCallback(form, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PROPERTIES_DIALOG_STR);


   /* Create the field displaying the selected file name */

   label_string = XmStringCreateLocalized (((char *) GETMESSAGE(21,30, "Name: ")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);            n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);        n++;
   XtSetArg (args[n], XmNleftOffset, 5);                        n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);         n++;
   XtSetArg (args[n], XmNtopOffset, 5);                         n++;
   XtSetArg (args[n], XmNtraversalOn, False);                   n++;
   nameLabel = XmCreateLabelGadget (form, "file_name_label", args, n);
   XtManageChild (nameLabel);
   XmStringFree (label_string);
   XtAddCallback(nameLabel, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PROPERTIES_DIALOG_STR);

   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET);      n++;
   XtSetArg (args[n], XmNleftWidget, nameLabel);                n++;
   XtSetArg (args[n], XmNleftOffset, 0);                        n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);         n++;
   XtSetArg (args[n], XmNtopOffset, 5);                         n++;
   XtSetArg (args[n], XmNtraversalOn, False);                   n++;
   name = XmCreateLabelGadget (form, "file_name", args, n);
   XtManageChild (name);
   XtAddCallback(name, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PROPERTIES_DIALOG_STR);

   /* Create the optional field displaying the symbolic link name */

   label_string = XmStringCreateLocalized ((GETMESSAGE(21,18, "Is Linked To: ")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);            n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);        n++;
   XtSetArg (args[n], XmNleftOffset, 5);                        n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, name);                      n++;
   XtSetArg (args[n], XmNtopOffset, 5);                         n++;
   XtSetArg (args[n], XmNtraversalOn, False);                   n++;
   link_label= XmCreateLabelGadget (form, "link_name_label", args, n);
   XtManageChild (link_label);
   XmStringFree (label_string);
   XtAddCallback(link_label, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PROPERTIES_DIALOG_STR);

   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET);      n++;
   XtSetArg (args[n], XmNleftWidget, link_label);               n++;
   XtSetArg (args[n], XmNleftOffset, 0);                        n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, name);                      n++;
   XtSetArg (args[n], XmNtopOffset, 5);                         n++;
   XtSetArg (args[n], XmNtraversalOn, False);                   n++;
   link_name = XmCreateLabelGadget (form, "link_name", args, n);
   XtManageChild (link_name);
   XtAddCallback(link_name, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PROPERTIES_DIALOG_STR);


   /* Create the field displaying the owner name and the group name*/

   label_string = XmStringCreateLocalized (((char *)
               GETMESSAGE(21, 3, "Owner Name: ")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);            n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);        n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, link_name);                 n++;
   XtSetArg (args[n], XmNtopOffset, 15);                        n++;
   XtSetArg (args[n], XmNleftOffset, 5);                        n++;
   XtSetArg (args[n], XmNtraversalOn, False);                   n++;
   ownerLabel = XmCreateLabelGadget (form, "owner_name", args, n);
   XtManageChild (ownerLabel);
   XmStringFree (label_string);
   XtAddCallback(ownerLabel, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PROPERTIES_DIALOG_STR);

   label_string = XmStringCreateLocalized (((char *)
               GETMESSAGE(21, 4, "Group Name: ")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);            n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);        n++;
   XtSetArg (args[n], XmNleftOffset, 5);                        n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, ownerLabel);                n++;
   XtSetArg (args[n], XmNtopOffset, 22);                        n++;
   XtSetArg (args[n], XmNtraversalOn, False);                   n++;
   groupLabel = XmCreateLabelGadget (form, "group_name", args, n);
   XtManageChild (groupLabel);
   XmStringFree (label_string);
   XtAddCallback(groupLabel, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PROPERTIES_DIALOG_STR);

   XtSetArg (args[0], XmNwidth, &len[0]);
   XtGetValues(ownerLabel, args, 1);
   XtSetArg (args[0], XmNwidth, &len[1]);
   XtGetValues(groupLabel, args, 1);

   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET);      n++;
   if(len[0] > len[1]) {
       XtSetArg (args[n], XmNleftWidget, ownerLabel);           n++;
   }
   else {
       XtSetArg (args[n], XmNleftWidget, groupLabel);           n++;
   }
   XtSetArg (args[n], XmNleftOffset, 0);                        n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, link_name);                 n++;
   XtSetArg (args[n], XmNtopOffset, 8);                         n++;
   owner = XmCreateTextField (form, "owner_text", args, n);
   XtManageChild (owner);
   XtAddCallback(owner, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PROPERTIES_DIALOG_STR);

   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET);      n++;
   if(len[0] > len[1]) {
       XtSetArg (args[n], XmNleftWidget, ownerLabel);           n++;
   }
   else {
       XtSetArg (args[n], XmNleftWidget, groupLabel);           n++;
   }
   XtSetArg (args[n], XmNleftOffset, 5);                        n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET);    n++;
   XtSetArg (args[n], XmNtopWidget, ownerLabel);                n++;
   XtSetArg (args[n], XmNtopOffset, 0);                         n++;
   XtSetArg (args[n], XmNtraversalOn, False);                   n++;
   ownerL = XmCreateLabelGadget (form, "owner_textL", args, n);
   XtAddCallback(ownerL, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PROPERTIES_DIALOG_STR);

   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET);      n++;
   if(len[0] > len[1]) {
       XtSetArg (args[n], XmNleftWidget, ownerLabel);           n++;
   }
   else {
       XtSetArg (args[n], XmNleftWidget, groupLabel);           n++;
   }
   XtSetArg (args[n], XmNleftOffset, 0);                        n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, ownerLabel);                n++;
   XtSetArg (args[n], XmNtopOffset, 15);                        n++;
   group = XmCreateTextField (form, "group_text", args, n);
   XtManageChild (group);
   XtAddCallback(group, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PROPERTIES_DIALOG_STR);

   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET);      n++;
   if(len[0] > len[1]) {
       XtSetArg (args[n], XmNleftWidget, ownerLabel);           n++;
   }
   else {
       XtSetArg (args[n], XmNleftWidget, groupLabel);           n++;
   }
   XtSetArg (args[n], XmNleftOffset, 5);                        n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET);    n++;
   XtSetArg (args[n], XmNtopWidget, groupLabel);                n++;
   XtSetArg (args[n], XmNtopOffset, 0);                         n++;
   XtSetArg (args[n], XmNtraversalOn, False);                   n++;
   groupL = XmCreateLabelGadget (form, "group_textL", args, n);
   XtAddCallback(groupL, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PROPERTIES_DIALOG_STR);

   /* Create the field displaying the file type info */

   n = 0;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, link_name);                 n++;
   XtSetArg (args[n], XmNtopOffset, 5);                         n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET);      n++;
   XtSetArg (args[n], XmNleftWidget, owner);                    n++;
   XtSetArg (args[n], XmNleftOffset, 20);                       n++;
   XtSetArg (args[n], XmNshadowThickness, 2);                   n++;
   XtSetArg (args[n], XmNshadowType, XmSHADOW_ETCHED_IN);       n++;
   XtSetArg (args[n], XmNhighlightThickness, 5);                n++;
   XtSetArg (args[n], XmNmarginWidth, 4);                       n++;
   XtSetArg (args[n], XmNmarginHeight, 4);                      n++;
   XtSetArg (args[n], XmNtraversalOn, False);                   n++;
   XtSetArg (args[n], XmNpixmapPosition, XmPIXMAP_TOP);         n++;
   XtSetArg (args[n], XmNbehavior, XmICON_LABEL);               n++;
   fileType = _DtCreateIcon (form, "file_type", args, n);
   XtManageChild (fileType);
   XtAddCallback(fileType, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PROPERTIES_DIALOG_STR);

   /* Create the label for the permissions */

   label_string = XmStringCreateLocalized (((char *)
               GETMESSAGE(21, 5, "Permissions:")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);            n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);        n++;
   XtSetArg (args[n], XmNleftOffset, 5);                        n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, groupLabel);                n++;
   XtSetArg (args[n], XmNtopOffset, 15);                        n++;
   XtSetArg (args[n], XmNtraversalOn, False);                   n++;
   permissionLabel = XmCreateLabelGadget (form, "permission_label", args, n);
   XtManageChild (permissionLabel);
   XmStringFree (label_string);
   XtAddCallback(permissionLabel, XmNhelpCallback, 
                 (XtCallbackProc)HelpRequestCB, 
                 HELP_PROPERTIES_DIALOG_STR);

   /* Create the box containing the permission settings */

   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);        n++;
   XtSetArg (args[n], XmNleftOffset, 10);                       n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);       n++;
   XtSetArg (args[n], XmNrightOffset, 5);                       n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, permissionLabel);           n++;
   XtSetArg (args[n], XmNtopOffset, 3);                         n++;
   XtSetArg (args[n], XmNshadowThickness, 2);                   n++;
   XtSetArg (args[n], XmNshadowType, XmSHADOW_IN);              n++;
   permissionForm = XmCreateForm (form, "permission_form", args, n);
   XtManageChild (permissionForm);
   XtAddCallback(permissionForm, XmNhelpCallback, 
                 (XtCallbackProc)HelpRequestCB, 
                 HELP_PROPERTIES_DIALOG_STR);

   /* Create the labels and toggles for the permissions */

   label_string = XmStringCreateLocalized (((char *)
               GETMESSAGE(21, 6, "Read")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);            n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);    n++;
   XtSetArg (args[n], XmNleftPosition, 20);                     n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);   n++;
   XtSetArg (args[n], XmNrightPosition, 45);                    n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);         n++;
   XtSetArg (args[n], XmNtopOffset, 10);                        n++;
   XtSetArg (args[n], XmNtraversalOn, False);                   n++;
   readLabel = XmCreateLabelGadget (permissionForm, "read_label", args, n);
   XtManageChild (readLabel);
   XmStringFree (label_string);
   XtAddCallback(readLabel, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PROPERTIES_DIALOG_STR);

   label_string = XmStringCreateLocalized (((char *)
               GETMESSAGE(21, 7, "Write")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);            n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);    n++;
   XtSetArg (args[n], XmNleftPosition, 45);                     n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);   n++;
   XtSetArg (args[n], XmNrightPosition, 70);                    n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);         n++;
   XtSetArg (args[n], XmNtopOffset, 10);                        n++;
   XtSetArg (args[n], XmNtraversalOn, False);                   n++;
   writeLabel = XmCreateLabelGadget (permissionForm, "write_label", args, n);
   XtManageChild (writeLabel);
   XmStringFree (label_string);
   XtAddCallback(writeLabel, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PROPERTIES_DIALOG_STR);

   label_string = XmStringCreateLocalized (((char *)
               GETMESSAGE(21, 8, "Execute")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);            n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);    n++;
   XtSetArg (args[n], XmNleftPosition, 70);                     n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);   n++;
   XtSetArg (args[n], XmNrightPosition, 95);                    n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);         n++;
   XtSetArg (args[n], XmNtopOffset, 10);                        n++;
   XtSetArg (args[n], XmNtraversalOn, False);                   n++;
   executeLabel = XmCreateLabelGadget(permissionForm, "execute_label", args, n);
   XtAddCallback(executeLabel, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PROPERTIES_DIALOG_STR);
   XtManageChild (executeLabel);
   XmStringFree (label_string);

   label_string = XmStringCreateLocalized (((char *)
               GETMESSAGE(21, 9, "Owner:")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);            n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);    n++;
   XtSetArg (args[n], XmNleftPosition, 5);                      n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, readLabel);                 n++;
   XtSetArg (args[n], XmNtopOffset, 10);                        n++;
   XtSetArg (args[n], XmNtraversalOn, False);                   n++;
   ownerPLabel = XmCreateLabelGadget (permissionForm, "owner_label", args, n);
   XtManageChild (ownerPLabel);
   XmStringFree (label_string);
   XtAddCallback(ownerPLabel, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PROPERTIES_DIALOG_STR);

   empty_label_string = XmStringCreateLocalized ("");
   n = 0;
   XtSetArg (args[n], XmNlabelString, empty_label_string);              n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);            n++;
   XtSetArg (args[n], XmNleftPosition, 30);                             n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET);      n++;
   XtSetArg (args[n], XmNtopWidget, ownerPLabel);                       n++;
   XtSetArg (args[n], XmNtopOffset, 0);                                 n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET);   n++;
   XtSetArg (args[n], XmNbottomWidget, ownerPLabel);                    n++;
   XtSetArg (args[n], XmNshadowThickness, 0);                           n++;
   ownerRead =
         XmCreateToggleButtonGadget (permissionForm, "owner_read", args, n);
   XtManageChild (ownerRead);
   XtAddCallback(ownerRead, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PROPERTIES_DIALOG_STR);

   n = 0;
   XtSetArg (args[n], XmNlabelString, empty_label_string);              n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);            n++;
   XtSetArg (args[n], XmNleftPosition, 54);                             n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET);      n++;
   XtSetArg (args[n], XmNtopWidget, ownerPLabel);                       n++;
   XtSetArg (args[n], XmNtopOffset, 0);                                 n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET);   n++;
   XtSetArg (args[n], XmNbottomWidget, ownerPLabel);                    n++;
   XtSetArg (args[n], XmNshadowThickness, 0);                           n++;
   ownerWrite =
         XmCreateToggleButtonGadget (permissionForm, "owner_write", args, n);
   XtManageChild (ownerWrite);
   XtAddCallback(ownerWrite, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PROPERTIES_DIALOG_STR);

   n = 0;
   XtSetArg (args[n], XmNlabelString, empty_label_string);              n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);            n++;
   XtSetArg (args[n], XmNleftPosition, 78);                             n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET);      n++;
   XtSetArg (args[n], XmNtopWidget, ownerPLabel);                       n++;
   XtSetArg (args[n], XmNtopOffset, 0);                                 n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET);   n++;
   XtSetArg (args[n], XmNbottomWidget, ownerPLabel);                    n++;
   XtSetArg (args[n], XmNshadowThickness, 0);                           n++;
   ownerExecute =
      XmCreateToggleButtonGadget (permissionForm, "owner_execute", args, n);
   XtManageChild (ownerExecute);
   XtAddCallback(ownerExecute, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PROPERTIES_DIALOG_STR);

   label_string = XmStringCreateLocalized (((char *)
               GETMESSAGE(21, 10, "Group:")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);            n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);    n++;
   XtSetArg (args[n], XmNleftPosition, 5);                      n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, ownerPLabel);               n++;
   XtSetArg (args[n], XmNtopOffset, 10);                        n++;
   XtSetArg (args[n], XmNtraversalOn, False);                   n++;
   groupPLabel = XmCreateLabelGadget (permissionForm, "group_label", args, n);
   XtManageChild (groupPLabel);
   XmStringFree (label_string);
   XtAddCallback(groupPLabel, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PROPERTIES_DIALOG_STR);

   n = 0;
   XtSetArg (args[n], XmNlabelString, empty_label_string);              n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);            n++;
   XtSetArg (args[n], XmNleftPosition, 30);                             n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET);      n++;
   XtSetArg (args[n], XmNtopWidget, groupPLabel);                       n++;
   XtSetArg (args[n], XmNtopOffset, 0);                                 n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET);   n++;
   XtSetArg (args[n], XmNbottomWidget, groupPLabel);                    n++;
   XtSetArg (args[n], XmNshadowThickness, 0);                           n++;
   groupRead =
          XmCreateToggleButtonGadget (permissionForm, "group_read", args, n);
   XtManageChild (groupRead);
   XtAddCallback(groupRead, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PROPERTIES_DIALOG_STR);

   n = 0;
   XtSetArg (args[n], XmNlabelString, empty_label_string);              n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);            n++;
   XtSetArg (args[n], XmNleftPosition, 54);                             n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET);      n++;
   XtSetArg (args[n], XmNtopWidget, groupPLabel);                       n++;
   XtSetArg (args[n], XmNtopOffset, 0);                                 n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET);   n++;
   XtSetArg (args[n], XmNbottomWidget, groupPLabel);                    n++;
   XtSetArg (args[n], XmNshadowThickness, 0);                           n++;
   groupWrite =
        XmCreateToggleButtonGadget (permissionForm, "group_write", args, n);
   XtManageChild (groupWrite);
   XtAddCallback(groupWrite, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PROPERTIES_DIALOG_STR);

   n = 0;
   XtSetArg (args[n], XmNlabelString, empty_label_string);              n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);            n++;
   XtSetArg (args[n], XmNleftPosition, 78);                             n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET);      n++;
   XtSetArg (args[n], XmNtopWidget, groupPLabel);                       n++;
   XtSetArg (args[n], XmNtopOffset, 0);                                 n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET);   n++;
   XtSetArg (args[n], XmNbottomWidget, groupPLabel);                    n++;
   XtSetArg (args[n], XmNshadowThickness, 0);                           n++;
   groupExecute =
         XmCreateToggleButtonGadget (permissionForm, "group_execute", args, n);
   XtManageChild (groupExecute);
   XtAddCallback(groupExecute, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PROPERTIES_DIALOG_STR);

   label_string = XmStringCreateLocalized (((char *)
               GETMESSAGE(21, 11, "Other:")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);                    n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);            n++;
   XtSetArg (args[n], XmNleftPosition, 5);                              n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);               n++;
   XtSetArg (args[n], XmNtopWidget, groupPLabel);                       n++;
   XtSetArg (args[n], XmNtopOffset, 10);                                n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);              n++;
   XtSetArg (args[n], XmNbottomOffset, 15);                             n++;
   XtSetArg (args[n], XmNtraversalOn, False);                           n++;
   otherPLabel = XmCreateLabelGadget (permissionForm, "other_label", args, n);
   XtManageChild (otherPLabel);
   XmStringFree (label_string);
   XtAddCallback(otherPLabel, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PROPERTIES_DIALOG_STR);

   n = 0;
   XtSetArg (args[n], XmNlabelString, empty_label_string);              n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);            n++;
   XtSetArg (args[n], XmNleftPosition, 30);                             n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET);      n++;
   XtSetArg (args[n], XmNtopWidget, otherPLabel);                       n++;
   XtSetArg (args[n], XmNtopOffset, 0);                                 n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET);   n++;
   XtSetArg (args[n], XmNbottomWidget, otherPLabel);                    n++;
   XtSetArg (args[n], XmNshadowThickness, 0);                           n++;
   otherRead =
         XmCreateToggleButtonGadget (permissionForm, "other_read", args, n);
   XtManageChild (otherRead);
   XtAddCallback(otherRead, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PROPERTIES_DIALOG_STR);

   n = 0;
   XtSetArg (args[n], XmNlabelString, empty_label_string);              n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);            n++;
   XtSetArg (args[n], XmNleftPosition, 54);                             n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET);      n++;
   XtSetArg (args[n], XmNtopWidget, otherPLabel);                       n++;
   XtSetArg (args[n], XmNtopOffset, 0);                                 n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET);   n++;
   XtSetArg (args[n], XmNbottomWidget, otherPLabel);                    n++;
   XtSetArg (args[n], XmNshadowThickness, 0);                           n++;
   otherWrite =
        XmCreateToggleButtonGadget (permissionForm, "other_write", args, n);
   XtManageChild (otherWrite);
   XtAddCallback(otherWrite, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PROPERTIES_DIALOG_STR);

   n = 0;
   XtSetArg (args[n], XmNlabelString, empty_label_string);              n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);            n++;
   XtSetArg (args[n], XmNleftPosition, 78);                             n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET);      n++;
   XtSetArg (args[n], XmNtopWidget, otherPLabel);                       n++;
   XtSetArg (args[n], XmNtopOffset, 0);                                 n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET);   n++;
   XtSetArg (args[n], XmNbottomWidget, otherPLabel);                    n++;
   XtSetArg (args[n], XmNshadowThickness, 0);                           n++;
   otherExecute =
      XmCreateToggleButtonGadget (permissionForm, "other_execute", args, n);
   XtManageChild (otherExecute);
   XmStringFree (empty_label_string);
   XtAddCallback(otherExecute, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PROPERTIES_DIALOG_STR);


   /* Create the optional label and push button to invoke a file-system specific dialog */
   /* Bottom and top attachments will be made when we know which of these widgets will be mannaged */
   /* The callback will be registered in SetValues after the config file has been read */

   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);	n++;
   XtSetArg (args[n], XmNleftPosition, 5);			n++;
   fsWarning = XmCreateLabelGadget (permissionForm, "fs_warning_label",
                                            args, n);

   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);	n++;
   XtSetArg (args[n], XmNleftPosition, 5);			n++;
   fsPushButton = XmCreatePushButtonGadget (permissionForm, "fs_dialog_button",
                                            args, n);



   /* Create the field displaying the size */

   label_string = XmStringCreateLocalized (((char *)
           GETMESSAGE(21, 12, "Size (in bytes): ")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);            n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, permissionForm);            n++;
   XtSetArg (args[n], XmNtopOffset, 10);                        n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);        n++;
   XtSetArg (args[n], XmNleftOffset, 5);                        n++;
   XtSetArg (args[n], XmNtraversalOn, False);                   n++;
   sizeLabel = XmCreateLabelGadget (form, "size_label", args, n);
   XtManageChild (sizeLabel);
   XmStringFree (label_string);
   XtAddCallback(sizeLabel, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PROPERTIES_DIALOG_STR);

   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET);      n++;
   XtSetArg (args[n], XmNleftWidget, sizeLabel);                n++;
   XtSetArg (args[n], XmNleftOffset, 0);                        n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, permissionForm);            n++;
   XtSetArg (args[n], XmNtopOffset, 10);                        n++;
   XtSetArg (args[n], XmNtraversalOn, False);                   n++;
   size = XmCreateLabelGadget (form, "size", args, n);
   XtManageChild (size);
   XtAddCallback(size, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PROPERTIES_DIALOG_STR);


   /* Create the field displaying the modification time */

   label_string = XmStringCreateLocalized (((char *)
             GETMESSAGE(21, 13, "Modified: ")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);            n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET);      n++;
   XtSetArg (args[n], XmNleftWidget, size);                     n++;
   XtSetArg (args[n], XmNleftOffset, 20);                       n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, permissionForm);            n++;
   XtSetArg (args[n], XmNtopOffset, 10);                        n++;
   XtSetArg (args[n], XmNtraversalOn, False);                   n++;
   modLabel = XmCreateLabelGadget (form, "modify_date_label", args, n);
   XtManageChild (modLabel);
   XmStringFree (label_string);
   XtAddCallback(modLabel, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PROPERTIES_DIALOG_STR);

   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET);      n++;
   XtSetArg (args[n], XmNleftOffset, 0);                        n++;
   XtSetArg (args[n], XmNleftWidget, modLabel);                 n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, permissionForm);            n++;
   XtSetArg (args[n], XmNtopOffset, 10);                        n++;
   XtSetArg (args[n], XmNtraversalOn, False);                   n++;
   mod = XmCreateLabelGadget (form, "modify_date", args, n);
   XtManageChild (mod);
   XtAddCallback(mod, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PROPERTIES_DIALOG_STR);


   /*  Create a separator between the buttons  */

   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);        n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);       n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, sizeLabel);                 n++;
   XtSetArg (args[n], XmNtopOffset, 5);                         n++;
   XtSetArg (args[n], XmNtraversalOn, False);                   n++;
   separator =  XmCreateSeparatorGadget (form, "separator", args, n);
   XtManageChild (separator);

   /*  Create the ok, cancel and help buttons  */

   n = 0;
   XtSetArg (args[n], XmNlabelString, okXmString);              n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);    n++;
   XtSetArg (args[n], XmNleftPosition, 2);                      n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);   n++;
   XtSetArg (args[n], XmNrightPosition, 32);                    n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, separator);                 n++;
   XtSetArg (args[n], XmNtopOffset, 5);                         n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);      n++;
   XtSetArg (args[n], XmNbottomOffset, 5);                      n++;
   XtSetArg (args[n], XmNmarginHeight, 4);                      n++;
   XtSetArg (args[n], XmNuserData, flag);                        n++;
   ok = XmCreatePushButtonGadget (form, "ok", args, n);
   XtManageChild (ok);
   XtAddCallback(ok, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PROPERTIES_DIALOG_STR);


   /* Set the default action */
   XtSetArg (args[0], XmNdefaultButton, ok);
   XtSetValues (form, args, 1);


   n = 0;
   XtSetArg (args[n], XmNlabelString, cancelXmString);          n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);    n++;
   XtSetArg (args[n], XmNleftPosition, 35);                     n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);   n++;
   XtSetArg (args[n], XmNrightPosition, 65);                    n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, separator);                 n++;
   XtSetArg (args[n], XmNtopOffset, 5);                         n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);      n++;
   XtSetArg (args[n], XmNbottomOffset, 5);                      n++;
   XtSetArg (args[n], XmNmarginHeight, 4);                      n++;
   XtSetArg (args[n], XmNuserData, flag);                        n++;
   cancel = XmCreatePushButtonGadget (form, "cancel", args, n);
   XtManageChild (cancel);
   XtAddCallback(cancel, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PROPERTIES_DIALOG_STR);


   n = 0;
   XtSetArg (args[n], XmNlabelString, helpXmString);            n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);    n++;
   XtSetArg (args[n], XmNleftPosition, 68);                     n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);   n++;
   XtSetArg (args[n], XmNrightPosition, 98);                    n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, separator);                 n++;
   XtSetArg (args[n], XmNtopOffset, 5);                         n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);      n++;
   XtSetArg (args[n], XmNbottomOffset, 5);                      n++;
   XtSetArg (args[n], XmNmarginHeight, 4);                      n++;
   help = XmCreatePushButtonGadget (form, "help", args, n);
   XtManageChild (help);
   XtAddCallback(help, XmNactivateCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PROPERTIES_DIALOG_STR);
   XtAddCallback(help, XmNhelpCallback, (XtCallbackProc)HelpRequestCB, 
                 HELP_PROPERTIES_DIALOG_STR);


   XtSetArg (args[0], XmNcancelButton, cancel);
   XtSetValues (form, args, 1);

   /* Fill in our instance structure */

   modAttr_rec->shell = shell;
   modAttr_rec->bboard = form;
   modAttr_rec->name = name;
   modAttr_rec->link_name = link_name;
   modAttr_rec->link_label = link_label;
   modAttr_rec->fileType = fileType;
   modAttr_rec->owner_label = ownerLabel;
   modAttr_rec->owner = owner;
   modAttr_rec->group = group;
   modAttr_rec->ownerL = ownerL;
   modAttr_rec->groupL = groupL;
   modAttr_rec->ownerRead = ownerRead;
   modAttr_rec->ownerWrite = ownerWrite;
   modAttr_rec->ownerExecute = ownerExecute;
   modAttr_rec->groupRead = groupRead;
   modAttr_rec->groupWrite = groupWrite;
   modAttr_rec->groupExecute = groupExecute;
   modAttr_rec->otherRead = otherRead;
   modAttr_rec->otherWrite = otherWrite;
   modAttr_rec->otherExecute = otherExecute;
   modAttr_rec->otherPLabel = otherPLabel;
   modAttr_rec->fsWarning = fsWarning;
   modAttr_rec->fsPushButton = fsPushButton;
   modAttr_rec->size = size;
   modAttr_rec->modTime = mod;
   modAttr_rec->ok = ok;
   modAttr_rec->cancel = cancel;
   modAttr_rec->help = help;


   /*  Set the return values for the dialog widget and dialog instance.  */

   *return_widget = form;
   *dialog = (XtPointer) modAttr_rec;
}




/************************************************************************
 *
 *  InstallChange
 *
 ************************************************************************/

static void
InstallChange(
        ModAttrRec *modAttr_rec,
        XtCallbackProc callback,
        XtPointer client_data )
{
   ModAttrApply * apply_data;


   /*  Setup the callback data to be sent to the Ok callback.   */
   /*  This contains the encapsulation callback to invoke upon  */
   /*  the data within the dialog being changed.                */

   apply_data = (ModAttrApply *) XtMalloc (sizeof (ModAttrApply));
   apply_data->callback = callback;
   apply_data->client_data = client_data;
   apply_data->modAttr_rec = (XtPointer) modAttr_rec;
   modAttr_rec->apply_data = apply_data;


   XtAddCallback (modAttr_rec->ok, XmNactivateCallback, 
                  (XtCallbackProc)OkCallback, (XtPointer) apply_data);
}




/************************************************************************
 *
 *  InstallClose
 *
 ************************************************************************/

static void
InstallClose(
        ModAttrRec *modAttr_rec,
        XtCallbackProc callback,
        XtPointer client_data )
{
   Atom delete_window_atom;

   XtAddCallback (modAttr_rec->cancel,
                  XmNactivateCallback, callback, client_data);

   /* This callback is removed in SetValues if the config file does  */
   /* not include a dismiss=yes directive. Thus for some filesystems */
   /* you can Cancel the regular permissions dialog when the fs      */
   /* dialog button is pressed (e.g. for DFS?) or you can let the    */
   /* the regular permissions dialog stay on screen along with the   */
   /* fs-specfic dialog.                                             */
  if (modAttr_rec->fsPushButton != NULL)
      XtAddCallback (modAttr_rec->fsPushButton,
                     XmNactivateCallback, callback, client_data);

   delete_window_atom = XmInternAtom (XtDisplay(modAttr_rec->shell),
                                      "WM_DELETE_WINDOW", True);
   XmRemoveWMProtocols( modAttr_rec->shell, &delete_window_atom, 1 );
   XmAddWMProtocolCallback( modAttr_rec->shell, delete_window_atom, callback, 
                            (XtPointer) client_data );
}




/************************************************************************
 *
 *  Destroy
 *
 ************************************************************************/

static void
Destroy(
        ModAttrRec *modAttr_rec )
{
   XtDestroyWidget (modAttr_rec->shell);
   XtFree ((char *) modAttr_rec->apply_data);
   XtFree ((char *) modAttr_rec);
}




/************************************************************************
 *
 *  GetValues
 *
 ************************************************************************/

static XtPointer
GetValues(
        ModAttrRec *modAttr_rec )
{
   ModAttrData * modAttr_data;


   /*  Allocate and initialize the modify file attribute dialog data.  */

   modAttr_data = (ModAttrData *) XtCalloc (1, sizeof (ModAttrData));

   modAttr_data->displayed = True;

   (void) GetModAttrValues(modAttr_rec, modAttr_data, False);

   return ((XtPointer) modAttr_data);
}




/************************************************************************
 *
 *  GetDefaultValues
 *
 ************************************************************************/

static XtPointer
GetDefaultValues( void )
{
   ModAttrData * modAttr_data;


   /*  Allocate and initialize the default file attribute dialog data.  */

   modAttr_data = (ModAttrData *) XtCalloc (1, sizeof (ModAttrData));

   modAttr_data->displayed = False;
   modAttr_data->x = 0;
   modAttr_data->y = 0;
   modAttr_data->height = 0;
   modAttr_data->width = 0;

   modAttr_data->name = NULL;
   modAttr_data->link_name = NULL;
   modAttr_data->filetype = NULL;
   modAttr_data->pixmapData = NULL;
   modAttr_data->owner = NULL;
   modAttr_data->group = NULL;
   modAttr_data->accessBits = 0;
   modAttr_data->setuidBits = 0;
   modAttr_data->size = 0;
   modAttr_data->modTime = time(NULL);
   modAttr_data->host = NULL;
   modAttr_data->directory = NULL;

   return ((XtPointer) modAttr_data);
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

   /* We are not using this code so we commented it out for now */


   ModAttrData * modAttr_data;
   String host, directory, name;
  

   /*  Allocate and get the resources for modify file attrs dialog data.  */

   modAttr_data = (ModAttrData *) XtCalloc (1, sizeof (ModAttrData));
  
   _DtDialogGetResources (data_base, name_list, MODIFY_ATTR, (char *)modAttr_data,
                       resources, modAttrClass->resource_count);
   

   modAttr_data->link_name = XtNewString(modAttr_data->link_name);

   /* 
    * Since the other fields are dynamic, use the obtained name to
    * stat the file, and use the current values.  If the file no
    * longer exists, then return the default values.
    */
   (void) _LoadFileAttributes (modAttr_data->host, modAttr_data->directory, 
                               modAttr_data->name, modAttr_data,
                               ALL_BUT_ACCESS);
   return ((XtPointer) modAttr_data);
}




/************************************************************************
 *
 *  SetValues
 *
 ************************************************************************/

static void
SetValues(
        ModAttrRec *modAttr_rec,
        ModAttrData *modAttr_data )
{
   Arg args[10];
   XmString string;
   char buf[20];
   char date_string[256];
   Arg trueArg[1];
   Arg falseArg[1];
   Boolean sensitivity;
   struct passwd * user_data;
   String temp;
   struct tm * tms;
   char * timeString;
   int mode;
   char * fullPath;
   uid_t effective_user;
   int n;

   Pixel background;
   Pixel foreground;
   Pixel top_shadow;
   Pixel bottom_shadow;
   Pixel select;
   Colormap colormap;
   XmString labelString;
   char * tmpStr;
   Tt_status tt_status;

   /*  Get the select color  */

   XtSetArg (args[0], XmNbackground, &background);
   XtSetArg (args[1], XmNcolormap,  &colormap);
   XtGetValues (modAttr_rec->bboard, args, 2);

   XmGetColors (XtScreen (modAttr_rec->bboard), colormap, background,
                &foreground, &top_shadow, &bottom_shadow, &select);

   /* Adjust the colors of the icon */
   SetToNormalColors (modAttr_rec->fileType, modAttr_rec->bboard, 
                      modAttr_data->main_widget, 0);

   /* Verify that we are allowed to change the attributes */
   if (modAttr_data->owner)
      user_data = getpwnam(modAttr_data->owner);
   else
      user_data = NULL;
   effective_user = geteuid();


   if (user_data && user_data->pw_uid == getuid()
       || effective_user == root_user)
   {
      /* Check for root user */
      if (effective_user == root_user)
      {
        fullPath = ResolveLocalPathName (modAttr_data->host, 
                                         modAttr_data->directory,
                                         modAttr_data->name,
                                         home_host_name, &tt_status);

        if( TT_OK == tt_status )
        {
          /* Check for access rights (e.g. NFS mounted filesystem)  */
          mode  = chmod(fullPath, modAttr_data->accessBits +
                        modAttr_data->setuidBits);
          XtFree(fullPath);

          if (mode == 0)
            sensitivity = True;
          else
            sensitivity = False;
        }
        else
          sensitivity = False;
      }
      else
        sensitivity = True;
   }
   else
     sensitivity = False;

   if (modAttr_data->link_name)
   {
      Widget children[2];

      children[0] = modAttr_rec->link_name;
      children[1] = modAttr_rec->link_label;
      XtManageChildren(children, 2);
      XtSetArg(args[0], XmNtopWidget, modAttr_rec->link_name);
      XtSetValues(modAttr_rec->owner, args, 1);
      XtSetValues(modAttr_rec->owner_label, args, 1);
      XtSetValues(modAttr_rec->fileType, args, 1);
   }
   else
   {
      Widget children[2];

      children[0] = modAttr_rec->link_name;
      children[1] = modAttr_rec->link_label;
      XtUnmanageChildren(children, 2);
      XtSetArg(args[0], XmNtopWidget, modAttr_rec->name);
      XtSetValues(modAttr_rec->owner, args, 1);
      XtSetValues(modAttr_rec->owner_label, args, 1);
      XtSetValues(modAttr_rec->fileType, args, 1);
   }

 
   if (sensitivity == True) 
   {
      /* Only allow root user to change the ownership.
      */
      if (effective_user == root_user)
      {
        XtManageChild(modAttr_rec->owner);
        XmTextFieldSetEditable(modAttr_rec->owner, True);
        XtSetArg (args[0], XmNcursorPositionVisible, True);
        XtSetArg (args[1], XmNshadowThickness, 2);
        XtSetValues(modAttr_rec->owner, args, 2);

        if (modAttr_data->owner)
	{
	  if (getpwnam(modAttr_data->owner) == NULL)
	  {
            char *newstring =  XtMalloc(strlen(modAttr_data->owner) +
                                     strlen(GETMESSAGE(21,19, "Unknown")) + 10);
            sprintf( newstring,"%s (%s) ",modAttr_data->owner,
                     GETMESSAGE(21,19, "Unknown") );
            XmTextFieldSetString(modAttr_rec->owner, newstring);
            XtFree( newstring );
	  }
	  else
            XmTextFieldSetString(modAttr_rec->owner, modAttr_data->owner);
	}
        else
          XmTextFieldSetString(modAttr_rec->owner, 
                               (GETMESSAGE(21,19, "Unknown")));

        XtUnmanageChild(modAttr_rec->ownerL);
        XtUnmanageChild(modAttr_rec->groupL);
        XtManageChild(modAttr_rec->group);
        XmTextFieldSetEditable(modAttr_rec->group, True);

        XtSetArg (args[0], XmNcursorPositionVisible, True);
        XtSetArg (args[1], XmNshadowThickness, 2);
        XtSetValues(modAttr_rec->group, args, 2);

        XtSetArg (args[0], XmNshadowType, XmSHADOW_IN);
        XtSetArg (args[1], XmNbackground, select);
        XtSetValues(XtParent (modAttr_rec->ownerRead), args, 2);

      }
      else
      {
        XtUnmanageChild(modAttr_rec->owner);

        if (modAttr_data->owner)
        {
          if (getpwnam(modAttr_data->owner) == NULL)
          {
            char *newstring =  XtMalloc(strlen(modAttr_data->owner) + 
                                        strlen(GETMESSAGE(21,19, "Unknown")) + 10);
            sprintf( newstring,"%s ( %s ) ",modAttr_data->owner,
                     GETMESSAGE(21,19, "Unknown") );
            labelString = XmStringCreateLocalized( newstring );
            XtFree( newstring );
          }
          else
            labelString = XmStringCreateLocalized(modAttr_data->owner);
        }
        else
        {
          labelString = XmStringCreateLocalized((GETMESSAGE(21,19, "Unknown")));
        }

        XtSetArg (args[0], XmNlabelString, labelString);
        XtSetValues(modAttr_rec->ownerL, args, 1);
        XmStringFree (labelString);
        XtManageChild(modAttr_rec->ownerL);

        XtUnmanageChild(modAttr_rec->group);
        if (modAttr_data->group)
        {
         if (getgrnam(modAttr_data->group) == NULL)
         {
            char *newstring =  XtMalloc(strlen(modAttr_data->group) +
               strlen(GETMESSAGE(21,19, "Unknown")) + 10);
            sprintf(newstring,"%s ( %s ) ",modAttr_data->group,
               GETMESSAGE(21,19, "Unknown"));
            labelString = XmStringCreateLocalized(newstring);
            XtFree(newstring);
         }
         else
            labelString = XmStringCreateLocalized(modAttr_data->group);
        }
        else
        {
         labelString = XmStringCreateLocalized((GETMESSAGE(21,19, "Unknown")));
        }
        XtSetArg (args[0], XmNlabelString, labelString);
        XtSetValues(modAttr_rec->groupL, args, 1);
        XmStringFree (labelString);
        XtManageChild(modAttr_rec->groupL);

        XtSetArg (args[0], XmNshadowType, XmSHADOW_ETCHED_IN);
        XtSetArg (args[1], XmNbackground, background);
        XtSetValues(XtParent (modAttr_rec->ownerRead), args, 2);

      }

      if (modAttr_data->group)
      {
        if(getgrnam(modAttr_data->group) == NULL)
        {
          char *newstring =  XtMalloc(strlen(modAttr_data->group) + 
                                      strlen(GETMESSAGE(21,19, "Unknown")) + 10);
          sprintf(newstring,"%s (%s) ",modAttr_data->group,
                  GETMESSAGE(21,19, "Unknown"));
          XmTextFieldSetString(modAttr_rec->group, newstring);
          XtFree(newstring);
        }
        else
          XmTextFieldSetString(modAttr_rec->group, modAttr_data->group);
      }
      else
      {
         XmTextFieldSetString(modAttr_rec->group, 
                              (GETMESSAGE(21,19, "Unknown")));
      }
   }
   else
   {
      XtUnmanageChild(modAttr_rec->owner);
      XtUnmanageChild(modAttr_rec->group);
/*    I'm not sure why we care to set all these up.
      Both widgets are being unmanaged by the above 2 statements

      XmTextFieldSetEditable(modAttr_rec->owner, False);
      XmTextFieldSetEditable(modAttr_rec->group, False);
      XtSetArg (args[0], XmNcursorPositionVisible, False);
      XtSetArg (args[1], XmNshadowThickness, 0);
      XtSetArg (args[2], XmNbackground, background);
      XtSetValues(modAttr_rec->owner, args, 3);
      XtSetValues(modAttr_rec->group, args, 3);
*/

      if (modAttr_data->owner)
      {
         if (getpwnam(modAttr_data->owner) == NULL)
         {
            char *newstring =  XtMalloc(strlen(modAttr_data->owner) + 
               strlen(GETMESSAGE(21,19, "Unknown")) + 10);
            sprintf(newstring,"%s ( %s ) ",modAttr_data->owner,
               GETMESSAGE(21,19, "Unknown"));
            labelString = XmStringCreateLocalized(newstring);
            XtFree(newstring);
         }
         else
            labelString = XmStringCreateLocalized(modAttr_data->owner);
      }
      else
      {
         labelString = XmStringCreateLocalized((GETMESSAGE(21,19, "Unknown")));
      }
      XtSetArg (args[0], XmNlabelString, labelString);
      XtSetValues(modAttr_rec->ownerL, args, 1);
      XmStringFree (labelString);

      if (modAttr_data->group)
      {
         if (getgrnam(modAttr_data->group) == NULL)
         {
            char *newstring =  XtMalloc(strlen(modAttr_data->group) + 
               strlen(GETMESSAGE(21,19, "Unknown")) + 10);
            sprintf(newstring,"%s ( %s ) ",modAttr_data->group,
               GETMESSAGE(21,19, "Unknown"));
            labelString = XmStringCreateLocalized(newstring);
            XtFree(newstring);
         }
         else
            labelString = XmStringCreateLocalized(modAttr_data->group);
      }
      else
      {
         labelString = XmStringCreateLocalized((GETMESSAGE(21,19, "Unknown")));
      }
      XtSetArg (args[0], XmNlabelString, labelString);
      XtSetValues(modAttr_rec->groupL, args, 1);
      XmStringFree (labelString);
      XtManageChild(modAttr_rec->ownerL);
      XtManageChild(modAttr_rec->groupL);

      XtSetArg (args[0], XmNshadowType, XmSHADOW_ETCHED_IN);
      XtSetArg (args[1], XmNbackground, background);
      XtSetValues(XtParent (modAttr_rec->ownerRead), args, 2);
   }


   if(modAttr_data->filetype && (
               strcmp(modAttr_data->filetype,LT_RECURSIVE_LINK) == 0 ||
               strcmp(modAttr_data->filetype,LT_BROKEN_LINK) == 0))
      sensitivity = False;

   /* User doesn't have modification privileges */
   XtSetSensitive(modAttr_rec->ok, sensitivity);
   XtSetSensitive(modAttr_rec->ownerRead, sensitivity);
   XtSetSensitive(modAttr_rec->ownerWrite, sensitivity);
   XtSetSensitive(modAttr_rec->ownerExecute, sensitivity);
   XtSetSensitive(modAttr_rec->groupRead, sensitivity);
   XtSetSensitive(modAttr_rec->groupWrite, sensitivity);
   XtSetSensitive(modAttr_rec->groupExecute, sensitivity);
   XtSetSensitive(modAttr_rec->otherRead, sensitivity);
   XtSetSensitive(modAttr_rec->otherWrite, sensitivity);
   XtSetSensitive(modAttr_rec->otherExecute, sensitivity);


   if (modAttr_data->name)
   {
      /* Build host:dir/name string */
      temp = XtMalloc(strlen(modAttr_data->host) + 
                      strlen(modAttr_data->directory) +
                      strlen(modAttr_data->name) + 3);
      temp[0] = '\0';
      if (modAttr_data->host)
      {
         (void) strcat(temp, modAttr_data->host);
         (void) strcat(temp, ":");
      }
      if (modAttr_data->directory)
      {
         (void) strcat(temp, modAttr_data->directory);
         if (strcmp (modAttr_data->directory, "/") != 0)
            (void) strcat(temp, "/");
      }
      (void) strcat(temp, modAttr_data->name);
      string = XmStringCreateLocalized(temp);
      XtFree ((char *) temp);
   }
   else
      string = XmStringCreateLocalized(" ");


   XtSetArg (args[0], XmNlabelString, string);
   XtSetValues(modAttr_rec->name, args, 1);
   XmStringFree(string);

   if (modAttr_data->link_name)
   {
      string = XmStringCreateLocalized(modAttr_data->link_name);
      XtSetArg (args[0], XmNlabelString, string);
      XtSetValues(modAttr_rec->link_name, args, 1);
      XmStringFree(string);
   }

   if (modAttr_data->filetype)
   {
      char *ptr;

      if (ptr = (char *)DtDtsDataTypeToAttributeValue(modAttr_data->filetype,
                                                      FM_TYPE_LABEL,
                                                      NULL))
      {
         string = XmStringCreateLocalized(ptr);
         DtDtsFreeAttributeValue(ptr);
      }
      else
      {
         string = XmStringCreateLocalized(modAttr_data->filetype);
      }
   }
   else
      string = XmStringCreateLocalized(" ");

   XtSetArg (args[0], XmNstring, string);
   if (modAttr_data->pixmapData)
     XtSetArg (args[1], XmNimageName, modAttr_data->pixmapData->iconFileName);
   else
     XtSetArg (args[1], XmNimageName, NULL);
   XtSetValues(modAttr_rec->fileType, args, 2);

   _DtCheckAndFreePixmapData(modAttr_data->filetype,
                             toplevel,
                             (DtIconGadget) modAttr_rec->fileType,
                             modAttr_data->pixmapData);
   modAttr_data->pixmapData = NULL;
   XmStringFree(string);
   fullPath = ResolveLocalPathName (modAttr_data->host, 
                                    modAttr_data->directory,
                                    modAttr_data->name,
                                    home_host_name, &tt_status);
   if (fsDialogAvailable(fullPath, &modAttr_rec->fileSystem))
   {
      if (strlen(modAttr_rec->fileSystem.warningMessage) == 0) /* fs dialog is available and there is no warning message */
      {
         string = XmStringCreateLocalized (modAttr_rec->fileSystem.buttonLabel);
         n = 0;
         XtSetArg (args[n], XmNlabelString, string);			n++;
         XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
         XtSetArg (args[n], XmNtopWidget, modAttr_rec->otherPLabel);	n++;
         XtSetArg (args[n], XmNtopOffset, 15);				n++;
         XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);	n++;
         XtSetArg (args[n], XmNbottomOffset, 15);			n++;
         XtSetValues(modAttr_rec->fsPushButton, args, n);
         XmStringFree(string);
      }
      else /* fs dialog is available and there is a warning message */
      {
         n = 0;
         string = XmStringCreateLocalized (modAttr_rec->fileSystem.warningMessage);
         XtSetArg (args[n], XmNlabelString, string);			n++;
         XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
         XtSetArg (args[n], XmNtopWidget, modAttr_rec->otherPLabel);	n++;
         XtSetArg (args[n], XmNtopOffset, 15);				n++;
         XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);		n++;
         XtSetArg (args[n], XmNrightOffset, 15);			n++;
         XtSetValues(modAttr_rec->fsWarning, args, n);
         XmStringFree(string);
         n = 0;
         string = XmStringCreateLocalized (modAttr_rec->fileSystem.buttonLabel);
         XtSetArg (args[n], XmNlabelString, string);			n++;
         XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
         XtSetArg (args[n], XmNtopWidget, modAttr_rec->fsWarning);	n++;
         XtSetArg (args[n], XmNtopOffset, 15);				n++;
         XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);	n++;
         XtSetArg (args[n], XmNbottomOffset, 15);			n++;
         XtSetValues(modAttr_rec->fsPushButton, args, n);
         XmStringFree(string);
     }
     n = 0;
     XtSetArg (args[n], XmNbottomAttachment, XmATTACH_NONE); 		n++;
     XtSetValues(modAttr_rec->otherPLabel, args, n);
     if (strlen(modAttr_rec->fileSystem.warningMessage) != 0)
        XtManageChild (modAttr_rec->fsWarning);
     else
        XtUnmanageChild (modAttr_rec->fsWarning);
     /* the fs dialog pushbutton is created with a callback to cancel the   */
     /* permissions dialog ... here that callback is removed if that was    */
     /* requested by a directive in the configuration file dtfile.config    */
     if (!modAttr_rec->fileSystem.dismissStdPermissionDialog)
     {
        XtRemoveAllCallbacks(modAttr_rec->fsPushButton, XmNactivateCallback);
     }
     XtAddCallback(modAttr_rec->fsPushButton,
                   XmNactivateCallback,
                   (XtCallbackProc)displayFSDialog,
                   &modAttr_rec->fileSystem);
     XtManageChild (modAttr_rec->fsPushButton);
   }
   else
   {
      if (strlen(modAttr_rec->fileSystem.warningMessage) == 0) /* fs dialog not available and there is no warning message */
      {
         XtUnmanageChild (modAttr_rec->fsPushButton);
         XtUnmanageChild (modAttr_rec->fsWarning);
         n = 0;
         XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);	n++;
         XtSetValues(modAttr_rec->otherPLabel, args, n);
      }
      else /* fs dialog is not available and there is a warning message */
      {
         n = 0;
         XtSetArg (args[n], XmNbottomAttachment, XmATTACH_NONE); 	n++;
         XtSetValues(modAttr_rec->otherPLabel, args, n);
         n = 0;
         string = XmStringCreateLocalized (modAttr_rec->fileSystem.warningMessage);
         XtSetArg (args[n], XmNlabelString, string);			n++;
         XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
         XtSetArg (args[n], XmNtopWidget, modAttr_rec->otherPLabel);	n++;
         XtSetArg (args[n], XmNtopOffset, 15);				n++;
         XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);	n++;
         XtSetArg (args[n], XmNbottomOffset, 15);			n++;
         XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);		n++;
         XtSetArg (args[n], XmNrightOffset, 15);			n++;
         XtSetValues(modAttr_rec->fsWarning, args, n);
         XmStringFree(string);
         XtUnmanageChild (modAttr_rec->fsPushButton);
         XtManageChild (modAttr_rec->fsWarning);
         XtUnmanageChild (modAttr_rec->fsPushButton);
      }
   }

   XtFree(fullPath);


   if(modAttr_data->filetype && (
               strcmp(modAttr_data->filetype,LT_RECURSIVE_LINK) == 0 ||
               strcmp(modAttr_data->filetype,LT_BROKEN_LINK) == 0 ))
   {
     tmpStr = GETMESSAGE(21,19, "Unknown");
     sprintf(date_string, "  %s",tmpStr);
     string = XmStringCreateLocalized(date_string);
     XtSetArg (args[0], XmNlabelString, string);
     XtSetValues(modAttr_rec->size, args, 1);
     XtSetArg (args[0], XmNlabelString, string);
     XtSetValues(modAttr_rec->modTime, args, 1);
     XmStringFree(string);
   }
   else
   {
     (void) sprintf(buf, "%d", modAttr_data->size);
     string = XmStringCreateLocalized(buf);
     XtSetArg (args[0], XmNlabelString, string);
     XtSetValues(modAttr_rec->size, args, 1);
     XmStringFree(string);

     tms = localtime(&modAttr_data->modTime);
     tmpStr = GETMESSAGE(21, 14, "%x %X ");
     timeString = XtNewString(tmpStr);
     (void)strftime(date_string, 256, timeString, tms);
     XtFree(timeString);
     string = XmStringCreateLocalized(date_string);
     XtSetArg (args[0], XmNlabelString, string);
     XtSetValues(modAttr_rec->modTime, args, 1);
     XmStringFree(string);
   }

   XtSetArg (trueArg[0], XmNset, True);
   XtSetArg (falseArg[0], XmNset, False);

   /* Set the access bits */
   if (modAttr_data->accessBits & S_IRUSR)
      XtSetValues(modAttr_rec->ownerRead, trueArg, 1);
   else
      XtSetValues(modAttr_rec->ownerRead, falseArg, 1);

   if (modAttr_data->accessBits & S_IWUSR)
      XtSetValues(modAttr_rec->ownerWrite, trueArg, 1);
   else
      XtSetValues(modAttr_rec->ownerWrite, falseArg, 1);

   if (modAttr_data->accessBits & S_IXUSR)
      XtSetValues(modAttr_rec->ownerExecute, trueArg, 1);
   else
      XtSetValues(modAttr_rec->ownerExecute, falseArg, 1);

   if (modAttr_data->accessBits & S_IRGRP)
      XtSetValues(modAttr_rec->groupRead, trueArg, 1);
   else
      XtSetValues(modAttr_rec->groupRead, falseArg, 1);

   if (modAttr_data->accessBits & S_IWGRP)
      XtSetValues(modAttr_rec->groupWrite, trueArg, 1);
   else
      XtSetValues(modAttr_rec->groupWrite, falseArg, 1);

   if (modAttr_data->accessBits & S_IXGRP)
      XtSetValues(modAttr_rec->groupExecute, trueArg, 1);
   else
      XtSetValues(modAttr_rec->groupExecute, falseArg, 1);

   if (modAttr_data->accessBits & S_IROTH)
      XtSetValues(modAttr_rec->otherRead, trueArg, 1);
   else
      XtSetValues(modAttr_rec->otherRead, falseArg, 1);

   if (modAttr_data->accessBits & S_IWOTH)
      XtSetValues(modAttr_rec->otherWrite, trueArg, 1);
   else
      XtSetValues(modAttr_rec->otherWrite, falseArg, 1);

   if (modAttr_data->accessBits & S_IXOTH)
      XtSetValues(modAttr_rec->otherExecute, trueArg, 1);
   else
      XtSetValues(modAttr_rec->otherExecute, falseArg, 1);
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

   /* We are not using this code so we commented it out for now */


   ModAttrData * modAttr_data = (ModAttrData *) values->data;
   ModAttrRec  * modAttr_rec;
   Arg args[4];
   

   /*  If the dialog is currently displayed, update the geometry  */
   /*  fields to their current values.                            */

   if (modAttr_data->displayed == True)
   {
      _DtGenericUpdateWindowPosition(values);
      modAttr_rec = (ModAttrRec *) _DtGetDialogInstance(values);
   }

   _DtDialogPutResources (fd, name_list, MODIFY_ATTR, values->data, 
                       resources, modAttrClass->resource_count);
}




/************************************************************************
 *
 *  FreeValues
 *
 ************************************************************************/

static void
FreeValues(
        ModAttrData *modAttr_data )
{
   XtFree ((char *) modAttr_data->host);
   XtFree ((char *) modAttr_data->directory);
   XtFree ((char *) modAttr_data->name);
   XtFree ((char *) modAttr_data->link_name);
   XtFree ((char *) modAttr_data->filetype);
   XtFree ((char *) modAttr_data->owner);
   XtFree ((char *) modAttr_data->group);

   if( modAttr_data->pixmapData )
   {
     DtDtsFreeAttributeValue(modAttr_data->pixmapData->hostPrefix);
     DtDtsFreeAttributeValue(modAttr_data->pixmapData->instanceIconName);
     DtDtsFreeAttributeValue(modAttr_data->pixmapData->iconName);
     DtDtsFreeAttributeValue(modAttr_data->pixmapData->iconFileName);
     XtFree((char *) modAttr_data->pixmapData);
   }
   XtFree ((char *) modAttr_data);
}




/************************************************************************
 *
 *  OkCallback
 *	This is a callback function called when the (Ok) button
 *	is pressed.  It makes any indicated modifications, and then
 *	discards the data and brings down the dialog.
 *
 ************************************************************************/

static XtCallbackProc
OkCallback(
        Widget w,
        ModAttrApply *apply_data,
        XtPointer call_data )
{
  ModAttrRec  * modAttr_rec = (ModAttrRec *) apply_data->modAttr_rec;
  ModAttrData * new_attr_data;
  FileMgrRec *file_mgr_rec;
  Widget close = modAttr_rec->cancel;
  DialogData * old_dialog;
  ModAttrData * old;
  Boolean question = False;
  short result;

  old_dialog = _DtGetInstanceData ((XtPointer)modAttr_rec);
  old = (ModAttrData *) old_dialog->data;

  if(RecheckFlag(NULL,w))
    return None;
  else if(RecheckFlag(NULL,close))
    return None;

  new_attr_data = (ModAttrData *) XtCalloc (1, sizeof (ModAttrData));
  new_attr_data->displayed = True;

  /* Verify the user and group names */
  result = GetModAttrValues(modAttr_rec, new_attr_data, True);
  if (result == GMA_VALID)
  {
    /* Check to see if the file we are changing is the CURRENT directory.
     * If it is we need to see if after the changes, the user will have
     * permissions to read this folder. If he/she doesn't we want to warn
     * the user that this will be the case and allow them to back out of
     * the change.  If they don't want to back out of the change, we need
     * to make the change, then move them up to the parent directory.
     */
    if(strcmp(old->name, ".") == 0)
    {
      /* It is the current directory, now we need to test the cases where
       * by changing permissions the user would no should be in this 
       * directory because lack of priviledges.
       * Cases where this is true:
       *   1. If the execute bit for owner has been changed to off and the 
       *      user is the owner.
       *   2. The user changes the owner, and the execute bit is for owner
       *      is the only execute bit set.
       *   3. The read access bit for owner has been turned off.
       *
       *   Others?
       */
      if(strcmp(old->owner, new_attr_data->owner) == 0 && 
         (new_attr_data->accessBits & 64) == 0)
      {
        /* Criteria 1 above has been meet */
        question = True;
      }
      else if (strcmp(old->owner, new_attr_data->owner) != 0 &&
               ((new_attr_data->accessBits & 73) == 0 ||
                (new_attr_data->accessBits & 73) == 64))
      {
        /* Criteria 2 above has been meet */
          question = True;
      }
      else if((new_attr_data->accessBits & 256) == 0)
      {
          /* Criteria 3 above has been meet */
          question = True;
      }
    }

    if(question)
    {
      char *title, *message;

      globalApplyData = apply_data; 
      parentDir = XtNewString(old->directory); 
      title = XtNewString(GETMESSAGE(21, 36, "Change Permissions Warning")); 
      message = XtNewString(GETMESSAGE(21, 37, "You are changing the permissions on the Current\nFolder in a way that won't allow you to view\nthis folder any longer.  It will be automatically\nclosed and you will no longer be able to reopen\nthe folder.\n\nIs this what you want to do?"));
      _DtMessageDialog(modAttr_rec->shell, title, message, NULL, TRUE,
                       DontChangeCB, ChangeCB, NULL, HelpRequestCB, False,
                       QUESTION_DIALOG);
      XtFree(title);
      XtFree(message);
    }
    else
      /* Information was valid */
      (*(apply_data->callback)) (w, apply_data->client_data, NULL);
  }
  else if (result == GMA_NO_FILE)
  {
     DialogData * old_dialog;
     char *title,*tmpmsg,*msg;
     ModAttrData * old;

     old_dialog = _DtGetInstanceData ((XtPointer)modAttr_rec);
     old = (ModAttrData *) old_dialog->data;

     file_mgr_rec =  (FileMgrRec *) LocateRecordStructure(w);
     ResetFlag(NULL,w);
     ResetFlag(NULL,close);
     title  = XtNewString(GETMESSAGE(21,33, "Permissions Error"));
     tmpmsg = GETMESSAGE(21,35,"The Permissions dialog for the object\n\n%s/%s\n\nhas been closed because this object has been deleted\nfrom your system.");
     msg = XtMalloc(strlen(tmpmsg)+strlen(old->directory)+strlen(old->name)+3);
     sprintf(msg,tmpmsg,old->directory,old->name);

     ModAttrClose ((XtPointer)file_mgr_rec, old_dialog, NULL);
     _DtMessage(toplevel, title, msg, NULL, HelpRequestCB);
     XtFree(title);
     XtFree(msg);
  }

  FreeValues (new_attr_data);
}



/************************************************************************
 *
 *  GetModAttrValues
 *	Update the current file attribute values within the data structure.
 *
 ************************************************************************/

static short
GetModAttrValues(
        ModAttrRec *modAttr_rec,
        ModAttrData *modAttr_data,
        Boolean validate )
{
   Arg args[4];
   DialogData * old_dialog;
   ModAttrData * old;
   int error;
   Boolean retStatus;

   old_dialog = _DtGetInstanceData ((XtPointer)modAttr_rec);
   old = (ModAttrData *) old_dialog->data;
   retStatus =  _LoadFileAttributes (old->host, old->directory, old->name,
			       modAttr_data, ALL_BUT_OWNER_GROUP_ACCESS);

   if(!retStatus)
     return ((short)GMA_NO_FILE);

   /* Get the current geometry */
   XtSetArg (args[0], XmNx, &modAttr_data->x);
   XtSetArg (args[1], XmNy, &modAttr_data->y);
   XtSetArg (args[2], XmNwidth, &modAttr_data->width);
   XtSetArg (args[3], XmNheight, &modAttr_data->height);
   XtGetValues (modAttr_rec->shell, args, 4);

   /* Get the user, group and access bits */
   if( XtIsManaged( modAttr_rec->owner ) )
     modAttr_data->owner = XmTextFieldGetString(modAttr_rec->owner);
   else
     modAttr_data->owner = XtNewString( old->owner );

   modAttr_data->group = XmTextFieldGetString(modAttr_rec->group);
   modAttr_data->accessBits = 0;

   if (XmToggleButtonGadgetGetState(modAttr_rec->ownerRead))
      modAttr_data->accessBits |= S_IRUSR;
   if (XmToggleButtonGadgetGetState(modAttr_rec->ownerWrite))
      modAttr_data->accessBits |= S_IWUSR;
   if (XmToggleButtonGadgetGetState(modAttr_rec->ownerExecute))
      modAttr_data->accessBits |= S_IXUSR;
   if (XmToggleButtonGadgetGetState(modAttr_rec->groupRead))
      modAttr_data->accessBits |= S_IRGRP;
   if (XmToggleButtonGadgetGetState(modAttr_rec->groupWrite))
      modAttr_data->accessBits |= S_IWGRP;
   if (XmToggleButtonGadgetGetState(modAttr_rec->groupExecute))
      modAttr_data->accessBits |= S_IXGRP;
   if (XmToggleButtonGadgetGetState(modAttr_rec->otherRead))
      modAttr_data->accessBits |= S_IROTH;
   if (XmToggleButtonGadgetGetState(modAttr_rec->otherWrite))
      modAttr_data->accessBits |= S_IWOTH;
   if (XmToggleButtonGadgetGetState(modAttr_rec->otherExecute))
      modAttr_data->accessBits |= S_IXOTH;

   if (old->link_name)
      modAttr_data->link_name = XtNewString(old->link_name);
   else
      modAttr_data->link_name = NULL;


   /* Validate the entry */
   if (validate)
   {
      /* Valid user name */
      error = -1;
      if (getpwnam(modAttr_data->owner) == NULL)
         error = UNKNOWN_USER;
      else if (getgrnam(modAttr_data->group) == NULL)
         error = UNKNOWN_GROUP;

      if (error != -1)
      {
         /* Bad entry; free up everything allocated so far */
         ResetFlag(NULL,modAttr_rec->ok);
         ResetFlag(NULL,modAttr_rec->cancel);
         InvalidModAttrMessage(modAttr_rec->shell, error);
         XtFree ((char *) modAttr_data->host);
          modAttr_data->host = NULL;
         XtFree ((char *) modAttr_data->directory);
          modAttr_data->directory = NULL;
         XtFree ((char *) modAttr_data->name);
          modAttr_data->name = NULL;
         XtFree ((char *) modAttr_data->filetype);
          modAttr_data->filetype = NULL;
         XtFree ((char *) modAttr_data->owner);
          modAttr_data->owner = NULL;
         XtFree ((char *) modAttr_data->group);
          modAttr_data->group = NULL;
         XtFree ((char *) modAttr_data->link_name);
         modAttr_data->link_name = NULL;
         XSync(XtDisplay(modAttr_rec->ok), False);
         return ((short)GMA_INVALID);
      }
   }

   return((short)GMA_VALID);
}




/************************************************************************
 *
 *  InvalidModAttrMessage
 *	Display an error message.
 *
 ************************************************************************/

static void
InvalidModAttrMessage(
        Widget w,
        int messageIndex )
{
   String string;
   char * title;
   char * tmpStr;
   static String unknownUser = NULL;
   static String unknownGroup = NULL;

   if (unknownUser == NULL)
   {
      tmpStr = GETMESSAGE(21,31, "Owner Name is invalid.\nThe specified user does not exist.");
      unknownUser = XtNewString(tmpStr);
      tmpStr = GETMESSAGE(21,32, "Group Name is invalid.\n\nNo group of users with this name has been created on your system.\nUse your system administration software if you want to create\na user group with this name.");
      unknownGroup = XtNewString(tmpStr);
   }

   switch (messageIndex)
   {
      case UNKNOWN_USER:
           string = unknownUser;
           break;
      case UNKNOWN_GROUP:
           string = unknownGroup;
           break;
   }

   tmpStr = GETMESSAGE(21,33, "Permissions Error");
   title = XtNewString(tmpStr);
   _DtMessage (w, title, string, NULL, HelpRequestCB);
   XtFree(title);
}




/************************************************************************
 *
 *  _LoadFileAttributes
 *	Load the data structure with the data describing the indicated file.
 *
 *      THIS IS A Private FUNCTION
 *
 ************************************************************************/ 

static Boolean
_LoadFileAttributes(
        String host,
        String dir,
        String file,
        ModAttrData *modAttr_data,
        unsigned long mask )
{
   String filetype;
   FileData *file_data=NULL;
   struct group * group_data;
   struct passwd * user_data;
   Boolean returnStatus=TRUE;
   char buf[32];

   file_data = (FileData *) ReadFileData(dir,file);

   if (file_data && (file_data->errnum == 0 || file_data->link))
   {
      /* Extract the most uptodate attributes */
      if (mask & HOST)
      {
         if (host)
            modAttr_data->host = XtNewString(host);
         else
            modAttr_data->host = NULL;
      }
      else
         modAttr_data->host = NULL;

      if (mask & DIR)
      {
         if (dir)
            modAttr_data->directory = XtNewString(dir);
         else
            modAttr_data->directory = NULL;
      }
      else
         modAttr_data->directory = NULL;

      if (mask & NAME)
         modAttr_data->name = XtNewString(file);
      else
         modAttr_data->name = NULL;

      if (mask & FILETYPE)
      {
         modAttr_data->filetype = XtNewString(file_data->logical_type);
	
         if (modAttr_data->pixmapData)
         {
             /* free old pixmap data */
           DtDtsFreeAttributeValue(modAttr_data->pixmapData->hostPrefix);
           DtDtsFreeAttributeValue(modAttr_data->pixmapData->instanceIconName);
           DtDtsFreeAttributeValue(modAttr_data->pixmapData->iconName);
           DtDtsFreeAttributeValue(modAttr_data->pixmapData->iconFileName);
           XtFree((char *) modAttr_data->pixmapData);
           modAttr_data->pixmapData = NULL;
         }

         modAttr_data->pixmapData = _DtRetrievePixmapData(
                                       file_data->logical_type,
                                       NULL,
                                       NULL,
                                       toplevel,
                                       LARGE);
      }
      else
      {
         modAttr_data->filetype = NULL;
         modAttr_data->pixmapData = NULL;
      }

      if (file_data->errnum == 0  &&
          (file_data->stat.st_mode & S_IFMT) != S_IFLNK )
      {
         if (mask & GROUP)
         {
            group_data = getgrgid(file_data->stat.st_gid);
            if(group_data != NULL)
               modAttr_data->group = XtNewString(group_data->gr_name);
            else
            {
               sprintf(buf, "%ld", (long)file_data->stat.st_gid);
               modAttr_data->group = XtNewString(buf);
            }
         }
         else
            modAttr_data->group = NULL;

         if (mask & OWNER)
         {
            user_data = getpwuid(file_data->stat.st_uid);
            if(user_data != NULL)
               modAttr_data->owner = XtNewString(user_data->pw_name);
            else
            {
               sprintf(buf, "%ld", (long)file_data->stat.st_uid);
               modAttr_data->owner = XtNewString(buf);
            }
         }
         else
            modAttr_data->owner = NULL;

         modAttr_data->setuidBits = file_data->stat.st_mode & 07000;
         if (mask & BITS)
            modAttr_data->accessBits = file_data->stat.st_mode & 0777;
         if (mask & SIZE)
            modAttr_data->size = file_data->stat.st_size;
         if (mask & MODTIME)
            modAttr_data->modTime = file_data->stat.st_mtime;
      }
      else
      {
         modAttr_data->owner = NULL;
         modAttr_data->group = NULL;
         modAttr_data->accessBits = 0;
         modAttr_data->size = 0;
         modAttr_data->modTime = 0;
      }
   }
   else
   {
      /* File did not exist or can't be accessed; use defaults */
      modAttr_data->host = NULL;
      modAttr_data->link_name = NULL;
      modAttr_data->directory = NULL;
      modAttr_data->name = NULL;
      modAttr_data->filetype = NULL;
      modAttr_data->pixmapData = NULL;
      modAttr_data->owner = NULL;
      modAttr_data->group = NULL;
      modAttr_data->accessBits = 0;
      modAttr_data->size = 0;
      modAttr_data->modTime = 0;
      returnStatus = False;
   }

   if(file_data)
     FreeFileData(file_data,True);

   return(returnStatus);

}


/************************************************************************
 *
 *  LoadFileAttributes
 *	Load the data structure with the data describing the indicated file.
 *
 *      THIS IS A Public FUNCTION
 *
 ************************************************************************/ 

Boolean
LoadFileAttributes(
        String host,
        String dir,
        String file,
        ModAttrData *modAttr_data )
{
   FileData *file_data=NULL;

   file_data = (FileData *) ReadFileData( dir, file);

   /* Get the link attributes */
   if (file_data && file_data->link)
      modAttr_data->link_name = XtNewString(file_data->link);
   else
      modAttr_data->link_name = NULL;

   if(file_data)
     FreeFileData(file_data,True);

   return _LoadFileAttributes (host, dir, file, modAttr_data, ALL_FIELDS);
}



/************************************************************************
 *
 *  displayFSDialog         
 *	Display dialog to edit file-system-specific attributes,
 *      e.g. AFS Access Control Lists. The dialog is displayed
 *      be executing an independent program. There is no communication
 *      between the two. While this is ok for filesystems like AFS
 *      where file-system specific attributes are independent of
 *      unix attributes, it might not be ok for file systems where
 *      the two are dependent on each other (e.g. DFS?).
 *
 ************************************************************************/  
 

static void 
displayFSDialog (Widget                      w,
                 dtFSData                    *fileSystem,       
                 XmPushButtonCallbackStruct  *cbs)

{ 
   static char *pname = "displayFSDialog";
   pid_t     pid;
   int       n;  
   Arg       args[20];
   Widget    dialog; 
   String    s;
   XmString  xm_string;
   String    errorScript = CDE_INSTALLATION_TOP "/bin/dtfile_error";

   switch ((pid = fork()))
   {
      case -1:  /* error forking */
         s = GETMESSAGE(21, 26, "Unable to create a process for file system dialog");
         xm_string = XmStringCreateLocalized(s);
         n = 0;
         XtSetArg(args[n], XmNmessageString, xm_string);	n++;
         dialog = XmCreateErrorDialog(XtParent(w),"fork_error",args,1);
         XmStringFree(xm_string);
         XtManageChild(dialog);
         break;
      
      case 0:  /* child */

         DBGFORK(("%s:  child forked\n", pname));

         execl(fileSystem->fsDialogProgram,
               fileSystem->fsDialogProgram,   /* argv[0] */
               fileSystem->path,              /* argv[1] */
               NULL);
         /* if the exec fails, build a string and execute a script */
         /*   to display it                                        */
         s = (char *)XtMalloc( 50
                               + strlen( GETMESSAGE(21, 27, "Error executing file system dialog") )
                               + strlen( GETMESSAGE(21, 28, "Attempting to execute: ") )
                               + strlen( fileSystem->fsDialogProgram )
                               + strlen( fileSystem->path )
                               + strlen( strerror(errno) ) );

         strcpy (s, GETMESSAGE(21, 27, "Error executing file system dialog"));
         strcat (s, "\n");
         strcat (s, GETMESSAGE(21, 28, "Attempting to execute: "));
         strcat (s, fileSystem->fsDialogProgram);
         strcat (s, " ");
         strcat (s, fileSystem->path);
         strcat (s, "\n");
         strcat (s, strerror(errno));
         strcat (s, "\n");

         execl(errorScript,
               errorScript,    /* argv[0] */
               s,              /* argv[1] */
               NULL);

         XtFree( s );

         DBGFORK(("%s:  child exiting\n", pname));

         exit(EXIT_FAILURE);   /* just in case */
         break;
      
      default:  /* parent */
         DPRINTF(("in parent after forking file system dialog\n"));
         break;
     
   }  /* end switch */

   DBGFORK(("%s:  forked child<%d>\n", pname, pid));
}  /* end displayFSDialog */


/************************************************************************
 *
 *  DontChangeCB - 
 *
 ************************************************************************/
static void
DontChangeCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
   ModAttrRec  * modAttr_rec = (ModAttrRec *) globalApplyData->modAttr_rec;
   Widget close = modAttr_rec->cancel;
   Widget ok = modAttr_rec->ok;

   XtUnmanageChild((Widget)client_data);
   XmUpdateDisplay((Widget)client_data);
   XtDestroyWidget((Widget)client_data);

   ResetFlag(NULL, ok);
   ResetFlag(NULL, close);
}

/************************************************************************
 *
 *  ChangeCB - 
 *
 *
 ************************************************************************/
static void
ChangeCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
   char *ptr;

   XtUnmanageChild((Widget)client_data);
   XmUpdateDisplay((Widget)client_data);
   XtDestroyWidget((Widget)client_data);

   /* user says they want to go ahead with the change, so first let's 
    * apply the change.
    */
   (*(globalApplyData->callback)) (w, globalApplyData->client_data, NULL);

   /* Now let's change the directory to the Parent directory */
   ptr = DtStrrchr(parentDir, '/');
   if(ptr != NULL && strcmp(parentDir, "/") != 0)
   {
      *ptr = '\0';
      _DtChangeTo(globalApplyData->client_data, parentDir);
   }

   XtFree(parentDir);
}
void
ModAttrFreeValues(
        ModAttrData *modAttr_data )
{
   FreeValues(modAttr_data);
}
