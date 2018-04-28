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
/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $TOG: mp_auth.h /main/2 1999/09/10 15:01:41 mgreess $ 			 				 */
/* 
 * @(#)mp_auth.h	1.36 95/01/25
 * 
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 * 
 * This file contains the implementation of the _Tt_auth object which
 * implements the MIT-MAGIC-COOKIE authentication for the _TT_AUTH_ICEAUTH
 * authentication type.  This class is simply a wrapper for calls to
 * code copied out of the X11 libICE library.  Alternatively, one can
 * compile with the OPT_ICE define and call the ICE routines directly.
 */
#ifndef MP_AUTH_H
#define MP_AUTH_H
#include "tt_options.h"
#include "mp/mp_auth_functions.h"
#include "util/tt_object.h"
#include "util/tt_list.h"
#include "util/tt_string.h"

/*
 * As of the CDE release of ToolTalk, _TT_AUTH_XAUTH is no longer supported.
 * It has been removed along with all references in the TT code, but the
 * value of _TT_AUTH_DES was not changed to the now-missing value of 1.
 */
/*
 * As of 2.1.30, the libICE MIT-MAGIC-COOKIE  authentication is added in
 * place of _TT_AUTH_XAUTH.  The code was inlined from libICE in such a
 * way that it could be easily replaced by libICE itself.
 */

#define _TT_ICEAUTH_AUTH_NAME		"MIT-MAGIC-COOKIE-1"
#define _TT_ICEAUTH_MAGIC_COOKIE_LEN	16
#define _TT_ICEAUTH_PROTOCOL_NAME	"TT"

#define _TT_ICEAUTH_DEFAULT_RETRIES	10   /* num of competitors we expect */
#define _TT_ICEAUTH_DEFAULT_TIMEOUT	2    /* in seconds, be quick */
#define _TT_ICEAUTH_DEFAULT_DEADTIME	600L /* 10 minutes in seconds */



enum _Tt_auth_level {
	_TT_AUTH_UNIX	= 0,	/* Unix "authentication" */
	_TT_AUTH_ICEAUTH= 1,	/* ICE MIT-COOKIE authentication (default) */
	_TT_AUTH_DES	= 2,	/* Secure RPC (DES encryption)	   */
	_TT_AUTH_NONE	= 3	/* No authentication */
};

class _Tt_auth : public _Tt_object {
      public:
	_Tt_auth(_Tt_auth_level auth_level = _TT_AUTH_ICEAUTH);
	virtual ~_Tt_auth();

	_Tt_auth_level		auth_level() {
	    return _auth_level;
	}
	const _Tt_string	&auth_cookie() const {
	    return _auth_cookie;
	}
	Tt_status		generate_auth_cookie();
	Tt_status		retrieve_auth_cookie();
	Tt_status		set_auth_level(_Tt_auth_level auth_level);
	Tt_status		set_sessionid(
					int		rpc_program,
					_Tt_auth_level	auth_level,
					_Tt_string	hostaddr,
					int		rpc_version);

      protected:
	//
	// state variables
	//
	_Tt_auth_level			_auth_level;
	_Tt_string			_auth_cookie;
	_Tt_string			_hostaddr;
	int				_rpc_program;
	int				_rpc_version;
	_Tt_string			_sessionid;
	_Tt_string			_ttauthfile;

      private:
	//
	// variables associated with generating a .ttauthority file entry.
	//
	_tt_AuthFileEntryList	*_entries_head;
	_tt_AuthFileEntryList	*_entries_tail;

	Tt_status		read_auth_file(char*);
	Tt_status		write_auth_file(char*);

	Tt_status		modify_auth_entry(_tt_AuthFileEntry*,
						  _tt_AuthFileEntryList**);
	Tt_status		read_auth_entries(FILE*,
						  _tt_AuthFileEntryList**);
};

#endif				/*  MP_AUTH_H */
