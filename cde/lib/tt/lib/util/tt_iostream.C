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
//%%  $TOG: tt_iostream.C /main/5 1998/04/03 17:10:03 mgreess $ 			 				
/*
 * @(#)tt_iostream.cc	1.4 93/08/15
 * 
 * Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 */

#include <string.h>
#include <stdarg.h>
#include "util/tt_iostream.h"
#include "util/tt_enumname.h"
#include "mp/mp_message.h"
#include "mp/mp_pattern.h"

_Tt_ostream::_Tt_ostream()
{
	_f = 0;
	_s = 0;
	_was_newline = 1;  // so we indent the first line
}

_Tt_ostream::_Tt_ostream(
	FILE *sink
)
{
	_f = 0;
	_s = 0;
	_was_newline = 0;
	*this = sink;
}

_Tt_ostream::_Tt_ostream(
	_Tt_string &sink
)
{
	_f = 0;
	_s = 0;
	_was_newline = 0;
	*this = sink;
}

_Tt_ostream::~_Tt_ostream()
{
}

_Tt_ostream &
_Tt_ostream::operator =(
	FILE *sink
)
{
	_f = sink;
	_s = 0;
	return *this;
}

_Tt_ostream &
_Tt_ostream::operator =(
	_Tt_string &sink
)
{
	_f = 0;
	_s = &sink;
	return *this;
}

const _Tt_ostream &
_Tt_ostream::operator <<(
	const _Tt_message &m
) const
{
	m.print( *this );
	return *this;
}

const _Tt_ostream &
_Tt_ostream::operator <<(
	const _Tt_pattern &p
) const
{
	p.print( *this );
	return *this;
}

const _Tt_ostream &
_Tt_ostream::operator <<(
	const _Tt_string &s
) const
{
	s.print( *this );
	return *this;
}

const _Tt_ostream &
_Tt_ostream::operator <<(
	Tt_status status
) const
{
	*this << _tt_enumname( status );
	return *this;
}

const _Tt_ostream &
_Tt_ostream::operator <<(
	Tt_callback_action val
) const
{
	*this << _tt_enumname( val );
	return *this;
}

const _Tt_ostream &
_Tt_ostream::operator <<(
	void *p
) const
{
	char buf[20];
	::sprintf( buf, "0x%p", p );
	*this << buf;
	return *this;
}

const _Tt_ostream &
_Tt_ostream::operator <<(
	const char *s
) const
{
	if (s == 0) {
		*this << "(null)";
		return *this;
	}
	//
	// If no indenting and no newlines...
	//
	if ((_indent.len() <= 0) && (strchr( s, '\n' ) == 0)) {
		//
		// ... just blast the whole string
		//
		if (_f != 0) {
			fputs( s, _f );
		}
		if (_s != 0) {
			*_s = (*_s).cat( s );
		}
		return *this;
	}


	// XXX - Inefficient.
	// The string either has a newline in it, or indents is > 0,
	// so deal with it on a per-character basis.
	const char *pc = s;
	while (*pc != '\0') {
		*this << *pc;
		pc++;
	}
	return *this;
}

const _Tt_ostream &
_Tt_ostream::operator <<(
	char c
) const
{
	if (_f != 0) {
		if (_indent.len() > 0) {
			if (_was_newline == 1) {
				_indent.print( _f );
			}
		}
		fputc( c, _f );
	}
	if (_s != 0) {
		if (_indent.len() > 0) {
			if (_was_newline == 1) {
				*_s = (*_s).cat( _indent );
			}
		}
		*_s = (*_s).cat( c );
	}

	if (c == '\n') {
		((_Tt_ostream *) this)->_was_newline = 1;
	} else  {
		((_Tt_ostream *) this)->_was_newline = 0;
	}

	return *this;
}

const _Tt_ostream &
_Tt_ostream::operator <<(
	int n
) const
{
	char buf[ 20 ];
	::sprintf( buf, "%d", n );
	*this << buf;
	return *this;
}

#if defined(__osf__) || defined(linux) || defined(CSRG_BASED)
/* This operator is being added to take care of uid_t and gid_t
 * for osf */
const _Tt_ostream &
_Tt_ostream::operator <<(
	unsigned int n
) const
{
	char buf[ 20 ];
	::sprintf( buf, "%u", n );
	*this << buf;
	return *this;
}
#endif

const _Tt_ostream &
_Tt_ostream::operator <<(
	long n
) const
{
	char buf[ 20 ];
	::sprintf( buf, "%ld", n );
	*this << buf;
	return *this;
}

const _Tt_ostream &
_Tt_ostream::operator <<(
	unsigned long n
) const
{
	char buf[ 20 ];
	::sprintf( buf, "%lu", n );
	*this << buf;
	return *this;
}

const _Tt_ostream &
_Tt_ostream::print(
	_Tt_object_printfn print_it,
	const _Tt_object  *obj
) const
{
	(*print_it)( *this, obj );
	return *this;
}

int
_Tt_ostream::sprintf(
	unsigned int      maxlen,
	const char       *format,
	...
) const
{
	va_list	args;
	char *buf = (char *)malloc( maxlen + 1 );
	if (buf == 0) {
		return -1;
	}
	va_start( args, format );
	int val = vsprintf( buf, format, args );
	va_end( args );
	if (val >= 0) {
		*this << buf;
	}
	free( buf );
	return val;
}

void
_Tt_ostream::set_indent(
	const _Tt_string &new_indent
) const
{
	_Tt_ostream *nonconst_this = (_Tt_ostream *)this;
	nonconst_this->_indent = new_indent;
}
