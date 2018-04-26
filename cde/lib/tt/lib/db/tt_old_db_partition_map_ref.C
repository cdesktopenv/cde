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
//%%  $XConsortium: tt_old_db_partition_map_ref.C /main/3 1995/10/23 10:06:41 rswiston $ 			 				
/*
 * tt_db_partition_global_map_ref.cc - Define the TT DB server partition
 *                                     global map ref class.
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 *
 */

#include "util/tt_port.h"
#include "util/tt_gettext.h"
#include "db/tt_old_db_partition_map_ref.h"

_Tt_old_db_table_ptr*
_Tt_old_db_partition_map_ref::dbPartitionMap = (_Tt_old_db_table_ptr *)
						  NULL;

_Tt_old_db_partition_map_ref::_Tt_old_db_partition_map_ref ()
{
  if (!dbPartitionMap) {
    dbPartitionMap = new _Tt_old_db_table_ptr;
    *dbPartitionMap = new _Tt_old_db_table ((_Tt_object_table_keyfn)
					    &dbPartitionMapKey);
  }
}

_Tt_old_db_partition_map_ref::~_Tt_old_db_partition_map_ref ()
{
  // The static table is freed up when the application dies or when
  // _Tt_old_db_partition_map_ref::flush() is called.
}

void _Tt_old_db_partition_map_ref::addDB (_Tt_old_db_ptr &db)
{
  (*dbPartitionMap)->insert(db);
}

void _Tt_old_db_partition_map_ref::removeDB (const _Tt_string &partition)
{
  (*dbPartitionMap)->remove(partition);
}

_Tt_old_db_ptr
_Tt_old_db_partition_map_ref::getDB (const _Tt_string        &partition,
				     const _Tt_db_client_ptr &db_conn)
{
  _Tt_old_db_ptr db_ptr = (*dbPartitionMap)->lookup(partition);
  if (db_ptr.is_null()) {
    db_ptr = new _Tt_old_db(partition, db_conn);

    _Tt_db_results results = db_ptr->getDBResults();
    if (results != TT_DB_OK) {
      _tt_syslog(0, LOG_ERR,
		   catgets(_ttcatd, 1, 7,
			   "Attempt to open database table %s:%s failed"),
		   (char *)db_conn->getHostname(),
		   (char *)db_ptr->getLastFileAccessed());
      db_ptr = (_Tt_old_db *)NULL;
    }
    else {
      addDB(db_ptr);
    }
  }

  return db_ptr;
}

_Tt_string
_Tt_old_db_partition_map_ref::dbPartitionMapKey (_Tt_object_ptr &db)
{
  return (((_Tt_old_db *)db.c_pointer())->getPartition());
}
 
void _Tt_old_db_partition_map_ref::flush ()
{
  if (dbPartitionMap) {
    delete dbPartitionMap;
  }
  dbPartitionMap = (_Tt_old_db_table_ptr *)NULL;
}
