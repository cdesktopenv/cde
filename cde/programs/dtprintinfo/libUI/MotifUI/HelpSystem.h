/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: HelpSystem.h /main/3 1995/11/06 09:40:54 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef HELP_SYSTEM_H
#define HELP_SYSTEM_H

#include "MotifUI.h"

typedef enum
{
   HELP_DIALOG,
   QUICK_HELP
} HelpStyle;

class HelpSystem : public MotifUI {

   friend void HyperlinkCB(Widget, XtPointer, XtPointer);
   friend void CloseCB(Widget, XtPointer, XtPointer);

 private:

   static void HyperlinkCB(Widget, XtPointer, XtPointer);
   static void CloseCB(Widget, XtPointer, XtPointer);
   void CreateHelpDialog(MotifUI *, char *, char *, char *, HelpStyle);
   boolean SetVisiblity(boolean flag);

   HelpStyle _style;

 public:

   HelpSystem(MotifUI * parent,
              char * name,
              char * volume = NULL,
              char * location_id = "_hometopic",
              HelpStyle style = HELP_DIALOG);
   HelpSystem(char *category,
              MotifUI * parent,
              char * name,
              char * volume = NULL,
              char * location_id = "_hometopic",
              HelpStyle style = HELP_DIALOG);

   char *HelpVolume();
   char *LocationID();
   void HelpVolume(char *volume,
		   char *location = "_hometopic");
   void LocationID(char *location);

   const UI_Class UIClass()           { return HELP_SYSTEM; }
   const int UISubClass()             { return _style; }
   const char *const UIClassName()    { return "HelpSystem"; }

};

#endif // HELP_SYSTEM_H
