//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $XConsortium: mp_context.C /main/3 1995/10/23 10:22:37 rswiston $ 			 				
/*
 *
 * @(#)mp_context.C	1.5 30 Jul 1993
 *
 * Tool Talk Message Passer (MP) - mp_context.cc
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 *
 * Implementation of the _Tt_context: a context slotname.
 */

#include <ctype.h>
#include <mp/mp_context.h>

_Tt_context::
_Tt_context()
{
}

_Tt_context::
_Tt_context(const _Tt_context &c)
{
	_slotName = c._slotName;
}

_Tt_context::
~_Tt_context()
{
}

Tt_status _Tt_context::
setName(
	const char *slotname
)
{
	if (slotname == 0) {
		return TT_ERR_SLOTNAME;
	}
	const char *pc = slotname;
	if (*pc == '$') {
		pc++;
	}
	while (*pc != 0) {
		if ((! isalnum(*pc)) && (*pc != '_')) {
			return TT_ERR_SLOTNAME;
		}
		pc++;
	}
	_slotName = slotname;
	return TT_OK;
}

void _Tt_context::
print(
	const _Tt_ostream &os
) const
{
	_slotName.print( os );
}

bool_t _Tt_context::
xdr(
	XDR *xdrs
)
{
	return _slotName.xdr( xdrs );
}
