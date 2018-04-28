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
/*%%  $XConsortium: tt_db_object.h /main/3 1995/10/23 10:03:53 rswiston $ 			 				 */
/*
 * tt_db_object.h - Declare the TT DB server object class.  This class defines the
 *                  object properties interface to the DB server.
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */

#ifndef _TT_DB_OBJECT_H
#define _TT_DB_OBJECT_H

#include "util/tt_object.h"
#include "util/tt_string.h"
#include "db/tt_db_file.h"
#include "db/tt_db_hostname_global_map_ref.h"
#include "db/tt_db_access_utils.h"
#include "db/tt_db_property_utils.h"

class _Tt_db_object : public _Tt_object {
public:
  // Instantiates a _Tt_object without any connection to an existing
  // spec or the database.
  _Tt_db_object ();

  // If the specified ID is an existing spec in the database, then
  // the database is connected to and the spec is loaded into memory.
  _Tt_db_object (const _Tt_string &objid);

  ~_Tt_db_object ();

  // Sets and gets the access mode of this object spec.
  void              setCurrentAccess (const _Tt_db_access_ptr &access);
  _Tt_db_access_ptr getCurrentAccess () const;

  // Creates the in memory version of the object spec and returns the
  // new object ID.
  _Tt_string create (const _Tt_string &file);
  _Tt_string create (const _Tt_string &file, const _Tt_string &key);

  // Writes the spec to the database.
  _Tt_db_results write ();

  // Refreshes the properties in memory with whatever is currently out
  // in the database.
  _Tt_db_results refresh ();

  // Removes the spec from the database and/or memory and leaves the specified
  // forward pointer in its place.
  _Tt_db_results remove ();
  _Tt_db_results remove (const _Tt_string &forward_pointer);

  // Copys the object spec to the new file
  _Tt_string copy (const _Tt_string &new_file);

  // Moves the object spec to the new file.  After the move is done,
  // this object actually contains the information of the new location.
  // However, to prevent any weird race conditions due to someone else
  // doing a move shortly after this move, it is best to just instantiate
  // a new _Tt_db_object with the new objid.
  _Tt_string move (const _Tt_string &new_file);

  // Writes the specified object property to memory.  All values of the
  // the object property with the same name are overwritten.  If the value of
  // the property is empty, this deletes all properties with the same name
  // from the database.
  _Tt_db_results setProperty (const _Tt_db_property_ptr &property);

  // Writes the specified object properties to memory.  All values of
  // the object properties with the same names are overwritten.  If the value
  // of some of the properties are empty, this deletes all properties with the
  // same name from the database.
  _Tt_db_results setProperties (const _Tt_db_property_list_ptr &properties);

  // Adds an object property to the list of values of the object property
  // with the same name in memory.  If the property does not already exist,
  // then the property is created.  If the "unique" argument is TRUE, then
  // the property name and value combination is guaranteed to be unique
  // and will not be added to the list of values if the combination already
  // exists.
  _Tt_db_results addProperty (const _Tt_db_property_ptr &property,
		              bool_t	                 unique=FALSE);
  
  // Deletes the object property value with the same name and value from the
  // DB.  If the value is left empty, then all values of the property are
  // deleted.  The deletion is not cached.  The deletion is immediately
  // executed on disk.
  _Tt_db_results deleteProperty (const _Tt_db_property_ptr &property); 
 
  // Gets the values of the object property with the specified name from the
  // memory cache.  If the property does not exist in memory, then the value
  // is obtained from the DB.
  _Tt_db_property_ptr      getProperty (const _Tt_string &name);

  // Gets a list of all of the object properties currently written to the DB.
  // If the object has never been written to the DB, then an error is returned.	
  _Tt_db_property_list_ptr getProperties ();

  // Sets and gets the otype of the specified object.
  _Tt_db_results setType (const _Tt_string &otype);
  _Tt_string     getType ();

  // Gets the file of the object.
  _Tt_string getFile ();

  // Sets and gets the access mode of the specified object.
  _Tt_db_results    setAccess (const _Tt_db_access_ptr &access);
  _Tt_db_access_ptr getAccess ();
 
  // Returns the object ID of this object.
  const _Tt_string& getObjectID () const
    {
      return dbObjectID;
    }

  // Returns the key porition of this object's object ID.
  _Tt_string getObjectKey () const;

  // Gets the results of the last database operation made with this object. 
  _Tt_db_results getDBResults () const
    {
      return dbResults;
    }

private:
  bool_t                         checkedDatabase;
  _Tt_db_hostname_global_map_ref dbHostnameGlobalMapRef;
  _Tt_db_access_ptr              dbObjectAccess;
  _Tt_db_access_ptr              dbObjectCurrentAccess;
  _Tt_db_client_ptr              dbObjectDatabase;
  _Tt_string                     dbObjectFile;
  _Tt_string                     dbObjectHostname;
  _Tt_string                     dbObjectID;
  _Tt_db_property_list_ptr       dbObjectProperties;
  int                            dbObjectPropertiesCacheLevel;
  _Tt_string                     dbObjectType;
  _Tt_db_results	         dbResults;
  bool_t                         forwardPointerFlag;
  bool_t			 memoryObjectCreated;

  void           setTtDBObjectDefaults ();
  _Tt_db_results createMemoryObject (const _Tt_string&, const _Tt_string&);
  void           getDBObjectHostnameFromID ();
  _Tt_db_results createDBObject ();

  //
  // Sets the internal object ID to a new ID and checks to see if the
  // object is in the database.
  //
  _Tt_db_results setObjectID (const _Tt_string&);

  _Tt_string     makeEquivalentObjectID (const _Tt_string&,
                                         const _Tt_string&);
  bool_t         isObjectInDatabase (bool_t force = FALSE);
  _Tt_db_results internalRefresh ();

  //
  // Does some special results processing for any public member functions
  // that do some database server operations.  It should be used to get
  // the final results for all such member functions.  Member functions
  // that are not public or that only work on the memory copy of the
  // object should not call processDBResults.
  //
  _Tt_db_results processDBResults (); 

  void setCurrentDBAccess ()
    {
      if (!dbObjectDatabase.is_null()) {
	dbObjectDatabase->setCurrentAccess(dbObjectCurrentAccess);
      }
    }
};

#endif // _TT_DB_OBJECT_H
