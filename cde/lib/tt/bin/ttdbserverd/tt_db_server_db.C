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
//%%  $TOG: tt_db_server_db.C /main/4 1999/09/23 15:12:26 mgreess $ 			 				
/*
 * tt_db_server_db.cc - Defines the TT DB server database.
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */

#include <string.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/param.h>
#include <netinet/in.h>

#include "util/tt_port.h"
#include "util/tt_gettext.h"
#include "tt_db_server_consts.h"
#include "tt_db_server_db.h"
#include "db/tt_db_create_objid.h"
#include "db/tt_db_property.h"
#include "db/tt_db_access.h"

extern FILE *errstr;

_Tt_db_server_db::_Tt_db_server_db ()
{
  _Tt_string partition = "/";
  connectToDB(partition);
}

_Tt_db_server_db::_Tt_db_server_db (const _Tt_string &partition)
{
  connectToDB(partition);
}

void _Tt_db_server_db::connectToDB (const _Tt_string &partition)
{
  dbResults = TT_DB_OK;
  dbPartition = partition;

  dbHostname = _tt_gethostname();

  _Tt_string base_dir = partition;
  if (base_dir[base_dir.len()-1] != '/') {
    base_dir = base_dir.cat("/");
  }
  base_dir = base_dir.cat("TT_DB/");

  // If mkdir fails, don't worry about it.  The problem will be caught
  // when NetISAM tries to open the table file.
  (void)mkdir(GNU_STRCAST base_dir, S_IRWXU+S_IRGRP+S_IXGRP+S_IROTH+S_IXOTH);

  _Tt_string file_table_file(TT_DB_FILE_TABLE_FILE);
  file_table_file = base_dir.cat(file_table_file);

  _Tt_string file_object_map_file(TT_DB_FILE_OBJECT_MAP_FILE);
  file_object_map_file = base_dir.cat(file_object_map_file);

  _Tt_string property_table_file(TT_DB_PROPERTY_TABLE_FILE);
  property_table_file = base_dir.cat(property_table_file);

  _Tt_string access_table_file(TT_DB_ACCESS_TABLE_FILE);
  access_table_file = base_dir.cat(access_table_file);

  // Create key descriptor for the file key in the file table
  fileTableFileKey = new _Tt_isam_key_descriptor;
  fileTableFileKey->addKeyPart(TT_DB_FIRST_KEY_OFFSET,
			       TT_DB_KEY_LENGTH,
			       BINTYPE);

  fileTable = new _Tt_isam_file(file_table_file,
				TT_DB_KEY_LENGTH+MAXPATHLEN,
				TT_DB_KEY_LENGTH+TT_DB_MAX_KEY_LENGTH,
				fileTableFileKey,
				ISVARLEN+ISINOUT+ISEXCLLOCK);
  dbLastFileAccessed = fileTable->getName();
  int results = fileTable->getErrorStatus();

  // Create key descriptor for the file path in the file table
  fileTableFilePathKey = new _Tt_isam_key_descriptor;
  fileTableFilePathKey->addKeyPart(TT_DB_FILE_PATH_OFFSET,
				   TT_DB_MAX_KEY_LENGTH,
				   CHARTYPE);

  // If this is new file add the key as an index
  if (!results && fileTable->isNew()) {
    (void)fileTable->addIndex(fileTableFilePathKey);
    (void)fileTable->writeMagicString(_Tt_string(TT_DB_VERSION));
  }
  else if (results) {
    dbResults = TT_DB_ERR_DB_OPEN_FAILED;
  }

  if (dbResults == TT_DB_OK) {
    // Create key descriptor for the object key in the file-object map
    fileObjectMapObjectKey = new _Tt_isam_key_descriptor;
    fileObjectMapObjectKey->addKeyPart(TT_DB_FIRST_KEY_OFFSET,
				       TT_DB_KEY_LENGTH,
				       BINTYPE);

    fileObjectMap = new _Tt_isam_file(file_object_map_file,
				      2*TT_DB_KEY_LENGTH,
				      2*TT_DB_KEY_LENGTH,
				      fileObjectMapObjectKey,
				      ISFIXLEN+ISINOUT+ISEXCLLOCK);
    dbLastFileAccessed = fileObjectMap->getName();
    results = fileObjectMap->getErrorStatus();

    // Create key descriptor for the file key in the file-object map
    fileObjectMapFileKey = new _Tt_isam_key_descriptor;
    fileObjectMapFileKey->addKeyPart(TT_DB_SECOND_KEY_OFFSET,
				     TT_DB_KEY_LENGTH,
				     BINTYPE);
    fileObjectMapFileKey->setDuplicates(TRUE);

    // If this is new file add the key as an index
    if (!results && fileObjectMap->isNew()) {
      (void)fileObjectMap->addIndex(fileObjectMapFileKey);
      (void)fileObjectMap->writeMagicString(_Tt_string(TT_DB_VERSION));
    }
    else if (results) {
      dbResults = TT_DB_ERR_DB_OPEN_FAILED;
    }
  }

  if (dbResults == TT_DB_OK) {
    // Create key descriptor for the object key and property name in
    // the property table
    propertyTablePropertyKey = new _Tt_isam_key_descriptor;
    propertyTablePropertyKey->addKeyPart(TT_DB_FIRST_KEY_OFFSET,
					 TT_DB_KEY_LENGTH,
					 BINTYPE);
    propertyTablePropertyKey->addKeyPart(TT_DB_PROPERTY_NAME_OFFSET,
					 TT_DB_MAX_PROPERTY_NAME_LENGTH,
					 CHARTYPE);
    propertyTablePropertyKey->setDuplicates(TRUE);

    propertyTable =
      new _Tt_isam_file(property_table_file,
			ISMAXRECLEN,
			TT_DB_KEY_LENGTH+TT_DB_MAX_PROPERTY_NAME_LENGTH,
			propertyTablePropertyKey,
			ISVARLEN+ISINOUT+ISEXCLLOCK);
    dbLastFileAccessed = propertyTable->getName();
    results = propertyTable->getErrorStatus();

    if (!results && propertyTable->isNew()) {
      (void)propertyTable->writeMagicString(_Tt_string(TT_DB_VERSION));
    }
    else if (results) {
      dbResults = TT_DB_ERR_DB_OPEN_FAILED;
    }
  }

  if (dbResults == TT_DB_OK) {
    // Create the key descriptor for the object key in the access table
    accessTableKey = new _Tt_isam_key_descriptor;
    accessTableKey->addKeyPart(TT_DB_FIRST_KEY_OFFSET,
			       TT_DB_KEY_LENGTH,
			       BINTYPE);

    accessTable = new _Tt_isam_file(access_table_file,
				    TT_DB_KEY_LENGTH+3*TT_DB_LONG_SIZE,
				    TT_DB_KEY_LENGTH+3*TT_DB_LONG_SIZE,
				    accessTableKey,
				    ISFIXLEN+ISINOUT+ISEXCLLOCK);
    dbLastFileAccessed = accessTable->getName();
    results = accessTable->getErrorStatus();

    if (!results && accessTable->isNew()) {
      (void)accessTable->writeMagicString(_Tt_string(TT_DB_VERSION));
    }
    else if (results) {
      dbResults = TT_DB_ERR_DB_OPEN_FAILED;
    }
  }
}

_Tt_db_server_db::~_Tt_db_server_db ()
{
}

_Tt_db_results _Tt_db_server_db::createFile (const _Tt_string        &file,
					     const _Tt_db_access_ptr &access)
{
  _Tt_db_key_ptr file_key = new _Tt_db_key;

  _Tt_isam_record_ptr record_ptr = fileTable->getEmptyRecord();
  record_ptr->setKeyPartValue(0, 0, file_key->binary());
  record_ptr->setBytes(TT_DB_FILE_PATH_OFFSET, file);

  int results = fileTable->writeRecord(record_ptr);
  dbLastFileAccessed = fileTable->getName();

  if (!results) {
    dbResults = TT_DB_OK;

    _Tt_string file_key_bytes(TT_DB_KEY_LENGTH);
    memcpy((char *)file_key_bytes,
	   (char *)file_key->binary(),
	   TT_DB_KEY_LENGTH);
    if (setAccess(file_key_bytes, access) != TT_DB_OK) {
      dbResults = TT_DB_ERR_CORRUPT_DB;
    }
  }
  else if (results == EDUPL) {
    dbResults = TT_DB_ERR_FILE_EXISTS;
  }
  else if (results == ELOCKED) {
    dbResults = TT_DB_ERR_DB_LOCKED;
  }
  else if (results == ENOSPC) {
    dbResults = TT_DB_ERR_DISK_FULL;
  }
  else {
    dbResults = TT_DB_ERR_CORRUPT_DB;
  }

  return dbResults;
}

_Tt_db_results
_Tt_db_server_db::createObject (const _Tt_string        &file,
			        const _Tt_string        &objid,
			        const _Tt_db_access_ptr &object_access,
				const _Tt_db_access_ptr &file_access)
{
  _Tt_string file_key;

  // If a file has been specified, get the key
  _Tt_string        real_file;
  if (!file.len()) {
    real_file = TT_DB_FORWARD_POINTER_FILE;
  }
  else {
    real_file = file;
  }

  dbResults = getFileKey(real_file, file_key);

  // If the file does not exist...
  if (dbResults == TT_DB_ERR_NO_SUCH_FILE) {
    _Tt_db_access_ptr real_file_access;
    if (!file.len()) {
      real_file_access = new _Tt_db_access;
      real_file_access->user = 0;
      real_file_access->group = 0;
      real_file_access->mode = (mode_t)-1;
    }
    else {
      real_file_access = file_access;
    }

    // Create the file
    dbResults = createFile(real_file, real_file_access);

    if (dbResults == TT_DB_OK) {
      // Get the new file key
      dbResults = getFileKey(real_file, file_key);
    }

    if (dbResults != TT_DB_OK) {
      return dbResults;
    }
  }
  // Else, if some sort of fatal error
  else if (dbResults != TT_DB_OK) {
    return dbResults;
  }
  
  // Verify access to the file
  if (verifyAccess(file_key, object_access, TRUE) != TT_DB_OK) {
    return dbResults;
  }

  if (dbResults == TT_DB_OK) {
    _Tt_string object_key = getObjectKey(objid);
    int        results;

    // Write a record to the file-object map
    _Tt_isam_record_ptr record_ptr = fileObjectMap->getEmptyRecord();
    record_ptr->setKeyPartValue(0, 0, object_key);
    record_ptr->setKeyPartValue(1, 0, file_key);
    results = fileObjectMap->writeRecord(record_ptr);
    dbLastFileAccessed = fileObjectMap->getName();

    if (!results) {
      if (setAccess(object_key, object_access) != TT_DB_OK) {
	dbResults = TT_DB_ERR_CORRUPT_DB;
      }
    }
    else if (results == EDUPL) {
      dbResults = TT_DB_ERR_OBJECT_EXISTS;
    }
    else if (results == ELOCKED) {
      dbResults = TT_DB_ERR_DB_LOCKED;
    }
    else if (results == ENOSPC) {
      dbResults = TT_DB_ERR_DISK_FULL;
    }
    else {
      dbResults = TT_DB_ERR_CORRUPT_DB;
    }
  }

  return dbResults;
}

_Tt_db_results _Tt_db_server_db::removeFile (const _Tt_string        &file,
					     const _Tt_db_access_ptr &access)
{
  _Tt_string file_key;
  if (getFileKey(file, file_key) != TT_DB_OK) {
    return dbResults;
  }

  if (verifyAccess(file_key, access, TRUE, TRUE) != TT_DB_OK) {
    return dbResults;
  }

  _Tt_string_list_ptr objids;

  // Find and remove the file's objects
  dbResults = getFileObjects(file, access, objids);
  if (dbResults == TT_DB_OK) {
    _Tt_string_list_cursor objids_cursor(objids);
    while (objids_cursor.next()) {
      dbResults = removeObject(*objids_cursor, access);
    }
  }
  else {
    return dbResults;
  }
  
  if (dbResults == TT_DB_OK) {
    dbResults = deleteProperties(file_key);
    
    if (dbResults == TT_DB_OK) {
      // Remove the file from the file table
      int results = fileTable->deleteCurrentRecord();
      dbLastFileAccessed = fileTable->getName();

      if (!results) {
	// Reposition the access table at the file's access record
        if (verifyAccess(file_key, access, TRUE, TRUE) != TT_DB_OK) {
          return dbResults;
        }

	// Remove the file's access info
	results = accessTable->deleteCurrentRecord();
	dbLastFileAccessed = accessTable->getName();
      }

      if (results) {
	dbResults = TT_DB_ERR_CORRUPT_DB;
      }
    }
  }

  return dbResults;
}

_Tt_db_results _Tt_db_server_db::removeObject (const _Tt_string        &objid,
					       const _Tt_db_access_ptr &access)
{
  _Tt_string object_key = getObjectKey(objid);
  if (verifyUserOnlyObjectAccess (object_key, access) != TT_DB_OK) {
    return dbResults;
  }

  dbResults = deleteProperties(object_key);

  if (dbResults == TT_DB_OK) {
    _Tt_isam_record_ptr record_ptr = fileObjectMap->getEmptyRecord();
    record_ptr->setKeyPartValue(0, 0, object_key);

    // Position the file just before the object in the file-object map
    int results = fileObjectMap->findStartRecord(fileObjectMapObjectKey,
                                                 0,
                                                 record_ptr,
                                                 ISEQUAL);
    dbLastFileAccessed = fileObjectMap->getName();
 
    if (!results) {
      // Read the object record
      (void)fileObjectMap->readRecord(ISNEXT);
      results = fileObjectMap->getErrorStatus();

      if (!results) {
	// Delete the record
        results = fileObjectMap->deleteCurrentRecord();
 
        if (!results) {
	  // Delete the object's access info
          results = accessTable->deleteCurrentRecord();
          dbLastFileAccessed = accessTable->getName();
        }
 
        if (results) {
          dbResults = TT_DB_ERR_CORRUPT_DB;
        }
      }
      else {
        dbResults = TT_DB_ERR_CORRUPT_DB;
      }
    }
    else {
      dbResults = TT_DB_ERR_CORRUPT_DB;
    }    
  }

  return dbResults;
}

_Tt_db_results
_Tt_db_server_db::setFileProperty (const _Tt_string          &file,
				   const _Tt_db_property_ptr &property,
				   const _Tt_db_access_ptr   &access)
{
  _Tt_string file_key;
  if (getFileKey(file, file_key) != TT_DB_OK) {
    return dbResults;
  }

  if (verifyAccess(file_key, access, TRUE) != TT_DB_OK) {
    return dbResults;
  }

  return setProperty(file_key, property);
}

_Tt_db_results _Tt_db_server_db
::setFileProperties (const _Tt_string               &file,
		     const _Tt_db_property_list_ptr &properties,
		     const _Tt_db_access_ptr        &access)
{
  _Tt_string file_key;
  if (getFileKey(file, file_key) != TT_DB_OK) {
    return dbResults;
  }

  if (verifyAccess(file_key, access, TRUE) != TT_DB_OK) {
    return dbResults;
  }

  return setProperties(file_key, properties);
}

_Tt_db_results
_Tt_db_server_db::addFileProperty (const _Tt_string          &file,
				   const _Tt_db_property_ptr &property,
				   bool_t                     unique,
				   const _Tt_db_access_ptr   &access)
{
  _Tt_string file_key;
  if (getFileKey(file, file_key) != TT_DB_OK) {
    return dbResults;
  }

  if (verifyAccess(file_key, access, TRUE) != TT_DB_OK) {
    return dbResults;
  }

  return addProperty(file_key, property, unique);
}

_Tt_db_results
_Tt_db_server_db::deleteFileProperty (const _Tt_string          &file,
				      const _Tt_db_property_ptr &property,
				      const _Tt_db_access_ptr   &access)
{
  _Tt_string file_key;
  if (getFileKey(file, file_key) != TT_DB_OK) {
    return dbResults;
  }

  if (verifyAccess(file_key, access, TRUE) != TT_DB_OK) {
    return dbResults;
  }

  return deleteProperty(file_key, property);
}

_Tt_db_results
_Tt_db_server_db::deleteFileProperties (const _Tt_string        &file,
				        const _Tt_db_access_ptr &access)
{
  _Tt_string file_key;
  if (getFileKey(file, file_key) != TT_DB_OK) {
    return dbResults;
  }

  if (verifyAccess(file_key, access, TRUE) != TT_DB_OK) {
    return dbResults;
  }

  return deleteProperties(file_key);
}

_Tt_db_results
_Tt_db_server_db::getFileProperty (const _Tt_string        &file,
				   const _Tt_string        &name,
				   const _Tt_db_access_ptr &access,
				   _Tt_db_property_ptr     &property)
{
  property = (_Tt_db_property *)NULL;

  _Tt_string file_key;
  if (getFileKey(file, file_key) != TT_DB_OK) {
    return dbResults;
  }

  if (verifyAccess(file_key, access) != TT_DB_OK) {
    return dbResults;
  }

  return getProperty(file_key, name, property);
}

_Tt_db_results _Tt_db_server_db
::getFileProperties (const _Tt_string         &file,
		     const _Tt_db_access_ptr  &access,
		     _Tt_db_property_list_ptr &properties)
{
  properties = (_Tt_db_property_list *)NULL;

  _Tt_string file_key;
  if (getFileKey(file, file_key) != TT_DB_OK) {
    return dbResults;
  }

  if (verifyAccess(file_key, access) != TT_DB_OK) {
    return dbResults;
  }

  return getProperties(file_key, properties);
}

_Tt_db_results
_Tt_db_server_db::getFileObjects (const _Tt_string        &file,
				  const _Tt_db_access_ptr &access,
				  _Tt_string_list_ptr     &objids)
{
  objids = (_Tt_string_list *)NULL;

  _Tt_string file_key;
  if (getFileKey(file, file_key) != TT_DB_OK) {
    return dbResults;
  }

  if (verifyAccess(file_key, access) != TT_DB_OK) {
    return dbResults;
  }

  _Tt_isam_record_ptr record_ptr = fileObjectMap->getEmptyRecord();
  record_ptr->setKeyPartValue(1, 0, file_key);

  // Position just before record with the specified file key in the
  // file-object map
  int results = fileObjectMap->findStartRecord(fileObjectMapFileKey,
					       0,
					       record_ptr,
					       ISEQUAL);
  dbLastFileAccessed = (char *)fileObjectMap->getName();

  if (results == ENOREC) {
    dbResults = TT_DB_OK;
  }
  else if (results) {
    dbResults = TT_DB_ERR_CORRUPT_DB;
  }
  else if (!results) {
    for (;;) {
      // Read the next file record
      record_ptr = fileObjectMap->readRecord(ISNEXT);
      results = fileObjectMap->getErrorStatus();

      if (!results) {
	// Extract the file key from the record just read
	_Tt_string file_key_bytes = record_ptr->getKeyPartValue(1, 0);

	// If the extracted key doesn't match the specified key,
	// then there are no more records with this key value,
	// therefore break out of the loop...
	if (file_key != file_key_bytes) {
	  dbResults = TT_DB_OK;
	  break;
	}
	// Else the file record matches...
	else {
	  // Extract the object key bytes and create a key object
	  _Tt_string object_key_bytes = record_ptr->getKeyPartValue(0, 0);
	  _Tt_db_key_ptr object_key = new _Tt_db_key(object_key_bytes);

	  // Construct the actual object ID
	  _Tt_string objid = _tt_db_create_objid(object_key,
					         "NFS",
					         dbHostname,
					         dbPartition);
	  if (objids.is_null()) {
	    objids = new _Tt_string_list;
	  }
	  objids->append(objid);
	}
      }
      // Else if no more records match the file key...
      else if ((results == ENOREC) || (results == EENDFILE)) {
	dbResults = TT_DB_OK;
	break;
      }
      else if (results) {
	dbResults = TT_DB_ERR_CORRUPT_DB;
	break;
      }
    }
  }

  return dbResults;
}
			       
_Tt_db_results
_Tt_db_server_db::deleteFileObjects (const _Tt_string        &file,
				     const _Tt_db_access_ptr &access)
{
  _Tt_string file_key;
  if (getFileKey(file, file_key) != TT_DB_OK) {
    return dbResults;
  }

  if (verifyAccess(file_key, access, TRUE) != TT_DB_OK) {
    return dbResults;
  }

  // Gets the object IDs, loop through them, and remove them...
  _Tt_string_list_ptr objids;
  if (getFileObjects(file, access, objids) == TT_DB_OK) {
    _Tt_string_list_cursor objids_cursor(objids);
    while (objids_cursor.next() && (dbResults == TT_DB_OK)) {
      dbResults = removeObject(*objids_cursor, access);
    }
  }

  return dbResults;
}

_Tt_db_results
_Tt_db_server_db::setFileFile (const _Tt_string        &file,
			       const _Tt_string        &new_file,
			       const _Tt_db_access_ptr &access)
{
  if (file == new_file) {
    return TT_DB_ERR_SAME_FILE;
  }

  _Tt_string file_key;

  // Use the "getFileKey" to position the current record of the
  // file table  on the record to update.
  if (getFileKey(file, file_key) != TT_DB_OK) {
    return dbResults;
  }

  if (verifyAccess(file_key, access, TRUE) != TT_DB_OK) {
    return dbResults;
  }

  // Re-read the current file-object map record
  _Tt_isam_record_ptr record_ptr;
  record_ptr = fileTable->readRecord(ISCURR);
  int results = fileTable->getErrorStatus();

  if (!results) {
    // Get a clean record
    _Tt_isam_record_ptr new_record = fileTable->getEmptyRecord();

    // Put the file key into the new record
    record_ptr->setKeyPartValue(0, 0, file_key);

    // Put the new file into the record
    record_ptr->setBytes(TT_DB_FILE_PATH_OFFSET, new_file);

    // Update the current file table record with the new info
    results = fileTable->updateCurrentRecord(record_ptr);
  }

  if (results) {
    dbResults = TT_DB_ERR_CORRUPT_DB;
  }

  return dbResults;
}

_Tt_db_results
_Tt_db_server_db::getFileChildren (const _Tt_string    &file,
				   _Tt_string_list_ptr &children)
{
  int results;

  // Construct the root path for finding children files
  _Tt_string root_path = file;
  root_path = root_path.cat("/");

  children = new _Tt_string_list;

  // Use the "getFileKey" to position the the file table on the first
  // record that matches our file and to see if the file exists
  // in the database.
  _Tt_string file_key;
  dbResults = getFileKey(file, file_key);
  if (dbResults == TT_DB_OK) {
    children->append(file);
  }
  else if (dbResults == TT_DB_ERR_NO_SUCH_FILE) {
    dbResults = TT_DB_OK;

    // Position the file at the first record that has the file name
    // as the root of its path
    _Tt_isam_record_ptr record_ptr = fileTable->getEmptyRecord();
    record_ptr->setKeyPartValue(1, 0, root_path);

    results =
      fileTable->findStartRecord(fileTableFilePathKey,
				 ((root_path.len() < TT_DB_MAX_KEY_LENGTH) ?
				  root_path.len() : TT_DB_MAX_KEY_LENGTH),
				 record_ptr,
				 ISEQUAL);
    dbLastFileAccessed = fileTable->getName();

    if (results == ENOREC) {
      dbResults = TT_DB_ERR_NO_SUCH_FILE;
    }
    else if (results) {
      dbResults = TT_DB_ERR_CORRUPT_DB;
    }
  }

  if (dbResults == TT_DB_OK) {
    _Tt_isam_record_ptr record_ptr;

    // Loop through the children files
    results = 0;
    while (!results) {
      record_ptr = fileTable->readRecord(ISNEXT);
      results = fileTable->getErrorStatus();

      if (!results) {
	_Tt_string child = record_ptr->getBytes(TT_DB_FILE_PATH_OFFSET, 0);

	// Make sure the record just read contains a child of the root path
	if (!strncmp((char *)root_path, (char *)child, root_path.len())) {
	  children->append(_Tt_string((char *)child));
	}
	// Else, no more children left...
	else {
	  results = ENOREC;
	}
      }
    }

    if ((results == ENOREC) || (results == EENDFILE)) {
      dbResults = TT_DB_OK;
    }
    else {
      dbResults = TT_DB_ERR_CORRUPT_DB;
    }
  }

  return dbResults;
}

_Tt_db_results
_Tt_db_server_db::setFileAccess (const _Tt_string        &file,
				 const _Tt_db_access_ptr &new_access,
				 const _Tt_db_access_ptr &access)
{
  _Tt_string file_key;
  if (getFileKey(file, file_key) != TT_DB_OK) {
    return dbResults;
  }

  (void)verifyAccess(file_key, access, TRUE, TRUE);
  if ((dbResults != TT_DB_OK) && (dbResults != TT_DB_ERR_NO_ACCESS_INFO)) {
    return dbResults;
  }
  
  return setAccess(file_key, new_access);
}

_Tt_db_results
_Tt_db_server_db::getFileAccess (const _Tt_string        &file,
			         const _Tt_db_access_ptr &access,
				 _Tt_db_access_ptr       &current_access)
{
  current_access = (_Tt_db_access *)NULL;

  _Tt_string file_key;
  if (getFileKey(file, file_key) != TT_DB_OK) {
    return dbResults;
  }

  if (verifyAccess(file_key, access) != TT_DB_OK) {
    return dbResults;
  }

  return getAccess(file_key, current_access);
}

_Tt_db_results
_Tt_db_server_db::setObjectProperty (const _Tt_string          &objid,
				     const _Tt_db_property_ptr &property,
				     const _Tt_db_access_ptr   &access)
{
  _Tt_string object_key = getObjectKey(objid);
  if (verifyObjectAccess(object_key, access, TRUE) != TT_DB_OK) {
    return dbResults;
  }

  return setProperty(object_key, property);
}

_Tt_db_results _Tt_db_server_db
::setObjectProperties (const _Tt_string               &objid,
		       const _Tt_db_property_list_ptr &properties,
		       const _Tt_db_access_ptr        &access)
{
  _Tt_string object_key = getObjectKey(objid);
  if (verifyObjectAccess(object_key, access, TRUE) != TT_DB_OK) {
    return dbResults;
  }

  return setProperties(object_key, properties);
}

_Tt_db_results
_Tt_db_server_db::addObjectProperty (const _Tt_string          &objid,
				     const _Tt_db_property_ptr &property,
				     bool_t                     unique,
				     const _Tt_db_access_ptr   &access)
{
  _Tt_string object_key = getObjectKey(objid);
  if (verifyObjectAccess(object_key, access, TRUE) != TT_DB_OK) {
    return dbResults;
  }

  return addProperty(object_key, property, unique);
}

_Tt_db_results
_Tt_db_server_db::deleteObjectProperty (const _Tt_string          &objid,
				        const _Tt_db_property_ptr &property,
				        const _Tt_db_access_ptr   &access)
{
  _Tt_string object_key = getObjectKey(objid);
  if (verifyObjectAccess(object_key, access, TRUE) != TT_DB_OK) {
    return dbResults;
  }

  return deleteProperty(object_key, property);
}

_Tt_db_results
_Tt_db_server_db::deleteObjectProperties (const _Tt_string        &objid,
					  const _Tt_db_access_ptr &access)
{
  _Tt_string object_key = getObjectKey(objid);
  if (verifyObjectAccess(object_key, access, TRUE) != TT_DB_OK) {
    return dbResults;
  }

  return deleteProperties(object_key);
}

_Tt_db_results
_Tt_db_server_db::getObjectProperty (const _Tt_string        &objid,
				     const _Tt_string        &name,
				     const _Tt_db_access_ptr &access,
				     _Tt_db_property_ptr     &property)
{
  property = (_Tt_db_property *)NULL;

  _Tt_string object_key = getObjectKey(objid);
  if (verifyObjectAccess(object_key, access) != TT_DB_OK) {
    return dbResults;
  }

  return getProperty(object_key, name, property);
}

_Tt_db_results _Tt_db_server_db
::getObjectProperties (const _Tt_string         &objid,
		       const _Tt_db_access_ptr  &access,
		       _Tt_db_property_list_ptr &properties)
{
  properties = (_Tt_db_property_list *)NULL;

  _Tt_string object_key = getObjectKey(objid);
  if (verifyObjectAccess(object_key, access) != TT_DB_OK) {
    return dbResults;
  }

  return getProperties(object_key, properties);
}

_Tt_db_results
_Tt_db_server_db::setObjectFile (const _Tt_string        &objid,
				 const _Tt_string        &file,
				 const _Tt_db_access_ptr &access)
{
  _Tt_string object_key = getObjectKey(objid);
  if (verifyObjectAccess(object_key, access) != TT_DB_OK) {
    return dbResults;
  }

  // Use "getFile" to position the current record of the file-object
  // map on the record to update.  Plus we need to get the old file
  // name to make sure a move is really needed... 
  _Tt_string old_file;
  dbResults = getFile(object_key, old_file);

  if (dbResults == TT_DB_OK) {
    // If the names are the same, then the move is not needed...
    if (old_file == file) {
      dbResults = TT_DB_ERR_SAME_OBJECT;
    }
    // Else, the move is needed...
    else {
      // Re-read the current file-object map record
      _Tt_isam_record_ptr record_ptr;
      record_ptr = fileObjectMap->readRecord(ISCURR);
      int results = fileObjectMap->getErrorStatus();

      if (!results) {
	// See if the file exists by trying to obtain its key
	_Tt_string file_key;
	dbResults = getFileKey(file, file_key);

	// If the file key was found...
	if (dbResults == TT_DB_OK) {
	  // Put the new file key into the record and update the table
	  record_ptr->setKeyPartValue(1, 0, file_key);
	  results = fileObjectMap->updateCurrentRecord(record_ptr);

	  if (results) {
	    dbResults = TT_DB_ERR_CORRUPT_DB;
	  }
	}
	// Else, the file is not in the DB...
	else if (dbResults == TT_DB_ERR_NO_SUCH_FILE) {
	  // Create a new file using the permissions of the object
          _Tt_db_access_ptr object_access;
	  dbResults = getAccess(object_key, object_access); 

	  if (dbResults == TT_DB_OK) {
	    dbResults = createFile (file, object_access);
	  }

	  if (dbResults == TT_DB_OK) {
	    // Get the new file key
	    if (getFileKey(file, file_key) == TT_DB_OK) {
	      // Put the new file key into the record and update the table
	      record_ptr->setKeyPartValue(1, 0, file_key);
	      results = fileObjectMap->updateCurrentRecord(record_ptr);

	      if (results) {
		dbResults = TT_DB_ERR_CORRUPT_DB;
	      }
	    }
	  }
	}
      }
      else {
        dbResults = TT_DB_ERR_CORRUPT_DB;
      }
    }
  }

  return dbResults;
}

_Tt_db_results
_Tt_db_server_db::getObjectFile (const _Tt_string        &objid,
				 const _Tt_db_access_ptr &access,
				 _Tt_string              &file)
{
  file = (char *)NULL;

  _Tt_string object_key = getObjectKey(objid);
  if (verifyObjectAccess(object_key, access) != TT_DB_OK) {
    return dbResults;
  }

  return getFile(object_key, file);
}

_Tt_db_results
_Tt_db_server_db::setObjectAccess (const _Tt_string        &objid,
				   const _Tt_db_access_ptr &new_access,
				   const _Tt_db_access_ptr &access)
{
  _Tt_string object_key = getObjectKey(objid);
  (void)verifyObjectAccess(object_key, access, TRUE, TRUE);
  if ((dbResults != TT_DB_OK) && (dbResults != TT_DB_ERR_NO_ACCESS_INFO)) {
    return dbResults;
  }

  return setAccess(object_key, new_access);
}

_Tt_db_results
_Tt_db_server_db::getObjectAccess (const _Tt_string        &objid,
				   const _Tt_db_access_ptr &access,
				   _Tt_db_access_ptr       &current_access)
{
  current_access = (_Tt_db_access *)NULL;

  _Tt_string object_key = getObjectKey(objid);
  if (verifyObjectAccess(object_key, access) != TT_DB_OK) {
    return dbResults;
  }

  return getAccess(object_key, current_access);
}

_Tt_db_results
_Tt_db_server_db::verifyObjectAccess (const _Tt_string        &object_key,
				      const _Tt_db_access_ptr &access,
				      bool_t                   write,
				      bool_t                   user_only)
{
  dbResults = verifyAccess(object_key, access, write, user_only);

  if (dbResults == TT_DB_ERR_NO_ACCESS_INFO) {
    _Tt_isam_record_ptr record_ptr = fileObjectMap->getEmptyRecord();
    record_ptr->setKeyPartValue(0, 0, object_key);

    // Position the file just before the object in the file-object map
    int results = fileObjectMap->findStartRecord(fileObjectMapObjectKey,
                                                 0,
                                                 record_ptr,
                                                 ISEQUAL);
    dbLastFileAccessed = fileObjectMap->getName();

    if (results == ENOREC) {
      dbResults = TT_DB_ERR_NO_SUCH_OBJECT;
    }
    else if (results) {
      dbResults = TT_DB_ERR_CORRUPT_DB;
    }
  }

  return dbResults;
}

_Tt_db_results _Tt_db_server_db
::verifyUserOnlyObjectAccess (const _Tt_string        &object_key,
			      const _Tt_db_access_ptr &access)
{
  if (verifyAccess(object_key, access, TRUE, TRUE) != TT_DB_OK) {
    _Tt_string file;
    if (getFile(object_key, file) == TT_DB_OK) {
      _Tt_string file_key;
      if (getFileKey(file, file_key) == TT_DB_OK) {
        dbResults = verifyAccess(file_key, access, TRUE, TRUE);
      }
    }
  }  

  return dbResults;
}

_Tt_db_results
_Tt_db_server_db::verifyAccess (const _Tt_string        &key,
				const _Tt_db_access_ptr &access,
				bool_t                   write,
				bool_t                   user_only)
{
  _Tt_isam_record_ptr record_ptr = accessTable->getEmptyRecord();
  record_ptr->setKeyPartValue(0, 0, key);

  // Position the file just before the specified key in the access table
  int results = accessTable->findStartRecord(accessTableKey,
					     0,
					     record_ptr,
					     ISEQUAL);
  dbLastFileAccessed = accessTable->getName();

  if (results == ENOREC) {
    dbResults = TT_DB_ERR_NO_ACCESS_INFO;
  }
  else if (results) {
    dbResults = TT_DB_ERR_CORRUPT_DB;
  }
  else {
    // Read the access record
    record_ptr = accessTable->readRecord(ISNEXT);
    results = accessTable->getErrorStatus();
    
    if (results) {
      dbResults = TT_DB_ERR_CORRUPT_DB;
    }
    else if (!results) {
      long n_user = *(long *)
		     ((char *)record_ptr->getRecord()+TT_DB_ACCESS_USER_OFFSET);
      uid_t user = (uid_t)ntohl(n_user);

      long n_group = *(long *)
		      ((char *)record_ptr->getRecord()+
			       TT_DB_ACCESS_GROUP_OFFSET);
      gid_t group = (gid_t)ntohl(n_group);

      long n_mode = *(long *)
		     ((char *)record_ptr->getRecord()+TT_DB_ACCESS_MODE_OFFSET);
      mode_t mode = (mode_t)ntohl(n_mode);

      // If the user in the DB is -1, all users match
      bool_t user_flag = FALSE;
      if (user == -1) {
        user_flag = TRUE;
      }
      else {
        if (user == access->user) {
          user_flag = TRUE;
        }
      }

      // If the group in the DB is -1, all groups match
      bool_t group_flag = FALSE;
      if (group == -1) {
        group_flag = TRUE;
      }
      else {
        if (group == access->group) {
          group_flag = TRUE;
        }
      }
      
      dbResults = TT_DB_ERR_ACCESS_DENIED;

      // Root can do everything
      if (access->user == 0) {
	return (dbResults = TT_DB_OK);
      }
      
      // User of object can do anything
      if (user_flag) {
	return (dbResults = TT_DB_OK);
      }

      if (!user_only) {
        if (mode == (mode_t)-1) {
	  return (dbResults = TT_DB_OK);
        }

        if (group_flag && (dbResults != TT_DB_OK)) {
	  if (write && (mode&S_IWGRP)) {
	    return (dbResults = TT_DB_OK);
	  }
	  else if (mode&S_IRGRP) {
	    return (dbResults = TT_DB_OK);
	  }
        }
      
        if (dbResults != TT_DB_OK) {
          if (write && (mode&S_IWOTH)) {
	    return (dbResults = TT_DB_OK);
          }
          else if (mode&S_IROTH) {
            return (dbResults = TT_DB_OK);
          }
        }
      }
    }
  }

  return dbResults;
}

_Tt_db_results _Tt_db_server_db::getFileKey (const _Tt_string &file,
					     _Tt_string       &file_key)
{
  dbResults = TT_DB_OK;

  _Tt_isam_record_ptr record_ptr = fileTable->getEmptyRecord();
  record_ptr->setKeyPartValue(1, 0, file);

  // Position file just before record with specified file path in the
  // file table
  int results =
    fileTable->findStartRecord(fileTableFilePathKey,
			       ((file.len() < TT_DB_MAX_KEY_LENGTH) ?
				file.len() : TT_DB_MAX_KEY_LENGTH),
			       record_ptr,
			       ISEQUAL);
  dbLastFileAccessed = fileTable->getName();

  if (results == ENOREC) {
    dbResults = TT_DB_ERR_NO_SUCH_FILE;
  }
  else if (results) {
    dbResults = TT_DB_ERR_CORRUPT_DB;
  }
  else {
    for (;;) {
      // Read the next record
      record_ptr = fileTable->readRecord(ISNEXT);
      results = fileTable->getErrorStatus();

      if (!results) {
	// Extract the full file path from the record
	_Tt_string temp_file = (char *)
                               record_ptr->getBytes(TT_DB_FILE_PATH_OFFSET, 0);

	// If the record file path matchs the specified file, then
	// we found our file, therefore break out of the loop...
	if (file == temp_file) {
	  break;
	}
      }
      // No more records left, our file doesn't exist...
      else if ((results == ENOREC) || (results == EENDFILE)) {
	dbResults = TT_DB_ERR_NO_SUCH_FILE;
	break;
      }
      else {
	dbResults = TT_DB_ERR_CORRUPT_DB;
	break;
      }
    }

    if (dbResults == TT_DB_OK) {
      // Extract the file key
      file_key = record_ptr->getKeyPartValue(0, 0);
    }
  }

  return dbResults;
}

_Tt_string _Tt_db_server_db::getObjectKey (const _Tt_string &objid)
{
  // Create a key object from the specified object ID
  _Tt_db_key_ptr object_key = new _Tt_db_key(objid);

  // Write the binary key to a return buffer
  _Tt_string object_key_bytes(TT_DB_KEY_LENGTH);
  memcpy((char *)object_key_bytes,
	 (char *)object_key->binary(),
         TT_DB_KEY_LENGTH);

  return object_key_bytes;
}

_Tt_db_results _Tt_db_server_db::addPropertyValue(const _Tt_string &key,
						  const _Tt_string &name,
						  const _Tt_string &value)
{
  dbResults = TT_DB_OK;

  _Tt_isam_record_ptr record_ptr = propertyTable->getEmptyRecord();
  record_ptr->setKeyPartValue(0, 0, key);
  record_ptr->setKeyPartValue(0, 1, name);

  record_ptr->setBytes(TT_DB_PROPERTY_VALUE_OFFSET, value);
  record_ptr->setLength(TT_DB_PROPERTY_VALUE_OFFSET+value.len());

  int results = propertyTable->writeRecord(record_ptr);
  dbLastFileAccessed = propertyTable->getName();

  if (results == ENOSPC) {
    dbResults = TT_DB_ERR_DISK_FULL;
  }
  else if (results == ELOCKED) {
    dbResults = TT_DB_ERR_DB_LOCKED;
  }
  else if (results) {
    dbResults = TT_DB_ERR_CORRUPT_DB;
  }

  return dbResults;
}

_Tt_db_results
_Tt_db_server_db::setProperty (const _Tt_string          &key,
			       const _Tt_db_property_ptr &property)
{
  _Tt_db_property_ptr temp_property = new _Tt_db_property;
  temp_property->name = property->name;
  (void)deleteProperty(key, temp_property);

  return addProperty(key, property, FALSE);
}

_Tt_db_results
_Tt_db_server_db::setProperties (const _Tt_string               &key,
				 const _Tt_db_property_list_ptr &properties)
{
  dbResults = deleteProperties(key, TRUE);

  _Tt_db_property_list_cursor properties_cursor(properties);
  while (properties_cursor.next()) {
    dbResults = setProperty(key, *properties_cursor);
  }

  return dbResults;
}

_Tt_db_results
_Tt_db_server_db::addProperty (const _Tt_string          &key,
			       const _Tt_db_property_ptr &property,
			       bool_t                     unique)
{
  dbResults = TT_DB_OK;

  _Tt_isam_record_ptr record_ptr;
  _Tt_string          temp_string;

  _Tt_string_list_cursor values_cursor(property->values);
  while (values_cursor.next() && (dbResults == TT_DB_OK)) {
    // If unique... therefore add the value to the property only if
    // the key, prop name and prop value combo does not already exist...
    if (unique) {
      record_ptr = propertyTable->getEmptyRecord();
      record_ptr->setKeyPartValue(0, 0, key);
      record_ptr->setKeyPartValue(0, 1, property->name);
  
      // Position the file just before the first record with the specified key
      // and property name value in the property table
      int results = propertyTable->findStartRecord(propertyTablePropertyKey,
						   0,
						   record_ptr,
						   ISEQUAL);
      dbLastFileAccessed = propertyTable->getName();

      // No such property, add the value
      if (results == ENOREC) {
	dbResults = addPropertyValue(key, property->name, *values_cursor);
      }
      else if (results) {
	dbResults = TT_DB_ERR_CORRUPT_DB;
      }
      // Property exists, let's check on its values...
      else {
	bool_t found = FALSE;
	for (;;) {
	  // Read the next record
	  record_ptr = propertyTable->readRecord(ISNEXT);
	  results = propertyTable->getErrorStatus();

	  if (!results) {
	    // Extract the key from the record
	    temp_string = record_ptr->getKeyPartValue(0, 0);

	    // If the key doesn't match, we're done looking for values...
	    if (temp_string != key) {
	      break;
	    }

	    // Extract the property name from the record
	    temp_string = record_ptr->getKeyPartValue(0, 1);

	    // If the property name doesn't match, we're done looking
	    // for values.
	    if (strcmp((char *)temp_string, (char *)property->name)) {
	      break;
	    }

	    // Extract the property value from the record
	    temp_string = record_ptr->getBytes(TT_DB_PROPERTY_VALUE_OFFSET, 0);

	    // If the values are the same, the new property is not unique,
	    // so let's leave...
	    if (temp_string == *values_cursor) {
	      found = TRUE;
	      break;
	    }
	  }
	  // No more records, the new value is unique...
	  else if ((results == ENOREC) || (results == EENDFILE)) {
	    break;
	  }
	  else {
	    found = TRUE;
	    dbResults = TT_DB_ERR_CORRUPT_DB;
	    break;
	  }
	}

	// If the same value wasn't found, the new value is unique,
	// therefore add it...
	if (!found) {
	  dbResults = addPropertyValue(key, property->name, *values_cursor);
	}
      }
    }
    // Else, who cares whether it's unique or not, just add it...
    else {
      dbResults = addPropertyValue(key, property->name, *values_cursor);
    }
  }

  return dbResults;
}

_Tt_db_results
_Tt_db_server_db::deleteProperty (const _Tt_string          &key,
				  const _Tt_db_property_ptr &property)
{
  dbResults = TT_DB_OK;

  _Tt_isam_record_ptr record_ptr = propertyTable->getEmptyRecord();
  record_ptr->setKeyPartValue(0, 0, key);
  record_ptr->setKeyPartValue(0, 1, property->name);

  // Position the file just before the first record with the specified key
  // and property name value in the property table
  int results = propertyTable->findStartRecord(propertyTablePropertyKey,
					       0,
					       record_ptr,
					       ISEQUAL);
  dbLastFileAccessed = propertyTable->getName();

  bool_t found = FALSE;
  if (results == ENOREC) {
    dbResults = TT_DB_ERR_NO_SUCH_PROPERTY;
  }
  else if (results) {
    dbResults = TT_DB_ERR_CORRUPT_DB;
  }
  else {
    _Tt_string_list_cursor values_cursor(property->values);
    for (;;) {
      // Read the next record
      record_ptr = propertyTable->readRecord(ISNEXT);
      results = propertyTable->getErrorStatus();

      // No more records, let's get out of here...
      if ((results == ENOREC) || (results == EENDFILE)) {
	break;
      }
      else if (results) {
	dbResults = TT_DB_ERR_CORRUPT_DB;
	break;
      }
      else {
        // Extract the key from the record
        _Tt_string temp_string = record_ptr->getKeyPartValue(0, 0);

	// If the key doesn't match, we're done looking for values...
	if (temp_string != key) {
	  break;
	}

	// Extract the property name from the record
	temp_string = record_ptr->getKeyPartValue(0, 1);

	// If the property name doesn't match, we're done looking
	// for values.
	if (strcmp((char *)property->name, (char *)temp_string)) {
	  break;
	}
	
	// If no specific values to delete were specified, delete all of
	// the values we find...
	if (property->is_empty()) {
	  found = TRUE;
	  results = propertyTable->deleteCurrentRecord();
	  if (results) {
	    dbResults = TT_DB_ERR_CORRUPT_DB;
	    break;
	  }
	}
	// Else specific values to delete have been specified...
	else {
	  // Extract the property value from the record
	  _Tt_string property_value =
	    record_ptr->getBytes(TT_DB_PROPERTY_VALUE_OFFSET, 0);

	  // Loop through the specified values and see if any match
	  // the value in the record.  If one matches, delete the
	  // record...
	  values_cursor.reset();  
	  while (values_cursor.next()) {
	    if (property_value == *values_cursor) {
	      found = TRUE;
	      results = propertyTable->deleteCurrentRecord();
	      break;
	    }
	  }

	  if (results) {
	    dbResults = TT_DB_ERR_CORRUPT_DB;
	    break;
	  }
	}
      }
    }
  }

  if ((dbResults == TT_DB_OK) && (!found)) {
    dbResults = TT_DB_ERR_NO_SUCH_PROPERTY;
  }

  return dbResults;
}

_Tt_db_results _Tt_db_server_db
::deleteProperties (const _Tt_string &key,
		    bool_t            preserve_special_properties)
{
  dbResults = TT_DB_OK;

  _Tt_isam_record_ptr record_ptr = propertyTable->getEmptyRecord();
  record_ptr->setKeyPartValue(0, 0, key);
    
  // Position the file just before the record with the specified key in
  // the property table
  int results = propertyTable->findStartRecord(propertyTablePropertyKey,
					       TT_DB_KEY_LENGTH,
					       record_ptr,
					       ISEQUAL);
  dbLastFileAccessed = propertyTable->getName();

  _Tt_string temp_string;
  for (;;) {
    if (!results) {
      // Read the property record
      record_ptr = propertyTable->readRecord(ISNEXT);
      results = propertyTable->getErrorStatus();

      if (!results) {
        // Extract the key from the record
        temp_string = record_ptr->getKeyPartValue(0, 0);

	// If the key doesn't match, we're done looking for properties...
	if (temp_string != key) {
	  break;
	}

	// If we need to preserve special properties, i.e. object type
	// and cache level properties
	if (preserve_special_properties) {
          temp_string = record_ptr->getKeyPartValue(0, 1);

	  if (strcmp((char *)temp_string, TT_DB_PROPS_CACHE_LEVEL_PROPERTY) &&
	      strcmp((char *)temp_string, TT_DB_OBJECT_TYPE_PROPERTY)) {
            results = propertyTable->deleteCurrentRecord();
            if (results) {
              dbResults = TT_DB_ERR_CORRUPT_DB;
            }
	  }
	}
	else {
	  results = propertyTable->deleteCurrentRecord();
	  if (results) {
	    dbResults = TT_DB_ERR_CORRUPT_DB;
	  }
        }
      }
    }
    else if ((results == EENDFILE) || (results == ENOREC)) {
      break;
    }
    else {
      dbResults = TT_DB_ERR_CORRUPT_DB;
      break;
    }
  }

  return dbResults;
}

_Tt_db_results
_Tt_db_server_db::getProperty (const _Tt_string    &key,
			       const _Tt_string    &name,
			       _Tt_db_property_ptr &property)
{
  dbResults = TT_DB_OK;

  _Tt_isam_record_ptr record_ptr = propertyTable->getEmptyRecord();
  record_ptr->setKeyPartValue(0, 0, key);
  record_ptr->setKeyPartValue(0, 1, name);

  // Position the file just before the first record with the specified key
  // and property name value in the property table
  int results = propertyTable->findStartRecord(propertyTablePropertyKey,
					       0,
					       record_ptr,
					       ISEQUAL);
  dbLastFileAccessed = (char *)propertyTable->getName();

  if (results == ENOREC) {
    dbResults = TT_DB_ERR_NO_SUCH_PROPERTY;
  }
  else if (results) {
    dbResults = TT_DB_ERR_CORRUPT_DB;
  }
  else {
    // Create the object to return the property in
    property = new _Tt_db_property;
    property->name = name;

    _Tt_string temp_string;
    for (;;) {
      // Read the next record
      record_ptr = propertyTable->readRecord(ISNEXT);
      results = propertyTable->getErrorStatus();

      if (!results) {
        // Extract the key from the record
        temp_string = record_ptr->getKeyPartValue(0, 0);

	// If the key does not match, we're done looking for values...
	if (temp_string != key) {
	  break;
	}
	
	// Extract the property name from the record
	temp_string = record_ptr->getKeyPartValue(0, 1);

	// If the property name doesn't match, we're done looking
	// for values.
	if (strcmp((char *)name, (char *)temp_string)) {
	  break;
	}
	
	// Extract the property value from the record and append the
	// value to the object being returned
	temp_string = record_ptr->getBytes(TT_DB_PROPERTY_VALUE_OFFSET, 0);
	property->values->append(temp_string);
      }
      // No more records, we're done getting the property...
      else if ((results == ENOREC) || (results == EENDFILE)) {
	break;
      }
      else {
	dbResults = TT_DB_ERR_CORRUPT_DB;
	break;
      }
    }
  }

  return dbResults;
}

_Tt_db_results
_Tt_db_server_db::getProperties (const _Tt_string         &key,
				 _Tt_db_property_list_ptr &properties)
{
  dbResults = TT_DB_OK;

  _Tt_isam_record_ptr record_ptr = propertyTable->getEmptyRecord();
  record_ptr->setKeyPartValue(0, 0, key);

  // Position the file just before the first record with the specified key
  // in the property table
  int results = propertyTable->findStartRecord(propertyTablePropertyKey,
					       TT_DB_KEY_LENGTH,
					       record_ptr,
					       ISEQUAL);
  dbLastFileAccessed = propertyTable->getName();

  if (results && (results != ENOREC)) {
    dbResults = TT_DB_ERR_CORRUPT_DB;
  }
  else {
    _Tt_string name;
    _Tt_string value;
    _Tt_string temp_string;
    for (;;) {
      // Read the next record
      record_ptr = propertyTable->readRecord(ISNEXT);
      results = propertyTable->getErrorStatus();

      if ((results == ENOREC) || (results == EENDFILE)) {
	break;
      }
      else if (results) {
	dbResults = TT_DB_ERR_CORRUPT_DB;
	break;
      }
      else {
        // Extract the key from the record
        temp_string = record_ptr->getKeyPartValue(0, 0);

	// If the key does not match, we're done looking for properties...
	if (temp_string != key) {
	  break;
	}
	
	// Extract the property name and value from the reocrd
	name = record_ptr->getKeyPartValue(0, 1);
	value = record_ptr->getBytes(TT_DB_PROPERTY_VALUE_OFFSET, 0);

	if (properties.is_null()) {
	  properties = new _Tt_db_property_list;
	}

	bool_t              found = FALSE;
	_Tt_db_property_ptr property;

	// See if a property with same name is already in our return list...
	_Tt_db_property_list_cursor properties_cursor(properties);
	while (properties_cursor.next()) {
	  // If the property exists, get the pointer to it...
	  if (properties_cursor->name == name) {
	    property = *properties_cursor;
	    found = TRUE;
	    break;
	  }	    
	}

	// If there is no property with the same name in the list,
	// create a new property and it to the list...
	if (!found) {
	  property = new _Tt_db_property;
	  properties->append(property);
	}

	// Append the value to the property
	property->name = name;
	property->values->append(value);
      }
    }
  }

  return dbResults;
}

_Tt_db_results _Tt_db_server_db::setAccess (const _Tt_string        &key,
					    const _Tt_db_access_ptr &access)
{
  dbResults = TT_DB_OK;

  _Tt_isam_record_ptr record_ptr = accessTable->getEmptyRecord();
  record_ptr->setKeyPartValue(0, 0, key);

  // Position the file just before the access info record for the key
  int results = accessTable->findStartRecord(accessTableKey,
					     0,
					     record_ptr,
					     ISEQUAL);
  dbLastFileAccessed = accessTable->getName();

  if (results == ENOREC) {
    *(long *)
     ((char *)record_ptr->getRecord()+
	      TT_DB_ACCESS_USER_OFFSET) = htonl(access->user);
    *(long *)
     ((char *)record_ptr->getRecord()+
              TT_DB_ACCESS_GROUP_OFFSET) = htonl(access->group);
    *(long *)
     ((char *)record_ptr->getRecord()+
              TT_DB_ACCESS_MODE_OFFSET) = htonl(access->mode);
 
    int results = accessTable->writeRecord(record_ptr);
    if (results == ENOSPC) {
      dbResults = TT_DB_ERR_DISK_FULL;
    }
    else if (results) {
      dbResults = TT_DB_ERR_CORRUPT_DB;
    }
  }
  else if (results) {
    dbResults = TT_DB_ERR_CORRUPT_DB;
  }
  else {
    // Read the access record
    record_ptr = accessTable->readRecord(ISNEXT);
    results = accessTable->getErrorStatus();
      
    if (results) {
      dbResults = TT_DB_ERR_CORRUPT_DB;
    }
    else if (!results) {
      *(long *)
       ((char *)record_ptr->getRecord()+
        TT_DB_ACCESS_USER_OFFSET) = htonl(access->user);
      *(long *)
       ((char *)record_ptr->getRecord()+
                TT_DB_ACCESS_GROUP_OFFSET) = htonl(access->group);
      *(long *)
       ((char *)record_ptr->getRecord()+
                TT_DB_ACCESS_MODE_OFFSET) = htonl(access->mode);

      // Update the record record that was read
      results = accessTable->updateCurrentRecord(record_ptr);
      dbResults = (results ? TT_DB_ERR_CORRUPT_DB : TT_DB_OK);
    }
  }

  return dbResults;
}

_Tt_db_results _Tt_db_server_db::getAccess (const _Tt_string  &key,
					    _Tt_db_access_ptr &access)
{
  _Tt_isam_record_ptr record_ptr = accessTable->getEmptyRecord();
  record_ptr->setKeyPartValue(0, 0, key);

  // Position the file just before the access info record for the key
  int results = accessTable->findStartRecord(accessTableKey,
					     0,
					     record_ptr,
					     ISEQUAL);
  dbLastFileAccessed = accessTable->getName();

  if (results) {
    dbResults = TT_DB_ERR_CORRUPT_DB;
  }
  else {
    // Read the access record
    record_ptr = accessTable->readRecord(ISNEXT);
    results = accessTable->getErrorStatus();
    
    if (results) {
      dbResults = TT_DB_ERR_CORRUPT_DB;
    }
    else if (!results) {
      access = new _Tt_db_access;

      long n_user = *(long *)
		     ((char *)record_ptr->getRecord()+
		              TT_DB_ACCESS_USER_OFFSET);
      access->user = (uid_t)ntohl(n_user);

      long n_group = *(long *)
		      ((char *)record_ptr->getRecord()+
		               TT_DB_ACCESS_GROUP_OFFSET);
      access->group = (gid_t)ntohl(n_group);

      long n_mode = *(long *)
		     ((char *)record_ptr->getRecord()+
		              TT_DB_ACCESS_MODE_OFFSET);
      access->mode = (mode_t)ntohl(n_mode);

      dbResults = TT_DB_OK;
    }
  }

  return dbResults;
}

_Tt_db_results _Tt_db_server_db::getFile (const _Tt_string &key,
                                          _Tt_string       &file) 
{ 
  _Tt_isam_record_ptr record_ptr = fileObjectMap->getEmptyRecord();
  record_ptr->setKeyPartValue(0, 0, key);
 
  // Position the file just before the object in the file-object map
  int results = fileObjectMap->findStartRecord(fileObjectMapObjectKey,
                                               0,
                                               record_ptr,
                                               ISEQUAL);
  dbLastFileAccessed = fileObjectMap->getName();
 
  if (results) { 
    dbResults = TT_DB_ERR_CORRUPT_DB;
  }
  else { 
    // Read the object record
    record_ptr = fileObjectMap->readRecord(ISNEXT);
    results = fileObjectMap->getErrorStatus();
 
    if (!results) {
      // Extract the file key from the record
      _Tt_string file_key = record_ptr->getKeyPartValue(1, 0);
 
      record_ptr = fileTable->getEmptyRecord();
      record_ptr->setKeyPartValue(0, 0, file_key);
 
      // Position the file just before the file in the file table
      int results = fileTable->findStartRecord(fileTableFileKey,
                                               0,
                                               record_ptr,
                                               ISEQUAL);
      dbLastFileAccessed = fileTable->getName();
 
      if (results) {
        dbResults = TT_DB_ERR_CORRUPT_DB;
      }  
      else {
        // Read the file record
        record_ptr = fileTable->readRecord(ISNEXT);
        if (results) {    
          dbResults = TT_DB_ERR_CORRUPT_DB;
        }
        else {   
          // Extract the file path from the record
          file = record_ptr->getBytes(TT_DB_FILE_PATH_OFFSET, 0);
          dbResults = TT_DB_OK;
        }
      } 
    }    
    else {
      dbResults = TT_DB_ERR_CORRUPT_DB;
    }    
  }
    
  return dbResults;
}
