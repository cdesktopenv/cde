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
//%%  $XConsortium: tt_object.C /main/3 1995/10/23 10:42:24 rswiston $ 			 				
/* @(#)tt_object.C	1.13 93/07/30
 *
 * tt_object.cc
 *
 * Copyright (c) 1991, 1992 by Sun Microsystems, Inc.
 * 
 * This is the tt_object class.
 */

#include <stdlib.h>
#include "util/tt_object.h"
#include "util/tt_port.h"

static _Tt_object_ptr *_tt_object_nil = 0;

_Tt_object::
~_Tt_object()
{
}

// Occasionally a routine needs to return a reference to a _ptr class instance
// with a null pointer.  We have just one of those as a static member of
// _Tt_object.  Since we can\'t use any static initialized class instances,
// we do the usual trick of making it a pointer and allocating the
// real instance with new on the first reference.

_Tt_object_ptr &_Tt_object::
null_ptr()
{
	if (!_tt_object_nil) {
		_tt_object_nil
		= new _Tt_object_ptr((_Tt_object *)0);
	}
	return *_tt_object_nil;
}


// Not every class that inherits from _Tt_object provides an xdr method,
// since not every class needs one.  However, the ptr classes always
// declare an xdr method which refers to the pointed-to classes\' xdr
// method.  For classes that don\'t declare their own xdr method, this
// generates a reference to _Tt_object::xdr, so we supply the following
// definition to satisfy them. If this ever gets called it is a gross
// design error (or a sign that an xdr method needs to be provided...)
bool_t _Tt_object::
xdr(XDR * /* xdrs */)
{
	_tt_syslog( 0, LOG_ERR, "_Tt_object::xdr()");
	abort();
	return 0;
}

// We don\'t want to make _refcount_ public, but there are occasional
// uses in debugging to just check that the refcount on an object
// is reasonable: greater than zero but not ridiculously large.
// This method checks the refcount and returns -1 if the refcount
// is too small (<=0), +1 if the refcount is too large (greater than
// the parameter supplied) or 0 if the refcount is just right.
int _Tt_object::
verify_refcount(int maxref)
{
	if (_refcount_<=0) return -1;
	if (_refcount_>maxref) return +1;
	return 0;
}

