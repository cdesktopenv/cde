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
//%%  $XConsortium: old_db_server_functions.C /main/3 1995/10/23 09:58:54 rswiston $ 			 				
/*
 * @(#)old_db_server_functions.C	1.13 93/09/07
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 *
 * This file contains the functions that implement all of the new RPC-like
 * functions required for old DB compatibility.
 */

#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

#include "util/tt_file_system.h"
#include "util/tt_file_system_entry.h"
#include "util/tt_path.h"
#include "util/tt_xdr_utils.h"
#include "db/db_server.h"
#include "db/tt_db_access.h"
#include "db/tt_db_property.h"
#include "db/tt_db_access_utils.h"
#include "db/tt_db_property_utils.h"
#include "db/tt_db_rpc_routines.h"
#include "db/tt_old_db_message_info_utils.h"
#include "db/tt_old_db_partition_map_ref.h"
#include "db/tt_old_db_consts.h"
#include "db/tt_old_db_utils.h"
#include "api/c/tt_c.h"

static void _old_tt_close_db_partition_files (const _Tt_string&);
static bool_t _old_tt_is_file_a_directory (const _Tt_string&);
static _Tt_string _old_tt_make_equivalent_object_id(const _Tt_string &objid,
						    const _Tt_string &partition);
static _Tt_db_results _old_tt_get_partition_db (const _Tt_string&,
						_Tt_old_db_ptr&,
					        const _Tt_db_client_ptr &);
static _Tt_db_access_ptr
       _old_tt_get_real_rpc_access (const _tt_access &rpc_access);
static _Tt_db_access_ptr _old_tt_get_file_access (const _Tt_string &file,
						  const _tt_access &rpc_access);
static _Tt_string
       _old_tt_get_file_partition (const _Tt_string        &file,
				   const _Tt_db_client_ptr &db_connection);
static _Tt_db_results
       _old_tt_increment_file_properties_cache_level (const _Tt_old_db_ptr&,
						      const _Tt_string&,
						      const _Tt_db_access_ptr&,
						      int&);
static _Tt_db_results
       _old_tt_get_file_properties_cache_level (const _Tt_old_db_ptr&,
						const _Tt_string&,
						const _Tt_db_access_ptr&,
						int&);
static _Tt_string _old_tt_get_object_partition (const _Tt_string &objid);
static _Tt_db_results
_old_tt_increment_object_properties_cache_level (const _Tt_old_db_ptr&,
						 const _Tt_string&,
						 const _Tt_db_access_ptr&,
						 int&);
static _Tt_db_results
       _old_tt_get_object_properties_cache_level (const _Tt_old_db_ptr&,
						  const _Tt_string&,
						  const _Tt_db_access_ptr&,
						  int&);
static void _old_tt_screen_object_properties (_Tt_db_property_list_ptr&);

_tt_file_partition_results *_tt_get_file_partition_1 (char **file,
						      const _Tt_db_client_ptr &db_conn)
{
  static _tt_file_partition_results results;
  _Tt_string partition = _old_tt_get_file_partition(*file, db_conn);

  results.partition = strdup((char *)partition);
  results.network_path = strdup(*file);
  results.results = TT_DB_OK;

  return &results;
}

_tt_db_cache_results *_tt_create_file_1 (_tt_create_file_args *args,
					 const _Tt_db_client_ptr &db_conn)
{
  static _tt_db_cache_results results;
  results.cache_level = -1;

  _Tt_string real_file = args->file;
  _Tt_string partition = _old_tt_get_file_partition(args->file, db_conn);
  _Tt_old_db_ptr db;
  results.results = _old_tt_get_partition_db(partition, db, db_conn);

  _Tt_db_access_ptr access = _old_tt_get_file_access(real_file, args->access);

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
	  _old_tt_increment_file_properties_cache_level(db,
							real_file,
							access,
							results.cache_level);
      }
    }
  }

  _old_tt_close_db_partition_files (partition);
  return &results;
}

_tt_db_cache_results *_tt_create_obj_1 (_tt_create_obj_args *args,
					const _Tt_db_client_ptr &db_conn)
{
  static _tt_db_cache_results results;
  results.cache_level = -1;

  _Tt_string partition = _old_tt_get_object_partition(args->objid);
  _Tt_old_db_ptr db;
  results.results = _old_tt_get_partition_db(partition, db, db_conn);

  _Tt_string        real_file = args->file;
  _Tt_db_access_ptr object_access = _old_tt_get_real_rpc_access(args->access);

  if (results.results == TT_DB_OK) {
    // See if the object already exists by trying to get the forward
    // pointer property
    _Tt_db_property_ptr temp_property;
    _Tt_db_results temp_results =
      db->getObjectProperty(args->objid,
			    TT_OLD_DB_FORWARD_POINTER_PROPERTY,
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
    _Tt_db_access_ptr file_access = _old_tt_get_file_access(real_file,
							    args->access);
    results.results = db->createObject(real_file,
				       args->objid,
				       object_access,
				       file_access);
    
    if (results.results == TT_DB_OK) {
      _Tt_db_property_list_ptr properties;
      _tt_get_rpc_properties(args->properties, properties);
      
      if (!properties.is_null()) {
	_old_tt_screen_object_properties (properties);
      }

      if (!properties.is_null()) {
	results.results = db->setObjectProperties(args->objid,
						  properties,
						  object_access);
      }
      
      // Set the otype of the object
      if (results.results == TT_DB_OK) {
	_Tt_db_property_ptr property = new _Tt_db_property;
	property->name = TT_OLD_DB_OBJECT_TYPE_PROPERTY;
	property->values->append(_Tt_string(args->otype));

	results.results = db->setObjectProperty(args->objid,
						property,
						object_access);
      }

      if (results.results == TT_DB_OK) {
	results.results =
	  _old_tt_increment_object_properties_cache_level(db,
							  args->objid,
							  object_access,
							  results.cache_level);
      }
    }
  }

  _old_tt_close_db_partition_files (partition);
  return &results;
}

_tt_db_results *_tt_remove_file_1 (_tt_remove_file_args *args,
				   const _Tt_db_client_ptr &db_conn)
{
  static _tt_db_results results;

  _Tt_db_access_ptr access = _old_tt_get_real_rpc_access(args->access);

  _Tt_string real_file = args->file;
  _Tt_string partition = _old_tt_get_file_partition(args->file, db_conn);
  _Tt_old_db_ptr db;
  results = _old_tt_get_partition_db(partition, db, db_conn);

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
	results = db->removeFile(*children_cursor, access);
      }
    }
  }

  _old_tt_close_db_partition_files (partition);
  return &results;
}

_tt_db_results *_tt_remove_obj_1 (_tt_remove_obj_args *args,
				  const _Tt_db_client_ptr &db_conn)
{
  static _tt_db_results results;

  _Tt_db_access_ptr access = _old_tt_get_real_rpc_access(args->access);

  _Tt_string partition = _old_tt_get_object_partition(args->objid);
  _Tt_old_db_ptr db;
  results = _old_tt_get_partition_db(partition, db, db_conn);

  if (results == TT_DB_OK) {
    results = db->removeObject(args->objid, access);

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
	  property->name = TT_OLD_DB_FORWARD_POINTER_PROPERTY;
	  property->values->append(_Tt_string(args->forward_pointer));

	  results = db->setObjectProperty(args->objid, property, access);
        }
      }
    }
  }

  _old_tt_close_db_partition_files (partition);
  return &results;
}

_tt_db_results *_tt_move_file_1 (_tt_move_file_args *args,
				 const _Tt_db_client_ptr &db_conn)
{
  static _tt_db_results results;

  _Tt_db_access_ptr access = _old_tt_get_real_rpc_access(args->access);

  _Tt_string real_file = args->file;
  _Tt_string real_new_file = args->new_file;
  _Tt_string partition;
  _Tt_string new_partition;

  // Make sure we really need to do a move
  if (real_file != real_new_file) {
    partition = _old_tt_get_file_partition(real_file, db_conn);
    new_partition = _old_tt_get_file_partition(real_new_file, db_conn);
    
    if (partition == new_partition) {
      // Get a connection to the partition DB
      _Tt_old_db_ptr db;
      results = _old_tt_get_partition_db(partition, db, db_conn);

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
	    db->setFileFile(*children_cursor, new_child, access);
	  }
	}
      }
    }
    // Else, different partitions, therefore we can only move one
    // non-directory file
    else if (!_old_tt_is_file_a_directory(real_file)) {
      // Get a connections to both partition DBs
      _Tt_old_db_ptr db;
      results = _old_tt_get_partition_db(partition, db, db_conn);

      _Tt_old_db_ptr new_db;
      if (results == TT_DB_OK) {
	results = _old_tt_get_partition_db(new_partition, new_db, db_conn);
      }

      if (results == TT_DB_OK) {
	_Tt_db_property_list_ptr properties;
	_Tt_db_access_ptr        current_access;
	_Tt_string_list_ptr      objids;

	// Get all of the file's possesions
	results = db->getFileProperties(real_file, access, properties);
	if (results == TT_DB_OK) {
	  results = db->getFileAccess(real_file, access, current_access);
	}
	if (results == TT_DB_OK) {
	  results = db->getFileObjects(real_file, access, objids);
	}

	// Create the new file
	if (results == TT_DB_OK) {
	  results = new_db->createFile(real_new_file, current_access);
	}

	// Copy the old file's properties to the new file
	if (results == TT_DB_OK) {
	  results = new_db->setFileProperties(real_new_file,
					      properties,
					      access);
	}

	// Create the new objects with equivalent objids on the new partition
	_Tt_string_list_ptr new_objids;
	if (results == TT_DB_OK  && (!objids->is_empty())) {
	  new_objids = new _Tt_string_list;

	  // Loop through the file's objects
	  _Tt_string_list_cursor objids_cursor(objids);
	  while ((results == TT_DB_OK) && objids_cursor.next()) {
	    _Tt_string new_objid =
	      _old_tt_make_equivalent_object_id(*objids_cursor, new_partition);
	    new_objids->append(new_objid);

	    _Tt_db_property_list_ptr properties;
	    _Tt_db_access_ptr        current_access;

	    // Get all of the object's possesions
	    results = db->getObjectProperties(*objids_cursor,
					      access,
					      properties);
	    if (results == TT_DB_OK) {
	      results = db->getObjectAccess(*objids_cursor,
					    access,
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
						    access);
	    }
	  }
	}

	// Remove the old file and all of its objects
	if (results == TT_DB_OK) {
	  results = db->removeFile(real_file, access);
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
	      property->name = TT_OLD_DB_FORWARD_POINTER_PROPERTY;
	      property->values->append(*new_objids_cursor);

	      results = db->setObjectProperty(*objids_cursor,
					      property,
					      access);
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

  _old_tt_close_db_partition_files (partition);
  if (partition != new_partition) {
    _old_tt_close_db_partition_files (new_partition);
  }
  return &results;
}

_tt_db_cache_results *_tt_set_file_props_1 (_tt_set_file_props_args *args,
					    const _Tt_db_client_ptr &db_conn)
{
  static _tt_db_cache_results results;
  results.cache_level = -1;

  _Tt_db_access_ptr access = _old_tt_get_real_rpc_access(args->access);

  _Tt_string real_file = args->file;
  _Tt_string partition = _old_tt_get_file_partition(args->file, db_conn);
  _Tt_old_db_ptr db;
  results.results = _old_tt_get_partition_db(partition, db, db_conn);

  if (results.results == TT_DB_OK) {
    _Tt_db_property_list_ptr properties;
    _tt_get_rpc_properties(args->properties, properties);

    if (!properties.is_null()) {
      results.results = db->setFileProperties(real_file,
					      properties,
					      access);
    }
    else {
      results.results = TT_DB_ERR_ILLEGAL_PROPERTY;
    }

    if (results.results == TT_DB_OK) {
      results.results =
	_old_tt_increment_file_properties_cache_level(db,
						      real_file,
						      access,
						      results.cache_level);
    }
  }

  _old_tt_close_db_partition_files (partition);
  return &results;
}

_tt_db_cache_results *_tt_set_file_prop_1 (_tt_set_file_prop_args *args,
					   const _Tt_db_client_ptr &db_conn)
{
  static _tt_db_cache_results results;
  results.cache_level = -1;

  _Tt_db_access_ptr access = _old_tt_get_real_rpc_access(args->access);

  _Tt_string real_file = args->file;
  _Tt_string partition = _old_tt_get_file_partition(args->file, db_conn);
  _Tt_old_db_ptr db;
  results.results = _old_tt_get_partition_db(partition, db, db_conn);

  if (results.results == TT_DB_OK) {
    _Tt_db_property_ptr property;
    _tt_get_rpc_property(args->property, property);

    if (!property.is_null()) {
      results.results = db->setFileProperty(real_file,
					    property,
					    access);
    }
    else {
      results.results = TT_DB_ERR_ILLEGAL_PROPERTY;
    }

    if (results.results == TT_DB_OK) {
      results.results =
	_old_tt_increment_file_properties_cache_level(db,
						      real_file,
						      access,
						      results.cache_level);
    }
  }

  _old_tt_close_db_partition_files (partition);
  return &results;
}

_tt_db_cache_results *_tt_add_file_prop_1 (_tt_add_file_prop_args *args,
					   const _Tt_db_client_ptr &db_conn)
{
  static _tt_db_cache_results results;
  results.cache_level = -1;

  _Tt_db_access_ptr access = _old_tt_get_real_rpc_access(args->access);

  _Tt_string real_file = args->file;
  _Tt_string partition = _old_tt_get_file_partition(args->file, db_conn);
  _Tt_old_db_ptr db;
  results.results = _old_tt_get_partition_db(partition, db, db_conn);

  if (results.results == TT_DB_OK) {
    _Tt_db_property_ptr property;
    _tt_get_rpc_property(args->property, property);

    if (!property.is_null()) {
      results.results = db->addFileProperty(real_file,
					    property,
					    args->unique,
					    access);
    }
    else {
      results.results = TT_DB_ERR_ILLEGAL_PROPERTY;
    }

    if (results.results == TT_DB_OK) {
      results.results =
	_old_tt_increment_file_properties_cache_level(db,
						      real_file,
						      access,
						      results.cache_level);
    }
  }

  _old_tt_close_db_partition_files (partition);
  return &results;
}

_tt_db_cache_results *_tt_delete_file_prop_1 (_tt_del_file_prop_args *args,
					      const _Tt_db_client_ptr &db_conn)
{
  static _tt_db_cache_results results;
  results.cache_level = -1;

  _Tt_db_access_ptr access = _old_tt_get_real_rpc_access(args->access);

  _Tt_string real_file = args->file;
  _Tt_string partition = _old_tt_get_file_partition(args->file, db_conn);
  _Tt_old_db_ptr db;
  results.results = _old_tt_get_partition_db(partition, db, db_conn);

  if (results.results == TT_DB_OK) {
    _Tt_db_property_ptr property;
    _tt_get_rpc_property(args->property, property);

    if (!property.is_null()) {
      results.results = db->deleteFileProperty(real_file,
					       property,
					       access);
    }
    else {
      results.results = TT_DB_ERR_ILLEGAL_PROPERTY;
    }

    if (results.results == TT_DB_OK) {
      results.results =
	_old_tt_increment_file_properties_cache_level(db,
						      real_file,
						      access,
						      results.cache_level);
    }
  }

  _old_tt_close_db_partition_files (partition);
  return &results;
}

_tt_file_prop_results *_tt_get_file_prop_1 (_tt_get_file_prop_args *args,
					    const _Tt_db_client_ptr &db_conn)
{
  static _tt_file_prop_results results;
  results.cache_level = -1;

  _Tt_db_access_ptr access = _old_tt_get_real_rpc_access(args->access);

  _Tt_string real_file = args->file;
  _Tt_string partition = _old_tt_get_file_partition(args->file, db_conn);
  _Tt_old_db_ptr db;
  results.results = _old_tt_get_partition_db(partition, db, db_conn);

  if (results.results == TT_DB_OK) {
    results.results = _old_tt_get_file_properties_cache_level(db,
							      real_file,
							      access,
							      results.cache_level);
  }

  _Tt_db_property_ptr property;
  if ((results.cache_level > args->cache_level) &&
      (results.results == TT_DB_OK)) {
    results.results = db->getFileProperty(real_file,
					  args->name,
					  access,
					  property);
  }
  _tt_set_rpc_property(property, results.property);

  _old_tt_close_db_partition_files (partition);
  return &results;
}

_tt_file_props_results *
_tt_get_file_props_1 (_tt_get_file_props_args *args,
		      const _Tt_db_client_ptr &db_conn)
{
  static _tt_file_props_results results;
  results.cache_level = -1;

  _Tt_db_access_ptr access = _old_tt_get_real_rpc_access(args->access);

  _Tt_string real_file = args->file;
  _Tt_string partition = _old_tt_get_file_partition(args->file, db_conn);
  _Tt_old_db_ptr db;
  results.results = _old_tt_get_partition_db(partition, db, db_conn);

  if (results.results == TT_DB_OK) {
    results.results = _old_tt_get_file_properties_cache_level(db,
							      real_file,
							      access,
							      results.cache_level);
  }

  _Tt_db_property_list_ptr properties;
  if ((results.cache_level > args->cache_level) &&
      (results.results == TT_DB_OK)) {
    results.results = db->getFileProperties(real_file,
					    access,
					    properties);
  }
  _tt_set_rpc_properties(properties, results.properties);

  _old_tt_close_db_partition_files (partition);
  return &results;
}

_tt_file_objs_results *_tt_get_file_objs_1 (_tt_get_file_objs_args *args,
					    const _Tt_db_client_ptr &db_conn)
{
  static _tt_file_objs_results results;

  // Make sure the returned cache level is higher then the callers
  // cache level, so that the caller will use the returned data
  results.cache_level = args->cache_level + 1;

  _Tt_db_access_ptr access = _old_tt_get_real_rpc_access(args->access);

  _Tt_string real_file = args->file;
  _Tt_string partition = _old_tt_get_file_partition(args->file, db_conn);
  _Tt_old_db_ptr db;
  results.results = _old_tt_get_partition_db(partition, db, db_conn);

  _Tt_string_list_ptr objids;
  if ((results.cache_level > args->cache_level) &&
      (results.results == TT_DB_OK)) {
    results.results = db->getFileObjects(real_file,
					 access,
					 objids);
  }
  _tt_set_rpc_strings(objids, results.objids);

  _old_tt_close_db_partition_files (partition);
  return &results;
}

_tt_db_results *_tt_set_file_access_1 (_tt_set_file_access_args *args,
				       const _Tt_db_client_ptr &db_conn)
{
  static _tt_db_results results;

  _Tt_db_access_ptr access = _old_tt_get_real_rpc_access(args->access);

  _Tt_string real_file = args->file;
  _Tt_string partition = _old_tt_get_file_partition(args->file, db_conn);
  _Tt_old_db_ptr db;
  results = _old_tt_get_partition_db(partition, db, db_conn);

  _Tt_db_access_ptr new_access;
  if (results == TT_DB_OK) {
    _tt_get_rpc_access(args->new_access, new_access);

    if ((new_access->user == (uid_t)-1) ||
        (new_access->group == (gid_t)-1) ||
        (new_access->mode == (mode_t)-1)) { 
      _Tt_db_access_ptr current_access;   
      results = db->getFileAccess(real_file, 
                                  access,
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
      results = db->setFileAccess(real_file, new_access, access);
    }
  }

  _old_tt_close_db_partition_files (partition);
  return &results;
}

_tt_file_access_results *_tt_get_file_access_1 (_tt_get_file_access_args *args,
						const _Tt_db_client_ptr &db_conn)
{
  static _tt_file_access_results results;

  _Tt_string real_file = args->file;
  _Tt_string partition = _old_tt_get_file_partition(args->file, db_conn);
  _Tt_old_db_ptr db;
  results.results = _old_tt_get_partition_db(partition, db, db_conn);

  _Tt_db_access_ptr access = _old_tt_get_real_rpc_access(args->access);

  _Tt_db_access_ptr current_access;
  if (results.results == TT_DB_OK) {
    results.results = db->getFileAccess(real_file,
					access,
					current_access);
  }
  _tt_set_rpc_access(current_access, results.access);

  _old_tt_close_db_partition_files (partition);
  return &results;
}

_tt_obj_props_results *_tt_set_obj_props_1 (_tt_set_obj_props_args *args,
					    const _Tt_db_client_ptr &db_conn)
{
  static _tt_obj_props_results results;
  results.cache_level = -1;

  _Tt_string partition = _old_tt_get_object_partition(args->objid);
  _Tt_old_db_ptr db;
  results.results = _old_tt_get_partition_db(partition, db, db_conn);

  _Tt_db_access_ptr access = _old_tt_get_real_rpc_access(args->access);

  if (results.results == TT_DB_OK) {
    results.results =
      _old_tt_get_object_properties_cache_level(db,
						args->objid,
						access,
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
							  access,
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
      _old_tt_screen_object_properties (properties);
    }

    if (!properties.is_null()) {
      results.results = db->setObjectProperties(args->objid,
						properties,
						access);
    }
    else {
      results.results = TT_DB_ERR_ILLEGAL_PROPERTY;
    }
  
    if (results.results == TT_DB_OK) {
      results.results =
	_old_tt_increment_object_properties_cache_level(db,
							args->objid,
							access,
							results.cache_level);
    }
  }

  _old_tt_close_db_partition_files (partition);
  return &results;
}

_tt_obj_props_results *_tt_set_obj_prop_1 (_tt_set_obj_prop_args *args,
					   const _Tt_db_client_ptr &db_conn)
{
  static _tt_obj_props_results results;
  results.cache_level = -1;

  _Tt_string partition = _old_tt_get_object_partition(args->objid);
  _Tt_old_db_ptr db;
  results.results = _old_tt_get_partition_db(partition, db, db_conn);

  _Tt_db_access_ptr access = _old_tt_get_real_rpc_access(args->access);

  if (results.results == TT_DB_OK) {
    results.results =
      _old_tt_get_object_properties_cache_level(db,
						args->objid,
						access,
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
							  access,
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
					      access);
    }
    else {
      results.results = TT_DB_ERR_ILLEGAL_PROPERTY;
    }
    
    if (results.results == TT_DB_OK) {
      results.results =
	_old_tt_increment_object_properties_cache_level(db,
							args->objid,
							access,
							results.cache_level);
    }
  }

  _old_tt_close_db_partition_files (partition);
  return &results;
}

_tt_obj_props_results *_tt_add_obj_prop_1 (_tt_add_obj_prop_args *args,
					   const _Tt_db_client_ptr &db_conn)
{
  static _tt_obj_props_results results;
  results.cache_level = -1;

  _Tt_string partition = _old_tt_get_object_partition(args->objid);
  _Tt_old_db_ptr db;
  results.results = _old_tt_get_partition_db(partition, db, db_conn);

  _Tt_db_access_ptr access = _old_tt_get_real_rpc_access(args->access);

  if (results.results == TT_DB_OK) {
    results.results =
      _old_tt_get_object_properties_cache_level(db,
						args->objid,
						access,
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
							  access,
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
					      access);
    }
    else {
      results.results = TT_DB_ERR_ILLEGAL_PROPERTY;
    }
    
    if (results.results == TT_DB_OK) {
      results.results =
	_old_tt_increment_object_properties_cache_level(db,
							args->objid,
							access,
							results.cache_level);
    }
  }

  _old_tt_close_db_partition_files (partition);
  return &results;
}

_tt_obj_props_results *_tt_delete_obj_prop_1 (_tt_del_obj_prop_args *args,
					      const _Tt_db_client_ptr &db_conn)
{
  static _tt_obj_props_results results;
  results.cache_level = -1;

  _Tt_string partition = _old_tt_get_object_partition(args->objid);
  _Tt_old_db_ptr db;
  results.results = _old_tt_get_partition_db(partition, db, db_conn);

  _Tt_db_access_ptr access = _old_tt_get_real_rpc_access(args->access);

  if (results.results == TT_DB_OK) {
    results.results =
      _old_tt_get_object_properties_cache_level(db,
						args->objid,
						access,
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
						 access);
    }
    else {
      results.results = TT_DB_ERR_ILLEGAL_PROPERTY;
    }
    
    if (results.results == TT_DB_OK) {
      results.results =
	_old_tt_increment_object_properties_cache_level(db,
							args->objid,
							access,
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
							  access,
							  properties);
    
    if (results.results != TT_DB_OK) {
      properties = (_Tt_db_property_list *)NULL;
      results.results = temp_results;
    }
  }
  _tt_set_rpc_properties(properties, results.properties);
  
  _old_tt_close_db_partition_files (partition);
  return &results;
}

_tt_obj_prop_results *_tt_get_obj_prop_1 (_tt_get_obj_prop_args *args,
					  const _Tt_db_client_ptr &db_conn)
{
  static _tt_obj_prop_results results;
  results.cache_level = -1;

  _Tt_string partition = _old_tt_get_object_partition(args->objid);
  _Tt_old_db_ptr db;
  results.results = _old_tt_get_partition_db(partition, db, db_conn);

  _Tt_db_access_ptr access = _old_tt_get_real_rpc_access(args->access);

  if (results.results == TT_DB_OK) {
    results.results =
      _old_tt_get_object_properties_cache_level(db,
						args->objid,
						access,
						results.cache_level);
  }

  _Tt_db_property_ptr property;

  // Only return values if the DB cache level is higher then the
  // callers cache level
  if ((results.results == TT_DB_OK) &&
      (results.cache_level > args->cache_level)) {
    results.results = db->getObjectProperty(args->objid,
					    args->name,
					    access,
					    property);
  }
  _tt_set_rpc_property(property, results.property);
    
  _old_tt_close_db_partition_files (partition);
  return &results;
}

_tt_obj_props_results *_tt_get_obj_props_1 (_tt_get_obj_props_args *args,
					    const _Tt_db_client_ptr &db_conn)
{
  static _tt_obj_props_results results;
  results.cache_level = -1;

  _Tt_string partition = _old_tt_get_object_partition(args->objid);
  _Tt_old_db_ptr db;
  results.results = _old_tt_get_partition_db(partition, db, db_conn);

  _Tt_db_access_ptr access = _old_tt_get_real_rpc_access(args->access);

  if (results.results == TT_DB_OK) {
    results.results =
      _old_tt_get_object_properties_cache_level(db,
						args->objid,
						access,
						results.cache_level);
  }

  _Tt_db_property_list_ptr properties;

  // Only return values if the DB cache level is higher then the
  // callers cache level
  if ((results.results == TT_DB_OK) &&
      (results.cache_level > args->cache_level)) {
    results.results = db->getObjectProperties(args->objid,
					      access,
					      properties);
  }
  _tt_set_rpc_properties(properties, results.properties);

  _old_tt_close_db_partition_files (partition);
  return &results;
}

_tt_db_results *_tt_set_obj_type_1 (_tt_set_obj_type_args *args,
				    const _Tt_db_client_ptr &db_conn)
{
  static _tt_db_results results;

  _Tt_string partition = _old_tt_get_object_partition(args->objid);
  _Tt_old_db_ptr db;
  results = _old_tt_get_partition_db(partition, db, db_conn);

  _Tt_db_access_ptr access = _old_tt_get_real_rpc_access(args->access);

  if (results == TT_DB_OK) {
    _Tt_db_property_ptr property = new _Tt_db_property;
    property->name = TT_OLD_DB_OBJECT_TYPE_PROPERTY;
    property->values->append(_Tt_string(args->otype));

    results = db->setObjectProperty(args->objid, property, access);
  }

  _old_tt_close_db_partition_files (partition);
  return &results;
}

/*
 * Use the old DB "gettype" instead
 *
_tt_obj_type_results *_tt_get_obj_type_1 (_tt_get_obj_type_args *args,
					  const _Tt_db_client_ptr &db_conn)
{
  static _tt_obj_type_results results;

  _Tt_string partition = _old_tt_get_object_partition(args->objid);
  _Tt_old_db_ptr db;
  results.results = _tt_get_partition_db(partition, db, db_conn);

  _Tt_db_access_ptr access = _old_tt_get_real_rpc_access(args->access);

  if (results.results == TT_DB_OK) {
    _Tt_db_property_ptr property;
    results.results = db->getObjectProperty(args->objid,
					    TT_DB_OBJECT_TYPE_PROPERTY,
					    access,
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

  _old_tt_close_db_partition_files (partition);
  return &results;
}
*/

_tt_db_results *_tt_set_obj_file_1 (_tt_set_obj_file_args *args,
				    const _Tt_db_client_ptr &db_conn)
{
  static _tt_db_results results;

  _Tt_string partition = _old_tt_get_object_partition(args->objid);
  _Tt_old_db_ptr db;
  results = _old_tt_get_partition_db(partition, db, db_conn);

  _Tt_db_access_ptr access = _old_tt_get_real_rpc_access(args->access);

  if (results == TT_DB_OK) {
    results = db->setObjectFile(args->objid, args->file, access);
  }

  _old_tt_close_db_partition_files (partition);
  return &results;
}

_tt_obj_file_results *_tt_get_obj_file_1 (_tt_get_obj_file_args *args,
					  const _Tt_db_client_ptr &db_conn)
{
  static _tt_obj_file_results results;

  _Tt_string partition = _old_tt_get_object_partition(args->objid);
  _Tt_old_db_ptr db;
  results.results = _old_tt_get_partition_db(partition, db, db_conn);

  _Tt_db_access_ptr access = _old_tt_get_real_rpc_access(args->access);

  if (results.results == TT_DB_OK) {
    _Tt_string file;
    results.results = db->getObjectFile(args->objid,
					access,
					file);

    results.file = (char *)NULL;
    if (results.results == TT_DB_OK) {
      if (file.len()) {
	results.file = strdup((char *)file);
      }
    }
  }

  _old_tt_close_db_partition_files (partition);
  return &results;
}

_tt_db_results *_tt_set_obj_access_1 (_tt_set_obj_access_args *args,
				      const _Tt_db_client_ptr &db_conn)
{
  static _tt_db_results results;

  _Tt_string partition = _old_tt_get_object_partition(args->objid);
  _Tt_old_db_ptr db;
  results = _old_tt_get_partition_db(partition, db, db_conn);

  _Tt_db_access_ptr access = _old_tt_get_real_rpc_access(args->access);

  _Tt_db_access_ptr new_access;
  if (results == TT_DB_OK) {
    _tt_get_rpc_access(args->new_access, new_access);

    if ((new_access->user == (uid_t)-1) ||
        (new_access->group == (gid_t)-1) ||
        (new_access->mode == (mode_t)-1)) {
      _Tt_db_access_ptr current_access;
      results = db->getObjectAccess(args->objid,
				    access,
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
      results = db->setObjectAccess(args->objid, new_access, access);
    }
  }

  _old_tt_close_db_partition_files (partition);
  return &results;
}

_tt_obj_access_results *_tt_get_obj_access_1 (_tt_get_obj_access_args *args,
					      const _Tt_db_client_ptr &db_conn)
{
  static _tt_obj_access_results results;

  _Tt_string partition = _old_tt_get_object_partition(args->objid);
  _Tt_old_db_ptr db;
  results.results = _old_tt_get_partition_db(partition, db, db_conn);

  _Tt_db_access_ptr access = _old_tt_get_real_rpc_access(args->access);

  _Tt_db_access_ptr current_access;
  if (results.results == TT_DB_OK) {
    results.results = db->getObjectAccess(args->objid,
					  access,
					  current_access);
  }
  _tt_set_rpc_access(current_access, results.access);

  _old_tt_close_db_partition_files (partition);
  return &results;
}

_tt_is_file_in_db_results *_tt_is_file_in_db_1 (_tt_is_file_in_db_args *args,
						const _Tt_db_client_ptr &db_conn)
{
  static _tt_is_file_in_db_results results;
  
  _Tt_db_access_ptr access = _old_tt_get_real_rpc_access(args->access);

  _Tt_string real_file = args->file;
  _Tt_string partition = _old_tt_get_file_partition(args->file, db_conn);
  _Tt_old_db_ptr db;
  results.results = _old_tt_get_partition_db(partition, db, db_conn);

  if (results.results == TT_DB_OK) {
    // See if the file already exists by trying to get its access info
    _Tt_db_access_ptr temp_access;
    _Tt_db_results temp_results = db->getFileAccess(real_file,
						    access,
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
    results.directory_flag = _old_tt_is_file_a_directory(real_file);
  }

  _old_tt_close_db_partition_files (partition);
  return &results;
}

_tt_is_obj_in_db_results *_tt_is_obj_in_db_1 (_tt_is_obj_in_db_args *args,
					      const _Tt_db_client_ptr &db_conn)
{
  static _tt_is_obj_in_db_results results;
  results.forward_pointer = (char *)NULL;
  
  _Tt_string partition = _old_tt_get_object_partition(args->objid);
  _Tt_old_db_ptr db;
  results.results = _old_tt_get_partition_db(partition, db, db_conn);

  _Tt_db_access_ptr access = _old_tt_get_real_rpc_access(args->access);

  if (results.results == TT_DB_OK) {
    _Tt_string file;

    // See if the object has an entry in the file-object map
    results.results = db->getObjectFile(args->objid,
					access,
					file);

    // If no entry in the file-object map...
    if (results.results != TT_DB_OK) {
      _Tt_db_property_ptr property;

      // See if there is a forward pointer
      _Tt_db_results temp_results =
	db->getObjectProperty(args->objid,
			      TT_OLD_DB_FORWARD_POINTER_PROPERTY,
			      access,
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

  _old_tt_close_db_partition_files (partition);
  return &results;
}

_tt_db_results *_tt_queue_message_1 (_tt_queue_msg_args *args,
				     const _Tt_db_client_ptr &db_conn)
{
  static _tt_db_results results;

  bool_t property_written = FALSE;

  _Tt_old_db_ptr db;
  _Tt_string     partition;
  _Tt_string     real_file = args->file;

  if (!args->message.body.body_len) {
    results = TT_DB_ERR_ILLEGAL_MESSAGE;
  }
  else {
    partition = _old_tt_get_file_partition(args->file, db_conn);
    results = _old_tt_get_partition_db(partition, db, db_conn);
  }

  _Tt_db_access_ptr access = new _Tt_db_access;
  access->user = geteuid();
  access->group = getegid();

  // Create a new message info structure for the new message
  _Tt_old_db_message_info_ptr message_info = new _Tt_old_db_message_info;
  if (results == TT_DB_OK) {
    XDR  xdrs;
    _Tt_xdr_size_stream xdrsz;

    // Get the current information on queued messages
    _Tt_db_property_ptr property;
    _Tt_string name = TT_OLD_DB_MESSAGE_INFO_PROPERTY;
    results = db->getFileProperty(real_file, name, access, property);

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
	_Tt_old_db_message_info_ptr last_message_info = new _Tt_old_db_message_info;
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
      message_info->numParts = (args->message.body.body_len / TT_OLD_MAX_RECORD_LENGTH) + 1;
      message_info->messageSize = args->message.body.body_len;
      _tt_get_rpc_strings(args->ptypes, message_info->ptypes);

      // Get the XDR size of the new message info structure
      u_int length = 0;
      if (!message_info->xdr((XDR *)xdrsz)) {
	results = TT_DB_ERR_ILLEGAL_MESSAGE;
      }
      else {
	length = (u_int)xdrsz.getsize();
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
	property->name = TT_OLD_DB_MESSAGE_INFO_PROPERTY;
	property->values->append(temp_string);

	results = db->addFileProperty(real_file, property, FALSE, access);
	property_written = TRUE;
      }
    }
  }
  
  if (results == TT_DB_OK) {
    int length = TT_OLD_MAX_RECORD_LENGTH;

    // Break up the message into TT_OLD_MAX_RECORD_LENGTH sized parts and store
    // as separate properties
    for (int i=0;
	 (results == TT_DB_OK) && (i < message_info->numParts);
	 i++) {
      // If this is the last part, it is probably shorter then
      // TT_OLD_MAX_RECORD_LENGTH, so calculate the exact length
      if (i == message_info->numParts-1) {
	length = message_info->messageSize - (i * TT_OLD_MAX_RECORD_LENGTH);
      }

      // Copy the message part into a buffer
      _Tt_string message_part(length);
      memcpy((char *)message_part,
	     args->message.body.body_val+i*TT_OLD_MAX_RECORD_LENGTH,
	     length);

      // Construct a property name of the form:
      //
      //           _TT_MSG_<ID#>_<PART#>
      //
      char name [64];
      sprintf(name, TT_OLD_DB_MESSAGE_PROPERTY, message_info->messageID, i);

      // Store the property
      _Tt_db_property_ptr property = new _Tt_db_property;
      property->name = name;
      property->values->append(message_part);

      results = db->setFileProperty(real_file, property, access);
      property_written = TRUE;
    }
  }

  if (property_written && (results == TT_DB_OK)) {
    int cache_level;
    results = _old_tt_increment_file_properties_cache_level(db,
							    real_file,
							    access,
							    cache_level);
  }

  _old_tt_close_db_partition_files (partition);
  return &results;
}

_tt_dequeue_msgs_results *
_tt_dequeue_messages_1 (_tt_dequeue_msgs_args *args,
			const _Tt_db_client_ptr &db_conn)
{
  static _tt_dequeue_msgs_results results;
  results.messages.messages_val = (_tt_message *)NULL;
  results.messages.messages_len = 0;

  bool_t property_written = FALSE;

  _Tt_string real_file = args->file;
  _Tt_string partition = _old_tt_get_file_partition(args->file, db_conn);
  _Tt_old_db_ptr db;
  results.results = _old_tt_get_partition_db(partition, db, db_conn);

  _Tt_db_access_ptr access = new _Tt_db_access;
  access->user = geteuid();
  access->group = getegid();

  // Get the message info property
  _Tt_db_property_ptr property;
  if (results.results == TT_DB_OK) {
    _Tt_string name = TT_OLD_DB_MESSAGE_INFO_PROPERTY;
    results.results = db->getFileProperty(real_file,
					  name,
					  access,
					  property);
  }
  
  _Tt_old_db_message_info_ptr message_info = new _Tt_old_db_message_info;
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
	  sprintf(name, TT_OLD_DB_MESSAGE_PROPERTY, message_info->messageID, i);

	  // Get the property value
	  _Tt_db_property_ptr property;
	  results.results = db->getFileProperty(real_file,
						name,
						access,
						property);
	  if (results.results == TT_DB_OK) {
	    _Tt_string message_bytes = (*property->values) [0];

	    // Copy each succesive part into a large buffer
	    memcpy((char *)message+i*TT_OLD_MAX_RECORD_LENGTH,
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
        length = (u_int)xdrsz.getsize();

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
					  access);
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
    results.results = _old_tt_increment_file_properties_cache_level(db,
								    real_file,
								    access,
								    cache_level);
  }

  _old_tt_close_db_partition_files (partition);
  return &results;
}

//
// ******* Static helper functions start here *******
//

void _old_tt_close_db_partition_files (const _Tt_string &partition)
{
  if (partition.len ()) {
    _Tt_old_db_partition_map_ref db_map;
    db_map.removeDB (partition);
  }
}

bool_t _old_tt_is_file_a_directory (const _Tt_string &file)
{
  _Tt_string local_path = _tt_network_path_to_local_path(file);

  DIR *dd = opendir((char *)local_path);

  if (dd) {
    (void)closedir(dd);
  }

  return (dd ? TRUE : FALSE);
}

// Replace the old partition with the new partition in the object ID
static _Tt_string _old_tt_make_equivalent_object_id (const _Tt_string &objid,
						     const _Tt_string &partition)
{
  _Tt_string temp_string = (char *)objid;
  _Tt_string new_objid;

  temp_string = temp_string.rsplit (':', new_objid);
  new_objid = new_objid.cat(partition);

  return new_objid;
}

static _Tt_db_results _old_tt_get_partition_db (const _Tt_string        &partition,
						_Tt_old_db_ptr          &db,
						const _Tt_db_client_ptr &db_conn)
{
  _Tt_old_db_partition_map_ref db_map;
  _Tt_db_results               results = TT_DB_OK;

  db = db_map.getDB(partition, db_conn);
  if (db.is_null()) {
    results = TT_DB_ERR_DB_OPEN_FAILED;
  }

  return results;
}

static _Tt_db_access_ptr _old_tt_get_real_rpc_access (const _tt_access &rpc_access)
{
  _Tt_db_access_ptr access;
  _tt_get_rpc_access(rpc_access, access);

  access->user = geteuid();
  access->group = getegid();

  return access;
}

static
_Tt_db_access_ptr _old_tt_get_file_access (const _Tt_string & /* file */,
					   const _tt_access &rpc_access)
{
  _Tt_db_access_ptr access = _old_tt_get_real_rpc_access(rpc_access);
  return access;
}

static
_Tt_string _old_tt_get_file_partition (const _Tt_string        &file,
				       const _Tt_db_client_ptr &db_conn)

{
  _Tt_string temp_string;
  _Tt_string path = file;
  path = path.split(':', temp_string);

  _Tt_string partition = db_conn->mfs(path);
  return partition;
}

static _Tt_db_results
_old_tt_increment_file_properties_cache_level
  (const _Tt_old_db_ptr &db,
   const _Tt_string           &file,
   const _Tt_db_access_ptr    &access,
   int                        &cache_level)
{
  cache_level = -1;

  _Tt_db_property_ptr property;
  _Tt_db_results results =
    db->getFileProperty(file,
			TT_OLD_DB_PROPS_CACHE_LEVEL_PROPERTY,
			access,
			property);

  if (results == TT_DB_OK) {
    _Tt_string cache_level_bytes = (*property->values) [0];
    memcpy ((char *)&cache_level, (char *)cache_level_bytes, sizeof(int));

    cache_level++;
    memcpy ((char *)cache_level_bytes, (char *)&cache_level, sizeof(int));
    (*property->values) [0] = cache_level_bytes;

    results = db->setFileProperty(file,
				  property,
				  access);
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
    property->name = TT_OLD_DB_PROPS_CACHE_LEVEL_PROPERTY;
    property->values->append(value);

    results = db->setFileProperty(file,
				  property,
				  access);
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
_old_tt_get_file_properties_cache_level (const _Tt_old_db_ptr &db,
					 const _Tt_string           &file,
					 const _Tt_db_access_ptr    &access,
					 int                        &cache_level)
{
  cache_level = -1;

  _Tt_db_property_ptr property;
  _Tt_db_results results =
    db->getFileProperty(file,
			TT_OLD_DB_PROPS_CACHE_LEVEL_PROPERTY,
			access,
			property);
  if (results == TT_DB_OK) {
    _Tt_string cache_level_bytes = (*property->values) [0];
    memcpy ((char *)&cache_level, (char *)cache_level_bytes, sizeof(int));
  }
  // The file was probably created for an object.  The cache level
  // was never stored as a property...
  else if (results == TT_DB_ERR_NO_SUCH_PROPERTY) {
    results = _old_tt_increment_file_properties_cache_level(db,
							    file,
							    access,
							    cache_level);
  }
  else {
    results = TT_DB_ERR_PROPS_CACHE_ERROR;
  }

  return results;
}

static _Tt_string _old_tt_get_object_partition (const _Tt_string &objid)
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
_old_tt_increment_object_properties_cache_level
  (const _Tt_old_db_ptr &db,
   const _Tt_string           &objid,
   const _Tt_db_access_ptr    &access,
   int                        &cache_level)
{
  cache_level = -1;

  _Tt_db_property_ptr property;
  _Tt_db_results results =
    db->getObjectProperty(objid,
			  TT_OLD_DB_PROPS_CACHE_LEVEL_PROPERTY,
			  access,
			  property);

  if (results == TT_DB_OK) {
    _Tt_string cache_level_bytes = (*property->values) [0];
    memcpy ((char *)&cache_level, (char *)cache_level_bytes, sizeof(int));

    cache_level++;
    memcpy ((char *)cache_level_bytes, (char *)&cache_level, sizeof(int));
    (*property->values) [0] = cache_level_bytes;

    results = db->setObjectProperty(objid,
				    property,
				    access);
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
    property->name = TT_OLD_DB_PROPS_CACHE_LEVEL_PROPERTY;
    property->values->append(value);

    results = db->setObjectProperty(objid,
				    property,
				    access);
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
_old_tt_get_object_properties_cache_level (const _Tt_old_db_ptr &db,
					   const _Tt_string           &objid,
					   const _Tt_db_access_ptr    &access,
					   int                        &cache_level)
{
  cache_level = -1;

  _Tt_db_property_ptr property;
  _Tt_db_results results = 
    db->getObjectProperty(objid,
			  TT_OLD_DB_PROPS_CACHE_LEVEL_PROPERTY,
			  access,
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
_old_tt_screen_object_properties (_Tt_db_property_list_ptr &properties)
{
  _Tt_db_property_list_cursor properties_cursor (properties);
  while (properties_cursor.next ()) {
    if (properties_cursor->name ==  TT_OLD_DB_PROPS_CACHE_LEVEL_PROPERTY) {
      properties_cursor.remove ();
    }
    else if (properties_cursor->name == TT_OLD_DB_OBJECT_TYPE_PROPERTY) {
      properties_cursor.remove ();
    }
  }

  if (properties->count () == 0) {
    properties = (_Tt_db_property_list *)NULL;
  }
}
