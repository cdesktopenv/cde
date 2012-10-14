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
 *  $XConsortium: InputMgrX.cc /main/5 1996/07/10 09:38:35 rcs $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 *
 */

#define C_InputMgrX
#define L_Managers

#include "UAS.hh"

#define C_xList
#define C_InputNotifier
#define L_Support

#define C_WindowSystem
#define L_Other

# include <Prelude.h>
# include <sstream>
using namespace std;


InputMgrX::InputMgrX () {
    InputNotifier::request((UAS_Receiver<WantInputReady> *) this);
    InputNotifier::request((UAS_Receiver<WantInputReadyCancel> *) this);
}

void
InputMgrX::receive (WantInputReady &msg, void *client_data) {
    InputMgrData *newData = new InputMgrData;
    newData->fMsg = msg;
    newData->fObj = this;
    newData->fId = XtAppAddInput(window_system().app_context(), msg.fChannel, (XtPointer) XtInputReadMask,
				(XtInputCallbackProc) InputMgrX::inputHandler, 
				(XtPointer) newData);
    fWantList.insert (newData);
    request(msg.fReceiver);
}

void
InputMgrX::receive (WantInputReadyCancel &msg, void *client_data) {
    List_Iterator<InputMgrData *> iterator(&fWantList);
    InputMgrData *cur;
    for ( ; iterator; iterator ++) {
	cur = iterator.item();
	if (cur->fMsg.fReceiver == msg.fReceiver && cur->fMsg.fChannel == msg.fChannel) {
	    break;
	}
    }
    if (iterator) {
	XtRemoveInput (cur->fId);
	fWantList.remove (cur);
	delete cur;
	//
	//  SWM: Cannot do the unrequest here, as the call stack currently may include
	//  inputHandler below, which is in the middle of a send_message which is in
	//  the middle of iterating through the list. send_message can't handle the
	//  list changing out from under it. The unrequest will happen when the document
	//  goes away.
	//
	//  unrequest(msg.fReceiver);
    }
}

void
InputMgrX::inputHandler(InputMgrData *data, int *, XtInputId *) {
    InputReady msg;
    msg.fChannel = data->fMsg.fChannel;
    data->fObj->send_message (msg);
}
