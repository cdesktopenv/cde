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
/*%%  $XConsortium: api_typecb_utils.h /main/3 1995/10/23 09:55:34 rswiston $ 			 				 */
/* @(#)api_typecb_utils.h	1.2 93/07/30 SMI
 *
 * api_typecb_utils.h
 *
 * Copyright (c) 1993 by Sun Microsystems, Inc.
 * 
 * Utilities declarations for _Tt_typecb
 */
#ifndef _API_TYPECB_UTILS_H
#define _API_TYPECB_UTILS_H
#include "util/tt_object.h"
#include "util/tt_list.h"
#include "util/tt_table.h"
#include "util/tt_string.h"

declare_list_of(_Tt_typecb)
declare_table_of(_Tt_typecb)

_Tt_string _tt_typecb_key(_Tt_object_ptr &o);
_Tt_string _tt_typecb_key(const _Tt_string &tid, int opnum);

#endif				/* MP_PATTERN_UTILS_H */
