//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $TOG: api_error.C /main/6 1999/10/14 18:39:14 mgreess $ 			 				
/*
 *
 * api_error.cc
 *
 * Copyright (c) 1993 by Sun Microsystems, Inc.
 *
 *
 */

#include "api/c/api_error.h"

// Users can create a call with the same name as this, and through
// the magic of linking they can be notified when this call is
// invoked, having their function invoked instead.  This call will
// be invoked whenever an API call is about to return an error.
//
void tt_error(const char *, Tt_status)
{
}
