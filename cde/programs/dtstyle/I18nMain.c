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
/* $TOG: I18nMain.c /main/4 1997/08/11 12:31:36 samborn $ */
/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        I18nMain.c
 **
 **   Description: Controls the Dtstyle I18N customization dialog
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/

/*+++++++++++++++++++++++++++++++++++++++*/
/* include files                         */
/*+++++++++++++++++++++++++++++++++++++++*/

#include <X11/Xlib.h>
#include <Xm/MwmUtil.h>

#include <Xm/XmP.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/LabelG.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleBG.h>
#include <Xm/SeparatoG.h>
#include <Xm/VendorSEP.h>
#include <Xm/MessageB.h>

#include <Dt/DialogBox.h>
#include <Dt/Icon.h>
#include <Dt/TitleBox.h>
#include <Dt/UserMsg.h>
#include <Dt/Message.h>
#include <Dt/HourGlass.h>
#include <Dt/Wsm.h>

#include "Help.h"
#include "Main.h"
#include "SaveRestore.h"
#include "Protocol.h"

#include <ctype.h>

/*+++++++++++++++++++++++++++++++++++++++*/
/* include extern functions              */
/*+++++++++++++++++++++++++++++++++++++++*/
#include "I18nMain.h"
#include "I18nEnv.h"

/*+++++++++++++++++++++++++++++++++++++++*/
/* Local #defines                        */
/*+++++++++++++++++++++++++++++++++++++++*/

#define MSG_IMMEDIATE  ((char *)GETMESSAGE(19, 20, "The new XmNpreeditType value will take effect\nas applications are restarted."))
#define MSG_LATER      ((char *)GETMESSAGE(19, 21, "Your selection will take effect\n at your next session."))

/*+++++++++++++++++++++++++++++++++++++++*/
/* Internal Functions                    */
/*+++++++++++++++++++++++++++++++++++++++*/

static int  InitI18nValues(Widget shell);
static void SetImServerHosts(I18nEnv *env);
static void SetImsToggleList(I18nEnv *env);
static void SetImsMode(I18nEnv *env);
static void SetPreeditType(I18nEnv *env, XmStringTable preeditTable,
			   int preeditTableNum);

static Widget BuildI18nDlg(Widget shell);
static void FormLayoutCB(Widget w, XtPointer client_data, XtPointer call_data);
static void MapCB(Widget w, XtPointer client_data, XtPointer call_data);
static void SystemDefaultCB(Widget w, XtPointer client_data, 
			    XtPointer call_data);
static void ServerHostCB(Widget w, XtPointer client_data, XtPointer call_data);
static void UpdateImList(I18nEnv *env, char *hostname);
static void TextFocusCB(Widget w, XtPointer client_data, XtPointer call_data);
static void TextLosingFocusCB(Widget w, XtPointer client_data, 
			      XtPointer call_data);
static void ButtonCB(Widget w, XtPointer client_data, XtPointer call_data);
static int  SaveSelectedValues(I18nEnv *env);
static void SetFileSelValues(I18nEnv *env);
static void ResetLastSavedValues(I18nEnv *env);

static void OkWarnCB(Widget w, XtPointer client_data, XtPointer call_data);
static void CancelWarnCB(Widget w, XtPointer client_data, XtPointer call_data);

static void ListItemSelectCB(Widget w, XtPointer client_data, 
			     XtPointer call_data);
static void MoveUpCB(Widget w, XtPointer client_data, XtPointer call_data);
static void MoveDownCB(Widget w, XtPointer client_data, XtPointer call_data);

static XmStringTable CopyST (XmStringTable st, int nst);
static void FreeST (XmStringTable st, int nst);

/*+++++++++++++++++++++++++++++++++++++++*/
/* Internal Variables                    */
/*+++++++++++++++++++++++++++++++++++++++*/

typedef struct {
    Widget      pictLabel;
    Widget      systemDefault;
    Widget      inputMethodTB;
    Widget      serverHostLabel;
    Widget      serverHostCB;
    Widget      inputMethodLabel;
    Widget      inputMethodRC;
    Widget      imStartModeTB;
    Widget      imStartModeRC;
    Widget      askAtLoginTG;
    Widget      resumeCurrentImTG;
    Widget      preeditTypeTB;
    Widget      preeditTypeList;
    int         preeditTypeListLastPos;
    Boolean     preeditHasChanged;
    XmStringTable preeditSavedVal;
    int         preeditSavedNum;
    Widget      buttonMoveUp;
    Widget      buttonMoveDown;
    Widget      warnDialog;
    Widget      defaultButton;
} I18n, *I18nPtr;

static I18n i18n;

static saveRestore save = {FALSE, 0, };

static char i18nRes[150]="";

static char *defaultPreedit[] = 
{ "OnTheSpot",
  "OverTheSpot",
  "OffTheSpot",
  "Root" 
};
#define NUM_PREEDIT 4

/*+++++++++++++++++++++++++++++++++++++++*/
/* popup_i18nBB                          */
/*+++++++++++++++++++++++++++++++++++++++*/

void 
popup_i18nBB(
        Widget shell )
{
    int ret = NoError;

    if (style.i18nDialog == NULL) {
      _DtTurnOnHourGlass(shell);  
      BuildI18nDlg(shell);
      ret = InitI18nValues(shell);
      XtManageChild(style.i18nDialog);
      if (ret != NoError)
	  _DtI18nErrorDialog(ret) ;
      _DtTurnOffHourGlass(shell);  
    }
    else 
    {
        XtManageChild(style.i18nDialog);
        raiseWindow(XtWindow(XtParent(style.i18nDialog)));
    }
}

/*+++++++++++++++++++++++++++++++++++++++*/
/* InitI18nValues                         */
/*+++++++++++++++++++++++++++++++++++++++*/

static int 
InitI18nValues(Widget shell)
{
    I18nEnv *env;
    int ret = NoError;
    int num_preedit = 0;
    XmString *str = style.xrdb.preeditType;

    /* Initialize */
    env = &i18n_env;
    env->shell = shell;

    /* Get all the needed values from the environment. */
    if ((ret = _DtI18nGetEnvValues(env)) == NoError) {
	/* Update the display with the values */
	SetImServerHosts(env);
	SetImsMode(env);
    }

    /* Get the preeditType resource value */
    while (str[num_preedit]) num_preedit++;
    
    SetPreeditType(env, style.xrdb.preeditType, num_preedit);

    i18n.preeditHasChanged = False;

    /* Save the initial value in case of a reset. */
    i18n.preeditSavedVal = style.xrdb.preeditType;
    i18n.preeditSavedNum = num_preedit;

    return ret ;
}


static void
SetImServerHosts(I18nEnv *env)
{
    Cardinal num_hosts = 0 ;

    /* Set the TextField of the ComboBox with the value found in 
       the IMS selection file */
    XtVaSetValues(XtNameToWidget(i18n.serverHostCB,"Text"), 
		  XmNvalue, env->file_sel->hostname, NULL);
    
    /* update ComboBox with list of server hostnames found in
       "imServerHosts" resource */

    if (style.xrdb.imServerHosts)
	while (style.xrdb.imServerHosts[num_hosts]) num_hosts ++ ;

    /* TBD: need to add file_sel->hostname if not null
       or localhost, if not already present */

    XtVaSetValues(i18n.serverHostCB,
		  XmNitemCount, num_hosts,
		  XmNitems, style.xrdb.imServerHosts, NULL);
}

static void 
SetImsToggleList(I18nEnv *env)
{
    Cardinal n,i;
    XmString string;
    Widget *im_tog = NULL;
    Arg args[5];
    Boolean found = FALSE;
    Cardinal default_index = 0;

    /* create new toggles */

    if (env->ims_sel->ims_list_size > 0)
	im_tog = (Widget*)XtMalloc(sizeof(Widget) 
				   * env->ims_sel->ims_list_size);


    for (i=0; i< env->ims_sel->ims_list_size; i++) {
	string = XmStringCreateLocalized(env->ims_sel->ims_list[i].im_label);
	n = 0;
	XtSetArg(args[n], XmNlabelString, string); n++;
	XtSetArg(args[n], XmNsensitive, 
		 !(env->ims_sel->ims_list[i].inactive)); n++;
	XtSetArg(args[n], XmNuserData, 
		 env->ims_sel->ims_list[i].im_name); n++;

	/* Need to default to the file_sel->im_name, if in the list */
	if (strcmp(env->file_sel->im_name, 
		   env->ims_sel->ims_list[i].im_name) == 0) {
	    found = TRUE;
	    XtSetArg(args[n], XmNset, True); n++;
	}

	if (env->ims_sel->ims_list[i].im_default) {
	    default_index = i;
	}

	im_tog[i] = XmCreateToggleButtonGadget(i18n.inputMethodRC, 
					       "IM", args, n);
	XmStringFree(string);
    }

    if (env->ims_sel->ims_list_size > 0)
	XtManageChildren(im_tog, env->ims_sel->ims_list_size);

    /* If we provided toggle buttons, but the file_sel->im_name was not 
       found, default it to the default from the locale file. */

    if (im_tog && !found) {
	n = 0;
	XtSetArg(args[n], XmNset, True); n++;
	XtSetValues(im_tog[default_index], args, n);
    }

    if (im_tog)
	XtFree((char *) im_tog);

    _DtTurnOffHourGlass(i18n.inputMethodTB);
}

static void
SetImsMode(I18nEnv *env)
{

    /* only called once at startup, or when resetting the saved values */
    if (env->file_sel->start_mode == 0)
	XmToggleButtonGadgetSetState(i18n.askAtLoginTG, True, True); 
    else
	XmToggleButtonGadgetSetState(i18n.resumeCurrentImTG, True, True); 
}

static void 
SetPreeditType(
    I18nEnv *env, 
    XmStringTable preeditTable, 
    int preeditTableNum)
{
    XmStringTable loc_preeditTable ;

    loc_preeditTable = CopyST(preeditTable, preeditTableNum); 
    
    /* initialize List with preeditType values */
    XtVaSetValues(i18n.preeditTypeList,
		  XmNvisibleItemCount, preeditTableNum,
		  XmNitemCount, preeditTableNum,
		  XmNitems, loc_preeditTable,
		  XmNselectedItemCount, 1,
		  XmNselectedItems, loc_preeditTable, NULL);

    /* set the last item position - This is the number of items in the list. */
    i18n.preeditTypeListLastPos = preeditTableNum;

    /* If more than one item in the list, set the Button MoveDown sentivity to 
       True. */
    if (preeditTableNum > 1)
	XtVaSetValues(i18n.buttonMoveDown, XmNsensitive, True, NULL);

    XtVaSetValues(i18n.buttonMoveUp, XmNsensitive, False, NULL);
}


/*+++++++++++++++++++++++++++++++++++++++*/
/* build__i18nDlg                        */
/*+++++++++++++++++++++++++++++++++++++++*/
static Widget 
BuildI18nDlg(
      Widget shell )
{
    register int     i, n;
    Arg              args[MAX_ARGS];
    XmString         button_string[NUM_LABELS]; 
    XmString         string; 
    Widget           form;
    Widget           inputMethodForm;
    Widget           imStartModeForm;
    Widget           preeditTypeForm;
    int              count = 0;
    Widget           widgetList1[6];

    /* get i18n resource values  */

    /* Set up DialogBoxDialog button labels */
    button_string[0] = CMPSTR(_DtOkString);
    button_string[1] = CMPSTR(_DtCancelString);
    button_string[2] = CMPSTR(_DtHelpString);

    /* Create toplevel DialogBox */

    /* Initialize the i18n structure */
    i18n.pictLabel = NULL;
    i18n.systemDefault = NULL;
    i18n.inputMethodTB = NULL;
    i18n.serverHostLabel = NULL;
    i18n.serverHostCB = NULL;
    i18n.inputMethodLabel = NULL;
    i18n.inputMethodRC = NULL;
    i18n.imStartModeTB = NULL;
    i18n.imStartModeRC = NULL;
    i18n.askAtLoginTG = NULL;
    i18n.resumeCurrentImTG = NULL;
    i18n.preeditTypeTB = NULL;
    i18n.preeditTypeList = NULL;
    i18n.buttonMoveUp = NULL;
    i18n.buttonMoveDown = NULL;
    i18n.warnDialog = NULL;

    /* saveRestore
     * Note that save.poscnt has been initialized elsewhere.  
     * save.posArgs may contain information from restoreBeep().*/

    XtSetArg(save.posArgs[save.poscnt], XmNbuttonCount, NUM_LABELS);  
    save.poscnt++;
    XtSetArg(save.posArgs[save.poscnt], XmNbuttonLabelStrings, button_string);
    save.poscnt++;
    XtSetArg(save.posArgs[save.poscnt], XmNdefaultPosition, False);
    save.poscnt++;
    style.i18nDialog = 
        __DtCreateDialogBoxDialog(shell, "i18nDialog", save.posArgs, 
				  save.poscnt);
    XtAddCallback(style.i18nDialog, XmNhelpCallback,
            (XtCallbackProc)HelpRequestCB, (XtPointer)HELP_I18N_DIALOG);

    XmStringFree(button_string[0]);
    XmStringFree(button_string[1]);
    XmStringFree(button_string[2]);

    widgetList1[0] = _DtDialogBoxGetButton(style.i18nDialog,2);
    n=0;
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
    XtSetArg(args[n], XmNcancelButton, widgetList1[0]); n++;
    XtSetValues (style.i18nDialog, args, n);

    n=0;
    XtSetArg(args[n], XmNtitle, 
	 ((char *)GETMESSAGE(19, 1, "Style Manager - Internationalization")));
    n++;
    XtSetArg (args[n], XmNuseAsyncGeometry, True); n++;
    XtSetArg(args[n], XmNmwmFunctions, DIALOG_MWM_FUNC); n++;
    XtSetValues (XtParent(style.i18nDialog), args, n);

    n = 0;
    XtSetArg(args[n], XmNallowOverlap, False); n++;
    XtSetArg(args[n], XmNchildType, XmWORK_AREA);  n++;
    form = XmCreateForm(style.i18nDialog, "i18nForm", args, n);

    n = 0;
    XtSetArg(args[n], XmNfillMode, XmFILL_SELF); n++;
    XtSetArg(args[n], XmNbehavior, XmICON_LABEL); n++;
    XtSetArg(args[n], XmNpixmapForeground, style.secBSCol); n++;
    XtSetArg(args[n], XmNpixmapBackground, style.secTSCol); n++;
    XtSetArg(args[n], XmNstring, NULL); n++;  
    XtSetArg(args[n], XmNshadowThickness, 0); n++;  
    XtSetArg(args[n], XmNimageName, I18N_ICON); n++;  
    XtSetArg(args[n], XmNtraversalOn, False); n++;  
    widgetList1[count++] = i18n.pictLabel =
	_DtCreateIcon(form, "i18npictLabel", args, n);

    n = 0;
    XtSetArg(args[n], XmNmarginHeight, LB_MARGIN_HEIGHT);  n++;
    XtSetArg(args[n], XmNmarginWidth, LB_MARGIN_WIDTH);  n++;
    string = CMPSTR(((char *)GETMESSAGE(19, 2, "Default")));
    XtSetArg(args[n], XmNlabelString, string); n++;
    widgetList1[count++] = i18n.systemDefault = 
        XmCreatePushButtonGadget(form, "systemDefault", args, n);
    XmStringFree(string);

    n = 0;
    string = CMPSTR((char *)GETMESSAGE(19, 3, "Input Method"));
    XtSetArg(args[n], XmNtitleString, string);  n++;
    widgetList1[count++] = i18n.inputMethodTB =
        _DtCreateTitleBox(form, "inputMethodTB", args, n);
    XmStringFree(string);
    
    n = 0;
    XtSetArg(args[n], XmNallowOverlap, False); n++;
    XtSetArg(args[n], XmNchildType, XmWORK_AREA);  n++;
    inputMethodForm = 
        XmCreateForm(i18n.inputMethodTB, "inputMethodForm", args, n);

    n = 0;
    string = CMPSTR((char *)GETMESSAGE(19, 4, "Input Method Start Mode"));
    XtSetArg(args[n], XmNtitleString, string);  n++;
    widgetList1[count++] = i18n.imStartModeTB =
        _DtCreateTitleBox(form, "imStartModeTB", args, n);
    XmStringFree(string);

    n = 0;
    XtSetArg(args[n], XmNallowOverlap, False); n++;
    XtSetArg(args[n], XmNchildType, XmWORK_AREA);  n++;
    imStartModeForm = 
        XmCreateForm(i18n.imStartModeTB, "imStartModeForm", args, n);

    n = 0;
    string = CMPSTR((char *)GETMESSAGE(19, 5, "Preedit Type"));
    XtSetArg(args[n], XmNtitleString, string);  n++;
    widgetList1[count++] = i18n.preeditTypeTB =
        _DtCreateTitleBox(form, "preeditTypeTB", args, n);
    XmStringFree(string);

    n = 0;
    XtSetArg(args[n], XmNallowOverlap, False); n++;
    XtSetArg(args[n], XmNchildType, XmWORK_AREA);  n++;
    preeditTypeForm = 
        XmCreateForm(i18n.preeditTypeTB, "preeditTypeForm", args, n);

    /* Create widgets inside the inputMethodForm */
    n = 0;
    string = CMPSTR((char *)GETMESSAGE(19, 6, "Server Host :"));
    XtSetArg(args[n], XmNlabelString, string); n++;
    i18n.serverHostLabel = 
	XmCreateLabelGadget(inputMethodForm, "serverHostLabel", args, n);

    n = 0;
    XtSetArg(args[n], XmNcomboBoxType, XmDROP_DOWN_COMBO_BOX); n++;
    i18n.serverHostCB =
  	(Widget) XmCreateComboBox(inputMethodForm, "serverHostCB", args, n);

    XtAddCallback(XtNameToWidget(i18n.serverHostCB, "Text"),
		  XmNactivateCallback, ServerHostCB, NULL);
    XtAddCallback(XtNameToWidget(i18n.serverHostCB, "Text"),
		  XmNfocusCallback, TextFocusCB, NULL);
    XtAddCallback(XtNameToWidget(i18n.serverHostCB, "Text"),
		  XmNlosingFocusCallback, TextLosingFocusCB, NULL);
    

    n = 0;
    string = CMPSTR((char *)GETMESSAGE(19, 7, "Input Method"));
    XtSetArg(args[n], XmNlabelString, string); n++;
    i18n.inputMethodLabel = 
	XmCreateLabelGadget(inputMethodForm, "inputMethodLabel", args, n);

    n = 0;
    XtSetArg(args[n], XmNmarginWidth, LB_MARGIN_WIDTH); n++;
    XtSetArg(args[n], XmNmarginHeight, LB_MARGIN_HEIGHT); n++;
    i18n.inputMethodRC = 
        XmCreateRadioBox(inputMethodForm, "inputMethodRC", args, n);


    /* Create widgets inside the imStartModeForm */
    n = 0;
    XtSetArg(args[n], XmNmarginWidth, 0); n++;
    XtSetArg(args[n], XmNmarginHeight, 0); n++;
    i18n.imStartModeRC = 
        XmCreateRadioBox(imStartModeForm, "imStartModeRC", args, n);

    n = 0;
    string = CMPSTR((char *)GETMESSAGE(19, 8, "Ask at login"));
    XtSetArg(args[n], XmNlabelString, string); n++;
    i18n.askAtLoginTG = 
	XmCreateToggleButtonGadget(i18n.imStartModeRC, "askAtLoginTG", 
				   args, n);
    XmStringFree(string);

    n = 0;
    string = CMPSTR((char *)GETMESSAGE(19, 9, "Resume Current Input Method"));
    XtSetArg(args[n], XmNlabelString, string); n++;
    i18n.resumeCurrentImTG = 
        XmCreateToggleButtonGadget(i18n.imStartModeRC, "resumeCurrentImTG", 
				   args, n);
    XmStringFree(string);

    /* Create widgets inside the preeditTypeForm */

    n = 0;
    XtSetArg(args[n], XmNselectionPolicy, XmBROWSE_SELECT); n++;
    i18n.preeditTypeList =
	(Widget) XmCreateList(preeditTypeForm, "preeditTypeList", args, n);

    n = 0;
    string = CMPSTR((char *)GETMESSAGE(19, 10, "Move Up"));
    XtSetArg(args[n], XmNlabelString, string); n++;
    XtSetArg(args[n], XmNsensitive, False); n++;
    i18n.buttonMoveUp =
	XmCreatePushButtonGadget(preeditTypeForm, "buttonMoveUp", args, n);
    XmStringFree(string);

    n = 0;
    string = CMPSTR((char *)GETMESSAGE(19, 11, "Move Down"));
    XtSetArg(args[n], XmNlabelString, string); n++;
    XtSetArg(args[n], XmNsensitive, False); n++;
    i18n.buttonMoveDown =
	XmCreatePushButtonGadget(preeditTypeForm, "buttonMoveDown", args, n);
    XmStringFree(string);

    XtAddCallback(style.i18nDialog, XmNmapCallback, FormLayoutCB, NULL);
    XtAddCallback(style.i18nDialog, XmNmapCallback, MapCB, shell);
    XtAddCallback(style.i18nDialog, XmNcallback, ButtonCB, NULL);
    XtAddCallback(i18n.systemDefault, XmNactivateCallback, 
		  SystemDefaultCB, NULL);
    XtAddCallback(i18n.preeditTypeList, XmNbrowseSelectionCallback, 
		  ListItemSelectCB, NULL);
    XtAddCallback(i18n.buttonMoveUp, XmNactivateCallback, 
		  MoveUpCB, NULL);
    XtAddCallback(i18n.buttonMoveDown, XmNactivateCallback, 
		  MoveDownCB, NULL);

    XtManageChild(form);
    XtManageChildren(widgetList1,count); 

    XtManageChild(inputMethodForm);
    XtManageChild(imStartModeForm);
    XtManageChild(preeditTypeForm);

    XtManageChild(i18n.serverHostLabel);
    XtManageChild(i18n.serverHostCB);
    XtManageChild(i18n.inputMethodLabel);
    XtManageChild(i18n.inputMethodRC);

    XtManageChild(i18n.imStartModeRC);
    XtManageChild(i18n.askAtLoginTG);
    XtManageChild(i18n.resumeCurrentImTG);

    XtManageChild(i18n.preeditTypeList);
    XtManageChild(i18n.buttonMoveUp);
    XtManageChild(i18n.buttonMoveDown);

    return(style.i18nDialog);
}


/*+++++++++++++++++++++++++++++++++++++++*/
/* FormLayoutCB                          */
/*+++++++++++++++++++++++++++++++++++++++*/
static void 
FormLayoutCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    int              n;
    Arg              args[MAX_ARGS];

    /* Picture Label */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNtopOffset,          style.verticalSpacing);    n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_NONE);       n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing);  n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_NONE);       n++;
    XtSetValues (i18n.pictLabel, args, n);

    /* system Default */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNtopOffset,          style.verticalSpacing);    n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_NONE);       n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_NONE);       n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNrightOffset,        style.horizontalSpacing);  n++;
    XtSetValues (i18n.systemDefault, args, n);

    /* Input Method TitleBox */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_WIDGET);     n++;
    XtSetArg(args[n], XmNtopWidget,          i18n.pictLabel);     n++;
    XtSetArg(args[n], XmNtopOffset,          style.horizontalSpacing);  n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_NONE);       n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing);  n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightOffset,        style.horizontalSpacing);  n++;
    XtSetValues (i18n.inputMethodTB, args, n);

    /* Input Method widgets */

    n = 0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_FORM);         n++;
    XtSetArg(args[n], XmNtopOffset,          2*style.verticalSpacing); n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_NONE);         n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_FORM);         n++;
    XtSetArg(args[n], XmNleftOffset,         0);                     n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_NONE);         n++;
    XtSetValues (i18n.serverHostLabel, args, n);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_FORM);         n++;
    XtSetArg(args[n], XmNtopOffset,          style.verticalSpacing); n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_NONE);         n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_WIDGET);       n++;
    XtSetArg(args[n], XmNleftWidget,         i18n.serverHostLabel);  n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_NONE);         n++;
    XtSetValues (i18n.serverHostCB, args, n);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_WIDGET);       n++;
    XtSetArg(args[n], XmNtopWidget,          i18n.serverHostCB);     n++;
    XtSetArg(args[n], XmNtopOffset,          style.verticalSpacing); n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_FORM);         n++;
    XtSetArg(args[n], XmNleftOffset,         0);                     n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_NONE);         n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_NONE);         n++;
    XtSetValues (i18n.inputMethodLabel, args, n);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_WIDGET);       n++;
    XtSetArg(args[n], XmNtopWidget,          i18n.inputMethodLabel); n++;
    XtSetArg(args[n], XmNtopOffset,          style.verticalSpacing); n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_FORM);         n++;
    XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing); n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_FORM);         n++;
    XtSetArg(args[n], XmNbottomOffset,       style.verticalSpacing); n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_FORM);         n++;
    XtSetArg(args[n], XmNrightOffset,        style.horizontalSpacing); n++;
    XtSetValues (i18n.inputMethodRC, args, n);
    
    /* Input Method Start Mode TitleBox */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_WIDGET);       n++;
    XtSetArg(args[n], XmNtopWidget,          i18n.inputMethodTB);    n++;
    XtSetArg(args[n], XmNtopOffset,          style.verticalSpacing); n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_NONE);         n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_FORM);         n++;
    XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing); n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_FORM);         n++;
    XtSetArg(args[n], XmNrightOffset,        style.horizontalSpacing); n++;
    XtSetValues (i18n.imStartModeTB, args, n);

    /* IM Start Mode RC */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_FORM);          n++;
    XtSetArg(args[n], XmNtopOffset,          style.verticalSpacing);  n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_NONE);          n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_FORM);          n++;
    XtSetArg(args[n], XmNleftOffset,         0);                      n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_FORM);          n++;
    XtSetArg(args[n], XmNrightOffset,        0);                      n++;
    XtSetValues (i18n.imStartModeRC, args, n);

    /* Preedit Type TitleBox */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_WIDGET);     n++;
    XtSetArg(args[n], XmNtopWidget,          i18n.imStartModeTB); n++;
    XtSetArg(args[n], XmNtopOffset,          style.verticalSpacing);    n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNbottomOffset,       style.verticalSpacing);    n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing);  n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightOffset,        style.horizontalSpacing);  n++;
    XtSetValues (i18n.preeditTypeTB, args, n);

    /* Preedit Type widgets */
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_FORM);     n++;
    XtSetArg(args[n], XmNtopOffset,          style.verticalSpacing);    n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNbottomOffset,       style.verticalSpacing);    n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing);  n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_NONE); n++;
    XtSetValues (i18n.preeditTypeList, args, n);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_FORM);     n++;
    XtSetArg(args[n], XmNtopOffset,          2*style.verticalSpacing); n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_NONE);       n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_WIDGET);       n++;
    XtSetArg(args[n], XmNleftWidget,         i18n.preeditTypeList); n++;
    XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing);  n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing);  n++;
    XtSetValues (i18n.buttonMoveUp, args, n);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_WIDGET);     n++;
    XtSetArg(args[n], XmNtopWidget,          i18n.buttonMoveUp); n++;
    XtSetArg(args[n], XmNtopOffset,          style.verticalSpacing); n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_NONE);       n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_WIDGET);       n++;
    XtSetArg(args[n], XmNleftWidget,         i18n.preeditTypeList); n++;
    XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing);  n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing);  n++;
    XtSetValues (i18n.buttonMoveDown, args, n);

    XtRemoveCallback(style.i18nDialog, XmNmapCallback, FormLayoutCB, NULL);
}

/*+++++++++++++++++++++++++++++++++++++++*/
/* MapCB                                 */
/*+++++++++++++++++++++++++++++++++++++++*/
static void 
MapCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{

    static int  first_time = 1;
    int         n;
    Arg         args[MAX_ARGS];


    DtWsmRemoveWorkspaceFunctions(style.display, XtWindow(XtParent(w)));

    if (!save.restoreFlag)
        putDialog ((Widget)client_data, w);
   
    XtRemoveCallback(style.i18nDialog, XmNmapCallback, MapCB, NULL);
  
}

/*+++++++++++++++++++++++++++++++++++++++*/
/* ServerHostCB - get the new hostname,  */
/* query its list of im and update the   */
/* radio box.                            */
/*+++++++++++++++++++++++++++++++++++++++*/
static void 
ServerHostCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    char *hostname;

     _DtTurnOnHourGlass(i18n.inputMethodTB);
    /* Get the current hostname in the TextField of the ComboBox, 
       as string */
    XtVaGetValues(XtNameToWidget(i18n.serverHostCB,"Text"), 
		  XmNvalue, &hostname, NULL);

    /* Update the display and query for new IM on this host. */
    UpdateImList(&i18n_env, hostname);
}

static void 
UpdateImList(
    I18nEnv *env,
    char *hostname)
{
    Cardinal i;
    XtArgVal n;
    Widget *im_tog = NULL;
    int ret = NoError;

    /* Clean first the existing list */
    
    /* destroy toggle in radio box is any */
    XtVaGetValues(i18n.inputMethodRC, XmNchildren, &im_tog,
		  XmNnumChildren, &n, NULL);
    if (n) {
	XtUnmanageChildren(im_tog, n);
	for (i=0; i<n; i++) XtDestroyWidget(im_tog[i]);
	im_tog = NULL;
    }    

    /* Empty what we used to have here */
    if (env->ims_sel->ims_list_size) {
	XtFree((char *) env->ims_sel->ims_list);
	env->ims_sel->ims_list = NULL;
	env->ims_sel->ims_list_size = 0 ;
    }

    /* then update the env struct with new ims name and status */
    ret = _DtI18nGetImList(env, hostname);
    if (ret != NoError) {
	_DtTurnOffHourGlass(i18n.inputMethodTB);
	_DtI18nErrorDialog(ret);
    }
}


/*+++++++++++++++++++++++++++++++++++++++*/
/* SystemDefaultCB                       */
/*+++++++++++++++++++++++++++++++++++++++*/
static void 
SystemDefaultCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    static XmStringTable preeditTable = NULL ;
    Cardinal i;
    I18nEnv *env;

    env = &i18n_env;

    /* Set the Server Host to local, that is use the NULL string */
    XtVaSetValues(XtNameToWidget(i18n.serverHostCB,"Text"), 
		  XmNvalue, "local", NULL);  

    UpdateImList(env, "local");

    /* Set the Input Method Start Mode to Ask at login */
    XmToggleButtonGadgetSetState(i18n.askAtLoginTG, True, True);

    if (!preeditTable) {
	/* Set the preeditType list to the default in Motif */
	preeditTable = (XmStringTable) XtMalloc(NUM_PREEDIT * 
						sizeof( XmString ));
	for ( i = 0; i < NUM_PREEDIT; i ++ )
	    preeditTable[i] = XmStringCreate(defaultPreedit[i],
					     "ISO8859-1");
    }
    
    SetPreeditType(env, preeditTable, NUM_PREEDIT);

    /* Mark that the preeditType list has changed. */
    i18n.preeditHasChanged = True;

    return;

}

/*+++++++++++++++++++++++++++++++++++++++*/
/* TextFocusCB - disable the default     */
/* button in the dialog, so that return  */
/* in the TextField doesn't activate it. */
/*+++++++++++++++++++++++++++++++++++++++*/
static void 
TextFocusCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    XtVaGetValues(style.i18nDialog, XmNdefaultButton, &i18n.defaultButton,
		  NULL);
    XtVaSetValues(style.i18nDialog, XmNdefaultButton, NULL, NULL);

    return;

}

/*+++++++++++++++++++++++++++++++++++++++*/
/* TextLosingFocusCB - enable the        */
/* default button, so that return key in */
/* the dialog activate it.               */
/*+++++++++++++++++++++++++++++++++++++++*/
static void 
TextLosingFocusCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    XtVaSetValues(style.i18nDialog, XmNdefaultButton, i18n.defaultButton,
		  NULL);
    
    return;

}

/*+++++++++++++++++++++++++++++++++++++++*/
/* ButtonCB                              */
/* callback for PushButtons in DialogBox */
/*+++++++++++++++++++++++++++++++++++++++*/
static void 
ButtonCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
  int         n;
  int         ret = NoError;
  I18nEnv     *env;
  DtDialogBoxCallbackStruct *cb = (DtDialogBoxCallbackStruct *) call_data;

  env = &i18n_env;

  switch (cb->button_position)
    {
    case OK_BUTTON:

      ret = SaveSelectedValues(env);
      XtUnmanageChild (w);

      break;
      
      
    case CANCEL_BUTTON:
    
      XtUnmanageChild(w);

      /* reset the i18n values from last saved values */
      ResetLastSavedValues(env);

      if (i18n.preeditHasChanged) {
	  /* Reset the preeditType value from the saved one. */
	  SetPreeditType(env, i18n.preeditSavedVal, i18n.preeditSavedNum);

	  i18n.preeditHasChanged = False;
      }
      break;
    
  case HELP_BUTTON:
    XtCallCallbacks(style.i18nDialog, XmNhelpCallback, (XtPointer)NULL);
    break;
    
  default:
    break;
  }
}

static int
SaveSelectedValues(
      I18nEnv *env)
{
    int ret = NoError;
    char *preeditStr = NULL, *tmpStr;
    int i;
    XtArgVal num_preedit = 0;
    XmStringTable list_preedit;
    static char preeditTypeRes[1024];

    /* Get the values from the UI and update the FileSel structure */
    SetFileSelValues(env);

    /* Write these values in the IMS Selection File */
    ret = _DtI18nWriteImSelectionFile(env);

    /* Save the preeditType resource value if needed */
    if (i18n.preeditHasChanged) {

	if (style.xrdb.writeXrdbImmediate)
	    InfoDialog(MSG_IMMEDIATE, style.shell, False);
	else
	    InfoDialog(MSG_LATER, style.shell, False);

	/* generate a string from the list to set the resource */
	XtVaGetValues(i18n.preeditTypeList,
		      XmNitemCount, &num_preedit,
		      XmNitems, &list_preedit, NULL);

	/* Save this value in case of a Cancel */
	if (i18n.preeditSavedVal) {
	    FreeST(i18n.preeditSavedVal, i18n.preeditSavedNum);
	}

	i18n.preeditSavedVal = CopyST(list_preedit, num_preedit);
	i18n.preeditSavedNum = num_preedit;

	for (i = 0; i < num_preedit; i++) {
	    /* get the text of the XmString */
	    tmpStr = XmStringUnparse(list_preedit[i], NULL, XmCHARSET_TEXT, 
				     XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);
	    /* realloc mallocs the first time */
	    preeditStr = XtRealloc(preeditStr, 
				   strlen(preeditStr) + strlen(tmpStr) + 2);

	    /* insert the comma before the second, third. etc */
	    if (i != 0) {
		strcat(preeditStr, ",");
		strcat(preeditStr, tmpStr);
	    }
	    else
		strcpy(preeditStr, tmpStr);

	    XtFree(tmpStr);
	}

	/* if writeXrdbImmediate true write to Xrdb else send to 
	   session mgr */

	sprintf(preeditTypeRes, "*preeditType: %s\n", preeditStr);

	XtFree(preeditStr);

	if(style.xrdb.writeXrdbImmediate)
	    _DtAddToResource(style.display, preeditTypeRes);

	SmNewPreeditSettings(preeditTypeRes);

	/* Reset the state of the list. */
	i18n.preeditHasChanged = False;
    }

    return (ret);

}

static void 
SetFileSelValues(
     I18nEnv *env
)
{
    char *hostname;
    Cardinal i;
    XtArgVal n;
    Widget *im_tog;

    /* The hostname value is stored in the ImsSel structure. */
    env->file_sel->hostname = XtNewString(env->ims_sel->host_name);

    /* Get the selected IM from the selected Toggle button */
    XtVaGetValues(i18n.inputMethodRC, XmNchildren, &im_tog,
		  XmNnumChildren, &n, NULL);
    if (n) {
	for (i=0; i<n; i++) {
	    if (XmToggleButtonGadgetGetState(im_tog[i]) == TRUE) {
		XtVaGetValues(im_tog[i], XmNuserData, 
			      &(env->file_sel->im_name),
			      NULL);
		break;
	    }
	}
    }
    else
	env->file_sel->im_name = NULL;

    /* Get the start mode from the askAtLoginTG state */
    if (XmToggleButtonGadgetGetState(i18n.askAtLoginTG) == TRUE)
	env->file_sel->start_mode = 0;
    else
	env->file_sel->start_mode = 1;
}

static void
ResetLastSavedValues(
     I18nEnv *env
)
{
    if (env->file_sel->start_mode == -1) return;

    /* Reset the TextField of the ComboBox to the last saved hostname. */
    XtVaSetValues(XtNameToWidget(i18n.serverHostCB,"Text"), 
		  XmNvalue, env->file_sel->hostname, NULL);  

    UpdateImList(env, env->file_sel->hostname);

    /* Reset the saved start mode */
    SetImsMode(env);

}

/************************************************************************
 * restoreI18n()
 * restore any state information saved with savei18n.
 * This is called from restoreSession with the application
 * shell and the special xrm database retrieved for restore.
 ************************************************************************/
void 
restoreI18n(
        Widget shell,
        XrmDatabase db )
{
    XrmName xrm_name[5];
    XrmRepresentation rep_type;
    XrmValue value;

    xrm_name [0] = XrmStringToQuark ("i18nDlg");
    xrm_name [2] = 0;

    /* get x position */
    xrm_name [1] = XrmStringToQuark ("x");
    if (XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value)){
      XtSetArg (save.posArgs[save.poscnt], XmNx, atoi((char *)value.addr)); 
      save.poscnt++;
      save.restoreFlag = True;
    }

    /* get y position */
    xrm_name [1] = XrmStringToQuark ("y");
    if (XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value)){
      XtSetArg (save.posArgs[save.poscnt], XmNy, atoi((char *)value.addr)); save.poscnt++;
    }

    xrm_name [1] = XrmStringToQuark ("ismapped");
    XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value);
    /* Are we supposed to be mapped? */
    if (strcmp(value.addr, "True") == 0) 
      popup_i18nBB(shell);
}

/************************************************************************
 * saveI18n()
 *
 * This routine will write out to the passed file descriptor any state
 * information this dialog needs.  It is called from saveSessionCB with the
 * file already opened.
 * All information is saved in xrm format.  There is no restriction
 * on what can be saved.  It doesn't have to be defined or be part of any
 * widget or Xt definition.  Just name and save it here and recover it in
 * restoreBackdrop.  The suggested minimum is whether you are mapped, and your
 * location.
 ************************************************************************/
void 
saveI18n(
        int fd )
{
    Position x,y;
    Dimension width, height;
    char *bufr = style.tmpBigStr;     /* size=[1024], make bigger if needed */
    XmVendorShellExtObject  vendorExt;
    XmWidgetExtData         extData;

    if (style.i18nDialog != NULL) 
    {
        if (XtIsManaged(style.i18nDialog))
            sprintf(bufr, "*i18nDlg.ismapped: True\n");
        else
            sprintf(bufr, "*i18nDlg.ismapped: False\n");

	/* Get and write out the geometry info for our Window */
	x = XtX(XtParent(style.i18nDialog));
	y = XtY(XtParent(style.i18nDialog));

        /* Modify x & y to take into account window mgr frames
         * This is pretty bogus, but I don't know a better way to do it.
         */
        extData = _XmGetWidgetExtData(style.shell, XmSHELL_EXTENSION);
        vendorExt = (XmVendorShellExtObject)extData->widget;
        x -= vendorExt->vendor.xOffset;
        y -= vendorExt->vendor.yOffset;

	width = XtWidth(style.i18nDialog);
	height = XtHeight(style.i18nDialog);

	sprintf(bufr, "%s*i18nDlg.x: %d\n", bufr, x);
	sprintf(bufr, "%s*i18nDlg.y: %d\n", bufr, y);
	sprintf(bufr, "%s*i18nDlg.width: %d\n", bufr, width);
	sprintf(bufr, "%s*i18nDlg.height: %d\n", bufr, height);
	write (fd, bufr, strlen(bufr));
    }
}


/*+++++++++++++++++++++++++++++++++++++++*/
/* cancelWarnCB - callback for the       */
/* cancel button of the warnDialog       */
/*+++++++++++++++++++++++++++++++++++++++*/

static void 
CancelWarnCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    return;
}




/*+++++++++++++++++++++++++++++++++++++++*/
/* okWarnCB - callback for the           */
/* OK button of the warnDialog           */
/*+++++++++++++++++++++++++++++++++++++++*/

static void 
OkWarnCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    return;
}


/*+++++++++++++++++++++++++++++++++++++++*/
/* _DtI18nSetSensitiveImTB - set the     */
/* sensitivity of the TitleBox. This is  */
/* used when changing host, while getting*/
/* information.                          */
/* Also recreate the toggle list.        */
/*+++++++++++++++++++++++++++++++++++++++*/

void 
_DtI18nSetSensitiveImTB(
			I18nEnv * env,
			Boolean sensitivity )
{
    static Boolean first_time = True;

    XtSetSensitive(i18n.inputMethodRC, sensitivity);

    if (sensitivity) {
	SetImsToggleList(env);
	/* Save the initial values in case of a Cancel */
	if (first_time) {
	    SetFileSelValues(env);
	    first_time = False;
	}
    }

}


void
_DtI18nErrorDialog(
    int err
)
{
    char *err_str;

    switch(err) {

    case ErrNoHome: 
	err_str = GETMESSAGE(19, 30, 
			     "The environment variable HOME is not defined.");
	break;
    case ErrNoLocale:
	err_str = GETMESSAGE(19, 31, 
			     "The environment variable LANG is not defined.");
	break;
    case ErrNoCDELocale:
	err_str = GETMESSAGE(19, 32, 
			     "This locale is not supported by the desktop.");
	break;

	/* File I/O */
    case ErrFileCreate:	
	err_str = GETMESSAGE(19, 33, 
		       "Cannot create the user Input Method Selection file");
	break;

	/* Selection */
    case ErrNoSelectionFile:
	err_str = GETMESSAGE(19, 34, 
			     "Cannot find the user Input Method Selection file.");
	break;
    case ErrSaveSelection:	
	err_str = GETMESSAGE(19, 35, 
			     "Cannot save the user Input Method Selection file.");
	break;

	/* Remote */
    case ErrUnknownHost:	
	err_str = GETMESSAGE(19, 36, "Unknown host");
	break;
    case ErrNoDtimsstart:
	err_str = GETMESSAGE(19, 37, 
			     "dtimsstart was not found.\nPerhaps the desktop environment\nis not properly installed.");
	break;
    case ErrRemoteFailed:
	err_str = GETMESSAGE(19, 38, "Remote execution failed on\n the selected host.");

        /* Pipe */
    case ErrNoPopen:
	err_str = GETMESSAGE(19, 39, "Cannot initialize remote execution");
	break;
    case ErrTimeOut:
	err_str = GETMESSAGE(19, 40, "Getting the Input Method on the \nselected host is taking too long");
	break;

    default:
	err_str = GETMESSAGE(19, 41, "Unknown error");
	break;
    }

    ErrDialog(err_str, style.i18nDialog);
}

/*+++++++++++++++++++++++++++++++++++++++*/
/* ListItemSelectCB - callback for the   */
/* selection on an item in the preedit   */
/* Type List.                            */
/*+++++++++++++++++++++++++++++++++++++++*/

static void 
ListItemSelectCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    XmListCallbackStruct *listCBS = (XmListCallbackStruct *) call_data;

    /* Set the sensitivity of the MoveUp and MoveDown buttons depending 
       on the selected item position. MoveUp is insensitive is the first
       item in the list is selected; MoveDown is insensitive if the last 
       item in the list is selected */

    if (listCBS->item_position == 1)
	XtSetSensitive(i18n.buttonMoveUp, False);
    else
	XtSetSensitive(i18n.buttonMoveUp, True);

    if (listCBS->item_position == i18n.preeditTypeListLastPos)
	XtSetSensitive(i18n.buttonMoveDown, False);
    else
	XtSetSensitive(i18n.buttonMoveDown, True);

    return;
}

/*+++++++++++++++++++++++++++++++++++++++*/
/* MoveUpCB - callback for the           */
/* MoveUp button                         */
/*+++++++++++++++++++++++++++++++++++++++*/

static void 
MoveUpCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    int *position_list;
    int position_count;

    if (XmListGetSelectedPos(i18n.preeditTypeList, &position_list, 
			     &position_count)) {

	/* there should be only one selected per our selection
	   policy set on the list, but consider the first selected one 
	   in any case */

	/* this should always be the case as the MoveUp button should be
	   insensitive when the first item is selected, but check anyway */

	if (position_list[0] > 1) {
	    XmStringTable list_items;
	    XmString items[2];
	    int sel_pos = position_list[0];
	    int sel_index = sel_pos - 1;

	    XtVaGetValues(i18n.preeditTypeList, XmNitems, &list_items, NULL);

	    /* makes an array of two XmStrings by reversing the selected
	       one and the item preceeding it */
	    items[0] = list_items[sel_index];
	    items[1] = list_items[sel_index - 1];

	    /* this call preserves selected state */
	    XmListReplaceItemsPos(i18n.preeditTypeList,
				  items, 2,
				  sel_pos - 1);

	    if (sel_pos == 2) {
		/* it's now 1, so set the MoveUp button insensitive */
		XtSetSensitive(i18n.buttonMoveUp, False);
	    }

	    if (sel_pos == i18n.preeditTypeListLastPos) {
		/* it's not anymore the last one, so set the MoveDown 
		   button sensitive */
		XtSetSensitive(i18n.buttonMoveDown, True);
	    }

	    /* preeditType has changed */
	    i18n.preeditHasChanged = True;
	}
    }
}

/*+++++++++++++++++++++++++++++++++++++++*/
/* MoveDownCB - callback for the         */
/* MoveDown button                       */
/*+++++++++++++++++++++++++++++++++++++++*/

static void 
MoveDownCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    int *position_list;
    int position_count;

    if (XmListGetSelectedPos(i18n.preeditTypeList, &position_list, 
			     &position_count)) {

	/* there should be only one selected per our selection
	   policy set on the list, but consider the first selected one 
	   in any case */

	/* this should always be the case as the Move Down button should be
	   insensitive when the first item is selected, but check anyway */

	if (position_list[0] != i18n.preeditTypeListLastPos) {
	    XmStringTable list_items;
	    XmString items[2];
	    int sel_pos = position_list[0];
	    int sel_index = sel_pos - 1;

	    XtVaGetValues(i18n.preeditTypeList, XmNitems, &list_items, NULL);

	    /* makes an array of two XmStrings by reversing the selected
	       one and the item succeeding it */
	    items[0] = list_items[sel_index + 1];
	    items[1] = list_items[sel_index];

	    /* this call preserves selected state */
	    XmListReplaceItemsPos(i18n.preeditTypeList,
				  items, 2,
				  sel_pos);

	    if (sel_pos == (i18n.preeditTypeListLastPos - 1)) {
		/* it's now the last one, so set the MoveDown button 
		   insensitive */
		XtSetSensitive(i18n.buttonMoveDown, False);
	    }

	    if (sel_pos == 1) {
		/* it's not anymore the first one, so set the MoveUp 
		   button sensitive */
		XtSetSensitive(i18n.buttonMoveUp, True);
	    }

	    /* preeditType has changed */
	    i18n.preeditHasChanged = True;
	}
    }

}


static XmStringTable
CopyST (XmStringTable st, int nst)
{
    XmStringTable lst = (XmStringTable) XtMalloc(sizeof(XmString) * nst);
    int i ;

    for (i=0; i < nst ; i++) {
	lst[i] = XmStringCopy(st[i]);
    }

    return lst ;
}


static void
FreeST (XmStringTable st, int nst)
{
    int i;

    for (i=0; i < nst ; i++) {
       XmStringFree(st[i]);
    }

    XtFree((char*)st);
}

