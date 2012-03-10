/*
 *+SNOTICE
 *
 *
 *	$XConsortium: DtMailEnvelope.C /main/4 1996/04/21 19:47:32 drk $
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

DtMail::Envelope::Envelope(DtMailEnv & error, Message * parent)
{
    _parent = parent;
    _obj_mutex = MutexInit();
    error.clear();
}

DtMail::Envelope::~Envelope(void)
{
    if (_obj_mutex) {
	MutexLock lock_scope(_obj_mutex);
	if (_obj_mutex) {
	    lock_scope.unlock_and_destroy();
	    _obj_mutex = NULL;
	}
    }
}
