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
 * $TOG: GraphicAgentMotif.C /main/30 1998/04/17 11:32:58 mgreess $
 *
 * Copyright (c) 1992 HAL Computer Systems International, Ltd.
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

#ifdef JBM
#include <tml/tml_objects.hxx>
#endif

#define C_TOC_Element
#define L_Basic

#define C_PixmapGraphic
#define C_PostScript
#define L_Graphics

#define C_MessageMgr
#define C_GraphicsMgr
#define C_NodeMgr
#define C_GhostScriptEventMgr
#define L_Managers

#define C_HelpAgent
#define C_GraphicAgent
#define L_Agents

#include <WWL/WTopLevelShell.h>
#include <WWL/WXmMenu.h>
#include <WWL/WXmForm.h>
#include <WWL/WXmPushButton.h>
#include <WWL/WXmToggleButton.h>
#include <WWL/WXmFrame.h>
#include <WWL/WXmSeparator.h>

#include "Managers/CatMgr.hh"
#include "Other/XmStringLocalized.hh"

#include <Prelude.h>
#include <iostream>
using namespace std;
#include <string.h>
#include <stdio.h>

#include "Registration.hh"

#include "Widgets/WXawPorthole.h"
#include "Widgets/WXawPanner.h"
#include "Widgets/WPixmap.h"

#include "utility/funcs.h"

#define CLASS GraphicAgent
#include "create_macros.hh"

enum { PANNER_UPPER_LEFT,
       PANNER_UPPER_RIGHT,
       PANNER_LOWER_RIGHT,
       PANNER_LOWER_LEFT,
       PANNER_NONE,
       PANNER_NUM_STATES };

inline int
min(int i1, int i2)
{
  return i1 < i2 ? i1 : i2 ;
}

// /////////////////////////////////////////////////////////////////////////
// Constructor
// /////////////////////////////////////////////////////////////////////////

GraphicAgent::GraphicAgent (UAS_Pointer<UAS_Common> &node_ptr,
			    UAS_Pointer<Graphic> &gr)
: f_shell (NULL),
  f_node_ptr(node_ptr),
  f_graphic (gr),
  f_panner_state (PANNER_NONE),
  f_current_scale (100),
  f_scale_button(NULL),
  f_setcustom(0)

{
  f_graphic->pixmap_graphic()->agent(this);

  UAS_Common::request ((UAS_Receiver<UAS_LibraryDestroyedMsg> *) this);
}

// /////////////////////////////////////////////////////////////////////////
// Destructor
// /////////////////////////////////////////////////////////////////////////

GraphicAgent::~GraphicAgent()
{
  f_graphic->pixmap_graphic()->agent(NULL);
  Pixmap pixmap = f_pixmap_widget->Picture();
  if (pixmap && pixmap != f_graphic->pixmap_graphic()->pixmap())
    {
      XFreePixmap(XtDisplay(*f_pixmap_widget),
		  pixmap);	// a scaled pixmap...give it back 

    }
  if(f_shell != 0)
    f_shell.Destroy();
}

// /////////////////////////////////////////////////////////////////////////
// popup
// /////////////////////////////////////////////////////////////////////////

void 
GraphicAgent::popup()
{
  if (f_shell == 0)
    create_ui();

  f_shell.Popup();
  f_shell.DeIconify();
}

// /////////////////////////////////////////////////////////////////////////
// display
// /////////////////////////////////////////////////////////////////////////

void 
GraphicAgent::display()
{
  if (f_shell == 0)
    create_ui();
    
  // assume all graphics are scalable for now.
  bool scalable;
  scalable = True;
  if(scalable)
  {
    Widget *kids = f_view_menu->Children() ;
    int num_children = f_view_menu->NumChildren();
    for (int i = 0 ; i < num_children; i++)
    {
      WXmPrimitive prim(kids[i]);
      prim.SetSensitive(True);
    }
  }
  popup();
}


void
GraphicAgent::receive(DisplayGraphic &message, void* /*client_data*/)
{
  ON_DEBUG(cerr << "GraphicAgent::receive(DisplayGraphic( " <<
	   message.graphic()->locator() << "))" << endl);

  ON_DEBUG(cerr << "local locator is " <<
	   f_graphic->locator() << endl);

  if (message.graphic() == f_graphic)
    {
      ON_DEBUG(cerr << "GraphicAgent::receive(DISPLAY)" << endl);
      display();
    }
}

void
GraphicAgent::receive(ReAttachGraphic &message, void* /*client_data*/)
{
  //reattach();
}

void
GraphicAgent::receive (UAS_LibraryDestroyedMsg &msg, void* /*client_data*/)
{
  if(f_node_ptr->lid() == (msg.fLib)->lid())
  {
    graphics_mgr().remove_detached((GraphicAgent*)this);
    delete this;
  }
}
// /////////////////////////////////////////////////////////////////////////
// create ui
// /////////////////////////////////////////////////////////////////////////

#define AM WAutoManage
void
GraphicAgent::create_ui()
{
  XmStringLocalized mtfstring;
  String	    string;

  f_shell = WTopLevelShell (window_system().toplevel(), WPopup, "graphic");
  window_system().register_shell (&f_shell);

  string = CATGETS(Set_GraphicAgent, 1, "Dtinfo: Detached Graphic");
  XtVaSetValues((Widget)f_shell, XmNtitle, string, NULL);

  // just use "delete_response" instead
  // add a destroy callback to nuke the agent when widget is destroyed.
  f_shell.SetDestroyCallback (this, (WWL_FUN) &GraphicAgent::ui_destroyed);

  DECL  (WXmForm,           form,         f_shell,      "form"         );
  DECLMC(WXmMenuBar,        menu_bar,     form,         "menu_bar"     );
#ifdef hpux
  //
  //  rtp: hard-wire the menu bar's background, since
  //       it comes up a bogus color. Copied from swm's
  //       change in NodeWindowAgentMotif.C
  //
  Pixel bg = form.Background();
  menu_bar.Background(bg);

#endif
  DECLM (WXmCascadeButton,  file,         menu_bar,     "file"         );
  DECLMC(WXmPulldownMenu,   file_menu,    menu_bar,     "file_menu"    );
  DECLM (WXmPushButton,     attach,       file_menu,    "attach"       );
  DECLM (WXmPushButton,     visit,        file_menu,    "visit"        );
  DECLM (WXmPushButton,     close,        file_menu,    "close"        );

  XtVaSetValues(file, XmNlabelString,
    (XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 1, "File")),
    XmNmnemonic,
    *CATGETS(Set_AgentLabel, 2, ""),
    NULL);

  XtVaSetValues(attach, XmNlabelString,
    (XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 167, "Attach")),
    XmNmnemonic,
    *CATGETS(Set_AgentLabel, 21, ""),
    NULL);

  XtVaSetValues(visit, XmNlabelString,
    (XmString)XmStringLocalized(CATGETS(Set_AgentLabel,168,"Display Section")),
    XmNmnemonic,
    *CATGETS(Set_AgentLabel, 119, ""),
    NULL);

  XtVaSetValues(close, XmNlabelString,
    (XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 12, "Close")),
    XmNmnemonic,
    *CATGETS(Set_AgentLabel, 13, ""),
    NULL);

  DECLM (WXmCascadeButton,  pan,          menu_bar,     "panner"       );
  DECLMC(WXmPulldownMenu,   panner_menu,  menu_bar,     "panner_menu"  );
  DECLM (WXmToggleButton,   none,         panner_menu,  "none"         );
  DECLM (WXmToggleButton,   up_left,      panner_menu,  "up_left"      );
  DECLM (WXmToggleButton,   up_right,     panner_menu,  "up_right"     );
  DECLM (WXmToggleButton,   low_left,     panner_menu,  "low_left"     );
  DECLM (WXmToggleButton,   low_right,    panner_menu,  "low_right"    );

  XtVaSetValues(pan, XmNlabelString,
    (XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 166, "Panner")),
    XmNmnemonic,
    *CATGETS(Set_AgentLabel, 9, ""),
    NULL);

  XtVaSetValues(none, XmNlabelString,
    (XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 178, "None")),
    XmNmnemonic,
    *CATGETS(Set_AgentLabel, 7, ""),
    NULL);

  XtVaSetValues(up_left, XmNlabelString,
    (XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 179, "Upper Left")),
    XmNmnemonic,
    *CATGETS(Set_AgentLabel, 254, ""),
    NULL);

  XtVaSetValues(up_right, XmNlabelString,
    (XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 180, "Upper Right")),
    XmNmnemonic,
    *CATGETS(Set_AgentLabel, 252, ""),
    NULL);

  XtVaSetValues(low_left, XmNlabelString,
    (XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 181, "Lower Left")),
    XmNmnemonic,
    *CATGETS(Set_AgentLabel, 113, ""),
    NULL);

  XtVaSetValues(low_right, XmNlabelString,
    (XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 182, "Lower Right")),
    XmNmnemonic,
    *CATGETS(Set_AgentLabel, 259, ""),
    NULL);

  // NOTE: some code in this module depends upon the exact ordering of items
  // view menu will only be sensitive for scalable graphics 
  DECL  (WXmCascadeButton,  view,          menu_bar,     "view"        );
  DECLMC(WXmPulldownMenu,   view_menu,     menu_bar,     "view_menu"   );
  DECLM (WXmPushButton,     fit_gr_to_win, view_menu,    "fit_window"  );
  DECLM (WXmPushButton,     fit_win_to_gr, view_menu,    "fit_graphic" );

  XtVaSetValues(view, XmNlabelString,
    (XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 4, "View")),
    XmNmnemonic,
    *CATGETS(Set_AgentLabel, 5, ""),
    NULL);

  XtVaSetValues(fit_gr_to_win, XmNlabelString,
    (XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 169, "Fit to Window")),
    XmNmnemonic,
    *CATGETS(Set_AgentLabel, 35, ""),
    NULL);


  XtVaSetValues(fit_win_to_gr, XmNlabelString,
    (XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 170, "Fit to Graphic")),
    XmNmnemonic,
    *CATGETS(Set_AgentLabel, 37, ""),
    NULL);

  DECLM (WXmSeparator,      separator,    view_menu,    "separator"    );
  DECLM (WXmToggleButton,   z_25,         view_menu,    "z_25"         );
  DECLM (WXmToggleButton,   z_50,         view_menu,    "z_50"         );
  DECLM (WXmToggleButton,   z_100,        view_menu,    "z_100"        );
  DECLM (WXmToggleButton,   z_125,        view_menu,    "z_125"        );
  DECLM (WXmToggleButton,   z_150,        view_menu,    "z_150"        );
  DECLM (WXmToggleButton,   z_200,        view_menu,    "z_200"        );
  DECLM (WXmToggleButton,   z_custom,     view_menu,    "z_custom"     );
  
  mtfstring = CATGETS(Set_AgentLabel, 171, "25 %");
  XtVaSetValues(z_25, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 172, "50 %");
  XtVaSetValues(z_50, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 173, "100 %");
  XtVaSetValues(z_100, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 174, "125 %");
  XtVaSetValues(z_125, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 175, "150 %");
  XtVaSetValues(z_150, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 176, "200 %");
  XtVaSetValues(z_200, XmNlabelString, (XmString)mtfstring, NULL);

  XtVaSetValues(z_custom, XmNlabelString,
    (XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 177, "Custom...")),
    XmNmnemonic,
    *CATGETS(Set_AgentLabel, 13, ""),
    NULL);

  z_100.Set(1);
  f_scale_button = (WXmToggleButton*)(Widget)z_100 ;
  f_custom_scale = (WXmToggleButton*)(Widget)z_custom ;

  f_view_menu = (WXmPulldownMenu*)(Widget)view_menu ;

  DECLM (WXmCascadeButton,  help,         menu_bar,      "help"        );
  DECLMC(WXmPulldownMenu,   help_menu,    menu_bar,      "help_menu"   );
  DECLM (WXmPushButton,     on_window,    help_menu,     "on_window"   );
  DECLM (WXmPushButton,     on_help,      help_menu,     "on_help"     );

  XtVaSetValues(help, XmNlabelString,
    (XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 48, "Help")),
    XmNmnemonic,
    *CATGETS(Set_AgentLabel, 39, ""),
    NULL);

  XtVaSetValues(on_window, XmNlabelString,
    (XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 147, "On Window")),
    XmNmnemonic,
    *CATGETS(Set_AgentLabel, 35, ""),
    NULL);

  XtVaSetValues(on_help, XmNlabelString,
    (XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 153, "On Help")),
    XmNmnemonic,
    *CATGETS(Set_AgentLabel, 39, ""),
    NULL);

  DECLM (WXmLabel, message_area, form, "message");
  f_message_area = (WXmLabel*)(Widget)message_area ;

  DECLM (WXmSeparator, sep, form, "sep2");

  DECL  (WXmFrame,          frame,        form,         "frame"        );
  DECL  (WXmForm,	    subform,      frame,	"subform"      );
  DECLM (WXawPanner,        panner,       subform,      "panner"       );
  DECLM (WXawPorthole,      porthole,     subform,      "porthole"     );
  DECLM (WPixmap,           pixmap,       porthole,     "pixmap"       );


  f_panner = (WXawPanner*)(Widget)panner ;
  f_pixmap_widget = (WPixmap*)(Widget)pixmap ;

  file.SubMenuId (file_menu);
  view.SubMenuId (view_menu);
  pan.SubMenuId (panner_menu);
  help.SubMenuId (help_menu);
  menu_bar.MenuHelpWidget (help);
    
  WArgList args;

  PixmapGraphic &pgraphic = *(f_graphic->pixmap_graphic());

  Graphic *graphic = f_graphic ;
  UAS_String titleStr(graphic->title());
  const char *title = titleStr ;

  if (title == NULL || *title == '\0')	// empty string 
    title = (char *)CATGETS(Set_GraphicAgent, 2, "Detached Graphic");

  {

    UAS_String buffer = CATGETS(Set_GraphicAgent, 8, "Dtinfo: ");
    buffer = buffer + title;

    f_shell.Title((char*)buffer);
    f_shell.IconName((char*)buffer);

  }

  args.Reset();
  panner.CanvasWidth (pgraphic.width(), args);
  panner.CanvasHeight (pgraphic.height(), args);
  panner.Set (args);

  SET_CALLBACK_D (porthole,Report,porthole_report,(Widget) panner);
  install_new_picture(&pgraphic);
  SET_CALLBACK_D (panner,Report,panner_report,(Widget) pixmap);

  ON_ACTIVATE (visit,visit_node);
  ON_ACTIVATE (close,close_window);
  ON_ACTIVATE (attach,reattach);

  SET_CALLBACK_D (none,ValueChanged,set_panner,PANNER_NONE);
  SET_CALLBACK_D (up_left,ValueChanged,set_panner,PANNER_UPPER_LEFT);
  SET_CALLBACK_D (up_right,ValueChanged,set_panner,PANNER_UPPER_RIGHT);
  SET_CALLBACK_D (low_left,ValueChanged,set_panner,PANNER_LOWER_LEFT);
  SET_CALLBACK_D (low_right,ValueChanged,set_panner,PANNER_LOWER_RIGHT);

  ON_ACTIVATE (fit_gr_to_win,fit_graphic_to_window_callback);
  ON_ACTIVATE (fit_win_to_gr,fit_window_to_graphic_callback);

  SET_CALLBACK_D (z_25,ValueChanged,zoom_callback,25);
  SET_CALLBACK_D (z_50,ValueChanged,zoom_callback,50);
  SET_CALLBACK_D (z_100,ValueChanged,zoom_callback,100);
  SET_CALLBACK_D (z_125,ValueChanged,zoom_callback,125);
  SET_CALLBACK_D (z_150,ValueChanged,zoom_callback,150);
  SET_CALLBACK_D (z_200,ValueChanged,zoom_callback,200);
  SET_CALLBACK_D (z_custom,ValueChanged,zoom_callback,0);

  help_agent().add_activate_help (on_window, (char*)"graphic_on_window");
  help_agent().add_activate_help (on_help,   (char*)"graphic_on_help");

  view.Manage();

  // menubars for scalable graphics 

  // assume all graphics are scalable
  bool scalable = True;
  if(scalable)
  {
    if (f_graphic->content_type() == "application/CGM")
       mtfstring = CATGETS(Set_GraphicAgent, 4, "Vector illustration (scalable)");
    else
       mtfstring = CATGETS(Set_GraphicAgent, 9, "Raster illustration (scalable)");
  }
  else
  {
#if !defined(VF_DEBUG)
      for (unsigned int i = 0 ; i < view_menu.NumChildren(); i++)
	WRect(view_menu.Children()[i]).SetSensitive(False);
#endif
    mtfstring = CATGETS(Set_GraphicAgent, 3,
		"Raster illustration (non-scalable)");
  }

#if EAM
  if (f_graphic->content_type() != "application/postscript") {

#if !defined(VF_DEBUG)
      for (int i = 0 ; i < view_menu.NumChildren(); i++)
	WRect(view_menu.Children()[i]).SetSensitive(False);
#endif
      mtfstring = CATGETS(Set_GraphicAgent, 3,
				"Raster illustration (non-scalable)");
  }
  else {
      mtfstring = CATGETS(Set_GraphicAgent, 4,
				"Vector illustration (scalable)");
  }
#endif

  XtVaSetValues((Widget)f_message_area, XmNlabelString, (XmString)mtfstring, NULL);

  // porthole.Manage();
  frame.Manage();
  subform.ShadowThickness(0);
  subform.Manage();
  form.ShadowThickness(0);
  form.Manage();

  // when panner first appears, it should be in upper-left corner
  up_left.Set (TRUE);
  adjust_panner (PANNER_UPPER_LEFT); 

  // Set Max width/height if not scalable 
  f_shell.Realize();

  
  Display *display = XtDisplay(f_shell);
  Screen      *retScr = XtScreen(f_shell);
  int screen =  XScreenNumberOfScreen(retScr);
  int dw = DisplayWidth(display, screen);
  int dh = DisplayHeight(display, screen);

  Dimension width = dw, height = dh;

  //if (f_graphic->content_type() == "application/CGM")

  // assume all graphics are scalable
  scalable = True;
  if(scalable)
  {
    // set max size of window
    height = min(height, dh - 100);
    width  = min(width,  dw - 100);

    f_shell.Set (WArgList (XmNmaxHeight, height,
			   XmNmaxWidth,  width,
			   NULL));

    // don't allow window to shrink smaller than original size
    f_shell.Get (WArgList (XmNheight, (XtArgVal) &height,
			   NULL));
    height = min(height, dh - 100);

    f_shell.Set (WArgList (XmNminHeight, height,
			     NULL));
  }
  else
  {
    f_shell.Get (WArgList (XmNheight, (XtArgVal) &height,
		     XmNwidth, &width,
		     NULL));
    height = min(height, dh - 100);
    width  = min(width,  dw - 100);

    f_shell.Set (WArgList (XmNmaxHeight, height,
		     XmNmaxWidth,  width,
		     NULL));
  }
}


void
GraphicAgent::close_window()
{
  reattach();
  f_shell.Popdown();
}

// /////////////////////////////////////////////////////////////////
// ui_destroyed - user nuked Motif window, so kill myself
// /////////////////////////////////////////////////////////////////
 
void
GraphicAgent::ui_destroyed()
{
  // Set f_shell to NULL, since Xt is destroying it for us.
  reattach();
  f_shell = NULL;
}


void
GraphicAgent::attach_graphic()
{
  reattach();
}


void
GraphicAgent::reattach()
{
  graphics_mgr().reattach_graphic(f_graphic);
//  Wait_Cursor bob;
//  UAS_String idata(*image_data);
//  PixmapGraphic *pgr = f_graphic->pixmap_graphic();

//  ON_DEBUG(cerr << "Graphic Agent reattach" << endl);
//  f_graphic->set_detached(FALSE);
//  f_graphic->
//    pixmap_graphic()->
//    UAS_Sender<ReAttachGraphic>::send_message(ReAttachGraphic(f_graphic));

//  graphics_mgr().remove_detached((GraphicAgent*)this);
//  delete this ;
}


void
GraphicAgent::visit_node()
{
  extern char g_top_locator[];
  extern bool g_scroll_to_locator;
  const char *glocator = f_graphic->locator();
  int len = 0;

  ON_DEBUG (printf ("Graphic jumping to <%s>\n", glocator));
  ON_DEBUG (printf ("  copying to %p\n", g_top_locator));
  len = MIN(strlen(&glocator[8]), 4096 - 1);
  *((char *) memcpy(g_top_locator, &glocator[8], len) + len) = '\0';
  g_scroll_to_locator = TRUE;
  f_node_ptr->retrieve();
}


void
GraphicAgent::porthole_report(WCallback *wcb)
{
  XawPannerReport *repp = (XawPannerReport*)wcb->CallData();
  XawPannerReport &rep = *repp;
  ON_DEBUG(cerr << "got a porthole_report" << hex << rep.changed << endl);
  if (rep.changed & (XawPRCanvasHeight | XawPRCanvasWidth)){
    ON_DEBUG(cerr << "porthole height or width change" << endl);

    WXawPorthole porthole(wcb->GetWidget());
    WXawPanner panner((Widget)wcb->ClientData());

    WCore picture(porthole.Children()[0]);

    Dimension port_width  = porthole.Width();
    Dimension port_height = porthole.Height();
    Dimension cwidth = panner.CanvasWidth();
    Dimension cheight= panner.CanvasHeight();

    ON_DEBUG(cerr << "Canvas ( " << cwidth << ", " << cheight << ")"<<endl); 
    ON_DEBUG(cerr << "Porthle( " << port_width << ", " << port_height << ")"
	     << endl); 

    WArgList args ;

    // re-adjust the slider 
    if (rep.changed & XawPRCanvasHeight)
      {
	ON_DEBUG(cerr << "adjust panner SliderHeight " << port_height <<
		 endl); 
	panner.SliderHeight(port_height, args);
      }

    if (rep.changed & XawPRCanvasWidth)
      {
	ON_DEBUG(cerr << "adjust panner SliderWidth " << port_width <<
		 endl); 
	panner.SliderWidth(port_width, args);
      }

    panner.Set(args);


    if ((cwidth <= port_width) &&
	 (cheight <= port_height))
      {
	ON_DEBUG(cerr << "PANNER UNmanage" << endl);
	panner.Unmanage();
      } else {
	if (f_panner_state != PANNER_NONE)
	  panner.Manage();
      }
  }
}


void
GraphicAgent::panner_report(WCallback *wcb)
{
  ON_DEBUG(cerr << "panner report " << endl);
  XawPannerReport *repp = (XawPannerReport*)wcb->CallData();
  XawPannerReport &rep = *repp;

  // slide the picture 

  WArgList args ;
  WCore core((Widget)wcb->ClientData());
  core.X(0 - rep.slider_x, args);
  core.Y(0 - rep.slider_y, args);
  core.Set(args);
}


void
GraphicAgent::adjust_panner(unsigned int state)
{
  ON_DEBUG(cerr << "adjust_panner( " << state << ")" << endl);
  if (state == f_panner_state)
    return;

  f_panner_state = state ;

  WArgList args ;

  WXawPanner &panner(*f_panner) ;

  switch (state){
  case PANNER_UPPER_LEFT:
    {
      /* upper left */
      panner.LeftAttachment(XmATTACH_FORM, args);
      panner.TopAttachment(XmATTACH_FORM, args);
      panner.RightAttachment(XmATTACH_POSITION, args);
      panner.RightPosition(20, args);
      panner.BottomAttachment(XmATTACH_POSITION, args);
      panner.BottomPosition(20, args);
    }
    break;
  case PANNER_UPPER_RIGHT:
    {
      /* upper right */
      panner.RightAttachment(XmATTACH_FORM, args);
      panner.TopAttachment(XmATTACH_FORM, args);
      panner.LeftAttachment(XmATTACH_POSITION, args);
      panner.LeftPosition(80, args);
      panner.BottomAttachment(XmATTACH_POSITION, args);
      panner.BottomPosition(20, args);
    }
    break;
  case PANNER_LOWER_RIGHT:
    {
      /* lower right */
      panner.RightAttachment(XmATTACH_FORM, args);
      panner.BottomAttachment(XmATTACH_FORM, args);
      panner.LeftAttachment(XmATTACH_POSITION, args);
      panner.LeftPosition(80, args);
      panner.TopAttachment(XmATTACH_POSITION, args);
      panner.TopPosition(80, args);
    }
    break;
  case PANNER_LOWER_LEFT:
    {
      /* lower left */
      panner.LeftAttachment(XmATTACH_FORM, args);
      panner.BottomAttachment(XmATTACH_FORM, args);
      panner.RightAttachment(XmATTACH_POSITION, args);
      panner.RightPosition(20, args);
      panner.TopAttachment(XmATTACH_POSITION, args);
      panner.TopPosition(80, args);
    }
    break;
  case PANNER_NONE:
    {
      /* unmanaged */
      panner.Unmanage();
    }
    break;
  default:
    assert(0);
    break;
  }

  panner.Set(args);

  // see if we can display it 

  Dimension cwidth =  panner.CanvasWidth();
  Dimension cheight = panner.CanvasHeight();

  WXawPorthole porthole(XtParent(*f_panner)) ;

  ON_DEBUG(cerr << "Panner - state: " << state << " canvas ( " << cwidth
	   << ", " << cheight << ") porthole ( " << porthole.Width() <<
	   ", " << porthole.Height() << ")" << endl);

  if ((state != PANNER_NONE) &&
      ((cwidth > porthole.Width()) ||
       (cheight > porthole.Height())))
    {
      panner.Manage();
    }
}


void
GraphicAgent::set_panner(WCallback *wcb)
{
  // adjust panner to the Set state 
  WXmToggleButton button(wcb->GetWidget());
  if (button.Set())
    {
      size_t state = (size_t)wcb->ClientData();
      adjust_panner(state);
    }      
}


void
GraphicAgent::zoom_callback(WCallback *wcb)
{
  ON_DEBUG (puts ("GraphicAgent::zoom_callback() called"));
  WXmToggleButton button(wcb->GetWidget());
#if DEBUG
  XmToggleButtonCallbackStruct &cbs = 
    *(XmToggleButtonCallbackStruct*)wcb->CallData();
#endif

  ON_DEBUG(cerr << "cbs.set = " << cbs.set << endl);
  ON_DEBUG(cerr << "cbs.rsn = " << cbs.reason << endl);
  ON_DEBUG(cerr << "cbs.event = " << cbs.event << endl);
  
  if (button.Set())
    {
      size_t scale = (size_t) wcb->ClientData();
      if (scale > 0)
	{
	  set_zoom (scale);
	  f_scale_button = (WXmToggleButton*)(Widget)button ;
	  f_setcustom = 0 ;
	}
      else
	{
	  // we get called twice when selecting already selected custom button
	  // just do nothing second time around 
	  if (f_setcustom)
	    {
	      f_setcustom = 0 ;
	      return ;
	    }
	  f_setcustom = 1 ;

	  UAS_String msg   = CATGETS(Set_GraphicAgent, 6,
					"Enter the new scaling percentage:");
	  UAS_String title = CATGETS(Set_GraphicAgent, 7, "Dtinfo: Set Scale");
	  scale = message_mgr().
		    get_integer((char*)msg, (char*)title, f_current_scale,
                                (Widget)f_shell);

	  ON_DEBUG(cerr << "scale value is " << scale << endl);
	  if ((int)scale == -1)
	    {
	      // unset Custom option in list 
	      if (((Widget)*f_scale_button) != ((Widget)button))
		{
		  button.Set(0);
		  f_scale_button->Set(1);
		  f_setcustom = 0 ;
		}
	      return;
	    }
	  set_zoom (scale);
	  f_scale_button = (WXmToggleButton*)(Widget)button ;
	}
        // maybe this should be a user option
        //fit_window_to_graphic();
    }
}


void
GraphicAgent::set_zoom(unsigned int scale)
{
  // Deal with bogus values. 
  if (scale < 10)
    scale = 10;
  else if (scale > 999)
    scale = 999;

  if (scale == f_current_scale)
    return;

  Wait_Cursor bob;

  XmStringLocalized mtfstring = CATGETS(Set_GraphicAgent, 5,
				"Scaling illustration, please wait...");

  XtVaSetValues((Widget)f_message_area, XmNlabelString, (XmString)mtfstring, NULL);

  XmUpdateDisplay((Widget)f_message_area);

  f_current_scale = scale;

  UAS_String f_graphic_content_type(f_graphic->content_type());
  UAS_String temp_str = f_graphic->data();

  PixmapGraphic *pgr = 
      f_graphic->pixmap_graphic(temp_str, f_graphic->data_length(),
                                 f_graphic_content_type, scale);
  install_new_picture(pgr);
  refresh(graphic());

  if (f_graphic->content_type() == "application/CGM")
    mtfstring = CATGETS(Set_GraphicAgent, 4, "Vector illustration (scalable)");
  else
    mtfstring = CATGETS(Set_GraphicAgent, 9, "Raster illustration (scalable)");

  XtVaSetValues((Widget)f_message_area, XmNlabelString, (XmString)mtfstring, NULL);

}


void
GraphicAgent::refresh(const UAS_Pointer<Graphic> &)
{
  // refresh by clearing window and sending in expose event
  XClearArea(XtDisplay(*f_pixmap_widget),
	     XtWindow(*f_pixmap_widget),
	     0, 0, 0, 0, TRUE);

  // update message area 
  XmStringLocalized mtfstring;
  if (f_graphic->content_type() == "application/CGM")
    mtfstring = CATGETS(Set_GraphicAgent, 4, "Vector illustration (scalable)");
  else
    mtfstring = CATGETS(Set_GraphicAgent, 9, "Raster illustration (scalable)");

  XtVaSetValues((Widget)f_message_area, XmNlabelString, (XmString)mtfstring, NULL);
}


void
GraphicAgent::fit_graphic_to_window_callback(WCallback *)
{
  Dimension gr_width;
  Dimension gr_height;
  int wscale, hscale;
#if 0
  unsigned int scale_factor;
  int dx, dy;
#endif

  // get size of graphic
  gr_width = f_graphic->pixmap_graphic()->width();
  gr_height = f_graphic->pixmap_graphic()->height();

  WXawPorthole porthole(XtParent(*f_pixmap_widget));
#if 0
  dx = abs(porthole.Width() - f_graphic->pixmap_graphic()->width());
  dy = abs(porthole.Height() - f_graphic->pixmap_graphic()->height());
  if (dx < dy)
    scale_factor = 
         (100 * porthole.Width()) / f_graphic->pixmap_graphic()->width();
  else
    scale_factor = 
         (100 * porthole.Height()) / f_graphic->pixmap_graphic()->height();
#endif
  

  wscale = (100 * porthole.Width()) / gr_width;
  hscale = (100 * porthole.Height()) / gr_height;

  // scaling from dimensions of original pixmap graphic 
  if (wscale < hscale)
    set_zoom(wscale);
  else
    set_zoom(hscale);

  // fit graphic to window means custom scale, so
  // toggle the button.
  f_scale_button->Set(0);
  f_scale_button = f_custom_scale;
  f_scale_button->Set(1);

}


void
GraphicAgent::fit_window_to_graphic_callback(WCallback *)
{
  fit_window_to_graphic();
}
void
GraphicAgent::fit_window_to_graphic()
{
  WXawPorthole porthole(XtParent(*f_pixmap_widget));
  WPixmap   picture(*f_pixmap_widget) ;
  WXawPanner panner(*f_panner);

  // desired width of porthole is in panner.Canvas{Width,Height}
  Dimension cwidth  = panner.CanvasWidth();
  Dimension cheight = panner.CanvasHeight() ;

  // menu_bar is first child of form which is first child of f_shell 
  WComposite form(f_shell.Children()[0]);
  WComposite menu_bar(form.Children()[0]) ;
  // f_message_area is Children()[1]
  WXmSeparator separator(form.Children()[2]);

  // account for frame margins 
  // frame is parent of subform is parent of panner 
  WXmFrame frame(XtParent(XtParent(panner)));

  cheight += menu_bar.Height() + frame.MarginHeight() * 2 + frame.TopOffset()
    + frame.BottomOffset() + separator.Height() + 2 * separator.Margin() +
    separator.TopOffset() + separator.BottomOffset() + 
    f_message_area->Height() + f_message_area->TopOffset() +
    f_message_area->BottomOffset() + f_message_area->MarginTop() +
    f_message_area->MarginBottom();// + 2 * f_message_area->MarginHeight(); 

  cwidth  += frame.MarginWidth() * 2 + frame.LeftOffset() +
    frame.RightOffset();

  WArgList args ;
  f_shell.Width(cwidth + 4, args);
  f_shell.Height(cheight + 4, args);
  f_shell.Set(args);

  panner.Unmanage();
}

void
GraphicAgent::install_new_picture(PixmapGraphic *pgr)
{
  // dispose of the old Pixmap if needed 
  Pixmap pixmap = f_pixmap_widget->Picture() ;
  if (pixmap && (pixmap != f_graphic->pixmap_graphic()->pixmap()))
    XFreePixmap(XtDisplay(*f_pixmap_widget), pixmap);	// a scaled pixmap...give it back 


  // put the new Pixmap into the pixmap widget
  WArgList args ;
  f_pixmap_widget->Picture(pgr->pixmap(), args) ; // install the new one 
  f_pixmap_widget->Width(pgr->width(), args);	  // update size 
  f_pixmap_widget->Height(pgr->height(), args);	  
  f_pixmap_widget->Set(args);

  // update some more UI crud

  args.Reset();
  WXawPorthole porthole(XtParent(*f_pixmap_widget));

  args.Reset();
  WXawPanner panner(*f_panner);
  panner.CanvasWidth(pgr->width(), args);
  panner.CanvasHeight(pgr->height(),args);
  panner.SliderWidth(porthole.Width(), args);
  panner.SliderHeight(porthole.Height(), args);
  panner.Set(args);

  if ((f_panner_state != PANNER_NONE) &&
      ((pgr->width() > porthole.Width()) ||
       (pgr->height() > porthole.Height())))
    {

      panner.Manage();

    } else {

      panner.Unmanage();

    }
  

  if (pgr != f_graphic->pixmap_graphic())
    {
      // clean up PixmapGraphic storage, but keep the X Pixmap
      // (its in the pixmap_widget) 
      pgr->set_type(PixmapGraphic::PRESERVE) ;
      delete pgr ;
    }
}

void
GraphicAgent::unset_scale_buttons()
{
  for (int i = 0 ; i < 10; i++)
    WXmToggleButton((*f_view_menu).Children()[i]).Set(False);
}

