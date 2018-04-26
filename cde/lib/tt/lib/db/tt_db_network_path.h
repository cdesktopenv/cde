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
/*%%  $XConsortium: tt_db_network_path.h /main/3 1995/10/23 10:03:37 rswiston $ 			 				 */
/*
 * Tool Talk Utility - tt_db_network_path.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 * Extern declarations for filepath utility functions
 *
 */

#ifndef  _TT_DB_NETWORK_PATH_H
#define  _TT_DB_NETWORK_PATH_H

#include <util/tt_string.h>
#include <db/tt_db_results.h>

extern _Tt_db_results _tt_db_network_path(const _Tt_string &path, 
					  _Tt_string       &local_path,
                                          _Tt_string       &hostname, 
          			          _Tt_string       &partition,
                                          _Tt_string       &network_path); 

#endif /* _TT_DB_NETWORK_PATH_H */
