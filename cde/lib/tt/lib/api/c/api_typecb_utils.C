//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $TOG: api_typecb_utils.C /main/4 1999/10/14 18:40:24 mgreess $ 			 				
/* @(#)api_typecb_utils.C	1.2 93/07/30 SMI
 *
 * api_typecb_utils.cc
 *
 * Copyright (c) 1993 by Sun Microsystems, Inc.
 */
#include <stdio.h>
#include "api/c/api_typecb_utils.h"
#include "api/c/api_typecb.h"

class _Tt_typecb;
_Tt_string
_tt_typecb_key(_Tt_object_ptr &o)
{
	_Tt_string name;
	int num;
	name = ((_Tt_typecb *)o.c_pointer())->get_typename();
	num = ((_Tt_typecb *)o.c_pointer())->opnum();
	return _tt_typecb_key(name, num);
}

_Tt_string
_tt_typecb_key(const _Tt_string &tid, int opnum)
{
	return tid.cat("#").cat(opnum);
}


implement_list_of(_Tt_typecb)

implement_table_of(_Tt_typecb)
