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
 *+SNOTICE
 *
 *	$TOG: AttachCmds.C /main/8 1999/07/02 14:33:38 mgreess $
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

#include <Xm/Xm.h>
#include <Xm/FileSB.h>
#include <Xm/SelectioB.h>
#include <Xm/MessageB.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>
#include "Application.h"
#include "AttachArea.h"
#include "Attachment.h"
#include "Icon.h"
#include "AttachCmds.h"
#include "InfoDialogManager.h"
#ifdef DEAD_WOOD
#include "QuestionDialogManager.h"
#endif /* DEAD_WOOD */
#include "RoamMenuWindow.h"
#include <DtMail/DtMailError.hh>
#include <DtMail/DtMail.hh>
#include "RoamApp.h"
#include "ViewMsgDialog.h"
#include "MailMsg.h"


extern "C" {
extern XtPointer _XmStringUngenerate (
				XmString string,
				XmStringTag tag,
				XmTextType tag_type,
				XmTextType output_type);
}

extern nl_catd	DtMailMsgCat;

static void okcb(XtPointer);

AttachAddCmd::AttachAddCmd ( AttachArea *attachArea, 
			     Widget parent, 
			     Widget clipWindow, 
			     char *name, 
			     char *label,
			     int active ) : Cmd ( name, label, active )
{

    _attachArea = attachArea;
    _clipWindow = clipWindow;
    _parent     = parent;

}

void AttachAddCmd::doit()
{
    FSState fsstate;
    Widget fsdialog;
    char *home;
    struct passwd *pwd;

    fsdialog = _attachArea->getFsDialog();
    if(fsdialog == NULL) {
	fsdialog = XmCreateFileSelectionDialog(_parent, "fsdialog", NULL, 0);
	XtUnmanageChild(
		XmFileSelectionBoxGetChild(fsdialog, XmDIALOG_HELP_BUTTON));
	XtVaSetValues(XmFileSelectionBoxGetChild(fsdialog, XmDIALOG_LIST),
	    XmNselectionPolicy, XmBROWSE_SELECT,
	    NULL);
	pwd = getpwuid(getuid());
	home = pwd->pw_dir;
	XtVaSetValues(fsdialog,
	    XtVaTypedArg, XmNdirectory, XtRString,
	    home, strlen(home)+1,
	    NULL);
	_attachArea->setFsDialog(fsdialog);
	XtAddCallback(fsdialog, XmNcancelCallback, 
			  &AttachAddCmd::cancelCallback,
			  (XtPointer) this );
    }
    fsstate = _attachArea->getFsState();
    if(fsdialog) {
	switch(fsstate) {
	case ADD:	// Do nothing
	    break;
	case SAVEAS:
	    // Remove callbacks
	    XtRemoveAllCallbacks(fsdialog, XmNokCallback);
	    // Fall Through
	case NOTSET:
	    // Install callbacks
	    XtAddCallback(fsdialog, XmNokCallback, 
			      &AttachAddCmd::okCallback,
			      (XtPointer) this );
	    break;
	}
	XtVaSetValues(XtParent(fsdialog),
		XmNtitle, GETMSG(DT_catd, 14, 1, "Add Attachment"),
		NULL);
	XtManageChild(fsdialog);
    }
    XtVaSetValues(fsdialog, 
		    XmNfileTypeMask, XmFILE_REGULAR, 
		    NULL);
    _attachArea->setFsState(ADD);
    _attachArea->activateDeactivate();
    XRaiseWindow(XtDisplay(fsdialog), XtWindow(XtParent(fsdialog)) );
}      

void AttachAddCmd::undoit()
{
    // Just print a message that allows us to trace the execution
    
}       

void AttachAddCmd::okCallback ( Widget w, XtPointer clientData, XtPointer callData )
{
    AttachAddCmd *obj = (AttachAddCmd *) clientData;

    obj->ok( w, callData );
    //XtUnmanageChild( w );
}

void AttachAddCmd::ok( Widget , XtPointer callData )
{
    XmFileSelectionBoxCallbackStruct *cbs = 
	    (XmFileSelectionBoxCallbackStruct *)callData;
    int count, i;
    XmStringTable string_table;
    char *filename;

    XtVaGetValues(XmFileSelectionBoxGetChild(_attachArea->getFsDialog(),
			XmDIALOG_LIST),
	XmNselectedItems, &string_table,
	XmNselectedItemCount, &count,
	NULL);

    if(count == 0) {
	filename = NULL;
        filename = (char *) _XmStringUngenerate(
					cbs->value, NULL,
					XmMULTIBYTE_TEXT, XmMULTIBYTE_TEXT);
	if (NULL == filename) return; // internal error
	add_file(filename);
    } else {
	for(i=0;i<count;i++) {
	    filename = NULL;
            filename = (char *) _XmStringUngenerate(
					string_table[i], NULL,
					XmMULTIBYTE_TEXT, XmMULTIBYTE_TEXT);
	    if (NULL == filename) return; // internal error
	    add_file(filename);
	}
    }

}

void AttachAddCmd::add_file( char *filename )
{
    int fd;
    struct stat s;
//    ExecState executable;
    Boolean validtype = TRUE;
    char *errormsg = new char[512];

    if(stat(filename, &s) == -1) {
	sprintf(errormsg, 
		GETMSG(DT_catd, 14, 2, "Unable to open %s"), 
		filename
	);
		
// 	theInfoDialogManager->post(
// 		    errormsg,
// 		    (void *)NULL,
//  		    okcb);
	delete [] errormsg;
	return;
    }

    if(S_ISFIFO(s.st_mode)) {
	sprintf(errormsg,
		GETMSG(DT_catd, 14, 3,
		    "Cannot attach FIFO files: %s"), filename
	);
	validtype = FALSE;
    } else if(S_ISCHR(s.st_mode)) {
	sprintf(errormsg,
		GETMSG(DT_catd, 14, 4,
		    "Cannot attach character special files: %s"), filename
	);
	validtype = FALSE;
    } else if(S_ISDIR(s.st_mode)) {
	sprintf(errormsg,
		GETMSG(DT_catd, 14, 5,
		    "Cannot attach directories: %s"), filename
	);
	validtype = FALSE;
    } else if(S_ISBLK(s.st_mode)) {
	sprintf(errormsg,
		GETMSG(DT_catd, 14, 6,
		    "Cannot attach block special files: %s"), filename
	);
	validtype = FALSE;
    } else if(S_ISSOCK(s.st_mode)) {
	sprintf(errormsg,
		GETMSG(DT_catd, 14, 7,
		    "Cannot attach socket files: %s"), filename
	);
	validtype = FALSE;
    }
    if(validtype == FALSE) {
// 	theInfoDialogManager->post(
// 		    errormsg,
// 		    (void *)NULL,
// 		    okcb);
	delete [] errormsg;
	return;

    }
    fd = open(filename, O_RDONLY);
    if(fd == -1) {
	sprintf(errormsg, 
		GETMSG(DT_catd, 14, 8,
		    "Unable to open %s"),filename
	);

// 	theInfoDialogManager->post(
// 		    errormsg,
// 		    (void *)NULL,
// 		    okcb);
	delete [] errormsg;
	return;
    } else {
	
	// We open and close this file just to make sure that we can
	// The attachment constructor must have a valid, openable, file
	close(fd);
    }

//     Attachment *attachment = new Attachment(
// 					_attachArea, 
// 					filename);
// 
//     attachment->setAttachArea(_attachArea);
//     attachment->initialize();
//     _attachArea->calcSizeOfAA();
// 
//     // Call CalcAttachmentPosition() before adding the attachment
//     // to the list so that we can successfully find the last attachment
//     // in the list
//     _attachArea->CalcAttachmentPosition(attachment);
//     _attachArea->addToList( attachment );
    XtFree(filename);
    _attachArea->CalcLastRow();
    _attachArea->AdjustCurrentRow();
    _attachArea->SetScrollBarSize(_attachArea->getLastRow()+1);
    _attachArea->DisplayAttachmentsInRow(_attachArea->getCurrentRow());
    delete [] errormsg;
}

void AttachAddCmd::cancelCallback ( Widget, XtPointer clientData, XtPointer callData )
{
    AttachAddCmd *obj = (AttachAddCmd *) clientData;

    obj->cancel( callData );
}

void AttachAddCmd::cancel( XtPointer )
{
    Widget fsdialog = _attachArea->getFsDialog();

    XtUnmanageChild(fsdialog);
}

AttachFetchCmd::AttachFetchCmd ( AttachArea *attachArea, 
				 char *name, 
				 char *label,
				 int active ) : Cmd ( name, label, active )
{
    _attachArea = attachArea;
}

void AttachFetchCmd::doit()
{
//    int i;
//  Attachment **list = _attachArea->getList();
//    String contents;
//    unsigned long size;
//    char error[1024];
//    Boolean compressed;

//    if(_attachArea->get_mailbox()->get_disconnected() == TRUE)
//	return;
//     for(i=0;i<_attachArea->getIconCount();i++) {
// 	if(((list[i]->get_ac_state() == ROAM_AVAILABLE) ||
// 	    (list[i]->get_ac_state() == ROAM_CACHED)) &&
// 	   (list[i]->getIcon()->isSelected() == TRUE)) {
// 	    RoamMenuWindow *rmw = 
// 			(RoamMenuWindow*)_attachArea->owner()->owner();
// 
// // 	    contents = (char * ) _attachArea->get_mailbox()->get_data(
// // 				    rmw->msgno(),
// // 				    list[i]->getSection(),
// // 				    &size,
// // 				    &compressed);
// 
// 	    contents = NULL;
// 	    list[i]->set_compressed(compressed);
// 	    // If contents is NULL then we were unable to get the contents
// 	    if(contents == NULL) {
// 		sprintf(error, 
//			GETMSG(DT_catd, 14, 9,
//			    "File contents unavailable: %s"),
// 			     list[i]->getLabel()
//		);
// // 		theInfoDialogManager->post(
// // 		    error,
// // 		    (void *)NULL,
// // 		    (DialogCallback)okcb);
// 		return;
// 	    }
// 	    list[i]->setContents(contents);
// 	    list[i]->setFilesize( size );
// 	    list[i]->set_ac_state(ROAM_LOCAL);
// //	    if(list[i]->getRow() == _attachArea->getCurrentRow())
// //		list[i]->getIcon()->expose((XtPointer)NULL);
// 	}
//     }
}      

void AttachFetchCmd::undoit()
{
    // Just print a message that allows us to trace the execution

}       

AttachDeleteCmd::AttachDeleteCmd ( AttachArea *attachArea, 
				   char *name, 
				   char *label,
				   int active ) : Cmd ( name, label, active )
{
    _attachArea = attachArea;
}

void AttachDeleteCmd::doit()
{
//    int i, j;
    WidgetList deleteList;

    deleteList = (WidgetList)XtMalloc(sizeof(Widget)*_attachArea->getIconCount());
//     Attachment **list = _attachArea->getList();
//     for(i=0, j=0;i<_attachArea->getIconCount();i++)
// 	if(list[i]->getIcon()->isSelected() == TRUE) {
// 	    deleteList[j++] = list[i]->baseWidget();
// 	    list[i]->getIcon()->unselect();
// 	    _attachArea->incDeleteCount();
// 	    _attachArea->setAttachmentsLabel();
// 	    list[i]->deleteIt();
// 	}
//     XtUnmanageChildren(deleteList, j);
//     XtFree((char *)deleteList);
//     _attachArea->activateDeactivate();
//     _attachArea->CalcAllAttachmentPositions();
//     _attachArea->CalcLastRow();
//     _attachArea->AdjustCurrentRow();
//     _attachArea->SetScrollBarSize(_attachArea->getLastRow()+1);
//     _attachArea->DisplayAttachmentsInRow(_attachArea->getCurrentRow());
}      

void AttachDeleteCmd::undoit()
{
    // Just print a message that allows us to trace the execution
    
}       

AttachOpenCmd::AttachOpenCmd ( AttachArea *attachArea, 
			       char *name, 
			       char *label,
			       int active ) : Cmd ( name, label, active )
{
    _attachArea = attachArea;
}

void AttachOpenCmd::doit()
{
//    int i;

//     Attachment **list = _attachArea->getList();
//     for(i=0;i<_attachArea->getIconCount();i++)
// 	if(list[i]->getIcon()->isSelected() == TRUE) {
// 	    list[i]->executeOpenMethod();
// 	    // Break after finding a selected file
// 	    break;
// 
// 	}
    
}      

void AttachOpenCmd::undoit()
{
    // Just print a message that allows us to trace the execution
    
}       

AttachRenameCmd::AttachRenameCmd ( AttachArea *attachArea, 
				   Widget parent, 
				   char *name, 
				   char *label,
				   int active ) : Cmd ( name, label, active )
{
    Widget renameDialog;
    XmString message;

    _attachArea = attachArea;
    renameDialog = XmCreatePromptDialog(parent, "renameDialog", NULL, 0);
    message = XmStringCreateLocalized(attachArea->getRenameMessageString());
    XtVaSetValues(renameDialog, XmNselectionLabelString, message, NULL);
    XmStringFree(message);
    XtVaSetValues(XtParent(renameDialog),
	    XmNtitle, GETMSG(DT_catd, 14, 9, "Mailer - Rename Attachment"),
	    NULL);
    XtUnmanageChild(
	XmSelectionBoxGetChild(renameDialog, XmDIALOG_HELP_BUTTON)
    );
    _attachArea->setRenameDialog(renameDialog);
    XtAddCallback(renameDialog, XmNcancelCallback, 
		      &AttachRenameCmd::cancelCallback,
		      (XtPointer) this );
    XtAddCallback(renameDialog, XmNokCallback, 
		      &AttachRenameCmd::okCallback,
		      (XtPointer) this );
}

void AttachRenameCmd::doit()
{
    Widget renameDialog;

    renameDialog = _attachArea->getRenameDialog();
    XtManageChild(renameDialog);
    XtPopup(XtParent(renameDialog), XtGrabNone);
}      

void AttachRenameCmd::undoit()
{
    // Just print a message that allows us to trace the execution
    
}       

void AttachRenameCmd::cancelCallback ( Widget, XtPointer clientData, XtPointer callData )
{
    AttachRenameCmd *obj = (AttachRenameCmd *) clientData;

    obj->cancel( callData );
}

void AttachRenameCmd::cancel( XtPointer )
{
    Widget renameDialog = _attachArea->getRenameDialog();

    XtUnmanageChild(renameDialog);
}

void AttachRenameCmd::okCallback ( Widget, XtPointer clientData, XtPointer callData )
{
    AttachRenameCmd *obj = (AttachRenameCmd *) clientData;

    obj->ok( callData );
}

void AttachRenameCmd::ok( XtPointer callData )
{
    XmSelectionBoxCallbackStruct *cbs = 
	    (XmSelectionBoxCallbackStruct *)callData;
    String label = NULL;

    if(_attachArea->getIconSelectedCount() != 1) {
// 	theInfoDialogManager->post(
// 		    GETMSG(DT_catd, 14, 12,
//	"Please select only one attachment to rename"),
// 		    (void *)NULL,
// 		    okcb);
	return;
    }
//     Attachment **list = _attachArea->getList();
//     for(i=0;i<_attachArea->getIconCount();i++)
// 	if(list[i]->getIcon()->isSelected()) {
//	    label = NULL;
//	    label = (char *) _XmStringUngenerate(
//					cbs->value, NULL,
//					XmMULTIBYTE_TEXT, XmMULTIBYTE_TEXT);
//	    if (NULL == label) return; // internal error
// 	    list[i]->setLabelAndCenter(label);
// 	}
}

AttachDescriptionCmd::AttachDescriptionCmd ( AttachArea *attachArea, 
					     Widget parent, 
					     char *name, 
					     char *label,
					     int active ) 
			: Cmd ( name, label, active )
{
    Widget descriptionDialog;
    XmString message;

    _attachArea = attachArea;
    descriptionDialog = XmCreatePromptDialog(parent, "descriptionDialog", NULL, 0);
    message = XmStringCreateLocalized(attachArea->getDescriptionMessageString());
    XtVaSetValues(descriptionDialog, XmNselectionLabelString, message, NULL);
    XmStringFree(message);
    XtVaSetValues(XtParent(descriptionDialog),
	    XmNtitle, GETMSG(DT_catd, 14, 10, "Description"),
	    NULL);
    XtUnmanageChild(XmSelectionBoxGetChild(descriptionDialog, XmDIALOG_HELP_BUTTON));
    _attachArea->setDescriptionDialog(descriptionDialog);
    XtAddCallback(descriptionDialog, XmNcancelCallback, 
		      &AttachDescriptionCmd::cancelCallback,
		      (XtPointer) this );
    XtAddCallback(descriptionDialog, XmNokCallback, 
		      &AttachDescriptionCmd::okCallback,
		      (XtPointer) this );
}

void AttachDescriptionCmd::doit()
{
//     Attachment **list = _attachArea->getList();
//     Widget descriptionDialog;
//     Widget text;
//     int i;
// 
//     descriptionDialog = _attachArea->getDescriptionDialog();
//     text = XmSelectionBoxGetChild(descriptionDialog, XmDIALOG_TEXT);
//     for(i=0;i<_attachArea->getIconCount();i++) {
// 	if(list[i]->getIcon()->isSelected()) {
// 	    XtVaSetValues(text,
// 		XmNvalue, list[i]->get_description() ?
// 			    list[i]->get_description() : "",
// 		NULL);
// 	    break;
// 	}
//     }
//    XtManageChild(descriptionDialog);
}      

void AttachDescriptionCmd::undoit()
{
    // Just print a message that allows us to trace the execution
    
}       

void AttachDescriptionCmd::cancelCallback ( Widget, XtPointer clientData, XtPointer callData )
{
    AttachDescriptionCmd *obj = (AttachDescriptionCmd *) clientData;

    obj->cancel( callData );
}

void AttachDescriptionCmd::cancel( XtPointer )
{
    Widget descriptionDialog = _attachArea->getDescriptionDialog();

    XtUnmanageChild(descriptionDialog);
}

void AttachDescriptionCmd::okCallback ( Widget, XtPointer clientData, XtPointer callData )
{
    AttachDescriptionCmd *obj = (AttachDescriptionCmd *) clientData;

    obj->ok( callData );
}

void AttachDescriptionCmd::ok( XtPointer callData )
{
    XmSelectionBoxCallbackStruct *cbs = 
	    (XmSelectionBoxCallbackStruct *)callData;
    String label = NULL;

    if(_attachArea->getIconSelectedCount() != 1) {
// 	theInfoDialogManager->post(
// 		    GETMSG(DT_catd, 14, 12,
//			    "Please select only one attachment to describe"),
// 		    (void *)NULL,
// 		    okcb);
	return;
    }
//     Attachment **list = _attachArea->getList();
//     for(i=0;i<_attachArea->getIconCount();i++)
// 	if(list[i]->getIcon()->isSelected()) {
//	    label = NULL;
//          label = (char *) _XmStringUngenerate(
//					cbs->value, NULL,
//					XmMULTIBYTE_TEXT, XmMULTIBYTE_TEXT);
//	    if (NULL == label) return; // internal error
// 	    list[i]->set_description(label);
// 	}
}

AttachSaveAsCmd::AttachSaveAsCmd ( AttachArea *attachArea, 
				   Widget parent, 
				   Widget clipWindow, 
				   char *name, 
				   char *label,
				   int active ) : Cmd ( name, label, active )
{
    _attachArea = attachArea;
    _clipWindow = clipWindow;
    _parent     = parent;
}

void AttachSaveAsCmd::doit()
{
    FSState fsstate;
    Widget fsdialog;
    struct passwd *pwd;
    char *home;
    
    fsdialog = _attachArea->getFsDialog();
    if(fsdialog == NULL) {
	fsdialog = XmCreateFileSelectionDialog(_parent, "fsdialog", NULL, 0);
	XtUnmanageChild(
		XmFileSelectionBoxGetChild(fsdialog, XmDIALOG_HELP_BUTTON));
	XtVaSetValues(XmFileSelectionBoxGetChild(fsdialog, XmDIALOG_LIST),
	    XmNselectionPolicy, XmBROWSE_SELECT,
	    NULL);
	pwd = getpwuid(getuid());
	home = pwd->pw_dir;
	XtVaSetValues(fsdialog,
	    XtVaTypedArg, XmNdirectory, XtRString,
	    home, strlen(home)+1,
	    NULL);
	_attachArea->setFsDialog(fsdialog);
	XtAddCallback(fsdialog, XmNcancelCallback, 
			  &AttachSaveAsCmd::cancelCallback,
			  (XtPointer) this );
    }
    
    fsstate = _attachArea->getFsState();
    if(fsdialog) {
	switch(fsstate) {
	case SAVEAS:	// Do nothing
	    break;
	case ADD:
	    // Remove callbacks
	    XtRemoveAllCallbacks(fsdialog, XmNokCallback);
	    // Fall Through
	case NOTSET:
	    // Install callbacks
	    XtAddCallback(fsdialog, XmNokCallback, 
			      &AttachSaveAsCmd::okCallback,
			      (XtPointer) this );
	    break;
	}
	XtVaSetValues(XtParent(fsdialog),
		XmNtitle, GETMSG(DT_catd, 14, 11, "Save Attachment As"),
		NULL);
	XtManageChild(fsdialog);
    }
    XtVaSetValues(fsdialog, 
		    XmNfileTypeMask, XmFILE_DIRECTORY, 
		    NULL);
    _attachArea->setFsState(SAVEAS);
    _attachArea->activateDeactivate();
    XRaiseWindow( XtDisplay(fsdialog), XtWindow(XtParent(fsdialog)) );

}      

void AttachSaveAsCmd::undoit()
{
    // Just print a message that allows us to trace the execution
    
}       

void AttachSaveAsCmd::okCallback ( Widget w, XtPointer clientData, XtPointer callData )
{
    AttachSaveAsCmd *obj = (AttachSaveAsCmd *) clientData;

    obj->ok( callData );
    XtUnmanageChild( w );
}

void ConfirmCallback( XtPointer)
{
//     Attachment *attachment = (Attachment *)clientData;
//     int fd;
//     char error[1024];
//     AttachArea *attachArea = attachment->parent();
//     String filecontents;
//     unsigned long size;
//     ssize_t retval;
//     Boolean compressed;
// 
//     RoamMenuWindow *rmw = (RoamMenuWindow *)attachArea->owner()->owner();
//     
//     switch(attachment->get_ac_state()) {
//     case ROAM_LOCAL:
// 	break;
//     case ROAM_AVAILABLE:
//     case ROAM_CACHED:
// 
// 	attachment->setContents(NULL);
// // 			(char *)
// // 			attachArea->get_mailbox()->get_data(
// // 			rmw->msgno(),
// // 			attachment->getSection(),
// // 			&size,
// // 			&compressed));
// 	attachment->set_compressed(compressed);
// 	attachment->setFilesize( size );
// 	attachment->set_ac_state(ROAM_LOCAL);
// //	attachment->getIcon()->expose((XtPointer)NULL);
// 	break;
//     case ROAM_UNAVAILABLE:
// // 	theInfoDialogManager->post(
// // 	    GETMSG(DT_catd, 14, 15, "File Contents Unavailable"),
// // 	    (void *)NULL,
// // 	    (DialogCallback)okcb);
// 	    return;
//     }
//     (void)attachment->open_and_write(attachment->getSaveAsFilename());
}

void AttachSaveAsCmd::ok( XtPointer callData )
{
    XmFileSelectionBoxCallbackStruct *cbs = 
	    (XmFileSelectionBoxCallbackStruct *)callData;
    char *dirname = NULL;
//    struct stat s;
//     Attachment **list = _attachArea->getList();
//     char error[1024];
//     int i, fd;
//     SaveFileState sfs = OK;
//     char buf[1024];
//     String completefilename;
//     String filecontents;
//     unsigned long size;
//     ssize_t retval;
//     Boolean compressed;
// 
//	dirname = NULL;
//      dirname = (char *) _XmStringUngenerate(
//					cbs->value, NULL,
//					XmMULTIBYTE_TEXT, XmMULTIBYTE_TEXT);
//	if (NULL == dirname) return; // internal error
//     //
//     // If more than one attachment is selected then the selection
//     // specification must be a directory and must already exist.
//     //
//     //if(_attachArea->getIconSelectedCount() != 1) {
// 	// This should not be able to happen
// 	//printf("AttachSaveAsCmd::ok: getIconSelectedCount != 1. This can't happen\n");
//     //} else {
//     for(i=0;i<_attachArea->getIconCount();i++) {
// 	if(list[i]->getIcon()->isSelected() == TRUE) {
// 	//
// 	// If a directory then keep the same file name and write to 
// 	// that directory
// 	//
// 	// If it is a file that does not exist then we will try to
// 	// create it (later)
// 	//
// 	// If the file already exists then we must prompt the user
// 	// to see if he wants to overwrite the file.
// 	//
// 	    if(stat(dirname, &s) != -1) {	// The file already exists
// 		// This shouldn't happen because we make the fsdialog
// 		// show directories only.  However, the user could enter
// 		// the name of an existing file.
// 		if(S_ISREG(s.st_mode)) {
// 		    // If the user is trying to write multiple attachments
// 		    // to a single existing file then we barf!
// 		    if(_attachArea->getIconSelectedCount() != 1) {
// 			sprintf(error, 
//				GETMSG(DT_catd, 14, 16, "Cannot create"));
// 			sfs = CONFIRM;
// 		    } else {
// 			// The file already exists
// 			completefilename = dirname;
// 			sprintf(error, 
//				GETMSG(DT_catd, 14, 17,
//				    "File %s exists. Overwrite?"),
// 					    completefilename);
// 			sfs = CONFIRM;
// 		    }
// 		} else if(S_ISDIR(s.st_mode)) {
// 		    sfs = OK;
// 		    sprintf(buf, "%s%s", dirname, list[i]->getLabel());
// 		    completefilename = buf;
// 		    if(stat(buf, &s) != -1) {	// The file exists
// 			if(S_ISREG(s.st_mode)) {
// 			    sprintf(error, 
// 				GETMSG(DT_catd, 14, 18,
//		"File %s exists. Overwrite?"), 
// 				completefilename);
// 			    sfs = CONFIRM;
// 			} else {
// 			    // We're trying to overwrite something other
// 			    // than a regular file. FAIL!
// 			    sprintf(error, 
//		GETMSG(14, 19, "Cannot create"));
// 			    sfs = CONFIRM;
// 			}
// 		    }
// 		} else {
// 		    // The file already exists but is not a regular file
// 		    completefilename = dirname;
// 		    sprintf(error,
// 			    GETMSG(
//    DT_catd, 14, 20, "Cannot create %s"), completefilename);
// 		    sfs = ERR;
// 		}
// 	    } else {	// The file does not exist. Maybe the user entered
// 			// a new file name in an existing directory.
// 		completefilename = dirname;
// 	    }
// 	    list[i]->setSaveAsFilename(strdup(completefilename));
// 	    switch(sfs) {
// 	    case ERR:
// // 		theInfoDialogManager->post(
// // 		    error,
// // 		    (void *)NULL,
// // 		    (DialogCallback)okcb);
// 		break;
// 	    case CONFIRM:
// // 		theQuestionDialogManager->post(error,
// // 					       (XtPointer)list[i],
// // 					       ConfirmCallback,
// // 					       okcb	// Cancel button
// // 					       );
// 		break;
// 	    case OK:
// 		RoamMenuWindow *rmw = 
// 		    (RoamMenuWindow*)_attachArea->owner()->owner();
// 
// 		switch(list[i]->get_ac_state()) {
// 		case ROAM_LOCAL:
// 		    break;
// 		case ROAM_CACHED:
// 		case ROAM_AVAILABLE:
// 		    list[i]->setContents(NULL);
// 
// // 			(char * )_attachArea->get_mailbox()->get_data(
// // 				rmw->msgno(),
// // 				list[i]->getSection(),
// // 				&size,
// // 				&compressed));
// 
// 		    list[i]->set_compressed(compressed);
// 		    list[i]->setFilesize( size );
// 		    list[i]->set_ac_state(ROAM_LOCAL);
// //		    list[i]->getIcon()->expose((XtPointer)NULL);
// 		    break;
// 		case ROAM_UNAVAILABLE:
// // 		    theInfoDialogManager->post(
// // 			GETMSG(DT_catd, 14. 21, "File Contents Unavailable"),
// // 			(void *)NULL, (DialogCallback)okcb);
// 			return;
// 		}
// 		(void)list[i]->open_and_write(completefilename);
// 		break;
// 	    }
// 	}
//     }
//     XtFree(dirname);
}

void AttachSaveAsCmd::cancelCallback ( Widget, XtPointer clientData, XtPointer callData )
{
    AttachSaveAsCmd *obj = (AttachSaveAsCmd *) clientData;

    obj->cancel( callData );
}

void AttachSaveAsCmd::cancel( XtPointer )
{
    XtUnmanageChild(_attachArea->getFsDialog());
}

AttachSelectAllCmd::AttachSelectAllCmd ( AttachArea *attachArea, 
					 char *name, 
					 char *label,
					 int active ) 
				: Cmd ( name, label, active )
{
    _attachArea = attachArea;
}

void AttachSelectAllCmd::doit()
{
//    int i;
//    Boolean disconnected;

//    Attachment **list = _attachArea->getList();
// //    disconnected = _attachArea->get_mailbox()->get_disconnected();
//     for(i=0;i<_attachArea->getIconCount();i++) {
// 	// Select the icon if and only if it is currently:
// 	//     1) not selected, 2) not deleted, and 3) not disconnected
// 	if(list[i]->getIcon()->isSelected() == FALSE &&
// 			list[i]->isDeleted() == FALSE && 
// 			list[i]->get_ac_state() != ROAM_UNAVAILABLE) {
// 		list[i]->getIcon()->select();
// 	}
//     }
}

void AttachSelectAllCmd::undoit()
{
    // Just print a message that allows us to trace the execution
    
}       

AttachUndeleteCmd::AttachUndeleteCmd ( AttachArea *attachArea, 
				       char *name, 
				       char *label,
				       int active ) 
			: Cmd ( name, label, active )
{
    _attachArea = attachArea;
}

void AttachUndeleteCmd::doit()
{
//    int i;

//     Attachment **list = _attachArea->getList();
//     for(i=0;i<_attachArea->getIconCount();i++)
// 	if(list[i]->isDeleted() == TRUE) {
// 	    list[i]->manage();
// 	    _attachArea->decDeleteCount();
// 	    _attachArea->setAttachmentsLabel();
// 	    list[i]->unDeleteIt();
// 	}
// 
//     _attachArea->activateDeactivate();
//     _attachArea->CalcAllAttachmentPositions();
//     _attachArea->CalcLastRow();
//     _attachArea->AdjustCurrentRow();
//     _attachArea->SetScrollBarSize(_attachArea->getLastRow()+1);
//     _attachArea->DisplayAttachmentsInRow(_attachArea->getCurrentRow());
}      

void AttachUndeleteCmd::undoit()
{
    // Just print a message that allows us to trace the execution
    
}       

AttachUnselectAllCmd::AttachUnselectAllCmd ( AttachArea *attachArea, 
					     char *name, 
					     char *label,
					     int active ) 
			: Cmd ( name, label, active )
{
    _attachArea = attachArea;
}

void AttachUnselectAllCmd::doit()
{
//    int i;

//     Attachment **list = _attachArea->getList();
//     for(i=0;i<_attachArea->getIconCount();i++)
// 	if(list[i]->getIcon()->isSelected() == TRUE)
// 	    list[i]->getIcon()->unselect();
}      

void AttachUnselectAllCmd::undoit()
{
    // Just print a message that allows us to trace the execution
    
}       

AttachInfoCmd::AttachInfoCmd ( AttachArea *attachArea, 
			       char *name, 
			       char *label,
			       int active ) 
			: Cmd ( name, label, active )
{
    _attachArea = attachArea;
    _info_dialog = NULL;
    _attachInfoDialogManager = NULL;
}

void AttachInfoCmd::doit()
{

    // Stubbed out. 

//     ViewAttachArea *vaa = (ViewAttachArea *)_attachArea;
//     int i;
// 
//     Attachment **list = _attachArea->getList();
//     for(i=0;i<_attachArea->getIconCount();i++)
// 	if(list[i]->getIcon()->isSelected() == TRUE) {
// 	    vaa->show_info(list[i]);
// 	    break;
// 	}

}

void AttachInfoCmd::undoit()
{
    // Just print a message that allows us to trace the execution
    
}       

AttachInfoCmd::~AttachInfoCmd()
{
    if(_info_dialog)
	XtDestroyWidget(_info_dialog);
}

static void okcb( XtPointer )
{
    //Empty
    // This function exists so that the OK button will appear on the
    // Info Dialog. It doesn't have to do anything because the dialog
    // automatically pops down. It is for information only.
}
