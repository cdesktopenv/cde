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
//%%  $XConsortium: tt_db_object.C /main/3 1995/10/23 10:03:44 rswiston $ 			 				
/*
 * tt_db_object.cc - Implement the TT DB server object class.
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 *
 */

#include "db/tt_db_create_objid.h"
#include "db/tt_db_key_utils.h"
#include "db/tt_db_network_path.h"
#include "db/tt_db_object_utils.h"
#include "db/tt_db_objid_to_key.h"
#include "db/tt_db_access.h"
#include "db/tt_db_property.h"

#define TT_DB_PROPS_CACHE_LEVEL_PROPERTY "_MODIFICATION_DATE"

_Tt_db_object::_Tt_db_object ()
{
  setTtDBObjectDefaults();
}

_Tt_db_object::_Tt_db_object (const _Tt_string &objid)
{
  setTtDBObjectDefaults();
  dbObjectID = objid;

  getDBObjectHostnameFromID ();
  if (isObjectInDatabase()) {
    dbObjectPropertiesCacheLevel = -1;
 
    setCurrentDBAccess ();
    dbResults =
      dbObjectDatabase->getObjectProperties(dbObjectID,
					    dbObjectPropertiesCacheLevel,
                                            dbObjectProperties);

    memoryObjectCreated = TRUE;

    if ((dbResults == TT_DB_ERR_DB_CONNECTION_FAILED) ||
	(dbResults == TT_DB_ERR_RPC_CONNECTION_FAILED)) {
      dbHostnameGlobalMapRef.removeDB (dbObjectHostname);
    }
  }
  else {
    dbResults = TT_DB_ERR_NO_SUCH_OBJECT;
  }
}

void _Tt_db_object::setTtDBObjectDefaults ()
{
  checkedDatabase = FALSE;
  dbObjectAccess = new _Tt_db_access;
  dbObjectCurrentAccess = new _Tt_db_access;
  dbObjectProperties = new _Tt_db_property_list;
  dbObjectPropertiesCacheLevel = -1;
  dbResults = TT_DB_OK;
  forwardPointerFlag = FALSE;
  memoryObjectCreated = FALSE;
}

_Tt_db_object::~_Tt_db_object ()
{
}

void _Tt_db_object::setCurrentAccess (const _Tt_db_access_ptr &access)
{
  dbObjectCurrentAccess = access;
}

_Tt_db_access_ptr _Tt_db_object::getCurrentAccess () const
{
  return dbObjectCurrentAccess;
}

_Tt_string _Tt_db_object::create (const _Tt_string &file)
{
	_Tt_string empty;
	return create(file, empty);
}

_Tt_string _Tt_db_object::create (const _Tt_string &file,
				  const _Tt_string &key)
{
  if (!memoryObjectCreated) {
    if (!file.len()) {
      dbResults = TT_DB_ERR_ILLEGAL_FILE;
    }
    else {
      dbResults = createMemoryObject(file, key);

      if (dbResults == TT_DB_OK) {
        memoryObjectCreated = TRUE;
      }
    }
  }
  else {
    dbResults = TT_DB_ERR_OBJECT_EXISTS;
  }

  return dbObjectID;
}

_Tt_db_results _Tt_db_object::write ()
{
  if (!isObjectInDatabase()) {
    dbResults = createDBObject();
  }
  else {
    _Tt_db_property_list_ptr properties_in_db;

    // Find the cache level property and make sure it is up to date
    // with the actual cache level
    // XXX - This needs to be done with 4/93 DB servers.  10/93 DB servers
    // will protect the cache level property on the DB server side.
    _Tt_db_property_list_cursor properties_cursor (dbObjectProperties);
    _Tt_string cache_level_bytes (sizeof (int));

    while (properties_cursor.next ()) {
      if (properties_cursor->name ==  TT_DB_PROPS_CACHE_LEVEL_PROPERTY) {
	memcpy ((char *)cache_level_bytes,
		&dbObjectPropertiesCacheLevel,
		sizeof (int));
	properties_cursor->values->flush ();
	properties_cursor->values->append (cache_level_bytes);
      }
    }

    setCurrentDBAccess ();
    dbResults =
      dbObjectDatabase->setObjectProperties(dbObjectID,
					    dbObjectProperties,
					    properties_in_db,
					    dbObjectPropertiesCacheLevel);

    // If there was an update conflict, that means someone else wrote
    // new props to the DB since this object had read its cached props.
    // The _Tt_db_client object returns the properties currently in the
    // database when this type of conflict occurs.
    if (dbResults == TT_DB_ERR_UPDATE_CONFLICT) {
      dbObjectProperties = properties_in_db;
    }
  }

  return processDBResults();
}

_Tt_db_results _Tt_db_object::refresh ()
{
  // Check if it is in the database and force it to find the object
  // if it has been moved.
  if (isObjectInDatabase(TRUE)) {
    int			     cache_level = dbObjectPropertiesCacheLevel;
    _Tt_db_property_list_ptr properties;

    setCurrentDBAccess ();
    dbResults = dbObjectDatabase->getObjectProperties(dbObjectID,
						      cache_level,
						      properties);

    // Update the cache if the _Tt_db_client has new property data
    if (cache_level > dbObjectPropertiesCacheLevel) {
      dbObjectPropertiesCacheLevel = cache_level;
      dbObjectProperties = properties;
    }
  }
  else {
    dbResults = TT_DB_OK;
  }

  return processDBResults();
}

_Tt_db_results _Tt_db_object::remove ()
{
  if (isObjectInDatabase()) {
    setCurrentDBAccess ();
    dbResults = dbObjectDatabase->removeObject(dbObjectID,
					       _Tt_string((char *)NULL));
    
    if (dbResults == TT_DB_OK) {
      setTtDBObjectDefaults();
    }
  }
  else {
    dbResults = TT_DB_ERR_NO_SUCH_OBJECT;
  }

  return processDBResults();
}

_Tt_db_results _Tt_db_object::remove (const _Tt_string &forward_pointer)
{
  if (isObjectInDatabase()) {
    setCurrentDBAccess ();
    dbResults = dbObjectDatabase->removeObject(dbObjectID, forward_pointer);
    
    if (dbResults == TT_DB_OK) {
      setTtDBObjectDefaults();
    }
  }
  else {
    dbResults = TT_DB_ERR_NO_SUCH_OBJECT;
  }

  return processDBResults();
}

_Tt_string _Tt_db_object::copy (const _Tt_string &new_file)
{
  _Tt_string new_objid;

  if (!new_file.len()) {
    dbResults = TT_DB_ERR_ILLEGAL_FILE;
    return new_objid;
  }

  if (isObjectInDatabase()) {
    dbObjectFile = getFile();

    _Tt_string new_local_file;
    _Tt_string new_hostname;
    _Tt_string new_partition;
    _Tt_string new_file_network_path;
    dbResults = _tt_db_network_path(new_file,
	                            new_local_file,
	                            new_hostname,
		                    new_partition,
	                            new_file_network_path);

    if (dbResults == TT_DB_OK) {
      // If we're not copying to the exact same file
      if (dbObjectFile != new_file_network_path) {
        _Tt_db_object_ptr new_object = new _Tt_db_object;
        new_objid = new_object->create(new_file_network_path);
        dbResults = new_object->getDBResults();

        if ((dbResults == TT_DB_OK) && new_objid.len()) {
	  // Refresh the current object's info
	  dbResults = internalRefresh();
	  if (dbResults == TT_DB_OK) {
            (void)new_object->setAccess(dbObjectAccess);
            (void)new_object->setProperties(dbObjectProperties);
            (void)new_object->setType(dbObjectType);
            dbResults = new_object->write();
	  }
        }
      }
      else {
        dbResults = TT_DB_ERR_SAME_OBJECT;
      }
    }
  }
  else {
    dbResults = TT_DB_ERR_NO_SUCH_OBJECT;
  }

  dbResults = processDBResults();
  return new_objid;
}

_Tt_string _Tt_db_object::move (const _Tt_string &new_file)
{
  _Tt_string new_objid;

  if (!new_file.len()) {
    dbResults = TT_DB_ERR_ILLEGAL_FILE;
    return new_objid;
  }

  if (isObjectInDatabase()) {
    dbObjectFile = getFile();

    _Tt_string new_local_file;
    _Tt_string new_hostname;
    _Tt_string new_partition;
    _Tt_string new_file_network_path;
    dbResults = _tt_db_network_path(new_file,
	                            new_local_file,
	                            new_hostname,
		                    new_partition,
	                            new_file_network_path);

    // If we're not moving to the exact same file
    if (dbObjectFile != new_file_network_path) {
      new_objid = makeEquivalentObjectID(new_hostname, new_partition);

      if ((dbResults == TT_DB_OK) && new_objid.len()) {
	// If the objids are still the same, we only need to change the
	// object's file in the database...
	if (dbObjectID == new_objid) {
	  dbResults = dbObjectDatabase->setObjectFile(dbObjectID,
						      new_file_network_path);

	  if (dbResults == TT_DB_OK) {
	    dbResults = TT_DB_WRN_SAME_OBJECT_ID;
	  }
	}
	// Different objids and therefore different partitions or hosts
	else {
	  _Tt_db_object_ptr new_object = new _Tt_db_object;
	  dbResults = new_object->setObjectID(new_objid);

	  // The new object should not exist, yet...
	  if (dbResults == TT_DB_ERR_NO_SUCH_OBJECT) {
	    new_object->dbObjectHostname = new_hostname;
	    new_object->dbObjectFile = new_file_network_path;

	    // Refresh the current object's info
	    dbResults = internalRefresh();
	    if (dbResults == TT_DB_OK) {
	      (void)new_object->setAccess(dbObjectAccess);
	      (void)new_object->setProperties(dbObjectProperties);
	      (void)new_object->setType(dbObjectType);
	      dbResults = new_object->write();
	    }
	  }
	  // If the new object already exists, then there is something
	  // weird going on...
	  else if (dbResults == TT_DB_OK) {
	    dbResults = TT_DB_ERR_CORRUPT_DB;
	  }

	  if (dbResults == TT_DB_OK) {
	    // Remove current object from the DB and specify the new objid
	    // as the forward pointer.
	    dbResults = remove(new_objid);

	    if (dbResults == TT_DB_OK) {
	      setTtDBObjectDefaults();
	      dbResults = setObjectID(new_objid);
	    }
	  }
	}
      }
      else {
	if (dbResults == TT_DB_OK) {
	  dbResults = TT_DB_ERR_ILLEGAL_OBJECT;
	}
      }
    }
    else {
      dbResults = TT_DB_ERR_SAME_OBJECT;
    }
  }
  else {
    dbResults = TT_DB_ERR_NO_SUCH_OBJECT;
  }

  dbResults = processDBResults();
  return new_objid;
}

_Tt_db_results
_Tt_db_object::setProperty (const _Tt_db_property_ptr &property)
{
  if (property->name.len()) {
    // See if a property with the same name exists...
    bool_t found = FALSE;
    _Tt_db_property_list_cursor properties_cursor(dbObjectProperties);
    while (properties_cursor.next()) {
      if (properties_cursor->name == property->name) {
        found = TRUE;
        break;
      }
    } 
    
    // If a property with the same name exists...
    if (found) {
      // If the new property has no values specified, then just
      // delete the old property...
      if (property->is_empty()) {
        properties_cursor.remove();
      }
      // Else, we actually want to set some values...
      else {
        *properties_cursor = property;
      }
    }
    // Else, no property with same name exists, just append the new
    // property (if it's not empty)
    else { if (!property->is_empty())
               dbObjectProperties->append(property);
    }

    dbResults = TT_DB_OK;
  }
  else {
    dbResults = TT_DB_ERR_ILLEGAL_PROPERTY;
  }

  return dbResults;
}

_Tt_db_results
 _Tt_db_object::setProperties (const _Tt_db_property_list_ptr &properties)
{
  // Replace the old properties with the new list
  dbObjectProperties = properties;

  return dbResults;
}

_Tt_db_results
_Tt_db_object::addProperty (const _Tt_db_property_ptr &property,
			    bool_t                     unique)
{
  if (property->name.len()) {
    bool_t found = FALSE; 

    // See if a property with the same name exists in the cache
    _Tt_db_property_list_cursor properties_cursor(dbObjectProperties);
    while (properties_cursor.next()) { 
      if (properties_cursor->name == property->name) { 
        found = TRUE;
        break; 
      }   
    } 
     
    // If a property with the same name exists...
    if (found) {
      _Tt_string_list_cursor values_cursor(property->values);

      // If a unique property name-value combinations are to be added...
      if (unique) {
	_Tt_string_list_cursor cache_values_cursor(properties_cursor->values);

	// Loop through the values to be added
	while (values_cursor.next()) {
	  cache_values_cursor.reset();
	
	  // Compare versus values already in the cache
	  bool_t is_unique = TRUE;
	  while (cache_values_cursor.next()) {
	    if (*cache_values_cursor == *values_cursor) {
	      is_unique = FALSE;
	    }
	  }
      
	  // If unique, append the value to the cache values
	  if (is_unique) {
	    properties_cursor->values->append(*values_cursor);
	  }
	}
      }
      // Else uniqueness is not a requirement...
      else {
	while (values_cursor.next()) {
	  properties_cursor->values->append(*values_cursor);
	}
      }
      
      dbResults = TT_DB_OK;
    }
    else {
      dbObjectProperties->append(property);
      dbResults = TT_DB_OK;
    }
  }
  else {
    dbResults = TT_DB_ERR_ILLEGAL_PROPERTY;
  }

  return dbResults;
}

_Tt_db_results
_Tt_db_object::deleteProperty (const _Tt_db_property_ptr &property)
{
  if (isObjectInDatabase()) {
    setCurrentDBAccess();

    // Delete the property on the database immediately and refresh
    // the cache with the latest and greatest.  If the cache level
    // here is not as high as the cache level in the DB, then the
    // delete will fail.
    dbResults =
      dbObjectDatabase->deleteObjectProperty(dbObjectID,
					     property,
					     dbObjectProperties,
					     dbObjectPropertiesCacheLevel);
  }
  else {
    dbResults = TT_DB_ERR_NO_SUCH_OBJECT;
  }

  return processDBResults();
}

_Tt_db_property_ptr _Tt_db_object::getProperty (const _Tt_string &name)
{
  _Tt_db_property_ptr property;
  
  if (name.len()) {
    bool_t found = FALSE;  
    _Tt_db_property_list_cursor properties_cursor(dbObjectProperties); 

    // See if a property matches the specified name
    while (properties_cursor.next()) { 
      if (properties_cursor->name == name) {  
        found = TRUE; 
        break;  
      }   
    }  
      
    if (found) {
      property = *properties_cursor;
      dbResults = TT_DB_OK;
    }
    else {
      dbResults = TT_DB_ERR_NO_SUCH_PROPERTY;
    }
  }
  else {
    dbResults = TT_DB_ERR_ILLEGAL_PROPERTY;
  }

  return property;
}

_Tt_db_property_list_ptr _Tt_db_object::getProperties ()
{
  dbResults = TT_DB_OK;
  return dbObjectProperties;
}

_Tt_db_results _Tt_db_object::setType (const _Tt_string &type)
{
  if (!isObjectInDatabase()) {
    dbObjectType = type;
    dbResults = TT_DB_OK;
  }
  else {
    dbResults = TT_DB_ERR_OTYPE_ALREADY_SET;
  }

  return dbResults;
}

_Tt_string _Tt_db_object::getType ()
{
  if (!dbObjectType.len() && isObjectInDatabase()) {
    setCurrentDBAccess();
    dbResults = dbObjectDatabase->getObjectType(dbObjectID, dbObjectType);
  }
  else {
    dbResults = TT_DB_OK;
  }

  dbResults = processDBResults();
  return dbObjectType;
}

_Tt_string _Tt_db_object::getFile ()
{
  if (isObjectInDatabase()) {
    setCurrentDBAccess();
    dbResults = dbObjectDatabase->getObjectFile(dbObjectID, dbObjectFile);
  }
  else {
    dbResults = TT_DB_OK;
  }

  dbResults = processDBResults();
  return dbObjectFile;
}

_Tt_db_results _Tt_db_object::setAccess (const _Tt_db_access_ptr &access)
{
  dbObjectAccess = access;

  if (isObjectInDatabase()) {
    setCurrentDBAccess();
    dbResults = dbObjectDatabase->setObjectAccess(dbObjectID, dbObjectAccess);
  }
  else {
    dbResults = TT_DB_OK;
  }

  return processDBResults();
}

_Tt_db_access_ptr _Tt_db_object::getAccess ()
{
  if (isObjectInDatabase()) {
    setCurrentDBAccess();
    dbResults = dbObjectDatabase->getObjectAccess(dbObjectID, dbObjectAccess);
  }
  else {
    dbResults = TT_DB_OK;
  }

  dbResults = processDBResults();
  return dbObjectAccess;
}

_Tt_string _Tt_db_object::getObjectKey () const
{
  _Tt_string key_string;

  if (dbObjectID.len()) {
    key_string = _tt_db_objid_to_key(dbObjectID);
  }

  return key_string;
}

_Tt_db_results
_Tt_db_object::createMemoryObject (const _Tt_string &file,
				   const _Tt_string& keystr)
{
  _Tt_string local_file;
  _Tt_string partition;

  dbObjectID = (char *)NULL;
  dbResults = _tt_db_network_path(file,
			          local_file,
			          dbObjectHostname,
			          partition,
			          dbObjectFile);

  if (dbResults == TT_DB_OK) {
	  // Create a key for the new object,
	  // or use the passed-in one if present
	  _Tt_db_key_ptr key;
	  if (keystr.len()) {
		  key = new _Tt_db_key(keystr);
	  }
	  else {
		  key = new _Tt_db_key();
	  }

	  dbObjectID = _tt_db_create_objid(key, "NFS",
					   dbObjectHostname, partition);
  }

  return dbResults;
}

void _Tt_db_object::getDBObjectHostnameFromID ()
{
  if (dbObjectID.len()) {
    _Tt_string objid_string = dbObjectID;
    _Tt_string temp_string;

    // Get rid of the file system type
    objid_string = objid_string.split(':', temp_string);
    if (objid_string.len() && temp_string.len()) {
      objid_string = objid_string.split(':', temp_string);
    }

    // Get the hostname
    if (objid_string.len() && temp_string.len()) {
      objid_string = objid_string.split(':', dbObjectHostname);
    }
  }
}

_Tt_db_results _Tt_db_object::createDBObject ()
{
  if (dbObjectID.len()) {
    if (dbObjectType.len()) {
      dbObjectDatabase = dbHostnameGlobalMapRef.getDB(dbObjectHostname,
						      dbObjectHostname,
						      dbResults);

      if (!dbObjectDatabase.is_null()) {
        setCurrentDBAccess();
        dbResults =
	  dbObjectDatabase->createObject (dbObjectFile,
					  dbObjectID,
					  dbObjectType,
					  dbObjectProperties,
					  dbObjectAccess,
					  dbObjectPropertiesCacheLevel);

	  dbObjectPropertiesCacheLevel = -1;

	if (dbResults == TT_DB_OK) {
	  dbResults = internalRefresh ();
	}
      }
    }
    else {
      dbResults = TT_DB_ERR_NO_OTYPE;
    }
  }
  else {
    dbResults = TT_DB_ERR_ILLEGAL_OBJECT;
  }

  return dbResults;
}

_Tt_db_results _Tt_db_object::setObjectID (const _Tt_string &new_objid)
{
  dbObjectID = new_objid;
  getDBObjectHostnameFromID();

  dbResults = (isObjectInDatabase(TRUE) ? TT_DB_OK :
					  TT_DB_ERR_NO_SUCH_OBJECT);
  return dbResults;
}

_Tt_string
_Tt_db_object::makeEquivalentObjectID (const _Tt_string &hostname,
				       const _Tt_string &partition)
{
  _Tt_string new_objid = _tt_db_objid_to_key(dbObjectID);
  new_objid = new_objid.cat(":NFS");
  new_objid = new_objid.cat(":").cat(hostname);
  new_objid = new_objid.cat(":").cat(partition);
  return new_objid;
}

bool_t _Tt_db_object::isObjectInDatabase (bool_t force)
{
  _Tt_string forward_pointer;

  if ((!checkedDatabase || force) && dbObjectHostname.len()) {
    // Get the DB connection for this objects DB host
    dbObjectDatabase = dbHostnameGlobalMapRef.getDB(dbObjectHostname,
						    dbObjectHostname,
						    dbResults);
    if (!dbObjectDatabase.is_null()) {
      setCurrentDBAccess();

      // See if the object is in the actual database and also check for
      // a forward pointer
      _Tt_string forward_pointer;
      dbResults = dbObjectDatabase->isObjectInDatabase(dbObjectID,
						       forward_pointer);
    }

    // Either a failure or a forward pointer...
    if (dbResults != TT_DB_OK) {
      if (dbResults == TT_DB_ERR_NO_SUCH_OBJECT) {
        dbHostnameGlobalMapRef.removeDB(dbObjectHostname);
        dbObjectDatabase = (_Tt_db_client *)NULL;
	dbResults = TT_DB_OK;
      }
      else if (dbResults == TT_DB_WRN_FORWARD_POINTER) {
	// Change this objects ID to the forward pointer.  The "setObjectID"
	// member function will call "isObjectInDatabase" again and the
	// forward pointer chain will eventually resolve to the final
	// resting place of the object.
	dbResults = setObjectID(forward_pointer);
	forwardPointerFlag = TRUE;
      }
    }

    checkedDatabase = TRUE;
  }

  return (dbObjectDatabase.is_null() ? FALSE : TRUE);
}

_Tt_db_results _Tt_db_object::internalRefresh ()
{
  setCurrentDBAccess ();
  dbResults =
    dbObjectDatabase->getObjectProperties(dbObjectID,
					  dbObjectPropertiesCacheLevel,
                                          dbObjectProperties);

  if (dbResults == TT_DB_OK) {
    dbResults = dbObjectDatabase->getObjectAccess(dbObjectID, dbObjectAccess);
  }

  if (dbResults == TT_DB_OK) {
    dbResults = dbObjectDatabase->getObjectType(dbObjectID, dbObjectType);
  }

  return dbResults;
}

_Tt_db_results _Tt_db_object::processDBResults ()
{
  // If an RPC fails, assume the DB server went down and attemp to
  // reconnect to it
  if ((dbResults == TT_DB_ERR_DB_CONNECTION_FAILED) ||
      (dbResults == TT_DB_ERR_RPC_CONNECTION_FAILED)) {
    if (!dbObjectDatabase.is_null()) {
      dbHostnameGlobalMapRef.removeDB(dbObjectHostname);
      dbObjectDatabase = dbHostnameGlobalMapRef.getDB(dbObjectHostname,
						      dbObjectHostname,
						      dbResults);

    }
  }   
  // Else if the results were OK, but the forward pointer flag was set,
  // return TT_WRN_FORWARD_POINTER
  else if ((dbResults == TT_DB_OK) && forwardPointerFlag) {
    forwardPointerFlag = FALSE;
    dbResults = TT_DB_WRN_FORWARD_POINTER;
  }

  return dbResults;
} 
