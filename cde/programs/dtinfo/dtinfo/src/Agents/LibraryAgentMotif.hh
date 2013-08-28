/* $TOG: LibraryAgentMotif.hh /main/17 1998/07/27 14:20:31 mgreess $ */
/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */
/*
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

#include "UAS.hh"
#include <X11/Intrinsic.h>

class OutlineListView;
class OutlineListView;
class WTopLevelShell;
class ScopeMenu;
class IcccmAgent;

struct OutlineListSelectionData;

class TrackingEntry;

class LibraryAgent : public Agent,
		     public UAS_Receiver<UAS_DocumentRetrievedMsg>

{
public: // functions 
  LibraryAgent();
  ~LibraryAgent();

  // load infolib specified in any format
  static int add_library(char*, Widget parent=NULL);  

  void display (OutlineList *library);
  void track_to (UAS_Pointer<UAS_Common> &node_ptr);
  void track (bool scroll = FALSE);
  void popup();
  bool popped_down() { return f_popped_down; }
  void keep_forever()
    { f_keep_forever = TRUE; }

  OutlineList *outline_list();

  void library_removed (UAS_Pointer<UAS_Common> lib);

  void close_set_sensitive(Boolean set) {
      f_close_sensitive = set; if (f_close) XtSetSensitive(f_close, set); }
  void copy_to_clipbd();

protected: // functions
  void create_ui();
  static void close_windowCB(Widget, XtPointer, XtPointer);
  void close_window();
  void ui_destroyed(); // leave in for backward compatibility
  static void ui_destroyedCB(Widget, XtPointer, XtPointer);
  static void detach_subtreeCB(Widget, XtPointer, XtPointer);
  void detach_subtree();
  static void add_libraryCB(Widget, XtPointer, XtPointer);
  static void remove_libraryCB(Widget, XtPointer, XtPointer);
  void entry_selected (void *, u_int notify_type);
  static void display_nodeCB(Widget, XtPointer, XtPointer);
  void display_node ();
  static void printCB(Widget, XtPointer, XtPointer);
  static void print_asCB(Widget, XtPointer, XtPointer);
  void print(WCallback *);
  void free_tracking_hierarchy();
  static void auto_track_toggleCB(Widget, XtPointer, XtPointer);
  // callbacks for use by drag and drop
  static void destCB(Widget, XtPointer, XtPointer);
  static void transferCB(Widget, XtPointer, XtPointer);

  void receive (UAS_DocumentRetrievedMsg &message, void *client_data);

  void text_callback(WCallback *wcb);
  static void search_help (Widget w, XtPointer client_data,
			   XEvent *event, Boolean *);
  void add_quick_help (Widget, const char* help_text);
  static void quick_helpEH (Widget, XtPointer, XEvent *, Boolean *);
//  void test();

private: // variables
  //WTopLevelShell         *f_shell;
  Widget                    f_shell;
  OutlineListView        *f_doc_tree_view;
  OutlineElement         *f_oe;
  bool                    f_keep_forever;
  WCallback	         *f_wm_delete_callback;
  TrackingEntry          *f_tracking_hierarchy;
  bool                    f_popped_down;
  Widget     f_close;
  Widget     f_copy;
  Widget     f_detach;
  Widget     f_detach2;
  Widget     f_view;
  Widget     f_view2;
  Widget     f_print;
  Widget     f_print2;
  Widget     f_print_as;
  Widget     f_remove;
  Widget     f_remove2;
  Widget     f_auto_track;
  bool                    f_close_sensitive;
  Widget     f_status_text;
  ScopeMenu             *f_scope_menu;

  UAS_List<UAS_String> f_help_text;

};

// inline:
inline
LibraryAgent::LibraryAgent()
: f_shell (NULL),
  f_oe (NULL),
  f_keep_forever (FALSE),
  f_wm_delete_callback (NULL),
  f_tracking_hierarchy (NULL),
  f_popped_down (TRUE),
  f_close (NULL),
  f_close_sensitive (FALSE)
{
}
