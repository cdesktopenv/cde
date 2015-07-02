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
/* $TOG: SmGlobals.c /main/37 1998/10/26 17:21:18 mgreess $ */
/*
 * (c) Copyright 1995 Digital Equipment Corporation.
 * (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 * (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 * (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994, 1995 Novell, Inc. 
 * (c) Copyright 1995 FUJITSU LIMITED.
 * (c) Copyright 1995 Hitachi.
 */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:        SmGlobals.c
 **
 **  Project:     HP DT Session Manager (dtsession)
 **
 **  Description:
 **  -----------
 **  This file contains all routines in charge of managing all global
 **  variables used by the session manager.  These variables include
 **  mostly state and setting information.
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
#include <stdio.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <unistd.h>
#ifdef __apollo
#include <X11/apollosys.h>      /* for pid_t, in hp-ux sys/types.h */
#endif
#include <sys/stat.h>
#include <sys/param.h>
#include <X11/Intrinsic.h>
#include <X11/Xutil.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>
#include <Dt/DtP.h>
#include <Dt/SessionP.h>
#include <Dt/Wsm.h>
#include <Dt/WsmP.h>
#include <Dt/UserMsg.h>
#include <Dt/HourGlass.h>
#include <Dt/SessionM.h>
#include <Dt/EnvControlP.h>
#include <Dt/Qualify.h>
#include <Dt/MsgLog.h>
#include "Sm.h"
#include "SmResource.h"
#include "SmError.h"
#include "SmUI.h"
#include "SmGlobals.h"
#include "SmLock.h"
#include "SmRestore.h"
#include "SmProtocol.h"
#include "SmXSMP.h"

/*
 * Internal Variable Declaraions
 */
static char savedDir    [MAXPATHLEN];
static char savedOldDir [MAXPATHLEN];
static char savedTmpDir [MAXPATHLEN];

/*
 * Internal Function Declaraions
 */

static int SetSysDefaults( void ) ;
static int SetResSet( void ) ;
static void RemoveFiles( char *) ;
static void TrimErrorlog(void);

static void _SmWaitClientTimeoutDefault (
	Widget widget, 
       int offset, 
       XrmValue *value);

static void _SmWaitWmTimeoutDefault (
	Widget widget, 
	int offset, 
	XrmValue *value);

void SmCvtStringToContManagement (
	XrmValue *args, 
	Cardinal numArgs,
	XrmValue *fromVal, 
	XrmValue *toVal);

unsigned char *_DtNextToken (
	unsigned char *pchIn, 
	int *pLen, 
	unsigned char **ppchNext);

Boolean _DtWmStringsAreEqual (
	unsigned char *pch1, 
	unsigned char *pch2, 
	int len);

static Boolean InitializeSpecificSession (
	char		*session_name,
	Display		*disp,
	unsigned int	argc,
	char		**argv);

static void InitializeGenericSession (
	Display		*disp);

static void InitializePaths (
	char		*session_option,
	Display		*disp);

static Boolean SetAlternateSession (
	char		* session_dir,
	char		* alt_dir,
	Boolean		make_dir);

/*
 * Global Data
 */

/*
 * These are the global structures used throughout dtsession
 * They are defined in Sm.h
 */
SessionResources     smRes;
SaverResources       smSaverRes;
SettingsSet           smToSet;
SettingsCust     smCust;
SessionSettings      smSettings;
GeneralData          smGD;
char SM_SCREEN_SAVER_LOC [MAXPATHLEN + 1];

/*
 * Internal Global Data
 */
static char     tmpDisplayName[MAXPATHLEN + 1];
int machineType = 0;

static XtResource sessionResources[]=
{
   {SmNwmStartup, SmCwmStartup, XtRString, sizeof(String),
        XtOffset(SessionResourcesPtr, wmStartup),
        XtRImmediate, (XtPointer) NULL},
   {SmNquerySettings, SmCquerySettings, XtRBoolean, sizeof(Boolean),
        XtOffset(SessionResourcesPtr, querySettings),
        XtRImmediate, (XtPointer) False},
   {SmNkeys, SmCkeys, XtRString, sizeof(String),
        XtOffset(SessionResourcesPtr, keyholders), XtRString, NULL},
   {SmNalarmTime, SmCalarmTime, XtRInt, sizeof(int),
        XtOffset(SessionResourcesPtr, alarmTime), 
        XtRImmediate, (XtPointer) 10},
   {SmNmemThreshold, SmCmemThreshold, XtRInt, sizeof(int),
        XtOffset(SessionResourcesPtr, memThreshold), 
        XtRImmediate, (XtPointer) 100},
   {SmNsessionVersion, SmCsessionVersion, XtRString, sizeof(String),
        XtOffset(SessionResourcesPtr, sessionVersion),
        XtRImmediate, (XtPointer) NULL},
   {SmNdisplayResolution, SmCdisplayResolution, XtRInt, sizeof(int),
        XtOffset(SessionResourcesPtr, displayResolution),
        XtRImmediate, (XtPointer) 0},
   {SmNsessionLang, SmCsessionLang, XtRString, sizeof(String),
        XtOffset(SessionResourcesPtr, sessionLang),
        XtRImmediate, (XtPointer) ""},
   {SmNcontManagement, SmCContManagement, SmRContManagement, sizeof(long),
        XtOffset(SessionResourcesPtr, contManagement),
        XtRImmediate, (XtPointer) (SM_CM_DEFAULT)},
   {SmNwaitClientTimeout, SmCWaitClientTimeout, XtRInt, sizeof(int),
        XtOffset(SessionResourcesPtr, waitClientTimeout),
        XtRCallProc, (XtPointer)_SmWaitClientTimeoutDefault },
   {SmNwaitWmTimeout, SmCWaitWmTimeout, XtRInt, sizeof(int),
        XtOffset(SessionResourcesPtr, waitWmTimeout),
        XtRCallProc, (XtPointer)_SmWaitWmTimeoutDefault },
   {SmNuseMessaging, SmCUseMessaging, XtRBoolean, sizeof(Boolean),
        XtOffset(SessionResourcesPtr, useBMS),
        XtRImmediate, (XtPointer) True},
   {SmNsaveFontPath, SmCsaveFontPath, XtRBoolean, sizeof(Boolean),
        XtOffset(SessionResourcesPtr, saveFontPath),
        XtRImmediate, (XtPointer) False},
   {SmNsaveYourselfTimeout, SmCsaveYourselfTimeout, XtRInt, sizeof(int),
        XtOffset(SessionResourcesPtr, saveYourselfTimeout),
        XtRImmediate, (XtPointer) 5},
   {SmNnumSessionsBackedup, SmCnumSessionsBackedup, XtRInt, sizeof(int),
        XtOffset(SessionResourcesPtr, numSessionsBackedup),
        XtRImmediate, (XtPointer) DEFAULT_NUM_SESSIONS_BACKED_UP},
   {SmNignoreEnvironment, SmCignoreEnvironment, XtRString, sizeof(String),
        XtOffset(SessionResourcesPtr, ignoreEnvironment),
        XtRImmediate, (XtPointer) NULL},
#if defined(USE_XINERAMA)	/* JET - Xinerama */
   {SmNxineramaPreferredScreen, SmCxineramaPreferredScreen, XtRInt, sizeof(int),
        XtOffset(SessionResourcesPtr, xineramaPreferredScreen),
        XtRImmediate, (XtPointer) 0},
#endif

}
;


static XtResource saverResources[]=
{
   {SmNcycleTimeout, SmCcycleTimeout, XtRInt, sizeof(int),
        XtOffset(SaverResourcesPtr, cycleTimeout),
        XtRImmediate, (XtPointer) -1},
   {SmNlockTimeout, SmClockTimeout, XtRInt, sizeof(int),
        XtOffset(SaverResourcesPtr, lockTimeout),
        XtRImmediate, (XtPointer) -1},
   {SmNsaverTimeout, SmCsaverTimeout, XtRInt, sizeof(int),
        XtOffset(SaverResourcesPtr, saverTimeout),
        XtRImmediate, (XtPointer) -1},
   {SmNrandom, SmCrandom, XtRBoolean, sizeof(Boolean),
        XtOffset(SaverResourcesPtr, random),
        XtRImmediate, (XtPointer) False},
   {SmNsaverList, SmCsaverList, XtRString, sizeof(String),
        XtOffset(SaverResourcesPtr, saverList),
        XtRImmediate, (XtPointer) ""},

}
;



/*
 * Machine specific defaults.
 */
 static struct 
 {
   int machineType;
   int clientTimeout;
   int wmTimeout;
 }
 machineDefault[] =
 {
   {  0,  20,  60},               /* Machine independent default */
#ifdef __hpux
   {300,  10,  60},               /* HP s300                     */
   {400,  10,  60},               /* HP s400                     */
   {600,   5,  60},               /* HP s600                     */
   {700,   5,  60},               /* HP s700                     */
   {800,   5,  60},               /* HP s800                     */
#endif /* __hpux */
 };
 #define MACHINEDEFAULTS (sizeof(machineDefault) / sizeof(machineDefault[0]))


/*************************************<->*************************************
 *
 *  _SmWaitClientTimeoutDefault (widget, offset, value)
 *
 *
 *  Description:
 *  -----------
 *  This function generates a default value for the waitClientTimeout resource.
 *  We dynamically default to 10 seconds for s400/s300 and to
 *  5 seconds for s700/s800.
 *
 *  Inputs:
 *  ------
 *  widget = this is not used
 *
 *  offset = this is the resource offset
 *
 *  value = this is a pointer to a XrmValue in which to store the result
 *
 *  Outputs:
 *  -------
 *  value = default resource value and size
 *
 *************************************<->***********************************/
static
void
_SmWaitClientTimeoutDefault (Widget widget, int offset, XrmValue *value)
{
    int i;

    for (i = 0; i < MACHINEDEFAULTS; i++)
    {
      if (machineDefault[i].machineType == machineType)
      {
        break;
      }
    }

    if (i == MACHINEDEFAULTS)
    {
      i = 0;
    }

    value->addr = (char *)&machineDefault[i].clientTimeout;
    value->size = sizeof (int);

} /* END OF FUNCTION _SmWaitClientTimeoutDefault */



/*************************************<->*************************************
 *
 *  _SmWaitWmTimeoutDefault (widget, offset, value)
 *
 *
 *  Description:
 *  -----------
 *  This function generates a default value for the waitWmTimeout resource.
 *  We dynamically default to 60 seconds for s400/s300 and to
 *  60 seconds for s700/s800.  This could change if we get feedback indicating
 *  the need for a new default.
 *
 *  Inputs:
 *  ------
 *  widget = this is not used
 *
 *  offset = this is the resource offset
 *
 *  value = this is a pointer to a XrmValue in which to store the result
 *
 *  Outputs:
 *  -------
 *  value = default resource value and size
 *
 *************************************<->***********************************/
static
void
_SmWaitWmTimeoutDefault (Widget widget, int offset, XrmValue *value)
{
    int i;
    
    smGD.userSetWaitWmTimeout = False; /* if we are here, it is not user set */

    for (i = 0; i < MACHINEDEFAULTS; i++)
    {
      if (machineDefault[i].machineType == machineType)
      {
        break;
      }
    }

    if (i == MACHINEDEFAULTS)
    {
      i = 0;
    }

    value->addr = (char *)&machineDefault[i].wmTimeout;
    value->size = sizeof (int);

} /* END OF FUNCTION _SmWaitWmTimeoutDefault */



/*************************************<->*************************************
 *
 *  InitSMGlobals ()
 *
 *
 *  Description:
 *  -----------
 *  Sets SM global resources and global settings to a starting value.
 *
 *
 *  Inputs:
 *  ------
 *  buttonForm = form widget for button that allows cursor to get colors
 *  smRes(global) = structure that holds session resources.
 *  smToSet(global) = structure that holds "which settings to set and how" info
 *  smGD(global) = structure that holds general data info
 * 
 *  Outputs:
 *  -------
 *  smRes(global) = structure that holds session resources.
 *  smToSet(global) = structure that holds "which settings to set and how" info
 *  smGD(global) = structure that holds general data info
 *
 *  Comments:
 *  --------
 *  Resources are set to an initial value by the resource manager.  The
 *  rest are set in the routine.
 * 
 *************************************<->***********************************/
void 
InitSMGlobals( void )
{
    int i;
    PropDtSmWindowInfo property;
    struct utsname nameRec;
    char *firstSlash;
    char *keyNum;

    smGD.userSetWaitWmTimeout = True; /* assume it is */

#ifdef __hpux
    if (uname(&nameRec) == 0)
    {
        keyNum = nameRec.machine;
        if (firstSlash = strchr(keyNum, '/'))
        {
            keyNum = ++firstSlash;

            if (     keyNum[0] == '3') 
            {
                machineType = 300;
            }
            else if (keyNum[0] == '4') 
            {
                machineType = 400;
            }
            else if (keyNum[0] == '6') 
            {
                machineType = 600;
            }
            else if (keyNum[0] == '7') 
            {
                machineType = 700;
            }
            else if (keyNum[0] == '8') 
            {
                machineType = 800;
            }
        }
    }
#endif /* __hpux */

    /*
     * Get application specific resource values
     */
    XtAppAddConverter (smGD.appCon, XtRString, SmRContManagement,
                       (XtConverter)SmCvtStringToContManagement, NULL, 0);
    XtGetApplicationResources(smGD.topLevelWid, (XtPointer) &smRes,
                              sessionResources,
                              XtNumber(sessionResources), NULL, 0);

    if (smGD.lockOnTimeoutStatus == True)
    {
     /*
      * Pull screen saver resources from Dtsession*extension.<name>.
      */
      smGD.SmNextension = "extension";
      smGD.SmCextension = "Extension";
      smGD.extensionSpec = "extension.";
    }
    else
    {
     /*
      * Pull screen saver resources from Dtsession*<name>.
      */
      smGD.SmNextension = smGD.SmCextension = smGD.extensionSpec = "";
    }

    XtGetSubresources(smGD.topLevelWid, (XtPointer) &smSaverRes,
                      smGD.SmNextension, smGD.SmCextension,
                      saverResources,
                      XtNumber(saverResources), NULL, 0);

    smGD.wmStartup = SmNewString(smRes.wmStartup);
    smGD.keyholders = SmNewString(smRes.keyholders);
    smGD.sessionLang = SmNewString(smRes.sessionLang);
    smGD.saverList = SmNewString(smSaverRes.saverList);

    /*
     * Initialize general data used by apps not initialized by
     * XtInitialize or DtInitialize
     */
    smGD.topLevelWindow = XtWindow(smGD.topLevelWid);
    smGD.numSavedScreens = (ScreenCount(smGD.display) > MAX_SCREENS_SAVED)
         ? MAX_SCREENS_SAVED : ScreenCount(smGD.display);
    smGD.dtwmRunning = False;
    smSettings.confirmMode = DtSM_VERBOSE_MODE;
    if (smGD.sessionType == CURRENT_SESSION ||
	smGD.sessionType == DEFAULT_SESSION)
        smSettings.startState = DtSM_CURRENT_STATE;
    else
        smSettings.startState = DtSM_HOME_STATE;
    smGD.homeSave = False;
    smGD.saverListParse = NULL;

    smGD.loggingOut = False;

    /*
     * Sanity check on timeouts for negative numbers
     */
    if (smRes.waitClientTimeout < 0)
    {
        smRes.waitClientTimeout = -smRes.waitClientTimeout;
    }
    if (smRes.waitWmTimeout < 0)
    {
        smRes.waitWmTimeout = -smRes.waitWmTimeout;
    }
    if (smRes.saveYourselfTimeout < 0)
    {
        smRes.saveYourselfTimeout = -smRes.saveYourselfTimeout;
    }


    /*
     * Now convert users view of seconds in to XtAppAddTimout's
     * need for milliseconds.
     */
    smRes.waitClientTimeout = 1000 * smRes.waitClientTimeout;
    smRes.waitWmTimeout = 1000 * smRes.waitWmTimeout;
    smRes.saveYourselfTimeout = 1000 * smRes.saveYourselfTimeout;
    smGD.savedWaitWmTimeout = smRes.waitWmTimeout;

    /*
     * Initialize lock data
     */
    smGD.screen = XDefaultScreen(smGD.display);
    smGD.blackPixel = XBlackPixel(smGD.display, smGD.screen);
    smGD.whitePixel = XWhitePixel(smGD.display, smGD.screen);
    smDD.lockDialog = NULL;
    smDD.lockCoverDialog = NULL;
    for(i = 0;i < smGD.numSavedScreens;i++)
    {
        smDD.coverDialog[i] = NULL;
        smDD.coverDrawing[i] = NULL;
    }
    smGD.lockedState = UNLOCKED;

   /*
    * Sanity check screen saver resource values.
    */ 
    if (smRes.alarmTime < 0) smRes.alarmTime = 0;

    #define SMBOUND(A) (A < 0 ? 0 : A)

    smSaverRes.lockTimeout = SMBOUND(smSaverRes.lockTimeout) * 60;
    smSaverRes.saverTimeout = SMBOUND(smSaverRes.saverTimeout) * 60;
    smSaverRes.cycleTimeout = SMBOUND(smSaverRes.cycleTimeout) * 60;

    CreateLockCursor();
    smGD.waitCursor = _DtGetHourGlassCursor(smGD.display);

    /*
     * Initialize other global data related to dialogs
     */
    smDD.confExit = NULL;
    smDD.qExit = NULL;
    smDD.compatExit = NULL;
    smDD.deadWid = NULL;
    smDD.saveSession = NULL;         	/* Error dialog for Save_Session */

    if (!smDD.smHelpDialog)
	/*
	 * Don't wipe it out if it is already created
	 */
        smDD.smHelpDialog = NULL;

    /*
     * Intern all the atoms needed for the WSM communication
     */
    {
      enum { XA_DT_SM_WINDOW_INFO, XA_DT_SM_WM_PROTOCOL,
	     XA_DT_SM_START_ACK_WINDOWS, XA_DT_SM_STOP_ACK_WINDOWS,
	     XA_DT_WM_WINDOW_ACK, XA_DT_WM_EXIT_SESSION,
	     XA_DT_WM_LOCK_DISPLAY, XA_DT_WM_READY, NUM_ATOMS };
      static char *atom_names[] = {
	     _XA_DT_SM_WINDOW_INFO, _XA_DT_SM_WM_PROTOCOL,
	     _XA_DT_SM_START_ACK_WINDOWS, _XA_DT_SM_STOP_ACK_WINDOWS,
	     _XA_DT_WM_WINDOW_ACK, _XA_DT_WM_EXIT_SESSION,
	     _XA_DT_WM_LOCK_DISPLAY, _XA_DT_WM_READY };

      Atom atoms[XtNumber(atom_names)];

      XInternAtoms(smGD.display, atom_names, XtNumber(atom_names),
		   False, atoms);

      XaVsmInfo = atoms[XA_DT_SM_WINDOW_INFO];
      XaSmWmProtocol = atoms[XA_DT_SM_WM_PROTOCOL];
      XaSmStartAckWindow = atoms[XA_DT_SM_START_ACK_WINDOWS];
      XaSmStopAckWindow = atoms[XA_DT_SM_STOP_ACK_WINDOWS];
      XaWmWindowAck = atoms[XA_DT_WM_WINDOW_ACK];
      XaWmExitSession = atoms[XA_DT_WM_EXIT_SESSION];
      XaWmLockDisplay = atoms[XA_DT_WM_LOCK_DISPLAY];
      XaWmReady = atoms[XA_DT_WM_READY];
    }

    /*
     * Set the session manager window property on the root window
     */
    property.flags = 0;
    property.smWindow = (unsigned long) smGD.topLevelWindow;
    XChangeProperty (smGD.display, RootWindow(smGD.display, 0),
                     XaVsmInfo, XaVsmInfo,
                     32, PropModeReplace, (unsigned char *)&property,
                     PROP_DT_SM_WINDOW_INFO_ELEMENTS);

    /*
     * Set up the signal handler for forking and execing
     */
    sigaction(SIGCHLD, &smGD.childvec, (struct sigaction *) NULL);
}



/*************************************<->*************************************
 *
 *  SetRestorePath ()
 *
 *
 *  Description:
 *  -----------
 *  Sets SM global resources and global settings to a starting value.
 *
 *
 *  Inputs:
 *  ------
 *  smGD.display = display structure for session manager.  Used to construct
 *                     a display directory
 * 
 *  Outputs:
 *  -------
 *  smGD.resourcePath(global) = Path where the resources to be restored are
 *                              held
 *  smGD.settingPath(global) = Path where the settings to be restored are
 *                              held
 *  smGD.clientPath(global) = Path where the clients to be restored are
 *                              held
 *  smGD.savePath(global) = Path where all save files are to be saved
 *
 *  Return:
 *  ------
 *  Display connection
 *
 *  Comments:
 *  --------
 *  WARNING:  This routine also determines whether dtsession is being started
 *            in compatibility mode.  If so - no restore paths are set up and
 *            the routine is exited.
 * 
 *************************************<->***********************************/
int
SetRestorePath(
        unsigned int argc,
        char *argv[] )
{
    Display  *tmpDisplay;
    int 	i;
    char        *displayName = NULL;
    char	*session_option = NULL;

    smGD.compatMode = False;

    for(i = 0;i < argc;i++)
    {
        if(!strcmp(argv[i], "-display"))
        {
            displayName = argv[i + 1];
            /* 
             * If -display is used but DISPLAY is not set,
             * put DISPLAY into the environment
             */
            if (getenv("DISPLAY") == 0)
            {
                snprintf(tmpDisplayName, MAXPATHLEN, "DISPLAY=%s", displayName);
                putenv(tmpDisplayName);
            }
        }

        if(!strcmp(argv[i], "-norestore"))
        {
            smGD.compatMode = True;
        }

        if(!strcmp(argv[i], "-session"))
        {
	    i++;
	    if (i >= argc) 
	    {
		char		*pch;

		pch = strdup ((char *) GETMESSAGE (40, 15,
			" No session name was provided for the -session command line option."));
	      	if (pch) 
		{ 
		    DtMsgLogMessage (argv[0], DtMsgLogWarning, pch);
		    free (pch);
		}
	        break;
	    }
            session_option = argv[i];
        }
    }

    /*
     *  If we are in compatibility mode - no restore paths are set
     *  up and we just return
     */
    if(smGD.compatMode == True)
    {
	smGD.clientPath[0] = 0;
	smGD.resourcePath[0] = 0;
	smGD.settingPath[0] = 0;
	smGD.sessionType = DEFAULT_SESSION;
	smGD.restoreSession = NULL;
        return(0);
    }

    tmpDisplay = XOpenDisplay(displayName);
    if(tmpDisplay == NULL)
    {
        PrintError(DtError, GETMESSAGE(4, 1, "Invalid display name - exiting."));
        SM_EXIT(-1);
    }

    if (session_option) 
    {
        if (!InitializeSpecificSession (session_option, tmpDisplay, argc, argv))
        	InitializeGenericSession (tmpDisplay);
    }
    else 
        InitializeGenericSession (tmpDisplay);

    /*
     * Need to know if the session is for a specific display
     */
    smGD.displaySpecific = True;
    if (session_option = strrchr (smGD.savePath, '/')) 
    {
	session_option++;
	if (!strcmp (session_option, DtSM_SESSION_DIRECTORY))
            smGD.displaySpecific = False;
    }

    TrimErrorlog();
    XCloseDisplay(tmpDisplay);

    /* 
    ** User's session startup script:
    **   $HOME/.dt/sessions/sessionetc
    */
    strcpy(smGD.etcPath, smGD.savePath);
    strcat(smGD.etcPath, "/");
    strcat(smGD.etcPath, smEtcFile);

    /* 
    ** User's session shutdown script: 
    **   $HOME/.dt/sessions/sessionexit
    */
    strcpy(smGD.exitPath, smGD.savePath);
    strcat(smGD.exitPath, "/");
    strcat(smGD.exitPath, smExitFile);

    return(0);
}

void
FixPath
    (
    char * the1stPath
    )
{
    char * tempPath;
    char * pathList = (char *)XtMalloc(strlen(SM_SYSTEM_PATH) +
			 strlen(":" CDE_INSTALLATION_TOP "/config") + 1);

    strcpy(pathList,SM_SYSTEM_PATH);
    strcat(pathList,":" CDE_INSTALLATION_TOP "/config");

    tempPath = _DtQualifyWithFirst(the1stPath,pathList);
    if (tempPath != NULL) {
        strcpy(the1stPath,tempPath);
        free(tempPath);
    }

    XtFree(pathList);
}


/*************************************<->*************************************
 *
 *  SetSysDefaults ()
 *
 *
 *  Description:
 *  -----------
 *  Sets the path to restore the system default files. A convenience routine
 *
 *
 *  Inputs:
 *  ------
 *  smGD.savePath = path that files are to be saved in (set up in
 *                      SetRestorePaths)
 * 
 *  Outputs:
 *  -------
 *  smGD.resourcePath(global) = Path where the resources to be saved are
 *                              to be saved.
 *  smGD.settingPath(global) = Path where the settings to be saved are
 *                              to be saved.
 *  smGD.clientPath(global) = Path where the clients to be saved are
 *                              to be saved.
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
static int 
SetSysDefaults( void )
{
    int status;
    struct stat buf;
    String tmpString;
    char        *langSpec;
    char        *tempPath;

    /*
     * No files exist for restoration - use the
     * system defaults
     */
    strcpy(smGD.resourcePath, "");
    strcpy(smGD.clientPath, "");
    smGD.settingPath[0] = 0;
    smGD.sessionType = DEFAULT_SESSION;
    smGD.restoreSession = (char *) SM_SYSTEM_DIRECTORY;

    langSpec = getenv("LANG");
    if ((langSpec != NULL) && (*langSpec != 0))
    {
        strcat(smGD.clientPath, "/");
        strncat(smGD.clientPath, langSpec, MAXPATHLEN-2);
        smGD.clientPath[MAXPATHLEN-1] = 0;
    }
    
    strcat(smGD.clientPath, "/");
    strcat(smGD.clientPath, SM_SYSTEM_CLIENT_FILE);
   
    FixPath(smGD.clientPath);

    /*
     * If the system files don't exist - we're in
     * trouble - First try LANG location then default
     */
    status = stat(smGD.clientPath, &buf);
    if(status == -1)
    {
        if((langSpec == NULL) || (*langSpec == 0))
        {
            PrintErrnoError(DtError, GETMESSAGE(4, 2,
                        "No defaults files exist.  "
                        "No applications will be restarted."));
            smGD.clientPath[0] = 0;
            smGD.resourcePath[0] = 0;
        }
        else
        {
            strcpy(smGD.clientPath, "/C/");
            strcat(smGD.clientPath, SM_SYSTEM_CLIENT_FILE);

	    FixPath(smGD.clientPath);

            status = stat(smGD.clientPath, &buf);
            if(status == -1)
            {
                PrintErrnoError(DtError, GETMESSAGE(4, 3,
                            "No defaults files exist.  "
                            "No applications will be restarted."));
                smGD.clientPath[0] = 0;
                smGD.resourcePath[0] = 0;
            }
        }
    }
    
    return(0);
}



/*************************************<->*************************************
 *
 *  SetResSet ()
 *
 *
 *  Description:
 *  -----------
 *  Sets the path to restore the settings and resource files.
 *  A convenience routine
 *
 *
 *  Inputs:
 *  ------
 *  smGD.savePath = path that files are to be saved in (set up in
 *                      SetRestorePaths)
 * 
 *  Outputs:
 *  -------
 *  smGD.resourcePath(global) = Path where the resources to be saved are
 *                              to be saved.
 *  smGD.settingPath(global) = Path where the settings to be saved are
 *                              to be saved.
 *  smGD.clientPath(global) = Path where the clients to be saved are
 *                              to be saved.
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
static int 
SetResSet( void )
{
    int status;
    struct stat buf;
    
    /*
     * If resource or settings file does not exist - just null out
     * the path so these things will not get restored
     */
    status = stat(smGD.resourcePath, &buf);
    if(status == -1)
    {
        smGD.resourcePath[0] = 0;
    }

    status = stat(smGD.settingPath, &buf);
    if(status == -1)
    {
        smGD.settingPath[0] = 0;
    }
    return(0);
}


/*************************************<->*************************************
 *
 *  UndoSetSavePath () - Undoes the directory manipulations done by 
 *  	SetSavePath.  This function is only called if a shutdown/save
 *	is canceled.
 *
 *************************************<->***********************************/
void 
UndoSetSavePath ( )
{
	char			* buf;

	if (strcmp ("", savedDir)) {

		/*
		 * Remove the directory where the save occurred, e.g.:
		 *
		 *    ~/.dt/<session_dir>/current
		 *    ~/.dt/<session_dir>/home
		 */
		buf = XtMalloc (strlen (savedDir) + 9);
		sprintf (buf, "rm -rf %s", savedDir);
		SystemCmd (buf);
		XtFree (buf);

		if (strcmp ("", savedOldDir)) {

			MoveDirectory (savedOldDir, savedDir, False);

			if (strcmp ("", savedTmpDir)) {
				MoveDirectory (savedTmpDir, savedOldDir, False);
			}
		}
	}
}


/*************************************<->*************************************
 *
 *  SetSavePath (saveToHome, mode)
 *
 *
 *  Description:
 *  -----------
 *  Sets up paths for files that need to be saved.  Also removes any files
 *  that shouldn't be there after the save.
 *
 *
 *  Inputs:
 *  ------
 *  smGD.display = display structure for session manager.  Used to construct
 *                     a display directory
 *  saveToHome = whether this is a save to home session or not
 *  mode = whether we are resetting or restarting
 * 
 *  Outputs:
 *  -------
 *  smGD.resourcePath(global) = Path where the resources to be saved are
 *                              to be saved.
 *  smGD.settingPath(global) = Path where the settings to be saved are
 *                              to be saved.
 *  smGD.clientPath(global) = Path where the clients to be saved are
 *                              to be saved.
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
int 
SetSavePath(
        Boolean saveToHome,
        int mode )
{
    struct stat         buf;
    int                 status;

    /*
     * These directory paths are needed in UndoSetSavePaths
     * if a shutdown/save is canceled.
     */
    strcpy (savedDir, "");
    strcpy (savedOldDir, "");
    strcpy (savedTmpDir, "");
    
    /*
     * Make sure the user hasn't done something like delete the .dt
     * directories during the session.  If so - recreate them
     */
    SM_FREE(smGD.savePath);
    smGD.savePath = _DtCreateDtDirs(smGD.display);
    if(smGD.savePath == NULL)
    {
        PrintErrnoError(DtError, smNLS.cantCreateDirsString);
        smGD.clientPath[0] = 0;
        smGD.settingPath[0] = 0;
        smGD.resourcePath[0] = 0;
        return(-1);
    }
        
    
    /*
     * Path for a save defaults to save the current session.
     * Otherwise just remove the directories
     */
    strcpy(smGD.clientPath, smGD.savePath);
    strcpy(smGD.settingPath, smGD.savePath);
    strcpy(smGD.resourcePath, smGD.savePath);
    strcat(smGD.clientPath, "/");
    strcat(smGD.settingPath, "/");
    strcat(smGD.resourcePath, "/");

    if(saveToHome == False)
    {
        strcat(smGD.clientPath, SM_CURRENT_DIRECTORY);
        strcat(smGD.resourcePath, SM_CURRENT_DIRECTORY);
        strcat(smGD.settingPath, SM_CURRENT_DIRECTORY);
	smGD.restoreSession = SM_CURRENT_DIRECTORY;
    }
    else
    {
        strcat(smGD.clientPath, SM_HOME_DIRECTORY);
        strcat(smGD.resourcePath, SM_HOME_DIRECTORY);
        strcat(smGD.settingPath, SM_HOME_DIRECTORY);
	smGD.restoreSession = SM_HOME_DIRECTORY;
    }

    if ((mode == DtSM_HOME_STATE) && (saveToHome == False) && 
       (smSettings.startState == DtSM_HOME_STATE))
    {
	/* 
	 * The only time this should should be true is if the
	 * current session is a Home session and the session
	 * is being exited.  The idea is that if a "current"
	 * directory exits, it must be moved because when the
	 * user next logs in, a Home session should be started
	 * (if a session is not selected at the login screen)
	 * and the Session Manager decides whether or not it
	 * starts a Home or Current session based on if a
	 * "current" directory exists.  If a "current" directory
	 * exists, a Current session will be started.
	 */
        status = stat(smGD.clientPath, &buf);
        if(status != -1)
        {
	    /*
	     * The "current" directory exists and must be moved to its
	     * ".old" version.  But first, if the ".old" version exists,
	     * it must be moved to a temporary directory.  This temporary
	     * directory will eventually be pruned - when a user next
	     * runs a Current session and saves the session.
	     */
	    strcpy (savedDir, smGD.clientPath);
            sprintf(smGD.etcPath, "%s.%s", smGD.clientPath, SM_OLD_EXTENSION);
            status = stat(smGD.etcPath, &buf);
            if(status == 0)
	    {
		char 		* tmpName;
                int len, tfd;

                strcpy(savedOldDir, smGD.etcPath);

                len = strlen(smGD.savePath) + strlen(smGD.restoreSession) 
                  + strlen("XXXXXX") + 3;
		tmpName = (char *) XtCalloc(1, len);

		sprintf(tmpName, "%s/%s.XXXXXX", smGD.savePath, 
                        smGD.restoreSession);

                if ((tfd = mkstemp(tmpName)) == -1)
                  {
                    PrintErrnoError(DtError, smNLS.cantCreateDirsString);
                  }
                else
                  {
                    close(tfd);
                    unlink(tmpName);

                    MoveDirectory(smGD.etcPath, tmpName, False);
                    
                    strncpy(savedTmpDir, tmpName, len - 1);
                  }
                XtFree((char *) tmpName);
	    }
	    MoveDirectory(smGD.clientPath, smGD.etcPath, False);
        }

        smGD.clientPath[0] = 0;
        smGD.settingPath[0] = 0;
        smGD.resourcePath[0] = 0;
    }
    else
    {
        strcpy (savedDir, smGD.clientPath);

        /*
         * If the desired directory doesn't exist, create it.
         */
        status = stat(smGD.clientPath, &buf);
        if(status == -1)
        {
            status = mkdir(smGD.clientPath, 0000);
            if(status == -1)
            {
                PrintErrnoError(DtError, smNLS.cantCreateDirsString);
                smGD.clientPath[0] = 0;
                smGD.settingPath[0] = 0;
                smGD.resourcePath[0] = 0;
                return(-1);
            }
            chmod(smGD.clientPath, 0755);
        }
        else
        {
	    /*
	     * The desired directory already exists so it must
	     * be archived by moving it to its ".old" version. But
	     * first, if its ".old" version already exists, it must
	     * be moved to a temporary directory that will be removed 
	     * when the session directories are pruned - after the
	     * save is complete.
	     */
	    char 		* tmpName;

            sprintf(smGD.etcPath, "%s.%s", smGD.clientPath, SM_OLD_EXTENSION);
            status = stat(smGD.etcPath, &buf);
            if(status == 0)
	    {
              int len, tfd;

              len = strlen(smGD.savePath) + strlen(smGD.restoreSession) 
                + strlen("XXXXXX") + 3;
              tmpName = (char *) XtCalloc(1, len);
              sprintf(tmpName, "%s/%s.XXXXXX", smGD.savePath, 
                      smGD.restoreSession);

              strcpy (savedOldDir, smGD.etcPath);

              if ((tfd = mkstemp(tmpName)) == -1)
                {
                  PrintErrnoError(DtError, smNLS.cantCreateDirsString);
                }
              else
                {
                  close(tfd);
                  unlink(tmpName);

                  MoveDirectory (smGD.etcPath, tmpName, False);
              
                  strcpy (savedTmpDir, tmpName);
                }
              XtFree((char *) tmpName);
	    }

            MoveDirectory(smGD.clientPath, smGD.etcPath, False);

            /*
             * Now re-make the directory
             */
	       status = mkdir(smGD.clientPath, 0000);
	       if(status == -1)
	       {
                 PrintErrnoError(DtError, smNLS.cantCreateDirsString);
                 smGD.clientPath[0] = 0;
                 smGD.settingPath[0] = 0;
                 smGD.resourcePath[0] = 0;
                 return(-1);
               }
            status = chmod(smGD.clientPath, 0755);
            if(status == -1)
              {
                PrintErrnoError(DtError, smNLS.cantCreateDirsString);
                smGD.clientPath[0] = 0;
                smGD.settingPath[0] = 0;
                smGD.resourcePath[0] = 0;
                return(-1);
              }
        }
        
        strcat(smGD.clientPath, "/");
        strcat(smGD.clientPath, SM_CLIENT_FILE2);
        strcat(smGD.settingPath, "/");
        strcat(smGD.settingPath, SM_SETTING_FILE);
        strcat(smGD.resourcePath, "/");
        strcat(smGD.resourcePath, SM_RESOURCE_FILE);
    }

    return(0);
}


/*************************************<->*************************************
 *
 *  SetFontSavePath (saveToHome, mode)
 *
 *
 *  Description:
 *  -----------
 *  Sets up the save path for the auxillary directory.
 *
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
 *************************************<->***********************************/
int 
SetFontSavePath(char *langPtr)
{
    struct stat         buf;
    int                 status;
    char                *sessionSaved;
    

    /*
     * Set up the session font directory and see if it exists
     */
    if(smGD.sessionType == CURRENT_SESSION)
    {
        sessionSaved = SM_CURRENT_FONT_DIRECTORY;
    }
    else
    {
        sessionSaved = SM_HOME_FONT_DIRECTORY;
    }

    snprintf(smGD.fontPath, MAXPATHLEN, "%s/%s", smGD.savePath, sessionSaved);
    status = stat(smGD.fontPath, &buf);
    if(status == -1)
    {
        status = mkdir(smGD.fontPath, 0000);
        if(status == -1)
        {
            PrintErrnoError(DtError, smNLS.cantCreateDirsString);
            smGD.fontPath[0] = 0;
            return(-1);
        }
        chmod(smGD.fontPath, 0755);
    }

    /*
     * Now add the lang subdirectory and see if it exists
     */
    strncat(smGD.fontPath, "/", MAXPATHLEN);
    strncat(smGD.fontPath, langPtr, MAXPATHLEN);
    status = stat(smGD.fontPath, &buf);
    if(status == -1)
    {
        status = mkdir(smGD.fontPath, 0000);
        if(status == -1)
        {
            PrintErrnoError(DtError, smNLS.cantCreateDirsString);
            smGD.fontPath[0] = 0;
            return(-1);
        }
        status = chmod(smGD.fontPath, 0755);
        if(status == -1)
          {
            PrintErrnoError(DtError, smNLS.cantCreateDirsString);
            smGD.fontPath[0] = 0;
            return(-1);
          }
    }

    return(0);
}

                      

/*************************************<->*************************************
 *
 *  RemoveFiles()
 *
 *
 *  Description:
 *  -----------
 *  Remove the files that need to be removed
 *
 *
 *  Inputs:
 *  ------
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
RemoveFiles(
        char *path )
{
    pid_t  clientFork;
    int    execStatus, childStatus, i, statLoc;
    String tmpString;

    /*
     * Fork and exec the client process
     */
    sigaction(SIGCHLD, &smGD.defvec, (struct sigaction *) NULL);

    clientFork = vfork();
    
    /*
     * If the fork fails - Send out an error and return
     */
    if(clientFork < 0)
    {
        PrintErrnoError(DtError, smNLS.cantForkClientString);
        return;
    }
    
    /*
     * Fork succeeded - now do the exec
     */
    if(clientFork == 0)
    {
        SetSIGPIPEToDefault ();

        /*
         * Set the gid of the process back from bin
         */
#ifndef __hpux
#ifndef SVR4
        setregid(smGD.runningGID, smGD.runningGID);
#else
        setgid(smGD.runningGID);
        setegid(smGD.runningGID);
#endif
#endif

        _DtEnvControl(DT_ENV_RESTORE_PRE_DT);
        
#if defined(__osf__) || defined(CSRG_BASED)
        setsid();
#else
        (void) setpgrp();
#endif /* __osf__ */
         
        execStatus = execlp("rm","rm", "-rf", path, (char *) 0);
        if(execStatus != 0)
        {
            tmpString = ((char *)GETMESSAGE(4, 4, "Unable to remove session directory.  Make sure write permissions exist on $HOME/.dt directory.  Invalid session files will not be removed.")) ;
            PrintErrnoError(DtError, tmpString);
            SM_FREE(tmpString);
            SM_EXIT(-1);
        }
    }

    while(wait(&statLoc) != clientFork);
    
    sigaction(SIGCHLD, &smGD.childvec, (struct sigaction *) NULL);
}
                      

/*************************************<->*************************************
 *
 *  MoveDirectory()
 *
 *  Description:
 *  -----------
 *  Move the directory specified by pathFrom - to the directory specified
 *  by pathTo.
 *
 *  Inputs:
 *  ------
 *  pathFrom = the directory to move from
 *  pathTo = the directory to move to
 *  removeDestDir = if True, directory pathTo is removed before the
 * 	move occurs
 * 
 *  Outputs:
 *  -------
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
void 
MoveDirectory(
	char 		*pathFrom,
	char 		*pathTo,
	Boolean		removeDestDir)
{
    struct stat buf;
    pid_t  clientFork;
    int    status, execStatus, childStatus, i, statLoc;
    String tmpString;

    /*
     * If the pathTo directory exists - remove it
     */
    if (removeDestDir) 
    {
        status = stat(pathTo, &buf);
        if(status != -1)
        {
            RemoveFiles(pathTo);
        }
    }
           
    /*
     * Fork and exec the client process
     */
    sigaction(SIGCHLD, &smGD.defvec, (struct sigaction *) NULL);

    clientFork = vfork();
    
    /*
     * If the fork fails - Send out an error and return
     */
    if(clientFork < 0)
    {
        PrintErrnoError(DtError, smNLS.cantForkClientString);
        return;
    }
    
    /*
     * Fork succeeded - now do the exec
     */
    if(clientFork == 0)
    {
        SetSIGPIPEToDefault ();

        /*
         * Set the gid of the process back from bin
         */
#ifndef __hpux
#ifndef SVR4
        setregid(smGD.runningGID, smGD.runningGID);
#else
        setgid(smGD.runningGID);
        setegid(smGD.runningGID);
#endif
#endif

        _DtEnvControl(DT_ENV_RESTORE_PRE_DT);
        
#if defined(__osf__) || defined(CSRG_BASED)
        setsid();
#else
        (void) setpgrp();
#endif /* __osf__ */
        
        execStatus = execlp("mv","mv", pathFrom, pathTo, (char *) 0);
        if(execStatus != 0)
        {
            tmpString = ((char *)GETMESSAGE(4, 4, "Unable to remove session directory.  Make sure write permissions exist on $HOME/.dt directory.  Invalid session files will not be removed.")) ;
            PrintErrnoError(DtError, tmpString);
            SM_FREE(tmpString);
            SM_EXIT(-1);
        }
    }

    while(wait(&statLoc) != clientFork);
    
    sigaction(SIGCHLD, &smGD.childvec, (struct sigaction *) NULL);
}

                                      

/*************************************<->*************************************
 *
 *  InitNlsStrings()
 *
 *
 *  Description:
 *  -----------
 *  Initialize the NLS strings used in dtsession
 *
 *
 *  Inputs:
 *  ------
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
InitNlsStrings( void )
{
    char        *tmpString;

    /*
     * Malloc failure error message - THIS MESSAGE MUST BE INITIALIZED FIRST
     */
    smNLS.cantMallocErrorString = strdup (((char *)GETMESSAGE(4, 5, "Unable to malloc memory for operation.")));

    /*
     * Error message when dtsession cant lock the display
     */
    smNLS.cantLockErrorString = strdup (((char *)GETMESSAGE(4, 6, "Unable to lock display.  Another application may have the pointer or keyboard grabbed.")));

    /*
     * Error message when dtsession cant open files for reading or writing
     */
    smNLS.cantOpenFileString = strdup (((char *)GETMESSAGE(4, 7, "Unable to open session file.  No clients will be restarted.")));

    /*
     * Error message when dtsession cant fork client process
     */
    smNLS.cantForkClientString = strdup (((char *)GETMESSAGE(4, 8, "Unable to fork client process.")));

    /*
     * Error message when dtsession cant create dt directories
     */
    smNLS.cantCreateDirsString = strdup (((char *)GETMESSAGE(4, 9, "Unable to create DT directories.  Check permissions on home directory.")));

    /*
     * Error message when trying to lock display on trusted system
     */
    smNLS.trustedSystemErrorString = strdup (((char *)GETMESSAGE(4, 10, "Unable to lock display due to security restrictions")));
}
 
                                      

/*************************************<->*************************************
 *
 *  WaitChildDeath()
 *
 *
 *  Description:
 *  -----------
 *  When a SIGCHLD signal comes in, wait for all child processes to die.
 *
 *
 *  Inputs:
 *  ------
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
WaitChildDeath( int i )
{
  int   stat_loc;
  pid_t pid;

 /*
  * Wait on any children that have died since the last SIGCHLD we
  * received. Any child process death that occurs while we are 
  * in WaitChildDeath() will not result in a SIGCHLD. Any
  * child proceses that die after our last call to waitpid() will
  * remain zombiefied until the next invocation of WaitChildDeath().
  */
  while ((pid = waitpid(-1, &stat_loc, WNOHANG)) > 0)
  ;

  sigaction(SIGCHLD, &smGD.childvec, (struct sigaction *) NULL);
}

                                      

/*************************************<->*************************************
 *
 *  TrimErrorlog()
 *
 *
 *  Description:
 *  -----------
 *  Trim the errorlog file using the following algorithm:
 *      if(errorlog.old exists and is not empty) move it to errorlog.oldest
 *      if(errorlog exists and is not empty) move it to errorlog.old
 *
 *
 *  Inputs:
 *  ------
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
TrimErrorlog( void )
{
    char        *savePath, 	/* common path element for all pathnames */
		*checkPath1, 
		*checkPath2;
    struct stat buf;
    int len, status;
    char *home;

    /*
     * Allocate the strings needed
     */
    savePath = (char *) SM_MALLOC(MAXPATHLEN + 1);
    checkPath1 = (char *) SM_MALLOC(MAXPATHLEN + 1);
    checkPath2 = (char *) SM_MALLOC(MAXPATHLEN + 1);
    if ((home=getenv("HOME")) == NULL)
        home="";
    
    len = strlen(home) + strlen(DtPERSONAL_CONFIG_DIRECTORY) + 2;
    if (len > MAXPATHLEN) savePath = SM_REALLOC(savePath, len);
    snprintf(savePath, len, "%s/%s", home, DtPERSONAL_CONFIG_DIRECTORY);
    
    /*
     * If errorlog.old exists and it is not empty, delete
     * errorlog.older and then move errolog.old to 
     * errorlog.older
     */
    if (len + strlen(DtOLDER_ERRORLOG_FILE) > MAXPATHLEN)
      checkPath1 = SM_REALLOC(savePath, len + strlen(DtOLDER_ERRORLOG_FILE));
    sprintf(checkPath1, "%s/%s", savePath, DtOLDER_ERRORLOG_FILE);

    if (len + strlen(DtOLD_ERRORLOG_FILE) > MAXPATHLEN)
      checkPath2 = SM_REALLOC(savePath, len + strlen(DtOLD_ERRORLOG_FILE));
    sprintf(checkPath2, "%s/%s", savePath, DtOLD_ERRORLOG_FILE);

    status = stat(checkPath2, &buf);
    if((status != -1) && (buf.st_size > 0))
    {
	(void) unlink(checkPath1);
        (void) link(checkPath2, checkPath1);
        (void) unlink(checkPath2);
    }

    /*
     * If errorlog exists and it is not empty, move it to
     * errorlog.old
     */
    if (len + strlen(DtERRORLOG_FILE) > MAXPATHLEN)
      checkPath1 = SM_REALLOC(savePath, len + strlen(DtERRORLOG_FILE));
    sprintf(checkPath1, "%s/%s", savePath, DtERRORLOG_FILE);

    status = stat(checkPath1, &buf);
    if((status != -1) && (buf.st_size > 0))
    {
        (void) link(checkPath1, checkPath2);
        (void) unlink(checkPath1);
    }

    SM_FREE((char *) savePath);
    SM_FREE((char *) checkPath1);
    SM_FREE((char *) checkPath2);

    return;
}

                                      

/*************************************<->*************************************
 *
 *  SetSystemReady()
 *
 *
 *  Description:
 *  -----------
 *  Do everything that needs to be done to get the system back into a
 *  READY state.  This includes checking to be sure that the BMS did not
 *  die while we were in process
 *
 *
 *  Inputs:
 *  ------
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
SetSystemReady( void )
{
    smGD.smState = READY;
    if(smGD.bmsDead == True)
    {
        WarnMsgFailure();
    }

    return;
}


/*************************************<->*************************************
 *
 *  SmCvtStringToContManagement (args, numArgs, fromVal, toVal)
 *
 *
 *  Description:
 *  -----------
 *  This function converts a string to a value for the 
 *  contention management flag set.
 *
 *
 *  Inputs:
 *  ------
 *  args = additional XrmValue arguments to the converter - NULL here
 *
 *  numArgs = number of XrmValue arguments - 0 here
 *
 *  fromVal = resource value to convert
 *
 * 
 *  Outputs:
 *  -------
 *  toVal = descriptor to use to return converted value
 *
 *************************************<->***********************************/

void SmCvtStringToContManagement (XrmValue *args, Cardinal numArgs, XrmValue *fromVal, XrmValue *toVal)
{

    unsigned char       *pch = (unsigned char *) (fromVal->addr);
    unsigned char       *pchNext;
    int         len;
    static long cval;
    Boolean     fHit = False;
    Boolean    fAddNext = True;

/*
 * Names of contention management options
 */

#define CM_ALL_STR              (unsigned char *)"all"
#define CM_SYSTEM_STR           (unsigned char *)"system"
#define CM_HANDSHAKE_STR        (unsigned char *)"handshake"
#define CM_NONE_STR             (unsigned char *)"none"

    /*
     * Check first token. If '-' we subtract from all options.
     * Otherwise, we start with no contention management and add things in.
     */

    if (*pch &&
        (_DtNextToken (pch, &len, &pchNext)) &&
        (*pch == '-'))
    {
        cval = SM_CM_ALL;
        fHit = True;
    }
    else
    {
        cval = SM_CM_NONE;
    }

    while (*pch && _DtNextToken (pch, &len, &pchNext))
    {
           /*
            * Strip off "sign" if prepended to another token, and process
            * that token the next time through.
            */

        if (*pch == '+')
        {
            if (len != 1)
            {
                pchNext = pch + 1;
            }
            fAddNext = TRUE;
        }

        else if (*pch == '-')
        {
            if (len != 1)
            {
                pchNext = pch + 1;
            }
            fAddNext = FALSE;
        }

        if ((*pch == 'A') || (*pch == 'a'))
        {
            if (_DtWmStringsAreEqual (pch, CM_ALL_STR, len))
            {
                cval = fAddNext ? (cval | SM_CM_ALL) :
                                  (cval & ~SM_CM_ALL);
                fHit = True;
            }
        }

        else if ((*pch == 'S') || (*pch == 's'))
        {
            if (_DtWmStringsAreEqual (pch, CM_SYSTEM_STR, len))
            {
                cval = fAddNext ? (cval | SM_CM_SYSTEM) :
                                  (cval & ~SM_CM_SYSTEM);
                fHit = True;
            }
        }

        else if ((*pch == 'H') || (*pch == 'h'))
        {
            if (_DtWmStringsAreEqual (pch, CM_HANDSHAKE_STR, len))
            {
                cval = fAddNext ? (cval | SM_CM_HANDSHAKE) :
                                  (cval & ~SM_CM_HANDSHAKE);
                fHit = True;
            }
        }

        else if ((*pch == 'N') || (*pch == 'n'))
        {
            if (_DtWmStringsAreEqual (pch, CM_NONE_STR, len))
            {
                /* don't bother adding or subtracting nothing */
                fHit = True;
            }
        }

        pch = pchNext;
    }



    /*
     * If we didn't match anything then set to default.
     */
    if (!fHit)
    {
        cval =  SM_CM_DEFAULT;
    }


    (*toVal).size = sizeof (long);
    (*toVal).addr = (caddr_t) &cval;


} /* END OF FUNCTION SmCvtStringToContManagement */


/*************************************<->*************************************
 *
 *  _DtNextToken (pchIn, pLen, ppchNext)
 *
 *
 *  Description:
 *  -----------
 *  XXDescription ...
 *
 *
 *  Inputs:
 *  ------
 *  pchIn = pointer to start of next token
 *
 * 
 *  Outputs:
 *  -------
 *  pLen  =    pointer to integer containing number of characters in next token
 *  ppchNext = address of pointer to following token
 *
 *  Return =   next token or NULL
 *
 *
 *  Comments:
 *  --------
 *  None.
 * 
 *************************************<->***********************************/

unsigned char *_DtNextToken (unsigned char *pchIn, int *pLen, 
        unsigned char **ppchNext)
{
    unsigned char *pchR = pchIn;
    register int   i;

#ifdef MULTIBYTE
    register int   chlen;

    for (i = 0; ((chlen = mblen ((char *)pchIn, MB_CUR_MAX)) > 0); i++)
    /* find end of word: requires singlebyte whitespace terminator */
    {
        if ((chlen == 1) && isspace (*pchIn))
        {
            break;
        }
        pchIn += chlen;
    }

#else
    for (i = 0; *pchIn && !isspace (*pchIn); i++, pchIn++)
    /* find end of word */
    {
    }
#endif

    /* skip to next word */
    ScanWhitespace (&pchIn);

    *ppchNext = pchIn;
    *pLen = i;
    if (i)
    {
        return(pchR);
    }
    else
    {
       return(NULL);
    }

} /* END OF FUNCTION _DtNextToken */   



/*************************************<->*************************************
 *
 *  _DtWmStringsAreEqual (pch1, pch2, len)
 *
 *
 *  Description:
 *  -----------
 *  XXDescription ...
 *
 *
 *  Inputs:
 *  ------
 *  pch1 =
 *  pch2 =
 *  len  =
 *
 * 
 *  Outputs:
 *  -------
 *  Return = (Boolean) True iff strings match (case insensitive)
 *
 *
 *  Comments:
 *  --------
 *  None.
 * 
 *************************************<->***********************************/

Boolean _DtWmStringsAreEqual (unsigned char *pch1, unsigned char *pch2, int len)
{
#ifdef MULTIBYTE
    int       chlen1;
    int       chlen2;
    wchar_t   wch1;
    wchar_t   wch2;

    while (len  && 
           ((chlen1 = mbtowc (&wch1, (char *) pch1, 2)) > 0) &&
           ((chlen2 = mbtowc (&wch2, (char *) pch2, 2)) == chlen1) )
    {
        if (chlen1 == 1)
        /* singlebyte characters -- make case insensitive */
        {
            if ((isupper (*pch1) ? tolower(*pch1) : *pch1) !=
                (isupper (*pch2) ? tolower(*pch2) : *pch2))
            {
                break;
            }
        }
        else
        /* multibyte characters */
        {
            if (wch1 != wch2)
            {
                break;
            }
        }
        pch1 += chlen1;
        pch2 += chlen2;
        len--;
    }

#else
    while (len  && *pch1 && *pch2 &&
           ((isupper (*pch1) ? tolower(*pch1) : *pch1) ==
            (isupper (*pch2) ? tolower(*pch2) : *pch2)))
    {
        *pch1++;
        *pch2++;
        len--;
    }
#endif

    return (len == 0);

} /* END OF _DtWmStringsAreEqual */   

/*************************************<->*************************************
 *
 *  InitializeGenericSession - set the smGD paths using the CDE 1.0
 *       session starting algorithm.
 *
 *  Description:
 *  -----------
 *  Initializes several variables in smGD
 *
 *  Inputs:
 *  ------
 *  tmpDisplay
 * 
 *  Outputs:
 *  -------
 *  Return = void
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
static void
InitializeGenericSession (
	Display		*tmpDisplay)
{
    struct 	stat buf;
    int 	session_needed = 1;
    int 	status;
    char	tmpPath[MAXPATHLEN];

    smGD.savePath = _DtCreateDtDirs(tmpDisplay);

    if (session_needed)
    {
      strcpy(smGD.clientPath, smGD.savePath);
      strcat(smGD.clientPath, "/");
      strcat(smGD.clientPath, SM_CURRENT_DIRECTORY);
      strcat(smGD.clientPath, "/");
      strcpy (tmpPath, smGD.clientPath);
      strcat(smGD.clientPath, SM_CLIENT_FILE2);

      status = stat(smGD.clientPath, &buf);

      if (status != 0) {
	 strcpy (smGD.clientPath, tmpPath);
	 strcat (smGD.clientPath, SM_CLIENT_FILE);
         status = stat(smGD.clientPath, &buf);
      }

      if(status == 0)
      {
       /*
        * sessions/current/dt.settings exist, restore to 'current' session
        */
        strcpy(smGD.resourcePath, smGD.savePath);
        strcat(smGD.resourcePath, "/");
        strcat(smGD.resourcePath, SM_CURRENT_DIRECTORY);
	strcat(smGD.resourcePath, "/");
	strcat(smGD.resourcePath, SM_RESOURCE_FILE);
        strcpy(smGD.settingPath, smGD.savePath);
        strcat(smGD.settingPath, "/");
        strcat(smGD.settingPath, SM_CURRENT_DIRECTORY);
        strcat(smGD.settingPath, "/");
        strcat(smGD.settingPath, SM_SETTING_FILE);
        SetResSet();
        smGD.sessionType = CURRENT_SESSION;
        smGD.restoreSession = (char *) SM_CURRENT_DIRECTORY;
        session_needed = 0;
      }
    }

    if (session_needed)
    {
      strcpy(smGD.clientPath, smGD.savePath);
      strcat(smGD.clientPath, "/");
      strcat(smGD.clientPath, SM_HOME_DIRECTORY);
      strcat(smGD.clientPath, "/");
      strcpy(tmpPath, smGD.clientPath);
      strcat(smGD.clientPath, SM_CLIENT_FILE2);

      status = stat(smGD.clientPath, &buf);

      if (status != 0) {
	 strcpy (smGD.clientPath, tmpPath);
	 strcat (smGD.clientPath, SM_CLIENT_FILE);
         status = stat(smGD.clientPath, &buf);
      }

      if(status == 0)
      {
       /*
        * sessions/home/dt.settings exist, restore to 'home' session
        */
        strcpy(smGD.resourcePath, smGD.savePath);
        strcat(smGD.resourcePath, "/");
        strcat(smGD.resourcePath, SM_HOME_DIRECTORY);
	strcat(smGD.resourcePath, "/");
	strcat(smGD.resourcePath, SM_RESOURCE_FILE);
        strcpy(smGD.settingPath, smGD.savePath);
        strcat(smGD.settingPath, "/");
        strcat(smGD.settingPath, SM_HOME_DIRECTORY);
        strcat(smGD.settingPath, "/");
        strcat(smGD.settingPath, SM_SETTING_FILE);
        SetResSet();
        smGD.sessionType = HOME_SESSION;
        smGD.restoreSession = (char *) SM_HOME_DIRECTORY;
        session_needed = 0;
      }
    }

    if (session_needed)
    {
      SetSysDefaults();
    }
}

/*************************************<->*************************************
 *
 *  InternSessionDir - intern the session dir property and put it
 *     on the root window
 *
 *     NOTE: - also checks the validity of the -session command line
 *	   option
 *
 *  Inputs:
 *  ------
 *  session_option - as given in the command line
 *  display
 * 
 *  Outputs:
 *  ------
 *  smGD.restoreSession - initialized
 *
 *  Return = void
 *
 *************************************<->***********************************/
static Boolean
InternSessionDir (
	char		*session_option,
	Display		*disp)
{
	char		*pch;

	if (!strcmp (session_option, SM_CURRENT_DIRECTORY)) {
		pch = DtSM_SESSION_DIRECTORY;
		smGD.restoreSession = SM_CURRENT_DIRECTORY;
	}
	else if (!strcmp (session_option, SM_HOME_DIRECTORY)) {
		pch = DtSM_SESSION_DIRECTORY;
		smGD.restoreSession = SM_HOME_DIRECTORY;
	}
	else if (!strcmp (session_option, SM_DISPLAY_HOME)) {
		pch = DtSM_SESSION_DISPLAY_DIRECTORY;
		smGD.restoreSession = SM_HOME_DIRECTORY;
	}
	else if (!strcmp (session_option, SM_DISPLAY_CURRENT)) {
		pch = DtSM_SESSION_DISPLAY_DIRECTORY;
		smGD.restoreSession = SM_CURRENT_DIRECTORY;
	}
	else
		/*
		 * The session_option is not supported 
		 */
		return (False);

	XaSmRestoreDir = XInternAtom(disp, _XA_DT_RESTORE_DIR, False);

	XChangeProperty(disp, RootWindow(disp, 0),
		XaSmRestoreDir, XA_STRING, 8, PropModeReplace,
		(unsigned char *)pch, strlen(pch));

	XFlush(disp);


	return (True);
}


/*************************************<->*************************************
 *
 *  InitializeSpecificSession - set the smGD paths based on the session
 *       name given on the command line.
 *
 *  Description:
 *  -----------
 *  Initializes several variables in smGD
 *
 *  Inputs:
 *  ------
 *  session_option
 *  tmpDisplay
 * 
 *  Outputs:
 *  -------
 *  Return = void
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
static Boolean
InitializeSpecificSession (
	char		*session_option,
	Display		*disp,
	unsigned int	argc,
	char		**argv)
{
	char		*session_dir = (char *) XtMalloc(MAXPATHLEN);
	char		*alt_dir = (char *) XtMalloc(MAXPATHLEN);
        int             len;
	struct stat 	buf;

	if (!InternSessionDir (session_option, disp))
		return (False);

	InitializePaths (smGD.restoreSession, disp);

	if (!strcmp (session_option, SM_HOME_DIRECTORY)) {
		/*
		 * Home session
		 */
		if (smGD.clientPath[0] == '\0') {
			if ((stat (smGD.savePath, &buf)) != 0) {
				if ((mkdir (smGD.savePath, 0000)) == 0)
					(void) chmod (smGD.savePath, 0755);
				else
                			PrintErrnoError(DtError, 
						smNLS.cantCreateDirsString);
			}
			SetSysDefaults ();
		}
		smGD.sessionType = HOME_SESSION;
		smGD.restoreSession = SM_HOME_DIRECTORY;
	} 
	else if (!strcmp (session_option, SM_CURRENT_DIRECTORY)) {
		/*
		 * Current session
		 */
		if (smGD.clientPath[0] == '\0') {
			/*
			 * Fallback to the generic home session
			 */
			(void) sprintf (session_dir, "%s/%s",
					smGD.savePath, SM_CURRENT_DIRECTORY);
			(void) sprintf (alt_dir, "%s/%s",
					smGD.savePath, SM_HOME_DIRECTORY);

			if (!SetAlternateSession (session_dir, alt_dir, True))
				SetSysDefaults ();
		}
		smGD.sessionType = CURRENT_SESSION;
		smGD.restoreSession = SM_CURRENT_DIRECTORY;
	}
	else if (!strcmp (session_option, SM_DISPLAY_HOME)) {
		/*
		 * Display-specific Home session
		 */
		(void) sprintf (session_dir, "%s/%s",
				smGD.savePath, SM_HOME_DIRECTORY);

		if ((stat(session_dir, &buf)) != 0){
			/*
			 * The session does not exist, give the user a
			 * change to exit.
			 */
			if (!ConfirmSessionCreation (HOME_SESSION, argc, argv))
				SM_EXIT (0);
				
		}

		if (smGD.clientPath[0] == '\0') {
			/*
			 * Fallback to the generic home session
			 */
			char		*home;

			if ((home = getenv ("HOME")) == 0)
				home = "";

                        /* JET - VU#497553 */
			len = strlen(home) + 
			  strlen(DtPERSONAL_CONFIG_DIRECTORY) +
			  strlen(DtSM_SESSION_DIRECTORY) +
			  strlen(SM_HOME_DIRECTORY);

			if (len > MAXPATHLEN)
			  alt_dir = XtRealloc(alt_dir, len + 1);

			(void) sprintf (alt_dir, "%s/%s/%s/%s",
					home,
					DtPERSONAL_CONFIG_DIRECTORY,
					DtSM_SESSION_DIRECTORY,
					SM_HOME_DIRECTORY);
			if (!SetAlternateSession (session_dir, alt_dir, True))
				SetSysDefaults ();
		}
		smGD.sessionType = HOME_SESSION;
		smGD.restoreSession = SM_HOME_DIRECTORY;
	}
	else if (!strcmp (session_option, SM_DISPLAY_CURRENT)) {
		/*
		 * Display-specific Current session
		 */
		(void) sprintf (session_dir, "%s/%s",
				smGD.savePath, SM_CURRENT_DIRECTORY);

		if ((stat(session_dir, &buf)) != 0){
			/*
			 * The session does not exist, give the user a
			 * change to exit.
			 */
			if (!ConfirmSessionCreation (CURRENT_SESSION, argc, argv))
				SM_EXIT (0);
		}

		if (smGD.clientPath[0] == '\0') {
			/*
			 * First, fallback to the display-specific home session
			 */
			char		*home;

			(void) sprintf (alt_dir, "%s/%s",
					smGD.savePath, SM_HOME_DIRECTORY);

			if (!SetAlternateSession (session_dir, alt_dir, False)){
				/*
				 * Try the generic home session
				 */
				if ((home = getenv ("HOME")) == 0)
					home = "";

                                /* JET - VU#497553 */
				len = strlen(home) + 
				  strlen(DtPERSONAL_CONFIG_DIRECTORY) +
				  strlen(DtSM_SESSION_DIRECTORY) +
				  strlen(SM_HOME_DIRECTORY);

				if (len > MAXPATHLEN)
				  alt_dir = XtRealloc(alt_dir, len + 1);

				snprintf(alt_dir, len, "%s/%s/%s/%s",
                                         home,
                                         DtPERSONAL_CONFIG_DIRECTORY,
                                         DtSM_SESSION_DIRECTORY,
                                         SM_HOME_DIRECTORY);

				if (!SetAlternateSession (session_dir, 
							  alt_dir, 
							  True))
					SetSysDefaults ();
			}
		}
		smGD.sessionType = CURRENT_SESSION;
		smGD.restoreSession = SM_CURRENT_DIRECTORY;
	}
	else {
		if (session_dir) XtFree(session_dir);
		if (alt_dir) XtFree(alt_dir);
		return (False);
	}
	
	if (session_dir) XtFree(session_dir);
	if (alt_dir) XtFree(alt_dir);
	return (True);
}


/*************************************<->*************************************
 *
 *  InitializePaths 
 *
 *  Description:
 *  -----------
 *  Initializes the following variables:
 *
 *     smGD.savePath
 *     smGD.settingsPath
 *     smGD.resourcePath
 *     smGD.clientPath - [0] = '\0' if the database files do not exist
 *
 *  Inputs:
 *  ------
 *  session_name = "current" or "home"
 * 
 *  Outputs:
 *  -------
 *  Return = void
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
static void
InitializePaths (
	char		* session_name,
	Display		* disp)
{
	char		*db_file = (char *) XtMalloc(MAXPATHLEN);
	struct stat 	buf;

        if (!db_file)
          {
            PrintError(DtError, smNLS.cantMallocErrorString);
            return;
          }

	smGD.savePath = _DtCreateDtDirs(disp);

	(void) sprintf (smGD.settingPath, "%s/%s/%s", 
			smGD.savePath, session_name, SM_SETTING_FILE);

	(void) sprintf (smGD.resourcePath, "%s/%s/%s",
			smGD.savePath, session_name, SM_RESOURCE_FILE);


	smGD.clientPath[0] = '\0';

	(void) sprintf (db_file, "%s/%s/%s", 
			smGD.savePath, session_name, SM_CLIENT_FILE2);

	if ((stat(db_file, &buf)) == 0)
		(void) strcpy (smGD.clientPath, db_file);
	else {
		(void) sprintf (db_file, "%s/%s/%s", 
				smGD.savePath, session_name, SM_CLIENT_FILE);
		if ((stat(db_file, &buf)) == 0)
			(void) strcpy (smGD.clientPath, db_file);
	}
	XtFree(db_file);
}


/*************************************<->*************************************
 *
 *  SetAlternateSession
 *
 *  Assumptions:
 *
 *     Neither of the client databases in the session_dir directory
 *     exist.
 *
 *  Inputs:
 *  ------
 *  session_dir - the directory path for the session including home
 *     or current
 *  alt_dir - the directory to use in the search for an alternate
 *     or fallback session database
 *  make_dir - if True, a session_dir will be created if it does not
 *     exits; if False, session_dir will not be created
 * 
 *  Outputs:
 *  -------
 *  Return = Boolean
 *
 *************************************<->***********************************/
static Boolean
SetAlternateSession (
	char		* session_dir,
	char		* alt_dir,
	Boolean		make_dir)
{
	char		*tmp_dir = NULL;
	char		*db_file1 = (char *) XtMalloc(MAXPATHLEN);
	char		*db_file2 = (char *) XtMalloc(MAXPATHLEN);
	struct stat 	buf;

        if (!db_file1 || !db_file2)
          {
            PrintError(DtError, smNLS.cantMallocErrorString);
            return False;
          }


	if ((stat (session_dir, &buf)) != 0) {
		/*
		 * The requested dir does not exist, create it
		 * by either copying the session from alt_dir (if
		 * it exists and has a client database) or by
		 * calling mkdir.
		 */
		
		(void) sprintf (db_file1, "%s/%s", 
				alt_dir, SM_CLIENT_FILE2);
		(void) sprintf (db_file2, "%s/%s", 
				alt_dir, SM_CLIENT_FILE);

		if ((stat(db_file1, &buf)) == 0)
			tmp_dir = db_file1;
		else if ((stat(db_file2, &buf)) == 0)
			tmp_dir = db_file2;

		if (!tmp_dir && !make_dir) {
			if (db_file1) XtFree(db_file1);
			if (db_file2) XtFree(db_file2);
			/*
			 * This alt_dir doesn't have a session database
			 * return now and try another directory
			 */
			return (False);
		}

		if (tmp_dir) {
			/*
			 * The alt_dir has a session database, create
			 * a copy of its entire directory because all
			 * of the sessions state is needed.
			 */
			char		*tmp;
			
			tmp = XtMalloc (strlen (alt_dir) + 
					strlen (session_dir) + 10);
			if (!tmp)
				return (False);

			(void) sprintf (tmp, "cp -r %s %s",
					alt_dir, session_dir);

			if ((system (tmp)) == -1)
				return (False);
			XtFree (tmp);

			if ((stat(session_dir, &buf)) != 0)
				/*
				 * It should have been created by the cp
				 */
				return (False);

			/*
			 * The session database needs to be updated.
			 */
			if (tmp_dir == db_file1)
				sprintf (smGD.clientPath, "%s/%s",
					session_dir, SM_CLIENT_FILE2);
			else
				sprintf (smGD.clientPath, "%s/%s",
					session_dir, SM_CLIENT_FILE);
		} else {
			/*
			 * The alt_dir did not have a session database.
			 * Create a directory and load the system defaults
			 */
			if ((mkdir (session_dir, 0000)) == 0)
				(void) chmod (session_dir, 0755);
			else {
                		PrintErrnoError(DtError, 
					smNLS.cantCreateDirsString);
				return (False);
			}
			SetSysDefaults ();
		}
	} else {
		/*
		 * The directory for the specified session exists
		 * but it doesn't have any client databases.  Start
		 * a new user session.  If a user wants a sesssion
		 * with no apps, they should create a zero-length
		 * session database.
		 */
		SetSysDefaults ();
	}

	XtFree(db_file1);
	XtFree(db_file2);
	return (True);
}


void 
SmExit (
	int		exitStatus)
{
  /* JET - needed to rework this to avoid exiting before we are
   *  *really* ready to 
   */
  if (smGD.ExitComplete)
    {
      if (smXSMP.saveState.saveComplete && 
	  smXSMP.saveState.shutdown &&
	  !smXSMP.saveState.shutdownCanceled)
	XSMPExit ();
      exit(exitStatus);
    }
  else
    return;
}

void
SetSIGPIPEToDefault ()
{
	struct sigaction pipeSig;

	sigemptyset(&pipeSig.sa_mask);
	pipeSig.sa_flags = 0;
	pipeSig.sa_handler = SIG_DFL;
	(void) sigaction(SIGPIPE, &pipeSig, (struct sigaction *) NULL);
}
