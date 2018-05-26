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
/* $TOG: ttMsgSupport.c /main/7 1999/09/15 14:23:02 mgreess $ */
/**********************************<+>*************************************
***************************************************************************
**
**  File:        ttMsgSupport.c
**
**  Project:     DT dtpad, a memo maker type editor based on the Dt Editor
**               widget.
**
**  Description:
**  -----------
**
**  Provides support for the Tool Talk "Desktop" and "Document and Media
**  Exchange" message sets.
**  
**  XXX - Need to use message catalogs for error messages and not write to
**	  stdout or stderr.
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
#include <dirent.h>
#include <Dt/DtpadM.h>

#define TIMEOUT_FACTOR	1000

XtInputId ProcessToolTalkInputId = 0;

extern Editor *pPadList;  /* list of Editor instances - declared in main.c */
extern char *initialServerGeometry;	/* declared in main.c */
extern Tt_message TTSaveContractCB(	/* declared in ttSaveSupport.c */
	Tt_message		m,
	void *			clientData,
	Tt_message		contract);
extern Tt_pattern TTCreateSavePattern(	/* declared in ttSaveSupport.c */
	Tt_message		contract,
	Ttdt_contract_cb	clientCB,
	void *			clientData,
	int			register_it);


/************************************************************************
 * TTdisplayError - posts a ToolTalk error dialog containing the specified
 *	error message followed by the related ToolTalk status message.
 ************************************************************************/
void
TTdisplayError(
	Editor *pPad,
	char *errorMsg,
	Tt_status status)
{
    char *statusMsg = tt_status_message(status);
    char buf[1024];

    if (errorMsg != (char *) NULL) {
	sprintf(buf, "%s\n(%d: %s)", errorMsg, status, statusMsg);
    } else {
	sprintf(buf, "(%d: %s)", status, statusMsg);
    }
    tt_free(statusMsg);
    Warning(pPad, buf, XmDIALOG_ERROR);
}


/******************************************************************************
 *  TTdt_session_joinCB -  Callback passed to ttdt_session_join() to handle
 *	standard "Desktop" messages.
 *
 *  In our case, this only deals with the standard Desktop Quit message with
 *  signature:
 *
 *	Quit(in boolean silent, in boolean force, in messageID operation2Quit)
 *
 *  XXX - This routine was setup by SUN but currently does nothing since:
 *  Requests to quit a specific Media Edit/Display request are handled in 
 *  TTdt_message_acceptCB() (which is registered in TTmedia_ptype_declareCB()
 *  when the Media Edit/Display request is first accepted).
 ******************************************************************************/
Tt_message
TTdt_session_joinCB(
	Tt_message	m,
	void *		client_data,
	Tt_message	contract)
{
     /* should never get here, if we do, notify sender with a fail */
     tttk_message_fail(m,
                       TT_DESKTOP_ENOTSUP,           /* status */
                       (char *) NULL,                  /* status str */
                       1);                             /* destroy request */

    return((Tt_message) 0);     /* we handled this message */

}


/******************************************************************************
 *  TTdt_message_acceptCB - callback (setup in ttdt_message_accept()) to handle
 *	TT_HANDLER-addressed messages.  Currently this routine handles:
 *
 *	Quit(4) request which can contain the following arguments:
 *
 *	    silent	   - controls whether or not the user is notified
 *			     when there are unsaved changes
 *	    force	   - controls whether or not the Quit is aborted
 *			     when there are unsaved changes
 *	    operation2Quit - controls whether all edit window(s) are closed or
 *			     whether a specific edit window is closed	
 *
 *	This routine is analogous to TTSaveContractCB() which handles
 *	Save requests.  Patterns to handle Quit requests are automatically
 *	registered  via ttdt_message_accept() whereas patterns to handle Save
 *	requests must be explicitly registered (see TTCreateSavePattern()).
 *
 *	See dtpad(1) for more details.
 *
 ******************************************************************************/
Tt_message
TTdt_message_acceptCB(
	Tt_message	m,
	void *		client_data,
	Tt_message	contract)
{
    Editor *pPad;
    int silent, force;
    Tt_status status;
    char *operation2Quit;
    int mark = tt_mark();

    char *opString = tt_message_op(m);
    Tttk_op op = tttk_string_op(opString);

    if (op == TTDT_QUIT) {

	/* Search the list of Editor instances for one with a TT media
	 * msg id matching the originating media msg id supplied as the
	 * 'operation2Quit' (3rd) argument of the TTDT_QUIT msg */
	if (operation2Quit = tt_message_arg_val(m, 2)) {
	    for (pPad = pPadList; pPad != (Editor *)NULL; pPad = pPad->pNextPad) {
		if (pPad->inUse == True && pPad->ttEditReq.msg_id &&
		  strcmp(pPad->ttEditReq.msg_id, operation2Quit) == 0) {
		    pPad->ttQuitReq.contract = m;	/* process Quit msg */
		    status = tt_message_arg_ival(m, 0, &silent);
		    status = tt_message_arg_ival(m, 1, &force);
		    pPad->ttQuitReq.silent = (Boolean) silent;
		    pPad->ttQuitReq.force = (Boolean) force;
		    pPad->ttQuitReq.operation2Quit = strdup(operation2Quit);
		    FileExitCB( (Widget) NULL, (caddr_t) pPad, (caddr_t) NULL);
		    tt_release(mark);
		    return((Tt_message) 0);	/* handling Quit */
		}
	    }
	    /* no Edit window matching the operation2Quit */
	    tttk_message_fail(m,
			TT_DESKTOP_ENOMSG,		/* status */
			(char *) NULL,			/* status str */
			1);				/* destroy request */

	} else {	/* no operation2Quit - quit all Edit windows */
	    /* XXX - not currently processing this type of Quit */
	    tttk_message_fail(m,
			TT_DESKTOP_ENOTSUP,		/* status */
			(char *) NULL,			/* status str */
			1);				/* destroy request */
	}

	/* We're handling all Quit requests.  If the request has not been
	 * failed already, pPad->ttQuitReq.contract will be non-NULL and
	 * the Quit message will be responded to or failed later. */
	tt_release(mark);
	return((Tt_message) 0);	/* handling Quit */

    } else {	/* unsupported message type */

	tt_release(mark);
	return(m);	/* didn't handle message */

    }

}



/******************************************************************************
 *  TTmedia_ptype_declareCB - callback (set up in ttmedia_ptype_declare()) to
 *	handle the standard ToolTalk Instantiate/Edit/Display media messages
 *	from a "requestor" dtpad or from other applications.
 * 
 *  It sets up TTdt_message_acceptCB() when it "accepts" the message which
 *  handles Quit messages relating to the accepted Instantiate/Edit/Display
 *  message.
 *
 *  Returns: 0 if the message is handled; otherwise, returns Tt_message.
 ******************************************************************************/
Tt_message
TTmedia_ptype_declareCB(
	Tt_message	m,
	void *		client_data,
	Tttk_op 	op,
	Tt_status	diag,
	unsigned char *	contents,
	int		len,
	char *		file,
	char *		docname)
{
    Editor *pPad;
    char *filename;
    Tt_message TTdt_message_acceptCB(), incoming;
    Tt_pattern *ttrc;
    DtEditorContentRec  contentRec;
    DtEditorErrorCode   errorCode;
    Boolean foundPad = False;
    char *context = (char *)NULL;
    Boolean  isCurrentlyVisible = False;
    int mark = tt_mark();
    char *localPath = tt_message_file(m);

    if (diag != TT_OK
       && !(op == TTME_INSTANTIATE && diag == TT_DESKTOP_ENODATA)) {	/* TTT - workaround for tttk bug */
        fprintf(stderr,
                "Ignored Media request with ToolTalk detected error: %s\n",
                tt_status_message(diag));
	return(m);
    }
    if (op != TTME_EDIT && op != TTME_DISPLAY && op != TTME_INSTANTIATE) {
	return(m);	/* Message not handled by this routine. */
    }

    /* if editing a file, make sure we can read it first,
     * if not, fail the request immediately
     */
    if (file) {
      char *pathEnd;

      if (MbStrrchr(localPath, '/') != (char *)NULL) {
	char *dir; DIR *pDir;

	dir = (char *) XtMalloc(strlen(localPath) + 1);
	strcpy(dir, localPath);
	pathEnd = MbStrrchr(dir, '/');
	if (pathEnd == dir) {
	  /* file is in the root directory
	   */
	  pathEnd++;
	}
	*pathEnd = (char)'\0';
	if ((pDir = opendir(dir)) != (DIR *)NULL) {
	  closedir(pDir);
	  XtFree(dir);
	} else {
	  tt_free(localPath);
	  localPath = (char *) NULL;
	  XtFree(dir);
	  tttk_message_fail(m,
			TT_DESKTOP_ENOENT,		/* status */
			(char *) NULL,			/* status str */
			1);				/* destroy request */
	  tt_release(mark);
	  return((Tt_message) 0);	/* Message handled by this routine. */
	}
      }
    }

    /*
     * Create an Editor "instance" and manage/realize it.
     * This involves creating its GUI components, setting it resources,
     * mapping its windows and, if specified, loading the file to be edited.
     */
    if (foundPad == False)
      foundPad = FindOrCreatePad(&pPad);    /* returns new Editor in pPad */

    /* 
     * If this message is from a dtpad requestor then replace selective
     * server resources with requestor resources shipped to the dtpad server
     * in the message context fields
     */
    if (tt_message_contexts_count(m)) {

      /*----> a session file, want to restore a session */
      if (*(context = tt_message_context_val(m, "SESSION"))) {
         pPad->xrdb.session = strdup(context);
      }
      else {
         /* -----> non text editor specific resource  */
         if (*(context = tt_message_context_val(m, "GEOMETRY"))) {
	   pPad->geometry = strdup(context);
         }
   
         /* -----> basic options */
         if (context = tt_message_context_val(m, "STATUSLINE")) {
	   if (*context == 't')
	     pPad->xrdb.statusLine = True;
	   else
	     pPad->xrdb.statusLine = False;
         }
   
         if (context = tt_message_context_val(m, "WINDOWWORDWRAP")) {
	   if (*context == 't')
	     pPad->xrdb.wordWrap = True;
	   else
	     pPad->xrdb.wordWrap = False;
         }
   
         if (context = tt_message_context_val(m, "OVERSTRIKE")) {
	   if (*context == 't')
	     pPad->xrdb.overstrike = True;
	   else
	     pPad->xrdb.overstrike = False;
         }
   
         if (context = tt_message_context_val(m, "SAVEONCLOSE")) {
	   if (*context == 't')
	     pPad->xrdb.saveOnClose = True;
	   else
	     pPad->xrdb.saveOnClose = False;
         }
   
         if (context = tt_message_context_val(m, "MISSINGFILEWARNING")) {
	   if (*context == 't')
	     pPad->xrdb.missingFileWarning = True;
	   else
	     pPad->xrdb.missingFileWarning = False;
         }
   
         if (context = tt_message_context_val(m, "NOREADONLYWARNING")) {
	   if (*context == 't')
	     pPad->xrdb.readOnlyWarning = True;
	   else
	     pPad->xrdb.readOnlyWarning = False;
         }
   
         if (context = tt_message_context_val(m, "NONAMECHANGE")) {
	   if (*context == 't')
	     pPad->xrdb.nameChange = True;
	   else
	     pPad->xrdb.nameChange = False;
         }
   
         if (context = tt_message_context_val(m, "VIEWONLY")) {
	   if (*context == 't')
	     pPad->xrdb.viewOnly = True;
	   else
	     pPad->xrdb.viewOnly = False;
         }
   
         if  (context = tt_message_context_val(m, "WORKSPACELIST")) {
	   pPad->xrdb.workspaceList = strdup(context);
         }
   
         /* -----> client/server control options */
         if (*(context = tt_message_context_val(m, "BLOCKING"))) {
	   pPad->xrdb.blocking = True;
	   pPad->blockChannel = strdup(context);
         }
      }
    }

    pPad->ttEditReq.contract = m;
    pPad->ttEditReq.msg_id = strdup(tt_message_id(m));
    pPad->ttEditReq.op = op;

    if (op == TTME_INSTANTIATE) {
	pPad->ttEditReq.contents = False;
	pPad->ttEditReq.returnBufContents = False;

    } else {	/* TTME_EDIT or TTME_DISPLAY */

	if (file) {
	    /* the mediaType of the first arg (contents) applies to either
	     * contents or a file */
	    pPad->ttEditReq.vtype = strdup(tt_message_arg_type(m, 0));
	    pPad->ttEditReq.contents = False;	/* editing a file */
	    pPad->ttEditReq.fileName = strdup(localPath); 
	    pPad->fileStuff.fileName = XtNewString(pPad->ttEditReq.fileName);
	} else {
	    pPad->ttEditReq.contents = True;	/* editing a buffer */
	    pPad->xrdb.nameChange = False;  /* disallow switching to another 
                                               file */
	    pPad->ttEditReq.returnBufContents = True;
        }
	if (docname) {
	    pPad->ttEditReq.docName = strdup(docname);
	} else {
	    pPad->ttEditReq.docName = (char *) NULL;
	}
	if (op == TTME_DISPLAY)
	    pPad->xrdb.viewOnly = True;
    }

    /* -----> Create GUI components of Editor instance, set resouces,
     *        map window and load file (if specified). */
    if (!foundPad) {
      RealizeNewPad(pPad);		/* pPad->mainWindow is created here */
    } else {
      ManageOldPad(pPad, isCurrentlyVisible);
    }
				
    /* -----> Accept the message.
     *
     * Registers patterns in the default session to handle TT_HANDLER-addressed
     * requests (created by specifying the "handler" argument in
     * tttk_message_create) in the following manner (based on the shell and
     * Ttdt_contract_cb args):
     *
     *    type	requests		handled				notes
     *    ----	--------------------	----------------------------	-----
     *	  (1)	Get|Set_Geometry,	transparently			(a)
     *		Get|Set_Iconified,
     *		Get|Set_Mapped,
     *		Get|Set_XInfo,
     *		Raise, Lower
     *	  (2)	Pause, Resume		passed to Ttdt_contract_cb	(b)
     *	  (3)	Quit, Get_Status	passed to Ttdt_contract_cb	(c)
     *
     *	NOTES:
     *	  (a)	type (1) requests are handled transparently because the shell
     *		arg is specified (that is, a pattern is registered to handle
     *          these requests via an internal TT callback)
     *	  (b)	type (2) requests are handled like type (3) because 
     *		Ttdt_contract_cb is specified; otherwise they would be
     *		handled transparently
     *    (c)	type (3) request are handled in Ttdt_contract_cb because it
     *		was specified; otherwise they fail with TT_DESKTOP_ENOTSUP
     *          (that is, Ttdt_contract_cb is the pattern callback registered
     *          in the pattern to handle these requests).
     */
    ttrc = ttdt_message_accept(
		m,			/* Tt_message */
		TTdt_message_acceptCB,	/* Ttdt_contract_cb */
		pPad->app_shell,	/* shell widget */
		pPad->mainWindow,	/* client data */
		1,			/* call tt_message_accept() */
		1);			/* send STATUS notice to requestor */
    if (tt_ptr_error((void *) ttrc) != TT_OK) {
        fprintf(stderr,
		"Could not ttdt_message_accept the edit request: %s\n",
	        tt_status_message((tt_ptr_error((void *) ttrc))));
    }

    /* -----> Create pattern to handle "Save" requests in a manner analogous
     *        to "Quit" requests.
     * NOTE:  This pattern (unlike the Quit pattern automatically registered
     *        via ttdt_message_accept()) is *not* automatically destroyed
     *        when the original Edit request is destroyed. */
    pPad->ttEditReq.savePattern = TTCreateSavePattern(
		m,			/* Tt_message */
		TTSaveContractCB,	/* Ttdt_contract_cb */
		pPad->mainWindow,	/* client data - not currently used */
		1);			/* register it */

    /* -----> Set the DtEditor widget contents. */
    if (pPad->ttEditReq.contents) {
	contentRec.type = DtEDITOR_TEXT;
	contentRec.value.string = (char *) contents;
	errorCode = DtEditorSetContents(pPad->editor, &contentRec);

/* [SuG 5/18/95] Done
   The NOT_SAMPLE warnings/errors need to be added to the message
   catalog. Removed ifdefs.*/

	switch (errorCode) {
	    case DtEDITOR_NO_ERRORS:
		break;
	    case DtEDITOR_NULLS_REMOVED:
        Warning(pPad, (char *) GETMESSAGE(6, 6,
                "Embedded Nulls stripped from file."),
            XmDIALOG_WARNING);
		break;
	    case DtEDITOR_INSUFFICIENT_MEMORY:
        Warning(pPad, (char*) GETMESSAGE(6, 7,
                "Unable to load file (insufficient memory)."),
            XmDIALOG_ERROR);
		break;
	    default:
        Warning(pPad, (char *) GETMESSAGE(6, 12,
                "Unable to read from the file (unknown reason)."),
            XmDIALOG_ERROR);
		break;
	}
	}

    tt_release(mark);
    return((Tt_message) 0);	/* Message handled by this routine. */
    
}

/********************** Exported Functions (via dtpad.h) **********************/

/******************************************************************************
 *  TTstartDesktopMediaExchange - Initializes Took Talk for handling the
 *	"Desktop" and "Document & Media Exchange" message sets.
 *
 *	Returns: True if successful;  0 if failure.
 ******************************************************************************/
Tt_status
TTstartDesktopMediaExchange(
        Widget		topLevelWithWmCommand,
	Editor *	pPad)
{
    char	*my_procid = NULL;
    int		 my_ttfd = 0;
    Tt_pattern	*tt_pat = NULL;
    Tt_status	 status = TT_OK;
    int		 mark = tt_mark();
    char	*sess = NULL;


    /*
     * Open a connection to the ToolTalk service
     * and send a Started notice so other applications know we're up.
     */

    sess = (char *)getenv("TT_SESSION");
    if (!sess || (*sess == '\0')) {
      sess = getenv("_SUN_TT_SESSION");
    } 
    if (!sess || (*sess == '\0')) {
      tt_default_session_set(
	tt_X_session(XDisplayString(XtDisplay(topLevelWithWmCommand))) );
    } 

    status = TTdesktopOpen( &my_ttfd, pPad, (int) pPad->xrdb.server);
    if (TT_OK != status)
      return(status);

    if (pPad->xrdb.server == True) {
	/*
	 * Declare we will handle messages defined under our ptype
	 * which in this case currently consist of the Instantialte, Display
	 * and Edit messages from the Media set, as defined in dtpad.ptypes.
	 * This will enable auto start of dtpad if any of the messages in the
	 * ptype are received.
	 */
	if ((status = ttmedia_ptype_declare(
		    DTPAD_TOOL_CLASS,	/* ptype */
		    0,			/* base opnum */
		    TTmedia_ptype_declareCB,/* Ttmedia_load_pat_cb */
		    (void *) pPadList,	/* client data */
		    1))			/* call tt_ptype_declare() */
		!= TT_OK) {
	    fprintf(stderr,
                    "Could not register as %s tool class handler: %s\n",
                    DTPAD_TOOL_CLASS,
		    tt_status_message(status)) ;
	    return(status);
	}

	/*
	 * Join the default session,
	 * and register to deal with standard Desktop requests in the following
	 * manner (controlled by the Ttdt_contract_cb and shell arguments): 
	 *
	 *    type	requests		handled				notes
	 *    ----	--------------------	----------------------------	-----
	 * 	  (1)	Get|Set_Environment,	transparently
	 *		Get|Set_Locale,
	 *		Get|Set_Situation,
	 *		Signal, Get_Sysinfo
	 *	  (2)	Get|Set_Geometry,	fail with TT_DESKTOP_ENOTSUP	(b)	
	 *		Get|Set_Iconified,
	 *		Get|Set_Mapped,
	 *		Raise, Lower,
	 *		Set_Xinfo,
	 *		Get_Xinfo		transparently
	 *	  (3)	Pause, Resume, Quit	passed to Ttdt_contract_cb	(c)
	 *	  (4)	Get_Status, Do_Command	passed to Ttdt_contract_cb
	 *
	 *	NOTES:
	 *	  (b)	type (2) requests (except Get_Xinfo) fail because the shell
	 *		arg is not a "realized mappedWhenManaged applicationShellWidget"
	 *	  (c)	type (3) requests are not handled transparently because
	 *		Ttdt_contract_cb is specified
	 */
	tt_pat = ttdt_session_join(
		    (const char *) 0,		/* join default session */
		    TTdt_session_joinCB,	/* Ttdt_contract_cb */
		    topLevelWithWmCommand,	/* shell widget */
		    (void *) 0,			/* client data */
		    1);				/* really join the session */
	if ((status = tt_ptr_error(tt_pat)) != TT_OK) {
	    fprintf(stderr,
                    "Could not join default ToolTalk session: %s\n",
		    tt_status_message(tt_ptr_error(tt_pat)));
	    ttdt_close(0, 0, 1);
	    return(status);
	}  
    }


    /*
     * Have Xt monitor the connection to the ToolTalk service.
     * tttk_Xt_input_handler() will ultimately call tt_message_receive(),
     * which will invoke the callbacks we've registered.
     */
    XtAppAddInput(XtWidgetToApplicationContext(topLevelWithWmCommand),
		my_ttfd, (XtPointer) XtInputReadMask, tttk_Xt_input_handler,
		my_procid);

    return(status);
}


/************************************************************************
 * TTmediaDepositContents - sends a ToolTalk Deposit request (containing
 *	the current contents of the DtEditor widget) directly to the
 *	sender of the original media request.
 *
 *	Returns: 0 if successful;  !0 if failure.
 ************************************************************************/
Boolean
TTmediaDepositContents(
	Editor *pPad)
{
    DtEditorErrorCode errorCode;
    DtEditorContentRec contentRec;
    Boolean addNewlines = pPad->xrdb.wordWrap == True &&
			  pPad->fileStuff.saveWithNewlines == True;
    Tt_status status;

    _DtTurnOnHourGlass(pPad->app_shell);

    contentRec.type = DtEDITOR_TEXT;
    errorCode = DtEditorGetContents(
			pPad->editor,
			&contentRec,
			addNewlines,
			False);		/* don't mark contents as saved yet */
    if (errorCode != DtEDITOR_NO_ERRORS) {
	Warning(pPad,
		"Internal Error: Unable to get contents from Editor Widget.",
		XmDIALOG_ERROR);
	return(1);
    }

    status = ttmedia_Deposit(
		pPad->ttEditReq.contract,	/* original request */
		(char *) NULL,			/* send directly to requestor */
		pPad->ttEditReq.vtype,		/* media type */
 		(unsigned char *) contentRec.value.string, /*new contents */
		strlen(contentRec.value.string),
		(char *) NULL,		/* don't use temp file for xfer */
		pPad->app_context,	/* we're the blocking application */
		10 * TIMEOUT_FACTOR);	/* milliseconds to block for reply */

    if (status == TT_OK) {
	/* XXX - an inefficient way of marking contents as saved - 
	 *       need to use a global to keep track of unsaved changes
	 *       and replace DtEditorCheckForUnsavedChanges() */
	errorCode = DtEditorGetContents(
			pPad->editor,
			&contentRec,
			addNewlines,
			True);		/* mark contents as saved */
	_DtTurnOffHourGlass(pPad->app_shell);
    } else {
	_DtTurnOffHourGlass(pPad->app_shell);
	TTdisplayError(pPad,
		(char *) GETMESSAGE(13, 1, "Checkpoint save failed."),
		status);
	return(1);
    }
    return(0);
}


/************************************************************************
 * TTmediaReply - replies to (and closes) a ToolTalk media request.
 *
 *	If a file is being edited, it is assumed at this point that it
 *	has already been updated with the current contents.
 *
 *	If a buffer is being edited, the current text is included in the
 *	reply only if pPad->ttEditReq.returnBufContents is True.
 *
 *	Returns: 0 if successful;  !0 if failure.
 ************************************************************************/
Boolean
TTmediaReply(
	Editor *pPad)
{
    DtEditorErrorCode errorCode;
    DtEditorContentRec contentRec;
    Tt_status status;
    Boolean addNewlines = pPad->xrdb.wordWrap == True &&
			  pPad->fileStuff.saveWithNewlines == True;

    _DtTurnOnHourGlass(pPad->app_shell);

    if (! pPad->ttEditReq.contract) {
      return 0;
    }

    /* ----> Not editing a buffer */
    if (! pPad->ttEditReq.contents) {
	status = ttmedia_load_reply(
		pPad->ttEditReq.contract,	/* original request */
		(unsigned char *) 0,		/* new contents */
		0,				/* contents length */
		True);			/* destroy message after reply */
	_DtTurnOffHourGlass(pPad->app_shell);
	if (status == TT_OK) {
	    pPad->ttEditReq.contract = 0;	/* request has been closed */
	    return(0);
	} else {
	    TTdisplayError(pPad,
		(char *) GETMESSAGE(13, 2, "Checkpoint reply failed."),
		status);
	    return(1);
	}
    }

    /* ----> Editing a buffer */
    if (pPad->ttEditReq.returnBufContents) {
	/* -----> Include the contents in the reply */
	contentRec.type = DtEDITOR_TEXT;
	errorCode = DtEditorGetContents(
			pPad->editor,
			&contentRec,
			addNewlines,
			False);		/* don't mark contents as saved yet */
	if (errorCode != DtEDITOR_NO_ERRORS) {
	    Warning(pPad,
		"Internal Error: Unable to get contents from Editor Widget.",
		XmDIALOG_ERROR);
	    tttk_message_fail(pPad->ttEditReq.contract,
			TT_DESKTOP_ENODATA,		/* status */
			(char *) NULL,			/* status str */
			1);				/* destroy request */
	    return(1);
	}

	status = ttmedia_load_reply(
		pPad->ttEditReq.contract,	/* original request */
		(unsigned char *) contentRec.value.string, /* new contents */
		strlen(contentRec.value.string),
		True);		/* destroy message after reply */
	if (status == TT_OK) {
	    /* XXX - an inefficient way of marking contents as saved - 
	     *       need to use a global to keep track of unsaved changes
	     *       and replace DtEditorCheckForUnsavedChanges() */
	    errorCode = DtEditorGetContents(
			pPad->editor,
			&contentRec,
			addNewlines,
			True);		/* mark contents as saved */
	    _DtTurnOffHourGlass(pPad->app_shell);
	    pPad->ttEditReq.contract = NULL;	/* request has been closed */
	} else {
	    _DtTurnOffHourGlass(pPad->app_shell);
	    TTdisplayError(pPad,
		(char *) GETMESSAGE(13, 2, "Checkpoint reply failed."),
		status);
	    return(1);
	}

    } else {
	/* -----> Don't include anything in the buffer reply (e.g.
         *        when the user responds "no" to saving unsaved contents).
         *
         *    NOTE: TT_DESKTOP_ECANCELED changes the "failure" into a "reply".
         */
	status = tttk_message_fail(
			pPad->ttEditReq.contract,	/* original request */
			TT_DESKTOP_ECANCELED,		/* status */
			(char *) NULL,			/* status str */
			1);				/* destroy request */
	_DtTurnOffHourGlass(pPad->app_shell);
	if (status == TT_OK) {
	    pPad->ttEditReq.contract = NULL;	/* request has been closed */
	} else {
	    TTdisplayError(pPad,
		    (char *) GETMESSAGE(13, 2, "Checkpoint reply failed."),
		    status);
	    return(1);
	}
    }
    return(0);
}


/************************************************************************
 * TTresetQuitArgs - resets TTDT_QUIT arguments set in TTdt_message_acceptCB()
 ************************************************************************/
void
TTresetQuitArgs(
	Editor *pPad)
{
    if (!pPad->ttQuitReq.contract)
	return;
    pPad->ttQuitReq.contract = NULL;
    if (pPad->ttQuitReq.operation2Quit != (char *) NULL) {
	XtFree(pPad->ttQuitReq.operation2Quit);
	pPad->ttQuitReq.operation2Quit = (char *) NULL;
    }
    pPad->ttQuitReq.silent = 0;
    pPad->ttQuitReq.force = 0;
}


/************************************************************************
 * TTfailPendingQuit - fails any pending Quit request - even if 'force' is
 *	specified
 ************************************************************************/
void
TTfailPendingQuit(
	Editor *pPad)
{
    if (pPad->ttQuitReq.contract) {
	tttk_message_fail(pPad->ttQuitReq.contract,	/* original Quit req */
			TT_DESKTOP_ECANCELED,		/* status */
			(char *) NULL,			/* status string */
			1);				/* destroy request */
	TTresetQuitArgs(pPad);
    }
}

/************************************************************************
 * TTdesktopOpen - This procedure just opens a connection to
 *  ToolTalk so a simple message can be sent.
 ************************************************************************/
Tt_status
TTdesktopOpen( int *ttFd, Editor *pPad, Boolean sendStarted )
{
    char * procId;
    Tt_status status;
    char *vendor;

#ifdef sun
    vendor = "Sun Microsystems";
#else
#ifdef _AIX
    vendor = "IBM";
#else
#ifdef hpux
    vendor = "Hewlett-Packard";
#else
    vendor = "Unknown";
#endif /* hpux */
#endif /* _AIX */
#endif /* sun */

    procId = ttdt_open( ttFd, "dtpad", vendor, "2.1", sendStarted );
    if ( (status = tt_ptr_error( procId )) != TT_OK )
    {
        ttdt_close( NULL, NULL, sendStarted );
        return( status );
    }

    ProcessToolTalkInputId =
            XtAppAddInput( pPad->app_context, 
                           *ttFd, (XtPointer)XtInputReadMask,
                           tttk_Xt_input_handler, procId );
    return( TT_OK );
}

/************************************************************************
 * TTwarning - This procedure displays a message dialog and exits.
 ************************************************************************/
/* ARGSUSED */
static void
okCB(Widget w, XtPointer clientData, XtPointer callData)
{
    *((int*) clientData) = 1;
}

void
TTwarning( Editor *pPad, Tt_status status, char *errsuff )
{
    FileWidgets *pFileWidgets;
    char	*errfmt;
    char	*errmsg;
    char	*statmsg;
    int		 done = 0;

    if (TT_OK == status || NULL == pPad) return;

    errfmt =
      GETMESSAGE(7, 13, "Could not connect to ToolTalk service:\n%s\n%s\n");
    statmsg = tt_status_message(status);
    errmsg = XtMalloc(strlen(errfmt) + strlen(statmsg) + strlen(errsuff) + 2);

    if (NULL == errsuff) errsuff = "";
    fprintf(stderr, errfmt, statmsg, errsuff);
    sprintf(errmsg, errfmt, statmsg, errsuff);
    Warning(pPad, errmsg, XmDIALOG_ERROR);

    pFileWidgets = &pPad->fileStuff.fileWidgets;
    XtAddCallback(pFileWidgets->gen_warning, XmNokCallback,
                  (XtCallbackProc) okCB, (XtPointer) &done);
    XFlush(pPad->display);
    XSync(pPad->display, False);

    while ( !done ) 
    {
	XtAppProcessEvent(
		XtWidgetToApplicationContext(pFileWidgets->gen_warning),
		XtIMAll);
    }
}
