/*
 * $XConsortium: Receiver.C /main/4 1996/09/27 19:02:29 drk $
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

#ifndef _Receiver_cc
#define _Receiver_cc

#include "Sender.hh"

#include <stdio.h>

// /////////////////////////////////////////////////////////////////
// class destructor
// /////////////////////////////////////////////////////////////////

template <class T>
Receiver<T>::~Receiver()
{
  ON_DEBUG (printf ("~Receiver() @ 0x%p\n", this));
  List_Iterator<Sender<T> *> i (f_sender_list);

  while (i)
    {
      ON_DEBUG (printf ("  Notify Sender 0x%x\n", i.item()));
      i.item()->unrequest (this);
      f_sender_list.remove (i);
    }
}

#endif /* _Receiver_cc */
/* DO NOT ADD ANY LINES AFTER THIS #endif */
