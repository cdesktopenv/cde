/*
 *+SNOTICE
 *
 *
 *	$TOG: DtMailMailBox.C /main/6 1997/09/26 09:07:17 mgreess $
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

#ifndef I_HAVE_NO_IDENT
#endif

#include <string.h>
#include <stdlib.h>
#include <DtMail/DtMail.hh>
#include <DtMail/Threads.hh>
#include <DtMail/IO.hh>

DtMail::MailBox::MailBox(DtMailEnv & error,
			 Session * session,
			 DtMailObjectSpace space,
			 void * arg,
			 DtMailCallback cb,
			 void * clientData) 
{
    _hide_access_events = DTM_FALSE;
    _key = session->newObjectKey();
    _session = session;
    error.clear();
    _space = space;
    _arg = arg;
    _errmsg_func = NULL;
    _err_data = NULL;
    _callback = cb;
    _cb_data = clientData;
    _obj_mutex = MutexInit();
}

DtMail::MailBox::~MailBox(void)
{
    if (_obj_mutex) {
	MutexDestroy(_obj_mutex);
	_obj_mutex = NULL;

	_session->removeObjectKey(_key);
    }
}

void
DtMail::MailBox::appendCB(
			DtMailEnv	&error,
			char		*buf,
			int		len,
			void		*clientData)
{
    DtMail::MailBox *obj = (DtMail::MailBox*) clientData;

    if (NULL == obj) return;
    obj->append(error, buf, len);
}

DtMail::Session *
DtMail::MailBox::session(void)
{
    return(_session);
}

DtMailBoolean
DtMail::MailBox::mailBoxWritable(DtMailEnv & error)
{
    error.clear();

    return(_mail_box_writable);
}
void
DtMail::MailBox::registerErrMsgFunc(err_func errmsg_func, void * err_data)
{
    _errmsg_func = errmsg_func;
    _err_data = err_data;
}
void
DtMail::MailBox::unregisterErrMsgFunc(DtMailEnv&)
{
    _errmsg_func = NULL;
    _err_data = NULL;
}
void
DtMail::MailBox::showError(char *fsnam)
{
    if (_errmsg_func && _err_data) {
         _errmsg_func(fsnam,FALSE,_err_data);
    }
}


