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
//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $TOG: tt_host.C /main/9 1999/10/14 18:41:36 mgreess $ 			 				
/*
 *
 * @(#)tt_host.C	1.12 93/09/07
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */

#include <sys/types.h>
#include <util/tt_host.h>
#include <sys/param.h>

#if defined(_AIX)
/* AIX's arpa/inet.h has a buggy declaration of inet_addr */
extern "C" in_addr_t inet_addr(const char *);
#endif

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#ifdef __osf__
#include <unistd.h>
#else
#if !defined(USL) && !defined(__uxp__) && !defined(linux) && !defined(CSRG_BASED) && !defined(sun)
#include <osfcn.h>
#endif
#endif /* __osf */

#define X_INCLUDE_NETDB_H
#define XOS_USE_XT_LOCKING
#include <X11/Xos_r.h>

#include "util/tt_port.h"   

#if defined(OPT_BUG_SUNOS_4) && defined(__GNUG__)
	extern "C" { int endhostent(); }
#endif

#define		IP_QUALIFIER	"tcp"
#define		TLI_QUALIFIER	"tli"



/* 
 * Null constructor.
 */
_Tt_host::
_Tt_host()
{
	_addr = (char *)0;
	_name = (char *)0;
	_netname = (char *)0;
}


/* 
 * Destructor. Sets _name to NULL so that reference counting can free up the
 * space if possible.
 */
_Tt_host::
~_Tt_host()
{
	_addr = (char *)0;
	_name = (char *)0;
	_netname = (char *)0;
}


/* 
 * Initializes a _Tt_host object from a hostent structure which is the
 * standard structure returned by gethostbyaddr and gethostbyname.
 */
int _Tt_host::
init_from_hostent(hostent *h)
{
	char		str[40];
	char		*ip_addr;

	if (h != (hostent *)0) {
		_name = h->h_name;
		_addr.set((const unsigned char *)h->h_addr, h->h_length);
		ip_addr = (char *)_addr;
		sprintf(str, "%d.%d.%d.%d",
		       (int)((unsigned char *)ip_addr)[0],
		       (int)((unsigned char *)ip_addr)[1],
		       (int)((unsigned char *)ip_addr)[2],
		       (int)((unsigned char *)ip_addr)[3]);
		_string_address = str;
		// We use the ip address but we qualify it with the
		// keyword IP_QUALIFIER so that SVR4 clients can
		// interoperate with this name.

		_name = h->h_name;
		_netname = IP_QUALIFIER;
		_netname = _netname.cat(":").cat(_string_address);
		return(1);
	} else {
		return(0);
	}
}


/* 
 * Initializes a _Tt_host object from an IP address.
 * 
 * --> This method should attempt to cache results as much as possible to
 * avoid repeated calls to gethostbyaddr (which may, in the abscence of
 * DNS trigger YP lookups). (and in the presence of DNS trigger DNS
 * lookups which can be just as bad...)
 */
int _Tt_host::
init_byaddr(_Tt_string addr)
{
  	int result;
	struct hostent		*addr_ret;
	_Xgethostbynameparams	addr_buf;

	memset((char*) &addr_buf, 0, sizeof(_Xgethostbynameparams));
  	addr_ret = _XGethostbyaddr((char *)addr, 4, AF_INET, addr_buf);
	result = init_from_hostent(addr_ret);
#ifdef OPT_BUG_SUNOS_5
	// gethostbyaddr is effectively built on gethostent.  If
	// endhostent is not called, storage is left around to save the
	// name service connection, etc.  bug 1111175
	endhostent();
#endif
	return result;
}


/* 
 * Initializes a _Tt_host object from a string representing the host
 * address in Internet '.' notation.
 */
int _Tt_host::
init_bystringaddr(_Tt_string addr)
{
	
#ifdef __osf__
        unsigned int    *ip_address;
        unsigned int    ip_address_buf;
#else
	unsigned long	*ip_address;
	unsigned long 	ip_address_buf;
#endif
	struct hostent		*addr_ret;
	_Xgethostbynameparams	addr_buf;

	memset((char*) &addr_buf, 0, sizeof(_Xgethostbynameparams));
	ip_address = &ip_address_buf;

	*ip_address = inet_addr((char *)addr);
	if (*ip_address == INADDR_NONE) {
		return(0);
	}

	_addr.set((const unsigned char *)ip_address, 4);
	addr_ret = _XGethostbyaddr((char *)_addr, 4, AF_INET, addr_buf);
	if (! init_from_hostent(addr_ret)) {
		// given an ip address we can still communicate with
		// this host but we may not know it's name
		_string_address = addr;
		_netname = IP_QUALIFIER;
		_netname = _netname.cat(":").cat(_string_address);
		// we couldn't find out the name of the host so we'll
		// just call it by it's ip address
		_name = addr;
	}
#ifdef OPT_BUG_SUNOS_5
	// gethostbyaddr is effectively built on gethostent.  If
	// endhostent is not called, storage is left around to save the
	// name service connection, etc.  bug 1111175
	endhostent();
#endif
	return(1);
}

/* 
 * Initializes a _Tt_host object from a host name.
 * 
 */
int _Tt_host::
init_byname(_Tt_string name)
{
	_Tt_string	qual;
	_Tt_string	host;
	int		result;
	struct hostent		*host_ret;
	_Xgethostbynameparams	host_buf;

	memset((char*) &host_buf, 0, sizeof(_Xgethostbynameparams));
	if (name.len() == 0) {
		_name = _tt_gethostname();
	} else {
		qual = name.split(':',_name);
		if (_name.len()== 0) {
			// unqualified name
			_name = qual;
		} else {
			if (qual == IP_QUALIFIER) {
				return(init_bystringaddr(_name));
			} else if (qual == TLI_QUALIFIER) {
				/* just leave _name alone */;
			} else {
				// unknown qualifier!
				return(0);
			}
		}
	}
	host_ret = _XGethostbyname((char *)_name, host_buf);
	if (host_ret == NULL) {
		result = init_bystringaddr(_name);
	} else {
		result = init_from_hostent(host_ret);
	}
#ifdef OPT_BUG_SUNOS_5
	// gethostbyname is effectively built on gethostent.  If
	// endhostent is not called, storage is left around to save the
	// name service connection, etc.  bug 1111175
	endhostent();
#endif
	return result;
}


/* 
 * Returns the length of the host address.
 */
int _Tt_host::
addr_length() const
{
	return(_addr.len());
}


bool_t _Tt_host::
xdr(XDR *xdrs)
{
	return(_addr.xdr(xdrs));
}

_Tt_string
_tt_host_addr(_Tt_object_ptr &o)
{
	return(((_Tt_host *)o.c_pointer())->addr());
}


_Tt_string
_tt_host_name(_Tt_object_ptr &o)
{
	return(((_Tt_host *)o.c_pointer())->name());
}
