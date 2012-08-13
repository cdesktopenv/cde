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
//%%  $TOG: db_server_functions.C /main/6 1999/10/14 18:38:12 mgreess $ 			 				
/*
 * @(#)db_server_functions.C	1.35 95/06/07
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 *
 * This file contains the functions that connect the DB server RPC
 * interface to the DB server classes.
 *
 * As a function is implemented, it should commented
 * out of the db_server_stubs.cc file.
 */

#include <dirent.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <errno.h>
#include <stdint.h>
#include <unistd.h>
#include <nl_types.h>

#include "api/c/tt_c.h"
#include "util/tt_file_system.h"
#include "util/tt_file_system_entry.h"
#include "util/tt_path.h"
#include "util/tt_port.h"
#include "util/tt_xdr_utils.h"
#include "util/tt_gettext.h"
#include "db/db_server.h"
#include "db/tt_db_access.h"
#include "db/tt_db_property.h"
#include "db/tt_db_access_utils.h"
#include "db/tt_db_property_utils.h"
#include "db/tt_db_rpc_routines.h"
#include "db_server_globals.h"
#include "tt_db_message_info_utils.h"
#include "tt_db_partition_global_map_ref.h"
#include "tt_db_server_consts.h"
#include "tt_db_server_db_utils.h"
#include "dm_access_cache.h"
#include "dm/dm_recfmts.h"


#if !defined(OPT_GARBAGE_THREADS)
#include <db/tt_db_client.h>
#include <db/tt_db_client_utils.h>

char				**global_argv;
char				**global_envp;

#endif

//
// This is the PID or TID of the procedure that is
// performing the garbage collection.
//
#if defined(USL) || defined(__uxp__)
int		_tt_garbage_id = 0;	// TID or PID.
#else
int		_tt_garbage_id = -1;	// TID or PID.
#endif

int		_tt_run_garbage_collect(int in_parallel);

static	const char		* sesProp = _TT_FILEJOIN_PROPNAME;
static	const char		* modDate = _MP_NODE_MOD_PROP;
static	const char		* propTable = "property_table";

extern _Tt_db_info	_tt_db_table[_TT_MAX_ISFD];

static bool_t _tt_is_file_a_directory (const _Tt_string&);
static _Tt_string _tt_make_equivalent_object_id(const _Tt_string &objid,
						const _Tt_string &partition);
static _Tt_db_results _tt_get_partition_db (const _Tt_string&,
					    _Tt_db_server_db_ptr&);
static _Tt_db_access_ptr
       _tt_get_real_rpc_access (const _tt_access &rpc_access);
static _Tt_db_access_ptr _tt_get_file_access (const _Tt_string &file,
					      const _tt_access &rpc_access);
static _Tt_db_access_ptr _tt_get_unix_file_access (const _Tt_string &file);
static _Tt_string _tt_get_file_partition (const _Tt_string &file);
static _Tt_db_results
       _tt_increment_file_properties_cache_level (const _Tt_db_server_db_ptr&,
						  const _Tt_string&,
						  const _Tt_db_access_ptr&,
						  int&);
static _Tt_db_results
       _tt_get_file_properties_cache_level (const _Tt_db_server_db_ptr&,
					    const _Tt_string&,
					    const _Tt_db_access_ptr&,
					    int&);
static _Tt_string _tt_get_object_partition (const _Tt_string &objid);
static _Tt_db_results
_tt_increment_object_properties_cache_level (const _Tt_db_server_db_ptr&,
					     const _Tt_string&,
					     const _Tt_db_access_ptr&,
					     int&);
static _Tt_db_results
       _tt_get_object_properties_cache_level (const _Tt_db_server_db_ptr&,
					      const _Tt_string&,
					      const _Tt_db_access_ptr&,
					      int&);
static void _tt_screen_object_properties (_Tt_db_property_list_ptr&);

static _Tt_string _tt_get_local_path (const _Tt_string &network_path,
				      _Tt_string       &hostname,
				      _Tt_string       &partition);

_tt_auth_level_results *_tt_get_min_auth_level_1 (void * /* dummy_arg */,
						  SVCXPRT * /* transp */)
{
  static _tt_auth_level_results results;
  results.auth_level = _tt_auth_level;
  results.results = TT_DB_OK;
  return &results;
}

_tt_file_partition_results *_tt_get_file_partition_1 (char **file,
						      SVCXPRT * /* transp */)
{
  static _tt_file_partition_results results;

  _Tt_string hostname = _tt_gethostname();

  _Tt_string file_hostname;
  _Tt_string network_path = *file;
  _Tt_string partition;
  _Tt_string local_path = _tt_get_local_path (network_path,
				 	      file_hostname,
			                      partition);
  results.partition = strdup((char *)partition);

  if (file_hostname == hostname) {
    network_path = file_hostname.cat(":").cat(local_path);
  }
  results.network_path = strdup((char *)network_path);

  results.results = TT_DB_OK;

  return &results;
}

_tt_db_cache_results *_tt_create_file_1 (_tt_create_file_args *args,
					 SVCXPRT * /* transp */)
{
  static _tt_db_cache_results results;
  results.cache_level = -1;

  _Tt_string real_file = args->file;
  _Tt_string partition = _tt_get_file_partition(args->file);
  _Tt_db_server_db_ptr db;
  results.results = _tt_get_partition_db(partition, db);

  _Tt_db_access_ptr access = _tt_get_file_access(real_file, args->access);

  if (results.results == TT_DB_OK) {
    // See if the file already exists by trying to get its access info
    _Tt_db_access_ptr temp_access;
    _Tt_db_results temp_results = db->getFileAccess(real_file,
						    access,
						    temp_access);

    // If the file exists...
    if ((temp_results == TT_DB_OK) ||
	(temp_results == TT_DB_ERR_ACCESS_DENIED)) {
      results.results = TT_DB_ERR_FILE_EXISTS;
    }
    // Else, if the file does not exist...
    else if (temp_results == TT_DB_ERR_NO_SUCH_FILE) {
      results.results = TT_DB_OK;
    }
    else {
      results.results = temp_results;
    }
  }

  if (results.results == TT_DB_OK) {
    results.results = db->createFile(real_file, access);

    if (results.results == TT_DB_OK) {
      _Tt_db_property_list_ptr properties;
      _tt_get_rpc_properties(args->properties, properties);

      if (!properties.is_null()) {
	results.results = db->setFileProperties(real_file,
						properties,
						access);
      }

      if (results.results == TT_DB_OK) {
	results.results =
	  _tt_increment_file_properties_cache_level(db,
						    real_file,
						    access,
						    results.cache_level);
      }
    }
  }

  return &results;
}

_tt_db_cache_results *_tt_create_obj_1 (_tt_create_obj_args *args,
					SVCXPRT * /* transp */)
{
  static _tt_db_cache_results results;
  results.cache_level = -1;

  _Tt_string partition = _tt_get_object_partition(args->objid);
  _Tt_db_server_db_ptr db;
  results.results = _tt_get_partition_db(partition, db);

  _Tt_string        real_file = args->file;
  _Tt_db_access_ptr object_access = _tt_get_real_rpc_access(args->access);

  if (results.results == TT_DB_OK) {
    // See if the object already exists by trying to get the forward
    // pointer property
    _Tt_db_property_ptr temp_property;
    _Tt_db_results temp_results =
      db->getObjectProperty(args->objid,
			    TT_DB_FORWARD_POINTER_PROPERTY,
			    object_access,
			    temp_property);

    // If the property exists, remove the object
    if (temp_results == TT_DB_OK) {
      (void)db->removeObject(args->objid, object_access);
    }
    // Else if the object exists without a forward pointer...
    else if (temp_results == TT_DB_ERR_NO_SUCH_PROPERTY) {
      results.results = TT_DB_ERR_OBJECT_EXISTS;
    }
    // Else if the object does not exist...
    else if (temp_results == TT_DB_ERR_NO_SUCH_OBJECT) {
      results.results = TT_DB_OK;
    }
    else {
      results.results = temp_results;
    }
  }

  if (results.results == TT_DB_OK) {
    _Tt_db_access_ptr file_access = _tt_get_file_access(real_file,
						        args->access);
    results.results = db->createObject(real_file,
				       args->objid,
				       object_access,
				       file_access);
    
    if (results.results == TT_DB_OK) {
      _Tt_db_property_list_ptr properties;
      _tt_get_rpc_properties(args->properties, properties);
      
      if (!properties.is_null()) {
	_tt_screen_object_properties (properties);
      }

      if (!properties.is_null()) {
	results.results = db->setObjectProperties(args->objid,
						  properties,
						  object_access);
      }

      // Set the otype of the object
      if (results.results == TT_DB_OK) {
	_Tt_db_property_ptr property = new _Tt_db_property;
	property->name = TT_DB_OBJECT_TYPE_PROPERTY;
	property->values->append(_Tt_string(args->otype));

	results.results = db->setObjectProperty(args->objid,
						property,
						object_access);
      }

      if (results.results == TT_DB_OK) {
	results.results =
	  _tt_increment_object_properties_cache_level(db,
						      args->objid,
						      object_access,
						      results.cache_level);
      }
    }
  }

  return &results;
}

_tt_db_results *_tt_remove_file_1 (_tt_remove_file_args *args,
				   SVCXPRT * /* transp */)
{
  static _tt_db_results results;

  _Tt_db_access_ptr accessPtr = _tt_get_real_rpc_access(args->access);

  _Tt_string real_file = args->file;
  _Tt_string partition = _tt_get_file_partition(args->file);
  _Tt_db_server_db_ptr db;
  results = _tt_get_partition_db(partition, db);

  if (results == TT_DB_OK) {
    // Get the list of children under the file to remove along with
    // the file.  The list includes the file itself.
    _Tt_string_list_ptr children;
    results = db->getFileChildren(real_file, children);

    if (children->is_empty()) {
      results = TT_DB_ERR_NO_SUCH_FILE;
    }
    else {
      _Tt_string_list_cursor children_cursor(children);
      while ((results == TT_DB_OK) && children_cursor.next()) {
	results = db->removeFile(*children_cursor, accessPtr);
      }
    }
  }

  return &results;
}

_tt_db_results *_tt_remove_obj_1 (_tt_remove_obj_args *args,
				  SVCXPRT * /* transp */)
{
  static _tt_db_results results;

  _Tt_db_access_ptr accessPtr = _tt_get_real_rpc_access(args->access);

  _Tt_string partition = _tt_get_object_partition(args->objid);
  _Tt_db_server_db_ptr db;
  results = _tt_get_partition_db(partition, db);

  if (results == TT_DB_OK) {
    results = db->removeObject(args->objid, accessPtr);

    if (results == TT_DB_OK) {
      if (args->forward_pointer && strlen(args->forward_pointer)) {
	// Allow everyone to read and delete the forward pointer
	_Tt_db_access_ptr new_access = new _Tt_db_access;
	new_access->user = (uid_t)-1;
	new_access->group = (gid_t)-1;
	new_access->mode = (mode_t)-1;

	// Create a special forward pointer object with no file...
        results = db->createObject((char *)NULL,
				   args->objid,
				   new_access,
				   new_access);

        if (results == TT_DB_OK) {
	  _Tt_db_property_ptr property = new _Tt_db_property;
	  property->name = TT_DB_FORWARD_POINTER_PROPERTY;
	  property->values->append(_Tt_string(args->forward_pointer));

	  results = db->setObjectProperty(args->objid, property, accessPtr);
        }
      }
    }
  }

  return &results;
}

_tt_db_results *_tt_move_file_1 (_tt_move_file_args *args,
				 SVCXPRT * /* transp */)
{
  static _tt_db_results results;

  _Tt_db_access_ptr accessPtr = _tt_get_real_rpc_access(args->access);

  _Tt_string real_file = args->file;
  _Tt_string real_new_file = args->new_file;

  // Make sure we really need to do a move
  if (real_file != real_new_file) {
    _Tt_file_system           fs;
    _Tt_file_system_entry_ptr entry = fs.bestMatchToPath(real_file);
    _Tt_file_system_entry_ptr new_entry = fs.bestMatchToPath(real_new_file);
    
    _Tt_string partition = entry->getMountPoint();
    _Tt_string new_partition = new_entry->getMountPoint();
    
    if (partition == new_partition) {
      // Get a connection to the partition DB
      _Tt_db_server_db_ptr db;
      results = _tt_get_partition_db(partition, db);

      _Tt_string_list_ptr children;
      if (results == TT_DB_OK) {
	// Get a list of the files to move
	results = db->getFileChildren(real_file, children);
      }

      if (results == TT_DB_OK) {
	if (children->is_empty ()) {
	  results = TT_DB_ERR_NO_SUCH_FILE;
	}
	else {
	  _Tt_string_list_cursor children_cursor(children);
	  while ((results == TT_DB_OK) && children_cursor.next()) {
	    // Construct the new file name by replacing the part
	    // that equals the "real_file" with the "real_new_file".
	    int length = (*children_cursor).len() - real_file.len();
	    _Tt_string new_child = real_new_file.cat("/");
	    new_child = new_child.cat((*children_cursor).right(length));

	    // Change the file name in the database
	    db->setFileFile(*children_cursor, new_child, accessPtr);
	  }
	}
      }
    }
    // Else, different partitions, therefore we can only move one
    // non-directory file
    else if (!_tt_is_file_a_directory(real_file)) {
      // Get a connections to both partition DBs
      _Tt_db_server_db_ptr db;
      results = _tt_get_partition_db(partition, db);

      _Tt_db_server_db_ptr new_db;
      if (results == TT_DB_OK) {
	results = _tt_get_partition_db(new_partition, new_db);
      }

      if (results == TT_DB_OK) {
	_Tt_db_property_list_ptr properties;
	_Tt_db_access_ptr        current_access;
	_Tt_string_list_ptr      objids;

	// Get all of the file's possesions
	results = db->getFileProperties(real_file, accessPtr, properties);
	if (results == TT_DB_OK) {
	  results = db->getFileAccess(real_file, accessPtr, current_access);
	}
	if (results == TT_DB_OK) {
	  results = db->getFileObjects(real_file, accessPtr, objids);
	}

	// Create the new file
	if (results == TT_DB_OK) {
	  results = new_db->createFile(real_new_file, current_access);
	}

	// Copy the old file's properties to the new file
	if (results == TT_DB_OK) {
	  results = new_db->setFileProperties(real_new_file,
					      properties,
					      accessPtr);
	}

	// Create the new objects with equivalent objids on the new partition
	_Tt_string_list_ptr new_objids;
	if (results == TT_DB_OK  && (!objids->is_empty())) {
	  new_objids = new _Tt_string_list;

	  // Loop through the file's objects
	  _Tt_string_list_cursor objids_cursor(objids);
	  while ((results == TT_DB_OK) && objids_cursor.next()) {
	    _Tt_string new_objid =
	      _tt_make_equivalent_object_id(*objids_cursor, new_partition);
	    new_objids->append(new_objid);

	    _Tt_db_property_list_ptr properties;
	    _Tt_db_access_ptr        current_access;

	    // Get all of the object's possesions
	    results = db->getObjectProperties(*objids_cursor,
					      accessPtr,
					      properties);
	    if (results == TT_DB_OK) {
	      results = db->getObjectAccess(*objids_cursor,
					    accessPtr,
					    current_access);
	    }

	    // Create the new object
	    if (results == TT_DB_OK) {
	      results = new_db->createObject(real_new_file,
					     new_objid,
					     current_access,
					     current_access);
	    }

	    // Copy the old object's properties to the new object
	    if (results == TT_DB_OK) {
	      results = new_db->setObjectProperties(new_objid,
						    properties,
						    accessPtr);
	    }
	  }
	}

	// Remove the old file and all of its objects
	if (results == TT_DB_OK) {
	  results = db->removeFile(real_file, accessPtr);
	}

	// Create the forwarding pointers for all of the objects on
	// the old file
	if ((results == TT_DB_OK) && (!objids->is_empty())) {
	  _Tt_string_list_cursor objids_cursor(objids);
	  _Tt_string_list_cursor new_objids_cursor(new_objids);

	  while ((results == TT_DB_OK) &&
		 objids_cursor.next() &&
		 new_objids_cursor.next()) {
	    // Allow everyone to read and delete the forward pointer
	    _Tt_db_access_ptr new_access = new _Tt_db_access;
	    new_access->user = (uid_t)-1;
	    new_access->group = (gid_t)-1;
	    new_access->mode = (mode_t)-1;

	    // Create a special forward pointer object with no file...
	    results = db->createObject((char *)NULL,
				       *objids_cursor,
				       new_access,
				       new_access);

	    if (results == TT_DB_OK) {
	      _Tt_db_property_ptr property = new _Tt_db_property;
	      property->name = TT_DB_FORWARD_POINTER_PROPERTY;
	      property->values->append(*new_objids_cursor);

	      results = db->setObjectProperty(*objids_cursor,
					      property,
					      accessPtr);
	    }
	  }
	}
      }
    }
    else {
      results = TT_DB_ERR_ILLEGAL_FILE;
    }
  }
  else {
    results = TT_DB_ERR_SAME_FILE;
  }

  return &results;
}

_tt_db_cache_results *_tt_set_file_props_1 (_tt_set_file_props_args *args,
					    SVCXPRT * /* transp */)
{
  static _tt_db_cache_results results;
  results.cache_level = -1;

  _Tt_db_access_ptr accessPtr = _tt_get_real_rpc_access(args->access);

  _Tt_string real_file = args->file;
  _Tt_string partition = _tt_get_file_partition(args->file);
  _Tt_db_server_db_ptr db;
  results.results = _tt_get_partition_db(partition, db);

  if (results.results == TT_DB_OK) {
    _Tt_db_property_list_ptr properties;
    _tt_get_rpc_properties(args->properties, properties);

    if (!properties.is_null()) {
      results.results = db->setFileProperties(real_file,
					      properties,
					      accessPtr);
    }
    else {
      results.results = TT_DB_ERR_ILLEGAL_PROPERTY;
    }

    if (results.results == TT_DB_OK) {
      results.results =
	_tt_increment_file_properties_cache_level(db,
						  real_file,
						  accessPtr,
						  results.cache_level);
    }
  }

  return &results;
}

_tt_db_cache_results *_tt_set_file_prop_1 (_tt_set_file_prop_args *args,
					   SVCXPRT * /* transp */)
{
  static _tt_db_cache_results results;
  results.cache_level = -1;

  _Tt_db_access_ptr accessPtr = _tt_get_real_rpc_access(args->access);

  _Tt_string real_file = args->file;
  _Tt_string partition = _tt_get_file_partition(args->file);
  _Tt_db_server_db_ptr db;
  results.results = _tt_get_partition_db(partition, db);

  if (results.results == TT_DB_OK) {
    _Tt_db_property_ptr property;
    _tt_get_rpc_property(args->property, property);

    if (!property.is_null()) {
      results.results = db->setFileProperty(real_file,
					    property,
					    accessPtr);
    }
    else {
      results.results = TT_DB_ERR_ILLEGAL_PROPERTY;
    }

    if (results.results == TT_DB_OK) {
      results.results =
	_tt_increment_file_properties_cache_level(db,
						  real_file,
						  accessPtr,
						  results.cache_level);
    }
  }

  return &results;
}

_tt_db_cache_results *_tt_add_file_prop_1 (_tt_add_file_prop_args *args,
					   SVCXPRT * /* transp */)
{
  static _tt_db_cache_results results;
  results.cache_level = -1;

  _Tt_db_access_ptr accessPtr = _tt_get_real_rpc_access(args->access);

  _Tt_string real_file = args->file;
  _Tt_string partition = _tt_get_file_partition(args->file);
  _Tt_db_server_db_ptr db;
  results.results = _tt_get_partition_db(partition, db);

  if (results.results == TT_DB_OK) {
    _Tt_db_property_ptr property;
    _tt_get_rpc_property(args->property, property);

    if (!property.is_null()) {
      results.results = db->addFileProperty(real_file,
					    property,
					    args->unique,
					    accessPtr);
    }
    else {
      results.results = TT_DB_ERR_ILLEGAL_PROPERTY;
    }

    if (results.results == TT_DB_OK) {
      results.results =
	_tt_increment_file_properties_cache_level(db,
						  real_file,
						  accessPtr,
						  results.cache_level);
    }
  }

  return &results;
}

_tt_db_cache_results *_tt_delete_file_prop_1 (_tt_del_file_prop_args *args,
					      SVCXPRT * /* transp */)
{
  static _tt_db_cache_results results;
  results.cache_level = -1;

  _Tt_db_access_ptr accessPtr = _tt_get_real_rpc_access(args->access);

  _Tt_string real_file = args->file;
  _Tt_string partition = _tt_get_file_partition(args->file);
  _Tt_db_server_db_ptr db;
  results.results = _tt_get_partition_db(partition, db);

  if (results.results == TT_DB_OK) {
    _Tt_db_property_ptr property;
    _tt_get_rpc_property(args->property, property);

    if (!property.is_null()) {
      results.results = db->deleteFileProperty(real_file,
					       property,
					       accessPtr);
    }
    else {
      results.results = TT_DB_ERR_ILLEGAL_PROPERTY;
    }

    if (results.results == TT_DB_OK) {
      results.results =
	_tt_increment_file_properties_cache_level(db,
						  real_file,
						  accessPtr,
						  results.cache_level);
    }
  }

  return &results;
}

_tt_file_prop_results *_tt_get_file_prop_1 (_tt_get_file_prop_args *args,
					    SVCXPRT * /* transp */)
{
  static _tt_file_prop_results results;
  results.cache_level = -1;

  _Tt_db_access_ptr accessPtr = _tt_get_real_rpc_access(args->access);

  _Tt_string real_file = args->file;
  _Tt_string partition = _tt_get_file_partition(args->file);
  _Tt_db_server_db_ptr db;
  results.results = _tt_get_partition_db(partition, db);

  if (results.results == TT_DB_OK) {
    results.results = _tt_get_file_properties_cache_level(db,
							  real_file,
							  accessPtr,
							  results.cache_level);
  }

  _Tt_db_property_ptr property;
  if ((results.cache_level > args->cache_level) &&
      (results.results == TT_DB_OK)) {
    results.results = db->getFileProperty(real_file,
					  args->name,
					  accessPtr,
					  property);
  }
  _tt_set_rpc_property(property, results.property);

  return &results;
}

_tt_file_props_results *
_tt_get_file_props_1 (_tt_get_file_props_args *args,
		      SVCXPRT * /* transp */)
{
  static _tt_file_props_results results;
  results.cache_level = -1;

  _Tt_db_access_ptr accessPtr = _tt_get_real_rpc_access(args->access);

  _Tt_string real_file = args->file;
  _Tt_string partition = _tt_get_file_partition(args->file);
  _Tt_db_server_db_ptr db;
  results.results = _tt_get_partition_db(partition, db);

  if (results.results == TT_DB_OK) {
    results.results = _tt_get_file_properties_cache_level(db,
							  real_file,
							  accessPtr,
							  results.cache_level);
  }

  _Tt_db_property_list_ptr properties;
  if ((results.cache_level > args->cache_level) &&
      (results.results == TT_DB_OK)) {
    results.results = db->getFileProperties(real_file,
					    accessPtr,
					    properties);
  }
  _tt_set_rpc_properties(properties, results.properties);

  return &results;
}

_tt_file_objs_results *_tt_get_file_objs_1 (_tt_get_file_objs_args *args,
					    SVCXPRT * /* transp */)
{
  static _tt_file_objs_results results;

  // Make sure the returned cache level is higher then the callers
  // cache level, so that the caller will use the returned data
  results.cache_level = args->cache_level + 1;

  _Tt_db_access_ptr accessPtr = _tt_get_real_rpc_access(args->access);

  _Tt_string real_file = args->file;
  _Tt_string partition = _tt_get_file_partition(args->file);
  _Tt_db_server_db_ptr db;
  results.results = _tt_get_partition_db(partition, db);

  _Tt_string_list_ptr objids;
  if ((results.cache_level > args->cache_level) &&
      (results.results == TT_DB_OK)) {
    results.results = db->getFileObjects(real_file,
					 accessPtr,
					 objids);
  }
  _tt_set_rpc_strings(objids, results.objids);

  return &results;
}

_tt_db_results *_tt_set_file_access_1 (_tt_set_file_access_args *args,
				       SVCXPRT * /* transp */)
{
  static _tt_db_results results;

  _Tt_db_access_ptr accessPtr = _tt_get_real_rpc_access(args->access);

  _Tt_string real_file = args->file;
  _Tt_string partition = _tt_get_file_partition(args->file);
  _Tt_db_server_db_ptr db;
  results = _tt_get_partition_db(partition, db);

  _Tt_db_access_ptr new_access;
  if (results == TT_DB_OK) {
    _tt_get_rpc_access(args->new_access, new_access);

    if ((new_access->user == (uid_t)-1) ||
        (new_access->group == (gid_t)-1) ||
        (new_access->mode == (mode_t)-1)) { 
      _Tt_db_access_ptr current_access;   
      results = db->getFileAccess(real_file, 
                                  accessPtr,
                                  current_access);
 
      if (results == TT_DB_OK) { 
        if (new_access->user == (uid_t)-1) {
          new_access->user = current_access->user;
        }
 
        if (new_access->group == (gid_t)-1) {
          new_access->group = current_access->group;
        }
 
        if (new_access->mode == (mode_t)-1) { 
          new_access->mode = current_access->mode; 
        } 
      }  
    }  
 
    if (results == TT_DB_OK) { 
      results = db->setFileAccess(real_file, new_access, accessPtr);
    }
  }

  return &results;
}

_tt_file_access_results *_tt_get_file_access_1 (_tt_get_file_access_args *args,
						SVCXPRT * /* transp */)
{
  static _tt_file_access_results results;

  _Tt_string real_file = args->file;
  _Tt_string partition = _tt_get_file_partition(args->file);
  _Tt_db_server_db_ptr db;
  results.results = _tt_get_partition_db(partition, db);

  _Tt_db_access_ptr accessPtr = _tt_get_real_rpc_access(args->access);

  _Tt_db_access_ptr current_access;
  if (results.results == TT_DB_OK) {
    results.results = db->getFileAccess(real_file,
					accessPtr,
					current_access);
  }
  _tt_set_rpc_access(current_access, results.access);

  return &results;
}

_tt_obj_props_results *_tt_set_obj_props_1 (_tt_set_obj_props_args *args,
					    SVCXPRT * /* transp */)
{
  static _tt_obj_props_results results;
  results.cache_level = -1;

  _Tt_string partition = _tt_get_object_partition(args->objid);
  _Tt_db_server_db_ptr db;
  results.results = _tt_get_partition_db(partition, db);

  _Tt_db_access_ptr accessPtr = _tt_get_real_rpc_access(args->access);

  if (results.results == TT_DB_OK) {
    results.results =
      _tt_get_object_properties_cache_level(db,
					    args->objid,
					    accessPtr,
					    results.cache_level);
  }

  _Tt_db_property_list_ptr properties;

  // If the cache level in the DB is higher then the passed in
  // cache level, then someone else must have updated before
  // the callers last read from the DB --> update conflict...
  if ((results.results == TT_DB_OK) &&
      (results.cache_level > args->cache_level)) {
    results.results = TT_DB_ERR_UPDATE_CONFLICT;
    
    _Tt_db_results temp_results = db->getObjectProperties(args->objid,
							  accessPtr,
							  properties);
    
    if (results.results != TT_DB_OK) {
      properties = (_Tt_db_property_list *)NULL;
      results.results = temp_results;
    }
  }
  _tt_set_rpc_properties(properties, results.properties);
  
  // No update conflicts or any other weirdness...
  if (results.results == TT_DB_OK) {
    _tt_get_rpc_properties(args->properties, properties);
    
    if (!properties.is_null()) {
      _tt_screen_object_properties (properties);
    }

    if (!properties.is_null()) {
      results.results = db->setObjectProperties(args->objid,
						properties,
						accessPtr);
    }
    else {
      results.results = TT_DB_ERR_ILLEGAL_PROPERTY;
    }
  
    if (results.results == TT_DB_OK) {
      results.results =
	_tt_increment_object_properties_cache_level(db,
						    args->objid,
						    accessPtr,
						    results.cache_level);
    }
  }

  return &results;
}

_tt_obj_props_results *_tt_set_obj_prop_1 (_tt_set_obj_prop_args *args,
					   SVCXPRT * /* transp */)
{
  static _tt_obj_props_results results;
  results.cache_level = -1;

  _Tt_string partition = _tt_get_object_partition(args->objid);
  _Tt_db_server_db_ptr db;
  results.results = _tt_get_partition_db(partition, db);

  _Tt_db_access_ptr accessPtr = _tt_get_real_rpc_access(args->access);

  if (results.results == TT_DB_OK) {
    results.results =
      _tt_get_object_properties_cache_level(db,
					    args->objid,
					    accessPtr,
					    results.cache_level);
  }

  _Tt_db_property_list_ptr properties;

  // If the cache level in the DB is higher then the passed in
  // cache level, then someone else must have updated before
  // the callers last read from the DB --> update conflict...
  if ((results.results == TT_DB_OK) &&
      (results.cache_level > args->cache_level)) {
    results.results = TT_DB_ERR_UPDATE_CONFLICT;
    
    _Tt_db_results temp_results = db->getObjectProperties(args->objid,
							  accessPtr,
							  properties);
    
    if (results.results != TT_DB_OK) {
      properties = (_Tt_db_property_list *)NULL;
      results.results = temp_results;
    }
  }
  _tt_set_rpc_properties(properties, results.properties);
  
  // No update conflicts or any other weirdness...
  if (results.results == TT_DB_OK) {
    _Tt_db_property_ptr property;
    _tt_get_rpc_property(args->property, property);
      
    if (!property.is_null()) {
      results.results = db->setObjectProperty(args->objid,
					      property,
					      accessPtr);
    }
    else {
      results.results = TT_DB_ERR_ILLEGAL_PROPERTY;
    }
    
    if (results.results == TT_DB_OK) {
      results.results =
	_tt_increment_object_properties_cache_level(db,
						    args->objid,
						    accessPtr,
						    results.cache_level);
    }
  }

  return &results;
}

_tt_obj_props_results *_tt_add_obj_prop_1 (_tt_add_obj_prop_args *args,
					   SVCXPRT * /* transp */)
{
  static _tt_obj_props_results results;
  results.cache_level = -1;

  _Tt_string partition = _tt_get_object_partition(args->objid);
  _Tt_db_server_db_ptr db;
  results.results = _tt_get_partition_db(partition, db);

  _Tt_db_access_ptr accessPtr = _tt_get_real_rpc_access(args->access);

  if (results.results == TT_DB_OK) {
    results.results =
      _tt_get_object_properties_cache_level(db,
					    args->objid,
					    accessPtr,
					    results.cache_level);
  }

  _Tt_db_property_list_ptr properties;

  // If the cache level in the DB is higher then the passed in
  // cache level, then someone else must have updated before
  // the callers last read from the DB --> update conflict...
  if ((results.results == TT_DB_OK) &&
      (results.cache_level > args->cache_level)) {
    results.results = TT_DB_ERR_UPDATE_CONFLICT;
    
    _Tt_db_results temp_results = db->getObjectProperties(args->objid,
							  accessPtr,
							  properties);
    
    if (results.results != TT_DB_OK) {
      properties = (_Tt_db_property_list *)NULL;
      results.results = temp_results;
    }
  }
  _tt_set_rpc_properties(properties, results.properties);
  
  // No update conflicts or any other weirdness...
  if (results.results == TT_DB_OK) {
    _Tt_db_property_ptr property;
    _tt_get_rpc_property(args->property, property);

    if (!property.is_null()) {
      results.results = db->addObjectProperty(args->objid,
					      property,
					      args->unique,
					      accessPtr);
    }
    else {
      results.results = TT_DB_ERR_ILLEGAL_PROPERTY;
    }
    
    if (results.results == TT_DB_OK) {
      results.results =
	_tt_increment_object_properties_cache_level(db,
						    args->objid,
						    accessPtr,
						    results.cache_level);
    }
  }

  return &results;
}

_tt_obj_props_results *_tt_delete_obj_prop_1 (_tt_del_obj_prop_args *args,
					      SVCXPRT * /* transp */)
{
  static _tt_obj_props_results results;
  results.cache_level = -1;

  _Tt_string partition = _tt_get_object_partition(args->objid);
  _Tt_db_server_db_ptr db;
  results.results = _tt_get_partition_db(partition, db);

  _Tt_db_access_ptr accessPtr = _tt_get_real_rpc_access(args->access);

  if (results.results == TT_DB_OK) {
    results.results =
      _tt_get_object_properties_cache_level(db,
					    args->objid,
					    accessPtr,
					    results.cache_level);
  }

  // If the cache level in the DB is higher then the passed in
  // cache level, then someone else must have updated before
  // the callers last read from the DB --> update conflict...
  if ((results.results == TT_DB_OK) &&
      (results.cache_level > args->cache_level)) {
    results.results = TT_DB_ERR_UPDATE_CONFLICT;
  }
    
  // No update conflicts or any other weirdness...

  if (results.results == TT_DB_OK) {
    _Tt_db_property_ptr property;
    _tt_get_rpc_property(args->property, property);

    if (!property.is_null()) {
      results.results = db->deleteObjectProperty(args->objid,
						 property,
						 accessPtr);
    }
    else {
      results.results = TT_DB_ERR_ILLEGAL_PROPERTY;
    }
    
    if (results.results == TT_DB_OK) {
      results.results =
	_tt_increment_object_properties_cache_level(db,
						    args->objid,
						    accessPtr,
						    results.cache_level);
    }
  }

  // The delete object prop function is the only object function that always
  // returns that latest object properties.  The cache level passed in
  // is only used for update conflict detection.
  _Tt_db_property_list_ptr properties;
  if ((results.results == TT_DB_OK) ||
      (results.results == TT_DB_ERR_UPDATE_CONFLICT)) {
    _Tt_db_results temp_results = db->getObjectProperties(args->objid,
							  accessPtr,
							  properties);
    
    if (results.results != TT_DB_OK) {
      properties = (_Tt_db_property_list *)NULL;
      results.results = temp_results;
    }
  }
  _tt_set_rpc_properties(properties, results.properties);
  
  return &results;
}

_tt_obj_prop_results *_tt_get_obj_prop_1 (_tt_get_obj_prop_args *args,
					  SVCXPRT * /* transp */)
{
  static _tt_obj_prop_results results;
  results.cache_level = -1;

  _Tt_string partition = _tt_get_object_partition(args->objid);
  _Tt_db_server_db_ptr db;
  results.results = _tt_get_partition_db(partition, db);

  _Tt_db_access_ptr accessPtr = _tt_get_real_rpc_access(args->access);

  if (results.results == TT_DB_OK) {
    results.results =
      _tt_get_object_properties_cache_level(db,
					    args->objid,
					    accessPtr,
					    results.cache_level);
  }

  _Tt_db_property_ptr property;

  // Only return values if the DB cache level is higher then the
  // callers cache level
  if ((results.results == TT_DB_OK) &&
      (results.cache_level > args->cache_level)) {
    results.results = db->getObjectProperty(args->objid,
					    args->name,
					    accessPtr,
					    property);
  }
  _tt_set_rpc_property(property, results.property);
    
  return &results;
}

_tt_obj_props_results *_tt_get_obj_props_1 (_tt_get_obj_props_args *args,
					    SVCXPRT * /* transp */)
{
  static _tt_obj_props_results results;
  results.cache_level = -1;

  _Tt_string partition = _tt_get_object_partition(args->objid);
  _Tt_db_server_db_ptr db;
  results.results = _tt_get_partition_db(partition, db);

  _Tt_db_access_ptr accessPtr = _tt_get_real_rpc_access(args->access);

  if (results.results == TT_DB_OK) {
    results.results =
      _tt_get_object_properties_cache_level(db,
					    args->objid,
					    accessPtr,
					    results.cache_level);
  }

  _Tt_db_property_list_ptr properties;

  // Only return values if the DB cache level is higher then the
  // callers cache level
  if ((results.results == TT_DB_OK) &&
      (results.cache_level > args->cache_level)) {
    results.results = db->getObjectProperties(args->objid,
					      accessPtr,
					      properties);
  }
  _tt_set_rpc_properties(properties, results.properties);

  return &results;
}

_tt_db_results *_tt_set_obj_type_1 (_tt_set_obj_type_args *args,
				    SVCXPRT * /* transp */)
{
  static _tt_db_results results;

  _Tt_string partition = _tt_get_object_partition(args->objid);
  _Tt_db_server_db_ptr db;
  results = _tt_get_partition_db(partition, db);

  _Tt_db_access_ptr accessPtr = _tt_get_real_rpc_access(args->access);

  if (results == TT_DB_OK) {
    _Tt_db_property_ptr property = new _Tt_db_property;
    property->name = TT_DB_OBJECT_TYPE_PROPERTY;
    property->values->append(_Tt_string(args->otype));

    results = db->setObjectProperty(args->objid, property, accessPtr);
  }

  return &results;
}

_tt_obj_type_results *_tt_get_obj_type_1 (_tt_get_obj_type_args *args,
					  SVCXPRT * /* transp */)
{
  static _tt_obj_type_results results;

  _Tt_string partition = _tt_get_object_partition(args->objid);
  _Tt_db_server_db_ptr db;
  results.results = _tt_get_partition_db(partition, db);

  _Tt_db_access_ptr accessPtr = _tt_get_real_rpc_access(args->access);

  if (results.results == TT_DB_OK) {
    _Tt_db_property_ptr property;
    results.results = db->getObjectProperty(args->objid,
					    TT_DB_OBJECT_TYPE_PROPERTY,
					    accessPtr,
					    property);

    results.otype = (char *)NULL;
    if (results.results == TT_DB_OK) {
      if (!property.is_null() && !property->is_empty()) {
	_Tt_string otype = (*property->values) [0];
	int length = otype.len();

	results.otype = (char *)malloc(length+1);
	memcpy(results.otype, (char *)otype, length);
	results.otype [length] = '\0';
      }
    }
    else if (results.results == TT_DB_ERR_NO_SUCH_PROPERTY) {
      results.results = TT_DB_ERR_NO_OTYPE;
    }
  }

  return &results;
}

_tt_db_results *_tt_set_obj_file_1 (_tt_set_obj_file_args *args,
				    SVCXPRT * /* transp */)
{
  static _tt_db_results results;

  _Tt_string partition = _tt_get_object_partition(args->objid);
  _Tt_db_server_db_ptr db;
  results = _tt_get_partition_db(partition, db);

  _Tt_db_access_ptr accessPtr = _tt_get_real_rpc_access(args->access);

  if (results == TT_DB_OK) {
    results = db->setObjectFile(args->objid, args->file, accessPtr);
  }

  return &results;
}

_tt_obj_file_results *_tt_get_obj_file_1 (_tt_get_obj_file_args *args,
					  SVCXPRT * /* transp */)
{
  static _tt_obj_file_results results;

  _Tt_string partition = _tt_get_object_partition(args->objid);
  _Tt_db_server_db_ptr db;
  results.results = _tt_get_partition_db(partition, db);

  _Tt_db_access_ptr accessPtr = _tt_get_real_rpc_access(args->access);

  if (results.results == TT_DB_OK) {
    _Tt_string file;
    results.results = db->getObjectFile(args->objid,
					accessPtr,
					file);

    results.file = (char *)NULL;
    if (results.results == TT_DB_OK) {
      if (file.len()) {
	results.file = strdup((char *)file);
      }
    }
  }

  return &results;
}

_tt_db_results *_tt_set_obj_access_1 (_tt_set_obj_access_args *args,
				      SVCXPRT * /* transp */)
{
  static _tt_db_results results;

  _Tt_string partition = _tt_get_object_partition(args->objid);
  _Tt_db_server_db_ptr db;
  results = _tt_get_partition_db(partition, db);

  _Tt_db_access_ptr accessPtr = _tt_get_real_rpc_access(args->access);

  _Tt_db_access_ptr new_access;
  if (results == TT_DB_OK) {
    _tt_get_rpc_access(args->new_access, new_access);

    if ((new_access->user == (uid_t)-1) ||
        (new_access->group == (gid_t)-1) ||
        (new_access->mode == (mode_t)-1)) {
      _Tt_db_access_ptr current_access;
      results = db->getObjectAccess(args->objid,
				    accessPtr,
				    current_access);

      if (results == TT_DB_OK) {
	if (new_access->user == (uid_t)-1) {
	  new_access->user = current_access->user;
	}

	if (new_access->group == (gid_t)-1) {
	  new_access->group = current_access->group;
	}

	if (new_access->mode == (mode_t)-1) {
	  new_access->mode = current_access->mode;
	}
      }
    }

    if (results == TT_DB_OK) {
      results = db->setObjectAccess(args->objid, new_access, accessPtr);
    }
  }

  return &results;
}

_tt_obj_access_results *_tt_get_obj_access_1 (_tt_get_obj_access_args *args,
					      SVCXPRT * /* transp */)
{
  static _tt_obj_access_results results;

  _Tt_string partition = _tt_get_object_partition(args->objid);
  _Tt_db_server_db_ptr db;
  results.results = _tt_get_partition_db(partition, db);

  _Tt_db_access_ptr accessPtr = _tt_get_real_rpc_access(args->access);

  _Tt_db_access_ptr current_access;
  if (results.results == TT_DB_OK) {
    results.results = db->getObjectAccess(args->objid,
					  accessPtr,
					  current_access);
  }
  _tt_set_rpc_access(current_access, results.access);

  return &results;
}

_tt_is_file_in_db_results *_tt_is_file_in_db_1 (_tt_is_file_in_db_args *args,
						SVCXPRT * /* transp */)
{
  static _tt_is_file_in_db_results results;
  
  _Tt_db_access_ptr accessPtr = _tt_get_real_rpc_access(args->access);

  _Tt_string real_file = args->file;
  _Tt_string partition = _tt_get_file_partition(args->file);
  _Tt_db_server_db_ptr db;
  results.results = _tt_get_partition_db(partition, db);

  if (results.results == TT_DB_OK) {
    // See if the file already exists by trying to get its access info
    _Tt_db_access_ptr temp_access;
    _Tt_db_results temp_results = db->getFileAccess(real_file,
						    accessPtr,
						    temp_access);

    // If the file exists...
    if ((temp_results == TT_DB_OK) ||
	(temp_results == TT_DB_ERR_ACCESS_DENIED)) {
      results.results = TT_DB_OK;
    }
    else {
      results.results = temp_results;
    }

    // See if the file is a directory
    results.directory_flag = _tt_is_file_a_directory(real_file);
  }

  return &results;
}

_tt_is_obj_in_db_results *_tt_is_obj_in_db_1 (_tt_is_obj_in_db_args *args,
					      SVCXPRT * /* transp */)
{
  static _tt_is_obj_in_db_results results;
  results.forward_pointer = (char *)NULL;
  
  _Tt_string partition = _tt_get_object_partition(args->objid);
  _Tt_db_server_db_ptr db;
  results.results = _tt_get_partition_db(partition, db);

  _Tt_db_access_ptr accessPtr = _tt_get_real_rpc_access(args->access);

  if (results.results == TT_DB_OK) {
    _Tt_string file;

    // See if the object has an entry in the file-object map
    results.results = db->getObjectFile(args->objid,
					accessPtr,
					file);

    // If no entry in the file-object map...
    if (results.results != TT_DB_OK) {
      _Tt_db_property_ptr property;

      // See if there is a forward pointer
      _Tt_db_results temp_results =
	db->getObjectProperty(args->objid,
			      TT_DB_FORWARD_POINTER_PROPERTY,
			      accessPtr,
			      property);

      // If there is a forward pointer...
      if (temp_results == TT_DB_OK) {
	_Tt_string forward_pointer = (*property->values) [0];
	int length = forward_pointer.len();
	results.forward_pointer = (char *)malloc(length+1);

	memcpy(results.forward_pointer, (char *)forward_pointer, length);
	results.forward_pointer [length] = '\0';
	results.results = TT_DB_WRN_FORWARD_POINTER;
      }
    }
  }

  return &results;
}

_tt_db_results *_tt_queue_message_1 (_tt_queue_msg_args *args,
				     SVCXPRT * /* transp */)
{
  static _tt_db_results results;

  bool_t property_written = FALSE;

  _Tt_db_server_db_ptr db;
  _Tt_string           real_file = args->file;
  if (!args->message.body.body_len) {
    results = TT_DB_ERR_ILLEGAL_MESSAGE;
  }
  else {
    _Tt_string partition = _tt_get_file_partition(args->file);
    results = _tt_get_partition_db(partition, db);
  }

  if (results == TT_DB_OK) {
    _Tt_db_access_ptr accessPtr = _tt_get_unix_file_access(real_file);

    // Set the DB file access to be the same as the actual file.  This
    // allows an easy method of controlled message security
    if (!accessPtr.is_null()) {
      results = db->setFileAccess(real_file, accessPtr, accessPtr);
    }
  }
  
  _Tt_db_access_ptr accessPtr = new _Tt_db_access;
  accessPtr->user = _tt_uid;
  accessPtr->group = _tt_gid;

  // Create a new message info structure for the new message
  _Tt_db_message_info_ptr message_info = new _Tt_db_message_info;
  if (results == TT_DB_OK) {
    XDR  xdrs;

    // Get the current information on queued messages
    _Tt_db_property_ptr property;
    _Tt_string name = TT_DB_MESSAGE_INFO_PROPERTY;
    results = db->getFileProperty(real_file, name, accessPtr, property);

    // Default new message ID if there are none in the queue
    int message_id = 1;

    if (results == TT_DB_OK) {
      // Calculate the index for the last message info structure in
      // the property
      int index = property->values->count() - 1;

      if (index > -1) {
	// Un-XDR the message info into the "last_message_info" object
	// and set the new message ID to the message ID in in
	// "last_message_info" + 1.
	_Tt_string message_info_bytes = (*property->values) [index];
	xdrmem_create(&xdrs,
		      (char *)message_info_bytes,
		      (u_int)message_info_bytes.len(),
		      XDR_DECODE);
	_Tt_db_message_info_ptr last_message_info = new _Tt_db_message_info;
	(void)last_message_info->xdr(&xdrs);
	message_id = last_message_info->messageID + 1;
      }
    }

    // It doesn't matter if there were no queued messages, as long as
    // there were no fatal errors
    if ((results == TT_DB_OK) ||
	(results == TT_DB_ERR_NO_SUCH_PROPERTY)) {
      results = TT_DB_OK;

      // Put the new message info into the message info structure
      message_info->messageID = message_id;
      message_info->numParts = (args->message.body.body_len / ISMAXRECLEN) + 1;
      message_info->messageSize = args->message.body.body_len;
      _tt_get_rpc_strings(args->ptypes, message_info->ptypes);

      // Get the XDR size of the new message info structure
      u_int length;
      _Tt_xdr_size_stream xdrsz;
      if (!message_info->xdr((XDR *)xdrsz)) {
	results = TT_DB_ERR_ILLEGAL_MESSAGE;
      } else {
	length = (unsigned int) xdrsz.getsize();
      }

      // XDR the message info structure into "temp_string"
      _Tt_string temp_string((int)length);
      if (results != TT_DB_ERR_ILLEGAL_MESSAGE) {
        xdrmem_create(&xdrs, (char *)temp_string, length, XDR_ENCODE);
        if (!message_info->xdr(&xdrs)) {
	  results = TT_DB_ERR_ILLEGAL_MESSAGE;
        }
      }

      if (results != TT_DB_ERR_ILLEGAL_MESSAGE) {
	// Add the XDR'd message info to the message info property
	_Tt_db_property_ptr property = new _Tt_db_property;
	property->name = TT_DB_MESSAGE_INFO_PROPERTY;
	property->values->append(temp_string);

	results = db->addFileProperty(real_file, property, FALSE, accessPtr);
	property_written = TRUE;
      }
    }
  }
  
  if (results == TT_DB_OK) {
    int length = ISMAXRECLEN;

    // Break up the message into ISMAXRECLEN sized parts and store
    // as separate properties
    for (int i=0;
	 (results == TT_DB_OK) && (i < message_info->numParts);
	 i++) {
      // If this is the last part, it is probably shorter then
      // ISMAXRECLEN, so calculate the exact length
      if (i == message_info->numParts-1) {
	length = message_info->messageSize - (i * ISMAXRECLEN);
      }

      // Copy the message part into a buffer
      _Tt_string message_part(length);
      memcpy((char *)message_part,
	     args->message.body.body_val+i*ISMAXRECLEN,
	     length);

      // Construct a property name of the form:
      //
      //           _TT_MSG_<ID#>_<PART#>
      //
      char name [64];
      sprintf(name, TT_DB_MESSAGE_PROPERTY, message_info->messageID, i);

      // Store the property
      _Tt_db_property_ptr property = new _Tt_db_property;
      property->name = name;
      property->values->append(message_part);

      results = db->setFileProperty(real_file, property, accessPtr);
      property_written = TRUE;
    }
  }

  if (property_written && (results == TT_DB_OK)) {
    int cache_level;
    results = _tt_increment_file_properties_cache_level(db,
							real_file,
							accessPtr,
							cache_level);
  }

  return &results;
}

_tt_dequeue_msgs_results *
_tt_dequeue_messages_1 (_tt_dequeue_msgs_args *args,
			SVCXPRT * /* transp */)
{
  static _tt_dequeue_msgs_results results;
  results.messages.messages_val = (_tt_message *)NULL;
  results.messages.messages_len = 0;

  bool_t property_written = FALSE;

  _Tt_string real_file = args->file;
  _Tt_string partition = _tt_get_file_partition(args->file);
  _Tt_db_server_db_ptr db;
  results.results = _tt_get_partition_db(partition, db);

  if (results.results == TT_DB_OK) {
    _Tt_db_access_ptr accessPtr = _tt_get_unix_file_access(real_file);

    // Set the DB file access to be the same as the actual file.  This
    // allows an easy method of control for queued message security
    if (!accessPtr.is_null()) {
      results.results = db->setFileAccess(real_file,
					  accessPtr,
					  accessPtr);
    }
  }
  
  _Tt_db_access_ptr accessPtr = new _Tt_db_access;
  accessPtr->user = _tt_uid;
  accessPtr->group = _tt_gid;

  // Get the message info property
  _Tt_db_property_ptr property;
  if (results.results == TT_DB_OK) {
    _Tt_string name = TT_DB_MESSAGE_INFO_PROPERTY;
    results.results = db->getFileProperty(real_file,
					  name,
					  accessPtr,
					  property);
  }
  
  _Tt_db_message_info_ptr message_info = new _Tt_db_message_info;
  _Tt_string_list_ptr     messages;

  if (results.results == TT_DB_ERR_NO_SUCH_PROPERTY) {
    results.results = TT_DB_OK;
  }
  else if (results.results == TT_DB_OK) {
    _Tt_string_list_ptr dequeue_ptypes;
    _tt_get_rpc_strings(args->ptypes, dequeue_ptypes);

    _Tt_string_list_cursor dequeue_ptypes_cursor(dequeue_ptypes);

    // Loop through the message info property (effectively looping
    // through the message info entries)
    _Tt_string_list_cursor values_cursor(property->values);
    while (values_cursor.next()) {
      XDR xdrs;

      // Un-XDR a message info structure
      xdrmem_create(&xdrs,
		    (char *)*values_cursor,
		    (*values_cursor).len(),
		    XDR_DECODE);
      (void)message_info->xdr(&xdrs);

      // Eliminate all of the ptypes from the message info list
      // that match the callers ptype list.
      bool_t ptype_matched = FALSE;
      _Tt_string_list_cursor ptypes_cursor(message_info->ptypes);
      while (ptypes_cursor.next()) {
	while (dequeue_ptypes_cursor.next()) {
	  if (*dequeue_ptypes_cursor == *ptypes_cursor) {
	    ptypes_cursor.remove();
	    ptype_matched = TRUE;
	    break;
	  }
	}
      }

      // A ptype matched, this message is to be returned...
      if (ptype_matched) {
	if (messages.is_null()) {
	  messages = new _Tt_string_list;
	}

	// Reconstruct the message from the _TT_MSG_<ID#>_<PART#> properties
	_Tt_string message(message_info->messageSize);
	for (int i=0; (results.results == TT_DB_OK) &&
	              (i < message_info->numParts); i++) {
	  // Construct the property name
	  char name [64];
	  sprintf(name, TT_DB_MESSAGE_PROPERTY, message_info->messageID, i);

	  // Get the property value
	  _Tt_db_property_ptr property;
	  results.results = db->getFileProperty(real_file,
						name,
						accessPtr,
						property);
	  if (results.results == TT_DB_OK) {
	    _Tt_string message_bytes = (*property->values) [0];

	    // Copy each succesive part into a large buffer
	    memcpy((char *)message+i*ISMAXRECLEN,
		   (char *)message_bytes,
		   message_bytes.len());
	  }
	}

	// Append the re-assembled message to the return list
	if (results.results == TT_DB_OK) {
	  messages->append(message);
	}
      }

      // No more ptypes left for this message, dequeue it...
      if (message_info->ptypes->is_empty()) {
	values_cursor.remove();
      }
      // Otherwise, update the property value with an updated version
      // of the message_info structure
      else {
        u_int  length;
        XDR    xdrs;
        _Tt_xdr_size_stream xdrsz;

        // Get the XDR size of the updated message info structure
        (void)message_info->xdr((XDR *)xdrsz);
        length = (unsigned int) xdrsz.getsize();

      	// XDR the message info structure into "temp_string"
      	_Tt_string temp_string((int)length);
      	xdrmem_create(&xdrs, (char *)temp_string, length, XDR_ENCODE);
      	if (!message_info->xdr(&xdrs)) {
	  // Update the property value
	  *values_cursor = temp_string;
	}
      }
    }

    // Put the message info property back into the DB.  If all of
    // the message info structures have been deleted from the
    // property, then this should effectively delete the property.
    results.results = db->setFileProperty(real_file,
					  property,
					  accessPtr);
    property_written = TRUE;

    if (!messages.is_null()) {
      if ((results.results == TT_DB_OK) && !messages->is_empty()) {
        // Allocate enough space to transport the messages back to the
        // caller
        results.messages.messages_val = (_tt_message *)
	                                malloc(sizeof(_tt_message)*
		  			       messages->count());
        results.messages.messages_len = messages->count();

        // Put the messages into the results structure
        int i = 0;
        _Tt_string_list_cursor messages_cursor(messages);
        while (messages_cursor.next()) {
	  results.messages.messages_val [i].body.body_val =
	    (char *)malloc((*messages_cursor).len());
	  results.messages.messages_val [i].body.body_len =
	    (*messages_cursor).len();
	  memcpy(results.messages.messages_val [i].body.body_val,
	         (char *)*messages_cursor,
	         (*messages_cursor).len());
	  i++;
        }
      }
    }
  }

  if (property_written && (results.results == TT_DB_OK)) {
    int cache_level;
    results.results = _tt_increment_file_properties_cache_level(db,
							        real_file,
							        accessPtr,
							        cache_level);
  }

  return &results;
}

// Since we're calling the wrapped API calls, bringing in tt_c.h
// doesn't get us the declarations we need, so we have to bring
// these two in manually like this.
//
extern char *		_tt_file_netfile(const char *);
extern char *		_tt_netfile_file(const char *);
extern Tt_status	_tt_pointer_error(void *p);
extern char *		_tt_status_message(Tt_status s);


// Call the API routine _tt_file_netfile() and return the
// results.
_tt_file_netfile_results *
_tt_file_netfile_1 (_tt_file_netfile_args *args, SVCXPRT * /* transp */)
{

#ifdef notdef
printf("DEBUG: SERVER: _tt_file_netfile_1: calling _tt_file_netfile(%s)\n",
	(char *) args->file_or_netfile);
#endif

	static _tt_file_netfile_results results;
	static char * canonical_path;

	// we have a netfilename, get the local file version...
	canonical_path = _tt_file_netfile(args->file_or_netfile);

#ifdef notdef
printf("DEBUG: SERVER: _tt_file_netfile_1: _tt_file_netfile(%s) returned %s\n",
	args->file_or_netfile, canonical_path);
#endif


	if (_tt_pointer_error(canonical_path) != TT_OK) {
		results.results = TT_DB_ERR_ILLEGAL_FILE;
		results.result_string = '\0';
	} else {
		results.results = TT_DB_OK;
		results.result_string = canonical_path;
	}
	results.tt_status = (int) _tt_pointer_error(canonical_path);

#ifdef notdef
printf("\t results.tt_status == %s\n",
	_tt_status_message(_tt_pointer_error(canonical_path)));
#endif

	return &results;
}


// Call the API routine _tt_netfile_file() and return the
// results.
_tt_file_netfile_results *
_tt_netfile_file_1 (_tt_file_netfile_args *args, SVCXPRT * /* transp */)
{

#ifdef notdef
printf("DEBUG: SERVER: _tt_netfile_file_1: calling _tt_netfile_file(%s)\n",
	(char *) args->file_or_netfile);
#endif

	static _tt_file_netfile_results results;
	static char * canonical_path;

	// we have a netfilename, get the local file version...
	canonical_path = _tt_netfile_file(args->file_or_netfile);

#ifdef notdef
printf("DEBUG: SERVER: _tt_netfile_file_1: _tt_netfile_file(%s) returned %s\n",
	args->file_or_netfile, canonical_path);
#endif

	if (_tt_pointer_error(canonical_path) != TT_OK) {
		results.results = TT_DB_ERR_ILLEGAL_FILE;
		results.result_string = '\0';
	} else {
		results.results = TT_DB_OK;
		results.result_string = canonical_path;
	}
	results.tt_status = (int) _tt_pointer_error(canonical_path);

#ifdef notdef
printf("\t results.tt_status == %s\n",
	_tt_status_message(_tt_pointer_error(canonical_path)));
#endif

	return &results;
}


//
// Delete the named session from the properties table.
//
_tt_delete_session_results	*
_tt_delete_session_1(_tt_delete_session_args	*args,
		     SVCXPRT			* /*NOTUSED*/)
{
    static _tt_delete_session_results	results;			 
    Table_oid_prop		 	record;

    u_int			 	fileOffset;
    int				 	propLen = strlen(propTable);
    int				 	isfd;

    char				*lastSlash;
    char				*pathName;

    results.tt_status = TT_DB_OK;

    //
    // For each property_table that we manage,
    // Compare the sessionID with the session that
    // we just found to be dead, and delete it.
    //
    for (fileOffset = 0; fileOffset < _TT_MAX_ISFD; fileOffset++) {

	pathName = _tt_db_table[fileOffset].db_path;

	if (pathName && strlen(pathName) > 0) {
	    if (_tt_db_table[fileOffset].client_has_open
		|| _tt_db_table[fileOffset].server_has_open) {
				
		//
		// Is the file name ".../property_table*" ?
		//
		lastSlash = strrchr(pathName, '/');
		if (lastSlash) {
		    lastSlash++;
		    if (strncmp(propTable,lastSlash,propLen)==0) {
			
			// Get the FD and process the file.
			isfd=cached_isopen(pathName, ISINOUT);
			
			//
			// Get the 1st record.
			//
			LOCK_RPC();
			isread(isfd, (char *)&record, ISFIRST);
			((char *)(&record))[isreclen] = '\0';

			// Delte the named session.
			if (strcmp(sesProp, record.propname) == 0) {
			  if (strcmp(args->session.value, record.propval) == 0) {
			    isdelcurr(isfd);
			    isfsync(isfd);
			  }
			}

			// Unconditionally delete ALL _MODIFICATION_DATE's
			if (strcmp(modDate, record.propname) == 0) {
			  isdelcurr(isfd);
			  isfsync(isfd);
			}
			
			UNLOCK_RPC();
						
			do {
			    LOCK_RPC();
			    if (isread(isfd, (char *)&record,ISNEXT) != 0) {
				UNLOCK_RPC();
				break;
			    }
			    ((char *)(&record))[isreclen] = '\0';
			    if (strcmp(sesProp, record.propname) == 0) {
				if(strcmp(args->session.value,
					  record.propval) == 0) {
				    isdelcurr(isfd);
				    isfsync(isfd);
				}
			    }

			    // Unconditionally delete ALL
			    // 	_MODIFICATION_DATE's
			    if (strcmp(modDate, record.propname) == 0) {
			      isdelcurr(isfd);
			      isfsync(isfd);
			    }
			    UNLOCK_RPC();
			} while(TRUE);
			cached_isclose(isfd);
		    }
		}
	    }
	}
    }
    return(&results);
}

//
// *All* is an over statment.
// It returns up to OPT_MAX_GET_SESSIONS sessions and the oidkey to use
// to resume.
//
_tt_get_all_sessions_results *
_tt_get_all_sessions_1(_tt_get_all_sessions_args * args,
		       SVCXPRT			 * /*NOTUSED*/)
{
	static _tt_get_all_sessions_results	results;

	static _Tt_string_list		  *list;
	list = new _Tt_string_list;

	int			  offset;
	int			  isfd;
	int			  recordCount = 0;
	int			  propLen = strlen(propTable);

	char			* lastSlash;
	char			* pathName;

	_Tt_string		  propValue;
	Table_oid_prop		  record;

	//
	// Empty out any existing session or key results.
	//
	list->flush();
	if (results.oidkey.oidkey_val != NULL) {
		free((char *)results.oidkey.oidkey_val);
	}
	for (offset = 0 ; offset < list->count() ; offset++) {
		if (results.session_list.values_val[offset].value != NULL) {
			free(results.session_list.values_val[offset].value);
		}
	}
	if (results.session_list.values_val != NULL) {
		free((char *)results.session_list.values_val);
		results.session_list.values_val = NULL;
	}
	memset(&results.oidkey, '\0', sizeof(results.oidkey));

	//
	// For each property_table that we manage,
	// pull out all of the session-ids and pass them back (up
	// to OPT_MAX_GET_SESSIONS passed back in each call)
	//
	for (offset = 0 ; offset < _TT_MAX_ISFD; offset++) {

	    pathName = _tt_db_table[offset].db_path;

	    if (pathName && strlen(pathName) > 0) {
		if (_tt_db_table[offset].client_has_open
		    || _tt_db_table[offset].server_has_open) {

		    //
		    // Is the file name ".../property_table*" ?
		    //
		    lastSlash = strrchr(pathName, '/');
		    if (lastSlash) {
			lastSlash++;
			if (strncmp(propTable, lastSlash, propLen) == 0) {
						
			    // Get the FD and process the file.
			    isfd = cached_isopen(pathName, ISINOUT);

			    //
			    // If the user passed in a starting key,
			    // then use it.
			    //
			    if (args->oidkey.oidkey_len > 0
				&& args->oidkey.oidkey_val != NULL) {
				issetcurpos(isfd,
					    (char *)args->oidkey.oidkey_val);
			    }
			    
			    //
			    // Get the 1st record.
			    //
			    isread(isfd, (char *)&record, ISFIRST);
			    ((char *)(&record))[isreclen] = '\0';
			    if (strcmp(sesProp, record.propname) == 0) {
				propValue = record.propval;
				
				// Append it to the list to send back.
				recordCount++;
				list->append(propValue);
				memset(&record, '\0', sizeof(record));
			    }
			    
			    while(isread(isfd, (char *)&record,ISNEXT) != -1) {
			      ((char *)(&record))[isreclen] = '\0';
				if (strcmp(sesProp, record.propname) == 0) {
				    propValue = record.propval;
				    list->append(propValue);
				    if (++recordCount > OPT_MAX_GET_SESSIONS-1) {
					results.oidkey.oidkey_val = NULL;
					isgetcurpos(isfd,
						    (int *)&results.oidkey.oidkey_len,
						    (char **)&results.oidkey.oidkey_val);
					break;
					
				    }
				}
				memset(&record, '\0', sizeof(record));
			    }
			    cached_isclose(isfd);
			    if (recordCount > OPT_MAX_GET_SESSIONS-1) {
				break;
			    }
			}
		    }
		}
	    }
	}
	results.session_list.values_len = list->count();
	if (results.session_list.values_len > 0) {
	  results.session_list.values_val = (_tt_string *)malloc(sizeof(_tt_string *) * list->count());
	} else {
	  results.session_list.values_val = (_tt_string *)NULL;
	}
	for (offset = 0 ; offset < results.session_list.values_len ; offset++) {
		propValue = list->top();
		results.session_list.values_val[offset].value = strdup(propValue);
		list->pop();
	}
	
	return(&results);
}

_tt_garbage_collect_results *
_tt_garbage_collect_1(void	* /*NOTUSED*/,
		      SVCXPRT	* /*NOTUSED*/)
{
	static _tt_garbage_collect_results	results;


	memset(&results, '\0', sizeof(_tt_garbage_collect_results));

#if defined(OPT_GARBAGE_THREADS)
	int	id;

	//
	// Collect the garbage and delete old sessions in
	// a seperate thread. As soon as the thread is started
	// this function returns and the dbserver is again
	// processing user requests.
	id = _tt_run_garbage_collect(OPT_GARBAGE_IN_PARALLEL);
	results.tt_status = (id >= 0) ? TT_OK : TT_ERR_INTERNAL;
#else
	//
	// Without threads, just compress (isgarbage) the 
	// db files. The user program will check for and
	// delete old sessions.
	//
	isgarbage_collect();
	results.tt_status = TT_OK;
#endif
	return(&results);
}


//
// ******* Static helper functions start here *******
//

bool_t _tt_is_file_a_directory (const _Tt_string &file)
{
  // This is sometimes called with a network path and a non-network path.
  // Make sure we always give stat a non-network path.
  char *slash = strchr((char *)file, '/');
  char *path = strchr((char *)file, ':');
  if (path != slash-1) {
    path = (char *)NULL;
  }
  DIR *dd = opendir(path ? path+1 : (char *)file);

  if (dd) {
    (void)closedir(dd);
  }

  return (dd ? TRUE : FALSE);
}

// Replace the old partition with the new partition in the object ID
static _Tt_string _tt_make_equivalent_object_id (const _Tt_string &objid,
						 const _Tt_string &partition)
{
  _Tt_string temp_string = (char *)objid;
  _Tt_string new_objid;

  temp_string = temp_string.rsplit (':', new_objid);
  new_objid = new_objid.cat(partition);

  return new_objid;
}

static _Tt_db_results _tt_get_partition_db (const _Tt_string     &partition,
					    _Tt_db_server_db_ptr &db)
{
  _Tt_db_partition_global_map_ref db_map;
  _Tt_db_results                  results = TT_DB_OK;

  db = db_map.getDB(partition);
  if (db.is_null()) {
    results = TT_DB_ERR_DB_OPEN_FAILED;
  }

  return results;
}

static _Tt_db_access_ptr _tt_get_real_rpc_access (const _tt_access &rpc_access)
{
  _Tt_db_access_ptr accessPtr;
  _tt_get_rpc_access(rpc_access, accessPtr);

  accessPtr->user = _tt_uid;
  accessPtr->group = (gid_t)-1;

  // Make sure the group ID is valid before using it
  for (int i=0; i < _tt_gidlen; i++) {
    if (_tt_gidlist [i] == _tt_gid) {
      accessPtr->group = _tt_gid;
    }
  }

  return accessPtr;
}

static _Tt_db_access_ptr _tt_get_file_access (const _Tt_string &file,
					      const _tt_access &rpc_access)
{
  _Tt_db_access_ptr accessPtr = _tt_get_unix_file_access(file);

  if (accessPtr.is_null()) {
    accessPtr = _tt_get_real_rpc_access(rpc_access);
  }
  else {
    accessPtr->mode = (mode_t)-1;
  }

  return accessPtr;
}

static _Tt_db_access_ptr _tt_get_unix_file_access (const _Tt_string &file)
{
  _Tt_db_access_ptr accessPtr;

  struct stat stat_buf;

  // Make sure we always give stat a non-network path.
  _Tt_string local_path = file;

  if (_tt_is_network_path(file)) {
    _Tt_string temp_string;
    local_path = local_path.split(':', temp_string);
  }

  if (!stat((char *)local_path, &stat_buf)) {
    accessPtr = new _Tt_db_access;
    accessPtr->user = stat_buf.st_uid;
    accessPtr->group = stat_buf.st_gid;
    accessPtr->mode = stat_buf.st_mode;
  }

  return accessPtr;
}

static _Tt_string _tt_get_file_partition (const _Tt_string &file)
{
  _Tt_string temp_string;
  _Tt_string local_path = file;
  local_path = local_path.split(':', temp_string);

  _Tt_file_system           fs;
  _Tt_file_system_entry_ptr entry = fs.bestMatchToPath(local_path);

  return entry->getMountPoint();
}

static _Tt_db_results
_tt_increment_file_properties_cache_level
  (const _Tt_db_server_db_ptr &db,
   const _Tt_string           &file,
   const _Tt_db_access_ptr    &accessPtr,
   int                        &cache_level)
{
  cache_level = -1;

  _Tt_db_property_ptr property;
  _Tt_db_results results =
    db->getFileProperty(file,
			TT_DB_PROPS_CACHE_LEVEL_PROPERTY,
			accessPtr,
			property);

  if (results == TT_DB_OK) {
    _Tt_string cache_level_bytes = (*property->values) [0];
    memcpy ((char *)&cache_level, (char *)cache_level_bytes, sizeof(int));

    cache_level++;
    memcpy ((char *)cache_level_bytes, (char *)&cache_level, sizeof(int));
    (*property->values) [0] = cache_level_bytes;

    results = db->setFileProperty(file,
				  property,
				  accessPtr);
    if (results != TT_DB_OK) {
      cache_level = -1;
      results = TT_DB_ERR_PROPS_CACHE_ERROR;
    }
  }
  else if (results == TT_DB_ERR_NO_SUCH_PROPERTY) {
    cache_level = 0;

    _Tt_string value(sizeof(int));
    memcpy((char *)value, (char *)&cache_level, sizeof(int));

    _Tt_db_property_ptr property = new _Tt_db_property;
    property->name = TT_DB_PROPS_CACHE_LEVEL_PROPERTY;
    property->values->append(value);

    results = db->setFileProperty(file,
				  property,
				  accessPtr);
    if (results != TT_DB_OK) {
      cache_level = -1;
      results = TT_DB_ERR_PROPS_CACHE_ERROR;
    }
  }
  else {
    results = TT_DB_ERR_PROPS_CACHE_ERROR;
  }

  return results;
}

static _Tt_db_results 
_tt_get_file_properties_cache_level (const _Tt_db_server_db_ptr &db,
				     const _Tt_string           &file,
				     const _Tt_db_access_ptr    &accessPtr,
				     int                        &cache_level)
{
  cache_level = -1;

  _Tt_db_property_ptr property;
  _Tt_db_results results =
    db->getFileProperty(file,
			TT_DB_PROPS_CACHE_LEVEL_PROPERTY,
			accessPtr,
			property);
  if (results == TT_DB_OK) {
    _Tt_string cache_level_bytes = (*property->values) [0];
    memcpy ((char *)&cache_level, (char *)cache_level_bytes, sizeof(int));
  }
  // The file was probably created for an object.  The cache level
  // was never stored as a property...
  else if (results == TT_DB_ERR_NO_SUCH_PROPERTY) {
    results = _tt_increment_file_properties_cache_level(db,
						        file,
							accessPtr,
							cache_level);
  }
  else {
    results = TT_DB_ERR_PROPS_CACHE_ERROR;
  }

  return results;
}

static _Tt_string _tt_get_object_partition (const _Tt_string &objid)
{
  _Tt_string partition = (char *)objid;
  _Tt_string temp_string;

  // Get rid of file system type and hostname - the partition
  // is the only thing left
  partition = partition.split(':', temp_string);
  partition = partition.split(':', temp_string);
  partition = partition.split(':', temp_string);

  return partition;
}

static _Tt_db_results
_tt_increment_object_properties_cache_level
  (const _Tt_db_server_db_ptr &db,
   const _Tt_string           &objid,
   const _Tt_db_access_ptr    &accessPtr,
   int                        &cache_level)
{
  cache_level = -1;

  _Tt_db_property_ptr property;
  _Tt_db_results results =
    db->getObjectProperty(objid,
			  TT_DB_PROPS_CACHE_LEVEL_PROPERTY,
			  accessPtr,
			  property);

  if (results == TT_DB_OK) {
    _Tt_string cache_level_bytes = (*property->values) [0];
    memcpy ((char *)&cache_level, (char *)cache_level_bytes, sizeof(int));

    cache_level++;
    memcpy ((char *)cache_level_bytes, (char *)&cache_level, sizeof(int));
    (*property->values) [0] = cache_level_bytes;

    results = db->setObjectProperty(objid,
				    property,
				    accessPtr);
    if (results != TT_DB_OK) {
      cache_level = -1;
      results = TT_DB_ERR_PROPS_CACHE_ERROR;
    }
  }
  else if (results == TT_DB_ERR_NO_SUCH_PROPERTY) {
    cache_level = 0;

    _Tt_string value(sizeof(int));
    memcpy((char *)value, (char *)&cache_level, sizeof(int));

    _Tt_db_property_ptr property = new _Tt_db_property;
    property->name = TT_DB_PROPS_CACHE_LEVEL_PROPERTY;
    property->values->append(value);

    results = db->setObjectProperty(objid,
				    property,
				    accessPtr);
    if (results != TT_DB_OK) {
      cache_level = -1;
      results = TT_DB_ERR_PROPS_CACHE_ERROR;
    }
  }
  else {
    results = TT_DB_ERR_PROPS_CACHE_ERROR;
  }

  return results;
}

static _Tt_db_results 
_tt_get_object_properties_cache_level (const _Tt_db_server_db_ptr &db,
				       const _Tt_string           &objid,
				       const _Tt_db_access_ptr    &accessPtr,
				       int                        &cache_level)
{
  cache_level = -1;

  _Tt_db_property_ptr property;
  _Tt_db_results results = 
    db->getObjectProperty(objid,
			  TT_DB_PROPS_CACHE_LEVEL_PROPERTY,
			  accessPtr,
			  property);

  if (results == TT_DB_OK) {
    _Tt_string cache_level_bytes = (*property->values) [0];
    memcpy ((char *)&cache_level, (char *)cache_level_bytes, sizeof(int));
  }
  else {
    results = TT_DB_ERR_PROPS_CACHE_ERROR;
  }

  return results;
}

static void
_tt_screen_object_properties (_Tt_db_property_list_ptr &properties)
{
  _Tt_db_property_list_cursor properties_cursor (properties);
  while (properties_cursor.next ()) {
    if (properties_cursor->name ==  TT_DB_PROPS_CACHE_LEVEL_PROPERTY) {
      properties_cursor.remove ();
    }
    else if (properties_cursor->name == TT_DB_OBJECT_TYPE_PROPERTY) {
      properties_cursor.remove ();
    }
  }

  if (properties->count () == 0) {
    properties = (_Tt_db_property_list *)NULL;
  }
}

static _Tt_string _tt_get_local_path (const _Tt_string &network_path,
				      _Tt_string       &hostname,
				      _Tt_string       &partition)
{
  _Tt_string temp_string = network_path;
  _Tt_string local_path = _tt_realpath (temp_string.split(':', hostname));

  _Tt_file_system file_system;
  _Tt_file_system_entry_ptr entry = file_system.bestMatchToPath(local_path);

  partition = entry->getMountPoint();

  _Tt_string loop_back_mount_point = entry->getLoopBackMountPoint();
  if (loop_back_mount_point.len()) {
    // Get the path info after the loop back mount point path
    local_path = local_path.right(local_path.len()-loop_back_mount_point.len());

    // Replace the loop back mount point path with the partition path
    local_path = partition.cat(local_path);
  }

  return local_path;
}

//
// This is the thread that performs the garbage collection.
// It is defined as a (void *) function for thr_create() compatibility.
//
static void	*
_tt_garbage_collect(void * /*NOTUSED*/)
{
	// thr_create() func returns (void *).
	void			* results = NULL;

	_tt_get_all_sessions_args	 args;

	_Tt_string			 sessionId;
	_Tt_string_list		 	*sessions;

	memset(&args, '\0', sizeof(args));

#if defined(OPT_GARBAGE_THREADS)

	sigset_t	new_thr_sigset;

	//
	// Tell ourself (this thread only) to ignore all SIGs, or quit.
	//
	if (sigfillset(&new_thr_sigset) != 0) {
		return(results);
	}

	if (thr_sigsetmask(SIG_BLOCK, &new_thr_sigset, NULL) < 0) {
		return(results);
	}

	LOCK_RPC();
	isgarbage_collect();		// 1st compress the DB files.
	UNLOCK_RPC();
#else
	_Tt_db_client_ptr	dbClient;

	if (!dbClient.is_null() 
	    && dbClient->getConnectionResults() == TT_DB_OK) {
	  	// Tell server to compress the files.
		dbClient->garbage_collect_in_server();
#endif

		// Get a list of all sessions.
		do {
			LOCK_RPC();
#if defined(OPT_GARBAGE_THREADS)
			_tt_get_all_sessions_results	*sessionList;

			sessionList = _tt_get_all_sessions_1(&args, NULL);
	    
			int			offset;
			_Tt_string		oneSession;

			sessions = new _Tt_string_list;

			for (offset = 0
			     ; offset < sessionList->session_list.values_len
			     ; offset++) {
				oneSession = sessionList->session_list.values_val[offset].value;
				sessions->append(oneSession);
			}
#else
			sessions = dbClient->get_all_sessions();
#endif
			if (sessions== NULL	|| sessions->count() == 0) {
				UNLOCK_RPC();
				break;
			}
	    
	    	UNLOCK_RPC();

			// Delete the list of sessions that are dead.
			do {
				Tt_status			ttstatus;
#if defined(OPT_GARBAGE_THREADS)
				_tt_delete_session_args		delsession;
#endif

				sessionId = sessions->top();
				if ((ttstatus=tt_default_session_set(sessionId)) != TT_OK) {

#if defined(OPT_GARBAGE_THREADS)
					delsession.session.value = sessionId;
					_tt_delete_session_1(&delsession,NULL);
#else
					dbClient->delete_session(sessionId);
#endif
				}
				sessions->pop();
			} while(sessions->count() > 0);

#if defined(OPT_GARBAGE_THREADS)
			//
			// Copy over the re-start key.
			// (for more than OPT_MAX_GET_SESSIONS).
			memcpy(&args.oidkey,
			       &sessionList->oidkey,
			       sizeof(args.oidkey));
#endif
		} while (args.oidkey.oidkey_len != 0);

#if defined(OPT_GARBAGE_THREADS)
		mutex_unlock(&garbage_run_in_process);
#else
	}
#endif
	return(results);
}

//
// Return the PID or TID of the running garbage collection function.
//
int
_tt_run_garbage_collect(int in_parallel)
{
	extern FILE	*errstr;

	/* Make sure in_parallel is used to quiet warnings */
	if (in_parallel) {}

#if defined(OPT_GARBAGE_THREADS)
	static int	mutex_inited = 0;

	// Error back from mutex_*() and thr_() funcs.
	int		err;

	// Times to try thr_create() if it return with EAGAIN.
	int		create_tries = OPT_SOLARIS_THREADED_TRIES;

	if (!mutex_inited) {
		mutex_inited = 1;
		// Init the RPC syncronazation mutex lock.
		mutex_init(&rpc_client_busy, USYNC_THREAD, 0);
		mutex_init(&garbage_run_in_process, USYNC_PROCESS, 0);

		// Raise the priority of ourselfs to be higher
		// than our new thread.
		thr_setprio(thr_self(), 10);
	}

	//
	// See if anyone else is running, if so, then do not run
	// gabage collection again.
	//
	if (mutex_trylock(&garbage_run_in_process) != 0) {
		return (_tt_garbage_id);
	}

	//

	//
	// Start the thread and keep trying OPT_SOLARIS_THREADED_TRIES,
	// or until it works.
	//
	while ((err = thr_create((void *)0,	// stack_base - use default.
				 (size_t)0,	// stack_size - use default.
				 _tt_garbage_collect,
		 (void *)_TT_GARBAGE_COLLECTION_FREQUENCY,// Arg to func.
				 THR_SUSPENDED|THR_BOUND,
				 (thread_t *)&_tt_garbage_id)) < 0) {

		if (errno == EAGAIN && (--create_tries > 0)) {
			thr_yield();
			continue;
		}

		_tt_garbage_id = -2;
		//
		// Get here only on thr_create() error.
		// Unable to create thread.
		//
		_tt_syslog(errstr, LOG_ERR, "%s",
			   catgets(_ttcatd, 5, 8,
			   "Unable to start garbage collection thread. thr_create()\n"));
	}


	//
	// If we are to garbage collect in parallel, then
	// let the garbage thread continue at a low priority.
	//
	// If we are not to garbage collect in parallel,
	// then let the thread run, then the main thread exits.
	//
	if (in_parallel == TRUE) {
		if (_tt_garbage_id > 0) {

			// Lower the priority of garbage collection to lowest.
			thr_setprio(_tt_garbage_id, 0);

			// And start it.
			thr_continue((thread_t)_tt_garbage_id);
		}
	} else {
		if (_tt_garbage_id > 0) {
			thr_continue((thread_t)_tt_garbage_id);
			thr_yield();
			thr_exit(0);
		} else {
			thr_exit(0);
		}
	}

#else //defined(OPT_GARBAGE_THREADS)

#if defined(OPT_AUTO_GARBAG_COLLECT)
	//
	// Do not start another, if one is running.
	//
	if (in_parallel == TRUE) {
	    if (_tt_garbage_id == -1) {
		//
		// FORK and EXEC ourself '-G'.
		//
#if defined(OPT_BUG_AIX) || defined(OPT_BUG_USL) || defined(OPT_BUG_UXP)
#define vfork fork
#endif
		switch (_tt_garbage_id = (int)vfork()) {
		      case 0:		// child
			{
				const char *newargv[3];

				newargv[0] = global_argv[0];
				newargv[1] = "-G";
				newargv[2] = NULL;
				
				execve((const char *)newargv[0],
					(char *const *)newargv,
					(char *const *)global_envp);

				_tt_syslog(errstr, LOG_ERR, "%s",
					   catgets(_ttcatd, 5, 9,
				"Unable to fork() for garbage collection.\n"));
				_tt_garbage_id = -3;
				_exit(1);	// ERROR, so exit new child.
			}
			break;

		      case -1:		// Error.
			_tt_syslog(errstr, LOG_ERR, "%s",
				   catgets(_ttcatd, 5, 9,
			    "Unable to fork() for garbage collection.\n"));
			_tt_garbage_id = -4;
			break;

		      default:		// Parent.
			break;
		    }
	    }
	} else {
#endif /* OPT_AUTO_GARBAGE_COLLECT*/
	    exit((intptr_t)_tt_garbage_collect(NULL));
#if defined(OPT_AUTO_GARBAG_COLLECT)
	}
#endif /* OPT_AUTO_GARBAGE_COLLECT*/

#endif // ! OPT_GARBAGE_THREADS
	return (_tt_garbage_id);
}

