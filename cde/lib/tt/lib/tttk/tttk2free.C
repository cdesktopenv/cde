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
//%%  $XConsortium: tttk2free.C /main/3 1995/10/23 10:33:09 rswiston $ 			 				
/*
 * @(#)tttk2free.C	1.3 93/09/07
 *
 * Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 */
#include "tttk/tttk2free.h"

_TttkItem2Free::_TttkItem2Free()
{
	_type = NoItem;
}

_TttkItem2Free::_TttkItem2Free(
	Tt_message msg
)
{
	operator=( msg );
}

_TttkItem2Free::_TttkItem2Free(
	Tt_pattern pat
)
{
	operator=( pat );
}

_TttkItem2Free::_TttkItem2Free(
	caddr_t ptr
)
{
	operator=( ptr );
}

_TttkItem2Free::~_TttkItem2Free()
{
	switch (_type) {
	    case Message:
		if (_msg != 0) {
			tttk_message_destroy( _msg );
		}
		break;
	    case Pattern:
		if (_pat != 0) {
			tt_pattern_destroy( _pat );
		}
		break;
	    case Pointer:
		if (_ptr != 0) {
			tt_free( _ptr );
		}
		break;
	    default:
		break;
	}
}

Tt_message
_TttkItem2Free::operator =(
	Tt_message msg
)
{
	_type = Message;
	_msg = msg;
	return msg;
}

Tt_pattern
_TttkItem2Free::operator =(
	Tt_pattern pat
)
{
	_type = Pattern;
	_pat = pat;
	return pat;
}

caddr_t
_TttkItem2Free::operator =(
	caddr_t ptr
)
{
	_type = Pointer;
	_ptr = ptr;
	return ptr;
}

_TttkList2Free::_TttkList2Free(
	unsigned int maxElems
)
{
	_num = 0;
	_max = maxElems;
#ifdef OPT_VECNEW
	_items = new _TttkItem2Free[ maxElems ];
	if (_items == 0) {
		_max = 0;
	}
#else
	_items = (_TttkItem2Free **)malloc( maxElems * sizeof(_TttkItem2Free *));
	if (_items == 0) {
		_max = 0;
	}
	for (unsigned int i = 0; i < _max; i++) {
		_items[ i ] = new _TttkItem2Free();
		if (_items[ i ] == 0) {
			_destruct();
			_max = 0;
			break;
		}
	}
#endif
}

_TttkList2Free::~_TttkList2Free()
{
	_destruct();
}

Tt_message
_TttkList2Free::operator +=(
	Tt_message msg
)
{
	if (_num >= _max) {
		return msg;
	}
	// LIFO
	_item( _max - _num - 1 ) = msg;
	_num++;
	return msg;
}

Tt_pattern
_TttkList2Free::operator +=(
	Tt_pattern pat
)
{
	if (_num >= _max) {
		return pat;
	}
	// LIFO
	_item( _max - _num - 1 ) = pat;
	_num++;
	return pat;
}

caddr_t
_TttkList2Free::operator +=(
	caddr_t ptr
)
{
	if (_num >= _max) {
		return ptr;
	}
	// LIFO
	_item( _max - _num - 1 ) = ptr;
	_num++;
	return ptr;
}

void
_TttkList2Free::flush()
{
	for (unsigned int i = 0; i < _num; i++) {
		_item( i ) = (caddr_t)0;
	}
}

void
_TttkList2Free::_destruct()
{
	if (_items != 0) {
#ifdef OPT_VECNEW
		delete [] _items;
#else
		for (unsigned int i = 0; i < _max; i++) {
			if (_items[ i ] != 0) {	
				delete _items[ i ];
				_items[ i ] = 0;
			}
		}
		free( _items );
		_items = 0;
#endif
	}
}

_TttkItem2Free &
_TttkList2Free::_item(
	int i
)
{
#ifdef OPT_VECNEW
	return _items[ i ];
#else
	return *_items[ i ];
#endif
}
