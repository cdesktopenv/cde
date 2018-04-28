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
/*%%  $XConsortium: tt_db_partition_redirection_map.h /main/3 1995/10/20 16:42:49 rswiston $ 			 				 */
/*
 * Tool Talk Utility - tt_partition_redirection_map.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 * Declares a partition redirection map.  A global version
 * of this object is declared in db_server_svc.cc.
 */

#ifndef _TT_PARTITION_REDIRECTION_MAP_H
#define _TT_PARTITION_REDIRECTION_MAP_H

#include "util/tt_new.h"
#include "util/tt_string_map_utils.h"

class _Tt_db_partition_redirection_map : public _Tt_allocated {
public:
  _Tt_db_partition_redirection_map ();
  ~_Tt_db_partition_redirection_map ();
  void refresh ();
  _Tt_string findEntry(const _Tt_string &address);
private:
  _Tt_string_map_ptr map;
};

#endif /* _TT_PARTITION_REDIRECTION_MAP_H */
