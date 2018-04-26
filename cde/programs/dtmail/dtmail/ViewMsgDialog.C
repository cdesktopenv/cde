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
 *	$TOG: ViewMsgDialog.C /main/6 1998/01/28 18:35:43 mgreess $
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

#include <assert.h>
#include <Xm/Label.h>
#include <Xm/ScrolledW.h>
#include <Xm/DialogS.h>
#include <Xm/Form.h>
#include <Xm/Text.h>
#include <Xm/Protocols.h>
#include <Xm/AtomMgr.h>
#include <Xm/PushB.h>
#include <Xm/MainW.h>

#include "MainWindow.h"
#include "ViewMsgDialog.h"
#include "RoamMenuWindow.h"
#include "MenuBar.h"
#include "RoamApp.h"
#include "MailMsg.h"
#include "Attachment.h"
#include "Help.hh"
#include "Help.hh"
#include "DtMailHelp.hh"

// Need the XmTextEditor.  Will eventually need the HP-Editor here!
//#include "XmTextEditor.h"

#include "DtMailEditor.hh"

extern nl_catd	DtMailMsgCat;

//#include "CDEM_CoeEd.hh"


// The WM_CLASS_NAME of a VMD is DtMail_SecWin.
// SQE's  test suites depend on the WM_CLASS_NAME being a 
// constant.  If you are going to change the name, talk to SQE
// and get their consent for whatever change you are making.

ViewMsgDialog::ViewMsgDialog (RoamMenuWindow *parent, WidgetClass wc) :
DialogShell ("DtMail_SecWin", parent, wc)
{
    _parent = parent;
    _workArea = NULL;
    _menuBar = NULL;
    my_editor = NULL;
    _genDialog  = NULL;

    _edit_copy = NULL;
    _edit_select_all = NULL;
    
    _attach_save_as = NULL;
    _attach_select_all = NULL;

    _attachmentActionsList = NULL;
    _attachmentMenu = NULL;
    _attachmentMenuList = NULL;
    _attachmentPopupMenuList = NULL;
    _textPopupMenuList = NULL;

    _main = NULL;
    _overview = NULL;
    _tasks = NULL;
    _reference = NULL;
    _on_item = NULL;
    _using_help = NULL;
    _about_mailer = NULL;
    _text = NULL;
    _msgno = NULL;

    assert( theApplication != NULL );

}


ViewMsgDialog::~ViewMsgDialog()
{

    delete _menuBar;
    delete	_genDialog;

    delete _edit_copy;
    delete _edit_select_all;

    delete _attach_save_as;
    delete _attach_select_all;
    
    delete _attachmentActionsList;
    delete _attachmentMenuList;
    delete _attachmentPopupMenuList;
    delete _textPopupMenuList;

    delete my_editor;

}


Widget 
ViewMsgDialog::createWorkArea ( Widget shell )
{

    Widget form = XtCreateWidget(
			"Work_Area", xmFormWidgetClass,
			shell, NULL, 0);

    _main = XtVaCreateManagedWidget ("separateViewWindow", 
		xmMainWindowWidgetClass,
		form, 
		XmNleftAttachment, XmATTACH_FORM,
		XmNtopAttachment, XmATTACH_FORM,
		XmNbottomAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		NULL);

    Widget form1 = XtCreateWidget("Work_Area", xmFormWidgetClass,
 				  _main, NULL, 0);
    printHelpId("form", form1);
    /* add help callback */
    // XtAddCallback(form1, XmNhelpCallback, HelpCB, helpId);


    my_editor = new DtMailEditor(form1, this);
    my_editor->initialize();

    my_editor->attachArea()->setOwnerShell(this);

    Widget wid = my_editor->container();

    XtVaSetValues( wid,
  	XmNrightAttachment,XmATTACH_FORM, 
  	XmNleftAttachment,XmATTACH_FORM, 
  	XmNtopAttachment,XmATTACH_FORM, 
  	XmNbottomAttachment, XmATTACH_FORM,
  	NULL );

    XtManageChild(form1);

    return form;
}				 

void
ViewMsgDialog::propsChanged(void)
{
}

void
ViewMsgDialog::text(const char *text, const unsigned long len)
{

    my_editor->textEditor()->set_contents(text, len);

}

void
ViewMsgDialog::append( const char *text, const unsigned long len ){
  
    my_editor->textEditor()->append_to_contents(text, len);
}

void 
ViewMsgDialog::popped_down()
{
    delete this;
}


void
ViewMsgDialog::popped_up()
{
    Widget text;
    Dimension width, height;

    text = _parent->get_editor()->container();
    
    XtVaGetValues(text,
	XmNwidth, &width,
	XmNheight, &height,
	NULL);

    XtVaSetValues(_w,
	XmNx, _parent->x() + (_parent->width() / 2),
	XmNy, _parent->y() + (_parent->height() /2),
	NULL );

}


// SR - added the messages below

void
ViewMsgDialog::initialize()
{

    DialogShell::initialize();

    _menuBar = new MenuBar ( _main, "menubar" );

    XtVaSetValues ( _main, 
		   XmNmenuBar, _menuBar->baseWidget(),
		   NULL);

    this->construct_edit_menu();
    this->construct_text_popup();
    this->construct_attachment_menu();
    this->construct_attachment_popup();
    this->construct_help_menu();

    _menuBar->manage();

    _genDialog = new DtMailGenDialog("Dialog", _main);

    XtManageChild(_main);
}


void 
ViewMsgDialog::quit()
{
    _parent->unregisterDialog(this);
    delete this;
}

void 
ViewMsgDialog::raise()
{
    XRaiseWindow(XtDisplay(_w), XtWindow(_w));
}

DtMailEditor* 
ViewMsgDialog::get_editor()
{

    return(my_editor);

}

void
ViewMsgDialog::auto_show_cursor_off()
{
    
    my_editor->textEditor()->auto_show_cursor_off();

}

void
ViewMsgDialog::auto_show_cursor_restore()
{

    my_editor->textEditor()->auto_show_cursor_restore();

}

void
ViewMsgDialog::set_to_top()
{

    my_editor->textEditor()->set_to_top();

}

void
ViewMsgDialog::text_selected()
{

    // turn on sensitivity for Cut/Clear/Copy/Paste/Delete

}

void
ViewMsgDialog::text_unselected()
{

    // turn off sensitivity for those items

}


void
ViewMsgDialog::attachment_selected()
{
    _attach_save_as->activate();
}


void
ViewMsgDialog::all_attachments_selected()
{
    _attach_save_as->deactivate();
    if (_attachmentActionsList != NULL) {
	_menuBar->removeCommands(_attachmentMenu, _attachmentActionsList);
	_menuPopupAtt->removeCommands(_attachmentPopupMenu, 
				_attachmentActionsList);
	delete _attachmentActionsList;
	_attachmentActionsList = NULL;
    }
}

void
ViewMsgDialog::all_attachments_deselected()
{
    _attach_save_as->deactivate();
    if (_attachmentActionsList != NULL) {
	_menuBar->removeCommands(_attachmentMenu, _attachmentActionsList);
	_menuPopupAtt->removeCommands(_attachmentPopupMenu, 
			_attachmentActionsList);
	delete _attachmentActionsList;
	_attachmentActionsList = NULL;
    }
}

void
ViewMsgDialog::construct_edit_menu()
{

    CmdList *cmdList;
    
    _edit_copy = new EditCopyCmd(
			"Copy",
			GETMSG(DT_catd, 1, 161, "Copy"), 
			TRUE, 
			this
		     );

    _edit_select_all = new EditSelectAllCmd(
			"Select All",
			GETMSG(DT_catd, 1, 162, "Select All"), 
			TRUE,
			this
		    );

    cmdList = new CmdList("Edit", GETMSG(DT_catd, 1, 163, "Edit"));
    cmdList->add(_edit_copy);
    cmdList->add(_edit_select_all);
    
    _menuBar->addCommands(cmdList);

}

void
ViewMsgDialog::construct_attachment_menu()
{

    // Separator for menu items
    SeparatorCmd *separator = new SeparatorCmd("Separator", "Separator", TRUE );

    _attach_save_as	= new SaveAttachCmd (
				"Save As...",
				GETMSG(DT_catd, 1, 164, "Save As..."),
				GETMSG(DT_catd, 1, 165, "Mailer - Attachments - Save As"),
				FALSE, 
				ViewMsgDialog::save_attachment_callback,
				this,
				this->baseWidget());

    _attach_select_all = new SelectAllAttachsCmd(
				"Select All",
				GETMSG(DT_catd, 1, 166, "Select All"), 
				this);

    _attachmentMenuList = new CmdList( 
				"Attachments",
				GETMSG(DT_catd, 1, 167, "Attachments") );

    _attachmentMenuList->add(_attach_save_as);
    _attachmentMenuList->add(_attach_select_all);

    _attachmentMenu = _menuBar->addCommands(_attachmentMenuList);

}

void
ViewMsgDialog::construct_attachment_popup(void)
{
   _attachmentPopupMenuList = new CmdList( "AttachmentsPopup", "AttachmentsPopup");

    LabelCmd *title     = new LabelCmd (
			"Mailer - Attachments",
			GETMSG(DT_catd, 1, 168, "Mailer - Attachments"), TRUE);
    SeparatorCmd *separator = new SeparatorCmd("Separator","Separator", TRUE );

    _attachmentPopupMenuList->add(title);
    _attachmentPopupMenuList->add(separator);
    _attachmentPopupMenuList->add(_attach_save_as);
    _attachmentPopupMenuList->add(_attach_select_all);
    _menuPopupAtt = new MenuBar(my_editor->attachArea()->getClipWindow(), 
					"ViewMsgAttachmentPopup", XmMENU_POPUP);
    _attachmentPopupMenu = _menuPopupAtt->addCommands(_attachmentPopupMenuList, 
				FALSE, XmMENU_POPUP);
}

void
ViewMsgDialog::construct_text_popup(void)
{
   if (theApplication->bMenuButton() != Button3)
	return;

   _textPopupMenuList = new CmdList("TextPopup", "TextPopup");

    LabelCmd *title     = new LabelCmd (
			"Mailer - Text",
			GETMSG(DT_catd, 1, 169, "Mailer - Text"), TRUE);
    SeparatorCmd *separator = new SeparatorCmd("Separator", "Separator", TRUE );

    _textPopupMenuList->add(title);
    _textPopupMenuList->add(separator);
    _textPopupMenuList->add(_edit_copy);
    _textPopupMenuList->add(_edit_select_all);

    Widget parent = my_editor->textEditor()->get_editor();
    _menuPopupText = new MenuBar(parent, "ViewMsgTextPopup", XmMENU_POPUP);
    _textPopupMenu = _menuPopupText->addCommands(_textPopupMenuList, 
				FALSE, XmMENU_POPUP);
}



void
ViewMsgDialog::construct_help_menu()
{
    CmdList * cmdList;

    // Separator for menu items
    
    SeparatorCmd *separator= new SeparatorCmd("Separator", "Separator", TRUE );
    _overview = new OnAppCmd("Overview", GETMSG(DT_catd, 1, 170, "Overview"), 
				TRUE, this);
    _tasks = new TasksCmd("Tasks", GETMSG(DT_catd, 1, 171, "Tasks"), 
				TRUE, this);
    _reference = new ReferenceCmd("Reference",
				GETMSG(DT_catd, 1, 172, "Reference"), 
				TRUE, this);
    _on_item = new OnItemCmd("On Item",
				GETMSG(DT_catd, 1, 173, "On Item"), 
				TRUE, this);
    _using_help = new UsingHelpCmd("Using Help",
				GETMSG(DT_catd, 1, 174, "Using Help"), 
				TRUE, this);
    cmdList = new CmdList( "Help", GETMSG(DT_catd, 1, 175, "Help") );
    cmdList->add ( _overview );
    cmdList->add ( separator );
    cmdList->add ( _tasks );
    cmdList->add ( _reference );
    cmdList->add ( separator );
    cmdList->add ( _on_item );
    cmdList->add ( separator );
    cmdList->add ( _using_help );
    cmdList->add ( separator );

    _about_mailer = new RelNoteCmd("About Mailer",
				    GETMSG(DT_catd, 1, 176, "About Mailer..."),
    				    TRUE, this);
    cmdList->add ( _about_mailer );

    // Make help menu show up on right side of menubar.
    _menuBar->addCommands ( cmdList, TRUE );
    
}

void
ViewMsgDialog::save_attachment_callback(
    void *client_data,
    char *selection
)
{
    DtMailEnv mail_error;

    // Initialize the mail_error.

    mail_error.clear();

    ViewMsgDialog *obj = (ViewMsgDialog *) client_data;

    obj->save_selected_attachment(selection);

}

void
ViewMsgDialog::save_selected_attachment(
    char *selection
)
{
   
    DtMailEnv mail_error;
    
    mail_error.clear();
    DtMailEditor *editor = this->get_editor();
    AttachArea *attacharea = editor->attachArea();
    Attachment *attachment = attacharea->getSelectedAttachment();

   // Get selected attachment, if none selected, then return.
   if ( attachment == NULL ) {
	  // Let User know that no attachment has been selected???
	  int answer = 0;
	  char *helpId = NULL;


	  _genDialog->setToErrorDialog(
		    GETMSG(DT_catd, 1, 177, "Mailer"),		  
		    GETMSG(DT_catd, 2, 20, "An attachment needs to be selected before issuing the\n\"Save As\" command to save to a file.") );
	  helpId = DTMAILHELPSELECTATTACH;
	  answer = _genDialog->post_and_return(
			GETMSG(DT_catd, 3, 75, "OK"), helpId );
	  return;
      }

    attachment->saveToFile(mail_error, selection);

    if (mail_error.isSet()) {
	// do something
    }

}

void
ViewMsgDialog::manage()
{

    Widget text;
    Dimension width, height;

    text = _parent->get_editor()->container();
    
    XtVaGetValues(text,
	XmNwidth, &width,
	XmNheight, &height,
	NULL);

    XtVaSetValues(_w,
	XmNx, _parent->x() + (_parent->width() / 2),
	XmNy, _parent->y() + (_parent->height() /2),
	NULL );

    DialogShell::manage();

}

void
ViewMsgDialog::addAttachmentActions(
    char **actions,
    int indx
)
{
    int i;
    char *anAction;
    AttachmentActionCmd *attachActionCmd;
    
    if (_attachmentActionsList == NULL) { 
	_attachmentActionsList = new CmdList("AttachmentActions",
					     "AttachmentActions");
    }
    else {
	_menuBar->removeCommands(_attachmentMenu, _attachmentActionsList);
	_menuPopupAtt->removeCommands(_attachmentPopupMenu, 
					_attachmentActionsList);
	delete _attachmentActionsList;
	_attachmentActionsList = new CmdList("AttachmentActions",
					     "AttachmentActions");
    }

    char *actionLabel;
    for (i = 0; i < indx; i++) {
	anAction = actions[i];
	actionLabel = DtActionLabel(anAction);  // get the localized action label
	attachActionCmd = new AttachmentActionCmd(
					anAction,
  				        actionLabel,
					this,
					i);
	_attachmentActionsList->add(attachActionCmd);
	
    }
    _attachmentMenu = _menuBar->addCommands(
				_attachmentMenu, 
				_attachmentActionsList
			);

    _attachmentPopupMenu = _menuPopupAtt->addCommands(
				_attachmentPopupMenu, 
				_attachmentActionsList);

}


void
ViewMsgDialog::removeAttachmentActions()
{

    // Stubbed out for now
}

void
ViewMsgDialog::invokeAttachmentAction(
    int index
)
{
    DtMailEditor *editor = this->get_editor();
    AttachArea *attacharea = editor->attachArea();
    Attachment *attachment = attacharea->getSelectedAttachment();

    attachment->invokeAction(index);

}

void
ViewMsgDialog::selectAllAttachments()
{
    
    DtMailEditor *editor = this->get_editor();
    AttachArea *attachArea = editor->attachArea();
    
    attachArea->selectAllAttachments();

}

void
ViewMsgDialog::activate_default_attach_menu()
{
    _attach_select_all->activate();
}

void
ViewMsgDialog::deactivate_default_attach_menu()
{
    _attach_select_all->deactivate();
}


void
ViewMsgDialog::showAttachArea()
{
    DtMailEditor *editor = this->get_editor();
    editor->showAttachArea();
}

void
ViewMsgDialog::hideAttachArea()
{
    DtMailEditor *editor = this->get_editor();
    editor->hideAttachArea();
}


void
ViewMsgDialog::attachmentFeedback(
    Boolean bval
)
{
    if (bval) {
	this->busyCursor();
    }
    else {
	this->normalCursor();
    }
}
