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
 *	$TOG: ComposeCmds.C /main/11 1998/10/21 17:23:13 mgreess $
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

#include <EUSCompat.h>
#include <unistd.h>
#include <sys/types.h>

#if defined(NEED_MMAP_WRAPPER)
extern "C" {
#endif
#include <sys/mman.h>
#if defined(NEED_MMAP_WRAPPER)
}
#endif

#include <pwd.h>
#include <Xm/Text.h>
#include <Xm/FileSBP.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/PushBG.h>
#include <Xm/PanedW.h>
#include <Xm/Form.h>
#include <Dt/Dts.h>
#include <Dt/Action.h>
#include <DtMail/IO.hh>
#include "RoamMenuWindow.h"
#include "SendMsgDialog.h"
#include "Undelete.hh"
#include "RoamCmds.h"
#include "ComposeCmds.hh"
#include "Application.h"
#include "RoamApp.h"
#include "DtMailWDM.hh"
#include "FindDialog.h"
#include "MsgScrollingList.hh"
#include "MsgHndArray.hh"
#include "MemUtils.hh"
#include "MailMsg.h"
#include "EUSDebug.hh"
#include "DtMailGenDialog.hh"
#include "DtMailHelp.hh"
#include <DtMail/DtMailError.hh>
#include "Help.hh"
#include <Dt/Help.h>
#include "Attachment.h"
#include "str_utils.h"

ComposeFamily::ComposeFamily(char *name,
			     char *label,
			     int active,
			     RoamMenuWindow *window)
: RoamCmd(name, label, active, window)
{
    _parent = window;
}

#ifndef CAN_INLINE_VIRTUALS
ComposeFamily::~ComposeFamily( void )
{
}
#endif /* ! CAN_INLINE_VIRTUALS */

// Append a formatted message to Compose's Text area.
// This routine is essentially the same as MsgScollingList::display_message()
// except for two major differences:
// 1. No RoamMenuWindow reference (so that Compose can be standalone).
// 2. Indent string can be used for "include" and "forward".
void
ComposeFamily::Display_entire_msg(DtMailMessageHandle msgno,
				  SendMsgDialog *compose, 
				  char *format 
				  )
{
    DtMailEnv error;
    
    int num_bodyParts;
    DtMail::MailBox *mbox = _menuwindow->mailbox();
    DtMail::Message *msg = mbox->getMessage(error, msgno);
    DtMail::Envelope *env = msg->getEnvelope(error);
    DtMail::BodyPart *tmpBP = NULL;
    DtMailBuffer tmpBuffer;
    void *buffer = NULL;
    unsigned long size = 0;
    
    Editor::InsertFormat ins_format = Editor::IF_NONE;
    Editor::BracketFormat brackets = Editor::BF_NONE;
    
    // Do not need to wrap "include", "forward", and "indent" with
    // catgets().
    if ( strcmp(format, "include") == 0 ) {
	ins_format = Editor::IF_BRACKETED;
	brackets = Editor::BF_INCLUDE;
    } else if ( strcmp(format, "forward") == 0 ) {
	ins_format = Editor::IF_BRACKETED;
	brackets = Editor::BF_FORWARD;
    } else if ( strcmp(format, "indent") == 0 ) {
	ins_format = Editor::IF_INDENTED;
    }
    
    // Get the editor to display the body of message with the appropriate
    // insert/bracket formatting.
    // We only include the first body part of the message. Attachments, 
    // etc. are "FORWARD"-ed but not "INCLUDE"-ed
    
    char * status_string;
    DtMailBoolean firstBPHandled =
	compose->get_editor()->textEditor()->set_message(
					 msg, 
					 &status_string, 
					 Editor::HF_ABBREV,
					 ins_format, 
					 brackets);

    // Now need to handle the unhandled body parts of the message.
    
    num_bodyParts = msg->getBodyCount(error);
    if (error.isSet()) {
	// do something
    }
    
    if (strcmp(format, "forward") == 0) {
	// If the message has attachments, then let the attach pane
	// handle attachments but not the first bodyPart (which has
	// already been handled here).
	
	if ((num_bodyParts > 1) || (!firstBPHandled)) {

	    tmpBP = msg->getFirstBodyPart(error);
	    if (firstBPHandled) {
		//  The first bodyPart has already been handled.
		// The others, beginning from the second, need to be parsed 
		// and put into the attachPane.

		compose->setInclMsgHnd(msg, TRUE);
		tmpBP = msg->getNextBodyPart(error, tmpBP);

	    } else {
		// The first bodyPart was not handled.
		// It may not have been of type text.
		// The attachment pane needs to handle all the bodyParts
		// beginning with the first.
	    
		compose->setInclMsgHnd(msg, FALSE);
	    }
	    
	    char *name;
	    while (tmpBP != NULL) {
		tmpBP->getContents(
				    error, (const void **) &tmpBuffer.buffer,
				    &tmpBuffer.size,
				    NULL,
				    &name,
				    NULL,
				    NULL);
		if (error.isSet()) {
		    // Do something
		}
		// It's possible for an attachment to not have a name.
		if (!name) {
		    name = "NoName";
		}

		compose->add_att(name, tmpBuffer);
		tmpBP = msg->getNextBodyPart(error, tmpBP);
		if (error.isSet()) {
		    // do something
		}

		if (strcmp(name, "NoName") != 0) {
		    free(name);
		}
	    }
	    if (error.isSet()) {
		
		// do something
	    }
	
	    // Need to call this after calling parseAttachments().

	    compose->get_editor()->manageAttachArea();
	
	    // This message has attachment and is being included/forwarded,
	    // so need to fill the Compose Message Handle with attachment 
	    // BodyParts.
	    // See function for further details.

	    // compose->updateMsgHndAtt();
	}
    } 
    else
    {
	// If the message has attachments, then let the attach pane
	// handle attachments but not the first bodyPart (which has
	// already been handled here).
	
	if ((num_bodyParts > 1) || (!firstBPHandled))
	{
	    char	*att;
	    Editor	*editor = compose->get_editor()->textEditor();
	    
	    att = GETMSG(
			DT_catd, 1, 255,
			"------------------ Attachments ------------------\n");

	    tmpBP = msg->getFirstBodyPart(error);
	    if (firstBPHandled)
	      tmpBP = msg->getNextBodyPart(error, tmpBP);
	    
	    editor->append_to_contents(att, strlen(att));
	    while (tmpBP != NULL)
	    {
	        editor->set_attachment(tmpBP, ins_format, brackets);
		tmpBP = msg->getNextBodyPart(error, tmpBP);
		if (error.isSet()) {
		    // do something
		}
	    }
	}
    } 
    
    // Leave it up to check point routine for update or do it now???
    compose->updateMsgHnd();
}

void
ComposeFamily::appendSignature(SendMsgDialog * compose)
{
    DtMailEnv error;
    DtMail::Session * d_session = theRoamApp.session()->session();
    DtMail::MailRc * mail_rc = d_session->mailRc(error);

    const char * value = NULL;
    mail_rc->getValue(error, "signature", &value);
    if (error.isSet()) {
	return;
    }

    char * fullpath = d_session->expandPath(error, value);
    compose->get_editor()->textEditor()->append_to_contents(fullpath);
    if (NULL != fullpath)
      free((void*) fullpath);
    if (NULL != value)
      free((void*) value);

    compose->get_editor()->textEditor()->set_to_top();
}

char *
ComposeFamily::valueToAddrString(DtMailValueSeq & value)
{
    int max_len = 0;
    int count;

    for (count = 0; count < value.length(); count++) {
	max_len += strlen(*(value[count]));
    }

    char * str = new char[max_len + count + 1];
    str[0] = 0;

    DtMailBoolean need_comma = DTM_FALSE;

    for (int cat = 0; cat < value.length(); cat++)
    {
	DtMailValue * val = value[cat];
	DtMailAddressSeq *addr_seq = val->toAddress();

	for (int ad = 0; ad < addr_seq->length(); ad++)
	{
	    DtMailValueAddress * addr = (*addr_seq)[ad];

	    // Deal with mail address parser shortcomings
	    if ( strcmp(addr->dtm_address, ",") == 0 )
		continue ;

	    if (need_comma) {
		strcat(str, ", ");
	    }

	    need_comma = DTM_TRUE;

	    strcat(str, addr->dtm_address);
	}

        delete addr_seq;
    }

    return(str);
}


// Container menu "Compose==>New Message"
ComposeCmd::ComposeCmd( 
			char *name,
			char *label,
			int active,
			RoamMenuWindow *window
			) : ComposeFamily( name, label, active, window )
{
}

// Put up a blank compose window.
void
ComposeCmd::doit()
{
    SendMsgDialog * newsend = theCompose.getWin();
    if (newsend == NULL) {
	DtMailGenDialog * dialog = _parent->genDialog();
	
	dialog->setToErrorDialog(GETMSG(DT_catd, 1, 203, "Mailer"),
				 GETMSG(DT_catd, 1, 204, "Unable to create a compose window."));
	char * helpId = DTMAILHELPNOCOMPOSE;
	int answer = dialog->post_and_return(helpId);
    }

    appendSignature(newsend);
}

// Container menu "Compose==>New, Include All" and "Compose==>Forward Message"
// The last parameter is a switch for "include" or "forward" format.
ForwardCmd::ForwardCmd( 
			char *name,
			char *label,
			int active,
			RoamMenuWindow *window, 
			int forward
			) : ComposeFamily(name, label, active, window)
{
    _forward = forward;
}

// Forward or Include selected messages.
// For Include message(s), all Compose window header fields are left blank.
// For Forward message(s), the Compose window "Subject" header field is filled
// with the subject of the last selected message.
void
ForwardCmd::doit()
{
    FORCE_SEGV_DECL(MsgHndArray, msgList);
    FORCE_SEGV_DECL(MsgStruct, tmpMS);
    DtMailMessageHandle msgno;
    
    // Get a Compose window.
    SendMsgDialog *newsend = theCompose.getWin();
    if ( newsend == NULL ) {
	DtMailGenDialog * dialog = _parent->genDialog();
	
	dialog->setToErrorDialog(GETMSG(DT_catd, 1, 205, "Mailer"),
				 GETMSG(DT_catd, 1, 206, "Unable to create a compose window."));
	char * helpId = DTMAILHELPNOCOMPOSE;
	int answer = dialog->post_and_return(helpId);
    }

    // Put the signature above the message.
    //
    appendSignature(newsend);

    // For Forwarding subject
    DtMail::MailBox * mbox = _menuwindow->mailbox();
    DtMail::Message * msg;
    DtMail::Envelope * env;
    DtMailValueSeq	value;
    DtMailEnv error;
    
    // For each selected message, put it in the Compose window.
    if ( msgList = _menuwindow->list()->selected() ) {
	for ( int k = 0;  k < msgList->length();  k++ ) {
	    tmpMS = msgList->at(k);
	    msgno = tmpMS->message_handle;
	    if ( _forward ) {
		msg = mbox->getMessage(error, msgno);
		env = msg->getEnvelope(error);
		value.clear();
		env->getHeader(error, DtMailMessageSubject, DTM_TRUE, value);
		if (!error.isSet()) {
		    const char *subject = *(value[0]);
		    newsend->setHeader("Subject", subject);
		    newsend->setTitle((char*) subject);
		    newsend->setIconTitle((char*) subject);
		}
		Display_entire_msg(msgno, newsend, "forward");
	    } else {
		Display_entire_msg(msgno, newsend, "indent");
	    }
	}
    }
    newsend->get_editor()->textEditor()->set_to_top();
}

// Container menu "Compose==>Reply to Semder" and
// "Compose==>Reply to Sender, Include"
// The last parameter is a switch for including the selected message or not.
ReplyCmd::ReplyCmd ( 
		     char *name, 
		     char *label,
		     int active, 
		     RoamMenuWindow *window, 
		     int include 
		     ) : ComposeFamily ( name, label, active, window )
{
    _include = include;
}

// For each message selected, reply to sender.
void
ReplyCmd::doit()
{
    FORCE_SEGV_DECL(MsgHndArray, msgList);
    FORCE_SEGV_DECL(MsgStruct, tmpMS);
    DtMailMessageHandle msgno;
    FORCE_SEGV_DECL(char, from);
    FORCE_SEGV_DECL(char, subject);
    FORCE_SEGV_DECL(char, cc);
    DtMailEnv error;
    DtMail::MailBox * mbox = _menuwindow->mailbox();
    
    // Initialize the error.
    error.clear();
    
    if (msgList = _menuwindow->list()->selected())
    {
	for ( int i=0; i < msgList->length(); i++ ) {
	    tmpMS = msgList->at(i);
	    msgno = tmpMS->message_handle;
	    SendMsgDialog *newsend = theCompose.getWin();
	    if ( newsend == NULL ) {
		DtMailGenDialog * dialog = _parent->genDialog();
		
		dialog->setToErrorDialog(GETMSG(DT_catd, 1, 207, "Mailer"),
					 GETMSG(DT_catd, 1, 208, "Unable to create a compose window."));
		char * helpId = DTMAILHELPNOCOMPOSE;
		int answer = dialog->post_and_return(helpId);
	    }
	    XmUpdateDisplay( newsend->baseWidget() );
	    
	    DtMail::Message * msg = mbox->getMessage(error, msgno);
	    DtMail::Envelope * env = msg->getEnvelope(error);
	    
	    DtMailValueSeq	value;
	    
	    env->getHeader(error, DtMailMessageSender, DTM_TRUE, value);
	    if (error.isSet()) {
		newsend->setHeader("To", "nobody@nowhere");
	    }
	    else {
		char * addr_str = valueToAddrString(value);
		newsend->setHeader("To", addr_str);
		delete [] addr_str;
	    }
	    
	    value.clear();
	    env->getHeader(error, DtMailMessageSubject, DTM_TRUE, value);
	    if (error.isSet()) {
		subject = new char[200];
		strcpy(subject, "Re: ");
		DtMailValueSeq sent;
		env->getHeader(error,
			       DtMailMessageSentTime,
			       DTM_TRUE,
			       sent);
		if (error.isSet()) {
		    strcat(subject, "Your Message");
		}
		else {
		    strcat(subject, "Your Message Sent on ");
		    strcat(subject, *(sent[0]));
		}
		newsend->setHeader("Subject", subject);
	    }
	    else {
		// Get the BE store of header.  It may contain newlines or 
		// tab chars which can munge the scrolling list's display!

		const char * orig = *(value[0]);

		int fc;
		int orig_length;
		char *tmp_subj;

		// Check if BE store contains the funky chars.

		for (fc = 0, orig_length = strlen(orig), 
		      tmp_subj = (char *) orig; 
		    fc < orig_length; 
		    fc++, tmp_subj++) {

		    char c = *tmp_subj;
		    if ((c == '\n') 
		     || (c == '\t') 
		     || (c == '\r')) {

			break;
		    }
		}

		subject = new char[fc+6];
		
		if (strncasecmp(orig, "Re:", 3)) {
		    strcpy(subject, "Re: ");
		}
		else {
		    *subject = 0;
		}
		
		strncat((char *)subject, orig, fc);

		newsend->setHeader("Subject", subject);
	    }
	    
            newsend->setTitle(subject);
            newsend->setIconTitle(subject);
	    delete [] subject;

	    if ( _include ) {
		Display_entire_msg(msgno, newsend, "indent");
		newsend->get_editor()->textEditor()->set_to_top();
	    }
	    appendSignature(newsend);
		newsend->setInputFocus(1);
	}

	delete msgList;
    }
}

// Container menu "Compose==>Reply to All" and "Compose==>Reply to All, Include"
// The last parameter is a switch for including the selected message or not.
ReplyAllCmd::ReplyAllCmd( 
			  char *name,
			  char *label,
			  int active,
			  RoamMenuWindow *window, 
			  int include 
			  ) : ComposeFamily( name, label, active, window )
{
    _include = include;
}

// For each message selected, reply to everybody.
void
ReplyAllCmd::doit()
{
    FORCE_SEGV_DECL(MsgHndArray, msgList);
    FORCE_SEGV_DECL(MsgStruct, tmpMS);
    FORCE_SEGV_DECL(char, subject);
    FORCE_SEGV_DECL(char, to);
    FORCE_SEGV_DECL(char, buffer);
    DtMailMessageHandle msgno;
    DtMail::MailBox *mbox = _menuwindow->mailbox();
    DtMailEnv error;
    char *currentCcValue;
    SendMsgDialog *newsend;
    DtMailGenDialog * dialog;
    DtMail::Message *msg;
    DtMail::Envelope *env;


    // Initialize the mail_error.
    error.clear();
    
    
    if ( msgList = _menuwindow->list()->selected() )
	for ( int k = 0;  k < msgList->length();  k++ ) {
	    DtMailValueSeq	value ;
            
	    tmpMS = msgList->at(k);
	    msgno = tmpMS->message_handle;
	    newsend = theCompose.getWin();
	    if ( newsend == NULL ) {
		dialog = _parent->genDialog();
		
		dialog->setToErrorDialog(GETMSG(DT_catd, 1, 209, "Mailer"),
					 GETMSG(DT_catd, 1, 210, "Unable to create a compose window."));
		char * helpId = DTMAILHELPNOCOMPOSE;
		int answer = dialog->post_and_return(helpId);
	    }
	    msg = mbox->getMessage(error, msgno);
	    env = msg->getEnvelope(error);
	    
	    env->getHeader(
			   error, 
			   DtMailMessageToReply,
			   DTM_TRUE, 
			   value);
	    
	    env->getHeader(
			   error, 
			   DtMailMessageSender, 
			   DTM_TRUE, 
			   value);

	    char * addr_str = valueToAddrString(value);
	    newsend->setHeader("To", addr_str);
	    delete [] addr_str;
	    value.clear();
	    env->getHeader(
			   error, 
			   DtMailMessageSubject, 
			   DTM_TRUE, 
			   value);
	    if ( error.isSet() ) {
		subject = new char[200];
		strcpy(subject, "Re: ");
		DtMailValueSeq sent;
		env->getHeader(error,
			       DtMailMessageSentTime,
			       DTM_TRUE,
			       sent);
		if (error.isSet()) {
		    strcat(subject, "Your Message");
		}
		else {
		    strcat(subject, "Your Message Sent on ");
		    strcat(subject, *(sent[0]));
		}
		newsend->setHeader("Subject", subject);
	    } else {
		// Get the BE store of header.  It may contain newlines or 
		// tab chars which can munge the scrolling list's display!

		const char * orig = *(value[0]);


		int fc = 0;
		int orig_length;
		char *tmp_subj;

		// Check if BE store contains the funky chars.

		for (fc = 0, orig_length = strlen(orig), 
		      tmp_subj = (char *)orig; 
		     fc < orig_length; 
		     fc++, tmp_subj++) {

		    char c = *tmp_subj;
		    if ((c == '\n') 
		     || (c == '\t') 
		     || (c == '\r')) {

			break;
		    }
		}

		subject = new char[fc+6];
		
		if (strncasecmp(orig, "Re:", 3)) {
		    strcpy(subject, "Re: ");
		}
		else {
		    *subject = 0;
		}
		
		strncat((char *)subject, orig, fc);

		newsend->setHeader("Subject", subject);
	    }
	    value.clear();
	    env->getHeader(
			   error, 
			   DtMailMessageCcReply, 
			   DTM_TRUE, 
			   value);
	    if (!error.isSet()) {
		// Strip out newlines from the cc line.  They *may* be 
		// present.
		currentCcValue = valueToAddrString(value);

		newsend->setHeader("Cc", currentCcValue);
		delete [] currentCcValue ;
	    }

            newsend->setTitle(subject);
            newsend->setIconTitle(subject);
	    delete [] subject;

	    if ( _include ) {
		Display_entire_msg(msgno, newsend, "indent");
		newsend->get_editor()->textEditor()->set_to_top();
	    }
	    appendSignature(newsend);
		newsend->setInputFocus(1);
	}
}

TemplateCmd::TemplateCmd(char *name,
			 char *label,
			 int active,
			 SendMsgDialog * compose,
			 const char * file)
: NoUndoCmd(name, label, active)
{
    _compose = compose;

    if (*file != '/' && *file != '~') {
        // Relative path.  Should be relative to home directory
        _file = (char *)malloc(strlen(file) + 4);
        if (_file != NULL) {
                strcpy(_file, "~/");
                strcat(_file, file);
        }
    } else {
        _file = strdup(file);
    }
}

TemplateCmd::~TemplateCmd(void)
{
    free(_file);
}

void
TemplateCmd::doit()
{
    DtMailEnv error;
    DtMail::Session * d_session = theRoamApp.session()->session();
    DtMailGenDialog * dialog = _compose->genDialog();
    DtMailBuffer mbuf;

    char * fullpath = d_session->expandPath(error, _file);

    // Map the file and try to parse it as a message. If it is a message,
    // then load it with headers. Otherwise, throw everything into the
    // editor.
    //
    int fd = SafeOpen(fullpath, O_RDONLY);
    if (fd < 0) {
	dialog->setToErrorDialog(GETMSG(DT_catd, 1, 211, "Mailer"),
				 GETMSG(DT_catd, 1, 212, "The template does not exist."));
	char * helpId = DTMAILHELPNOTEMPLATE;
	int answer = dialog->post_and_return(helpId);
	free(fullpath);
	return;
    }

    struct stat buf;
    if (SafeFStat(fd, &buf) < 0) {
	dialog->setToErrorDialog(GETMSG(DT_catd, 1, 213, "Mailer"),
				 GETMSG(DT_catd, 1, 214, "The template appears to be corrupt."));
	char * helpId = DTMAILHELPCORRUPTTEMPLATE;
	int answer = dialog->post_and_return(helpId);
	SafeClose(fd);
	free(fullpath);
	return;
    }

    int page_size = (int)sysconf(_SC_PAGESIZE);
    size_t map_size = (size_t) (buf.st_size + 
				(page_size - (buf.st_size % page_size)));

    int free_buf = 0;
    mbuf.size = buf.st_size;
#ifdef __osf__
    // This version of mmap does NOT allow requested length to be
    // greater than the file size ...  in contradiction to the
    // documentation (don't round up).
    mbuf.buffer = mmap(0, buf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
#else
    mbuf.buffer = mmap(0, map_size, PROT_READ, MAP_PRIVATE, fd, 0);
#endif
    if (mbuf.buffer == (char *)-1) {
	free_buf = 1;
	mbuf.buffer = new char[mbuf.size];
	if (mbuf.buffer == NULL) {
	    dialog->setToErrorDialog(GETMSG(DT_catd, 1, 215, "Mailer"),
				     GETMSG(DT_catd, 1, 216, "There is not enough memory to load the template."));
	    char * helpId = DTMAILHELPNOMEMTEMPLATE;
	    int answer = dialog->post_and_return(helpId);
	    SafeClose(fd);
	    free(fullpath);
	    return;
	}

	if (SafeRead(fd, mbuf.buffer, (unsigned int)mbuf.size) < mbuf.size) {
	    dialog->setToErrorDialog(GETMSG(DT_catd, 1, 217, "Mailer"),
				     GETMSG(DT_catd, 1, 218, "The template appears to be corrupt."));
	    char * helpId = DTMAILHELPERROR;
	    int answer = dialog->post_and_return(helpId);
	    SafeClose(fd);
	    delete [] mbuf.buffer;
	    free(fullpath);
	    return;
	}
    }

    // Now we ask the library to parse it. If this fails for any reason, this
    // is not a message, so we give up.
    //
    DtMail::Message * msg = d_session->messageConstruct(error,
							DtMailBufferObject,
							&mbuf,
							NULL,
							NULL,
							NULL);
    if (error.isSet()) {
	_compose->get_editor()->textEditor()->append_to_contents((char *)mbuf.buffer,
							       mbuf.size);
    }
    else {
	_compose->loadHeaders(msg, DTM_TRUE);

	DtMail::BodyPart * bp = msg->getFirstBodyPart(error);
	if (error.isNotSet()) {
	    const void * contents;
	    unsigned long length;

	    bp->getContents(error,
			    &contents,
			    &length,
			    NULL,
			    NULL,
			    NULL,
			    NULL);

	    _compose->get_editor()->textEditor()->append_to_contents(
						(char *)contents, length);
	}
    }

    free(fullpath);

    if (free_buf) {
	free(mbuf.buffer);
    }
    else {
	munmap((char *)mbuf.buffer, map_size);
    }

    SafeClose(fd);
}

HideShowCmd::HideShowCmd(char *name,
			 char *widgetlabel,
			 int active,
			 SendMsgDialog * compose,
			 const char * label)
: NoUndoCmd(name, (char *)widgetlabel, active)
{
    _compose = compose;
    _header = strdup(label);
}

HideShowCmd::~HideShowCmd(void)
{
    if (_header) {
	free(_header);
    }
}

void
HideShowCmd::doit(void)
{
    _compose->changeHeaderState(_header);
}
