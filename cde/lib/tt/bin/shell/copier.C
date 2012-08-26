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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $XConsortium: copier.C /main/3 1995/10/20 16:35:57 rswiston $ 			 				
/*
 * copier.cc - Link Service/ToolTalk wrapper for cp(1).
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 */

#include "tt_options.h"
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/param.h>
#include <errno.h>
#include "api/c/tt_c.h"
#include "util/tt_path.h"
#include "util/tt_gettext.h"
#include "util/copyright.h"
#include "copier.h"

/*
 * External variables
 */
extern char *_tt_get_realpath(char *, char *);

/*
 * copier::copier()
 */
copier::
copier( char *arg0 )
{
	if (arg0 != NULL) {
		char *base = strrchr( arg0, '/' );
		if (base == NULL) {
			base = arg0;
		} else {
			base++; /* Don't want the '/' */
		}
		_prog_name = base;
		_process_name = _prog_name;
	}
	_args = new _Tt_string_list();
	_from_paths = new _Tt_string_list();
	_should_cp = TRUE;
	_recurse = FALSE;
	_preserve = FALSE;
	_clonedir_mode = FALSE;
	_tt_opened = FALSE;
}

copier::
~copier()
{
}

/*
 * copier::do_cp() - Use system() to invoke cp(1), and return its exit status.
 *	We can just use _args, since we never get here when our one
 *	cp-incompatible option (-L) has been given.
 */
int copier::
do_cp()
{
	_Tt_string		cmd( "cp" );
	_Tt_string_list_cursor	arg_cursor( _args );

	while (arg_cursor.next()) {
		cmd = cmd.cat( " " ).cat( *arg_cursor );
	}
	//printf( "Invoking: %s\n", (char *)cmd );
	int sys_status = system( (char *)cmd );
	if (WIFEXITED(sys_status)) {
		return WEXITSTATUS(sys_status);
	} else {
		fprintf( stderr,
			 "%s: system(\"%s\"): %d\n",
			 (char *)_process_name, (char *)cmd, sys_status );
		return 1;
	}
}

/*
 * copier::do_ttcp() - Use tttar(1) to copy the objects of the _from_paths.
 *
 * Algorithm:
 *
 * if (_clonedir_mode) {
 *         cd _from_path; tttar cfhL - . | (cd ../_to_path; tttar xfLp? -)
 * } else {
 *         if (_to_path_is_dir) {
 *                 tttar cfhL - _from_paths | (cd _to_path ; tttar xfLp? - )
 *         } else {
 *                 tttar cfhL - _from_path | tttar xfLp? - -rename from to
 *         }
 * }
 */
Tt_status copier::
do_ttcp()
{
	_Tt_string		cmd;
	_Tt_string_list_cursor	from_path_cursor( _from_paths );
	Tt_status		err;

	if (_clonedir_mode) {
#ifdef DO_TTTAR_AFTER_CP
		if (mkdir( (char *)_to_path, S_IRWXU ) != 0) {
			return TT_ERR_PATH;
		}
#endif
		cmd = cmd.cat( "cd " ).cat( _from_paths->top()).cat( " ; ");
	}
	cmd = cmd.cat( "tttar cfhL -" );
	while (from_path_cursor.next()) {
		_Tt_string from_path = *from_path_cursor;
		if (! this->can_cp( from_path )) {
			/*
			 * Don't tttar any paths we know that cp(1) will
			 * reject. We do this for clonedir mode, too, so
			 * we can return if the cloning shouldn't be done.
			 */
			from_path_cursor.remove();
		} else {
			/*
			 * tt_file_destroy() any path that cp(1) will delete
			 */
			_Tt_string path2zap = _to_path;
			if (_to_path_is_dir) {
				/*
				 * cp(1) will overwrite any entry in _to_path
				 * that has the same name as a _from_path.
				 */
				_Tt_string dir, base;
				base = from_path.rsplit( '/', dir );
				path2zap = _to_path.cat( "/" ).cat( base );
			}
			err = tt_file_destroy( (char *)path2zap );
			if (err > TT_WRN_LAST) {
				fprintf( stderr,
					 catgets(_ttcatd, 8, 12,
						 "%s: Could not remove "
						 "ToolTalk objects of %s "
						 "because %s\n"),
					 (char *)_process_name,
					 (char *)path2zap,
					 tt_status_message(err) );
			}
		}
	}
	if (_from_paths->count() <= 0) {
		return TT_OK;
	}
	if (_clonedir_mode) {
		/*
		 * In clonedir mode, we just tttar up everything in
		 * the directory we're cloning.
		 */
		cmd = cmd.cat( " ." );
	} else {
		from_path_cursor.reset();
		while (from_path_cursor.next()) {
			cmd = cmd.cat( " " ).cat( *from_path_cursor );
		}
	}
	cmd = cmd.cat( " |" );
	if (_to_path_is_dir) {
		cmd = cmd.cat( " ( cd " );
		if (_clonedir_mode) {
			char		realpath_buf[ MAXPATHLEN ];

			/*
			 * If we're in _clonedir_mode, then we'll be
			 * cd'ing down into _from_path, and so we want
			 * a realpath of _to_path to cd over to,
			 * because if _from_path is a symlink then
			 * in _from_path "../_to_path" is _not_ _to_path.
			 */
			char *real_to_path = _tt_get_realpath( (char *)_to_path,
						       realpath_buf );
			if (real_to_path == NULL) {
				fprintf( stderr, "%s: %s: %s\n",
					 (char *)_process_name,
					 (char *)_to_path, strerror(errno) );
				return TT_ERR_PATH;
			}
			cmd = cmd.cat( real_to_path );
		} else {
			cmd = cmd.cat( _to_path );
		}
		cmd = cmd.cat( " ;" );
	}
	cmd = cmd.cat( " tttar xfL" );
	if (_preserve) {
		cmd = cmd.cat( "p" );
	}
	cmd = cmd.cat( " -" );
	/*
	 * Use the hack we built into tttar(1) to rename paths
	 * as they're extracted.  Rename each _from_path to _to_path.
	 */
	from_path_cursor.reset();
	while (from_path_cursor.next()) {
		_Tt_string from_path = *from_path_cursor;
		cmd = cmd.cat( " -rename " ).cat( from_path )
			 .cat( " " ).cat( _to_path );
		/*
		 * If the copy is to be made in a subdirectory of _to_path,
		 * make tttar maps _from_path to the appropriate
		 * subdirectory of _to_path as it extracts.
		 */
		if ((_to_path_is_dir) && (! _clonedir_mode)) {
			_Tt_string dir_name;
			cmd = cmd.cat( "/" )
				 .cat( from_path.rsplit( '/', dir_name ));
		}
	}
	if (_to_path_is_dir) {
		cmd = cmd.cat( " )" );
	}
	//printf( "Invoking: %s\n", (char *)cmd );
	int sys_status = system( (char *)cmd );
#ifdef DO_TTTAR_AFTER_CP
	if (_clonedir_mode) {
		/*
		 * remove the target directory, so that cp(1) won't
		 * see it and make _from_path a subdirectory of it.
		 */
		if (rmdir( (char *)_to_path ) != 0) {
			fprintf( stderr, "%s: rmdir(\"%s\"): %s\n",
				 (char *)_process_name, (char *)_to_path,
				 strerror(errno) );
			return TT_ERR_PATH;
		}
	}
#endif
	if (WIFEXITED(sys_status)) {
		if (WEXITSTATUS(sys_status) == 0) {
			return TT_OK;
		} else {
			return TT_ERR_INTERNAL;
		}
	} else {
		fprintf( stderr,
			 "%s: system(\"%s\"): %d\n",
			 (char *)_process_name, sys_status );
		return TT_ERR_INTERNAL;
	}

} /* do_ttcp() */

/*
 * copier::can_cp() - Can we copy this path to _to_path?
 *
 *	TO_DO: can_cp() can be as tricky as you like.
 */
bool_t copier::
can_cp( _Tt_string from_path )
{
	struct stat stat_buf;

	if (stat( (char *)from_path, &stat_buf) != 0) {
		return FALSE;
	}
	if (S_ISDIR(stat_buf.st_mode) && (! _recurse)) {
		return FALSE;
	}
	return TRUE;
}

/*
 * copier::open_tt()
 */
Tt_status copier::
open_tt()
{
	char *process_id = tt_open();
	Tt_status err = tt_ptr_error( process_id );
	if (err == TT_OK) {
		_process_id = process_id;
		_tt_opened = TRUE;
	} else if (err > TT_WRN_LAST) {
		fprintf( stderr,
			 "%s: tt_open(): %s\n",
			 (char *)_process_name, tt_status_message( err ) );
	}
	return err;
}

/*
 * copier::close_tt()
 */
Tt_status copier::
close_tt()
{
	if (! _tt_opened) {
		return TT_OK;
	}
	Tt_status err = tt_close();
	if (err > TT_WRN_LAST) {
		fprintf( stderr,
			 "%s: tt_close(): %s\n",
			 (char *)_process_name, tt_status_message( err ) );
	}
	return err;
}

/*
 * copier::parse_args()
 */
void copier::
parse_args( int argc, char **argv )
{
	for ( int arg_num = 1; arg_num < argc; arg_num++ ) {
		_Tt_string arg( argv[arg_num] );
		_args->append( arg );
		if (arg[0] == '-') {
			this->_parse_arg( (char *)arg );
		} else {
			if (arg_num == argc - 1) {
				_to_path = arg;
			} else {
				_from_paths->append( arg );
			}
		}
	}
	if ((_to_path.len() <= 0) || (_from_paths->count() <= 0)) {
		this->usage();
		exit(1);
	}
	if (_from_paths->count() > 1) {
		/*
		 * If multiple things to move, the place we're moving them to
		 * must be a directory.
		 */
		struct stat stat_buf;

		if (stat( (char *)_to_path, &stat_buf) != 0) {
			fprintf( stderr, "%s: %s: ", (char *)_process_name,
				 (char *)_to_path );
			perror(NULL);
			exit(2);
		}
		if (! S_ISDIR(stat_buf.st_mode)) {
			fprintf( stderr, "%s: %s: %s\n",
				 (char *)_process_name, (char *)_to_path,
				 strerror(ENOTDIR) );
			this->usage();
			exit(2);
		}
		_to_path_is_dir = TRUE;
	} else {
		struct stat stat_buf;

		_to_path_is_dir = FALSE;
		if (stat( (char *)_to_path, &stat_buf) == 0) {
			_to_path_is_dir = S_ISDIR(stat_buf.st_mode);
		} else {
			/*
			 * If you "cp -r dir1 dir2" and dir2 doesn't
			 * exist, cp(1) creates it and copies dir1's
			 * contents into it.  What we need to do in this
			 * case is to
			 * 1. mkdir( dir2 ), [so we can cd there]
			 * 2. (cd dir1; tttar cf - .) | (cd dir2; tttar xf -)
			 * 3. rmdir( dir2 )  [so cp(1) will do right thing]
			 */
			_Tt_string from_path = _from_paths->top();
			if (    (_from_paths->count() == 1)
			     && (stat( (char *)from_path, &stat_buf) == 0)
			     && S_ISDIR(stat_buf.st_mode))
			{
				_clonedir_mode = TRUE;
				_to_path_is_dir = TRUE;
			}
		}
	}

} /* parse_args() */

/*
 * copier::_parse_arg() - Parse an option
 *
 * If you add any options not supported by cp(1) and still expect
 * it to be system()'d, fix ::do_cp() to pass cp(1) the right args.
 */
void copier::
_parse_arg( char *arg )
{
	if (arg == NULL) {
		return;
	}
	int n = -1;
	while (arg[++n] != '\0') {
		switch (arg[n]) {
		    case '-':
			if (n != 0) {
				this->usage();
				exit(1);
			}
			break;
		    case 'i':
			this->usage();
			exit(1);
			break;
		    case 'L':
			_should_cp = FALSE;
			break;
		    case 'p':
			_preserve = TRUE;
			break;
		    case 'r':
		    case 'R':
			_recurse = TRUE;
			break;
		    case 'v':
			_TT_PRINT_VERSIONS((char *)_prog_name)
			exit(0);
			break;
		    case 'h':
		    default:
			this->usage();
			exit(1);
		}
	}
} /* _parse_arg() */

/*
 * copier::usage()
 */
void copier::
usage(FILE *fs) const
{
	fprintf( fs,
		 catgets(_ttcatd, 8, 13,
			 "Usage: %s [-pL] file1 file2\n"
			 "       %s [-prRL] path1 [path2 ...] dir\n"
			 "       %s -v\n"
			 "       %s -h\n"),
		 (char *)_prog_name, (char *)_prog_name, (char *)_prog_name,
		 (char *)_prog_name );
	fprintf( fs, "%s",
		 catgets(_ttcatd, 8, 14,
			 "\t-L      do not perform a cp(1)\n"
			 "\t-v      print the version number and quit\n"
			 "\t-h      print this message\n" ));
}
