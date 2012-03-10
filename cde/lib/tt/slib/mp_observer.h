/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: mp_observer.h /main/3 1995/10/23 11:50:34 rswiston $ 			 				 */
/*
 *
 * mp_observer.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */
#ifndef _MP_OBSERVER_H
#define _MP_OBSERVER_H
#include <mp/mp_global.h>

class _Tt_observer  : public _Tt_object {
      public:
	_Tt_observer();
	_Tt_observer(_Tt_string ptid,
		     int opnum,
		     Tt_disposition reliability,
		     Tt_scope s = TT_SCOPE_NONE);
	virtual ~_Tt_observer();
	_Tt_string		&ptid() {
		return _ptid;
	}
	Tt_disposition		reliability() {
		return _reliability;
	}
	void			set_reliability(Tt_disposition r) {
		_reliability = r;
	}
	Tt_state		state() {
		return _state;
	}
	int			opnum() {
		return _opnum;
	}
	Tt_scope		scope() {
		return _scope;
	}
	void			set_scope(Tt_scope s) {
		_scope = s;
	}
	void			set_state(Tt_state s) {
		_state = s;
	}
      private:
	Tt_state		_state;
	Tt_scope		_scope;
	_Tt_string		_ptid;
	int			_opnum;
	Tt_disposition		_reliability;
};

#endif			/* _MP_OBSERVER_H */
