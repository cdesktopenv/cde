//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $XConsortium: tt_db_objid_to_key.C /main/3 1995/10/23 10:04:15 rswiston $ 			 				
/*
 * Tool Talk Utility - tt_db_objid_to_key.cc
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 * Object ID to object key conversion routine.
 *
 */

#include "db/tt_db_objid_to_key.h"

_Tt_string _tt_db_objid_to_key (const _Tt_string &objid)
{
  _Tt_string key;
  _Tt_string temp = objid;

  (void)temp.split(':', key);
  return key;
}
