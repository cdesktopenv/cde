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
/*%%  $XConsortium: tt_path.h /main/3 1995/10/23 10:43:27 rswiston $ 			 				 */
/*
 * Tool Talk Utility - tt_path.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 * Extern declarations for filepath utility functions
 *
 */

#ifndef  _TT_PATH_H
#define  _TT_PATH_H

#include "util/tt_string.h"

extern int _Tt_dirname(const char *, int len = 0);
extern int _Tt_basename(const char *, int len = 0);
extern _Tt_string_list_ptr _tt_dir_entries(const _Tt_string &path,
					   bool_t follow_symlinks);
extern _Tt_string _tt_realpath(const _Tt_string &path);
extern int _tt_isdir(const _Tt_string &path);
extern _Tt_string _tt_local_network_path(const _Tt_string &path);
extern _Tt_string _tt_network_path_to_local_path(const _Tt_string &path);
extern bool_t _tt_is_network_path(const _Tt_string &path);
extern _Tt_string _tt_user_path (_Tt_string file,
				 _Tt_string user_env,
				 bool_t     system_only);

#endif /* _TT_PATH_H */
