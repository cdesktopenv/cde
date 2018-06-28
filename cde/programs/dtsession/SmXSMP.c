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
/*
 * (c) Copyright 1995 Digital Equipment Corporation.
 * (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 * (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 * (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994, 1995 Novell, Inc. 
 * (c) Copyright 1995 FUJITSU LIMITED.
 * (c) Copyright 1995 Hitachi.
 *
 * $TOG: SmXSMP.c /main/40 1999/01/18 15:42:07 samborn $
 */

/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:        SmXSMP.c
 **
 **  Project:     DT Session Manager (dtsession)
 **
 *****************************************************************************
 *************************************<+>*************************************/

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "SmXSMP.h"
#include "SmAuth.h"
#include "SmWatch.h"
#include "SmProp.h"
#include "Sm.h"
#include "SmUI.h"
#include "SmSave.h"
#include "SmRestore.h"
#include "SmGlobals.h"

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/Xatom.h>
#include <X11/SM/SMlib.h>

#include <Dt/MsgLog.h>
#include <Dt/Lock.h>
#include <Dt/Wsm.h>

#include <Tt/tt_c.h>

/*
 * Private constants
 */
#define ERRORMSGLEN 			256
#define GET_CLIENT_WORKSPACE_MSG	"GetWsmClients"


/*
 * Private variables
 */
static Boolean 		authenticationInitialized = False;
static char		* networkIds;

/*
 * Private functions
 */
static void InitializeXSMPGlobals ();

static void PutSessionManagerOnRootWindow (
	char			*networkIds);

static void InitializeSaveState (
	Boolean 		startUp);

static Boolean SendGetWsmClientsMessage ();

static int GetCurrentWorkspaceNumber ();

static void ProcessInteract (
	ClientRecPtr		client,
	Boolean			getWsmClientOK);

static void CancelShutdown ();

static void FreeProps (
	PropertyRecPtr		pProp);

static void ProcessSaveYourselfResponses ();


/*
 * Public variables
 */
XSMPSettings 		smXSMP;
ClientRecPtr		connectedList;
Atom 			XaSmClientId;


/*
 * Public functins
 */
void InstallIOErrorHandler ();

Status NewClientProc (
	SmsConn         	smsConn,
	SmPointer       	managerData,
	unsigned long   	*maskRet,
	SmsCallbacks    	*callbacksRet,
	char            	**failureReasonRet);

void NewConnectionXtProc (
	XtPointer		client_data,
	int			*source,
	XtInputId		*id);

Status RegisterClientProc (
	SmsConn 		smsConn,
	SmPointer 		managerData,
	char 			*previousId);

void InteractRequestProc (
	SmsConn 		smsConn,
	SmPointer  		managerData,
	int			dialogType);

void InteractDoneProc (
	SmsConn			smsConn,
	SmPointer 		managerData,
	Bool			cancelShutdown);

void SaveYourselfReqProc (
	SmsConn     		smsConn,
	SmPointer   		managerData,
	int	    		saveType,
	Bool	    		shutdown,
	int         		interactStyle,
	Bool        		fast,
	Bool        		global);

void SaveYourselfPhase2ReqProc (
	SmsConn     		smsConn,
	SmPointer   		managerData);

void SaveYourselfDoneProc (
	SmsConn     		smsConn,
	SmPointer 		managerData,
	Bool			success);

void CloseConnectionProc (
	SmsConn 		smsConn,
	SmPointer  		managerData,
	int			count,
	char 			**reasonMsgs);

void CompleteXSMPSave ();

void CloseDownClient (
	ClientRecPtr		client );

/*
 * List manipulation functions
 */
void AddClient (
	ClientRecPtr		newClient);

/*
 * Functions
 */

Boolean InitXSMP (void)
{
	char		errorMsg[ERRORMSGLEN];
	char		*env;
	int		i;

	InitializeXSMPGlobals ();

	InstallIOErrorHandler ();

	if (!SmsInitialize (SM_VENDOR_NAME, SM_RELEASE_NAME, 
				NewClientProc, NULL,
        			HostBasedAuthProc, 
				ERRORMSGLEN, errorMsg)) {
		DtMsgLogMessage (smGD.programName, DtMsgLogError, errorMsg);
		PostXSMPFailureDialog (XSMP_FAILURE_SMS_INITIALIZE, True);
	}

	if (!IceListenForConnections (&smXSMP.numTransports, 
				      &smXSMP.listenObjs, 
				      ERRORMSGLEN, errorMsg)) {
		DtMsgLogMessage (smGD.programName, DtMsgLogError, errorMsg);
		PostXSMPFailureDialog (XSMP_FAILURE_ICE_LISTEN, True);
	}

	if (!SetAuthentication (smXSMP.numTransports, 
			        smXSMP.listenObjs, 
				&smXSMP.authDataEntries))
		PostXSMPFailureDialog (XSMP_FAILURE_AUTHENTICATION, False);

	authenticationInitialized = True;

	if (!InitWatchProcs (smGD.appCon))
		PostXSMPFailureDialog (XSMP_FAILURE_ICE_ADD_WATCH, False);

	for (i = 0; i < smXSMP.numTransports; i++) {
		XtAppAddInput (smGD.appCon,
			IceGetListenConnectionNumber (smXSMP.listenObjs[i]),
			(XtPointer) XtInputReadMask,
			NewConnectionXtProc, (XtPointer) smXSMP.listenObjs[i]);
	}

	networkIds = IceComposeNetworkIdList (smXSMP.numTransports, 
				smXSMP.listenObjs); 
	if (!networkIds)
		PostXSMPFailureDialog (XSMP_FAILURE_ICE_COMPOSE_IDS, False);

	env = (char *) XtMalloc (strlen (SM_SESSION_MANAGER) + 
			         strlen (networkIds) + 2);
	if (!env) {
		free (networkIds);
		PostXSMPFailureDialog (XSMP_FAILURE_MALLOC, False);
	} else {
		(void) sprintf (env, "%s=%s", SM_SESSION_MANAGER, networkIds);
		(void) putenv (env);
	}

	PutSessionManagerOnRootWindow (networkIds);

	XaSmClientId = XInternAtom(smGD.display, SM_CLIENT_ID, False);

#ifdef DEBUG
	printf ("%s\n", env);
#endif /* DEBUG */

	return (True);
}


static void
PutSessionManagerOnRootWindow (
	char			*networkIds)
{
	Atom     		sessionManager;

	sessionManager = XInternAtom(smGD.display, SM_SESSION_MANAGER, False);

	XChangeProperty(smGD.display, RootWindow(smGD.display, 0),
		sessionManager, XA_STRING, 8, PropModeReplace,
		(unsigned char *) networkIds, strlen((char *)networkIds));
}


static void
InitializeSaveState (
	Boolean			startUp)
{
	smXSMP.saveState.global = False;
	smXSMP.saveState.shutdown = False;
	smXSMP.saveState.interactStyle = SmInteractStyleAny;
	smXSMP.saveState.clientInteracting = False;
	smXSMP.saveState.inProgress = False;
	smXSMP.saveState.doneSuccess = True;
	smXSMP.saveState.saveComplete = False;
	smXSMP.saveState.interactCount = 0;
	smXSMP.saveState.numClientIds = 0;
	smXSMP.saveState.interactClient = NULL;
	if (startUp) {
		smXSMP.saveState.clientIds = NULL;
		smXSMP.saveState.workspaceNums = NULL;
	} else {
		if (smXSMP.saveState.clientIds) {
			free (smXSMP.saveState.clientIds);
			smXSMP.saveState.clientIds = NULL;
		}
		if (smXSMP.saveState.workspaceNums) {
			free (smXSMP.saveState.workspaceNums);
			smXSMP.saveState.workspaceNums = NULL;
		}
	}
}

static void 
InitializeXSMPGlobals (void)
{
	smXSMP.authDataEntries = NULL;

	connectedList = NULL;	

	smXSMP.xsmpDbList = NULL;
	
	smXSMP.dbVersion = SM_VENDOR_NAME;

	smXSMP.dbSessionId = SM_RELEASE_NAME;

	InitializeSaveState (True);
}


/*ARGSUSED*/
Status
NewClientProc (
	SmsConn         	smsConn,
	SmPointer       	managerData,
	unsigned long   	*maskRet,
	SmsCallbacks    	*callbacksRet,
	char            	**failureReasonRet)
{
	ClientRecPtr 		newClient;
	
#ifdef DEBUG
	(void) printf ("\nNewClientProc: IceConn fd = %d\n",
			IceConnectionNumber (SmsGetIceConnection (smsConn)));
#endif /* DEBUG */

	newClient = (ClientRecPtr) XtMalloc (sizeof (ClientRec));
	if (!newClient) {
		char		*str;

		str = strdup ((char *) GETMESSAGE (4, 5, 
			"Unable to malloc memory for operation."));
		if (str) {
			if ((*failureReasonRet = (char *) 
				XtMalloc ( strlen (str) + 1)) != NULL) 
				strcpy (*failureReasonRet, str);
      
			DtMsgLogMessage (smGD.programName, DtMsgLogError, str);
			free (str);
		}

		return (0);
	}

	*maskRet = 0;

	newClient->smConn = smsConn;
	newClient->iceConn = SmsGetIceConnection (smsConn);
	newClient->clientId = NULL;
	newClient->clientHost = NULL;
	newClient->screenNum = 0;
	newClient->restartHint = SmRestartIfRunning;
	newClient->props = NULL;
	newClient->active = False;
	newClient->saveYourselfDone = False;
	newClient->saveYourselfP2Requested = False;
	newClient->interactRequested = False;
	newClient->next = NULL;

	AddClient (newClient);

	*maskRet |= SmsRegisterClientProcMask;
	callbacksRet->register_client.callback = RegisterClientProc;
	callbacksRet->register_client.manager_data = (SmPointer) newClient;

	*maskRet |= SmsInteractRequestProcMask;
	callbacksRet->interact_request.callback = InteractRequestProc;
	callbacksRet->interact_request.manager_data = (SmPointer) newClient;

	*maskRet |= SmsInteractDoneProcMask;
	callbacksRet->interact_done.callback = InteractDoneProc;
	callbacksRet->interact_done.manager_data = (SmPointer) newClient;

	*maskRet |= SmsSaveYourselfRequestProcMask;
	callbacksRet->save_yourself_request.callback = SaveYourselfReqProc;
	callbacksRet->save_yourself_request.manager_data = 
		(SmPointer) newClient;

	*maskRet |= SmsSaveYourselfP2RequestProcMask;
	callbacksRet->save_yourself_phase2_request.callback =
		SaveYourselfPhase2ReqProc;
	callbacksRet->save_yourself_phase2_request.manager_data =
		(SmPointer) newClient;

	*maskRet |= SmsSaveYourselfDoneProcMask;
	callbacksRet->save_yourself_done.callback = SaveYourselfDoneProc;
	callbacksRet->save_yourself_done.manager_data  = (SmPointer) newClient;

	*maskRet |= SmsCloseConnectionProcMask;
	callbacksRet->close_connection.callback = CloseConnectionProc;
	callbacksRet->close_connection.manager_data = (SmPointer) newClient;

	*maskRet |= SmsSetPropertiesProcMask;
	callbacksRet->set_properties.callback = SetPropertiesProc;
	callbacksRet->set_properties.manager_data = (SmPointer) newClient;

	*maskRet |= SmsDeletePropertiesProcMask;
	callbacksRet->delete_properties.callback = DeletePropertiesProc;
	callbacksRet->delete_properties.manager_data = (SmPointer) newClient;

	*maskRet |= SmsGetPropertiesProcMask;
	callbacksRet->get_properties.callback = GetPropertiesProc;
	callbacksRet->get_properties.manager_data = (SmPointer) newClient;

	return (True);
}


/*ARGSUSED*/
void
NewConnectionXtProc (
	XtPointer		client_data,
	int			*source,
	XtInputId		*id)
{
	IceConn			ice_conn;
	IceAcceptStatus		status;

#ifdef DEBUG
	(void) printf ("NewConnectionXtProc [fd = %d]\n", *source);
#endif /* DEBUG */

	if (smXSMP.saveState.shutdown == True)
	{
		/*
		 * Don't accept new connections if we are in the middle
		 * of a shutdown.
		 */

		return;
	}

	ice_conn = IceAcceptConnection((IceListenObj) client_data, &status);

    	if (!ice_conn) {
		char		*message;

		message = strdup ((char *) GETMESSAGE (40, 20, 
				"IceAcceptConnection failed."));
		
		if (message) {
			DtMsgLogMessage (smGD.programName, DtMsgLogError, 
					message);
			free (message);
		}
    	} else {
		IceConnectStatus cstatus;

		while ((cstatus = IceConnectionStatus (ice_conn)) ==
				IceConnectPending) {
	    		XtAppProcessEvent (smGD.appCon, XtIMAll);
		}

		if (cstatus == IceConnectAccepted) {
#ifdef DEBUG
			char			*connstr;
			printf ("ICE Connection opened IceConn fd = %d, ",
				IceConnectionNumber (ice_conn));
			connstr = IceConnectionString (ice_conn);
			printf ("\tAccept at networkId %s\n\n", connstr);
			free (connstr);
#else
			return;
#endif /* DEBUG */
		} else {
#ifdef DEBUG
			if (cstatus == IceConnectIOError)
		    		printf ("IO error opening ICE Connection!\n");
			else
		    		printf ("ICE Connection rejected!\n");
#endif /* DEBUG */
		}
    	}
}



Status 
RegisterClientProc (
	SmsConn 		smsConn,
	SmPointer 		managerData,
	char 			*previousId)
{
	ClientRec   		*client = (ClientRec *) managerData;
	char			*id = previousId;
	Boolean			sendSave = False;
	char			*pchar;
#ifdef DEBUG
	int			i;
#endif /* DEBUG */

#ifdef DEBUG
	(void) printf ("Received REGISTER CLIENT [%d] - id = %s\n", 
			smsConn, previousId ? previousId : "New Client");
#endif /* DEBUG */

	if (!previousId) {
		id = SmsGenerateClientID (smsConn);
		sendSave = True;
	}
	else {
		ClientRecPtr		pClientRec;
		XSMPClientDBRecPtr	pDbRec;
		Boolean			found = False;

		for (pClientRec = connectedList; pClientRec != NULL; 
			pClientRec = pClientRec->next) {
			if (!strcmp (pClientRec->clientId, previousId)) {
#ifdef DEBUG
				(void) printf ("\tAlready connected.\n");
#endif /* DEBUG */
				if (!pClientRec->active)
					/*
					 * A client that terminated is 
					 * re-using its id
					 */
					found = True;
				break;
			}
		}

		for (pDbRec = smXSMP.xsmpDbList; 
		     pDbRec != NULL && found == False; 
		     pDbRec = pDbRec->next) {
			if (!strcmp (pDbRec->clientId, previousId)) {
#ifdef DEBUG
				(void) printf ("\tClient in DB.\n");
#endif /* DEBUG */
				found = True;
			}
		}

		if (!found) {
			/*
			 * The client is using an invalid id or
			 * this clientID is already being used.
			 * Reject the connection.
			 */
#ifdef DEBUG
			(void) printf ("\tID is NOT valid.\n");
#endif /* DEBUG */
			free (previousId);
			return (0);
		}
	}

	client->clientId = strdup (id);
	pchar = SmsClientHostName (smsConn);
	if (pchar)
		client->clientHost = (strchr (pchar, '/')) + 1;
	else
		client->clientHost = pchar;
	client->active = True;

	SmsRegisterClientReply (smsConn, id);

	if (sendSave)
		SmsSaveYourself (smsConn, SmSaveLocal, False, 
				SmInteractStyleNone, False);

#ifdef DEBUG
	(void) printf ("CLIENTS REGISTERED:\n");

	for (i = 1, client = connectedList; 
	     client != NULL; 
	     i++, client = client->next) {

		if (client->active)
			(void) printf ("\t[%2d] = %s\n", i, client->clientId);
	}
#endif /* DEBUG */

    	return (1);
}


/*ARGSUSED*/
void 
InteractRequestProc (
	SmsConn 		smsConn,
	SmPointer  		managerData,
	int			dialogType)
{
	ClientRecPtr   		client = (ClientRecPtr) managerData;
	Boolean			getWsmClientOK = True;

#ifdef DEBUG
	(void) printf ("Received INTERACT REQUEST [%d]\n", smsConn);
	if (dialogType == SmDialogError)
		(void) printf ("\tSmDialogError\n");
	else if (dialogType == SmDialogNormal)
		(void) printf ("\tSmDialogNormal\n");
	else
		(void) printf ("\tSMlib Error: should have checked for bad value\n");
#endif /* DEBUG */

	client->interactRequested = True;

	if (smXSMP.saveState.interactCount == 0) {
		/*
		 * Only need to get the Wkspace list once for a save
		 */
	        smXSMP.saveState.interactCount++;

		if (!SendGetWsmClientsMessage ()) {
			char		*pch;

			pch = strdup ((char *) GETMESSAGE (40, 17, 
				"An attempt to get a client list from the 'Window Manager' failed."));
			if (pch) {
				DtMsgLogMessage (smGD.programName, 
						 DtMsgLogWarning, pch);
				free (pch);
			}
			getWsmClientOK = False;
		} else {
			/*
		 	 * Cann't do anything else until the GetWsmClients
		 	 * message handler is invoked.
			 *
			 * Must cache this client because it is needed in the 
			 * GetWsmClients callback and ToolTalk apparently 
			 * doesn't allow 'client_data' to be assigned to its
			 * callback funtions.
		         */
			smXSMP.saveState.interactClient = client;
			return;
		}
	}

	ProcessInteract (client, getWsmClientOK);
}


/*ARGSUSED*/
void 
InteractDoneProc (
	SmsConn	smsConn,
	SmPointer 		managerData,
	Bool			cancelShutdown)
{
	ClientRecPtr   		client = (ClientRecPtr) managerData;

#ifdef DEBUG
	(void) printf ("Received INTERACT DONE [%d] - Cancel Shutdown = %s\n",
			smsConn, cancelShutdown ? "True" : "False");
#endif /* DEBUG */

	client->interactRequested = False;
	smXSMP.saveState.clientInteracting = False;

	if (cancelShutdown)
		smXSMP.saveState.shutdownCanceled = True;

	if (cancelShutdown &&
	    smXSMP.saveState.shutdown == True &&
	    (smXSMP.saveState.interactStyle == SmInteractStyleErrors ||
	    (smXSMP.saveState.interactStyle == SmInteractStyleAny))) {
		
		ClientRecPtr		pClientRec;
		char			*pch;

		for (pClientRec = connectedList; pClientRec != NULL; 
			pClientRec = pClientRec->next) {

			SmsShutdownCancelled (pClientRec->smConn);
#ifdef DEBUG
			(void) printf ("Sent ShutdownCancelled to %d\n",
					pClientRec->smConn);
#endif /* DEBUG */
		}

		pch = strdup ((char *) GETMESSAGE (40, 22, "A session shutdown was cancelled by the application '%s'."));
		if (pch) {
			DtMsgLogMessage (smGD.programName, 
					 DtMsgLogInformation,
					 pch, 
					 GetArrayPropertyValue (client, 
						SmProgram));
			free (pch);
		}

		return;
    	}

	ProcessInteract (client, True);
}


/*ARGSUSED*/
void 
SaveYourselfReqProc (
	SmsConn     		smsConn,
	SmPointer   		managerData,
	int	    		saveType,
	Bool	    		shutdown,
	int         		interactStyle,
	Bool        		fast,
	Bool        		global)
{
	ClientRecPtr		tmpClient;
	ClientRecPtr		pClientRec;
	Boolean			notify = True;

#ifdef DEBUG
	(void) printf ("Received SAVE YOURSELF REQUEST [%d].\n", smsConn);
	(void) printf ("\tglobal   = %s\n", global ? "True" : "False");
	(void) printf ("\tshutdown = %s\n", shutdown ? "True" : "False");
	(void) printf ("\tfast     = %s\n", fast ? "True" : "False");
	(void) printf ("\tsaveType = ");
	switch (saveType) {
		case SmSaveLocal:  printf ("SmSaveLocal\n"); break;
		case SmSaveGlobal: printf ("SmSaveGlobal\n"); break;
		case SmSaveBoth:   printf ("SmSaveBoth\n"); break;
		default:           printf ("save type NOT supported\n");
	}
	(void) printf ("\tinteractStyle = ");
	switch (saveType) {
		case SmInteractStyleNone:  printf ("SmInteractStyleNone\n"); 
			break;
		case SmInteractStyleErrors: printf ("SmInteractStyleErrors\n"); 
			break;
		case SmInteractStyleAny:   printf ("SmInteractStyleAny\n"); 
			break;
		default:           printf ("interact style NOT supported\n");
	}
#endif /* DEBUG */

	if (smXSMP.saveState.inProgress) {
		char		*pch;

		pch = strdup ((char *) GETMESSAGE (40, 21, "The session will not be saved because a Save Session is in progress."));
		if (pch) {
			DtMsgLogMessage (smGD.programName, DtMsgLogError, pch);
			free (pch);
		}
		return;
	}

	if (!global) {
		/*
		 * The client wants to be told to save itself but
		 * no other clients should be notified.  [smsConn
		 * will be NULL if this non-global save came from
		 * a non-XSMP client (e.g via a ToolTalk message).
		 */
		if (smsConn) {
			SmsSaveYourself (smsConn, saveType, shutdown,
					interactStyle, fast);
#ifdef DEBUG
			(void) printf ("\tSent SaveYourself to %d\n", smsConn);
#endif /* DEBUG */
		}
		return;
	}

	smXSMP.saveState.inProgress = True;
	smXSMP.saveState.shutdown = shutdown;
	smXSMP.saveState.shutdownCanceled = False;
	smXSMP.saveState.interactStyle = interactStyle;
	smXSMP.saveState.global = global;

	/*
	 * Before notifying the clients, setup a directory
	 * for them to save their state.
	 */
	if (smGD.homeSave || (smGD.sessionType == HOME_SESSION && 
			 smSettings.startState == DtSM_HOME_STATE)) {
		if (smXSMP.saveState.shutdown)
			/*
			 * Leave the old session dir in place.  It
			 * will only be used by XSMP apps.
			 */
			notify = False;
		else
	    		SetupSaveState (True, DtSM_HOME_STATE);

	} else {
		if (smGD.sessionType == CURRENT_SESSION ||
	    		smGD.sessionType == DEFAULT_SESSION)
	    		SetupSaveState (False, DtSM_CURRENT_STATE);
		else if (smGD.sessionType == HOME_SESSION && 
			 smSettings.startState == DtSM_CURRENT_STATE)
	    		SetupSaveState (False, DtSM_HOME_STATE);
        	else
            		SetupSaveState (False, DtSM_HOME_STATE);
	}

	/*
	 * Before the XSMP clients are saved, the ICCC apps must be 
	 * sent a WM_SAVE_YOURSELF message.  This needs to be done because some
	 * apps do not update their geometry information until they
	 * get this message.  If an ICCC app doesn't update their geometry,
	 * then an XSMP-based Window Manager will not have the appropriate
	 * geometry information and the app will not be restore in the
	 * appropriate location.
	*/
	if (notify) {
		ShowWaitState(True);
		NotifyProxyClients ();
		ShowWaitState(False);
	}

	for (pClientRec = connectedList; pClientRec != NULL; 
		pClientRec = pClientRec->next) {

		if (pClientRec->active) {
			SmsSaveYourself (pClientRec->smConn, saveType, 
					shutdown, interactStyle, fast);
#ifdef DEBUG
			(void) printf ("\tSent saveyourself to %d\n",
				pClientRec->smConn);
#endif /* DEBUG */
		}
	}

	/*
	 * If all of the clients are P2 clients, then process
	 * the save now because these clients won't send a 
	 * SaveYourselfDone msg until after they have processed
	 * a P2 message.
	 */
	tmpClient = NULL;

	for (pClientRec = connectedList; pClientRec != NULL; 
		pClientRec = pClientRec->next) {

		if (pClientRec->active) {
			if (pClientRec->saveYourselfP2Requested)
				tmpClient = pClientRec;
			else
				return;
		}
	}

	if (tmpClient) {
		SmsSaveYourselfPhase2 (tmpClient->smConn);
#ifdef DEBUG
		(void) printf ("\tSent SaveYourselfPhase2 to %d\n", 
				pClientRec->smConn);
#endif /* DEBUG */
	}
}


/*ARGSUSED*/
void 
SaveYourselfPhase2ReqProc (
	SmsConn     		smsConn,
	SmPointer   		managerData)
{
	ClientRecPtr   client = (ClientRecPtr) managerData;

#ifdef DEBUG
	(void) printf ("Received SAVE YOURSELF PHASE 2 REQUEST [%d]\n", 
			smsConn);
#endif /* DEBUG */

	client->saveYourselfP2Requested = True;

	/*
	 * A client may have sent this message in response to
	 * the SM's start-up SaveYourself message.  So if
	 * a session isn't currently being saved, return.
	 */
	if (!smXSMP.saveState.inProgress) {
		/*
		 * The client is responding to the start-up SaveYourself
		 * message - this isn't a user-initiated save.
		 */
		
		SmsSaveComplete (smsConn);
		return;
	}

	ProcessSaveYourselfResponses ();
}


/*ARGSUSED*/
void 
SaveYourselfDoneProc (
	SmsConn     		smsConn,
	SmPointer 		managerData,
	Bool			success)
{
	ClientRecPtr		pClientRec = (ClientRec *) managerData;

#ifdef DEBUG
	(void) printf ("Received SAVE YOURSELF DONE [%d] - Success = %s\n",
	       		smsConn, success ? "True" : "False");
#endif /* DEBUG */

	if (!smXSMP.saveState.inProgress) {
		/*
		 * The client is responding to the start-up SaveYourself
		 * message - this isn't a user-initiated save.
		 */
		SmsSaveComplete (smsConn);
		return;
	}

	/*
	 * Cache success if it is a failure - it will be needed later
	 */
	if (success == False)
		smXSMP.saveState.doneSuccess = False;
	
	pClientRec->saveYourselfDone = True;

	ProcessSaveYourselfResponses ();
}


static void
ProcessSaveYourselfResponses (void)
{
	ClientRecPtr		pClientRec;
	Boolean			done = False;

	/*
	 * If all clients are marked as saveYourselfDone, complete 
	 * the save.
	 */
	for (done = True, pClientRec = connectedList; 
		pClientRec != NULL; pClientRec = pClientRec->next) {

		if (pClientRec->active && 
		    pClientRec->saveYourselfDone == False) {
			done = False;
			break;
		}
	}

	if (done) {
		if (smXSMP.saveState.shutdownCanceled)
			CancelShutdown ();
		else
			CompleteXSMPSave ();
		return;
	}
	
	/*
	 * If any client is marked as not having sent a 
	 * SaveYourselfDone message and it is has not been
	 * marked as having requested a SaveYourselfP2,
	 * then return.
	 */
	for (pClientRec = connectedList; pClientRec != NULL; 
		pClientRec = pClientRec->next) {

		if (pClientRec->active && 
		    pClientRec->saveYourselfDone == False &&
		    pClientRec->saveYourselfP2Requested == False)
			return;
	}

	/*
	 * Only clients which requested a SaveYourselfP2 have not
	 * responded with a SaveYourselfDone message.
	 *
	 * Tell the P2 clients to save themselves.
	 */
	for (pClientRec = connectedList; pClientRec != NULL; 
		pClientRec = pClientRec->next) {

		if (pClientRec->active) {
			if (pClientRec->saveYourselfP2Requested &&
			   !pClientRec->saveYourselfDone) {
#ifdef DEBUG
				(void) printf ("\tSent SaveYourselfPhase2 to %d\n", 
						pClientRec->smConn);
#endif /* DEBUG */
				SmsSaveYourselfPhase2 (pClientRec->smConn);
			}
		}
	}
}


/*ARGSUSED*/
void 
CloseConnectionProc (
	SmsConn 		smsConn,
	SmPointer  		managerData,
	int			count,
	char 			**reasonMsgs)
{
	ClientRecPtr		pClientRec = (ClientRec *) managerData;
	ClientRecPtr		tmp = pClientRec;

#ifdef DEBUG
	int 			i;

	(void) printf ("Received CONNECTION CLOSED [%d]\n", smsConn);

	for (i = 0; i < count; i++)
		(void) printf ("\tReason [%2d]: %s\n", i+1, reasonMsgs[i]);
#endif /* DEBUG */

	if (count > 0) {
		PostReasonsDialog (GetArrayPropertyValue (tmp, SmProgram), count, reasonMsgs, True);
		SmFreeReasons (count, reasonMsgs);
	}

	CloseDownClient (pClientRec);
}

void
CompleteXSMPSave (void)
{
	ClientRecPtr		pClientRec;

	/*
	 * Save the XSMP clients' state and the Proxy clients' state 
	 * and save the rest of the settings, resources, etc.
	 */
	CompleteSave ();

	/*
	 * If this isn't a shutdown, tell the clients that the save
	 * is complete.
	 */
	if (!smXSMP.saveState.shutdown) {
		for (pClientRec = connectedList; pClientRec != NULL; 
	     		pClientRec = pClientRec->next) {

			if (pClientRec->active) {
				SmsSaveComplete (pClientRec->smConn);
#ifdef DEBUG
				printf ("SENT SmsSaveComplete to: %d\n",
					pClientRec->smConn);
#endif
			}
		}
	}

	if (!smXSMP.saveState.shutdown) {
		for (pClientRec = connectedList; pClientRec != NULL; 
	     	     pClientRec = pClientRec->next) {

			if (pClientRec->active) {
				pClientRec->saveYourselfDone = False;
				pClientRec->saveYourselfP2Requested = False;
				pClientRec->interactRequested = False;
			}
		}

		InitializeSaveState (False);

	} else {

		int		restartHint;

		for (pClientRec = connectedList; pClientRec != NULL; 
	     		pClientRec = pClientRec->next) {

			if (pClientRec->active) {
				SmsDie (pClientRec->smConn);
#ifdef DEBUG
				printf ("SENT SmsDie to: %d\n", 
					pClientRec->smConn);
#endif
			}

			if (!GetCardPropertyValue (pClientRec, 
						   SmRestartStyleHint, 
						   &restartHint))
				restartHint = pClientRec->restartHint;

			if (restartHint == SmRestartAnyway)
				ExecuteCommandProperty (SmShutdownCommand, 
							pClientRec);
		}
		/*
		 * Cannot exit until all of the clients go away.
		 */
		smXSMP.saveState.saveComplete = True;
		return;
	}
}

static
void FreeProps (
	PropertyRecPtr		pProp)
{
	PropertyRecPtr		tmp;
	PropertyRecPtr		trail;
	int			i;

	for (tmp = pProp; tmp != NULL; ) {

		for (i = 0; i < tmp->prop.num_vals; i++)
			XtFree (tmp->prop.vals[i].value);
		if (tmp->prop.num_vals > 0)
			XtFree ((char *) tmp->prop.vals);
		trail = tmp;
		tmp = tmp->next;
		XtFree ((char *) trail);
	}
}


void CloseDownClient (
	ClientRecPtr		pClientRec )
{
	int			restartHint;
	ClientRecPtr	tmp, trail;
	SmsConn oldConn;

	SmsCleanUp (pClientRec->smConn);
	IceSetShutdownNegotiation (pClientRec->iceConn, False);
	IceCloseConnection (pClientRec->iceConn);

	/* save connection information for later compare */
	oldConn = pClientRec->smConn;

	pClientRec->iceConn = NULL;
	pClientRec->smConn = NULL;
	pClientRec->active = False;

	if (!GetCardPropertyValue (pClientRec, SmRestartStyleHint, 
				   &restartHint))
		restartHint = pClientRec->restartHint;

	if (!smXSMP.saveState.inProgress && restartHint == SmRestartImmediately)
		ExecuteCommandProperty (SmRestartCommand, pClientRec);

	if (restartHint == SmRestartAnyway)
		ExecuteCommandProperty (SmResignCommand, pClientRec);

	if (restartHint == SmRestartNever) {
		/*
		 * Remove the client from the list
		 */
		for (tmp = trail = connectedList; tmp != NULL; 
			trail = tmp, tmp = tmp->next) {
			if (tmp->smConn == oldConn) {
				FreeProps (pClientRec->props);
				if (tmp == connectedList)
					connectedList = tmp->next;
				trail->next = tmp->next;
				XtFree ((char *) tmp);
				break;
			}
		}
	}

	/*
	 * If a shutdown is occurring and all of the clients
	 * are inactive, exit.
	 */
	if (smXSMP.saveState.shutdown) { 
		/* 
		 * Return if any clients are still active 
		 */ 
		for (tmp = connectedList; tmp != NULL; tmp = tmp->next) { 
			if (tmp->active) { 
				return; 
			}
		}

		/*
		 * All clients are inactive - its time to exit 
		 */
		_DtReleaseLock (smGD.display, SM_RUNNING_LOCK);
		SM_EXIT (0);
	} 
}


/*ARGSUSED*/
static 
Tt_callback_action GetWsmClientsHandler(
	Tt_message 		message,
	Tt_pattern 		pattern)
{
	int			lenClientIds;
	int			num_args =  tt_message_args_count (message);
#ifdef DEBUG
	char			* pchar;	
	int			i;
#endif

	if (num_args != 3) {
		smXSMP.saveState.numClientIds = 0;
		smXSMP.saveState.clientIds = NULL;
		smXSMP.saveState.workspaceNums = NULL;
		ProcessInteract (smXSMP.saveState.interactClient, False);
		return (TT_CALLBACK_PROCESSED);
	}

	/*
	 * Before extracting the new values from this message, free any
	 * old values.
	 */
	if (smXSMP.saveState.clientIds) 
		free (smXSMP.saveState.clientIds); 
	if (smXSMP.saveState.workspaceNums) 
		free (smXSMP.saveState.workspaceNums);

	tt_message_arg_ival (message, 0,
			&smXSMP.saveState.numClientIds);
	tt_message_arg_bval (message, 1, 
			(unsigned char **) &smXSMP.saveState.clientIds, 
			&lenClientIds);
	tt_message_arg_bval (message, 2, 
			(unsigned char **) &smXSMP.saveState.workspaceNums, 
			&lenClientIds);
	tt_message_reply (message);
	tt_message_destroy (message);

#ifdef DEBUG
	(void) printf("GetWsmClientsHandler: num clients = %d\n", 
			smXSMP.saveState.numClientIds);
	for (i = 0, pchar = smXSMP.saveState.clientIds; 
	     i < smXSMP.saveState.numClientIds; 
	     i++, pchar += strlen (pchar) + 1) {
		(void) printf("\tclient [%2d]: workspace = %2d, id = %s\n", 
				i+1, smXSMP.saveState.workspaceNums[i], pchar);
	}
#endif

	ProcessInteract (smXSMP.saveState.interactClient, True);

	return (TT_CALLBACK_PROCESSED);
}


static
Boolean SendGetWsmClientsMessage (void)
{
	Tt_message		message;
	Tt_status		status;

	message = tt_message_create ();
	status = tt_ptr_error (message);
	if (status != TT_OK)
		return (False);

	tt_message_class_set (message, TT_REQUEST);
	tt_message_scope_set (message, TT_SESSION);
	tt_message_address_set (message, TT_PROCEDURE);

	tt_message_session_set (message, tt_default_session());
	tt_message_op_set (message, GET_CLIENT_WORKSPACE_MSG);
	tt_message_callback_add (message, GetWsmClientsHandler);

	/*
	 * Three arguments are expected in the reply so apparently 
	 * they must be accounted for now.
	 */
	tt_message_iarg_add (message, TT_OUT, "integer", 0);
	tt_message_barg_add (message, TT_OUT, "stringlist", 0, 0);
	tt_message_barg_add (message, TT_OUT, "intlist", 0, 0);

	status = tt_message_send (message);
	if (status != TT_OK)
		return (False);

	return (True);
}


static 
int GetCurrentWorkspaceNumber (void)
{
	Atom 			currentWorkspace;
	Atom 			*workspaceList;
	Window			root;
	int 			rval;
	int			numWorkspaces;
	int			i;

	root = XDefaultRootWindow (smGD.display);

	rval = DtWsmGetCurrentWorkspace(smGD.display, root, &currentWorkspace);
	if (rval != Success)
		return (-1);

	rval = DtWsmGetWorkspaceList (smGD.display, root, &workspaceList, 
			(int *) &numWorkspaces);
	if (rval != Success)
		return (-1);

	for (i = 0; i < numWorkspaces; i++) {
		if (currentWorkspace == workspaceList[i])
			return (currentWorkspace);
	}

	return (-1);
}


static 
void CancelShutdown (void)
{
	ClientRecPtr		pClientRec;
	char			*pch;

	for (pClientRec = connectedList; pClientRec != NULL; 
		pClientRec = pClientRec->next) {

		if (pClientRec->active) {

			ExecuteCommandProperty (SmDiscardCommand, pClientRec);

			pClientRec->saveYourselfDone = False;
			pClientRec->saveYourselfP2Requested = False;
			pClientRec->interactRequested = False;
		}
	}

	UndoSetSavePath ();

	InitializeSaveState (False);
}


/*
 * This function can be invoked via different paths:
 *
 *  1. From the InteractRequest callback
 *  2. From the InteractDone callback
 *  3. From the GetWsmClients message callback
 */
static
void ProcessInteract (
	ClientRecPtr		client,
	Boolean			getWsmClientOK)
{
	int			currentWorkspace;
	int			i;
	ClientRecPtr		pClientRec;
	ClientRecPtr		tmp = NULL;
	char			*pchar;

	if (!getWsmClientOK) {
		smXSMP.saveState.clientInteracting = True;
		SmsInteract (client->smConn);
		return;
	}

	/*
	 * If a client wants to interact and its workspace matches
	 * the current workspace, then let it interact.
	 */
	currentWorkspace = GetCurrentWorkspaceNumber ();      

	for (pClientRec = connectedList; 
	     pClientRec != NULL; 
	     pClientRec = pClientRec->next) {

		if (pClientRec->interactRequested) {
			tmp = pClientRec;

			for (i = 0, pchar = smXSMP.saveState.clientIds; 
			     i < smXSMP.saveState.numClientIds; 
			     i++, pchar += strlen (pchar) + 1) {

				if ((currentWorkspace == 
					smXSMP.saveState.workspaceNums[i]) &&
			            (!strcmp (pchar, pClientRec->clientId))) {

					smXSMP.saveState.clientInteracting = 
						True;
					SmsInteract (pClientRec->smConn);
					return;
				}
			}
		}
	}

	/*
	 * Tmp didn't meet all of the requirements but it does
	 * want to interact so let it.
	 */
	if (tmp) {
		smXSMP.saveState.clientInteracting = True;
		SmsInteract (tmp->smConn);
	}
}


/*
 * List manipulation functions
 */
void AddClient (
	ClientRecPtr		newClient)
{
	ClientRecPtr		pClient;

	if (!connectedList) {
		connectedList = newClient;
		return;
	}

	/*
	 * Find the end of the list
	 */
	for (pClient = connectedList; pClient->next != NULL; 
		pClient = pClient->next);

	pClient->next = newClient;
}


/*
 * The real way to handle IO errors is to check the return status
 * of IceProcessMessages.  dtsession properly does this.
 *
 * Unfortunately, a design flaw exists in the ICE library in which
 * a default IO error handler is invoked if no IO error handler is
 * installed.  This default handler exits.  We must avoid this.
 *
 * To get around this problem, we install an IO error handler that
 * does a little magic.  Since a previous IO handler might have been
 * installed, when we install our IO error handler, we do a little
 * trick to get both the previous IO error handler and the default
 * IO error handler.  When our IO error handler is called, if the
 * previous handler is not the default handler, we call it.  This
 * way, everyone's IO error handler gets called except the stupid
 * default one which does an exit!
 */

static IceIOErrorHandler prev_handler;

void
MyIoErrorHandler (
	IceConn 		ice_conn)
{
	if (prev_handler)
		(*prev_handler) (ice_conn);
}    

void
InstallIOErrorHandler (void)
{
	IceIOErrorHandler default_handler;

	prev_handler = IceSetIOErrorHandler (NULL);
	default_handler = IceSetIOErrorHandler (MyIoErrorHandler);
	if (prev_handler == default_handler)
		prev_handler = NULL;
}


void XSMPExit (void)
{
	char 		* pchar;

	if (authenticationInitialized)
		FreeAuthenticationData (smXSMP.numTransports, 
					smXSMP.authDataEntries);

	/*
	 * If the local socket file exists, remove it.
	 *
	 * Assume the format of networkIds is:
	 *
	 *    local/<host_name>:/<socket_file_name>,<other_stuff>
	 */
	if (!strncmp (networkIds, "local/", 6)) {
		if (pchar = strchr (networkIds, ':')) {
			pchar++;
			if (pchar && *pchar != '\000') {
				char		* pchar2;

				if (pchar2 = strchr (pchar, ',')) {
					struct		stat buf;
					/*
					 * This modifies networkIds but
					 * that's OK because an exit is
					 * about to happen.
					 */
					*pchar2 = '\000';
					if ((stat (pchar, &buf)) == 0) {
						(void) unlink (pchar);
					}
				}
			}
		}
	}
}
