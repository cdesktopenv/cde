// $XConsortium: ClientEventMgrX.hh /main/3 1996/06/11 16:24:14 cde-hal $
#include <X11/Intrinsic.h>

class ClientEventHandler ;
class ClientEventMgr : public Long_Lived
{
public:

  void register_handler(ClientEventHandler *, Widget);

private:

  static void dispatch(Widget, XtPointer, XEvent *, Boolean *);

  LONG_LIVED_HH(ClientEventMgr, client_event_manager);
};

LONG_LIVED_HH2(ClientEventMgr, client_event_manager);
