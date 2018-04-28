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
//%%  $XConsortium: tt_db_partition_global_map_ref.C /main/3 1995/10/20 16:42:23 rswiston $ 			 				
/*
 * tt_db_partition_global_map_ref.cc - Define the TT DB server partition
 *                                     global map ref class.
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 *
 */

#include <errno.h>
#include <sys/stat.h>
#include "util/tt_port.h"
#include "util/tt_gettext.h"
#include "tt_isam_file_utils.h"
#include "tt_isam_record_utils.h"
#include "tt_db_partition_redirection_map.h"
#include "tt_db_partition_global_map_ref.h"
#include "tt_db_server_consts.h"

extern _Tt_db_partition_redirection_map *db_pr_map;
extern FILE *errstr;

_Tt_db_server_db_table_ptr*
_Tt_db_partition_global_map_ref::dbPartitionMap = (_Tt_db_server_db_table_ptr *)
						  NULL;

_Tt_db_partition_global_map_ref::_Tt_db_partition_global_map_ref ()
{
  if (!dbPartitionMap) {
    dbPartitionMap = new _Tt_db_server_db_table_ptr;
    *dbPartitionMap = new _Tt_db_server_db_table ((_Tt_object_table_keyfn)
						  &dbPartitionMapKey);
  }
}

_Tt_db_partition_global_map_ref::~_Tt_db_partition_global_map_ref ()
{
  // The static table is freed up when the application dies.
}

void _Tt_db_partition_global_map_ref::addDB (_Tt_db_server_db_ptr &db)
{
  (*dbPartitionMap)->insert(db);
}

void _Tt_db_partition_global_map_ref::removeDB (const _Tt_string &partition)
{
  (*dbPartitionMap)->remove(partition);
}

_Tt_db_server_db_ptr
_Tt_db_partition_global_map_ref::getDB (const _Tt_string &partition)
{
  _Tt_string real_partition = db_pr_map->findEntry(partition);
  if (real_partition.is_null() || !real_partition.len()) {
    real_partition = partition;
  }

  _Tt_db_server_db_ptr db_ptr = (*dbPartitionMap)->lookup(real_partition);
  if (db_ptr.is_null()) {
    if (!checkForOldDB(real_partition)) {
      db_ptr = new _Tt_db_server_db(real_partition);

      _Tt_db_results results = db_ptr->getDBResults();
      if (results != TT_DB_OK) {
	      _tt_syslog(errstr, LOG_ERR,
			 "_Tt_db_server_db(\"%s\"): %d (%s)",
			 (char *)real_partition, results,
			 (char *)db_ptr->getLastFileAccessed());
	      db_ptr = (_Tt_db_server_db *)NULL;
      }
      else {
        addDB(db_ptr);
      }
    }
  }

  return db_ptr;
}

_Tt_string
_Tt_db_partition_global_map_ref::dbPartitionMapKey (_Tt_object_ptr &db)
{
  return (((_Tt_db_server_db *)db.c_pointer())->getPartition());
}

bool_t
_Tt_db_partition_global_map_ref::checkForOldDB(const _Tt_string &partition)
{
  if (checkForOldDBTable(partition, "docoid_path", TT_DB_FILE_TABLE_FILE)) {
    return TRUE;
  }

  if (checkForOldDBTable(partition,
			 "oid_container",
			 TT_DB_FILE_OBJECT_MAP_FILE)) {
    return TRUE;
  }

  if (checkForOldDBTable(partition, "oid_prop", TT_DB_PROPERTY_TABLE_FILE)) {
    return TRUE;
  }

  if (checkForOldDBTable(partition, "oid_access", TT_DB_ACCESS_TABLE_FILE)) {
    return TRUE;
  }

  return FALSE;
}

bool_t _Tt_db_partition_global_map_ref
       ::checkForOldDBTable (const _Tt_string &partition,
			     const _Tt_string &old_file,
			     const _Tt_string &new_file)
{
  static bool_t old_db_diagnostic = FALSE;

  _Tt_string base_dir = (char *)partition;
  if (base_dir[base_dir.len()-1] != '/') {
    base_dir = base_dir.cat("/");
  }
  base_dir = base_dir.cat("TT_DB/");

  _Tt_string old_db_file(base_dir);
  old_db_file = old_db_file.cat(old_file);

  _Tt_string new_db_file(base_dir);
  new_db_file = new_db_file.cat(new_file);

  _Tt_string old_db_index(old_db_file);
  old_db_index = old_db_index.cat(".ind");

  _Tt_string new_db_index(new_db_file);
  new_db_index = new_db_index.cat(".ind");

  struct stat stat_buf;
  int         temp_errno = 0;

  // If there is no new DB and there is an old DB...
  if (stat((char *)new_db_index, &stat_buf) &&
      ((temp_errno = errno) == ENOENT) &&
      !stat((char *)old_db_index, &stat_buf)) {
    if (new_file == TT_DB_ACCESS_TABLE_FILE) {
      _Tt_isam_file_ptr old_db_table =
        new _Tt_isam_file(old_db_file, ISFIXLEN+ISINOUT+ISEXCLLOCK);
      _Tt_isam_file_ptr access_table;
      
      int results = old_db_table->getErrorStatus();
      
      if (!results) {
	_Tt_isam_key_descriptor_ptr access_key = new _Tt_isam_key_descriptor;
	access_key->addKeyPart(0, TT_DB_KEY_LENGTH, BINTYPE);
	
	access_table =
	  new _Tt_isam_file(new_db_file,
			    TT_DB_KEY_LENGTH+3*TT_DB_LONG_SIZE, 
			    TT_DB_KEY_LENGTH+3*TT_DB_LONG_SIZE,   
			    access_key,
			    ISFIXLEN+ISINOUT+ISEXCLLOCK);
	results = access_table->getErrorStatus();

        if (!results) {
	  results = access_table->writeMagicString(_Tt_string(TT_DB_VERSION));
        }
      }
      
      if (!results) {
	_Tt_isam_record_ptr new_record = access_table->getEmptyRecord();
	_Tt_isam_record_ptr record;
	
	while (!results) {
	  record = old_db_table->readRecord(ISNEXT);
	  results = old_db_table->getErrorStatus();
	  
	  if (!results) {
	    memset((char *)new_record->getRecord(),
		   '\0',
		   new_record->getLength());

	    memcpy((char *)new_record->getRecord(),
		   (char *)record->getRecord(),
		   TT_DB_KEY_LENGTH);

            short n_user = *(short *)
			    ((char *)record->getRecord()+TT_DB_KEY_LENGTH);
	    long user = (long)ntohs(n_user);
            u_long nl_user = htonl(user);
	    memcpy((char *)new_record->getRecord()+TT_DB_ACCESS_USER_OFFSET,
		   (char *)&nl_user,
		   TT_DB_LONG_SIZE);

            short n_group = *(short *)
			     ((char *)record->getRecord()+
                                      TT_DB_KEY_LENGTH+TT_DB_SHORT_SIZE);
	    long group = (long)ntohs(n_group);
            u_long nl_group = htonl(group);
	    memcpy((char *)new_record->getRecord()+TT_DB_ACCESS_GROUP_OFFSET,
		   (char *)&nl_group,
		   TT_DB_LONG_SIZE);
	
	    short n_mode = *(short *)
			    ((char *)record->getRecord()+
			             TT_DB_KEY_LENGTH+2*TT_DB_SHORT_SIZE);
	    long mode = (long)ntohs(n_mode);
            u_long nl_mode = htonl(mode);
	    memcpy((char *)new_record->getRecord()+TT_DB_ACCESS_MODE_OFFSET,
		   (char *)&nl_mode,
		   TT_DB_LONG_SIZE);
	    
	    results = access_table->writeRecord(new_record);
	  }
	}
	
	if (results == EENDFILE) {
	  results = 0;
	}
      }
      
      if (!results) {
	(void)iserase((char *)old_db_file);
      }
      
      if (results) {
	return TRUE;
      }
    }
    else {
      if (isrename((char *)old_db_file, (char *)new_db_file)) {
	return TRUE;
      }
      else {
        _Tt_isam_file_ptr new_db_table =
	  new _Tt_isam_file(new_db_file, ISFIXLEN+ISINOUT+ISEXCLLOCK);
        int results = new_db_table->getErrorStatus();

	if (results) {
	  new_db_table = new _Tt_isam_file(new_db_file,
				           ISVARLEN+ISINOUT+ISEXCLLOCK);
          results = new_db_table->getErrorStatus();
	}

	if (!results) {
	  (void)new_db_table->writeMagicString(TT_DB_VERSION);
        }
      }
    }
  }
  else if (temp_errno && (temp_errno != ENOENT)) {
    return TRUE;
  }
  else if (!stat((char *)old_db_index, &stat_buf)) {
    if (old_db_diagnostic == FALSE) {
	    _tt_syslog(errstr, LOG_ERR,
		       catgets(_ttcatd, 5, 5,
			       "Any data written using an old (<= 1.0.2) "
			       "rpc.ttdbserverd after using a new (>= 1.1) "
			       "rpc.ttdbserverd will be ignored"));
	    old_db_diagnostic = TRUE;
    }
  }

  return FALSE;
}
