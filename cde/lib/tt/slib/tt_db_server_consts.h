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
/*%%  $XConsortium: tt_db_server_consts.h /main/3 1995/10/23 12:03:11 rswiston $ 			 				 */
/*
 * tt_db_server_consts.h - Declares constants that are used by the
 *			   _Tt_db_server_db class that may be useful
 *                         in other parts of the DB server.
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 *
 */

#ifndef _TT_DB_SERVER_CONSTS_H
#define _TT_DB_SERVER_CONSTS_H

#include "db/tt_db_key.h"

#include <limits.h>
#ifndef LONG_BIT
#define LONG_BIT 32
#endif

// The TT_DB_VERSION should be set to the lowest version of ToolTalk
// that includes a DB server that is compatible with the current
// database table formats.  Currently, the last change to the table
// formats took place in ToolTalk version 1.1.
#define TT_DB_VERSION				"1.1"
 
#define TT_DB_FILE_TABLE_FILE			"file_table" 
#define TT_DB_FILE_OBJECT_MAP_FILE		"file_object_map" 
#define TT_DB_PROPERTY_TABLE_FILE		"property_table" 
#define TT_DB_ACCESS_TABLE_FILE			"access_table" 
 
#define TT_DB_PROPS_CACHE_LEVEL_PROPERTY	"_MODIFICATION_DATE" 
#define TT_DB_FORWARD_POINTER_PROPERTY		"_NEW_OIDKEY" 
#define TT_DB_OBJECT_TYPE_PROPERTY		"_NODE_TYPE" 
#define TT_DB_MESSAGE_PROPERTY			"_TT_MSG_%d_%d" 
#define TT_DB_MESSAGE_INFO_PROPERTY		"_TT_QUEUED_MSGS" 

// The ':' at the beginning of the file name is for old client
// compatibility.  The old server protocol expects a ':' in all file
// paths to delimit a hostname.
#define TT_DB_FORWARD_POINTER_FILE		":.TT_DB_FORWARD_POINTER_FILE"

const int TT_DB_MAX_KEY_LENGTH = 120;
const int TT_DB_MAX_PROPERTY_NAME_LENGTH = 64;

const int TT_DB_LONG_SIZE = LONG_BIT/8;
const int TT_DB_SHORT_SIZE = 2;
 
const int TT_DB_FIRST_KEY_OFFSET = 0;
const int TT_DB_SECOND_KEY_OFFSET = TT_DB_KEY_LENGTH;
const int TT_DB_FILE_PATH_OFFSET = TT_DB_KEY_LENGTH;
const int TT_DB_PROPERTY_NAME_OFFSET = TT_DB_KEY_LENGTH;
const int TT_DB_PROPERTY_VALUE_OFFSET = TT_DB_KEY_LENGTH+
				        TT_DB_MAX_PROPERTY_NAME_LENGTH;
const int TT_DB_ACCESS_USER_OFFSET = TT_DB_KEY_LENGTH;
const int TT_DB_ACCESS_GROUP_OFFSET = TT_DB_KEY_LENGTH+TT_DB_LONG_SIZE;
const int TT_DB_ACCESS_MODE_OFFSET = TT_DB_KEY_LENGTH+2*TT_DB_LONG_SIZE;

#endif /* _TT_DB_SERVER_CONSTS_H */
