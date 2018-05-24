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
/*%%  $XConsortium: tt_db_file.h /main/3 1995/10/23 10:02:07 rswiston $ 			 				 */
/*
 * tt_db_file.h - Declare the TT DB server file class.  This class defines the
 *                file properties interface to the DB server.
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */

#ifndef _TT_DB_FILE_H
#define _TT_DB_FILE_H

#include "util/tt_object.h"
#include "util/tt_string.h"
#include "db/tt_db_hostname_global_map_ref.h"
#include "db/tt_db_access_utils.h"
#include "db/tt_db_property_utils.h"
#if defined(OPT_BUG_HPUX) || defined(OPT_BUG_AIX)
#	undef copy
#endif

class _Tt_db_file : public _Tt_object {
public:
  _Tt_db_file ();

  // If the file already exists in the DB on its partition, then
  // the DB is connected to when it needs to be.  If the file
  // does not exist on the database, the file entry is created
  // on the database.
  _Tt_db_file (const _Tt_string &file);
  _Tt_db_file (const _Tt_string               &file,
	       const _Tt_db_property_list_ptr &properties,
	       const _Tt_db_access_ptr        &access);
  ~_Tt_db_file ();

  // Sets and gets the access mode of this file.
  void              setCurrentAccess (const _Tt_db_access_ptr &access);
  _Tt_db_access_ptr getCurrentAccess () const;

  // Removes the file and all of its objects from the database.  Once this is
  // called this particular object instantiation is useless until "setFile" has
  // been called.
  _Tt_db_results remove ();

  // Copies the file to the new file name
  _Tt_db_results copy (const _Tt_string &new_file);

  // Moves the file to the new file name
  _Tt_db_results move (const _Tt_string &new_file);

  // Gets the local path of the file.  Regardless of what path was specified
  // in the object constructor, getLocalPath always returns the real path
  // (the result of _tt_realpath).
  const _Tt_string& getLocalPath () const
    {
      return dbFile;
    }

  // Gets the network absolute path of the file.  The path is in the
  // form:
  //
  //               hostname:/path
  //
  const _Tt_string& getNetworkPath () const
    {
      return dbFileNetworkPath;
    }

  // Gets the network absolute path of the file argument.  The path
  // is in the form:
  //
  //               hostname:/path
  //
  static _Tt_string getNetworkPath (const _Tt_string &file);

  // Returns the hostname of the machine the file is located on.
  _Tt_string getHostname ();

  // Writes the specified file property to the database.  All values of the
  // the file property with the same name are overwritten.  If the value of
  // the property is NULL, this deletes all properties with the same name
  // from the database.
  _Tt_db_results setProperty (const _Tt_db_property_ptr &property);

  // Writes the specified file properties to the database.  All values of the
  // the file properties with the same names are overwritten.  If the value of
  // some of the properties are NULL, this deletes all properties with the same
  // name from the database.
  _Tt_db_results setProperties (const _Tt_db_property_list_ptr &properties);

  // Adds a file property to the list of values of the file property
  // with the same name.  If the property does not already exist, then
  // the property is created.  If the "unique" argument is TRUE, then
  // the property name and value combination is guaranteed to be unique
  // and will not be added to the list of values if the combination already
  // exists.
  _Tt_db_results addProperty (const _Tt_db_property_ptr &property,
                              bool_t                     unique=FALSE);

  // Deletes the file property value with the same name and value.  If
  // the value is left empty, then all values of the property are deleted.
  _Tt_db_results deleteProperty (const _Tt_db_property_ptr &property);

  // Gets the values of the file property with the specified name.
  _Tt_db_property_ptr      getProperty (const _Tt_string &name);

  // Gets a list of all of the file properties.
  _Tt_db_property_list_ptr getProperties ();

  // Returns a list of all of the object IDs for the specified file.
  _Tt_string_list_ptr getObjects ();

  // Sets and gets the access mode of the specified file.
  _Tt_db_results    setAccess (const _Tt_db_access_ptr &access);
  _Tt_db_access_ptr getAccess ();

  // Add a session to the file
  _Tt_db_results addSession (const _Tt_string &session);

  // Delete a session from the file
  _Tt_db_results deleteSession (const _Tt_string &session);

  // Get a list of sessions that have joined this file
  _Tt_string_list_ptr getSessions ();  

  // For this file, queues the message and the list of ptypes
  // that the message is addressed to.
  _Tt_db_results queueMessage (const _Tt_string_list_ptr &ptypes,
			       const _Tt_message_ptr     &message);

  // For this file, returns a list of messages that are addressed
  // the specified ptypes.  If there are no other ptypes that this message
  // is addressed to, the message is deleted from the database.
  _Tt_db_results dequeueMessages (const _Tt_string_list_ptr &ptypes,
				  _Tt_message_list_ptr      &messages);

  // Gets the results of the last database operation made with this object. 
  _Tt_db_results getDBResults () const
    {
      return dbResults;
    }

private:
  bool_t                         checkedDatabase;
  bool_t                         directoryFlag;
  _Tt_string                     dbFile;
  _Tt_db_access_ptr              dbFileAccess;
  _Tt_db_access_ptr              dbFileCurrentAccess;
  _Tt_db_client_ptr              dbFileDatabase;
  _Tt_string                     dbFileHostname;
  _Tt_string                     dbFileNetworkPath;
  _Tt_string_list_ptr            dbFileObjects;
  int                            dbFileObjectsCacheLevel;
  _Tt_string                     dbFilePartition;
  _Tt_db_property_list_ptr       dbFileProperties;
  int                            dbFilePropertiesCacheLevel;
  _Tt_string_list_ptr            dbFilePtypes;
  _Tt_db_hostname_global_map_ref dbHostnameGlobalMapRef;
  _Tt_db_results	         dbResults;

  // Sets the file in the database that the object is operating on.  
  // If the file already exists in the DB on its partition, then
  // the DB is connected to when it needs to be.  If the file
  // does not exist on the database, the file entry is created
  // on the database.
  _Tt_db_results setTtDBFileDefaults (const _Tt_string&,
				      const _Tt_db_property_list_ptr&,
				      const _Tt_db_access_ptr&);

  _Tt_db_results createDBFile ();
  bool_t         isFileInDatabase ();

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
      if (!dbFileDatabase.is_null()) {
	dbFileDatabase->setCurrentAccess(dbFileCurrentAccess);
      }
    }
};

#endif // _TT_DB_FILE_H
