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
/*%%  $XConsortium: tt_list.h /main/3 1995/10/23 10:41:02 rswiston $ 			 				 */
/*-*-C++-*-
 *
 * list.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 */
#if !defined(_LIST_H)
#define _LIST_H
#include <util/tt_ptr.h>
#include <util/tt_object.h>
#include <util/tt_object_list.h>
#include <util/tt_assert.h>

#define list_of(t) name2(t,_list)
#define ptr_to_list_of(t) name2(t,_list_ptr)
#define cursor_in_list_of(t) name2(t,_list_cursor)

#define declare_list_of(TYPE) declare_derived_list_of(TYPE,_Tt_object)

// Note that list_of always declares a append_ordered function, but this
// is implemented only for a very few classes.  The implementation
// cannot be shared as append_ordered, unlike other functions, actually
// needs to understand the contents of the elements of the list.

#define declare_derived_list_of(TYPE,PARENT)                                 \
declare_derived_ptr_to(TYPE,PARENT)					     \
declare_derived_ptr_to(list_of(TYPE),list_of(PARENT))			     \
									     \
class list_of(TYPE) : public list_of(PARENT) {				     \
    friend class cursor_in_list_of(TYPE);				     \
      public:								     \
	list_of(TYPE)();						     \
	list_of(TYPE)(const list_of(TYPE) &t): list_of(PARENT)(t) {}         \
	~list_of(TYPE)();						     \
									     \
	void				flush()				     \
	{ _Tt_object_list::flush(); }					     \
	list_of(TYPE)&			push(const ptr_to(TYPE) &e)	     \
	{ return (list_of(TYPE)&)_Tt_object_list::push(e); }		     \
	list_of(TYPE)&			pop()				     \
	{ return (list_of(TYPE)&)_Tt_object_list::pop(); }		     \
	list_of(TYPE)&			append(const ptr_to(TYPE) &e)	     \
	{ return (list_of(TYPE)&)_Tt_object_list::append(e); }		     \
	list_of(TYPE)&			append_ordered(const ptr_to(TYPE) &e);\
	list_of(TYPE)&			append(ptr_to_list_of(TYPE) l)	     \
	{ return (list_of(TYPE)&)_Tt_object_list::append(l); }		     \
	list_of(TYPE)&		append_destructive(ptr_to_list_of(TYPE) l)   \
	{ return (list_of(TYPE)&)_Tt_object_list::append_destructive(l); }   \
	list_of(TYPE)&			dequeue()			     \
	{ return (list_of(TYPE)&)_Tt_object_list::dequeue(); }		     \
	ptr_to(TYPE)&			top() const			     \
	{ return (ptr_to(TYPE)&)_Tt_object_list::top(); }		     \
	ptr_to(TYPE)&			bot() const			     \
	{ return (ptr_to(TYPE)&)_Tt_object_list::bot(); }		     \
	ptr_to(TYPE)&			operator[] (int n) const	     \
	{ return (ptr_to(TYPE)&)_Tt_object_list::operator[](n); }	     \
									     \
	int				is_empty() const		     \
	{ return _Tt_object_list::is_empty(); }				     \
	int				count() const			     \
	{ return _Tt_object_list::count(); }				     \
	bool_t				xdr(XDR *xdrs);			     \
};     	       	       	       	       	       	       	       	       	     \
									     \
class cursor_in_list_of(TYPE) : public _Tt_object_list_cursor {		     \
      public:								     \
	cursor_in_list_of(TYPE)() {}					     \
	cursor_in_list_of(TYPE)(const cursor_in_list_of(TYPE) &c):	     \
		_Tt_object_list_cursor(c) {}				     \
	cursor_in_list_of(TYPE)(const ptr_to_list_of(TYPE) &l):		     \
		_Tt_object_list_cursor(l) {}				     \
	~cursor_in_list_of(TYPE)();					     \
	/* overload * (pointer deref) to mean "get contents of element	     \
	 * under cursor".*/						     \
	ptr_to(TYPE)			&operator*() const		     \
	{ return (ptr_to(TYPE)&)_Tt_object_list_cursor::operator*(); }	     \
        ptr_to(TYPE)                    &operator->() const		     \
	{ return (ptr_to(TYPE)&)_Tt_object_list_cursor::operator*(); }	     \
	cursor_in_list_of(TYPE)&	reset()				     \
	{ return (cursor_in_list_of(TYPE)&)_Tt_object_list_cursor::reset(); }\
	cursor_in_list_of(TYPE)&	reset(const ptr_to_list_of(TYPE) &l) \
	{ return (cursor_in_list_of(TYPE)&)_Tt_object_list_cursor::reset(l);}\
	int				next()				     \
	{ return _Tt_object_list_cursor::next(); }			     \
	int				prev()				     \
	{ return _Tt_object_list_cursor::prev(); }			     \
	cursor_in_list_of(TYPE)&	insert(const ptr_to(TYPE) &p)	     \
	{ return (cursor_in_list_of(TYPE)&)_Tt_object_list_cursor::insert(p);}\
	cursor_in_list_of(TYPE)&	remove()			     \
	{ return (cursor_in_list_of(TYPE)&)_Tt_object_list_cursor::remove();}\
	int				is_valid() const		     \
	{ return _Tt_object_list_cursor::is_valid(); }			     \
};

#define implement_list_of(TYPE)						      \
implement_derived_ptr_to(TYPE,_Tt_object)				      \
implement_derived_ptr_to(list_of(TYPE),_Tt_object_list)                       \
list_of(TYPE)::								      \
list_of(TYPE)()								      \
{									      \
}									      \
list_of(TYPE)::								      \
~list_of(TYPE)()							      \
{									      \
}									      \
									      \
bool_t	list_of(TYPE)::							      \
xdr(XDR *xdrs)								      \
{									      \
	return _Tt_object_list::xdr(xdrs,				      \
				    (_Tt_new_xdrfn)xdrfn_of(TYPE),	      \
				    (_Tt_object *(*)())	     	      	      \
				    constructor_of(TYPE));		      \
}		     							      \
									      \
cursor_in_list_of(TYPE)::						      \
~cursor_in_list_of(TYPE)()						      \
{									      \
}

#endif
