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
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*
 * $XConsortium: SearchQueryAgentMotif.hh /main/3 1996/06/11 16:16:15 cde-hal $
 *
 * Copyright (c) 1991 HaL Computer Systems, Inc.  All rights reserved.
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

class WTopLevelShell;
class WXmText;
class Manager;
class WXawPorthole;
class WXawTree;
class WXawPanner;
class WXmOpButton;
class PNode;

class SearchQueryAgent : public Agent
{
public: // functions
  SearchQueryAgent (Manager &my_manager);

  void init ();
  void display ();

  // The following are all action procs
  // NOTE: prototypes should be Widget, XEvent *, String *, Cardinal * 
  static void select_start (Widget, XEvent, String, Cardinal);
  static void select_end (Widget, XEvent, String, Cardinal);
  static void forward_thing (Widget, XEvent, String, Cardinal);
  static void backward_thing (Widget, XEvent, String, Cardinal);
  static void delete_thing (Widget, XEvent, String, Cardinal);
  static void next_empty_text (Widget, XEvent, String, Cardinal);

private: //functions
  void create_window ();
  void new_query ();
  void sensitize_buttons ();
  void modify_verify (WCallback *wcb);
  void motion_verify (WCallback *wcb);
  void insert_in_string (XmTextPosition start, XmTextPosition end,
			 char *text, int length);
  void search_activate (WCallback *);
  inline bool in_string (XmTextPosition position);
  void reset_text (WCallback *);

#ifdef DEBUG
  void create_pwindow ();
  void panner_movement (WCallback *);
  void porthole_resize (WCallback *);
  void display_tree_cb (WCallback *);
  void display_tree (PNode *node);
#endif
    

private: // variables
  WTopLevelShell	*f_shell;
  WXmText		*f_text;
  WXmOpButton           *f_button[8];

  PRoot			*f_root;	    // root of the parse tree
  long                   f_cursor;	    // position of cursor
  bool                f_self_modifying;
  int                    f_item_start;      // start pos of active item

#ifdef DEBUG
  WTopLevelShell        *f_pshell;
  WXawPorthole          *f_porthole;
  WXawPanner            *f_panner;
  WXawTree              *f_tree;
#endif
};

// inline:

inline
SearchQueryAgent::SearchQueryAgent (Manager &my_manager)
: Agent (my_manager)
{
}
