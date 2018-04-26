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
//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $XConsortium: mp_c_file.C /main/3 1995/10/23 10:19:32 rswiston $ 			 				
/*
 * Tool Talk Message Passer (MP) - mp_c_file.cc
 *
 * Copyright (c) 1990,1992 by Sun Microsystems, Inc.
 *
 * Implementation of the _Tt_file class representing a document.
 */

#include <stdio.h>
#include <unistd.h>
#include "mp/mp_c_file.h"
#include "mp/mp_c_mp.h"
#include "mp/mp_c_global.h"     
#include "mp/mp_rpc_interface.h"
#include "mp/mp_c_session.h"
#include "mp/mp_c_procid.h"
#include "mp/mp_c_message.h"
#include "api/c/api_error.h"
#include "util/tt_port.h"

//
// Use parent class (_Tt_file) for construction and destruction.
//
_Tt_c_file::_Tt_c_file()
{
}

_Tt_c_file::_Tt_c_file(
	_Tt_string path
) :
	_Tt_file( path )
{
}

_Tt_c_file::~_Tt_c_file()
{
}

// 
// Joins the indicated file on behalf of the default procid. Joining a file
// means that any file-scoped patterns for this procid are updated to
// contain this file. It also means we have to indicated to procids in
// other sessions that this procid is interested in messages addressed to
// this file. This is done by storing the session for this procid in a
// persistent property on this file. The details of that are in
// _Tt_file::add_joined_file. Another responsibility of this method is to
// process any queued messages that are destined for any ptype declared
// by p.  This client/server division in the way file-scope queued
// messages are implemented reflects a goal of offloading any
// computation from the server to its clients. This is specially true
// of any computation that involves communicating with the database
// servers.
//
// XXX: it's misleading to have a _Tt_procid_ptr input arg here when
// in fact the default procid joins the file, not the specified
// procid.  Either figure out how to join the specified procid or
// remove the parameter.
// 
Tt_status
_Tt_c_file::c_join(
	_Tt_procid_ptr &
)
{
	Tt_status			status;
	Tt_status			rs;
	_Tt_db_results			dbStatus;
	_Tt_file_join_args		args;
	_Tt_session_ptr			d_session;
	_Tt_c_procid			*sp;

	sp = (_Tt_c_procid *)_tt_c_mp->default_procid().c_pointer();
	args.procid = _tt_c_mp->default_procid();
	args.path = getNetworkPath();
	d_session = sp->default_session();
	rs = d_session->call(TT_RPC_JOIN_FILE,
			     (xdrproc_t)tt_xdr_file_join_args,
			     (char *)&args,
			     (xdrproc_t)xdr_int,
			     (char *)&status);
	if (status == TT_WRN_NOTFOUND) {
		// No file patterns updated on the server side.
		status = TT_OK;
	} else {
		sp->add_joined_file(getNetworkPath());
		// add our session to the list of sessions
		// interested in this file.
		if (status == TT_WRN_STALE_OBJID) {
			dbStatus = addSession(d_session->process_tree_id());
			status = _tt_get_api_error( dbStatus, _TT_API_FILE );
			if (status == TT_ERR_INTERNAL) {
				_tt_syslog( 0, LOG_WARNING,
					    "_Tt_db_file::addSession(): %d",
					    dbStatus );
			}
		}
		if (status != TT_OK) {
			return status;
		}
		// process any queued messages on this file
		// for any of the ptypes we've declared.
		process_message_queue();
	}
	
	return((rs == TT_OK) ? status : rs);
}

// 
// Removes the session for the given procid from the list of interested
// sessions in this file. This method also invokes an rpc on the server
// session to remove this file from the patterns registered by this
// procid. The rpc call to the session will return a special status code
// TT_WRN_STALE_OBJID if this method is to actually remove the session
// from the file property of interested sessions. This is because the
// server session keeps a counter of how many procids are interested in
// each file.
// 
Tt_status
_Tt_c_file::c_quit(
	_Tt_procid_ptr &p
)
{
	_Tt_file_join_args		args;
	Tt_status			rs;
	Tt_status			status;
	_Tt_session_ptr			d_session;
		
	if (p.is_null()) {
		return(TT_ERR_PROCID);
	}
	d_session = ((_Tt_c_procid *)p.c_pointer())->default_session();
	if (d_session.is_null()) {
		return(TT_ERR_SESSION);
	}
	args.procid = p;
	args.path = getNetworkPath();
	rs = d_session->call(TT_RPC_QUIT_FILE,
			     (xdrproc_t)tt_xdr_file_join_args,
			     (char *)&args,
			     (xdrproc_t)xdr_int,
			     (char *)&status);
	if (status == TT_WRN_NOTFOUND) {
		status = TT_OK; 
	} else if (status == TT_WRN_STALE_OBJID) {
		_Tt_db_results dbStatus;
		dbStatus = deleteSession( d_session->process_tree_id() );
		if (dbStatus == TT_DB_ERR_NO_SUCH_PROPERTY) {
			status = TT_OK;
		} else {
			status = _tt_get_api_error( dbStatus, _TT_API_FILE );
		}
		if (status == TT_ERR_INTERNAL) {
			_tt_syslog( 0, LOG_WARNING,
				    "_Tt_db_file::deleteSession(): %d",
				    dbStatus );
			status = TT_ERR_INTERNAL;
		}
	}
	p->del_joined_file(getNetworkPath());
	return((status == TT_OK) ? rs : status);
}

//
// Proces any queued messages in this file. If so, then for each
// message destined for a ptype declared by the current default
// procid, retrieve the message and dispatch it to our current
// session. Note that messages are stored only once even if they're
// destined for multiple ptypes so we only de-queue the message if
// there are no destination ptypes once we're done dispatching the
// message.
//
// The exact layout of how messages are stored is described in
// _Tt_file::q_message below.
//
Tt_status
_Tt_c_file::process_message_queue(
	int dispatch_msgs
)
{
	_Tt_message_list_ptr	msgs;
	Tt_status		status;
	_Tt_db_results		dbStatus;

	dbStatus = dequeueMessages( _tt_c_mp->default_procid()->ptypes(),
				    msgs );
	status = _tt_get_api_error( dbStatus, _TT_API_FILE );
	if (status != TT_OK) {
		if (status == TT_ERR_INTERNAL) {
			_tt_syslog( 0, LOG_WARNING,
				    "_Tt_db_file::dequeueMessages(): %d",
				    dbStatus );
		}
		return status;
	}
	if (! dispatch_msgs) {
		return TT_OK;
	}
	_Tt_message_list_cursor msg( msgs );
	while (msg.next()) {
		msg->set_state( TT_QUEUED );
		(void)((_Tt_c_message *)(*msg).c_pointer())->dispatch( 1 );
	}
	return TT_OK;
}
