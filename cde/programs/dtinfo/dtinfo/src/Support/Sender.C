/*
 * $XConsortium: Sender.C /main/4 1996/09/27 19:02:35 drk $
 *
 * Copyright (c) 1993 HAL Computer Systems International, Ltd.
 * All rights reserved.  Unpublished -- rights reserved under
 * the Copyright Laws of the United States.  USE OF A COPYRIGHT
 * NOTICE IS PRECAUTIONARY ONLY AND DOES NOT IMPLY PUBLICATION
 * OR DISCLOSURE.
 * 
 * THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE
 * SECRETS OF HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.  USE,
 * DISCLOSURE, OR REPRODUCTION IS PROHIBITED WITHOUT THE
 * PRIOR EXPRESS WRITTEN PERMISSION OF HAL COMPUTER SYSTEMS
 * INTERNATIONAL, LTD.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject
 * to the restrictions as set forth in subparagraph (c)(l)(ii)
 * of the Rights in Technical Data and Computer Software clause
 * at DFARS 252.227-7013.
 *
 *          HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.
 *                  1315 Dell Avenue
 *                  Campbell, CA  95008
 * 
 */

#ifndef _Sender_cc
#define _Sender_cc

#include "Receiver.hh"

#include <stream.h>

// /////////////////////////////////////////////////////////////////
// class destructor - remove this sender from all receiver's lists
// /////////////////////////////////////////////////////////////////

template <class T>
Sender<T>::~Sender()
{
  ON_DEBUG (printf ("~Sender() @ 0x%p\n", this));
  List_Iterator<Receiver<T> *> i (f_receiver_list);

  while (i)
    {
      ON_DEBUG (printf ("  Notify Receiver 0x%x\n", i.item()));
      i.item()->remove (this);
      f_receiver_list.remove (i);
    }
}


// /////////////////////////////////////////////////////////////////
// request/unrequest 
// /////////////////////////////////////////////////////////////////

template <class T> void
Sender<T>::request (Receiver<T> *receiver)
{
  f_receiver_list.append (receiver);
  // Save this Sender in the receiver's list. 
  receiver->append (this);
  ON_DEBUG (printf ("Sender::request (0x%p) (this = 0x%p / len = %d)\n",
		    receiver, this, f_receiver_list.length()));
}      

template <class T> void
Sender<T>::unrequest (Receiver<T> *receiver)
{
  ON_DEBUG (printf ("Sender::unrequest (0x%p) (this = 0x%p)\n",
		    receiver, this));
  f_receiver_list.remove (receiver);
}


// /////////////////////////////////////////////////////////////////
// send - send a message to all receivers
// /////////////////////////////////////////////////////////////////

template <class T> void
Sender<T>::send_message (const T &message)
{
  ON_DEBUG (printf ("Sender::send() from 0x%p (len = %d)\n",
		    this, f_receiver_list.length()));
  List_Iterator<Receiver<T> *> i;

  //
  //  SWM: Modify the loop slightly to take into account
  //  the possibility of the guy we're currently sending to
  //  removing himself from the list when he receives this
  //  message. This is a bit dangerous as it depends on the
  //  implementation of list iterators.
  //
  for (i = f_receiver_list; i != NULL; )
    {
      Receiver<T> *cur = i.item();
      i ++;
      ON_DEBUG (printf ("  sending to 0x%x\n", i.item()));
      cur->receive((T &) message);
    }
}

#endif /* _Sender_cc */
/* DO NOT ADD ANY LINES AFTER THIS #endif */
