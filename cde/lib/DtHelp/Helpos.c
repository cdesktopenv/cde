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
/* $TOG: Helpos.c /main/13 1998/07/30 12:08:32 mgreess $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        Dtos.c
 **
 **   Project:     Rivers Project
 **
 **   Description: All system/os dependent calls are placed in here. 
 ** 
 **
 ** WARNING:  Do NOT put any Xt or Xm dependencies in this code.
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 ****************************************************************************
 ************************************<+>*************************************/


#include <sys/param.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define X_INCLUDE_PWD_H
#define XOS_USE_XT_LOCKING
#include <X11/Xos_r.h>
#include <unistd.h>
#ifndef _SUN_OS /* don't need the nl_types.h file */
# include <nl_types.h>
#endif /* ! _SUN_OS */

#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <Xm/Xm.h>

/* Dt Includes */
#include <Dt/Help.h>
#include <Dt/DtNlUtils.h>

#include "HelpP.h"
#include "HelpI.h"
#include "HelposI.h"
#include "Lock.h"

#ifndef NL_CAT_LOCALE
static const int NL_CAT_LOCALE = 0;
#endif



/* Global Message Catalog file names */
static char *CatFileName=NULL;



/*****************************************************************************
 * Function:	   Boolean _DtHelpOSGetHomeDirName(
 *
 *
 * Parameters:    Output string, size of output string 
 *
 * Return Value:    String.
 *
 *
 * Description: 
 *
 *****************************************************************************/
void _DtHelpOSGetHomeDirName(
    String outptr,
    size_t len)

/* outptr is allocated outside this function, just filled here. */
/* this solution leads to less change in the current (mwm) code */
{
   int uid;
   static char *ptr = NULL;
   _Xgetpwparams	pwd_buf;
   struct passwd *	pwd_ret;

    _DtHelpProcessLock();
    if (ptr == NULL) {
	if((ptr = (char *)getenv("HOME")) == NULL) {
	    if((ptr = (char *)getenv("USER")) != NULL) 
		pwd_ret = _XGetpwnam(ptr, pwd_buf);
	    else {
		uid = getuid();
		pwd_ret = _XGetpwuid(uid, pwd_buf);
	    }
	    if (pwd_ret != NULL) 
		ptr = pwd_ret->pw_dir;
	    else 
		ptr = NULL;
	}
    }

    if (ptr) {
        strncpy(outptr, ptr, len);
        outptr[len-1] = '\0';		/* Make sure it is Null terminated */
    }
    else 
        outptr[0] = '\0' ;
    _DtHelpProcessUnlock();
}






/*****************************************************************************
 * Function:	   _DtHelpGetUserSearchPath(
 *
 *
 * Parameters:     
 *
 * Return Value:    String, owned by caller.
 *
 *
 * Description: 
 *                  Gets the user search path for use
 *                  when searching for a volume.
 *                  Takes path from the environment, 
 *                  or uses the default path. 
 *
 *****************************************************************************/
String _DtHelpGetUserSearchPath(void)
{
  String path;
  char homedir[MAXPATHLEN];
  String localPath;
  extern char * _DtCliSrvGetDtUserSession();  /* in libCliSrv */

   localPath = (char *)getenv (DtUSER_FILE_SEARCH_ENV);
   if (localPath  == NULL) 
   {
       char * session;

       /* Use our default path */
       _DtHelpOSGetHomeDirName(homedir, sizeof(homedir));
       session = _DtCliSrvGetDtUserSession();

       path = calloc(1, 3*strlen(session) + 6*strlen(homedir) +
                        strlen(DtDEFAULT_USER_PATH_FORMAT));
       sprintf(path, DtDEFAULT_USER_PATH_FORMAT, 
                     homedir, session, homedir, session, homedir, session,
                     homedir, homedir, homedir);
       free(session);
       /* homedir is a local array */

       /* Return our expanded default path */
       return(path);
   }
   else
   {
       /* Make a local copy for us */
       localPath = strdup(localPath);

       /* Just Use our local path */
       return (localPath);
   }

}



/*****************************************************************************
 * Function:	   _DtHelpGetSystemSearchPath(
 *
 *
 * Parameters:     
 *
 * Return Value:    String, owned by caller.
 *
 *
 * Description: 
 *                  Gets the system search path for use
 *                  when searching for a volume.
 *                  Takes path from the environment, 
 *                  or uses the default path. 
 *
 *****************************************************************************/
String _DtHelpGetSystemSearchPath(void)
{
   char * path;

   /* try to retrieve env var value */
   path = getenv(DtSYS_FILE_SEARCH_ENV);

   /* if fail, use default */
   if (NULL == path)
      path = DtDEFAULT_SYSTEM_PATH;

   return strdup(path);
}




/*****************************************************************************
 * Function:	   void DtHelpSetCatalogName(char *catFile);
 *
 *
 * Parameters:     catFile   Specifies the name of the message catalog file
 *                           to use.  
 *
 * Return Value:   void
 *
 *
 * Description:    This function will set the name of the message catalog file
 *                 within the Dt help system environment. 
 *      
 *                 The new name must be of the format <name>.cat and the file
 *                 must be installed such that the NLS search path can find it.
 *           
 *                 If this function is not called then the default value of 
 *                 Dt.cat will be used.
 *
 *
 *****************************************************************************/
void DtHelpSetCatalogName(
        char* catFile)
{
  int len;

  _DtHelpProcessLock();
  /* Setup our Message Catalog file names */
  if (catFile == NULL)
    {
      /* Setup the short and long versions */
      CatFileName = strdup("DtHelp");
    }
  else
    {

      /* If we have a full path, just use it */
      if (*catFile == '/')
        CatFileName = strdup(catFile);
      else
        {
          /* hp-ux os does not work with the ".cat" extention, so
           * if one exists, remove it.
           */

           if (strcmp(&catFile[strlen(catFile) -4], ".cat") != 0)
            CatFileName = strdup(catFile);
          else
            {
              /* Create our CatFileName with out the extention */
              len = strlen(catFile) -4;
              CatFileName = malloc(len +1);
              strncpy (CatFileName, catFile, len);
              CatFileName[len]= '\0';
            }
        }
    }
  _DtHelpProcessUnlock();
}



#ifndef NO_MESSAGE_CATALOG
/*****************************************************************************
 * Function:	   Boolean _DtHelpGetMessage(
 *
 *
 * Parameters:     
 *
 * Return Value:   char *
 *
 *
 * Description:    This function will retreive the requested message from the
 *                 cache proper cache creek message catalog file.
 *
 *****************************************************************************/
char *_DtHelpGetMessage(
        int set,
        int n,
        char *s)
{
   char *msg;
   char *loc;
   nl_catd catopen();
   char *catgets();
   static int first = 1;
   static nl_catd nlmsg_fd;

   _DtHelpProcessLock();
   if ( first )
   {

     /* Setup our default message catalog names if none have been set! */
     if (CatFileName  == NULL)
       {
         /* Setup the short and long versions */
         CatFileName = strdup("DtHelp");
       }

     loc = _DtHelpGetLocale();
     if (!loc || !(strcmp (loc, "C")))
	/*
	 * If LANG is not set or if LANG=C, then there
	 * is no need to open the message catalog - just
	 * return the built-in string "s".
	 */
	nlmsg_fd = (nl_catd) -1;
     else
	nlmsg_fd = catopen(CatFileName, NL_CAT_LOCALE);

     first = 0;
   }
   
   msg=catgets(nlmsg_fd,set,n,s);
   _DtHelpProcessUnlock();
   return (msg);
}
#endif



/*****************************************************************************
 * Function:	   char * _DtHelpGetLocale(
 *
 *
 * Parameters:     
 *
 * Return Value:   char *
 *
 *
 * Description:    Returns the value of LC_MESSAGES from the environ.
 *                 If that is NULL, returns the value of LANG form the environ.
 *                 If that is NULL, returns NULL.
 *
 *****************************************************************************/
char *_DtHelpGetLocale(void)
{
    char *loc;

    loc = setlocale(LC_MESSAGES, NULL);
    if (NULL == loc || '\0' == loc[0]) loc = getenv("LANG");
    if (NULL == loc || '\0' == loc[0]) return NULL;

    loc = strdup(loc);    /* getenv() returns ptr to private memory */
    return loc;
}

