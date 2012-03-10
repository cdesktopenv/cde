/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: tt_old_db_partition_map_ref.h /main/3 1995/10/23 10:06:49 rswiston $ 			 				 */
/*
 * tt_old_db_partition__map_ref.h - Declare the old TT DB server partition
 *                           global map ref class.  This class contains
 *			     a global map that provides a partition to
 *			     old DB server DB mapping.  This way only one set
 *			     of ISAM file connections is made per partition
 *			     from a particular client.
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */

#ifndef _TT_OLD_DB_PARTITION_MAP_REF_H
#define _TT_OLD_DB_PARTITION_MAP_REF_H

#include "util/tt_new.h"
#include "util/tt_string.h"
#include "db/tt_old_db_utils.h"

class _Tt_old_db_partition_map_ref : public _Tt_allocated {
public:
  _Tt_old_db_partition_map_ref ();
  ~_Tt_old_db_partition_map_ref ();

  void           addDB (_Tt_old_db_ptr &db);
  void           removeDB (const _Tt_string &partition);
  _Tt_old_db_ptr getDB (const _Tt_string        &partition,
			const _Tt_db_client_ptr &db_conn);

  static void flush (); 

  static _Tt_string dbPartitionMapKey (_Tt_object_ptr &db);

private:
  static _Tt_old_db_table_ptr *dbPartitionMap;

  bool_t checkForOldDB (const _Tt_string&);
  bool_t checkForOldDBTable (const _Tt_string&,
			     const _Tt_string&,
			     const _Tt_string&);
};

#endif // _TT_OLD_DB_PARTITION_MAP_REF_H
