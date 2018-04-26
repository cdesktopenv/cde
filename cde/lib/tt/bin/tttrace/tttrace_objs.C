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
//%%  $XConsortium: tttrace_objs.C /main/3 1995/10/20 17:02:43 rswiston $ 			 				
/*
 * @(#)tttrace_objs.C	1.4 93/11/04
 *
 * Copyright (c) 1993 by Sun Microsystems, Inc.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "tttrace_objs.h"
#include "util/tt_trace.h"

_Tt_trace_optobj::_Tt_trace_optobj()
{
	_msg_tracing = 1;	// trace messages and calls by default
	_follow = 0;		// follow off by default
	_api_calls = 1;		// trace calls into API by default
	_all_attrs = 0;		// short form by default
	_has_script_inline = 0;
	_has_script_filename = 0;
	_inline_or_filename = "version 1 ";
	_has_outfile = 0;
	_has_session = 2;		// 1 -> -S option, 2 -> set by default
	_has_command = 0;
	_form = NO_FORM;
	for(int i = 0; i < MAXARGS; i++) {
		_cargv[i] = NULL;
	}
}

int
_Tt_trace_optobj::getopts(int argc, char** argv)
{
	int		c;			// command-line option char
	int		i;

	extern char* 	optarg;			// command-line option
	extern int	optind;			// option index into argv
	extern int	opterr;			// getopt error value

	_Tt_string	tmpstr;
	_Tt_string 	tmppath;
	_Tt_string	_tt_realpath(const _Tt_string&);

	_form = NO_FORM;

	// needed by the getopt call
	opterr = 0;
	while ((c = getopt(argc, argv, argstr)) != -1) {
		switch (c) {
		    case '0':
			// Turn off msg tracing in <session> or run <command>
			// without message tracing
			if (_form == SCRIPT_FORM) return 1;
			_form = NOSCRIPT_FORM;
			_msg_tracing = 0;
			break;
		    case 'F':
			// Follow children
			if (_form == SCRIPT_FORM) return 1;
			_form = NOSCRIPT_FORM;
			_follow = 1;
			break;
		    case 'C':
			// Do not trace client calls into the TT API
			if (_form == SCRIPT_FORM) return 1;
			_form = NOSCRIPT_FORM;
			_api_calls = 0;
			break;
		    case 'a':
			// verbose form
			if (_form == SCRIPT_FORM) return 1;
			_form = NOSCRIPT_FORM;
			_all_attrs = 1;
			break;
		    case 'e':
			// command-line script
			if (_form == NOSCRIPT_FORM) return 1;
			_form = SCRIPT_FORM;

			if (!_has_script_filename) {
				_inline_or_filename = optarg;
				_has_script_inline = 1;
			}
			else {
				return 1;
			}
			break;
		    case 'f':
			// script file
			if (_form == NOSCRIPT_FORM) return 1;
			_form = SCRIPT_FORM;

			if (!_has_script_inline) {

				struct stat sbuf;

				tmppath = optarg;
				tmpstr = _tt_realpath(tmppath);
				_inline_or_filename = tmpstr;
				if (stat(_inline_or_filename, &sbuf) != 0) {
					fprintf(stderr, "ttrace: %s: %s\n",
						(char *)_inline_or_filename,
						strerror(errno));
					return 2;
				}
				_has_script_filename = 1;
				
			}
			else {
				return 1;
			}
			break;
		    case 'o':
			// outfile
			if (_form == SCRIPT_FORM) return 1;
			_form = NOSCRIPT_FORM;
			_outfile = optarg;
			_has_outfile = 1;
			break;
		    case 'S':
			// XXX: Note that a Session_Trace message *always*
			// gets sent when this option is given, so we have
			// to do a tt_open somewhere, and it may as well be
			// here, so we can do a tt_default_session if
			// necessary.

			Tt_status tts;
			char* procid;
	
			_session = optarg;
			tts = tt_default_session_set(_session);
			if (tts != TT_OK) {
				fprintf(stderr, "tttrace: "
					"tt_default_session_set:\n%s\n",
					tt_status_message(tts));
				exit(1);
			}

			procid = tt_open();
			tts = tt_ptr_error(procid);
			if (tts != TT_OK) {
				fprintf(stderr, "tttrace: tt_open:\n%s\n",
					tt_status_message(tts));
				exit(1);
			}

			_has_session = 1;
			break;
		    case '?':
		    default:
			// error or help request.
			return 1;
		}
	}

	if (optind < argc) {

		// Command given after options

		if (_has_session == 2) {

			// Override the default
			_has_session = 0;
		}
		else if (_has_session == 1) {

			// Can't give session and command together
		        return 1;
		}

		// command given after options

		_command = argv[optind++];
		_cargv[0] = (char *) _command;
		for (i = 1; optind < argc; ++i, ++optind) {
			_cargv[i] = argv[optind];
		}
		_cargv[i] = (char *) 0;
		_has_command = 1;
		
	} else if (_has_session == 2) {

		// default -- tttrace with no session or command args

		(void) tt_open();
		_session = tt_default_session();
	}

	if ((! _has_outfile) || (_outfile == "-")) {

		// We need to set up a FIFO to stuff the output
		// into, and give the FIFO name to the command/session
				
		_pipenm = tempnam(NULL, "trace");
		if (mkfifo(_pipenm, S_IWUSR|S_IRUSR) == -1) {
			fprintf(stderr, "tttrace: mkfifo(\"%s\"): %s\n",
				(char *)_pipenm, strerror(errno));
			exit(2);
		}
       	}

	mkenvstr();

	if (_form == NO_FORM) {
		_has_script_inline = 1;
		_form = SCRIPT_FORM;
	}
	
	return 0;
}

int
_Tt_trace_optobj::msg_tracing()
{
	return _msg_tracing;
}

int
_Tt_trace_optobj::follow()
{
	return _follow;
}

int
_Tt_trace_optobj::api_calls()
{
	return _api_calls;
}

int
_Tt_trace_optobj::all_attrs()
{
	return _all_attrs;
}

int
_Tt_trace_optobj::script(_Tt_string& script_string)
{
	int ret = 0;

	if (!_msg_tracing) {

		// _msg_tracing is 1 by default, so if it's zero here, it
		// means the -0 option was given, so whatever else was
		// specified by the command line is overridden by this

		script_string = "states none";
		return 1;
	}
	
	if (_has_script_inline) {
		ret = 1;
	}
	else if (_has_script_filename) {
		ret = 2;
	}

	script_string = _inline_or_filename;

	return ret;
}

int
_Tt_trace_optobj::outfile(_Tt_string& filename)
{
	filename = _outfile;
	
	if (_has_outfile) {
		return 1;
	}
	else {
		return 0;
	}
}

int
_Tt_trace_optobj::session(_Tt_string& session_string)
{
	session_string = _session;
	
	if (_has_session) {
		return 1;
	}
	else {
		return 0;
	}
}

int
_Tt_trace_optobj::command(_Tt_string& command_string)
{
	command_string = _command;
	
	if (_has_command) {
		return 1;
	}
	else {
		return 0;
	}
}

char**
_Tt_trace_optobj::cargv()
{
	return _cargv;
}

int
_Tt_trace_optobj::mkenvstr()
{
	char *val = getenv(TRACE_SCRIPT);
	if ((val != 0) && (_form == NO_FORM)) {
		_envstr = val;
		if (_inline_or_filename[0] == '/' ||
		    _inline_or_filename[0] == '.') {

			_form = NOSCRIPT_FORM;	// filename
		}
		else {

			_form = SCRIPT_FORM;	// inline script
		}
		return 1;
	}
	_envstr = TRACE_SCRIPT;
	_envstr = _envstr.cat("=");

	if (! _has_script_filename) {
		//
		// The script will be inline, instead of in a file
		//
		if ((! _has_outfile) || (_outfile == "-")) {
			//
			// No -o, so prepend "> pipe; " onto script
			//
			if (_pipenm.len() > 0) {
				_Tt_string output_cmd = "> ";
				output_cmd = output_cmd.cat(_pipenm).cat("; ");
				_inline_or_filename =
					output_cmd.cat( _inline_or_filename );
			}
		} else {
			//
			// -o option overrides ">" in script, so
			// append "; > outfile" to script
			//
			if (_outfile.len() > 0) {
				_Tt_string output_cmd = "> ";
				output_cmd = output_cmd.cat(_outfile);
				if (_inline_or_filename.len() > 0) {
					_inline_or_filename =
						_inline_or_filename.cat("; ");
				}
				_inline_or_filename =
					_inline_or_filename.cat( output_cmd );
			}
		}
		// 
	} else {
		// XXX if both -o and -f, we need to read the file!
	}
	if (!_has_script_inline && !_has_script_filename) {
		if (! _msg_tracing) {
			_inline_or_filename =
				_inline_or_filename.cat("; states none");
		}

		if (_follow) {
			_inline_or_filename =
				_inline_or_filename.cat("; follow on");
		}

		if (!_api_calls) {
			_inline_or_filename =
				_inline_or_filename.cat("; functions none");
		}

		if (_all_attrs) {
			_inline_or_filename =
				_inline_or_filename.cat("; attributes all");
		}

		_has_script_inline = 1;
	}
	
	_envstr = _envstr.cat(_inline_or_filename);
	return 1;
}

int
_Tt_trace_optobj::operation_mode()
{
	if (_has_session)
		return SESSION_TRACE;
	else if (_has_command)	// _has_command
		return FORK_COMMAND;
	else
		return -1;	// error
}

_Tt_string
_Tt_trace_optobj::envstr()
{
	return _envstr;
}

_Tt_string&

_Tt_trace_optobj::pipe_name()
{
	return _pipenm;
}


