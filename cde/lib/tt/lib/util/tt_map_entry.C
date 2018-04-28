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
//%%  $XConsortium: tt_map_entry.C /main/3 1995/10/23 10:41:23 rswiston $ 			 				
/* @(#)tt_map_entry.C	1.3 @(#)
 * Copyright (c) 1992, Sun Microsystems, Inc.
 *
 * String map class implementation.
 */

#include "tt_map_entry.h"

_Tt_string _Tt_map_entry::
getAddress (_Tt_object_ptr &entry)
{
      return (((_Tt_map_entry *)entry.c_pointer())->address);
}

_Tt_string _Tt_map_entry::
getPathAddress (_Tt_object_ptr &entry)
{
      _Tt_string real_path = _tt_realpath(((_Tt_map_entry *)
					   entry.c_pointer())->address);
      return real_path;
}
