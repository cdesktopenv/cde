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
 * $XConsortium: MarkIconMotif.C /main/13 1996/11/19 15:03:42 cde-hal $
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

#define C_TOC_Element
#define L_Basic

#define C_WindowSystem
#define L_Other

#define C_MarkCanvas
#define C_MarkIcon
#define C_MarkChooser
#define L_Agents

#define C_MessageMgr
#define L_Managers

#define USES_OLIAS_FONT

#include <X11/IntrinsicP.h>
#include <X11/Intrinsic.h>
#include <X11/CoreP.h>

#include "Prelude.h"

#include "Managers/CatMgr.hh"
#include "Other/XmStringLocalized.hh"

#include "Registration.hh"

#include <ctype.h>
#include <WWL/WXmSeparatorGadget.h>
#include <WWL/WXmLabelGadget.h>
#include <WWL/WXmPushButton.h>

#include <iostream>
using namespace std;

#define CLASS MarkIcon
#include "create_macros.hh"

WTimeOut *g_timeout;


// /////////////////////////////////////////////////////////////////
// class constructor
// /////////////////////////////////////////////////////////////////

MarkIcon::MarkIcon (Widget parent, 
		    Widget chooser_parent,
		    MarkCanvas *mark, 
		    int ypos, int scrollbar_offset)
: f_selected (FALSE),
  f_y_position (ypos),
  f_chooser_parent (chooser_parent)
{
  static XtTranslations translations;

  if (translations == NULL) {

    register_actions();

    translations = XtParseTranslationTable ( 
	"<Btn1Down>:single_click()\n"
	"<Btn1Down>(2):double_click()\n"
	"<Key>osfHelp:Help()\n"
	"<Key>Return:double_click()\n"
	"Ctrl<Key>Return:double_click()\n"
	"<Key>osfActivate: double_click()");
  }

  widget = WXmPushButton (parent, "mark_icon", WAutoManage);

  XtOverrideTranslations(widget, translations);

  // ON_ACTIVATE (*this, activate);

  append (mark);

  UserData (this);

  ON_DEBUG(cerr << "MarkIcon::MarkIcon -> ypos: " << ypos << " offset: " <<
    scrollbar_offset << endl);


  Y ((Position) ypos - scrollbar_offset);
  TopOffset(ypos - scrollbar_offset);
  Raise();
}


// /////////////////////////////////////////////////////////////////
// register_actions
// /////////////////////////////////////////////////////////////////

void
MarkIcon::register_actions()
{
 static bool registered = FALSE;

 if (registered)
    return;

 static XtActionsRec actions_list[] =
 { 
      {(char*)"double_click",    &MarkIcon::static_double_click},
      {(char*)"single_click",    &MarkIcon::static_single_click},
 };

 XtAppAddActions (window_system().app_context(), actions_list, XtNumber (actions_list));


}

// /////////////////////////////////////////////////////////////////
// static_double_click
// /////////////////////////////////////////////////////////////////

void 
MarkIcon::static_double_click(Widget widget, XEvent *event, String *args, Cardinal *num_args)
{

 ((MarkIcon *) WXmPushButton(widget).UserData())->double_click (event);

}

// /////////////////////////////////////////////////////////////////
// static_single_click
// /////////////////////////////////////////////////////////////////

void 
MarkIcon::static_single_click(Widget widget, XEvent *event, String *args, Cardinal *num_args)
{

 ((MarkIcon *) WXmPushButton(widget).UserData())->single_click (event);

}


// /////////////////////////////////////////////////////////////////
// double_click
// /////////////////////////////////////////////////////////////////

void 
MarkIcon::double_click(XEvent *event)
{
 
  if (mark_count() == 1)
  {
      List_Iterator<MarkCanvas *> m (f_mark_list);
	  m.item()->select();
	  m.item()->mark_ptr()->edit();
  }
  else // mark_mark_count() > 1
  {
      assert (mark_count() > 1);

      if (g_timeout) {
	delete g_timeout;
	g_timeout = NULL;
      }

      UAS_String title  = CATGETS(Set_MarkChooser, 2, "Dtinfo: Edit Mark");
      UAS_String btitle = CATGETS(Set_AgentLabel, 16, "Edit");
      MarkChooser chooser (f_chooser_parent, f_mark_list,
					(char*)title, (char*)btitle);
	  
      MarkCanvas *choice = chooser.get_choice();
      if (choice != NULL)
      {
        choice->select();
        choice->mark_ptr()->edit();
      }
   }
}

// /////////////////////////////////////////////////////////////////
// single_click
// /////////////////////////////////////////////////////////////////

void 
MarkIcon::single_click(XEvent *event)
{

  if (mark_count() == 1)
    {
      List_Iterator<MarkCanvas *> m (f_mark_list);
	  if (!f_selected)
	    m.item()->select();
	  else
	    m.item()->deselect();
    }
  else // mark_mark_count() > 1
    {
      assert (mark_count() > 1);

      // Use the double click delay plus 30 milliseconds. 
      int delay = XtGetMultiClickTime (XtDisplay (widget)) + 30;
      g_timeout =
        new WTimeOut (window_system().app_context(), delay, this,
		  (WTimeOutFunc) &MarkIcon::select_mark_timeout);
    }
}


// /////////////////////////////////////////////////////////////////
// class destructor
// /////////////////////////////////////////////////////////////////

MarkIcon::~MarkIcon()
{
  List_Iterator<MarkCanvas *> m (f_mark_list);
  while (m)
    {
      m.item()->set_mark_icon (NULL);
      m++;
    }

  if (g_timeout) {
    delete g_timeout;
    g_timeout = NULL;
  }

  XtDestroyWidget (widget);
}


// /////////////////////////////////////////////////////////////////
// highlight
// /////////////////////////////////////////////////////////////////

inline void
MarkIcon::highlight()
{
  if (f_selected == FALSE)
    {
      ON_DEBUG(printf ("highlighting mark %p\n", this));
      swap_colors();
    }
}

void
MarkIcon::select (MarkCanvas *)
{
  highlight();

  f_selected = TRUE;
}


// /////////////////////////////////////////////////////////////////
// unhighlight
// /////////////////////////////////////////////////////////////////

inline void
MarkIcon::unhighlight()
{
  if (f_selected == TRUE)
    {
      ON_DEBUG(printf ("unhighlighting mark %p\n", this));
      swap_colors();
    }
}

void
MarkIcon::deselect (MarkCanvas *)
{
  unhighlight();

  f_selected = FALSE;
}


// /////////////////////////////////////////////////////////////////
// update_icon
// /////////////////////////////////////////////////////////////////

void
MarkIcon::update_icon (MarkCanvas *)
{
  Pixmap button_pixmap = 0;

  int multi = mark_count() > 1;
  int anno  = False;


  List_Iterator<MarkCanvas *> m (f_mark_list);
  while (m)
    {
      if (*(m.item()->mark_ptr()->notes()) != '\0')
	{
	  anno = True;
	  break;
	}
      m++;
    }

  if (anno) {
    if (multi)
      button_pixmap = XmGetPixmap(window_system().screen(),(char*)"anno_double",
				  Foreground(), Background());
    else
      button_pixmap = XmGetPixmap(window_system().screen(),(char*)"anno_single",
				  Foreground(), Background());
  }
  else {
    if (multi)
      button_pixmap = XmGetPixmap(window_system().screen(),(char*)"mark_double",
				  Foreground(), Background());
    else
      button_pixmap = XmGetPixmap(window_system().screen(),(char*)"mark_single",
				  Foreground(), Background());
  }

  if (button_pixmap && button_pixmap != XmUNSPECIFIED_PIXMAP) {
    LabelPixmap(button_pixmap);
  }
  else {
#ifdef BOOKMARK_DEBUG
    cerr << "XmGetPixmap failed." << endl;
#endif
  }

#if 0
  static char buf[2];
  *buf = OLIAS_BOOKMARK_ICON;

  List_Iterator<MarkCanvas *> m (f_mark_list);
  while (m)
    {
      if (*(m.item()->mark_ptr()->notes()) != '\0')
	{
	  *buf = OLIAS_ANNOTATION_ICON;
	  break;
	}
      m++;
    }

  // NOTE: Slight hack here.  Depends on multi-icons being uppercase version
  // of single icons.  DJB 10/27/92 
  if (mark_count() > 1)
    *buf = toupper (*buf);

  if (f_icon_char != *buf)
    {
      f_icon_char = *buf;
      LabelString (buf);
    }
#endif
}


// /////////////////////////////////////////////////////////////////
// insert - add a mark to this icon
// /////////////////////////////////////////////////////////////////

void
MarkIcon::insert (MarkCanvas *mark_canvas)
{
  mark_canvas->set_mark_icon (this);

  // Insert the new mark in the mark list in sorted order. 
  List_Iterator<MarkCanvas *> m (f_mark_list);
  while (m && mark_canvas->offset() > m.item()->offset())
    m++;

  // Check for last mark in the icon (ie: start of list)
  if (m == NULL)
    {
      f_offset = mark_canvas->offset();
      f_mark_list.append (mark_canvas);
    }
  else
    {
      f_mark_list.insert_before (m, mark_canvas);
    }

  // BUG ALERT!! 
  // If mark_canvas is first item in list, we might have a problem
  // since the later items in the list may no longer overlap it :-(

  if (mark_canvas->selected())
    highlight();

  update_icon (mark_canvas);
}


// /////////////////////////////////////////////////////////////////
// append - add to the end of the list 'o marks
// /////////////////////////////////////////////////////////////////

void
MarkIcon::append (MarkCanvas *mark_canvas)
{
  mark_canvas->set_mark_icon (this);
  f_mark_list.append (mark_canvas);
  f_offset = mark_canvas->offset();

  if (mark_canvas->selected())
    highlight();

  update_icon (mark_canvas);
}


// /////////////////////////////////////////////////////////////////
// remove - remove a mark tml from this icon
// /////////////////////////////////////////////////////////////////

void
MarkIcon::remove (MarkCanvas *mark_canvas)
{
  mark_canvas->set_mark_icon (NULL);
  f_mark_list.remove (mark_canvas);

  // See if we need to recompute the offset. 
  if (mark_count() > 0 && mark_canvas->offset() == f_offset)
    {
      List_Iterator<MarkCanvas *> m (f_mark_list);
      m.last();
      f_offset = m.item()->offset();
    }
  update_icon (NULL);
}


// /////////////////////////////////////////////////////////////////
// activate - activate callback
// /////////////////////////////////////////////////////////////////

void
MarkIcon::activate (WCallback *wcb)
{
  CALL_DATA (XmPushButtonCallbackStruct, pbcs);

  if (mark_count() == 1)
    {
      List_Iterator<MarkCanvas *> m (f_mark_list);
      if (pbcs->click_count == 1)
	{
	  if (!f_selected)
	    m.item()->select();
	  else
	    m.item()->deselect();
	}
      else if (pbcs->click_count == 2)
	{
	  m.item()->select();
	  m.item()->mark_ptr()->edit();
	}
    }
  else // mark_mark_count() > 1
    {
      assert (mark_count() > 1);
      
      if (pbcs->click_count == 1)
	{
	  // Use the double click delay plus 20 milliseconds. 
	  int delay = XtGetMultiClickTime (XtDisplay (widget)) + 2000;
	  g_timeout =
	    new WTimeOut (window_system().app_context(), delay, this,
			  (WTimeOutFunc) &MarkIcon::select_mark_timeout);
		
	}
      else if (pbcs->click_count == 2)
	{
	  delete g_timeout;
	  g_timeout = NULL;

	  UAS_String title  = CATGETS(Set_MarkChooser, 2, "Dtinfo: Edit Mark");
	  UAS_String btitle = CATGETS(Set_AgentLabel, 16, "Edit");
	  MarkChooser chooser (f_chooser_parent, f_mark_list,
					(char*)title, (char*)btitle);
	  
	    MarkCanvas *choice = chooser.get_choice();
	    if (choice != NULL)
	      {
	        choice->select();
	        choice->mark_ptr()->edit();
	      }

	}
    }
}

// /////////////////////////////////////////////////////////////////
// select_mark_timeout
// /////////////////////////////////////////////////////////////////

void
MarkIcon::select_mark_timeout()
{
  // WTimeOut will destruct itself, do not delete g_timeout here
  g_timeout = NULL;

  UAS_String title  = CATGETS(Set_MarkChooser, 1, "Dtinfo: Select Mark");
  UAS_String btitle = CATGETS(Set_AgentLabel, 161, "Ok");
  MarkChooser chooser (f_chooser_parent, f_mark_list,
					(char*)title, (char*)btitle);

  MarkCanvas *choice = chooser.get_choice();

  if (choice)
  {
    if (! f_selected)
      choice->select();
    else
      choice->deselect();
  }
}

// /////////////////////////////////////////////////////////////////
// swap_colors - swap the foreground and background colors
// /////////////////////////////////////////////////////////////////

void
MarkIcon::swap_colors()
{
  // Swap the fg and bg colors.

  int multi = mark_count() > 1;
  int anno  = False;

  List_Iterator<MarkCanvas *> m (f_mark_list);
  while (m)
    {
      if (*(m.item()->mark_ptr()->notes()) != '\0')
	{
	  anno = True;
	  break;
	}
      m++;
    }

  Pixel background, foreground;

  if (f_selected == FALSE) { // about to select
    foreground = Background();
    background = Foreground();
  }
  else {		     // about to de-select
    foreground = Foreground();
    background = Background();
  }

  Pixmap button_pixmap;

  if (anno) {
    if (multi)
      button_pixmap = XmGetPixmap(window_system().screen(),(char*)"anno_double",
				  foreground, background);
    else
      button_pixmap = XmGetPixmap(window_system().screen(),(char*)"anno_single",
				  foreground, background);
  }
  else {
    if (multi)
      button_pixmap = XmGetPixmap(window_system().screen(),(char*)"mark_double",
				  foreground, background);
    else
      button_pixmap = XmGetPixmap(window_system().screen(),(char*)"mark_single",
				  foreground, background);
  }

  if (button_pixmap && button_pixmap != XmUNSPECIFIED_PIXMAP) {
    LabelPixmap(button_pixmap);
  }

}  

