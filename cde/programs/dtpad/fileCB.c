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
/* $TOG: fileCB.c /main/13 1999/11/08 08:23:14 mgreess $ */
/**********************************<+>*************************************
***************************************************************************
**
**  File:        fileCB.c
**
**  Project:     DT dtpad, a memo maker type editor based on the Dt Editor
**               widget.
**
**  Description: 
**  -----------
**
**	This file contains the callbacks for the "File" menu items.
**	There's some hair here, due to the nested nature of some of
**	the dialogs.  The "New, "Open" and "Exit" callbacks can cause
**	the opening of a do-you-wish-to-save dialog, and we have to
**	remember how we got there.  This is done through the use
**	of the pendingFileFunc field of the Editor struct.
**
*******************************************************************
**  (c) Copyright Hewlett-Packard Company, 1991.  All rights are
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
#include <Dt/DtpadM.h>
#include <Dt/Action.h>

#include <Xm/XmPrivate.h> /* _XmStringUngenerate */

#include "dtpad.h"

extern int numActivePads;  /* declared in main.c */


/************************************************************************
 *			Forward Declarations
 ************************************************************************/
static void FileDoXpPrint(
	Editor* pPad,
        Boolean silent);
static void FileOpenOkCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data );
static void IncludeFile(
        Widget w,
        caddr_t client_data,
        caddr_t call_data );
static Boolean SaveUnsaved(
	Editor* pPad,
	void (*callingFunc)() );
static Boolean FileExitWP(
	XtPointer client_data);


/************************************************************************
 * FileCascadingCB -  callback assigned to "File" menu to determine
 *	whether Save button in menu is labeled "Save" or "Save (needed)".
 ************************************************************************/
/* ARGSUSED */
void
FileCascadingCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    Editor *pPad = (Editor *)client_data;
    Arg al[1];

    if (DtEditorCheckForUnsavedChanges(pPad->editor)) {
	XtSetArg(al[0], XmNlabelString, pPad->fileStuff.saveNeededBtnLabel);
    } else {
	XtSetArg(al[0], XmNlabelString, pPad->fileStuff.saveBtnLabel);
    }
    XtSetValues(pPad->fileStuff.fileWidgets.saveBtn, al, 1);

}


/************************************************************************
 * FileNewCB -  callback assigned to "File" menu "New" button which
 *	saves the current text if it hasn't been saved and then calls
 *	LoadFile().  Since pPad->fileStuff.fileName is NULL, LoadFile
 *	sets the contents to "" rather than loading a file.
 ************************************************************************/
/* ARGSUSED */
void
FileNewCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    Editor *pPad = (Editor *)client_data;

    if (SaveUnsaved(pPad, FileNewCB)) {
	return;
    }

    if (pPad->ttEditReq.contract && pPad->ttEditReq.op != TTME_INSTANTIATE) {
	TTmediaReply(pPad);	/* reply/close ToolTalk media request */
    }

    if (pPad->fileStuff.fileName != (char *)NULL) {
	XtFree(pPad->fileStuff.fileName);
        pPad->fileStuff.fileName = (char *)NULL;
    }
    _DtTurnOnHourGlass(pPad->app_shell);
    LoadFile(pPad, NULL);
    ChangeMainWindowTitle(pPad);
    _DtTurnOffHourGlass(pPad->app_shell);
}


/************************************************************************
 * FileOpenCB - callback assigned to "File" menu "Open..." button
 ************************************************************************/
/* ARGSUSED */
void
FileOpenCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    Editor *pPad = (Editor *)client_data;
    FileStuff *pStuff = &pPad->fileStuff;

    if (SaveUnsaved(pPad, FileOpenCB)) {
	return;
    }

    /* ToolTalk media requests are replied to (and closed) in FileOpenOkCB */
    
    /* -----> set FSB title passed GetFileName */
    if(pStuff->openTitle == (XmString)NULL) {
	char buf[256];

	strcpy(buf, DialogTitle(pPad));
	strcat(buf, (char *)GETMESSAGE(4, 1, "Open a File"));
	pStuff->openTitle = XmStringCreateLocalized(buf);
    }

    /* -----> obtain the name of the file to open (via a Xm file selection box)
     *        and load its contents (via FileOpenOkCB) to the Editor widget */
    pStuff->pendingFileFunc = FileOpenOkCB;	/* FSB XmNokCallback */
    pStuff->pendingFileHelpFunc = HelpOpenDialogCB;  /* FSB XmNhelpCallback */
    GetFileName(pPad, pStuff->openTitle, OPEN);
}


/************************************************************************
 * FileIncludeCB - callback assigned to "File" menu "Include..." button
 ************************************************************************/
/* ARGSUSED */
void
FileIncludeCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    Editor *pPad = (Editor *)client_data;
    FileStuff *pStuff = &pPad->fileStuff;

    if (pStuff->includeTitle == (XmString)NULL) {
	char buf[256];

	strcpy(buf, DialogTitle(pPad));
	strcat(buf, (char *)GETMESSAGE(4, 3, "Include a File"));
	pStuff->includeTitle = XmStringCreateLocalized(buf);
    }
    pPad->fileStuff.pendingFileFunc = IncludeFile;
    pPad->fileStuff.pendingFileHelpFunc = HelpIncludeDialogCB;
    GetFileName(pPad, pStuff->includeTitle, INCLUDE);
}


/************************************************************************
 * FileSaveCB - callback assigned to "File" menu "Save" button and to the
 *	AskIfSave dialog's "OK" button (which is displayed when there are
 *	unsaved changes when switching to a new file/buffer).
 *
 *
 ************************************************************************/
void
FileSaveCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    Editor *pPad = (Editor *)client_data;
    void (*pFunc)();
    DtEditorErrorCode errorCode;
    Boolean addNewlines = pPad->xrdb.wordWrap == True &&
			  pPad->fileStuff.saveWithNewlines == True;
    Tt_message m;

    if (pPad->fileStuff.fileName && *pPad->fileStuff.fileName) { /* filename? */
	/* -----> if called directly from [Save] menu and word wrap is on,
	 *         display Save dialog so that saveWithNewlines can be set */
	if (pPad->xrdb.wordWrap &&
	  !pPad->fileStuff.pendingFileFunc && !pPad->ttSaveReq.contract) {
            AskIfSave(pPad);  
	    pPad->fileStuff.pendingFileFunc = FileSaveCB;
            return;
	}
	_DtTurnOnHourGlass(pPad->app_shell);
	errorCode = DtEditorSaveContentsToFile(
			pPad->editor,
			pPad->fileStuff.fileName,
			True,			/* overwrite existing file */
			addNewlines,		/* replace soft line feeds? */
			True);			/* mark contents as saved */
	_DtTurnOffHourGlass(pPad->app_shell);
	if (errorCode != DtEDITOR_NO_ERRORS) {
	    PostSaveError(pPad, pPad->fileStuff.fileName, errorCode);
	    pPad->fileStuff.pendingFileFunc = (void(*)()) NULL;
	    TTfailPendingSave(pPad);
	    return;
	}
	if (pPad->ttEditReq.contract) {
	    /* ZZZ -----> Create and send Saved notice */
	    m = ttdt_file_notice(
			pPad->ttEditReq.contract,	/* context */
			TTDT_SAVED,			/* op */
			TT_SESSION,			/* Tt_scope */
			pPad->fileStuff.fileName,	/* msg file name */
			True);				/* send & destroy */
	}
	if (pPad->ttSaveReq.contract) {
	    if (! pPad->ttEditReq.contract) {
	        TTfailPendingSave(pPad);
	    } else {
		tt_message_reply(pPad->ttSaveReq.contract);
		tttk_message_destroy(pPad->ttSaveReq.contract);
	    }
	}
    } else {	/* no fileName associated with current text */
	if (pPad->ttEditReq.contract) {
	    if (pPad->ttEditReq.contents) {
		if (TTmediaDepositContents(pPad) != 0) {
		    if (pPad->fileStuff.pendingFileFunc == FileExitCB) {
			TTfailPendingQuit(pPad);
		    }
		    TTfailPendingSave(pPad);
		    pPad->fileStuff.pendingFileFunc = (void(*)()) NULL;
		    return;	/* deposit failed */
		}
	    } else {	/* TT request without fileName and contents */
		FileSaveAsCB(w, client_data, call_data);
		return;
	    }
	    if (pPad->ttSaveReq.contract) {
		tt_message_reply(pPad->ttSaveReq.contract);
		tttk_message_destroy(pPad->ttSaveReq.contract);
	    }
	} else {  /* non-TT request and no file name */
	    if (pPad->ttSaveReq.contract) {
		TTfailPendingSave(pPad);
	    } else {
		FileSaveAsCB(w, client_data, call_data);
	    }
	    return;
	}
    }

    if ((pFunc = pPad->fileStuff.pendingFileFunc) != (void (*)())NULL)  {
	pPad->fileStuff.pendingFileFunc = (void(*)()) NULL;
	if (pFunc != FileSaveCB) {
	    (*pFunc)(w, client_data, call_data);
	}
    }

}


/************************************************************************
 * FileSaveAsCB - callback assigned to "File" menu "SaveAs..." button
 ************************************************************************/
void
FileSaveAsCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    Editor *pPad = (Editor *)client_data;
    SaveAs *pSaveAs = &pPad->fileStuff.fileWidgets.saveAs;
    Widget textField;

    if (!pSaveAs->saveAs_form) {
        CreateSaveAsDialog(pPad);
        XtManageChild (pSaveAs->saveAs_form);
/*
 * XXX - Should be dealing with the FSB instead of the text field directly.
 *       Also, should be setting an XmString, rather than a char *
 */
        textField = XmFileSelectionBoxGetChild(
		        pPad->fileStuff.fileWidgets.saveAs.saveAs_form,
		        XmDIALOG_TEXT);
        XmTextFieldSetString(textField, "");
    } else {
        XtManageChild (pSaveAs->saveAs_form);
        textField = XmFileSelectionBoxGetChild(
		        pPad->fileStuff.fileWidgets.saveAs.saveAs_form,
		        XmDIALOG_TEXT);
    }

    SetSaveAsDirAndFile(pPad); /* seed the SaveAs FSB dir and file fields */

    /* Force the focus to the text field */
    XmProcessTraversal(textField, XmTRAVERSE_CURRENT);
    XSync(pPad->display, 0);

    _DtTurnOffHourGlass(pPad->app_shell);
}


extern Boolean ActionDBInitialized; /* declared in main.c */


/************************************************************************
 * PrintActionCB - callback assigned to "File" menu "Print" button
 ************************************************************************/
/* ARGSUSED */
void
PrintActionCB(
	DtActionInvocationID actionID,
	XtPointer client_data,
	DtActionArg *actionArgp,
	int argCount,
	DtActionStatus actionStatus)
{
    Editor *pPad = (Editor *) client_data;
    switch ((DtActionStatus) actionStatus) {
	case DtACTION_INVOKED:
	case DtACTION_STATUS_UPDATE:
	    /* break; */
	case DtACTION_DONE:
	case DtACTION_FAILED:
	case DtACTION_CANCELED:
	default:
/*	    XtSetSensitive(pPad->app_shell, True); */
	    _DtTurnOffHourGlass(pPad->app_shell);
	    break;
    }
}


/************************************************************************
 * FilePrintCB - callback assigned to "File" menu "Print" button
 ************************************************************************/
/* ARGSUSED */
void
FilePrintCB(
	Widget w,
	caddr_t client_data,
	caddr_t call_data)
{
    Editor *pPad = (Editor *) client_data;
    DtActionArg *actionArgp = (DtActionArg *) XtCalloc(2,sizeof(DtActionArg));
    DtActionInvocationID actionID;
    char *pr_name = (char *) NULL, *user_name;
    DtEditorErrorCode errorCode;

    Boolean addNewlines = pPad->xrdb.wordWrap == True &&
			  pPad->fileStuff.saveWithNewlines == True;

    _DtTurnOnHourGlass(pPad->app_shell);
    /* -----> Disallow keyboard, button, motion, window enter/leave & focus
     *        events till print (dialog) action is done
     * XtSetSensitive(pPad->app_shell, False);
     */

    /* -----> Get a place to temporarily write the text */
    if ((pr_name = GetTempFile()) == (char *)NULL) {
	_DtTurnOffHourGlass(pPad->app_shell);
	Warning(pPad, ((char *)
		GETMESSAGE(4, 5, "Unable to create a temporary file.")),		XmDIALOG_ERROR);
	return;
    }

    /* -----> Write the contents to the temp file */
    errorCode = DtEditorSaveContentsToFile(
			pPad->editor,
			pr_name,
			True,		/* overwrite existing file */
			addNewlines,	/* replace soft line feeds? */
			False);		/* don't mark contents as saved */
    if (errorCode != SUCCESS) {		/* this should never occur */
	_DtTurnOffHourGlass(pPad->app_shell);
	PostSaveError(pPad, pr_name, errorCode);
	return;
    }

    /* -----> Load the action database */
    if (! ActionDBInitialized) {
	StartDbUpdate( (XtPointer) NULL );
	/* register interest in Action DB changes (for printing) */
	DtDbReloadNotify( StartDbUpdate, (XtPointer) NULL );
    }

    /* -----> Determine the name the user will see in the Print dialog */
    if (pPad->ttEditReq.contract &&
		(pPad->ttEditReq.docName && *pPad->ttEditReq.docName)) {
	user_name = strdup(pPad->ttEditReq.docName);
    } else if (pPad->fileStuff.fileName && *pPad->fileStuff.fileName) {
	user_name = strdup(pPad->fileStuff.fileName);
    } else {
	user_name = strdup(UNNAMED_TITLE_P);
    }

    /* ----> Setup the action arguments - one for the filename as known by
     *       the user (Arg_1) and one for the temporary print file (Arg_2).
     *       PRINT_DTPAD_TEMPFILE does something like:
     *		/usr/dt/bin/dtlp -u %(String)Arg_2% -e %(File)Arg_1%
     *       dtlp displays a dialog to gather lp paramters and then prints
     *       the file.  The -e says to remove the temporary file after it
     *       it has been passed to lp. */
    actionArgp[0].argClass = DtACTION_FILE;
    actionArgp[0].u.file.name = pr_name;
    actionArgp[1].argClass = DtACTION_FILE;
    actionArgp[1].u.file.name = user_name;

    /* XXX - Try this after everything's working - don't directly output to
     *       a temp file but to a buffer and pass the buffer to the action
     *       and let the action automatically create/remove the temp file.
     *       This'll require a new action w/o -e.
     * actionArgp[0].argClass = DtACTION_BUFFER;
     * actionArgp[0].u.buffer.bp = (void *) buffer;
     * actionArgp[0].u.buffer.size = strlen(buffer;
     * actionArgp[0].u.buffer.type = TEXT;
     * actionArgp[0].u.buffer.writable = False;
     */

    /* XXX - Also, may want to set XtNsensitive to False on pPad->app_shell
     *       and turn it back on in PrintActionCB() when it receives a
     *       DtACTION_DONE status */ 
    /* ----> Invoke the print action */
    actionID = DtActionInvoke(pPad->app_shell,
		"PRINT_DTPAD_TEMPFILE",		/* action */
		actionArgp, 2,			/* action arguments & count */
		(char *) NULL,			/* terminal options */
		(char *) NULL,			/* execution host */
		(char *) NULL,			/* context dir */
		False,				/* no "use indicator" */
		PrintActionCB,			/* action callback */
                (XtPointer) pPad);		/* callback client data */

    XtFree(pr_name);
    XtFree(user_name);

   /*  _DtTurnOffHourGlass(pPad->app_shell);  this is done in PrintActionCB */
}




/************************************************************************
 * FileDoXpPrint - procedure doing the work of the XpPrint callbacks
 ************************************************************************/
/* ARGSUSED */
static void FileDoXpPrint(Editor *pPad, Boolean silent)
{
    PrintJob *pJob;

    DtActionArg *actionArgp = (DtActionArg *) XtCalloc(2,sizeof(DtActionArg));
    DtActionInvocationID actionID;
    DtEditorErrorCode errorCode;

    char *pr_name = (char *) NULL, *user_name;

    Boolean addNewlines = pPad->xrdb.wordWrap == True &&
			  pPad->fileStuff.saveWithNewlines == True;

    _DtTurnOnHourGlass(pPad->app_shell);
    /* -----> Disallow keyboard, button, motion, window enter/leave & focus
     *        events till print (dialog) action is done
     * XtSetSensitive(pPad->app_shell, False);
     */

    /* -----> Get a place to temporarily write the text */
    if ((pr_name = GetTempFile()) == (char *)NULL) {
	_DtTurnOffHourGlass(pPad->app_shell);
	Warning(pPad, ((char *)
		GETMESSAGE(4, 5, "Unable to create a temporary file.")),		XmDIALOG_ERROR);
	return;
    }

    /* -----> Write the contents to the temp file */
    errorCode = DtEditorSaveContentsToFile(
			pPad->editor,
			pr_name,
			True,		/* overwrite existing file */
			addNewlines,	/* replace soft line feeds? */
			False);		/* don't mark contents as saved */
    if (errorCode != SUCCESS) {		/* this should never occur */
	_DtTurnOffHourGlass(pPad->app_shell);
	PostSaveError(pPad, pr_name, errorCode);
	return;
    }

    /* -----> Determine the name the user will see in the Print dialog */
    if (pPad->ttEditReq.contract &&
		(pPad->ttEditReq.docName && *pPad->ttEditReq.docName)) {
	user_name = strdup(pPad->ttEditReq.docName);
    } else if (pPad->fileStuff.fileName && *pPad->fileStuff.fileName) {
	user_name = strdup(pPad->fileStuff.fileName);
    } else {
	user_name = strdup(UNNAMED_TITLE_P);
    }

    /* ----> XPPRINT:  Create and execute a print job. */
    pJob = PrintJobCreate(user_name, pr_name, silent, pPad);
    PrintJobExecute(pJob);

    XtFree(pr_name);
    XtFree(user_name);

   _DtTurnOffHourGlass(pPad->app_shell);
}

/************************************************************************
 * FileXpPrintCB - callback assigned to "File" menu "Print..." button
 ************************************************************************/
/* ARGSUSED */
void
FileXpPrintCB(
	Widget w,
	caddr_t client_data,
	caddr_t call_data)
{
    Editor *pPad = (Editor *) client_data;
    FileDoXpPrint(pPad, FALSE);
}


/************************************************************************
 * FileExitCB - callback assigned to "File" menu "Close" button
 ************************************************************************/
/* ARGSUSED */
void
FileExitCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data)
{
    Editor *pPad = (Editor *)client_data;

    if (SaveUnsaved(pPad, FileExitCB)) {
	/*
	 * If SaveUnsaved() returns True, don't close the window at this
	 * time since either:
	 *
	 * 1) a dialog has been posted which needs to be responded to
	 *    (and pendingFileFunc has been set to FileExitCB so that
	 *    this function will be resumed after the response) or
	 * 2) an error has occurred (in which case we abort the exit operation,
	 *    failing the TTDT_QUIT request if it initiated the exit).
	 */
	if (pPad->fileStuff.pendingFileFunc != FileExitCB) { /* error occurred */
	    TTfailPendingQuit(pPad);
	}
	return;
    }

    if (pPad->numPendingTasks > 0)
    {
	char	*msg;

	msg =
	GETMESSAGE(14, 20, "Close pending:  waiting for task to terminate ...");
	SetStatusMessage(pPad, msg);

	if (pPad->fileExitWorkprocID == 0)
	  pPad->fileExitWorkprocID = XtAppAddWorkProc(
					pPad->app_context,
					FileExitWP, pPad);
    }
    else
	FileExitWP((XtPointer) pPad);
}


/************************************************************************
 * FileExitWP - workproc called from FileExitCB
 ************************************************************************/
/* ARGSUSED */
static Boolean
FileExitWP(XtPointer client_data)
{
    Editor *pPad = (Editor *)client_data;
    Tt_status status;

    if (pPad->numPendingTasks > 0)
      return FALSE;

    if (pPad->fileExitWorkprocID != 0)
    {
	XtRemoveWorkProc(pPad->fileExitWorkprocID);
	pPad->fileExitWorkprocID = 0;
    }

    if (pPad->ttQuitReq.contract) {  /* reply to ToolTalk Quit request */
	status = tt_message_reply(pPad->ttQuitReq.contract);
	status = tttk_message_destroy(pPad->ttQuitReq.contract);
    }

    if (pPad->ttEditReq.contract) {
	TTmediaReply(pPad);	/* reply/close ToolTalk Edit/Display request */
    }

    if (pPad->xrdb.standAlone) {
        exit(pPad->confirmStuff.confirmationStatus);
    }

    pPad->inUse = False;
    numActivePads--;
    UnmanageAllDialogs(pPad);
    XtSetMappedWhenManaged(pPad->app_shell, False);
	XmImUnregister(pPad->editor);
    XtPopdown(pPad->app_shell);
    XWithdrawWindow(pPad->display, XtWindow(pPad->app_shell),
			XDefaultScreen(pPad->display));
    XFlush(pPad->display);

    /* -----> Send "DONE" notice if dealing with a requestor dtpad */
/*     if (!pPad->ttEditReq.contract && pPad->xrdb.blocking) { */
/* 	char numBuf[10]; */
/* 	sprintf(numBuf, "%d", pPad->confirmStuff.confirmationStatus); */
/* 	_DtSendSuccessNotification( */
/* 			"*", */
/* 			(DtString) NULL, */
/* 			(DtString) DTPAD_DONE, */
/* 			pPad->blockChannel, numBuf, NULL); */
/*     } */

    if (numActivePads == 0 && pPad->xrdb.exitOnLastClose) {
	exit(pPad->confirmStuff.confirmationStatus);
    }

    /*
     * If we're going to remain around, clean up Pad for its next use.
     * This speeds up opening a cached Pad at the expense of cycles at
     * close time.  Perception is reality.
     */

    /* -----> clear ToolTalk message info */
    TTresetQuitArgs(pPad);
    pPad->ttEditReq.contract = 0;
    if (pPad->ttEditReq.msg_id != (char *)NULL) {
	XtFree(pPad->ttEditReq.msg_id);
	pPad->ttEditReq.msg_id = (char *)NULL;
    }
    if (pPad->ttEditReq.vtype != (char *)NULL) {
	XtFree(pPad->ttEditReq.vtype);
	pPad->ttEditReq.vtype = (char *)NULL;
    }
    if (pPad->ttEditReq.fileName != (char *)NULL) {
	XtFree(pPad->ttEditReq.fileName);
	pPad->ttEditReq.fileName = (char *)NULL;
    }
    if (pPad->ttEditReq.docName != (char *)NULL) {
	XtFree(pPad->ttEditReq.docName);
	pPad->ttEditReq.docName = (char *)NULL;
    }
    if (pPad->ttEditReq.savePattern) {
	tt_pattern_destroy(pPad->ttEditReq.savePattern);
	pPad->ttEditReq.savePattern = NULL;
    }

    if (pPad->dialogTitle != (char *)NULL) {
	XtFree(pPad->dialogTitle);
	pPad->dialogTitle = NULL;
    }

    pPad->saveRestore = False;

    pPad->fileStuff.pendingFileFunc = (void (*)())NULL;
    pPad->fileStuff.pendingFileHelpFunc = (void (*)())NULL;
    pPad->fileStuff.fileExists = False;
    pPad->fileStuff.saveWithNewlines = True;
    pPad->fileStuff.readOnly = False;
    if (pPad->fileStuff.fileName != (char *) NULL) {
	XtFree(pPad->fileStuff.fileName);
	pPad->fileStuff.fileName = (char *) NULL;
    }
    if (pPad->fileStuff.netfile != (char *) NULL) {
	tt_free(pPad->fileStuff.netfile);
	 pPad->fileStuff.netfile = (char *) NULL;
    }

    /* -----> Clear contents, undo, find/change, format and message area */
    DtEditorReset(pPad->editor);

    /* -----> Reset resources to server's initial resource state */
    RestoreInitialServerResources(pPad);

    /* -----> Set iconic state to false */
    pPad->iconic = False;

    /* -----> Set app shell geo (pixels), resize hints, position & size hints */
    SetWindowSize(pPad);

    /* -----> Clear director "seed" in SaveAs FSB */
    if (pPad->fileStuff.fileWidgets.saveAs.saveAs_form != (Widget)NULL) {
	Widget textField = XmFileSelectionBoxGetChild(
				pPad->fileStuff.fileWidgets.saveAs.saveAs_form,
				XmDIALOG_TEXT);
	XmTextFieldSetString(textField, "");
    }

    return TRUE;
}


/************************************************************************
 * oldFileExitCB - callback assigned to "File" menu "Close" button
 ************************************************************************/
/* ARGSUSED */
void
oldFileExitCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data)
{
    Editor *pPad = (Editor *)client_data;
    Tt_status status;

    if (SaveUnsaved(pPad, FileExitCB)) {
	/*
	 * If SaveUnsaved() returns True, don't close the window at this
	 * time since either:
	 *
	 * 1) a dialog has been posted which needs to be responded to
	 *    (and pendingFileFunc has been set to FileExitCB so that
	 *    this function will be resumed after the response) or
	 * 2) an error has occurred (in which case we abort the exit operation,
	 *    failing the TTDT_QUIT request if it initiated the exit).
	 */
	if (pPad->fileStuff.pendingFileFunc != FileExitCB) { /* error occurred */
	    TTfailPendingQuit(pPad);
	}
	return;
    }

    if (pPad->ttQuitReq.contract) {  /* reply to ToolTalk Quit request */
	status = tt_message_reply(pPad->ttQuitReq.contract);
	status = tttk_message_destroy(pPad->ttQuitReq.contract);
    }

    if (pPad->ttEditReq.contract) {
	TTmediaReply(pPad);	/* reply/close ToolTalk Edit/Display request */
    }

    if (pPad->xrdb.standAlone) {
        exit(pPad->confirmStuff.confirmationStatus);
    }

    pPad->inUse = False;
    numActivePads--;
    UnmanageAllDialogs(pPad);
    XtSetMappedWhenManaged(pPad->app_shell, False);
	XmImUnregister(pPad->editor);
    XtPopdown(pPad->app_shell);
    XWithdrawWindow(pPad->display, XtWindow(pPad->app_shell),
			XDefaultScreen(pPad->display));
    XFlush(pPad->display);

    /* -----> Send "DONE" notice if dealing with a requestor dtpad */
/*     if (!pPad->ttEditReq.contract && pPad->xrdb.blocking) { */
/* 	char numBuf[10]; */
/* 	sprintf(numBuf, "%d", pPad->confirmStuff.confirmationStatus); */
/* 	_DtSendSuccessNotification( */
/* 			"*", */
/* 			(DtString) NULL, */
/* 			(DtString) DTPAD_DONE, */
/* 			pPad->blockChannel, numBuf, NULL); */
/*     } */

    if (numActivePads == 0 && pPad->xrdb.exitOnLastClose) {
	exit(pPad->confirmStuff.confirmationStatus);
    }

    /*
     * If we're going to remain around, clean up Pad for its next use.
     * This speeds up opening a cached Pad at the expense of cycles at
     * close time.  Perception is reality.
     */

    /* -----> clear ToolTalk message info */
    TTresetQuitArgs(pPad);
    pPad->ttEditReq.contract = 0;
    if (pPad->ttEditReq.msg_id != (char *)NULL) {
	XtFree(pPad->ttEditReq.msg_id);
	pPad->ttEditReq.msg_id = (char *)NULL;
    }
    if (pPad->ttEditReq.vtype != (char *)NULL) {
	XtFree(pPad->ttEditReq.vtype);
	pPad->ttEditReq.vtype = (char *)NULL;
    }
    if (pPad->ttEditReq.fileName != (char *)NULL) {
	XtFree(pPad->ttEditReq.fileName);
	pPad->ttEditReq.fileName = (char *)NULL;
    }
    if (pPad->ttEditReq.docName != (char *)NULL) {
	XtFree(pPad->ttEditReq.docName);
	pPad->ttEditReq.docName = (char *)NULL;
    }
    if (pPad->ttEditReq.savePattern) {
	tt_pattern_destroy(pPad->ttEditReq.savePattern);
	pPad->ttEditReq.savePattern = NULL;
    }

    if (pPad->dialogTitle != (char *)NULL) {
	XtFree(pPad->dialogTitle);
    }

    pPad->saveRestore = False;

    pPad->fileStuff.pendingFileFunc = (void (*)())NULL;
    pPad->fileStuff.pendingFileHelpFunc = (void (*)())NULL;
    pPad->fileStuff.fileExists = False;
    pPad->fileStuff.saveWithNewlines = True;
    pPad->fileStuff.readOnly = False;
    if (pPad->fileStuff.fileName != (char *) NULL) {
	XtFree(pPad->fileStuff.fileName);
	pPad->fileStuff.fileName = (char *) NULL;
    }
    if (pPad->fileStuff.netfile != (char *) NULL) {
	tt_free(pPad->fileStuff.netfile);
	 pPad->fileStuff.netfile = (char *) NULL;
    }

    /* -----> Clear contents, undo, find/change, format and message area */
    DtEditorReset(pPad->editor);

    /* -----> Reset resources to server's initial resource state */
    RestoreInitialServerResources(pPad);

    /* -----> Set iconic state to false */
    pPad->iconic = False;

    /* -----> Set app shell geo (pixels), resize hints, position & size hints */
    SetWindowSize(pPad);

    /* -----> Clear director "seed" in SaveAs FSB */
    if (pPad->fileStuff.fileWidgets.saveAs.saveAs_form != (Widget)NULL) {
	Widget textField = XmFileSelectionBoxGetChild(
				pPad->fileStuff.fileWidgets.saveAs.saveAs_form,
				XmDIALOG_TEXT);
	XmTextFieldSetString(textField, "");
    }
}


/************************************************************************
 * NoSaveCB - callback associated with the [No] button in the "Save changes
 *	to <file>?" PromptDialog created by CreateSaveWarning().
 ************************************************************************/
void
NoSaveCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    Editor *pPad = (Editor *)client_data;
    void (*pFunc)();

    XtUnmanageChild(pPad->fileStuff.fileWidgets.select.save_warning);


    if ((pFunc = pPad->fileStuff.pendingFileFunc) != (void(*)()) NULL)  {
	/* -----> don't clear the pending function if it calls SaveUnsaved() */
	if (pPad->fileStuff.pendingFileFunc != FileNewCB &&
	  pPad->fileStuff.pendingFileFunc != FileOpenCB &&
	  pPad->fileStuff.pendingFileFunc != FileExitCB) {
	    pPad->fileStuff.pendingFileFunc = (void(*)()) NULL;
	}
	if (pFunc != FileSaveCB) {
	    (*pFunc)(w, client_data, call_data);
	}
    }
}


/************************************************************************
 * CancelFileSelectCB - 
 ************************************************************************/
/* ARGSUSED */
void
CancelFileSelectCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    Editor *pPad = (Editor *)client_data;

    pPad->fileStuff.pendingFileFunc = (void(*)()) NULL;
    pPad->fileStuff.pendingFileHelpFunc = (void(*)()) NULL;

    /* popdown the file selection box */
    XtUnmanageChild (w);
    _DtTurnOffHourGlass(w);
    _DtTurnOffHourGlass(pPad->app_shell);
}


/************************************************************************
 * FileOpenOkCB - saves the name of a file to be opened and its directory,
 *	and then loads its contents into the DtEditor widget.
 *
 *	This callback is assigned to the "Ok" button of the File
 *	Selection Box displayed by the callback, FileOpenCB() assigned
 *	to the "File" menu "Open" button.
 ************************************************************************/
static void
FileOpenOkCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    Editor 	*pPad = (Editor *)client_data;
    FileStuff 	*pStuff = &pPad->fileStuff;
    XmFileSelectionBoxCallbackStruct *cb = (XmFileSelectionBoxCallbackStruct *)
					   call_data;
    char *name = (char *) XtMalloc( sizeof(char) * cb->length + 1 );
    name[0] ='\0';

    _DtTurnOnHourGlass(pPad->app_shell);
    _DtTurnOnHourGlass(w);

    if (pPad->ttEditReq.contract && pPad->ttEditReq.op != TTME_INSTANTIATE) {
	TTmediaReply(pPad);	/* reply/close ToolTalk media request */
    }

    /* -----> Get the name of the directory and file.
     * XXX - Eventually, it makes sense to store the name/etc. as an XmString
     * rather than convert everything to a string.  This will mean
     * changing the pPad.fileName type.
     * Additionally, we can get quit saving the text field ID and
     * deal only with the FSB. */

    name = (char *) _XmStringUngenerate(cb->value, NULL,
                                        XmMULTIBYTE_TEXT, XmMULTIBYTE_TEXT);

    if (pStuff->fileName != (char *)NULL) {
	XtFree(pStuff->fileName);
    }
    pStuff->fileName = name;
    ExtractAndStoreDir(pPad, name, OPEN); /* store pPad->fileStuff.pathDir */

    LoadFile(pPad, NULL);		/* this is always successful */
    ChangeMainWindowTitle(pPad);

    CancelFileSelectCB(w, client_data, call_data);
}


/************************************************************************
 * IncludeFile - obtains the name of a file to include (via a Xm FSB),
 *	parses the name and then
 *	inserts the file contents into the Dt Editor Widget (via LoadFile).
 *
 *	This callback is assigned to the "Ok" button of the
 *	File Selection Box displayed by the callback,
 *	FileIncludeCB() assigned to the "File" menu "Include" button.
 ************************************************************************/
static void
IncludeFile(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    Editor 	*pPad = (Editor *)client_data;
    XmFileSelectionBoxCallbackStruct *cb = (XmFileSelectionBoxCallbackStruct *)
					   call_data;
    char *name = (char *) XtMalloc( sizeof(char) * cb->length + 1 );
    name[0] ='\0';

    _DtTurnOnHourGlass(pPad->app_shell);
    _DtTurnOnHourGlass(w);

    /*
     *  Get the name of the file
     * ******
     * Eventually, it makes sense to store the name/etc. as an XmString
     * rather than convert everything to a string.  This will mean
     * changing the pPad.fileName type.
     * Additionally, we can get quit saving the text field ID and
     * deal only with the FSB.
     */

    name = (char *) _XmStringUngenerate(cb->value, NULL,
                                        XmMULTIBYTE_TEXT, XmMULTIBYTE_TEXT);

    ExtractAndStoreDir(pPad, name, INCLUDE); /* store pPad->fileStuff.pathDir */

    LoadFile(pPad, name);
    ChangeMainWindowTitle(pPad);
    CancelFileSelectCB(w, client_data, call_data);
    if (name != (char *)NULL)
        XtFree(name);
}


/************************************************************************
 * SaveUnsaved - allows unsaved changes to be saved to the current file
 *	or buffer.  If the AskIfSave dialog is posted, sets the global
 *	pendingFileFunc to the callingFunc so that the calling function
 *	can be reentered to finish its processing.
 *
 *	Returns True if the calling function should not continue due to:
 *
 *	1) some error condition (e.g. the file couldn't be saved), or
 *	2) a dialog has been posted which needs to be responded to
 *	   (pPad->fileStuff.pendingFileFunc will be set to the calling
 *	   function which may again be executed via a callback set on
 *	   the posted dialog)
 *
 ************************************************************************/
static Boolean
SaveUnsaved(
	Editor* pPad,
	void (*callingFunc)() )
{
    Boolean addNewlines;
    Tt_message m;

    /*
     * If there are unsaved changes, ask the user if they wish to
     * write them out.  If saveOnClose is True, then just write it.
     */
    if (DtEditorCheckForUnsavedChanges(pPad->editor)) {

	/* -----> If handling a "silent" TTDT_QUIT request, don't AskIfSave
	 *        and don't save any unsaved changes (TTDT_SAVE does this) */
	if (callingFunc == FileExitCB && 
	  pPad->ttQuitReq.contract && pPad->ttQuitReq.silent) {
	    if (pPad->ttQuitReq.force) {
		return False;	/* close edit window */
	    } else {
		return True;	/* don't close edit window */
	    }
	}

	pPad->ttEditReq.returnBufContents = True;

	if ((pPad->xrdb.saveOnClose) && 
		(pPad->fileStuff.fileName && *pPad->fileStuff.fileName)) {
	    DtEditorErrorCode errorCode;
	    addNewlines = pPad->xrdb.wordWrap == True &&
				  pPad->fileStuff.saveWithNewlines == True;
	    _DtTurnOnHourGlass(pPad->app_shell);
	    errorCode = DtEditorSaveContentsToFile(
			pPad->editor,
			pPad->fileStuff.fileName,
			True,			/* overwrite existing file */
			addNewlines,		/* replace soft line feeds? */
			True);			/* mark contents as saved */
            _DtTurnOffHourGlass(pPad->app_shell);
	    if (errorCode != SUCCESS) {
	        PostSaveError(pPad, pPad->fileStuff.fileName, errorCode);
	        if (callingFunc == FileExitCB) {
		    /* Set saveOnClose to False to force user to explicitly
		     * choose to not save changes in order to exit. */
		    pPad->xrdb.saveOnClose = False;
		}
		pPad->fileStuff.pendingFileFunc = (void(*)()) NULL;
		return True;	/* don't finish calling func */
	    } else {
		if (pPad->ttEditReq.contract) {
		    /* ZZZ -----> Create and send Saved notice */
		    m = ttdt_file_notice(
			pPad->ttEditReq.contract,	/* context */
			TTDT_SAVED,			/* op */
			TT_SESSION,			/* Tt_scope */
			pPad->fileStuff.fileName,	/* msg file name */
			True);				/* send & destroy */
		}
	    }
	} else {
	    if (callingFunc == pPad->fileStuff.pendingFileFunc) {
		/* We've already did AskIfSave but the user responded
		 * "No" so lets not keep asking (NoSaveCB does not clear
		 * pPad->fileStuff.pendingFileFunc). */
		pPad->fileStuff.pendingFileFunc = (void(*)()) NULL;
		pPad->ttEditReq.returnBufContents = False;
	    } else {
		/* AskIfSave assigns either FileSaveAsCB or FileSaveCB to the
		 * the o.k. dialog button that it posts.  These callbacks
		 * execute pPad->fileStuff.pendingFileFunc when done. */
       		pPad->fileStuff.pendingFileFunc = callingFunc;
		AskIfSave(pPad);
		return True;	/* don't finish calling func */
	    }
	}
    } else {	/* no unsaved contents */
	pPad->ttEditReq.returnBufContents = False;
    }

    return False;	/* finish calling funct */
}


/************************************************************************
 * SaveNewLinesCB - 
 ************************************************************************/
/* ARGSUSED */
void 
SaveNewLinesCB( 
        Widget w,
        XtPointer client_data,
        XtPointer call_data) 
{
    Editor *pPad = (Editor *)client_data;

    if (w == pPad->fileStuff.fileWidgets.saveAs.toggleWidgets.with_newl ||
	w == pPad->fileStuff.fileWidgets.select.toggleWidgets.with_newl)
	pPad->fileStuff.saveWithNewlines = True;
    else
	pPad->fileStuff.saveWithNewlines = False;
}


/************************************************************************
 * SaveAsOkCB - save the file
 ************************************************************************/
void
SaveAsOkCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    Editor *pPad = (Editor *)client_data;
    SaveAs *pSaveAs = &pPad->fileStuff.fileWidgets.saveAs;
    void (*pFunc)();
    Widget textField = XmFileSelectionBoxGetChild(
		    pPad->fileStuff.fileWidgets.saveAs.saveAs_form,
		    XmDIALOG_TEXT);
    DtEditorErrorCode errorCode;
    Tt_message m;
    Boolean addNewlines = pPad->xrdb.wordWrap == True &&
			  pPad->fileStuff.saveWithNewlines == True;
    Boolean overWrite, markSaved;
    XmFileSelectionBoxCallbackStruct *cb = (XmFileSelectionBoxCallbackStruct *)
                                           call_data;
    char *name = (char *) XtMalloc( sizeof(char) * cb->length + 1 );
    name[0] ='\0';

    _DtTurnOnHourGlass(pPad->app_shell);
    _DtTurnOnHourGlass(pSaveAs->saveAs_form);

    /* -----> Get the "save as" file name */

    name = (char *) _XmStringUngenerate(cb->value, NULL,
                                        XmMULTIBYTE_TEXT, XmMULTIBYTE_TEXT);

    pPad->fileStuff.savingName = name;
    ExtractAndStoreDir(pPad, name, OPEN);  /* store pPad->fileStuff.pathDir */

    /*
     * Normally, we would first try writing without overwriting the file
     * in which case we would get an error if the file pre-exists and we
     * would post a dialog asking the user if they wished to overwrite it.
     * However, if the name of the file to save is the same as the file
     * being edited, the user opened the file and knows that it exists.
     * In this case we would overwrite it without presenting the overwrite
     * option dialog.
     */
    if (strcmp(pPad->fileStuff.fileName, pPad->fileStuff.savingName) == 0) {
	overWrite = True;			/* overwrite */
    } else {
	overWrite = False;			/* don't overwrite yet */
    }

    /* -----> Don't mark the current contents as saved if saved to a
     *        file different than the (to be) current file */
    if (! pPad->xrdb.nameChange &&
      pPad->fileStuff.fileName &&   /* allow for -noNameChange w/o a fileName */
      ! overWrite) {
	markSaved = False;
    } else {
	markSaved = True;
    }

    errorCode = DtEditorSaveContentsToFile(
			pPad->editor,
			pPad->fileStuff.savingName,
			overWrite,
			addNewlines,		/* replace soft line feeds? */
			markSaved);		/* mark contents as saved? */
    if (errorCode == DtEDITOR_WRITABLE_FILE) { /* file exists & not overwriting */
	PostAlreadyExistsDlg(pPad);	/* save handled in AlrdyExistsOkCB */
	XtUnmanageChild (pSaveAs->saveAs_form);
	_DtTurnOffHourGlass(pSaveAs->saveAs_form);
	_DtTurnOffHourGlass(pPad->app_shell);
	return;
    }
    if (errorCode != SUCCESS) {
        PostSaveError(pPad, pPad->fileStuff.savingName, errorCode);
	XtFree(pPad->fileStuff.savingName);
	pPad->fileStuff.savingName = (char *)NULL;
	pPad->fileStuff.pendingFileFunc = (void(*)()) NULL;
    } else {
	if (pPad->ttEditReq.contract) {
	    /* ZZZ -----> Create and send Saved notice */
	    m = ttdt_file_notice(
			pPad->ttEditReq.contract,	/* context */
			TTDT_SAVED,			/* op */
			TT_SESSION,			/* Tt_scope */
			pPad->fileStuff.savingName,	/* msg file name */
			True);				/* send & destroy */
	    
	}
        if (pPad->xrdb.nameChange == True) {
	    if (pPad->ttEditReq.contract && pPad->ttEditReq.op != TTME_INSTANTIATE) {
		pPad->ttEditReq.returnBufContents = False;  /* drop chgs w/o notice */
		TTmediaReply(pPad);	/* reply/close ToolTalk media request */
	    }
            XtFree(pPad->fileStuff.fileName);
            pPad->fileStuff.fileName = pPad->fileStuff.savingName;
            ChangeMainWindowTitle(pPad);
        }
    }

    pPad->nodo = TRUE;
    XtUnmanageChild (pSaveAs->saveAs_form);
    _DtTurnOffHourGlass(pSaveAs->saveAs_form);
    _DtTurnOffHourGlass(pPad->app_shell);
    if ((pFunc = pPad->fileStuff.pendingFileFunc) != (void (*)())NULL)  {
	pPad->fileStuff.pendingFileFunc = (void(*)()) NULL;
	(*pFunc)(w, client_data, call_data);
    }
}


/************************************************************************
 * AlrdyExistsOkCB - the ok callback for a saveAs of a file which already
 *	exists.  Specifically, this routine:
 *
 *	- saves the current text to the file specified by
 *	  pPad->fileStuff.savingName
 *	- if appropriate, resets the name of the current file
 *	  (pPad->fileStuff.fileName) to pPad->fileStuff.savingName and
 *	  frees pPad->fileStuff.savingName
 *	- executes pPad->fileStuff.pendingFileFunc if specified
 
 ************************************************************************/
void
AlrdyExistsOkCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    Editor *pPad = (Editor *)client_data;
    void (*pFunc)();
    DtEditorErrorCode errorCode;
    Boolean addNewlines = pPad->xrdb.wordWrap == True &&
			  pPad->fileStuff.saveWithNewlines == True;
    Boolean markSaved;
    Tt_message m;

    _DtTurnOnHourGlass(pPad->app_shell);
    _DtTurnOnHourGlass(w);

    /* -----> Don't mark the current contents as saved if saved to a
     *        file different than the (to be) current file */
    if (! pPad->xrdb.nameChange && 
      pPad->fileStuff.fileName &&   /* allow for -noNameChange w/o a fileName */
      (strcmp(pPad->fileStuff.fileName, pPad->fileStuff.savingName) != 0)) {
	markSaved = False;
    } else {
	markSaved = True;
    }

    errorCode = DtEditorSaveContentsToFile(
			pPad->editor,
			pPad->fileStuff.savingName,
			True,			/* overwrite existing file */
			addNewlines,		/* replace soft line feeds? */
			markSaved);		/* mark contents as saved? */

    XtUnmanageChild (w);
    _DtTurnOffHourGlass(w);
    _DtTurnOffHourGlass(pPad->app_shell);

    if (errorCode == SUCCESS) {
	if (pPad->ttEditReq.contract) {
	    /* ZZZ -----> Create and send Saved notice */
	    m = ttdt_file_notice(
			pPad->ttEditReq.contract,	/* context */
			TTDT_SAVED,			/* op */
			TT_SESSION,			/* Tt_scope */
			pPad->fileStuff.savingName,	/* msg file name */
			True);				/* send & destroy */
	}
        if (pPad->xrdb.nameChange == True) {
	    if (pPad->ttEditReq.contract && pPad->ttEditReq.op != TTME_INSTANTIATE) {
		pPad->ttEditReq.returnBufContents = False;  /* drop chgs w/o notice */
		TTmediaReply(pPad);	/* reply/close ToolTalk media request */
	    }
            XtFree(pPad->fileStuff.fileName);
            pPad->fileStuff.fileName = pPad->fileStuff.savingName;
            ChangeMainWindowTitle(pPad);
        }
    } else {
        PostSaveError(pPad, pPad->fileStuff.savingName, errorCode);
    }
    if (pPad->fileStuff.savingName != pPad->fileStuff.fileName)
	XtFree(pPad->fileStuff.savingName);
    pPad->fileStuff.savingName = (char *)NULL;

    if ((pFunc = pPad->fileStuff.pendingFileFunc) != (void (*)())NULL)  {
	pPad->fileStuff.pendingFileFunc = (void(*)()) NULL;
	(*pFunc)(w, client_data, call_data);
    }
}


/************************************************************************
 * SaveAsCancelCB - Unmanage the SaveAs dialog
 ************************************************************************/
/* ARGSUSED */
void
SaveAsCancelCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    Editor *pPad = (Editor *) client_data;

    XtUnmanageChild ((Widget) pPad->fileStuff.fileWidgets.saveAs.saveAs_form);
    pPad->fileStuff.pendingFileFunc = (void (*)())NULL;
    pPad->fileStuff.pendingFileHelpFunc = (void (*)())NULL;
}


/************************************************************************
 * AlrdyExistsCancelCB - Unmanage the AlreadyExists dialog
 ************************************************************************/
/* ARGSUSED */
void
AlrdyExistsCancelCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    Editor *pPad = (Editor *)client_data;
    XtUnmanageChild ((Widget) pPad->fileStuff.fileWidgets.saveAs.alrdy_exist);
    XtFree(pPad->fileStuff.savingName);
    pPad->fileStuff.savingName = (char *) NULL;
    pPad->fileStuff.pendingFileFunc = (void (*)())NULL;
    pPad->fileStuff.pendingFileHelpFunc = (void (*)())NULL;
}


/************************************************************************
 * AskIfSaveCancelCB - 
 ************************************************************************/
/* ARGSUSED */
void
AskIfSaveCancelCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    Editor *pPad = (Editor *)client_data;

    XtUnmanageChild ((Widget) pPad->fileStuff.fileWidgets.select.save_warning);
    if (pPad->fileStuff.pendingFileFunc == FileExitCB) {
	TTfailPendingQuit(pPad);
    }
    pPad->fileStuff.pendingFileFunc = (void (*)())NULL;
    pPad->fileStuff.pendingFileHelpFunc = (void (*)())NULL;
}

