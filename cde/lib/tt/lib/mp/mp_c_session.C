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
//%%  $TOG: mp_c_session.C /main/5 1999/10/14 18:41:24 mgreess $ 			 				
/*
 *  @(#)mp_c_session.C	1.23	94/10/03
 *
 *  Copyright (c) 1990,1992,1993 by Sun Microsystems, Inc.
 *
 *  Implementation of the _Tt_session class.
 */
#include "util/tt_global_env.h"
#include "util/tt_host.h"
#include "util/tt_enumname.h"
#include "mp/mp_global.h"
#include "mp/mp_c_message.h"
#include "mp/mp_mp.h"
#include "mp/mp_pattern.h"
#include "mp/mp_rpc_client.h"
#include "mp/mp_rpc_interface.h"
#include "mp/mp_c_session.h"
#include "mp/mp_desktop.h"
#include "mp/mp_xdr_functions.h"
#include "util/tt_port.h"
#include <unistd.h>
#include <signal.h>

#ifdef OPT_UNIX_SOCKET_RPC
#	include <sys/socket.h>
#	include <sys/un.h>
#endif // OPT_UNIX_SOCKET_RPC
     
// Use the parent class (_Tt_session) for construction and destruction.
_Tt_c_session::_Tt_c_session () {}
_Tt_c_session::~_Tt_c_session () {}



// 
// Initializes a session object. This means that we initialize our
// desktop connection if this is a desktop session and find the address
// of a server session. If none is found or if an address is found but we
// are unable to connect to the session then we attempt to auto-start a
// server session. 
//
// Returns:
//	TT_OK
//	TT_ERR_ACCESS	Could not init desktop (diagnostic emitted)
//	TT_ERR_NO_MATCH	Advertised address's version is too new
//			(diagnostic emitted)
//	TT_ERR_INVALID	Authorization error
//	TT_ERR_NOMP	No server running or startable
// 
Tt_status _Tt_c_session::
c_init()
{
	_Tt_string	start_ttcmd;
	int		tried = 0;
	int		done = 0;
	Tt_status	status;

	if (env() == _TT_ENV_X11) {
		if (_displayname.len() == 0) {
			_displayname = _tt_global->xdisplayname;
		}
	}
	while (tried < 10) {
		_is_dead = 0;
		status = client_session_init();
		switch (status) {
			int pings;
		    case TT_OK:
			// We try to ping the session to give it
			// reasonable time to start up.
			for (pings = 1; pings <= OPT_PING_TRIES; pings++) {
				status = ping();
				if (status != TT_ERR_NOMP) {
					// Fatal error or TT_OK
					return status;
				}
				sleep(OPT_PING_SLEEP);
			}

			// Session could not be pinged.  If this is an
			// X11 session, handle the case where there is
			// an invalid TT_CDE_XATOM_NAME xprop and a valid
			// TT_XATOM_NAME xprop, because in that case we
			// were trying to ping the dead TT_CDE_XATOM_NAME
			// session.

			if (env() == _TT_ENV_X11) {
				_Tt_string desktop_addr;

				if (_desktop->get_prop(TT_CDE_XATOM_NAME,
						       desktop_addr) &&
				    desktop_addr == _address_string) {

					// Bogus session xprop found -- re-initialize
					// trying the TT_XATOM_NAME xprop.

					if (_desktop->get_prop(TT_XATOM_NAME,
							       _address_string)) {
						// Continue the initialization
						// loop WITHOUT attempting to start
						// a new ttsession
						tried++;
						continue;
					}
				}
			}
			else {
				// If we\'re here, a session was found but could
				// not be pinged.  Force re-initialization.
				_address_string = (char *) 0;
				break;
			}
		    case TT_ERR_ACCESS:
		    case TT_ERR_NO_MATCH:
			// Can't recover from these
			return status;
		    case TT_WRN_NOTFOUND:
		    case TT_ERR_INVALID:
		    case TT_ERR_NOMP:
			// No ttsession.
			// If we haven\'t tried to start one already, do
			// so. If we have tried but failed, force
			// client_session_init to re-initialize.
			if (tried > 0) _address_string = (char *) 0;
			break;
		    case TT_AUTHFILE_ACCESS:
		    case TT_AUTHFILE_LOCK:
		    case TT_AUTHFILE_LOCK_TIMEOUT:
		    case TT_AUTHFILE_UNLOCK:
		    case TT_AUTHFILE_MISSING:
		    case TT_AUTHFILE_ENTRY_MISSING:
		    case TT_AUTHFILE_WRITE:
			return TT_ERR_AUTHORIZATION;
		    default:
			_tt_syslog( 0, LOG_ERR,
				    "_Tt_session::client_session_init(): %s",
				    _tt_enumname(status));
			return TT_ERR_INTERNAL;
		}
		//
		// Attempt to start a new server
		//
		if (env() != _TT_ENV_X11) {
			// can't auto-start anything other than an X
			// tooltalk session
			return TT_ERR_NOMP;
		}
		if (tried == 0) {
			int sysStat = startup_ttsession(start_ttcmd);
			int exitStat;
			if (WIFEXITED(sysStat)) {
				exitStat = WEXITSTATUS(sysStat);
			} else if (WIFSIGNALED(sysStat)) {
				_tt_syslog( 0, LOG_ERR,
					    "libtt: system(\"%s\"): signal %d",
					    (char *)start_ttcmd,
					    WTERMSIG(sysStat) );
				exitStat = 11;
			} else {
				_tt_syslog( 0, LOG_ERR,
					    "libtt: system(\"%s\"): %d",
					    (char *)start_ttcmd, sysStat );
				exitStat = 11;
			}
			switch (exitStat) {
			    case 1:
				// Race condition -- no ttsession was
				// found in the above code, but by the time
				// we got around to trying to start one,
				// another had appeared, so this one exited
				// normally.
				// Fall through.
			    case 2:
				// another ttsession discovered.
				// Either way, loop around and connect to it
				// after nulling _address_string to make sure
				// we pick the address off the X server again
				// since the new server will have a new
				// address.
				_address_string = (char *)0;

				// Since the other ttsession was just started,
				// let it have a chance to initialize
				sleep(2);

				// fall through
			    case 0:
				// ttsession started.
				break;
			    default:
				return TT_ERR_NOMP;
			}
		} else {
			sleep(1);
		}
		tried++;
	}

	return TT_ERR_NOMP;
}

// Starts ttsession by executing either the hardcoded
// default command "ttsession -s -d <displayname>"
// or the command specified in TTSESSION_CMD or SUN_TTSESSION_CMD.
// TTSESSION_CMD and SUN_TTSESSION_CMD provide a way of overriding the
// standard options specified when tools auto-start
// ttsession. For example, to switch from Classing
// Engine format to xdr format for auto-start set
// TTSESSION_CMD or SUN_TTSESSION_CMD to "ttsession -X".
// The string used is returned in a parameter so c_init can use it in
// error messages.


int _Tt_c_session::
startup_ttsession(_Tt_string &start_ttcmd)
{
	// If TTSESSION_CMD or SUN_TTSESSION_CMD is specified, we use system() to run the
	// command since it might have shell metacharacters (a common
	// thing to do is TTSESSION_CMD or SUN_TTSESSION_CMD=truss ttsession >/tmp/trace
	// in order to truss ttsession startup)

    char* pc_env_value = _tt_get_first_set_env_var(2, "TTSESSION_CMD", "SUN_TTSESSION_CMD");

	if (pc_env_value) {
		start_ttcmd = pc_env_value;
		start_ttcmd = start_ttcmd.cat(" -d ");
		start_ttcmd = start_ttcmd.cat(_displayname);
		return system((char *)start_ttcmd);
	}
	// start_ttcmd is not really used but we set it so it can be used
	// in messages
	start_ttcmd = "ttsession -s -d ";
	start_ttcmd = start_ttcmd.cat(_displayname);
	
	// This is basically a stripped down and specialized system() call...

	int	status;
	pid_t	pid, w;

#if defined(__GNUG__)
	typedef void (*SIG_PF)(int);
#endif
	SIG_PF istat, qstat, cstat;

#if defined(_AIX) || defined(OPT_BUG_USL) || defined(OPT_BUG_UXP)
#define vfork fork
#endif
	if((pid = vfork()) == 0) {
		fflush(stdout);
		(void) execlp("ttsession", "ttsession",
			      "-s",
			      "-d",
			      (char *)_displayname,
			      (char *)0);
		_exit(127);
	}

	istat = signal(SIGINT, SIG_IGN);
	qstat = signal(SIGQUIT, SIG_IGN);
	cstat = signal(SIGCHLD, SIG_DFL);

	w = waitpid(pid, &status, 0);

	(void) signal(SIGINT, istat);
	(void) signal(SIGQUIT, qstat);
	(void) signal(SIGCHLD, cstat);

	return((w == -1)? w: status);
	
}

//
// Invokes an rpc call on the server session that causes all of the
// given procid's patterns to have a session id for the current
// session (this allows them to begin matching messages).
//
// See _Tt_s_session::s_join for the server-side of this method.
//
Tt_status _Tt_c_session::
c_join(_Tt_procid_ptr &procid)
{
	Tt_status		status;
	Tt_status		rstatus;

	rstatus = call(TT_RPC_JOIN_SESSION,
		       (xdrproc_t)tt_xdr_procid,
		       (char *)&procid,
		       (xdrproc_t)xdr_int,
		       (char *)&status);
	return((rstatus == TT_OK) ? status : rstatus);
}


//
// Invokes an rpc call on the server session that causes all of the
// given procid's patterns to not have a session id for the current
// session (this prevents them from matching messages).
//
// See _Tt_s_session::s_quit for the server-side of this method.
//
Tt_status _Tt_c_session::
c_quit(_Tt_procid_ptr &procid)
{
	Tt_status		status;
	Tt_status		rstatus;
		
	rstatus = call(TT_RPC_QUIT_SESSION,
		       (xdrproc_t)tt_xdr_procid, (char *)&procid,
		       (xdrproc_t)xdr_int, (char *)&status);
	return((rstatus == TT_OK) ? status : rstatus);
}
