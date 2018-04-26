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
/*%%  $XConsortium: tttar_api.h /main/3 1995/10/20 17:00:02 rswiston $ 			 				 */
/*
 * tttar_api.h - Link Service/ToolTalk object archiving interface functions.
 *
 * If LS/TT object archiving were ever to go into the LS/TT API,
 * some variation on these would be the functions to put in.
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 */

#ifndef _TTTAR_API_H
#define _TTTAR_API_H

#include "tttar_string_map.h"

#define CURRENT_ARCHIVE_VERSION 1

typedef enum object_kind {
	NO_KIND,
	VERSION_NUM,
	SPEC,
	ARCHIVE_END,
	SUN_LINK } Object_kind;

bool_t	pathlist_lstt_archive(
			_Tt_string_list_ptr	paths,
		        bool_t			recurse,
		        bool_t			follow_symlinks,
			int			verbosity,
		        XDR		       *xdrs );
bool_t	pathlist_lstt_dearchive(
			_Tt_string_list_ptr	paths_to_extract,
			Lstar_string_map_list_ptr renamings,
			_Tt_string		where_to_dearchive,
			bool_t			preserve__props,
			int			verbosity,
		        XDR		       *xdrs );
bool_t	pathlist_lstt_archive_list(
			_Tt_string_list_ptr	paths_to_extract,
			int			verbosity,
		        XDR		       *xdrs );
bool_t	spec_archive(
			char	       *id,
			char 	       *path,
			int		verbosity,
			XDR	       *xdrs,
			Tt_status      *err );
bool_t	spec_dearchive(
			char	      **old_spec_id_ptr,
			char	      **new_spec_id_ptr,
			char	      **path_as_archived,
			Lstar_string_map_list_ptr renamings,
			char	       *where_to_create,
			bool_t		preserve__props,
			bool_t	      (*dearchive_this_path)(char *, void *),
			void	       *context,
		        int		verbosity,
			XDR 	       *xdrs,
			Tt_status      *err );

#endif /* _TTTAR_API_H */
