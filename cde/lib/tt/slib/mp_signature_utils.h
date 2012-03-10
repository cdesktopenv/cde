/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: mp_signature_utils.h /main/3 1995/10/23 12:01:56 rswiston $ 			 				 */
/*
 *
 * mp_signature_utils.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */
#ifndef MP_SIGNATURE_UTILS_H
#define MP_SIGNATURE_UTILS_H
#include <util/tt_object.h>
#include <util/tt_list.h>
#include <util/tt_string.h>

declare_list_of(_Tt_signature)


class _Tt_sigs_by_op : public _Tt_object {
      public:
	_Tt_sigs_by_op();
	~_Tt_sigs_by_op();
	_Tt_sigs_by_op(_Tt_string o);
	void			set_op(_Tt_string o){ _op = o; };
	_Tt_string		op(){ return _op; };
	_Tt_signature_list_ptr	sigs;
      private:
	_Tt_string		_op;
};
_Tt_string _tt_sigs_by_op_op(_Tt_object_ptr &o);
declare_list_of(_Tt_sigs_by_op)
declare_table_of(_Tt_sigs_by_op)
#endif				/* MP_SIGNATURE_UTILS_H */
