/*
 *+SNOTICE
 *
 *
 *	$XConsortium: DtMailTransport.C /main/4 1996/04/21 19:47:48 drk $
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

#include <DtMail/DtMail.hh>
#include <DtMail/Threads.hh>

DtMail::Transport::Transport(DtMailEnv &,
			     Session * session,
			     DtMailStatusCallback cb,
			     void * client_data)
{
    _key = session->newObjectKey();
    _session = session;
    _callback = cb;
    _cb_data = client_data;
    _obj_mutex = MutexInit();
}

DtMail::Transport::~Transport(void)
{
    MutexDestroy(_obj_mutex);
    _session->removeObjectKey(_key);
}

void
DtMail::Transport::callCallback(DtMailOperationId id, void * arg)
{
    DtMailEnv * error = (DtMailEnv *)arg;

    _callback(id, *error, _cb_data);
}
