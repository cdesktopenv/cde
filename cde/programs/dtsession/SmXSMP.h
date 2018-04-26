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
 * $XConsortium: SmXSMP.h /main/16 1996/02/13 11:02:37 barstow $
 */

/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:        SmXSMP.h
 **
 **  Project:     DT Session Manager (dtsession)
 **
 *****************************************************************************
 *************************************<+>*************************************/
#ifndef _smxsmp_h
#define _smxsmp_h

#include <X11/Intrinsic.h>
#include <X11/SM/SMlib.h>
#include <X11/ICE/ICEutil.h>

/*
 * Constants for the Save_Session ToolTalk message
 */
#define DEFAULT_SAVE_TYPE               SmSaveGlobal
#define DEFAULT_SHUTDOWN                False
#define DEFAULT_INTERACT_STYLE          SmInteractStyleAny
#define DEFAULT_FAST                    True
#define DEFAULT_GLOBAL                  True


typedef struct _PropertyRec {
	SmProp			prop;
	struct _PropertyRec	*next;
} PropertyRec, *PropertyRecPtr;

typedef struct _ClientRec {
	SmsConn			smConn;
	IceConn			iceConn;
	char			* clientId;
	char			* clientHost;
	int			restartHint;
	int			screenNum;
	PropertyRecPtr		props;
	Boolean			active;
	Boolean			saveYourselfDone;
	Boolean			saveYourselfP2Requested;
	Boolean			interactRequested;
	struct _ClientRec	* next;
} ClientRec, *ClientRecPtr;

typedef struct {
	Boolean			global;
	Boolean			shutdown;
	Boolean			clientInteracting;
	Boolean			inProgress;
	Boolean			doneSuccess;
	Boolean			saveComplete;
	Boolean			shutdownCanceled;
	int			interactStyle;
	int			interactCount;
	int			numClientIds;
	char			* clientIds;
	int			* workspaceNums;
	ClientRecPtr		interactClient;
} SaveStateRec;

typedef struct _XSMPClientDBRec {
	char			* clientId;
	char			* clientHost;
	char			* program;
	char			* cwd;
	int			screenNum;
	char			** restartCommand;
	char			** cloneCommand;
	char			** discardCommand;
	char			** environment;
	int			restartHint;
	struct _XSMPClientDBRec	* next;
} XSMPClientDBRec, *XSMPClientDBRecPtr;

typedef struct _ProxyClientDBRec {
	char			* clientHost;
	char			** command;
	int			screenNum;
} ProxyClientDBRec, *ProxyClientDBRecPtr;

typedef struct {
	IceListenObj 		* listenObjs;
	int			numTransports;
	IceAuthDataEntry 	* authDataEntries;
	SaveStateRec		saveState;
	XSMPClientDBRecPtr	xsmpDbList;
	char			* dbVersion;
	char			* dbSessionId;
} XSMPSettings, *XSMPSettingPtr;

/*
 * Failure modes during the initializatin of XSMP
 */
typedef enum {
	XSMP_FAILURE_SMS_INITIALIZE,
	XSMP_FAILURE_ICE_LISTEN,
	XSMP_FAILURE_AUTHENTICATION,
	XSMP_FAILURE_ICE_ADD_WATCH,
	XSMP_FAILURE_ICE_COMPOSE_IDS,
	XSMP_FAILURE_MALLOC
} XSMPFailure;

/*
 * Public variable declarations
 */
extern XSMPSettings		smXSMP;
extern ClientRecPtr		connectedList;

extern char			SM_SESSION_MANAGER[];
extern char 			SM_VENDOR_NAME[];
extern char 			SM_RELEASE_NAME[];
extern char 			SM_CLIENT_ID[];
extern Atom 			XaSmClientId;

/*
 * Public function declarations
 */
extern Boolean InitXSMP ( );

void SaveYourselfReqProc (
	SmsConn     		smsConn,
	SmPointer   		managerData,
	int	    		saveType,
	Bool	    		shutdown,
	int         		interactStyle,
	Bool        		fast,
	Bool        		global);

void CloseDownClient (
	ClientRecPtr		client );

void XSMPExit ();

#endif /*_smxsmp_h*/
