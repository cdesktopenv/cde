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
/*%%  $XConsortium: tt_const.h /main/3 1995/10/23 09:50:21 rswiston $ 			 				 */
/*
 *
 * tt_const.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 * This file contains global constants and definitions used throughout
 * tooltalk, plus #defines for compilation-time options.
 *
 * HACK: many of these are only used in the DM...
 */

#ifndef  _TT_CONST_H
#define  _TT_CONST_H

/* Compilation options */

/*
 * To *not* use the dbserver (i.e. to link NetISAM into every TT client) 
 * use:  "make DEFINES=-DTT_NO_DBSERVER"
 */

const char NULL_CHAR = '\0';
const int OID_KEY_LENGTH = 16;
const int MAX_PROP_LENGTH = 64;
const int PATH_LENGTH = 1024;
const int HOST_LENGTH = 1024;
const int MAX_OID_LENGTH = HOST_LENGTH + PATH_LENGTH + OID_KEY_LENGTH;
const int MSG_ID_LENGTH = 4;
const int MSG_PART_LENGTH = 4;
const int MAX_OTID_LENGTH = 64;

//
// How often to run garbage collection - Number of seconds in 24 hours.
//
const int _TT_GARBAGE_COLLECTION_FREQUENCY = 86400;

#define _TT_FILEJOIN_PROPNAME "_TT_SESSIONS"
#define _MP_NODE_MOD_PROP "_MODIFICATION_DATE"

#endif /* _TT_CONST_H */
