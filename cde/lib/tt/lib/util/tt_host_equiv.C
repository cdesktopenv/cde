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
//%%  $TOG: tt_host_equiv.C /main/9 1999/10/14 18:41:50 mgreess $ 			 				
/*
 *
 * @(#)tt_host_equiv.C
 *
 * Copyright (c) 1994 by Sun Microsystems, Inc.
 */

#include <util/tt_host_equiv.h>

#define X_INCLUDE_NETDB_H
#define XOS_USE_XT_LOCKING
#if defined(linux) || defined(CSRG_BASED)
#define index
#define rindex
#endif
#include <X11/Xos_r.h>
#if defined(linux) || defined(CSRG_BASED)
#undef index
#undef rindex
#endif
static int _cache_it(_Tt_hostname_cache_ptr, _Tt_string &);

// This null constructor is required by the _table_of macro:
// DO NOT use it!  Use the one below that that's a _Tt_string &.
_Tt_hostname_cache::
_Tt_hostname_cache()
{
	hostname = (_Tt_string) 0;
}

_Tt_hostname_cache::
_Tt_hostname_cache(_Tt_string & new_hostname)
{
	hostname = new_hostname;
	addr_list = new _Tt_string_list;
	addr_length = 0;
}

_Tt_hostname_cache::
~_Tt_hostname_cache()
{
}

_Tt_string _Tt_hostname_cache::
h_keyfn (_Tt_object_ptr & p)
{
	return ((_Tt_hostname_cache *)p.c_pointer())->hostname_val();
}

_Tt_host_equiv::
_Tt_host_equiv()
{
        _cache_table =
		new _Tt_hostname_cache_table((_Tt_object_table_keyfn) & _Tt_hostname_cache::h_keyfn);
}

_Tt_host_equiv::
~_Tt_host_equiv()
{
}


// return a string which uniquely identifies host,
// based on the contents of host and the supplied
// string localhost. The intent of this function
// is to allow one to get a name which IDs
// a useful minimum string of a host across
// multiple domains:
//	i.e. if you have A.Eng.Sun.COM,
// 	and localhost.Eng.Sun.COM, return A,
//	because you both are in the same domain.
//
//	However, if if you have B.Corp.Sun.COM and
//	localhost.Eng.Sun.COM return B.Corp
//
_Tt_string _Tt_host_equiv::
prefix_host(const _Tt_string & host, const _Tt_string & localhost)
{
	int i, j;
	_Tt_string h_prefix = host, l_prefix = localhost,
		   h, l, rpart_host, rpart_localhost;

#ifdef notdef
printf("DEBUG prefix_host: host = %s, localhost = %s\n",
	(char *) host, (char *) localhost);
#endif

	while (rpart_host == rpart_localhost) {
		h = h_prefix;
		l = l_prefix;

		if ((i = h.rindex('.')) == -1 ||
		    (j = l.rindex('.')) == -1) {
			break;
		}

		rpart_host = h.split(i, h_prefix);
		rpart_localhost = l.split(j, l_prefix);
	}

#ifdef notdef
printf("DEBUG prefix_host: returning %s\n", (char *) h);
#endif
	return h;
}

// Determine if two hostnames refer to the same host.
// This is required because a user may pass in a
// simple hostname or a hostname with a fully qualified
// domain name (FQDN) attached.
//
int _Tt_host_equiv::
hostname_equiv(const _Tt_string & host1, const _Tt_string & host2)
{
        // First try a simple comparison.
        if (host1 == host2) {
#ifdef notdef
printf("DEBUG hostname_equiv: host1 == host2, returning 1\n");
#endif
                return 1;
        }

        int	is_found = 0, long_count = 0, short_count = 0;
        _Tt_string shorthost, longhost;
        _Tt_string short_prefix, long_prefix;
        _Tt_string short_tmp, long_tmp;
 
        if (host1.len() > host2.len()) {
                shorthost = host2;
                longhost = host1;
        } else {
                // "longhost" may actually be equal to the
                // len of "shorthost".
                shorthost = host1;
                longhost = host2;
        }
 
        // Find how many .'s are in the names, in case
        // one or both of the names are an FQDN.
	long_tmp = longhost;
	short_tmp = shorthost;
        while (long_tmp.index('.') != -1) {  
                long_count++;
		long_tmp = long_tmp.split('.', long_prefix);
        }

        while (short_tmp.index('.') != -1) {  
                short_count++;
		short_tmp = short_tmp.split('.', short_prefix);
        }

	// loop until we determine short is a prefix of long, or
	// until we run out of things to compare.
	long_tmp = longhost;
	short_tmp = shorthost;
	for (is_found = 0;
	     short_count >= 0 && long_count > 0;
	     long_count--, short_count--) {

		// Get the chunk upto the next dot
		long_tmp = long_tmp.split('.', long_prefix);
		short_tmp = short_tmp.split('.', short_prefix);

		if (short_prefix.len() == 0) {
			// shorthost has no dots in it,
			// so do just one comparison.

			if (short_tmp != long_prefix) {
#ifdef notdef
printf("DEBUG hostname_equiv: short_tmp %s != long_prefix %s, returning 0\n",
	(char *) short_tmp, (char *) long_prefix);
#endif
				return 0;
			}
			is_found = 1;
			break;
		}

		if (short_prefix != long_prefix) {
#ifdef notdef
printf("DEBUG hostname_equiv: short_prefix %s != long_prefix %s, returning 0\n",
	(char *) short_prefix, (char *) long_prefix);
#endif
			return 0;
		}
	}

	if (is_found != 1) {
#ifdef notdef
printf("DEBUG hostname_equiv: is_found != 1, returning 0\n");
#endif
		return 0;	// no prefix was found
	}

	// One name is a prefix of the other.
        // Now do a name lookup on both strings,
        // get the IP addresses, and compare them
        // to determine if the names do truly
        // reference the same machine.


	// Lookup the both entries int the cache to see if we`ve
	// already mapped them.  If we haven't, map & cache them.
	//
	_Tt_hostname_cache_ptr sh, lh;

	sh = _cache_table->lookup(shorthost);
	if (sh.is_null()) {
		sh = new _Tt_hostname_cache(shorthost);

		if (_cache_it(sh, shorthost) == 1) {
			_cache_table->insert(sh);
		} else {
#ifdef notdef
printf("DEBUG hostname_equiv: sh.is_null(): returning 0\n");
#endif
			return 0;
		}
	}

	lh = _cache_table->lookup(longhost);
	if (lh.is_null()) {
		lh = new _Tt_hostname_cache(longhost);

		if (_cache_it(lh, longhost) == 1) {
			_cache_table->insert(lh);
		} else {
#ifdef notdef
printf("DEBUG hostname_equiv: lh.is_null(): returning 0\n");
#endif
			return 0;
		}
	}

	// Now sort through the address list looking
	// for a match.

	// This will probably never happen but why
	// not be careful?
	if (sh->addr_length != lh->addr_length) {
#ifdef notdef
printf("DEBUG hostname_equiv: sh->addr_length %d != h->addr_length %d, returning 0\n",
	sh->addr_length, lh->addr_length);
#endif
		return 0;
	}

	_Tt_string_list_cursor sh_c(sh->addr_list);
	_Tt_string_list_cursor lh_c(lh->addr_list);

        while (sh_c.next()) {
        	while (lh_c.next()) {
#ifdef notdef
printf("DEBUG hostname_equiv: checking address at 0x%x w/ 0x%x\n", 
	(char *) *sh_c, (char *) *lh_c);
#endif
			// Do the IP addresses match?
			if (memcmp((char *) *sh_c, (char *) *lh_c,
				    sh->addr_length) == 0) {
				// success!
#ifdef notdef
printf("DEBUG hostname_equiv: found match!\n");
#endif
				return 1;
			}
                }
        }

	// failure
        return 0;
}


// This is done here instead of in the constructor because
// gethostbyname can fail  and I don't want things that can fail
// to be in a constructor.
static int
_cache_it(_Tt_hostname_cache_ptr cache_ptr, _Tt_string & hostname)
{
        _Tt_hostname_cache_ptr	sh, lh;
	struct hostent	       *host_ret;
	_Xgethostbynameparams	host_buf;
	int			failed = 0;
	char			**h_addr_list;


	memset((char*) &host_buf, 0, sizeof(_Xgethostbynameparams));
	if ((host_ret = _XGethostbyname((char*) hostname, host_buf)) == NULL) {
		return 0;
	}

	// cache the parts of the entry we're
	// interested in.  We have to do this
	// since gethostbyname() returns a pointer
	// to static memory, and since struct hostent's
	// have pointers in them.

	cache_ptr->addr_length = host_ret->h_length;

	// Copy the list of IP addresses
	for (h_addr_list = host_ret->h_addr_list; 
	     h_addr_list; 
	     h_addr_list++) {

		if (! *h_addr_list) {
			break;	// no more addresses
		}

                // copy the address
		_Tt_string new_addr((const unsigned char *)h_addr_list,
				    host_ret->h_length);
		// cache it
		cache_ptr->addr_list->append(new_addr);
	}

	return 1;
}
