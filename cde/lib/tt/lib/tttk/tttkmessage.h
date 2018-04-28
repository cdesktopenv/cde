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
/*%%  $XConsortium: tttkmessage.h /main/3 1995/10/23 10:33:31 rswiston $ 			 				 */
/* -*-C++-*-
 *
 * @(#)tttkmessage.h	1.3 93/09/07
 *
 * Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 */
#ifndef ttdtmessage_h
#define ttdtmessage_h

#include "tttk/tttk.h"

typedef Tt_message	(*_TtDtMessageCB)(
				Tt_message	msg,
				Tt_pattern	pat,
				void	       *clientCB,
				void	       *clientData
			);

Tt_message		_tttk_message_receive(
				const char	       *procid
			);
Tt_status		_tttk_message_reply(
				Tt_message		msg
			);
Tt_message		_ttDtPMessageCreate(
				Tt_message		context,
				Tt_class		theClass,
				Tt_scope		theScope,
				const char	       *handler,
				Tttk_op			op,
				_TtDtMessageCB		ttDtCb,
				void		       *clientCB,
				void		       *clientData
			);
char		       *_tttk_message_id(
				Tt_message		msg,
				int        		arg = 0
			);
int			_tttk_message_in_final_state(
				Tt_message		msg
			);
int			_tttk_message_am_handling(
				Tt_message		msg
			);
int			_tttk_message_arg_ival(
				Tt_message		msg,
				unsigned int		n,
				int			default_val
			);
char		       *_tttk_message_arg_val(
				Tt_message		msg,
				unsigned int		n,
				const char	       *default_val
			);
int			_tttk_message_arg_is_set(
				Tt_message		msg,
				unsigned int		n
			);
Tt_message		_ttTkNoteReplyStatus(
				Tt_message		msg,
				Tt_pattern		,
				void		       *,
				void		       *clientData
			);

#endif
