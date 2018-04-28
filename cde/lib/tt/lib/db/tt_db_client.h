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
/*%%  $XConsortium: tt_db_client.h /main/3 1995/10/23 10:01:13 rswiston $ 			 				 */
/*
 * @(#)tt_db_client.h	1.19 95/02/21
 *
 * Declare the TT DB client class.  This class defines a
 * client interface to the DB server.
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */

#ifndef _TT_DB_CLIENT_H
#define _TT_DB_CLIENT_H

// db_server.h requires this definition by the client side code
#define _TT_DBCLIENT_SIDE

#include "mp/mp_message.h"
#include "util/tt_object.h"
#include "util/tt_string.h"
#include "db/tt_db_access_utils.h"
#include "db/tt_db_property_utils.h"
#include "db/tt_db_property.h"
#include "db/tt_db_results.h"

// ********** Old DB Compatibility Include Files **********
#include "db/tt_old_db_results.h"
#include "db/tt_client_isam.h"
// ********** Old DB Compatibility Include Files **********

class _Tt_db_client : public _Tt_object {
public:
  // Connects to rpc.ttdbserverd on the specified host.  If no hostname
  // is provided, then the connection is made to the host the process
  // is running on.
  _Tt_db_client(_Tt_db_results	& status);
  _Tt_db_client ();	// <- Yuck, it does work, we need a status.
  _Tt_db_client (const _Tt_string &hostname, _Tt_db_results & status);
  ~_Tt_db_client ();

  // Returns the results of the DB connection attempted when this object
  // is instantiated.
  _Tt_db_results getConnectionResults () const
    {
      return dbConnectionResults;
    }

  // returns the hostname of the machine this DB object is connected to.
  _Tt_string getHostname () const
    {
      _Tt_string hostname = (char *)dbHostname;
      return hostname;
    }

  // Sets and gets the current access mode to be used when reading, writing
  // and removing data to and from the database.  If not used, the default
  // access is world read and write.
  void              setCurrentAccess (const _Tt_db_access_ptr &access);
  _Tt_db_access_ptr getCurrentAccess () const;

  // Returns the file partition name that the specified file is in and
  // the real path of the file on the remote machine.  It is assumed that
  // the file is on the same machine that the DB connection is on.
  _Tt_db_results getFilePartition (const _Tt_string &path,
				   _Tt_string       &partition,
				   _Tt_string       &network_path);

  // Creates a file entry in the database.  A list of file properties may
  // be specified along with the file name.  The cache level returned
  // can be used to initialize the cache levels for the types of info
  // stored for this file.
  _Tt_db_results createFile (const _Tt_string               &file,
			     const _Tt_db_property_list_ptr &properties,
			     const _Tt_db_access_ptr	    &access,
			     int                            &cache_level);

  // Creates an object entry in the database. If the file does not already
  // exist in the database, the file entry is also created.  The cache level
  // returned can be used to initialize the cache levels for the types of info
  // stored for this object.
  _Tt_db_results createObject (const _Tt_string               &file,
			       const _Tt_string               &objid,
			       const _Tt_string	              &otype,
			       const _Tt_db_property_list_ptr &properties,
			       const _Tt_db_access_ptr	      &access,
			       int                            &cache_level);

  // Removes the specified file and all of its objects from the database.
  _Tt_db_results removeFile (const _Tt_string &file);

  // Removes the specified field from the databse.
  _Tt_db_results removeObject (const _Tt_string &objid,
			       const _Tt_string &forward_pointer);

  // Moves the specified file and all of its objects to the new file.
  _Tt_db_results moveFile (const _Tt_string &file,
			   const _Tt_string &new_file);

  // Writes the specified file property to the database.  All values of the
  // the file property with the same name are overwritten.  If the value of
  // the property is NULL, this deletes all properties with the same name
  // from the database.
  _Tt_db_results setFileProperty (const _Tt_string          &file,
				  const _Tt_db_property_ptr &property,
				  int                       &cache_level);

  // Writes the specified file properties to the database.  All values of
  // the the file properties with the same names are overwritten.  If the
  // value of some of the properties are NULL, this deletes all properties
  // with the same name from the database.
  _Tt_db_results
  setFileProperties (const _Tt_string               &file,
		     const _Tt_db_property_list_ptr &properties,
		     int                            &cache_level);

  // Adds a file property to the list of values of the file property
  // with the same name.  If the property does not already exist, then
  // the property is created.  If the "unique" argument is TRUE, then
  // the property name and value combination is guaranteed to be unique
  // and will not be added to the list of values if the combination already
  // exists.
  _Tt_db_results addFileProperty (const _Tt_string          &file,
				  const _Tt_db_property_ptr &property,
				  bool_t                     unique,
                                  int                       &cache_level);

  // Deletes the file property value with the same name and value.  If
  // the value is left empty, then all values of the property are deleted.
  _Tt_db_results
  deleteFileProperty (const _Tt_string          &file,
		      const _Tt_db_property_ptr &property,
		      int                       &cache_level);

  // Gets the values of the file property with the specified name.
  _Tt_db_results getFileProperty (const _Tt_string    &file,
				  const _Tt_string    &name,
				  int                 &cache_level,
				  _Tt_db_property_ptr &property);

  // Gets a list of all of the file properties.
  _Tt_db_results
  getFileProperties (const _Tt_string         &file,
		     int                      &cache_level,
		     _Tt_db_property_list_ptr &properties);

  // Returns a list of all of the object IDs for the specified file.
  _Tt_db_results getFileObjects (const _Tt_string    &file,
				 int                 &cache_level,
				 _Tt_string_list_ptr &objids);

  // Sets and gets the access mode of the specified file.
  _Tt_db_results setFileAccess (const _Tt_string        &file,
				const _Tt_db_access_ptr &access);
  _Tt_db_results getFileAccess (const _Tt_string  &file,
				_Tt_db_access_ptr &access);
 
  // Writes the specified object property to the database.  All values of the
  // the object property with the same name are overwritten.  If the value of
  // the property is NULL, this deletes all properties with the same name
  // from the database.  If the cache level in the database is higher then
  // the cache level passed in, the write is not done and the values of all
  // properties in the database are returned through the properties argument.
  // The latest cache level is always returned.
  _Tt_db_results setObjectProperty (const _Tt_string          &objid,
				    const _Tt_db_property_ptr &property,
				    _Tt_db_property_list_ptr  &properties,
				    int                       &cache_level);

  // Writes the specified object properties to the database.  All values of
  // the object properties with the same names are overwritten.  If the value
  // of some of the properties are NULL, this deletes all properties with the
  // same name from the database.  If the cache level in the database is
  // higher then the cache level passed in, the write is not done and
  // all of the object properties in the database are returned through the
  // out_properties argument.  The latest cache level is always returned.
  _Tt_db_results
  setObjectProperties (const _Tt_string               &objid,
		       const _Tt_db_property_list_ptr &in_properties,
		       _Tt_db_property_list_ptr       &out_properties,
		       int                            &cache_level);

  // Adds an object property to the list of values of the object property
  // with the same name.  If the property does not already exist, then
  // the property is created.  If the "unique" argument is TRUE, then
  // the property name and value combination is guaranteed to be unique
  // and will not be added to the list of values if the combination already
  // exists.  If the cache level in the database is higher then the cache
  // level passed in, the write is not done and the values of all the properties
  // in the database are returned through the properties argument.
  // The latest cache level is always returned.
  _Tt_db_results addObjectProperty (const _Tt_string          &objid,
				    const _Tt_db_property_ptr &property,
				    bool_t                     unique,
				    _Tt_db_property_list_ptr  &properties,
				    int                       &cache_level);

  // Deletes the object property values with the same name and values.  If
  // the value list is empty, then all values of the property are deleted.
  // If the cache level in the database is higher then the cache level passed
  // in, the write is not done and the values of all the properties
  // in the database are returned through the properties argument.
  // The latest cache level is always returned.
  _Tt_db_results
  deleteObjectProperty (const _Tt_string          &objid,
			const _Tt_db_property_ptr &property,
			_Tt_db_property_list_ptr  &properties,
			int                       &cache_level);

  // Gets the values of the object property with the specified name.
  _Tt_db_results getObjectProperty (const _Tt_string    &objid,
				    const _Tt_string    &name,
				    int                 &cache_level,
				    _Tt_db_property_ptr &property);

  // Gets a list of all of the object properties.
  _Tt_db_results
  getObjectProperties (const _Tt_string         &objid,
		       int                      &cache_level,
		       _Tt_db_property_list_ptr &property);

  // Sets and gets the otype of the specified object.
  _Tt_db_results setObjectType (const _Tt_string &objid,
				const _Tt_string &otype);
  _Tt_db_results getObjectType (const _Tt_string &objid,
				_Tt_string       &otype);
 
  // Sets the file of the object.  This should only be used to support
  // a same partition move of an object.
  _Tt_db_results setObjectFile (const _Tt_string &objid,
				const _Tt_string &file);
  // Gets the file of the object.
  _Tt_db_results getObjectFile (const _Tt_string &objid,
				_Tt_string       &file);

  // Sets and gets the access mode of the specified object.
  _Tt_db_results setObjectAccess (const _Tt_string        &objid,
				  const _Tt_db_access_ptr &access);
  _Tt_db_results getObjectAccess (const _Tt_string  &objid,
				  _Tt_db_access_ptr &access);

  _Tt_db_results
  isFileInDatabase (const _Tt_string &file,
		    bool_t           &directory_flag);
 
  _Tt_db_results
  isObjectInDatabase (const _Tt_string &objid,
		      _Tt_string       &forward_pointer);
 
  // For the specified file, queues the message and the list of ptypes
  // that the message is addressed to.
  _Tt_db_results queueMessage (const _Tt_string          &file,
			       const _Tt_string_list_ptr &ptypes,
			       const _Tt_message_ptr     &message);

  // For the specified file, returns a list of messages that are addressed
  // to the specified ptypes.  If there are no other ptypes that this message
  // is addressed to, the message is deleteed from the database.
  _Tt_db_results dequeueMessages (const _Tt_string          &file,
				  const _Tt_string_list_ptr &ptypes,
				  _Tt_message_list_ptr      &messages);

  // Gets the remote ends' idea of what the netfile version of the specified
  // file is.
  _Tt_db_results file_netfile (const _Tt_string &file, _Tt_string &netfile);

  // Gets the remote ends' idea of what the file version of the specified
  // netfile is.
  _Tt_db_results netfile_file (const _Tt_string &netfile, _Tt_string &file);

  // Return a list of all of the sessions known to the dbserver.
  _Tt_string_list *  get_all_sessions();

  // Tell the dbserver to perform garbage collection.
  _Tt_db_results garbage_collect_in_server();

  // Delete the named session.
  _Tt_db_results delete_session(_Tt_string session);

// ********** Old DB Server Compatibility Members **********

  // The following members are used to provide compatibility
  // with old DB servers.  All of the member functions map directly
  // to an old DB server RPC call.  The names of these member functions
  // have been kept the same as the base name of the corresponding
  // RPC call.
  _Tt_string mfs (const _Tt_string &path);
  _Tt_db_results addsession (const _Tt_string &file,
			     const _Tt_string &session);
  _Tt_db_results delsession (const _Tt_string &file,
			     const _Tt_string &session);
  _Tt_db_results gettype (const _Tt_string &objid,
			  _Tt_string       &type);
  int isaddindex (int, struct keydesc*);
  int isbuild (char*, int, struct keydesc*, int);
  int isclose (int);
  int iscntl (int, int, char*);
  int isdelrec (int, long, char*);
  int iserase (char*);
  int isopen (char*, int);
  int isread (int, char*, int);
  int isrewrec (int, long, char*);
  int isstart (int, struct keydesc*, int, char*, int);
  int iswrite (int, char*);

  // Data members used by the old DB server compatibility member
  // functions.
  int  iserrno;
  long isrecnum;
  int  isreclen;

// ********** Old DB Server Compatibility Members **********

private:
  _Tt_db_access_ptr  dbAccess;
  int                dbAuthLevel;
  _Tt_db_results     dbConnectionResults;
  _Tt_string         dbHostname;
  CLIENT            *dbServer;
  _Tt_string         dbServerNetName;
#if !defined(OPT_TLI)
  sockaddr_in        dbSocket;
#endif
  int		     dbVersion;

  void createAuth ();
  void setTtDBDefaults ();
  _Tt_db_results connectToDB (const _Tt_string&);
  void SetError(enum clnt_stat cf_stat);

// ********** Old DB Server Compatibility Members **********

  // Helper functions for old DB server function compatibility

_Tt_db_results oldDBToNewDBResults (_Tt_old_db_results old_db_results);

// ********** Old DB Server Compatibility Members **********
};

#endif // _TT_DB_H
