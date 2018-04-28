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
//%%  $XConsortium: api_mp.C /main/4 1995/11/28 19:21:30 cde-sun $ 			 				
/* @(#)api_mp.C	1.48 93/07/30
 *
 * api_mp.cc
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 * 
 * This file contains API functions related to the MP. For
 * each API function named tt_<name> there is a _tt_<name> function in
 * some file named api_mp_*.cc.
 */

#include "db/tt_db_hostname_global_map_ref.h"
#include "db/tt_old_db_partition_map_ref.h"
#include "mp/mp_c_global.h"
#include "mp/mp_c.h"
#include "api/c/tt_c.h"
#include "api/c/api_api.h"
#include "api/c/api_mp.h"
#include "api/c/api_handle.h"
#include "api/c/api_spec_map_ref.h"
#include "api/c/api_typecb.h"
#include "util/tt_file_system.h"
#include "util/tt_global_env.h"
#include "util/tt_audit.h"

//HACK: The following variable needs to be moved to the procid structure;
//handles should be procid-dependent. RFM 11/12/90
_Tt_api_handle_table *_tt_htab;

char *		_tt_open();
Tt_status	_tt_close();
Tt_status	_tt_session_join(const char *);
Tt_status	_tt_session_quit(const char *);
int		_tt_fd();
char *		_tt_X_session(const char *);
Tt_status	_tt_session_join(const char *);
char *		_tt_initial_session();
Tt_message	_tt_pnotice_create(Tt_scope , const char *);
Tt_message	_tt_prequest_create(Tt_scope , const char *);
Tt_message	_tt_onotice_create(const char *, const char *);
Tt_status	_tt_ptype_opnum_callback_add(const char *, int,
					     Tt_message_callback);
Tt_status	_tt_otype_opnum_callback_add(const char *, int,
			     		     Tt_message_callback);
Tt_message	_tt_orequest_create(const char *, const char *);

char *
tt_open()
{
	_Tt_audit audit;
        Tt_status status = audit.entry("v", TT_OPEN, 0);
	char *result;

        if (status != TT_OK) {
		audit.exit((char *)error_pointer(status));
                return (char *)error_pointer(status);
        }

        result = _tt_open();
        audit.exit(result);

	return result;
}


Tt_status
tt_close()
{
	_Tt_audit audit;
        Tt_status status = audit.entry("v", TT_CLOSE, 0);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status =  _tt_close();
        audit.exit(status);

	return status;
}


char *
tt_initial_session()
{
	_Tt_audit audit;
        Tt_status status = audit.entry("v", TT_INITIAL_SESSION, 0);
	char *result;

        if (status != TT_OK) {
		audit.exit((char *)error_pointer(status));
                return (char *)error_pointer(status);
        }

        result = _tt_initial_session();
        audit.exit(result);

	return result;
}


char *
tt_X_session(const char *display_name)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("C", TT_X_SESSION, display_name);
	char *result;

        if (status != TT_OK) {
		audit.exit((char *)error_pointer(status));
                return (char *)error_pointer(status);
        }

        result = _tt_X_session(display_name);
        audit.exit(result);

	return result;
}



Tt_status
tt_session_join(const char * sessid)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("C", TT_SESSION_JOIN, sessid);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status = _tt_session_join(sessid);
        audit.exit(status);

	return status;
}


Tt_status
tt_session_quit(const char * sessid)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("C", TT_SESSION_QUIT, sessid);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status = _tt_session_quit(sessid);
        audit.exit(status);

	return status;
}


int
tt_fd(void)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("v", TT_FD, 0);
	int result;

        if (status != TT_OK) {
		audit.exit(error_int(status));
                return error_int(status);
        }

	result = _tt_fd();
        audit.exit(result);

	return result;
}


Tt_message
tt_pnotice_create(Tt_scope scope, const char *op)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("SC", TT_PNOTICE_CREATE, scope, op);
	Tt_message result;

        if (status != TT_OK) {
		audit.exit((Tt_message)error_pointer(status));
		return (Tt_message)error_pointer(status);
        }

        result = _tt_pnotice_create(scope, op);

        audit.exit(result);       

        return result;
}


Tt_message
tt_prequest_create(Tt_scope scope, const char *op)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("SC", TT_PREQUEST_CREATE, scope, op);
        Tt_message result;

        if (status != TT_OK) {
		audit.exit((Tt_message)error_pointer(status));
		return (Tt_message)error_pointer(status);
        }

        result = _tt_prequest_create(scope, op);

        audit.exit(result);

        return result;
}


Tt_message
tt_onotice_create(const char *objid, const char *op)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("oC", TT_ONOTICE_CREATE, objid, op);
        Tt_message result;

        if (status != TT_OK) {
		audit.exit((Tt_message)error_pointer(status));
		return (Tt_message)error_pointer(status);
        }

        result = _tt_onotice_create(objid, op);

        audit.exit(result);

        return result;
}


Tt_message
tt_orequest_create(const char *objid, const char *op)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("oC", TT_OREQUEST_CREATE, objid, op);
        Tt_message result;

        if (status != TT_OK) {
		audit.exit((Tt_message)error_pointer(status));
		return (Tt_message)error_pointer(status);
        }

        result = _tt_orequest_create(objid, op);

        audit.exit(result);

        return result;
}


Tt_status
tt_ptype_opnum_callback_add(const char *ptid, int opnum, Tt_message_callback f)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("CiX", TT_PTYPE_OPNUM_CALLBACK_ADD,
					     ptid, opnum, f);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status = _tt_ptype_opnum_callback_add(ptid, opnum, f);
        audit.exit(status);

        return status;
}


Tt_status
tt_otype_opnum_callback_add(const char *otid, int opnum, Tt_message_callback f)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("CiX", TT_OTYPE_OPNUM_CALLBACK_ADD,
					     otid, opnum, f);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status = _tt_otype_opnum_callback_add(otid, opnum, f);
        audit.exit(status);

        return status;
}


/* 
 * Allocates a procid which is a ToolTalk message port. Can be called
 * multiple times if a process wants to have multiple procids.
 * Also initialize all the needed data structures. This function will set
 * the default procid and the default session (if not set).
 */
char *
_tt_open()
{
	Tt_status ttrc;

	
	_tt_internal_init();

	ttrc=_tt_c_mp->c_init();
	if (ttrc != TT_OK && ttrc != TT_ERR_NOMP) {
		return (char *)error_pointer(ttrc);
	}
	if (TT_OK != (ttrc=_tt_c_mp->create_new_procid())) {
		return (char *)error_pointer(ttrc);
	}
	// Refresh the DB hostname redirection map
	_tt_global->db_hr_map.refresh();
	return _tt_strdup(_tt_c_mp->default_procid()->id());
}


void
_tt_internal_init()
{
	if (! _tt_mp) {
		if (! _tt_global) {
			_tt_global = new _Tt_global();
		}
		_tt_c_mp = new _Tt_c_mp();
		_tt_mp = (_Tt_mp *)_tt_c_mp;
		if (! _tt_htab) {
			_tt_htab = new _Tt_api_handle_table;
		}
	}
}


/* 
 * Destroys any open connections for the current default procid. If this
 * is the last procid for this process then all mp resources will be
 * destroyed. 
 */
Tt_status
_tt_close()
{
	// destroy any state related to the current procid
	// and set the default procid to the next available
	// procid

	// XXX: Note that this passes in the *thread-specific* default
	// procid.  The process-wide default procid will be left
	// unset if it is the same as the thread-specific procid.
	_tt_c_mp->c_remove_procid(_tt_c_mp->default_procid());

	// if there are no more procids, then we go ahead and destroy
	// everything else.
	if (_tt_c_mp->procid_count() == 0) {
		// XXX why?  not necessary.  Try removing this line.
		// 7/30/95 STR -- okay, since I'm mucking in here for
		// the MT-safe stuff anyway, I'll give it a try....
		//_tt_c_mp->default_c_session = (_Tt_c_session *)0;

		// We used to delete _tt_global here, but we
		// no longer do that because it's possible that
		// someone may do another tt_open() in this same
		// process later.

		delete _tt_htab;
		delete _tt_c_mp;
		_tt_c_mp = 0;
		_tt_mp = 0;
		_tt_htab = 0;

		//  XXX: if we were to make these classes have instances
		// pointed to out of _tt_global, instead of having a
		// static pointer internal to them, we wouldn\'t have to
		// put individual flush calls here. RFM 93/12/10.

		_Tt_db_hostname_global_map_ref db_map;
		db_map.flush();

		// This map is only required for old DB server compatibility
		_Tt_old_db_partition_map_ref old_db_map;
		old_db_map.flush();

		_Tt_api_spec_map_ref spec_map;
		spec_map.flush();
	}

	return TT_OK;
}


/* 
 * Returns the id of the initial session (the process tree session if
 * specified, else the session associated with the X display named in
 * the DISPLAY environment variable.
 */
char *
_tt_initial_session()
{
	_Tt_string s = _tt_c_mp->initial_session->id();
	return _tt_strdup((const char *) s);
}


char *
_tt_X_session(const char *display_name)
{
	_Tt_string	id;

	_tt_internal_init();

	id = _tt_c_mp->initial_session->Xid(display_name);

	if (id.len() == 0) {
		return((char *)error_pointer(TT_ERR_SESSION));
	}

	return _tt_strdup(id);
}

/* 
 * Session functions (FSpec A.11)
 */
Tt_status
_tt_session_join(const char * sessid)
{
	Tt_status	status;
	_Tt_c_procid	*d_procid = _tt_c_mp->default_c_procid().c_pointer();
	_Tt_string	sessidstr = (char *)0;


	// Add the P on if isn't already.
	_prepend_P_to_sessid(sessid, sessidstr);

	status = d_procid->set_default_session(sessidstr);
	if (status != TT_OK) {
		return(status);
	}

	PCOMMIT;

	status = d_procid->default_session()->
          c_join(_tt_c_mp->default_c_procid());

	return(status);
}


Tt_status
_tt_session_quit(const char * sessid)
{
	Tt_status	status;
	_Tt_c_procid	*d_procid = _tt_c_mp->default_c_procid().c_pointer();
	_Tt_c_session   *d_session;
	
	d_session = d_procid->default_session().c_pointer();
	if (d_session->id() != sessid) {
		return(TT_ERR_SESSION);
	}
	status = d_session->c_quit(_tt_c_mp->default_c_procid());

	return(status);
}


/* 
 * Message arrival notification methods (FSpec A.13)
 */



/* 
 * Returns a file descriptor that will become active for input whenever a
 * message arrives for the process, and will stay active until the
 * message is received.
 */
int
_tt_fd(void)
{
	int result;
	_Tt_c_procid	*d_procid = _tt_c_mp->default_c_procid().c_pointer();

	PCOMMIT;

	if (d_procid->set_fd_channel()) {
		result = d_procid->fd();
	} else {
		result =  error_int(TT_ERR_NOMP);
	}
	return result;
}


/* 
 * The following are "macros" which provide a simpler high level
 * interface to the above low level calls.
 */


/* 
 * Create a procedural notification with the given scope and op
 */
Tt_message
_tt_pnotice_create(Tt_scope scope, const char *op)
{
	Tt_message	result;
	Tt_status	err;
	_Tt_c_procid	*d_procid = _tt_c_mp->default_c_procid().c_pointer();


	// tt_message_create
	_Tt_c_message_ptr msg = new _Tt_c_message();

	msg->set_state(TT_CREATED);
	msg->set_sender(_tt_c_mp->default_c_procid());
	msg->set_id();
	msg->set_file(d_procid->default_file());
	msg->set_session(d_procid->default_session());
	msg->set_sender_ptype(d_procid->default_ptype());
	// tt_message_address_set
	err = msg->set_paradigm(TT_PROCEDURE);
	if (err != TT_OK) {
		return (Tt_message)error_pointer(err);
	}
	// tt_message_class_set
	err = msg->set_message_class(TT_NOTICE);
	if (err != TT_OK) {
		return (Tt_message)error_pointer(err);
	}
	// tt_message_op_set
	err = msg->set_op(op);
	if (err != TT_OK) {
		return (Tt_message)error_pointer(err);
	}
	// tt_message_scope_set
	err = msg->set_scope(scope);
	if (err != TT_OK) {
		return (Tt_message)error_pointer(err);
	}
	result = _tt_htab->lookup_mhandle(msg);
	return result;
}


/* 
 * Create a procedural request with the given scope and op
 */
Tt_message
_tt_prequest_create(Tt_scope scope, const char *op)
{
	Tt_message	result;
	Tt_status	err;
	_Tt_c_procid	*d_procid = _tt_c_mp->default_c_procid().c_pointer();

	// tt_message_create
	_Tt_c_message_ptr msg = new _Tt_c_message();
	msg->set_state(TT_CREATED);
	msg->set_sender(_tt_c_mp->default_c_procid());
	msg->set_id();
	msg->set_file(d_procid->default_file());
	msg->set_session(d_procid->default_session());
	msg->set_sender_ptype(d_procid->default_ptype());
	// tt_message_address_set
	err = msg->set_paradigm(TT_PROCEDURE);
	if (err != TT_OK) {
		return (Tt_message)error_pointer(err);
	}
	// tt_message_class_set
	err = msg->set_message_class(TT_REQUEST);
	if (err != TT_OK) {
		return (Tt_message)error_pointer(err);
	}
	// tt_message_op_set
	err = msg->set_op(op);
	if (err != TT_OK) {
		return (Tt_message)error_pointer(err);
	}
	// tt_message_scope_set
	err = msg->set_scope(scope);
	if (err != TT_OK) {
		return (Tt_message)error_pointer(err);
	}
	result = _tt_htab->lookup_mhandle(msg);
	return result;
}

/* 
 * Create an object oriented notification with the given node and op
 */
Tt_message
_tt_onotice_create(const char *objid, const char *op)
{
	Tt_message	result;
	Tt_status	err;
	_Tt_c_procid	*d_procid = _tt_c_mp->default_c_procid().c_pointer();

	// tt_message_create
	_Tt_c_message_ptr msg = new _Tt_c_message();
	msg->set_state(TT_CREATED);
	msg->set_sender(_tt_c_mp->default_c_procid());
	msg->set_id();
	msg->set_file(d_procid->default_file());
	msg->set_session(d_procid->default_session());
	msg->set_sender_ptype(d_procid->default_ptype());
	// tt_message_address_set
	err = msg->set_paradigm(TT_OBJECT);
	if (err != TT_OK) {
		return (Tt_message)error_pointer(err);
	}
	// tt_message_class_set
	err = msg->set_message_class(TT_NOTICE);
	if (err != TT_OK) {
		return (Tt_message)error_pointer(err);
	}
	// tt_message_op_set
	err = msg->set_op(op);
	if (err != TT_OK) {
		return (Tt_message)error_pointer(err);
	}
	// tt_message_object_set
	_Tt_string oid(objid);
	err = msg->set_object(oid.unquote_nulls());
	if (err != TT_OK) {
		return (Tt_message)error_pointer(err);
	}
	result = _tt_htab->lookup_mhandle(msg);
	return result;
}


/* 
 * Create an object oriented request with the given node and op
 */
Tt_message
_tt_orequest_create(const char *objid, const char *op)
{
	Tt_message result;
	Tt_status err;
	_Tt_c_procid	*d_procid = _tt_c_mp->default_c_procid().c_pointer();


	// tt_message_create
	_Tt_c_message_ptr msg = new _Tt_c_message();
	msg->set_state(TT_CREATED);
	msg->set_sender(_tt_c_mp->default_c_procid());
	msg->set_id();
	msg->set_file(d_procid->default_file());
	msg->set_session(d_procid->default_session());
	msg->set_sender_ptype(d_procid->default_ptype());
	// tt_message_address_set
	err = msg->set_paradigm(TT_OBJECT);
	if (err != TT_OK) {
		return (Tt_message)error_pointer(err);
	}
	// tt_message_class_set
	err = msg->set_message_class(TT_REQUEST);
	if (err != TT_OK) {
		return (Tt_message)error_pointer(err);
	}
	// tt_message_op_set
	err = msg->set_op(op);
	if (err != TT_OK) {
		return (Tt_message)error_pointer(err);
	}
	// tt_message_object_set
	_Tt_string oid(objid);
	err = msg->set_object(oid.unquote_nulls());
	if (err != TT_OK) {
		return (Tt_message)error_pointer(err);
	}
	result = _tt_htab->lookup_mhandle(msg);
	return result;
}


/*
 * Register ptype and otype callbacks.
 */

static Tt_status
type_opnum_callback_add(_Tt_typecb_table_ptr &callbacks,
			const char *tid, int opnum, Tt_message_callback f);

Tt_status
_tt_ptype_opnum_callback_add(const char *ptid, int opnum,
			     Tt_message_callback f)
{
	_Tt_c_procid	*d_procid = _tt_c_mp->default_c_procid().c_pointer();

	_Tt_typecb_table_ptr & tcbt = d_procid->ptype_callbacks();
	return type_opnum_callback_add(tcbt, ptid, opnum, f);
}


Tt_status
_tt_otype_opnum_callback_add(const char *otid, int opnum,
			     Tt_message_callback f)
{
	Tt_status	result;
	_Tt_c_procid	*d_procid = _tt_c_mp->default_c_procid().c_pointer();

	_Tt_typecb_table_ptr & tcbt = d_procid->otype_callbacks();
	result = type_opnum_callback_add(tcbt, otid, opnum, f);

	if (TT_ERR_PTYPE==result) {
		result = TT_ERR_OTYPE;
	}
	return result;
}
	

static Tt_status
type_opnum_callback_add(_Tt_typecb_table_ptr &callbacks,
			const char *tid, int opnum, Tt_message_callback f)
{
	if (0==tid) {
		return TT_ERR_PTYPE;
	}

	if (0==f) {
		return TT_ERR_POINTER;
	}

	if (callbacks.is_null()) {
		callbacks = new _Tt_typecb_table(_tt_typecb_key);
	}

	_Tt_string key = _tt_typecb_key(tid,opnum);
	_Tt_typecb_ptr tcb = callbacks->lookup(key);
	if (tcb.is_null()) {
		tcb = new _Tt_typecb(tid,opnum);
		callbacks->insert(tcb);
	}

	tcb->add_callback(f);

	return TT_OK;
}

Tt_status
_tt_ptype_opnum_user_set(const char *ptype, int opnum, void *clientdata)
{
	// XXX if ever promoted to real API, use auditing instead
	if ((ptype == 0) || (tt_ptr_error(ptype) != TT_OK)) {
		return TT_ERR_POINTER;
	}

	_Tt_c_procid	*d_procid = _tt_c_mp->default_c_procid().c_pointer();
	_Tt_typecb_table_ptr &callbacks = d_procid->ptype_callbacks();
	_Tt_string key = _tt_typecb_key( ptype, opnum );
	_Tt_typecb_ptr tcb = callbacks->lookup( key );
	if (tcb.is_null()) {
		tcb = new _Tt_typecb( ptype, opnum );
		if (tcb.is_null()) {
			return TT_ERR_NOMEM;
		}
		callbacks->insert( tcb );
	}
	tcb->clientdata = clientdata;
	return TT_OK;
}


void *
_tt_ptype_opnum_user(const char *ptype, int opnum)
{
	// XXX if ever promoted to real API, use auditing instead
	if ((ptype == 0) || (tt_ptr_error( ptype ) != TT_OK)) {
		return error_pointer( TT_ERR_POINTER );
	}

	_Tt_c_procid *d_procid = _tt_c_mp->default_c_procid().c_pointer();
	_Tt_typecb_table_ptr &callbacks = d_procid->ptype_callbacks();
	_Tt_string key = _tt_typecb_key( ptype, opnum );
	_Tt_typecb_ptr tcb = callbacks->lookup( key );
	if (tcb.is_null()) {
		return error_pointer( TT_WRN_NOTFOUND );
	}
	return tcb->clientdata;
}


/*
 tt_trace_control() - toggle tracing

 trace states:
	Unitialized
	Off
	On
	Suspended

Note: a zero-length $TT_TRACE_SCRIPT is the same as TT_TRACE_SCRIPT
not being in the environment at all.  (This is because there is
no MT-safe way to unset environment variables.)  Below, TT_TRACE_SCRIPT
is considered "unset" if getenv() returns NULL or "".

			Uninit	Off	On		Suspended
		      -------------------------------------------
tt_trace_control( 0 ) | -	Uninit	Uninit		Uninit
tt_trace_control( 1 ) | On	On	-		On
tt_trace_control( -1 )| On	On	Suspended	On
tt_*, unset script    | Off	-	-		-
tt_*, script          | On	-	-		-

Uninit => On
Uninit => Off
Off => On
	Reads $TT_TRACE_SCRIPT and unsets it if unless it specifies
	"follow on".

Suspended => On
	Does not reread $TT_TRACE_SCRIPT.

 Returns:
	0	State was On before the call
	1	State was ! On before the call
*/
int
tt_trace_control(int onoff)
{
	int was_on = _Tt_trace::any_tracing();
	switch (onoff) {
	    case 0:
		_Tt_trace::fini();
                break;
	    case -1:
		_Tt_trace::toggle();
		break;
 	    default:
		_Tt_trace::init( 1 );
                break;  
        }
	return was_on;
}
