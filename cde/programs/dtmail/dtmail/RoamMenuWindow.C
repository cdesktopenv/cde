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
 *	$TOG: RoamMenuWindow.C /main/80 1999/07/07 14:23:51 mgreess $
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

#include <stdio.h>
#include <unistd.h>
#include <sys/param.h>

#ifdef __uxp__
#include <string.h>
#endif

#if !defined(USL) && !defined(__uxp__)
#include <strings.h>
#endif
#include <errno.h>
#include <unistd.h>
#include <sys/utsname.h>
#ifdef SunOS
#include <sys/systeminfo.h>
#endif
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/cursorfont.h>
#include <Xm/TextF.h>
#include <Xm/Text.h>
#include <Xm/Form.h>
#include <Xm/RepType.h>
#include <Xm/PanedW.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/List.h>
#include <Xm/Xm.h>
#include <Dt/Dnd.h>
#include <DtMail/DtMailTypes.h>
#include <DtMail/IO.hh>
#include "SelectFileCmd.h"
#include "RoamMenuWindow.h"
#include "RoamApp.h"
#include "ButtonInterface.h"
#include "WorkingDialogManager.h"
#ifdef DEAD_WOOD
#include "QuestionDialogManager.h"
#endif /* DEAD_WOOD */
#include "MemUtils.hh"
#include "Help.hh"
#include "DtMailHelp.hh"
#include "MailMsg.h"
#include "SendMsgDialog.h"
#include "AttachArea.h"
#include "Attachment.h"

#include <DtMail/OptCmd.h>
#include "ComposeCmds.hh"
#include "EUSDebug.hh"

#include "SortCmd.hh"

extern int force( Widget );

 static const char * NormalIcon = "DtMail";
 static const char * EmptyIcon = "DtMnone";
 static const char * NewMailIcon = "DtMnew";
 static const char * MailDragIcon = "DtMmsg";

extern nl_catd	DtMailMsgCat;
#define MAXIMUM_PATH_LENGTH	2048

#define		RMW_CONCAT_MAILRC_KEY(buf, pfx, key) \
{ if (NULL==(pfx)) (void) sprintf((buf), "%s", (key)); \
  else (void) sprintf((buf), "%s_%s", (pfx), (key)); }

char *
RoamMenuWindow:: _supported[] =
{
  "TARGETS", "MESSAGES", "STRING", "DELETE"
};
enum {TARGETS,MESSAGES, STR, DEL};

XtResource
RoamMenuWindow::_resources[] =
{
  {
    "fullheader",
    "FullHeader",
    XtRBoolean,
    sizeof( Boolean ),
    XtOffset ( RoamMenuWindow *, _full_header_resource ),
    XtRBoolean,
    ( XtPointer ) 0,
  },

  {
    "mailbox",
    "MailBox",
    XtRString,
    sizeof( XtRString ),
    XtOffset ( RoamMenuWindow *, _mailbox_name_resource ),
    XtRString,
    ( XtPointer ) NULL,
  },

  {
    "mailfiles",
    "MailFiles",
    XtRString,
    sizeof( XtRString ),
    XtOffset ( RoamMenuWindow *, _mail_files_resource ),
    XtRString,
    ( XtPointer ) NULL,
  },

};


// We need to maintain a constant WM_CLASS_NAME for SQE test
// suites to run consistently for all RMWs.  We do that by
// maintaining a constant "name", as in WM_CLASS_NAME, for all
// RMWs by passing "dtmail" to a RMW's parent
// at creation time.  We distinguish individual RMWs via the
// title which indicates what mail container they are currently
// looking at.  The title is passed here as name; its dynamic
// (i.e., each RMW has its own "name", as in WM_NAME).
// If you are going to change the name, talk to SQE
// and get their consent for whatever change you are making.
//
// Note also that the name chosen must match whatever is specified
// in dtwm.fp (front panel configuration file) as the CLIENT_NAME
// for the Mail CONTROL. This is so PUSH_RECALL works.
// Typically this is the executable name.

RoamMenuWindow::RoamMenuWindow (char *name) : MenuWindow ("dtmail", True)
{
    DtMailEnv mail_error;
    
    // Initialize mail_error.
    mail_error.clear();
    
    // Get a handle to the Inbox.
    char		*mail_file = NULL;
    DtMailObjectSpace	space;
    DtMail::Session	*d_session = theRoamApp.session()->session();
    DtMail::MailRc	*mailrc = d_session->mailRc(mail_error);
    const char		*value = NULL;
    char		*buffer = NULL;

    _forward_filename = NULL;
    // Set the _mailbox_name here.  
    // In the absence of the RMW's title being set via title(),
    // the _mailbox_name is what will be used in initialize().
    _mailbox_name = NULL;
    if (name)
      mailboxName(name);
    else
      mailboxName("");
    _mailbox_fullpath = NULL;

    _mailbox_name_resource = NULL;
    _mail_files_resource = NULL;
    _full_header_resource = FALSE;

    _required_conversion = FALSE;
    _checkformail_when_mapped = FALSE;
    _delete_on_quit = FALSE;
    _we_called_newmail = FALSE;

    _dialogs = NULL;
    _numDialogs = 0;
    _findDialog = NULL;

    //
    // ContainerList
    //
    _display_cached_list = FALSE;
    _max_cached_list_size = 0;
    _first_cached_item = 0;
    _user_containerlist = NULL;
    _cached_containerlist = NULL;
    _filemenu2 = NULL;

    //
    // File Menu
    //
    _file_cmdlist = NULL;
    _file_separator = NULL;
    _file_check_new_mail = NULL;
    _file_open_inbox	= NULL;
    _file_new_container = NULL;
    _file_open	= NULL;
    _file_destroy_deleted_msgs = NULL;
    _file_quit	= NULL;

    _file_cascade = NULL;

    //
    // OpenContainer Cascade Menu
    //
    _open_container_cmdlist = NULL;
    _open_container_separator = NULL;
    _open_container_inbox = NULL;
    _open_container_other = NULL;

    _open_container_containerlist = NULL;
    _open_container_containerlist_cached = NULL;
    _opencontainerMenu = NULL;

    //
    // Message Menu
    //
    _msg_cmdlist = NULL;
    _msg_separator = NULL;
    _msg_open = NULL;
    _msg_save_as = NULL;
    _msg_print = NULL;
    _msg_find = NULL;
    _msg_select_all = NULL;
    _msg_delete = NULL;
    _msg_undelete_last = NULL;
    _msg_undelete_from_list = NULL;
    

    //
    // CopyTo Cascade Menu
    //
    _copyto_cmdlist = NULL;
    _copyto_separator = NULL;
    _copyto_inbox = NULL;
    _copyto_other = NULL;

    _copyto_containerlist = NULL;
    _copyto_containerlist_cached = NULL;
    _copytoMenu = NULL;
    _message_cascade = NULL;
    

    //
    // Edit Menu
    //
    _edit_cmdlist = NULL;
    _edit_copy = NULL;
    _edit_select_all = NULL;


    //
    // Attachment menu
    //
    _att_cmdlist = NULL;
    _att_save = NULL;
    _att_select_all = NULL;

    //
    // View Menu
    //
    _view_cmdlist = NULL;
    _view_separator = NULL;
    _view_next = NULL;
    _view_previous = NULL;
    _view_abbrev_headers = NULL;
    _view_sortTD = NULL;
    _view_sortSender = NULL;	
    _view_sortSubject = NULL;	
    _view_sortSize = NULL;	
    _view_sortStatus = NULL;
    
    //
    // Compose Menu
    //
    _comp_cmdlist = NULL;
    _comp_separator = NULL;
    _comp_new = NULL;
    _comp_new_include = NULL;
    _comp_forward = NULL;
    _comp_replySender = NULL;
    _comp_replyAll = NULL;
    _comp_replySinclude = NULL;
    _comp_replyAinclude = NULL;
    
    
    //
    // Move Menu
    //
    _move_cmdlist = NULL;
    _move_separator = NULL;
    _move_inbox = NULL;
    _move_other = NULL;

    _move_containerlist = NULL;
    _move_containerlist_cached = NULL;
    _moveMenu = NULL;
    _move_cascade = NULL;


    // Help Menu
    _help_cmdlist = NULL;
    _help_separator = NULL;
    _help_overview = NULL;
    _help_tasks = NULL;
    _help_reference = NULL;
    _help_on_item = NULL;
    _help_using_help = NULL;
    _help_about_mailer = NULL;
    
    //
    // Message Popup
    //
    _msgsPopup_cmdlist = NULL;
    _msgsPopup_separator = NULL;

    _menuPopupMsgs = NULL;
    _msgsPopupMenu = NULL;
    _msgsPopupMoveMenu = NULL;
    
    //
    // Text Popup
    //
    _textPopup_cmdlist = NULL;
    _textPopup_separator = NULL;
    
    //
    // Attachment Popup
    //
    _attPopup_cmdlist = NULL;
    _attPopup_separator = NULL;
    _attActions_cmdlist = NULL;

    _attachmentMenu = NULL;

    //
    // ConvertContainerCmd
    //
    _convertContainerCmd = NULL;
    _openContainerCmd = NULL;

    //
    // Message view pane buttons
    //
    _delete_button = NULL;
    _next_button = NULL;
    _previous_button = NULL;
    _replySender_button = NULL;
    _print_button = NULL;
    _move_copy_button = NULL;

    
    _mailbox    = NULL;
    d_session->queryImpl(mail_error,
			 d_session->getDefaultImpl(mail_error),
			 DtMailCapabilityInboxName,
			 &space,
			 &mail_file);
    if (strcmp(mail_file, mailboxName()) == 0)
      _inbox = TRUE;
    else
      _inbox = FALSE;

    if (NULL != mail_file)
      free((void*)mail_file);

    _list	= NULL;
    _genDialog  = NULL;

    //
    // Initialize private variables
    //
    _mbox_image = 0;
    _mbox_mask  = 0;

    _my_editor = NULL;
    _message = NULL;
    _message_summary = NULL;
    _rowOfLabels = NULL;
    _rowOfButtons = NULL;
    _rowOfMessageStatus = NULL;

    _quitWorkprocID = 0;
    _clear_message_p = FALSE;
    _is_loaded = FALSE;


    buffer = XtMalloc(BUFSIZ);
    if (buffer)
    {
	if (_inbox)
	{
    	    RMW_CONCAT_MAILRC_KEY(buffer, DTMAS_INBOX, "sortby");
	}
	else
	{
    	    RMW_CONCAT_MAILRC_KEY(buffer, mailboxName(), "sortby");
	}
    }
    
    mail_error.clear();
    mailrc->getValue(mail_error, buffer, &value);
    if (mail_error.isSet() || NULL == value)
      _last_sorted_by = SortTimeDate;
    else
      _last_sorted_by = (SortBy) atoi(value);

    if (NULL != value) free((void*) value);
    if (NULL != buffer) XtFree(buffer);

}

RoamMenuWindow::~RoamMenuWindow() 
{
    DtMailEnv	error;

    error.clear();
    theRoamApp.session()->close(error, _mailbox);
    if (error.isSet()) {
	// Cannot return error to caller since there's
	// no caller.  What do we do?
	// exit(0)??
    }
    
    if (NULL != _forward_filename)
      free(_forward_filename);
    if (_mailbox_name)
        free((void*) _mailbox_name);
    if (_mailbox_fullpath)
        free((void*) _mailbox_fullpath);

    if (NULL != _dialogs)
	delete []_dialogs;
    if (NULL != _findDialog)
      delete _findDialog;


    // ContainerLists
    if (NULL != _user_containerlist)
      delete _user_containerlist;
    if (NULL != _cached_containerlist)
      delete _cached_containerlist;
    if (NULL != _filemenu2)
      free((void*) _filemenu2);

    // File Menu
    delete	_file_cmdlist;
    delete	_file_separator;
    delete	_file_check_new_mail;
    delete	_file_open_inbox;
    delete	_file_new_container;
#if defined(USE_OLD_FILE_OPEN)
    delete	_file_open;
#endif
    delete	_file_destroy_deleted_msgs;
    delete	_file_quit;

    // OpenContainer Cascade Menu
    delete	_open_container_cmdlist;
    delete	_open_container_separator;
    delete	_open_container_inbox;
    delete	_open_container_other;

    if (NULL != _open_container_containerlist)
      delete _open_container_containerlist;
    if (NULL != _open_container_containerlist_cached)
      delete _open_container_containerlist_cached;


    // Message Menu
    delete	_msg_cmdlist;
    delete	_msg_separator;
    delete	_msg_open;
    delete	_msg_save_as;
    delete	_msg_print;
    delete	_msg_find;
    delete	_msg_delete;
    delete	_msg_undelete_last;
    delete	_msg_undelete_from_list;

    // CopyTo Cascade Menu
    delete	_copyto_cmdlist;
    delete	_copyto_separator;
    delete	_copyto_inbox;
    delete	_copyto_other;

    if (NULL != _copyto_containerlist)
      delete _copyto_containerlist;
    if (NULL != _copyto_containerlist_cached)
      delete _copyto_containerlist_cached;

    // Edit Menu
    delete	_edit_cmdlist;
    delete	_edit_copy;
    delete	_edit_select_all;

    // Attachment menu
    delete 	_att_cmdlist;
    delete	_att_save;
    delete	_att_select_all;

    // View Menu
    delete	_view_cmdlist;
    delete	_view_separator;
    delete	_view_next;
    delete	_view_previous;
    delete	_view_abbrev_headers;
    delete	_view_sortTD;
    delete	_view_sortSender;	
    delete	_view_sortSubject;	
    delete	_view_sortSize;	
    delete	_view_sortStatus;
    
    // Compose Menu
    delete	_comp_cmdlist;
    delete	_comp_separator;
    delete	_comp_new;
    delete	_comp_new_include;
    delete	_comp_forward;
    delete	_comp_replySender;
    delete	_comp_replyAll;
    delete	_comp_replySinclude;
    delete	_comp_replyAinclude;
    
    // Move menu
    delete	_move_cmdlist;
    delete	_move_separator;
    delete	_move_inbox;
    delete	_move_other;

    if (NULL != _move_containerlist)
      delete _move_containerlist;
    if (NULL != _move_containerlist_cached)
      delete _move_containerlist_cached;

    // Help Menu
    delete	_help_separator;
    delete	_help_cmdlist;
    delete	_help_overview;
    delete	_help_tasks;
    delete	_help_reference;
    delete	_help_on_item;
    delete	_help_using_help;
    delete	_help_about_mailer;
    
    // Message Popup
    delete      _msgsPopup_cmdlist;
    delete	_msgsPopup_separator;

    delete 	_menuPopupMsgs;

    // Text Popup
    delete 	_textPopup_cmdlist;
    delete	_textPopup_separator;

    // Attachment Popup
    delete 	_attPopup_cmdlist;
    delete	_attPopup_separator;
    delete	_attActions_cmdlist;
    
    // ConvertContainerCmd
    delete	_convertContainerCmd;
    delete	_openContainerCmd;
    
    // Message view pane buttons
    delete	_delete_button;
    delete	_next_button;
    delete	_previous_button;
    delete      _replySender_button;
    delete	_print_button;
    delete	_move_copy_button;
    
    delete	_genDialog;

    if (_list)
    {
	_list->shutdown();
        delete	_list;
    }

    // Private
    if (_mbox_image)
	XmDestroyPixmap(XtScreen(baseWidget()), _mbox_image);
    if (_mbox_mask)
	XmDestroyPixmap(XtScreen(baseWidget()), _mbox_mask);
    delete	_my_editor;
}

void
RoamMenuWindow::initialize()
{
    XmRepTypeInstallTearOffModelConverter();
    MenuWindow::initialize();

    if (!this->baseWidget())
    {
	fprintf(
		stderr, "%s",
		GETMSG(DT_catd, 2, 3,
		  "Unable to initialize windows. Exiting.\n"));
	exit(1);
    }

    busyCursor();
    setStatus(GETMSG(DT_catd, 3, 2, "Initializing..."));

    // XInternAtom(XtDisplay(this->baseWidget()) ,"STRING", False);
    // XInternAtom(XtDisplay(this->baseWidget()) ,"MESSAGES", False);
    getResources(_resources, XtNumber(_resources));
    if (_mailbox_name_resource) mailboxName(_mailbox_name_resource);
    
    // Mailbox image & mask
    {
	Screen *screen = XtScreen(baseWidget());
	char    icon_name[256];

	sprintf(icon_name, "%s.m", MailDragIcon);
	_mbox_image = XmGetPixmap(screen, icon_name,
	    BlackPixelOfScreen(screen), WhitePixelOfScreen(screen));

	sprintf(icon_name, "%s.m_m.bm", MailDragIcon);
	_mbox_mask = XmGetPixmapByDepth(screen, icon_name, 1, 0, 1);

	if (_mbox_image == XmUNSPECIFIED_PIXMAP ||
	    _mbox_mask == XmUNSPECIFIED_PIXMAP)
	  _mbox_image = _mbox_mask = 0;
    }
    
    // Add an event handler for structureNotify.
    // This EH will handle exposure, configure notifies ...
    XtAddEventHandler(
		this->baseWidget(), StructureNotifyMask, False,
		(XtEventHandler) &RoamMenuWindow::structurenotify,
		(XtPointer) this);

    _convertContainerCmd = new ConvertContainerCmd(
					"convertContainer", 
					"convertContainer", 
					TRUE, this);
    _openContainerCmd = new OpenContainerCmd(
					"openContainer", 
					"openContainer",
					TRUE, this);
    _genDialog = new DtMailGenDialog("Dialog", _main);
    setIconName(NormalIcon);
}

Boolean
RoamMenuWindow::inbox()
{
    return(_inbox);
}

Widget
RoamMenuWindow::createWorkArea(Widget parent)
{
    Widget form1, panedW;
    Widget form2, form3;
    Dimension x, y, width, height, bwid;
    VacationCmd *vacation_cmd = theRoamApp.vacation();

    form1 = XmCreateForm(parent, "Work_Area",  NULL, 0);
    XtVaSetValues(form1, XmNresizePolicy, XmRESIZE_NONE, NULL);

    printHelpId("form1", form1);
    /* add help callback */
    // XtAddCallback(form1, XmNhelpCallback, HelpCB, helpId);
    XtAddCallback(form1, XmNhelpCallback, HelpCB, (void *)DTMAILWINDOWID);

    panedW = XtCreateManagedWidget("panedW", xmPanedWindowWidgetClass,
				   form1, NULL, 0);
    printHelpId ("panedW", panedW);
    /* add help callback */
    // XtAddCallback(panedW, XmNhelpCallback, HelpCB, helpId);

    XtVaSetValues(panedW,
	XmNrightAttachment,XmATTACH_FORM, 
	XmNleftAttachment,XmATTACH_FORM, 
	XmNtopAttachment,XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,
	XmNsashIndent, -30,
	XmNsashWidth, 18,
	XmNsashHeight, 12,
	NULL );


    form2 = XtCreateManagedWidget("form2", xmFormWidgetClass, panedW, NULL, 0);
    XtVaSetValues(form2, XmNallowResize, True, NULL);
    printHelpId ("form2", form2);
    /* add help callback */
    // XtAddCallback(form2, XmNhelpCallback, HelpCB, helpId);

    _rowOfLabels =
      XtCreateManagedWidget("RowOfLabels", xmFormWidgetClass, form2, NULL, 0);

    printHelpId("rowOfLabels", _rowOfLabels);
    /* add help callback */
    // XtAddCallback(_rowOfLabels, XmNhelpCallback, HelpCB, helpId);
    XtAddCallback(
	_rowOfLabels, XmNhelpCallback, HelpCB, (void *)DTMAILWINDOWROWOFLABELSID);

    XtVaSetValues(_rowOfLabels, 
		  XmNrightAttachment,XmATTACH_FORM, 
		  XmNleftAttachment,XmATTACH_FORM, 
		  XmNtopAttachment,XmATTACH_FORM,
		  XmNtopOffset, 5,
		  XmNorientation, XmHORIZONTAL,
		  XmNspacing, 120,
		  XmNmarginWidth, 10,
		  XmNentryAlignment, XmALIGNMENT_CENTER,
		  NULL);

    _list = new MsgScrollingList( this, form2, "Message_List");
    msgListDragSetup();

    DtMailEnv error;
    DtMail::Session *d_session = theRoamApp.session()->session();
    DtMail::MailRc  *mailrc = d_session->mailRc(error);
    const char      *value = NULL;
    int		    msgnums = False;

    this->addToRowOfLabels(_list);

    // See if the header size has changed.
    value = NULL;
    mailrc->getValue(error, "headerlines", &value);
    if (error.isSet()) value = strdup("15");

    int header_lines = (int) strtol(value, NULL, 10);
    _list->visibleItems(header_lines);
    if (NULL != value) free((void*) value);

    XtAddCallback(
	_list->get_scrolling_list(), XmNhelpCallback, 
	HelpCB, (void *)DTMAILMSGLISTID);

    XtVaSetValues(_list->get_scrolling_list(), XmNuserData, this, NULL);
    _rowOfButtons =
      XtCreateManagedWidget("RowColumn", xmFormWidgetClass, form2, NULL, 0);
    printHelpId ("rowOfButtons", _rowOfButtons);
    // Add help callback
    // XtAddCallback(_rowOfButtons, XmNhelpCallback, HelpCB, helpId);


    // Place it closer to the scrolling list than to the text widget
    // that is below.
    XtVaSetValues(XtParent(_list->get_scrolling_list()),
		XmNrightAttachment,XmATTACH_FORM, 
		XmNleftAttachment,XmATTACH_FORM, 
		XmNtopAttachment,XmATTACH_WIDGET,
		XmNtopWidget, _rowOfLabels,
		XmNtopOffset, 3,
		XmNbottomAttachment, XmATTACH_WIDGET,
		XmNbottomWidget, _rowOfButtons,
		XmNbottomOffset, 7,
		NULL );

    XtVaSetValues(_rowOfButtons,
		  XmNrightAttachment,XmATTACH_FORM, 
		  XmNleftAttachment,XmATTACH_FORM, 
		  XmNbottomAttachment, XmATTACH_FORM,
		  XmNresizable, FALSE, 
		  NULL );

    this->addToRowOfButtons();

    form3 = XtCreateManagedWidget("form3", xmFormWidgetClass, panedW, NULL, 0);
    XtVaSetValues(form3, XmNallowResize, True, NULL);
    printHelpId ("form3", form3);
    // Add help callback
    // XtAddCallback(form3, XmNhelpCallback, HelpCB, helpId);

#if 0
    XtVaSetValues(form3,
	XmNrightAttachment,XmATTACH_FORM, 
	XmNleftAttachment,XmATTACH_FORM, 
	XmNtopAttachment,XmATTACH_FORM, 
	NULL );
#endif

    _rowOfMessageStatus =
      XtCreateManagedWidget("Message_Status", xmFormWidgetClass, form3, NULL,0);

     XtVaSetValues(_rowOfMessageStatus,
  	XmNrightAttachment,XmATTACH_FORM, 
  	XmNleftAttachment,XmATTACH_FORM, 
  	XmNtopAttachment,XmATTACH_FORM,
 	XmNtopOffset, 5,
 	NULL );

    this->addToRowOfMessageStatus();

    _my_editor = new DtMailEditor(form3, this);
    _my_editor->initialize();
    _my_editor->attachArea()->setOwnerShell(this);

    // DtMailEditor contains a widget that contains the textEditor and 
    // attachArea.  Get that widget...

    Widget wid = _my_editor->container();

    printHelpId ("wid", wid);
    /* add help callback */
    // XtAddCallback(wid, XmNhelpCallback, HelpCB, helpId);

    XtVaSetValues(wid,
	XmNrightAttachment,XmATTACH_FORM, 
	XmNleftAttachment,XmATTACH_FORM, 
	XmNtopAttachment,XmATTACH_WIDGET, 
	XmNtopWidget, _rowOfMessageStatus,
	XmNbottomAttachment, XmATTACH_FORM,
	NULL );

    DtMailEnv mail_error;

    // Initialize the mail_error.
    mail_error.clear();
    mailboxFullpath(mailboxName());

    // Set the appShell's title to be _mailbox_fullpath
    if (vacation_cmd != NULL && vacation_cmd->priorVacationRunning())
      setVacationTitle();
    else
      setTitle(NULL);

    // Set the icon name to the folder name.  Extract foldername from full path.
    char *fname = strrchr(_mailbox_fullpath, '/');
    if (fname == NULL)
      setIconTitle(_mailbox_fullpath);
    else
    {
	// Extract the filename from it.
	fname++;
	setIconTitle(fname);
    }

    XtManageChild(_list->baseWidget());
    XtManageChild(form1);
    
    value = NULL;
    mailrc->getValue(error, "retrieveinterval", &value);
    if (error.isNotSet() && value && *value != '\0')
    {
	long ping = (time_t) strtol(value, NULL, 10);
	if (ping <= 0)
	  _checkformail_when_mapped = FALSE;
	else
	  _checkformail_when_mapped = TRUE;
    }
    error.clear();
    if (NULL != value) free((void*) value);

    XtVaGetValues(_main,
		  XmNx, &x,
		  XmNy, &y,
		  XmNwidth, &width,
		  XmNheight, &height,
		  XmNborderWidth, &bwid,
		  NULL);
    _x = x;
    _y = y;
    _width = width;
    _height = height;
    _border_width = bwid;
    return form1;
}

// This routine is just a place holder callback to pass when creating
// a mailbox when we doing want to do anything when the mailbox gets
// updated.
//
static DtMailBoolean
openCallback(
	DtMailCallbackOp,
	const char *,
	const char *,
	void *,
	...
)
{
    return(DTM_FALSE);
}

// msgListTransferCallback
//
// Handles the transfer of data that is dragged and dropped to the
// MsgScrollingList.  Files and buffers are transferred by inserting 
// them as messages into the scrolling list.
//
void
RoamMenuWindow::msgListTransferCallback(
    Widget	,
    XtPointer	clientData,
    XtPointer	callData)
{
    RoamMenuWindow *rmw = (RoamMenuWindow *) clientData;
    DtDndTransferCallbackStruct *transferInfo =
			(DtDndTransferCallbackStruct *) callData;
    DtDndContext        *dropData = transferInfo->dropData;
    char		*filepath;
    int			ii, numItems;
    DtMail::MailBox	*mbox, *tmpmbox;
    DtMailEnv mail_error;
    MailSession *session = theRoamApp.session();

    DebugPrintf(3, "In RoamMenuWindow::msgListTransferCallback\n");

    // Initialize the mail_error.
    mail_error.clear();

    numItems = dropData->numItems;

    switch (dropData->protocol) {
	case DtDND_FILENAME_TRANSFER:

    	    // Loop through the files that were dropped onto the msg list

    	    for (ii = 0; ii < numItems; ii++) {

		// Try to open the file as a mail container.
		filepath = dropData->data.files[ii];
		tmpmbox = session->open(
				mail_error, filepath, 
				&RoamMenuWindow::syncViewAndStoreCallback,
				rmw, DTM_FALSE, DTM_TRUE);

		// Reject the drop if we weren't able to open the file.
		if (!tmpmbox || mail_error.isSet()) {
		    transferInfo->status = DtDND_FAILURE;
		    return;
		} else {
		    // We were able to open the container, so now we get the 
		    // current mailbox and copy all the messages into it.
		    mbox = rmw->mailbox();
		    mbox->copyMailBox(mail_error, tmpmbox);
		    rmw->checkForMail(mail_error);
		    delete tmpmbox;
		}
	    }
	    break;

	case DtDND_BUFFER_TRANSFER:

    	    // Loop through the buffers that were dropped onto the msg list

    	    for (ii = 0; ii < numItems; ii++) {

		// Turn the dropped data into a mail buffer to pass to
		// the mailbox constructor.
		DtMailBuffer buf;
		buf.buffer = (char *) transferInfo->dropData->data.buffers->bp;
		buf.size = transferInfo->dropData->data.buffers->size;

		// Convert the buffer into a mailbox object.
		tmpmbox = session->session()->mailBoxConstruct(mail_error,
						DtMailBufferObject, &buf,
						openCallback, NULL);

		// Reject the drop if we weren't able to convert it to a mailbox
		if (!tmpmbox || mail_error.isSet()) {
		    transferInfo->status = DtDND_FAILURE;
		    return;
		} else {
		    // Parse the dropped data into the tmpmbox.
		    tmpmbox->open(mail_error);
		    if (mail_error.isSet()) {
			transferInfo->status = DtDND_FAILURE;
			delete tmpmbox;
			return;
		    }
		    mbox = rmw->mailbox();
		    mbox->copyMailBox(mail_error, tmpmbox);
		    rmw->checkForMail(mail_error);
		    delete tmpmbox;
	        }
	    }
	    break;

	default:
	    transferInfo->status = DtDND_FAILURE;
	    return;
    }
}

// msgListDropRegister
//
// Register the message list to accept file & buffer drops
//
void
RoamMenuWindow::msgListDropRegister()
{
    static XtCallbackRec transferCBRec[] = { 
	{&RoamMenuWindow::msgListTransferCallback, NULL}, {NULL, NULL} };

    // Pass the RoamMenuWindow object (this) as clientData.
    transferCBRec[0].closure = this;

    DtDndVaDropRegister(_list->get_scrolling_list(),
	    DtDND_FILENAME_TRANSFER | DtDND_BUFFER_TRANSFER,
	    (unsigned char)(XmDROP_MOVE | XmDROP_COPY), transferCBRec,
	    DtNtextIsBuffer, True,
	    NULL);
}

// msgListDropEnable
//
// Enable the message list for drops by restoring the operation
//
void
RoamMenuWindow::msgListDropEnable()
{
    Arg	args[1];

    XtSetArg(args[0], XmNdropSiteOperations, XmDROP_MOVE | XmDROP_COPY);
    XmDropSiteUpdate(_list->get_scrolling_list(), args, 1);
}

// msgListDropDisable
//
// Disable the message list for drops by setting the operation to noop
//
void
RoamMenuWindow::msgListDropDisable()
{
    Arg	args[1];

    XtSetArg(args[0], XmNdropSiteOperations, XmDROP_NOOP);
    XmDropSiteUpdate(_list->get_scrolling_list(), args, 1);
}


// msgListConvertCallback
//
// Provide the msg list as a mailbox for the drag
//
void
RoamMenuWindow::msgListConvertCallback(
    Widget	/* dragContext */,
    XtPointer	clientData,
    XtPointer	callData)
{
    DtDndConvertCallbackStruct *convertInfo =
			    (DtDndConvertCallbackStruct *) callData;
    DtDndBuffer		*buffer = &(convertInfo->dragData->data.buffers[0]);
    RoamMenuWindow 	*rmw = (RoamMenuWindow *)clientData;
    MsgScrollingList	*msgList;
    Widget		listW;
    DtMail::MailBox	*mbox;
    DtMail::Message	*msg;
    DtMailMessageHandle msgHandle;
    DtMailBuffer 	*mbufList;
    int			*pos_list, pos_count, ii;
    unsigned long	bufSize = 0;
    char 		*bufPtr;
    DtMailEnv 		error;

    DebugPrintf(3, "In RoamMenuWindow:msgListConvertCallback\n");

    error.clear();

    switch (convertInfo->reason) {
	case DtCR_DND_CONVERT_DATA:

	    msgList = rmw->list();
	    listW = msgList->get_scrolling_list();
	    mbox = rmw->mailbox();

	    if (!XmListGetSelectedPos(listW, &pos_list, &pos_count)) {
		convertInfo->status = DtDND_FAILURE;
		return;
	    }

	    mbufList = (DtMailBuffer*)XtMalloc(pos_count*sizeof(DtMailBuffer));
	
	    for (ii = 0; ii < pos_count; ii++) {
		    msgHandle = msgList->msgno(pos_list[ii]);
		    msg = mbox->getMessage(error, msgHandle);
		    if (error.isSet()) {
			convertInfo->status = DtDND_FAILURE;
			return;
		    }
		    msg->toBuffer(error, mbufList[ii]);
		    if (error.isSet()) {
			convertInfo->status = DtDND_FAILURE;
			return;
		    }
		    bufSize += mbufList[ii].size + 1;
	    }

	    bufPtr = XtMalloc((unsigned int)bufSize);

	    buffer->bp = bufPtr;
	    buffer->size = (int)bufSize;
	    buffer->name = "Mailbox";

	    for (ii = 0; ii < pos_count; ii++) {
		strncpy(bufPtr, (char *)mbufList[ii].buffer, 
				(unsigned int)mbufList[ii].size);
		bufPtr += mbufList[ii].size;
		bufPtr[0] = '\n';
		bufPtr++;
		delete mbufList[ii].buffer;
	    }

	    XtFree((char *)mbufList);
	    XtFree((char *)pos_list);
	    break;

	case DtCR_DND_CONVERT_DELETE:
	    // The drag and drop succeeded, so we can now delete the messages
	    // from the scrolling list.
	    rmw->list()->deleteSelected();
	    break;
    }
}

// msgListDragFinishCallback
//
// Clean up from the convert callback and restore state
//
void
RoamMenuWindow::msgListDragFinishCallback(
	Widget 		/* widget */,
	XtPointer 	clientData,
	XtPointer 	callData)
{
    DtDndDragFinishCallbackStruct *finishInfo =
		(DtDndDragFinishCallbackStruct *) callData;
    DtDndContext 	*dragData = finishInfo->dragData;
    RoamMenuWindow 	*rmw = (RoamMenuWindow *) clientData;
    MsgScrollingList	*msgList = rmw->list();
    DtMailEnv 		mail_error;
    int			ii;

    DebugPrintf(3, "In RoamMenuWindow::msgListDragFinishCallback\n");

    mail_error.clear();

    // Re-enable drops on this message list
    if (rmw->mailbox()->mailBoxWritable(mail_error) == DTM_TRUE) {
	rmw->msgListDropEnable();
    }

    if (finishInfo->sourceIcon)
	XtDestroyWidget(finishInfo->sourceIcon);

    // Free any memory allocated for the drag.
    for (ii = 0; ii < dragData->numItems; ii++) {
	XtFree((char *)dragData->data.buffers[ii].bp);
    }

}

// getDragIcon
//
// Use the mailbox pixmap to create a drag icon
//
Widget
RoamMenuWindow::getDragIcon(
	Widget 		widget)
{
    if (_mbox_image && _mbox_mask) {
	return DtDndCreateSourceIcon(widget, _mbox_image, _mbox_mask);
    } else {
	return NULL;
    }
}

// msgListDragStart
//
// Translation start a drag from the msg list
//
XtActionProc
RoamMenuWindow::msgListDragStart(
	Widget		widget,
	XEvent		*event,
	String		* /* params */,
	Cardinal	* /* numParams */)
{
    static XtCallbackRec convertCBRec[] = { 
	{&RoamMenuWindow::msgListConvertCallback, NULL},{NULL, NULL} };
    static XtCallbackRec dragFinishCBRec[] = {
	{&RoamMenuWindow::msgListDragFinishCallback, NULL}, {NULL, NULL} };
    RoamMenuWindow 	*rmw = NULL;
    unsigned char	operations;
    Widget 		drag_icon;
    DtMailEnv 		mail_error;

    DebugPrintf(3, "In RoamMenuWindow::msgListProcessDrag\n");

    mail_error.clear();

    XtVaGetValues(widget, XmNuserData, &rmw, NULL);

    if (rmw == NULL)
	return NULL;

    drag_icon = rmw->getDragIcon(widget);
    //drag_icon = NULL;

    rmw->msgListDropDisable();

    // Choose the drag operations based on the writeability of the mailbox

    if (rmw->mailbox()->mailBoxWritable(mail_error) == DTM_TRUE) {
	// RW Folder.  Permit Copy and Move.
	operations = (unsigned char)(XmDROP_COPY | XmDROP_MOVE);
    } else {
	// RO Folder.  Permit only Copy drags.
	operations = (unsigned char)XmDROP_COPY;
    }

    // Start the drag

    convertCBRec[0].closure = (XtPointer) rmw;
    dragFinishCBRec[0].closure = (XtPointer) rmw;

    if (DtDndVaDragStart(widget, event, DtDND_BUFFER_TRANSFER, 1,
			operations, convertCBRec, dragFinishCBRec,
			DtNsourceIcon, drag_icon,
			NULL) == NULL) {
	DebugPrintf(3, "DragStart returned NULL.\n");
    }

    return NULL;
}

/* ARGSUSED */
Bool
RoamMenuWindow::msgListLookForButton (
        Display *,
        XEvent * event,
        XPointer arg)
{

#define DAMPING 5
#define ABS_DELTA(x1, x2) (x1 < x2 ? x2 - x1 : x1 - x2)

    if( event->type == MotionNotify)  {
        XEvent * press = (XEvent *) arg;

        if (ABS_DELTA(press->xbutton.x_root, event->xmotion.x_root) > DAMPING ||
            ABS_DELTA(press->xbutton.y_root, event->xmotion.y_root) > DAMPING)
            return(True);
    }
    else if (event->type == ButtonRelease)
        return(True);
    return(False);
}

// msgListProcessPress
//
// Translation implementing Motif 1.2.5 ProcessPress function
//

#define SELECTION_ACTION        0
#define TRANSFER_ACTION         1

void
RoamMenuWindow::msgListProcessPress(
	Widget		w,
	XEvent		*event,
	String		*params,
	Cardinal	*num_params)
{
   int i, action, cur_item;
   int *selected_positions, nselected_positions;

   DebugPrintf(3, "In RoamMenuWindow::msgListProcessPress\n");

   //  This action happens when Button1 is pressed and the Selection
   //  and Transfer are integrated on Button1.  It is passed two
   //  parameters: the action to call when the event is a selection,
   //  and the action to call when the event is a transfer.

    if (*num_params != 2 || !XmIsList(w))
      return;

    action = SELECTION_ACTION;
    cur_item = XmListYToPos(w, event->xbutton.y);

    if (cur_item > 0)
    {
        XtVaGetValues(w,
		XmNselectedPositions, &selected_positions,
		XmNselectedPositionCount, &nselected_positions,
		NULL);

	for (i=0; i<nselected_positions; i++)
	{
	    if (cur_item == selected_positions[i])
	    {
                // The determination of whether this is a transfer drag
		// cannot be made until a Motion event comes in.  It is
		// not a drag as soon as a ButtonUp event happens.
                XEvent new_event;
                
		XPeekIfEvent(
			XtDisplay(w),
			&new_event,
			&RoamMenuWindow::msgListLookForButton,
			(XPointer) event);
                switch (new_event.type)
                {
                    case MotionNotify:
      	               action = TRANSFER_ACTION;
                       break;
                    case ButtonRelease:
        	       action = SELECTION_ACTION;
                       break;
                }
		break;
	    }
	}
    }

    XtCallActionProc(w, params[action], event, params, *num_params);
}

// msgListDragSetup
//
// Override default list translations to start our own drags
//
void
RoamMenuWindow::msgListDragSetup()
{
    Widget msgList = _list->get_scrolling_list();
    static char	translations[] = "\
		~c ~s ~m ~a <Btn1Down>:\
		    dt-process-press(ListBeginSelect,MsgListDragStart)\n\
		c ~s ~m ~a <Btn1Down>:\
		    dt-process-press(ListBeginToggle,MsgListDragStart)";
    static char	btn2_translations[] = "\
		~c ~s ~m ~a <Btn2Down>:\
		    dt-process-press(ListBeginSelect,MsgListDragStart)\n\
		c ~s ~m ~a <Btn2Down>:\
		    dt-process-press(ListBeginToggle,MsgListDragStart)\n\
		<Btn2Motion>:ListButtonMotion()\n\
		~c ~s ~m ~a <Btn2Up>:ListEndSelect()\n\
		c ~s ~m ~a <Btn2Up>:ListEndToggle()";
    int		btn1_transfer = 0;
    XtTranslations	new_translations;
    static XtActionsRec	actionTable[] = {
	{"MsgListDragStart", 
	(XtActionProc) &RoamMenuWindow::msgListDragStart},
	{"dt-process-press", 
	(XtActionProc) &RoamMenuWindow::msgListProcessPress}
    };

    DebugPrintf(3, "In RoamMenuWindow::msgListDragSetup\n");

    XtAppAddActions(
		theRoamApp.appContext(),
		actionTable,
		sizeof(actionTable)/sizeof(actionTable[0]));
    new_translations = XtParseTranslationTable(translations);
    XtOverrideTranslations(msgList, new_translations);

    XtVaGetValues(
	(Widget)XmGetXmDisplay(XtDisplayOfObject(msgList)),
	"enableBtn1Transfer", &btn1_transfer,
	NULL);
    
    if (btn1_transfer != True) {
	new_translations = XtParseTranslationTable(btn2_translations);
	XtOverrideTranslations(msgList, new_translations);
    }
}

void
RoamMenuWindow::open_and_load(
    DtMailEnv &error,
    DtMailBoolean create,
    DtMailBoolean lock)
{
    _openContainerCmd->set_create_lock_flags(create, lock);
    _openContainerCmd->execute();

    // if it required conversion, let conversion handle the error
    // (its a callback routine and we have "lost control" of what
    // it returns anyway.)
    // 
    // if it did not require conversion, then open() should have succeeded.
    // If it did succeed, then its _mailbox should be set by now.
    // If its not set, then either open() failed somewhere or it was
    // cancelled by the user.  We now have an error condition.
    if ((!_required_conversion) && (_mailbox == NULL))
    {
	error.setError(DTME_NoMailBox);
	return;
    }

    // If it required conversion, then let the conversion process handle
    // errors if any occur.
    // Reset the DtMailEnv for this method.
    if (_required_conversion) error.clear();

    // If it required conversion, and conversion succeeded, then the
    // mailbox was also loaded and _is_loaded gets set to TRUE.
    // If it required no conversion, then it implies that we already
    // have a container in hand and we just load it (implicitly
    // setting _is_loaded to TRUE)
    if ((!_required_conversion) && (_is_loaded == FALSE))
    {
	this->load_mailbox(error);
	_is_loaded = TRUE;
    }
}


// Ideally, open() should set the error if the user cancels the open.  
// And we should error after open() returns at the caller's end...

void
RoamMenuWindow::open(
    DtMailEnv & error, 
    DtMailBoolean create_flag, 
    DtMailBoolean lock_flag
)
{
    FORCE_SEGV_DECL(char, tmp);
    Dimension win_x, win_y, win_wid, win_ht, win_bwid;
    MailSession *ses = theRoamApp.session();
    int answer = 0;
    char *helpId;

    char *buf = new char[2*MAXPATHLEN];

    _openContainerCmd->set_create_lock_flags(create_flag, lock_flag);

    // Obtain the current dimensions of the RMW
    XtVaGetValues(_main,
		  XmNx, &win_x,
		  XmNy, &win_y,
		  XmNwidth, &win_wid,
		  XmNheight, &win_ht,
		  XmNborderWidth, &win_bwid,
		  NULL);
    _x = win_x;
    _y = win_y;
    _width = win_wid;
    _height = win_ht;
    _border_width = win_bwid;

    // Check to see if the mbox is already open.  If it is, we will
    // simply make sure it's displayed in the current workspace.
    if (ses->isMboxOpen(_mailbox_fullpath))
    {
	RoamMenuWindow *rmw = NULL;
	rmw = ses->getRMW(_mailbox_fullpath);
	if (NULL != rmw)
	{
	    ses->activateRMW(rmw);
	    rmw->displayInCurrentWorkspace();
	}
	return;
    }

    // Try to open this folder, but don't take the lock and and don't create it.
    _mailbox = ses->open(error, _mailbox_fullpath, 
			 &RoamMenuWindow::syncViewAndStoreCallback,
			 this, create_flag, lock_flag);
    if (error.isSet())
    {
        if((DTMailError_t) error == DTME_OutOfSpace )
        {
              ShowErrMsg((char *)error.getClient(),FALSE,(void*)this );
              error.setClient(NULL);
    	      delete [] buf;
	      return;
        }
	
	// Did we ask for a non-existent file?
	if ((DTMailError_t)error == DTME_NoSuchFile)
	{
	    // if (create_flag == DTM_TRUE)
            if (create_flag == DTM_FALSE)
	    {
		sprintf(buf,
			GETMSG(DT_catd, 
				3, 3, 
				"The mailbox %s does not exist.\nCreate a mailbox with this name?"),
			_mailbox_fullpath);
		_genDialog->setToQuestionDialog(GETMSG(DT_catd, 
							3, 4, "Mailer"),
					     buf);
		helpId = DTMAILHELPERROR;
		answer = _genDialog->post_and_return(GETMSG(DT_catd, 
							     3, 5, 
							     "OK"), 
						     GETMSG(DT_catd,
							     1, 5,
							     "Cancel"),
						     helpId);

		if (answer == 1) open(error, DTM_TRUE, lock_flag);
		if (answer == 2) {
    		    delete [] buf;
		    return;
		}
	    }
	    else {
                // A special case should be taken care
                // The create_flag is TRUE but still can not be created
                //      This is because the path is something like
                //      /valid_path/not_such_dir/file_name.
                //     The full file path is not valid so that we need flag
                //     an error
                sprintf(buf,
                        GETMSG(DT_catd,
                                3, 44,
                                "Unable to create %s."),
                        _mailbox_fullpath);
                _genDialog->setToQuestionDialog(GETMSG(DT_catd,
                                                        3, 4, "Mailer"),
                                             buf);
                helpId =  DTMAILHELPNOCREATE;
                answer = _genDialog->post_and_return(GETMSG(DT_catd,
                                                             3, 5,
                                                             "OK"),
                                                     helpId);
    	       delete [] buf;
               return;
	    }
	}
	else if (((DTMailError_t)error == DTME_OtherOwnsWrite) ||
	         ((DTMailError_t)error == DTME_AlreadyLocked))
	{
	    // See if they want to take the lock.
// 	    sprintf(buf, 
// 		    GETMSG(DT_catd, 3, 6, "The mailbox %s is locked.\n\
// You can manually unlock the mailbox and try again\n\
// or contact your System Administrator."),
// 		   _mailbox_fullpath);

// 	    _genDialog->setToErrorDialog(
// 				GETMSG(DT_catd, 3, 7, "Mailer"),
// 				buf);
// 	    helpId = DTMAILHELPTAKELOCK;
// 	    _genDialog->post_and_return(
// 				GETMSG(DT_catd, 3, 8, "OK"),
// 				helpId);

// 	    error.setError(DTME_GetLockRefused);
	    // The above else-if code doesn't make sense.  If ses->open() failed
	    // because the folder was already locked or not writable then 
	    // posting the specified error dialog and setting the error to GetLockRefused
	    // is meaningless.  Especially since the calling function 
	    // OpenContainerCmd::doit() doesn't even check the error we 
	    // return.  Lets post a meaningful error dialog and return.
	    postErrorDialog(error);
            delete [] buf;
	    return;
	}
	else if ((DTMailError_t)error == DTME_BadRunGroup)
	{

	    sprintf(buf, "%s", GETMSG(DT_catd, 2, 4,
		   "Mailer has not been properly installed,\n\
and cannot run because the execution group\n\
is incorrectly set."));

	    _genDialog->setToQuestionDialog(
				GETMSG(DT_catd, 1, 6, "Mailer"),
				buf);
	    
	    // No choice at this state other than to OK.

	    helpId = DTMAILHELPBADGROUPID;
	    answer = _genDialog->post_and_return(
				GETMSG(DT_catd, 3, 9, "OK"),
				helpId);
            delete [] buf;
	    return;

	}
	else if ((DTMailError_t)error == DTME_NoPermission)
	{
	    /* NL_COMMENT
	     * The %s is the name of the mailbox the user doesn't have
	     * permission to view.
	     */
	    sprintf(buf, GETMSG(DT_catd, 2, 5,
		   "You do not have permission to view %s"), _mailbox_fullpath);

	    _genDialog->setToQuestionDialog(
				GETMSG(DT_catd, 1, 7, "Mailer"),
				buf);
	    
	    // No choice at this state other than to OK.

	    helpId = DTMAILHELPNOVIEW;
	    answer = _genDialog->post_and_return(
				GETMSG(DT_catd, 3, 10, "OK"),
				helpId);
    	    delete [] buf;
	    return;
	}
	else if ((DTMailError_t)error == DTME_IsDirectory)
	{
	    sprintf(buf, GETMSG(DT_catd, 2, 6,
		    "The mailbox %s is a directory and can not be opened."),
		    _mailbox_fullpath);

	    _genDialog->setToQuestionDialog(GETMSG(DT_catd, 1, 8, "Mailer"),
					    buf);

	    helpId = DTMAILHELPDIRECTORYONLY;
	    answer = _genDialog->post_and_return(GETMSG(DT_catd, 3, 11, "OK"),
						 helpId);
    	    delete [] buf;
	    return;
	}
	else if ((DTMailError_t)error == DTME_AlreadyOpened)
	{
           sprintf(buf, GETMSG(DT_catd,20,1,
                   "The mailbox %s is already open."), _mailbox_fullpath);
 
           _genDialog->setToQuestionDialog(GETMSG(DT_catd, 1, 8, "Mailer"),
                                           buf);
 
           // there is no help message for this error
           // open a defect and put helpId later
           helpId = NULL;
           answer = _genDialog->post_and_return(GETMSG(DT_catd, 3, 11, "OK"),
                                                helpId);
           delete [] buf;
           return;
	}
	else
	{
	    // This is a real error. Punt!
	    this->postErrorDialog(error);
	}
    }

    if(_mailbox)
    {
        if (_mailbox->mailBoxWritable(error) == DTM_TRUE)
	  msgListDropRegister();
        _mailbox->registerErrMsgFunc(ShowErrMsg,this);
        if (isIconified())
	  _mailbox->hideAccessEvents(DTM_TRUE);
        else
	  _mailbox->hideAccessEvents(DTM_FALSE);
    }
    delete [] buf;
    return;
}

void RoamMenuWindow::createMenuPanes()
{
    this->createContainerList();

    this->construct_file_menu();

    this->construct_message_menu();

    this->construct_edit_menu();

    this->construct_attachment_menu();

    this->construct_view_menu();

    this->construct_compose_menu();

    this->construct_move_menu();

    this->construct_help_menu();

    this->construct_message_popup();

    this->construct_text_popup();

    this->construct_attachment_popup();

    this->syncCachedContainerList();

}

void
RoamMenuWindow::resetCacheList(int new_size)
{
    // Remove the extra items in the list
    for (int i = _cached_containerlist->length(); i > new_size; i--) {

	_cached_containerlist->remove(i-1);
	_open_container_containerlist_cached->remove(i-1);
	_copyto_containerlist_cached->remove(i-1);
	_move_containerlist_cached->remove(i-1);

	_menuBar->removeCommand(_opencontainerMenu, _first_cached_item+i-1);
	_menuBar->removeCommand(_copytoMenu, _first_cached_item+i-1);
	_menuBar->removeCommand(_moveMenu, _first_cached_item+i-1);
	_menuBar->removeCommand(_msgsPopupMoveMenu, _first_cached_item+i-1);
    }
}

void
RoamMenuWindow::propsChanged(void)
{
    MailSession		*ses = theRoamApp.session();
    DtMail::Session	*d_session = ses->session();
    DtMail::MailRc 	*mailrc = get_mail_rc();
    DtMailEnv		 error;
    const char		*value = NULL;
    char		*inbox_path = NULL;
    DtMailObjectSpace	 space;
    Boolean	 	 should_be_inbox;

    if (! _is_loaded) return;
    enableWorkAreaResize();

    d_session->queryImpl(
			error,
			d_session->getDefaultImpl(error),
			DtMailCapabilityInboxName,
			&space,
			&inbox_path);
    should_be_inbox = (inbox_path && 0 == strcmp(inbox_path, mailboxName()));

    RoamMenuWindow	*rmw = NULL;
    char		*password = NULL;
    password = MailRetrievalOptions::getPassword(DTMAS_INBOX);

    if (_inbox != should_be_inbox)
    {

	if (_inbox)
	{
	    char buf[2048];
	    int answer;

	    //
	    // Remove inbox attributes
	    //
	    displayInCurrentWorkspace();
	    sprintf(
		buf, "%s",
		GETMSG(DT_catd, 99, 99,"The INBOX path has changed.\nReopen?"));
	    _genDialog->setToQuestionDialog(
					GETMSG(DT_catd, 3, 22, "Mailer"),
					buf);
	    answer = _genDialog->post_and_return(
					GETMSG(DT_catd, 3, 29, "OK"),
					GETMSG(DT_catd, 3, 19, "Cancel"),
					DTMAILHELPERROR);

	    rmw = ses->getRMW(inbox_path);
	    if (rmw)
	    {
	        rmw->_inbox = TRUE;
	        rmw->_mailbox->createMailRetrievalAgent(password);
	        rmw->_open_container_inbox->deactivate();
	        rmw->_file_open_inbox->deactivate();
	    }

	    if (answer == 1)
	    {
	        if (NULL != rmw)
		  rmw->displayInCurrentWorkspace();
		else
	          view_mail_file(inbox_path, DTM_FALSE);
	    }

	    _inbox = FALSE;
	    _mailbox->deleteMailRetrievalAgent();
	    _open_container_inbox->activate();
	    _file_open_inbox->activate();
	    
	}
	else
	{
	    _inbox = TRUE;
    	    _mailbox->createMailRetrievalAgent(password);
	    _open_container_inbox->deactivate();
	    _file_open_inbox->deactivate();
	}
    }
    else
    {
	if (_inbox) _mailbox->createMailRetrievalAgent(password);
	else
	{
	    rmw = ses->getRMW(inbox_path);
	    if (rmw) rmw->_mailbox->createMailRetrievalAgent(password);
	}
    }

    if (password) free(password);


    // See if the header size has changed.
    //
    mailrc->getValue(error, "headerlines", &value);
    if (error.isSet())
    { 
	value = strdup("15");
	error.clear();
    }

    int header_lines = (int) strtol(value, NULL, 10);
    if (header_lines != _list->visibleItems())
	_list->visibleItems(header_lines);

    if (NULL != value)
      free((void*) value);

    _list->checkDisplayProp();
    _my_editor->textEditor()->update_display_from_props();

    value = NULL;
    mailrc->getValue(error, "dontdisplaycachedfiles", &value);
    if (error.isNotSet() && value != NULL)
    {
        if (_display_cached_list)
	  // They just turned off the Display Up To prop so
	  //reset the cache list to zero length.
	  resetCacheList(0);

	_max_cached_list_size = 0;
	_display_cached_list = FALSE;
	free((void*) value);
    }
    else
    {
        error.clear();
	value = NULL;
        mailrc->getValue(error, "cachedfilemenusize", &value);
        if (error.isNotSet() && value != NULL && *value != '\0')
	{
	    int new_size;

	    new_size = (int) strtol(value, NULL, 10);
	    if (new_size != _max_cached_list_size && new_size >=0)
	    {
		// They just changed the display number so chop the 
		// list if it is bigger than the new size just set 
		if (new_size < _max_cached_list_size)
		  resetCacheList(new_size);

		_max_cached_list_size = new_size;
	    }
	}
	if (NULL != value)
	  free((void*) value);
    }

    error.clear(); 
    value = NULL;
    mailrc->getValue(error, "filemenu2", &value);
    if  ( (value == NULL && _filemenu2 != NULL) ||
	  (value != NULL && _filemenu2 == NULL) ||
	  (value != NULL && _filemenu2 != NULL &&
	   strcmp(value, _filemenu2) != 0) )
    {
	// Recreate the containter list
	this->createContainerList();

	// Recreate the File menu
	this->construct_file_menu();	

	// Recreate the Message menu
	this->construct_message_menu();	

	// Recreate the Move menu...
	this->construct_move_menu();	

	// Recreate the Message popup menu
	this->construct_message_popup();	
    }
    if (NULL != value)
      free((void*) value);

    value = NULL;
    mailrc->getValue(error, "retrieveinterval", &value);
    if (error.isNotSet() && value && *value != '\0')
    {
	long ping = (time_t) strtol(value, NULL, 10);
	if (ping <= 0)
	  _checkformail_when_mapped = FALSE;
	else
	  _checkformail_when_mapped = TRUE;
    }
    error.clear();
    if (NULL != value) free((void*) value);

    disableWorkAreaResize();
}

void RoamMenuWindow::registerDialog( ViewMsgDialog *dialog )
{
    int i;
    ViewMsgDialog **newList = (ViewMsgDialog **)-1;
  
    // Allocate a new list large enough to hold the new
    // object, and copy the contents of the current list 
    // to the new list
  
    newList = new ViewMsgDialog*[_numDialogs + 1];
  
    for ( i = 0; i < _numDialogs; i++ )
	newList[i] = _dialogs[i];
  
    // Install the new list and add the window to the list
  
    if ( _numDialogs > 0 )
	delete []_dialogs;
    _dialogs =  newList;
    _dialogs[_numDialogs] = dialog;
    _numDialogs++;

  }
void RoamMenuWindow::unregisterDialog ( ViewMsgDialog *dialog )
{
  int i, index;
  ViewMsgDialog **newList = (ViewMsgDialog **)-1;
  
  // Allocate a new, smaller list
  
  newList = new ViewMsgDialog *[_numDialogs - 1];
  
  // Copy all objects, except the one to be 
  // removed, to the new list
  
  index = 0;
  for ( i = 0; i < _numDialogs; i++ )
    if ( _dialogs[i] != dialog )
    newList[index++] = _dialogs[i];
  
  // Install the new list
  
  delete []_dialogs;
  _dialogs =  newList;
  
  _numDialogs--;
}

void
RoamMenuWindow::forwardFilename( char *file )
{
  _forward_filename = (char *)realloc(_forward_filename, strlen(file)+1);
  strcpy( _forward_filename, file );
}

char *
RoamMenuWindow::forwardFilename()
{
  return _forward_filename;
}



// If it is a configure notify, we are interested in it.
// We need to then capture its new position.
// And if the RMW has not been loaded, we need to load it.

void
RoamMenuWindow::structurenotify(
    Widget,		// w
    XtPointer clientData,
    XEvent *event,
    Boolean * )		// continue_to_dispatch
{
   RoamMenuWindow *rmw=(RoamMenuWindow *) clientData;

   if (event->type == ConfigureNotify)
   {
       rmw->configurenotify(
			event->xconfigurerequest.x,
			event->xconfigurerequest.y,
			event->xconfigurerequest.width,
			event->xconfigurerequest.height,
			event->xconfigurerequest.border_width);
   }
   else if ((event->type == MapNotify) || ( event->type == UnmapNotify))
   {
       if (rmw->_mailbox)
         if (event->type == UnmapNotify)
	 {
	     rmw->_mailbox->save();
	     rmw->_mailbox->hideAccessEvents(DTM_TRUE);
	 }
	 else
	 {
	     rmw->_mailbox->hideAccessEvents(DTM_FALSE);
	     if (FALSE==rmw->_inbox || TRUE==rmw->_checkformail_when_mapped)
	     {
                 DtMailEnv        error;
                 DtMail::Session *m_session = theRoamApp.session()->session(); 
                 DtMail::MailRc  *mailrc = m_session->mailRc(error);
    		 const char      *value = NULL;

    		 mailrc->getValue(error, "retrievemailonmapnotify", &value);
    		 if (error.isNotSet()) rmw->checkForMail(error);
             }
         }

       rmw->mapnotify();
   }       
}

// Capture its position coordinates.

void 
RoamMenuWindow::configurenotify( 
    unsigned int win_x, unsigned int win_y,
    unsigned int win_wid, unsigned int win_ht,
    unsigned int win_bwid
)
{

    _x = win_x;
    _y = win_y;
    _width = win_wid;
    _height = win_ht;
    _border_width = win_bwid;

}

// If it is not already loaded, then load it (it might involve
// conversion, etc.; all handled by open_and_load())
// If its been loaded already, then mapnotify gets called when
// the state changes from iconic to open (i.e., the user double-clicks
// on an RMW icon).  If we want to load a folder at that time, this
// is the place to do it.


void
RoamMenuWindow::mapnotify()
{
    // If its not been loaded, then open and load it.
    if (!_is_loaded)
    {
	DtMailEnv mail_error;
	
	mail_error.clear();
	
	theRoamApp.busyAllWindows();
	this->open_and_load(
			mail_error,
			(DtMailBoolean) _create_mailbox_file,
			DTM_TRUE);
	theRoamApp.unbusyAllWindows();
	
	// If there's been an error then we quit the container.
 	if (mail_error.isSet())
	{
	    // Need to remove the base Widgets destroy callback since
	    // we end up destroying it twice otherwise...
	    XtRemoveAllCallbacks(this->baseWidget(), XmNdestroyCallback);
	    this->quit(TRUE);
 	}
	else {
	    // We need to disable the editable menu options if the mail
	    // box is readonly.
	    //
	    if (_mailbox->mailBoxWritable(mail_error) == DTM_FALSE)
	    {
		_msg_delete->deactivate();
		_msg_undelete_last->deactivate();
		_msg_undelete_from_list->deactivate();
		_delete_button->deactivate();
		_file_destroy_deleted_msgs->deactivate();
		if (NULL != _move_cascade)
		  XtSetSensitive(_move_cascade, FALSE);
		if (NULL != _msgsPopupMoveMenu)
		  XtSetSensitive(_msgsPopupMoveMenu, FALSE);
		
		char * readonly = GETMSG(DT_catd, 20, 3, "Read Only");
		setTitle(readonly);
	    }

	    if (_list->get_num_messages())
	      setIconName(NormalIcon);
	    else
	      setIconName(EmptyIcon);
	}
    }
    else
    {
	// If the mailbox has messages, set to normal icon
	if (_list->get_num_messages() > 0)
	  setIconName(NormalIcon);
	else
	  setIconName(EmptyIcon);
    }
}

void
RoamMenuWindow::last_sorted_by(SortBy type)
{
    char	 	 id[16];
    char		*buffer = NULL;
    DtMailEnv		 error;
    DtMail::Session	*m_session = theRoamApp.session()->session(); 
    DtMail::MailRc 	*mailrc = m_session->mailRc(error);

    _last_sorted_by = type;

    ((SortCmd*)_view_sortTD)->setButtonState(FALSE, FALSE);
    ((SortCmd*)_view_sortSender)->setButtonState(FALSE, FALSE);
    ((SortCmd*)_view_sortSubject)->setButtonState(FALSE, FALSE);
    ((SortCmd*)_view_sortSize)->setButtonState(FALSE, FALSE);
    ((SortCmd*)_view_sortStatus)->setButtonState(FALSE, FALSE);

    XtVaSetValues(_sender_lbl, XmNlabelString, _sender_xms, NULL);
    XtVaSetValues(_subject_lbl, XmNlabelString, _subject_xms, NULL);
    XtVaSetValues(_date_lbl, XmNlabelString, _date_xms, NULL);
    XtVaSetValues(_size_lbl, XmNlabelString, _size_xms, NULL);

    buffer = XtMalloc(BUFSIZ);
    if (buffer)
    {
	if (_inbox)
	{
    	    RMW_CONCAT_MAILRC_KEY(buffer, DTMAS_INBOX, "sortby");
	}
	else
	{
    	    RMW_CONCAT_MAILRC_KEY(buffer, mailboxName(), "sortby");
	}
    }
    
    switch (_last_sorted_by)
    {
	case SortTimeDate:
	    XtVaSetValues(_date_lbl, XmNlabelString, _date_key_xms, NULL);
	    ((SortCmd*)_view_sortTD)->setButtonState(TRUE,FALSE);
	    if (buffer) mailrc->removeValue(error, buffer);
	    break;
        case SortSender:
	    XtVaSetValues(_sender_lbl, XmNlabelString, _sender_key_xms, NULL);
	    ((SortCmd*)_view_sortSender)->setButtonState(TRUE,FALSE);
	    sprintf(id, "%d", SortSender);
	    if (buffer) mailrc->setValue(error, buffer, id);
	    break;
        case SortSubject:
	    XtVaSetValues(_subject_lbl, XmNlabelString, _subject_key_xms, NULL);
	    ((SortCmd*)_view_sortSubject)->setButtonState(TRUE,FALSE);
	    sprintf(id, "%d", SortSubject);
	    if (buffer) mailrc->setValue(error, buffer, id);
	    break;
        case SortSize:	
	    XtVaSetValues(_size_lbl, XmNlabelString, _size_key_xms, NULL);
	    ((SortCmd*)_view_sortSize)->setButtonState(TRUE,FALSE);
	    sprintf(id, "%d", SortSize);
	    if (buffer) mailrc->setValue(error, buffer, id);
	    break;
        case SortStatus:
	    ((SortCmd*)_view_sortStatus)->setButtonState(TRUE,FALSE);
	    sprintf(id, "%d", SortStatus);
	    if (buffer) mailrc->setValue(error, buffer, id);
	    break;
        default:
	    XtVaSetValues(_date_lbl, XmNlabelString, _date_key_xms, NULL);
	    ((SortCmd*)_view_sortTD)->setButtonState(TRUE,FALSE);
	    if (buffer) mailrc->removeValue(error, buffer);
	    break;
    }
    if (buffer) XtFree(buffer);

    //_list->layoutLabels();
    mailrc->update(error);
}

void
RoamMenuWindow::message( char *text )
{
    int text_size = strlen(text);
    char *buf, *str;
    XmString labelStr;

    if (text_size > 0) {
	str = GETMSG(DT_catd, 3, 12, "%s");
	buf = new char[strlen(str) + text_size + 1];
	sprintf(buf, str, text);
	labelStr = XmStringCreateLocalized(buf);
	_clear_message_p = TRUE;	       
    }
    else {
	buf = new char[2];
	sprintf(buf, "%s", "");
	labelStr = XmStringCreateLocalized(buf);
	_clear_message_p = FALSE;
    }
    
    XtVaSetValues(_message, XmNlabelString, labelStr, NULL);
    XmUpdateDisplay(this->baseWidget());

    XmStringFree(labelStr); 
    delete [] buf;
}

void
RoamMenuWindow::setStatus(const char * msg)
{
    message((char *)msg);
}

void
RoamMenuWindow::clearStatus(void)
{
    setStatus("");
}

void
RoamMenuWindow::message_summary()
{
    this->message_summary(
		list()->selected_item_position(),
		list()->get_num_messages(), 
		list()->get_num_new_messages(),
		list()->get_num_deleted_messages());
}

void
RoamMenuWindow::message_summary(
    int sel_pos,
    int num_msgs,
    int num_new,
    int num_deleted
)
{
    char *buf, *str; 
    XmString labelStr;
    int num_live_msgs = num_msgs - num_deleted;  // Undeleted msgs
    DtMail::MailRc * mailrc = get_mail_rc();
    DtMailEnv error;
    const char * value = NULL;

    mailrc->getValue(error, "nerdmode", &value);
    if (error.isSet()) {
	str = GETMSG(DT_catd, 3, 13, "Message %d of %d, %d new, %d deleted"); 
    }
    else {
	str = "Message 0x%x of 0x%x, ignoring 0x%x, 0x%x forgotten";
    }
    if (NULL != value)
      free((void*) value);

    buf = new char[strlen(str) + 100];
    sprintf(buf, str, sel_pos, num_msgs, num_new, num_deleted);

    labelStr = XmStringCreateLocalized(buf);
    
    XtVaSetValues(_message_summary, XmNlabelString, labelStr, NULL);
    XmUpdateDisplay(this->baseWidget());

    XmStringFree(labelStr);
    delete [] buf;
}    

void
RoamMenuWindow::message_selected(
    int msg_num,
    int num_msgs,
    int num_new,
    int num_deleted
)
{
    char *buf, *str;
    XmString labelStr;

	/* NL_COMMENT
	 * The user will see the following message display as:
	 * "Message 3 of 10, 2 new, 6 deleted"
	 * This means ??? -- Explain to translator.
	 */
    str = GETMSG(DT_catd, 3, 14, "Message %d of %d, %d new, %d deleted"); 
    buf = new char[strlen(str) + 20];
    sprintf(buf, str, msg_num, num_msgs, num_new, num_deleted);
    
    labelStr = XmStringCreateLocalized(buf);

    XtVaSetValues(_message_summary, XmNlabelString, labelStr, NULL);
    XmUpdateDisplay(this->baseWidget());

    XmStringFree(labelStr);
    delete [] buf;
}
    

FindDialog *
RoamMenuWindow::get_find_dialog()
{
  
  if (!_findDialog) {
    theRoamApp.busyAllWindows();
    // No find dialog.  Create it
    _findDialog = new FindDialog(this);
    _findDialog->initialize();
    theRoamApp.unbusyAllWindows();
  }

  // Show it
  _findDialog->manage();
  _findDialog->popup();
  return _findDialog;
}

Boolean 
RoamMenuWindow::quitWorkproc(XtPointer client_data)
{
    RoamMenuWindow	*rmw = (RoamMenuWindow *) client_data;
    MailSession		*ses = theRoamApp.session();
    static int called = 0;

    if (rmw->_numPendingTasks > 0)
    {
        rmw->busyCursor();
        return FALSE;
    }
    
    if (rmw->_quitWorkprocID != 0)
    {
	XtRemoveWorkProc(rmw->_quitWorkprocID);
	rmw->_quitWorkprocID = 0;
    }

    rmw->normalCursor();
    rmw->_genDialog->unmanage();
    rmw->unmanage();

    if (rmw->_delete_on_quit)
    {
        rmw->_list->shutdown();
        delete rmw;
    }
    else
      ses->deactivateRMW(rmw);

    theRoamApp.checkForShutdown();
    return TRUE;
}

int 
RoamMenuWindow::queryExpunge()
{
    int answer = 0;
    DtMailEnv error;
    int i = 0;

    if (NULL != _mailbox && _mailbox->mailBoxWritable(error) == DTM_TRUE)
    {
      theRoamApp.busyAllWindows(GETMSG(DT_catd, 3, 15, "Saving..."));

      if (_list->get_num_deleted_messages())
      {
	// We need to deal with deleted messages, based on what the
	// user wants to do. There are two properties that control
	// this. They are:
	//
	// keepdeleted - Keep deleted messages on close.
	// quietdelete - Delete without asking.
	//
	// If the user wants to keep the deleted messages, then we
	// can just blow by the second. If not, then we have to
	// clear the deleted messages, asking first based on the
	// second option.
	//
	DtMail::MailRc * mailrc = get_mail_rc();
	const char * value = NULL;

	error.clear();
	mailrc->getValue(error, "keepdeleted", &value);
	if (error.isSet())
	{
	    if (NULL != value)
	      free((void*) value);

	    // The user wants to expunge on close. See if they want
	    // to be asked first.
	    //
	    value = NULL;
	    error.clear();
	    mailrc->getValue(error, "quietdelete", &value);
	    if (error.isSet() && !theRoamApp.quitSilently())
	    {
		if (isIconified())
		  manage();

		/* NL_COMMENT
		 * This dialog comes up when the user tries to quit the
		 * mailbox and the user is asked if he wants to destroy
		 * the messages marked for deletion.
		 */
		/* NL_COMMENT
		 * Messages 16 and 17 are no longer being used.  They are
		 * being replaced by message 86 and 87.
		 */
		_genDialog->setToQuestionDialog(
#ifdef undef
		    GETMSG(DT_catd, 3, 16, "Mailer"),
		    GETMSG(DT_catd, 3, 17, "Destroy the messages you have marked\nfor deletion in this mailbox?"));
#endif // undef
		/* NL_COMMENT
		 * This dialog comes up when the user tries to quit the
		 * mailbox.  The user is asked if they want to destroy
		 * the deleted messages.
		 */
		    GETMSG(DT_catd, 3, 87, "Mailer - Close"),
		    GETMSG(DT_catd, 3, 88, "Destroy the deleted messages and close this mailbox?"));
		char * helpId = DTMAILHELPDESTROYMARKMSG;
		int answer = _genDialog->post_and_return(
				GETMSG(DT_catd, 3, 89, "Destroy and Close"),
				GETMSG(DT_catd, 3, 73, "Cancel"),
				GETMSG(DT_catd, 3, 90, "Retain and Close"),
				helpId);
		if (answer == 1)
		{
		    error.clear();
		    _mailbox->expunge(error);
    		    if ((DTMailError_t) error == DTME_OutOfSpace)
		    {
       		        ShowErrMsg(
				(char *)error.getClient(),
				FALSE,
				(void*)this);
                        error.setClient(NULL);
    		    }

	            if (_msg_undelete_from_list->dialog())
	              _msg_undelete_from_list->dialog()->expunge();
        	    _list->expunge();

		    if (error.isSet())
		      postErrorDialog(error);
		}
		else if (answer == 2)
		{
		    // This is a very bad way to code selection of the 
		    // cancel button.  If someone changes its position
		    // in the dialog, this code will break!
		    theRoamApp.unbusyAllWindows();
		    return 0;
		}
	    }
	    else
	    {
		// If killed by a signal, don't post a dialog.
		error.clear();
		_mailbox->expunge(error);
                if ((DTMailError_t) error == DTME_OutOfSpace)
		{
       		   ShowErrMsg((char *)error.getClient(),FALSE,(void*)this );
                   error.setClient(NULL);
    	        }

	        if (_msg_undelete_from_list->dialog())
	          _msg_undelete_from_list->dialog()->expunge();
        	_list->expunge();

		if (error.isSet())
		  if (! theRoamApp.quitSilently())
		    postErrorDialog(error);
	    }
	}
	if (NULL != value)
	  free((void*) value);
      }
    }
    return 1;
}


void 
RoamMenuWindow::quit(Boolean delete_win)
{
    DtMailEnv error;
    int i = 0;

    if (! queryExpunge())
      return;

    for (i = 0; i < _numDialogs; i++)
      _dialogs[i]->unmanage();

    XmUpdateDisplay(baseWidget());

    for (i = 0; i < _numDialogs; i++)
      _dialogs[i]->quit();

    theRoamApp.unbusyAllWindows();

    this->_delete_on_quit = delete_win;
    if (_numPendingTasks > 0)
    {
	char	*msg;

        busyCursor();
	msg = GETMSG(
		DT_catd,
		21, 22, "Close pending:  waiting for task to terminate ...");
	setStatus(msg);

        if (_quitWorkprocID == 0)
          _quitWorkprocID = XtAppAddWorkProc(
                                        XtWidgetToApplicationContext(_w),
                                        &RoamMenuWindow::quitWorkproc,
                                        (XtPointer) this);

        unmanage();
    }
    else
      quitWorkproc((XtPointer) this);
}

void 
RoamMenuWindow::panicQuit()
{
    if (_mailbox != NULL)
    	_mailbox->unlock();
}

void 
RoamMenuWindow::quit_silently()
{
    DtMailEnv error;
    int i = 0;

    if (_mailbox->mailBoxWritable(error) == DTM_TRUE) {

      theRoamApp.busyAllWindows(GETMSG(DT_catd, 3, 15, "Saving..."));

      if (_list->get_num_deleted_messages()) {
	// We need to deal with deleted messages, based on what the
	// user wants to do. There are two properties that control
	// this. They are:
	//
	// keepdeleted - Keep deleted messages on close.
	// quietdelete - Delete without asking.
	//
	// If the user wants to keep the deleted messages, then we
	// can just blow by the second. If not, then we have to
	// clear the deleted messages, asking first based on the
	// second option.
	//
	DtMail::MailRc * mailrc = get_mail_rc();
	const char * value = NULL;

	error.clear();
	mailrc->getValue(error, "keepdeleted", &value);
	if (error.isSet()) {
	    error.clear();
	    _mailbox->expunge(error);
	    if (error.isSet()) {
		this->postErrorDialog(error);
	    }
 	}
        if (NULL != value)
          free((void*) value);

      }
    }

    for (i = 0; i < _numDialogs; i++) {
	_dialogs[i]->unmanage();
    }

    XmUpdateDisplay(this->baseWidget());

    for (i = 0; i < _numDialogs; i++) {
	_dialogs[i]->quit();
    }

    this->_delete_on_quit = FALSE;
    if (_numPendingTasks > 0)
    {
	char	*msg;

        busyCursor();
	msg = GETMSG(
		DT_catd,
		21, 22, "Close pending:  waiting for task to terminate ...");
	setStatus(msg);

        if (_quitWorkprocID == 0)
          _quitWorkprocID = XtAppAddWorkProc(
                                        XtWidgetToApplicationContext(_w),
                                        &RoamMenuWindow::quitWorkproc,
                                        (XtPointer) this);

        unmanage();
    }
    else
      quitWorkproc((XtPointer) this);
}

// Callback to open a new mail container.

void
RoamMenuWindow::file_selection_callback(void *client_data, char *selection)
{
    if (NULL == selection || 0 == strlen(selection)) return;

    RoamMenuWindow *obj=(RoamMenuWindow *) client_data;
    obj->view_mail_file(selection, DTM_FALSE);
    XtFree(selection);
}


void RoamMenuWindow::reopen_mail_file()
{
    char *filename;

    theApplication->disableShutdown();

    filename = strdup(this->_mailbox_fullpath);
    this->unmanage();
    this->quit(TRUE);
    view_mail_file(filename, DTM_FALSE);
    free(filename);

    theApplication->enableShutdown();
}

// Given the name of a container, create a new RoamMenuWindow
// and open the container into it.

void RoamMenuWindow::view_mail_file(char *filename, DtMailBoolean create)
{
    DtMailEnv		mail_error;
    MailSession		*ses = theRoamApp.session();
    DtMail::Session	*d_session = ses->session();
    RoamMenuWindow	*roamwin = NULL;
    char		*expanded_filename = NULL;
    char		*plus_filename = NULL;
    char		*relative_filename = NULL;

    theRoamApp.busyAllWindows(GETMSG(DT_catd, 3, 20, "Opening mailbox..."));

    // If the first character of destname is alphanumeric, we can
    // safely assume that it is relative to the root folder directory.
    // Prepend a '+' and call 'expandPath' to get the actual path.
    if (isalnum(filename[0]))
    {
	plus_filename = (char *) malloc(strlen(filename)+2);
 	sprintf(plus_filename, "+%s", filename);
        expanded_filename = d_session->expandPath(mail_error, plus_filename);
	free(plus_filename);
    }
    else
      expanded_filename = d_session->expandPath(mail_error, filename);

    // Check to see if the mbox is already open.  If it is, we will
    // simply make sure it's displayed in the current workspace.
    if (ses->isMboxOpen(expanded_filename))
    {
	roamwin = ses->getRMW(expanded_filename);
	ses->activateRMW(roamwin);
	if (NULL != roamwin) roamwin->displayInCurrentWorkspace();
    }
    else
    {
        if (DTM_FALSE == create &&
	    -1 == SafeAccess(expanded_filename, F_OK) &&
	    ENOENT == errno)
        {
	    char *buf = new char[2048];
	    int answer;
	    DtMailGenDialog *dialog = genDialog();

	    sprintf(
		buf,
		GETMSG(DT_catd, 3, 3, "The mailbox %s does not exist.\nCreate a mailbox with this name?"),
		filename);
	    dialog->setToQuestionDialog(GETMSG(DT_catd, 3, 22, "Mailer"), buf);
	    answer = dialog->post_and_return(DTMAILHELPERROR);
	    delete [] buf;
	    if (2 == answer) goto do_unbusy;
	    create = DTM_TRUE;
        }
    
        roamwin = new RoamMenuWindow(expanded_filename);
	roamwin->_create_mailbox_file = DTM_TRUE;
        roamwin->initialize();
        roamwin->mailboxFullpath(expanded_filename);
        roamwin->mailboxName(filename);
        roamwin->manage();
    }

    free(expanded_filename);
    relative_filename = d_session->getRelativePath(mail_error, filename);
    theRoamApp.globalAddToCachedContainerList(relative_filename);
    free(relative_filename);

do_unbusy:
    theRoamApp.unbusyAllWindows();
}

void
RoamMenuWindow::move_callback(void *client_data, char *selection)
{
    DtMailEnv mail_error;

    mail_error.clear();
    RoamMenuWindow *obj = (RoamMenuWindow *) client_data;

    theRoamApp.busyAllWindows(GETMSG(DT_catd, 3, 15, "Saving..."));
    obj->_mailbox->save();
    theRoamApp.unbusyAllWindows();

    obj->_list->copySelected(mail_error, selection, TRUE, FALSE);
    if (mail_error.isSet()) obj->postErrorDialog(mail_error);

}

void
RoamMenuWindow::copy_callback(void *client_data, char *selection)
{
    DtMailEnv mail_error;

    mail_error.clear();
    RoamMenuWindow *obj = (RoamMenuWindow *) client_data;

    theRoamApp.busyAllWindows(GETMSG(DT_catd, 3, 15, "Saving..."));
    obj->_mailbox->save();
    theRoamApp.unbusyAllWindows();

    obj->_list->copySelected(mail_error, selection, FALSE, FALSE);
    if (mail_error.isSet()) obj->postErrorDialog(mail_error);	
}


void
RoamMenuWindow::create_container_callback(void *client_data, char *selection)
{
    RoamMenuWindow *obj = (RoamMenuWindow*) client_data;
    obj->create_new_container(selection);
    if (NULL != selection) XtFree(selection);
}

void
RoamMenuWindow::create_new_container(char *filename)
{
    int answer;

    if (SafeAccess(filename, F_OK) == 0)
    {
        char *buf = new char[2048];
	sprintf(
		buf,
		GETMSG(DT_catd, 3, 21, "%s already exists.\nOverwrite?"),
		filename);
	_genDialog->setToQuestionDialog(GETMSG(DT_catd, 3, 22, "Mailer"), buf);
	answer = _genDialog->post_and_return(DTMAILHELPERROR);
	if (answer == 2) {
	    delete [] buf;
	    return;
	}

	if (unlink(filename) < 0)
	{
	    sprintf(buf, 
		GETMSG(DT_catd, 3, 23,
		  "Unable to overwrite %s.\nCheck file permissions and retry."),
		filename);

	    _genDialog->setToErrorDialog(GETMSG(DT_catd, 3, 24, "Mailer"), buf);
	    (void) _genDialog->post_and_return(DTMAILHELPERROR);
	    delete [] buf;
	    return;
	}
	delete [] buf;
    }
    
    // Path filename is ok -- now follow the same route as for Open
    this->view_mail_file(filename, DTM_TRUE);
}



// SR - added methods below

ViewMsgDialog*
RoamMenuWindow::ifViewExists(DtMailMessageHandle msg_num)
{

    int i;
    FORCE_SEGV_DECL(ViewMsgDialog, a_view);

    for (i = 0; i < _numDialogs; i++) {
	a_view = _dialogs[i];
	if (a_view->msgno() == msg_num) {
	    return(a_view);
	}
    }
    return(NULL);
}

	
void
RoamMenuWindow::addToRowOfButtons()
{
    FORCE_SEGV_DECL(CmdInterface, ci);
    Widget w, prev_widget;
    int offset = 10;

    _delete_button = new DeleteCmd ( 
				"Delete",
				GETMSG(DT_catd, 1, 9, "Delete"), 
				TRUE, this );
    ci  = new ButtonInterface (_rowOfButtons, _delete_button);
    w = ci->baseWidget();
    XtAddCallback(w, XmNhelpCallback, HelpCB, (void *)DTMAILDELBTNID);
    XtVaSetValues(w,
	XmNleftAttachment, XmATTACH_FORM,
	XmNtopAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,
	XmNmarginLeft, offset,
	XmNmarginRight, offset,
 	NULL );
    ci->manage();
    prev_widget = w;

    _next_button  = new NextCmd ( 
			"Next",
			GETMSG(DT_catd, 1, 10, "Next"), 
			TRUE, this );
    ci  = new ButtonInterface (_rowOfButtons, _next_button);
    w = ci->baseWidget();
    XtAddCallback(w, XmNhelpCallback, HelpCB, (void *)DTMAILNEXTBTNID);
    XtVaSetValues(w,
	XmNleftAttachment, XmATTACH_WIDGET,
	XmNleftWidget, prev_widget,
	XmNleftOffset, 10,
	XmNtopAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,
	XmNmarginLeft, offset,
	XmNmarginRight, offset,
 	NULL );
    ci->manage();
    prev_widget = w;

    _previous_button = new PrevCmd ( 
				"Previous",
				GETMSG(DT_catd, 1, 11, "Previous"), 
				TRUE, this );
    ci  = new ButtonInterface (_rowOfButtons, _previous_button);
    w = ci->baseWidget();
    XtAddCallback(w, XmNhelpCallback, HelpCB, (void *)DTMAILPREVBTNID);
    XtVaSetValues(w,
	XmNleftAttachment, XmATTACH_WIDGET,
	XmNleftWidget, prev_widget,
	XmNleftOffset, 10,
	XmNtopAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,
	XmNmarginLeft, offset,
	XmNmarginRight, offset,
 	NULL );
    ci->manage();
    prev_widget = w;

    _replySender_button = new ReplyCmd (
				"Reply to Sender",
				GETMSG(DT_catd, 1, 12, "Reply to Sender"), 
				TRUE, 
				this, 
				FALSE );
    ci  = new ButtonInterface (_rowOfButtons, _replySender_button);
    w = ci->baseWidget();
    XtAddCallback(w, XmNhelpCallback, HelpCB, (void *)DTMAILREPLYBTNID);
    XtVaSetValues(w,
	XmNleftAttachment, XmATTACH_WIDGET,
	XmNleftWidget, prev_widget,
	XmNleftOffset, 10,
	XmNtopAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,
	XmNmarginLeft, offset,
	XmNmarginRight, offset,
 	NULL );
    ci->manage();
    prev_widget = w;


    _print_button  = new PrintCmd ( "Print", GETMSG(DT_catd, 1, 13, "Print"),
				TRUE, TRUE, this);
    ci  = new ButtonInterface (_rowOfButtons, _print_button);
    w = ci->baseWidget();
    XtAddCallback(w, XmNhelpCallback, HelpCB, (void *)DTMAILPRINTBTNID);
    XtVaSetValues(w,
	XmNleftAttachment, XmATTACH_WIDGET,
	XmNleftWidget, prev_widget,
	XmNleftOffset, 10,
	XmNtopAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,
	XmNmarginLeft, offset,
	XmNmarginRight, offset,
 	NULL );
    ci->manage();
    prev_widget = w;
}

void
RoamMenuWindow::addToRowOfLabels(MsgScrollingList *msglist)
{

    XmString		 arrow, basexms, spaces, spaces_arrow;
    char		 arrow_symbol[2];
    char		*glyph_font;

    glyph_font = theRoamApp.glyphName();

#if 0
    XFontStruct		*xfs;
    XmFontListEntry	xmfle;
    XmFontList		xmfl, xmfl_old;
    Widget		 lbl;
    
    xfs = XLoadQueryFont(XtDisplay(_rowOfLabels), glyph_font);
    xmfle = XmFontListEntryCreate("arrow", XmFONT_IS_FONT, (XtPointer) xfs);

    lbl = XtVaCreateManagedWidget("t", xmLabelGadgetClass, _rowOfLabels, NULL);
    XtVaGetValues(lbl, XmNfontList, &xmfl_old, NULL);
    xmfl = XmFontListAppendEntry(xmfl_old, xmfle);
#else
//  Pixel		foreground, background;
    XmRenderTable	rt, rt_old;
    XmRendition		rend;
    Arg			args[25];
    int			nargs = 0;

//  lbl = XtVaCreateManagedWidget("t", xmLabelGadgetClass, _rowOfLabels, NULL);
//  XtVaGetValues(
//	lbl,
//	XmNrenderTable, &rt_old,
//	XmNforeground, &foreground,
//	XmNbackground, &background,
//	NULL);

//  XtSetArg(args[nargs], XmNrenditionBackground, background); nargs++;
//  XtSetArg(args[nargs], XmNrenditionForeground, foreground); nargs++;
//  XtSetArg(args[nargs], XmNloadModel, XmLOAD_IMMEDIATE); nargs++;

    XtSetArg(args[nargs], XmNfontName, glyph_font); nargs++;
    XtSetArg(args[nargs], XmNfontType, XmFONT_IS_FONT); nargs++;
    rend = XmRenditionCreate(_rowOfLabels, "arrow", args, nargs);
#endif

    arrow_symbol[0] = (char) 209;
    arrow_symbol[1] = '\0';

    spaces = XmStringCreateLocalized("  ");
    arrow = XmStringCreate((char*) arrow_symbol, "arrow");
    spaces_arrow = XmStringConcat(spaces, arrow);

    XmStringFree(spaces);
    spaces = XmStringCreateLocalized("    ");

    basexms = XmStringCreateLocalized(GETMSG(DT_catd, 1, 14, "Sender"));
    _sender_xms = XmStringConcat(basexms, spaces);
    _sender_key_xms = XmStringConcat(basexms, spaces_arrow);
    XmStringFree(basexms);

    _sender_lbl = XtVaCreateManagedWidget(
				"Sender", xmLabelGadgetClass, 
				_rowOfLabels,
#if 0
				XmNfontList, xmfl,
				XmNrenderTable, rt,
#endif
				XmNlabelString, _sender_xms,
				XmNalignment, XmALIGNMENT_BEGINNING,
				NULL);

    XtSetArg(args[nargs], XmNfontName, glyph_font); nargs++;
    XtSetArg(args[nargs], XmNfontType, XmFONT_IS_FONT); nargs++;
    rend = XmRenditionCreate(_rowOfLabels, "arrow", args, nargs);

    XtVaGetValues(_sender_lbl, XmNrenderTable, &rt_old, NULL);
    rt = XmRenderTableCopy(rt_old, NULL, 0);
    rt = XmRenderTableAddRenditions(rt, &rend, 1, XmMERGE_REPLACE);
    XtVaSetValues(_sender_lbl, XmNrenderTable, rt, NULL);

    // Add help callback
    // printHelpId("Sender", _sender_lbl);
    // XtAddCallback(_sender_lbl, XmNhelpCallback, HelpCB, helpId);

    basexms = XmStringCreateLocalized(GETMSG(DT_catd, 1, 15, "Subject"));
    _subject_xms = XmStringConcat(basexms, spaces);
    _subject_key_xms = XmStringConcat(basexms, spaces_arrow);
    XmStringFree(basexms);

    _subject_lbl = XtVaCreateManagedWidget(
				"Subject", xmLabelGadgetClass, 
				_rowOfLabels,
#if 0
				XmNfontList, xmfl,
				XmNrenderTable, rt,
#endif
				XmNlabelString, _subject_xms,
				XmNalignment, XmALIGNMENT_BEGINNING,
				NULL);

    XtSetArg(args[nargs], XmNfontName, glyph_font); nargs++;
    XtSetArg(args[nargs], XmNfontType, XmFONT_IS_FONT); nargs++;
    rend = XmRenditionCreate(_rowOfLabels, "arrow", args, nargs);

    XtVaGetValues(_subject_lbl, XmNrenderTable, &rt_old, NULL);
    rt = XmRenderTableCopy(rt_old, NULL, 0);
    rt = XmRenderTableAddRenditions(rt, &rend, 1, XmMERGE_REPLACE);
    XtVaSetValues(_subject_lbl, XmNrenderTable, rt, NULL);

    // Add help callback
    // printHelpId("Subject", _subject_lbl);
    // XtAddCallback(_subject_lbl, XmNhelpCallback, HelpCB, helpId);

    basexms = XmStringCreateLocalized(GETMSG(DT_catd, 1, 16,"Date and Time"));
    _date_xms = XmStringConcat(basexms, spaces);
    _date_key_xms = XmStringConcat(basexms, spaces_arrow);
    XmStringFree(basexms);

    _date_lbl = XtVaCreateManagedWidget(
				"DateTime", xmLabelGadgetClass, 
				_rowOfLabels,
#if 0
				XmNfontList, xmfl,
				XmNrenderTable, rt,
#endif
				XmNlabelString, _date_xms,
				XmNalignment, XmALIGNMENT_BEGINNING,
				NULL);

    XtSetArg(args[nargs], XmNfontName, glyph_font); nargs++;
    XtSetArg(args[nargs], XmNfontType, XmFONT_IS_FONT); nargs++;
    rend = XmRenditionCreate(_rowOfLabels, "arrow", args, nargs);

    XtVaGetValues(_date_lbl, XmNrenderTable, &rt_old, NULL);
    rt = XmRenderTableCopy(rt_old, NULL, 0);
    rt = XmRenderTableAddRenditions(rt, &rend, 1, XmMERGE_REPLACE);
    XtVaSetValues(_date_lbl, XmNrenderTable, rt, NULL);

    // Add help callback
    // printHelpId("DateTime", _subject_lbl);
    // XtAddCallback(_date_lbl, XmNhelpCallback, HelpCB, helpId);

    basexms = XmStringCreateLocalized(GETMSG(DT_catd, 1, 17, "Size"));
    _size_xms = XmStringConcat(basexms, spaces);
    _size_key_xms = XmStringConcat(basexms, spaces_arrow);
    XmStringFree(basexms);

    _size_lbl = XtVaCreateManagedWidget(
				"Size", xmLabelGadgetClass, 
				_rowOfLabels,
#if 0
				XmNfontList, xmfl,
				XmNrenderTable, rt,
#endif
				XmNlabelString, _size_xms,
				XmNalignment, XmALIGNMENT_BEGINNING,
				NULL);

    XtSetArg(args[nargs], XmNfontName, glyph_font); nargs++;
    XtSetArg(args[nargs], XmNfontType, XmFONT_IS_FONT); nargs++;
    rend = XmRenditionCreate(_rowOfLabels, "arrow", args, nargs);

    XtVaGetValues(_size_lbl, XmNrenderTable, &rt_old, NULL);
    rt = XmRenderTableCopy(rt_old, NULL, 0);
    rt = XmRenderTableAddRenditions(rt, &rend, 1, XmMERGE_REPLACE);
    XtVaSetValues(_size_lbl, XmNrenderTable, rt, NULL);

    // Add help callback
    // printHelpId("Size", _size_lbl);
    // XtAddCallback(_size_lbl, XmNhelpCallback, HelpCB, helpId);

    // Adjust labels so the align on the columns
    msglist->layoutLabels(_sender_lbl, _subject_lbl, _date_lbl, _size_lbl);

    XmStringFree(arrow);
    XmStringFree(spaces);
    XmStringFree(spaces_arrow);
    return;
}

void
RoamMenuWindow::addToRowOfMessageStatus()
{
    XmString labelStr1, labelStr2;

    // Size of first label
    
    labelStr1 = XmStringCreateLocalized(GETMSG(DT_catd, 3, 25,
	   "Loading container..."));

    labelStr2 = XmStringCreateLocalized(
			GETMSG(DT_catd, 3, 26, "Folder Summary Information"));

    _message = XtCreateManagedWidget(
			"Message_Status_Text", xmLabelWidgetClass,
			_rowOfMessageStatus, NULL, 0);
    printHelpId("_message", _message);
    /* add help callback */
    // XtAddCallback(_message, XmNhelpCallback, HelpCB, helpId);


     XtVaSetValues(_message,
	XmNalignment, XmALIGNMENT_BEGINNING,
	XmNleftAttachment, XmATTACH_FORM,
	XmNtopAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,
 	XmNlabelString, labelStr1,
 	NULL );
    
    _message_summary = XtCreateManagedWidget("Message_Summary", 
					xmLabelWidgetClass,
				        _rowOfMessageStatus, NULL, 0);
     XtVaSetValues(_message_summary,
	XmNalignment, XmALIGNMENT_END,
 	XmNlabelString, labelStr2,
 	XmNrightAttachment, XmATTACH_FORM,
 	NULL );

     XtVaSetValues(_message,
	 XmNrightAttachment, XmATTACH_WIDGET,
	 XmNrightWidget, _message_summary,
	 XmNrightOffset, 10,
	 NULL);

     XmStringFree(labelStr1);
     XmStringFree(labelStr2);
 
}

void
RoamMenuWindow::createContainerList()
{
    FORCE_SEGV_DECL(DtMail::Session, m_session);
    DtMailEnv		error;
    const char		*val = NULL;
    const char		*cached_containerlist_size = NULL;
    const char		*display_cfs = NULL;
    DtMailBoolean	user_list = DTM_FALSE;

    //
    // Get names for permanent containers from .mailrc.
    // We get the items from the "filemenu2" variable.
    if (_filemenu2 != NULL) free (_filemenu2);

    m_session = theRoamApp.session()->session();
    m_session->mailRc(error)->getValue(error, "filemenu2", &val);
    if (error.isNotSet() && val != NULL && *val != '\0')
    {
	user_list = DTM_TRUE;
        _filemenu2 = strdup(val);
    }
    else _filemenu2 = NULL;

    if (NULL != val)
      free((void*) val);

    // We will use _user_containerlist to keep track of the static set of
    // containers coming from the "filemenu2" value. 
    // We will use _cached_containerlist to keep track of containers
    // which get cached from "Other Mailboxes... operations from the
    // Open, CopyTo, and Move menus.

    if (_user_containerlist != NULL)
	delete (_user_containerlist);
    _user_containerlist = new DtVirtArray<ContainerMenuCmd*> (3);

    m_session->mailRc(error)->getValue(
				error,
				"dontdisplaycachedfiles",
				&display_cfs);

    if (error.isNotSet() && display_cfs != NULL) 
      _max_cached_list_size = 0;
    else
    {
	error.clear();
	m_session->mailRc(error)->getValue(
					error, 
					"cachedfilemenusize",
					&cached_containerlist_size);
	if (error.isNotSet() &&
	    cached_containerlist_size && 
	    *cached_containerlist_size)
	  _max_cached_list_size = 
	    (int) strtol(cached_containerlist_size, NULL, 10);
	else 
	  _max_cached_list_size = 10;
    }

    if (NULL != display_cfs)
      free((void*) display_cfs);
    if (NULL != cached_containerlist_size)
      free((void*) cached_containerlist_size);

    int size = (_max_cached_list_size ? _max_cached_list_size : 1);
    _cached_containerlist = new DtVirtArray<ContainerMenuCmd*> (size);

    if (user_list == DTM_TRUE) {
    	char *expanded_list = m_session->expandPath(error, _filemenu2);
    	char *token = NULL;

	//
	// Create arrays to hold the user defined container list and the
	// recently visited (cached) container list.
	//
	if ((token = (char *) strtok(expanded_list, " ")))
	{
	    ContainerMenuCmd *null_container;
	    
	    null_container= new ContainerMenuCmd(
						strdup(token),
						token,
						TRUE,
						this,
						DTM_NONE);
	    _user_containerlist->append(null_container);
	    while (token = (char *) strtok(NULL, " "))
	    {
	        null_container= new ContainerMenuCmd(
						strdup(token),
						token,
						TRUE,
						this,
						DTM_NONE);
	        _user_containerlist->append(null_container);
	    }
	}
        free(expanded_list);
    }

    if (_user_containerlist->length() > 0)
      _first_cached_item = _user_containerlist->length() +4;
    else
      _first_cached_item = 3;
}

void
RoamMenuWindow::createOpenContainerList(CmdList * open_container)
{
    int			ncontainers, i;
    char		*container_name;
    ContainerMenuCmd	*container_cmd;

    _open_container_separator = new SeparatorCmd("Separator","Separator", TRUE);

    _open_container_inbox = new OpenInboxCmd(
					"Inbox",
					GETMSG(DT_catd, 1, 221, "Inbox"),
					(FALSE == this->inbox()),
					this);

    _open_container_other = new UnifiedSelectMailboxCmd(
				"Open",
				GETMSG(DT_catd, 1, 246, "Other Mailboxes..."), 
				GETMSG(DT_catd, 1, 26, "Mailer - Open"),
				GETMSG(DT_catd, 1, 27, "Open"),
				TRUE,
				RoamMenuWindow::file_selection_callback,
				this,
				this->baseWidget(),
				DTM_TRUE);

    //
    // We will use _open_container_containerlist to keep track of the
    // static set of OpenContainer commands coming from the "filemenu2"
    // value.
    //
    ncontainers = _user_containerlist->length();

    if (_open_container_containerlist != NULL)
      delete (_open_container_containerlist);
    _open_container_containerlist =
      new DtVirtArray<ContainerMenuCmd*>(ncontainers);

    for (i=0; i<ncontainers; i++)
    {
	container_name = (*_user_containerlist)[i]->containerName();
	if (NULL != container_name)
	{
	    container_cmd = new ContainerMenuCmd(
						strdup(container_name),
						container_name,
						TRUE,
						this,
						DTM_OPEN);
	    open_container->add(container_cmd);
	    _open_container_containerlist->append(container_cmd);
	}
    }

    if (0 < ncontainers)
      open_container->add(_open_container_separator);
    open_container->add(_open_container_inbox);
    open_container->add(_open_container_other);
    open_container->add(_open_container_separator);

    //
    // We will use _open_container_containerlist_cached
    // to keep track of OpenContainer which get cached from
    // "Other Mailboxes..." operations from the Open, CopyTo, and Move menus.
    //

    ncontainers =  (_max_cached_list_size ? _max_cached_list_size : 1);

    if (_open_container_containerlist_cached != NULL)
      delete (_open_container_containerlist_cached);
    _open_container_containerlist_cached =
      new DtVirtArray<ContainerMenuCmd*> (ncontainers);
 
    ncontainers = (_display_cached_list ? _cached_containerlist->length() : 0);
    for (i=0; i<ncontainers; i++)
    {
	container_name = (*_cached_containerlist)[i]->containerName();
	if (NULL != container_name)
	{
	    container_cmd = new ContainerMenuCmd(
						strdup(container_name),
						container_name,
						TRUE,
						this,
						DTM_OPEN);
	    open_container->add(container_cmd);
	    _open_container_containerlist_cached->append(container_cmd);
	}
    }
}


void
RoamMenuWindow::construct_file_menu()
{
    FORCE_SEGV_DECL(CmdList, cmdList);

    _file_separator = new SeparatorCmd( "Separator","Separator", TRUE );
    
    // Create the "Container" item in the menubar.  And fill 
    // with items below the "Container" item in the menubar.
    
    cmdList = new CmdList( "Mailbox", GETMSG(DT_catd, 1, 18, "Mailbox") );
    _file_cmdlist = cmdList;

    _file_check_new_mail	= new CheckForNewMailCmd(
				"Check for New Mail",
				GETMSG(DT_catd, 1, 19, "Check for New Mail"), 
				TRUE, 
				this);
    if (this->inbox()) { // Deactivate the Open Inbox item 
	_file_open_inbox	       = new OpenInboxCmd(
					"Open Inbox",
					GETMSG(DT_catd, 1, 20, "Open Inbox"),
					FALSE,
					this);
    }
    else { // Activate the Open Inbox item.
	_file_open_inbox	       = new OpenInboxCmd(
					"Open Inbox",
					GETMSG(DT_catd, 1, 21, "Open Inbox"),
					TRUE,
					this);
    }

    _file_new_container     = new UnifiedSelectMailboxCmd( 
				"New...",
				GETMSG(DT_catd, 1, 22, "New..."),
				GETMSG(DT_catd, 1, 23, "Mailer - New"),
				GETMSG(DT_catd, 1, 24, "New"),
				TRUE,
				RoamMenuWindow::create_container_callback,
				this,
				this->baseWidget(),
				DTM_TRUE);
#if defined(USE_OLD_FILE_OPEN)
    _file_open              = new UnifiedSelectMailboxCmd(
				"Open...",
				GETMSG(DT_catd, 1, 25, "Open..."), 
				GETMSG(DT_catd, 1, 26, "Mailer - Open"),
				GETMSG(DT_catd, 1, 27, "Open"),
				TRUE,
				RoamMenuWindow::file_selection_callback,
				this,
				this->baseWidget());
#endif

    _open_container_cmdlist = new CmdList(
				"Open Container",
				GETMSG(DT_catd, 1, 245, "Open"));
    createOpenContainerList(_open_container_cmdlist);

    _file_destroy_deleted_msgs  = new DestroyCmd(
					"Destroy Deleted Message",
					GETMSG(DT_catd, 1, 28, 
						"Destroy Deleted Messages"), 
					      TRUE,
					      this);


    _file_quit              = new QuitCmd (
				"Close",
				GETMSG(DT_catd, 1, 29, "Close"), 
				TRUE, 
				this);
    
    cmdList->add(_file_check_new_mail);
    cmdList->add(_file_open_inbox);
    cmdList->add(_file_separator);
    cmdList->add(_file_new_container);
#if defined(USE_OLD_FILE_OPEN)
    cmdList->add(_file_open);
#endif
    cmdList->add(_open_container_cmdlist);
    cmdList->add(_file_separator);
    cmdList->add(_file_destroy_deleted_msgs);
    cmdList->add(theRoamApp.mailOptions());
    cmdList->add(_file_separator);
    cmdList->add(_file_quit);
    
    _menuBar->addCommands(&_file_cascade, cmdList);
 
    _opencontainerMenu = _open_container_cmdlist->getPaneWidget();
 
    XtVaSetValues(
		_opencontainerMenu, 
		XmNpacking, XmPACK_COLUMN, 
		XmNorientation, XmVERTICAL,
		NULL);

    XtAddCallback(
		_opencontainerMenu,
		XmNmapCallback, &RoamMenuWindow::map_menu,
		NULL);

    XtSetSensitive(_opencontainerMenu, TRUE);
}

void
RoamMenuWindow::createCopyList(CmdList * copy_to)
{
    int			ncontainers, i;
    char		*container_name;
    ContainerMenuCmd	*container_cmd;

    _copyto_separator = new SeparatorCmd("Separator","Separator", TRUE);

    _copyto_inbox = new CopyToInboxCmd(
			    	"Inbox",
			    	GETMSG(DT_catd, 1, 221, "Inbox"), 
			    	TRUE, 
			    	this);

    // This is where we initialize _move_copy_button, so this needs to
    // be the first place that we use it.  This routine needs to be
    // called before construct_move_menu(), which expects _move_copy_button
    // to be intialized.

    DtMailEnv error;
    DtMail::Session * d_session = theRoamApp.session()->session();
    DtMail::MailRc * mailrc = d_session->mailRc(error);
    const char *value = NULL;
    DtMailBoolean only_show_mailboxes = DTM_FALSE;

    mailrc->getValue(error, "movecopytomailboxesonly", &value);
    only_show_mailboxes = (error.isSet()) ? DTM_FALSE : DTM_TRUE;
    { 
	value = strdup("15");
	error.clear();
    }
    if (value)
      free((void*) value);

    _move_copy_button = new MoveCopyCmd (
				"Other Mailboxes...",
                                GETMSG(DT_catd, 1, 65, "Other Mailboxes..."),
                                TRUE,
                                RoamMenuWindow::move_callback,
                                RoamMenuWindow::copy_callback,
                                this,
                                this->baseWidget(),
				only_show_mailboxes);
    _copyto_other = new CopyCmd(
			        GETMSG(DT_catd, 1, 237, "Copy"),
                                GETMSG(DT_catd, 1, 65, "Other Mailboxes..."),
				TRUE,
				this,
				(MoveCopyCmd *) _move_copy_button);

    //
    // We will use _copyto_containerlist to keep track of the
    // static set of CopyTo commands coming from the "filemenu2"
    // value.
    //
    ncontainers = _user_containerlist->length();

    if (_copyto_containerlist != NULL)
      delete (_copyto_containerlist);
    _copyto_containerlist =
      new DtVirtArray<ContainerMenuCmd *> (ncontainers);

    for (i=0; i<ncontainers; i++)
    {
	container_name = (*_user_containerlist)[i]->containerName();
	if (NULL != container_name)
	{
	    container_cmd = new ContainerMenuCmd(
						strdup(container_name),
						container_name,
						TRUE,
						this,
						DTM_COPY);
	    copy_to->add(container_cmd);
	    _copyto_containerlist->append(container_cmd);
	}
    }

    if (0 < ncontainers)
      copy_to->add(_copyto_separator);
    copy_to->add(_copyto_inbox);
    copy_to->add(_copyto_other);
    copy_to->add(_copyto_separator);

    //
    // We will use _copyto_containerlist_cached
    // to keep track of CopyTo commands which get cached from
    //"Other Mailboxes..." operations from the Open, CopyTo, and Move menus.
    //

    ncontainers = (_max_cached_list_size ? _max_cached_list_size : 1);

    if (_copyto_containerlist_cached != NULL)
      delete (_copyto_containerlist_cached);
    _copyto_containerlist_cached =
      new DtVirtArray<ContainerMenuCmd *> (ncontainers);

    ncontainers = (_display_cached_list ? _cached_containerlist->length() : 0);
    for (i=0; i<ncontainers; i++)
    {
	container_name = (*_cached_containerlist)[i]->containerName();
	if (NULL != container_name)
	{
	    container_cmd = new ContainerMenuCmd(
						strdup(container_name),
						container_name,
						TRUE,
						this,
						DTM_COPY);
	    copy_to->add(container_cmd);
	    _copyto_containerlist_cached->append(container_cmd);
	}
    }
}

void
RoamMenuWindow::construct_message_menu()
{
    FORCE_SEGV_DECL(CmdList, cmdList);
    // Separator for menu items
    
    _msg_separator= new SeparatorCmd( "Separator","Separator", TRUE );
    
    _msg_open		= new OpenMsgCmd(
				"Open",
				GETMSG(DT_catd, 1, 30, "Open"), 
				TRUE, this);

    _msg_save_as	= new SaveAsTextCmd (
				"Save As Text...",
				GETMSG(DT_catd, 1, 31, "Save As Text..."),
				GETMSG(DT_catd, 1, 32, "Mailer - Message - Save As Text"),
				TRUE,
				get_editor()->textEditor(),
				this,
				this->baseWidget());

    _copyto_cmdlist = new CmdList("Copy To", GETMSG(DT_catd, 1, 33, "Copy To"));
    createCopyList(_copyto_cmdlist);

    _msg_print		= new PrintCmd(
				"Print...",
				GETMSG(DT_catd, 1, 34, "Print..."), 
				TRUE, FALSE, this);

    _msg_find		= new FindCmd (
				"Find...",
 				GETMSG(DT_catd, 1, 35, "Find..."), 
 				TRUE, this );

    _msg_select_all = new SelectAllCmd (
				"Select All",
				GETMSG(DT_catd, 1, 36, "Select All"),
				TRUE, this );

    _msg_delete		= new DeleteCmd(
				"Delete",
				GETMSG(DT_catd, 1, 37, "Delete"),
				TRUE, this);

    _msg_undelete_last       = new UndeleteCmd ( 
				"Undelete Last",
				GETMSG(DT_catd, 1, 38, "Undelete Last"), 
				TRUE, this, FALSE );

    _msg_undelete_from_list   = new UndeleteCmd(
				"Undelete From List...",
				GETMSG(DT_catd, 1, 39, 
					"Undelete From List..."), 
				TRUE, this, TRUE);

    // Message Menu
    
    cmdList = new CmdList( "Message", GETMSG(DT_catd, 1, 40, "Message") );
    _msg_cmdlist = cmdList;

    cmdList->add(_msg_open);
    cmdList->add(_msg_save_as);
    cmdList->add(_copyto_cmdlist);
    cmdList->add(_msg_print);
    cmdList->add(_msg_find);
    cmdList->add(_msg_select_all);

    cmdList->add(_msg_separator);

    cmdList->add(_msg_delete);
    cmdList->add(_msg_undelete_last);
    cmdList->add(_msg_undelete_from_list);

    _menuBar->addCommands(&_message_cascade, cmdList, FALSE, XmMENU_BAR);
 
    _copytoMenu = _copyto_cmdlist->getPaneWidget();
 
    XtVaSetValues(_copytoMenu, 
		  XmNpacking, XmPACK_COLUMN, 
		  XmNorientation, XmVERTICAL,
		  NULL);

    XtAddCallback(_copytoMenu, XmNmapCallback, &RoamMenuWindow::map_menu, NULL);

    XtSetSensitive(_copytoMenu, TRUE);
}

void
RoamMenuWindow::construct_message_popup(void)
{
   _msgsPopup_cmdlist = new CmdList( "MsgsPopup", "MsgsPopup");

   LabelCmd *title     = new LabelCmd (
			"Mailer - Messages",
                        GETMSG(DT_catd, 1, 42, "Mailer - Messages"), TRUE);
   _msgsPopup_separator = new SeparatorCmd( "Separator","Separator", TRUE );
   _msgsPopup_cmdlist->add(title);
   _msgsPopup_cmdlist->add(_msgsPopup_separator);
   _msgsPopup_cmdlist->add(_msg_delete);
   _msgsPopup_cmdlist->add(_msg_undelete_last);
   _msgsPopup_cmdlist->add(_msg_print);
   _msgsPopup_cmdlist->add(_comp_replySender);
   _msgsPopup_cmdlist->add(_move_cmdlist);
   _msgsPopup_cmdlist->add(_msg_save_as);
   _msgsPopup_cmdlist->add(_file_check_new_mail);

   Widget parent = XtParent(_list->get_scrolling_list());
   _menuPopupMsgs = new MenuBar(parent, "RoamMsgsPopup", XmMENU_POPUP);
   _msgsPopupMenu = _menuPopupMsgs->addCommands(_msgsPopup_cmdlist, 
			FALSE, XmMENU_POPUP);
   XtAddEventHandler(parent, ButtonPressMask,
                        FALSE, MenuButtonHandler, (XtPointer) this);
   
   _msgsPopupMoveMenu = _move_cmdlist->getPaneWidget();

   XtVaSetValues(_msgsPopupMoveMenu, 
		 XmNpacking, XmPACK_COLUMN, 
		 XmNorientation, XmVERTICAL,
		 NULL);
   XtAddCallback(_msgsPopupMoveMenu, XmNmapCallback, &RoamMenuWindow::map_menu, NULL);

   XtSetSensitive(_msgsPopupMoveMenu, TRUE);
 
}

void
RoamMenuWindow::construct_edit_menu()
{
    FORCE_SEGV_DECL(CmdList, cmdList);
    // Separator for menu items
    
    _edit_copy = new EditCopyCmd(
			"Copy",
			GETMSG(DT_catd, 1, 43, "Copy"), 
			TRUE, 
			this
		     );

    _edit_select_all = new EditSelectAllCmd(
			"Select All",
			GETMSG(DT_catd, 1, 44, "Select All"), 
			TRUE,
			this
		    );

    cmdList = new CmdList("Edit", GETMSG(DT_catd, 1, 45, "Edit"));
    _edit_cmdlist = cmdList;

    cmdList->add(_edit_copy);
    cmdList->add(_edit_select_all);
    
    _menuBar->addCommands(cmdList);
}

void
RoamMenuWindow::construct_text_popup(void)
{
   if (theApplication->bMenuButton() != Button3)
       return;

   _textPopup_cmdlist = new CmdList( "TextPopup", "TextPopup");

    LabelCmd *title     = new LabelCmd (
			"Mailer - Text",
			GETMSG(DT_catd, 1, 46, "Mailer - Text"), TRUE);
    _textPopup_separator = new SeparatorCmd( "Separator","Separator", TRUE );

    _textPopup_cmdlist->add(title);
    _textPopup_cmdlist->add(_textPopup_separator);
    _textPopup_cmdlist->add(_edit_copy);
    _textPopup_cmdlist->add(_edit_select_all);

    Widget parent = _my_editor->textEditor()->get_editor();
    _menuPopupText = new MenuBar(parent, "RoamTextPopup", XmMENU_POPUP);
    _textPopupMenu = _menuPopupText->addCommands(_textPopup_cmdlist, 
				FALSE, XmMENU_POPUP);
}


void RoamMenuWindow::construct_view_menu()
{
    FORCE_SEGV_DECL(CmdList, cmdList);
    FORCE_SEGV_DECL(CmdList, subCmdList);

    _view_separator= new SeparatorCmd("Separator","Separator",TRUE);
    
    _view_next		= new NextCmd ( 
				"Next",
				GETMSG(DT_catd, 1, 47, "Next"), TRUE, this );
    _view_previous		= new PrevCmd ( 
				"Previous",
				GETMSG(DT_catd, 1, 48, "Previous"), TRUE, this );
    
    _view_abbrev_headers = new AbbrevHeadersCmd(
				"Abbreviated Headers",
				GETMSG(DT_catd, 1, 49, "Abbreviated Headers"), 
				this);


    _view_sortTD = new SortCmd ("By Date/Time", 
				GETMSG(DT_catd, 1, 50, "By Date/Time"),
				TRUE,
				this,
				SortTimeDate);
    _view_sortSender = new SortCmd ("By Sender",
				GETMSG(DT_catd, 1, 51, "By Sender"),
				TRUE,
				this,
				SortSender);
    _view_sortSubject = new SortCmd ("By Subject",
				GETMSG(DT_catd, 1, 52, "By Subject"),
				TRUE,
				this,
				SortSubject);
    _view_sortSize = new SortCmd ("By Size",
				GETMSG(DT_catd, 1, 53, "By Size"),
				TRUE,
				this,
				SortSize);
    _view_sortStatus = new SortCmd ("By Status",
				GETMSG(DT_catd, 1, 54, "By Status"),
				TRUE,
				this,
				SortStatus);

    // View Menu
    
    cmdList = new CmdList( "View", GETMSG(DT_catd, 1, 55, "View") );
    _view_cmdlist = cmdList;
    
    cmdList->add(_view_next);
    cmdList->add(_view_previous);
    cmdList->add(_view_separator);
    cmdList->add(_view_abbrev_headers);
    cmdList->add(_view_separator);
    cmdList->add(_view_sortTD);
    cmdList->add(_view_sortSender);
    cmdList->add(_view_sortSubject);
    cmdList->add(_view_sortSize);
    cmdList->add(_view_sortStatus);
    
    _menuBar->addCommands ( cmdList );
}


void
RoamMenuWindow::construct_compose_menu()
{
    FORCE_SEGV_DECL(CmdList, cmdList);
    FORCE_SEGV_DECL(CmdList, subCmdList);

    // Separator for menu items
    
    _comp_separator= new SeparatorCmd( "Separator","Separator", TRUE );
    
    _comp_new	= new ComposeCmd ( 
			"New Message",
			GETMSG(DT_catd, 1, 56, "New Message"), 
			TRUE, 
			this );
    _comp_new_include = new ForwardCmd ( 
			"New, Include All",
			GETMSG(DT_catd, 1, 57, "New, Include All"), 
			TRUE, 
			this, 
			FALSE );
    _comp_forward	= new ForwardCmd ( 
			"Forward Message",
			GETMSG(DT_catd, 1, 58, "Forward Message"), 
			TRUE, 
			this, 
			TRUE );

    _comp_replySender = new ReplyCmd ( 
			      "Reply to Sender",
			      GETMSG(DT_catd, 1, 59, "Reply to Sender"), 
			      TRUE, 
			      this, 
			      FALSE );

    _comp_replyAll	= new ReplyAllCmd ( 
			"Reply to All",
			GETMSG(DT_catd, 1, 60, "Reply to All"), 
			TRUE, 
			this, 
			FALSE );

    _comp_replySinclude= new ReplyCmd ( 
			"Reply to Sender, Include",
			GETMSG(DT_catd, 1, 61, "Reply to Sender, Include"), 
			TRUE, 
			this, 
			TRUE );	

    _comp_replyAinclude= new ReplyAllCmd ( 
			"Reply to All, Include",
			GETMSG(DT_catd, 1, 62, "Reply to All, Include"), 
			TRUE, 
			this, 
			TRUE );	

    
    // Compose Menu
    
    cmdList = new CmdList( "Compose", GETMSG(DT_catd, 1, 63, "Compose") );
    _comp_cmdlist = cmdList;
    cmdList->add(_comp_new);
    cmdList->add(_comp_new_include);
    cmdList->add(_comp_forward);
    cmdList->add(_comp_separator);
    cmdList->add(_comp_replySender);
    cmdList->add(_comp_replyAll);
    cmdList->add(_comp_replySinclude);
    cmdList->add(_comp_replyAinclude);

    _menuBar->addCommands ( cmdList );
}

// construct_move_menu() builds the Move menu on the menu bar.  
// There is a user defined set of items at the top, the Inbox,
// the "Mail Filing..." item, and a dynamic list of most recently
// accessed containers at the bottom.

void
RoamMenuWindow::construct_move_menu()
{
    int			ncontainers, i;
    char		*container_name;
    ContainerMenuCmd	*container_cmd;


    if (_move_cmdlist != NULL)
      delete _move_cmdlist;
    _move_cmdlist = new CmdList( "Move", GETMSG(DT_catd, 1, 64, "Move") );

    _move_separator = new SeparatorCmd("Separator","Separator", TRUE );

    _move_inbox = new MoveToInboxCmd(
                  		"Inbox",
       		  		GETMSG(DT_catd, 1, 221, "Inbox"),
 		  		TRUE, 
  		  		this);

    // We expect _move_copy_button to have been initialized already when
    // we constructed the copy menu.
    _move_other = new MoveCmd(
				GETMSG(DT_catd, 1, 90, "Move"),
                                GETMSG(DT_catd, 1, 65, "Other Mailboxes..."),
				TRUE,
				this,
				(MoveCopyCmd *) _move_copy_button);

    //
    // We will use _move_containerlist to keep track of the
    // static set of Move commands coming from the "filemenu2"
    // value.
    //
    ncontainers = _user_containerlist->length();

    if (_move_containerlist != NULL)
      delete _move_containerlist;
    _move_containerlist = new DtVirtArray<ContainerMenuCmd *> (ncontainers);

    for (i=0; i<ncontainers; i++)
    {
	container_name = (*_user_containerlist)[i]->containerName();
	if (NULL != container_name)
	{
	    container_cmd = new ContainerMenuCmd(
						strdup(container_name),
						container_name,
						TRUE,
						this,
						DTM_MOVE);
	    _move_cmdlist->add(container_cmd);
	    _move_containerlist->append(container_cmd);
	}
    }

    // Move menu
    if (0 < ncontainers)
      _move_cmdlist->add(_move_separator);
    _move_cmdlist->add(_move_inbox);

    _move_cmdlist->add(_move_other);
    _move_cmdlist->add(_move_separator);

    //
    // We will use _move_containerlist_cached
    // to keep track of Move commands which get cached from
    // "Other Mailboxes..." operations from the Open, CopyTo, and Move menus.
    //

    ncontainers = (_max_cached_list_size ? _max_cached_list_size : 1 );

    if (_move_containerlist_cached != NULL)
      delete _move_containerlist_cached;
    _move_containerlist_cached =
      new DtVirtArray<ContainerMenuCmd *> (ncontainers);
    
    ncontainers = (_display_cached_list ? _cached_containerlist->length() : 0);
    for (i=0; i<ncontainers; i++)
    {
	container_name = (*_cached_containerlist)[i]->containerName();
	if (NULL != container_name)
	{
	    container_cmd = new ContainerMenuCmd(
						strdup(container_name),
						container_name,
						TRUE,
						this,
						DTM_MOVE);
	    _move_cmdlist->add(container_cmd);
	    _move_containerlist_cached->append(container_cmd);
	}
    }


    _moveMenu = _menuBar->addCommands(
				&_move_cascade, _move_cmdlist, 
				FALSE, XmMENU_BAR);
    XtVaSetValues(_moveMenu, 
		  XmNpacking, XmPACK_COLUMN, 
		  XmNorientation, XmVERTICAL,
		  NULL);
    XtAddCallback(_moveMenu, XmNmapCallback, &RoamMenuWindow::map_menu, NULL);
}


// Attachment menu
void
RoamMenuWindow::construct_attachment_menu()
{

    _att_save	= new SaveAttachCmd (
				"Save As...",
				GETMSG(DT_catd, 1, 66, "Save As..."),
				GETMSG(DT_catd, 1, 67, "Mailer - Attachments - Save As"),
				FALSE, 
				RoamMenuWindow::save_attachment_callback,
				this,
				this->baseWidget());

    _att_select_all = new SelectAllAttachsCmd(
					"Select All",
					GETMSG(
					    DT_catd, 1, 68, "Select All"
					),
					this);

    _att_cmdlist = new CmdList( 
					"Attachments",
					GETMSG(
						DT_catd, 1, 69, 
						"Attachments"
					) 
				);
    _att_cmdlist->add(_att_save);
    _att_cmdlist->add(_att_select_all);

    // Create a pulldown from the items in the list.  Retain a handle
    // to that pulldown since we need to dynamically add/delete entries 
    // to this menu based on the selection of attachments.

    _attachmentMenu = _menuBar->addCommands(_att_cmdlist);
}

void
RoamMenuWindow::construct_attachment_popup(void)
{
   _attPopup_cmdlist = new CmdList( "AttachmentsPopup", "AttachmentsPopup");

    LabelCmd *title     = new LabelCmd (
			"Mailer - Attachments",
			GETMSG(DT_catd, 1, 70, "Mailer - Attachments"), TRUE);
    _attPopup_separator = new SeparatorCmd( "Separator","Separator", TRUE );

    _attPopup_cmdlist->add(title);
    _attPopup_cmdlist->add(_attPopup_separator);
    _attPopup_cmdlist->add(_att_save);
    _attPopup_cmdlist->add(_att_select_all);
    _menuPopupAtt = new MenuBar(_my_editor->attachArea()->getClipWindow(), 
					"RoamAttachmentPopup", XmMENU_POPUP);
    _attachmentPopupMenu = _menuPopupAtt->addCommands(_attPopup_cmdlist, 
				FALSE, XmMENU_POPUP);
}

void
RoamMenuWindow::construct_help_menu()
{
    FORCE_SEGV_DECL(CmdList, cmdList);

    // Separator for menu items
    
    _help_separator= new SeparatorCmd( "Separator","Separator", TRUE );
    _help_overview = new OnAppCmd("Overview", GETMSG(DT_catd, 1, 71, "Overview"), 
				TRUE, this);
    _help_tasks = new TasksCmd("Tasks", GETMSG(DT_catd, 1, 72, "Tasks"), 
				TRUE, this);
    _help_reference = new ReferenceCmd("Reference",
				GETMSG(DT_catd, 1, 73, "Reference"), 
				TRUE, this);
    _help_on_item = new OnItemCmd("On Item", GETMSG(DT_catd, 1, 74, "On Item"), 
				TRUE, this);
    _help_using_help = new UsingHelpCmd("Using Help",
				GETMSG(DT_catd, 1, 75, "Using Help"), 
				TRUE, this);
    cmdList = new CmdList( "Help", GETMSG(DT_catd, 1, 76, "Help") );
    _help_cmdlist = cmdList;

    cmdList->add ( _help_overview );
    cmdList->add ( _help_separator );
    cmdList->add ( _help_tasks );
    cmdList->add ( _help_reference );
    cmdList->add ( _help_separator );
    cmdList->add ( _help_on_item );
    cmdList->add ( _help_separator );
    cmdList->add ( _help_using_help );
    cmdList->add ( _help_separator );

    _help_about_mailer = new RelNoteCmd("About Mailer...", 
				    GETMSG(DT_catd, 1, 77, "About Mailer..."),
    				    TRUE, this);
    cmdList->add ( _help_about_mailer );

    // Make help menu show up on right side of menubar.
    _menuBar->addCommands ( cmdList, TRUE );
    
}
 
void
RoamMenuWindow::syncCachedContainerList(void)
{
    RoamMenuWindow	*rmw = NULL;

    rmw = theRoamApp.nextRoamMenuWindow(NULL);
    if (NULL == rmw)
      return;

    // Sync the cached lists to an existing RoamMenuWindow.
    for (int i = rmw->_cached_containerlist->length(); i > 0; i--) {
	char *name = (*(rmw->_cached_containerlist))[i-1]->containerName();
	addToCachedContainerList(name);
    }
}

DtMail::MailRc *
RoamMenuWindow::get_mail_rc()
{
    DtMailEnv error;

    if (NULL == _mailbox) return NULL;
    return _mailbox->session()->mailRc(error);
}


void
RoamMenuWindow::load_mailbox(
    DtMailEnv & mail_error
)
{
    int		count = 0;
    
    // If there is no mailbox, return.
    if (!_mailbox) {
	mail_error.clear();
	return;
    }

    // Now load the messages into the scrolling list.
    // This will get the DtMailMessageHandles into the _msgs array and
    // it will also get their XmStrings into the CharArray of the _list.
    theRoamApp.busyAllWindows(GETMSG(DT_catd, 3, 27, "Loading..."));
	
    // Call load_headers() to get the XmStrings into the XmList!

    count = _list->load_headers(mail_error);
    
    if (mail_error.isSet()) {
	// Return whatever error mailbox->get_next_msg() returned.
	theRoamApp.unbusyAllWindows();
	return;
    }

    // If no messages

    if (count == 0) {
	this->message(GETMSG(DT_catd, 3, 28, "Empty container"));
	setIconName(EmptyIcon);
    }

    _list->sort_messages();

    theRoamApp.unbusyAllWindows();
}

void
RoamMenuWindow::clear_message()
{
    this->message("");
}

void
RoamMenuWindow::text_selected()
{

    // turn on sensitivity for Cut/Clear/Copy/Paste/Delete

}

void
RoamMenuWindow::text_unselected()
{

    // turn off sensitivity for those items

}

    // syncViewAndStore() does the sync-ing of the view of a mail
    // container and the storage of that container.
    // This method gets invoked every time a message gets expunged
    // by the back end based on "timed delete".
    // The method needs to then remove the expunged message from the
    // deleted messages list, thereby syncing the view to be always
    // current with the storage.
    // Similarly, the method also gets invoked when the container store
    // has received new mail.  The view then needs to be updated....


DtMailBoolean
RoamMenuWindow::syncViewAndStoreCallback(
    DtMailCallbackOp op,
    const char *path,
    const char *,	// prompt_hint
    void *client_data,	
    ...
)
{

    DtMailBoolean bval;
    va_list args;

    va_start(args, client_data);
    RoamMenuWindow * rmw = (RoamMenuWindow *)client_data;

    bval = rmw->syncViewAndStore(op, path, args);
    return(bval);
}

DtMailBoolean
RoamMenuWindow::syncViewAndStore(
    DtMailCallbackOp op,
    const char *path,
    va_list args
)
{

    DtMailEnv error;
    DtMailMessageHandle	tmpMH;
    char *errmsg;
    int answer;


    this->busyCursor();

    switch(op) {
      case DTMC_NEWMAIL:

	// New mail has come in.  Load it in and update the
	// view's list of headers to also display it.

	tmpMH = va_arg(args, DtMailMessageHandle);
	this->_list->load_headers(error, tmpMH);

	newMailIndicators();

	break;

      case DTMC_DELETEMSG:

	// A message has been expunged from the store by the back end.  
	// The expunge has been done based on "timed delete". 
	// Remove the expunged message from the displayed list of
	// message headers. 
	break;

      case DTMC_BADSTATE:

	errmsg = GETMSG(DT_catd, 1, 238, "Mailer is confused about the state of this mailbox.\nIt may be that another process has rewritten this mailbox in an unexpected way.\n\nSelecting OK will cause the Mailer to close and reopen the mailbox.\nNOTE that any changes made to the mailbox since the last save may be lost.");
	this->_genDialog->setToErrorDialog(
				GETMSG(DT_catd, 2, 10, "Mailer"),
				errmsg);
	answer = this->_genDialog->post_and_return(
				GETMSG(DT_catd, 3, 29, "OK"),
				DTMAILHELPUNKNOWNSTATE);

	this->reopen_mail_file();
	if (DTM_TRUE)
	  return(DTM_TRUE);
	break;

      case DTMC_ACCESSFAILED:
	
	errmsg = GETMSG(DT_catd, 1, 239, "Mailer can no longer access this mailbox.\nIt may be that another process has deleted the mailbox file.\n\nSelecting OK will cause the Mailer to close and reopen the mailbox.\nNOTE that any changes made to the mailbox since the last save may be lost.\n\nSelecting CANCEL will leave the mailbox unchanged.");
	this->_genDialog->setToErrorDialog(
				GETMSG(DT_catd, 2, 10, "Mailer"),
				errmsg);
	answer = this->_genDialog->post_and_return(
				GETMSG(DT_catd, 3, 29, "OK"),
				GETMSG(DT_catd, 3, 19, "Cancel"),
				DTMAILHELPUNKNOWNSTATE);
	this->normalCursor();
	if (answer == 1)
	{
	    this->reopen_mail_file();
	    return(DTM_TRUE);
	}
	else
	  return(DTM_FALSE);
	break;

      case DTMC_SERVERPASSWORDNEEDED:
	
	{
	    char *buffer = new char[2048];
	    char *errmsgarg =	va_arg(args, char*);

	    sprintf(buffer, "%s", errmsgarg);
	    this->_genDialog->setToTextFieldDialog(
				GETMSG(DT_catd, 2, 10, "Mailer"),
				buffer, DTM_TRUE);
	    answer = this->_genDialog->post_and_return(
				GETMSG(DT_catd, 3, 29, "Ok"),
				GETMSG(DT_catd, 3, 19, "Cancel"),
				DTMAILHELPUNKNOWNSTATE);
	    this->normalCursor();
	    delete [] buffer;
	    if (answer == 1)
	    {
		char		*password = NULL;
                DtMailEnv	mail_error;
		password = _genDialog->getTextFieldValue();
		_mailbox->updateMailRetrievalPassword(password);
		_mailbox->checkForMail(error);
		if (NULL != password) free(password);
	        return(DTM_TRUE);
	    }
	    else
	      return(DTM_FALSE);
	}
	break;

      case DTMC_SERVERACCESSFAILED:
	
	{
	    char *buffer = new char[2048];
	    char *errmsgarg =	va_arg(args, char*);

	    errmsg = GETMSG(DT_catd, 1, 256, "Mail server access failed:\n%s");

	    sprintf(buffer, errmsg, errmsgarg);
	    this->_genDialog->setToErrorDialog(
				GETMSG(DT_catd, 2, 10, "Mailer"),
				buffer);
	    answer = this->_genDialog->post_and_return(
				GETMSG(DT_catd, 1, 257, "Options..."),
				GETMSG(DT_catd, 3, 19, "Cancel"),
				DTMAILHELPUNKNOWNSTATE);
	    this->normalCursor();
	    delete [] buffer;
	    if (answer == 1)
	    {
		OptCmd	*optCmd = (OptCmd *) theRoamApp.mailOptions();
		optCmd->displayMailRetrievalOptionsPane();
	        return(DTM_TRUE);
	    }
	    else
	      return(DTM_FALSE);
	}
	break;

      case DTMC_GETMAILCOMMANDFAILED:
	
	{
	    char *buffer = new char[2048];
	    char *errmsgarg =	va_arg(args, char*);

	    errmsg =
	      GETMSG(DT_catd, 1, 258, "User Getmail command failed:\n%s");

	    sprintf(buffer, errmsg, errmsgarg);
	    this->_genDialog->setToErrorDialog(
				GETMSG(DT_catd, 2, 10, "Mailer"),
				buffer);
	    answer = this->_genDialog->post_and_return(
				GETMSG(DT_catd, 1, 257, "Options..."),
				GETMSG(DT_catd, 3, 19, "Cancel"),
				DTMAILHELPUNKNOWNSTATE);
	    this->normalCursor();
    	    delete [] buffer;
	    if (answer == 1)
	    {
		OptCmd	*optCmd = (OptCmd *) theRoamApp.mailOptions();
		optCmd->displayMailRetrievalOptionsPane();
	        return(DTM_TRUE);
	    }
	    else
	      return(DTM_FALSE);
	}
	break;

      case DTMC_SERVERACCESSINFO:
	
	{
	    char *infomsg = va_arg(args, char*);
	    message(infomsg);
	    return DTM_TRUE ;
	}
	break;

      case DTMC_SERVERACCESSINFOERROR:
	
	{
	    char *buffer = new char[2048];
	    char *errmsgarg =	va_arg(args, char*);

	    sprintf(buffer, "%s", errmsgarg);
	    this->_genDialog->setToErrorDialog(
				GETMSG(DT_catd, 2, 10, "Mailer"),
				buffer);
	    answer = this->_genDialog->post_and_return(
				GETMSG(DT_catd, 1, 257, "Options..."),
				GETMSG(DT_catd, 3, 19, "Cancel"),
				DTMAILHELPUNKNOWNSTATE);
	    this->normalCursor();
	    delete [] buffer;
	    if (answer == 1)
	    {
		OptCmd	*optCmd = (OptCmd *) theRoamApp.mailOptions();
		optCmd->displayMailRetrievalOptionsPane();
	        return(DTM_TRUE);
	    }
	    else
	      return(DTM_FALSE);
	}
	break;

      case DTMC_INODECHANGED:
	
	errmsg = GETMSG(DT_catd, 1, 240, "Mailer can no longer access this mailbox.\nAnother process has rewritten the mailbox file changing the inode.\n\nSelecting OK will cause the Mailer to close and reopen the mailbox.\nNOTE that any changes made to the mailbox since the last save may be lost.\n\nSelecting CANCEL will leave the mailbox unchanged.");

	this->_genDialog->setToErrorDialog(
				GETMSG(DT_catd, 2, 10, "Mailer"),
				errmsg);
	answer = this->_genDialog->post_and_return(
				GETMSG(DT_catd, 3, 29, "OK"),
				GETMSG(DT_catd, 3, 19, "Cancel"),
				DTMAILHELPUNKNOWNSTATE);
	this->normalCursor();
	if (answer == 1)
	{
	    this->reopen_mail_file();
	    return(DTM_TRUE);
	}
	else
	  return(DTM_FALSE);
	break;

      case DTMC_UNLOCK:
	
	// We are asked to save changes and close the file.
	// The backend will take care of unlocking the file.
	this->quit_silently();
	return(DTM_TRUE);

      case DTMC_QUERYLOCK:

	// The file is lock by another mailer.
	// Should we ask for the lock?
	_genDialog->setToQuestionDialog(GETMSG(DT_catd, 3, 16, "Mailer"),
	   GETMSG(DT_catd, 3, 82, "Someone else is using this mailbox.\nWould you like to demand exclusive access?"));
	answer = _genDialog->post_and_return(GETMSG(DT_catd, 3, 18, "OK"),
	   GETMSG(DT_catd, 3, 19, "Cancel"), DTMAILHELPTAKELOCK);

	this->normalCursor();
	if (answer == 1) {
	  return(DTM_TRUE);
	} else {
	  return(DTM_FALSE);
	}

    case DTMC_READONLY:

      // We are not able to obtain a TT lock on this folder,
      // ask user if he wants to open it as read only.
      _genDialog->setToQuestionDialog(GETMSG(DT_catd, 3, 16, "Mailer"),
				      GETMSG(DT_catd, 3, 83, "Mailer is unable to obtain exclusive access to this mailbox.\nWould you like to open this mailbox read-only?"));
	answer = _genDialog->post_and_return(GETMSG(DT_catd, 3, 18, "OK"),
					     GETMSG(DT_catd, 3, 19, "Cancel"),
					     DTMAILHELPOPENREADONLY);
	this->normalCursor();
	if (answer == 1) {
	  return(DTM_TRUE);
	} else {
	  return(DTM_FALSE);
	}

    case DTMC_READWRITEOVERRIDE:
      // We are not able to obtain a lock on this folder,
      // ask user if he wants to open it as read only.
      _genDialog->setToQuestionDialog(GETMSG(DT_catd, 3, 16, "Mailer"),
				      GETMSG(DT_catd, 3, 94, "Mailer is unable to obtain exclusive access to this\nmailbox because the system is not responding.\n\nFor this time only, you can choose to open this mailbox\nread-only, or to open it read-write without exclusive\naccess (use only if no one else is using this mailbox)."));
	answer = _genDialog->post_and_return(GETMSG(DT_catd, 3, 95, "Read-only"),
					     GETMSG(DT_catd, 3, 19, "Cancel"),
					     GETMSG(DT_catd, 3, 96, "Read-Write"), 
					     DTMAILHELPOPENREADWRITEOVERRIDE);
	this->normalCursor();
	// Now the tricky part - since this method can only return 
	// DTM_TRUE or DTM_FALSE, we must have a way to indicate "readOnly",
	// "readWrite" or "cancel" - horrid hack: readOnly is DTM_FALSE, 
	// "readWrite" is DTM_TRUE, and "cancel" is (DTM_TRUE+DTM_FALSE)*2
	//
	switch (answer)
	  {
	  case 1:	// Read-only
	    return(DTM_FALSE);
	  case 3:	// Read-Write
	    return(DTM_TRUE);
	  default:	// cancel
	    return((DtMailBoolean)((DTM_FALSE+DTM_TRUE)*2));
	  }

    case DTMC_DOTDTMAILLOCKFAILED:

      {
        char *buffer		= new char[2048];
	char *lockpath		= va_arg(args, char*);
	char *errormsg		= va_arg(args, char*);
	char *cancel		= GETMSG(DT_catd, 3, 19, "Cancel");
        char *ok		= GETMSG(DT_catd, 3, 18, "OK");
        char *mailer		= GETMSG(DT_catd, 3, 16, "Mailer");
        char *msgfmt		= GETMSG(DT_catd, 3, 97, "%s\n\nThis may indicate that another Mailer has opened\n%s\n\nYou may open this mailbox READ ONLY by choosing '%s'.\n\nOtherwise, you may choose '%s',\nmake sure that another Mailer has not opened this mailbox,\ndelete %s,\nand reopen this mailbox.");

        // We are not able to obtain a .dtmail lock on this folder,
        // ask user if he wants to open it as read only.
	sprintf(buffer, msgfmt, errormsg, path, ok, cancel, lockpath);
        _genDialog->setToQuestionDialog(mailer, buffer);
	answer =_genDialog->post_and_return(ok, cancel, DTMAILHELPOPENREADONLY);
	this->normalCursor();
	delete [] buffer;
	if (answer == 1) {
	  return(DTM_TRUE);
	} else {
	  return(DTM_FALSE);
	}
      }

      default:
	this->normalCursor();
	return(DTM_FALSE);
    }

    this->normalCursor();
    return(DTM_TRUE);
}

// Convert the rfc file.
// Commented out for PAR 5.  Needs to be uncommented out for PAR 6
// and above.

void
RoamMenuWindow::convert(
    char *,		// src
    char *		// dest
)
{
    // Obsoleted when Bento went away
}

// The back end uses the return value to either continue conversion or
// to stop converting.

int
RoamMenuWindow::ConvertStatusCB(
    int current, 
    int total, 
    void * client_data
)
{

    RoamMenuWindow *rmw = (RoamMenuWindow *)client_data;

    // Return 0 if the conversion is still to proceed.
    // Return 1 if the conversion is to stop (e.g., if the user
    // has interrupted it...)

    return(rmw->showConversionStatus(current, total));
}

int
RoamMenuWindow::showConversionStatus(
    int current, 
    int total
)
{
    char *buf, *str;
    int num_already_converted;  // num msgs already converted as set prev.
    int previously_complete;  // percent of converted msgs as set prev.
    int now_complete;  // percent of converted messages based on parameters

    // Remember,  the number set may differ from the previous call to
    // this method.  We are not doing set_convert_data() for every call
    // to this method and so, what's set will differ from the previous
    // call to this method...

    num_already_converted = _convertContainerCmd->get_num_converted();
    previously_complete = num_already_converted * 100 / total;

    now_complete = current * 100 / total;

    if ((now_complete > (previously_complete + 5)) ||
	(now_complete == 100)) {
	str = GETMSG(DT_catd, 1, 79, "Converting... %d percent complete"); 
	// Estimate 4 characters for numbers and 1 null terminator.
	buf = new char[strlen(str) + 5];   
	sprintf(buf, str, now_complete);
	
	_convertContainerCmd->updateDialog(buf);
	delete [] buf;

	_convertContainerCmd->set_convert_data(current, total);
    }
    else {
	_convertContainerCmd->updateAnimation();
    }

    // Check and see if the user had interrupted the conversion
    // If the user had interrupted, we need to stop the back-end
    // convert...

    if (_convertContainerCmd->interrupted()) {
	return(1);
    }
    else {
	return(0);
    }
}

    
void
RoamMenuWindow::conversionFinishedCallback( 
    RoamInterruptibleCmd *,
    Boolean interrupted,
    void	     *clientData
)
{

    // Do something only it the conversion really finished.
    // If it was interrupted, just return...

    if (!interrupted) {
	RoamMenuWindow *rmw = (RoamMenuWindow *) clientData;
	rmw->conversionFinished();
    }
}

// If the conversion finished successfully, then we try to open() again
// and if no problems, load the headers into the scrolling list...

void
RoamMenuWindow::conversionFinished()
{
    DtMailEnv mail_error;

    // Initialize the mail_error.
    mail_error.clear();

    mailboxFullpath(_convertContainerCmd->get_destination_name());

    this->open(mail_error, 
	       _openContainerCmd->_open_create_flag,
	       _openContainerCmd->_open_lock_flag);
    
    // if the user had cancelled the open, then the RMW will not have
    // its mailbox set. Do not proceed to load the mailbox.  Return.

    // Ideally, open() should set the mail_error if the user cancels
    // the open.  And we should check the mail_error here...

    if (mail_error.isSet()) {
	return;
    }

    this->load_mailbox(mail_error);

    if (mail_error.isSet()) {

	// Post a dialog indicating problems in loading...
    }

}


// Post the dialog displaying the error text.
// Also display the minor_code, if resource/environment variable
// wants it.
// User has no choice but to OK
// postErrorDialog() is the benign uncle of postFatalErrorDialog() --
// it does not quit the RMW.

void
RoamMenuWindow::postErrorDialog(
    DtMailEnv &mail_error
)
{
    char *helpId;

    const char *text_str = (const char *)mail_error;

    // Assume that the char *text that comes in has already 
    // been processed for i18n.

    _genDialog->setToErrorDialog(GETMSG(DT_catd, 2, 13, "Mailer"), 
				 (char *)text_str);
    
    helpId = DTMAILHELPERROR;
    int i = _genDialog->post_and_return(GETMSG(DT_catd, 3, 31, "OK"), helpId);

    // Clear the Error
    mail_error.clear();

}


void
RoamMenuWindow::manage()
{
    Dimension win_x, win_y, win_wid, win_ht, win_bwid;

    MainWindow::manage();
    XmUpdateDisplay(this->baseWidget());
    // Obtain the current dimensions of the RMW

    XtVaGetValues(_w,
		  XmNx, &win_x,
		  XmNy, &win_y,
		  XmNwidth, &win_wid,
		  XmNheight, &win_ht,
		  XmNborderWidth, &win_bwid,
		  NULL);

    _x = win_x;
    _y = win_y;
    _width = win_wid;
    _height = win_ht;
    _border_width = win_bwid;

}

void
RoamMenuWindow::expunge(void)
{
    DtMailEnv error;
    error.clear();
    
    // First order of business - busy out active windows
    //
    theRoamApp.busyAllWindows(
	GETMSG(DT_catd, 3, 32, "Destroying deleted messages..."));

    // Before changing the state of any deleted/undeleted lists,
    // perform the destroy deleted operation and make sure that
    // it was successful - if there was an error, notify the user
    // and discontinue processing
    //
    _mailbox->expunge(error);
    if((DTMailError_t) error == DTME_OutOfSpace )
    {
       stopAutoSave();
       ShowErrMsg((char *)error.getClient(),FALSE,(void *)this );
       error.setClient(NULL);
       startAutoSave();
       theRoamApp.unbusyAllWindows();
       return;
    }

    if (error.isSet()) {
	// An error happened - must inform the user
	//
	postErrorDialog(error);
	error.clear();
    }
    else {
	if (_msg_undelete_from_list->dialog())
	  _msg_undelete_from_list->dialog()->expunge();
	
	_list->expunge();
    }

    // Last order of business - unbusy out active windows
    //
    theRoamApp.unbusyAllWindows();
}

void
RoamMenuWindow::attachment_selected()
{
    _att_save->activate();
}


void
RoamMenuWindow::all_attachments_selected()
{
    _att_save->deactivate();

    if (_attActions_cmdlist != NULL) {
	_menuBar->removeCommands(_attachmentMenu, _attActions_cmdlist);
	_menuPopupAtt->removeCommands(_attachmentPopupMenu, 
			_attActions_cmdlist);
	delete _attActions_cmdlist;
	_attActions_cmdlist = NULL;
    }
}

void
RoamMenuWindow::all_attachments_deselected()
{
    _att_save->deactivate();

    if (_attActions_cmdlist != NULL) {
	_menuBar->removeCommands(_attachmentMenu, _attActions_cmdlist);
	_menuPopupAtt->removeCommands(_attachmentPopupMenu, 
				_attActions_cmdlist);
	delete _attActions_cmdlist;
	_attActions_cmdlist = NULL;
    }
}

void
RoamMenuWindow::activate_default_message_menu()
{
    
    _msg_open->activate();

    _msg_save_as->activate();

    XtSetSensitive(_copyto_cmdlist->getPaneWidget(), TRUE);

    _msg_print->activate();

    _msg_delete->activate();

}

void
RoamMenuWindow::deactivate_default_message_menu()
{
    
    _msg_open->deactivate();

    _msg_save_as->deactivate();

    XtSetSensitive(_copyto_cmdlist->getPaneWidget(), FALSE);

    _msg_print->deactivate();

    _msg_delete->deactivate();

}  

void
RoamMenuWindow::activate_default_attach_menu()
{
    _att_select_all->activate();
}

void
RoamMenuWindow::deactivate_default_attach_menu()
{
    _att_select_all->deactivate();
}

void
RoamMenuWindow::newMailIndicators(void)
{
    // Set to new mail icon only if the window is iconified
    if (this->isIconified()) {
	setIconName(NewMailIcon);
    }

    if ((_we_called_newmail == FALSE) || (this->isIconified())){

	// See if we are supposed to ring the bell.
	//
	DtMailEnv error;
	const char * val = NULL;
	DtMail::Session * m_session = theRoamApp.session()->session(); 
	m_session->mailRc(error)->getValue(error, "flash", &val);
	if (error.isNotSet()) {
	    int flashes = (int) strtol(val, NULL, 10);
	    flash(flashes);
	}
        if (NULL != val)
          free((void*) val);

	val = NULL;
	m_session->mailRc(error)->getValue(error, "bell", &val);
	if (error.isNotSet()) {
	    int beeps = (int) strtol(val, NULL, 10);
	    while(beeps > 0) {
		XBell(XtDisplay(baseWidget()), 0);
		beeps -= 1;
	    }
	}
	else // Default to 1 beep
	    XBell(XtDisplay(baseWidget()), 0);

        if (NULL != val)
          free((void*) val);

	val = NULL;
	m_session->mailRc(error)->getValue(error, "realsound", &val);
	if (error.isNotSet()) {
	    struct stat stat;
	    if (SafeStat("/usr/bin/audioplay", &stat) == 0) {
		char *play_str = new char[1500];
		sprintf(play_str, "/usr/bin/audioplay %s", val);
		system(play_str);
		delete [] play_str;
	    }
	}

        if (NULL != val)
          free((void*) val);
    }

    _we_called_newmail = FALSE;
}

void
RoamMenuWindow::save_attachment_callback(
    void *client_data,
    char *selection
)
{

    RoamMenuWindow *obj = (RoamMenuWindow *) client_data;
    obj->save_selected_attachment(selection);
}

void
RoamMenuWindow::save_selected_attachment(
    char *selection
)
{
    DtMailEnv mail_error;

    // Initialize the mail_error.
    mail_error.clear();

    DtMailEditor *editor = this->get_editor();
    AttachArea *attacharea = editor->attachArea();
    Attachment *attachment = attacharea->getSelectedAttachment();

   // Get selected attachment, if none selected, then return.
   if ( attachment == NULL ) {
	  // Let User know that no attachment has been selected???
	  int answer = 0;
	  char *helpId = NULL;


	  _genDialog->setToErrorDialog(
		    GETMSG(DT_catd, 1, 80, "Mailer"),		  
		    GETMSG(DT_catd, 2, 14, "An attachment needs to be selected before issuing the\n\"Save As\" command to save to a file.") );
	  helpId = DTMAILHELPSELECTATTACH;
	  answer = _genDialog->post_and_return(
			GETMSG(DT_catd, 3, 33, "OK"), helpId );
	  return;
      }


    attachment->saveToFile(mail_error, selection);

    if (mail_error.isSet()) {
	// do something
    }

}

void
RoamMenuWindow::addAttachmentActions(
    char **actions,
    int indx
)
{
    int i;
    char *anAction;
    AttachmentActionCmd *attachActionCmd;
    
    if (_attActions_cmdlist == NULL) { 
	_attActions_cmdlist = new CmdList("AttachmentActions", "AttachmentActions");
    }
    else {
	_menuBar->removeCommands(_attachmentMenu, _attActions_cmdlist);
	_menuPopupAtt->removeCommands(_attachmentPopupMenu, 
				_attActions_cmdlist);
	delete _attActions_cmdlist;
	_attActions_cmdlist = new CmdList("AttachmentActions", "AttachmentActions");
    }

    char *actionLabel;
    for (i = 0; i < indx; i++) {
	anAction = actions[i];

	// Retrieve the localized action label
	actionLabel = DtActionLabel(anAction);
	attachActionCmd = new AttachmentActionCmd(
					anAction, 
					actionLabel,
					this,
					i);
	_attActions_cmdlist->add(attachActionCmd);
	
    }
    _attachmentMenu = _menuBar->addCommands(
				_attachmentMenu, 
				_attActions_cmdlist
			);
    _attachmentPopupMenu = _menuPopupAtt->addCommands(
				_attachmentPopupMenu, 
				_attActions_cmdlist
			);
}

void
RoamMenuWindow::removeAttachmentActions()
{

    // Stubbed out for now
}

void
RoamMenuWindow::invokeAttachmentAction(
    int index
)
{

    DtMailEditor *editor = this->get_editor();
    AttachArea *attacharea = editor->attachArea();
    Attachment *attachment = attacharea->getSelectedAttachment();

    attachment->invokeAction(index);

}

void
RoamMenuWindow::selectAllAttachments()
{
    
    DtMailEditor *editor = this->get_editor();
    AttachArea *attachArea = editor->attachArea();
    
    attachArea->selectAllAttachments();

}

// Returns the index of the match, -1 if there is no match.
int
RoamMenuWindow::inList(char *filename, DtVirtArray<ContainerMenuCmd *> *list)
{
    ContainerMenuCmd *cmd;

    if (list == NULL) return(-1);

    if (filename &&
	*filename != '/' &&
	*filename != '.' && 
	*filename != '$' &&
	*filename != '~')
    { // relative path
	
	DtMail::Session * d_session = theRoamApp.session()->session();
    	DtMailEnv error;
    	DtMail::MailRc * mailrc = d_session->mailRc(error);
    	const char *value = NULL;
	char *newname;

    	mailrc->getValue(error, "folder", &value);
	if (error.isNotSet())
	{
	    newname = (char*)malloc(strlen(filename) + strlen(value) + 2);
	    sprintf(newname, "%s/%s", value, filename);
    	    for (int i=0; i < list->length(); i++)
	    {
		cmd = (*list)[i];
		if (strcmp(newname, cmd->containerName()) == 0) {
	 	    free(newname);
	    	    return(i);
		}
    	    }
	    free((void*) newname);
	}

        if (NULL != value)
          free((void*) value);
    }

    for (int i=0; i < list->length(); i++)
    {
	cmd = (*list)[i];
	if (strcmp(filename, cmd->containerName()) == 0)
	  return(i);
    }
    return(-1);
}


void
RoamMenuWindow::addToCachedContainerList(char *filename)
{
    int			i, index;
    ContainerMenuCmd	*null_container, *open_container;
    ContainerMenuCmd	*move_container, *copy_container;
    char		*name;

    if (filename != NULL &&
	*filename != '\0' &&
	(_max_cached_list_size > 0))
    {
        DtMail::Session	*d_session = theRoamApp.session()->session();
        DtMailObjectSpace space;
	DtMailEnv	mail_error;
	int		is_inbox;
	char		*mail_file = NULL;

	// Is the file in the user defined list?
	if ((index = inList(filename, _user_containerlist)) != -1)
	  return;

	d_session->queryImpl(
			mail_error,
			d_session->getDefaultImpl(mail_error),
			DtMailCapabilityInboxName,
			&space,
			&mail_file);

        is_inbox = (0 == strcmp(mail_file, filename));
	if (NULL != mail_file)
	  free((void*) mail_file);
	if (is_inbox)
	  return;

	// Is the file in the recently used list?
	if ((index = inList(filename, _cached_containerlist)) != -1)
	{
	    // Move filename to top of list and move everything else down.
	    if (index == 0) return;

	    //
	    // Change the filenames that each of the Cmds points to in both
	    // the move and copy cached lists.
	    //
	    name = (*_cached_containerlist)[index]->containerName();

	    for (i = index; i > 0; i--) {
		char	*s = (*_cached_containerlist)[i-1]->containerName();

		(*_cached_containerlist)[i]->changeContainer(s);
		(*_open_container_containerlist_cached)[i]->changeContainer(s);
		(*_copyto_containerlist_cached)[i]->changeContainer(s);
		(*_move_containerlist_cached)[i]->changeContainer(s);
	    }

	    (*_cached_containerlist)[0]->changeContainer(name);
	    (*_open_container_containerlist_cached)[0]->changeContainer(name);
	    (*_copyto_containerlist_cached)[0]->changeContainer(name);
	    (*_move_containerlist_cached)[0]->changeContainer(name);

	    //
	    // Rename the labels in the menu.
	    //
	    if (_first_cached_item != _first_cached_item + index)
	    {
	        _menuBar->rotateLabels(
				_opencontainerMenu,
				_first_cached_item,
				_first_cached_item + index);
	        _menuBar->rotateLabels(
				_copytoMenu,
				_first_cached_item,
				_first_cached_item + index);
	        _menuBar->rotateLabels(
				_moveMenu,
				_first_cached_item,
				_first_cached_item + index);
	        _menuBar->rotateLabels(
				_msgsPopupMoveMenu,
				_first_cached_item,
				_first_cached_item + index);
	    }
	 }
	 else
	 {
	    // Add filename to Recently Used List.
	    int	cached_list_size = _cached_containerlist->length();

	    // Is there room for the menu to grow?
	    if (cached_list_size < _max_cached_list_size)
	    {
		// Create the new command.
		null_container = new ContainerMenuCmd(strdup(filename),
							filename,
							TRUE,
							this,
							DTM_NONE);
		open_container = new ContainerMenuCmd(strdup(filename),
							filename,
							TRUE,
							this,
							DTM_OPEN);
		copy_container = new ContainerMenuCmd(strdup(filename),
							filename,
							TRUE,
							this,
							DTM_COPY);
		move_container = new ContainerMenuCmd(strdup(filename),
							filename,
							TRUE,
							this,
							DTM_MOVE);

		// Add it to the end of the arrays.
		_cached_containerlist->append(null_container);
		_open_container_containerlist_cached->append(open_container);
		_copyto_containerlist_cached->append(copy_container);
		_move_containerlist_cached->append(move_container);

		// Add it to the end of the menus.
		_menuBar->addCommand(_opencontainerMenu, open_container);
		_menuBar->addCommand(_copytoMenu, copy_container);
		_menuBar->addCommand(_moveMenu, move_container);
		_menuBar->addCommand(_msgsPopupMoveMenu, move_container);

	        // Recursively call addToCachedContainerList to rotate to top.
	        cached_list_size = _cached_containerlist->length();
		if (cached_list_size > 1)
		  addToCachedContainerList(filename);
	    }
	    else
	    {
		// Add new entry to the bottom of the cache.
	        int	i = cached_list_size-1;
		char	*s = filename;
		
		// Replace the end of the arrays.
		(*_cached_containerlist)[i]->changeContainer(strdup(s));
		(*_open_container_containerlist_cached)[i]->changeContainer(
								strdup(s));
		(*_copyto_containerlist_cached)[i]->changeContainer(strdup(s));
		(*_move_containerlist_cached)[i]->changeContainer(strdup(s));

		// Replace the end of the menus.
		i += _first_cached_item;
		_menuBar->changeLabel(_opencontainerMenu, i, s);
		_menuBar->changeLabel(_copytoMenu, i, s);
		_menuBar->changeLabel(_moveMenu, i, s);
		_menuBar->changeLabel(_msgsPopupMoveMenu, i, s);

	        // Recursively call addToCachedContainerList to rotate to top.
		if (_first_cached_item != i)
		  addToCachedContainerList(filename);
	    }
	}
    }
}

void
RoamMenuWindow::showAttachArea()
{
    DtMailEditor *editor = this->get_editor();
    editor->showAttachArea();
}

void
RoamMenuWindow::hideAttachArea()
{
    DtMailEditor *editor = this->get_editor();
    editor->hideAttachArea();
}

void
RoamMenuWindow::fullHeader(
    Boolean state
)
{
    DtMailMessageHandle msgHandle;
    DtMailEnv mail_error;

    // Initialize the mail_error.
    mail_error.clear();

    _full_header_resource = state;
    
    msgHandle = this->list()->current_msg_handle();
    if (msgHandle) {
	this->list()->display_message(mail_error, msgHandle);
    }

}

void 
RoamMenuWindow::postMsgsPopup(XEvent *event)
{
	XmMenuPosition(_msgsPopupMenu, (XButtonEvent *)event);
	XtManageChild(_msgsPopupMenu);
}

void
RoamMenuWindow::MenuButtonHandler(
    Widget ,
    XtPointer cd,
    XEvent *event,
    Boolean *)
{
	RoamMenuWindow *obj = (RoamMenuWindow *)cd;

	if(event->xany.type != ButtonPress)
		return;

	XButtonEvent *be = (XButtonEvent *)event;

	if(be->button == theApplication->bMenuButton())
		obj->postMsgsPopup(event);
}

void
RoamMenuWindow::attachmentFeedback(
    Boolean bval
)
{
    if (bval) {
	this->busyCursor();
    }
    else {
	this->normalCursor();
    }
}

// map_menu is used to figure out how many columns to split the menu
// into.  It is a callback that is called when the menu is mapped.
// If the menu is over half the height of the screen, it figures out
// how many columns to make the menu, and sets its XmNnumColumns
// attribute to that value.  It calculates the maximum number of columns
// that would fit and never goes beyond that number.

void
RoamMenuWindow::map_menu(
		    Widget menu, 
		    XtPointer,
		    XtPointer)
{
        Position y;
        Dimension h, w;
        short   maxcols, newcols, columns;
        int     screenheight = HeightOfScreen(XtScreen(menu));
        int     fudgefact = 20; /* to allow for decorations on menu */
 
        XtVaGetValues(menu,
        XmNheight, &h,
        XmNwidth, &w,
        XmNy, &y,
        XmNnumColumns, &columns,
        NULL);
 
 
        if ((int) (h + fudgefact) > ((int) screenheight / 2)) {
 
        /* the menu is taller than half the screen.  We need to find out how
           many more columns to specify for the menu to make it fit. */
 
                newcols = (columns * (int) ((int) (h + fudgefact)/(int) (screenheight/2))) + 1;
                maxcols = WidthOfScreen(XtScreen(menu))/(int) ((int)w/(int)columns);
 
                if (newcols > maxcols)
                        newcols = maxcols;
 
                XtVaSetValues(menu, XmNnumColumns, newcols, NULL);

        }
 

}


void
RoamMenuWindow::setTitle(char *suffix)
{
    DtMailEnv		mail_error;
    MailSession		*ses = theRoamApp.session();
    DtMail::Session	*d_session = ses->session();
    char *prefix = GETMSG(DT_catd, 1, 6, "Mailer");
    char *path = NULL;
    char *new_title;
    char *format;
    int   len;

    if (mailbox_fullpath())
      path = d_session->getRelativePath(mail_error, mailbox_fullpath());
    else if (mailboxName())
      path = d_session->getRelativePath(mail_error, mailboxName());
    else path = strdup("UNTITLED");

    if (suffix)
    {
	format = "%s - %s [%s]";
	len = strlen(format) + strlen(prefix) + strlen(path) + strlen(suffix);
	new_title = new char[len];
    	sprintf(new_title, format, prefix, path, suffix);
    }
    else
    {
	format = "%s - %s";
	len = strlen(format) + strlen(prefix) + strlen(path);
	new_title = new char[len];
    	sprintf(new_title, format, prefix, path);
    }

    title(new_title);
    delete [] new_title; 
    free(path);
}

void
RoamMenuWindow::setVacationTitle()
{
    // Add "[Vacation]" to the title of the roam menu window 
    char *vacation = GETMSG(DT_catd, 1, 3, "Vacation");
    setTitle(vacation);
}

void
RoamMenuWindow::removeVacationTitle()
{
    // Reset the title on the roam menu window; take out "[Vacation]"
    setTitle(NULL);
}

void
RoamMenuWindow::ShowErrMsg(char * fsname,Boolean compose,void *client_data)
{
Widget parent=NULL;
  assert((NULL != fsname));
  if(!compose)
  {
    RoamMenuWindow * self = (RoamMenuWindow *)client_data;
    parent = self->GetMainWin();
  }
  else
  {
    SendMsgDialog * self = (SendMsgDialog *)client_data;
    parent = self->GetMainWin();
  }
    if(!parent)
            parent = theApplication->baseWidget();
    	    DtMailGenDialog *genDialog = new DtMailGenDialog("Dialog",parent,XmDIALOG_FULL_APPLICATION_MODAL);

            char *errMsg = (char *) XtCalloc(1,10240+strlen(fsname));


                // Serious error here -- No Space on Filesystem --
                sprintf(errMsg,"Insufficient space on %s filesystem. Dtmail unable to show any new \n messages, delete messages, or save further changes to the open \n mailbox. Please contact your System Administrator to correct the \n filesystem space problem.",fsname);
	    free(fsname);
            genDialog->setToErrorDialog(
                                GETMSG(DT_catd, 1, 6, "Mailer"),
                                errMsg);
            XtFree(errMsg);

            genDialog->post_and_return(
                                GETMSG(DT_catd, 3, 9, "OK"),
                                NULL);
            delete genDialog;
}
void
RoamMenuWindow::stopAutoSave(void)
{
    DtMailEnv error;
    _mailbox->startAutoSave(error,DTM_FALSE);
}
void
RoamMenuWindow::startAutoSave(void)
{
    DtMailEnv error;
    _mailbox->startAutoSave(error,DTM_TRUE);
}

void
RoamMenuWindow::sync_work_area_size(void)
{
    Dimension width, height;

    XtVaGetValues(_workArea, XmNwidth, &width, XmNheight, &height, NULL);

    if (_workarea_width && _workarea_height &&
	(_workarea_width!=width || _workarea_height!=height))
    {
	enableWorkAreaResize();
	XtVaSetValues(_workArea, XmNwidth, width, XmNheight, height, NULL);
	disableWorkAreaResize();
    }
    _workarea_width = width;
    _workarea_height = height;
}
