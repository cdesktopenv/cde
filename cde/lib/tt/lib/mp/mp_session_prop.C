//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $XConsortium: mp_session_prop.C /main/3 1995/10/23 10:29:02 rswiston $ 			 				
/*
 *
 * mp_session_prop.cc
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */
#include "mp/mp_session_prop.h"
#include "util/tt_string.h"


_Tt_session_prop::
_Tt_session_prop()
{
}

_Tt_session_prop::
_Tt_session_prop(_Tt_string name, _Tt_string_list_ptr values)
{
	_name = name;
	_values = values;
}

_Tt_session_prop::
_Tt_session_prop(_Tt_string name, _Tt_string value)
{
	_name = name;
	_values = new _Tt_string_list();
	_values->append(value);
}

_Tt_session_prop::
~_Tt_session_prop()
{
}
