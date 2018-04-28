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
/*%%  $XConsortium: tttk2free.h /main/3 1995/10/23 10:33:17 rswiston $ 			 				 */
/*
 * @(#)tttk2free.h	1.3 93/09/07
 *
 * Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 */
#ifndef tttk2free_h
#define tttk2free_h

#include "api/c/tt_c.h"
#include "util/tt_new.h"
#include "tttk/tttk.h"

//
// An instance of this class will automatically destroy() a message or
// tt_free() a pointer when the instance goes out of scope.  This lets
// you avoid having to repetitively code the destroy() or free() right
// before every possible return from the scope. Typical usage:
//
//	TtDtItem2Free item;
//	Tt_message msg = tt_message_create();
//	item = msg;
//	...
//	if (error) return error;	// msg is automatically destroyed here
//	...
//	item = 0;			// now, msg won't be destroyed on return
//	return TT_OK;
//
// This class actually uses ttDtDestroy() instead of tt_message_destroy().
//
class _TttkItem2Free : public _Tt_allocated {
    public:
				_TttkItem2Free();
				_TttkItem2Free(
					Tt_message msg
				);
				_TttkItem2Free(
					Tt_pattern pat
				);
				_TttkItem2Free(
					caddr_t    ptr
				);
				~_TttkItem2Free();

	Tt_message		operator =(
					Tt_message msg
				);
	Tt_pattern		operator =(
					Tt_pattern pat
				);
	caddr_t			operator =(
					caddr_t    ptr
				);
    private:
	enum {
		NoItem,
		Message,
		Pattern,
		Pointer
	}			_type;
	union {
		Tt_message	_msg;
		Tt_pattern	_pat;
		caddr_t		_ptr;
	};
};

//
// This class is an array of _TttkItem2Free's.  Its main benefit is
// that the ::flush() method lets you defuse all the items at once
// (typically, right before returning successfully from a scope).
//
// A malloc() is only done at constructor-time, so that you need not
// check if the +=() method failed due to malloc() failure.  Thus,
// you have to know the max size of your list at constructor-time.
// Overflow items are ignored, and are thus potential memory leaks.
//
class _TttkList2Free : public _Tt_allocated {
    public:
			_TttkList2Free(
				unsigned int maxElems
			);
			~_TttkList2Free();

	Tt_message	operator +=(
				Tt_message   msg2Destroy
			);
	Tt_pattern	operator +=(
				Tt_pattern   pat2Destroy
			);
	caddr_t		operator +=(
				caddr_t	     ptr2tt_free
			);
	void		flush();
    private:
	void		_destruct();
	_TttkItem2Free  &_item(
				int	     i
			);

	unsigned int	_num;
	unsigned int	_max;
#ifdef OPT_VECNEW
	_TttkItem2Free  *_items;
#else
	_TttkItem2Free **_items;
#endif
};

#endif
