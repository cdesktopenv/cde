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
/* $TOG: ToolTalk.c /main/6 1999/09/16 13:41:16 mgreess $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 *   FILE:           ToolTalk.c
 *
 *   COMPONENT_NAME: Desktop File Manager (dtfile)
 *
 *   Description:    Contains routines to handle tooltalk messages.
 *
 *   FUNCTIONS: FileCallback
 *		FinalizeToolTalkSession
 *		InitializeToolTalkProcid
 *		InitializeToolTalkSession
 *		MediaCallback
 *		SessionCallback
 *		ToolTalkError
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *
 ****************************************************************************
 ************************************<+>*************************************/

#include <Xm/XmP.h>
#include <Xm/DrawingA.h>
#include <Xm/DrawingAP.h>
#include <Xm/MessageB.h>
#include <Xm/RowColumn.h>
#include <Xm/MwmUtil.h>

#include <Dt/Icon.h>
#include <Dt/IconP.h>
#include <Dt/IconFile.h>

#include <Dt/HelpDialog.h>

#ifdef SHAPE
#include <X11/extensions/shape.h>
#endif

#include <X11/Shell.h>
#include <X11/Xatom.h>
#include <Xm/Protocols.h>

#include <Dt/Session.h>
#include <Dt/DtP.h>
#include <Dt/Connect.h>
#include <Dt/FileM.h>
#include <Dt/Indicator.h>
#include <Dt/Lock.h>
#include <Dt/UserMsg.h>
#include <Dt/Wsm.h>
#include <Dt/WsmP.h>
#include <Dt/DtNlUtils.h>
#include <Dt/CommandM.h>
#include <Dt/EnvControlP.h>

#include <Tt/tttk.h>

#include "Encaps.h"
#include "SharedProcs.h"
#include "FileMgr.h"
#include "Desktop.h"
#include "Main.h"

/* ToolTalk messages */
#define FM_TT_MSG_QUIT        "Quit"
#define FM_TT_MSG_PAUSE       "Pause"
#define FM_TT_MSG_RESUME      "Resume"
#define FM_TT_MSG_GET_STAT    "Get_Status"
#define FM_TT_MSG_DO_CMD      "Do_Command"

#define VENDOR                "CDE"
#define VERSION               "1.0"

static char* RESTRICTED_HEADER = "-restricted";

static Tt_pattern * FileManagerToolTalkPattern = NULL;
static Boolean sendStopped = False;
XtInputId ProcessToolTalkInputId = 0;


Tt_message
FileCallback(
	Tt_message msg,
	Tttk_op op,
	char * file,
	void * clientdata,
	int trust,
	int self)
{
  View *view = clientdata;
  char local_host[MAXHOSTNAMELEN];
  extern char home_host_name[];

  switch (op) {
	  char *to_file;
	  DialogData *dialog_data;
	  int i;
      default:
	  break;
      case TTDT_MOVED:
	  if (view == 0) {
		  break;
	  }
	  to_file = tt_message_arg_val(msg, 1);
	  strcpy(local_host, home_host_name);
	  dialog_data = (DialogData *)view->dialog_data;
	  ShowNewDirectory( (FileMgrData *)dialog_data->data,
			    local_host, to_file);

	  /* Quit the old file, join the new */
	  ttdt_file_quit( view->pats, 0 );
	  view->pats = ttdt_file_join( to_file, TT_SESSION,
				       0, FileCallback, view );
	  if (tt_is_err( tt_ptr_error( view->pats ))) {
	      view->pats = 0;
          }
	  tt_free( to_file );
	  tttk_message_destroy( msg );
	  msg = 0;
  }
  tt_free( file );
  return msg;
}


void
FinalizeToolTalkSession( )
{
  Tt_status ttRc;
  int i;

  for (i = 0; i < view_count; i++)
  {
      if (view_set[i]->msg != 0) {
	 if (view_set[i]->pats != 0) {
	    ttdt_file_quit( view_set[i]->pats, 0 );
	    view_set[i]->pats = 0;
	 }
	 tt_message_reply( view_set[i]->msg );
	 tttk_message_destroy( view_set[i]->msg );
	 view_set[i]->msg = 0;
      }
  }

  if( FileManagerToolTalkPattern &&
      tt_ptr_error( FileManagerToolTalkPattern ) == TT_OK )
  {
    ttRc = ttdt_session_quit( NULL,
                              FileManagerToolTalkPattern,
                              1 );
    if( ProcessToolTalkInputId )
      XtRemoveInput( ProcessToolTalkInputId );
  }
  ttRc = ttdt_close( NULL, NULL, sendStopped );
}

void
ToolTalkError( char * procName, Tt_status errId )
{
  if( tt_is_err( errId ) )
  {
    switch( errId )
    {
      case TT_ERR_NOMP:
         XtRemoveInput( ProcessToolTalkInputId );
         break;
      default:
         break;
    }
  }
}

Tt_message
SessionCallback( Tt_message msg, void * client_data, Tt_message contract )
{
  View *view = client_data;
  char *opString = tt_message_op( msg );
  Tttk_op op = tttk_string_op( opString );
  tt_free( opString );

  switch (op) {
	  int i;
      default:
	  break;
      case TTDT_QUIT:
	  if (contract == 0) {
		  tt_message_reply( msg );
		  FinalizeToolTalkSession( );
		  exit( 0 );
	  }
	  /*
	   * Instead of quitting the whole process, we are to just
	   * quit the view associated with contract.
	   */
	  if (view == 0) {
		  tttk_message_fail( msg, TT_DESKTOP_EINVAL, 0, 1 );
		  return 0;
	  }
	  CloseView( (DialogData *)view->dialog_data );
	  tt_message_reply( msg );
	  tttk_message_destroy( msg );
	  return 0;
  }
  return msg;
}

Tt_message
MediaCallback( Tt_message msg,
	       void * client_data,
	       Tttk_op op,
	       Tt_status diag,
	       unsigned char * contents,
	       int len,
	       char * file,
	       char * docname )
{
	char *ceiling;
	Boolean restricted = False;

	tt_free( docname );
	if (diag != TT_OK) {
		return msg;
	}
	if (file == 0) {
		if (contents == 0) {
			tttk_message_fail( msg, TT_DESKTOP_ENODATA, 0, 1 );
			return 0;
		} else {
			/* Buffers and files are the same to dtfile */
			/* ToolTalk buffers are always null-terminated */
			tt_free( file );
			file = (char *)contents;
		}
	}
	/* Old libtt SEGVs if you ask for a context that is not there */
	if (tt_message_contexts_count( msg ) > 0) {
		ceiling = tt_message_context_val( msg, "restricted" );
		if (! tt_is_err( tt_ptr_error( ceiling ))) {
			if ((ceiling == 0) || (strcmp( ceiling, "." ) != 0)) {
				tttk_message_fail( msg,
						   TT_DESKTOP_ENOTSUP, 0, 1 );
				return 0;
			}
			restricted = True;
		}
	}
	switch ( op ) {
	    default:
		break;
	    case TTME_DISPLAY:
	    case TTME_EDIT:
		/*
		 * Create a display of the given directory tree
		 */
		ViewDirectoryProc( file, restricted, msg );
		msg = 0;
		break;
	}
	tt_free( file );
	tt_free( (caddr_t)contents );
	return msg;
}

Tt_status
InitializeToolTalkProcid( int *ttFd, Widget topLevel, Boolean sendStarted )
{
  char * procId;
  Tt_status ttstat;

#ifdef DT_PERFORMANCE
   printf("  InitializeToolTalkProcid\n");
   gettimeofday(&update_time_s, NULL);
#endif
  sendStopped = sendStarted;
  procId = ttdt_open( ttFd,
                      DTFILE_CLASS_NAME,
                      VENDOR,
                      VERSION,
                      sendStarted );

#ifdef DT_PERFORMANCE
   gettimeofday(&update_time_f, NULL);
   if (update_time_s.tv_usec > update_time_f.tv_usec) {
      update_time_f.tv_usec += 1000000;
      update_time_f.tv_sec--;
   }
   printf("    done InitializeToolTalkProcid, time: %ld.%ld\n\n", update_time_f.tv_sec - update_time_s.tv_sec, update_time_f.tv_usec - update_time_s .tv_usec);
#endif

  if( (ttstat = tt_ptr_error( procId )) == TT_OK )
  {
    ProcessToolTalkInputId =
	    XtAppAddInput( XtWidgetToApplicationContext( toplevel ),
			   *ttFd, (XtPointer)XtInputReadMask,
			   tttk_Xt_input_handler, procId );
    return( TT_OK );
  }
  else
  {
    ttdt_close( NULL, NULL, sendStopped );
    return( ttstat );
  }
}


int
InitializeToolTalkSession( Widget topLevel, int ttFd )
{
  Tt_status ttstat;

  ttstat = ttmedia_ptype_declare( "DT_File_Manager",
                                  0,
                                  MediaCallback,
                                  (void *) 0,
                                  0);
  if (ttstat == TT_OK)
    ttstat = ttmedia_ptype_declare( "DT_File_Manager",
                                    1000,
                                    MediaCallback,
                                    (void *) 0,
                                    1);
  tt_ptype_opnum_callback_add( "DT_File_Manager", 0, HandleTtRequest );
  if (ttstat == TT_OK)
  {
    /*
     * If we were started by a message, the following call to
     * tttk_Xt_input_handler will process it.  Otherwise,
     * tttk_Xt_input_handler will just return.
     */
    tttk_Xt_input_handler( NULL, 0, 0 );

    FileManagerToolTalkPattern = ttdt_session_join( tt_default_session( ),
                                                    SessionCallback,
                                                    topLevel,
                                                    NULL,
                                                    1 );
    if( tt_is_err( tt_ptr_error( FileManagerToolTalkPattern )))
    {
      ttdt_close( NULL, NULL, sendStopped );
      return( 0 );
    }
  }
  else
  {
    ttdt_close( NULL, NULL, sendStopped );
    return( 0 );
  }
  return( TT_OK );
}
