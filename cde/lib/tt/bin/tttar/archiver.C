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
//%%  $XConsortium: archiver.C /main/3 1995/10/20 16:59:21 rswiston $ 			 				
/* @(#)archiver.C	1.17 93/07/30 
 * archiver.cc - ToolTalk wrapper for tar(1).
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 */

#include "tt_options.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "api/c/tt_c.h"
#include "util/copyright.h"
#include "util/tt_gettext.h"
#include "tttar_utils.h"
#include "tttar_file_utils.h"
#include "tttar_api.h"
#include "archiver.h"

/*
 * External variables
 */

/*
 * archiver::archiver()
 */
archiver::
archiver( char *arg0 )
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
	} else {
		_process_name = "(No process name)";
		_prog_name = "(No program name)";
	}
	_mode			= NO_MODE;
	_verbosity		= 0;
	_follow_symlinks	= FALSE;
	_recurse		= TRUE;
	_archive_links		= TRUE;
	_archive_external_links	= TRUE;
	_preserve_modes		= FALSE;
	_preserve__props	= TRUE;
	_should_tar		= TRUE;
	_only_1_look_at_tarfile	= FALSE;
	_io_stream		= NULL;
	_paths2tar		= new _Tt_string_list;
	_renamings		= new Lstar_string_map_list;
}

archiver::
~archiver()
{
}

/*
 * archiver::do_tar() - Invoke tar(1) using system()
 *	If _mode is CREATE,  do_tttarfile means "also archive tttarfile".
 *	If _mode is EXTRACT, do_tttarfile means "only extract tttarfile".
 */
int archiver::
do_tar( _Tt_string tttardir, bool_t do_tttarfile )
{
	char		_curdir[ MAXPATHLEN+1 ];
	_Tt_string	curdir;
	int		status2return;
	_Tt_string	cmd( "tar " );
	_Tt_string	exclude_file;

	/*
	 * Further down, we use the X option to exclude the tttarfile when
	 * extracting so we can extract it into a different place.
	 * Unfortunately AIX and HPUX don\'t have the X option, so
	 * their users will just have to suffer with having the extra
	 * tttarfile appear when doing extracting, and we hope we
	 * have permission to do so..
	 */

#if !defined(OPT_TAR_HAS_EXCLUDE_OPTION)
	_only_1_look_at_tarfile = TRUE;
#endif	
	/*
	 * If we're extracting just the tttarfile, chdir() to
	 * a place where we can extract it with permission and
	 * without collisions.
	 */
	if (do_tttarfile && (_mode == EXTRACT)) {
#if !defined(OPT_BUG_SUNOS_4)
                curdir =  getcwd( _curdir, MAXPATHLEN );
#else
		curdir = getwd( _curdir );
#endif
		int status = chdir( (char *)tttardir );
		if (status != 0) {
			return status;
		}
	}
	switch (_mode) {
	    case CREATE:
		cmd = cmd.cat( "c" );
		break;
	    case EXTRACT:
		cmd = cmd.cat( "x" );
		break;
	    case LIST:
		cmd = cmd.cat( "t" );
		break;
	    case NO_MODE:
	    default:
		return 1;
	}
	for (int n = 0; n < _verbosity; n++) {
		cmd = cmd.cat( "v" );
	}
	if (_follow_symlinks) {
		cmd = cmd.cat( "h" );
	}
	if (_preserve_modes) {
		cmd = cmd.cat( "p" );
	}
	if (_tarfile_arg.len() != 0) {
		cmd = cmd.cat( "f" );
	}
	/*
	 * If we're extracting and we don't want the tttarfile, we
	 * need to pass the X option to exclude the tttarfile.
	 * But if we're extracting from stdin, we only get one chance
	 * to read the input, so in that case we can't exclude the
	 * tttarfile from being extracted.
	 */

	if ((_mode == EXTRACT) && (! do_tttarfile) && (!_only_1_look_at_tarfile))
	{
		cmd = cmd.cat( "X" );
	}
	if (_tarfile_arg.len() != 0) {
		cmd = cmd.cat( " " );
		/*
		 * If we've chdir()'d to our temp dir to extract the
		 * tttarfile, then be sure to prepend the old cwd
		 * to the tarfile's name if it's relative.
		 */
		if (    do_tttarfile
		     && (_mode == EXTRACT)
		     && (_tarfile_arg[0] != '/'))
		{
			cmd = cmd.cat( curdir ).cat( "/" );
		}
		cmd = cmd.cat( _tarfile_arg );
	}
	_Tt_string_list_cursor pathc( _paths2tar );
	switch (_mode) {
	    case CREATE:
		while (pathc.next()) {
			cmd = cmd.cat( " " ).cat( *pathc );
		}
		/*
		 * Have tar dip into our temporary directory and
		 * pick up the tttarfile.
		 */
		if (do_tttarfile) {
			cmd = cmd.cat( " -C " ).cat( tttardir ).
				cat( " tttarfile" );
		}
		break;
	    case EXTRACT:
		if (do_tttarfile) {
			/*
			 * Only extract the tttarfile.
			 */
			cmd = cmd.cat( " tttarfile" );
		} else {
			if (_only_1_look_at_tarfile) {
				/*
				 * If we're extracting from stdin, we
				 * can only invoke tar(1) once, so
				 * hope we have write permission
				 * on the current directory, 'cuz that's
				 * where we've got to put the tttarfile.
				 * Only explicitly ask for the tttarfile
				 * if we are being picky in what we extract.
				 */
				if (_paths2tar->count() > 0) {
					cmd = cmd.cat( " tttarfile" );
				}
			} else {
				/*
				 * Extract everything _but_ the tttarfile.
				 */
				exclude_file = tttardir.cat( "/tar.exclude" );
				FILE *fp = fopen( (char *)exclude_file, "w" );
				if (fp == NULL) {
					cmd = cmd.cat( " /dev/null" );
				} else {
					fprintf( fp, "tttarfile\n" );
					fclose( fp );
					cmd = cmd.cat( " " ).cat( exclude_file );
				}
			}
			while (pathc.next()) {
				cmd = cmd.cat( " " ).cat( *pathc );
			}
		}
		break;
	    case LIST:
		break;
	    case NO_MODE:
	    default:
		return 1;
	}
	//printf( "Invoking: %s\n", (char *)cmd );
	int sys_status = system( (char *)cmd );
	if (WIFEXITED(sys_status)) {
		status2return = WEXITSTATUS(sys_status);
	} else {
		fprintf( stderr,
			 "%s: system(\"%s\"): %d\n",
			 (char *)_process_name, (char *)cmd, sys_status );
		status2return = 1;
	}
	if (_mode == EXTRACT) {
		int status;
		if (do_tttarfile) {
			status = chdir( (char *)curdir );
			if (status != 0) {
				fprintf( stderr, "%s: chdir(\"%s\"): %s\n",
					(char *)_process_name,
					(char *)curdir, strerror(errno) );
				exit( status );
			}
		} else if (exclude_file.len() > 0) {
			status = unlink( (char *)exclude_file );
			if (status != 0) {
				perror( (char *)exclude_file );
			}
		}
	}
	return status2return;

} /* archiver::do_tar() */

/*
 * archiver::do_tttar() - Perform just the LS/TT part of tttar
 */
bool_t archiver::
do_tttar( char *tttarfile_name, bool_t silent )
{
	char		       *process_id;
	int		        first_ttmalloc;
	XDR			xdrs;
	bool_t			val2return = TRUE;
	char			_curdir[ MAXPATHLEN+1 ];
	_Tt_string		curdir;

	_io_stream = this->_open_io_stream( tttarfile_name, silent );
	if (_io_stream == NULL) {
		return FALSE;
	}
	switch (_mode) {
	    case CREATE:
		xdrstdio_create( &xdrs, _io_stream, XDR_ENCODE );
		break;
	    case LIST:
	    case EXTRACT:
		xdrstdio_create( &xdrs, _io_stream, XDR_DECODE );
		break;
	    case NO_MODE:
	    default:
		fprintf( stderr, "%s: Archive_mode: %d\n",
			 (char *)_process_name, (int)_mode );
		return FALSE;
	}

	/*
	 * Tooltalk setup
	 */
	first_ttmalloc = tt_mark();
	note_ptr_err( tt_open() );
	if (IS_TT_ERR(err_noted)) {
		return FALSE;
	}
	process_id = ptr_returned;

	switch (_mode) {
	    case CREATE:
		val2return = pathlist_lstt_archive(
				_paths2tar, _recurse, _follow_symlinks,
				_verbosity, &xdrs );
		break;
	    case LIST:
		val2return = pathlist_lstt_archive_list(_paths2tar, _verbosity,
							&xdrs);
		break;
	    case EXTRACT:
#if !defined(OPT_BUG_SUNOS_4)
		curdir = getcwd( _curdir, MAXPATHLEN );
#else
		curdir = getwd( _curdir );
#endif
		val2return = pathlist_lstt_dearchive( _paths2tar, _renamings,
						      curdir, _preserve__props,
						      _verbosity, &xdrs);
		break;
	    case NO_MODE:
	    default:
		break;
	}
	
	/*
	 * Tooltalk teardown
	 */
	note_err( tt_close() );
	my_tt_release( first_ttmalloc );

	xdr_destroy( &xdrs );
	if ((_io_stream != stdin) && (_io_stream != stdout)) {
		fclose( _io_stream );
	}
	return val2return;
}

/*
 * archiver::parse_args()
 */
void archiver::
parse_args( int argc, char **argv )
{
	bool_t	next_arg_is_tarfile	= FALSE;

	for ( int argnum = 1; argnum < argc; argnum++ ) {
		char *cp = argv[argnum];
		_Tt_string arg( cp );
		if (argnum == 1) {
			/*
			 * Process mode setting and other flags.
			 */
			while (*cp != '\0') {
				switch (*cp) {
				    case 'c':
				    case 't':
				    case 'x':
					if (_mode == NO_MODE) {
						switch (*cp) {
						    case 'c':
							_mode = CREATE;
							break;
						    case 't':
							_mode = LIST;
							break;
						    case 'x':
							_mode = EXTRACT;
							break;
						}
					} else {
						this->usage();
						exit(1);
					}
					break;
				    case '-':
					cp++;
					switch (*cp) {
					    case 'v':
						_TT_PRINT_VERSIONS
						          ((char *)_prog_name)
						exit(0);
						break;
					    case 'h':
						this->usage();
						exit(0);
						break;
					    default:
						this->usage();
						exit(1);
						break;
					}
					break;
				    case 'f':
					next_arg_is_tarfile = TRUE;
					break;
				    case 'R':
					_recurse = FALSE;
					break;
				    case 'h':
					_follow_symlinks = TRUE;
					break;
				    case 'p':
					_preserve_modes = TRUE;
					break;
				    case 'P':
					_preserve__props = FALSE;
					break;
				    case 'S':
					_archive_links = FALSE;
					break;
				    case 'E':
					_archive_external_links = FALSE;
					break;
				    case 'L':
					_should_tar = FALSE;
					break;
				    case 'v':
					_verbosity++;
					break;
				    default:
					this->usage();
					exit(1);
				}
				cp++;
			}
		} else if (next_arg_is_tarfile) {
			_tarfile_arg = cp;
			next_arg_is_tarfile = FALSE;
		} else if (arg == "-rename") {
			Lstar_string_map_ptr m = new Lstar_string_map;

			if (++argnum >= argc) {
				this->usage();
				exit(1);
			} else {
				m->old_string_set( argv[argnum] );
				if (++argnum >= argc) {
					this->usage();
					exit(1);
				} else {
					m->new_string_set( argv[argnum] );
					_renamings->append( m );
				}
			}
		} else {
			/*
			 * Add this pathname to the list.
			 */
			_Tt_string path = cp;
			_paths2tar->append( path );
		}
	}
	if (    ((_tarfile_arg.len() == 0) && (! _should_tar))
	     || (_mode == NO_MODE)
	     || (_should_tar && (! _recurse)))
	{
		this->usage();
		exit(1);
	}
	_only_1_look_at_tarfile =
		(_tarfile_arg.len() == 0) || (_tarfile_arg == "-");
	/*
	 * Renaming as you dearchive is not a tar(1) option.
	 */
	if (    (_renamings->count() > 0)
	     && (_should_tar || (_mode != EXTRACT)))
	{
		this->usage();
		exit(1);
	}
}

/*
 * archiver::_open_io_stream() - Returns an file pointer opened according
 *	to _mode.  Uses stdin/stdout if filename is NULL or "-".
 *	Exits with an error message if the _mode is not set,
 *	or if the named file cannot be opened.
 */
FILE * archiver::
_open_io_stream(char *filename, bool_t silent)
{
	bool_t	in_pipe;
	FILE   *fp = NULL;

	in_pipe = (    (filename == NULL)
		    || (strcmp( filename, "-" ) == 0));
	switch (_mode) {
	    case CREATE:
		if (in_pipe) {
			return stdout;
		} else {
			fp = fopen( filename, "w" );
		}
		break;
	    case EXTRACT:
	    case LIST:
		if (in_pipe) {
			return stdin;
		} else {
			fp = fopen( filename, "r" );
		}
		break;
	    case NO_MODE:
	    default:
		fprintf( stderr, "%s: Archive_mode: %d\n",
			 (char *)_process_name, (int)_mode );
	}
	if ((fp == NULL) && (! silent)) {
		fprintf( stderr, "%s: %s: %s\n",
			 (char *)_process_name, filename, strerror(errno) );
	}
	return fp;

} /* archiver::_open_io_stream() */

/*
 * archiver::usage()
 */
void archiver::
usage( FILE *fs ) const
{
	fprintf( fs,
		 catgets(_ttcatd, 7, 2,
			 "Usage: %s {ctx}[fhpPv[v]] [tarfile] pathname ...\n"
			 "       %s {ctx}fL[hpPRv[v]] tttarfile pathname ...\n"
			 "       %s -v\n"
			 "       %s -h\n"),
		 (char *)_prog_name, (char *)_prog_name, (char *)_prog_name,
		 (char *)_prog_name );
	fprintf( fs, "%s",
		 catgets(_ttcatd, 7, 3,
			 "\tc       create an archive\n"
			 "\tt       list an archive's contents\n"
			 "\tx       extract from an archive\n"
			 "\tf       use next arg <tarfile> as archive\n"
			 "\th       follow symbolic links\n"
			 "\tL       do not invoke tar(1)\n"
			 "\tp       preserve file modes\n"
			 "\tP       (root) do not preserve objects' "
			            "owner, mod time, etc.\n"
			 "\tR       do not recurse into directories\n"
			 "\tv       be verbose\n"
			 "\tvv      be very verbose\n"
			 "\t-v      print the version number and quit\n"
			 "\t-h[elp] print this message\n"));
}
