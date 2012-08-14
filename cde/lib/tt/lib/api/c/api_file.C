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
//%%  $XConsortium: api_file.C /main/4 1995/11/21 19:36:59 cde-sun $ 			 				
/*
 *
 * api_file.cc
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */

#include "mp/mp_c.h"
#include "api/c/tt_c.h"
#include "api/c/api_api.h"
#include "api/c/api_error.h"
#include "api/c/api_mp.h"
#include "api/c/api_handle.h"
#include "db/tt_db_network_path.h"
#include "util/tt_global_env.h"
#include "util/tt_audit.h"

static Tt_filter_action _tt_filter_callback(Tt_filter_function fn,
					    _Tt_string nodeid,
					    void *context,
					    void *accumulator);


/************************************************************************
* File Functions (FSpec A.3)						*
************************************************************************/

Tt_status
tt_file_join(const char *path)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("C", TT_FILE_JOIN, path);
 
        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status =  _tt_file_join(path);
 
        audit.exit(status);
        return status;
}


Tt_status
tt_file_quit(const char *path)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("C", TT_FILE_QUIT, path);
 
        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status =  _tt_file_quit(path);
 
        audit.exit(status);
        return status;
}


Tt_status
tt_file_destroy(const char *path)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("C", TT_FILE_DESTROY, path);
 
        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status =  _tt_file_destroy(path);
 
        audit.exit(status);
        return status;
}


Tt_status
tt_file_move(const char *old_path, const char *new_path)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("CC", TT_FILE_MOVE, old_path,
						new_path);
 
        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status = _tt_file_move(old_path, new_path);
 
        audit.exit(status);
        return status;
}

Tt_status
tt_file_copy(const char *old_path, const char *new_path)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("CC", TT_FILE_COPY, old_path,
						new_path);
 
        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status = _tt_file_copy(old_path, new_path);
 
        audit.exit(status);
        return status;
}


Tt_status
tt_file_objects_query(const char *path, Tt_filter_function filter, void *context,
		      void *accumulator)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("CXAA", TT_FILE_OBJECTS_QUERY, path, filter,
					context, accumulator);
 
        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

	status = _tt_file_objects_query(path, filter, context, accumulator);
 
        audit.exit(status);
        return status;
}


/* 
 * Joins the given file. This causes the file to be the default file and,
 * if a session has been joined, modifies the file scope patterns to
 * include this file explicitly.
 *
 * Possible errors:
 *	TT_ERR_NOMP
 *	TT_ERR_DBAVAIL
 *	TT_ERR_DBEXIST
 *	TT_ERR_PATH
 *	TT_ERR_POINTER
 */
Tt_status
_tt_file_join(const char *path)
{
 	_Tt_c_file_ptr	file;
  	Tt_status	status;
	_Tt_c_procid	*d_procid = _tt_c_mp->default_c_procid().c_pointer();

	status = _tt_mp->find_file( path, file, 1 );
	if (status != TT_OK) {
		return status;
	}
	if (file.is_null()) {
		return TT_ERR_INTERNAL;
	}
	status = file->c_join( _tt_c_mp->default_procid() );
   	if (status == TT_OK) {
  		d_procid->set_default_file(file->getNetworkPath());
 	}
  	return status;
}

/* 
 * Quits the file named by filepath. Any currently registered patterns
 * with file scope have the file removed from their file values. If the
 * named file is the default file, the file default is nulled. Returns:
 *	TT_ERR_POINTER
 *	TT_ERR_FILE
 */
Tt_status
_tt_file_quit(const char *path)
{
 	_Tt_c_file_ptr	file;
  	Tt_status	status;
	_Tt_c_procid	*d_procid = _tt_c_mp->default_c_procid().c_pointer();


	status = _tt_mp->find_file( path, file, 0 );
	if (status != TT_OK) {
		return status;
	}
	if (file.is_null()) {
		return TT_ERR_FILE;
	}
	status = file->c_quit( _tt_c_mp->default_procid() );
  	if (status == TT_OK) {
		if (    d_procid->default_file()
		     == file->getNetworkPath())
		{
			_Tt_string null;
			d_procid->set_default_file( null );
		}
 	}
  	return(status);
}

/* 
 * Tells TT the filetree rooted at filepath is gone.
 * All specs of objects in the filetree are destroyed.
 *
 * Possible errors:
 *	TT_ERR_NOMP
 *	TT_ERR_DBAVAIL
 *	TT_ERR_DBEXIST
 *	TT_ERR_ACCESS
 *	TT_ERR_FILE
 *	TT_ERR_POINTER
 *	TT_ERR_PATH
 */
Tt_status
_tt_file_destroy(const char *path)
{
 	_Tt_file_ptr	file;
	Tt_status	status;

	status = _tt_mp->find_file( path, file, 1 );
	if (status != TT_OK) {
		return status;
	}
	if (file.is_null()) {
		return TT_ERR_INTERNAL;
	}
	_Tt_db_results dberr = file->remove();
	switch (dberr) { // XXX
	    case TT_DB_OK:
		status = TT_OK;
		break;
	    case TT_DB_ERR_DB_CONNECTION_FAILED:
	        status = TT_ERR_DBEXIST;
		break;
	    case TT_DB_ERR_RPC_CONNECTION_FAILED:
	        status = TT_ERR_DBAVAIL;
		break;
	    case TT_DB_ERR_ACCESS_DENIED:
		status = TT_ERR_ACCESS;
		break;
	    default:
		status = TT_ERR_INTERNAL;
		break;
	}

	if (status == TT_OK) {
		_tt_mp->remove_file(path);
	}

	return status;
}


/* 
 * Tells TT the filetree rooted at filepath has been moved.
 * All specs of objects in the filetree are moved.
 * Destroys the objects in the destination filetree,
 * since a spec-merging facility would be dangerous to use,
 * and no fun to write.
 */
Tt_status
_tt_file_move(const char *oldPath, const char *newPath)
{
 	_Tt_file_ptr	file;

	Tt_status status = _tt_file_destroy( newPath );
	if (tt_is_err( status )) {
		return status;
	}

	status = _tt_mp->find_file( oldPath, file, 1 );
	if (status != TT_OK) {
		return status;
	}
	_Tt_db_results dberr = file->move( newPath );
	switch (dberr) { // XXX
	    case TT_DB_OK:
		status = TT_OK;
		break;
	    case TT_DB_ERR_DB_CONNECTION_FAILED:
	        status = TT_ERR_DBEXIST;
		break;
	    case TT_DB_ERR_RPC_CONNECTION_FAILED:
	        status = TT_ERR_DBAVAIL;
		break;
	    case TT_DB_ERR_ACCESS_DENIED:
		status = TT_ERR_ACCESS;
		break;
	    default:
		status = TT_ERR_INTERNAL;
		break;
	}

	if (status == TT_OK) {
		_tt_mp->remove_file(oldPath);
	}

	return status;
}

/* 
 * Tells TT the file named by filepath has been copied.
 * All specs of objects in the file are copied.
 */
Tt_status
_tt_file_copy(const char *oldPath, const char *newPath)
{
 	_Tt_file_ptr	file;
	Tt_status	status;

	status = _tt_mp->find_file( oldPath, file, 1 );
	if (status != TT_OK) {
		return status;
	}
	if (file.is_null()) {
		return TT_ERR_INTERNAL;
	}
	_Tt_db_results dberr = file->copy( newPath );
	switch (dberr) { // XXX
	    case TT_DB_OK:
		status = TT_OK;
		break;
	    case TT_DB_ERR_DB_CONNECTION_FAILED:
	        status = TT_ERR_DBEXIST;
		break;
	    case TT_DB_ERR_RPC_CONNECTION_FAILED:
	        status = TT_ERR_DBAVAIL;
		break;
	    case TT_DB_ERR_ACCESS_DENIED:
		status = TT_ERR_ACCESS;
		break;
	    default:
		status = TT_ERR_INTERNAL;
		break;
	}
	return status;
}

/* 
 * Invokes filter on all the nodes contained in filepath until all the
 * nodes have been visited or filter returns "TT_FILTER_STOP". `context'
 * and `accumulator' can be used to pass in context information to the
 * filter and accumulate results.
 */
Tt_status
_tt_file_objects_query(const char *path, Tt_filter_function filter, void *context,
		       void *accumulator)
{
 	_Tt_file_ptr	file;
  	Tt_status	status;

	status = _tt_mp->find_file( path, file, 1 );
	if (status != TT_OK) {
		return status;
	}
	status = file->query( _tt_filter_callback, filter, context,
			      accumulator );
	return status;
}

Tt_filter_action
_tt_filter_callback(Tt_filter_function fn, _Tt_string nodeid, void *context,
		    void *accumulator)
{
	//
	// The spec says that the char * we pass to the filter
	// function is only valid for a given invocation of the filter
	// function.  Using a _tt_strdup()'d string delivers on this
	// guarantee.
	//
	char *temp_nodeid = _tt_strdup(nodeid);
	Tt_filter_action ret_val = (*fn)( temp_nodeid, context, accumulator );
	_tt_free( temp_nodeid );

	return ret_val;
}

// Temporary function so mailtool can do better diagnosis in S493.
// Return the host on which the server for the file is.
// See bug 1113489.

extern "C" {
	Tt_status _ttds_file_server(const char *filename, char **hostname);
}

Tt_status
_ttds_file_server(const char *filename, const char **hostname)
{
 	_Tt_c_file_ptr	file;
	_Tt_db_results	dbresults;
	Tt_status	status;
	_Tt_string	filename_s;
	_Tt_string	local_path;
	_Tt_string	hostname_s;
	_Tt_string	partition;
	_Tt_string	network_path;
	
	*hostname = "";
	if (filename==0 || hostname==0) {
		return TT_ERR_POINTER;
	}
	filename_s = filename;
	
	dbresults =  _tt_db_network_path(filename_s,
					 local_path,
					 hostname_s,
					 partition,
					 network_path);
	
	// if this file\'s db has been redirected, it\'s the redirected
	// host, where the dbserver is, which is interesting to the caller.
	
	_Tt_string temp_hostname = _tt_global->db_hr_map.findEntry(hostname_s);
	if (!temp_hostname.is_null() && 0<temp_hostname.len()) {
		hostname_s = temp_hostname;
	}
	*hostname = _tt_strdup((const char *)hostname_s);
	// Currently _tt_get_api_error translates
	// TT_DB_ERR_DB_CONNECTION_FAILED into TT_ERR_DBAVAIL.
	// This is wrong, at least in the common case of rpc program
	// not registered, which should be TT_ERR_DBEXIST.
	// Probably the clnt_create for db connections ought to be
	// doing more analysis of the clnt_create results.
	if (dbresults==TT_DB_ERR_DB_CONNECTION_FAILED) {
		status = TT_ERR_DBEXIST;
	} else {
		status = _tt_get_api_error(dbresults, _TT_API_FILE);
	}

	return status;
}
