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
