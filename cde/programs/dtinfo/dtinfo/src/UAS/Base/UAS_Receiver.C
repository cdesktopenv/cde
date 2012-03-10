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
