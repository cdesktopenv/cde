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
