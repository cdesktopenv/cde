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

#if defined(ultrix)
#include <rpc/types.h>
#endif

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
