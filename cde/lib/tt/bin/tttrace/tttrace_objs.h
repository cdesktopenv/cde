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
/*%%  $XConsortium: tttrace_objs.h /main/3 1995/10/20 17:02:51 rswiston $ 			 				 */
/*
 * @(#)tttrace_objs.h	1.2 93/11/04
 *
 * Copyright (c) 1993 by Sun Microsystems, Inc.
 */

#include "util/tt_string.h"

#define NO_FORM 0
#define NOSCRIPT_FORM 1
#define SCRIPT_FORM 2

#define FORK_COMMAND 0
#define SESSION_TRACE 1

#define MAXARGS 256

#define	argstr "0FCao:e:f:S:"

class _Tt_trace_optobj : public _Tt_object {

    public:

	// If msg_tracing is 0, only do client tracing.  If 1, then
	// do message and client tracing

	
	int	msg_tracing();

	// If follow is 1, then trace all children forked
	
	int	follow();

	// If api_calls is 1, trace client calls into the TT API

	int	api_calls();

	// if all_attrs is 1, use long form

	int	all_attrs();

	// if script returns 1, there is a script specified, and
	// the text of the script will be in script_string.
	// if script returns 2, the script is a filename

	int	script(_Tt_string& script_string);

	// if outfile returns 1, there is an outfile specified, and
	// its value will be in filename

	int	outfile(_Tt_string& filename);

	// if session returns 1, there is a session specified, and
	// its value will be in session_string

	int	session(_Tt_string& session_string);

	// if command returns 1, there is a command specified, and
	// its value will be in command_string

	int	command(_Tt_string& command_string);

	// argv for command

	char**	cargv();

	// envstr returns a string suitable for putenv(), which is
	// used if a command is forked
	
	_Tt_string	envstr();

	// Name of named pipe returned by tempnam

	_Tt_string&	pipe_name();

	// constructor

	_Tt_trace_optobj();

	// do command-line processing

	int	getopts(int argc, char** argv);

	// tttrace operates in two rundamental modes:
	//  - fork command, using specified script or script file,
	//    or command-line args as the script
	//  - send session_trace requests, using specified script or script file

	int		operation_mode();

    private:



	int			_form;		// discriminates allowed options
	int			_msg_tracing;
	int			_follow;
	int			_api_calls;
	int			_all_attrs;
	int			_has_script_inline;
	int			_has_script_filename;
	_Tt_string		_inline_or_filename;
	int			_has_outfile;
	_Tt_string		_outfile;
	int			_has_session;
	_Tt_string		_session;
	int			_has_command;
	_Tt_string		_command;
	char*			_cargv[MAXARGS];
	_Tt_string		_envstr;
	_Tt_string		_pipenm;


        int			mkenvstr();	// makes string for putenv();
};
