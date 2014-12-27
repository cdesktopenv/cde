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
/* $TOG: SmCommun.c /main/14 1999/09/20 13:47:25 mgreess $ */
/* 
 * (c) Copyright 1997, The Open Group 
 */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:        SmCommun.c
 **
 **  Project:     HP DT Session Manager (dtsession)
 **
 **  Description:
 **  -----------
 **  This file contains functionality needed to communicate with the
 **  other DT components.  This includes initialization and callback code.
 **
 *****************************************************************************
 *************************************<+>*************************************/
/*                                                               
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1990, 1993, 1994, 1996 Hewlett-Packard Company        
 * (c) Copyright 1993, 1994, 1996 International Business Machines Corp.       
 * (c) Copyright 1993, 1994, 1996 Sun Microsystems, Inc.                      
 * (c) Copyright 1993, 1994, 1996 Novell, Inc.                                
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

#include <stdio.h>
#ifdef _SUN_OS /* for the strtok calls */
#include <string.h>
#endif
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#if defined (USE_X11SSEXT)
#include <X11/extensions/scrnsaver.h>
#endif /* USE_X11SSEXT */
#include <Xm/Xm.h>
#include <Dt/Dt.h>
#include <Dt/DtP.h>
#include <Dt/Action.h>
#include <Dt/SessionM.h>
#include <Dt/UserMsg.h>
#include <Dt/Indicator.h>
#include <Dt/MsgLog.h>
#include <Tt/tttk.h>
#include "Sm.h"
#include "SmError.h"
#include "SmLock.h"
#include "SmCommun.h"
#include "SmRestore.h"
#include "SmSave.h"
#include "SmUI.h"
#include "SmProtocol.h"
#include "SmGlobals.h"
#include "SmXSMP.h"

/*
 * Pulic var declarations
 */
extern int  clientRunning; /* from SmConMgmt.c */
extern char **smExecArray;

/*
 * Defines for this file only
 */
#define	SS_ON		0
#define	SS_OFF		1
#define	SS_DRAW		2

/*
 * Constants for the Save_Session ToolTalk message
 */
static const char *SAVE_TYPE		= "save_type";
static const char *SHUTDOWN		= "shutdown";
static const char *INTERACT_STYLE	= "interact_style";
static const char *FAST			= "fast";
static const char *GLOBAL		= "global";
static const char *NUM_BACKUPS		= "num_sessions_backedup";
static const char *SAVE_LOCAL		= "Local";
static const char *SAVE_GLOBAL		= "Global";
static const char *SAVE_BOTH		= "Both";
static const char *INTERACT_NONE	= "None";
static const char *INTERACT_ERRORS	= "Errors";
static const char *TRUE_STR		= "True";
static const char *FALSE_STR		= "False";

/*
 * Local function definitions
 */

static void DtwmStarted(void);

static void
ProcessSaveSessionMessage (
	Tt_message 		saveMsg )
{
	int			i;

	int			saveType = -1;
	Boolean			shutdown = DEFAULT_SHUTDOWN;
	int			interactStyle = DEFAULT_INTERACT_STYLE;
	Boolean			fast = DEFAULT_FAST;
	Boolean			global = DEFAULT_GLOBAL;
	int			numSessionsBackedup;

	char			* type;
	char			* value;
	int			num_args = tt_message_args_count (saveMsg);

	Tt_message 		tmpMsg;
	Tt_status 		status;

	for (i = 0; i < num_args; i++) {

		type  = tt_message_arg_type  (saveMsg, i);
  		status = tt_ptr_error (type);
  		if (status != TT_OK || type == NULL)
			continue;

		value = tt_message_arg_val (saveMsg, i);
  		status = tt_ptr_error (value);
  		if (status != TT_OK || value == NULL)
			continue;

		if (!strcmp (type, SAVE_TYPE)) {
			if (!strcmp (value, SAVE_LOCAL))
				saveType = SmSaveLocal;
			else if (!strcmp (value, SAVE_GLOBAL))
				saveType = SmSaveGlobal;
			else if (!strcmp (value, SAVE_BOTH))
				saveType = SmSaveBoth;
			else  {
				tt_free (type);
				tt_free (value);
				break;
			}
		}
		else if (!strcmp (type, SHUTDOWN)) {
			if (!strcmp (value, TRUE_STR))
				 shutdown = True;
			else if (!strcmp (value, FALSE_STR))
				 shutdown = False;
		}
		else if (!strcmp (type, INTERACT_STYLE)){
			if (!strcmp (value, INTERACT_NONE))
				interactStyle = SmInteractStyleNone;
			else if (!strcmp (value, INTERACT_ERRORS))
				interactStyle = SmInteractStyleErrors;
		}
		else if (!strcmp (type, FAST)) {
			if (!strcmp (value, TRUE_STR))
				 fast = True;
			else if (!strcmp (value, FALSE_STR))
				 fast = False;
		}
		else if (!strcmp (type, GLOBAL)) {
			if (!strcmp (value, TRUE_STR))
				 global = True;
			else if (!strcmp (value, FALSE_STR))
				 global = False;
		}
		else if (!strcmp (type, NUM_BACKUPS)) {
			numSessionsBackedup = atoi (value);
			if (numSessionsBackedup > 0)
				smRes.numSessionsBackedup = numSessionsBackedup;
		}
		else {
			char		*pch1;
			char		*pch2;

			pch1 = strdup ((char *) GETMESSAGE (40, 16,
				"The Save_Session message contains the unrecognized argument '%s'."));
			if (pch1) {
				pch2 = XtMalloc (strlen (pch1)+strlen (type)+3);
				if (pch2) {
					(void) sprintf (pch2, pch1, type);
					DtMsgLogMessage (smGD.programName, 
						DtMsgLogWarning, pch2);
					free (pch1); free (pch2);
				}
				else {
					free (pch1);
				}
			}
		}

		tt_free (type);
		tt_free (value);
	}

	if (saveType == -1) {
		PostSaveSessionErrorDialog ();
		return;
	}

        smGD.smState = IN_PROCESS;
	tmpMsg = tttk_message_create (0, TT_NOTICE, TT_SESSION, 0,
				   "DtActivity_Beginning", 0);
	tt_message_send (tmpMsg);
	tt_message_destroy (tmpMsg);

        SaveState ((smGD.sessionType == HOME_SESSION), 
			smSettings.startState, saveType, shutdown,
			interactStyle, fast, global);

	tmpMsg = tttk_message_create (0, TT_NOTICE, TT_SESSION, 0,
				   "DtActivity_Began", 0);
	tt_message_send (tmpMsg);
	tt_message_destroy (tmpMsg);

	SetSystemReady ();
}


/*************************************<->*************************************
 *
 *  handleSessionMgrRequest ()
 *
 *
 *  Description:
 *  -----------
 *  Handle ToolTalk requests for which the session manager is responsible.
 *
 *
 *  Inputs:
 *  ------
 *  Incoming request, and the pattern it matched.
 *
 * 
 *  Outputs:
 *  -------
 *  Whether the message has been consumed.
 *
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
Tt_callback_action
handleSessionMgrRequest(
	Tt_message msg,
	Tt_pattern pat
)
{
  char *op;
  Tt_status status;
  Tt_callback_action rc;
  Boolean destroyMsg;

  if (tt_message_state( msg ) != TT_SENT) 
  {
    /* msg is a reply to ourself */
    return TT_CALLBACK_CONTINUE;
  }

  op = tt_message_op( msg );
  status = tt_ptr_error( op );
  if ((status != TT_OK) || (op == 0)) 
  {
    /* Let tttk_Xt_input_handler() Do The Right Thing */
    return TT_CALLBACK_CONTINUE;
  }
    
  destroyMsg = True;
  if (strcmp( op, SM_DISPLAY_LOCK ) == 0) 
  {
    if(smGD.smState == READY) 
    {
      smGD.smState = IN_PROCESS;
      LockDisplay(True);
    }
    rc = TT_CALLBACK_PROCESSED;
  }
  else if (strcmp( op, SM_XSESSION_EXIT ) == 0) 
  {
    if(smGD.smState == READY) 
    {
      smGD.smState = IN_PROCESS;
      ExitSession( msg );
      /*
       * The session may have been canceled so set the smState
       * back to ready.
       */
      smGD.smState = READY;
      destroyMsg = False; /* done in SmUI.c */
    }
    rc = TT_CALLBACK_PROCESSED;
  } 
  else if (strcmp( op, SM_RESOURCES_RELOAD ) == 0) 
  {
    if(smGD.smState == READY) 
    {
      smGD.smState = IN_PROCESS;
      ReloadResources();
    }
    rc = TT_CALLBACK_PROCESSED;
  } 
  else if (strcmp( op, SM_SAVE_SESSION ) == 0) 
  {
    if(smGD.smState == READY) 
    {
      smGD.smState = IN_PROCESS;
      ProcessSaveSessionMessage (msg);
    }
    rc = TT_CALLBACK_PROCESSED;
  } 
  else 
  {
    rc = TT_CALLBACK_CONTINUE;
    destroyMsg = False;
  }

  if (destroyMsg == True)
  {
    tt_message_reply( msg );
    tt_message_destroy( msg );
  }
        
  tt_free( op );
  return (rc);
}


/*************************************<->*************************************
 *
 *  StartMsgServer ()
 *
 *
 *  Description:
 *  -----------
 *  Initialize the BMS and register the session manager with it.  Then
 *  register all requests and notifications that the session manager is
 *  interested in.
 *
 *
 *  Inputs:
 *  ------
 *  app = Application context for dtsession
 *
 * 
 *  Outputs:
 *  -------
 *
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
void 
StartMsgServer(void)
{
    Boolean 	makeConnect;
    String	tmpString;
    char *	sessId;
    char *	procId;
    int		fd;
    Tt_status	status;
    
    /*
     * Before we start anything - initialize the customize data structure
     */
    smCust.screenSavChange = False;
    smCust.audioChange = False;
    smCust.keyboardChange = False;
    smCust.fontChange = False;
    smCust.pointerChange = False;
    smCust.dClickChange = False;
    smCust.preeditChange = False;
    
    makeConnect = DtAppInitialize(smGD.appCon, smGD.display, smGD.topLevelWid,
				  SM_RESOURCE_NAME,
				  DtSM_TOOL_CLASS);

    smDD.okString = XmStringCreateLocalized(((char *)GETMESSAGE(18, 39, "OK")));
    smDD.okLogoutString = XmStringCreateLocalized(((char *)GETMESSAGE(18, 39, "OK")));
    smDD.cancelLogoutString = XmStringCreateLocalized(((char *)GETMESSAGE(18,40, "Cancel")));
    smDD.helpString = XmStringCreateLocalized(((char *)GETMESSAGE(18, 41, "Help")));

    if(makeConnect == False)
    {
	smGD.bmsDead = True;
    }
    else
    {
	smGD.bmsDead = False;
    }

    procId = tt_open();
    status = tt_ptr_error( procId );
    if (status == TT_OK) {
	fd = tt_fd();
	status = tt_int_error( fd );
	if (status == TT_OK) {
	    XtAppAddInput( smGD.appCon, fd, (XtPointer)XtInputReadMask,
			   tttk_Xt_input_handler, procId );
	}
    }
    if (status != TT_OK) {
	smGD.bmsDead = True;
    }

    smGD.requests2Handle = 0;
    if (status == TT_OK) {
	smGD.requests2Handle = tt_pattern_create();
	tt_pattern_category_set( smGD.requests2Handle, TT_HANDLE );
	tt_pattern_class_add( smGD.requests2Handle, TT_REQUEST );
	tt_pattern_scope_add( smGD.requests2Handle, TT_SESSION );
	sessId = tt_default_session();
	tt_pattern_session_add( smGD.requests2Handle, sessId );
	tt_free( sessId );
	tt_pattern_op_add( smGD.requests2Handle, SM_DISPLAY_LOCK );
	tt_pattern_op_add( smGD.requests2Handle, SM_XSESSION_EXIT );
	tt_pattern_op_add( smGD.requests2Handle, SM_RESOURCES_RELOAD );
	tt_pattern_op_add( smGD.requests2Handle, SM_SAVE_SESSION );
	tt_pattern_callback_add( smGD.requests2Handle,
				 handleSessionMgrRequest );
	status = tt_pattern_register( smGD.requests2Handle );
	if (status != TT_OK) {
	    smGD.bmsDead = True;
	}
    }

    if (smGD.bmsDead) {
        char	*errfmt, *errmsg, *statmsg;

	errfmt = SmNewString((char *) GETMESSAGE (6, 1, 
	  "Could not connect to ToolTalk message server:\n%sExiting ...\n"));
        statmsg = tt_status_message(status);
        errmsg = XtMalloc(strlen(errfmt) + strlen(statmsg) + 2);
        sprintf(errmsg, errfmt, statmsg);

	PrintError(DtError, errmsg);
	SM_FREE(errfmt);
	XtFree(errmsg);
	WarnNoStartup();
    }
} /* END OF FUNCTION StartMsgServer  */


/*************************************<->*************************************
 *
 *  DtwmStarted (fields, client_data, num_words)
 *
 *
 *  Description:
 *  -----------
 *
 *  Inputs:
 *  ------
 *
 * 
 *  Outputs:
 *  -------
 *
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
static void 
DtwmStarted()
{
    smGD.dtwmRunning = True;
} /* END OF FUNCTION  DtwmStarted */


/*************************************<->*************************************
 *
 *  RestoreDefaults ()
 *
 *
 *  Description:
 *  -----------
 *  A request has come in (usually from the customizer) to restore one of
 *  the settings to their default states
 *
 *  Inputs:
 *  ------
 *
 * 
 *  Outputs:
 *  -------
 *  smToSet = (global) turns off flag of setting no longer set
 *
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
void 
RestoreDefaults(
		Atom toRestore)
{

  if(toRestore == XaDtSmScreenInfo)
  {
    smToSet.screenSavChange = False;
  }
  else if(toRestore == XaDtSmAudioInfo)
  {
    smToSet.audioChange = False;
  }
  else if(toRestore == XaDtSmKeyboardInfo)
  {
    smToSet.keyboardChange = False;
  }
  else if(toRestore == XaDtSmPointerInfo)
  {
    smToSet.pointerChange = False;
    smToSet.pointerMapChange = False;
  }
} /* END OF FUNCTION RestoreDefaults  */


/*************************************<->*************************************
 *
 *  ProcessPropertyNotify ()
 *
 *
 *  Description:
 *  -----------
 *  The customizer has changed one of the properties on the sm top level
 *  window.  This fact is remembered, so that at shutdown the information
 *  can be saved
 *
 *  Inputs:
 *  ------
 *
 * 
 *  Outputs:
 *  -------
 *  smToSet = (global) turns on flag of setting being set
 *
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
void 
ProcessPropertyNotify(
	XEvent *next)
{
  XPropertyEvent *pEvent = (XPropertyEvent *) next;

  if (pEvent->state != PropertyNewValue)
  {
    return;
  }

  if(pEvent->atom == XaDtSmScreenInfo)
  {
    smToSet.screenSavChange = True;
    smCust.screenSavChange = True;
  }
  else if(pEvent->atom == XaDtSmAudioInfo)
  {
    smToSet.audioChange = True;
    smCust.audioChange = True;
  }
  else if(pEvent->atom == XaDtSmKeyboardInfo)
  {
    smToSet.keyboardChange = True;
    smCust.keyboardChange = True;
  }
  else if(pEvent->atom == XaDtSmFontInfo)
  {
    smCust.fontChange = True;
  }
  else if(pEvent->atom == XaDtSmPreeditInfo)
  {
    smCust.preeditChange = True;
  }
  else if(pEvent->atom == XaDtSmPointerInfo)
  {
    smToSet.pointerChange = True;
    smCust.pointerChange = True;
    smToSet.pointerMapChange = True;
    smCust.dClickChange = True;
  }
} /* END OF FUNCTION RecordChanges   */



/*************************************<->*************************************
 *
 *  ProcessClientMessage(next)
 *
 *
 *  Description:
 *  -----------
 *  A client message has come from somewhere.  Process it if we know how.
 *
 *  Inputs:
 *  ------
 *  next - the client message event
 *
 * 
 *  Outputs:
 *  -------
 *
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
void 
ProcessClientMessage(
		     XEvent *next)
{
  XClientMessageEvent	*cEvent = (XClientMessageEvent *) next;

 /*
  * If this event came from the style manager - process it
  */
  if(cEvent->message_type == XaDtSmStmProtocol)
  {
    if(cEvent->data.l[0] == XaDtSmSaveToHome)
    {
     /*
      * The style manager is asking for a save home session
      */
      if(smGD.smState == READY)
      {
        int startStateOrig;
        int confirmModeOrig;
	Tt_message msg;

        smGD.smState = IN_PROCESS;
	msg = tttk_message_create( 0, TT_NOTICE, TT_SESSION, 0,
				   "DtActivity_Beginning", 0 );
	tt_message_send( msg );
	tt_message_destroy( msg );

        startStateOrig = smSettings.startState;
        confirmModeOrig = smSettings.confirmMode;

        smSettings.startState = cEvent->data.l[1];
        smSettings.confirmMode = cEvent->data.l[2];;
                
        smGD.homeSave = True;
	SaveState(True, smSettings.startState, SmSaveLocal,
			DEFAULT_SHUTDOWN, SmInteractStyleNone,
			DEFAULT_FAST, DEFAULT_GLOBAL);
        smGD.homeSave = False;

        smSettings.startState = startStateOrig;
        smSettings.confirmMode = confirmModeOrig;

	msg = tttk_message_create( 0, TT_NOTICE, TT_SESSION, 0,
				   "DtActivity_Began", 0 );
	tt_message_send( msg );
	tt_message_destroy( msg );
        SetSystemReady();
      }
    }
    else if (cEvent->data.l[0] == XaDtSmRestoreDefault)
    {
      RestoreDefaults((Atom) cEvent->data.l[1]);
    }
  }
  else if(cEvent->message_type == XaSmWmProtocol)
  {
   /*
    * If this event came from the ws manager - process it
    */
    if(cEvent->data.l[0] == XaWmExitSession)
    {
     /*
      * The ws manager is sending an exit session message
      */
      if(smGD.smState == READY)
      {
        smGD.smState = IN_PROCESS;
        ExitSession(0);
      }
    }
    else if(cEvent->data.l[0] == XaWmLockDisplay)
    {
     /*
      * The ws manager is sending an lock display message
      */
      if(smGD.smState == READY)
      {
        smGD.smState = IN_PROCESS;
        LockDisplay(True);
      }
    }
    else if(cEvent->data.l[0] == XaWmWindowAck)
    {
     /*
      * The ws manager is sending a "client has been managed"
      */
      clientRunning = True;
    }
    else if(cEvent->data.l[0] == XaWmReady)
    {
     /*
      * The ws manager is sending a "ready for clients"
      */
      smGD.dtwmRunning = True;
    }
  }
  else if (cEvent->message_type == XaDtSmStateInfo)
  {
    SmStateInfo state;
    int flags = (int)cEvent->data.l[0];

   /*
    * Session state has been changed. Get new values.
    */
    if(_DtGetSmState(smGD.display, smGD.topLevelWindow, &state) == Success)
    {
      char *newRes = malloc(BUFSIZ);

     /*
      * Copy selected changes to data areas.
      */
      if (flags & SM_STATE_START)
      {
        smSettings.startState = state.smStartState;
      }

      if (flags & SM_STATE_CONFIRM)
      {
        smSettings.confirmMode = state.smConfirmMode;
      }

      newRes[0] = '\0';
      if (flags & SM_STATE_CYCLETIMEOUT)
      {
        smSaverRes.cycleTimeout = state.smCycleTimeout;
        sprintf(newRes+strlen(newRes), "dtsession*%scycleTimeout: %d\n",
                smGD.extensionSpec,
                smSaverRes.cycleTimeout/60);
      }

      if (flags & SM_STATE_LOCKTIMEOUT)
      {
        smSaverRes.lockTimeout = state.smLockTimeout;
        sprintf(newRes+strlen(newRes), "dtsession*%slockTimeout: %d\n",
                smGD.extensionSpec,
                smSaverRes.lockTimeout/60);
      }

      if (flags & SM_STATE_SAVERTIMEOUT)
      {
        smSaverRes.saverTimeout = state.smSaverTimeout;
        sprintf(newRes+strlen(newRes), "dtsession*%ssaverTimeout: %d\n",
                smGD.extensionSpec,
                smSaverRes.saverTimeout/60);
      }

      if (flags & SM_STATE_RANDOM)
      {
        smSaverRes.random = state.smRandom;
        sprintf(newRes+strlen(newRes), "dtsession*%srandom: %s\n",
                smGD.extensionSpec,
                smSaverRes.random ? "True\n" : "False\n");
      }

      if (newRes[0] != '\0')
      {
        _DtAddToResource(smGD.display, newRes);
      }

      free(newRes);
    }
  }
  else if (cEvent->message_type == XaDtSmSaverInfo)
  {
    SmSaverInfo saver;

   /*
    * Session screen saver list has been changed. Get new values.
    */
    if(_DtGetSmSaver(smGD.display, smGD.topLevelWindow, &saver) == Success)
    {
      char *pRes;

      SM_FREE(smGD.saverList);
      smGD.saverList = SmNewString(saver.saverList);
      SM_FREE(saver.saverList);

      pRes = malloc(strlen("dtsession*saverList: ") +
                    strlen(smGD.saverList) +
                    strlen(smGD.extensionSpec) +
                    2); /* for the '/n' and '/0' */
      if (pRes)
      {
        sprintf(pRes, "dtsession*%ssaverList: %s\n",
               smGD.extensionSpec,
               smGD.saverList);
        _DtAddToResource(smGD.display, pRes);
        free(pRes);
      }

      if (smGD.saverListParse)
      {
        SM_FREE(smGD.saverListParse);
        smGD.saverListParse = NULL; 
      }
    }
  }
#if defined (USE_HPSSEXT)
  else if(cEvent->message_type == XaSmScreenSaveRet)
  {
    if(cEvent->data.l[0] == SS_ON)
    {
      if (smSaverRes.saverTimeout + smSaverRes.lockTimeout > 0 &&
          smGD.smState == READY)
      {
        smGD.smState = IN_PROCESS;
        LockDisplay(False);
      }
    }
  }
#endif /* USE_HPSSEXT */

  return;

} /* END OF FUNCTION ProcessClientMessage  */

/*************************************<->*************************************
 *
 *  ProcessScreenSaverMessage(next)
 *
 *
 *  Description:
 *  -----------
 *  A screen saver message has come from the server.  Process it if we know how.
 *
 *  Inputs:
 *  ------
 *  next - the client message event
 *
 *
 *  Outputs:
 *  -------
 *
 *
 *  Comments:
 *  --------
 *
 *************************************<->***********************************/
#if defined (USE_X11SSEXT)

void
ProcessScreenSaverMessage(
                     XEvent *next)
{
  XScreenSaverNotifyEvent *ssEvent =  (XScreenSaverNotifyEvent *) next;
  static int          ssCount = 0;

  if (ssEvent->state == ScreenSaverOn)
  { 
   /*
    * Screen saver activated. 
    */
    if (smSaverRes.saverTimeout + smSaverRes.lockTimeout > 0 &&
        smGD.smState == READY)
    {
     /*
      * Resource says to lock the display and SM is ready, so lock it.
      */
      smGD.smState = IN_PROCESS;
      LockDisplay(False);
    }
  }
}
#endif /* USE_X11SSEXT */

/*************************************<->*************************************
 *
 *  ProcessReloadActionsDatabase(void)
 *
 *
 *  Description:
 *  -----------
 *  Register for notification of action database changes and load the action
 *  database into our address space. This function can be called directly
 *  by session mgr code, or called as a callback by the actions database.
 *
 *  Inputs:
 *  ------
 *
 *  Outputs:
 *  -------
 *
 *  Comments:
 *  --------
 *  The first time in, this call registers itself as a callback routine with
 *  the actions database. The first and subsequent times in, this call will
 *  load the actions database into the session mgr address space.
 *
 *************************************<->***********************************/

void
ProcessReloadActionsDatabase(void)
{
  static int needtoregister = 1;

  if (needtoregister)
  {
   /*
    * Have not yet registered with the actions database to call this
    * callback when the database changes. Do so.
    */
    DtDbReloadNotify((DtDbReloadCallbackProc) ProcessReloadActionsDatabase,
			(XtPointer) NULL);
    needtoregister = 0;
  }

 /*
  * Our copy of the actions database must be out of date. Reload.
  */
  DtDbLoad();  
}

void
ProcessEvent(w, client_data, event, continue_to_dispatch)
  Widget w;
  XtPointer client_data;
  XEvent *event;
  Boolean *continue_to_dispatch;
{
  switch(event->type)
  {
    case ClientMessage:
      ProcessClientMessage(event);
      break;
    case PropertyNotify:
      ProcessPropertyNotify(event);
      break;
    default:
#if defined (USE_X11SSEXT)
      if (event->type == smGD.ssEventType)
        ProcessScreenSaverMessage(event);
#endif
      break;
  }
}
