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
/* $XConsortium: help.c /main/4 1996/03/25 13:12:20 ageorge $ */
/*                                                                      *
 *  help.c                                                              *
 *   Contains all support for help in the Calculator.                   *
 *                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc. 				*
 */

#include <Xm/Xm.h>
#include <Xm/XmP.h>
#include <Xm/MwmUtil.h>
#include <Xm/MessageB.h>

#include <Dt/Dt.h>
#include <Dt/Wsm.h>
#include <Dt/HelpDialog.h>

#include "calctool.h"
#include "motif.h"

#define  DIALOG_MWM_FUNC MWM_FUNC_MOVE | MWM_FUNC_CLOSE

extern char *base_str[] ;       /* Strings for each base value. */
extern char *dtype_str[] ;      /* Strings for each display mode value. */
extern char *mode_str[] ;       /* Strings for each mode value. */
extern char *ttype_str[] ;      /* Strings for each trig type value. */
extern Vars v ;                 /* Calctool variables and options. */
extern struct button buttons[] ;           /* Calculator button values. */


extern Boolean ignore_event;
extern XtIntervalId timerId;
extern void _DtChildPosition     P((Widget, Widget, Position *, Position *)) ;

typedef struct _helpStruct {
    struct _helpStruct *pNext;
    struct _helpStruct *pPrev;
    Widget dialog;
    Boolean inUse;
} HelpStruct;

static Widget GetHelpDialog     P(()) ;
static void UnmanageCB          P(()) ;

void
Help(char *helpVolume, char *locationId)
{
    Arg args[10];
    Position newX, newY;
    int n;

    if(X->helpDialog == NULL)
    {
        n = 0;
        XtSetArg(args[n], DtNhelpType, DtHELP_TYPE_TOPIC); n++;
        XtSetArg(args[n], DtNhelpVolume, helpVolume);   n++;
        XtSetArg(args[n], DtNlocationId, locationId);   n++;
        XtSetArg(args[n], XmNtitle, GETMESSAGE(4, 1, "Calculator - Help")); n++;
        X->helpDialog = DtCreateHelpDialog(X->mainWin, "helpDlg", args, n);

        DtWsmRemoveWorkspaceFunctions(X->dpy,
                      XtWindow(XtParent(X->helpDialog)));
        XtAddCallback(X->helpDialog, DtNhyperLinkCallback,
                      (XtCallbackProc)HelpHyperlinkCB, NULL);
        XtAddCallback(X->helpDialog, DtNcloseCallback,
                      (XtCallbackProc)HelpCloseCB, NULL);
    }
    else
    {
        n = 0;
        XtSetArg(args[n], DtNhelpType, DtHELP_TYPE_TOPIC); n++;
        XtSetArg(args[n], DtNhelpVolume, helpVolume);   n++;
        XtSetArg(args[n], DtNlocationId, locationId);   n++;
        XtSetValues(X->helpDialog, args, n);
    }

    XtSetMappedWhenManaged(XtParent(X->helpDialog), False);
    XSync(X->dpy, False);

    XtManageChild(X->helpDialog) ;

    _DtChildPosition(X->helpDialog, X->kframe, &newX, &newY);
    XtSetArg(args[0], XmNx, newX);
    XtSetArg(args[1], XmNy, newY);
    XtSetValues(X->helpDialog, args, 2);

    XtSetMappedWhenManaged(XtParent(X->helpDialog), True);
    XSync(X->dpy, False);

    XtMapWidget(XtParent(X->helpDialog));
    X->helpMapped = True;

}

void
HelpCloseCB(Widget widget, caddr_t client_data, caddr_t call_data)
{

  HelpStruct *pHelpCache = (HelpStruct *)client_data;
  HelpStruct *pHelp;
 
  for (pHelp = pHelpCache; pHelp != (HelpStruct *)NULL && pHelp->dialog !=widget;
                           pHelp = pHelp->pNext);
  if (pHelp != (HelpStruct *)NULL)
  {
     pHelp->inUse = False;
  }
  XtUnmapWidget(XtParent(widget));
  X->helpMapped = False;
}

void 
HelpHyperlinkCB(Widget widget, caddr_t client_data, caddr_t call_data)
{
  DtHelpDialogCallbackStruct *pHyper = (DtHelpDialogCallbackStruct *) call_data;
  
  switch(pHyper->hyperType)
  {
     case DtHELP_LINK_TOPIC:
          DisplayHelp(pHyper->helpVolume, pHyper->locationId);
          break;
     default:
          ;
   }
}
 

void 
HelpRequestCB(Widget widget, caddr_t client_data, caddr_t call_data)
{
  char  *helpVolume, *locationId;
  int   topic;

  topic = ((int) client_data) & 0xFFFF;
  helpVolume = HELP_VOLUME;

  if(topic < 56)
  {
     switch (topic)
     {
	   case 0:
               locationId = HELP_BLANK_STR;
               break;
	   case 8:
               locationId = HELP_ACC_STR;
               break;
	   case 12:
               if(v->modetype == LOGICAL)
                  locationId = HELP_LSHIFT_STR;
               else
                  locationId = buttons[topic].str;
               break;
	   case 13:
               if(v->modetype == LOGICAL)
                  locationId = HELP_RSHIFT_STR;
               else if(v->modetype == FINANCIAL)
                  locationId = HELP_INT_STR;
               else
                  locationId = buttons[topic].str;
               break;
	   case 14:
               if(v->modetype == LOGICAL)
                  locationId = HELP_TRUNC16_STR;
               else if(v->modetype == SCIENTIFIC)
                  locationId = HELP_ETOX_STR;
               else
                  locationId = buttons[topic].str;
               break;
	   case 15:
               if(v->modetype == LOGICAL)
                  locationId = HELP_TRUNC32_STR;
               else if(v->modetype == SCIENTIFIC)
                  locationId = HELP_TENTOX_STR;
               else
                  locationId = buttons[topic].str;
               break;
	   case 16:
               if(v->modetype == SCIENTIFIC)
                  locationId = HELP_YTOX_STR;
               else
                  locationId = buttons[topic].str;
               break;
	   case 17:
               if(v->modetype == SCIENTIFIC)
                  locationId = HELP_XFACT_STR;
               else if(v->modetype == FINANCIAL)
                  locationId = HELP_PAYPYR_STR;
               else
                  locationId = buttons[topic].str;
               break;
	   case 21:
               if(v->modetype == LOGICAL)
                  locationId = HELP_BLANK_STR;
               else
                  locationId = buttons[topic].str;
               break;
	   case 22:
               if(v->modetype == LOGICAL)
                  locationId = HELP_BLANK_STR;
               else
                  locationId = buttons[topic].str;
               break;
	   case 23:
               if(v->modetype == LOGICAL || v->modetype == FINANCIAL)
                  locationId = HELP_BLANK_STR;
               else
                  locationId = buttons[topic].str;
               break;
	   case 24:
               locationId = HELP_RECIP_STR;
               break;
	   case 25:
               locationId = HELP_SQUARE_STR;
               break;
	   case 26:
               locationId = HELP_SQRT_STR;
               break;
	   case 27:
               locationId = HELP_PERCENT_STR;
               break;
	   case 28:
               locationId = HELP_LPAREN_STR;
               break;
	   case 29:
               locationId = HELP_RPAREN_STR;
               break;
	   case 35:
               locationId = HELP_CHGSIGN_STR;
               break;
	   case 39:
               locationId = HELP_TIMES_STR;
               break;
	   case 40:
               locationId = HELP_SEVEN_STR;
               break;
	   case 41:
               locationId = HELP_EIGHT_STR;
               break;
	   case 42:
               locationId = HELP_NINE_STR;
               break;
	   case 43:
               locationId = HELP_DIVIDE_STR;
               break;
	   case 44:
               locationId = HELP_FOUR_STR;
               break;
	   case 45:
               locationId = HELP_FIVE_STR;
               break;
	   case 46:
               locationId = HELP_SIX_STR;
               break;
	   case 47:
               locationId = HELP_MINUS_STR;
               break;
	   case 48:
               locationId = HELP_ONE_STR;
               break;
	   case 49:
               locationId = HELP_TWO_STR;
               break;
	   case 50:
               locationId = HELP_THREE_STR;
               break;
	   case 51:
               locationId = HELP_PLUS_STR;
               break;
	   case 52:
               locationId = HELP_ZERO_STR;
               break;
	   case 53:
               locationId = HELP_DECIMAL_STR;
               break;
	   case 54:
               locationId = HELP_EQUAL_STR;
               break;
           default:
               locationId = buttons[topic].str;
     }
  }
  else
  {
     switch (topic)
     {
	   case HELP_HELP_MENU:
	       locationId = HELP_HELP_MENU_STR;
               break;
           case HELP_INTRODUCTION:
               locationId = HELP_INTRODUCTION_STR;
               break;
           case HELP_TABLEOFCONTENTS:
               locationId = HELP_TABLEOFCONTENTS_STR;
               break;
           case HELP_TASKS:
               locationId = HELP_TASKS_STR;
               break;
           case HELP_REFERENCE:
               locationId = HELP_REFERENCE_STR;
               break;
           case HELP_ITEM:
               locationId = HELP_ITEM_STR;
               break;
           case HELP_USING:
               locationId = HELP_USING_STR;
               helpVolume = HELP_USING_HELP_VOLUME;
               break;
           case HELP_VERSION:
               locationId = HELP_VERSION_STR;
               break;
           case HELP_ASCII:
               locationId = HELP_ASCII_STR;
               break;
           case HELP_CONSTANT:
               locationId = HELP_CONSTANT_STR;
               break;
           case HELP_FUNCTION:
               locationId = HELP_FUNCTION_STR;
               break;
           case HELP_DISPLAY:
               locationId = HELP_DISPLAY_STR;
               break;
           case HELP_MODELINE:
               locationId = HELP_MODELINE_STR;
               break;
           case HELP_MODE:
               locationId = mode_str[(int) v->modetype];
               break;
           case HELP_BASE:
               locationId = base_str[(int) v->base];
               break;
           case HELP_NOTATION:
               locationId = dtype_str[(int) v->dtype];
               break;
           case HELP_TRIG:
               locationId = ttype_str[(int) v->ttype];
               break;
           case HELP_MENUBAR:
               locationId = HELP_MENUBAR_STR;
               break;
           default:
               locationId = HELP_INTRODUCTION_STR;
     }
  }
  Help(helpVolume, locationId);
  ignore_event = True;
  timerId = XtAppAddTimeOut (XtWidgetToApplicationContext (X->kframe), 300,
                         (XtTimerCallbackProc) TimerEvent, (XtPointer) NULL);
}

void
HelpModeCB(Widget w, caddr_t client_data, caddr_t call_data)
{
  Widget widget;
  char *errorMsg, *tmp;
 
  switch(DtHelpReturnSelectedWidgetId(X->mainWin, (Cursor)NULL, &widget))
   {
      case DtHELP_SELECT_VALID:
	while (!XtIsShell(widget))
	{
		if(XtHasCallbacks(widget, XmNhelpCallback) == XtCallbackHasSome)
		{
			XtCallCallbacks(widget, XmNhelpCallback, 
                                                       (XtPointer)client_data);
			return;
		}
		widget = XtParent(widget);
	}
	break;

     case DtHELP_SELECT_INVALID:
        errorMsg = GETMESSAGE(4, 2, "You must select an item within Calculator");
        tmp = XtNewString(errorMsg);
	ErrDialog((char *) tmp, X->mainWin);
        XtFree(tmp);
        break;
   }
}

void
DisplayHelp(char *helpVolume, char *locationId)
{
    Arg args[10];
    int n;

    X->helpDialog = GetHelpDialog();

    n = 0;
    XtSetArg(args[n], DtNhelpType, DtHELP_TYPE_TOPIC); n++;
    XtSetArg(args[n], DtNhelpVolume, helpVolume);       n++;
    XtSetArg(args[n], DtNlocationId, locationId);       n++;
    XtSetValues(X->helpDialog, args, n);

    XtManageChild(X->helpDialog);
    XtMapWidget(XtParent(X->helpDialog));
    X->helpMapped = True;
}


static Widget
GetHelpDialog(void)
{
    static HelpStruct       *pHelpCache;

    HelpStruct *pHelp;
    Arg args[5];

    if(pHelpCache == (HelpStruct *)NULL)
    {
        pHelp = pHelpCache = (HelpStruct *)XtMalloc(sizeof(HelpStruct));
        pHelp->pNext = (HelpStruct *)NULL;
        pHelp->pPrev = (HelpStruct *)NULL;
        pHelp->inUse = True;

        XtSetArg(args[0], XmNtitle, GETMESSAGE(4, 1, "Calculator - Help"));
        pHelp->dialog = DtCreateHelpDialog(X->mainWin, "helpDlg",  args, 1);

        DtWsmRemoveWorkspaceFunctions(X->dpy,
                XtWindow(XtParent(pHelp->dialog)));
        XtAddCallback(pHelp->dialog, DtNhyperLinkCallback,
                      (XtCallbackProc)HelpHyperlinkCB, NULL);
        XtAddCallback(pHelp->dialog, DtNcloseCallback,
                      (XtCallbackProc)HelpCloseCB, pHelpCache);
        return pHelp->dialog;
    }
    else
    {
        for(pHelp = pHelpCache;
            pHelp != (HelpStruct *)NULL;
            pHelp = pHelp->pNext)
        {
            if(pHelp->inUse == False)
            {
                pHelp->inUse = True;
                return pHelp->dialog;
            }
        }
        pHelp = (HelpStruct *) XtMalloc(sizeof(HelpStruct));
        pHelpCache->pPrev = pHelp;
        pHelp->pNext = pHelpCache;
        pHelpCache = pHelp;
        pHelp->pPrev = (HelpStruct *)NULL;
        pHelp->inUse = True;

        XtSetArg(args[0], XmNtitle, GETMESSAGE(4, 1, "Calculator - Help"));
        pHelp->dialog = DtCreateHelpDialog(X->mainWin, "helpDlg",  args, 1);
        DtWsmRemoveWorkspaceFunctions(X->dpy,
                      XtWindow(XtParent(pHelp->dialog)));
        XtAddCallback(pHelp->dialog, DtNhyperLinkCallback,
                      (XtCallbackProc)HelpHyperlinkCB, NULL);
        XtAddCallback(pHelp->dialog, DtNcloseCallback,
                      (XtCallbackProc)HelpCloseCB, pHelpCache);
        return pHelp->dialog;
    }
}

void
ErrDialog(char *errString, Widget visualParent)
{
  int   n;
  Arg   args[10];
  XmString label, ok;

  label = XmStringCreateLocalized(errString) ;
  X->errParent = visualParent;

  if (X->errDialog == NULL)
  {
    ok = XmStringCreateLocalized(GETMESSAGE(4, 4, "OK"));

    n = 0;
    XtSetArg (args[n], XmNokLabelString, ok);           n++;
    XtSetArg (args[n], XmNmessageString, label);        n++;
    XtSetArg (args[n], XmNmwmFunctions, DIALOG_MWM_FUNC); n++;
    XtSetArg (args[n], XmNautoUnmanage,  FALSE);        n++;
    XtSetArg (args[n], XmNdefaultPosition, FALSE);      n++;

    X->errDialog = XmCreateErrorDialog (X->mainWin, "ErroNotice", args, n);

    XtAddCallback (X->errDialog, XmNokCallback, UnmanageCB, NULL);
    XtAddCallback (X->errDialog, XmNmapCallback, CenterMsgCB, NULL);

    XtUnmanageChild (XmMessageBoxGetChild (X->errDialog, 
                                                    XmDIALOG_CANCEL_BUTTON));

    XtUnmanageChild (XmMessageBoxGetChild (X->errDialog, XmDIALOG_HELP_BUTTON));

    n = 0;
    XtSetArg (args[n], XmNmwmInputMode, 
                                  MWM_INPUT_PRIMARY_APPLICATION_MODAL);n++;
    XtSetArg (args[n], XmNuseAsyncGeometry, TRUE); n++;
    XtSetArg (args[n], XmNtitle, GETMESSAGE(4, 3, "Error")); n++;
    XtSetValues (XtParent(X->errDialog), args, n);
  }
  else
  {
     XtSetArg (args[0], XmNmessageString, label);
     XtSetValues (X->errDialog, args, 1);
  }
  XmStringFree (label);
  XtManageChild(X->errDialog);
}

static void
UnmanageCB(Widget  widget, XtPointer client_data, XtPointer call_data)
{
    XtUnmanageChild(widget);
}

void
CenterMsgCB (Widget widget, XtPointer client_data, XtPointer call_data)
{
    int n;
    Position newX, newY;
    Arg args[4];
    Widget   shell;

    shell = (Widget)client_data;
    if (shell == NULL)
    {
        if (!X->errParent || !XtParent(X->errParent) ||
                !XtIsManaged(X->errParent))
                        shell = X->mainWin;
        else
                shell = XtParent(X->errParent);
    }
    else
        shell = XtParent (shell);

    newX = XtX(shell) + XtWidth(shell)/2 - XtWidth(widget)/2;
    newY = XtY(shell) + XtHeight(shell)/2 - XtHeight(widget)/2;

    if (newX < 0) newX = 0;
    if (newY < 0) newY = 0;

    n = 0;
    XtSetArg (args[n], XmNx, newX);             n++;
    XtSetArg (args[n], XmNy, newY);             n++;
    XtSetValues(widget, args, n);
}

