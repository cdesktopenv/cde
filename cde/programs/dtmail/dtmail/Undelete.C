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
 *      $TOG: Undelete.C /main/9 1998/09/24 17:33:53 mgreess $
 *
 *      RESTRICTED CONFIDENTIAL INFORMATION:
 *
 *      The information in this document is subject to special
 *      restrictions in a confidential disclosure agreement between
 *      HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *      document outside HP, IBM, Sun, USL, SCO, or Univel without
 *      Sun's specific written approval.  This document and all copies
 *      and derivative works thereof must be returned or destroyed at
 *      Sun's request.
 *
 *      Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */
 
#include <Xm/Form.h>
#include <Xm/LabelG.h>
#include <Xm/RowColumn.h>
#include "Undelete.hh"
#include "MsgScrollingList.hh"
#include "RoamMenuWindow.h"
#ifdef DEAD_WOOD
#include "QueryDialogManager.hh"
#endif /* DEAD_WOOD */
#include "DialogShell.h"
#include "Application.h"
#include "ButtonInterface.h"
#include "RoamCmds.h"
#include "RoamApp.h"
#include "CmdList.h"
#include "MsgHndArray.hh"
#include "Help.hh"
#include "MemUtils.hh"
#include "MailMsg.h"
#include "DtMailGenDialog.hh"


UndelMsgScrollingList::UndelMsgScrollingList (RoamMenuWindow *rmw, Widget w,
						char *string)
    : MsgScrollingList(rmw, w, string) 
{
}

UndelMsgScrollingList::~UndelMsgScrollingList()
{
}

void
UndelMsgScrollingList::extended_selection(DtMailEnv &, int)
{
}


void
UndelMsgScrollingList::deleteSelected(Boolean)
{
    // SR - Added stuff below.  Made code more efficient also.
    FORCE_SEGV_DECL(MsgStruct, a_del_msg_struct);
    FORCE_SEGV_DECL(MsgStruct, tmpMS);
    int  position_in_list, i;
    FORCE_SEGV_DECL(int, position_list);
    int position_count;
    Boolean any_selected;
    int num_msgs;
 
 

    XtVaGetValues( _w,
                   XmNitemCount, &num_msgs,
                   NULL );
 
    any_selected = XmListGetSelectedPos(_w,
                                        &position_list,
                                        &position_count);
 
    if (!any_selected) return;
 
    for (i=0; i < position_count; i++ ) {
        position_in_list = *(position_list + i);
 
        a_del_msg_struct = get_message_struct(position_in_list);
        _msgs->mark_for_delete(position_in_list - 1);
           
    }
 
    _msgs->compact(0);
 
 
    // Delete the items from the scrolling list
     XmListDeletePositions(_w,
                          position_list,
                          position_count);
 
    // XnListGetSelectedPos allocated memory for the array and
    // wants us to free it.  See documentation.
 
 
    num_msgs = _msgs->length();
    XtFree((char*) position_list);
}
 
void
UndelMsgScrollingList::insertMsg(DtMailMessageHandle tmpMH)
{
    MsgScrollingList::insertMsg(tmpMH);
}

void
UndelMsgScrollingList::insertMsg(
    DtMailEnv &mail_error, 
    MsgStruct *tmpMS
)
{
    XmString item;

    _msgs->append(tmpMS);

    DtMailHeaderLine info;

    DtMail::MailBox * mbox = this->parent()->mailbox();

    mbox->getMessageSummary(mail_error,
			    tmpMS->message_handle, 
			    _header_info, 
			    info);
    DtMail::Message * msg = mbox->getMessage(mail_error, tmpMS->message_handle);


    if (mail_error.isSet()) {
	return;
    }

    item = formatHeader(info,
			tmpMS->sessionNumber,
			show_with_attachments(msg),
			msg->flagIsSet(mail_error, DtMailMessageNew));
    XmListAddItem( _w , item, 0 );
    XmStringFree( item );
 
    scroll_to_bottom();
}

void
UndelMsgScrollingList::loadMsgs(
    DtMailEnv &mail_error, 
    MsgHndArray *deleted_msgs,
    int count
)
{
    MsgStruct *tmpMS;
    XmString *msg_hdrs;
    int i;
    DtMailHeaderLine info;
    DtMail::Message * msg;


    msg_hdrs = (XmString *)malloc(sizeof(XmString) * count);
    memset(msg_hdrs, 0, sizeof(XmString) * count);    

    DtMail::MailBox * mbox = this->parent()->mailbox();

    for (i = 0; i < count; i ++) {
	tmpMS = deleted_msgs->at(i);
	_msgs->append(tmpMS);
	
	mbox->getMessageSummary(mail_error,
			    tmpMS->message_handle, 
			    _header_info, 
			    info);

	msg =  mbox->getMessage(mail_error, tmpMS->message_handle);

	if (mail_error.isSet()) {
	    return;
	}

	msg_hdrs[i] = formatHeader(info,
				tmpMS->sessionNumber,
				show_with_attachments(msg),
				msg->flagIsSet(mail_error, DtMailMessageNew));
    }

    XmListAddItems(_w, msg_hdrs, count, 0);

    // Free the strings we added to the header array.
    for (i = 0; i < count; i++) {
	XmStringFree(msg_hdrs[i]);
    }

    free (msg_hdrs);

    scroll_to_bottom();
}

void
UndelFromListDialog::loadMsgs(
    DtMailEnv &mail_error, 
    MsgHndArray *deleted_msgs,
    int count
)
{
    _list->loadMsgs(mail_error, deleted_msgs, count);
}

UndelFromListDialog::UndelFromListDialog ( 
    char *name, 
    RoamMenuWindow *parent
) : DialogShell ( name, parent )
{
    assert (theApplication != NULL);
    _list = NULL;
    _undelete_button = NULL;
    _close_button = NULL;
    _undel_list_button = NULL;
    rowOfLabels = NULL;
    rowOfButtons = NULL;
    rowOfMessageStatus = NULL;
    my_owner = NULL;
}

UndelFromListDialog::~UndelFromListDialog()
{
}

void
UndelFromListDialog::popped_down()
{
    XtUnmapWidget(this->baseWidget());
}

void
UndelFromListDialog::popped_up()
{
    if (_workArea)
	XtMapWidget(this->baseWidget());
}

void
UndelFromListDialog::initialize()
{
    DialogShell::initialize();
}

void
UndelFromListDialog::quit()
{
    // Pop down the dialog
    this->popped_down();
}

Widget
UndelFromListDialog::createWorkArea ( Widget parent )
{

    XmString labelStr;
    Widget l1, l2, l3, l4;
    Widget form1;
    XtWidgetGeometry size;

    form1 = XmCreateForm(parent, "Work_Area",  NULL, 0);

    // add help callback
    printHelpId("form1", form1);

    rowOfLabels = XtCreateManagedWidget ("rowOfLabels",
			xmFormWidgetClass, 
			form1,
			NULL, 0);


    printHelpId("rowOfLabels", rowOfLabels);
    // add help callback

    size.request_mode = CWHeight;
    XtQueryGeometry(rowOfLabels, NULL, &size);
    XtVaSetValues(rowOfLabels,
		XmNpaneMaximum, size.height,
		XmNpaneMinimum, size.height,
		XmNrightAttachment, XmATTACH_FORM,
		XmNleftAttachment, XmATTACH_FORM,
		XmNtopAttachment, XmATTACH_FORM,
		XmNtopOffset, 5,
		XmNorientation, XmHORIZONTAL,
		XmNspacing, 120,
		XmNmarginWidth, 20,
		XmNentryAlignment, XmALIGNMENT_CENTER,
		NULL);

    labelStr = XmStringCreateLocalized(GETMSG(DT_catd, 1, 14,
           "Sender"));

    l1 = XtVaCreateManagedWidget("Label1", xmLabelGadgetClass,
                rowOfLabels,
                XmNlabelString, labelStr,
                NULL);

    XmStringFree(labelStr);
    printHelpId("l1", l1);
    /* add help callback */
    // XtAddCallback(l1, XmNhelpCallback, HelpCB, helpId);


    labelStr = XmStringCreateLocalized(GETMSG(DT_catd, 1, 15,
           "Subject"));
    l2 = XtVaCreateManagedWidget("Label2",
                                xmLabelGadgetClass, rowOfLabels,
                                XmNlabelString, labelStr,
                                NULL);



    XmStringFree(labelStr);
    printHelpId("l2", l2);
    /* add help callback */
    // XtAddCallback(l2, XmNhelpCallback, HelpCB, helpId);

    labelStr = XmStringCreateLocalized(GETMSG(DT_catd, 1, 16,
           "Date and Time"));
    l3 = XtVaCreateManagedWidget("Label3",
                        xmLabelGadgetClass, rowOfLabels,
                        XmNlabelString, labelStr,
                        NULL);

    XmStringFree(labelStr);
    printHelpId("l3", l3);
    /* add help callback */
    // XtAddCallback(l3, XmNhelpCallback, HelpCB, helpId);

    labelStr = XmStringCreateLocalized(GETMSG(DT_catd, 1, 17,
           "Size"));
    l4 = XtVaCreateManagedWidget("Label4",
                xmLabelGadgetClass, rowOfLabels,
                XmNlabelString, labelStr,
                NULL);

    XmStringFree(labelStr);
    printHelpId("l4", l4);
    /* add help callback */

    _list = new UndelMsgScrollingList( this->parent(), form1, "Message_List");

    DtMailEnv error;
    DtMail::Session * d_session = theRoamApp.session()->session();
    DtMail::MailRc * mailrc = d_session->mailRc(error);
    const char * value;
    int msgnums = False;

    mailrc->getValue(error, "showmsgnum", &value);
    if (error.isNotSet()) {
        msgnums = True;
    }
    if (NULL != value)
      free((void*) value);

    // Adjust labels so the align on the columns
    _list->layoutLabels(l1, l2, l3, l4);


    rowOfButtons = XtCreateManagedWidget("RowColumn",
					xmFormWidgetClass,
					form1,
					NULL, 0);
    // add help callback
    printHelpId ("rowOfButtons", rowOfButtons);

    XtVaSetValues(XtParent(_list->get_scrolling_list()),
		XmNrightAttachment, XmATTACH_FORM,
		XmNleftAttachment, XmATTACH_FORM,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, rowOfLabels,
		XmNtopOffset, 3,
		XmNbottomAttachment, XmATTACH_WIDGET,
		XmNbottomWidget, rowOfButtons,
		XmNbottomOffset, 7,
		NULL);

    XtVaSetValues(rowOfButtons,
		XmNorientation, XmHORIZONTAL,
		XmNpaneMaximum, size.height,
		XmNpaneMinimum, size.height,
		XmNrightAttachment, XmATTACH_FORM,
		XmNleftAttachment, XmATTACH_FORM,
		XmNbottomAttachment, XmATTACH_FORM,
	        XmNfractionBase, 100,
		XmNresizable, FALSE,
		NULL);
    
    // Create undelete and close buttons.
    this->addToRowOfButtons();



    // Label should go here that says in how many days
    // deleted messages will be destroyed, and for status.
    rowOfMessageStatus = XtCreateManagedWidget("Message_Status",
				xmFormWidgetClass,
				form1, NULL, 0);
    XtVaSetValues(rowOfMessageStatus,
		XmNrightAttachment, XmATTACH_FORM,
		XmNleftAttachment, XmATTACH_FORM,
		XmNtopAttachment, XmATTACH_FORM,
		XmNtopOffset, 5,
		NULL);


    XtManageChild(_list->baseWidget());
    XtManageChild(form1);

    // return the widget
    return(form1);
}
    
void
UndelFromListDialog::insertMsg(DtMailEnv &mail_error, MsgStruct *tmpMS)
{
    _list->insertMsg(mail_error, tmpMS);
}

void
UndelFromListDialog::undelLast()
{
    int len;

    len = _list->get_messages()->length();
    _list->get_messages()->mark_for_delete(len - 1);
    _list->get_messages()->compact(0);
    XmListDeletePos(_list->get_scrolling_list(), len);

}


void
UndelFromListDialog::undelSelected()
{
    FORCE_SEGV_DECL(int, position_list);
    FORCE_SEGV_DECL(MsgHndArray, tmpMHlist);
    int position_count;
    Boolean any_selected = FALSE;

    any_selected = XmListGetSelectedPos(_list->get_scrolling_list(),
					&position_list,
					&position_count);

    // Don't do anything if there aren't any messages selected.

    if ((!any_selected) || (position_count <= 0)) return;


	// Put the selected messages from the Deleted Messages List
	// into the RoamMenuWindow Message Scrolling List.


	// This is kind of tricky, because we have to get the list of
	// selected messages before we delete the selected messages,
	// but we have to delete the selected messages before we
	// pass them to undelete_messages().  The reason for this
	// is that deleteSelected() changes the is_deleted attribute
	// of the message handle to TRUE, which will undo undelete_messages()
	// setting is_deleted to FALSE!  Did you follow all that?

    tmpMHlist = _list->selected();

	// Remove the selected messages from the Deleted Messages Dialog.
    _list->deleteSelected();

	// Add the messages to the RoamMenuWindow scrolling list.
    MsgScrollingList *msl = parent()->list();
    msl->undelete_messages(tmpMHlist);

    XtFree((char*) position_list);

}

void
UndelFromListDialog::addToRowOfButtons()
{
    FORCE_SEGV_DECL(CmdInterface, ci);
    Widget w, prev_widget;

    _undelete_button = new DoUndeleteCmd(
	                        "Undelete", 
	                        GETMSG(DT_catd, 1, 115, "Undelete"), 
	                        TRUE, 
	                        this);
    ci = new ButtonInterface (rowOfButtons, _undelete_button);

    w = ci->baseWidget();
    XtVaSetValues(w,
        XmNleftAttachment, XmATTACH_POSITION,
	XmNleftPosition, 35,          // centralized tUndelete and Close buttons
        XmNbottomAttachment, XmATTACH_FORM,
        NULL );
    ci->manage();
    prev_widget = w;

    _close_button = new CloseUndelCmd(
				"Close",
				GETMSG(DT_catd, 1, 116, "Close"), 
				TRUE, 
				this);
    ci = new ButtonInterface (rowOfButtons, _close_button);
    w = ci->baseWidget();
    XtVaSetValues(w,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, prev_widget,
        XmNleftOffset, 30,
        XmNbottomAttachment, XmATTACH_FORM,
        NULL );
    ci->manage();

}

void
UndelFromListDialog::expunge(void)
{
    for (int pos = 0; pos < _list->get_num_messages(); pos++) {
	_list->get_messages()->mark_for_delete(pos);
    }

    _list->get_messages()->compact(0);

    XmListDeleteAllItems(_list->get_scrolling_list());
}

