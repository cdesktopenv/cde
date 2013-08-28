/* $XConsortium: NodeWindowAgentMotif.hh /main/24 1996/11/23 14:13:00 cde-hal $ */
/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1994, 1995, 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */
/*
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

#include "UAS.hh"

class NodeViewInfo;
class NodeHandle;
class WTopLevelShell;
class WXmPushButton;
class Manager;
class WXmToggleButton;
class WXmOptionMenu;
class WXmPulldownMenu;
class WXmRowColumn;
class WXmScrolledWindow;
class WXmFrame;
class WXmTextField;
class WXmCascadeButton;
class WXmForm;
class HitList;
class Ancestor;
class SelectionChanged;
class BookTab;
class PixmapGraphic;
class MarkSelectionChanged;
class ScopeMenu;
class Graphic;
class Mark;
class MarkCreated;
class MarkDeleted;
class MarkCanvas ;
class MarkIcon ;
// class DetachGraphic;
// class ReAttachGraphic;
// class DisplayGraphic;

class GraphicsHandler ;
// self is used by some MACROS somewhere, but the X include files use
// it as something different

// we require #undef XtIsManaged, XtWindow and XtDisplay because they are defined
// as a macros in the Private Xm Header files, but we rely on the
// Function definitions to cause automatic casts to Widgets in our .C
// file - (brad) 


#undef self
#include <DtI/DisplayAreaP.h>
#define self *this
#undef XtIsManaged
#undef XtWindow
#undef XtDisplay

class NodeWindowAgent : public Agent,
			public UAS_Receiver<SelectionChanged>,
			public UAS_Receiver<MarkSelectionChanged>,
			public UAS_Sender<SelectionChanged>,
			public UAS_Receiver<MarkCreated>,
			public UAS_Receiver<MarkDeleted>,
			public UAS_Receiver<UAS_LibraryDestroyedMsg>,
			public IcccmAgent
{
public: // functions

friend class Ancestor;

  NodeWindowAgent(u_int serial_no = 0);
  ~NodeWindowAgent();

  void display (UAS_Pointer<UAS_Common> &n);

  unsigned long last_access_time ()
    { return (f_last_access_time); }

  // NOTE: check re_display() before changing operation of these routines - jbm
  // Returns TRUE if willing to display the specified node.
  bool can_display (NodeHandle &)
    { return (!f_locked); }
  bool locked()
    { return (f_locked); }

  void update_fonts(unsigned int serial_number = 0);

  // if graphic is visible, refresh screen 
  void refresh(const UAS_Pointer<Graphic> &);

  // re-display node via display() functions...when style sheet changes
  void re_display();

  void selection_changed ();
  bool selection_contains_graphic();

  // only for use by the Canvas Help Display Area hypertext callback
  void link_to (const char *locator);

  // when the canvas sends us a resize callback
  void canvas_resize ();

  void vscroll (unsigned int value);
  void hscroll (unsigned int value);

  void arm ();
  void disarm() ;

  void detach( UAS_Pointer<Graphic> &);
  void reattach( UAS_Pointer<Graphic> &);

  NodeViewInfo *node_view_info() { return f_node_view_info; }

  void popup_menu(XButtonPressedEvent*);  // popup detach graphic menu
  void detach_gr(); // for detach popup menu
  void attach_gr(); // for detach popup menu
  void raise_gr(); // for detach popup menu
  void go_to_link();
  void open_new_node();
  void exit_cb();

public: // Motif-version functions (usable by other Motif agents only)

  void close_set_sensitive(Boolean set) {
      f_close_sensitive = set; if (f_close) XtSetSensitive(f_close, set); }

private: // functions
  void create_ui();

  // Callbacks
  static void printCB(Widget, XtPointer, XtPointer);
  static void print_asCB(Widget, XtPointer, XtPointer);
  void print_panel_activate();
  static void dismissCB(Widget, XtPointer, XtPointer);
  void dismiss ();		// close the window, blow away the node
  static void cloneCB(Widget, XtPointer, XtPointer);
  static void lock_toggleCB(Widget, XtPointer, XtPointer);
  static void node_nextCB(Widget, XtPointer, XtPointer);
  static void node_previousCB(Widget, XtPointer, XtPointer);
  static void preview_nextCB(Widget, XtPointer, XtPointer);
  static void preview_previousCB(Widget, XtPointer, XtPointer);
  static void preview_history_nextCB(Widget, XtPointer, XtPointer);
  static void preview_history_prevCB(Widget, XtPointer, XtPointer);
  void history_display();
  static void history_nextCB(Widget, XtPointer, XtPointer);
  static void history_prevCB(Widget, XtPointer, XtPointer);
  static void search_previousCB(Widget, XtPointer, XtPointer);
  void search_previous();
  static void search_nextCB(Widget, XtPointer, XtPointer);
  void search_next();
  static void search_help (Widget w, XtPointer client_data,
			   XEvent *event, Boolean *);
  void display_version();
  void initialize_controls();
  void initialize_hierarchy();
  void initialize_history();
  void record_history_position();
  void reset_hierarchy();
  void initialize_path();
  void initialize_tabs();
  void select_tab();
  void title_menu_activate (WCallback *);
  void map_activate();

  static void detach_graphicCB(Widget, XtPointer, XtPointer);
  void detach_graphic();
  _DtCvSegment *xy_to_seg(int, int, _DtCvElemType*); 

  void static dialog_mapped (Widget, XtPointer, XEvent *, Boolean *);


  void receive (SelectionChanged &, void *client_data);
  void preview_init (UAS_Pointer<UAS_Common> &doc_ptr);
  void preview (WTimeOut *);
  static void unpreviewCB(Widget, XtPointer, XtPointer);
  void unpreview();
  void text_callback(WCallback *wcb);

  void add_quick_help (Widget, const char*);
  static void quick_helpEH (Widget, XtPointer, XEvent *, Boolean *);

  void receive (MarkSelectionChanged &, void *client_data);

  static void create_annotationCB(Widget, XtPointer, XtPointer);
  static void create_bookmarkCB(Widget, XtPointer, XtPointer);
  static void edit_markCB(Widget, XtPointer, XtPointer);
  static void move_markCB(Widget, XtPointer, XtPointer);
  void move_mark();
  static void delete_markCB(Widget, XtPointer, XtPointer);
  void delete_mark();
  void set_min_size();

  static void search_on_selectionCB(Widget, XtPointer, XtPointer);
  void do_search_selection(const char*, unsigned long);
  static void clear_searchCB(Widget, XtPointer, XtPointer);
  void clear_search_hits_activate();

  int make_bookmark(Boolean edit, MarkCanvas* refmark = NULL);
  void do_bookmarks(bool move_to_mark = TRUE);
  MarkCanvas *add_mark (UAS_Pointer<Mark> &);
  void cleanup_marks();
  void show_mark (MarkCanvas *, bool move_to_mark = TRUE);
  void receive (MarkCreated &message, void *client_data);
  void receive (MarkDeleted &message, void *client_data);

  void layout_mark_icons() ;

  void delete_mark_visuals (UAS_Pointer<Mark> &mark_ptr);
  MarkCanvas *create_canvas_mark (_DtCvHandle canvas, NodeViewInfo*, UAS_Pointer<Mark>&);
  void select_mark_in_canvas (MarkCanvas *, bool show_it);
  void deselect_mark_in_canvas (MarkCanvas *);
  void receive (UAS_LibraryDestroyedMsg &msg, void *client_data);

  void replace(UAS_Pointer<Graphic> &gr);

#ifdef DEBUG
  // debugging stuff 
  void document_id_display();
#endif

  static void show_locatorCB(Widget, XtPointer closure, XtPointer);
  void show_locator();

#ifdef MOSAIC_SUPPORT
  void import_mosaic_hotlist();
#endif

#ifdef DEBUG
#ifdef MONITOR
  void monitor(WCallback *);
#endif
#endif

  void SetTopic(_DtCvTopicPtr);


private: // class

private: // variables
  NodeViewInfo          *f_node_view_info;   // Display specific view info. 
  
  // NOTE: Maybe these losers should be references?!
  WTopLevelShell	*f_shell;

  DtHelpDispAreaStruct *f_help_dsp_area ;

  Widget        f_close;
  Widget        f_node_prev, f_node_prev2, f_node_prev3;
  Widget        f_node_next, f_node_next2, f_node_next3;
  Widget        f_history_prev, f_history_prev2;
  Widget        f_history_next, f_history_next2;
  Widget        f_search_prev, f_search_prev2;
  Widget        f_search_next, f_search_next2;
  WXmOptionMenu         *f_title_option;
  WXmPulldownMenu       *f_title_menu;
  Widget        f_status_text;
  WXmTextField          *f_search_text;
  ScopeMenu            *f_scope_menu;
  WXmRowColumn		*f_tab_area;
  WXmFrame              *f_frame;    // 94/10/21 haya for get nodeview height.
  Widget         f_create_bmrk;
  Widget         f_create_anno;
  Widget         f_create_link;
  Widget         f_move_mark;
  Widget         f_edit_mark;
  Widget         f_delete_mark;
  Widget         f_detach_graphic;
  Widget         f_detach_menu; // popup detach graphic menu
  Widget         f_detach_button;
  Widget         f_attach_button;
  Widget         f_raise_button;
  Widget	 f_preview_menu; // popup link preview
  Widget	 f_preview_label;
  Widget	 f_default_menu; // popup default menu
  UAS_String	 f_link_spec;
  Widget	 f_clear_search_hits;
  WXmPushButton		*f_graphical_map;
  WXmForm		*f_search_form;
  Widget	f_search_menu_button;
  Widget	f_print, f_print2;
  Widget	f_print_as;
  xList<Ancestor *>      f_ancestor_list;
  Ancestor              *f_current_ancestor;
  Widget		f_form;

  u_int                 f_move_mark_sensitive;
  u_int			f_last_access_time;
  bool		        f_locked;
  WCallback            *f_wm_delete_callback;

  xList<BookTab *>      f_tab_btn_list;

  UAS_Pointer<UAS_Common>     f_preview_document;
  WTimeOut             *f_preview_timeout;

  u_int			f_serial_number;

  LocalHistoryMgr       f_history_list;
  bool                  f_history_display;

  // Marks (from ViewportAgent - brad 12/20/95)
  xList<MarkCanvas *>	f_mark_list ;
  xList<MarkIcon   *>	f_mark_icon_list ;

  unsigned int		f_vscrollbar_offset ;
  unsigned int		f_hscrollbar_offset ;
  _DtCvSegment         *f_graphic_segment;

  GraphicsHandler      *f_graphics_handler ;

  bool                    f_close_sensitive;

  static unsigned long g_black;
  static unsigned long g_grey;

  UAS_List<UAS_String> f_textstore;
};

// use a new GraphicsHandler for each new node displayed
// GraphicsHandler will get/send graphics type messages and by deleting it
// each time, we get automagic cleanup of Sender/Receiver stuff
class GraphicsHandler : public UAS_Receiver<DetachGraphic>,
                        public UAS_Sender<DetachGraphic>,
			public UAS_Receiver<ReAttachGraphic>,
			public UAS_Sender<DisplayGraphic>
{
public:
  GraphicsHandler(NodeWindowAgent *agent);
  virtual ~GraphicsHandler();

  void receive(DetachGraphic &, void *client_data);
  void receive(ReAttachGraphic &, void *client_data);

  void display_graphic(UAS_Pointer<Graphic> &);
  

private:
  NodeWindowAgent	*f_agent;
};

