
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

