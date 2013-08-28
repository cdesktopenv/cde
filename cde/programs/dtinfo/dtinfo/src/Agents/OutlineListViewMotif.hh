/*	Copyright (c) 1995 FUJITSU LIMITED	*/
/*	All Rights Reserved			*/

/*
 * $XConsortium: OutlineListViewMotif.hh /main/5 1996/07/26 11:31:19 rcs $
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

class List;
class OutlineElement;
class OutlineList;
class LibraryAgent;

#include <WWL/WXmList.h>

class OutlineListView : public WWL, public WXmList, public FolioObject
{

public:  // functions
  enum { ENTRY_SELECTED = FolioObjectLast, _LAST };
  enum { LEVEL_UNLIMITED = -1 };

  OutlineListView (const WComposite &parent, const char *name,
		   bool automanage = FALSE, 
		   bool enable_activate = FALSE);
  ~OutlineListView();

  // accessing
  OutlineList *list()
    { return (f_list); }
  OutlineElement *selection()
    { return (f_current_selection); }
  void set_list (OutlineList *list, BitHandle handle = 0);
  void set_agent (LibraryAgent *agent)
    { f_library_agent = agent; }

  unsigned int tracking_position()
    { return (f_tracking_position); }
  void track_to (OutlineElement *, unsigned int location, char icon);
  void untrack();

  int max_level()
    { return (f_max_level); }
  // NOTE: This method should make sure the new value is enforced immediately. 
  void max_level (int max)
    { f_max_level = max; }

  BitHandle data_handle()
    { return (f_data_handle); }
  BitHandle data_handle (BitHandle new_handle);

  unsigned int item_pos()
    { return (f_item_pos); }
  int selected_item_count()
    { return (f_selected_item_count); }
  List *selected_item_list();

  virtual void clear();

  void update_list (OutlineList *list, BitHandle handle);

protected: // functions
  void regen_list();
  void generate_table (OutlineList *list, XmStringTable &table,
		       bool *selected_list, u_int level, unsigned force = 0);
  void set_icon (OutlineElement *);
  void set_track_icon (OutlineElement *, u_int position, char icon);
  void register_actions ();
  OutlineElement *item_at(unsigned int position);
  OutlineElement *y_to_outline_element (Position y);
  Position icon_extent(OutlineElement *oe) const;
  XmString xmstring (OutlineElement *, unsigned force = 0, 
		     char track_icon = 0);
  XmString create_xm_string (OutlineElement*, int base_level, 
			     unsigned char track, char icon); 

  // update visuals to correspond with internal list 
  void update_highlighting (OutlineList *, u_int &item_pos);
  void update_highlighting_recursive (OutlineList *, u_int &item_pos); 

  // Not public because it doesn't update the visuals
  enum deselect_mode_t { DESELECT_ALL, DESELECT_VISIBLE };
  void deselect (OutlineList *list, deselect_mode_t mode);

  // Callbacks
  virtual void select (WCallback *);
  void activate (WCallback *);
  void printConvertCallback(WCallback *);
  
  // Actions 
  static void _select_start (Widget, XEvent *, String *, Cardinal *);
  static void _select_end (Widget, XEvent *, String *, Cardinal *);
  void select_start (Widget, XEvent *, String *, Cardinal *);
  void select_end (Widget, XEvent *, String *, Cardinal *);
  
protected: // view variables
  OutlineList    *f_list;
  BitHandle	  f_data_handle ;
  Dimension       f_margin;
  Dimension       f_icon_width;
  u_int           f_item_pos;	    // zero-based item position
  OutlineElement *f_outline_element;
  short           f_max_level;
  unsigned short  f_base_level;
  unsigned char   f_selection_policy;
  u_int           f_serial_number;
  OutlineElement *f_current_selection;
  u_int		  f_selected_item_count;
  unsigned char   f_tracking_possible;
  u_int           f_tracking_position;
  OutlineElement *f_tracking_element;
  LibraryAgent   *f_library_agent;
};
