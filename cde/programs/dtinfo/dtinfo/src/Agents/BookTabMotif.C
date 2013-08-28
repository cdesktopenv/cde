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
 * $XConsortium: BookTabMotif.cc /main/4 1996/06/11 16:11:24 cde-hal $
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


#define C_NodeMgr
#define C_MessageMgr
#define L_Managers

#define C_Tab
#define C_Node
#define L_Doc

#define C_BookTab
#define L_Agents

#include "Prelude.h"

#include <WWL/WComposite.h>

BookTab::~BookTab()
{
}

// /////////////////////////////////////////////////////////////////
// class constructors
// /////////////////////////////////////////////////////////////////

#ifdef UNUSED
BookTab::BookTab (const WComposite &parent)
: WXyzTab ((Widget) NULL),
  f_selected (FALSE)
{
  widget = XyzCreateTabButton (parent, "tab", NULL, 0);

  SetActivateCallback (this, (WWL_FUN) &BookTab::activate);
}
#endif

BookTab::BookTab (NodeWindowAgent *nwa,
		  const WComposite &parent, UAS_Pointer<UAS_Common> &tab_ptr)
: WXyzTab ((Widget) NULL),
  f_node_window_agent (nwa),
  f_selected (FALSE)
{
  widget = XyzCreateTabButton (parent, "tab", NULL, 0);

  SetActivateCallback (this, (WWL_FUN) &BookTab::activate);

  set_tab (tab_ptr);
}


// /////////////////////////////////////////////////////////////////
// set_tab
// /////////////////////////////////////////////////////////////////

void
BookTab::set_tab (UAS_Pointer<UAS_Common> &tab_ptr)
{
  if (f_tab_ptr != tab_ptr)
    {
      f_tab_ptr = tab_ptr;

      if (f_tab_ptr != (const int)NULL)
	{
	  UAS_String st = f_tab_ptr->tab_title();
	  LabelString ((char *) st);
	  if (f_selected)
	    {
	      deselect();
	      f_selected = FALSE;
	    }
	  Manage();
	}
      else
	{
	  Unmanage();
	}
    }
}


// /////////////////////////////////////////////////////////////////
// activate
// /////////////////////////////////////////////////////////////////

void
BookTab::activate()
{
  Xassert (f_tab_ptr != (const int)NULL);
  node_mgr().set_preferred_window (f_node_window_agent);
  f_tab_ptr->retrieve();
}



// /////////////////////////////////////////////////////////////////
// select_if_same - select this tab if it links to specified node
// /////////////////////////////////////////////////////////////////

void
BookTab::select_if_same (UAS_Pointer<UAS_Common> &doc_ptr)
{
  Xassert (f_tab_ptr != (const int)NULL);
  // If this tab points to the Node in question, select the tab,
  // otherwise deselect the tab. 
  if (f_tab_ptr == doc_ptr)
    select();
  else
    deselect();
}


// /////////////////////////////////////////////////////////////////
// select - select this tab button
// /////////////////////////////////////////////////////////////////

void
BookTab::select()
{
  if (f_selected)
    return;

  Pixel bg, fg, arm, top, bottom;
  Pixmap top_pix, bottom_pix;

  /* Swap bg and arm color, fg and bg shadow colors. */
  Get (WArgList (XmNbackground, (XtArgVal) &bg, XmNarmColor, &arm,
		 XmNtopShadowColor, &top, XmNbottomShadowColor, &bottom,
		 XmNforeground, &fg,
		 XmNtopShadowPixmap, &top_pix,
		 XmNbottomShadowPixmap, &bottom_pix,
		 NULL));
  if (fg == arm)
    fg = bg;
  Set (WArgList (XmNbackground, (XtArgVal) arm, XmNarmColor, bg,
		 XmNtopShadowColor, bottom, XmNbottomShadowColor, top,
		 XmNforeground, fg,
		 /* XmNtopShadowPixmap, &bottom_pix,
		 XmNbottomShadowPixmap, &top_pix, */
		 NULL));

  f_selected = TRUE;
}


// /////////////////////////////////////////////////////////////////
// deselect - deselect this tab button
// /////////////////////////////////////////////////////////////////

void
BookTab::deselect()
{
  if (!f_selected)
    return;

  Pixel bg, fg, arm, top, bottom;
  Pixmap top_pix, bottom_pix;

  /* Swap bg and arm color, fg and bg shadow colors. */
  Get (WArgList (XmNbackground, (XtArgVal) &bg, XmNarmColor, &arm,
		 XmNtopShadowColor, &top, XmNbottomShadowColor, &bottom,
		 XmNforeground, &fg,
		 XmNtopShadowPixmap, &top_pix,
		 XmNbottomShadowPixmap, &bottom_pix,
		 NULL));
  if (fg == arm)
    fg = bg;
  Set (WArgList (XmNbackground, (XtArgVal) arm, XmNarmColor, bg,
		 XmNtopShadowColor, bottom, XmNbottomShadowColor, top,
		 XmNforeground, fg,
		 /* XmNtopShadowPixmap, &bottom_pix,
		 XmNbottomShadowPixmap, &top_pix, */
		 NULL));

  f_selected = FALSE;
}
