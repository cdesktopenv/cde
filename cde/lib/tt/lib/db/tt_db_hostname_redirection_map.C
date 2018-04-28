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
//%%  $XConsortium: tt_db_hostname_redirection_map.C /main/3 1995/10/23 10:02:44 rswiston $ 			 				
/*
 * Tool Talk Utility - tt_hostname_redirection_map.cc
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 * Defines a hostname redirection map.  A global version
 * of this object is in _tt_global.
 */

#include "db/tt_db_hostname_redirection_map.h"
#include "util/tt_path.h"

static const char hr_map_file[] = "hostname_map";
static const char hr_map_env[]  = "_SUN_TT_HOSTNAME_MAP";
static const char hr_cde_map_env[]  = "TT_HOSTNAME_MAP";

_Tt_db_hostname_redirection_map::
_Tt_db_hostname_redirection_map ()
{
      map = new _Tt_string_map((_Tt_object_table_keyfn)
			       &_Tt_map_entry::getAddress);
}

_Tt_db_hostname_redirection_map::
~_Tt_db_hostname_redirection_map ()
{
}

_Tt_string _Tt_db_hostname_redirection_map::
findEntry(const _Tt_string &address)
{
      return map->findEntry(address);
}

void _Tt_db_hostname_redirection_map::
refresh ()
{
  // give preference to the cde named setting
  const char *hr_map_env_to_use = (getenv(hr_cde_map_env) ? hr_cde_map_env : hr_map_env);

  _Tt_string path = _tt_user_path (hr_map_file, hr_map_env_to_use, FALSE);
  if (path.len()>0) {
    map->loadFile(path);
  }
}
