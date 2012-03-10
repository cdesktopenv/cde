// $XConsortium: InputMgrX.hh /main/4 1996/07/10 09:38:43 rcs $
#include "UAS.hh"
# include <X11/Intrinsic.h>

class InputMgrX;

struct InputMgrData {
    InputMgrX *			fObj;
    WantInputReady		fMsg;
    XtInputId			fId;
};

class InputMgrX: public UAS_Receiver<WantInputReady>,
		 public UAS_Receiver<WantInputReadyCancel>,
		 public UAS_Sender<InputReady> {

    public:
	InputMgrX ();

    public:
	void		receive (WantInputReady &msg, void *client_data);
	void		receive (WantInputReadyCancel &msg, void *client_data);

    private:
	static void	inputHandler (InputMgrData *, int *, XtInputId *);

    private:
	xList<InputMgrData *>		fWantList;
};
