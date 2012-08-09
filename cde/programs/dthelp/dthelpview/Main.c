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
/* $TOG: Main.c /main/5 1998/04/20 12:52:56 mgreess $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        Main.c
 **
 **   Project:     Cache Creek (Rivers) Project
 **
 **   Description: This is the main.c file for the helpview program.
 ** 
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/




/* System Include Files  */

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <limits.h>
#include <unistd.h>  /* R_OK */
#ifdef __osf__
/* Suppress unaligned access message */
#include <sys/types.h>
#include <sys/sysinfo.h>
#endif /* __osf__ */

#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/Xatom.h>

#include <Xm/MwmUtil.h>
#include <Xm/Protocols.h>
#include <Xm/Xm.h>
#include <Xm/PushB.h>

#include <Dt/HelpDialog.h>


#include <Dt/DtNlUtils.h>
#include <Dt/EnvControlP.h>

/* Local Includes */

#include <DtI/HelposI.h>
#include <DtI/HelpP.h>
#include <DtI/FileUtilsI.h>
#include "Main.h"
#include "UtilI.h"
#include "ManPageI.h"

/*  Application resource list definition  */

static XrmOptionDescRec option_list[] =
{
   {  "-helpVolume",     "helpVolume",     XrmoptionSepArg,  NULL  },
   {  "-locationId",     "locationId",     XrmoptionSepArg,  NULL  },
   {  "-file",           "file",           XrmoptionSepArg,  NULL  },
   {  "-manPage",        "manPage",        XrmoptionSepArg,  NULL  },
   {  "-man",            "man",            XrmoptionNoArg,  "True" },

};


/*  Structure, resource definitions, for View's optional parameters.  */


typedef struct 
{
   char * helpVolume;
   char * locationId;
   char * file;
   char * manPage;
   char * man;
   
} ApplicationArgs, *ApplicationArgsPtr;

static ApplicationArgs application_args;

static XtResource resources[] =
{

   {
     "helpVolume", "HelpVolume", XmRString, sizeof (char *),
     XtOffset (ApplicationArgsPtr, helpVolume),XmRImmediate,(caddr_t) NULL,
   },

   {
     "locationId", "LocationId", XmRString, sizeof (char *),
     XtOffset (ApplicationArgsPtr, locationId), XmRImmediate, (caddr_t) NULL,
   },

   {
     "file", "File", XmRString, sizeof (char *),
     XtOffset (ApplicationArgsPtr, file), XmRImmediate, (caddr_t) NULL,
   },
   {
     "manPage", "ManPage", XmRString, sizeof (char *),
     XtOffset (ApplicationArgsPtr, manPage), XmRImmediate, (caddr_t) NULL,
   },
   {
     "man", "Man", XmRString, sizeof (char *),
     XtOffset (ApplicationArgsPtr, man), XmRImmediate, (caddr_t) NULL,
   },

};






/********    Static Function Declarations    ********/

static void Usage(
    char ** argv);
static void ExpandVolume(
    char    **helpVolume);




/* Global Variables */
#define MAX_ARGS 20
#define charset       XmFONTLIST_DEFAULT_TAG






/****************************************************************************
 * Function:	    static void GlobalInit();
 *
 * Parameters:      
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Initializes our global variables to valid starting values.
 *
 ****************************************************************************/
static void GlobalInit()

{

  pCacheListHead   = NULL; 
  pCacheListTale   = NULL;  
  totalCacheNodes  = 0;
  
  helpClass = XtNewString("Dthelpview");

  viewWidget = NULL;
  manWidget  = NULL;
  manBtn = NULL;
  manText = NULL;
  manForm = NULL;
  closeBtn = NULL;

}

/***************************************************************************
 * Main Line Program:
 *
 ***************************************************************************/
void main(
    int argc,
    char **argv)
{
  char		  *appName;
  Arg	 	  args[2];	
  int             n;
  int             newArgc=0;
  char            **newArgv;
  int             counter=0;

#ifdef __osf__
/* Code to suppress unaligned access message. */
   unsigned long        op;
   int                  buffer[2];
   unsigned long        nbytes = 1;
   char*                arg = 0;
   unsigned long        flag = 0;

   int                  ssi_status;

   op = SSI_NVPAIRS;

   buffer[0] = SSIN_UACPROC;
   buffer[1] =  0x00000001;
#ifdef DEBUG_UAC
   buffer[1] |= 0x00000004;
#endif

   ssi_status = setsysinfo ( op, (caddr_t) buffer, nbytes, arg, flag );
#endif


  XtSetLanguageProc(NULL, NULL, NULL);
  startCommand = argv[0];
  appName = strrchr(argv[0], '/');
  if (appName != NULL)
    appName++;
  else
    appName = argv[0];
   

  /* Copy our argv values into a new array for use in DisplayTopic... */
   newArgc = argc;
   newArgv = (char **) XtMalloc (sizeof(char *) * (argc +1));

   for (counter=0;counter < argc; counter++)
     {
       newArgv[counter] = XtMalloc (strlen(argv[counter]) +1);
       strcpy (newArgv[counter], argv[counter]);
      }
   
  /* Setup our Help message catalog file name */
  DtHelpSetCatalogName("DtHelp.cat");

  _DtEnvControl(DT_ENV_SET);
   {
     /*  char * foo = ((char *)GETMESSAGE(7, 1, ""));  ??? */
   }


  /* Initialize toolkit and open the display */
  topLevel = XtInitialize(appName, "Dthelpview", option_list, 5, &argc, argv);
  appDisplay = XtDisplay(topLevel);

  if (!appDisplay) 
    {
      XtWarning ("Dialogs: Can't open display, exiting...");
      exit (0);
    }
  
  /* Get the application resources. */
  XtGetApplicationResources(topLevel, &application_args, 
                            resources, XtNumber(resources), NULL, 0);

  /*  If all of the command line parameters were not processed  */
  /*  out, print out a usage message set and exit.              */

  if (argc != 1)
    Usage (argv);

  /* Call our global init routine */
  GlobalInit();


  /* Setup or environment to handle multi-byte stuff */
#ifdef NLS16
  Dt_nlInit();
#endif
 


  /* Give our shell a default size greater than zero */
  n = 0;
  XtSetArg (args[n], XmNheight, 10);          n++;
  XtSetArg (args[n], XmNwidth, 10);           n++;
  XtSetValues(topLevel, args, n);
 

  /* Setup up our top level shell */
   /* XtSetMappedWhenManaged(topLevel, FALSE);
    * XtRealizeWidget(topLevel);
    */


  if (application_args.file != NULL)
      DisplayFile(topLevel, application_args.file);    
  else if (application_args.manPage != NULL)
      DisplayMan(topLevel, application_args.manPage,
                 EXIT_ON_CLOSE);
  else if (application_args.man != NULL)
      PostManDialog(topLevel, newArgc, newArgv);
  else if (application_args.helpVolume != NULL)
    {
      /* See if you can expand the helpVolume value */
      ExpandVolume(&(application_args.helpVolume));
      DisplayTopic(topLevel, application_args.helpVolume,
                   application_args.locationId, newArgc, newArgv);
    }
  else
    Usage (argv);    

  XtMainLoop();
}



/************************************************************************
 *
 *  Usage
 *	When incorrect parameters have been specified on the command
 *	line, print out a set of messages detailing the correct use
 *	and exit.
 *
 ************************************************************************/
static void Usage(
    char ** argv)
{
   (void) fprintf (stderr, ((char *)_DTGETMESSAGE(7, 1, 
                            "Usage: %s...\n")), argv[0]);
   (void) fprintf (stderr, "\n");
   (void) fprintf (stderr, ((char *)_DTGETMESSAGE(7, 2, 
                            "\t-helpVolume <Help Volume File>\n")), argv[0]);
   (void) fprintf (stderr, ((char *) _DTGETMESSAGE(7, 3,
                     "\t-locationId  <ID>\n")), argv[0]);
   (void) fprintf (stderr, ((char *) _DTGETMESSAGE(7, 5,
                     "\t-file  <ASCII Text File>\n")), argv[0]);
   (void) fprintf (stderr, ((char *) _DTGETMESSAGE(7, 6,
                     "\t-man\n")), argv[0]);
   (void) fprintf (stderr, ((char *) _DTGETMESSAGE(7, 7,
                     "\t-manPage  <Unix Man Page>\n\n\n")), argv[0]);

   exit (0);
}



/****************************************************************************
 * Function:         ExpandVolume()
 *
 * Parameters:      
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Takes a helpVolume file name and adds the cwd path to the 
 *                  front of it, and possibly adds a suffix.
 *                  It then stats the file; if the file is present, the 
 *                  function returns a malloc'd full path in helpVolume, 
 *                  otherwise it returns a malloc'd copy of the original value.
 *
 ***************************************************************************/
static void ExpandVolume(
    char    **helpVolume)
{

  char * workingPath=NULL;

   /* try to locate file and its entry, if present */
   /* True: search relative to current directory as well */
   workingPath = _DtHelpFileLocate("volumes", *helpVolume,
                                  _DtHelpFileSuffixList,True,R_OK);
   if (workingPath) *helpVolume = workingPath;
   else *helpVolume = strdup(*helpVolume);
}






