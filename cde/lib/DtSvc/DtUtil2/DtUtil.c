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
/* $TOG: DtUtil.c /main/10 1998/07/30 12:13:08 mgreess $ */
/*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/******************************************************************************
 *
 * File Name: DtUtil.c
 *
 *  Contains the DT functions used by an application to connect to the
 *  underlying communications mechanism.
 *
 *****************************************************************************/
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netdb.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>
#include <Dt/DtP.h>
#include <Dt/UserMsg.h>
#include <Dt/DtNlUtils.h>
#include <Dt/EnvControlP.h>
#include "DtSvcLock.h"

/*****************************************
 *
 * External functions not defined in header files.
 *
 *****************************************/

/********    End Public Function Declarations    ********/

/********    Static Function Declarations    ********/

static Boolean DtBigInitialize( 
                        Display *display,
                        Widget widget,
                        char *name,
                        char *toolClass,
                        XtAppContext app_context) ;
static void InitButtonLabels( void ) ;
static void DtGlobalsInitialize( 
                        Display *display,
                        char *name,
                        char *toolClass) ;

/********    End Static Function Declarations    ********/


/*****************************************
 *
 * Global variables 
 *
 *****************************************/

XrmDatabase _DtResourceDatabase = NULL;	/* This Dt global indicates which
					   Xrm database should be read for
					   resources. */

XtAppContext _DtAppContext = NULL;	/* This Dt global keeps track of the
					   app-context, if one has been 
					   specified.  Note that libXv does
					   not yet support multiple app-
					   contexts. */
XtAppContext *_DtInitAppContextp = NULL;
Widget   _DtInitTtContextWidget = NULL;

Display *_DtDisplay = NULL;		/* This global variable is saved
					   when a client invokes "DtInitialize"
					   It is used later to get resources
					   when the DT databases are loaded.*/
char *_DtApplicationName = NULL;	/* This global variable is the
					   client's "ApplicationName". */
char *_DtApplicationClass = NULL;	/* This global variable is the
					   client's "ApplicationClass". */
char *_DtToolClass = NULL;		/* Tool class passed to _DtInit...() */


/* Localizable button labels */
const char * _DtOkString = NULL;
const char * _DtCancelString = NULL;
const char * _DtHelpString = NULL;
const char * _DtApplyString = NULL;
const char * _DtCloseString = NULL;


/*********************************************
 *
 * Initialization Functions
 *
 *********************************************/

Boolean 
DtAppInitialize(
        XtAppContext app_context,
        Display *display,
        Widget widget,
        char *name,
        char *toolClass )
{
    Boolean result;

    _DtSvcAppLock(app_context);
   result = DtBigInitialize (display, widget, name, toolClass, app_context);
   _DtSvcAppUnlock(app_context);
   return (result);
}

Boolean 
DtInitialize(
        Display *display,
        Widget widget,
        char *name,
        char *toolClass )
{
    Boolean result;
    _DtSvcDisplayToAppContext(display);

    _DtSvcAppLock(app);
   result = DtBigInitialize (display, widget, name, toolClass, NULL);
   _DtSvcAppUnlock(app);
   return (result);
}

static Boolean 
DtBigInitialize(
        Display *display,
        Widget widget,
        char *name,
        char *toolClass,
        XtAppContext app_context )
   
{
   static Boolean initialized = False;

   /* Initialization can only be performed once. */
   _DtSvcProcessLock();
   if (initialized) {
      _DtSvcProcessUnlock();
      return (False);
   }

   /* Preserve the pre-Dt environ and add Dt-specifics to environ */
   (void) _DtEnvControl (DT_ENV_SET); 

   /* Initialize a bunch of miscellaneous things. */
   DtNlInitialize();
   InitButtonLabels();
   DtGlobalsInitialize (display, name, toolClass);

   if ( XmIsGadget(widget) )
        _DtInitTtContextWidget = XtParent(widget);
   else
        _DtInitTtContextWidget = widget;

   if (app_context)
       _DtAppContext =  app_context;
   else
       _DtAppContext =  XtWidgetToApplicationContext(_DtInitTtContextWidget);
   _DtInitAppContextp = &_DtAppContext;

   initialized = TRUE;
   _DtSvcProcessUnlock();
   return (initialized);
}


/* Initialize the global button labels */

static void 
InitButtonLabels( void )

{
   _DtOkString = XtNewString(Dt11GETMESSAGE(28, 1, "OK"));
   _DtCancelString = XtNewString(Dt11GETMESSAGE(28, 2, "Cancel"));
   _DtHelpString = XtNewString(Dt11GETMESSAGE(28, 3, "Help"));
   _DtApplyString = XtNewString(Dt11GETMESSAGE(28, 4, "Apply"));
   _DtCloseString = XtNewString(Dt11GETMESSAGE(28, 5, "Close"));
}

static void 
DtGlobalsInitialize(
        Display *display,
        char *name,
        char *toolClass )
{
   _DtResourceDatabase = XtDatabase (display);
   DtProgName = name;
   _DtToolClass = XtNewString(toolClass);

   XeToolClass = XtNewString (toolClass);

   /*
    * Save the application name and application class.
    */
   _DtDisplay = display;
   XtGetApplicationNameAndClass (display,
				 &_DtApplicationName,
				 &_DtApplicationClass);
}   
