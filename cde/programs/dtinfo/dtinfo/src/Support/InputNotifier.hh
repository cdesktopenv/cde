// $XConsortium: InputNotifier.hh /main/3 1996/06/11 16:34:55 cde-hal $
#ifndef _InputNotifier_h_
#define _InputNotifier_h_

#include "UAS.hh"


struct InputReady: public Destructable {
    int				fChannel;
};

struct WantInputReady: public Destructable {
    UAS_Receiver<InputReady> *	fReceiver;
    int				fChannel;
};

struct WantInputReadyCancel: public Destructable {
    UAS_Receiver<InputReady> *	fReceiver;
    int				fChannel;
};

class InputNotifier {
    public:
	InputNotifier (UAS_Receiver<InputReady> *, int);
	~InputNotifier ();
	STATIC_SENDER_HH (WantInputReady);
	STATIC_SENDER_HH (WantInputReadyCancel);

    private:
	UAS_Receiver<InputReady> *		fReceiver;
	int				fChannel;
};

#endif
