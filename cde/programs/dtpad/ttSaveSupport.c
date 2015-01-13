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
/* $XConsortium: ttSaveSupport.c /main/4 1996/05/08 20:09:07 drk $ */
/**********************************<+>*************************************
***************************************************************************
**
**  File:        ttSaveSupport.c
**
**  Project:     DT dtpad, a memo maker type editor based on the Dt Editor
**               widget.
**
**  Description:
**  -----------
**
**  Provides support for the ToolTalk "Save" message in a manner analogous
**  to "Quit" message support provided by the ttdt_message_accept() function.
**
*******************************************************************
**  (c) Copyright Hewlett-Packard Company, 1990, 1991, 1992, 1993.
**  All rights are
**  reserved.  Copying or other reproduction of this program
**  except for archival purposes is prohibited without prior
**  written consent of Hewlett-Packard Company.
********************************************************************
**
********************************************************************
**  (c) Copyright 1993, 1994 Hewlett-Packard Company
**  (c) Copyright 1993, 1994 International Business Machines Corp.
**  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
**  (c) Copyright 1993, 1994 Novell, Inc.
********************************************************************
**
**
**************************************************************************
**********************************<+>*************************************/
#include "dtpad.h"

/* definitions from tttk/tttkutils.h */
char _TTKeys[7];
#define _TTCBKey		((int)(intptr_t)&_TTKeys[0])
#define _TTClientCBKey		((int)(intptr_t)&_TTKeys[1])
#define _TTClientDataKey	((int)(intptr_t)&_TTKeys[2])
#define _TTDepositPatKey	((int)(intptr_t)&_TTKeys[3])
#define _TTJoinInfoKey		((int)(intptr_t)&_TTKeys[4])
#define _TTContractKey		((int)(intptr_t)&_TTKeys[5])
#define _TTSubContractKey	((int)(intptr_t)&_TTKeys[6])

extern Editor *pPadList;  /*  list of Editor instances - declared in main.c */

/******************************************************************************
 *  TTSaveContractCB - callback (setup in ttdt_message_accept()) to handle
 *	Save(4) requests.  Currently this routine handles one argument:
 *
 *	    messageID - This must be the message ID the originating Edit
 *			request whose contents are to that is to be saved
 *			(or deposited if a buffer is being edited).
 *
 *	This routine is analogous to TTdt_message_acceptCB() which handles
 *	Quit requests.  Patterns to handle Quit requests are automatically
 *	registered  via ttdt_message_accept() whereas patterns to handle Save
 *	requests must be explicitly registered (see TTCreateSavePattern()).
 *
 *	See dtpad(1) for more details.
 *
 ******************************************************************************/
Tt_message
TTSaveContractCB(
	Tt_message	m,
	void *		clientData,
	Tt_message	contract)
{
    Editor *pPad;
    int silent, force;
    Tt_status status;
    char *messageID;
    int mark = tt_mark();

    char *opString = tt_message_op(m);
    Tttk_op op = tttk_string_op(opString);

    if (op == TTDT_SAVE) {

	/* Search the list of Editor instances for one with a TT media
	 * msg id matching the originating media msg id supplied as the
	 * 'messageID' (1st) argument of the TTDT_SAVE msg */
	if (messageID = tt_message_arg_val(m, 0)) {
	    for (pPad = pPadList; pPad != (Editor *)NULL; pPad = pPad->pNextPad) {
		if (pPad->inUse == True && pPad->ttEditReq.msg_id &&
		  strcmp(pPad->ttEditReq.msg_id, messageID) == 0) {
		    pPad->ttSaveReq.contract = m;	/* process Save msg */
		    pPad->ttSaveReq.messageID = strdup(messageID);
		    FileSaveCB((Widget) NULL, (caddr_t) pPad, (caddr_t) NULL);
		    tt_release(mark);
		    return((Tt_message) 0);	/* handling Save */
		}
	    }
	    /* no Edit window matching the messageID */
	    tttk_message_fail(m,
			TT_DESKTOP_ECANCELED,		/* status */
			(char *) NULL,			/* status str */
			1);				/* destroy request */

	} else {	/* no messageID - which is required */
	    tttk_message_fail(m,
			TT_DESKTOP_ECANCELED,		/* status */
			(char *) NULL,			/* status str */
			1);				/* destroy request */
	}

	/* We're handling all Save requests.  If the request has not been
	 * failed already, pPad->ttSaveReq.contract will be non-NULL and
	 * the Save message will be responded to or failed later. */
	tt_release(mark);
	return((Tt_message) 0);	/* handling Quit */

    } else {	/* unsupported message type */

	tt_release(mark);
	return(m);	/* didn't handle message */

    }

}



/******************************************************************************
 * _TT_pattern_contract - obtains the contracting message from the pattern
 ******************************************************************************/
Tt_message
_TT_pattern_contract(
	Tt_pattern	pat)
{
    void *contract = tt_pattern_user(pat, _TTContractKey);
    Tt_status status = tt_ptr_error(contract);
    if (status != TT_OK) {
	return 0;
    }
    return (Tt_message) contract;
}



/******************************************************************************
 * _TTpatternCB - Retrieves the client callback and callback arguments from
 *	the pattern and calls it (ala tttkpattern.C:_ttDtPatternCB()).
 ******************************************************************************/
static Tt_callback_action
_TTpatternCB(
	Tt_message	msg,
	Tt_pattern	pat)
{
    Ttdt_contract_cb	clientCB;
    void *		clientData;
    Tt_status 		status;

    /* ----> Obtain client callback */
    clientCB = (Ttdt_contract_cb) tt_pattern_user(pat, _TTClientCBKey);
    status = tt_ptr_error(clientCB);
    if (status != TT_OK) {
	return TT_CALLBACK_PROCESSED;
    }
    if (clientCB == 0) {
	return TT_CALLBACK_CONTINUE;
    }

    /* ----> Obtain client data */
    clientData = tt_pattern_user(pat, _TTClientDataKey);
    status = tt_ptr_error(clientData);
    if (status != TT_OK) {
	return TT_CALLBACK_PROCESSED;
    }

    /* -----> Call client callback */
    msg = (*clientCB)(msg, clientData, _TT_pattern_contract(pat));

    status = tt_ptr_error(msg);
    if ((status != TT_OK) || (msg == 0)) {
	return TT_CALLBACK_PROCESSED;
    } else {
	return TT_CALLBACK_CONTINUE;
    }
}


/******************************************************************************
 * _TTCreatePattern - creates a pattern for a desktop message
 *	(ala cdesrc/lib/tt/lib/tttk/tttkpattern.C:_ttDtPatternCreate())
 ******************************************************************************/
Tt_pattern
_TTCreatePattern(
	Tt_category		category,
	Tt_scope		theScope,
	int			addDefaultSess,
	const char *		file,
	const char *		op_string,
	Ttdt_contract_cb	clientCB,
	void *			clientData)
{

    Tt_pattern pat = tt_pattern_create();
    Tt_status status = tt_ptr_error(pat);
    if (status != TT_OK) {
	return pat;
    }

    status = tt_pattern_category_set(pat, category);
    if (status != TT_OK) {
	tt_pattern_destroy(pat);
	return (Tt_pattern)tt_error_pointer(status);
    }

    /* we only observe notices and only handle requests */
    if (category == TT_HANDLE) {
	status = tt_pattern_class_add(pat, TT_REQUEST);
    }
    if (category == TT_OBSERVE) {
	status = tt_pattern_class_add(pat, TT_NOTICE);
    }
    if (status != TT_OK) {
	tt_pattern_destroy(pat);
	return (Tt_pattern)tt_error_pointer(status);
    }

    status = tt_pattern_scope_add(pat, theScope);
    if (status != TT_OK) {
	tt_pattern_destroy(pat);
	return (Tt_pattern)tt_error_pointer(status);
    }

    if (addDefaultSess) {
	char *sess = tt_default_session();
	status = tt_pattern_session_add(pat, sess);
	tt_free(sess);
	if (status != TT_OK) {
	    tt_pattern_destroy(pat);
	    return (Tt_pattern)tt_error_pointer(status);
	}
    }

    if (file != 0) {
	status = tt_pattern_file_add(pat, file);
	if (status != TT_OK) {
	    tt_pattern_destroy(pat);
	    return (Tt_pattern)tt_error_pointer(status);
	}
    }

    if (op_string) {
	status = tt_pattern_op_add(pat, op_string);
	if (status != TT_OK) {
	    tt_pattern_destroy(pat);
	    return (Tt_pattern)tt_error_pointer(status);
	}
    }

    /* -----> Add the general purpose pattern callback (as well as the
     *        client callback and client data) as fields to the pattern.
     *        When the general purpose pattern callback is called, it
     *        obtains the client callback and client data from the pattern
     *        and calls the client callback with the appropriate arguments. */
    status = tt_pattern_callback_add( pat, _TTpatternCB );
    if (status != TT_OK) {
	tt_pattern_destroy(pat);
	return (Tt_pattern)tt_error_pointer(status);
    }
    status = tt_pattern_user_set(pat, _TTClientCBKey, (void *) clientCB);
    if (status != TT_OK) {
	tt_pattern_destroy(pat);
	return (Tt_pattern)tt_error_pointer(status);
    }
    status = tt_pattern_user_set(pat, _TTClientDataKey, clientData);
    if (status != TT_OK) {
	tt_pattern_destroy(pat);
	return (Tt_pattern)tt_error_pointer(status);
    }

    return pat;
}



/******************************************************************************
 * _TTmessage_id - Returns sender-set message id if any, otherwise the
 *	tt_message_id() (ala lib/tt/lib/tttk/tttkmessage.C:_tttk_message_id())
 ******************************************************************************/
char *
_TTmessage_id(
	Tt_message	msg)
{
    int i, miss, numArgs = tt_message_args_count( msg );
    Tt_status status = tt_int_error( numArgs );
    char *msgID;

    if (status != TT_OK) {
	return 0;
    }

    for (i = 0; i < numArgs; i++) {
	char *type = tt_message_arg_type(msg, i);
	status = tt_ptr_error(type);
	if (status != TT_OK) {
	    return 0;
	}
	if (type == 0) {
	    continue;
	}
	miss = strcmp(type, Tttk_message_id);
	tt_free(type);
	if (miss) {
	    continue;
	}
	msgID = tt_message_arg_val(msg, i);
	status = tt_ptr_error(type);
	if (status != TT_OK) {
	    return 0;
	}
	return msgID;
    }

    return tt_message_id(msg);
}



/******************************************************************************
 * TTCreateSavePattern - Creates and (optionally) registers a pattern to handle
 *	Save requests (ala cdesrc/lib/tt/lib/tttk/ttdesktop.C:_ttdt_pat() and
 *	_ttDesktopPatternFinish())
 ******************************************************************************/
Tt_pattern
TTCreateSavePattern(
	Tt_message		contract,
	Ttdt_contract_cb	clientCB,
	void *			clientData,
	int			register_it)
{
    char *msgID;

    /* -----> Create pattern */
    Tt_pattern pat = _TTCreatePattern(
			TT_HANDLE,	/* catagory */
			TT_SESSION,	/* scope */
			1,		/* add default session to pat */
			0,		/* don't add file to pat */
			"Save",		/* desktop message (op) type */
			clientCB,
			clientData);

    Tt_status status = tt_ptr_error( pat );
    if (status != TT_OK) {
		return pat;
    }

    /* -----> Add message id and contracting message to pattern */
    if (contract != 0) {
	char *msgID = _TTmessage_id(contract);
	status = tt_pattern_arg_add(pat, TT_IN, Tttk_message_id, msgID);
	tt_free(msgID);
	if (status != TT_OK) {
	    tt_pattern_destroy(pat);
	    return (Tt_pattern) tt_error_pointer(status);
	}
	tt_pattern_user_set(pat, _TTContractKey, contract);
    }

    /* -----> Register pattern */
    if (register_it) {
	status = tt_pattern_register(pat);
	if (status != TT_OK) {
	    tt_pattern_destroy(pat);
	    return (Tt_pattern) tt_error_pointer(status);
	}
    }

    return pat;
}


/************************************************************************
 * TTresetSaveArgs - resets TTDT_SAVE argments set in TTSaveContractCB()
 ************************************************************************/
void
TTresetSaveArgs(
	Editor *pPad)
{
    if (!pPad->ttSaveReq.contract)
	return;
    pPad->ttSaveReq.contract = NULL;
    if (pPad->ttSaveReq.messageID != (char *) NULL) {
	XtFree(pPad->ttSaveReq.messageID);
	pPad->ttSaveReq.messageID = (char *) NULL;
    }
}


/************************************************************************
 * TTfailPendingSave - fails any pending Save request
 ************************************************************************/
void
TTfailPendingSave(
	Editor *pPad)
{
    if (pPad->ttSaveReq.contract) {
	tttk_message_fail(pPad->ttSaveReq.contract,	/* original Save req */
			TT_DESKTOP_ECANCELED,		/* status */
			(char *) NULL,			/* status string */
			1);				/* destroy request */
	TTresetSaveArgs(pPad);
    }
}
