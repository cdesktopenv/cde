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
/*%%  $XConsortium: mp_context.h /main/3 1995/10/23 10:22:44 rswiston $ 			 				 */
/* -*-C++-*-
 *
 * @(#)mp_context.h	1.5 30 Jul 1993
 *
 * Tool Talk Message Passer (MP) - mp_context.h
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 *
 *
 * _Tt_context is an object that represents a keyword.
 */

#ifndef _MP_CONTEXT_H
#define _MP_CONTEXT_H

#include <util/tt_object.h>
#include <util/tt_string.h>
#include <api/c/tt_c.h>
#include <rpc/rpc.h>

class _Tt_context : public _Tt_object {
      public:
	_Tt_context();
	_Tt_context(const _Tt_context &);
	virtual ~_Tt_context();

	Tt_status		setName(
					const char *slotname
				);

	void			print(
					const _Tt_ostream &os
				) const;
	bool_t			xdr(
					XDR *xdrs
				);

	const _Tt_string       &slotName() const { return _slotName; }

      protected:
	_Tt_string		_slotName;
};

#endif				/* _MP_CONTEXT_H */
