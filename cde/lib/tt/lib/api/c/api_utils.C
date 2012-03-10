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
