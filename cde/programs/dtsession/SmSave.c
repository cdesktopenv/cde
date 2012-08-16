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
/* $TOG: SmSave.c /main/27 1998/07/23 18:07:58 mgreess $ */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:        SmSave.c
 **
 **  Project:     HP DT Session Manager (dtsession)
 **
 **  Description:
 **  -----------
 **  This file contains all routines that save off the session at any point
 **  in time.  The session includes settings, resources, and clients.  How
 **  the session manager behaves during a save depends on how the user has
 **  configured it to behave.
 **
 **
 *****************************************************************************
 *************************************<+>*************************************/
/*
 * (c) Copyright 1997 The Open Group
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1990, 1993, 1994, 1996 Hewlett-Packard Company.
 * (c) Copyright 1993, 1994, 1996 International Business Machines Corp.
 * (c) Copyright 1993, 1994, 1996 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994, 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */


#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#ifdef _SUN_OS
#include <arpa/inet.h>
#include <string.h>
#endif /* _SUN_OS */

#include <X11/Intrinsic.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/StringDefs.h>
#include <X11/keysymdef.h>
#include <Dt/DtP.h>
#include <Dt/Connect.h>
#include <Dt/UserMsg.h>
#include <Dt/SessionM.h>
#include <Dt/MsgLog.h>
#include "Sm.h"
#include "SmSave.h"
#include "SmResource.h"
#include "SmError.h"
#include "SmGlobals.h"
#include "SmUI.h"
#include "SmWindow.h"
#include "SmProtocol.h"
#include "SmRestore.h"
#include "SmXSMP.h"
#include "SmDB.h"
#include "SmProperty.h"
#include "SmProp.h"
#include "Srv.h"
#include "SrvFile_io.h"

#ifdef   MULTIBYTE
#include <stdlib.h>
#include <limits.h>
#endif

#ifndef __osf__
#include <X11/Xlibint.h>
#endif /* __osf__ */

typedef struct _ProxyClientCache {
	Window		window;
	int		screen;
} ProxyClientCache, *ProxyClientCachePtr;

static ProxyClientCachePtr proxyList;
static int numProxyClients = 0;
static int numProxyAllocated = 0;
static int proxyCacheAllocIncrement = 10;

/*
 * Local Function Declarations
 */
static void CacheProxyClient (
	Boolean			firstClient, 
	int			screenNum, 
	Window			window);

static void PruneSessionDirectory ();

static Boolean WriteClientDatabase ();

static void OutputXSMPClients (
	ClientDB		outputDB);

static void OutputProxyClients (
	ClientDB		outputDB);

static Boolean CheckRequiredProperties (
	ClientRecPtr		pClientRec);

static void PrintStartupInfo( 
			ClientDB outputDB,
                        int screen,
                        Window window);
static int QueryServerSettings( void ) ;
static int SaveCustomizeSettings( void ) ;
static int OutputResource( void ) ;
static void PrintPointerControl( 
                        XrmDatabase *smBase) ;
static void PrintSessionInfo( 
                        XrmDatabase *smBase) ;
static void PrintScreenSaver( 
                        XrmDatabase *smBase) ;
static int PrintFontPaths( 
                        XrmDatabase *smBase,
                        char **fontPath,
                        int numPaths) ;
static int PrintKeyboardControl( 
                        XrmDatabase *smBase);
static int PrintPointerMapping( 
                        XrmDatabase *smBase,
                        char *buttonRet,
                        int numButton) ;
static void PrintWorkHintString( 
                        FILE *hintFile,
                        char *hintString) ;
static void PrintCmdHintString( 
                        FILE *hintFile,
                        char *hintString) ;
static void PrintCmdString( 
                        FILE *cmdFile,
                        char *cmdString) ;
static void PrintRemoteCmdString( 
                        FILE *cmdFile,
                        char *cmdString) ;


/*
 * Global variables that are exposed
 */
SmScreenInfo		screenSaverVals;
SmAudioInfo		audioVals;
SmKeyboardInfo		keyboardVals;

/*
 * Variables global to this module only
 */

/*
 * used as a buffer to write resources into
 * before they are written out
 */
static int	resSize = 10000;
static char	*resSpec;

/*
 * Vars used to maintain state that is sent to the SaveState
 * function.
 */
static Boolean	saveHome;
int		saveMode;




/*************************************<->*************************************
 *
 *  SetupSaveState
 *
 *  Description:
 *  -----------
 *  Do all of the preliminary work that must be done before the state
 *  of a checkpoint can be saved.
 *
 *  Inputs:
 *  ------
 *  saveToHome:  This session is to be saved to the home session or current
 *  mode = whether the session is to be reset or restarted
 *
 *************************************<->***********************************/
void
SetupSaveState(
        Boolean 	saveToHome,
        int 		mode)
{
  unsigned char		*propData;

  /*
   * Cache saveToHome and mode - they will be needed after all of
   * the XSMP clients are saved and the Proxy clients are saved.
   */
  saveHome = saveToHome;
  saveMode = mode;

  /*
   * First set up the save paths so we know if we have to do anything here
   */
  if ((SetSavePath(saveToHome, mode)) == -1)
      return;

  /*
   * Take care of the save mode property on the root window so that the
   * clients will know how to save this file (if they save files)
   */
  if(saveToHome == False)
  {
      propData = (unsigned char *) SM_CURRENT_DIRECTORY;
  }
  else
  {
      propData = (unsigned char *) SM_HOME_DIRECTORY;
  }

  /*
   * Change the save mode to give the subdirectory saved to
   *
   */
  XChangeProperty(smGD.display, RootWindow(smGD.display, 0),
		  XaSmSaveMode, XA_STRING, 8, PropModeReplace,
		   propData, strlen((char *)propData));
  XFlush(smGD.display);
}

/*************************************<->*************************************
 *
 *  SaveState
 *
 *  Description:
 *  -----------
 *  Tells the SM to notify the XSMP clients to save their state.
 *  The function CompleteSave fill be invoked after the XSMP clients
 *  have completed their save.
 *
 *  Inputs:
 *  ------
 *  saveToHome:  This session is to be saved to the home session or current
 *  mode = whether the session is to be reset or restarted
 *
 *  Comments:
 *  --------
 *  As a result of this routine files will be saved in the users .dt directory
 *  that will allow the session manager to restore the session at a later
 *  time.
 *
 *  NOTE - this function is called as the result of the following
 *	events:
 *
 *	1. User initiates a Save Home Session via the Style Manager
 *	2. The Session Manager receives a SM_XSESSION_EXIT message
 *	3. The Session Manager receives a SM_SAVE_SESSION message
 * 
 *************************************<->***********************************/
void
SaveState(
        Boolean 		saveToHome,
        int 			mode,
	int                     saveType,
	Bool                    shutdown,
	int                     interactStyle,
	Bool                    fast,
	Bool                    global)
{
  /*
   * Notify XSMP clients of the save
   */
  SaveYourselfReqProc (NULL, NULL, saveType, shutdown, interactStyle, 
			fast, global);
}


/*************************************<->*************************************
 *
 *  CompleteSave 
 *
 *  Description:
 *  -----------
 *  Called after the XSMP clients have saved their state.  In charge of 
 *  calling all routines that save session information for XSMP client
 *  and Proxy clients as well as settings, resources, etc.
 *
 *  Outputs:
 *  -------
 *
 *************************************<->***********************************/
int 
CompleteSave ( )
{

  FILE 			*convertFP;
  unsigned char		*propData;
  char			*tmpPath, tmpVersion[20];

  Boolean 		saveToHome = saveHome;
  int			mode = saveMode;

  /*
   * If this is a Home session and the user is logging out, return
   * now - the XSMP clients have all responded to the save and the
   * ICCC clients don't save their state when a Home session is
   * exited.
   */
  if (smGD.sessionType == HOME_SESSION && 
      smXSMP.saveState.shutdown == True && 
      smSettings.startState == DtSM_HOME_STATE)
      return (0);

  /*
   * Start a wait state - don't want anything to happen while
   * ICCC clients are being saved
   */
  ShowWaitState(True);
  
  resSpec = (char *) SM_MALLOC(resSize * sizeof(char));
  if (resSpec==NULL)
  {
      PrintErrnoError(DtError, smNLS.cantMallocErrorString);
      return(-1);
  }

  if (!WriteClientDatabase ()) {
      ShowWaitState(False);
      SM_FREE(resSpec);
      return (-1);
  }

  /*
   * WARNING - THIS VARIABLE MUST BE SET BEFORE THE SETTING INFORMATION IS
   * CALLED - SETTINGS MUST BE SAVED BEFORE RESOURCES BECAUSE SETTINGS
   * GETS INFORMATION SAVED IN RESOURCES (smToSet.dClickBuf)
   */
  smToSet.dClickBuf[0] = 0;

  /*
   * Do the actual output of the X settings information.  Output will
   * go to the resource database at this point and be written to the
   * resource file with the rest of the resources (below).
   */
  if(smRes.querySettings == True)
  {
      QueryServerSettings();
  }
  else
  {
      SaveCustomizeSettings();
  }

  /*
   * Do the output of resource information.
   */
  if(OutputResource())
  {
      ShowWaitState(False);
      SM_FREE(resSpec);
      return(-1);
  }

  XDeleteProperty(smGD.display, DefaultRootWindow(smGD.display), XaSmSaveMode);
 
  /* 
   * Go save the default palette at this time
   */
  if(saveToHome == True)
     SaveDefaultPalette(smGD.display, smGD.savePath, DtSM_HOME_STATE);
  else
     SaveDefaultPalette(smGD.display, smGD.savePath, mode);
 
  PruneSessionDirectory ();

  ShowWaitState(False);

  return(0);
}


/*************************************<->*************************************
 *
 *  PruneSessionDirectory -
 *
 *  Description: Removes the oldest session direcotry if the number
 *	of directories exceeds the smRes.numSessionsBackedup.
 *
 *  Inputs: void
 * 
 *************************************<->***********************************/
static void
PruneSessionDirectory ()
{
	DIR			* dirp;
	struct dirent 		* dp;
	int			numDirs = 0;
	struct			stat buf;
	time_t			oldestTime;
	char			*clientDB;
	char			*dirPrefix;
	char			*oldDir;	/* the .old dir */
	char			*oldestDir;
	char			*saveDir;
	char			*tmpDir;

	if ((dirp = opendir (smGD.savePath)) == NULL)
		return;

	clientDB = (char*) XtMalloc((2*MAXPATHLEN) + 1);
	oldestDir = (char*) XtMalloc(MAXPATHLEN + 1);
	saveDir = (char*) XtMalloc(MAXPATHLEN + 1);
	tmpDir = (char*) XtMalloc(MAXPATHLEN + 1);
	
	dirPrefix = XtMalloc (strlen (smGD.restoreSession) + 2);
	sprintf (dirPrefix, "%s.", smGD.restoreSession);
	if (strlen (dirPrefix) > 5) {
		dirPrefix[4] = '.';
		dirPrefix[5] = '\000';
	}

	oldDir = XtMalloc (strlen (smGD.restoreSession) + 
			   strlen (SM_OLD_EXTENSION) + 2);
	sprintf (oldDir, "%s.%s", smGD.restoreSession, SM_OLD_EXTENSION);

	(void) time (&oldestTime);

	while ((dp = readdir (dirp)) != NULL) {
		/*
		 * Do some sanity checks to limit the number of * stat() 
		 * calls.  Note that for backward compatibility with CDE 
		 * 1.0 if a ".old" directory exists, keep it.
		 */
		if ((!strcmp (dp->d_name, ".")) || 
		    (!strcmp (dp->d_name, "..")) ||
		    (!strcmp (dp->d_name, oldDir)) ||
		    (strncmp  (dp->d_name, dirPrefix, strlen (dirPrefix))))
			continue;

		sprintf (tmpDir, "%s/%s", smGD.savePath, dp->d_name);

                if (((stat (tmpDir, &buf)) == 0) && S_ISDIR (buf.st_mode)) {
			if (buf.st_mtime < oldestTime) {
				/*
				 * Keep track of the oldest dir in case
				 * it needs to be removed
				 */
				oldestTime = buf.st_mtime;
				strcpy (oldestDir, tmpDir);
			}
			numDirs++;
		}
        }
	closedir(dirp);

	if (numDirs > smRes.numSessionsBackedup) {
		/*
		 * Need to remove the oldest directory.  The general
		 * process (using "home" as an example) is:
		 *
		 *   mv home home.tmp
		 *   mv oldestDir home
		 *   Execute the DiscardCommand props in the home directory
		 *   rm -rf home
		 *   mv home.tmp home
		 *
		 * The reason this is done is because when the 
		 * DiscardCommand were saved, the directory at the time of
		 * the save contained "home".  For example, a DiscardCommand
		 * may look like:
		 *
		 *    rm -f /u/somebody/.dt/sessions/home/app.state
		 *
		 * yet because of the session archiving, the app.state
		 * file may actually be in a directory like:
		 *
		 *    /u/somebody/.dt/sessions/home.<tempnam>
		 */ 

		char		* tmpName;
		char		* tmp;

		tmpName = (char *) XtMalloc (strlen (smGD.restoreSession) + 2);
		sprintf (tmpName, "%s.", smGD.restoreSession);
		if (strlen (tmpName) > 5) {
			tmpName[4] = '.';
			tmpName[5] = '\000';
		}
		tmp = (char *) tempnam (smGD.savePath, tmpName);

		sprintf (saveDir, "%s/%s", smGD.savePath, smGD.restoreSession);

		MoveDirectory (saveDir, tmp, False);
		MoveDirectory (oldestDir, saveDir, False);

		sprintf (clientDB, "%s/%s/%s", smGD.savePath, 
				smGD.restoreSession, SM_CLIENT_FILE2);

		ExecuteDiscardCommands (clientDB);

		MoveDirectory (tmp, saveDir, True);

		free (tmp);
		XtFree (tmpName);
	}

	/*
	 * If exiting and the current session is a Current session
	 * but smSettings.startState has been set to to a 
	 * Home session (this would be true if a user started a 
	 * Current session but used the Style Manager to set the session
	 * upon next login to be a Home session), then if a home
	 * directory exists, mv current to current.old
	 */

	if (smXSMP.saveState.shutdown && smGD.sessionType == CURRENT_SESSION &&
	    smSettings.startState == DtSM_HOME_STATE) {

		sprintf (tmpDir, "%s/%s", smGD.savePath, SM_HOME_DIRECTORY);

                if (((stat (tmpDir, &buf)) == 0) && S_ISDIR (buf.st_mode)) {

			sprintf (saveDir, "%s/%s", smGD.savePath, oldDir);

                	if (((stat (saveDir, &buf)) == 0) && 
					S_ISDIR (buf.st_mode)) {

				sprintf (clientDB, "rm -rf %s", saveDir);
				SystemCmd (clientDB);

				sprintf (oldestDir, "%s/%s", smGD.savePath, 
					SM_CURRENT_DIRECTORY);

				MoveDirectory (oldestDir, saveDir, False);
			}
		}
	}

	XtFree(clientDB);
	XtFree(dirPrefix);
	XtFree(oldDir);
	XtFree(oldestDir);
	XtFree(saveDir);
	XtFree(tmpDir);
}


/*************************************<->*************************************
 *
 *  WriteClientDatabase -
 *
 *  Description: Writes the client database (for XSMP and Proxy clients)
 *
 *  Inputs: None
 * 
 *  Returns True if the database is wrtten OK; False otherwise
 *
 *************************************<->***********************************/
static Boolean
WriteClientDatabase ()
{
	char			*db;
	ClientDB		outputDB;

	db = (char*) XtMalloc(MAXPATHLEN + 1);
	(void) sprintf (db, "%s/%s/%s", smGD.savePath, smGD.restoreSession,
			SM_CLIENT_FILE2);

	if ((outputDB = OpenOutputClientDB (db, smXSMP.dbVersion,
					    smXSMP.dbSessionId)) == NULL) {
		char		* str;

		str = strdup ((char *) GETMESSAGE (40, 18,
			"The following database file cannot be opened for writing:\n\n   %s\n\nThe session will not be saved."));

		if (str) {
			DtMsgLogMessage (smGD.programName, DtMsgLogError,
					 str, db);
			free (str);
		}
			
		XtFree(db);
		return (False);
	}
	XtFree(db);

	OutputProxyClients (outputDB);
	
	OutputXSMPClients (outputDB);

	CloseClientDB (outputDB, True);

	return (True);
}


/*************************************<->*************************************
 *
 *  OutputProxyClients -
 *
 *  Description: Outputs the Proxy clients
 *
 *  Inputs: database pointer
 * 
 *  Returns: void
 *
 *************************************<->***********************************/
static void
OutputProxyClients (
	ClientDB		outputDB)
{
	int		i;
	for (i = 0; i < numProxyClients; i++)
		PrintStartupInfo (outputDB, 
				  proxyList[i].screen, 
				  proxyList[i].window);
}


/*************************************<->*************************************
 *
 *  OutputXSMPClients -
 *
 *  Description: Outputs the XSMP clients
 *
 *  Inputs: database pointer
 * 
 *  Returns: void
 *
 *************************************<->***********************************/
static void
OutputXSMPClients (
	ClientDB		outputDB)
{
	XSMPClientDBRec		dbRec;
	ClientRecPtr            pClientRec;

	for (pClientRec = connectedList; pClientRec != NULL;
		pClientRec = pClientRec->next) {

		dbRec.restartHint = pClientRec->restartHint;
		(void) GetCardPropertyValue (pClientRec, SmRestartStyleHint, 
					     &dbRec.restartHint);

		if (dbRec.restartHint == SmRestartNever)
			continue;

		if (!pClientRec->active)
			continue;

		if (!CheckRequiredProperties (pClientRec))
			continue;

		dbRec.screenNum = 0;

		dbRec.clientId = pClientRec->clientId;
		dbRec.clientHost = pClientRec->clientHost;

		dbRec.program = GetArrayPropertyValue (pClientRec, 
					SmProgram);
		dbRec.cwd = GetArrayPropertyValue (pClientRec, 
					SmCurrentDirectory);

		dbRec.restartCommand = GetListOfArrayPropertyValue (pClientRec,
					SmRestartCommand);
		dbRec.cloneCommand = GetListOfArrayPropertyValue (pClientRec,
					SmCloneCommand);
		dbRec.discardCommand = GetListOfArrayPropertyValue (pClientRec,
					SmDiscardCommand);
		dbRec.environment = GetListOfArrayPropertyValue (pClientRec,
					SmEnvironment);

		if (!PutXSMPClientDBRec (outputDB, &dbRec)) {
			
			char		*str;

			str = strdup ((char *) GETMESSAGE (40, 19,
				"Application '%s' cannot be saved."));

			if (str) {
				DtMsgLogMessage (smGD.programName, 
						 DtMsgLogError, str, 
					 	 dbRec.program);
				free (str);
			}
		}

		if (dbRec.restartCommand)
			XtFree ((char *) dbRec.restartCommand);
		if (dbRec.cloneCommand)
			XtFree ((char *) dbRec.cloneCommand);
		if (dbRec.discardCommand)
			XtFree ((char *) dbRec.discardCommand);
		if (dbRec.environment)
			XtFree ((char *) dbRec.environment);
	}
}


/*************************************<->*************************************
 *
 *  CheckRequiredProperties -
 *
 *  Description: Check a client's required properties
 *
 *  Inputs: ClientRecPtr
 * 
 *  Returns: True if the client has all of the required properties;
 *	otherwise returns False
 *
 *************************************<->***********************************/
static Boolean
CheckRequiredProperties (
	ClientRecPtr		pClientRec)
{
	PropertyRecPtr		pPropRec;
	char			* str;
	char			* program = NULL;

	if ((program = GetArrayPropertyValue (pClientRec, SmProgram)) == NULL){

		str = strdup ((char *) GETMESSAGE (40, 23,
			"The following application did not set the required\nproperty '%s':\n\n   %s\n\nThis application will not be saved."));

		if (str) {
			DtMsgLogMessage (smGD.programName, DtMsgLogError, str, 
					 SmProgram, pClientRec->clientId);
			free (str);
		}
		return (False);
	}

	if ((pPropRec = GetPropertyRec (pClientRec, SmRestartCommand)) == NULL){

		str = strdup ((char *) GETMESSAGE (40, 24,
			"24 Application '%s' will not be saved because it did not set\nthe required property '%s'."));

		if (str) {
			DtMsgLogMessage (smGD.programName, DtMsgLogError,
					 str, program, SmRestartCommand);
			free (str);
		}
		return (False);
	}

	if ((pPropRec = GetPropertyRec (pClientRec, SmCloneCommand)) == NULL) {
		/*
		 * This is a warning - still want to save the client
		 */
		str = strdup ((char *) GETMESSAGE (40, 25,
			"Application '%s' did not set the required property '%s'."));

		if (str) {
			DtMsgLogMessage (smGD.programName, DtMsgLogWarning, str,
					 program, SmCloneCommand);
			free (str);
		}
	}

	return (True);
}


/*************************************<->*************************************
 *
 *  NotifyProxyClients -
 *
 *
 *  Description:
 *  -----------
 *  Notify Proxy clients - calls routines to find top level windows, and 
 *  notifies the Proxy clients about an impending save.
 *
 *  Inputs:
 *  ------
 *
 *  Outputs:
 *  -------
 *
 *************************************<->***********************************/
void 
NotifyProxyClients ( )
{
  unsigned int 		topListLength, containedListLength;
  WindowInfo 		* topList;	/* list of top-level windows */
  WindowInfo 		topLevelWindowInfo;
  Atom 			actualType;
  int 			actualFormat;
  unsigned long 	nitems;
  unsigned long 	leftover;
  unsigned char 	* data = NULL;
  Boolean		containedClient;
  int 			i, j;
  int			numClients = 0;

  /*
   * Save off the clients currently running.  This will be done
   * one screen at a time
   */
  for(j = 0;j < smGD.numSavedScreens;j++)
  {
      /*
       * Get a list of children of the root window. If there are none
       * return with an error condition.
       */
      if(GetTopLevelWindows(j,&topList,&topListLength, &containedListLength))
	  return;
  
      /*
       * Figure out how to restart the client associate with each window.
       */
      for (i=0 ; i<(topListLength + containedListLength); ++i)
      {
	  topLevelWindowInfo=topList[i];

	  if(i >= topListLength)
	  {
	      containedClient = True;
	  }
	  else
	  {
	      containedClient = False;
	  }
      
	  /*
	   * If this window is my window - skip it I don't want to
	   * save myself because I will be started automatically
	   */
	  if (XGetWindowProperty(smGD.display, topLevelWindowInfo.wid,
				 XA_WM_CLASS,0L,
				 (long)BUFSIZ,False,XA_STRING,&actualType,
				 &actualFormat,&nitems,&leftover,
				 &data)==Success)
	  {
	      if (actualType==XA_STRING &&
		  !strcmp((char *) data, SM_RESOURCE_NAME))
	      {
		  SM_FREE((char *)data);
		  continue;
	      }
	      else
	      {
		  SM_FREE((char *)data);
	      }
	  }
      
	 /*
	  * If the client participates in the WM_SAVE_YOURSELF protocol, 
	  * send the appropriate messages and wait for the proper 
	  * responses.
	  */
	 (void) SaveYourself(topLevelWindowInfo);

	 CacheProxyClient ((numClients == 0), j, topLevelWindowInfo.wid);

	 numClients++;
      }

      /*
       * Free space malloc'd by GetTopLevelWindowInfo
       */
      if(topListLength > 0)
      {
	  SM_FREE((char *) topList);
      }
  }
}


/*************************************<->*************************************
 *
 *  CacheProxyClient -
 *
 *  Description:
 *  -----------
 *  Add a Proxy client to the proxy client cache.
 *
 *  Inputs:
 *  ------
 *  firstClient - if True, this is the first client in the cache
 *  screenNum - the client's screen number
 *  window - the client's window number
 *
 *  Outputs:
 *  -------
 *
 *************************************<->***********************************/
static void CacheProxyClient (
	Boolean			firstClient, 
	int			screenNum, 
	Window			window)
{
	if (firstClient)
		numProxyClients = 0;

	if (numProxyClients >= numProxyAllocated) {
		numProxyAllocated += proxyCacheAllocIncrement;
		proxyList = (ProxyClientCachePtr) XtRealloc ((char *) proxyList,
			numProxyAllocated * sizeof (ProxyClientCache));
	}

	proxyList[numProxyClients].screen = screenNum;
	proxyList[numProxyClients].window = window;

	numProxyClients++;
}



/*************************************<->*************************************
 *
 *  PrintStartupInfo -
 *
 *  Description:
 *  -----------
 *  Output startup information for one proxy client 
 *
 *  Inputs:
 *  ------
 *  outputDB = a pointer to the client database
 *  screen = which screen the client is running on
 *  window = the top level window of the lient
 * 
 *  Outputs:
 *  -------
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
static void
PrintStartupInfo(
	ClientDB outputDB,
        int screen,
        Window window)
{
  Boolean		writeStatus;
  int 			wargc;
  char 			** wargv;
  char 			*clientMachine;
  XWindowAttributes 	windowAttr;
  Boolean		remoteHost;
  Boolean		xsmpClient;
  ProxyClientDBRec 	dbRec;
  char			** argv;
  int			i;
  Status		status;

#ifdef _SUN_OS /* pull out quotes from OpenWindow tools commands */
  char 			* ptr;
#endif

  /*
   * Get the attributes and properties on the window
   */
  if ((XGetWindowAttributes(smGD.display, window, &windowAttr)) == 0)
    return;

  if (GetStandardProperties(window, screen, &wargc, &wargv, 
			    &clientMachine, &xsmpClient)==0)
    return;
  
  /*
   * If the client doesn't have a WM_COMMAND property, we can't restart it
   */
  if (!wargv||!wargv[0]) 
    return;

  /*
   * If the client is an XSMP client, don't save it as a Proxy client.
   */
  if (xsmpClient)
     return;

#ifdef _SUN_OS /* pull out quotes from OpenWindow tools commands */
  if (wargc == 1) {
     ptr = wargv[0];
     /* check for foreground */
     if ((ptr = strstr (ptr, "fg \"")) != NULL) {
        if (strlen (ptr) > 3)
           ptr[3] = ' ';
        if ((ptr = strchr (ptr, '"')) != NULL)
           ptr[0] = ' ';
     }

     /* check for background */
     if ((ptr = strstr (wargv[0], "bg \"")) != NULL) {
        if (strlen (ptr) > 3)
           ptr[3] = ' ';
        if ((ptr = strchr (ptr, '"')) != NULL)
           ptr[0] = ' ';
     }
  }
#endif
  
  /*
   * Find out if the client is running locally
   */
  dbRec.clientHost = NULL;
  if(clientMachine != NULL)
  {
      if(DtIsLocalHostP(clientMachine) == FALSE)
      {
	  dbRec.clientHost = clientMachine;
      }
  }

  /*
   * Must NULL-terminate the WM_COMMAND before writing it
   */
  argv = (char **) XtMalloc ((wargc + 1) * sizeof (char *));
  if (!argv)
      return;

  for (i = 0; i < wargc; i++)
      argv[i] = wargv[i];
  argv[wargc] = NULL;

  dbRec.command = argv;
  dbRec.screenNum = screen;

  writeStatus = PutProxyClientDBRec (outputDB, &dbRec);

  XtFree ((char *) argv);

  if (writeStatus == False)
  {
    /*
     * There should be an error written here - however
     * since this means that there already is a write error to
     * the disk - no error can be written
     */
    (void) CloseClientDB (outputDB, True);

    /*
     * On a write error - move the old session back to
     * be used and exit with an error condition
     */
    XFreeStringList(wargv);

    strcat(smGD.savePath, "/");
    if(smGD.sessionType == HOME_SESSION)
    {
        strcat(smGD.savePath, SM_HOME_DIRECTORY);
    }
    else
    {
        strcat(smGD.savePath, SM_CURRENT_DIRECTORY);
    }

    MoveDirectory(smGD.etcPath, smGD.savePath, False);
    SM_EXIT(-1);
  }

  /*
   * Free up the argv property
   */
  XFreeStringList(wargv);
}


/*************************************<->*************************************
 *
 *  QueryServerSettings ()
 *
 *
 *  Description:
 *  -----------
 *  Query the server for all it's current settings information, and then
 *  write that information to the session managers settings resource file.
 *
 *  Inputs:
 *  ------
 *  smGD.settingPath = (global) pointer to file where
 *			setting info should be saved.
 * 
 *  Outputs:
 *  -------
 *
 *  Comments:
 *  --------
 *  This routine slows down the session manager's shutdown process
 *  considerably, and is therefore only called when the user specifies
 *  that all settings should be queried.  Otherwise, only settings set in
 *  the customizer should be saved.
 * 
 *************************************<->***********************************/
static int 
QueryServerSettings( void )
{
    int			i, numLoops;
    char		**fontPath;
    KeySym		*tmpSyms, tmpMod;
    XModifierKeymap	*modMap;
    char		*buttonRet;
    int			numButton, numPaths;
    int			buttonSize = 128;
    XrmDatabase 	smBase = NULL;
    char		tmpChar[35];


    buttonRet = (char *) SM_MALLOC(buttonSize * sizeof(char));
    if (buttonRet==NULL)
    {
	PrintErrnoError(DtError, smNLS.cantMallocErrorString);
	return(-1);
    }

    /*
     * First set up all pointer control parameters
     * These are simple integer values that get saved
     * here, then read into a structure and set at restore
     * time
     */
    XGetPointerControl(smGD.display, &smSettings.accelNum,
		       &smSettings.accelDenom,
		       &smSettings.threshold);
    PrintPointerControl(&smBase);

    /*
     * Get all the screen saver values.  These are all simple integer
     * values that are read back into a structure and set at restore
     * time.
     */
    XGetScreenSaver(smGD.display, &screenSaverVals.smTimeout,
		    &screenSaverVals.smInterval,
		    &screenSaverVals.smPreferBlank,
		    &screenSaverVals.smAllowExp);
    PrintScreenSaver(&smBase);

    /*
     *  Write out all font path information. 
     *  Write out all the paths one by one concatenated by commas.
     */
    fontPath = XGetFontPath(smGD.display, &numPaths);
    if(fontPath != NULL)
    {
	if(PrintFontPaths(&smBase, fontPath, numPaths) != 0)
	{
	    XFreeFontPath(fontPath);
	    SM_FREE(buttonRet);
	    return(-1);
	}
    }

    /*
     * Write out all the keyboard control information.
     */
    XGetKeyboardControl(smGD.display, &smSettings.kbdState);
    if(PrintKeyboardControl(&smBase) != 0)
    {
	if (fontPath)
	    XFreeFontPath(fontPath);
	return(-1);
    }
    
    /*
     * Put all the button modifier information into the resource database.
     * Find out the number of buttons and then write out the character
     * string for each button.
     */
    numButton = XGetPointerMapping(smGD.display, (unsigned char *)buttonRet, 128);
    if(numButton > 128)
    {
	buttonRet = (char *) SM_REALLOC(buttonRet, numButton * sizeof(char));
	if (buttonRet==NULL)
	{
	    if (fontPath)
		XFreeFontPath(fontPath);
	    PrintErrnoError(DtError, smNLS.cantMallocErrorString);
	    return(-1);
	}
	numButton = XGetPointerMapping(smGD.display, (unsigned char *)buttonRet, numButton);
    }
    if(PrintPointerMapping(&smBase, buttonRet,
			   numButton)  != 0)
    {
	if (fontPath)
	    XFreeFontPath(fontPath);
	SM_FREE(buttonRet);
	return(-1);
    }

    /*
     * Put all keyboard mapping information in the resource database
     * Don't bother putting this in a separate print function because
     * the customizer doesn't do keymap savings this way.
     */
    smSettings.numKeyCode = smGD.display->max_keycode -
	smGD.display->min_keycode;
    tmpSyms = XGetKeyboardMapping(smGD.display,
				  (KeyCode) smGD.display->min_keycode,
				  smSettings.numKeyCode,
				  &smSettings.keySymPerCode);
    if(tmpSyms == NULL)
    {
	if (fontPath)
	    XFreeFontPath(fontPath);
	SM_FREE(buttonRet);
	PrintErrnoError(DtError, smNLS.cantMallocErrorString);
	return(-1);
    }
    
    strcpy(resSpec, SM_RESOURCE_CLASS);
    strcat(resSpec, "*");
    strcat(resSpec, SmCnumKeyCode);
    strcat(resSpec, ":");
    sprintf(tmpChar,"%d", smSettings.numKeyCode);
    strcat(resSpec, tmpChar);
    XrmPutLineResource( &smBase, resSpec);
    
    strcpy(resSpec, SM_RESOURCE_CLASS);
    strcat(resSpec, "*");
    strcat(resSpec, SmCkeySymsPerKey);
    strcat(resSpec, ":");
    sprintf(tmpChar,"%d", smSettings.keySymPerCode);
    strcat(resSpec, tmpChar);
    XrmPutLineResource( &smBase, resSpec);

    strcpy(resSpec, SM_RESOURCE_CLASS);
    strcat(resSpec, "*");
    strcat(resSpec, SmCkeySyms);
    strcat(resSpec, ":");
    numLoops = smSettings.numKeyCode * smSettings.keySymPerCode;
    sprintf(tmpChar,"%ld", tmpSyms[0]);
    strcat(resSpec, tmpChar);
    for(i = 1;i < numLoops;i++)
    {
	if((strlen(resSpec) + 5) >= resSize)
	{
	    resSize += 5000;
	    resSpec = (char *) SM_REALLOC(resSpec, resSize * sizeof(char));
	    if (resSpec==NULL)
	    {
		SM_FREE((char *)tmpSyms);
		if (fontPath)
		    SM_FREE((char *)fontPath);
		SM_FREE(buttonRet);
		PrintErrnoError(DtError, smNLS.cantMallocErrorString);
		return(-1);
	    }
	}
	strcat(resSpec, ",");
	sprintf(tmpChar,"%ld", tmpSyms[i]);
	strcat(resSpec, tmpChar);
    }
    XrmPutLineResource( &smBase, resSpec);
	

    /*
     * Write out all keyboard modifier info.  This will be a pretty slow
     * process considering that every keycode will have to be turned into
     * a keysym before writing.  Don't bother putting print stuff in a
     * separate function because it will not be handled in the customize
     * version.
     */
    modMap = XGetModifierMapping(smGD.display);
    if(modMap == NULL)
    {
	SM_FREE((char *)tmpSyms);
	if (fontPath)
	    XFreeFontPath(fontPath);
	SM_FREE(buttonRet);
	PrintErrnoError(DtError, smNLS.cantMallocErrorString);
	return(-1);
    }

    strcpy(resSpec, SM_RESOURCE_CLASS);
    strcat(resSpec, "*");
    strcat(resSpec, SmCmaxKeyPerMod);
    strcat(resSpec, ":");
    sprintf(tmpChar,"%d", modMap->max_keypermod);
    strcat(resSpec, tmpChar);
    XrmPutLineResource( &smBase, resSpec);

    strcpy(resSpec, SM_RESOURCE_CLASS);
    strcat(resSpec, "*");
    strcat(resSpec, SmCmodMap);
    strcat(resSpec, ":");
    numLoops = (8 * modMap->max_keypermod) - 1;
    for(i = 0;i <= numLoops;i++)
    {
	if((strlen(resSpec) + 9) >= resSize)
	{
	    resSize += 5000;
	    resSpec = (char *) SM_REALLOC(resSpec, resSize * sizeof(char));
	    if (resSpec==NULL)
	    {
		SM_FREE((char *)tmpSyms);
		XFreeModifiermap(modMap);
		if (fontPath)
		    XFreeFontPath(fontPath);
		SM_FREE(buttonRet);
		PrintErrnoError(DtError, smNLS.cantMallocErrorString);
		return(-1);
	    }
	}
	tmpMod = XKeycodeToKeysym(smGD.display, modMap->modifiermap[i], 0);
	sprintf(tmpChar,"%ld", tmpMod);
	strcat(resSpec, tmpChar);
	if(i != numLoops)
	{
	    strcat(resSpec, ",");
	}
    }
    XrmPutLineResource( &smBase, resSpec);
    

    /*
     * Print session manager settings
     */
    PrintSessionInfo(&smBase);
    
    /*
     * Write settings resources out to disk
     */
    XrmPutFileDatabase(smBase, smGD.settingPath);

    /*
     * Free all allocated resources
     */
    if (fontPath)
	XFreeFontPath(fontPath);
    SM_FREE((char *)tmpSyms);
    XFreeModifiermap(modMap);
    SM_FREE(buttonRet);
    XrmDestroyDatabase(smBase);
    return(0);
}


/*************************************<->*************************************
 *
 *  SaveCustomizeSettings ()
 *
 *
 *  Description:
 *  ------------
 *  Saves those settings that have been set in the customizer during the
 *  session to whatever values they were set to during the session.
 *
 *
 *  Inputs:
 *  ------
 *  smGD.resourcePath = (global) a pointer
 *			to where the settings should be stored.
 *
 * 
 *  Outputs:
 *  -------
 *
 *
 *  Comments:
 *  --------
 *  This routine is the default saved for saving settings.  If the user
 *  wants all settings saved, they have to set a resource saying that that
 *  will be the case.
 * 
 *************************************<->***********************************/
static int 
SaveCustomizeSettings( void )
{
    int			i,numPaths;
    char		**fontPath;
    XrmDatabase 	smBase = NULL;
    char		*tmpChar[20], *ptrRet = NULL, *tmpKey;
    Status		newStat;
	char		tmpMap[128];


    if(smCust.pointerChange)
    {
	newStat = _DtGetSmPointer(smGD.display, smGD.topLevelWindow,
				 XaDtSmPointerInfo, &ptrRet);
	if(newStat != Success)
	{
	    smToSet.pointerChange = False;
	    smToSet.pointerMapChange = False;
	}
	else
	{
	    sscanf(ptrRet, "%d %s %d %d %s",
		   &smToSet.numButton,
		   &tmpMap,
		   &smSettings.accelNum,
		   &smSettings.threshold,
		   &smToSet.dClickBuf);
	    smSettings.accelDenom = 1;

	    tmpKey = strtok(tmpMap, "_");
	    i = 0;
	    while(tmpKey != NULL)
	    {
		smToSet.pointerMap[i] = (char) atoi(tmpKey);
		i++;
		tmpKey = strtok(NULL, "_");
	    }


	    /*
	     * append a \n to dClickBuf because it was scanned out
	     */
	    strcat(smToSet.dClickBuf, "\n");
	}
        if (ptrRet) XFree(ptrRet);
    }
				     
    if(smToSet.pointerChange == True)
    {
	PrintPointerControl(&smBase);
    }

    if(smToSet.pointerMapChange == True)
    {
	if(PrintPointerMapping(&smBase, smToSet.pointerMap,
			    smToSet.numButton) != 0)
	{
	    return(-1);
	}
    }

    if(smToSet.screenSavChange == True)
    {
	PrintScreenSaver(&smBase);
    }

    if(smCust.audioChange == True)
    {
      newStat = _DtGetSmAudio(smGD.display, smGD.topLevelWindow,
				   XaDtSmAudioInfo, &audioVals);
     
      if(newStat == Success)
      {
        smSettings.kbdState.bell_percent = audioVals.smBellPercent;
        smSettings.kbdState.bell_pitch = audioVals.smBellPitch;
        smSettings.kbdState.bell_duration = audioVals.smBellDuration;
      }  
      else
      {
        smToSet.audioChange = False;
      }
    }
    
    if (smToSet.audioChange)
    {
      strcpy(resSpec, SM_RESOURCE_CLASS);
      strcat(resSpec, "*");
      strcat(resSpec, SmCbellPercent);
      strcat(resSpec, ":");
      sprintf((char *)tmpChar,"%d",smSettings.kbdState.bell_percent);
      strcat(resSpec, (char *)tmpChar);
      XrmPutLineResource(&smBase, resSpec);

      strcpy(resSpec, SM_RESOURCE_CLASS);
      strcat(resSpec, "*");
      strcat(resSpec, SmCbellPitch);
      strcat(resSpec, ":");
      sprintf((char *)tmpChar,"%d",smSettings.kbdState.bell_pitch);
      strcat(resSpec, (char *)tmpChar);
      XrmPutLineResource(&smBase, resSpec);

      strcpy(resSpec, SM_RESOURCE_CLASS);
      strcat(resSpec, "*");
      strcat(resSpec, SmCbellDuration);
      strcat(resSpec, ":");
      sprintf((char *)tmpChar,"%d",smSettings.kbdState.bell_duration);
      strcat(resSpec, (char *)tmpChar);
      XrmPutLineResource(&smBase, resSpec);
    }

    if(smCust.keyboardChange == True)
    {
      newStat = _DtGetSmKeyboard(smGD.display, smGD.topLevelWindow,
                                      XaDtSmKeyboardInfo, &keyboardVals);
      if(newStat == Success)
      {
        smSettings.kbdState.key_click_percent = keyboardVals.smKeyClickPercent;
        smSettings.kbdState.global_auto_repeat = keyboardVals.smGlobalAutoRepeat;
      }
      else
      {
        smToSet.keyboardChange = False;
      }
    }

    if(smToSet.keyboardChange == True)
    {
      strcpy(resSpec, SM_RESOURCE_CLASS);
      strcat(resSpec, "*");
      strcat(resSpec, SmCkeyClick);
      strcat(resSpec, ":");
      sprintf((char *)tmpChar,"%d", smSettings.kbdState.key_click_percent);
      strcat(resSpec, (char *)tmpChar);
      XrmPutLineResource(&smBase, resSpec);

      strcpy(resSpec, SM_RESOURCE_CLASS);
      strcat(resSpec, "*");
      strcat(resSpec, SmCglobalRepeats);
      strcat(resSpec, ":");
      sprintf((char *)tmpChar,"%d", smSettings.kbdState.global_auto_repeat);
      strcat(resSpec, (char *)tmpChar);
      XrmPutLineResource(&smBase, resSpec);
    }


    /*
     *  Write out all font path information. 
     *  Write out all the paths one by one concatenated by commas.
     */

    fontPath = XGetFontPath(smGD.display, &numPaths);
    if(fontPath != NULL) {
	if(PrintFontPaths(&smBase,  fontPath, numPaths) != 0) {
	    XFreeFontPath(fontPath);
	    return(-1);
	}
    }

    /*
     * Save session manager settings
     */
    PrintSessionInfo(&smBase);
    
    if (fontPath)
	XFreeFontPath(fontPath);

    /*
     * Write settings resources out to disk
     */
    XrmPutFileDatabase(smBase, smGD.settingPath);
    XrmDestroyDatabase(smBase);

    return(0);
}



/*************************************<->*************************************
 *
 *  OutputResource ()
 *
 *
 *  Description:
 *  -----------
 *  Save session resources by getting the current RESOURCE_MANAGER property
 *  on the root window.
 *
 *  Inputs:
 *  ------
 *  smGD.resourcePath = (global) file name
 *			where property contents should be saved.
 * 
 *  Outputs:
 *  -------
 *
 *
 *  Comments:
 *  --------
 *  This method is not a 100% accurate representation of currently active
 *  resources in the system.  It is however, the best representation (the
 *  values with the highest priority) without becoming overly invasive
 *  and slow.
 * 
 *************************************<->***********************************/
static int 
OutputResource( void )
{
    Atom 		actualType;
    int	 		actualFormat;
    unsigned long	nitems, leftover; 
    char	 	*data = NULL;
    XrmDatabase		db;
    Status		newStat;
    char		*fontBuf = NULL, *langPtr, tmpChar[20], *sessionRes;
    float		fltYRes;
    int			intYRes;
    char                *preeditBuf = NULL;

    /*
     * Add anything to the Resource Manager property that needs to be added
     */

    /*
     * Write out the LANG variable and the screen's Y resolution
     */
    *resSpec = 0;
    langPtr = getenv("LANG");
    if((langPtr != NULL) && (*langPtr != 0))
    {
	sprintf(resSpec, "%s*%s: %s\n", SM_RESOURCE_NAME, SmNsessionLang,
		langPtr);
    }
    else
    {
	sprintf(resSpec, "%s*%s:   \n", SM_RESOURCE_NAME, SmNsessionLang);
    }

    fltYRes = ((float) DisplayHeight(smGD.display, 0) /
	(float) DisplayHeightMM(smGD.display, 0)) * 1000;
    intYRes = fltYRes;

    if(*resSpec == 0)
    {
	strcpy(resSpec, SM_RESOURCE_NAME);
    }
    else
    {
	strcat(resSpec, SM_RESOURCE_NAME);
    }
    strcat(resSpec, "*");
    strcat(resSpec, SmNdisplayResolution);
    strcat(resSpec, ":");
    sprintf(tmpChar, "%d",intYRes);
    strcat(resSpec, tmpChar);
    strcat(resSpec, "\n");
    _DtAddToResource(smGD.display, resSpec);
    
    if(smCust.fontChange == True)
    {
	newStat = _DtGetSmFont(smGD.display, smGD.topLevelWindow,
			      XaDtSmFontInfo, &fontBuf);
	if(newStat == Success)
	{
	    /*
	     * Create the auxillary resource file
	     */
	    if(SetFontSavePath(langPtr) != -1)
	    {
		db = XrmGetStringDatabase(fontBuf);
	    
		if(intYRes < MED_RES_Y_RES)
		{
		    sessionRes = SM_LOW_RES_EXT;
		}
		else
		{
		    if(intYRes >= HIGH_RES_Y_RES)
		    {
			sessionRes = SM_HIGH_RES_EXT;
		    }
		    else
		    {
			sessionRes = SM_MED_RES_EXT;
		    }
		}

		strcat(smGD.fontPath, "/");
		strcat(smGD.fontPath, SM_FONT_FILE);
		
		strcat(smGD.fontPath, ".");
		strcat(smGD.fontPath, sessionRes);
		    
		XrmPutFileDatabase(db, smGD.fontPath);    
		XrmDestroyDatabase(db);
	    }
	    
	    /*
	     * Now add this to the resource manager property to be saved
	     */
	    _DtAddToResource(smGD.display, fontBuf);
	}
        if (fontBuf) XFree(fontBuf);
    }

    if(smCust.preeditChange == True)
    {
	newStat = _DtGetSmPreedit(smGD.display, smGD.topLevelWindow,
				  XaDtSmPreeditInfo, &preeditBuf);
	if(newStat == Success)
	{
	    _DtAddToResource(smGD.display, preeditBuf);
	}
	if (preeditBuf) XFree(preeditBuf);
    }

    if((smCust.dClickChange == True) && (smToSet.dClickBuf[0] != 0))
    {
	_DtAddToResource(smGD.display, (char *)smToSet.dClickBuf);
    }

    /*
     * Get the contents of the _DT_SM_PREFERENCES property
     */
    data = _DtGetResString(smGD.display, _DT_ATR_PREFS);

   /* 
    * Get _DT_SM_PREFERENCES database 'db'.
    */
    db  = XrmGetStringDatabase((char *)data);

    XrmPutFileDatabase(db, smGD.resourcePath);

    /*
     * Don't forget to free your data
     */
    SM_FREE((char *)data);
    XrmDestroyDatabase(db);
    
    return(0);
}



/*************************************<->*************************************
 *
 *  PrintPointerControl (smBase)
 *
 *
 *  Description:
 *  -----------
 *  A convenience function that is separated out instead of being included
 *  in both QueryServerSettings, and SaveCustomizeSettings.
 *  Saves pointer control information to the named resource file.
 *
 *  Inputs:
 *  ------
 *  smBase = pointer to newly opened resource db used to store setting info
 *  resSpec = a buffer to hold string resource information until it is
 *            printed.
 *  smSettings = place where settings are stored.
 * 
 *  Outputs:
 *  -------
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
static void 
PrintPointerControl(
        XrmDatabase *smBase)
{
    char tmpChar[20];
    
    strcpy(resSpec, SM_RESOURCE_CLASS);
    strcat(resSpec, "*");
    strcat(resSpec, SmCaccelNum);
    strcat(resSpec, ":");
    sprintf(tmpChar, "%d", smSettings.accelNum);
    strcat(resSpec, tmpChar);
    XrmPutLineResource(smBase, resSpec);
    
    strcpy(resSpec, SM_RESOURCE_CLASS);
    strcat(resSpec, "*");
    strcat(resSpec, SmCaccelDenom);
    strcat(resSpec, ":");
    sprintf(tmpChar, "%d",smSettings.accelDenom);
    strcat(resSpec, tmpChar);
    XrmPutLineResource(smBase, resSpec);
    
    strcpy(resSpec, SM_RESOURCE_CLASS);
    strcat(resSpec, "*");
    strcat(resSpec, SmCthreshold);
    strcat(resSpec, ":");
    sprintf(tmpChar, "%d",smSettings.threshold);
    strcat(resSpec, tmpChar);
    XrmPutLineResource(smBase, resSpec);

    return;
}


/*************************************<->*************************************
 *
 *  PrintSessionInfo (smBase)
 *
 *
 *  Description:
 *  -----------
 *  A convenience function that is separated out instead of being included
 *  in both QueryServerSettings, and SaveCustomizeSettings.
 *  Saves session manager settings to the named resource file.
 *
 *  Inputs:
 *  ------
 *  smBase = pointer to newly opened resource db used to store setting info
 *  resSpec = a buffer to hold string resource information until it is
 *            printed.
 *  smSettings = place where settings are stored.
 * 
 *  Outputs:
 *  -------
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
static void 
PrintSessionInfo(
        XrmDatabase *smBase)
{
    char tmpChar[20];

    /*
     * Write out the settings for logout confirmation and
     * the correct session to return to
     */
    strcpy(resSpec, SM_RESOURCE_CLASS);
    strcat(resSpec, "*");
    strcat(resSpec, SmCshutDownState);
    strcat(resSpec, ":");
    sprintf(tmpChar, "%d",smSettings.confirmMode);
    strcat(resSpec, tmpChar);
    XrmPutLineResource(smBase, resSpec);
    
    strcpy(resSpec, SM_RESOURCE_CLASS);
    strcat(resSpec, "*");
    strcat(resSpec, SmCshutDownMode);
    strcat(resSpec, ":");
    sprintf(tmpChar, "%d",smSettings.startState);
    strcat(resSpec, tmpChar);
    XrmPutLineResource(smBase, resSpec);

    return;
}


/*************************************<->*************************************
 *
 *  PrintScreenSaver (smBase)
 *
 *
 *  Description:
 *  -----------
 *  A convenience function that is separated out instead of being included
 *  in both QueryServerSettings, and SaveCustomizeSettings.
 *  Saves screen saver information to the named resource file.
 *
 *  Inputs:
 *  ------
 *  smBase = pointer to newly opened resource db used to store setting info
 *  resSpec = a buffer to hold string resource information until it is
 *            printed.
 * 
 *  Outputs:
 *  -------
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
static void 
PrintScreenSaver(
        XrmDatabase *smBase)
{
    Status		newScreenStat;
    char 		tmpChar[20];

    if(smCust.screenSavChange == True)
    {
	newScreenStat = _DtGetSmScreen(smGD.display, smGD.topLevelWindow,
				      XaDtSmScreenInfo, &screenSaverVals);
	/*
	 *  If the property has been deleted, it means that we return to
	 *  the default
	 */
	if(newScreenStat != Success)
	{
	    return;
	}
    }

    strcpy(resSpec, SM_RESOURCE_CLASS);
    strcat(resSpec, "*");
    strcat(resSpec, SmCtimeout);
    strcat(resSpec, ":");
    sprintf(tmpChar, "%d", screenSaverVals.smTimeout);
    strcat(resSpec, tmpChar);
    XrmPutLineResource(smBase, resSpec);
    
    strcpy(resSpec, SM_RESOURCE_CLASS);
    strcat(resSpec, "*");
    strcat(resSpec, SmCinterval);
    strcat(resSpec, ":");
    sprintf(tmpChar, "%d", screenSaverVals.smInterval);
    strcat(resSpec, tmpChar);
    XrmPutLineResource(smBase, resSpec);
    
    strcpy(resSpec, SM_RESOURCE_CLASS);
    strcat(resSpec, "*");
    strcat(resSpec, SmCpreferBlank);
    strcat(resSpec, ":");
    sprintf(tmpChar, "%d", screenSaverVals.smPreferBlank);
    strcat(resSpec, tmpChar);
    XrmPutLineResource(smBase, resSpec);
    
    strcpy(resSpec, SM_RESOURCE_CLASS);
    strcat(resSpec, "*");
    strcat(resSpec, SmCallowExp);
    strcat(resSpec, ":");
    sprintf(tmpChar, "%d", screenSaverVals.smAllowExp);
    strcat(resSpec, tmpChar);
    XrmPutLineResource(smBase, resSpec);

    return;
}


/*************************************<->*************************************
 *
 *  PrintFontPaths (smBase, fontPaths, numPaths)
 *
 *
 *  Description:
 *  -----------
 *  A convenience function that is separated out instead of being included
 *  in both QueryServerSettings, and SaveCustomizeSettings.
 *  Saves pointer control information to the named resource file.
 *
 *  Inputs:
 *  ------
 *  smBase = pointer to newly opened resource db used to store setting info
 *  resSpec = a buffer to hold string resource information until it is
 *            printed
 *  fontPaths = font paths to be saved
 *  numPaths = number of font paths to be saved
 * 
 *  Outputs:
 *  -------
 *  resSize = made bigger if current buffer is too small to hold all font info
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
static int 
PrintFontPaths(
        XrmDatabase *smBase,
        char **fontPath,
        int numPaths)
{
    int	i;
    
    /* If no fp save req'd, just pretend we did it */
    if (smRes.saveFontPath == False) return 0;

    strcpy(resSpec, SM_RESOURCE_CLASS);
    strcat(resSpec, "*");
    strcat(resSpec, SmCfontPath);
    strcat(resSpec, ":");
    strcat(resSpec, *fontPath);
    for(i = 1;i < numPaths;i++)
    {
	if((strlen(resSpec) + strlen(fontPath[i]) + 1) >= resSize)
	{
	    resSize += 5000;
	    resSpec = (char *) SM_REALLOC(resSpec, resSize * sizeof(char));
	    if (resSpec==NULL)
	    {
		PrintErrnoError(DtError, smNLS.cantMallocErrorString);
		return(-1);
	    }
	}
	strcat(resSpec, ",");
	strcat(resSpec, fontPath[i]);
    }
    XrmPutLineResource(smBase, resSpec);

    return(0);
}



/*************************************<->*************************************
 *
 *  PrintKeyboardControl (smBase)
 *
 *
 *  Description:
 *  -----------
 *  A convenience function that is separated out instead of being included
 *  in both QueryServerSettings, and SaveCustomizeSettings.
 *  Saves pointer control information to the named resource file.
 *
 *  Inputs:
 *  ------
 *  smBase = pointer to newly opened resource db used to store setting info
 *  resSpec = a buffer to hold string resource information until it is
 *            printed
 *  resSize = size of current resource buffer
 * 
 *  Outputs:
 *  -------
 *  resSize = buffer is enlarged if more room is need to hold this info
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
static int 
PrintKeyboardControl(
        XrmDatabase *smBase)
{
    int		i,j;
    char	bitTest;
    Bool	firstRepeat = True;
    char	tmpChar[20];
    
    strcpy(resSpec, SM_RESOURCE_CLASS);
    strcat(resSpec, "*");
    strcat(resSpec, SmCkeyClick);
    strcat(resSpec, ":");
    sprintf(tmpChar, "%d",smSettings.kbdState.key_click_percent);
    strcat(resSpec, tmpChar);
    XrmPutLineResource(smBase, resSpec);

    strcpy(resSpec, SM_RESOURCE_CLASS);
    strcat(resSpec, "*");
    strcat(resSpec, SmCbellPercent);
    strcat(resSpec, ":");
    sprintf(tmpChar, "%d",smSettings.kbdState.bell_percent);
    strcat(resSpec, tmpChar);
    XrmPutLineResource(smBase, resSpec);

    strcpy(resSpec, SM_RESOURCE_CLASS);
    strcat(resSpec, "*");
    strcat(resSpec, SmCbellPitch);
    strcat(resSpec, ":");
    sprintf(tmpChar, "%d",smSettings.kbdState.bell_pitch);
    strcat(resSpec, tmpChar);
    XrmPutLineResource(smBase, resSpec);

    strcpy(resSpec, SM_RESOURCE_CLASS);
    strcat(resSpec, "*");
    strcat(resSpec, SmCbellDuration);
    strcat(resSpec, ":");
    sprintf(tmpChar, "%d",smSettings.kbdState.bell_duration);
    strcat(resSpec, tmpChar);
    XrmPutLineResource(smBase, resSpec);
    
    strcpy(resSpec, SM_RESOURCE_CLASS);
    strcat(resSpec, "*");
    strcat(resSpec, SmCledMask);
    strcat(resSpec, ":");
    sprintf(tmpChar, "%ld",smSettings.kbdState.led_mask);
    strcat(resSpec, tmpChar);
    XrmPutLineResource(smBase, resSpec);

    strcpy(resSpec, SM_RESOURCE_CLASS);
    strcat(resSpec, "*");
    strcat(resSpec, SmCglobalRepeats);
    strcat(resSpec, ":");
    sprintf(tmpChar, "%d",smSettings.kbdState.global_auto_repeat);
    strcat(resSpec, tmpChar);
    XrmPutLineResource(smBase, resSpec);

    strcpy(resSpec, SM_RESOURCE_CLASS);
    strcat(resSpec, "*");
    strcat(resSpec, SmCautoRepeats);
    strcat(resSpec, ":");

    /*
     * Now write out which keys need to be auto repeated one at a time
     */
    if(smSettings.kbdState.global_auto_repeat != AutoRepeatModeOn)
    {
	for(i = 0;i < 32;i++)
	{
	    bitTest = 0x01;
	    for(j = 0;j < 8;j++)
	    {
		if((bitTest & smSettings.kbdState.auto_repeats[i]) != 0)
		{
		    if(firstRepeat == True)
		    {
			firstRepeat = False;
		    }
		    else
		    {
			strcat(resSpec, ",");
		    }
		    
		    if((strlen(resSpec) + 5) >= resSize)
		    {
			resSize += 5000;
			resSpec = (char *) SM_REALLOC(resSpec,
						   resSize * sizeof(char));
			if (resSpec==NULL)
			{
			    PrintErrnoError(DtError,
					    smNLS.cantMallocErrorString);
			    return(-1);
			}
		    }
			
		    sprintf(tmpChar, "%d", ((8 * i) + j));
		    strcat(resSpec, tmpChar);
		}
		bitTest <<= 1;
	    }
	}
    }
    else
    {
	strcat(resSpec, "");
    }
    XrmPutLineResource(smBase, resSpec);

    return(0);
}



/*************************************<->*************************************
 *
 *  PrintPointerMapping (smBase, buttonRet,  numButton)
 *
 *
 *  Description:
 *  -----------
 *  A convenience function that is separated out instead of being included
 *  in both QueryServerSettings, and SaveCustomizeSettings.
 *  Saves pointer mapping information to the named resource file.
 *
 *  Inputs:
 *  ------
 *  smBase = pointer to newly opened resource db used to store setting info
 *  resSpec = a buffer to hold string resource information until it is
 *            printed
 *  resSize = size of resource buffer
 *  buttonRet = button mapping to be saved
 *  numButton = number of buttons in button mapping
 * 
 *  Outputs:
 *  -------
 *  resSize = size of buffer is change if it needs to be enlarged during
 *            routine
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
static int 
PrintPointerMapping(
        XrmDatabase *smBase,
        char *buttonRet,
        int numButton )
{
    int		i, numLoops;
    char	tmpChar[20];
    
    strcpy(resSpec, SM_RESOURCE_CLASS);
    strcat(resSpec, "*");
    strcat(resSpec, SmCbuttonMap);
    strcat(resSpec, ":");

    numLoops = numButton - 1;
    for(i = 0;i <= numLoops;i++)
    {
	if((strlen(resSpec) + 5) >= resSize)
	{
	    resSize += 5000;
	    resSpec = (char *) SM_REALLOC(resSpec, resSize * sizeof(char));
	    if (resSpec==NULL)
	    {
		PrintErrnoError(DtError, smNLS.cantMallocErrorString);
		return(-1);
	    }
	}
	    
	sprintf(tmpChar, "%d",buttonRet[i]);
	strcat(resSpec, tmpChar);
	if(i != numLoops)
	{
	    strcat(resSpec, ",");
	}
	XrmPutLineResource(smBase, resSpec);
    }

    return(0);
}
