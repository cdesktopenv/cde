/*
 * $XConsortium: WindowSystemMotif.hh /main/15 1996/11/27 12:29:47 rcs $
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

#include <WWL/WApplicationShell.h>
#include "Other/AppPrintData.hh"

enum WaitState {
    WS_ON,
    WS_OFF
};

//  VideoShell structure definition
 
typedef struct _VideoShell
{
    Widget widget;
    Boolean print_only;
    String file_name;
    Boolean hierarchy;
    String printer;
    int copies;
    String paper_size;
    Boolean silent;
} VideoShell;

typedef void _DtHelpDeSelectAll_t(_WidgetRec*,_XEvent*,char**,unsigned*);
typedef _DtHelpDeSelectAll_t * DeSelectAll_ptr;
typedef void _DtHelpSelectAll_t(_WidgetRec*,_XEvent*,char**,unsigned*);
typedef _DtHelpSelectAll_t * SelectAll_ptr;
typedef void _DtHelpActivateLink_t(_WidgetRec*,_XEvent*,char**,unsigned*);
typedef _DtHelpActivateLink_t * ActivateLink_ptr;
typedef void _DtHelpCopyAction_t(_WidgetRec*,_XEvent*,char**,unsigned*);
typedef _DtHelpCopyAction_t * CopyAction_ptr;
typedef void _DtHelpPageUpOrDown_t(_WidgetRec*,_XEvent*,char**,unsigned*);
typedef _DtHelpPageUpOrDown_t * PageUpOrDown_ptr;
typedef void _DtHelpPageLeftOrRight_t(_WidgetRec*,_XEvent*,char**,unsigned*);
typedef _DtHelpPageLeftOrRight_t * PageLeftOrRight_ptr;
typedef void _DtHelpNextLink_t(_WidgetRec*,_XEvent*,char**,unsigned*);
typedef _DtHelpNextLink_t * NextLink_ptr;
typedef void XmuCvtStringToGravity_t(XrmValue*,unsigned*,XrmValue*,XrmValue*);
typedef XmuCvtStringToGravity_t * XmuCvtStringToGravity_ptr;

extern "C"
{
  typedef void core_dump_handler_t(int signal_number);
  typedef core_dump_handler_t * core_dump_handler_ptr;
  typedef void interrupt_handler_t(int signal_number);
  typedef interrupt_handler_t * interrupt_handler_ptr;
  typedef int xevent_error_aborter_t(Display *display,XErrorEvent* error_event);
  typedef xevent_error_aborter_t * xevent_error_aborter_ptr;
}

class WindowGeometry;

class WindowSystem : public WWL, public FolioObject
{
  struct CursorStackElement
    {
      Cursor cursor;		// Cursor set at this stack level. 
      Widget exception;		// Any exception to this cursor. 
    };
public:  // functions
  
  // Constructing
  WindowSystem (int &argc, char *argv[]);
  
  // Destructing 
  ~WindowSystem();		

  // Initialization
  void init();
  
  // Accessing
  XtAppContext  	app_context(){ return f_application_context ; } 
  XtAppContext  	applicationContext() { return app_context(); }
  AppPrintData *        GetAppPrintData() { return f_AppPrintData; };

  WApplicationShell &toplevel();
  WApplicationShell &printToplevel();
  WApplicationShell &onlineToplevel();
  Display      	       *display();
  Screen      	       *screen();

  Boolean  printing();
  Display      	       *printDisplay();
  Display      	       *onlineDisplay();
  void      	       setPrintDisplay(Widget widget);
  void      	       setPrinting(Boolean state);
  VideoShell   	       *videoShell();
  Atom WM_DELETE_WINDOW()
    { return f_wm_delete_window; }
  
  Pixmap locked_pixmap(Widget w);
  Pixmap unlocked_pixmap(Widget w);
  Pixmap semilocked_pixmap(Widget w);
  
  Pixmap  default_pixmap (Dimension *width, Dimension *height);
  // NOTE: until we get a real detached pixmap, use default
  Pixmap  detached_pixmap(Dimension *width, Dimension *height);
  
  static WindowSystem &window_system()
    { return (*f_window_system); }
  
  // utility 
  XmString make_space(int num_pixels, Widget w);
  
  void beep()
    { XBell (f_display, 0); }

  void show_all_windows();

  // use this only after a fork!!
  void close_display_connection();
  
  // Color and Preferences
  
  // get_color returns 0 for failure 
  unsigned long get_color(const char *colorval, unsigned long &pixel) const ;
  
  // Execution
  void		run();
  
  Cursor create_cursor (const char *filename);
  
  void register_shell (WShell *);
  void register_full_modal_shell (WShell *);
  void register_primary_modal_shell (WShell *);
  void set_cursor (Cursor cursor, Widget exception = NULL);
  void reset_cursor (WCallback *wcb = NULL);
  void full_modal_cursor (WCallback *wcb = NULL);
  void primary_modal_cursor (WCallback *wcb = NULL);
  void wait_cursor()
    { set_cursor (f_wait_cursor); }
  
  // NOTE: replace calls to set_wait_state with other calls 
  void      set_wait_state (WaitState state)
    { if (state == WS_ON) wait_cursor(); else reset_cursor(); }
  
  // Get bool value from resource file
  bool get_boolean_app_resource (const char *name)
    { return (get_boolean_default (name)); }
  
  bool get_boolean_default (const char *name);
  int get_int_default (const char *name);
  const char *get_string_default (const char *name);
  const WindowGeometry &get_geometry_default (const char *name);
#if 1
  Pixel get_color_default(const char *name);
#endif  
  // Get message from resource file
  char *get_message(const char *message_name);
  
  void update_display();

  bool nofonts()
    { return f_nofonts; }

  XmFontListEntry dtinfo_font();
  XmFontListEntry dtinfo_space_font();

private: // functions
  void unregister_shell (WCallback *);
  static void core_dump_handler (int signal_number);
  static void interrupt_handler (int signal_number); 
 
  Pixmap	read_pixmap(const char *name,
			    Dimension *width,
			    Dimension *height);
  
protected: // variables
  int		       *f_argc;
  char		      **f_argv;
  WApplicationShell     f_toplevel;
  Display	       *f_display;
  Display	       *f_print_display;
  Boolean               f_printing;
  Screen	       *f_screen;
  Cursor                f_wait_cursor;
  Cursor                f_modal_cursor;
  Cursor                f_bomb_cursor;
  Atom                  f_wm_delete_window;
  Pixmap                f_locked_pixmap;
  Pixmap                f_unlocked_pixmap;
  Pixmap                f_semilocked_pixmap;
  Pixmap                f_default_pixmap;
  Pixmap                f_default_print_pixmap;
  // Node Pixmaps - default and detached 
  Dimension             f_defpix_width;
  Dimension             f_defpix_height;
  Dimension             f_print_defpix_width;
  Dimension             f_print_defpix_height;
  Pixmap		f_detached_pixmap ;
  Dimension		f_detached_width ;
  Dimension		f_detached_height ;
  List                  f_shell_list;
  CursorStackElement    f_cursor_stack[16];
  short                 f_cursor_stack_pos;
  bool			f_nofonts;
  static WindowSystem  *f_window_system;
  AppPrintData *        f_AppPrintData;   
  VideoShell * f_video_shell;        
  WApplicationShell     f_print_toplevel;
  Screen	       *f_print_screen;
  XmFontListEntry	f_dtinfo_font;
  XmFontListEntry	f_dtinfo_space_font;
public:
  XtAppContext		f_application_context;

};


inline WindowSystem &
window_system()
{
  return (WindowSystem::window_system());
}


class Wait_Cursor : public Destructable
{
public:
  Wait_Cursor()
    { window_system().set_wait_state (WS_ON); }
  ~Wait_Cursor()
    { window_system().set_wait_state (WS_OFF); }
};

enum { DTINFO_FONT, DTINFO_SPACE_FONT };

extern XmFontListEntry dtinfo_font(int);

