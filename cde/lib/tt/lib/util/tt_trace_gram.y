/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: tt_trace_gram.y /main/3 1995/10/23 10:46:17 rswiston $ 			 				 */
%{

/*
 *
 * tt_trace_gram.y
 *
 * Copyright (c) 1993 by Sun Microsystems, Inc.
 */
 
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/param.h>

extern int _tt_trace_iserr;	/* declared in tt_trace_parser.cc */

static int wrapped = 0;
static int parsed = 0;
static int ret = 0;
static int all = 0;

extern void _tt_process_follow(int);
extern void _tt_process_sink(char*, int);
extern void _tt_clear_functions(void);
extern int _tt_process_functionlist(char*);
extern void _tt_process_functions_allnone(int);
extern void _tt_process_attributes(int);
extern void _tt_clear_states(void);
extern int _tt_process_state(int, int);
extern void _tt_create_op_list(char*);
extern void _tt_add_op(char*);
extern void _tt_create_sp_list(char*);
extern void _tt_add_sender_ptype(char*);
extern void _tt_create_hp_list(char*);
extern void _tt_add_handler_ptype(char*);



%}

	/*
	 * union for return values
	 */

%union {
	char	special;
	char	string[MAXPATHLEN];
	int		tokenval;
	struct {
		int state_type;
		int state_val;
	} stateval;
}

	/*
	 * Lexical keyword tokens
	 */

%token			_TT_TRACE_FOLLOW _TT_TRACE_SINK _TT_TRACE_SINK_APPEND
%token			_TT_TRACE_FUNCTIONS _TT_TRACE_ATTRIBUTES _TT_TRACE_STATES
%token			_TT_TRACE_OPS _TT_TRACE_SENDER_PTYPES
%token			_TT_TRACE_HANDLER_PTYPES _TT_TRACE_ON
%token			_TT_TRACE_OFF _TT_TRACE_ALL _TT_TRACE_NONE
%token			_TT_TRACE_STATES_NEDD _TT_TRACE_STATES_TT
%token			_TT_TRACE_EDGE _TT_TRACE_DELIVER
%token			_TT_TRACE_DISPATCH

	/*
	 * Value keyword tokens
	 */

%token <tokenval>	_TT_TRACE_VERSION _TT_TRACE_IVAL
%token <stateval>	_TT_TRACE_STATES_VAL
%token <string>	 	_TT_TRACE_ALPHANUM _TT_TRACE_FILENAME
%token <special>	_TT_TRACE_LINETERM _TT_TRACE_PARSE_EOF

%start list



%%

finish:		list _TT_TRACE_PARSE_EOF {
			if (parsed)
				ret = 0;
			else
				ret = 1;
			parsed = 0;
			return ret;
		}

list:		/* nothing */
	|	list version_list		{ parsed = 1; wrapped = 0; }
	|	list follow_list		{ parsed = 1; wrapped = 0; }
	|	list sink_list			{ parsed = 1; wrapped = 0; }
	|	list functions_list		{ parsed = 1; wrapped = 0; }
	|	list attributes_list		{ parsed = 1; wrapped = 0; }
	|	list states_list		{ parsed = 1; wrapped = 0; }
	|	list ops_list			{ parsed = 1; wrapped = 0; }
	|	list sender_ptypes_list		{ parsed = 1; wrapped = 0; }
	|	list handler_ptypes_list	{ parsed = 1; wrapped = 0; }
	|	list _TT_TRACE_LINETERM
	;

version_list:		_TT_TRACE_VERSION _TT_TRACE_LINETERM
			;	/* Do nothing for version 1 grammar */

follow_list:		_TT_TRACE_FOLLOW _TT_TRACE_IVAL _TT_TRACE_LINETERM
				{ _tt_process_follow($2); }
			;

sink_list:		_TT_TRACE_SINK _TT_TRACE_FILENAME _TT_TRACE_LINETERM
				{ _tt_process_sink($2, O_RDWR | O_CREAT); }
	|		_TT_TRACE_SINK_APPEND _TT_TRACE_FILENAME _TT_TRACE_LINETERM
				{ _tt_process_sink($2, O_APPEND | O_CREAT); }

			/*
			  * It's a pain to distinguish between a legal filename string and
			  * a plain alphanumeric string, so just lump them together.
			  */

	|		_TT_TRACE_SINK _TT_TRACE_ALPHANUM _TT_TRACE_LINETERM
				{ _tt_process_sink($2, O_RDWR | O_CREAT); }
	|		_TT_TRACE_SINK_APPEND _TT_TRACE_ALPHANUM _TT_TRACE_LINETERM
				{ _tt_process_sink($2, O_APPEND | O_CREAT); }
			;

functionkywrd:		_TT_TRACE_FUNCTIONS
				{ _tt_clear_functions(); }
			;

functionlist:		functionkywrd _TT_TRACE_ALPHANUM
				{ _tt_process_functionlist($2); }
	|		functionlist _TT_TRACE_ALPHANUM
				{ _tt_process_functionlist($2); }
			;

functions_list:		_TT_TRACE_FUNCTIONS _TT_TRACE_IVAL _TT_TRACE_LINETERM
				{
					all = ($2 == _TT_TRACE_ALL ? 1 : 0);
					_tt_process_functions_allnone(all);
				}
	|		functionlist _TT_TRACE_LINETERM
			;

attributes_list:	_TT_TRACE_ATTRIBUTES _TT_TRACE_IVAL _TT_TRACE_LINETERM
				{
					all = ($2 == _TT_TRACE_ALL ? 1 : 0);
					_tt_process_attributes($2);
				}
			;



stateskywrd:		_TT_TRACE_STATES
				{ _tt_clear_states(); }

stateslist:		stateskywrd _TT_TRACE_STATES_VAL
				{
					/* process_states can't fail here because the
					 * grammar ensures that this is the first
					 * value on the line
					 */
					(void) _tt_process_state($2.state_type,
							     $2.state_val);
				}
	|		stateslist _TT_TRACE_STATES_VAL
				{
					if (!_tt_process_state($2.state_type,
							  $2.state_val)) {
						_tt_trace_iserr = 2;
						tt_trace_error("Can't mix none with other state values");
					}
				}
	|		stateskywrd _TT_TRACE_IVAL
				{
					/* process_states can't fail here because the
					 * grammar ensures that this is the first
					 * value on the line
					 */

					if ($2 == _TT_TRACE_NONE) {
						_tt_process_state(_TT_TRACE_STATES_NEDD,
							      _TT_TRACE_NONE);
					}
					else {	 /* Syntax error */
						_tt_trace_iserr = 3;
						tt_trace_error("Bad value for state");
					}
				}
	|		stateslist _TT_TRACE_IVAL
				{
					if ($2 == _TT_TRACE_NONE) {
						if (!_tt_process_state(_TT_TRACE_STATES_NEDD,
								   _TT_TRACE_NONE)) {
							_tt_trace_iserr = 2;
							tt_trace_error("Can't mix none with other state values");
						}
					}
					else {	 /* Syntax error */
						_tt_trace_iserr = 3;
						tt_trace_error("Bad value for state");
					}
				}
			;



states_list:		stateslist _TT_TRACE_LINETERM
			;

oplist:			_TT_TRACE_OPS _TT_TRACE_ALPHANUM
				{ _tt_create_op_list($2); }
	|		oplist _TT_TRACE_ALPHANUM
				{ _tt_add_op($2); }

ops_list:		oplist _TT_TRACE_LINETERM
			;

senderptypeslist:	_TT_TRACE_SENDER_PTYPES _TT_TRACE_ALPHANUM
				{ _tt_create_sp_list($2); }
	|		senderptypeslist _TT_TRACE_ALPHANUM
				{ _tt_add_sender_ptype($2); }

sender_ptypes_list:	senderptypeslist _TT_TRACE_LINETERM
			;

handlerptypeslist:	_TT_TRACE_HANDLER_PTYPES _TT_TRACE_ALPHANUM
				{ _tt_create_hp_list($2); }
	|		handlerptypeslist _TT_TRACE_ALPHANUM
				{ _tt_add_handler_ptype($2); }

handler_ptypes_list:	handlerptypeslist _TT_TRACE_LINETERM
			;

%%



void
tt_trace_error(char* s)
{
	if (!wrapped) {

		/*
		 * _tt_trace_iserr is set before explicit calls to
		 * tt_trace_error.  If its value is still zero, then
		 * YACC called this routine because of a syntax error.
		 */

		fprintf(stderr, "%s\n", s);
		if (!_tt_trace_iserr) _tt_trace_iserr = 1;
	}
}

int
tt_trace_wrap()
{
	wrapped = 1;
	return 1;
}
