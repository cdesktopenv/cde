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
/* $XConsortium: ds_xlib.c /main/3 1995/11/01 12:40:45 rswiston $ */
/*									*
 *  ds_xlib.c                                                           *
 *   Contains some common functions which use the xlib library used     *
 *   throughout the Desktop Calculator.                                 *
 *                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc. 				*
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/param.h>
#include <sys/stat.h>
#include "ds_xlib.h"
#include <X11/Xutil.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>
#include <Xm/Protocols.h>

#define  FREE         (void) free
#define  FPRINTF      (void) fprintf
#define  GETHOSTNAME  (void) gethostname
#define  SPRINTF      (void) sprintf
#define  STRCPY       (void) strcpy

#define  EQUAL(a, b)  !strncmp(a, b, strlen(b))
#define  MAXLINE      120        /* Maximum length for character strings. */

/*  Function:     ds_beep()
 *
 *  Purpose:      Ring the bell (at base volume).
 *
 *  Parameters:   display    connection to the X server.
 *                           (returned from XOpenDisplay).
 *
 *  Returns:      None.
 */

void
ds_beep(Display *display)
{
  XBell(display, 0) ;
}



/*  Function:     ds_get_resource()
 *
 *  Purpose:      Get an X resource from the server.
 *
 *  Parameters:   rDB          X resources database.
 *
 *                appname      application name.
 *
 *                resource     X resource string to search for.
 *
 *  Returns:      resource string, or NULL if not found.
 *
 *  Note:         The first character of the appname and resource strings may
 *                be modified.
 */

char *
ds_get_resource(XrmDatabase rDB, char *appname, char *resource)                          
{
  char app[MAXLINE], res[MAXLINE] ;
  char cstr[MAXLINE], nstr[MAXLINE] ;
  char *str_type[20] ;
  XrmValue value ;

  STRCPY(app, appname) ;
  STRCPY(res, resource) ;
  if (isupper(app[0])) app[0] = tolower(app[0]) ;
  SPRINTF(nstr, "%s.%s", app, res) ;

  if (islower(res[0])) res[0] = toupper(res[0]) ;
  if (islower(app[0])) app[0] = toupper(app[0]) ;
  SPRINTF(cstr, "%s.%s", app, res) ;

  if (XrmGetResource(rDB, nstr, cstr, str_type, &value) == 0)
    return((char *) NULL) ;
  else return(value.addr) ;
}


/*  Function:     ds_load_resources()
 *
 *  Purpose:      Get the resource databases. These are looked for in the
 *                following ways:
 *
 *                Classname file in the app-defaults directory. 
 *
 *                Classname file in the directory specified by the
 *                XUSERFILESEARCHPATH or XAPPLRESDIR environment variable.
 *
 *                Property set using xrdb, accessible through the
 *                XResourceManagerString macro or, if that is empty, the
 *                ~/.Xdefaults file.
 *
 *                XENVIRONMENT environment variable or, if not set,
 *                .Xdefaults-hostname file.
 *
 *                DTCALCDEF environment variable or, if not set, the
 *                ~/.dtcalcdef file
 *
 *  Parameters:   display    connection to the X server.
 *                           (returned from XOpenDisplay).
 *
 *  Returns:      X combined resources database.
 */

XrmDatabase
ds_load_resources(Display *display)
{
  XrmDatabase db, rDB ;
  char *home, name[MAXPATHLEN], *ptr ;
  int len ;

  rDB  = NULL ;
  home = getenv("HOME") ;
  XrmInitialize() ;

/* Merge server defaults, created by xrdb. If nor defined, use ~/.Xdefaults. */

  if (XResourceManagerString(display) != NULL)
    db = XrmGetStringDatabase(XResourceManagerString(display)) ;
  else
    { 
      SPRINTF(name, "%s/.Xdefaults", home) ;
      db = XrmGetFileDatabase(name) ;
    }
  XrmMergeDatabases(db, &rDB) ;

/*  Open XENVIRONMENT file or, if not defined, the .Xdefaults, and merge
 *  into existing database.
 */

  if ((ptr = getenv("XENVIRONMENT")) == NULL)
    {
      SPRINTF(name, "%s/.Xdefaults-", home) ;
      len = strlen(name) ;
      GETHOSTNAME(name+len, 1024-len) ;
      db = XrmGetFileDatabase(name) ;
    }
  else db = XrmGetFileDatabase(ptr) ;
  XrmMergeDatabases(db, &rDB) ;

/*  Finally merge in Dtcalc defaults via DTCALCDEF or, if not
 *  defined, the ~/.dtcalcdef file.
 */

  if ((ptr = getenv("DTCALCDEF")) == NULL)
    {
      snprintf(name, MAXPATHLEN - 1, "%s/.dtcalcdef", home) ;
      db = XrmGetFileDatabase(name) ;
    }
  else db = XrmGetFileDatabase(ptr) ;
  XrmMergeDatabases(db, &rDB) ;
  return(rDB) ;
}


/*  Function:     ds_put_resource()
 *
 *  Purpose:      Adds an X resource string (name and value) to a resources
 *                database.
 *
 *  Parameters:   rDB          X resources database.
 *
 *                appname      application name.
 *
 *                rstr         X resource string name.
 *
 *                rval         X resource string value.
 *
 *  Returns:      None.
 *
 *  Note:         The first character of the appname and resource strings may
 *                be modified.
 */

void
ds_put_resource(XrmDatabase *rDB, char *appname, char *rstr, char *rval)
{
  char app[MAXLINE], resource[MAXLINE] ;

  STRCPY(app, appname) ;
  if (isupper(app[0])) app[0] = tolower(app[0]) ;
  SPRINTF(resource, "%s.%s", app, rstr) ;

  XrmPutStringResource(rDB, resource, rval) ;
}


/*  Function:     ds_save_cmdline()
 *
 *  Purpose:      Save away the application command line options.
 *
 *  Parameters:   display      connection to the X server.
 *                             (returned from XOpenDisplay).
 *
 *                w            The id of the applications main window.
 *
 *                argc         Number of command line options.
 *
 *                argv         An array of command line options.
 *
 *  Returns:      None.
 */

void
ds_save_cmdline(Display *display, Window w, int argc, char **argv)
{
  XSetCommand(display, w, argv, argc) ;
}


/*  Function:     ds_save_resources()
 *
 *  Purpose:      Save away the resources database to the file given by the
 *                DTCALCDEF environment variable (if set), or
 *                to $HOME/.dtcalcdef.
 *
 *  Parameters:   rDB        X resources database to save.
 *
 *  Returns:      1          if cannot access resource database to write.
 *                0          on successful completion.
 */


int
ds_save_resources(XrmDatabase rDB, char *filename)
{
  char *home;
  struct stat statbuf ;

  if(filename == NULL)
  {
    if ((filename = getenv("DTCALCDEF")) == NULL)
      {
        home = getenv("HOME") ;
        filename = (char*) calloc(1, strlen(home) + 18) ;
        SPRINTF(filename, "%s/.dtcalcdef", home) ;
      }
  }

/* If file exists but user does not have access. */

  if (stat(filename, &statbuf) != -1 && access(filename, W_OK) != 0)
    { 
      FREE(filename) ;
      return(1) ;
    }

/* If file does not exist this call will create it. */

  XrmPutFileDatabase(rDB, filename) ;
  FREE(filename) ;
  return(0) ;
}

