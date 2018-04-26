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
/*%%  $XConsortium: mp_ce_attrs.h /main/3 1995/10/23 11:49:51 rswiston $ 			 				 */
/*
 *
 * mp_ce_attrs.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */
enum _Tt_ce_attr {
	_TYPE_NAME = 0,
	_TT_OP,
	_TT_ARG,
	_TT_SCOPE,
	_TT_MSET_SCOPE,
	_TT_DISPOSITION,
	_TT_MSET_DISPOSITION,
	_TT_MSET_OPNUM,
	_TT_OPNUM,
	_TT_MSET_HANDLER_PTYPE,
	_TT_HANDLER_PTYPE,
	_TT_MSET_OTYPE,
	_TT_OTYPE,
	_TT_PARENT,
	_TT_OUT,
	_TT_IN,
	_TT_INOUT,
	_TT_SESSION,
	_TT_FILE,
	_TT_BOTH,
	_TT_FILE_IN_SESSION,
	_TT_DISCARD,
	_TT_QUEUE,
	_TT_START,
	_TT_CATEGORY,
	_TT_OBSERVE,
	_TT_HANDLE,
	_TT_CLASS,
	_TT_REQUEST,
	_TT_NOTICE,
	_TT_TOOLTALK_TYPE,
	_TT_TOOLTALK_PTYPE,
	_TT_TOOLTALK_OTYPE,
	_TT_TOOLTALK_SIGNATURE,
	_TT_CE_ATTR_LAST
};

const char	*_tt_ce_attr_string(_Tt_ce_attr);


	
