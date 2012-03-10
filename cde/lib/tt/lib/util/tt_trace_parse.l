/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: tt_trace_parse.l /main/3 1995/10/23 10:46:31 rswiston $ 			 				 */
%{

/*
 *
 * tt_trace_parse.l
 *
 * Copyright (c) 1993 by Sun Microsystems, Inc.
 */
 
#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <api/c/tt_c.h>
#include "util/tt_trace.tab.h"

#undef input
#undef unput
#define input _tt_trace_input
#define unput(c) _tt_trace_unput(c)

extern int _tt_trace_input();
extern void _tt_trace_unput(int);

static char* fname_chars = "/0123456789:;=@ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_,.";

%}

	/*
	 * macros for common character classes
	 */

%a 10000
%e 300
%p 1500
%n 200
%k 100
%o 12000

SP		[ \t]+
LINE_TERM	[\n;]

%%

{LINE_TERM}	{ return _TT_TRACE_LINETERM; }

""		{ return _TT_TRACE_PARSE_EOF; }
\0		{ return _TT_TRACE_PARSE_EOF; }
''		{ return _TT_TRACE_PARSE_EOF; }

\#.*		;	/* comments */
{SP}		;

follow		{ return _TT_TRACE_FOLLOW; }
">>"		{ return _TT_TRACE_SINK_APPEND; }
">"		{ return _TT_TRACE_SINK; }
functions	{ return _TT_TRACE_FUNCTIONS; }
attributes	{ return _TT_TRACE_ATTRIBUTES; }
states		{ return _TT_TRACE_STATES; }
ops		{ return _TT_TRACE_OPS; }
sender_ptypes	{ return _TT_TRACE_SENDER_PTYPES; }
handler_ptypes	{ return _TT_TRACE_HANDLER_PTYPES; }

off		{
			tt_trace_lval.tokenval = _TT_TRACE_OFF;
			return _TT_TRACE_IVAL;
		}

on		{
			tt_trace_lval.tokenval = _TT_TRACE_ON;
			return _TT_TRACE_IVAL;
		}

all		{
			tt_trace_lval.tokenval = _TT_TRACE_ALL;
			return _TT_TRACE_IVAL;
		}

none		{
			tt_trace_lval.tokenval = _TT_TRACE_NONE;
			return _TT_TRACE_IVAL;
		}

edge		{
			tt_trace_lval.stateval.state_type =
				_TT_TRACE_STATES_NEDD;
			tt_trace_lval.stateval.state_val = _TT_TRACE_EDGE;
			return _TT_TRACE_STATES_VAL;
		}

deliver		{ 
			tt_trace_lval.stateval.state_type =
				_TT_TRACE_STATES_NEDD;
			tt_trace_lval.stateval.state_val = _TT_TRACE_DELIVER;
			return _TT_TRACE_STATES_VAL; 
		}

dispatch	{ 
			tt_trace_lval.stateval.state_type =
				_TT_TRACE_STATES_NEDD;
			tt_trace_lval.stateval.state_val = _TT_TRACE_DISPATCH;
			return _TT_TRACE_STATES_VAL; 
		}

TT_CREATED	{ 
			tt_trace_lval.stateval.state_type = _TT_TRACE_STATES_TT;
			tt_trace_lval.stateval.state_val = TT_CREATED;
			return _TT_TRACE_STATES_VAL; 
		}

TT_SENT		{ 
			tt_trace_lval.stateval.state_type = _TT_TRACE_STATES_TT;
			tt_trace_lval.stateval.state_val = TT_SENT;
			return _TT_TRACE_STATES_VAL; 
		}

TT_HANDLED	{ 
			tt_trace_lval.stateval.state_type = _TT_TRACE_STATES_TT;
			tt_trace_lval.stateval.state_val = TT_HANDLED;
			return _TT_TRACE_STATES_VAL; 
		}

TT_FAILED	{ 
			tt_trace_lval.stateval.state_type = _TT_TRACE_STATES_TT;
			tt_trace_lval.stateval.state_val = TT_FAILED;
			return _TT_TRACE_STATES_VAL; 
		}

TT_QUEUED	{ 
			tt_trace_lval.stateval.state_type = _TT_TRACE_STATES_TT;
			tt_trace_lval.stateval.state_val = TT_QUEUED;
			return _TT_TRACE_STATES_VAL; 
		}

TT_STARTED	{ 
			tt_trace_lval.stateval.state_type = _TT_TRACE_STATES_TT;
			tt_trace_lval.stateval.state_val = TT_STARTED;
			return _TT_TRACE_STATES_VAL; 
		}

TT_REJECTED	{ 
			tt_trace_lval.stateval.state_type = _TT_TRACE_STATES_TT;
			tt_trace_lval.stateval.state_val = TT_REJECTED;
			return _TT_TRACE_STATES_VAL; 
		}

[a-zA-Z0-9_]+	{
			strcpy(tt_trace_lval.string, tt_trace_text);
			return _TT_TRACE_ALPHANUM;
		}

[\/0-9:=@A-Za-z_,\.]+ {
			strcpy(tt_trace_lval.string, tt_trace_text);
			return _TT_TRACE_FILENAME;
		}

version{SP}[0-9]+ {
			int vers;
			char* num_ptr = strpbrk(tt_trace_text, "0123456789");
			sscanf(num_ptr, "%d", &vers);
			tt_trace_lval.tokenval = vers;
			
			return _TT_TRACE_VERSION;
		}
%%
