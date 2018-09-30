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
//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $XConsortium: tttar_file_utils.C /main/3 1995/10/20 17:00:26 rswiston $ 			 				
/*
 * tttar_file_utils.cc - File utilities for the ToolTalk archive tool.
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include "tttar_utils.h"
#include "tttar_file_utils.h"

extern char *_tt_get_realpath(char  *, char *);

/*
 * is_child_in() - Is <path> a node in any of the path trees listed?
 *
 *	Returns true if, for any string <pathtree> in <paths>,
 *	a) "<path>" == "<pathtree>", or
 *	b) "<pathtree>/" is a prefix of "<path>".
 *
 */
bool_t
is_child_in( _Tt_string path, _Tt_string_list_ptr paths )
{
	_Tt_string_list_cursor	path_cursor( paths );
	while (path_cursor.next()) {
		if (is_child_of( path, (*path_cursor))) {
			return TRUE;
		}
	}
	return FALSE;
}

/*
 * is_child_of() - Is <path> in the path tree given?
 *
 *	Returns true if
 *	a) "<path>" == "<pathtree>", or
 *	b) "<pathtree>/" is a prefix of "<path>".
 *
 */
bool_t
is_child_of( _Tt_string path, _Tt_string pathtree )
{
	if (pathtree == path) {
		return TRUE;
	}
	_Tt_string prefix = pathtree.cat( "/" );
	if ( prefix == path.left( prefix.len())) {
		return TRUE;
	}
	return FALSE;
}

/*
 * new_name() - Return the most specific renaming, or an empty string.
 */
_Tt_string
new_name( _Tt_string old_name, Lstar_string_map_list_ptr renamings )
{
	_Tt_string			_new_name;
	Lstar_string_map_ptr		best_renaming(new Lstar_string_map);
	Lstar_string_map_list_cursor	renaming_cursor( renamings );

	while (renaming_cursor.next()) {
		_Tt_string rename_pattern = (*renaming_cursor)->old_string();
		if (is_child_of( old_name, rename_pattern )) {
			if (   rename_pattern.len()
			     > best_renaming->old_string().len())
			{
				best_renaming = *renaming_cursor;
			}
		}
	}
	/*
	 * If a renaming was found...
	 */
	if ( best_renaming->old_string().len() > 0) {
		/*
		 * ... the new name will have as its prefix the
		 * replacement part of the mapping.
		 */
		_new_name = best_renaming->new_string();
		/*
		 * If it was not a perfect match...
		 */
		if (old_name != best_renaming->old_string()) {
			/*
			 * ... we need to tack on whatever is
			 * unique about old_string.
			 */
			_new_name =
			    _new_name.cat(
				old_name.right(
				      old_name.len()
				    - best_renaming->old_string().len()));
		}
	}
	return _new_name;
}

/*
 * dir_entries() - Return a new list of paths, with one entry for each
 *	entry in the directory <path>, each entry consisting of
 *	<path> appended by a slash and the name of the entry.
 *	Returns an empty list if <path> is not a directory.
 *	If !follow_symlinks, returns an empty list if <path> is a symlink.
 */
_Tt_string_list_ptr
dir_entries( _Tt_string path, bool_t follow_symlinks )
{
	DIR		       *dirp;
	_Tt_string_list_ptr	entries(new _Tt_string_list);

	if (! follow_symlinks) {
		struct stat	lstat_buf;
		int		lstat_status;

		lstat_status = lstat( (char *)path, &lstat_buf );
		if (( lstat_status == 0) && S_ISLNK(lstat_buf.st_mode)) {
			return entries;
		}
	}
	dirp = opendir( (char *)path );
	if (dirp == NULL) {
		return entries;
	}
	while (TRUE) {
		struct dirent  *entry;
		_Tt_string	epath;
		_Tt_string	epath_slash;
		_Tt_string	ename;

		entry = readdir( dirp );
		if (entry == NULL) {
			break;
		}
		ename = entry->d_name;
		if ((ename == ".") || (ename == "..")) {
			continue;
		}
		epath_slash = path.cat( "/" );
		epath = epath_slash.cat( entry->d_name );
		entries->push( epath );
	}
	if (closedir( dirp ) != 0) {
		fprintf( stderr, "%s: closedir(\"%s\"): %s\n",
			 our_process_name, (char *)path, strerror(errno) );
	}
	return entries;

} /* dir_entries() */

/*
 * realtrees() - Return a new absolutized list of the paths given.
 *	If follow_symlinks, then recurse on any directories listed and
 *	put the realpath of the other end of the symlink onto the list.
 */
_Tt_string_list_ptr
realtrees( _Tt_string_list_ptr paths, bool_t follow_symlinks )
{
	_Tt_string_list_ptr	realpaths(new _Tt_string_list);
	_Tt_string_list_cursor	path_cursor( paths );

	while (path_cursor.next()) {
		char		resolved_path_buf[ MAXPATHLEN+1 ];
		char	       *resolved_path;
		_Tt_string	abs_path;
		_Tt_string	path;
		struct stat	lstat_buf;
		int		lstat_status;

		path = (*path_cursor);
		lstat_status = lstat( (char *)path, &lstat_buf );
		if ( lstat_status != 0) {
			/*
			 * ToolTalk objects can be associated
			 * with paths that don't exist.
			 */
			if (errno != ENOENT) {
				fprintf( stderr,
					 "%s: lstat(\"%s\"): %s\n",
					 our_process_name,
					 (char *)path, strerror(errno) );
				continue;
			} else {
				resolved_path =
					_tt_get_realpath( (char *)path,
						  resolved_path_buf );
			}
		} else if (S_ISLNK(lstat_buf.st_mode)) {
			if (follow_symlinks) {
				resolved_path =	_tt_get_realpath( (char *)path,
						  	  resolved_path_buf );
			} else {
				/*
				 * Use the absolute path of the
				 * symlink instead of the path of the
				 * linked file.
				 */
				char *dir = dirname( (char *)path );
				char *base = basename( (char *)path );
				resolved_path = _tt_get_realpath( dir,
							  resolved_path_buf );
				if (resolved_path != NULL) {
					strcat( resolved_path_buf, "/" );
					int len = strlen( resolved_path_buf );
					strncat( resolved_path_buf,
						 base, MAXPATHLEN - len );
				}
				free(dir);
			}
		} else {
			resolved_path = _tt_get_realpath( (char *)path,
						  resolved_path_buf );
		}
		if (resolved_path != NULL) {
			abs_path = resolved_path;
		} else {
			if (errno == ENOENT) {
				/*
				 * XXX: We need to figure out here what the
				 * realpath would be if the file existed.
				 */
			}
			fprintf( stderr, "%s: %s: %s\n",
				 our_process_name,
				 (char *)path, strerror(errno) );
			continue;
		}
		realpaths->push( abs_path );
		if (follow_symlinks) {
			append_real_subtrees( realpaths, abs_path );
		}
	}
	return realpaths;

} /* realtrees() */

/*
 * append_real_subtrees() - If <path> is a directory, add to <realtrees>
 *	any directories it contains links to, and recurse on both
 *	these and any other real directories in <path>.
 */
void
append_real_subtrees( _Tt_string_list_ptr realtrees, _Tt_string path )
{
	struct stat	stat_buf;
	DIR	       *dirp;

	if (stat( (char *)path, &stat_buf ) != 0) {
		fprintf( stderr, "%s: stat(\"%s\"): %s\n",
			 our_process_name, (char *)path, strerror(errno) );
		return;
	}
	if (! S_ISDIR(stat_buf.st_mode)) {
		return;
	}
	dirp = opendir( (char *)path );
	if (dirp == NULL) {
		fprintf( stderr, "%s: realpath(\"%s\"): %s\n",
			 our_process_name, (char *)path, strerror(errno) );
		perror( NULL );
		return;
	}
	while (TRUE) {
		struct dirent  *entry;
		struct stat	lstat_buf;
		_Tt_string	epath;
		_Tt_string	epath_slash;
		_Tt_string	ename;

		entry = readdir( dirp );
		if (entry == NULL) {
			break;
		}
		ename = entry->d_name;
		if ((ename == ".") || (ename == "..")) {
			continue;
		}
		epath_slash = path.cat( "/" );
		epath = epath_slash.cat( entry->d_name );
		if (lstat( (char *)epath, &lstat_buf ) != 0) {
			fprintf( stderr, "%s: lstat(\"%s\"): %s\n",
				 our_process_name, (char *)epath,
				 strerror(errno) );
			perror( NULL );
			continue;
		}
		if (stat( (char *)epath, &stat_buf ) != 0) {
			fprintf( stderr, "%s: stat(\"%s\"): %s\n",
				 our_process_name, (char *)epath,
				 strerror(errno) );
			perror( NULL );
			continue;
		}
		if (S_ISDIR(stat_buf.st_mode)) {
			if (S_ISLNK(lstat_buf.st_mode)) {
				char	rpath_buf[ MAXPATHLEN+1 ];
				char   *rpath;

				rpath = _tt_get_realpath( (char *)epath, rpath_buf );
				if (rpath == NULL) {
					fprintf( stderr,
						 "%s: realpath(\"%s\"): %s\n",
						 our_process_name,(char *)epath,
						 strerror(errno) );
				} else {
					_Tt_string rp( rpath );
					realtrees->push( rp );
				}
			}
			append_real_subtrees( realtrees, epath );
		}
	}
	if (closedir( dirp ) != 0) {
		fprintf( stderr, "%s: closedir(\"%s\"): %s\n",
			 our_process_name, (char *)path, strerror(errno) );
	}

} /* append_real_subtrees() */

/*
 * basename() - Return the last component of a pathname.
 */
#if !defined(__linux__)
char *basename( char *pathname ) {
	char *the_basename;
	
	the_basename = strrchr( pathname, '/' );
	if (the_basename == NULL) {
		the_basename = pathname;
	} else {
		the_basename++;		// Don't want the '/'
	}
	return the_basename;
}
#endif

/*
 * dirname() - Return the pathname minus the basename, or "." if the
 *	basename is all there is.  Caller is responsible for free()ing
 *	the storage returned.
 */
char *dirname( char *pathname ) {
	char *the_basename;
	char *the_dirname;
	
	the_basename = strrchr( pathname, '/' );
	if (the_basename == NULL) {
		the_dirname = (char *)malloc((size_t)(2 * sizeof(char)));
		the_dirname[0] = '.';
		the_dirname[1] = '\0';
	} else {
		int len = the_basename - pathname;
		the_dirname = (char *)
			malloc((size_t)( sizeof(char) * (len + 1)));
		strncpy( the_dirname, pathname, len );
		the_dirname[ len ] = '\0';
	}
	return the_dirname;
}
