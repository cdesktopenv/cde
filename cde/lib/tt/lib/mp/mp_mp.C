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
//%%  $TOG: mp_mp.C /main/7 1999/09/21 08:43:02 mgreess $ 			 				
/*
 *
 * mp_mp.cc -- Server and client state object for the Message Passer
 *
 * Copyright (c) 1990,1992 by Sun Microsystems, Inc.
 */

#include "mp/mp_global.h"
#include "mp/mp_mp.h"
#include "mp/mp_file.h"
#include "mp/mp_session.h"
#include "mp/mp_c_session.h"
#include "mp/mp_procid.h"
#include "mp/mp_rpc_client.h"
#include "mp/mp_xdr_functions.h"
#include "util/tt_base64.h"
#include "util/tt_port.h"
#include "util/tt_path.h"
#include "api/c/api_error.h"
#include <errno.h>
#include <sys/resource.h>
#include <stdlib.h>
#include "tt_options.h"

#if defined(_AIX)
/* AIX's FD_ZERO macro uses bzero() without declaring it. */
#include <strings.h>
#endif

// 
// This file contains methods for the _Tt_mp object which is an object
// whose only function is to serve as a placeholder for global
// data/objects relevant to the message-passer routines. Another major
// use is to hold the various object caches that map object ids to a
// single object of the given class.
//
// Really, the _Tt_mp object as we see it here should go away, being
// replaced by _Tt_c_mp, which should represent the client's idea
// of the ttsession server it's connected to, and _Tt_s_mp, which
// represents the global state of a ttsession server.
// 

//
// Global pointer to *the* _Tt_mp object. There should only be one
// _Tt_mp object per tooltalk client.
//
_Tt_mp          *_tt_mp = (_Tt_mp *)0;

//
// Constructs a _Tt_mp object in the indicated mode.
//
_Tt_mp::
_Tt_mp()
{
	_flags = 0;
	FD_ZERO(&_session_fds);
	_session_cache = new _Tt_session_table(_tt_session_address);
	active_messages = 0;
	_current_message_id = _current_pattern_id = 0;
	active_procs = new _Tt_procid_table(_tt_procid_id, 50);
}

_Tt_mp::
~_Tt_mp()
{
}



// 
// Returns the value of a counter and then increments the counter. Used
// to make different messages sent by the same sender unique.
//
int _Tt_mp::
generate_message_id()
{
	return(_current_message_id++);
}


// 
// Returns the value of a counter and then increments the counter. Used
// to make different patterns registered by the same sender unique.
//
int _Tt_mp::
generate_pattern_id()
{
	return(_current_pattern_id++);
}




// 
// Finds the _Tt_file object associated with the network pathname <path>.
// Optionally creates the _Tt_file object.  Returns:
//	TT_OK		but: if (! createIfNot), then fp.is_null() is possible
//	TT_ERR_FILE
//	TT_ERR_PATH
//	TT_ERR_DBAVAIL
//	TT_ERR_DBEXIST
//	TT_ERR_INTERNAL	can mean ENOMEM
// 
Tt_status
_Tt_mp::find_file(
	_Tt_string	path,
	_Tt_file_ptr   &file,
	int		createIfNot
)
{
	_Tt_string network_path;
	if (_tt_is_network_path(path)) {
		network_path = path;
	}
	else {
		network_path = _Tt_db_file::getNetworkPath(path);
	}

	if (_file_cache.is_null()) {
		if (!createIfNot) {
			return TT_OK;
		}
		else {
			_file_cache = new _Tt_file_table(_Tt_file::networkPath_);
			if (_file_cache.is_null()) {
				return TT_ERR_NOMEM;
			}
		}
	}
	else {
		file = _file_cache->lookup( network_path );
		if (! file.is_null()) {
			return TT_OK;
		}
	}

	// If we get to this point, the file is not in the cache

	Tt_status status = TT_OK;
	if (createIfNot) {
		file = new _Tt_file( path );
		if (file.is_null()) {
			return TT_ERR_NOMEM;
		}
		_Tt_db_results dbStatus = file->getDBResults();
		status = _tt_get_api_error( dbStatus, _TT_API_FILE );
		if (status == TT_OK) {
			_file_cache->insert( file );
		} else if (status == TT_ERR_INTERNAL) {
			_tt_syslog( 0, LOG_ERR,
				    "_Tt_db_file::_Tt_db_file(): %d", dbStatus);
		}
	}
	return status;
}

void _Tt_mp::remove_file(_Tt_string path)
{
	_Tt_string network_path = _Tt_db_file::getNetworkPath(path);
	_file_cache->remove(network_path);
}

void _Tt_mp::
remove_session(_Tt_string id)
{
	_session_cache->remove(id);
}


// 
// Attempts to find the _Tt_session object associated with the given
// session id. If not found and create_ifnot is 1 then a new session is
// created and initialized. When it is initialized, the auto_start
// parameter is examined. If it is 1 then the new session will be
// initialized with the c_init method that will do an auto-start of the
// session if it isn't running. Otherwise, the "client_session_init"
// method is used which just tries to connect to a running session.
//
// Note that the server does use _Tt_c_session since servers can
// act as clients for one another when passing file scoped messages.
// 
Tt_status _Tt_mp::
find_session(_Tt_string id, _Tt_session_ptr &sp,
	     int create_ifnot, int auto_start)
{
	Tt_status result = TT_OK;

	_Tt_session_ptr		s;
	if (_session_cache->lookup(id,s)) {
		sp = s;
		result = TT_OK;
	} else {
		// If this is a session address, try the X session ID
		// XXX - this is a kludge to deal with X session
		// IDs  now  that we use addres_strings as the lookup
		// key.  This should be removed if/when we expunge
		// our internal use of X session IDs.
		_Tt_session_table_cursor sc(_session_cache);

		while (sc.next()) {
			if (sc->has_id(id)) {
				sp = *sc;

				// Note: we return right here!
				return TT_OK;
			}
		}
	}

	if (! create_ifnot || id.len() <= 0) {
		result = TT_ERR_SESSION;
	} else {
		_Tt_c_session_ptr s = new _Tt_c_session();

		result = s->set_id(id);
		if (result != TT_OK) return result;

		if (auto_start) result = s->c_init();
		if (result != TT_OK) return result;

		if (result == TT_OK) result = s->client_session_init();
		if (result != TT_OK) return result;

		_session_cache->insert(s);
		sp = s;
		result = TT_OK;
	}
	return result;
}


void _Tt_mp::
save_session_fd(int fd)
{
	FD_SET(fd, &_session_fds);
}


int _Tt_mp::
find_session_by_fd(int fd, _Tt_session_ptr &sp)
{
	_Tt_rpc_client_ptr		rpc_client;
	_Tt_session_table_cursor	sc(_session_cache);

	while (sc.next()) {
		rpc_client = sc->_rpc_client;
		if (rpc_client->socket() == fd) {
			sp = *sc;
			return(1);
		}
	}
	return(0);
}


void _Tt_mp::
check_if_sessions_alive()
{
	fd_set			s_fds;
	int			fd, maxfds, n;
	timeval			tmout;
	_Tt_session_ptr		s;
	_Tt_string		id;

	tmout.tv_sec = 0;
	tmout.tv_usec = 0;
	s_fds = _session_fds;
	maxfds = _tt_getdtablesize();
	n = select(maxfds, (fd_set *) 0, &s_fds, (fd_set *) 0, &tmout);

	if (n < 0) {
		return;
	}
	fd = 0;
	while (n > 0) {
		if (FD_ISSET(fd, &s_fds)) {
			if (! find_session_by_fd(fd, s)) {
				FD_CLR(fd, &_session_fds);
			} else if (s->ping() != TT_OK) {
				id = s->process_tree_id();
				_tt_mp->remove_session(id);
				FD_CLR(fd, &_session_fds);
			}
			n--;
		}
		fd++;
	}
}
