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
/*******************************************************************************
**
**  help.c
**
**  $XConsortium: help.c /main/7 1996/04/19 15:47:41 rswiston $
**
**  RESTRICTED CONFIDENTIAL INFORMATION:
**
**  The information in this document is subject to special
**  restrictions in a confidential disclosure agreement between
**  HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
**  document outside HP, IBM, Sun, USL, SCO, or Univel without
**  Sun's specific written approval.  This document and all copies
**  and derivative works thereof must be returned or destroyed at
**  Sun's request.
**
**  Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
**
*******************************************************************************/

/*									*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc. 				*
 */

#ifndef lint
static char sccsid[] = "@(#)help.c 1.19 95/03/28 Copyr 1991 Sun Microsystems, Inc.";
#endif

#include <EUSCompat.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/param.h> /* MAXPATHLEN defined here */
#ifdef SVR4
#include <sys/utsname.h> /* SYS_NMLN */
#endif /* SVR4 specific includes */
#include <dirent.h>
#include <ctype.h>
#include <string.h>
#include <sys/time.h>
#include <sys/signal.h>
#include <rpc/rpc.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <X11/Xlib.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/LabelG.h>
#include <Xm/List.h>
#include <Xm/DialogS.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/Scale.h>
#include <Xm/SeparatoG.h>
#include <Xm/TextF.h>
#include <Xm/Text.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>
#include <Dt/HourGlass.h>
#include <Dt/HelpDialog.h>
#include "calendar.h"
#include "revision.h"
#include "help.h"
#include "props_pu.h"

static void display_help(Calendar *, ArgList, int);

static void
display_help(Calendar *c, ArgList args, int nargs)
{
  if (!c->mainhelpdialog)
  {
    Arg oneArg;
    char *title = XtNewString(catgets(c->DT_catd, 1, 1076, "Calendar : Help"));

    XtSetArg(oneArg, XmNtitle, title);
    c->mainhelpdialog =
      (Widget)DtCreateHelpDialog(c->frame, "mainhelpdialog", &oneArg, 1);
    XtFree(title);
  }

  XtSetValues(c->mainhelpdialog, args, nargs);
  XtManageChild(c->mainhelpdialog);
}

void show_main_help(Widget w, XtPointer clientData, XtPointer cbs)
{
        int 		ac;
        Arg 		al[10];
        Calendar 	*c = calendar;
	Props_pu       	*p = (Props_pu *) c->properties_pu;
        Widget 		selWidget = NULL;
        int 		status = DtHELP_SELECT_ERROR;
 
        ac = 0;
        XtSetArg(al[ac], DtNhelpType, DtHELP_TYPE_TOPIC); ac++;
        switch ((int)clientData) {
            case HELP_OVERVIEW:
        	XtSetArg(al[ac], DtNhelpVolume, "Calendar"); ac++;
                XtSetArg(al[ac], DtNlocationId, "_HOMETOPIC"); ac++;
                break;
            case HELP_USINGHELP:
        	XtSetArg(al[ac], DtNhelpVolume, "Help4Help"); ac++;
                XtSetArg(al[ac], DtNlocationId, "_HOMETOPIC"); ac++;
                break;
            case HELP_TASKS:
        	XtSetArg(al[ac], DtNhelpVolume, "Calendar"); ac++;
                XtSetArg(al[ac], DtNlocationId, "TASKS"); ac++;
                break;
            case HELP_REFERENCE:
        	XtSetArg(al[ac], DtNhelpVolume, "Calendar"); ac++;
                XtSetArg(al[ac], DtNlocationId, "REFERENCE"); ac++;
                break;
            case HELP_ABOUTCALENDAR:
        	XtSetArg(al[ac], DtNhelpVolume, "Help4Help"); ac++;
                XtSetArg(al[ac], DtNlocationId, "_COPYRIGHT"); ac++;
                break;
            case HELP_TOC:
        	XtSetArg(al[ac], DtNhelpVolume, "Calendar"); ac++;
                XtSetArg(al[ac], DtNlocationId, "TOC"); ac++;
                break;
            case HELP_ONITEM:
               while (!XtIsSubclass(w, applicationShellWidgetClass))
                   w = XtParent(w);
 
               status = DtHelpReturnSelectedWidgetId(w, NULL, &selWidget);     
               switch ((int)status)
                  {
                    case DtHELP_SELECT_ERROR:
                      printf("%s", catgets(c->DT_catd, 1, 350, "Selection Error, cannot continue\n"));
                    break;
                    case DtHELP_SELECT_VALID:
                      while (selWidget != NULL)
                        {
                          if ((XtHasCallbacks(selWidget, XmNhelpCallback)                                            == XtCallbackHasSome))
                            {
                               XtCallCallbacks((Widget)selWidget,
                                               XmNhelpCallback,NULL);
                               break;
                            }
                          else
                           selWidget = XtParent(selWidget);
                        }
                    break;
                    case DtHELP_SELECT_ABORT:
                      return;

                    case DtHELP_SELECT_INVALID:
		      {
			char *title = XtNewString(catgets(c->DT_catd, 1, 110, "Calendar : Error - Help"));
			char *text = XtNewString(catgets(c->DT_catd, 1, 352, "You must select a component within your application."));
			char *ident1 = XtNewString(catgets(c->DT_catd, 1, 95, "Continue"));
			dialog_popup(c->frame,
				     DIALOG_TITLE, title,
				     DIALOG_TEXT, text,
				     BUTTON_IDENT, 1, ident1,
				     DIALOG_IMAGE, p->xm_error_pixmap,
				     NULL);
			XtFree(ident1);
			XtFree(text);
			XtFree(title);
			return;
		      }
                  }
 
               break;
 
            default:
        	XtSetArg(al[ac], DtNhelpVolume, "Calendar"); ac++;
                XtSetArg(al[ac], DtNlocationId, "_HOMETOPIC"); ac++;
                break;
        }

	display_help(c, al, ac);
}

extern void
help_cb(Widget w, XtPointer clientData, XtPointer callData)
{
        int ac;
        Arg al[10];
        Calendar *c = calendar;
	char *locationid = (char *) clientData;

        ac = 0;
        XtSetArg(al[ac], DtNhelpType, DtHELP_TYPE_TOPIC); ac++;
        XtSetArg(al[ac], DtNhelpVolume, "Calendar"); ac++;
        XtSetArg(al[ac], DtNlocationId, locationid); ac++;

	display_help(c, al, ac);
}

/* special case where we must determine the view */
extern void
help_view_cb(Widget w, XtPointer clientData, XtPointer callData)
{
        Calendar *c = calendar;

	switch (c->view->glance) {
		case dayGlance:
			help_cb(w, (XtPointer)CALENDAR_HELP_DAY_WINDOW, callData);
			break;
		case weekGlance:
			help_cb(w, (XtPointer)CALENDAR_HELP_WEEK_WINDOW, callData);
			break;
		case monthGlance:
			help_cb(w, (XtPointer)CALENDAR_HELP_MONTH_WINDOW, callData);
			break;
		default:
			break;
	}
}
