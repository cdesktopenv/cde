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
/* $TOG: IndexedOptionMenu.C /main/3 1997/11/21 18:42:21 mgreess $ */

/*
 *+SNOTICE
 *
 *	$:$
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1994 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */
/*
 *		     Common Desktop Environment
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *   (c) Copyright 1995 Digital Equipment Corp.
 *   (c) Copyright 1995 Fujitsu Limited
 *   (c) Copyright 1995 Hitachi, Ltd.
 *                                                                   
 *
 *                     RESTRICTED RIGHTS LEGEND                              
 *
 *Use, duplication, or disclosure by the U.S. Government is subject to
 *restrictions as set forth in subparagraph (c)(1)(ii) of the Rights in
 *Technical Data and Computer Software clause in DFARS 252.227-7013.  Rights
 *for non-DOD U.S. Government Departments and Agencies are as set forth in
 *FAR 52.227-19(c)(1,2).

 *Hewlett-Packard Company, 3000 Hanover Street, Palo Alto, CA 94304 U.S.A.
 *International Business Machines Corp., Route 100, Somers, NY 10589 U.S.A. 
 *Sun Microsystems, Inc., 2550 Garcia Avenue, Mountain View, CA 94043 U.S.A.
 *Novell, Inc., 190 River Road, Summit, NJ 07901 U.S.A.
 *Digital Equipment Corp., 111 Powdermill Road, Maynard, MA 01754, U.S.A.
 *Fujitsu Limited, 1015, Kamikodanaka Nakahara-Ku, Kawasaki 211, Japan
 *Hitachi, Ltd., 6, Kanda Surugadai 4-Chome, Chiyoda-ku, Tokyo 101, Japan
 */

#include <stdio.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/DialogS.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleB.h>

#include "Dmx.h"
#include "IndexedOptionMenu.h"
#include "MailMsg.h"
#include "dtmailopts.h"

IndexedOptionMenu::IndexedOptionMenu (
				Widget	parent,
				int	nmenu_items,
				char	**strings,
				void	**data
			 	) : UIComponent( "IndexedOptionMenu" )
{
    int		nargs;
    Arg		args[8];
    Widget	menu;
    XmString	xms;

    _nmenu_items = nmenu_items;
    if (nmenu_items && strings != NULL)
    {
	_strings = (char **) XtMalloc(nmenu_items * sizeof(char*));
	for (int i=0; i<nmenu_items; i++)
	  _strings[i] = strings[i];
    }
    if (nmenu_items && data != NULL)
    {
	_data = (void **) XtMalloc(nmenu_items * sizeof(void*));
	for (int i=0; i<nmenu_items; i++)
	  _data[i] = data[i];
    }

    /*
     * Create the pulldown menu for the option menus,
     * and populate it with pushbuttons.
     * Attach the original strings used to generate
     * the button labels to the buttons as userData.
     */
    menu = XmCreatePulldownMenu(parent, "Menu", NULL, 0);
    _buttons = (Widget *) XtMalloc( _nmenu_items * sizeof(Widget) );
    for (int i=0; i<_nmenu_items; i++)
    {
	static char	button_label[32];

	sprintf(button_label, "Button%d", i);
	_buttons[i] = XmCreatePushButton(menu, button_label, NULL, 0);
	xms = XmStringCreateLocalized(_strings[i]);
        XtVaSetValues(
		_buttons[i],
		XmNuserData, i,
		XmNlabelString, xms,
		NULL);
        XmStringFree(xms);
	XtManageChild(_buttons[i]);
    }

    nargs=0;
    XtSetArg(args[nargs], XmNsubMenuId, menu); nargs++;
    _w = XmCreateOptionMenu(parent, "IndexedOptionMenu", args, nargs);
    installDestroyHandler();

    //XtRealizeWidget(_w);
}

IndexedOptionMenu::IndexedOptionMenu (
				Widget	option_menu,
				int	nmenu_items,
				char	**strings,
				void	**data,
				Widget	*buttons
			 	) : UIComponent( "IndexedOptionMenu" )
{
    _w = option_menu;
    installDestroyHandler();
    _nmenu_items = nmenu_items;
    _strings = NULL;
    _data = NULL;
    _buttons = NULL;

    if (nmenu_items && strings != NULL)
    {
	_strings = (char **) XtMalloc(nmenu_items * sizeof(char*));
	for (int i=0; i<nmenu_items; i++)
	  _strings[i] = strings[i];
    }
    if (nmenu_items && data != NULL)
    {
	_data = (void **) XtMalloc(nmenu_items * sizeof(void*));
	for (int i=0; i<nmenu_items; i++)
	  _data[i] = data[i];
    }
    if (nmenu_items && buttons != NULL)
    {
        _buttons = (Widget *) XtMalloc( _nmenu_items * sizeof(Widget) );
        for (int i=0; i<nmenu_items; i++)
          _buttons[i] = buttons[i];
    }
}


IndexedOptionMenu::~IndexedOptionMenu (void)
{
    if (_w != NULL)
      XtDestroyWidget(_w);
    if (_buttons)
      XtFree((char *) _buttons);
    if (_strings)
      XtFree((char *) _strings);
    if (_data)
      XtFree((char *) _data);
}

void
IndexedOptionMenu::addMenuButtonCallback(
					char* callback_name,
					XtCallbackProc callback,
					XtPointer client_data)
{
    for (int i=0; i<_nmenu_items; i++)
      XtAddCallback(_buttons[i], callback_name, callback, client_data);
}

void*
IndexedOptionMenu::getDataSpec (void)
{
    int	index = getIndexSpec();
    return _data[index];
}

int
IndexedOptionMenu::getIndexSpec (void)
{
    int		data = 0;

    if (_w)
    {
        XtArgVal arg;
        Widget	selected;
        XtVaGetValues(_w, XmNmenuHistory, &arg, NULL);
        selected = (Widget)arg;
        XtVaGetValues(selected, XmNuserData, &arg, NULL);
        data = (int)arg;
    }
    return data;
}

char*
IndexedOptionMenu::getStringSpec(void)
{
    int	index = getIndexSpec();
    return _strings[index];
}

void
IndexedOptionMenu::setSpec (int spec)
{
    XtVaSetValues(_w, XmNmenuHistory, _buttons[spec], NULL );
}

void
IndexedOptionMenu::setSpec (const char *string)
{
    int	spec = stringToIndex(string);
    if (spec >= _nmenu_items) return;
    setSpec(spec);
}

void
IndexedOptionMenu::setSpec (void *data)
{
    int	spec = dataToIndex(data);
    if (spec >= _nmenu_items) return;
    setSpec(spec);
}

int
IndexedOptionMenu::dataToIndex (void *data)
{
    int	index;

    for (index=0; index<_nmenu_items; index++)
	if (! strncmp((char*) _data[index], (char*) data, strlen((char*) data)))
	  return index;
    
    return index;
}

int
IndexedOptionMenu::stringToIndex (const char *string)
{
    int	index;

    for (index=0; index<_nmenu_items; index++)
	if (! strncmp(_strings[index], string, strlen(string)) )
	  return index;

    return index;
}
