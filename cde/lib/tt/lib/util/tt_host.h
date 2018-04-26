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
/*%%  $XConsortium: tt_host.h /main/3 1995/10/23 10:39:40 rswiston $ 			 				 */
/*
 *
 * tt_host.h
 *
 * Copyright (c) 1990, 1994 by Sun Microsystems, Inc.
 *
 * This file implements the _Tt_host object which is a utility object
 * that handles locating a host address by name or locating a host name
 * by a host address. 
 */
#ifndef _TT_HOST_H
#define _TT_HOST_H

#include <netdb.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "util/tt_object.h"
#include "util/tt_string.h"

class _Tt_host : public _Tt_object {
      public:
	_Tt_host();
	virtual ~_Tt_host();
	int			init_byaddr(_Tt_string addr);
	int			init_byname(_Tt_string name);
	int			init_bystringaddr(_Tt_string addr);
	const _Tt_string &	name()	      const {return _name;};
	const _Tt_string &	netname()     const {return _netname;};
	const _Tt_string &	addr()	      const {return _addr;};
	int			addr_length() const;
	const _Tt_string &	stringaddr()  const {return _string_address;};
	bool_t			xdr(XDR *xdrs);		

      private:
	int			init_from_hostent(hostent *h);
	// Internet '.' notation
        _Tt_string              _string_address;
        _Tt_string              _name;
	//
	// IP_QUALIFIER:<ip_address>
	// TLI_QUALIFIER:<fully_qualified_hostname>
	//
        _Tt_string              _netname;
	_Tt_string		_addr;
};

_Tt_string _tt_host_addr(_Tt_object_ptr &o);
_Tt_string _tt_host_name(_Tt_object_ptr &o);
#endif				/* _TT_HOST_H */
