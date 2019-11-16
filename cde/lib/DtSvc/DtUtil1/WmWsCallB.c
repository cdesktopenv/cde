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
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:     WmWsCallB.c
 **
 **   RCS:	$XConsortium: WmWsCallB.c /main/6 1996/11/06 18:42:04 drk $
 **
 **   Project:  DT Workspace Manager
 **
 **   Description: Workspace change callback functions
 **
 ** (c) Copyright 1993, 1994 Hewlett-Packard Company
 ** (c) Copyright 1993, 1994 International Business Machines Corp.
 ** (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 ** (c) Copyright 1993, 1994 Novell, Inc.
 **
 ****************************************************************************
 ************************************<+>*************************************/
#include <Tt/tttk.h>
#include <Dt/Service.h>
#include <Dt/Wsm.h> 
#include <Dt/WsmM.h>
#include <Dt/SvcTT.h>
#include "WsmP.h" 
#include "DtSvcLock.h"

/*************************************<->*************************************
 *
 *  DtWsmWsModifiedProc _DtWsmWsChangeHandler (widget, aWS, 
 *						reason, client_data);
 *
 *
 *  Description:
 *  -----------
 *  Internal function called when workspace changes.
 *
 *
 *  Inputs:
 *  ------
 *  widget		- widget (for window where service is registered)
 *  aWS			- Atom for workspace identification
 *  reason		- type of workspace modification
 *  client_data		- pointer to data
 *
 *  Outputs:
 *  --------
 *  Return	- none
 *
 *  Comments:
 *  ---------
 * 
 *************************************<->***********************************/
static void
_DtWsmWsChangeHandler (
	Widget			widget,
	Atom			aWS,
	DtWsmWsReason		reason,
	XtPointer		client_data)
{
    struct _DtWsmCBContext *pCbCtx;

    pCbCtx = (struct _DtWsmCBContext *) client_data;

    /*
     * We only deal with the workspace changes
     */
    if (reason == DtWSM_REASON_CURRENT)
    {
	/*
	 * Call registered callback function.
	 */
	(*(pCbCtx->ws_cb)) (pCbCtx->widget, aWS, 
					pCbCtx->client_data);
    }
    
} /* END OF FUNCTION _DtWsmWsChangeHandler */

/*************************************<->*************************************
 *
 *  DtWsmCBContext * DtWsmAddCurrentWorkspaceCallback (widget, 
 *						 	ws_change, 
 *							client_data)
 *
 *
 *  Description:
 *  -----------
 *  Register a function to be called when the workspace changes.
 *
 *
 *  Inputs:
 *  ------
 *  widget	- widget for this client
 *  ws_change	- function to call when workspace changes
 *  client_data	- additional data to pass back to client when called.
 *
 *  Outputs:
 *  --------
 *  Return	- ptr to callback context data (opaque) 
 *
 *  Comments:
 *  ---------
 *  The callback context data ptr should be saved if you intend to
 *  removed this callback at some point in the future. 
 * 
 *************************************<->***********************************/
DtWsmCBContext 
DtWsmAddCurrentWorkspaceCallback (
	Widget			widget,
	DtWsmWsChangeProc	ws_change,
	XtPointer		client_data)
{
    struct _DtWsmCBContext *pCbCtx;
    _DtSvcWidgetToAppContext(widget);

    _DtSvcAppLock(app);

    /*
     * Allocate data to remember stuff about this callback
     */
    pCbCtx = (struct _DtWsmCBContext * ) 
		XtMalloc (sizeof(struct _DtWsmCBContext));


    /* 
     * Save what we want to remember
     */
    pCbCtx->widget = widget;
    pCbCtx->ws_cb = ws_change;
    pCbCtx->client_data = client_data;

    /*
     * Register interest in the workspace change message
     */
    pCbCtx->nested_context = (XtPointer)
	DtWsmAddWorkspaceModifiedCallback (widget, 
			      (DtWsmWsModifiedProc)_DtWsmWsChangeHandler, 
			      (XtPointer) pCbCtx);

    _DtSvcAppUnlock(app);
    return (pCbCtx);

} /* END OF FUNCTION DtWsmAddCurrentWorkspaceCallback */

/*
----------------------------------------------------------------------
*/

/*************************************<->*************************************
 *
 *  Tt_callback_action _WsModifiedCB (Tt_message m, tt_pattern p)
 *
 *
 *  Description:
 *  -----------
 *  Internal function called when a workspace is modified.
 *
 *
 *  Inputs:
 *  ------
 *  m		- ToolTalk message
 *  p		- ToolTalk pattern
 *
 *  Outputs:
 *  --------
 *  Return	- ToolTalk callback status
 *
 *  Comments:
 *  ---------
 * 
 *************************************<->***********************************/
static Tt_callback_action
_WsModifiedCB (Tt_message m, Tt_pattern p)
{
    struct _DtWsmCBContext *pCbCtx;
    Atom 	aWs;
    DtWsmWsReason	reason;

    Widget		widget;
    DtWsmWsModifiedProc	ws_modify;
    XtPointer		client_data;

    /*
     * user data 0: Widget		widget;
     * user data 1: DtWsmWsModifiedProc	ws_modify;
     * user data 2: XtPointer		client_data;
     */
    widget = (Widget)tt_pattern_user(p, 0);
    ws_modify = (DtWsmWsModifiedProc)tt_pattern_user(p, 1);
    client_data = (XtPointer)tt_pattern_user(p, 2);

    /*
     * 0th arg: screen number, string, not used
     */

    /*
     * Convert the atom to binary.
     */
    aWs = (Atom)strtoul(tt_message_arg_val(m, 1), (char **)NULL, 0);

    /*
     * Convert "reason" of workspace modification
     */
    reason = (DtWsmWsReason)strtoul(tt_message_arg_val(m, 2), (char **)NULL, 0);

    /*
     * Call registered callback function.
     */
    (*ws_modify)(widget, aWs, reason, client_data);
    
    return TT_CALLBACK_PROCESSED;
} /* END OF FUNCTION _DtWsmWsModifyHandler */

/*************************************<->*************************************
 *
 *  DtWsmCBContext * DtWsmAddWorkspaceModifiedCallback (widget, 
 *						 	ws_modify, 
 *							client_data)
 *
 *
 *  Description:
 *  -----------
 *  Register a function to be called when the workspace is modified.
 *
 *
 *  Inputs:
 *  ------
 *  widget	- widget for this client
 *  ws_modify	- function to call when workspace is modified
 *  client_data	- additional data to pass back to client when called.
 *
 *  Outputs:
 *  --------
 *  Return	- ptr to callback context data (opaque) 
 *
 *  Comments:
 *  ---------
 *  The callback context data ptr should be saved if you intend to
 *  removed this callback at some point in the future. 
 * 
 *************************************<->***********************************/
DtWsmCBContext 
DtWsmAddWorkspaceModifiedCallback (
	Widget			widget,
	DtWsmWsModifiedProc	ws_modify,
	XtPointer		client_data)
{
    struct _DtWsmCBContext *pCbCtx;
    int		screen;
    String	sName;
    char	sNum[32];

    Tt_status	status;
    Tt_pattern	pattern;
    char *	sessId;
    _DtSvcWidgetToAppContext(widget);

    _DtSvcAppLock(app);

    /*
     * This function register a ToolTalk pattern for every
     * callback added.
     */
    _DtSvcInitToolTalk(widget);

    pattern = tt_pattern_create();
    status = tt_ptr_error(pattern);
    if (status != TT_OK) {
	_DtSvcAppUnlock(app);
	return NULL;
    }

    if (tt_pattern_scope_add(pattern, TT_SESSION) != TT_OK) {
	_DtSvcAppUnlock(app);
	return NULL;
    }
    if (tt_pattern_category_set(pattern, TT_OBSERVE) != TT_OK) {
	_DtSvcAppUnlock(app);
	return NULL;
    }
    if (tt_pattern_class_add(pattern, TT_NOTICE) != TT_OK) {
	_DtSvcAppUnlock(app);
	return NULL;
    }
    if (tt_pattern_state_add(pattern, TT_SENT) != TT_OK) {
	_DtSvcAppUnlock(app);
	return NULL;
    }
    sessId = tt_default_session();
    if (tt_pattern_session_add(pattern, sessId) != TT_OK) {
	_DtSvcAppUnlock(app);
	return NULL;
    }
    tt_free( sessId );

    screen = XScreenNumberOfScreen(XtScreen(widget));
    sprintf(sNum, "%d", screen);
    sName = _DtWsmSelectionNameForScreen (screen);

    /*
     * Only receive DtWorkspace_Modified notice from the screen
     * we registered with.
     */
    status = tt_pattern_arg_add(pattern, TT_IN, Tttk_string, sNum);
    if (status != TT_OK) {
	_DtSvcAppUnlock(app);
	return NULL;
    }

    if (tt_pattern_op_add(pattern, "DtWorkspace_Modified") != TT_OK) {
	_DtSvcAppUnlock(app);
	return NULL;
    }

    /*
     * Store information needed by the callback in the user data
     * fields of the pattern.
     */
    status = tt_pattern_user_set(pattern, 0, (void *)widget);
    if (status != TT_OK) {
	_DtSvcAppUnlock(app);
	return NULL;
    }
    status = tt_pattern_user_set(pattern, 1, (void *)ws_modify);
    if (status != TT_OK) {
	_DtSvcAppUnlock(app);
	return NULL;
    }
    status = tt_pattern_user_set(pattern, 2, (void *)client_data);
    if (status != TT_OK) {
	_DtSvcAppUnlock(app);
	return NULL;
    }

    /*
     * _WsModifiedCB is the ToolTalk callback which will call
     * the user callback.
     */
    if (tt_pattern_callback_add(pattern, _WsModifiedCB) != TT_OK) {
	_DtSvcAppUnlock(app);
	return NULL;
    }

    if (tt_pattern_register(pattern) != TT_OK) {
	_DtSvcAppUnlock(app);
	return NULL;
    }

    /*
     * Allocate data to remember stuff about this callback
     */
    pCbCtx = (struct _DtWsmCBContext * ) 
		XtMalloc (sizeof(struct _DtWsmCBContext));

    /* 
     * Save what we want to remember
     */
    pCbCtx->pattern = pattern;
    pCbCtx->widget = widget;
    pCbCtx->ws_cb = ws_modify;
    pCbCtx->client_data = client_data;
    pCbCtx->nested_context = NULL;

    XtFree (sName);

    _DtSvcAppUnlock(app);
    return (pCbCtx);

} /* END OF FUNCTION DtWsmAddWorkspaceModifiedCallback */

/*************************************<->*************************************
 *
 *  DtWsmRemoveWorkspaceCallback (pCbCtx)
 *
 *  Description:
 *  -----------
 *  Unregister a workspace callback.
 *
 *
 *  Inputs:
 *  ------
 *  pCbCtx	- ptr to context returned when callback added
 *
 *  Outputs:
 *  --------
 *  Return	- none
 *
 *  Comments:
 *  ---------
 * 
 *************************************<->***********************************/
void
DtWsmRemoveWorkspaceCallback (DtWsmCBContext 	pCbCtx)
{
    /*
     * Is this somewhat valid?
     */
    if (pCbCtx && (pCbCtx->widget != NULL)) {
        _DtSvcWidgetToAppContext(pCbCtx->widget);
	_DtSvcAppLock(app);

	if (pCbCtx->nested_context) {
	    /*
	     * This was a convenience callback for just the workspace
	     * change info.
	     */
	    DtWsmRemoveWorkspaceCallback (
				(DtWsmCBContext) pCbCtx->nested_context);
	}
	else {
	    /*
	     * Unregister interest in this message
	     */
	    tt_pattern_destroy(pCbCtx->pattern);
	}

	/*
	 * Free previously allocated data
	 */
	XtFree((char *) pCbCtx);

	_DtSvcAppUnlock(app);
    }

} /* END OF FUNCTION DtWsmRemoveWorkspaceCallback */

/*************************************<->*************************************
 *
 *  _DtWsmSelectionNameForScreen (scr)
 *
 *  Description:
 *  -----------
 *  Returns a string containing the selection name used for
 *  communication with the workspace manager on this screen
 *
 *
 *  Inputs:
 *  ------
 *  scr		- number of screen
 *
 *  Outputs:
 *  --------
 *  Return	- ptr to string with selection name (free with XtFree)
 *
 *  Comments:
 *  ---------
 *  Assumes the screen number is < 1000.
 * 
 *************************************<->***********************************/
String
_DtWsmSelectionNameForScreen (
	int			scr)
{
    String sName;

    sName = (String) XtMalloc (strlen(DtWSM_TOOL_CLASS) + 4 + 1);

    sprintf ((char *)sName, "%s_%d", DtWSM_TOOL_CLASS, (scr % 1000));

    return (sName);
} /* END OF FUNCTION _DtWsmSelectionNameForScreen */


Tt_callback_action
_DtWsmConsumeReply(
        Tt_message msg,
        Tt_pattern pat )
{
	switch (tt_message_state( msg )) {
	    case TT_HANDLED:
	    case TT_FAILED:
		tttk_message_destroy( msg );
		return TT_CALLBACK_PROCESSED;
	    default:
		return TT_CALLBACK_CONTINUE;
	}
}
