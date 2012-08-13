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
//%%  $TOG: mp_typedb.C /main/5 1998/03/20 14:29:07 mgreess $ 			 				
/*
 * @(#)mp_typedb.C	1.54 93/07/29 SMI
 *
 * mp_typedb.cc - _Tt_typedb represents the database of ToolTalk types
 *
 * Copyright (c) 1990,1992 by Sun Microsystems, Inc.
 */

//
// Contains methods for reading and writing type databases which can be
// stored either as Classing Engine databases or a native xdr format
// databases. 
//
#include <stdlib.h>
#if defined(linux) || defined(CSRG_BASED)
/*# include <g++/minmax.h>*/
#else
# include <macros.h>
#endif
#include <fcntl.h>
#include "tt_options.h"
#include "mp/mp_arg.h"
#include "mp/mp.h"
#include "mp_otype.h"
#include "mp_ptype.h"
#include "mp_typedb.h"
#include "api/c/api_api.h"
#include "tttk/tttk.h"
#include <sys/stat.h>
#include <errno.h>
#include <sys/wait.h>
#include "mp_ce_attrs.h"
#ifdef OPT_CLASSING_ENGINE
/*
 * See the comments in ./ce.h for why local includes are used instead of
 * the real ce.h.
 */
#	include "ce.h"
#	include "ce_err.h"
#endif
#include <stdlib.h>
#include <unistd.h>
#include "util/tt_enumname.h"
#include "util/tt_port.h"
#include "util/tt_gettext.h"
#include "util/tt_global_env.h"
#include "util/tt_xdr_version.h"

#if defined(ultrix)
extern "C" void xdrstdio_create(XDR *, FILE *, enum xdr_op);
#endif


enum _Tt_typedb_flags {
	_TT_TYPEDB_USER,
	_TT_TYPEDB_SYSTEM,
	_TT_TYPEDB_NETWORK,
	_TT_TYPEDB_XDR_MODE,
	_TT_TYPEDB_LOCKED
};

#ifdef OPT_CLASSING_ENGINE
#	ifdef OPT_DLOPEN_CE
#		include <util/tt_ldpath.h>
#		include <dlfcn.h>

        typedef void *(*_Tt_cefn_ptr)(...);
	// Function-table for all Classing Engine functions used. Note
	// that it is important that the function slots be named the
	// same as the corresponding Classing Engine function. This is
	// so the CALLCE macro works properly.
	//
	struct {
		_Tt_cefn_ptr	ce_abort_write;
		_Tt_cefn_ptr	ce_add_attribute;
		_Tt_cefn_ptr	ce_add_entry;
		_Tt_cefn_ptr	ce_add_namespace;
		_Tt_cefn_ptr	ce_alloc_entry;
		_Tt_cefn_ptr	ce_alloc_ns_entry;
		_Tt_cefn_ptr	ce_begin;
		_Tt_cefn_ptr	ce_commit_write;
		_Tt_cefn_ptr	ce_get_attribute;
		_Tt_cefn_ptr	ce_get_attribute_id;
		_Tt_cefn_ptr	ce_get_attribute_name;
		_Tt_cefn_ptr	ce_get_attribute_type;
		_Tt_cefn_ptr	ce_get_entry_db_info;
		_Tt_cefn_ptr	ce_get_namespace_id;
		_Tt_cefn_ptr	ce_map_through_attrs;
		_Tt_cefn_ptr	ce_map_through_entries;
		_Tt_cefn_ptr	ce_remove_entry;
		_Tt_cefn_ptr	ce_start_write;
	} _tt_celib;
	//
	// define CALLCE such that it indirects through a function
	// slot in _tt_celib which will be filled in by dlsyming
	// entry points from a dynamically loaded libce.so
	//
#	define CALLCE(fn) (*_tt_celib . fn)
#	else
        // assume libce.so is linked in so CALLCE just becomes a
	// function call.
#	define CALLCE(fn) fn
#	endif				// OPT_DLOPEN_CE
/*
 * Handle to the Type namespace
 */
static CE_NAMESPACE	_ce_type_ns = (CE_NAMESPACE)0;
/* 
 * Set of CE attributes used to avoid having to do string comparisons for
 * string fields in the CE databases. Each CE namespace assigns unique
 * numeric ids to all attribute names in order to provide a cheap way for
 * clients to compare against attribute names.
 */
static CE_ATTRIBUTE	_tt_attrs[_TT_CE_ATTR_LAST];
static CE_NAMESPACE	_ce_write_ns = (CE_NAMESPACE)0;
#endif				// OPT_CLASSING_ENGINE


#ifdef OPT_CLASSING_ENGINE
_Tt_typedb::
_Tt_typedb(char *ce_dir)
{
	_ce_dir = ce_dir;
#else
_Tt_typedb::
_Tt_typedb(char * /* ce_dir */)
{
#endif
	_flags = 0;
	ptable = new _Tt_ptype_table(_tt_ptype_ptid);
	otable = new _Tt_otype_table(_tt_otype_otid);
	ceDB2Use = TypedbAll;
}


_Tt_typedb::
~_Tt_typedb()
{
}


//
// XDR's a _Tt_typedb object. 
bool_t _Tt_typedb::
xdr(XDR *xdrs)
{
	if (!ptable.xdr(xdrs)) {
		return  0;
	}

	if (!otable.xdr(xdrs)) {
		return  0;
	}

	if  (xdrs->x_op == XDR_DECODE) {

		// We have to be sure that a null ptable or otable are
		// NEVER passed into the _Tt_object_table::xdr. XXX.

		if (ptable.is_null()) {
			ptable = new _Tt_ptype_table(_tt_ptype_ptid);
		}
		if (otable.is_null()) {
			otable = new _Tt_otype_table(_tt_otype_otid);
		}
	}

	return 1;
}



// 
// This function returns the full pathnames to the user database, system
// database, and network databases in udb, sdb, and ndb respectively.
// This three-level model of databases is intended to be similar to the
// Classing Engine model. The intent is that types in the user database
// shadow types in the system and network databases and that the types in
// the system database shadow the types in the network database. In order
// to provide some user-configurability, we provide an
// environment variable TTPATH that is a three-path list separated by ":"
// pointing to the user,system, and network databases. This function
// looks at that variable and returns the paths.
// 
//  If TTPATH isn't set this function should returns
//  $HOME/.tt/types.xdr for the user database, /etc/tt/types.xdr for the
//  system database, and $OPENWINHOME/etc/tt/types.xdr for the network
//  database.
// 
int
_tt_map_xdr_dbpaths(_Tt_string &udb, _Tt_string &sdb, _Tt_string &ndb)
{
	_Tt_string_list_ptr path = _Tt_typedb::tt_path();
	if (path.is_null()) return 1;
	_Tt_string_list_cursor pathC( path );
	if (! pathC.next()) {
		return 1;
	}
	udb = *pathC;
	if (! pathC.next()) {
		return 1;
	}
	sdb = *pathC;
	if (! pathC.next()) {
		return 1;
	}
	ndb = path->bot();
	return 1;
}

_Tt_string_list *
_Tt_typedb::tt_path()
{
	_Tt_string_list *pathlist = new _Tt_string_list;
	if (pathlist == 0) return 0;
	_Tt_string path = getenv("TTPATH");
	if (path.len() <= 0) {
		_Tt_string home = getenv("HOME");
		pathlist->append(home.cat("/.tt/types.xdr"));
		pathlist->append(_Tt_string("/etc/tt/types.xdr"));
		pathlist->append(
			_Tt_string("/usr/dt/appconfig/tttypes/types.xdr"));
		home = getenv("OPENWINHOME");
		if (home.len() == 0) {
			home = "/usr/openwin";
		}
		pathlist->append(home.cat(_Tt_string("/etc/tt/types.xdr")));
	} else {
		// parse the user:system:network from path variable
		int n = 0;
		_Tt_string pathname;
		while (n >= 0) {
			n = path.index(':');
			if (n > 0) {
				pathname = path.left(n);
				if (pathname.len() == 0) break;
				path = path.right(path.len() - n - 1);
			} else {
				pathname = path;
				path = 0;
			}
			if (_tt_isdir(pathname)) {
				pathname = pathname.cat("/types.xdr");
			}
			pathlist->append( pathname );
		}
	}
	return pathlist;
}


Tt_status _Tt_typedb::
init_xdr(const _Tt_string &compiled_file)
{
	//
	// A temporary is needed because
	// a types file is an XDRed _Tt_typedb_ptr, (XXX)
	// instead of an XDRed _Tt_typedb, and
	// _Tt_typedb_ptr::xdr() creates a new _Tt_typedb.
	// This was a bad choice, but we are stuck with it.
	//
	_Tt_typedb_ptr	tmpdb;
	Tt_status	status;
	//
	// Need to remember what kind of _Tt_typedb we are,
	// in case we are asked to remove or merge types.
	//
	_flags |= (1<<_TT_TYPEDB_XDR_MODE);
	
	status = merge_from(compiled_file, tmpdb);
	if (status != TT_OK) {
		return(status);
	}
	//
	// Now snare a reference to the tables of the temp db.
	// The temp db goes away, but we keep its tables.
	//
	if (!tmpdb.is_null()) {
		ptable = tmpdb->ptable;
		otable = tmpdb->otable;
	}
	return TT_OK;
}

Tt_status _Tt_typedb::
init_xdr(FILE *f)
{
	_Tt_typedb_ptr	tmpdb;
	Tt_status	status;
	int		version;

	_flags |= (1<<_TT_TYPEDB_XDR_MODE);

	status = merge_from(f, tmpdb, version);
	if (status != TT_OK) {
		return(status);
	}

	if (!tmpdb.is_null()) {
		ptable = tmpdb->ptable;
		otable = tmpdb->otable;
	}
	return TT_OK;
}

// 
// Initializes this object from the given xdr database.
// Returns:
//	TT_ERR_PATH		Bad $TTPATH
//	TT_ERR_NO_MATCH		version mismatch
//	TT_ERR_DBCONSIST	XDR failure, corrupt database
// 
Tt_status _Tt_typedb::
init_xdr(_Tt_typedbLevel xdb)
{
	_Tt_typedb_ptr		tmpdb;
	Tt_status		status;
	
	if (! _tt_map_xdr_dbpaths(user_db, system_db, network_db)) {
		_tt_syslog(stderr, LOG_ERR, "$TTPATH: %s", strerror(EINVAL));
		return(TT_ERR_PATH);
	}

	_flags |= (1<<_TT_TYPEDB_XDR_MODE);
	
	// type files are read in in reverse order of TTPATH
	// variable so that entries in databases to the left
	// of others in TTPATH shadow those to the right.
	
	_Tt_string_list_ptr path;
	switch (xdb) {
	    case TypedbAll:
	    default:
		path = tt_path();
		if (path.is_null()) {
			status = TT_ERR_NOMEM;
		} else {
			_Tt_string_list_cursor pathC( path );
			status = TT_OK;
			while (pathC.prev() && (status == TT_OK)) {
				status = merge_from(*pathC, tmpdb);
			}
			if (status == TT_OK) {
				_flags |= (1<<_TT_TYPEDB_NETWORK);
				_flags |= (1<<_TT_TYPEDB_SYSTEM);
				_flags |= (1<<_TT_TYPEDB_USER);
			}
		}
		break;
	    case TypedbNetwork:
		if (network_db.len()) {
			status = merge_from(network_db, tmpdb);
			if (status == TT_OK) {
				_flags |= (1<<_TT_TYPEDB_NETWORK);
			}
		} else {
			_tt_syslog(stderr, LOG_ERR, "!network_db.len()");
			status = TT_ERR_INTERNAL;
		}
		break;
	    case TypedbSystem:
		if (system_db.len()) {
			status = merge_from(system_db, tmpdb);
			if (status == TT_OK) {
				_flags |= (1<<_TT_TYPEDB_SYSTEM);
			}
		} else {
			_tt_syslog(stderr, LOG_ERR, "!system_db.len()");
			status = TT_ERR_INTERNAL;
		}
		break;
	    case TypedbUser:
		if (user_db.len()) {
			status = merge_from(user_db, tmpdb);
			if (status == TT_OK) {
				_flags |= (1<<_TT_TYPEDB_USER);
			}
		} else if (xdb != TypedbAll) {
			_tt_syslog(stderr, LOG_ERR, "!user_db.len()");
			status = TT_ERR_INTERNAL;
		}
		break;
	}
	if (status != TT_OK) {
		return status;
	}
	if (!tmpdb.is_null()) {
		ptable = tmpdb->ptable;
		otable = tmpdb->otable;
	}
	return(TT_OK);
}


// 
// Merges (or reads) types from dbpath into a (new) _Tt_typedb in tdb
// 
Tt_status _Tt_typedb::
merge_from(const _Tt_string &dbpath, _Tt_typedb_ptr &tdb)
{
	FILE			*f;
	Tt_status		result;
	int			version;

	// The automatic converter (ttce2xdr) will just touch the
	// user\'s .tt/types.xdr if there were no ToolTalk types in the
	// classing engine db.  This means that a zero-length file
	// is perfectly OK, it just contains no types.

	struct stat	stat_buf;
	if (stat( (char *)dbpath, &stat_buf ) == 0) {
		if (stat_buf.st_size == 0) {
			return TT_OK;
		}
	}

	if (f = fopen((char *)dbpath, "r")) {
		fcntl(fileno(f), F_SETFD, 1);	/* close on exec */
		result = merge_from(f, tdb, version);
		fclose(f);
	} else {
		// It is OK for the database not to exist, ToolTalk runs
		// even if there are no types.
		result = TT_OK;
	}
	
	if (result == TT_ERR_NO_MATCH) {
		// This file is newer than we are, so we cannot
		// decode it.
		_tt_syslog(stderr, LOG_ERR,
			   catgets(_ttcatd, 2, 9,
				   "%s is a version %d types "
				   "database, and this version "
				   "can only read versions %d and earlier"),
			   (char *)dbpath, version,
			   TT_PUSH_ROTATE_XDR_VERSION);
	} else if (result == TT_ERR_DBCONSIST) {
		_tt_syslog(stderr, LOG_ERR,
			   catgets(_ttcatd, 2, 10,
				   "could not decode types from types "
				   "database: %s. It may be damaged."),
			   (char *)dbpath);
	}

	return TT_OK;
}


// 
// Merges (or reads) types from f into a (new) _Tt_typedb in tdb
// 
Tt_status _Tt_typedb::
merge_from(FILE *f, _Tt_typedb_ptr &tdb, int &version)
{
	XDR			xdrs;

	xdrstdio_create(&xdrs, f, XDR_DECODE);
	return merge_from(&xdrs, tdb, version);
}

// 
// Merges (or reads) types from xdrs into a (new) _Tt_typedb in tdb
// 
// Picks the version off the xdr xstream and then invokes the
// xdr method on the given _Tt_typedb object to merge in the types.
// Used both by merge_from above to read in files and to handle
// the types sent over from clients via tt_session_types_load().
// 
Tt_status _Tt_typedb::
merge_from(XDR *xdrs, _Tt_typedb_ptr &tdb, int &version)
{
	
	if (! xdr_int(xdrs, &version)) {
		return TT_ERR_DBCONSIST;
	}
	if (version > TT_PUSH_ROTATE_XDR_VERSION) {
		// This file is newer than we are, so we cannot
		// decode it.
		return TT_ERR_NO_MATCH;
	}
	
	_Tt_xdr_version	xvers(version);
	
	if (! tdb.xdr(xdrs)) {
		return TT_ERR_DBCONSIST;
	}
	return TT_OK;
}

#ifdef OPT_CLASSING_ENGINE

// 
// Function that prints out the given Classing Engine error code to
// stderr.
// 
static void
print_ce_error(int ce_err)
{
	_tt_syslog(stderr, LOG_ERR, "Classing Engine: %d (%s)",
		   ce_err, _tt_enumname( (_Tt_ce_status)ce_err ));
}
#endif				// OPT_CLASSING_ENGINE


// 
// Aborts a write transaction to a databse. If this is a write to an xdr
// database then the lock file is removed. Otherwise it is a Classing
// Engine database and the appropiate abort function is invoked.
// 
int _Tt_typedb::
abort_write()
{
#ifdef OPT_CLASSING_ENGINE
	if (_flags&(1<<_TT_TYPEDB_XDR_MODE)) {
#endif

		(void)unlink((char *)_lock_file);
		return(1);

#ifdef OPT_CLASSING_ENGINE
	} else {
		int	   ce_err;

		ce_err = (int)CALLCE(ce_abort_write)(0);

		return(ce_err != 0);
	}
#endif				// !OPT_CLASSING_ENGINE
}


// 
// Removes an otype with id otid from the database either in xdr format
// or in Classing Engine format. If the database is in xdr format then
// this method just removes the otype from the in-memory otype table
// since it is assumed that a subsequent _Tt_typedb::end_write will cause
// the in-memory otype table to supersede the one on disk. If the
// database is a Classing Engine database then we remove it using the
// appropiate Classing Engine routines.
// 
int _Tt_typedb::
remove_otype(_Tt_string otid)
{
	_Tt_otype_ptr   ot;
	if (! otable->lookup(otid,ot)) {
		return(1);
	}

#ifdef OPT_CLASSING_ENGINE
	char		*db_name, *db_path;

	if (! (_flags&(1<<_TT_TYPEDB_XDR_MODE))) {
		int				ce_err;
		_Tt_signature_list_cursor	sigs;

		// remove the otype entry from the ce database
		// XXX: shouldn't this be done only if this entry was
		// in the database we have open for write?? (see how
		// this is done below in removing signatures)
		ce_err = (int)CALLCE(ce_remove_entry)(_ce_type_ns,
						      (CE_ENTRY)ot->ce_entry);

		// remove all the observer signatures (which are
		// separate entries) from the ce databse.
		sigs.reset(ot->_osigs);
		while (ce_err == 0 && sigs.next()) {
			// get the db info for this ce entry.
			ce_err = (int)CALLCE(ce_get_entry_db_info)(_ce_type_ns,
							      (CE_ENTRY)sigs->ce_entry,
							      &db_name, &db_path);
			if (ce_err==0 && _ce_dir==db_name) {
				// if this entry was in the ce
				// database we have open for write
				// then remove it.
				ce_err = (int)CALLCE(ce_remove_entry)(_ce_type_ns,
								 (CE_ENTRY)sigs->ce_entry);
			}
		}

		// remove all the handler signatures (which are
		// separate entries) from the ce databse.
		sigs.reset(ot->_hsigs);
		while (ce_err == 0 && sigs.next()) {
			// get the db info for this ce entry.
			ce_err = (int)CALLCE(ce_get_entry_db_info)(_ce_type_ns,
							      (CE_ENTRY)sigs->ce_entry,
							      &db_name, &db_path);
			if (ce_err==0 && _ce_dir==db_name) {
				// if this entry was in the ce
				// database we have open for write
				// then remove it.
				ce_err = (int)CALLCE(ce_remove_entry)(_ce_type_ns,
								 (CE_ENTRY)sigs->ce_entry);
			}
		}
		if (ce_err != 0) {
			print_ce_error(ce_err);
			return(0);
		}
	}
#endif				// OPT_CLASSING_ENGINE

	otable->remove(otid);
	return(1);
}



// 
// Removes an ptype with id ptid from the database either in xdr format
// or in Classing Engine format. If the database is in xdr format then
// this method just removes the ptype from the in-memory ptype table
// since it is assumed that a subsequent _Tt_typedb::end_write will cause
// the in-memory ptype table to supersede the one on disk. If the
// database is a Classing Engine database then we remove it using the
// appropiate Classing Engine routines.
// 
int _Tt_typedb::
remove_ptype(_Tt_string ptid)
{
	_Tt_ptype_ptr   pt;
	if (! ptable->lookup(ptid,pt)) {
		return(1);
	}

#ifdef OPT_CLASSING_ENGINE
	char		*db_name, *db_path;

	if (! (_flags&(1<<_TT_TYPEDB_XDR_MODE))) {
		int				ce_err;
		_Tt_signature_list_cursor	sigs;

		// remove the ptype entry from the database
		// XXX: shouldn't this be done only if this entry was
		// in the database we have open for write?? (see how
		// this is done below in removing signatures)
		ce_err = (int)CALLCE(ce_remove_entry)(_ce_type_ns,
						 (CE_ENTRY)pt->ce_entry);

		// remove the ptype's observer signatures from the
		// database. 
		sigs.reset(pt->_osigs);
		while (ce_err==0 && sigs.next()) {
			// get the db info for this ce entry.
			ce_err = (int)CALLCE(ce_get_entry_db_info)(_ce_type_ns,
							      (CE_ENTRY)sigs->ce_entry,
							      &db_name, &db_path);
			if (ce_err==0 && _ce_dir==db_name) {
				// if this entry was in the ce
				// database we have open for write
				// then remove it.
				ce_err = (int)CALLCE(ce_remove_entry)(_ce_type_ns,
								 (CE_ENTRY)sigs->ce_entry);
			}
		}

		// remove the ptype's handler signatures from the database.
		sigs.reset(pt->_hsigs);
		while (ce_err==0 && sigs.next()) {
			// get the db info for this ce entry.
			ce_err = (int)CALLCE(ce_get_entry_db_info)(_ce_type_ns,
							      (CE_ENTRY)sigs->ce_entry,
							      &db_name, &db_path);
			if (ce_err==0 && _ce_dir==db_name) {
				// if this entry was in the ce
				// database we have open for write
				// then remove it.
				ce_err = (int)CALLCE(ce_remove_entry)(_ce_type_ns,
								 (CE_ENTRY)sigs->ce_entry);
			}
		}
		if (ce_err != 0) {
			print_ce_error(ce_err);
			return(0);
		}
	}
#endif				// OPT_CLASSING_ENGINE

	ptable->remove(ptid);
	return(1);
}


// 
// Inserts a new ptype into the ptype table for this type database. If
// this database is stored in xdr format then this method just inserts
// the object into the in-memory table since it assumes a subsequent
// invocation of _Tt_typedb::end_write will write out the new table to
// disk. In Classing Engine format we create a Classing Engine entry and
// then use the appropiate ce functions to insert it into the database.
// 
int _Tt_typedb::
insert(_Tt_ptype_ptr &pt)
{

	ptable->insert(pt);


#ifndef OPT_CLASSING_ENGINE
	return(1);
#else	
	if (_flags&(1<<_TT_TYPEDB_XDR_MODE)) {
		return(1);
	}

	int				ce_err;
	_Tt_signature_list_cursor	sigs;

	pt->ce_entry = make_ce_entry(pt);
	ce_err = (int)CALLCE(ce_add_entry)(_ce_write_ns, pt->ce_entry);

	// insert all observer signatures as separate classing engine
	// entries. 
	sigs.reset(pt->_osigs);
	while (ce_err==0 && sigs.next()) {
		if (sigs->otid().len()) {
			continue;
		}
		sigs->ce_entry = (void *)make_ce_entry(*sigs);
		if (sigs->ce_entry == (void *)0) {
			return(0);
		}
		ce_err = (int)CALLCE(ce_add_entry)(_ce_write_ns,
					      (CE_ENTRY)sigs->ce_entry);
	}

	// insert all handler signatures as separate classing engine
	// entries. 
	sigs.reset(pt->_hsigs);
	while (ce_err==0 && sigs.next()) {
		if (sigs->otid().len()) {
			continue;
		}
		sigs->ce_entry = (void *)make_ce_entry(*sigs);
		if (sigs->ce_entry == (void *)0) {
			return(0);
		}
		ce_err = (int)CALLCE(ce_add_entry)(_ce_write_ns,
					      (CE_ENTRY)sigs->ce_entry);
	}
	if (ce_err != 0) {
		print_ce_error(ce_err);
		return(0);
	}

	return(1);
#endif				// !OPT_CLASSING_ENGINE

}


// 
// Inserts a new otype into the ptype table for this type database. If
// this database is stored in xdr format then this method just inserts
// the object into the in-memory table since it assumes a subsequent
// invocation of _Tt_typedb::end_write will write out the new table to
// disk. In Classing Engine format we create a Classing Engine entry and
// then use the appropiate ce functions to insert it into the database.
// 
int _Tt_typedb::
insert(_Tt_otype_ptr &ot)
{

	otable->insert(ot);

#ifndef OPT_CLASSING_ENGINE
	return(1);
#else	
	if (_flags&(1<<_TT_TYPEDB_XDR_MODE)) {
		return(1);
	}

	int				ce_err;
	_Tt_signature_list_cursor	sigs;

	ot->ce_entry =  make_ce_entry(ot);
	if (ot->ce_entry == (void *)0) {
		return(0);
	}
	ce_err = (int)CALLCE(ce_add_entry)(_ce_write_ns, ot->ce_entry);

	// insert all the otype's observer signatures as separate
	// entries. 
	sigs.reset(ot->_osigs);
	while (ce_err==0 && sigs.next()) {
		sigs->ce_entry = (void *)make_ce_entry(*sigs);
		if (sigs->ce_entry == (void *)0) {
			return(0);
		}
		ce_err = (int)CALLCE(ce_add_entry)(_ce_write_ns,
					      (CE_ENTRY)sigs->ce_entry);
	}

	// insert all the otype's handler signatures as separate
	// entries. 
	sigs.reset(ot->_hsigs);
	while (ce_err==0 && sigs.next()) {
		sigs->ce_entry = (void *)make_ce_entry(*sigs);
		if (sigs->ce_entry == (void *)0) {
			return(0);
		}
		ce_err = (int)CALLCE(ce_add_entry)(_ce_write_ns,
					      (CE_ENTRY)sigs->ce_entry);
	}
	if (ce_err != 0) {
		print_ce_error(ce_err);
		return(0);
	}

	return(1);
#endif				// !OPT_CLASSING_ENGINE
}


// 
// Prepares the database to be written out to disk. In Classing Engine
// mode this means we have to add the ToolTalk namespace to the database
// if it doesn't exist and invoke the appropiate Classing Engine function
// to start the write transaction. In xdr mode, we have to acquire the
// lock file to write out the database.
// 
int _Tt_typedb::
#ifdef OPT_CLASSING_ENGINE
begin_write(_Tt_typedbLevel db)
#else
begin_write(_Tt_typedbLevel /* db */)
#endif
{
#ifdef OPT_CLASSING_ENGINE
	if (! (_flags&(1<<_TT_TYPEDB_XDR_MODE))) {
		CE_ENTRY	cn;
		int		ce_err;
		int		i;
		_Tt_string	val;
		int		namespace_created = 0;

		ce_err = (int)CALLCE(ce_start_write)(level_name(db));
		if (ce_err != 0) {
			print_ce_error(ce_err);
			return(0);
		}
		ce_err = (int)CALLCE(ce_add_namespace)(TT_NS_NAME, &_ce_write_ns);
		switch (ce_err) {
		      case 0:
			// namespace not already there add it
			ce_err = (int)CALLCE(ce_alloc_ns_entry)(_ce_write_ns, &cn);
			if (ce_err != 0) {
				print_ce_error(ce_err);
				return(0);
			}
			val = "$CEPATH/tns_mgr.so";
			ce_err = (int)CALLCE(ce_add_attribute)(_ce_write_ns, &cn,
							  "NS_MANAGER",
							  "string",
							  val, val.len());
			for (i=0; ce_err == 0 && i < _TT_CE_ATTR_LAST; i++) {
				ce_err = (int)CALLCE(ce_alloc_entry)(_ce_write_ns,
								&cn);
				if (ce_err != 0) {
					break;
				}
				val = "$CEPATH/tns_mgr.so";
				ce_err = (int)CALLCE(ce_add_attribute)(_ce_write_ns,
								  &cn,
								  _tt_ce_attr_string((_Tt_ce_attr)i), "string", "attr", strlen("attr"));
			}
			if (ce_err != 0) {
				print_ce_error(ce_err);
				return(0);
			}
			namespace_created = 1;
			// fall into next case
		      case CE_ERR_NAMESPACE_EXISTS:
			if (!namespace_created) {
				_ce_write_ns = _ce_type_ns;
			}
			_ce_dir = level_name(db);
			return(1);
		      default:
			print_ce_error(ce_err);
			break;
		}

		return(0);
	}
#endif				// OPT_CLASSING_ENGINE

	int		n;
	int		fd;
	_Tt_string	path;
	_Tt_string	dir_path;

	// acquire a write lock
	if (_flags&(1<<_TT_TYPEDB_USER)) {
		path = user_db;
	} else if (_flags&(1<<_TT_TYPEDB_SYSTEM)) {
		path = system_db;
	} else if (_flags&(1<<_TT_TYPEDB_NETWORK)) {
		path = network_db;
	}

	n = path.rindex('/');
	if (n == -1) {
		dir_path = ".";
	} else {
		dir_path = path.left(n);
		
	}
	_lock_file = dir_path.cat("/.tt_lock");
	n = 0;

	// mkdir in case the "tt" or ".tt" subdirectory doesn't exist.

	(void)mkdir((char *)dir_path, 0777); // ignore errors, probably EEXIST

	while ((fd = open((char *)_lock_file,
			  O_WRONLY|O_CREAT|O_EXCL, 0777) == -1)
		&& errno == EEXIST)
	{
		_tt_syslog(stderr, LOG_ERR, "%s: %m",
			   (char *)_lock_file, strerror(EEXIST));
		if (n++ == 5) {
			_flags &= ~(1<<_TT_TYPEDB_LOCKED);
			return(0);
		}
		sleep(2);
	}
	if (fd > -1 ) {
		close(fd);	// Cleanup
	}
	_flags |= (1<<_TT_TYPEDB_LOCKED);
	return(1);
}


// 
// Commits the write transaction to the database. In Classing Engine mode
// we just call the ce_commit_write function. In xdr mode we write out
// the database to a temporary file and then rename the file to be the
// appropiate path.
// 
int _Tt_typedb::
end_write()
{
#ifdef OPT_CLASSING_ENGINE
	if (! (_flags&(1<<_TT_TYPEDB_XDR_MODE))) {
		int	ce_err = (int)CALLCE(ce_commit_write)(0);

		if (ce_err != 0) {
			print_ce_error(ce_err);
			return(0);
		}

		return(1);
	}
#endif				// OPT_CLASSING_ENGINE

	_Tt_string		dbpath;
	_Tt_string		dbpath_tmp;
	int			success;

	if (!(_flags&(1<<_TT_TYPEDB_LOCKED))) {
		return(0);
	}
	if (_flags&(1<<_TT_TYPEDB_USER)) {
		dbpath = user_db;
	} else if (_flags&(1<<_TT_TYPEDB_SYSTEM)) {
		dbpath = system_db;
	} else if (_flags&(1<<_TT_TYPEDB_NETWORK)) {
		dbpath = network_db;
	}
	dbpath_tmp = dbpath.cat("_tmp");
	success = write(dbpath_tmp) == TT_OK;
	if (! success) {
		(void)unlink((char *)dbpath_tmp);
		(void)unlink((char *)_lock_file);
		return(0);
	}

	success = (0 == rename((char *)dbpath_tmp, (char *)dbpath));
	if (success) {
		send_saved( dbpath );
		_tt_syslog(stdout, LOG_INFO,
			   catgets(_ttcatd, 2, 11, "Overwrote %s"),
			   (char *)dbpath);
	} else {
		_tt_syslog(stderr, LOG_ERR,
			   "rename( \"%s\", \"%s\" ): %m",
			   (char *)dbpath_tmp, (char *)dbpath);
	}
	(void)unlink((char *)_lock_file);
	(void)unlink((char *)dbpath_tmp);
	return(success);
}

Tt_status _Tt_typedb::
write(const _Tt_string &outfile)
{
	if (outfile.len() == 0) {
		return TT_DESKTOP_ENOENT;
	}
	FILE *f = fopen((char *)outfile,"w");
	if (f == 0) {
		_tt_syslog(stderr, LOG_ERR, "%s: %m", (char *)outfile);
		return _tt_errno_status( errno );
	}
	fcntl(fileno(f), F_SETFD, 1);	/* Close on exec */
	Tt_status status = write( f );
        fclose(f);
	return status;
}

Tt_status _Tt_typedb::
write(FILE *outfile)
{
	int	xdr_version;
        XDR	xdrs;
	_Tt_typedb_ptr		tdb_ptr = this;

	// For maximum compatibility, only write out types files
	// using the new XDR routines if the signatures have
	// contexts.
	xdr_version = xdr_version_required();
        _Tt_xdr_version xvers(xdr_version);
 
        xdrstdio_create(&xdrs, outfile, XDR_ENCODE);
        if (! xdr_int(&xdrs, &xdr_version) || ! tdb_ptr.xdr(&xdrs)) {
		_tt_syslog(stderr, LOG_ERR, "! _Tt_typedb_ptr::xdr()" );
                return TT_ERR_XDR;
        }
	return TT_OK;
}

//
// Do a tt_open() (if needed) and a ttdt_file_notice(), syslog()ing any error.
//
Tt_status _Tt_typedb::
send_saved(const _Tt_string &savedfile)
{
	const char	*default_opt = "Saved";

	char *procid = tt_default_procid();
	Tt_status status = tt_ptr_error( procid );
	switch (status) {
	    case TT_OK:
		tt_free( procid );
		break;
	    case TT_ERR_NOMP:
	    case TT_ERR_PROCID:
		procid = tt_open();
		status = tt_ptr_error( procid );
		if (status == TT_OK) {
			tt_free( procid );
		}
	}
	if (status != TT_OK) {
		//
		// No default session from which to send the notice,
		// so silently omit it.  In principle libtt can send
		// a file-scoped notice without having a default session,
		// but the API does not permit this.
		//
		return status;
	}
	//
	// The HP linker thinks that ttdt_file_notice() ultimately
	// depends on some Xt symbols.  The HP linker is wrong.
	// The AIX linker knows better.  On SunOS, of course, we
	// use dlopen().
	//
	Tt_message msg = tt_message_create();
	tt_message_class_set( msg, TT_NOTICE );
	tt_message_scope_set( msg, TT_FILE );
	tt_message_address_set( msg, TT_PROCEDURE );
	tt_message_op_set( msg, default_opt );
	status = tt_message_file_set( msg, savedfile );
	if (status != TT_OK) {
		_tt_syslog(stderr, LOG_ERR,
		   "tt_message_file_set(): %s", tt_status_message(status));
	}
	tt_message_arg_add( msg, TT_IN, "File", 0 );
	status = tt_message_send( msg );
	if (status != TT_OK) {
		_tt_syslog(stderr, LOG_ERR,
			   "tt_message_send(): %s", tt_status_message(status));
	}
	return status;
}

#ifdef OPT_CLASSING_ENGINE

//
// Creates a Classing Engine entry from a pointer to a _Tt_ptype
// object. A CE entry is essentially an attribute/value list. The
// names of the attributes are derived from the function
// _tt_ce_attr_string which returns a string from an enum describing
// the attribute that is to be written out.
//
void * _Tt_typedb::
make_ce_entry(_Tt_ptype_ptr &pt)
{
	CE_ENTRY			cn;
	int				ce_err;
	const char			*val;
	int				len;
	_Tt_ptype_prop_list_cursor	props;

	ce_err = (int)CALLCE(ce_alloc_entry)(_ce_write_ns, &cn);
	if (ce_err != 0) {
		print_ce_error(ce_err);
		return(void *)0;
	}
	val = (char *)pt->ptid();
	len = pt->ptid().len();
	ce_err = (int)CALLCE(ce_add_attribute)(_ce_write_ns, &cn,
				  _tt_ce_attr_string(_TYPE_NAME),
				  _tt_ce_attr_string(_TT_TOOLTALK_PTYPE),
				  val, len);
	if (ce_err != 0) {
		print_ce_error(ce_err);
		return(void *)0;
	}
	val = _tt_ce_attr_string(_TT_TOOLTALK_PTYPE);
	len = strlen(val);
	ce_err = (int)CALLCE(ce_add_attribute)(_ce_write_ns, &cn,
					  _tt_ce_attr_string(_TT_TOOLTALK_TYPE),
					  "string", val,len);
	if (ce_err != 0) {
		print_ce_error(ce_err);
		return(void *)0;
	}
	props.reset(pt->_props);
	while (props.next()) {
		val = (char *)props->value();
		len = props->value().len();
		ce_err = (int)CALLCE(ce_add_attribute)(_ce_write_ns, &cn,
						  (char *)props->name(),
						  "string", val, len);
		if (ce_err != 0) {
			print_ce_error(ce_err);
			return(void *)0;
		}
	}

	return((void *)cn);
}

//
// Creates a Classing Engine entry from a pointer to a _Tt_otype
// object. A CE entry is essentially an attribute/value list. The
// names of the attributes are derived from the function
// _tt_ce_attr_string which returns a string from an enum describing
// the attribute that is to be written out.
//
void * _Tt_typedb::
make_ce_entry(_Tt_otype_ptr &ot)
{
	CE_ENTRY			cn;
	int				ce_err;
	_Tt_string_list_cursor		anc;
	const char		       *val;
	int				len;

	ce_err = (int)CALLCE(ce_alloc_entry)(_ce_write_ns, &cn);
	if (ce_err != 0) {
		print_ce_error(ce_err);
		return((void *)0);
	}
	val = (char *)ot->_otid;
	len = ot->_otid.len();
	ce_err = (int)CALLCE(ce_add_attribute)(_ce_write_ns, &cn,
					  _tt_ce_attr_string(_TYPE_NAME),
					  _tt_ce_attr_string(_TT_TOOLTALK_OTYPE),
					  val, len);
	if (ce_err != 0) {
		print_ce_error(ce_err);
		return((void *)0);
	}	
	val = _tt_ce_attr_string(_TT_TOOLTALK_OTYPE);
	len = strlen(val);
	ce_err = (int)CALLCE(ce_add_attribute)(_ce_write_ns, &cn,
					  _tt_ce_attr_string(_TT_TOOLTALK_TYPE),
					  "string", val, len);
	if (ce_err != 0) {
		print_ce_error(ce_err);
		return((void *)0);
	}
	if (! ot->_ancestors.is_null()) {
		anc.reset(ot->_ancestors);
		while (anc.next()) {
			val = (char *)*anc;
			len = strlen(val);
			ce_err = (int)CALLCE(ce_add_attribute)(_ce_write_ns, &cn,
							  _tt_ce_attr_string(_TT_PARENT),
							  "string",val, len);

			if (ce_err != 0) {
				print_ce_error(ce_err);
				return(void *)0;
			}
		}
	}

	return((void *)cn);
}


//
// Creates a Classing Engine entry from a pointer to a _Tt_signature
// object. A CE entry is essentially an attribute/value list. The
// names of the attributes are derived from the function
// _tt_ce_attr_string which returns a string from an enum describing
// the attribute that is to be written out.
//
void * _Tt_typedb::
make_ce_entry(_Tt_signature_ptr &st)
{
	CE_ENTRY		cn;
	int			ce_err;
	_Tt_arg_list_cursor	argc;
	_Tt_string		val;
	const char	       *name;
	Tt_mode			amode;

	ce_err = (int)CALLCE(ce_alloc_entry)(_ce_write_ns, &cn);
	if (ce_err != 0) {
		print_ce_error(ce_err);
		return(void *)0;
	}
	if (st->_mangled_args.len() == 0) {
		// in order to uniquely name this signature we have to
		// play the same tricks that C++ does and write out
		// the ptype or otype, the operation name of the
		// signature and then an encoding of the signature
		// argument types to distinguish it in the case of
		// overloading. 

		st->_mangled_args = "_";
		argc.reset(st->_args);
		while (argc.next()) {
			amode = argc->mode();
			st->_mangled_args = st->_mangled_args.cat(_tt_enumname(amode)).cat(argc->type()).cat("_");
		}
	}

	val = ((st->_otid.len()) ? st->_otid : st->_ptid);
	val = val.cat("::").cat(st->_op).cat(st->_mangled_args);
	ce_err = (int)CALLCE(ce_add_attribute)(_ce_write_ns, &cn,
					  _tt_ce_attr_string(_TYPE_NAME),
					  _tt_ce_attr_string(_TT_TOOLTALK_TYPE),
					  (char *)val, val.len());
	if (ce_err != 0) {
		print_ce_error(ce_err);
		return(void *)0;
	}
	val = _tt_ce_attr_string(_TT_TOOLTALK_SIGNATURE);
	ce_err = (int)CALLCE(ce_add_attribute)(_ce_write_ns, &cn,
					  _tt_ce_attr_string(_TT_TOOLTALK_TYPE),
					  "string", (char *)val, val.len());
	if (ce_err != 0) {
		print_ce_error(ce_err);
		return(void *)0;
	}

	val = st->_op;
	ce_err = (int)CALLCE(ce_add_attribute)(_ce_write_ns, &cn,
					  _tt_ce_attr_string(_TT_OP),
					  "string", (char *)val, val.len());
	if (ce_err != 0) {
		print_ce_error(ce_err);
		return(void *)0;
	}

	val = _tt_enumname(st->_pattern_category);
	ce_err = (int)CALLCE(ce_add_attribute)(_ce_write_ns, &cn,
					  _tt_ce_attr_string(_TT_CATEGORY),
					  "string", (char *)val, val.len());
	if (ce_err != 0) {
		print_ce_error(ce_err);
		return(void *)0;
	}

	argc.reset(st->_args);
	int argn=0;
	_Tt_string	argc_type;
	char *attrname =
	  (char *)malloc(strlen(_tt_ce_attr_string(_TT_ARG)) + 5);
	char *attrnamenum = attrname+strlen(_tt_ce_attr_string(_TT_ARG));
	strcpy(attrname,_tt_ce_attr_string(_TT_ARG));

	while (argc.next()) {
		sprintf((char *)attrnamenum, "%d", argn);
		val = _tt_enumname(argc->mode());
		argc_type = argc->type();
		val = val.cat(" ").cat(argc_type).cat(" ").cat(argc->name());
		ce_err = (int)CALLCE(ce_add_attribute)(_ce_write_ns, &cn,
						       attrname,
						       "string",
						       (char *)val, val.len());
		if (ce_err != 0) {
			print_ce_error(ce_err);
			return(void *)0;
		}
		argn++;
	}
	free(attrname);

	name = ((st->_otid.len()) ? _tt_ce_attr_string(_TT_MSET_SCOPE) :
		_tt_ce_attr_string(_TT_SCOPE));
	val = _tt_enumname(st->_scope);
	ce_err = (int)CALLCE(ce_add_attribute)(_ce_write_ns, &cn, name, "string",
					  (char *)val, val.len());
	if (ce_err != 0) {
		print_ce_error(ce_err);
		return(void *)0;
	}

	val = _tt_enumname(st->_message_class);
	ce_err = (int)CALLCE(ce_add_attribute)(_ce_write_ns, &cn,
					  _tt_ce_attr_string(_TT_CLASS),
					  "string", (char *)val, val.len());
	if (ce_err != 0) {
		print_ce_error(ce_err);
		return(void *)0;
	}
	_Tt_string attr_typename = "string";
	switch (st->_reliability) {
	    case TT_START+TT_QUEUE:
#ifdef NOT_BACKWARD_COMPATIBLE
		val = _tt_ce_attr_string( _TT_START );
		val = val.cat( "+" ).cat( _tt_ce_attr_string( _TT_QUEUE ));
		ce_err = (int)CALLCE(ce_add_attribute)(_ce_write_ns, &cn,
						  _tt_ce_attr_string(_TT_MSET_DISPOSITION),
						  (char *)attr_typename,
						  (char *)val, val.len());
		if (ce_err != 0) {
			print_ce_error(ce_err);
			return(void *)0;
		}
		break;
#else
		//
		// Version 1.0 can only handle finding start, queue,
		// or discard in the disposition attribute (bug
		// 1082628).  So we will hide a clue that we are
		// something else in the typename of this attribute.
		// This works because in 1.0 _tt_convert_signature_attrs()
		// assumes the typename is "string", and never checks it.
		//
		attr_typename =
			attr_typename.cat( ":" )
			.cat( _tt_ce_attr_string( _TT_START ))
			.cat( "+" )
			.cat( _tt_ce_attr_string( _TT_QUEUE ));
		//
		// Now pretend it was START, so that we put start reliability
		// where 1.0 versions will find it.  We assume here
		// that starting is more important than queueing.
		//
		val = "TT_START";
		ce_err = (int)CALLCE(ce_add_attribute)(_ce_write_ns, &cn,
						  _tt_ce_attr_string(_TT_MSET_DISPOSITION),
						  (char *)attr_typename,
						  (char *)val, val.len());
		if (ce_err != 0) {
			print_ce_error(ce_err);
			return(void *)0;
		}
		break;
#endif /* NOT_BACKWARD_COMPATIBLE */
	    case TT_START:
		val = "TT_START";
		ce_err = (int)CALLCE(ce_add_attribute)(_ce_write_ns, &cn,
						  _tt_ce_attr_string(_TT_MSET_DISPOSITION),
						  (char *)attr_typename,
						  (char *)val, val.len());
		if (ce_err != 0) {
			print_ce_error(ce_err);
			return(void *)0;
		}
		break;
	    case TT_QUEUE:
		val = "TT_QUEUE";
		ce_err = (int)CALLCE(ce_add_attribute)(_ce_write_ns, &cn,
						  _tt_ce_attr_string(_TT_MSET_DISPOSITION),
						  (char *)attr_typename,
						  (char *)val, val.len());
		if (ce_err != 0) {
			print_ce_error(ce_err);
			return(void *)0;
		}
		break;
	    case TT_DISCARD:
	    default:
		val = "TT_DISCARD";
		ce_err = (int)CALLCE(ce_add_attribute)(_ce_write_ns, &cn,
						  _tt_ce_attr_string(_TT_MSET_DISPOSITION),
						  (char *)attr_typename,
						  (char *)val, val.len());
		if (ce_err != 0) {
			print_ce_error(ce_err);
			return(void *)0;
		}
		break;
	}
	char opnumstring[40];
	sprintf(opnumstring,"%d", st->_opnum);
	val = opnumstring;
	ce_err = (int)CALLCE(ce_add_attribute)(_ce_write_ns, &cn,
					  _tt_ce_attr_string(_TT_MSET_OPNUM),
					  "string", (char *)val, val.len());
	if (ce_err != 0) {
		print_ce_error(ce_err);
		return(void *)0;
	}

	val = st->_ptid;
	ce_err = (int)CALLCE(ce_add_attribute)(_ce_write_ns, &cn,
				  _tt_ce_attr_string(_TT_MSET_HANDLER_PTYPE),
				  "string", (char *)val, val.len());
	if (ce_err != 0) {
		print_ce_error(ce_err);
		return(void *)0;
	}

	if (st->_otid.len()) {
		val = st->_otid;
		ce_err = (int)CALLCE(ce_add_attribute)(_ce_write_ns, &cn,
					  _tt_ce_attr_string(_TT_MSET_OTYPE),
					  "string", (char *)val, val.len());
		if (ce_err != 0) {
			print_ce_error(ce_err);
			return(void *)0;
		}		
	}

	return((void *)cn);
}


// 
// This function is passed in as a callback to ce_map_through_attrs by
// _tt_ce_entry_to_ptype. It's function is to process a ptype
// attribute. This involves determining which attribute it is and then
// modifying the _Tt_ptype object (which is passed in as arg by the
// ce_map_through_atts call). 
// 
static void *
_tt_convert_ptype_attrs(CE_ATTRIBUTE attr, char *value, void *arg)
{
	char				*name;
	_Tt_string			name_s;
	_Tt_string			value_s;
	_Tt_ptype			*p = (_Tt_ptype *)arg;
	
	if (attr ==  _tt_attrs[_TYPE_NAME] ||
	    attr == _tt_attrs[_TT_TOOLTALK_TYPE]) {
		return(0);
	}
	name = (char *)CALLCE(ce_get_attribute_name)(attr);
	name_s = name;
	if (p->getprop(name_s, value_s)) {
		return(0);
	}
	// add property to ptype.
	value_s = value;
	p->appendprop(name_s, value_s);
	
	return(0);
}


//
// This function is passed in as a callback to ce_map_through_attrs by
// _tt_ce_entry_to_otype. Its function is to process the given
// attribute and modify the _Tt_otype object appropiately. This object
// is passed in as arg by ce_map_through_attrs. If this callback
// returns anything other than (void *)0, then it causes
// ce_map_through_attrs to return immediately without processing the
// rest of the current entry's attributes. This signals an error to
// _tt_ce_entry_to_otype.  The attribute is mapped to the appropiate
// tooltalk attribute by way of the _tt_attrs table (this avoid having
// to do repeated strcmps on the attribute's name).
//
static void *
_tt_convert_otype_attrs(CE_ATTRIBUTE attr, char *value, void *arg)
{
	_Tt_string_list_ptr	ancs;
	_Tt_string_list_cursor	anc_c;
	_Tt_otype		*o = (_Tt_otype *)arg;
	
	if (attr ==  _tt_attrs[_TYPE_NAME] ||
	    attr == _tt_attrs[_TT_TOOLTALK_TYPE]) {
		return(0);
	}
	if (attr == _tt_attrs[_TT_PARENT]) {
		ancs = o->parents();
		if (ancs.is_null()) {
			ancs = new _Tt_string_list();
			o->set_ancestors(ancs);
		}
		anc_c.reset(ancs);
		while (anc_c.next()) {
			if (*anc_c == value) {
				return(0);
			}
		}
		ancs->push(_Tt_string(value));
	}
	
	return(0);
}


//
// This function is passed in as a callback to ce_map_through_attrs by
// _tt_ce_entry_to_signature. It's function is to process the
// attribute and modify the _Tt_signature object (passed in as arg by
// ce_map_through_attrs). If this function returns anything other than
// (void *)0 then ce_map_through_attrs will return immediately with
// this value and _tt_ce_entry_to_signature will return an error. 
// The attribute is mapped to the appropiate tooltalk attribute by way
// of the _tt_attrs table (this avoid having to do repeated strcmps on
// the attribute's name). 
//
static void *
_tt_convert_signature_attrs(CE_ATTRIBUTE attr, char *value, void *arg)
{
	char			*name;
	_Tt_signature		*s = (_Tt_signature *)arg;
	_Tt_arg_list_ptr	args;
	_Tt_arg_ptr		narg;
	_Tt_string		valstring;
	_Tt_string		mstring;
	_Tt_string		tstring;
	_Tt_string		nstring;
	CE_ATTRIBUTE		val_id;
	
	if (attr ==  _tt_attrs[_TYPE_NAME] ||
	    attr == _tt_attrs[_TT_TOOLTALK_TYPE]) {
		return 0;
	} else if (attr ==  _tt_attrs[_TT_OP]) {

		s->set_op(value);

	} else if (attr == _tt_attrs[_TT_CLASS]) {

		val_id = CALLCE(ce_get_attribute_id)(_ce_type_ns, value);
		if (val_id == _tt_attrs[_TT_REQUEST]) {
			s->set_message_class(TT_REQUEST);
		} else if (val_id == _tt_attrs[_TT_NOTICE]) {
			s->set_message_class(TT_NOTICE);
		} else {
			// XXX: assume TT_CLASS_UNDEFINED
			s->set_message_class(TT_CLASS_UNDEFINED);
		}

	} else if ((attr ==  _tt_attrs[_TT_SCOPE])
		   || (attr == _tt_attrs[_TT_MSET_SCOPE])) {

		val_id = CALLCE(ce_get_attribute_id)(_ce_type_ns, value);
		if (val_id ==  _tt_attrs[_TT_SESSION]) {
			s->set_scope(TT_SESSION);
		} else if (val_id == _tt_attrs[_TT_FILE]) {
			s->set_scope(TT_FILE);
		} else if (val_id == _tt_attrs[_TT_BOTH]) {
			s->set_scope(TT_BOTH);
		} else if (val_id == _tt_attrs[_TT_FILE_IN_SESSION]) {
			s->set_scope(TT_FILE_IN_SESSION);
		} else {
			return(arg);
		}

	} else if ((attr == _tt_attrs[_TT_DISPOSITION]) ||
		   (attr == _tt_attrs[_TT_MSET_DISPOSITION])) {

		_Tt_string typename = (char *)CALLCE(ce_get_attribute_type)(
							_ce_type_ns,
							s->ce_entry, attr );
		if (typename == "string") {
			val_id = CALLCE(ce_get_attribute_id)(_ce_type_ns, value);
			if (val_id == _tt_attrs[_TT_DISCARD]) {
				s->set_reliability(TT_DISCARD);
			} else if (val_id == _tt_attrs[_TT_QUEUE]) {
				s->set_reliability(TT_QUEUE);
			} else if (val_id == _tt_attrs[_TT_START]) {
				s->set_reliability(TT_START);
			} else {
				Tt_disposition disp= TT_DISCARD;
				_Tt_string valstring = value;
				_Tt_string left, right;

				while (valstring.len() > 0) {
					right = valstring.split('+', left);
					if (left.len() <= 0) {
						// XXX change split's behavior
						left = right;
						right = 0;
					}
					if (left ==
					    _tt_ce_attr_string(_TT_START)) {
						disp = (Tt_disposition)
						       (disp | TT_START);
					} else if (left ==
						   _tt_ce_attr_string(_TT_QUEUE)) {
						disp = (Tt_disposition)
						       (disp | TT_QUEUE);
					}
					valstring = right;
				}
				s->set_reliability( disp );
			}
		} else {
			_Tt_string hack = "string:";
			hack = hack.cat( _tt_ce_attr_string( _TT_START ))
				.cat( "+" )
				.cat( _tt_ce_attr_string( _TT_QUEUE ));
			if (typename == hack) {
				s->set_reliability (TT_START);
				s->set_reliability (TT_QUEUE);
			} else {
				return(arg);
			}
		}

	} else if (attr == _tt_attrs[_TT_MSET_HANDLER_PTYPE]) {

		s->set_ptid(value);

	} else if (attr == _tt_attrs[_TT_MSET_OTYPE]) {

		s->set_otid(value);

	} else if (attr == _tt_attrs[_TT_MSET_OPNUM]) {

		s->set_opnum(atoi(value));

	} else if (attr == _tt_attrs[_TT_CATEGORY]) {

		val_id = CALLCE(ce_get_attribute_id)(_ce_type_ns, value);
		if (val_id == _tt_attrs[_TT_OBSERVE]) {
			s->set_pattern_category(TT_OBSERVE);
		} else if (val_id == _tt_attrs[_TT_HANDLE]) {
			s->set_pattern_category(TT_HANDLE);
		} else {
			return(arg);
		}

	} else {
		
		// none of the above, check for TT_ARG%d which is
		// an argument specifier.

		name = (char *)CALLCE(ce_get_attribute_name)(attr);
		if (! strncmp(name,
			      _tt_ce_attr_string(_TT_ARG),
			      strlen(_tt_ce_attr_string(_TT_ARG)))) {
			narg = new _Tt_arg();
			valstring = value;
			valstring = valstring.split(' ',mstring);
			nstring = valstring.split(' ',tstring);
			val_id = CALLCE(ce_get_attribute_id)(_ce_type_ns,
							     (char *)mstring);
			if (val_id ==  _tt_attrs[_TT_OUT]) {
				narg->set_mode(TT_OUT);
			} else if (val_id ==  _tt_attrs[_TT_IN]) {
				narg->set_mode(TT_IN);
			} else if (val_id == _tt_attrs[_TT_INOUT]) {
				narg->set_mode(TT_INOUT);
			} else {
				return(arg);
			}
			narg->set_type(tstring);
			narg->set_name(nstring);
			s->append_arg(narg);
		} else {
			_tt_syslog(stderr, LOG_ERR,
				   catgets(_ttcatd, 2, 12,
					   "Ignoring unknown attribute <%s> "
					   "of ToolTalk signature..."), name );
		}
	}
	
	return(0);
}

// 
// Called by _tt_collect_types_in_cedb to produce a _Tt_ptype object from
// the given Classing Engine entry. Uses the _tt_convert_ptype_attrs
// function as a callback to ce_map_through_attrs to do the real work.
// 
static int
_tt_ce_entry_to_ptype(_Tt_ptype *p, CE_ENTRY ne)
{
	p->ce_entry = (void *)ne;
	return(0==CALLCE(ce_map_through_attrs)(_ce_type_ns, ne,
					       _tt_convert_ptype_attrs, p));
}


// 
// Called by _tt_collect_types_in_cedb to produce a _Tt_otype object from
// the given Classing Engine entry. Uses the _tt_convert_otype_attrs
// function as a callback to ce_map_through_attrs to do the real work.
// 
static int 
_tt_ce_entry_to_otype(_Tt_otype *o, CE_ENTRY ne)
{
	o->ce_entry = (void *)ne;
	return(0==CALLCE(ce_map_through_attrs)(_ce_type_ns, ne,
					       _tt_convert_otype_attrs, o));
}


// 
// Called by _tt_collect_types_in_cedb to produce a _Tt_signature object from
// the given Classing Engine entry. Uses the _tt_convert_signature_attrs
// function as a callback to ce_map_through_attrs to do the real work.
// 
static int
_tt_ce_entry_to_signature(_Tt_signature *s, CE_ENTRY ne)
{
	s->ce_entry = (void *)ne;
	return(0==CALLCE(ce_map_through_attrs)(_ce_type_ns, ne,
					       _tt_convert_signature_attrs,
					       s));
}


//
// This function is a callback passed in to ce_map_though_entries. It
// is applied to each entry in the ToolTalk namespace in a Classing
// Engine database. Each entry specifies either a ptype, otype or
// signature. For each class of entry the appropiate _tt_ce_entry_to_*
// function is invoked. 
//
void *
_tt_collect_types_in_cedb(CE_NAMESPACE ns, CE_ENTRY ne, void *t)
{
	_Tt_otype_ptr		o;
	_Tt_ptype_ptr		p;
	_Tt_signature_ptr	s;
	_Tt_typedb		*tdb = (_Tt_typedb *)t;
	char			*type;
	CE_ATTRIBUTE		type_id;
	char			*ptid;
	char			*otid;
	int			insert_required = 0;
	
	// XXX: should use ce_get_attribute_type when it's implemented
	//
	// type = CALLCE(ce_get_attribute_type)(ne, _TT_TYPE_NAME);

	if (tdb->ceDB2Use != TypedbAll) {
		//
		// We have been restricted to a single CE database.
		// Ignore this entry if it is not from the right database.
		// XXX CE should have had a better way to do this, such
		// as a parameter to ce_begin().
		//
		char *db_name, *db_path;
		int ceStatus = (int)CALLCE(ce_get_entry_db_info)(
						ns, ne, &db_name, &db_path );
		if (ceStatus != 0) {
			return 0;
		}
		if (_Tt_typedb::level( db_name ) != tdb->ceDB2Use) {
			return 0;
		}
	}

	type = (char *)CALLCE(ce_get_attribute)(ns, ne, _tt_attrs[_TT_TOOLTALK_TYPE]);
	if (type == (char *)0) {
		return(0);
	}
	type_id = CALLCE(ce_get_attribute_id)(_ce_type_ns, type);
	if (type_id == _tt_attrs[_TT_TOOLTALK_PTYPE]) {

		// ptype entry

		ptid = (char *)CALLCE(ce_get_attribute)(ns, ne,
							_tt_attrs[_TYPE_NAME]);
		if (tdb->ptable.is_null()) {
			tdb->ptable = new _Tt_ptype_table(_tt_ptype_ptid);
		}
		if (! tdb->ptable->lookup(ptid,p)) {
			p = new _Tt_ptype();
			p->set_ptid(ptid);
			insert_required = 1;
		}
		if (!_tt_ce_entry_to_ptype(p.c_pointer(), ne)) {
			return(t);
		}
		if (insert_required) {
			tdb->ptable->insert(p);
		}
		
		return(0);
	} else if (type_id == _tt_attrs[_TT_TOOLTALK_OTYPE]) {

		// otype entry

		otid = (char *)CALLCE(ce_get_attribute)(ns, ne,
							_tt_attrs[_TYPE_NAME]);
		if (tdb->otable.is_null()) {
			tdb->otable = new _Tt_otype_table(_tt_otype_otid);
		}
		if (! tdb->otable->lookup(otid,o)) {
			o = new _Tt_otype(otid);
			insert_required = 1;
		}
		if (!_tt_ce_entry_to_otype(o.c_pointer(), ne)) {
			return(t);
		}
		if (insert_required) {
			tdb->otable->insert(o);
		}
	} else if (type_id == _tt_attrs[_TT_TOOLTALK_SIGNATURE]) {

		// signature entry

		_Tt_signature_list_ptr	sp;
		
		s = new _Tt_signature();
		if (! _tt_ce_entry_to_signature(s.c_pointer(), ne)) {
			// failed to decode signature
			return(t);
		}
		// first insert the signature into the relevant
		// ptype.
		if (! tdb->ptable->lookup(s->ptid(),p)) {
			p = new _Tt_ptype();
			p->set_ptid(s->ptid());
			tdb->ptable->insert(p);
		}
		switch (s->category()) {
		    case TT_OBSERVE:
			p->append_osig(s);
			break;
		    case TT_HANDLE:
		    case TT_HANDLE_PUSH:
		    case TT_HANDLE_ROTATE:
			p->append_hsig(s, s->category());
			break;
		}
		if (s->otid().len() > 0) {
			// otype signature
			if (tdb->otable.is_null()) {
				tdb->otable = new _Tt_otype_table(_tt_otype_otid);
			}
			if (! tdb->otable->lookup(s->otid(),o)) {
				o = new _Tt_otype(s->otid());
				tdb->otable->insert(o);
			}
			switch (s->category()) {
			    case TT_OBSERVE:
				o->append_osig(s);
				break;
			    case TT_HANDLE:
			    case TT_HANDLE_PUSH:
			    case TT_HANDLE_ROTATE:
				o->append_hsig(s, s->category());
				break;
			    default: // inherited
				o->append_inhs(s);
				break;
			}
		}
	}
	
	return(0);
}
#endif				// OPT_CLASSING_ENGINE


 
Tt_status _Tt_typedb::
init_ce(
#if defined(OPT_CLASSING_ENGINE)	
	_Tt_typedbLevel db
#else
	_Tt_typedbLevel
#endif	
)
{

#ifndef OPT_CLASSING_ENGINE
	_tt_syslog(stderr, LOG_ERR, "_Tt_typedb::init_ce()!");
	return(TT_ERR_INTERNAL);
#else
	int		i;
	CE_ATTRIBUTE	c;
	int		ce_err;

#ifdef OPT_DLOPEN_CE
	int		load_celib_environment();

	if (! load_celib_environment()) {
		_tt_syslog(stderr, LOG_ERR,
			   "dlopen( \"libce.so\", 1 ): %s", dlerror());
		return(TT_ERR_INTERNAL);
	}
#endif				// OPT_DLOPEN_CE

	ceDB2Use = db;
	if ((ce_err = (int)CALLCE(ce_begin)(0)) != 0) {
		print_ce_error(ce_err);
		return(TT_ERR_INTERNAL);
	}
	_ce_type_ns = CALLCE(ce_get_namespace_id)(TT_NS_NAME);
	if (_ce_type_ns == (CE_NAMESPACE)0) {
		return(TT_ERR_DBEXIST);
	}
	// Initialize the ce attributes relevant to ToolTalk types
	for (i = 0; i < _TT_CE_ATTR_LAST; i++) {
		c = CALLCE(ce_get_attribute_id)(_ce_type_ns,
					_tt_ce_attr_string((_Tt_ce_attr)i));
		// if c==0 then we can continue reading the database
		// since it just means we've encountered an attribute
		// not present in any of the namespace entries.
		_tt_attrs[i] = c;
	}
	if (0 != CALLCE(ce_map_through_entries)(_ce_type_ns,
						_tt_collect_types_in_cedb,
						(void *)this)) {
		return(TT_ERR_INTERNAL);
	}
	return(TT_OK);
#endif				// !OPT_CLASSING_ENGINE
}


Tt_status _Tt_typedb::
ce2xdr()
{
	struct stat	xdrStat, ceStat;
	int		status;
	int		xdrExists = 0;
	_Tt_string	home;
	_Tt_string	xdrdb;
	_Tt_string	cedb;

	home = getenv( "HOME" );
	xdrdb = home.cat( "/.tt/types.xdr" );
	status = stat( (char *)xdrdb, &xdrStat );
	if (status == 0) {
		xdrExists = 1;
	} else if (errno != ENOENT) {
		_tt_syslog( 0, LOG_ERR, "$HOME/.tt/types.xdr: %m" );
		return TT_ERR_FILE;
	}
	cedb = home.cat( "/.cetables/cetables" );
	status = stat( (char *)cedb, &ceStat );
	if (status != 0) {
		if (errno == ENOENT) {
			// No cetables to convert
			return TT_OK;
		} else {
			_tt_syslog( 0, LOG_ERR,
				    "$HOME/.cetables/cetables: %m" );
			return TT_ERR_FILE;
		}
	}
	if (xdrExists) {
		if (ceStat.st_mtime <= xdrStat.st_mtime) {
			// XDR database is newer
			return TT_OK;
		}
	}
	status = system( "ttce2xdr -d user &" );
	if (WIFEXITED(status)) {
		if (WEXITSTATUS(status) == 0) {
			return TT_OK;
		} else {
			_tt_syslog(stderr, LOG_ERR,
				   catgets(_ttcatd, 2, 13,
					   "ttce2xdr failed (status=%d); types"
					   " in Classing Engine \"user\" "
					   "database not converted..."),
				   WEXITSTATUS(status) );
			return TT_ERR_PTYPE_START;
		}
	} else {
		_tt_syslog(stderr, LOG_ERR, "system(\"ttce2xdr -d user\"): %d",
			   status );
		return TT_ERR_PTYPE_START;
	}
}

_Tt_typedbLevel _Tt_typedb::
level( const _Tt_string &dbname )
{
	if (dbname == "user") {
		return TypedbUser;
	} else if (dbname == "system") {
		return TypedbSystem;
	} else if (dbname == "network") {
		return TypedbNetwork;
	} else {
		return TypedbNone;
	}
}

const char * _Tt_typedb::
level_name( _Tt_typedbLevel db )
{
	switch (db) {
	    case TypedbUser:
		return "user";
	    case TypedbSystem:
		return "system";
	    case TypedbNetwork:
		return "network";
	    default:
		return 0;
	}
}


int _Tt_typedb::
xdr_version_required() const
{
	int version = TT_TYPESDB_DEFAULT_XDR_VERSION;
	_Tt_ptype_table_cursor 	ptypes(ptable);
	while (ptypes.next()) {
		if(ptypes->xdr_version_required() > version) {
			version = ptypes->xdr_version_required();
		}
//		version = max(version, ptypes->xdr_version_required());
	}
	_Tt_otype_table_cursor	otypes(otable);
	while (otypes.next()) {
		if(otypes->xdr_version_required() > version) {
			version = otypes->xdr_version_required();
		}
//		version = max(version, otypes->xdr_version_required());
	}
	return version;
}


static void
write_out_tt_attrs(const _Tt_ostream &os)
{
	int		i;

	FILE *fs = os.theFILE();
	for (i=0; i < _TT_CE_ATTR_LAST; i++) {
		fprintf(fs,"\t\t(%s,string,<attr>)\n",
			_tt_ce_attr_string((_Tt_ce_attr)i));
	}
	fprintf(fs,"\t)");
}


void _Tt_typedb::
pretty_print(const _Tt_ostream &os) const
{
	_Tt_ptype_table_cursor 	ptypes(ptable);
	while (ptypes.next()) {
		ptypes->pretty_print(os);
		os << "\n";
	}
	_Tt_otype_table_cursor	otypes(otable);
	while (otypes.next()) {
		otypes->pretty_print(os);
		os << "\n";
	}
}



void _Tt_typedb::
print(const _Tt_ostream &os) const
{
	if (write_ce_header(os)) {
		os << "\nNS_ENTRIES= (\n\t(\t";
		write_out_tt_attrs(os);
		ptable->print(_tt_ptype_print, os);
		otable->print(_tt_otype_print, os);
		os << "\n)\n}";
	}
}


int _Tt_typedb::
write_ce_header(const _Tt_ostream &os) const
{
	os << "{\nNS_NAME=" << TT_NS_NAME
	   << "\nNS_ATTR=(\n\t\t(NS_MANAGER,string,"
	      "<$CEPATH/tns_mgr.so>)\n\t)";
	return(1);
}



#if defined(OPT_DLOPEN_CE) && defined(OPT_CLASSING_ENGINE)

/************************************************************************** 
 * This section contains functions for dynamically loading in the
 * necessary ce functions. This avoids the need for applications to link
 * in libce. Note that all of this section is ifdefd with OPT_DLOPEN_CE so
 * only functions relevant to dynamic loading of ce should be included.
 **************************************************************************/



static int
load_celib_environment()
{
	int		load_celib_fns_from_handle(void *);
	void		*dlhandle;
	_Tt_string	path;

	// first try opening the current executable to see if the
	// functions have already been defined
	dlhandle = dlopen((char *)0, 1);
	if (load_celib_fns_from_handle(dlhandle)) {
		return(1);
	}

#if defined(OPT_BUG_SUNOS_4)	
	// Not strictly a bug, but SunOS 4.x doesn't do what we want.
	if (! tt_ldpath("libce.so", path)) {
		return(0);
	}
#else
	// SVR4 version of dlopen does the path searching for us
	// (yay!). 
	path = "libce.so";
#endif				// !SVR4

#if defined(OPT_BUG_SUNOS_4)
	// SunOS 4 knows nothing of RTLD options
	dlhandle = dlopen((char *)path, 1);
#else
	// XXX: In theory, RTLD_LAZY (the default) is the preferred way of
	// using dlopen. However, there were some bugs in early 5.0 versions
	// that made dlopen slow down considerably if RTLD_LAZY is used.
	// For now, we'll use RTLD_NOW which seems to get rid of the
	// behavior but perhaps not the cause.
	// Next time we do performance tuning we should retry with
	// RTLD_LAZY.
	dlhandle = dlopen((char *)path, RTLD_NOW);
#endif
	if (dlhandle == (void *)0) {
		return(0);
	}
	
	return(load_celib_fns_from_handle(dlhandle));
}


/* 
 * XXX: This is sort of ugly. dlopen doesn't make the symbol available
 * for normal dynamic linking. This means that if the library that is
 * dlopened itself uses some of the symbols that are made available by
 * the use of dlsym then the library needs to be linked in with the
 * executable. We circumvent this by defining those symbols directly
 * here. This is unfortunately sensitive to the implementation of libce
 * changing. Ie. if it is changed to use more symbols than
 * ce_get_attribute_id and ce_get_attribute then libce will have to be
 * linked in regardless of whether it is dlopened or not.
 * 
 * All of a sudden we're getting infinite loops here, under ce_begin().
 * So I'm commenting these two puppies out.  Brian 8/05/92
 *
CE_ATTRIBUTE
ce_get_attribute_id(CE_NAMESPACE ns, char *a)
{
	return((CE_ATTRIBUTE)CALLCE(ce_get_attribute_id)(ns,a));
}

char *
ce_get_attribute(CE_NAMESPACE ns, CE_ENTRY e, CE_ATTRIBUTE a)
{
	return((char *)CALLCE(ce_get_attribute)(ns,e,a));
}
 */


static int
load_celib_fns_from_handle(void *dlhandle)
{
#if defined(__STDC__)
#define	DLINK_FN(fn)\
	_tt_celib . fn = (_Tt_cefn_ptr)dlsym(dlhandle, #fn);		\
	if (_tt_celib . fn == (_Tt_cefn_ptr)0) {			\
		return(0);						\
	}
#else
#define	DLINK_FN(fn)\
	_tt_celib . fn = (_Tt_cefn_ptr)dlsym(dlhandle, "fn");		\
	if (_tt_celib . fn == (_Tt_cefn_ptr)0) {			\
		return(0);						\
	}
#endif /* __STDC__ */

	DLINK_FN(ce_abort_write)
	DLINK_FN(ce_add_attribute)
	DLINK_FN(ce_add_entry)
	DLINK_FN(ce_add_namespace)
	DLINK_FN(ce_alloc_entry)
	DLINK_FN(ce_alloc_ns_entry)
	DLINK_FN(ce_get_attribute_id)
	DLINK_FN(ce_begin)
	DLINK_FN(ce_commit_write)
	DLINK_FN(ce_get_attribute)
	DLINK_FN(ce_get_attribute_name)
	DLINK_FN(ce_get_attribute_type)
	DLINK_FN(ce_get_entry_db_info)
	DLINK_FN(ce_get_namespace_id)
	DLINK_FN(ce_map_through_attrs)
	DLINK_FN(ce_map_through_entries)
	DLINK_FN(ce_remove_entry)
	DLINK_FN(ce_start_write)

	return(1);
}

#endif				// OPT_DLOPEN_CE && OPT_CLASSING_ENGINE
