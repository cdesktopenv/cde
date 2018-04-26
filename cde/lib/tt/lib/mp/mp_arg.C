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
//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $XConsortium: mp_arg.C /main/3 1995/10/23 10:18:54 rswiston $ 			 				
/*
 *
 * @(#)mp_arg.C	1.19 93/09/07
 *
 * Tool Talk Message Passer (MP) - mp_arg.C
 *
 * Copyright (c) 1990,1992 by Sun Microsystems, Inc.
 *
 * Implementation of the _Tt_arg & _Tt_arg_list classes representing
 * arguments to a method or a procedure.
 */
#include "mp/mp_global.h"
#include "mp/mp_arg.h"
#include <stdlib.h>
#include <string.h>
#include "util/tt_enumname.h"
#include "util/tt_iostream.h"
#include "util/tt_xdr_utils.h"
#include "util/tt_global_env.h"


/* 
 * Construct a _Tt_arg with everything zeroed out
 */
_Tt_arg::
_Tt_arg()
{
	constructor_common();
}

/*
 * Copy constructor for _Tt_args.
 */
_Tt_arg::
_Tt_arg(const _Tt_arg &a)
{
	_mode = a._mode;
	_type = a._type;
	_name = a._name;
	_data_type = a._data_type;
	_data_int = a._data_int;
	_data_string = a._data_string;
}

/*
 * Copy the _Tt_arg pointed to by a _Tt_arg_ptr.
 */
_Tt_arg::
_Tt_arg(const _Tt_arg_ptr &p)
{
	_mode = p->_mode;
	_type = p->_type;
	_name = p->_name;
	_data_type = p->_data_type;
	_data_int = p->_data_int;
	_data_string = p->_data_string;
}

_Tt_arg::
_Tt_arg(const _Tt_string &t)
{
	constructor_common();
	_type = t;
}

/* 
 * Constructs a _Tt_arg where the mode and type are known.
 * No valid data is yet known.
 * 
 */
_Tt_arg::
_Tt_arg(Tt_mode m, _Tt_string &t)
{
	constructor_common();
	_mode = m;
	_type = t;
}


_Tt_arg::
_Tt_arg(const char *t)
{
	_Tt_string ts(t);
	constructor_common();
	_type = ts;
}

/* 
 * Constructs a _Tt_arg where the mode and type are known.
 * No valid data is yet known.
 * 
 */
_Tt_arg::
_Tt_arg(Tt_mode m, const char *t)
{
	_Tt_string ts(t);
	constructor_common();
	_mode = m;
	_type = ts;
}


/* 
 * Destroys any memory associated with _Tt_arg
 */
_Tt_arg::
~_Tt_arg()
{
}


/* 
 * Sets the data associated with this arg as an integer
 */
Tt_status _Tt_arg::
set_data_int(int i)
{
	_data_type = INT;
	_data_int = i;
	return TT_OK;
}


/* 
 * Sets the data associated with this arg as a byte string
 */ 
Tt_status _Tt_arg::
set_data_string(const _Tt_string &s)
{
	_data_type = STRING;
	_data_string = s;
	return TT_OK;
}


/* 
 * Obtain the data for this arg as an int.
 * Return TT_ERR_NUM if the value is a string.
 */
Tt_status _Tt_arg::
data_int(int &i) const
{
	if (_data_type != INT) return TT_ERR_NUM;
	i = _data_int;
	return TT_OK;
}

/* 
 * Obtain the data for this arg as a string.
 * Return TT_ERR_POINTER if the value is an int.
 */
Tt_status _Tt_arg::
data_string(_Tt_string &s) const
{
	if (_data_type != STRING) return TT_ERR_POINTER;
	s = _data_string;
	return TT_OK;
}

/* 
 * Prints out a _Tt_arg. Mainly used for debugging purposes only.
 */
void _Tt_arg::
print(const _Tt_ostream &os) const
{
	os << _tt_enumname(_mode) << " " << _type << ": ";

	if (_name.len() > 0) {
		os << "<" << _name << ">\n";
	} else if (_data_type==INT) {
		os << _data_int << "\n";
	} else {
		_data_string.print(os, _Tt_string_user_width, 1);
		os << "\n";
	}
}	


/* 
 * XDR encodes and decodes a _Tt_arg object. Returns 1 if successful and
 * 0 otherwise. 
 */
bool_t _Tt_arg::
xdr(XDR *xdrs)
{
	if (! xdr_int(xdrs, (int *)&_mode)) {
		return(0);
	}
	if (! _type.xdr(xdrs)) {
		return(0);
	}
	if (_tt_global->xdr_version() < TT_PUSH_ROTATE_XDR_VERSION) {
		//
		// Earlier versions of the XDR protocol included an
		// a never-used _Tt_string called _matched_type.
		//
		_Tt_string _matched_type;
		if (! _matched_type.xdr(xdrs)) {
			return(0);
		}
	}
	if (! _name.xdr(xdrs)) {
		return(0);
	}
	if (! xdr_int(xdrs, (int *)&_data_type)) {
		return(0);
	}
	if (_data_type==INT) {
		if (! xdr_int(xdrs, &_data_int)) {
			return(0);
		}
	} else {		
		if (! _data_string.xdr(xdrs)) {
			return(0);
		}
	}
	return(1);
}

/*
 * update_value is used when merging a reply with the original message.
 * The value cells of this arg are updated with the value cells in the
 * given arg.
 */
Tt_status _Tt_arg::
update_value(const _Tt_arg &x)
{
	_data_type = x._data_type;
	_data_int = x._data_int;
	_data_string = x._data_string;
	return TT_OK;
}

/* 
 * Wrapper whose only purpose is to invoke the xdr method on the given
 * pointer. 
 */
bool_t
tt_xdr_arg(XDR *xdrs, _Tt_arg_ptr *ptr)
{
	return((*ptr)->xdr(xdrs));
}


/* 
 * XDR encodes and decodes a list of _Tt_arg objects. p is assumed to
 * point to the list on which to append/get the objects.
 */
bool_t
tt_xdr_arg_list(XDR *xdrs, _Tt_arg_list_ptr *p)
{
	return((*p).xdr(xdrs)); 
}


void 
_tt_arg_print(const _Tt_ostream &os, const _Tt_object *obj)
{
	((_Tt_arg *)obj)->print(os);
}

/*
 * Common initialization code for _Tt_arg constructors.
 */
void _Tt_arg::
constructor_common()
{
	_mode = TT_MODE_UNDEFINED;
	_type = (char *)0;
	_name = (char *)0;
	_data_type = UNSET;
	_data_int = 0;
	_data_string = (char *)0;
}	

/* 
 * Returns non-zero score if \'this\', which must be a pattern arg, matches
 * the given arg, which must be a message arg.  Matching is
 * less restrictive than equality, since the following exceptions
 * apply:
 * 1\) If the pattern type is ALL, it matches any type in the message.
 * 2\) If the message type is ALL, it matches any type in the pattern.
 *    \[Somehow, the pattern type should get into the message, but
 *     I don\'t know how yet.\]
 * 3\) If the pattern value is not set, it matches any value in the
 *    message.
 * Although this method is only used in the server, it is too hard
 * to figure out how to get messages and patterns un-XDRed in
 * the server with args as a _Tt_s_arg instead of _Tt_arg.
 */
int _Tt_arg::
match_score(const _Tt_arg &msg_arg, int &used_wildcard) const
{
	// Comparing args works because XDR is defined as always
	// padding with zeroes.

	int score = 1; // wildcard match scores only 1
	used_wildcard = 0;

	// match modes
	if (_mode == msg_arg.mode()) {
		score++;
	} else if (_mode != TT_MODE_UNDEFINED) {
		return 0;
	}

	// match types
	if (_type == msg_arg.type()) {
		// check equality first, in case both are "ALL"
		score++;
	} else if (_type == "ALL") {
		used_wildcard = 1; // pattern ALL matches everything
	} else if (msg_arg.type() == "ALL") {
		used_wildcard = 1; // message ALL matches everything
	} else {
		return 0;	// types mismatched
	}

	// match values
	switch (_data_type) {
	    case INT:
	    case STRING:
	    default:
		if (msg_arg.data_type() != _data_type) {
			return 0;
		}
		switch (_data_type) {
		    case INT:
			if (msg_arg.data_int() != _data_int) {
				return 0;
			}
			break;
		    case STRING:
			if (msg_arg.data_string() != _data_string) {
				return 0;
			}
			break;
		    default:
			return 0;
		}
		score++;
		break;
	    case UNSET:
		if (msg_arg.data_type() != _data_type) {
			used_wildcard = 1;
		}
		break;
	}

	// everything matches
	return score;
}

//
// Returns 1 if the typed values are equal, else 0.  Ignores mode and
// vtype.  Used by _Tt_pat_context::deleteValue().
//
int _Tt_arg::
operator==(const _Tt_arg &arg) const
{
	if (_data_type != arg.data_type()) {
		return 0;
	}
	switch (_data_type) {
	    case INT:
		if (_data_int != arg.data_int()) {
			return 0;
		}
		break;
	    case STRING:
		if (_data_string != arg.data_string()) {
			return 0;
		}
	}
	return 1;
}
