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
//%%  $XConsortium: tt_db_network_path.C /main/3 1995/10/23 10:03:29 rswiston $ 			 				
/*
 * tt_db_network_path.cc - Defines a function that takes a local
 *	path and returns the real local path, hostname, partition
 *	and real remote path of the file.  The partition and real
 *	remote path are for the file on its native host.
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 * Implementation for filepath utility functions
 *
 */

#include "util/tt_path.h"
#include "util/tt_file_system.h"
#include "util/tt_file_system_entry.h"
#include "db/tt_db_hostname_global_map_ref.h"

_Tt_db_results _tt_db_network_path (const _Tt_string &path,
				    _Tt_string	     &local_path,
                          	    _Tt_string       &hostname,
			  	    _Tt_string	     &partition,
                          	    _Tt_string       &network_path)
{
	_Tt_db_results results = TT_DB_OK;


	if (_tt_is_network_path(path)) {
		local_path = _tt_network_path_to_local_path(path);
	} else {
		local_path = _tt_realpath(path);
	}

	_Tt_file_system file_system;
	_Tt_file_system_entry_ptr file_system_entry =
			    file_system.bestMatchToPath(local_path);

	hostname = file_system_entry->getHostname();

	if (file_system_entry->isLocal()) {
		partition = file_system_entry->getMountPoint();

		_Tt_string loop_back_mount_point =
		       file_system_entry->getLoopBackMountPoint();

		if (loop_back_mount_point.len ()) {
			// Get the path info after the mount point path
			local_path = local_path.right(local_path.len() -
						loop_back_mount_point.len());

			// Replace the mount point path with
			// the exported partition path.
			if (partition != "/") {
				local_path = loop_back_mount_point.cat(local_path);
			}
		} 
		network_path = hostname.cat(":").cat(local_path);
	} else {
		_Tt_db_hostname_global_map_ref map_ref;
		_Tt_db_client_ptr database =
			map_ref.getDB(hostname, hostname, results);

		if (!database.is_null()) {
			_Tt_string temp_string;
			_Tt_string local_network_path;

			if (_tt_is_network_path(path)) {
				local_network_path = path;
			} else {
				local_network_path =
					_tt_local_network_path(path);
			}

			results = database->getFilePartition(local_network_path,
							     partition,
							     network_path);
		}

		if ((results == TT_DB_ERR_DB_CONNECTION_FAILED) ||
		    (results == TT_DB_ERR_RPC_CONNECTION_FAILED) ||
		    (results == TT_DB_ERR_RPC_FAILED) ||
		    (results == TT_DB_ERR_DB_OPEN_FAILED)) {
			map_ref.removeDB (hostname);
		}
	}

	return results;
}
