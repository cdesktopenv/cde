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
/*%%  $XConsortium: tt_db_msg_q_lock_entry.h /main/3 1995/10/20 16:41:58 rswiston $ 			 				 */
/*
 * tt_db_msg_q_lock_entry.h - Defines a class for holding lock information in the
 *                            locks list in the tt_db_msg_q_lock class.
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */

#ifndef _TT_DB_MSG_Q_LOCK_ENTRY_H
#define _TT_DB_MSG_Q_LOCK_ENTRY_H

#include "util/tt_object.h"
#include "util/tt_string.h"

class _Tt_db_msg_q_lock_entry : public _Tt_object {
public:
  _Tt_db_msg_q_lock_entry () {}
  ~_Tt_db_msg_q_lock_entry () {}

  _Tt_string clientID;
  _Tt_string fileKey;
};

#endif /* _TT_DB_MSG_Q_LOCK_ENTRY_H */
