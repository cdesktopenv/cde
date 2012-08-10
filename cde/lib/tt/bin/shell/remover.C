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
//%%  $TOG: remover.C /main/5 1999/10/14 18:37:42 mgreess $ 			 				
/*
 * remover.cc - ToolTalk wrapper for rm(1) and rmdir(1).
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
#include "util/tt_path.h"
#include "util/tt_gettext.h"
#include "util/copyright.h"
#include "remover.h"

/*
 * External variables
 */

/*
 * remover::remover()
 */
remover::
remover( char *arg0 )
{
	if (arg0 != NULL) {
		char *base = strrchr( arg0, '/' );
		if (base == NULL) {
			base = arg0;
		} else {
			base++;
		}
		_prog_name = base;
		_process_name = _prog_name;
	}
	_am_rmdir = (_prog_name == "ttrmdir");
	_args = new _Tt_string_list();
	_paths = new _Tt_string_list();
	_should_rm = TRUE;
	_force = FALSE;
	_recurse = FALSE;
	_tt_opened = FALSE;
}

remover::
~remover()
{
}

/*
 * remover::do_rm() - Invoke rm[dir](1), and return its exit status.
 *	We can just use _args, since we never get here when our one
 *	incompatible option (-L) has been given.
 */
int remover::
do_rm()
{
	_Tt_string		cmd;
	_Tt_string_list_cursor	arg_cursor( _args );

	if (_am_rmdir) {
		cmd = "rmdir";
	} else {
		cmd = "rm";
	}
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
 * remover::do_ttrm() - Use tt_file_destroy() on the paths to destroy.
 */
Tt_status remover::
do_ttrm()
{
	return this->_ttrm_paths( _paths );
}

/*
 * remover::ttrm_path() - tt_file_destroy() this path.
 */
Tt_status remover::
_ttrm_paths( _Tt_string_list_ptr paths )
{
	Tt_status	worst_err = TT_OK;
	Tt_status	err;
	bool_t		abort = FALSE;

	while ((! paths->is_empty()) && (! abort)) {
		_Tt_string		path     = paths->top();
		_Tt_string_list_ptr	children;

		paths->pop();
		if (! this->can_rm( path )) {
			continue;
		}
		if (_recurse) {
			_Tt_string_list_ptr children = _tt_dir_entries(path,
								       FALSE);
			err = this->_ttrm_paths( children );
			if (err > TT_WRN_LAST) {
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
		if (! abort) {
			err = tt_file_destroy( (char *)path );
			if (err > TT_WRN_LAST) {
				worst_err = err;
				if (! _force) {
					fprintf( stderr,
						 catgets(_ttcatd, 8, 7,
							 "%s: Could not remove "
							 "ToolTalk objects of "
							 "%s because %s\n"),
						 (char *)_process_name,
						 (char *)path,
						 tt_status_message(err) );
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
	}
	return worst_err;

} /* ttrm_paths() */

/*
 * remover::can_rm() - Can we remove this path?
 *
 *	TO_DO: For now I'll count on tt_file_destroy() not to let me
 *	destroy the specs in files I can't remove.
 */
bool_t remover::
can_rm( _Tt_string path )
{
	if (_am_rmdir) {
	} else {
		if (! _recurse) {
			struct stat lstat_buf;

			if (lstat( (char *)path, &lstat_buf) == 0) {
				if (S_ISDIR(lstat_buf.st_mode)) {
					/*
					 * rm(1) without the -r flag
					 * won't remove directories,
					 * but tt_file_destroy() will,
					 * so we make sure not to ask
					 * it to.
					 */
					if (! _force) {
						fprintf( stderr, "%s: %s: %s\n",
							 (char *)_prog_name,
							 (char *)path,
							 strerror(EISDIR) );
					}
					return FALSE;
				} else if (S_ISLNK(lstat_buf.st_mode)) {
					/*
					 * Don't tt_file_destroy() a symlink,
					 * or TT will tt_file_destroy() the
					 * linked file.
					 */
					return FALSE;
				} else {
					return TRUE;
				}
			} else {
				return FALSE;
			}
		}
	}
	return FALSE;
}

/*
 * remover::open_tt()
 */
Tt_status remover::
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
 * remover::close_tt()
 */
Tt_status remover::
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
 * remover::parse_args()
 */
void remover::
parse_args( int argc, char **argv )
{
	bool_t no_more_options = FALSE;

	for ( int arg_num = 1; arg_num < argc; arg_num++ ) {
		_Tt_string arg( argv[arg_num] );
		_args->append( arg );
		if (_am_rmdir) {
			if (arg[0] == '-') {
				this->_parse_arg( (char *)arg );
			} else {
				_paths->append( arg );
			}
		} else {
			if ((arg[0] != '-') || no_more_options) {
				_paths->append( arg );
			} else {
				if (arg[1] == '\0') {
					/*
					 * The bare option "-" means take the
					 * subsequent arguments to be paths.
					 */
					no_more_options = TRUE;
				} else {
					this->_parse_arg( (char *)arg );
				}
			}
		}
	}
	if (_paths->count() <= 0) {
		this->usage();
		exit(1);
	}
}

/*
 * remover::_parse_arg() - Parse an option 
 */
void remover::
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
		    case 'f':
			if (_am_rmdir) {
				this->usage();
				exit(1);
			} else {
				_force = TRUE;
			}
			break;
		    case 'r':
		    case 'R':
			if (_am_rmdir) {
				this->usage();
				exit(1);
			} else {
				_recurse = TRUE;
			}
			break;
		    case 'L':
			_should_rm = FALSE;
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

/*
 * remover::usage()
 */
void remover::
usage(FILE *fs) const
{
	fprintf( fs,
		 catgets(_ttcatd, 8, 8,
			 "Usage: %s [-] [-%s] %s ...\n"
			 "       %s -v\n"
			 "       %s -h\n"),
		 (char *)_prog_name,
		 (_am_rmdir ? "L" : "fLrR"),
		 (_am_rmdir ? catgets(_ttcatd, 8, 9, "file")
			: catgets(_ttcatd, 8, 10, "dir") ),
		 (char *)_prog_name, (char *)_prog_name );
	fprintf( fs,
		 catgets(_ttcatd, 8, 11,
			 "\t-L      do not perform a %s(1)\n"
			 "\t-v      print the version number and quit\n"
			 "\t-h[elp] print this message\n" ),
		 (_am_rmdir ? "rmdir" : "rm"));
}
