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
