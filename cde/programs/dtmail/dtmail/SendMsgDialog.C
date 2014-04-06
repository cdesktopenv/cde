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
/*
 *+SNOTICE
 *
 * $TOG: SendMsgDialog.C /main/43 1999/03/25 13:42:29 mgreess $
 *
 * RESTRICTED CONFIDENTIAL INFORMATION:
 * 
 * The information in this document is subject to special
 * restrictions in a confidential disclosure agreement between
 * HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 * document outside HP, IBM, Sun, USL, SCO, or Univel without
 * Sun's specific written approval.  This document and all copies
 * and derivative works thereof must be returned or destroyed at
 * Sun's request.
 *
 * Copyright 1993, 1994, 1995 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

#include <EUSCompat.h>
#include "AttachArea.h"
#include "Attachment.h"
#include "RoamApp.h"
#include "SendMsgDialog.h"
#include "NoOpCmd.h"
#include "RoamCmds.h"
#include "ComposeCmds.hh"
#include "CmdList.h"
#include "MenuBar.h"
#include "ButtonInterface.h"
#include "MemUtils.hh"
#include "Help.hh"
#include "DtMailHelp.hh"
#include "SelectFileCmd.h"
#include "DtMailGenDialog.hh"
#include "Application.h"
#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <Xm/RepType.h>
#include <Xm/ScrolledW.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/CascadeB.h>
#include <Xm/SeparatoG.h>
#include <DtMail/IO.hh>
#include <DtMail/DtMailP.hh>
#include <stdio.h>
#include <errno.h>
#include <sys/param.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>

#include "MailMsg.h"
#include "str_utils.h"

#define OFFSET 10    // Number of spaces from margin

#ifdef DTMAIL_TOOLTALK
// Time to self destruct
#define DESTRUCT_TIMEOUT   60000    // 1 minutes
#endif

// Pipe used between RFCTransport::childHandler and XtAppAddInput
static int _transfds[2];

static const char *ComposeIcon = "IcMcomp";

struct DefaultHeaders {
    int				msg_set;
    int				msg_number;
    char *			dflt_label;
    char *			label;
    const char *		header;
    SendMsgDialog::ShowState	show;
};

static int		isInitializedDefaultHeaderList = 0;
static DefaultHeaders	DefaultHeaderList[] = {
{ 1, 241, "To",		NULL, DtMailMessageTo,	SendMsgDialog::SMD_ALWAYS }, 
{ 1, 242, "Subject",	NULL, DtMailMessageSubject, SendMsgDialog::SMD_ALWAYS },
{ 1, 243, "Cc",		NULL, DtMailMessageCc,	SendMsgDialog::SMD_ALWAYS },
{ 1, 244, "Bcc",	NULL, DtMailMessageBcc,	SendMsgDialog::SMD_HIDDEN },
{ 0, 0,   NULL,		NULL, NULL,		SendMsgDialog::SMD_NEVER  }
};

// These headers can never be controlled by the user. They are generated
// by dtmail and the user is not allowed to override the values generated
// by the software. Besides, most users would not have a clue as to what
// a correct value would be.
//
static const char * BlockedHeaders[] = {
    "Content-Length",
    "Content-Type",
    "Content-MD5",
    "Content-Transfer-Encoding",
    "Mime-Version",
    "X-Sun-Charset",
    NULL
};


static DtMailBoolean
block(const char * header)
{
    for (const char ** test = BlockedHeaders; *test; test++) {
	if (strcasecmp(header, *test) == 0) {
	    return(DTM_TRUE);
	}
    }

    return(DTM_FALSE);
}

class CmdList;

Compose theCompose;   // Manages all compose windows.

SendMsgDialog::HeaderList::HeaderList(void)
{
    label = NULL;
    header = NULL;
    value = NULL;
    show = SMD_NEVER;
    form_widget = NULL;
    label_widget = NULL;
    field_widget = NULL;
}

SendMsgDialog::HeaderList::HeaderList(const HeaderList & other)
{
    label = NULL;
    header = NULL;
    show = other.show;
    form_widget = other.form_widget;
    label_widget = other.label_widget;
    field_widget = other.field_widget;

    if (other.label) {
	label = strdup(other.label);
    }

    if (other.header) {
	header = strdup(other.header);
    }

    if (other.value) {
	value = strdup(other.value);
    }
}

SendMsgDialog::HeaderList::~HeaderList(void)
{
    if (label) {
	free(label);
    }

    if (header) {
	free(header);
    }

    if (value) {
	free(value);
    }
}
Boolean SendMsgDialog::reservedHeader(const char *label)
{
    for (DefaultHeaders * hl = DefaultHeaderList; hl->dflt_label; hl++)
      if (strcmp(label, hl->label) == 0)
        return TRUE;
    return FALSE;
}


SendMsgDialog::SendMsgDialog() 
: MenuWindow ( "ComposeDialog", True ),
_header_list(16)
{
    _main_form = NULL;
    _num_sendAs = 0;
    _genDialog = NULL;

    _show_attach_area = FALSE;
    _takeDown = FALSE;
    _first_time = TRUE;
    _already_sending = FALSE;

    _separator = NULL;

    // Alias Popup menus
    _aliasList = NULL;
    _bccPopupCmdlist = NULL;
    _bccPopupMenu = NULL;
    _bccPopupMenuBar = NULL;
    _ccPopupCmdlist = NULL;
    _ccPopupMenu = NULL;
    _ccPopupMenuBar = NULL;
    _toPopupCmdlist = NULL;
    _toPopupMenu = NULL;
    _toPopupMenuBar = NULL;

    // Attachment menu
    _attachmentActionsList = NULL;
    _attachmentMenu = NULL;
    _attachmentMenuList = NULL;
    _attachmentPopupMenuList = NULL;
    _textPopupMenuList = NULL;
    
    _att_show_pane = NULL;
    _att_select_all = NULL;
    _att_save = NULL;
    _att_add = NULL;
    _att_delete = NULL;
    _att_undelete = NULL;
    _att_rename = NULL;
    _att_select_all = NULL;
    _auto_save_interval = 0;
    _auto_save_path = NULL;
    _auto_save_file = NULL;
    _dead_letter_buf = NULL;
    
    _file_include = NULL;
    _file_save_as = NULL;
    _file_send = NULL;
    _file_log = NULL;
    _file_close = NULL;

    _format_word_wrap = NULL;
    _format_settings = NULL;
    _format_find_change = NULL;
    _format_spell = NULL;
    _format_menu = NULL;
    _format_cmds = NULL;
    _format_cascade = NULL;
    _format_separator = NULL;
    _templateList = NULL;

    // Now we need to get the additional headers from the property system.
    //   
    DtMailEnv error;
    DtMail::Session * d_session = theRoamApp.session()->session();
    DtMail::MailRc * mail_rc = d_session->mailRc(error);
 
    const char * value = NULL;  
    mail_rc->getValue(error, "additionalfields", &value);
 
    DtVirtArray<PropStringPair *> results(8);
    if (error.isNotSet()) {
        _additionalfields = strdup(value);
        parsePropString(value, results);
    }
 
    if (NULL != value)
      free((void*) value);

    // Load the header list with the predefined/fixed headers.
    //
    if (! isInitializedDefaultHeaderList)
      for (DefaultHeaders * hl = DefaultHeaderList; hl->dflt_label; hl++) {
	  hl->label =
	    GETMSG(DT_catd, hl->msg_set, hl->msg_number, hl->dflt_label);

          isInitializedDefaultHeaderList = TRUE;
      }

    for (DefaultHeaders * hl = DefaultHeaderList; hl->dflt_label; hl++) {
	HeaderList * copy_hl = new HeaderList;
	copy_hl->label = strdup(hl->label);
	copy_hl->value = getPropStringValue(results, hl->label);
	copy_hl->header = strdup(hl->header);
	copy_hl->show = hl->show;
	_header_list.append(copy_hl);
    }

    if (error.isNotSet()) {

	for (int mrc = 0; mrc < results.length(); mrc++) {
	    PropStringPair * psp = results[mrc];

	    if (!reservedHeader(psp->label)) {
	    	HeaderList * copy_hl = new HeaderList;
	    	copy_hl->label = strdup(psp->label);
	    	copy_hl->header = strdup(psp->label);
	    	if (psp->value) 
			copy_hl->value = strdup(psp->value);
	    	copy_hl->show = SMD_HIDDEN;
	    	_header_list.append(copy_hl);
	    }
	}

	while(results.length()) {
	    PropStringPair * psp = results[0];
	    delete psp;
	    results.remove(0);
	}
    }
    else 
	_additionalfields = NULL;

}

SendMsgDialog::~SendMsgDialog()
{
    
    delete _genDialog;
    delete _separator;
    
    // File
    
    delete _file_include;
    delete _file_save_as;
    delete _file_send;
    delete _file_log;
    delete _file_close;
    
    // Edit
    
    delete _edit_undo;
    delete _edit_cut;
    delete _edit_copy;
    delete _edit_paste;
    delete _edit_clear;
    delete _edit_delete;
    delete _edit_select_all;
    
    // Alias Popup menus

    delete _aliasList;

    XtDestroyWidget(_ccPopupMenu);
    delete _ccPopupMenuBar;
    delete _ccPopupCmdlist;

    XtDestroyWidget(_toPopupMenu);
    delete _toPopupMenuBar;
    delete _toPopupCmdlist;

    XtDestroyWidget(_bccPopupMenu);
    delete _bccPopupMenuBar;
    delete _bccPopupCmdlist;

    // Attachment
    
    delete _att_save;
    delete _att_add;
    delete _att_delete;
    delete _att_undelete;
    delete _att_rename;
    delete _att_select_all;
    
    delete _attachmentActionsList;
    
    // Format
    
    delete _format_word_wrap;
    delete _format_settings;
    delete _format_find_change;
    delete _format_spell;
    delete _format_separator;
    
    // Things created by createWorkArea()
    delete _my_editor;
    delete _send_button;
    delete _close_button;
    
    //
    // Allocated using 'malloc'.
    // Purify requires us to free it using 'free'.
    //
    free(_auto_save_path);
    if (_auto_save_file) {
        delete _auto_save_file;
    }
    if (_dead_letter_buf) {
        delete _dead_letter_buf;
    }

    delete _attachmentMenuList;
    delete _attachmentPopupMenuList;
    delete _textPopupMenuList;

}

// Callback for each header row
void
header_form_traverse(Widget w, XtPointer, XtPointer)
{
    (void) XmProcessTraversal(w, XmTRAVERSE_NEXT_TAB_GROUP);
}

#ifdef DEAD_WOOD
void
send_pushb_callback(Widget, XtPointer, XtPointer)
{
    
}
#endif /* DEAD_WOOD */

// Create Message Handle with empty first body part.
DtMail::Message *
SendMsgDialog::makeMessage(void)
{
    DtMailEnv error;
    DtMail::Session * d_session = theRoamApp.session()->session();
    DtMailBuffer mbuf;
    
    mbuf.buffer = NULL;
    mbuf.size = 0;
    
    DtMail::Message * msg = d_session->messageConstruct(error,
							DtMailBufferObject,
							&mbuf,
							NULL,
							NULL,
							NULL);
    if (error.isSet() || !msg) {
	return(NULL);
    }
    
    DtMail::BodyPart * bp = msg->newBodyPart(error, NULL);
    
    // For now, reserve the first body part for text.
    setFirstBPHandled(TRUE);
    
    return(msg);
}

// Before Submitting, call this routine.
// This routine does not save attachment(s).
// Check point routine should also call this routine.
void
SendMsgDialog::updateMsgHnd()
{
    DtMailEnv error;
    DtMail::Envelope * env;
    int textLen;

    env = _msgHandle->getEnvelope(error);
    storeHeaders();

    char * widget_text = this->text();
    if (widget_text && *widget_text == '\0') {
	textLen = 0;
	XtFree(widget_text);
	widget_text = NULL;
    }
    else
    	textLen = strlen(widget_text);
    
    // Even if textlen is 0 because user has cleared all previous text,
    // need to setContents again to clear first BP.  Otherwise, deleted
    // text will show up. 

    // Get FirstBodyPart and fill it up.
    DtMail::BodyPart *bp = _msgHandle->getFirstBodyPart(error);
    bp->setContents(error, widget_text, textLen, NULL, NULL, 0, NULL);
    setFirstBPHandled(TRUE);
    
    if (NULL != widget_text)
      XtFree(widget_text);
}

// Before Submitting, also call this routine.
// This routine fills the message handle with attachment(s).
// The reason why we get attachment(s) from the Back End before submission
// is in case the attachment(s) are updated during the compose session.
// If changes are saved back, then the AttachArea class would register the
// update with Back End.  So BE always has the latest attachments.
void
SendMsgDialog::updateMsgHndAtt()
{
    if ( _inclMsgHandle == NULL )
	return;    // No attachments
    
    DtMailEnv error;
    DtMail::BodyPart *msgBP;
    DtMail::BodyPart *inclBP = _inclMsgHandle->getFirstBodyPart(error);
    const void *contents;
    unsigned long len;
    char *type;
    char *name;
    int mode;
    char *desc;
    
    // If message handle to be copied, _inclMsgHandle, does not have its first
    // body part as text, then get its first body part content and copy it.
    // If it contains text, then skip it.
    if ( !_inclMsgHasText ) {
	inclBP->getContents(error, &contents, &len, &type, &name, &mode, &desc);
	if ( _firstBPHandled ) {
	    msgBP = _msgHandle->newBodyPart(error, _lastAttBP); 
	} else {
	    msgBP = _msgHandle->getFirstBodyPart(error);
	}
	msgBP->setContents(error, contents, len, type, name, mode, desc);
	setLastAttBP(msgBP);
	free(name);
	free(type);
    }
    
    // Continue to get the next body part and start copy.
    while ((inclBP = _inclMsgHandle->getNextBodyPart(error, inclBP)) != NULL) {
	inclBP->getContents(error, &contents, &len, &type, &name, &mode, &desc);
	if ( _firstBPHandled ) {
	    msgBP = _msgHandle->newBodyPart(error, _lastAttBP); 
	} else {
	    msgBP = _msgHandle->getFirstBodyPart(error);
	}
	msgBP->setContents(error, contents, len, type, name, mode, desc);
	setLastAttBP(msgBP);
	free(name);
	free(type);
    }  // end of while loop
}

// Update the _lastAttBP pointer so that subsequent newBodyPart() calls can
// return a message body part following the last body part.
void
SendMsgDialog::setLastAttBP()
{
    DtMailEnv error;
    _lastAttBP = _msgHandle->getNextBodyPart(error, _lastAttBP);
}

// This sister routine is needed when _lastAttBP has not been initialized.
void
SendMsgDialog::setLastAttBP(DtMail::BodyPart *bp)
{
    _lastAttBP = bp;
}

// Initialize _msgHandle
void
SendMsgDialog::setMsgHnd()
{
    _msgHandle = makeMessage();
    _inclMsgHandle = NULL;
    _inclMsgHasText = FALSE;
    _lastAttBP = NULL;
}

// Set timeout to ten minutes (as milliseconds)
//
void
SendMsgDialog::startAutoSave(void)
{
    mkAutoSavePath();
    if(!_auto_save_path) return;
    _auto_save_interval = XtAppAddTimeOut(theApplication->appContext(),
					  getAutoSaveInterval(),
					  autoSaveCallback,
					  this);
}

void
SendMsgDialog::stopAutoSave(void)
{
    if (!_auto_save_interval) {
	return;
    }
    
    XtRemoveTimeOut(_auto_save_interval);
    _auto_save_interval = 0;
    
    unlink(_auto_save_file);
    free(_auto_save_file);
    _auto_save_file = NULL;
}

static const char * DEAD_LETTER_DIR = "~/dead_letter";
static const char * BACKUP_DEAD_LETTER = "./dead_letter";
static const char * PREFIX = "mail.dead.letter";

void
SendMsgDialog::mkAutoSavePath(void)
{
    // First, see if we need to set up the path.
    //

    if (!_auto_save_path) {
	DtMail::Session * d_session = theRoamApp.session()->session();
	DtMailEnv error;
    	const char *save_path = NULL;

    	d_session->mailRc(error)->getValue(error, "deaddir", &save_path);
  	if (error.isNotSet() && save_path != NULL && *save_path != '\0')
	    _auto_save_path = d_session->expandPath(error, save_path);
	else {
	
		_auto_save_path = d_session->expandPath(error, DEAD_LETTER_DIR);
		if (!_auto_save_path)
	    	_auto_save_path = d_session->expandPath(error, BACKUP_DEAD_LETTER);
    	}

        if (NULL != save_path)
          free((void*) save_path);
    }
    
    // If we still have a path, punt.
    // 
    if (!_auto_save_path) {
	return;
    }
    
    if (SafeAccess(_auto_save_path, W_OK) != 0) {
	if (errno != ENOENT) {
	    // Not an error we can overcome here.
	    //
	    free(_auto_save_path);
	    _auto_save_path = NULL;
	    return;
	}
	
	if (mkdir(_auto_save_path, 0700) < 0) {
	    free(_auto_save_path);
	    _auto_save_path = NULL;
	    return;
	}
    }
    
    // Now we run through the possible file names until we hit pay dirt.
    //
    _auto_save_file = (char*) malloc((size_t) strlen(_auto_save_path) + 100);
    for (int suffix = 1; ; suffix++) {
	sprintf(_auto_save_file, "%s/%s.%d", _auto_save_path, PREFIX, suffix);
	if (SafeAccess(_auto_save_file, F_OK) != 0) {
	    break;
	}
    }
}

void
SendMsgDialog::loadDeadLetter(const char * path)
{
    _auto_save_file = strdup(path);
    parseNplace(path);
    
    _auto_save_interval = XtAppAddTimeOut(theApplication->appContext(),
					  getAutoSaveInterval(),
					  autoSaveCallback,
					  this);
}

void
SendMsgDialog::autoSaveCallback(XtPointer client_data, XtIntervalId * id)
{
    SendMsgDialog * self = (SendMsgDialog *)client_data;
    
    if (self->_auto_save_interval != *id) {
	// Random noise. Ignore it.
	return;
    }
    
    self->doAutoSave();
    
    self->_auto_save_interval = XtAppAddTimeOut(theApplication->appContext(),
						self->getAutoSaveInterval(),
						autoSaveCallback,
						self);
}

void
SendMsgDialog::doAutoSave(char *filename)
{
    DtMail::Session * d_session = theRoamApp.session()->session();
    DtMailEnv error;

    busyCursor();
    setStatus(GETMSG(DT_catd, 3, 70, "Writing dead letter..."));

    updateMsgHnd();
    
    assert((NULL != filename));
    RFCWriteMessage(error, d_session, filename, _msgHandle);
    if((DTMailError_t) error == DTME_OutOfSpace )
    {
       RoamMenuWindow::ShowErrMsg((char *)error.getClient(),TRUE,this );
    }
    normalCursor();
    clearStatus();
}

void
SendMsgDialog::doAutoSave(void)
{
    doAutoSave(_auto_save_file);
}

int
SendMsgDialog::getAutoSaveInterval(void)
{
    DtMailEnv error;
    // Initialize the mail_error.
    
    error.clear();
    
    DtMail::Session * d_session = theRoamApp.session()->session();
    DtMail::MailRc * mail_rc = d_session->mailRc(error);
    
    int save_interval;
    const char * value = NULL;
    mail_rc->getValue(error, "composeinterval", &value);
    if (error.isSet() || value == NULL) {
	save_interval = 10 * 60 * 1000; // 10 minutes
    }
    else {
	save_interval = (int) strtol(value, NULL, 10) * 60 * 1000;
	save_interval = (save_interval <= 0) ? 10 * 60 * 1000 : save_interval;
    }
    if (NULL != value)
      free((void*) value);
    
    return(save_interval);
}

void
SendMsgDialog::setInclMsgHnd(DtMail::Message *msg, Boolean text)
{
    _inclMsgHandle = msg;
    _inclMsgHasText = text;
}

void
SendMsgDialog::setFirstBPHandled(Boolean handle)
{
    _firstBPHandled = handle;
}

void
SendMsgDialog::setHeader(const char * name, const char * value)
{
    // See if this header is in the list. If so, set the widget for
    // it.
    //
    int slot = lookupHeader(name);
    if (slot < 0) {
	return;
    }

    HeaderList * hl = _header_list[slot];
    if (hl->show == SMD_NEVER) {
	// The user removed this header via props.
	//
	return;
    }

    if (hl->field_widget) {
	XtVaSetValues(hl->field_widget,
		      XmNvalue, value,
		      NULL);
	if (hl->show == SMD_HIDDEN) {
	    changeHeaderState(name);
	}
    }
}

void
SendMsgDialog::setHeader(const char * name, DtMailValueSeq & value)
{
    if (value.length() == 0) {
	return;
    }

    if (value.length() == 1) {
	setHeader(name, *(value[0]));
    }
    else {
	int max_len = 0;
	for (int slen = 0; slen < value.length(); slen++) {
	    max_len += strlen(*(value[slen]));
	}

	char * new_str = new char[max_len + (value.length() * 3)];

	strcpy(new_str, "");
	for (int copy = 0; copy < value.length(); copy++) {
	    if (copy != 0) {
		strcat(new_str, " ");
	    }

	    strcat(new_str, *(value[copy]));
	}

	setHeader(name, new_str);
	delete [] new_str;
    }
}

void
SendMsgDialog::loadHeaders(DtMail::Message * input,
			   DtMailBoolean load_all)
{
    // We are going to go through every header in the message.
    // If it is not one of the headers we block, then we will
    // load it into the header pane, depending on the setting of
    // load_all. If true, then we load every header we allow and
    // create new dynamic headers as necessary. If load_all is false,
    // then we only load headers that already are available in the
    // memory pane.
    //
    DtMailEnv error;
    DtMail::Message * msg = (input ? input : _msgHandle);
    DtMail::Envelope * env = msg->getEnvelope(error);
    DtMailHeaderHandle hnd;

    int hcount = 0;
    char * name;
    DtMailValueSeq value;

    for (hnd = env->getFirstHeader(error, &name, value);
	 error.isNotSet() && hnd;
	 hnd = env->getNextHeader(error, hnd, &name, value)) {

	// Always ignore the Unix from line.
	//
	if (hcount == 0 &&
	    strcmp(name, "From") == 0) {
	    free(name);
	    value.clear();
	    hcount += 1;
	    continue;
	}

	hcount += 1;

	// See if the name is one we always block.
	//
	if (block(name)) {
	    free(name);
	    value.clear();
	    continue;
	}

	int slot = lookupHeader(name);
	if (slot < 0) {
	    // We dont have a place for this information. We may need
	    // to create a new header.
	    //
	    if (load_all) {
		HeaderList *hl = new HeaderList;
		hl->label = strdup(name);
		hl->header = strdup(name);
		hl->show = SMD_HIDDEN;
		_header_list.append(hl);
		createHeaders(_header_form);
		doDynamicHeaderMenus();
	    }
	    else {
		free(name);
		value.clear();
		continue;
	    }
	}

	setHeader(name, value);
	free(name);
	value.clear();
    }
}

void
SendMsgDialog::storeHeaders(DtMail::Message * input)
{
    DtMail::Message * msg = (input ? input : _msgHandle);
    DtMailEnv error;
    DtMail::Envelope * env = msg->getEnvelope(error);

    // Walk through the headers. Fetch the strings from the ones
    // that are visible to the user and stuff them into the
    // message.
    //
    for (int scan = 0; scan < _header_list.length(); scan++) {
	HeaderList * hl = _header_list[scan];
	if (hl->show != SMD_ALWAYS && hl->show != SMD_SHOWN) {
	    continue;
	}

	char * value;
	XtVaGetValues(hl->field_widget,
		      XmNvalue, &value,
		      NULL);
	// If the header has a value, we want to set the value
	// in the back end.  Otherwise, we don't want to send
	// out a blank header, so we remove it.
	if (strlen(value) > 0) {
	    env->setHeader(error, hl->header, DTM_TRUE, value);
	}
	else {
	    env->removeHeader(error, hl->header);
	}
	if (value) {
		XtFree(value);
        }
    }
}

void
SendMsgDialog::changeHeaderState(const char * name)
{
    int slot = lookupHeader(name);
    if (slot < 0) {
	return;
    }

    HeaderList * hl = _header_list[slot];
    if (hl->show == SMD_ALWAYS || hl->show == SMD_NEVER) {
	return;
    }

    // If the user is trying to remove a header with a value other than
    // the default, we should at least ask.
    //
    if (hl->show == SMD_SHOWN) {
	char * value;
	XtVaGetValues(hl->field_widget,
		      XmNvalue, &value,
		      NULL);
	if (strlen(value) > 0) {
	    if (!hl->value || strcmp(value, hl->value) != 0) {
		char *buf = new char[256];
		sprintf(buf, 
			GETMSG(DT_catd, 2, 17, 
				"You have edited \"%s\". Delete anyway?"), 
			hl->label);
		_genDialog->setToWarningDialog(GETMSG(DT_catd, 3, 71,
						       "Mailer"),
					       buf);
		char * helpId = DTMAILHELPERROR;
		int answer = _genDialog->post_and_return(
					    GETMSG(DT_catd, 3, 72, "OK"), 
					    GETMSG(DT_catd, 3, 73, "Cancel"), 
					    helpId);
		delete [] buf;
		if (answer == 2) {
		    free(value);
		    return;
		}
	    }
	}

	free(value);
    }

    // Now we need to toggle the current state of the header.
    //
    char *label = new char[100];
    if (hl->show == SMD_SHOWN) {
	XtUnmanageChild(hl->form_widget);
	hl->show = SMD_HIDDEN;
 	sprintf(label, "%s ", GETMSG(DT_catd, 1, 228, "Add"));

    }
    else {
	XtManageChild(hl->form_widget);
	hl->show = SMD_SHOWN;
	sprintf(label, "%s ", GETMSG(DT_catd, 1, 229, "Delete"));
    }

    justifyHeaders();
    reattachHeaders();

    // Change the label on the menu item.
    //
    strcat(label, hl->label);
    strcat(label, ":");
    char *button_name = new char[100];
    sprintf(button_name, "%s ", GETMSG(DT_catd, 1, 228, "Add"));
    strcat(button_name, hl->label);
    strcat(button_name, ":");

    _menuBar->changeLabel(_format_menu, button_name, label);
    delete [] label;
    delete [] button_name;
}

void
SendMsgDialog::setStatus(const char * str)
{
    char *tmpstr = strdup(str);
    XmString label = XmStringCreateLocalized(tmpstr);

    XtVaSetValues(_status_text,
		  XmNlabelString, label,
		  NULL);

    XmUpdateDisplay(baseWidget());
    XmStringFree(label);
    free(tmpstr);
}

void
SendMsgDialog::clearStatus(void)
{
    setStatus(" ");
}

Boolean
SendMsgDialog::isMsgValid(void)
{
    if (!_msgHandle)
        return FALSE;
    else
        return TRUE;
}


// Sendmail is exed'd and this parent process returns immediately.  When
// the sendmail child exits, this function is called with the pid of the 
// child and its status.
void
SendMsgDialog::sendmailErrorProc (int, int status, void *data)
{
    SendMsgDialog *smd = (SendMsgDialog *)data;
    char *helpId = NULL;
    char *buf = new char[2048];

    smd->_first_time = TRUE;
    smd->_takeDown = FALSE;

    // pid is the child process (sendmail) id
    // status is the exit status of the child process
    // data is any extra data associated with the child process
 
    switch (status) {
        case DTME_NoError:
	    // The mail was successfully sent so return the compose
	    // window to the cache and then return.
	    smd->_send_button->activate();
	    smd->_close_button->activate();
	    smd->goAway(FALSE);
	    return;

        case DTME_BadMailAddress:
	    /* NL_COMMENT
	     * There was an error in one or more of the email addresses.  
	     * Ask the user to type in a valid address and try again.
	     */
	    sprintf(buf, GETMSG(DT_catd, 5, 5, 
"Some of the addresses in the message are incorrect,\n\
and do not refer to any known users in the system.\n\
Please make sure all of the addresses are valid and try again."));
  	    helpId = DTMAILHELPBADADDRESS;

	    break;
 
        case DTME_NoMemory:
	    /* NL_COMMENT
	     * Mailer ran out of memory.  Ask the user to quit some other
	     * applications so there will be more memory available.
	     */
 
            sprintf(buf, GETMSG(DT_catd, 5, 6,
"Mailer does not have enough memory\n\
available to send this message.\n\
Try quitting other applications and\n\
resend this message."));
            helpId = DTMAILHELPNOMEMORY;
		
	    break;

	case DTME_TransportFailed:
	default:
	    /* NL_COMMENT
	     * There was an error from the mail transport (sendmail).
	     */

	    sprintf(buf, GETMSG(DT_catd, 5, 7,
"An error occurred while trying to send your message.\n\
Check to make sure the message was received.  If not,\n\
you may have to resend this message."));
	    helpId = DTMAILHELPTRANSPORTFAILED;
    }

    // popup the compose window
    smd->manage();
    smd->_send_button->activate();
    smd->_close_button->activate();

    // popup the error dialog
    smd->_genDialog->setToErrorDialog(GETMSG(DT_catd, 2, 21, "Mailer"),
			     (char *) buf);
    smd->_genDialog->post_and_return(GETMSG(DT_catd, 3, 76, "OK"), helpId);
    delete [] buf;
}


void
SendMsgDialog::send_message(const char * trans_impl, int trans_type)
{
    DtMailEnv mail_exec_error;
    DtMailOperationId id;
    DtMail::Transport * mail_transport;
    DtMail::Session * d_session = theRoamApp.session()->session();
    DtMailEditor *editor = this->get_editor();
    AttachArea *attachArea = editor->attachArea();
    int numPendingActions, answer;
    char *helpId = NULL;
    char *buf = new char[2048];
    static int first_time = 1;

    //
    // Check to see if we are already trying to send from this
    // SendMsgDialog.  If we are, we don't want to do it again.
    if (_already_sending) {
	delete [] buf;
	return;
    } else {
	_already_sending = TRUE;
    }

    // First remove (unmap) the window;
    // send the message.
    // If you do it in the reverse order, users get confused coz
    // the window remains behind for a couple seconds after hitting 
    // "Send" and it ...
    this->unmanage();
    _send_button->deactivate();
    _close_button->deactivate();

    mail_exec_error.clear();
    

    // Check if message has addressees.  If it doesn't, what sense does
    // it make to Send it? 

    if (!this->hasAddressee()) {
	// Message has no valid addressee.  Pop up error dialog.

 	sprintf(buf, GETMSG(DT_catd, 5, 8,
		 "Try Send after specifying  recipient(s) of the message in \nthe To:, Cc:, or Bcc: fields."));

	helpId = DTMAILHELPNEEDADDRESSEE;

        // Need help tag for above HelpID.
	
	// popup the compose window
	this->manage();
	_send_button->activate();
	_close_button->activate();

	_genDialog->setToErrorDialog(GETMSG(DT_catd, 2, 21, "Mailer"),
				     (char *) buf);
	_genDialog->post_and_return(GETMSG(DT_catd, 3, 76, "OK"), helpId);

	// Reset the flag before we return.
	_already_sending = FALSE;

	delete [] buf;
	return;
    }

    // Since we are Send-ing, the SMD can be taken down later without
    // checking for dirty...

    _first_time = TRUE;
    _takeDown = TRUE;

    stopAutoSave();
    
    // Just get text from text widget; attachment BPs are filled
    // already when they are included/forwarded/added.
    
    updateMsgHnd();
    
    // Check if there are any pending attachments (open, print....)
    // If there are, pop up the dialog.
    // If the user wants to Send the message as is, continue with  the
    // submission process. 
    // If the user opted to Cancel, then return.
    
    
    numPendingActions = attachArea->getNumPendingActions();
    sprintf(buf, GETMSG(
	                DT_catd, 
	                3, 
	                77, 
                        "You have an attachment open that may have unsaved changes.\nSending this message will break the connection to the open\n attachment. Any unsaved changes will not be part of the\n message. You can use Save As to save Changes after the\n connection is broken, but the changes will not be part of\n the attachment." ));
    
    while (numPendingActions != 0) {
	// popup the compose window
	this->manage();
	_send_button->activate();
	_close_button->activate();

	/* NL_COMMENT
	 * The user tried to send a messages without saving changes in
	 * some open attachments.  This warning makes sure that is what
	 * the user intended.
	 */
	
	_genDialog->setToQuestionDialog(
			GETMSG(DT_catd, 5, 1, "Mailer"),
			buf);
	helpId = DTMAILHELPPENDINGACTIONS;
	
	answer = _genDialog->post_and_return(helpId);
	
	if (answer == 1) {
	    // OK selected
	    numPendingActions = 0;
	    this->unmanage();
	    _send_button->deactivate();
	    _close_button->deactivate();
	}
	else if (answer == 2) {
	    // Cancel selected
	    // Reset the flag before we return.
	    _already_sending = FALSE;
	    delete [] buf;
	    return;
	}
    }
   
    // Determine which transport mechanism will be used.
    if ( trans_type ) {   // Default
	// Only register XtAppAddInput once
        if (first_time)
        {
	    // Create the pipe between the RFCTransport::childHandler
	    // and XtAppAddInput
	    if (pipe(_transfds) < 0) {
	        // If this failed, make sure we try to initialize again later.
		mail_exec_error.setError(DTME_NoMemory); 	
    		popupMemoryError (mail_exec_error);

		// Reset the flag before we return.
		_already_sending = FALSE;
	        delete [] buf;
		return;
	    }

	    // Call ourproc when input is available on _transfds[0]
	    XtAppAddInput(XtWidgetToApplicationContext(this->_main_form),
		_transfds[0], (XtPointer)XtInputReadMask,
	        (XtInputCallbackProc)
		    (theRoamApp.default_transport()->getSendmailReturnProc()),
		NULL);
	    first_time = 0;
	}

	// Tell the transport where the callback is
	theRoamApp.default_transport()->initTransportData( _transfds,
		&(SendMsgDialog::sendmailErrorProc), this);
	id = theRoamApp.default_transport()->submit(mail_exec_error,
		_msgHandle, _log_msg);

    } else {
	// Construct transport
	mail_transport = d_session->transportConstruct(mail_exec_error,
		trans_impl, RoamApp::statusCallback, this);

	// Only register XtAppAddInput once
        if (first_time)
        {
	    // Create the pipe between the RFCTransport::childHandler
	    // and XtAppAddInput
	    if (pipe(_transfds) < 0) {
	        // If this failed, make sure we try to initialize again later.
		mail_exec_error.setError(DTME_NoMemory); 	
    		popupMemoryError (mail_exec_error);

		// Reset the flag before we return.
		_already_sending = FALSE;
	        delete [] buf;
		return;
	    }

	    // Call ourproc when input is available on _transfds[0]
	    XtAppAddInput(XtWidgetToApplicationContext(this->_main_form),
		_transfds[0], (XtPointer)XtInputReadMask,
		(XtInputCallbackProc)(mail_transport->getSendmailReturnProc()),
		NULL);
	    first_time = 0;
	}

	// Tell the transport where the callback is
	mail_transport->initTransportData(_transfds,
		&(SendMsgDialog::sendmailErrorProc), this);
	id = mail_transport->submit(mail_exec_error, _msgHandle, _log_msg);
    }
  
    popupMemoryError (mail_exec_error);

    // Reset the flag before we return.
    _already_sending = FALSE;

    delete [] buf;
}

void
SendMsgDialog::popupMemoryError(DtMailEnv &error)
{
    char *helpId = NULL;
    char *buf = new char[2048];

    _takeDown = FALSE;

    // Popup an error dialog if necessary. 
    if (error.isSet()) {
	if ((DTMailError_t)error == DTME_NoMemory) {

	    /* NL_COMMENT
	     * Mailer ran out of memory.  Ask the user to quit some other
	     * applications so there will be more memory available.
	     */
	    sprintf(buf, GETMSG(DT_catd, 5, 6,
"Mailer does not have enough memory\n\
available to send this message.\n\
Try quitting other applications and\n\
resend this message."));
	    helpId = DTMAILHELPNOMEMORY;
         }
	 else {
		/* NL_COMMENT
             * An unidentifiable error happened during mail transport
             * Pop it up *as is* (need to update this function if so)
             */
            sprintf(buf, "%s", (const char *)error);
            helpId = DTMAILHELPERROR;
        }

	// popup the compose window
	this->manage();
	_send_button->activate();
	_close_button->activate();

	// popup the error dialog
	this->_genDialog->setToErrorDialog(GETMSG(DT_catd, 2, 21, "Mailer"),
		(char *) buf);
	this->_genDialog->post_and_return(GETMSG(DT_catd, 3, 76, "OK"),
		helpId);
    }	
    delete [] buf;
}


Widget 
SendMsgDialog::createWorkArea ( Widget parent )
{
    FORCE_SEGV_DECL(CmdInterface, ci);
    Widget send_form;
    
    // Create the parent form
    
    _main_form = XmCreateForm( parent, "Work_Area", NULL, 0 );
    XtVaSetValues(_main_form, XmNresizePolicy, XmRESIZE_NONE, NULL);
    
    printHelpId("form", _main_form);
    /* add help callback */
    XtAddCallback(_main_form, XmNhelpCallback, HelpCB, (void *)DTMAILCOMPOSEWINDOW);
    XtVaSetValues(_main_form, XmNallowResize, True, NULL);
    
    
    // Create the area for status messages.
    //
    _status_form = XtVaCreateManagedWidget("StatusForm",
					   xmFormWidgetClass, _main_form,
					   XmNtopAttachment, XmATTACH_FORM,
					   XmNrightAttachment, XmATTACH_FORM,
					   XmNrightOffset, 2,
					   XmNleftAttachment, XmATTACH_FORM,
					   XmNleftOffset, 2,
					   NULL);

    _status_text = XtVaCreateManagedWidget("StatusLabel",
					   xmLabelWidgetClass, _status_form,
					   XmNtopAttachment, XmATTACH_FORM,
					   XmNbottomAttachment, XmATTACH_FORM,
					   XmNrightAttachment, XmATTACH_FORM,
					   XmNleftAttachment, XmATTACH_FORM,
					   XmNalignment, XmALIGNMENT_BEGINNING,
					   NULL);

    clearStatus();

    Widget s_sep = XtVaCreateManagedWidget("StatusSep",
					   xmSeparatorGadgetClass,
					   _main_form,
					   XmNtopAttachment, XmATTACH_WIDGET,
					   XmNtopWidget, _status_form,
					   XmNleftAttachment, XmATTACH_FORM,
					   XmNrightAttachment, XmATTACH_FORM,
					   NULL);
    
    _header_form = XtVaCreateManagedWidget("HeaderArea",
					   xmFormWidgetClass, _main_form,
					   XmNtopAttachment, XmATTACH_WIDGET,
					   XmNtopWidget, s_sep,
					   XmNleftAttachment, XmATTACH_FORM,
					   XmNrightAttachment, XmATTACH_FORM,
					   NULL);
    printHelpId("header_form", _header_form);

    createHeaders(_header_form);
    
    Widget sep1 = XtVaCreateManagedWidget("Sep1",
					  xmSeparatorGadgetClass,
					  _main_form,
					  XmNtopAttachment, XmATTACH_WIDGET,
					  XmNtopWidget, _header_form,
					  XmNtopOffset, 1,
					  XmNrightAttachment, XmATTACH_FORM,
					  XmNleftAttachment, XmATTACH_FORM,
					  NULL);

    // Create the editor and attach it to the header_form
    
    _my_editor = new DtMailEditor(_main_form, this);
    
    _my_editor->initialize();
    _my_editor->attachArea()->setOwnerShell(this);
    _my_editor->setEditable(TRUE);
    _my_editor->manageAttachArea();
    
    // Create a RowCol widget that contains buttons
    
    send_form =  XtCreateManagedWidget("SendForm", 
				       xmFormWidgetClass, _main_form, NULL, 0);
    
    
    // Create the Send and Close buttons as children of rowCol
    
    _send_button = new SendCmd ( "Send", 
	                         GETMSG(DT_catd, 1, 230, "Send"), 
	                         TRUE, 
	                         this, 
	                         1);
    ci  = new ButtonInterface (send_form, _send_button);
    
    XtVaSetValues(ci->baseWidget(),
		  XmNleftAttachment, XmATTACH_FORM,
		  XmNleftOffset, OFFSET,
		  XmNbottomAttachment, XmATTACH_FORM,
		  XmNbottomOffset, 5,
		  NULL);
    
    Widget send_bw = ci->baseWidget();
    XtManageChild(send_bw);
    ci->manage();
    
    _close_button = new CloseCmd (
				  "Close",
	                          GETMSG(DT_catd, 1, 118, "Close"),
				  TRUE, 
				  this->baseWidget(), 
				  this );
    ci = new ButtonInterface (send_form, _close_button);
    XtVaSetValues(ci->baseWidget(),
		  XmNleftOffset, 20,
		  XmNleftAttachment, XmATTACH_WIDGET,
		  XmNleftWidget, send_bw,
		  XmNbottomAttachment, XmATTACH_FORM,
		  XmNbottomOffset, 5,
		  NULL);
    
    XtManageChild(ci->baseWidget());
    ci->manage();
    
    // Now attach the editor to the form and to the rowCol
    // And the rowCol to the bottom of the form.
    // We need this attachment ordering so that resizes always
    // get transferred to the editor.
    
    Widget wid = _my_editor->container();
    XtVaSetValues(wid,
		  XmNleftAttachment, XmATTACH_FORM,
		  XmNrightAttachment, XmATTACH_FORM,
		  XmNtopAttachment, XmATTACH_WIDGET,
		  XmNtopWidget, sep1,
		  XmNtopOffset, 1,
		  XmNbottomAttachment, XmATTACH_WIDGET,
		  XmNbottomWidget, send_form,
		  NULL);
    
    XtVaSetValues(send_form,
		  XmNbottomAttachment, XmATTACH_FORM,
		  NULL);
    
    
    // Set focus 
    HeaderList * hl = _header_list[0];
    (void) XmProcessTraversal(hl->field_widget, XmTRAVERSE_CURRENT);
    
    // Set the title to be New Message
    //char *ttl = GETMSG(DT_catd, 1, 119, "New Message");
    //this->setTitle(ttl);
    //this->setIconTitle(ttl);
    
    XtManageChild(_main_form);
    return _main_form;
}


void
SendMsgDialog::createHeaders(Widget header_form)
{
    Widget	previous_form = NULL;
    char	*field_name = new char[50];

    for (int header = 0; header < _header_list.length(); header++) {
	HeaderList * hl = _header_list[header];

	// We use SMD_NEVER to indicate the header has disappeared from
	// the list.
	//
	if (hl->show == SMD_NEVER) {
	    continue;
	}

	// If the widgets already exist, then simply manage them.
	if (hl->form_widget) {
	    previous_form = hl->form_widget;
	    if (hl->value)
	    	XtVaSetValues(hl->field_widget,
                          XmNvalue, hl->value,
                          NULL);
 	    else
	    	XtVaSetValues(hl->field_widget,
                          XmNvalue, "",
                          NULL);
	    continue;
	}

	if (previous_form == NULL) {
	    // Create a form, attaching it to the top. This is a special
	    // case. Other lines are created attached to the form above
	    // them.
	    strcpy(field_name, "form_");
	    strncat(field_name, hl->label, 45);
	    field_name[strlen(hl->label) + 5] = 0;
	    hl->form_widget =
		XtVaCreateWidget(field_name,
				 xmFormWidgetClass,
				 header_form,
				 XmNtopAttachment, XmATTACH_FORM,
				 XmNtopOffset, 2,
				 XmNleftAttachment, XmATTACH_FORM,
				 XmNleftOffset, 3,
				 XmNrightAttachment, XmATTACH_FORM,
				 XmNrightOffset, 3,
				 NULL);

	}
	else {
	    strcpy(field_name, "form_");
	    strncat(field_name, hl->label, 45);
	    field_name[strlen(hl->label) + 5] = 0;
	    hl->form_widget =
		XtVaCreateWidget(field_name,
				 xmFormWidgetClass,
				 header_form,
				 XmNtopAttachment, XmATTACH_WIDGET,
				 XmNtopWidget, previous_form,
				 XmNleftAttachment, XmATTACH_FORM,
				 XmNleftOffset, 3,
				 XmNrightAttachment, XmATTACH_FORM,
				 XmNrightOffset, 3,
				 NULL);

	}

	// The label will be to the left of the form.
	//
	strcpy(field_name, hl->label);
	strcat(field_name, ":");
	XmString label = XmStringCreateLocalized(field_name);
	hl->label_widget =
	    XtVaCreateManagedWidget(hl->label,
				    xmLabelWidgetClass,
				    hl->form_widget,
				    XmNtopAttachment, XmATTACH_FORM,
				    XmNbottomAttachment, XmATTACH_FORM,
				    XmNleftAttachment, XmATTACH_FORM,
				    XmNlabelString, label,
				    NULL);
	XmStringFree(label);
	
	strcpy(field_name, "field_");
	strncat(field_name, hl->label, 43);
	field_name[strlen(hl->label) + 6] = 0;
	
	hl->field_widget =
	    XtVaCreateManagedWidget(field_name,
				    xmTextFieldWidgetClass,
				    hl->form_widget,
				    XmNtraversalOn,	True,
				    XmNtopAttachment, XmATTACH_FORM,
				    XmNrightAttachment, XmATTACH_FORM,
				    XmNleftAttachment, XmATTACH_WIDGET,
				    XmNleftWidget, hl->label_widget,
				    NULL);

	if (hl->show != SMD_HIDDEN) {
	    XtManageChild(hl->form_widget);
	}
	else {
	    XtVaSetValues(hl->form_widget,
			  XmNtopAttachment, XmATTACH_NONE,
			  NULL);
	}

	XtAddCallback(hl->field_widget,
		      XmNactivateCallback,
		      header_form_traverse,
		      NULL);

	XtAddCallback(hl->field_widget,
		      XmNvalueChangedCallback,
		      headerValueChanged,
		      this);

	if (hl->value) {
	    XtVaSetValues(hl->field_widget,
			  XmNvalue, hl->value,
			  NULL);
	}

	previous_form = hl->form_widget;
    }

    justifyHeaders();
    delete [] field_name;
}

void
SendMsgDialog::doDynamicHeaderMenus(void)
{
    // This is really a pain, but we have to blow away the list to
    // build another one. This could probably be done more efficiently,
    // but we wont try to figure out how right now.
    //
    if (_format_cmds) {
	_menuBar->removeOnlyCommands(_format_menu, _format_cmds);
    }

    _format_cmds = new CmdList("DynamicFormatCommands", "DynamicFormatCommands");

    // Only put on commands that are shown or hidden. The items that
    // are always are never should not be presented to the user as
    // an option to change.
    //
    char *label = new char[100];

    for (int h = 0; h < _header_list.length(); h++) {
	HeaderList * hl = _header_list[h];

	switch(hl->show) {
	  case SMD_SHOWN:
      	    sprintf(label, "%s ", GETMSG(DT_catd, 1, 229, "Delete"));
	    break;

	  case SMD_HIDDEN:
            sprintf(label, "%s ", GETMSG(DT_catd, 1, 228, "Add"));
	    break;

	  default:
	    continue;
	}

	strcat(label, hl->label);
	strcat(label, ":");

	char * priv_label = strdup(label);

	Cmd * new_cmd = new HideShowCmd(priv_label, priv_label,
					1, this, hl->label);

	// Add the commands one at a time with addCommand() vs. all
	// at once with addCommands(). That way new commands will
	// be created instead of reusing old ones.
    	_menuBar->addCommand(_format_menu, new_cmd);
	_format_cmds->add(new_cmd);
    }
    delete [] label;
}

void
bogus_cb(void *)
{
// Should theInfoDialogManager be destroyed here ???
}

#ifdef DEAD_WOOD
void
SendMsgDialog::open_att_cb( void *clientData, char *selection )
{
    SendMsgDialog *obj = (SendMsgDialog *)clientData;
    
    obj->open_att(selection);
}

void
SendMsgDialog::open_att( char *)  // arg is char *selection
{
}
#endif /* DEAD_WOOD */

void
SendMsgDialog::include_file_cb( void *client_data, char *selection )
{
    SendMsgDialog *obj = (SendMsgDialog *)client_data;
    obj->include_file(selection);
    if (NULL != selection)
      XtFree(selection);
    
}

void
SendMsgDialog::include_file(
			    char *selection
			    )
{
    FILE *fp;
    char *buf = new char[MAXPATHLEN];
    
    // I don't need to open the file to see if it's readable if loadFile()
    // returns error status.
    if ( (fp = fopen(selection, "r")) == NULL ) {
	sprintf(buf, GETMSG(DT_catd, 2, 18, "Error: Cannot include file %s"), 
		selection);
	theInfoDialogManager->post(
				   "Mailer", 
				   buf, 
				   (void *)this->_file_include, 
				   bogus_cb);
    } else {
	fclose(fp);
	this->_my_editor->textEditor()->append_to_contents("\n", 2);
	this->_my_editor->textEditor()->append_at_cursor(selection);
	this->_my_editor->textEditor()->append_to_contents("\n", 2);
    }
    delete [] buf;
}

int
SendMsgDialog::get_confirm_attachment_threshold()
{
    DtMailEnv        error;
    DtMail::Session *m_session = theRoamApp.session()->session();
    const char      *value = NULL;
    int              threshold;

    m_session->mailRc(error)->getValue(error, "confirmattachments", &value);
    if (error.isSet()) return 0;

    m_session->mailRc(error)->getValue(error, "confirmattachmentthreshold",
					&value);
    if (error.isNotSet() && NULL!=value)
      threshold = 1024 * atoi(value);
    else
      threshold = 1024 * 64;
    
    return threshold;
}

int
SendMsgDialog::confirm_add_attachment(char *file, int size)
{
    char *buf = new char[BUFSIZ];
    char *format;
    int   answer;
	
    format =
      GETMSG(DT_catd, 1, 263,
	     "The attachment '%s' is %d kilobytes.\nAdd as attachment?");
    sprintf(buf, format, file, size/1024);
    _genDialog->setToQuestionDialog(GETMSG(DT_catd, 5, 2, "Mailer"), buf);
    answer = _genDialog->post_and_return(NULL);
    return (answer==1);
}

void
SendMsgDialog::add_att_cb( void *client_data, char *selection )
{
    SendMsgDialog *obj = (SendMsgDialog *)client_data;
    obj->add_att(selection);
    if (NULL != selection)
      free(selection);
}

void
SendMsgDialog::add_att(char *file)
{
    struct stat statbuf;

    if (-1 != stat(file, &statbuf) && _confirm_attachment_threshold &&
        _confirm_attachment_threshold < statbuf.st_size)
    {
        if (! confirm_add_attachment(file, statbuf.st_size)) return;
    }

    // Activate Attachment menu???
    this->get_editor()->attachArea()->
      addAttachment(_msgHandle, _lastAttBP, file, NULL);
    this->setLastAttBP();
    this->activate_default_attach_menu();

    // This will manage the attach pane too.
    ((ToggleButtonCmd *)_att_show_pane)->setButtonState(TRUE, TRUE);
}

void
SendMsgDialog::add_att(char *name, DtMailBuffer buf)
{
    if (_confirm_attachment_threshold &&
	_confirm_attachment_threshold < buf.size)
    {
        if (! confirm_add_attachment("", buf.size)) return;
    }

    this->get_editor()->attachArea()->
      addAttachment(_msgHandle, _lastAttBP, name, buf);
    this->setLastAttBP();
    this->activate_default_attach_menu();
    
    // This will manage the attach pane too.
    ((ToggleButtonCmd *)_att_show_pane)->setButtonState(TRUE, TRUE);
}

void
SendMsgDialog::add_att(DtMailBuffer buf)
{
    char *name = NULL;
    add_att(name, buf);
}

void
SendMsgDialog::save_att_cb( void *client_data, char *selection )
{
    SendMsgDialog *obj = (SendMsgDialog *)client_data;
    
    obj->save_selected_attachment(selection);
    
}

void
SendMsgDialog::save_selected_attachment(
					char *selection
					)
{
    DtMailEnv mail_error;
    
    mail_error.clear();
    
    AttachArea *attarea = this->get_editor()->attachArea();
    Attachment *attachment = attarea->getSelectedAttachment(); 
    
    // Get selected attachment, if none selected, then return.
    if ( attachment == NULL ) {
	// Let User know that no attachment has been selected???
	int answer = 0;
	char *helpId = NULL;
	
	
	_genDialog->setToErrorDialog(
				     GETMSG(DT_catd, 1, 120, "Mailer"),    
				     GETMSG(DT_catd, 2, 19, "An attachment needs to be selected before issuing the\n\"Save As\" command to save to a file.") );
	helpId = DTMAILHELPSELECTATTACH;
	answer = _genDialog->post_and_return(
					     GETMSG(DT_catd, 3, 74, "OK"), helpId );
	return;
    }
    
    // Save selected attachments.
    attachment->saveToFile(mail_error, selection);
    if ( mail_error.isSet() ) {
	// Let User know error condition???
	return;
    }
}

void
SendMsgDialog::propsChanged(void)
{
    DtMail::Session *m_session = theRoamApp.session()->session();
    const char * value = NULL;
    DtMailEnv error;

    enableWorkAreaResize();

    m_session->mailRc(error)->getValue(error, "hideattachments", &value);
    if (value) {
	if (_show_attach_area) {
        	_show_attach_area = FALSE;
        	this->hideAttachArea();
	}
    }
    else 
	if (!_show_attach_area) {
        	_show_attach_area = TRUE;
		this->showAttachArea();
	}
    if (NULL != value)
      free((void*) value);

    _confirm_attachment_threshold = get_confirm_attachment_threshold();

    // Reset Log State
    const char * logfile = NULL;
    m_session->mailRc(error)->getValue(error, "record", &logfile);
    if (logfile == NULL)
	_file_log->deactivate();
    else
	_file_log->activate();

    _my_editor->textEditor()->update_display_from_props();

    m_session->mailRc(error)->getValue(error, "dontlogmessages", &value);
    if (logfile == NULL || error.isNotSet()) {
        // logfile is not specified or "dontlogmessages" is TRUE
        setLogState(DTM_FALSE);
        ((ToggleButtonCmd *)_file_log)->setButtonState(FALSE, TRUE);
    } else {
        // logfile is specified and "dontlogmessages" is FALSE
        setLogState(DTM_TRUE);
        ((ToggleButtonCmd *)_file_log)->setButtonState(TRUE, TRUE);
    }

    if (NULL != logfile)
      free((void*) logfile);
    if (NULL != value)
      free((void*) value);

    value = NULL;
    m_session->mailRc(error)->getValue(error, "templates", &value);
    if  ( (value == NULL && _templateList != NULL) ||
          (value != NULL &&
          (_templateList == NULL || strcmp(value, _templateList)) != 0) ) {
	// Template list has changed
        if (_templateList != NULL)
		free (_templateList);
	if (value != NULL && *value != '\0')
  		_templateList = strdup(value);
	else
		_templateList = NULL;
    	createFormatMenu();
    }
    if (NULL != value)
      free((void*) value);

    // Alias Popup Menus
    DtVirtArray<PropStringPair*> 	*newAliases;
    Boolean				aliasesChanged = FALSE;
    
    newAliases = new DtVirtArray<PropStringPair*> (10);
    createAliasList(newAliases);
    if (newAliases->length() == _aliasList->length())
    {
	int	length = newAliases->length();
	int	i = 0;

	while (i<length && aliasesChanged==FALSE)
	{
	    PropStringPair	*p1 = (*newAliases)[i];
	    PropStringPair	*p2 = (*_aliasList)[i];

	    if ( strncmp(p1->label, p2->label, strlen(p2->label)) ||
	         strncmp(p1->value, p2->value, strlen(p2->value)) )
	      aliasesChanged = TRUE;

	    i++;
	}
    }
    else
      aliasesChanged = TRUE;
    
    if (aliasesChanged == TRUE)
    {
        destroyAliasPopupMenus();
	destroyAliasList(_aliasList);
	_aliasList = newAliases;
        createAliasPopupMenus();
    }
    else
      destroyAliasList(newAliases);

    disableWorkAreaResize();
}

void
SendMsgDialog::createMenuPanes()
{
    CmdList *cmdList;
    Cardinal n = 0;
    DtMailEnv error;
    const char * value = NULL;

    
    _separator = new SeparatorCmd( "Separator","Separator", TRUE );
    
    // File
    cmdList = new CmdList( "File", GETMSG(DT_catd, 1, 121, "File") );
    
    // Default directory is set below at the same time as the default
    // directory for att_add.
    _file_include   = new UnifiedSelectFileCmd (
				 "Include...",
				 GETMSG(DT_catd, 1, 122, "Include..."),
				 GETMSG(DT_catd, 1, 123, "Mailer - Include"),
				 GETMSG(DT_catd, 1, 124, "Include"),
				 TRUE, 
				 SendMsgDialog::include_file_cb, 
				 this,
				 this->baseWidget());
    

    // Remap OK button to Include
    // XtVaSetValues(_file_include->fileBrowser, 
    //    XmNokLabelString, GETMSG(DT_catd,
    //    1, 77, "Include"), NULL);
    _file_save_as = new SaveAsTextCmd(
			      "Save As Text...",
			      GETMSG(DT_catd, 1, 125, "Save As Text..."),
			      GETMSG(DT_catd, 1, 126, "Mailer - Save As Text"),
			      TRUE,
			      get_editor()->textEditor(),
			      this,
			      baseWidget());
    
    _file_log = new LogMsgCmd ( 
			"Log Message",
			GETMSG(DT_catd, 1, 127, "Log Message"), TRUE, this);
    
    // 1 for default transport.
    
    _file_send = new SendCmd (
		 "Send",
                 GETMSG(DT_catd, 1, 117, "Send"), 
                 TRUE, 
                 this, 
                 1 );
    
    // Find out how many transports there are and build sub menu dynamically.
    DtMail::Session *d_session;

    if ( theRoamApp.session() == NULL ) {
	 MailSession *new_session = new MailSession(
						error, 
						theApplication->appContext());
	 theRoamApp.setSession(new_session);
     }

    CmdList *subcmdList1 = new CmdList (
					"Send As",
					GETMSG(DT_catd, 1, 128, "Send As") );
    
    d_session = theRoamApp.session()->session();
    const char **impls = d_session->enumerateImpls(error);
    
    for ( int impl = 0;  impls[impl];  impl++ ) {
	DtMailBoolean trans;
	d_session->queryImpl(error, impls[impl],
			     DtMailCapabilityTransport, &trans);
	if (!error.isSet() && trans == DTM_TRUE ) {
	    _file_sendAs[_num_sendAs] = new SendCmd( strdup(impls[impl]), 
						     (char *)impls[impl],
						     TRUE, 
						     this, 
						     0 );
	    subcmdList1->add( _file_sendAs[_num_sendAs] );
	    _num_sendAs++;
	}
	// Assume an error means this query failed.  But keep going and
	// get the next transport.
    }
    
    _file_close = new CloseCmd ( 
				 "Close",
				 GETMSG(DT_catd, 1, 129, "Close"), 
				 TRUE, 
				 _menuBar->baseWidget(), 
				 this );
    
    // Now build the menu
    
    cmdList->add( _file_include );
    cmdList->add( _file_save_as );
    cmdList->add( _file_log );
    cmdList->add( _separator );
    
    cmdList->add( _file_send );
#if defined(USE_SEND_AS_MENU)
    cmdList->add( subcmdList1 );
#endif
    cmdList->add( _separator );
    
    cmdList->add( _file_close );
    
    _menuBar->addCommands ( cmdList );
    delete subcmdList1;
    delete cmdList;
    
    // Edit
    
    cmdList = new CmdList( "Edit", GETMSG(DT_catd, 1, 130, "Edit") );
    
    _edit_undo = new EditUndoCmd ( "Undo",
					GETMSG(DT_catd, 1, 131, "Undo"),
					TRUE, this );
    _edit_cut = new EditCutCmd ( "Cut",
					GETMSG(DT_catd, 1, 132, "Cut"),
					TRUE, this );
    _edit_copy = new EditCopyCmd ( "Copy",
					GETMSG(DT_catd, 1, 133, "Copy"),
					TRUE, this );
    _edit_paste = new EditPasteCmd ( "Paste",
					GETMSG(DT_catd, 1, 134 , "Paste"), 
					TRUE, 
					this );
    
    // Begin Paste Special submenu
    subcmdList1 = new CmdList ( "Paste Special", GETMSG(DT_catd, 1, 135 , "Paste Special") );
    _edit_paste_special[0] = new EditPasteSpecialCmd (
						      "Bracketed",
						      GETMSG(DT_catd, 1, 136 , "Bracketed"),
						      TRUE, this, Editor::IF_BRACKETED
						      );
    subcmdList1->add(_edit_paste_special[0]);
    _edit_paste_special[1] = new EditPasteSpecialCmd (
						      "Indented",
						      GETMSG(DT_catd, 1, 137 , "Indented"), 
						      TRUE, this, Editor::IF_INDENTED );
    subcmdList1->add(_edit_paste_special[1]);
    // End Paste Special submenu
    
    _edit_clear = new EditClearCmd ( "Clear", GETMSG(DT_catd, 1, 138, "Clear"), 
				     TRUE, 
				     this );
    _edit_delete = new EditDeleteCmd ( "Delete", GETMSG(DT_catd, 1, 139, "Delete"), 
				       TRUE, 
				       this );
    _edit_select_all = new EditSelectAllCmd ( 
					      "Select All",
					      GETMSG(DT_catd, 1, 140, "Select All"), 
					      TRUE, 
					      this );
    _format_find_change = new FindChangeCmd ( 
					      "Find/Change...",
					      GETMSG(DT_catd, 1, 155, "Find/Change..."), 
					      TRUE, 
					      this );
    _format_spell = new SpellCmd (
				  "Check Spelling...",
				  GETMSG(DT_catd, 1, 156, "Check Spelling..."), 
				  TRUE, 
				  this );

    cmdList->add( _edit_undo );
    cmdList->add( _separator );
    cmdList->add( _edit_cut );
    cmdList->add( _edit_copy );
    cmdList->add( _edit_paste );
    cmdList->add( subcmdList1 ); // Add Paste Special submenu
    cmdList->add( _separator );
    cmdList->add( _edit_clear );
    cmdList->add( _edit_delete );
    cmdList->add( _separator );
    cmdList->add( _edit_select_all );
    cmdList->add( _separator );
    cmdList->add( _format_find_change );
    /*
     *
     * SpellCheck is not supported by Base System for the multibyte language
     * currently. ( See dtpad's source ) So that this should be disabled.
     * See Defect 174873. (I should think this solution is not good one, but..)
     * What is the best way to check if I'm in MB or SB.....???
     *
     */
    if ( MB_CUR_MAX == 1 )
      cmdList->add( _format_spell );

    _menuBar->addCommands ( cmdList );
    delete subcmdList1;
    delete cmdList;

    // Alias Popup Menus
    if (NULL != _aliasList) delete _aliasList;
    _aliasList = new DtVirtArray<PropStringPair*> (10);
    createAliasList(_aliasList);
    createAliasPopupMenus();

    // Compose Popup CmdList
    construct_text_popup();
    
    // Attachment
    
    cmdList = new CmdList(
			"Attachments",
			GETMSG(DT_catd, 1, 141, "Attachments"));
    
    _att_add   = new UnifiedSelectFileCmd (
				"Add File...",
				GETMSG(DT_catd, 1, 142, "Add File..."),
				GETMSG(DT_catd, 1, 143, "Mailer - Add"), 
				GETMSG(DT_catd, 1, 144, "Add"),
				TRUE, 
				SendMsgDialog::add_att_cb,
				this,
				this->baseWidget());

    _att_save  = new SaveAttachCmd (
				"Save As...",
				GETMSG(DT_catd, 1, 145, "Save As..."),
				GETMSG(DT_catd, 1, 146,
					"Mailer - Attachments - Save As"),
				FALSE, 
				SendMsgDialog::save_att_cb,
				this,
				this->baseWidget());
    _att_delete = new DeleteAttachCmd (
				"Delete",
				GETMSG(DT_catd, 1, 147, "Delete"),
				FALSE, 
				this);
    _att_undelete = new UndeleteAttachCmd (
				"Undelete",
				GETMSG(DT_catd, 1, 148, "Undelete"),
				FALSE, 
				this);
    _att_rename = new RenameAttachCmd(
				"Rename",
				GETMSG(DT_catd, 1, 149, "Rename"),
				FALSE,
				this);
    
    _att_select_all = new SelectAllAttachsCmd( 
				"Select All",
				GETMSG(DT_catd, 1, 150, "Select All"), 
				this);
    
	/* NL_COMMENT
	 * This is the label for a toggle item in a menu.  When the item
	 * is set to "Show List", the Attachment List is mapped in the
	 * Compose Window.  This message replaces message 151 in set 1.
	 */
    _att_show_pane = new ShowAttachPaneCmd(
					   "Show List",
					   GETMSG(DT_catd, 1, 226, "Show List"),
					   this
					   );
    cmdList->add( _att_add );
    cmdList->add( _att_save );
    cmdList->add( _separator );
    
//   subcmdList1 = new CmdList ( "Create", "Create" );
//   // subcmdList1->add( att_audio );
//   // subcmdList1->add( att_appt );
//   cmdList->add( subcmdList1 );
//   cmdList->add( _separator );
    
    cmdList->add( _att_delete );
    cmdList->add( _att_undelete );
    cmdList->add( _att_rename );
    cmdList->add( _att_select_all );
    cmdList->add(_att_show_pane);
    
    // Create a pulldown from the items in the list.  Retain a handle
    // to that pulldown since we need to dynamically add/delete entries 
    // to this menu based on the selection of attachments.
    
    _attachmentMenu = _menuBar->addCommands ( cmdList );
    construct_attachment_popup();
 
//  delete subcmdList1;
    delete cmdList;
    
    value = NULL;
    d_session->mailRc(error)->getValue(error, "templates", &value);
    if (value != NULL && *value != '\0')
	_templateList = strdup(value);
    if (NULL != value)
      free((void*) value);

    createFormatMenu();
    
    _overview = new OnAppCmd("Overview",
				GETMSG(DT_catd, 1, 71, "Overview"),
                                TRUE, this);
    _tasks = new TasksCmd("Tasks", GETMSG(DT_catd, 1, 72, "Tasks"), 
				TRUE, this);
    _reference = new ReferenceCmd("Reference",
				GETMSG(DT_catd, 1, 73, "Reference"), 
				TRUE, this);
    _on_item = new OnItemCmd("On Item", GETMSG(DT_catd, 1, 74, "On Item"),
                                TRUE, this);
    _using_help = new UsingHelpCmd("Using Help",
				GETMSG(DT_catd, 1, 75, "Using Help"), 
				TRUE, this);
    _about_mailer = new RelNoteCmd("About Mailer...",
				GETMSG(DT_catd, 1, 77, "About Mailer..."),
                                TRUE, this);
    cmdList = new CmdList("Help", GETMSG(DT_catd, 1, 76, "Help"));
    cmdList->add(_overview);
    cmdList->add(_separator);
    cmdList->add(_tasks);
    cmdList->add(_reference);
    cmdList->add(_separator);
    cmdList->add(_on_item);
    cmdList->add(_separator);
    cmdList->add(_using_help);
    cmdList->add(_separator);
    cmdList->add(_about_mailer);
    _menuBar->addCommands(cmdList, TRUE);
    delete cmdList;
}

void
SendMsgDialog::construct_attachment_popup(void)
{
   _attachmentPopupMenuList = new CmdList( "AttachmentsPopup", "AttachmentsPopup");

    LabelCmd *title     = new LabelCmd (
			"Mailer - Attachments",
			GETMSG(DT_catd, 1, 158, "Mailer - Attachments"), TRUE);
    SeparatorCmd *separator = new SeparatorCmd( "Separator","Separator", TRUE );

    _attachmentPopupMenuList->add(title);
    _attachmentPopupMenuList->add(separator);
    _attachmentPopupMenuList->add( _att_add );
    _attachmentPopupMenuList->add( _att_save );
    _attachmentPopupMenuList->add( _att_delete );
    _attachmentPopupMenuList->add( _att_undelete );
    _attachmentPopupMenuList->add( _att_select_all );

    _menuPopupAtt = new MenuBar(_my_editor->attachArea()->getClipWindow(), 
					"RoamAttachmentPopup", XmMENU_POPUP);
    _attachmentPopupMenu = _menuPopupAtt->addCommands(_attachmentPopupMenuList, 
				FALSE, XmMENU_POPUP);
}

void
SendMsgDialog::construct_text_popup(void)
{
   if (theApplication->bMenuButton() != Button3)
        return;

   _textPopupMenuList = new CmdList( "TextPopup", "TextPopup");

    LabelCmd *title     = new LabelCmd (
			"Mailer - Compose",
			GETMSG(DT_catd, 1, 159, "Mailer - Compose"), TRUE);
    SeparatorCmd *separator = new SeparatorCmd("Separator", "Separator", TRUE );

    _textPopupMenuList->add(title);
    _textPopupMenuList->add(separator);
    _textPopupMenuList->add(_file_send);
    _textPopupMenuList->add( _edit_undo );
    _textPopupMenuList->add( _edit_cut );
    _textPopupMenuList->add( _edit_copy );
    _textPopupMenuList->add( _edit_paste );
#ifdef __osf__
    // Work in progress from Mike. This adds the Paste Special to the
    // third mouse button in the compose area of a compose window.
    // Begin Paste Special submenu
    CmdList * subcmdList1 = new CmdList ( "Paste Special", GETMSG(DT_catd, 1, 135 , "Paste Special") );
    subcmdList1->add(_edit_paste_special[0]);
    subcmdList1->add(_edit_paste_special[1]);
    // End Paste Special submenu
    _textPopupMenuList->add( subcmdList1 ); // Add Paste Special submenu
    // (Either way) _textPopupMenuList->add( separator );
    _textPopupMenuList->add( _edit_clear );
#endif
    _textPopupMenuList->add( _edit_delete );
    _textPopupMenuList->add( _edit_select_all );

    Widget parent = _my_editor->textEditor()->get_editor();
    _menuPopupText = new MenuBar(parent, "SendMsgTextPopup", XmMENU_POPUP);
    _textPopupMenu = _menuPopupText->addCommands(_textPopupMenuList, 
				FALSE, XmMENU_POPUP);
}

static int cmp_prop_pair(const void *v1, const void *v2)
{
    PropStringPair      *p1 = *((PropStringPair **) v1);
    PropStringPair      *p2 = *((PropStringPair **) v2);
    int                 ret;

    ret =  strcmp((const char *) p1->label, (const char *) p2->label);
    return ret;
}

static void alias_stuffing_func(char * key, void * data, void * client_data)
{
    DtVirtArray<PropStringPair *>	*alias_list;
    PropStringPair			*new_pair;

    alias_list = (DtVirtArray<PropStringPair*> *) client_data;
    new_pair = new PropStringPair;
    new_pair->label = strdup(key);
    new_pair->value = strdup((char *)data);
    alias_list->append(new_pair);
}

void
SendMsgDialog::createAliasList(DtVirtArray<PropStringPair*> *aliases)
{
    DtMailEnv		error;
    DtMail::Session	*d_session = theRoamApp.session()->session();
    DtMail::MailRc	*mail_rc = d_session->mailRc(error);
    int			nalias = 0;

    mail_rc->getAliasList(alias_stuffing_func, aliases);

    if (nalias = aliases->length())
    {
	PropStringPair	**prop_pairs = NULL;

	prop_pairs = (PropStringPair**) malloc(nalias*sizeof(PropStringPair*));
	int i;
	for (i=0; i<nalias; i++)
	{
	    prop_pairs[i] = (*aliases)[0];
	    aliases->remove(0);
	}
	qsort(prop_pairs, nalias, sizeof(PropStringPair*), cmp_prop_pair);
	for (i=0; i<nalias; i++)
	  aliases->append(prop_pairs[i]);
	
	free((void*) prop_pairs);
    }
}

void
SendMsgDialog::destroyAliasList(DtVirtArray<PropStringPair*> *aliases)
{
    while (aliases->length() >= 0)
    {
	PropStringPair	*prop_pair = (*aliases)[0];
	aliases->remove(0);
	delete(prop_pair);
    }
}

// map_menu is used to figure out how many columns to split the menu
// into.  It is a callback that is called when the menu is mapped.
// If the menu is over half the height of the screen, it figures out
// how many columns to make the menu, and sets its XmNnumColumns
// attribute to that value.  It calculates the maximum number of columns
// that would fit and never goes beyond that number.

static void map_alias_menu(Widget menu, XtPointer, XtPointer)
{
    Position y;
    Dimension h, w;
    Dimension maxcols, newcols, columns;
    Dimension screenheight = (Dimension) HeightOfScreen(XtScreen(menu));
    Dimension fudgefact = 20; /* to allow for decorations on menu */

    XtVaGetValues(
		menu,
		XmNheight, &h,
		XmNwidth, &w,
		XmNy, &y,
		XmNnumColumns, &columns,
		NULL);

    if ((h + fudgefact) > (screenheight / 2))
    {
        // The menu is taller than half the screen. 
	// We need to find out how many more columns
	// to specify for the menu to make it fit.

        newcols = (columns * ((h+fudgefact)/(screenheight/2))) + 1;
        maxcols = WidthOfScreen(XtScreen(menu))/(w/columns);

        if (newcols > maxcols)
          newcols = maxcols;

        XtVaSetValues(menu, XmNnumColumns, newcols, NULL);
    }
}

void
SendMsgDialog::aliasMenuButtonHandler(
    				Widget,
    				XtPointer client_data,
    				XEvent *event,
    				Boolean *)
{
    Widget		menu = (Widget) client_data;
    XButtonEvent	*be = (XButtonEvent *) event;

    if (event->xany.type != ButtonPress) return;
    if(be->button == theApplication->bMenuButton())
    {
        XmMenuPosition(menu, (XButtonEvent *)event);
        XtManageChild(menu);
    }
}

Widget
SendMsgDialog::createAliasPopupMenu(
				Widget parent,
				MenuBar **menubar,
				CmdList **cmdlist,
				DtVirtArray<PropStringPair*> *aliases)
{
    Widget		menu = NULL;
    OtherAliasesCmd	*otherAliases =
			  new OtherAliasesCmd(
				"Other Aliases...",
				GETMSG(DT_catd, 1, 247, "Other Aliases..."),
				TRUE);
#if defined(USE_TITLED_ALIAS_POPUPS)
    LabelCmd		*title =
			  new LabelCmd(
				"Mailer - Aliases",
				GETMSG(DT_catd, 1, 248, "Mailer - Aliases"),
				TRUE);
#endif
    SeparatorCmd	*separator =
			  new SeparatorCmd("Separator","Separator", TRUE);

    (*cmdlist) = new CmdList("AliasCommands", "AliasCommands");
#if defined(USE_TITLED_ALIAS_POPUPS)
    (*cmdlist)->add(title);
    (*cmdlist)->add(separator);
#endif
    for (int i=0, length=aliases->length(); i<length; i++)
    {
	PropStringPair	*prop_pair = (*aliases)[i];

	AliasCmd *alias = new AliasCmd(
				strdup(prop_pair->label),
				strdup(prop_pair->label),
				TRUE,
				parent);
        (*cmdlist)->add(alias);
    }
    if (0 < aliases->length())
      (*cmdlist)->add(separator);
    (*cmdlist)->add(otherAliases);

    *menubar = new MenuBar(parent, "AliasesPopup", XmMENU_POPUP);
    menu = (*menubar)->addCommands((*cmdlist), FALSE, XmMENU_POPUP);

    XtAddEventHandler(
		parent,
		ButtonPressMask,
		FALSE,
		aliasMenuButtonHandler,
		(XtPointer) menu);

    XtVaSetValues(
		menu,
		XmNpacking, XmPACK_COLUMN,
		XmNorientation, XmVERTICAL,
		NULL);

    XtAddCallback(
		menu,
		XmNmapCallback, &map_alias_menu,
		NULL);

    return menu;
}

void
SendMsgDialog::destroyAliasPopupMenu(
				Widget	parent,
				MenuBar *menubar,
				CmdList *cmdlist,
				Widget	menu)
{
    XtRemoveEventHandler(
		parent,
		ButtonPressMask,
		FALSE,
		aliasMenuButtonHandler,
		(XtPointer) menu);

    XtRemoveCallback(
		menu,
		XmNmapCallback, &map_alias_menu,
		NULL);

    XtDestroyWidget(menu);

    delete menubar;
    delete cmdlist;
}

Widget
SendMsgDialog::getHeaderWidget(const char *hdrname)
{
    for (int i=0, length=_header_list.length(); i<length; i++)
    {
	HeaderList	*hdritem = _header_list[i];

	if (0 == strncmp(hdrname, hdritem->header, strlen(hdrname)))
	  return hdritem->field_widget;
    }

    return NULL;
}

void
SendMsgDialog::createAliasPopupMenus(void)
{
    Widget	w;

    w = getHeaderWidget(DtMailMessageTo);
    if (NULL != w)
      _toPopupMenu = createAliasPopupMenu(
					w,
					&_toPopupMenuBar,
					&_toPopupCmdlist,
					_aliasList);

    w = getHeaderWidget(DtMailMessageCc);
    if (NULL != w)
      _ccPopupMenu = createAliasPopupMenu(
					w,
					&_ccPopupMenuBar,
					&_ccPopupCmdlist,
					_aliasList);

    w = getHeaderWidget(DtMailMessageBcc);
    if (NULL != w)
      _bccPopupMenu = createAliasPopupMenu(
					w,
					&_bccPopupMenuBar,
					&_bccPopupCmdlist,
					_aliasList);
}

void
SendMsgDialog::destroyAliasPopupMenus(void)
{
    Widget	w;

    w = getHeaderWidget(DtMailMessageTo);
    destroyAliasPopupMenu(w, _toPopupMenuBar, _toPopupCmdlist, _toPopupMenu);
    _toPopupMenuBar = NULL;
    _toPopupCmdlist = NULL;
    _toPopupMenu = NULL;

    w = getHeaderWidget(DtMailMessageCc);
    destroyAliasPopupMenu(w, _ccPopupMenuBar, _ccPopupCmdlist, _ccPopupMenu);
    _ccPopupMenuBar = NULL;
    _ccPopupCmdlist = NULL;
    _ccPopupMenu = NULL;

    w = getHeaderWidget(DtMailMessageBcc);
    destroyAliasPopupMenu(w, _bccPopupMenuBar, _bccPopupCmdlist, _bccPopupMenu);
    _bccPopupMenuBar = NULL;
    _bccPopupCmdlist = NULL;
    _bccPopupMenu = NULL;
}

void
SendMsgDialog::createFormatMenu()
{
    CmdList *cmdList;
    _format_separator = new SeparatorCmd( "Separator","Separator", TRUE );

    cmdList = new CmdList( "Format", GETMSG(DT_catd, 1, 152,"Format") );
    
    _format_word_wrap = new WordWrapCmd ( 
					  "Word Wrap",
					  GETMSG(DT_catd, 1, 153, "Word Wrap"), 
					  TRUE, 
					  this );
    _format_settings = new FormatCmd ( "Settings...",
				       GETMSG(DT_catd, 1, 154, "Settings..."), 
				       TRUE, 
				       this );
    
    cmdList->add( _format_word_wrap );
    cmdList->add( _format_settings );
    cmdList->add( _format_separator);
    
    _templates = new CmdList ( "Templates", GETMSG(DT_catd, 1, 157, "Templates") );
    addTemplates(_templates);
    
    cmdList->add(_templates);
    
    cmdList->add( _format_separator );

    _format_menu = _menuBar->addCommands ( &_format_cascade, cmdList, 
			FALSE, XmMENU_BAR);

    delete cmdList;

    doDynamicHeaderMenus();

    if (_template_count == 0 && _templates->getPaneWidget())
    {
	XtSetSensitive(_templates->getPaneWidget(), FALSE);
    }
    
}

void
SendMsgDialog::addTemplates(CmdList * subCmd)
{
    DtMailEnv error;

    _template_count = 0;
    
    if (_templateList == NULL)
        return;

    DtMail::Session *m_session = theRoamApp.session()->session();
    char * expanded_list = m_session->expandPath(error, _templateList);

    DtVirtArray<PropStringPair *> templates(8);
    parsePropString(expanded_list, templates);
    free(expanded_list);

    _template_count = templates.length();

    for (int tmp = 0; tmp < _template_count; tmp++) {
	PropStringPair * psp = templates[tmp];
	if (psp->label && psp->value) {
	    Cmd * button = new TemplateCmd(strdup(psp->label), 
					   strdup(psp->label), 
					   1, 
					   this, 
					   psp->value);
	    subCmd->add(button);
	}
    }

    while (templates.length()) {
	PropStringPair * psp = templates[0];
	delete psp;
	templates.remove(0);
    }
}

void
SendMsgDialog::initialize()
{
    Cardinal n = 0;
    Arg args[1];
    const char * hideAttachPane = NULL;
    DtMailEnv error;
    
    // Without the TearOffModelConverter call, there will be warning messages:
    // Warning: No type converter registered for 'String' to 'TearOffModel' 
    // conversion.
    
    XmRepTypeInstallTearOffModelConverter();
    MenuWindow::initialize();

    char *ttl = GETMSG(DT_catd, 1, 160, "New Message");
    setTitle(ttl);

    XtSetArg(args[n], XmNdeleteResponse, XmDO_NOTHING); n++;
    XtSetValues( _w, args, n);
    
    _genDialog = new DtMailGenDialog("Dialog", _main);
    
    // See if the .mailrc specifies if attachPane is to be shown or hid 
    // at SMD startup time.
    
    DtMail::Session *m_session = theRoamApp.session()->session();
    m_session->mailRc(error)->getValue(error, "hideattachments", 
				&hideAttachPane);
    
    if (!hideAttachPane) {
	_show_attach_area = TRUE;
    }
    else {
	_show_attach_area = FALSE;
	// The user wants to hide attachments

	this->hideAttachArea();
    }
    if (NULL != hideAttachPane)
      free((void*) hideAttachPane);

    _confirm_attachment_threshold = get_confirm_attachment_threshold();
    
    // Log Message Toggle button.  A LogMsgCmd is a ToggleButtonCmd....
    const char * logfile = NULL;
    const char * value = NULL;
    m_session->mailRc(error)->getValue(error, "record", &logfile);
    if(logfile == NULL)
	_file_log->deactivate();
    else
	_file_log->activate();

    m_session->mailRc(error)->getValue(error, "dontlogmessages", &value);
    if (logfile == NULL || error.isNotSet()) {
	// logfile is not specified or "dontlogmessages" is TRUE
	setLogState(DTM_FALSE);
	((ToggleButtonCmd *)_file_log)->setButtonState(FALSE, TRUE);
    } else {
	// logfile is specified and "dontlogmessages" is FALSE
	setLogState(DTM_TRUE);
	((ToggleButtonCmd *)_file_log)->setButtonState(TRUE, TRUE);
    }
    
    if (NULL != logfile)
      free((void*) logfile);
    if (NULL != value)
      free((void*) value);

    // Word Wrap Toggle button.  A WordWrapCmd is a ToggleButtonCmd...
    ((ToggleButtonCmd *)_format_word_wrap)->setButtonState(
			   ((WordWrapCmd *)_format_word_wrap)->wordWrap(), 
			   FALSE
			   );

    // Initialize the Edit menu

    this->text_unselected();
	
    setIconName(ComposeIcon);
}

static void
Self_destruct(XtPointer, XtIntervalId *)
{
#ifdef WM_TT
    fprintf(stderr, "DEBUG: Self_destruct(): invoked!\n");
#endif
    
    
    XtRemoveAllCallbacks(
			 theApplication->baseWidget(), 
			 XmNdestroyCallback);
    delete theApplication;
}

// Clears Compose window title, header fields, text, and attachment areas.
void
SendMsgDialog::reset()
{
    _my_editor->textEditor()->clear_contents();
    _my_editor->attachArea()->resetPendingAction();

    _takeDown = FALSE;
    // This will deselect any Attachment action, if any available now.
    // Also deselect text menu items....

    this->deactivate_default_attach_menu();
    this->text_unselected();
    this->all_attachments_deselected();
    _att_undelete->deactivate();  // This needs to be done in addition

    this->get_editor()->attachArea()->removeCurrentAttachments();

    // Unmanage the dialog
    this->unmanage();

    if (_show_attach_area) { // .mailrc wants default attach area invisible

    // Unmanage the attach Area.  Set the show_pane button.
    // This is done because if we are caching this window (after 
    // unmanaging), we don't want the window to pop back up, on uncaching,
    // with the attachment pane visible, etc..

	this->showAttachArea();
    }
    else {
	this->hideAttachArea();
    }

    // Need to destroy current Message handle.
    delete _msgHandle;    // All its body parts are deleted.
    _msgHandle = NULL;
    _lastAttBP = NULL;    // So just set this to NULL.
    // Delete or set to NULL ???
    _inclMsgHandle = NULL; 
    _inclMsgHasText = 0;

    for (int clear = 0; clear < _header_list.length(); clear++) {
	HeaderList * hl = _header_list[clear];

        // Bugfix: Old selection area remained selected, after text cleared
        // and parent widget unmanged, and then managed again for next
        // Compose.  (So new text in old select area was still being selected).
        // Perhaps, this is a Motif bug ... but this fixes the problem.
        XmTextFieldClearSelection( hl->field_widget, CurrentTime );

	if (hl->value) {
	    XtVaSetValues(hl->field_widget,
			  XmNvalue, hl->value,
			  NULL);
	}
	else {
	    XtVaSetValues(hl->field_widget,
			  XmNvalue, "",
			  NULL);
	}
    }
    // Reset the Log state in case the user happened to change it.
    DtMail::Session *m_session = theRoamApp.session()->session();
    const char * logfile = NULL;
    const char * value = NULL;
    DtMailEnv error;

    m_session->mailRc(error)->getValue(error, "record", &logfile);
    if(logfile == NULL)
	_file_log->deactivate();
    else
	_file_log->activate();

    m_session->mailRc(error)->getValue(error, "dontlogmessages", &value);
    if (logfile == NULL || error.isNotSet()) {
        // logfile is not specified or "dontlogmessages" is TRUE
        setLogState(DTM_FALSE);
        ((ToggleButtonCmd *)_file_log)->setButtonState(FALSE, TRUE);
    } else {
        // logfile is specified and "dontlogmessages" is FALSE
        setLogState(DTM_TRUE);
        ((ToggleButtonCmd *)_file_log)->setButtonState(TRUE, TRUE);
    }
	
    if (NULL != logfile)
      free((void*) logfile);
    if (NULL != value)
      free((void*) value);
}

// Recycles Compose window.
void
SendMsgDialog::quit(Boolean delete_win)
{

    // There are several ways we could have reached here.
    // 1) From the user choosing Send.
    // 2) From the user clicking on the Close button or Close menu item
    // 3) The user choosing Close from window manager menu.
    // For (1), we just forge ahead.  For that, the _takeDown boolean
    // is set in send_message() method.
    // For (2), the boolean is set in goAway().
    // For (3), we call goAway() which sets the _takeDown depending on
    // a dialog negotiation if SMD has contents.

     if (_file_include->fileBrowser() != NULL) 
         XtPopdown(XtParent(_file_include->fileBrowser())); 
     if (_att_add->fileBrowser() != NULL) 
         XtPopdown(XtParent(_att_add->fileBrowser())); 

     if (_file_save_as->fileBrowser() != NULL) 
         XtPopdown(XtParent(_file_save_as->fileBrowser())); 
     if (_att_save->fileBrowser() != NULL) 
         XtPopdown(XtParent(_att_save->fileBrowser())); 

    if (!_takeDown) {
        // Check to see if it's the first time through the quit()
        // method.  Set _first_time to FALSE so that we don't come
        // down this path again until we're done quitting or bad
        // things will happen.
        if (_first_time == TRUE) {
            _first_time = FALSE;
            this->goAway(TRUE);
            // We're done quitting, so we can set _first_time to TRUE again.
            _first_time = TRUE;
        }
        return;
    }  

    stopAutoSave();
    
#ifdef DTMAIL_TOOLTALK
    //  For explanation of dtmail_mapped, look at RoamApp.h.
    if ( started_by_tt && (0 == theCompose.getTimeOutId()) &&
	 (theCompose.numUnusedWindows() == theCompose.numCreatedWindows()) &&
	 !dtmail_mapped )
    {
	int id;
	id = XtAppAddTimeOut(
			theApplication->appContext(), 
			(unsigned long)DESTRUCT_TIMEOUT, 
			Self_destruct, NULL);
	theCompose.putTimeOutId(id);
    }
#endif

    if (delete_win)
     delete this;
    else
    {
        this->reset();
        theCompose.putWin(this, FALSE);
    }
    
    //
    // If there are no composer timeouts, check if its time to shutdown.
    //
    if (0 == theCompose.getTimeOutId()) theRoamApp.checkForShutdown();
}

void
SendMsgDialog::panicQuit()
{
    //
    // Need to make sure the message is still valid before proceeding.
    // ::reset may have been called so the message may no longer be valid.
    //
    if (!isMsgValid())
      return;

    doAutoSave();
}


// Given a file name, include the file as attachment.
void
SendMsgDialog::inclAsAttmt(char *file, char *name)
{
    this->get_editor()->attachArea()->addAttachment(_msgHandle, _lastAttBP,
                                                    file, name);
    this->setLastAttBP();
    
}

// Given a buffer, include its content as an attachment.
void
SendMsgDialog::inclAsAttmt(unsigned char *contents, int len, char *name)
{
    DtMailBuffer mbuf;

    mbuf.buffer = (void *)contents;
    mbuf.size = (unsigned long)len;
    this->get_editor()->attachArea()->addAttachment(_msgHandle, _lastAttBP,
                                                    (String)name, mbuf);
    this->setLastAttBP();
}

// Given a RFC_822_Message formatted buffer, parse it and fill the Compose Window.
void
SendMsgDialog::parseNplace(char *contents, int len)
{
    // 1. Create message handle for contents

    DtMailEnv error;
    DtMail::Session * d_session = theRoamApp.session()->session();
    DtMailBuffer mbuf;
    
    mbuf.buffer = (void *)contents;
    mbuf.size = (unsigned long)len;
    
    DtMail::Message * msg = d_session->messageConstruct(error,
							DtMailBufferObject,
							&mbuf,
							NULL,
							NULL,
							NULL);
    if ( !msg ) {
	return;
    } else if ( error.isSet() ) {
	if  ( (DTMailError_t) error == DTME_UnknownFormat ) {
	    // The content does not have header info.  Therefore, store
	    // everything as text.
	    _my_editor->textEditor()->set_contents((const char *)mbuf.buffer,
						   mbuf.size);
	    return;
	}
    }

    char * status_string;
    DtMailBoolean first_bp_handled;
    first_bp_handled = _my_editor->textEditor()->set_message(
							     msg,
							     &status_string,
							     Editor::HF_NONE
							     );
    
    int num_bodyParts = msg->getBodyCount(error);

    // Don't use setInclMsgHnd() because it causes the SMD's attachments
    // to get out of sink with the BE.  Just assign the newly created message
    // to _msgHandle.
    //
    if ((num_bodyParts > 1) || (!first_bp_handled)) {
	int start;
	if (first_bp_handled) {
	    start = 2;
//	    setInclMsgHnd(msg, TRUE);
	}
	else {
	    start = 1;
//	    setInclMsgHnd(msg, FALSE);
	}

	if (_msgHandle) {
	    delete _msgHandle;
	}
	_msgHandle = msg;
	_my_editor->attachArea()->parseAttachments(error,
						   msg,
						   TRUE,
//						   FALSE,
						   start);
	// Need to call this after calling parseAttachments() so attachments
	// will be displayed in the attachment pane.
	_my_editor->manageAttachArea();

	// Need to update this compose window's internal message handle.
	//
	// GL - calling updateMsgHndAtt is no longer necessary because we
	// just assigning the newly created msg to _msgHandle.
//	updateMsgHndAtt();
    }

    loadHeaders(msg, DTM_TRUE);
}

// Given a RFC_822_Message formatted file, parse it and fill the Compose Window.
void
SendMsgDialog::parseNplace(const char * path)
{
    // 1. Get file content into buffer.
    int fd = SafeOpen(path, O_RDONLY);
    if (fd < 0) {
	return;
    }
    
    struct stat buf;
    if (SafeFStat(fd, &buf) < 0) {
	close(fd);
	return;
    }
    
    _dead_letter_buf = new char[buf.st_size];
    if (!_dead_letter_buf) {
	close(fd);
	return;
    }
    
    if (SafeRead(fd, _dead_letter_buf, 
		 (unsigned int) buf.st_size) != buf.st_size) {
	delete _dead_letter_buf;
	close(fd);
	return;
    }
    
    parseNplace(_dead_letter_buf, (int) buf.st_size);
}

void
SendMsgDialog::text( const char *text )
{
    _my_editor->textEditor()->set_contents( text, strlen(text) );
}

void
SendMsgDialog::append( const char *text )
{
    _my_editor->textEditor()->append_to_contents( text, strlen(text) );
}

char *
SendMsgDialog::text()
{
    // Potential memory leak here.  Because XmTextGetString returns 
    // pointer to space containing all the text in the widget.  Need 
    // to call XtFree after we use this space
    // Also DtEditor widget requires application to free data.
    
    return (_my_editor->textEditor()->get_contents());
    
}


void
SendMsgDialog::text_selected()
{
    // turn on sensitivity for Cut/Clear/Copy/Delete
    _edit_cut->activate();
    _edit_copy->activate();
    _edit_clear->activate();
    _edit_delete->activate();
    _edit_select_all->activate();
}

void
SendMsgDialog::text_unselected()
{
    // turn off sensitivity for those items
    _edit_cut->deactivate();
    _edit_copy->deactivate();
    _edit_clear->deactivate();
    _edit_delete->deactivate();
}

// Attachments

void
SendMsgDialog::attachment_selected()
{
    _att_save->activate();
    _att_delete->activate();
    _att_rename->activate();
    
}

void
SendMsgDialog::all_attachments_selected()
{
    _att_delete->activate();
    _att_save->deactivate();
    _att_rename->deactivate();

    if (_attachmentActionsList != NULL) {
	_menuBar->removeCommands(_attachmentMenu, _attachmentActionsList);
	_menuPopupAtt->removeCommands(_attachmentPopupMenu, 
				_attachmentActionsList);
	delete _attachmentActionsList;
	_attachmentActionsList = NULL;
    }

}


void
SendMsgDialog::all_attachments_deselected()
{
    _att_save->deactivate();
    _att_delete->deactivate();
    _att_rename->deactivate();
    
    if (_attachmentActionsList != NULL) {
	_menuBar->removeCommands(_attachmentMenu, _attachmentActionsList);
	_menuPopupAtt->removeCommands(_attachmentPopupMenu, 
				_attachmentActionsList);
	delete _attachmentActionsList;
	_attachmentActionsList = NULL;
    }
    
}


void
SendMsgDialog::addAttachmentActions(
				    char **actions,
				    int indx
				    )
{
    int i;
    char *anAction;
    AttachmentActionCmd *attachActionCmd;
    
    if (_attachmentActionsList == NULL) { 
	_attachmentActionsList = new CmdList("AttachmentActions", "AttachmentActions");
    }
    else {
	_menuBar->removeCommands(_attachmentMenu, _attachmentActionsList);
	_menuPopupAtt->removeCommands(_attachmentPopupMenu, 
					_attachmentActionsList);
	delete _attachmentActionsList;
	_attachmentActionsList = new CmdList("AttachmentActions", "AttachmentActions");
    }
    
    char *actionLabel;         
    for (i = 0; i < indx; i++) {
	anAction = actions[i];
	actionLabel = DtActionLabel(anAction);   // get the localized action label
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
					    _attachmentActionsList
					    );
}

void
SendMsgDialog::removeAttachmentActions()
{
    
    // Stubbed out for now
}

void
SendMsgDialog::invokeAttachmentAction(
				      int index
				      )
{
    DtMailEditor *editor = this->get_editor();
    AttachArea *attacharea = editor->attachArea();
    Attachment *attachment = attacharea->getSelectedAttachment();
    
    attachment->invokeAction(index);
}

void
SendMsgDialog::selectAllAttachments()
{
    
    DtMailEditor *editor = this->get_editor();
    AttachArea *attachArea = editor->attachArea();
    
    attachArea->selectAllAttachments();
    
}


void
SendMsgDialog::activate_default_attach_menu()
{
    _att_select_all->activate();
}

void
SendMsgDialog::deactivate_default_attach_menu()
{
    _att_select_all->deactivate();
}


void
SendMsgDialog::delete_selected_attachments()
{
    DtMailEnv mail_error;
    
    // Initialize the mail_error.

    mail_error.clear();
    
    AttachArea *attachArea = _my_editor->attachArea();
    attachArea->deleteSelectedAttachments(mail_error);
    
    if (mail_error.isSet()) {
	// do something
    }
    
    // Activate this button to permit the user to undelete.
    
    _att_undelete->activate();
    
    // Deactivate buttons that will be activated when another
    // selection applies.
    
    _att_save->deactivate();
    _att_delete->deactivate();
    _att_rename->deactivate();

    if (_attachmentActionsList) {
	_menuBar->removeCommands(_attachmentMenu, _attachmentActionsList);
	_menuPopupAtt->removeCommands(_attachmentPopupMenu, 
				      _attachmentActionsList);
	delete _attachmentActionsList;
	_attachmentActionsList = NULL;
    }
}

void
SendMsgDialog::undelete_last_deleted_attachment()
{
    DtMailEnv mail_error;
    
    // Initialize the mail_error.
    
    mail_error.clear();
    
    AttachArea *attachArea = _my_editor->attachArea();
    attachArea->undeleteLastDeletedAttachment(mail_error);
    
    if (mail_error.isSet()) {
	// do something
    }
    
    if(_my_editor->attachArea()->getIconSelectedCount()) 
    	_att_delete->activate();

    if (attachArea->getDeleteCount() == 0) {
	_att_undelete->deactivate();
    }
}

Boolean
SendMsgDialog::renameAttachmentOK()
{
    AttachArea *attachArea = _my_editor->attachArea();
    
    if (attachArea->getIconSelectedCount() > 1) {
       char *buf = new char[512];
	
       sprintf(buf, GETMSG(DT_catd, 5, 4, "Select only one attachment\n\
and then choose rename"));
	
	_genDialog->setToQuestionDialog(
					GETMSG(DT_catd, 5, 2, "Mailer"),
					buf);
	
	char * helpId = DTMAILHELPSELECTONEATTACH;
	
	int answer = _genDialog->post_and_return(helpId);
	
	delete [] buf;
	return(FALSE);
    }
    else {
	return(TRUE);
    }
}

void
SendMsgDialog::showAttachArea()
{
    DtMailEditor *editor = this->get_editor();
    editor->showAttachArea();
    ((ToggleButtonCmd *)_att_show_pane)->setButtonState(TRUE, FALSE);
}

void
SendMsgDialog::hideAttachArea()
{
    DtMailEditor *editor = this->get_editor();
    editor->hideAttachArea();
    ((ToggleButtonCmd *)_att_show_pane)->setButtonState(FALSE, FALSE);
}

int
SendMsgDialog::lookupHeader(const char * name)
{
    for (int h = 0; h < _header_list.length(); h++) {
	HeaderList * hl = _header_list[h];
	if (hl->show != SMD_NEVER && 
		strcmp(hl->label, name) == 0) {
	    return(h);
	}
    }

    return(-1);
}

void
SendMsgDialog::headerValueChanged(Widget,
				  XtPointer client_data,
				  XtPointer)
{
    SendMsgDialog * self = (SendMsgDialog *)client_data;
    self->_headers_changed = DTM_TRUE;
}

void
SendMsgDialog::reattachHeaders(void)
{
    // We have to walk through the entire list of headers, attaching
    // the shown headers to the ones above them.
    //
    HeaderList * hl = _header_list[0];
    Widget previous_form = hl->form_widget;

    for (int h = 1; h < _header_list.length(); h++) {
	hl = _header_list[h];

	switch(hl->show) {
	  case SMD_ALWAYS:
	    previous_form = hl->form_widget;
	    break;

	  case SMD_SHOWN:
	    XtVaSetValues(hl->form_widget,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, previous_form,
			  NULL);
	    previous_form = hl->form_widget;
	    break;

	  default:
	    break;
	}
    }

    forceFormResize(_main_form);
    forceFormResize(_header_form);
}

void
SendMsgDialog::justifyHeaders(void)
{
    // Find out which header label has the longest display width to right
    // justify all labels.
    //
    Dimension longest = 0;
    Dimension w = 0;
    Dimension margin;
    for (int count = 0;  count < _header_list.length(); count++) {
	HeaderList * hl = _header_list[count];
	if (hl->show == SMD_HIDDEN || hl->show == SMD_NEVER) {
	    continue;
	}

	XtVaGetValues(hl->label_widget,
		      XmNwidth, &w,
		      XmNmarginLeft, &margin,
		      NULL);
	w -= margin;
	if ( w > longest ) {
	    longest = w;
	}
    }

    for (int adjust = 0;  adjust < _header_list.length();  adjust++) {
	HeaderList * hl = _header_list[adjust];
	if (hl->show == SMD_HIDDEN || hl->show == SMD_NEVER) {
	    continue;
	}

	XtVaGetValues(hl->label_widget,
		      XmNwidth, &w,
		      XmNmarginLeft, &margin,
		      NULL);
	w -= margin;
	XtVaSetValues(hl->label_widget, XmNmarginLeft, (longest-w) > 0 ? longest-w : 1, NULL );
    }
}

void
SendMsgDialog::forceFormResize(Widget form)
{
    // The Motif Form widget is at least a little bit brain damaged.
    // We need to convince it to do the right thing after we make
    // minor adjustments in the children.
    //
    Dimension width, height, border;
    XtVaGetValues(form,
		  XmNwidth, &width,
		  XmNheight, &height,
		  XmNborderWidth, &border,
		  NULL);

    XtVaSetValues(form,
		  XmNwidth, width + 1,
		  XmNheight, height + 1,
		  NULL);

    XtVaSetValues(form,
		  XmNwidth, width,
		  XmNheight, height,
		  NULL);
}


Boolean
SendMsgDialog::unfilled_headers()
{
    // Walk through the headers. See if any of them have a value.
    //
    for (int scan = 0; scan < _header_list.length(); scan++) {
	HeaderList * hl = _header_list[scan];
	if (hl->show != SMD_ALWAYS && hl->show != SMD_SHOWN) {
	    continue;
	}

	char * value = NULL;
	XtVaGetValues(hl->field_widget,
		      XmNvalue, &value,
		      NULL);
	if (value) {
	  if (strlen(value) > 0) {
	      XtFree(value);
	      return(FALSE);
	  }
	  XtFree(value);
        }
    }
    return(TRUE);
}

// Method checks if self has text in it.  

Boolean
SendMsgDialog::checkDirty()
{
    if (this->unfilled_headers() && 
       (this->get_editor()->textEditor()->no_text()) && 
       (this->get_editor()->attachArea()->getIconCount() == 0) ) {

	// return FALSE so quit() can go ahead.

	return(FALSE);

    } 
    else {
	return(TRUE);
    }
}

Boolean
SendMsgDialog::handleQuitDialog()
{
    char *helpId;

    DtMail::Session *m_session = theRoamApp.session()->session();
    const char * value = NULL;
    DtMailEnv error;

    m_session->mailRc(error)->getValue(error, "expert", &value);
    if (error.isNotSet() && value != NULL)
    {
        if (NULL != value)
          free((void*) value);

	return (TRUE); 
    }

    DtMailGenDialog *dialog = this->genDialog();

    dialog->setToQuestionDialog(
			GETMSG(
				DT_catd, 
				1, 
				99, 
				"Mailer - Close"),
			GETMSG(
				DT_catd, 
				3, 
				58, 
				"The Compose window contains text or\n\
attachments that will be lost if\n\
the window is closed.\n\
Close the Compose window?")
		);
    helpId = DTMAILHELPCLOSECOMPOSEWINDOW;
    if ( dialog->post_and_return(
			GETMSG(
			       DT_catd, 
			       1, 
			       100, 
			       "OK"),
			GETMSG(
			       DT_catd, 
			       1, 
			       101, 
			       "Cancel"),
			helpId) == 1 ) {    // Close selected
	    
	return(TRUE);
    }
    else {
	return(FALSE);	// Cancel selected
    }
}

void
SendMsgDialog::goAway(
    Boolean checkForDirty
)
{
    
    if (!checkForDirty) {
	_takeDown = TRUE;
	this->quit();
    }
    else {
	// Check to see if self has contents (ie., is dirty)

	Boolean is_dirty = this->checkDirty();

	if (is_dirty) {
	    if (isIconified()) {
		MainWindow::manage();
	    }

	    // Enquire if user really wants this window to go away

	    Boolean really_quit = this->handleQuitDialog();
	    if (!really_quit) {
		return;
	    }
	}
	_takeDown = TRUE;
	this->quit();
    }
}

void
SendMsgDialog::manage()
{
    MenuWindow::manage();
    // Set focus 
    HeaderList * hl = _header_list[0];
    (void) XmProcessTraversal(hl->field_widget, XmTRAVERSE_CURRENT);
   
}

void
SendMsgDialog::unmanage()
{
    MenuWindow::unmanage();
    XFlush(XtDisplay(this->_main_form));
    XSync(XtDisplay(this->_main_form), False);
}
    

Compose::Compose()
{
    _compose_head = NULL;
    _not_in_use = 0;
    _num_created = 0;
    _timeout_id = 0;
}

Compose::~Compose()
{
    Compose::Compose_Win *a_node;
    Compose::Compose_Win *next_node;

    theRoamApp.registerPendingTask();

    a_node = _compose_head;
    while (a_node)
    {
	next_node = a_node->next;
	if (!a_node->in_use)
	{
	    delete a_node->win;
	    free((void*) a_node);
	}
	a_node = next_node;
    }

    theRoamApp.unregisterPendingTask();
}

void
Compose::putWin(SendMsgDialog *smd, Boolean in_use)
{
    Compose::Compose_Win* a_node = NULL; 
    Compose::Compose_Win *tmp = NULL;

    //
    // Update the _not_in_use count.
    //
    if (! in_use)
      _not_in_use++;

    //
    // Check to see if compose window is already in the list.
    //
    for (a_node = _compose_head; a_node; a_node=a_node->next)
    {
	if (a_node->win == smd)
	{
	    a_node->in_use = in_use;
	    return;
	}
    }

    // Need new node with smd.
    tmp = (Compose::Compose_Win *)malloc(sizeof(Compose::Compose_Win));
    tmp->win = smd;
    tmp->next = NULL;
    tmp->in_use = in_use;

    // If nothing is cached so far, add this Compose window to the head.
    if (NULL == _compose_head)
    {
	_compose_head = tmp;
	return;
    }
    
    // There exists a cache.  Add this compose window to the tail.
    for (a_node=_compose_head; a_node; a_node=a_node->next)
    {
	if (NULL == a_node->next)
	{
	    a_node->next = tmp;
	    return;
	}
    }
}

SendMsgDialog*
Compose::getUnusedWin()
{
    if (NULL == _compose_head) return NULL;
   
    Compose::Compose_Win* a_node = NULL; 
    Compose::Compose_Win* the_node = NULL; 
    
    // Find a node with unused smd.  Return smd
    for (a_node=_compose_head; a_node; a_node=a_node->next)
    {
	if (!a_node->in_use)
	{
	    a_node->in_use = TRUE;
	    _not_in_use--;
	    return a_node->win;
	}
    }

   return NULL;
}

// Get a compose window either by creating a new SendMsgDialog or
// from the recycle list.
SendMsgDialog *
Compose::getWin()
{
    SendMsgDialog *newsend = NULL;
    
#ifdef DTMAIL_TOOLTALK
    if (_timeout_id)
    {
	XtRemoveTimeOut(_timeout_id);
	_timeout_id = 0;
    }
#endif
    
    newsend = getUnusedWin();

    if (!newsend)
    {
	// We have no unused SMDs around; therefore, create new window.
	theRoamApp.busyAllWindows();
	newsend = new SendMsgDialog();
	newsend->initialize();
	_num_created++;
	putWin(newsend, TRUE);
	theRoamApp.unbusyAllWindows();
    }
    else
    {
	newsend->resetHeaders();
	newsend->displayInCurrentWorkspace();
    }

    newsend->text_unselected();
    newsend->manage();
    newsend->startAutoSave();
    
    // Get new Message Handle
    newsend->setMsgHnd();
    char *ttl = GETMSG(DT_catd, 1, 160, "New Message");
    newsend->setTitle(ttl);
    newsend->setIconTitle(ttl);
    return newsend;
}


void
SendMsgDialog::setTitle(char *subject)
{
    char *format = "%s - %s";
    char *prefix = GETMSG(DT_catd, 1, 6, "Mailer");
    char *new_title;
    int   len;

    len = strlen(format) + strlen(prefix) + strlen(subject) + 1;
    new_title = new char[len];
    sprintf(new_title, format, prefix, subject);

    title(new_title);
    delete [] new_title; 
}

void
SendMsgDialog::resetHeaders(void)
{
    DtMail::Session *m_session = theRoamApp.session()->session();
    const char * value = NULL;
    DtMailEnv error;
    int i, j;

    m_session->mailRc(error)->getValue(error, "additionalfields", &value);

    // Return if no props were applied and headers did not change.
    if ((_additionalfields == NULL && value == NULL) ||
	( _additionalfields != NULL && value != NULL &&
	  strcmp (_additionalfields, value) == 0))
    {
        if (NULL != value)
          free((void*) value);

	return;
    }

    // User changed the Header list via props. Recreate list
    // and menus...

    // First hide all shown headers 
    for (i=0; i < _header_list.length(); i++) {
    	HeaderList * hl = _header_list[i];
    	if (hl->show == SMD_SHOWN) {
		hl->show = SMD_HIDDEN;
		XtUnmanageChild(hl->form_widget);
    	}
    }

    // Now remove the old list.
    DtVirtArray<PropStringPair *> results(8);
    parsePropString(_additionalfields, results);
    for (i=0, j=results.length(); i < j; i++) {
	PropStringPair * psp = results[i];
    	int slot = lookupHeader(psp->label);
	// dont allow removal of default headers.
	HeaderList * hl = _header_list[slot];
	if (!reservedHeader(hl->label)) {
    		if (slot != -1) 
			hl->show = SMD_NEVER;
	}
	else if (hl->value != NULL) {
		free(hl->value);
		hl->value = NULL;
	}
    }
    while(results.length()) {
	PropStringPair * psp = results[0];
    	delete psp;
    	results.remove(0);
    }

    if (_additionalfields != NULL) 
	free(_additionalfields);
    if (value != NULL && *value != '\0')
	_additionalfields = strdup(value);
    else
	_additionalfields = NULL;

    parsePropString(value, results);

    // New List...
    for (j=results.length(), i=0; i < j; i++) {
    	PropStringPair * psp = results[i];
    	int slot = lookupHeader(psp->label);
    	if (slot != -1) {
		// Already in list
		HeaderList * hl = _header_list[slot];
		if (!reservedHeader(hl->label)) 
                	hl->show = SMD_HIDDEN;
		if (hl->value != NULL) {
			free(hl->value);
			hl->value = NULL;
		}
		if (psp->value != NULL)
			hl->value = strdup(psp->value);
		continue;
    	}	
    	HeaderList * copy_hl = new HeaderList;
    	copy_hl->label = strdup(psp->label);
    	copy_hl->header = strdup(psp->label);
    	if (psp->value)
		copy_hl->value = strdup(psp->value);
	copy_hl->show = SMD_HIDDEN;
	_header_list.append(copy_hl);
    }
    while(results.length()) {
    	PropStringPair * psp = results[0];
    	delete psp;
    	results.remove(0);
    }

    createHeaders(_header_form);
    doDynamicHeaderMenus();

    if (NULL != value)
      free((void*) value);
}

void
SendMsgDialog::setInputFocus(const int mode)
{
    if (mode == 0) {
        // Set focus
        HeaderList * hl = _header_list[0];
        (void) XmProcessTraversal(hl->field_widget, XmTRAVERSE_CURRENT);
    }
    else if (mode == 1) {
        Widget edWid = _my_editor->textEditor()->get_editor();
        (void) XmProcessTraversal(edWid, XmTRAVERSE_CURRENT);
    }
}
 
void
SendMsgDialog::attachmentFeedback(
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

Boolean
SendMsgDialog::hasAddressee()
{
  DtMailEnv error;
  
  DtMail::Envelope * env = _msgHandle->getEnvelope(error);
  
  // Walk through the headers. 
  // Return TRUE if the message has a value for either of the
  // following headers: To:, Cc:, or Bcc:.
  // Return FALSE if none of the three headers have any value.
  
  for (int scan = 0; scan < _header_list.length(); scan++) {
    HeaderList * hl = _header_list[scan];
    if (hl->show != SMD_ALWAYS && hl->show != SMD_SHOWN) {
      continue;
    }
    if ((strcmp(hl->label, "To") == 0) ||
	(strcmp(hl->label, "Cc") == 0) || 
	(strcmp(hl->label, "Bcc") == 0)) {
      char * value = NULL;
      XtVaGetValues(hl->field_widget,
		    XmNvalue, &value,
		    NULL);
      if (value) {
	for (char *cv = value; *cv; cv++) {
	  if (!isspace(*cv)) {
	    XtFree(value);
	    return(TRUE);	// text value contains contents
	  }
	}
	XtFree(value);		// text value is "content free" - try the next one
      }
    }
  }
  return(FALSE);		// no field has contents 
}
