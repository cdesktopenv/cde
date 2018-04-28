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
//%%  $XConsortium: tt_trace_parser.C /main/3 1995/10/23 10:46:38 rswiston $ 			 				
/*
 *
 * tt_trace_parser.cc
 *
 * Copyright (c) 1993 by Sun Microsystems, Inc.
 */
 
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <fcntl.h>
#include "util/tt_enumname.h"
#include "tt_trace_parser.h"
#include "tt_trace_parse.h"

// Implementation of the top-level tracer class

static int _tt_match_string_list(const _Tt_string&, _Tt_string_list_ptr);
static Tt_state _token_to_value(int tval);
extern int _tt_trace_parse(_Tt_string&, _Tt_trace_parser*);

_Tt_trace_parser::_Tt_trace_parser()
    : trace_stream()
{
	tracer_init();

	mode = 0; // Technically this is openmode O_RDONLY but will be overwritten 
}

_Tt_trace_parser::_Tt_trace_parser(_Tt_trace_parser& templ)
{
	trace_stream = templ.trace_stream;
	follow = templ.follow;
	sink = templ.sink;
	mode = templ.mode;
	ifunctions = templ.ifunctions;
	sfunctions = templ.sfunctions;
	_funcd = templ._funcd;
	attributes = templ.attributes;
	states = templ.states;
	ops = templ.ops;
	sender_ptypes = templ.sender_ptypes;
	handler_ptypes = templ.handler_ptypes;
#ifdef _OPT_TIMERS_SUNOS
	timers = templ.timers;
#endif
}

_Tt_trace_parser::_Tt_trace_parser(int fd)
: trace_stream(), sink()
{
	// Read in the file into the trace_stream string...we can't
	// just read each character as we need it because the Lex
	// unput() macro needs to stuff back characters, and those
	// characters might be different from those read -- which
	// would require that the parse config file be opened read/write.

	int numread = 0;
	const int bufsize = 1000;	// Read in 1000 bytes at a time
	char buf[bufsize];

	do {
		numread = read(fd, buf, bufsize);
		_Tt_string tmp_str((const unsigned char*) buf, numread);
		trace_stream = trace_stream.cat(tmp_str);
	} while (numread == bufsize);

	trace_stream[trace_stream.len()] = '\0';

	tracer_init();
	
	mode = 0; // Technically this is openmode O_RDONLY but will be overwritten 
}

_Tt_trace_parser::_Tt_trace_parser(const char* script)
{
	trace_stream = script;
	tracer_init();

	mode = 0; // Technically this is openmode O_RDONLY but will be overwritten 
}

void
_Tt_trace_parser::tracer_init()
{
	follow = _TT_TRACE_OFF;
	ifunctions = 1;
	sfunctions = new _Tt_string_list();
	_funcd = _TT_FUNS_ALLNONE;
	attributes = _TT_TRACE_ALL;
	states = new _Tt_int_rec_list();
	states->append( new _Tt_int_rec( _TT_TRACE_DISPATCH ));
	ops = new _Tt_string_list();
	sender_ptypes = new _Tt_string_list();
	handler_ptypes = new _Tt_string_list();
#ifdef _OPT_TIMERS_SUNOS
	timers = _TT_TRACE_OFF;
#endif
}

_Tt_trace_parser::~_Tt_trace_parser()
{
	(void) states->flush();
	(void) ops->flush();
	(void) sender_ptypes->flush();
	(void) handler_ptypes->flush();
}

int
_Tt_trace_parser::parse()
{
	if ((trace_stream.len() == 0) || (trace_stream[0] == '\0')) {
		return 0;
	}

	return _tt_trace_parse(trace_stream, this);
}

int
_Tt_trace_parser::any_tracing()
{
        int msg_tracing =    (states->count() != 1)
                          || (states->top()->val != _TT_TRACE_NONE);
        int api_tracing;
        if (_funcd == _TT_FUNS_ALLNONE) {
                api_tracing = ifunctions;
        } else {
                api_tracing =    (! sfunctions.is_null())
                              && (! sfunctions->is_empty());
        }
        return msg_tracing || api_tracing;
}

int
_Tt_trace_parser::follow_val()
{
	if (follow == _TT_TRACE_OFF) {
		return 0;
	}
	else {
		return 1;
	}
}

_Tt_string&
_Tt_trace_parser::sink_val()
{
	return sink;
}

const char*
_Tt_trace_parser::mode_val()
{
	// Turn off all flags we don't care about
	
	int tmp_mode = mode & (O_ACCMODE | O_APPEND);
	
	if (tmp_mode == O_RDONLY)
		return "r";
	else if (tmp_mode == O_WRONLY)
		return "w";
	else if (tmp_mode == O_RDWR)
		return "r+";
	else if (tmp_mode == O_APPEND || tmp_mode == (O_WRONLY | O_APPEND))
		return "a";
	else if (tmp_mode == (O_RDONLY | O_APPEND) ||
		 tmp_mode == (O_RDWR | O_APPEND))
		return "a+";
	else
		return "w";		// default....
}

int
_Tt_trace_parser::ifunction_val()
{
	if (_funcd == _TT_FUNS_ALLNONE)
		return ifunctions;
	else
		return 0;
}

_Tt_string_list_ptr
_Tt_trace_parser::sfunction_val()
{
	return sfunctions;
}

int
_Tt_trace_parser::attributes_val() const
{
	if (attributes == _TT_TRACE_NONE) {
		return 0;
	}
	else {
		return 1;
	}
}

_Tt_int_rec_list_ptr
_Tt_trace_parser::states_val()
{
	return states;
}
   
_Tt_string_list_ptr
_Tt_trace_parser::ops_val()
{
	return ops;
}

_Tt_string_list_ptr
_Tt_trace_parser::sender_ptypes_val()
{
	return sender_ptypes;
}

_Tt_string_list_ptr
_Tt_trace_parser::handler_ptypes_val()
{
	return handler_ptypes;
}

#ifdef _OPT_TIMERS_SUNOS
int
_Tt_trace_parser::timers_val()
{
	if (timers == _TT_TRACE_OFF) {
		return 0;
	}
	else {
		return 1;
	}
}
#endif

void
_Tt_trace_parser::set_follow(int fval)
{
	follow = fval;
}

void
_Tt_trace_parser::set_sink(const _Tt_string& sinkfile, int open_mode)
{
	sink = sinkfile;
	mode = open_mode;
}

void
_Tt_trace_parser::clear_functions()
{
	if (_funcd == _TT_FUNS_EXPLICIT) (void) sfunctions->flush();
	_funcd = _TT_FUNS_NOTSET;
	ifunctions = 0;
}

void
_Tt_trace_parser::add_function(const _Tt_string& fun)
{
	_funcd = _TT_FUNS_EXPLICIT;
	sfunctions->append(fun);
}

void
_Tt_trace_parser::set_function(int fun)
{
	_funcd = _TT_FUNS_ALLNONE;
	ifunctions = fun;
}

void
_Tt_trace_parser::set_attributes(int attr)
{
	attributes = attr;
}

void
_Tt_trace_parser::clear_states()
{
	if (!states->is_empty()) states->flush();
}

// add_state can return an error if the line in the control file has
// a line like "states edge none TT_SENT", which is semantically
// incorrect.  If none is given as a states value, it must be alone
// on the line.

int
_Tt_trace_parser::add_state(int state_val)
{
	if (!states->is_empty()) {

		if (state_val == _TT_TRACE_NONE && states->count() > 0) {
			return 0;
		}
		else if (state_val != _TT_TRACE_NONE &&
			 states->count() == 1) {
			_Tt_int_rec_list_cursor state_curs(states);
			if (state_curs.next() == _TT_TRACE_NONE) {
				return 0;
			}
		}
	}
		
	_Tt_int_rec_ptr tmp_ptr = new _Tt_int_rec(state_val);

	states->append(tmp_ptr);

	return 1;
}

void
_Tt_trace_parser::clear_ops()
{
	if (!ops->is_empty()) ops->flush();
}

void
_Tt_trace_parser::add_op(const _Tt_string& newop)
{
	ops->append(newop);
}

void
_Tt_trace_parser::clear_sender_ptypes()
{
	if (!sender_ptypes->is_empty()) sender_ptypes->flush();
}

void
_Tt_trace_parser::add_sender_ptype(const _Tt_string& sp)
{
	sender_ptypes->append(sp);
}

void
_Tt_trace_parser::clear_handler_ptypes()
{
	if (!handler_ptypes->is_empty()) handler_ptypes->flush();
}

void
_Tt_trace_parser::add_handler_ptype(const _Tt_string& hp)
{
	handler_ptypes->append(hp);
}

#ifdef _OPT_TIMERS_SUNOS
void
_Tt_trace_parser::set_timers(int tval)
{
	timers = tval;
}
#endif

int
_Tt_trace_parser::is_traced(const _Tt_message& msg) const
{
	_Tt_int_rec_list_cursor stateC( states );
	while (stateC.next()) {
		if (stateC->val == _TT_TRACE_NONE) {
			return 0;
		}
	}
	return _is_traced( msg );
}

int
_Tt_trace_parser::is_traced(const _Tt_message& msg,
			    _Tt_dispatch_stage stage) const
{
	// Do we trace this sort of message at all?
	if (! _is_traced( msg )) {
		return 0;
	}

	// Is the message even in the indicated dispatch stage?
	if (stage == Edge) {
		switch (msg.state()) {
		    case TT_SENT:
		    case TT_HANDLED:
		    case TT_RETURNED:
		    case TT_FAILED:
			break;
		    default:
			return 0;
		}
	}

	// Is the message in a traced state?
	_Tt_int_rec_list_ptr realStates = new _Tt_int_rec_list;
	// realStates really should just be computed once XXX
	_Tt_int_rec_list_cursor fakeStateC( states );
	while (fakeStateC.next()) {
		switch (fakeStateC->val) {
		    case _TT_TRACE_NONE:
			return 0;
		    case _TT_TRACE_DISPATCH:
		    case _TT_TRACE_DELIVER:
			return 1;
		    case _TT_TRACE_EDGE:
			realStates->append( new _Tt_int_rec(TT_SENT));
			realStates->append( new _Tt_int_rec(TT_HANDLED));
			realStates->append( new _Tt_int_rec(TT_RETURNED));
			realStates->append( new _Tt_int_rec(TT_FAILED));
			break;
		    default:
			realStates->append( new _Tt_int_rec(
				_token_to_value( fakeStateC->val )));
			break;
		}
	}
	_Tt_int_rec_list_cursor realStateC( realStates );
	while (realStateC.next()) {
		if (realStateC->val == msg.state()) {
			return 1;
		}
	}
	return 0;
}

int
_Tt_trace_parser::_is_traced(const _Tt_message& msg) const
{
	// Now match on ops

	if (!ops->is_empty() &&
	    !_tt_match_string_list(msg.op(), ops))
		return 0;

	// Now match on sender_ptypes

	if (!sender_ptypes->is_empty() &&
	    !_tt_match_string_list(msg.sender_ptype(), sender_ptypes))
		return 0;

	// Now match on handler_ptypes

	if (!handler_ptypes->is_empty() &&
	    !_tt_match_string_list(msg.handler_ptype(), handler_ptypes))
		return 0;

	return 1;
}
	
int
_Tt_trace_parser::is_traced(_Tt_entry_pt fun) const
{
	if (_funcd == _TT_FUNS_ALLNONE) return ifunctions;
	
	_Tt_string fun_str = _tt_enumname(fun);
	return _tt_match_string_list(fun_str, sfunctions);
}
	
static int
_tt_match_string_list(const _Tt_string& str, _Tt_string_list_ptr slist)
{
	int matched = 0;
	if (slist->is_empty()) {

		matched = 0;
	}
	else {

		_Tt_string_list_cursor curs(slist);
		while (curs.next()) {

			if (str.sh_match(*curs)) {

				matched = 1;
				break;
			}
		}
	}

	if (!matched) return 0;
	return 1;
}

static Tt_state _token_to_value(int tval)
{
	switch(tval) {
	    case _TT_TRACE_STATE_CREATED:
		return TT_CREATED;
	    case _TT_TRACE_STATE_SENT:
		return TT_SENT;
	    case _TT_TRACE_STATE_HANDLED:
		return TT_HANDLED;
	    case _TT_TRACE_STATE_RETURNED:
		return TT_HANDLED;
	    case _TT_TRACE_STATE_FAILED:
		return TT_FAILED;
	    case _TT_TRACE_STATE_QUEUED:
		return TT_QUEUED;
	    case _TT_TRACE_STATE_STARTED:
		return TT_STARTED;
	    case _TT_TRACE_STATE_REJECTED:
		return TT_REJECTED;
	    case _TT_TRACE_STATE_ACCEPTED:
		return TT_ACCEPTED;
	    case _TT_TRACE_STATE_ABSTAINED:
		return TT_ABSTAINED;
	    default:
		return TT_STATE_LAST;
	}
}
