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
//%%  $TOG: tt_db_client.C /main/11 1999/10/14 18:40:38 mgreess $ 			 				
/*
 * @(#)tt_db_client.C	1.54 95/09/26
 *
 * tt_db_client.C - Define the TT DB client class.  This class defines a
 *                   client interface to the DB server.
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 *
 * NOTE:  Result fields need to be freed with xdr_free if they were
 *        allocated by XDR routines.  When "faking" the new calls
 *        on an old DBserver, the results are allocated with malloc.
 *        the correspoding tt_free... or just plain free() call 
 *        is then used.
 * XXX:   Pervasively, this code assumed freeing a null pointer is OK.
 * 	  While ANSI C specifies this is OK lots of platforms gag on it.
 *	  rather than try to figure out which free() calls may have nulls, I
 *        just wrapped them all in "if (0!=ptr) {...}".
 */

#include <fcntl.h>
#include <sys/param.h>
#include <sys/socket.h>

#include "util/tt_port.h"
#include "util/tt_gettext.h"
#include "db/tt_db_client.h"
#include "db/tt_db_access.h"
#include "db/tt_db_rpc_message_routines.h"
#include "db/db_server.h"
#include "db/tt_db_rpc_routines.h"
#include "db/tt_old_db_partition_map_ref.h"

/* Included after "util/tt_string.h" to avoid index/strchr conflicts. */
#define X_INCLUDE_NETDB_H
#define XOS_USE_XT_LOCKING
#include <X11/Xos_r.h>

// Some old versions of RPC headers don\'t define AUTH_NONE but
// do define the older AUTH_NULL flavor.

#if !defined(AUTH_NONE)
#define AUTH_NONE AUTH_NULL
#endif

static const char            TT_DB_RPC_PROTO[] = "tcp";
static const struct timeval  TT_DB_RPC_NORMAL_TIMEOUT = {1000000, 0};
static const struct timeval  TT_DB_RPC_QUICK_TIMEOUT = {4, 0};
const int              TT_DB_RPC_RETRIES = 3;

// ********** Old DB Server Compatibility Include Files **********
#include "db/tt_db_client_consts.h"
// ********** Old DB Server Compatibility Include Files **********

_Tt_db_client::_Tt_db_client()
{
  _Tt_string db_hostname = _tt_gethostname();

  setTtDBDefaults();
  connectToDB(db_hostname);
}

_Tt_db_client::_Tt_db_client (_Tt_db_results & status)
{
  _Tt_string db_hostname = _tt_gethostname();

  setTtDBDefaults();
  connectToDB(db_hostname);
  status = dbConnectionResults;
}

_Tt_db_client::_Tt_db_client (const _Tt_string &hostname, _Tt_db_results & status)
{
  setTtDBDefaults();
  connectToDB(hostname);
  status = dbConnectionResults;
}

_Tt_db_results _Tt_db_client::connectToDB (const _Tt_string &hostname)
{
	_tt_auth_level_results *auth_level_results = (_tt_auth_level_results *)NULL;
	int _socket;
	_socket = -1;


	dbHostname = hostname;

	// Connect to the dbserver on the specified host.
	// If we don't have TI_RPC we cannot depend on CLGET_FD, so
	// we have to use clnttcp_create so that we get the socket FD back
	// in order to set close_on_exec.
	
#if defined(OPT_TLI)

#ifdef OPT_HAS_CLNT_CREATE_TIMED
	
	struct timeval		tv = { OPT_CLNT_CREATE_TIMEOUT, 0};

	dbServer = clnt_create_timed((char *)dbHostname,
				     TT_DBSERVER_PROG,
				     TT_DBSERVER_VERS,
				     (char *)TT_DB_RPC_PROTO,
				     &tv);
#else
	dbServer = clnt_create((char *)dbHostname,
				TT_DBSERVER_PROG,
				TT_DBSERVER_VERS,
				(char *)TT_DB_RPC_PROTO);
#endif

	if (dbServer) {
		clnt_control(dbServer, CLGET_FD, (char *)&_socket);
	}
#else
	struct sockaddr_in server_addr;
	struct hostent	       *host_ret;
	_Xgethostbynameparams	host_buf;

	memset((char*) &host_buf, 0, sizeof(_Xgethostbynameparams));
	if ((host_ret = _XGethostbyname((char *)dbHostname, host_buf)) != NULL) {
		_socket = RPC_ANYSOCK;
		memset(&server_addr, 0, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(0);
		memcpy(&server_addr.sin_addr.s_addr,
		       *(host_ret->h_addr_list),
		       sizeof(server_addr.sin_addr.s_addr));

		dbServer = clnttcp_create(&server_addr,
					  TT_DBSERVER_PROG,
					  TT_DBSERVER_VERS,
					  &_socket,
					  0, 0);

	} else {
		// gethostbyname failed, fake RPC error
		dbServer = 0;
		rpc_createerr.cf_stat = RPC_UNKNOWNHOST;
	}
#endif	

	// Connection failed.
	if (!dbServer) {

		_tt_syslog(0, LOG_ERR, catgets(_ttcatd, 1, 3,
			 "clnt_create for rpc.ttdbserverd on %s failed%s"),
			 (char *)dbHostname, clnt_spcreateerror(""));

		// Set dbConnectionResults data member...
		SetError(rpc_createerr.cf_stat);

		return dbConnectionResults;
	}

	// Connection succeeded.
	clnt_control(dbServer, CLSET_TIMEOUT, (char *)&TT_DB_RPC_QUICK_TIMEOUT);

	dbConnectionResults = TT_DB_ERR_DB_CONNECTION_FAILED;	// Default value.


	for (;dbVersion > 0; dbVersion--) {

		if (dbVersion > 1) {
			auth_level_results =
				_tt_get_min_auth_level_1 ((void *)NULL, dbServer);
		} else {
			// If dbVersion == 1, then we are talking to an old DB server
			static _tt_auth_level_results results;

			char      *path = "";
			clnt_stat  rpc_status;
			int       *result = (int *)NULL;
			result = _tt_min_auth_level_1(&path, dbServer, &rpc_status);

			if (result) {
				results.results = TT_DB_OK;
				results.auth_level = *result;
				auth_level_results = &results;
			}
		}

		if ((auth_level_results) && (auth_level_results->results == TT_DB_OK)) {

			clnt_control(dbServer, CLSET_TIMEOUT,
				     (char *)&TT_DB_RPC_NORMAL_TIMEOUT);

			dbAuthLevel = auth_level_results->auth_level;
		 
			dbConnectionResults = TT_DB_OK;	// Default return value.

			switch (dbAuthLevel) {
				case AUTH_NONE:
				case AUTH_UNIX:
					dbServer->cl_auth = authunix_create_default();
				break;

#ifdef OPT_SECURE_RPC
				case AUTH_DES: {
					char    server_net_name [MAXNETNAMELEN+1];
					struct hostent	       *host_ret;
					_Xgethostbynameparams	host_buf;
					if (host2netname(server_net_name, dbHostname, 0) &&
					    ((host_ret = _XGethostbyname((char *)dbHostname, host_buf)) != NULL)) {
#ifdef OPT_TLI
						dbServerNetName = server_net_name;
#else
						memcpy((caddr_t) &dbSocket.sin_addr,
						       host_ret->h_addr, 
						       host_ret->h_length);
						dbSocket.sin_family = AF_INET;
						dbSocket.sin_port = 0;
#endif
					} else {
						dbConnectionResults =
							TT_DB_ERR_DB_CONNECTION_FAILED;
					}
				}
				break;
#endif // OPT_SECURE_RPC
				default:
					dbConnectionResults =
						 TT_DB_ERR_DB_CONNECTION_FAILED;
				break;
			}
			break;
		} else {
			// If _tt_get_min_auth_level_1 is not available, then we are talking
			// to an old DB server.
                        if (_tt_get_rpc_result() != RPC_AUTHERROR) {

				_tt_syslog(0, LOG_ERR,
					catgets(_ttcatd, 1, 4, "Error: rpc.ttdbserverd on %s is not running"),
					(char *)dbHostname);

				SetError(_tt_get_rpc_result());

				break;	// Give up and return error code.
			}
		}
	}	// end -for()-

	//
	// Cleanup if failure.
	//
	if (dbConnectionResults != TT_DB_OK) {
		if (dbServer) {
			clnt_destroy(dbServer);
			dbServer = (CLIENT *)NULL;
		}
	}

	// Set close-on-exec bit so a libtt client which forks and execs won't
	// be short some fd's in the child.
	if (-1 != _socket && -1 == fcntl(_socket, F_SETFD, 1)) {
		_tt_syslog( 0, LOG_ERR, "_Tt_db_client::connectToDb(): "
			    "fcntl(F_SETFD): %m");
	}		

	return dbConnectionResults;
}

void _Tt_db_client::
SetError(enum clnt_stat cf_stat)
{

#ifdef  notdef
printf("DEBUG _Tt_db_client::SetError() -- cf_stat == %d\n", cf_stat);
#endif  /* notdef */
	
	switch (cf_stat) {
		case RPC_PROGNOTREGISTERED:
		case RPC_AUTHERROR:
		case RPC_VERSMISMATCH:
		case RPC_PROGUNAVAIL:
		case RPC_PROGVERSMISMATCH:
		case RPC_PROCUNAVAIL:
			dbConnectionResults = TT_DB_ERR_DB_OPEN_FAILED;
		break;

		case RPC_CANTENCODEARGS:
		case RPC_CANTDECODEARGS:
		case RPC_CANTDECODERES:
#if defined(OPT_TLI)
		case RPC_UDERROR:
#endif
			dbConnectionResults = TT_DB_ERR_RPC_FAILED;
		break;

		case RPC_CANTRECV:
		case RPC_CANTSEND:
		case RPC_SYSTEMERROR:
		case RPC_TIMEDOUT:
#if defined(OPT_TLI)
		case RPC_INTR:
		case RPC_TLIERROR:
		case RPC_UNKNOWNADDR:
#endif
		case RPC_UNKNOWNHOST:
		case RPC_UNKNOWNPROTO:
			dbConnectionResults = TT_DB_ERR_RPC_CONNECTION_FAILED;
		break;

		default:
			dbConnectionResults = TT_DB_ERR_DB_CONNECTION_FAILED;
		break;
	}
}	// end -SetError()-

void _Tt_db_client::setTtDBDefaults ()
{
	dbAccess = new _Tt_db_access;
	dbAuthLevel = AUTH_NONE;
	dbConnectionResults = TT_DB_OK;
	dbServer = (CLIENT *)NULL;
	
	dbVersion = 2; // Initially assume version 2.  If one of the new
		// RPC calls cannot be found (RPC_PROCUNAVAIL),
		// then downgrade the version to 1.  This is only
			// required for old DB server compatibility.
}

_Tt_db_client::~_Tt_db_client ()
{
	if (dbServer) {
		if (dbServer->cl_auth) {
			auth_destroy(dbServer->cl_auth);
		}
		clnt_destroy (dbServer);
	}
}

void _Tt_db_client::setCurrentAccess (const _Tt_db_access_ptr &access)
{
	dbAccess = access;
}

_Tt_db_access_ptr _Tt_db_client::getCurrentAccess () const
{
	return dbAccess;
}

_Tt_db_results
_Tt_db_client::getFilePartition (const _Tt_string &file_path,
				 _Tt_string       &partition,
				 _Tt_string	  &network_path)
{
	_Tt_db_results retval;	
	char  *temp_path = (char *)file_path;
	char **temp_path_ptr = &temp_path;
	
	createAuth();
	
	_tt_file_partition_results *results =
		(dbVersion > 1 ?
		 _tt_get_file_partition_1(temp_path_ptr, dbServer) :
		 _tt_get_file_partition_1(temp_path_ptr, this));
	
	if (!results) {
		return (TT_DB_ERR_RPC_CONNECTION_FAILED);
	}
	
	partition = results->partition;
	network_path = results->network_path;
	retval = results->results;
	
	if (dbVersion==1) {
		if (0!=results->partition) {
			free(results->partition);
		}
		
		if (0!=results->network_path) {
			free(results->network_path);
		}
	} else {
		xdr_free((xdrproc_t)xdr_tt_file_partition_results, (char *)results);
	}
	
	return retval;
}

_Tt_db_results
_Tt_db_client::createFile (const _Tt_string               &file,
			   const _Tt_db_property_list_ptr &properties,
			   const _Tt_db_access_ptr	  &access,
			   int                            &cache_level)
{
	_tt_create_file_args args;
	
	args.file = (char *)file;
	_tt_set_rpc_properties(properties, args.properties);
	_tt_set_rpc_access(access, args.access);
	
	createAuth();
	_tt_db_cache_results *results =
		(dbVersion > 1 ?
		 _tt_create_file_1(&args, dbServer) :
		 _tt_create_file_1(&args, this));
	_tt_free_rpc_properties(args.properties);
	
	if (!results) {
		return (TT_DB_ERR_RPC_CONNECTION_FAILED);
	}
	
	cache_level = results->cache_level;
	return results->results;
}

_Tt_db_results
_Tt_db_client::createObject (const _Tt_string               &file,
			     const _Tt_string               &objid,
			     const _Tt_string               &otype,
			     const _Tt_db_property_list_ptr &properties,
			     const _Tt_db_access_ptr	    &access,
			     int                            &cache_level)
{
	_tt_create_obj_args args;
	
	args.file = (char *)file;
	args.objid = (char *)objid;
	args.otype = (char *)otype;
	_tt_set_rpc_properties (properties, args.properties);
	_tt_set_rpc_access(access, args.access);
	
	createAuth();
	_tt_db_cache_results *results =
		(dbVersion > 1 ?
		 _tt_create_obj_1(&args, dbServer) :
		 _tt_create_obj_1(&args, this));
	_tt_free_rpc_properties(args.properties);
	
	if (!results) {
		return (TT_DB_ERR_RPC_CONNECTION_FAILED);
	}
	
	cache_level = results->cache_level;
	return results->results;
}

_Tt_db_results
_Tt_db_client::removeFile (const _Tt_string &file)
{
	_tt_remove_file_args args;
	_tt_set_rpc_access(dbAccess, args.access);
	
	args.file = (char *)file;
	
	_tt_db_results *results =
		(dbVersion > 1 ?
		 _tt_remove_file_1(&args, dbServer) :
		 _tt_remove_file_1(&args, this));
	
	if (!results) {
		return (TT_DB_ERR_RPC_CONNECTION_FAILED);
	}
	return *results;
}

_Tt_db_results
_Tt_db_client::removeObject (const _Tt_string &objid,
			     const _Tt_string &forward_pointer)
{
	_tt_remove_obj_args args;
	_tt_set_rpc_access(dbAccess, args.access);
	
	args.objid = (char *)objid;
	args.forward_pointer = (forward_pointer.len() ?
				(char *)forward_pointer : (char *)NULL);
	
	createAuth();
	_tt_db_results *results =
		(dbVersion > 1 ?
		 _tt_remove_obj_1(&args, dbServer) :
		 _tt_remove_obj_1(&args, this));
	
	if (!results) {
		return (TT_DB_ERR_RPC_CONNECTION_FAILED);
	}
	return *results;
}

_Tt_db_results
_Tt_db_client::moveFile (const _Tt_string &file,
			 const _Tt_string &new_file)
{
	_tt_move_file_args args;
	_tt_set_rpc_access(dbAccess, args.access);
	
	args.file = (char *)file;
	args.new_file = (char *)new_file;
	
	_tt_db_results *results =
		(dbVersion > 1 ?
		 _tt_move_file_1(&args, dbServer) :
		 _tt_move_file_1(&args, this));
	
	if (!results) {
		return (TT_DB_ERR_RPC_CONNECTION_FAILED);
	}
	return *results;
}

_Tt_db_results
_Tt_db_client::setFileProperty (const _Tt_string          &file,
				const _Tt_db_property_ptr &property,
				int                       &cache_level)
{
	_tt_set_file_prop_args args;
	
	args.file = (char *)file;
	_tt_set_rpc_property(property, args.property);
	_tt_set_rpc_access(dbAccess, args.access);
	
	createAuth();
	_tt_db_cache_results *results =
		(dbVersion > 1 ?
		 _tt_set_file_prop_1(&args, dbServer) :
		 _tt_set_file_prop_1(&args, this));
	_tt_free_rpc_property(args.property);
	
	if (!results) {
		return (TT_DB_ERR_RPC_CONNECTION_FAILED);
	}
	
	cache_level = results->cache_level;
	return results->results;
}

_Tt_db_results _Tt_db_client
::setFileProperties (const _Tt_string               &file,
		     const _Tt_db_property_list_ptr &properties,
		     int                            &cache_level)
{
	_tt_set_file_props_args args;
	
	args.file = (char *)file;
	_tt_set_rpc_properties (properties, args.properties);
	_tt_set_rpc_access(dbAccess, args.access);
	
	createAuth();
	_tt_db_cache_results *results =
		(dbVersion > 1 ?
		 _tt_set_file_props_1(&args, dbServer) :
		 _tt_set_file_props_1(&args, this));
	_tt_free_rpc_properties(args.properties);
	
	if (!results) {
		return (TT_DB_ERR_RPC_CONNECTION_FAILED);
	}
	
	cache_level = results->cache_level;
	return results->results;
}

_Tt_db_results
_Tt_db_client::addFileProperty (const _Tt_string          &file,
				const _Tt_db_property_ptr &property,
				bool_t                     unique,
				int                       &cache_level)
{
	_tt_add_file_prop_args args;
	
	args.file = (char *)file;
	_tt_set_rpc_property(property, args.property);
	args.unique = (int)unique;
	_tt_set_rpc_access(dbAccess, args.access);
	
	createAuth();
	
	_tt_db_cache_results *results;
	if (dbVersion > 1) {
		results = _tt_add_file_prop_1(&args, dbServer);
	}
	else {
		if (property->name == TT_DB_SESSION_PROPERTY) {
			static _tt_db_cache_results rpc_results;
			
			results = &rpc_results;
			results->cache_level = cache_level+1;
			results->results = addsession(file, (*property->values)[0]);
		}
		else {
			results = _tt_add_file_prop_1(&args, this);
		}
	}
	_tt_free_rpc_property(args.property);
	
	if (!results) {
		return (TT_DB_ERR_RPC_CONNECTION_FAILED);
	}
	
	cache_level = results->cache_level;
	return results->results;
}

_Tt_db_results
_Tt_db_client::deleteFileProperty (const _Tt_string          &file,
				   const _Tt_db_property_ptr &property,
				   int                       &cache_level)
{
	_tt_del_file_prop_args args;
	
	args.file = (char *)file;
	_tt_set_rpc_property(property, args.property);
	_tt_set_rpc_access(dbAccess, args.access);
	
	createAuth();
	_tt_db_cache_results *results;
	if (dbVersion > 1) {
		results = _tt_delete_file_prop_1(&args, dbServer);
	}
	else {
		if (property->name == TT_DB_SESSION_PROPERTY) {
			static _tt_db_cache_results rpc_results;
			
			results = &rpc_results;
			results->cache_level = cache_level+1;
			results->results = delsession(file, (*property->values)[0]);
		}
		else {
			results = _tt_delete_file_prop_1(&args, this);
		}
	}
	_tt_free_rpc_property(args.property);
	
	if (!results) {
		return (TT_DB_ERR_RPC_CONNECTION_FAILED);
	}
	
	cache_level = results->cache_level;
	return results->results;
}

_Tt_db_results
_Tt_db_client::getFileProperty (const _Tt_string    &file,
				const _Tt_string    &name,
				int                 &cache_level,
				_Tt_db_property_ptr &property)
{
	_Tt_db_results retval;	
	_tt_get_file_prop_args args;
	
	args.file = (char *)file;
	args.name = (char *)name;
	_tt_set_rpc_access(dbAccess, args.access);
	args.cache_level = cache_level;
	
	createAuth();
	_tt_file_prop_results *results =
		(dbVersion > 1 ?
		 _tt_get_file_prop_1(&args, dbServer) :
		 _tt_get_file_prop_1(&args, this));
	
	if (!results) {
		return (TT_DB_ERR_RPC_CONNECTION_FAILED);
	}
	
	cache_level = results->cache_level;
	_tt_get_rpc_property(results->property, property);
	retval = results->results;
	if (dbVersion==1) {
		_tt_free_rpc_property(results->property);
	} else {
		xdr_free((xdrproc_t)xdr_tt_file_prop_results, (char *)results);
	}
	return retval;
}

_Tt_db_results _Tt_db_client::
getFileProperties (const _Tt_string         &file,
		   int                      &cache_level,
		   _Tt_db_property_list_ptr &properties)
{
	_Tt_db_results retval;	
	_tt_get_file_props_args args;
	
	args.file = (char *)file;
	_tt_set_rpc_access(dbAccess, args.access);
	args.cache_level = cache_level;
	
	createAuth();
	_tt_file_props_results *results =
		(dbVersion > 1 ?
		 _tt_get_file_props_1(&args, dbServer) :
		 _tt_get_file_props_1(&args, this));
	
	if (!results) {
		return (TT_DB_ERR_RPC_CONNECTION_FAILED);
	}
	
	cache_level = results->cache_level;
	retval = results->results;
	_tt_get_rpc_properties(results->properties, properties);
	if (dbVersion==1) {
		_tt_free_rpc_properties(results->properties);
	} else {
		xdr_free((xdrproc_t)xdr_tt_file_props_results, (char *)results);
	}  
	return retval;
}

_Tt_db_results
_Tt_db_client::getFileObjects (const _Tt_string    &file,
			       int                 &cache_level,
			       _Tt_string_list_ptr &objids)
{
	_Tt_db_results retval;	
	_tt_get_file_objs_args args;
	
	args.file = (char *)file;
	_tt_set_rpc_access(dbAccess, args.access);
	args.cache_level = cache_level;
	
	createAuth();
	_tt_file_objs_results *results =
		(dbVersion > 1 ?
		 _tt_get_file_objs_1(&args, dbServer) :
		 _tt_get_file_objs_1(&args, this));
	
	if (!results) {
		return (TT_DB_ERR_RPC_CONNECTION_FAILED);
	}
	
	cache_level = results->cache_level;
	retval = results->results;
	_tt_get_rpc_strings(results->objids, objids);
	if (dbVersion==1) {
		_tt_free_rpc_strings(results->objids);
	} else {
		xdr_free((xdrproc_t)xdr_tt_file_objs_results, (char *)results);
	}
	return retval;
}

_Tt_db_results
_Tt_db_client::setFileAccess (const _Tt_string        &file,
			      const _Tt_db_access_ptr &access)
{
	_tt_set_file_access_args args;
	
	args.file = (char *)file;
	_tt_set_rpc_access(access, args.new_access);
	_tt_set_rpc_access(dbAccess, args.access);
	
	createAuth();
	_tt_db_results *results =
		(dbVersion > 1 ?
		 _tt_set_file_access_1(&args, dbServer) :
		 _tt_set_file_access_1(&args, this));
	
	if (!results) {
		return (TT_DB_ERR_RPC_CONNECTION_FAILED);
	}
	return *results;
}

_Tt_db_results
_Tt_db_client::getFileAccess (const _Tt_string  &file,
			      _Tt_db_access_ptr &access)
{
	_tt_get_file_access_args args;
	
	args.file = (char *)file;
	_tt_set_rpc_access(dbAccess, args.access);
	
	createAuth();
	_tt_file_access_results *results =
		(dbVersion > 1 ?
		 _tt_get_file_access_1(&args, dbServer) :
		 _tt_get_file_access_1(&args, this));
	
	if (!results) {
		return (TT_DB_ERR_RPC_CONNECTION_FAILED);
	}
	
	_tt_get_rpc_access(results->access, access);
	return results->results;
}

_Tt_db_results
_Tt_db_client::setObjectProperty (const _Tt_string          &objid,
				  const _Tt_db_property_ptr &property,
				  _Tt_db_property_list_ptr  &properties,
				  int                       &cache_level)
{
	_Tt_db_results retval;	
	_tt_set_obj_prop_args args;
	
	args.objid = (char *)objid;
	_tt_set_rpc_property(property, args.property);
	_tt_set_rpc_access(dbAccess, args.access);
	args.cache_level = cache_level;
	
	createAuth();
	_tt_obj_props_results *results =
		(dbVersion > 1 ?
		 _tt_set_obj_prop_1(&args, dbServer) :
		 _tt_set_obj_prop_1(&args, this));
	_tt_free_rpc_property(args.property);
	
	if (!results) {
		return (TT_DB_ERR_RPC_CONNECTION_FAILED);
	}
	
	cache_level = results->cache_level;
	retval = results->results;
	_tt_get_rpc_properties(results->properties, properties);
	if (dbVersion==1) {
		_tt_free_rpc_properties(results->properties);
	} else {
		xdr_free((xdrproc_t)xdr_tt_obj_props_results, (char *)results);
	}
	return retval;
}

_Tt_db_results _Tt_db_client::
setObjectProperties (const _Tt_string               &objid,
		     const _Tt_db_property_list_ptr &in_properties,
		     _Tt_db_property_list_ptr       &out_properties,
		     int                            &cache_level)
{
	_Tt_db_results retval;	
	_tt_set_obj_props_args args;
	
	args.objid = (char *)objid;
	_tt_set_rpc_properties (in_properties, args.properties);
	_tt_set_rpc_access(dbAccess, args.access);
	args.cache_level = cache_level;
	
	createAuth();
	_tt_obj_props_results *results =
		(dbVersion > 1 ?
		 _tt_set_obj_props_1(&args, dbServer) :
		 _tt_set_obj_props_1(&args, this));
	_tt_free_rpc_properties(args.properties);
	
	if (!results) {
		return (TT_DB_ERR_RPC_CONNECTION_FAILED);
	}
	
	cache_level = results->cache_level;
	retval = results->results;
	_tt_get_rpc_properties(results->properties, out_properties);
	if (dbVersion==1) {
		_tt_free_rpc_properties(results->properties);
	} else {
		xdr_free((xdrproc_t)xdr_tt_obj_props_results, (char *)results);
	}
	return retval;
}

_Tt_db_results
_Tt_db_client::addObjectProperty (const _Tt_string          &objid,
				  const _Tt_db_property_ptr &property,
				  bool_t                     unique,
				  _Tt_db_property_list_ptr  &properties,
				  int                       &cache_level)
{
	_Tt_db_results retval;	
	_tt_add_obj_prop_args args;
	
	args.objid = (char *)objid;
	_tt_set_rpc_property(property, args.property);
	args.unique = (int)unique;
	_tt_set_rpc_access(dbAccess, args.access);
	args.cache_level = cache_level;
	
	createAuth();
	_tt_obj_props_results *results =
		(dbVersion > 1 ?
		 _tt_add_obj_prop_1(&args, dbServer) :
		 _tt_add_obj_prop_1(&args, this));
	_tt_free_rpc_property(args.property);
	
	if (!results) {
		return (TT_DB_ERR_RPC_CONNECTION_FAILED);
	}
	
	cache_level = results->cache_level;
	retval = results->results;
	_tt_get_rpc_properties(results->properties, properties);
	if (dbVersion==1) {
		_tt_free_rpc_properties(results->properties);
	} else {
		xdr_free((xdrproc_t)xdr_tt_obj_props_results, (char *)results);
	}
	return retval;
}

_Tt_db_results _Tt_db_client::
deleteObjectProperty (const _Tt_string          &objid,
		      const _Tt_db_property_ptr &property,
		      _Tt_db_property_list_ptr  &properties,
		      int                       &cache_level)
{
	_Tt_db_results retval;	
	_tt_del_obj_prop_args args;
	
	args.objid = (char *)objid;
	_tt_set_rpc_property(property, args.property);
	_tt_set_rpc_access(dbAccess, args.access);
	args.cache_level = cache_level;
	
	createAuth();
	_tt_obj_props_results *results =
		(dbVersion > 1 ?
		 _tt_delete_obj_prop_1(&args, dbServer) :
		 _tt_delete_obj_prop_1(&args, this));
	_tt_free_rpc_property(args.property);
	
	if (!results) {
		return (TT_DB_ERR_RPC_CONNECTION_FAILED);
	}
	
	_tt_get_rpc_properties(results->properties, properties);
	cache_level = results->cache_level;
	retval = results->results;
	if (dbVersion==1) {
		_tt_free_rpc_properties(results->properties);
	} else {
		xdr_free((xdrproc_t)xdr_tt_obj_props_results, (char *)results);
	}
	return retval;
}

_Tt_db_results
_Tt_db_client::getObjectProperty (const _Tt_string    &objid,
				  const _Tt_string    &name,
				  int                 &cache_level,
				  _Tt_db_property_ptr &property)
{
	_Tt_db_results retval;	
	_tt_get_obj_prop_args args;
	
	args.objid = (char *)objid;
	args.name = (char *)name;
	_tt_set_rpc_access(dbAccess, args.access);
	args.cache_level = cache_level;
	
	createAuth();
	_tt_obj_prop_results *results =
		(dbVersion > 1 ?
		 _tt_get_obj_prop_1(&args, dbServer) :
		 _tt_get_obj_prop_1(&args, this));
	
	if (!results) {
		return (TT_DB_ERR_RPC_CONNECTION_FAILED);
	}
	
	cache_level = results->cache_level;
	retval = results->results;
	_tt_get_rpc_property(results->property, property);
	if (dbVersion==1) {
		_tt_free_rpc_property(results->property);
	} else {
		xdr_free((xdrproc_t)xdr_tt_obj_prop_results, (char *)results);
	}
	return retval;
}

_Tt_db_results _Tt_db_client::
getObjectProperties (const _Tt_string         &objid,
		     int                      &cache_level,
		     _Tt_db_property_list_ptr &properties)
{
	_Tt_db_results retval;	
	_tt_get_obj_props_args args;
	
	args.objid = (char *)objid;
	_tt_set_rpc_access(dbAccess, args.access);
	args.cache_level = cache_level;
	
	createAuth();
	_tt_obj_props_results *results =
		(dbVersion > 1 ?
		 _tt_get_obj_props_1(&args, dbServer) :
		 _tt_get_obj_props_1(&args, this));
	
	if (!results) {
		return (TT_DB_ERR_RPC_CONNECTION_FAILED);
	}
	
	cache_level = results->cache_level;
	retval = results->results;
	_tt_get_rpc_properties(results->properties, properties);
	if (dbVersion==1) {
		_tt_free_rpc_properties(results->properties);
	} else {
		xdr_free((xdrproc_t)xdr_tt_obj_props_results, (char *)results);
	}
	return retval;
}

_Tt_db_results _Tt_db_client::setObjectType (const _Tt_string &objid,
					     const _Tt_string &otype)
{
	_tt_set_obj_type_args args;
	
	args.objid = (char *)objid;
	args.otype = (char *)otype;
	_tt_set_rpc_access(dbAccess, args.access);
	
	createAuth();
	_tt_db_results *results =
		(dbVersion > 1 ?
		 _tt_set_obj_type_1(&args, dbServer) :
		 _tt_set_obj_type_1(&args, this));
	
	if (!results) {
		return (TT_DB_ERR_RPC_CONNECTION_FAILED);
	}
	return *results;
}

_Tt_db_results _Tt_db_client::getObjectType (const _Tt_string &objid,
					     _Tt_string       &otype)
{
	_Tt_db_results retval;	
	_tt_get_obj_type_args args;
	
	args.objid = (char *)objid;
	_tt_set_rpc_access(dbAccess, args.access);
	
	createAuth();
	_tt_obj_type_results *results;
	if (dbVersion > 1) {
		results = _tt_get_obj_type_1(&args, dbServer);
		if (!results) {
			return (TT_DB_ERR_RPC_CONNECTION_FAILED);
		}
		retval = results->results;
		otype = results->otype;
		xdr_free((xdrproc_t)xdr_tt_obj_type_results, (char *)results);
	} else {
		retval = gettype(objid, otype);
	}
	return retval;
}

_Tt_db_results _Tt_db_client::setObjectFile (const _Tt_string &objid,
					     const _Tt_string &file)
{
	_tt_set_obj_file_args args;
	
	args.objid = (char *)objid;
	args.file = (char *)file;
	_tt_set_rpc_access(dbAccess, args.access);
	
	createAuth();
	_tt_db_results *results =
		(dbVersion > 1 ?
		 _tt_set_obj_file_1(&args, dbServer) :
		 _tt_set_obj_file_1(&args, this));
	
	if (!results) {
		return (TT_DB_ERR_RPC_CONNECTION_FAILED);
	}
	return *results;
}


_Tt_db_results _Tt_db_client::getObjectFile (const _Tt_string &objid,
					     _Tt_string       &file)
{
	_Tt_db_results retval;	
	_tt_get_obj_file_args args;
	
	args.objid = (char *)objid;
	_tt_set_rpc_access(dbAccess, args.access);
	
	createAuth();
	_tt_obj_file_results *results =
		(dbVersion > 1 ?
		 _tt_get_obj_file_1(&args, dbServer) :
		 _tt_get_obj_file_1(&args, this));
	
	if (!results) {
		return (TT_DB_ERR_RPC_CONNECTION_FAILED);
	}
	
	retval = results->results;
	file = results->file;
	if (dbVersion==1) {
		if (0!=results->file) {
			free(results->file);
		}
	} else {
		xdr_free((xdrproc_t)xdr_tt_obj_file_results, (char *)results);
	}
	return retval;
}

_Tt_db_results
_Tt_db_client::setObjectAccess (const _Tt_string        &objid,
				const _Tt_db_access_ptr &access)
{
	_tt_set_obj_access_args args;
	
	args.objid = (char *)objid;
	_tt_set_rpc_access(access, args.new_access);
	_tt_set_rpc_access(dbAccess, args.access);
	
	createAuth();
	_tt_db_results *results =
		(dbVersion > 1 ?
		 _tt_set_obj_access_1(&args, dbServer) :
		 _tt_set_obj_access_1(&args, this));
	
	if (!results) {
		return (TT_DB_ERR_RPC_CONNECTION_FAILED);
	}
	return *results;
}

_Tt_db_results
_Tt_db_client::getObjectAccess (const _Tt_string  &objid,
				_Tt_db_access_ptr &access)
{
	_tt_get_obj_access_args args;
	
	args.objid = (char *)objid;
	_tt_set_rpc_access(dbAccess, args.access);
	
	createAuth();
	_tt_obj_access_results *results =
		(dbVersion > 1 ?
		 _tt_get_obj_access_1(&args, dbServer) :
		 _tt_get_obj_access_1(&args, this));
	
	if (!results) {
		return (TT_DB_ERR_RPC_CONNECTION_FAILED);
	}
	
	_tt_get_rpc_access(results->access, access);
	return results->results;
}

_Tt_db_results _Tt_db_client::
isFileInDatabase (const _Tt_string &file,
		  bool_t           &directory_flag)
{
	_tt_is_file_in_db_args args;
	
	args.file = (char *)file;
	_tt_set_rpc_access(dbAccess, args.access);
	
	createAuth();
	_tt_is_file_in_db_results *results =
		(dbVersion > 1 ?
		 _tt_is_file_in_db_1(&args, dbServer) :
		 _tt_is_file_in_db_1(&args, this));
	
	if (!results) {
		return (TT_DB_ERR_RPC_CONNECTION_FAILED);
	}
	
	directory_flag = results->directory_flag;
	return results->results;
}

_Tt_db_results _Tt_db_client::
isObjectInDatabase (const _Tt_string &objid,
		    _Tt_string       &forward_pointer)
{
	_Tt_db_results retval;	
	_tt_is_obj_in_db_args args;
	
	args.objid = (char *)objid;
	_tt_set_rpc_access(dbAccess, args.access);
	
	createAuth();
	_tt_is_obj_in_db_results *results =
		(dbVersion > 1 ?
		 _tt_is_obj_in_db_1(&args, dbServer) :
		 _tt_is_obj_in_db_1(&args, this));
	
	if (!results) {
		return (TT_DB_ERR_RPC_CONNECTION_FAILED);
	}
	
	forward_pointer = results->forward_pointer;
	retval = results->results;
	if (dbVersion==1) {
		if (0!=results->forward_pointer) {
			free(results->forward_pointer);
		}
	} else {
		xdr_free((xdrproc_t)xdr_tt_is_obj_in_db_results, (char *)results);
	}  
	return retval;
}

_Tt_db_results
_Tt_db_client::queueMessage (const _Tt_string          &file,
			     const _Tt_string_list_ptr &ptypes,
			     const _Tt_message_ptr     &message)
{
	_tt_queue_msg_args args;
	
	args.file = (char *)file;
	_tt_set_rpc_strings(ptypes, args.ptypes);
	if (_tt_set_rpc_message(message, args.message) != TT_DB_OK) {
		return (TT_DB_ERR_ILLEGAL_MESSAGE);
	}
	
	createAuth();
	_tt_db_results *results =
		(dbVersion > 1 ?
		 _tt_queue_message_1(&args, dbServer) :
		 _tt_queue_message_1(&args, this));
	_tt_free_rpc_strings(args.ptypes);
	_tt_free_rpc_message(args.message);
	
	if (!results) {
		return (TT_DB_ERR_RPC_CONNECTION_FAILED);
	}
	return *results;
}

_Tt_db_results
_Tt_db_client::dequeueMessages (const _Tt_string          &file,
				const _Tt_string_list_ptr &ptypes,
				_Tt_message_list_ptr      &messages)
{
	_tt_dequeue_msgs_args args;
	
	args.file = (char *)file;
	_tt_set_rpc_strings(ptypes, args.ptypes);
	
	createAuth();
	_tt_dequeue_msgs_results *results =
		(dbVersion > 1 ?
		 _tt_dequeue_messages_1(&args, dbServer) :
		 _tt_dequeue_messages_1(&args, this));
	_tt_free_rpc_strings(args.ptypes);
	
	if (!results) {
		return (TT_DB_ERR_RPC_CONNECTION_FAILED);
	}
	
	_tt_get_rpc_messages(results->messages, messages);
	if (dbVersion==1) {
		_tt_free_rpc_messages(results->messages);	
	} else {
		xdr_free((xdrproc_t)xdr_tt_dequeue_msgs_results, (char *)results);
	}  
	return results->results;
}

void _Tt_db_client::createAuth ()
{
#ifdef OPT_SECURE_RPC
	if (dbAuthLevel == AUTH_DES) {
		if (dbServer->cl_auth) {
			auth_destroy(dbServer->cl_auth);
		}
		const int CRED_EXPIRE = 10;
#ifdef OPT_TLI
		dbServer->cl_auth = authdes_seccreate((char *)dbServerNetName,
						      CRED_EXPIRE,
						      (char *)dbServerNetName,
						      0);
#else
		dbServer->cl_auth = authdes_create((char *)dbServerNetName,
						   CRED_EXPIRE,
						   &dbSocket,
						   0);
#endif
	}
#endif				// OPT_SECURE_RPC
}


// Gets the remote ends' idea of what the netfile version of the specified
// file is.
_Tt_db_results _Tt_db_client ::
file_netfile(const _Tt_string &file, _Tt_string &netfile)
{
	_Tt_db_results retval;
	_tt_file_netfile_args args;
	
	
	args.file_or_netfile = (char *)file;
	
	createAuth();
	
	_tt_file_netfile_results *results = _tt_file_netfile_1(&args, dbServer);
	
	//XXX - this needs to detect that we tried contacting an old dbserver
		//	so we can return TT_ERR_UNIMP
			if (!results) {
				return (TT_DB_ERR_RPC_CONNECTION_FAILED);
			}
	
	// prepare what we're passing back to the caller.
		retval = results->results;
	netfile = results->result_string;
	
	xdr_free((xdrproc_t)xdr_tt_file_netfile_results, (char *)results);
	return retval;
}


// Gets the remote ends' idea of what the file version of the specified
// netfile is.
_Tt_db_results _Tt_db_client ::
netfile_file(const _Tt_string &netfile, _Tt_string &file)
{
	_Tt_db_results retval;
	_tt_file_netfile_args args;
	
	args.file_or_netfile = (char *)netfile;
	
	createAuth();
	
	_tt_file_netfile_results *results = _tt_netfile_file_1(&args, dbServer);
	
	// XXX - this needs to detect that we tried contacting an old dbserver
	//	so we can return TT_ERR_UNIMP
	if (!results) {
		return (TT_DB_ERR_RPC_CONNECTION_FAILED);
	}
	
	// prepare what we're passing back to the caller.
		retval = results->results;
	file = results->result_string;
	
	xdr_free((xdrproc_t)xdr_tt_file_netfile_results, (char *)results);
	return retval;
}


// ********** Old DB Server Compatibility Prototcol **********
//
// The following member functions are used to provide compatibility
// with old DB servers.  All of these member functions map directly
// to an old DB server RPC call.  The names of these member functions
// have been kept the same as the base name of the corresponding
// RPC call.

#include "db/tt_old_db_consts.h"

_Tt_string _Tt_db_client::mfs(const _Tt_string &path)
{
	_Tt_string retval;
	char  *char_path = (char *)path;
	char **char_partition = _tt_mfs_1(&char_path, dbServer);
	
	if (char_partition) {
		retval = *char_partition;
		xdr_free((xdrproc_t)xdr_wrapstring, (char *)char_partition);
		return retval;
	} else {
		// Theoretically "should not happen".  If it does just use
			// the root partition which is our general fallback...
				return _Tt_string("/");
	}
	
}

_Tt_db_results _Tt_db_client::addsession(const _Tt_string &file,
					 const _Tt_string &session)
{
	_Tt_isam_results *res;
	_Tt_session_args args;
	
	_Tt_string temp_string;
	_Tt_string path = file;
	path = path.split(':', temp_string);
	
	_Tt_string partition = mfs(path);
	_Tt_old_db_partition_map_ref map;
	_Tt_old_db_ptr old_db = map.getDB(partition, this);
	
	_Tt_string key;
	_Tt_db_results results = old_db->getFileKey(file, key);
	
	if (results == TT_DB_OK) {
		args.isfd = old_db->propertyTableFD;
		args.key = old_db->propertyTablePropertyKey->getKeyDescriptor();
		args.key_len = TT_OLD_DB_KEY_LENGTH + TT_OLD_DB_MAX_PROPERTY_NAME_LENGTH;
		args.oidkey.oidkey_len = TT_OLD_DB_KEY_LENGTH;
		args.oidkey.oidkey_val = (char *)key;
		args.session.session_len = session.len();
		args.session.session_val = (char *)session;
		res = _tt_addsession_1(&args, dbServer);
		return oldDBToNewDBResults((_Tt_old_db_results)res->result);
	}
	
	return results;
}

_Tt_db_results _Tt_db_client::delsession(const _Tt_string &file,
					 const _Tt_string &session)
{
	_Tt_isam_results *res;
	_Tt_session_args args;
	
	_Tt_string temp_string;
	_Tt_string path = file;
	path = path.split(':', temp_string);
	
	_Tt_string partition = mfs(path);
	_Tt_old_db_partition_map_ref map;
	_Tt_old_db_ptr old_db = map.getDB(partition, this);
	
	_Tt_string key;
	_Tt_db_results results = old_db->getFileKey(file, key);
	
	if (results == TT_DB_OK) {
		args.isfd = old_db->propertyTableFD;
		args.key = old_db->propertyTablePropertyKey->getKeyDescriptor();
		args.key_len = TT_OLD_DB_KEY_LENGTH + TT_OLD_DB_MAX_PROPERTY_NAME_LENGTH;
		args.oidkey.oidkey_len = TT_OLD_DB_KEY_LENGTH;
		args.oidkey.oidkey_val = (char *)key;
		args.session.session_len = session.len();
		args.session.session_val = (char *)session;
		res = _tt_delsession_1(&args, dbServer);
		if ((res->result == -1) && (res->iserrno == ENOREC)) { // XXX where else?
									       return TT_DB_ERR_NO_SUCH_PROPERTY;
							       }
		return oldDBToNewDBResults((_Tt_old_db_results)res->result);
	}
	
	return results;
}

_Tt_db_results _Tt_db_client::gettype(const _Tt_string &objid,
				      _Tt_string       &type)
{
	_Tt_spec_props *res;
	_Tt_spec_props args;
	_Tt_prop props, *propp;
	_Tt_db_results retval;
	
	_Tt_string partition = objid;
	_Tt_string temp_string;
	
	// Get rid of file system type and hostname - the partition
		// is the only thing left
			partition = partition.split(':', temp_string);
	partition = partition.split(':', temp_string);
	partition = partition.split(':', temp_string);
	
	_Tt_old_db_partition_map_ref map;
	_Tt_old_db_ptr old_db = map.getDB(partition, this);
	_Tt_db_key object_key(objid);
	
	args.isfd = old_db->propertyTableFD;
	args.key = old_db->propertyTablePropertyKey->getKeyDescriptor();
	args.key_len = TT_OLD_DB_KEY_LENGTH + TT_OLD_DB_MAX_PROPERTY_NAME_LENGTH;
	args.oidkey.oidkey_len = TT_OLD_DB_KEY_LENGTH;
	args.oidkey.oidkey_val = (char *)object_key.binary();
	args.props.props_len = 1;
	args.props.props_val = &props;
	props.propname.propname_val = TT_OLD_DB_OBJECT_TYPE_PROPERTY;
	props.propname.propname_len = strlen(TT_OLD_DB_OBJECT_TYPE_PROPERTY);
	props.recnum = 0;
	props.value.value_len = 0;
	props.value.value_val = 0;
	res = _tt_gettype_1(&args, dbServer);
	if (res->result == TT_DB_OK) {
		propp = &(res->props.props_val[0]);
		type.set((unsigned char *)propp->value.value_val,
			 propp->value.value_len);
	}
	retval = oldDBToNewDBResults((_Tt_old_db_results)res->result);
	xdr_free((xdrproc_t)xdr_Tt_spec_props, (char *)res);
	return retval;
}

int _Tt_db_client::
isaddindex(int isfd, struct keydesc *key)
{
	createAuth();
	_Tt_isaddindex_args args;
	args.isfd = isfd;
	args.key = key;
	_Tt_isam_results *isresult = _tt_isaddindex_1(&args, dbServer);
	if (!isresult) {
		iserrno = ERPC;
		return -1;
	}
	iserrno = isresult->iserrno;
	return isresult->result;
}

int _Tt_db_client::
isbuild(char *path, int reclen, struct keydesc *key, int mode)
{
	createAuth();
	_Tt_isbuild_args args;
	args.path = path;
	args.reclen = reclen;
	args.key = key;
	args.mode = mode;
	args.isreclen = isreclen;
	_Tt_isam_results *isresult = _tt_isbuild_1(&args, dbServer);
	if (!isresult) {
		iserrno = ERPC;
		return -1;
	}
	iserrno = isresult->iserrno;
	return isresult->result;
}

int _Tt_db_client::
isclose(int isfd)
{
	createAuth();
	_Tt_isam_results *isresult = _tt_isclose_1(&isfd, dbServer);
	if (!isresult) {
		iserrno = ERPC;
		return -1;
	}
	iserrno = isresult->iserrno;
	return isresult->result;
}

int _Tt_db_client::
iscntl(int isfd, int func, char *arg)
{
	createAuth();
	_Tt_iscntl_args args;
	args.isfd = isfd;
	args.func = func;
	args.arg.arg_len = ISAPPLMAGICLEN;
	args.arg.arg_val = arg;
	_Tt_iscntl_results *isresult = _tt_iscntl_1(&args, dbServer);
	if (!isresult) {
		iserrno = ERPC;
		return -1;
	}
	iserrno = isresult->iserrno;
	memcpy(arg, isresult->arg.arg_val, isresult->arg.arg_len);
	if (isresult->arg.arg_val != 0) {
		xdr_free((xdrproc_t)xdr_Tt_iscntl_results, (char *)isresult);
	}
	return isresult->result;
}

int _Tt_db_client::
isdelrec(int isfd, long recnum, char *rec)
{
	createAuth();
	_Tt_isdelrec_args args;
	args.isfd = isfd;
	args.recnum = recnum;
	args.rec.rec_len = isreclen;
	args.rec.rec_val = rec;
	_Tt_isam_results *isresult = _tt_isdelrec_1(&args, dbServer);
	if (!isresult) {
		iserrno = ERPC;
		return -1;
	}
	iserrno = isresult->iserrno;
	return isresult->result;
}

int _Tt_db_client::
iserase(char *path)
{
	createAuth();
	_Tt_isam_results *isresult = _tt_iserase_1(&path, dbServer);
	if (!isresult) {
		iserrno = ERPC;
		return -1;
	}
	iserrno = isresult->iserrno;
	return isresult->result;
}

int _Tt_db_client::
isopen(char *path, int mode)
{
	createAuth();
	_Tt_isopen_args args;
	args.path = path;
	args.mode = mode;
	_Tt_isam_results *isresult = _tt_isopen_1(&args, dbServer);
	if (!isresult) {
		iserrno = ERPC;
		return -1;
	}
	iserrno = isresult->iserrno;
	return isresult->result;
}

int _Tt_db_client::
isread(int isfd, char *rec, int mode)
{
	createAuth();
	_Tt_isread_args args;
	args.isfd = isfd;
	args.rec.rec_len = isreclen;
	args.rec.rec_val = rec;
	args.mode = mode;
	args.isrecnum = -1;
	_Tt_isread_results *isresult = _tt_isread_1(&args, dbServer);
	if (!isresult) {
		iserrno = ERPC;
		return -1;
	}
	isreclen = isresult->isreclen;
	isrecnum = isresult->isrecnum;
	iserrno = isresult->isresult.iserrno;
	if (isresult->isresult.result != -1) {
		memcpy(rec, isresult->rec.rec_val, isresult->isreclen);
	}
	if (isresult->rec.rec_val != 0) {
		xdr_free((xdrproc_t)xdr_Tt_isread_results, (char *)isresult);
	}
	return isresult->isresult.result;
}

int _Tt_db_client::
isrewrec(int isfd, long recnum, char *rec)
{
	createAuth();
	_Tt_isrewrec_args args;
	args.isfd = isfd;
	args.recnum = recnum;
	args.rec.rec_len = isreclen;
	args.rec.rec_val = rec;
	_Tt_isam_results *isresult = _tt_isrewrec_1(&args, dbServer);
	if (!isresult) {
		iserrno = ERPC;
		return -1;
	}
	iserrno = isresult->iserrno;
	return isresult->result;
}

int _Tt_db_client::
isstart(int isfd, keydesc *key, int key_len, char *rec, int mode)
{
	createAuth();
	_Tt_isstart_args args;
	args.isfd = isfd;
	args.key = key;
	args.key_len = key_len;
	args.rec.rec_len = rec ? isreclen : 0;
	args.rec.rec_val = rec ? rec : 0;
	args.mode = mode;
	_Tt_isam_results *isresult = _tt_isstart_1(&args, dbServer);
	if (!isresult) {
		iserrno = ERPC;
		return -1;
	}
	iserrno = isresult->iserrno;
	return isresult->result;
}

int _Tt_db_client::
iswrite(int isfd, char *rec)
{
	createAuth();
	_Tt_iswrite_args args;
	args.isfd = isfd;
	args.rec.rec_len = isreclen;
	args.rec.rec_val = rec;
	_Tt_isam_results *isresult = _tt_iswrite_1(&args, dbServer);
	if (!isresult) {
		iserrno = ERPC;
		return -1;
	}
	iserrno = isresult->iserrno;
	return isresult->result;
}

_Tt_db_results _Tt_db_client::
oldDBToNewDBResults (_Tt_old_db_results old_db_results)
{
	switch (old_db_results) {
	    case TT_OLD_DB_OK:
		return TT_DB_OK;
		
	    case TT_OLD_DB_ERROR:
	    case TT_OLD_DB_INIT_FAILED:
		return TT_DB_ERR_RPC_CONNECTION_FAILED;
		
	    case TT_OLD_DB_DBDESC_EXISTS:
	    case TT_OLD_DB_DB_EXISTS:
	    case TT_OLD_DB_CREATE_FAILED:
		return TT_DB_ERR_CORRUPT_DB;
		
	    case TT_OLD_DB_OPEN_FAILED:
		return TT_DB_ERR_DB_OPEN_FAILED;
		
	    case TT_OLD_DB_NO_RECORD:
	    case TT_OLD_DB_READ_FAILED:
	    case TT_OLD_DB_WRITE_FAILED:
	    case TT_OLD_DB_DELETE_FAILED:
	    case TT_OLD_DB_CLOSE_FAILED:
		return TT_DB_ERR_CORRUPT_DB;
		
	    case TT_OLD_DB_UNKNOWN_DBTABLE:
	    case TT_OLD_DB_UNKNOWN_INDEX:
	    case TT_OLD_DB_INVALID_VERSION_NUMBER:
	    case TT_OLD_DB_PATHMAP_FAILED:
	    case TT_OLD_DB_UPDATE_MFS_INFO_FAILED:
	    case TT_OLD_DB_CLEAR_LOCKS_FAILED:
		return TT_DB_ERR_CORRUPT_DB;
		
	    case TT_OLD_DB_RECORD_LOCKED:
		return TT_DB_ERR_DB_LOCKED;
		
	    case TT_OLD_DB_NO_MFS:
	    case TT_OLD_DB_UNKNOWN_FS:
	    case TT_OLD_DB_CONVERSION_ERROR:
	    case TT_OLD_DB_RECORD_SET:
		return TT_DB_ERR_CORRUPT_DB;
		
	    case TT_OLD_DB_ACCESS_DENIED:
		return TT_DB_ERR_ACCESS_DENIED;
		
	    default:
		break;
	}
	
	return TT_DB_ERR_CORRUPT_DB;
}

_Tt_string_list	*
_Tt_db_client::get_all_sessions()
{
  
	static	_Tt_string_list			* results = NULL;

	if (results != NULL) {
	  delete results;
	}
	results = new _Tt_string_list;

	_Tt_string				string;
	_tt_get_all_sessions_results		*xdr_results;
	_tt_get_all_sessions_args		args;

	u_int					offset;

	// Clear out any old data.
	memset(&args, '\0', sizeof(args));
	results->flush();

	do {
		xdr_results = _tt_get_all_sessions_1(&args, dbServer);
		if (xdr_results == NULL) {
			return(results);
		}
		for (offset=0; offset<xdr_results->session_list.values_len; offset++){
			string = xdr_results->session_list.values_val[offset].value;
			results->append(string);
		}

		xdr_free((xdrproc_t)xdr_tt_get_all_sessions_results,
			 (char *)xdr_results);

		// Copy over any continuation key.
		memcpy(&xdr_results->oidkey, &args.oidkey, sizeof(args.oidkey));
	} while(xdr_results->oidkey.oidkey_len > 0);

	return (results);
}

_Tt_db_results
_Tt_db_client::delete_session(_Tt_string session)
{
	_tt_delete_session_results	*res;
	_tt_delete_session_args		args;

	args.session.value = session;

	res = _tt_delete_session_1(&args, dbServer);
	if (!res) {
		return (TT_DB_ERR_RPC_CONNECTION_FAILED);
	}
	return TT_DB_OK;
}

_Tt_db_results
_Tt_db_client::garbage_collect_in_server()
{
	_tt_db_results results = TT_DB_OK;
	_tt_garbage_collect_results	*res;

	res = _tt_garbage_collect_1(NULL, dbServer);
	if (!res) {
		return (TT_DB_ERR_RPC_CONNECTION_FAILED);
	}
	return results;
}
