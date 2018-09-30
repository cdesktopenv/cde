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
/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: tt_new_ptr.h /main/3 1995/10/23 10:42:10 rswiston $ 			 				 */
/*
 *
 * tt_new_ptr.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */
#if !defined(_TT_NEW_PTR_H)
#define _TT_NEW_PTR_H

#include "tt_options.h"
#if defined(OPT_BUG_SUNOS_4) && defined(__GNUG__)
#	include <stdlib.h>
#endif
#include <rpc/rpc.h>
#include "util/tt_generic.h"
#include "util/tt_new.h"

class _Tt_object;
class _Tt_new_ptr : public _Tt_allocated {
      protected:
	_Tt_object *sb;
      public:
	_Tt_new_ptr(_Tt_object *s);
	_Tt_new_ptr();
	_Tt_new_ptr(const _Tt_new_ptr &x);
	~_Tt_new_ptr() ;
	_Tt_new_ptr & operator=(const _Tt_new_ptr &x);
	_Tt_new_ptr & operator=(_Tt_object *x);
	bool_t	       xdr(XDR *xdrs,
			   _Tt_new_xdrfn xdrfn,
			   _Tt_object *(*make_new)());
};

#endif				// !_TT_NEW_PTR_H
