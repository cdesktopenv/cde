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

/*
 *  $XConsortium: ClientEventMgrX.cc /main/4 1996/06/11 16:24:09 cde-hal $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 *
 */

#define C_ClientEventHandler
#define C_ClientEventMgr
#define L_Managers

#define C_WindowSystem
#define L_Other

#include "Prelude.h"


void
ClientEventMgr::register_handler(ClientEventHandler *client, Widget widget)
{
  XtAddEventHandler(widget, None, True,
		    (XtEventHandler)dispatch, client);
}

void
ClientEventMgr::dispatch(Widget widget, XtPointer client_data,
			 XEvent *event, Boolean *bvalue)
{
  ((ClientEventHandler*)client_data)->handle_event(widget, event, bvalue);
}

LONG_LIVED_CC(ClientEventMgr,client_event_manager);

