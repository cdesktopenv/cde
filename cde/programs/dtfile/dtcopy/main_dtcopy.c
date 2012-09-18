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
/* $TOG: main_dtcopy.c /main/10 1998/10/26 12:42:41 mgreess $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 *   FILE:           main_dtcopy.c
 *
 *   COMPONENT_NAME: dtcopy - Handles move/copy/link operations for 'dtfile'
 *
 *   DESCRIPTION:    Main program for dtfile_copy.
 *
 *
 *   FUNCTIONS: AppendErrorMsg
 *		CheckForMap
 *		ConfirmHandler
 *		ErrorHandler
 *		EventCheck
 *		InitNlsMessage
 *		TimeoutHandler
 *		UpdateErrorCount
 *		UpdateStatus
 *		UpdateSummary
 *		get_command_line
 *		main
 *		moveDeleteCallback
 *		moveErrorCallback
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *
 ****************************************************************************
 ************************************<+>*************************************/

#include <stdio.h>
#include <locale.h>
#include <time.h>
#include <errno.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>

/* #Include <X11/Xmu/Editres.h> */      /* for editres debugging tool */

#include <Dt/Connect.h>
#include <Dt/DtNlUtils.h>
#include <Dt/EnvControlP.h>

#include "fsrtns.h"
#include "dosync.h"
#include "dtcopy.h"
#include "sharedFuncs.h"


/*--------------------------------------------------------------------
 * Global Variables
 *------------------------------------------------------------------*/

extern Widget G_toplevel;
Widget G_summary_text;
Widget G_status_text;
/* Widget G_status_text2; */
Widget G_error_count;
Widget G_error_msgs;
Widget G_copy_dialog;
Widget G_error_dialog = NULL;
Widget G_over_dialog = NULL;
Widget G_over_confirm;
Widget G_copy_action_area_pbuttons[4];
Widget G_help_dialog;
Widget G_overwrite_radio;
Widget G_rename_text;
Widget G_toggle_main;
Widget G_toggle_error;

int    G_overwrite_selection = G_OVERWRITE_EXISTING;
int    G_move = FALSE;
int    G_do_copy = TRUE;
int    G_pause_copy = FALSE;
int    G_user_pause_copy = FALSE;
int    G_wait_on_error = TRUE;
int    G_error_status = 0;
int    G_ignore_errors = FALSE;
int    G_overwrite_confirm  = TRUE;
int    G_wait_on_input = TRUE;
extern int    G_dialog_closed;

char    G_rename_oldname[MAX_PATH];
char    G_source_dir[MAX_PATH];

/*  debugging flag  */
#ifdef DEBUG
int debug = 0;
#endif

static XtAppContext app_context;
static Display *display;
static Boolean is_mapped = False;
static int nfiles = 0;
static int ndirs = 0;
static int nerr = 0;
static char last_dir[MAX_PATH];

#define FILEOPNUM 14
static char *LongFileOpNames[FILEOPNUM];
static char *DefaultLongFileOpNames[] = {
  "processing",
  "opening",
  "opening",
  "opening",
  "reading link",
  "deleting",
  "copying",
  "creating directory",
  "copying link",
  "creating link",
  "working",
  "is a copy",
  "is a link",
  "renaming"
};

/*--------------------------------------------------------------------
 * application resource definitions
 *------------------------------------------------------------------*/

typedef struct
{
  char *source_name;
  char *target_name;
  SyncParams sync;
  Boolean move;
  Boolean confirmreplace;
  Boolean confirmerrors;
  Boolean popdown;
  int delay;
  Boolean use_toggle_buttons;
  Boolean slow;
  Boolean checkPerms;
} AppArgs, *AppArgsPtr;

static AppArgs app_args;

static XtResource resources[] =
{
   /* source and target directories */

   { "source", "Source", XmRString, sizeof (char *),
     XtOffset (AppArgsPtr, source_name), XmRImmediate, (XtPointer) NULL, },

   { "target", "Target", XmRString, sizeof (char *),
     XtOffset (AppArgsPtr, target_name), XmRImmediate, (XtPointer) NULL, },

   /* SyncParams */

   { "verbose", "Verbose", XmRBool, sizeof (int),
     XtOffset (AppArgsPtr, sync.verbose), XmRImmediate, (XtPointer) True, },

   { "quiet", "Quiet", XmRBool, sizeof (int),
     XtOffset (AppArgsPtr, sync.quiet), XmRImmediate, (XtPointer) False, },

   { "dontDoIt", "DontDoIt", XmRBool, sizeof (int),
     XtOffset (AppArgsPtr, sync.dontdoit), XmRImmediate, (XtPointer) False, },


   { "keepNew", "KeepNew", XmRBool, sizeof (int),
     XtOffset (AppArgsPtr, sync.keepnew), XmRImmediate, (XtPointer) False, },

   { "keepOld", "KeepOld", XmRString, sizeof (char *),
     XtOffset (AppArgsPtr, sync.keepold), XmRImmediate, (XtPointer) NULL, },

   { "dontDelete", "DontDelete", XmRBool, sizeof (int),
     XtOffset (AppArgsPtr, sync.dontdelete), XmRImmediate, (XtPointer) False, },

   { "dontAdd", "DontAdd", XmRBool, sizeof (int),
     XtOffset (AppArgsPtr, sync.dontadd), XmRImmediate, (XtPointer) False, },

   { "dontReplace", "DontReplace", XmRBool, sizeof (int),
     XtOffset (AppArgsPtr, sync.dontreplace),
     XmRImmediate, (XtPointer) False, },

   { "dontRecur", "DontRecur", XmRBool, sizeof (int),
     XtOffset (AppArgsPtr, sync.dontrecur), XmRImmediate, (XtPointer) False, },

   { "keepLinks", "KeepLinks", XmRBool, sizeof (int),
     XtOffset (AppArgsPtr, sync.keeplinks), XmRImmediate, (XtPointer) False, },

   { "keepCopies", "KeepCopies", XmRBool, sizeof (int),
     XtOffset (AppArgsPtr, sync.keepcopies), XmRImmediate, (XtPointer) False, },

   { "forceCopies", "ForceCopies", XmRBool, sizeof (int),
     XtOffset (AppArgsPtr, sync.forcecopies), XmRImmediate, (XtPointer) False, },

   { "listLinks", "ListLinks", XmRBool, sizeof (int),
     XtOffset (AppArgsPtr, sync.listlinks), XmRImmediate, (XtPointer) False, },

   { "listCopies", "ListCopies", XmRBool, sizeof (int),
     XtOffset (AppArgsPtr, sync.listcopies), XmRImmediate, (XtPointer) False, },

   { "linkFolders", "LinkFolders", XmRBool, sizeof (int),
     XtOffset (AppArgsPtr, sync.linkdirs), XmRImmediate, (XtPointer) False, },

   { "linkFiles", "LinkFiles", XmRBool, sizeof (int),
     XtOffset (AppArgsPtr, sync.linkfiles), XmRImmediate, (XtPointer) False, },

   { "copyFolders", "CopyFolders", XmRBool, sizeof (int),
     XtOffset (AppArgsPtr, sync.copydirs), XmRImmediate, (XtPointer) False, },

   { "copyFiles", "CopyFiles", XmRBool, sizeof (int),
     XtOffset (AppArgsPtr, sync.copyfiles), XmRImmediate, (XtPointer) False, },

   { "copyTop", "CopyTop", XmRBool, sizeof (int),
     XtOffset (AppArgsPtr, sync.copytop), XmRImmediate, (XtPointer) False, },

   /* other options */

   { "move", "Move", XmRBoolean, sizeof (Boolean),
     XtOffset (AppArgsPtr, move), XmRImmediate, (XtPointer) False, },

   { "confirmReplace", "ConfirmReplace", XmRBoolean, sizeof (Boolean),
     XtOffset (AppArgsPtr, confirmreplace), XmRImmediate, (XtPointer) False, },

   { "confirmErrors", "ConfirmErrors", XmRBoolean, sizeof (Boolean),
     XtOffset (AppArgsPtr, confirmerrors), XmRImmediate, (XtPointer) False, },

   { "popDown", "PopDown", XmRBoolean, sizeof (Boolean),
     XtOffset (AppArgsPtr, popdown), XmRImmediate, (XtPointer) False, },

   { "delay", "Delay", XmRInt, sizeof (int),
     XtOffset (AppArgsPtr, delay), XmRImmediate, (XtPointer) 5000, },    /* in milliseconds */

   { "toggle", "Toggle", XmRBoolean, sizeof (Boolean),
     XtOffset (AppArgsPtr, use_toggle_buttons), XmRImmediate, (XtPointer) True, },
   { "slow", "Slow", XmRBoolean, sizeof (Boolean),
     XtOffset (AppArgsPtr, slow), XmRImmediate, (XtPointer) False, },

   { "checkPermissions", "CheckPermissions", XmRBoolean, sizeof (Boolean),
     XtOffset (AppArgsPtr, checkPerms), XmRImmediate, (XtPointer) False, },
};


static XrmOptionDescRec option_list[] =
{
   {  "-from",           "source",         XrmoptionSepArg,   NULL  },
   {  "-to",             "target",         XrmoptionSepArg,   NULL  },

   {  "-verbose",        "verbose",        XrmoptionNoArg,   "True"  },
   {  "-v",              "verbose",        XrmoptionNoArg,   "True"  },
   {  "-quiet",          "quiet",          XrmoptionNoArg,   "True"  },
   {  "-q",              "quiet",          XrmoptionNoArg,   "True"  },
   {  "-dontDoIt",       "dontDoIt",       XrmoptionNoArg,   "True"  },
   {  "-n",              "dontDoIt",       XrmoptionNoArg,   "True"  },

   {  "-keepNew",        "keepNew",        XrmoptionNoArg,   "True"  },
   {  "-kn",             "keepNew",        XrmoptionNoArg,   "True"  },
   {  "-keepOld",        "keepOld",        XrmoptionSepArg,   NULL   },
   {  "-ko",             "keepOld",        XrmoptionSepArg,   NULL   },
   {  "-dontDelete",     "dontDelete",     XrmoptionNoArg,   "True"  },
   {  "-nd",             "dontDelete",     XrmoptionNoArg,   "True"  },
   {  "-dontAdd",        "dontAdd",        XrmoptionNoArg,   "True"  },
   {  "-na",             "dontAdd",        XrmoptionNoArg,   "True"  },
   {  "-dontReplace",    "dontReplace",    XrmoptionNoArg,   "True"  },
   {  "-nr",             "dontReplace",    XrmoptionNoArg,   "True"  },
   {  "-dontRecur",      "dontRecur",      XrmoptionNoArg,   "True"  },
   {  "-nR",             "dontRecur",      XrmoptionNoArg,   "True"  },

   {  "-keepLinks",      "keepLinks",      XrmoptionNoArg,   "True"  },
   {  "-kl",             "keepLinks",      XrmoptionNoArg,   "True"  },
   {  "-keepCopies",     "keepCopies",     XrmoptionNoArg,   "True"  },
   {  "-kc",             "keepCopies",     XrmoptionNoArg,   "True"  },
   {  "-forceCopies",    "forceCopies",    XrmoptionNoArg,   "True"  },
   {  "-fc",             "forceCopies",    XrmoptionNoArg,   "True"  },
   {  "-listLinks",      "listLinks",      XrmoptionNoArg,   "True"  },
   {  "-ll",             "listLinks",      XrmoptionNoArg,   "True"  },
   {  "-listCopies",     "listCopies",     XrmoptionNoArg,   "True"  },
   {  "-lc",             "listCopies",     XrmoptionNoArg,   "True"  },
   {  "-linkFolders",    "linkFolders",    XrmoptionNoArg,   "True"  },
   {  "-ld",             "linkFolders",    XrmoptionNoArg,   "True"  },
   {  "-linkFiles",      "linkFiles",      XrmoptionNoArg,   "True"  },
   {  "-lf",             "linkFiles",      XrmoptionNoArg,   "True"  },
   {  "-copyFolders",    "copyFolders",    XrmoptionNoArg,   "True"  },
   {  "-cd",             "copyFolders",    XrmoptionNoArg,   "True"  },
   {  "-copyFiles",      "copyFiles",      XrmoptionNoArg,   "True"  },
   {  "-cf",             "copyFiles",      XrmoptionNoArg,   "True"  },
   {  "-copyTop",        "copyTop",        XrmoptionNoArg,   "True"  },
   {  "-ct",             "copyTop",        XrmoptionNoArg,   "True"  },

   {  "-move",           "move",           XrmoptionNoArg,   "True"  },
   {  "-mv",             "move",           XrmoptionNoArg,   "True"  },
   {  "-confirmReplace", "confirmReplace", XrmoptionNoArg,   "True"  },
   {  "-cr",             "confirmReplace", XrmoptionNoArg,   "True"  },
   {  "-confirmErrors",  "confirmErrors",  XrmoptionNoArg,   "True"  },
   {  "-ce",             "confirmErrors",  XrmoptionNoArg,   "True"  },
   {  "-popDown",        "popDown",        XrmoptionNoArg,   "True"  },
   {  "-delay",          "delay",          XrmoptionSepArg,   NULL   },
   {  "-slow",           "slow",           XrmoptionNoArg,   "True"  },
   {  "-toggle",         "toggle",         XrmoptionNoArg,   "True"  },
   {  "-notoggle",       "toggle",         XrmoptionNoArg,   "False" },
   {  "-checkPerms",     "checkPermissions",XrmoptionNoArg,  "True"  },
};


/*--------------------------------------------------------------------
 * display update functions
 *------------------------------------------------------------------*/

static void
UpdateSummary(void)
{
  XmString xs;
  char     msg[1024];
  char     files[128];
  char     dirs[128];
  int new_nfiles = nfiles, new_ndirs = ndirs;

  if (nfiles == 1)
    strcpy(files, GETMESSAGE(2, 3, "file"));
  else
    strcpy(files, GETMESSAGE(2, 4, "files"));

  if (ndirs == 1)
    strcpy(dirs, GETMESSAGE(2, 5, "subfolder"));
  else
    strcpy(dirs, GETMESSAGE(2, 6, "subfolders"));

  if ( nfiles < 0 )
    new_nfiles = 0;
  if ( ndirs < 0 )
    new_ndirs = 0;
  sprintf(msg, "%4d %s\n%4d %s", new_nfiles, files, new_ndirs, dirs);

  xs = XmStringCreateLocalized(msg);
  XtVaSetValues (G_summary_text,
                 XmNlabelString, xs,
                 NULL);
  XmStringFree(xs);
}


static void
UpdateStatus(
	char *msg1,
	char *msg2)
{

           String  msg;
           String  pad;
    const  String  ellipsis="...";
    const  int     lenEllipsis = 3;
           int     lenSourceDir, lenMsg, lenMsg2, numBlanks;
    static int     maxMsg = -1;

    DPRINTF(("%s %s\n",msg1,msg2));
/*
    XtUnmanageChild(G_status_text2);

    XtVaSetValues (G_status_text,
                   XmNvalue, msg1,
                   XmNcursorPosition, 0,
                   NULL);

    XtVaSetValues (G_status_text2,
                   XmNvalue, msg2,
                   NULL);

    XtManageChild(G_status_text2);

    XtVaSetValues (G_status_text2,
                   XmNcursorPosition, strlen(msg2),
                   NULL);
*/

    /* The pathname is truncated, if possible, by replacing  */
    /*    the name of the source directory with "...". Thus, */
    /*    when copying /u/joeuser/mydir/myfile, the pathanme */
    /*    is written as .../myfile                           */

    lenSourceDir = strlen(G_source_dir);
    lenMsg2      = strlen(msg2);
    if ( (strncmp(G_source_dir,msg2,lenSourceDir) == 0) &&
         (lenMsg2 > lenSourceDir) )
    {
       msg = XtMalloc(strlen(msg1) + lenEllipsis + (lenMsg2 - lenSourceDir) + 2);
       sprintf(msg, "%s %s%s", msg1, ellipsis, msg2+lenSourceDir);
    }
    else
    {
       msg = XtMalloc(strlen(msg1) + lenMsg2 + 2);
       sprintf(msg, "%s %s", msg1, msg2);
    }

    /* In order to minimize re-sizing of the dialog, the     */
    /*    length of the message is not allowed to decrease.  */
    /*    Blanks are added to the end of shorter messages.   */

    lenMsg = strlen(msg);
    maxMsg = (maxMsg > lenMsg) ? maxMsg : lenMsg;   /* MAX(maxMsg,lenMsg) */
    if (lenMsg < maxMsg)
    {
       numBlanks = maxMsg - lenMsg;
       pad = XtMalloc(numBlanks+1);
       memset(pad,' ',numBlanks);
       pad[numBlanks] = '\0';
       msg = XtRealloc(msg,maxMsg+1);
       strcat(msg,pad);
       XtFree(pad);
    }


    XmTextFieldSetString(G_status_text, msg);

    XtFree(msg);


}


static void
UpdateErrorCount(void)
{
  char msg[128];

  if (nerr == 0)
    strcpy(msg, GETMESSAGE(2, 7, "Errors: none"));
  else
    sprintf(msg, GETMESSAGE(2, 8, "Errors: %d"), nerr);

  /* for the first error, manage the widgets used to display errors */
  if (nerr == 1)
  {
    XtVaSetValues(G_status_text,
                  XmNbottomAttachment, XmATTACH_NONE,
                  NULL);
/*
    XtVaSetValues(G_status_text2,
                  XmNbottomAttachment, XmATTACH_NONE,
                  NULL);
*/
    XtManageChild(G_error_count);
    XtManageChild(G_error_msgs);
    XtManageChild(XtParent(G_error_msgs));
  }

  XtVaSetValues (G_error_count,
                 XmNvalue,   msg,
                 XmNcursorPosition, 0,
                 NULL);
}


static void
AppendErrorMsg(char *msg)
{
  XmTextPosition pos;

  pos = XmTextGetLastPosition (G_error_msgs);
  XmTextInsert (G_error_msgs, pos, msg);
  XmTextSetInsertionPosition (G_error_msgs, pos);
  XmTextShowPosition (G_error_msgs, pos);
}


/*--------------------------------------------------------------------
 * Callback functions
 *------------------------------------------------------------------*/

/* ARGSUSED */
static void
CheckForMap(
	Widget w,
	XtPointer client_data,
	XEvent *event,
	Boolean *cont)
/* set a flag when the main window is mapped */
{
  if (event->type == MapNotify)
    is_mapped = True;
}


static int
EventCheck(void)
/* process X events until there are no more pending events queued */
{
  XEvent event;
  unsigned long pend_result;

  for (;;) {
    if (!G_pause_copy) {
      /* check if there are any X events */
      if (!(XtAppPending(app_context) & XtIMXEvent)) {
        /* maybe more events are "in the pipe" */
        XSync(XtDisplay(G_toplevel), False);
        if (!(XtAppPending(app_context) & XtIMXEvent))
          break;
      }
    }
    /* get the next event and process it */
    XtAppNextEvent(app_context, &event);
    XtDispatchEvent(&event);
  }

  /* if the user clicked on the Cancel button and Warning 'yes' btn, abort the copy */
  if (G_do_copy)
    return 0;
  else {
    return 1;
    }
}


static int
ErrorHandler(FileOp op, char *fname, int errnum)
{
  char msg[2048];

  /* increment the error count */
  nerr++;
  UpdateErrorCount();

  /* don't count this file as one that has been copied */
  nfiles--;
  UpdateSummary();

  /* add error message to error_msgs */
  sprintf(msg, "%s: %s\n", fname, strerror(errnum));
  AppendErrorMsg(msg);

  XmUpdateDisplay(G_toplevel);

  /* Initialize the error status variable. It will be set if the user */
  /* has chosen not to ignore errors */
  G_error_status = 0;

  if (!G_ignore_errors)
    {
       create_error_dialog(G_toplevel, LongFileOpNames[op], fname, errnum);

       G_wait_on_input = TRUE;
       while (G_wait_on_input)
          EventCheck();
    }

  return (G_error_status);
}



static int
ConfirmHandler(
	FileOp op,
	char *sname,
	int stype,
	char *tname,
	int ttype,
	char *link)
{
  char msg[1024];
  Arg args[5];
  Boolean is_overwrite = False;
  Boolean overwrite_dialog = False;     /* was overwrite dialog invoked? */
  char *fname;
  int rc = 0;
  SyncParams sync;

  /* update copy counts */
  if (op == op_sync)
  {
    ndirs++;
    strcpy(last_dir, sname);
    UpdateSummary();
  }
  else if (op == op_copy || op == op_cplink || op == op_mklink)
  {
    if (ttype != ft_noent)
      is_overwrite = True;
    if (!is_overwrite)
    {
      nfiles++;
      UpdateSummary();
    }
  }

  /* display a message that tells the user what we are working on */
  if (op == op_delete || op == op_mkdir)
    UpdateStatus(LongFileOpNames[op], tname);
  else
    UpdateStatus(LongFileOpNames[op], sname);

  XmUpdateDisplay(G_toplevel);


  if (is_overwrite)  {

    /* target already exists and would be replaced */

    /* if confirmation is turned on, ask the user */
    /* set overwrite_dialog so we know if the user was prompted for this file */
    /*   Otherwise, G_overwrite_selection is either the default or was set   */
    /*   by the user in a prior invocation of the overwrite dialog  */
    if (G_overwrite_confirm)
    {
      overwrite_dialog = TRUE;
      create_overwrite_dialog (G_toplevel, sname, tname, ttype & ft_isdir);

      G_wait_on_input = TRUE;
      while (G_wait_on_input)
        EventCheck();

    }

    if (G_overwrite_selection != G_SKIP)
    {
      nfiles++;
      UpdateSummary();
      XmUpdateDisplay(G_toplevel);
    }

    switch (G_overwrite_selection)
    {
      case G_OVERWRITE_EXISTING:
        rc = 0;
        break;

      case G_RENAME_EXISTING:   /* i.e. move clashing target */
        if (!overwrite_dialog)
        {
	   if (auto_rename(tname) == 0)
	   {
              touch(tname);    /* SyncDirectory complains if the file is gone */
              rc = 0;
           }
           else
           {
              ErrorHandler(op_rename, tname, errno);
              rc = 1;         /* do not do the copy */
           }
        }
        else
        {
           rc = 0;
        }
        break;

      case G_SKIP:
        rc = 1;
        break;
    }
  }

  if (app_args.slow) {
    XFlush(XtDisplay(G_toplevel));
    XSync(XtDisplay(G_toplevel), False);
    sleep(1);
  }

  return (rc);
}


/*
 * moveDeleteCallback, moveErrorCallback:
 *   called if "-move" option was given, when the
 *   source directory is deleted after a successful copy.
 */

static int
moveDeleteCallback(char *fname)
{
  return ConfirmHandler(op_delete, "", ft_noent, fname, 0, "");
}


static int
moveErrorCallback(char *fname, int errnum)
{
  return ErrorHandler(op_delete, fname, errnum);
}


/*--------------------------------------------------------------------
 * Get command line arguments
 *------------------------------------------------------------------*/

static int
get_command_line(int argc, char *argv[])
{
  int i = 1;

#define USAGE \
"Usage: %s [options ...] source target\n"

  /* get source & target directory, if not yet set */

  if (app_args.source_name == NULL && i < argc)
    app_args.source_name = argv[i++];

  if (app_args.target_name == NULL && i < argc)
    app_args.target_name = argv[i++];

  /*
   * There should be no more arguments left, and
   * source & target directory should be set
   */

  if (i < argc ||
      app_args.source_name == NULL ||
      app_args.target_name == NULL)
  {
    fprintf(stderr, GETMESSAGE(2, 10, USAGE), argv[0]);
    exit(1);
  }

  return (0);
}


/*--------------------------------------------------------------------
 * Initialize LongFileOpNames by message catalog.
 *------------------------------------------------------------------*/

static void
InitNlsMessage(void)
{
    int i;
    char *msg;

    for (i = 0; i < FILEOPNUM; i++) {
        msg = GETMESSAGE(2, i + 20, DefaultLongFileOpNames[i]);
        LongFileOpNames[i] = XtNewString(msg);
    }
}


/*--------------------------------------------------------------------
 * Main
 *------------------------------------------------------------------*/

int
main(int argc, char *argv[])
{
  char msg[1024];
  XEvent event;
  Arg args[3];
  int rc, n,perm_status=0;
  char * tmpStr;

  Boolean copy_status;

  _DtEnvControl(DT_ENV_SET);
  XtSetLanguageProc(NULL, NULL, NULL);

   /* initialize debugging flag */
#ifdef DEBUG
   if ((tmpStr = getenv("DTFILE_DEBUG")) != NULL)
   {
      debug = atoi(tmpStr);
      if (debug <= 0)
         debug = 1;
   }
#endif

  /* Initialize the toolkit and Open the Display */
  n=0;
  XtSetArg (args[n], XmNmappedWhenManaged, FALSE); n++;

  G_toplevel = XtAppInitialize(&app_context,
                               "Dtfile", /* Dtcopy is considered part of Dtfile */
                               option_list, XtNumber(option_list),
                               &argc, argv, NULL, args, n);

  InitNlsMessage();

  XtGetApplicationResources(G_toplevel, &app_args,
                            resources, XtNumber(resources), NULL, 0);

   /* add support for editres debugging tool */
/* XtAddEventHandler(G_toplevel, (EventMask) 0, True, _XEditResCheckMessages, NULL); */

  /* Get the source and destination directories */
  get_command_line(argc,argv);
  CheckDeleteAccess(app_context,app_args.delay,app_args.checkPerms,
                                         app_args.move,app_args.source_name);
  G_ignore_errors = !app_args.confirmerrors;
  G_overwrite_confirm = app_args.confirmreplace;
  G_move = app_args.move;

  /* Set resources for the shell */
  n = 0;
  XtSetArg(args[n], XmNallowShellResize, TRUE);	n++;
  if (G_move)
  {
    XtSetArg(args[n], XmNtitle, GETMESSAGE(2, 11, "File Manager - Folder Move"));
    n++;
  }
  else
  {
    XtSetArg(args[n], XmNtitle, GETMESSAGE(2, 12, "File Manager - Folder Copy"));
    n++;
  }

  XtSetValues(G_toplevel, args, n);

  /* Initialize images for error, warning, ... icons */
  ImageInitialize(XtDisplay(G_toplevel));

  /* Create The Copy Directory Dialog */
  copy_status = create_copydir_dialog(G_toplevel, app_args.source_name,
                                      app_args.target_name);

  if(copy_status == False)
  {
      XtRealizeWidget(G_toplevel);
      XtAppAddTimeOut(app_context, app_args.delay, TimeoutHandler, NULL);

      /* wait for user to close the dialog before exiting */
      while (!G_dialog_closed)
      {
          XtAppNextEvent(app_context, &event);
          XtDispatchEvent(&event);
      }
      exit (-1);
  }

  /* initialize status, error count, etc ... */
  sensitize_copy_action_area(G_copy_action_area_pbuttons);
  nfiles = 0;
  ndirs = -1;        /* -1 so the source folder does not get counted, only subfolders */
  UpdateSummary();
  nerr = 0;
  UpdateErrorCount();

  /* Realize the top level Widget */
  XtRealizeWidget(G_toplevel);
  XtMapWidget(G_toplevel);

  /*
   * Wait for the top level Widget to be mapped.
   * Reason:  The routine for selecting a window position for popup windows
   *   (error and overwrite warning dialogs) won't work until the main window
   *   is mapped.  Therefore, we don't want to start the actual copy operation
   *   (which might generate error or warning messages) before the main window
   *   is mapped.
   */
  XtAddEventHandler(G_toplevel, StructureNotifyMask, False, CheckForMap, NULL);
  while (!is_mapped)
    EventCheck();
  XtRemoveEventHandler(G_toplevel, XtAllEvents, True, CheckForMap, NULL);

  /* Set up the Callbacks for the copy operation */
  periodicCallback     =  EventCheck;
  syncConfirmCallback  =  ConfirmHandler;
  syncErrorCallback    =  ErrorHandler;

  /* set up parameters for the copy operation */
  app_args.sync.source = app_args.source_name;   /* source directory */
  app_args.sync.target = app_args.target_name;   /* target directory */
  app_args.sync.verbose++;
  strcpy(last_dir, app_args.sync.source);

  /*
   * Call the Copy Function. This copy function will look at the
   * even queue and see if there are any pending events. If so,
   * it will dispatch these events and then resume the copy
   * operation.  The function will return after the copy operation
   * has completed or when the user aborts the operation by
   * clicking the "Cancel" button.
   */
  SyncDirectory(&app_args.sync);

  /* if move was requested and the copy was successful, delete the source now */
  if (G_move)
  {
    if (nerr > 0)
    {
       sprintf(msg, GETMESSAGE(2, 13, "%s not removed because of errors."),
               app_args.sync.source);
       AppendErrorMsg(msg);
    }
    else
    {
      /* arrange for callback's during recursive directory delete */
      progressCallback = moveDeleteCallback;
      errorCallback = moveErrorCallback;

      /* delete the source directory */
      fsErase(app_args.sync.source, &rc, True);
    }
  }

  /* display the result */
  if (G_do_copy)
    UpdateStatus(GETMESSAGE(2, 14, "Completed."), "");
  else
    UpdateStatus(GETMESSAGE(2, 15, "Cancelled in"), last_dir);

  XtVaSetValues (G_error_msgs,
                 XmNcursorPosition, 0,
                 XmNtopCharacter,   0,
                 NULL);

  /* desensitize the copy area */
  desensitize_copy_action_area(G_copy_action_area_pbuttons);

  /* if successful popdown the window after a delay */
  if (app_args.popdown && nerr == 0)
    XtAppAddTimeOut(app_context, app_args.delay, TimeoutHandler, NULL);

  /* wait for user to close the dialog before exiting */
  while (!G_dialog_closed)
  {
    XtAppNextEvent(app_context, &event);
    XtDispatchEvent(&event);
  }

  exit(nerr);
}

