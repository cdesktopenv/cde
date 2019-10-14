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
//%%  $XConsortium: api_filemap.C /main/3 1995/10/23 09:52:27 rswiston $ 			 				

/*
 *
 * @(#)api_filemap.C	1.21 95/02/21
 *
 * Copyright (c) 1990, 1993 by Sun Microsystems, Inc.
 *
 * THis file implements the api filename mapping calls to map to/from
 * canonical name formats of the form hostname:pathname to an
 * absolute pathname of the form /pathname.
 */

#include <unistd.h>
#include <sys/param.h>
#include "api/c/api_filemap.h"
#include "db/tt_db_file.h"
#include "mp/mp_c.h"
#include "util/tt_path.h"
#include "api/c/tt_c.h"
#include "api/c/api_api.h"
#include "api/c/api_mp.h"
#include "api/c/api_error.h"
#include "util/tt_audit.h"
#include "util/tt_port.h"
#include "util/tt_host_equiv.h"

// "magic" (in the /etc/magic sense) prefix for netfile strings
#define TT_NETFILE_PREFIX "TTN0"

// Include COMPATIBILITY code for earlier CDE snapshots

#define CDE_SNAPSHOT_COMPATIBILITY 1

char * _tt_host_file_netfile(const char * host, const char * filename);
char * _tt_host_netfile_file(const char * host, const char * netfilename);
char * _tt_file_netfile(const char * filename);
char * _tt_netfile_file(const char * netfilename);
static int get_keyword_value(_Tt_string s, const char * keyword,
			     int &start, int &end);

/******************************************************************/
/* filename mapping API calls implementation.			  */
/******************************************************************/

// On the specified host, assemble a netfilename for the specified filename.
char *
tt_host_file_netfile(const char * host, const char * filename)
{
        _Tt_audit audit;
        Tt_status status = audit.entry("CC", TT_HOST_FILE_NETFILE, host, filename);
        char *result;

        if (status != TT_OK) {
		audit.exit((char *)_tt_error_pointer(status));
                return (char *)_tt_error_pointer(status);
        }

        result = _tt_host_file_netfile(host, filename);
        audit.exit(result);

        return result;
}


// On the specified host, create a filename for the specified netfilename.
char *
tt_host_netfile_file(const char * host, const char * netfilename)
{
        _Tt_audit audit;
        Tt_status status = audit.entry("CC", TT_HOST_NETFILE_FILE, host, netfilename);
        char *result;

        if (status != TT_OK) {
		audit.exit((char *)_tt_error_pointer(status));
                return (char *)_tt_error_pointer(status);
        }

        result = _tt_host_netfile_file(host, netfilename);
        audit.exit(result);

        return result;
}


// On the local host, create a netfilename for the specified filename.
char *
tt_file_netfile(const char *filename)
{
        _Tt_audit audit;
        Tt_status status = audit.entry("C", TT_FILE_NETFILE, filename);
        char *result;

        if (status != TT_OK) {
		audit.exit((char *)_tt_error_pointer(status));
                return (char *)_tt_error_pointer(status);
        }

        result = _tt_file_netfile(filename);
        audit.exit(result);

        return result;
}


// On the local host, create a filename for the specified netfilename.
char *
tt_netfile_file(const char *netfilename)
{
        _Tt_audit audit;
        Tt_status status = audit.entry("C", TT_NETFILE_FILE, netfilename);
        char *result;

        if (status != TT_OK) {
		audit.exit((char *)_tt_error_pointer(status));
                return (char *)_tt_error_pointer(status);
        }

        result = _tt_netfile_file(netfilename);
        audit.exit(result);

        return result;
}


// Do the following:
//
//	1) Make RPC call to remote host, and on that
//	   host run tt_file_netfile(filename).
//
//	2) Return the result here.
char *
_tt_host_file_netfile(const char * host, const char * filename)
{
	Tt_status       status;
	_Tt_string	hostname(host);
	_Tt_string	local_host = _tt_gethostname();

        _tt_internal_init();

	// If this is an RPC call to our localhost, do
	// it directly and don't bother with RPC.

	_Tt_host_equiv_ptr eq_p = new _Tt_host_equiv;

	if (eq_p->hostname_equiv(hostname, local_host) == 1) {
		// strdup already done in _tt_netfile_file()
		return _tt_file_netfile(filename);
	}

	_Tt_string	path(filename);
	_Tt_string	netfile;

	// Connect to dbserver on remote host.
	_Tt_db_results	db_status;
	_Tt_db_client_ptr h_dbserv = new _Tt_db_client(hostname, db_status);

	// run _tt_file_netfile() on the remote host.
	if ((status = _tt_get_api_error(h_dbserv->getConnectionResults(),
					  _TT_API_FILE_MAP)) == TT_OK) {

		status = _tt_get_api_error(h_dbserv->file_netfile(path, netfile),
					   _TT_API_FILE_MAP);
	}

	if (status != TT_OK) {
		return (char *)_tt_error_pointer(status);
	}

#if CDE_SNAPSHOT_COMPATIBILITY
	// For compatibility with development snapshots of CDE,
	// if the returned netfile does not start with TT_NETFILE_PREFIX,
	// put it on.

	if (netfile.left(strlen(TT_NETFILE_PREFIX)) != TT_NETFILE_PREFIX) {
		path = netfile;
		netfile = TT_NETFILE_PREFIX;
		netfile = netfile.cat(path);
	}
#endif /* CDE_SNAPSHOT_COMPATIBILITY */	

	return _tt_strdup((char *) netfile);
}


// Do the following:
//
//	1) Make RPC call to remote host, and on that
//	   host run tt_netfile_file(netfilename).
//
//	2) Return the result here.
char *
_tt_host_netfile_file(const char * host, const char * netfilename)
{
	Tt_status       status;
	_Tt_string	hostname(host);
	_Tt_string	local_host = _tt_gethostname();

        _tt_internal_init();

        // If this is an RPC call to our localhost, do
        // it directly and don't bother with RPC. 
         
	_Tt_host_equiv_ptr	eq_p = new _Tt_host_equiv;

	if (eq_p->hostname_equiv(hostname, local_host) == 1) {
		// strdup already done in _tt_netfile_file()
                return _tt_netfile_file(netfilename);
        }

	_Tt_string	path(netfilename);
	_Tt_string	file;


	// Connect to dbserver on remote host.
	_Tt_db_results	db_status;
	_Tt_db_client_ptr h_dbserv = new _Tt_db_client(hostname, db_status);

	// run _tt_netfile_file() on the remote host.
	if ((status = _tt_get_api_error(h_dbserv->getConnectionResults(),
				        _TT_API_FILE_MAP)) != TT_OK) {
		status = status == TT_ERR_PATH ? TT_ERR_NETFILE : status;
		return (char *)_tt_error_pointer(status);
	}
	
	if ((status = _tt_get_api_error(h_dbserv->netfile_file(path, file),
					_TT_API_FILE_MAP)) == TT_OK) {
		return _tt_strdup((char *) file);
	}

#if CDE_SNAPSHOT_COMPATIBILITY
	// Mapping failed.  It's just possible that it's a dbserver from
	// an earlier development snapshot which doesn't understand
	// netfiles with TT_NETFILE_PREFIX in front of them.


	if (path.left(strlen(TT_NETFILE_PREFIX)) == TT_NETFILE_PREFIX) {
		path = path.right(path.len()-strlen(TT_NETFILE_PREFIX));
		
		if ((status = _tt_get_api_error(h_dbserv->netfile_file(path, file),
					_TT_API_FILE_MAP)) == TT_OK) {
			return _tt_strdup((char *) file);
		}
	}

#endif /* CDE_SNAPSHOT_COMPATIBILITY */

	status = status == TT_ERR_PATH ? TT_ERR_NETFILE : status;
	return (char *)_tt_error_pointer(status);	

}


// given a filename in a valid file path format,
// i.e. /a/b/..., return a string in the canonical
// format of hostname:/x/y/....
//
char *
_tt_file_netfile(const char *filename)
{
	_Tt_string path(filename);

        _tt_internal_init();

	// Create the canonical filename.
	_Tt_api_filename_map_ptr mapp = new _Tt_api_filename_map;
	return _tt_strdup(mapp->set_filename(path));
}


// given a filename in the canonical format, i.e.
// hostname:/x/y/.... return a string in the local
// file path format, i.e. /a/b/...
//
char *
_tt_netfile_file(const char *netfilename)
{
	_Tt_string		 network_path(netfilename), result;
	_Tt_api_filename_map_ptr original_mapp = new _Tt_api_filename_map;
	_Tt_api_filename_map_ptr new_mapp = new _Tt_api_filename_map;

	_tt_internal_init();

        // We want to turn the rpath into a local path.  We'd prefer
        // to use the same local path the creator of the netfile used,
        // if that works and means the same file.  We do this by a two
        // step process:
	//
        //      1) generate a new rpath based on the original lpath.
        //         If the new rpath == the original rpath, use the
	//         original lpath.
        //
        //      2) If the new rpath isn't the same, then turn
        //         the original rpath into a lpath by trudging through
        //         the mount tables.

	// parse the canonical filename into its' component parts.
	result = original_mapp->parse_netfilename(network_path);
	if (result.len() == 0) {
		return (char *)_tt_error_pointer(TT_ERR_NETFILE);
	}

        // Do step 1 above: generate new rpath from old lpath.
	new_mapp->set_filename(original_mapp->lpath_val());

	_Tt_string host = new_mapp->hostname_val();
	_Tt_string rpath = new_mapp->rpath_val();
	_Tt_string lpath = new_mapp->lpath_val();

	// compare the new rpath with the old rpath.
	if (host == original_mapp->hostname_val()) {
		if (rpath == original_mapp->rpath_val()) {
			return _tt_strdup(original_mapp->lpath_val());
		}
	}

	// Do step 2 above: convert the original rpath to a local path.
	result = _tt_network_path_to_local_path(original_mapp->hostname_val().cat(':').cat(original_mapp->rpath_val()));

	return _tt_strdup(result);
}


/******************************************************************/
/* _Tt_api_filename_map class implementation below.		  */
/******************************************************************/

// A canonical pathname is of the following form:
// "HOST=0-x,RPATH=x-y,LPATH=y-z:HostnameLpathRpath", where
// x, y, and z are ASCII representations of the number
// which is the position within the character array where
// the concatenated strings begin and end.

// "pathname" should be an absolute or relative pathname.
_Tt_string _Tt_api_filename_map::
set_filename(const _Tt_string & filename)
{
	_Tt_string	absolute_path = filename;
	_Tt_string	tmp_string(MAXPATHLEN * 3);
	int		i;

	if (filename[0] != '/') {
	// A relative path, make it absolute.
		char wd[MAXPATHLEN+1];

		if (getcwd(wd, sizeof(wd))) {
			absolute_path = _Tt_string(wd).cat("/").cat(filename);
		}
	}

	// Now get the network path of the file.
	tmp_string = _tt_local_network_path(absolute_path);

	_lpath = absolute_path;	// what we know the file as.

	// load hostname and rpath simultaneously
	_rpath = tmp_string.split(':', _hostname);

	// construct a canonical pathname
	_canonical_path = TT_NETFILE_PREFIX;
	_canonical_path = _canonical_path.cat("HOST=0-").cat(_hostname.len() - 1).cat(',');
	i = _hostname.len();

	_canonical_path =
	_canonical_path.cat("RPATH=").cat(i).cat('-').cat(i + _rpath.len() - 1).cat(',');
	i += _rpath.len();

	_canonical_path =
	_canonical_path.cat("LPATH=").cat(i).cat('-').cat(i + _lpath.len() - 1).cat(':');

	_canonical_path = _canonical_path.cat(_hostname).cat(_rpath).cat(_lpath);

	return _canonical_path;
}


// "canonical_name" is of the format
// "TTN0HOST=0-x,RPATH=x-y,LPATH=y-z:HostnameLpathRpath" as described above
// If not, parse_netfilename returns (_Tt_string)0.
_Tt_string _Tt_api_filename_map::
parse_netfilename(const _Tt_string & canonical_name)
{
	_Tt_string	tmp_string, dummy;
	int		i, h_begin, h_end, l_begin, l_end, r_begin, r_end;

	// parse the canonical_path.

	// It's important to do this in a way that is independent of the
	// order of the keywords, and which ignores any extraneous keywords
	// which may be introduced later.

	// A canonical name should start with TT_NETFILE_PREFIX ("TTN0"),
	// which we ignore.  However if CDE_SNAPSHOT_COMPATIBILITY is 1, and
	// it starts with HOST= it's from
	// a slightly older version of the code, and we accept that too.

	i = strlen(TT_NETFILE_PREFIX);
	if (canonical_name.left(i) == TT_NETFILE_PREFIX) {
		tmp_string = canonical_name.mid(i, canonical_name.len()-i);
#if CDE_SNAPSHOT_COMPATIBILITY
	} else if (canonical_name.left(5) == "HOST=") {
		tmp_string = canonical_name;
#endif /* CDE_SNAPSHOT_COMPATIBILITY */		
	} else {
		return (_Tt_string) 0;
	}

	// All the keyword stuff is before the colon.
	
	tmp_string.split(':',tmp_string);

	if (!(get_keyword_value(tmp_string, "HOST", h_begin, h_end) &&
	      get_keyword_value(tmp_string, "RPATH", r_begin, r_end) &&
	      get_keyword_value(tmp_string, "LPATH", l_begin, l_end)))
	{
		return (_Tt_string)0;
	}


	// get the raw data in the canonical_path, and parse out
	// the components

	_canonical_path = canonical_name;

	tmp_string = _canonical_path.split(':', dummy);

	_hostname = tmp_string.mid(h_begin, h_end - h_begin + 1);

	_rpath = tmp_string.mid(r_begin, r_end - r_begin + 1);

	_lpath = tmp_string.mid(l_begin, l_end - l_begin + 1);

	return _canonical_path;
}


_Tt_string _Tt_api_filename_map::
canonical_path_val()
{
	return _canonical_path;
}

_Tt_string _Tt_api_filename_map::
lpath_val()
{
	return _lpath;
}

_Tt_string _Tt_api_filename_map::
rpath_val()
{
	return _rpath;
}

_Tt_string _Tt_api_filename_map::
hostname_val()
{
	return _hostname;
}


// Find a keyword in the netfile prefix and return the associated start and
// end byte offsets.  If the keyword is not found, or the string is badly
// formed, return 0. Else, if the parsing succeeds, return 1.
// The TT_NETFILE_PREFIX  and everything from the colon on are assumed
// to already be lopped off s.

static int
get_keyword_value(_Tt_string s, const char *keyword, int &start, int &end)
{
	// It makes the parsing more uniform if we can always expect to
	// find the keyword preceded by a comma, which is true for all
	// but the first keyword.  So put a comma on the front, and it
	// will be true for all keywords.  Similarly, put a comma
	// in front of the keyword and an equals sign after it.
	// Similarly, add a comma at the end of s so even the last
	// keyword is terminated by one.
	
	_Tt_string ts = ",";
	_Tt_string tk = ",";
	_Tt_string junk;
	int i;

	ts = ts.cat(s).cat(",");
	tk = tk.cat(keyword).cat("=");
	
	if (-1==(i=ts.index(tk))) {
		return 0;
	}
	// lop off extraneous stuff before the keyword
	ts = ts.mid(i,ts.len()-i);
	// lop off the keyword
	ts = ts.split('=',junk);
	// lop off extraneous stuff after the comma that terminates
	// this keyword
	ts.split(',',junk);
	i = sscanf((char *)ts, "%d-%d", &start, &end);
	return (i==2);
}
