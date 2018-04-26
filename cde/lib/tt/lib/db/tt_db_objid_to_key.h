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
/*%%  $XConsortium: tt_db_objid_to_key.h /main/3 1995/10/23 10:04:22 rswiston $ 			 				 */
/*
 * Tool Talk Utility - tt_db_objid_to_key.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc*
 *
 * Extern declarations for object ID to key conversion routine.
 *
 */

#ifndef _TT_DB_OBJID_TO_KEY_H
#define _TT_DB_OBJID_TO_KEY_H

#include <util/tt_string.h>

extern _Tt_string _tt_db_objid_to_key (const _Tt_string &objid);

#endif /* _TT_DB_OBJID_TO_KEY */
