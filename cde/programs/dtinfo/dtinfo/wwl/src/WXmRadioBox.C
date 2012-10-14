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
/* $XConsortium: WXmRadioBox.cc /main/3 1996/06/11 17:03:50 cde-hal $ */

#ifndef STREAMH
#include <sstream>
using namespace std;
#endif

#ifndef _WXmRadioBox_h
#include <WWL/WXmRadioBox.h>
#endif

WXmToggleButtonGadget
WXmRadioBox :: AddToggleButton (char* name, XtCallbackProc proc, caddr_t closure, ArgList args, Cardinal card)
{
	WXmToggleButtonGadget bg(widget, name, args, card);
	if (proc)
		XtAddCallback (Widget(bg), XmNvalueChangedCallback,
			       proc, closure);

	return bg;
}


WXmToggleButtonGadget
WXmRadioBox :: AddToggleButton (char* name, WWL* obj, WWL_FUN fun, ArgList args, Cardinal card)
{
	WXmToggleButtonGadget bg(widget, name, args, card);
	bg.SetValueChangedCallback (obj, fun);

	return bg;
}


