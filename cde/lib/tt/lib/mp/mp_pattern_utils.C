//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $XConsortium: mp_pattern_utils.C /main/3 1995/10/23 10:27:06 rswiston $ 			 				
/*
 *
 * mp_pattern_utils.cc
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */
#include <mp/mp_pattern.h>

_Tt_patlist::
_Tt_patlist()
{
}

_Tt_patlist::
~_Tt_patlist()
{
}

_Tt_patlist::
_Tt_patlist(_Tt_string o)
{
	_op = o;
}

_Tt_string
_tt_patlist_op(_Tt_object_ptr &o)
{
	return(((_Tt_patlist *)o.c_pointer())->op());
}

implement_list_of(_Tt_pattern)
implement_list_of(_Tt_patlist)
implement_table_of(_Tt_patlist)
