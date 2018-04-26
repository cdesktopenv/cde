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
/* $TOG: SmError.c /main/5 1998/10/26 17:20:29 mgreess $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:        SmError.c
 **
 **  Project:     HP DT Session Manager (dtsession)
 **
 **  Description:
 **  -----------
 **  This file contains all session manager error functions.  The session
 **  manager traps all errors from the toolkit and server, and takes action
 **  depending on the type of the error.
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
#ifdef _SUN_OS
#include <string.h>
#endif
#include <X11/Intrinsic.h>
#include <Dt/UserMsg.h>
#include "Sm.h"
#include "SmError.h"
#include "SmGlobals.h"


/*
 * Global variables
 */
NlsStrings   		smNLS;

/*
 * Local functions
 */

static int LibError( Display *, XErrorEvent *) ;
static int LibIOError( void ) ;
static void ToolkitWarning( char *) ;
static void ToolkitError( char *) ;



/*************************************<->*************************************
 *
 *  InitErrorHandler ()
 *
 *
 *  Description:
 *  -----------
 *  Initialize all error handlers for use with the session manager
 *  session manager should only exit on real severe conditions.
 *  it should try to gracefully recover on the rest.
 *
 *
 *  Inputs:
 *  ------
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
void 
InitErrorHandler( void )
{
    XSetErrorHandler(LibError);
    XSetIOErrorHandler( (IOErrorHandlerProc) LibIOError);
    XtSetWarningHandler(ToolkitWarning);
    XtSetErrorHandler(ToolkitError);
}


/*************************************<->*************************************
 *
 *  LibError (display, errorEvent)
 *
 *
 *  Description:
 *  -----------
 *  X error handler.  Takes care of X errors so that the server will
 *  not terminate the session manager on any error.
 *
 *
 *  Inputs:
 *  ------
 *  errorEvent = pointer to error event returned by the server.
 *
 * 
 *  Outputs:
 *  -------
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
static int 
LibError(
        Display *display,
        XErrorEvent *errorEvent )
{
#ifdef DEBUG
    switch (errorEvent->error_code)
    {
      case Success:
	break;
      case BadAccess:
	PrintError(DtError, BAD_ACCESS);
	break;
      case BadAtom:
	PrintError(DtError, BAD_ATOM);	
	break;
      case BadDrawable:
	PrintError(DtError, BAD_DRAWABLE);
	break;
      case BadMatch:
	PrintError(DtError, BAD_MATCH);
	break;
      case BadValue:
	PrintError(DtError, BAD_VALUE);
	break;
      case BadWindow:
	PrintError(DtError, BAD_WINDOW);
	break;
      default:
	PrintError(DtError, DEFAULT_ERROR);
	break;
    }
#endif /*DEBUG*/
    return 0;
}


/*************************************<->*************************************
 *
 *  LibIOError ()
 *
 *
 *  Description:
 *  -----------
 *  IO error handler.  In charge of handling IO events from the
 *  X server
 *
 *
 *  Inputs:
 *  ------
 *
 * 
 *  Outputs:
 *  -------
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
static int 
LibIOError( void )
{
    PrintError(DtError, GETMESSAGE(8, 1, "Connection to server lost - exiting."));
    SM_EXIT(-1);
    return 0;
}


/*************************************<->*************************************
 *
 *  ToolkitWarning (message)
 *
 *
 *  Description:
 *  -----------
 *  Handles all toolkit warnings
 *
 *
 *  Inputs:
 *  ------
 *  message = error message sent by toolkit
 *
 * 
 *  Outputs:
 *  -------
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
static void 
ToolkitWarning(
        char *message )
{
#ifdef DEBUG
    PrintError(DtError, message);
#endif /*DEBUG*/    
}



/*************************************<->*************************************
 *
 *  ToolkitError (message)
 *
 *
 *  Description:
 *  -----------
 *  Handles all toolkit errors
 *
 *
 *  Inputs:
 *  ------
 *  message = error message sent by toolkit
 *
 * 
 *  Outputs:
 *  -------
 *
 *  Comments:
 *  --------
 * 
 *  Xt assumes the client will exit when an XtError is generated
 *  so we must exit since the state will be undefined if we
 *  continue
 * 
 * 
 *************************************<->***********************************/
static void 
ToolkitError(
        char *message )
{

    PrintError(DtError, message);
    SM_EXIT(-1);
}


/*************************************<->*************************************
 *
 *  PrintError (severity, help)
 *
 *
 *  Description:
 *  -----------
 *  Handles the printing of all session manager errors using the dt API
 *  These are simple errors that don't set errno
 *
 *
 *  Inputs:
 *  ------
 *  severity = severity of the error
 *  help = help message to user (what type of error)
 *
 * 
 *  Outputs:
 *  -------
 *
 *  Comments:
 *  --------
 *  WARNING: Currently the va_alist parameter is not used in the
 *  DtSimpleError is not used
 * 
 *************************************<->***********************************/
void 
PrintError(
        DtSeverity severity,
        char *help )
{
    _DtSimpleError(DtProgName, severity, NULL, "%.2000s", help);
}


/*************************************<->*************************************
 *
 *  PrintErrnoError (severity, help)
 *
 *
 *  Description:
 *  -----------
 *  Handles the printing of all session manager errors using the dt API
 *  These are simple errors that set errno
 *
 *
 *  Inputs:
 *  ------
 *  severity = severity of the error
 *  help = help message to user (what type of error)
 *
 * 
 *  Outputs:
 *  -------
 *
 *  Comments:
 *  --------
 *  WARNING: Currently the va_alist parameter is not used in the
 *  DtSimpleErrnoError is not used
 * 
 *************************************<->***********************************/
void 
PrintErrnoError(
        DtSeverity severity,
        char *help )
{
    _DtSimpleErrnoError(DtProgName, severity, NULL, "%s", help);
}
