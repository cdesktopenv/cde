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
//%%  $XConsortium: tt_string.C /main/5 1996/09/24 10:06:12 drk $ 			 				
/* @(#)tt_string.C	1.61 95/02/27
 *
 * tt_string.cc
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 * 
 * Implementation of _Tt_string class operations.
 * There are two classes involved: _Tt_string, which is just a pointer
 * to _Tt_string_buf, which includes ref counts, the length of the
 * string, and a pointer to the (malloc'ed) actual contents.
 * Though _Tt_string_buf is the actual "guts" of a string, clients
 * never refer to it, but always use _Tt_string.  _Tt_string_buf
 * therefore has no public members.
 *
 * We always malloc one more byte than specified in the _Tt_string_buf
 * length member, to ensure that there's room for a terminating zero byte.
 * This means we can just hand back the _Tt_string_buf contents pointer
 * to clients wanting a char * for passing to other routines (e.g. printf.)
 * 
 * This also makes the internal length field the same as strlen.
 */

#include <string.h>
#include <memory.h>
#if defined(ultrix)
#include <rpc/types.h>
#endif
#include <stdlib.h>
#include <ctype.h>
#if defined(linux) || defined(CSRG_BASED)
#include <wctype.h>
#endif
#include "util/tt_string.h"
#include "util/tt_assert.h"
#include "util/tt_xdr_utils.h"
#include "util/tt_xdr_version.h"
#include "util/tt_iostream.h"
#include "util/tt_port.h"
#include "util/tt_global_env.h"

implement_list_of(_Tt_string_buf)

typedef bool_t (*local_xdrproc_t)(XDR *, caddr_t *);

//
// The plain constructor for _Tt_string_buf just creates a null
// pointer. (Not a null string, which would be a pointer to a zero byte.)
//
/* 
 * _Tt_string_buf::
 * _Tt_string_buf()
 * {
 * 	content = (char *)0;
 * 	length = 0;
 * }
 */
//
// The _Tt_string_buf copy constructor should not be used much, since
// generally pointers to _Tt_string_buf's are passed around.
//

_Tt_string_buf::
_Tt_string_buf(const _Tt_string_buf& s)
{
	content = (char *)malloc(s.length+1);
	length = s.length;
	memcpy(content,s.content,length+1);
}

//
// The _Tt_string_buf destructor frees the actual string storage.
//

_Tt_string_buf::
~_Tt_string_buf()
{
	if (content != (char *)0) {
		(void)free((MALLOCTYPE *)content);
	}
}

void _Tt_string_buf::
set(const unsigned char *s, int len)
{
	length = len;
	if ((s != (const unsigned char*) 0) && (len >= 0)) {
		content = (char *)malloc(len+1);
		memcpy(content,s,len);
		content[len] = '\0';
	} else {
		content = (char *)0;
	}
}

// a non-member function for use when you have a char *, but don't
// yet have a _Tt_string.
void
_tt_print_escaped_string(const _Tt_ostream &os, const char *s, int length,
			 int max_print_width, int quote_it)
{
	int i, count;

	static int width_inited = 0;
	static int _max_print_width;
	if (! width_inited) {
		width_inited = 1;
		_max_print_width = 40;
		char *s = _tt_get_first_set_env_var(2, "TT_ARG_TRACE_WIDTH", 
						       "_SUN_TT_ARG_TRACE_WIDTH");
		if (s != 0) {
			_max_print_width = atoi(s);
		}
	}
	switch (max_print_width) {
	    case _Tt_string_unlimited:
		max_print_width = 80000;
		break;
	    case _Tt_string_user_width:
		max_print_width = _max_print_width;
		break;
	}
	if (s == 0) {
		os << "(null)";
	} else {
		wchar_t wc;
		int n;

		if (quote_it) {
			os << '"';
		}

		for (count=0, i=0; count < max_print_width && i < length;) {

			if (length - i >= MB_LEN_MAX) {

				if ((n = mbtowc(&wc, &s[i], MB_LEN_MAX)) != -1) {
				// multi-byte character buffer. Interpret it appropriately.

					if (iswprint(wc) || iswspace(wc)) {
						count += (n == 0) ? 1 : n;

						// We do it this way rather than through
						// _Tt_iostream operator<<  method because
						// wchar_t is equivalent  to a long and the
						// compiler doesn't know which method to use.
						os.sprintf(MB_LEN_MAX, "%C", wc);
					} else {
						count += (3  + ((n == 0) ? 1 : n));

						// don't print past limit, even a little.
						if (count < max_print_width) {
							os.sprintf(10, "\\%03C", wc);
						}
					}

					i += (n == 0) ? 1 : n;

					continue;
				}
			}

			// non-multi-byte character buffer...
			if (isprint((unsigned char) s[i]) ||
			    isspace((unsigned char) s[i])) {
				count += 1;
				os << s[i];
			} else {
				count += 3;

				// don't print past limit, even a little.
				if (count < max_print_width) {
					os.sprintf(10, "\\%03o", (unsigned char) s[i]);
				}
			}
			i++;
		}

		if (quote_it) {
			os << '"';
		}

		if (i < length) {
			os << "[...]";
		}
	}
}


// print on a string is mainly for debugging. So we limit the number
// of characters printed, and print escaped octal (\ddd) for nongraphic
// characters.
void _Tt_string_buf::
print(const _Tt_ostream &os, int max_print_width, int quote_it) const
{
	_tt_print_escaped_string(os, content, length, max_print_width,
			 quote_it);
}

bool_t _Tt_string_buf::
xdr(XDR *xdrs)
{
	if (_tt_global->xdr_version() == 1) {
		return(xdr_1(xdrs));
	} else {
		return(xdr_2(xdrs));
	}
}


bool_t _Tt_string_buf::
xdr_1(XDR *xdrs)
{
	char	*sp;
	u_int	u_length;

	if (xdr_int(xdrs, &length)) {
		if (length > 0) {
			if (xdrs->x_op == XDR_DECODE) {
				sp =  (char *)malloc(length + 1);
			} else {
				sp = content;
				u_length = length;
			}
			if (xdr_bytes(xdrs, &sp, &u_length, length)) {
				if (xdrs->x_op == XDR_DECODE) {
					if (content != (char *)0) {
						(void)free((MALLOCTYPE *)content);
					}
					content = sp;
					content[length] = '\0';
				}
			} else {
				// couldn't xdr content field
				return 0;
			}
		}
		return 1;
	} else {
		// couldn't xdr length field
		return 0;
	}
}


bool_t _Tt_string_buf::
xdr_2(XDR *xdrs)
{
	char	*sp;
	int	 len = length;

	if (content == (char *)0) len = -1;

	if (xdr_int(xdrs, &len)) {
		// if a regular string...
		if (len > 0) {
			if (xdrs->x_op == XDR_DECODE) {
				sp = (char *)malloc(len + 1);
			} else {
				sp = content;
			}

			if (xdr_opaque(xdrs, (caddr_t)sp, len)) {
				if (xdrs->x_op == XDR_DECODE) {
					if (content != (char *)0) {
						(void)free((MALLOCTYPE *)content);
					}
					content = sp;
					content[len] = '\0';
					length = len;
				}
			} else {
				// couldn't xdr content field
				return 0;
			}
		}
		// if an empty string ("")...
		else if (len == 0) {
			if (xdrs->x_op == XDR_DECODE) {
				if (content != (char *)0) {
					(void)free((MALLOCTYPE *)content);
				}
				content = (char *)malloc(1);
				*content = '\0';
				length = 0;
			}
		}
		// if a NULL string...
		else if (len == -1) {
			if (xdrs->x_op == XDR_DECODE) {
				if (content != (char *)0) {
					(void)free((MALLOCTYPE *)content);
				}
				content = (char *)0;
				length = 0;
			}
		}

		return 1;
	} else {
		// couldn't xdr length field
		return 0;
	}
}


//
// The plain _Tt_string constructor creates a null string.
//

_Tt_string::
_Tt_string()
{
	if (_tt_global != 0) {
		*(_Tt_string_buf_ptr *)this=_tt_global->universal_null_string;
	}
	else {
		*(_Tt_string_buf_ptr *)this = new _Tt_string_buf;
	}
}

//
// The _Tt_string copy constructor is sure to call the parent copy constructor
//
_Tt_string::
_Tt_string(const _Tt_string &s) : _Tt_string_buf_ptr(s)
{
}

//
//  A _Tt_string can be constructed from a "char *" C string.
//
_Tt_string::
_Tt_string(const char *s)
{
	*(_Tt_string_buf_ptr *)this = new _Tt_string_buf;
	if (s != (char *)0) {
		(*this)->length = strlen(s);
		(*this)->content = (char *)malloc((*this)->length+1);
		memcpy((*this)->content,s,(*this)->length);
		((*this)->content)[(*this)->length] = '\0';
	} else {
		(*this)->length = 0;
		(*this)->content = (char *)0;
	}
}

//
//  A special constructor builds a _Tt_string containing "n" bytes,
//  not initialized to anything.  
//
_Tt_string::
_Tt_string(int n)
{
	// Trying to create a string of negative length is arguably
	// a fatal error, but it's really obnoxious to have
	// constructors fail.  So if we get a negative argument,
	// just build a null string.

	if (n<0) {
		n = 0;
	}
	
	*(_Tt_string_buf_ptr *)this = new _Tt_string_buf;
	(*this)->length = n;
	(*this)->content = (char *)malloc(n+1);
	(*this)->content[n] = '\0';
}

//
// A _Tt_string can be constructed from a bytestring.
//
_Tt_string::
_Tt_string(const unsigned char *s, int len)
{
	*(_Tt_string_buf_ptr *)this = new _Tt_string_buf;
	(*(_Tt_string_buf_ptr *)this)->set(s, len);
}


_Tt_string::
~_Tt_string()
{
}

//
// Assigning from a char * string is supported, copying the string.
//
_Tt_string& _Tt_string::
operator=(const char * s)
{
	_Tt_string newstring(s);
	*(_Tt_string_buf_ptr *)this = newstring;
	return *this;
}

//
// "set" is very like assignment, but takes a pointer and a string
// to handle bytestrings.
//
_Tt_string& _Tt_string::
set(const unsigned char * s, int n)
{
	_Tt_string newstring(s,n);
	*(_Tt_string_buf_ptr *)this = newstring;
	return *this;
}

//
// Operator [] (subscripting) is overloaded to allow access to individual
// elements of the string.
//
const char& _Tt_string::
operator[](int offset) const
{
	ASSERT(0<=offset && offset<length,"String subscript out of range");
	return (*this)->content[offset];
}

char& _Tt_string::
operator[](int offset)
{
	return (*this)->content[offset];
}

//
// operator char * (conversion to char *) returns the
// base string.
//
_Tt_string::
operator char*() const
{
	return (*this)->content;
}

//
// operator const char * (conversion to const char *) returns the
// base string as a const.
//
_Tt_string::
operator const char*() const
{
	return (const char *) ((*this)->content);
}

//
// left(n) returns a _Tt_string containing the first n characters
// of the string, or the string if there aren't that many.
//
_Tt_string _Tt_string::
left(int l) const
{
	return mid(0,l);
}

//
// right(n) returns a _Tt_string containing the last n characters of the
// string, or the string if there aren't that many.
//
_Tt_string _Tt_string::
right(int l) const
{
	return mid(len()-l,l);
}

//
// mid(o,l) returns a _Tt_string containing the l characters of the
// string starting at position o (zero-based.)
//
_Tt_string _Tt_string::
mid(int o, int l) const
{
	ASSERT(o>=0 && l>=0,"Invalid argument");
	if (o+l>len()) {
		l = len() - o;
	}

	_Tt_string result(l);

	for (int i=0;i<l;++i) {
		result[i] = (*this)[i+o];
	}
	return result;
}

_Tt_string _Tt_string::
split(int i, _Tt_string &prefix) const
{
	if (i < 0) {
		prefix = 0;
		return *this;
	} else {
		prefix = mid(0,i);
		return mid(i+1,len()-i-1);
	}
}

//
// split(c,prefix) finds the first occurrence of character c in
// this string.  All characters up to (but not including) c are
// returned as parameter "prefix"; the rest of the string (after c,
// but not including c) are returned as the function result.
// If the character does not include character c, prefix is returned
// as null and this string is returned as the result.
//
_Tt_string _Tt_string::
split(char c, _Tt_string &prefix) const
{
	return split( index(c), prefix );
}

//
// Ditto, for string s instead of char c
//
_Tt_string _Tt_string::
split(const char *s, _Tt_string &prefix) const
{
	int i = index(s);
	if (i<0) {
		prefix = "";
		return *this;
	}
	prefix = left(i);
	return right(len()-i-strlen(s));
}

//
// rsplit() is like split(), but it finds the _last_ occurrence of c
//
_Tt_string _Tt_string::
rsplit(char c, _Tt_string &prefix) const
{
	return split( rindex(c), prefix );
}

//
// quote_nulls() returns a _Tt_string containing this string, with all
// embedded nulls replaced with "\0", and all "\" replaced
// with "\\".  This gives a string which can be safely treated as
// a "C" string, and which can be turned back into the original
// form with unquote_nulls().  The 
//
_Tt_string _Tt_string::
quote_nulls() const
{
	int l = len();
	char *result = (char *)malloc(2*l+1); // string will get at most 2x bigger
	char *p = result;
	_Tt_string r;


	for (int i=0;i<l;++i) {
		int c = (*this)[i];
		if (c == '\0') {
			*p++ = '\\';
			*p++ = '0';
		} else if (c == '\\') {
			*p++ = '\\';
			*p++ = '\\';
		} else {
			*p++ = c;
		}
	}
	*p++ = '\0';

	r = result;		// Put the value in a _Tt_string
	(void)free((MALLOCTYPE *)result);

	return r;
}
//
// unquote_nulls() returns a _Tt_string containing this string, with all
// backslash-quoted characters replaced by their equivalents, in particular
// \0 is replaced by a null.
//
_Tt_string _Tt_string::
unquote_nulls() const
{
	int l;
	unsigned char *result;
	_Tt_string r;
	register unsigned char *p;
	register unsigned char *q;

	l = len();
	if (l==0) {
		r = "";
		return r;
	}
	result = (unsigned char *)malloc(l); // string will not grow

	p = (unsigned char *)((*this)->content);
        q = result;

	while (*p) {
		if (*p == '\\') {
			++p;
			ASSERT(*p,"Badly formed quote_null string");
			if (*p == '0') {
				*q++ = '\0';
			} else {
				*q++ = *p;
			}
		} else {
			*q++ = *p;
		}
		++p;
	}

	r.set(result,q-result);
	(void)free((MALLOCTYPE *)result);
	return r;
}

//
// cat(s) returns a _Tt_string which is the concatenation of the
// object string and the argument string.
//
_Tt_string _Tt_string::
cat(const _Tt_string &s) const
{
	_Tt_string	result(len()+s.len());
	int		result_len = len() + s.len();

	memcpy(result->content,(*this)->content,len());
	memcpy(result->content+len(),s->content,s.len());
	(result->content)[result_len] = '\0';
	return result;
}

_Tt_string _Tt_string::
cat(char c) const
{
	_Tt_string	result(len()+1);
	int		result_len = len() + 1;
	memcpy(result->content,(*this)->content,len());
	(result->content)[result_len-1] = c;
	(result->content)[result_len] = '\0';
	return result;
}

_Tt_string _Tt_string::
cat(int i) const
{
	return cat((long) i);
}

_Tt_string _Tt_string::
cat(unsigned int i) const
{
	return cat((unsigned long) i);
}

_Tt_string _Tt_string::
cat(long i) const
{
	char		buf[32];

	sprintf(buf, "%ld", i);
	return cat( buf );
}

_Tt_string _Tt_string::
cat(unsigned long i) const
{
	char		buf[32];
	sprintf(buf, "%lu", i);
	return cat( buf );
}

//
// Return a new string, with each instance of old in this string replaced
// by with.  No rescan, so replacing "x" with "xx" does not loop.
//
_Tt_string _Tt_string::
replace(const char *old, const _Tt_string &with) const
{
	_Tt_string result;
	_Tt_string prefix;
	_Tt_string remainder = *this;
	int i;
	int oldlen = strlen(old);

	if (0==oldlen) {
		// arguably replacing the null string ought to insert
		// once between every old character and at beginning and
		// end.  But we don't bother.
		return (*this);
	}
	
	do {
		i = remainder.index(old);
		if (i<0) {
			// no more matches found
			result = result.cat(remainder);
			remainder = "";
		} else {
			// Match found, replace it
			result = result.cat(remainder.left(i));
			result = result.cat(with);
			remainder = remainder.right(remainder.len()-i-oldlen);
		}
	} while (remainder.len() > 0);
	return result;
}

//
// len returns the length (strlen) of the object string
//
/* int _Tt_string::
 * len() const
 * {
 * 	return (*this)->length;
 * }
 */
// private cmp() compares and returns negative, 0, positive for object 
// less, equal, greater than arg.
// sadly, strcmp() can't be used here since we allow embedded
// nulls, and bcmp() only reports equal/not equal.
int _Tt_string::
cmp(const char *q, int qlen) const
{
	register char *p = (*this)->content;
	register int plen = len();
	register int pcmp;

	if (0==p && 0==q) return 0;
	if (0==p) return -1;
	if (0==q) return 1;
	if (qlen == -1) {
		qlen = strlen(q);
	}
	pcmp = memcmp(p, q, (plen < qlen) ? plen : qlen);
	if (pcmp == 0 && plen > qlen) {
		return(1);
	} else if (pcmp == 0 && plen < qlen) {
		return (-1);
	} else {	
		return(pcmp);
	}
}

/* XXX: old definition of cmp
 * {	
 * 
 * 	register char *p = (*this)->content;
 * 	register char *end;
 * 	register int plen = len();
 * 	
 * 	// null pointers are treated as null strings, ignoring length.
 * 	if (0==p && 0==q) return 0;
 * 	if (0==p) return -1;
 * 	if (0==q) return 1;
 * 
 * 	if (qlen == -1) {	// really comparing two _Tt_strings
 * 		qlen = strlen(q);
 * 	}
 * 	if (plen<qlen) {
 * 		end = p+plen;
 * 	} else {
 * 		end = p+qlen;
 * 	}
 * 	
 * 	while (p<end) {
 * 		if (*p<*q) return -1;
 * 		if (*p>*q) return +1;
 * 		++p; ++q;
 * 	}
 * 
 * 	// all the chars that are there compare, so the shorter string
 * 	// is the lesser.
 * 
 * 	if (plen<qlen) return -1;
 * 	if (plen>qlen) return +1;
 * 	return 0;
 * }
 */


//
// index(c) returns the offset of the first occurence of char c within
// the object string.  Again, we can't use the system index() function.
// -1 is returned if the character doesn't exist in the object string.
//
int _Tt_string::
index(char c) const
{
	if (len() == 0) {
		return -1;
	}

	register char *p = (*this)->content;
	register char *end = p+len();
	
	while (p<end && *p!=c) ++p;
	if (p==end) {
		return -1;
	} else {
		return p-(*this)->content;
	}
}

//
// If you really must search for null-containing strings, feel free
// to change this to take a const _Tt_string &.
//
int _Tt_string::
index(const char *s) const
{
	if (s == 0 || len() == 0) {
		return -1;
	}

	char *p = (*this)->content;
	char *end = p+len();
	int slen = strlen(s);
	int found = 0;

	while ((! found) && (p < end - slen + 1)) {
		found = 1;
		for (int i = 0; i < slen; i++) {
			if (p[i] != s[i]) {
				found = 0;
				p++;
				break;
			}
		}
	}

	if (! found) {
		return -1;
	} else {
		return p-(*this)->content;
	}
}


//
// rindex(c) returns the offset of the last occurence of char c within
// the object string.  Again, we can't use the system index() function.
// -1 is returned if the character doesn't exist in the object string.
//
int _Tt_string::
rindex(char c) const
{
	if (len() == 0) {
		return -1;
	}

	register char *beg = (*this)->content;
	register char *p = beg+len()-1;

	while (p>=beg && *p!=c) --p;
	if (p<beg) {
		return -1;
	} else {
		return p-beg;
	}
}

int _Tt_string::
strchr(char c) const
{
  return index(c);
}

int _Tt_string::
strchr(const char *s) const
{
  return index(s);
}

int _Tt_string::
strrchr(char c) const
{
  return rindex(c);
}

int _Tt_string::
hash(int max_buckets) const
{
	int hash_value = 0;
	int length = len();
	char *p = (*this)->content;
	
	while (length != 0) {
		hash_value += (length * *p++);
		length--;
	}
	hash_value = hash_value % max_buckets;
	if (hash_value < 0) {
		hash_value = 0 - hash_value;
	}
	return (hash_value); /* hash to a bucket number */	
}


void _Tt_string::
print(const _Tt_ostream &os, int max, int quote_it) const
{
	(*this)->print(os, max, quote_it);
}

// We used to handle the two flavors of print via defaulting, but
// that was too inconvenient in the debugger since "stdout" is a macro
// not a global, so trying to type stringname.print\(stdout\) was no good.
void _Tt_string::
print() const
{
	(*this)->print(stdout);
}


//
// XDR support
//
bool_t _Tt_string::
xdr(XDR *xdrs)
{
	if (_tt_global != 0 &&
	    (*(_Tt_string_buf_ptr *)this).c_pointer() ==
	    _tt_global->universal_null_string.c_pointer()) {
		*(_Tt_string_buf_ptr *)this = new _Tt_string_buf;
	}

	return((*this)->xdr(xdrs));
}

_Tt_string_list::
_Tt_string_list() : _Tt_string_buf_list()
{
}

_Tt_string_list::
_Tt_string_list(const _Tt_string_list &l) : _Tt_string_buf_list(l)
{
}

_Tt_string_list::
_Tt_string_list(const _Tt_string_buf_list &l) : _Tt_string_buf_list(l)
{
}


_Tt_string_list::
~_Tt_string_list()
{
}

_Tt_string_list_ptr::
~_Tt_string_list_ptr()
{
}

/* 
 * Pointer assignment
 */
_Tt_string_list_ptr &_Tt_string_list_ptr::
operator=(_Tt_string_list *s)
{
	*(_Tt_string_buf_list_ptr *)this = (_Tt_string_buf_list *)s;
	return *this;
}

_Tt_string_list_ptr &_Tt_string_list_ptr::
operator=(const _Tt_string_list_ptr &s)
{
	return (_Tt_string_list_ptr &)_Tt_string_buf_list_ptr::operator=(s);
}

_Tt_string_list_cursor::
_Tt_string_list_cursor()
{
}

_Tt_string_list_cursor::
_Tt_string_list_cursor(const _Tt_string_list_ptr & l) : _Tt_string_buf_list_cursor(l)
{
}

_Tt_string_list_cursor::
~_Tt_string_list_cursor()
{
}

_Tt_string _Tt_string_list_cursor::
operator *() const
{
	return **(const _Tt_string_buf_list_cursor *)this;
}


_Tt_string _Tt_string_list_cursor::
operator ->() const {
	return **(const _Tt_string_buf_list_cursor *)this;
}

/* 
 * Encode data into an opaque Tt_string
 */
int
_tt_xdr_encode(xdrproc_t xdr_fn, void *data, _Tt_string &opaque_string)
{
	unsigned long	datasize;
	XDR		xdrs;


	if (!(datasize = _tt_xdr_sizeof(xdr_fn, (char *)data))) {
		return 0;
	}
	_Tt_string s((int)datasize);
	// create an in-memory xdr stream
	xdrmem_create(&xdrs, (char *)s, (u_int)datasize, XDR_ENCODE);
	// encode the data
	if (! (*(local_xdrproc_t)xdr_fn)(&xdrs, (caddr_t *)data)) {
		return 0;
	}
	opaque_string = s;
	return 1;
}

/* 
 * Decode data from an opaque _Tt_string
 */
int
_tt_xdr_decode(xdrproc_t xdr_fn, void *data, _Tt_string opaque_string)
{
	char	*encoded = (char *)opaque_string;
	XDR	xdrs;

	xdrmem_create(&xdrs, encoded, (u_int)opaque_string.len(), XDR_DECODE);
	if (! (*(local_xdrproc_t)xdr_fn)(&xdrs, (caddr_t *)data)) {
		return 0;
	}
	return 1;
}


/* 
 * XDR encode or decode a _Tt_string
 */
int
tt_xdr_string(XDR *xdrs, _Tt_string *s)
{
	return(s->xdr(xdrs));
}

/* 
 * XDR encode or decode a _Tt_string_list
 */
bool_t
_tt_xdr_string_list(XDR *xdrs, _Tt_string_list_ptr *strings_ptr)
{
	return (*strings_ptr)->xdr(xdrs);
}

_Tt_string_list_ptr::
_Tt_string_list_ptr() : _Tt_string_buf_list_ptr()
{
}

void	_tt_string_print(const _Tt_ostream &os, const _Tt_object *obj)
{
	((_Tt_string_buf *)obj)->print(os);
}
