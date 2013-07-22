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
/* $TOG: HelpUtil.c /main/19 1998/04/09 17:43:30 mgreess $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        HelpUtil.c
 **
 **   Project:     Rivers Project
 **
 **   Description: 
 ** 
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 **
 ****************************************************************************
 ************************************<+>*************************************/


#include <sys/param.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  /* R_OK */
#include <sys/stat.h>

#include <sys/types.h>
#define X_INCLUDE_PWD_H
#define XOS_USE_XT_LOCKING
#include <X11/Xos_r.h>

#include <Xm/Xm.h>
#include <Xm/XmP.h>
#include <Xm/MwmUtil.h>

#include <Xm/MessageB.h>
#include <X11/Shell.h>
#include <X11/Intrinsic.h>
#include <X11/cursorfont.h>

/* private includes */
#include "Access.h"
#include "bufioI.h"
#include "DisplayAreaI.h"
#include "DisplayAreaP.h"

#include <Dt/Help.h>
#include "HelpI.h"
#include "HelpP.h"
#include "StringFuncsI.h"
#include "HelpDialogI.h"
#include "HelpDialogP.h"
#include "HelpUtilI.h"
#include "HelposI.h"
#include "HyperTextI.h"
#include "FormatI.h"
#include "MessagesP.h"
#include "HelpQuickD.h"
#include "SetListI.h"
#include "DestroyI.h"
#include "HelpAccessI.h"
#include "FileUtilsI.h"
#include "HourGlassI.h"
#include "Lock.h"

#include <Dt/DtNlUtils.h>

/******* global variables *******/
char _DtHelpDefaultHelp4HelpVolume[] = "Help4Help";
char _DtHelpDefaultLocationId[] = "_HOMETOPIC";

/**** Help Util Error message Defines ****/

#define UtilMessage0	_DtHelpMsg_0010
#define UtilMessage2	_DtHelpMsg_0011


static void _DtMessageClose(
    Widget w,
    XtPointer client_data,
    XEvent *event);
static void  CloseDefBoxCB(
    Widget w,
    XtPointer client_data,
    XtPointer call_data);

/* Macro for finding a point within a gadget.
 * Its used for item help 
 */
#define  PT_IN_CHILD(X, Y, CHILD) \
	 ((((int)(X)) >= ((int) (CHILD)->core.x)) && \
          (((int)(X)) <= ((int)((CHILD)->core.x + (CHILD)->core.width))) && \
          (((int)(Y)) >= ((int) (CHILD)->core.y)) && \
          (((int)(Y)) <= ((int)((CHILD)->core.y + (CHILD)->core.height))))


/******** useful constants ********/
#define EOS  '\0'
#define DIR_SLASH '/'

#define HUSET 8     /* message catalog set */

/******** static variables ********/


/******** data structures ********/
typedef struct ExecContext 
{
   char * command;
   XtPointer pDisplayArea;
} ExecContext;

/******** The onitem cursor (32x32, xbm format) ********/
#define onitem32_width 32
#define onitem32_height 32
#define onitem32_x_hot 0
#define onitem32_y_hot 0
static unsigned char onitem32_bits[] = {
   0xff, 0xff, 0xff, 0xff, 0xfd, 0xff, 0x1f, 0xfc, 0xf9, 0xff, 0xe7, 0xf3,
   0xf1, 0xff, 0xfb, 0xef, 0xe1, 0xff, 0xfd, 0xdf, 0xc1, 0xff, 0xfd, 0xdf,
   0x83, 0xff, 0xfe, 0xbf, 0x03, 0xff, 0x7e, 0x7e, 0x03, 0xfe, 0xbe, 0x7d,
   0x03, 0xfc, 0xbe, 0x7d, 0x03, 0xf0, 0xc1, 0x7d, 0x03, 0xe0, 0xff, 0x7e,
   0x07, 0xc0, 0x7f, 0xbf, 0x07, 0x80, 0xbf, 0xbf, 0x07, 0x00, 0xde, 0xdf,
   0x07, 0x00, 0xdc, 0xef, 0x07, 0x00, 0xdf, 0xf7, 0x07, 0x80, 0xdf, 0xfb,
   0x0f, 0xc0, 0xdf, 0xfb, 0x0f, 0xc0, 0xdf, 0xfb, 0x0f, 0x81, 0xdf, 0xfb,
   0xcf, 0x83, 0x3f, 0xfc, 0xef, 0x07, 0xff, 0xff, 0xff, 0x07, 0xff, 0xff,
   0xff, 0x0f, 0x3e, 0xfc, 0xff, 0x0f, 0xde, 0xfb, 0xff, 0x1f, 0xdc, 0xfb,
   0xff, 0x1f, 0xdc, 0xfb, 0xff, 0x3f, 0xd8, 0xfb, 0xff, 0x3f, 0x3c, 0xfc,
   0xff, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

#define onitem32_m_width 32
#define onitem32_m_height 32
#define onitem32_m_x_hot 0
#define onitem32_m_y_hot 0
static unsigned char onitem32_m_bits[] = {
   0x03, 0x00, 0x00, 0x00, 0x07, 0x00, 0xe0, 0x03, 0x0f, 0x00, 0xf8, 0x0f,
   0x1f, 0x00, 0xfc, 0x1f, 0x3f, 0x00, 0xfe, 0x3f, 0x7f, 0x00, 0xfe, 0x3f,
   0xfe, 0x00, 0xff, 0x7f, 0xfe, 0x01, 0xff, 0xff, 0xfe, 0x03, 0x7f, 0xfe,
   0xfe, 0x0f, 0x7f, 0xfe, 0xfe, 0x1f, 0x3e, 0xfe, 0xfe, 0x3f, 0x00, 0xff,
   0xfc, 0x7f, 0x80, 0x7f, 0xfc, 0xff, 0xc1, 0x7f, 0xfc, 0xff, 0xe3, 0x3f,
   0xfc, 0xff, 0xe7, 0x1f, 0xfc, 0xff, 0xe3, 0x0f, 0xfc, 0xff, 0xe0, 0x07,
   0xf8, 0x7f, 0xe0, 0x07, 0xf8, 0x7f, 0xe0, 0x07, 0xf8, 0xff, 0xe0, 0x07,
   0xf8, 0xfe, 0xc0, 0x03, 0x38, 0xfc, 0x01, 0x00, 0x18, 0xfc, 0x01, 0x00,
   0x00, 0xf8, 0xc3, 0x03, 0x00, 0xf8, 0xe3, 0x07, 0x00, 0xf0, 0xe7, 0x07,
   0x00, 0xf0, 0xe7, 0x07, 0x00, 0xe0, 0xef, 0x07, 0x00, 0xe0, 0xc7, 0x03,
   0x00, 0xc0, 0x03, 0x00, 0x00, 0x80, 0x00, 0x00};

#if 0 /* XPM format */
static char * onitem32_xpm[] = {
/* width height ncolors cpp [x_hot y_hot] */
"32 32 3 1 0 0",
/* colors */
" 	s iconColor1	m black	c black",
".	s background	m black	c #969696969696",
"X	s iconColor2	m white	c white",
/* pixels */
"  ..............................",
" X ..................XXXXX......",
" XX ...............XX     XX....",
" XXX .............X         X...",
" XXXX ...........X           X..",
" XXXXX ..........X           X..",
". XXXXX ........X             X.",
". XXXXXX .......X      XX      X",
". XXXXXXX ......X     X..X     X",
". XXXXXXXX  ....X     X..X     X",
". XXXXXXXXXX ....XXXXX...X     X",
". XXXXXXXXXXX ..........X      X",
".. XXXXXXXXXXX ........X      X.",
".. XXXXXXXXXXXX  .....X       X.",
".. XXXXXXXXXXXXXX ...X       X..",
".. XXXXXXXXXXXXXXX ..X      X...",
".. XXXXXXXXXXXXX  ...X     X....",
".. XXXXXXXXXXXX .....X    X.....",
"... XXXXXXXXXX ......X    X.....",
"... XXXXXXXXXX ......X    X.....",
"... XXXX XXXXXX .....X    X.....",
"... XX  . XXXXX ......XXXX......",
"... X .... XXXXX ...............",
"...  ..... XXXXX ...............",
"........... XXXXX ....XXXX......",
"........... XXXXX ...X    X.....",
"............ XXXXX ..X    X.....",
"............ XXXXX ..X    X.....",
"............. XXXXX .X    X.....",
"............. XXXX ...XXXX......",
".............. X  ..............",
"............... ................"};
#endif

#define ROOT_USER 0
#define BIN_USER  2
#define SYS_USER  3

#define NO_CONDITION           0
#define MISMATCHING_HOME_DIRS  1

/* ------------------------------------------------------------ *
**
**  Function     trusted
**
**  Purpose      Determines if the passed help volume is a 
**               "trusted"  help volume or not.  We call it 
**               trusted if it meets the following conditions:
**                  1. File Owner is root, bin, or system. 
**                  2. File is NOT writable by group or others.
**                     
**  Returns
**               True   -  if the help volume IS Trusted
**              False   -  if the help volume is NOT Trusted
**
** ------------------------------------------------------------ */
static Boolean trusted (char *hv_path)  /* Full path to the help volume */
{
   struct stat buf;
   Boolean     writable;

   if ( (stat (hv_path, &buf)) == -1)
      return False;


/* S_IWGRP */   /* write group */
/* S_IWOTH */   /* write other */

  /** ---------------------------------------------------------------------- *
   **  The help volume MUST be owned by root, bin, or sys to be trusted.
   ** ---------------------------------------------------------------------- */

   if ( buf.st_uid != ROOT_USER &&
        buf.st_uid != BIN_USER  &&
        buf.st_uid != SYS_USER)
   {
      return False;
   }

  /** ----------------------------------------------------------------------- *
   **  The help volume MUST not be writable by group or others to be trusted.
   ** ----------------------------------------------------------------------- */

   writable = (((buf.st_mode & S_IWGRP) == 0) && (buf.st_mode & S_IWOTH) == 0) ?  
              True : False;

   return writable;

}


/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/****       API Error Dialog Support Functions       *****/
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/


/************************************************************************
 * Function: _DtMessageClose
 *
 *	Close the error/info message box.
 *
 ************************************************************************/ 
static void _DtMessageClose(
    Widget w,
    XtPointer client_data,
    XEvent *event )
{                        
   /* NOTE: ExecuteContextCB() is dependent on this code */
   if (event->type == UnmapNotify)
   {
      XtRemoveEventHandler (XtParent (client_data), StructureNotifyMask, 
                            True, (XtEventHandler) _DtMessageClose, client_data);

      XtUnmanageChild (client_data);
      XtDestroyWidget (client_data);
   }
}

/************************************************************************
 * Function: ExecuteContextCB
 *
 *	Execute an execution context
 *
 ************************************************************************/ 
static void ExecuteContextCB(
    Widget w,
    XtPointer client_data,
    XtPointer callData )
{
    ExecContext * ec = (ExecContext *) client_data;

    if (ec && ec->command && ec->pDisplayArea)
    {
        _DtHelpExecProcedure(ec->pDisplayArea,ec->command);
        free(ec->command);
    }
    XtFree((char *) ec);

    /* unmap, rather than unmanage and destroy, because of the code
       in _DtMessageClose().  _DtMessageClose() is notified when 
       the widget unmaps and it destroys the widget. */
    XtUnmapWidget(w);       /* w is the message dialog */
}

/*****************************************************************************
 * Function: CreateErrorDialog
 *
 *  Creates an XmMessageDialog with the message and all buttons
 *  except the 'Close' (OK) button unmanaged.
 *  Also adds a callback that destroys the widget when the dialog is closed.
 * 
 *****************************************************************************/
static Widget 
CreateErrorDialog(
    Widget                   parent,
    char *                   message)
{     
   Widget	button;
   Widget	messageDialog;
   Arg		args[10];
   int	        n;
 
   XmString	label_string;
   XmString      ok_string;
   char          *title_string; 
 
   /* Setup the message string and dialog title */
 
   ok_string = XmStringCreateLocalized(((char *)_DTGETMESSAGE
                               (HUSET, 2,"Close")));
   label_string = XmStringCreateLocalized(message);
   title_string = XtNewString((char *)_DTGETMESSAGE
                              (HUSET, 5,"Help Error"));
 
   n = 0;
   XtSetArg (args[n], XmNmessageString, label_string);  n++;
   XtSetArg (args[n], XmNtitle,title_string); n++;
   XtSetArg (args[n], XmNcancelLabelString, ok_string); n++;
   XtSetArg (args[n], XmNdefaultButtonType, XmDIALOG_CANCEL_BUTTON); n++;
   messageDialog = XmCreateErrorDialog (parent, "errorDialog",
 					       args, n);
   XtSetArg(args[0], XmNmwmDecorations,
 		              MWM_DECOR_BORDER | MWM_DECOR_TITLE);
   XtSetArg(args[1], XmNuseAsyncGeometry, True);
   XtSetValues(XtParent(messageDialog), args, 2);
 
   XmStringFree (label_string);
   XmStringFree (ok_string);
   XtFree(title_string);
 
   /* unmanage or define the other buttons */
   button = XmMessageBoxGetChild (messageDialog, XmDIALOG_OK_BUTTON);
   XtUnmanageChild (button);
   button = XmMessageBoxGetChild (messageDialog, XmDIALOG_HELP_BUTTON);
   XtUnmanageChild (button);
 
   /* StructureNotifyMask gets Circulate, Configure, Destroy, 
      Gravity, Map, Reparent, & Unmap events */
   XtAddEventHandler(XtParent(messageDialog),
 	             StructureNotifyMask,True,
                     (XtEventHandler) _DtMessageClose, (XtPointer) messageDialog);
 
   return messageDialog;           /* RETURN */

}

/*****************************************************************************
 * Function: CreateExecErrorDlg
 *
 * 
 * 
 * Called by:
 *****************************************************************************/
static Widget
CreateExecErrorDlg(
      Widget   helpWidget,
      const char *  cmdStr,
      Boolean  invalidAlias,
      _DtHelpCommonHelpStuff * pHelpStuff,
      int        condition,
      char     * current_hd)
{
   DtHelpListStruct *pHelpInfo;
   Widget msgDlg;
   Widget btn;
   char *  msg;
   char *  fullmsg;

   /* handle the error case */
   if (invalidAlias)
   {
      msg = (char *)_DTGETMESSAGE(HUSET, 12,
               "The help volume wanted to execute a command alias.\n"
               "The alias '%s' is not defined.");
   }

   else if (condition == MISMATCHING_HOME_DIRS)
   {
      msg = (char *)_DTGETMESSAGE(HUSET, 14,
"The help volume wanted to execute a command as the root user, but the\n"
"home directory of  \"%s\"  ($HOME) does not match the root\n"
"user's home directory.  This could result in executing unexpected\n"
"commands.\n\n"

"The command is:  \"%s\"\n\n"

"Note:  to avoid this in the future:\n"
"  execute \"su - root\"  rather than \"su root\".\n");
   }

   else
   {
      msg = (char *)_DTGETMESSAGE(HUSET, 13,
             "The help volume wanted to execute a command.\n"
             "For security reasons, automatic command execution is turned off.\n"
             "The command is:  %s");
   }
   fullmsg = (char *) malloc(strlen(msg)+strlen(cmdStr)+30);
   if (fullmsg) 
   {
      if (condition == MISMATCHING_HOME_DIRS)
         sprintf(fullmsg, msg, current_hd, cmdStr);
      else
         sprintf(fullmsg,msg,cmdStr);
   }
   else 
      fullmsg = msg;

   /* create an error dialog, but don't manage it yet */
   msgDlg = CreateErrorDialog(XtParent(helpWidget),fullmsg);
   
   if (msg != fullmsg) free(fullmsg);

   btn = XmMessageBoxGetChild (msgDlg, XmDIALOG_HELP_BUTTON);
   XtManageChild (btn);           /* re-manage the button */

   /* add the HelpOnHelp callback */
   pHelpInfo = _DtHelpListAdd(DtHELP_ExecutionPolicy_STR,
                        helpWidget, pHelpStuff, &pHelpStuff->pHelpListHead);
   XtAddCallback(btn, XmNactivateCallback, _DtHelpCB, (XtPointer) pHelpInfo);

   return msgDlg;
}
 
/*****************************************************************************
 * Function: _DtHelpErrorDialog
 *
 * 
 * 
 * Called by:
 *****************************************************************************/
void _DtHelpErrorDialog(
    Widget                   parent,
    char *                   message)
{     
  Widget messageDialog;

  messageDialog = CreateErrorDialog(parent,message);

  /* Display help window. This used to be before the call 
     to add a StructureNotify event handler */
  XtManageChild (messageDialog);
}


/*****************************************************************************
 * Function: _DtHelpFilterExecCmdStr
 *
 *   Args:
 *    helpWidget:	help widget requesting to exec the command
 *    pDisplayStuff:    ptr to the DisplayWidget stuff of the help widget
 *    commandStr:	command string to execute
 *    ret_cmdStr:       the screened & possibly rewritten command is put here
 *    ret_invalidAlias:  was the command an invalid alias?
 *    ret_execPermitted: if executionPolicy permit exec & ret_cmdStr is valid
 *    ret_queryNeeded:  if executionPolicy requires a query before exec
 *
 * Description:
 *    ret_cmdStr gets memory owned by the calling function; it should be
 *    freed when no longer needed.  The string will be the same as the
 *    commandStr if commandStr was not an alias.  If the commandStr
 *    is an alias and if the alias is defined, the ret_cmdStr will be the
 *    value of the alias.  If the alias isn't defined, the ret_cmdStr will
 *    be the default command if available, or the alias name otherwise.
 * 
 *    ret_invalidAlias will be True if the alias was undefined and
 *    no default command was given.
 *
 *    ret_execPermitted will be True if executionPolicy is DtHELP_EXECUTE_ALL
 *    or DtHELP_EXECUTE_QUERY_xxx and ret_cmdStr is valid
 *
 *    ret_queryNeeded will be True if executionPoilcy is 
 *    DtHELP_EXECUTE_QUERY_ALL or if it is DtHELP_EXECUTE_QUERY_UNALIASED 
 *    and ret_cmdStr did not derive from an alias (i.e. was hardcoded
 *    in the help volume, not retrieved from a resource).
 *
 * Returns:
 *    True:  if execPermitted and a valid command string
 *    False: if if execPermitted is False or invalid command string
 *
 * Comments:
 *    This code is written such that we don't need nor want to know
 *    whether it is a general or quick help widget.
 * 
 * Warning:
 *    command string must be writable; it is written, but left
 *    unchanged when the function exits.
 *
 *****************************************************************************/
Boolean _DtHelpFilterExecCmdStr(
    Widget                   helpWidget,
    unsigned char            executionPolicy,
    const char *             commandStr,
    char * *                 ret_cmdStr,
    Boolean *                ret_invalidAlias,
    Boolean *                ret_execPermitted,
    Boolean *                ret_queryNeeded,
    char *		     hv_path)     /* Path to the Help Volume */
{     
   char * token;
   char * tokenEnd;
   char   tokenEndChar;
   char * aliasCommand = NULL;
   Boolean ret;

#define RN_execAlias "executionAlias"
#define RC_execAlias "ExecutionAlias"
#define ExecAliasCmd "DtHelpExecAlias"

   /* default values */
   *ret_cmdStr = NULL;
   *ret_invalidAlias = False;
   *ret_execPermitted = False;
   *ret_queryNeeded = False;

   if (NULL == commandStr) 
      return False;

  /** ------------------------------------------------------------- *
   **  If the executionPolicy is query all unaliased (query for all
   **  execution links that have no execution alias defined), we 
   **  make an exception:  only query the user for help volumes 
   **  deemed NOT "trusted".
   ** ------------------------------------------------------------- */

   if (DtHELP_EXECUTE_QUERY_UNALIASED == executionPolicy)
   {
      if ( ! (trusted (hv_path)))
         *ret_queryNeeded = True;   /* Query ALL non-trusted help volumes */
   }
   else
      *ret_queryNeeded = (DtHELP_EXECUTE_QUERY_ALL == executionPolicy);

   /* get whether exec permitted */
   if (   DtHELP_EXECUTE_ALL == executionPolicy 
       || DtHELP_EXECUTE_QUERY_UNALIASED == executionPolicy
       || DtHELP_EXECUTE_QUERY_ALL == executionPolicy)
      *ret_execPermitted = True;
   else
      *ret_execPermitted = False;

   /* parse apart the command string, looking for DtHelpExecAlias */
   /* The first call will return true, with the first string */
   token = (char *) commandStr + DtStrspn((char *)commandStr, " \t");
   tokenEnd = token + DtStrcspn(token, " \t");
   tokenEndChar = *tokenEnd;
   if (tokenEnd) *tokenEnd = EOS;

   if ( NULL == token || _DtHelpCeStrCaseCmpLatin1(token, ExecAliasCmd) != 0 )
   {
      /*** the command is not an alias; proceed using execution Policy ***/
   
      *tokenEnd = tokenEndChar;       /* restore the string */
   
      *ret_cmdStr = strdup(commandStr);
      ret = *ret_execPermitted;

      return ret;                             /* RETURN ret */
   }


   /**** It's an alias; get it , look it up, and return it ****/

   *tokenEnd = tokenEndChar;       /* restore the string */

   /* get the next token */
   token = tokenEnd + DtStrspn(tokenEnd, " \t");
   tokenEnd = token + DtStrcspn(token, " \t");
   tokenEndChar = *tokenEnd;
   if (tokenEnd) *tokenEnd = EOS;

   if ( token )
   {
      Display * dpy = XtDisplay(helpWidget);
      XrmDatabase appDb = XrmGetDatabase(dpy);
      XrmValue value;
      String appname, appclass;
      char * reptype;
      char *rsrc_name, *rsrc_class;

      rsrc_name = XtMalloc(200);
      rsrc_class = XtMalloc(200);

      XtGetApplicationNameAndClass(dpy,&appname,&appclass);

      /* query the application's database for the alias command */
      /* build alias resource class and resource */
      /* e.g. App.executionAlias.<alias> */
      sprintf(rsrc_name,"%s.%s.%s",appclass, RN_execAlias,token);
      /* e.g. App.ExecutionAlias.<alias> */
      sprintf(rsrc_class,"%s.%s.%s",appclass, RC_execAlias,token);

      /* Get alias command */
      if (XrmGetResource(appDb,rsrc_name,rsrc_class,&reptype,&value) == True)
         aliasCommand = value.addr;

      /* build alias resource name and resource */
      /* e.g. app.executionAlias.<alias> */
      sprintf(rsrc_name,"%s.%s.%s",appname, RN_execAlias,token);
      /* e.g. app.ExecutionAlias.<alias> */
      sprintf(rsrc_class,"%s.%s.%s",appname, RC_execAlias,token);

      /* Get alias command and override class with instance, if defined */
      if (XrmGetResource(appDb,rsrc_name,rsrc_class,&reptype,&value) == True)
         aliasCommand = value.addr;

      if (rsrc_name) XtFree(rsrc_name);
      if (rsrc_class) XtFree(rsrc_class);
   }  /* if alias token */
   else 
   {
      token = "";
   }

   if (tokenEnd) *tokenEnd = tokenEndChar; /* restore the string */

   /* alias was defined */
   if (aliasCommand) 
   {
      *ret_cmdStr = strdup(aliasCommand);
      /* see if query needed; is not if policy is query_unaliased or all */
      *ret_queryNeeded = !(   DtHELP_EXECUTE_QUERY_UNALIASED == executionPolicy
                           || DtHELP_EXECUTE_ALL == executionPolicy);
      ret = *ret_execPermitted;
   }
   else /* the alias wasn't defined */
   {
      char * aliasToken = token;   /* token currently pts to alias */

      /* look for a default command */
      /* get the next token */
      token = tokenEnd + DtStrspn(tokenEnd, " \t");
      tokenEnd = token + DtStrcspn(token, " \t");

      if (token == tokenEnd)
      {  /* alias wasn't defined and no default command */
         *ret_cmdStr = strdup(aliasToken);
         *ret_invalidAlias = True;
         *ret_queryNeeded = False;  /* no query needed on invalid alias, ever */
         *ret_execPermitted = False; /* can't exec an invalid alias */
         ret = False;
      }
      else 
      {  /* alias wasn't defined but a default command */
         /* query is whatever was determined earlier */
         *ret_cmdStr = strdup(token);
         ret = *ret_execPermitted;
      }
   }

   return ret;                          /* RETURN ret */
}

/*********************************************************************
 * _DtHelpCeWaitAndProcessEvents
 *
 *  Purpose:
 *    _DtHelpCeWaitAndProcessEvents will process events and call
 *    the waitProc until waitProc returns False.   This function
 *    is useful to put up modal dialogs that must be reponded to 
 *    in the midst of executing code that must remain on the call stack.
 *
 *  Warning:
 *    This function should only be used on modal dialogs.
 *
 *********************************************************************/

void
_DtHelpCeWaitAndProcessEvents (
    Widget             w,
    _DtHelpCeWaitProc  waitProc,
    void *             clientData)
{
    Boolean waitFlag;
    XEvent   event;
    XtAppContext app;

    app = XtWidgetToApplicationContext(w);
    do 
    {
#ifndef XTHREADS
        XtAppNextEvent(app,&event);
        XtDispatchEvent(&event);
#else
	XtInputMask mask;

	while (!(mask = XtAppPending(app)))
	  ;   /* Busy waiting - so we don't lose our Lock! */
	
	if (mask & XtIMXEvent)         /* We have an XEvent */
	  {
	    /* Get the XEvent - we know it's there! Note that XtAppNextEvent
	       would also process timers/alternate inputs.
	     */
	    XtAppNextEvent(app, &event);  /* No blocking, since an event is ready */
	    XtDispatchEvent(&event);
	  }
	else   /* Not a XEvent, it's an alternate input/timer event */
	  {
	    XtAppProcessEvent(app, mask); /* No blocking, since an event is ready */
	  }
#endif /* XTHREADS */
        /* check to see if we're done waiting */
        waitFlag = (*waitProc)(w, clientData); 
    } while (waitFlag);
}

/*****************************************************************************
* Function: WaitForBtnActivatedCB
*
* Purpose:
*    Treats the 'clientData' as a pointer to an integer
*    and turns its value into a Boolean
*
* Returns:  *(int *)clientData < 0
*****************************************************************************/
static Boolean 
WaitForBtnActivatedCB(
    Widget w,
    void * clientData)
{
    return (*(int *)clientData < 0);
   /* True=keep waiting; False= wait no longer */
}


typedef struct ModalMsgDlgCBStruct
{
  Widget msgDlg;
  Widget okBtn;
  Widget cancelBtn;
  Widget helpBtn;
  int    activatedBtnId;
} ModalMsgDlgCBStruct;

/*****************************************************************************
* Function: IdentifyActivatedBtnCB
*
* Purpose:
*    Treats the 'clientData' as a pointer to an integer.
*    Waits for the value pointed to by clientData to be >= 0.
*
*****************************************************************************/
static void 
IdentifyActivatedBtnCB(
    Widget w,
    XtPointer clientData,
    XtPointer callData)
{
   ModalMsgDlgCBStruct * pMd = (ModalMsgDlgCBStruct *) clientData;
    
   /* w must be a XmMessageDialog widget */
   if (pMd->okBtn == w) 
      { pMd->activatedBtnId = XmDIALOG_OK_BUTTON; return; /* RETURN */ }
   if (pMd->cancelBtn == w) 
      { pMd->activatedBtnId = XmDIALOG_CANCEL_BUTTON; return; /* RETURN */ }
   if (pMd->helpBtn == w) 
      { pMd->activatedBtnId = XmDIALOG_HELP_BUTTON; return; /* RETURN */ }
   pMd->activatedBtnId = -1;      /* unknown button */
}
    

/*****************************************************************************
 * Function: _DtHelpFilterExecCmd
 *
 *   Args:
 *    helpWidget:	help widget requesting to exec the command
 *    command:		command string to execute
 *    execPolicy:       current policy setting
 *    useQueryDialog:   use a dialog to query user whether to exec, if not allowed
 *    pHelpStuff:       ptr to the HelpStuff structure of the help widget
 *    ret_filteredCmdStr: filtered command string
 * 
 * Returns:
 *    0: no error; filteredCmdStr can be exec'd
 *   -1: error: either internal or executionPolicy denies exec; 
 *       filteredCmdStr is NULL
 *
 * Purpose:
 *    This function filters an execution command string.  This can
 *    occur in several ways.  In all cases, the command string is
 *    supports command alias replacement.  If the final outcome
 *    is that execution is permitted, the returned string is
 *    is the command string to execute.  If execution is not
 *    permitted, the return string is a NULL pointer. 
 *
 *    Filtering of the command occurs as follows.
 *    If executionPolicy permits execution, only alias replacement occurs.
 *    If executionPolicy does restrict execution and a
 *    dialog is requested, then a modal dialog is posted and the
 *    user can decide whether to execute or not.
 *    If a dialog is not requested, the return string is NULL.
 *
 * Comments:
 *    This code is written such that we don't need nor want to know
 *    whether it is a general or quick help widget.
 * 
 * Warning:
 *    command string must be writable; it is written, but left
 *    unchanged whent the function exits.
 *
 *    This operation is synchronous, meaning that, if a dialog is
 *    posted, it is a modal dialog and the function won't return 
 *    until the user selects a button.
 *
 * Called by:
 *****************************************************************************/
int _DtHelpFilterExecCmd(
    Widget        helpWidget,
    const char *  commandStr,
    unsigned char executionPolicy,
    Boolean       useQueryDialog,
    _DtHelpCommonHelpStuff * pHelpStuff,
    char * *      ret_filteredCmdStr,
    char *		     hv_path)
{     
   ModalMsgDlgCBStruct msgDlgCBStruct;
   Boolean goodCmd;
   Boolean invalidAlias;
   Boolean execPermitted;
   Boolean queryNeeded;
   Widget  msgDlg;
   char *  filteredCmdStr = NULL;
   Arg	   args[5];
   int     n;
   XmString labelString;
   XmString labelString2;
   Widget  noexecBtn;
   Widget  execBtn;

   goodCmd = _DtHelpFilterExecCmdStr(helpWidget, executionPolicy, 
                 commandStr, &filteredCmdStr, &invalidAlias, 
                 &execPermitted, &queryNeeded, hv_path);

   /* if permissions allow immediate execution, do so */
   if (execPermitted && False == queryNeeded)
   {
      *ret_filteredCmdStr = filteredCmdStr;
      return 0;                           /* RETURN ok */
   }

   if (False == useQueryDialog)
   {
      *ret_filteredCmdStr = NULL;
      return -1;                          /* RETURN error */
   }

   /* create the dialog, but don't yet manage it */
   msgDlg =  CreateExecErrorDlg(helpWidget,filteredCmdStr,
                        invalidAlias,pHelpStuff, NO_CONDITION, "");

   /* if a bad alias or no exec permitted, 
      don't need to wait for a response; dlg has close & Help */
   if (False == execPermitted || False == queryNeeded)  
   {
      XtManageChild(msgDlg);                 /* manage modeless dialog */
      *ret_filteredCmdStr = NULL;            /* don't execute */
      XtFree(filteredCmdStr);
      return -1;                            /* RETURN error */
   }

   /* if got this far, query is needed;make the dialog include 
      Execute Anyway and Don't Execute buttons */

   /* give the right title to the buttons */
   labelString = XmStringCreateLocalized(((char *)_DTGETMESSAGE
                   (HUSET, 10,"Execute Anyway")));
   /* give the right title to the Cancel button */
   labelString2 = XmStringCreateLocalized(((char *)_DTGETMESSAGE
                   (HUSET, 11,"Don't Execute")));
   n = 0;
   XtSetArg (args[n], XmNokLabelString, labelString);	n++;
   XtSetArg (args[n], XmNcancelLabelString, labelString2);		n++;
   XtSetArg (args[n], XmNdialogStyle, XmDIALOG_PRIMARY_APPLICATION_MODAL); n++;
   XtSetValues(msgDlg,args,n);
   XmStringFree(labelString);
   XmStringFree(labelString2);

   /* We put an activate callback on the DontExecute and ExecuteAnyway buttons 
      and wait until a button is pressed.  */
   noexecBtn = XmMessageBoxGetChild (msgDlg, XmDIALOG_CANCEL_BUTTON);
   XtAddCallback(noexecBtn, XmNactivateCallback, 
                             IdentifyActivatedBtnCB, (XtPointer) &msgDlgCBStruct);

   execBtn = XmMessageBoxGetChild (msgDlg, XmDIALOG_OK_BUTTON);
   XtManageChild (execBtn);           /* re-manage the button */
   XtAddCallback(execBtn, XmNactivateCallback, 
                             IdentifyActivatedBtnCB, (XtPointer) &msgDlgCBStruct);
   
   /* fill out the CB information structure used by IdentifyActivatedBtnCB */
   msgDlgCBStruct.msgDlg = msgDlg;
   msgDlgCBStruct.okBtn = execBtn;
   msgDlgCBStruct.cancelBtn = noexecBtn;
   msgDlgCBStruct.activatedBtnId = -1;

   /* Display message dialog */
   XtManageChild (msgDlg);

   /*
    * turn on the modal dialog indicator
    */
   _DtHelpTurnOnNoEnter(helpWidget);

   /* wait until 'msgDlgCBStruct.activatedBtnId' has a value >= 0 */
   /* this occurs when the user responds to the msg dialog */
   _DtHelpCeWaitAndProcessEvents(msgDlg, 
              WaitForBtnActivatedCB, &msgDlgCBStruct.activatedBtnId);

   /*
    * turn off the modal dialog indicator
    */
   _DtHelpTurnOffNoEnter(helpWidget);

   /* no need to destroy msgDlg; it has a closeCallback to do that */

   /* act based on which button was activated */
   if (msgDlgCBStruct.activatedBtnId == XmDIALOG_OK_BUTTON)
   {
      *ret_filteredCmdStr = filteredCmdStr;  /* do execute command */
      return 0;                             /* RETURN ok */
   }
   else
   {
      *ret_filteredCmdStr = NULL;            /* don't execute */
      XtFree(filteredCmdStr);
      return -1;                            /* RETURN error */
   }
}

/*****************************************************************************
 * Function: _DtHelpExecFilteredCmd
 *
 *   Args:
 *    helpWidget:	help widget requesting to exec the command
 *    command:		command string to execute
 *    modal:            is the execution modal (sync) or modeless (async)
 *    helpLocationId:	helpOnHelp file location for Help btn in error dialog
 *    pDisplayStuff:    ptr to the DisplayWidget stuff of the help widget
 *    pHelpStuff:	ptr to the CommonHelp stuff of the help widget
 * 
 * Comments:
 *    This code is written such that we don't need nor want to know
 *    whether it is a general or quick help widget.
 * 
 * Warning:
 *    command string must be writable; it is written, but left
 *    unchanged whent the function exits.
 *
 *    At the moment, the helpLocationId is ignored, and the
 *    help location is hardwired to DtHELP_ExecutionPolicy_STR
 *    in CreateExecErrorDialog().
 * Called by:
 *****************************************************************************/
void _DtHelpExecFilteredCmd(
    Widget                   helpWidget,
    char *                   commandStr,
    char *                   helpLocationId,
    _DtHelpDisplayWidgetStuff * pDisplayStuff,
    _DtHelpCommonHelpStuff * pHelpStuff)
{     
   Boolean goodCmd;
   Boolean invalidAlias;
   Boolean execPermitted;
   Boolean queryNeeded;
   char *  filteredCmdStr = NULL;
   ExecContext * execContext;
   XmString labelString;
   XmString labelString2;
   Widget   msgDlg;
   Widget   btn;
   int     n;	
   Arg	   args[5];
   char    *hv_path=NULL;

   uid_t         user;
   char          *home_dir;
   Boolean        diff_home_dirs=False; /* True ==> $HOME is different from */
                                        /* root user's $HOME directory      */
/*
   getpw{uid,nam}_r routines fail on IBM platform when search password info
   via NIS (yellow pages). However, in the case of root, we'll assume that
   the password info is in /etc/passwd. If this is not the case, the
   following code can fail on IBM platform when XTHREADS and XUSE_MTSAFE_API
   are defined.
*/
/*
   _Xgetpwparams pwd_buf;
*/
   struct passwd * pwd_ret;

  /** -------------------------------------------------------------- *
   **  If we're running as the root user
   **   o check if the value of the HOME env var matches 
   **     root's home directory (defined by /etc/passwd).
   **   o If they do not match, then present a dialog 
   **     alerting the user of this, along with the command to 
   **     invoke.
   ** -------------------------------------------------------------- */

   if ( (user=getuid()) == ROOT_USER)
   {
      home_dir = getenv ("HOME");

      if (home_dir != NULL && strlen(home_dir) >= (size_t) 1)
      {
         if (((pwd_ret = _XGetpwuid(user, pwd_buf)) != NULL)
                         && (strcmp(home_dir, pwd_ret->pw_dir)))
         {
            diff_home_dirs = True;
         }
      }
   }

   hv_path = _DtHelpFileLocate(DtHelpVOLUME_TYPE, pDisplayStuff->helpVolume,
                               _DtHelpFileSuffixList, False, R_OK);

   /* The desired and intended behaviour is to use _DtHelpFilterExecCmdStr(), but
      the other code is left here, should a change be wished. */
#if 1
   /* This function runs a filter for policy and alias but posts no dialog  */
   goodCmd=_DtHelpFilterExecCmdStr(helpWidget, 
              pDisplayStuff->executionPolicy, commandStr, 
               &filteredCmdStr, &invalidAlias, &execPermitted, &queryNeeded, hv_path);
#else
    /* This function does an synchronous filter; i.e. the code runs a filter
       for policy and alias, and if policy denies exec and the command is
       valid, then posts a modal dialog and waits for the user to decide
       what to do before returning. */
   goodCmd = _DtHelpFilterExecCmd(helpWidget, commandStr, 
                                  pDisplayStuff->executionPolicy, True, 
                                  pHelpStuff, &filteredCmdStr, hv_path);
   execPermitted = (goodCmd == 0);  /* convert an error int into a Boolean */
   queryNeeded =    
          ((   pDisplayStuff->executionPolicy==DtHELP_EXECUTE_QUERY_ALL)
            || (pDisplayStuff->executionPolicy==DtHELP_EXECUTE_QUERY_UNALIASED))
       && goodCmd;
#endif

   /* if permissions allow immediate execution, do so */
   if (execPermitted && False == queryNeeded  && diff_home_dirs == False)
   {
      (void) _DtHelpExecProcedure (pHelpStuff->pDisplayArea, filteredCmdStr);
      free(filteredCmdStr);
      return;                           /* RETURN */
   }

   /* this traps bad cmds and also use of the synchronous filter call */
   if (NULL == filteredCmdStr) return;       /* RETURN */

   /*** Create a modeless dialog to inform the user of the problem
        and possibly allow them to execute the command anyway. ***/

   /* create the dialog, but don't yet manage it */
   if ( diff_home_dirs == True)
     msgDlg =  CreateExecErrorDlg(helpWidget,filteredCmdStr, invalidAlias,pHelpStuff, 
                                  MISMATCHING_HOME_DIRS, home_dir );
   else
     msgDlg =  CreateExecErrorDlg(helpWidget,filteredCmdStr, invalidAlias,pHelpStuff, 
                                  NO_CONDITION, "");


   /*** setup ExecuteAnyway and Help buttons ***/

   if ( (diff_home_dirs == True)  
                     ||
        (queryNeeded && execPermitted) )
   {
      /* give the right title to the buttons */
      labelString = XmStringCreateLocalized(((char *)_DTGETMESSAGE
                         (HUSET, 10,"Execute Anyway")));
      /* give the right title to the Cancel button */
      labelString2 = XmStringCreateLocalized(((char *)_DTGETMESSAGE
                      (HUSET, 11,"Don't Execute")));
      n = 0;
      XtSetArg (args[n], XmNokLabelString, labelString);	n++;
      XtSetArg (args[n], XmNcancelLabelString, labelString2);	n++;
      XtSetValues(msgDlg,args,n);
      XmStringFree(labelString);
      XmStringFree(labelString2);
   
      btn = XmMessageBoxGetChild (msgDlg, XmDIALOG_OK_BUTTON);
      XtManageChild (btn);           /* re-manage the button */
   
      /* add the ExecuteContextCB() client-data and callback */
      execContext = malloc(sizeof(ExecContext));
      if (execContext) 
      { 
         execContext->command = filteredCmdStr;
         execContext->pDisplayArea = pHelpStuff->pDisplayArea;
         XtAddCallback(btn, XmNactivateCallback, 
                             ExecuteContextCB, (XtPointer) execContext);
      }
      else
      {
         free(filteredCmdStr);
      }
   }  /* cmd wasn't an alias */
   else
   {
      free(filteredCmdStr);
   }

   /* Display message dialog */
   XtManageChild (msgDlg);

   /* RETURN */
}



/*****************************************************************************
 * Function: LocateWidgetId()
 *
 *   
 *
 * Called by: 
 *****************************************************************************/
static Widget  LocateWidgetId(
    Display          *dpy,
    int              screen,
    int             *statusRet,
    Widget           shellWidget,
    Cursor           cursorIn)
{
    static Cursor    DfltOnItemCursor = 0;
    Widget           widget;
    Widget           child;
    CompositeWidget  comp_widget;
    int              status;
    Cursor           cursor;
    XEvent           event;
    int              x,y;
    int              i;
    Window           parent;
    Window           sub;
    Window           target_win;
    int              new_x, new_y;
    int              offset;
    KeySym           keySym;
    Boolean          notDone=TRUE;

 
    /* Make the target cursor */
    if (cursorIn != 0)
        cursor = cursorIn;
    else
#if 0
        cursor = XCreateFontCursor (dpy, XC_question_arrow);
#else
    {
        _DtHelpProcessLock();
        if (0 == DfltOnItemCursor)
        {
            char        *bits;
            char        *maskBits;
            unsigned int width;
            unsigned int height;
            unsigned int xHotspot;
            unsigned int yHotspot;
            Pixmap       pixmap;
            Pixmap       maskPixmap;
            XColor       xcolors[2];
   
            width    = onitem32_width;
            height   = onitem32_height;
            bits     = (char *) onitem32_bits;
            maskBits = (char *) onitem32_m_bits;
            xHotspot = onitem32_x_hot;
            yHotspot = onitem32_y_hot;
    
            pixmap = XCreateBitmapFromData (dpy,
                         RootWindowOfScreen(XtScreen(shellWidget)), bits,
                         width, height);
    
    
            maskPixmap = XCreateBitmapFromData (dpy,
                         RootWindowOfScreen(XtScreen(shellWidget)), maskBits,
                         width, height);
    
            xcolors[0].pixel = BlackPixelOfScreen(ScreenOfDisplay(dpy, screen));
            xcolors[1].pixel = WhitePixelOfScreen(ScreenOfDisplay(dpy, screen));
    
            XQueryColors(dpy,
			 DefaultColormapOfScreen(ScreenOfDisplay(dpy, screen)), 
			 xcolors, 
			 2);
    
            DfltOnItemCursor = XCreatePixmapCursor (dpy, pixmap, maskPixmap,
                                          &(xcolors[0]), &(xcolors[1]),
                                          xHotspot, yHotspot);
            XFreePixmap (dpy, pixmap);
            XFreePixmap (dpy, maskPixmap);
        }  /* if dflt cursor not yet created */
        cursor = DfltOnItemCursor;
        _DtHelpProcessUnlock();
    }  /* if to use the standard cursor */
#endif


    /* Grab the pointer using target cursor, letting it roam all over */
    status = XtGrabPointer (shellWidget, TRUE,
                           ButtonPressMask|ButtonReleaseMask, GrabModeAsync,
                           GrabModeAsync, None, cursor, CurrentTime);
    if (status != GrabSuccess)
      {
        XmeWarning(shellWidget,(char *)_DTGETMESSAGE(HUSET, 3,
              "Internal Error: Could not grab the mouse\nDtHelpReturnSelectedWidget aborted.\n"));
        *statusRet = DtHELP_SELECT_ERROR;
        return(NULL);
      }



    /* Grab the Keyboard so we can catch the ESC button press */
    status = XtGrabKeyboard(shellWidget, False,
                           GrabModeAsync, GrabModeAsync, CurrentTime);
    if (status != GrabSuccess)
      {
        
        XtUngrabPointer (shellWidget, CurrentTime);
        XmeWarning(shellWidget,(char *)_DTGETMESSAGE(HUSET, 4,
        "Internal Error: Could not grab the keyboard\nDtHelpReturnSelectedWidget() aborted.\n"));
        *statusRet = DtHELP_SELECT_ERROR;
        return(NULL);
      }

    /* We are ok so let the user select a window... */
    while (notDone)
    {
      XtAppContext app = XtWidgetToApplicationContext(shellWidget);
      /* allow one more event */
#ifndef XTHREADS
      XtAppNextEvent(app, &event);
#else
      XtInputMask mask;

      while (!(mask = XtAppPending(app)))
	  ;   /* Busy waiting - so we don't lose our Lock! */
	
      if (!(mask & XtIMXEvent)) /* Not a XEvent, it's an alternate input/timer event */
	    XtAppProcessEvent(app, mask); /* No blocking, since an event is ready */
      else   /* We have an XEvent */
	{
	    /* Get the XEvent - we know it's there! Note that XtAppNextEvent
	       would also process timers/alternate inputs.
	     */
	    XtAppNextEvent(app, &event);
#endif /* XTHREADS */
      widget = XtWindowToWidget(dpy, event.xany.window);
      
      switch (event.type) {
            case ButtonPress:
                break;
            case ButtonRelease:
                notDone = FALSE;
                break;
            case KeyPress:
                /* Look for ESC key press and stop if we get one */
                if (event.xkey.state & ShiftMask)
                  offset = 1;
                else
                  offset = 0;
                
                keySym = XLookupKeysym((XKeyEvent *)&event, offset);
                if (keySym == XK_Escape)
		  {
                    XtUngrabKeyboard (shellWidget, CurrentTime);
                    XtUngrabPointer (shellWidget, CurrentTime);
                    *statusRet = DtHELP_SELECT_ABORT;
                    return(NULL);
		  }
            default:
                  XtDispatchEvent(&event);
      }
#ifdef XTHREADS
	}    /* if */
#endif
    } 

    XtUngrabKeyboard (shellWidget, CurrentTime);      /* Done with keyboard */
    XtUngrabPointer (shellWidget, CurrentTime);      /* Done with pointer */

    /* If its null then the user selected some area outside our window(s) */
    if (widget == shellWidget)
      {
        *statusRet = DtHELP_SELECT_INVALID;
        return (NULL);
      }
    if (!XtIsComposite (widget))
      {
        *statusRet = DtHELP_SELECT_VALID;
        return (widget);
      }
    
    /* Get the x and y and parent relative to the current window */
    parent = RootWindow(dpy, screen);
    target_win = XtWindow(widget);
    x = event.xbutton.x_root;
    y = event.xbutton.y_root;

    XTranslateCoordinates(dpy, parent, target_win, x, y,
                             &new_x, &new_y, &sub);
    x = new_x;
    y = new_y;

    comp_widget = (CompositeWidget)widget;

    /*  look for gadgets at this point  */
    for (i = 0; i < comp_widget->composite.num_children; i++) {
        child = comp_widget->composite.children[i];
        /* put in check for only managed widgets here */
         if(XtIsManaged(child))
           if (PT_IN_CHILD (x, y, child))
              {
                *statusRet = DtHELP_SELECT_VALID;
                return (child);
              }
    }

    *statusRet = DtHELP_SELECT_VALID;
    return (widget);
}




/*****************************************************************************
 * Function:	   Boolean RememberDir(String path)
 *
 * Parameters:		path		Specifies the path to check.
 *
 * Return Value:	Boolean		if the path name is good.
 *
 * Description:	Use the directory caching mechanism to improve performance
 *		by remembering the directories that have already been
 *		stat'ed.
 *
 *****************************************************************************/
static	Boolean
RememberDir(String path)
{
    int		 result = 0;
    char        *ptr;
    struct stat  buf;

    if (path == NULL || *path == '\0')
	return False;

    if (_DtHelpCeStrrchr(path, "/", MB_CUR_MAX, &ptr) == 0 && ptr != path)
      {
	*ptr   = '\0';
	result = _DtHelpCeCheckAndCacheDir(path);
	*ptr   = '/';
      }
    if (result == 0 && access(path, R_OK) == 0 &&
				stat(path, &buf) == 0 && S_ISREG(buf.st_mode))
	return True;

    return False;
}

/*****************************************************************************
 * Function:	   Boolean _DtHelpResolvePathname(
 *
 *
 * Parameters:     
 *
 * Return Value:    Boolean.
 *
 *
 * Description: _DtHelpResolvePathname attempts to validate and expand a path
 *              to a Cache Creek help access file.
 *
 *****************************************************************************/
Boolean _DtHelpResolvePathname(
   Widget       widget,
   char * *	io_fileName,
   _DtHelpVolumeHdl * io_volumeHandle,
   char *	sysVolumeSearchPath,
   char *	userVolumeSearchPath)
{
   String           newPath = NULL;

   /* try to locate file and its entry, if present */
   newPath = _DtHelpFileLocate(DtHelpVOLUME_TYPE, *io_fileName,
                                  _DtHelpFileSuffixList,False,R_OK);

   /* If we found a valid file let's do some set up here */

   if (newPath != NULL)       /* We have a valid volume file so open it */
   {
       /* Close the current one if we have one open */
       if (*io_volumeHandle != NULL)
           _DtHelpCloseVolume(*io_volumeHandle);

       /* Open the help volume file and save the handle id */
       if (_DtHelpOpenVolume(newPath,io_volumeHandle) >= 0)
       {
           /* Copy the expanded file location path */
           XtFree(*io_fileName);
           *io_fileName = newPath;

           return(TRUE);
       }
       else
       {
           /* ERROR; leave io_fileName untouched on error
            * We used to set it to null here now we just return what came in
            */
           /* later NOTE: this seems strange, since we have closed the
              old volume, invalidating io_fileName */
           XtFree(newPath);
           XmeWarning(widget,(char*)UtilMessage2); 
           return (FALSE);
       }      
   }
   else                       /* We have a non-valid path */
   {
       /* ERROR; leave io_fileName untouched on error
        * We used to set it to null here now we just return what came in
        */

       XmeWarning(widget,(char*)UtilMessage0);        
       return (FALSE);
   }

}





/*****************************************************************************
 * Function:	   Boolean _DtHelpExpandHelpVolume(DtHelpDialogWidget nw);
 *
 *
 * Parameters:     nw  Specifies the current help dialog widget.
 *
 * Return Value:    Boolean.
 *

 * Description: _DtHelpExpandHelpVolume looks for a $LANG variable in the 
 *              helpAccesFile string and if found, replaces it with the 
 *              current lang variable.
 *
 *****************************************************************************/
Boolean _DtHelpExpandHelpVolume(
   Widget                      w,
   _DtHelpDisplayWidgetStuff * display,
   _DtHelpCommonHelpStuff *    help,
   _DtHelpPrintStuff *         print)
{
  Boolean validTopic = FALSE;
  Boolean validPath  = FALSE;
  char  *topLevelId;

  /* Free the old, and Copy the new volumeHandle to printVolume varialbe */
  if (print->printVolume != NULL)
     XtFree(print->printVolume);

  print->printVolume = XtNewString(display->helpVolume);     

  validPath = _DtHelpResolvePathname((Widget)w,
				&print->printVolume,
                                &display->volumeHandle,
                                help->sysVolumeSearchPath,
                                help->userVolumeSearchPath);

 
  /* Check to see that we resolved our path correctly */
  if (!validPath)
    return (FALSE);			/* RETURN */
  else
    {

       /* The following routine will malloc memory for the topLevelId
        * variable, so we must free our current version first.
        */
       XtFree(help->topLevelId);

       /* Assign our top level topic for this help access file */
       validTopic = _DtHelpCeGetTopTopicId(display->volumeHandle, &topLevelId);

       if (!validTopic)
         {
           /* Bad top level topic */
           help->topLevelId = NULL;
           return(FALSE);
         }
       else   
         {
            /* recall that the topLevelId/File vars are malloc'd */
            help->topLevelId = topLevelId;
            return(TRUE); 
	 }
    }
}


/*****************************************************************************
 * Function:	   char *_DtHelpParseIdString(char * specification);
 *
 *
 * Parameters:     specification  Specifies an author defined help topic.
 *
 * Return Value:    Void.
 *
 * Description:   This function copies the locationId portion of the 
 *                specification and returns it to the calling routine.
 *
 *****************************************************************************/
char *_DtHelpParseIdString(
   char *specification)
{

  char *pAccessFile = NULL;
  char *tmpSpec=NULL;
  char *returnStr=NULL;
  char *strtok_ptr;

  tmpSpec = XtNewString(specification);

  
  /* First look for a blank in the specification.  This will signify that
   * we have a HelpAccessFile as part of the specification.
   */
  
  /* The first call will return true, with the first string */
  pAccessFile = DtStrtok_r(tmpSpec, " ", &strtok_ptr);
  returnStr = XtNewString(pAccessFile);

  /* The second call will return true only if we have another string */
  pAccessFile = DtStrtok_r(NULL, " ", &strtok_ptr);

  if (pAccessFile != NULL)
    {
      /* We have a helpAccessFile in our specification */
           
      XtFree(returnStr);
   
      returnStr = XtNewString(pAccessFile);
      XtFree(tmpSpec);
      return(returnStr);

    }
  else
    {
      /* We don't have a helpAccessFile as part of the specificaiton
       * so we just return our locationId.
       */
       XtFree(tmpSpec);
       return (returnStr);
     }

}



/*****************************************************************************
 * Function:	   char *_DtHelpParseAccessFile(char * specification);
 *
 *
 * Parameters:     specification  Specifies an author defined help topic.
 *
 * Return Value:    Void.
 *
 * Description:   This function copies the helpAccessFile portion of the 
 *                specification and returns it to the calling routine.
 *
 *****************************************************************************/
char *_DtHelpParseAccessFile(
   char *specification)
{
  char *pAccessFile = NULL;
  char *tmpSpec=NULL;
  char *returnStr=NULL;
  char *strtok_ptr;

  tmpSpec = XtNewString(specification);

  
  /* First look for a blank in the specification.  This will signify that
   * we have a HelpAccessFile as part of the specification.
   */
  
  /* The first call will return true, with the first string */
  pAccessFile = DtStrtok_r(tmpSpec, " ", &strtok_ptr);
  returnStr = XtNewString(pAccessFile);

  /* The second call will return true only if we have another string */
  pAccessFile = DtStrtok_r(NULL, " ", &strtok_ptr);

  if (pAccessFile != NULL)
    {
      /* We have a helpAccessFile in our specification */
           

      /* If we have an accessFile, but it's not a full path, then we
       * must get the full path from the reg file.
       */
      XtFree(tmpSpec);
      return(returnStr);

    }
  else
    {
      /* We don't have a helpAccessFile as part of the specificaiton
       * so return NULL.
       */
       XtFree(returnStr);
       XtFree(tmpSpec);
       return (NULL);
     }

}








/*****************************************************************************
 * Function:	 DtHelpReturnSelectedWidgetId 
 *
 * Parameters:   parent      Specifies the widget ID to use as the bases of 
 *                           interaction, usually a top level shell.
 *
 *               cursor      Specifies the cursor to be used for the pointer
 *                           during the interaction.  If a value of NULL is 
 *                           used this function will use a default cursor
 *                           value.
 *
 *               widget      This is the return value (e.g. the selected 
 *                           widget).  A value of NULL is returned on error.
 *
 * Return Value:  Status: (-1,0 or 1).
 *
 * Purpose: Allows developers to get the widget ID for any widget in their UI
 *          that the user has selected via the pointer.  This function will
 *          cause the cursor to change and allow a user to select an item in 
 *          the UI.
 *
 *****************************************************************************/
int DtHelpReturnSelectedWidgetId(
    Widget parent,
    Cursor cursor,
    Widget  *widget)
{


 Display   *dpy;
 int       screen;  
 Widget    selectedItem;
 int       status=DtHELP_SELECT_ERROR;
 Screen    *retScr;
 int	   result;
 _DtHelpWidgetToAppContext(parent);

  _DtHelpAppLock(app);
  /* Setup some needed variables */
  dpy = XtDisplay(parent);
  retScr = XtScreen(parent);
 
  screen = XScreenNumberOfScreen(retScr);
 
  /* refresh the display */
  XmUpdateDisplay(parent);

  /* Change the curser to let the user select the desired widget */
  selectedItem = LocateWidgetId(dpy, screen, &status, parent, cursor);
  
  switch (status)
    {
      case DtHELP_SELECT_VALID:
        *widget = selectedItem;
        result = DtHELP_SELECT_VALID;
	break;
     
      case DtHELP_SELECT_ABORT:
        *widget = NULL;
        result = DtHELP_SELECT_ABORT;
	break;

      case DtHELP_SELECT_ERROR:
        *widget = NULL;
        result = DtHELP_SELECT_ERROR;
	break;
 
      case DtHELP_SELECT_INVALID:
      default:
        *widget = NULL;
        result = DtHELP_SELECT_INVALID;
	break;
    }

  _DtHelpAppUnlock(app);
  return result;
}








/*****************************************************************************
 * Function:	    void _DtHelpTopicListAddToHead(
 *                                       char *locationId,
 *                                       int topicType,
 *                                       int maxNodex,
 *                                       DtTopicListStruct *pHead,
 *                                       DtTopicListStruct *pTale,
 *                                       totalNodes) 
 *                            
 *
 * Parameters:  
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Adds an element to the top of the given topicList.
 *
 *****************************************************************************/
void _DtHelpTopicListAddToHead(
    char *locationId,
    XmString topicTitle,
    int topicType,
    int maxNodes,
    char *accessPath,
    DtTopicListStruct **pHead,
    DtTopicListStruct **pTale,
    int *totalNodes,
    int scrollPosition)
{
  DtTopicListStruct *pTemp=NULL;

  /* add the new topic to the top */
  pTemp = (DtTopicListStruct *) XtMalloc((sizeof(DtTopicListStruct)));

  pTemp->pNext            = (*pHead);
  pTemp->pPrevious        = NULL;

  /* Assign the passed in values to our first element */
  pTemp->locationId       = XtNewString(locationId);
  pTemp->topicTitleLbl    = NULL;
  if (topicTitle != NULL)
      pTemp->topicTitleLbl= XmStringCopy(topicTitle);
  pTemp->topicType        = topicType;
  pTemp->helpVolume       = XtNewString(accessPath);
  pTemp->scrollPosition   = scrollPosition; 

  /* Add locationId as first element if pHead = NULL */
  if (*pHead == NULL)
    {
      /* Assign our tale pointer */
      (*pTale) = (*pHead);

      /* Make sure or totalNodes counter is correct, e.g. force it to 1 */
      *totalNodes = 1;
    }
  else 
    {  /* We have a list so add the new topic to the top */
    
     (*pHead)->pPrevious     = pTemp;

     /* Assign our tale pointer only the first time in this block */
     if (*totalNodes == 1)
       (*pTale) = (*pHead);

     /* Re-Assign our head pointer to point to the new head of the list */

     /* Bump our totalNode count */
     *totalNodes = *totalNodes +1;
    }

  /* set the head to the current entry */
  (*pHead) = pTemp;

   /* If we have reached our maxNodes remove a node from the end of our list */
   if (*totalNodes > maxNodes)
     {
       pTemp            = (*pTale);
       (*pTale)         = (*pTale)->pPrevious;
       (*pTale)->pNext  = NULL;
       pTemp->pPrevious = NULL;
               
       /* Free the id String and AccessPath elements */
       XtFree(pTemp->locationId);
       XtFree(pTemp->helpVolume);
       if (pTemp->topicTitleLbl != NULL)
           XmStringFree(pTemp->topicTitleLbl);

       /* Now, free the whole node */
       XtFree((char*)pTemp);

       /* Bump back our total node counter */
       *totalNodes = *totalNodes -1;

     }
}




/*****************************************************************************
 * Function:	    void _DtHelpTopicListDeleteHead(
 *                                       DtTopicListStruct *pHead,
 *                                       DtTopicListStruct *pTale,
 *                                       totalNodes) 
 *                            
 *
 * Parameters:  
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Delets an element from the top of the given topicList.
 *
 *****************************************************************************/
void _DtHelpTopicListDeleteHead(
    DtTopicListStruct **pHead,
    DtTopicListStruct **pTale,
    int *totalNodes)
{
  DtTopicListStruct *pTemp=NULL;

  /* Delete the top node in our topic list */
  if (*pHead != NULL)
    {
      pTemp = (*pHead);
      if(pTemp != (*pTale))        /* (e.g. more than one node in list) */
	{
           (*pHead)            = pTemp->pNext;
           pTemp->pNext        = NULL; 
           (*pHead)->pPrevious = NULL;

           /* Free the id String and accessPath elements */
           XtFree(pTemp->locationId);
           XtFree(pTemp->helpVolume);

           /* Now, free the whole node */
           XtFree((char*)pTemp);

           /* Bump back our total node counter */
           *totalNodes = *totalNodes -1;
         } 
    }
}






/*****************************************************************************
 * Function:	    void _DtHelpMapCB()
 *                   
 *                            
 *
 * Parameters:      client_data is the widget in reference to
 *                  which widget w is placed
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Determins where a new child dialog should be mapped in
 *                  relation to its parent.
 *
 * Algorithm:       1. attempt left or right placement with no overlap
 *                  2. if fails, attempt up or down placement with no overlap
 *                  3. if fails, determines location with least
 *                     amount of overlap, and places there.
 *
 *****************************************************************************/
XtCallbackProc _DtHelpMapCB(
    Widget w,
    XtPointer client_data,
    XtPointer call_data )
{
    Arg         args[2];
    Widget      parent;
    Position    centeredY, bestX, bestY, pX, pY; 
    Dimension   pHeight, myHeight, pWidth, myWidth;
    Dimension   maxX, maxY;
    int	        rhsX, lhsX, topY, botY;   /* needs to be int, not Dimension */
    Display *   display;
    Screen *    screen;
    int         screenNumber;

    parent = (Widget)client_data;
    display = XtDisplay(w);
    screen = XtScreen(w);
    screenNumber = XScreenNumberOfScreen(screen);

    pX = XtX(parent);
    pY = XtY(parent);
    if (pX < 0) pX = 0;
    if (pY < 0) pY = 0;
    pHeight = XtHeight(parent);
    pWidth = XtWidth(parent);
    myHeight = XtHeight(w);
    myWidth = XtWidth(w);
    maxX = XDisplayWidth(display,screenNumber);
    maxY = XDisplayHeight(display,screenNumber);

    /* algorithm 
     * 1. attempt left or right placement with no overlap
     * 2. if fails, attempt up or down placement with no overlap
     * 3. if fails, places on the right in the middle
     */
    
    /* first try left right placement */
    bestY = pY + pHeight/2 - myHeight/2;
    centeredY = bestY;
    rhsX = pX + pWidth;
    lhsX = pX - myWidth - 8;     /* 8: account for border */
    if ( ((int)(rhsX + myWidth)) < ((int) maxX) ) bestX = rhsX;
    else if ( lhsX > 0 ) bestX = lhsX;
    else
    {
        /* then try up down placement */
        bestX = pX + pWidth/2 - myWidth/2;
        botY = pY + pHeight;
        topY = pY - myHeight - 44;     /* 44: account for menu border */
        if ( ((int)(botY + myWidth)) < ((int) maxY) ) bestY = botY;
        else if ( topY > 0 ) bestY = topY;
        else
        {
            /* otherwise, center vertically and on the right */
            bestX = maxX - myWidth;
            bestY = centeredY;
        }
    }

    XtSetArg(args[0], XmNx, bestX);
    XtSetArg(args[1], XmNy,  bestY);
    XtSetValues(w, args, 2);

    return((XtCallbackProc) NULL);

}






/*****************************************************************************
 * Function:      void _DtHelpMapCenteredCB(
 *
 *
 *
 * Parameters:
 *
 * Return Value:    Void.
 *
 * Purpose:       Determins where the center of our help dialog is and sets
 *                  where new child dialog should be mapped such that its centered.
 *
 *****************************************************************************/
XtCallbackProc _DtHelpMapCenteredCB(
    Widget w,
    XtPointer client_data,
    XtPointer call_data )
{
    Arg         args[2];
    Widget      parent;
    Position newX, newY, pY, pX;
    Dimension pHeight, myHeight, pWidth, myWidth;

    parent = (Widget)client_data;

    pX = XtX(parent);
    pY = XtY(parent);
    pHeight = XtHeight(parent);
    pWidth = XtWidth(parent);
    myHeight = XtHeight(w);
    myWidth = XtWidth(w);


    newY = ((Position)(pHeight - myHeight) / 2) + pY;
    newX = ((Position)(pWidth - myWidth) / 2) + pX;

    XtSetArg(args[0], XmNx, newX);
    XtSetArg(args[1], XmNy, newY);
    XtSetValues(w, args, 2);

    return((XtCallbackProc) NULL);

}




/*****************************************************************************
 * Function:	   void _DtHelpDisplayDefinitionBox(
 *                            Widget new,   
 *                            Widget definitionBox,
 *                            char * path,
 *                            char * locationId);
 *       
 * Parameters:   
 *
 * Return Value:    
 *
 * Purpose: 	   This routine will create and post the definition box.
 *                 (e.g. the Quick Help Dialog widget)
 *
 ****************************************************************************/
void _DtHelpDisplayDefinitionBox(
    Widget parent,  
    Widget **definitionBox, 
    char * path,
    char * locationId)
{

  Arg  args[10];
  int    n;
  Widget printWidget, helpWidget, backWidget;
  XmString closeString;
  char *title;

  /* get the title from the main help dialog and use it here for */
  n = 0;
  XtSetArg (args[n], XmNtitle, &(title));  ++n;
  XtGetValues (XtParent(parent), args, n);


  if (*definitionBox == NULL)
    {

      /* Create the QuickHelpDialog widget to use as the definition box */
      closeString = XmStringCreateLocalized(((char *)_DTGETMESSAGE
                              (HUSET, 2,"Close")));
      n =0;
      XtSetArg (args[n], DtNhelpVolume, path);            n++; 
      XtSetArg (args[n], DtNhelpType, DtHELP_TYPE_TOPIC); n++; 
      XtSetArg (args[n], DtNlocationId, locationId);      n++;
      XtSetArg (args[n], DtNcloseLabelString, closeString);  n++;
      XtSetArg (args[n], XmNtitle, title);                n++;
      *definitionBox = 
           (Widget *)DtCreateHelpQuickDialog(parent, "definitionBox", 
                                               args, n);
      XmStringFree(closeString);

      /* Catch the close callback so we can destroy the widget */
      XtAddCallback((Widget)*definitionBox, DtNcloseCallback,
                    CloseDefBoxCB, (XtPointer) NULL);


      /* We do not want a print button for now so we unmap it */     
      printWidget = DtHelpQuickDialogGetChild ((Widget)*definitionBox, 
                                        DtHELP_QUICK_PRINT_BUTTON);
      XtUnmanageChild (printWidget);
  
      /* We do not want a help button for now so we unmap it */     
      helpWidget = DtHelpQuickDialogGetChild ((Widget)*definitionBox, 
                                       DtHELP_QUICK_HELP_BUTTON);
      XtUnmanageChild (helpWidget);


      /* We do not want a BACK button for now so we unmap it */     
      backWidget = DtHelpQuickDialogGetChild ((Widget)*definitionBox, 
                                           DtHELP_QUICK_BACK_BUTTON);
      XtUnmanageChild (backWidget);


  
      /*  Adjust the decorations for the dialog shell of the dialog  */
      n = 0;
      XtSetArg(args[n], XmNmwmFunctions, MWM_FUNC_RESIZE | MWM_FUNC_MOVE); n++;
      XtSetArg (args[n], XmNmwmDecorations, 
                MWM_DECOR_BORDER | MWM_DECOR_TITLE | MWM_DECOR_RESIZEH); n++;
      XtSetValues (XtParent(*definitionBox), args, n);
    
      /* Add the popup position callback to our history dialog */
      XtAddCallback (XtParent(*definitionBox), XmNpopupCallback,
                    (XtCallbackProc)_DtHelpMapCenteredCB, (XtPointer)XtParent(parent));



    }
  else
    {
       /* We already have one so lets use it. */

       /* Set the proper title */
       n = 0;
       XtSetArg (args[n], XmNtitle, title);  ++n;
       XtSetValues (XtParent(*definitionBox), args, n);
   
       /* Set the proper contents. */
       n = 0;
       XtSetArg (args[n], DtNhelpType, DtHELP_TYPE_TOPIC); n++; 
       XtSetArg (args[n], DtNhelpVolume, path);             n++; 
       XtSetArg (args[n], DtNlocationId, locationId);       n++;
       XtSetValues ((Widget)*definitionBox, args, n);
  
    }
 

  /* Display the dialog */
  XtManageChild((Widget)*definitionBox);     
  XtMapWidget(XtParent((Widget)*definitionBox));
 
}


/*****************************************************************************
 * Function:	   static void CloseDefBoxCB(
 *                            Widget w,   
 *                            XtPointer client_data,
 *                            XtPointer call_data);
 *       
 * Parameters:   
 *
 * Return Value:    
 *
 * Purpose: 	   This routine closes and destroys the Definition Box
 *                 Dialog Widget that we create.
 *
 ****************************************************************************/
static void  CloseDefBoxCB(
    Widget w,
    XtPointer client_data,
    XtPointer call_data )
{

   XtUnmanageChild(w);


}





/*****************************************************************************
 * Function:	   void _DtHelpDisplayFormatError()
  *       
 * Parameters:   
 *
 * Return Value:    
 *
 * Purpose: 	   This routine generate and display the proper errror 
 *                 message to the display area as well as send the proper 
 *                 error to XmWarning() function.
 *
 ****************************************************************************/
void _DtHelpDisplayFormatError(
    XtPointer displayArea,
    Widget widget,   
    char *userError,
    char *systemError)
{
   XtPointer  topicHandle;

   /* Set the string to the current help dialog */
   (void) _DtHelpFormatAsciiStringDynamic(displayArea, userError, &topicHandle);
         
   /* We ignor the status return here, because if we error out here we are
    * in big trouble because this is an error routine
    */
   
   _DtHelpDisplayAreaSetList (displayArea, topicHandle, FALSE, -1);

   if (systemError != NULL)  
     XmeWarning((Widget)widget, systemError);
  
}

 

/*****************************************************************************
 * Function:	   void _DtHelpCommonHelpInit()
  *       
 * Parameters:   
 *
 * Return Value:    
 *
 * Purpose: 	   This routine inits common help stuff
 *
 ****************************************************************************/
void _DtHelpCommonHelpInit(
    _DtHelpCommonHelpStuff * help)
{
   help->topLevelId          = NULL;
   help->currentHelpFile     = NULL;

   /* for help on help */
   if ( help->helpOnHelpVolume != _DtHelpDefaultHelp4HelpVolume)
       help->helpOnHelpVolume = XtNewString(help->helpOnHelpVolume);
   if ( NULL == help->helpOnHelpVolume ) 
       help->helpOnHelpVolume = (char *)_DtHelpDefaultHelp4HelpVolume;
   help->pHelpListHead = NULL;		/* Help List Pointer */
   help->onHelpDialog = NULL;		/* help on help dialog */
   help->pDisplayArea = NULL;		/* Display widget handle */

   /* get the search paths used by the widget */
   help->userVolumeSearchPath = _DtHelpGetUserSearchPath();
   help->sysVolumeSearchPath = _DtHelpGetSystemSearchPath();
}

 

/*****************************************************************************
 * Function:	   void _DtHelpCommonHelpClean()
  *       
 * Parameters:   
 *
 * Return Value:    
 *
 * Purpose: 	   This routine cleans up common help stuff
 *
 ****************************************************************************/
void _DtHelpCommonHelpClean(
    _DtHelpCommonHelpStuff * help,
    Boolean                 destroy)
{
   free(help->topLevelId);
   XtFree(help->currentHelpFile);

   help->topLevelId          = NULL;
   help->currentHelpFile     = NULL;

   if (destroy)
   {
      if (help->helpOnHelpVolume != _DtHelpDefaultHelp4HelpVolume)
         XtFree(help->helpOnHelpVolume);

      /* Free all the info we saved for our help callbacks */
      _DtHelpListFree(&help->pHelpListHead);

      XtFree(help->userVolumeSearchPath);
      XtFree(help->sysVolumeSearchPath);

      memset(help,0,sizeof(_DtHelpCommonHelpStuff));
   }
   else
   {
      /* Set our display area to a null starting vlaues */
      _DtHelpDisplayAreaClean(help->pDisplayArea);
   }
}

 

/*****************************************************************************
 * Function:	   void _DtHelpSetDlgButtonsWidth
 *       
 * Parameters:   
 *
 * Return Value:    
 *
 * Purpose: 	   This routine cleans up common help stuff
 *
 ****************************************************************************/
void _DtHelpSetButtonPositions(
    Widget     btnList[],
    int        numBtns,
    Dimension  minFormWidth,
    Dimension  btnMargins,
    Dimension  minBetweenBtnSpace)
{ /* position buttons */

      /* This code adds up the sizes of the buttons to go into
         the bottom row and calculates the form position percentages.
         All buttons are the same size, and are able to hold all
         the strings that may be dynamically placed in them.
         All buttons are 5% apart. */
      /* This code is specifically written to handle 3 buttons
         and assumes that the first 3 strings are to the ActionBtn */
   Dimension minWidthWithSpace = 0;
   Dimension borderWidth = 0;
   Dimension sumWidth = 0;
   Dimension leftPos = 0;
   Dimension rightPos = 0;
   Dimension spaceWidth = 0;
   Dimension btnWidth;
   Dimension maxBtnWidth = 0;
   float scale = 0.0;
   XmFontList fontList = NULL;
   int        i;
   int        n;
   Arg        args[5];

   if (numBtns <= 0 || NULL == btnList[0]) return;

   /* get the fontList for the button */
   XtSetArg (args[0], XmNborderWidth, &borderWidth);
   XtSetArg (args[1], XmNfontList, &fontList);
   XtGetValues (btnList[0], args, 2);
   /* assumption: the fontList that is returned is not owned by me; don't free */

   /* cycle through all buttons */
   for (i=0; i<numBtns && NULL!=btnList[i]; i++)
   {
      XmString labelString;

      /* get the label from the button */
      XtSetArg (args[0], XmNlabelString, &labelString);
      XtGetValues (btnList[i], args, 1);

      btnWidth = XmStringWidth(fontList,labelString) + borderWidth + btnMargins;
      if (btnWidth > maxBtnWidth) maxBtnWidth = btnWidth;

      XmStringFree(labelString);
   } /* for calcing widths */
   numBtns = i;  /* number of valid buttons */

   /* calc the space */
   sumWidth = maxBtnWidth * numBtns;
   minWidthWithSpace = sumWidth + minBetweenBtnSpace * (numBtns * 2);
   if (minWidthWithSpace > minWidthWithSpace) minFormWidth = minWidthWithSpace;
   spaceWidth = ((int)(minFormWidth - sumWidth) / (numBtns * 2));

   /* scale pixels to percent */
   scale = 100.0 / (float) minFormWidth;

   /* set the positions of each button */
   leftPos = spaceWidth;
   for ( i=0; i<numBtns; i++ )
   {
      rightPos = leftPos + maxBtnWidth;
   
      n = 0;
      XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);            n++;
      XtSetArg (args[n], XmNleftPosition, (Dimension) (((float) leftPos)*scale)); n++;
      XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);           n++;
      XtSetArg (args[n], XmNrightPosition,(Dimension) (((float) rightPos)*scale)); n++;
      XtSetValues (btnList[i], args, n);
   
      leftPos = rightPos + spaceWidth + spaceWidth;
   }  /* setup the positions for all buttons */
}  /* _DtHelpSetDlgButtonsWidth */



/*****************************************************************************
 * Function:	   _DtHelpXmFontListGetPropertyMax
 *       
 * Parameters:   
 *     fontList:  an XmFontList
 *     atom:      an XA_xxx value (see Vol 1, chpt 6.2.9)
 *     ret_propertyValue: ptr to long value that will hold the max value
 *
 * Return Value:    
 *     True: got at least one value
 *     False: unable to get any value; ret_propertyValue unchanged
 *
 * Purpose:
 *    This function returns the max value of XGetFontProperty calls 
 *    for each font in the XmFontList
 *    If there is an error, ret_propertyValue is left unchanged.
 *
 ****************************************************************************/
Boolean
_DtHelpXmFontListGetPropertyMax(
        XmFontList fontList,
        Atom atom,
        unsigned long *ret_propertyValue)
{
    Boolean gotValue = False;
    XmFontContext context;
    XmFontListEntry entry;
    XtPointer fontData;

    if (NULL == fontList) return False;           /* RETURN on error */

    /* walk through the fontList entries and add them in */
    XmFontListInitFontContext(&context,fontList);
    for ( entry = XmFontListNextEntry(context);
          NULL != entry;
          entry = XmFontListNextEntry(context) )
    {
       unsigned long value;
       XmFontType type;

       fontData = XmFontListEntryGetFont(entry,&type);
       if (XmFONT_IS_FONT == type)
       {
          XFontStruct * fontStruct;

          /* cast according to type */
          fontStruct = (XFontStruct *) fontData;

          if(XGetFontProperty(fontStruct, atom, &value) == TRUE) 
          {
             if(gotValue == False) /* haven't gotten any prop value yet */
             {
                 *ret_propertyValue = value;
                 gotValue = True;
             }
             else   /* have a good prop value already...get the max one */
             {
                 if(value > *ret_propertyValue)
                     *ret_propertyValue = value;
             }
          } /* if the getproperty call was good */
       }
       else  /* XmFONT_IS_FONTSET */
       {
          XFontSet fontSet;
          XFontStruct **font_list;
          char **name_list;
          int numfont;
          int i;

          /* cast according to type */
          fontSet = (XFontSet) fontData;
 
          numfont=XFontsOfFontSet(fontSet,&font_list,&name_list);
          for(i = 0; i < numfont; i++) 
          {
              if(XGetFontProperty(font_list[i], atom, &value) == TRUE) 
              {
                  if(gotValue == False) /* haven't gotten any prop value yet */
                  {
                      *ret_propertyValue = value;
                      gotValue = True;
                  }
                  else   /* have a good prop value already...get the max one */
                  {
                      if(value > *ret_propertyValue)
                          *ret_propertyValue = value;
                  }
              } /* if the getproperty call was good */
          } /* for all fonts in the font set */
       } /* this entry uses a font set */
    } /* for all font entries in the font list */
    XmFontListFreeFontContext(context);

    return(gotValue);
}
