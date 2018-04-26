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
/*%%  $XConsortium: tt_global_env.h /main/5 1995/12/05 09:25:41 rswiston $ 			 				 */
/*
 *
 * tt_global_env.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */
#ifndef TT_GLOBAL_ENV_H
#define TT_GLOBAL_ENV_H
#include "tt_options.h"
#include "util/tt_object.h"
#include "util/tt_host_utils.h"
#include "util/tt_file_system_entry_utils.h"
#include "mp/mp_rpc_interface.h"
#include "db/tt_db_hostname_redirection_map.h"

#ifdef OPT_XTHREADS
#include <X11/Xthreads.h>
#endif

//
// Global object to hang information needed by various server and
// client objects such as a cache of known hosts, and the current xdr
// version (see comment in tt_xdr_version.h).
//
class _Tt_global : public _Tt_object {
      public:
	_Tt_global();
	~_Tt_global();
	// given a host ip address, return a pointer to a _Tt_host
	// object which contains more information on the host.
	int				find_host(_Tt_string haddr,
						  _Tt_host_ptr &h,
						  int create_ifnot);

	// same idea as find_host except a name is given instead
	int				find_host_byname(_Tt_string name,
							 _Tt_host_ptr &h);

	// return in h a pointer to a _Tt_host object representing the
	// local host. Returns 1 for success and 0 for failure.
	int				get_local_host(_Tt_host_ptr &h);

	// the name of the X11 session
	_Tt_string			xdisplayname;

	// the name of the program name
	_Tt_string			progname;

	// returns maximum number of fds
	int				maxfds();

	// flag saying whether messages should be printed out.
	int				silent;

	// returns current xdr/rpc version
	int				xdr_version();
	
	// sets the current xdr version, defaults to the
	// TT_XDR_VERSION constant.
	void		set_xdr_version(int v = TT_XDR_VERSION);

	// Utilities for mt-safe-ness
        void				grab_mutex();
	void				drop_mutex();

	// Test-and-set routine -- returns 1 if threading has not yet
	// been turned on and this is the first TT API call.
	// Note the assignment (*NOT* comparison test!) with _multithreaded
	int				set_multithreaded() {
		return _multithreaded = 1 && event_counter == 1;
	};

	int				multithreaded() { return _multithreaded; };

	// the uid of the process
	uid_t				uid;

	// the gid of the process
	gid_t				gid;

	// DB hostname redirection map
	_Tt_db_hostname_redirection_map	db_hr_map;

	// Internal cache of mnttab entries.
	_Tt_file_system_entry_list_ptr	fileSystemEntries;

	// Universal _Tt_string buffer so that _Tt_strings don't
	// do ridiculous initial allocations until the space is
	// actually needed
	_Tt_string_buf_ptr		universal_null_string;

	// how many "events", typically API calls in libtt and or
	// RPC calls in ttsession/dbserver, have been called in
	// the life of this process.
	int				event_counter;

	// next time that garbage collection should be done.
	time_t				next_garbage_run;

#ifdef OPT_XTHREADS
        xmutex_rec			global_mutex;
        xthread_key_t			threadkey;
#endif

      private:
	static int			_maxfds;
        int				_multithreaded;
	int				_xdr_version;
	_Tt_host_table_ptr		_host_cache;
	_Tt_host_table_ptr		_host_byname_cache;
	_Tt_host_ptr			_local_host;
#ifdef OPT_XTHREADS
        int				_lock_free;
        xcondition_rec			_proceed;
#endif
};
extern	_Tt_global	*_tt_global;
#endif				/* TT_GLOBAL_ENV_H */
