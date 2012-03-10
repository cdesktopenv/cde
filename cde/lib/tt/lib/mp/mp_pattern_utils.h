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
