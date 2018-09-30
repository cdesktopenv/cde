/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $XConsortium: tt_int_rec.C /main/3 1995/10/23 10:40:21 rswiston $ 			 				
/*
 *
 * tt_int_rec.cc
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */
#include "util/tt_int_rec.h"
#include "util/tt_iostream.h"

_Tt_int_rec::
_Tt_int_rec()
{
	val = 0;
}


_Tt_int_rec::
_Tt_int_rec(int v)
{
	val = v;
}


_Tt_int_rec::
~_Tt_int_rec()
{
}



void _Tt_int_rec::
print(const _Tt_ostream &os) const
{
	os << val;
}


bool_t _Tt_int_rec::
xdr(XDR *xdrs)
{
	return(xdr_int(xdrs, &val));
}

implement_list_of(_Tt_int_rec)

_Tt_pid_t_rec::
_Tt_pid_t_rec()
{
	val = 0;
}


_Tt_pid_t_rec::
_Tt_pid_t_rec(pid_t v)
{
	val = v;
}


_Tt_pid_t_rec::
~_Tt_pid_t_rec()
{
}



void _Tt_pid_t_rec::
print(const _Tt_ostream &os) const
{
	os << val;
}


bool_t _Tt_pid_t_rec::
xdr(XDR *xdrs)
{
	long tv;
	bool_t result;

	// Some systems have pid_t an int, some have it a long, probably
	// some have it a short.  We XDR them all as longs.  I don't even
	// want to think about 64-bit pid_ts...

	if (xdrs->x_op == XDR_ENCODE) {
		tv = (long)val;
	}
	if (!(result = xdr_long(xdrs, &tv))) {
		return result;
	}
	if (xdrs->x_op == XDR_DECODE) {
		val = (pid_t)tv;
	}
	return result;
}

implement_list_of(_Tt_pid_t_rec)
