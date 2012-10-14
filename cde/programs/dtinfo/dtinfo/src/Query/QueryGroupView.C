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
/*	Copyright (c) 1994 FUJITSU LIMITED	*/
/*	All Rights Reserved			*/

/*
 * $XConsortium: QueryGroupView.cc /main/3 1996/06/11 16:31:56 cde-hal $
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


#define C_QueryGroup
#define C_QueryGroupView
#define C_QueryTermView
#define C_QueryEditor
#define C_QueryTerm
#define L_Query

#include "Prelude.h"

#include <Xm/DrawingA.h>

// /////////////////////////////////////////////////////////////////
// class constructor
// /////////////////////////////////////////////////////////////////

QueryGroupView::QueryGroupView (QueryGroup *group, Widget parent)
: f_query_group (group),
  f_term_view_list (NULL)
{
  f_restraint = WRestraint (WComposite (parent), "group_view", WAutoManage);
  f_form = WXmForm (f_restraint, "group_form");

  QueryTermView *view = NULL;
  QueryTerm *term;

  for (term= group->term_list(); term != NULL; term = term->next())
    view = new QueryTermView (term, this, view, NULL);

  f_form.Realize();  // This statement is extremely critical.  02/03/93 DJB 
  f_form.Manage();

  // Need special event handler for top level group to catch
  // resizes of window and resize the query view.
  // NOTE: It's just about time to move this to another method. 
  if (XtClass (XtParent (f_restraint)) == xmDrawingAreaWidgetClass)
    {
      // Let's see what the parent's size is now:
      ON_DEBUG(printf ("Drawing area is width: %d, height: %d\n",
		       WCore(f_restraint.Parent()).Width(),
		       WCore(f_restraint.Parent()).Height()));
      // We need to grow the drawing area if it is smaller than the
      // restraint widget, since we don't deal with horizontal scrolling.
      Dimension restraint_width = f_restraint.Width();
      Dimension da_width = WCore(f_restraint.Parent()).Width();
      
      if (da_width < restraint_width)
	{
	  // Can't resize the drawing area because the ^$&(*% Motif
	  // pane widget will not allow the horizontal resize.  So,
	  // instead calculate the size increase and do it on the shell. 
	  Dimension increase = restraint_width - da_width;
	  // Find the shell widget. 
	  Widget w = f_restraint.Parent();
	  while (!XtIsShell (w))
	    w = XtParent(w);
	  WTopLevelShell shell (w);

	  ON_DEBUG(printf ("** Resizing shell by = %d\n", increase));

	  // Change state if needed. 
	  Boolean allow_resize = shell.AllowShellResize();
	  if (!allow_resize)
	    shell.AllowShellResize (True);
	  // Change the width. 
	  shell.MinWidth (shell.Width() + increase);
	  shell.Width (shell.Width() + increase);
	  // Restore state if needed. 
	  if (!allow_resize)
	    shell.AllowShellResize (False);
	}
      else if (da_width > restraint_width)
	{
	  f_restraint.Width (WCore(f_restraint.Parent()).Width());
	}
      
      XtAddEventHandler (XtParent (f_restraint), StructureNotifyMask, False,
			 (XtEventHandler) &QueryGroupView::resize,
			 (Widget) f_restraint);
      // Store the min width in UserData...
      f_restraint.UserData ((XtPointer)(size_t) f_restraint.Width());
    }
  // Make sure the restraint widget isn't too narrow.
  else
    {
      f_restraint.Width (WCore(f_restraint.Parent()).Width());
    }
}


// /////////////////////////////////////////////////////////////////
// class descrutctor
// /////////////////////////////////////////////////////////////////

QueryGroupView::~QueryGroupView()
{
  // The QueryTermView destructor updates this pointer, which
  // is what lets the loop eventually terminate. 
  while (f_term_view_list != NULL)
    delete f_term_view_list;
}

//#ifdef UseFJMTF
#if 1
// /////////////////////////////////////////////////////////////////
// re_load_terms - load search terms again
// /////////////////////////////////////////////////////////////////

void QueryGroupView::re_load_terms(QueryGroup* group)
{
    QueryTermView* view;

    // destroy widgets of term view
    for (view = f_term_view_list; view; view = view->next_term_view())
	view->destroy_widgets();

    // delete term views
    while (f_term_view_list != NULL)
	delete f_term_view_list;

    delete f_query_group;
    f_query_group = group;

    // load new terms
    QueryTerm *term;
    for (view = 0, term= group->term_list(); term != NULL; term = term->next())
	view = new QueryTermView (term, this, view, NULL);
}
#endif

// /////////////////////////////////////////////////////////////////
// resize - resize top level group when window resizes
// /////////////////////////////////////////////////////////////////

void
QueryGroupView::resize (Widget, Widget w,
			XConfigureEvent *event)
{
  if (event->type != ConfigureNotify)
    return;

  WRestraint restraint (w);
  Dimension min_width = (Dimension) (size_t) restraint.UserData();

  ON_DEBUG(printf ("Group Form Resized: (event %d)\n", event->type));

  ON_DEBUG(printf ("min_width = %d -- sized to %d\n", min_width, event->width));
  /* Width shouldn't be zero, but for some reason it is... */
  if (event->width == 0)
    return;

  // If the parent is bigger than the min size, grow to match it.
  if (event->width >= min_width)
    {
      ON_DEBUG(printf ("  resizing group to event size: %d\n", event->width));
      restraint.Width (event->width);
    }
  // If the parent is smaller, make it the min_width.
  else if (event->width < min_width)
    {
      /* This shouldn't happen now because the editor sets the min shell
	 width not to allow sizes this small. */
      ON_DEBUG(printf ("  resizing group to min width: %d\n", min_width));
      ON_DEBUG(printf ("  ^^^ THIS SHOULDN'T BE HAPPENING!\n"));
      restraint.Width (min_width);
    }
}


// /////////////////////////////////////////////////////////////////
// traverse_here
// /////////////////////////////////////////////////////////////////

void
QueryGroupView::traverse_here()
{
  if (f_term_view_list != NULL)
    f_term_view_list->traverse_here();
}
