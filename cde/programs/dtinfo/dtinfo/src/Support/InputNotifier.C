// $XConsortium: InputNotifier.cc /main/3 1996/06/11 16:34:50 cde-hal $
# define L_Support
# define C_InputNotifier

# include <Prelude.h>
# include <stream.h>

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
