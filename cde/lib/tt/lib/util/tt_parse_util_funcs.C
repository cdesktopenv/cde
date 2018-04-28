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
//%%  $XConsortium: tt_parse_util_funcs.C /main/3 1995/10/23 10:43:05 rswiston $ 			 				
/*
 * @(#)tt_parse_util_funcs.C	1.9 93/11/04
 * 
 * Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 */

#include <stdio.h>
#include <ctype.h>
#include <sys/param.h>
#include "tt_trace.tab.h"
#include "tt_trace_parser.h"

// utility functions to provide a C interface to tt_trace_parser classes

static _Tt_trace_parser* trace_parser = 0;

static int _is_tt_func(const _Tt_string);

extern "C" {
	void _tt_process_follow(const int);
	void _tt_process_sink(const char* const, const int);
	void _tt_clear_functions(void);
	int _tt_process_functionlist(const char* const);
	void _tt_process_functions_allnone(const int);
	void _tt_process_attributes(const int);
	void _tt_clear_states();
	int _tt_process_state(const int, const int);
	void _tt_create_op_list(const char* const);
	void _tt_add_op(const char* const);
	void _tt_create_sp_list(const char* const);
	void _tt_add_sender_ptype(const char* const);
	void _tt_create_hp_list(const char* const);
	void _tt_add_handler_ptype(const char* const);
	int _tt_trace_input();
	void _tt_trace_unput(int);
}

void _tt_set_trace_parser(_Tt_trace_parser *tp)
{
	trace_parser = tp;
}

void _tt_process_follow(const int on_off)
{
	trace_parser->set_follow(on_off);
}

void _tt_process_sink(const char* const fname, const int open_mode)
{
	trace_parser->set_sink(fname, open_mode);
}

void _tt_clear_functions(void)
{
	trace_parser->clear_functions();
}

int _tt_process_functionlist(const char* const text_item)
{
	if (_is_tt_func(text_item)) {
		trace_parser->add_function(tmp_item);
		return 1;
	}
	else {
		_tt_syslog( 0, LOG_ERR, "tttracefile(4): \"functions %s\": %s",
			    text_item, strerror(EINVAL) );
		return 0;
	}
}

void _tt_process_functions_allnone(const int all_none)
{
	trace_parser->clear_functions(); /* clear any previous settings */
	trace_parser->set_function(all_none);
}



void _tt_process_attributes(const int all_none)
{
	trace_parser->set_attributes(all_none);
}

void _tt_clear_states(void)
{
	trace_parser->clear_states(); /* clear any previous settings */
}

int _tt_process_state(const int state_type, const int state_val)
{
	int sval = state_val;
	
	if (state_type == _TT_TRACE_STATES_NEDD) {
		switch (state_val) {
			
		    case _TT_TRACE_NONE:
			sval = _tt_states_none;
			break;
		    case _TT_TRACE_EDGE:
			sval = _tt_states_edge;
			break;
		    case _TT_TRACE_DELIVER:
			sval = _tt_states_deliver;
			break;
		    case _TT_TRACE_DISPATCH:
			sval = _tt_states_dispatch;
			break;
		    default:
			sval = _tt_states_none;
			break;
		}
	}
	
	return trace_parser->add_state(sval);
}

void _tt_create_op_list(const char* const op)
{
	_Tt_string tmp_op(op);	// Shuts compiler up
		
	trace_parser->clear_ops();
	trace_parser->add_op(tmp_op);
}

void _tt_add_op(const char* const op)
{
	_Tt_string tmp_op(op);	// Shuts compiler up
		
	trace_parser->add_op(tmp_op);
}

void _tt_create_sp_list(const char* const sp)
{
	_Tt_string tmp_sp(sp);	// Shuts compiler up
		
	trace_parser->clear_sender_ptypes();
	trace_parser->add_sender_ptype(tmp_sp);
}

void _tt_add_sender_ptype(const char* const sp)
{
	_Tt_string tmp_sp(sp);	// Shuts compiler up
		
	trace_parser->add_sender_ptype(tmp_sp);
}



void _tt_create_hp_list(const char* const hp)
{
	_Tt_string tmp_hp(hp);	// Shuts compiler up
		
	trace_parser->clear_handler_ptypes();
	trace_parser->add_handler_ptype(tmp_hp);
}

void _tt_add_handler_ptype(const char* const hp)
{
	_Tt_string tmp_hp(hp);	// Shuts compiler up
		
	trace_parser->add_handler_ptype(tmp_hp);
}

// Lex utility functions

int _tt_trace_input(void)
{
	static int	toggle = 1;
	int		ret;

	// Make sure the last character returned is a LINETERM character.
	// This is necessary for inline scripts that do not have a
	// semicolon as the last character.

	// The "if (toggle)" statement seems like it should be the
	// *outer* if statement, but this is not the case, because it
	// is possible (likely, in fact) that the final semicolon returned
	// gets unput by lex, in which case nect_char() will pick it up
	// on the next tiem through, and the value of toggle will carry
	// through to the next invocation of this routine.
		
	char c = trace_parser->next_char();
	if (!isprint(c)) {

		if (toggle) {

			ret = ';';	// Force LINETERM char at end
			toggle = 0;
		}
		else {
			ret = '\0';
			toggle = 1;
		}
	}
	else {
		ret = c;
	}
	
	return ret;
}

void _tt_trace_unput(int ch)
{
	trace_parser->ungetc(ch);
}

// The following include file is generated by the TT API
// names in tt_c.h.  It declares an array of strings of all
// TT PAI calls (_tt_entries), and an int count of the number
// of API names (_tt_entries_count)

#include <util/tt_entry_pt_names.h>

const char *
_tt_enumname(_Tt_entry_pt x) {
	if ((x < 0) || (x >= TT_API_CALL_LAST)) {
		return 0;
	} else {
		return _tt_entries[ x ];
	}
}

static int _is_tt_func(const _Tt_string fun)
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
