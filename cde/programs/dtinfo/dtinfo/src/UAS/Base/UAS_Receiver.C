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
// $XConsortium: UAS_Receiver.cc /main/3 1996/06/11 16:39:11 cde-hal $
#ifndef _UAS_Receiver_cc_
#define _UAS_Receiver_cc_

#include "UAS_Sender.hh"

// /////////////////////////////////////////////////////////////////
// class destructor
// /////////////////////////////////////////////////////////////////

template <class T>
UAS_Receiver<T>::~UAS_Receiver()
{
    for (int i = f_sender_list.numItems() - 1; i >= 0; i --) {
	f_sender_list[i]->unrequest (this);
	f_sender_list.remove(f_sender_list[i]);
    }
}

#endif
