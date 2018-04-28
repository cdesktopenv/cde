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
//%%  $XConsortium: mp_s_mp.C /main/6 1996/05/09 20:30:03 drk $ 			 				
/*
 *
 * @(#)mp_s_mp.C	1.42	95/09/18
 *
 * Copyright 1990,1993 Sun Microsystems, Inc.  All rights reserved.
 */
#include "mp_s_global.h"
#include "mp_s_mp.h"
#include "mp/mp_mp.h"
#include "mp_s_procid.h"
#include "mp_self_procid.h"
#include "mp_rpc_implement.h"
#include "mp_rpc_server.h"
#include "mp_s_session.h"
#include "mp/mp_xdr_functions.h"
#include "mp_ptype.h"
#include "mp_otype.h"
#include "mp/mp_arg.h"
#include "mp_s_pattern.h"
#include "mp_signature.h"
#include "mp_s_message.h"
#include "mp_typedb.h"
#include "mp/mp_file.h"
#include "util/tt_global_env.h"
#include "util/tt_base64.h"
#include "util/tt_host.h"
#include "util/tt_port.h"
#include <errno.h>
#include <sys/resource.h>
#include <time.h>
#include <unistd.h>
#include <nl_types.h>
#include <util/tt_gettext.h>
#include <tt_const.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// global pointer to the _Tt_s_mp object. There should only be one
// instance of this object.
_Tt_s_mp *_tt_s_mp = (_Tt_s_mp *)0;

_Tt_s_mp::
_Tt_s_mp() : _Tt_mp()
{
	_flags |= (1<<_TT_MP_IN_SERVER);
	initial_session = initial_s_session = new _Tt_s_session;
	
	_active_fds = new _Tt_int_rec_list();
	_active_fds_procids = new _Tt_string_list();
	_mp_start_time = (int)time(0);

	_min_timeout = -1;
	_next_procid_key = 0;
	xfd = -1;
	exit_main_loop = 0;
	max_active_messages = 2000;
	fin = 0;
	fout = 0;
	ptable = new _Tt_ptype_table(_tt_ptype_ptid, 50);
	otable = new _Tt_otype_table(_tt_otype_otid, 50);
	sigs = new _Tt_sigs_by_op_table(_tt_sigs_by_op_op, 250);
	opful_pats = new _Tt_patlist_table(_tt_patlist_op, 250);
	opless_pats = new _Tt_pattern_list();
	active_procs = new _Tt_s_procid_table(_tt_procid_id, 250);
	now = 1;
	when_last_observer_registered = 1;
	update_args.message = new _Tt_s_message();
	_self = (_Tt_s_procid *)new _Tt_self_procid();

	map_ptypes = 0;
	unix_cred_chk_flag = 0;
	garbage_collector_pid = 0;
	_next_garbage_run = 0;
}


_Tt_s_mp::
~_Tt_s_mp()
{
	//
	// Emulate a tt_close() for _self.
	// XXX Commented out because currently s_remove_procid() neither:
	// - deletes this session from the interest list, nor
	// - sends an on_exit message (since _self submits none)
	//
	// if (! _self.is_null()) {
	// 	s_remove_procid( _self );
	// }
}

// 
// Initializes a _Tt_s_mp object. This entails initializing
// the initial session which is the default session that other clients
// connect to. This method will also initialize the global _Tt_s_mp
// object if the session is initialized. This is important since the
// _Tt_s_mp::init method assumes that the session has been initialized.
// XXX: the existence of the s_init and init methods is left over from
// an earlier incomplete split of _Tt_mp into server and client subclasses.
// Really, there should be just one virtual init method.
// 
Tt_status _Tt_s_mp::
s_init()
{
	Tt_status		status;

	active_messages = 0;

	// initialize "initial" session which is the server session in
	// server mode and the "default" session in client mode.
	
	status = initial_s_session->s_init();
	if (status == TT_OK) {
		status = init();
	}

	return(status);
}





// 
// Initializes the _Tt_s_mp object. This entails putting the two special
// "pseudo-procids" into the _active_fds and _active_fds_procids lists
// (see comment for _Tt_s_mp::find_proc)
// 
Tt_status _Tt_s_mp::
init()
{
	if (xfd != -1) {
		// put in fd for X connection
		_active_fds->push(new _Tt_int_rec(xfd));
		_active_fds_procids->push(_Tt_string("X"));
	}
#ifdef OPT_UNIX_SOCKET_RPC
	/* XXX: UNIX_SOCKET */		
	if (unix_fd != -1) {
		// put in fd for local rpc connections
		_active_fds->push(new _Tt_int_rec(unix_fd));
		_active_fds_procids->push(_Tt_string("U"));
	}		
	/* XXX: UNIX_SOCKET */		
#endif	// OPT_UNIX_SOCKET_RPC

	return(TT_OK);
}

//
// Initialize our _Tt_self_procid and register patterns for the
// messages we are interested in.
//
Tt_status
_Tt_s_mp::init_self()
{
	// Grab the global mutex to lock out other threads

	_tt_global->grab_mutex();

	// Increment the counter for the number of RPC calls
	// handled during the life of this process.  This has
	// to be done here since init_self() ultimately calls
	// something which calls updateFileSystemEntries(),
	// yet it's not done through an API or RPC call.  Thus,
	// this line here is somewhat of a bandaid. XXX
	_tt_global->event_counter++;

	// Use the lame do-loop hack to avoid repeating the drop_mutex
	// code after every possible failure...
	
	Tt_status status = TT_OK;;

	do {
		//
		// tt_open(), tt_fd()
		//
		Tt_status status = _self->init();
		if (status != TT_OK) {
			break;
		}
		status = add_procid( _self );
		if (status != TT_OK) {
			break;
		}
		_self->set_fd();
		status = _handle_Session_Trace();
		if (status != TT_OK) {
			break;
		}
		status = _observe_Saved();
		if (status != TT_OK) {
			break;
		}
	} while (0);

	_tt_global->drop_mutex();

	return TT_OK;
}

Tt_status
_Tt_s_mp::_handle_Session_Trace()
{
	//
	// tt_pattern_create(), tt_pattern_category_set(),
	// tt_pattern_scope_add(), tt_pattern_session_add(),
	// tt_pattern_op_add(), tt_pattern_arg_add()
	//
	_Tt_s_pattern_ptr pat = _Tt_self_procid::s_pattern_create();
	Tt_status status = pat->set_category( TT_HANDLE );
	if (status != TT_OK) {
		return status;
	}
	status = pat->add_message_class( TT_REQUEST );
	if (status != TT_OK) {
		return status;
	}
	status = pat->add_scope( TT_SESSION );
	if (status != TT_OK) {
		return status;
	}
	status = pat->add_session( _tt_s_mp->initial_s_session->address_string() );
	if (status != TT_OK) {
		return status;
	}
	status = pat->add_op( "Session_Trace" );
	if (status != TT_OK) {
		return status;
	}
	_Tt_arg_ptr arg = new _Tt_arg( TT_IN, "string" );
	status = pat->add_arg( arg );
	if (status != TT_OK) {
		return status;
	}
	pat->server_callback = _Tt_self_procid::handle_Session_Trace;
	//
	// tt_pattern_register()
	//
	status = _self->add_pattern( pat );
	if (status != TT_OK) {
		return status;
	}
	//
	// tt_session_join()
	//
	// _Tt_s_procid_ptr s_proc = (_Tt_s_procid *)_self.c_pointer();
	// status = _tt_s_mp->initial_s_session->s_join( s_proc );
	// if (status != TT_OK) {
	//	return status;
	// }
	return TT_OK;
}

Tt_status
_Tt_s_mp::_observe_Saved()
{
	//
	// tt_pattern_create(), tt_pattern_category_set(),
	// tt_pattern_scope_add(), tt_pattern_op_add(), tt_pattern_arg_add()
	//
	_Tt_s_pattern_ptr pat = _Tt_self_procid::s_pattern_create();
	Tt_status status = pat->set_category( TT_OBSERVE );
	if (status != TT_OK) {
		return status;
	}
	status = pat->add_message_class( TT_NOTICE );
	if (status != TT_OK) {
		return status;
	}
	status = pat->add_scope( TT_BOTH );
	if (status != TT_OK) {
		return status;
	}
	status = pat->add_session( _tt_s_mp->initial_s_session->address_string() );
	if (status != TT_OK) {
		return status;
	}
	_Tt_string_list_ptr ttpath = _Tt_typedb::tt_path();
	if (ttpath.is_null()) {
		return TT_ERR_NOMEM;
	}
	_Tt_string_list_cursor pathC( ttpath );
	while (pathC.next()) {
		status = pat->add_netfile( *pathC, 1 );
		if (status != TT_OK) {
			return status;
		}
	}
	status = pat->add_op( "Saved" );
	if (status != TT_OK) {
		return status;
	}
	_Tt_arg_ptr arg = new _Tt_arg( TT_IN, "File" );
	status = pat->add_arg( arg );
	if (status != TT_OK) {
		return status;
	}
	pat->server_callback = _Tt_self_procid::observe_Saved;
	//
	// tt_pattern_register()
	//
	status = _self->add_pattern( pat );
	if (status != TT_OK) {
		return status;
	}
	status = pat->join_files(_tt_s_mp->initial_s_session->process_tree_id());
	if (status != TT_OK) {
		return status;
	}
	return TT_OK;
}


// 
// Installs a list of signatures. Each signature is installed in a table
// called sigs_table that maps op names to lists of signatures with that
// same opname. This cuts down on the number of signatures that need to
// be examined to dispatch a message.
// 
void _Tt_s_mp::
install_signatures(_Tt_signature_list_ptr &s)
{
	_Tt_signature_list_cursor	sigC(s);
	_Tt_sigs_by_op_ptr		sigs_byop;
	
	while (sigC.next()) {
		sigs_byop = (_Tt_sigs_by_op *)0;
		if (! sigs->lookup(sigC->op(),sigs_byop)) {
			sigs_byop = new _Tt_sigs_by_op(sigC->op());
			sigs_byop->sigs = new _Tt_signature_list();
			sigs->insert(sigs_byop);
		} 
		sigs_byop->sigs->push(*sigC);
	}
}

void _Tt_s_mp::
remove_signatures(const _Tt_ptype &ptype)
{
	_Tt_sigs_by_op_table_cursor	sigs_byopC(sigs);
	
	while (sigs_byopC.next()) {
		_Tt_signature_list_cursor sigC(sigs_byopC->sigs);
		while (sigC.next()) {
			if (sigC->ptid() == ptype.ptid()) {
				sigC.remove();
			}
		}
	}
}

void _Tt_s_mp::
remove_signatures(const _Tt_otype &otype)
{
	_Tt_sigs_by_op_table_cursor	sigs_byopC(sigs);
	
	while (sigs_byopC.next()) {
		_Tt_signature_list_cursor sigC(sigs_byopC->sigs);
		while (sigC.next()) {
			if (sigC->otid() == otype.otid()) {
				sigC.remove();
			}
		}
	}
}

// 
// Iterates through the given table of ptypes and installs all the
// signatures contained in each ptype.
// 
void _Tt_s_mp::
install_ptable(_Tt_ptype_table_ptr &p)
{
	_Tt_ptype_table_cursor	ptypes;
	
	ptable = p;
	ptypes.reset(ptable);
	while (ptypes.next()) {
		remove_signatures(**ptypes);
		install_signatures(ptypes->hsigs());
		install_signatures(ptypes->osigs());
	}
}


// 
// Iterates through the given table of otypes and installs all the
// signatures contained in each otype.
// 
void _Tt_s_mp::
install_otable(_Tt_otype_table_ptr &o)
{
	_Tt_otype_table_cursor	otypes;
	
	otable = o;
	otypes.reset(otable);
	while (otypes.next()) {
		remove_signatures(**otypes);
		install_signatures(otypes->hsigs());
		install_signatures(otypes->osigs());
	}
}


// 
// It is important that the mp contain exactly one object for each
// procid that is registered because there is state that is contained
// in this instance of a procid that shouldn't be duplicated. This
// method takes a generic \(neither server nor client\) procid and
// returns either
// the _Tt_s_procid object that the mp has for the given id or else creates
// a new object if create_ifnot is equal to 1. When a procid is
// initialized, it's signalling fd is added to two parallel lists,
// _active_fds and _active_fds_procids.  _active_fds contains a list
// of fds whereas _active_fds_procids contains a list of procid ids.
// Each position in the two lists corresponds to each other. This is
// equivalent to having a single list of records where each record
// contains an fd and a procid id. These lists are used by the
// _Tt_s_mp::main_loop to poll all the fds in the list and match them
// with their respective procids. 
//
// The _active_fds_procids list can contain two "pseudo-procids" that
// are used to keep track of some special fds. These can be "U" for
// the special unix socket fd (used if OPT_UNIX_SOCKET_RPC is defined)
// that clients use to establish a unix socket rpc connection, and "X"
// for the fd that represents the connection to our desktop session. 
// See _Tt_s_mp::main_loop to see how they are used.
// 
//  XXX: Replacing the parallel list structure with a list of records
//  would increase readability.
// 
int
_Tt_s_mp::find_proc(
		    const _Tt_procid_ptr   &procid,
		    _Tt_s_procid_ptr &proc_returned,
		    int               create_ifnot
		    )
{
	_Tt_s_procid_ptr	sp;
	
	if (! procid.is_null()) {
		if (! _last_proc_hit.is_null()) {
			if (_last_proc_hit->is_equal(procid)) {
				proc_returned = _last_proc_hit;
				return(1);
			}
		}
		active_procs->lookup(procid->id(),sp);
	}
	if (!sp.is_null()) {
		_last_proc_hit = sp;
		proc_returned = sp;
		return(1);
	}
	if (create_ifnot) {
		sp = new _Tt_s_procid(procid);
		
		/* procid not found, add it to list */
		if (sp->init() == TT_OK) {
			proc_returned = sp;
			add_procid( sp );
			return(1);
		} else {
			return(0);
		}
	} else {
		return(0);
	}
}


// 
// Called in order to generate a unique key for a new procid. This key is
// prepended to a procid's session in order to form an id that is unique
// among all procids joined to the server's session.
// 
_Tt_string _Tt_s_mp::
alloc_procid_key()
{
	int		key = _next_procid_key++;
	_Tt_string	result(_tt_base64_encode((unsigned long)key));
	
	result = result.cat(".").cat(_tt_base64_encode((unsigned long)_mp_start_time));
	return(result);
}


// 
// This is the main loop of the message server. This method is
// responsible for servicing events such as rpc requests, disconnect
// signals from procids, connection requests for unix socket rpc
// requests, and events generated from our desktop connection (if
// applicable).  The main loop checks for the values of exit_main_loop
// and of fin and fout (see comments in bin/ttsession/mp_server.C)
// before invoking the _Tt_rpc_server::run method which will block on
// rpc requests and polling on the supplied list of file descriptors
// supplied in the _active_fds list.
// 
void _Tt_s_mp::
main_loop()
{
	_Tt_int_rec_list_cursor		fds;
	_Tt_string_list_cursor		fd_procid;
	_Tt_s_procid_ptr		sp;

	while (! exit_main_loop && (fin == fout)) {
		switch (initial_s_session->_rpc_server->run_until(&exit_main_loop,
								  _min_timeout,
								  _active_fds)) {
		    case _TT_RPCSRV_ERR:
		    case _TT_RPCSRV_OK:
			break;
		      case _TT_RPCSRV_TMOUT:
			break;
		    case _TT_RPCSRV_FDERR:
			// this error code is returned if any of the
			// file descriptors in _active_fds has input
			// pending. In the case of file descriptors
			// associated with procid signalling channels
			// this means that the connection to them was
			// lost, in the case of the file descriptor
			// being associated with the desktop
			// connection this means that there was an
			// event generated by the desktop server.
			// Otherwise, if the file descriptor was the
			// one associated with the unix socket rpc
			// socket then this is a new connection
			// request.
			//
			// Note that the _Tt_rpc_server::run method
			// will indicate which fd was active by
			// negating its value thus the test for being
			// less than zero indicates that was the fd we
			// were looking for.

			fds.reset(_active_fds);
			fd_procid.reset(_active_fds_procids);
			while (fds.next() && fd_procid.next()) {
				if (fds->val < 0) {
					if (*fd_procid == "X") {
						// X event came in
						fds->val *= -1;
						if (initial_session->desktop_event_callback()==-1) {
							exit_main_loop = 1;
							xfd = -2;
							break;
						}
#ifdef OPT_UNIX_SOCKET_RPC
					} else if ((0 - fds->val)==unix_fd) {
						// connection request
						// for local rpc transport.

						fds->val = unix_fd;
						initial_s_session->u_rpc_init();
#endif				// OPT_UNIX_SOCKET_RPC
					} else {
						// signalling channel
						// became active for
						// some procid. This
						// means the
						// connection was lost
						// since the
						// signalling channel
						// is a write-only
						// connection.
						if (active_procs->lookup(*fd_procid,sp)) {
							// Before cleaning up,
							// send any on_exit
							// messages.
							sp->send_on_exit_messages();
							sp->set_active(-1);
							active_procs->remove(sp->id());
						}
						fds.remove();
						fd_procid.remove();
					}
				}
			}
			break;
		    default:
			break;
		}
	}
}


// 
// Returns 1 if there exist file-scope patterns for the given pathname.
// Uses the two parallel lists _file_scope_paths and
// _file_scope_refcounts which form a logical list of records of file
// pathnames to number of patterns registered.
// 
//  XXX: Use of parallel lists is confusing. Recoding using a single list
//  of records would improve readability.
// 
int _Tt_s_mp::
in_file_scope(const _Tt_string &f)
{
	if (_file_scope_refcounts.is_null()) {
		return(0);
	}
	_Tt_int_rec_list_cursor		refcounts(_file_scope_refcounts);
	_Tt_string_list_cursor		paths(_file_scope_paths);
	
	while (refcounts.next() && paths.next()) {
		if (*paths == f) {
			return(1);
		}
	}
	
	return(0);
}


// 
// Adds (subtracts) number of file-scope patterns registered for the
// given pathname if add_scope is 1 (0). If the refcount of patterns goes
// to 0 then the pathname is removed from the _file_scope_refcounts and
// _file_scope_paths lists. See comment for _Tt_s_mp::in_file_scope for
// an explanation of these lists.
// 
void _Tt_s_mp::
mod_file_scope(const _Tt_string &f, int add_scope)
{
	if (_file_scope_refcounts.is_null()) {
		_file_scope_refcounts = new _Tt_int_rec_list();
		_file_scope_paths = new _Tt_string_list();
	}
	
	_Tt_int_rec_list_cursor		refcounts(_file_scope_refcounts);
	_Tt_string_list_cursor		paths(_file_scope_paths);
	
	while (refcounts.next() && paths.next()) {
		if (*paths == f) {
			refcounts->val += (add_scope ? 1 : -1);
			if (refcounts->val == 0) {
				refcounts.remove();
				paths.remove();
			}
			return;
		}
	}
	if (add_scope) {
		_file_scope_refcounts->push(new _Tt_int_rec(1));
		_file_scope_paths->push(f);
	}
}


Tt_status
_Tt_s_mp::add_procid(
		     _Tt_s_procid_ptr &proc
		     )
{
	_active_fds->push(new _Tt_int_rec(-1));
	_active_fds_procids->push(proc->id());
	active_procs->insert(proc);
	_last_proc_hit = proc;
	return TT_OK;
}


// 
// Called in response to a client invoking the tt_close api call. This
// method will deallocate any resources that this procid holds and will
// remove the procid reference from any global data-structures. 
// 
Tt_status _Tt_s_mp::
s_remove_procid(_Tt_s_procid &proc)
{
	proc.cancel_on_exit_messages();
	// de-activate this procid
	proc.set_active(0);
	// remove from list of active procs
	active_procs->remove(proc.id());
	
	return(TT_OK);
}
