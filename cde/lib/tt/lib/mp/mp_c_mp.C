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
//%%  (c) Copyright 1993, 1994, 1996 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $TOG: mp_c_mp.C /main/6 1999/10/14 18:41:09 mgreess $ 			 				
/*
 *
 * mp_c_mp.cc
 *
 * Implementations of client-only members of _Tt_mp object.
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */
#include "mp/mp_c_global.h"
#include "mp/mp_c_mp.h"
#include "mp/mp_c_session.h"
#include "mp/mp_c_procid.h"
#include "mp/mp_rpc_interface.h"
#include "util/tt_global_env.h"
#include "util/tt_port.h"
#include "util/tt_threadspecific.h"

_Tt_c_mp	*_tt_c_mp = (_Tt_c_mp *)0;

_Tt_c_mp::
_Tt_c_mp() : _Tt_mp()
{
	_flags &= ~(1<<_TT_MP_IN_SERVER);
	initial_session = initial_c_session =
		default_c_session = new _Tt_c_session;
}

_Tt_c_mp::
~_Tt_c_mp()
{
}

// 
// Initializes the _Tt_c_mp object. This initializes all the caches
// maintained by this object as well as the "initial" session which is
// the "default" session for a ToolTalk client.
// 
// Returns:
//	TT_OK
//	TT_ERR_NOMP - the initial session could not be initialized.

Tt_status _Tt_c_mp::
c_init()
{
	Tt_status		status;
	

	if (_flags&(1<<_TT_MP_INIT_DONE)) {
		return(TT_OK);
	}

	_file_cache = new _Tt_file_table( _Tt_file::networkPath_ );

	// initialize "initial" session which is the server session in
	// server mode and the "default" session in client mode.
	
	status = initial_c_session->c_init();
	if (status == TT_ERR_NOMP) {
		if (default_c_session.is_eq(initial_c_session)) {
			default_c_session = 0;
		}
#ifdef OPT_XTHREADS
		_Tt_threadspecific* tss = (_Tt_threadspecific *) 0;
		xthread_get_specific(_tt_global->threadkey, (void **) &tss);
	
		if (!tss) {
			// thread-specific storage not yet initialized -- do it
			tss = new _Tt_threadspecific(default_c_session,
						     _default_c_procid);
			xthread_set_specific(_tt_global->threadkey, tss);
		}

        	_Tt_c_session_ptr cs = tss->thread_c_session();
		_Tt_c_session_ptr nullsess = (_Tt_c_session *) 0;
		if (!cs.is_null() && cs.is_eq(initial_c_session)) {
			tss->set_thread_c_session(nullsess);
		}
#endif	

		initial_session = initial_c_session = 0;
	}

	if (status == TT_OK && !in_server()) {
		// enter session object in session table
		_session_cache->insert(initial_session);
	}
	_flags |= (1<<_TT_MP_INIT_DONE);
	return(status);
}	

_Tt_procid_ptr & _Tt_c_mp::
default_procid()
{
	// XXX: (futures) For clients using multi-threading, this
	// function should index the default procid off the current
	// thread id so that there are no conflicts with other
	// threads. 
	
	return(default_c_procid());
}

_Tt_c_procid_ptr & _Tt_c_mp::
default_c_procid()
{
	
#ifdef OPT_XTHREADS

	// Return a thread-specific procid if one is set and libthread
	// has actually been linked in

	if (_tt_global->multithreaded()) {
		_Tt_threadspecific* tss = (_Tt_threadspecific *) 0;
		xthread_get_specific(_tt_global->threadkey, (void **) &tss);
	
		if (!tss) {
			// thread-specific storage not yet initialized -- do it
			tss = new _Tt_threadspecific(default_c_session,
						     _default_c_procid);

			xthread_set_specific(_tt_global->threadkey, tss);
		}

		if (tss->thread_c_procid().is_null()) {
			tss->set_thread_c_procid(_default_c_procid);
		}
		
		return tss->thread_c_procid();
	}
#endif
	return(_default_c_procid);
}


Tt_status _Tt_c_mp::
set_default_procid(
		_Tt_string	id,
		int
#ifdef OPT_XTHREADS
				thread_only
#endif
		   )
{
	_Tt_procid_ptr	p;
	
	if (id.len() == 0) {
		return(TT_ERR_PROCID);
	}
	p = active_procs->lookup(id);
	if (p.is_null()) {
		return(TT_ERR_PROCID);
	}

	_Tt_c_procid_ptr pp = (_Tt_c_procid *) p.c_pointer();

#ifdef OPT_XTHREADS
	if (thread_only) {
		_Tt_threadspecific* tss = (_Tt_threadspecific *) 0;
		xthread_get_specific(_tt_global->threadkey, (void **) &tss);
	
		if (!tss) {
			// thread-specific storage not yet initialized -- do it
			tss = new _Tt_threadspecific(default_c_session, pp);
			xthread_set_specific(_tt_global->threadkey, tss);
		} else {
			tss->set_thread_c_procid(pp);
		}
	}
	else		// fall through to set _default_c_procid
#endif
	_default_c_procid = pp;


	return(TT_OK);
}
// 
// A client-only method that removes a procid from the active procid
// cache and also deallocates any resources/network connections held by
// the procid. If this is the last active procid to be removed then the
// default session is also shut down. This method operates in this
// fashion in order for it to be suitable for the semantics of the
// "tt_close" api call which overloads the call to mean "close the last
// procid" and "deallocate tooltalk resources (if no more procids left)".
// 
Tt_status _Tt_c_mp::
c_remove_procid(_Tt_procid_ptr &proc)
{
	_Tt_session_ptr			d_session;
	Tt_status			rstatus;
	Tt_status			status = TT_OK;
	_Tt_procid_table_cursor		procs;
	int				remove_session;
	_Tt_c_procid			*cp;
	
	if (proc.is_null()) {
		return(TT_ERR_INVALID);
	}

	cp = (_Tt_c_procid *)proc.c_pointer();
	cp->close();
	// remove from list of active procs
	active_procs->remove(proc->id());
		
	d_session = cp->default_session();
	if (d_session.is_null()) {
		return(TT_ERR_SESSION);
	}
	rstatus = d_session->call(TT_RPC_CLOSE_PROCID,
				  (xdrproc_t)tt_xdr_procid,
				  (char *)&proc,
				  (xdrproc_t)xdr_int,
				  (char *)&status);
		
	if (!d_session.is_eq(initial_session)) { 
		
		// check to see if this is the last procid to
		// reference this session (this could be done quicker
		// by just checking the _refcount_ on the session but
		// then we'd have to be careful not to have any other
		// references to this session or else we'd fail to
		// remove it from the session cache)
			
		procs.reset(active_procs);
		remove_session = 1;
		while (procs.next()) {
			cp = (_Tt_c_procid *)(*procs).c_pointer();
			if (cp->default_session()->address_string() ==
			    d_session->address_string()) {
				remove_session = 0;
				break;
			}
		}
		// unlink d_session and remove from session cache if
		// this is the last procid to be using it.
		if (remove_session) {
			_session_cache->remove(d_session->address_string());
		}
	}
		
	// this should cause the session to be disconnected if
	// remove_session is set since the last remaining
	// pointer to the session should be _default_session.
	// unless it's the initial session in which case it
	// should stay around.
	_Tt_string nullprocid = (char*)0;
	cp->set_default_session(nullprocid);

	// Zero out the default procid if it matches the passed-in -- i.e.
	// thread-specific -- procid.
	if (proc.is_eq(_default_c_procid)) {
		_default_c_procid = (_Tt_c_procid *)0;
	}

#ifdef OPT_XTHREADS
	// Free the storage for the thread-specific procid

	_Tt_threadspecific* tss = (_Tt_threadspecific *) 0;
	xthread_get_specific(_tt_global->threadkey, (void **) &tss);
	
	if (tss) {
		tss->free_procid();
	}

#endif

	if (rstatus != TT_OK) {
		status = rstatus;
	}

	return(status);
}


// 
// Creates a new procid. If this is the first procid to be created then
// we need to check for the start token being set which indicates this
// procid was activated in response to a tooltalk start message. If this
// is the case then this procid is "commited" to the default session.
// 
Tt_status _Tt_c_mp::
create_new_procid()
{
	Tt_status		status;
	const char	       *start_token;
	int			commit = 0;
	_Tt_string		session;
	_Tt_c_procid		*cp = new _Tt_c_procid();
	_Tt_c_procid_ptr	cpp = cp;
	
	// initialize default procid

	_default_c_procid = cp;
#ifdef OPT_XTHREADS
	if (_tt_global->multithreaded()){
		_Tt_threadspecific* tss = (_Tt_threadspecific *) 0;
		xthread_get_specific(_tt_global->threadkey, (void **) &tss);
	
		if (!tss) {
			// thread-specific storage not yet initialized -- do it
			tss = new _Tt_threadspecific(default_c_session, cpp);
			xthread_set_specific(_tt_global->threadkey, tss);
		} else {
			tss->set_thread_c_procid(cpp);
		}
	}
#endif

	//
	// Check if there is a START_MESSAGE for us.
	// 
	start_token = _tt_get_first_set_env_var(2, TT_CDE_START_TOKEN, TT_START_TOKEN);
	session = _tt_get_first_set_env_var(2, TT_CDE_XATOM_NAME, TT_XATOM_NAME);
	if ((start_token != 0 && *start_token != 0 )
	    && default_c_session->has_id(session)) {
		cp->set_start_token(start_token);
		commit = 1;
		//
		// now clear the value of this environment variable so
		// that subsequent children of this process don't get
		// confused.
		//


		// This is done in a really ugly fashion, because older
		// versions of the ToolTalk library on Sun assumed that the
		// token was there if the _TT_TOKEN environment variable
		// existed, even if the value (after the '=') was null.
		// Unfortunately there is no portable way to completely
		// remove a environment variable from the environment.  The
		// best we can do is change the environment variable name.
		// Since we're already being ugly, we do this in the brutal
		// fashion of just backing up from the environment variable
		// value into the name and punching over one of the
		// characters.  In particular, we change the 4th character
		// in front of the equals sign (the 5th character in front
		// of the value) to an "A".  This changes _TT_TOKEN to
		// _TT_TAKEN and _SUN_TT_TOKEN to _SUN_TT_TAKEN.  This looks
		// OK enough (the token's been taken) that maybe people
		// looking in the environment won't be upset.
		
		char *p;
		p = getenv(TT_CDE_START_TOKEN);
		if (p) {
			*(p-5) = 'A';
		}
		p = getenv(TT_START_TOKEN);
		if (p) {
			*(p-5) = 'A';
		}
	}

	status = cp->init();
	if (commit) {
		// we should check for TT_CDE_START_TOKEN or
		// TT_START_TOKEN to be set. If
		// so then we commit this procid which is required to
		// join the session which caused this process to be
		// started.

		(void)cp->commit();
	}
	if (status != TT_OK) {
		_default_c_procid = (_Tt_c_procid *)0;
#ifdef OPT_XTHREADS
		_Tt_threadspecific* tss = (_Tt_threadspecific *) 0;
		xthread_get_specific(_tt_global->threadkey, (void **) &tss);
		
		if (!tss) {
			// thread-specific storage not yet initialized -- do it
			tss = new _Tt_threadspecific(default_c_session,
						     (_Tt_c_procid *) 0);
			xthread_set_specific(_tt_global->threadkey, tss);
		}
		else {
			tss->free_procid();
		}

#endif

		return(status);
	}
	active_procs->insert(default_c_procid());
	
	return(TT_OK);
}


//
// Returns a count of how many procids are in the active procid cache.
//
int _Tt_c_mp::
procid_count()
{
	if (active_procs.is_null()) {
		return(0);
	} else {
		return(active_procs->count());
	}
}

#ifdef OPT_XTHREADS

//
// Initializes structures needed for multithreading
//
void _Tt_c_mp::
set_multithreaded()
{
	if (_tt_global->multithreaded()) {
		_Tt_threadspecific* tss =
			new _Tt_threadspecific(default_c_session,
					       (_Tt_c_procid *) 0);
		xthread_set_specific(_tt_global->threadkey, tss);
	}
}
#endif
