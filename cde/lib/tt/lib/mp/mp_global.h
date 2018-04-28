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
/*%%  $XConsortium: mp_global.h /main/3 1995/10/23 10:24:02 rswiston $ 			 				 */
/*
 * mp.h -- public interface classes to the Message Passer
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */
#ifndef _MP_GLOBAL_H
#define _MP_GLOBAL_H


#include <stdio.h>
#include <string.h>
#include "util/tt_object.h"
#include "util/tt_list.h"
#include "util/tt_string.h"
#include "mp/mp_xdr_functions.h"
#include "api/c/tt_c.h"

#define _TT_TYPES_CE_VERSION 1
#define _TT_XATOM_VERSION 1

class _Tt_mp;
extern _Tt_mp *_tt_mp;

enum _Tt_mp_status {
	_MP_OK = TT_OK,
	_MP_ERR = TT_ERR_INTERNAL,
	_MP_NO_NODE = TT_ERR_OBJID,
	_MP_NO_VALUE = TT_ERR_NO_VALUE,
	_MP_STALE_NODEID = TT_WRN_STALE_OBJID,
	_MP_NODE_LOCKED = TT_STATUS_LAST + 1,
	_MP_ERR_TYPE = TT_ERR_OTYPE,
	_MP_ERR_DB = TT_ERR_DBAVAIL,
	_MP_ERR_DBOPEN = TT_ERR_DBAVAIL,
	_MP_ERR_ACCESS = TT_ERR_ACCESS
};


void		dbg(char *fmt, char *f, int l, char *arg1 = (char *)0);
#define		DBG(msg) (dbg(msg, __FILE__, __LINE__))
#define		DBG1(fmt, arg1) (dbg(fmt, __FILE__, __LINE__, arg1))
#define		TRACE(msg) ((_tt_mp->trace) || ((msg) && (msg)->trace))
#endif				/* _MP_GLOBAL_H */
