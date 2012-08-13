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
//%%  $TOG: tt_tracefile_parse.C /main/4 1998/03/19 19:00:04 mgreess $ 			 				
/*
 *
 * tt_trace_parse.C
 *
 * Copyright (c) 1993 by Sun Microsystems, Inc.
 */

#include <ctype.h>
#include "api/c/tt_c.h"
#include "util/tt_string.h"
#include "util/tt_port.h"
#include "util/tt_gettext.h"
#include "util/tt_trace_parse.h"
#include "util/tt_trace_parser.h"
#include "util/tt_entry_pt_names.h"

#if defined (_AIX) || defined(hpux) || defined(__osf__) || defined(linux) || \
	defined(CSRG_BASED)
#include <fcntl.h>
#endif

static int iserr;

static _Tt_trace_tokens get_token(const _Tt_trace_tokens, _Tt_string&,
				  int&, _Tt_string&);
static void print_error(_Tt_string&, int);
static void eat_line(_Tt_string&, int&);
static void put_value(_Tt_trace_parser*, _Tt_trace_tokens,
		      _Tt_trace_tokens, _Tt_string&);
static _Tt_trace_tokens allnone_check(const _Tt_string&);
static _Tt_trace_tokens state_name_to_val(const _Tt_string&);
static int _is_tt_func(const _Tt_string&);

int
_tt_trace_parse(_Tt_string& parse_buf,
		_Tt_trace_parser* trace_parser)
{
	int			buf_ptr = 0;
	_Tt_trace_tokens	token;
	_Tt_trace_tokens	prev = _TT_TRACE_ENDLINE;
	_Tt_trace_tokens	kywrd = _TT_TRACE_ENDFILE;
	_Tt_string		value;

	iserr = 0;
	
	while (token = get_token(prev, parse_buf, buf_ptr, value)) {

		if (token == _TT_TRACE_UNKNOWN) {
			print_error(parse_buf, buf_ptr);
			eat_line(parse_buf, buf_ptr);
			prev = _TT_TRACE_ENDLINE;
			continue;
		}

		switch (token) {
		    case _TT_TRACE_FUNCTIONS:
			trace_parser->clear_functions();
			break;
		    case _TT_TRACE_STATES:
			trace_parser->clear_states();
			break;
		    case _TT_TRACE_OPS:
			trace_parser->clear_ops();
			break;
		    case _TT_TRACE_SENDER_PTYPES:
			trace_parser->clear_sender_ptypes();
			break;
		    case _TT_TRACE_HANDLER_PTYPES:
			trace_parser->clear_handler_ptypes();
			break;
		    default:
			break;
		}

		if (prev == _TT_TRACE_ENDLINE) {
			// An improper keyword here would be
			// flagged in get_token
			kywrd = token;
		}
		else {
			if (token != _TT_TRACE_ENDLINE &&
			    token != _TT_TRACE_ENDFILE) {
				put_value(trace_parser,
					  kywrd, token, value);
			}
			else {
				// Did the EOL or EOF follow
				// a keyword (a no-no!) ?
				if (kywrd == prev) {
					print_error(parse_buf, buf_ptr);
				}
			}
		}

		if (token == _TT_TRACE_ENDFILE) break;

		prev = token;
	}

	return iserr;
}

static _Tt_trace_tokens
get_token(const _Tt_trace_tokens prev,
	  _Tt_string& parse_buf,
	  int& buf_ptr,
	  _Tt_string& value)
{
	int i;
	
	// Eat blanks
	
	while (parse_buf[buf_ptr] == ' ' ||
	       parse_buf[buf_ptr] == '\t') {
		buf_ptr++;
	}

	int beg = buf_ptr;

	// Go to whitespace or EOL

	if (parse_buf[buf_ptr] == ';' ||
	    parse_buf[buf_ptr] == '\n') {
		buf_ptr++;
		return _TT_TRACE_ENDLINE;
	}
	else if (parse_buf[buf_ptr] == '\0') {
		return _TT_TRACE_ENDFILE;
	}
	else if (parse_buf[buf_ptr] == '#') {
		// comment
		eat_line(parse_buf, buf_ptr);
		return _TT_TRACE_ENDLINE;
	}
	else {
		// Must be a real, non-semicolon character
		do {
			buf_ptr++;
		} while (parse_buf[buf_ptr] &&
			 parse_buf[buf_ptr] != ' ' &&
			 parse_buf[buf_ptr] != '\t' &&
			 parse_buf[buf_ptr] != ';' &&
			 parse_buf[buf_ptr] != '\n');
	}

	value = parse_buf.mid(beg, buf_ptr - beg);

	// Try to interpret what type of token "value" is

	switch (prev) {
	    case _TT_TRACE_ENDLINE:
		if (value == "follow") return _TT_TRACE_FOLLOW;
		if (value == ">") return _TT_TRACE_WRITEFILE;
		if (value == ">>") return _TT_TRACE_APPENDFILE;
		if (value == "functions") return _TT_TRACE_FUNCTIONS;
		if (value == "attributes") return _TT_TRACE_ATTRIBUTES;
		if (value == "states") return _TT_TRACE_STATES;
		if (value == "ops") return _TT_TRACE_OPS;
		if (value == "sender_ptypes") return _TT_TRACE_SENDER_PTYPES;
		if (value == "handler_ptypes") return _TT_TRACE_HANDLER_PTYPES;
		if (value == "version") return _TT_TRACE_VERSION;
#ifdef _OPT_TIMERS_SUNOS
		if (value == "timers") return _TT_TRACE_TIMERS;
#endif
		return _TT_TRACE_UNKNOWN;
	    case _TT_TRACE_VERSION:
		for (i = 0; i < value.len(); ++i) {
			if (!isdigit(value[i])) return _TT_TRACE_UNKNOWN;
		}
		return _TT_TRACE_NUMBER;
	    case _TT_TRACE_FOLLOW:
#ifdef _OPT_TIMERS_SUNOS
    case _TT_TRACE_TIMERS:
#endif
		if (value == "on") {
			return _TT_TRACE_ON;
		}
		else if (value == "off") {
			return _TT_TRACE_OFF;
		}
		else {
			return _TT_TRACE_UNKNOWN;
		}
	    case _TT_TRACE_WRITEFILE:
	    case _TT_TRACE_APPENDFILE:
		// Just about anything goes.  It's not up to this
		// code to validate filenames.
		return _TT_TRACE_FILENAME;
	    case _TT_TRACE_FUNCTIONS:
		if (allnone_check(value) == _TT_TRACE_UNKNOWN) {
			if (_is_tt_func(value)) {
				return _TT_TRACE_FUNC_VAL;
			}
			else {
				return _TT_TRACE_UNKNOWN;
			}
		}
	    case _TT_TRACE_ATTRIBUTES:
		return allnone_check(value);
	    case _TT_TRACE_STATES:
		if (value == "none") {
			return _TT_TRACE_NONE;
		}
		else if (value == "edge") {
			return _TT_TRACE_EDGE;
		}
		else if (value == "deliver") {
			return _TT_TRACE_DELIVER;
		}
		else if (value == "dispatch") {
			return _TT_TRACE_DISPATCH;
		}
		else {
			return state_name_to_val(value);
		}
	    case _TT_TRACE_OPS:
	    case _TT_TRACE_STRING:
		// An op is completely free-format
		return _TT_TRACE_STRING;
	    case _TT_TRACE_SENDER_PTYPES:
	    case _TT_TRACE_HANDLER_PTYPES:
	    case _TT_TRACE_IDENTIFIER:
		// Must be a legal identifier
		for (i = 0; i < value.len(); ++i) {
			if (!isalnum(value[i]) && value[i] != '_') {
				return _TT_TRACE_UNKNOWN;
			}
		}
		return _TT_TRACE_IDENTIFIER;
	    case _TT_TRACE_EDGE:
	    case _TT_TRACE_DELIVER:
	    case _TT_TRACE_DISPATCH:
		if (value == "edge") {
			return _TT_TRACE_EDGE;
		}
		else if (value == "deliver") {
			return _TT_TRACE_DELIVER;
		}
		else if (value == "dispatch") {
			return _TT_TRACE_DISPATCH;
		}
		else {
			return _TT_TRACE_UNKNOWN;
		}
	    case _TT_TRACE_NUMBER:
	    case _TT_TRACE_ON:
	    case _TT_TRACE_OFF:
	    case _TT_TRACE_ALL:
	    case _TT_TRACE_NONE:
	    case _TT_TRACE_FILENAME:
		// No tokens should follow these except _TT_TRACE_ENDLINE.
		// Since _TT_TRACE_ENDLINE is checked for above, this
		// code should not be reached when the syntax is correct.
		return _TT_TRACE_UNKNOWN;
	    case _TT_TRACE_STATE_CREATED:
	    case _TT_TRACE_STATE_SENT:
	    case _TT_TRACE_STATE_HANDLED:
	    case _TT_TRACE_STATE_FAILED:
	    case _TT_TRACE_STATE_QUEUED:
	    case _TT_TRACE_STATE_STARTED:
	    case _TT_TRACE_STATE_REJECTED:
		return state_name_to_val(value);
	    default:
		return _TT_TRACE_UNKNOWN;
	}
}

static void
print_error(_Tt_string& parse_buf,
	    int buf_ptr)
{
	int beg = buf_ptr - 1;
	int end = buf_ptr;

	iserr = 1;

	// Find beginning of line
	while (parse_buf[beg] != ';' &&
	       parse_buf[beg] != '\n' &&
	       beg != 0) {
		beg--;
	}
	beg++;

	// Find end of line
	while (parse_buf[end] != ';' &&
	       parse_buf[end] != '\n' &&
	       parse_buf[end] != '\0') {
		end++;
	}
	_tt_syslog( 0, LOG_ERR,
		    catgets( _ttcatd, 1, 21,
			     "tttracefile syntax error in line:\n%s" ),
		    parse_buf.mid(beg, end-beg).operator const char *() );

	return;
}

static void
eat_line(_Tt_string& parse_buf, int& buf_ptr)
{
	while (parse_buf[buf_ptr] != ';' && parse_buf[buf_ptr] != '\n') {
		buf_ptr++;
	}
}

static void
put_value(_Tt_trace_parser* tp,
	  _Tt_trace_tokens kywrd,
	  _Tt_trace_tokens token,
	  _Tt_string& value)
{
	switch(kywrd) {
	    case _TT_TRACE_VERSION:
		break;		// don't care yet
	    case _TT_TRACE_FOLLOW:
		tp->set_follow(token);
		break;
	    case _TT_TRACE_WRITEFILE:
		tp->set_sink(value, O_WRONLY);
		break;
	    case _TT_TRACE_APPENDFILE:
		tp->set_sink(value, O_APPEND);
		break;
	    case _TT_TRACE_FUNCTIONS:
		switch (token) {
		    case _TT_TRACE_ALL:
			tp->set_function(1);
			break;
		    case _TT_TRACE_NONE:
			tp->set_function(0);
			break;
		    default:
			tp->add_function(value);
			break;
		}
	    case _TT_TRACE_ATTRIBUTES:
		tp->set_attributes(token);
		break;
	    case _TT_TRACE_STATES:
		tp->add_state(token);
		break;
	    case _TT_TRACE_OPS:
		tp->add_op(value);
		break;
	    case _TT_TRACE_SENDER_PTYPES:
		tp->add_sender_ptype(value);
		break;
	    case _TT_TRACE_HANDLER_PTYPES:
		tp->add_handler_ptype(value);
		break;
#ifdef _OPT_TIMERS_SUNOS
	    case _TT_TRACE_TIMERS:
		tp->set_timers(token);
		break;
#endif
	}
}

static _Tt_trace_tokens
allnone_check(const _Tt_string& value)
{
	if (value == "all") {
		return _TT_TRACE_ALL;
	}
	else if (value == "none") {
		return _TT_TRACE_NONE;
	}
	else {
		return _TT_TRACE_UNKNOWN;
	}
}

static _Tt_trace_tokens
state_name_to_val(const _Tt_string& value)
{
	if (value == "TT_CREATED") {
		return _TT_TRACE_STATE_CREATED;
	}
	else if (value == "TT_SENT") {
		return _TT_TRACE_STATE_SENT;
	}
	else if (value == "TT_HANDLED") {
		return _TT_TRACE_STATE_HANDLED;
	}
	else if (value == "TT_FAILED") {
		return _TT_TRACE_STATE_FAILED;
	}
	else if (value == "TT_QUEUED") {
		return _TT_TRACE_STATE_QUEUED;
	}
	else if (value == "TT_STARTED") {
		return _TT_TRACE_STATE_STARTED;
	}
	else if (value == "TT_REJECTED") {
		return _TT_TRACE_STATE_REJECTED;
	}
	else if (value == "TT_RETURNED") {
		return _TT_TRACE_STATE_HANDLED;
	}
	else if (value == "TT_ACCEPTED") {
		return _TT_TRACE_STATE_ACCEPTED;
	}
	else if (value == "TT_ABSTAINED") {
		return _TT_TRACE_STATE_ABSTAINED;
	}
	else {
		return _TT_TRACE_UNKNOWN;
	}
}

static int _is_tt_func(const _Tt_string& fun)
{
	int arr_indx = 0;
	int i = 0;
	int name_len = fun.len();
	int entry_len;

	// This algorithm depends on the API names in _tt_entries to be
	// sorted, which is guaranteed by util/mktrcfiles.sh

	for (arr_indx = 0; arr_indx < _tt_entries_count; ++arr_indx) {

		entry_len = strlen(_tt_entries[arr_indx]);
		if (entry_len != name_len) continue;
	
		while (_tt_entries[arr_indx][i] == fun[i] &&
		       i < name_len) i++;

		if (i == name_len) return 1;
	}

	return 0;
}

const char* _tt_enumname(_Tt_entry_pt x) {
	if ((x < 0) || (x >= TT_API_CALL_LAST)) {
		return 0;
	} else {
		return _tt_entries[ x ];
	}
}
