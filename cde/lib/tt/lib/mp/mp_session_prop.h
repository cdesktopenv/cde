/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: mp_session_prop.h /main/3 1995/10/23 10:29:09 rswiston $ 			 				 */
/*
 *
 * mp_session_prop.h
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */

#if !defined(_MP_SESSION_PROP_H)
#define _MP_SESSION_PROP_H

#include "util/tt_string.h"

class _Tt_session_prop : public _Tt_object {
      public:
	_Tt_session_prop();
	_Tt_session_prop(_Tt_string name, _Tt_string_list_ptr values);
	_Tt_session_prop(_Tt_string name, _Tt_string value);
	virtual ~_Tt_session_prop();
	_Tt_string		_name;
	_Tt_string_list_ptr	_values;
};

#endif
