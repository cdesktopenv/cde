/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: mp_c_global.h /main/3 1995/10/23 10:20:03 rswiston $ 			 				 */
/*
 *
 * mp_c_global.h
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 *
 *
 * _tt_c_mp and _tt_mp always point to the same object, but client-only
 * functions can use _tt_c_mp in order to access client-only functions.
 */
#if !defined(_MP_C_GLOBAL_H)
#define _MP_C_GLOBAL_H

class _Tt_c_mp;
extern	_Tt_c_mp			*_tt_c_mp;

#include "mp/mp_global.h"

#endif
