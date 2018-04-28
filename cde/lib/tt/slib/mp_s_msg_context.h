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
/*%%  $XConsortium: mp_s_msg_context.h /main/3 1995/10/23 11:57:02 rswiston $ 			 				 */
/* -*-C++-*-
 *
 * @(#)mp_s_msg_context.h	1.3 29 Jul 1993
 *
 * Tool Talk Message Passer (MP) - mp_s_msg_context.h
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 *
 * _Tt_s_msg_context knows the client side of the context RPC interface.
 */

#ifndef _MP_S_MSG_CONTEXT_H
#define _MP_S_MSG_CONTEXT_H

#include <mp/mp_msg_context.h>
#include <mp/mp_session.h>
#include <mp/mp_procid_utils.h>

class _Tt_s_msg_context : public _Tt_msg_context {
      public:
	_Tt_s_msg_context();
	virtual ~_Tt_s_msg_context();

	Tt_status		s_join(
					_Tt_procid_ptr &procID
				) const;
	Tt_status		s_quit(
					_Tt_procid_ptr &procID
				) const;
};

#endif				/* _MP_S_MSG_CONTEXT_H */
