/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: tt_db_hostname_global_map_ref.h /main/3 1995/10/23 10:02:37 rswiston $ 			 				 */
/*
 * tt_db_hostname_global_map_ref.h - Declare the TT DB server hostname 
 *                                   global map ref class.  This class
 *                                   contains a global map that provides
 *				     a hostname to DB connection mapping.
 *				     This way only one RPC connection per
 *			    	     machine is created.
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */

#ifndef _TT_DB_HOSTNAME_GLOBAL_MAP_REF_H
#define _TT_DB_HOSTNAME_GLOBAL_MAP_REF_H

#include "util/tt_new.h"
#include "util/tt_string.h"
#include "db/tt_db_client_utils.h"

class _Tt_db_hostname_global_map_ref : public _Tt_allocated {
public:
  _Tt_db_hostname_global_map_ref ();
  ~_Tt_db_hostname_global_map_ref ();

  void              addDB (_Tt_db_client_ptr &db);
  void              removeDB (const _Tt_string &hostname);
  _Tt_db_client_ptr getDB (const _Tt_string &hostname,
			   _Tt_string       &real_hostname,
			   _Tt_db_results   &dbResults);

  static void flush ();
  static _Tt_string dbHostnameMapKey (_Tt_object_ptr &db);

private:
  static _Tt_db_client_table_ptr *dbHostnameMap;

};

#endif // _TT_DB_HOSTNAME_GLOBAL_MAP_REF_H
