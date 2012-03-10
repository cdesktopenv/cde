/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: tt_table.h /main/3 1995/10/23 10:45:54 rswiston $ 			 				 */
/* -*-C++-*-
 *
 * tt_table.h
 *
 * Generic hash table implementation
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */
#if !defined(_TT_TABLE_H)
#define _TT_TABLE_H
#include "util/tt_generic.h"
#include "util/tt_list.h"
#include "util/tt_object_table.h"

#define table_of(t) name2(t,_table)
#define ptr_to_table_of(t) name2(table_of(t),_ptr)
#define cursor_in_table_of(t) name2(table_of(t),_cursor)

#define declare_table_of(TYPE)							\
class table_of(TYPE) : public _Tt_object_table {				\
    friend class cursor_in_table_of(TYPE);					\
      public:									\
        table_of(TYPE)(int nbkts = 19): _Tt_object_table(nbkts) {}		\
        table_of(TYPE)(_Tt_object_table_keyfn kf,				\
		       int nbkts = 19): _Tt_object_table(nbkts) {		\
			       set_keyfn(kf);					\
        }									\
	virtual ~table_of(TYPE)();						\
	void			insert(ptr_to(TYPE) &o) {			\
		_Tt_object_table::insert(o);					\
	}									\
	ptr_to(TYPE)		&lookup(const _Tt_string &key) {		\
		return (ptr_to(TYPE)&)_Tt_object_table::lookup(key);		\
	}									\
	int			lookup(const _Tt_string &key, ptr_to(TYPE) &obj) {\
		return _Tt_object_table::lookup(key, obj);		\
	}								\
	bool_t			xdr(XDR *xdrs);					\
};										\
declare_derived_ptr_to(table_of(TYPE),_Tt_object_table)				\
										\
class cursor_in_table_of(TYPE) : public _Tt_object_table_cursor {		\
      public:									\
	cursor_in_table_of(TYPE)() {}						\
	cursor_in_table_of(TYPE)(const _Tt_object_table_ptr &l) :		\
				_Tt_object_table_cursor(l){}			\
	cursor_in_table_of(TYPE)(const _Tt_object_table_cursor &c) :		\
				 _Tt_object_table_cursor(c) {}			\
	~cursor_in_table_of(TYPE)();						\
        cursor_in_table_of(TYPE)&  reset() {					\
		return (cursor_in_table_of(TYPE)&)_Tt_object_table_cursor::reset();	\
	}									\
	cursor_in_table_of(TYPE)&  reset(ptr_to(table_of(TYPE)) &l) {		\
		return (cursor_in_table_of(TYPE)&)_Tt_object_table_cursor::reset(l);	\
        }									\
	int 				next() {				\
		return _Tt_object_table_cursor::next();				\
	}									\
	int				prev() {				\
		return _Tt_object_table_cursor::prev();				\
	}									\
	int				is_valid() const {			\
		return _Tt_object_table_cursor::is_valid();			\
	}									\
        ptr_to(TYPE)			&operator*() {				\
		return (ptr_to(TYPE)&)_Tt_object_table_cursor::operator*();	\
	}									\
	ptr_to(TYPE)			&operator->() {				\
		return (ptr_to(TYPE)&)_Tt_object_table_cursor::operator*();	\
	}									\
};

#define implement_table_of(TYPE)						\
table_of(TYPE)::								\
~table_of(TYPE)()								\
{										\
}										\
										\
bool_t table_of(TYPE)::							      \
xdr(XDR *xdrs)								      \
{									      \
    return _Tt_object_table::xdr(xdrs, (_Tt_new_xdrfn)xdrfn_of(TYPE),  	      \
			     (_Tt_object *(*)())			      \
			     constructor_of(TYPE));			      \
}									      \
cursor_in_table_of(TYPE)::							\
~cursor_in_table_of(TYPE)()							\
{										\
}										\
implement_derived_ptr_to(table_of(TYPE),_Tt_object_table)

#endif				// TT_TABLE_H
