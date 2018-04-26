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
//%%  $XConsortium: tt_object_table.C /main/3 1995/10/23 10:42:52 rswiston $ 			 				
/* -*-C++-*-
 * @(#)tt_object_table.C	1.11 @(#)
 *
 * tt_object_table.cc
 *
 * Generic hash table implementation
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */

#include "tt_options.h"
#if defined(OPT_BUG_SUNOS_4) && defined(__GNUG__)
	// The GNU headers for 4.x do not fix malloc.h, so abort its inclusion
#	define __malloc_h
#endif

#include <util/tt_object_table.h>
									      
/*									      
 * Table operations							      
 */									      
									      
_Tt_object_table::							      
_Tt_object_table(int n)						      
{									      
	num_buckets = n;						      
	buckets = (_Tt_object_list_ptr *)calloc(n,
						sizeof(_Tt_object_list_ptr));
	_count = 0;
	_getkey = NULL;
}									      
									      
_Tt_object_table::							      
~_Tt_object_table()							      
{									      
	for (int i = 0; i < num_buckets; i++) {				      
		buckets[i] = (_Tt_object_list *)0;			      
	}								      
	(void)free((MALLOCTYPE *)buckets);					      
}									      
									      
_Tt_object_ptr & _Tt_object_table::
lookup(const _Tt_string &k) const
{
	_Tt_object_list_ptr bl = buckets[k.hash(num_buckets)];

	if (!bl.is_null() && bl->count()!=0) {
		_Tt_object_list_cursor b(bl);
		while (b.next()) {
			if ((*_getkey)(*b) == k) {
				return *b;
			}
		}
	}
	return _Tt_object::null_ptr();
}

int _Tt_object_table::
lookup(const _Tt_string &k, _Tt_object_ptr &obj) const
{
	_Tt_object_list_ptr bl = buckets[k.hash(num_buckets)];

	if (bl.is_null() || bl->count()==0) {
		obj = (_Tt_object *)0;
		return FALSE;
	}
	_Tt_object_list_cursor b(bl);
	while (b.next()) {
		if ((*_getkey)(*b) == k) {
			obj=*b;
			return(TRUE);
		}
	}
	obj = (_Tt_object *)0;
	return FALSE;
}

void _Tt_object_table::						      
insert(_Tt_object_ptr &n)
{									      
	_Tt_string	key((*_getkey)(n));
	int bucket_no = key.hash(num_buckets);
	
	if (buckets[bucket_no].is_null()) {
		buckets[bucket_no] = new _Tt_object_list;
	}
	buckets[bucket_no]->push(n);
	++_count;
}

									      
void _Tt_object_table::
remove(const _Tt_string &k)
{
	_Tt_object_list_cursor b(buckets[k.hash(num_buckets)]);
	while (b.next()) {
		if ((*_getkey)(*b) == k) {
			b.remove();
			--_count;
			return;
		}
	}
}


void _Tt_object_table::
flush()
{
	int i;
	for (i=0;i<num_buckets;++i) {
		if (!buckets[i].is_null()) {
			buckets[i] = (_Tt_object_list *)0;
		}
	}
	_count = 0;
}									      
									      
void _Tt_object_table::
print(_Tt_object_printfn print_elt, const _Tt_ostream &os) const
{
	_Tt_object_table_ptr	 t = (_Tt_object_table *)this;
	_Tt_object_table_cursor	 c(t);
	while (c.next()) {
		(*print_elt)(os, (*c).c_pointer());
	}
}


/*
 *  XDR encoding and decoding function for tables
 */
bool_t _Tt_object_table::
xdr(XDR *xdrs, _Tt_new_xdrfn xdrfn, _Tt_object *(*make_new)())
{
	if (! xdr_int(xdrs, &_count)) {
		return(0);
	}
	if (xdrs->x_op == XDR_ENCODE) {
		_Tt_object_table_ptr t = this;
		_Tt_object_table_cursor cursor(t);
		while (cursor.next()) {
			if ((*cursor).is_null()) {
				continue;
			}
			if (! (*xdrfn)(xdrs, (*cursor).c_pointer())) {
				return(0);
			}
		}
	} else {
		int		i;
		int		len;
		_Tt_object_ptr	ptr;

		len = _count;
		_count = 0;
		for (i=len; i > 0; --i) {
			ptr = (*make_new)();
			if (! (*xdrfn)(xdrs, ptr.c_pointer())) {
				return(0);
			} else {
				_Tt_object_ptr ptr2;
				int found = lookup((*_getkey)(ptr),ptr2);
				if (found==TRUE) {
					remove((*_getkey)(ptr));
				}
				insert(ptr);
			}
		}
	}

	return(1);
}
									     
									     
/*									     
 * Cursor functions							     
 */									     
									     
_Tt_object_table_cursor::
_Tt_object_table_cursor()
{
	table = (_Tt_object_table *)0;
	current_bucket = -1;
}


_Tt_object_table_cursor::						     
_Tt_object_table_cursor(const _Tt_object_table_cursor &c)	     
{									     
	table = c.table;						     
	listcursor = c.listcursor;					     
	current_bucket = c.current_bucket;				     
}									     
									     
_Tt_object_table_cursor::						      
_Tt_object_table_cursor(const _Tt_object_table_ptr &l)		      
{									      
	table = l;							      
	current_bucket = -1;						      
}									      
									      
_Tt_object_table_cursor::						      
~_Tt_object_table_cursor()						      
{									      
}									      
									      
_Tt_object_table_cursor & _Tt_object_table_cursor::		      
reset()									      
{									      
	current_bucket = -1;						      
	return *this;							      
}									      
									      
_Tt_object_table_cursor & _Tt_object_table_cursor::		      
reset(_Tt_object_table_ptr &l)					      
{									      
	table = l;							      
	current_bucket = -1;						      
	return *this;							      
}									      
									      
_Tt_object_ptr & _Tt_object_table_cursor::				      
operator*()
{									      
	if (current_bucket == -1) {
		return _Tt_object::null_ptr();
	} else {							      
		return(*listcursor);
	}
}									      
									      
_Tt_object_ptr & _Tt_object_table_cursor::				      
operator->()
{									      
	return(*listcursor);						      
}									      
									      
int _Tt_object_table_cursor::					      
next()									      
{									      
	if (current_bucket == -1) {					      
		current_bucket = 0;					      
		listcursor.reset(table->buckets[current_bucket]);	      
	}								      
	while (!listcursor.next()) {					      
		++current_bucket;					      
		if (current_bucket >= table->num_buckets) {		      
			this->reset(table);				      
			return 0;					      
		} else {						      
			listcursor.reset(table->buckets[current_bucket]);     
		}							      
	}								      
	return 1;							      
}									      
									      
int _Tt_object_table_cursor::					      
prev()									      
{									      
	if (current_bucket == -1) {					      
		current_bucket = table->num_buckets-1;			      
		listcursor.reset(table->buckets[current_bucket]);	      
	}								      
	while (!listcursor.prev()) {					      
		--current_bucket;					      
		if (current_bucket<0) {					      
			this->reset(table);				      
			return 0;					      
		} else {						      
			listcursor.reset(table->buckets[current_bucket]);     
		}							      
	}								      
	return 1;							      
}									      
									      
int _Tt_object_table_cursor::					      
is_valid() const							      
{									      
	if (current_bucket==-1) return 0;				      
	return listcursor.is_valid();					      
}									      
implement_ptr_to(_Tt_object_table)



