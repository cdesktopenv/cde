/* $XConsortium: WXmRadioBox.h /main/3 1996/06/11 17:01:45 cde-hal $ */
#ifndef WXmRadioBox_h
#define WXmRadioBox_h

#include "WXmRowColumn.h"
#include "WXmCascadeButton.h"
#include "WXmToggleButtonGadget.h"

class WXmRadioBox : public WXmRowColumn {
public :
		WXmRadioBox (WComposite& father, char* name = NULL, ArgList args = NULL, Cardinal card = 0)
		  : WXmRowColumn (XmCreateRadioBox (Widget(father), name, args, card)) { }
	WXmToggleButtonGadget	AddToggleButton (char* name, XtCallbackProc proc = (XtCallbackProc)NULL, caddr_t closure = NULL, ArgList args = NULL, Cardinal card = 0);
	WXmToggleButtonGadget	AddToggleButton (char* name, WWL*, WWL_FUN, ArgList args = NULL, Cardinal card = 0);
};

#endif
