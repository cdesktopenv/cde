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
// $XConsortium: UAS_Receiver.hh /main/5 1996/08/21 15:44:41 drk $

/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

#ifndef _UAS_Receiver_hh
#define _UAS_Receiver_hh

# include "UAS_PtrList.hh"

template <class T> class UAS_Sender;

template <class T>
class UAS_Receiver
{
friend class UAS_Sender<T>;
protected:
  // This is an abstract base class.
  UAS_Receiver() { }
  virtual ~UAS_Receiver();

  virtual void receive (T &message, void *client_data = 0) = 0;

  // ~UAS_Sender() automatically calls append and remove.
  void append (UAS_Sender<T> *sender)
    { f_sender_list.append (sender); }
  void remove (UAS_Sender<T> *sender)
    { f_sender_list.remove (sender); }

#ifdef UNNECESSARY
  // Normally I'd add a reference arg function to the UAS_Sender,
  // by alas, the stupid compiler crashes on that. 
  operator UAS_Receiver<T> *()
    { return (this); }
#endif

private:
  // Keep track of all classes that might send us messages so that
  // we can unregister from them when we are destroyed. 
  UAS_PtrList<UAS_Sender<T> > f_sender_list;
};

#ifdef EXPAND_TEMPLATES
#include "UAS_Receiver.C"
#endif

#endif
