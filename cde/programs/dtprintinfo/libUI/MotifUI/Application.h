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
