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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
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
# if defined(sun)
	struct xdr_ops ops;
# else
	struct XDR::xdr_ops ops;
# endif
#endif
};

#endif				/*  TT_XDR_UTILS_H */
