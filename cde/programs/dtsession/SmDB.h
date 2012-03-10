#ifndef SM_DB_H
#define SM_DB_H

/* $XConsortium: SmDB.h /main/3 1996/02/02 16:03:03 rswiston $ */
/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

#include "SmXSMP.h"
#include <X11/Intrinsic.h>

typedef void *ClientDB;

/* Public variables */
extern char *versionStr;
extern char *dtsessionIDStr;

/********* Open the client database *********/
/*
 * Open client database for reading.
 */
extern ClientDB OpenInputClientDB(char *fileName,
				  char **version,
				  char **dtsessionID);
/*
 * Open client database for writing.
 */
extern ClientDB OpenOutputClientDB(char *fileName,
				   char *version,
				   char *dtsessionID);


/********* Retrieve client record from database *********/
/*
 * Retrieve the next XSMP client record from the database.
 */
extern XSMPClientDBRecPtr GetXSMPClientDBRec(ClientDB inputDB);
/*
 * Retrieve the next Proxy client record from the database.
 */
extern ProxyClientDBRecPtr GetProxyClientDBRec(ClientDB inputDB);


/********* Save client record to database *********/
/*
 * Save XSMP client record to database.
 */
extern Boolean PutXSMPClientDBRec(ClientDB outputDB,
				  XSMPClientDBRecPtr clientPtr);
/*
 * Save Proxy client record to database.
 */
extern Boolean PutProxyClientDBRec(ClientDB outputDB,
				   ProxyClientDBRecPtr clientPtr);


/********* Close client database *********/
extern Boolean CloseClientDB(ClientDB clientDB, Boolean writeDB);


/********* Free memory allocated for client records. *********/
extern void FreeXSMPClientDBRec(XSMPClientDBRecPtr);
extern void FreeProxyClientDBRec(ProxyClientDBRecPtr);

#endif /* SM_DB_H */
