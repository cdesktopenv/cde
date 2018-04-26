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
/*%%  $XConsortium: tt_old_db.h /main/3 1995/10/23 10:05:54 rswiston $ 			 				 */
/*
 *
 * @(#)tt_old_db.h	1.4 30 Jul 1993
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */
/*
 * tt_old_db.h - Defines the old TT DB server database.  This class
 *               represents the actual data model used to store
 *               ToolTalk databse information in the old DB server.
 *               This is used purely for compatibility with the
 *               old DB server.
 *
 * The actual database schema is the following:
 *
 * Table         Fields        Length                 Description
 * -----         ------        ------                 -----------
 * docoid_path                                        Contains the names of all
 *                                                    the that have properties
 *                                                    or objects in the db.
 *
 *               File Key      TT_DB_KEY_LENGTH       Unique key that is used
 *                                                    to search (16 bytes) for
 *                                                    a file's properties and
 *                                                    objects in the other
 *                                                    tables.
 *
 *               File Name     Variable up to         The name of the file.
 *                             MAXPATHLEN (256)
 *
 * Table         Fields        Length                 Description
 * -----         ------        ------                 -----------
 * oid_container                                      Contains all of the
 *                                                    mappings of file keys to
 *                                                    the keys of the objects
 *                                                    in the file.
 *
 *               Object Key    TT_DB_KEY_LENGTH       Unique key of an object
 *                                                    in a file.
 *
 *               File Key      TT_DB_KEY_LENGTH       Unique key of a file.
 *
 * Table         Fields        Length                 Description
 * -----         ------        ------                 -----------
 * oid_prop                                           Contains all of the
 *                                                    properties corresponding
 *                                                    to files and objects in
 *                                                    the db.
 *
 *               Key           TT_DB_KEY_LENGTH       Unique key of an object
 *                                                    or a file.
 *
 *               Property Name TT_DB_PROP_NAME_LENGTH The name of a file or
 *                                                    object property.
 *
 *               Property      Variable up to        The value of the property.
 *                             ISMAXRECLEN. 
 *
 * Table         Fields        Length                 Description
 * -----         ------        ------                 -----------
 * oid_access                                         Contains the access
 *                                                    privileges of all the
 *                                                    files and objects in
 *                                                    the db.
 *
 *               Key           TT_DB_KEY_LENGTH       Unique key of an object
 *                                                    or a file.
 *
 *               User ID       XDR_SHORT_SIZE         The user ID of the owner
 *                                                    of the object or file.
 *
 *               Group ID      XDR_SHORT_SIZE         The group ID of the
 *                                                    object or file.
 *
 *               Mode          XDR_SHORT_SIZE         The access mode of the
 *                                                    object or file.
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */

#ifndef _TT_OLD_DB_H
#define _TT_OLD_DB_H

#include "util/tt_object.h"
#include "util/tt_string.h"
#include "db/tt_client_isam_file_utils.h"
#include "db/tt_db_access_utils.h"
#include "db/tt_db_client_utils.h"
#include "db/tt_db_key_utils.h"
#include "db/tt_db_property_utils.h"
#include "db/tt_db_results.h"

class _Tt_old_db : public _Tt_object {
public:
  // Empty constructor for _utils
  _Tt_old_db ();

  _Tt_old_db (const _Tt_string&,
	      const _Tt_db_client_ptr&);
  ~_Tt_old_db ();

  // Returns the partition this DB object is connected to
  _Tt_string getPartition () const
    {
      _Tt_string partition = (char *)dbPartition;
      return partition;
    }

  // Creates a file in the TT DB and sets the access permissions of the
  // file.
  _Tt_db_results createFile (const _Tt_string        &file,
			     const _Tt_db_access_ptr &access);

  // Creates an object in the TT DB.  If the file is not in the database,
  // the file will be created using the "file_access" argument to specify
  // its access permission.  If no file is specified, this is a special
  // object being created just to hold a forward pointer.
  _Tt_db_results createObject (const _Tt_string        &file,
			       const _Tt_string        &objid,
			       const _Tt_db_access_ptr &object_access,
			       const _Tt_db_access_ptr &file_access);

  _Tt_db_results removeFile (const _Tt_string        &file,
			     const _Tt_db_access_ptr &access);

  _Tt_db_results removeObject (const _Tt_string        &objid,
			       const _Tt_db_access_ptr &access);

  _Tt_db_results moveFile (const _Tt_string        &file,
			   const _Tt_string        &new_file,
			   const _Tt_db_access_ptr &access);

  _Tt_db_results setFileProperty (const _Tt_string          &file,
				  const _Tt_db_property_ptr &property,
				  const _Tt_db_access_ptr   &access);

  _Tt_db_results setFileProperties (const _Tt_string               &file,
				    const _Tt_db_property_list_ptr &properties,
				    const _Tt_db_access_ptr        &access);

  _Tt_db_results addFileProperty (const _Tt_string          &file,
				  const _Tt_db_property_ptr &property,
				  bool_t                     unique,
				  const _Tt_db_access_ptr   &access);

  _Tt_db_results deleteFileProperty (const _Tt_string          &file,
				     const _Tt_db_property_ptr &property,
				     const _Tt_db_access_ptr   &access);

  _Tt_db_results deleteFileProperties (const _Tt_string        &file,
				       const _Tt_db_access_ptr &access);

  _Tt_db_results getFileProperty (const _Tt_string        &file,
				  const _Tt_string        &name,
				  const _Tt_db_access_ptr &access,
				  _Tt_db_property_ptr     &property);

  _Tt_db_results getFileProperties (const _Tt_string         &file,
				    const _Tt_db_access_ptr  &access,
				    _Tt_db_property_list_ptr &properties);

  _Tt_db_results getFileObjects (const _Tt_string        &file,
				 const _Tt_db_access_ptr &access,
				 _Tt_string_list_ptr     &objids);

  _Tt_db_results deleteFileObjects (const _Tt_string        &file,
				    const _Tt_db_access_ptr &access);

  _Tt_db_results setFileFile (const _Tt_string        &file,
			      const _Tt_string        &new_file,
			      const _Tt_db_access_ptr &access);

  // If the specified file is a directory, this returns all of the
  // file names stored in the database that are in the directory.
  // The file itself is also in the return list.
  _Tt_db_results getFileChildren (const _Tt_string    &file,
				  _Tt_string_list_ptr &children);

  _Tt_db_results setFileAccess (const _Tt_string        &file,
				const _Tt_db_access_ptr &new_access,
				const _Tt_db_access_ptr &access);

  _Tt_db_results getFileAccess (const _Tt_string        &file,
				const _Tt_db_access_ptr &access,
				_Tt_db_access_ptr       &current_access);

  _Tt_db_results setObjectProperty (const _Tt_string          &objid,
				    const _Tt_db_property_ptr &property,
				    const _Tt_db_access_ptr   &access);

  _Tt_db_results
  setObjectProperties (const _Tt_string               &objid,
		       const _Tt_db_property_list_ptr &properties,
		       const _Tt_db_access_ptr        &access);

  _Tt_db_results addObjectProperty (const _Tt_string          &objid,
				    const _Tt_db_property_ptr &property,
				    bool_t                     unique,
				    const _Tt_db_access_ptr   &access);

  _Tt_db_results deleteObjectProperty (const _Tt_string          &objid,
				       const _Tt_db_property_ptr &property,
				       const _Tt_db_access_ptr   &access);

  _Tt_db_results deleteObjectProperties (const _Tt_string        &objid,
					 const _Tt_db_access_ptr &access);

  _Tt_db_results getObjectProperty (const _Tt_string        &objid,
				    const _Tt_string        &name,
				    const _Tt_db_access_ptr &access,
				    _Tt_db_property_ptr     &property);

  _Tt_db_results
  getObjectProperties (const _Tt_string         &objid,
		       const _Tt_db_access_ptr  &access,
		       _Tt_db_property_list_ptr &properties);

  // Used to facilitate moving an object to a new file on the same
  // partition.  The file specified must be on the partition this DB
  // server object is connected to.  If the file is not in the database,
  // it will be created with the access permissions of the object.
  _Tt_db_results setObjectFile (const _Tt_string        &objid,
				const _Tt_string        &file,
				const _Tt_db_access_ptr &access);

  _Tt_db_results getObjectFile (const _Tt_string        &objid,
				const _Tt_db_access_ptr &access,
				_Tt_string              &file);

  _Tt_db_results setObjectAccess (const _Tt_string        &objid,
				  const _Tt_db_access_ptr &new_access,
				  const _Tt_db_access_ptr &access);

  _Tt_db_results getObjectAccess (const _Tt_string        &objid,
				  const _Tt_db_access_ptr &access,
				  _Tt_db_access_ptr       &current_access);

  _Tt_db_results getDBResults () const
    {
      return dbResults;
    }

  const _Tt_string &getLastFileAccessed () const
    {
      return dbLastFileAccessed;
    }

  // Needed by addsession and delsession in _Tt_db_client
  _Tt_db_results getFileKey (const _Tt_string&, _Tt_string&);

  // Needed by addsession, delsession and gettype in _Tt_db_client
  int propertyTableFD;

  // Some of the keys are needed by _Tt_db_client for the addsession,
  // delsession and gettype functions.  All of the keys have been made
  // public in case similar needs arise later.
  _Tt_client_isam_key_descriptor_ptr fileTableFileKey;
  _Tt_client_isam_key_descriptor_ptr fileTableFilePathKey;
  _Tt_client_isam_key_descriptor_ptr fileObjectMapFileKey;
  _Tt_client_isam_key_descriptor_ptr fileObjectMapObjectKey;
  _Tt_client_isam_key_descriptor_ptr propertyTablePropertyKey;
  _Tt_client_isam_key_descriptor_ptr accessTableKey;

 private:
  _Tt_client_isam_file_ptr fileTable;
  _Tt_client_isam_file_ptr fileObjectMap;
  _Tt_client_isam_file_ptr propertyTable;
  _Tt_client_isam_file_ptr accessTable;

  _Tt_string               dbHostname;
  _Tt_string               dbPartition;
  _Tt_db_results           dbResults;
  _Tt_string		   dbLastFileAccessed;

  void connectToDB (const _Tt_string&, const _Tt_db_client_ptr&);

  _Tt_db_results verifyObjectAccess (const _Tt_string&,
			             const _Tt_db_access_ptr&,
			             bool_t=FALSE,
			             bool_t=FALSE);
  _Tt_db_results verifyUserOnlyObjectAccess (const _Tt_string&,
                                             const _Tt_db_access_ptr&);
  _Tt_db_results verifyAccess (const _Tt_string&,
			       const _Tt_db_access_ptr&,
			       bool_t=FALSE,
			       bool_t=FALSE);
  _Tt_string getObjectKey (const _Tt_string&);

  _Tt_db_results setProperty (const _Tt_string&, const _Tt_db_property_ptr&);
  _Tt_db_results setProperties (const _Tt_string&,
				const _Tt_db_property_list_ptr&);
  _Tt_db_results addProperty (const _Tt_string&,
			      const _Tt_db_property_ptr&,
			      bool_t);
  _Tt_db_results deleteProperty (const _Tt_string&,
				 const _Tt_db_property_ptr&);
  _Tt_db_results deleteProperties (const _Tt_string&,
				   bool_t=FALSE);
  _Tt_db_results getProperty (const _Tt_string&,
			      const _Tt_string&,
			      _Tt_db_property_ptr&);
  _Tt_db_results getProperties (const _Tt_string&,
			        _Tt_db_property_list_ptr&);
  _Tt_db_results setAccess (const _Tt_string&, const _Tt_db_access_ptr&);
  _Tt_db_results getAccess (const _Tt_string&, _Tt_db_access_ptr&);
  _Tt_db_results getFile (const _Tt_string&, _Tt_string&);
  
  _Tt_db_results addPropertyValue (const _Tt_string&,
				   const _Tt_string&,
			           const _Tt_string&);
};

#endif /* _TT_OLD_DB_H */
