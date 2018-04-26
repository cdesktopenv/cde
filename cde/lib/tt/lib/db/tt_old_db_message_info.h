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
/*%%  $XConsortium: tt_old_db_message_info.h /main/3 1995/10/23 10:06:18 rswiston $ 			 				 */
/*
 *
 * @(#)tt_old_db_message_info.h	1.6 07 Sep 1993
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */
/*
 * tt_old_db_message_info.h - Used to create a database record that stores
 *                        queued message info in a way that is reasonable
 *		          for the new DB server and compatible with the
 *		          old DB server.
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 *
 */

#ifndef _TT_OLD_DB_MESSAGE_INFO_H
#define _TT_OLD_DB_MESSAGE_INFO_H

#include "util/tt_object.h"
#include "util/tt_string.h"
#include "util/tt_xdr_version.h"

class _Tt_old_db_message_info : public _Tt_object {
public:
  _Tt_old_db_message_info ();
  ~_Tt_old_db_message_info ();

  int                 messageID;
  int                 numParts;
  int                 messageSize;
  _Tt_string_list_ptr ptypes;

  bool_t xdr (XDR *xdrs);

};

#endif /* _TT_OLD_DB_MESSAGE_INFO_H */
