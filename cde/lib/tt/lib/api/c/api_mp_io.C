//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $XConsortium: api_mp_io.C /main/3 1995/10/23 09:53:36 rswiston $ 			 				
/*
 *
 * api_mp_oi.cc
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 * Functions for changing mp objects like procids, oids, etc. to and from
 * character strings, for presentation at the API level
 */

#include "mp/mp_c.h"

//
// output procid as string
//
_Tt_string _Tt_c_procid::
api_out()
{
	return id();
}


Tt_status _Tt_c_procid::
api_in(_Tt_string &id)
{
	_id = id;
	return(TT_OK);
}
