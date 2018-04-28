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
/*%%  $XConsortium: dm_enums.h /main/3 1995/10/23 10:11:45 rswiston $ 			 				 */
/*
 * Tool Talk Database Manager (DM) - dm_enums.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 * This file contains all the enumerations type used by the DM.
 */

#ifndef  _TT_DM_ENUMS_H
#define  _TT_DM_ENUMS_H

#include "tt_const.h"
#include "tt_options.h"
#if defined(OPT_BUG_SUNOS_4) && defined(__GNUG__)
#	include <stdlib.h>
#endif
#include "api/c/tt_c.h"

const int VER_NO_LENGTH = 4;
const int INITIAL_VERSION_NO = 0;
const int THIS_VERSION_NO = 1;
const int ETHER_ADDR_LEN = 6;
const int DEFAULT_KEY_VERSION = 0;

#define NSE_ENV	"NSE_ENV"
#define TT_DB_DIR "TT_DB_DIR"

enum _Tt_dm_status
{
	DM_OK = TT_OK,
	DM_ERROR = TT_STATUS_LAST+1,
	DM_INIT_FAILED,
	DM_DBDESC_EXISTS,
	DM_DB_EXISTS,
	DM_CREATE_FAILED,
	DM_OPEN_FAILED,
	DM_NO_RECORD,
	DM_READ_FAILED,
	DM_WRITE_FAILED,
	DM_DELETE_FAILED,
	DM_CLOSE_FAILED,
	DM_UNKNOWN_DBTABLE,
	DM_UNKNOWN_INDEX,
	DM_INVALID_VERSION_NUMBER,
	DM_PATHMAP_FAILED,
	DM_UPDATE_MFS_INFO_FAILED,
	DM_CLEAR_LOCKS_FAILED,
	DM_RECORD_LOCKED,
	DM_NO_MFS,
	DM_UNKNOWN_FS,
	DM_CONVERSION_ERROR,
	DM_RECORD_SET,
	DM_ACCESS_DENIED
};

/* Database table identifiers */

enum _Tt_dbtable_id
{
	DM_TABLE_OID_PROP,
	DM_TABLE_OID_ACCESS,
	DM_TABLE_OID_CONTAINER,
	DM_TABLE_DOCOID_PATH,
	DM_TABLE_MSG_QUEUE,
	DM_TABLE_LINK_ENDS,
	DM_TABLE_LINK_PROP,
	DM_TABLE_LINK_ACCESS,
	DM_TABLE_LAST
};

/* Database table key identifiers */

enum _Tt_key_id
{
	DM_KEY_UNDEFINED,	/* for records read iterator */
	DM_KEY_NONE,
	DM_KEY_OID_PROP,
	DM_KEY_OID,
	DM_KEY_DOC,
	DM_KEY_PATH,
	DM_KEY_MSG_PART,
	DM_KEY_LINK_DIR,
	DM_KEY_END,
	DM_KEY_LINK_PROP
};

/* Name spaces for oids' location */

enum _Tt_name_space
{
	DM_NS_NFS,
	DM_NS_NSE,
	DM_NS_STANDALONE,     /* standalone mode, dbs path in TT_DB_DIR env var */
	DM_NS_REDIRECT,
	DM_NS_LAST
};

/* states of a property's value */

enum _Tt_propvalue_state
{
	DM_PVS_OLD,
	DM_PVS_NEW,
	DM_PVS_DELETE
};

#define DM_LINKUR  0x1
#define DM_LINKUW  0x2
#define DM_LINKGR  0x4
#define DM_LINKGW  0x8
#define DM_LINKOR  0x10
#define DM_LINKOW  0x20

const char * _tt_enumname(_Tt_dm_status x);
const char * _tt_enumname(_Tt_dbtable_id x);
const char * _tt_enumname(_Tt_key_id x);
const char * _tt_enumname(_Tt_name_space x);
const char * _tt_enumname(_Tt_propvalue_state x);

#endif  /* _TT_DM_ENUMS_H */
