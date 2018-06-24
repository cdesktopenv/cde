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
 * (c) Copyright 1989, 1990, 1991, 1992 OPEN SOFTWARE FOUNDATION, INC. 
 * ALL RIGHTS RESERVED 
*/ 
/* 
 * Motif Release 1.2.1
*/
/*
 * (c) Copyright 1987, 1988, 1989, 1990 HEWLETT-PACKARD COMPANY */

/*
 * Included Files:
 */

#include "WmGlobal.h"

#include <locale.h>
#include <Dt/Message.h>
#include <Dt/EnvControlP.h>
/*
 * include extern functions
 */

#include "WmCEvent.h"
#include "WmEvent.h"
#include "WmInitWs.h"
#include "WmError.h"
#include "WmIPC.h"
#include "WmBackdrop.h"


/*
 * Function Declarations:
 */
int WmReturnIdentity (int argc, char *argv[], char *environ[]);
#define ManagedRoot(w) (!XFindContext (DISPLAY, (w), wmGD.screenContextType, \
(caddr_t *)&pSD) ? (SetActiveScreen (pSD), True) : \
(IsBackdropWindow (ACTIVE_PSD, (w))))

WmScreenData *pSD;

/*
 * Global Variables:
 */

WmGlobalData wmGD;
#ifndef NO_MESSAGE_CATALOG
NlsStrings wmNLS;
#endif
int WmIdentity;



/*************************************<->*************************************
 *
 *  main (argc, argv, environ)
 *
 *
 *  Description:
 *  -----------
 *  This is the main window manager function.  It calls window manager
 *  initializtion functions and has the main event processing loop.
 *
 *
 *  Inputs:
 *  ------
 *  argc = number of command line arguments (+1)
 *
 *  argv = window manager command line arguments
 *
 *  environ = window manager environment
 *
 *************************************<->***********************************/

int
main (int argc, char *argv [], char *environ [])
{
    XEvent	event;
    Boolean	dispatchEvent;

    setlocale(LC_ALL, "");

    /*
     * Set up environment variables for this HP DT client
     */
    _DtEnvControl(DT_ENV_SET);

    /*
     * Force LANG lookup early. 
     * (Front end may change $LANG to 'C' as part
     *  of string space reduction optimization.)
     */
     {
	 char * foo = ((char *)GETMESSAGE(44, 1, ""));
     }
    XtSetLanguageProc (NULL, (XtLanguageProc)NULL, NULL);

    /*  
     * Get Identity
     */
    WmIdentity = WmReturnIdentity(argc, argv, environ);

    /*
     * Initialize the workspace:
     */

    InitWmGlobal (argc, argv, environ);

    /*
     * Set up PATH variable if it must run as standalone command
     * invoker
     */
    if (wmGD.dtLite)
    {
	_DtEnvControl(DT_ENV_SET_BIN);
    }
    
    /*
     * MAIN EVENT HANDLING LOOP:
     */

    for (;;)
    {
        XtAppNextEvent (wmGD.mwmAppContext, &event);


        /*
	 * Check for, and process non-widget events.  The events may be
	 * reported to the root window, to some client frame window,
	 * to an icon window, or to a "special" window management window.
	 * The lock modifier is "filtered" out for window manager processing.
	 */

	wmGD.attributesWindow = 0L;

	if ((event.type == ButtonPress) || 
	    (event.type == ButtonRelease))
	{
	    if ((wmGD.evLastButton.button != 0) &&
		ReplayedButtonEvent (&(wmGD.evLastButton), 
				     &(event.xbutton)))
	    {
		wmGD.bReplayedButton = True;
	    }
	    else
	    {
		/* save this button for next comparison */
		memcpy (&wmGD.evLastButton, &event, sizeof (XButtonEvent));
		wmGD.bReplayedButton = False;
	    }
	}
	dispatchEvent = True;
	if (wmGD.menuActive)
	{
	    /*
	     * Do special menu event preprocessing.
	     */

	    if (wmGD.checkHotspot || wmGD.menuUnpostKeySpec ||
		wmGD.menuActive->accelKeySpecs)
	    {
	        dispatchEvent = WmDispatchMenuEvent ((XButtonEvent *) &event);
	    }
	}

	if (dispatchEvent)
	{
	    if (ManagedRoot(event.xany.window))
	    {
	        dispatchEvent = WmDispatchWsEvent (&event);
	    }
	    else
	    {
	        dispatchEvent = WmDispatchClientEvent (&event);
	    }

	    if (dispatchEvent)
	    {
                /*
                 * Dispatch widget related event:
                 */

                XtDispatchEvent (&event);
	    }
	}
    }

} /* END OF FUNCTION main */

/******************************<->*************************************
 *
 *  WmReturnIdentity (argc, argv, environ)
 *
 *
 *  Description:
 *  -----------
 *  This function checks the last component of the (path)name
 *  contained in argv[0] and makes a global decision as to whether
 *  it should fetch resources as mwm or dtwm.
 *
 *  Inputs:
 *  ------
 *  argc = number of command line arguments (+1)
 *
 *  argv = window manager command line arguments
 *
 *  environ = window manager environment
 *
 ******************************<->***********************************/

int WmReturnIdentity ( int argc, char *argv[], char *environ[]) 
{
	char *tempString;
	char *origPtr;

	/* assume it's dtwm until proven differently */

 	int retVal = DT_MWM;

	if (!(tempString = 
	      (char *)(XtMalloc ((unsigned int)(strlen (argv[0]) + 1)))))
	{
		Warning(((char *)GETMESSAGE(44, 2, "Insufficient memory for name of window manager")));
		exit(WM_ERROR_EXIT_VALUE);
	}

	origPtr = tempString;

	if (strrchr(argv[0], '/'))
	{
		
		strcpy(tempString, (strrchr(argv[0], '/')));

		tempString++;
	}
	else
		strcpy(tempString, argv[0]);

	if (!(strcmp(tempString, WM_RESOURCE_NAME)))
	/*
	 *
	 *   If it's explicity "mwm", then set our identity anew.
	 *
	 */
	{
		retVal = MWM;
	}

	XtFree((char *)origPtr);

	return(retVal);

} /* END OF FUNCTION WmReturnIdentity */

/*************************     eof   ******************************/
