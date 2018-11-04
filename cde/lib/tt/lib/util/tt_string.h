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
/*%%  $XConsortium: tt_string.h /main/5 1996/09/24 10:06:16 drk $ 			 				 */
/*-*-C++-*-
 * 
 * @(#)tt_string.h	1.27 @(#)
 *
 * tt_string.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 * Headers for tt string class.
 */
#ifndef _TT_STRING_H
#define _TT_STRING_H

#include "tt_options.h"
#if defined(OPT_BUG_SUNOS_4) && defined(__GNUG__)
#	include <stdlib.h>
#endif
#include <limits.h>

#if defined(OPT_BUG_HPUX)
extern "C" {
#include <wchar.h>
};
#elif defined(OPT_BUG_SUNOS_5)
#include <widec.h>
#include <wctype.h>
#elif defined(OPT_BUG_AIX)
#include <string.h>
#endif

#include <rpc/rpc.h>
#include "util/tt_list.h"
#include "util/tt_object.h"
#include <memory.h>

const int _Tt_string_unlimited	= -1;
const int _Tt_string_user_width	= -2;

declare_list_of(_Tt_string_buf)
class _Tt_string_buf : public _Tt_object {
    friend class _Tt_string;
      public:
	_Tt_string_buf() 
	{
		content = (char *)0;
		length = 0;
	}
	_Tt_string_buf(const _Tt_string_buf &b);
	virtual ~_Tt_string_buf();
        void    set(const unsigned char*s, int len);
        bool_t	operator==(const _Tt_string_buf &b);
        bool_t	xdr(XDR *xdrs);
	bool_t	xdr_1(XDR *xdrs);
	bool_t	xdr_2(XDR *xdrs);
	// _Tt_string_unlimited:  no limit
	// _Tt_string_user_width: use $TT_ARG_TRACE_WIDTH or $_SUN_TT_ARG_TRACE_WIDTH
	void	print(const _Tt_ostream &os,
		      int max_print_width = 80000,
		      int quote_it = 0) const;
      private:
	char * content;
	int length;		// includes null byte (length=strlen()+1)
};

class _Tt_string : public _Tt_string_buf_ptr {
      public:
	_Tt_string();
	_Tt_string(const _Tt_string_buf_ptr &s) : _Tt_string_buf_ptr(s) {};
	_Tt_string(const _Tt_string &s);
	_Tt_string(const char *s);
	_Tt_string(const unsigned char *s, int len);
	_Tt_string(int len);	// Used by cat/substring to make result cells
	~_Tt_string();
	_Tt_string& operator=(const char *s);
	_Tt_string& operator=(const _Tt_string_buf_ptr &s)
			{*(_Tt_string_buf_ptr *)this = s; return *this;}
	_Tt_string& operator=(const _Tt_string &s)
			{*(_Tt_string_buf_ptr *)this = s; return *this;}
	_Tt_string& set(const unsigned char*s, int len);
	char& operator[](int offset);
	const char& operator[](int offset) const;
	//
	// XXX char *() should not be const, but so much of UNIX takes char *
	// instead of const char *
	//
	operator char *() const;
	operator const char *() const;
	_Tt_string left(int len) const;
	_Tt_string right(int len) const;
	_Tt_string mid(int offset, int len) const;
	_Tt_string split(int index, _Tt_string &prefix) const;
	_Tt_string split(char c, _Tt_string &prefix) const;
	_Tt_string split(const char *s, _Tt_string &prefix) const;
	_Tt_string rsplit(char c, _Tt_string &prefix) const;
	_Tt_string quote_nulls() const;
	_Tt_string unquote_nulls() const;
	_Tt_string cat(const _Tt_string &s) const;
	_Tt_string cat(char c) const;
	_Tt_string cat(int i) const;
	_Tt_string cat(unsigned int i) const;
	_Tt_string cat(long i) const;
	_Tt_string cat(unsigned long i) const;
	_Tt_string replace(const char *old, const _Tt_string &with) const;
	int len() const {
		return (*this)->length;
	}
	int operator==(const _Tt_string& s) const {
		return((*this)->length==s->length &&
		       0==memcmp((*this)->content, s->content,
				 (*this)->length));
	}
	int operator==(const char *s) const {return 0==cmp(s);}
	int operator<(const _Tt_string& s) const
	{return -1==cmp(s->content, s->length);}
	int operator<(const char *s) const {return -1==cmp(s);}
	int operator>(const _Tt_string& s) const
	{return +1==cmp(s->content, s->length);}
	int operator>(const char *s) const {return +1==cmp(s);}
	int operator!=(const _Tt_string &s) const {return !(*this==s);}
	int operator!=(const char *s) const {return !(*this==s);}
	int operator<=(const _Tt_string &s) const {return !(*this>s);}
	int operator<=(const char *s) const {return !(*this>s);}
	int operator>=(const _Tt_string &s) const {return !(*this<s);}
	int operator>=(const char *s) const {return !(*this<s);}
	int sh_match(const _Tt_string &pattern) const;
#ifdef _DEBUG_MALLOC_INC
	int DBindex(char *fname, int lnum, char c) const;
	int DBrindex(char *fname, int lnum, char c) const;
#else
	int index(char c) const;
	int index(const char *s) const;
	int rindex(char c) const;
	int strchr(char c) const;
	int strchr(const char *s) const;
	int strrchr(char c) const;
#endif
	int hash(int max_buckets) const;
	bool_t xdr(XDR *xdrs);
	void print(void) const;
	void print(const _Tt_ostream &os, int max_print_width = 80000,
		   int quote_it = 0) const;
	int cmp(const _Tt_string &s) const {return cmp(s->content, s->length);}
	int cmp(const char *q, int qlen = -1) const;
};

// Since _Tt_string is "just a _Tt_string_buf_ptr" with some extra operations,
// a _Tt_string_list and associated classes are just the _Tt_string_buf_list
// class and associated classes.

void	_tt_string_print(const _Tt_ostream &os, const _Tt_object *obj);
class _Tt_string_list : public _Tt_string_buf_list {
      public:
	_Tt_string_list();
	_Tt_string_list(const _Tt_string_list &l);
	_Tt_string_list(const _Tt_string_buf_list &l);
	~_Tt_string_list();
};

class _Tt_string_list_ptr : public _Tt_string_buf_list_ptr {
      public:
	_Tt_string_list_ptr();
	_Tt_string_list_ptr(_Tt_string_list *s) :
	  _Tt_string_buf_list_ptr((_Tt_string_buf_list *)s) {};
	_Tt_string_list_ptr(const _Tt_string_list_ptr &p) :
	  _Tt_string_buf_list_ptr(p) {};
	~_Tt_string_list_ptr();
	_Tt_string_list_ptr &operator=(_Tt_string_list *s);
	_Tt_string_list_ptr &operator=(const _Tt_string_list_ptr &s);
};

class _Tt_string_list_cursor: public _Tt_string_buf_list_cursor {
      public:
	_Tt_string_list_cursor();
	_Tt_string_list_cursor(const _Tt_string_list_cursor &c);
	_Tt_string_list_cursor(const _Tt_string_list_ptr &l);
	~_Tt_string_list_cursor();
	_Tt_string operator *() const;
	_Tt_string operator ->() const;
};	
	
int _tt_xdr_encode(xdrproc_t xdr_fn, void *data, _Tt_string &opaque_string);
int _tt_xdr_decode(xdrproc_t xdr_fn, void *data, _Tt_string opaque_string);
bool_t tt_xdr_string(XDR *xdrs, _Tt_string *s);
bool_t _tt_xdr_string_list(XDR *xdrs, _Tt_string_list_ptr *strings_ptr);
void _tt_print_escaped_string(const _Tt_ostream &os,
			      const char *s, int length,
			      int max_print_width = 8000,
			      int quote_it = 0);

#include "util/tt_iostream.h"

#endif
