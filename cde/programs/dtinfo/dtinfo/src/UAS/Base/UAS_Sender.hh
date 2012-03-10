// $XConsortium: UAS_Sender.hh /main/7 1996/08/21 15:44:45 drk $
#ifndef _UAS_Sender_hh
#define _UAS_Sender_hh

# include "UAS_PtrList.hh"

#ifndef CONCAT
#if defined(__STDC__) || defined(hpux)
#define CONCAT(A,B) A##B
#define CONCAT3(A,B,C) A##B##C
#else
#define CONCAT(A,B) A/**/B
#define CONCAT3(A,B,C) A/**/B/**/C
#endif
#endif

#if defined(__osf__)
#if defined(CONCAT)
#undef CONCAT
#define CONCAT(A,B) A##B
#endif
#if defined(CONCAT3)
#undef CONCAT3
#define CONCAT3(A,B,C) A##B##C
#endif
#endif

template <class T> class UAS_Receiver;

template <class T>
class UAS_Sender {
public:
  UAS_Sender() { }
  ~UAS_Sender();

  void send_message (const T &message, void *client_data = 0);

  // Request message T.
  void request (UAS_Receiver<T> *receiver);
  // Stop receiving message T.
  void unrequest (UAS_Receiver<T> *receiver);

private:
  // Keep track of message receivers so that we can send them messages
  // and notify them when we have been destroyed.
  UAS_PtrList<UAS_Receiver<T> > f_receiver_list;
};

#define STATIC_SENDER_HH(T)				\
  static void send_message (const T &message, void *client_data = 0)		\
    { CONCAT(sender,T)().send_message (message, client_data); }	\
  static void request (UAS_Receiver<T> *receiver)		\
    { CONCAT(sender,T)().request (receiver); }		\
  static void unrequest (UAS_Receiver<T> *receiver)		\
    { CONCAT(sender,T)().unrequest (receiver); }	\
  static UAS_Sender<T> &CONCAT(sender,T)()

#define STATIC_SENDER_CC(T) \
  UAS_Sender<T> &CLASS::CONCAT(sender,T)() { \
    static UAS_Sender<T> CONCAT(f_sender,T); \
    return (CONCAT(f_sender,T)); }

#ifdef EXPAND_TEMPLATES
#include "UAS_Sender.C"
#endif

#endif
