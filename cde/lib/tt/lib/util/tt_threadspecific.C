// $XConsortium: tt_threadspecific.C /main/3 1996/05/09 20:50:37 drk $
//%%  (c) Copyright 1995 Sun Microsystems, Inc.			

#include <util/tt_threadspecific.h>
#ifdef OPT_XTHREADS

_Tt_threadspecific::
_Tt_threadspecific(const _Tt_c_session_ptr& sess, const _Tt_c_procid_ptr& proc)
{
	_thread_c_session = sess;
	_thread_c_procid = proc;
	_thread_stgstack = 0;
}

_Tt_api_stg_stack* _Tt_threadspecific::
thread_stgstack()
{
	if (!_thread_stgstack) {
		_thread_stgstack = new _Tt_api_stg_stack;
	}

	return _thread_stgstack;
}

void _Tt_threadspecific::
free_procid()
{
	_thread_c_procid = (_Tt_c_procid *) 0;
}
#endif	// OPT_XTHREADS
