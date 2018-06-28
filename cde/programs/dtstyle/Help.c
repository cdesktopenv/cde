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
/* $XConsortium: Help.c /main/4 1995/10/30 13:09:47 rswiston $ */
/****************************************************************************
 ****************************************************************************
 **
 **   File:        HelpCB.h
 **
 **   Project:     DT 3.0
 **
 **   Description: Routines that create/cache the dialogs for Help
 **
 **
 **  (c) Copyright Hewlett-Packard Company, 1990, 1991, 1992.  All rights are
 **  reserved.  Copying or other reproduction of this program
 **  except for archival purposes is prohibited without prior
 **  written consent of Hewlett-Packard Company.
 **
 **
 **
 ****************************************************************************/

#include <stdio.h>

#include <Xm/DialogS.h>
#include <Xm/Form.h>
#include <Xm/MwmUtil.h>
#include <Dt/DtP.h>

#include <Dt/Help.h>
#include <Dt/HelpDialog.h>

#include "Main.h"
#include "Help.h"

static Widget
GetHelpDialog(void)
{
    static HelpStruct       *pHelpCache;

    HelpStruct *pHelp;
    Arg args[5];
    int n;

    if(pHelpCache == (HelpStruct *)NULL)
    {
	pHelp = pHelpCache = (HelpStruct *)XtMalloc(sizeof(HelpStruct));
	pHelp->pNext = (HelpStruct *)NULL;
	pHelp->pPrev = (HelpStruct *)NULL;
	pHelp->inUse = True;

	n = 0;
        XtSetArg(args[n], XmNtitle, 
            ((char *)GETMESSAGE(2, 7, "Style Manager - Help"))); n++;
	pHelp->dialog = DtCreateHelpDialog(style.shell, "helpDlg", 
					    args, n);

        DtWsmRemoveWorkspaceFunctions(style.display, 
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

        n=0;
        XtSetArg(args[n], XmNtitle, 
            ((char *)GETMESSAGE(2, 7, "Style Manager - Help"))); n++;
	pHelp->dialog = DtCreateHelpDialog(style.shell, "helpDlg", 
					    args, n);
        DtWsmRemoveWorkspaceFunctions(style.display, 
                      XtWindow(XtParent(pHelp->dialog)));
        XtAddCallback(pHelp->dialog, DtNhyperLinkCallback, 
                      (XtCallbackProc)HelpHyperlinkCB, NULL);
        XtAddCallback(pHelp->dialog, DtNcloseCallback, 
                      (XtCallbackProc)HelpCloseCB, pHelpCache);
	return pHelp->dialog;
    }
}

void
DisplayHelp(
        char *helpVolume,
	char *locationId)
{
    Widget helpDialog;
    Arg args[10];
    int n;

    helpDialog = GetHelpDialog();

    n = 0;
    XtSetArg(args[n], DtNhelpType, DtHELP_TYPE_TOPIC); n++;
    XtSetArg(args[n], DtNhelpVolume, helpVolume);	n++;
    XtSetArg(args[n], DtNlocationId, locationId);	n++;
    XtSetValues(helpDialog, args, n);

    XtManageChild(helpDialog);
    XtMapWidget(XtParent(helpDialog));
}

void
Help(
        char *helpVolume,
	char *locationId)
{
    static  Widget helpDialog = NULL;
    Arg args[10];
    int n;

    if(helpDialog == NULL)
    {
        n = 0;
        XtSetArg(args[n], DtNhelpType, DtHELP_TYPE_TOPIC); n++;
        XtSetArg(args[n], DtNhelpVolume, helpVolume);	n++;
        XtSetArg(args[n], DtNlocationId, locationId);	n++;
        XtSetArg (args[n], XmNtitle, 
            ((char *)GETMESSAGE(2, 7, "Style Manager - Help"))); n++;
	helpDialog = DtCreateHelpDialog(style.shell, "helpDlg", args, n);

        DtWsmRemoveWorkspaceFunctions(style.display, 
                      XtWindow(XtParent(helpDialog)));
        XtAddCallback(helpDialog, DtNhyperLinkCallback, 
                      (XtCallbackProc)HelpHyperlinkCB, NULL);
        XtAddCallback(helpDialog, DtNcloseCallback, 
                      (XtCallbackProc)HelpCloseCB, NULL);

        XtManageChild(helpDialog);
    }
    else
    {
        n = 0;
        XtSetArg(args[n], DtNhelpType, DtHELP_TYPE_TOPIC); n++;
        XtSetArg(args[n], DtNhelpVolume, helpVolume);	n++;
        XtSetArg(args[n], DtNlocationId, locationId);	n++;
        XtSetValues(helpDialog, args, n);
    }

    XtMapWidget(XtParent(helpDialog));
}

