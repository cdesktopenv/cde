//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $XConsortium: tt_db_access_utils.C /main/3 1995/10/23 10:00:48 rswiston $ 			 				
/* @(#)tt_db_access_utils.C	1.5 @(#)
 * tt_db_access_utils.cc - Defines the _Tt_db_access utilities.
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */

#include "db/tt_db_access.h"
#include "db/tt_db_access_utils.h"

implement_ptr_to(_Tt_db_access)

// Normally, we don't put the constructors in the _utils.cc, but since
// tt_db_access.cc doesn\'t exist, it seems a waste to create it just
// to hold some null constructors.

_Tt_db_access::
_Tt_db_access()
{
      user = (uid_t)-1;
      group = (gid_t)-1;
      mode = (mode_t)-1; // Default: Everyone can read and write it
}

_Tt_db_access::
~_Tt_db_access()
{     
}     
