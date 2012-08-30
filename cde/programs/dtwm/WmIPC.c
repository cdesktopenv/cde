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
/* $TOG: WmIPC.c /main/11 1999/09/20 15:17:25 mgreess $ */
/* 
 * (c) Copyright 1997, The Open Group 
 */
/* 
 * (c) Copyright 1987, 1988, 1989, 1990, 1992, 1993 HEWLETT-PACKARD COMPANY 
 * ALL RIGHTS RESERVED 
*/ 
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = ""
#endif
#endif

/*
 * Included Files:
 */
#include "WmGlobal.h"
#include "WmProtocol.h"
#include <Xm/XmAll.h>
#include <Dt/Dt.h>
#include <Dt/DtP.h>
#include <Dt/Action.h>
#include <Dt/WsmM.h>
#include <Dt/IndicatorM.h>
#include <Dt/UserMsg.h>
#include <Dt/Icon.h>

#include "WmBackdrop.h"
#include "WmError.h"
#include "WmFunction.h"
#include "WmWrkspace.h"
#include "WmIPC.h"
#include "WmXSMP.h"
#include "WmPanelP.h"
#include "DataBaseLoad.h"


/*
 * include extern functions and definitions
 */

extern void UpdateFileTypeControlFields ( void );
extern WmScreenData * GetScreenForWindow (Window);

static void ToolTalkError(Widget, char*, Tt_status);


/*
 *   data for the "DT screen"
 */


/*
 * globals
 */
Const char *szWM_TOOL_CLASS = DtWM_TOOL_CLASS;



/******************************<->*************************************
 *
 *  dtInitialize (char * program_name, XtAppContext appContext)
 *
 *  Description:
 *  -----------
 *  Initialize the messaging mechanism
 *
 *  Inputs:
 *  ------
 *  program_name - argv[0]
 *  appContext   - used throughout the WM
 *
 *  Outputs:
 *  -------
 *
 *  Comments:
 *  --------
 *
 ******************************<->***********************************/
void 
dtInitialize(
        char *program_name,
        XtAppContext appContext )
{
    
    (void) DtAppInitialize(appContext, DISPLAY1, wmGD.topLevelW1, 
				program_name, (char *)szWM_TOOL_CLASS);

    /* 
     * Load action definitions from the action database.
     */
#ifdef DT_PERFORMANCE
_DtPerfChkpntMsgSend("Begin action database load");
#endif

    DtDbLoad(); 

#ifdef DT_PERFORMANCE
_DtPerfChkpntMsgSend("End   action database load");
#endif

} /* END OF FUNCTION dtInitialize */

/******************************<->*************************************
 *
 *  dtInitializeMessaging (Widget)
 *
 *  Description:
 *  -----------
 *  Initialize the messaging mechanisms
 *
 *  Inputs:
 *  ------
 *
 *  Outputs:
 *  -------
 *
 *  Comments:
 *  --------
 *
 ******************************<->***********************************/

void
dtInitializeMessaging(Widget toplevel)
{
    int 		i;
    WmScreenData *	pSD;
    String		sName;

    Tt_status		status;
    Tt_pattern		notice_pattern, request_pattern;
    char		*default_session;

    int			fd;
    char		*procId;
    char		*errfmt;

    Tt_callback_action NoticeMsgCB(
	Tt_message m,
	Tt_pattern p);
    Tt_callback_action RequestMsgCB(
	Tt_message m,
	Tt_pattern p);

    /*
     * Makef sure we have a ToolTalk connection
     */
    procId = tt_open();
    status = tt_ptr_error(procId);
    if (status != TT_OK) {
        errfmt =
          GETMESSAGE(2, 2, "Could not connect to ToolTalk:\n%s\n");
        ToolTalkError(toplevel, errfmt, status);
	return;
    }
    fd = tt_fd();
    status = tt_int_error(fd);
    if (status == TT_OK) {
	XtAppAddInput(XtWidgetToApplicationContext(wmGD.topLevelW), fd,
		(XtPointer)XtInputReadMask, tttk_Xt_input_handler, procId);
    } else {
        ToolTalkError(toplevel, "tt_fd()", status);
    }


    default_session = tt_default_session();
    status = tt_ptr_error(default_session);
    if (status != TT_OK) {
        errfmt = GETMESSAGE(2, 4, "Could not get default ToolTalk session:\n%s\n");
        ToolTalkError(toplevel, errfmt, status);
	return;
    }

    errfmt = GETMESSAGE(2, 5, "Error constructing ToolTalk pattern:\n%s\n");
    notice_pattern = tt_pattern_create();
    status = tt_ptr_error(notice_pattern);
    if (status != TT_OK) {
        ToolTalkError(toplevel, errfmt, status);
	return;
    }
    request_pattern = tt_pattern_create();
    status = tt_ptr_error(request_pattern);
    if (status != TT_OK) {
        ToolTalkError(toplevel, errfmt, status);
	return;
    }

    status = tt_pattern_category_set(notice_pattern, TT_OBSERVE);
    if (status != TT_OK) {
        ToolTalkError(toplevel, errfmt, status);
	return;
    }
    status = tt_pattern_category_set(request_pattern, TT_HANDLE);
    if (status != TT_OK) {
        ToolTalkError(toplevel, errfmt, status);
	return;
    }
    status = tt_pattern_scope_add(notice_pattern, TT_SESSION);
    if (status != TT_OK) {
        ToolTalkError(toplevel, errfmt, status);
	return;
    }
    status = tt_pattern_scope_add(request_pattern, TT_SESSION);
    if (status != TT_OK) {
        ToolTalkError(toplevel, errfmt, status);
	return;
    }
    status = tt_pattern_session_add(notice_pattern, default_session);
    if (status != TT_OK) {
        ToolTalkError(toplevel, errfmt, status);
	return;
    }
    status = tt_pattern_session_add(request_pattern, default_session);
    if (status != TT_OK) {
        ToolTalkError(toplevel, errfmt, status);
	return;
    }

    tt_free( default_session );
    status = tt_pattern_class_add(notice_pattern, TT_NOTICE);
    if (status != TT_OK) {
        ToolTalkError(toplevel, errfmt, status);
	return;
    }
    status = tt_pattern_state_add(notice_pattern, TT_SENT);
    if (status != TT_OK) {
        ToolTalkError(toplevel, errfmt, status);
	return;
    }
    status = tt_pattern_class_add(request_pattern, TT_REQUEST);
    if (status != TT_OK) {
        ToolTalkError(toplevel, errfmt, status);
	return;
    }
    status = tt_pattern_state_add(request_pattern, TT_SENT);
    if (status != TT_OK) {
        ToolTalkError(toplevel, errfmt, status);
	return;
    }


    /*
     * Ops handled by the notice_pattern
     */
    status = tt_pattern_op_add(notice_pattern, "DtActivity_Beginning");
    if (status != TT_OK) {
        ToolTalkError(toplevel, errfmt, status);
	return;
    }
    status = tt_pattern_op_add(notice_pattern, "DtActivity_Began");
    if (status != TT_OK) {
        ToolTalkError(toplevel, errfmt, status);
	return;
    }
    status = tt_pattern_op_add(notice_pattern, "DtTypes_Reloaded");
    if (status != TT_OK) {
        ToolTalkError(toplevel, errfmt, status);
	return;
    }

    /*
     * Ops handled by the request_pattern
     */
    status = tt_pattern_op_add(request_pattern, "DtPanel_Restore");
    if (status != TT_OK) {
        ToolTalkError(toplevel, errfmt, status);
	return;
    }
    status = tt_pattern_op_add(request_pattern, "DtWorkspace_SetCurrent");
    if (status != TT_OK) {
        ToolTalkError(toplevel, errfmt, status);
	return;
    }
    status = tt_pattern_op_add(request_pattern, "DtWorkspace_Title_Set");
    if (status != TT_OK) {
        ToolTalkError(toplevel, errfmt, status);
	return;
    }
    status = tt_pattern_op_add(request_pattern, "DtWorkspace_Add");
    if (status != TT_OK) {
        ToolTalkError(toplevel, errfmt, status);
	return;
    }
    status = tt_pattern_op_add(request_pattern, "DtWorkspace_Delete");
    if (status != TT_OK) {
        ToolTalkError(toplevel, errfmt, status);
	return;
    }
    status = tt_pattern_op_add(request_pattern, "GetWsmClients");
    if (status != TT_OK) {
        ToolTalkError(toplevel, errfmt, status);
	return;
    }

    /*
     * Register callback for the notice_pattern
     */
    status = tt_pattern_callback_add(notice_pattern, NoticeMsgCB);
    if (status != TT_OK) {
        ToolTalkError(toplevel, errfmt, status);
	return;
    }

    /*
     * Register callback for the request_pattern
     */
    status = tt_pattern_callback_add(request_pattern, RequestMsgCB);
    if (status != TT_OK) {
        ToolTalkError(toplevel, errfmt, status);
	return;
    }

    status = tt_pattern_register(notice_pattern);
    if (status != TT_OK) {
        ToolTalkError(toplevel, errfmt, status);
	return;
    }
    status = tt_pattern_register(request_pattern);
    if (status != TT_OK) {
        ToolTalkError(toplevel, errfmt, status);
	return;
    }

} /* END OF FUNCTION dtInitializeMessaging */


/******************************<->*************************************
 *
 *  dtCloseIPC ()
 *
 *  Description:
 *  -----------
 *  Shuts down the messaging mechanism
 *
 *  Inputs:
 *  ------
 *
 *  Outputs:
 *  -------
 *
 *  Comments:
 *  --------
 *  Should be done before exiting
 *
 ******************************<->***********************************/
void 
dtCloseIPC( void )
{
} /* END OF FUNCTION dtCloseIPC */



/******************************<->*************************************
 *  
 * void dtReadyNotification()
 *  
 *  Description:
 *  -----------
 *  Tells the world that we're up and ready.
 *
 *  Inputs:
 *  ------
 * 
 *  Outputs:
 *  -------
 *
 *  Comments:
 *  --------
 *  Invoked as the fitting culmination of dtwm initialization
 *
 ******************************<->***********************************/
void 
dtReadyNotification( void )
{
    SendClientMsg( wmGD.dtSmWindow,
		  (long) wmGD.xa_DT_SM_WM_PROTOCOL,
		  (long) wmGD.xa_DT_WM_READY,
		  CurrentTime, NULL, 0);

} /* END OF FUNCTION dtReadyNotification */


/******************************<->*************************************
 *
 *  WmStopWaiting ()
 *
 *  Description:
 *  -----------
 *  This is called to turn off "system busy" activity
 *
 *  Inputs:
 *  ------
 * 
 *  Outputs:
 *  -------
 *  None
 *
 *  Comments:
 *  ---------
 *  This routine relies on two globals, blinkerPCW and dtSD,
 *  on the major assumptions that:
 *     - there is just one DT Screen, with the front panel enabled
 *     - there is just one dtwmbusy control in that front panel
 ******************************<->***********************************/
void 
WmStopWaiting( void )
{
#ifdef PANELIST 
     if (wmGD.dtSD)
     {
	 WmFrontPanelSetBusy (False);
     }
#endif /* PANELIST */
} /* END OF FUNCTION WmStopWaiting */



/******************************<->*************************************
 *
 *  dtSendWorkspaceModifyNotification ()
 *
 *  Description:
 *  -----------
 *  This is called to announce that the workspace set has been
 *  modified
 *
 *  Inputs:
 *  ------
 *  pSD		- pointer to screen data
 *  aWs		- id of workspace just modified
 *  iType	- type of modification
 * 
 *  Outputs:
 *  -------
 *  None
 *
 *  Comments:
 *  ---------
 *  Sends the message:
 *
 *    "MODIFY_WORKSPACE"  "<atom_for_wsname>" "<modify_type>"
 *
 *    <modify_type> is one of:
 *				ADD
 *				DELETE
 *				BACKDROP
 *				TITLE
 ******************************<->***********************************/
void
dtSendWorkspaceModifyNotification(
	WmScreenData *pSD,
	Atom aWs,
	int iType)
{
    char        sNum[40];
    char        pch[40];
    char        pchType[40];
    Tt_message  msg;
    Tt_status   status;

    msg = tt_pnotice_create(TT_SESSION, "DtWorkspace_Modified");
    status = tt_ptr_error(msg);
    if (status != TT_OK) {
        return;
    }
    sprintf(sNum, "%d", pSD->screen);
    status = tt_message_arg_add(msg, TT_IN, Tttk_string, sNum);
    if (status != TT_OK) {
        return;
    }
    sprintf (pch, "%ld", aWs);
    status = tt_message_arg_add(msg, TT_IN, Tttk_string, pch);
    if (status != TT_OK) {
        return;
    }
    sprintf (pchType, "%d", iType);
    status = tt_message_arg_add(msg, TT_IN, Tttk_string, pchType);
    if (status != TT_OK) {
        return;
    }
    status = tt_message_send(msg);
    if (status != TT_OK) {
        return;
    }
    tt_message_destroy(msg);
}


/******************************<->*************************************
 *
 *  dtSendMarqueeSelectionNotification ()
 *
 *  Description:
 *  -----------
 *  This is called to announce marquee selection state
 *
 *  Inputs:
 *  ------
 *  pSD		- pointer to screen data
 *  type	- id of workspace just modified
 *  x		- x position of UL corner of rectangle
 *  y		- y position of UL corner of rectangle
 *  width	- width of rectangle
 *  heigth	- height of rectangle
 * 
 *  Outputs:
 *  -------
 *  None
 *
 *  Comments:
 *  ---------
 *  Sends the message:
 *
 *    "MARQUEE_SELECTION"  "<type>" "<x>" "<y>" "<width>" "<height>"
 *
 *    <modify_type> is one of:
 *				BEGIN
 *				END
 *				CONTINUE
 *				CANCEL
 ******************************<->***********************************/
void
dtSendMarqueeSelectionNotification(
	WmScreenData *pSD,
	int	type,
	Position x,
	Position y,
	Dimension width,
	Dimension height)
{
    Tt_message  msg;
    Tt_status   status;
    char        sNum[40];

    msg = tt_pnotice_create(TT_SESSION, "DtMarquee_Selection");
    status = tt_ptr_error(msg);
    if (status != TT_OK) {
        return;
    }

    sprintf(sNum, "%d", pSD->screen);
    status = tt_message_arg_add(msg, TT_IN, Tttk_string, sNum);
    if (status != TT_OK) {
        return;
    }
    if (tt_message_arg_add(msg, TT_IN, "integer", NULL) != TT_OK) {
	return;
    }
    if (tt_message_arg_ival_set(msg, 1, type) != TT_OK) {
	return;
    }
    if (tt_message_arg_add(msg, TT_IN, "integer", NULL) != TT_OK) {
	return;
    }
    if (tt_message_arg_ival_set(msg, 2, x) != TT_OK) {
	return;
    }
    if (tt_message_arg_add(msg, TT_IN, "integer", NULL) != TT_OK) {
	return;
    }
    if (tt_message_arg_ival_set(msg, 3, y) != TT_OK) {
	return;
    }
    if (tt_message_arg_add(msg, TT_IN, "integer", NULL) != TT_OK) {
	return;
    }
    if (tt_message_arg_ival_set(msg, 4, width) != TT_OK) {
	return;
    }
    if (tt_message_arg_add(msg, TT_IN, "integer", NULL) != TT_OK) {
	return;
    }
    if (tt_message_arg_ival_set(msg, 5, height) != TT_OK) {
	return;
    }
    status = tt_message_send(msg);
    if (status != TT_OK) {
        return;
    }
    tt_message_destroy(msg);
}


/******************************<->*************************************
 *
 *  NoticeMsgCB ()
 *
 *
 *  Description:
 *  -----------
 *  This is called to handle busy and stopbusy message
 *
 *  Inputs:
 *  ------
 *  m = ToolTalk message
 *  p = ToolTalk pattern
 * 
 *  Outputs:
 *  -------
 *  TT_CALLBACK_PROCESSED
 *  TT_CALLBACK_CINTINUE
 *
 *  Comments:
 *  ---------
 ******************************<->***********************************/
Tt_callback_action
NoticeMsgCB(Tt_message m, Tt_pattern p)
{
    char	*op;
    Tt_status	status;

    if (tt_message_state(m) != TT_SENT) {
	return TT_CALLBACK_CONTINUE;
    }
    op = tt_message_op(m);
    status = tt_ptr_error(op);
    if (status != TT_OK) {
	return TT_CALLBACK_CONTINUE;
    }
    if (!strcmp(op, "DtActivity_Began")) {
	WmStopWaiting();

	/* CDExc21081 */
	tt_message_destroy(m);
    }
#ifdef PANELIST 
    else if (!strcmp(op, "DtActivity_Beginning")) {
	if (wmGD.dtSD) {
	   WmFrontPanelSetBusy (True);
	}

	/* CDExc21081 */
	tt_message_destroy(m);
    }
#endif /* PANELIST */
    else if (!strcmp(op, "DtTypes_Reloaded")) {
	/*
	 * Blink busy light during reload.
	 */
	WmFrontPanelSetBusy (True);

	/* 
	 * Load action definitions from the action database.
	 */
	DtDbLoad(); 

	UpdateFileTypeControlFields();

	/*
	 * Turn off busy light.
	 */
	WmFrontPanelSetBusy (False);

	/* CDExc21081 */
	tt_message_destroy(m);
    }

    tt_free(op);
    return TT_CALLBACK_PROCESSED;

} /* END OF FUNCTION NoticeMsgCB */


/******************************<->*************************************
 *
 *  RequestMsgCB ()
 *
 *
 *  Description:
 *  -----------
 *  This is called to handle busy and stopbusy message
 *
 *  Inputs:
 *  ------
 *  m = ToolTalk message
 *  p = ToolTalk pattern
 * 
 *  Outputs:
 *  -------
 *  TT_CALLBACK_PROCESSED
 *  TT_CALLBACK_CINTINUE
 *
 *  Comments:
 *  ---------
 ******************************<->***********************************/
Tt_callback_action
RequestMsgCB(Tt_message m, Tt_pattern p)
{
    char	*op;
    Tt_status	status;

    int			screen_num;
    WmScreenData	*pSD;
    WmWorkspaceData	*pWS = NULL;
    Atom		aWs;
    char		*pch;
    String		sName;

    if (tt_message_state(m) != TT_SENT) {
	return TT_CALLBACK_CONTINUE;
    }
    op = tt_message_op(m);
    status = tt_ptr_error(op);
    if (status != TT_OK) {
	return TT_CALLBACK_CONTINUE;
    }
    if (!strcmp(op, "DtPanel_Restore")) {
	tt_message_reply(m);
	tt_message_destroy(m);

	SessionDeleteAll();

	F_Restart( DTWM_REQP_NO_CONFIRM, NULL, NULL );
    }
    else if (!strcmp(op, "DtWorkspace_SetCurrent")) {
	/*
	 * 1st arg: integer, screen number
	 * 2nd arg: string, atom of workspace name
	 */

	/* get the first arg from the message */
	tt_message_arg_ival(m, 0, &screen_num);
	pSD = &wmGD.Screens[screen_num];

	/* get the second arg from the message */
	pch = tt_message_arg_val(m, 1);

	/* retrieve the selected workspace */
	aWs = strtoul (pch, (char **) NULL, 0);
	tt_free( pch );
	pWS = GetWorkspaceData (pSD, aWs);

	if (pWS) {
	    ChangeToWorkspace (pWS);
	}

	tt_message_reply(m);
	tt_message_destroy(m);
    }
    else if (!strcmp(op, "DtWorkspace_Title_Set")) {
	/*
	 * 1st arg: integer, screen number
	 * 2nd arg: string, atom of workspace name
	 * 3rd arg: string, new name for the workspace
	 */

	/* get the first arg from the message */
	tt_message_arg_ival(m, 0, &screen_num);
	pSD = &wmGD.Screens[screen_num];

	/* get the second arg from the message */
	pch = tt_message_arg_val(m, 1);

	/* retrieve the selected workspace */
	aWs = strtoul (pch, (char **) NULL, 0);
	tt_free( pch );
	pWS = GetWorkspaceData (pSD, aWs);

	/* get the third arg from the message */
	pch = tt_message_arg_val(m, 2);

	if (pWS) {
	    ChangeWorkspaceTitle (pWS, pch);
	}
	tt_free( pch );

	tt_message_reply(m);
	tt_message_destroy(m);
    }
    else if (!strcmp(op, "DtWorkspace_Add")) {
	/*
	 * 1st arg: string, user-visible title of the workspace
	 */
	pch = tt_message_arg_val(m, 0);

	F_CreateWorkspace( pch, NULL, NULL );
	tt_free( pch );

	tt_message_reply(m);
	tt_message_destroy(m);
    }
    else if (!strcmp(op, "DtWorkspace_Delete")) {
	/*
	 * 1st arg: string, atom of workspace name
	 */
	pch = tt_message_arg_val(m, 0);

	/* retrieve the selected workspace */
	aWs = strtoul (pch, (char **) NULL, 0);
	tt_free( pch );
	sName = (String) XmGetAtomName (DISPLAY1, aWs);

	F_DeleteWorkspace( sName, NULL, NULL );

	tt_message_reply(m);
	tt_message_destroy(m);

	XtFree(sName);
    }
    else if (!strcmp(op, "GetWsmClients")) {
	/* No TT_IN args. */
	ClientData **clients;
	int nClients;

	if (GetSmClientIdClientList(&clients, &nClients))
	{
	    char *clientIds = (char *)NULL;
	    int *clientWorkspaces = (int *)NULL;
	    int clientIdLen = 0;

	    SortClientListByWorkspace(clients, nClients);

	    tt_message_arg_ival_set(m, 0, nClients);
	    if (nClients > 0)
	    {
		int i;
		char *ptr;
		ClientData *pCD;

		clientIdLen = 0;
		for (i = 0; i < nClients; i++)
		    clientIdLen += strlen(clients[i]->smClientID) + 1;

		if (((clientIds =
		      (char *)XtMalloc(clientIdLen * sizeof(char)))
		     == (char *)NULL) ||
		    ((clientWorkspaces =
		      (int *)XtMalloc(nClients * sizeof(int)))
		     == (int *)NULL))
		{
		    if (clientIds != (char *)NULL)
			XtFree(clientIds);

		    /* LATER - Right way to handle errors? */
		    tt_message_fail(m);
		    tt_message_destroy(m);
		    tt_free(op);
		    return TT_CALLBACK_PROCESSED;
		}
		else
		{
		    for (i = 0, ptr = clientIds;
			 i < nClients;
			 ptr += strlen(pCD->smClientID) + 1, i++)
		    {
			pCD = clients[i];
			strcpy(ptr, pCD->smClientID);
			clientWorkspaces[i] =
#ifdef WSM
			    pCD->pWsList[pCD->currentWsc].wsID;
#else
			    pCD->pSD->pWS->id;
#endif
		    }
		}
	    }
	    tt_message_arg_bval_set(m, 1, (unsigned char *)clientIds,
				    clientIdLen * sizeof(char));
	    tt_message_arg_bval_set(m, 2, (unsigned char *)clientWorkspaces,
				    nClients * sizeof(int));

	    if (clientIds != (char *)NULL)
		XtFree(clientIds);
	    if (clientWorkspaces != (int *)NULL)
		XtFree((char *)clientWorkspaces);
	    XtFree((char *)clients);
	}

	tt_message_reply(m);
	tt_message_destroy(m);
    } else {
	tt_free( op );
	return TT_CALLBACK_CONTINUE;
    }

    tt_free(op);
    return TT_CALLBACK_PROCESSED;

} /* END OF FUNCTION RequestMsgCB */



/******************************<->*************************************
 *
 *  ToolTalkError ()
 *
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
 *  Comments:
 *  ---------
 ******************************<->***********************************/
#define GETXMSTRING(s, m, d)	XmStringCreateLocalized(GETMESSAGE(s,m,d))

static void
OKCB (Widget dialog, XtPointer client_data, XtPointer call_data)
{
    XtUnmanageChild((Widget) client_data);
}

static void
ToolTalkError(Widget parent, char *errfmt, Tt_status status)
{
    Arg		 args[10];
    Widget	 dialog, dialogShell;
    char	*errmsg, *statmsg, *title;
    XmString	 xms_errmsg, xms_ok, xms_title;
    int		 n;

    if (! tt_is_err(status)) return;

    statmsg = tt_status_message(status);
    errmsg = XtMalloc(strlen(errfmt) + strlen(statmsg) + 2);
    sprintf(errmsg, errfmt, statmsg);

    xms_ok = GETXMSTRING(2, 3, "OK");
    xms_errmsg = XmStringCreateLocalized(errmsg);
    xms_title = GETXMSTRING(2, 1, "Dtwm - Error");

    n = 0;
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
    XtSetArg(args[n], XmNokLabelString, xms_ok); n++;
    XtSetArg(args[n], XmNdialogTitle, xms_title); n++;
    XtSetArg(args[n], XmNmessageString, xms_errmsg); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); n++;

    dialog = XmCreateErrorDialog(parent, "IconEditorError", args, n);
    XtAddCallback(dialog, XmNokCallback, OKCB, (XtPointer) dialog);
    XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON));
    XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON));

    /*
     * Disable the frame menu from dialog since we don't want the user
     * to be able to close dialogs with the frame menu
     */
    dialogShell = XtParent(dialog);
    n = 0;
    XtSetArg(args[n], XmNmwmDecorations, MWM_DECOR_BORDER | MWM_DECOR_TITLE); n++;
    XtSetValues(dialogShell, args, n);
    XtManageChild(dialog);
    XtRealizeWidget(dialogShell);

    _DtSimpleError("Dtwm", DtFatalError, NULL, errmsg);

    XtFree(errmsg);
    XmStringFree(xms_ok);
    XmStringFree(xms_errmsg);
    XmStringFree(xms_title);
}
/****************************   eof    ***************************/
