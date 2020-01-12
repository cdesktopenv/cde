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
/* $XConsortium: SmUtil.c /main/5 1996/06/21 17:26:03 ageorge $ */
/*                                                                            *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                           *
 * (c) Copyright 1993, 1994 International Business Machines Corp.             *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                            *
 * (c) Copyright 1993, 1994 Novell, Inc.                                      *
 */
/******************************************************************************
 *
 * File Name: SmUtil.c
 *
 *  Contains the DT functions used by an application to communicate with
 *  dtsession.
 *
 *****************************************************************************/

#if 0
#include <sys/param.h>
#include <sys/types.h>
#endif

#include <sys/stat.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <Dt/WsmP.h>
#include <Dt/DtP.h>
#include <Dt/Session.h>
#include <Dt/SmCreateDirs.h>
#include "DtSvcLock.h"

#ifndef CDE_INSTALLATION_TOP
#define CDE_INSTALLATION_TOP "/opt/dt"
#endif

/********    Static Function Declarations    ********/

static char *getSessionName(Display *, Atom);
static Boolean getSessionPath( Widget, char *, char **, char **);

/********    End Static Function Declarations    ********/

/*************************************<->*************************************
 *
 *  getSessionName (display, prop )
 *
 *
 *  Description:
 *  -----------
 *  Returns the session name. 
 *
 *  Inputs:
 *  ------
 *  display - the display
 *  prop - the property name of the save or restore session
 *
 *  Outputs:
 *  -------
 *  
 *  Return:
 *  ------
 *  Returns the session name string or NULL if it could not be obtained. 
 *  This value should be freed with XFree().
 *  
 *
 *************************************<->***********************************/

static char *
getSessionName(
        Display *display,
        Atom prop)
{
  int                 propStatus;
  Atom                actualType;
  int                 actualFormat;
  unsigned long       nitems;
  unsigned long       leftover;
  char               *property = NULL;

  propStatus = XGetWindowProperty (display, RootWindow(display, 0), 
                                   prop, 0L,
                                   1000000L, False,
                                   AnyPropertyType, &actualType,
                                   &actualFormat, &nitems, &leftover, 
                                   (unsigned char **)&property);

 
  if(propStatus == Success &&
     actualType != None &&
     actualFormat == 8 &&
     nitems != 0)
  {
    return(property);
  }
 
  if (property)
  {
    XFree(property);
  }

  return(NULL);
}

/*************************************<->*************************************
 *
 *  getSessionPath (widget, propstring, savePath, saveFile)
 *
 *
 *  Description:
 *  -----------
 *  This function generates a full path name for an application's state
 *  file. If *saveFile is NULL, a new file name is generated, else
 *  *saveFile is used. It returns True if the path is returned, False
 *  otherwise.
 *
 *
 *  Inputs:
 *  ------
 *  widget - a widget to use to get the display
 *  propstring - session name property
 *  savePath - pointer to memory in which to place pointer to path
 *  saveFile - pointer to filename. If *saveFile is NULL, a new filename
 *             will be allocated and returned in *saveFile, else *saveFile
 *             will be used to generate path name
 *
 *  Outputs:
 *  -------
 *  True - path name returned
 *  False - path name not returned
 *
 *************************************<->***********************************/

static Boolean
getSessionPath(
        Widget widget,
        char *propstring,
        char **savePath,
        char **saveFile )
{
    Display 		*display;
    char 		*tmpPath = NULL;
    char        	*property = NULL;
    char                *fileName;
    struct stat  	buf;
    int 		status;
    
    display = XtDisplay(widget); 
    
    tmpPath = _DtCreateDtDirs(display);
    if (tmpPath == NULL) goto abort;
    
    property = getSessionName(display, 
                  XInternAtom(display, propstring, False));
    if (property == NULL) goto abort;

   /*
    * NOTE: it is assumed that _DtCreateDtDirs() returns a buffer of 
    *       size MAXPATHLEN. This allows us to avoid a extra alloc
    *       and copy -- at the expense of code maintainability.
    *
    * JET - 2020.  This is stupid.  At least account for the strings
    * you are adding further on down...  This "solution" isn't great
    * either.  Real fix would be to have all callers pass in bufptr
    * and len all the way down the chain instead of tmpPath.
    */
    if ((strlen(tmpPath)
         + 1 /* "/" */
         + strlen(property)
         + 1 /* "/" */
         + ((*saveFile == NULL) ? strlen("dtXXXXXX") + 1 : strlen(*saveFile))
        ) >= MAXPATHLEN)
    {
        goto abort;
    }

   /* 
    * parse the property string and create directory if needed 
    */
    (void)strcat(tmpPath, "/");
    (void)strcat(tmpPath, property);
    status = stat(tmpPath, &buf);

   /*
    * directory does not exist.
    */
    if(status == -1)
    {
        status = mkdir(tmpPath, 0000);
        if(status == -1) goto abort;

        (void)chmod(tmpPath, 0755);
    }

    (void)strcat(tmpPath, "/");

    if (*saveFile == NULL)
    {
     /*
      * No saveFile name was provided, so generate a new one.
      */
      int len = strlen(tmpPath);

      (void)strcat(tmpPath, "dtXXXXXX");
      (void)mktemp(tmpPath);

      *saveFile = (char *) XtMalloc(15 * sizeof(char));
      if(*saveFile == NULL) goto abort;

      (void)strcpy(*saveFile, tmpPath+len);
    }
    else
    {
     /*
      * A saveFile name was provided, so use it.
      */
      (void)strcat(tmpPath, *saveFile);
    }

    *savePath = tmpPath;

    XFree ((char *)property);
    return(True);

  abort:
   /*
    * ObGoto: if it clarifies the logic and reduces code, 
    *         goto's are ok by me.
    */
    *savePath = NULL;
    if (tmpPath) XtFree ((char *)tmpPath);
    if (property) XFree ((char *)property);
    return(False);
}


/*************************************<->*************************************
 *
 *  DtSessionSavePath (widget, savePath, saveFile)
 *
 *
 *  Description:
 *  -----------
 *  This function returns (in it's parameters) the full path name for an
 *  application to save to, as well as the file name to save away for later
 *  restoration.  It returns True if the path is returned, False
 *  otherwise.
 *
 *
 *  Inputs:
 *  ------
 *  widget - a widget to use to get the display
 *
 *  Outputs:
 *  -------
 *  returns a status value
 *
 *************************************<->***********************************/

Boolean 
DtSessionSavePath(
        Widget widget,
        char **savePath,
        char **saveFile )
{
    Boolean             result;
    _DtSvcWidgetToAppContext(widget);

    _DtSvcAppLock(app);
    *saveFile = NULL;
    result = getSessionPath(widget, _XA_DT_SAVE_MODE, savePath, saveFile);
    _DtSvcAppUnlock(app);
    
    return(result);
} /* END OF FUNCTION DtSessionSavePath */


/*************************************<->*************************************
 *
 *  DtSessionRestorePath (widget, restorePath, restoreFile)
 *
 *
 *  Description:
 *  -----------
 *  This function returns (in its parameters), the path where the application
 *  is to restore its file from.
 *  It returns True if the path is returned, False
 *  otherwise.
 *
 *
 *  Inputs:
 *  ------
 *  widget - a widget to use to get the display
 *
 *  Outputs:
 *  -------
 *  returns a status value
 *
 *************************************<->***********************************/

Boolean 
DtSessionRestorePath(
        Widget widget,
        char **savePath,
        char *saveFile )
{
    Boolean             result;
    _DtSvcWidgetToAppContext(widget);

    _DtSvcAppLock(app);
    result = getSessionPath(widget, _XA_DT_RESTORE_MODE, savePath, &saveFile);
    _DtSvcAppUnlock(app);
    
    return(result);
} /* END OF FUNCTION DtSessionRestorePath */
