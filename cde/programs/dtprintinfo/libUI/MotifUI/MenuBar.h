/* $XConsortium: MenuBar.h /main/3 1995/11/06 09:43:42 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef MENUBAR_H
#define MENUBAR_H

#include "MotifUI.h"

class Menu;

class MenuBar : public MotifUI {

 public:

   MenuBar(MotifUI *parent, 
           char *name = "MenuBar");
   virtual ~MenuBar();
   void SetHelpMenu(Menu *); 

   const UI_Class UIClass()        { return MENU_BAR; }
   const char *const UIClassName() { return "MenuBar"; }

};

#endif /* MENUBAR_H */
