/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: mp_s_pat_context.h /main/3 1995/10/23 11:57:25 rswiston $ 			 				 */
/* -*-C++-*-
 *
 * @(#)mp_s_pat_context.h	1.4 29 Jul 1993
 *
 * Tool Talk Message Passer (MP) - mp_s_pat_context.h
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 *
 * _Tt_s_pat_context knows server-side context matching.
 */

#ifndef _MP_S_PAT_CONTEXT_H
#define _MP_S_PAT_CONTEXT_H

#include <mp/mp_pat_context.h>

class _Tt_s_pat_context : public _Tt_pat_context {
      public:
	_Tt_s_pat_context();
	_Tt_s_pat_context(const _Tt_context &c);
	virtual ~_Tt_s_pat_context();

	int			matchVal(
					const _Tt_message &msg
				);
};

#endif				/* _MP_S_PAT_CONTEXT_H */
