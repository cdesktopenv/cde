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
