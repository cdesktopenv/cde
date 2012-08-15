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
//%%  $TOG: mp_session.C /main/13 1999/09/16 13:46:46 mgreess $ 			 				
/*
 * @(#)mp_session.C	1.84 96/01/10
 *
 * Tool Talk Message Passer (MP) - mp_session.cc
 *
 * Copyright (c) 1990,1992 by Sun Microsystems, Inc.
 *
 * Implementation of the _Tt_session class.
 */
#include "util/tt_global_env.h"
#include "util/tt_xdr_version.h"
#include "util/tt_host.h"
#include "util/tt_port.h"
#include "util/tt_gettext.h"
#include "mp/mp_global.h"
#include "mp/mp_message.h"
#include "mp/mp_mp.h"
#include "mp/mp_c_global.h"
#include "mp/mp_c_session.h"
#include "mp/mp_c_mp.h"
#include "mp/mp_pattern.h"
#include "mp/mp_rpc_client.h"
#include "mp/mp_rpc_interface.h"
#include "mp/mp_session.h"
#include "mp/mp_desktop.h"
#include "mp/mp_xdr_functions.h"
#include <unistd.h>

#ifdef OPT_UNIX_SOCKET_RPC
#	include <sys/socket.h>
#	include <sys/un.h>
#endif // OPT_UNIX_SOCKET_RPC


_Tt_session::
_Tt_session()
{
	_env = _TT_ENV_LAST;
	_is_server = 0;		// default server mode
	_is_dead = 0;
	_rpc_version = 0;
}


_Tt_session::
~_Tt_session()
{
	if (! _desktop.is_null()) {
		if (_is_server && _env == _TT_ENV_X11 && (_id.len() > 0)) {
			if (   (! _desktop->del_prop(TT_XATOM_NAME))
			    || (! _desktop->del_prop(TT_CDE_XATOM_NAME)))
			{
				_tt_syslog( 0, LOG_WARNING,
					    catgets( _ttcatd, 1, 14,
						     "could not delete the X "
						     "root window property %s "
						     "advertising this "
						     "session" ),
					    TT_XATOM_NAME );
			}
		}
	}
#ifdef OPT_UNIX_SOCKET_RPC
	if (_is_server && _socket_name.len()) {
		(void)unlink((char *)_socket_name);
	}
#endif
}



#ifdef OPT_UNIX_SOCKET_RPC
// 
// Returns the name of the socket file if the unix socket rpc option is
// enabled. Note that the name should be unique for all sessions running
// on the same machine so it has to be derived from the session id
// (which has the required uniqueness properties).
// 
char * _Tt_session::
local_socket_name()
{
#define SPRFX "/tmp/.TT"

	if (_socket_name.len() == 0) {
		char *sname = (char *)malloc(_id.len() + strlen(SPRFX) + 1); 
		char *sc;

		sprintf(sname,"%s%s", SPRFX, (char *)_id);
		sc = sname;
		while (*sc) {
			if (*sc == ' ') {
				*sc = '_';
			}
			sc++;
		}
		_socket_name = sname;
		free((MALLOCTYPE *)sname);
	}
	
	return((char *)_socket_name);
}

// 
// Opens a connection to the unix socket bound to the given socket name.
// If successful returns the fd for the connection. Otherwise, returns
// -1. 
//
int
c_open_unix_socket(char *socket_name)
{
	int			sock;
	struct sockaddr_un	server_addr;
	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sock < 0) {
		_tt_syslog( 0, LOG_ERR, "c_open_unix_socket(): socket(): %m" );
		return -1;
	}
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sun_family = AF_UNIX;
	strcpy(server_addr.sun_path, socket_name);
#if defined(ultrix) || defined(_AIX) || defined(hpux) || defined(__osf__)
        int servlen = strlen(server_addr.sun_path) + sizeof(server_addr.sun_fam\
ily);
        if (connect(sock, (sockaddr *)&server_addr, servlen) < 0) {
#else
	if (connect(sock, (sockaddr *)&server_addr, sizeof(sockaddr_un)) < 0) {
#endif
		return(-1);
	}

	return(sock);
}

#endif // OPT_UNIX_SOCKET_RPC


//
// Called if an event comes in from our desktop connection.
//
int  _Tt_session::
desktop_event_callback()
{
	if (! _desktop->process_event()) {
		return (-1);
	}
	else 
		return 0;
}


// 
// Initializes a session with the assumption that the server session is
// already running. If the server session isn't running then this method
// will return an error as opposed to attempting to autostart a new
// session (see _Tt_session::c_init). This is intended as a lower-level
// init function used by c_init or whenever autostarting is not required.
//
// Returns:
//	TT_OK
//	TT_ERR_ACCESS	Could not init desktop (diagnostic emitted)
//	TT_WRN_NOTFOUND	No advertised address
//	TT_ERR_INVALID	Could not parse advertised address
//	TT_ERR_NO_MATCH	Advertised address's version is too new
//			(diagnostic emitted)
//	TT_ERR_ADDRESS	Could not find advertised host
//	TT_ERR_NOMP	Could not init as rpc client
// 
Tt_status _Tt_session::
client_session_init()
{		
	// this is a client of this session. Note that since this
	// function may be called in the server, we can't use
	// _tt_mp->in_server().
	//
	Tt_status err;
	int			rpc_init_done = 0;
	_Tt_host_ptr		lh;


	_is_server = 0;
	if (env() == _TT_ENV_X11 && _desktop.is_null()) {
		_desktop = new _Tt_desktop();
		if (! _desktop->init(_displayname, _TT_DESKTOP_X11)) {
			return(TT_ERR_ACCESS);
		}
	}
	if (_address_string.len() == 0) {
		if (find_advertised_address(_address_string) != TT_OK) {
			return(TT_WRN_NOTFOUND);
		}
	}
	if (_address_string.len() == 0) {
		// Don't call parsed_address() with an empty string,
		// or it will think we're trying to create an address
		// string instead of parse it.  parsed_address()
		// shouldn't be this cute. XXX
		return TT_ERR_INVALID;
	}
	if ((err = parsed_address(_address_string)) != TT_OK) {
		_address_string = (char *)0;
		return(err);
	}
	if (_TT_AUTH_ICEAUTH == _auth.auth_level() &&
	    (err = _auth.retrieve_auth_cookie()) != TT_OK) {
		return err;
	}
	if (! _tt_global->find_host(_hostaddr, _host, 1)) {
		return(TT_ERR_ADDRESS);
	}

	if (_id.len() == 0) {
		(void)set_id();
	}

#ifdef OPT_UNIX_SOCKET_RPC
	if (_tt_global->get_local_host(lh)) {
		// if we are in the same host as the server session
		// then we attempt to connect to its advertised unix
		// socket to set up an rpc connection. If we succeed
		// then we set rpc_init_done to 1 to prevent a normal
		// tcp rpc connection from being made.
		if (lh->stringaddr() == _host->stringaddr()) {
			_u_sock = c_open_unix_socket(local_socket_name());
			if (_u_sock != -1) {
				_rpc_client = new _Tt_rpc_client(_u_sock);
				if (_rpc_client->init(lh, _rpc_program,
						      _rpc_version,
						      _server_uid,
						      &_auth)) {
					rpc_init_done = 1;
					// Since we opened the fd ourselves,
					// clnt_destroy won\'t close it
					// unless we tell it it\'s OK.
					clnt_control(_rpc_client->rpc_handle(),
						     CLSET_FD_CLOSE,
						     (char *) NULL);
				}
			}
		}
	}
#endif	// OPT_UNIX_SOCKET_RPC
	if (! rpc_init_done) {
		_rpc_client = new _Tt_rpc_client();
		if (! _rpc_client->init(_host, _rpc_program, _rpc_version,
					_server_uid, _auth)) {
			return(TT_ERR_NOMP);
		}
	}

	// Get and save the file descriptor for this session.
	_tt_mp->save_session_fd(_rpc_client->socket());
	return(TT_OK);
}


// 
// Verifies that the server side of the session is alive and is a
// ToolTalk session. Currently this is done by first invoking rpc
// procedure 0 to see if the session is responding to rpc requests. If
// that succeeds then we invoke rpc procedure TT_RPC_VRFY_SESSION which
// is a reasonably large number. The reply to this procedure should be
// the session id of the session we're talking to. This is a fairly good
// (but not foolproof!) method of verifying that this is a ToolTalk
// session.
//
// In fact it seems to be too picky, if the session id we have is
// different only in the IP address, it may just be an alternate
// IP address for the very same session.  
//
// Returns:
//	TT_OK
//	TT_ERR_NOMP	Session unreachable or a hoaxer
//	TT_ERR_NO_MATCH	Session has wrong version
//	TT_ERR_INVALID	Could not parse advertised address
//	TT_ERR_AUTHORIZATION	RPC authorization error
//
Tt_status _Tt_session::
ping()
{
	Tt_status	is_live;
	_Tt_string	sid;
	
	// We used to call the NULLPROC here first, but that seems
	// to be a waste of a round-trip since it doesn't tell
	// us anything that the TT_RPC_VRFY_SESSION call doesn't.

	is_live = call((int)TT_RPC_VRFY_SESSION,
		       (xdrproc_t)xdr_void, 0,
		       (xdrproc_t)tt_xdr_string,
		       (char *)&sid);

	// We used to string-compare the returned session id with the one we
	// have, but that was too picky since the IP address can usefully be
	// different.  Skip the check for now, perhaps we should check at
	// least some of the components (UNIX pid?)  We already know the
	// transient RPC number is good, and most everything else is
	// constant.  We do make a minimal check that the id string is non
	// null, this might catch the (unlikely) case where the program at
	// the other end isn't a ttsession but does happen to have a
	// procedure with a number the same as TT_RPC_VRFY_SESSION,
	// although in that case I'd expect the RPC to fail in the
	// XDR routines.

	if (is_live == TT_OK && 0 == sid.len()) {
		is_live = TT_ERR_NOMP;
	}


	if (is_live == TT_ERR_UNIMP) {
		is_live = TT_ERR_INTERNAL;
	}

	// And finally, for reasons I haven't quite figured out,
	// if the session id we have *isn't* equal to the one returned
	// by ttsession, file scoped messages fail.  I suspect
	// pattern matching somewhere compares the ids.
	// So, if the ttsession thinks its name is different, replace
	// our idea of the name with its idea of its name.

	if (is_live == TT_OK) {
		set_id(sid);
	}

	return(is_live);
}


// 
// Invokes an rpc call named by the rpc_proc parameter on the server
// session and returns any results. If timeout is anything other than -1
// then it will be used as the timeout value. Otherwise a suitable
// timeout value is chosen. If rebind is 1 then a new rpc connection will
// be attempted if this rpc fails.
//
// Returns:
//	TT_OK
//	TT_ERR_NOMP	Could not talk to session
//	TT_ERR_VERSION_MISMATCH	RPC VERSMISMATCH
//	TT_ERR_UNIMP	rpc_proc not implemented by session
//	TT_ERR_INVALID	Could not parse address
//	TT_ERR_AUTHORIZATION	RPC authorization error
//
Tt_status _Tt_session::
call(int rpc_proc,
     xdrproc_t xdr_arg_fn, char *arg,
     xdrproc_t xdr_res_fn, char *res,
     int timeout, int rebind)
{
	//
	// Ensure global xdr version is set to proper value for this session.
	//
	int xdr_version_2_use = _rpc_version;
	//
	// Some rpc calls use an xdr version number that is greater
	// than the version number of the rpc protocol they exist in.
	// See e.g. _tt_rpc_add_pattern_with_context().
	//
	switch (rpc_proc) {
	    case TT_RPC_ADD_PATTERN_WITH_CONTEXT:
		if (xdr_version_2_use < TT_CONTEXTS_XDR_VERSION) {
			xdr_version_2_use = TT_CONTEXTS_XDR_VERSION;
		}
		break;
	}
	_Tt_xdr_version ver(xdr_version_2_use);
	Tt_status		status;
	clnt_stat		rpc_status;
	int			processing;
	int			retry;
	int			tmout;

	if (_rpc_client.is_null()) {
		return(TT_ERR_INTERNAL);
	}

	if (timeout == -1) {
		switch (rpc_proc) {
		      case TT_RPC_HDISPATCH:
		      case TT_RPC_HUPDATE_MSG:
			rebind = 1;
			/* fall through */
		      case TT_RPC_DISPATCH_2:
		      case TT_RPC_DISPATCH_2_WITH_CONTEXT:
		      case TT_RPC_UPDATE_MSG_2:
		      case TT_RPC_MSGREAD_2:
			tmout = -1;
			break;
		      default:
			tmout = TT_RPC_TMOUT;
			break;
		}
	} else {
		tmout = timeout;
	}

	retry = 5;
	processing = 1;
	status = TT_ERR_NOMP;
	while (processing && retry >= 0) {
		if (_is_dead) {
			// We sometimes hang if we call a dead session
			rpc_status = RPC_CANTRECV;
			if (! _rpc_client->init(_host, _rpc_program,
						_rpc_version,
						_server_uid,
						_auth)) {
				status = TT_ERR_NOMP;
				processing = 0;
				break;
			}

		} else {
			rpc_status = _rpc_client->call(rpc_proc, xdr_arg_fn,
						       arg, xdr_res_fn, res,
						       tmout);
		}
		switch (rpc_status) {
		      case RPC_SUCCESS:
			status = TT_OK;
			processing = 0;
			break;
		      case RPC_CANTRECV:
			_is_dead = 1;
			status = TT_ERR_NOMP;
			retry--;
			break;
		      case RPC_VERSMISMATCH:
		      case RPC_PROGVERSMISMATCH:
			status = TT_ERR_VERSION_MISMATCH;
			processing = 0;
			break;
		      case RPC_TIMEDOUT:
			if (tmout == 0 || tmout == -1) {
				// rpc always returns a timeout error if we're
				// in message-passing mode
				status = TT_OK;
			} else {
				// network errors
				status = TT_ERR_NOMP;
			}
			processing = 0;
			break;
		      case RPC_CANTSEND:
			if (!rebind) {
				status = TT_ERR_NOMP;
				retry--;
				break;
			}
			// try to rebind _rpc_client and try again
			if (! _rpc_client->init(_host, _rpc_program,
						_rpc_version,
						_server_uid,
						_auth)) {
				status = TT_ERR_NOMP;
				processing = 0;
				break;
			}
			continue;
		      case RPC_PROCUNAVAIL:
			// We get this if there is a mild version 
			// mismatch, e.g. a 1.1 library talking to 
			// a 1.0.x server, and one of the new
			// API calls is used.  This is relatively
			// benign, just let the user know that
			// his call is unimplemented in this system
			status = TT_ERR_UNIMP;
			processing = 0;
			break;
		      case RPC_AUTHERROR:
			status = TT_ERR_AUTHORIZATION;
			processing = 0;
			break;
		      default:
			status = TT_ERR_INTERNAL;
			processing = 0;
			break;
		}
	}

	if (status == TT_ERR_NOMP &&
	    !_tt_mp->in_server() &&
	    !_tt_c_mp->default_c_session.is_null() &&
	    has_id(_tt_c_mp->default_c_session->address_string())) {
		_tt_c_mp->default_c_session = NULL;
	}

	return(status);
}


// 
// Attempts to find the address of the server session. This is done
// according to what kind of environment or session type we're in. 
//
Tt_status _Tt_session::
find_advertised_address(_Tt_string &session_addr)
{
	switch (env()) {
	      case _TT_ENV_X11:
		if ((! _desktop->get_prop(TT_CDE_XATOM_NAME, session_addr)) &&
                    (! _desktop->get_prop(TT_XATOM_NAME, session_addr))) {
			return(TT_ERR_NOMP);
		}
		return(TT_OK);
	      case _TT_ENV_PROCESS_TREE:
		if (_address_string.len() == 0) {
			session_addr = _tt_get_first_set_env_var(2, TT_CDE_XATOM_NAME, TT_XATOM_NAME);
		} else {
			session_addr = _address_string;
		}
		return(TT_OK);		
	      case _TT_ENV_LAST:
	      default:
		return(TT_ERR_NOMP);
	}
}


//
// Returns a number indicating what type of session we're in.
//
_Tt_env _Tt_session::
env()
{
	if (_tt_mp->in_server()) {
		if (_env == _TT_ENV_LAST && getenv("DISPLAY")) {
			_env = _TT_ENV_X11;
		}
		return(_env);
	} else if (_env != _TT_ENV_LAST) {
		return(_env);
	} else {
		if (_tt_get_first_set_env_var(2, TT_CDE_XATOM_NAME, TT_XATOM_NAME)) {
			return(_env = _TT_ENV_PROCESS_TREE);
		} else if (getenv("DISPLAY")) {
			return(_env = _TT_ENV_X11);
		} else {
			return(_TT_ENV_LAST);
		}
	}
}


// 
// Sets the session-type for the session. This affects how the session id
// is advertised to potential clients. 
// 
void _Tt_session::
set_env(_Tt_env session_env, _Tt_string arg)
{
	_env = session_env;
	switch (_env) {
	      case _TT_ENV_X11:
		_displayname = arg;
		break;
	      default:
		break;
	}
}



// 
// Returns the ToolTalk session id for the X session named by xdisp. Note
// that no actual contact is attempted.
// 
_Tt_string _Tt_session::
Xid(_Tt_string xdisp)
{
	return _desktop->session_name(xdisp);
}

/*
void _Tt_session::
print(FILE *fs) const
{
	fprintf(fs,"_Tt_session::\n");
}
*/

//
// Used to set or get the address string of a session as well as set
// appropiately the _rpc_version field of the session. If addr_string is
// null, then parsed_address will set the address string of the session.
// In this case, it sets _rpc_version to the default TT_RPC_VERSION. If
// addr_string is not null then parsed_address will attempt to parse the
// string and extract the information in it. In this case, if the
// addr_string happens to be an fcs1 version of session addresses then
// _rpc_version is set to 1, otherwise it is set to TT_RPC_VERSION.
//
// Note that the intended convention for address strings is that only the
// information that is known to this method will be parsed out of them.
// If there is any information beyond that then it is left uninterpreted.
// This means that future ToolTalk versions are free to append
// information to the end of an address string (as is already done
// below) without compromising compatibility with older clients.
//
// Returns:
//	TT_OK
//	TT_ERR_INVALID	Could not parse address
//	TT_ERR_NO_MATCH	Address version is too new (diagnostic emitted)
//
Tt_status _Tt_session::
parsed_address(_Tt_string &addr_string)
{
#define IPVADDRLEN	16
	char		session_host[IPVADDRLEN];
	char		strid[BUFSIZ];
	int		junk_version = 1;
	Tt_status	status;
	const char 	*addr_format_fmt = "%%ld %%d %%d %%d %%lu %%%ds %%d";
	char 		 addr_format[32];
	const char 	*fcs1_addr_format_fmt = "%%ld %%d %%d %%d %%lu %%%ds";
	char 		 fcs1_addr_format[32];
	
	//
	// Note: the fcs 1.0 version of tooltalk uses the first 6
	// fields in addr_format. Changing these first 6 fields
	// compromises binary compatibility with the 1.0 version.
	// Additional fields can be added at the end of addr_format as
	// long as if the parsing fails then the old format
	// (fcs1_addr_format) is tried.
	//
	_rpc_version = TT_RPC_VERSION;
	if (addr_string.len() == 0) {
		int ipaddr_len;

		// set address string
		if (_host.is_null()) {
			return(TT_ERR_INTERNAL);
		}
		if (_host->stringaddr().len() == 0) {
			return(TT_ERR_INTERNAL);
		}

		ipaddr_len = _host->stringaddr().len();
		if (ipaddr_len > IPVADDRLEN) ipaddr_len = IPVADDRLEN;

		sprintf(fcs1_addr_format, fcs1_addr_format_fmt, ipaddr_len);
		sprintf(addr_format,      addr_format_fmt,      ipaddr_len);

		// put version number for format first (this is so we
		// can change the format later)
		sprintf(strid, "%02d ", _TT_XATOM_VERSION);
		sprintf(strid+3, addr_format,
			(long)_pid,
			_rpc_program,
			junk_version,
			(int)_auth.auth_level(),
			(long)_server_uid,
			(char *)_host->stringaddr(),
			_rpc_version);
		
		addr_string = strid;
		if ((status = _auth.set_sessionid(
						_rpc_program,
						_auth.auth_level(),
						_host->stringaddr(),
						_rpc_version)) != TT_OK) {
		    _tt_syslog(0, LOG_ERR,"_auth.set_sessionid() != TT_OK");
		    return(status);
		}
	} else {
		// get (parse) address string
		char *str = (char *)addr_string;

		sprintf(fcs1_addr_format, fcs1_addr_format_fmt, IPVADDRLEN);
		sprintf(addr_format,      addr_format_fmt,      IPVADDRLEN);

		// check version number of format first

		// XXX: note that if _TT_XATOM_VERSION is ever changed
		// then backward compatibility is seriously
		// compromised. 

		if (atoi(str) != _TT_XATOM_VERSION) {
			_tt_syslog( 0, LOG_ERR,
				    catgets( _ttcatd, 1, 15,
					     "address version is %d, but "
					     "I only understand %d! (address: "
					     "<%s>)" ),
				    atoi(str), _TT_XATOM_VERSION,
				    addr_string.operator const char *() );
			return TT_ERR_NO_MATCH;
		}
		
		// XXX: Originally, the fcs1_addr_format put in a
		// field describing the rpc version of the server.
		// However, the fcs1 version of client_session_init
		// erroneously initialized the rpc connection with the
		// *server* rpc version rather than its own. Thus we
		// can't use the rpc version field in fcs1_addr_format
		// because then old fcs1.0 clients would mistakenly
		// initialize themselves as version 1+ clients. Thus
		// (sigh) we have to add a field at the end of
		// fcs1_addr_format that is the real rpc version of
		// the server. This is the reason that the first rpc
		// version parsed from addr_format is thrown away. 

		long long_pid;
		long long_server_uid;
		_Tt_auth_level auth_level;
		if (7 != sscanf(str+3, addr_format,
				&long_pid,
				&_rpc_program,
				&junk_version, /* always 1 ... */
				&auth_level,
				&long_server_uid,
				session_host,
				&_rpc_version)) {

			// new format scan failed. Try to parse the
			// string for the old format

			if (6 != sscanf(str+3, fcs1_addr_format,
					&long_pid,
					&_rpc_program,
					&_rpc_version,
					&auth_level,
					&long_server_uid,
					session_host)) {
			  	_pid = (pid_t) long_pid;
				_server_uid = (uid_t) long_server_uid;
				return(TT_ERR_INVALID);
			} else {
				_rpc_version = 1;
			}
		}
		_pid = (pid_t) long_pid;
		_server_uid = (uid_t) long_server_uid;
		_hostaddr = session_host;
		_auth.set_auth_level(auth_level);
		if ((status = _auth.set_sessionid(
				_rpc_program,
				auth_level,
				_hostaddr,
				_rpc_version)) != TT_OK) {
		    _tt_syslog(0, LOG_ERR,"_auth.set_sessionid() != TT_OK");
		    return(status);
		}
	}

	return(TT_OK);
}


// 
// Sets the id of a session. Note that addressing information (the rpc
// number and version) are not part of the id. This makes it possible in
// the future to provide server address rebinding.
// 
Tt_status _Tt_session::
set_id(char *id)
{
	char			host[64];
	char			dpname[125];
	int			svnum;
	char			*ssid;
	
	if (id != (char *)0) {
		_id = id;
		switch (_id[0]) {
		      case 'X':
			// construct a suitable DISPLAY name from the id
			// which should have the format "X <host> <server_num>"
			if (sscanf((char *)id, "X %s %d", host, &svnum) != 2) {
				return(TT_ERR_SESSION);
			}
			/* We _cannot_ set _displayname based solely on host and svnum, 
			 * because :0 is NOT the same as 127.0.0.1:0 as far as X11
			 * is concerned: by default, it will only accept connections
			 * to the former. (XOpenDisplay etc. will fail if you try the below!)
			sprintf(dpname, "%s:%d", host, svnum);
			_displayname = dpname;
			 */
			if (! _displayname.len()) {
				_displayname = _tt_global->xdisplayname;
			}

			_server_num = svnum;
			_env = _TT_ENV_X11;
			break;
		      case 'P':
			// A process-tree session id is just an address string
			// preceded by "P ".
			_address_string = _id.mid(2, _id.len());
			_env = _TT_ENV_PROCESS_TREE;
			break;
		      default:
			return(TT_ERR_SESSION);
		}
		
	} else {
		// set the "id" which is the publicly visible *logical*
		// address of this session.
		ssid = _tt_get_first_set_env_var(2, TT_CDE_START_SID, TT_START_SID);
		if (ssid != (char *)0) {
			_id = ssid;
		} else {
			switch (env()) {
			      case _TT_ENV_X11:
				_type = "X";
				if (! _displayname.len()) {
					_displayname = _tt_global->xdisplayname;
				}
				_id = Xid(_displayname);
				break;
			      case _TT_ENV_PROCESS_TREE:
				_type = "P";
				_server_num = _pid;
				// sami said originally:
				// we need to include the address string in
				// the id of a process-tree session because
				// there is no alternate means of finding
				// the address (ie. for X11 the id could be
				// rid of address info because the X server
				// served as the alternate way of finding
				// the address).
				// rfm sez, 16 June 94:
				// Of course that alternate way was a bad
				// idea since it means other users can't
				// send messages using the session because
				// they can't contact the X server.
				_id = process_tree_id();
				break;
			      case _TT_ENV_LAST:
			      default:
				return(TT_ERR_INTERNAL);
			}
		}
	}
	
	return(TT_OK);
}


// 
// Returns a process-tree id for this session. This is essentially just
// the address of the session preceded by a "P". The purpose of this is
// that whenever this exact session needs to be advertised to a
// persistent medium (as is done with file-scope messages) this version
// of the session id is used so that if the session goes down and comes
// up again, the session id stored on disk will become stale.
// 
_Tt_string _Tt_session::
process_tree_id()
{
	_Tt_string	result = "P ";

	return(result.cat(_address_string));
}


bool_t _Tt_session::
xdr(XDR *xdrs)
{
	return(_id.xdr(xdrs) && _address_string.xdr(xdrs));
}


_Tt_string _Tt_session::
address_string()
{
	return(_address_string);
}

_Tt_string _Tt_session::
displayname()
{
	return(_displayname);
}


_Tt_string
_tt_session_address(_Tt_object_ptr &o)
{
	return ((_Tt_session *)o.c_pointer())->address_string();
}


// 
// Returns 1 if the given id is the same as the id of this session or if
// it is a process-tree id then it has the same address as this session.
// 
int _Tt_session::
has_id(const _Tt_string &id)
{
	if (id == _id) {
		return(1);
	}

	if (id.len() == 0) {
		return(0);
	}

	if (id == _address_string) {
		return(1);
	}

	if (id[0] == 'P') {
		_Tt_string	addr = id.mid(2,id.len());
		return(addr == _address_string);
	}

	return(0);
}


// ...for strings list
int _Tt_session::
has_id(const _Tt_string_list_ptr slist_p)
{
	_Tt_string_list_cursor c(slist_p);

	while (c.next()) {
		if (has_id(*c)) {
			return(1);
		}
	}
	return(0);
}
