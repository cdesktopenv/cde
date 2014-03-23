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
/* $TOG: SmUI.c /main/32 1998/07/23 18:08:17 mgreess $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:        SmUI.c
 **
 **  Project:     DT Session Manager (dtsession)
 **
 **  Description:
 **  -----------
 **  This file handles all UI components of the session manager.  This
 **  includes all dialog boxes.   The session manager does not handle its
 **  representations in the front panel and customizer.  That is handled
 **  by those tools.
 **
 **
 **
 *******************************************************************
 **  (c) Copyright Hewlett-Packard Company, 1990.  All rights are  
 **  reserved.  Copying or other reproduction of this program      
 **  except for archival purposes is prohibited without prior      
 **  written consent of Hewlett-Packard Company.		     
 ********************************************************************
 **
 **
 **
 *****************************************************************************
 *************************************<+>*************************************/

#include <signal.h>
#include <stdio.h>
#include <sys/param.h>
#include <X11/Intrinsic.h>
#include <Xm/MwmUtil.h>
#include <Xm/Xm.h>
#include <Xm/PushB.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/DrawingA.h>
#include <Xm/Frame.h>
#include <Xm/Separator.h>
#include <Xm/MessageB.h>
#include <Xm/DialogS.h>
#include <Xm/Text.h>
#include <Dt/DtP.h>
#include <Dt/SessionM.h>
#include <Dt/Icon.h>
#include <Dt/MsgLog.h>
#include <Dt/Lock.h>
#include <Tt/tttk.h>

#include "Sm.h"
#include "SmUI.h"
#include "SmSave.h"
#include "SmRestore.h"
#include "SmHelp.h"
#include "SmGlobals.h"

#ifdef USE_XINERAMA
#include <DtXinerama.h>
#endif

typedef enum {
	ConfirmationNone,
	ConfirmationOK,
	ConfirmationCancel,
	ConfirmationHelp
} ConfirmState;

ConfirmState	confirmState;

/*
 * #define statements
 */
#define PASSWORD_INDICATOR " "

typedef struct _ExitRecord {
  Tt_message     *pmsg;
  union {
    Widget exitCancelledDialog;
    int queryExitConfirmedMode;
  } u;
  Boolean	doSave;
} ExitRecord;

/*
 * Global variables
 */
DialogData   	smDD;
Arg      		uiArgs[20];

/*
 * Local Function Declarations
 */

static int CompatModeExit( void ) ;
static void ExitConfirmed( Widget, XtPointer, XtPointer ) ;
static void ExitCancelled( Widget, XtPointer, XtPointer ) ;
static void LockDialogUp( Widget, XtPointer, XtPointer ) ;
static void SimpleOK( Widget, XtPointer, XtPointer ) ;
static void ConfirmOKCB ( Widget, XtPointer, XtPointer ) ;
static void ConfirmCancelCB ( Widget, XtPointer, XtPointer ) ;
static void ConfirmHelpCB ( Widget, XtPointer, XtPointer ) ;
static void XSMPFailureOKCB( Widget w, XtPointer client_data, XtPointer call_data );
static void SaveTimeout( XtPointer , XtIntervalId *) ;


/*
 * Local vars
 */
static Boolean session_confirmed = False;
static Boolean reasonsDialogOK;
static Boolean saveTimeout;



/*************************************<->*************************************
 *
 *  CreateLockDialog ()
 *
 *
 *  Description:
 *  -----------
 *  Create the lock dialog when it exists NOT as a part of a cover
 *
 *
 *  Inputs:
 *  ------
 *
 * 
 *  Outputs:
 *  -------
 *  None.
 *
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
Widget 
CreateLockDialog( void )
{
    int 	i;
    Widget	loginLabel, instructLabel, tmpLock, indFrame;
    Widget  	passwdLabel, passwdForm, picFrame, loginPic, loginFrame;
    Dimension	width;		/* width, height of login label    */
    XmString	lockString, passwordString;
    char	*lockMessage;
    char	*envLog;
    Pixel   	fg, bg, focus_color;  /* foreground, background colors */
    
    i = 0;
    XtSetArg(uiArgs[i], XmNallowShellResize, True); i++;
    XtSetArg(uiArgs[i], XmNmarginWidth, 0); i++;
    XtSetArg(uiArgs[i], XmNmarginHeight, 0); i++;
    XtSetArg(uiArgs[i], XmNshadowType, XmSHADOW_OUT); i++;
    XtSetArg(uiArgs[i], XmNshadowThickness,5); i++;
    XtSetArg(uiArgs[i], XmNunitType, XmPIXELS); i++;
    XtSetArg(uiArgs[i], XmNresizePolicy, XmRESIZE_NONE);i++;
    XtSetArg(uiArgs[i], XmNdialogStyle, XmDIALOG_SYSTEM_MODAL); i++;
    XtSetArg(uiArgs[i], XmNmwmInputMode, MWM_INPUT_SYSTEM_MODAL);i++;
    XtSetArg(uiArgs[i], XmNmappedWhenManaged, True); i++;
    tmpLock = XmCreateFormDialog(smGD.topLevelWid, "lockDialog", uiArgs, i);
    XtAddCallback (XtParent(tmpLock), XmNpopupCallback,
		   LockDialogUp, NULL);
    smDD.matte[0] = tmpLock;

    i = 0;
    XtSetArg(uiArgs[i], XmNuseAsyncGeometry, True);i++;
    XtSetArg(uiArgs[i], XmNmwmDecorations, 0);i++;
    XtSetArg(uiArgs[i], XmNmwmInputMode, MWM_INPUT_SYSTEM_MODAL);i++;
    XtSetValues(XtParent(tmpLock), uiArgs, i);
		
    i = 0;
    XtSetArg(uiArgs[i], XmNshadowType, XmSHADOW_OUT); i++;
    XtSetArg(uiArgs[i], XmNshadowThickness, 2); i++; 
    XtSetArg(uiArgs[i], XmNtopAttachment, XmATTACH_FORM); i++;
    XtSetArg(uiArgs[i], XmNtopOffset, 15); i++;
    XtSetArg(uiArgs[i], XmNbottomAttachment, XmATTACH_FORM); i++;
    XtSetArg(uiArgs[i], XmNbottomOffset, 15); i++;
    XtSetArg(uiArgs[i], XmNrightAttachment, XmATTACH_FORM); i++;
    XtSetArg(uiArgs[i], XmNrightOffset, 15); i++;
    picFrame = XmCreateFrame(tmpLock, "picFrame", uiArgs, i);

	i = 0;
	XtSetArg(uiArgs[i], XmNforeground, &fg); i++;
	XtSetArg(uiArgs[i], XmNbackground, &bg); i++;
	XtGetValues(tmpLock, uiArgs, i);

    i = 0;
	XtSetArg(uiArgs[i], XmNfillMode, XmFILL_SELF); i++;
	XtSetArg(uiArgs[i], XmNbehavior, XmICON_LABEL); i++;
	XtSetArg(uiArgs[i], XmNpixmapForeground, fg); i++;
	XtSetArg(uiArgs[i], XmNpixmapBackground, bg); i++;
	XtSetArg(uiArgs[i], XmNstring, NULL); i++;
	XtSetArg(uiArgs[i], XmNshadowThickness, 0); i++;
	XtSetArg(uiArgs[i], XmNtraversalOn, False); i++;
	loginPic = _DtCreateIcon(picFrame, "lockLabelPixmap", uiArgs, i);

    i = 0;
    XtSetArg(uiArgs[i], XmNshadowType, XmSHADOW_OUT); i++;
    XtSetArg(uiArgs[i], XmNshadowThickness, 2); i++; 
    XtSetArg(uiArgs[i], XmNtopAttachment, XmATTACH_FORM); i++;
    XtSetArg(uiArgs[i], XmNtopOffset, 15); i++;
    XtSetArg(uiArgs[i], XmNbottomAttachment, XmATTACH_FORM); i++;
    XtSetArg(uiArgs[i], XmNbottomOffset, 15); i++;
    XtSetArg(uiArgs[i], XmNleftAttachment, XmATTACH_FORM); i++;
    XtSetArg(uiArgs[i], XmNleftOffset, 15); i++;
    XtSetArg(uiArgs[i], XmNrightAttachment, XmATTACH_WIDGET); i++;
    XtSetArg(uiArgs[i], XmNrightOffset, 0); i++;
    XtSetArg(uiArgs[i], XmNrightWidget, loginPic); i++;
    loginFrame = XmCreateFrame(tmpLock, "loginFrame", uiArgs, i);


    /* 
     * create the login matte...
     */
    i = 0;
    XtSetArg(uiArgs[i], XmNresizePolicy, XmRESIZE_NONE);i++;
    smDD.loginMatte[0] = XmCreateForm(loginFrame, "loginMatte", uiArgs, i);

    /*
     *  create the login/password forms
     */
    i = 0;
    XtSetArg(uiArgs[i], XmNshadowThickness, 0); i++; 
    XtSetArg(uiArgs[i], XmNrightAttachment, XmATTACH_FORM); i++;
    XtSetArg(uiArgs[i], XmNrightOffset, 15); i++;
    XtSetArg(uiArgs[i], XmNbottomAttachment, XmATTACH_POSITION); i++;
    XtSetArg(uiArgs[i], XmNbottomPosition, 50); i++;
    XtSetArg(uiArgs[i], XmNleftAttachment, XmATTACH_FORM); i++;
    XtSetArg(uiArgs[i], XmNleftOffset, 15); i++;
    smDD.loginForm[0] = XmCreateForm(smDD.loginMatte[0], "loginForm", uiArgs, i);

    i = 0;
    XtSetArg(uiArgs[i], XmNresizePolicy, XmRESIZE_NONE);i++;
    XtSetArg(uiArgs[i], XmNshadowThickness, 0); i++; 
    XtSetArg(uiArgs[i], XmNtopAttachment, XmATTACH_POSITION); i++;
    XtSetArg(uiArgs[i], XmNtopPosition, 50); i++;
	XtSetArg(uiArgs[i], XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET); i++;
	XtSetArg(uiArgs[i], XmNleftWidget, smDD.loginForm[0]); i++;
	XtSetArg(uiArgs[i], XmNrightAttachment, XmATTACH_OPPOSITE_WIDGET); i++;
	XtSetArg(uiArgs[i], XmNrightWidget, smDD.loginForm[0]); i++;
    passwdForm = XmCreateForm(smDD.loginMatte[0], "passwdForm", uiArgs, i);


    /*
     *  create the login/password labels...
     */
    i = 0;
    envLog = getenv("LOGNAME");
    lockMessage = XtMalloc(100 + strlen(envLog));
    sprintf(
	lockMessage,
	(char*) GETMESSAGE(18, 1, "Display locked by user %s."), envLog);
    lockString = XmStringCreateLocalized(lockMessage);
    XtSetArg(uiArgs[i], XmNtopAttachment, XmATTACH_POSITION); i++;
    XtSetArg(uiArgs[i], XmNtopPosition, 20); i++;
    XtSetArg(uiArgs[i], XmNalignment, XmALIGNMENT_CENTER); i++;
    XtSetArg(uiArgs[i], XmNlabelString,	lockString); i++;
    loginLabel = XmCreateLabelGadget(smDD.loginForm[0],
				     "loginLabel", uiArgs, i);
    XtManageChild(loginLabel);
    XmStringFree(lockString);
    XtFree(lockMessage);

    i = 0;
    lockString = XmStringCreateLocalized(((char *)GETMESSAGE(18, 2, "Enter password to unlock.")));
    XtSetArg(uiArgs[i], XmNtopAttachment, XmATTACH_WIDGET); i++;
    XtSetArg(uiArgs[i], XmNtopWidget, loginLabel); i++;
    XtSetArg(uiArgs[i], XmNalignment, XmALIGNMENT_CENTER); i++;
    XtSetArg(uiArgs[i], XmNlabelString,	lockString); i++;
    instructLabel = XmCreateLabelGadget(smDD.loginForm[0], "instructLabel",
					uiArgs, i);
    XtManageChild(instructLabel);
    XmStringFree(lockString);

    i = 0;
    passwordString = XmStringCreateLocalized(((char *)GETMESSAGE(18, 3, "Password: ")));
    XtSetArg(uiArgs[i], XmNtopAttachment, XmATTACH_POSITION); i++;
    XtSetArg(uiArgs[i], XmNtopPosition, 20); i++;
    XtSetArg(uiArgs[i], XmNbottomAttachment, XmATTACH_POSITION); i++;
    XtSetArg(uiArgs[i], XmNbottomPosition, 80); i++;
    XtSetArg(uiArgs[i], XmNleftAttachment, XmATTACH_FORM); i++;
    XtSetArg(uiArgs[i], XmNalignment, XmALIGNMENT_END); i++;
    XtSetArg(uiArgs[i], XmNlabelString,	passwordString); i++;
    passwdLabel = XmCreateLabelGadget(passwdForm,
				      "passwdLabel", uiArgs, i);
    XtManageChild(passwdLabel);
    XmStringFree(passwordString);

    /*
     * Give the password label an offset
     */
    i = 0;
    XtSetArg(uiArgs[i], XmNwidth, &width);i++;
    XtGetValues(passwdLabel, uiArgs, i);

    i = 0;
    width += (width/6);
    XtSetArg(uiArgs[i], XmNwidth, width);i++;
    XtSetArg(uiArgs[i], XmNrecomputeSize, False);i++;
    XtSetValues(passwdLabel, uiArgs, i);


    i = 0;
    XtSetArg(uiArgs[i], XmNshadowType, XmSHADOW_IN); i++;
    XtSetArg(uiArgs[i], XmNshadowThickness, 1); i++;
    XtSetArg(uiArgs[i], XmNtopAttachment, XmATTACH_POSITION); i++;
    XtSetArg(uiArgs[i], XmNtopPosition, 20); i++;
    XtSetArg(uiArgs[i], XmNbottomAttachment, XmATTACH_POSITION); i++;
    XtSetArg(uiArgs[i], XmNbottomPosition, 80); i++;
    XtSetArg(uiArgs[i], XmNleftAttachment, XmATTACH_WIDGET); i++;
    XtSetArg(uiArgs[i], XmNleftWidget, passwdLabel); i++;
    XtSetArg(uiArgs[i], XmNrightAttachment, XmATTACH_FORM); i++;
    XtSetArg(uiArgs[i], XmNrightOffset, 10); i++;
    XtSetArg(uiArgs[i], XmNalignment, XmALIGNMENT_END); i++;
    indFrame = XmCreateFrame(passwdForm,  "indFrame", uiArgs, i);

    i = 0;
    passwordString = XmStringCreateLocalized("|");
    XtSetArg(uiArgs[i], XmNlabelString,	passwordString); i++;
    XtSetArg(uiArgs[i], XmNalignment, XmALIGNMENT_BEGINNING); i++;
    smDD.indLabel[0] = XmCreateLabel(indFrame, "indLabel",
					uiArgs, i);
    XtManageChild(smDD.indLabel[0]);
    XmStringFree(passwordString);

    i = 0;
    XtSetArg(uiArgs[i], XmNhighlightColor,  &focus_color); i++;
    XtGetValues(indFrame, uiArgs, i);
    XtVaSetValues ( smDD.indLabel[0],
                    XmNborderWidth, 2,
                    XmNborderColor,  focus_color, 
                    NULL );
    
    /*
     * Manage forms AFTER all children have been managed
     */
    XtManageChild(indFrame);
    XtManageChild(passwdForm);
    XtManageChild(smDD.loginForm[0]);
    XtManageChild(smDD.loginMatte[0]);
    XtManageChild(loginPic);
    XtManageChild(picFrame);
    XtManageChild(loginFrame);

    return(tmpLock);
}



/*************************************<->*************************************
 *
 *  ExitSession ()
 *
 *
 *  Description:
 *  -----------
 *  Determines which exit routines get called when an exit request is made
 *  of the session manager.  
 *  If smGD.bmsDead == false, we just exit.
 *  If ASK_STATE is turned on, the query dialog is
 *  put up, if VERBOSE is on, confirm exit in current mode (restart or reset)
 *  if confirmation is turned off - exit immediately.
 *
 *
 *  Inputs:
 *  ------
 *  msg -- if non-zero, Session_Exit request to reply/fail && destroy
 *
 * 
 *  Outputs:
 *  -------
 *  None.
 *
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
void
ExitSession(
	Tt_message msg)
{
    if (smGD.bmsDead == True)
    {
	ImmediateExit(-1, msg, True);
    }
    if(smGD.compatMode == True)
    {
	if (msg != 0) {
	    tt_message_reply( msg );
	    tt_message_destroy( msg );
	}
	CompatModeExit();
    }
    else
    {
	if(smSettings.confirmMode == DtSM_VERBOSE_MODE || 
	   smSettings.startState == DtSM_ASK_STATE)
	{
	    ConfirmExit( msg, True );
	}
	else
	{
	    ImmediateExit(smSettings.startState, msg, True);
	}
    }
}



/*************************************<->*************************************
 *
 *  ConfirmExit ()
 *
 *
 *  Description:
 *  -----------
 *  Create the exit confirmation dialog box (if it hasn't been) and confirm
 *  that the user wants to exit the session.  This routine only gets called
 *  when the user hasn't turned of exit confirmation and is not in ASK mode
 *
 *
 *  Inputs:
 *  ------
 *  msg -- if non-zero, Session_Exit request to reply/fail && destroy
 *
 * 
 *  Outputs:
 *  -------
 *  None.
 *
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
int 
ConfirmExit( 
    Tt_message msg,
    Boolean doSave)
{
    int 	i;
    String	tmpString;
    ExitRecord  *exitRec;
    Tt_message  *pmsg;
    static	XmString homeToHome; 	/* started Home, restore to Home */
    static	XmString returnToCurrent; /* started Home or Current but
					    retrun to Current */
    static	XmString currentToHome; /* started Current, return to Home */

    if(smDD.confExit == NULL)
    {
	/*
	 * Create all compound strings for confirmation dialogs
	 */
	returnToCurrent = XmStringCreateLocalized(((char *)GETMESSAGE(18, 4, 
		"Exiting the desktop session...\n\n\
Your Current session will be restored upon login.\n\n\
Application updates you have not saved will be lost.\n\n\
Continue Logout?")));

	homeToHome = XmStringCreateLocalized(((char *)GETMESSAGE(18, 5, 
		"Exiting the desktop session...\n\n\
Your Home session will be restored upon login.\n\
Your Current session will not be saved.\n\n\
Application updates you have not saved will be lost.\n\n\
Continue Logout?")));

	currentToHome = XmStringCreateLocalized(((char *)GETMESSAGE(18, 75,
		"Exiting the desktop session...\n\n\
Your Current session will be saved but your Home\n\
session will be restored upon login.\n\n\
Application updates you have not saved will be lost.\n\n\
Continue Logout?")));

	/*
	 * Build up the correct string for this dialog
	 */
	i = 0;
	if (smGD.sessionType == HOME_SESSION && 
	    smSettings.startState == DtSM_HOME_STATE) 
	{
	    XtSetArg(uiArgs[i], XmNmessageString, homeToHome); i++;
	}
	else if (smGD.sessionType == CURRENT_SESSION &&
	         smSettings.startState == DtSM_HOME_STATE) 
        {
	    XtSetArg(uiArgs[i], XmNmessageString, currentToHome); i++;
	}
	else
	{
	    XtSetArg(uiArgs[i], XmNmessageString, returnToCurrent); i++;
	}

	/*
	 * Now create the dialog box
	 */
	tmpString = GETMESSAGE(18, 6, "Logout Confirmation");
	XtSetArg (uiArgs[i], XmNallowShellResize, False);  i++;
	XtSetArg(uiArgs[i], XmNdialogStyle, XmDIALOG_SYSTEM_MODAL); i++;
	XtSetArg(uiArgs[i], XmNmessageAlignment, XmALIGNMENT_CENTER); i++;
	XtSetArg(uiArgs[i], XmNtitle, tmpString); i++;
	XtSetArg(uiArgs[i], XmNokLabelString, smDD.okLogoutString); i++;
	XtSetArg(uiArgs[i], XmNcancelLabelString, smDD.cancelLogoutString); i++;
	XtSetArg(uiArgs[i], XmNhelpLabelString, smDD.helpString); i++;
	XtSetArg(uiArgs[i], XmNautoUnmanage, False); i++;

        pmsg = (Tt_message *)XtMalloc(sizeof(Tt_message));
        XtSetArg(uiArgs[i], XmNuserData, pmsg); i++;

	smDD.confExit = XmCreateWarningDialog(smGD.topLevelWid, "exitDialog",
					      uiArgs, i);
	
	i = 0;
	XtSetArg(uiArgs[i], XmNuseAsyncGeometry, True);i++;
	XtSetArg(uiArgs[i], XmNmwmFunctions, 0);i++;
	XtSetArg(uiArgs[i], XmNmwmDecorations,
		 (MWM_DECOR_TITLE | MWM_DECOR_BORDER));i++;
	XtSetArg(uiArgs[i], XmNmwmInputMode, MWM_INPUT_SYSTEM_MODAL);i++;
	XtSetValues(XtParent(smDD.confExit), uiArgs, i);

        exitRec = (ExitRecord *)XtMalloc( sizeof(ExitRecord) );
        exitRec->pmsg = pmsg;
        exitRec->doSave = doSave;
	XtAddCallback (smDD.confExit, XmNokCallback, ExitConfirmed, exitRec);
#ifndef NO_XVH
	XtAddCallback (smDD.confExit, XmNhelpCallback,
		       TopicHelpRequested, HELP_LOGOUT_STR);
#endif
	exitRec = (ExitRecord *)XtMalloc( sizeof(ExitRecord) );
	exitRec->pmsg = pmsg;
        exitRec->doSave = doSave;
	exitRec->u.exitCancelledDialog = smDD.confExit;
	XtAddCallback (smDD.confExit, XmNcancelCallback,
		       ExitCancelled, exitRec);
    }
    else
    {
	/*
	 * The user may have changed the type of session to be
	 * restored so must update the dialog's message.
	 */
	i = 0;
	if (smGD.sessionType == HOME_SESSION && 
	    smSettings.startState == DtSM_HOME_STATE) 
	{
	    XtSetArg(uiArgs[i], XmNmessageString, homeToHome); i++;
	}
	else if (smGD.sessionType == CURRENT_SESSION &&
	         smSettings.startState == DtSM_HOME_STATE) 
        {
	    XtSetArg(uiArgs[i], XmNmessageString, currentToHome); i++;
	}
	else
	{
	    XtSetArg(uiArgs[i], XmNmessageString, returnToCurrent); i++;
	}

	XtSetValues(smDD.confExit, uiArgs, i);
    }

   /*
    * Refresh buffer containing pointer to Tt_message.
    */
    i=0;
    XtSetArg(uiArgs[i], XmNuserData, &pmsg); i++;
    XtGetValues(smDD.confExit, uiArgs, i);
    *pmsg = msg;


    XtAddCallback (XtParent(smDD.confExit), XmNpopupCallback, DialogUp, NULL);
	
    XtManageChild(smDD.confExit);
    return(0);
}


/*************************************<->*************************************
 *
 *  WarnMsgFailue ()
 *
 *
 *  Description:
 *  -----------
 *  Let the user know that the bms has died and that the current session
 *  will not be saved.
 *
 *
 *  Inputs:
 *  ------
 *
 * 
 *  Outputs:
 *  -------
 *  None.
 *
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
int 
WarnMsgFailure( void )
{
    int 	i;
    XmString 	bmsDeadString;
    String	tmpString;

    if(smDD.deadWid == NULL)
    {
	bmsDeadString = XmStringCreateLocalized(((char *)GETMESSAGE(18, 38, 
		"Messaging System Inoperative\n\n\
To restart:\n\n1) Save all open data files.\n\
2) Logout.\n\
3) Log in again.\n\nNote: The current session will not be saved.\n\n\
When you are ready to begin the restart process, click [OK] and\n\
proceed to save your files.")));

	/*
	 * Now create the dialog box
	 */
	i = 0;
	tmpString = GETMESSAGE(18, 12, "Message Failure");
	XtSetArg(uiArgs[i], XmNmessageString, bmsDeadString);i++;
	XtSetArg(uiArgs[i], XmNallowShellResize, True);  i++;
	XtSetArg(uiArgs[i], XmNdialogStyle, XmDIALOG_SYSTEM_MODAL); i++;
	XtSetArg(uiArgs[i], XmNmessageAlignment, XmALIGNMENT_BEGINNING); i++;
	XtSetArg(uiArgs[i], XmNtitle, tmpString); i++;
	XtSetArg(uiArgs[i], XmNokLabelString, smDD.okString); i++;
	XtSetArg(uiArgs[i], XmNhelpLabelString, smDD.helpString); i++;
	XtSetArg(uiArgs[i], XmNautoUnmanage, False); i++;
	smDD.deadWid = XmCreateWarningDialog(smGD.topLevelWid, "deadDialog",
					     uiArgs, i);

	i = 0;
	XtSetArg(uiArgs[i], XmNuseAsyncGeometry, True);i++;
	XtSetArg(uiArgs[i], XmNmwmFunctions, 0);i++;
	XtSetArg(uiArgs[i], XmNmwmDecorations,
		 (MWM_DECOR_TITLE | MWM_DECOR_BORDER));i++;
	XtSetArg(uiArgs[i], XmNmwmInputMode, MWM_INPUT_SYSTEM_MODAL);i++;
	XtSetValues(XtParent(smDD.deadWid), uiArgs, i);
	
	XtAddCallback (XtParent(smDD.deadWid),
		       XmNpopupCallback, DialogUp, NULL);
	XtUnmanageChild(XmMessageBoxGetChild(smDD.deadWid,
					     XmDIALOG_CANCEL_BUTTON));

	/*
	 * Now add in the callback and get out of here
	 */
	XtAddCallback (smDD.deadWid, XmNokCallback,
		       SimpleOK, (XtPointer) smDD.deadWid);
#ifndef NO_XVH
	XtAddCallback (smDD.deadWid, XmNhelpCallback,
		       TopicHelpRequested, HELP_BMS_DEAD_STR);
#endif
	XtAddCallback (smDD.deadWid, XmNcancelCallback,
		       	NULL, NULL);
	XmStringFree(bmsDeadString);
    }

    XtManageChild(smDD.deadWid);
    return(0);
}


/*************************************<->*************************************
 *
 *  CompatModeExit ()
 *
 *
 *  Description:
 *  -----------
 *  Let the user know that the logout button can not be used to exit in
 *  compatibility mode.
 *
 *
 *  Inputs:
 *  ------
 *
 * 
 *  Outputs:
 *  -------
 *  None.
 *
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
static int 
CompatModeExit( void )
{
    int 	i;
    XmString 	compatModeString;
    String	tmpString;

    if(smDD.compatExit == NULL)
    {
	compatModeString = XmStringCreateLocalized(((char *)GETMESSAGE(18, 34, 
		"This session was started from an X Window System startup script.\n\n\
No session information will be saved.\n\nUse the reset key sequence to log out.")));

	/*
	 * Now create the dialog box
	 */
	i = 0;
	tmpString = GETMESSAGE(18, 35, "Logout Message");
	XtSetArg(uiArgs[i], XmNmessageString, compatModeString);i++;
	XtSetArg(uiArgs[i], XmNallowShellResize, True);  i++;
	XtSetArg(uiArgs[i], XmNdialogStyle, XmDIALOG_SYSTEM_MODAL); i++;
	XtSetArg(uiArgs[i], XmNmessageAlignment, XmALIGNMENT_CENTER); i++;
	XtSetArg(uiArgs[i], XmNtitle, tmpString); i++;
	XtSetArg(uiArgs[i], XmNokLabelString, smDD.okString); i++;
	XtSetArg(uiArgs[i], XmNhelpLabelString, smDD.helpString); i++;
	XtSetArg(uiArgs[i], XmNautoUnmanage, False); i++;
	smDD.compatExit = XmCreateInformationDialog(smGD.topLevelWid,
						    "compatDialog",
						    uiArgs, i);

	i = 0;
	XtSetArg(uiArgs[i], XmNuseAsyncGeometry, True);i++;
	XtSetArg(uiArgs[i], XmNmwmFunctions, 0);i++;
	XtSetArg(uiArgs[i], XmNmwmDecorations,
		 (MWM_DECOR_TITLE | MWM_DECOR_BORDER));i++;
    XtSetArg(uiArgs[i], XmNmwmInputMode, MWM_INPUT_SYSTEM_MODAL);i++;
	XtSetValues(XtParent(smDD.compatExit), uiArgs, i);
	
	XtAddCallback (XtParent(smDD.compatExit),
		       XmNpopupCallback, DialogUp, NULL);
	XtUnmanageChild(XmMessageBoxGetChild(smDD.compatExit,
					     XmDIALOG_CANCEL_BUTTON));

	/*
	 * Now add in the callback and get out of here
	 */
	XtAddCallback (smDD.compatExit, XmNokCallback,
		       SimpleOK, smDD.compatExit);
#ifndef NO_XVH
	XtAddCallback (smDD.compatExit, XmNhelpCallback,
		       TopicHelpRequested, HELP_LOGOUT_COMPAT_MODE_STR);
#endif
	XmStringFree(compatModeString);
    }

    XtManageChild(smDD.compatExit);
    return(0);
}



/*************************************<->*************************************
 *
 *  CreateLockDialogWithCover()
 *
 *
 *  Description:
 *  -----------
 *  Create the lock dialog when it exists as a part of a cover
 *
 *
 *  Inputs:
 *  ------
 *
 * 
 *  Outputs:
 *  -------
 *  None.
 *
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
Widget 
CreateLockDialogWithCover(
        Widget parent )
{
    int 	i;
    Widget	loginLabel, instructLabel, tmpLock;
    Widget	indFrame, loginPic, picFrame, loginFrame;
    Widget  	passwdLabel, passwdForm;
    Dimension	width;		/* width, height of drop shadow    */
    XmString	lockString, passwordString;
    char	*lockMessage;
    char	*envLog;
    Pixel   fg, bg, focus_color;         /* foreground, background colors   */

    i = 0;
    XtSetArg(uiArgs[i], XmNmarginWidth, 0); i++;
    XtSetArg(uiArgs[i], XmNmarginHeight, 0); i++;
    XtSetArg(uiArgs[i], XmNshadowType, XmSHADOW_OUT); i++;
    XtSetArg(uiArgs[i], XmNshadowThickness,5); i++;
    XtSetArg(uiArgs[i], XmNunitType, XmPIXELS); i++;
    XtSetArg(uiArgs[i], XmNresizePolicy, XmRESIZE_NONE);i++;
    XtSetArg(uiArgs[i], XmNmappedWhenManaged, False); i++;
    tmpLock = XmCreateForm(parent, "lockDialog", uiArgs, i);
    smDD.matte[1] = tmpLock;

    i = 0;
    XtSetArg(uiArgs[i], XmNshadowType, XmSHADOW_OUT); i++;
    XtSetArg(uiArgs[i], XmNshadowThickness, 2); i++;
    XtSetArg(uiArgs[i], XmNtopAttachment, XmATTACH_FORM); i++;
    XtSetArg(uiArgs[i], XmNtopOffset, 15); i++;
    XtSetArg(uiArgs[i], XmNbottomAttachment, XmATTACH_FORM); i++;
    XtSetArg(uiArgs[i], XmNbottomOffset, 15); i++;
    XtSetArg(uiArgs[i], XmNrightAttachment, XmATTACH_FORM); i++;
    XtSetArg(uiArgs[i], XmNrightOffset, 15); i++;
    picFrame = XmCreateFrame(tmpLock, "picFrame", uiArgs, i);

    i = 0;
    XtSetArg(uiArgs[i], XmNforeground, &fg); i++;
    XtSetArg(uiArgs[i], XmNbackground, &bg); i++;
    XtGetValues(tmpLock, uiArgs, i);
     
    i = 0;
    XtSetArg(uiArgs[i], XmNfillMode, XmFILL_SELF); i++;
    XtSetArg(uiArgs[i], XmNbehavior, XmICON_LABEL); i++;
    XtSetArg(uiArgs[i], XmNpixmapForeground, fg); i++;
    XtSetArg(uiArgs[i], XmNpixmapBackground, bg); i++;
    XtSetArg(uiArgs[i], XmNstring, NULL); i++;
    XtSetArg(uiArgs[i], XmNshadowThickness, 0); i++;
    XtSetArg(uiArgs[i], XmNtraversalOn, False); i++;
    loginPic = _DtCreateIcon(picFrame, "lockLabelPixmap", uiArgs, i);
     
    i = 0;
    XtSetArg(uiArgs[i], XmNshadowType, XmSHADOW_OUT); i++;
    XtSetArg(uiArgs[i], XmNshadowThickness, 2); i++; 
    XtSetArg(uiArgs[i], XmNtopAttachment, XmATTACH_FORM); i++;
    XtSetArg(uiArgs[i], XmNtopOffset, 15); i++;
    XtSetArg(uiArgs[i], XmNbottomAttachment, XmATTACH_FORM); i++;
    XtSetArg(uiArgs[i], XmNbottomOffset, 15); i++;
    XtSetArg(uiArgs[i], XmNleftAttachment, XmATTACH_FORM); i++;
    XtSetArg(uiArgs[i], XmNleftOffset, 15); i++;
    XtSetArg(uiArgs[i], XmNrightAttachment, XmATTACH_WIDGET); i++;
    XtSetArg(uiArgs[i], XmNrightWidget, picFrame); i++;
    XtSetArg(uiArgs[i], XmNrightOffset, 0); i++;
    loginFrame = XmCreateFrame(tmpLock, "loginFrame", uiArgs, i);

    /* 
     * create the login matte...
     */
    i = 0;
    XtSetArg(uiArgs[i], XmNresizePolicy, XmRESIZE_NONE);i++;
    smDD.loginMatte[1] = XmCreateForm(loginFrame, "loginMatte", uiArgs, i);

    /*
     *  create the login/password forms
     */
    i = 0;
    XtSetArg(uiArgs[i], XmNshadowThickness, 0); i++; 
    XtSetArg(uiArgs[i], XmNrightAttachment, XmATTACH_FORM); i++;
    XtSetArg(uiArgs[i], XmNrightOffset, 15); i++;
    XtSetArg(uiArgs[i], XmNbottomAttachment, XmATTACH_POSITION); i++;
    XtSetArg(uiArgs[i], XmNbottomPosition, 50); i++;
    XtSetArg(uiArgs[i], XmNleftAttachment, XmATTACH_FORM); i++;
    XtSetArg(uiArgs[i], XmNleftOffset, 15); i++;
    smDD.loginForm[1] = XmCreateForm(smDD.loginMatte[1], "loginForm", uiArgs, i);

    i = 0;
    XtSetArg(uiArgs[i], XmNresizePolicy, XmRESIZE_NONE);i++;
    XtSetArg(uiArgs[i], XmNshadowThickness, 0); i++; 
    XtSetArg(uiArgs[i], XmNtopAttachment, XmATTACH_POSITION); i++;
    XtSetArg(uiArgs[i], XmNtopPosition, 50); i++;
	XtSetArg(uiArgs[i], XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET); i++;
	XtSetArg(uiArgs[i], XmNleftWidget, smDD.loginForm[1]); i++;
	XtSetArg(uiArgs[i], XmNrightAttachment, XmATTACH_OPPOSITE_WIDGET); i++;
	XtSetArg(uiArgs[i], XmNrightWidget, smDD.loginForm[1]); i++;
    passwdForm = XmCreateForm(smDD.loginMatte[1], "passwdForm", uiArgs, i);


    /*
     *  create the login/password labels...
     */
    i = 0;
    envLog = getenv("LOGNAME");
    lockMessage = XtMalloc(100 + strlen(envLog));
    sprintf(
	lockMessage,
	((char *)GETMESSAGE(18, 1, "Display locked by user %s.")), envLog);
    lockString = XmStringCreateLocalized(lockMessage);
    XtSetArg(uiArgs[i], XmNtopAttachment, XmATTACH_POSITION); i++;
    XtSetArg(uiArgs[i], XmNtopPosition, 20); i++;
    XtSetArg(uiArgs[i], XmNalignment, XmALIGNMENT_CENTER); i++;
    XtSetArg(uiArgs[i], XmNlabelString,	lockString); i++;
    loginLabel = XmCreateLabelGadget(smDD.loginForm[1],
				     "loginLabel", uiArgs, i);
    XtManageChild(loginLabel);
    XmStringFree(lockString);
    XtFree(lockMessage);

    i = 0;
    lockString = XmStringCreateLocalized(((char *)GETMESSAGE(18, 2, "Enter password to unlock.")));
    XtSetArg(uiArgs[i], XmNtopAttachment, XmATTACH_WIDGET); i++;
    XtSetArg(uiArgs[i], XmNtopWidget, loginLabel); i++;
    XtSetArg(uiArgs[i], XmNalignment, XmALIGNMENT_CENTER); i++;
    XtSetArg(uiArgs[i], XmNlabelString,	lockString); i++;
    instructLabel = XmCreateLabelGadget(smDD.loginForm[1], "instructLabel",
					uiArgs, i);
    XtManageChild(instructLabel);
    XmStringFree(lockString);

    i = 0;
    passwordString = XmStringCreateLocalized(((char *)GETMESSAGE(18, 3, "Password: ")));
    XtSetArg(uiArgs[i], XmNtopAttachment, XmATTACH_POSITION); i++;
    XtSetArg(uiArgs[i], XmNtopPosition, 20); i++;
    XtSetArg(uiArgs[i], XmNbottomAttachment, XmATTACH_POSITION); i++;
    XtSetArg(uiArgs[i], XmNbottomPosition, 80); i++;
    XtSetArg(uiArgs[i], XmNleftOffset, 0); i++;
    XtSetArg(uiArgs[i], XmNalignment, XmALIGNMENT_END); i++;
    XtSetArg(uiArgs[i], XmNlabelString,	passwordString); i++;
    passwdLabel = XmCreateLabelGadget(passwdForm,
				      "passwdLabel", uiArgs, i);
    XtManageChild(passwdLabel);
    XmStringFree(passwordString);

    /*
     * Give the password label an offset
     */
    i = 0;
    XtSetArg(uiArgs[i], XmNwidth, &width);i++;
    XtGetValues(passwdLabel, uiArgs, i);

    i = 0;
    width += (width/6);
    XtSetArg(uiArgs[i], XmNwidth, width);i++;
    XtSetArg(uiArgs[i], XmNrecomputeSize, False);i++;
    XtSetValues(passwdLabel, uiArgs, i);


    i = 0;
    XtSetArg(uiArgs[i], XmNshadowType, XmSHADOW_IN); i++;
    XtSetArg(uiArgs[i], XmNshadowThickness, 1); i++;
    XtSetArg(uiArgs[i], XmNtopAttachment, XmATTACH_POSITION); i++;
    XtSetArg(uiArgs[i], XmNtopPosition, 20); i++;
    XtSetArg(uiArgs[i], XmNbottomAttachment, XmATTACH_POSITION); i++;
    XtSetArg(uiArgs[i], XmNbottomPosition, 80); i++;
    XtSetArg(uiArgs[i], XmNleftAttachment, XmATTACH_WIDGET); i++;
    XtSetArg(uiArgs[i], XmNleftWidget, passwdLabel); i++;
    XtSetArg(uiArgs[i], XmNrightAttachment, XmATTACH_FORM); i++;
    XtSetArg(uiArgs[i], XmNrightOffset, 10); i++;
    XtSetArg(uiArgs[i], XmNalignment, XmALIGNMENT_END); i++;
    indFrame = XmCreateFrame(passwdForm,  "indFrame", uiArgs, i);

    i = 0;
    passwordString = XmStringCreateLocalized("|");
    XtSetArg(uiArgs[i], XmNlabelString,	passwordString); i++;
    XtSetArg(uiArgs[i], XmNalignment, XmALIGNMENT_BEGINNING); i++;
    smDD.indLabel[1] = XmCreateLabel(indFrame, "indLabel",
					uiArgs, i);
    XtManageChild(smDD.indLabel[1]);
    XmStringFree(passwordString);
    
    i = 0;
    XtSetArg(uiArgs[i], XmNhighlightColor,  &focus_color); i++;
    XtGetValues(indFrame, uiArgs, i);
    XtVaSetValues ( smDD.indLabel[1],
                    XmNborderWidth, 2,
                    XmNborderColor,  focus_color,
                    NULL );
    /*
     * Manage forms AFTER all children have been managed
     */
    XtManageChild(indFrame);
    XtManageChild(passwdForm);
    XtManageChild(smDD.loginForm[1]);
    XtManageChild(smDD.loginMatte[1]);
    XtManageChild(loginPic);
    XtManageChild(picFrame);
    XtManageChild(loginFrame);
	
    return(tmpLock);
}



/*************************************<->*************************************
 *
 *  CreateCoverDialog ()
 *
 *
 *  Description:
 *  -----------
 *  Create the cover dialogs for all the screens
 *
 *
 *  Inputs:
 *  ------
 *
 * 
 *  Outputs:
 *  -------
 *  None.
 *
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
Widget 
CreateCoverDialog(
        int screenNum,
        Boolean withLock )
{
    int i;
    Widget	tmpCover, table;
    char	geomString[50];

    sprintf(geomString, "%dx%d+0+0",
	    DisplayWidth(smGD.display, screenNum),
	    DisplayHeight(smGD.display, screenNum));

    i = 0;
    XtSetArg(uiArgs[i], XmNmwmDecorations, 0);i++;
    XtSetArg(uiArgs[i], XmNgeometry, (String) geomString);i++;
    XtSetArg(uiArgs[i], XmNuseAsyncGeometry, True);i++;
    XtSetArg(uiArgs[i], XmNallowShellResize, True); i++;
    XtSetArg(uiArgs[i], XmNresizePolicy, XmRESIZE_NONE);i++;
    XtSetArg(uiArgs[i], XmNmwmInputMode, MWM_INPUT_SYSTEM_MODAL);i++;
    XtSetArg(uiArgs[i], XmNdepth, DefaultDepth(smGD.display, screenNum));i++;
    XtSetArg(uiArgs[i], XmNscreen,
	     ScreenOfDisplay(smGD.display, screenNum));i++;
    XtSetArg(uiArgs[i], XmNcolormap,
	     DefaultColormap(smGD.display, screenNum));i++;
    tmpCover = XtCreatePopupShell("coverDialog", topLevelShellWidgetClass,
				  smGD.topLevelWid,  uiArgs, i);

    i = 0;
    XtSetArg(uiArgs[i], XmNmarginWidth, 0); i++;
    XtSetArg(uiArgs[i], XmNmarginHeight, 0); i++;
    XtSetArg(uiArgs[i], XmNshadowThickness, 0); i++;
    XtSetArg(uiArgs[i], XmNheight,
             (Dimension) DisplayHeight(smGD.display, smGD.screen)); i++;
    XtSetArg(uiArgs[i], XmNwidth,
             (Dimension) DisplayWidth(smGD.display, smGD.screen)); i++;
    XtSetArg(uiArgs[i], XmNresizePolicy, XmRESIZE_NONE);i++;
    table = XmCreateDrawingArea(tmpCover, "drawArea", uiArgs, i);
    XtManageChild(table);
    smDD.coverDrawing[screenNum] = table;

    if(withLock == True)
    {
	XtAddCallback (tmpCover, XmNpopupCallback,
		       LockDialogUp, NULL);
    }

    XtRealizeWidget(tmpCover);
    
    return(tmpCover);
}


/*************************************<->*************************************
 *
 *  ExitConfirmed ()
 *
 *
 *  Description:
 *  -----------
 *  Callback that is called when user confirms the exit of a session by
 *  pressing the OK button on the confirmation dialog.  This routine just
 *  facilitates the exit process.
 *
 *
 *  Inputs:
 *  ------
 *
 * 
 *  Outputs:
 *  -------
 *  None.
 *
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
static void 
ExitConfirmed(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    ExitRecord *exitRec = (ExitRecord *)client_data;

    XtUnmanageChild(smDD.confExit);

    ImmediateExit(smSettings.startState, *exitRec->pmsg, exitRec->doSave);
}



/*************************************<->*************************************
 *
 *  ImmediateExit -
 *
 *
 *  Description:
 *  -----------
 *  This process puts in motion the exit procedure, and then exits.
 *
 *
 *  Inputs:
 *  ------
 *  mode = Whether this session should be reset or restarted
 *  msg -- if non-zero, Session_Exit request to reply/fail && destroy
 *  doSave - if True, the session will be saved.
 *
 * 
 *  Outputs:
 *  -------
 *  None.
 *
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
void 
ImmediateExit(
        int mode,
	Tt_message msg,
	Boolean doSave)
{
    long old;
    Tt_message notice;

    /*
     * Turn off SIGTERM so we don't catch one in the middle of shutting
     * down
     */
#if !defined(SVR4) && !defined(sco)
    old = sigblock(sigmask(SIGTERM));
    sigblock(sigmask(SIGHUP));
    sigblock(sigmask(SIGPIPE));
#else
    old = sighold(SIGTERM);
#endif
    /*
     *
     */
    if(smGD.bmsDead == False)
    {
	notice = (Tt_message) tttk_message_create( NULL, TT_NOTICE, TT_SESSION, NULL,
						  "XSession_Ending", NULL);
	tt_message_send( notice );
	tt_message_destroy( notice );
    }
    if (msg != 0) {
	tt_message_reply( msg );
	tt_message_destroy( msg );
    }

    if (doSave)
    {
	XEvent		next;
	Tt_message	msg;
	int		sessionType = smGD.sessionType;
	XtIntervalId	timerId;

    	msg = (Tt_message) tttk_message_create( NULL, TT_NOTICE, TT_SESSION, NULL,
					       "DtActivity_Beginning", NULL );
	tt_message_send( msg );
	tt_message_destroy( msg );

	if (smGD.sessionType == CURRENT_SESSION ||
	    smGD.sessionType == DEFAULT_SESSION)
	    SaveState (False, DtSM_CURRENT_STATE, SmSaveLocal, True, 
		 DEFAULT_INTERACT_STYLE, DEFAULT_FAST, DEFAULT_GLOBAL);
	else if (smGD.sessionType == HOME_SESSION && 
		 smSettings.startState == DtSM_CURRENT_STATE)
	    SaveState (False, DtSM_HOME_STATE, SmSaveLocal, True, 
		 DEFAULT_INTERACT_STYLE, DEFAULT_FAST, DEFAULT_GLOBAL);
        else
            SaveState (False, DtSM_HOME_STATE, SmSaveGlobal, True, 
		 DEFAULT_INTERACT_STYLE, DEFAULT_FAST, DEFAULT_GLOBAL);

	/*
	 * Restore sessionType - it may have been changed in SaveState
	 */
	smGD.sessionType = sessionType;

	XSync(smGD.display, 0);

	smGD.loggingOut = True;

        /* JET - set this here so we don't exit prematurely (while
         * handling our own SM exit callback - duh).
         */
	smGD.ExitComplete = False;

        saveTimeout = False;
	timerId = XtAppAddTimeOut (smGD.appCon, smRes.saveYourselfTimeout, 
				   SaveTimeout, NULL);

	while (smXSMP.saveState.saveComplete == False && 
	       smXSMP.saveState.shutdownCanceled == False) {
	    if (saveTimeout)
		break;
	    XtAppNextEvent(smGD.appCon, &next);
	    if (next.type != 0)
		XtDispatchEvent(&next);
	}
    }
    
    /* JET - need this, since dtsession was exiting in the
     * XtAppNextEvent above (receiving it's own EXIT SM message) This
     * is checked in SmExit() so exit's will only occur after this
     * housekeeping has been completed.
     */
    smGD.ExitComplete = True; 

    if (smXSMP.saveState.shutdownCanceled == False) {
	/* 
	 * Run the user's exit script if there is one
	 */
	if (smGD.compatMode == False) 
	{
	    StartEtc(True);    /* run sessionexit */
	}

	_DtReleaseLock(smGD.display, SM_RUNNING_LOCK);
	SM_EXIT(0);
    }
}



/*************************************<->*************************************
 *
 *  ExitCancelled ()
 *
 *
 *  Description:
 *  -----------
 *  Called when the user bails out from a logout at the confirmation dialog
 *
 *
 *  Inputs:
 *  ------
 *  client_data - tells which dialog to unmange (the query or confirm exit)
 *  msg -- if non-zero, Session_Exit request to reply/fail && destroy
 *
 * 
 *  Outputs:
 *  -------
 *  None.
 *
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
static void 
ExitCancelled(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    ExitRecord *exitRec = (ExitRecord *)client_data;

    if(XtIsManaged(exitRec->u.exitCancelledDialog))
    {
	XtUnmanageChild(exitRec->u.exitCancelledDialog);
    }

    if(smDD.smHelpDialog && XtIsManaged(smDD.smHelpDialog))
    {
	XtUnmanageChild(smDD.smHelpDialog);
    }

    if (*exitRec->pmsg != 0) {
	    tttk_message_fail(*exitRec->pmsg, TT_DESKTOP_ECANCELED, 0, 1 );
    }
    SetSystemReady();
}



/*************************************<->*************************************
 *
 *  DialogUp ()
 *
 *
 *  Description:
 *  -----------
 *  Once the dialog is managed, but not popped up - reposition it so that
 *  it appears in the middle of the screen then remove the popup callback
 *
 *
 *  Inputs:
 *  ------
 *
 * 
 *  Outputs:
 *  -------
 *  None
 *
 *
 *  Comments:
 *  --------
 *  This routine can be used for any generic SYSTEM_MODAL dialog
 * 
 *************************************<->***********************************/
void 
DialogUp(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    int 	i;
    Dimension	width, height;
    Position	x, y;
    unsigned int dpwidth, dpheight, xorg, yorg; /* JET - Xinerama */

    /*
     * Get the size of the dialog box - then compute its position
     */
    i = 0;
    XtSetArg(uiArgs[i], XmNwidth, &width);i++;
    XtSetArg(uiArgs[i], XmNheight, &height);i++;
    XtGetValues(w, uiArgs, i);
    
				/* JET - get xinerama info */
#ifdef USE_XINERAMA
                                /* use the 'prefered' screen */
    if (!_DtXineramaGetScreen(smGD.DtXineramaInfo, 
                              smRes.xineramaPreferredScreen, 
			      &dpwidth, &dpheight, &xorg, &yorg))
      {                         /* no joy here either - setup for normal */
        dpwidth = DisplayWidth(smGD.display, smGD.screen);
        dpheight = DisplayHeight(smGD.display, smGD.screen);
	xorg = yorg = 0;
      }
#else  /* no Xinerama */
    dpwidth = DisplayWidth(smGD.display, smGD.screen);
    dpheight = DisplayHeight(smGD.display, smGD.screen);
    xorg = yorg = 0;
#endif

    x = (dpwidth / 2) - (width / 2);
    y = (dpheight / 2) - (height / 2);

				/* add the x/y origins for Xinerama */
    x += xorg;
    y += yorg;

    i = 0;
    XtSetArg(uiArgs[i], XmNx, x);i++;
    XtSetArg(uiArgs[i], XmNy, y);i++;
    XtSetValues(w, uiArgs, i);

    XtRemoveCallback(w, XmNpopupCallback, DialogUp, NULL);
}




/*************************************<->*************************************
 *
 *  ShowWaitState (flag)
 *
 *
 *  Description:
 *  -----------
 *  Enter/Leave the wait state.
 *
 *
 *  Inputs:
 *  ------
 *  flag = TRUE for Enter, FALSE for Leave.
 *
 * 
 *  Outputs:
 *  -------
 *  None.
 *
 *
 *  Comments:
 *  --------
 *  Stolen from the window manager code.
 * 
 *************************************<->***********************************/

void 
ShowWaitState(
        Boolean flag )
{
    if (flag)
    {
	XGrabPointer (smGD.display, DefaultRootWindow(smGD.display), FALSE, 
		      0, GrabModeAsync, GrabModeAsync, None, 
		      smGD.waitCursor, CurrentTime);
	XGrabKeyboard (smGD.display, DefaultRootWindow(smGD.display), FALSE, 
		       GrabModeAsync, GrabModeAsync, CurrentTime);
    }
    else
    {
	XUngrabPointer (smGD.display, CurrentTime);
	XUngrabKeyboard (smGD.display, CurrentTime);
    }
    XSync(smGD.display, 0);
}


/*************************************<->*************************************
 *
 *  InitCursorInfo ()
 *
 *
 *  Description:
 *  -----------
 *  This function determines whether a server supports large cursors.  It it
 *  does large feedback cursors are used in some cases (wait state and
 *  system modal state); otherwise smaller (16x16) standard cursors are used.
 *
 *  Outputs:
 *  -------
 *  Returns true if large cursors are supported, false otherwise
 *
 *  Comments:
 *  ---------
 *  This code was stolen from the window manager
 * 
 *************************************<->***********************************/
Boolean 
InitCursorInfo( void )
{
    unsigned int cWidth;
    unsigned int cHeight;

    if (XQueryBestCursor (smGD.display, DefaultRootWindow(smGD.display),
			  32, 32, &cWidth, &cHeight))
    {
	if ((cWidth >= 32) && (cHeight >= 32))
	{
	   return(True);
	}
    }

    return(False);
}



/*************************************<->*************************************
 *
 *  LockDialogUp ()
 *
 *
 *  Description:
 *  -----------
 *  Once the lock dialog is managed, but not popped up - reposition it so that
 *  it appears in the middle of the screen then remove the popup callback
 *
 *
 *  Inputs:
 *  ------
 *
 * 
 *  Outputs:
 *  -------
 *  None.
 *
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
static void 
LockDialogUp(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    
    register int	i;
    Dimension	width, height;	/* size values returned by XtGetValues	   */
    Dimension	shadowThickness;/* size values returned by XtGetValues	   */
    unsigned int dpwidth, dpheight, xorg, yorg; /* JET - xinerama */
    
    struct
    {			/* position, size of widgets (pixels)	   */
	int x, y;
	int	width;
	int height;
	int shadow;
    } mw;	/* matte, logo, drop shadow & login matte  */

    int		width1, width2; /* general width variable		   */
    int		x1, y1;		/* general position variables		   */
    int		index;
    
				/* JET - get xinerama info */
#ifdef USE_XINERAMA
                                /* use the prefered screen */
    if (!_DtXineramaGetScreen(smGD.DtXineramaInfo, 
                              smRes.xineramaPreferredScreen, 
			      &dpwidth, &dpheight, &xorg, &yorg))
      {                         /* no joy here either - setup for normal */
        dpwidth = DisplayWidth(smGD.display, smGD.screen);
        dpheight = DisplayHeight(smGD.display, smGD.screen);
	xorg = yorg = 0;
      }
#else  /* no Xinerama */
    dpwidth = DisplayWidth(smGD.display, smGD.screen);
    dpheight = DisplayHeight(smGD.display, smGD.screen);
    xorg = yorg = 0;
#endif

    /*
     * The partial cover has widgets of index 0 - the cover has
     * index 1
     */
    if(smGD.coverScreen == True)
    {
	index = 1;
    }
    else
    {
	index = 0;
    }
    
    /*
     *  - center the main matte horizontally and vertically...
     */
    i = 0;
    XtSetArg(uiArgs[i], XmNwidth, &width); i++;
    XtSetArg(uiArgs[i], XmNheight, &height); i++;
    XtSetArg(uiArgs[i], XmNshadowThickness, &shadowThickness); i++;
    XtGetValues(smDD.matte[index], uiArgs, i);

    mw.shadow = shadowThickness;
    mw.width  = width;
    mw.height = height;
    mw.x      = (dpwidth  - mw.width)/2;
    mw.y      = (dpheight - mw.height)/2;

    if ( mw.x < 0 ) mw.x = 0;
    if ( mw.y < 0 ) mw.y = 0;
    
				/* adjust origins if using Xinerama */
    x1 = mw.x + xorg;
    y1 = mw.y + yorg;

    i = 0;

    XtSetArg(uiArgs[i], XmNx, x1); i++;
    XtSetArg(uiArgs[i], XmNy, y1); i++;

    XtSetValues(smDD.matte[index], uiArgs, i);

    /*
     *  - center the login/password frames horizontally in the login_matte...
     */
    XtSetArg(uiArgs[0], XmNwidth,  &width);
    XtGetValues(smDD.loginMatte[index], uiArgs, 1);
    width1 = (int)width;    

    XtSetArg(uiArgs[0], XmNwidth,  &width);
    XtGetValues(smDD.loginForm[index], uiArgs, 1);
    width2 = (int)width;
    
    i = 0;
    XtSetArg(uiArgs[i], XmNleftAttachment, XmATTACH_FORM); i++;
    XtSetArg(uiArgs[i], XmNleftOffset, (width1 - width2) / 2); i++;
    XtSetValues(smDD.loginForm[index],  uiArgs, i);
}



/*************************************<->*************************************
 *
 *  SimpleOK()
 *
 *
 *  Description:
 *  -----------
 *  Simply dismiss a dialog.  Does special process for a compatibility mode
 *  logout dialog and when the bms won't start.
 *
 *
 *  Inputs:
 *  ------
 *  client_data - sends in the dialog to be dismissed.
 *
 * 
 *  Outputs:
 *  -------
 *  None.
 *
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
static void 
SimpleOK(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    Widget	dismissDialog = (Widget) client_data;

    if(XtIsManaged(dismissDialog))
    {
	XtUnmanageChild(dismissDialog);
    }

    if(dismissDialog == smDD.compatExit)
    {
	SetSystemReady();
    }

    if(dismissDialog == smDD.noStart)
    {
	SM_EXIT(-1);
    }
    if(dismissDialog == smDD.clientReasons)
    {
        reasonsDialogOK = True;
    }
}



/*************************************<->*************************************
 *
 *  UpdatePasswdField ()
 *
 *
 *  Description:
 *  -----------
 *  Give the visual feedback neccessary when the user is entering a password
 *
 *
 *  Inputs:
 *  ------
 *  numChars = number of characters entered into the field
 *
 * 
 *  Outputs:
 *  -------
 *  None.
 *
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
void 
UpdatePasswdField(
        int numChars )
{
    int i, index;
    char *passwdMessage = XtMalloc(numChars + 1);
    XmString tmpString;

    if (!passwdMessage) {
      PrintErrnoError(DtError, smNLS.cantMallocErrorString);
      return;
    }

    if(numChars > 0)
    {
	strcpy(passwdMessage, "|");
	for(i = 1;i < numChars;i++)
	{
	    if(i==1)
	    	strcpy(passwdMessage, "|");
	    else
	    	strcat(passwdMessage, " ");
	}
	strcat(passwdMessage, PASSWORD_INDICATOR);
    }
    else
    {
	strcpy(passwdMessage, "|");
    }

    tmpString = XmStringCreateLocalized (passwdMessage);
    XtFree(passwdMessage);
    /*
     * Set the index for the indLabel widget
     */
    if(smGD.coverScreen == True)
    {
	index = 1;
    }
    else
    {
	index = 0;
    }
    
    i = 0;
    XtSetArg(uiArgs[i], XmNlabelString,	tmpString); i++;
    XtSetValues(smDD.indLabel[index], uiArgs, i);

    XmStringFree(tmpString);
}



/*************************************<->*************************************
 *
 *  WarnNoStartup ()
 *
 *
 *  Description:
 *  -----------
 *  When the BMS refuses to be started, warn the user about why dt is
 *  crashing and then exit.
 *
 *
 *  Inputs:
 *  ------
 *
 * 
 *  Outputs:
 *  -------
 *  None.
 *
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
int 
WarnNoStartup( void )
{
    int 	i;
    XmString 	bmsNoStartString;
    String	tmpString;

#ifdef __osf__

    bmsNoStartString = XmStringCreateLocalized(((char *)GETMESSAGE(18, 76,
        "The DT messaging system could not be started.\n\n\
To correct the problem:\n\n\
1.  Choose [OK] to return to the login screen.\n\n\
2.  Select Failsafe Session from the login screen's option\n\
         menu and log in.\n\n\
3.  Check to see that your hostname exists correctly in /etc/hosts if your\n\
     network has already been configured.\n\
4.  If your network has not yet been configured, make sure that /etc/hosts\n\
     has the following entry in it:\n\
     127.0.0.1 localhost \n\n\
For additional information, see the CDE User's Guide.")));
#else

    bmsNoStartString = XmStringCreateLocalized(((char *)GETMESSAGE(18, 36,
        "The desktop messaging system could not be started.\n\n\
To correct the problem:\n\n\
1.  Choose [OK] to return to the login screen.\n\n\
2.  Select Failsafe Session from the login screen's option\n\
	 menu and log in.\n\n\
3.  Check to see that the desktop is properly installed,\n\
	 the hostname is correct (/etc/hosts) and that the\n\
	 network is properly configured.\n\n\
For additional information, see the CDE User's Guide.")));
#endif

    /*
     * Now create the dialog box
     */
    i = 0;
    tmpString = GETMESSAGE(18, 37, "Action Required");
    XtSetArg(uiArgs[i], XmNmessageString, bmsNoStartString);i++;
    XtSetArg(uiArgs[i], XmNallowShellResize, True);  i++;
    XtSetArg(uiArgs[i], XmNdialogStyle, XmDIALOG_SYSTEM_MODAL); i++;
    XtSetArg(uiArgs[i], XmNmessageAlignment, XmALIGNMENT_BEGINNING); i++;
    XtSetArg(uiArgs[i], XmNtitle, tmpString); i++;
    XtSetArg(uiArgs[i], XmNokLabelString, smDD.okString); i++;
    XtSetArg(uiArgs[i], XmNautoUnmanage, False); i++;
    smDD.noStart = XmCreateWarningDialog(smGD.topLevelWid, "noStartDialog",
					     uiArgs, i);

    XtSetArg(uiArgs[i], XmNuseAsyncGeometry, True);i++;
    XtSetValues(XtParent(smDD.noStart), uiArgs, i);

    XtAddCallback (XtParent(smDD.noStart),
		   XmNpopupCallback, DialogUp, NULL);
    XtUnmanageChild(XmMessageBoxGetChild(smDD.noStart,
					 XmDIALOG_HELP_BUTTON));
    XtUnmanageChild(XmMessageBoxGetChild(smDD.noStart,
					 XmDIALOG_CANCEL_BUTTON));

    /*
     * Now add in the callback and get out of here
     */
    XtAddCallback (smDD.noStart, XmNokCallback,
		   SimpleOK, smDD.noStart);
    XmStringFree(bmsNoStartString);

    XtManageChild(smDD.noStart);

    return(0);
} /* END OF FUNCTION WarnNoStartup */


#ifdef __osf__

/*************************************<->*************************************
 *
 *  WarnNewProfile()
 *
 *
 *  Description:
 *  -----------
 *  Warn the user that a new .dtprofile has just been added to their $HOME
 *  directory, indicating a need to edit it and their .login/.profile files.
 *
 *
 *  Inputs:
 *  ------
 *
 * 
 *  Outputs:
 *  -------
 *  None.
 *
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
int 
WarnNewProfile( void )
{
    int       i;
    XmString  newProfileString;
    String    tmpString;

    newProfileString = XmStringCreateLocalized(((char *)GETMESSAGE(18, 99,
   "The new file '.dtprofile' has been added to your home directory.\n\
\n\
   Follow the instructions in this file to ensure that when you log in\n\
   again your '.login' or '.profile' file will be activated and \n\
   that it will interact correctly with CDE. \n\
\n\
   For additional information, see the CDE Advanced User's and System\n\
   Administrator's Guide.")));


    /*
     * Now create the dialog box
     */
    i = 0;
    tmpString = GETMESSAGE(18, 37, "Action Required");
    XtSetArg(uiArgs[i], XmNmessageString, newProfileString);i++;
    XtSetArg(uiArgs[i], XmNallowShellResize, True);  i++;
    XtSetArg(uiArgs[i], XmNdialogStyle, XmDIALOG_SYSTEM_MODAL); i++;
    XtSetArg(uiArgs[i], XmNmessageAlignment, XmALIGNMENT_BEGINNING); i++;
    XtSetArg(uiArgs[i], XmNtitle, tmpString); i++;
    XtSetArg(uiArgs[i], XmNokLabelString, smDD.okString); i++;
    XtSetArg(uiArgs[i], XmNautoUnmanage, False); i++;
    smDD.newProfile = XmCreateWarningDialog(smGD.topLevelWid, 
					    "newProfileDialog",
					    uiArgs, i);

    XtSetArg(uiArgs[i], XmNuseAsyncGeometry, True);i++;
    XtSetValues(XtParent(smDD.newProfile), uiArgs, i);

    XtAddCallback (XtParent(smDD.newProfile),
		   XmNpopupCallback, DialogUp, NULL);
    XtUnmanageChild(XmMessageBoxGetChild(smDD.newProfile,
					 XmDIALOG_HELP_BUTTON));
    XtUnmanageChild(XmMessageBoxGetChild(smDD.newProfile,
					 XmDIALOG_CANCEL_BUTTON));

    /*
     * Now add in the callback and get out of here
     */
    XtAddCallback (smDD.newProfile, XmNokCallback,
		   SimpleOK, smDD.newProfile);
    XmStringFree(newProfileString);

    XtManageChild(smDD.newProfile);

    return(0);
} /* END OF FUNCTION WarnNewProfile */


#endif


/*************************************<->*************************************
 *
 *  ConfirmSessionCreation () -
 *
 *  Description:
 *  -----------
 *
 *  Inputs:
 *  ------
 * 
 *  Outputs:
 *  -------
 *
 *  Returns True if the user says to continue the session; False otherwise
 *     or if malloc fails;
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
Boolean
ConfirmSessionCreation (
	short		session_type,
	unsigned int	_argc,
	char		**_argv)
{
	int 		i, j;
	XmString	title_string;
	XmString	ok_string;
	XmString	cancel_string;
	XmString	help_string;
	XmString	msg_string;
	Widget		tmp_widget;
	Arg      	args[20];
	char		*pch1;
	char		*pch2;
	char		*pch3;
	Dimension	width, height;
	Position	x, y;
	XEvent		next;
	int		argc = _argc;
	char		**argv = NULL;

	/*
	 * Create temporary argv because the X toolkit may remove
	 * pieces of the command line options.
	 */
	if (_argc > 0) {
		argv = (char **) XtMalloc (_argc * sizeof (char *));
		for (i = 0; i < _argc; i++)
			argv[i] = _argv[i];
	}

	if (session_type == HOME_SESSION)
		pch1 = strdup ((char *) GETMESSAGE(18, 50, "Home"));
	else
		pch1 = strdup ((char *) GETMESSAGE(18, 51, "Current"));
	if (!pch1) {
		PrintError(DtError, smNLS.cantMallocErrorString);
		SM_EXIT(1);
	}

	pch2 = strdup ((char *) GETMESSAGE(18, 52, 
		"A display-specific '%s' session was selected\nfor display '%s' but one does not exist.\n\nIf you continue, a new display-specific session will\nbe created."));
	if (!pch2) {
		PrintError(DtError, smNLS.cantMallocErrorString);
		SM_EXIT(1);
	}

	pch3 = XtMalloc (strlen (pch1) +
			     strlen (XDisplayName (getenv("DISPLAY"))) +
			     strlen (pch2) +
			     4);
	if (!pch3) {
		PrintError(DtError, smNLS.cantMallocErrorString);
		SM_EXIT(1);
	}
	(void) sprintf (pch3, pch2, pch1, XDisplayName (getenv("DISPLAY")));

	/*
	 * The X toolkit has not yet been initialized, so do it now.
	 */
	j = 0;
	XtToolkitInitialize ();
	smGD.appCon = XtCreateApplicationContext();
	smGD.display = XtOpenDisplay(smGD.appCon, NULL, argv[0], 
			SM_RESOURCE_CLASS, NULL, 0, &argc, argv);

	XtSetArg(args[j], XmNbackground,
		XBlackPixel(smGD.display, XDefaultScreen(smGD.display))); j++;
	XtSetArg(args[j], XmNmappedWhenManaged, False); j++;
	XtSetArg (args[j], XmNwidth, 1); j++;
	XtSetArg (args[j], XmNheight, 1); j++;
	tmp_widget = XtAppCreateShell (
			"foo", "foo",
			applicationShellWidgetClass,
			smGD.display, args, j);
	XtRealizeWidget(tmp_widget);

	/*
	 * Create the compound strings for the confirmation dialog
	 */
	msg_string = XmStringCreateLocalized (pch3);
	title_string = XmStringCreateLocalized(pch3);

	ok_string = XmStringCreateLocalized(((char *) GETMESSAGE(18, 39, "OK")));
	cancel_string = XmStringCreateLocalized(((char *) GETMESSAGE(18, 40, "Cancel")));
	help_string = XmStringCreateLocalized(((char *) GETMESSAGE(18, 41, "Help")));

	free (pch1); free (pch2); XtFree (pch3);

	/*
	 * Create the dialog box
	 */
	i = 0;
	XtSetArg (uiArgs[i], XmNmessageString, msg_string); i++;
	XtSetArg (uiArgs[i], XmNallowShellResize, False);  i++;
	XtSetArg (uiArgs[i], XmNdialogStyle, XmDIALOG_SYSTEM_MODAL); i++;
	XtSetArg (uiArgs[i], XmNmessageAlignment, XmALIGNMENT_BEGINNING); i++;
	XtSetArg (uiArgs[i], XmNmappedWhenManaged, True); i++;
	XtSetArg (uiArgs[i], XmNtitle, title_string); i++;
	XtSetArg (uiArgs[i], XmNokLabelString, ok_string); i++;
	XtSetArg (uiArgs[i], XmNcancelLabelString, cancel_string); i++;
	XtSetArg (uiArgs[i], XmNhelpLabelString,  help_string); i++;
	XtSetArg (uiArgs[i], XmNautoUnmanage, False); i++;

	smDD.confirmSession = XmCreateWarningDialog(
			tmp_widget, "confirmDialog", uiArgs, i);

	XtAddCallback (smDD.confirmSession, XmNokCallback, 
			ConfirmOKCB, NULL);

	/*
	 * Some help state is not yet initialized and the help
	 * callback assumes it is so some initializations must
	 * be done now.
	 */
	smDD.smHelpDialog = NULL;
	smDD.okString = ok_string;
	smGD.topLevelWid = tmp_widget;

	XtAddCallback (smDD.confirmSession, XmNhelpCallback,
		       TopicHelpRequested, HELP_CONFIRMATION_SESSION_STR);

	XtAddCallback (smDD.confirmSession, XmNcancelCallback,
			ConfirmCancelCB, NULL);
	
    	XtManageChild(smDD.confirmSession);

	/*
	 * Center the dialog on the display
	 */
	i = 0;
	XtSetArg(uiArgs[i], XmNwidth, &width); i++;
	XtSetArg(uiArgs[i], XmNheight, &height); i++;
	XtGetValues(smDD.confirmSession, uiArgs, i);

	x = (DisplayWidth(smGD.display,  XDefaultScreen(smGD.display)) / 2) 
			- (width / 2);
	y = (DisplayHeight(smGD.display, XDefaultScreen(smGD.display)) / 2) 
			- (height / 2);

	i = 0;
	XtSetArg(uiArgs[i], XmNx, x); i++;
	XtSetArg(uiArgs[i], XmNy, y); i++;
	XtSetValues(smDD.confirmSession, uiArgs, i);

	XmStringFree (msg_string);
	XmStringFree (title_string);
	XmStringFree (cancel_string);
	XmStringFree (help_string);

	/*
	 * Spin until the OK or Cancel CB is invoked
	 */
	confirmState = ConfirmationNone;
	while(1) {
		XtAppNextEvent(smGD.appCon, &next);
		if (next.type != 0)
			XtDispatchEvent(&next);
		if (confirmState == ConfirmationOK) {
			XtDestroyWidget (tmp_widget);
			XmStringFree (ok_string);
			return (True);
		}
		else if (confirmState == ConfirmationCancel) {
			XtDestroyWidget (tmp_widget);
			XmStringFree (ok_string);
			return (False);
		}
	}
}


/*************************************<->*************************************
 *
 *  ConfirmOKCB ()
 *
 *************************************<->***********************************/
static void 
ConfirmOKCB(
        Widget 			w,
        XtPointer 		client_data,
        XtPointer 		call_data )
{
	XtUnmanageChild(smDD.confirmSession);
	confirmState = ConfirmationOK;
}


/*************************************<->*************************************
 *
 *  ConfirmCancelCB ()
 * 
 *************************************<->***********************************/
static void 
ConfirmCancelCB(
        Widget 			w,
        XtPointer 		client_data,
        XtPointer 		call_data )
{
	XtUnmanageChild(smDD.confirmSession);
	confirmState = ConfirmationCancel;
}


/*************************************<->*************************************
 *
 *  PostXSMPFailureDialog () -
 *
 *  Description:
 *  -----------
 *
 *  Inputs:
 *  ------
 * 
 *  Outputs:
 *  -------
 *
 *  Returns True if the user says to continue the session; False otherwise
 *     or if malloc fails;
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
void
PostXSMPFailureDialog (
	XSMPFailure 		failure_code,
	Boolean			check_errorlog)
{
	int 		i, j;
	XmString	ok_string;
	XmString	help_string;
	XmString	msg_string;
	Arg      	args[20];
	char		*pch1;
	char		*pch2 = "";
	char		*pch3;
	char		*pch4;
	char		*pch5;
	char		*error_file;
	Dimension	width, height;
	Position	x, y;
	XEvent		next;
        int             len;

	pch1 = strdup ((char *) GETMESSAGE(40, 1, 
		"A session cannot be started because of the\nfollowing error:"));
	if (!pch1) {
		PrintError(DtError, smNLS.cantMallocErrorString);
		SM_EXIT(1);
	}

	switch (failure_code) {
		case XSMP_FAILURE_SMS_INITIALIZE:
			pch2 = strdup ((char *) GETMESSAGE (40, 2,
				"SmsInitialize failed."));
			break;
		case XSMP_FAILURE_ICE_LISTEN:
			pch2 = strdup ((char *) GETMESSAGE (40, 3,
				"IceListenForConnections failed."));
			break;
		case XSMP_FAILURE_AUTHENTICATION:
			pch2 = strdup ((char *) GETMESSAGE (40, 4,
				"The authentication file ~/.ICEauthority could not be created.\n   If the files ~/.ICEauthority-c and ~/.ICEauthority-l exist,\n   they must be removed before your session can be started."));
			break;
		case XSMP_FAILURE_ICE_ADD_WATCH:
			pch2 = strdup ((char *) GETMESSAGE (40, 5,
				"IceAddConnectionWatch failed."));
			break;
		case XSMP_FAILURE_ICE_COMPOSE_IDS:
			pch2 = strdup ((char *) GETMESSAGE (40, 6,
				"IceComposeNetworkIds failed."));
			break;
		case XSMP_FAILURE_MALLOC:
			pch2 = strdup ((char *) GETMESSAGE (40, 7,
				"Could not create the SESSION_MANAGER environment variable."));
			break;
	}
	if (!pch2) {
		PrintError(DtError, smNLS.cantMallocErrorString);
		SM_EXIT(1);
	}

	pch3 = strdup ((char *) GETMESSAGE(18, 70, 
		"See the following for more information:"));
	if (!pch3) {
		PrintError(DtError, smNLS.cantMallocErrorString);
		SM_EXIT(1);
	}

	pch4 = strdup ((char *) GETMESSAGE(18, 71, 
		"CDE Advanced Users and System's Administration Guide"));
	if (!pch4) {
		PrintError(DtError, smNLS.cantMallocErrorString);
		SM_EXIT(1);
	}

	error_file = XtMalloc(MAXPATHLEN+1);
	strcpy (error_file, "");

        /* JET - VU#497553 */
	if (check_errorlog) 
	  {
	    char		*home;

	    if (home = getenv ("HOME"))
	      {
		len = strlen(home) +
		  strlen(DtPERSONAL_CONFIG_DIRECTORY) +
		  strlen(DtERRORLOG_FILE);

		if (len > MAXPATHLEN)
		  error_file = XtRealloc(error_file, len + 1);

		sprintf (error_file, "%s/%s/%s", home, 
			 DtPERSONAL_CONFIG_DIRECTORY,
			 DtERRORLOG_FILE);
	      }
	  }

	pch5 = XtMalloc (strlen (pch1) + strlen (pch2) + strlen (pch3) + strlen (pch4) + 
				strlen (error_file) + 15);
	if (!pch5) {
		PrintError(DtError, smNLS.cantMallocErrorString);
		SM_EXIT(1);
	}
	(void) sprintf (pch5, "%s\n\n   %s\n\n%s\n\n   %s\n   %s\n",
			pch1, pch2, pch3, pch4, error_file);

	XtFree(error_file);

	/*
	 * No top level widget has been created so must create one now.
	 */
	j = 0;
	XtSetArg (args[j], XmNbackground, XBlackPixel(smGD.display, 
			XDefaultScreen(smGD.display))); j++;
	XtSetArg (args[j], XmNmappedWhenManaged, False); j++;
	XtSetArg (args[j], XmNwidth, 1); j++;
	XtSetArg (args[j], XmNheight, 1); j++;

        smGD.topLevelWid = XtAppCreateShell (SM_RESOURCE_NAME, 
			SM_RESOURCE_CLASS, applicationShellWidgetClass,
			smGD.display, args, j);

	XtRealizeWidget(smGD.topLevelWid);

	/*
	 * Create the compound strings for the confirmation dialog
	 */
	msg_string = XmStringCreateLocalized (pch5);

	ok_string = XmStringCreateLocalized(((char *) GETMESSAGE(18, 39, "OK")));
	help_string = XmStringCreateLocalized(((char *) GETMESSAGE(18, 41, "Help")));

	free (pch1); free (pch2); free (pch3); free (pch4); XtFree (pch5);

	/*
	 * Create the dialog box
	 */
	i = 0;
	XtSetArg (uiArgs[i], XmNmessageString, msg_string); i++;
	XtSetArg (uiArgs[i], XmNallowShellResize, False);  i++;
	XtSetArg (uiArgs[i], XmNdialogStyle, XmDIALOG_SYSTEM_MODAL); i++;
	XtSetArg (uiArgs[i], XmNmessageAlignment, XmALIGNMENT_BEGINNING); i++;
	XtSetArg (uiArgs[i], XmNmappedWhenManaged, True); i++;
	XtSetArg (uiArgs[i], XmNokLabelString, ok_string); i++;
	XtSetArg (uiArgs[i], XmNhelpLabelString,  help_string); i++;
	XtSetArg (uiArgs[i], XmNautoUnmanage, False); i++;

	smDD.confirmSession = XmCreateWarningDialog(
			smGD.topLevelWid, "confirmDialog", uiArgs, i);

	XtAddCallback (smDD.confirmSession, XmNokCallback, 
			XSMPFailureOKCB, NULL);

	/*
	 * Some help state is not yet initialized and the help
	 * callback assumes it is so some initializations must
	 * be done now.
	 */
	smDD.smHelpDialog = NULL;
	smDD.okString = ok_string;

	XtAddCallback (smDD.confirmSession, XmNhelpCallback,
		       TopicHelpRequested, HELP_XSMP_INIT_FAILURE_STR);

	XtUnmanageChild(XmMessageBoxGetChild(smDD.confirmSession,
					     XmDIALOG_CANCEL_BUTTON));

    	XtManageChild(smDD.confirmSession);

	/*
	 * Center the dialog on the display
	 */
	i = 0;
	XtSetArg(uiArgs[i], XmNwidth, &width); i++;
	XtSetArg(uiArgs[i], XmNheight, &height); i++;
	XtGetValues(smDD.confirmSession, uiArgs, i);

	x = (DisplayWidth(smGD.display,  XDefaultScreen(smGD.display)) / 2) 
			- (width / 2);
	y = (DisplayHeight(smGD.display, XDefaultScreen(smGD.display)) / 2) 
			- (height / 2);

	i = 0;
	XtSetArg(uiArgs[i], XmNx, x); i++;
	XtSetArg(uiArgs[i], XmNy, y); i++;
	XtSetValues(smDD.confirmSession, uiArgs, i);

	XmStringFree (msg_string);
	XmStringFree (help_string);

	/*
	 * Spin until the OK CB is invoked
	 */
	while(1) {
		XtAppNextEvent(smGD.appCon, &next);
		if (next.type != 0)
			XtDispatchEvent(&next);
	}
}


/*************************************<->*************************************
 *
 *  XSMPFailureOKCB ()
 *
 *************************************<->***********************************/
static void 
XSMPFailureOKCB(
        Widget 			w,
        XtPointer 		client_data,
        XtPointer 		call_data )
{
	XtUnmanageChild(smDD.confirmSession);
	SM_EXIT(1);
}




/*************************************<->*************************************
 *
 *  PostSaveSessionErrorDialog
 *
 *  Description:
 *  -----------
 *  Let the user know that the Save_Session message does not have the
 *  required data and the session will not be saved.
 *
 *  Inputs:
 *  ------
 * 
 *  Outputs:
 *  -------
 *
 *************************************<->***********************************/
void 
PostSaveSessionErrorDialog ( void )
{
    int 	i;
    XmString 	messageString;
    String	titleString;

    if(smDD.saveSession == NULL)
    {
	messageString = XmStringCreateLocalized (((char *) GETMESSAGE(18, 56, 
		"The session cannot be saved because the required\n\
'save_type' parameter was not in the message.\n\n\
The session will not be saved.")));

	titleString = GETMESSAGE(18, 55, "Save Session Failure");

	i = 0;
	XtSetArg (uiArgs[i], XmNmessageString,    messageString); i++;
	XtSetArg (uiArgs[i], XmNallowShellResize, True); i++;
	XtSetArg (uiArgs[i], XmNdialogStyle,      XmDIALOG_SYSTEM_MODAL); i++;
	XtSetArg (uiArgs[i], XmNmessageAlignment, XmALIGNMENT_BEGINNING); i++;
	XtSetArg (uiArgs[i], XmNtitle,            titleString); i++;
	XtSetArg (uiArgs[i], XmNokLabelString,    smDD.okString); i++;
	XtSetArg (uiArgs[i], XmNhelpLabelString,  smDD.helpString); i++;
	XtSetArg (uiArgs[i], XmNautoUnmanage,     False); i++;

	smDD.saveSession = XmCreateErrorDialog(smGD.topLevelWid, 
			"sessionSaveDialog", uiArgs, i);

	i = 0;
	XtSetArg (uiArgs[i], XmNuseAsyncGeometry, True);i++;
	XtSetArg (uiArgs[i], XmNmwmFunctions,     0);i++;
	XtSetArg (uiArgs[i], XmNmwmDecorations,
		 (MWM_DECOR_TITLE | MWM_DECOR_BORDER)); i++;
	XtSetArg (uiArgs[i], XmNmwmInputMode,     MWM_INPUT_SYSTEM_MODAL);i++;

	XtSetValues(XtParent(smDD.saveSession), uiArgs, i);
	
	XtAddCallback (XtParent(smDD.saveSession),
		       XmNpopupCallback, DialogUp, NULL);

	XtUnmanageChild (XmMessageBoxGetChild(smDD.saveSession,
					     XmDIALOG_CANCEL_BUTTON));

	XtAddCallback (smDD.saveSession, XmNokCallback,
		       SimpleOK, (XtPointer) smDD.saveSession);
#ifndef NO_XVH
	XtAddCallback (smDD.saveSession, XmNhelpCallback,
		       TopicHelpRequested, HELP_SESSION_SAVE_ERROR_STR);
#endif
	XmStringFree(messageString);
    }

    XtManageChild(smDD.saveSession);

    return;
}



/*************************************<->*************************************
 *
 *  PostReasonsDialog - posts a Warning dialog on behalf of a client
 *	that supplied a "reason" when it closed its connection
 *
 *  Inputs:
 *  ------
 * 
 *  Outputs: 	void
 *  -------
 *
 *************************************<->***********************************/
void
PostReasonsDialog (
	char			* progName,
	int			numMsgs,
	char			** message,
	Boolean			waitForResponse)
{
	XmString		msgString;
	XmString		okString;
	XmString		helpString;
	Arg      		args[20];
	char			* titleString;
	char			* str1;
	char			* str2;
	char			* str3;
	int 			i;
	int			len;
	XEvent			next;

	str1 = strdup ((char *) GETMESSAGE(40, 28, 
		"Application '%s'\nexited for the following reason:\n\n%s"));
	if (!str1) {
		PrintError(DtError, smNLS.cantMallocErrorString);
		SM_EXIT(1);
	}

	for (i = 0, len = 0; i < numMsgs; i++) 
		len += strlen (message[i]);

	str2 = XtMalloc (len + (numMsgs * 3) + 1);
	if (!str2) {
		PrintError(DtError, smNLS.cantMallocErrorString);
		SM_EXIT(1);
	}
	str2[0] = '\000';
	for (i = 0; i < numMsgs; i++) {
		strcat (str2, message[i]);
		strcat (str2, "\n");
	}

	str3 = XtMalloc (strlen (str1) + strlen (str2) + strlen (progName) + 4);
	if (!str3) {
		PrintError(DtError, smNLS.cantMallocErrorString);
		SM_EXIT(1);
	}
	(void) sprintf (str3, str1, progName, str2);

	DtMsgLogMessage (smGD.programName, DtMsgLogWarning, str3);

	free (str1);
	str1 = strdup ((char *) GETMESSAGE(40, 29, 
		"Application Close Reasons"));
	if (!str1) {
		PrintError(DtError, smNLS.cantMallocErrorString);
		SM_EXIT(1);
	}

	msgString = XmStringCreateLocalized (str3);

	okString = XmStringCreateLocalized(((char *) GETMESSAGE(18, 39, "OK")));

	helpString = XmStringCreateLocalized(((char *) GETMESSAGE(18, 41, "Help")));

	/*
	 * Create the dialog box
	 */
	i = 0;
	XtSetArg (uiArgs[i], XmNmessageString, msgString); i++;
	XtSetArg (uiArgs[i], XmNallowShellResize, False);  i++;
	XtSetArg (uiArgs[i], XmNdialogStyle, XmDIALOG_SYSTEM_MODAL); i++;
	XtSetArg (uiArgs[i], XmNmessageAlignment, XmALIGNMENT_BEGINNING); i++;
	XtSetArg (uiArgs[i], XmNmappedWhenManaged, True); i++;
	XtSetArg (uiArgs[i], XmNtitle, str1); i++;
	XtSetArg (uiArgs[i], XmNokLabelString, okString); i++;
	XtSetArg (uiArgs[i], XmNhelpLabelString, helpString); i++;
	XtSetArg (uiArgs[i], XmNautoUnmanage, False); i++;

	smDD.clientReasons = XmCreateWarningDialog(smGD.topLevelWid, 
		"clientReasons", uiArgs, i);

	i = 0;
	XtSetArg(uiArgs[i], XmNuseAsyncGeometry, True);i++;
	XtSetArg(uiArgs[i], XmNmwmFunctions, 0);i++;
	XtSetArg(uiArgs[i], XmNmwmDecorations,
		 (MWM_DECOR_TITLE | MWM_DECOR_BORDER));i++;
	XtSetArg(uiArgs[i], XmNmwmInputMode, MWM_INPUT_SYSTEM_MODAL);i++;
	XtSetValues(XtParent(smDD.clientReasons), uiArgs, i);
	
	XtAddCallback (XtParent(smDD.clientReasons),
		       XmNpopupCallback, DialogUp, NULL);

	XtUnmanageChild (XmMessageBoxGetChild (smDD.clientReasons,
					     XmDIALOG_CANCEL_BUTTON));

	/*
	 * Now add in the callback and get out of here
	 */
	XtAddCallback (smDD.clientReasons, XmNokCallback,
		       SimpleOK, (XtPointer) smDD.clientReasons);
#ifndef NO_XVH
	XtAddCallback (smDD.clientReasons, XmNhelpCallback,
		       TopicHelpRequested, HELP_APP_CLOSE_REASONS_STR);
#endif
	free (str1); 
	XtFree (str2); 
	XtFree (str3);
	XmStringFree (msgString);
	XmStringFree (okString);
	XmStringFree (helpString);

    	XtManageChild (smDD.clientReasons);

	/*
	 * Spin until the OK callback is invoked.  If a shutdown
	 * is in progress, we don't want to return until the
	 * user has acknowledged this message.
	 */
	reasonsDialogOK = False;
	if (waitForResponse) {
		while (1) {
			XtAppNextEvent (smGD.appCon, &next);
			if (next.type != 0)
				XtDispatchEvent(&next);
			if (reasonsDialogOK == True)
				break;
		}
	}
}


/*************************************<->*************************************
 *
 *  SaveTimeout
 *
 *  Description:
 *  -----------
 *  Timeout procedure that is invoked when a save timer expires.
 *
 *  Inputs:
 *  ------
 *
 *  Outputs: None
 *  -------
 *
 *************************************<->***********************************/
static void
SaveTimeout (
	XtPointer 		client_data,
	XtIntervalId 		*id)
{
	saveTimeout = True;

	if (smXSMP.saveState.shutdownCanceled == False) {
		/* 
		 * Run the user's exit script if there is one
		 */
		if (smGD.compatMode == False)
			StartEtc(True);    /* run sessionexit */

		_DtReleaseLock(smGD.display, SM_RUNNING_LOCK);
		SM_EXIT(0);
	}
}
