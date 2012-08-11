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
 *	$TOG: MsgScrollingList.C /main/38 1998/12/10 19:08:02 mgreess $
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
#include <ctype.h>
#include <assert.h>
#include <Xm/Text.h>
#include <Dt/Dts.h>

#include <DtMail/DtMailError.hh>
#include <DtMail/IO.hh>
#include "DtMailGenDialog.hh"
#include "DtMailHelp.hh"
#include "Help.hh"	// Remove after fixing problem with empty time headers
#include "MailMsg.h"
#include "MemUtils.hh"
#include "MsgHndArray.hh"
#include "MsgScrollingList.hh"
#ifdef DEAD_WOOD
#include "QueryDialogManager.hh"
#endif /* DEAD_WOOD */
#include "RoamApp.h"
#include "RoamMenuWindow.h"
#include "Sort.hh"


#include <X11/IntrinsicP.h> // Include for moving X location of titles

extern int force( Widget );

MsgScrollingList::MsgScrollingList(
    RoamMenuWindow *menuwindow,
    Widget parent,
    char *name
)
    : ScrollingList(
	parent,
	name
)
{
    _parent=menuwindow;
    _numbered = DTM_FALSE;
    _selected_item_position=-1;
    _displayed_item_position=-1;
    _selection_on = FALSE;
    _xmstr_collector = NULL;
    _xtarg_collector = NULL;
    _selected_items = NULL;
    _sorter = new Sort ();

    XtAddCallback( _w,
		   XmNextendedSelectionCallback,
		   (XtCallbackProc)
		   &MsgScrollingList::extendedSelectionCallback,
		   this );

    _msgs = new MsgHndArray(1024);
    _deleted_messages = new MsgHndArray(1024);
    num_new_messages = 0;
    num_deleted_messages = 0;
    session_message_number = 0;
   
    // Can later initialize these from the last use of the session.
    // Each folder will have some idea of which message was last
    // read.  We should select and display it at next load, no?

    _selected_item_position = 0;
    _displayed_item_position = 0;

    DtMailEnv mail_error;
    // Initialize the mail_error.
    mail_error.clear();
    DtMail::Session * d_session = theRoamApp.session()->session();
    DtMail::MailRc * mailrc = d_session->mailRc(mail_error);

    if (mailrc) {
    	const char * value = NULL;
    	mailrc->getValue(mail_error, "showto", &value);
	if (mail_error.isNotSet())
	  _header_info.number_of_names = 5;
    	else 
	  _header_info.number_of_names = 4;
	if (NULL != value)
	  free((void*) value);
    }
    else
	 _header_info.number_of_names = 4;

    // Set up array for 5 items. The DtMailMessageTo is used only 
    // if showto is set, but create placeholder for 5th item in case 
    // they apply props in this same session. Then we just have to
    // toggle between 4 or 5 number_of_nanes.

    _header_info.header_name = new (char* [5]);

    _header_info.header_name[0] = NULL;
    _header_info.header_name[1] = NULL;
    _header_info.header_name[2] = NULL;
    _header_info.header_name[3] = NULL;
    _header_info.header_name[4] = NULL;

    _header_info.header_name[0] = strdup(DtMailMessageSender);
    _header_info.header_name[1] = strdup(DtMailMessageReceivedTime);
    _header_info.header_name[2] = strdup(DtMailMessageContentLength);
    _header_info.header_name[3] = strdup(DtMailMessageSubject);
    _header_info.header_name[4] = strdup(DtMailMessageTo);
}

MsgScrollingList::~MsgScrollingList()
{
    MsgStruct	*ms;
    int 	i, length;

    for (i=0; i<5; i++)
    {
        if (_header_info.header_name[i])
	{
            free(_header_info.header_name[i]);
            _header_info.header_name[i] = NULL;
        }
    }
    delete _header_info.header_name;

    for (i=0, length=_deleted_messages->length(); i<length; i++)
    {
	ms = _deleted_messages->at(i);
        delete ms;
    }
    _deleted_messages->clear();
    delete _deleted_messages;

    for (i=0, length=_msgs->length(); i<length; i++)
    {
	ms = _msgs->at(i);
        delete ms;
    }
    _msgs->clear();
    delete _msgs;

    delete _sorter;
}

Widget
MsgScrollingList::get_scrolling_list()
{
    return(_w);
}

void
MsgScrollingList::items(
    XmString items[],
    int count )
{
    XtVaSetValues( _w,
		   XmNitems, items,
		   XmNitemCount, count,
		   NULL );
}

#ifdef DEAD_WOOD
void 
MsgScrollingList::addChooseCommand(
    ChooseCmd *cmd
) 
{
    _choose=cmd;
}

void 
MsgScrollingList::addDeleteCommand(
    DeleteCmd *cmd
) 
{
    _delete=cmd;
}
#endif /* DEAD_WOOD */


void
MsgScrollingList::select_next_item()
{
    INSERT_STACK_PROBE
    int num_msgs = 0;
    FORCE_SEGV_DECL(MsgStruct, tmpMS);
    DtMailEnv mail_error;

    // Initialize the mail_error.
    mail_error.clear();

    XtVaGetValues( _w,
		   XmNitemCount, &num_msgs,
		   NULL );
    
    _selected_item_position = _displayed_item_position + 1;

    if (_selected_item_position <= num_msgs && 
	_selected_item_position > 0 ) {

	tmpMS = get_message_struct(_selected_item_position);
	if (tmpMS == NULL) {
	    return;
	}
	else {
	    // Deselect all items currently selected.
	    // display_and_select_message() will select, highlight
	    // and display the "next" message.

	    XmListDeselectAllItems(_w);

	    this->display_and_select_message(mail_error,
					tmpMS->message_handle);
	    if (mail_error.isSet()) {
		// Post an exception here...
	    }
	}
    }
    else {
	return;
    }
}

void
MsgScrollingList::select_prev_item()
{
    INSERT_STACK_PROBE
    int num_msgs;
    FORCE_SEGV_DECL(MsgStruct, tmpMS);
    DtMailEnv mail_error;

    // Initialize the mail_error.
    mail_error.clear();

    XtVaGetValues( _w,
		   XmNitemCount, &num_msgs,
		   NULL );
    
    if( _displayed_item_position != 1 )
	_selected_item_position = _displayed_item_position - 1 ;
    else
	_selected_item_position = _displayed_item_position ;
    
    if (_selected_item_position >= 1) {
	tmpMS = get_message_struct(_selected_item_position);
	if (tmpMS == NULL) {
	    return;
	}
	else {
	    // Deselect all items currently selected.
	    // display_and_select_message() will select, highlight
	    // and display the "previous" message.

	    XmListDeselectAllItems(_w);

	    this->display_and_select_message(mail_error,
					tmpMS->message_handle);
	    if (mail_error.isSet()) {
		// Post an exception here...
	    }
	}
    }
    else {
	return;
    }
}

DtMailMessageHandle
MsgScrollingList::msgno( 
			 int index 
		     ) 
{ 
    if (index <= 0) {
	return(NULL);
    }
    else {
	return _msgs->at(index-1)->message_handle; 
    }
}

MsgStruct*
MsgScrollingList::get_message_struct(
				     int index 
				 ) 
{ 
    if (index <= 0) {
	return(NULL);
    }
    else {
	return(_msgs->at(index-1));
    }
}

int 
MsgScrollingList::position( 
			    DtMailMessageHandle msgno 
			) 
{ 
    return (_msgs->indexof(msgno))+1; 
}

#ifdef DEAD_WOOD
int 
MsgScrollingList::position( 
			    MsgStruct *a_msg_struct
			) 
{ 
    return (_msgs->indexof(a_msg_struct))+1; 
}

void
MsgScrollingList::appendMsg(
    DtMailMessageHandle msg_hndl
)
{
    MsgStruct *newMS;

    // A new message has come in.
    // Increase the session_message_number which keeps track of the
    // number of messages in this session for this folder (scrolling list).
    // 
    
    newMS = new MsgStruct();
    newMS->message_handle = msg_hndl;
    newMS->indexNumber = session_message_number-num_deleted_messages;
    newMS->sessionNumber = session_message_number;
    newMS->is_deleted = FALSE;
    _msgs->append(newMS);
    session_message_number++;

}
#endif /* DEAD_WOOD */

    
void
MsgScrollingList::insertMsg(
    DtMailMessageHandle msg_hndl
)
{
    MsgStruct *newMS;

    // A new message has come in.
    // Increase the session_message_number which keeps track of the
    // number of messages in this session for this folder (scrolling list).
    // 

    newMS = new MsgStruct();
    newMS->message_handle = msg_hndl;
    newMS->indexNumber = session_message_number-num_deleted_messages;
    newMS->sessionNumber = session_message_number;
    newMS->is_deleted = FALSE;
    _msgs->append(newMS);

    session_message_number++;
}

void
MsgScrollingList::insertDeletedMsg(DtMailMessageHandle msg_hndl)
{
    MsgStruct *newMS;

    // A new message has come in.
    // Increase the session_message_number which keeps track of the
    // number of messages in this session for this folder (scrolling list).
    // 

    newMS = new MsgStruct();
    newMS->message_handle = msg_hndl;
    newMS->indexNumber = num_deleted_messages;
    newMS->sessionNumber = session_message_number;
    newMS->is_deleted = FALSE;
    _deleted_messages->append(newMS);

    session_message_number++;
    num_deleted_messages++;
}

int
MsgScrollingList::load_headers(
    DtMailEnv &mail_error
)
{

    DtMailMessageHandle tmpMH;
#ifdef undef
    XmString new_status, read_status;
#endif
    XmString complete_header; // text of header w/ glyphs
    int num_items = 0;
    int select_item;
    DtMailHeaderLine info;
    DtMail::MailBox * mbox = this->parent()->mailbox();
    DtMailEnv error;
    DtMailBoolean first_new = DTM_TRUE;

    // Create a class to collect the mail header XmStrings
    // then get all the items from the current list.
    _xmstr_collector = new XmStrCollector();

#ifdef undef
/* NL_COMMENT
 * In a mailer container window's message scrolling list, a "N" appears
 * to the left of a mail message header indicating that the mail message
 * is "new" (just arrived and not yet viewed by the user).
 * There is only space to display 1 character.  If "N" needs to be translated,
 * please make sure the translation is only 1 character.
 */
   new_status = XmStringCreateLocalized(GETMSG(DT_catd, 1, 110, "N"));
   read_status = XmStringCreateLocalized(" ");
#endif

    // Allocate memory for the XmString array and initialize it.

    int visible;
    XtVaGetValues(_w, XmNvisibleItemCount, &visible, NULL);

    // Retrieve the message_handles, and from them their headers.
    // Create an XmString and toss it into the XmStrCollector. 

    select_item = 0;
    int n_vis = 0;
    MsgStruct *ms;

    for (tmpMH = mbox->getFirstMessageSummary(error, _header_info, info);
	 tmpMH && !error.isSet();
	 tmpMH = mbox->getNextMessageSummary(error, tmpMH, _header_info, info),
	 num_items++) {

	DtMail::Message * msg = mbox->getMessage(error, tmpMH);
	if (msg->flagIsSet(error, DtMailMessageDeletePending) == DTM_TRUE) {
	    insertDeletedMsg(tmpMH);
	    continue;
	}
 	else {
	    insertMsg(tmpMH);
	    n_vis += 1;
	}

    	ms = get_message_struct(get_num_messages());
	complete_header = formatHeader(
				info,
				ms->indexNumber,
				show_with_attachments(msg),
				msg->flagIsSet(error, DtMailMessageNew));

 
	if (msg->flagIsSet(error, DtMailMessageNew) == DTM_TRUE) {
	    num_new_messages++;

	    // We want to select the last read message before the
	    // first new message.  We will select the first new
	    // message if it is the first message.

 	    if (first_new) {
 		first_new = DTM_FALSE;
 		if (num_items > 0)
 		    select_item = num_items - 1;
 		else
 		    select_item = 0;
 	    }
	}

        // Insert the XmString into the array.
	_xmstr_collector->AddItemToList (complete_header);

	// Free the space allocated for info
	// delete []info.header_values;
	mbox->clearMessageSummary(info);
    }

    // If there were no new messages, select and display the last message.
    if (first_new) {
 	select_item = num_items - 1;
     }

    select_item += 1; // Message slots start at 1.

    select_item -= num_deleted_messages; // List does not have deleted msgs.

    // Add the items to the XmList.
    // All XmStrings are freed in the XmStrCollector destructor.
    _xtarg_collector = new XtArgCollector;

    // The first time the headers are loaded, they should all be loaded
    // at the same time.  XtVaSetValues is used for this rather than
    // XmListAddItems so that all the other resource will be set at
    // the same time.  This prevents multiple repaints.
    //
    // However, in the case where only an item or two are being added,
    // XmListAddItems should be used.  This prevents an unnecessary
    // repaint in this case.
    XmListAddItems (_w, _xmstr_collector->GetItems(),
    	    _xmstr_collector->GetNumItems(), 0);

    display_message_summary();
    display_message(mail_error, select_item);

    _xtarg_collector->SetValues(_w);
    if (_selected_items)
    {
	XmStringFree (_selected_items);
	_selected_items = NULL;
    }

    delete _xtarg_collector;
    delete _xmstr_collector;
    _xtarg_collector = NULL;
    _xmstr_collector = NULL;

    return(num_items);
}

void
MsgScrollingList::load_headers(
    DtMailEnv &mail_error,
    DtMailMessageHandle last
)
{
    DtMailMessageHandle tmpMH;
#ifdef undef
    XmString read_status, new_status;
#endif
    XmString complete_header; // read status + attach + header_text.
    int num_items;
    int num_new = 0, num_vis = 0;
    DtMailHeaderLine info;
    DtMailEnv error;
    DtMail::MailBox * mbox = this->parent()->mailbox();


    // Create a class to collect the mail header XmStrings
    // then get all the items from the current list.
    _xmstr_collector = new XmStrCollector();

    mail_error.clear();

#ifdef undef
/* NL_COMMENT
 * In a mailer container window's message scrolling list, a "N" appears
 * to the left of a mail message header indicating that the mail message
 * is "new" (just arrived and not yet viewed by the user).
 * There is only space to display 1 character.  If "N" needs to be translated,
 * please make sure the translation is only 1 character.
 */
    new_status = XmStringCreateLocalized(GETMSG(DT_catd, 1, 111, "N"));
    read_status = XmStringCreateLocalized(" ");
#endif

    // Allocate memory for the XmString array and initialize it.

    XtVaGetValues(_w, XmNvisibleItemCount, &num_items, NULL);

    MsgStruct *ms;

    for (tmpMH = mbox->getNextMessageSummary(error, last, _header_info, info);
	 tmpMH && !error.isSet();
	 tmpMH = mbox->getNextMessageSummary(error, tmpMH, _header_info, info),
	 num_new++) {

	DtMail::Message * msg = mbox->getMessage(error, tmpMH);
	if (msg->flagIsSet(error, DtMailMessageDeletePending) == DTM_TRUE) {
	    insertDeletedMsg(tmpMH);
	    continue;
	}
	else {
	    insertMsg(tmpMH);
	    num_vis += 1;
	}

    	ms = get_message_struct(get_num_messages());
	complete_header = formatHeader(
				info,
				ms->indexNumber,
				show_with_attachments(msg),
				msg->flagIsSet(error, DtMailMessageNew));

	if (msg->flagIsSet(error, DtMailMessageNew) == DTM_TRUE) {
	    num_new_messages++;
	}

        // Insert the XmString into the array.
	_xmstr_collector->AddItemToList (complete_header);

	// Free the space allocated for info
	// delete []info.header_values;
	mbox->clearMessageSummary(info);
    }

    // Add the items to the XmList.
    // All XmStrings are freed in the XmStrCollector destructor.
    _xtarg_collector = new XtArgCollector;

    // The first time the headers are loaded, they should all be loaded
    // at the same time.  XtVaSetValues is used for this rather than
    // XmListAddItems so that all the other resource will be set at
    // the same time.  This prevents multiple repaints.
    //
    // However, in the case where only an item or two are being added,
    // XmListAddItems should be used.  This prevents an unnecessary
    // repaint in this case.
    XmListAddItems (_w, _xmstr_collector->GetItems(),
    	    _xmstr_collector->GetNumItems(), 0);

	do_list_vis_adjustment();

    display_message_summary();

    _xtarg_collector->SetValues(_w);
    if (_selected_items)
    {
	XmStringFree (_selected_items);
	_selected_items = NULL;
    }

    delete _xtarg_collector;
    delete _xmstr_collector;
    _xtarg_collector = NULL;
    _xmstr_collector = NULL;
}

void MsgScrollingList::do_list_vis_adjustment()
{
    Widget list = _w; 
    int numNew = _xmstr_collector->GetNumItems();
    int focItm = _selected_item_position;

    int numItems;
    int cItmCnt, pItmCnt, sItmCnt;
    int cFocItm, cTopItm, cBotItm, cInvItm;
 
    XtVaGetValues(list, XmNvisibleItemCount, &numItems, NULL);
    XtVaGetValues(list, XmNitemCount, &cItmCnt, NULL);
    XtVaGetValues(list, XmNselectedItemCount, &sItmCnt, NULL);
    XtVaGetValues(list, XmNtopItemPosition, &cTopItm, NULL);
    pItmCnt = cItmCnt - numNew;
 
    cBotItm = cTopItm + numItems - 1; //[cTopItm...cBotItm is our window
 
    //[User has chosen to view some messages and that view needs to be
    //[maintained.
    if (cBotItm != pItmCnt)
        return;
 
    //[If Brand New Mailbox
    if (cItmCnt <= numItems)
        return;
 
    //[If no items selected, simply synch right till bottom
    if (sItmCnt == 0) {
        XmListSetBottomPos(list, cItmCnt);
        return;
    }
 
    // assert(cFocItm != -1);

    cFocItm = focItm;      
    cInvItm = pItmCnt - cBotItm; //[Num below bottom-most, or hidden
 
    if ((cFocItm <= cBotItm) && (cFocItm >= cTopItm)) {
        int winM = cFocItm - cTopItm - cInvItm;
        if (winM <= 0) { //[There is no scope of adjustment
            return;
        }
        //[All the new messages can be accomodated w/o scrolling curr selection
        if (numNew <= winM) {
            XmListSetBottomPos(list, cItmCnt);
            return;
        }
        //[All the new messages cannot be accomodated, but we will do best fit
        else {
            int numNotShow = numNew - winM;
            XmListSetBottomPos(list, cItmCnt - numNotShow);
            return;
        }
    }
    else {
        XmListSetBottomPos(list, cItmCnt);
        return;
    }
}

void 
MsgScrollingList::deleteSelected(Boolean silent)
{
    // SR - Added stuff below.  Made code more efficient also.
    FORCE_SEGV_DECL(MsgStruct, a_del_msg_struct);
    FORCE_SEGV_DECL(MsgStruct, tmpMS);
    FORCE_SEGV_DECL(ViewMsgDialog, tmpView);
    DtMailMessageHandle tmpMH;
    int  position_in_list, i;
    FORCE_SEGV_DECL(int, position_list);
    int position_count;
    Boolean any_selected;
    int num_msgs;
    UndelFromListDialog *undel_dialog;
    int first_selected_pos;
    DtMailEnv mail_error, error;
    DtMailBoolean	cur_state;
    char *status_message, *str;

    // Initialize the mail_error.
    mail_error.clear();

    XtVaGetValues( _w, XmNitemCount, &num_msgs, NULL );
    any_selected = XmListGetSelectedPos(_w, &position_list, &position_count);
    if (!any_selected) return;


    // Of the items in the list, potentially many could be selected
    // for delete.  And those selected for deleted need not be 
    // contiguous necessarily.  Note the position of the item that 
    // appears first in the list.  When the items are deleted, other
    // items if any exist will shift up to take the places of the 
    // deleted items.  You want to display the corresponding item that
    // takes the "first position".

    // Say, if you have messages A, B, C, D, Evisible in that order.
    // You select B and C for delete.
    // The first selected pos is 2.
    // When B and C are deleted, D (and E...) move up.
    // The new order is A, D, E...
    // You want to display the second message which is D.

    first_selected_pos = *position_list;
    undel_dialog = parent()->get_undel_dialog();

    for (i=0; i < position_count; i++ )
    {
	position_in_list = *(position_list + i);
	a_del_msg_struct = get_message_struct(position_in_list);
	_msgs->mark_for_delete(position_in_list - 1);
	_deleted_messages->append(a_del_msg_struct);
	if (undel_dialog)
	  undel_dialog->insertMsg(mail_error, a_del_msg_struct);
	if (mail_error.isSet()) parent()->postErrorDialog(mail_error);

	// See if there is a standalone view of the message.
	// If there is, quit it.

	tmpMH = a_del_msg_struct->message_handle;
	tmpView = parent()->ifViewExists(tmpMH);
	if (tmpView) tmpView->quit();

	// Update the status of the message in the persistent store.
	//
	DtMail::Message * msg = _parent->mailbox()->getMessage(
					error, 
					a_del_msg_struct->message_handle);
	msg->setFlag(error, DtMailMessageDeletePending);

	// Check if message is new.  If new and it is being deleted,
	// reduce the new messages count.

	cur_state = msg->flagIsSet(error, DtMailMessageNew);

	if (cur_state == DTM_TRUE) num_new_messages--;
    }

    _msgs->compact(0);

    // Delete the items from the scrolling list

     XmListDeletePositions(_w, position_list, position_count);
    num_deleted_messages += position_count;

    // XnListGetSelectedPos allocated memory for the array and
    // wants us to free it.  See documentation.

    num_msgs = _msgs->length();

    // Always remove the current attachments from the attachment area
    // regardless of whether this is the last message in the list or not
    // We call clearAttachArea if there are any other messages in the folder
    // as parseAttachments will be called which will clean up the attachment
    // status window. In the case of deleting the last message we must call
    // removeCurrentAttachments which cleans up the status window as well.
    //
    if (num_msgs == 0)
      parent()->get_editor()->attachArea()->removeCurrentAttachments();
    else
      parent()->get_editor()->attachArea()->clearAttachArea();

    if (num_msgs == 0)
    {
	//No more messages left; clear the editor.
	parent()->get_editor()->textEditor()->set_contents("", 1);

	// NOTE: Need to obtain the attachArea and clear it up too

	_displayed_item_position = 0;	// Not displaying any.
	_selected_item_position = 0;	// Reset...
	free(position_list);
	display_message_summary();
	return;
    }

    // If there are messages 1, 2 and 3 and 2 is selected and deleted
    // first_selected_pos will be 2;
    // after removing it, will need to display the 2nd message
    // (which will now be 3)
    // If 2 and 3 have been deleted, and there is no 2nd message,
    // display the nth message (in our example, n = 1)
    
    _selected_item_position = first_selected_pos;
    if (_selected_item_position > num_msgs)
      _selected_item_position = num_msgs; 

    // Having determined which message to display, confirm that it is
    // within the bounds.  Call display_message().
    // FYI, display_message() sets the _displayed_item_position.

    if ((_selected_item_position > 0) &&
	(_selected_item_position <= num_msgs))
    {
	XmListSelectPos(_w, _selected_item_position, FALSE);
	tmpMS = this->get_message_struct(_selected_item_position);
	if (tmpMS == NULL)
	{
	    free(position_list);
	    return;
	}
	else
	{
	    this->display_message(mail_error, tmpMS->message_handle);
	    if (mail_error.isSet()) parent()->postErrorDialog(mail_error);
	}
    }

    if (!silent)
    {
	if (position_count > 1)
	{
	    /* NL_COMMENT
	    * The following sentence means %d number of mail messages have 
	    * been deleted from the mail folder.  This is the plural form 
	    * of the message that gets printed if more than one message 
	    * is moved.
	    */
	    str = GETMSG(DT_catd, 3, 84, "%d messages deleted"); 
	}
	else
	{
	    /* NL_COMMENT
	    * The following sentence means %d number of mail messages have 
	    * been deleted from the mail folder.  This is the singular 
	    * form of the message that gets printed if only one message 
	    * is moved.
	    */
	    str = GETMSG(DT_catd, 3, 85, "%d message deleted"); 
	}
	status_message = new char[strlen(str) + 10];
	sprintf(status_message, str, position_count);
	parent()->message(status_message);
	delete [] status_message;
    }

    updateListItems(-1, TRUE, NULL);
    display_message_summary();

    XtFree((char*) position_list);
}

DtMailBoolean
copyCallback(
    DtMailCallbackOp,
    const char *,
    const char *,
    void *,
    ...
)
{
    return(DTM_FALSE);
}

// copySelected() will either copy or move the selected messages
// into the container called destname.  If the delete_after_copy
// flag is set to TRUE, it is effectively a move; otherwise, it
// is a copy.  If the container named by destname is a relative
// path, but it isn't prefixed with a '+', then a '+' will be
// prepended to the name so that the open() call will "do the
// right thing."
// If silent is TRUE then no status messages are displayed and the
// destname is not added to the copy/move cache.

int
MsgScrollingList::copySelected(
    DtMailEnv &mail_error,
    char *destname, 
    int delete_after_copy,
    int silent
)
{
    FORCE_SEGV_DECL(DtMail::MailBox, mbox);
    FORCE_SEGV_DECL(int, position_list);
    FORCE_SEGV_DECL(DtMail::MailBox, target);
    DtMail::MailRc * mailrc;
    DtMail::Session * d_session = theRoamApp.session()->session();
    int position_count, position, i;
    Boolean any_selected = FALSE;
    DtMailMessageHandle msg;
    char *status_message, *str;
    char *newdestname;
    RoamMenuWindow	*rmw;


    any_selected = XmListGetSelectedPos(_w,
					&position_list,
					&position_count);
    // If there aren't any selected messages, then there isn't
    // anything for us to do.

    if (!any_selected) {
	display_no_message();
	if (! silent)
	{
            char * helpId = NULL;
	    DtMailGenDialog *dialog = _parent->genDialog();

	    dialog->setToErrorDialog(
				GETMSG(DT_catd, 3, 50, "Mailer"),
            			GETMSG(DT_catd, 2, 16, "No message selected."));
	    dialog->post_and_return(helpId);
	}
	return(1);
    }

    mbox = parent()->mailbox();
    mailrc = mbox->session()->mailRc(mail_error);
    if (!mailrc) {
    // NL_COMMENT
    // The following is an error message.  "mailrc" is the name of the
    // mail resource file.  Translate as appropriate.
    //
    parent()->message(GETMSG(DT_catd, 2, 15,"Error - Unable to get mailrc."));
    return(1);
    }

    // If the first character of destname is alphanumeric, we can
    // safely assume that it is a relative path, so we prepend a
    // '+' to it.
    if (isalnum(destname[0])) {
	// Make sure we allocate enough for the name + '+' + null terminator.
	newdestname = (char *) malloc(strlen(destname) + 2);
	memset(newdestname, 0, strlen(destname) + 2);
 	sprintf(newdestname, "+%s", destname);
        char *path = d_session->expandPath(mail_error, newdestname);
	target = theRoamApp.session()->open(mail_error,
					path,
					copyCallback,
					NULL,
					DTM_TRUE,  
					DTM_FALSE,
					DTM_FALSE);
	free(newdestname);
	free(path);
	newdestname = NULL;
    } else {
	target = theRoamApp.session()->open(mail_error,
					destname,
					copyCallback,
					NULL,
					DTM_TRUE,  
					DTM_FALSE,
					DTM_FALSE);
    }
    if (mail_error.isSet()) {
        // if the error is DTME_AlreadyOpened, we don't care.
        // go ahead to clear the error
        // otherwise, 
	// We couldn't open the container, so we want to post an
	// error dialog.
        if ((DTMailError_t)mail_error == DTME_AlreadyOpened)
         {
           mail_error.clear();
         } else {
	  parent()->postErrorDialog(mail_error);
	  return(0);
         }
    }

    // Go through the selected messages and copy them to the
    // specified container.

    if ( position_count > 0 )
        parent()->busyCursor() ;

    for (i=0; i < position_count; i++) {
	position = *(position_list + i);
	msg = msgno(position);
	DtMail::Message * dtmsg = mbox->getMessage(mail_error, msg);
	if (mail_error.isSet()) {
            parent()->normalCursor() ;
	    parent()->postErrorDialog(mail_error);
	    theRoamApp.session()->close(mail_error, target);
	    return(0);
	}

	mail_error.clear();
	target->copyMessage(mail_error, dtmsg);
	if (mail_error.isSet()) {
            parent()->normalCursor() ;
	    parent()->postErrorDialog(mail_error);
	    theRoamApp.session()->close(mail_error, target);
	    return(0);
	}

        parent()->normalCursor() ;
        
    }

    
    rmw = theRoamApp.session()->getRMW(target);
    mail_error.clear();
    if (rmw) rmw->checkForMail(mail_error);

    if (delete_after_copy) {
	deleteSelected();

	if (i > 1) {
	    // NL_COMMENT
	    // The following sentence means %d number of mail messages have 
	    // been moved to the %s mail folder.  The %s is the name of a 
	    // mail folder.  This is the plural form of the message that gets
	    // printed if more than one message is moved.
	    //
	    str = GETMSG(DT_catd, 3, 65, "%d messages moved to %s"); 
	} else {
	    // NL_COMMENT
	    // The following sentence means %d number of mail messages have 
	    // been moved to the %s mail folder.  The %s is the name of a 
	    // mail folder.  This is the singular form of the message that
	    // gets printed if only one message is moved.
	    //
	    str = GETMSG(DT_catd, 3, 66, "%d message moved to %s"); 
	}

    } else {
	if (i > 1) {
	    // NL_COMMENT
	    // The following sentence means %d number of mail messages have been
	    // copied to the %s mail folder.  This is the plural form of the
	    // message that gets printed if more than one message is copied.
	    //
	    str = GETMSG(DT_catd, 3, 67, "%d messages copied to %s"); 
	} else {
	    // NL_COMMENT
	    // The following sentence means %d number of mail messages have been
	    // copied to the %s mail folder.  This is the singular form of the
	    // message that gets printed if only one message is copied.
	    //
	    str = GETMSG(DT_catd, 3, 68, "%d message copied to %s"); 
	}

    }
    newdestname = d_session->getRelativePath(mail_error, destname);
    status_message = new char[strlen(str) + strlen(newdestname) + 10];
    sprintf(status_message, str, i, newdestname);

    if (!silent) {
    	theRoamApp.globalAddToCachedContainerList(newdestname);
    	parent()->message(status_message);
    }

    free(newdestname);

    theRoamApp.session()->close(mail_error, target);

    delete [] status_message;
    return(0);
}


//-----------------------------------------------------------------------------
// This method returns a list of the currently selected messages.  This list
// must be deleted by the calling method.
//-----------------------------------------------------------------------------

MsgHndArray * 
MsgScrollingList::selected()
{
    FORCE_SEGV_DECL(MsgStruct, a_msg_struct);
    FORCE_SEGV_DECL(int, position_list);
    int  position_in_list, i;
    int position_count;
    Boolean any_selected;

    // Find out first if any have been selected.
    // If i has been selected, how many?
    // We need the number selected so that we can allocate 
    // space for that many mesasgeStructs to be returned.

    any_selected = XmListGetSelectedPos(_w,
 					&position_list, 
 					&position_count);

    // If nothing selected, return

    if (!any_selected) return NULL;

    //  Allocate memory for position_count number of messageStructs
    // in MsgHndArray.

    MsgHndArray *msgList = new MsgHndArray(position_count);

    for (i=0; i < position_count; i++ ) {
	position_in_list = *(position_list + i);
	a_msg_struct = get_message_struct(position_in_list);
	msgList->append(a_msg_struct);
    }
    return msgList;
}

DtMailBoolean
MsgScrollingList::show_with_attachments(DtMailMessageHandle msg_num)
{
    DtMailBoolean	has_attachments = DTM_FALSE;
    DtMailEnv		mail_error;
    DtMail::MailBox	*mbox=parent()->mailbox();
    DtMail::Message	*msg = mbox->getMessage(mail_error, msg_num);

    has_attachments = show_with_attachments(msg);
    return has_attachments;
}


DtMailBoolean
MsgScrollingList::show_with_attachments(DtMail::Message * msg)
{
    DtMail::BodyPart	*bp;
    DtMailEnv		error;
    DtMailBoolean	has_attachments = DTM_FALSE;
    DtMailBoolean	is_multipart = DTM_FALSE;
    int			num_bodyParts;
    char		*type;

    is_multipart = msg->flagIsSet(error, DtMailMessageMultipart);
    if (! is_multipart)
      return DTM_FALSE;

    num_bodyParts = msg->getBodyCount(error);
    if (num_bodyParts > 1)
      return DTM_TRUE;

    bp = msg->getFirstBodyPart(error);
    bp->getContents(error, NULL, NULL, &type, NULL, NULL, NULL);
    if (NULL != type)
    {
	char *attr;

	attr = DtDtsDataTypeToAttributeValue(type, DtDTS_DA_IS_TEXT, NULL);
	if (attr && strcasecmp(attr, "true") != 0)
	  has_attachments = DTM_TRUE;
	
	if (type)
	  free(type);
	if (attr)
	  DtDtsFreeAttributeValue(attr);
    }
    return has_attachments;
}


void
MsgScrollingList::display_message(
    DtMailEnv	&mail_error,
    DtMailMessageHandle   msg_num
)
{
    int			item_index;
    DtMail::MailBox	*mbox=parent()->mailbox();
    Editor*		rmw_editor;
    DtMailBoolean	cur_state;
    int num_selected;
    int num_bodyParts;

    // If there is a status message displayed, clear it first.
    parent()->clear_message();

    
	// We could have called display_msg from anywhere.
	// Need to calculate what is the position of that item
	// in the scrolling list, given the DtMailMessageHandle.
	// Determine the index at the _msgs array; increment by 1
	// since array begins at 0 and XmList begins at 1.

    item_index = _msgs->indexof(msg_num);
    if (item_index < 0) return;
	
    _displayed_item_position = item_index + 1;

    // Make sure the header is visible in the scrolling list.
    this->scroll_to_position(_displayed_item_position);

    // Retrieve the header text and insert it. We need to retrieve
    // message from the handle and inset the headers.
    //

    DtMail::Message * msg = mbox->getMessage(mail_error, msg_num);
    DtMail::Envelope * env = msg->getEnvelope(mail_error);


    // There are multiple paths to this place:
    // 1) user has already read this message but is re-reading it;
    // 2) user has undeleted this message (implicitly, they have
    //    read this message);
    // 3) user has not read this message yet.
    //
    // For (1), we don't need to do anything fancy.
    // For (2), we have already reset the IsDeleted flag while 
    // undeleting.
    // For (3), we need to reset the flag in store to indicate
    // its read.

    cur_state = msg->flagIsSet(mail_error, DtMailMessageNew);

    // If the message was previously new, we need to reset the list
    // item to remove the "N" on the item.  We do this by reconstructing
    // the header line without the "N".
    //
    if (cur_state == DTM_TRUE) {
	DtMailHeaderLine  info;

	msg->resetFlag(mail_error, DtMailMessageNew);

	mbox->getMessageSummary(mail_error, msg_num, _header_info, info);

        MsgStruct *ms;
    	ms = get_message_struct(_displayed_item_position);

	XmString complete_header;
	complete_header = formatHeader(
			    info,
			    ms->indexNumber,
			    show_with_attachments(msg),
			    msg->flagIsSet(mail_error, DtMailMessageNew));

	mbox->clearMessageSummary(info);

	XmListReplaceItemsPos(_w, &complete_header,1, _displayed_item_position);
	XmStringFree(complete_header);

	// The default selection policy is extended_select.
	// Problem:
	// User extend selects multiple items
	// The last item selected is a "N" message
	// We display the "N" message and have to replace it without
	// the "N".
	// There is no way to replace with something else and select at 
	// the same time.  
	// Therefre, we need to explicitly select the replacement.
	// The problem is: selecting the replacement deselects the other
	// selected items.
	// To work around this, we switch temporarily to MULTIPLE_SELECT,
	// select the replacement, and switch back to extend_select.
	// This will select the replacement and *not*  drop the
	// selection on the other selected items.
	// Say "Amen" to  OSF for such convoluted thinking!

	XtVaGetValues(_w,
	    XmNselectedItemCount, &num_selected,
	    NULL);

	if (num_selected > 1) {
	    // Change to MULTIPLE_SELECT.
	    // Select item
	    // Change back to EXTEND_SELECT
	    XtVaSetValues (_w,
		XmNselectionPolicy, XmMULTIPLE_SELECT,
		XmNselectionMode,   XmNORMAL_MODE,
		NULL);
	  
	   // When loading mail headers, we need to make sure that
	   // the XmList resources are all set at the same time to
	   // avoid painting multiple times.  So here we collect
	   // these resources.
	   if (_xtarg_collector && _xmstr_collector)
	   {
		XmString *items = _xmstr_collector->GetItems();
	
		// Keep a handle to the malloced string copy so that
		// we can free it after the XtSetValues
		_selected_items 
		    = XmStringCopy (items[_displayed_item_position - 1]);

		_xtarg_collector->AddItemToList (XmNselectedItems,
		    (XtArgVal) &_selected_items);
		_xtarg_collector->AddItemToList (
		    XmNselectedItemCount, 1);
		_xtarg_collector->AddItemToList (
		    XmNselectionPolicy, XmEXTENDED_SELECT);
	    }
	    else
	    {
	        XmListSelectPos(_w, _displayed_item_position, FALSE);

	        XtVaSetValues (_w,
		    XmNselectionPolicy, XmEXTENDED_SELECT,
		    XmNselectionMode,   XmNORMAL_MODE,
		    NULL);
	    }
	}
	else {
	   if (_xtarg_collector && _xmstr_collector)
	   {
		XmString *items = _xmstr_collector->GetItems();

		_selected_items 
		    = XmStringCopy (items[_displayed_item_position - 1]);

		_xtarg_collector->AddItemToList (XmNselectedItems,
		    (XtArgVal) &_selected_items);
		_xtarg_collector->AddItemToList (
		    XmNselectedItemCount, 1);
	    }
	    else
	    {
	        XmListSelectPos(_w, _displayed_item_position, FALSE);
	    }
	}

	num_new_messages--;
    }

    display_message_summary();

    // Display the message now.
    rmw_editor = parent()->get_editor()->textEditor();
    rmw_editor->disable_redisplay();
    rmw_editor->auto_show_cursor_off();
    rmw_editor->clear_contents();

    num_bodyParts = msg->getBodyCount(mail_error);

    char * status_string;
    DtMailBoolean firstBPHandled;

    // Turn on the busy Cursor

    parent()->busyCursor();

    if (parent()->fullHeader())
      firstBPHandled =
	rmw_editor->set_message(msg, &status_string, Editor::HF_FULL);
    else
      firstBPHandled =
	rmw_editor->set_message(msg, &status_string, Editor::HF_ABBREV);

    if (status_string) parent()->message(status_string);
    if (mail_error.isSet()) {} // do something

    if ((num_bodyParts > 1) || (!firstBPHandled))
    {
	// If the message has attachments, then let the attach pane
	// handle attachments but not the first bodyPart (which has
	// already been handled here).

	if (firstBPHandled) {

	    //  The first bodyPart has already been handled.
	    // The others, beginning from the second, need to be parsed 
	    // and put into the attachPane.

	    parent()->get_editor()->attachArea()->parseAttachments(
 					mail_error,
 					msg, 
					TRUE,
 					2);
	}
	else {
	    // The first bodyPart was not handled.
	    // It may not have been of type text.
	    // The attachment pane needs to handle all the bodyParts
	    // beginning with the first.

	    parent()->get_editor()->attachArea()->parseAttachments(
 					mail_error,
 					msg, 
					TRUE,
 					1);
	}

	// Check for errors.
	// Manage the attach area to display attachments.
	if (mail_error.isSet()) {} // do something

	parent()->get_editor()->manageAttachArea();
	parent()->activate_default_attach_menu();
    }
    else
    {
	parent()->deactivate_default_attach_menu();
	parent()->get_editor()->unmanageAttachArea();
    }

    parent()->sync_work_area_size();
    rmw_editor->auto_show_cursor_restore();

    // Turn on text editor and manage attachPane
    rmw_editor->set_to_top();
    rmw_editor->enable_redisplay();

    parent()->normalCursor();
}

void
MsgScrollingList::display_and_select_message(
    DtMailEnv &mail_error,
    DtMailMessageHandle msg_num
)
{
    int		item_index;

    // Need to calculate what is the position of that item
    // in the scrolling list, given the DtMailMessageHandle.
    // Determine the index at the _msgs array; increment by 1
    // since array begins at 0 and XmList begins at 1.

    mail_error.clear();

    item_index = _msgs->indexof(msg_num);
    if (item_index < 0) return;
	
    _displayed_item_position = item_index + 1;
    _selected_item_position = _displayed_item_position;
    
    // Select this message in the scrolling list and display
    // the message.

   // When loading mail headers, we need to make sure that
   // the XmList resources are all set at the same time to
   // avoid painting multiple times.  So here we collect
   // these resources.
   if (_xtarg_collector && _xmstr_collector)
   {
	XmString *items = _xmstr_collector->GetItems();

        // Keep a handle to the malloced string copy so that
        // we can free it after the XtSetValues
	_selected_items 
	    = XmStringCopy (items[_displayed_item_position - 1]);

	_xtarg_collector->AddItemToList (XmNselectedItems,
	    (XtArgVal) &_selected_items);
	_xtarg_collector->AddItemToList (
	    XmNselectedItemCount, 1);
    }
    else
    {
	XmListSelectPos(_w, _displayed_item_position, FALSE);
    }
    this->display_message(mail_error, msg_num);

    return;
}

void
MsgScrollingList::select_all_and_display_last(
    DtMailEnv         & error
)
 
{
    register int                item_pos;
    int				num_items;
    MsgHndArray			* msgHandles = get_messages();
 
    error.clear();

    if (this->get_num_messages() == 0) return;

    // If no message selected, return.

    if (this->get_selected_item() == 0) return;

    XtVaSetValues (_w, XmNselectionPolicy, XmMULTIPLE_SELECT, NULL);
    //
    // We have to go to the end of the list and go backwards.
    // We display the last one, and select the rest.
    //
    // A NULL terminated list.
    //
 
    XmListDeselectAllItems(baseWidget());
    XtVaGetValues(baseWidget(), XmNitemCount, &num_items, NULL);
 
    for (item_pos = 1; item_pos < num_items; item_pos++) {
        XmListSelectPos(baseWidget(), item_pos, FALSE);
    }
 
    // Invoke the selection callback on the last item.
    display_and_select_message(error, 
				msgHandles->at(item_pos-1)->message_handle);
    XtVaSetValues (_w, XmNselectionPolicy, XmEXTENDED_SELECT, NULL);
    XtVaSetValues (_w, XmNselectionMode, XmNORMAL_MODE, NULL);
}

void
MsgScrollingList::select_all_and_display_last(
    DtMailEnv 	& error,
    DtMailMessageHandle *handleArray,
    unsigned int	   elements
)
{
  register int		handleOffset = 0;
  register int		item_pos;

  error.clear();

  XtVaSetValues (_w, XmNselectionPolicy, XmMULTIPLE_SELECT, NULL);
  //
  // We have to go to the end of the list and go backwards.
  // We display the last one, and select the rest.
  //
  // A NULL terminated list.
  //
  handleOffset = elements;

  XmListDeselectAllItems(baseWidget());

  while (--handleOffset >= 0 && error.isNotSet()) {

    item_pos = _msgs->indexof(handleArray[handleOffset]);	// Get position
    if (item_pos < 0) {
      continue;
    }
    
    //
    // Select this message in the scrolling list and IF
    // it is the last message, display it.
    //
    if (handleOffset == elements - 1) {
      display_and_select_message(error, handleArray[handleOffset]);
    } else {
      XmListSelectPos(_w, item_pos + 1, FALSE);
    }
  }
  XtVaSetValues (_w, XmNselectionPolicy, XmEXTENDED_SELECT, NULL);
  XtVaSetValues (_w, XmNselectionMode, XmNORMAL_MODE, NULL);
  return;
}


void
MsgScrollingList::display_no_message()
{

	/* NL_COMMENT
	 * No mail message has been selected by the user.
	 */

    parent()->message(GETMSG(DT_catd, 2, 16, "No message selected."));
    _displayed_item_position = 0;
    _selected_item_position = 0;

    parent()->get_editor()->textEditor()->clear_contents();
    parent()->get_editor()->attachArea()->removeCurrentAttachments();
}

// In need of a simple but useful optimization here.
// By the time we get to this method, we already have displayed
// the selected message in the lower section of the combo window.
// Instead of having to parse the messageHandle and construct the
// text buffer to display, we can just get the parsed-and-formatted
// text from the lower section of the combo window and stick it into
// the VMD's editor.
// This will save us the effort of parsing and formatting a message
// 

void 
MsgScrollingList::viewInSeparateWindow(DtMailEnv &mail_error)
{
    FORCE_SEGV_DECL(const char, title);
    FORCE_SEGV_DECL(char, header_txt);
    FORCE_SEGV_DECL(MsgStruct, tmpMS);
    DtMailMessageHandle  msgHandle;
    DtMail::MailBox	*mbox=parent()->mailbox();
    ViewMsgDialog	*newview;
    int			 num_bodyParts;
    Editor*		 vmd_editor;

    DtMailEnv		 error;
    DtMail::Session	*d_session = theRoamApp.session()->session();
    DtMail::MailRc	*mail_rc = d_session->mailRc(error);
    const char		*value = NULL;  
 
    // If no message selected, return.
    if (this->get_selected_item() <= 0) return;

    // Double-check.  If none selected, return
    MsgHndArray *selected_msgs = this->selected();
    if (!selected_msgs) return;

    for (int a_msg_num = 0; a_msg_num < selected_msgs->length(); a_msg_num++)
    {
	tmpMS = selected_msgs->at(a_msg_num);
	if (tmpMS == NULL) return;
	   
	msgHandle = tmpMS->message_handle;
	newview = parent()->ifViewExists(msgHandle);
	   
	if (newview != NULL)
	{
            /* NL_COMMENT
	     * The current mail message selected is already displayed in a
	     * separate window.  Therefore this 'separate' window will be
	     * raised in front of existing windows so the user can see it.
	     */
	    parent()->message(
			GETMSG(
				DT_catd, 3, 69,
				"View already exists.  Raising it."));
	    newview->displayInCurrentWorkspace();
	    return;
	} 

	// No view exists.  Display it.  For feedback, set busyCursor().
	parent()->busyCursor();
        mail_rc->getValue(error, "separatemessageviewer", &value);
	if (error.isSet())
	  newview = new ViewMsgDialog(parent(), xmDialogShellWidgetClass);
	else
	{
	    if (value) free((void*) value);
	    newview = new ViewMsgDialog(parent(), topLevelShellWidgetClass);
	}
	parent()->registerDialog(newview);
	newview->initialize();
	vmd_editor = newview->get_editor()->textEditor();
	   
	// Set the VMD's msgHandle. This unique handle is what is
	// used to raise the VMD when the same message is double
	// clicked on later.
	newview->msgno(msgHandle);
	   
	DtMailEnv error;
	DtMail::Message * msg = mbox->getMessage(error, msgHandle);
	DtMail::Envelope * env = msg->getEnvelope(error);
	   
	DtMailValueSeq title_value;
	env->getHeader(error, DtMailMessageSubject, DTM_TRUE, title_value);
	if (error.isSet())
	  title = "NO SUBJECT!";
	else
	  title = *(title_value[0]);
	   
	newview->title((char *)title);
	newview->auto_show_cursor_off();
	   
	// Unset the error produced when obtaining title...
	mail_error.clear();
	vmd_editor->disable_redisplay();
	   
	char * status_string;
	DtMailBoolean firstBPHandled;
	   
	if (parent()->fullHeader())
	  firstBPHandled = newview->get_editor()->textEditor()->set_message(
					msg, &status_string, Editor::HF_FULL);
	else
	  firstBPHandled = newview->get_editor()->textEditor()->set_message(
					msg, &status_string, Editor::HF_ABBREV);
	   
	// If the message has attachments, then let the attach pane
	// handle attachments but not the first bodyPart (which has
	// already been handled here).
	num_bodyParts = msg->getBodyCount(mail_error);
	if (mail_error.isSet()) {} // do something
	   
	if ((num_bodyParts == 1) && firstBPHandled)
	{
	    newview->get_editor()->unmanageAttachArea();
	    newview->deactivate_default_attach_menu();
	}
	else if ((num_bodyParts > 1) || (!firstBPHandled))
	{
	    // If the message has attachments, then let the attach pane
	    // handle attachments but not the first bodyPart (which has
	    // already been handled here).
	       
	    if (firstBPHandled)
	    {
	        //  The first bodyPart has already been handled.
		// The others, beginning from the second, need to be parsed 
		// and put into the attachPane.
	        newview->get_editor()->attachArea()->parseAttachments(
		       				mail_error, msg, TRUE, 2);
	    }
	    else
	    {
		// The first bodyPart was not handled.
		// It may not have been of type text.
		// The attachment pane needs to handle all the bodyParts
		// beginning with the first.
		newview->get_editor()->attachArea()->parseAttachments(
		       				mail_error, msg, TRUE, 1);
	    }
	       
	    // Check for errors.
	    // Manage the attach area to display attachments.
	    if (mail_error.isSet()) {} // do something
	    newview->get_editor()->manageAttachArea();
	    newview->activate_default_attach_menu();
	}
	   
	newview->set_to_top();
	newview->auto_show_cursor_restore();
	vmd_editor->enable_redisplay();
	newview->manage();
    }

    parent()->normalCursor();
}

// defaultAction() gets called *after* extendedSelectionCallback() is
// called.  
// Display the message in extendedSelectionCallback().
// viewInSeparateWindow() in defaultAction().

void 
MsgScrollingList::defaultAction(Widget, XtPointer, XmListCallbackStruct *cbs)
{
    FORCE_SEGV_DECL(MsgStruct, tmpMS);
    DtMailEnv mail_error;

    // Initialize the mail_error.
    mail_error.clear();
   _selected_item_position = cbs->item_position;

   tmpMS = get_message_struct(_selected_item_position);
   if (tmpMS == NULL)
     return;
   else
   {
       this->viewInSeparateWindow(mail_error);
       if (mail_error.isSet()) {} // Post dialog indicating error
   }
}

    
void
MsgScrollingList::extendedSelectionCallback(
    Widget ,			// w
    XtPointer clientData,
    XmListCallbackStruct *cbs
)
{
    int last_clicked_on_pos;
    int above_selected_pos;
    int tmp_selected_pos;
    int i, num_selected;
    DtMailEnv mail_error;

    // Initialize the mail_error.
    mail_error.clear();

    
    Boolean IS_SELECTION = FALSE;
    Boolean SELECTION_MADE = FALSE;

    MsgScrollingList *obj=(MsgScrollingList *) clientData;

    // If all items have been deselected
    if (cbs->selected_item_count == 0) {
	obj->extended_selection(mail_error, 0);
	if (mail_error.isSet()) {
	    return;
	}
	SELECTION_MADE = TRUE;
    }
    else {
	
	last_clicked_on_pos = cbs->item_position;

    // Check to see if this was a selection or deselection
    // We do that by seeing if last_clicked_on_pos is in the 
    // selected_item_positions array.  If it is, then it is a 
    // selection; if its not there, then its a deselection.

	num_selected =  cbs->selected_item_count;
    
	for (i = 0; i < num_selected; i++) {
	    if (last_clicked_on_pos == cbs->selected_item_positions[i]) {
		// Yes, it was a selection
	    
		IS_SELECTION = TRUE;
		obj->extended_selection(mail_error, last_clicked_on_pos);
		if (mail_error.isSet()) {
		    return;
		}

		SELECTION_MADE = TRUE;
	    }
	}

    // If it was not a selection, then we need to find the selected
    // item nearest to the deselected item.  We have a choice there -
    // we can find the nearest one above, or the nearest one below 
    // the deselected item.  Let's find the nearest one above.
    // We do that by cruising through the selected_item_positions array.
    // Motif arranges the array in ascending order, no matter what order
    // you selected the items!  If you deselect an item, the items on
    // either side of it are the ones above and below it.  We use that
    // ordering to now select and display the one above it.
    //

	if (!IS_SELECTION) {

	// If the first selected item is below the deselected item,
	// it follows that all selected items are below the deselected
	// item.  Display the first selected item and be done.

	    if (cbs->selected_item_positions[0] > last_clicked_on_pos) {
		obj->extended_selection(mail_error,
					cbs->selected_item_positions[0]);
		if (mail_error.isSet()) {
		    return;
		}
		SELECTION_MADE = TRUE;
	    } 
	    // If the last selected item is above the deselected item,
	    // it follows that all selected items are above the deselected
	    // item.  Display the last selected item and be done.
	    else if (cbs->selected_item_positions[num_selected - 1] <
				last_clicked_on_pos) {
		obj->extended_selection(mail_error,
			   cbs->selected_item_positions[num_selected - 1]);
		if (mail_error.isSet()) {
		    return;
		}
		SELECTION_MADE = TRUE;
	    }
	    // Otherwise, the deselected item must lie in between other
	    // selected items.  We choose to find the closest selected
	    // item above the deselected item.
	    else {
		// There are selected items that are above the deselected 
		// item.
		// Need to find the one that is both above the deselected 
		// item and closest to it.
		// Iterate until you find the nearest above; select it and
		// drop out of loop when after selection.

		for (i = 0; i < (num_selected - 1) && !SELECTION_MADE; i++) {
		    above_selected_pos = cbs->selected_item_positions[i];
		    tmp_selected_pos = cbs->selected_item_positions[i + 1];
		    if (tmp_selected_pos > last_clicked_on_pos) {
			obj->extended_selection(mail_error,
					above_selected_pos);
			if (mail_error.isSet()) {
			    return;
			}
			SELECTION_MADE = TRUE;
		    }
		}
	    }
	}
    }
}


void
MsgScrollingList::extended_selection(
    DtMailEnv &mail_error,
    int position 
)
{
   FORCE_SEGV_DECL(MsgStruct, tmpMS);

   // Disable the SaveAttachments menu item first.  
   // It gets enabled when an attachment is selected.

   _parent->all_attachments_deselected();

   if (position == 0) {  // all items deselected.
       _parent->hideAttachArea();
       this->display_no_message();
       _selection_on = FALSE;
       _parent->deactivate_default_message_menu();
       return;
   }

   // if there were no selected item(s) before and now we
   // have one/some, we need to turn on the message menu at
   // the parent level.

   if (!_selection_on) {
       _selection_on = TRUE;
       _parent->activate_default_message_menu();
   }
   
   // If selected message is the displayed message, return.
   // No need to redisplay the same message.

   if (position == _selected_item_position) return;

   // Retrieve message...
   // Display the selected message...
   _selected_item_position = position;

   tmpMS = get_message_struct(_selected_item_position);
   if (tmpMS == NULL) {
       return;
    }
   else {
       display_message_summary();
       this->display_message(mail_error, tmpMS->message_handle);
       if (mail_error.isSet()) {
	   return;
       }
   }
}


int
MsgScrollingList::get_selected_item()
{
    return(_selected_item_position);
}

int
MsgScrollingList::get_displayed_item()
{
    return(_displayed_item_position);
}

void
MsgScrollingList::scroll_to_bottom()
{
    XmListSetBottomPos( this->baseWidget() , 0 );
}

// Scroll the list so that the item at position is in the
// middle of the scrolling list.  If the number of items
// that the scrolling list can display at one time is greater
// than the total number of items, then display them all.
// If the item at position is close to the bottom of the list
// make sure we make as many items visible as possible.
//
// If the item at position is already visible, then don't
// do anything.

void
MsgScrollingList::scroll_to_position(
    int position
)
{
    int top, visible, total;
    int top_pos;

    // Determine the position of the header that we want to select.
    // If the XtArgCollector exists, then add
    // the resources to the XtArgCollector so that we can prevent
    // multiple redisplays in the XmList widget.
    if (_xmstr_collector)
    {
        XtVaGetValues( _w,
		   XmNtopItemPosition, &top,
		   XmNvisibleItemCount, &visible,
		   NULL );
	total = _xmstr_collector->GetNumItems();
    }
    else
    {
        XtVaGetValues( _w,
		   XmNtopItemPosition, &top,
		   XmNvisibleItemCount, &visible,
		   XmNitemCount, &total,
		   NULL );
    }

    if (( position < top ) || ( position >= top+visible )) {

	if ((total <= visible) || (position <= visible/2)) {
	    // If we can display them all, make the first item appear
	    // at the top of the list.
	    top_pos = 1;
	} else if (position > (total-(visible/2))) {
	    top_pos = total - visible + 1;
	} else {
	    top_pos = position - visible/2 + 1;
	}

	// Determine the position of the header that we want to select.
	// If the XtArgCollector exists, then add
	// the resources to the XtArgCollector so that we can prevent
	// multiple redisplays in the XmList widget.
	if (_xtarg_collector)
	    _xtarg_collector->AddItemToList (
		XmNtopItemPosition, top_pos);
	else
	    XmListSetPos (_w, top_pos);
    }
}

void
MsgScrollingList::undelete_messages(MsgHndArray *tmpMHlist)
{
    FORCE_SEGV_DECL(MsgStruct, tmpMS);
    FORCE_SEGV_DECL(XmString, deleted_headers);
    int  i, num_entries, entry_position, del_pos;
    int whichToSelectDisplay = 0;
    DtMail::MailBox	*mbox=parent()->mailbox();
    DtMail::Message * tmpMsg;
    DtMailEnv mail_error;
    DtMailMessageHandle tmpMH;
#ifdef undef
    XmString read_status, new_status;
#endif
    XmString complete_header;	// read status + glyph + header_text.

#ifdef undef
/* NL_COMMENT
 * In a mailer container window's message scrolling list, a "N" appears
 * to the left of a mail message header indicating that the mail message
 * is "new" (just arrived and not yet viewed by the user).
 * There is only space to display 1 character.  If "N" needs to be translated,
 * please make sure the translation is only 1 character.
 */
//
// gregl - new_status and read_status are not used in this function.
//	   either comment them out (like I'm doing) or free them.
//
    new_status = XmStringCreateLocalized(GETMSG(DT_catd, 1, 112, "N"));
    read_status = XmStringCreateLocalized(" ");
#endif

    // Initialize the mail_error.
    mail_error.clear();
    num_entries = tmpMHlist->length();
    if (num_entries == 0) return;

    // Deselect all items currently selected.
    // display_and_select_message() will select, highlight
    // and display the last "undeleted" message.
    
    XmListDeselectAllItems(_w);

    XtVaSetValues (_w, XmNselectionPolicy, XmMULTIPLE_SELECT, NULL);

    for (i = 0; i < num_entries; i++)
    {
	DtMailHeaderLine info;

	tmpMS = tmpMHlist->at(i);
	tmpMS->is_deleted = FALSE;

	// Reset the flag of the message in message store so that the  
	// message will not be expunged when the folder is quit.
	// 

	tmpMsg = mbox->getMessage(mail_error, tmpMS->message_handle);
	tmpMsg->resetFlag(mail_error, DtMailMessageDeletePending);
	tmpMH = tmpMS->message_handle;
	
	// Remove chosen item from list of deleted messages;
	// insert it back into _msgs at the right place (which is 
	// determined by session_number of retrieved MsgStruct).
	// Insert back into scrolling list for visual display
	// at the position session_number.

	entry_position = _msgs->insert(tmpMS);

	// Increment by one, because the index into the scrolling
	// list is always one greater than the index into the
	// message handle array that we got the index from.

	entry_position = entry_position + 1;

	// Maintain the assumption that the item at entry_position
	// is the selected item

	_selected_item_position = entry_position;

	mbox->getMessageSummary(mail_error, tmpMS->message_handle,
				_header_info, info);
	DtMail::Message * msg = mbox->getMessage(mail_error, tmpMH);
	complete_header = formatHeader(
			   info,
			   tmpMS->indexNumber,
			   show_with_attachments(msg),
			   msg->flagIsSet(mail_error, DtMailMessageNew));

	mbox->clearMessageSummary(info);

	if (msg->flagIsSet(mail_error, DtMailMessageNew) == DTM_TRUE)
	  num_new_messages++;

	XmListAddItem(_w, complete_header, entry_position);
	XmListSelectItem(_w, complete_header, FALSE);
	XmStringFree(complete_header);

	// Get position of undeleted message structure in _deleted_messages
	// and remove the entry from _deleted_messages

	del_pos = _deleted_messages->indexof(tmpMS);
	_deleted_messages->remove_entry(del_pos);
    }

    // Display this message and select it.

    this->display_message(mail_error, tmpMS->message_handle);
    XtVaSetValues (_w, XmNselectionPolicy, XmEXTENDED_SELECT, NULL);
    XtVaSetValues (_w, XmNselectionMode, XmNORMAL_MODE, NULL);

    if (mail_error.isSet()) return;

    num_deleted_messages -= num_entries;

    MsgHndArray *selected_messages = selected();
    updateListItems(-1, TRUE, selected_messages);
    delete selected_messages;

    display_message_summary();
}

void
MsgScrollingList::undelete_last_deleted()
{
    FORCE_SEGV_DECL(MsgStruct, tmpMS);
    int entry_position;
    int len;
    DtMail::MailBox	*mbox=parent()->mailbox();
    UndelFromListDialog *undel_dialog;
    DtMailEnv mail_error;
    DtMailHeaderLine info;
    DtMail::Message * tmpMsg;
    DtMailMessageHandle tmpMH;
#ifdef undef
    XmString read_status, new_status;
#endif
    XmString complete_header;	// read status + glyph + header_text.

#ifdef undef
/* NL_COMMENT
 * In a mailer container window's message scrolling list, a "N" appears
 * to the left of a mail message header indicating that the mail message
 * is "new" (just arrived and not yet viewed by the user).
 * There is only space to display 1 character.  If "N" needs to be translated,
 * please make sure the translation is only 1 character.
 */
    new_status = XmStringCreateLocalized(GETMSG(DT_catd, 1, 113, "N"));
    read_status = XmStringCreateLocalized(" ");
#endif


    if (num_deleted_messages == 0) return;

    // Initialize the mail_error.
    mail_error.clear();


    // Delete the message from the Deleted Messages Dialog.
    undel_dialog = parent()->get_undel_dialog();
    if (undel_dialog)
	undel_dialog->undelLast();
    
    // Restore the message in RoamMenuWindow:MessageScrollingList.
    len = _deleted_messages->length();

    tmpMS = _deleted_messages->at(len - 1);
    tmpMS->is_deleted = FALSE;

    // Reset the flag of the message in message store so that the  
    // message will not be expunged when the folder is quit.
    // 

    tmpMsg = mbox->getMessage(mail_error, tmpMS->message_handle);
    tmpMsg->resetFlag(mail_error, DtMailMessageDeletePending);
    tmpMH = tmpMS->message_handle;

    // Remove chosen item from list of deleted messages;
    // insert it back into _msgs at the right place (which is 
    // determined by session_number of retrieved MsgStruct).
    // Insert back into scrolling list for visual display
    // at the position session_number.

    entry_position = _msgs->insert(tmpMS);

    // Increment by one, because the index into the scrolling
    // list is always greater than the index into the
    // message handle array that we got the index from.

    entry_position = entry_position + 1;

    mbox->getMessageSummary(mail_error, tmpMS->message_handle,
			    _header_info, info);
    DtMail::Message * msg = mbox->getMessage(mail_error, tmpMH);

    complete_header = formatHeader(
			info,
			tmpMS->indexNumber,
			show_with_attachments(msg),
			msg->flagIsSet(mail_error, DtMailMessageNew));

    mbox->clearMessageSummary(info);

    if (msg->flagIsSet(mail_error, DtMailMessageNew) == DTM_TRUE) {
	num_new_messages++;
    }

    _deleted_messages->remove_entry(len - 1);
    num_deleted_messages--;
    XmListAddItemUnselected(_w, complete_header, entry_position);
    XmStringFree(complete_header);

    
    // If the undeleted message is before the currently viewed message,
    // then need to readjust our numbers by adding one to them -- there
    // is one more message above the currently-viewed message.
    // Don't need to do anything if the undeleted message is after the
    // currently-viewed message.

    
    // Deselect all items currently selected.
    // display_and_select_message() will select, highlight
    // and display the last "undeleted" message.
    
    XmListDeselectAllItems(_w);

    _selected_item_position = entry_position;
    this->display_and_select_message(mail_error, tmpMS->message_handle);
    if (mail_error.isSet()) return;

    updateListItems(-1, TRUE, NULL);
    display_message_summary();
}

DtMailBoolean
MsgScrollingList::senderIsToHeaderWhenMailFromMe(void)
{
    if (_header_info.number_of_names == 5)
      return(DTM_TRUE);

    return(DTM_FALSE);
}

void
MsgScrollingList::checkDisplayProp(void)
{
    DtMail::MailBox * mbox;
    DtMail::MailRc * mailrc;
    DtMailEnv mail_error;
    Boolean state_changed = FALSE;

    mbox = parent()->mailbox();
    mailrc = mbox->session()->mailRc(mail_error);

    const char * value = NULL;
    mailrc->getValue(mail_error, "showto", &value);
    if (mail_error.isNotSet()) {
	// showto is set...if number_of_names is 4 then they
	// just applied props and the showto value
	if (_header_info.number_of_names == 4) {
	    _header_info.number_of_names = 5;
	    state_changed = TRUE;
	}
    }
    else if (_header_info.number_of_names == 5) {
	// They just applied props and changed the showto value
	_header_info.number_of_names = 4;
	state_changed = TRUE;
    }
    if (NULL != value)
      free((void*) value);

    // Here we need to adjust the header labels to maintain them left
    // justified. It looks better than centered. We only whant to change
    // the labels if message numbering has been enabled. Otherwise we will
    // leave them alone. Note: The _numbered stores the previous state for
    // message numbering. Check the error return when getting the value uses
    // reverse logic. ugly...
    DtMailBoolean use_msg_numbers;

    value = NULL;
    mail_error.clear();
    mailrc->getValue(mail_error, "showmsgnum", &value);

    use_msg_numbers = (mail_error.isNotSet()) ? DTM_TRUE : DTM_FALSE;
    if (NULL != value) free((void*) value);

    if (use_msg_numbers != _numbered) 
    {
	_numbered = use_msg_numbers;
	layoutLabels(_sender_lbl, _subject_lbl, _date_lbl, _size_lbl);
    }
    else if (!state_changed) return;

    // We have to build two lists from the current normal and deleted
    // lists. These will contain the new header lines.
    //
    DtMailHeaderLine info;
    XmString * normal_list = new XmString[_msgs->length()];

    for (int m = 0; m < _msgs->length(); m++) {
	MsgStruct * ms = _msgs->at(m);

	DtMail::Message * msg =
			mbox->getMessage(mail_error, ms->message_handle);
	mbox->getMessageSummary(
			mail_error, ms->message_handle,
			_header_info, info);
	normal_list[m] = formatHeader(
			    info,
			    ms->indexNumber,
			    show_with_attachments(msg),
			    msg->flagIsSet(mail_error, DtMailMessageNew));
	mbox->clearMessageSummary(info);
    }

    XmListReplaceItemsPos(_w, normal_list, _msgs->length(), 1);
    for (int fr = 0; fr < _msgs->length(); fr++) {
	XmStringFree(normal_list[fr]);
    }
    delete normal_list;

    UndelFromListDialog * del_dialog = _parent->get_undel_dialog();
    if (del_dialog) {
	XmString * del_list = new XmString[_deleted_messages->length()];
	
	for (int m2 = 0; m2 < _deleted_messages->length(); m2++) {
	    MsgStruct * ms = _deleted_messages->at(m2);
	    
	    DtMail::Message * msg =
		mbox->getMessage(mail_error, ms->message_handle);
	    mbox->getMessageSummary(
				mail_error, ms->message_handle,
				_header_info, info);
	    del_list[m2] = formatHeader(
			     info,
			     ms->indexNumber,
			     show_with_attachments(msg),
			     msg->flagIsSet(mail_error, DtMailMessageNew));
	    mbox->clearMessageSummary(info);
	}
	
	del_dialog->replaceItems(del_list, _deleted_messages->length());
	for (int fr2 = 0; fr2 < _deleted_messages->length(); fr2++) {
	    XmStringFree(del_list[fr2]);
	}
	delete del_list;
    }
}

//
// Update the scrolling list. Current is the index of the message
// to position the scrolling list to.  -1 to keep it as is.
//
void
MsgScrollingList::updateListItems(int current,
				  Boolean renumber_only,
				  MsgHndArray *selected_messages)
{
    DtMail::MailBox * mbox = NULL;
    DtMail::MailRc * mailrc;
    DtMailEnv mail_error;
    const char * value = NULL;
    int		nmsgs;

    mbox = parent()->mailbox();
    mailrc = mbox->session()->mailRc(mail_error);

    if (current < 0) current = _displayed_item_position;
    resetIndexNums();

    mailrc->getValue(mail_error, "showmsgnum", &value);
    if (mail_error.isSet() && renumber_only) return;

    if (selected_messages)
      XtVaSetValues (_w, XmNselectionPolicy, XmMULTIPLE_SELECT, NULL);

    //
    // We need to build a new list of strings to display
    // in the scrolling list.  Initialize that now.
    //
    DtMailHeaderLine info;
    XmString * newList;
    MsgStruct *ms;

    nmsgs = _msgs->length();
    newList = new XmString[nmsgs];
    memset (newList, '0', nmsgs * sizeof (XmString *));

    // Loop through _msgs and create new strings to display in the
    // scrolling list. This is inefficient and dominates the time
    // spent in sort.  It may be worth while finding a way to just
    // rearrange the existing strings.
    for (int m = 0; m < nmsgs; m++)
    {
	DtMail::Message * msg = mbox->getMessage(mail_error,
			_msgs->at(m)->message_handle);

    	if (mail_error.isSet())
	  fprintf(stderr, "dtmail: getMessage: Couldn't get message #%d\n", m);
	
	mbox->getMessageSummary(
			mail_error, _msgs->at(m)->message_handle,
			_header_info, info);

    	if (mail_error.isSet())
	  fprintf(stderr,
	   "dtmail: getMessageSummary: Couldn't get summary for msg # %d\n", m);

	if ((msg == NULL) || (mbox == NULL))
	{
		// Error
		;
	}
	else
	{
    		ms = get_message_struct(m + 1);
		newList [m] =
		  formatHeader(
			info,
			ms->indexNumber,
			show_with_attachments(msg),
			msg->flagIsSet(mail_error, DtMailMessageNew));
	}

	// Free the space allocated for info
	// delete []info.header_values;
	mbox->clearMessageSummary(info);
    }

    XmListReplaceItemsPos(_w, newList, session_message_number, 1);
    for (int fr = 0; fr < nmsgs; fr++)
      XmStringFree(newList[fr]);

    // Update current message
    _selected_item_position = current;
    _displayed_item_position = current;
    scroll_to_position(_displayed_item_position);

    if (selected_messages)
    {
        XmListDeselectAllItems(_w);
        for (int m=0, nselected=selected_messages->length(); m<nmsgs; m++)
        {
            MsgStruct *ms = get_message_struct(m+1);
	    for (int s=0; s<nselected; s++)
	    {
		MsgStruct *sms = selected_messages->at(s);
		if (ms == sms)
		  XmListSelectPos(_w, m+1, FALSE);
	    }
        }
        XtVaSetValues (_w, XmNselectionPolicy, XmEXTENDED_SELECT, NULL);
    }
    else
      XmListSelectPos(_w, _displayed_item_position, FALSE);

    XtVaSetValues (_w, XmNselectionPolicy, XmEXTENDED_SELECT, NULL);
    XtVaSetValues (_w, XmNselectionMode, XmNORMAL_MODE, NULL);
    delete newList;
}

XmString
MsgScrollingList::formatHeader(DtMailHeaderLine & info,
			       int sess_num,
			       DtMailBoolean has_attachments,
			       DtMailBoolean new_msg)
{
    char *buf = new char[BUFSIZ];
    memset(buf, 0, BUFSIZ);
    const char *from=NULL;
    char *subject;
    int contentLength;
    char contentStr[20];
    char *date = new char[BUFSIZ];
    int msg_num = sess_num + 1;
    static XmString attachment_glyph = NULL;
    static XmString no_attachment_glyph = NULL;
    static XmString new_status = NULL;
    static XmString read_status = NULL;
    static unsigned char attach_symbol[16];
    Boolean showto = FALSE;

    if (!attachment_glyph) {
	attach_symbol[0] = 168;
	attach_symbol[1] = 0;
	attachment_glyph = XmStringCreate((char *)attach_symbol, "attach");
	no_attachment_glyph = XmStringCreateLocalized(" ");
	new_status = XmStringCreateLocalized(GETMSG(DT_catd, 1, 114, "N"));
	read_status = XmStringCreateLocalized(" ");
    }
    
    // strip out the Name of sender and retain only the address.
    //Later, we will have separate  entries for name and address
    //and this stripping will not need to be done.
    
    // IMAP is incapable of handling a message header that begins
    // with From (space) (NOTE: not a From:).
    // Thus, if a message has only a From but does not have a From:
    // or a Reply-To: or Received-From:, IMAP doesn't tell us who
    // it is from.  In such cases, we set it to "???".
    
    DtMailAddressSeq * addr_seq = NULL;
    DtMailValueAddress * addr = NULL;
    
    if (info.header_values[0].length() != 0) {
	addr_seq = ((info.header_values[0])[0])->toAddress();
        addr = (*addr_seq)[0];
        if (_header_info.number_of_names == 5 && addr 
		&& addr->dtm_address && info.header_values[4].length() != 0) {
        	// Check if mail is from me
		const char *ptr;
        	passwd pw;
		GetPasswordEntry(pw);
       		if ((ptr = strchr(addr->dtm_address, '@')) != NULL) {
			if (strncmp(pw.pw_name, addr->dtm_address, 
				ptr-addr->dtm_address) == 0) {
				from = *((info.header_values[4])[0]);
				showto = TRUE;
			}
  		}
		else 
			if (strcmp(pw.pw_name, addr->dtm_address) == 0) {
				from = *((info.header_values[4])[0]);
				showto = TRUE;
			}
	}
        if (from == NULL) {
                if (addr && addr->dtm_person)
                        from = addr->dtm_person;
                else if (addr && addr->dtm_address)
                        from = addr->dtm_address;
                else from = "???";
        }
    }
    else
    	from = "???";
    
    // If the Subject is nil
    
    if (info.header_values[3].length() == 0) {
	subject = new char[1];
	subject[0] = '\0';
    } else {
	// Get the BE store of header.  It may contain newlines or 
	// tab chars which can munge the scrolling list's display!
	// 
	const char *real_subj_header = *((info.header_values[3])[0]);
	int fc;
	int subj_len;
	char *tmp_subj;

	// Check if BE store contains the funky chars.

	for (fc = 0, subj_len = strlen(real_subj_header), 
	      tmp_subj = (char *)real_subj_header; 
	     fc < subj_len; fc++, tmp_subj++) {

	    char c = *tmp_subj;
	    if ( (c == '\n') 
	      || (c == '\t') 
	      || (c == '\r')) {

		break;
	    }
	}
	subject = new char[fc+1];
	strncpy((char *)subject, real_subj_header, fc);
	subject[fc] = '\0';
    }

    
    // Skip the first (beginning) space in from; search for the next
    // occurring space.  
    

    const char *dateformat;
    if (info.header_values[1].length() > 0)
    {
	DtMailValueDate ds = ((info.header_values[1])[0])->toDate();

	if (ds.dtm_date && ds.dtm_tz_offset_secs)
	{
#define USE_YEAR_FORMAT_SECONDS	(60 * 60 * 24 * 180)
	    time_t now;
	    tm tm_struct;

	    SafeLocaltime(&ds.dtm_date, tm_struct);

            // Refer to strftime man page for explanation of the date format.
	    now = time(NULL);
	    if (USE_YEAR_FORMAT_SECONDS < now - ds.dtm_date)
	      dateformat = GETMSG(DT_catd, 1, 259, "%a %b %d  %Y");
	    else
	    {

#ifdef sun
	        dateformat = GETMSG(DT_catd, 1, 260, "%a %b %d %k:%M");
#else
	        dateformat = GETMSG(DT_catd, 1, 261, "%a %b %d %H:%M");
#endif
	    }

	    SafeStrftime(date, BUFSIZ, dateformat, &tm_struct);
	}
	else
	  // Couldn't get Date string from Message. Make it empty.
	  sprintf(date, "%s", " ");
    }
    else
    {
	tm epoch;
	memset(&epoch, 0, sizeof(tm));

        /* Refer to strftime man page for explanation of the date format.  */
	dateformat = GETMSG(DT_catd, 1, 259, "%a %b %d  %Y");
	SafeStrftime(date, BUFSIZ, dateformat, &epoch);
    }
    
    if (info.header_values[2].length() > 0) {
	contentLength = (int) strtol(*((info.header_values[2])[0]), NULL, 10);
    }
    else {
	contentLength = 0;
    }
    
    if (contentLength < 1000) {
	sprintf(contentStr, "%d", contentLength);
    }
    else if (contentLength < 1000000) {
	sprintf(contentStr, "%dK", contentLength / 1000);
    }
    else {
	sprintf(contentStr, "%dM", contentLength / 1000000);
    }
    
    // If we are to print the message_number in the header_list,
    // use msg_num as the first element in the sprintf.  
    // Introduce a %d at the beginning though.

    DtMail::MailBox * mbox;
    DtMail::MailRc * mailrc;
    DtMailEnv mail_error;
  
    mbox = parent()->mailbox();
    mailrc = mbox->session()->mailRc(mail_error);

    const char * value = NULL;
    mailrc->getValue(mail_error, "showmsgnum", &value);
    if (mail_error.isSet()) {
        // No message numbers ... keep usual "35" col. "Subject".
      if (showto)
              sprintf(buf, " To %-15.15s %-35.35s %-17.17s %-5.5s",
                      from,
                      subject,
                      date,
                      contentStr);
      else
              sprintf(buf, " %-18.18s %-35.35s %-17.17s %-5.5s",
                      from,
                      subject,
                      date,
                      contentStr);
    }
    else {
        //  To keep 80 column format use 5 less columns of
        //  subject , when msg numbers are on.
      if (showto)
              sprintf(buf, " To %-15.15s %-30.30s %-17.17s %-5.5s",
                      from,
                      subject,
                      date,
                      contentStr);
      else
              sprintf(buf, " %-18.18s %-30.30s %-17.17s %-5.5s",
                      from,
                      subject,
                      date,
                      contentStr);
    }
    if (NULL != value)
      free((void*) value);

    XmString header_text = XmStringCreateLocalized(buf);
    XmString item, item2, complete_header;

    if (has_attachments == DTM_TRUE) {
	item = XmStringConcat(attachment_glyph, header_text);
	XmStringFree(header_text);
    }
    else {
	item = XmStringConcat(no_attachment_glyph, header_text);
	XmStringFree(header_text);
    }

    if (new_msg == DTM_FALSE) {
	item2 = XmStringConcat(read_status, item);
	XmStringFree(item);
    }
    else {
	item2 = XmStringConcat(new_status, item);
	XmStringFree(item);
    }

    value = NULL;
    mailrc->getValue(mail_error, "showmsgnum", &value);
    if (mail_error.isSet()) {
//	complete_header = XmStringCopy(item2);
	complete_header = item2;
	_numbered = DTM_FALSE;
    }
    else {
	char num_buf[64];

        if (NULL != value)
          free((void*) value);

	mailrc->getValue(mail_error, "nerdmode", &value);
	if (mail_error.isSet()) {
	    sprintf(num_buf, "%4d ", msg_num);
	}
	else {
	    sprintf(num_buf, "%4x ", msg_num - 1);
	}

	XmString num_str = XmStringCreateLocalized(num_buf);
        complete_header = XmStringConcat(num_str, item2);
	XmStringFree(item2);
	XmStringFree(num_str);
	_numbered = DTM_TRUE;
    }
    if (NULL != value)
      free((void*) value);

    delete addr_seq;
    delete subject;
    
    delete [] buf;
    delete [] date;
    return(complete_header);
}

void
MsgScrollingList::shutdown()
{
    int num_entries, i;
    FORCE_SEGV_DECL(MsgStruct, tmpMS);
    DtMailMessageHandle tmpMH;
    DtMail::MailBox	*mbox=parent()->mailbox();
    DtMailEnv mail_error;

    // Initialize the mail_error.
    mail_error.clear();

    if (num_deleted_messages == 0) return;
    
    num_entries = _deleted_messages->length();

    for (i = 0; i < num_entries; i++) {
	tmpMS = _deleted_messages->at(i);
	tmpMH = tmpMS->message_handle;

//	mbox->deleteMsg(mail_error, tmpMH);
	
    }
}

#ifdef DEAD_WOOD
DtMailMessageHandle
MsgScrollingList::lastMsg()
{
    if (_msgs->length() > 0) {
	return((_msgs->at(_msgs->length() - 1))->message_handle);
    }
    else {  // Currently an empty folder
	return(NULL);
    }
}
#endif /* DEAD_WOOD */

void
MsgScrollingList::clearMsgs()
{
    if ( _msgs->length() > 0 )
	_msgs->clear();
}


void
MsgScrollingList::display_message_summary()
{
    parent()->message_summary(
			selected_item_position(),
			get_num_messages(), 
			get_num_new_messages(),
			get_num_deleted_messages());
}

void
MsgScrollingList::display_message(
    DtMailEnv &mail_error,
    int pos
)
{
    if ((pos > 0) && (pos <= session_message_number)) {
	// When loading mail headers, we need to make sure that
	// the XmList resources are all set at the same time to
	// avoid painting multiple times.  So here we collect
	// these resources.
	if (_xtarg_collector && _xmstr_collector)
	{
	    XmString *items = _xmstr_collector->GetItems();
            
	    // Keep a handle to the malloced string copy so that
            // we can free it after the XtSetValues
	    _selected_items = XmStringCopy (items[pos-1]);

	    _xtarg_collector->AddItemToList (XmNselectedItems,
	        (XtArgVal) &_selected_items);
	    _xtarg_collector->AddItemToList (
	        XmNselectedItemCount, 1);
	}
	else
	{
	    XmListSelectPos(_w, pos, FALSE);
        }

	this->extended_selection(mail_error, pos);
	if (mail_error.isSet()) {
	    // Return whatever error mailbox->get_next_msg() returned.
	    return;
	}
    }
    else return;
}

DtMailMessageHandle
MsgScrollingList::current_msg_handle()
{
    DtMailMessageHandle	msg_number;

    if ( _displayed_item_position > 0 )
	msg_number=msgno( _displayed_item_position );
    else
	msg_number=NULL;

    return msg_number;
}

void
MsgScrollingList::expunge(void)
{
    for (int i = _deleted_messages->length() - 1; i >= 0; i--) {
	_deleted_messages->remove_entry(i);
    }
    resetSessionNums();
    resetIndexNums();
    num_deleted_messages = 0;
    display_message_summary();
}

int
MsgScrollingList::resetIndexNums(void)
{
    int	m;
    int	length = _msgs->length();
    MsgStruct *ms;
    
    for (m = 0; m < length; m++)
    {
        ms = _msgs->at(m);
        ms->indexNumber = m;
    }

    length = _deleted_messages->length();
    for (m = 0; m < length; m++)
    {
        ms = _deleted_messages->at(m);
        ms->indexNumber = m;
    }

    return m;
}

int
MsgScrollingList::resetSessionNums(void)
{
    int	m;
    int	length = _msgs->length();
    MsgStruct *ms;
    
    for (m = 0; m < length; m++)
    {
        ms = _msgs->at(m);
        ms->sessionNumber = m;
    }
    session_message_number = m;
    return session_message_number;
}

void
MsgScrollingList::sort_messages(void)
{
    DtMail::MailBox	*mbox;
    enum sortBy		sortby;
    int			current_msg;

    MsgHndArray *selected_messages = selected();

    mbox = _parent->mailbox();
    sortby = _parent->last_sorted_by();

    // Sort array of message handles
    current_msg = _sorter->sortMessages(this, mbox, sortby);

    // The array of message handles is sorted. Now we need to update
    // the display preserving the selected state of the messages.
    updateListItems(current_msg, FALSE, selected_messages);

    delete selected_messages;

    _parent->last_sorted_by(sortby);
    display_message_summary();
}

//
// Layout out the row of labels above the scrolling list
//
void
MsgScrollingList::layoutLabels(
	Widget sender,
	Widget subject,
	Widget date,
	Widget size)
{
    // Save the input values 
    _sender_lbl = sender; 
    _subject_lbl = subject;
    _date_lbl = date;
    _size_lbl = size;

    layoutLabels();

}
//
// Layout out the row of labels above the scrolling list
//
void
MsgScrollingList::layoutLabels()
{
    // Width of fields.  +1 for spaces
    int	num_width = 5,
	sender_width = 18 + 1,
	subject_width = 35 + 1,
        date_width = 3+1 + 3+1 + 2+1 + 5 + 2; // DDD mmm dd hh:mm 2spaces
    int	char_width;		// Width of a single character
    int	n = 0;
    XmString	xmstr;
    XmFontList	font_list;

    // Calculate the width of date format to allocate the label 
    // width dynamically
    struct tm *tm;
    time_t clock;
    char   buf[40];
    
    clock = time((time_t *) 0);
    tm = localtime(&clock);  
    #ifdef sun
	SafeStrftime(buf,
	     sizeof(buf), 
	     GETMSG(DT_catd, 1, 222, "%a %b %d %k:%M"), 
	     tm);
    #else
        SafeStrftime(buf, 
	     sizeof(buf), 
	     GETMSG(DT_catd, 1, 223, "%a %b %d %H:%M"), 
	     tm);
    #endif


    // List uses a fixed width font. Therefore all characters are the
    // same size.  So we use a space to determine the width of a char
    xmstr = XmStringCreateLocalized(" ");
    XtVaGetValues(_w, XmNfontList, &font_list, NULL);
    char_width = XmStringWidth(font_list, xmstr);

    if (_numbered) {
	// Numbering is on
	n = num_width;
	subject_width -= num_width;
    } else {
	n = 0;
    }

    // XXX dipol: Need to take into account if the scrollbar is on the
    // right or left
    n += 3;	// Margin.

    XtWidgetGeometry geom;
    geom.request_mode = CWX;
    XtQueryGeometry(_sender_lbl, NULL, &geom);
    if (geom.x != (n * char_width)) {
        geom.x = (n * char_width);
        XtMoveWidget(_sender_lbl, geom.x, geom.y);
    } // Move the X location of the sender title

    XtVaSetValues(_sender_lbl, XmNx, n * char_width, NULL);
    n += sender_width;

    XtQueryGeometry(_subject_lbl, NULL, &geom);
    if (geom.x != (n * char_width)) {
        geom.x = (n * char_width);
        XtMoveWidget(_subject_lbl, geom.x, geom.y);
    } // Move the X location of the subject title

    XtVaSetValues(_subject_lbl, XmNx, n * char_width, NULL);
    n += subject_width;
    XtVaSetValues(_date_lbl, XmNx, n * char_width, NULL);
    n += date_width;
    XtVaSetValues(_size_lbl, XmNx, n * char_width, NULL);

    XmStringFree(xmstr);

    return;
}
