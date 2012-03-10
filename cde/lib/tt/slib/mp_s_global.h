/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: mp_s_global.h /main/3 1995/10/23 11:55:30 rswiston $ 			 				 */
/*
 *
 * mp_s_global.h
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 *
 *
 * _tt_s_mp and _tt_mp always point to the same object, but server-only
 * functions can use _tt_s_mp in order to access server-only functions.
 */
#if !defined(_MP_S_GLOBAL_H)
#define _MP_S_GLOBAL_H

class _Tt_s_mp;
extern	_Tt_s_mp			*_tt_s_mp;

#include "mp/mp_global.h"

#endif
