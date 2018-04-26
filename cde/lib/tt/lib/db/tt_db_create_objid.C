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
//%%  $XConsortium: tt_db_create_objid.C /main/3 1995/10/23 10:01:45 rswiston $ 			 				
/*
 * tt_db_create_objid.cc - Defines a routine for constructing an objid.
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */

#include "db/tt_db_create_objid.h"

_Tt_string _tt_db_create_objid (const _Tt_db_key_ptr &object_key,
				const _Tt_string     &file_system_type,
				const _Tt_string     &hostname,
				const _Tt_string     &partition)
{
  _Tt_string objid = object_key->string();
  objid = objid.cat(":").cat(file_system_type);
  objid = objid.cat(":").cat(hostname);
  objid = objid.cat(":").cat(partition);
  return objid;
}
