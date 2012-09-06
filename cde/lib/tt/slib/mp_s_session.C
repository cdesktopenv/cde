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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $TOG: mp_s_session.C /main/6 1999/09/10 19:15:11 mgreess $ 			 				
/*
 * mp_s_session.C 1.25 29 Jul 1993
 *
 * mp_s_session.cc - Server-only methods of the session object
 *
 * Copyright (c) 1990,1992 by Sun Microsystems, Inc.
 */
#include <stdio.h>

#include "mp/mp_global.h"
#include "mp_s_message.h"
#include "mp_s_mp.h"
#include "mp/mp_mp.h"
#include "mp_s_pattern.h"
#include "mp_s_procid.h"
#include "mp_rpc_server.h"
#include "mp_rpc_implement.h"
#include "mp_s_session.h"
#include "mp/mp_desktop.h"
#include "mp/mp_xdr_functions.h"
#include "util/tt_global_env.h"
#include "util/tt_host.h"
#include "util/tt_enumname.h"
#include "util/tt_port.h"
#include "util/tt_gettext.h"
#include <unistd.h>
#include "util/tt_port.h"

#include <X11/Xlib.h>     

#ifdef OPT_UNIX_SOCKET_RPC
#	include <sys/socket.h>
#	include <sys/un.h>
#	include "mp_rpc_server_utils.h"
#endif // OPT_UNIX_SOCKET_RPC

// Use the parent class (_Tt_session) for construction and destruction.
_Tt_s_session::_Tt_s_session () {}

_Tt_s_session::~_Tt_s_session () 
{
#ifdef OPT_UNIX_SOCKET_RPC
	if (_socket_name.len()) {
		// A UNIX domain socket that was bound to a pathname 
		// and therefore must be removed by the owner when the session 
		// server exits. This ensures that when the next session 
		// server starts, it can choose the same pathname and bind
		// to it. The pathname cannot refer to a file existing 
		// on the system.
		(void)unlink(_socket_name);
	}
#endif // OPT_UNIX_SOCKET_RPC
}

// 
// Callback for i/o errors detected for the desktop session. The default
// action is to exit immediately.
// 
int
_tt_xio_handler(Display * /* xdisp */)
{
	exit(0);
	return 0;
}

#ifdef OPT_UNIX_SOCKET_RPC

// 
// Utility routine to open a unix domain socket and bind it to the given
// socket name.
// 
//  XXX: It would be more elegant to add unix domain support to the
//  _Tt_stream_socket class rather than have these special routines for
//  dealing with them.
// 
static int
s_open_unix_socket(char *socket_name)
{
	/* open a unix socket */
	int			sock;
	struct sockaddr_un	server_addr;
	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sock < 0) {
		_tt_syslog(0, LOG_ERR, "s_open_unix_socket(): socket(): %m");
		return(-1);
	}
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sun_family = AF_UNIX;
	(void)unlink(socket_name);
	strcpy(server_addr.sun_path, socket_name);
#if defined(ultrix) || defined(_AIX)  || defined(hpux) || defined(__osf__)
        int servlen = strlen(server_addr.sun_path) + sizeof(server_addr.sun_fam\
ily);
        if (bind(sock, (struct sockaddr *)&server_addr,servlen) < 0) {
#else
	if (bind(sock, (struct sockaddr *)&server_addr,
		 sizeof(struct sockaddr_un)) < 0) {
#endif
		return(-1);
	}
	if (listen(sock, 5) != 0) {
		return(-1);
	}

	return(sock);
}


// 
// Called in response to a client request for a new rpc connection. We
// accept the connection on the unix socket and then use the standard
// _Tt_rpc_server object, which acts accordingly if given an open file
// descriptor in its constructor (see that method for more details) to
// open the rpc connection.
// 
void _Tt_s_session::
u_rpc_init()
{
	int			msgsock;
	_Tt_rpc_server_ptr	server_object;

	//  connection request came in for the unix socket
	msgsock = accept(_u_sock, (struct sockaddr *)0, 0);
	if (msgsock == -1) {
		return;
	}
	server_object = new _Tt_rpc_server(_rpc_program,
					   TT_RPC_VERSION,
					   msgsock,
					   _auth);
	if (! server_object->init(_tt_service_rpc)) {
		_tt_syslog(0, LOG_ERR, "u_rpc_init: server_object->init(): 0");
	}	
}
#endif // OPT_UNIX_SOCKET_RPC


// 
// This is the server-side init routine for the _Tt_session object. The
// basic duties for this method are to advertise the server address
// accordingly after initializing the rpc server object. If we are in a
// desktop session then we first check for a session already running and
// return an error code if we find one.
//
// Returns:
//	TT_OK
//	TT_ERR_SESSION	Found a live session (diagnostic has been emitted)
//	TT_ERR_NOMP	Could not initialize (diagnostic has been emitted)
// 
Tt_status _Tt_s_session::
s_init()
{
	_Tt_session		rsession;
	Tt_status		status;
	_Tt_string		h;
	_Tt_desktop_lock_ptr	dt_lock;

	// this is the server session for this session
	_is_server = 1;
	if (env() == _TT_ENV_X11) {
		if (_displayname.len() == 0) {
			_displayname = _tt_global->xdisplayname;
		}
		_desktop = new _Tt_desktop();
		if (! _desktop->init(_displayname, _TT_DESKTOP_X11)) {
			return(TT_ERR_NOMP);
		}
		_desktop->set_error_handler((_Tt_dt_errfn)_tt_xio_handler);

		// set this field in the _Tt_s_mp object which will
		// then be used to add the fd for the desktop
		// connection to the list of fds that get polled.
		_tt_s_mp->xfd = _desktop->notify_fd();

		//
		// Grab the X server.  The grab is in effect until
		// dt_lock goes out of scope (i.e., when we return).
		// For proper test-and-set, the grab needs to start
		// before check_for_live_session() and end after
		// advertise_address().
		//
		dt_lock = new _Tt_desktop_lock( _desktop );

		// check for a session already running
		switch (status = check_for_live_session()) {
		    case TT_OK:
		    case TT_ERR_NOMP:
			// Muscle in on dead ttsessions
			status = TT_OK;
			break;
		    case TT_ERR_SESSION:
			return TT_ERR_SESSION;
		    default:
			_tt_syslog(0, LOG_ERR,
				   "_Tt_s_session::check_for_live_session(): %s",
				   _tt_enumname(status));
			return TT_ERR_INTERNAL;
		}
	}
	_pid = getpid();
	_server_uid = getuid();
	_queued_messages = new _Tt_message_list();
	_properties = new _Tt_session_prop_list;
	
	// Create an RPC server managing object which will
	// invoke the appropiate dispatch function on any RPC
	// requests. This object also allocates an RPC program
	// number which is then used to compose the session id to
	// be advertised to potential clients.
	_rpc_server = new _Tt_rpc_server(RPC_ANYSOCK,
					 TT_RPC_VERSION,
					 RPC_ANYSOCK,
					 _auth); 
	
	if (! _rpc_server->init(_tt_service_rpc)) { 
		return(TT_ERR_NOMP);
	}
	_rpc_program = _rpc_server->program();
	_rpc_version = _rpc_server->version();

	// initializes our local host object which allows us to
	// inquire our host address so we can then advertise it to
	// clients. 
	if (! _tt_global->get_local_host(_host)) {
		_tt_syslog(0, LOG_ERR,"get_local_host(): 0");
		return(TT_ERR_NOMP);
	}

	if (parsed_address(_address_string) != TT_OK) {
		_tt_syslog(0, LOG_ERR,"parsed_address() != TT_OK");
		return(TT_ERR_NOMP);
	}

	if (_TT_AUTH_ICEAUTH == _auth.auth_level() &&
	    (status = _auth.generate_auth_cookie()) != TT_OK) {
		return status;
	}

	if ((status = set_id()) == TT_OK) {
		// advertise our address according to the type of
		// session environment that we are in.

		if ((status = advertise_address()) == TT_OK) {
#ifdef OPT_UNIX_SOCKET_RPC
			// open a unix domain socket for connection
			// requests and set the unix_fd field in the
			// _Tt_s_mp object to it so it can be added to
			// the list of file descriptors polled.
			_u_sock = s_open_unix_socket(local_socket_name());
			_tt_s_mp->unix_fd = _u_sock;
#endif	// OPT_UNIX_SOCKET_RPC
			return(TT_OK);
		} else {
			return(status);
		}
	} else {
		return(status);
	}
}


// 
// Used to check whether there is already a live session. Only desktop
// sessions need this check since process-tree sessions are always unique
// since they are based on the process hierarchy. 
// 
//  XXX: If the range of environments were to be extended to more than
//  just desktop and process-tree sessions we would need more abstract
//  methods for session environments that return whether this check is
//  necessary. 
//
// Returns:
//	TT_OK		Found no session
//	TT_ERR_SESSION	Found a live session (diagnostic emitted)
//	TT_ERR_NOMP	Found a dead session (diagnostic emitted)
// 
Tt_status _Tt_s_session::
check_for_live_session()
{
	_Tt_session		rsession;
	Tt_status		status;


	if (env() != _TT_ENV_X11) {
		return(TT_OK);
	}

	// try to find the address of an advertised session
	if (find_advertised_address(rsession._address_string) == TT_OK) {
		// found another server id, check to see that it's
		// running. 
		
		rsession._desktop = _desktop;
		rsession._displayname = _displayname;
		status = rsession.client_session_init();
		switch (status) {
			Tt_status ping_status;
		      case TT_OK:
			switch (ping_status = rsession.ping()) {
			    case TT_OK:
			    case TT_ERR_INVALID:
			    case TT_ERR_NO_MATCH:
			    case TT_ERR_UNIMP:
				_tt_syslog(stderr, LOG_ERR,
					   catgets(_ttcatd, 2, 6,
						   "Found another session run"
						   "ning (host=%s, pid=%d)"),
					   (char *)(rsession.host()->name()),
					   rsession._pid);
				return(TT_ERR_SESSION);
			    default:
				_tt_syslog(0, LOG_ERR,
					   "_Tt_session::ping(): %s",
					   _tt_enumname(ping_status));
				return TT_ERR_INTERNAL;
			    case TT_ERR_NOMP:
				break; // and fall through
			}
			// Fall through
		      case TT_ERR_NOMP:
			{const char *hostname = "";
			if (! rsession.host().is_null()) {
				hostname = rsession.host()->name();
			}
			_tt_syslog(stderr, LOG_ERR,
				   catgets(_ttcatd, 2, 7,
					   "Can't contact alleged session "
					   "(host=%s, pid=%d); "
					   "assuming it crashed..."),
				   hostname, rsession._pid);
			return(TT_ERR_NOMP);}
		      case TT_ERR_INVALID:
			_tt_syslog(stderr, LOG_ERR,
				   catgets(_ttcatd, 2, 8,
					   "Can't parse advertised session id;"
					   " overwriting it with my own..."));
			return(TT_ERR_NOMP);
		      case TT_WRN_NOTFOUND:
		      case TT_ERR_ACCESS:
		      case TT_ERR_NO_MATCH:
			return(TT_OK);
		      default:
			_tt_syslog(0, LOG_ERR,
				   "_Tt_session::client_session_init(): %s",
				   _tt_enumname(status));
			return(TT_ERR_INTERNAL);
		}
	}
	
	return(TT_OK);
}

#if defined(ultrix)
/* strdup - make duplicate of string s */
#include <string.h>
#include <malloc.h>

static char *
strdup(char *s)
{
    char *p;

    p = (char *)malloc(strlen(s) + 1);  /* + 1 for '\0' character */
    if (p != NULL) 
        strcpy(p,s);
    return p;
}
#endif

// 
// Advertises a session procid so that any clients that come up within
// the appropiate domain will find the session id.
// 
Tt_status _Tt_s_session::
advertise_address()
{
	_Tt_string	s;
	_Tt_string	prop(TT_XATOM_NAME);
	_Tt_string	cde_prop(TT_CDE_XATOM_NAME);
	_Tt_string	xdisp("DISPLAY");
	
	switch(env()) {
	      case _TT_ENV_X11:
		// advertise our address by setting a special property
		// on our desktop session.

		if (_desktop->set_prop(cde_prop, _address_string) &&
		    _desktop->set_prop(prop, _address_string))  {
			s = xdisp.cat("=").cat(_displayname);
			(void)putenv(strdup((char *)s));
			return(TT_OK);
		}
		return(TT_ERR_INTERNAL);
	      case _TT_ENV_PROCESS_TREE:
		// advertise our address by exporting a special
		// environment variable.

		if (_tt_put_all_env_var (2, _address_string, (char*)cde_prop, (char*)prop) != 2) {
			return(TT_ERR_INTERNAL);
		}
		return(TT_OK);
	      case _TT_ENV_LAST:
	      default:
		return(TT_ERR_INTERNAL);
	}
}


// 
// This function is called to notify the session object that a new
// pattern has been registered by some procid. This means that for any
// queued messages, there may now be a procid that is suitable to receive
// the message so this function iterates through all the queued messages
// attempting to deliver the message.
// 
// XXX: For efficiency, this method might take an argument which is the
// new pattern and then delivery of the queued messages could be
// restricted to just matching against the new pattern.
// 
void _Tt_s_session::
pattern_added()
{
	_Tt_message_list_cursor		qmsgs;
	_Tt_s_message			*sm;
	
	/* try to deliver session queued msgs */
	qmsgs.reset(_queued_messages);
	while (qmsgs.next()) {
		sm = (_Tt_s_message *)(*qmsgs).c_pointer();
		// set state to "sent"
		sm->set_state(TT_SENT);
		_Tt_msg_trace trace( **qmsgs, TTDR_SESSION_JOIN );
		if (sm->deliver( trace )) {
			qmsgs.remove();
		} else {
			// couldn't successfully deliver so revert
			// state to queued.
			sm->set_state(TT_QUEUED);
		}
	}
}


// 
// The given procid wishes to join this session. We iterate through the
// procid's patterns and add our session id to them. We also invoke the
// pattern_added method since we may have an opportunity to deliver more
// queued messages.
// 
Tt_status _Tt_s_session::
s_join(_Tt_s_procid_ptr &procid)
{
	// update session-scoped handler patterns with session id
	mod_session_id_in_patterns(procid->patterns(), 1);
	procid->set_active(1);
	if (procid->is_active()) {
		// we've altered the patterns registered so we call
		// pattern_added() to attempt delivery of any
		// queued messages. 
		pattern_added();
	}
	return(TT_OK);
}


// 
// For each pattern in patterns that is session-scoped, this method will
// add or delete (depending on add parameter) this session's id to the
// session list for the pattern.
// 
void _Tt_s_session::
mod_session_id_in_patterns(_Tt_pattern_list_ptr &patterns, int add)
{
	_Tt_pattern_list_cursor		pcursor;
	int				scopes;
	
	pcursor.reset(patterns);
	while (pcursor.next()) {
		scopes = pcursor->scopes();
		if (scopes&(1<<TT_SESSION) ||
		    scopes&(1<<TT_FILE_IN_SESSION) ||
		    scopes&(1<<TT_BOTH)) {
			if (add) {
				pcursor->add_session(address_string());
				// set special pattern flag to
				// optimize session-scoped pattern
				// matching. 
				pcursor->set_in_session();
			} else {
				pcursor->del_session(address_string());
				// clear special pattern flag to
				// optimize session-scoped pattern
				// matching. 
				pcursor->clr_in_session();
			}
		}
	}
}


// 
// The given procid wishes to quit this session. We remove our session id
// from the procid's patterns.
// 
Tt_status _Tt_s_session::
s_quit(_Tt_s_procid_ptr &procid)
{
	// delete session from handler patterns
	mod_session_id_in_patterns(procid->patterns(), 0);
		
	return(TT_OK);
}


// 
// Adds a message to the queue of session-queued messages.
// 
void _Tt_s_session::
queue_message(_Tt_message_ptr &m)
{
	if (! m->handler_ptype().len()) {
		// can't queue a message with no handler ptype
		return;
	}

	_Tt_message_list_cursor	qm;
	Tt_state		qm_state;
	_Tt_string		qm_ptype;
	
	// verify that only one message with m's id and handler_ptype
	// exists in the queue (in the same state as m)
	qm.reset(_queued_messages);
	while (qm.next()) {
		if (qm->is_equal(m)) {
			qm_state = qm->state();
			if (qm_state == m->state()) {
				qm_ptype = qm->handler_ptype();
				if (qm_ptype == m->handler_ptype()){
					return;
				}
			}
		}
	}

	_queued_messages->append(m);
}


_Tt_message_list_ptr & _Tt_s_session::
queued_messages()
{
	return(_queued_messages);
}

