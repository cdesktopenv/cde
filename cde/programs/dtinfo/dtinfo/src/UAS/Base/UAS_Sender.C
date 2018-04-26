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
// $XConsortium: UAS_Sender.cc /main/4 1996/07/10 09:41:53 rcs $
#ifndef _UAS_Sender_CC_
#define _UAS_Sender_CC_

#include "UAS_Receiver.hh"

// /////////////////////////////////////////////////////////////////
// class destructor - remove this sender from all receiver's lists
// /////////////////////////////////////////////////////////////////

template <class T>
UAS_Sender<T>::~UAS_Sender()
{
    for (int i = f_receiver_list.numItems() -1; i >= 0; i --) {
	f_receiver_list[i]->remove(this);
	f_receiver_list.remove(f_receiver_list[i]);
    }
}


// /////////////////////////////////////////////////////////////////
// request/unrequest 
// /////////////////////////////////////////////////////////////////

template <class T> void
UAS_Sender<T>::request (UAS_Receiver<T> *receiver)
{
  f_receiver_list.append (receiver);
  receiver->append (this);
}      

template <class T> void
UAS_Sender<T>::unrequest (UAS_Receiver<T> *receiver)
{
  f_receiver_list.remove (receiver);
}


// /////////////////////////////////////////////////////////////////
// send - send a message to all receivers
// /////////////////////////////////////////////////////////////////

template <class T> void
UAS_Sender<T>::send_message (const T &message, void *client_data)
{
    for (int i = f_receiver_list.numItems()-1; i >= 0; i --) {
	f_receiver_list[i]->receive ((T &) message, client_data);
    }
}

#endif
