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
 *	$TOG: FindDialog.C /main/7 1998/07/23 17:59:06 mgreess $
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
#include <X11/Intrinsic.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/MessageB.h>
#include <Xm/TextF.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/DialogS.h>
#include <Xm/PanedW.h>
#include <Xm/LabelG.h>
#include <Xm/Text.h>
#include <Xm/SeparatoG.h>
#include <DtMail/DtMail.h>
#include <DtMail/DtMail.hh>
#include "FindDialog.h"
#include "RoamApp.h"
#include "RoamMenuWindow.h"
#include "RoamCmds.h"
#include "Help.hh"
#include "MailMsg.h"
#include <EUSCompat.h>
#include "str_utils.h"


//
// Clear out the data. After this function is complete the
// data should look as if the constructor was just called and
// before initialize().
//
void
FindDialog::clear()
{
  register unsigned int		offset;

  //
  if (_text_labels != NULL) {
    for (offset = 0; offset < _num_text_fields; offset++) {
      if (_text_labels[offset] != NULL) {
	free(_text_labels[offset]);
      }
    }
    delete _text_labels;
  }

  //
  if (_text_values != NULL) {
    for (offset = 0; offset < _num_text_fields; offset++) {
      if (_text_values[offset] != NULL) {
	free(_text_values[offset]);
      }
    }
    delete _text_values;
  }

  //
  if (_text_abstract_name != NULL) {
    for (offset = 0; offset < _num_text_fields; offset++) {
      if (_text_abstract_name[offset] != NULL) {
	free(_text_abstract_name[offset]);
      }
    }
    delete _text_abstract_name;
  }


  //
  if (_buttonData != NULL) {
    for (offset = 0; offset < _num_buttons; offset++) {
      if (_buttonData[offset].label != NULL) {
	free(_buttonData[offset].label);
      }
    }
    delete _buttonData;
  }

  if (_text_fields != NULL) {
    delete _text_fields;
  }

  if (_text_names != NULL) {
    delete _text_names;
  }
}

//
// The only constructor.
//
FindDialog::FindDialog(RoamMenuWindow *parent) : Dialog("find", parent)
{
  _roamWindow = parent;
  _num_text_fields = 4;
  _num_buttons = 5;

  //
  // Allocate storage for labels, widgets, and data.
  //
  _text_labels = new char *[_num_text_fields];
  _text_names = new char *[_num_text_fields];
  _text_values = new char *[_num_text_fields];
  _text_abstract_name = new char *[_num_text_fields];
  _text_fields = new Widget[_num_text_fields];
  _buttonData = new ActionAreaItem[_num_buttons];
  _searchForward = TRUE;

  //
  // Initialize the buttons.
  //
  _buttonData[0].label = strdup(GETMSG(DT_catd, 1, 183, "Find"));
  _buttonData[0].callback = findCallback;
  _buttonData[0].data = (caddr_t) this;

#ifdef NL_OBSOLETE
  /*
   * NL_COMMENT
   * This is an obsolete message.  Replaced by message 220 in set 1
   */
  _buttonData[1].label = strdup(GETMSG(DT_catd, 1, 184, "Find & Select All"));
#endif
   /*
    * NL_COMMENT
    * This message replaces message 184 in set 1
    */
  _buttonData[1].label = strdup(GETMSG(DT_catd, 1, 220, "Select All"));
  _buttonData[1].callback = findSelectAllCallback;
  _buttonData[1].data = (caddr_t) this;

  _buttonData[2].label = strdup(GETMSG(DT_catd, 1, 185, "Clear"));
  _buttonData[2].callback = clearCallback;
  _buttonData[2].data = (caddr_t) this;

  _buttonData[3].label = strdup(GETMSG(DT_catd, 1, 186, "Close"));
  _buttonData[3].callback = closeCallback;
  _buttonData[3].data = (caddr_t) this;

  _buttonData[4].label = strdup(GETMSG(DT_catd, 1, 187, "Help"));
  _buttonData[4].callback = HelpCB;
  _buttonData[4].data = (caddr_t) DTMAILFINDDIALOG;

  _text_labels[0] = strdup(GETMSG(DT_catd, 1, 188, "To:"));
  _text_labels[1] = strdup(GETMSG(DT_catd, 1, 189, "From:"));
  _text_labels[2] = strdup(GETMSG(DT_catd, 1, 190, "Subject:"));
  _text_labels[3] = strdup(GETMSG(DT_catd, 1, 191, "Cc:"));

  // These strings should not be translated.  They are
  // the Motif names for the widgets that will be created (they are
  // not the labels).
  _text_names[0] = "To";
  _text_names[1] = "From";
  _text_names[2] = "Subject";
  _text_names[3] = "Cc";

  //
  // Initialize the names of the fields to the abstract
  // names used by libDtMail.
  //
  _text_abstract_name[0] = strdup(DtMailMessageTo);
  _text_abstract_name[1] = strdup(DtMailMessageSender);
  _text_abstract_name[2] = strdup(DtMailMessageSubject);
  _text_abstract_name[3] = strdup(DtMailMessageCc);
}

//
// Print a string in the status line of the find dialog.
//
void
FindDialog::setStatus(const char * str)
{
    char *tmpstr = strdup(str);
    XmString label = XmStringCreateLocalized(tmpstr);
 
    XtVaSetValues(_status_text,
                  XmNlabelString, label,
                  NULL);
 
    XmUpdateDisplay(baseWidget());
    XmStringFree(label);
}

//
// Clear the status line of the find dialog.
//
void
FindDialog::clearStatus(void)
{
    setStatus(" ");
}

//
// Create the guts of the dialog
//
Widget
FindDialog::createWorkArea(Widget dialog)
{
  // TODO - CHECK ERROR!!!
  Widget *label = new Widget[_num_text_fields]; 


  register unsigned int		offset;

  _name = GETMSG(DT_catd, 1, 192, "Mailer - Find");

  title(_name);

	// make this a modal dialog
	/*
	XtVaSetValues (dialog,
			XmNdialogStyle,	XmDIALOG_FULL_APPLICATION_MODAL,
			NULL);
	*/

  	printHelpId("dialog", dialog);

  /* add help callback */
  // XtAddCallback(dialog, XmNhelpCallback, HelpCB, helpId);

	Widget fd_pane = XtVaCreateWidget ("fd_pane",
				xmPanedWindowWidgetClass,
				dialog,
				XmNsashWidth,	1,
				XmNsashHeight,	1,
				NULL);

	printHelpId ("fd_pane", fd_pane);
	// add help callback
	// XtAddCallback (fd_pane, XmNhelpCallback, HelpCB, helpId);

	Widget	fd_form = XtVaCreateWidget ("fd_form",
				xmFormWidgetClass,
				fd_pane,
				XmNfractionBase,	100,
				NULL);

	printHelpId ("fd_form", fd_form);
	// add help callback
	// XtAddCallback (fd_form, XmNhelpCallback, HelpCB, helpId);


	Widget _fd_labelbox = XtVaCreateManagedWidget ("_fd_labelbox",
				xmRowColumnWidgetClass,
				fd_form,
				XmNtopAttachment,	XmATTACH_FORM,
				XmNleftAttachment,	XmATTACH_POSITION,
				XmNrightAttachment,	XmATTACH_POSITION,
				XmNleftPosition,	5,
				XmNrightPosition,	95,
				XmNpacking,		XmPACK_COLUMN,
				XmNnumColumns,		2,
				XmNorientation,		XmVERTICAL,
				XmNisAligned,		True,
				XmNentryAlignment,	XmALIGNMENT_END,
				XmNentryVerticalAlignment,	XmALIGNMENT_CENTER,
				NULL); 
	printHelpId ("_fd_labelbox", _fd_labelbox);
	// add help callback
	// XtAddCallback (_fd_labelbox, XmNhelpCallback, HelpCB, helpId);


	Widget	*_fd_labels = new Widget [_num_text_fields];

	int	_fd_i = 0;
	for (_fd_i = 0; _fd_i < _num_text_fields; _fd_i++)
	{
		_fd_labels [_fd_i] = XtVaCreateManagedWidget (
					_text_labels [_fd_i],
					xmLabelGadgetClass,
					_fd_labelbox,
					NULL);

		printHelpId ("_fd_labels [%s]", _fd_labels [_fd_i]);
		// naturally, this is bogus --must be fixed to return proper label
		// add help callback
		// XtAddCallback(_fd_labels [_fd_i], XmNhelpCallback, HelpCB, helpId);
	}

	for (_fd_i = 0; _fd_i < _num_text_fields; _fd_i++)
	{
		_text_fields [_fd_i] = XtVaCreateManagedWidget (
					_text_names [_fd_i],
					xmTextFieldWidgetClass,
					_fd_labelbox,
					NULL);
		printHelpId ("_text_fields [%s]", _text_fields [_fd_i]);
		// naturally, this is bogus --must be fixed to return proper label
		// add help callback
		// XtAddCallback(_text_fields [_fd_i], XmNhelpCallback, HelpCB, helpId);

		XtAddCallback(_text_fields [_fd_i], XmNactivateCallback,
			(XtCallbackProc)textFieldCallback, (XtPointer)this);
	}


  XmString	strForward = XmStringCreateLocalized(GETMSG(DT_catd, 1, 193, "Forward"));
  XmString	strBackward = XmStringCreateLocalized(GETMSG(DT_catd, 1, 194, "Backward"));

  Widget fd_direction
	= XmVaCreateSimpleRadioBox(fd_form,
				"Direction",
			       0,		// Initial selection
			       directionCallback,
				//NULL,
			       XmVaRADIOBUTTON, strForward, NULL, NULL, NULL,
			       XmVaRADIOBUTTON, strBackward, NULL, NULL, NULL,
			       XmNuserData,	this,
				XmNsensitive,	True,
				XmNtopAttachment,	XmATTACH_WIDGET,
				XmNtopWidget,		_fd_labelbox,
				XmNorientation,	XmHORIZONTAL,
				XmNleftAttachment,	XmATTACH_POSITION,
				XmNleftPosition,	33,
			       NULL);
	 printHelpId ("fd_direction", fd_direction);
	// add help callback
	//XtAddCallback (fd_direction, XmNhelpCallback, HelpCB, helpId);

  XmStringFree(strForward);
  XmStringFree(strBackward);

  //
  // Now create the Action Area.
  //
#define TIGHTNESS	20

  register Widget		widget;

  Widget fd_action = XtVaCreateWidget("actionArea",
				 xmFormWidgetClass,
				 fd_pane,
				 XmNleftAttachment,	XmATTACH_FORM,
				 XmNrightAttachment,	XmATTACH_FORM,
				 XmNfractionBase, TIGHTNESS * _num_buttons-1,
				 NULL);
	 printHelpId ("actionArea", fd_action);
	// add help callback
	//XtAddCallback (fd_action, XmNhelpCallback, HelpCB, helpId);

  for (offset = 0; offset < _num_buttons; offset++) 
  {  widget = XtVaCreateManagedWidget(_buttonData[offset].label,
				     xmPushButtonWidgetClass,	fd_action,

				     XmNleftAttachment,
				     offset ? XmATTACH_POSITION:XmATTACH_FORM,

				     XmNleftPosition,	TIGHTNESS * offset,
				     XmNtopAttachment,	XmATTACH_FORM,

				     XmNrightAttachment,
				     offset != _num_buttons - 1 ? XmATTACH_POSITION : XmATTACH_FORM,

				     XmNrightPosition,
				     TIGHTNESS * offset + (TIGHTNESS - 1),

				     XmNshowAsDefault,	offset == 0,
				     NULL);

	// again, bogus -- doesn't each one need a unique tag?
	 printHelpId ("widget", widget);
	// add help callback
	//XtAddCallback (widget, XmNhelpCallback, HelpCB, helpId);

    if (_buttonData[offset].callback != NULL) {
      XtAddCallback(widget, XmNactivateCallback,
		    _buttonData[offset].callback,
		    _buttonData[offset].data);
    }


    if (offset == 0) {
      Dimension		height;
      Dimension		margin;

      XtVaGetValues(fd_action, XmNmarginHeight, &margin, NULL);
      XtVaGetValues(widget, XmNheight, &height, NULL);
      height +=2 * margin;
      XtVaSetValues(fd_action,
		    XmNdefaultButton,	widget,
		    XmNpaneMaximum,	height,
		    XmNpaneMinimum,	height,
		    NULL);

    }
  }

  _status_text = XtVaCreateManagedWidget("StatusLabel",
					   xmLabelWidgetClass, fd_pane,
                                           XmNrightAttachment, XmATTACH_FORM,
                                           XmNleftAttachment, XmATTACH_FORM,
                                           XmNalignment, XmALIGNMENT_BEGINNING,
                                           NULL);
	    
  Dimension height;
  XtWidgetGeometry size;

  size.request_mode = CWHeight;
  XtQueryGeometry(_status_text, NULL, &size);
  XtVaSetValues(_status_text,
		XmNpaneMaximum, size.height,
		XmNpaneMinimum, size.height,
		NULL);
 
  clearStatus();

  XtManageChild (fd_form);
  XtManageChild (fd_direction);
  XtManageChild(fd_action);
  XtManageChild(fd_pane);

  XtManageChild(dialog);

  // Make sure get the height of the dialog after it has been
  // managed.
  XtVaGetValues(dialog, XmNheight, &height, NULL);
  XtVaSetValues(dialog, 
		XmNmappedWhenManaged, True,
		XmNminHeight, height,
		NULL);
  XtRealizeWidget(dialog);

  return (fd_pane);
}


//
// Look for all matching messages.
//
Boolean
FindDialog::findMatching(Boolean findAll)
{
  // TODO - CHECK ERROR!!!
  DtMailEnv		error;
  unsigned int		matchCount = 0;

  /* NL_COMMENT
   * This string is displayed on the find dialog status line
   * when searching for a matching message.
   */

  setStatus(GETMSG(DT_catd, 1, 231, "Searching..."));
  busyCursor();
  theRoamApp.busyAllWindows(NULL);

  //
  // Get the active list.
  //
  MsgScrollingList	* displayList = _roamWindow->list();

  //
  // Find  the max. number of messages that we are to find matching.
  //
  int		 	  numberMessages = displayList->get_num_messages();

  //
  // Are there any messages?
  //
  if (numberMessages > 0) {

    //
    // A pointer to the currently interesting message.
    //
    register DtMailMessageHandle	  currentHandle = NULL;

    //
    // The offset of the currentHandle in the MsgScrollingList.
    //
    register int		  handleOffset;

    //
    // Find the current message. We would always start from the
    // currently selected message.
    //
    // Get the handle to the currently displaied message.
    //
    DtMailMessageHandle	  initialHandle = displayList->current_msg_handle();

    //
    // Get the list of DtMailMessageHandle's.
    
    MsgHndArray		* msgHandles = displayList->get_messages();

    //
    // Up to all of them can match, allocate and clear the list.
    //
    DtMailMessageHandle	* matchList = NULL;
    if (findAll) {
      matchList = new DtMailMessageHandle[numberMessages+1];
    }
    unsigned int	 matchOffset = 0;

    //
    // Deselect all messages.
    //
    XmListDeselectAllItems(displayList->baseWidget());

    //
    // Start the search from the initially displaied message (+1).
    //
    handleOffset = displayList->position(initialHandle) - 1;
    if (_searchForward) {
      handleOffset++;
      if (handleOffset >= numberMessages) {
	handleOffset = 0;
      }
    } else {
      handleOffset--;
      if (handleOffset < 0) {
	handleOffset = numberMessages - 1;
      }
    }

    for (; handleOffset < numberMessages;) {
      currentHandle = msgHandles->at(handleOffset)->message_handle;
    
      //
      // See if this message is a match, if it is...
      //
      if (compareMessage(currentHandle)) {
	matchCount++;

	//
	// If we are finding all, then add to the list.
	// If not, then display this message and we are done.
	//
	if (findAll) {
	  matchList[matchOffset++] = currentHandle;
	} else {
	  XmListDeselectAllItems(displayList->baseWidget());
	  //displayList->set_selected_item_position(handleOffset);
	  displayList->display_and_select_message(error, currentHandle);
	  break;			// Only one.
	}
      }

      //
      // If we have looped back to the initial
      // message (handle), then we are done.
      //
      if (currentHandle == initialHandle) {
	break;
      }

      //
      // Get the next message.
      //
      // If we have reached the end, start over.
      // (as if the list was a circular list)
      //
      // We loop forward (_searchForward == TRUE) else we loop backward.
      //
      if (_searchForward) {
	handleOffset++;
	if (handleOffset >= numberMessages) {
	  handleOffset = 0;
	}
      } else {
	handleOffset--;
	if (handleOffset < 0) {
	  handleOffset = numberMessages - 1;
	}
      }
      currentHandle = msgHandles->at(handleOffset)->message_handle;
    }

    //
    // Select all the messages that match, and display the last
    // one in the list.
    //
    if (findAll) {
      
      displayList->select_all_and_display_last(error, matchList, matchCount);
      if (matchCount > 0) {
	char *line = new char[80];
	/* NL_COMMENT
	 * These strings are displayed on the find dialog status line
	 * when one or more matching messages are found.  The first
	 * string is displayed when there is one matching message,
	 * and the second string is displayed when there is more than
	 * one.  The %d is the number of messages that matched.
	 */
	if (matchCount == 1) {
	    strcpy(line, GETMSG(DT_catd, 1, 232, "1 message selected"));
	} else {
	    sprintf(line, GETMSG(DT_catd, 1, 233, "%d messages selected"), 
			    matchCount);
	}
	setStatus(line);
	delete [] line;
      }

      // Clean up.
      delete matchList;
      matchList = NULL;
    }
  }

  normalCursor();
  theRoamApp.unbusyAllWindows();
  if (error.isNotSet()) {
    if (matchCount > 0) {
	if (!findAll) {
	    clearStatus();
	}
	return(TRUE);
    }
  }
  /* NL_COMMENT
   * This string is displayed on the find dialog status line when
   * no matching messages were found.
   */
  setStatus(GETMSG(DT_catd, 1, 234, "No matches were found"));
  return(False);
}

Boolean
FindDialog::compareMessage(DtMailMessageHandle	  handle)
{
  Boolean				found = False;
  register unsigned int		offset;

  //
  // Check for something to do.
  //
  for (offset = 0; offset < _num_text_fields; offset++) {
    if (_text_values[offset] != NULL) {
      break;
    }
  }

  // If all fields are empty then we match anything
  if (offset >= _num_text_fields) {
	return TRUE;
  }

  if (offset < _num_text_fields && handle != NULL) {

    // TODO - CHECK ERROR!!!
    DtMailEnv		error;

    //
    // Get the mail box.
    //
    DtMail::MailBox	* mbox = _roamWindow->mailbox();

    //
    // Get the DtMail::Message and Envelope for this handle.
    //
    DtMail::Message	* message = mbox->getMessage(error, handle);
    DtMail::Envelope	* envelope = message->getEnvelope(error);

    //
    // Get the meassage header.
    //
    DtMailValueSeq	  header;

    for (offset = 0; offset < _num_text_fields; offset++) {
      if (_text_values[offset] != NULL) {
	if (_text_abstract_name[offset] != NULL) {
	  envelope->getHeader(error, _text_abstract_name[offset],
			      DTM_TRUE, header);
	  found = TRUE;
	} else {
	  envelope->getHeader(error, _text_names[offset],
			      DTM_FALSE, header);
	  found = TRUE;
	}
	if (!compareHeader(error, header, _text_values[offset])) {
	  found = False;
	  break;
	}
        else {
          // Problem: if we have multiple search fields ... and use
          // the same "header" array ... "compareHeader" looks for
          // each "find" field in each (available) header field.
          // So, make sure only one is available for searching.
          // Really "offset" should be passed to "compareHeader" ...
          // That way, correct comparison can be done and the
          // memory for this array can be released correctly via the
          // destructor .... since "remove" fails to do so.
          header.remove(0);
        }
      }
    }
    if (offset > _num_text_fields) {
      found = TRUE;
    }
  }
  return(found);
}

#if !defined(CSRG_BASED)
//
// See if string 'toFind' is anyware in string 'str'.
// A case-insensitive version of strstr().
//
static const char	*
strcasestr(const char *str, const char *toFind)
{
  const char	*result = NULL;		// Default to not found.

  if (str && toFind) {		// Sanity check
    register int	offset = 0;
    register int	lenToFind = strlen(toFind);
    register int	lenStr = strlen(str);

    //
    // If toFind == "", then return the entire string (like strstr()).
    //
    if (lenToFind == 0) {
      result = str;
    } else {
      //
      // Start at each position in the string and look for
      // toFind - ignore case.
      //
      for (offset = 0; offset + lenToFind <= lenStr; offset++) {
	if (strncasecmp(&str[offset], toFind, lenToFind) == 0) {
	  result = &str[offset];
	  break;
	}
      }
    }
  }
  return(result);
}
#endif

Boolean
FindDialog::compareHeader(DtMailEnv		& error,
			  DtMailValueSeq	& header,
			  const char		* cmpToString)
{
  register int		headerOffset = header.length() - 1;

  error.clear();

  while(headerOffset >= 0) {
    if ((strcasestr(*(header[headerOffset]), cmpToString)) != NULL) {
      return(TRUE);
    }
    headerOffset--;
  }
  return(False);
}

//
// Pull all fields out of the dialog and store in the class.
//
void
FindDialog::getAllFields()
{
  register unsigned int		offset;

  for (offset = 0; offset < _num_text_fields; offset++) {
    if (_text_fields[offset] != NULL) {
      _text_values[offset] = XmTextFieldGetString(_text_fields[offset]);

      // Ignore zero length strings.
      if (_text_values[offset] != NULL) {
	if (strlen(_text_values[offset]) == 0) {
	  _text_values[offset] = NULL;
	}
      }
    }
  }

  return;
}

void
FindDialog::textFieldCallback(
	Widget		field,
	XtPointer	data,
	XtPointer)
{
	char *s;
	FindDialog    *findData = (FindDialog *)data;

	if (*(s = XmTextGetString(field)) == '\0') {
		// Empty field.  Traverse
		(void) XmProcessTraversal(field, XmTRAVERSE_NEXT_TAB_GROUP);
	} else {
		// Field not empty. Do search
		findData->getAllFields();
		if (!findData->findMatching(False)) {
			XBell(XtDisplay(field), 0);
		}
	}

	return;
}	

void
FindDialog::directionCallback(Widget	widget,
			      XtPointer closure,
			      XtPointer)
{
  int	       which = (int) ((long) closure);	// closure contains button #
  FindDialog	*find; 
  
  // Client data is actually on the @!$?@* parent, not the toggle item
  XtVaGetValues(XtParent(widget), XmNuserData, &find, NULL);

  if (which == 0) {
    find->setSearchForward(TRUE);
  } else {
    find->setSearchForward(False);
  }

  return;
}

void
FindDialog::findCallback(Widget		/*button*/,
			 XtPointer	closure,
			 XtPointer	/*call_data*/)
{
  FindDialog	*findData = (FindDialog *)closure;

  findData->getAllFields();
  findData->findMatching(False);
  return;
}


void
FindDialog::findSelectAllCallback(Widget	/*button*/,
				  XtPointer	closure,
				  XtPointer	/*call_data*/)
{
  FindDialog	*findData = (FindDialog *)closure;

  findData->getAllFields();
  findData->findMatching(TRUE);
  return;
}

void
FindDialog::clearCallback(Widget	/*button*/,
			  XtPointer	closure,
			  XtPointer	/*call_data*/)
{
  FindDialog	*findData = (FindDialog *)closure;
  register unsigned int		offset;

  for (offset = 0; offset < findData->_num_text_fields; offset++) {
    if (findData->_text_fields[offset] != NULL) {
      XmTextFieldSetString(findData->_text_fields[offset], "");
    }
  }
  return;
}

void
FindDialog::closeCallback(Widget	/*button*/,
			  XtPointer	closure,
			  XtPointer	/*call_data*/)
{
  FindDialog	*findData = (FindDialog *)closure;
  
  findData->popdown();
  return;
}
