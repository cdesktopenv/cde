/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: mp_pat_context.h /main/3 1995/10/23 10:26:23 rswiston $ 			 				 */
/* -*-C++-*-
 *
 * @(#)mp_pat_context.h	1.4 30 Jul 1993
 *
 * Tool Talk Message Passer (MP) - mp_pat_context.h
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 *
 * _Tt_pat_context is a context slot in a pattern.
 */

#ifndef _MP_PAT_CONTEXT_H
#define _MP_PAT_CONTEXT_H

#include <mp/mp_context.h>
#include <mp/mp_pat_context_utils.h>
#include <mp/mp_msg_context.h>
#include <mp/mp_arg_utils.h>
#include <rpc/rpc.h>

class _Tt_pat_context : public _Tt_context {
      public:
	_Tt_pat_context();
	_Tt_pat_context(const _Tt_context &);
	virtual ~_Tt_pat_context();

	Tt_status		addValue(
					const _Tt_string &value
				);
	Tt_status		addValue(
					int value
				);
	Tt_status		addValue(
					const _Tt_msg_context &msgCntxt
				);
	Tt_status		deleteValue(
					const _Tt_msg_context &msgCntxt
				);

	void			print(
					const _Tt_ostream &os
				) const;
	static void		print_(
					const _Tt_ostream &os,
					const _Tt_object *obj
				);
	bool_t			xdr(
					XDR *xdrs
				);

      protected:
	_Tt_arg_list_ptr	_values;
};

#endif				/* _MP_PAT_CONTEXT_H */
