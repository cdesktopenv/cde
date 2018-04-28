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
/*%%  $XConsortium: tttar_file_utils.h /main/3 1995/10/20 17:00:34 rswiston $ 			 				 */
/*
 * tttar_file_utils.h - File utilities for the LS/TT archive tool.
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */

#ifndef	_LSTAR_FILE_UTILS_H
#define	_LSTAR_FILE_UTILS_H

#include "tttar_string_map.h"

/*
 * Procedure declarations
 */

bool_t		is_child_in( _Tt_string path, _Tt_string_list_ptr paths );
bool_t		is_child_of( _Tt_string path, _Tt_string pathtree );
_Tt_string	new_name( _Tt_string old_name,
			  Lstar_string_map_list_ptr renamings );
_Tt_string_list_ptr	dir_entries( _Tt_string path, bool_t follow_symlinks );
_Tt_string_list_ptr	realtrees( _Tt_string_list_ptr	paths,
					bool_t follow_symlinks );
void		append_real_subtrees( _Tt_string_list_ptr realtrees,
				      _Tt_string path );
char	       *basename( char *pathname );
char	       *dirname( char *pathname );

#endif /* _LSTAR_FILE_UTILS_H */
