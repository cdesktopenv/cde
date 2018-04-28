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
//%%  $XConsortium: api_utils.C /main/3 1995/10/23 09:55:42 rswiston $ 			 				
/*
 *
 * api_utils.cc
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */
#include "util/tt_object.h"
#include "util/tt_list.h"
#include "mp/mp_c.h"
#include "api/c/api_api.h"
#include "api/c/api_mp.h"
#include "api/c/api_filemap.h"
#include "api/c/api_handle.h"
#include "api/c/api_storage.h"
#include "api/c/api_objid_spec.h"

implement_list_of(_Tt_api_handle)
implement_list_of(_Tt_api_userdata)
implement_list_of(_Tt_api_callback)
implement_list_of(_Tt_api_stg_stack_elm)
implement_ptr_to(_Tt_api_stg_stack)
implement_derived_ptr_to(_Tt_objid_spec, _Tt_db_object)
implement_table_of(_Tt_objid_spec)
implement_ptr_to(_Tt_api_filename_map)
