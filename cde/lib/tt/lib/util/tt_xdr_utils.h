/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: tt_xdr_utils.h /main/3 1995/10/23 10:47:19 rswiston $ 			 				 */
/*
 *
 * tt_xdr_utils.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */
#ifndef TT_XDR_UTILS_H
#define TT_XDR_UTILS_H
#include "util/tt_new.h"

unsigned long	_tt_xdr_sizeof(xdrproc_t f, void *data);

// _Tt_xdr_size_stream is always an automatic object, so it
// doesn't need to inherit from _Tt_object.  But inherit from
// _Tt_allocated to keep any references to the generic operator new and
// operator delete implementations in libC from creeping in.

class _Tt_xdr_size_stream : public _Tt_allocated {
      public:
	_Tt_xdr_size_stream();
	~_Tt_xdr_size_stream() {};
	operator 	XDR *();
	unsigned long 	getsize()	{return xdrstream.x_handy;};
	void 		reset()		{xdrstream.x_handy = 0;};
      private:
	XDR		xdrstream;
	long		buf[5];
#ifdef __DECCXX
        XDR::xdr_ops ops;
#else
	struct XDR::xdr_ops ops;
#endif
};

#endif				/*  TT_XDR_UTILS_H */
