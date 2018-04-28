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
/*%%  $XConsortium: mp_pattern_utils.h /main/3 1995/10/23 10:27:13 rswiston $ 			 				 */
/*
 *
 * mp_pattern_utils.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 * 
 * Utilities declarations for _Tt_pattern
 */
#ifndef MP_PATTERN_UTILS_H
#define MP_PATTERN_UTILS_H
#include <util/tt_object.h>
#include <util/tt_list.h>
#include <util/tt_string.h>
#include <util/tt_table.h>

class _Tt_pattern;
declare_list_of(_Tt_pattern)

class _Tt_patlist : public _Tt_object {
      public:
	_Tt_patlist();
	~_Tt_patlist();
	_Tt_patlist(_Tt_string o);
	void			set_op(_Tt_string o){ _op = o; };
	_Tt_string		op(){ return _op; };
	_Tt_pattern_list_ptr	patterns;
      private:
	_Tt_string		_op;
};
_Tt_string _tt_patlist_op(_Tt_object_ptr &o);
declare_list_of(_Tt_patlist)
declare_table_of(_Tt_patlist)
#endif				/* MP_PATTERN_UTILS_H */
