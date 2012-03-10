/* $XConsortium: WXmMenu.h /main/3 1996/06/11 17:01:03 cde-hal $ */
#ifndef WXmMenu_h
#define WXmMenu_h

#include "WXmRowColumn.h"
#include "WXmCascadeButton.h"
#include "WXmCascadeButtonGadget.h"
#include "WXmPushButtonGadget.h"
#include "WXmLabelGadget.h"

class WXmPopupMenu : public WXmRowColumn {

public :

   WXmPopupMenu()
     { }
   WXmPopupMenu (WComposite& father, char* name = NULL,
		 ArgList args = NULL, Cardinal card = 0);
   WXmPopupMenu (WComposite& father, char* name, WArgList args);
   WXmPopupMenu (WComposite& father, char* name, int /* automanage */,
	         WArgList args);
   ~WXmPopupMenu () {}

   WXmPushButtonGadget
      AddPushButton (char* name, XtCallbackProc proc = (XtCallbackProc)NULL,
		     caddr_t closure = NULL, ArgList args = NULL,
		     Cardinal card = 0);

   WXmPushButtonGadget
      AddPushButton (char* name, WWL*, WWL_FUN, ArgList args = NULL,
		     Cardinal card = 0);

   inline void	MenuPosition(XButtonPressedEvent *event) {
      XmMenuPosition(widget, event);
   }
   inline void Popup (XButtonPressedEvent *event)
     { MenuPosition (event); Manage(); }
   inline void Popup()
     { Manage(); }
   inline void Popdown()
     { Unmanage(); }
};

class WXmPulldownMenu : public WXmRowColumn {

public :

   WXmPulldownMenu (WComposite& father, char* name, ArgList args = NULL,
		    Cardinal card = 0)
   : WXmRowColumn (XmCreatePulldownMenu(Widget(father), name, args, card)) {}

   WXmPulldownMenu (WComposite& father, char* name, WArgList args)
   : WXmRowColumn (XmCreatePulldownMenu (Widget(father), name, args.Args(),
				         args.NumArgs())) {}

   WXmPulldownMenu (WComposite& father, char* name, int /* automanage */,
	            WArgList args)
   : WXmRowColumn (XmCreatePulldownMenu (Widget(father), name, args.Args(),
				         args.NumArgs()))
     { Manage(); }

   WXmPushButtonGadget
      AddPushButton (char* name, XtCallbackProc proc = (XtCallbackProc)NULL,
		     caddr_t closure = NULL, ArgList args = NULL,
		     Cardinal card = 0);

   WXmPushButtonGadget
      AddPushButton (char* name, WWL*, WWL_FUN, ArgList args = NULL,
		     Cardinal card = 0);
};

class WXmOptionMenu : public WXmRowColumn {

public :
   WXmOptionMenu()
     { }

   WXmOptionMenu (WComposite& father, char* name);

   WXmOptionMenu (WComposite& father, char* name, ArgList args,
		  Cardinal card)
   : WXmRowColumn (XmCreateOptionMenu(Widget(father), name, args, card)){}

   WXmOptionMenu (WComposite& father, char* name, int /* automanage */,
		  ArgList args = NULL, Cardinal card = 0)
   : WXmRowColumn (XmCreateOptionMenu(Widget(father), name, args, card))
     { Manage(); }

   WXmOptionMenu (WComposite& father, char* name, int /* automanage */,
	          WArgList args)
   : WXmRowColumn (XmCreateOptionMenu (Widget(father), name, args.Args(),
				       args.NumArgs()))
     { Manage(); }

   WXmPushButtonGadget
      AddPushButton (char* name, XtCallbackProc proc = (XtCallbackProc)NULL,
		     caddr_t closure = NULL, ArgList args = NULL,
		     Cardinal card = 0);

   WXmPushButtonGadget
      AddPushButton (char* name, WWL*, WWL_FUN, ArgList args = NULL,
		     Cardinal card = 0);

   WXmCascadeButtonGadget
      OptionButtonGadget (void);

   WXmLabelGadget
      OptionLabelGadget (void);

};

class WXmMenuBar : public WXmRowColumn {

public :

   WXmMenuBar (WComposite& father, char* name = NULL, ArgList args = NULL,
	       Cardinal card = 0)
   : WXmRowColumn (XmCreateMenuBar (Widget(father), name, args, card)) {}

   WXmMenuBar (WComposite& father, char* name, int /* automanage */,
	       ArgList args = NULL, Cardinal card = 0)
   : WXmRowColumn (XmCreateMenuBar (Widget(father), name, args, card))
     { Manage(); }

   WXmMenuBar (WComposite& father, char* name, int /* automanage */,
	       WArgList args)
   : WXmRowColumn (XmCreateMenuBar (Widget(father), name, args.Args(),
				    args.NumArgs()))
     { Manage(); }

   WXmCascadeButton
      AddCascadeButton (char* name, char mnemonic=0);

   inline WXmCascadeButton
      AddCascadeMenu (char* name, char mnemonic, WXmPulldownMenu& menu) {
	 WXmCascadeButton cb(AddCascadeButton(name, mnemonic));
	 cb.SubMenuId(menu);
	 return cb;
   }

   inline WXmCascadeButton
      AddCascadeMenu (char* name, WXmPulldownMenu& menu) {
	 WXmCascadeButton cb(AddCascadeButton(name));
	 cb.SubMenuId(menu);
	 return cb;
   }
};

#endif
