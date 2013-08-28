/*	Copyright (c) 1994 FUJITSU LIMITED	*/
/*	All Rights Reserved			*/

/*
 * $XConsortium: MarkIconMotif.hh /main/6 1996/06/11 16:14:02 cde-hal $
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

#include <WWL/wwl.h>

#include <WWL/WXmDialogShell.h>
#include <WWL/WXmPushButton.h>
#include <WWL/WXmList.h>

class MarkCanvas;
#ifdef UseTmlRenderer
class WViewport;
class ViewportAgent;
#endif

class MarkIcon : public WWL, public WXmPushButton
{
public:
  MarkIcon (Widget parent, Widget chooser_parent,
	    MarkCanvas *, int ypos, int scrollbar_offset);
  virtual ~MarkIcon();

  void append (MarkCanvas *);
  void insert (MarkCanvas *);
  void remove (MarkCanvas *);

  unsigned int ypos ()	{ return f_y_position ; }

  unsigned int mark_count()
    { return (f_mark_list.length()); }

  // Offset of the last MarkTml associated with this Icon. 
  unsigned int offset() const
    { return (f_offset); }

  // For MarkTml use ONLY:
  // (MarkTml sends a message for these operations, but it's
  // inefficient for the MarkIcon to request these messages, so
  //  these routines provide a direct notification method instead.) 
  void select (MarkCanvas *);
  void deselect (MarkCanvas *);
  void update_icon (MarkCanvas *);

protected:
  void register_actions ();

private:
  void swap_colors();
  void highlight();
  void unhighlight();

  // User actions: 
#ifdef UseFJMTF
  friend void icon_activate(Widget, XtPointer, XEvent);
  void activate ();
#else
  void activate (WCallback *);
#endif
  void select_mark_timeout();
  static void static_double_click(Widget , XEvent *, String *, Cardinal *);
  static void static_single_click(Widget , XEvent *, String *, Cardinal *);
  void double_click(XEvent *event );
  void single_click(XEvent *event );


private:
  // MarkTml objects are stored in reverse order in the list
  // to allow quick access to the last element. 
  xList<MarkCanvas *> f_mark_list;
  char             f_icon_char;
  bool             f_selected;
  unsigned int     f_offset;
  unsigned int	   f_y_position ;
  Widget	   f_chooser_parent ;
};
