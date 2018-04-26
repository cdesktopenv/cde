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
//%%  $XConsortium: tt_new_ptr.C /main/3 1995/10/23 10:42:04 rswiston $ 			 				
/*
 *
 * tt_new_ptr.cc
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */
#include "util/tt_object.h"
#include "util/tt_new_ptr.h"
#include <stdio.h>
#include <stdlib.h>
     

//
// The methods for this object implement the basic reference-counting
// functionality for the generic _Tt_new_ptr class which is the parent
// class for all the generated pointer classes (see declare_ptr_to,
// and implement_ptr_to in util/tt_ptr.h). The basic duties of these
// methods are to decrement refcounts when a reference is removed to
// an object and call the object's destructor if the refcount goes to
// zero. If a reference is added then the refcount is incremented.
//

_Tt_new_ptr::
~_Tt_new_ptr()
{
	if (sb && !--sb->_refcount_) {
		delete sb;
	}
}

_Tt_new_ptr::
_Tt_new_ptr (_Tt_object *s)
{
     sb = s;
     if (sb) {
	sb->_refcount_++;
     }
}

_Tt_new_ptr::
_Tt_new_ptr ()
{
	sb = (_Tt_object *)0;
}

_Tt_new_ptr::
_Tt_new_ptr (const _Tt_new_ptr &x)
{
       sb = x.sb;
       if (sb) {
		sb->_refcount_++;
	}
}

_Tt_new_ptr& _Tt_new_ptr::
operator=(const _Tt_new_ptr &x)
{
	/* be sure to increment before decrement in case argument is the only
	 * pointer and it\'s being assigned to itself
	 */
	if (x.sb) {
		x.sb->_refcount_++;
	}
	if (sb && 0==--sb->_refcount_) {
		delete sb;
	}

	sb = x.sb;

	return *this;
}

_Tt_new_ptr& _Tt_new_ptr::
operator=(_Tt_object *x)
{
	if (x) {
		x->_refcount_++;
	}
	if (sb && 0==--sb->_refcount_) {
		delete sb;
	}
	sb = x;
	return *this;
}


//
// Generic xdr routine for all the pointer classes. The xdrfn function
// is used to invoke the appropiate xdr function (note that this is
// arranged at compile-time by the declare_ptr_to and implement_ptr_to
// macros). The make_new argument is a function that returns a new
// instance of the appropiate subclass (this is also arranged by the
// declare_ptr_to and implement_ptr_to macros).
//
// Another, more natural, way to implement this would be to just
// have the make_new function return a new instance of the appropiate
// subclass and then invoking the xdr method on the instance (which
// would mean that the xdr method would have to be virtual). This was
// not done here because of the negative impact on the library's data
// and relocation segment due to virtuals.
//
bool_t _Tt_new_ptr::
xdr(XDR *xdrs, _Tt_new_xdrfn xdrfn,  _Tt_object *(*make_new)())
{
	int		isnull;

	if (xdrs->x_op == XDR_ENCODE) {
		isnull = (sb == (_Tt_object *)0);
	}
	if (! xdr_int(xdrs, &isnull)) {
		return(0);
	}
	if (!isnull) {
		if (xdrs->x_op == XDR_DECODE) {
			if (sb == (_Tt_object *)0) {
				sb = (*make_new)();
				sb->_refcount_++;
			}
		}

		return((*xdrfn)(xdrs, sb));
	} else {
                if (sb && !--sb->_refcount_) {
			delete sb;
		}
                sb = (_Tt_object *)0;
		return(1);
	}
}
