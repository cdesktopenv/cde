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
// $XConsortium: InputNotifier.cc /main/3 1996/06/11 16:34:50 cde-hal $
# define L_Support
# define C_InputNotifier

# include <Prelude.h>
# include <sstream>
using namespace std;

#define CLASS InputNotifier

STATIC_SENDER_CC (WantInputReady);

STATIC_SENDER_CC (WantInputReadyCancel);

InputNotifier::InputNotifier (UAS_Receiver<InputReady> *receiver, int channel) {
    fReceiver = receiver;
    fChannel = channel;
    WantInputReady msg;
    msg.fReceiver = fReceiver;
    msg.fChannel = fChannel;
    send_message (msg);
}

InputNotifier::~InputNotifier () {
    WantInputReadyCancel msg;
    msg.fReceiver = fReceiver;
    msg.fChannel = fChannel;
    send_message (msg);
}
