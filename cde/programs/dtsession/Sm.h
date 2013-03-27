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
/* $XConsortium: Sm.h /main/15 1996/09/14 14:48:18 drk $ */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:        Sm.h
 **
 **  Project:     HP DT Session Manager (dtsession)
 **
 **  Description:
 **  -----------
 **  Contains all general, global variables used by the session manager
 **
 **
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

#ifndef _sm_h
#define _sm_h

/*
 * #include statements
 */

#include <signal.h>
#include <sys/param.h>
#include <X11/Xlib.h>
#include <Dt/DtP.h>
#include <Dt/UserMsg.h>
#include <Tt/tt_c.h>
#include "SmError.h"

#ifdef USE_XINERAMA
# include <DtXinerama.h>
#endif

/* 
 *  #define statements 
 */

#define HELPVIEW_COMMAND_LINE  CDE_INSTALLATION_TOP "/bin/dthelpview -helpVolume Dtintro"


/*
 * Wrappers for system calls
 */
#define		SM_MALLOC(SIZE)			XtMalloc(SIZE)
#define		SM_REALLOC(PTR, SIZE)	XtRealloc(PTR, SIZE)
#define		SM_FREE(PTR)			XtFree(PTR)
#define		SM_EXIT(STATUS)			SmExit(STATUS)

#ifdef __hpux
#define		SM_SETEUID(EUID)	setresuid(-1, EUID, -1)
#define		SM_SETESUID(UID)	setresuid(-1, UID, UID)
#else
#ifndef	SVR4
#define         SM_SETEUID(EUID)        seteuid(EUID)
#define         SM_SETESUID(UID)        setreuid(UID, UID)
#else
#define         SM_SETEUID(EUID)        seteuid(EUID)
#define         SM_SETESUID(UID)        (setuid(UID), seteuid(UID))
#endif
#endif

/*
 * Types of states the session manager can be in
 */
#define		READY			0
#define		IN_PROCESS		1

/*
 *  session types .. home or current or default
 */
#define         DEFAULT_SESSION         0
#define         HOME_SESSION            1
#define         CURRENT_SESSION         2

/*
 * The lower bounds for each resolution of display - caculated
 * as Y Resolution - which is pixels/meter
 */
#define			HIGH_RES_Y_RES			3583
#define			MED_RES_Y_RES			3150
#define			LOW_RES_Y_RES			1

/*
 * Maximum number of screens saved during a session
 */
#define 	MAX_SCREENS_SAVED	10

/* contention management definitions */
#define SM_CM_SYSTEM       (1L << 0)
#define SM_CM_HANDSHAKE    (1L << 1)

#define SM_CM_ALL          (SM_CM_SYSTEM  | SM_CM_HANDSHAKE)
#define SM_CM_NONE         0

#define SM_CM_DEFAULT      SM_CM_NONE

#ifndef NO_MESSAGE_CATALOG
# ifdef __ultrix
#  define _CLIENT_CAT_NAME "dtsession.cat"
# else  /* __ultrix */
#  define _CLIENT_CAT_NAME "dtsession"
# endif /* __ultrix */
extern char *_DtGetMessage(char *filename, int set, int n, char *s);
# define GETMESSAGE(set, number, string)\
    (_DtGetMessage(_CLIENT_CAT_NAME, set, number, string))
#else
# define GETMESSAGE(set, number, string)\
    string
#endif

typedef int (*IOErrorHandlerProc)(
	Display *
);

/* 
 * typedef statements 
 */

/*
 * Pointer to hold information returned by session settings
 */
typedef struct
{
    int			accelNum;	    /* Pointer Control Information */
    int			accelDenom;
    int 		threshold;

    int			timeout;	    /*Screen Saver Information */
    int			interval;
    int			preferBlank;
    int			allowExp;

    char		*fontDirs;	    /*Font path Information */
    
    XKeyboardState	kbdState;           /* Keyboard Control Information */
    char		*autoRepeats;
    
    char		*buttonMap;	    /*Button Mapping Information */
    
    int 		numKeyCode;	    /*Key Mapping information */
    int			keySymPerCode;
    char		*keySyms;

    int			maxKeyPerMod;	    /*Modifier Mapping Information */
    char		*modSyms;

    Boolean		didQuery;	/*Was this information queried for*/

    int			confirmMode;
    int			startState;
} SessionSettings, *SessionSettingsPtr;

/*
 * Structure to save all of the session manager resources
 */
typedef struct
{
    char	*wmStartup;
    Boolean	querySettings;
    char	*keyholders;
    int		alarmTime;
    int		memThreshold;
    char	*sessionVersion;
    int		displayResolution;
    char	*sessionLang;
    long        contManagement;
    int         waitClientTimeout;
    int         waitWmTimeout;
    Boolean     useBMS;
    Boolean	saveFontPath;
    int         saveYourselfTimeout;
    Boolean     mergeXdefaults;
    int		numSessionsBackedup;
    char	*ignoreEnvironment;
#if defined(USE_XINERAMA)
    int         xineramaPreferredScreen; /* prefered xinerama screen */
#endif
} SessionResources, *SessionResourcesPtr;

/*
 * Screen saver resources.
 */
typedef struct
{
    int         cycleTimeout;
    int         saverTimeout;
    int         lockTimeout;
    Boolean     random;
    char        *saverList;
} SaverResources, *SaverResourcesPtr;

/*
 * Structure to hold global state settings
 */
typedef struct
{
    Boolean			audioChange;
    Boolean			pointerChange;
    Boolean			pointerMapChange;
    char			pointerMap[5];
    int				numButton;
    Boolean			keyboardChange;
    Boolean			screenSavChange;
    char			dClickBuf[50];
} SettingsSet;

typedef struct
{
	Boolean         audioChange;
	Boolean         pointerChange;
	Boolean         keyboardChange;
	Boolean         screenSavChange;
	Boolean         dClickChange;
	Boolean         fontChange;
	Boolean         preeditChange;
} SettingsCust;


/*
 * Structure to hold general information needed to be shared by
 * different modules in the session manager
 */
typedef struct
{
    Display		*display;
    Widget		topLevelWid;
    Window		topLevelWindow;
    XtAppContext 	appCon;
    char		*programName;		/* main()'s argv[0] */
    int			numSavedScreens;
    int			screen;
    int			lockedState;
    int			smState;
    short		sessionType;
    unsigned long	blackPixel;
    unsigned long	whitePixel;
    unsigned long	foregroundPix;
    unsigned long 	backgroundPix;
    Cursor		lockCursor, padlockCursor, blankCursor;
    Cursor		waitCursor;
    char		resourcePath[MAXPATHLEN + 1];
    char		settingPath[MAXPATHLEN + 1];
    char		clientPath[MAXPATHLEN + 1];
    char		fontPath[MAXPATHLEN + 1];
    char		exitPath[MAXPATHLEN + 1];
    char		etcPath[MAXPATHLEN + 1]; /*
						  * Used for sessionEtc on
						  * the way up and for
						  * moving current to
						  * current.old on the way
						  * down
						  */
    char		*savePath;
    char		*restoreSession;
    Boolean		dtwmRunning;
    Boolean		bmsDead;
    Tt_pattern		requests2Handle;
    Tt_pattern		events2Watch;
    Boolean		compatMode;
    Boolean		lockOnTimeoutStatus;
    gid_t		runningGID, conMgmtGID;
    uid_t		runningUID, unLockUID;
    Boolean		secureSystem;
    Boolean		screenSaver;  /* obsolete */
    Boolean		screenSaverRunning; /* obsolete */
    Boolean		userSetWaitWmTimeout;
    int                 savedWaitWmTimeout;
    /*
     * These strings are for our copy once we
     * fetch resources with XtGetApplicationResources.
     * These string returned can be overwritten anytime
     * another Xrm call is made as in MergeCoeResources (libXv3.0)
     * or in our own calls to XrmGetFileDatabase.
     */

    /*
     * This set is the for smRes
     */
    char	*wmStartup;
    char	*keyholders;
    char	*sessionVersion;
    char	*sessionLang;

    /*
     * This set is the for smSettings
     */
    char	*fontDirs;	    /*Font path Information */
    char	*autoRepeats;
    char	*buttonMap;	    /*Button Mapping Information */
    char	*keySyms;
    char	*modSyms;
   
    char        *saverList; /* Current screen saver list */
    void        *saverListParse; /* Current parsed screen saver list */

    Boolean	coverScreen; /* full screen cover */
    char        *SmNextension; /* screen saver extension name */
    char        *SmCextension; /* screen saver extension class */
    char        *extensionSpec; /* sprintf specification */
#if defined (USE_X11SSEXT)
    int         ssEventType; /* screen saver event type */
#endif /* USE_X11SSEXT */
    struct sigaction childvec;
    struct sigaction defvec;

    Boolean	homeSave;	 /* True if the save is via the Style
				    Manager's Save Home Session button. */
    Boolean	displaySpecific; /* Is Ture if the session is for a
				    specific display, otherwise False */
    Boolean	loggingOut;	/* Is True if the current save is for
				   a logout; False otherwise. */

#ifdef USE_XINERAMA		/* JET - Xinerama.  Schwiing!  */
  DtXineramaInfoPtr_t DtXineramaInfo;
#endif

    Boolean ExitComplete;	/* JET - don't exit before we are ready... */

} GeneralData;


/*
 * External variables
 */
extern	char	*_dtsessionMG();

/*
 * Files that the session manager saves sessions to and restores sessions
 * from.  allocated in SmStrDef.c
 */
extern char SM_CLIENT_FILE[];
extern char SM_CLIENT_FILE2[];
extern char SM_RESOURCE_FILE[];
extern char SM_FONT_FILE[];
extern char SM_LOW_RES_EXT[];
extern char SM_MED_RES_EXT[];
extern char SM_HIGH_RES_EXT[];
extern char SM_SETTING_FILE[];
extern char SM_CONVERSION_FILE[];
extern char SM_SYSTEM_CLIENT_FILE[];
extern char SM_SYSTEM_RESOURCE_FILE[];
extern char SM_SYSTEM_FONT_FILE[];

extern char SM_SYSTEM_PATH[];
extern char SM_SECURE_PATH[];

extern char SM_CURRENT_DIRECTORY[];
extern char SM_CURRENT_FONT_DIRECTORY[];
extern char SM_HOME_DIRECTORY[];
extern char SM_HOME_FONT_DIRECTORY[];
extern char SM_OLD_EXTENSION[];
extern char SM_SYSTEM_DIRECTORY[];
extern char smEtcFile[];
extern char smExitFile[];

/*
 * Session manager name and class used to get resources allocated in SmStrDef.c
 */
extern char SM_RESOURCE_CLASS[];
extern char SM_RESOURCE_NAME[];
extern char SM_HELP_VOLUME[];


/*
 * Name of the lock the session manager has to make sure it's the only
 * session manager running
 */
extern char	SM_RUNNING_LOCK[];

/*
 * Location and name of the screen saver program
 */
extern char	SM_SCREEN_SAVER_LOC[];
extern char	SM_SCREEN_SAVER_NAME[];

/*
 * Global resource settings. These settings determine program behavior
 * wrt settings restoration, lock behavior, window manager startup etc
 */
extern SessionResources		smRes;
extern SaverResources		smSaverRes;
extern SettingsSet		smToSet;
extern SettingsCust		smCust;
extern SessionSettings		smSettings;
extern GeneralData		smGD;


/*  
 *  External Interface  
 */

#endif /* __sm_h*/
