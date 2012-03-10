/* $XConsortium: Menu.h /main/3 1995/11/06 09:43:09 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef MENU_H
#define MENU_H

#include "MotifUI.h"

class Menu : public MotifUI {

   static void MapCB(Widget, XtPointer, XtPointer);
   friend void MapCB(Widget, XtPointer, XtPointer);

 protected:

   Widget _cascadeButton;
   Widget _title;
   Widget _sep;
   MenuType _menu_type;

   void CreateMenu(Widget parent, char *name, char *category,
		   char *mnemonic, MenuType menu_type);
   void CreateTitle(Widget, char *title, char *category);
   boolean SetName(char *name);

 public:

   Menu(char *category,
        MotifUI *parent,
	char *name,
	char *mnemonic = NULL,
	MenuType menu_type = PULLDOWN_MENU);
   Menu(char *category,
	MotifUI *parent,
	boolean has_title,
        char *title,
	char *name,
	char *mnemonic = NULL,
	MenuType menu_type = PULLDOWN_MENU);
   Menu(MotifUI *parent,
	char *name,
	char *mnemonic = NULL,
	MenuType menu_type = PULLDOWN_MENU);
   Menu(MotifUI *parent,
	boolean has_title,
	char *title,
	char *name,
	char *mnemonic = NULL,
	MenuType menu_type = PULLDOWN_MENU);

   void SetRadio(boolean flag);
   Widget GetCascade() { return _cascadeButton; }
   boolean SetActivity(boolean flag);

   const UI_Class UIClass()         { return MENU; }
   const int UISubClass()           { return _menu_type; }
   const char *const UIClassName()  { return "Menu"; }

};

#endif /* MENU_H */
