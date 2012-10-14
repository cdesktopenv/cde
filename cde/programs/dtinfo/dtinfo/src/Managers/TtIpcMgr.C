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
/*
 * $TOG: TtIpcMgr.C /main/16 1999/09/20 13:30:40 mgreess $
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

#define C_LibraryAgent
#define C_PrintPanelAgent
#define C_UrlAgent
#define L_Agents

#define C_Long_Lived
#define L_Basic

#define C_EnvMgr
#define C_TtIpcMgr
#define C_NodeMgr
#define C_MessageMgr
#define C_WorkspaceMgr
#define L_Managers

#define C_WindowSystem
#define L_Other

#include "Prelude.h"

#include "Managers/CatMgr.hh"
#include "Registration.hh"

#include "utility/mmdb_exception.h"

#include <string.h>
#include <stdio.h>

#include <X11/Intrinsic.h>

//  the Session include file(s) are for locale_of_desktop()
//#include <Dt/SessionP.h>
#include <Dt/SessionM.h>

#include <Dt/EnvControlP.h>
#include <Dt/Action.h>
#include <Dt/DtGetMessageP.h>
#include <Dt/MsgLog.h>

#include <Tt/tt_c.h>
#include <Tt/tttk.h>



LONG_LIVED_CC( TtIpcMgr, tt_manager )

#if 0
#  define RCS_DEBUG(statement) cerr << statement << endl
#else
#  define RCS_DEBUG(statement) 
#endif

// the following defines for tt msg opnums must match those in the tttypes
// declaration for associated (Dtinfo) ptype use
#define LOAD_INFO_LIB		1
#define SHOW_INFO_AT_LOC	2
#define QUIT_DTINFO		3
#define PRINT_INFO_AT_LOC	4

// the following defines are for message index per catalog
// Set_AddLibraryAgent:
#define ERR_INFOLIB_SPEC_FORMAT	5
// Set_TtIpcMgr:
#define ERR_TT_DISPLAY_AS_SESSION	1
#define ERR_TT_DEFAULT_AS_SESSION	2
#define ERR_TT_OPEN			3
#define ERR_TT_PTYPE_DECLARE		4

#define DFLTXT_DISPLAY_AS_SESSION "Can't get tt session for X display:\n%s"
#define DFLTXT_DEFAULT_AS_SESSION "Could not set default tooltalk session:\n%s"
#define DFLTXT_OPEN		   "Failed to open ToolTalk connection:\n%s"
#define DFLTXT_PTYPE_DECLARE	   "Could not declare ptype DtInfo:\n%s"

// the last message handled, for sanity checking
Tt_message prior_tt_msg ;

// temp internal tracking of curr workspace at time tt msg received
Atom	   curr_ws_tt ;


// /////////////////////////////////////////////////////////////////
// LogToolTalkMessage - records messages to log
// /////////////////////////////////////////////////////////////////

static char *
LogToolTalkMessage (
    DtMsgLogType	msg_type,
    int			msg_num,
    char		* dflt_txt,
    Tt_status		status)
{
    char		* errfmt;
    char		* statmsg;
    char		* errmsg;
    
    if (! tt_is_err(status)) return XtNewString("");;

    errfmt = CATGETS(Set_TtIpcMgr, msg_num, dflt_txt);
    statmsg = tt_status_message(status);
    errmsg = XtMalloc(strlen(errfmt) + strlen(statmsg) + 2);
    if (! strlen(errfmt))
      errfmt = (char*)"%s";
    sprintf(errmsg, errfmt, statmsg);

    DtMsgLogMessage ("Dtinfo", msg_type, errfmt, errmsg);
    return errmsg;
}


// /////////////////////////////////////////////////////////////////
// class constructor
// /////////////////////////////////////////////////////////////////

// TtIpcMgr constructor
//
// It is highly recommended that this be invoked only after event
// loop entry, or at least as much as possible deferred until then.
// It is the creator's responsibility to do this at the first level
// via work proc or similar.
//
TtIpcMgr::TtIpcMgr()
{
  int		mark ;
  int		ttfd ;
  char *	xsession ;
  char *	errmsg ;


  // Initialize ToolTalk

  window_system().wait_cursor();    // this tt stuff Can take a while

  mark = tt_mark() ;

  xsession = tt_X_session(
       (const char *)(XDisplayString((window_system().display()))) ) ;

  if( tt_ptr_error( xsession ) != TT_OK )
  {
      errmsg = LogToolTalkMessage (DtMsgLogError, 
				   ERR_TT_DISPLAY_AS_SESSION,
				   (char*)DFLTXT_DISPLAY_AS_SESSION,
				   tt_state);
      message_mgr().error_dialog( (char*)UAS_String(errmsg) ) ;
      XtFree (errmsg);
      tt_release( mark ) ;
      return ;
  }

  // set up to actually join the "default" session for any
  // patterns registered below to take effect there-in
  XtAppAddWorkProc( window_system().app_context(),
                    ipc_init_wp1, this ) ;

  if( (tt_state = tt_default_session_set( xsession )) == TT_OK )
  {
      procid = ttdt_open( &ttfd, "Dtinfo", "Fujitsu", "CDE", True ) ;
      
      if( (tt_state = tt_pointer_error( procid ))
          == TT_OK )
      {
          // declare Dtinfo Ptype
          if( (tt_state = tt_ptype_declare( "DtInfo" )) == TT_OK )
          {
	      // success -- add an input handler for tooltalk msgs
	      XtAppAddInput( window_system().app_context(), ttfd,
			 (XtPointer) XtInputReadMask,
			 receive_tt_msg, this ) ;

              // declare DtinfoPrint ptype (non-fatal if prior ptype ok).
              // Also can be broken out & deferred via the work proc.
              XtAppAddWorkProc( window_system().app_context(),
                                ipc_init_wp0, this ) ;
          }
 	  else
          {
              // HEY! No matter what exit you take from here, it must
              // return True! -- its a work proc!

	      errmsg = LogToolTalkMessage (DtMsgLogError, 
					   ERR_TT_PTYPE_DECLARE,
					   (char*)DFLTXT_PTYPE_DECLARE,
					   tt_state);
              message_mgr().error_dialog( (char*)UAS_String(errmsg) );
	      XtFree(errmsg);
          }
      }
      else
      {
        errmsg = LogToolTalkMessage (DtMsgLogError,
				     ERR_TT_OPEN,
				     (char*)DFLTXT_OPEN,
				     tt_state);
        message_mgr().error_dialog( (char*)UAS_String(errmsg) );
	XtFree(errmsg);
      }
  }
  else
  {
    errmsg = LogToolTalkMessage (DtMsgLogError, 
				 ERR_TT_DEFAULT_AS_SESSION,
				 (char*)DFLTXT_DEFAULT_AS_SESSION,
				 tt_state);
    message_mgr().error_dialog( (char*)UAS_String(errmsg) );
    XtFree(errmsg);
  }
  if( tt_state != TT_OK )
  {
      XtAppAddWorkProc( window_system().app_context(),
                        ipc_error_wp, &tt_state ) ;
  }
  // else -- ipc_init_wp1 will attempt to join the session

  tt_release( mark ) ;
  window_system().reset_cursor();
}


// /////////////////////////////////////////////////////////////////
// ipc_init_wp0()
// 2nd-level tt init functions, set by class constructor as XtWorkProc
// /////////////////////////////////////////////////////////////////

Boolean
TtIpcMgr::ipc_init_wp0( XtPointer /* theIpcObj */ )
{
  int		mark ;

  mark = tt_mark() ;

  Tt_status tstp = tt_ptype_declare("DtInfoPrint");
  if(tstp != TT_OK )
  {    
      DtMsgLogMessage("dtinfo", DtMsgLogError,
                    "%s: %s", "tt_ptype_declare",
                    (char *) CATGETS(
                    Set_TtIpcMgr, 9, 
   "The process type 'DtInfoPrint' is not the\nname of an installed process type."));
  }
  tt_release( mark ) ;
  return True ;		// do workproc once
}


// /////////////////////////////////////////////////////////////////
// ipc_init_wp1()
// 3rd-level tt init functions, set by class constructor as XtWorkProc
// /////////////////////////////////////////////////////////////////

Boolean
TtIpcMgr::ipc_init_wp1( XtPointer theIpcObj )
{
  if( ((TtIpcMgr*)theIpcObj)->tt_state == TT_OK )
  {
      window_system().wait_cursor();	// this tt Can take a while

      int mark = tt_mark() ;

      // actually join the "default" session for all previously
      // registered patterns to take effect there-in
      Tt_pattern *sess_patterns =
             ttdt_session_join( NULL, NULL,
                         window_system().toplevel(), theIpcObj, True ) ;
      // invoke msg handler explicitly in case a message already queued
      XtAppAddWorkProc( window_system().app_context(),
                        receive_tt_msg_wp, theIpcObj ) ;
      tt_release( mark ) ;
      window_system().reset_cursor();
  }
  return True ;		// do workproc once
}


// /////////////////////////////////////////////////////////////////
// class destructor
// /////////////////////////////////////////////////////////////////

TtIpcMgr::~TtIpcMgr()
{
  char  *dfile;

  // send a Dtinfo_Quit message to whomever may be observing
  notify_quit() ;

  // if ((dfile = tt_default_file()) != (char *)0)
  // {
  //   tt_file_quit(dfile) ;
  // }
  // ttdt_session_quit( NULL, sess_patterns, True ) ;
  // ttdt_close( NULL, NULL, True ) ;
}


// /////////////////////////////////////////////////////////////////
// establish_server - for any coordination purposes (future)
// /////////////////////////////////////////////////////////////////

void
TtIpcMgr::establish_server()
{
}


// /////////////////////////////////////////////////////////////////
// do_infolib
//
// load of infolib requested, so do it here if infolib ID valid
// /////////////////////////////////////////////////////////////////

int
TtIpcMgr::do_infolib( char *lib_path )
{
  int	sts = ID_SUCCESS ;

  if( !lib_path )
  {
      // if no file argument specified, get the default
      if( !(lib_path = env().infolibDefault()) )
      {
          message_mgr().error_dialog( (char*)UAS_String(CATGETS(
                    Set_AddLibraryAgent, ERR_INFOLIB_SPEC_FORMAT,
                    "Infolib specification format error." )) ) ;
          return (sts = BAD_ARG_FORMAT);
      }
      sts = LibraryAgent::add_library( lib_path ) ;
      XtFree( lib_path ) ;  // this path only
  }
  else
      sts = LibraryAgent::add_library( lib_path ) ;

  // possible returns ID_SUCCESS, NOT_FOUND, BAD_ARG_FORMAT, LOAD_ERROR
  return sts ;
}


// /////////////////////////////////////////////////////////////////
// do_locator
//
// view of specific doc requested, so do it here if locator ID valid
//
// Check for NULL locator pointer before passing it.
// /////////////////////////////////////////////////////////////////

int
TtIpcMgr::do_locator( char	*locator,
		      Boolean	new_window )
{
  return UrlAgent::document( locator, new_window ) ;
}

// /////////////////////////////////////////////////////////////////
// do_print
// 
// takes a section locators and prints it
//
// Prerequisites: Load any infolibs separately specified first.
// /////////////////////////////////////////////////////////////////
int
TtIpcMgr::do_print(Tt_message msg)
{
    
    int	sts = ID_SUCCESS ;
    UAS_Pointer<UAS_Common> d = NULL ;
    char *filepath = tt_message_file(msg);
    FILE *fp;
    char locator[512];
    
    fprintf(stderr, "TtIpcMgr::do_print: filepath = %s.\n", filepath);
    
    //  Open the file containing the list of sections to be printed for reading
    
    if ((fp = fopen(filepath, "r")) == NULL) {
	fprintf(stderr, "Cannot open file %s.\n", filepath);
    }

    //  Get a handle to the AppPrintData allocated in the WindowSystem class

    AppPrintData* p = window_system().GetAppPrintData();

    //  Create a list of items to be printed

    xList<UAS_Pointer<UAS_Common> > * print_list = new  xList<UAS_Pointer<UAS_Common> >;

    // each line of the temporary file contains a locator.  create a UAS_Common buffer
    // for each locator and add it to the print list
    
    while (fgets(locator, sizeof(locator), fp) != NULL) {
   
	if( !strchr( locator, ':' ) && !strchr( locator, '/' ) )
	{
	    // assume given a unique locator ID for the target.
	    // construct a fully-qualified form and pass it on.
	    
	    char *buffer =
		new char[strlen("mmdb:LOCATOR=") + strlen(locator) + 1];
	    sprintf (buffer, "mmdb:LOCATOR=%s", locator);
	    d = UAS_Common::create (buffer);
	    delete [] buffer;
	}
	else
	{
	    // assume to have a fully-qualified locator
	    d = UAS_Common::create(locator);
	}

	//  add UAS_Common pointer to list of sections to print

	if (d != (const int)NULL)
	{
	    print_list->append(d);
	}
	else {
	    fprintf(stderr, "Wow!  Null section!\n");
	}
	    
    }    // end locators in file

    p->f_outline_element = NULL;
    p->f_print_list = print_list;

    XtAppAddWorkProc(window_system().app_context(), ipc_print_wp, p) ;
    
    fclose(fp);
    fprintf(stderr, "do_print: exiting\n");    
    return 1 ;
}


// /////////////////////////////////////////////////////////////////
// handle
//
// based on message type, initiate the appropriate response
// /////////////////////////////////////////////////////////////////

int
TtIpcMgr::handle( Tt_message msg_in )
{
  int	opstatus = UNKNOWN_MSG ;
  char *info_file ;
  char *info_uuid ;
  Tt_status	tst ;

  {
    switch( tt_message_opnum(msg_in) )
    {
      case SHOW_INFO_AT_LOC :

	// - arg for locator required (any valid format).
	// - if File provided, an infolib Add is attempted first. 
	// - The infolib path may be provided via the locator part,
	//   but the infolib will not be Added if only this is the case.

	if( (opstatus = check_locale( msg_in, 2 )) == ID_SUCCESS )
	{
            // get the locator value, per spec'd position
            info_uuid = tt_message_arg_val( msg_in, 3 ) ;
            if( (tst = tt_ptr_error( info_uuid )) != TT_OK ) 
            {
              XtAppAddWorkProc( window_system().app_context(),
                                ipc_error_wp, &tst ) ;
              return (opstatus = MSG_SYNTAX_ERROR) ;
            }

            // check to see if an optional File arg is supplied for infolib

            info_file = tt_message_file( msg_in ) ;
            if( (tst = tt_ptr_error( info_file )) != TT_OK )
            {
              XtAppAddWorkProc( window_system().app_context(),
                                ipc_error_wp, &tst ) ;
              return (opstatus = MSG_SYNTAX_ERROR) ;
            }
            else
            {
              do_infolib( info_file ) ;   // load infolib per File, first
            }
                       // try to get the section
            workspace().target( curr_ws_tt ) ;
            opstatus = do_locator( info_uuid, True ) ;
	}
        break ;

      case LOAD_INFO_LIB :

	if( (opstatus = check_locale( msg_in, 2 )) == ID_SUCCESS )
	{
            info_uuid = tt_message_file( msg_in ) ;
            if( (tst = tt_ptr_error( info_uuid )) != TT_OK )
            {
              XtAppAddWorkProc( window_system().app_context(),
                                ipc_error_wp, &tst ) ;
              return (opstatus = MSG_SYNTAX_ERROR) ;
            }

            workspace().target( curr_ws_tt ) ;
            opstatus = do_infolib( info_uuid ) ;
	}
        break ;

      case PRINT_INFO_AT_LOC :	  
	  opstatus = do_print(msg_in);
        break ;

      case QUIT_DTINFO :	// ignored
        break ;

      default :
        /* don't know what else to do with this message */
        opstatus = UNKNOWN_MSG ;
        break ;
    }
  }
  return opstatus ;
}


// /////////////////////////////////////////////////////////////////
// check_locale
//
// 
// /////////////////////////////////////////////////////////////////
int
TtIpcMgr::check_locale(	Tt_message msg_in,
			int	   locale_arg_pos )
{
  int	    sts = ID_SUCCESS ;
  Tt_status tst ;
  char * target_locale ;

  // test for locale match -- can't display data for different locale.
  // NULL locale is OK.
  target_locale = tt_message_arg_val( msg_in, locale_arg_pos ) ;

  if( (tst = tt_ptr_error( target_locale )) != TT_OK ) 
  {
      XtAppAddWorkProc( window_system().app_context(),
                        ipc_error_wp, &tst ) ;
      return (sts = MSG_SYNTAX_ERROR) ;
  }
  if(    *target_locale != '\0'
      && strcmp( target_locale, env().lang() ) != 0
      && getenv( "LANG" ) != NULL
      && strcmp( target_locale, getenv( "LANG" ) ) != 0 )
  { sts = NOT_OWNER ; }

  return (sts) ;
}


char * locale_of_desktop() ;

// /////////////////////////////////////////////////////////////////
// receive_tt_msg
//
// a static member function of the TtIpcMgr class.
// used as an input handler per X
//
// gets the msg, invokes handler for class, and makes the appropriate
// tt response per handler disposition status
// /////////////////////////////////////////////////////////////////

// /*ARGSUSED*/
void
TtIpcMgr::receive_tt_msg( XtPointer client_data,
                          int *fid,
                          XtInputId *id )
{
  int   	mark;
  Tt_status	tst ;

  mark = tt_mark();

  Tt_message msg_in = tt_message_receive() ;

  if( msg_in == NULL )
  {
    // NULL msg pointer input is "normal" in certain tt cases,
    // and in typical non-tt start-up of dtinfo
    tt_release(mark);
    return ;
  }
  if( msg_in == prior_tt_msg )
  {
    // A single instance of dtinfo should never get the same message
    // handle invocation twice, unless it is a reply to the msg which
    // we sent (and none are currently expected).  This check
    // should prevent any infinite loop conditions (which are possible
    // between tt and joined processes, under certain error conditions).
    tttk_message_fail( msg_in,
		       TT_ERR_PROCID, NULL, 1 ) ;
    tt_release(mark);
    return ;
  }

  // record the current desktop workspace now
  // (this should approximate tt requestor's workspace)
  workspace().current( &curr_ws_tt ) ;

  if( (tst = tt_pointer_error( msg_in )) == TT_ERR_NOMP )
  {
    // fprintf(stderr,"ToolTalk server down.\n");
    XtAppAddWorkProc( window_system().app_context(),
                      ipc_error_wp, &tst ) ;
    return ;   // should not be fatal, but need feedback...
  }

  switch( ((TtIpcMgr*)client_data)->handle(msg_in) )
  {
    case ID_SUCCESS :

        if( ( tt_message_class( msg_in ) == TT_REQUEST )
            || ( id == NULL ) )
        {
            tt_message_reply( msg_in ) ;
        }
        break ;

    case MSG_SYNTAX_ERROR :
    case BAD_ARG_FORMAT :
    case LOAD_ERROR :
    case NOT_FOUND :

	   // *Only at This point* for "NOT_FOUND" case could we decide
	   // that "exec_host" might be relevant for a subsequent remote
	   // access attempt (were we handling that from dtinfo to kick
	   // off a fallback process...)

	// arg format, not found, or
	// load errors have already been "handled" with an error dialog,
	// so if we did a fail here, there will be redundant and less
	// specific dialog presented by the messaging system. Thus, reply.

        if( ( tt_message_class( msg_in ) == TT_REQUEST )
            || ( fid == NULL ) )
        {
            tt_message_reply( msg_in ) ;
        }
        // tttk_message_fail( msg_in,	// can't process message
	//		   TT_ERR_FILE, NULL, 1 ) ;
        break ;

    case NOT_OWNER :

	// exec host has to be detected prior to the dtinfo start to
	// have any effect. locale is also expected to be set correctly
	// prior to dtinfo invocation. However, if locale was specified
	// it is checked, and in any case for which it does not match,
	// the message must be rejected so that tt can pass it to another
	// instance or launch an instance to handle it. Since this dtinfo
	// instance may be the only handler currently available, we could
	// also launch a dtinfo instance in the requested locale before
	// rejecting the message. Else, if we are in the default
	// (desktop's) locale, ttsession may go into an infinite
	// loop creating default dtinfo instances (depending on
	// the ptype definition), trying to find a handler...
	// But... if this instance is Not running in the default locale,
	// we want tt to go ahead and start such a copy! Need to determine
	// whether this instance is running in the default/desktop locale,
	// and based on that do fail or reject... jcb
      {
        char *ref_locale = locale_of_desktop();
        if(   ref_locale
           && strcmp( ref_locale, env().lang() ) != 0
           && getenv( "LANG" ) != NULL
           && strcmp( ref_locale, getenv( "LANG" ) ) != 0 )
        {
            // don't want to process nor kill message, so reject to
            // let tooltalk start a default instance. If that instance
            // also cannot service the request precisely, it will kill
            // the message.
            tttk_message_reject( msg_in, TT_ERR_NOTHANDLER, NULL, 0 ) ;
        }
        else                   // can't process message
            tttk_message_fail( msg_in,
                               TT_ERR_PROCID, NULL, 1 ) ;

        if( ref_locale )  XFree( ref_locale );
      }
        break;

    default:
    case UNKNOWN_MSG :

        tttk_message_abandon( msg_in ) ;	// don't understand message
        break ;
  }

  // tt_mark/tt_release tags memory allocated by tt for a given
  // msg cycle, to ensure that it is all freed when done.

  tt_release(mark);

  return;
}


// receive_tt_msg_wp
//
// Background task to check explicitly for tt message input
//
Boolean
TtIpcMgr::receive_tt_msg_wp( XtPointer theIpcObj )
{
  receive_tt_msg( theIpcObj, NULL, NULL ) ;
  return True ;
}


// ipc_print_wp
//
// Display print dialog
//
Boolean
TtIpcMgr::ipc_print_wp( XtPointer client_data )
{
    AppPrintData* p = (AppPrintData *)client_data;
 
    CreatePrintSetup(p->f_pshell_parent, p);
    
    XtManageChild(p->f_print_dialog); /* popup dialog each time */
    
    return True ;
}

// ipc_error_wp
//
// Translate any tooltalk error message numbers and post for user.
// Probables: TT_ERR_SESSION TT_ERR_NOMP TT_ERR_PROCID TT_ERR_PTYPE
//
Boolean
TtIpcMgr::ipc_error_wp( XtPointer client_data )
{
  message_mgr().error_dialog(
                  tt_status_message( *((Tt_status*)client_data) ) ) ;
  return True ;
}


void
TtIpcMgr::notify_quit()
{
  Tt_message msg_out;

  // create and send a "quit" notice message for any observers

  msg_out = tt_pnotice_create( TT_SESSION, "DtInfo_Quit" ) ;

  tt_message_send( msg_out ) ;
  
  // no reply expected since this was a notice, so no longer need
  // handle for the message

  tt_message_destroy( msg_out ) ;
}


// get the Dt session manager's locale context property as a reliable
// reference point with which to compare to the dtinfo instance's
// current locale. Caller needs to free the string memory returned
// with XFree().
//
// Note: this function should really reside in the EnvMgr class
//
char *
locale_of_desktop()
{
        static Atom _DT_SM_WINDOW_INFO = None;
        static Atom WM_LOCALE_NAME = None;

        Atom type = None;
        int format;
        unsigned long nitems, after;
        // WM_LOCALE_NAME Atom (string data)
        char	*dt_locale;
	// _DT_SM_WINDOW_INFO Atom (PropDtSmWindowInfo structure data)
        Window  *sm_prop_window;

	Display *dpy = window_system().display();


        if(WM_LOCALE_NAME == None) {
            WM_LOCALE_NAME = XInternAtom(dpy, "WM_LOCALE_NAME", True);
        }
#ifdef DEBUG
        if (!WM_LOCALE_NAME)
            printf( "Atom WM_LOCALE_NAME not interned\n" );
#endif

        if (!WM_LOCALE_NAME)	return (char *)NULL;

        if(_DT_SM_WINDOW_INFO == None) {
            _DT_SM_WINDOW_INFO =
                         XInternAtom(dpy, _XA_DT_SM_WINDOW_INFO, False);
        }
#ifdef DEBUG
        if (!_DT_SM_WINDOW_INFO)
            printf( "Atom _DT_SM_WINDOW_INFO not interned\n" );
#endif

        if (!_DT_SM_WINDOW_INFO)  return (char *)NULL;

        XGetWindowProperty( dpy,
                            XDefaultRootWindow(dpy),
                            _DT_SM_WINDOW_INFO,
                            1L, 1L, False,
                            _DT_SM_WINDOW_INFO,
                            &type, &format, &nitems, &after,
                            (unsigned char **)&sm_prop_window) ;

#ifdef DEBUG
// property.smWindow = (unsigned long) smGD.topLevelWindow;
        if (!sm_prop_window)	
            printf(
             "null value from Atom _DT_SM_WINDOW_INFO for SM window ID\n" );
        else
            printf( "dtsession window is %lx\n", *sm_prop_window );
#endif

        if (!sm_prop_window)	return (char *)NULL;

        type = None;
        XGetWindowProperty( dpy,
                            *sm_prop_window,
                            WM_LOCALE_NAME,
                            0L, (long)8, False,
                            XA_STRING,
                            &type, &format, &nitems, &after,
                            (unsigned char **)&dt_locale) ;

#ifdef DEBUG
        printf( "desktop's locale is %s\n", dt_locale ) ;
#endif

        if (!dt_locale)   return (char *)NULL;
        else              return dt_locale;
}


