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
//%%  $XConsortium: rcopier.C /main/3 1995/10/20 16:36:32 rswiston $ 			 				
/*
 * rcopier.cc - Link Service/ToolTalk wrapper for rcp(1).
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 */

#include "tt_options.h"
#include <string.h>
#include <osfcn.h>
#if defined(OPT_BUG_SUNOS_4)
#	include <stdlib.h>
#endif
#include <api/c/tt_c.h>
#include <util/tt_path.h>
#include <util/tt_enumname.h>
#include <util/copyright.h>
#include "rcopier.h"

/*
 * External variables
 */

implement_list_of(rpath)

/*
 * rpath::rpath()
 */
rpath::
rpath()
{
}

/*
 * rpath::~rpath()
 */
rpath::
~rpath()
{
}

/*
 * rpath::rpath()
 */
rpath::
rpath( _Tt_string)
{
	/*
	 * host:path
	 * user@host:path
	 */
}

/*
 * rcopier::rcopier()
 */
rcopier::
rcopier( char *arg0 )
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
	_from_paths = new rpath_list();
	_should_rcp = TRUE;
	_recurse = FALSE;
	_preserve = FALSE;
	_tt_opened = FALSE;
}

rcopier::
~rcopier()
{
}

/*
 * rcopier::do_rcp() - Use system() to invoke rcp(1), and return its exit status.
 *	We can just use _args, since we never get here when our one
 *	rcp-incompatible option (-L) has been given.
 */
int rcopier::
do_rcp()
{
	_Tt_string		cmd( "rcp" );
	_Tt_string_list_cursor	arg_cursor( _args );

	while (arg_cursor.next()) {
		cmd = cmd.cat( " " ).cat( *arg_cursor );
	}
	printf( "Invoking: %s\n", (char *)cmd );
	int sys_status = system( (char *)cmd );
	if (WIFEXITED(sys_status)) {
		return WEXITSTATUS(sys_status);
	} else {
		fprintf( stderr,
			 "%s: I invoked rcp(1), but system() returned "
			 "%d, which is not an exit status!\n",
			 (char *)_process_name, sys_status );
		return 1;
	}
}

/*
 * rcopier::do_ttrcp() - Use tttar(1) to copy the objects of the _from_paths.
 *
 * Algorithm:
 *
 * [rsh fromhost -l user] tttar cfhL - frompath |
 * [rsh tohost -l user] tttar xfL - -rename frompath topath
 *
 * TO_DO: this won't work if topath is a directory.  If it's a
 * remote directory, how do we find this out?
 * }
 */
Tt_status rcopier::
do_ttrcp()
{
	rpath_list_cursor	from_path_cursor( _from_paths );

	/*
	 * TO_DO: tt_file_destroy() any paths that rcp(1) will delete
	 */
	while (from_path_cursor.next()) {
		_Tt_string	cmd;
		rpath_ptr      	rp = *from_path_cursor;

		if (rp->host().len() >= 0) {
			cmd = cmd.cat( "rsh ").cat( rp->host()).cat( " -l ")
				 .cat( _username ).cat( " " );
		}
		cmd = cmd.cat( "tttar cfhL - " ).cat( rp->path()).cat(" | ");
		if (_to_path->host().len() >= 0) {
			cmd = cmd.cat( "rsh ").cat( _to_path->host())
				 .cat( " -l ").cat( _username ).cat( " " );
		}
		cmd = cmd.cat( "tttar xfL" );
		if (_preserve) {
			cmd = cmd.cat( "p" );
		}
		cmd = cmd.cat( " - -rename " ).cat( rp->path())
			 .cat( _to_path->path());
		printf( "Invoking: %s\n", (char *)cmd );
		int sys_status = system( (char *)cmd );
		if (WIFEXITED(sys_status)) {
			if (WEXITSTATUS(sys_status) != 0) {
				return TT_ERR_INTERNAL;
			}
		} else {
			fprintf( stderr,
				 "%s: invoked tttar(1), but system() returned "
				 "%d, which is not an exit status!\n",
				 (char *)_process_name, sys_status );
			return TT_ERR_INTERNAL;
		}
	}
	return TT_OK;

} /* do_ttrcp() */

/*
 * rcopier::open_tt()
 */
Tt_status rcopier::
open_tt()
{
	char *process_id = tt_open();
	Tt_status err = tt_ptr_error( process_id );
	if (err == TT_OK) {
		_process_id = process_id;
		_tt_opened = TRUE;
	} else if (err > TT_WRN_LAST) {
		fprintf( stderr,
			 "%s: Could not initialize ToolTalk because %s\n",
			 (char *)_process_name,
#ifndef TT_STATUS_MSG_TO_DO
			 _tt_enumname( (Tt_status)err )
#else
			 tt_status_message( err )
#endif
		       );
	}
	return err;
}

/*
 * rcopier::close_tt()
 */
Tt_status rcopier::
close_tt()
{
	if (! _tt_opened) {
		return TT_OK;
	}
	Tt_status err = tt_close();
	if (err > TT_WRN_LAST) {
		fprintf( stderr,
			 "%s: Could not close ToolTalk because %s\n",
			 (char *)_process_name,
#ifndef TT_STATUS_MSG_TO_DO
			 _tt_enumname( (Tt_status)err )
#else
			 tt_status_message( err )
#endif
		       );
	}
	return err;
}

/*
 * rcopier::parse_args()
 */
void rcopier::
parse_args( int argc, char **argv )
{
	for ( int arg_num = 1; arg_num < argc; arg_num++ ) {
		_Tt_string arg( argv[arg_num] );
		_args->append( arg );
		if (arg[0] == '-') {
			this->_parse_arg( (char *)arg );
		} else {
			if (arg_num == argc - 1) {
				_to_path = new rpath( arg );
			} else {
				rpath_ptr rp = new rpath( arg );
				_from_paths->append( rp );
			}
		}
	}
	if (_to_path->path().len() <= 0) {
		this->usage();
		exit(1);
	}
	if (_from_paths->count() <= 0) {
		this->usage();
		exit(1);
	}
	if (_from_paths->count() > 1) {
		/*
		 * If multiple things to move, the place we're
		 * moving them to must be a directory.
		 */
		if (_to_path->host().len() <= 0) {
			/*
			 * to_path is local.  Yay!
			 */
			struct stat stat_buf;

			if (stat( (char *)_to_path->path(), &stat_buf) != 0) {
				fprintf( stderr, "%s: %s: ",
					 (char *)_process_name,
					 (char *)_to_path->path() );
				perror(NULL);
				exit(2);
			}
			if (! S_ISDIR(stat_buf.st_mode)) {
				fprintf( stderr, "%s: %s is not a directory\n",
					 (char *)_process_name,
					 (char *)_to_path->path() );
				this->usage();
				exit(2);
			}
			_to_path_is_dir = TRUE;
		} else {
			/*
			 * TO_DO: figure out if remote path is dir or not
			 */
		}
	} else {
		if (_to_path->host().len() <= 0) {
			struct stat stat_buf;

			_to_path_is_dir = FALSE;
			if (stat( (char *)_to_path->path(), &stat_buf) == 0) {
				_to_path_is_dir = S_ISDIR(stat_buf.st_mode);
			} 
			/*
			 * I don't think rcp has a clonedir mode. (see ttcp)
			else {
				_Tt_string from_path = _from_paths->top();
				if (    (_from_paths->count() == 1)
				     && (stat( (char *)from_path, &stat_buf) == 0)
				     && S_ISDIR(stat_buf.st_mode))
				{
					_clonedir_mode = TRUE;
					_to_path_is_dir = TRUE;
				}
			}
			 */
		} else {
			/*
			 * TO_DO: figure out if remote path is dir or not
			 */
		}
	}

} /* parse_args() */

/*
 * rcopier::_parse_arg() - Parse an option 
 */
void rcopier::
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
		    case 'L':
			_should_rcp = FALSE;
			break;
		    case 'p':
			_preserve = TRUE;
			break;
		    case 'r':
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
 * rcopier::usage()
 */
void rcopier::
usage(FILE *fs) const
{
	fprintf( fs,
		 "Usage: %s [-pL] file1 file2\n"
		 "       %s [-prL] path1 [path2 ...] dir\n"
		 "       %s -v\n"
		 "       %s -h\n",
		 (char *)_prog_name, (char *)_prog_name, (char *)_prog_name,
		 (char *)_prog_name );
	fprintf( fs, "\t-L      do not perform a rcp(1)\n" );
	fprintf( fs, "\t-v      print the version number and quit\n" );
	fprintf( fs, "\t-h[elp] print this message\n" );
}
