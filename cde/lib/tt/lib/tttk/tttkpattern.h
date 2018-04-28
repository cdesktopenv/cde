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
/*%%  $XConsortium: tttkpattern.h /main/3 1995/10/23 10:33:51 rswiston $ 			 				 */
/* -*-C++-*-
 *
 * @(#)tttkpattern.h	1.3 93/09/07
 *
 * Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 */
#ifndef tttkpattern_h
#define tttkpattern_h

#include "tttk/tttk.h"
#include "tttk/tttkmessage.h"

Tt_status		_tttk_patterns_destroy(
				Tt_pattern	       *pats
			);
Tt_pattern		_ttDtPatternCreate(
				Tt_category		category,
				Tt_scope		theScope,
				int			addDefaultSess,
				const char	       *file,
				Tttk_op			op,
				_TtDtMessageCB		cb,
				void		       *clientCB,
				void		       *clientData,
				int			registerIt
			);
Tt_message		_tttk_pattern_contract(
				Tt_pattern		pat
			);

#endif
