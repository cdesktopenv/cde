/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: mp_s_pattern_inlines.h /main/3 1995/10/23 11:58:03 rswiston $ 			 				 */
/*
 *
 * mp_s_pattern_inlines.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */

// inline functions definitions for selected _Tt_s_pattern methods.

inline Tt_status _Tt_s_pattern::
add_arg(const _Tt_arg_ptr &arg)
{
	_args->append(arg);
	return(TT_OK);
}
