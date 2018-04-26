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
/* $XConsortium: MenuBar.C /main/2 1995/07/17 14:06:37 drk $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include "MenuBar.h"
#include "Menu.h"
#include <Xm/RowColumn.h>

MenuBar::MenuBar(MotifUI *parent,
		 char *name)
	: MotifUI(parent, name, NULL)
{
   _w = XtVaCreateManagedWidget("menuBar", xmRowColumnWidgetClass, 
                                parent->InnerWidget(),
				XmNrowColumnType, XmMENU_BAR, NULL);

   XtVaSetValues(parent->InnerWidget(), XmNmenuBar, _w, NULL);
   InstallHelpCB();
}

void MenuBar::SetHelpMenu(Menu *menu)
{
   XtVaSetValues(_w, XmNmenuHelpWidget, menu->GetCascade(), NULL);
}

MenuBar::~MenuBar()
{
   // Empty
}
