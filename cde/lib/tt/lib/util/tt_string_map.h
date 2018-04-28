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
/*%%  $XConsortium: tt_string_map.h /main/3 1995/10/23 10:45:24 rswiston $ 			 				 */
/* @(#)tt_string_map.h	1.4 @(#)
 * Tool Talk Utility - tt_string_map.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 * Declares a class for holding string maps.  This
 * class is used for 2 different things at this time:
 *
 *	_Tt_db_hostname_redirection_map
 *	_Tt_db_partition_redirection_map
 */

#ifndef _TT_STRING_MAP_H
#define _TT_STRING_MAP_H

#include "util/tt_map_entry_utils.h"
#include "util/tt_object.h"
#include "util/tt_string.h"
#include "util/tt_table.h"

class _Tt_string_map : public _Tt_object {
public:
  _Tt_string_map (_Tt_object_table_keyfn key_function);
  _Tt_string_map ();

  ~_Tt_string_map ();

  void loadFile (const _Tt_string &file);
  
  _Tt_string findEntry(const _Tt_string &address);

private:
  _Tt_map_entry_table_ptr mapEntries;
};

#endif /* _TT_STRING_MAP_H */
