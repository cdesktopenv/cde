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
/* $TOG: WindowSystemMotif.C /main/35 1998/04/20 12:54:36 mgreess $ */
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

#include <X11/Shell.h>
#include <X11/Xmu/Editres.h>
#include <Xm/Xm.h>
#include <Xm/XmP.h>
#include <Xm/Protocols.h>
#include <Xm/Scale.h>

#include <Xm/XpmP.h>

#include <Dt/Dt.h>
#include <Dt/EnvControlP.h>

typedef enum {
    DtIgnore,
    DtInformation,
    DtWarning,
    DtError,
    DtFatalError,
    DtInternalError
} DtSeverity;

extern "C" void _DtSimpleError( 
    char *progName,
    DtSeverity severity,
    char *help,
    char *format,
    ...) ;


extern "C" int _DtPrintDefaultErrorSafe(
    Display *dpy,
    XErrorEvent *event,
    char *msg,
    int bytes);


#if XmVersion < 1002
#include <X11/IntrinsicP.h>
#include <X11/CoreP.h>
#include <X11/StringDefs.h>
#endif /* XmVersion < 1002 */

#include <X11/Xmu/Converters.h>

#if XmVersion >= 1002
#include <Xm/RepType.h>
#include <Xm/AtomMgr.h>
#endif /* XmVersion >= 1002 */

#define C_TOC_Element
#define L_Basic

#define C_WindowGeometryPref
#define L_Preferences

#ifdef UseDlOpen
#   define C_XpmLib
#endif

#define C_WindowSystem
#define L_Other

#define C_ServiceMgr
#define C_MessageMgr
#define C_EnvMgr
#define L_Managers



#define USES_OLIAS_FONT

#include "Prelude.h"

#include "Other/XmStringLocalized.hh"
#include "Managers/CatMgr.hh"

#include "Registration.hh"

#include <WWL/WApplicationShell.h>
#include <WWL/WXmMessageBox.h>
#include <WWL/WXmPushButton.h>

// These guys are backups for when there is not enough
// colors to display the standard ones
#ifdef UseDlOpen
#include "graphic_unavailable.xbm"
#include "detached_bw.xbm"
#else
#include "graphic_unavailable.xpm"
#include "detached_bw.xpm"
#endif

// four bitmap images for user marks

#define mark_single_width 13
#define mark_single_height 13
static unsigned char mark_single_bits[] = {
   0x00, 0x00, 0xfe, 0x01, 0x82, 0x02, 0x82, 0x04, 0x82, 0x08, 0x82, 0x0f,
   0x02, 0x08, 0x02, 0x08, 0x02, 0x08, 0x02, 0x08, 0x02, 0x08, 0xfe, 0x0f,
   0x00, 0x00};

#define mark_double_width 15
#define mark_double_height 15
static unsigned char mark_double_bits[] = {
   0x00, 0x00, 0xf8, 0x07, 0x08, 0x0a, 0x0e, 0x12, 0x0a, 0x22, 0x0a, 0x3e,
   0x0a, 0x20, 0x0a, 0x20, 0x0a, 0x20, 0x0a, 0x20, 0x0a, 0x20, 0xfa, 0x3f,
   0x02, 0x08, 0xfe, 0x0f, 0x00, 0x00};

#define anno_single_width 13
#define anno_single_height 13
static unsigned char anno_single_bits[] = {
   0x00, 0x00, 0xfe, 0x01, 0x82, 0x02, 0xba, 0x04, 0x82, 0x08, 0xba, 0x0f,
   0x02, 0x08, 0xfa, 0x0b, 0x02, 0x08, 0xfa, 0x0b, 0x02, 0x08, 0xfe, 0x0f,
   0x00, 0x00};

#define anno_double_width 15
#define anno_double_height 15
static unsigned char anno_double_bits[] = {
   0x00, 0x00, 0xf8, 0x07, 0x08, 0x0a, 0xee, 0x12, 0x0a, 0x22, 0xea, 0x3e,
   0x0a, 0x20, 0xea, 0x2f, 0x0a, 0x20, 0xea, 0x2f, 0x0a, 0x20, 0xfa, 0x3f,
   0x02, 0x08, 0xfe, 0x0f, 0x00, 0x00};

#if defined(SVR4) || defined(linux) || defined(CSRG_BASED)
#include <signal.h>
#else
#include <sys/signal.h>
#endif

#ifdef hpux
typedef void (*SIG_PF)();
#endif

#include <unistd.h>
#include <stdio.h>

#include <DtI/ActionsI.h>

// virtual root finder
extern "C" {
#include "vroot.h"
}

extern Boolean print_server_error;

static String fallbacks[] = {
(char*)"Dtinfo.Print*background:white",
(char*)"Dtinfo.Print*renderTable:-dt-application-bold-r-normal-serif-0-0-0-0-p-0-iso8859-1",
(char*)"Dtinfo.Print*shadowThickness:0",
(char*)"Dtinfo.Print*highlightThickness:0",
(char*)"Dtinfo.Print*pform.marginHeight: 1in",
(char*)"Dtinfo.Print*pform.marginWidth: 1in",
(char*)"Dtinfo.Print*ptext.Attachment:attach_form",
NULL
};


static XtActionsRec DrawnBActions[] =
    {
        {(char*)"DeSelectAll"    , _DtHelpDeSelectAll    },
        {(char*)"SelectAll"      , _DtHelpSelectAll      },
        {(char*)"ActivateLink"   , _DtHelpActivateLink   },
        {(char*)"CopyToClipboard", _DtHelpCopyAction     },
        {(char*)"PageUpOrDown"   , _DtHelpPageUpOrDown   },
        {(char*)"PageLeftOrRight", _DtHelpPageLeftOrRight},
        {(char*)"NextLink"       , _DtHelpNextLink       }
    };



WindowSystem *WindowSystem::f_window_system;
static Atom XA_WM_STATE;

class Shell_Info {
public:
  Shell_Info (Widget w)
    : f_shell (w),
      f_size_hints (NULL),
      f_restore (False)
    { }

public:
  Widget f_shell;
  XSizeHints *f_size_hints;
  bool f_has_size_hints : 1;
  bool f_restore : 1;
  bool f_iconic : 1;
  bool f_has_wm_state : 1;
};


// /////////////////////////////////////////////////////////////////
// class constructor
// /////////////////////////////////////////////////////////////////

WindowSystem::WindowSystem (int &argc, char *argv[])
: f_printing(False),
  f_default_pixmap (0),
  f_defpix_width (0),
  f_defpix_height (0),
  f_detached_pixmap(0),
  f_shell_list (20),
  f_cursor_stack_pos(-1),
  f_dtinfo_font(NULL),
  f_dtinfo_space_font(NULL)
{
  f_argc = &argc;
  f_argv = argv;
  f_window_system = this;
  init();

  f_modal_cursor = create_cursor ("xm_noenter16");
  f_wait_cursor = create_cursor ("xm_hour16");
  f_bomb_cursor = create_cursor ("bomb");

  /* -------- Debugging setup.  Should be a function. -------- */

  bool debugging = get_boolean_app_resource("debug");
  if (!debugging)
    {
      signal (SIGABRT, /*DWC IBM (SIG_PF)*/ core_dump_handler);
      signal (SIGBUS, /*(SIG_PF)*/ core_dump_handler);
      signal (SIGSEGV, /*(SIG_PF)*/ core_dump_handler);
      signal (SIGINT, interrupt_handler);
    }

  /* Reference the scale widget so Veritas Replay Xt lib can link. */
  /* WidgetClass xxx = xmScaleWidgetClass; */

  int count;
  char **names = XListFonts(window_system().display(),
			    "-dt-dtinfo-*-14-*-dtinfo-?", 1, &count);

  if (count && names)
  {
    f_dtinfo_font = XmFontListEntryLoad(f_display, *names,
					XmFONT_IS_FONT, (char*)"olias");
    f_dtinfo_space_font = XmFontListEntryLoad(f_display, *names,
					      XmFONT_IS_FONT, (char*)"ospace");
    XFreeFontNames(names);
  }
}

// /////////////////////////////////////////////////////////////////////////
// Destructor
// /////////////////////////////////////////////////////////////////////////

WindowSystem::~WindowSystem()
{
  signal (SIGABRT, SIG_DFL);
  signal (SIGBUS, SIG_DFL);
  signal (SIGSEGV, SIG_DFL);
}

XmFontListEntry
WindowSystem::dtinfo_font() 
{
    return f_dtinfo_font; 
}

XmFontListEntry 
WindowSystem::dtinfo_space_font() 
{
    return f_dtinfo_space_font; 
}

WApplicationShell &
WindowSystem::toplevel()
{
    if (f_printing) {
	return (f_print_toplevel);
    }
    else {
	return (f_toplevel);
    }
}

WApplicationShell &
WindowSystem::printToplevel()
{
    return (f_print_toplevel);
}

WApplicationShell &
WindowSystem::onlineToplevel()
{
    return (f_toplevel);
}

Display *
WindowSystem::display()
{
    if (f_printing) {
	return (f_print_display);
    }
    else {
	return (f_display);
    }
}

Screen *
WindowSystem::screen()
{
    if (f_printing) {
	return (f_print_screen);
    }
    else {
	return (f_screen);
    }
}

Display *
WindowSystem::onlineDisplay()
{
    return (f_display);
}

Display *
WindowSystem::printDisplay()
{
    return (f_print_display);
}

Boolean
WindowSystem::printing()
{
    return (f_printing);
}

void
WindowSystem::setPrintDisplay(Widget widget) 
{
    f_print_toplevel = widget;
    f_print_display = XtDisplay(widget);
    f_print_screen = XtScreen(widget);
}

void
WindowSystem::setPrinting(Boolean state)
{
    f_printing = state;
}

void
WindowSystem::update_display()
{
  XmUpdateDisplay (f_toplevel);
}


// /////////////////////////////////////////////////////////////////
// init - initialize
// /////////////////////////////////////////////////////////////////

#ifdef JBM
extern "C" { void tml_dp_set_font_display(Display *); }
#endif

// command line options

// include "dbg" so we can use the startup script -debug flag independently 
XrmOptionDescRec options[] = {
  {(char*)"-debug",	(char*)"*debug",	XrmoptionNoArg,  (char*)"True"},
  {(char*)"-dbg",	(char*)"*debug",	XrmoptionNoArg,  (char*)"True"},
  {(char*)"-author",	(char*)"*author",	XrmoptionNoArg,  (char*)"True"},
  {(char*)"-readonly",	(char*)"*readonly",	XrmoptionNoArg,  (char*)"True"},
  {(char*)"-ro",	(char*)"*readonly",	XrmoptionNoArg,  (char*)"True"},
  {(char*)"-autohelp",	(char*)"*AutomaticHelp",XrmoptionNoArg,  (char*)"On"},
  {(char*)"-nofonts",	(char*)"*NoFonts",	XrmoptionNoArg,  (char*)"True"},
  {(char*)"-print",	(char*)"printOnly",	XrmoptionNoArg,  (char*)"True"},
  {(char*)"-hierarchy",	(char*)"hierarchy",	XrmoptionNoArg,  (char*)"True"},
  {(char*)"-printer",	(char*)"printer",	XrmoptionSepArg, NULL},
  {(char*)"-copies",	(char*)"copies",	XrmoptionSepArg, NULL},
  {(char*)"-paperSize",	(char*)"paperSize",	XrmoptionSepArg, NULL},
  {(char*)"-silent",	(char*)"silent",	XrmoptionNoArg,  (char*)"True"},
  {(char*)"-outputFile",(char*)"outputFile",	XrmoptionSepArg, NULL}
};

static XtResource VideoResources[] =
{
  {(char*)"printOnly", (char*)"PrintOnly", XmRBoolean, sizeof (Boolean),
	XtOffsetOf (VideoShell, print_only), XmRImmediate, (XtPointer)False,
  },
  {(char*)"outputFile", (char*)"OutputFile", XmRString, sizeof (char *),
	XtOffsetOf (VideoShell, file_name), XmRImmediate, (XtPointer)NULL,
  },
  {(char*)"hierarchy", (char*)"Hierarchy", XmRBoolean, sizeof (Boolean),
	XtOffsetOf (VideoShell, hierarchy), XmRImmediate, (XtPointer)False,
  },
  {(char*)"printer", (char*)"Printer", XmRString, sizeof (char *),
	XtOffsetOf (VideoShell, printer), XmRImmediate, (XtPointer)NULL,
  },
  {(char*)"copies", (char*)"Copies", XmRInt, sizeof (int),
	XtOffsetOf (VideoShell, copies), XmRImmediate, (XtPointer)1,
  },
  {(char*)"paperSize", (char*)"PaperSize", XmRString, sizeof (char *),
	XtOffsetOf (VideoShell, paper_size), XmRImmediate, (XtPointer)NULL,
  },
  {(char*)"silent", (char*)"Silent", XmRBoolean, sizeof (Boolean),
	XtOffsetOf (VideoShell, silent), XmRImmediate, (XtPointer)False,
  },
};

/*
 * static function declarations
 */
VideoShell* VideoShell_new(Display* display);


/*
 * ------------------------------------------------------------------------
 * Name: videoShell
 *
 * Description:
 *
 *     Access function for f_video_shell containing application resources
 *     and command line options.
 *
 * Return value:
 *
 *     A pointer to the VideoShell structure.
 */
VideoShell *
WindowSystem::videoShell()
{
  return (f_video_shell);
}

/*
 * ------------------------------------------------------------------------
 * Name: VideoShell_new
 *
 * Description:
 *
 *     Allocates a new VideoShell data structure.
 *
 *     This function creates a top level application shell on the passed
 *     video display.
 *
 * Return value:
 *
 *     A pointer to the new VideoShell structure.
 */
VideoShell*
VideoShell_new(Display*, Widget app_shell)
{
    VideoShell* me = (VideoShell*)XtCalloc(1, sizeof(VideoShell));

    me->widget = app_shell;

    XtGetApplicationResources(me->widget, me,
			      VideoResources, XtNumber(VideoResources),
			      NULL, 0);

    // Modify the videoShell data based on environment variable settings
  
    // if DTPRINTSILENT is set then turn silent printing on

    if (getenv("DTPRINTSILENT") != NULL) {
	me->silent = True;
    }

    return me;
}

static int
xevent_error_aborter(Display *display, XErrorEvent* error_event)
{
    #define _DTINFO_BUFSIZE 1024
    char error_msg[_DTINFO_BUFSIZE];

    // log error

    _DtPrintDefaultErrorSafe(display, error_event, error_msg, _DTINFO_BUFSIZE);
    _DtSimpleError((char*)"dtinfo", DtWarning, NULL, error_msg, NULL);
    
    // if the error occured on the print display we're going to set 
    // a variable so that and when the job is done, right before calling
    // XpEndJob, we call XpCancelJob, and notify the user.

    if ( (display == window_system().printDisplay()) &&
	 (error_event->error_code == BadAlloc) ) {	
	print_server_error = True;
    }

    return 0;

}

void
WindowSystem::init()
{
  XtSetLanguageProc(NULL, (XtLanguageProc)NULL, NULL);

  _DtEnvControl(DT_ENV_SET);

  Widget app_shell = XtAppInitialize(&f_application_context, CLASS_NAME,
					options, XtNumber(options),
					f_argc, f_argv, fallbacks, NULL, 0);
  f_display = XtDisplay(app_shell);

#ifdef XEV_DEBUG
  XSynchronize(f_display, True);
#endif
  XSync(f_display, False);
  XSetErrorHandler(xevent_error_aborter);

#if 0
  XtAppSetFallbackResources(f_application_context, fallbacks);
#endif

  //  exit if display cannot be opened.  this code will need to be revised
  //  if we ever want to handle gui-less printing

  if (f_display == (Display*)NULL)
  {
      fprintf(stderr, "%s",
		CATGETS(Set_WindowSystem, 3, "Unable to open display.\n"));
      exit(1);
  }
    
  //  Create the top level video shell

  f_video_shell = VideoShell_new(f_display, app_shell);

  //  Create a new AppPrintData object here and initialize it since 
  //  the $%^& doesn't seem to get called.

  f_AppPrintData = new AppPrintData;
  f_AppPrintData->f_print_data = (DtPrintSetupData*)XtCalloc(1, sizeof(DtPrintSetupData));
  f_AppPrintData->f_pform = NULL ;
  f_AppPrintData->f_print_dialog = NULL;
  f_AppPrintData->f_print_shell = NULL;
  f_AppPrintData->f_print_only = f_video_shell->print_only;
  f_toplevel = f_video_shell->widget;

  if (DtAppInitialize( f_application_context,
               f_display, f_toplevel, f_argv[0], (char*)CLASS_NAME) == False)
  {
     /* DtAppInitialize() has already logged an appropriate error msg */
     exit(-1);
  }

  // Make sure this shell never appears. 
  XtSetMappedWhenManaged (f_toplevel, False);

  XtAppAddActions (f_application_context, DrawnBActions, XtNumber (DrawnBActions));

  //  register_shell (f_toplevel) 

  f_display = XtDisplay(f_toplevel);
  // NOTE: Check for failure DJB
#ifdef JBM
  tml_dp_set_font_display(f_display);
#endif

  // Create atom used elsewhere by agents
  f_wm_delete_window = XmInternAtom (f_display, (char*)"WM_DELETE_WINDOW", False);
  XA_WM_STATE = XmInternAtom (f_display, (char*)"WM_STATE", False);

  // -------- Add some converters -------- //

#if XmVersion < 1002
  // This converter let's us specify form connections in resources
  static XtConvertArgRec parentCvtArg[] = {
    { XtWidgetBaseOffset, (XtPointer) XtOffsetOf(WidgetRec, core.parent),
      sizeof(Widget)}
  };
  XtSetTypeConverter (XtRString, XtRWindow, XmuNewCvtStringToWidget,
	      parentCvtArg, XtNumber (parentCvtArg), XtCacheNone, NULL);
#endif /* XmVersion < 1002 */

  XtAddConverter (XtRString, XtRGravity, XmuCvtStringToGravity, NULL, 0);

#if XmVersion >= 1002
  XmRepTypeInstallTearOffModelConverter();
#endif /* XmVersion >= 1002 */

  // NOTE: Really need to install a string to pixmap converter!
  /* -------- Get some pixmaps. -------- */

  f_screen = XtScreen(f_toplevel);
  f_nofonts = get_boolean_default ("NoFonts");

  f_locked_pixmap = 0;
  f_semilocked_pixmap  = 0;
  f_unlocked_pixmap = 0;

  XImage *mark_single =
	XCreateImage(f_display, DefaultVisualOfScreen(f_screen),
	1, XYBitmap, 0, (char*)mark_single_bits,
	mark_single_width, mark_single_height, 8, 0);
  mark_single->byte_order = LSBFirst;
  mark_single->bitmap_bit_order = LSBFirst;
  mark_single->bitmap_unit = 8;

  XImage *mark_double =
	XCreateImage(f_display, DefaultVisualOfScreen(f_screen),
	1, XYBitmap, 0, (char*)mark_double_bits,
	mark_double_width, mark_double_height, 8, 0);
  mark_double->byte_order = LSBFirst;
  mark_double->bitmap_bit_order = LSBFirst;
  mark_double->bitmap_unit = 8;

  XImage *anno_single =
	XCreateImage(f_display, DefaultVisualOfScreen(f_screen),
	1, XYBitmap, 0, (char*)anno_single_bits,
	anno_single_width, anno_single_height, 8, 0);
  anno_single->byte_order = LSBFirst;
  anno_single->bitmap_bit_order = LSBFirst;
  anno_single->bitmap_unit = 8;
		
  XImage *anno_double =
	XCreateImage(f_display, DefaultVisualOfScreen(f_screen),
	1, XYBitmap, 0, (char*)anno_double_bits,
	anno_double_width, anno_double_height, 8, 0);
  anno_double->byte_order = LSBFirst;
  anno_double->bitmap_bit_order = LSBFirst;
  anno_double->bitmap_unit = 8;

  XmInstallImage(mark_single, (char*)"mark_single");
  XmInstallImage(mark_double, (char*)"mark_double");
  XmInstallImage(anno_single, (char*)"anno_single");
  XmInstallImage(anno_double, (char*)"anno_double");
}

Pixmap 
WindowSystem::locked_pixmap(Widget w)
{ 
    Pixel  fg, bg;

    if (!f_locked_pixmap) {
	XtVaGetValues(w,
		      XmNforeground, &fg,
		      XmNbackground, &bg,
		      NULL);
	
	f_locked_pixmap = XmGetPixmap (f_screen, (char*)"locked.xbm", fg, bg);
	if (f_locked_pixmap == XmUNSPECIFIED_PIXMAP) {
	    fprintf(stderr, "Couldn't load locked.xbm.\n");
	}
    }
    
    return (f_locked_pixmap); 
}

Pixmap 
WindowSystem::unlocked_pixmap(Widget w)
{ 
    Pixel  fg, bg;

    
    if (!f_unlocked_pixmap) {
	XtVaGetValues(w,
		      XmNforeground, &fg,
		      XmNbackground, &bg,
		      NULL);
	
	f_unlocked_pixmap = XmGetPixmap (f_screen, (char*)"unlocked.xbm", fg, bg);
	if (f_unlocked_pixmap == XmUNSPECIFIED_PIXMAP) {
	    fprintf(stderr, "Couldn't load locked.xbm.\n");
	}
    }

    return (f_unlocked_pixmap); 
}

Pixmap 
WindowSystem::semilocked_pixmap(Widget w)
{ 
    Pixel  fg, bg;

    if (!f_semilocked_pixmap) {
	XtVaGetValues(w,
		      XmNforeground, &fg,
		      XmNbackground, &bg,
		      NULL);
	
	f_semilocked_pixmap = XmGetPixmap (f_screen, (char*)"semilocked.xbm", fg, bg);
	if (f_semilocked_pixmap == XmUNSPECIFIED_PIXMAP) {
	    fprintf(stderr, "Couldn't load locked.xbm.\n");
	}
    }

    return (f_semilocked_pixmap); 
}

// /////////////////////////////////////////////////////////////////
// run - main event handling loop
// /////////////////////////////////////////////////////////////////

void
WindowSystem::run()
{
    // don't realize the toplevel shell if we are doing print only

    toplevel().Realize();

    service_manager().establish_server();

    XtAppMainLoop (app_context());
}


// return value is success/failure value 
unsigned long
WindowSystem::get_color( const char * colorval, unsigned long &pixel) const
{
    // NOTE: do we have to free these colors on exit ??? 


    pixel = 0 ;			// a reasonable value on failures ! 

    XColor xcolor ;
    Status status ;
    Colormap colormap = DefaultColormap(f_display,DefaultScreen(f_display));

    status = XParseColor( f_display,
			  colormap,
			  colorval,
			  &xcolor);
    
    //  return now if call failed
    if (!status)
      return status ;

    // now see if we can get the requested color
    status = XAllocColor( f_display,
			  colormap,
			  &xcolor );

    if (status)
      pixel = xcolor.pixel ;

    return status ;

}


// /////////////////////////////////////////////////////////////////
// register_shell - register a toplevel shell for cursor stuff
// /////////////////////////////////////////////////////////////////

// TMP hack for bogus HGL help lib
extern "C" { void olias_register_shell (Widget); }
void olias_register_shell (Widget shell)
{
  window_system().register_shell ((WShell *) shell);
}

void
WindowSystem::register_shell (WShell *shell)
{
  Shell_Info *si = new Shell_Info (*shell);
  f_shell_list.append ((FolioObject *) si);

  shell->SetDestroyCallback (this, (WWL_FUN)&WindowSystem::unregister_shell,
			     (void *) si);

  XtAddEventHandler (*shell, (EventMask) 0, TRUE,
		     (XtEventHandler) _XEditResCheckMessages, NULL);
}

void
WindowSystem::register_full_modal_shell (WShell *shell)
{
  Shell_Info *si = new Shell_Info (*shell);
  f_shell_list.append ((FolioObject *) si);

  shell->SetPopupCallback (this, (WWL_FUN) &WindowSystem::full_modal_cursor);
  shell->SetPopdownCallback (this,(WWL_FUN) &WindowSystem::reset_cursor);
  shell->SetDestroyCallback (this, (WWL_FUN) &WindowSystem::unregister_shell,
			     (void *) si);

  XtAddEventHandler (*shell, (EventMask) 0, TRUE,
		     (XtEventHandler) _XEditResCheckMessages, NULL);
}

void
WindowSystem::register_primary_modal_shell (WShell *shell)
{
  Shell_Info *si = new Shell_Info (*shell);
  shell->SetPopupCallback (this, (WWL_FUN)&WindowSystem::primary_modal_cursor);
  shell->SetPopdownCallback (this,(WWL_FUN) &WindowSystem::reset_cursor);
  shell->SetDestroyCallback (this, (WWL_FUN) &WindowSystem::unregister_shell,
			     (void *) si);

  XtAddEventHandler (*shell, (EventMask) 0, TRUE,
		     (XtEventHandler) _XEditResCheckMessages, NULL);
}


// /////////////////////////////////////////////////////////////////
// unregister_shell
// /////////////////////////////////////////////////////////////////

void
WindowSystem::unregister_shell (WCallback *wcb)
{
  // Reset the cursor if it is currently set because of this shell. 
  if (f_cursor_stack_pos >= 0 &&
      f_cursor_stack[f_cursor_stack_pos].exception == wcb->GetWidget())
    reset_cursor (wcb);

  Shell_Info *si = (Shell_Info *) wcb->ClientData();
  f_shell_list.remove ((FolioObject *) si);
  if (si->f_size_hints != NULL)
    XFree ((char *) si->f_size_hints);
  delete si;
}


// /////////////////////////////////////////////////////////////////
// set_cursor - turn a cursor on
// /////////////////////////////////////////////////////////////////

void
WindowSystem::set_cursor (Cursor cursor, Widget exception)
{
  Widget shell;
  //  ON_DEBUG (printf ("Cursor change <%d>\n", f_cursor_stack_pos));
  for (unsigned int i = 0; i < f_shell_list.length(); i++)
    {
      shell = ((Shell_Info *) f_shell_list[i])->f_shell;
      if (shell != exception && XtWindow (shell) != 0)
	{
	  //  ON_DEBUG (printf("  Setting cursor on %s\n", XtName(shell)));
	  XDefineCursor (f_display, XtWindow (shell), cursor);
	}
    }
  XFlush (f_display);
  // Save the cursor setting on the cursor stack.
  ON_DEBUG (printf ("Setting cursor #%d\n", f_cursor_stack_pos + 1));
  assert (f_cursor_stack_pos <
	  (int)(sizeof(f_cursor_stack) / sizeof(f_cursor_stack[0])));
  f_cursor_stack_pos++;
  f_cursor_stack[f_cursor_stack_pos].cursor = cursor;
  f_cursor_stack[f_cursor_stack_pos].exception = exception;
}


// /////////////////////////////////////////////////////////////////
// reset_cursor - reset the cursor to the previous
// /////////////////////////////////////////////////////////////////

void
WindowSystem::reset_cursor (WCallback *wcb)
{
  // Make sure a cursor has been set first. 
  assert (f_cursor_stack_pos >= 0);
  Widget shell;
  Widget exception = wcb ? wcb->GetWidget() : NULL;
  f_cursor_stack_pos--;
  Cursor cursor = 0;
  if (f_cursor_stack_pos >= 0)
    cursor = f_cursor_stack[f_cursor_stack_pos].cursor;

  for (unsigned int i = 0; i < f_shell_list.length(); i++)
    {
      shell = ((Shell_Info *) f_shell_list[i])->f_shell;
      if (shell != exception && XtWindow (shell) != 0) {
	// Reset the previous cursor if there's one on the stack, revert
	// to default cursor if the stack is empty. 
	//  printf ("Resetting cursor on %s\n", XtName(shell));
	if (f_cursor_stack_pos >= 0 &&
	    shell != f_cursor_stack[f_cursor_stack_pos].exception)
	  XDefineCursor (f_display, XtWindow (shell), cursor);
	else
	  XUndefineCursor (f_display, XtWindow (shell));
      }
    }
  XFlush (f_display);
}


// /////////////////////////////////////////////////////////////////
// modal_cursor - turn on the modal cursor
// /////////////////////////////////////////////////////////////////

void
WindowSystem::full_modal_cursor (WCallback *wcb)
{
  set_cursor (f_modal_cursor, wcb ? wcb->GetWidget() : (Widget) NULL);
  // Make sure the cursor of a freshly popped up dialog is the default. 
  if (wcb && wcb->GetWidget())
    XUndefineCursor (f_display, XtWindow (wcb->GetWidget()));
}

void
WindowSystem::primary_modal_cursor (WCallback *wcb)
{
  // NOTE: really needs to traverse up hierarchy setting cursor on shell
  // parents! 
  set_cursor (f_modal_cursor, wcb ? wcb->GetWidget() : (Widget) NULL);
}


// /////////////////////////////////////////////////////////////////
// create_cursor
// /////////////////////////////////////////////////////////////////

Cursor
WindowSystem::create_cursor (const char *filename)
{
  Cursor cursor;
  Pixmap cursor_bits, cursor_mask;
  Screen *screen = DefaultScreenOfDisplay (f_display);
  unsigned short c = ~0;
  static XColor white = { 0,  c,  c,  c, DoRed | DoGreen | DoBlue };
  static XColor black = { 0,  0,  0,  0, DoRed | DoGreen | DoBlue };
  int  hot_x, hot_y;
  int depth, len;
  Boolean success;

  // Get the cursor pixmap. 
  cursor_bits = XmGetPixmapByDepth (screen, (char *) filename, 1, 0, 1);
  if (cursor_bits == XmUNSPECIFIED_PIXMAP)
    {
      fprintf (stderr, "Unable to load bitmap %s\n", filename);
      abort();
    }

  success = XmeGetPixmapData (screen, cursor_bits, //
			      0,		   // image_name
			      &depth,		   // depth
			      0, 0,		   // foreground, background
			      &hot_x, &hot_y,	   // 
			      0, 0);		   // width, height

  if(!success) {
    assert (success);
  }

  // Get the cursor mask pixmap. 
  len = strlen(filename);
  char *mask_filename = new char [len + 2];
  *((char *) memcpy(mask_filename, filename, len) + len) = '\0';
  *((char *) memcpy(mask_filename + len, "m", 1) + 1) = '\0';
  cursor_mask = XmGetPixmapByDepth (screen, mask_filename, 1, 0, 1);
  if (cursor_mask == XmUNSPECIFIED_PIXMAP)
    {
      fprintf (stderr, "Unable to load bitmap %s\n", mask_filename);
      abort();
    }

  delete [] mask_filename;

  cursor = XCreatePixmapCursor (f_display, cursor_bits, cursor_mask,
				&black, &white, hot_x, hot_y);

  XmDestroyPixmap (screen, cursor_bits);
  XmDestroyPixmap (screen, cursor_mask);

  return (cursor);
}


// /////////////////////////////////////////////////////////////////
// core_dump_handler
// /////////////////////////////////////////////////////////////////

void
WindowSystem::core_dump_handler (int signal_number)
{
  if ((XtWindow ((Widget)window_system().toplevel())) != 0)
    {
      WXmMessageDialog byebye (window_system().toplevel(), (char*)"core_dump");
      XtVaSetValues((Widget)byebye, XmNmessageString,
	(XmString)XmStringLocalized(CATGETS(Set_WindowSystem, 2,
					    "Bombing...")), NULL);

      WShell shell (XtParent ((Widget) byebye));
      window_system().register_shell (&shell);
      window_system().set_cursor (window_system().f_bomb_cursor);
      XtVaSetValues((Widget)shell, XmNtitle, 
	CATGETS(Set_WindowSystem, 1, "Dtinfo: Fatal Error"), NULL);

      byebye.OkPB().Unmanage();
      byebye.CancelPB().Unmanage();
      byebye.HelpPB().Unmanage();
      byebye.Separator().Unmanage();

      byebye.Manage();
      // Wait for window manager then process events.
      // The best solution would be to wait for the dialog to be mapped,
      // but this generally works. 
      XSync (window_system().f_display, False);
      XmUpdateDisplay (byebye);
      sleep (1);
      XmUpdateDisplay (byebye);
    }
  else
    {
      fputs ("Fatal error: core dumping...\n", stderr);
    }

  signal (signal_number, SIG_DFL);
  kill (getpid(), signal_number);
}

// /////////////////////////////////////////////////////////////////
// Get default pixmap
// /////////////////////////////////////////////////////////////////

Pixmap
WindowSystem::default_pixmap (Dimension *width, Dimension *height)
{
    Pixmap temp_pixmap;
    Dimension temp_width, temp_height;

    // if printing return default print pixmap info

    if (f_printing) {
	if (f_default_print_pixmap) {	    
	    *width = f_print_defpix_width;
	    *height = f_print_defpix_height;
	    return f_default_print_pixmap;	    
	}
    }


    // if not printing return default print pixmap info

    else {
	if (f_default_pixmap) {	    
	    *width = f_defpix_width;
	    *height = f_defpix_height;
	    return f_default_pixmap;	    
	}
    }    

    // NOTE: name should be a resource 
    temp_pixmap = read_pixmap("default.xpm", width, height);
    temp_width = *width ;
    temp_height = *height ;
    
    if (temp_pixmap == 0){

#ifdef UseDlOpen
	Screen* screen = XtScreen((Widget)toplevel());
	temp_pixmap =
	    XCreatePixmapFromBitmapData(f_display, XtWindow((Widget)toplevel()),
					default_bits, default_width, default_height,
					screen->black_pixel, screen->white_pixel,
					DefaultDepthOfScreen(screen));
	
	if (! temp_pixmap)
	    throw(CASTEXCEPT Exception());
	
	temp_width = default_width;
	temp_height = default_height;
#else
	XpmAttributes xpm_attr ;
	xpm_attr.valuemask = 0 ;
	
	XmeXpmCreatePixmapFromData(f_display,
					    XtWindow((Widget)toplevel()),
					    (char**)graphic_unavailable_data,
					    &temp_pixmap,
					    NULL, &xpm_attr);
	temp_width = xpm_attr.width;
	temp_height = xpm_attr.height ;
#endif
	
	*width = temp_width ;
	*height = temp_height ;
    }

    // if printing, store values in print variables

    if (f_printing) {
	f_default_print_pixmap = temp_pixmap;
	f_print_defpix_width = temp_width;
	f_print_defpix_height = temp_height;	
    }

    // if not printing store values in display variables

    else {
	f_default_pixmap = temp_pixmap;
	f_defpix_width = temp_width;
	f_defpix_height = temp_height;
    }

    return temp_pixmap;
}

Pixmap
WindowSystem::detached_pixmap(Dimension *width, Dimension *height)
{
  if (f_detached_pixmap){
    *width = f_detached_width ;
    *height = f_detached_height ;
  } else { 
    // NOTE: name should be a resource 
    f_detached_pixmap = read_pixmap("detached.xpm", width, height);
    f_detached_width = *width ;
    f_detached_height = *height ;
    if (f_detached_pixmap == 0) {
#ifdef UseDlOpen
      Screen* screen = XtScreen((Widget)toplevel());
      f_detached_pixmap =
	XCreatePixmapFromBitmapData(f_display, XtWindow((Widget)toplevel()),
		detached_bits, detached_width, detached_height,
		screen->black_pixel, screen->white_pixel,
		DefaultDepthOfScreen(screen));

      int status = f_detached_pixmap ? 0 : 1;

      f_detached_width  = detached_width;
      f_detached_height = detached_height;
#else
      XpmAttributes xpm_attr;
      xpm_attr.valuemask = 0 ;

      int status = XmeXpmCreatePixmapFromData(f_display,
					   XtWindow((Widget)toplevel()),
					   (char**)detached_pixmap_data,
					   &f_detached_pixmap,
					   NULL, &xpm_attr);
      f_detached_width = xpm_attr.width ;
      f_detached_height = xpm_attr.height ;
#endif
      
      if (status != XpmSuccess){
	fprintf(stderr, "missing pixmap \"detached.xpm\"\n");
	throw(CASTEXCEPT Exception());
      }else{
	*width = f_detached_width ;
	*height = f_detached_height ;
      }
    }
  }
  return f_detached_pixmap ;
}


Pixmap 
WindowSystem::read_pixmap(const char *pname,
			  Dimension *width, Dimension *height)

{
  XpmAttributes  xpm_attributes;
  int            status;
  char           fname[255];

  Pixmap	pixmap ;

  xpm_attributes.valuemask = 0;

  int len = MIN(strlen(pname), 255 - 1);
  *((char *) memcpy(fname, pname, len) + len) = '\0';

#ifdef UseDlOpen
  status = xpm_lib().ReadFileToPixmap (f_display, XtWindow ((Widget)toplevel()),
				fname, &pixmap, NULL, &xpm_attributes);
#else
  status = XmeXpmReadFileToPixmap (f_display, XtWindow ((Widget)toplevel()),
				fname, &pixmap, NULL, &xpm_attributes);
#endif

  if (status != XpmSuccess) {
    pixmap = 0;
    *width = 0;
    *height = 0;

    // NOTE: exception to throw here! 

  }
  else {
    *width = xpm_attributes.width;
    *height = xpm_attributes.height;
  }

  return pixmap ;
}


bool
WindowSystem::get_boolean_default (const char *name)
{
  Boolean value;

  XtResource resource [1];
  
  resource[0].resource_name = (char *) name;
  resource[0].resource_class = (char *) name;
  resource[0].resource_type = XtRBoolean;
  resource[0].resource_size = sizeof(Boolean);
  resource[0].resource_offset = 0;
  resource[0].default_type = XtRImmediate;
  resource[0].default_addr = False;

  XtGetApplicationResources(toplevel(), &value, resource, 1, NULL, 0);

  return ((bool) value);
}

int
WindowSystem::get_int_default (const char *name)
{
  int value;

  XtResource resource [1];
  
  resource[0].resource_name = (char *) name;
  resource[0].resource_class = (char *) name;
  resource[0].resource_type = XtRInt;
  resource[0].resource_size = sizeof(int);
  resource[0].resource_offset = 0;
  resource[0].default_type = XtRImmediate ;
  resource[0].default_addr = 0;

  XtGetApplicationResources(toplevel(), &value, resource, 1, NULL, 0);

  return (value);
}

const char *
WindowSystem::get_string_default (const char *name)
{
  XtResource resource [1];
  String string;
  
  resource[0].resource_name = (char *) name;
  resource[0].resource_class = (char *) name;
  resource[0].resource_type = XtRString;
  resource[0].resource_size = sizeof(String);
  resource[0].resource_offset = 0;
  resource[0].default_type = XtRString;
  resource[0].default_addr = (void *) "";

  XtGetApplicationResources(toplevel(), &string, resource, 1, NULL, 0);

  return (string);
}

const WindowGeometry &
WindowSystem::get_geometry_default (const char *name)
{
  XtResource resource [1];
  String string;
  static WindowGeometry wg;
  
  resource[0].resource_name = (char *) name;
  resource[0].resource_class = (char *) name;
  resource[0].resource_type = XtRString;
  resource[0].resource_size = sizeof(String);
  resource[0].resource_offset = 0;
  resource[0].default_type = XtRString;
  resource[0].default_addr = (void *) "0x0";

  XtGetApplicationResources(toplevel(), &string, resource, 1, NULL, 0);

  // -1 is sentinel value for now.
  wg.ulx = wg.uly = -1;
  wg.width = wg.height = 0;
  XParseGeometry (string, &wg.ulx, &wg.uly, &wg.width, &wg.height);

  return (wg);
}


Pixel
WindowSystem::get_color_default(const char *name)
{
  Pixel value ;

  XtResource resource [1];
  
  resource[0].resource_name = (char *) name;
  resource[0].resource_class = (char *) name;
  resource[0].resource_type = XtRPixel;
  resource[0].resource_size = sizeof(Pixel);
  resource[0].resource_offset = 0;
  resource[0].default_type = XtRImmediate ;
  resource[0].default_addr = (XtPointer)
    WhitePixelOfScreen(DefaultScreenOfDisplay(f_display));

  XtGetApplicationResources(toplevel(), &value, resource, 1, NULL, 0);

  return value ;
}


char *
WindowSystem::get_message (const char *message_name)
{
  XtResource resource[1];
  String     string;
  static char default_message[256];

  resource[0].resource_name = (char *) message_name;
  resource[0].resource_class = (char*)"Message";
  resource[0].resource_type = XtRString;
  resource[0].resource_size = sizeof(String);
  resource[0].resource_offset = 0;
  resource[0].default_type = XtRString;
  resource[0].default_addr = default_message;

  XtGetApplicationResources (toplevel(), &string, resource, 1, NULL, 0);

  if (string == default_message)
    snprintf (string, sizeof(default_message),
		"%s (Message description not found)", message_name);

  return (string);
}

XmString
WindowSystem::make_space (int space_to_fill, Widget w)
{
  // make sure we can deal with bad args, as space is usually a calculated
  // value 
  if (space_to_fill <= 0)
    return WXmString("").disown();

  char space_array[256];
  int i;

  // Check for missing fonts case
  if (f_nofonts)
    {
      // get the font that the widget is using
      XmFontList fontlist;
      XtVaGetValues (w, XmNfontList, &fontlist, NULL);

      // Get width of space character in the font
      WXmString sp(" ");
      Dimension width = XmStringWidth (fontlist, sp);

      // compute closest number of spaces
      int nspaces = space_to_fill / width + 1;

      // generate space string
      for (i = 0; i < nspaces && i < 255; i++)
	space_array[i] = ' ';
      space_array[i] = '\0';

      return WXmString (space_array).disown();
    }

  int thirty_two, sixteen, eight, four, two, one ;

  thirty_two = space_to_fill / 32 ;
  space_to_fill -= 32 * thirty_two ;

  sixteen = space_to_fill / 16 ;
  space_to_fill -= 16 * sixteen ;

  eight = space_to_fill / 8 ;
  space_to_fill -= 8 * eight ;

  four = space_to_fill / 4 ;
  space_to_fill -= 4 * four ;

  two = space_to_fill / 2 ;
  space_to_fill -= 2 * two ;

  one = space_to_fill ;

  space_array[0] = 0 ;

  if (thirty_two != 0){
    for (i = 0 ; i < thirty_two ; i++)
      space_array[i] = OLIAS_SPACE32 ;
    space_array[i] = 0 ;
  } 
  WXmString string_32(space_array, (char*)OLIAS_SPACE_FONT) ;

  
  space_array[0] = 0 ;
  if (sixteen != 0){
    for (i = 0 ; i < sixteen ; i++)
      space_array[i] = OLIAS_SPACE16 ;
    space_array[i] = 0 ;
  } 
  WXmString string_16(space_array, (char*)OLIAS_SPACE_FONT) ;

  space_array[0] = 0 ;
  if (eight != 0){
    for (i = 0 ; i < eight ; i++)
      space_array[i] = OLIAS_SPACE08 ;
    space_array[i] = 0 ;
  }
  WXmString string_8(space_array, (char*)OLIAS_SPACE_FONT) ;

  space_array[0] = 0 ;
  if (four != 0){
    for (i = 0 ; i < four; i++)
      space_array[i] = OLIAS_SPACE04 ;
    space_array[i] = 0 ;
  }
  WXmString string_4(space_array, (char*)OLIAS_SPACE_FONT) ;

  space_array[0] = 0 ;
  if (two != 0){
    for (i = 0 ; i < two ; i++)
      space_array[i] = OLIAS_SPACE02 ;
    space_array[i] = 0 ;
  }
  WXmString string_2(space_array, (char*)OLIAS_SPACE_FONT) ;

  space_array[0] = 0 ;
  if (one != 0){
    for (i = 0 ; i < one ; i++)
      space_array[i] = OLIAS_SPACE01 ;
    space_array[i] = 0 ;
  }
  WXmString string_1(space_array, (char*)OLIAS_SPACE_FONT) ;

  WXmString space = string_32 + string_16 + string_8 + string_4 + string_2 + string_1 ;

  return space.disown();
}


// /////////////////////////////////////////////////////////////////
// wait_for_wm
// /////////////////////////////////////////////////////////////////

static void
wait_for_wm (Widget, XtPointer client_data, XEvent *event, Boolean *)
{
  // See if the property we're waiting for has changed.
  if (event->type == PropertyNotify &&
      event->xproperty.atom == XA_WM_STATE)
    {
      // Got our event.  waiting_for_wm = False. 
      *((bool *) client_data) = False;
    }
}

// /////////////////////////////////////////////////////////////////
// show_all_windows
// /////////////////////////////////////////////////////////////////


/*  Here's how this routine works:

    For each shell window
      if the window needs to be restored
         map the window
	 restore the initial state to normal if it was iconic
	 restore the size hints if there were any

*/

void
WindowSystem::show_all_windows()
{
  Shell_Info *si;
  Boolean waiting_for_wm;

  for (unsigned int i = 0; i < f_shell_list.length(); i++)
    {
      si = (Shell_Info *) f_shell_list[i];
      if (si->f_restore)
	{
	  // Set up an event handler so we can tell when the window
	  // manager has finished dealing with our map request.
	  if (si->f_has_wm_state)
	    {
	      waiting_for_wm = True;
	      XtAddEventHandler (si->f_shell, PropertyChangeMask, False,
				 (XtEventHandler) wait_for_wm,
				 &waiting_for_wm);
	    }
	    
	  // Get the window back on the screen. 
	  XMapWindow (f_display, XtWindow (si->f_shell));

	  // Wait for wm if one is running. 
	  if (si->f_has_wm_state)
	    {
	      XEvent event;
	      ON_DEBUG (printf ("Wait for wm to handle 0x%p", si->f_shell));
	      while (waiting_for_wm)
		{
		  XtAppNextEvent (f_application_context, &event);
		  XtDispatchEvent (&event);
		}
	      // Get rid of event handler now. 
	      XtRemoveEventHandler (si->f_shell, PropertyChangeMask, False,
				    (XtEventHandler) wait_for_wm,
				    &waiting_for_wm);
	    }

	  ON_DEBUG (printf (" - done\n"));
	  // Now that the wm is finished (if there is one), we can reset
	  // the various state variables. 
	  if (si->f_iconic)
	    {
	      // If it was iconic, reset the initial state.
	      WWMShell (si->f_shell).InitialState (NormalState);
	    }
	  else
	    {
	      // Restore the the size hints, because we set USPosition
	      // and USSize to prevent interactive placement.
	      if (si->f_has_size_hints)
		XSetWMNormalHints (f_display, XtWindow (si->f_shell),
				   si->f_size_hints);
	    }

	  // Reset the restore flag.
	  si->f_restore = FALSE;
	}
    }
}

// /////////////////////////////////////////////////////////////////
// generally necessary after a fork to avoid weird conditions
// /////////////////////////////////////////////////////////////////

void
WindowSystem::close_display_connection()
{
  close (ConnectionNumber(display()));
}


// /////////////////////////////////////////////////////////////////
// interrupt_handler - deal with user interrupt (usually ^C)
// /////////////////////////////////////////////////////////////////

void
WindowSystem::interrupt_handler (int /* signal_number */)
{
  // I'm probably forgetting why it's bad to call the message_mgr
  // from within the WindowSystem object.  We'll find out why
  // eventually, I'm sure.  20:56 04-May-94 DJB 

  Wait_Cursor bob;

  if (!(window_system().videoShell()->silent)) {
      message_mgr().quit_dialog (
	  (char*)UAS_String(CATGETS(Set_Messages, 6, "Quit Dtinfo?")));
  }
  else {
      exit(1);
  }

#if defined(SVR4) || defined(hpux) || defined(_IBMR2)
  signal (SIGINT, interrupt_handler);
#endif
}
