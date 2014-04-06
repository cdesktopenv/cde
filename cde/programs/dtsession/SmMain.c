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
/* $TOG: SmMain.c /main/18 1998/04/20 12:59:26 mgreess $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:        SmMain.c
 **
 **  Project:     HP DT Session Manager (dtsession)
 **
 **  Description:
 **  -----------
 **  This is the controlling program for the session manager.  It
 **  calls routines to start the BMS, initialize globals and handlers,
 **  and restore the correct session.
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
#include <locale.h>
#ifdef __osf__
#include <sys/access.h>
#endif
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#if defined (USE_X11SSEXT)
#include <X11/extensions/scrnsaver.h>
#endif /* USE_X11SSEXT */

#include <Xm/Xm.h>
#include <Dt/UserMsg.h>
#include <Dt/EnvControlP.h>
#include <Dt/DtP.h>
#include <Dt/Lock.h>
#ifdef USE_XINERAMA
#include <DtXinerama.h>		/* JET - Xinerama support */
#endif
#include "Sm.h"
#include "SmError.h"
#include "SmGlobals.h"
#include "SmCommun.h"
#include "SmRestore.h"
#include "SmUI.h"
#include "SrvPalette.h"
#include "SmProtocol.h"
#include "SmXSMP.h"

/*
 * Internal Functions
 */
static void StopAll(int i);
static int RegisterX11ScreenSaver(Display *display, int *ssEventType);

/*
 * Internal Defines
 */
#ifdef __hpux
#define SECURE_SYS_PATH "/.secure/etc/passwd"
#endif
#ifdef _AIX
#define SECURE_SYS_PATH "/etc/security/passwd"
#endif
#ifdef SVR4
#define SECURE_SYS_PATH "/etc/shadow"
#endif
#ifdef CSRG_BASED
#define SECURE_SYS_PATH "/etc/master.passwd"
#endif



/*************************************<->*************************************
 *
 *  main (argc, argv)
 *
 *
 *  Description:
 *  -----------
 *  Controls the startup and event dispatching of the session manager.
 *
 *
 *  Inputs:
 *  ------
 *  argc = command line options
 *  argv = number of command line options
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
int
main (int argc, char **argv)
{
    int                         n, tmp;
    Arg                         args[10];
    XEvent                      next;
    String                      tmpString;
    XWindowAttributes           windAtt;
    XPropertyEvent              *pEvent = (XPropertyEvent *) &next;
    int                         status;
    struct stat                 buf;
    Display                     *srvDisplay;
    struct sigaction            stopvec;
    char		 	*lang;

#ifdef __osf__

# include <sys/sysinfo.h>
# include <sys/proc.h>

    unsigned long        op;
    int                  buffer[2];
    unsigned long        nbytes = 1;
    char*                arg = 0;
    unsigned long        flag = 0;

    int                  ssi_status;

    op = SSI_NVPAIRS;

    buffer[0] = SSIN_UACPROC;
    buffer[1] =  UAC_NOPRINT;
# ifdef DEBUG_UAC
    buffer[1] |= UAC_SIGBUS;
# endif

    ssi_status = setsysinfo ( op, buffer, nbytes, arg, flag );
#endif

    setlocale( LC_ALL, "" );
    XtSetLanguageProc( NULL, NULL, NULL );

    smGD.smState = IN_PROCESS;
    smGD.programName = strdup (argv[0]);

    /* JET - By default, we always want to leave when SmExit() is called. */
    smGD.ExitComplete = True;

    /*
     * We report some errors before we call the DtInitialize procedure.
     * The Dt initialize procedure sets this name.  If we report
     * an error via DtSimple error before this is set we see the message
     * prepended with <unknown program name> in the error log.
     */
    DtProgName = SM_RESOURCE_NAME ;
    
#ifdef DEBUG
    if(argc > 5)
    {
        int junk = 1;
        while(junk)
        {
           junk = 1;
        }
    }
#endif /*  DEBUG */
    /*
     * Set our effective gid to the real gid until we need it to be
     * sys (during contention management)
     */
    smGD.runningGID = getgid();
    smGD.conMgmtGID = getegid();

    /*
     * Set uid up according to whether this is a secure system
     * Secure systems need root priviledges to read the /etc/passwd file
     */
    smGD.runningUID = getuid();

#ifdef linux			/* linux always needs to be setup as secure */

    /*
     * Save the root priviledge to be restored when trying to unlock
     */
    smGD.unLockUID = geteuid();
    smGD.secureSystem = True;
    SM_SETEUID(smGD.runningUID);
    
#else

# ifdef SECURE_SYS_PATH
    status = stat(SECURE_SYS_PATH, &buf);
# else
    status = -1;
# endif

    if(status == -1)
    {
        /*
         * this is not a secure system - remove all suid priviledges
         */
        smGD.unLockUID = smGD.runningUID;
        smGD.secureSystem = False;
        SM_SETESUID(smGD.runningUID);
    }
    else
    {
       /*
        * Save the root priviledge to be restored when trying to unlock
        */
        smGD.unLockUID = geteuid();
        smGD.secureSystem = True;
        SM_SETEUID(smGD.runningUID);
    }

#endif /* linux */

    /*
     * Initialize LANG if it isn't defined.
     */
    if ((lang = getenv ("LANG")) == NULL)
    {
	lang = XtMalloc (7);
	(void) strcpy (lang, "LANG=C");
	(void) putenv (lang);
    }

#ifdef __hpux
    setresgid(-1, smGD.runningGID, -1);
#else  /* _AIX or any other system */
#ifndef	SVR4
    setregid(smGD.conMgmtGID, smGD.runningGID);
#else
    setgid(smGD.conMgmtGID);
    setegid(smGD.runningGID);
#endif
#endif /* !hpux */

   /*
    * Set up POSIX sigaction structs
    */

    /*
     * Must set SIGPIPE to SIG_IGN so the process does not exit
     * if a child terminates unexpectedly during an I/O operation
     * that raises SIGPIPE.  Note that before this process exec's
     * any subprocesses, SIGPIPE must be set to SIG_DFL.
     */
    sigemptyset(&stopvec.sa_mask);
    stopvec.sa_flags = 0;
    stopvec.sa_handler = SIG_IGN;
    (void) sigaction(SIGPIPE, &stopvec, (struct sigaction *) NULL);

    stopvec.sa_handler = StopAll;
    sigemptyset(&stopvec.sa_mask);
    stopvec.sa_flags = 0;
 
    smGD.childvec.sa_handler = WaitChildDeath;
    sigemptyset(&smGD.childvec.sa_mask);
    smGD.childvec.sa_flags = 0;

    smGD.defvec.sa_handler = SIG_DFL;
    sigemptyset(&smGD.defvec.sa_mask);
    smGD.defvec.sa_flags = 0;

    smGD.appCon = 0;
    smGD.display = 0;

    /*
     * This must be done before XtToolkitInitialize
     * to set up the local environment
     */
    _DtEnvControl(DT_ENV_SET);
    _DtEnvControl(DT_ENV_SET_BIN);

    /*
     * Set up NLS error messages first
     */
    InitNlsStrings();

    /*
     * Set the paths of where resources are to be restored from
     */
    SetRestorePath(argc, argv);

    /*
     * The first thing that must happen is that resources must be restored
     * so that my resources will be correct
     */
    if(smGD.compatMode == False)
    {
     /*
      * Load session resources.
      */
      RestoreResources(False,
                       "-load",
                       "-system",
                       "-xdefaults",
                       smGD.resourcePath[0] != '\0' ? "-file" : NULL,
                       smGD.resourcePath,
                       NULL);
    }

    /*
     * Set up to catch SIGTERM
     */
    stopvec.sa_handler = StopAll;
    sigemptyset(&stopvec.sa_mask);
    stopvec.sa_flags = 0;

    sigaction(SIGTERM, &stopvec, (struct sigaction *) NULL);

    /*
     * Now set up a communication with the toolkit
     *
     * Create one display connection for dtsession, and one for
     * the color server. We cannot share a display connection since
     * motif creates a display object for the color server's display during
     * color server initialization. Since the color server is not yet 
     * operational, any dialogs (ie the dtsession logout confirmation
     * dialogs) created on that display do not get the color server colors. 
     * The dtsession display object is created after color server
     * initialization is complete.
     */
    if (smGD.appCon == 0) {
	/*
	 * The following code will have been initialized if the
	 * session creation code posted a dialog.
	 */
        XtToolkitInitialize();
        smGD.appCon = XtCreateApplicationContext();
        smGD.display = XtOpenDisplay(smGD.appCon, NULL, argv[0], 
				SM_RESOURCE_CLASS,
				NULL, 0, &argc, argv);
    }

    srvDisplay = XtOpenDisplay(smGD.appCon, NULL, argv[0], SM_RESOURCE_CLASS,
                                 NULL, 0, &argc, argv);

    /*
     * Initialize XSMP 
     */
    if (!InitXSMP (argv[0]))
        SM_EXIT(-1);

    /* Added the following check to exit in case of error RK 09.08.93 */
    if(smGD.display == NULL)
    {
        PrintError(DtError, GETMESSAGE(4, 1, "Invalid display name - exiting."));
        SM_EXIT(-1);
    }

    /*
     * Lock out other session managers from running - if there is one
     * running already - exit
     */
    if(!_DtGetLock(smGD.display, SM_RUNNING_LOCK))
    {
        PrintError(DtError, GETMESSAGE(2, 2, "Another dtsession is currently running - exiting."));
        SM_EXIT(-1);
    }

    /* JET - initialize for Xinerama, if present 4/12/2001 */
#ifdef USE_XINERAMA
    smGD.DtXineramaInfo = _DtXineramaInit(smGD.display);

# ifdef DEBUG
    if (smGD.DtXineramaInfo == NULL)
      {                         /* No xinerama, how... sad. */
        fprintf(stderr, "### JET SmMain: Xinerama NOT available.\n");
      }
    else
      {
        fprintf(stderr, "### JET SmMain: Xinerama available, scrns = %d\n",
                dpyinfo.DtXineramaInfo->numscreens);
      }
# endif

#endif


   /*
    * Restore preferences
    */
    if (smGD.resourcePath[0] != '\0')
    {
      RestorePreferences(smGD.resourcePath);
    }

    /*
     * Start up the color server
     */
    InitializeDtcolor(srvDisplay, smGD.sessionType);

    /*
     * AFTER the colors are set up - create the top level widget
     * Set up a NULL WM_COMMAND property
     */
    n = 0;
    XtSetArg(args[n], XmNbackground,
             XBlackPixel(smGD.display, XDefaultScreen(smGD.display))); n++;
    XtSetArg(args[n], XmNmappedWhenManaged, False); n++;
    XtSetArg (args[n], XmNwidth, 1); n++;
    XtSetArg (args[n], XmNheight, 1); n++;
    smGD.topLevelWid = XtAppCreateShell (SM_RESOURCE_NAME, SM_RESOURCE_CLASS,
                                         applicationShellWidgetClass,
                                         smGD.display, args, n);
    XtRealizeWidget(smGD.topLevelWid);

    /*
     * Select to get the lock on timeout if the user requests it
     * and if the server supports it. This must be done before
     * InitSMGlobals() so smGD.lockOnTimeoutStatus is known.
     */
#if defined (USE_HPSSEXT)
#ifdef USE_HP_SPECIFIC_XLIB
    tmp = XHPSSChangeNotify(smGD.display, &XaSmScreenSaveRet, 3);
#else /* USE_HP_SPECIFIC_XLIB */
    tmp = -1;
#endif /* USE_HP_SPECIFIC_XLIB */
#elif defined (USE_X11SSEXT)
    tmp = RegisterX11ScreenSaver(smGD.display, &smGD.ssEventType);
#else
    tmp = -1;
#endif

    if(tmp != 0)
    {
	smGD.lockOnTimeoutStatus = False;
	XaSmScreenSaveRet = None;
#if defined (USE_X11SSEXT)
	smGD.ssEventType = None;
#endif
    }
    else
    {
	smGD.lockOnTimeoutStatus = True;
    }

    InitSMGlobals();

    /*
     * Put the program into a wait state
     */
    ShowWaitState(True);

    InitErrorHandler();

    /*
     * Restore resources for lang/resolution independence
     */
    if((smGD.resourcePath[0] != 0) || (smGD.compatMode == False))
    {
	RestoreIndependentResources();
    }

    /*
     * Now restore the rest of the clients and the settings
     */
    if((smGD.clientPath[0] != 0) && (smGD.compatMode == False))
    {
	if(RestoreState() == -1)
	{
	    StartWM();
	}
    }
    else
    {
	if(smGD.compatMode == True)
	{
	    /*
	     * dtstyle needs the DT_SESSION_STATE  even in
	     * compatibility mode
	     */
	    SetCompatState();
	}
	StartWM();
    }

    /* 
     * Run the user's startup script if there is one
     */

    if(smGD.compatMode == False)
    {
	StartEtc(False); /* run sessionetc */
    }

    InitProtocol ();

    /*
     * If we are in compatibility mode - kill the parent and leave the
     * child running so that the script exits
     */
    if(smGD.compatMode == True)
    {
	KillParent();
    }

    /*
     * Select for property notify on the top level window - so that
     * when the style manager changes something - it is known
     */
    XGetWindowAttributes(smGD.display, smGD.topLevelWindow, &windAtt);
    XSelectInput(smGD.display, smGD.topLevelWindow,
		 windAtt.your_event_mask | PropertyChangeMask);

    ShowWaitState(False);

    /*
     * Register for events
     * - PropertyChange (maskable)
     * - ClientMessage (non-maskable)
     */
    XtAddEventHandler(smGD.topLevelWid,
                      PropertyChangeMask,
                      True, ProcessEvent, NULL);

    smGD.smState = READY;

#ifdef __osf__
    /* If we've just populated a new .dtprofile into the user's home
     * directory, put up a popup dialog explaining the need to edit it
     * to allow .login/.profile to work properly
     */

    arg = getenv("DTNEWPROFILE");
    if (arg)
        WarnNewProfile();
#endif

    while(1)
    {
      XtAppNextEvent(smGD.appCon, &next);
      if (next.type != 0)
      {
#if defined (USE_X11SSEXT)
        if (next.type == smGD.ssEventType)
        {
         /*
          * We should simply be calling XtDispatchEvent() but the toolkit
          * doesn't seem to know how to dispatch a run-time generated
          * event type.
          */
          ProcessEvent(smGD.topLevelWid, NULL, &next, NULL);
        }
        else
        {
          XtDispatchEvent(&next);
        }
#else
        XtDispatchEvent(&next);
#endif
      }
    }
}




/*************************************<->*************************************
 *
 *  StopAll
 *
 *
 *  Description:
 *  -----------
 *  Signal handler for SIGTERM. Causes dtsession to do a normal shutdown
 *  procedure without saving any state.
 *
 *
 *  Inputs:
 *  ------
 *  shutDown (state flag)
 *
 *
 *  Outputs:
 *  -------
 *
 *
 *  Comments:
 *  --------
 *
 *  We want to be careful to ignore the TERM signal if we are already
 *  shutting down when it occurs.
 *
 *************************************<->***********************************/

static void
StopAll(int i)
{
    ImmediateExit(-1, 0, True);
}

/*************************************<->*************************************
 *
 *  RegisterX11ScreenSaver
 *
 *
 *  Description:
 *  -----------
 *  Register with X11 screen saver server extension for screen saver events. 
 *
 *  Inputs:
 *  ------
 *  display - display from XtOpenDisplay()
 *  pssEventType - pointer to buffer in which to return ss event type
 *
 *  Outputs:
 *  -------
 *  pssEventType - (rc=0) screen saver event type, (rc!=0) undefined
 *
 *  Returns:
 *  -------
 *  0 - successfully registered for screen saver events
 *  other - failed to register for screen saver events
 *
 *  Comments:
 *  --------
 *
 *************************************<->***********************************/

#if defined (USE_X11SSEXT)
static int
RegisterX11ScreenSaver(
  Display *display,
  int *pssEventType)
{
 /*
  * Register with X11 screen saver server extension.
  */
  int result = -1;
  int ssErrorBase;
  int majorVersion;
  int minorVersion;
  int screen;
  Window root;
  XID xid;
  Atom type;

  if (XScreenSaverQueryExtension(display, pssEventType, &ssErrorBase) &&
      XScreenSaverQueryVersion(display, &majorVersion, &minorVersion) &&
      majorVersion == 1)
  {
   /*
    * Server supports requested version of X11 screen saver extension.
    */
    screen = DefaultScreen(display);
    root = DefaultRootWindow(display);
   
    XGrabServer(display);
    if (!XScreenSaverGetRegistered(display, screen, &xid, &type))
    {
     /* 
      * No other clients registered with this server so register this one.
      */
      XScreenSaverRegister(display, screen, XtWindow(smGD.topLevelWid), XA_WINDOW);
      result = 0;
    }
    XUngrabServer(display);

    if (result == 0)
    {
      XSetWindowAttributes attr;

     /* 
      * Registration successful.
      */
      XScreenSaverSelectInput(display, root, 
                              ScreenSaverNotifyMask|ScreenSaverCycleMask);

      /* Even though OverrideRedirect is the default attribute in this
       *  check, lets make sure it's set, and make sure only the
       *  CWOverrideRedirect attr is being looked at.  The rest is
       *  just random garbage anyway, since it's allocated from the stack.
       */
      attr.override_redirect = True;
      XScreenSaverSetAttributes(display, root, 0, 0, 1, 1, 0, CopyFromParent,
                          CopyFromParent, CopyFromParent, CWOverrideRedirect, &attr);
    }
  }
  return(result);   
}
#endif /* USE_X11SSEXT */

