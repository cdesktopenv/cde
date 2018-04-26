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
/*%%  $XConsortium: tt_object_table.h /main/3 1995/10/23 10:42:59 rswiston $ 			 				 */
/* @(#)tt_object_table.h	1.11 @(#)
 *
 * tt_object_table.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */

#if !defined(_TT_OBJECT_TABLE_H)
#define _TT_OBJECT_TABLE_H
#include <util/tt_object.h>
#include <util/tt_ptr.h>
#include <util/tt_string.h>
#include <util/tt_assert.h>
#include <string.h>

typedef _Tt_string (*_Tt_object_table_keyfn)(_Tt_object_ptr &);
class _Tt_object_table : public _Tt_object {
	friend class _Tt_object_table_cursor;
      public:
	_Tt_object_table(int num_buckets = 19);
	virtual ~_Tt_object_table();
	_Tt_object_ptr &	lookup(const _Tt_string &key) const;
	int			lookup(const _Tt_string &key,
				       _Tt_object_ptr &obj) const;
	void			set_keyfn(_Tt_object_table_keyfn gk) {
		_getkey = gk;
	}
	void			insert(_Tt_object_ptr &o);
	void			remove(const _Tt_string &key);
	void			flush();
	bool_t			xdr(XDR * /* xdrs */) {
		fprintf(stderr,"Warning: _Tt_object_table::xdr called");
		return 0;
	}
	bool_t			xdr(XDR *xdrs,
				    _Tt_new_xdrfn xdrfn,
				    _Tt_object *(*make_new)());
	void			print(_Tt_object_printfn print_elt,
				      const _Tt_ostream &os) const;
	int			count() const {
		return _count;
	}
      private:
	_Tt_object_table_keyfn		_getkey;
	int				num_buckets;
        int				_count;
	ptr_to_list_of(_Tt_object) 	*buckets;
};
declare_ptr_to(_Tt_object_table)

class _Tt_object_table_cursor : public _Tt_allocated {
      public:
	_Tt_object_table_cursor();
	_Tt_object_table_cursor(const _Tt_object_table_cursor &c);
	_Tt_object_table_cursor(const _Tt_object_table_ptr &l);
	~_Tt_object_table_cursor();
        _Tt_object_ptr			&operator*();
	_Tt_object_ptr			&operator->();
	_Tt_object_table_cursor&	reset();
	_Tt_object_table_cursor&	reset(_Tt_object_table_ptr &l);
	int				next();
	int				prev();
	int				is_valid() const;
      private:
        _Tt_object_table_ptr		table;
	cursor_in_list_of(_Tt_object) 	listcursor;
	int				current_bucket;
};

#endif				/* _TT_OBJECT_TABLE_H */
