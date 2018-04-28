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
/* $XConsortium: Application.h /main/3 1995/11/06 09:38:56 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef APPLICATION_H
#define APPLICATION_H

#include "MotifUI.h"
#include <stdio.h>

class Application : public MotifUI {

   friend int main(int, char **);
   friend void SaveSessionCB(Widget, XtPointer, XtPointer);
   friend void CloseCB(Widget, XtPointer, XtPointer);

 protected:

   static void SaveSessionCB(Widget, XtPointer, XtPointer);
   static void CloseCB(Widget, XtPointer, XtPointer);
   XFontStruct *fs;
   FILE *fp;
   char **attributes;
   char **values;
   int n_attrs;
   char *session_info;
   char *session_file;
   char *session_path;
   char *new_display;
   virtual void Run();

 public:

   int argc;
   char **argv;

   Application(char *name,
	       char *className,
	       int *argc,
	       char **argv);
   virtual ~Application();

   boolean SetVisiblity(boolean);
   boolean SetName(char *);
   virtual void SaveYourSelf() { }
   virtual char *SessionFile() { return NULL; }
   virtual char *SessionPath() { return NULL; }
   void Save(char *attribute, char *value);
   char *Restore(char *attribute);
   void SaveMe(boolean save_as_session = false);

   const UI_Class UIClass()        { return APPLICATION; }

   const char *const UIClassName() { return "Application"; }

};

#endif /* APPLICATION_H */
