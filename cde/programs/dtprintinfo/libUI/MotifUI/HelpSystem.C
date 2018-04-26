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
/* $XConsortium: HelpSystem.C /main/2 1995/07/17 14:05:53 drk $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include "HelpSystem.h"

#include <stdio.h>

#include <Xm/AtomMgr.h>
#include <Xm/Protocols.h>
#include <Dt/Help.h>
#include <Dt/HelpDialog.h>
#include <Dt/HelpQuickD.h>
#include <Dt/Wsm.h>

HelpSystem::HelpSystem(MotifUI *parent,
	               char *name,
	               char *volume,
	               char *location_id,
	               HelpStyle style)
	: MotifUI(parent, name, NULL)
{
   CreateHelpDialog(parent, name, volume, location_id, style);
}

HelpSystem::HelpSystem(char *category,
	               MotifUI *parent,
	               char *name,
	               char *volume,
	               char *location_id,
	               HelpStyle style)
	: MotifUI(parent, name, category)
{
   CreateHelpDialog(parent, name, volume, location_id, style);
}

void HelpSystem::CreateHelpDialog(MotifUI *parent, char *name, char *volume, 
			          char *location_id, HelpStyle style)
{
   Arg args[6];
   int n;

   _style = style;
   n = 0;
   XtSetArg(args[n], XmNtitle, name); n++;
   XtSetArg(args[n], XmNautoUnmanage, false); n++;
   XtSetArg(args[n], DtNshowNewWindowButton, false); n++;
   XtSetArg(args[n], DtNhelpType, DtHELP_TYPE_TOPIC); n++;
   XtSetArg(args[n], DtNhelpVolume, volume); n++;
   XtSetArg(args[n], DtNlocationId, location_id); n++;
   if (_style == HELP_DIALOG)
      _w = DtCreateHelpDialog(parent->InnerWidget(), "helpDlg", args, n);
   else
      _w = DtCreateHelpQuickDialog(parent->InnerWidget(), "helpDlg", args, n);

   DtWsmRemoveWorkspaceFunctions(display, XtWindow(XtParent(_w)));
   XtAddCallback(_w, DtNhyperLinkCallback, 
      (XtCallbackProc)HyperlinkCB, (XtPointer) this);
   XtAddCallback(_w, DtNcloseCallback, (XtCallbackProc)CloseCB,
      (XtPointer) this);
   XmAddWMProtocolCallback(XtParent(_w), 
      XmInternAtom(display, "WM_DELETE_WINDOW", False), 
      &HelpSystem::CloseCB, (XtPointer) this);
}

boolean HelpSystem::SetVisiblity(boolean flag)
{
   MotifUI::SetVisiblity(flag);
   if (flag == true && XtIsRealized(XtParent(_w)))
      XtMapWidget(XtParent(_w));
   return true;
}

void HelpSystem::HyperlinkCB(Widget, XtPointer data, XtPointer)
{
   HelpSystem *obj = (HelpSystem *)data;
}

void HelpSystem::CloseCB(Widget, XtPointer data, XtPointer)
{
   HelpSystem *obj = (HelpSystem *)data;
   obj->Visible(false);
}

char *HelpSystem::HelpVolume()
{
   char *value;
   XtVaGetValues(_w, DtNhelpVolume, &value, NULL);
   return value;
}

char *HelpSystem::LocationID()
{
   char *value;
   XtVaGetValues(_w, DtNlocationId, &value, NULL);
   return value;
}

void HelpSystem::LocationID(char *locationId)
{
   //if (strcmp(LocationID(), locationId))
      XtVaSetValues(_w, DtNhelpType, DtHELP_TYPE_TOPIC, 
	            DtNlocationId, locationId, NULL);
}

void HelpSystem::HelpVolume(char *helpVolume, char *locationId)
{
   //if (strcmp(HelpVolume(), helpVolume) || strcmp(LocationID(), locationId))
      XtVaSetValues(_w, DtNhelpVolume, helpVolume, DtNlocationId, locationId,
                    DtNhelpType, DtHELP_TYPE_TOPIC, NULL);
}
