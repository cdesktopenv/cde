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
/* $TOG: SmCreateDirs.c /main/9 1997/02/24 09:23:16 barstow $ */
/*                                                                            *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                           *
 * (c) Copyright 1993, 1994 International Business Machines Corp.             *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                            *
 * (c) Copyright 1993, 1994 Novell, Inc.                                      *
 */
/******************************************************************************
 *
 * File Name: SmCreateDirs.c
 *
 *****************************************************************************/

#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xlibint.h>
#include <X11/Intrinsic.h>
#include <Dt/DtNlUtils.h>
#include <Dt/DtPStrings.h>
#include <Dt/WsmP.h>

/********    Private Function Declarations    ********/

static int 
GetShortHostname (
	char*, 
	unsigned int);

static char *
GetSessionDirProperty (
	Display		*display);

static char *
GetDisplayName (
	Display		*display);


/*************************************<->*************************************
 *
 *  _DtCreateDtDirs (display) 
 *
 *    1. Creates ~/.dt, ~/.dt/types, ~/.dt/tmp and either 
 *       ~/.dt/sessions or ~/.dt/<display_name>
 *
 *    2. Returns the name of the session directory
 *
 *  Outputs:
 *  -------
 *  Returns the session directory or NULL if malloc fails or ~/.dt
 *  cannot be created
 *
 *************************************<->***********************************/

char * 
_DtCreateDtDirs(
        Display *display )
{
    char 		*tmpPath;
    Boolean 		needSessionsDir = False;
    Boolean 		useOldSession = False;
    struct stat 	buf;
    int 		status;
    char 		*home;
    char		*sessionDir;
    char		*displayName;

    /*
     * Sanity check - make sure there's an existing display
     */
    if(!display)
	return(NULL);
    
    if ((home =getenv("HOME")) == NULL)
        home = "";
    
    tmpPath = (char *) XtMalloc((MAXPATHLEN + 1) * sizeof(char));
    if(tmpPath == NULL)
	return(NULL);

    /*
     * If the $HOME/.dt directory does not exist, create it
     */
    strcpy(tmpPath, home);
    strcat(tmpPath, "/" DtPERSONAL_CONFIG_DIRECTORY);

    status = stat(tmpPath, &buf);
    if (status == -1) {
	status = mkdir(tmpPath, 0000);
	if (status == -1) {
	    XtFree(tmpPath);
	    return(NULL);
	}
	(void)chmod(tmpPath, 0755);
    }

    /*
     * Create the personal DB directory if it does not exist.  
     */
    strcpy(tmpPath, home);
    strcat(tmpPath, "/" DtPERSONAL_DB_DIRECTORY);
    
    if ((status = stat (tmpPath, &buf)) == -1) {
        if ((status = mkdir (tmpPath, 0000)) != -1)
	    (void) chmod (tmpPath, 0755);
    }

    /*
     * Create the personal tmp dir if it does not exist.
     */
    strcpy(tmpPath, home);
    strcat(tmpPath, "/" DtPERSONAL_TMP_DIRECTORY);

    if ((status = stat (tmpPath, &buf)) == -1) {
	if ((status = mkdir (tmpPath, 0000)) != -1)
	    (void) chmod (tmpPath, 0755);
    }

    /*
     * The creation of the session directory is tricky:
     *  
     *   For CDE 1.0 sessions, if a display-specific directory exists,
     *   it will take precedence.  CDE 1.0 does not support the selection
     *   of a session.
     *
     *   However, for newer CDE implementations, if a specific session
     *   was selected, the specified session will be used.  If no session
     *   was selected, the CDE 1.0 mechanism will be used.
     *
     * If a CDEnext session is being used, the session directory will
     * be on a property on the root window.  
     *
     * Must check for this property and use it if is set.
     */
    if ((sessionDir = GetSessionDirProperty (display)) != NULL) {
	if (!strcmp (sessionDir, DtSM_SESSION_DIRECTORY)) {
	    /*
	     * Need to create a DtSM_SESSION_DIRECTORY dir if it
	     * does not exist.
	     */
	    needSessionsDir = True;

	} else if (!strcmp (sessionDir, DtSM_SESSION_DISPLAY_DIRECTORY)) {
	    /*
	     * Create a directory for a display-specific session if necessary
	     */
	    if ((displayName = GetDisplayName (display)) != NULL) {

		strcpy (tmpPath, home);
		strcat (tmpPath, "/" DtPERSONAL_CONFIG_DIRECTORY);
                strcat (tmpPath, "/");
                strcat (tmpPath, displayName);

		free(displayName);  /* CDExc22771 */

                if ((status = stat (tmpPath, &buf)) == -1) {
	            if ((status = mkdir (tmpPath, 0000)) != -1)
	                (void) chmod (tmpPath, 0755);
		    else
	                useOldSession = True;
                }
	    }
	    else {
		/*
		 * Something's wrong with the display, use the fallback
		 */
	        useOldSession = True; 
	    }
	} else {
	    /*
	     * The property contains an unrecognized value, fallback to
	     * other session selection algorithm.
	     */
	    useOldSession = True; 
	}
	XFree (sessionDir);
    }
    else
	useOldSession = True; 
    
    if (useOldSession) {
	/*
	 *  Check for a display dependent directory.  If one exists, 
	 *  it will be used.
	 *  
	 *  This is done for backward compatibility - THE DISPLAY 
	 *  DEPENDENT DIR TAKES PRECEDENT.
	 */
	if ((displayName = GetDisplayName (display)) != NULL) {

	    strcpy (tmpPath, home);
	    strcat (tmpPath, "/" DtPERSONAL_CONFIG_DIRECTORY);
	    strcat (tmpPath, "/");
	    strcat (tmpPath, displayName);

	    free(displayName);  /* CDExc22771 */

	    if ((status = stat(tmpPath, &buf)) != 0)
		/*
		 * The display directory does not exist
		 */
		needSessionsDir = True;
	}
	else
	    needSessionsDir = True;
    }

    if(needSessionsDir)
    {
	/*
	 *  If we don't have an old style directory - we check for a sessions
	 *  directory, and create it if it doesn't exist
	 */
	strcpy (tmpPath, home);
	strcat (tmpPath, "/" DtPERSONAL_CONFIG_DIRECTORY);
	strcat (tmpPath, "/" DtSM_SESSION_DIRECTORY);

	if ((status = stat(tmpPath, &buf)) == -1) {
	    if ((status = mkdir(tmpPath, 0000)) == -1) {
		XtFree(tmpPath);
		return(NULL);
	    }
	    (void)chmod(tmpPath, 0755);
	}
    }

    return(tmpPath);
}

/*------------------------------------------------------------------------+*/

static int
GetShortHostname(
        char *buffer,
        unsigned int bufsize )
{
   char * ptr;
   int status;

   if (status = gethostname(buffer, bufsize))
      return status; /* failed gethostname */
   if (ptr = strstr(buffer, (char *)"."))
      *ptr = '\0';  /* delete domain name if there is one */
   return 0;
}

/*------------------------------------------------------------------------+*/

/*
 * GetSessionDirProperty -
 */
static char *
GetSessionDirProperty (
	Display		*display)
{
	int			propStatus;
	Atom			actualType;
	int			actualFormat;
	unsigned long		nitems;
	unsigned long		leftover;
	char			*property = NULL;
	Atom			tmpAtom;

	tmpAtom = XInternAtom(display, _XA_DT_RESTORE_DIR, False);

	propStatus = XGetWindowProperty (display, RootWindow(display, 0),
					(Atom) tmpAtom, 0L,
					1000000L, False,
					AnyPropertyType, &actualType,
					&actualFormat, &nitems, &leftover,
					(unsigned char **)&property);

	if (propStatus == Success && actualType != None && 
			actualFormat == 8 && nitems != 0)
		return(property);

	if (property)
		XFree(property);
	return (NULL);
}

/*------------------------------------------------------------------------+*/

/*
 * GetDisplayName -
 */
static char *
GetDisplayName (
	Display		*display)
{
    char 		*tmpPath;
    char 		hostName[101], displayName[101];
    char		*pch, *tmpNumber = NULL;
    char		*returnPath;

    /*
     * Create the display name and append it to the current path.
     */
    (void)strcpy(hostName, display->display_name);
    (void)strcpy(displayName, display->display_name);
    
    /*
     * If this is run to unix or local get the host name - otherwise
     * just use what we have
     */

    /*
     * Strip host name to nothing but the unqualified (short) host name
     */
    if (pch = DtStrchr(hostName, ':'))
	*pch = '\0';

    if (pch = DtStrchr(hostName, '.'))
	*pch = '\0';

    if((!strcmp(hostName, "unix")) || (!strcmp(hostName, "local"))
       || (!strcmp(hostName, "")))
    {
	/*
	 * host name is local - get the real name
	 */
	(void) GetShortHostname(hostName, 25);
    }
    
    /*
     * Strip screen off of display name
     */
    if (tmpNumber = DtStrchr(displayName, ':'))
	if (pch = DtStrchr(tmpNumber, '.'))
	    *pch = '\0';

    /*
     * Strip it down to 14 chars (actually, 14 bytes or less)
     */
    if((strlen(tmpNumber) + strlen(hostName)) > (size_t)14)
    {
	size_t 		tnLen;
	int 		lastChLen;
	char 		*lastCh;

	/* Pare display number to at most 12 bytes */
	while ((tnLen = strlen(tmpNumber)) > (size_t)12)
	{
	    /* Remove the last character, an try again */
	    DtLastChar(tmpNumber, &lastCh, &lastChLen);
	    *lastCh = '\0';
	}

	/* Pare down host name, if necessary */
	while ((strlen(hostName) + tnLen) > (size_t)14)
	{
	    /* Remove the last character, and try again */
	    DtLastChar(hostName, &lastCh, &lastChLen);
	    *lastCh = '\0';
	}
    }
    
    strcat (hostName, tmpNumber);

    returnPath = strdup (hostName);

    return (returnPath);
}
