/* $XConsortium: WXmRadioBox.cc /main/3 1996/06/11 17:03:50 cde-hal $ */

#ifndef STREAMH
#include <stream.h>
#endif

#ifndef _WXmRadioBox_h
#include <WWL/WXmRadioBox.h>
#endif

WXmToggleButtonGadget
WXmRadioBox :: AddToggleButton (char* name, XtCallbackProc proc, caddr_t closure, ArgList args, Cardinal card)
{
	WXmToggleButtonGadget bg(widget, name, args, card);
	if (proc)
		XtAddCallback (Widget(bg), XmNvalueChangedCallback,
			       proc, closure);

	return bg;
}


WXmToggleButtonGadget
WXmRadioBox :: AddToggleButton (char* name, WWL* obj, WWL_FUN fun, ArgList args, Cardinal card)
{
	WXmToggleButtonGadget bg(widget, name, args, card);
	bg.SetValueChangedCallback (obj, fun);

	return bg;
}


