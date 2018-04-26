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
/* $TOG: OptCmd.C /main/20 1999/07/13 08:40:16 mgreess $ */
/*
 *+SNOTICE
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement bertween
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel wihtout
 *	Sun's specific written approval.  This documment and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */
/*
 *		     Common Desktop Environment
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *   (c) Copyright 1995 Digital Equipment Corp.
 *   (c) Copyright 1995 Fujitsu Limited
 *   (c) Copyright 1995 Hitachi, Ltd.
 *                                                                   
 *
 *                     RESTRICTED RIGHTS LEGEND                              
 *
 *Use, duplication, or disclosure by the U.S. Government is subject to
 *restrictions as set forth in subparagraph (c)(1)(ii) of the Rights in
 *Technical Data and Computer Software clause in DFARS 252.227-7013.  Rights
 *for non-DOD U.S. Government Departments and Agencies are as set forth in
 *FAR 52.227-19(c)(1,2).

 *Hewlett-Packard Company, 3000 Hanover Street, Palo Alto, CA 94304 U.S.A.
 *International Business Machines Corp., Route 100, Somers, NY 10589 U.S.A. 
 *Sun Microsystems, Inc., 2550 Garcia Avenue, Mountain View, CA 94043 U.S.A.
 *Novell, Inc., 190 River Road, Summit, NJ 07901 U.S.A.
 *Digital Equipment Corp., 111 Powdermill Road, Maynard, MA 01754, U.S.A.
 *Fujitsu Limited, 1015, Kamikodanaka Nakahara-Ku, Kawasaki 211, Japan
 *Hitachi, Ltd., 6, Kanda Surugadai 4-Chome, Chiyoda-ku, Tokyo 101, Japan
 */
///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//         This example code is from the book:
//
//           Object-Oriented Programming with C++ and OSF/Motif
//         by
//           Douglas Young
//           Prentice Hall, 1992
//           ISBN 0-13-630252-1	
//
//
//  Permission to use, copy, modify, and distribute this software for 
//  any purpose except publication and without fee is hereby granted, provided 
//  that the above copyright notice appear in all copies of the software.
///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
// OptCmd.C : creates the mail options dialog
//////////////////////////////////////////////////////////
#include <stdlib.h>
#include <sys/types.h>
#include <nl_types.h>
#include <pwd.h>
#include <unistd.h>

#include <X11/IntrinsicP.h>

#include "Application.h"
#include "DmxPrintOptions.h"
#include "DtMailHelp.hh"
#include "Help.hh"
#include "MailMsg.h"
#include "MailRetrievalOptions.h"
#include "MainWindow.h"
#include "RoamApp.h"
#include "RoamCmds.h"
#include "dtmailopts.h"
#include "options_ui.h"

#include <DtMail/PropUi.hh>
#include <DtMail/CheckForMailUiItem.hh>
#include <DtMail/SpinBoxUiItem.hh>
#include <DtMail/TextFieldUiItem.hh>
#include <DtMail/CheckBoxUiItem.hh>
#include <DtMail/AntiCheckBoxUiItem.hh>
#include <DtMail/ListUiItem.hh>
#include <DtMail/IgnoreListUiItem.hh>
#include <DtMail/MoveMenuListUiItem.hh>
#include <DtMail/AlternatesListUiItem.hh>
#include <DtMail/CustomListUiItem.hh>
#include <DtMail/TemplateListUiItem.hh>
#include <DtMail/AliasListUiItem.hh>
#include <DtMail/ScaleUiItem.hh>
#include <DtMail/OptCmd.h>
#include <DtMail/DtVirtArray.hh>
#include <DtMail/options_util.h>
#include <DtMail/Common.h>

nl_catd Dtb_project_catd;
extern Boolean props_changed;

#include "MailMsg.h"
static Dimension max_width = 0;
static Dimension max_height = 0;


OptCmd::OptCmd ( char *name, char *label, int active, Widget parent) : 
                 WarnNoUndoCmd ( name, label, active ) 
{
    _dialogParentWidget = parent;

    _reopen_mailboxes = FALSE;
    vacation_status_changed = FALSE;
    vacation_msg_changed = FALSE;

    _opt_dlog = NULL;
    catstr = NULL;
    CurrentPane = NULL;

    mh_pane_list = NULL;
    msg_view_pane_list = NULL;
    compose_pane_list = NULL;
    msg_filing_pane_list = NULL;
    vacation_pane_list = NULL;
    template_pane_list = NULL;
    aliases_pane_list = NULL;
    advanced_pane_list = NULL;
    mailretrieval_pane_list = NULL;
    vaca_sub = NULL;
    vaca_msg = NULL;
    generalDialog = NULL;
    _PrintingOptions = NULL;
    _MailRetrievalOptions = NULL;
    printing_pane_list = NULL;
}

void
OptCmd::help_button_CB(Widget current_pane)
{
    if (current_pane == _opt_dlog->msg_header_pane)
	DisplayMain(current_pane, "Mailer", "MessageHeaderListOptions");
    else if (current_pane == _opt_dlog->msg_view_pane)
	DisplayMain(current_pane, "Mailer", "MessageViewOptions");
    else if (current_pane == _opt_dlog->compose_win_pane)
	DisplayMain(current_pane, "Mailer", "ComposeWindowOptions");
    else if (current_pane == _opt_dlog->msg_filing_pane)
	DisplayMain(current_pane, "Mailer", "MessageFilingOptions");
    else if (current_pane == _opt_dlog->vacation_pane)
	DisplayMain(current_pane, "Mailer", "VacationMailOptions");
    else if (current_pane == _opt_dlog->templates_pane)
	DisplayMain(current_pane, "Mailer", "MailTemplateOptions");
    else if (current_pane == _opt_dlog->aliases_pane)
	DisplayMain(current_pane, "Mailer", "MailAliasOptions");
    else if (current_pane == _opt_dlog->advanced_pane)
	DisplayMain(current_pane, "Mailer", "AdvancedMailOptions");
    else if (current_pane == _opt_dlog->printing_pane)
	DisplayMain(current_pane, "Mailer", "PrintingMailOptions");
    else if (current_pane == _opt_dlog->printing_pane)
	DisplayMain(current_pane, "Mailer", "MailRetrievalOptions");
}

void
opt_help_button_CB(Widget wid, XtPointer clientData, XtPointer calldata)
{
    OptCmd *cmd_ptr = (OptCmd*)clientData;

    // Suppress warnings
    wid = wid;
    calldata = calldata;

    cmd_ptr->help_button_CB(cmd_ptr->currentPane());
}

void OptCmd::create_dlog()
{
    theRoamApp.busyAllWindows(
	           GETMSG(DT_catd, 3, 78, "Initializing Mail Options..."));

    // make a new widget structure
    _opt_dlog = (DtbOptionsDialogInfo)malloc(sizeof(
	                       DtbOptionsDialogInfoRec));
	
    // make sure it's all cleaned up
    dtbOptionsDialogInfo_clear(_opt_dlog);

    /*
     * Open the standard message catalog for the project.
     */
    Dtb_project_catd = catopen(DTB_PROJECT_CATALOG, NL_CAT_LOCALE);

#ifdef NEVER
    // Not a fatal error
    if (Dtb_project_catd == (nl_catd)-1)
      {
        fprintf(stderr, "Could not open message catalog: %s\n",
		DTB_PROJECT_CATALOG);
	exit(1);
      }
#endif

    // make us some a dem widgets!  :-)
    dtb_options_dialog_initialize(_opt_dlog,
				  _dialogParentWidget);
    //
    // The Printing Options pane is handled outside the
    // Builder generated code.
    //
    _PrintingOptions = new DmxPrintOptions(_opt_dlog->layer);
    _opt_dlog->printing_pane = _PrintingOptions->baseWidget();

    //
    // The Mail Server Options pane is handled outside the
    // Builder generated code.
    //
    _MailRetrievalOptions = new MailRetrievalOptions(_opt_dlog->layer);
    _opt_dlog->mailretrieval_pane = _MailRetrievalOptions->baseWidget();


    // We need to do this for the help system
    dtb_save_toplevel_widget(_dialogParentWidget); 

    XtSetMappedWhenManaged(_opt_dlog->dialog, False);
    XtManageChild(_opt_dlog->dialog_shellform);

    // hook them up to the back end!
    this->init_panes();

    // Attach the object to the mail options panes so they can later be referred
    // to in options_set_category_pane().
    XtVaSetValues(_opt_dlog->msg_header_pane, XmNuserData, this, NULL);
    XtVaSetValues(_opt_dlog->msg_view_pane, XmNuserData, this, NULL);
    XtVaSetValues(_opt_dlog->compose_win_pane, XmNuserData, this, NULL);
    XtVaSetValues(_opt_dlog->msg_filing_pane, XmNuserData, this, NULL);
    XtVaSetValues(_opt_dlog->vacation_pane, XmNuserData, this, NULL);
    XtVaSetValues(_opt_dlog->templates_pane, XmNuserData, this, NULL);
    XtVaSetValues(_opt_dlog->aliases_pane, XmNuserData, this, NULL);
    XtVaSetValues(_opt_dlog->advanced_pane, XmNuserData, this, NULL);
    XtVaSetValues(_opt_dlog->printing_pane, XmNuserData, this, NULL);
    XtVaSetValues(_opt_dlog->mailretrieval_pane, XmNuserData, this, NULL);

    // initialize the "current" pane
    options_set_category_pane(_opt_dlog->msg_header_pane);

    XtAddCallback(_opt_dlog->subject_tf,
		  XmNvalueChangedCallback,
		  vaca_msg_changedCB,
		  (XtPointer)this);
    
    XtAddCallback(_opt_dlog->vacation_msg_tp,
		  XmNvalueChangedCallback,
		  vaca_msg_changedCB,
		  (XtPointer)this);

     XtAddCallback(_opt_dlog->help_button, XmNactivateCallback, 
		opt_help_button_CB,	(XtPointer)this);
     XtAddCallback(_opt_dlog->ctrl_panel, XmNhelpCallback, 
		opt_help_button_CB,	(XtPointer)this);
     XtAddCallback(_opt_dlog->msg_header_pane, XmNhelpCallback, 
		opt_help_button_CB,	(XtPointer)this);
     XtAddCallback(_opt_dlog->msg_view_pane, XmNhelpCallback, 
		opt_help_button_CB,	(XtPointer)this);
     XtAddCallback(_opt_dlog->compose_win_pane, XmNhelpCallback, 
		opt_help_button_CB,	(XtPointer)this);
     XtAddCallback(_opt_dlog->msg_filing_pane, XmNhelpCallback, 
		opt_help_button_CB,	(XtPointer)this);
     XtAddCallback(_opt_dlog->vacation_pane, XmNhelpCallback, 
		opt_help_button_CB,	(XtPointer)this);
     XtAddCallback(_opt_dlog->templates_pane, XmNhelpCallback, 
		opt_help_button_CB,	(XtPointer)this);
     XtAddCallback(_opt_dlog->aliases_pane, XmNhelpCallback, 
		opt_help_button_CB,	(XtPointer)this);
     XtAddCallback(_opt_dlog->advanced_pane, XmNhelpCallback, 
		opt_help_button_CB,	(XtPointer)this);
     XtAddCallback(_opt_dlog->dialog_button_panel, XmNhelpCallback, 
		opt_help_button_CB,	(XtPointer)this);
     XtAddCallback(_opt_dlog->printing_pane, XmNhelpCallback, 
		opt_help_button_CB,	(XtPointer)this);
     XtAddCallback(_opt_dlog->mailretrieval_pane, XmNhelpCallback, 
		opt_help_button_CB,	(XtPointer)this);

     theRoamApp.unbusyAllWindows();
     theRoamApp.setOptionsDialog((char*)_opt_dlog);
     props_changed = FALSE;
}

//////////////////////////////////////////////////////////////////////////////
void vaca_msg_changedCB(Widget w, XtPointer ClientData, XtPointer calldata)
{
  OptCmd *cmd_ptr = (OptCmd*)ClientData;
  
  w = w;
  calldata = calldata;

  cmd_ptr->set_vaca_msg_Changed();

}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void
options_set_category_pane(
    Widget	pane
)
{
  char buf[1024];
  OptCmd *cmd_ptr;
  Widget current_pane;
  int answer;

  XtVaGetValues(pane,
	XmNuserData, &cmd_ptr,
	NULL);

  current_pane = cmd_ptr->currentPane();
  if (pane == current_pane)
	return;

  if (current_pane != NULL && props_changed) {
        sprintf(buf, "%s", GETMSG(DT_catd, 15, 1, "You have made unsaved changes.\nYou may save your changes, discard your changes,\nor return to your previous place in the dialog."));
 
        cmd_ptr->genDialog()->setToQuestionDialog(
			GETMSG(DT_catd, 5, 2, "Mailer"),
			buf);
 
        answer = cmd_ptr->genDialog()->post_and_return(
			GETMSG(DT_catd, 15, 2, "Save"),
                        GETMSG(DT_catd, 1, 182, "Cancel"),
                        GETMSG(DT_catd, 15, 3, "Discard"),
                        DTMAILHELPERROR);
 
        if (answer == 1) { //Save
		if (! cmd_ptr->optionsAreValid()) return;
		cmd_ptr->update_source();
		cmd_ptr->doVacationCheck();
        }
        else if (answer == 3) { // Discard
		cmd_ptr->update_panes();
        }
        else if (answer == 2) { // Cancel
                //props_changed = FALSE;
                cmd_ptr->setOldCategoryStr();
                return;
        }
  }
  props_changed = FALSE;
  XtManageChild(pane);
 
  if (current_pane != NULL)
    XtUnmanageChild(current_pane);
 
  cmd_ptr->setCurrentPane(pane);
  cmd_ptr->setNewCategoryStr();
}

//////////////////////////////////////////////////////////////////////////////
OptCmd::~OptCmd()
{
//  free(_opt_dlog);

  _opt_dlog = NULL;

}
//////////////////////////////////////////////////////////////////////////////
// hookup the front and back ends of the mail options
void OptCmd::init_panes()
{

  this->init_msg_hdr_pane();
  this->init_msg_view_pane();
  this->init_compose_pane();
  this->init_msg_filing_pane();
  this->init_vacation_pane();
  this->init_template_pane();
  this->init_aliases_pane();
  this->init_advanced_pane();
  this->init_printing_pane();
  this->init_mailretrieval_pane();

}

//////////////////////////////////////////////////////////////////////////////
void OptCmd::update_panes()
{
  VacationCmd *vacation_cmd = theRoamApp.vacation();
  Boolean vaca_running = FALSE;

  char *tmp_ptr;

  Arg args[1];

  if(vacation_cmd != NULL)
    vaca_running = vacation_cmd->priorVacationRunning();

  this->updateUiFromBackEnd(mh_pane_list);    
  this->updateUiFromBackEnd(msg_view_pane_list);    
  this->updateUiFromBackEnd(compose_pane_list);    
  this->updateUiFromBackEnd(msg_filing_pane_list);    
  this->updateUiFromBackEnd(template_pane_list);
  this->updateUiFromBackEnd(aliases_pane_list);
  this->updateUiFromBackEnd(advanced_pane_list);
  this->updateUiFromBackEnd(printing_pane_list);
  this->updateUiFromBackEnd(mailretrieval_pane_list);

  // make sure Vacation pane is correct
  XtVaSetValues(_opt_dlog->vacation_rb_items.On_item,
		XmNset, vaca_running,
		NULL);

  XtVaSetValues(_opt_dlog->vacation_rb_items.Off_item,
		XmNset, !vaca_running,
		NULL);

  // set Subject and Message
  if(vacation_cmd != NULL)
    {
      tmp_ptr = vacation_cmd->subject();

  // if the vaction subject is a null string ( for the case there is no
  // .vacation.msg file but the user already typed his own subject but
  // would like to reset. We need reset the subject back to the message
  // "Dtb_project_catd, 2, 68,"
  
      XtSetArg(args[0], XmNvalue, GETMSG(Dtb_project_catd, 2, 68, "Out of the office"));

      if(tmp_ptr != NULL)
        XtVaSetValues(_opt_dlog->subject_tf,
                      XmNvalue, tmp_ptr,
                      NULL);
  // set back to the default value
      else
        XtSetValues(_opt_dlog->subject_tf,args,1);
        

      tmp_ptr = vacation_cmd->body();

// Do same thing for the body part

      if(tmp_ptr != NULL)
	XtVaSetValues(_opt_dlog->vacation_msg_tp,
		      XmNvalue, tmp_ptr,
		      NULL);
      else   //set a space to the body
        XtVaSetValues(_opt_dlog->vacation_msg_tp,
                      XmNvalue, " ",
                      NULL);

    }

  _reopen_mailboxes = FALSE;
  vacation_status_changed = FALSE;
  vacation_msg_changed = FALSE;
  props_changed = FALSE;
  
}

//////////////////////////////////////////////////////////////////////////////
void OptCmd::update_source()
{

  DtMailEnv error;
  DtMail::Session * d_session = theRoamApp.session()->session();
  DtMail::MailRc * mail_rc = d_session->mailRc(error);

  this->updateBackEndFromUi(mh_pane_list);    
  this->updateBackEndFromUi(msg_view_pane_list);    
  this->updateBackEndFromUi(compose_pane_list);    
  this->updateBackEndFromUi(msg_filing_pane_list);    
  this->updateBackEndFromUi(template_pane_list);
  this->updateBackEndFromUi(aliases_pane_list);
  this->updateBackEndFromUi(advanced_pane_list);
  this->updateBackEndFromUi(printing_pane_list);
  this->updateBackEndFromUi(mailretrieval_pane_list);

  if (mail_rc->getParseError() != DTME_NoError)
  {
    char* helpId;
    int answer = 0;

    genDialog()->setToErrorDialog(GETMSG(DT_catd, 2, 1, "Mailer"),
      GETMSG(DT_catd, 2, 25, "There were unrecoverable syntax errors found in the ~/.mailrc file.\nMail options settings can be applied in the current session but can\nnot be saved to file."));
    helpId = DTMAILHELPERROR;
    genDialog()->post_and_return(GETMSG(DT_catd, 3, 5, "OK"), helpId);
    return;
  }
  else mail_rc->update(error);

  if (_reopen_mailboxes)
  {
      theRoamApp.reopenRoamMenuWindows();
      _reopen_mailboxes = FALSE;
  }
  theRoamApp.globalPropChange();
  props_changed = FALSE;
}

//////////////////////////////////////////////////////////////////////////////
// Handle the file locking stuff.

int OptCmd::doFileLockingCheck()
{
    int answer = 0;

    DtMailEnv        error;
    DtMail::Session *m_session = theRoamApp.session()->session();
    const char      *value = NULL;
    Boolean	     use_file_locking;
    unsigned char    is_set;

    m_session->mailRc(error)->getValue(error, "cdetooltalklock", &value);
    use_file_locking = error.isNotSet();
    if (NULL != value) free((void*) value);

    if (XtIsManaged(_opt_dlog->advanced_pane))
    {
        XtVaGetValues(
	  _opt_dlog->adv_def_cb_items.Use_network_aware_mail_file_locking_item,
	  XmNset, &is_set,
	  NULL);

	if (is_set != use_file_locking)
	{
	    char	    *message = GETMSG(DT_catd, 2, 26,
"The value of the file locking option has changed.\nTo take immediate effect, folders need to be closed and reopened.\n  o Click on 'Reopen' to reopen folders,\n    making the new option take effect immediately.\n  o Click on 'Continue' to avoid reopening folders.\n    The new option will take effect the next time you open a folder.\n  o Click on 'Cancel' to cancel the Apply and return to editing options.");
            genDialog()->setToQuestionDialog(
				GETMSG(DT_catd, 2, 1, "Mailer"),
				message);
	    answer =
              genDialog()->post_and_return(
				GETMSG(DT_catd, 2, 27, "Reopen"),
				GETMSG(DT_catd, 2, 29, "Cancel"),
				GETMSG(DT_catd, 2, 28, "Continue"),
				"AdvancedMailOptions");

	    //
	    // 1 == Reopen
	    // 2 == Cancel
	    // 3 == Continue
	    //
	    if (1 == answer)
	      _reopen_mailboxes = TRUE;
	    else
	      _reopen_mailboxes = FALSE;

	    if (2 != answer)
              theRoamApp.closeInactiveRoamMenuWindows();
        }
    }
    return (2!=answer);
}


//////////////////////////////////////////////////////////////////////////////
// Handle the vacation stuff.

void OptCmd::doVacationCheck()
{
  Boolean is_set;

  VacationCmd *vacation_cmd = theRoamApp.vacation();
  DtMailGenDialog *dialog = genDialog();

  vacation_cmd->setGenDialog(dialog);
  if(vacation_status_changed)
  {   // there was a change in status
      if (XtIsManaged(_opt_dlog->vacation_pane))
      {
	 XtVaGetValues(_opt_dlog->vacation_rb_items.On_item,
			  XmNset, &is_set,
			  NULL);
	 if(is_set)
	 {
	    Boolean status = this->startVacation();

	    // startVacation also updates the mgs strings if changed
      	    vacation_msg_changed = FALSE;
            vacation_status_changed = status; // reset flag
 	 }
	 else 
	 {
	    this->stopVacation();
            vacation_status_changed = FALSE; // reset flag
         }
       }
  }

  if (vacation_msg_changed)
  {
    if (vacation_cmd)
    {
	char *subj, *body;
	XtVaGetValues(_opt_dlog->subject_tf, XmNvalue, &subj, NULL);
	XtVaGetValues(_opt_dlog->vacation_msg_tp, XmNvalue, &body, NULL);

	vacation_cmd->handleMessageFile(subj, body);
    }
    vacation_msg_changed = FALSE;
  }
}

void OptCmd::setVacationChanged()
{
  vacation_status_changed = TRUE;
  props_changed = TRUE;
}

void OptCmd::set_vaca_msg_Changed()
{
  vacation_msg_changed = TRUE;
  props_changed = TRUE;
}
Boolean
OptCmd::startVacation()
{
  Boolean status = theRoamApp.startVacation(
			   _opt_dlog->subject_tf, 
			   _opt_dlog->vacation_msg_tp);
  return status;
}


void
OptCmd::stopVacation()
{
  theRoamApp.stopVacation();
}

//////////////////////////////////////////////////////////////////////////////
// Check the printing options to make sure they are valid

Boolean OptCmd::optionsAreValid()
{
    char	*errMsg = NULL;
    PropUiItem	*propui_ptr = NULL;
    int		answer;

    for (propui_ptr = _PrintingOptions->getFirstProp();
	 propui_ptr != (PropUiItem *) NULL;
	 propui_ptr = _PrintingOptions->getNextProp())
    {
	errMsg = propui_ptr->uiValueIsValid();
        if (errMsg != NULL)
	{
            this->genDialog()->setToQuestionDialog(
	    		GETMSG(DT_catd, 5, 2, "Mailer"),
			errMsg);
 
            answer = this->genDialog()->post_and_return(
			GETMSG(DT_catd, 26, 5, "Continue"),
                        DTMAILHELPERROR);
 
	    XtFree(errMsg);
	    return FALSE;
	}
    }

    for (propui_ptr = _MailRetrievalOptions->getFirstProp();
	 propui_ptr != (PropUiItem *) NULL;
	 propui_ptr = _MailRetrievalOptions->getNextProp())
    {
	errMsg = propui_ptr->uiValueIsValid();
        if (errMsg != NULL)
	{
            this->genDialog()->setToQuestionDialog(
	    		GETMSG(DT_catd, 5, 2, "Mailer"),
			errMsg);
 
            answer = this->genDialog()->post_and_return(
			GETMSG(DT_catd, 26, 5, "Continue"),
                        DTMAILHELPERROR);
 
	    XtFree(errMsg);
	    return FALSE;
	}
    }
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
void register_pane_size(Widget pane)
{
    Dimension   width, height;

    XtManageChild(pane);
    XtVaGetValues(pane,
                XmNwidth, &width, XmNheight, &height,
                NULL);

    if (width > max_width)
        max_width = width;

    if (height > max_height)
        max_height = height;

    XtUnmanageChild(pane);
}

//////////////////////////////////////////////////////////////////////////////
// init the message header list pane
void OptCmd::init_msg_hdr_pane()
{

  PropUiItem *propui_ptr = NULL;

  // register overall size of pane
  register_pane_size(_opt_dlog->msg_header_pane);

  // test for the message header list pane
  
  // make the list...
  mh_pane_list = new DtVirtArray<PropUiItem *>(3);

  // make the check for new mail SB
  propui_ptr = (PropUiItem *)new CheckForMailUiItem(_opt_dlog->mail_check_sb,
					            _FROM_MAILRC,
					            "retrieveinterval");
  propui_ptr->writeFromSourceToUi();
  mh_pane_list->append(propui_ptr);

  // make the signal with beep SB
  propui_ptr = (PropUiItem *)new SpinBoxUiItem(_opt_dlog->signal_sb,
					       _FROM_MAILRC,
					       "bell");
  propui_ptr->writeFromSourceToUi();
  mh_pane_list->append(propui_ptr);

  // make the sig with flashes SB
  propui_ptr = (PropUiItem *)new SpinBoxUiItem(_opt_dlog->spinbox2,
					       _FROM_MAILRC,
					       "flash");
  propui_ptr->writeFromSourceToUi();
  mh_pane_list->append(propui_ptr);

  // make the disp headers SB
  propui_ptr = (PropUiItem *)new SpinBoxUiItem(_opt_dlog->display_sb,
					       _FROM_MAILRC,
					       "headerlines");
  propui_ptr->writeFromSourceToUi();
  mh_pane_list->append(propui_ptr);

  // make the sho "To: Recipient"
  propui_ptr = (PropUiItem *)new CheckBoxUiItem(
	 _opt_dlog->checkbox_items.Show_To_recipient_when_mail_is_from_me_item,
						_FROM_MAILRC,
						"showto");
  propui_ptr->writeFromSourceToUi();
  mh_pane_list->append(propui_ptr);

  // make the sho "Display message numbers"
  propui_ptr = (PropUiItem *)new CheckBoxUiItem(
	 _opt_dlog->checkbox_items.Display_message_numbers_item,
						_FROM_MAILRC,
						"showmsgnum");
  propui_ptr->writeFromSourceToUi();
  mh_pane_list->append(propui_ptr);

  // make the sho "destroy on close"
  propui_ptr = (PropUiItem *)new AntiCheckBoxUiItem(
	 _opt_dlog->deleted_msg_cb_items.When_I_close_the_mailbox_item,
						_FROM_MAILRC,
						"keepdeleted");
  propui_ptr->writeFromSourceToUi();
  mh_pane_list->append(propui_ptr);

  // make the sho "show confirm notice"
  propui_ptr = (PropUiItem *)new AntiCheckBoxUiItem(
	 _opt_dlog->deleted_msg_cb_items.Show_confirmation_notice_item,
						_FROM_MAILRC,
						"quietdelete");
  propui_ptr->writeFromSourceToUi();
  mh_pane_list->append(propui_ptr);
  
}

//////////////////////////////////////////////////////////////////////////////
void OptCmd::init_msg_view_pane()
{
  DtMailEnv error;
  DtMail::Session * d_session = theRoamApp.session()->session();
  DtMail::MailRc * mail_rc = d_session->mailRc(error);
  PropUiItem *propui_ptr = NULL;
  ListUiItem *list_ptr = NULL;

  // register overall size of pane 
  register_pane_size(_opt_dlog->msg_view_pane); 
  
  // make the list...
  msg_view_pane_list = new DtVirtArray<PropUiItem *>(2);

  //---
  propui_ptr = (PropUiItem *)new SpinBoxUiItem(_opt_dlog->display_row_sb,
					       _FROM_MAILRC,
					       "popuplines");
  propui_ptr->writeFromSourceToUi();
  msg_view_pane_list->append(propui_ptr);

  //---
  propui_ptr = (PropUiItem *)new SpinBoxUiItem(_opt_dlog->characters_wide_sb,
					       _FROM_MAILRC,
					       "toolcols");
  propui_ptr->writeFromSourceToUi();
  msg_view_pane_list->append(propui_ptr);

  list_ptr = (ListUiItem *)new IgnoreListUiItem(_opt_dlog->hidden_fields_list,
					    _FROM_MAILRC,
					    "ignore",
					    _opt_dlog->fields_hide);
  
  add_cbs_to_list(list_ptr,
		  _opt_dlog->h_field_add,
		  _opt_dlog->h_field_delete,
		  _opt_dlog->h_field_change);
    
  list_ptr->writeFromSourceToUi();
  list_ptr->AddDefaults();
  msg_view_pane_list->append(list_ptr);

}

//////////////////////////////////////////////////////////////////////////////
void OptCmd::init_compose_pane()
{
  PropUiItem *propui_ptr = NULL;
  DtMailEnv error;
  DtMail::Session * d_session = theRoamApp.session()->session();
  DtMail::MailRc * mail_rc = d_session->mailRc(error);

  DtVirtArray<char *> fields_list(10);
  ListUiItem *list_ptr = NULL;

  // register overall size of pane 
  register_pane_size(_opt_dlog->compose_win_pane); 

  // make the list...
  compose_pane_list = new DtVirtArray<PropUiItem *>(1);

  propui_ptr = (PropUiItem *)new TextFieldUiItem(_opt_dlog->indent_str_tf,
					       _FROM_MAILRC,
					       "indentprefix");
  propui_ptr->writeFromSourceToUi();
  compose_pane_list->append(propui_ptr);

  propui_ptr = (PropUiItem *)new AntiCheckBoxUiItem(
	_opt_dlog->wrap_checkbox_items.Show_Attachment_List_item,
					       _FROM_MAILRC,
					       "hideattachments");
  propui_ptr->writeFromSourceToUi();
  compose_pane_list->append(propui_ptr);

  propui_ptr = (PropUiItem *)new CheckBoxUiItem(
	_opt_dlog->cfrm_att_checkbox_items.Confirm_Attachment_item,
					       _FROM_MAILRC,
					       "confirmattachments");
  propui_ptr->writeFromSourceToUi();
  compose_pane_list->append(propui_ptr);

  //--
  propui_ptr = (PropUiItem *)new SpinBoxUiItem(_opt_dlog->cfrm_att_size_sb,
					       _FROM_MAILRC,
					       "confirmattachmentthreshold");
  propui_ptr->writeFromSourceToUi();
  compose_pane_list->append(propui_ptr);

  propui_ptr = (PropUiItem *)new TextFieldUiItem(_opt_dlog->deadletter_field,
					       _FROM_MAILRC,
					       "deaddir");
  propui_ptr->writeFromSourceToUi();
  compose_pane_list->append(propui_ptr);

  list_ptr = (ListUiItem *)new CustomListUiItem(_opt_dlog->list,
						  _FROM_MAILRC,
						  "",
						  _opt_dlog->hdr_field,
						  _opt_dlog->def_value_field);
  add_cbs_to_list(list_ptr,
		  _opt_dlog->add_button,
		  _opt_dlog->del_button,
		  _opt_dlog->change_button);
  
  list_ptr->writeFromSourceToUi();
  compose_pane_list->append(list_ptr);

}

//////////////////////////////////////////////////////////////////////////////
void OptCmd::init_msg_filing_pane()
{
  DtMailEnv error;
  DtMail::Session * d_session = theRoamApp.session()->session();
  DtMail::MailRc * mail_rc = d_session->mailRc(error);
  const char *list_str = NULL;
  DtVirtArray<char *> move_menu_list(10);
  PropUiItem *propui_ptr = NULL;

  // register overall size of pane 
  register_pane_size(_opt_dlog->msg_filing_pane); 

  ListUiItem *list_ptr = NULL;

  // make the list...
  msg_filing_pane_list = new DtVirtArray<PropUiItem *>(5);

  //--
  propui_ptr = (PropUiItem *)new TextFieldUiItem(_opt_dlog->srt_looking_tf,
						 _FROM_MAILRC,
						 "folder");
  propui_ptr->writeFromSourceToUi();
  msg_filing_pane_list->append(propui_ptr);

  propui_ptr = (PropUiItem *)new AntiCheckBoxUiItem(
	_opt_dlog->dpy_up_mbox_cb_items.Display_up_to_item,
					       _FROM_MAILRC,
					       "dontdisplaycachedfiles");
  propui_ptr->writeFromSourceToUi();
  msg_filing_pane_list->append(propui_ptr);

  //--
  propui_ptr = (PropUiItem *)new SpinBoxUiItem(_opt_dlog->recent_sb,
					       _FROM_MAILRC,
					       "cachedfilemenusize");
  propui_ptr->writeFromSourceToUi();
  msg_filing_pane_list->append(propui_ptr);

  //--
  propui_ptr = (PropUiItem *)new TextFieldUiItem(_opt_dlog->sent_mail_tf,
						 _FROM_MAILRC,
						 "record");
  propui_ptr->writeFromSourceToUi();
  msg_filing_pane_list->append(propui_ptr);


   propui_ptr = (PropUiItem *)new AntiCheckBoxUiItem(
 	 _opt_dlog->log_cb_items.Log_all_sent_messages_item,
 						_FROM_MAILRC,
 						"dontlogmessages");

//    propui_ptr = (PropUiItem *)new AntiCheckBoxUiItem(
//  	 _opt_dlog->log_cb_items.Log_all_sent_messages_item,
//  						_FROM_MAILRC,
//  						"nodontlogmessages");
  propui_ptr->writeFromSourceToUi();
  msg_filing_pane_list->append(propui_ptr);

  list_ptr = (ListUiItem *)new MoveMenuListUiItem(_opt_dlog->mv_menu_lst,
						  _FROM_MAILRC,
						  "",
						  _opt_dlog->path_file_name_tf);

  add_cbs_to_list(list_ptr,
		  _opt_dlog->mm_add_butt,
		  _opt_dlog->mm_delete_butt,
		  _opt_dlog->mm_change_butt);
    
  list_ptr->writeFromSourceToUi();
  msg_filing_pane_list->append(list_ptr);

}

//////////////////////////////////////////////////////////////////////////////
void OptCmd::init_vacation_pane()
{
  // register overall size of pane 
  register_pane_size(_opt_dlog->vacation_pane); 
}

//////////////////////////////////////////////////////////////////////////////
void OptCmd::init_template_pane()
{
  DtMailEnv error;
  ListUiItem *list_ptr = NULL;

  // register overall size of pane 
  register_pane_size(_opt_dlog->templates_pane); 

  // make the list...
  template_pane_list = new DtVirtArray<PropUiItem *>(3);

  list_ptr = (ListUiItem *)new TemplateListUiItem(_opt_dlog->templates_list,
						  _FROM_MAILRC,
						  "templates",
						  _opt_dlog->menu_label,
						  _opt_dlog->path_filename_label);

    add_cbs_to_list(list_ptr,
		  _opt_dlog->tem_add_but,
		  _opt_dlog->tem_del_but,
		  _opt_dlog->tem_change_but);

  list_ptr->writeFromSourceToUi();
  template_pane_list->append(list_ptr);

}

//////////////////////////////////////////////////////////////////////////////
void OptCmd::init_aliases_pane()
{
  DtMailEnv error;
  ListUiItem *list_ptr = NULL;

  // register overall size of pane 
  register_pane_size(_opt_dlog->aliases_pane); 

  // make the list...
   aliases_pane_list = new DtVirtArray<PropUiItem *>(3);

  list_ptr = (ListUiItem *)new AliasListUiItem(_opt_dlog->alias_list,
					       _FROM_MAILRC,
					       "alias",
					       _opt_dlog->alias_tf,
					       _opt_dlog->addresses_tf);

  add_cbs_to_list(list_ptr,
		  _opt_dlog->alias_add_but,
		  _opt_dlog->alias_delete_but,
		  _opt_dlog->al_change_but);

  list_ptr->writeFromSourceToUi();
  aliases_pane_list->append(list_ptr);

}

//////////////////////////////////////////////////////////////////////////////
void OptCmd::init_advanced_pane()
{
  DtMailEnv error;
  DtVirtArray<char *> alternates_list(8);
  PropUiItem *propui_ptr = NULL;
  uid_t euid;
  struct passwd *user_passwd = NULL;
  char *label_str = NULL;
  XmString label_xm_str;

  // register overall size of pane 
  register_pane_size(_opt_dlog->advanced_pane); 

  ListUiItem *list_ptr = NULL;

  label_str = (char *)malloc(256);

  // set the list to be a mono spaced font...


  // make the list...
  advanced_pane_list = new DtVirtArray<PropUiItem *>(6);

  propui_ptr = (PropUiItem *)new ScaleUiItem(
		_opt_dlog->update_mbox_scale,
                                                _FROM_MAILRC,
                                                "saveinterval");
  propui_ptr->writeFromSourceToUi();
  advanced_pane_list->append(propui_ptr);

  propui_ptr = (PropUiItem *)new AntiCheckBoxUiItem(
	      _opt_dlog->adv_def_cb_items.Show_confirmation_notices_item,
						_FROM_MAILRC,	
						"expert");
  propui_ptr->writeFromSourceToUi();
  advanced_pane_list->append(propui_ptr);

  propui_ptr = (PropUiItem *)new CheckBoxUiItem(
      _opt_dlog->adv_def_cb_items.Use_strict_MIME_character_encoding_item,
						_FROM_MAILRC,	
						"strictmime");
  propui_ptr->writeFromSourceToUi();
  advanced_pane_list->append(propui_ptr);

  propui_ptr = (PropUiItem *)new CheckBoxUiItem(
      _opt_dlog->adv_def_cb_items.Use_network_aware_mail_file_locking_item,
						_FROM_MAILRC,	
						"cdetooltalklock");
  propui_ptr->writeFromSourceToUi();
  advanced_pane_list->append(propui_ptr);

  propui_ptr = (PropUiItem *)new CheckBoxUiItem(
      _opt_dlog->adv_def_cb_items.Ignore_host_name_in_address_allnet_item,
						_FROM_MAILRC,	
						"allnet");
  propui_ptr->writeFromSourceToUi();
  advanced_pane_list->append(propui_ptr);

  propui_ptr = (PropUiItem *)new CheckBoxUiItem(
      _opt_dlog->adv_def_cb_items.Include_me_when_I_Reply_to_all_metoo_item,
						_FROM_MAILRC,	
						"metoo");
  propui_ptr->writeFromSourceToUi();
  advanced_pane_list->append(propui_ptr);

  propui_ptr = (PropUiItem *)new CheckBoxUiItem(
	      _opt_dlog->adv_def_cb_items.Use_local_name_username_item,
						_FROM_MAILRC,	
						"usealternates");
  propui_ptr->writeFromSourceToUi();
  advanced_pane_list->append(propui_ptr);

  // set uid in string...
  euid = geteuid();  // get uid of user

  user_passwd = getpwuid(euid); // get passwd entry for user 

  sprintf(label_str, "%s \"%s\" =",
	  GETMSG( DT_catd, 20, 2, "Use local name: "), user_passwd->pw_name);

  label_xm_str = XmStringCreateLocalized(label_str);

  XtVaSetValues(_opt_dlog->adv_def_cb_items.Use_local_name_username_item,
		XmNlabelString, label_xm_str,
		NULL);


  list_ptr = (ListUiItem *)new AlternatesListUiItem(_opt_dlog->local_name_list,
						    _FROM_MAILRC,
						    "alternates",
						    _opt_dlog->local_name_tf);
  
  add_cbs_to_list(list_ptr,
		  _opt_dlog->local_name_add_but,
		  _opt_dlog->local_name_del_but,
		  _opt_dlog->local_name_chg_but);
    

  list_ptr->writeFromSourceToUi();
  advanced_pane_list->append(list_ptr);

}


//////////////////////////////////////////////////////////////////////////////
void OptCmd::init_printing_pane()
{
  DtMailEnv	error;
  PropUiItem	*propui_ptr = NULL;
  int		npropui;

  //_PrintingOptions = new DmxPrintOptions(_opt_dlog->layer);
  //_opt_dlog->printing_pane = _PrintingOptions->baseWidget();

  // register overall size of pane 
  register_pane_size(_opt_dlog->printing_pane); 

  // make the list...
  npropui = _PrintingOptions->getNumProps();
  printing_pane_list = new DtVirtArray<PropUiItem *>(npropui);

  if (npropui)
  {
      for (propui_ptr = _PrintingOptions->getFirstProp();
	   propui_ptr != (PropUiItem *) NULL;
	   propui_ptr = _PrintingOptions->getNextProp())
      {
	  propui_ptr->writeFromSourceToUi();
          printing_pane_list->append(propui_ptr);
      }
  }
}


//////////////////////////////////////////////////////////////////////////////
void OptCmd::init_mailretrieval_pane()
{
  DtMailEnv	error;
  PropUiItem	*propui_ptr = NULL;
  int		npropui;

  //_PrintingOptions = new DmxPrintOptions(_opt_dlog->layer);
  //_opt_dlog->printing_pane = _PrintingOptions->baseWidget();

  // register overall size of pane 
  register_pane_size(_opt_dlog->mailretrieval_pane); 

  // make the list...
  npropui = _MailRetrievalOptions->getNumProps();
  mailretrieval_pane_list = new DtVirtArray<PropUiItem *>(npropui);

  if (npropui)
  {
      for (propui_ptr = _MailRetrievalOptions->getFirstProp();
	   propui_ptr != (PropUiItem *) NULL;
	   propui_ptr = _MailRetrievalOptions->getNextProp())
      {
	  propui_ptr->writeFromSourceToUi();
          mailretrieval_pane_list->append(propui_ptr);
      }
  }

  _MailRetrievalOptions->initOptionInteractions();
}


//////////////////////////////////////////////////////////////////////////////
void OptCmd::doit()
{
  // make sure all the sheets are uptodate
  this->update_panes();

  XtVaSetValues(_opt_dlog->cancel_button,
		XmNuserData, this,
		NULL);

  XtVaSetValues(_opt_dlog->ok_button,
		XmNuserData, this,
		NULL);

  XtVaSetValues(_opt_dlog->apply_button,
		XmNuserData, this,
		NULL);

  XtVaSetValues(_opt_dlog->reset_button,
		XmNuserData, this,
		NULL);

  XtVaSetValues(_opt_dlog->vacation_rb_items.On_item,
		XmNuserData, this,
		NULL);

  XtVaSetValues(_opt_dlog->vacation_rb_items.Off_item,
		XmNuserData, this,
		NULL);

  XtPopdown(_opt_dlog->dialog);
  XtPopup(_opt_dlog->dialog, XtGrabNone);
  XtManageChild(_opt_dlog->dialog_shellform);
  props_changed = FALSE;
}       
//////////////////////////////////////////////////////////////////
void OptCmd::updateUiFromBackEnd(DtVirtArray<PropUiItem *> *list)
{
  int len, i;

  if(list != NULL)
    {
      len = list->length();
      
      for(i = 0; i < len; i++)
	(*list)[i]->writeFromSourceToUi();
    }
  else
    fprintf(stderr,"NULL list passed to updateUiFromBackEnd\n");
  
}
//////////////////////////////////////////////////////////////////
void OptCmd::updateBackEndFromUi(DtVirtArray<PropUiItem *> *list)
{
  int len, i;

  if(list != NULL)
    {
      len = list->length();
      
      for(i = 0; i < len; i++)
	(*list)[i]->writeFromUiToSource();
    }
  else
    fprintf(stderr,"NULL list passed to updateBackEndFromUi\n");
  
}


//////////////////////////////////////////////////////////////////////////////
void OptCmd::unmanage()
{

  XtPopdown(_opt_dlog->dialog);

}

//////////////////////////////////////////////////////////////////////////////
void
OptCmd::execute()
{
    _opt_dlog = (DtbOptionsDialogInfo)theRoamApp.optionsDialog();	
    if (_opt_dlog == NULL) {
	this->create_dlog();
        generalDialog = new DtMailGenDialog("Dialog", _opt_dlog->dialog);
    	this->doit();
    }
    else {
        XtPopdown(_opt_dlog->dialog);
	XtPopup(_opt_dlog->dialog, XtGrabNone);
	XtManageChild(_opt_dlog->dialog_shellform);
    }
}

//////////////////////////////////////////////////////////////////////////////
void OptCmd::setNewCategoryStr()
{
        XtVaGetValues(XmOptionButtonGadget(_opt_dlog->category_opmenu), XmNlabelString, &catstr, NULL);
}

//////////////////////////////////////////////////////////////////////////////
void OptCmd::setOldCategoryStr()
{
        XtVaSetValues(XmOptionButtonGadget(_opt_dlog->category_opmenu), XmNlabelString, catstr, NULL);
}

//////////////////////////////////////////////////////////////////////////////
void OptCmd::displayAliasesOptionsPane()
{
    execute();
    options_set_category_pane(_opt_dlog->aliases_pane);
    XtVaSetValues(
		_opt_dlog->category_opmenu,
		XmNmenuHistory, _opt_dlog->category_opmenu_items.Aliases_item,
		NULL);
}

//////////////////////////////////////////////////////////////////////////////
void OptCmd::displayPrintingOptionsPane()
{
    execute();
    options_set_category_pane(_opt_dlog->printing_pane);
    XtVaSetValues(
		_opt_dlog->category_opmenu,
		XmNmenuHistory, _opt_dlog->category_opmenu_items.Printing_item,
		NULL);
}

//////////////////////////////////////////////////////////////////////////////
void OptCmd::displayMailRetrievalOptionsPane()
{
    execute();
    options_set_category_pane(_opt_dlog->mailretrieval_pane);
    XtVaSetValues(
	_opt_dlog->category_opmenu,
	XmNmenuHistory,_opt_dlog->category_opmenu_items.MailRetrieval_item,
	NULL);
}
