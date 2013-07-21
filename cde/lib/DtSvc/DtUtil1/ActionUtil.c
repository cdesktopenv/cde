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
/* $TOG: ActionUtil.c /main/14 1999/02/19 13:10:09 mgreess $ */
/*************************************<+>*************************************
*****************************************************************************
**
**   File:         ActionUtil.c
**
**   Project:	    DT
**
**   Description:  This file contains the action library utility source code.
**		  
** (c) Copyright 1993, 1994 Hewlett-Packard Company
** (c) Copyright 1993, 1994 International Business Machines Corp.
** (c) Copyright 1993, 1994 Sun Microsystems, Inc.
** (c) Copyright 1993, 1994 Novell, Inc.
****************************************************************************
************************************<+>*************************************/

/*LINTLIBRARY*/
#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>	/* for *MAX* macros */
#define X_INCLUDE_NETDB_H
#define X_INCLUDE_GRP_H
#define XOS_USE_XT_LOCKING
#include <X11/Xos_r.h>

#ifdef _SUN_OS /* Need this for the strtod () call */
#include <floatingpoint.h>
#endif /* _SUN_OS */

#include <string.h>
#include <stdlib.h>
#include <limits.h>


#include <X11/Xlib.h>	/* for DisplayString */
#include <X11/Xresource.h>	/* for X resource defs */
#include <X11/Intrinsic.h>	/* for X toolkit functions */

#include <Dt/ActionP.h>
#include <Dt/ActionUtilP.h>
#include <Dt/CmdInvP.h>
#include <Dt/DtNlUtils.h>
#include <Dt/Indicator.h>

#include "myassertP.h"
#include "DtSvcLock.h"

/******************************************************************************
 *
 * List of active DtActionInvoke() DtActionInvocationID's
 *
 *****************************************************************************/

static _DtActInvRecT	**_DtActInvRecArray;		/* top of array */
static int		_actInvRecArraySize;		/* size of array */

static void	_DtActFreeInvRec();			/* free an inv rec */	
static void	 _DtActFreeChildRec( _DtActChildRecT *childp);

#ifndef NDEBUG
	/* I want to exercise the realloc code for now */
#define 		_START_INVREC_SIZE 1		/* initial array size */
#else
#define 		_START_INVREC_SIZE 32		/* initial array size */
#endif	/* NDEBUG */

#ifndef P_tmpdir
#define P_tmpdir	"/var/tmp"
#endif
/*******************************************************************************
 *  _DtBasename -- utiltiy function to return a pointer to a
 * string containing the basename of a file name.  It doesn't
 * modifiy the original string parameter. If the original file
 * path ends in "/" the last component of the path is returned as
 * the basename.
 ******************************************************************************/
char *
_DtBasename( const char *s ) 
{
	char *basep;
	char buf[MAXPATHLEN + MAXHOSTNAMELEN + 1];
	char *p = buf;


        if (!s)
		return NULL;

	/* Work on a local copy of the original string */
	*p = '\0';
	(void)strcpy(p,s);

#ifdef _Dt_HOST_COLON_PATH_SUPPORT
	/* Chop off the "host:" if necessary */
	if ( basep = DtStrchr(p,'/') )
	{
		/* if ( basep > p && (*(basep - 1) == ':') ) */
		if ( basep > p && ( *DtPrevChar(p,basep) == ':') )
		p = basep;
	}
#endif	/* _Dt_HOST_COLON_PATH_SUPPORT */

	if ( (basep = DtStrrchr(p,'/')) == NULL )
		return XtNewString(p);

	
	if (( basep == p) && (strlen(basep) == 1))
		return XtNewString("/");
	
	/*
	 * check for trailing slash
	 */
	/* while (basep == p+strlen(p)-1 ) */
	while (basep == DtPrevChar(p,p+strlen(p)) )
	{
                myassert( *basep == '/');
		if ( basep == p )
			return XtNewString(basep);
		*basep = '\0';	/* replace trailing slash */
		if ( (basep = DtStrrchr(p,'/')) == NULL )
			return XtNewString(p);
	}

	/* skip past '/' before returning basename */
	basep++;
	return XtNewString(basep);

}


/*******************************************************************************
 * _DtPathString -- returns the path portion of the "host:/path" string
 * passed in as a parameter.  
 * The incomming string is assumed to be in : "[host:]/path format.
 * NOTE: a FULL path name is required.
 ******************************************************************************/
char *
_DtPathname( const char *s)
{
	char *slashp;

	if ( !s )
	{
		myassert(0);
		return NULL;
	}

	/* Chop off the "host:" if necessary */
	slashp = DtStrchr((char *)s,'/'); 

	/* if ( slashp > s && (*(slashp - 1) != ':') ) */
	if ( slashp > s && (*DtPrevChar(s,slashp) != ':') )
	{
		/* 
		 *  full path name required
		 * --- should never get here
		 */
		myassert(0);
		return NULL;
	}

	return XtNewString(slashp);
}

/*******************************************************************************
 * _DtDirname -- returns the directory portion of the file
 * path string passed in as a parameter.  The original string
 * may be modified to remove trailing slashes.  
 * The incomming string is assumed to be in : "[host:/]dir/file" format.
 ******************************************************************************/
char *
_DtDirname( const char *s)
{
	char *slashp;
	char *dirp;
	char buf[MAXPATHLEN + MAXHOSTNAMELEN + 1];
	char *p = buf;;


	/* Work on a local copy of the original string */
	*p = '\0';
	(void)strcpy(p,s);

#ifdef _Dt_HOST_COLON_PATH_SUPPORT
	/* Chop off the "host:" if necessary */
	if ( slashp = DtStrchr(p,'/') )
	{
		/* if ( slashp > p && (*(slashp - 1) == ':') ) */
		if ( slashp > p && (*DtPrevChar(p,slashp) == ':') )
		p = slashp;
	}
#endif	/* _Dt_HOST_COLON_PATH_SUPPORT */

	/* handle multiple trailing slashes */

	while ( (slashp = DtStrrchr(p,'/')) )
	{

		/*
		 * Special case '/' -- return '/'
		 */
		if ( slashp == p ) 
			return XtNewString("/");

		/*
		 * Is this a trailing slash ?
		 * -- then try again else break
		 */
		if ( slashp == DtPrevChar(p,p + strlen(p)) )
			*slashp = '\0';
		else
			break;
	}
	/* malformed path */
	if (!slashp )
		return NULL;

	/*
	 * Replace the last '/' with a NULL to get the 
	 * directory name.
	 */
	dirp = XtNewString(p);
	*(dirp + (slashp - p)) = '\0';
	return dirp;
}

/*****************************************************************************
 *
 * _DtHostString
 *	Extract the host name string from files in the [host:][/][path/]file
 *	format.  This function mallocs space for a new copy of the host
 *	name string; it is up to the caller to free this space.
 *		This function requires that names which include the hoststring
 *	use a "full path" name to specify the file location.  If the hoststring
 *	is omitted the path may be relative.
 *
 *	RETURN
 *		If a host name string can be found; a pointer to a newly
 *		malloced copy of the host name string is returned; otherwise
 *		a NULL pointer is returned.
 *
 *	NOTE:  This function should NOT be used to extract host name strings
 *	       from display variables.
 *	
 *****************************************************************************/

char *
_DtHostString( const char *s)
{
	char *slashp;
	char *host;
	char buf[MAXPATHLEN + MAXHOSTNAMELEN + 1];
	char *p= buf;

	if ( (slashp = DtStrchr((char *)s,'/')) == NULL )
		return NULL;

	/*
	 * Make a local copy of the string to avoid problems modifying
	 * "const" strings.
	 */
	*p = '\0';
	(void) strcpy(p,s);

	/* if ( (slashp > s) && (*(slashp -1) == ':' )) */
	if ( (slashp > s) && (*DtPrevChar(s,slashp)  == ':' ))
	{
		/* *(p + (slashp - s - 1)) = NULL; */
                *(p + (DtPrevChar(s,slashp) - s)) = '\0';
		host = XtNewString(p);
		return host;
	}

	return NULL;
}


char *
_DtGetSessionHostName( void )
{
	static char *sessionHostName = NULL;

	_DtSvcProcessLock();
	if ( sessionHostName && *sessionHostName ) {
	        _DtSvcProcessUnlock();
		return XtNewString(sessionHostName);
	      }

	sessionHostName =  getenv(ENV_SESSION_SVR);

	if ( sessionHostName && *sessionHostName ) {
	        _DtSvcProcessUnlock();
		return	XtNewString(sessionHostName);
	      }

	/*
	 * Default to display host name if the session host environment
	 * variable is not set or set to null.  The command invoker
	 * initialization should have tucked away the display name.
	 */

	sessionHostName = _DtGetDisplayHostName( (Display *) NULL );
	_DtSvcProcessUnlock();
	return XtNewString(sessionHostName);

}

char *
_DtGetDisplayHostName( Display *dp)
{
	static char *displayHostName = NULL;
	char *tmpName = NULL;
	char *tmp;
	
	_DtSvcProcessLock();

	if ( displayHostName && *displayHostName ) {
	        _DtSvcProcessUnlock();
		return XtNewString(displayHostName); 
	      }

	tmpName = XtMalloc(MAXHOSTNAMELEN + 5);
	tmpName[0] = '\0';

	if ( dp )
	{
		/*
		 * If a display pointer has been provided use it to determine
		 * display host name.
		 */
		
		strcpy(tmpName,DisplayString(dp));
		if (( tmp = DtStrrchr(tmpName,':') ))
		{
			*tmp = '\0';
			displayHostName = XtNewString(tmpName);
		}
	} 
	else
	{
		/*
		 * As a last resort,
		 * In the absence of a display pointer, use the
		 * DISPLAY environment variable.
		 */
		strcpy(tmpName,getenv("DISPLAY"));
		if (( tmp = DtStrrchr(tmpName,':') ))
		{
			*tmp = '\0';
			displayHostName = XtNewString(tmpName);
		}
	}
	XtFree(tmpName);

	/*
         * Check for degenerate forms of the display name
         */
        if ( !(  displayHostName &&
              *displayHostName &&
              strcmp(displayHostName,"local") &&
              strcmp(displayHostName, "unix") ))
        {
            /*
             * default to localHostName
             */
             if (displayHostName)
                    XtFree(displayHostName);
             displayHostName = _DtGetLocalHostName();
        }

	myassert( displayHostName && *displayHostName );
	myassert( (DtStrchr( displayHostName, ':' ) == NULL) );

	_DtSvcProcessUnlock();
	return XtNewString(displayHostName); 
}

/******************************************************************************
 *
 * _DtGetLocalHostname:
 *	return the short form of the local host name.
 *	(i.e. truncate the hostname at the first '.' character).
 ******************************************************************************/

char *
_DtGetLocalHostName( void )
{
	static char *localHostName = NULL;
	static char hostNameBuf[MAXHOSTNAMELEN + 1];
	char *ptr;

	_DtSvcProcessLock();
	if ( localHostName && *localHostName ) {
	        _DtSvcProcessUnlock();
		return XtNewString(localHostName);
	      }

	if ( gethostname(hostNameBuf, sizeof(hostNameBuf)) ) {
	        _DtSvcProcessUnlock();
		return NULL; /* failed gethostname */
	      }
	if ((ptr = DtStrchr(hostNameBuf, '.')))
		*ptr = '\0';  /* delete domain name if there is one */

	localHostName = hostNameBuf;
	_DtSvcProcessUnlock();
	return XtNewString(localHostName);
}

/******************************************************************************
 *
 * _DtIsSameHost
 *	return True if the two host names provided are actually references to
 *	the same host; False otherwise.  If either host1 or host2 is a NULL
 *	pointer use the "local host" name.
 *
 ******************************************************************************/


int
_DtIsSameHost(const char *host1, const char *host2)
{
	char hostName1[MAXHOSTNAMELEN + 1];
	char hostName2[MAXHOSTNAMELEN + 1];
	struct hostent		*host_ret;
	_Xgethostbynameparams	host_buf;
	char *tp;

	/*
	 * If either parameter is null; use the local host name in its stead
	 */
	if ( !host1 )
	{
		tp = _DtGetLocalHostName();
		strcpy(hostName1,tp);
		XtFree(tp);
	}
	else
	{
		strcpy(hostName1,host1);
	}

	if ( !host2)
	{
		tp = _DtGetLocalHostName();
		strcpy(hostName2,tp);
		XtFree(tp);
	}
	else
	{
		strcpy(hostName2,host2);
	}

	/*
	 * We now have local copies of the hostnames in the
	 * arrays hostName1 and hostName2.  Truncate the names
	 * to their short form before doing the compare.
	 */
	if ( (tp = DtStrchr(hostName1,'.')) != NULL )
		*tp = '\0';
	if ( (tp = DtStrchr(hostName2,'.')) != NULL )
		*tp = '\0';

	/*
	 * Try to avoid querying the name server (or /etc/hosts).
	 * Do the name strings match?
	 */

	if ( !strcmp(hostName1,hostName2) )
		return True;

	if ( (host_ret = _XGethostbyname(hostName1, host_buf)) == NULL )
		return False;	/* treat them as different on failure */

	/*
	 * Save the data from gethostbyname() in "hostName1" so we can
	 * call gethostbyname() again without losing it.
	 */
	strcpy(hostName1, host_ret->h_name);
	if ( (tp = DtStrchr(hostName1,'.')) != NULL )
		*tp = '\0';

	/*
	 * Try comparing again -- avoiding another gethostbyname
	 * if successful.
	 */
	if ( !strcmp( hostName1,hostName2) )
		return True;

	/* restore the dot if necessary */
	if ( tp) *tp = '.';

	if ( (host_ret = _XGethostbyname(hostName2, host_buf)) == NULL )
		return False;	/* treat them as different on failure */

	if ( !strcmp(hostName1, host_ret->h_name) )
		return	True;	

	return False;	/* The names are different */
}


/******************************************************************************
 *
 * _DtGetActionIconDefault - 
 *	return the default action icon name string based on the "*ActionIcon"
 *      X resource and the DtACTION_ICON_DEFAULT value.
 *
 *****************************************************************************/

char *
_DtGetActionIconDefault ( void )
{
   static char *defaultActionIcon = NULL;
   char		nameBuf[_DtAct_MAX_BUF_SIZE];
   char		classBuf[_DtAct_MAX_BUF_SIZE];
   XrmValue 	resource_value;
   XrmDatabase 	db;
   char 	*rep_type;
   int		bytesNeeded;
   char	*name;
   char *class;

   _DtSvcProcessLock();
   if ( defaultActionIcon ) {
        _DtSvcProcessUnlock();
	return XtNewString(defaultActionIcon);
      }

   bytesNeeded = strlen(DtACTION_ICON_RESOURCE_NAME) 
                       + strlen(_DtApplicationName) + 4;
   if ( bytesNeeded > _DtAct_MAX_BUF_SIZE )
	name = XtMalloc(bytesNeeded);
   else
	name = nameBuf;

   sprintf (name, "%s*%s",
	_DtActNULL_GUARD( _DtApplicationName) , DtACTION_ICON_RESOURCE_NAME);


   bytesNeeded = strlen(DtACTION_ICON_RESOURCE_CLASS) 
                       + strlen(_DtApplicationClass) + 4;
   if ( bytesNeeded > _DtAct_MAX_BUF_SIZE )
	class = XtMalloc(bytesNeeded);
   else
	class = classBuf;
   sprintf (class, "%s*%s", 
	_DtActNULL_GUARD(_DtApplicationClass) , DtACTION_ICON_RESOURCE_CLASS);

   if(_DtDisplay)
        db = XtDatabase (_DtDisplay);
   else
	db = 0;
   if (db && XrmGetResource (db, nameBuf, classBuf, &rep_type, &resource_value))
      defaultActionIcon = (char *) XtNewString (resource_value.addr);
   else
      defaultActionIcon = (char *) XtNewString (DtACTION_ICON_DEFAULT);

   if ( name != nameBuf )
	XtFree(name);
   if ( class != classBuf )
	XtFree(class);

   _DtSvcProcessUnlock();
   return XtNewString(defaultActionIcon);
}


/******************************************************************************
 *
 * _DtGetExecHostsDefault - 
 *	Returns the default execution host string based on the "*executionHosts"
 *	X resource and the default vaule of DtEXEC_HOSTS_DEFAULT.
 *
 * PARAMETERS: None.
 *
 * RETURNS: char *
 *
 *****************************************************************************/

char *
_DtGetExecHostsDefault ( void )
{   
   static char *executionHosts = NULL;
   char		nameBuf[_DtAct_MAX_BUF_SIZE];
   char		classBuf[_DtAct_MAX_BUF_SIZE];
   XrmValue resource_value;
   XrmDatabase db;
   char *rep_type;
   char *name, *class;
   int bytesNeeded;

   _DtSvcProcessLock();
   if ( executionHosts ) {
        _DtSvcProcessUnlock();
	return XtNewString(executionHosts);
      }

   bytesNeeded = strlen(DtEXEC_HOSTS_NAME) + strlen(_DtApplicationName) + 4;

   if ( bytesNeeded > _DtAct_MAX_BUF_SIZE )
	name = XtMalloc(bytesNeeded);
   else
	name = nameBuf;
   sprintf (name, "%s*%s",
	_DtActNULL_GUARD(_DtApplicationName), DtEXEC_HOSTS_NAME);

  
   bytesNeeded = strlen(DtEXEC_HOSTS_CLASS) + strlen(_DtApplicationClass) + 4;
   if ( bytesNeeded > _DtAct_MAX_BUF_SIZE )
	class = XtMalloc(bytesNeeded);
   else
	class = classBuf;
   sprintf (class, "%s*%s",
	_DtActNULL_GUARD(_DtApplicationClass), DtEXEC_HOSTS_CLASS);

   db = XtDatabase (_DtDisplay);
   if (db && XrmGetResource (db, name, class, &rep_type, &resource_value))
      executionHosts = (char *) XtNewString (resource_value.addr);
   else
      executionHosts = (char *) XtNewString (DtEXEC_HOSTS_DEFAULT);

   if ( name != nameBuf )
	   XtFree (name);
   if ( class != classBuf )
	   XtFree (class);

   _DtSvcProcessUnlock();
   return XtNewString(executionHosts);
}


/******************************************************************************
 char *_DtGetDtTmpDir() 

    check resource; then go to internal default for value of Dt Tmp
    directory path.  This function returns a newly allocated string, it is
    up to the caller to free it.
 
 *****************************************************************************/
char *_DtGetDtTmpDir(void)
{
    static char *DtTmpDirPath = NULL;
    char *dirBuf = NULL;
    char nameBuf[_DtAct_MAX_BUF_SIZE];
    char classBuf[_DtAct_MAX_BUF_SIZE];
    char *name;
    char *class;
    int bytesNeeded;
    char	*rep_type;
    XrmValue	resource_value;
    XrmDatabase	db;

    _DtSvcProcessLock();
    if ( DtTmpDirPath ) {
            _DtSvcProcessUnlock();
	    return XtNewString(DtTmpDirPath);
	  }

    /*
     * Check if a resource has been set for the tmp dir location
     */
 
    bytesNeeded = strlen(DtACTION_DTTMPDIR_RESOURCE_NAME)
		   + strlen(_DtApplicationName) + 4;
    if ( bytesNeeded > _DtAct_MAX_BUF_SIZE )
	name = XtMalloc(bytesNeeded);
    else
        name = nameBuf;

    sprintf (name, "%s*%s",
        _DtActNULL_GUARD( _DtApplicationName) , DtACTION_DTTMPDIR_RESOURCE_NAME);


    bytesNeeded = strlen(DtACTION_DTTMPDIR_RESOURCE_CLASS)
                       + strlen(_DtApplicationClass) + 4;
    if ( bytesNeeded > _DtAct_MAX_BUF_SIZE )
        class = XtMalloc(bytesNeeded);
    else
        class = classBuf;
    sprintf (class, "%s*%s",
        _DtActNULL_GUARD(_DtApplicationClass) , DtACTION_DTTMPDIR_RESOURCE_CLASS);

    db = XtDatabase (_DtDisplay);
    if (db && XrmGetResource (db, nameBuf, classBuf, &rep_type, &resource_value))
      DtTmpDirPath = (char *) XtNewString (resource_value.addr);
    else
    {
      /* RWV: is this the right HOME if we've changed user id? */
      dirBuf = XtMalloc(MAXPATHLEN);
      strcpy(dirBuf,getenv("HOME"));
      strcat(dirBuf,"/");
      strcat(dirBuf,DtACTION_DTTMPDIR_DEFAULT);
      DtTmpDirPath = XtNewString(dirBuf);
      XtFree(dirBuf);
    }
    
    /*
     * Save a copy of the path for future reference
     */
    _DtSvcProcessUnlock();
    return XtNewString(DtTmpDirPath);
}

/*****************************************************************************
 *
 * _DtActGenerateTmpFile(char *dir, char *format)
 *	Generate a temporary file  in the directory and format specified.
 *      format is assumed to contain a single %s and to be in a form
 *	suitable for use by sprintf().  If  "dir" is not accessable or NULL
 *	then the default CDE tmp dir, the contents of the "TMPDIR" environment
 *	variable, the P_tmpdir defined in stdio.h and finally "/tmp" are 
 *	tried in turn.  
 *		This function returns a newly malloc-ed string containing a
 *	full path for a new tmp file.  The open file descriptor for the tmp
 *	file is returned through an int pointer (*fd).
 *
 *	If no unused tmp name can be generated (within 1000 tries) then this
 *      function returns NULL.
 *
 *****************************************************************************/

char *
_DtActGenerateTmpFile(char *dir,char *format,mode_t mode,int *fd)
{
        int pid; 
        static unsigned long nameCount = 0xA;
        int free_d = 0;
	int countTrys = 0;	/* count of the number of tmp names tried */
        char nameBuf[MAXPATHLEN];
        char *d = dir;
        char *f = format;
        char *base;
      
        struct stat statbuf;

        pid = getpid();

        if ( !d )
        {
                free_d = 1;
        	d = _DtGetDtTmpDir();
        }
        if ( !f )
                f = "%s";

        /*
         * Make sure the desired directory is avaliable
         * if not try P_tmpdir (i.e. /usr/tmp), finally resort
         * to "/tmp" if no other tmp dir can be accessed.
         */
        if ( stat(d,&statbuf) )
        {
		/* 
		 * The passed in directory cannot be accessed so
                 * try some alternatives.
                 */
		int i;
		static char *AltTmpDirs[] = {
                                        NULL,   /* reserved for getenv() */
                                    	P_tmpdir,	/* from stdio.h */
                                        "/tmp",
					NULL,
                                      }; 

		_DtSvcProcessLock();
		AltTmpDirs[0] = getenv("TMPDIR");

                if ( free_d ) 
                {
			XtFree(d);
                        free_d = 0;
                }

                /*
                 * Because "free_d" is false (i.e.0) at this point any
                 * pointer assigned to d from the static array AltTmpDirs
                 * is protected from being erroneously freed at the
                 * end of this function.
                 */
		myassert(free_d == 0);

		for ( i= 0; i < sizeof(AltTmpDirs)/sizeof(char *); i++ )
                {
			if ( !(d = AltTmpDirs[i]) )
				continue;
                	if ( stat(d,&statbuf) == 0 )
				break;
                }
		_DtSvcProcessUnlock();

                if ( !d )
                {
                        myassert(0 /* this should never happen */);
			return NULL;
                }

        }

        do {

	        _DtSvcProcessLock();
		sprintf(nameBuf,"%lx_%d",nameCount++,pid);
	        _DtSvcProcessUnlock();
		base = XtNewString(nameBuf);
		/*
		 * Convert the base name to the desired format
		 */
		sprintf(nameBuf,f,base);
		XtFree(base);

                /*
                 * If the format string does not allow for variation
                 * of the tmp file name (i.e. no %s in format) then
                 * there is no sense trying more than once.
                 */
                if ( countTrys > 0 && (strcmp(f,nameBuf) == 0))
                    return NULL;

		base = XtNewString(nameBuf);

		/*
		 * generate the full path name to the new tmp file
		 */ 
                /* if ( d[strlen(d)-1] != '/' ) */
                if ( *DtPrevChar(d,d + strlen(d)) != '/' )
			sprintf(nameBuf,"%s/%s",d,base);
                else
			sprintf(nameBuf,"%s%s",d,base);
                
		XtFree(base);

               /*
		* Check if such a file already exists.
                */
		*fd = open(nameBuf,(O_WRONLY | O_CREAT | O_EXCL), mode );
	}
	while ( *fd == -1 && errno == ENOENT && countTrys++ < 1000 );

	if ( free_d )
        	XtFree(d);

        if ( *fd == -1 )
		return NULL;	/* unable to generate desired name format */

	/* 
         * The file has been successfully created -- return the name for 
         * use by the caller.  The already open fd is also available to the
         * calling function.  It is up to the caller to close the file.
         */

        return XtNewString(nameBuf);
}

/*******************************************************************************
 * _DtRemoveTrailingBlanksInPlace
 *	Removes trailing white space from the passed string.  The string
 *	is modified in place.
 ******************************************************************************/
void
_DtRemoveTrailingBlanksInPlace(char **s)
{
	register char *p;

	if (!s || !strlen(*s))
		 return;

	for ( p = DtPrevChar(*s,*s + strlen(*s)); 
              DtIsspace(p) && (p > *s); 
              p=DtPrevChar(*s,p))
		*p = '\0';
	
}


/******************************************************************************
 *
 * _DtExecuteAccess ( path ) 
 *
 * PARAMETERES:
 *   char *path;	// path of potentially executable file
 *
 * For effective user id, 
 * RETURNS: True (1) if file is executable; 
 *	    False (0)if file is not executable;
 *	    -1 if the file cannot be accessed.
 *
 *****************************************************************************/


#ifdef NGROUPS_UMAX
#define NGROUPS_MAX_VALUE	NGROUPS_UMAX
#else
#define NGROUPS_MAX_VALUE	NGROUPS_MAX
#endif

int
_DtExecuteAccess( const char *path )
{
	int i, amode, rval;
        uid_t euid;
        struct stat s;
	gid_t *pgid;
	gid_t groupids[NGROUPS_MAX_VALUE];
	struct group *gr;
	_Xgetgrparams grp_buf;

        if (stat( path, &s ) == -1 ) {
		/* could not stat file, no access */ 
                return -1;
	}

	euid = geteuid(); 

        if( (S_IXUSR & s.st_mode) == S_IXUSR ) {
        	if(euid == s.st_uid || euid == 0) {
                	/* execution permitted for user */
                	return  1;
		}
        } else {
		if (euid == s.st_uid && euid != 0) {
			/* user execution not permitted */
			return 0;
		}
	}

        if( (S_IXGRP & s.st_mode) == S_IXGRP ) {
        	if(getegid() == s.st_gid || euid == 0) {
                	/* execution permitted for group (or superuser) */
                	return  1;
		}

		i = getgroups(getgroups(0,groupids), groupids);

		if ( i > 0) {
			for (pgid = groupids; i--; pgid++) {
				if ((gr = _XGetgrgid(*pgid, grp_buf)) != NULL)
				    if (gr->gr_gid == s.st_gid) {
					/* execution permitted to group list */
			 		return 1;
				}
			}
		}
	}
 
        if( (S_IXOTH & s.st_mode) == S_IXOTH ) {
                /* execution permitted for "others" */
                return  1;
	}

	/* no access */

        return 0;
}

/******************************************************************************
 *
 * Routines to manipulate DtActionInvocationID's.
 */

/**************************************************
 *
 * Allocate an unused DtActionInvocationID
 * between 1..INT_MAX
 */
DtActionInvocationID _DtActAllocID()
{
    extern _DtActInvRecT   **_DtActInvRecArray;		/* global */
    extern int             _actInvRecArraySize;		/* global */

    static unsigned long   lastIdWas = 100;		/* 0..99 for errors */
    int                    i, found;


    _DtSvcProcessLock();
    do {
        found = 1;

	/*
	 * Need to track down better define than INT_MAX to determine cap
	 */
	if ( lastIdWas == INT_MAX )
	    lastIdWas = 1;
	else
	    lastIdWas++;

	/*
	 * Verify that the ID is not being used already
	 */
	for ( i = 0 ; i < _actInvRecArraySize ; i++ ) {
	    if ( _DtActInvRecArray[i] ) {
		if ( (_DtActInvRecArray[i] -> id) == lastIdWas )
		    found = 0;
	    }
	}
    } while ( ! found );
    
    _DtSvcProcessUnlock();
    return( lastIdWas );
}


/**************************************************
 *
 * Allocate an Invocation Record
 */
_DtActInvRecT *_DtActAllocInvRec()
{
    extern _DtActInvRecT   **_DtActInvRecArray;		/* global */
    extern int             _actInvRecArraySize;		/* global */

    int                    i, newslot;
    static int             first_time = 1;


    _DtSvcProcessLock();
    /*
     * If first time, malloc array of InvRec pointers
     */
    if (first_time) {
	_actInvRecArraySize = _START_INVREC_SIZE;
	_DtActInvRecArray = (_DtActInvRecT **) XtMalloc(sizeof(_DtActInvRecT *)
							* _actInvRecArraySize);

	/*
	 * A NULL indicates an available slot.
	 */
	for ( i = 0; i < _actInvRecArraySize; i++ )
	    _DtActInvRecArray[i] = NULL;

	first_time = 0;
    }

    /*
     * Look through existing list of InvRec's for an available slot.
     */
    newslot = -1;
    for ( i = 0; i < _actInvRecArraySize; i++ ) {
	if ( _DtActInvRecArray[i] == NULL ) {
	    newslot = i;
	    break;
	}
    }

    if ( newslot == -1 ) {
	/*
	 * Need to grow InvRecArray since current one is full.
	 */
	_actInvRecArraySize += 10;
	_DtActInvRecArray = (_DtActInvRecT **)
				XtRealloc( (char *) _DtActInvRecArray,
						sizeof(_DtActInvRecT *)
						* _actInvRecArraySize   );

	/*
	 * NULL out new entries
	 */
	for ( i = _actInvRecArraySize-10; i < _actInvRecArraySize; i++ )
	    _DtActInvRecArray[i] = NULL;

	newslot = _actInvRecArraySize-10;
    }

    /*
     * Hang a new InvRec off the array and initialize all to zero.
     */
    _DtActInvRecArray[newslot] = (_DtActInvRecT *)
                                        XtCalloc(1, sizeof(_DtActInvRecT) );
    _DtActInvRecArray[newslot]->id =  _DtActAllocID();
    _DtSvcProcessUnlock();

    return( _DtActInvRecArray[newslot] );
}

/******************************************************************************
 *
 * _DtActFreeChildRec()
 *
 * Completely free the contents of, and free the existance of a childRec.
 *
 *****************************************************************************/

static void
_DtActFreeChildRec( _DtActChildRecT *childRecP)
{
    CallbackData *data;


    XtFree((char *)childRecP->argMap);

    if ( IS_CMD( childRecP->mask ) ) {
	XtFree((char *) childRecP->u.cmd.TTProcId);

	if (childRecP->u.cmd.reqMessage) {
	    data = (CallbackData *)
		tt_message_user(childRecP->u.cmd.reqMessage,0);

	    if (data) {
		XtFree((char *) data->actionLabel);

		/*
		 * tjg: question if we should do this
		 *
		 * if (data->actionPtr)
		 *      _DtFreeActionStruct( data->actionPtr );
		 */

		if (data->requestPtr)
		    _DtFreeRequest(data->requestPtr);

		XtFree((char *) data);
	    }

	    tttk_message_destroy(childRecP->u.cmd.reqMessage);
	}
    }
    else if ( IS_TT_MSG( childRecP->mask ) ) {
	tt_free((char *) childRecP->u.tt.TTProcId);

	if (childRecP->u.tt.reqMessage) {
	    data = (CallbackData *)
		tt_message_user(childRecP->u.tt.reqMessage,0);

	    if (data) {
		XtFree((char *) data->actionLabel);

		/*
		 * tjg: question if we should do this
		 *
		 * if (data->actionPtr)
		 *      _DtFreeActionStruct( data->actionPtr );
		 */

		if (data->requestPtr)
		    _DtFreeRequest(data->requestPtr);

		XtFree((char *) data);
	    }
	    tttk_message_destroy(childRecP->u.tt.reqMessage);
	}
    }

    XtFree( (char *) childRecP );
}

/******************************************************************************
 *
 * _DtActDeleteChildRec()
 *
 * Within an existing invRec, delete one of its children.   Squeeze the
 * childRec array if needed (but don't bother to realloc smaller).
 * 
 *	Returns 1 for successful deletion.
 *	Returns 0 if unable to delete the record.
 *
 ******************************************************************************/
int _DtActDeleteChildRec( _DtActInvRecT *invp, _DtActChildRecT *childp)
{
	register int i,j;

	if ( !invp || !childp )
		return 0;

	for ( i = 0; i < invp->numChildren; i++ )
	{
		if ( invp->childRec[i] == childp )
		{
			_DtActFreeChildRec(childp);
			invp->numChildren--;
			invp->childRec[i] = NULL;
			/*
			 * Close the potential gap created in the array.
			 */
			for ( j = i; j < invp->numChildren; j++)
			{
				invp->childRec[i] = invp->childRec[i+1];
				invp->childRec[i+1] = NULL;
			}
			return 1;
		}
	} 
	/* child not found */
	return 0;
}

/******************************************************************************
 *
 * _DtActDeleteInvRec( id )
 * Delete an Action Invocation Record given an invocation id
 *	Returns 1 for successful deletion.
 *	Returns 0 if unable to delete the record.
 *	Returns -1 if the record is not found.
 *
 * note: as pointers are free'd, they should be set to NULL
 *
 ******************************************************************************/
int _DtActDeleteInvRec( DtActionInvocationID id )
{
    register int i;
    CallbackData *data;

    _DtSvcProcessLock();
    for ( i = 0; i < _actInvRecArraySize; i++ ) 
    {
	if ( _DtActInvRecArray[i] ) 
        {
	    if ( (_DtActInvRecArray[i]->id) == id ) 
            {
                _DtActInvRecT *invp  = _DtActInvRecArray[i];
                _DtActArgInfo *infop;
                int j;
               
                /*
                 * Check for any tmp files created to house buffers
                 * Delete them if they still exist.
                 */

                for ( j = 0; j < invp->ac; j++)
                {
                    infop = &invp->info[j];

                    if (IS_BUFFER_OBJ(infop->mask) && IS_FILE_OBJ(infop->mask))
                    {
                        myassert((infop->name != NULL) && (*infop->name != '\0'));
                        if ( !infop->name )
                            continue;
                        /*
                         * These tmp names should have been created by
                         * the execution management code and such names
                         * should not refer to directories. We will not
                         * check that assertion here.
                         */
                        if ( !IS_WRITE_OBJ(infop->mask) )
                        {
                             mode_t mode = ( S_IRUSR|S_IRGRP|S_IWUSR|S_IWGRP);
                             /*
                              * Change the file's mode before deleting
                              */
                             chmod(infop->name,mode);
                        }
                        (void)unlink(infop->name);
                        RESET_FILE_OBJ(infop->mask);
                    } 
 
                }

		/*
		 * Cleanup invocation record.  Don't worry about squeezing
		 * and realloc'ing the _DtActInvRecArray.
		 */
                _DtActFreeInvRec( _DtActInvRecArray[i] );
		_DtActInvRecArray[i] = NULL;
		_DtSvcProcessUnlock();
		return 1; /* successfully deleted */
	    }
	}
    }

    _DtSvcProcessUnlock();
    return -1;	/* not found */	
}

/**************************************************
 *
 * Allocate a Child Record; hang off parent
 */
_DtActChildRecT *_DtActAllocChildRec( _DtActInvRecT *invRec )
{
    _DtActChildRecT        *tchildRec;


    if ( invRec ) {
	(invRec->numChildren)++;
	if ( invRec->numChildren == 1 ) {
	    /*
	     * First child.  Build array.
	     */
	    invRec->childRec = (_DtActChildRecT **)
					XtMalloc( sizeof(_DtActChildRecT *) );
	}
	else {
	    invRec->childRec = (_DtActChildRecT **)
					XtRealloc( (char *) invRec->childRec,
					    sizeof(_DtActChildRecT *) *
					    invRec->numChildren);
	}

	/*
	 * Hang a child rec off and initialize
	 */
	invRec->childRec[invRec->numChildren - 1] = (_DtActChildRecT *)
				XtMalloc( sizeof(_DtActChildRecT) );

	tchildRec = invRec->childRec[invRec->numChildren - 1];	/* shorthand */

        memset(tchildRec, 0, sizeof(_DtActChildRecT));
	tchildRec->childId     = invRec->numChildren;	/* serial # of sorts */

	/* tchildRec->u.*      = initialized elsewhere */

	return( (_DtActChildRecT *) tchildRec );
    }
    else {
	return( (_DtActChildRecT *) NULL );	/* should not happen */
    }
}


/**************************************************
 *
 * Given an ID, find a DtActInvRec.
 */
_DtActInvRecT *_DtActFindInvRec( DtActionInvocationID id )
{
    extern _DtActInvRecT   **_DtActInvRecArray;
    extern int             _actInvRecArraySize;

    int i;

    _DtSvcProcessLock();
    for ( i = 0; i < _actInvRecArraySize; i++ ) {
	if ( _DtActInvRecArray[i] ) {
	    if ( (_DtActInvRecArray[i]->id) == id ) {
	        _DtSvcProcessUnlock();
		return( _DtActInvRecArray[i] );		/* match */
	    }
	}
    }

    _DtSvcProcessUnlock();
    return( (_DtActInvRecT *) NULL );			/* no match */
}

/**************************************************
 *
 * Given an ID and a childId, find a DtActChildRec.
 */
_DtActChildRecT *_DtActFindChildRec( 
	DtActionInvocationID id,
	unsigned long        childId)
{
    int           i;
    _DtActInvRecT *invRec;


    if ( (invRec = _DtActFindInvRec( id )) == NULL )
	return( (_DtActChildRecT *) NULL );		/* no match */

    for ( i = 0; i < invRec->numChildren; i++ ) {
	if ( invRec->childRec[i] ) {
	    if ( (invRec->childRec[i]->childId) == childId ) {
		return( invRec->childRec[i] );		/* match */
	    }
	}
    }

    return( (_DtActChildRecT *) NULL );			/* no match */
	  
}

/******************************************************************************
 *
 * _DtActFreeInvRec( p )
 * 	Free a given action invocation record.
 *
 *****************************************************************************/

static void
_DtActFreeInvRec( _DtActInvRecT *invp )
{
    register int i;


    /*
     *  Free info argMap of action arguments.
     */
    if ( invp->info ) {
         for ( i=0; i < invp->ac; i++ ) {
             XtFree(invp->info[i].type);
             XtFree(invp->info[i].name);
         }    
         XtFree((char *)invp->info);
         invp->info = NULL;
    }
    

    /*
     * Free up cached data if any.   This is a rude move since it should
     * have been expected to be uploaded by now.
     */
    if (invp->cachedUploadCnt) {
	for ( i = 0; i < invp->cachedUploadCnt; i++ ) {
	    _DtActFreeArgArray( invp->cachedUploads[i].newArgp,
			        invp->cachedUploads[i].newArgc);
	}
	XtFree( (char *) invp->cachedUploads );
    }

    /*
     * Free up child records if necessary.
     */
    for ( i = 0; i < invp->numChildren; i++ ) {
	/*
	 * Remove each child
	 */
	_DtActFreeChildRec( invp->childRec[i] );
    }
    XtFree( (char *) invp->childRec );

    XtFree((char *)invp);
}

/******************************************************************************
 *
 * Add an update to the invocation upload cache.
 */
static void _DtActCacheArgs(
    _DtActInvRecT   *invRecP,
    DtActionArg     *newArgp,
    int             newArgc,
    DtActionStatus  userStatus )
{

    /*
     * Grow the cache.  We'll add to it one by one, but when a cache
     * flush happens later, all entries will go and be freed.
     */
    if ( invRecP->cachedUploadCnt )
	invRecP->cachedUploads = (_DtActUpdateCache *)
				 XtRealloc( (char *) invRecP->cachedUploads,
					    (invRecP->cachedUploadCnt+1) *
					    sizeof(_DtActUpdateCache) );
    else
	invRecP->cachedUploads = (_DtActUpdateCache *)
				 XtMalloc( sizeof(_DtActUpdateCache) );

    invRecP->cachedUploads[invRecP->cachedUploadCnt].newArgp = newArgp;
    invRecP->cachedUploads[invRecP->cachedUploadCnt].newArgc = newArgc;
    invRecP->cachedUploads[invRecP->cachedUploadCnt].userStatus = userStatus;

    (invRecP->cachedUploadCnt)++;
}

/******************************************************************************
 *
 * Routine to evaluate the done-ness of an invocation session.
 *
 * This is the key evalulation routine that should be used at all
 * execution leaf nodes to decide if and how an "invocation session"
 * should be shutdown.
 *
 *****************************************************************************/
unsigned long _DtActEvalChildren(DtActionInvocationID id)
{
	_DtActInvRecT *invRec;
	unsigned long cstats;
	int i;


	invRec = _DtActFindInvRec( id );
	myassert(invRec);

	if ( IS_INV_FINISHED(invRec->state) ) { 
	    /*
	     * All children have been launched (FINISHED), or we're done
	     * launching new children (CANCEL), so now look at the status
	     * of each child to figure out what to do.
	     */

	    if ( invRec->numChildren ) {
		cstats = 0;
		for ( i = 0; i < invRec->numChildren; i++ ) {
		    cstats |= invRec->childRec[i]->childState;
		}
            }
	    else {
		/*
		 * The invocation is finished, and yet there are no
		 * children.  Probably the result of a cancel, so
		 * cobble up a child status of _DtActCHILD_CANCELED.
		 */
		cstats = _DtActCHILD_CANCELED;
	    }

	    /*
	     * Return worst case information on child.
	     */
	    if ( cstats & _DtActCHILD_UNKNOWN )
					return( _DtActCHILD_UNKNOWN );
	    else if ( IS_CHILD_PENDING_START(cstats) )
					return ( _DtActCHILD_PENDING_START );
	    else if ( IS_CHILD_ALIVE_UNKOWN( cstats ) )
					return ( _DtActCHILD_ALIVE_UNKNOWN );
	    else if ( IS_CHILD_ALIVE( cstats ) )
					return ( _DtActCHILD_ALIVE );
	    else if ( IS_CHILD_FAILED( cstats ) )
					return ( _DtActCHILD_FAILED );
	    else if ( IS_CHILD_CANCELED( cstats ) )
					return ( _DtActCHILD_CANCELED );
	    else if ( IS_CHILD_DONE( cstats ) )
					return ( _DtActCHILD_DONE );
	    else
					return( _DtActCHILD_UNKNOWN );
	}
	else {
	    /*
	     * Not all the children have been launched, so no need to do
	     * an analysis.
	     */
	    return( _DtActCHILD_UNKNOWN );
	}
}

/******************************************************************************
 *
 * _DtActExecutionLeafNodeCleanup()
 *
 * At natural execution path leafs within the DtAction code,
 * _DtActExecutionLeafNodeCleanup() should be called.
 *
 * See rev 1.19 to see how DtActionQuit() functionality was
 * once integrated into this routine.
 *
 * Basic shutdown process:
 *
 *    1. If not in a DtACTION_DONE* state and there are no arguments,
 *       do nothing.
 *
 *    2. If not in a DtACTION_DONE* state and there are arguments,
 *       generate a DtACTION_STATUS_UPDATE with arguments.
 *
 *    3. If in a DtACTION_DONE* state with or without arguments,
 *       generate a DtACTION_DONE* with or without arguments,
 *       and do a total shutdown of the entire invocation session.
 */
void _DtActExecutionLeafNodeCleanup(
    DtActionInvocationID  id,
    DtActionArg           *newArgp,
    int                   newArgc,
    int                   respectQuitBlock )
{
    unsigned long  evalStatus;
    DtActionStatus userStatus;
    _DtActInvRecT *invRecP;
    int            flushCache, useCache;
    int            i;

    flushCache = 0;	/* flush the cache 1st */
    useCache   = 0;	/* must cache */

    invRecP = _DtActFindInvRec( id );
    if ( !invRecP ) {
	/*
	 * The whole invocation session is already down.
	 *
	 * One possible way that this can happen is a fast
	 * tt_reply that triggers a DtACTION_DONE *before*
	 * the _DtActTimerCB() routine can go off.
	 *
	 * Free any args since they won't be going anywhere.
	 */
	_DtActFreeArgArray( newArgp, newArgc );
	return;
    }

    if ( !IS_INV_ID_RETURNED(invRecP->state) )
	useCache = 1;		/* Need to cache */
    else if (invRecP->cachedUploadCnt)
	flushCache = 1;		/* Can do uploads 1st, if any */

    /*
     * See if we still need to generate the DtACTION_INVOKED
     * update.
     */
    if (IS_INV_FINISHED(invRecP->state)) {
        if ( IS_INV_INDICATOR_ON(invRecP->state) )
        {
	    /* Turn off the activity indicator */
            _DtSendActivityDoneNotification();
            RESET_INV_INDICATOR_ON(invRecP->state);
        }
	if ( CALL_INV_CB(invRecP->state) ) {
	    if ( invRecP->cb  ) {
	        (invRecP->cb)( id, invRecP->client_data,
		    (DtActionArg *) NULL, 0,
		    DtACTION_INVOKED );
            }
	    SET_INV_CB_CALLED(invRecP->state);
	}
    }

    /*
     * See if we have any cache stuff to flush
     */
    if (flushCache) {
	for ( i = 0; i < invRecP->cachedUploadCnt; i++ ) {
	    if (invRecP->cb) {
		(invRecP->cb)( id, invRecP->client_data,
				invRecP->cachedUploads[i].newArgp,
				invRecP->cachedUploads[i].newArgc,
				invRecP->cachedUploads[i].userStatus );
	    }
	    else {
	  	/*
		 * We normally won't have cached data if no cb.
		 */
		_DtActFreeArgArray( invRecP->cachedUploads[i].newArgp,
				    invRecP->cachedUploads[i].newArgc);
	    }
	}

	XtFree( (char *) invRecP->cachedUploads );
	invRecP->cachedUploadCnt = 0;
    }

    /*
     * Data compression
     *
     * If 'newArgp' is nothing but DtACTION_NULLARGS, free it and the
     * following code will respond correctly.
     *
     * If the user did not register a callback, then free 'newArgp'
     * since it won't be going anywhere.
     */
    if ( !(invRecP->cb) ) {
	_DtActFreeArgArray( newArgp, newArgc );
	newArgc = 0;
    }
    else {
	if (newArgp) {
	    for ( i = 0; i < newArgc; i++ ) {
		if ( newArgp[i].argClass != DtACTION_NULLARG ) {
		    break;
		}
	    }
            if ( i == newArgc ) {
                _DtActFreeArgArray( newArgp, newArgc );
		newArgc = 0;
                newArgp = NULL;
	    }
	}
    }
	
    /*
     * See what the overall invocation session status is given the
     * child's possible status change.
     */
    evalStatus = _DtActEvalChildren(id);
    switch (evalStatus) {
	case _DtActCHILD_DONE:
	    userStatus = DtACTION_DONE;
	    break;
	case _DtActCHILD_CANCELED:
	    userStatus = DtACTION_CANCELED;
	    break;
	case _DtActCHILD_FAILED:
	    userStatus = DtACTION_FAILED;
	    break;
	default:
	    /*
	     * This is pseudo correct.   The setting here just causes us
	     * to dive into other test conditions.
	     */
	    userStatus = DtACTION_STATUS_UPDATE;
	    break;
    }

    /****************************************************************
     *
     * Even though the invocation session appears "done", the
     * following test case may convert us into doing just
     * an "update".
     */
    if ( (useCache) && (userStatus != DtACTION_STATUS_UPDATE) ) {
	/*
	 * The session is done, but we're in a caching situation,
	 * so convert this "done" status to an "update" status.
	 * When we get out of the caching situation, another evaluation
	 * (by way of a XtTimer) will re-generate the "done" status
	 * for us.
	 */
	userStatus = DtACTION_STATUS_UPDATE;
    }

    if ( userStatus == DtACTION_STATUS_UPDATE ) {
	/*
	 * Some sort of DtACTION_STATUS_UPDATE state.
	 *
	 * Suppress giving an update if there are no arguments - it's
	 * a waste of effort.
	 */
	if ( (newArgc) && (invRecP->cb) ) {
	    if (useCache) {
		_DtActCacheArgs( invRecP, newArgp, newArgc, userStatus );
	    }
	    else {
		(invRecP->cb)(  id, invRecP->client_data,
				newArgp, newArgc,
				userStatus );
	    }
	}
    }
    else {
	/*
	 * Some sort of DtACTION_DONE* state
	 *
	 * We are not under a caching condition, so we must handle the
	 * shutdown here.  No matter whether we have new arguments, we
	 * need to deliver the DtACTION_DONE* status.
	 */
	if ( invRecP->cb ) {
	    if (useCache) {
		_DtActCacheArgs( invRecP, newArgp, newArgc, userStatus );
	    }
	    else {
		(invRecP->cb)( id, invRecP->client_data,
				   newArgp, newArgc,
				   userStatus );
	    }
	}

	/*
	 * Delete the entire invocation session.  "The Final Act"!
	 */
	_DtActDeleteInvRec( id );
    }
}

/******************************************************************************
 *
 * Create an array of N DtACTION_NULLARG's.
 */
DtActionArg *_DtActMallocEmptyArgArray(int ac)
{
    DtActionArg *newArgP;
    int i;

    if ( ac == 0 )
	return NULL;

    newArgP = (DtActionArg *) XtCalloc( ac, sizeof(DtActionArg) );

    for ( i = 0; i < ac; i++ ) {
	newArgP[i].argClass = DtACTION_NULLARG;
    }

    return(newArgP);
}

/******************************************************************************
 *
 * Free an array of N DtActionArg's.
 */
void _DtActFreeArgArray( DtActionArg *argp, int ac )
{
    int i;

    if (argp) {
	for ( i = 0; i < ac; i++ ) {
	    if ( argp[i].argClass == DtACTION_FILE ) {
		XtFree( argp[i].u.file.name );
	    }
	    else if ( argp[i].argClass == DtACTION_BUFFER ) {
		XtFree( (char *) argp[i].u.buffer.bp );
		XtFree( argp[i].u.buffer.type );
		XtFree( argp[i].u.buffer.name );
	    }
	}
	XtFree( (char *) argp );
    }
}

/*****************************************************************************
 *
 * _DtActReadTmpFileToBuffer( fname, sizep )
 *
 *	Read the contents of the tmp file named: "fname" into a buffer.
 *	Return a pointer to the buffer and fill in "sizep" with the number
 *	of bytes consumed by the buffer.
 *
 *	In case of error return a NULL pointer and zero size.
 *
 *****************************************************************************/

void *
_DtActReadTmpFileToBuffer( char *fname, int *sizep )
{
   /*
    * Read in the current contents of the temp file into a
    * buffer for return to original caller.
    */
    int fd;
    int bytes;
    int size;
    int space;
    char *buf;

    if ( !fname )
    {
        /* should never get here */
        myassert(fname != NULL);
        *sizep = 0;
        return NULL;
    }

    if ( (fd = open(fname,O_RDONLY)) < 0 )
    {
	/* couldn't read tmp file */
	/* myassert( fd >= 0 ); */
        *sizep = 0;
        return NULL;
    }

    buf = (char *) XtMalloc(MAX_BUF_SIZE);
    for (size=0, space=MAX_BUF_SIZE;
	 (bytes=read(fd,buf+size,space)) != 0;
         size += bytes  )
    {
	if ( bytes < 0 )
	{
	    myassert(0 /* read error on tmp file */);
	    break;  /* return as much as we got */
	}
	if ( (space -= bytes) <= 0 )
	{
	    /*
	     * Time to allocate more space
	     */
	    buf= (char *)XtRealloc((char *)buf,size + bytes + MAX_BUF_SIZE);
	    space = MAX_BUF_SIZE;
	}
	
    }
    
    close(fd);
    *sizep = size;
    return XtRealloc((char *)buf,size);
}

/*****************************************************************************
 *
 * _DtActRetCmdChildArgs(childp, aargv)
 *
 *	Create an argment vector containing the returnable arguments for
 *	a command action child.  The vector pointer is passed in "aargv"
 *      space for it should already have been allocated by the caller.
 *      The number of returned arguments is returned by the function.
 *
 *****************************************************************************/

int
_DtActGetCmdReturnArgs( 
    DtActionInvocationID invId,
    _DtActChildRecT *childp,
    DtActionArg     **aargv )
{
    register int i;
    _DtActInvRecT	*invp;
    DtActionArg		*newArgp;

    if ( !(invp = _DtActFindInvRec(invId)) )
    {
        myassert( 0 /* this should never happen */ );
        *aargv = NULL;
        return 0;
    }

    /*
     * Allocate enough arguments args for the original invocation
     */

    *aargv = newArgp =  _DtActMallocEmptyArgArray( invp->ac );

    for ( i = 0; i < childp->numObjects; i++ )
    {
        _DtActArgInfo	*infop;
        int Idx;

        Idx = childp->argMap[i].argIdx;
        infop = &invp->info[Idx];
    
        if ( !IS_WRITE_OBJ(infop->mask) )
	    continue;	/* only return writable objectes */

        if ( IS_BUFFER_OBJ( infop->mask) )
        {
            if ( IS_FILE_OBJ( infop->mask) )
            {
               newArgp[Idx].argClass = DtACTION_BUFFER;
               newArgp[Idx].u.buffer.type = XtNewString(infop->type);
               /* 
                * RWV: 
                *     If did the following, the returned buffer name
                *     may NOT be the same as the name which was passed
                *     in to us (e.g. we were unable to create a tmp file
                *     with the desired name).  At present there is nothing
                *     that preseves the original buffer name.
                *
                *   newArgp[Idx].u.buffer.name = _DtBasename(infop->name);
                *
                *     instead we'll set the returned name field to NULL; this
                *     is after all a new copy of the buffer.
                */
               newArgp[Idx].u.buffer.name = NULL;
               newArgp[Idx].u.buffer.writable = True;
               newArgp[Idx].u.buffer.bp = 
                   _DtActReadTmpFileToBuffer( infop->name,
                       &newArgp[Idx].u.buffer.size );
            }
            else
            {
                /* 
                 * We already have the buffer in memory
                 *  -- THIS SHOULD NEVER BE THE CASE FOR COMMAND ACTIONS--
                 */
                myassert( 0 /* should never get here */ );
            }

        }
        else if ( IS_FILE_OBJ( infop->mask) )
        {
            /*
             * RWV
             * What about file objects passed in with the String qualifier?
             * This returns the original string string which may or may not
             * have been a file name.  Are we OK here?
             */
            newArgp[Idx].argClass = DtACTION_FILE;
            newArgp[Idx].u.file.name = XtNewString( infop->name );
        } else
	    myassert( 0 /* unsupported object */ );
    }
    
    return invp->ac;
}

/*
 * Check the command invoker's queued commands for an instance of
 * the given invocation id.
 */

int
_DtCmdCheckQForId(DtActionInvocationID id)
{
        extern Cmd_RequestQueue * _DtCmdGetQueueHead( void );
	Cmd_RequestQueue *pNode;
        CallbackData *dp;

	for ( pNode = _DtCmdGetQueueHead();
	      pNode; pNode = pNode->next )
        {
                dp = (CallbackData *)(pNode->success_data);
		if ( dp->requestPtr->invocId == id )
			return 1;	/* found a match */
        }

        /* Unable to find matching invocation id in the queue */
	return 0;
}
