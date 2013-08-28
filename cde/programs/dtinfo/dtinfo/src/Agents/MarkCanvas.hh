/*
 * $XConsortium: MarkCanvas.hh /main/6 1996/07/10 09:36:01 rcs $
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

// Device dependent wrapper for the device Independent Mark 

#include "UAS.hh"

#include <X11/Intrinsic.h>


class Agent;
class Mark;
class MarkIcon;
class MarkSelectionChanged;
class MarkChanged;

class MarkCanvas : public Destructable, 
		   public UAS_Receiver<MarkChanged> 
{
public:
  MarkCanvas (UAS_Pointer<Mark>, Agent *agent, 
	      unsigned int offset, unsigned int ypos);
  virtual ~MarkCanvas();

  STATIC_SENDER_HH (MarkSelectionChanged);

  UAS_Pointer<Mark> &mark_ptr()
    { return (f_mark_ptr); }

  unsigned int offset() const
    { return (f_full_offset); }

  unsigned int y_position() 
    { return f_y_position ; }
  unsigned int y_position (unsigned int ypos)
    { f_y_position = ypos ; return ypos ; }

  MarkIcon *mark_icon() const
    { return (f_mark_icon); }
  void set_mark_icon (MarkIcon *mark_icon)
    { f_mark_icon = mark_icon; }

  // indicate if the select should also attempt to bring the mark into
  // the viewing area
  void select(bool move_to = 0);
  void deselect();

  bool selected() const
    { return (g_selected_mark == this); }
  static MarkCanvas *selected_mark() 
    { return (g_selected_mark); }

  Agent *agent () { return f_agent ; }

private: // functions
#ifdef JBM
  void compute_location();
#endif
  void receive (MarkChanged &, void *client_data);

private: // variables
  UAS_Pointer<Mark>   f_mark_ptr;
  MarkIcon       *f_mark_icon;
  unsigned int    f_full_offset;
  unsigned int	  f_y_position ;
  Agent		 *f_agent ;

  // This variable is purposely not part of the Mark object becase the
  // the decision to have only one selected Mark is tied to the UI, not
  // the Mark object itself.  DJB 11/02/92 
  static MarkCanvas *g_selected_mark;
};


struct MarkSelectionChanged
{
  enum selection_type { SELECT, DESELECT };
  selection_type  f_selection_type;
  bool            f_move_to ;
};


/*

  Data for positional restore:
    (for now just offset and length for simplicity)

  preceeding locator
  offset
  first n characters
  length
  last n characters

*/


#if 0

 NOTES:

MarkCanvas holds selection flag
MarkIcon sends message on select/deselect

selected mark ptr is maintained in node window agent

#endif
