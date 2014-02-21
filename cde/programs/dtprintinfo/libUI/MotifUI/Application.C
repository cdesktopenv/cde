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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $TOG: Application.C /main/9 1998/07/24 16:14:41 mgreess $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include "Application.h"
#include <sys/stat.h>
#include <Xm/Protocols.h>

#ifndef NO_CDE
extern "C" {
#include <Dt/Dt.h>
#include <Dt/EnvControlP.h>
#include <Dt/DbUtil.h>
#include <Dt/Action.h>
#include <Dt/Session.h>
}
#endif

#include <stdlib.h>  // for putenv function

const char *SESSION_FLAG = "-session";
const char *SESSION_NAME = "session";
const char *SESSION_CLASSNAME = "Session";
const char *DEFAULT_FONT = "fixed";

typedef struct
{
   XmFontList user_font;
   XmFontList system_font;
   char *session_file;
   Dimension shadow_thickness;
} ApplicationArgs, *ApplicationArgsPtr;
    
static XtResource resources[] =
{
   {"userFont", "XmCFontList", XmRFontList, sizeof(XmFontList), 
    XtOffsetOf (ApplicationArgs, user_font), XmRString,
#ifdef aix
    (XtPointer) DEFAULT_FONT
#else
    (XtPointer) "fixed"
#endif
   },
   {"systemFont", "XmCFontList", XmRFontList, sizeof(XmFontList), 
    XtOffsetOf (ApplicationArgs, system_font), XmRString,
#ifdef aix
    (XtPointer) DEFAULT_FONT
   },
   {(char *)SESSION_NAME, (char *)SESSION_CLASSNAME, XtRString, sizeof(char *), 
#else
    (XtPointer) "fixed"
   },
   {"session", "Session", XtRString, sizeof(char *), 
#endif
    XtOffsetOf (ApplicationArgs, session_file), XmRString, (XtPointer) NULL,
   },
   {XmNshadowThickness, XmCShadowThickness, XmRHorizontalDimension,
    sizeof (Dimension), XtOffsetOf (ApplicationArgs, shadow_thickness),
    XmRImmediate, (XtPointer) 1
   } 
};

static XrmOptionDescRec options[] =
{
#ifdef aix
   {(char *)SESSION_FLAG, (char *)SESSION_NAME, XrmoptionSepArg, NULL}
#else
   {"-session", "session", XrmoptionSepArg, NULL}
#endif
};

extern "C" {
   extern Boolean XmeRenderTableGetDefaultFont(XmFontList, XFontStruct **);
} 

Application::Application(char *name,
                         char *appClassName,
                         int *_argc,
			 char **_argv)
	: MotifUI(NULL, name, appClassName, appClassName)
{
#ifndef NO_CDE
   _DtEnvControl(DT_ENV_SET);
#endif

   XtSetLanguageProc(NULL, NULL, NULL);
   _w = XtVaAppInitialize(&appContext, appClassName, options, XtNumber(options),
			  _argc, _argv, NULL, XmNallowShellResize, true,
			  XmNtitle, name, XmNiconName, name, NULL);

   topLevel = _w;
   display = XtDisplay(_w);
   root = RootWindowOfScreen(XtScreen(_w));
   white = WhitePixelOfScreen(XtScreen(_w));
   black = BlackPixelOfScreen(XtScreen(_w));
   depth = DefaultDepthOfScreen(XtScreen(_w));
   int numMouseButtons = XGetPointerMapping(display, (unsigned char *)NULL, 0);
   bMenuButton = (numMouseButtons < 3) ? Button2 : Button3;
   ApplicationArgs application_args;
   XtGetApplicationResources(_w, &application_args, resources,
			     XtNumber(resources), NULL, 0);
   userFont = application_args.user_font;
   userFont = application_args.system_font;
   session_info = NULL;
   attributes = NULL;
   values = NULL;
   n_attrs = 0;
   session_path = NULL;
   fp = NULL;
   if (session_file = application_args.session_file)
    {
      if (*session_file == '/')
	 session_path = strdup(session_file);
      else
         DtSessionRestorePath(topLevel, &session_path, session_file);
    }
      
   shadowThickness = application_args.shadow_thickness;
   {
     XmFontContext context;
     if (XmFontListInitFontContext(&context, userFont))
       {
	 XmFontListEntry entry = XmFontListNextEntry(context);
	 if (entry)
	   {
	     XmFontType _type_return;
	     fs = (XFontStruct *)XmFontListEntryGetFont(entry, &_type_return);
	     if (_type_return != XmFONT_IS_FONT) {
	       XmeRenderTableGetDefaultFont(userFont, &fs);
	     }
	     if (fs) {
	       font = fs->fid;
	     }
	   }
	 XmFontListFreeFontContext(context);
       }
   }

   argc = *_argc;
   argv = _argv;

   InstallDestroyCB();

   // If the user specified -display on the command line, we need 
   // to set the environment variable DISPLAY to this value so that
   // any aixterms or X applications launched from this program
   // go to the same display
   new_display = new char [strlen("DISPLAY=") +
                           strlen(DisplayString(display)) + 1];
   sprintf(new_display, "DISPLAY=%s", DisplayString(display));
   putenv(new_display);

   // Make the environment variable ENV set to nothing.  This is so that
   // we don't run the user's ENV every time we open a child process.
   putenv("ENV=");

#ifndef NO_CDE
   if (!DtAppInitialize(appContext, display, topLevel, argv[0], appClassName))
     {
      // Fatal Error: could not connect to the messaging system.
      // DtAppInitialize() has already logged an appropriate error msg
      exit(-1);
    }
   // Load the filetype/action dbs; DtInvokeAction() requires this 
   DtDbLoad();
#endif

   Atom xa_WM_SAVE_YOURSELF = XInternAtom(display, "WM_SAVE_YOURSELF", False);
   XmAddWMProtocols(topLevel, &xa_WM_SAVE_YOURSELF, 1);
   XmAddWMProtocolCallback(topLevel, xa_WM_SAVE_YOURSELF, SaveSessionCB,
			   (XtPointer)this);
   XmAddWMProtocolCallback(topLevel,
			   XmInternAtom(display, "WM_DELETE_WINDOW", False),
			   CloseCB, (XtPointer)this);
}

Application::~Application()
{
   if (userFont)
      XmFontListFree(userFont);
   delete session_info;
   delete attributes;
   delete values;
   delete new_display;
}

boolean Application::SetName(char *name)
{	
   if (!_w)
      return false;

   XtVaSetValues(_w, XmNtitle, name, XmNiconName, name, NULL);

   return true;
}

boolean Application::SetVisiblity(boolean flag)
{	
   if (!_w)
      return false;
   
   if (flag)
    {
      XtRealizeWidget(_w);
      XtVaSetValues(_w, XmNallowShellResize, False, NULL);
    }

   return true;
}

void Application::Run()
{	
   XtAppMainLoop(appContext);
}

void Application::SaveSessionCB(Widget, XtPointer client_data, XtPointer)
{
   Application *obj = (Application *)client_data;
   obj->SaveMe(true);
}

void Application::CloseCB(Widget, XtPointer client_data, XtPointer)
{
   Application *obj = (Application *)client_data;
   obj->SaveMe();
}

void Application::SaveMe(boolean save_as_session)
{
   char *path = NULL, *name = NULL;

   if (save_as_session)
      DtSessionSavePath(topLevel, &path, &name);
   else
    {
      name = SessionFile();
      char *path1 = SessionPath();
      path = new char[strlen(path1) + strlen(name) + 2];
      sprintf(path, "%s/%s", path1, name);
      name = NULL;
    }
   if (path && (fp = fopen(path, "w")))
    {
      SaveYourSelf();
      fflush(fp);
      fclose(fp);
      fp = NULL;
    }


   if (save_as_session)
    {
      char **new_argv = NULL;
      char **_argv;
      int _argc;

      XtVaGetValues(topLevel, XmNargc, &_argc, XmNargv, &_argv, NULL);

      int i;
      if (path && name)
       {
         if (_argc > 2 && !STRCMP(SESSION_FLAG, _argv[1]))
	  {
            new_argv = new char *[_argc + 1];
            for (i = 0; i < _argc; i++)
               new_argv[i] = _argv[i];
            new_argv[2] = name;
            new_argv[i] = NULL;
	  }
         else
          {
            int j;

            new_argv = new char *[_argc + 3];
            new_argv[0] = _argv[0];
            new_argv[1] = (char *)SESSION_FLAG;
            new_argv[2] = name;
            for (i = 1, j = 3; i < _argc; i++, j++)
               new_argv[j] = _argv[i];
            new_argv[j] = NULL;
            _argc += 2;
          }
       }
      else
       {
         new_argv = new char *[_argc + 1];
         for (i = 0; i < _argc; i++)
            new_argv[i] = _argv[i];
         new_argv[i] = NULL;
       }
      XSetCommand(display, XtWindow(topLevel), new_argv, _argc);
      delete [] new_argv;
    }
   if (path)
      XtFree(path);
   if (name)
      XtFree(name);
}

void Application::Save(char *attribute, char *value)
{
   if (fp)
      fprintf(fp, "%s\n%s\n", attribute, value);
}

char *Application::Restore(char *attribute)
{
   char *value = NULL;
   if (!session_info)
    {
      if (!session_path)
       {
         char *name = SessionFile();
         char *path = SessionPath();
         session_path = new char[strlen(path) + strlen(name) + 2];
         sprintf(session_path, "%s/%s", path, name);
	 session_file = strdup(session_path);
       }
      struct stat statbuff;
      if (stat(session_path, &statbuff) != -1)
       {
	 FILE *fp1;
	 if (fp1 = fopen(session_path, "r"))
	  {
	    session_info = new char[statbuff.st_size + 1];
	    fread(session_info, (unsigned int)statbuff.st_size, 1, fp1);
	    fclose(fp1);
	    session_info[statbuff.st_size] = '\0';
            char *s, *s1 = session_info;
            s = s1;
            while (s && (s = strchr(s1, '\n')))
             {
               n_attrs++;
               s1 = s + 1;
             }

	    n_attrs /= 2;
            attributes = new char*[n_attrs];
            values = new char*[n_attrs];
	    s = strtok(session_info, "\n");
	    int i = 0;
            while (s && *s)
	     {
	       attributes[i] = s;
	       values[i] = strtok(NULL, "\n");
	       s = strtok(NULL, "\n");
	       i++;
	     }
	  }
       }
      if (!session_info)
	 session_info = strdup("");
    }
   int i;
   for (i = 0; i < n_attrs; i++)
      if (!STRCMP(attributes[i], attribute))
       {
         value = values[i];
         break;
       }
   return value;
}
