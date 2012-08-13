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
//%%  $XConsortium: mover.C /main/4 1995/10/20 16:36:15 rswiston $ 			 				
/*
 * mover.cc - Link Service/ToolTalk wrapper for mv(1).
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 */

#include "tt_options.h"
#include <errno.h>
#include <string.h>
#if defined(__osf__) || defined(linux) || defined(CSRG_BASED)
#include <unistd.h>
#else
#if !defined(USL) && !defined(__uxp__)
#include <osfcn.h>
#endif
#endif
#include <stdlib.h>
#include <errno.h>
#if defined(ultrix)
#include <sys/inode.h>
#define S_ISLNK(m)     (((m)&IFMT) == IFLNK)
#endif
#include <sys/stat.h>
#if defined(OPT_BUG_USL) || defined(OPT_BUG_UXP)
#define S_ISLNK(mode)     (((mode) & S_IFMT) == S_IFLNK)
#endif
#include <sys/wait.h>
#include "api/c/tt_c.h"
#include "util/tt_gettext.h"
#include "util/copyright.h"
#include "mover.h"

/*
 * External variables
 */

/*
 * mover::mover()
 */
mover::
mover( char *arg0 )
{
	if (arg0 != NULL) {
		char *base = strrchr( arg0, '/' );
		if (base == NULL) {
			base = arg0;
		} else {
			base++; // Don't want the '/'
		}
		_prog_name = base;
		_process_name = _prog_name;
	}
	_args = new _Tt_string_list();
	_from_paths = new _Tt_string_list();
	_should_mv = TRUE;
	_force = FALSE;
	_tt_opened = FALSE;
}

mover::
~mover()
{
}

/*
 * mover::do_mv() - Use system() to invoke mv(1), and return its exit status.
 *	We can just use _args, since we never get here when our one
 *	mv-incompatible option (-L) has been given.
 */
int mover::
do_mv()
{
	_Tt_string		cmd( "mv" );
	_Tt_string_list_cursor	arg_cursor( _args );

	while (arg_cursor.next()) {
		cmd = cmd.cat( " " ).cat( *arg_cursor );
	}
	//printf( "Invoking: %s\n", (char *)cmd );
	int sys_status = system( (char *)cmd );
	if (WIFEXITED(sys_status)) {
		return WEXITSTATUS(sys_status);
	} else {
		if (! _force) {
			fprintf(stderr, "%s: system(\"%s\"): %d\n",
				(char *)_process_name, (char *)cmd, sys_status);
		}
		return 1;
	}
}

/*
 * mover::do_ttmv() - Use tt_file_move() on the things to move.
 */
Tt_status mover::
do_ttmv()
{
	Tt_status	worst_err = TT_OK;
	Tt_status	err;
	_Tt_string	full_to_path;
	bool_t		abort = FALSE;
	bool_t		are_more;

	full_to_path = _to_path;
	_Tt_string_list_cursor from_path_cursor( _from_paths );
	/*
	 * call to next() must be first, so that are_more will be valid
	 * if we abort.  Why does the next() method wrap around?
	 */
	while ((are_more = from_path_cursor.next()) && (! abort)) {
		if (! this->can_mv( *from_path_cursor )) {
			continue;
		}
		/*
		 * tt_file_destroy() any path that mv(1) will delete
		 */
		if (_to_path_is_dir) {
			full_to_path = _to_path.cat("/").
				       cat(*from_path_cursor);
		}
		/*
		 * mv(1) will overwrite any entry in _to_path that
		 * has the same name as a _from_path.
		 */
		err = tt_file_destroy( (char *)full_to_path );
		if ((err > TT_WRN_LAST) && (! _force)) {
			fprintf( stderr,
				 catgets(_ttcatd, 8, 2,
					 "%s: Could not remove "
					 "ToolTalk objects of %s "
					 "because %s\n"),
				 (char *)_process_name,
				 (char *)full_to_path,
				 tt_status_message(err) );
		}
		err = tt_file_move( (char *)*from_path_cursor,
				    (char *)full_to_path );
		if (err > TT_WRN_LAST) {
			worst_err = err;
			if (! _force) {
				fprintf( stderr,
					 catgets(_ttcatd, 8, 3,
						 "%s: Could not move ToolTalk "
						 "objects of \"%s\" to \"%s\" "
						 "because %s\n"),
					 (char *)_process_name,
					 (char *)*from_path_cursor,
					 (char *)full_to_path,
					 tt_status_message( err ));
			}
			switch (err) {
			    case TT_ERR_DBAVAIL:
			    case TT_ERR_PATH:
				break;
			    case TT_ERR_NOMP:
			    case TT_ERR_DBEXIST:
			    default:
				abort = TRUE;
				break;
			}
		}
	}
	if (are_more && (! _force)) {
		from_path_cursor.prev();
		fprintf( stderr,
			 catgets(_ttcatd, 8, 4,
				 "%s: Will not attempt to move the ToolTalk "
				 "objects of:\n"),
			 (char *)_process_name );
		while (from_path_cursor.next()) {
			fprintf( stderr, "\t%s\n", (char *)*from_path_cursor );
		}
	}
	/*
	 * TO_DO: This should be uncommented if you think that warning them
	 * about hygiene is more important than obeying the -f flag.
	 *
	if ((worst_err > TT_WRN_LAST) && _should_mv && _force) {
		fprintf( stderr, "%s: The ToolTalk objects of some files were "
			 "not moved.\nSince you've told us to move the files "
			 "anyway, you will need to\nuse %s -L to move "
			 "the ToolTalk objects of the problem files.\n",
			 (char *)_process_name, (char *)_prog_name );
	}
	*/
	return worst_err;

} /* do_ttmv() */

/*
 * mover::can_mv() - Can we move this path to _to_path?
 *
 *	TO_DO: Judging by mv.c, can_mv() can be as tricky as you like.
 *	I'll count on tt_file_move() to Do The Right Thing.
 */
bool_t mover::
can_mv( _Tt_string from_path )
{
	struct stat lstat_buf;
	if (lstat( (char *)from_path, &lstat_buf) == 0) {
		if (S_ISLNK(lstat_buf.st_mode)) {
			/*
			 * Don't tt_file_move() a symlink, or TT will
			 * tt_file_move() the linked file.
			 */
			return FALSE;
		} else {
			return TRUE;
		}
	} else {
		/*
		 * If we're trying to mv a file that doesn't exist,
		 * let's not tt_file_move() the associated pathname.
		 * But if we're trying to ttmv -L a file that doesn't
		 * exist, we should probably tt_file_move() it anyway.
		 */
		if (_should_mv) {
			return FALSE;
		} else {
			return TRUE;
		}
	}
}

/*
 * mover::open_tt()
 */
Tt_status mover::
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
			 (char *)_process_name, tt_status_message(err) );
	}
	return err;
}

/*
 * mover::close_tt()
 */
Tt_status mover::
close_tt()
{
	if (! _tt_opened) {
		return TT_OK;
	}
	Tt_status err = tt_close();
	if (err > TT_WRN_LAST) {
		fprintf( stderr,
			 "%s: tt_close(): %s\n",
			 (char *)_process_name, tt_status_message(err) );
	}
	return err;
}

/*
 * mover::parse_args()
 */
void mover::
parse_args( int argc, char **argv )
{
	bool_t no_more_options = FALSE;

	for ( int arg_num = 1; arg_num < argc; arg_num++ ) {
		_Tt_string arg( argv[arg_num] );
		_args->append( arg );
		if ((arg[0] == '-') && (! no_more_options)) {
			if (arg[1] == '\0') {
				/*
				 * The bare option "-" means take the
				 * subsequent arguments to be pathnames.
				 */
				no_more_options = TRUE;
			} else {
				for (int n = 1; n < arg.len(); n++) {
					switch (arg[n]) {
					    case 'f':
						_force = TRUE;
						break;
					    case 'L':
						_should_mv = FALSE;
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
			}
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
			fprintf( stderr, "%s: \"%s\": ", (char *)_process_name,
				 (char *)_to_path );
			perror(NULL);
			exit(2);
		}
		if (! S_ISDIR(stat_buf.st_mode)) {
			fprintf( stderr, "%s: \"%s\": %s\n",
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
		}
	}
}

/*
 * mover::usage()
 */
void mover::
usage(FILE *fs) const
{
	fprintf( fs,
		 catgets(_ttcatd, 8, 5,
			 "Usage: %s [-] [-fL] path1 path2\n"
			 "       %s [-] [-fL] path1 [path2 ...] dir\n"
			 "       %s -v\n"
			 "       %s -h\n"),
		 (char *)_prog_name, (char *)_prog_name, (char *)_prog_name,
		 (char *)_prog_name );
	fprintf( fs,
		 catgets(_ttcatd, 8, 6,
			 "\t-L      do not perform a mv(1)\n"
			 "\t-v      print the version number and quit\n"
			 "\t-h      print this message\n" ));
}
