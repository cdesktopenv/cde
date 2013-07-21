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
/****************************<+>*************************************
 ********************************************************************
 **
 **   File:     EnvControl.c
 **
 **   $TOG: EnvControl.c /main/21 1999/01/28 17:59:53 mgreess $
 **
 **   Project:  DtSvc Runtime Library
 **
 **   Description: Get/Set the client's shell environment
 **
 **(c) Copyright 1992,1993,1994 by Hewlett-Packard Company
 **(c) Copyright 1993,1994 International Business Machines Corp.
 **(c) Copyright 1993,1994 Sun Microsystems, Inc.
 **(c) Copyright 1993,1994 Unix System Labs, Inc., a subsidiary of Novell, Inc.
 **
 ********************************************************************
 ****************************<+>*************************************/
#include <stdlib.h>
#include <stdio.h>
#define X_INCLUDE_STRING_H
#define XOS_USE_XT_LOCKING
#include <X11/Xos_r.h>
#include <X11/Intrinsic.h>
#include <X11/Xatom.h>
#include <Dt/DtosP.h>
#include <Dt/DtNlUtils.h>
#include "EnvControlP.h"
#include "EnvControlI.h"
#include "DtSvcLock.h"

extern char **environ;

 /* 
  *  We assume that the following fields of these static structures
  *  are initialized at runtime to NULL pointers.
  */
static struct environStruct _postDtEnvironment;
static struct environStruct _preDtEnvironment;

/* envBitVector is used to determine whether an environment
 * variable has been put to the environment by the application.
 * The flag values are in EnvControlI.h.
 */
static unsigned int envBitVector = 0;

static void _EnvAdd (char *, char *, unsigned int);


/****************************************************************
* NOTES on an application's environ(5):
* 
* The environment variable is an array of NULL-terminated strings.
* char **environ is defined in crt.o and is publically extern-ed to
* all applications.  There is one such variable per UN*X application.
* 
* To deference an environment string, as in the debugger, use:
* 
*        environ[0] -- environ[n-1], where n is the number of
*        name=value pairs.
* 
* E.g, in xdb:
* 
*        >p environ[0]
* 
* You must manually malloc() space to keep a specific environment string
* around.
* 
* putenv() is useless at NULLing out an environment variable.  It will
* create a new value and will replace an existing value; however,
* in terms of getting rid of an environment variable, putenv() will only
* replace it with a NULL string ("") rather than freeing the space 
* for it and NULLing the pointer.  Afterwards, getenv() will return
* the NULL string rather than a NULL pointer, and hence give "incorrect"
* information to an application querying for a non-set environment
* variable.  
*
* E.g., there's a big difference between the following pairs 
* to XmGetPixmap():
* 
*         XBMLANGPATH=""
* 
*         --and--
* 
*         XBMLANGPATH=<null pointer>
* 
* The first will cause XmGetPixmap() to look nowhere for an icon image;
* the second will cause a lookup in all the default locations, which
* is what you want.
* 
* Bottom line is that you have to NULL out an unneeded environ manually.
* 
* The environment array is able to be jerked any which way you'd like,
* using direct string/pointer manipulations.  However, for safety:
* make a local copy of **environ.
* 
********************************************************************/



/*
 * Common suffixes for icon pixmap and icon bitmap paths.  Each array must end
 * with a null pointer.  Note: the unusual string concatenation is necessary
 * in order to prevent expansion of SCCS id keywords.
 */

static const char *iconPmSuffixes[] = {
    "%B" "%M.pm",
    "%B" "%M.bm",
    "%B",
    NULL
};

static const char *iconBmSuffixes[] = {
    "%B" "%M.bm",
    "%B" "%M.pm",
    "%B",
    NULL
};


/*
 * makeDefaultIconPath
 * 
 * Construct the icon pixmap or bitmap search path by constructing the values
 * from the home directory, and then appending the system default path.  The
 * output is of the form "var=value", suitable for passing to putenv().
 *
 * Output:
 *
 * outbuf	A pointer to the memory to receive the output value.  This
 *		must be large enough to accommodate the largest anticipated
 *		environment variable; no bounds checking is done!
 * 
 * Inputs:
 * 
 * varname	The environment variable name.
 * basedefault	The default value to be appended to the end.
 * suffixes	An array of suffixes used in constructing the path elements in
 *		the user's home directory.  Must have a trailing NULL pointer.
 */
static char *
makeDefaultIconPath(
	const char	*varname,
	const char	*basedefault,
	const char	**suffixes)
{
	char		*fmt = "%s/.dt/icons/%s:";
	char		*homedir;
	char		*outbuf;
	int		bytes_needed, fmt_bytes, homedir_bytes;

	bytes_needed = strlen(varname) + strlen(basedefault) + 2;

	homedir = getenv("HOME");
	if (NULL != homedir)
	{
	    homedir_bytes = strlen(homedir);
	    fmt_bytes = strlen(fmt);
	    for ( ; *suffixes ; ++suffixes)
	      bytes_needed += (homedir_bytes + fmt_bytes + strlen(*suffixes));
	}

	outbuf = XtMalloc(bytes_needed);
	(void)sprintf(outbuf, "%s=", varname);
	if (homedir != NULL)
	{
	    char *temp = XtMalloc(bytes_needed);

	    for ( ; *suffixes ; ++suffixes)
	    {
		(void)sprintf(temp, fmt, homedir, *suffixes);
		(void)strcat(outbuf, temp);
	    }

	    if (temp) XtFree(temp);
	}
	(void)strcat(outbuf, basedefault);

	return outbuf;
}


/*
 * Construct a default value for XMICONSEARCHPATH into outbuf.
 */
static char *
makeDefaultIconPmPath()
{
    static char *defaultIconPmPath = NULL;

    if (NULL == defaultIconPmPath)
      defaultIconPmPath =
	makeDefaultIconPath(PM_PATH_ENVIRON, DTPMSYSDEFAULT, iconPmSuffixes);
    
    return XtNewString(defaultIconPmPath);
}


/*
 * Construct a default value for XMICONBMSEARCHPATH into outbuf.
 */
static char *
makeDefaultIconBmPath()
{
    static char *defaultIconBmPath = NULL;

    if (NULL == defaultIconBmPath)
      defaultIconBmPath =
	makeDefaultIconPath(BM_PATH_ENVIRON, DTBMSYSDEFAULT, iconBmSuffixes);
    
    return XtNewString(defaultIconBmPath);
}


/*****************************<->*************************************
 *
 *  _DtEnvSessionManager ()
 *
 *
 *  Description:
 *  -----------
 *  Sets the SESSION_MANAGER environment variable if not already set.
 *
 *  Inputs:
 *  ------
 *  NONE
 *
 *  Returns:
 *  --------
 *  void
 * 
 *****************************<->***********************************/
void
_DtEnvSessionManager()
{
#define SESSION_MANAGER	"SESSION_MANAGER"
    char *session_manager = getenv(SESSION_MANAGER);
    if (NULL == session_manager)
    {
	Display	*display;
        Atom sm_atom;

	display = XOpenDisplay(NULL);
	if (NULL != display)
	{
            sm_atom = XInternAtom(display, SESSION_MANAGER, True);
	    if (None != sm_atom)
	    {
    	        Atom actual_type;
    	        unsigned long nitems, leftover;
    	        int actual_format;

	        if (Success == XGetWindowProperty(
				display, XDefaultRootWindow(display),
				sm_atom, 0L, 256, False, XA_STRING,
				&actual_type, &actual_format,
				&nitems, &leftover,
				(unsigned char **) &session_manager))
	        {
	            if (NULL != session_manager && None != actual_format)
		    {
		        char *envstr;
			envstr = (char*) malloc(
						strlen(SESSION_MANAGER) +
						strlen(session_manager) + 2);
			sprintf(
				envstr, "%s=%s",
				SESSION_MANAGER,
				session_manager);
	                putenv(envstr);
	                XtFree(session_manager);
		    }
	        }
	    }
	    XCloseDisplay(display);
	}
    }
}

/*****************************<->*************************************
 *
 *  _DtEnvControl ( int mode )
 *
 *
 *  Description:
 *  -----------
 *  Gets/Sets the application's environment 
 *
 *  Inputs:
 *  ------
 *  mode        - the command to the function; e.g., to set the
 *                DT environment, to restore the originial env., etc.
 *
 *  Returns:
 *  --------
 *  Returns a success code--usually just the value of the original
 *  mode parameter.  If an error occurs, returns DT_ENV_NO_OP.
 * 
 *****************************<->***********************************/
int
_DtEnvControl(
        int mode )
{
    static int environSetup = 0;

    char *tempString;  
    char *ptr; /* used for indexing into the $PATH */
    int returnValue = DT_ENV_NO_OP;
    int bytes_needed;

    _DtSvcProcessLock();
    switch (mode) {

       	case DT_ENV_SET:
	    if (!environSetup)  /* first time through */
	    {
		/*
		 * Make sure the SESSION_MANAGER variable is set.
		 */
		_DtEnvSessionManager();

		/*
		 * Set up DT environment in the application 
		 * environment, while stashing the old environment
		 * in the _preDtEnvironment structure 
		 *
		 * Note: this code will not check for duplicate
		 * environment values--it will append or prepend
		 * the DT environment values regardless of what's
		 * in the current environment.
		 *
		 *  Of the form:
		 *
		 *  PATH = /opt/dt/bin :  originalPath
		 *  %s  %s    %s        %s    %s
		 *
		 *
		 *  XBMLANGPATH = originalPath : dtPath
		 *        %s    %s    %s       %s    %s
		 *
		 *  Note: Of all the environmental vars, ONLY $PATH
		 *  is jammed with the DT value first.
		 *
		 *  Check for NULL environment strings before the sprintf
		 *
		 */

		/*
		 * Get the application's original environment 
		 * to save it in the _preDtEnvironment structure 
		 *
		 * We save in the _preDtEnvironment structure
		 * only existing (non-NULL getenv()) values.
		 *
		 */

		/*
		 * Prepend BIN_PATH_STRING to the PATH component.
		 */
		tempString = getenv(BIN_PATH_ENVIRON);

		/*
		 *  First, ensure that BIN_PATH_STRING isn't already there.
		 */
		if (!tempString || !(strstr(tempString, BIN_PATH_STRING)))
		{
		    if (!tempString)
		    {
			/* 
			 * No existing PATH environment variable.
			 * Just create the new DT environment.
			 */
			bytes_needed =
			  strlen(BIN_PATH_ENVIRON) + strlen(BIN_PATH_STRING) +2;

		        _postDtEnvironment.binPath = XtMalloc(bytes_needed);
			sprintf(_postDtEnvironment.binPath,
				"%s=%s",
				BIN_PATH_ENVIRON,
				BIN_PATH_STRING);
		    }
		    else
		    {
		        /*
			 * Save the existing PATH.
			 */
			bytes_needed =
			  strlen(BIN_PATH_ENVIRON) + strlen(tempString) + 2;

			_preDtEnvironment.binPath = XtMalloc(bytes_needed);
			sprintf(_preDtEnvironment.binPath,
				"%s=%s",
			    	BIN_PATH_ENVIRON,
			    	tempString);

			bytes_needed =
			 	strlen(tempString) +
				strlen(BIN_PATH_ENVIRON) +
			        strlen(BIN_PATH_STRING) + 4;

		        _postDtEnvironment.binPath = XtMalloc(bytes_needed);
#ifdef sun
			if ((ptr = strstr(tempString, "/usr/openwin/bin")))
#elif defined(CSRG_BASED)
			if ((ptr = strstr(tempString, "/usr/X11R6/bin")))
#elif defined(linux)
			if ((ptr = strstr(tempString, "/usr/bin")))
#else
			if ((ptr = strstr(tempString, "/usr/bin/X11")))
#endif
			{
			     /* 
			      * Shorten the string in tempString
			      * to the initial segment, up to the 
			      * initial slash in "/usr/bin/X11"
			      */
			     if (ptr != tempString)
			     {
			        /* 
				 * then put our dt string just ahead of 
				 * "/usr/bin/X11" in the new PATH 
				 */
				 *(ptr - 1) = '\0'; 
				 sprintf(_postDtEnvironment.binPath,
				   "%s=%s:%s:%s",
				    BIN_PATH_ENVIRON,
				    tempString,
				    BIN_PATH_STRING,
				    ptr);
			     }
			     else
			     {
				/*
				 *  Turns out that "/usr/bin/X11"
				 *  is at the front of the PATH, so...
				 */
				sprintf(_postDtEnvironment.binPath,
				    "%s=%s:%s",
				    BIN_PATH_ENVIRON,
				    BIN_PATH_STRING,
				    tempString);
			    }

			}
			else if (ptr = strstr(tempString, "/usr/bin"))
			{
			     /* 
			      * Shorten the string in tempString
			      * to the initial segment, up to the 
			      * initial slash in "/usr/bin"
			      */
			     if (ptr != tempString)
			     {
			        /* 
				 * then put our dt string just ahead of 
				 * "/usr/bin" in the new PATH 
				 */

				 *(ptr - 1) = '\0'; 
				 sprintf(_postDtEnvironment.binPath,
				   "%s=%s:%s:%s",
				    BIN_PATH_ENVIRON,
				    tempString,
				    BIN_PATH_STRING,
				    ptr);
			     }
			     else
			     {
				/*
				 *  Turns out that "/usr/bin"
				 *  is at the front of the PATH, so...
				 */
				sprintf(_postDtEnvironment.binPath,
				    "%s=%s:%s",
				    BIN_PATH_ENVIRON,
				    BIN_PATH_STRING,
				    tempString);
			    }
			}
			else
			{
			    /* 
			     * Put our dt string on the front of the PATH 
			     */
			    sprintf(_postDtEnvironment.binPath,
				"%s=%s:%s",
				BIN_PATH_ENVIRON,
				BIN_PATH_STRING,
				tempString);
			}
		    }

		    _EnvAdd (BIN_PATH_ENVIRON, 
			       _postDtEnvironment.binPath, 
			       BV_BINPATH);
		}


		tempString = getenv(PM_PATH_ENVIRON);
		if (tempString)
		{
		    bytes_needed =
		      strlen(PM_PATH_ENVIRON) + strlen(tempString) + 2;

		    _preDtEnvironment.pmPath = XtMalloc(bytes_needed);
		    sprintf(_preDtEnvironment.pmPath,
			"%s=%s",
			PM_PATH_ENVIRON,
			tempString);
		}
		else
		{
		    /* it doesn't exist, so generate a default value */
		    _postDtEnvironment.pmPath = makeDefaultIconPmPath();
		    _EnvAdd(PM_PATH_ENVIRON, _postDtEnvironment.pmPath,
			BV_PMPATH);
		}


		tempString = getenv(BM_PATH_ENVIRON);
		if (tempString)
		{
		    bytes_needed =
		      strlen(BM_PATH_ENVIRON) + strlen(tempString) + 2;

		    _preDtEnvironment.bmPath = XtMalloc(bytes_needed);
		    sprintf(_preDtEnvironment.bmPath,
			"%s=%s",
			BM_PATH_ENVIRON,
			tempString);
		}
		else
		{
		    /* it doesn't exist, so generate a default value */
		    _postDtEnvironment.bmPath = makeDefaultIconBmPath();
		    _EnvAdd(BM_PATH_ENVIRON, _postDtEnvironment.bmPath,
			BV_BMPATH);
		}


		/* Do the admin for the NLSPATH env variable */
		tempString = getenv(NLS_PATH_ENVIRON);
		if (!tempString)
		{
		    /* If it doesn't exist, set it to the CDE default */
		    bytes_needed =
		      strlen(NLS_PATH_ENVIRON) + strlen(NLS_PATH_STRING) + 2;

		    _postDtEnvironment.nlsPath = XtMalloc(bytes_needed);
		    sprintf(_postDtEnvironment.nlsPath,
			"%s=%s",
			NLS_PATH_ENVIRON,
			NLS_PATH_STRING);
		}
		else
		{
		    /* If it does exist, store it away so it can be
		     * restored afterwards.....
		     */
		    bytes_needed =
		      strlen(NLS_PATH_ENVIRON) + strlen(tempString) + 2;

		    _preDtEnvironment.nlsPath = XtMalloc(bytes_needed);
		    sprintf(_preDtEnvironment.nlsPath,
			"%s=%s",
			NLS_PATH_ENVIRON,
			tempString);

		    /* ... then append the CDE default to the existing
		     * value
		     */
		    bytes_needed =
		    	strlen(NLS_PATH_ENVIRON) +
			strlen(tempString) +
			strlen(NLS_PATH_STRING) + 3;

		    _postDtEnvironment.nlsPath = XtMalloc(bytes_needed);
		    sprintf(_postDtEnvironment.nlsPath,
			"%s=%s:%s",
			NLS_PATH_ENVIRON,
			tempString,
			NLS_PATH_STRING);
		}

		_EnvAdd(NLS_PATH_ENVIRON, _postDtEnvironment.nlsPath,
			   BV_NLSPATH);


		tempString = getenv(SYSTEM_APPL_PATH_ENVIRON);
		if (!tempString)
		{
		    bytes_needed =
			strlen(SYSTEM_APPL_PATH_ENVIRON) +
			strlen(SYSTEM_APPL_PATH_STRING) + 2;

		    _postDtEnvironment.sysApplPath = XtMalloc(bytes_needed);
		    sprintf(_postDtEnvironment.sysApplPath,
			"%s=%s",
			SYSTEM_APPL_PATH_ENVIRON,
			SYSTEM_APPL_PATH_STRING);
		}
		else
		{
		    bytes_needed =
		      strlen(SYSTEM_APPL_PATH_ENVIRON) + strlen(tempString) + 2;

		    _preDtEnvironment.sysApplPath = XtMalloc(bytes_needed);
		    sprintf(_preDtEnvironment.sysApplPath,
			"%s=%s",
			SYSTEM_APPL_PATH_ENVIRON,
			tempString);

		    bytes_needed =
			strlen(SYSTEM_APPL_PATH_ENVIRON) +
			strlen(tempString) +
			strlen(SYSTEM_APPL_PATH_STRING) + 3;

		    _postDtEnvironment.sysApplPath = XtMalloc(bytes_needed);
		    sprintf(_postDtEnvironment.sysApplPath,
			"%s=%s:%s",
			SYSTEM_APPL_PATH_ENVIRON,
			tempString,
			SYSTEM_APPL_PATH_STRING);
		}

		_EnvAdd (SYSTEM_APPL_PATH_ENVIRON,
			   _postDtEnvironment.sysApplPath,
			   BV_SYSAPPLPATH);

		environSetup = 1;
	        returnValue = DT_ENV_SET;
	    }
	    else /* we've already been here -- do nothing */
	    {
		returnValue = DT_ENV_NO_OP;
	    }
	    break;

	case  DT_ENV_RESTORE_PRE_DT:
       	    if (environSetup) 
            {
		if (_preDtEnvironment.nlsPath) {
		    _EnvAdd (NLS_PATH_ENVIRON, 
			       _preDtEnvironment.nlsPath,
			       BV_NLSPATH);
		}
		else {
		    _DtEnvRemove(NLS_PATH_ENVIRON, 0);
		    envBitVector &= ~BV_NLSPATH;
		}

		if (_preDtEnvironment.sysApplPath) {
		    _EnvAdd (SYSTEM_APPL_PATH_ENVIRON, 
			       _preDtEnvironment.sysApplPath,
			       BV_SYSAPPLPATH);
		}
		else {
		    _DtEnvRemove(SYSTEM_APPL_PATH_ENVIRON, 0);
		    envBitVector &= ~BV_SYSAPPLPATH;
		}

		if (_preDtEnvironment.pmPath) {
		    _EnvAdd (PM_PATH_ENVIRON,
			       _preDtEnvironment.pmPath,
			       BV_PMPATH);
		}
		else {
		    _DtEnvRemove(PM_PATH_ENVIRON, 0);
		    envBitVector &= ~BV_PMPATH;
		}

		if (_preDtEnvironment.bmPath) {
		    _EnvAdd (BM_PATH_ENVIRON,
			       _preDtEnvironment.bmPath,
			       BV_BMPATH);
		}
		else {
		    _DtEnvRemove(BM_PATH_ENVIRON, 0);
		    envBitVector &= ~BV_BMPATH;
		}

	        returnValue = DT_ENV_RESTORE_PRE_DT;
            }
            else
	    {
		returnValue = DT_ENV_NO_OP;
	    }
	    break;

	case DT_ENV_RESTORE_POST_DT:
       	    if (environSetup) 
            {

		if (_postDtEnvironment.nlsPath) {
		    _EnvAdd (NLS_PATH_ENVIRON, 
			       _postDtEnvironment.nlsPath,
			       BV_NLSPATH);
		}

		if (_postDtEnvironment.pmPath) {
		    _EnvAdd (PM_PATH_ENVIRON,
			       _postDtEnvironment.pmPath,
			       BV_PMPATH);
		}

		if (_postDtEnvironment.bmPath) {
		    _EnvAdd (BM_PATH_ENVIRON,
			       _postDtEnvironment.bmPath,
			       BV_BMPATH);
		}

		if (_postDtEnvironment.sysApplPath) {
		    _EnvAdd (SYSTEM_APPL_PATH_ENVIRON, 
			       _postDtEnvironment.sysApplPath,
			       BV_SYSAPPLPATH);
		}

	        returnValue = DT_ENV_RESTORE_POST_DT;
	    }
            else
	    {
		returnValue = DT_ENV_NO_OP;
	    }
	    break;

	default:
	    /* do nothing */
	    break;
    }

    _DtSvcProcessUnlock();

#ifdef DEBUG
	switch (mode) {
	case DT_ENV_SET:
		printf("DT environment set:\n");
		printf("-------------------------------\n");
		break;

	case DT_ENV_RESTORE_PRE_DT:
		printf("Pre-DT environment restored:\n");
		printf("-------------------------------\n");
		break;

	case DT_ENV_RESTORE_POST_DT:
		printf("Post-DT environment restored:\n");
		printf("-------------------------------\n");
		break;

	case DT_ENV_NO_OP:
		printf("No change from last invocation:\n");
		printf("-------------------------------\n");
		break;
	}

	tempString = getenv(BIN_PATH_ENVIRON);
	printf("%s=|%s|\n", BIN_PATH_ENVIRON, tempString);
	tempString = getenv(SYSTEM_APPL_PATH_ENVIRON);
	printf("%s=|%s|\n", SYSTEM_APPL_PATH_ENVIRON, tempString);
	tempString = getenv(NLS_PATH_ENVIRON);
	printf("%s=|%s|\n", NLS_PATH_ENVIRON, tempString);
	tempString = getenv(PM_PATH_ENVIRON);
	printf("%s=|%s|\n", PM_PATH_ENVIRON, tempString);
	tempString = getenv(BM_PATH_ENVIRON); 
	printf("%s=|%s|\n", BM_PATH_ENVIRON, tempString);

#endif /* DEBUG */
	return (returnValue);

} /* END OF FUNCTION _DtEnvControl */

/*****************************<->*************************************
 *
 *  _EnvAdd(char * str)
 *
 *
 *  Description:
 *  -----------
 *  Adds the given string to the application's environment
 *  If the existing environment variable string was allocated
 *  by this application, the space is freed.
 *
 *  Inputs:
 *  ------
 *  envVar          The environment variable. E.g. NLSPATH
 *  envVarSetting   The environment variable setting. 
 *                  E.g. NLSPATH=/usr/lib/nls
 *  bv_flag         The constant denoting which environment variable
 *                  is being set.
 * 
 *****************************<->***********************************/
static void _EnvAdd
  (
   char * envVar,
   char * envVarSetting,
   unsigned int bv_flag
  )
{
  _DtSvcProcessLock();
  if (envBitVector & bv_flag) {
#if defined(CSRG_BASED) || defined(linux)
      setenv(envVar, envVarSetting + strlen(envVar) + 1, 1);
#else

      register int i;
      size_t envVarLen = strlen(envVar);
      char *envPtr = NULL;

    /* if we have previously put this environment variable out to the
     * environment, we can retrieve it and reuse it rather than letting
     * it disappear into the ether
     */

    for ( i = 0; environ[i] ; i++ )
    {
        if ( environ[i][0] == envVar[0]
             && ( strlen(environ[i]) >= envVarLen )
             && ( environ[i][envVarLen] == '=' ) 
             && !strncmp(envVar,environ[i],envVarLen))
        {
		envPtr = environ[i];
		break;
        }
    }
    
    if ( envPtr )
    {
            XtFree(envPtr);
            envPtr = strdup(envVarSetting);
	    strcpy(envPtr,envVarSetting);
	    environ[i] = envPtr;
    }
    else
    {
            /* This should never happen */
	    putenv(strdup(envVarSetting));
    }
#endif /* linux || CSRG_BASED */
  }
  else
    putenv(strdup(envVarSetting));

  envBitVector |= bv_flag;
  _DtSvcProcessUnlock();
}

/*****************************<->*************************************
 *
 *  _DtEnvRemove(
 *      char *str,
 *      int length )
 *
 *
 *  Description:
 *  -----------
 *  Removes the given string from the application's environment
 *  Uses the char **extern environment string array.
 *
 *  Inputs:
 *  ------
 *  str         - the name of the variable to remove (e.g., "NLSPATH")
 *  length      - the length of the variable--we compute it dynamically.
 *
 *  Returns:
 *  --------
 *  Returns a success code--"0" if the variable is found and removed.
 *                          "1" if the variable isn't found.
 * 
 *****************************<->***********************************/
int
_DtEnvRemove(
        char *str,
        int length )
{
	char **pEnviron, **pEnviron2 = environ;
	char *p, *freeMe;
	int temp;
			 
	int count  = 0;  /* count is the number of items in the */
			 /* environ                             */
	int index;   /* index will range from 0 to count - 1 */
        size_t len;

	if (!(len = strlen(str)))
		return(1);

	pEnviron = pEnviron2;

	p = *pEnviron;

	while (p)
	{
		pEnviron++;
		count++;
		p = *pEnviron;
	}

	pEnviron = pEnviron2;
	p = *pEnviron;

	for  (index = 0; index < count; index++)
	{
		if (  p[0] == str[0] 
                      && ( strlen(p) >= len )
                      && ( p[len] == '=' )
                      && !strncmp(p, str, len))
		{
#if defined(linux) || defined(CSRG_BASED)
		  /* JET - 2/19/99
		     It seems much safer to let libc worry about this
		     rather than try to do it ourselves.  
		     */

		  if (str)
		    unsetenv(str);
#else

		    freeMe = pEnviron2[index];

		    /* just move the last one into the gap - any
		     * putenv destroyed the initial lexical ordering
		     * anyway
		     */

		    pEnviron2[index] = pEnviron2[count - 1];
		    pEnviron2[count - 1] = NULL;

		    XtFree (freeMe);
#endif /* linux || CSRG_BASED */
		    return(0);
		}
		pEnviron++;
		p = *pEnviron;
	}
	return(1);
}

#if 0
int
_EnvPrint( void )
{
	char **p, *q;

	p = (char **)(environ);

	q = *p;

	while (q)
	{
		printf("%s\n", q);
		p++;
		q = *p;
	}
	return 0;
}
#endif


/*************************************<->*************************************
 *
 *  _AddToPath (sPath, sDir)
 *
 *
 *  Description:
 *  -----------
 *  Append a directory to a directory path. 
 *
 *  Inputs:
 *  ------
 *  sPath	- the old path (must be a pointer to malloc'ed memory)
 *  sDir	- the directory to add to the path
 *
 *
 *  Returns:
 *  --------
 *  A pointer to the new path if successful.
 *  NULL on memory allocation failures.
 *
 *  Comment:
 *  --------
 *  This functions adds sDir to the path of directories already in
 *  sPath. Each directory is separated by a colon (':'). The function
 *  is designed to work off the heap. sPath should be a pointer into
 *  the heap on entry, or NULL. If sPath is NULL, then new memory is
 *  allocated and returned. If sPath is not NULL, the storage for sPath
 *  is reallocated to include space for the new string. In this case,
 *  the pointer returned may point to a different value than sPath
 *  that was passed in. If the pointer returned is different than sPath,
 *  then sPath is invalid and should be reassigned.
 *
 *************************************<->***********************************/
static char *
_AddToPath (
	char * sPath,
	char * sDir)
{
    char * sNew;

    if (sPath != NULL)
    {
	sNew = XtRealloc (sPath, 1+strlen(sPath)+1+strlen(sDir));
    }
    else
    {
	sNew = XtMalloc (1+strlen(sPath)+1+strlen(sDir));
    }

    strcat (sNew, ":");
    strcat (sNew, sDir);

    return (sNew);
}


/*************************************<->*************************************
 *
 *  _DtWsmSetBackdropSearchPath(screen, backdropPath, useMultiColorIcons)
 *
 *
 *  Description:
 *  -----------
 *  Append the specified directories to DTICONSEARCHPATH environment 
 *  variable.  This must be done before any icon lookup by XmGetPixmap.
 *  Each directory in backdropPath is modified so the backdrop file
 *  names can be matched (%B, %M, etc.).
 *
 *  _DtEnvControl( DT_ENV_RESTORE_PRE_DT ) will restore the original
 *  environment.
 *
 *  Inputs:
 *  ------
 *  screen		- screen of display 
 *  backdropPath	- a colon-separated list of directories
 *  useMultiColorIcons 	- True if color backdrops are desired.
 *
 *  Returns:
 *  --------
 *  Returns a success code:   success: DT_ENV_SET
 *                            failure: DT_ENV_NO_OP
 *
 *************************************<->***********************************/
int
_DtWsmSetBackdropSearchPath (
	Screen  *screen,
        char    *backdropPath,
        Boolean  useMultiColorIcons)

{
    char *sBackDirs;
    char *sOldEnv;
    char *sNext;
    char *bm_pm_fmt = "%s/%%B.bm:%s/%%B.pm:%s/%%B";  
    char *pm_bm_fmt = "%s/%%B.pm:%s/%%B.bm:%s/%%B";  
    int fmtlen = strlen(pm_bm_fmt);
    int returnValue = DT_ENV_NO_OP;
    int bytes_needed;
    _Xstrtokparams strtok_buf;

    /* for creating the name=value string */
    char postDtEnvironmentStringBuf[MAX_ENV_STRING]; 
    char *postDtEnvironmentString;

    if (backdropPath == NULL) return (returnValue);

    sOldEnv = getenv(PM_PATH_ENVIRON);

    DtNlInitialize();

    _DtSvcProcessLock();
    if (!sOldEnv)
    {
	/* 
	 * The user has no existing value for this
	 * environment variable.  Don't save it.
	 * Just create the new DT environment.
	 */

	/* make a copy that we can modify locally */
	sBackDirs = strdup(backdropPath);
	strcpy (sBackDirs, backdropPath);

	/*
	 * Initialize path string
	 */
	if (useMultiColorIcons)
	  _postDtEnvironment.pmPath = makeDefaultIconPmPath();
	else
	  _postDtEnvironment.pmPath = makeDefaultIconBmPath();

	/* get first directory */
	sNext = _XStrtok(sBackDirs, ":", strtok_buf);

	/* process each directory individually */
	while (sNext != NULL)
	{
	    bytes_needed = (3 * (strlen(sNext) + fmtlen));
	    if (bytes_needed >= MAX_ENV_STRING)
	      postDtEnvironmentString = XtMalloc(bytes_needed);
	    else
	      postDtEnvironmentString = postDtEnvironmentStringBuf;

	    if (useMultiColorIcons)
	      sprintf(postDtEnvironmentString, pm_bm_fmt, sNext, sNext, sNext);
	    else
	      sprintf(postDtEnvironmentString, bm_pm_fmt, sNext, sNext, sNext);

	    _postDtEnvironment.pmPath = 
	      _AddToPath(_postDtEnvironment.pmPath, postDtEnvironmentString);

	    if (postDtEnvironmentString != postDtEnvironmentStringBuf)
	    {
	        if (postDtEnvironmentString) XtFree(postDtEnvironmentString);
		postDtEnvironmentString = NULL;
	    }

	    /* get next directory */
	    sNext = _XStrtok(NULL, ":", strtok_buf);
	}

	XtFree (sBackDirs);
    }
    else
    {
	/* 
	 * Save the old path if not previously saved
	 */
	if (!_preDtEnvironment.pmPath)
	{
	    bytes_needed = strlen(PM_PATH_ENVIRON) + strlen(sOldEnv) + 2;
	    _preDtEnvironment.pmPath = XtMalloc(bytes_needed);
	    sprintf(_preDtEnvironment.pmPath,"%s=%s", PM_PATH_ENVIRON, sOldEnv);
	}

	/* make a copy that we can modify locally */
	sBackDirs = strdup(backdropPath);
	strcpy (sBackDirs, backdropPath);

	/*
	 * Initialize path environment string
	 */
	bytes_needed = strlen(PM_PATH_ENVIRON) + strlen(sOldEnv) + 2;
	_postDtEnvironment.pmPath = XtMalloc(bytes_needed);
	sprintf(_postDtEnvironment.pmPath, "%s=%s",  PM_PATH_ENVIRON, sOldEnv);

	/* get first directory */
	sNext = _XStrtok(sBackDirs, ":", strtok_buf);

	/* process each directory individually */
	while (sNext != NULL)
	{
	    bytes_needed = (3 * (strlen(sNext) + fmtlen));
	    if (bytes_needed >= MAX_ENV_STRING)
	      postDtEnvironmentString = XtMalloc(bytes_needed);
	    else
	      postDtEnvironmentString = postDtEnvironmentStringBuf;

	    if (useMultiColorIcons)
	      sprintf(postDtEnvironmentString, pm_bm_fmt, sNext, sNext, sNext);
	    else
	      sprintf(postDtEnvironmentString, bm_pm_fmt, sNext, sNext, sNext);

	    _postDtEnvironment.pmPath = 
	      _AddToPath(_postDtEnvironment.pmPath, postDtEnvironmentString);

	    if (postDtEnvironmentString != postDtEnvironmentStringBuf)
	    {
	        if (postDtEnvironmentString) XtFree(postDtEnvironmentString);
		postDtEnvironmentString = NULL;
	    }

	    /* get next directory */
	    sNext = _XStrtok(NULL, ":", strtok_buf);
	}

	XtFree (sBackDirs);
    }

    putenv(strdup(_postDtEnvironment.pmPath));
    envBitVector |= BV_PMPATH;


    /*
     * Now do the same thing for the bitmap lookup path
     */
    sOldEnv = getenv(BM_PATH_ENVIRON);
    if (!sOldEnv)
    {
	/* 
	 * The user has no existing value for this
	 * environment variable.  Don't save it.
	 * Just create the new DT environment.
	 */

	/* make a copy that we can modify locally */
	sBackDirs = strdup(backdropPath);
	strcpy (sBackDirs, backdropPath);

	/*
	 * Initialize path string
	 */
	_postDtEnvironment.bmPath = makeDefaultIconBmPath();

	/* get first directory */
	sNext = _XStrtok(sBackDirs, ":", strtok_buf);

	/* process each directory individually */
	while (sNext != NULL)
	{
	    bytes_needed = (3 * (strlen(sNext) + fmtlen));
	    if (bytes_needed >= MAX_ENV_STRING)
	      postDtEnvironmentString = XtMalloc(bytes_needed);
	    else
	      postDtEnvironmentString = postDtEnvironmentStringBuf;

	    sprintf(postDtEnvironmentString, bm_pm_fmt, sNext, sNext, sNext);

	    _postDtEnvironment.bmPath = 
	      _AddToPath(_postDtEnvironment.bmPath, postDtEnvironmentString);

	    if (postDtEnvironmentString != postDtEnvironmentStringBuf)
	    {
	        if (postDtEnvironmentString) XtFree(postDtEnvironmentString);
		postDtEnvironmentString = NULL;
	    }

	    /* get next directory */
	    sNext = _XStrtok(NULL, ":", strtok_buf);
	}

	XtFree (sBackDirs);
    }
    else
    {
	/* 
	 * Save the old path if not previously saved
	 */
	if (!_preDtEnvironment.bmPath)
	{
	    bytes_needed = strlen(BM_PATH_ENVIRON) + strlen(sOldEnv) + 2;
	    _preDtEnvironment.pmPath = XtMalloc(bytes_needed);
	    sprintf(_preDtEnvironment.pmPath,"%s=%s", BM_PATH_ENVIRON, sOldEnv);
	}

	/* make a copy that we can modify locally */
	sBackDirs = strdup(backdropPath);
	strcpy (sBackDirs, backdropPath);

	/*
	 * Initialize path environment string
	 */
	bytes_needed = strlen(BM_PATH_ENVIRON) + strlen(sOldEnv) + 2;
	_postDtEnvironment.bmPath = XtMalloc(bytes_needed);
	sprintf(_postDtEnvironment.bmPath, "%s=%s",  BM_PATH_ENVIRON, sOldEnv);

	/* get first directory */
	sNext = _XStrtok(sBackDirs, ":", strtok_buf);

	/* process each directory individually */
	while (sNext != NULL)
	{
	    bytes_needed = (3 * (strlen(sNext) + fmtlen));
	    if (bytes_needed >= MAX_ENV_STRING)
	      postDtEnvironmentString = XtMalloc(bytes_needed);
	    else
	      postDtEnvironmentString = postDtEnvironmentStringBuf;

	    sprintf(postDtEnvironmentString, bm_pm_fmt, sNext, sNext, sNext);

	    _postDtEnvironment.bmPath = 
	      _AddToPath(_postDtEnvironment.bmPath, postDtEnvironmentString);

	    if (postDtEnvironmentString != postDtEnvironmentStringBuf)
	    {
	        if (postDtEnvironmentString) XtFree(postDtEnvironmentString);
		postDtEnvironmentString = NULL;
	    }

	    /* get next directory */
	    sNext = _XStrtok(NULL, ":", strtok_buf);
	}

	XtFree (sBackDirs);
    }

    putenv(strdup(_postDtEnvironment.bmPath));
    envBitVector |= BV_BMPATH;

    _DtSvcProcessUnlock();
    returnValue = DT_ENV_SET;
    return (returnValue);

} /* END OF FUNCTION _DtWsmSetBackdropSearchPath */

/*********************         eof   *************************/
