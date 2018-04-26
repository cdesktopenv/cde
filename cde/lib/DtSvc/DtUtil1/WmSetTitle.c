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
/* $XConsortium: WmSetTitle.c /main/6 1996/05/20 16:08:13 drk $
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
 **   File:     WmSetTitle.c
 **
 **   Project:  DT Workspace Manager
 **
 **   Description: Set the title for a workspace
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


/*************************************<->*************************************
 *
 *  int DtWsmSetWorkspaceTitle (widget, aWs, pchNewName)
 *
 *
 *  Description:
 *  -----------
 *  Rename a workspace
 *
 *
 *  Inputs:
 *  ------
 *  widget	- a widget
 *  aWs  	- atom of workspace 
 *  pchNewName	- new name for the workspace
 *
 *  Outputs:
 *  --------
 *  Return	- Success if communication to workspace manager
 *		  was successful.  Note that this is *not* the same
 *                value _DtWsmSetWorkspaceTitle returns.
 *
 *  Comments:
 *  ---------
 *  public interface to _DtWsmSetWorkspaceTitle()
 * 
 *************************************<->***********************************/
int
DtWsmSetWorkspaceTitle (
	Widget widget,
        Atom aWs,
	char * pchNewName)
{
  int rcode = _DtWsmSetWorkspaceTitle(widget, aWs, pchNewName);

  if (rcode == DT_SVC_SUCCESS)
    return Success;
  else /* rcode == DT_SVC_FAIL || rcode == dtmsg_FAIL */
    return rcode;
}


/*************************************<->*************************************
 *
 *  int _DtWsmSetWorkspaceTitle (widget, aWs, pchNewName)
 *
 *
 *  Description:
 *  -----------
 *  Rename a workspace
 *
 *
 *  Inputs:
 *  ------
 *  widget	- a widget
 *  aWs  	- atom of workspace 
 *  pchNewName	- new name for the workspace
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
_DtWsmSetWorkspaceTitle (
	Widget widget,
        Atom aWs,
	char * pchNewName)
{
    Tt_status	status;
    Tt_message	msg;
    char	pch[40];

    sprintf (pch, "0x%lx", aWs);

    msg = tttk_message_create(0, TT_REQUEST, TT_SESSION, 0,
	    "DtWorkspace_Title_Set", 0);
    status = tt_ptr_error(msg);
    if (status != TT_OK) {
	return DT_SVC_FAIL;
    }
    status = tt_message_arg_add(msg, TT_IN, "integer", NULL);
    if (status != TT_OK) {
	return DT_SVC_FAIL;
    }
    status = tt_message_arg_ival_set(msg, 0,
	    XScreenNumberOfScreen(XtScreen(widget)) % 1000);
    if (status != TT_OK) {
	return DT_SVC_FAIL;
    }
    status = tt_message_arg_add(msg, TT_IN, Tttk_string, pch);
    if (status != TT_OK) {
	return DT_SVC_FAIL;
    }
    status = tt_message_arg_add(msg, TT_IN, Tttk_string, pchNewName);
    if (status != TT_OK) {
	return DT_SVC_FAIL;
    }
    status = tt_message_callback_add(msg, _DtWsmConsumeReply);
    if (status != TT_OK) {
	return dtmsg_FAIL;
    }
    status = tt_message_send(msg);
    if (status != TT_OK) {
	return DT_SVC_FAIL;
    }

    return DT_SVC_SUCCESS;

} /* END OF FUNCTION DtWsmSetWorkspaceTitle */
