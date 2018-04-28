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
/*%%  $XConsortium: tt_trace_parser.h /main/3 1995/10/23 10:46:45 rswiston $ 			 				 */
/* -*-C++-*-
 *
 * @(#)tt_trace_parser.h	1.8 93/11/04
 * 
 * Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 */
#ifndef _TT_TRACE_PARSER_H
#define _TT_TRACE_PARSER_H
 
#include	"api/c/tt_c.h"
#include	"util/tt_object.h"
#include	"util/tt_string.h"
#include	"util/tt_int_rec.h"
#include	"mp/mp_message.h"
#include	"util/tt_entry_pt.h"
#include	"util/tt_trace_parse.h"

// Function handling constants

const int	_TT_FUNS_ALLNONE = 2;
const int	_TT_FUNS_EXPLICIT = 1;
const int	_TT_FUNS_NOTSET = 0;

typedef enum {
	None,
	Edge,
	Dispatch,
	Deliver
} _Tt_dispatch_stage;

class _Tt_trace_parser : public virtual _Tt_allocated {
    public:

	// Constructor / Destructor
		
	_Tt_trace_parser();
	_Tt_trace_parser(_Tt_trace_parser& templ);
	_Tt_trace_parser(int fd);
	_Tt_trace_parser(const char* script);
	~_Tt_trace_parser();

	// Parse function
		
	int				parse();

	// Is any tracing being done?

	int				any_tracing();
	
	// Retrieval functions -- used by client and server tracing
		
	int				follow_val();
	_Tt_string&			sink_val();
	const char*			mode_val();	// sink file open mode

	// ifunction_val returns one of the consts defined above --
	// _TT_FUNS_ALLNONE, _TT_FUNS_EXPLICIT, or _TT_FUNS_NOTSET.  If
	// _TT_FUNS_NOTSET is returned, there still may be a value
	// returned from sfunction_val
	
	int				ifunction_val();

	// sfunction_val returns the list ov functions to trace if an explicit
	// list was given in the config info.  If no such list was specified,
	// an empty _Tt_string will be returned.
		
	_Tt_string_list_ptr		sfunction_val();
	int				attributes_val() const;

	// states can be set to multiple values in the config file,
	// so states_val returns the list of values specified.
		
	_Tt_int_rec_list_ptr		states_val();
	_Tt_string_list_ptr		ops_val();
	_Tt_string_list_ptr		sender_ptypes_val();
	_Tt_string_list_ptr		handler_ptypes_val();
#ifdef _OPT_TIMERS_SUNOS
	int				timers_val();
#endif

	// Utility functions used by parser -- not intended
	// for use by any other code
		
	// Field setting functions
		
	// Follow
	void	set_follow(int fval);
	
	// sink
	void	set_sink(const _Tt_string& sinkfile, int mode);
	
	// functions
	void	clear_functions();
	void	add_function(const _Tt_string& fun);
	void	set_function(int fun);
	
	// attributes
	void	set_attributes(int attr);
	
	// states
	void	clear_states();
	int	add_state(int state_val);
	
	// ops
	void	clear_ops();
	void	add_op(const _Tt_string& newop);
	
	// sender_ptypes
	void	clear_sender_ptypes();
	void	add_sender_ptype(const _Tt_string& sp);
	
	// handler_ptypes
	void	clear_handler_ptypes();
	void	add_handler_ptype(const _Tt_string& hp);

	// API timers
#ifdef _OPT_TIMERS_SUNOS
	void				set_timers(int tval);
#endif

	// Tracing filters -- used by the tracing admin class
		
	// message tracing filter
	int is_traced(const _Tt_message& msg) const;
	int is_traced(const _Tt_message& msg,
		      _Tt_dispatch_stage stage) const;
	int is_traced(_Tt_entry_pt fun) const;

    private:

	// config file or inline config stream -- used by YACC and Lex
		
	_Tt_string			trace_stream;

	int				follow;
	_Tt_string			sink;
	int				mode;	// Sink file open mode
	int				ifunctions;
	_Tt_string_list_ptr		sfunctions;

	// discriminator for function values -- either _TT_FUNS_ALLNONE
	// or _TT_FUNS_EXPLICIT
	
	int				_funcd;
	
	int				attributes;
	_Tt_int_rec_list_ptr		states;
	_Tt_string_list_ptr		ops;
	_Tt_string_list_ptr		sender_ptypes;
	_Tt_string_list_ptr		handler_ptypes;
#ifdef _OPT_TIMERS_SUNOS
	int				timers;
#endif

	// does initialization common to all constructors
		
	void	tracer_init();

	// Code common to the two is_traced methods that take a message arg

	int	_is_traced(const _Tt_message& msg) const;
};

#endif
