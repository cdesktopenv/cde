/*
 *+SNOTICE
 *
 *
 *	$XConsortium: DtMailMessage.C /main/4 1996/04/21 19:47:38 drk $
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement bertween
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel wihtout
 *	Sun's specific written approval.  This documment and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

#include <string.h>
#include <stdlib.h>
#include <DtMail/DtMail.hh>
#include <DtMail/Threads.hh>

DtMail::Message::Message(DtMailEnv & error, MailBox * parent)
{
    _parent = parent;
    _obj_mutex = MutexInit();
// TOGO    _message = NULL;
    _envelope = NULL;
    _session = NULL;
    error.clear();
}

DtMail::Message::~Message(void)
{
    if (_obj_mutex) {
	MutexLock lock_scope(_obj_mutex);
	if (_obj_mutex) {
	    if (_envelope) {
		delete _envelope;
	    }

	    lock_scope.unlock_and_destroy();
	    _obj_mutex = NULL;
	}
    }
}

DtMail::MailBox *
DtMail::Message::mailBox(void)
{
    return(_parent);
}

DtMail::Session *
DtMail::Message::session(void)
{
    return(_session);
}
