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
/*
 * $XConsortium: HelpAgent.C /main/16 1996/11/26 12:49:50 cde-hal $
 *
 * Copyright (c) 1993 HAL Computer Systems International, Ltd.
 * All rights reserved.  Unpublished -- rights reserved under
 * the Copyright Laws of the United States.  USE OF A COPYRIGHT
 * NOTICE IS PRECAUTIONARY ONLY AND DOES NOT IMPLY PUBLICATION
 * OR DISCLOSURE.
 * 
 * THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE
 * SECRETS OF HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.  USE,
 * DISCLOSURE, OR REPRODUCTION IS PROHIBITED WITHOUT THE
 * PRIOR EXPRESS WRITTEN PERMISSION OF HAL COMPUTER SYSTEMS
 * INTERNATIONAL, LTD.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject
 * to the restrictions as set forth in subparagraph (c)(l)(ii)
 * of the Rights in Technical Data and Computer Software clause
 * at DFARS 252.227-7013.
 *
 *          HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.
 *                  1315 Dell Avenue
 *                  Campbell, CA  95008
 * 
 */

#define C_TOC_Element
#define L_Basic

#define C_WindowSystem
#define L_Other

#define C_HelpAgent
#define L_Agents

#define C_MessageMgr
#define C_EnvMgr
#define C_LibraryMgr
#define L_Managers

#include "Prelude.h"

#include "Other/XmStringLocalized.hh"
#include "Managers/CatMgr.hh"

#include "Registration.hh"

// HGL Help include file: 
// #include "Help/Help.h"

#include "WWL/WXmPrimitive.h"
#include "WWL/WXmManager.h"
#include "WWL/WXmPushButton.h"
#include "WWL/WXmPushButtonGadget.h"
#include <X11/cursorfont.h>

//#include <Dt/HelpQuickD.h>
#include <Dt/Help.h>
#include <Dt/HelpDialog.h>
#include <stdio.h>
#include <iostream>
using namespace std;

#define CLASS HelpAgent
#include "create_macros.hh"

#include "external-api/olias.h"

/******** The onitem cursor (32x32, xbm format) ********/
#define onitem32_width 32
#define onitem32_height 32
#define onitem32_x_hot 0
#define onitem32_y_hot 0
static unsigned char onitem32_bits[] = {
   0xff, 0xff, 0xff, 0xff, 0xfd, 0xff, 0x1f, 0xfc, 0xf9, 0xff, 0xe7, 0xf3,
   0xf1, 0xff, 0xfb, 0xef, 0xe1, 0xff, 0xfd, 0xdf, 0xc1, 0xff, 0xfd, 0xdf,
   0x83, 0xff, 0xfe, 0xbf, 0x03, 0xff, 0x7e, 0x7e, 0x03, 0xfe, 0xbe, 0x7d,
   0x03, 0xfc, 0xbe, 0x7d, 0x03, 0xf0, 0xc1, 0x7d, 0x03, 0xe0, 0xff, 0x7e,
   0x07, 0xc0, 0x7f, 0xbf, 0x07, 0x80, 0xbf, 0xbf, 0x07, 0x00, 0xde, 0xdf,
   0x07, 0x00, 0xdc, 0xef, 0x07, 0x00, 0xdf, 0xf7, 0x07, 0x80, 0xdf, 0xfb,
   0x0f, 0xc0, 0xdf, 0xfb, 0x0f, 0xc0, 0xdf, 0xfb, 0x0f, 0x81, 0xdf, 0xfb,
   0xcf, 0x83, 0x3f, 0xfc, 0xef, 0x07, 0xff, 0xff, 0xff, 0x07, 0xff, 0xff,
   0xff, 0x0f, 0x3e, 0xfc, 0xff, 0x0f, 0xde, 0xfb, 0xff, 0x1f, 0xdc, 0xfb,
   0xff, 0x1f, 0xdc, 0xfb, 0xff, 0x3f, 0xd8, 0xfb, 0xff, 0x3f, 0x3c, 0xfc,
   0xff, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

#define onitem32_m_width 32
#define onitem32_m_height 32
#define onitem32_m_x_hot 0
#define onitem32_m_y_hot 0
static unsigned char onitem32_m_bits[] = {
   0x03, 0x00, 0x00, 0x00, 0x07, 0x00, 0xe0, 0x03, 0x0f, 0x00, 0xf8, 0x0f,
   0x1f, 0x00, 0xfc, 0x1f, 0x3f, 0x00, 0xfe, 0x3f, 0x7f, 0x00, 0xfe, 0x3f,
   0xfe, 0x00, 0xff, 0x7f, 0xfe, 0x01, 0xff, 0xff, 0xfe, 0x03, 0x7f, 0xfe,
   0xfe, 0x0f, 0x7f, 0xfe, 0xfe, 0x1f, 0x3e, 0xfe, 0xfe, 0x3f, 0x00, 0xff,
   0xfc, 0x7f, 0x80, 0x7f, 0xfc, 0xff, 0xc1, 0x7f, 0xfc, 0xff, 0xe3, 0x3f,
   0xfc, 0xff, 0xe7, 0x1f, 0xfc, 0xff, 0xe3, 0x0f, 0xfc, 0xff, 0xe0, 0x07,
   0xf8, 0x7f, 0xe0, 0x07, 0xf8, 0x7f, 0xe0, 0x07, 0xf8, 0xff, 0xe0, 0x07,
   0xf8, 0xfe, 0xc0, 0x03, 0x38, 0xfc, 0x01, 0x00, 0x18, 0xfc, 0x01, 0x00,
   0x00, 0xf8, 0xc3, 0x03, 0x00, 0xf8, 0xe3, 0x07, 0x00, 0xf0, 0xe7, 0x07,
   0x00, 0xf0, 0xe7, 0x07, 0x00, 0xe0, 0xef, 0x07, 0x00, 0xe0, 0xc7, 0x03,
   0x00, 0xc0, 0x03, 0x00, 0x00, 0x80, 0x00, 0x00};

#define NULL_LOCATORID "__HaL__DefaultLocatorId__"

HelpAgent *HelpAgent::g_help_agent;

// /////////////////////////////////////////////////////////////////
// class constructor
// /////////////////////////////////////////////////////////////////

HelpAgent::HelpAgent()
: f_helper (NULL)
{
  Widget app_shell = window_system().toplevel();
  f_cursor = create_help_cursor(app_shell);
}


// /////////////////////////////////////////////////////////////////
// class destructor
// /////////////////////////////////////////////////////////////////

HelpAgent::~HelpAgent()
{
  g_help_agent = NULL;
}


// /////////////////////////////////////////////////////////////////
// create_ui
// /////////////////////////////////////////////////////////////////

void
HelpAgent::create_ui()
{
  Arg args[2];
  int n;
  Widget app_shell = window_system().toplevel();
  f_appXrmDb = XrmGetDatabase (XtDisplay (app_shell));

  n = 0;
  XtSetArg(args[n], XmNtitle, "Dtinfo Help"); n++;
  f_helper = DtCreateHelpDialog(app_shell,
                                (char*)"helpdialog",
                                args, n);
}


// /////////////////////////////////////////////////////////////////
// display_help
// /////////////////////////////////////////////////////////////////
void
HelpAgent::display_help (Widget w)
{
  char* locator_id=get_locator_id(w);
  if(locator_id == NULL)
  {
    message_mgr().error_dialog ((char*)UAS_String(
                  CATGETS(Set_Messages, 3, "No help available")));
    return;
  }

  Wait_Cursor bob;

  if (f_helper == NULL)
    create_ui();
  XtVaSetValues(f_helper,
                DtNhelpType, DtHELP_TYPE_TOPIC,
                DtNhelpVolume, "Infomgr",
                DtNlocationId, locator_id,
                XmNdialogStyle, XmDIALOG_MODELESS,
                NULL);

  XtManageChild(f_helper);
  Popup();
}

void
HelpAgent::display_help (const String locatoridResourceString)
{
  Wait_Cursor bob;

  if (f_helper == NULL)
    create_ui();

  if (f_appXrmDb)
  {
    char* type;
    XrmValue value;

    if(env().debug())
      cerr << "Resource string: " << locatoridResourceString << endl;
    if(XrmGetResource(f_appXrmDb, locatoridResourceString,
                        locatoridResourceString, &type, &value ))
    {
      if(env().debug())
        cerr << "Value: " << value.addr << endl;
      XtVaSetValues(f_helper,
                    DtNhelpType, DtHELP_TYPE_TOPIC,
                    DtNhelpVolume, "Infomgr",
                    DtNlocationId, value.addr,
                    XmNdialogStyle, XmDIALOG_MODELESS,
                    NULL);
      XtManageChild(f_helper);
      Popup();
    }
    else
    {
      message_mgr().error_dialog (UAS_String(
                    CATGETS(Set_Messages, 3, "No help available")));
    }
  }
}

void
HelpAgent::Popup()
{
  Widget app_shell = window_system().toplevel();
  Widget parent = XtParent(f_helper);
  //XRaiseWindow(XtDisplay(parent), XtWindow(parent));
  UAS_List<UAS_String> env_infolibs(env().infolibs());
  library_mgr().init(env_infolibs);

  XMapRaised(XtDisplay(app_shell), XtWindow(app_shell));
  XMapRaised(XtDisplay(parent), XtWindow(parent));
}


// /////////////////////////////////////////////////////////////////
// Help Callback Functions
// /////////////////////////////////////////////////////////////////
// This provides F1 help support

static void
helpCB(Widget w, XtPointer, XtPointer)
{
  help_agent().help_cb(w);
}

void
HelpAgent::add_help_cb (Widget w)
{
  XtAddCallback(w, XmNhelpCallback, helpCB, NULL);
}

void
HelpAgent::help_cb (Widget w)
{
  display_help (w);
}


// /////////////////////////////////////////////////////////////////
// Context Help functions
// /////////////////////////////////////////////////////////////////
// This provides context-sensitive help

static void
contextHelpCB(Widget w, XtPointer, XtPointer)
{
  help_agent().context_help(w);
}

void
HelpAgent::add_context_help (Widget w)
{
  XtAddCallback(w, XmNactivateCallback, contextHelpCB, NULL);
}

void
HelpAgent::context_help(Widget widget)
{
  while(!XtIsSubclass(widget, topLevelShellWidgetClass))
    widget = XtParent(widget);

  if (widget != NULL)
  {
    XEvent event;
    Widget sel_widget = XmTrackingEvent(widget, f_cursor, False, &event);
    if (sel_widget != NULL)
      display_help(sel_widget);
  }
}

// /////////////////////////////////////////////////////////////////
// Activate Help functions
// /////////////////////////////////////////////////////////////////
// This provides help for help buttons and help menu items.
// Widgets that register an activate callback also need a help
// callback.

static void
activateHelpCB(Widget w, XtPointer client_data, XtPointer)
{
  help_agent().activate_help(w, (String)client_data);
}

void
HelpAgent::add_activate_help (Widget w, const String locator_id)
{
  XtAddCallback(w, XmNactivateCallback, activateHelpCB, (XtPointer)locator_id);
  add_help_cb(w);
}

void
HelpAgent::add_activate_help (WXmPushButton& w, const String locator_id)
{
  XtAddCallback((Widget)w, XmNactivateCallback, activateHelpCB, (XtPointer)locator_id);
}

void
HelpAgent::add_activate_help (WXmPushButtonGadget& w, const String locator_id)
{
  XtAddCallback((Widget)w, XmNactivateCallback, activateHelpCB, (XtPointer)locator_id);
}

void
HelpAgent::activate_help (Widget w, String locator_id)
{
  if (locator_id != NULL)
    display_help(locator_id);
  else
    display_help (w);
}

#if 0
void HelpAgent::add_context_help (WXmPrimitive &w)
{
  w.SetHelpCallback (this, (WWL_FUN) &HelpAgent::context_help_cb);
}

void HelpAgent::add_context_help (WXmManager &w)
{
  w.SetHelpCallback (this, (WWL_FUN) &HelpAgent::context_help_cb);
}

void
HelpAgent::add_context_help(WXmPushButton &pb, int context)
{
  pb.SetHelpCallback (this, (WWL_FUN) &HelpAgent::context_help_cb);
  //pb.SetActivateCallback (this, (WWL_FUN) &HelpAgent::context_help_cb,
//			  (XtPointer) context);
}

void
HelpAgent::add_context_help(WXmPushButtonGadget &pb, int context)
{
  pb.SetActivateCallback (this, (WWL_FUN) &HelpAgent::context_help_cb,
			  (XtPointer) context);
}

void
HelpAgent::add_activate_help (WXmPushButton &pb, const String card_id)
{
  pb.SetActivateCallback (this, (WWL_FUN) &HelpAgent::help_cb,
			  (void *) card_id);
}

void
HelpAgent::add_activate_help (WXmPushButtonGadget &pbg, const String card_id)
{
  pbg.SetActivateCallback (this, (WWL_FUN) &HelpAgent::help_cb,
			   (void *) card_id);
}

void
HelpAgent::add_activate_help (Widget w, const String card_id)
{
  pbg.SetActivateCallback (this, (WWL_FUN) &HelpAgent::help_cb,
			   (void *) card_id);
}

#endif
char*
HelpAgent::get_locator_id(const Widget w)
{
   XtResource   res;
   //res.resource_name   = (resourceName ? (String)resourceName : "helpcard");
   res.resource_name   = (char*)"helpcard";
   res.resource_class  = XtCString;
   res.resource_type   = XtRString;
   res.resource_size   = sizeof(String);
   res.resource_offset = 0;
   res.default_type    = XtRString;
   res.default_addr    = (void*)NULL_LOCATORID;

   String       string;
   XtGetApplicationResources(w, &string, &res, 1, NULL, 0);

   if(env().debug())
   {
      UAS_String wname = XtName(w);
      UAS_String dot(".");
      Widget parent = XtParent(w);
      while(parent != NULL)
      {
         UAS_String Parent(XtName(parent));
         wname = Parent + dot + wname;   
         parent = XtParent(parent);
      }
      cerr << "Widget: " << (char*)wname << endl;
   }

   if ( strcmp(string, NULL_LOCATORID)==0 )
   {  
      // Locator id not found, look for it in a parent
      //cerr << "Locator id not found, searching parent" << endl;
      Widget parent = XtParent(w);
      while(parent != NULL)
      {
        XtGetApplicationResources(parent, &string, &res, 1, NULL, 0);
        if ( strcmp(string, NULL_LOCATORID)==0 )     // Not found
          parent = XtParent(parent);
        else
        {
          if(env().debug())
            cerr << "Parent Locator is: " << string << endl;
          return string;
        }
      }
      return NULL;
    }

   if(env().debug())
     cerr << "Locator is: " << string << endl;
   return string;
}

Cursor
HelpAgent::create_help_cursor(Widget parent)
{
  Display     *display = XtDisplay(parent);
  Screen      *retScr = XtScreen(parent);
  int          screen = XScreenNumberOfScreen(retScr);
  char        *bits;
  char        *maskBits;
  unsigned int width;
  unsigned int height;
  unsigned int xHotspot;
  unsigned int yHotspot;
  Pixmap       pixmap;
  Pixmap       maskPixmap;
  XColor       xcolors[2];
  Cursor      cursor;

  width    = onitem32_width;
  height   = onitem32_height;
  bits     = (char *) onitem32_bits;
  maskBits = (char *) onitem32_m_bits;
  xHotspot = onitem32_x_hot;
  yHotspot = onitem32_y_hot;

  pixmap = XCreateBitmapFromData (display,
             RootWindowOfScreen(XtScreen(parent)), bits,
             width, height);


  maskPixmap = XCreateBitmapFromData (display,
                 RootWindowOfScreen(XtScreen(parent)), maskBits,
                 width, height);

  xcolors[0].pixel = BlackPixelOfScreen(ScreenOfDisplay(display, screen));
  xcolors[1].pixel = WhitePixelOfScreen(ScreenOfDisplay(display, screen));

  XQueryColors (display,
    DefaultColormapOfScreen(ScreenOfDisplay(display, screen)), xcolors, 2);

  cursor = XCreatePixmapCursor (display, pixmap, maskPixmap,
                       &(xcolors[0]), &(xcolors[1]),
                       xHotspot, yHotspot);
  XFreePixmap (display, pixmap);
  XFreePixmap (display, maskPixmap);

  return cursor;

}
