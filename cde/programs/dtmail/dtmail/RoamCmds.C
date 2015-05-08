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
 *	$TOG: RoamCmds.C /main/43 1999/07/13 08:41:44 mgreess $
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
 *	Copyright 1993, 1994, 1995 Sun Microsystems, Inc.  All rights reserved.
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

#include <errno.h>
#include <signal.h>
#include <ctype.h>
#include <string.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <pwd.h>
#include <X11/Intrinsic.h>
#include <Xm/Text.h>
#include <Xm/FileSBP.h>
#include <Xm/FileSB.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/PushBG.h>
#include <Xm/PanedW.h>
#include <Xm/Form.h>
#include <Dt/Dts.h>
#include <Dt/Action.h>
#include <Dt/Help.h>
#include <Dt/DtPStrings.h>

#include <DtMail/IO.hh>
#include <DtMail/DtMailError.hh>
#ifndef __ppc
#include <DtMail/Buffer.hh>
#endif
#ifdef __ppc
#include <DtMail/Buffer.hh>
#endif
#include <DtMail/OptCmd.h>
#include <EUSCompat.h>
#include "EUSDebug.hh"
#include "Application.h"
#include "AttachArea.h"
#include "Attachment.h"
#include "Dmx.h"
#include "DmxPrintJob.h"
#include "DtMailEditor.hh"
#include "DtMailGenDialog.hh"
#include "DtMailHelp.hh"
#include "DtMailWDM.hh"
#include "FindDialog.h"
#include "Help.hh"
#include "MailMsg.h"
#include "MemUtils.hh"
#include "MsgScrollingList.hh"
#include "MsgHndArray.hh"
#include "RoamApp.h"
#include "RoamCmds.h"
#include "RoamMenuWindow.h"
#include "SendMsgDialog.h"
#include "Undelete.hh"

#if defined(NEED_MMAP_WRAPPER)
extern "C" {
#endif
#include <sys/mman.h>
#if defined(NEED_MMAP_WRAPPER)
}
#endif

extern "C" {
extern XtPointer _XmStringUngenerate (
				XmString string,
				XmStringTag tag,
				XmTextType tag_type,
				XmTextType output_type);
}


#include <sys/file.h>


extern void forceUpdate( Widget );

RoamCmd::RoamCmd
(char *name, char *label, int active, RoamMenuWindow *window)
: NoUndoCmd (name, label, active)
{
    _menuwindow = window;
}

#ifdef DEAD_WOOD
SearchCmd::SearchCmd(
		     char *name, 
		     char *label,
		     int active, 
		     RoamMenuWindow *window
		     ) : InterruptibleCmd (name, label, active)
{
    _menuwindow = window;
    _criteria = NULL;
}

void
SearchCmd::execute ( 
		     TaskDoneCallback callback, 
		     void *clientData 
		     )
{
    InterruptibleCmd::execute( callback, clientData );
}

void
SearchCmd::execute()
{
    
    _menuwindow->list()->clearMsgs();
    _menuwindow->busyCursor();
    
    if ( !_criteria ) {
	_criteria=( char * )realloc(_criteria, strlen( this->name()) + 1);
	strcpy(_criteria, this->name());
    }
    
    InterruptibleCmd::execute();
}

void
SearchCmd::doit()
{
    int 	count;
    DtMailEnv	mail_error;
    
    // Initialize the mail_error.
    mail_error.clear();
    
    MsgScrollingList *list=_menuwindow->list();
    
    
    // load_headers will retrieve all of the message headers and
    // add the handles to the list.
    //
    count = list->load_headers(mail_error);
    
    _menuwindow->normalCursor();
    
    if (count == 0) {
	_menuwindow->message(GETMSG(DT_catd, 3, 46, "Empty container"));
	_done = TRUE;
	return;
    }
    
    list->scroll_to_bottom();
    _done=TRUE;
}      

void
SearchCmd::undoit()
{
    // Just print a message that allows us to trace the execution
    
    DebugPrintf(1, "%s: undoit\n", name());
}       

void
SearchCmd::updateMessage (char *msg)
{
    InterruptibleCmd::updateMessage(msg);
}
#endif /* DEAD_WOOD */

CheckForNewMailCmd::CheckForNewMailCmd( 
					char *name, 
					char *label,
					int active, 
					RoamMenuWindow *window
					) :  NoUndoCmd ( name, label, active )
{
    _menuwindow = window;
}

void
CheckForNewMailCmd::doit()
{
    DtMailEnv error;
    // Initialize the mail_error.
    error.clear();
    _menuwindow->checkForMail(error);
    
}

OpenInboxCmd::OpenInboxCmd(
    char *name,
    char *label,
    int active,
    RoamMenuWindow *rmw
) : Cmd (name, label, active)
{

    _menuWindow = rmw;
    
}

void
OpenInboxCmd::doit()
{
    RoamMenuWindow *rmw = theRoamApp.inboxWindow();

    if (rmw)
    {
	MailSession *ses = theRoamApp.session();
        ses->activateRMW(rmw);
        rmw->manage();
    }
    else
    {
	DtMailEnv	mail_error;
	char		*mail_file = NULL;
	DtMailObjectSpace space;
	DtMail::Session *d_session = theRoamApp.session()->session();

	mail_error.clear();
	d_session->queryImpl(mail_error,
			 d_session->getDefaultImpl(mail_error),
			 DtMailCapabilityInboxName,
			 &space,
			 &mail_file);

        // Check for error
	if (mail_file) _menuWindow->view_mail_file(mail_file, DTM_FALSE);
    }
}

void
OpenInboxCmd::undoit()
{
}

// 
// OpenContainerCmd methods implementation.
// For the most part, we treat container->open() as a benign thing.
// The magic, as we see it, deals with converting the container 
// if necessary.  
// For OpenContainerCmd, if no conversion is necessary bingo! it opens.
// If conversion is necessary, it punts the work to ConvertContainerCmd.
//
//

OpenContainerCmd::OpenContainerCmd (
				    char *name, 
				    char *label,
				    int active, 
				    RoamMenuWindow *window
				) : RoamInterruptibleCmd (name, label, active)
{
    _menuWindow = window;
}

// Parent's execute() ends up calling derived class's doit()
void
OpenContainerCmd::execute()
{
    RoamInterruptibleCmd::execute();
}

void
OpenContainerCmd::execute( 
			   RoamTaskDoneCallback rtd_callback, 
			   void *clientData
			   )
{
    RoamInterruptibleCmd::execute(rtd_callback, clientData);
}

// Tell the RMW to open.  The RMW may end up calling its convert()
// which depends on ConvertContainerCmd's doit...
// By the time RMW->open() returns, the conversion would have
// been done too.
// This is the case of a RinterruptibleCmd derived class
// getting its work done in its doit() in one call.
//

void
OpenContainerCmd::doit()
{
    DtMailEnv error;
    
    assert(_menuWindow != NULL);
    
    // Initialize the mail_error.
    error.clear();
    
    _menuWindow->open(error, _open_create_flag, _open_lock_flag);
    if (error.isSet()) {
	// Post a dialog indicating error and exit?
	return;		// for now. Should exit instead?
    }
    _done = TRUE;
}      

void
OpenContainerCmd::undoit()
{
    // Just print a message that allows us to trace the execution
    
    DebugPrintf(1, "%s: undoit\n", name());
}       

void
OpenContainerCmd::check_if_done()
{
    // Have nothing fancy to do here.  Since we do not want a dialog
    // in any case, set it to true...
    
    _done = TRUE;
    
}


void
OpenContainerCmd::post_dialog()
{
    // Empty.  We don't want a dialog on open...
}

void
OpenContainerCmd::unpost_dialog()
{
    // Empty.  We don't post, and we don't unpost.
}
void
OpenContainerCmd::updateMessage (char *msg)
{
    RoamInterruptibleCmd::updateMessage(msg);
}

void
OpenContainerCmd::set_create_lock_flags(
					DtMailBoolean create,
					DtMailBoolean lock
					)
{
    _open_create_flag = create;
    _open_lock_flag   = lock;
}


// ConvertContainerCmd
// Here be dragons!

ConvertContainerCmd::ConvertContainerCmd(
					 char *name, 
					 char *label,
					 int active, 
					 RoamMenuWindow *window
				 ) : RoamInterruptibleCmd (name, label, active)
{
    _menuWindow = window;
    _num_converted = 0;
    _num_to_be_converted = 0;
    _dialog = NULL;
}

void
ConvertContainerCmd::execute()
{
    if (!_dialog) {
	_dialog = new DtMailWDM("Convert");
    }
    
    RoamInterruptibleCmd::execute();
}

void
ConvertContainerCmd::execute ( RoamTaskDoneCallback rtd_callback, 
			       void *clientData)
{
    if (!_dialog) {
	_dialog = new DtMailWDM("Convert");
    }
    
    RoamInterruptibleCmd::execute(rtd_callback, clientData);
}

// Here be bigger dragons!
// The doit() calls the session->convert().
// And returns right away!
// the ses->convert() however ends up calling the conv_cb for every
// message that it has converted.
// So, we now have two loops working in parallel:
// 1) the parent()'s execute() class which called this doit() and is now
//    calling check_if_done() periodically via its workProc;
// 2) the session->convert() which is calling the _conv_cb() for every
//    message that it converts.  In the _conv_cb(), we do the following:
//    a) force update the dialog and see if it was interrupted;
//    b) if not interrupted, set_convert_data() where we set _done if 
//       we are really done.
//  

void
ConvertContainerCmd::doit()
{
    assert(_menuWindow != NULL);
    
    MailSession *ses = theRoamApp.session();
    DtMailEnv mail_error;
    
    // Initialize the mail_error.
    mail_error.clear();
    
//    ses->convert(mail_error, _src, _dest, _conv_cb, _menuWindow);
    
    if (mail_error.isSet()) {
	_menuWindow->postErrorDialog(mail_error);
    }
}      

void
ConvertContainerCmd::set_convert_data(
				      int converted,
				      int to_be_converted
				      )
{
    _num_converted = converted;
    _num_to_be_converted = to_be_converted;
    
    if ((_num_converted == _num_to_be_converted) && !_interrupted) {
	_done = TRUE;
    }
}

int
ConvertContainerCmd::get_num_converted()
{
    return(_num_converted);
}


void
ConvertContainerCmd::check_if_done()
{
    if (_interrupted) {
	_done = FALSE;
    }
    else if (_num_converted == _num_to_be_converted) {
	_done = TRUE;
    }
}

void
ConvertContainerCmd::updateDialog(
				  char *msg
				  )
{
    forceUpdate(_dialog->baseWidget());
    _dialog->updateDialog( msg );  
}

void
ConvertContainerCmd::updateAnimation()
{
    forceUpdate(_dialog->baseWidget());
    _dialog->updateAnimation();
}

void
ConvertContainerCmd::post_dialog()
{
    Dimension x, y, wid, ht;
    
    char * buf = new char[25];
    
    sprintf(buf, "Converted: %3d%", 0);
    
    _dialog->post ("Mailer",
		   buf,
		   _menuWindow->baseWidget(),
		   (void *) this,
		   NULL, 
		   &RoamInterruptibleCmd::interruptCallback );
    
    XtVaGetValues(_dialog->baseWidget(),
		  XmNx, &x,
		  XmNy, &y,
		  XmNwidth, &wid,
		  XmNheight, &ht,
		  NULL);
    
}

void
ConvertContainerCmd::unpost_dialog()
{
    _dialog->unpost();
}

void
ConvertContainerCmd::undoit()
{
    // Just print a message that allows us to trace the execution
    
    DebugPrintf(1, "%s: undoit\n", name());
}       

void
ConvertContainerCmd::set_data(
			      char *path1,
			      char *path2,
			      ConversionStatusCB cb
			      )
{
    _src = path1;
    _dest = path2;
    _conv_cb = cb;
}

char *
ConvertContainerCmd::get_destination_name()
{
    return _dest;
}

// Here be sheep!
// FindCmd

FindCmd::FindCmd( 
		  char *name, 
		  char *label,
		  int active,
		  RoamMenuWindow *window 
		  ) : RoamCmd ( name, label, active, window )
{
}

void
FindCmd::doit()
{
    
    _menuwindow->get_find_dialog();
    
//  SearchCmd::doit();
    
}


ChooseCmd::ChooseCmd( 
		      char *name, 
		      char *label,
		      int active, 
		      RoamMenuWindow *window
		      ) :  NoUndoCmd( name, label, active )
{
    _menuwindow = window;
}

void
ChooseCmd::doit()
{
    
}

SelectAllCmd::SelectAllCmd(
			    char *name,
			    char *label,
			    int active,
			    RoamMenuWindow *window
			    ) : Cmd ( name, label, active )
{
    _menuwindow = window;
}

void
SelectAllCmd::doit()
{
    DtMailEnv mail_error;

    _menuwindow->list()->select_all_and_display_last(mail_error);
}

void
SelectAllCmd::undoit()
{
}


DeleteCmd::DeleteCmd( 
		      char *name, 
		      char *label,
		      int active, 
		      RoamMenuWindow *window
		      ) :  Cmd ( name, label, active )
{
    _menuwindow = window;
}

void
DeleteCmd::doit()
{
    _menuwindow->list()->deleteSelected(FALSE);
    
}

void
DeleteCmd::undoit()
{
    
}

DestroyCmd::DestroyCmd(
			char *name, 
			char *label, 
			int active, 
			RoamMenuWindow *window
			) : Cmd(name, label, active)
{
    _menuwindow = window;
}

void
DestroyCmd::doit()
{
    // Call Expunge only if there are deleted messages.

    if (_menuwindow->list()->get_num_deleted_messages()) {
	_menuwindow->expunge();
    }
}

void
DestroyCmd::undoit()
{
}

// Unified Select File Cmd stuff
int   UnifiedSelectFileCmd::_is_initialized = 0;
char *UnifiedSelectFileCmd::_unified_directory = NULL;
char *UnifiedSelectFileCmd::_unified_file = NULL;
int   UnifiedSelectFileCmd::_unified_hidden = 0;
int   UnifiedSelectFileCmd::_unify_selection = 1;

UnifiedSelectFileCmd::UnifiedSelectFileCmd ( 
			       		char   *name, 
			       		char   *label,
			       		char   *title,
			       		char   *ok_label,
			       		int    active, 
			       		FileCallback
					       select_callback,
			       		void   *client_data,
			       		Widget parent)
:SelectFileCmd (name,
		label,
		title,
		ok_label,
		active,
		unifiedFileSelectedCB,
		this,
		unifiedFileCanceledCB,
		this,
		parent)
{
   if (! _is_initialized)
   {
       FORCE_SEGV_DECL(DtMail::Session, m_session);
       DtMailEnv	error;
       const char	*dirname = NULL;
       const char	*expanded_dirname = NULL;
       const char	*value = NULL;
       char		*full_dirname = NULL;

       _unified_directory = NULL;
       _unified_file = NULL;
       _unified_hidden = 0;
       _unify_selection = 1;
       _is_initialized = 1;

       m_session = theRoamApp.session()->session();

       error.clear();
       m_session->mailRc(error)->getValue(error, "filefolder", &dirname);
       if (error.isSet()) {
	   dirname = strdup("~");
	   error.clear();
       }
       expanded_dirname = m_session->expandPath(error, dirname);
       _unified_directory = XtNewString(expanded_dirname);

       error.clear();
       m_session->mailRc(error)->getValue(
					error,
					"dontunifyfileselection",
					&value);
       if (! error.isSet())
	 _unify_selection = 0;

       free((void*) dirname);
       free((void*) expanded_dirname);
       free((void*) value);
   }

   _select_file_callback = select_callback;
   _select_file_client_data = client_data;
   _genDialog = new DtMailGenDialog("Dialog", parent);
}

UnifiedSelectFileCmd::~UnifiedSelectFileCmd()
{
    if (_genDialog) delete _genDialog;
}

void
UnifiedSelectFileCmd::doit()
{
    if (NULL == _fileBrowser)
    {
        SelectFileCmd::doit();
        if (NULL != _unified_directory)
          setDirectory(_unified_directory);
        if (NULL != _unified_file)
          setSelected(_unified_file);
        setHidden(_unified_hidden);
    }
    else
    {
        if (_unify_selection)
        {
            if (NULL != _unified_directory)
              setDirectory(_unified_directory);
            if (NULL != _unified_file)
              setSelected(_unified_file);
            setHidden(_unified_hidden);
        }
        SelectFileCmd::doit();
    }
}

void
UnifiedSelectFileCmd::unifiedFileSelectedCB(void *client_data, char *selection)
{
    UnifiedSelectFileCmd *self = (UnifiedSelectFileCmd *) client_data;

    if (NULL != self)
    {
        self->updateUnifiedData();
        self->unifiedFileSelected(selection);
    }
}

void
UnifiedSelectFileCmd::unifiedFileSelected(char *selection)
{
    DtMailEnv	error;

    SafePathIsAccessible(error, selection);
    if (error.isSet())
    {
	const char *errmsg = NULL;
	char *err;
	int answer;

        errmsg = (const char*) error;
        err = strdup(errmsg);

	_genDialog->setToErrorDialog(GETMSG(DT_catd, 3, 48, "Mailer"), err);
	answer = _genDialog->post_and_return(DTMAILHELPERROR);
	if (1 == answer) doit();
	if (err) free(err);
	return;
    }

    if (_select_file_callback)
      _select_file_callback(_select_file_client_data, selection);
}

void
UnifiedSelectFileCmd::unifiedFileCanceledCB(void *client_data, char *)
{
    UnifiedSelectFileCmd *self = (UnifiedSelectFileCmd *) client_data;

    if (NULL != self)
      self->updateUnifiedData();
}

void
UnifiedSelectFileCmd::updateUnifiedData()
{
    if (! _unify_selection)
      return;

    if (NULL != _unified_file)
      XtFree(_unified_file);
    _unified_file = getSelected();


    if (NULL != _unified_directory)
      XtFree(_unified_directory);
    _unified_directory = getDirectory();

    _unified_hidden = getHidden();
}

// Unified Select Mailbox Cmd stuff
int   UnifiedSelectMailboxCmd::_is_initialized = 0;
char *UnifiedSelectMailboxCmd::_unified_directory = NULL;
char *UnifiedSelectMailboxCmd::_unified_file = NULL;
int   UnifiedSelectMailboxCmd::_unified_hidden = 0;
int   UnifiedSelectMailboxCmd::_unify_selection = 1;

UnifiedSelectMailboxCmd::UnifiedSelectMailboxCmd ( 
			       		char   *name, 
			       		char   *label,
			       		char   *title,
			       		char   *ok_label,
			       		int    active, 
			       		FileCallback
					       select_callback,
			       		void   *client_data,
			       		Widget parent,
					DtMailBoolean only_show_mailboxes)
:SelectFileCmd (name,
		label,
		title,
		ok_label,
		active,
		unifiedMailboxSelectedCB,
		this,
		unifiedMailboxCanceledCB,
		this,
		parent)
{
   if (! _is_initialized)
   {
       FORCE_SEGV_DECL(DtMail::Session, m_session);
       DtMailEnv	 error;
       const char	*dirname = NULL;
       const char	*expanded_dirname = NULL;
       const char	*value = NULL;
       char		*full_dirname = NULL;

       _unified_directory = NULL;
       _unified_file = NULL;
       _unified_hidden = 0;
       _unify_selection = 1;
       _is_initialized = 1;
       _only_show_mailboxes = only_show_mailboxes;

       m_session = theRoamApp.session()->session();

       error.clear();
       m_session->mailRc(error)->getValue(error, "folder", &dirname);
       if (error.isSet()) {
	   dirname = strdup("~");
	   error.clear();
       }
       expanded_dirname = m_session->expandPath(error, dirname);
       _unified_directory = XtNewString(expanded_dirname);

       error.clear();
       m_session->mailRc(error)->getValue(
					error,
					"dontunifyfileselection",
					&value);
       if (! error.isSet())
	 _unify_selection = 0;

       free((void*) dirname);
       free((void*) expanded_dirname);
       free((void*) value);
   }

   _select_file_callback = select_callback;
   _select_file_client_data = client_data;
   _genDialog = new DtMailGenDialog("Dialog", parent);
}

UnifiedSelectMailboxCmd::~UnifiedSelectMailboxCmd()
{
    if (_genDialog) delete _genDialog;
}

void
UnifiedSelectMailboxCmd::doit()
{
    if (NULL == _fileBrowser)
    {
        SelectFileCmd::doit();
        if (NULL != _unified_directory)
          setDirectory(_unified_directory);
        if (NULL != _unified_file)
          setSelected(_unified_file);
        setHidden(_unified_hidden);

	if (_fileBrowser)
	  XtVaSetValues(
		_fileBrowser,
		XmNfileSearchProc,
		UnifiedSelectMailboxCmd::unifiedMailboxSearchProc,
		NULL);
    }
    else
    {
        if (_unify_selection)
        {
            if (NULL != _unified_directory)
              setDirectory(_unified_directory);
            if (NULL != _unified_file)
              setSelected(_unified_file);
            setHidden(_unified_hidden);
        }
        SelectFileCmd::doit();
    }
}

extern "C" {
extern void	_XmOSBuildFileList(
			String dirPath,
			String pattern,
#if NeedWidePrototypes
                        unsigned int typeMask,
#else
                        unsigned char typeMask,
#endif /* NeedWidePrototypes */
                        String **pEntries,
                        unsigned int *pNumEntries,
                        unsigned int *pNumAlloc);

extern int	_XmOSFileCompare(const void *sp1, const void *sp2);
extern char	*_XmStringGetTextConcat(XmString string);
}

void
UnifiedSelectMailboxCmd::unifiedMailboxSearchProc(
					Widget w,
					XtPointer sd)
{   
    XmFileSelectionBoxWidget fs =
				(XmFileSelectionBoxWidget) w;
    XmFileSelectionBoxCallbackStruct * searchData =
				(XmFileSelectionBoxCallbackStruct *) sd;
    String          dir;
    String          pattern;
    Arg             args[3];
    int             Index;
    String *        fileList;
    unsigned int    numFiles;
    unsigned int    numItems = 0;
    unsigned int    numAlloc;
    XmString *      XmStringFileList;
    unsigned        dirLen;
    XtEnum	    fileFilterStyle, pathMode;
    unsigned char   fileTypeMask;

    if (!(dir = _XmStringGetTextConcat(searchData->dir)))
      return ;
    
    if (!(pattern = _XmStringGetTextConcat(searchData->pattern)))
    {
	XtFree(dir);
        return;
    } 
    fileList = NULL;

    XtVaGetValues(
		w,
		XmNfileTypeMask, &fileTypeMask,
		XmNfileFilterStyle, &fileFilterStyle,
		XmNpathMode, &pathMode,
		NULL);

    _XmOSBuildFileList(
		dir, pattern, fileTypeMask,
                &fileList,  &numFiles, &numAlloc);

    if (fileList && numFiles)
    {
	Boolean showDotFiles = (fileFilterStyle == XmFILTER_NONE);

	if (numFiles > 1)
	  qsort((void*) fileList, numFiles, sizeof(char*), _XmOSFileCompare);
	
        XmStringFileList = (XmString*) XtMalloc(numFiles*sizeof(XmString));
        
        Index = 0;
	dirLen = strlen(dir);

	while (Index < numFiles)
	{
	    Boolean	isMailBox = 0;
	    char	*dataType = NULL;

	    dataType = DtDtsFileToDataType(fileList[Index]);
	    if (dataType)
	      isMailBox = (0 == strcmp(dataType, "DTMAIL_FILE"));
	    DtDtsFreeDataType(dataType);

	    if (isMailBox &&
		(showDotFiles || ((fileList[Index])[dirLen] != '.')) )
	    {   
                if (pathMode ==  XmPATH_MODE_FULL)
		  XmStringFileList[numItems++] = 
			XmStringGenerate(fileList[Index],
					 XmFONTLIST_DEFAULT_TAG,
					 XmCHARSET_TEXT, NULL);
		else 
		  XmStringFileList[numItems++] = 
			XmStringGenerate(&(fileList[Index])[dirLen],
					 XmFONTLIST_DEFAULT_TAG,
					 XmCHARSET_TEXT, NULL) ;
	    } 
	    ++Index ;
	} 

	/* Update the list.
        */
        Index = 0 ;
	XtVaSetValues(
		w,
		XmNfileListItemCount, numItems,
		XmNfileListItems, XmStringFileList,
		XmNlistUpdated, TRUE,
		NULL);

        Index = numFiles;
        while(Index--)
          XtFree( fileList[Index]);

        while(numItems--)
          XmStringFree(XmStringFileList[numItems]);

        XtFree((char*) XmStringFileList);
    }
    else
    {
	XtVaSetValues(
		w,
		XmNfileListItemCount, 0,
		XmNfileListItems, NULL,
		XmNlistUpdated, TRUE,
		NULL);
    } 

    XtFree((char *) fileList);
    XtFree(pattern);
    XtFree(dir);
    return;
}

void
UnifiedSelectMailboxCmd::unifiedMailboxSelectedCB(
					void *client_data,
					char *selection)
{
    UnifiedSelectMailboxCmd *self = (UnifiedSelectMailboxCmd *) client_data;

    if (NULL != self)
    {
        self->updateUnifiedData();
        self->unifiedMailboxSelected(
				self->_select_file_callback,
				self->_select_file_client_data,
				selection);
    }
}

void
UnifiedSelectMailboxCmd::unifiedMailboxSelected(
						FileCallback	cb,
						void		*client_data,
						char		*selection)
{
    DtMailEnv	error;

    SafePathIsAccessible(error, selection);
    if (error.isSet())
    {
	const char *errmsg = NULL;
	char *err = NULL;
	int answer;

        errmsg = (const char*) error;
        err = strdup(errmsg);

	_genDialog->setToErrorDialog(GETMSG(DT_catd, 3, 48, "Mailer"), err);
	answer = _genDialog->post_and_return(DTMAILHELPERROR);
	if (1 == answer) doit();
	if (err) free(err);
	return;
    }

    updateUnifiedData();
    if (cb) cb(client_data, selection);
}

void
UnifiedSelectMailboxCmd::unifiedMailboxCanceledCB(void *client_data, char *)
{
    UnifiedSelectMailboxCmd *self = (UnifiedSelectMailboxCmd *) client_data;

    if (NULL != self)
      self->updateUnifiedData();
}

void
UnifiedSelectMailboxCmd::updateUnifiedData()
{
    if (! _unify_selection)
      return;

    if (NULL != _unified_file)
      XtFree(_unified_file);
    _unified_file = getSelected();


    if (NULL != _unified_directory)
      XtFree(_unified_directory);
    _unified_directory = getDirectory();

    _unified_hidden = getHidden();
}

ContainerMenuCmd::ContainerMenuCmd(
    char *name,
    char *label,
    int active,
    RoamMenuWindow *window,
    ContainerOp op
) : RoamCmd ( name, label, active, window )
{
    _menuwindow = window;
    _container_name = name;
    _operation = op;
}

void
ContainerMenuCmd::doit()
{
    DtMailEnv mail_error;

    // Initialize mail_error.
    mail_error.clear();

    theRoamApp.busyAllWindows(GETMSG(DT_catd, 3, 15, "Saving..."));
    _menuwindow->mailbox()->save();
    theRoamApp.unbusyAllWindows();

    switch (_operation)
    {
        case DTM_NONE:
	    break;
        case DTM_OPEN:
    	    _menuwindow->view_mail_file(_container_name, DTM_FALSE);
	    break;
        case DTM_COPY:
	    _menuwindow->list()->copySelected(
					mail_error,
					_container_name,
					FALSE, FALSE);
	    if (mail_error.isSet())
	    {
	        // We had an error in copying the message to a container!
	    }
	    break;
	case DTM_MOVE:
	    _menuwindow->list()->copySelected(
					mail_error,
					_container_name,
					TRUE, FALSE);
	    if (mail_error.isSet())
	    {
	        // We had an error in moving the message to a container!
	    }
	    break;
    }
}

ContainerMenuCmd::~ContainerMenuCmd()
{
}

    
// Move the messages that are selected in the RoamMenuWindow to the Inbox.
MoveToInboxCmd::MoveToInboxCmd(
			       char *name,
			       char *label,
			       int active,
			       RoamMenuWindow *window
			       ) : RoamCmd (name, label, active, window)
{
    _menuwindow = window;
}

void
MoveToInboxCmd::doit()
{
    DtMailEnv mail_error;
    
    // Initialize mail_error.
    mail_error.clear();
    
    // Get a handle to the Inbox.
    char * mail_file = NULL;
    DtMailObjectSpace space;
    DtMail::Session * d_session = theRoamApp.session()->session();
    
    d_session->queryImpl(mail_error,
			 d_session->getDefaultImpl(mail_error),
			 DtMailCapabilityInboxName,
			 &space,
			 &mail_file);
    _menuwindow->list()->copySelected(mail_error, mail_file, TRUE, FALSE);
    if (mail_error.isSet()) {
	// We had an error in moving the messages to the Inbox!
    }
}

MoveToInboxCmd::~MoveToInboxCmd()
{
}

// Copy the selected messages to the Inbox.
CopyToInboxCmd::CopyToInboxCmd(
                             char *name,
			     char *label,
                             int active,
                             RoamMenuWindow *window
                             ) : RoamCmd (name, label, active, window)
{
    _menuwindow = window;
}
 
void
CopyToInboxCmd::doit()
{
    DtMailEnv mail_error;
 
    // Initialize mail_error.
    mail_error.clear();
 
    // Get a handle to the Inbox.
    char * mail_file = NULL;
    DtMailObjectSpace space;
    DtMail::Session * d_session = theRoamApp.session()->session();
 
    d_session->queryImpl(mail_error,
                         d_session->getDefaultImpl(mail_error),
                         DtMailCapabilityInboxName,
                         &space,
                         &mail_file);
    _menuwindow->list()->copySelected(mail_error, mail_file, FALSE, FALSE);
    if (mail_error.isSet()) {
	// We ad an error in copying the messages to the Inbox!
    }
}

CopyToInboxCmd::~CopyToInboxCmd()
{
}


// This is hooked up the Undelete button in the Deleted
// Messages List dialog box.

DoUndeleteCmd::DoUndeleteCmd( 
			      char *name, 
			      char *label,
			      int active, 
			      UndelFromListDialog *undelDialog
			      ) :  Cmd ( name, label, active )
{
    _undelDialog = undelDialog;
}

void
DoUndeleteCmd::doit()
{
    // Undelete the selected messages.
    _undelDialog->undelSelected();
}

void
DoUndeleteCmd::undoit()
{
    // nothing
}

DoUndeleteCmd::~DoUndeleteCmd()
{
}

// This is hooked up to the Close button in the Deleted Messages
// List dialog box.

CloseUndelCmd::CloseUndelCmd( 
			      char *name, 
			      char *label,
			      int active,
			      UndelFromListDialog *undelDialog
			      ) :  Cmd ( name, label, active )
{
    _undelDialog = undelDialog;
}

void
CloseUndelCmd::doit()
{
    // Close the dialog.
    _undelDialog->popped_down();
}

void
CloseUndelCmd::undoit()
{
    // nothing
}


CloseUndelCmd::~CloseUndelCmd()
{
}

UndeleteCmd::UndeleteCmd ( 
			   char *name, 
			   char *label,
			   int active, 
			   RoamMenuWindow *window,
			   Boolean viaDeleteList
			   ) : ChooseCmd  ( name, label, active, window )
{
    _menuwindow = window;
    _undelFromList = NULL;
    _fromList = viaDeleteList;
}

UndeleteCmd::~UndeleteCmd()
{
}

void
UndeleteCmd::doit()
{
    FORCE_SEGV_DECL(MsgStruct, tmpMS);
    MsgScrollingList *list = _menuwindow->list();
    MsgHndArray *deleted_messages;
    DtMailEnv mail_error;
    
    // Initialize the mail_error.
    mail_error.clear();
    
    
    if (_fromList) {
	// Create the Deleted Messages Dialog
	
	if (_undelFromList) {
	    // Hack for user testing.  If the dialog is up, we destroy it.
	    XtDestroyWidget(_undelFromList->baseWidget());
	}
//	if (!_undelFromList) {
	_undelFromList = new UndelFromListDialog(
	                 GETMSG(DT_catd, 1, 227, "Mailer - Deleted Messages"), 
	                 _menuwindow);	
	_undelFromList->initialize();
	
	// Check for existing list of deleted messages
	_num_deleted = list->get_num_deleted_messages();
	
	// If there are deleted messages, put them in the Deleted
	// Messages List.
	
	if (_num_deleted > 0) {
	    deleted_messages = list->get_deleted_messages();
	    _undelFromList->loadMsgs(
				mail_error, 
				deleted_messages, 
				_num_deleted);
	    if (mail_error.isSet()) {
		// Post an exception here!
		_menuwindow->postErrorDialog(mail_error);
	    }
		
	}
	// Display the dialog
	
	_undelFromList->popped_up();
    } else {
	// Although we don't display the Deleted Message Dialog here, we
	// need to make sure that it gets updated for the next time
	// we bring it up.
	list->undelete_last_deleted();
    }
}


#ifdef DEAD_WOOD
SaveCmd::SaveCmd ( char *name, 
		   char *label, 
		   int active, 
		   RoamMenuWindow *window 
		) : RoamCmd ( name, label, active, window )
{
    
}

void
SaveCmd::doit()
{
    
    assert(_menuwindow->mailbox() != NULL);
}
#endif /* DEAD_WOOD */



MoveCopyCmd::MoveCopyCmd( char *name, 
			  char *label,
			  int active, 
			  FileCallback move_callback, 
			  FileCallback copy_callback, 
			  RoamMenuWindow * menu_window,
			  Widget parent,
			  DtMailBoolean only_show_mailboxes)
: UnifiedSelectMailboxCmd(name,
			  label,
			  GETMSG(DT_catd, 1, 89, "Mailer - Other Mailboxes"),
			  "Move",
			  active, 
			  move_callback, 
			  menu_window,
			  parent,
			  only_show_mailboxes)
{
    _copy_callback = copy_callback;
    _menuwindow = menu_window;
    _copy_button = NULL;
    _move_button = NULL;
}

MoveCopyCmd::~MoveCopyCmd()
{
}

void
MoveCopyCmd::setDefault(Widget button)
{
    Arg args[1];

    _default_button = button;
    XtSetArg( args[0], XmNdefaultButton, _default_button );
    XtSetValues( _fileBrowser, args, 1 );
}

void
MoveCopyCmd::doit()
{
    XmString move;
    Widget filter_button;
    Widget unused_button;
    Widget action_area;
    DtMailEnv error;
    
    if (!_fileBrowser) {
	UnifiedSelectMailboxCmd::doit();
	// Customize buttons for MoveCopy dialog
	move = XmStringCreateLocalized(GETMSG(DT_catd, 1, 90, "Move"));
	
	filter_button = XtNameToWidget(_fileBrowser, "*Apply");
	_move_button = XtNameToWidget(_fileBrowser, "*OK");
	action_area = XtParent(_move_button);
	unused_button = XtVaCreateWidget(
			"Unused Button",
			xmPushButtonWidgetClass, _fileBrowser,
			NULL);
	_copy_button = XtVaCreateManagedWidget(
			GETMSG(DT_catd, 1, 237, "Copy"),
			/*xmPushButtonWidgetClass, _fileBrowser,*/
			xmPushButtonGadgetClass, _fileBrowser,
                	XmNlabelString,
                	XmStringCreateLocalized(GETMSG(DT_catd, 1, 43, "Copy")),
			NULL);
	printHelpId("Copy", _copy_button);
	//
	// add help callback
	// XtAddCallback(_copy_button, XmNhelpCallback, HelpCB, helpId);
	//
	XtAddCallback(
		_copy_button,
		XmNhelpCallback, 
		HelpCB,
		(void *)"dtmailViewmainWindowWork-AreapanedWform2RowColumnMoveCopy");
	XtAddCallback(
		_copy_button,
		XmNactivateCallback,
		&MoveCopyCmd::fileSelectedCallback2,
		(XtPointer) this );

	if (_menuwindow->mailbox()->mailBoxWritable(error) == DTM_FALSE)
	  XtUnmanageChild(_move_button);
	else
	  XtManageChild(_move_button);

//  XtVaSetValues(_move_button, XmNsensitive, FALSE);

	_file_list = XtNameToWidget(_fileBrowser, "*ItemsList");
	XtAddCallback(
		_file_list,
		XmNbrowseSelectionCallback, &MoveCopyCmd::setDefaultButtonCB,
		(XtPointer) this);
	XtAddCallback(
		_file_list,
		XmNextendedSelectionCallback, &MoveCopyCmd::setDefaultButtonCB,
		(XtPointer) this);
	XtAddCallback(
		_file_list,
		XmNmultipleSelectionCallback, &MoveCopyCmd::setDefaultButtonCB,
		(XtPointer) this);
	XtAddCallback(
		_file_list,
		XmNsingleSelectionCallback, &MoveCopyCmd::setDefaultButtonCB,
		(XtPointer) this);

	_file_text = XtNameToWidget(_fileBrowser, "*Text");
	if (NULL != _file_text)
	  XtAddCallback(
		_file_text,
		XmNfocusCallback, &MoveCopyCmd::setDefaultButtonCB,
		(XtPointer) this);

	XmStringFree(move);

    } else {
	UnifiedSelectMailboxCmd::doit();
    }
    
}

void
MoveCopyCmd::fileSelectedCallback2 ( 
				     Widget	,
				     XtPointer	clientData,
				     XtPointer	callData
				     )
{
    MoveCopyCmd	*obj = (MoveCopyCmd *) clientData;
    XmFileSelectionBoxCallbackStruct *cb =
        	(XmFileSelectionBoxCallbackStruct *) callData;
    char	*name = NULL;
    char	*dir_str = NULL;
    char	*fname = NULL;
    char	*dname = NULL;
    int		status = 0;
    XmString	xmstr;
    
    static char	selected[MAXPATHLEN+1];

    // Bring the file selection dialog down.
    XtUnmanageChild ( obj->_fileBrowser );  

    //
    // Get the file name
    //
    XtVaGetValues(obj->_fileBrowser, XmNdirectory, &xmstr, NULL);
    if (xmstr)
      dname = (char*) _XmStringUngenerate(
					xmstr, NULL,
					XmMULTIBYTE_TEXT, XmMULTIBYTE_TEXT);
    
    //
    // Get the file name
    //
    XtVaGetValues(obj->_fileBrowser, XmNdirSpec, &xmstr, NULL);
    if (xmstr)
    {
	// Extract the first character string matching the default
	// character set from the compound string
        fname = (char *) _XmStringUngenerate(
					xmstr, NULL,
					XmMULTIBYTE_TEXT, XmMULTIBYTE_TEXT);
	  
	if (NULL == fname || strlen(fname) == 0)
	  return;

	// If a string was successfully extracted, call
	// unifiedMailboxSelected to handle the file.
	*selected = '\0';
	if (NULL != dname) strcat(selected, dname);
	strcat(selected, fname);
        obj->updateUnifiedData();
	obj->unifiedMailboxSelected(
				obj->_copy_callback,
				obj->_menuwindow,
				selected);
    }
}

void
MoveCopyCmd::setDefaultButtonCB( 
			     Widget,
			     XtPointer	clientData,
			     XtPointer)
{
    MoveCopyCmd *thisCmd = (MoveCopyCmd *) clientData;
    thisCmd->setDefault(thisCmd->_default_button);
}


CopyCmd::CopyCmd( char *name, 
		   char *label, 
		   int active, 
		   RoamMenuWindow *window,
		   MoveCopyCmd *move_copy_cmd
		) : RoamCmd ( name, label, active, window )
{
    _move_copy_cmd = move_copy_cmd;
}

CopyCmd::~CopyCmd()
{
}

void
CopyCmd::doit()
{
    _move_copy_cmd->doit();
    _move_copy_cmd->setDefault(_move_copy_cmd->getCopyButton());
}


MoveCmd::MoveCmd( char *name, 
		   char *label, 
		   int active, 
		   RoamMenuWindow *window,
		   MoveCopyCmd *move_copy_cmd
		) : RoamCmd ( name, label, active, window )
{
    _move_copy_cmd = move_copy_cmd;
}

MoveCmd::~MoveCmd()
{
}

void
MoveCmd::doit()
{
    _move_copy_cmd->doit();
    _move_copy_cmd->setDefault(_move_copy_cmd->getMoveButton());
}


NextCmd::NextCmd( 
		  char *name, 
		  char *label,
		  int active, 
		  RoamMenuWindow *window 
		  ) : RoamCmd ( name, label, active, window )
{
}

void
NextCmd::doit()
{
    _menuwindow->list()->select_next_item();
}


PrevCmd::PrevCmd( 
		  char *name, 
		  char *label,
		  int active, 
		  RoamMenuWindow *window 
		  ) : RoamCmd ( name, label, active, window )
{
}

void
PrevCmd::doit()
{
    _menuwindow->list()->select_prev_item();
}

#ifdef DEAD_WOOD
MessagesCmd::MessagesCmd( 
			  char *name, 
			  char *label,
			  int active, 
			  RoamMenuWindow *window 
			  ) : RoamCmd ( name, label, active, window )
{
}


void
MessagesCmd::doit()
{
    Boolean old=_menuwindow->fullHeader();
    MsgScrollingList *list=_menuwindow->list();
    
    ( !strcmp( this->name(), "Full Header" ) ? _menuwindow->fullHeader( True ) : _menuwindow->fullHeader( False ) );
    
    if ( old!=_menuwindow->fullHeader() && _menuwindow->msgView() ) {
//    list->chooseCurrent();
    }
    
}
#endif /* DEAD_WOOD */

PrintCmd::PrintCmd ( 
		     char *name, 
		     char *label,
		     int active, 
		     int silent,
		     RoamMenuWindow *window 
		 ) : ChooseCmd ( name, label, active, window ), _tmp_files(5)
{
    _parent = window;
    _silent = silent;
}

void
PrintCmd::doit()
{
    // The entire implementation of print was broken. It has
    // be removed until a proper implementation can be provided.
    // dlp 10/04/93
    
    printit(_silent);
    return;
}

void
PrintCmd::actioncb(
		   DtActionInvocationID id,
		   XtPointer     clientData,
		   DtActionArg *,
		   int,
		   int           status
		   )
{
    PrintCmd	*data;
    
    switch (status) {
      case DtACTION_INVOKED:
	break;
      default:
	data = (PrintCmd *)clientData;
	data->_parent->message("");
	data->_unregister_tmp_file(id);
	break;
    }
    
    return;
}

void
PrintCmd::printjobcb( Widget w, XtPointer client_data, XtPointer )
{
    char *filename = (char *) client_data;

    XtRemoveCallback(w, XtNdestroyCallback, &PrintCmd::printjobcb, filename);

    unlink(filename);
    free(filename);
}

void
PrintCmd::printit( int silent )
{
    char *p;
    char *silent_str = "DTPRINTSILENT";
    char *tmpdir = new char[MAXPATHLEN+1];
    DtMailEnv	mail_error;
    MsgScrollingList	*list;
    
    DebugPrintf(1, "%s: printit\n", name());
    
    // Create tmp file.
    sprintf(tmpdir, "%s/%s", getenv("HOME"), DtPERSONAL_TMP_DIRECTORY);
    if ((p = tempnam(tmpdir, "dtmail")) == NULL) {
	delete [] tmpdir;
	return;
    }
    delete [] tmpdir;
    
    mail_error.clear();
    list = _parent->list();
    
    // Copy selected messages to a temp file
    int status = list->copySelected(mail_error, p, FALSE, TRUE);
    if (mail_error.isSet())
    {
	_parent->postErrorDialog(mail_error);
	free(p);
	return;
    }
    if (0 != status) return;
    

    DmxPrintJob *pjob = new DmxPrintJob(p,
					(silent ? DTM_TRUE : DTM_FALSE),
					_parent);

    XtAddCallback(pjob->baseWidget(),
		XtNdestroyCallback,
		&PrintCmd::printjobcb,
		(XtPointer) strdup(p));

    pjob->execute();
    free(p);
    return;
}

int
PrintCmd::_register_tmp_file(
			     const char		*name,
			     DtActionInvocationID	id
			     )
{
    struct tmp_file	*f;
    
    // Allocate struct to hold id and temp file
    if ((f = (struct tmp_file *)malloc(sizeof(struct tmp_file))) == NULL) {
	return -1;
    }
    
    // Save file name and action id
    f->file = strdup(name);
    f->id = id;
    
    // Add to list of temp files
    _tmp_files.append(f);
    
    return 0;
}

void
PrintCmd::_unregister_tmp_file(
			       DtActionInvocationID	id
			       )
{
    int n;
    struct tmp_file *f;
    
    // Find the temp file that was used by the Action specified by id
    for (n = _tmp_files.length() - 1; n >= 0; n--) {
	f = _tmp_files[n];
	if (f->id == id) {
	    // Found the file.  Unlink and free data structs
	    unlink(f->file);
	    free(f->file);
	    free(f);
	    // Remove entry from list
	    _tmp_files.remove(n);
	    break;
	}
    }
    
    return;
}

#ifdef DEAD_WOOD
PopupCmd::PopupCmd ( 
		     char *name, 
		     char *label,
		     int active,
		     PopupWindow * (RoamMenuWindow::* member) (void), 
		     RoamMenuWindow *myparent 
		     ) : NoUndoCmd ( name, label, active )
{
    parent=myparent;
    pmpopup=member;
}

void
PopupCmd::doit()
{
    PopupWindow *popup=(parent->*pmpopup)();
//  popup->manage();
}
#endif /* DEAD_WOOD */

// OnItemCmd brings up the Help On Item help.
OnItemCmd::OnItemCmd ( char * name, 
		       char *label, 
		       int active, 
		       UIComponent *window ) 
: NoUndoCmd (name, label, active)
{
    _parent = window;
}

void
OnItemCmd::doit()
{
    int status = DtHELP_SELECT_ERROR;
    Widget widget = _parent->baseWidget();
    Widget selWidget = NULL;
    
    // Display the appropriate help information for the selected item.
    
    status = DtHelpReturnSelectedWidgetId(widget, 0, &selWidget);
    
    switch ((int) status) {
      case DtHELP_SELECT_ERROR:
	printf("Selection Error, cannot continue\n");
	break;
      case DtHELP_SELECT_VALID:
	while (selWidget != NULL) {
	    if ((XtHasCallbacks(selWidget, XmNhelpCallback)
		 == XtCallbackHasSome)) {
		XtCallCallbacks((Widget) selWidget, XmNhelpCallback, NULL);
		break;
	    } else {
		selWidget = XtParent(selWidget);
	    }
	}
	break;
      case DtHELP_SELECT_ABORT:
	printf("Selection Aborted by user.\n");
	break;
      case DtHELP_SELECT_INVALID:
	printf("You must select a component within your app.\n");
	break;
      default:
	;
	// Empty
    }
    
}

OnAppCmd::OnAppCmd ( char * name, 
		     char *label,
		     int active, 
		     UIComponent *window ) 
: NoUndoCmd (name, label, active)
{
    _parent = window;
}

void
OnAppCmd::doit()
{
    DisplayMain (_parent->baseWidget(), "Mailer", DTMAILWINDOWID);
}

TasksCmd::TasksCmd ( char * name, 
		     char *label,
		     int active, 
		     UIComponent *window )
: NoUndoCmd (name, label, active)
{
    _parent = window;
}

void
TasksCmd::doit()
{
    DisplayMain (_parent->baseWidget(), "Mailer", HELP_MAILER_TASKS);
}

ReferenceCmd::ReferenceCmd ( char * name, 
			     char *label,
			     int active, 
			     UIComponent *window )
: NoUndoCmd (name, label, active)
{
    _parent = window;
}

void
ReferenceCmd::doit()
{
    DisplayMain (_parent->baseWidget(), "Mailer", HELP_MAILER_REFERENCE);
}

UsingHelpCmd::UsingHelpCmd ( char * name, 
			     char *label,
			     int active, 
			     UIComponent *window )
: NoUndoCmd (name, label, active)
{
    _parent = window;
}

void
UsingHelpCmd::doit()
{
    DisplayMain (_parent->baseWidget(), "Help4Help", "_HOMETOPIC");
}

RelNoteCmd::RelNoteCmd ( char * name, 
			 char *label,
			 int active, 
			 UIComponent *window 
		     ) : NoUndoCmd (name, label, active )
{
    _parent = window;
    _genDialog = NULL;
}

void
RelNoteCmd::doit()
{
    // int answer;

    // if (!_genDialog)
    //   _genDialog = new DtMailGenDialog("AboutBox", _parent->baseWidget());
    
    // _genDialog->setToAboutDialog();
    // answer = _genDialog->post_and_return(GETMSG(DT_catd, 1, 92, "OK"), NULL);

    DisplayMain(_parent->baseWidget(), "Mailer", "_copyright");
}

RelNoteCmd::~RelNoteCmd()
{
    delete _genDialog;
}

#ifdef DEAD_WOOD
ClearCmd::ClearCmd ( 
		     char * name, 
		     char *label,
		     int active, 
		     RoamMenuWindow *window 
		     ) : NoUndoCmd (name, label, active )
{
    parent=window;
}

void
ClearCmd::doit()
{
//  ((FindPopup *) parent->find_popup())->clear_text_values();
}

StartCmd::StartCmd( char *name, 
		    char *label,
		    int active ) : Cmd ( name, label, active )
{
}

void 
StartCmd::doit()
{
    char *forward= ".forward";
    
    struct passwd pwd;
    GetPasswordEntry(pwd);
    
    char *forward_filename=new char[strlen(pwd.pw_dir)+1+strlen(forward)+1];
    sprintf( forward_filename, "%s/%s", pwd.pw_dir, forward );
}


void 
StartCmd::undoit()
{
}


ChangeCmd::ChangeCmd( 
		      char *name, 
		      char *label,
		      int active 
		      ) : Cmd (name, label, active )
{
}

void 
ChangeCmd::doit()
{
    struct passwd pwd;
    GetPasswordEntry(pwd);
    
    char *user_name=new char[strlen(pwd.pw_name)+1];
    strcpy(user_name,pwd.pw_name);
    
}


void 
ChangeCmd::undoit()
{
}


StopCmd::StopCmd( 
		  char *name, 
		  char *label,
		  int active, 
		  RoamMenuWindow *window 
		  ) : Cmd (name, label, active )
{
    parent=window;
}

void
StopCmd::doit()
{
    unlink( parent->forwardFilename() );
    parent->title( NULL );
}


void
StopCmd::undoit()
{
    
}
#endif /* DEAD_WOOD */



SendCmd::SendCmd(
		 char *name, 
		 char *label,
		 int active, 
		 SendMsgDialog *parent,
		 int trans_type) 
: NoUndoCmd( name, label, active )
{
    _parent=parent;
    _default_trans = trans_type;
}

void
SendCmd::doit()
{
    if (!_parent->isMsgValid())
	return;
    else
	_parent->send_message( this->name(), _default_trans );
}

// JT - Added methods below

OpenMsgCmd::OpenMsgCmd(
		       char *name,
		       char *label,
		       int active,
		       RoamMenuWindow *window) 
: RoamCmd (name, label, active, window)
{
}

void
OpenMsgCmd::doit()
{
    DtMailEnv mail_error;

    // Initialize the mail_error.
    mail_error.clear();

    _menuwindow->list()->viewInSeparateWindow(mail_error);
    if (mail_error.isSet()) {
        _menuwindow->postErrorDialog(mail_error);
    }

}

// Attachment Cmds stuff

SaveAttachCmd::SaveAttachCmd ( char *name, 
			       char *label,
			       char * title,
			       int active, 
			       FileCallback save_callback,
			       RoamMenuWindow *clientData,
			       Widget parent)
:UnifiedSelectFileCmd (name,
		       label,
		       title,
		       GETMSG(DT_catd, 1, 93, "Save"),
		       active,
		       save_callback,
		       clientData,
		       parent)
{
   _parent = clientData;
}

SaveAttachCmd::SaveAttachCmd ( 
			       char *name, 
			       char *label,
			       char * title,
			       int active, 
			       FileCallback save_callback,
			       ViewMsgDialog *clientData,
			       Widget parent
			       )
:UnifiedSelectFileCmd (name,
		       label,
		       title,
		       GETMSG(DT_catd, 1, 93, "Save"),
		       active,
		       save_callback,
		       clientData,
		       parent )
{
   _parent = clientData;
}

SaveAttachCmd::SaveAttachCmd ( 
			       char *name, 
			       char *label,
			       char * title,
			       int active, 
			       FileCallback save_callback,
			       SendMsgDialog *clientData,
			       Widget parent
			       )
:UnifiedSelectFileCmd (name,
		       label,
		       title,
		       GETMSG(DT_catd, 1, 93, "Save"),
		       active,
		       save_callback,
		       clientData,
		       parent )
{
    _parent = clientData;
}

void
SaveAttachCmd::doit()
{
    UnifiedSelectFileCmd::doit();

    DtMailEditor *editor = _parent->get_editor();
    AttachArea *aa = editor->attachArea();
    XmString attachmentName = aa->getSelectedAttachName();
    XtVaSetValues(_fileBrowser, XmNtextString, attachmentName, NULL);
    XtAddCallback ( _fileBrowser, XmNapplyCallback,
		    &SaveAttachCmd::updateCallback,
		    (XtPointer) this);

    _name = XmStringCopy(attachmentName);
    XmStringFree(attachmentName);
}

// Attachment Cmds stuff


void SaveAttachCmd::updateCallback(Widget, XtPointer clientData, XtPointer )
{
    SaveAttachCmd *obj = (SaveAttachCmd *)clientData;
    
    XtVaSetValues(obj->_fileBrowser, XmNtextString, obj->_name, NULL);
}

SaveAsTextCmd::SaveAsTextCmd ( 
			       char *name, 
			       char *label,
			       char *title,
			       int active, 
			       Editor * editor,
			       RoamMenuWindow *parent_roam_menu_window,
			       Widget parent
			       ) 
:UnifiedSelectFileCmd (name,
		       label,
		       title,
		       GETMSG(DT_catd, 1, 95, "Save"),
		       active,
		       fileCB,
		       this,
		       parent )
{
    _text_editor = editor;
    _roam_menu_window = parent_roam_menu_window;
}

SaveAsTextCmd::SaveAsTextCmd ( 
			       char *name, 
			       char *label,
			       char *title,
			       int active, 
			       Editor * editor,
			       void *,
			       Widget parent
			       ) 
:UnifiedSelectFileCmd (name,
		       label,
		       title,
		       GETMSG(DT_catd, 1, 95, "Save"),
		       active,
		       fileCB,
		       this,
		       parent )
{
    _text_editor = editor;
    _roam_menu_window = NULL;
}

void
SaveAsTextCmd::fileCB(void * client_data, char * selection)
{
    SaveAsTextCmd * self = (SaveAsTextCmd *)client_data;
    self->saveText(selection);
}

void
SaveAsTextCmd::saveText(const char * filename)
{
    int answer, status;
    char *buf = new char[2048];
    char * helpId;

    // Is it already there?
    status = SafeAccess(filename, F_OK);
    if (0 == status)
    {
	sprintf(buf,
		GETMSG(DT_catd, 3, 47, "%s already exists.\nOverwrite?"),
		filename);
	
	_genDialog->setToQuestionDialog(GETMSG(DT_catd, 3, 48, "Mailer"), buf);
	helpId = DTMAILHELPERROR;
	answer = _genDialog->post_and_return(helpId);
	if (answer==2) {
	    delete [] buf;
	    return;
	}

	if (unlink(filename) < 0)
	{
	    sprintf(buf, 
		    GETMSG(DT_catd, 3, 49, "Unable to overwrite %s.\n\
Check file permissions and retry."), 
		    filename);
	    _genDialog->setToErrorDialog(GETMSG(DT_catd, 3, 50, "Mailer"), buf);
            helpId = DTMAILHELPNOOVERWRITE;
	    _genDialog->post_and_return(helpId);
	    delete [] buf;
	    return;
	}
    }

    // Create or truncate, and then write the bits.
    int fd = SafeOpen(filename, O_RDWR | O_CREAT | O_TRUNC, 0600);
    if (fd < 0)
    {
	sprintf(buf, GETMSG(DT_catd, 3, 51, "Unable to create %s."), filename);
	_genDialog->setToErrorDialog(GETMSG(DT_catd, 3, 52, "Mailer"), buf);
        helpId = DTMAILHELPNOCREATE;        
	_genDialog->post_and_return(helpId);
	delete [] buf;
	return;
    }
    
    if (SafeWrite(fd, "\n", 1) < 1)
    {
	sprintf(buf, 
		GETMSG(DT_catd, 3, 53, "Unable to write to %s."), 
		filename);
	_genDialog->setToErrorDialog(GETMSG(DT_catd, 3, 54, "Mailer"), buf);
        helpId = DTMAILHELPNOWRITE;
	_genDialog->post_and_return(helpId);
	SafeClose(fd);
	unlink(filename);
	delete [] buf;
	return;
    }
    
    if (NULL == _roam_menu_window)
    {
	char	*text_buf = _text_editor->get_contents();
	writeText((XtPointer) (intptr_t) fd, text_buf);
        XtFree((char*) text_buf);
    }
    else
      writeTextFromScrolledList(fd);
    
    SafeClose(fd);
    delete [] buf;
}

void
SaveAsTextCmd::writeTextFromScrolledList(int fd)
{
    static char	buf[2048];
    char	*helpId;
    char	*tmppath;
    DtMailEnv	mail_error;
    MsgScrollingList *list;
    DmxMailbox	*mailbox;
    DmxMsg	*next_msg;

    //
    // Create temp file.
    //
    char *tmpdir = new char[MAXPATHLEN+1];
    sprintf(tmpdir, "%s/%s", getenv("HOME"), DtPERSONAL_TMP_DIRECTORY);
    if ((tmppath = tempnam(tmpdir, "dtmail")) == NULL) {
	sprintf(buf, GETMSG(DT_catd, 3, 51, "Unable to create %s."), tmpdir);
	_genDialog->setToErrorDialog(GETMSG(DT_catd, 3, 52, "Mailer"), buf);
        helpId = DTMAILHELPNOCREATE;        
	_genDialog->post_and_return(helpId);
	delete [] tmpdir;
	return;
    }
    delete [] tmpdir;

    mail_error.clear();
    list = _roam_menu_window->list();

    //
    // Copy the selected messages to a temp file.
    //
    int status = list->copySelected(mail_error, tmppath, FALSE, TRUE);
    if (mail_error.isSet()) {
        _roam_menu_window->postErrorDialog(mail_error);
        free(tmppath);
        return;
    }
    if (0 != status) return;

    mailbox = new DmxMailbox(tmppath);
    mailbox->loadMessages();
    next_msg = mailbox->firstMessage();
    do
    {
	DmxPrintHeadersEnum	visible_headers;

	if (_roam_menu_window->fullHeader())
	  visible_headers = DMX_PRINT_HEADERS_ALL;
	else
	  visible_headers = DMX_PRINT_HEADERS_ABBREV;

	next_msg->display(
			visible_headers,
			&SaveAsTextCmd::writeText,
			(XtPointer) (intptr_t) fd);
	writeText((XtPointer) (intptr_t) fd, "\n\n");
    } while ((next_msg = mailbox->nextMessage()) != (DmxMsg *) NULL);
    delete mailbox;

    //
    // Clean up the temporary file.
    //
    unlink(tmppath);
    free(tmppath);
}

void
SaveAsTextCmd::writeText(XtPointer filedes, char *text_buf)
{
    long 	fdl = (long) filedes;
    int 	fd = (int) fdl;
    int		len = strlen(text_buf);
    
    if (SafeWrite(fd, text_buf, len) < len) {
#if 0
        char	buf[2048];
        char	*helpId;

	sprintf(
		buf,
		GETMSG(DT_catd, 3, 53, "Unable to write to %s."), 
		filename);
        helpId = DTMAILHELPNOWRITE;
	_genDialog->setToErrorDialog(GETMSG(DT_catd, 3, 56, "Mailer"), buf);
	_genDialog->post_and_return(helpId);
#endif
    }
}

void
SaveAsTextCmd::doit()
{
    MsgScrollingList	*list;
    Widget		listW;
    int			*pos_list = NULL;
    int			pos_count = 0;

    if (_roam_menu_window &&
	(list = _roam_menu_window->list()) &&
	(listW = list->get_scrolling_list()))
    {
        if (!XmListGetSelectedPos(listW, &pos_list, &pos_count))
          return;

        if (0 == pos_count)
        {
	    DtMailGenDialog *dialog = _roam_menu_window->genDialog();

	    dialog->setToErrorDialog(
			    GETMSG(DT_catd, 3, 50, "Mailer"),
			    GETMSG(DT_catd, 2, 16, "No message selected."));
	    dialog->post_and_return(NULL);

            return;
        }
    }

    UnifiedSelectFileCmd::doit();

    if (1 == pos_count)
    {
        int			pos_selected, last_space;
	DtMailEnv		error;
	DtMailHeaderLine	header;
	DtMailMessageHandle	msg_handle;
	DtMailHeaderRequest	request;
	DtMail::MailBox		*mbox;

	pos_selected = list->get_selected_item();
	msg_handle = list->msgno(pos_selected);

	request.number_of_names = 1;
	request.header_name = (char**) malloc(sizeof(char*));
	request.header_name[0] = strdup(DtMailMessageSubject);

	mbox = _roam_menu_window->mailbox();
	mbox->getMessageSummary(error, msg_handle, request, header);

	if (0 != header.header_values[0].length())
	{
	    const char	*orig_subject = *((header.header_values[0])[0]);
	    int		i,j;
	    int		orig_len = strlen(orig_subject);
	    char 	*subject = (char*) malloc(orig_len + 1);
	    XmString	xms;

	    for (i=0,j=0,last_space=0; i<orig_len; i++)
	    {
		if (isspace(orig_subject[i]))
		{
		    if (last_space < i-1)
		      subject[j++] = ' ';

		    last_space = i;
		}
		else if (orig_subject[i] == '/')
		  subject[j++] = '\\';
		else
		  subject[j++] = orig_subject[i];
	    }
	    subject[j] = '\0';

	    xms = XmStringCreateLocalized(subject);
	    XtVaSetValues(_fileBrowser, XmNtextString, xms, NULL);

	    XmStringFree(xms);
	    free(subject);
	}

	if (NULL != request.header_name)
	{
	  if (NULL != request.header_name[0])
	    free(request.header_name[0]);
	  free(request.header_name);
	}

	mbox->clearMessageSummary(header);
    }
}


DeleteAttachCmd::DeleteAttachCmd( 
				  char *name, 
				  char *label,
				  int active,
				  SendMsgDialog *smd
				  
				  ) : Cmd ( name, label, active )
{
    _parent = smd;
}

void
DeleteAttachCmd::doit()
{
    _parent->delete_selected_attachments();
}

void
DeleteAttachCmd::undoit()
{
}

UndeleteAttachCmd::UndeleteAttachCmd( 
				      char *name, 
				      char *label,
				      int active,
				      SendMsgDialog *smd
				      
				      ) : Cmd ( name, label, active )
{
    _parent = smd;
}

void
UndeleteAttachCmd::doit()
{
    _parent->undelete_last_deleted_attachment();
    
}

void
UndeleteAttachCmd::undoit()
{
}

RenameAttachCmd::RenameAttachCmd ( 
				   char *name, 
				   char *label,
				   int active,
				   SendMsgDialog *smd
				   ) : Cmd ( name, label, active )
{
    Widget renameDialog;
    XmString message;
    
    _parent = smd;
    renameDialog = XmCreatePromptDialog(
					smd->baseWidget(), 
					"renameDialog", 
					NULL, 
					0
					);
    
    message = XmStringCreateLocalized(GETMSG(DT_catd, 1, 96, "Empty"));
    XtVaSetValues(renameDialog, XmNselectionLabelString, message, NULL);
    XmStringFree(message);
    XmString ok_str = XmStringCreateLocalized(GETMSG(DT_catd, 1, 97, "Rename"));
    XtVaSetValues(XtParent(renameDialog),
		  XmNtitle, GETMSG(DT_catd, 1, 98, "Mailer - Rename"),
		  NULL);
    XtVaSetValues(renameDialog,
		  XmNokLabelString, ok_str,
		  NULL);

    XmStringFree(ok_str);
    XtUnmanageChild(XmSelectionBoxGetChild(renameDialog, XmDIALOG_HELP_BUTTON));

    _parent->get_editor()->attachArea()->setRenameDialog(renameDialog);
    XtAddCallback(renameDialog, XmNcancelCallback, 
		  &RenameAttachCmd::cancelCallback,
		  (XtPointer) this );
    XtAddCallback(renameDialog, XmNokCallback, 
		  &RenameAttachCmd::okCallback,
		  (XtPointer) this );
}

void RenameAttachCmd::doit()
{
    Widget renameDialog;
    XmString oldAttachName = NULL;
    XmString message;
    char	buf[512];
    AttachArea *aa;
    
    if (!_parent->renameAttachmentOK()) {
	return;
    }
    
    aa = _parent->get_editor()->attachArea();
    
    oldAttachName = aa->getSelectedAttachName();
    
    if (oldAttachName == NULL) return;
    
    renameDialog = aa->getRenameDialog();
    
    sprintf(buf, "%s", GETMSG(DT_catd, 3, 57, "Rename attachment as"));
    
    message = XmStringCreateLocalized(buf);
    
    XtVaSetValues(renameDialog, 
		  XmNselectionLabelString, message,
		  XmNtextString, oldAttachName,
		  NULL);
    
//     XtFree(buf);
    XmStringFree(message);
    XmStringFree(oldAttachName);
    
    XtManageChild(renameDialog);
    XtPopup(XtParent(renameDialog), XtGrabNone);
}      

void RenameAttachCmd::undoit()
{
    // Just print a message that allows us to trace the execution
    
}       

void RenameAttachCmd::cancelCallback ( 
				       Widget, 
				       XtPointer clientData, 
				       XtPointer callData 
				       )
{
    RenameAttachCmd *obj = (RenameAttachCmd *) clientData;
    
    obj->cancel( callData );
}

void RenameAttachCmd::cancel( XtPointer )
{
    AttachArea* aa;
    
    aa = _parent->get_editor()->attachArea();
    
    Widget renameDialog = aa->getRenameDialog();
    
    XtUnmanageChild(renameDialog);
}

void RenameAttachCmd::okCallback ( 
				   Widget, 
				   XtPointer clientData, 
				   XtPointer callData 
				   )
{
    RenameAttachCmd *obj = (RenameAttachCmd *) clientData;
    obj->ok( callData );
}

void RenameAttachCmd::ok( XtPointer callData )
{
    XmSelectionBoxCallbackStruct *cbs = 
	(XmSelectionBoxCallbackStruct *)callData;
    AttachArea *aa;
    
    aa = _parent->get_editor()->attachArea();
    
    Widget renameDialog = aa->getRenameDialog();
    
    XtUnmanageChild(renameDialog);
    
    aa->setSelectedAttachName(cbs->value);
    
}

AttachmentActionCmd::AttachmentActionCmd(
					 char *name,
					 char *label,
					 RoamMenuWindow *rmw,
					 int indx
					 ) : Cmd (name, label, TRUE)
{
    _index = indx;
    
    _parent = rmw;
}

AttachmentActionCmd::AttachmentActionCmd(
					 char *name,
					 char *label,
					 ViewMsgDialog *vmd,
					 int indx
					 ) : Cmd (name, label, TRUE)
{
    _index = indx;
    
    _parent = vmd;
}

AttachmentActionCmd::AttachmentActionCmd(
					 char *name,
					 char *label,
					 SendMsgDialog *smd,
					 int indx
					 ) : Cmd (name, label, TRUE)
{
    _index = indx;
    
    _parent = smd;
}


void
AttachmentActionCmd::doit()
{
    _parent->invokeAttachmentAction(_index);
}

void
AttachmentActionCmd::undoit()
{
}

SelectAllAttachsCmd::SelectAllAttachsCmd(
					 char *name,
					 char *label,
					 RoamMenuWindow *rmw
					 ) : Cmd(name, label, TRUE)
{
    _parent = rmw;
}

SelectAllAttachsCmd::SelectAllAttachsCmd(
					 char *name,
					 char *label,
					 ViewMsgDialog *vmd
					 ) : Cmd(name, label, TRUE)
{
    _parent = vmd;
}

SelectAllAttachsCmd::SelectAllAttachsCmd(
					 char *name,
					 char *label,
					 SendMsgDialog *smd
					 ) : Cmd(name, label, FALSE)
{
    _parent = smd;
}

void
SelectAllAttachsCmd::doit()
{
    _parent->selectAllAttachments();
}

void
SelectAllAttachsCmd::undoit()
{
    // 
}

ShowAttachPaneCmd::ShowAttachPaneCmd(
				     char *name,
				     char *label,
				     AbstractEditorParent *aep
				     ) : ToggleButtonCmd(name, label, TRUE)
{
    _parent = aep;
}

void
ShowAttachPaneCmd::doit()
{
    // If button is OFF
    if (!this->getButtonState()) {
	_parent->hideAttachArea();
    }
    else {  // button is ON
	_parent->showAttachArea();
    }
}

void
ShowAttachPaneCmd::undoit()
{
    // 
}

AbbrevHeadersCmd::AbbrevHeadersCmd(
				   char *name,
				   char *label,
				   RoamMenuWindow *rmw
				   ) : ToggleButtonCmd(name, label, TRUE)
{
    _parent = rmw;
}

void
AbbrevHeadersCmd::doit()
{
    // If button is OFF
    if (!this->getButtonState()) {
	_parent->fullHeader(TRUE);
    }
    else {  // button is ON
	_parent->fullHeader(FALSE);
    }
}

void
AbbrevHeadersCmd::undoit()
{
    // 
}

CloseCmd::CloseCmd( 
    char *name, 
    char *label,
    int active, 
    Widget w, 
    SendMsgDialog *s ) 
    : NoUndoCmd(name, label, active)
{
    _compose_dialog = s;
    menubar_w = w;
}

void
CloseCmd::doit()
{
    // Call the goAway() method on the SMD.  Argument TRUE requests it
    // to check if the SMD is dirty.  Let it handle the 
    // case where text may be present in the compose window.
    if (!_compose_dialog->isMsgValid())
	return;
    else
	_compose_dialog->goAway(TRUE);

}

EditUndoCmd::EditUndoCmd( 
    char *name, 
    char *label,
    int active, 
    AbstractEditorParent *w ) 
    : NoUndoCmd( name, label, active )
{
    editor = w->get_editor()->textEditor();
}

void
EditUndoCmd::doit()
{
	editor->undo_edit();
}

EditCutCmd::EditCutCmd( 
    char *name, 
    char *label,
    int active, 
    AbstractEditorParent *w
) 
    : NoUndoCmd( name, label, active )
{
    editor = w->get_editor()->textEditor();

    // className() is a virtual method
    if (w->className() == "SendMsgDialog") {
	_compose_dialog = (SendMsgDialog *)w;
    }
    else {
	_compose_dialog = NULL;
    }
}

void
EditCutCmd::doit()
{
    editor->cut_selection();
	
    if (_compose_dialog) {
	// Turn Paste on
	_compose_dialog->activate_edit_paste();
	_compose_dialog->activate_edit_paste_indented();
	_compose_dialog->activate_edit_paste_bracketed();
    }
}

EditCopyCmd::EditCopyCmd( 
    char *name, 
    char *label,
    int active, 
    AbstractEditorParent *w ) 
    : NoUndoCmd( name, label, active )
{
    editor = w->get_editor()->textEditor();
    
    // className() is a virtual method
    if (w->className() == "SendMsgDialog") {
	_compose_dialog = (SendMsgDialog *)w;
    }
    else {
	_compose_dialog = NULL;
    }
}

void
EditCopyCmd::doit()
{
    editor->copy_selection();
    if (_compose_dialog) {
	// Turn Paste on
	_compose_dialog->activate_edit_paste();
	_compose_dialog->activate_edit_paste_indented();
	_compose_dialog->activate_edit_paste_bracketed();
    }
}

EditPasteCmd::EditPasteCmd( 
    char *name, 
    char *label,
    int active, 
    AbstractEditorParent *w ) 
    : NoUndoCmd( name, label, active )
{
    editor = w->get_editor()->textEditor();
}

void
EditPasteCmd::doit()
{
	editor->paste_from_clipboard();
}

EditPasteSpecialCmd::EditPasteSpecialCmd( 
    char *name, 
    char *label,
    int active, 
    AbstractEditorParent *w,
    Editor::InsertFormat format) 
    : NoUndoCmd( name, label, active )
{
    editor = w->get_editor()->textEditor();
    insert_format = format;
}

void
EditPasteSpecialCmd::doit()
{
	editor->paste_special_from_clipboard(insert_format);
}


EditClearCmd::EditClearCmd( 
    char *name, 
    char *label,
    int active, 
    AbstractEditorParent *w ) 
    : NoUndoCmd( name, label, active )
{
	editor = w->get_editor()->textEditor();
    // this->deactivate();
}

void
EditClearCmd::doit()
{
    editor->clear_selection();
	// Turn Paste on
	// _edit_paste->activate();
}

EditDeleteCmd::EditDeleteCmd( 
    char *name, 
    char *label,
    int active, 
    AbstractEditorParent *w ) 
    : NoUndoCmd( name, label, active )
{
    editor = w->get_editor()->textEditor();
	// this->deactivate();
}

void
EditDeleteCmd::doit()
{
    editor->delete_selection();
	// Turn Paste off
	// _edit_paste->deactivate();
}

EditSelectAllCmd::EditSelectAllCmd( 
    char *name, 
    char *label,
    int active, 
    AbstractEditorParent *w ) 
    : NoUndoCmd( name, label, active )
{
    editor = w->get_editor()->textEditor();
}

void
EditSelectAllCmd::doit()
{
	editor->select_all();
}



WordWrapCmd::WordWrapCmd( 
    char *name, 
    char *label,
    int active, 
    AbstractEditorParent *w 
) : ToggleButtonCmd( name, label, active )
{
    editor = w->get_editor()->textEditor();
    /*
     * allow the app-defaults setting for WordWrap
     */

    Widget _w = editor->get_text_widget();
    Arg args[1];

    XtSetArg( args[0], XmNwordWrap, &cur_setting );
    XtGetValues( _w, args, 1 );
    editor->set_word_wrap(cur_setting);		
}

void
WordWrapCmd::doit()
{
    cur_setting = ((ToggleButtonCmd *)this)->getButtonState();
    editor->set_word_wrap(cur_setting);		
}

Boolean
WordWrapCmd::wordWrap()
{
    return(cur_setting);
}

FindChangeCmd::FindChangeCmd( 
    char *name, 
    char *label,
    int active, 
    AbstractEditorParent *w ) 
    : NoUndoCmd( name, label, active )
{
    editor = w->get_editor()->textEditor();
}

void
FindChangeCmd::doit()
{
	editor->find_change();
}

SpellCmd::SpellCmd( 
    char *name, 
    char *label,
    int active, 
    AbstractEditorParent *w ) 
    : NoUndoCmd( name, label, active )
{
    editor = w->get_editor()->textEditor();
}

void
SpellCmd::doit()
{
	editor->spell();
}

AliasCmd::AliasCmd(
    char *name,
    char *label,
    int active,
    Widget header)
    : NoUndoCmd (name, label, active)
{
    _header = header;
    _alias = strdup(name);
}

void
AliasCmd::doit()
{
    char	*value;

    XtVaGetValues(_header, XmNvalue, &value, NULL);

    if (strlen(value))
    {
	char	*newvalue = (char *) malloc(strlen(value) + strlen(_alias) + 3);
	sprintf(newvalue, "%s, %s", value, _alias);
	XtVaSetValues(_header, XmNvalue, newvalue, NULL);
	free(newvalue);
    }
    else
      XtVaSetValues(_header, XmNvalue, _alias, NULL);
    
    XtFree(value);
}

AliasCmd::~AliasCmd()
{
    free((void*) _alias);
}


OtherAliasesCmd::OtherAliasesCmd(
    char *name,
    char *label,
    int active)
    : NoUndoCmd (name, label, active)
{
}

void
OtherAliasesCmd::doit()
{
    OptCmd	*optCmd = (OptCmd *) theRoamApp.mailOptions();
    optCmd->displayAliasesOptionsPane();
}

OtherAliasesCmd::~OtherAliasesCmd()
{
}


FormatCmd::FormatCmd( 
    char *name, 
    char *label,
    int active, 
    AbstractEditorParent *w ) 
    : NoUndoCmd( name, label, active )
{
    editor = w->get_editor()->textEditor();
}

void
FormatCmd::doit()
{
	editor->format();
}

LogMsgCmd::LogMsgCmd( 
    char *name, 
    char *label,
    int active, 
    SendMsgDialog * send 
) : ToggleButtonCmd( name, label, active )
{
  // Go to props and find out the default, ie. to log or not to log.
  // But for now, just look in .mailrc to see if "record" is set.

  DtMailEnv error;
  const char *logfile = NULL;

  _send = send;

}

void
LogMsgCmd::doit()
{
    if (!((ToggleButtonCmd *)this)->getButtonState()) {
	// turn off logging for this message
	_send->setLogState(DTM_FALSE);
    }
    else {
	// turn on logging for this message
	_send->setLogState(DTM_TRUE);
    }

}


VacationCmd::VacationCmd(
    char *name,
    char *label
) : Cmd (name, label, TRUE)
{

    _forwardFile = ".forward";
    _backupSuffix = "..BACKUP";
    _subject = NULL;
    _body = NULL;
    _msg = NULL;
    _dialog = NULL;

    // Check if a .forward file exists.  

    _priorVacationRunning = this->priorVacationRunning();

    // parse the .vacation.msg file and retain the subject
    // and body.
    // They need to be retrieved for display in the dialog.

    this->parseVacationMessage();
}


VacationCmd::~VacationCmd()
{
    if (NULL != _subject)
      free((void*) _subject);
    
    if (NULL != _msg)
      delete _msg;
}

void
VacationCmd::doit()
{
}

static unsigned long
writeToFileDesc(const char * buf, int len, va_list args)
{
    int fd = va_arg(args, int);
    int cnt = va_arg(args, int);
    int status = 0;

    do {
	status = SafeWrite(fd, buf, len);
    } while (status < 0 && errno == EAGAIN);
	
    return(0);
}

int
VacationCmd::startVacation(
    char *subj,
    char *text
)
{
    int i = this->handleMessageFile(subj, text);
    if (i == 0) {
	i = this->handleForwardFile();
    }

    return (i);
}

void
VacationCmd::stopVacation()
{
    char *forwardfile = new char[MAXPATHLEN+1];
    
    sprintf(forwardfile, "%s/%s", getenv("HOME"), _forwardFile);

    // Remove the current .forward file (it has vacation in it)
    // Recover and replace the original backup forward file, if 
    // one exists.

    unlink(forwardfile);

    this->recoverForwardFile(forwardfile);
    delete [] forwardfile;
}

Boolean
VacationCmd::priorVacationRunning()

{
    char	buf[256];
    int	fd;
    Boolean	retval = FALSE;
    char *forwardfile = new char[MAXPATHLEN+1];

    sprintf(forwardfile, "%s/%s", getenv("HOME"), _forwardFile);

    if (SafeAccess(forwardfile, F_OK) != 0) {
	delete [] forwardfile;
	return(FALSE);
    }
	
    fd = SafeOpen(forwardfile, O_RDONLY);
	
    buf[sizeof buf -1] = '\0';
    int len;
    while ((len = SafeRead(fd, buf, (sizeof buf) - 1)) > 0) {
	buf[len] = 0;
	if ((strstr(buf, "/usr/bin/vacation")) ||
	    (strstr(buf, "/usr/ucb/vacation"))) {
	    retval = TRUE;
	    break;
	}
    }

    SafeClose(fd);

    delete [] forwardfile;
    return(retval);
}

int
VacationCmd::handleForwardFile()
{
    int		fwd_fd;
    int		bkup_fd;
    int		fsize;
    int		answer;
    int		forwarding;
    int		lastchar;
    caddr_t	fwdptr;
    char	*helpId;
    Boolean	forwardExists;
    DtMailGenDialog	*dialog;

    int error_bufLen = 10000;
    char *error_buf = new char[error_bufLen];
    char *forwardfile = new char[MAXPATHLEN+1];
    char *messagefile = new char[256];
    char *buf = new char[2048];

    // initialize the error_buf
    memset(error_buf, 0, error_bufLen);
    memset(buf, 0, 2048);

    answer = FALSE;
    forwarding = FALSE;
    forwardExists = FALSE;

    passwd pw;
    GetPasswordEntry(pw);

    sprintf(forwardfile, "%s/%s", pw.pw_dir, _forwardFile);
    
    if (SafeAccess(forwardfile, F_OK) == 0 ) {
	forwardExists = TRUE;
	}
    else {
	forwardExists = FALSE;
    }
    
    if (forwardExists && !_priorVacationRunning) {

	/* STRING_EXTRACTION -
	 *
	 * This confirmation window is brought up when the user
	 * tries to update the vacation status when the user is
	 * already using a .forward file.
	 */
	if (NULL != _dialog)
	  dialog = _dialog;
	else
	  dialog = theRoamApp.genDialog();

	sprintf(error_buf, "%s", GETMSG(DT_catd, 1, 102, "You are already using the forwarding facility for\nsomething other than Vacation.  While Vacation is\nrunning, Vacation will be appended to this other\nforwarding activity. Is it still OK to start Vacation?\0"));

	dialog->setToQuestionDialog(GETMSG(DT_catd, 1, 103, "Mailer"), 
				    error_buf);

	helpId = DTMAILHELPOKSTARTVACATION;
	answer = dialog->post_and_return(helpId);

	if (answer == 2) {// Cancel chosen
	    delete [] buf;
    	    delete [] messagefile;
	    delete [] error_buf;
    	    delete [] forwardfile;
	    return 1;
	}
	
	if (this->backupFile(forwardfile) < 0) {
	    delete [] buf;
    	    delete [] messagefile;
	    delete [] error_buf;
    	    delete [] forwardfile;
	    return 1;
	}

	/* A .forward file is currently in use. Merge vacation
	 * into this file, rather than overwrite it. To do so,
	 * set the appropriate variable to indicate mode.
	 */

	_priorVacationRunning = TRUE;
	
	// Turn on the bit to indicate we are currently using a .forward
	forwarding = TRUE;
    }
    else if (forwardExists && _priorVacationRunning) {

	/* STRING_EXTRACTION -
	 *
	 * This confirmation window is brought up when the user
	 * tries to update the vacation status when the user is
	 * already using a .forward file.
	 */

	if (NULL != _dialog)
	  dialog = _dialog;
	else
	  dialog = theRoamApp.genDialog();

 	sprintf(error_buf, "%s", GETMSG(DT_catd, 1, 104, "You are already running the vacation program in your .forward file.\nConsult documentation on how to stop it and remove it from your .forward file.\nTry this command after fixing that problem.\0"));
 	
 	dialog->setToErrorDialog("Error", error_buf);
 	helpId = DTMAILHELPREMOVEVACATION;
 	answer = dialog->post_and_return(helpId);

	delete [] buf;
    	delete [] messagefile;
 	delete [] error_buf;
    	delete [] forwardfile;
 	return 1;

    }	

    // Re-initialize the error_buf
    memset(error_buf, 0, error_bufLen);

    sprintf(messagefile, "%s/.vacation.msg", pw.pw_dir);

    if (forwardExists) {
	fwd_fd = SafeOpen(forwardfile, O_WRONLY | O_APPEND | O_CREAT);
    }
    else {
	fwd_fd = SafeOpen(forwardfile, O_WRONLY | O_CREAT);
    }

    if (fwd_fd < 0 ) {// If fwdfile is not writable/appendable
	
	// Put up error dialog indicating fwdfile not writable
	/* restore the original .forward file */
	
	this->recoverForwardFile(forwardfile);
	delete [] buf;
    	delete [] messagefile;
	delete [] error_buf;
    	delete [] forwardfile;
	return 1;
    }

    SafeFChmod(fwd_fd, 0644);

    // Make buf be forwardfile._backupSuffix.
    // Then create a backup file of name buf
    
    strcpy(buf, forwardfile);
    strcat(buf, _backupSuffix);

    // If we are currently using a .forward then we need to append/prepend
    // the vacation command

    if (forwarding) {

	/* CREATE NEW .forward FILE
	 *
	 * The original .forward file has been renamed to the
	 * backup file name. We need to open the backup .forward
	 * file so we can copy from it.
	 */

	if ((bkup_fd = SafeOpen(buf, O_RDONLY)) < 0) {
	    /* restore the original .forward file */
	    if (answer)
		this->recoverForwardFile(forwardfile);
	    delete [] buf;
    	    delete [] messagefile;
	    delete [] error_buf;
    	    delete [] forwardfile;
	    return 1;
	}

	/* COPY OLD .forward TO NEW .forward
	 *
	 * Using mmap is quite fast, so rather than do a while
	 * loop to copy line by line, we'll use mmap followed by
	 * a write.
	 */
	
	fsize= (int)lseek(bkup_fd, 0, SEEK_END);
	if (fsize > 0)
	{
            fwdptr =
	      (caddr_t) mmap(0, fsize, PROT_READ, MAP_PRIVATE, bkup_fd, 0);

	    // If map failed
	    if (fwdptr == (char *)-1) {
	        // error
	        delete [] buf;
    	        delete [] messagefile;
	        delete [] error_buf;
    	        delete [] forwardfile;
	        SafeClose(bkup_fd);
	        return 1;
	    }

	    // If write failed
	    if (SafeWrite(fwd_fd, fwdptr, fsize) < fsize) {
	        // error
	        delete [] buf;
    	        delete [] messagefile;
	        delete [] error_buf;
    	        delete [] forwardfile;
	        SafeClose(bkup_fd);
	        return 1;
	    }
	
	    /* RELEASE .forward FILE
	     *
	     * Un-mmap the new .forward file
	     */

	    lastchar = fwdptr[fsize-1];
	    munmap(fwdptr, fsize);
        }
	else
	  lastchar = '\n';

	/* APPEND VACATION LINE
	 *
	 * The new .forward file is still open, so append the
	 * new line below as the last line of the .forward file.
	 * Check to make sure last character in the file is a
	 * newline. If it's not, add one so our work goes on
	 * a separate line.
	 */

	if (lastchar != '\n') {
	    lseek(fwd_fd, 0, SEEK_END);
	    char *txt = "\n";
	    if (SafeWrite(fwd_fd, txt, strlen(txt)) < strlen(txt)) {
		// error
	        delete [] buf;
    	        delete [] messagefile;
		delete [] error_buf;
    	        delete [] forwardfile;
		SafeClose(bkup_fd);
		return 1;
	    }
	}

	/*
	 * Now, add the vacation line to the next line.
	 */
	char *append_buf1 = new char[1024*2];
	sprintf(append_buf1, "|\" /usr/bin/vacation %s\"\n", pw.pw_name);

	if (SafeWrite(fwd_fd, append_buf1, strlen(append_buf1)) < 
	    strlen(append_buf1)) {
	    // error
	    delete [] buf;
    	    delete [] messagefile;
	    delete [] error_buf;
    	    delete [] forwardfile;
    	    delete [] append_buf1;
	    SafeClose(bkup_fd);
	    return 1;
	}
    	delete [] append_buf1;

	SafeClose(bkup_fd);
    }

	/* Create known backup file. The known backup
	 * file allows mailtool to differentiate between
	 * vacation being started from mailtool, and vacation
	 * being invoked (the Unix program as opposed to the
	 * MailTool Vacation menu item) via tty session.
	 */

    if (!forwardExists) {

	if ((bkup_fd = SafeOpen(buf, O_WRONLY | O_APPEND | O_CREAT)) < 0) {
	    /* restore the original .forward file */
	    if (answer)
		this->recoverForwardFile(forwardfile);
	    delete [] buf;
    	    delete [] messagefile;
	    delete [] error_buf;
    	    delete [] forwardfile;
	    SafeClose(fwd_fd);
	    return 1;
	}

	char *end_text = "User not using forward file\n";

	if (SafeWrite(bkup_fd, end_text, strlen(end_text)) < 
	    strlen(end_text)) {
	    // error
	    delete [] buf;
    	    delete [] messagefile;
	    delete [] error_buf;
    	    delete [] forwardfile;
	    SafeClose(fwd_fd);
	    return 1;
	}
    }

    if (!forwardExists) {

        /* WRITE NEW .forward FILE
	 *
	 * There was no .forward file, so no appending
	 * must be done. Simply write the standard
	 * vacation line into the new .forward file.
	 */

	char *append_buf2 = new char[1024*2];

	sprintf(append_buf2, "\\%s, |\" /usr/bin/vacation %s\"\n", 
	        pw.pw_name, pw.pw_name);
	if (SafeWrite(fwd_fd, append_buf2, strlen(append_buf2)) <
	    strlen(append_buf2)) {
	    // error
	    SafeClose(bkup_fd);
	    delete [] buf;
    	    delete [] messagefile;
	    delete [] error_buf;
    	    delete [] forwardfile;
    	    delete [] append_buf2;
	    return 1;
	}
    	delete [] append_buf2;
    }

    SafeClose(bkup_fd);
    SafeClose(fwd_fd);

    system("/usr/bin/vacation -I");

    delete [] buf;
    delete [] messagefile;
    delete [] error_buf;
    delete [] forwardfile;
    return 0;
}

int
VacationCmd::backupFile(
    char *file
)
{
	char *buf = new char[MAXPATHLEN+1];

	strcpy(buf, file);
	strcat(buf, _backupSuffix);

	if (rename(file, buf) < 0) {
                /* STRING_EXTRACTION -
                 *
                 * We tried to make a backup copy of your .forward file, but
                 * it failed.  The first %s is the name of the rename
                 * target; the second %s is the system error string.
                 */
	
	    // Put up error dialog 
	    delete [] buf;
	    return(-1);
	}

	delete [] buf;
	return(0);
}

int
VacationCmd::recoverForwardFile(
    char	*file
)
{
	char *buf = new char[BUFSIZ+1];
	int  fd;

	sprintf(buf, "%s", file);
	strcat(buf, _backupSuffix);

	if (rename(buf, file) < 0) {

	    /* STRING_EXTRACTION -
	     *
	     * We tried to restore your original .forward file, but could
	     * not.  The first %s is the name of the original .forward file,
	     * the second %s is the the system error string.
	     */

	    // Handle dialog indicating error in recovering .forward file.
	    // Error usually caused by starting /usr/bin/vacation outside
	    // of dtmail

	    delete [] buf;
	    return(-1);
	}	

	if ((fd = SafeOpen(file, O_RDONLY)) == 0) {
	    delete [] buf;
	    return(-1);
	}
	
	buf[sizeof file -1] = '\0';
	while (SafeRead(fd, buf, BUFSIZ) != 0) {
		if (strstr(buf, "User not using forward file")) {
			unlink(file);
			break;
		}
	}

	SafeClose(fd);

	delete [] buf;
	return(0);
}

char *
VacationCmd::subject()
{
    return(_subject);
}


char *
VacationCmd::body()
{
    if (_body) {
	return((char *)_body);
    }
    else {
	return(NULL);
    }
}

void
VacationCmd::parseVacationMessage()
{
    passwd pw;
    int		fd;

    DtMailGenDialog	*dialog;
    char * helpId;
    int answer;
    char dialog_text[1024*4];
    DtMailEnv error;
    DtMail::Session * d_session = theRoamApp.session()->session();
    DtMailBuffer mbuf;

    if (NULL != _dialog)
      dialog = _dialog;
    else
      dialog = theRoamApp.genDialog();

    GetPasswordEntry(pw);

    char *messagefile = new char[MAXPATHLEN+1];
    sprintf(messagefile, "%s/.vacation.msg", pw.pw_dir);

    // See if the messagefile exists.
    // If it doesn't create one and throw in the text found in the
    // properties sheet.  If no text found, use default template.

    char * fullpath = d_session->expandPath(error, messagefile);
    delete [] messagefile;
    messagefile = NULL;

    // Map the file and try to parse it as a message. If it is a message,
    // then load it with headers. Otherwise, throw everything into the
    // editor.
    //

    fd = SafeOpen(fullpath, O_RDONLY);
    free(fullpath);

    if (fd < 0) {// File doesn't exist
	
	_subject = NULL;
	_body = NULL;
	return;
    }

    struct stat buf;
    if (SafeFStat(fd, &buf) < 0) {

	sprintf(dialog_text, "%s",
		GETMSG(DT_catd, 1, 105, "Cannot open .vacation.msg file -- No write permission."));
	dialog->setToQuestionDialog("Mailer", dialog_text);
	helpId = DTMAILHELPNOWRITEVACATION;
	answer = dialog->post_and_return(helpId);
	    
	_subject = NULL;
	_body = NULL;

	SafeClose(fd);
	return;
    }		

    int page_size = (int)sysconf(_SC_PAGESIZE);
    size_t map_size = (int) (buf.st_size + 
			    (page_size - (buf.st_size % page_size)));

    if (buf.st_size == 0)
      return;

    int free_buf = 0;
    mbuf.size = buf.st_size;
#ifdef __osf__
    mbuf.buffer = (char *)mmap(0, map_size, PROT_READ, MAP_PRIVATE, fd, 0);
#else
    mbuf.buffer = mmap(0, map_size, PROT_READ, MAP_PRIVATE, fd, 0);
#endif
    if (mbuf.buffer == (char *)-1) {
	free_buf = 1;
	mbuf.buffer = new char[mbuf.size];
	if (mbuf.buffer == NULL) {
	    dialog->setToErrorDialog(GETMSG(DT_catd, 3, 59, "No Memory"),
				     GETMSG(DT_catd, 3, 60, "There is not enough memory to load the existing .vacation.msg file."));
	    helpId = DTMAILHELPNOLOADVACATION;
	    answer = dialog->post_and_return(helpId);
	    SafeClose(fd);

	    _subject = NULL;
	    _body = NULL;

	    return;
	}

	if (SafeRead(fd, mbuf.buffer, (unsigned int)mbuf.size) < mbuf.size) {
	    dialog->setToErrorDialog(GETMSG(DT_catd, 3, 61, "Mailer"),
				     GETMSG(DT_catd, 3, 62, "The existing .vacation.msg file appears to be corrupt."));
	    helpId = DTMAILHELPCORRUPTVACATION;
	    answer = dialog->post_and_return(helpId);
	    SafeClose(fd);
	    delete [] mbuf.buffer;
	    _subject = NULL;
	    _body = NULL;

	    return;
	}
    }

    // Now we ask the library to parse it. If this fails for any reason, this
    // is not a message, so we give up.
    //
    DtMail::Message * msg = d_session->messageConstruct(error,
							DtMailBufferObject,
							&mbuf,
							NULL,
							NULL,
							NULL);
    SafeClose(fd);

    if (error.isSet()) {
	_subject = NULL;
	_body = NULL;
	_msg = NULL;
	return;
    }
    else {
	DtMail::Envelope * env = msg->getEnvelope(error);
	DtMailHeaderHandle hnd;

	int hcount = 0;
	char * name;
	DtMailValueSeq value;

	for (hnd = env->getFirstHeader(error, &name, value);
	    error.isNotSet() && hnd;
	    hnd = env->getNextHeader(error, hnd, &name, value)) {
	    
	    if (!strcmp(name, "Subject") == 0) {
		continue;
	    }
	    else {

		int max_len = 0;
		for (int slen = 0; slen < value.length(); slen++) {
		    max_len += strlen(*(value[slen]));
		}

		char * new_str = new char[max_len + (value.length() * 3)];

		strcpy(new_str, "");
		for (int copy = 0; copy < value.length(); copy++) {
		    if (copy != 0) {
			strcat(new_str, " ");
		    }

		    strcat(new_str, *(value[copy]));
		}

		_subject = strdup(new_str);
		value.clear();
		free(name);
		delete [] new_str;
		break;
	    }
	}

	DtMail::BodyPart * bp = msg->getFirstBodyPart(error);
	if (error.isNotSet()) {
	    unsigned long length;

	    bp->getContents(error,
			    &_body,
			    &length,
			    NULL,
			    NULL,
			    NULL,
			    NULL);
	}

	//
	// Avoid a memory leak.
	//
	_msg = msg;
    }

}

int
VacationCmd::handleMessageFile(
    char *subj,
    char *text
)
{
    int		fd;

    DtMailGenDialog	*dialog;
    char * helpId;
    int answer;
    char dialog_text[1024*4];
    Boolean text_changed = FALSE;
    char *messagefile = new char[256];

    BufferMemory buf(4096);

    if (NULL != _dialog)
      dialog = _dialog;
    else
      dialog = theRoamApp.genDialog();

    // Check if a .forward file exists.  
    passwd pw;
    GetPasswordEntry(pw);

    sprintf(messagefile, "%s/.vacation.msg", pw.pw_dir);

    // See if the messagefile exists.
    // If it doesn't create one and throw in the text found in the
    // properties sheet.  If no text found, use default template.

    answer = 0;

    int msg_file_exists = SafeAccess(messagefile, F_OK);
    if (subj != NULL) {
	 if (_subject == NULL || strcmp(_subject, subj) != 0)
		text_changed = TRUE;
    }
    else if (_subject != NULL)
	text_changed = TRUE;

    if (!text_changed) {
    	if (text != NULL) {
	 	if (_body == NULL || strcmp((char*)_body, text) != 0)
			text_changed = TRUE;
    	}
    	else if (_body != NULL)
		text_changed = TRUE;
    }

    if (msg_file_exists >= 0 &&  text_changed) {
	sprintf(dialog_text, "%s",
		GETMSG(DT_catd, 1, 106, ".vacation.msg file exists.  Replace with new text?"));
	dialog->setToQuestionDialog("Mailer", dialog_text);
	helpId = DTMAILHELPEXISTSVACATION;
	answer = dialog->post_and_return(helpId);

	if (answer == 1) {
		// backup the messageFile
		this->backupFile(messagefile);
	}
    }
    
    // If the file doesn't exist or if the user has okayed creation

    if ((msg_file_exists < 0) || (answer == 1)) {

	fd = SafeOpen(messagefile, O_WRONLY | O_CREAT);
	if (fd < 0) {
	    sprintf(dialog_text, "%s",
		GETMSG(DT_catd, 1, 107, "Cannot open .vacation.msg file -- No write permission."));
	    dialog->setToQuestionDialog("Mailer", dialog_text);
	    helpId = DTMAILHELPERROR;
	    answer = dialog->post_and_return(helpId);
	    
	    // Handle dialog indicating file not writable
	    delete [] messagefile;
	    return (-1);	
	}		
	SafeFChmod (fd, 0644);

	if (!subj) { 
	    /* NL_COMMENT
	     * This is the default value of the subject field in the
	     * message that gets returned to the sender when vacation
	     * is turned on.
	     */
	    subj = GETMSG(DT_catd, 1, 108, "I am on vacation");
	} else {
	    buf.appendData("Subject: ", 9);
	    buf.appendData(subj, strlen(subj));
	    buf.appendData("\nPrecedence: junk\n\n", 19);
	}
        if (_subject)
	    free (_subject);
        _subject = strdup(subj);
	
	if (!text) {
	    text = GETMSG(DT_catd, 1, 109,
			    "I'm on vacation.\nYour mail regarding \"$SUBJECT\" will be read when I return.\n");
	}
	buf.appendData(text, strlen(text));
	if (strlen(text) > 0 && text[strlen(text) - 1] != '\n') {
	    buf.appendData("\n", 1);
	}
	_body = strdup(text);

	buf.iterate(writeToFileDesc, fd);
	
	SafeClose(fd);
    }
    delete [] messagefile;
    return(0);
}
