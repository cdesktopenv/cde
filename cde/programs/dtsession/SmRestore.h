/* $XConsortium: SmRestore.h /main/10 1996/02/08 11:29:05 barstow $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        SmRestore.h
 **
 **   Project:     HP DT
 **
 **   Description
 **   -----------
 **   Variables and declarations needed for
 **   Session Restoration for the session manager
 **
 **
 **  (c) Copyright Hewlett-Packard Company, 1990.  
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
#ifndef _smrestore_h
#define _smrestore_h
 
/* 
 *  #include statements 
 */
#include "SmXSMP.h"

/* 
 *  #define statements 
 */
#define	MAX_REMOTE_CLIENTS	200

/* 
 * typedef statements 
 */
/*
 *  External variables  
 */


/*  
 *  External Interface  
 */

extern void ReloadResources( void ) ;
extern int RestoreState( void ) ;
extern int SetCompatState( void ) ;
extern int StartWM( void ) ;
extern int RestoreResources(Boolean, ...) ;
extern void RestoreIndependentResources(void) ;
extern void StartEtc( Boolean ) ;
extern void CreateExecString(char *) ;
extern void KillParent( void ) ;
extern void ScanWhitespace( unsigned char **) ;
extern void SystemCmd (char *pchCmd);

extern Boolean StartClient (
	char 			* program, 
	char 			** argv, 
	char 			* hostname,
	char 			* cwd,
	char 			** envp, 
	Boolean			checkCwd,
	Boolean 		useIgnoreEnvResource,
	int			screen);

extern Boolean StartXSMPSession (
	char			* databaseName);

extern Boolean StartXSMPClient (
	XSMPClientDBRecPtr 	pDbRec,
	char			* databaseName);

extern Boolean StartProxyClient (
	ProxyClientDBRecPtr 	pDbRec);

void ExecuteDiscardCommands (
	char			* db);

extern Boolean ExecuteCommandProperty (
	char			* propertyName,
	ClientRecPtr            pClientRec);

PropertyRecPtr GetPropertyRec (
	ClientRecPtr		pClientRec,
	char			* propName);

char * GetStringPropertyValue (
	ClientRecPtr		pClientRec,
	char			* propName);


#endif /*_smrestore_h*/
