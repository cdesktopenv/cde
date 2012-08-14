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
//%%  $XConsortium: tt_db_file.C /main/3 1995/10/23 10:01:59 rswiston $ 			 				
/*
 * tt_db_file.cc - Implement the TT DB server file class.
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 *
 * NOTE: Caching is not implemented.  Therefore all of the cache levels
 *       are always set to -1 to force a full read from the database.
 */

#include "util/tt_path.h"
#include "db/tt_db_client_consts.h"
#include "db/tt_db_file_utils.h"
#include "db/tt_db_network_path.h"
#include "db/tt_db_object_utils.h"
#include "db/tt_db_access.h"
#include "db/tt_db_property.h"

_Tt_db_file::
_Tt_db_file()
{
}

_Tt_db_file::_Tt_db_file (const _Tt_string &file)
{
  if (!file.len()) {
    dbResults = TT_DB_ERR_ILLEGAL_FILE;
  }
  else {
    _Tt_db_property_list_ptr properties;
    _Tt_db_access_ptr access;
    dbResults = setTtDBFileDefaults(file, properties, access);
  }
}

_Tt_db_file::_Tt_db_file (const _Tt_string               &file,
			  const _Tt_db_property_list_ptr &properties,
			  const _Tt_db_access_ptr        &access)
{
  if (!file.len()) {
    dbResults = TT_DB_ERR_ILLEGAL_FILE;
  }
  else {
    dbResults = setTtDBFileDefaults(file, properties, access);
  }
}

_Tt_db_results
_Tt_db_file::setTtDBFileDefaults (const _Tt_string               &file,
				  const _Tt_db_property_list_ptr &properties,
				  const _Tt_db_access_ptr        &access)
{
  checkedDatabase = FALSE;
  directoryFlag = FALSE;
  dbFileCurrentAccess = new _Tt_db_access;
  dbFileObjects = new _Tt_string_list;
  dbFileObjectsCacheLevel = -1;
  dbFilePropertiesCacheLevel = -1;
  dbFilePtypes = new _Tt_string_list;
  dbResults = TT_DB_OK;

  if (access.is_null()) {
    dbFileAccess = new _Tt_db_access;
  }
  else {
    dbFileAccess = access;
  }

  if (properties.is_null()) {
    dbFileProperties = new _Tt_db_property_list;
  }
  else {
    dbFileProperties = properties;
  }

  if (file.len()) {
    _Tt_string local_path;

    if (_tt_is_network_path(file)) {
      dbFile = _tt_network_path_to_local_path(file);
      dbFileNetworkPath = file;
      (void)dbFileNetworkPath.split(':', dbFileHostname);
    }
    else {
      // Get the absoulte network information for the path
      _Tt_string temp_string;
      dbResults = _tt_db_network_path (file,
			               dbFile,
			               dbFileHostname,
			               dbFilePartition,
			               dbFileNetworkPath);
    }

    if ((dbResults == TT_DB_OK)) {
      if (!isFileInDatabase()) {
	if (dbResults != TT_DB_ERR_DB_OPEN_FAILED) {
	  dbResults = createDBFile();
	}
      }
    }
  }

  return dbResults;
}

_Tt_db_file::~_Tt_db_file ()
{
}

void _Tt_db_file::setCurrentAccess (const _Tt_db_access_ptr &access)
{
  dbFileCurrentAccess = access;
}

_Tt_db_access_ptr _Tt_db_file::getCurrentAccess () const
{
  return dbFileCurrentAccess;
}

_Tt_db_results _Tt_db_file::remove ()
{
  if (dbFileNetworkPath.len()) {
    setCurrentDBAccess ();
    dbResults = dbFileDatabase->removeFile(dbFileNetworkPath);
    
    if (dbResults == TT_DB_OK) {
      dbFile = (char *)NULL;
      dbFileNetworkPath = (char *)NULL;
      dbFileProperties = (_Tt_db_property_list *)NULL;
      dbFileAccess = (_Tt_db_access *)NULL;
    }
  }
  else {
    dbResults = TT_DB_ERR_ILLEGAL_FILE;
  }

  return processDBResults();
}

_Tt_db_results _Tt_db_file::copy (const _Tt_string &new_file)
{
  if (!new_file.len()) {
    return (dbResults = TT_DB_ERR_ILLEGAL_FILE);
  }

  if (isFileInDatabase()) {
    _Tt_string new_local_path;
    _Tt_string new_hostname;
    _Tt_string new_partition;
    _Tt_string new_network_path;
    dbResults = _tt_db_network_path(new_file,
				    new_local_path,
				    new_hostname,
				    new_partition,
				    new_network_path);

    if (dbResults == TT_DB_OK) {
      // If we're not copying to the exact same file
      if (dbFileNetworkPath != new_network_path) {
        setCurrentDBAccess();
        _Tt_db_property_list_ptr properties = getProperties();
        _Tt_db_access_ptr        access = getAccess ();
        _Tt_string_list_ptr      objids = getObjects();

        if (dbResults == TT_DB_OK) {
	  _Tt_db_file_ptr new_db_file = new _Tt_db_file(new_network_path,
						        properties,
						        access);

	  _Tt_string_list_cursor objids_cursor(objids);
	  while (objids_cursor.next()) {
	    _Tt_db_object_ptr object = new _Tt_db_object(*objids_cursor);
	    (void)object->copy(new_file);
	  }
        }
      }
      else {
        dbResults = TT_DB_ERR_SAME_FILE;
      }
    }
  }
  else {
    dbResults = TT_DB_ERR_ILLEGAL_FILE;
  }

  return dbResults;
}

_Tt_db_results _Tt_db_file::move (const _Tt_string &new_file)
{
  if (!new_file.len()) {
    return (dbResults = TT_DB_ERR_ILLEGAL_FILE);
  }

  if (isFileInDatabase()) {
    // Get the hostname and remote path of the new file
    _Tt_string new_local_path;
    _Tt_string new_hostname;
    _Tt_string new_partition;
    _Tt_string new_network_path;
    dbResults = _tt_db_network_path(new_file,
		                    new_local_path,
		                    new_hostname,
		                    new_partition,
		                    new_network_path);

    if (dbResults == TT_DB_OK) {
      // If we're not moving to the exact same file
      if (dbFileNetworkPath != new_network_path) {
        setCurrentDBAccess();
      
        if (dbResults == TT_DB_OK) {
          // If is the new file is on the same machine...
          if (dbFileHostname == new_hostname)  {
	    // We're not allowed to move directories across partitions
	    if (directoryFlag && (dbFilePartition != new_partition)) {
	      dbResults = TT_DB_ERR_ILLEGAL_FILE;
	    }
	    else {
              dbResults = dbFileDatabase->moveFile(dbFileNetworkPath,
	  				           new_network_path);
	
	      if (dbResults == TT_DB_OK) {
	        // Change the official file of this object
	        _Tt_db_property_list_ptr properties;
	        _Tt_db_access_ptr access;
	        dbResults = setTtDBFileDefaults(new_network_path,
						properties,
					        access);
	      }
	    }
          }
	  // Else, the move is between different hosts...
          else {
	    // We're not allowed to move directories across machines
	    if (directoryFlag) {
	      dbResults = TT_DB_ERR_ILLEGAL_FILE;
	    }
	    // Not a directory, move it...
	    else {
	      _Tt_db_property_list_ptr properties = getProperties();
	      _Tt_db_access_ptr        access = getAccess ();
	      _Tt_string_list_ptr      objids = getObjects();
	      
	      if (dbResults == TT_DB_OK) {
		_Tt_db_file_ptr new_db_file = new _Tt_db_file(new_network_path,
							      properties,
							      access);
		
		_Tt_string_list_cursor objids_cursor(objids);
		while (objids_cursor.next()) {
		  _Tt_db_object_ptr object = new _Tt_db_object(*objids_cursor);
		  (void)object->move(new_network_path);
		}
	      }
	    }
	    
	    if (dbResults == TT_DB_OK) {
	      dbResults = dbFileDatabase->removeFile(dbFileNetworkPath);
	    }
	  
	    if (dbResults == TT_DB_OK) {
	      // Change the official file of this object
	      _Tt_db_property_list_ptr properties;
	      _Tt_db_access_ptr access;
	      dbResults = setTtDBFileDefaults(new_network_path,
					      properties,
					      access);
	    }
	  }
	}
      }
      else {
	dbResults = TT_DB_ERR_SAME_FILE;
      }
    }
  }
  else {
    dbResults = TT_DB_ERR_ILLEGAL_FILE;
  }
  
  return dbResults;
}

_Tt_string _Tt_db_file::getHostname ()
{
  return dbFileHostname;
}

_Tt_db_results
_Tt_db_file::setProperty (const _Tt_db_property_ptr &property)
{
  if (isFileInDatabase()) {
    setCurrentDBAccess();
    dbResults = dbFileDatabase->setFileProperty(dbFileNetworkPath,
						property,
						dbFilePropertiesCacheLevel);
  }
  else {
    dbResults = TT_DB_ERR_ILLEGAL_FILE;
  }

  return processDBResults();
}

_Tt_db_results
_Tt_db_file::setProperties (const _Tt_db_property_list_ptr &properties)
{
  if (isFileInDatabase()) {
    setCurrentDBAccess();
    dbResults =
      dbFileDatabase->setFileProperties(dbFileNetworkPath,
					properties,
					dbFilePropertiesCacheLevel);
  }
  else {
    dbResults = TT_DB_ERR_ILLEGAL_FILE;
  }

  return processDBResults();
}

_Tt_db_results
_Tt_db_file::addProperty (const _Tt_db_property_ptr &property,
			  bool_t                     unique)
{
  if (isFileInDatabase()) {
    setCurrentDBAccess();
    dbResults = dbFileDatabase->addFileProperty(dbFileNetworkPath,
						property,
						unique,
						dbFilePropertiesCacheLevel);
  }
  else {
    dbResults = TT_DB_ERR_ILLEGAL_FILE;
  }

  return processDBResults();
}

_Tt_db_results
_Tt_db_file::deleteProperty (const _Tt_db_property_ptr &property)
{
  if (isFileInDatabase()) {
    setCurrentDBAccess();
    dbResults =
      dbFileDatabase->deleteFileProperty(dbFileNetworkPath,
					 property,
					 dbFilePropertiesCacheLevel);
  }
  else {
    dbResults = TT_DB_ERR_ILLEGAL_FILE;
  }

  return processDBResults();
}

_Tt_db_property_ptr _Tt_db_file::getProperty (const _Tt_string &name)
{
  _Tt_db_property_ptr property;

  if (isFileInDatabase()) {
    setCurrentDBAccess();
    dbFilePropertiesCacheLevel = -1;
    dbResults = dbFileDatabase->getFileProperty(dbFileNetworkPath,
						name,
						dbFilePropertiesCacheLevel,
						property);
  }
  else {
    dbResults = TT_DB_ERR_ILLEGAL_FILE;
  }

  dbResults = processDBResults();
  return property;
}

_Tt_db_property_list_ptr _Tt_db_file::getProperties ()
{
  _Tt_db_property_list_ptr properties;

  if (isFileInDatabase()) {
    setCurrentDBAccess();
    dbFilePropertiesCacheLevel = -1;
    dbResults = dbFileDatabase->getFileProperties(dbFileNetworkPath,
						  dbFilePropertiesCacheLevel,
						  properties);
  }
  else {
    dbResults = TT_DB_ERR_ILLEGAL_FILE;
  }

  dbResults = processDBResults();
  return properties;
}

_Tt_string_list_ptr _Tt_db_file::getObjects ()
{
  _Tt_string_list_ptr objids;

  if (isFileInDatabase()) {
    setCurrentDBAccess();
    dbFileObjectsCacheLevel = -1;
    dbResults = dbFileDatabase->getFileObjects(dbFileNetworkPath,
					       dbFileObjectsCacheLevel,
					       objids);
  }
  else {
    dbResults = TT_DB_ERR_ILLEGAL_FILE;
  }

  dbResults = processDBResults();
  return objids;
}

_Tt_db_results _Tt_db_file::setAccess (const _Tt_db_access_ptr &access)
{
  if (isFileInDatabase()) {
    setCurrentDBAccess();
    dbResults = dbFileDatabase->setFileAccess(dbFileNetworkPath, access);
  }
  else {
    dbResults = TT_DB_ERR_ILLEGAL_FILE;
  }

  return processDBResults();
}

_Tt_db_access_ptr _Tt_db_file::getAccess ()
{
  _Tt_db_access_ptr access;

  if (isFileInDatabase()) {
    setCurrentDBAccess();
    dbResults = dbFileDatabase->getFileAccess(dbFileNetworkPath, access);
  }
  else {
    dbResults = TT_DB_ERR_ILLEGAL_FILE;
  }

  dbResults = processDBResults();
  return access;
}

_Tt_db_results _Tt_db_file::addSession (const _Tt_string &session)
{
  _Tt_db_property_ptr property = new _Tt_db_property;
  property->name = TT_DB_SESSION_PROPERTY;

  _Tt_string value = session;
  (void)property->values->append(value);

  return addProperty(property, TRUE);
}

_Tt_db_results _Tt_db_file::deleteSession (const _Tt_string &session)
{
  _Tt_db_property_ptr property = new _Tt_db_property;
  property->name = TT_DB_SESSION_PROPERTY;

  _Tt_string value = session;
  property->values->append(value);

  return deleteProperty(property);
}

_Tt_string_list_ptr _Tt_db_file::getSessions ()
{
  _Tt_db_property_ptr property;
  _Tt_string_list_ptr sessions;
  
  if (isFileInDatabase()) {
    _Tt_string name(TT_DB_SESSION_PROPERTY);
    property = getProperty(name);
    if (dbResults == TT_DB_ERR_NO_SUCH_PROPERTY) {
      dbResults = TT_DB_OK;
    }
    
    if (!property.is_null()) {
      if (!property->is_empty()) {
        sessions = new _Tt_string_list;
      
        _Tt_string_list_cursor values_cursor(property->values);
        while (values_cursor.next()) { 
	  sessions->append(*values_cursor);
        }
      }
    }
  }
  
  return sessions;
}

_Tt_db_results
_Tt_db_file::queueMessage (const _Tt_string_list_ptr &ptypes,
			   const _Tt_message_ptr     &message)
{
  if (isFileInDatabase()) {
    dbResults = dbFileDatabase->queueMessage(dbFileNetworkPath,
					     ptypes,
					     message);
  }
  else {
    dbResults = TT_DB_ERR_ILLEGAL_FILE;
  }

  return dbResults;
}
			     
_Tt_db_results
_Tt_db_file::dequeueMessages (const _Tt_string_list_ptr &ptypes,
			      _Tt_message_list_ptr      &messages)
{
  if (isFileInDatabase()) {
    dbResults = dbFileDatabase->dequeueMessages(dbFileNetworkPath,
						ptypes,
						messages);
  }
  else {
    dbResults = TT_DB_ERR_ILLEGAL_FILE;
  }

  return dbResults;
}

_Tt_string _Tt_db_file::getNetworkPath (const _Tt_string &file)
{
  _Tt_string network_path;

  if (file.len()) {
    if (_tt_is_network_path(file)) {
      network_path = file;
    }
    else {
      _Tt_string local_path;
      _Tt_string hostname;
      _Tt_string partition;

      _tt_db_network_path(file,
					           local_path,
					           hostname,
					           partition,
					           network_path);
    }
  }

  return network_path;
}

_Tt_db_results _Tt_db_file::createDBFile ()
{
  if (!isFileInDatabase()) {
    dbFileDatabase = dbHostnameGlobalMapRef.getDB(dbFileHostname,
						  dbFileHostname,
						  dbResults);
    if (dbFileDatabase.is_null()) {
      return dbResults;
    }

    int cache_level = -1;

    setCurrentDBAccess ();
    dbResults = dbFileDatabase->createFile(dbFileNetworkPath,
					   dbFileProperties,
				  	   dbFileAccess,
					   cache_level);

    dbFileObjectsCacheLevel = cache_level;
    dbFilePropertiesCacheLevel = cache_level;
  }
  else {
    dbResults = TT_DB_ERR_FILE_EXISTS;
  }

  return processDBResults();
}

bool_t _Tt_db_file::isFileInDatabase ()
{
	// Property caching is not currently implemented in _Tt_db_file,
	// so this function does not read the properties from the database
	// if the file exists.  If caching is added, this is where the
	// cache should be initially filled from.

	if (!checkedDatabase) {
		checkedDatabase = TRUE;

		dbFileDatabase = dbHostnameGlobalMapRef.getDB(dbFileHostname,
							      dbFileHostname,
							      dbResults);
		if (dbFileDatabase.is_null() || dbResults != TT_DB_OK) {
			return FALSE;
		}
		_Tt_db_access_ptr access;
		dbResults = dbFileDatabase->isFileInDatabase(dbFileNetworkPath,
							     directoryFlag);

		if (dbResults != TT_DB_OK) {
			dbHostnameGlobalMapRef.removeDB(dbFileHostname);
			dbFileDatabase = (_Tt_db_client *)NULL;
		}
	}

	return (dbFileDatabase.is_null() ? FALSE : TRUE);
}

_Tt_db_results _Tt_db_file::processDBResults ()
{
  // If an RPC fails, assume the DB server went down and attempt to
  // reconnect to it
  if ((dbResults == TT_DB_ERR_DB_CONNECTION_FAILED) ||
      (dbResults == TT_DB_ERR_RPC_CONNECTION_FAILED) ||
      (dbResults == TT_DB_ERR_RPC_FAILED) ||
      (dbResults == TT_DB_ERR_DB_OPEN_FAILED)) {
    if (!dbFileDatabase.is_null()) {
      dbHostnameGlobalMapRef.removeDB(dbFileHostname);
      dbFileDatabase = dbHostnameGlobalMapRef.getDB(dbFileHostname,
						    dbFileHostname,
						    dbResults);

    }
  }

  return dbResults;
}
