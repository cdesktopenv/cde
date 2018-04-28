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
//%%  $TOG: api_default.C /main/9 1999/10/14 18:39:00 mgreess $ 			 				
/*
 *
 * api_default.cc
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */
#include "db/tt_db_file.h"
#include "mp/mp_c.h"
#include "util/tt_path.h"
#include "api/c/tt_c.h"
#include "api/c/api_api.h"
#include "api/c/api_mp.h"
#include <util/tt_audit.h>
#include "util/tt_global_env.h"
#include "util/tt_threadspecific.h"

char           *
tt_default_ptype(void)
{
	_Tt_audit audit;
	Tt_status status = audit.entry("v", TT_DEFAULT_PTYPE, 0);
	char *retval;


	if (status != TT_OK) {
		audit.exit((char *) _tt_error_pointer(status));
		return (char *) _tt_error_pointer(status);
	}

	retval = _tt_default_ptype();
	audit.exit(retval);
	return retval;
}


Tt_status 
tt_default_ptype_set(const char *ptid)
{
	_Tt_audit audit;
	Tt_status status = audit.entry("C", TT_DEFAULT_PTYPE_SET, ptid);


	if (status != TT_OK) {
		audit.exit(status);
		return status;
	}

	status =  _tt_default_ptype_set(ptid);

	audit.exit(status);
	return status;
}


char           *
tt_default_file(void)
{
	_Tt_audit audit;
	Tt_status status = audit.entry("v", TT_DEFAULT_FILE, 0);
	char *retval;


	if (status != TT_OK) {
		audit.exit((char *) _tt_error_pointer(status));
		return (char *) _tt_error_pointer(status);
	}

	retval = _tt_default_file();
	audit.exit(retval);
	return retval;
}


Tt_status 
tt_default_file_set(const char *docid)
{
	_Tt_audit audit;
	Tt_status status = audit.entry("c", TT_DEFAULT_FILE_SET, docid);


	if (status != TT_OK) {
		audit.exit(status);
		return status;
	}

	status = _tt_default_file_set(docid);
	audit.exit(status);
	return status;
}


char           *
tt_default_session(void)
{
	_Tt_audit audit;
	Tt_status status = audit.entry("v", TT_DEFAULT_SESSION, 0);
	char *retval;


	if (status != TT_OK) {
		audit.exit((char *) _tt_error_pointer(status));
		return (char *) _tt_error_pointer(status);
	}

	retval = _tt_default_session();
	audit.exit(retval);
	return retval;
}


Tt_status
tt_default_session_set(const char *sessid)
{
	_Tt_audit audit;
	Tt_status status = audit.entry("C", TT_DEFAULT_SESSION_SET, sessid);


	if (status != TT_OK) {
		audit.exit(status);
		return status;
	}

	status = _tt_default_session_set(sessid);
	audit.exit(status);
	return status;
}


char           *
tt_thread_session(void)
{
	_Tt_audit audit;
	Tt_status status = audit.entry("v", TT_THREAD_SESSION, 0);
	char *retval;


	if (status != TT_OK) {
		audit.exit((char *) _tt_error_pointer(status));
		return (char *) _tt_error_pointer(status);
	}

#ifdef OPT_XTHREADS
	if (!_tt_global->multithreaded()) {
		// Multithreading not enabled, so this is the same
		// as tt_default_session
		retval = _tt_default_session();
	}
	else {
		retval = _tt_thread_session();
	}
#else
	retval = _tt_default_session();
#endif
	audit.exit(retval);
	return retval;
}


Tt_status
tt_thread_session_set(const char *sessid)
{
	_Tt_audit audit;
	Tt_status status = audit.entry("C", TT_THREAD_SESSION_SET, sessid);


	if (status != TT_OK) {
		audit.exit(status);
		return status;
	}

#ifdef OPT_XTHREADS
	if (!_tt_global->multithreaded()) {
		// Multithreading not enabled, so this not legal
		status = TT_ERR_TOOLATE;
	}
	else {
		status = _tt_thread_session_set(sessid);
	}
#else
	status = _tt_default_session_set(sessid);
#endif
	audit.exit(status);
	return status;
}


char           *
tt_default_procid(void)
{
	_Tt_audit audit;
	Tt_status status = audit.entry("v", TT_DEFAULT_PROCID, 0);
	char *retval;


	if (status != TT_OK) {
		audit.exit((char *) _tt_error_pointer(status));
		return (char *) _tt_error_pointer(status);
	}

	retval = _tt_default_procid();
	audit.exit(retval);
	return retval;
}


Tt_status
tt_default_procid_set(const char *procid)
{
	_Tt_audit audit;
	Tt_status status = audit.entry("C", TT_DEFAULT_PROCID_SET, procid);


	if (status != TT_OK) {
		audit.exit(status);
		return status;
	}

	status = _tt_default_procid_set(procid);
	audit.exit(status);
	return status;
}


char           *
tt_thread_procid(void)
{
	_Tt_audit audit;
	Tt_status status = audit.entry("v", TT_THREAD_PROCID, 0);
	char *retval;


	if (status != TT_OK) {
		audit.exit((char *) _tt_error_pointer(status));
		return (char *) _tt_error_pointer(status);
	}

#ifdef OPT_XTHREADS
	if (!_tt_global->multithreaded()) {
		// Multithreading not enabled, so this is the same
		// as tt_default_procid()
		retval = _tt_default_procid();
	}
	else {
		retval = _tt_thread_procid();
	}
#else
	retval = _tt_default_procid();
#endif

	audit.exit(retval);
	return retval;
}


Tt_status
tt_thread_procid_set(const char *procid)
{
	_Tt_audit audit;
	Tt_status status = audit.entry("C", TT_THREAD_PROCID_SET, procid);


	if (status != TT_OK) {
		audit.exit(status);
		return status;
	}

#ifdef OPT_XTHREADS
	if (!_tt_global->multithreaded()) {
		// Multithreading not enabled, so this is not legal
		status = TT_ERR_TOOLATE;
	}
	else {
		status = _tt_thread_procid_set(procid);
	}
#else
	status = _tt_default_procid_set(procid);
#endif

	audit.exit(status);
	return status;
}

char           *
tt_procid_session(const char* procid)
{
	_Tt_audit audit;
	Tt_status status = audit.entry("C", TT_PROCID_SESSION, procid);
	char *retval;

	if (status != TT_OK) {
		audit.exit((char *) _tt_error_pointer(status));
		return (char *) _tt_error_pointer(status);
	}

	retval = _tt_procid_session(procid);

	audit.exit(retval);
	return retval;
}

Tt_status tt_feature_enabled(Tt_feature f)
{
	_Tt_audit audit;
	Tt_status status = audit.entry("F", TT_FEATURE_ENABLED, f);

	if (status != TT_OK) {
		audit.exit(status);
		return status;
	}

#ifdef OPT_XTHREADS
	status =  _tt_feature_enabled(f);
#else
	status = TT_ERR_UNIMP;
#endif

	// Hack down the event counter, since this call shouldn't count.

	if (_tt_global->event_counter > 0) _tt_global->event_counter--;

	audit.exit(status);
	return status;
}

Tt_status tt_feature_required(Tt_feature f)
{
	_Tt_audit audit;
	Tt_status status = audit.entry("F", TT_FEATURE_REQUIRED, f);

	if (status != TT_OK) {
		audit.exit(status);
		return status;
	}

#ifdef OPT_XTHREADS
	status = _tt_feature_required(f);
#else
	status = TT_ERR_UNIMP;
#endif

	audit.exit(status);
	return status;
}

/* 
 * Return the default ptype for the current proc or NULL if none has been
 * declared. 
 */
char *
_tt_default_ptype(void)
{
	return _tt_strdup(_tt_c_mp->default_c_procid()->default_ptype());
}


/* 
 * Sets the default ptype to ptid.
 */
Tt_status
_tt_default_ptype_set(const char * ptid)
{
	_Tt_string p = ptid;

	_tt_c_mp->default_c_procid()->set_default_ptype(p);

	return TT_OK;
}


/* 
 * Returns the default file or NULL if none has been declared.
 */
char *
_tt_default_file(void)
{
	_Tt_c_procid	*d_procid = _tt_c_mp->default_c_procid().c_pointer();

	return _tt_strdup(
		_tt_network_path_to_local_path(d_procid->default_file()));
}


/* 
 * Sets the default file 
 */
Tt_status
_tt_default_file_set(const char *filepath)
{
        _Tt_string abspath = _Tt_db_file::getNetworkPath(filepath);
        if ((filepath != 0) && (abspath.len() <= 0)) {
                return TT_ERR_FILE;
        }
	_tt_c_mp->default_c_procid()->set_default_file(abspath);
	return TT_OK;
}

/* 
 * Returns the id of the default session.
 */
char *
_tt_default_session()
{
	if (_tt_c_mp->default_c_session.is_null()) {
		return (char *)_tt_error_pointer(TT_ERR_SESSION);
	}
	return _tt_strdup(_tt_c_mp->default_c_session->id());
}


/* 
 * Sets the id of the default session.
 */
Tt_status
_tt_default_session_set(const char *sessid)
{
	_Tt_string		strsessid = (char *)0;
	_Tt_c_session_ptr	sptr = new _Tt_c_session;
	Tt_status		status;

	// Add the P on if isn't already.
	_prepend_P_to_sessid(sessid, strsessid);

	_tt_internal_init();

	status = _tt_c_mp->find_session(strsessid, sptr, 1, 1);
	if (TT_OK != status) return status;

	_tt_c_mp->default_c_session = sptr;
	return (TT_OK);
}

#ifdef OPT_XTHREADS

/* 
 * Returns the id of the default session for the current thread.
 * NOTE: We only get into this routine if multithreading has been
 * turned on via a call to tt_feature_required
 */
char *
_tt_thread_session()
{
	// Try to get the default session out of thread-specific storage
	_Tt_threadspecific* tss = (_Tt_threadspecific *) 0;
	xthread_get_specific(_tt_global->threadkey, (void **) &tss);
	if (!tss) {
		// thread-specific storage not yet initialized -- do it
		tss = new _Tt_threadspecific(_tt_c_mp->default_c_session,
					     (_Tt_c_procid *) 0);
		xthread_set_specific(_tt_global->threadkey, tss);
	}
	
	_Tt_c_session_ptr thread_session = tss->thread_c_session();

	if (!thread_session.is_null()) {
		return _tt_strdup(thread_session->id());
	}

	// XXX: Probably should never happen....
	if (_tt_c_mp->default_c_session.is_null()) {
		return (char *)_tt_error_pointer(TT_ERR_SESSION);
	}
	return _tt_strdup(_tt_c_mp->default_c_session->id());
}


/* 
 * Sets the id of the default session for the current thread.
 * NOTE: We only get into this routine if multithreading has been
 * turned on via a call to tt_feature_required
 */
Tt_status
_tt_thread_session_set(const char *sessid)
{
	_Tt_string		strsessid = (char *)0;
	_Tt_c_session_ptr	sptr = new _Tt_c_session;
	Tt_status		status;

	// Add the P on if isn't already.
	_prepend_P_to_sessid(sessid, strsessid);

	_tt_internal_init();

	status = _tt_c_mp->find_session(strsessid, sptr, 1, 1);
	if (TT_OK != status) return status;

	_Tt_threadspecific* tss = (_Tt_threadspecific *) 0;
	xthread_get_specific(_tt_global->threadkey, (void **) &tss);
	if (!tss) {
		// thread-specific storage not yet initialized -- do it
		tss = new _Tt_threadspecific(_tt_c_mp->default_c_session,
					     (_Tt_c_procid *) 0);
		xthread_set_specific(_tt_global->threadkey, tss);
	}

	tss->set_thread_c_session(sptr);
	return TT_OK;
}

#endif

/* 
 * Returns the id of the default procid
 */
char *
_tt_default_procid()
{
	return _tt_strdup(_tt_c_mp->default_c_procid()->id());
}


/* 
 * Sets the id of the default procid
 */
Tt_status
_tt_default_procid_set(const char *procid)
{
	return _tt_c_mp->set_default_procid(procid);
}


char           *
_tt_procid_session(const char* procid)
{
	char* retval;
	char* _sessid;
	_Tt_string sessid;
	
	_Tt_procid_ptr p = _tt_c_mp->active_procs->lookup(procid);

	if (p.is_null()) {
		retval = (char *) _tt_error_pointer(TT_ERR_PROCID);
	} else {
		_Tt_c_procid_ptr pp = (_Tt_c_procid *) p.c_pointer();
		_sessid = (char *) pp->default_session()->address_string();
		_prepend_P_to_sessid((const char *) _sessid, sessid);
		retval = (char *) sessid;
	}
	return retval;
}


#ifdef OPT_XTHREADS

/* 
 * Returns the id of the default procid for the current thread
 * NOTE: We only get into this routine if multithreading has been
 * turned on via a call to tt_feature_required
 */
char *
_tt_thread_procid()
{
	_Tt_threadspecific* tss = (_Tt_threadspecific *) 0;
	xthread_get_specific(_tt_global->threadkey, (void **) &tss);
	if (!tss) {
		// thread-specific storage not yet initialized -- do it
		tss = new _Tt_threadspecific(_tt_c_mp->default_c_session,
					     (_Tt_c_procid *) 0);
		xthread_set_specific(_tt_global->threadkey, tss);
	}

	_Tt_c_procid_ptr c_procid = tss->thread_c_procid();

	if (!c_procid.is_null()) {
		return _tt_strdup(c_procid->id());
	}
	else {
		return _tt_strdup(_tt_c_mp->default_c_procid()->id());
	}
}


/* 
 * Sets the id of the default procid for the current thread
 * NOTE: We only get into this routine if multithreading has been
 * turned on via a call to tt_feature_required
 */
Tt_status
_tt_thread_procid_set(const char *procid)
{
	return _tt_c_mp->set_default_procid(procid, 1);
}

Tt_status _tt_feature_enabled(Tt_feature f)
{
	switch (f) {
	    case _TT_FEATURE_MULTITHREADED:
		if (_tt_global->multithreaded()) {
			return TT_OK;
		}
		else {
			return TT_WRN_NOT_ENABLED;
		}
	    default:
		return TT_ERR_UNIMP;
	}
}

Tt_status _tt_feature_required(Tt_feature f)
{
	_tt_internal_init();

	// Note that since we are within an API call the global
	// mutex has been grabbed from within Tt_trace::entry

	switch (f) {
	    case _TT_FEATURE_MULTITHREADED:
		if (!_tt_global->multithreaded()) {
			if (!_tt_global->set_multithreaded()) {
				return TT_ERR_TOOLATE;
			}
			else {
				_tt_c_mp->set_multithreaded();
			}
		}
		return TT_OK;
	    default:
		return TT_ERR_UNIMP;
	}
}

#endif /* OPT_XTHREADS */
