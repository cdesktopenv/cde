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
/* $XConsortium: frozen.tt_trace_gram.c /main/3 1995/10/23 10:35:40 rswiston $ */

# line 16 "util/tt_trace_gram.y"

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




# line 59 "util/tt_trace_gram.y"
typedef union
#ifdef __cplusplus
	TT_TRACE_STYPE
#endif
 {
	char	special;
	char	string[MAXPATHLEN];
	int		tokenval;
	struct {
		int state_type;
		int state_val;
	} stateval;
} TT_TRACE_STYPE;
# define _TT_TRACE_FOLLOW 257
# define _TT_TRACE_SINK 258
# define _TT_TRACE_SINK_APPEND 259
# define _TT_TRACE_FUNCTIONS 260
# define _TT_TRACE_ATTRIBUTES 261
# define _TT_TRACE_STATES 262
# define _TT_TRACE_OPS 263
# define _TT_TRACE_SENDER_PTYPES 264
# define _TT_TRACE_HANDLER_PTYPES 265
# define _TT_TRACE_ON 266
# define _TT_TRACE_OFF 267
# define _TT_TRACE_ALL 268
# define _TT_TRACE_NONE 269
# define _TT_TRACE_STATES_NEDD 270
# define _TT_TRACE_STATES_TT 271
# define _TT_TRACE_EDGE 272
# define _TT_TRACE_DELIVER 273
# define _TT_TRACE_DISPATCH 274
# define _TT_TRACE_VERSION 275
# define _TT_TRACE_IVAL 276
# define _TT_TRACE_STATES_VAL 277
# define _TT_TRACE_ALPHANUM 278
# define _TT_TRACE_FILENAME 279
# define _TT_TRACE_LINETERM 280
# define _TT_TRACE_PARSE_EOF 281

#include <stdlib.h>
#include <memory.h>
#include <values.h>

#ifdef __cplusplus

#ifndef tt_trace_error
	void tt_trace_error(const char *);
#endif

#ifndef tt_trace_lex
#ifdef __EXTERN_C__
	extern "C" { int tt_trace_lex(void); }
#else
	int tt_trace_lex(void);
#endif
#endif
	int tt_trace_parse(void);

#endif
#define tt_trace_clearin tt_trace_char = -1
#define tt_trace_errok tt_trace_errflag = 0
extern int tt_trace_char;
extern int tt_trace_errflag;
TT_TRACE_STYPE tt_trace_lval;
TT_TRACE_STYPE tt_trace_val;
typedef int tt_trace_tabelem;
#ifndef TT_TRACE_MAXDEPTH
#define TT_TRACE_MAXDEPTH 150
#endif
#if TT_TRACE_MAXDEPTH > 0
int tt_trace__tt_trace_s[TT_TRACE_MAXDEPTH], *tt_trace_s = tt_trace__tt_trace_s;
TT_TRACE_STYPE tt_trace__tt_trace_v[TT_TRACE_MAXDEPTH], *tt_trace_v = tt_trace__tt_trace_v;
#else	/* user does initial allocation */
int *tt_trace_s;
TT_TRACE_STYPE *tt_trace_v;
#endif
static int tt_trace_maxdepth = TT_TRACE_MAXDEPTH;
# define TT_TRACE_ERRCODE 256

# line 250 "util/tt_trace_gram.y"




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
tt_trace_tabelem tt_trace_exca[] ={
-1, 1,
	0, -1,
	-2, 0,
	};
# define TT_TRACE_NPROD 40
# define TT_TRACE_LAST 61
tt_trace_tabelem tt_trace_act[]={

    13,    14,    15,    16,    18,    28,    25,    26,    27,    40,
    39,    60,    46,    41,    47,    44,    59,    45,    12,    42,
    36,    43,    37,    11,    58,    57,    56,    55,    54,    34,
    33,    32,    31,    53,    29,    52,    51,    50,    49,    38,
    48,    35,    30,    22,    21,    20,    19,    24,    17,    23,
    10,     9,     8,     7,     6,     5,     4,     3,     2,    -1,
     1 };
tt_trace_tabelem tt_trace_pact[]={

-10000000,  -257,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,
-10000000,-10000000,  -246,  -234,  -247,  -249,  -235,  -258,  -237,  -267,
  -259,  -263,  -266,  -238,  -239,  -242,  -243,  -245,-10000000,-10000000,
  -252,  -253,  -254,  -255,  -256,  -264,-10000000,-10000000,  -269,-10000000,
-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,
-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,
-10000000 };
tt_trace_tabelem tt_trace_pgo[]={

     0,    60,    59,    58,    57,    56,    55,    54,    53,    52,
    51,    50,    49,    48,    47,    46,    45,    44,    43 };
tt_trace_tabelem tt_trace_r1[]={

     0,     2,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     3,     4,     5,     5,     5,     5,    12,
    13,    13,     6,     6,     7,    14,    15,    15,    15,    15,
     8,    16,    16,     9,    17,    17,    10,    18,    18,    11 };
tt_trace_tabelem tt_trace_r2[]={

     0,     5,     0,     5,     5,     5,     5,     5,     5,     5,
     5,     5,     4,     4,     7,     7,     7,     7,     7,     3,
     5,     5,     7,     4,     7,     3,     5,     5,     5,     5,
     4,     5,     5,     4,     5,     5,     4,     5,     5,     4 };
tt_trace_tabelem tt_trace_chk[]={

-10000000,    -1,    -3,    -4,    -5,    -6,    -7,    -8,    -9,   -10,
   -11,   280,   275,   257,   258,   259,   260,   -13,   261,   -15,
   -16,   -17,   -18,   -12,   -14,   263,   264,   265,   262,   280,
   276,   279,   278,   279,   278,   276,   278,   280,   276,   277,
   276,   280,   278,   280,   278,   280,   278,   280,   278,   277,
   276,   278,   278,   278,   280,   280,   280,   280,   280,   280,
   280 };
tt_trace_tabelem tt_trace_def[]={

     2,    -2,     3,     4,     5,     6,     7,     8,     9,    10,
    11,    12,     0,     0,     0,     0,    19,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    25,    13,
     0,     0,     0,     0,     0,     0,    21,    23,     0,    27,
    29,    30,    32,    33,    35,    36,    38,    39,    20,    26,
    28,    31,    34,    37,    14,    15,    17,    16,    18,    22,
    24 };
typedef struct
#ifdef __cplusplus
	tt_trace_toktype
#endif
{ char *t_name; int t_val; } tt_trace_toktype;
#ifndef TT_TRACE_DEBUG
#	define TT_TRACE_DEBUG	0	/* don't allow debugging */
#endif

#if TT_TRACE_DEBUG

tt_trace_toktype tt_trace_toks[] =
{
	"_TT_TRACE_FOLLOW",	257,
	"_TT_TRACE_SINK",	258,
	"_TT_TRACE_SINK_APPEND",	259,
	"_TT_TRACE_FUNCTIONS",	260,
	"_TT_TRACE_ATTRIBUTES",	261,
	"_TT_TRACE_STATES",	262,
	"_TT_TRACE_OPS",	263,
	"_TT_TRACE_SENDER_PTYPES",	264,
	"_TT_TRACE_HANDLER_PTYPES",	265,
	"_TT_TRACE_ON",	266,
	"_TT_TRACE_OFF",	267,
	"_TT_TRACE_ALL",	268,
	"_TT_TRACE_NONE",	269,
	"_TT_TRACE_STATES_NEDD",	270,
	"_TT_TRACE_STATES_TT",	271,
	"_TT_TRACE_EDGE",	272,
	"_TT_TRACE_DELIVER",	273,
	"_TT_TRACE_DISPATCH",	274,
	"_TT_TRACE_VERSION",	275,
	"_TT_TRACE_IVAL",	276,
	"_TT_TRACE_STATES_VAL",	277,
	"_TT_TRACE_ALPHANUM",	278,
	"_TT_TRACE_FILENAME",	279,
	"_TT_TRACE_LINETERM",	280,
	"_TT_TRACE_PARSE_EOF",	281,
	"-unknown-",	-1	/* ends search */
};

char * tt_trace_reds[] =
{
	"-no such reduction-",
	"finish : list _TT_TRACE_PARSE_EOF",
	"list : /* empty */",
	"list : list version_list",
	"list : list follow_list",
	"list : list sink_list",
	"list : list functions_list",
	"list : list attributes_list",
	"list : list states_list",
	"list : list ops_list",
	"list : list sender_ptypes_list",
	"list : list handler_ptypes_list",
	"list : list _TT_TRACE_LINETERM",
	"version_list : _TT_TRACE_VERSION _TT_TRACE_LINETERM",
	"follow_list : _TT_TRACE_FOLLOW _TT_TRACE_IVAL _TT_TRACE_LINETERM",
	"sink_list : _TT_TRACE_SINK _TT_TRACE_FILENAME _TT_TRACE_LINETERM",
	"sink_list : _TT_TRACE_SINK_APPEND _TT_TRACE_FILENAME _TT_TRACE_LINETERM",
	"sink_list : _TT_TRACE_SINK _TT_TRACE_ALPHANUM _TT_TRACE_LINETERM",
	"sink_list : _TT_TRACE_SINK_APPEND _TT_TRACE_ALPHANUM _TT_TRACE_LINETERM",
	"functionkywrd : _TT_TRACE_FUNCTIONS",
	"functionlist : functionkywrd _TT_TRACE_ALPHANUM",
	"functionlist : functionlist _TT_TRACE_ALPHANUM",
	"functions_list : _TT_TRACE_FUNCTIONS _TT_TRACE_IVAL _TT_TRACE_LINETERM",
	"functions_list : functionlist _TT_TRACE_LINETERM",
	"attributes_list : _TT_TRACE_ATTRIBUTES _TT_TRACE_IVAL _TT_TRACE_LINETERM",
	"stateskywrd : _TT_TRACE_STATES",
	"stateslist : stateskywrd _TT_TRACE_STATES_VAL",
	"stateslist : stateslist _TT_TRACE_STATES_VAL",
	"stateslist : stateskywrd _TT_TRACE_IVAL",
	"stateslist : stateslist _TT_TRACE_IVAL",
	"states_list : stateslist _TT_TRACE_LINETERM",
	"oplist : _TT_TRACE_OPS _TT_TRACE_ALPHANUM",
	"oplist : oplist _TT_TRACE_ALPHANUM",
	"ops_list : oplist _TT_TRACE_LINETERM",
	"senderptypeslist : _TT_TRACE_SENDER_PTYPES _TT_TRACE_ALPHANUM",
	"senderptypeslist : senderptypeslist _TT_TRACE_ALPHANUM",
	"sender_ptypes_list : senderptypeslist _TT_TRACE_LINETERM",
	"handlerptypeslist : _TT_TRACE_HANDLER_PTYPES _TT_TRACE_ALPHANUM",
	"handlerptypeslist : handlerptypeslist _TT_TRACE_ALPHANUM",
	"handler_ptypes_list : handlerptypeslist _TT_TRACE_LINETERM",
};
#endif /* TT_TRACE_DEBUG */
/*
 * Copyright (c) 1993 by Sun Microsystems, Inc.
 */

#pragma ident	"@(#)yaccpar	6.12	93/06/07 SMI"

/*
** Skeleton parser driver for yacc output
*/

/*
** yacc user known macros and defines
*/
#define TT_TRACE_ERROR		goto tt_trace_errlab
#define TT_TRACE_ACCEPT	return(0)
#define TT_TRACE_ABORT		return(1)
#define TT_TRACE_BACKUP( newtoken, newvalue )\
{\
	if ( tt_trace_char >= 0 || ( tt_trace_r2[ tt_trace_tmp ] >> 1 ) != 1 )\
	{\
		tt_trace_error( "syntax error - cannot backup" );\
		goto tt_trace_errlab;\
	}\
	tt_trace_char = newtoken;\
	tt_trace_state = *tt_trace_ps;\
	tt_trace_lval = newvalue;\
	goto tt_trace_newstate;\
}
#define TT_TRACE_RECOVERING()	(!!tt_trace_errflag)
#define TT_TRACE_NEW(type)	malloc(sizeof(type) * tt_trace_newmax)
#define TT_TRACE_COPY(to, from, type) \
	(type *) memcpy(to, (char *) from, tt_trace_newmax * sizeof(type))
#define TT_TRACE_ENLARGE( from, type) \
	(type *) realloc((char *) from, tt_trace_newmax * sizeof(type))
#ifndef TT_TRACE_DEBUG
#	define TT_TRACE_DEBUG	1	/* make debugging available */
#endif

/*
** user known globals
*/
int tt_trace_debug;			/* set to 1 to get debugging */

/*
** driver internal defines
*/
#define TT_TRACE_FLAG		(-10000000)

/*
** global variables used by the parser
*/
TT_TRACE_STYPE *tt_trace_pv;			/* top of value stack */
int *tt_trace_ps;			/* top of state stack */

int tt_trace_state;			/* current state */
int tt_trace_tmp;			/* extra var (lasts between blocks) */

int tt_trace_nerrs;			/* number of errors */
int tt_trace_errflag;			/* error recovery flag */
int tt_trace_char;			/* current input token number */



#ifdef TT_TRACE_NMBCHARS
#define TT_TRACE_LEX()		tt_trace_cvtok(tt_trace_lex())
/*
** tt_trace_cvtok - return a token if i is a wchar_t value that exceeds 255.
**	If i<255, i itself is the token.  If i>255 but the neither 
**	of the 30th or 31st bit is on, i is already a token.
*/
#if defined(__STDC__) || defined(__cplusplus)
int tt_trace_cvtok(int i)
#else
int tt_trace_cvtok(i) int i;
#endif
{
	int first = 0;
	int last = TT_TRACE_NMBCHARS - 1;
	int mid;
	wchar_t j;

	if(i&0x60000000){/*Must convert to a token. */
		if( tt_trace_mbchars[last].character < i ){
			return i;/*Giving up*/
		}
		while ((last>=first)&&(first>=0)) {/*Binary search loop*/
			mid = (first+last)/2;
			j = tt_trace_mbchars[mid].character;
			if( j==i ){/*Found*/ 
				return tt_trace_mbchars[mid].tvalue;
			}else if( j<i ){
				first = mid + 1;
			}else{
				last = mid -1;
			}
		}
		/*No entry in the table.*/
		return i;/* Giving up.*/
	}else{/* i is already a token. */
		return i;
	}
}
#else/*!TT_TRACE_NMBCHARS*/
#define TT_TRACE_LEX()		tt_trace_lex()
#endif/*!TT_TRACE_NMBCHARS*/

/*
** tt_trace_parse - return 0 if worked, 1 if syntax error not recovered from
*/
#if defined(__STDC__) || defined(__cplusplus)
int tt_trace_parse(void)
#else
int tt_trace_parse()
#endif
{
	register TT_TRACE_STYPE *tt_trace_pvt;	/* top of value stack for $vars */

#if defined(__cplusplus) || defined(lint)
/*
	hacks to please C++ and lint - goto's inside switch should never be
	executed; tt_trace_pvt is set to 0 to avoid "used before set" warning.
*/
	static int __yaccpar_lint_hack__ = 0;
	switch (__yaccpar_lint_hack__)
	{
		case 1: goto tt_trace_errlab;
		case 2: goto tt_trace_newstate;
	}
	tt_trace_pvt = 0;
#endif

	/*
	** Initialize externals - tt_trace_parse may be called more than once
	*/
	tt_trace_pv = &tt_trace_v[-1];
	tt_trace_ps = &tt_trace_s[-1];
	tt_trace_state = 0;
	tt_trace_tmp = 0;
	tt_trace_nerrs = 0;
	tt_trace_errflag = 0;
	tt_trace_char = -1;

#if TT_TRACE_MAXDEPTH <= 0
	if (tt_trace_maxdepth <= 0)
	{
		if ((tt_trace_maxdepth = TT_TRACE_EXPAND(0)) <= 0)
		{
			tt_trace_error("yacc initialization error");
			TT_TRACE_ABORT;
		}
	}
#endif

	{
		register TT_TRACE_STYPE *tt_trace__pv;	/* top of value stack */
		register int *tt_trace__ps;		/* top of state stack */
		register int tt_trace__state;		/* current state */
		register int  tt_trace__n;		/* internal state number info */
	goto tt_trace_stack;	/* moved from 6 lines above to here to please C++ */

		/*
		** get globals into registers.
		** branch to here only if TT_TRACE_BACKUP was called.
		*/
	tt_trace_newstate:
		tt_trace__pv = tt_trace_pv;
		tt_trace__ps = tt_trace_ps;
		tt_trace__state = tt_trace_state;
		goto tt_trace__newstate;

		/*
		** get globals into registers.
		** either we just started, or we just finished a reduction
		*/
	tt_trace_stack:
		tt_trace__pv = tt_trace_pv;
		tt_trace__ps = tt_trace_ps;
		tt_trace__state = tt_trace_state;

		/*
		** top of for (;;) loop while no reductions done
		*/
	tt_trace__stack:
		/*
		** put a state and value onto the stacks
		*/
#if TT_TRACE_DEBUG
		/*
		** if debugging, look up token value in list of value vs.
		** name pairs.  0 and negative (-1) are special values.
		** Note: linear search is used since time is not a real
		** consideration while debugging.
		*/
		if ( tt_trace_debug )
		{
			register int tt_trace__i;

			printf( "State %d, token ", tt_trace__state );
			if ( tt_trace_char == 0 )
				printf( "end-of-file\n" );
			else if ( tt_trace_char < 0 )
				printf( "-none-\n" );
			else
			{
				for ( tt_trace__i = 0; tt_trace_toks[tt_trace__i].t_val >= 0;
					tt_trace__i++ )
				{
					if ( tt_trace_toks[tt_trace__i].t_val == tt_trace_char )
						break;
				}
				printf( "%s\n", tt_trace_toks[tt_trace__i].t_name );
			}
		}
#endif /* TT_TRACE_DEBUG */
		if ( ++tt_trace__ps >= &tt_trace_s[ tt_trace_maxdepth ] )	/* room on stack? */
		{
			/*
			** reallocate and recover.  Note that pointers
			** have to be reset, or bad things will happen
			*/
			int tt_trace_ps_index = (tt_trace__ps - tt_trace_s);
			int tt_trace_pv_index = (tt_trace__pv - tt_trace_v);
			int tt_trace_pvt_index = (tt_trace_pvt - tt_trace_v);
			int tt_trace_newmax;
#ifdef TT_TRACE_EXPAND
			tt_trace_newmax = TT_TRACE_EXPAND(tt_trace_maxdepth);
#else
			tt_trace_newmax = 2 * tt_trace_maxdepth;	/* double table size */
			if (tt_trace_maxdepth == TT_TRACE_MAXDEPTH)	/* first time growth */
			{
				char *newtt_trace_s = (char *)TT_TRACE_NEW(int);
				char *newtt_trace_v = (char *)TT_TRACE_NEW(TT_TRACE_STYPE);
				if (newtt_trace_s != 0 && newtt_trace_v != 0)
				{
					tt_trace_s = TT_TRACE_COPY(newtt_trace_s, tt_trace_s, int);
					tt_trace_v = TT_TRACE_COPY(newtt_trace_v, tt_trace_v, TT_TRACE_STYPE);
				}
				else
					tt_trace_newmax = 0;	/* failed */
			}
			else				/* not first time */
			{
				tt_trace_s = TT_TRACE_ENLARGE(tt_trace_s, int);
				tt_trace_v = TT_TRACE_ENLARGE(tt_trace_v, TT_TRACE_STYPE);
				if (tt_trace_s == 0 || tt_trace_v == 0)
					tt_trace_newmax = 0;	/* failed */
			}
#endif
			if (tt_trace_newmax <= tt_trace_maxdepth)	/* tables not expanded */
			{
				tt_trace_error( "yacc stack overflow" );
				TT_TRACE_ABORT;
			}
			tt_trace_maxdepth = tt_trace_newmax;

			tt_trace__ps = tt_trace_s + tt_trace_ps_index;
			tt_trace__pv = tt_trace_v + tt_trace_pv_index;
			tt_trace_pvt = tt_trace_v + tt_trace_pvt_index;
		}
		*tt_trace__ps = tt_trace__state;
		*++tt_trace__pv = tt_trace_val;

		/*
		** we have a new state - find out what to do
		*/
	tt_trace__newstate:
		if ( ( tt_trace__n = tt_trace_pact[ tt_trace__state ] ) <= TT_TRACE_FLAG )
			goto tt_trace_default;		/* simple state */
#if TT_TRACE_DEBUG
		/*
		** if debugging, need to mark whether new token grabbed
		*/
		tt_trace_tmp = tt_trace_char < 0;
#endif
		if ( ( tt_trace_char < 0 ) && ( ( tt_trace_char = TT_TRACE_LEX() ) < 0 ) )
			tt_trace_char = 0;		/* reached EOF */
#if TT_TRACE_DEBUG
		if ( tt_trace_debug && tt_trace_tmp )
		{
			register int tt_trace__i;

			printf( "Received token " );
			if ( tt_trace_char == 0 )
				printf( "end-of-file\n" );
			else if ( tt_trace_char < 0 )
				printf( "-none-\n" );
			else
			{
				for ( tt_trace__i = 0; tt_trace_toks[tt_trace__i].t_val >= 0;
					tt_trace__i++ )
				{
					if ( tt_trace_toks[tt_trace__i].t_val == tt_trace_char )
						break;
				}
				printf( "%s\n", tt_trace_toks[tt_trace__i].t_name );
			}
		}
#endif /* TT_TRACE_DEBUG */
		if ( ( ( tt_trace__n += tt_trace_char ) < 0 ) || ( tt_trace__n >= TT_TRACE_LAST ) )
			goto tt_trace_default;
		if ( tt_trace_chk[ tt_trace__n = tt_trace_act[ tt_trace__n ] ] == tt_trace_char )	/*valid shift*/
		{
			tt_trace_char = -1;
			tt_trace_val = tt_trace_lval;
			tt_trace__state = tt_trace__n;
			if ( tt_trace_errflag > 0 )
				tt_trace_errflag--;
			goto tt_trace__stack;
		}

	tt_trace_default:
		if ( ( tt_trace__n = tt_trace_def[ tt_trace__state ] ) == -2 )
		{
#if TT_TRACE_DEBUG
			tt_trace_tmp = tt_trace_char < 0;
#endif
			if ( ( tt_trace_char < 0 ) && ( ( tt_trace_char = TT_TRACE_LEX() ) < 0 ) )
				tt_trace_char = 0;		/* reached EOF */
#if TT_TRACE_DEBUG
			if ( tt_trace_debug && tt_trace_tmp )
			{
				register int tt_trace__i;

				printf( "Received token " );
				if ( tt_trace_char == 0 )
					printf( "end-of-file\n" );
				else if ( tt_trace_char < 0 )
					printf( "-none-\n" );
				else
				{
					for ( tt_trace__i = 0;
						tt_trace_toks[tt_trace__i].t_val >= 0;
						tt_trace__i++ )
					{
						if ( tt_trace_toks[tt_trace__i].t_val
							== tt_trace_char )
						{
							break;
						}
					}
					printf( "%s\n", tt_trace_toks[tt_trace__i].t_name );
				}
			}
#endif /* TT_TRACE_DEBUG */
			/*
			** look through exception table
			*/
			{
				register int *tt_trace_xi = tt_trace_exca;

				while ( ( *tt_trace_xi != -1 ) ||
					( tt_trace_xi[1] != tt_trace__state ) )
				{
					tt_trace_xi += 2;
				}
				while ( ( *(tt_trace_xi += 2) >= 0 ) &&
					( *tt_trace_xi != tt_trace_char ) )
					;
				if ( ( tt_trace__n = tt_trace_xi[1] ) < 0 )
					TT_TRACE_ACCEPT;
			}
		}

		/*
		** check for syntax error
		*/
		if ( tt_trace__n == 0 )	/* have an error */
		{
			/* no worry about speed here! */
			switch ( tt_trace_errflag )
			{
			case 0:		/* new error */
				tt_trace_error( "syntax error" );
				goto skip_init;
			tt_trace_errlab:
				/*
				** get globals into registers.
				** we have a user generated syntax type error
				*/
				tt_trace__pv = tt_trace_pv;
				tt_trace__ps = tt_trace_ps;
				tt_trace__state = tt_trace_state;
			skip_init:
				tt_trace_nerrs++;
				/* FALLTHRU */
			case 1:
			case 2:		/* incompletely recovered error */
					/* try again... */
				tt_trace_errflag = 3;
				/*
				** find state where "error" is a legal
				** shift action
				*/
				while ( tt_trace__ps >= tt_trace_s )
				{
					tt_trace__n = tt_trace_pact[ *tt_trace__ps ] + TT_TRACE_ERRCODE;
					if ( tt_trace__n >= 0 && tt_trace__n < TT_TRACE_LAST &&
						tt_trace_chk[tt_trace_act[tt_trace__n]] == TT_TRACE_ERRCODE)					{
						/*
						** simulate shift of "error"
						*/
						tt_trace__state = tt_trace_act[ tt_trace__n ];
						goto tt_trace__stack;
					}
					/*
					** current state has no shift on
					** "error", pop stack
					*/
#if TT_TRACE_DEBUG
#	define _POP_ "Error recovery pops state %d, uncovers state %d\n"
					if ( tt_trace_debug )
						printf( _POP_, *tt_trace__ps,
							tt_trace__ps[-1] );
#	undef _POP_
#endif
					tt_trace__ps--;
					tt_trace__pv--;
				}
				/*
				** there is no state on stack with "error" as
				** a valid shift.  give up.
				*/
				TT_TRACE_ABORT;
			case 3:		/* no shift yet; eat a token */
#if TT_TRACE_DEBUG
				/*
				** if debugging, look up token in list of
				** pairs.  0 and negative shouldn't occur,
				** but since timing doesn't matter when
				** debugging, it doesn't hurt to leave the
				** tests here.
				*/
				if ( tt_trace_debug )
				{
					register int tt_trace__i;

					printf( "Error recovery discards " );
					if ( tt_trace_char == 0 )
						printf( "token end-of-file\n" );
					else if ( tt_trace_char < 0 )
						printf( "token -none-\n" );
					else
					{
						for ( tt_trace__i = 0;
							tt_trace_toks[tt_trace__i].t_val >= 0;
							tt_trace__i++ )
						{
							if ( tt_trace_toks[tt_trace__i].t_val
								== tt_trace_char )
							{
								break;
							}
						}
						printf( "token %s\n",
							tt_trace_toks[tt_trace__i].t_name );
					}
				}
#endif /* TT_TRACE_DEBUG */
				if ( tt_trace_char == 0 )	/* reached EOF. quit */
					TT_TRACE_ABORT;
				tt_trace_char = -1;
				goto tt_trace__newstate;
			}
		}/* end if ( tt_trace__n == 0 ) */
		/*
		** reduction by production tt_trace__n
		** put stack tops, etc. so things right after switch
		*/
#if TT_TRACE_DEBUG
		/*
		** if debugging, print the string that is the user's
		** specification of the reduction which is just about
		** to be done.
		*/
		if ( tt_trace_debug )
			printf( "Reduce by (%d) \"%s\"\n",
				tt_trace__n, tt_trace_reds[ tt_trace__n ] );
#endif
		tt_trace_tmp = tt_trace__n;			/* value to switch over */
		tt_trace_pvt = tt_trace__pv;			/* $vars top of value stack */
		/*
		** Look in goto table for next state
		** Sorry about using tt_trace__state here as temporary
		** register variable, but why not, if it works...
		** If tt_trace_r2[ tt_trace__n ] doesn't have the low order bit
		** set, then there is no action to be done for
		** this reduction.  So, no saving & unsaving of
		** registers done.  The only difference between the
		** code just after the if and the body of the if is
		** the goto tt_trace__stack in the body.  This way the test
		** can be made before the choice of what to do is needed.
		*/
		{
			/* length of production doubled with extra bit */
			register int tt_trace__len = tt_trace_r2[ tt_trace__n ];

			if ( !( tt_trace__len & 01 ) )
			{
				tt_trace__len >>= 1;
				tt_trace_val = ( tt_trace__pv -= tt_trace__len )[1];	/* $$ = $1 */
				tt_trace__state = tt_trace_pgo[ tt_trace__n = tt_trace_r1[ tt_trace__n ] ] +
					*( tt_trace__ps -= tt_trace__len ) + 1;
				if ( tt_trace__state >= TT_TRACE_LAST ||
					tt_trace_chk[ tt_trace__state =
					tt_trace_act[ tt_trace__state ] ] != -tt_trace__n )
				{
					tt_trace__state = tt_trace_act[ tt_trace_pgo[ tt_trace__n ] ];
				}
				goto tt_trace__stack;
			}
			tt_trace__len >>= 1;
			tt_trace_val = ( tt_trace__pv -= tt_trace__len )[1];	/* $$ = $1 */
			tt_trace__state = tt_trace_pgo[ tt_trace__n = tt_trace_r1[ tt_trace__n ] ] +
				*( tt_trace__ps -= tt_trace__len ) + 1;
			if ( tt_trace__state >= TT_TRACE_LAST ||
				tt_trace_chk[ tt_trace__state = tt_trace_act[ tt_trace__state ] ] != -tt_trace__n )
			{
				tt_trace__state = tt_trace_act[ tt_trace_pgo[ tt_trace__n ] ];
			}
		}
					/* save until reenter driver code */
		tt_trace_state = tt_trace__state;
		tt_trace_ps = tt_trace__ps;
		tt_trace_pv = tt_trace__pv;
	}
	/*
	** code supplied by user is placed in this switch
	*/
	switch( tt_trace_tmp )
	{
		
case 1:
# line 97 "util/tt_trace_gram.y"
{
			if (parsed)
				ret = 0;
			else
				ret = 1;
			parsed = 0;
			return ret;
		} break;
case 3:
# line 107 "util/tt_trace_gram.y"
{ parsed = 1; wrapped = 0; } break;
case 4:
# line 108 "util/tt_trace_gram.y"
{ parsed = 1; wrapped = 0; } break;
case 5:
# line 109 "util/tt_trace_gram.y"
{ parsed = 1; wrapped = 0; } break;
case 6:
# line 110 "util/tt_trace_gram.y"
{ parsed = 1; wrapped = 0; } break;
case 7:
# line 111 "util/tt_trace_gram.y"
{ parsed = 1; wrapped = 0; } break;
case 8:
# line 112 "util/tt_trace_gram.y"
{ parsed = 1; wrapped = 0; } break;
case 9:
# line 113 "util/tt_trace_gram.y"
{ parsed = 1; wrapped = 0; } break;
case 10:
# line 114 "util/tt_trace_gram.y"
{ parsed = 1; wrapped = 0; } break;
case 11:
# line 115 "util/tt_trace_gram.y"
{ parsed = 1; wrapped = 0; } break;
case 14:
# line 123 "util/tt_trace_gram.y"
{ _tt_process_follow(tt_trace_pvt[-1].tokenval); } break;
case 15:
# line 127 "util/tt_trace_gram.y"
{ _tt_process_sink(tt_trace_pvt[-1].string, O_RDWR | O_CREAT); } break;
case 16:
# line 129 "util/tt_trace_gram.y"
{ _tt_process_sink(tt_trace_pvt[-1].string, O_APPEND | O_CREAT); } break;
case 17:
# line 137 "util/tt_trace_gram.y"
{ _tt_process_sink(tt_trace_pvt[-1].string, O_RDWR | O_CREAT); } break;
case 18:
# line 139 "util/tt_trace_gram.y"
{ _tt_process_sink(tt_trace_pvt[-1].string, O_APPEND | O_CREAT); } break;
case 19:
# line 143 "util/tt_trace_gram.y"
{ _tt_clear_functions(); } break;
case 20:
# line 147 "util/tt_trace_gram.y"
{ _tt_process_functionlist(tt_trace_pvt[-0].string); } break;
case 21:
# line 149 "util/tt_trace_gram.y"
{ _tt_process_functionlist(tt_trace_pvt[-0].string); } break;
case 22:
# line 153 "util/tt_trace_gram.y"
{
					all = (tt_trace_pvt[-1].tokenval == _TT_TRACE_ALL ? 1 : 0);
					_tt_process_functions_allnone(all);
				} break;
case 24:
# line 161 "util/tt_trace_gram.y"
{
					all = (tt_trace_pvt[-1].tokenval == _TT_TRACE_ALL ? 1 : 0);
					_tt_process_attributes(tt_trace_pvt[-1].tokenval);
				} break;
case 25:
# line 170 "util/tt_trace_gram.y"
{ _tt_clear_states(); } break;
case 26:
# line 173 "util/tt_trace_gram.y"
{
					/* process_states can't fail here because the
					 * grammar ensures that this is the first
					 * value on the line
					 */
					(void) _tt_process_state(tt_trace_pvt[-0].stateval.state_type,
							     tt_trace_pvt[-0].stateval.state_val);
				} break;
case 27:
# line 182 "util/tt_trace_gram.y"
{
					if (!_tt_process_state(tt_trace_pvt[-0].stateval.state_type,
							  tt_trace_pvt[-0].stateval.state_val)) {
						_tt_trace_iserr = 2;
						tt_trace_error("Can't mix none with other state values");
					}
				} break;
case 28:
# line 190 "util/tt_trace_gram.y"
{
					/* process_states can't fail here because the
					 * grammar ensures that this is the first
					 * value on the line
					 */

					if (tt_trace_pvt[-0].tokenval == _TT_TRACE_NONE) {
						_tt_process_state(_TT_TRACE_STATES_NEDD,
							      _TT_TRACE_NONE);
					}
					else {	 /* Syntax error */
						_tt_trace_iserr = 3;
						tt_trace_error("Bad value for state");
					}
				} break;
case 29:
# line 206 "util/tt_trace_gram.y"
{
					if (tt_trace_pvt[-0].tokenval == _TT_TRACE_NONE) {
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
				} break;
case 31:
# line 227 "util/tt_trace_gram.y"
{ _tt_create_op_list(tt_trace_pvt[-0].string); } break;
case 32:
# line 229 "util/tt_trace_gram.y"
{ _tt_add_op(tt_trace_pvt[-0].string); } break;
case 34:
# line 235 "util/tt_trace_gram.y"
{ _tt_create_sp_list(tt_trace_pvt[-0].string); } break;
case 35:
# line 237 "util/tt_trace_gram.y"
{ _tt_add_sender_ptype(tt_trace_pvt[-0].string); } break;
case 37:
# line 243 "util/tt_trace_gram.y"
{ _tt_create_hp_list(tt_trace_pvt[-0].string); } break;
case 38:
# line 245 "util/tt_trace_gram.y"
{ _tt_add_handler_ptype(tt_trace_pvt[-0].string); } break;
	}
	goto tt_trace_stack;		/* reset registers in driver code */
}

