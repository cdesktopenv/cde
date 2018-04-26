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
 *	$TOG: Editor.C /main/11 1998/07/24 16:06:00 mgreess $
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

#include <Xm/RowColumn.h>
#include <EUSCompat.h>
#include <assert.h>
#include <stdio.h>

#include <Dt/Dts.h>

#include "RoamApp.h"
#include "MailMsg.h"
#include "Editor.hh"
#include "str_utils.h"

Editor::Editor()
       : UIComponent("Editor")
{
}

Editor::~Editor() 
{
}


AbstractEditorParent::AbstractEditorParent() {
	_attachmentPopupMenu = NULL;
	_textPopupMenu = NULL;
	_menuPopupAtt = NULL;
	_menuPopupText = NULL;
}

AbstractEditorParent::~AbstractEditorParent() 
{
	delete _menuPopupAtt;
	delete _menuPopupText;
}   

DtMailBoolean
Editor::set_message(DtMail::Message * msg,
		    char ** status_string,
		    HeaderFormat header_format,
		    InsertFormat format,
		    BracketFormat brackets)
{
    DtMailEnv error;
    DtMail::Session *m_session = theRoamApp.session()->session(); 
    DtMail::MailRc * mail_rc = m_session->mailRc(error);

    *status_string = NULL;

    DtMail::Envelope *env = msg->getEnvelope(error);

    DtMailHeaderHandle hdr_hnd;
    char *name;
    DtMailValueSeq value;

    // Here is not the place for getting the indent string 
    // but it'll do for now.
    const char *indent_str = NULL;
    mail_rc->getValue(error, "indentprefix", &indent_str);
    if (error.isSet()) 
      indent_str = strdup("> ");

    disable_redisplay();

    int indent = 0;

    if (format == IF_BRACKETED) {
	char * ins_bracket;
	switch (brackets) {
	  case BF_FORWARD:
	    ins_bracket = GETMSG(DT_catd, 1, 195, "------------- Begin Forwarded Message -------------\n\n");
	    break;
	    
	  case BF_INCLUDE:
	  default:
	    ins_bracket = GETMSG(DT_catd, 1, 196, "------------- Begin Included Message -------------\n\n");
	    break;
	}

	append_to_contents(ins_bracket, strlen(ins_bracket));
    }

    // Code from MsgScrollingList - display_message().
    // We're trying to reduce heap size by not allocating and 
    // deleting space in every loop iteration.  So just have a 
    // fixed size buffer initially.
    // 

    // Initial line size.  When not enough, allocate more.
    int line_size = 1024;   
    int tmp_count = 0;
    char *line = new char[line_size];
    int hdr_num = 0;

    if (header_format != HF_NONE) {
	for ( hdr_hnd = env->getFirstHeader(
					    error, 
					    &name, 
					    value);
	      hdr_hnd && !error.isSet();
	      hdr_hnd = env->getNextHeader(
					   error, 
					   hdr_hnd, 
					   &name, 
					   value), hdr_num++ ) {
	    
	    
	    if ((header_format == HF_ABBREV)
			&& (hdr_num != 0 || strcmp(name, "From") != 0)) {
		DtMailEnv ierror;
		if (mail_rc->ignore(ierror, name)) {
		    free(name);
		    value.clear();
		    continue;
		}
	    }
	    
	    for ( int val = 0;  val < value.length();  val++ ) {
		tmp_count = strlen(name) + 
		    strlen(*(value[val])) +
		    strlen(indent_str) + 
		    5;
		if ( tmp_count > line_size ) { // Need to increase line size.
		    delete [] line;
		    line_size = tmp_count;
		    line = new char[line_size];
		}
		memset(line, 0, line_size);
		if (format == IF_INDENTED) {
		    strcpy(line, indent_str);
		    strcat(line, name);
		} else {
		    strcpy(line, name);
		}
		
		if (hdr_num != 0 || strcmp(name, "From") != 0) {
		    strcat(line, ": ");
		}
		else {
		    strcat(line, " ");
		}
		
		strcat(line, *(value[val]));
		strcat(line, "\n");
		append_to_contents(line, strlen(line));
	    }
	    value.clear();
	    free(name);
	}
    }
    // Don't delete line yet, because it's used below.
   
    if (format == IF_INDENTED) {
	append_to_contents(indent_str, strlen(indent_str));
    }

    if (header_format != HF_NONE) {
	append_to_contents("\n", 1);
    }

    DtMail::BodyPart *bp = msg->getFirstBodyPart(error);
   
    char *type;
    DtMailBoolean firstBPHandled = DTM_FALSE;

    bp->getContents(error, NULL, NULL, &type, NULL, NULL, NULL);
    if ( type ) {
	char *attr = DtDtsDataTypeToAttributeValue(
					type, 
					DtDTS_DA_IS_TEXT, 
					NULL);
	if ((attr && strcasecmp(attr, "true") == 0)
	    || strcoll(type, DtDTS_DT_UNKNOWN) == 0 ) {
	    const void *contents;
	    unsigned long size;
	    bp->lockContents(error, DtMailLockRead);
	    bp->getContents(error, &contents, &size, NULL, NULL, NULL, NULL);

	    if (format == IF_INDENTED) {
		int byte_count = 0;
		int content_count = (int) size;
		const char *last = NULL, *content_ptr = NULL;
		// Parse the result of getContents().
		// Spit out indent string with each line.
		// Is contents NULL terminated???
		for ( last = (const char *)contents, 
		    content_ptr = (const char *)contents;
		    content_count > 0;
		    content_ptr++, 
		    byte_count++, 
		    content_count-- ) {
		    if ((*content_ptr == '\n') || 
			(content_count == 1) ) {
			// 2 for null terminator and new line.
			tmp_count = strlen(indent_str) + byte_count + 2;
			if ( tmp_count > line_size ) { 
			   // Need to increase line size.
			   delete [] line;
			   line_size = tmp_count;
			   line = new char[line_size];
			}
			memset(line, 0, line_size);
			strcpy(line, indent_str);
			strncat(line, last, byte_count + 1);   
			// Copy the '\n' also
			append_to_contents(line, strlen(line));
			last = content_ptr + 1;
			byte_count = -1;
		    }
		}  // end of for loop
	    } else {
	       append_to_contents((const char *)contents, size);
	    }
	    bp->unlockContents(error);
	    firstBPHandled = DTM_TRUE;
	}
	if (attr) {
	    DtDtsFreeAttributeValue(attr);
	    attr = NULL;
	}
	free(type);

	// DLP: We will turn off this test for now. We need to study the problem
	// of text checksums more.
	//
	//if (bp->checksum(error) == DtMailCheckBad) {
	//    *status_string = GETMSG(DT_catd, 1, -1, "Digital signature did not match.");
	//}
    }
    delete [] line;

    if (format == IF_BRACKETED) {
	char * ins_bracket;
	switch (brackets) {
	  case BF_FORWARD:
	    ins_bracket = GETMSG(DT_catd, 1, 197, "------------- End Forwarded Message -------------\n\n");
	    break;
	    
	  case BF_INCLUDE:
	  default:
	    ins_bracket = GETMSG(DT_catd, 1, 198, "------------- End Included Message -------------\n\n");
	    break;
	}

	append_to_contents(ins_bracket, strlen(ins_bracket));
    }

    enable_redisplay();

    if (NULL != indent_str)
      free((void*) indent_str);

    return(firstBPHandled);
}

void
Editor::append_newline_to_contents()
{
    append_to_contents("\n", strlen("\n"));
}


void
Editor::set_attachment(
		DtMail::BodyPart *bp,
		InsertFormat format,
		BracketFormat brackets)
{
    DtMailEnv error;
    DtMail::Session *m_session = theRoamApp.session()->session(); 
    DtMail::MailRc * mail_rc = m_session->mailRc(error);
    DtMailValueSeq value;

    char		*input, *name, *dttype, *description, *mimetype;
    const char		*indent_str = NULL;
    unsigned long	len;

    if (format == IF_INDENTED)
    {
        mail_rc->getValue(error, "indentprefix", &indent_str);
        if (error.isSet()) 
          indent_str = strdup("> ");
    }

    disable_redisplay();
    if ((format == IF_BRACKETED) && (brackets == BF_INCLUDE))
    {
	char * ins_bracket =
	  GETMSG(
		DT_catd, 1, 249,
		"------------- Begin Included Attachment -------------\n\n");

	append_to_contents(ins_bracket, strlen(ins_bracket));
    }
    else if (format == IF_INDENTED)
    {
        append_to_contents(indent_str, strlen(indent_str));
        append_newline_to_contents();
    }

    bp->getContents(error, NULL, &len, &dttype, &name, NULL, &description);
    
    if ((NULL != name) && (0 != strlen(name)))
    {
        if (NULL != indent_str)
          append_to_contents(indent_str, strlen(indent_str));
	input = GETMSG(DT_catd, 1, 251, "       Attachment Name:  ");
        append_to_contents(input, strlen(input));
        append_to_contents(name, strlen(name));
        append_newline_to_contents();
    }

    if ((NULL != dttype) && (0 != strlen(dttype)))
    {
        if (NULL != indent_str)
          append_to_contents(indent_str, strlen(indent_str));
	input = GETMSG(DT_catd, 1, 252, "     Attachment DtType:  ");
        append_to_contents(input, strlen(input));
        append_to_contents(dttype, strlen(dttype));
        append_newline_to_contents();
    }

    bp->getContentType(error, &mimetype);
    if ((NULL != mimetype) && (0 != strlen(mimetype)))
    {
        if (NULL != indent_str)
          append_to_contents(indent_str, strlen(indent_str));
	input = GETMSG(DT_catd, 1, 253, "Attachment ContentType:  ");
        append_to_contents(input, strlen(input));
        append_to_contents(mimetype, strlen(mimetype));
        append_newline_to_contents();
    }

    if ((NULL != description) && (0 != strlen(description)))
    {
        if (NULL != indent_str)
          append_to_contents(indent_str, strlen(indent_str));
	input = GETMSG(DT_catd, 1, 254, "Attachment Description:  ");
        append_to_contents(input, strlen(input));
        append_to_contents(description, strlen(description));
        append_newline_to_contents();
    }

    if ((format == IF_BRACKETED) && (brackets == BF_INCLUDE))
    {
	char * ins_bracket =
	  GETMSG(
		DT_catd, 1, 250,
		"------------- End Included Attachment -------------\n\n");

	append_to_contents(ins_bracket, strlen(ins_bracket));
    }
    else if (format == IF_INDENTED)
    {
        append_to_contents(indent_str, strlen(indent_str));
        append_newline_to_contents();
    }
    enable_redisplay();

    if (NULL != indent_str)
      free((void*) indent_str);
    if (NULL != name)
      free((void*) name);
    if (NULL != dttype)
      free((void*) dttype);
    if (NULL != mimetype)
      free((void*) mimetype);
    if (NULL != description)
      free((void*) description);
}

void
Editor::update_display_from_props(void)
{
    int rows, cols;
    DtMailEnv  error;
    DtMail::Session * d_session = theRoamApp.session()->session();
    DtMail::MailRc * mailrc = d_session->mailRc(error);
    const char * value = NULL;

    mailrc->getValue(error, "popuplines", &value);
    if (error.isSet()) {
	value = strdup("24");
    }
    rows = (int) strtol(value, NULL, 10);
    free((void*) value);
    set_rows(rows);

    // If toolcols is set, overwrite the column width with "toolcols" value.
    // Otherwise, default resource value will be used.
    value = NULL;
    error.clear();
    mailrc->getValue(error, "toolcols", &value);
    if (!error.isSet()){
        cols = (int) strtol(value, NULL, 10);      
        free((void*) value);
    } else {
	/*
	 * MB_CUR_MAX == 1 : SingleByteLanguage
	 * MB_CUR_MAX >  1 : MultiByteLanguage
	 */
	if ( MB_CUR_MAX == 1 )
	  cols = 80;
	else
	  cols = 40;
    }
    set_columns(cols);
}


void 
AbstractEditorParent::postAttachmentPopup(XEvent *event)
{
	XmMenuPosition(_attachmentPopupMenu, (XButtonEvent *)event);
	XtManageChild(_attachmentPopupMenu);
}

void 
AbstractEditorParent::postTextPopup(XEvent *event)
{
	if (_textPopupMenu == NULL)
		return;

	XmMenuPosition(_textPopupMenu, (XButtonEvent *)event);
	XtManageChild(_textPopupMenu);
}

