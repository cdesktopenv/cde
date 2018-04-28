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
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*
 *+SNOTICE
 *
 *	$TOG: MailSession.C /main/9 1999/03/26 16:51:51 mgreess $
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
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <X11/Intrinsic.h>
#include <DtMail/DtMail.hh>
#include <DtMail/DtMailError.hh>
#include <DtMail/DtMailXtProc.h>
#include <DtMail/IO.hh>

#include "MailSession.hh"
#include "MemUtils.hh"
#include "RoamMenuWindow.h"

DtMail::Session * MailSession::_app_session = NULL;
int MailSession::_num_deactivated = 0;

MailSession::MailSession(DtMailEnv & error, XtAppContext context)
: _open_mailboxes(128)
{
    error.clear();

    if (_app_session) {
	error.setError(DTME_ObjectInUse);
	return;
    }

    _app_session = new DtMail::Session(error, "dtmail");
    if (error.isSet())
    {
	// Benign error
	if ((DTMailError_t)error == DTME_NoMsgCat) error.clear();

	// Else need to translate the error into an DTME_* error
	// defined in DtMailError.hh
    }

    XtAppAddInput(context,
		  _app_session->eventFileDesc(error),
		  (XtPointer)XtInputReadMask,
		  DtMailXtInputProc,
		  _app_session);

#if 0
    //
    // Used to be both XtInputReadMask and XtInputExceptMask
    // went to the same procedure.  Resulted in problems in
    // when using R6 polling instead of R5 select.  Now
    // use separate input handlers.
    //
    XtAppAddInput(context,
		  _app_session->eventFileDesc(error),
		  (XtPointer)XtInputExceptMask,
		  DtMailXtExceptProc,
		  _app_session);
#endif

    DtMailDamageContext = context;
}

MailSession::~MailSession(void)
{
    for (int mb = 0; mb < _open_mailboxes.length(); mb++)
    {
	delete _open_mailboxes[mb]->handle;
	delete _open_mailboxes[mb]->path;
    }

    delete _app_session;
}

Boolean
MailSession::isMboxOpen(const char *path)
{
    int pos = locate(path);
    if (pos >= 0) return (TRUE);
    return (FALSE);
}

RoamMenuWindow *
MailSession::getRMW(const char *path)
{
    int slot = locate(path);
    if (slot >= 0)
      return(_open_mailboxes[slot]->rmw);
    return NULL;
}

RoamMenuWindow *
MailSession::getRMW(DtMail::MailBox *mbox)
{
    int slot = locate(mbox);
    return(_open_mailboxes[slot]->rmw);
}

DtMail::MailBox *
MailSession::open(
    DtMailEnv & error, 
    const char * path,
    DtMailCallback cb_func, 
    void * client_data,
    DtMailBoolean auto_create, 
    DtMailBoolean request_lock,
    DtMailBoolean auto_parse)
{
    // First, see if we have this open in this process.
    int slot = locate(path);

    // We do not allow open an already opened mailbox
    // because this may cause memory fault when a message is
    // deleted from one RMW and the other RMW would like to
    // read the already deleted message.
    // (1) set error to DTME_AlreadyOpened
    // (2) Return a NULL
    if (slot >= 0)
    {
        error.setError(DTME_AlreadyOpened);
        if (client_data == NULL)
	{
            _open_mailboxes[slot]->open_ref_count += 1;
            return _open_mailboxes[slot]->handle;
        } else return NULL;
    }

    // Create a handle for determining what to do next. This will
    // add us to the file session so we will start getting call
    // back requests on this file.
    DtMail::MailBox * mailbox = _app_session->mailBoxConstruct(
						error, DtMailFileObject,
				       		(void *)path,
						cb_func, client_data);
    if (error.isSet())
    {
	error.setError(DTME_ObjectCreationFailed);
	delete mailbox;
	return NULL;
    }
    
    // Does this file exist? If it doesn't and create is
    // on then make one. If create is off, then raise an
    // error.
    mailbox->open(error, 
	      auto_create, 
	      DTMAIL_DEFAULT_OPEN_MODE, 
	      DTMAIL_DEFAULT_CREATE_MODE,
	      request_lock,
	      auto_parse);
 
    // Need to translate from BE error to a FE error that the
    // user can understand.  Opening a mail container can result
    // in a variety of errors.  Translate those that are considered
    // to be relevant (for now) - the set can always be extended.
    // Toss others into an UnknownFormat error.
    if (error.isSet())
    {
	delete mailbox;
	return(NULL);
    }

    addToList(mailbox, path, (RoamMenuWindow *) client_data);
    return mailbox;
}

void
MailSession::close(DtMailEnv & error, const DtMail::MailBox * mb)
{
    // Find the mail box slot.
    int slot = locate(mb);
    if (slot < 0)
    {
	error.setError(DTME_ObjectInvalid);
	return;
    }

    MailBoxItem * mbi = _open_mailboxes[slot];
    mbi->open_ref_count -= 1;
    if (mbi->open_ref_count <= 0)
    {
        if (! _open_mailboxes[slot]->is_active) _num_deactivated--;

	delete mbi->handle;
	delete mbi->path;

	_open_mailboxes.remove(slot);
	delete mbi;
    }
}

void 
MailSession::convert(
    DtMailEnv & error, 
    const char *,		// old_path
    const char *,		// new_path
    DtMailStatusCallback,	// cb_func
    void *)			// client_data
{
    error.clear();
}

void
MailSession::copy(
    DtMailEnv & error, 
    DtMail::Message &,		// msg
    const char *)		// path
{
    error.clear();
}

int
MailSession::locate(const char *path)
{
    int slot;
    struct stat pathbuf;

    SafeStat(path, &pathbuf);
    for (slot = 0; slot < _open_mailboxes.length(); slot++)
    {
        if (strcmp(_open_mailboxes[slot]->path, path) == 0)
	  return slot;

	if (! _open_mailboxes[slot]->stated)
	{
	    _open_mailboxes[slot]->stated = TRUE;
	    SafeStat(
		_open_mailboxes[slot]->path,
		&_open_mailboxes[slot]->statbuf);
	}
	
	if (pathbuf.st_ino == _open_mailboxes[slot]->statbuf.st_ino &&
	    pathbuf.st_dev == _open_mailboxes[slot]->statbuf.st_dev)
	  return slot;
    }

    return -1;
}

int
MailSession::locate(const DtMail::MailBox *mb)
{
    for (int slot = 0; slot < _open_mailboxes.length(); slot++)
      if (mb == _open_mailboxes[slot]->handle)
	return slot;

    return -1;
}

int
MailSession::locate(const RoamMenuWindow *rmw)
{
    for (int slot = 0; slot < _open_mailboxes.length(); slot++)
      if (rmw == _open_mailboxes[slot]->rmw)
	return slot;

    return -1;
}

int
MailSession::isActiveRMW(RoamMenuWindow *rmw)
{
    int	slot = locate((const RoamMenuWindow *) rmw);
    if (slot < 0) return 0;
    
    return _open_mailboxes[slot]->is_active;
}

void
MailSession::activateRMW(RoamMenuWindow *rmw)
{
    int	slot = locate((const RoamMenuWindow *) rmw);
    if (slot < 0) return;
    
    if (! _open_mailboxes[slot]->is_active)
    {
	DtMail::MailBox *mailbox = rmw->mailbox();
	mailbox->enableMailRetrieval();

        DtMailEnv mail_error;
        mail_error.clear();
	rmw->checkForMail(mail_error);

        _open_mailboxes[slot]->is_active = TRUE;
	_num_deactivated--;
    }
}

void
MailSession::deactivateRMW(RoamMenuWindow *rmw)
{
    int	slot = locate((const RoamMenuWindow *) rmw);
    if (slot < 0) return;
    
    if (_open_mailboxes[slot]->is_active)
    {
	DtMail::MailBox *mailbox = rmw->mailbox();
	mailbox->disableMailRetrieval();

        _open_mailboxes[slot]->is_active = FALSE;
	_num_deactivated++;
    }
}

void
MailSession::addToList(
		DtMail::MailBox *mb,
		const char *path,
		RoamMenuWindow *rmw)
{
    MailBoxItem * mbi = new MailBoxItem;

    mbi->handle = mb;
    mbi->is_active = TRUE;
    mbi->stated = FALSE;
    mbi->open_ref_count = 1;
    mbi->path = strdup_n(path);
    mbi->rmw = rmw;
    
    _open_mailboxes.append(mbi);
}

Tt_status 
MailSession::lockCB(Tt_message, Tttk_op, const char*, uid_t, int, void*)
{
    // We never give up the lock during a copy!
    return TT_ERR_INVALID;
}
