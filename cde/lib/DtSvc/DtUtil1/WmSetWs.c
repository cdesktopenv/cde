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
/* $XConsortium: WmSetWs.c /main/6 1996/06/21 17:24:17 ageorge $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1993,1994,1996 Hewlett-Packard Company.
 * (c) Copyright 1993,1994,1996 International Business Machines Corp.
 * (c) Copyright 1993,1994,1996 Sun Microsystems, Inc.
 * (c) Copyright 1993,1994,1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:     WmSetWs.c
 **
 **   Project:  DT Workspace Manager
 **
 **   Description: Set the current workspace
 **
 ****************************************************************************
 ************************************<+>*************************************/
#include <stdio.h>
#include <X11/Intrinsic.h>
#include <Dt/Wsm.h> 
#include <Dt/WsmP.h> 
#include <Dt/WsmM.h>
#include <Dt/Service.h>
#include <Xm/Xm.h>
#include <Xm/AtomMgr.h>
#include <Tt/tttk.h>
#include "DtSvcLock.h"

/*************************************<->*************************************
 *
 *  int DtWsmSetCurrentWorkspace (widget, aWs)
 *
 *
 *  Description:
 *  -----------
 *  Set the current workspace
 *
 *
 *  Inputs:
 *  ------
 *  screen_num	- screen number, integer
 *  aWs  	- atom of workspace 
 *
 *  Outputs:
 *  --------
 *  Return	- 1 (*not* Success) if communication to workspace manager
 *		  was successful.
 *
 *  Comments:
 *  ---------
 *  The odd-ball successful return value is a CDE 1.0 bug being
 *  preserved for backward compatibility.
 * 
 *************************************<->***********************************/
int
DtWsmSetCurrentWorkspace (
	Widget widget,
        Atom aWs)
{
    String	pStr[2];
    char	pch[40];
    Tt_message	msg;
    Tt_status	status;
    _DtSvcWidgetToAppContext(widget);

    _DtSvcAppLock(app);
    sprintf (pch, "0x%lx", aWs);
    pStr[0] = (String) &pch[0];
    pStr[1] = NULL;
    msg = tttk_message_create(0, TT_REQUEST, TT_SESSION, 0,
	    "DtWorkspace_SetCurrent", 0);
    status = tt_ptr_error(msg);
    if (status != TT_OK) {
	_DtSvcAppUnlock(app);
	return dtmsg_FAIL;
    }
    status = tt_message_arg_add(msg, TT_IN, "integer", NULL);
    if (status != TT_OK) {
	_DtSvcAppUnlock(app);
	return dtmsg_FAIL;
    }
    status = tt_message_arg_ival_set(msg, 0,
	    XScreenNumberOfScreen(XtScreen(widget)) % 1000);
    if (status != TT_OK) {
	_DtSvcAppUnlock(app);
	return dtmsg_FAIL;
    }
    status = tt_message_arg_add(msg, TT_IN, Tttk_string, pStr[0]);
    if (status != TT_OK) {
	_DtSvcAppUnlock(app);
	return dtmsg_FAIL;
    }
    status = tt_message_callback_add(msg, _DtWsmConsumeReply);
    if (status != TT_OK) {
	_DtSvcAppUnlock(app);
	return dtmsg_FAIL;
    }
    status = tt_message_send(msg);
    if (status != TT_OK) {
	_DtSvcAppUnlock(app);
	return dtmsg_FAIL;
    }

    _DtSvcAppUnlock(app);
    return (dtmsg_SUCCESS);

} /* END OF FUNCTION DtWsmSetCurrentWorkspace */
