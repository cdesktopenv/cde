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
/* $TOG: main.c /main/11 1999/09/17 17:22:06 mgreess $ */
/*****************************************************************************/
/*                                                                           */
/*      main.c                                                               */
/*                                                                           */
/*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <locale.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/XmAll.h>
#include <Xm/Xm.h>
#include <Xm/Protocols.h>
#include <Xm/VendorSEP.h>
#include <Xm/XmPrivate.h> /* XmeFlushIconFileCache */
#include <Dt/Action.h>
#include <Dt/GetDispRes.h>
#include <Dt/Icon.h>
#include <Dt/EnvControlP.h>
#include <Dt/UserMsg.h>
#include <Dt/Session.h>
#include <Dt/Wsm.h>

#define GETXMSTRING(s, m, d)	XmStringCreateLocalized(GETMESSAGE(s,m,d))
#define CLASS_NAME		"Dtcreate"

/* Copied from Xm/BaseClassI.h */
extern XmWidgetExtData _XmGetWidgetExtData( 
                        Widget widget,
#if NeedWidePrototypes
                        unsigned int extType) ;
#else
                        unsigned char extType) ;
#endif /* NeedWidePrototypes */

#define NOEXTERN
#include "dtcreate.h"
#include "ca_aux.h"
#include "cmnrtns.h"
#include "ErrorDialog.h"
#include "fileio.h"

#ifdef __TOOLTALK
#include <Tt/tttk.h>
void DieFromToolTalkError(Widget, char*, Tt_status);
Tt_message ProcessToolTalkMessage( );
#endif

#if !defined(NL_CAT_LOCALE)
#define NL_CAT_LOCALE 0
#endif


/*----------------------------------------------------
 * UxXt.h needs to be included only when compiling a
 * stand-alone application.
 *---------------------------------------------------*/
#ifndef DESIGN_TIME
#include "UxXt.h"
#endif /* DESIGN_TIME */

XtAppContext    UxAppContext;
Widget          UxTopLevel;
Display         *UxDisplay;
int             UxScreen;

/*----------------------------------------------
 * Insert application global declarations here
 *---------------------------------------------*/

extern XtArgVal /* Dimension */ wintypeheight;

/*  Structure used on a save session to see if a dt is iconic  */
typedef struct {
   int state;
   Window icon;
} WM_STATE;

/*****************************************************************************/
/*                                                                           */
/* Routine:   GetSessionInfo                                                 */
/*                                                                           */
/* Purpose:   get session management saved information                       */
/*                                                                           */
/*****************************************************************************/
void GetSessionInfo( void )
{
    XrmDatabase        db;
    char               *tmpStr, *tmpStr2;
    char               *path;
    XrmName            xrm_name[5];
    XrmRepresentation  rep_type;
    XrmValue           value;
    char               *msgPtr;
    char               *fmtPtr;
    char               *errPtr;

#ifdef DEBUG
  if (debug)
    stat_out("GetSessionInfo\n");
#endif

    /*****************************************/
    /***  Open the resource database file  ***/
    /*****************************************/

    /************************************************************************/
    /*  UxTopLevel is used because CreateActionAppShell isn't created yet...*/
    /*  okay because it only uses it to get a display, not a window         */
    /************************************************************************/
    if (DtSessionRestorePath(UxTopLevel, &path, sessiondata.pszSessionFile) == False)
        path = sessiondata.pszSessionFile;
#ifdef DEBUG
    printf("session file = %s\n", path);
#endif
    if ((db = XrmGetFileDatabase (path)) == NULL)
    {
       msgPtr = GETMESSAGE(5, 162, "Could not restore session from file:");
       fmtPtr = "%s\n %s\n";
       errPtr = XtMalloc((strlen(msgPtr) + strlen(fmtPtr) +
			  strlen(path) + 1) * sizeof(char));
       sprintf(errPtr, fmtPtr, msgPtr, path);
       display_error_message(UxTopLevel, errPtr);
       XtFree(errPtr);
       if (path != sessiondata.pszSessionFile) {
          XtFree(path);
       }
       sessiondata.useSession = False;
       return;
    }
    if (path != sessiondata.pszSessionFile) {
       XtFree(path);
    }

    /*** now get the information we want from the database ***/
    /*** make sure values are at least somewhat reasonable ***/

    xrm_name[1] = '\0';

    /*******************************/
    /* get x position              */
    /*******************************/
    xrm_name[0] = XrmStringToQuark ("x");
    if (XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value))
        sessiondata.x = (Position)atoi((char *)value.addr);
    else
        sessiondata.x = 100;
    if (sessiondata.x < 0) sessiondata.x = 0;

    /*******************************/
    /* get y position              */
    /*******************************/
    xrm_name[0] = XrmStringToQuark ("y");
    if (XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value))
        sessiondata.y = (Position)atoi((char *)value.addr);
    else
        sessiondata.y = 100;
    if (sessiondata.y < 0) sessiondata.y = 0;

    /*******************************/
    /* get width                   */
    /*******************************/
    xrm_name[0] = XrmStringToQuark ("width");
    if (XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value))
        sessiondata.width = (Dimension)atoi((char *)value.addr);
    /*
    else
        sessiondata.width = DEFAULT_WIDTH;
    if (sessiondata.width < DEFAULT_WIDTH) sessiondata.width = DEFAULT_WIDTH;
    */

    /*******************************/
    /* get height                  */
    /*******************************/
    xrm_name[0] = XrmStringToQuark ("height");
    if (XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value))
        sessiondata.height = (Dimension)atoi((char *)value.addr);
    /*
    else
        sessiondata.height = DEFAULT_HEIGHT;
    if (sessiondata.height < DEFAULT_HEIGHT) sessiondata.height = DEFAULT_HEIGHT;
    */

    /*******************************/
    /* get iconic state            */
    /*******************************/
    xrm_name[0] = XrmStringToQuark ("iconic");
    if (XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value))
    {
        if ( value.addr!=NULL && strcmp((char *)value.addr, "True")==0 )
            sessiondata.iconicState = IconicState;
        else
            sessiondata.iconicState = NormalState;
    }

    /*******************************/
    /* get expert status           */
    /*******************************/
    xrm_name[0] = XrmStringToQuark ("expert");
    if (XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value))
    {
	sessiondata.showExpert = ((value.addr != NULL) &&
				  (strcmp((char *)value.addr, "True") == 0));
    }

    /*******************************/
    /* get workspace #             */
    /*******************************/
    xrm_name[0] = XrmStringToQuark ("workspace");
    if (XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value))
    {
        if ( value.addr!=NULL)
            strcpy(sessiondata.workspaces, value.addr);
        else
            sessiondata.workspaces[0] = '\0';
    }

    /*******************************/
    /* get file name               */
    /*******************************/
    xrm_name[0] = XrmStringToQuark ("file");
    if (XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value)) {
#ifdef DEBUG
       printf("file to edit = '%s'\n", value.addr);
#endif
       pszFileToEdit = (char *)XtMalloc(strlen(value.addr) + 1);
       strcpy(pszFileToEdit, value.addr);
    }
}
/*****************************************************************************/
/*                                                                           */
/* Routine:   SaveSession                                                    */
/*                                                                           */
/* Purpose:   save state information for session management                  */
/*                                                                           */
/*****************************************************************************/
void SaveSession( void )
{
    char                    *path, *name;
    int                     fd, n;
    char                    *xa_CommandStr[3];
    char                    *tmpStr, *tmpStr2;
    Position                x,y;
    Dimension               width, height;
    char                    *msgPtr, *fmtPtr, *errPtr;
    XmVendorShellExtObject  vendorExt;
    XmWidgetExtData         extData;
    WM_STATE                *wmState;
    Atom                    wmStateAtom, actualType;
    Atom		    *ws_presence = NULL;
    int                     actualFormat, j;
    unsigned long           nitems, leftover, num_workspaces=0;
    char		    all_workspace_names[256];
    char		    *workspace_name;
    Display 		    *display = XtDisplay(CreateActionAppShell);
    FILE		    *fp;

    DtSessionSavePath(CreateActionAppShell, &path, &name);
#ifdef DEBUG
    printf("path = %s\n", path);
    printf("name = %s\n", name);
#endif

    /*  Create the session file  */
    if (((fd = creat(path, S_IRUSR | S_IRGRP | S_IWUSR | S_IWGRP)) == -1) ||
	((fp = fdopen(fd, "w")) == (FILE *)NULL))
    {
       msgPtr = GETMESSAGE(5, 160, "Could not save session to file:");
       fmtPtr = "%s\n %s\n";
       errPtr = XtMalloc((strlen(msgPtr) + strlen(fmtPtr) +
			  strlen(path) + 1) * sizeof(char));
       sprintf(errPtr, fmtPtr, msgPtr, path);
       display_error_message(CreateActionAppShell, errPtr);
       XtFree(errPtr);
       if (fd != -1)
       {
	   close(fd);
	   unlink(path);
       }
       XtFree ((char *)path);
       XtFree ((char *)name);
       return;
    }

    /*  Getting the WM_STATE property to see if iconified or not */
#ifdef DEBUG
    printf("getting wm state\n");
#endif
    wmStateAtom = XInternAtom(display, "WM_STATE", False);

#ifdef DEBUG
    printf("getting window property\n");
#endif
    XGetWindowProperty (XtDisplay(CreateActionAppShell),
                        XtWindow(CreateActionAppShell), wmStateAtom, 0L,
                        (long)BUFSIZ, False, wmStateAtom, &actualType,
                        &actualFormat, &nitems, &leftover,
                        (unsigned char **) &wmState);

    if (wmState->state == IconicState)
        fprintf(fp, "*iconic: True\n");
    else
        fprintf(fp, "*iconic: False\n");

    fprintf(fp, "*expert: %s\n",
	    XtIsManaged(XprtOptionForm) ? "True" : "False");

    /*** Get and write out the geometry info for our Window ***/

#ifdef DEBUG
    printf("getting geometry info\n");
#endif

    x = XtX(CreateActionAppShell);
    y = XtY(CreateActionAppShell);
    width = XtWidth(CreateActionAppShell);
    height = XtHeight(CreateActionAppShell);

    /*  Modify x & y to take into account window mgr frames
     *  This is pretty bogus, but I don't know a better way to do it.
     */
    extData = _XmGetWidgetExtData(CreateActionAppShell, XmSHELL_EXTENSION);
    vendorExt = (XmVendorShellExtObject)extData->widget;
    x -= vendorExt->vendor.xOffset;
    y -= vendorExt->vendor.yOffset;

    fprintf(fp, "*x: %d\n", x);
    fprintf(fp, "*y: %d\n", y);
    fprintf(fp, "*width: %d\n", width);
    fprintf(fp, "*height: %d\n", height);

#ifdef DEBUG
    printf("getting workspaces this window is present \n");
#endif

    if (DtWsmGetWorkspacesOccupied (display,
    				    XtWindow(CreateActionAppShell),
				    &ws_presence, &num_workspaces) == Success)
    {
	for (j = 0; j < num_workspaces; j++)
	{
	    workspace_name = XGetAtomName (display, ws_presence[j]);
	    if (j == 0)
		(void) strcpy(all_workspace_names, workspace_name);
	    else
	    {
		(void) strcat(all_workspace_names, "*");
		(void) strcat(all_workspace_names, workspace_name);
	    }
	    XFree ((char *) workspace_name);
	}
	XtFree((char *)ws_presence);
        fprintf(fp, "*workspace: %s\n", all_workspace_names);
    }

    /*  This will be used to indicate what file is currently being edited. */
    if (pszFileToEdit != NULL) {
        fprintf(fp, "*file: %s\n", pszFileToEdit);
    }

    fflush(fp);
    fclose(fp);

    n = 0;
    xa_CommandStr[n] = pszExecName; n++;
    xa_CommandStr[n] =  "-s";       n++;
    xa_CommandStr[n] = name;        n++;

    XSetCommand(XtDisplay(CreateActionAppShell), XtWindow(CreateActionAppShell), xa_CommandStr, n);
    XtFree ((char *)path);
    XtFree ((char *)name);

    /*  Don't exit yet, SM needs time to get the new commandStr.*/
}

/*****************************************************************************/
/*                                                                           */
/* SaveSessionCB                                                             */
/*                                                                           */
/* Session is ending, save session information                               */
/*                                                                           */
/*****************************************************************************/
static void
SaveSessionCB (
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{
  char      *xa_CommandStr[3];
  int       i;

#if 0
  i = 0;
  xa_CommandStr[i] = pszExecName; i++;
  XSetCommand(XtDisplay(CreateActionAppShell), XtWindow(CreateActionAppShell), xa_CommandStr, i);
#endif

  SaveSession();
}

/*****************************************************************************/
/*                                                                           */
/*  GetIconDataFromWid                                                       */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
IconData *GetIconDataFromWid( Widget wid )
{
  int       i;
  IconData  *pIconData;

  for (i=0; i < ICON_NUMBER; i++) {
     if ((IconDataList[i]) && (IconDataList[i]->wid == wid)) {
        return(IconDataList[i]);
     }
  }
  return((IconData *)NULL);
}

/*****************************************************************************/
/*                                                                           */
/*  GetIconDataFromMsgID                                                     */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
IconData *GetIconDataFromMsgID( char *msgID )
{
  int       i;
  IconData  *pIconData;

  for (i=0; i < ICON_NUMBER; i++) {
     if (IconDataList[i]->pmMsgID) {
        if (!strcmp(IconDataList[i]->pmMsgID, msgID)) {
           return(IconDataList[i]);
        }
     }
     if (IconDataList[i]->bmMsgID) {
        if (!strcmp(IconDataList[i]->bmMsgID, msgID)) {
           return(IconDataList[i]);
        }
     }
  }
  /*
  for (i=0; i < ICON_NUMBER; i++) {
     if ( ((IconDataList[i]->pmMsgID) &&
           (!strcmp(IconDataList[i]->pmMsgID, msgID))) ||
          ((IconDataList[i]->bmMsgID) &&
           (!strcmp(IconDataList[i]->bmMsgID, msgID))) ) {
        return(IconDataList[i]);
     }
  }
  */
  return((IconData *)NULL);
}

/*****************************************************************************/
/*                                                                           */
/*  RemoveTmpIconFiles                                                       */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
void RemoveTmpIconFiles( void )
{
  int       i;
  IconData  *pIconData;
  char      maskFile[MAXFILENAME];

  for (i=0; i < ICON_NUMBER; i++) {
     if ( (IconDataList[i]->pmDirtyBit) &&
          (IconDataList[i]->pmFileName) &&
          (strlen(IconDataList[i]->pmFileName)) ) {
#ifdef DEBUG
        printf("RemoveTmpIconFiles: unlink '%s'\n", IconDataList[i]->pmFileName);  /* debug */
#endif
        unlink(IconDataList[i]->pmFileName);
        /* This is bogus.  But just in case the user specified a bitmap */
        /* rather than a pixmap during the edit process, this will make */
        /* sure that file gets removed.  The chance of this occurring   */
        /* is very unlikely.                                            */
        strcpy(maskFile, IconDataList[i]->pmFileName);
        strcat(maskFile, "_m");

#ifdef DEBUG
        printf("Remove maskFile = %s\n", maskFile); /* debug */
#endif
        unlink(maskFile);
     }
     if ( (IconDataList[i]->bmDirtyBit) &&
          (IconDataList[i]->bmFileName) &&
          (strlen(IconDataList[i]->bmFileName)) ) {
#ifdef DEBUG
        printf("RemoveTmpIconFiles: unlink '%s'\n", IconDataList[i]->bmFileName);  /* debug */
#endif
        unlink(IconDataList[i]->bmFileName);
        strcpy(maskFile, IconDataList[i]->bmFileName);
        strcat(maskFile, "_m");
#ifdef DEBUG
        printf("Remove maskFile = %s\n", maskFile); /* debug */
#endif
        unlink(maskFile);
     }
  }
}

#ifdef __TOOLTALK
/*****************************************************************************/
/*                                                                           */
/*  DieFromToolTalkError                                                     */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
static void
ExitCB (Widget dialog, XtPointer client_data, XtPointer call_data)
{
    exit((int)(XtArgVal) client_data);
}

void
DieFromToolTalkError(Widget parent, char *errfmt, Tt_status status)
{
    Arg		 args[10];
    Widget	 dialog, dialogShell;
    char	*errmsg, *statmsg, *title;
    XmString	 xms_errmsg, xms_ok, xms_title;
    int		 n;

    if (! tt_is_err(status)) return;

    statmsg = tt_status_message(status);
    /* Solaris dtcreate ignores this so we should too */
    if(status == TT_ERR_PTYPE)
            return;
    errmsg = XtMalloc(strlen(errfmt) + strlen(statmsg) + 2);
    sprintf(errmsg, errfmt, statmsg);

    xms_ok = GETXMSTRING(13, 3, "OK");
    xms_errmsg = XmStringCreateLocalized(errmsg);
    xms_title = GETXMSTRING(13, 1, "Create Action - Error");

    n = 0;
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
    XtSetArg(args[n], XmNokLabelString, xms_ok); n++;
    XtSetArg(args[n], XmNdialogTitle, xms_title); n++;
    XtSetArg(args[n], XmNmessageString, xms_errmsg); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); n++;

    dialog = XmCreateErrorDialog(parent, "IconEditorError", args, n);
    XtAddCallback(dialog, XmNokCallback, ExitCB, (XtPointer) status);
    XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON));
    XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON));

    /*
     * Disable the frame menu from dialog since we don't want the user
     * to be able to close dialogs with the frame menu
     */
    dialogShell = XtParent(dialog);
    n = 0;
    XtSetArg(args[n], XmNmwmDecorations, MWM_DECOR_ALL | MWM_DECOR_MENU); n++;
    XtSetValues(dialogShell, args, n);
    XtManageChild(dialog);
    XtRealizeWidget(dialogShell);

    _DtSimpleError(CLASS_NAME, DtFatalError, NULL, errmsg);

    XtFree(errmsg);
    XmStringFree(xms_ok);
    XmStringFree(xms_errmsg);
    XmStringFree(xms_title);

    while (TRUE)
      XtAppProcessEvent(XtWidgetToApplicationContext(dialog), XtIMAll);
}

/*****************************************************************************/
/*                                                                           */
/*  ProcessToolTalkMessage                                                   */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
Tt_message ProcessToolTalkMessage(Tt_message msg,
                                  void* cdata,
                                  Tt_message contract )
{

  int ttMark = tt_mark();

  if( 0 == strcmp( tt_message_op( msg ), "Quit" ) ) {
          /*
           * Do the necessary cleanup, call tt_message_reply(),
           * and exit.  Since this is not yet fully implemented, just
           * call tt_message_reply and exit.
           */

#ifdef DEBUG
          printf("ProcessTTMsg: Quit\n");  /* debug */
#endif
          tt_message_reply(msg);
          RemoveTmpIconFiles();
          exit(1);
  } else {
          /*
           * Any other desktop message ops that one would want
           * to implement would go here.  If
           * a mapped, realized, top-level widget were passed into
           * ttdt_session_join instead of (Widget) 0, then the
           * messages that interact with the X environment
           * Get_Geometry, etc.) would be handled automatically.
           */

#ifdef DEBUG
          printf("ProcessTTMsg: Other\n");
#endif

  }
  tt_release( ttMark );

  return (Tt_message) 0;
}

/*****************************************************************************/
/* NAME:        IconEdit_tt_handler                                          */
/* DESCRIPTION: Handler for Tooltalk conversation about editing icon.        */
/* PARAMETERS:                                                               */
/* RETURN:      none                                                         */
/* CREATION:    02/21/94                                                     */
/* REVISIONS:                                                                */
/*****************************************************************************/
Tt_callback_action IconEdit_tt_handler( Tt_message m, Tt_pattern p )
{
  int             ttMark;
  int             blen;
  int             wlen;
  int             tmpfd = -1;
  Tt_state        mstate;
  char            *msgPtr, *fmtPtr, *errPtr;
  char            tmpbuf[MAXBUFSIZE];
  char*           ttbuf;
  char*           op;
  char            *tmpIconFile;
  char            *msgID;
  IconData        *pIconData;
  Boolean         bIsNewFile = False;

  char            *tmp = (char *)NULL;
  char            *tmp1 = (char *)NULL;
  int             tmplen;

  ttMark = tt_mark( );

  op = tt_message_op(m);
  /************************************************************************/
  /* Edit message...                                                      */
  /************************************************************************/
  if (!strcmp(op, "Edit")) {
#ifdef DEBUG
     printf("IconEdit_tt_handler: Edit tt msg = %p\n", m);  /* debug */
#endif
     mstate = tt_message_state( m );
     switch ((int)mstate) {
         case TT_FAILED:
             TurnOffHourGlassAllWindows();

	     /* INTERNATIONALIZE */
	     msgPtr = "Failed ToolTalk msg";
	     fmtPtr = "%s = %p";
	     errPtr = XtMalloc((strlen(msgPtr) + strlen(fmtPtr) +
				20) * sizeof(char));
             sprintf(errPtr, fmtPtr, msgPtr, m);
             display_error_message(CreateActionAppShell, errPtr);
	     XtFree(errPtr);
#ifdef DEBUG
             printf( "Failed ToolTalk msg = %p\n", m);   /* debug */
#endif
             tt_message_destroy( m );
             break;
         case TT_HANDLED:
             TurnOffHourGlassAllWindows();
#ifdef DEBUG
             printf( "Handled ToolTalk msg = %p\n", m);  /* debug */
#endif
             tt_message_destroy( m );
             break;
#ifdef DEBUG
         case TT_STARTED:
             printf( "Started ToolTalk msg = %p\n", m);  /* debug */
#endif
     }
  } else {
#ifdef DEBUG
     printf("IconEdit_tt_handler: Saved tt msg = %p\n", m);  /* debug */
#endif
     msgID = tt_message_arg_val(m, 1);
     pIconData = GetIconDataFromMsgID(msgID);
#ifdef DEBUG
     printf("msgID = '%s'\n", msgID);  /* debug */
#endif

     /*********************************************************************/
     /* Saved message...                                                  */
     /*********************************************************************/
     if ( (pIconData) && (!strcmp(op, "Saved")) ) {
        tt_message_arg_bval(m, 0, (unsigned char **)&ttbuf, &blen);

        if ( (pIconData->pmMsgID) && (!strcmp(pIconData->pmMsgID, msgID)) ) {
           if (pIconData->pmDirtyBit) {
              tmpIconFile = pIconData->pmFileName;
           } else {
              tmpnam(pIconData->pmFileName);
              bIsNewFile = True;
              tmpIconFile = pIconData->pmFileName;
           }
        } else {
           if (pIconData->bmDirtyBit) {
              tmpIconFile = pIconData->bmFileName;
           } else {
              tmpnam(pIconData->bmFileName);
              bIsNewFile = True;
              tmpIconFile = pIconData->bmFileName;
           }
        }
        if (bIsNewFile) {
            tmpfd = open(tmpIconFile, O_CREAT | O_WRONLY | O_NDELAY, 0666);
        } else {
           tmpfd = open(tmpIconFile, O_TRUNC | O_WRONLY | O_NDELAY, 0666);
        }
        if (tmpfd == -1) {
	   msgPtr = GETMESSAGE(5, 60, "Could not open the following file:");
	   fmtPtr = "%s\n   %s";
	   errPtr = XtMalloc((strlen(msgPtr) + strlen(fmtPtr) +
			      strlen(tmpIconFile) + 1) * sizeof(char));
	   sprintf(errPtr, fmtPtr, msgPtr, tmpIconFile);
	   display_error_message(XtParent(pIconData->wid), errPtr);
	   XtFree(errPtr);
           tt_release( ttMark );
           return (TT_CALLBACK_CONTINUE);
        }

        /*******************************************************************/
        /* Determine if this is a pixmap or bitmap.  If it is a pixmap,    */
        /* handle as a one file buffer.  If a bitmap, then it may be       */
        /* double buffered including the mask.                             */
        /*******************************************************************/

        /*******************************************************************/
        /* Check for Pixmap first.                                         */
        /*******************************************************************/
        if ( (ttbuf) && (!strncmp(ttbuf, "/* XPM */", 9)) ) {
#ifdef DEBUG
           printf("This is a XPM file\n");  /* debug */
#endif
           tmp = strstr(ttbuf, "};");
           if (tmp) {
              tmp += 2;                        /* Go one char past the ";" */
              /*
              tmplen = blen - strlen(tmp) + 1;
              printf("Amro tmplen = %d\n", tmplen);
              */
              tmplen = (char *)tmp - (char *)ttbuf;
#ifdef DEBUG
              printf("Troy tmplen = %d\n", tmplen); /* debug */
#endif
           } else {
              tmplen = blen;
           }
           if ((wlen = write(tmpfd, ttbuf, tmplen)) == tmplen) {
              if (tmpfd > -1) close(tmpfd);
           } else {
	      /* INTERNATIONALIZE */
	      msgPtr = "Could not write to the following file:";
	      fmtPtr = "%s\n   %s";
	      errPtr = XtMalloc((strlen(msgPtr) + strlen(fmtPtr) +
				 strlen(tmpIconFile) + 1) * sizeof(char));
              sprintf(errPtr, fmtPtr, msgPtr, tmpIconFile);
              display_error_message(XtParent(pIconData->wid), errPtr);
	      XtFree(errPtr);
              tt_release( ttMark );
              if (tmpfd > -1) close(tmpfd);
              return (TT_CALLBACK_CONTINUE);
           }

        /*******************************************************************/
        /* Must be a Bitmap.                                               */
        /*******************************************************************/
        } else {
#ifdef DEBUG
           printf("This is a XBM file\n");  /* debug */
           printf("ttbuffer size = %d\n", blen); /* debug */
#endif
           /****************************************************************/
           /* Determine size of first bitmap.                              */
           /****************************************************************/
           tmp = strstr(ttbuf, "};");
           if (tmp) {
              tmp += 2;                        /* Go one char past the ";" */
              /*
              tmplen = blen - strlen(tmp) + 1;
              printf("Amro tmplen = %d\n", tmplen);
              */
              tmplen = (char *)tmp - (char *)ttbuf;
#ifdef DEBUG
              printf("Troy tmplen = %d\n", tmplen); /* debug */
#endif
           } else {
              tmplen = blen;
           }
#ifdef DEBUG
           printf("File 1 size = %d\n", tmplen); /* debug */
#endif

           /****************************************************************/
           /* Write out first bitmap file.                                 */
           /****************************************************************/
#ifdef DEBUG
           printf("Writing first bitmap file\n"); /* debug */
#endif
           if ((wlen = write(tmpfd, ttbuf, tmplen)) == tmplen) {
              if (tmpfd > -1) close(tmpfd);
           } else {
	      /* INTERNATIONALIZE */
	      msgPtr = "Could not write to the following file:";
	      fmtPtr = "%s\n   %s";
	      errPtr = XtMalloc((strlen(msgPtr) + strlen(fmtPtr) +
				 strlen(tmpIconFile) + 1) * sizeof(char));
              sprintf(errPtr, fmtPtr, msgPtr, tmpIconFile);
              display_error_message(XtParent(pIconData->wid), errPtr);
	      XtFree(errPtr);
              tt_release( ttMark );
              if (tmpfd > -1) close(tmpfd);
              return (TT_CALLBACK_CONTINUE);
           }

           /****************************************************************/
           /* Now determine if there is a second bitmap and its size.      */
           /****************************************************************/
           if (tmp) {
              tmp1 = strstr(tmp, "};");
           }
           sprintf(tmpbuf, "%s_m", tmpIconFile);
           if (tmp1) {
              tmp1 += 2;                       /* Go one char past the ";" */
#ifdef DEBUG
              printf("Mask file name = '%s'\n", tmpbuf); /* debug */
#endif
              tmpfd = open(tmpbuf, O_CREAT | O_WRONLY | O_NDELAY, 0666);
              if (tmpfd == -1) {
		 msgPtr =
		     GETMESSAGE(5, 60, "Could not open the following file:");
		 fmtPtr = "%s\n   %s";
		 errPtr = XtMalloc((strlen(msgPtr) + strlen(fmtPtr) +
				    strlen(tmpbuf) + 1) * sizeof(char));
                 sprintf(errPtr, fmtPtr, msgPtr, tmpbuf);
                 display_error_message(XtParent(pIconData->wid), errPtr);
		 XtFree(errPtr);
                 tt_release( ttMark );
                 return (TT_CALLBACK_CONTINUE);
              }

#ifdef DEBUG
              printf("writing mask file \n");
#endif
              /*
              tmplen = strlen(tmp);
              printf("Amro tmplen = %d\n", tmplen);
              */
              tmplen = (char *)tmp1 - (char *)tmp;
#ifdef DEBUG
              printf("Troy tmplen = %d\n", tmplen); /* debug */
#endif
              if ((wlen = write(tmpfd, tmp, tmplen)) == tmplen) {
                 if (tmpfd > -1) close(tmpfd);
              } else {
		 /* INTERNATIONALIZE */
		 msgPtr = "Could not write to the following file:";
		 fmtPtr = "%s\n   %s";
		 errPtr = XtMalloc((strlen(msgPtr) + strlen(fmtPtr) +
				    strlen(tmpbuf) + 1) * sizeof(char));
                 sprintf(errPtr, fmtPtr, msgPtr, tmpbuf);
                 display_error_message(XtParent(pIconData->wid), errPtr);
		 XtFree(errPtr);
                 tt_release( ttMark );
                 if (tmpfd > -1) close(tmpfd);
                 return (TT_CALLBACK_CONTINUE);
              }

           /****************************************************************/
           /* Must not have a mask, so lets clean up any old mask file.    */
           /****************************************************************/
           } else {
#ifdef DEBUG
              printf("Remove maskFile = %s\n", tmpbuf); /* debug */
#endif
              unlink(tmpbuf);
           }
        }

        /****************************************************************/
        /* Set the icon gadget with new icon, and flag the dirty bit.   */
        /****************************************************************/
        if ( ( (pIconData->pmMsgID) &&
               (!strcmp(pIconData->pmMsgID, msgID)) &&
               (bShowPixmaps) )
           ||
             ( (pIconData->bmMsgID) &&
               (!strcmp(pIconData->bmMsgID, msgID)) &&
               (!bShowPixmaps) ) ) {
           XmeFlushIconFileCache(NULL);
#ifdef DEBUG
           printf("Change icon image for widget = %p\n", pIconData->wid); /* debug */
#endif
           SET_ICONGADGET_ICON(pIconData->wid, tmpIconFile);
        }
        if ((pIconData->pmMsgID) && (!strcmp(pIconData->pmMsgID, msgID))) {
           pIconData->pmDirtyBit = True;
        } else {
           pIconData->bmDirtyBit = True;
        }
#ifdef DEBUG
        printf("**********tmp file name = %s\n", tmpIconFile);  /* debug */
#endif
     }
  }
  tt_release( ttMark );
  return( TT_CALLBACK_PROCESSED );
}

/*****************************************************************************/
/* NAME:        UxDoEditPixmap                                               */
/* DESCRIPTION: Loads the icon editor.                                       */
/* PARAMETERS:  none                                                         */
/* RETURN:      none                                                         */
/* CREATION:    02/11/94                                                     */
/* REVISIONS:                                                                */
/*****************************************************************************/
void UxDoEditPixmap(Widget wid, char *fname)
{
  Tt_message     m;         /* ToolTalk message */
  Tt_pattern     p;         /* ToolTalk dynamic pattern */
  Tt_status      ttRc;      /* ToolTalk status  */

  int            fd1 = -1;
  int            fd2 = -1;

  int            lenFile1 = 0, rlenFile1 = 0;
  int            lenFile2 = 0, rlenFile2 = 0;

  int            mark;
  char           *msgID;
  char           **ppMsgID;
  IconData       *pIconData;
  unsigned char  *buffer;
  char           *ptr;

  char           tmpbuf[MAXBUFSIZE];
  char           *errPtr;
  char           *msgPtr;
  char           *fmtPtr;
  char           *mname = (char *)NULL;
  char           *type_name, *size_name;


#ifdef DEBUG
  if (fname) printf("Edit filename: '%s'\n", fname);  /* debug */
#endif

  mark = tt_mark();

  /*********************************************************************/
  /* Create request                                                    */
  /*********************************************************************/
  m = tt_prequest_create( TT_SESSION, "Edit" );
  msgID = tt_message_id(m);
  pIconData = GetIconDataFromWid(wid);
  if (bShowPixmaps) {
     ppMsgID = &(pIconData->pmMsgID);
  } else {
     ppMsgID = &(pIconData->bmMsgID);
  }
  if (*ppMsgID) {
     free(*ppMsgID);
  }
  if (msgID && strlen(msgID)) {
     *ppMsgID = (char *)malloc(strlen(msgID) + 1);
     strcpy(*ppMsgID, msgID);
  }

#ifdef DEBUG
  printf("msgID = '%s'\n", msgID);  /* debug */
  printf("Create Edit msg = %p\n", m);  /* debug */
#endif

  /*********************************************************************/
  /* Add arguments to message                                          */
  /*********************************************************************/
  if (fname) {
     fd1 = open(fname, O_RDONLY);
     if (fd1 == -1) {
        /* Error opening icon file! */
	msgPtr = GETMESSAGE(5, 60, "Could not open the following file:");
	fmtPtr = "%s\n   %s";
	errPtr = XtMalloc((strlen(msgPtr) + strlen(fmtPtr) +
			   strlen(fname) + 1) * sizeof(char));
        sprintf(errPtr, fmtPtr, msgPtr, fname);
        display_error_message(XtParent(wid), errPtr);
	XtFree(errPtr);
        return;
     } else {
        /***************************************/
        /* Find out how big a buffer is needed */
        /***************************************/
        lenFile1 = lseek(fd1, 0, SEEK_END);
        (void) lseek(fd1, 0, SEEK_SET);

        /*************************************************************/
        /* Read first few bytes to determine what kind of file it is */
        /*************************************************************/
        read(fd1, tmpbuf, 10);
        (void) lseek(fd1, 0, SEEK_SET);

        /******************************************************************/
        /* If the file is a bitmap, then check for its mask file and add. */
        /******************************************************************/
        if ( strncmp(tmpbuf, "/* XPM */", 9) ) {

           mname = CreateMaskName(fname);

           /***************************************************************/
           /* Check if mask file exists.                                  */
           /***************************************************************/
           if ((mname) && (check_file_exists(mname))) {
              fd2 = open(mname, O_RDONLY);
              if (fd2 == -1) {
                 /* Error opening icon file! */
		 msgPtr =
		     GETMESSAGE(5, 60, "Could not open the following file:");
		 fmtPtr = "%s\n   %s";
		 errPtr = XtMalloc((strlen(msgPtr) + strlen(fmtPtr) +
				    strlen(mname) + 1) * sizeof(char));
                 sprintf(errPtr, fmtPtr, msgPtr, mname);
                 display_error_message(XtParent(wid), errPtr);
		 XtFree(errPtr);
                 XtFree(mname);
                 return;
              } else {
                 /***************************************/
                 /* Find out how big a buffer is needed */
                 /***************************************/
                 lenFile2 = lseek(fd2, 0, SEEK_END);
                 (void) lseek(fd2, 0, SEEK_SET);
              }
           }
        }
        /******************************************************************/
        /* Create 1 big buffer                                            */
        /******************************************************************/
#ifdef DEBUG
        printf("Size of file 1 = %d\n", lenFile1); /* debug */
        printf("Size of file 2 = %d\n", lenFile2); /* debug */
#endif
        buffer = (unsigned char *)XtMalloc((Cardinal)(lenFile1 + lenFile2 + 1));
        memset(buffer, 0, (lenFile1 + lenFile2 + 1));
        if (buffer) {
           /***************************************/
           /* Read file1 into buffer.             */
           /***************************************/
           if ((rlenFile1 = read(fd1, buffer, lenFile1)) != lenFile1) {
              /* didn't read whole file! */

	      /* INTERNATIONALIZE */
	      msgPtr = "Could not read the entire file:";
	      fmtPtr = "%s\n   %s";
	      errPtr = XtMalloc((strlen(msgPtr) + strlen(fmtPtr) +
				 strlen(fname) + 1) * sizeof(char));
              sprintf(errPtr, fmtPtr, msgPtr, fname);
              display_error_message(XtParent(wid), errPtr);
	      XtFree(errPtr);
              if (mname) XtFree(mname);
              XtFree((char *) buffer);
              return;
           }
           if (lenFile2) {
#ifdef DEBUG
              printf("buffer = %p\n", buffer);
#endif
            /*ptr = (char *)&(buffer[lenFile1 + 1]);*/
              ptr = (char *)&(buffer[lenFile1]);
#ifdef DEBUG
              printf("ptr = %p\n", ptr);
#endif
              if ((rlenFile2 = read(fd2, ptr, lenFile2)) != lenFile2) {
                 /* didn't read whole file! */

		 /* INTERNATIONALIZE */
		 msgPtr = "Could not read the entire file:";
		 fmtPtr = "%s\n   %s";
		 errPtr = XtMalloc((strlen(msgPtr) + strlen(fmtPtr) +
				    strlen(fname) + 1) * sizeof(char));
		 sprintf(errPtr, fmtPtr, msgPtr, mname);
                 display_error_message(XtParent(wid), errPtr);
		 XtFree(errPtr);
                 XtFree((char *) buffer);
                 if (mname) XtFree(mname);
                 return;
              }
           }
           if (fd1 > -1) close(fd1);
           if (fd2 > -1) close(fd2);

#ifdef DEBUG
           printf("final buffer = '%s'\n", buffer); /* debug */
           printf("Buffer = '%s'\n", buffer); /* debug */
#endif
           tt_message_barg_add( m, TT_INOUT, "XPM", buffer, lenFile1 + lenFile2 + 1);
           if (mname) XtFree(mname);
           XtFree((char *) buffer);
        }
     }
  } else {
     tt_message_barg_add( m, TT_INOUT, "XPM", (const unsigned char *)NULL, 0);
  }
  tt_message_arg_add(m, TT_IN, "messageID", msgID);

  /******************************************************************/
  /* Create a pattern and add a callback to get notifications       */
  /* of periodic saves                                              */
  /******************************************************************/
  p = tt_pattern_create();
  tt_pattern_category_set(p, TT_OBSERVE);
  tt_pattern_scope_add(p, TT_SESSION);
  tt_pattern_session_add(p, tt_default_session());
  tt_pattern_address_add(p, TT_HANDLER);
  tt_pattern_op_add(p, "Saved");
  tt_pattern_callback_add(p, IconEdit_tt_handler);
  tt_pattern_register(p);

  /******************************************************************/
  /* Add callback to notify when change complete                    */
  /******************************************************************/
  tt_message_callback_add( m, IconEdit_tt_handler );

  /******************************************************************/
  /* Send message                                                   */
  /******************************************************************/
  TurnOnHourGlassAllWindows();
  ttRc = tt_message_send( m );
  DieFromToolTalkError( UxTopLevel, "tt_message_send", ttRc );
}
#endif  /* __TOOLTALK */

/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*    main                                                                   */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
int
main(int argc, char *argv[])
{
        /*-----------------------------------------------------------
         * Declarations.
         * The default identifier - mainIface will only be declared
         * if the interface function is global and of type swidget.
         * To change the identifier to a different name, modify the
         * string mainIface in the file "xtmain.dat". If "mainIface"
         * is declared, it will be used below where the return value
         * of  PJ_INTERFACE_FUNCTION_CALL will be assigned to it.
         *----------------------------------------------------------*/
#ifdef __TOOLTALK
        char * procId;
        int ttMark, ttFd;
	Tt_pattern *ttPatt;
        Tt_status ttRc;
        char * errfmt;
        char * sessionString;
        char * ttsession;
#endif

        Widget          mainIface;
        extern swidget  create_AddFiletype();
        extern swidget  create_FileCharacteristics();
        extern swidget  create_OpenFile();
        extern swidget  create_Confirmed();
        extern Widget   create_ErrorDialog();
        int             rc;
        Atom            xa_WM_SAVE_YOURSELF;
        char            *ptr;

        /*---------------------------------
         * Interface function declaration
         *--------------------------------*/
        Widget  create_applicationShell1(swidget);
        swidget UxParent = NULL;
        int i;

        /*---------------------
         * Initialize program
         *--------------------*/

        _DtEnvControl(DT_ENV_SET);
        XtSetLanguageProc(NULL, NULL, NULL);
        pszFileToEdit = (char *)NULL;
        memset(&sessiondata, 0, sizeof(SessionData));
        memset(IconDataList, 0, sizeof(IconDataList));

        /********************************************************************/
        /* Parse the command line arguments.                                */
        /********************************************************************/
        pszExecName = argv[0];

        if (argc > 1) {
          for (i=1; i<argc; i++) {
            if ((argv[i][0] == '-') && (argv[i][1] == '?')) {
	       setlocale(LC_ALL, "");
	       nlmsg_fd = catopen("dtcreate", NL_CAT_LOCALE);
               printf("%s\n",
                      GETMESSAGE(5, 110, "Usage: dtcreate [<filename>] [-?]"));
               exit(0);
            } else {
               if (argv[i][0] == '-') {
                  if (!strcmp(argv[i], "-s")) {
                     i+=1;
                     sessiondata.pszSessionFile = (char *)XtMalloc(strlen(argv[i]) + 1);
                     strcpy(sessiondata.pszSessionFile, argv[i]);
                     sessiondata.useSession = True;
                  } else {
                     i+=1;
                  }
               } else {
                  pszFileToEdit = (char *)XtMalloc(strlen(argv[i]) + 1);
                  strcpy(pszFileToEdit, argv[i]);
               }
            }
          }
        }


        UxTopLevel = XtAppInitialize(&UxAppContext, CLASS_NAME,
                                     NULL, 0, &argc, argv, NULL, NULL, 0);

        nlmsg_fd = catopen("dtcreate", NL_CAT_LOCALE);

        UxDisplay = XtDisplay(UxTopLevel);
        UxScreen = XDefaultScreen(UxDisplay);

        if (sessiondata.useSession) {
           GetSessionInfo();
        }

        /*---------------------
         * Determine resolution
         *--------------------*/
        if (_DtGetDisplayResolution(UxDisplay, UxScreen) == 1) {
           bLowRes = True;
        } else {
           bLowRes = False;
        }

#ifdef DEBUG_LOWRES
        bLowRes = True;
#endif

        /*-------------------------------------------------------
         * Insert initialization code for your application here
         *------------------------------------------------------*/
#ifdef __TOOLTALK
#ifdef DEBUG
        printf("display string = '%s'\n", DisplayString(UxDisplay));
#endif
        ttsession = getenv("TT_SESSION");
        if (!ttsession || strlen(ttsession)==0) {
	   char *ttxsession = tt_X_session(XDisplayString(UxDisplay));
           ttRc = tt_default_session_set(ttxsession);
        }

        ttMark = tt_mark( );

        procId = tt_open( );
	ttRc = tt_ptr_error(procId);
        errfmt =
          GETMESSAGE(13, 2, "Could not connect to ToolTalk:\n%s\nExiting ...");
        DieFromToolTalkError( UxTopLevel, errfmt, ttRc );

        ttFd = tt_fd( );
        DieFromToolTalkError( UxTopLevel, "tt_fd", tt_int_error( ttFd ) );

        ttRc = tt_ptype_declare( "dtcreate" );
        DieFromToolTalkError( UxTopLevel, "tt_ptype_declare", ttRc );

        ttRc = tt_session_join( tt_default_session( ));
        DieFromToolTalkError( UxTopLevel, "tt_session_join", ttRc );

        XtAppAddInput( UxAppContext, ttFd, (XtPointer)XtInputReadMask,
                            (XtInputCallbackProc)ProcessToolTalkMessage, 0 );
#endif

        if (!pszFileToEdit) {
           initAD(&AD);
        }


        /*----------------------------------------------------------------
         * Create and popup the first window of the interface.  The
         * return value can be used in the popdown or destroy functions.
         * The Widget return value of  PJ_INTERFACE_FUNCTION_CALL will
         * be assigned to "mainIface" from  PJ_INTERFACE_RETVAL_TYPE.
         *---------------------------------------------------------------*/

        mainIface = create_applicationShell1(UxParent);
        AddFiletype = create_AddFiletype(mainIface);
        FileCharacteristics = create_FileCharacteristics(AddFiletype);
        OpenFile = create_OpenFile();

        UxPopupInterface(mainIface, no_grab);
        if (pszFileToEdit) {
           if (!OpenDefinitionFile(pszFileToEdit, &AD)) {
              writeCAToGUI(&AD);
           }
        }
        XtVaGetValues (CA_WindowTypeArea, XmNheight, &wintypeheight, NULL);

        /*------------------------------------------------------------------*/
        /*   Register for save and restore.                                 */
        /*------------------------------------------------------------------*/
        xa_WM_SAVE_YOURSELF = XInternAtom(XtDisplay(CreateActionAppShell), "WM_SAVE_YOURSELF", False);
        XmAddWMProtocols(CreateActionAppShell, &xa_WM_SAVE_YOURSELF, 1);
        XmAddWMProtocolCallback(CreateActionAppShell, xa_WM_SAVE_YOURSELF,
                                SaveSessionCB, (XtPointer)CreateActionAppShell);

        /*------------------------------------------------------------------*/
        /* Make a copy of the AD data structure.  This will be used later   */
        /* when the user wants to exit to determine if the latest data has  */
        /* been saved or not.                                               */
        /*------------------------------------------------------------------*/
        pMyCopyAD = copyAD(&AD);

#ifdef __TOOLTALK
        DtDbLoad();
        ttMark = tt_mark( );

        procId = ttdt_open( &ttFd, "dtcreate", "CDE", "1.0", 1 );
	ttRc = tt_ptr_error(procId);
        errfmt =
          GETMESSAGE(13, 2, "Could not connect to ToolTalk:\n%s\nExiting ...");
        DieFromToolTalkError( UxTopLevel, errfmt, ttRc );

        ttFd = tt_fd( );
	ttRc = tt_int_error( ttFd );
        DieFromToolTalkError( UxTopLevel, "tt_fd", ttRc );

        sessionString = tt_default_session();
	ttRc = tt_ptr_error( sessionString );
        DieFromToolTalkError( UxTopLevel, "tt_default_session", ttRc );

	ttPatt = ttdt_session_join( sessionString, ProcessToolTalkMessage,
				    (Widget) 0, (void *) 0, 1 );
        ttRc = tt_ptr_error(ttPatt);
        DieFromToolTalkError( UxTopLevel, "ttdt_session_join", ttRc );

        XtAppAddInput( UxAppContext, ttFd, (XtPointer)XtInputReadMask,
                            tttk_Xt_input_handler, 0 );
#endif

        /*-----------------------
         * Enter the event loop
         *----------------------*/

        XtAppMainLoop (UxAppContext);

#ifdef __TOOLTALK
        ttdt_session_quit(0, 0, 1);
        tt_close( );
        tt_release( ttMark );
        RemoveTmpIconFiles();
#endif
}
