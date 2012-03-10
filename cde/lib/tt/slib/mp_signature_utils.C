//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $XConsortium: mp_signature_utils.C /main/3 1995/10/23 12:01:47 rswiston $ 			 				
/*
 *
 * mp_signature_utils.cc
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */
#include "mp_signature.h"

_Tt_sigs_by_op::
_Tt_sigs_by_op()
{
}

_Tt_sigs_by_op::
~_Tt_sigs_by_op()
{
}

_Tt_sigs_by_op::
_Tt_sigs_by_op(_Tt_string o)
{
	_op = o;
}

_Tt_string
_tt_sigs_by_op_op(_Tt_object_ptr &o)
{
	return(((_Tt_sigs_by_op *)o.c_pointer())->op());
}

implement_list_of(_Tt_signature)
implement_list_of(_Tt_sigs_by_op)
implement_table_of(_Tt_sigs_by_op)
