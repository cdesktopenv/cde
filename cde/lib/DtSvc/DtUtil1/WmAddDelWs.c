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
/* $XConsortium: WmAddDelWs.c /main/7 1996/05/22 12:01:11 drk $
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
 **   File:     WmAddDelWs.c
 **
 **   Project:  DT Workspace Manager
 **
 **   Description: Add/Delete a workspace
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
 *  int DtWsmDeleteWorkspace (widget, aWs)
 *
 *
 *  Description:
 *  -----------
 *  Delete a workspace
 *
 *
 *  Inputs:
 *  ------
 *  widget	- a widget (with a window!)
 *  aWs  	- atom of workspace to delete
 *
 *  Outputs:
 *  --------
 *  Return	- Success if communication to workspace manager
 *		  was successful.  Note that this is *not* the same
 *		  value _DtWsmDeleteWorkspace returns.
 *
 *  Comments:
 *  ---------
 *  public interface to _DtWsmDeleteWorkspace()
 * 
 *************************************<->***********************************/
int
DtWsmDeleteWorkspace (
	Widget widget,
        Atom aWs)
{
  int rcode = _DtWsmDeleteWorkspace(widget, aWs);

  if (rcode == dtmsg_SUCCESS)
    return Success;
  else /* rcode == dtmsg_FAIL */
    return rcode;
}


/*************************************<->*************************************
 *
 *  int _DtWsmDeleteWorkspace (widget, aWs)
 *
 *
 *  Description:
 *  -----------
 *  Delete a workspace
 *
 *
 *  Inputs:
 *  ------
 *  widget	- a widget (with a window!)
 *  aWs  	- atom of workspace to delete
 *
 *  Outputs:
 *  --------
 *  Return	- 1 (*not* Success) if communication to workspace manager
 *		  was successful.
 *
 *  Comments:
 *  ---------
 * 
 *************************************<->***********************************/
int
_DtWsmDeleteWorkspace (
	Widget widget,
        Atom aWs)
{
    char	pch[40];
    Tt_message	msg;
    Tt_status	status;

    sprintf (pch, "0x%lx", aWs);
    msg = tttk_message_create(0, TT_REQUEST, TT_SESSION, 0,
	    "DtWorkspace_Delete", 0);
    status = tt_ptr_error(msg);
    if (status != TT_OK) {
	return dtmsg_FAIL;
    }
    status = tt_message_arg_add(msg, TT_IN, Tttk_string, pch);
    if (status != TT_OK) {
	return dtmsg_FAIL;
    }
    status = tt_message_callback_add(msg, _DtWsmConsumeReply);
    if (status != TT_OK) {
	return dtmsg_FAIL;
    }
    status = tt_message_send(msg);
    if (status != TT_OK) {
	return dtmsg_FAIL;
    }

    return (dtmsg_SUCCESS);

} /* END OF FUNCTION _DtWsmDeleteWorkspace */


/*************************************<->*************************************
 *
 *  int DtWsmAddWorkspace (widget, pchTitle)
 *
 *
 *  Description:
 *  -----------
 *  Add a workspace
 *
 *
 *  Inputs:
 *  ------
 *  widget	- a widget (with a window!)
 *  pchTitle	- user-visible title of the workspace
 *
 *  Outputs:
 *  --------
 *  Return	- Success if communication to workspace manager
 *		  was successful.  Note that this is *not* the
 *		  same value _DtWsmCreateWorkspace returns.
 *
 *  Comments:
 *  ---------
 *  public interface to _DtWsmCreateWorkspace()
 * 
 *************************************<->***********************************/
int
DtWsmAddWorkspace (Widget widget, char * pchTitle)
{
  int rcode = _DtWsmCreateWorkspace(widget, pchTitle);

  if (rcode == dtmsg_SUCCESS)
    return Success;
  else /* rcode == dtmsg_FAIL */
    return rcode;
}


/*************************************<->*************************************
 *
 *  int _DtWsmCreateWorkspace (widget, pchTitle)
 *
 *
 *  Description:
 *  -----------
 *  Add a workspace
 *
 *
 *  Inputs:
 *  ------
 *  widget	- a widget (with a window!)
 *  pchTitle	- user-visible title of the workspace
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
_DtWsmCreateWorkspace (Widget widget, char * pchTitle)
{
    Tt_message	msg;
    Tt_status	status;

    msg = tttk_message_create(0, TT_REQUEST, TT_SESSION, 0,
	    "DtWorkspace_Add", 0);
    status = tt_ptr_error(msg);
    if (status != TT_OK) {
	return dtmsg_FAIL;
    }
    status = tt_message_arg_add(msg, TT_IN, Tttk_string, pchTitle);
    if (status != TT_OK) {
	return dtmsg_FAIL;
    }
    status = tt_message_callback_add(msg, _DtWsmConsumeReply);
    if (status != TT_OK) {
	return dtmsg_FAIL;
    }
    status = tt_message_send(msg);
    if (status != TT_OK) {
	return dtmsg_FAIL;
    }

    return (dtmsg_SUCCESS);

} /* END OF FUNCTION _DtWsmCreateWorkspace */
