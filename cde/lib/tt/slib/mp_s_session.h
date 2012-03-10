/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: mp_s_session.h /main/3 1995/10/23 11:59:47 rswiston $ 			 				 */
/* 
 * mp_s_session.h
 * 
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 * 
 * This file contains the implementation of the server side of
 * the _Tt_session object.
 */
#ifndef MP_S_SESSION_H
#define MP_S_SESSION_H
#include "mp/mp_session.h"
#include "mp_s_session_utils.h"
#include "mp_s_procid_utils.h"
#include "mp_rpc_server_utils.h"

class _Tt_s_session : public _Tt_session {
      public:
	_Tt_s_session();
	virtual ~_Tt_s_session();

	Tt_status		s_addprop(_Tt_string prop, _Tt_string val);
	Tt_status		s_getprop(_Tt_string prop, int i,
					  _Tt_string &value);
	Tt_status		s_init();
	Tt_status		s_join(_Tt_s_procid_ptr &procid);
	Tt_status		s_propcount(_Tt_string prop, int &cnt);
	Tt_status		s_propname(int i, _Tt_string &prop);
	Tt_status		s_propnames_count(int &cnt);
	Tt_status		s_quit(_Tt_s_procid_ptr &procid);
	Tt_status		s_setprop(_Tt_string prop, _Tt_string val);
	void			queue_message(_Tt_message_ptr &m);
	void			pattern_added();
	Tt_status		check_for_live_session();

      private:
	Tt_status		advertise_address();
	void			mod_session_id_in_patterns(_Tt_pattern_list_ptr
							   &patterns,
							   int add);
	void			u_rpc_init();
	_Tt_message_list_ptr	&queued_messages();

	_Tt_rpc_server_ptr	_rpc_server;

	friend class _Tt_s_mp;
};

#endif				/*  MP_S_SESSION_H */
