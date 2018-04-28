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
/*%%  $XConsortium: tt_xdr_version.h /main/3 1995/10/23 10:47:26 rswiston $ 			 				 */
/* @(#)tt_xdr_version.h	1.7 @(#)
 *
 * tt_xdr_version.h
 *
 * Copyright (c) 1990, 1992 by Sun Microsystems, Inc.
 */
#if !defined(_TT_XDR_VERSION_H)
#define _TT_XDR_VERSION_H
#include <util/tt_global_env.h>

/* 
 * XDR/RPC versioning scheme
 * 
 * For client/server applications using rpc, there are two general
 * approaches to versioning. The first approach which is potentially
 * cleaner if useable is to have different servers for each version of
 * the protocol they support.  Clients will contact the version of server
 * they know how to talk and everything works. The second approach is to
 * have a single server that registers itself under as many versions as
 * it supports. This multi-version server then deals with rpc requests
 * from clients registering under different versions by using the rq_vers
 * field in the svc_req structure that the rpc system hands the handler
 * routine. Note that both version schemes assume that the version of rpc
 * and xdr are the same. In other words, if different xdr formats are
 * desired then those should be part of the rpc protocol version. For
 * tooltalk there is the complication that whenever information has to be
 * written out to a database version 1 xdr formats must be used. In other
 * words, tooltalk needs to distinguish between the version of an rpc
 * client making a request and the version it is using for xdr when
 * writing out information.
 * 
 * For this reason there is a slightly different version scheme in use.
 * There is a version field in the _Tt_global object _tt_global (see
 * tt_global_env.cc). This version field is shared by both the rpc versioning
 * and xdr versioning. The way it is set is by using the special
 * _Tt_xdr_version object. The constructor for this object takes an
 * integer argument which is the version number to set the default to.
 * The destructor for the object resets the global version number to the
 * desired default (version 2 for 1.0.1). Thus if a method needs to use
 * version 1 xdr formats to write something out to a database it does the
 * following:
 * 
 *                  method_using_version_1()
 * 		 {
 * 			_Tt_xdr_version		xvers(1);
 * 
 * 				... body of method ...
 * 
 * 		 } // destructor for xvers will reset the
 * 		   // version number appropiately
 * 
 * This same mechanism is used for rpc requests. where the
 * _tt_service_rpc dispatch routine (see tt/lib/mp/mp_rpc_implement.cc)
 * sets the global version to the rq_vers field handed to us by the rpc
 * system. In this fashion the needs of both xdr and rpc versioning are
 * taken care of.
 */

class	_Tt_xdr_version : public _Tt_allocated {
      public:
	_Tt_xdr_version(int v) {
		// save the current default in _vers
		_vers = _tt_global->xdr_version();
		// set the default to the given version
		_tt_global->set_xdr_version(v);
	};
	~_Tt_xdr_version() {
		// reset to the default version when this object was
		// constructed. 
		_tt_global->set_xdr_version(_vers);
	}
      private:
	int	_vers;
};

#endif
