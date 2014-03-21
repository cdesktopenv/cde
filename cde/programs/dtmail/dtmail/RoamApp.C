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
 *+SNOTICE
 *
 *	$TOG: RoamApp.C /main/62 1999/09/14 16:52:18 mgreess $
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993,1994,1995 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */
/*
 *		     Common Desktop Environment
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *   (c) Copyright 1995 Digital Equipment Corp.
 *   (c) Copyright 1995 Fujitsu Limited
 *   (c) Copyright 1995 Hitachi, Ltd.
 *                                                                   
 *
 *                     RESTRICTED RIGHTS LEGEND                              
 *
 *Use, duplication, or disclosure by the U.S. Government is subject to
 *restrictions as set forth in subparagraph (c)(1)(ii) of the Rights in
 *Technical Data and Computer Software clause in DFARS 252.227-7013.  Rights
 *for non-DOD U.S. Government Departments and Agencies are as set forth in
 *FAR 52.227-19(c)(1,2).

 *Hewlett-Packard Company, 3000 Hanover Street, Palo Alto, CA 94304 U.S.A.
 *International Business Machines Corp., Route 100, Somers, NY 10589 U.S.A. 
 *Sun Microsystems, Inc., 2550 Garcia Avenue, Mountain View, CA 94043 U.S.A.
 *Novell, Inc., 190 River Road, Summit, NJ 07901 U.S.A.
 *Digital Equipment Corp., 111 Powdermill Road, Maynard, MA 01754, U.S.A.
 *Fujitsu Limited, 1015, Kamikodanaka Nakahara-Ku, Kawasaki 211, Japan
 *Hitachi, Ltd., 6, Kanda Surugadai 4-Chome, Chiyoda-ku, Tokyo 101, Japan
 */

#include <unistd.h>
#include <wchar.h>
#if defined(USL) || defined(__uxp__) || defined(linux) || defined(CSRG_BASED)
#define wcswcs wcsstr
#include <wctype.h>  // iswspace is defined in this header on USL */
#endif

#ifdef DTMAIL_TOOLTALK
#include <Tt/tt_c.h>
#include <Tt/tttk.h>
#include "SendMsgDialog.h"
#endif

#if defined(POSIX_THREADS)
#include <thread.h>
#endif

#include <signal.h>
#include <ctype.h>
#include <locale.h>
#include <sys/param.h>
#include <sys/wait.h>

#include <X11/X.h>
#include <Xm/MessageB.h>

#include <Dt/Action.h>
#include <Dt/Dt.h>
#include <Dt/DtPStrings.h>
#include <Dt/Dts.h>
#include <Dt/EnvControlP.h>

#include <DtMail/Common.h>
#include <DtMail/DtMailError.hh>
#include <DtMail/DtMailSigChld.h>
#include <DtMail/IO.hh>
#include <DtMail/OptCmd.h>

#include "DmxPrintJob.h"
#include "DtMailGenDialog.hh"
#include "DtMailHelp.hh"
#include "EUSDebug.hh"
#include "Fonts.h"
#include "MemUtils.hh"
#include "MailMsg.h"
#include "RoamApp.h"
#include "RoamMenuWindow.h"
#include "RoamCmds.h"
#include "SendMsgDialog.h"
#include "WorkingDialogManager.h"


int use_XmTextEditor = 0;



// Provide interface to the DtSvc function DtSimpleError
// When this interface is better defined, this can be removed
// and replaced with the appropriate include file

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

static int x_error_handler(Display *display, XErrorEvent* error_event)
{
    #define _DTMAIL_BUFSIZE 1024
    char error_msg[_DTMAIL_BUFSIZE];

    // log error
    _DtPrintDefaultErrorSafe(display, error_event, error_msg, _DTMAIL_BUFSIZE);
    _DtSimpleError("dtmail", DtWarning, NULL, error_msg, NULL);
    
    // if the error occured on the print display we're going to set 
    // a variable so that and when the job is done, right before calling
    // XpEndJob, we call XpCancelJob, and notify the user.

    if (theRoamApp.isActivePrintDisplay(display) &&
        error_event->error_code == BadAlloc)
    {
        theRoamApp.setErrorPrintDisplay(display);
        return 1;
    }
    
    theRoamApp._default_x_error_handler(display, error_event);
    return 1;
}


void
force( Widget w)
{
  Widget	shell;
  Display	*dpy;
  XWindowAttributes	xwa;
  Window	window;
  XtAppContext cxt=XtWidgetToApplicationContext( w );
  XEvent		event;

  shell=w;
  dpy=XtDisplay(shell);
  window=XtWindow( shell );

  while ( XGetWindowAttributes(dpy,window,&xwa)) {
    if ( XGetWindowAttributes( dpy, window, &xwa ) &&
	 xwa.map_state != IsViewable )
      break;

    XtAppNextEvent( cxt, &event );
    XtDispatchEvent( &event );
  }
  XmUpdateDisplay(shell);
}

void
forceUpdate( Widget w )
{
  Widget diashell, topshell;
  Window diawindow, topwindow;
  XtAppContext cxt = XtWidgetToApplicationContext( w );
  Display		*dpy;
  XWindowAttributes	xwa;
  XEvent		event;

  for (diashell=w;!XtIsShell(diashell);diashell=XtParent(diashell));
  for ( topshell=diashell;XtIsTopLevelShell( topshell );
	topshell = XtParent( topshell ) );

  dpy=XtDisplay(diashell);
  diawindow=XtWindow( diashell );
  topwindow=XtWindow(topshell);
  while ( XGetWindowAttributes(dpy,diawindow,&xwa)  
          && XEventsQueued( dpy, QueuedAlready) ) {
      
      XtAppNextEvent( cxt, &event );
      XtDispatchEvent( &event );
    }
  XmUpdateDisplay(topshell);
}

XtResource
RoamApp::_resources[] = {
  {
    "printscript",
    "PrintScript",
    XtRString,
    sizeof( XtRString ),
    XtOffset ( RoamApp *, _print_script ),
    XtRString,
    ( XtPointer ) "lp",
  },
  {
    "mailfiles",
    "MailFiles",
    XtRString,
    sizeof( XtRString ),
    XtOffset ( RoamApp *, _mailfiles_folder ),
    XtRString,
    ( XtPointer ) ".",
  },
  {
    "defaultmailbox",
    "DefaultMailBox",
    XtRString,
    sizeof( XtRString ),
    XtOffset ( RoamApp *, _default_mailbox ),
    XtRString,
    ( XtPointer ) ".",
  },

  // Fixed width font
  {
    "userFont",
    "UserFont",
    XtRString,
    sizeof( XtRString ),
    XtOffset ( RoamApp *, _user_font ),
    XtRString,
    ( XtPointer )"-b&h-lucidatypewriter-medium-r-*-sans-*-120-*-*-*-*-*-*",
  },

  // Fixed width fontlist
  {
    "userFont",
    "UserFont",
    XmRFontList,
    sizeof( XmFontList ),
    XtOffset ( RoamApp *, _user_fontlist ),
    XtRString,
    ( XtPointer )"-b&h-lucidatypewriter-medium-r-*-sans-*-120-*-*-*-*-*-*",
  },

  // Variable width font
  {
    "systemFont",
    "SystemFont",
    XtRString,
    sizeof( XtRString ),
    XtOffset ( RoamApp *, _system_font ),
    XtRString,
    ( XtPointer )"-b&h-lucida-medium-r-*-sans-*-120-*-*-*-*-*-*"
  },

  // Variable width fontlist
  {
    "systemFont",
    "SystemFont",
    XmRFontList,
    sizeof( XmFontList ),
    XtOffset ( RoamApp *, _system_fontlist ),
    XtRString,
    ( XtPointer )"-b&h-lucida-medium-r-*-sans-*-120-*-*-*-*-*-*"
  },

  // Font used for attachment glyph
  {
    "glyphFont",
    "GlyphFont",
    XtRString,
    sizeof( XtRString ),
    XtOffset ( RoamApp *, _glyph_font ),
    XtRString,
    ( XtPointer )
    NULL,
  },

};

RoamApp theRoamApp("Dtmail");
int just_compose = 0;

void
SigUsr1(int)
{
    theRoamApp.quitSilently();
    theRoamApp.closeAllWindows();
}

void
#if defined(sun)
panicQuitSignalHandler()
#else
panicQuitSignalHandler(int)
#endif
{
    theRoamApp.setQuitQuickly();
    theRoamApp.setQuitSilently();
    theRoamApp.closeAllWindows();
    _exit(1);
}

void
RoamApp::closeAllWindows(void)
{
    // Remove callbacks to prevent XtPhase2Destroy crashes.
    XtRemoveAllCallbacks(baseWidget(), XmNdestroyCallback);

    for (int win = 0; win < _numWindows; win++)
      if (quitQuickly())
        _windows[win]->panicQuit();
      else if (quitSilently())
        _windows[win]->quit();
      else
        _windows[win]->quit();
    
    //shutdown();
}

void
RoamApp::statusCallback(DtMailOperationId, DtMailEnv &error, void *)
{
   if (error.isSet()) {
	 // fprintf(stderr, "DEBUG: statusCallback(): Submission failed error = %d\n", error._major);
   } else {
	 // fprintf(stderr, "DEBUG: statusCallback(): Submission done.%s\n");
   }
}

#ifdef DTMAIL_TOOLTALK

int started_by_tt = 0;
// Move this to constructor of RoamMenuWindow???
int dtmail_mapped = 0;    // For explanation, look in RoamApp.h.
static int roam_tt_fd = 0;
char *roam_tt_procid = NULL;
Tt_pattern *roam_tt_pattern = NULL;

//  Report ToolTalk error
int dieFromTtError(Tt_status errid, char *procname, char *errmsg, char *helpid)
{
    /* Do not die on warnings or TT_OK */
    if ( tt_is_err(errid) )
    {
	char		*title = GETMSG(DT_catd, 2, 1, "Mailer");
	char		*errmsg = tt_status_message(errid);
        DtMailEnv	 error;

	ttdt_close(0, 0, 1);

	error.logError(
		DTM_TRUE,
		GETMSG(DT_catd, 2, 30, "%s returned ToolTalk error: %s\n"), 
		procname, tt_status_message(errid));

	DtMailGenDialog *exit_dialog = new DtMailGenDialog(
					   "ExitDialog", 
					   theApplication->baseWidget());
	exit_dialog->setToErrorDialog(title, errmsg);
	if (NULL == helpid) helpid = DTMAILHELPERROR;
	exit_dialog->post_and_return(GETMSG(DT_catd, 1, 1, "OK"), helpid);

	XtRemoveAllCallbacks(theApplication->baseWidget(), XmNdestroyCallback);
	exit(1);
    }
    return 0;
}

Tt_message attachmt_msg_handler(
    				Tt_message msg,
    				void *client_data,
    				Tttk_op op,
    				Tt_status diag,
    				unsigned char *contents,
    				int len,
    				char *file,
    				char *docname)
{
   static const char *thisFcn = "attachmt_msg_handler()";
   Tt_status status = TT_OK;
   Tt_pattern *pattern;
   SendMsgDialog *compose;

   if ( diag != TT_OK ) {
      // Toolkit detected an error
      // Let toolkit handle error
      return msg;
   }

   pattern = ttdt_message_accept(msg, NULL, (Widget)0, client_data, 1, 1);
   dieFromTtError(tt_ptr_error(pattern),
		"attachmt_msg_handler.ttdt_message_accept", NULL, NULL);

   if ( op == TTME_MAIL_EDIT ) {
       status = tt_message_reply(msg);
       dieFromTtError(status,
		"attachmt_msg_handler.tt_message_reply", NULL, NULL);

       // Put the data that is coming in (via buffer or file) as
       // the attachment of a message.
       if ( len > 0 ) {    // Pass by content
	   compose = theCompose.getWin();
	   compose->inclAsAttmt(contents, len, docname);
	   if ( docname ) {
	       compose->setTitle(docname);
	       compose->setIconTitle(docname);
	   }
       } else if ( file != NULL ) {    // Pass by filename
	   compose = theCompose.getWin();
	   compose->inclAsAttmt(file, docname);
	   if ( docname ) {
	       compose->setTitle(docname);
	       compose->setIconTitle(docname);
	   }
       } else {   // DO NOTHING.
	   // This is not an entry point to bring up a blank compose window
	   // because ttMediaLoadPatCB() returns with diagnosis =
	   // TT_DESKTOP_ENODATA. For INOUT mode, it expects file or buffer.
       }
   } else {
       status = tttk_message_fail( msg, TT_DESKTOP_ENOTSUP, 0, 1 );
       dieFromTtError(status,
		"tooltalk_msg_handler.tttk_message_fail", NULL, NULL);
   }

   //
   // Not sure if these should be freed.
   //
   //    tt_free( (caddr_t)contents );
   //    tt_free( file );
   //    tt_free( docname );

   return 0;
}

typedef struct _msg_handler_done_cb_struct
{
    Tt_message msg;
    RoamApp *roamApp;
    RoamMenuWindow *roamWin;

} MsgHandlerDoneCbData;

static void
tooltalk_msg_handler_done_cb( Widget w, XtPointer client_data, XtPointer )
{
    MsgHandlerDoneCbData	*cbData = (MsgHandlerDoneCbData *) client_data;
    Tt_status			status = TT_OK;

    status = tt_message_reply(cbData->msg);
    dieFromTtError(status,
		"tooltalk_msg_handler_done_cb.tt_message_reply", NULL, NULL);

    XtRemoveCallback(
   		w,
		XtNdestroyCallback, tooltalk_msg_handler_done_cb,
		cbData);

    cbData->roamApp->unregisterPendingTask();
    if (NULL != cbData->roamWin)
    {
	cbData->roamWin->quit(TRUE);
        delete cbData->roamWin;
    }
    XtFree((char*) cbData);
}

static char *tooltalk_save_buffer_to_file(
				unsigned char	*contents,
				int		 len)
{
    char *p = NULL;
    char *tmpdir = new char[MAXPATHLEN+1];

    // 1. Get buffer content into file.
    sprintf(tmpdir, "%s/%s", getenv("HOME"), DtPERSONAL_TMP_DIRECTORY);
    p = tempnam(tmpdir, "mail");
    if (p == NULL)
    {
	delete [] tmpdir;
	return NULL;
    }

    int fd = SafeOpen(p, O_RDWR | O_CREAT);
    if (fd < 0)
    {
	delete [] tmpdir;
	free(p);
	return NULL;
    }
    if (SafeWrite(fd, contents, len) != len)
    {
	free(p);
	p = NULL;
    }

    close(fd);
    delete [] tmpdir;
    return p;
}

static Tt_message
tooltalk_msg_handler( 
    Tt_message msg,
    void *client_data,
    Tttk_op op,
    Tt_status diag,
    unsigned char *contents,
    int len,
    char *file,
    char *docname 
)
{
    static const char *thisFcn = "RoamApp::tooltalk_msg_handler()";
    Tt_status status = TT_OK;
    SendMsgDialog *compose;
    RoamApp *roamapp = (RoamApp *) client_data;
    Tt_pattern *pattern;

    if ( diag != TT_OK ) {
	// toolkit detected an error
	// Let toolkit handle error
	return msg;
    }

    // Need to check the return value of this call.
    //
    pattern = ttdt_message_accept(msg, NULL, (Widget)0, client_data, 1, 1);
    dieFromTtError(tt_ptr_error(pattern),
		"tooltalk_msg_handler.ttdt_message_accept", NULL, NULL);

    if ( op == TTME_MAIL ) {
	// Send without GUI.
	status = tt_message_reply(msg);
	dieFromTtError(status,
			"tooltalk_msg_handler.tt_message_reply", NULL, NULL);

	// Construct message handle
	DtMailBuffer mbuf;
	if ( len > 0 ) {
	    mbuf.buffer = (void *)contents;
	    mbuf.size = (unsigned long)len;
	} else if ( file != NULL ) {
	    // 1. Get file content into buffer.
	    int fd = SafeOpen(file, O_RDONLY);
	    if (fd < 0) {
		return msg;
	    }
	    struct stat buf;
	    if (SafeFStat(fd, &buf) < 0) {
		close(fd);
		return msg;
	    }
 
	    mbuf.buffer = new char[buf.st_size];
	    if (!mbuf.buffer) {
		close(fd);
		return msg;
	    }
 
	    if (SafeRead(fd, mbuf.buffer, 
				(unsigned int) buf.st_size) != buf.st_size) {
		delete [] mbuf.buffer;
		close(fd);
		return msg;
	    }
	    mbuf.size = (unsigned long)buf.st_size;
	}
	DtMailEnv error;
	DtMail::Session *d_session = theRoamApp.session()->session();
	DtMail::Message *msgHandle = d_session->messageConstruct(
							error,
							DtMailBufferObject,
							&mbuf,
							NULL,
							NULL,
							NULL);
	if ( error.isSet() || !msgHandle ) {
	    return msg;
	}

	// Send the message
	theRoamApp.default_transport()->submit(error, msgHandle,
					       (DtMailBoolean)FALSE);

    } else if ( op == TTME_MAIL_COMPOSE ) {
	// Bring up blank compose window.
	status = tt_message_reply(msg);
	dieFromTtError(status,
		"tooltalk_msg_handler.tt_message_reply", NULL, NULL);

	compose = theCompose.getWin();
	if ( docname ) {
	    compose->setTitle(docname);
	    compose->setIconTitle(docname);
	}
    } else if ( op == TTME_MAIL_EDIT ) {
	// Bring up compose window with given data filled in.
	status = tt_message_reply(msg);
	dieFromTtError(status,
		"tooltalk_msg_handler.tt_message_reply", NULL, NULL);

	// Parse data (coming in as buffer or file)
	if ( len > 0 ) {    // Pass by content
	    compose = theCompose.getWin();
	    compose->parseNplace((char *)contents, len);
	    if ( docname ) {
	        compose->setTitle(docname);
	        compose->setIconTitle(docname);
	    }
	} else if ( file != NULL ) {    // Pass by filename
	    compose = theCompose.getWin();
	    compose->parseNplace(file);
	    if ( docname ) {
	        compose->setTitle(docname);
	        compose->setIconTitle(docname);
	    }
	} else {
            // DO NOTHING
	    // This is not an entry point to bring up a blank compose window
            // because ttMediaLoadPatCB() returns with diagnosis =
            // TT_DESKTOP_ENODATA. For INOUT mode, it expects file or buffer.
	}
    } else if ( op == TTME_DISPLAY ) {

	char *opname = tt_message_op(msg);

	// It is the Display message.  
	// Since the compose window can be started independent of
	// RoamMenuWindow, a DtMail process may be around
	// (because of a compose window).  Then if a DtMail process
	// exists (ToolTalk will not start another DtMail process),
	// that process needs to respond to this DISPLAY message. 
	// If for some reason, the view of the specified mail folder
	// is unmapped, then need to map it.  Otherwise, create the
	// view for the specified mail folder.  Need to remove self
	// destruct when using an existing DtMail process if that
	// process was started by compose.

        // This is the wrong place to reply to this message.
	// Replying here causes the action layer to think that
	// it is ok to remove the file it passed in, but we
        // are not done with it yet.  Calling tt_message_reply
	// at the end of this function doesn't work either.  We
	// are just not going to reply to this tooltalk message
        // (tooltalk folks say this is ok) for the time being.
	if (! strcmp("Display", opname)) {
	    MailSession *ses = theRoamApp.session();
	    RoamMenuWindow *roamwin = NULL;

	    if ( theApplication == NULL ) {
#ifdef WM_TT
	        fprintf(stdout, "%s: theApplication is NULL\n", thisFcn);
#endif
	        return 0;
	    }

#ifdef WM_TT
	    fprintf(stdout, "%s: TTME_DISPLAY\n", thisFcn);
#endif

	    if ( theCompose.getTimeOutId() != 0 )
	      XtRemoveTimeOut(theCompose.getTimeOutId());


	    if (len > 0)
            {
	        DtMailEnv	mail_error;
	        DtMailObjectSpace space;
	        DtMail::Session *d_session = theRoamApp.session()->session();

		char *str = (char*) malloc(len + 1);

		memcpy(str, contents, len);
		str[len] = '\0';

		if (0 == strcmp("DTMAIL_INBOX", str))
		{
	            mail_error.clear();
	            d_session->queryImpl(
					mail_error,
			         	d_session->getDefaultImpl(mail_error),
			         	DtMailCapabilityInboxName,
			         	&space,
			         	&file);
		}

		if (str) free(str);

		if (NULL == file)
		{
		    file = tooltalk_save_buffer_to_file(contents, len);
		    if (NULL == file) return msg;
		}
            }

	    // Check to see if the mbox is already open.  If it is, we will
	    // simply make sure it's displayed in the current workspace.
	    if (ses->isMboxOpen(file))
	    {
		Widget w = NULL;
		roamwin = ses->getRMW(file);
		ses->activateRMW(roamwin);
		if (NULL != roamwin) roamwin->displayInCurrentWorkspace();
	    }
	    else
	    {
	        roamwin = new RoamMenuWindow(file);
	        roamwin->initialize();
	        roamwin->manage();
	    }

	    MsgHandlerDoneCbData *cbData;
	    
	    cbData = (MsgHandlerDoneCbData*)
			XtMalloc(sizeof(MsgHandlerDoneCbData));
	    cbData->msg = msg;
	    cbData->roamApp = roamapp;
	    cbData->roamWin = NULL;
	    XtAddCallback(roamwin->GetMainWin(),
			XtNdestroyCallback,
			tooltalk_msg_handler_done_cb,
			(XtPointer) cbData);
            roamapp->registerPendingTask();

	    // Set this to True so Self_destruct()
	    // would not be started by Compose.
	    dtmail_mapped = 1;
        } else if (! strcmp("Print", opname)) {
	    char *p;

	    if ( len > 0 ) {
		p = tooltalk_save_buffer_to_file(contents, len);
		if (NULL == p) return msg;
	    } else if (file != NULL) {
	       p = strdup(file);
	    } else {
	       return msg;
	    }
		
#if 1
	    MsgHandlerDoneCbData *cbData = NULL;
            DtMailEnv		 error;
	    DmxPrintJob		*pjob = NULL;
	    RoamMenuWindow	*roamwin = NULL;
	    MailSession		*ses = theRoamApp.session();

	    cbData =
	      (MsgHandlerDoneCbData*) XtMalloc(sizeof(MsgHandlerDoneCbData));
	    if (ses->isMboxOpen(file))
	    {
		Widget w = NULL;
		roamwin = ses->getRMW(file);
		ses->activateRMW(roamwin);
		if (NULL != roamwin) roamwin->displayInCurrentWorkspace();

		cbData->roamWin = NULL;
	    }
	    else
	    {
	        roamwin = new RoamMenuWindow(file);
	        roamwin->initialize();
	        roamwin->manage();
		roamwin->list()->select_all_and_display_last(error);

	        cbData->roamWin = roamwin;
	    }

	    pjob = new DmxPrintJob(p, DTM_FALSE, (MainWindow*) roamwin);

	    cbData->msg = msg;
	    cbData->roamApp = roamapp;
	    XtAddCallback(pjob->baseWidget(),
			XtNdestroyCallback,
			tooltalk_msg_handler_done_cb,
			(XtPointer) cbData);
            roamapp->registerPendingTask();

	    pjob->execute();
#else
	    MainWindow *mw = roamapp->defaultStatusWindow();

	    if (mw != NULL) {
	      char *buf = new char[1024];
	      sprintf(buf, "Printing %s", p);
	      mw->setStatus(buf);
	      delete [] buf;
	    }

 	    status = tt_message_reply(msg);
 	    dieFromTtError(status,
		"tooltalk_msg_handler.tt_message_reply", NULL, NULL);
#endif
        }
    } else {
        status = tttk_message_fail( msg, TT_DESKTOP_ENOTSUP, 0, 1 );
 	dieFromTtError(status,
		"tooltalk_msg_handler.tttk_message_fail", NULL, NULL);
    }

    //     tt_free( (caddr_t)contents );
    //     tt_free( file );
    //     tt_free( docname );
    return 0;
}

Tt_message
quit_message_cb(Tt_message m,
	void *client_data,
	Tt_message)
{
    static const char *thisFcn = "quit_message_cb()";
    int silent, force;
    Tt_status status;

    // int mark = tt_mark();

    char *op = tt_message_op(m);
    if ( tttk_string_op(op) == TTDT_QUIT ) {

	/* silent? */
	status = tt_message_arg_ival(m, 0, &silent);
	if ( status != TT_OK ) {
	    fprintf(stderr, "DEBUG: %s: tt_message_arg_ival(0): %s\n",
		    thisFcn, tt_status_message(status));
	    return m;
	}
	if ( silent ) {
	}

	/* force? */
	status = tt_message_arg_ival(m, 1, &force);
	if ( status != TT_OK ) {
	    fprintf(stderr, "DEBUG: %s: tt_message_arg_ival(1): %s\n",
		    thisFcn, tt_status_message(status));
	    return m;
	}
	if ( force ) {
	    tt_message_reply(m);
	    free(op);
	    // tt_release(mark);
	    RoamApp *app = (RoamApp *)client_data;
	    // Need to make the following call; otherwise, will 
	    // run into Phase2Destroy
	    XtRemoveAllCallbacks(app->baseWidget(), 
				 XmNdestroyCallback);
	    app->shutdown();
	}
	// tt_release(mark);
	return m; 
    } else {
	// tt_release(mark);
	free(op);
	return m;
    }
}

static void
reload_notify_cb(XtPointer)
{
    DebugPrintf(2, "reload_notify_cb:  Reloading types database\n");
    DtDbLoad();
}
#endif   /* DTMAIL_TOOLTALK */

#ifdef DEAD_WOOD
static void
SigChldHandler(int)
{
    int status;
    int pid = (int) wait(&status);

    ChildExitNotify(pid, status);
}
#endif /* DEAD_WOOD */


void pspace_signal( int )
{
   Widget parent=theApplication->baseWidget();
   if(parent)
   {
        DtMailGenDialog *genDialog = new DtMailGenDialog("Dialog",parent);

        char *errMsg = (char *) XtCalloc(1,10240);

                // Serious error here -- No Space on Filesystem --
        sprintf(errMsg,"Insufficient paging space, \n Mailer cannot perform any operations.\n Please contact the System Administrator to \n correct the paging space problem ");
        genDialog->setToErrorDialog(
                            GETMSG(DT_catd, 1, 6, "Mailer"),
                            errMsg);
        XtFree(errMsg);

        genDialog->post_and_return(
                            GETMSG(DT_catd, 3, 9, "OK"),
                            NULL);
        delete genDialog;
    }
    else
        printf("Insufficient paging space, \n Mailer cannot perform any operations.\n Please contact the System Administrator to \n correct the paging space problem ");


}


void
Usage(char *progname)
{
   printf("Usage:  %s\n", progname);
   printf("[-h]  Help\n");
   printf("[-c]  A blank compose window comes up.\n");
   printf("[-a file1 ... fileN]  Compose window comes up with file1 through fileN as attachments.\n"); 
   printf("[-f mailfile]  The specified mail folder is displayed instead of INBOX.\n");
   printf("[-l]  Start the compose window on a dead letter file.\n");

   // There is also a "-e" option which would run dtmail with the XmTextEditor
   // instead of DtEditor.  Right now it is an undocumented feature.
}

nl_catd DT_catd = (nl_catd) -1;    // catgets file descriptor

#if defined(reallyoldsun) || defined(USL)
#define SA_HANDLER_TYPE void (*)(void)
#else
#define SA_HANDLER_TYPE void (*)(int)
#endif

void RoamApp::initialize(int *argcp, char **argv)
{
    char		**av = argv;
    struct sigaction	*action;
    struct sigaction	action_buf;
    Tt_status		status;

    _busy_count = 0;
    _firstSaveYourselfArrived = FALSE;
    _options = NULL;
    _optionsHandle = NULL;
    _quitSilently = FALSE;
    _quitQuickly = FALSE;
    _shutdownWorkprocID = 0;

    int n = 1;
    char * mail_file = NULL;
    char * dead_letter = NULL;
    char *session_file = NULL;
    int	opt;
    char *helpId;

#ifdef hpV4
    signal(SIGUSR1, (void(*)(int ...))SigUsr1);
#else
    signal(SIGUSR1, SigUsr1);
#endif

#ifdef _AIX
    (void)signal( SIGDANGER, pspace_signal );
#endif /* _AIX */

    action = &action_buf;
    memset((void*) action, 0, sizeof(struct sigaction));
    action->sa_handler = (SA_HANDLER_TYPE) panicQuitSignalHandler;
    action->sa_flags = 0;
    sigaction(SIGHUP, action, NULL);
    sigaction(SIGINT, action, NULL);
    sigaction(SIGQUIT, action, NULL);
    sigaction(SIGILL, action, NULL);
    sigaction(SIGABRT, action, NULL);
    sigaction(SIGBUS, action, NULL);
    sigaction(SIGSEGV, action, NULL);
    sigaction(SIGTERM, action, NULL);

    // Must be called before XtAppInitialize.
    XtSetLanguageProc(NULL, NULL, NULL);

    // Set up environment variable (including NLSPATH) before calling 
    // XtAppInitialize() (cmvc 6576). 
    _DtEnvControl (DT_ENV_SET);

    // This will take care of standard arguments such as -display.
    Application::initialize(argcp,argv);

    // If -session arg is present remove it and return session file name. 
    session_file = parseSessionArg(argcp, argv);

    // export display and locale settings

    static char displayenv[256];	/* Needs to be static for putenv */
    sprintf(displayenv, "DISPLAY=%s", XDisplayString(_display));
    putenv(displayenv);

    // Process dtmail opt args.

    int num_legit_args = 0;

    while((opt = getopt(*argcp, argv, "a:cef:hl:t")) != EOF) {
	switch (opt) {
	  case 'a':
	    	just_compose = 1;
		num_legit_args++;
		break;
	  case 'c':
		just_compose = 1;
		num_legit_args++;
		break;

	  case 'e':
		use_XmTextEditor = 1;
		num_legit_args++;
		break;

	  case 'f':
		mail_file = strdup(optarg);
		num_legit_args++;
		break;

	  case 'l':
		just_compose = 1;
		num_legit_args++;
		dead_letter = optarg;
		break;

	  case 't':
		// Started by ToolTalk
		started_by_tt++;
		num_legit_args++;
		n = 2;
		break;

	  case 'h':
	  default:
		Usage(argv[0]);
		// Remove cbs else face Phase2Destroy!
		XtRemoveAllCallbacks(baseWidget(), XmNdestroyCallback);
		exit (1);
	}
    }

    initDebug();

    if(!just_compose && !started_by_tt && !mail_file && session_file)
    	openSessionFile(session_file); // Open the session file

    MdbgChainCheck();
    getResources( _resources, XtNumber ( _resources ) );

    DtInitialize(XtDisplay(baseWidget()), baseWidget(), argv[0], "Dtmail");

    // Must be called after XtSetLanguageProc and DtInitialize because
    // DtInitialize sets the environment variable NLSPATH which is used
    // in catopen(). That's why we have to take out catopen from
    // Application::initialize and put a new mathod open->catalog for
    // both Application and RoamApp
    this->open_catalog();

    // Initialize the mail_error. This also has to be done after DtInitialize 
    // because the DtMailEnv consturctor calls catopen as well.
    DtMailEnv mail_error;
    mail_error.clear();

    DtDbLoad();

    //
    // Some of the scrolling lists contain formatted text and thus
    // require a fixed width font to display the text correctly.
    // We want to use the fixed width font defined by dt.userFont as
    // it will be internationalized correctly.  If it is not there
    // we fall back to a fixed width lucida font.  If that fails
    // we let the widget fallback to what ever it thinks is best.
    //

    // Default font in case we can't find anything else.
    XrmDatabase db = XtScreenDatabase(XtScreen(baseWidget()));

    if (db) {
	FontType	userfont;
        char		*buf = new char[1024];


	// Need to check fixed width font mailrc variable.  Assume
	// True for now
#if 0
	if (True) {

	    XrmPutStringResource(
		&db, "*Work_Area*XmText.fontList", _user_font);
	    XrmPutStringResource(
		&db, "*Work_Area*XmTextField.fontList", _user_font);
#endif
	    XrmPutStringResource(
		&db, "*Work_Area*DtEditor.textFontList", _user_font);
	    XrmPutStringResource(
		&db, "*Work_Area*iconGadget.fontList", _user_font);
	    XrmPutStringResource(
		&db, "*XmDialogShell*XmList.FontList", _user_font);

		// Convert the user fontlist to a font.
            if (!fontlist_to_font(_user_fontlist, &userfont)) {
                if (!(userfont.f.cf_font = XLoadQueryFont(
							XtDisplay(baseWidget()),
							"fixed")) )
            /* Couldn't convert the user fontlist to a font */
		    fprintf(
			stderr,
			"RoamApp::initialize : error loading fixed font\n");
                else
                    userfont.cf_type = XmFONT_IS_FONT;
            }

#if 0
	} else {
	    FontType	systemfont;

	    XrmPutStringResource(
		&db, "*Work_Area*Text*fontList", _system_font);
	    XrmPutStringResource(
		&db, "*Work_Area*iconGadget*fontList", _system_font);
	    XrmPutStringResource(
		&db, "*Work_Area*Text*textFontList", _system_font);
	    XrmPutStringResource(
		&db, "*XmDialogShell*XmList*FontList", _system_font);

            if (!fontlist_to_font(_system_fontlist, &systemfont)) {
                /* Couldn't convert the system fontlist to a font */
                if (!(systemfont.f.cf_font = XLoadQueryFont(
							XtDisplay(baseWidget()),
							"variable")) )
		    fprintf(
			stderr,
			"RoamApp::initialize : error loading variable font\n");
                else
                    systemfont.cf_type = XmFONT_IS_FONT;
            }
                
	}
#endif

	// If the glyph font is specified in a resource file, use it.
        if (_glyph_font)
	  _glyph_name = XtNewString(_glyph_font);
	else
	  _glyph_name = NULL;
	
	// If the glyph font hasn't been specified, try to match it with
	// the user font.
        if (!_glyph_name) {
	    // Get the font name that matches the user font pixel size.
            load_app_font(baseWidget(), &userfont, &_glyph_name);
        }

	// Create a fontlist that contains the glyph and user fonts
	strcpy(buf, _user_font);
#if 0
	// Never refer to the "plain" tag so dont add it.
	if (strchr(_user_font, '=') == NULL) {
	    // No tag.  Add one
	    strcat(buf, "=plain, ");
	}
#endif
	// If the symbol font can't be found, use user font above
	if (_glyph_name) {
	    strcat(buf, ", ");
	    strcat(buf, _glyph_name);
	    strcat(buf, "=attach");
	}

	// Loosely bind font to message list.  This lets users override
 	// with a more strongly bound name (ie "Dtmail*Message_List*FontList");
	//
	// Matches the MsgScrollingList in the RoamMenuWindows.
	XrmPutStringResource(&db, "*Message_List*FontList", buf);
	//
	// CDExc19318
	// Matches the UndelMsgScrollingList in the UndelFromListDialog
	// and overrides the "*XmDialogShell*XmList*FontList" spec above.
	XrmPutStringResource(&db, "*XmDialogShell*Message_List*FontList", buf);

	delete [] buf;
    }

    // If we don't have a mail file yet, then we need to retrieve the
    // Initialize Tooltalk
    // NOTE:  For now, must make the FIRST ttdt_open call to be the proc_id 
    //        that will respond to the start message.  Therefore, call gui's 
    //        ttdt_open before libDtMail calls its.
    char *sess = NULL;

    sess = (char *)getenv("TT_SESSION");
    if (!sess || (*sess == '\0')) {
      sess = getenv("_SUN_TT_SESSION");
    }
    if (!sess || (*sess == '\0')) {
      tt_default_session_set(tt_X_session(XDisplayString(_display)));
    }

    roam_tt_procid = ttdt_open( &roam_tt_fd, "DTMAIL", "SunSoft", "%I", 1 );
    dieFromTtError(tt_ptr_error(roam_tt_procid),
		"initialize.ttdt_open",
		GETMSG(DT_catd, 2, 2, "ToolTalk is not initialized.  Mailer cannot run without ToolTalk.\nTry starting /usr/dt/bin/dtsession, or contact your System Administrator."),
		DTMAILHELPCANTINITTOOLTALK);

    // This is for supporting old ptype where RFC_822_Message is
    // in lower case.
    status = ttmedia_ptype_declare( "RFC_822_Message",
				    0,
				    tooltalk_msg_handler,
				    this,
				    1);
    dieFromTtError(status,
		"initialize.ttmedia_ptype_declare.RFC_822_Message", NULL, NULL);

    status = ttmedia_ptype_declare( "RFC_822_MESSAGE",
				    0,
				    tooltalk_msg_handler,
				    this,
				    1);
    dieFromTtError(status,
		"initialize.ttmedia_ptype_declare.RFC_822_MESSAGE", NULL, NULL);

    status = ttmedia_ptype_declare( "MAIL_TYPE",
				    0,
				    attachmt_msg_handler,
				    this,
				    1);
    dieFromTtError(status,
		"initialize.ttmedia_ptype_declare.MAIL_TYPE", NULL, NULL);

    /* Join the default session -- This should have been done by default */
    roam_tt_pattern = ttdt_session_join(
				(const char *)0,
			        (Ttdt_contract_cb)quit_message_cb,
			        baseWidget(), this, 1);
    dieFromTtError(tt_ptr_error(roam_tt_pattern),
		"initialize.ttdt_session_join", NULL, NULL);

    _mail_session = new MailSession(mail_error, Application::appContext());
    if (mail_error.isSet()) {
	// what do we do here?  there are no windows for dialogs
	// should we just register a syslog() and exit? ugggh
	// SR
	fprintf(stderr, "RoamApp::initialize : error creating MailSession\n");
	exit (1);
    }

    //
    // .mailrc parsing error checking
    //
    DtMail::Session * d_session = _mail_session->session();
    DtMail::MailRc* mailRc = d_session->mailRc(mail_error);
    DTMailError_t pErr = mailRc->getParseError();
    if (pErr != DTME_NoError) {
	int answer = 0;
    	DtMailGenDialog *mailrc_dialog = new DtMailGenDialog(
						"MailRcDialog",
                                         	theApplication->baseWidget());
        mailrc_dialog->setToQuestionDialog(
				GETMSG(DT_catd, 2, 1, "Mailer"),
        			GETMSG(DT_catd, 2, 22,
					"There were unrecoverable syntax errors found in the ~/.mailrc file.\nCheck for more messages on terminal. Fix the errors and restart dtmail.\nIf you choose to continue you will not be able to save any changes made\nin the options dialog to file.") );
        helpId = DTMAILHELPERROR;
        answer = mailrc_dialog->post_and_return(
					GETMSG(DT_catd, 2, 23, "Continue"),
					GETMSG(DT_catd, 2, 24, "Exit"),
					helpId);
	if (answer == 2) {
	    XtRemoveAllCallbacks(
		theApplication->baseWidget(),
		XmNdestroyCallback);
	    exit(1);
	}
    }


    //
    // Check if mailer is installed properly if we are not in debugging mode.
    //
    const char *value = NULL;
    mail_error.clear();
    d_session->mailRc(mail_error)->getValue(mail_error, 
					    "__ignore_group_permissions", 
					    &value);
    if (mail_error.isSet()) {
        char grp_name[100];

	mail_error.clear();
        theApplication->enableGroupPrivileges();
        GetGroupName(grp_name);
        theApplication->disableGroupPrivileges();
        if (strcmp(grp_name, DTMAIL_DEFAULT_CREATE_MAILGROUP))
          mail_error.setError(DTME_BadRunGroup);
    

        if ((DTMailError_t)mail_error == DTME_BadRunGroup) {
            char	buf[512];
    	    int answer = 0;

    	    DtMailGenDialog *install_errDialog =
		new DtMailGenDialog("Dialog", theApplication->baseWidget());
            sprintf(buf, GETMSG(DT_catd, 2, 4,
                "Mailer has not been properly installed,\n\
and cannot run because the execution group\n\
is incorrectly set."));

            install_errDialog->setToErrorDialog(
                                GETMSG(DT_catd, 1, 6, "Mailer"),
                                buf);

            // No choice at this state other than to OK.
            helpId = DTMAILHELPBADGROUPID;
            answer = install_errDialog->post_and_return(
                                GETMSG(DT_catd, 3, 9, "OK"),
                                helpId);
	    XtRemoveAllCallbacks(
		theApplication->baseWidget(),
		XmNdestroyCallback);

            exit(1);
        }
    }
    if (NULL != value)
      free((void*) value);

    _options = new OptCmd("Mail Options...",
                          GETMSG(DT_catd, 1, 2,"Mail Options..."),
                          TRUE,
                          _w);

    if ( just_compose ) {
	// Need to install self destruct
	SendMsgDialog *compose = theCompose.getWin();

	if (dead_letter) {
            char *ttl = GETMSG(DT_catd, 1, 262, "Dead Letter Message");
	    compose->loadDeadLetter(dead_letter);
	    compose->setTitle(ttl);
	    compose->setIconTitle(ttl);
	}
	else {
	    if ( *argcp > num_legit_args + 1) {    // Have attachment(s)
		for ( int k = num_legit_args + 1;  k < *argcp;  k++ ) {
		    compose->inclAsAttmt(argv[k], NULL);
		}
	    }
	}
    }
    
    // inbox. We do this by querying the current implementation for
    // the path to its inbox.
    //
    if (!mail_file) {
	DtMailObjectSpace space;

	d_session->queryImpl(mail_error,
			     d_session->getDefaultImpl(mail_error),
			     DtMailCapabilityInboxName,
			     &space,
			     &mail_file);
    }

    // We need to determine which transport to use. The order of preference
    // is:
    //
    // 1) The value of DEFAULT_TRANSPORT in the .mailrc file.
    //
    // 2) The value of DEFAULT_TRANSPORT in the environment.
    //
    // 3) The transport, if available for the default implementation.
    //
    // 4) The first implementation that provides a transport.
    //
    const char * trans_impl = NULL;

    d_session->mailRc(mail_error)->getValue(mail_error, 
					    "DEFAULT_TRANSPORT", 
					    &trans_impl);
    if (mail_error.isSet()) {
	trans_impl = NULL;
    }
    
    
    if (!trans_impl) {
	trans_impl = getenv("DEFAULT_TRANSPORT");
    }

    if (!trans_impl) {
	trans_impl = d_session->getDefaultImpl(mail_error);

	DtMailBoolean trans;

	d_session->queryImpl(mail_error, trans_impl, 
			     DtMailCapabilityTransport, &trans);
	if (mail_error.isSet() || trans == DTM_FALSE) {
	    trans_impl = NULL;
	}
    }

    if (!trans_impl) {
	const char ** impls = d_session->enumerateImpls(mail_error);

	for (int impl = 0; impls[impl]; impl++) {
	    DtMailBoolean trans;
	    d_session->queryImpl(mail_error, impls[impl],
				 DtMailCapabilityTransport, &trans);
	    if (!mail_error.isSet() || trans == DTM_TRUE) {
		trans_impl = impls[impl];
		break;
	    }
	}
    }

    if (trans_impl) {
	_mail_transport = d_session->transportConstruct(mail_error,
							trans_impl,
							statusCallback,
							this);
    }
    else {
	_mail_transport = NULL;
    }

    // Register all callbacks the backend may have to deal with
    //
    d_session->registerLastInteractiveEventTimeCallback(lastInteractiveEventTime, this);

    // These callbacks are only needed on systems where sendmail is run as 
    // 'root', and so has access permissions to any file on the system. 
    d_session->registerDisableGroupPrivilegesCallback(disableGroupPrivileges, this);
    d_session->registerEnableGroupPrivilegesCallback(enableGroupPrivileges, this);
    
    if (d_session->pollRequired(mail_error) == DTM_TRUE) {
	d_session->registerBusyCallback(mail_error, setBusyState, this);
	_appTimeoutId = XtAppAddTimeOut(
				XtWidgetToApplicationContext( _w ),
				15000,
				&RoamApp::applicationTimeout,
				(XtPointer) this );
    }

    // Process the message that started us, if any.  Should I pass in
    // roam_tt_procid or NULL.
    tttk_Xt_input_handler( 0, 0, 0 );
    XtAppAddInput( Application::appContext(), roam_tt_fd,
		   (XtPointer)XtInputReadMask,
 		   tttk_Xt_input_handler, roam_tt_procid );

    // Get the vacation handle before the new RoamMenuWindow
    // This is for setting the Vacation title stripe on the window
    _vacation = new VacationCmd("Vacation", GETMSG(DT_catd, 1, 3, "Vacation"));

    // DtMail only supports the "Mail" message.  
    // If DtMail is started by ToolTalk, then we assume that the 
    // client wants a Compose window.  Therefore, we do not
    // bring up a RoamMenuWindow.
    //
    if ( !session_fp && !started_by_tt && !just_compose) {
	dtmail_mapped = 1;
    	_mailview = new RoamMenuWindow(mail_file);
    	_mailview->initialize();
    	_mailview->manage();
    } else {
	if(session_fp ) 
		restoreSession();
	else
		_mailview = NULL;
    }

    initSession();
 
    _dialog = new DtMailGenDialog("Dialog", _w);

    DtDbReloadNotify(reload_notify_cb, (XtPointer) NULL);
    _default_x_error_handler = XSetErrorHandler(x_error_handler);
    if (NULL != mail_file)
      free((void*) mail_file);
} 

RoamApp::RoamApp(char *name) : Application (name), _activePrintDisplays(5)
{
    DebugPrintf(2, "RoamApp::RoamApp(%p \"%s\")\n", name, name);
    theRoamApp = *this;
}

// Let the destructor of parent Application class handle the
// exit().

RoamApp::~RoamApp()
{
#ifdef DTMAIL_TOOLTALK
    if ( roam_tt_procid != NULL ) {
	//
	// Temporary workaround to get PrintToFile to work.
	// Currently, PrintToFile forks a child which is
	// calling ~RoamApp when it exits.  This clobbers
	// the tooltalk connection causing the parent to hang.
	//
	ttdt_close(0, 0, 1);
        roam_tt_procid = NULL;
    }
#endif

    catclose(DT_catd);
    if (_appTimeoutId)
      XtRemoveTimeOut(_appTimeoutId);
}

Boolean
RoamApp::shutdownWorkproc(XtPointer client_data)
{
    RoamApp *thisApp = (RoamApp*) client_data;

    if (thisApp->_numPendingTasks > 0)
      return FALSE;
    
    if (thisApp->_shutdownWorkprocID != 0)
    {
        XtRemoveWorkProc(thisApp->_shutdownWorkprocID);
	thisApp->_shutdownWorkprocID = 0;
    }

    delete thisApp->_mail_session;

    // Delete this and let the parent class's destructor call exit(0)
    delete thisApp;

    return TRUE;
}

void
RoamApp::shutdown()
{
    if (_numPendingTasks > 0)
    {
      if (_shutdownWorkprocID == 0)
        _shutdownWorkprocID = XtAppAddWorkProc(
                                        _appContext,
                                        &RoamApp::shutdownWorkproc,
                                        (XtPointer) this);
    }
    else
      shutdownWorkproc((XtPointer) this);
}

void
RoamApp::checkForShutdown()
{
    int nappwin, ncompunused, nrmwunused;

    if (! theApplication->isEnabledShutdown()) return;
    nappwin = theApplication->num_windows();
    ncompunused = theCompose.numUnusedWindows();
    nrmwunused = theRoamApp.session()->numDeactivatedRMW();

    if (nappwin == ncompunused + nrmwunused)
    {
	theCompose.~Compose();
        while (_numWindows > 0)
	{
	    delete _windows[0];
        }
    }
}

void
RoamApp::applicationTimeout( XtPointer client_data,
				   XtIntervalId *id )
{
  RoamApp *app = ( RoamApp * ) client_data;
  app->timeout( id );
}

void
RoamApp::timeout( XtIntervalId * )
{
    DtMail::Session * d_session = _mail_session->session();
    DtMailEnv error;

    d_session->poll(error);

    _appTimeoutId = XtAppAddTimeOut(
				XtWidgetToApplicationContext( _w ),
		     		15000,
		     		&RoamApp::applicationTimeout,
		     		(XtPointer) this );
}

long
RoamApp::lastInteractiveEventTime(void * client_data)
{
    RoamApp * self = (RoamApp *)client_data;
    return (theApplication->lastInteractiveEventTime());
}

void
RoamApp::disableGroupPrivileges(void * client_data)
{
    RoamApp * self = (RoamApp *)client_data;
    theApplication->disableGroupPrivileges();
    return;
}

void
RoamApp::enableGroupPrivileges(void * client_data)
{
    RoamApp * self = (RoamApp *)client_data;
    theApplication->enableGroupPrivileges();
    return;
}


void
RoamApp::setBusyState(
		DtMailEnv	&error,
		DtMailBusyState	busy_state,
		void		*client_data)
{
    static const int		MAXBUSYSTATES=20;
    static int			busyStateStackTop = 0;
    static DtMailBusyState	busyStateStack[MAXBUSYSTATES] =
						{DtMailBusyState_NotBusy};
    RoamApp			*self = (RoamApp*) client_data;

    switch (busy_state)
    {
        case DtMailBusyState_AutoSave:
        case DtMailBusyState_NewMail:
        default:
          showBusyState(error, busy_state, client_data);
//        if (busyStateStackTop)  	// Already busy from previous message?
//          self->_busy_count--;	// Decrement busy count for
					// displaying this message
          busyStateStack[++busyStateStackTop] = busy_state;
          assert(busyStateStackTop < MAXBUSYSTATES);
          break;

        case DtMailBusyState_NotBusy:
          if (busyStateStackTop)
	  {
	    if (error.isSet())
	      self->_mailview->postErrorDialog(error);
	   
	    busy_state = busyStateStack[--busyStateStackTop];
            showBusyState(error, busy_state, client_data);
//          if (busyStateStackTop)
//            self->_busy_count--;	// Decrement busy count for
					// redisplaying this message
          }
          else
	    showBusyState(error, busy_state, client_data);
          break;
    }
    assert(busyStateStack[0] == DtMailBusyState_NotBusy);
}

void
RoamApp::showBusyState(
			DtMailEnv &,
			DtMailBusyState busy_state,
			void * client_data
		      )
{
    RoamApp * self = (RoamApp *)client_data;

    switch (busy_state)
    {
    case DtMailBusyState_AutoSave:
      self->busyAllWindows(GETMSG(DT_catd, 3, 1, "Auto-saving..."));
      break;

    case DtMailBusyState_NewMail:
      self->busyAllWindows(GETMSG(DT_catd, 3, 86, "Checking for new mail..."));
      break;

    case DtMailBusyState_NotBusy:
    default:
      self->unbusyAllWindows();
      break;
    }
}

void
RoamApp::busyAllWindows(const char * msg)
{
  for (int win = 0; win < _numWindows; win++) {
//  if (_busy_count == 0) {
    _windows[win]->busyCursor();
//  }
    if (msg) {
      _windows[win]->setStatus(msg);
    }
  }
//  _busy_count++;
}

void
RoamApp::unbusyAllWindows(void)
{
//  _busy_count--;
//  if (_busy_count == 0) {
      for (int win = 0; win < _numWindows; win++) {
	_windows[win]->normalCursor();
	_windows[win]->clearStatus();
      }
//  }
}

void
RoamApp::globalAddToCachedContainerList(char *destname)
{
    const char *cname;
    RoamMenuWindow *rmw;

    for (int win = 0; win < _numWindows; win++) {
	cname = _windows[win]->className(); 
        if (strcmp(cname, "RoamMenuWindow") == 0) { 
            rmw = (RoamMenuWindow *)_windows[win];
	    rmw->addToCachedContainerList(destname);
	}
    }
}

void
RoamApp::globalPropChange(void)
{
    busyAllWindows(GETMSG(DT_catd, 1, 4, "Updating properties..."));

    for (int win = 0; win < _numWindows; win++) {
	_windows[win]->propsChanged();
    }

    unbusyAllWindows();
}

DtMailGenDialog *
RoamApp::genDialog()
{
    return(_dialog);
}


Boolean
RoamApp::startVacation(
    Widget subject_tf,
    Widget text_tp
)
{
    XmString subj;
    XmString text;
    RoamMenuWindow *rmw;
    const char *cname;
    Boolean status;

    XtVaGetValues(subject_tf,
	XmNvalue, &subj,
	NULL);

    XtVaGetValues(text_tp,
	XmNvalue, &text,
	NULL);
	
    status = _vacation->startVacation((char *)subj, (char *) text);

    for (int win = 0; win < _numWindows; win++) {
	cname = _windows[win]->className();
        if (strcmp(cname, "RoamMenuWindow") == 0) {
            rmw = (RoamMenuWindow *)_windows[win];
	    rmw->setVacationTitle();
	}
    }
    return status;
}

void
RoamApp::stopVacation()
{
    const char *cname;
    RoamMenuWindow *rmw;

    _vacation->stopVacation();

    for (int win = 0; win < _numWindows; win++) {
	cname = _windows[win]->className(); 
        if (strcmp(cname, "RoamMenuWindow") == 0) { 
            rmw = (RoamMenuWindow *)_windows[win];
	    rmw->removeVacationTitle();
	}
    }
}

char*
getPropStringValue(DtVirtArray<PropStringPair *> &results, const char *label)
{
        for (int mrc = 0; mrc < results.length(); mrc++) {
            PropStringPair * psp = results[mrc];
            if (strcmp(label, psp->label) == 0)
                return strdup(psp->value);
        }
        return (char*)NULL;
}

void
parsePropString(const char * input, DtVirtArray<PropStringPair *> & results)
{
  if (input == NULL)
	return;

    // If it's not multibyte, use the regular string function
  if (MB_CUR_MAX <= 1) {
     const char * start = input;
     const char * end;
     char *ptr;

    do {
	while (isspace(*start)) {
	    start += 1;
	}
	
	for (end = start; end && *end; end++) 
	    if (isspace(*end) && *(end-1) != '\\')
		break;

	PropStringPair * new_pair = new PropStringPair;

	int len = end - start;
	char * label = new char[len + 5];
	strncpy(label, start, len);
	label[len] = 0;
	for (ptr = label; (ptr = strstr(ptr, "\\ ")); ptr++) 
		strcpy(ptr, ptr+1); 
	
	char * file = strchr(label, ':');
	if (!file) {
	    new_pair->label = strdup(label);
	    new_pair->value = NULL;
	}
	else {
	    *file = 0;
	    new_pair->label = strdup(label);

	    file += 1;
	    if (strlen(file) == 0) {
		new_pair->value = NULL;
	    }
	    else {
		new_pair->value = strdup(file);
	    }
	}

	results.append(new_pair);
	delete [] label;

	start = end;
	
	while (*start && isspace(*start)) {
	    start += 1;
	}
	
    } while(*start && *end);
  }

  else {
    // The string can contain multibyte characters and it must be converted to 
    // wide character string before parsing
    int len = strlen(input);
    wchar_t *wc_input= new wchar_t[len+1];
    mbstowcs(wc_input, input, len+1);
    const wchar_t *start = wc_input;
    const wchar_t *end;
    wchar_t *ptr;

    do {
        while (iswspace(*start)) {             
            start += 1;             
        }

        for (end = start; end && *end; end++)
            if (iswspace(*end) && *(end-1) != (wint_t)'\\')
                break;
        PropStringPair * new_pair = new PropStringPair;

        int wclen = end - start;
        wchar_t *wc_label = new wchar_t[wclen+1];
        wcsncpy(wc_label, start, wclen);
        wc_label[wclen] = (wint_t)'\0';

	// Search the string "\ " and take out the back slash
        wchar_t esc_space[3];
        mbstowcs(esc_space, "\\ ", 3);
        for (ptr = wc_label; (ptr = wcswcs(ptr, esc_space)); ptr++)
                wcscpy(ptr, ptr+1);

        wchar_t *file = wcschr(wc_label, (wint_t)':');
        if (!file) {
            new_pair->label = new char[(wclen+1)*MB_CUR_MAX];
            wcstombs(new_pair->label, wc_label, wclen+1);
            new_pair->value = NULL;

        }

        else {
            *file = (wint_t)'\0';
            new_pair->label = new char[(wclen+1)*MB_CUR_MAX];
            wcstombs(new_pair->label, wc_label, (wclen+1)*MB_CUR_MAX);

            file += 1;
            int filelen = wcslen(file);
            if (filelen == 0) {
                new_pair->value = NULL;
            }
            else {
                new_pair->value = new char[(filelen+1)*MB_CUR_MAX];
                wcstombs(new_pair->value, file, (filelen+1)*MB_CUR_MAX);

            }
        }
        results.append(new_pair);
        delete []wc_label;
        start = end;

        while (*start && iswspace(*start)) {
            start += 1;
        }

    } while(*start && *end);

    delete []wc_input;
}
}

PropStringPair::PropStringPair(void)
{
    label = NULL;
    value = NULL;
}

PropStringPair::PropStringPair(const PropStringPair & other)
{
    label = NULL;
    value = NULL;

    if (other.label) {
	label = strdup(other.label);
    }

    if (other.value) {
	value = strdup(other.value);
    }
}

PropStringPair::~PropStringPair(void)
{
    if (label) {
	free(label);
    }

    if (value) {
	free(value);
    }
}

void
RoamApp::setSession( MailSession *session )
{
   _mail_session = session; 
}


VacationCmd*
RoamApp::vacation()
{
    return (_vacation);
}

char *formatPropPair(char * key, void * data)
{
    char *formatted_str = NULL;
    char *white_space = NULL;
    int i, num_spaces = 0;
    int key_len = strlen(key);
    int m_size;

    if (data == NULL)
	data = (void *)"";

    // figure out whitespace for formatting
    // assume 13 for normal sized alias name

    if(key_len < 13)  // need to add spaces
      {
	num_spaces = 13 - key_len;

	white_space = (char *)malloc(num_spaces + 1);
	strcpy(white_space, "\0");

	for(i = 0; i < num_spaces; i++)
	  strcat(white_space, " ");

	m_size = key_len + strlen((char *)white_space)
		 + strlen((char *)data) + strlen(" = ") + 1;
	formatted_str = (char *)malloc(m_size);

	sprintf(formatted_str, "%s%s = %s",key, white_space, data);

      }
    else
      {
		/* make an alias string */
	formatted_str = (char *)malloc(key_len + strlen((char *)data) + 2);
    
	m_size = key_len + strlen((char *)data) + strlen(" = ") + 1;
	formatted_str = (char *)malloc(m_size);

	sprintf(formatted_str, "%s = %s",key, data);

      }
    
    return formatted_str;

}

RoamMenuWindow*
RoamApp::inboxWindow()
{
    RoamMenuWindow *rmw;
    const char *cname;

    for (int win = 0; win < _numWindows; win++) {
	cname = _windows[win]->className();
	if (strcmp(cname, "RoamMenuWindow") == 0) {
	    rmw = (RoamMenuWindow *)_windows[win];
	    if (rmw->inbox()) {
		return(rmw);
	    }
	}
    }
    return(NULL);
}

RoamMenuWindow*
RoamApp::nextRoamMenuWindow(RoamMenuWindow *last)
{
    RoamMenuWindow *looking_for = last;
    RoamMenuWindow *rmw;
    const char *cname;

    for (int win = 0; win < _numWindows; win++) {
	cname = _windows[win]->className();
	if (strcmp(cname, "RoamMenuWindow") == 0) {
	    rmw = (RoamMenuWindow *)_windows[win];
	    if (NULL == looking_for)
	      return(rmw);
	    if (rmw == looking_for)
	      looking_for = NULL;
	}
    }
    return(NULL);
}

void
RoamApp::closeInactiveRoamMenuWindows(void)
{
    MailSession *ses = theRoamApp.session();
    RoamMenuWindow **rmws;
    const char *cname;
    int win=0, nrmws=0;

    rmws = (RoamMenuWindow**) malloc(_numWindows * sizeof(RoamMenuWindow *));
    for (win=0; win<_numWindows; win++)
    {
	cname = _windows[win]->className();
	if (strcmp(cname, "RoamMenuWindow") == 0)
	  rmws[nrmws++] = (RoamMenuWindow*) _windows[win];
    }

    for (win=0; win<nrmws; win++)
      if (! ses->isActiveRMW(rmws[win])) rmws[win]->quit(TRUE);

    free(rmws);
}

void
RoamApp::reopenRoamMenuWindows(void)
{
    RoamMenuWindow *rmw, **rmws;
    const char *cname;
    int win=0, nrmws=0;

    theApplication->disableShutdown();

    rmws = (RoamMenuWindow**) malloc(_numWindows * sizeof(RoamMenuWindow *));
    for (win=0; win<_numWindows; win++)
    {
	cname = _windows[win]->className();
	if (strcmp(cname, "RoamMenuWindow") == 0)
	  rmws[nrmws++] = (RoamMenuWindow*) _windows[win];
    }

    for (win=0; win<nrmws; win++)
    {
	char *name;
	rmw = rmws[win];
	name = strdup(rmw->mailboxName());
	rmw->quit(TRUE);

	rmw = new RoamMenuWindow(name);
    	rmw->initialize();
    	rmw->manage();
	free(name);
    }
    free(rmws);

    theApplication->enableShutdown();
}

MainWindow*
RoamApp::defaultStatusWindow()
{
    MainWindow *mw = NULL;

    if ((mw = (MainWindow *) _mailview) != NULL)
      return mw;
    if ((mw = (MainWindow *) inboxWindow()) != NULL)
      return mw;
    if (_numWindows > 0)
      mw = (MainWindow *) _windows[0];

    return mw;
}

void
RoamApp::open_catalog()
{
    Application::open_catalog();      // Open Motif Application message catalog file
    DT_catd = catopen(DTMAIL_CAT, NL_CAT_LOCALE); // Open DtMail message catalog file
}
