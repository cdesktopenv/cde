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
/* $TOG: SmDB.c /main/5 1998/07/23 18:11:31 mgreess $ */
/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "SmDB.h"

#define RESOURCE_BUF_SZ 1024
#define CLIENT_ID_SZ 20

typedef struct _ClientDBRec
{
    XrmDatabase xrmDB;          /* Xrm resource database. */
    Boolean openForInput;       /* True if input, False if output. */

    char *xrmDBFileName;        /* Filename, for openForInput == False. */
    char *tmpDBFileName;
    FILE *xrmDBFile;

    /* Below used if openForInput. */
    char **XSMPClients;         /* malloc'd array of XSMP client IDs. */
    int nXSMPClients;           /* number of XSMP clients. */
    int nextXSMPClientIndex;    /* index of next XSMP client to Get;
				 * also used as sequence # for Put. */

    char **proxyClients;        /* malloc'd array of Proxy client IDs. */
    int nProxyClients;          /* number of Proxy clients. */
    int nextProxyClientIndex;   /* index of next Proxy client to Get;
				 * also used as sequence # for Put. */
} ClientDBRec;

static Boolean xrmInitialized = False;
static XrmName XSMPName[2];
static XrmClass XSMPClass[2];
static XrmName proxyName[2];
static XrmClass proxyClass[2];
static XrmQuark anyQuark;
static char *resourceBuf = NULL;
static char *clientIDBuf = NULL;

/* Public strings */
char *versionStr = ".version";
char *dtsessionIDStr = ".dtsessionID";

/* Strings for XSMP clients. */
static char *clientStr = "!\nClient.%s";
static char *programStr = "%s.Program";
static char *cwdStr = "%s.Cwd";
static char *restartCmdStr = "%s.RestartCommand";
static char *cloneCmdStr = "%s.CloneCommand";
static char *discardCmdStr = "%s.DiscardCommand";
static char *environmentStr = "%s.Environment";
static char *restartHintStr = "%s.RestartHint";
static char *sessionIDStr = "%s.SessionID";

/* Strings for Proxy clients. */
static char *proxyStr = "!\nProxyClient.%s";
static char *commandStr = "%s.Command";

/* Common resources (XSMP and Proxy). */
static char *clientHostStr = "%s.ClientHost";
static char *screenNumStr = "%s.ScreenNum";

static void _initXrm(void);
static Bool _countProc(XrmDatabase *, XrmBindingList, XrmQuarkList,
		       XrmRepresentation *, XrmValue *, XPointer);
static Bool _fillClientIDProc(XrmDatabase *, XrmBindingList, XrmQuarkList,
			      XrmRepresentation *, XrmValue *, XPointer);
static Bool _fillStringArrayProc(XrmDatabase *, XrmBindingList, XrmQuarkList,
				 XrmRepresentation *, XrmValue *, XPointer);
static Boolean _getStringResource(XrmDatabase, char *,
				  char *, char **, char *);
static Boolean _getIntResource(XrmDatabase, char *, char *, int *, int);
static Boolean _getStringArrayResource(XrmDatabase, char *, char *,
				       char ***, char **);
static void _freeStringArray(char **);
static Boolean _putStringResource(ClientDBRec *, char *, char *, char *);
static Boolean _putIntResource(ClientDBRec *, char *, char *, int);
static Boolean _putStringArrayResource(ClientDBRec *, char *, char *, char **);
static char *_tmpFileName(char *);

static void
_initXrm(void)
{
    if (!xrmInitialized)
    {
	XrmInitialize(); /* Just in case. */

	anyQuark = XrmStringToQuark("?");
	XSMPName[0] = proxyName[0] = anyQuark;
	XSMPClass[0] = XrmStringToClass("Client");
	proxyClass[0] = XrmStringToClass("ProxyClient");
	XSMPName[1] = XSMPClass[1] =
	    proxyName[1] = proxyClass[1] = NULLQUARK;
	xrmInitialized = True;
    }
}

static Bool
_countProc(XrmDatabase *clientDB, XrmBindingList bindingList,
	   XrmQuarkList quarkList, XrmRepresentation *reps,
	   XrmValue *value, XPointer uData)
{
    int *countP = (int *)uData;

    (*countP)++;

    return FALSE;
}

static Bool
_fillClientIDProc(XrmDatabase *clientDB, XrmBindingList bindingList,
		  XrmQuarkList quarkList, XrmRepresentation *reps,
		  XrmValue *value, XPointer uData)
{
    char ***idListPtr = (char ***)uData;
    char **clientIDPtr = *idListPtr;

    *clientIDPtr = (char *)value->addr;
    (*idListPtr)++;

    return FALSE;
}

static Bool
_fillStringArrayProc(XrmDatabase *clientDB, XrmBindingList bindingList,
		     XrmQuarkList quarkList, XrmRepresentation *reps,
		     XrmValue *value, XPointer uData)
{
    char **stringsPtr = (char **)uData;
    char *indexStr;

    /* Our index into this string array is at quarkList[2]. */
    if ((indexStr = XrmQuarkToString(quarkList[2])) == (char *)NULL)
	return True;

    if ((stringsPtr[atoi(indexStr)] = XtNewString((char *)value->addr))
	== (char *)NULL)
	return True;

    return FALSE;
}

static Boolean
_getStringResource(XrmDatabase xrmDB, char *fmtStr,
		   char *clientID, char **resourcePtr, char *defaultVal)
{
    char *resourceType;
    XrmValue resourceValue;

    if (NULL == resourceBuf) resourceBuf = XtMalloc(RESOURCE_BUF_SZ);
    sprintf(resourceBuf, fmtStr, clientID);
    if (XrmGetResource(xrmDB, resourceBuf, resourceBuf,
		       &resourceType, &resourceValue))
    {
	if ((*resourcePtr = XtNewString(resourceValue.addr)) == (char *)NULL)
	    return False;

	return True;
    }

    *resourcePtr = defaultVal;
    return True;
}

static Boolean
_getIntResource(XrmDatabase xrmDB, char *fmtStr,
		char *clientID, int *resourcePtr, int defaultVal)
{
    char *resourceType;
    XrmValue resourceValue;

    if (NULL == resourceBuf) resourceBuf = XtMalloc(RESOURCE_BUF_SZ);
    sprintf(resourceBuf, fmtStr, clientID);
    *resourcePtr = (XrmGetResource(xrmDB, resourceBuf, resourceBuf,
				   &resourceType, &resourceValue)) ?
			atoi(resourceValue.addr) : defaultVal;

    return True;
}

static Boolean
_getStringArrayResource(XrmDatabase xrmDB, char *fmtStr,
			char *clientID, char ***resourcePtr,
			char **defaultVal)
{
    XrmQuark resourceName[3];
    XrmQuark resourceClass[3];
    char *resourceType;
    XrmValue resourceValue;
    int nStrings;

    resourceName[0] = resourceName[1] = anyQuark;
    resourceName[2] = NULLQUARK;

    if (NULL == resourceBuf) resourceBuf = XtMalloc(RESOURCE_BUF_SZ);
    sprintf(resourceBuf, fmtStr, clientID);
    XrmStringToQuarkList(resourceBuf, resourceClass);

    nStrings = 0;
    XrmEnumerateDatabase(xrmDB, resourceName, resourceClass,
			 XrmEnumOneLevel, _countProc,
			 (XPointer)&nStrings);

    if (nStrings > 0)
    {
	char **stringsPtr;
	int i;

	if ((stringsPtr = (char **)XtMalloc((nStrings + 1) * sizeof(char *)))
	    == (char **)NULL)
	    return False;

	/* Initialize array entries to NULL so free can work. */
	/* NOTE: Final entry (nStrings) will remain NULL. */
	for (i = 0; i <= nStrings; i++)
	    stringsPtr[i] = (char *)NULL;

	if (XrmEnumerateDatabase(xrmDB,
				 resourceName, resourceClass,
				 XrmEnumOneLevel, _fillStringArrayProc,
				 (XPointer)stringsPtr))
	{
	    for (i = 0; i < nStrings; i++)
		XtFree(stringsPtr[i]);
	    XtFree((char *)stringsPtr);
	    return False;
	}

	*resourcePtr = stringsPtr;
	return True;
    }

    *resourcePtr = defaultVal;
    return True;
}

static void
_freeStringArray(char **stringsPtr)
{
    if (stringsPtr)
    {
	int i;

	for (i = 0; stringsPtr[i] != (char *)NULL; i++)
	    XtFree(stringsPtr[i]);

	XtFree((char *)stringsPtr);
    }
}

static Boolean
_putStringResource(ClientDBRec *clientDB, char *fmtStr, char *clientID,
		   char *resourceVal)
{
    if (resourceVal != (char *)NULL)
    {
        if (NULL == clientIDBuf) clientIDBuf = XtMalloc(RESOURCE_BUF_SZ);
	sprintf(clientIDBuf, fmtStr, clientID);
	if (fprintf(clientDB->xrmDBFile, "%s: %s\n", clientIDBuf, resourceVal)
	    < 0)
	    return False;
    }

    return True;
}

static Boolean
_putIntResource(ClientDBRec *clientDB, char *fmtStr, char *clientID,
		int resourceVal)
{
    if (NULL == clientIDBuf) clientIDBuf = XtMalloc(RESOURCE_BUF_SZ);
    sprintf(clientIDBuf, fmtStr, clientID);
    if (fprintf(clientDB->xrmDBFile, "%s: %d\n", clientIDBuf, resourceVal)
	< 0)
	return False;

    return True;
}

static Boolean
_putStringArrayResource(ClientDBRec *clientDB, char *fmtStr, char *clientID,
			char **resourceVal)
{
    int i;

    if (resourceVal != (char **)NULL)
    {
	for (i = 0; resourceVal[i] != (char *)NULL; i++)
	{
            if (NULL == clientIDBuf) clientIDBuf = XtMalloc(RESOURCE_BUF_SZ);
	    sprintf(clientIDBuf, fmtStr, clientID);
	    if (fprintf(clientDB->xrmDBFile, "%s.%d: %s\n",
			clientIDBuf, i, resourceVal[i]) < 0)
		return False;
	}
    }

    return True;
}

static char *
_tmpFileName(char *fileName)
{
    int fileNameLen = strlen(fileName);
    char *newFileName =
	(char *)XtMalloc((fileNameLen + 20) * sizeof(char));

    if (newFileName != (char *)NULL)
    {
	int i;
	char *ptr = &(newFileName[fileNameLen]);

	strcpy(newFileName, fileName);

	/* Hm.  I suppose if a couple billion versions of this file exist */
	/* we could loop forever, but that's not likely. */
	for (i = 0; ; i++)
	{
	    sprintf(ptr, "%d", i);
	    if (access(newFileName, F_OK) != 0)
		break;
	}
    }

    return newFileName;
}

ClientDB
OpenInputClientDB(char *fileName,
		  char **version,
		  char **dtsessionID)
{
    ClientDBRec *inputDB;
    char **tmpPtr;
    char *resourceType;
    XrmValue resourceValue;

    *version = (char *)NULL;
    *dtsessionID = (char *)NULL;

    _initXrm();

    if ((fileName == (char *)NULL) ||
	((inputDB = (ClientDBRec *)XtMalloc(sizeof(ClientDBRec)))
	 == (ClientDBRec *)NULL))
	return (ClientDB)NULL;

    if ((inputDB->xrmDB = XrmGetFileDatabase(fileName))
	== (XrmDatabase)NULL)
    {
	XtFree((char *)inputDB);
	return (ClientDB)NULL;
    }

    inputDB->openForInput = True;
    inputDB->XSMPClients = inputDB->proxyClients = (char **)NULL;
    inputDB->nXSMPClients = inputDB->nProxyClients = 0;
    inputDB->nextXSMPClientIndex = inputDB->nextProxyClientIndex = 0;

    /* Count the number of XSMP and Proxy clients. */
    XrmEnumerateDatabase(inputDB->xrmDB, XSMPName, XSMPClass,
			 XrmEnumOneLevel, _countProc,
			 (XPointer)&inputDB->nXSMPClients);
    XrmEnumerateDatabase(inputDB->xrmDB, proxyName, proxyClass,
			 XrmEnumOneLevel, _countProc,
			 (XPointer)&inputDB->nProxyClients);

    /* Allocate space for the client IDs and fill from database. */
    if (inputDB->nXSMPClients > 0)
    {
	if ((inputDB->XSMPClients =
	     (char **)XtMalloc(inputDB->nXSMPClients * sizeof(char *)))
	    == (char **)NULL)
	{
	    XrmDestroyDatabase(inputDB->xrmDB);
	    XtFree((char *)inputDB);
	    return (ClientDB)NULL;
	}

	tmpPtr = inputDB->XSMPClients;
	XrmEnumerateDatabase(inputDB->xrmDB, XSMPName, XSMPClass,
			     XrmEnumOneLevel, _fillClientIDProc,
			     (XPointer)&tmpPtr);
    }
    if (inputDB->nProxyClients > 0)
    {
	if ((inputDB->proxyClients =
	     (char **)XtMalloc(inputDB->nProxyClients * sizeof(char *)))
	    == (char **)NULL)
	{
	    XrmDestroyDatabase(inputDB->xrmDB);
	    XtFree((char *)inputDB->XSMPClients);
	    XtFree((char *)inputDB);
	    return (ClientDB)NULL;
	}

	tmpPtr = inputDB->proxyClients;
	XrmEnumerateDatabase(inputDB->xrmDB, proxyName, proxyClass,
			     XrmEnumOneLevel, _fillClientIDProc,
			     (XPointer)&tmpPtr);
    }

    if ((!XrmGetResource(inputDB->xrmDB, versionStr, versionStr,
			 &resourceType, &resourceValue)) ||
	((*version = XtNewString(resourceValue.addr)) == (char *)NULL) ||
	(!XrmGetResource(inputDB->xrmDB, dtsessionIDStr, dtsessionIDStr,
			 &resourceType, &resourceValue)) ||
	((*dtsessionID = XtNewString(resourceValue.addr)) == (char *)NULL))
    {
	if (*version)
	{
	    XtFree(*version);
	    *version = (char *)NULL;
	}
	XrmDestroyDatabase(inputDB->xrmDB);
	XtFree((char *)inputDB->XSMPClients);
	XtFree((char *)inputDB->proxyClients);
	XtFree((char *)inputDB);
	return (ClientDB)NULL;
    }

    return (ClientDB)inputDB;
}

ClientDB
OpenOutputClientDB(char *fileName,
		   char *version,
		   char *dtsessionID)
{
    ClientDBRec *outputDB;

    _initXrm();

    if ((fileName == (char *)NULL) ||
	(version == (char *)NULL) ||
	(dtsessionID == (char *)NULL) ||
	((outputDB = (ClientDBRec *)XtMalloc(sizeof(ClientDBRec)))
	 == (ClientDBRec *)NULL))
	return (ClientDB)NULL;

    outputDB->xrmDB = (XrmDatabase)NULL;
    outputDB->openForInput = False;
    outputDB->nextXSMPClientIndex = 0;
    outputDB->nextProxyClientIndex = 0;

    /* Save current DB to tmp file in case we need to restore later. */
    if (((outputDB->xrmDBFileName = XtNewString(fileName)) == (char *)NULL) ||
	((outputDB->tmpDBFileName = _tmpFileName(fileName)) == (char *)NULL))
    {
	if (outputDB->xrmDBFileName != (char *)NULL)
	    XtFree(outputDB->xrmDBFileName);
	XtFree((char *)outputDB);
	return (ClientDB)NULL;
    }

    if ((rename(fileName, outputDB->tmpDBFileName) != 0) &&
	(errno != ENOENT))
    {
	XtFree(outputDB->xrmDBFileName);
	XtFree(outputDB->tmpDBFileName);
	XtFree((char *)outputDB);
	return (ClientDB)NULL;
    }

    /* Open fileName for writing. */
    if ((outputDB->xrmDBFile = fopen(fileName, "w")) == (FILE *)NULL)
    {
        int rv;
	rv = rename(outputDB->tmpDBFileName, fileName);
	XtFree(outputDB->xrmDBFileName);
	XtFree(outputDB->tmpDBFileName);
	XtFree((char *)outputDB);
	return (ClientDB)NULL;
    }

    /* Store version and session ID. */
    fprintf(outputDB->xrmDBFile, "! dtsession.db\n!\n%s: %s\n",
	    versionStr, version);
    fprintf(outputDB->xrmDBFile, "%s: %s\n", dtsessionIDStr, dtsessionID);

    return (ClientDB)outputDB;
}

XSMPClientDBRecPtr
GetXSMPClientDBRec(ClientDB inputDBPtr)
{
    ClientDBRec *inputDB = (ClientDBRec *)inputDBPtr;
    XSMPClientDBRecPtr clientPtr;
    char *clientID;

    if ((inputDB == (ClientDBRec *)NULL) ||
	(!inputDB->openForInput) ||
	(inputDB->nextXSMPClientIndex >= inputDB->nXSMPClients) ||
	((clientPtr = (XSMPClientDBRecPtr)XtMalloc(sizeof(XSMPClientDBRec)))
	 == (XSMPClientDBRecPtr)NULL))
    {
	return (XSMPClientDBRecPtr)NULL;
    }

    clientID = inputDB->XSMPClients[inputDB->nextXSMPClientIndex];

    /* Initialize pointers so Free is easy. */
    clientPtr->clientId = (char *)NULL;
    clientPtr->clientHost = clientPtr->program =
	clientPtr->cwd = (char *)NULL;
    clientPtr->restartCommand = clientPtr->cloneCommand =
	clientPtr->discardCommand = clientPtr->environment = (char **)NULL;

    /* Retrieve resources from the database. */
    if (!_getStringResource(inputDB->xrmDB, sessionIDStr, clientID,
			    &clientPtr->clientId, (char *)NULL) ||
	!_getStringResource(inputDB->xrmDB, clientHostStr, clientID,
			     &clientPtr->clientHost, (char *)NULL) ||
	!_getStringResource(inputDB->xrmDB, programStr, clientID,
			    &clientPtr->program, (char *)NULL) ||
	!_getStringResource(inputDB->xrmDB, cwdStr, clientID,
			    &clientPtr->cwd, (char *)NULL) ||
	!_getIntResource(inputDB->xrmDB, screenNumStr, clientID,
			 &clientPtr->screenNum, 0) ||
	!_getStringArrayResource(inputDB->xrmDB, restartCmdStr, clientID,
				 &clientPtr->restartCommand, (char **)NULL) ||
	!_getStringArrayResource(inputDB->xrmDB, cloneCmdStr, clientID,
				 &clientPtr->cloneCommand, (char **)NULL) ||
	!_getStringArrayResource(inputDB->xrmDB, discardCmdStr, clientID,
				 &clientPtr->discardCommand, (char **)NULL) ||
	!_getStringArrayResource(inputDB->xrmDB, environmentStr, clientID,
				 &clientPtr->environment, (char **)NULL) ||
	!_getIntResource(inputDB->xrmDB, restartHintStr, clientID,
			 (int *)&clientPtr->restartHint, 0))
    {
	FreeXSMPClientDBRec(clientPtr);
	return (XSMPClientDBRecPtr)NULL;
    }

    clientPtr->next = (XSMPClientDBRec *)NULL;

    inputDB->nextXSMPClientIndex++;

    return clientPtr;
}

ProxyClientDBRecPtr
GetProxyClientDBRec(ClientDB inputDBPtr)
{
    ClientDBRec *inputDB = (ClientDBRec *)inputDBPtr;
    ProxyClientDBRecPtr clientPtr;
    char *clientID;

    if ((inputDB == (ClientDBRec *)NULL) ||
	(!inputDB->openForInput) ||
	(inputDB->nextProxyClientIndex >= inputDB->nProxyClients) ||
	((clientPtr = (ProxyClientDBRecPtr)XtMalloc(sizeof(ProxyClientDBRec)))
	 == (ProxyClientDBRecPtr)NULL))
    {
	return (ProxyClientDBRecPtr)NULL;
    }

    clientID = inputDB->proxyClients[inputDB->nextProxyClientIndex];

    /* Initialize pointers so Free is easy. */
    clientPtr->clientHost = (char *)NULL;
    clientPtr->command = (char **)NULL;

    /* Retrieve resources from the database. */
    if (!_getStringResource(inputDB->xrmDB, clientHostStr, clientID,
			     &clientPtr->clientHost, (char *)NULL) ||
	!_getStringArrayResource(inputDB->xrmDB, commandStr, clientID,
				 &clientPtr->command, (char **)NULL) ||
	!_getIntResource(inputDB->xrmDB, screenNumStr, clientID,
			 &clientPtr->screenNum, 0))
    {
	FreeProxyClientDBRec(clientPtr);
	return (ProxyClientDBRecPtr)NULL;
    }

    inputDB->nextProxyClientIndex++;

    return clientPtr;
}

Boolean
PutXSMPClientDBRec(ClientDB outputDBPtr,
		   XSMPClientDBRecPtr clientPtr)
{
    ClientDBRec *outputDB = (ClientDBRec *)outputDBPtr;
    char clientID[CLIENT_ID_SZ];

    if ((outputDB == (ClientDBRec *)NULL) ||
	(outputDB->openForInput) ||
	(clientPtr == (XSMPClientDBRecPtr)NULL) ||
	(clientPtr->clientId == (char *)NULL))
	return False;

    sprintf(clientID, "C%d", outputDB->nextXSMPClientIndex++);

    if (!_putStringResource(outputDB, clientStr, clientID, clientID) ||
	!_putStringResource(outputDB, sessionIDStr, clientID,
			    clientPtr->clientId) ||
	!_putStringResource(outputDB, clientHostStr, clientID,
			    clientPtr->clientHost) ||
	!_putStringResource(outputDB, programStr, clientID,
			    clientPtr->program) ||
	!_putStringResource(outputDB, cwdStr, clientID,
			    clientPtr->cwd) ||
	!_putIntResource(outputDB, screenNumStr, clientID,
			 clientPtr->screenNum) ||
	!_putStringArrayResource(outputDB, restartCmdStr, clientID,
				 clientPtr->restartCommand) ||
	!_putStringArrayResource(outputDB, cloneCmdStr, clientID,
				 clientPtr->cloneCommand) ||
	!_putStringArrayResource(outputDB, discardCmdStr, clientID,
				 clientPtr->discardCommand) ||
	!_putStringArrayResource(outputDB, environmentStr, clientID,
				 clientPtr->environment) ||
	!_putIntResource(outputDB, restartHintStr, clientID,
			 (int)clientPtr->restartHint))
	return False;

    return True;
}

Boolean
PutProxyClientDBRec(ClientDB outputDBPtr,
		    ProxyClientDBRecPtr clientPtr)
{
    ClientDBRec *outputDB = (ClientDBRec *)outputDBPtr;
    char clientID[CLIENT_ID_SZ];

    if ((outputDB == (ClientDBRec *)NULL) ||
	(outputDB->openForInput) ||
	(clientPtr == (ProxyClientDBRecPtr)NULL))
	return False;

    sprintf(clientID, "PC%d", outputDB->nextProxyClientIndex++);

    if (!_putStringResource(outputDB, proxyStr, clientID, clientID) ||
	!_putStringResource(outputDB, clientHostStr, clientID,
			    clientPtr->clientHost) ||
	!_putStringArrayResource(outputDB, commandStr, clientID,
				 clientPtr->command) ||
	!_putIntResource(outputDB, screenNumStr, clientID,
			 clientPtr->screenNum))
	return False;

    return True;
}

Boolean
CloseClientDB(ClientDB clientDBPtr, Boolean writeDB)
{
    ClientDBRec *clientDB = (ClientDBRec *)clientDBPtr;

    if (clientDB == (ClientDBRec *)NULL)
	return False;

    /* Input? */
    if (clientDB->openForInput)
    {
	XtFree((char *)clientDB->XSMPClients);
	XtFree((char *)clientDB->proxyClients);
	XrmDestroyDatabase(clientDB->xrmDB);
	XtFree((char *)clientDB);

	return True;
    }

    /* Otherwise, output. */
    if (writeDB)
    {
	/* Close file and remove temp DB. */
	fclose(clientDB->xrmDBFile);
	unlink(clientDB->tmpDBFileName);
    }
    else
    {
      int rv;
	/* Close file and remove it; restore original DB. */
	fclose(clientDB->xrmDBFile);
	rv = rename(clientDB->tmpDBFileName, clientDB->xrmDBFileName);
    }

    XtFree(clientDB->xrmDBFileName);
    XtFree(clientDB->tmpDBFileName);
    XtFree((char *)clientDB);

    return True;
}

void
FreeXSMPClientDBRec(XSMPClientDBRecPtr clientPtr)
{
    if (clientPtr != (XSMPClientDBRecPtr)NULL)
    {
	XtFree(clientPtr->clientId);
	XtFree(clientPtr->clientHost);
	XtFree(clientPtr->program);
	XtFree(clientPtr->cwd);

	_freeStringArray(clientPtr->restartCommand);
	_freeStringArray(clientPtr->cloneCommand);
	_freeStringArray(clientPtr->discardCommand);
	_freeStringArray(clientPtr->environment);

	XtFree((char *)clientPtr);
    }
}

void
FreeProxyClientDBRec(ProxyClientDBRecPtr clientPtr)
{
    if (clientPtr != (ProxyClientDBRecPtr)NULL)
    {
	XtFree(clientPtr->clientHost);

	_freeStringArray(clientPtr->command);

	XtFree((char *)clientPtr);
    }
}
