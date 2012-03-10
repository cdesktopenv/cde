/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: tt_object.h /main/3 1995/10/23 10:42:31 rswiston $ 			 				 */
/* @(#)tt_object.h	1.19 93/07/30
 *
 * tt_object.h
 *
 * Copyright (c) 1990, 1992 by Sun Microsystems, Inc.
 */
#ifndef _TT_OBJECT_H
#define _TT_OBJECT_H
#include "util/tt_ptr.h"
#if defined(ultrix)
#include <rpc/types.h>
#endif

declare_ptr_to(_Tt_object)

class _Tt_ostream;

//
// _Tt_object should instead be a public virtual _Tt_allocated,
// but C++ is not smart enough to realize that sizeof _Tt_allocated is
// zero, and so _Tt_objects do not need a pointer to them.
//
class _Tt_object: public _Tt_allocated {
	friend class _Tt_new_ptr;
      public:
	_Tt_object() { _refcount_ = 0;};
	virtual ~_Tt_object();
	static _Tt_object_ptr &null_ptr();
	bool_t	       xdr(XDR *xdrs);
	int		verify_refcount(int maxref);
      private:
	int	_refcount_;
};
typedef void (*_Tt_object_printfn)(const _Tt_ostream &os,
				   const _Tt_object *obj);

#endif				/* _TT_OBJECT_H */
