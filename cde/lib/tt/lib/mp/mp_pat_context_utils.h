/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: mp_pat_context_utils.h /main/3 1995/10/23 10:26:36 rswiston $ 			 				 */
/* -*-C++-*-
 *
 * @(#)mp_pat_context_utils.h	1.4 30 Jul 1993
 *
 * Tool Talk Message Passer (MP) - mp_pat_context_utils.h
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 *
 * _Tt_pat_context is a context slot in a pattern.
 */

#ifndef MP_PAT_CONTEXT_UTILS_H
#define MP_PAT_CONTEXT_UTILS_H

#include <util/tt_object.h>
#include <util/tt_list.h>
#include <mp/mp_context_utils.h>

class _Tt_pat_context;
declare_derived_list_of(_Tt_pat_context,_Tt_context)

#endif				/* MP_PAT_CONTEXT_UTILS_H */
