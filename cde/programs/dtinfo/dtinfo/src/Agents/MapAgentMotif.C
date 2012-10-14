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
/*	Copyright (c) 1994,1995,1996 FUJITSU LIMITED	*/
/*	All Rights Reserved				*/

/*
 * $TOG: MapAgentMotif.C /main/16 1998/04/17 11:33:40 mgreess $
 *
 * Copyright (c) 1992 HaL Computer Systems, Inc.  All rights reserved.
 * UNPUBLISHED -- rights reserved under the Copyright Laws of the United
 * States.  Use of a copyright notice is precautionary only and does not
 * imply publication or disclosure.
 * 
 * This software contains confidential information and trade secrets of HaL
 * Computer Systems, Inc.  Use, disclosure, or reproduction is prohibited
 * without the prior express written permission of HaL Computer Systems, Inc.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions as set forth in subparagraph (c)(l)(ii) of the Rights in
 * Technical Data and Computer Software clause at DFARS 252.227-7013.
 *                        HaL Computer Systems, Inc.
 *                  1315 Dell Avenue, Campbell, CA  95008
 * 
 */

#include "UAS.hh"

#define C_TOC_Element
#define L_Basic

#define C_WindowSystem
#define L_Other

#define C_HelpAgent
#define C_Agent
#define C_MapAgent
#define L_Agents

#define C_WindowGeometryPref
#define L_Preferences

#define C_PrefMgr
#define C_MessageMgr
#define L_Managers

#include "Prelude.h"

#include "Other/XmStringLocalized.hh"
#include "Managers/CatMgr.hh"

#include "Registration.hh"

#include <WWL/WTopLevelShell.h>
#include <WWL/WXmForm.h>
#include <WWL/WXmPushButton.h>
#include <WWL/WXmPanedWindow.h>
#include <WWL/WXmFrame.h>
#include <WWL/WXmPushButton.h>
#include <WWL/WXmToggleButton.h>
#include <WWL/WXmArrowButton.h>

#include "Widgets/WXawPorthole.h"
#include "Widgets/WXawPanner.h"
#include "Widgets/WXawTree.h"

#define CLASS MapButton
#include "create_macros.hh"

class MapButton : public WWL
{
public:
  MapButton (WComposite &parent, const UAS_Pointer<UAS_Common> &doc_ptr,
	     MapButton *ancestor);

  void activate();
  void expand();
  void destroy();

  WXmForm        f_form;
  WXmPushButton  f_button;
  WXmArrowButton f_arrow;
  UAS_Pointer<UAS_Common> f_doc_ptr;
  bool           f_expanded;

  static void ManageKids();
  static Widget *f_kids;
  static int     f_num_kids;
  static int     f_kids_size;
};

Widget *MapButton::f_kids;
int MapButton::f_num_kids;
int MapButton::f_kids_size;

MapButton::MapButton (WComposite &parent,
                      const UAS_Pointer<UAS_Common> &doc_ptr,
		      MapButton *ancestor)
: f_form (parent, "form"),
  f_doc_ptr (doc_ptr),
  f_expanded (FALSE)
{
  ON_DEBUG (printf ("MapButton::MapButton (%s)\n", (char*)f_doc_ptr->title()));
  static bool expandable_tree =
    window_system().get_boolean_default ("ExpandableMap");
  UAS_List<UAS_Common> kids (f_doc_ptr->children());
  UAS_String t = f_doc_ptr->title();
  if (kids.length() > 0 && expandable_tree)
    {
      f_arrow = WXmArrowButton (f_form, "expand", WAutoManage);
      ON_ACTIVATE (f_arrow,expand);
      f_button =
	WXmPushButton (f_form, t, WAutoManage,
		       WArgList (XmNrightWidget, (XtArgVal) ((Widget) f_arrow),
				 XmNrightAttachment, XmATTACH_WIDGET,
				 NULL));
    }
  else
    {
      f_button = WXmPushButton (f_form, t, WAutoManage);
    }

  ON_ACTIVATE (f_button,activate);
  SET_CALLBACK (f_form,Destroy,destroy);

  f_form.ShadowThickness (0);
  // f_form.Realize();

  // Add to the list so that we can manage 'em all at once. 
  if (f_num_kids + 1 > f_kids_size)
    {
      f_kids_size *= 2;
      f_kids = (Widget *) realloc (f_kids, sizeof (Widget) * f_kids_size);
    }
  f_kids[f_num_kids++] = (Widget) f_form;
  ON_DEBUG (printf ("Form managed = %d\n", XtIsManaged (f_form)));
    
  if (ancestor != NULL)
    {
      CXawTree form (f_form);
      WArgList args;

      form.TreeParent (ancestor->f_form, args);
      form.Set (args.Args(), args.NumArgs());
    }
}

void
MapButton::ManageKids()
{
  XtManageChildren (f_kids, f_num_kids);
  f_num_kids = 0;
}

void
MapButton::activate()
{
  f_doc_ptr->retrieve();
}

void
MapButton::expand()
{
  if (f_expanded)
    {
      f_arrow.ArrowDirection (XmARROW_RIGHT);
      f_expanded = FALSE;
    }
  else
    {
      f_arrow.ArrowDirection (XmARROW_DOWN);
      f_expanded = TRUE;
      // Create a button for each child.
      UAS_List<UAS_Common> kids (f_doc_ptr->children());
      WXawTree tree (XtParent (f_form));
      for (int i = 0; i < kids.length(); i++)
	new MapButton (tree, kids[i], this);
      ON_DEBUG (puts ("managing kids"));
      MapButton::ManageKids();
      ON_DEBUG (puts ("calling force layout"));
      tree.ForceLayout();
      ON_DEBUG (puts ("called force layout"));
    }
}


void
MapButton::destroy()
{
  // Free object memory when the widget is destroyed.
  // Assumes heap allocation of MapButton objects. 
  delete this;
}


// /////////////////////////////////////////////////////////////////
// class constructor
// /////////////////////////////////////////////////////////////////

MapAgent::MapAgent()
: f_shell (NULL),
  f_onscreen (FALSE)
{
}


// /////////////////////////////////////////////////////////////////
// mode - return the current map mode
// /////////////////////////////////////////////////////////////////

MapAgent::map_mode_t
MapAgent::mode() const
{
  return (f_map_mode);
}


// /////////////////////////////////////////////////////////////////
// set_mode - set the map mode
// /////////////////////////////////////////////////////////////////

void
MapAgent::set_mode (const map_mode_t mode)
{
  Xassert (mode == LOCAL_MODE || mode == GLOBAL_MODE);

  f_map_mode = mode;
}


// /////////////////////////////////////////////////////////////////
// can_display - return true if this agent is willing to display
// /////////////////////////////////////////////////////////////////

bool
MapAgent::can_display (UAS_Pointer<UAS_Common> &) const
{
  return (TRUE);
}


// /////////////////////////////////////////////////////////////////
// display - dislay a map localized around the specified document
// /////////////////////////////////////////////////////////////////

void
MapAgent::display (UAS_Pointer<UAS_Common> &doc_ptr, bool popup)
{
  static bool first_time = True;
  u_int i, num_children;
  MapButton *parent_button, *child_button, *this_button = NULL;

  if (f_shell == NULL)
    create_ui();

  // Just pop up the window if the map has already been created
  // for the specified document.
  if (doc_ptr == f_doc_ptr)
    {
      if (popup) {
	f_shell->Popup();
        XMapRaised(XtDisplay(*f_shell), XtWindow(*f_shell));
      }
      f_onscreen = TRUE;
      return;
    }

  // Must have to create a new tree, so start by wiping out the old one. 
  if (f_tree != NULL)
    {
      f_tree->Destroy();
      delete f_tree;
    }
  f_tree = new WXawTree (*f_porthole, "tree");
  //  f_tree->Realize();

  // Tree gravity should be a preference that is retrieved right here.
  // (Or better yet stored in the class record.) 

  /* -------- Start the local map at this node's parent. -------- */

  UAS_Pointer<UAS_Common> toc_this = doc_ptr;
  UAS_Pointer<UAS_Common> toc_parent =
      (doc_ptr != (UAS_Pointer<UAS_Common>)0)
	  ? doc_ptr->parent() : (UAS_Pointer<UAS_Common>)0;

  // If the entry has a parent, create a button for it and each of
  // the entry's siblings. 
  if (toc_parent != (UAS_Pointer<UAS_Common>)NULL)
    {
      parent_button = new MapButton (*f_tree, toc_parent, NULL);
      //  parent_button->expand();

      /* -------- Create a button for each sibling. -------- */

      UAS_List<UAS_Common> kids = toc_parent->children();
      num_children = kids.length();
      UAS_Pointer<UAS_Common> toc_kid;
      for (i = 0; i < num_children; i++)
	{
	  toc_kid = kids[i];
	  child_button = new MapButton (*f_tree, toc_kid, parent_button);
	  if (toc_kid == doc_ptr)
            {
              f_doc_ptr = doc_ptr;
   	      this_button = child_button;
            }
	}
    }
  else // No TOC parent -- SWM: Also may be no TOC!!!
    {
      f_doc_ptr = doc_ptr;
      this_button = new MapButton (*f_tree, toc_this, NULL);
    }

  if (this_button == NULL)
    {
      message_mgr().
	error_dialog (CATGETS(Set_Messages, 7, "File a Bug"));
      return;
    }
  else
    {
      static bool first_time = TRUE;
      static Pixel highlight_bg, highlight_fg;
      if (first_time)
	{
	  const char *s;
	  unsigned long status;
	  s = window_system().get_string_default ("MapHighlightBackground");
	  if (s == NULL || *s == '\0')
	    {
	      highlight_bg = this_button->f_button.Foreground();
	    }
	  else
	    {
	      status = window_system().get_color (s, highlight_bg);
	      // On failure to allocate, just invert. 
	      if (status == 0)
		{
		  highlight_bg = this_button->f_button.Foreground();
		  highlight_fg = this_button->f_button.Background();
		}
	      // Got bg, so now try for fg. 
	      else
		{
	          s = window_system().
		        get_string_default ("MapHighlightForeground");
		  if (s == NULL || *s == '\0')
		    {
		      highlight_fg =this_button->f_button.Background();
		    }
		  else
		    {
		      status = window_system().get_color (s, highlight_fg);
		      // If we cant get both colors, just invert the button. 
		      if (status == 0)
			{
			  Display *dpy = window_system().display();
			  XFreeColors (dpy,
				       DefaultColormap(dpy,DefaultScreen(dpy)),
				       &highlight_bg, 1, 0);
			  highlight_bg = this_button->f_button.Foreground();
			  highlight_fg = this_button->f_button.Background();
			}
		    }
		}
	    }
	  if (highlight_fg == this_button->f_button.Foreground() ||
	      highlight_bg == this_button->f_button.Background() ||
	      highlight_fg == highlight_bg)
	    {
	      highlight_bg = this_button->f_button.Foreground();
	      highlight_fg = this_button->f_button.Background();
	    }
	  first_time = FALSE;
	}
      this_button->f_button.Background (highlight_bg);
      this_button->f_button.Foreground (highlight_fg);
      //  this_button->expand();
    }

  /* -------- Create a button for each child. -------- */

  if (toc_this != (UAS_Pointer<UAS_Common>)NULL)
    {
      UAS_List<UAS_Common> myKids = toc_this->children();
      num_children = myKids.length();
      for (i = 0; i < num_children; i++)
	child_button =
	  new MapButton (*f_tree, myKids[i], this_button);

#if 0
  if (!XtIsRealized(*f_shell))
    {
      f_tree->Manage();
      f_shell->Realize();
    }
#endif

    }
  // Manage all the children.
  MapButton::ManageKids();

  UAS_String buffer = CATGETS(Set_MapAgent, 2, "Dtinfo: ");
  buffer = buffer + doc_ptr->title();
  f_shell->Title ((char*)buffer);

  if (!XtIsRealized (*f_shell))
    f_shell->Realize();

  f_tree->Realize();
  f_tree->ForceLayout();

  f_min_tree_width = f_tree->Width();
  f_min_tree_height = f_tree->Height();
  ON_DEBUG (printf ("+++++ Tree min dims: %d x %d\n",
		    f_min_tree_width, f_min_tree_height));

  center_on (this_button);
  f_tree->Manage();

  if (popup)
    {
      if (first_time)
	{
  	  WXmForm form (XtParent (XtParent (*f_panner)));
	  form.Height (50);
	  first_time = False;
	}
      f_shell->Popup();
      XMapRaised(XtDisplay(*f_shell), XtWindow(*f_shell));
    }

  f_onscreen = TRUE;
}


// /////////////////////////////////////////////////////////////////
// create_ui - create the map user interface
// /////////////////////////////////////////////////////////////////

void
MapAgent::create_ui()
{
  MapButton::f_kids_size = 8;
  MapButton::f_kids = (Widget *)
                      malloc (sizeof (Widget) * MapButton::f_kids_size);
  
  f_shell = new WTopLevelShell (toplevel(), WPopup, "map");
  window_system().register_shell (f_shell);

  XtVaSetValues(*f_shell, XmNtitle,
	        CATGETS(Set_MapAgent, 1, "Dtinfo: Local Map"), NULL);
  WindowGeometry wg = pref_mgr().get_geometry (PrefMgr::MapGeometry);
  f_shell->Set(WArgList(XmNwidth, wg.width,
			XmNheight, wg.height,
			NULL));

  // rtp - added new code 8/27/92
  WXmForm rootform (*f_shell, "rootform");

  WXmForm bottomform (rootform, "bottomform");
  WXmPushButton close (bottomform, "close", WAutoManage);
  WXmPushButton help (bottomform, "help", WAutoManage);
  help_agent().add_activate_help (help, (char*)"map_help");

  XtVaSetValues(close, XmNlabelString,
	(XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 12, "Close")), NULL);
  XtVaSetValues(help, XmNlabelString,
	(XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 48, "Help")), NULL);

  WXmPanedWindow pane (rootform, "pane");
  WXmForm topform (pane, "topform");
  XtVaSetValues(topform, XmNheight, 75, NULL);
  XtVaSetValues(pane, XmNpaneMinimum, 75, XmNpositionIndex, 1, NULL);

  WXmFrame panner_frame (topform, "pframe", WAutoManage);
  f_panner = new WXawPanner (panner_frame, "panner", WAutoManage);
  WXmFrame tree_frame (pane, "tframe", WAutoManage);
  f_porthole = new WXawPorthole (tree_frame, "porthole");

  f_lock = new WXmToggleButton (topform, "lock", WAutoManage);
  XtVaSetValues(*f_lock, XmNlabelString,
     (XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 196, "Auto Update")),
                         XmNindicatorSize, 15, NULL);
  bool lock_status = pref_mgr().get_boolean (PrefMgr::MapAutoUpdate);
  f_lock->Set(lock_status);
  f_locked = lock_status ;
  // NOTE: Should have a way to remove displayer so that receive function
  // isn't called every time a node is displayed.  14:08 12-Oct-93 DJB 
  UAS_Common::request ((UAS_Receiver<UAS_DocumentRetrievedMsg> *) this);

  f_lock->SetValueChangedCallback(this, (WWL_FUN) &MapAgent::lock_toggle);
  f_porthole->SetReportCallback (this, (WWL_FUN) &MapAgent::porthole_resize);
  f_panner->SetReportCallback (this, (WWL_FUN) &MapAgent::panner_movement);

  bottomform.Manage();
  
  Dimension height;
  f_lock->Get (WArgList (XmNheight, (XtArgVal)&height, NULL));
  topform.Set(WArgList(XmNpaneMinimum, height, NULL));

  topform.Manage();
  f_porthole->Manage();
  pane.Manage();

  /* -------- Callbacks -------- */
  f_wm_delete_callback =
    new WCallback (*f_shell, window_system().WM_DELETE_WINDOW(),
                   this, (WWL_FUN) &MapAgent::close_window);
  close.SetActivateCallback (this, (WWL_FUN) &MapAgent::close_window);

  rootform.Manage();

  //  f_shell->Realize();

  f_tree = NULL;
}


// /////////////////////////////////////////////////////////////////
// porthole_resize
// /////////////////////////////////////////////////////////////////

void
MapAgent::porthole_resize (WCallback *wcb)
{
  WArgList args;
  XawPannerReport *rep = (XawPannerReport *) wcb->CallData();

  ON_DEBUG (printf (">>> Porthole resize, mtw = %d x %d\n",
		    f_min_tree_width, f_min_tree_height));

  // Don't let the tree widget get bigger that the space needed
  // to hold the tree when it can't fit in the porthole.  If we
  // allow this, the user could grow the tree to a large size
  // by enlarging the map window, then shrink the map window and
  // keep the tree at the large size and pan to empty space. 
  if (rep->changed & ~(XawPRSliderX | XawPRSliderY))
    {
      Dimension twidth, theight;

      twidth = f_tree->Width();
      theight = f_tree->Height();
      ON_DEBUG (printf ("Tree attempts width = %d, height = %d\n",
			twidth, theight));
      if ((twidth > f_min_tree_width && twidth > f_porthole->Width()) ||
          (theight > f_min_tree_height && theight > f_porthole->Height()))
        {
          f_tree->Width(f_min_tree_width, args);
          f_tree->Height(f_min_tree_height, args);
	  ON_DEBUG (puts ("Adjusting tree"));
          f_tree->Set (args);
	  ON_DEBUG (puts ("... Done adjusting tree"));
          return;
        }
    }
     
  ON_DEBUG (printf (">>> Porthole resize (%d, %d, %d, %d, %d, %d)\n",
		    rep->slider_x, rep->slider_y,
		    rep->slider_width, rep->slider_height,
		    rep->canvas_width, rep->canvas_height));
  f_panner->SliderX (rep->slider_x, args);
  f_panner->SliderY (rep->slider_y, args);
  if (rep->changed & ~(XawPRSliderX | XawPRSliderY))
    {
      ON_DEBUG (printf (".....width & height change\n"));
      f_panner->SliderWidth (rep->slider_width, args);
      f_panner->SliderHeight (rep->slider_height, args);
      f_panner->CanvasWidth (rep->canvas_width, args);
      f_panner->CanvasHeight (rep->canvas_height, args);
    }
  f_panner->Set (args);
}


// /////////////////////////////////////////////////////////////////
// panner_movement
// /////////////////////////////////////////////////////////////////

void
MapAgent::panner_movement (WCallback *wcb)
{
  WArgList args;
  XawPannerReport *rep = (XawPannerReport *) wcb->CallData();

  f_tree->X (-rep->slider_x, args);
  f_tree->Y (-rep->slider_y, args);
  f_tree->Set (args);
}


// /////////////////////////////////////////////////////////////////
// close_window
// /////////////////////////////////////////////////////////////////

void
MapAgent::close_window (WCallback *)
{
  f_shell->Popdown();
  f_onscreen = FALSE;
}

// /////////////////////////////////////////////////////////////////
// lock_toggle - callback for lock button
// /////////////////////////////////////////////////////////////////

void 
MapAgent::lock_toggle (WCallback *wcb)
{
  ON_DEBUG(cerr << "lock toggle callback" << endl);

  XmToggleButtonCallbackStruct &cbs =
    *(XmToggleButtonCallbackStruct*) wcb->CallData();

  f_locked = cbs.set;
}

// /////////////////////////////////////////////////////////////////
// can_update - return true if this agent is willing to update
// /////////////////////////////////////////////////////////////////

bool
MapAgent::can_update()
{
  return (f_onscreen);
}

// /////////////////////////////////////////////////////////////////////////
// lock - set the lock
// /////////////////////////////////////////////////////////////////////////

void
MapAgent::lock()
{
  f_locked = TRUE;
  f_lock->Set(TRUE);
}

// /////////////////////////////////////////////////////////////////////////
// unlock - unset the lock
// /////////////////////////////////////////////////////////////////////////
void
MapAgent::unlock()
{
  f_locked = FALSE ;
  f_lock->Set(False);
}

// /////////////////////////////////////////////////////////////////
// center_on - center current node in tree view
// /////////////////////////////////////////////////////////////////

void
MapAgent::center_on (MapButton *map_button)
{
  // WXmPushButton button = map_button->f_button;
  WXmForm &button = map_button->f_form;
  WArgList args;
  Position bcenter_x, bcenter_y;
  Position pcenter_x, pcenter_y;
  Dimension pwidth, pheight;
  Dimension twidth, theight;
  Position off_x = 0, off_y = 0;

  bcenter_x = button.X() + button.Width() / 2;
  bcenter_y = button.Y() + button.Height() / 2;
  ON_DEBUG (printf ("Form widget id = %p\n", (Widget) button));
  ON_DEBUG (printf ("Button realized = %d\n", XtIsRealized (button)));
  ON_DEBUG (printf ("Button coords = (%d,%d)\n", button.X(), button.Y()));
  ON_DEBUG (printf("Button dims = (%d,%d)\n", button.Width(),button.Height()));
  ON_DEBUG (printf ("Button center = (%d,%d)\n", bcenter_x, bcenter_y));

  pwidth = f_porthole->Width();
  pheight = f_porthole->Height();
  twidth = f_tree->Width();
  theight = f_tree->Height();
  ON_DEBUG (printf ("Porthole dimensions = (%d,%d)\n", pwidth, pheight));
  ON_DEBUG (printf ("Tree dimensions = (%d,%d)\n", twidth, theight));

  pcenter_x = f_porthole->X() + pwidth / 2;
  pcenter_y = f_porthole->Y() + pheight / 2;
  ON_DEBUG (printf ("Porthole center = (%d,%d)\n", pcenter_x, pcenter_y));

  if (bcenter_x > pcenter_x)
    off_x = pcenter_x - bcenter_x;

  if (bcenter_y > pcenter_y)
    off_y = pcenter_y - bcenter_y;

  if (off_x + twidth < pwidth)
    off_x = pwidth - twidth;

  if (off_y + theight < pheight)
    off_y = pheight - theight;

  ON_DEBUG (printf ("Button center offset = (%d,%d)\n", off_x, off_y));

  if (off_x < 0)
    {
      f_tree->X (off_x, args);
      ON_DEBUG (printf ("Setting Tree X to %d\n", off_x));
    }

  if (off_y < 0)
    {
      f_tree->Y (off_y, args);
      ON_DEBUG (printf ("Setting Tree Y to %d\n", off_y));
    }

  if (args.NumArgs() > 0)
    f_tree->Set (args);
}


// /////////////////////////////////////////////////////////////////
// receive - handle node display messages
// /////////////////////////////////////////////////////////////////

void
MapAgent::receive (UAS_DocumentRetrievedMsg &message, void* /*client_data*/)
{
  extern bool g_style_sheet_update ;
  ON_DEBUG (printf ("MapAgent::receive - updating the map!\n"));
  if (f_locked && f_onscreen && !g_style_sheet_update)
    {
      // MapAgent::receive is just for automatic self-update, so
      // there's no need for mapping/raising the window.
      display (message.fDoc, FALSE);
    }
}

void
MapAgent::clear_map(UAS_Pointer<UAS_Common> &lib)
{
  if(lib->lid() == f_doc_ptr->lid())
  {
    clear_it();
    f_doc_ptr = NULL;
  }
}

void
MapAgent::clear_it()
{
  if ((f_tree != NULL) && f_onscreen)
  {
    f_tree->Destroy();
    delete f_tree;
    f_tree = NULL;
  }
}
