// $XConsortium: ClientEventHandler.hh /main/3 1996/06/11 16:18:07 cde-hal $

#include <X11/Intrinsic.h>

class ClientEventHandler : public Long_Lived
{
public:
  virtual void handle_event(Widget widget, XEvent *event, Boolean *bvalue) = 0 ;
};
