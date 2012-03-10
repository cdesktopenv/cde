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
