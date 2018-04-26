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
//%%  $XConsortium: tt_parse_utils.C /main/3 1995/10/23 10:43:13 rswiston $ 			 				

#include <stdio.h>
#include <sys/param.h>
#include "tt_trace.tab.h"
#include "tt_trace_parser.h"

static _Tt_trace_parser_ptr trace_parser(NULL);

extern "C" {
	void process_follow(const int);
	void process_sink(const char* const);
	void clear_functions(void);
	void process_functionlist(const char* const);
	void process_functions_allnone(const int);
	void process_attributes(const int);
	void clear_states();
	int process_state(const int, const int);
	void create_op_list(const char* const);
	void add_op(const char* const);
	void create_sp_list(const char* const);
	void add_sender_ptype(const char* const);
	void create_hp_list(const char* const);
	void add_handler_ptype(const char* const);
	int _tt_trace_input();
	void _tt_trace_unput(int);
}

void set_trace_parser(const _Tt_trace_parser_ptr tp)
{
	trace_parser = tp;
}

void process_follow(const int on_off)
{
	trace_parser->set_follow(on_off);
}

void process_sink(const char* const fname)
{
	_Tt_string tmp_fname(fname); // Shuts compiler up
		
	trace_parser->set_sink(tmp_fname);
}

void clear_functions(void)
{
	trace_parser->clear_functions();
}

void process_functionlist(const char* const text_item)
{
	_Tt_string tmp_item(text_item);	// Shuts compiler up
		
	trace_parser->add_function(tmp_item);
}

void process_functions_allnone(const int all_none)
{
	trace_parser->clear_functions(); /* clear any previous settings */
	trace_parser->set_function(all_none);
}

void process_attributes(const int all_none)
{
	trace_parser->set_attributes(all_none);
}

void clear_states(void)
{
	trace_parser->clear_states(); /* clear any previous settings */
}

int process_state(const int state_type, const int state_val)
{
	int sval = state_val;
	
	if (state_type == _TT_TRACE_STATES_NEDD) {
		switch (state_val) {
			
		    case _TT_TRACE_NONE:
			sval = states_none;
			break;
		    case _TT_TRACE_EDGE:
			sval = states_edge;
			break;
		    case _TT_TRACE_DELIVER:
			sval = states_deliver;
			break;
		    case _TT_TRACE_DISPATCH:
			sval = states_dispatch;
			break;
		    default:
			sval = states_none;
			break;
		}
	}
	
	return trace_parser->add_state(sval);
}

void create_op_list(const char* const op)
{
	_Tt_string tmp_op(op);	// Shuts compiler up
		
	trace_parser->clear_ops();
	trace_parser->add_op(tmp_op);
}

void add_op(const char* const op)
{
	_Tt_string tmp_op(op);	// Shuts compiler up
		
	trace_parser->add_op(tmp_op);
}

void create_sp_list(const char* const sp)
{
	_Tt_string tmp_sp(sp);	// Shuts compiler up
		
	trace_parser->clear_sender_ptypes();
	trace_parser->add_sender_ptype(tmp_sp);
}

void add_sender_ptype(const char* const sp)
{
	_Tt_string tmp_sp(sp);	// Shuts compiler up
		
	trace_parser->add_sender_ptype(tmp_sp);
}

void create_hp_list(const char* const hp)
{
	_Tt_string tmp_hp(hp);	// Shuts compiler up
		
	trace_parser->clear_handler_ptypes();
	trace_parser->add_handler_ptype(tmp_hp);
}

void add_handler_ptype(const char* const hp)
{
	_Tt_string tmp_hp(hp);	// Shuts compiler up
		
	trace_parser->add_handler_ptype(tmp_hp);
}

// Lex utility functions

int _tt_trace_input(void)
{
	return trace_parser->next_char();
}

void _tt_trace_unput(int ch)
{
	trace_parser->ungetc(ch);
}
