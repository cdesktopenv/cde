//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $XConsortium: tt_db_partition_redirection_map.C /main/3 1995/10/20 16:42:41 rswiston $ 			 				
/*
 * Tool Talk Utility - tt_partition_redirection_map.cc
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 * Defines a partition redirection map.  A global version
 * of this object is in declared db_server_svc.cc.
 */

#include "tt_db_partition_redirection_map.h"
#include "util/tt_string_map.h"
#include "util/tt_map_entry.h"
#include "util/tt_path.h"

const char *pr_map_file = "partition_map";
const char *pr_map_env  = "_SUN_TT_PARTITION_MAP";
const char *pr_cde_map_env  = "TT_PARTITION_MAP";

_Tt_db_partition_redirection_map::
_Tt_db_partition_redirection_map ()
{
	map = new _Tt_string_map((_Tt_object_table_keyfn)
				 &_Tt_map_entry::getPathAddress);
}

_Tt_db_partition_redirection_map::
~_Tt_db_partition_redirection_map ()
{
}

_Tt_string _Tt_db_partition_redirection_map::
findEntry(const _Tt_string &address)
{
	return map->findEntry(address);
}

void _Tt_db_partition_redirection_map::refresh ()
{
	// give preference to the cde named setting
	const char* pr_map_env_to_use = (getenv(pr_cde_map_env) ? pr_cde_map_env : pr_map_env);

	_Tt_string path = _tt_user_path (pr_map_file, pr_map_env_to_use, TRUE);
	if (path.len()) {
		map->loadFile(path);
	}
}
