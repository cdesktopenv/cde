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
/* $TOG: frozen.mp_types_gram.C /main/4 1999/10/15 12:31:31 mgreess $ */

# line 16 "mp_types_gram.y"

#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include "mp/mp_arg.h"
#include "mp_otype.h"
#include "mp_ptype.h"
#include "mp_signature.h"
#include "mp_types_table.h"

extern int yyleng;
extern char yytext[];


# line 33 "mp_types_gram.y"
typedef union
#ifdef __cplusplus
	YYSTYPE
#endif
	{
	int		num;
	char		*str;
	} YYSTYPE;
# define PER_FILE 257
# define PER_SESSION 258
# define START 259
# define FILE_SCOPE 260
# define SESSION_SCOPE 261
# define FILE_IN_SESSION 262
# define REQUEST 263
# define NOTIFICATION 264
# define VOID_ARGS 265
# define QUEUE 266
# define OPNUM 267
# define IN 268
# define OUT 269
# define INOUT 270
# define OTYPE 271
# define INHERIT 272
# define FROM 273
# define PTYPE 274
# define OBSERVE 275
# define HANDLE 276
# define HANDLE_PUSH 277
# define HANDLE_ROTATE 278
# define COLON 279
# define SEMICOLON 280
# define LCURL 281
# define RCURL 282
# define INFER 283
# define LPAREN 284
# define RPAREN 285
# define COMMA 286
# define EQUAL 287
# define CONTEXT 288
# define TT_IDENTIFIER 289
# define TT_STRING 290
# define TT_NUMBER 291

#include <memory.h>

#if defined (__hpux)
/*
 * On HP MAXINT is defined in both <values.h> and <sys/param.h>
 */
#undef MAXINT
#endif
#if defined(CSRG_BASED)
#define MAXINT INT_MAX
#else
#include <values.h>
#endif

#ifdef __cplusplus

#ifndef yyerror
	void yyerror(const char *);
#endif

#ifndef yylex
#ifdef __EXTERN_C__
	extern "C" { int yylex(void); }
#else
	int yylex(void);
#endif
#endif
	int yyparse(void);

#endif
#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
extern int yychar;
extern int yyerrflag;
YYSTYPE yylval;
YYSTYPE yyval;
typedef int yytabelem;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
#if YYMAXDEPTH > 0
int yy_yys[YYMAXDEPTH], *yys = yy_yys;
YYSTYPE yy_yyv[YYMAXDEPTH], *yyv = yy_yyv;
#else	/* user does initial allocation */
int *yys;
YYSTYPE *yyv;
#endif
static int yymaxdepth = YYMAXDEPTH;
# define YYERRCODE 256

# line 427 "mp_types_gram.y"

yytabelem yyexca[] ={
-1, 1,
	0, -1,
	-2, 0,
-1, 87,
	289, 60,
	-2, 51,
-1, 88,
	289, 60,
	-2, 52,
-1, 89,
	289, 60,
	-2, 53,
-1, 90,
	289, 60,
	-2, 54,
	};
# define YYNPROD 90
# define YYLAST 200
yytabelem yyact[]={

    67,    66,   136,     8,   104,    76,     7,   107,    14,    53,
   110,    12,    78,   134,   115,   116,    94,   124,    93,    69,
    12,     5,    60,    61,    62,    63,    92,    91,   127,    54,
    24,    25,    26,    27,    28,    10,   112,    18,    75,    32,
    74,    73,    72,    71,    36,    35,    34,    33,    30,   104,
    80,   130,   126,    84,    85,    86,    84,    85,    86,    98,
    99,   100,    47,    48,    49,   122,   129,   103,   121,    77,
   125,   106,    97,    68,    13,    52,    82,    11,     2,    87,
     9,    31,    40,   105,   109,    83,    81,    79,   123,    96,
    59,    58,    57,    56,    55,    45,    44,    37,    16,   132,
   113,   101,    51,    23,    22,    39,    21,    20,    19,    38,
    29,    17,    15,    50,     6,     4,     3,    41,    42,    43,
    65,    64,    46,     1,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,    70,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    88,    89,    90,     0,     0,     0,     0,     0,
     0,     0,    95,     0,     0,     0,     0,     0,     0,   102,
     0,   108,     0,   111,     0,     0,     0,     0,     0,   114,
     0,     0,     0,     0,     0,   117,   118,   120,   119,     0,
     0,     0,     0,   128,     0,     0,     0,   133,   135,   131 };
yytabelem yypact[]={

  -268,  -268,-10000000,-10000000,-10000000,-10000000,  -246,  -278,  -281,-10000000,
-10000000,-10000000,-10000000,-10000000,-10000000,  -245,  -247,  -231,  -241,-10000000,
-10000000,-10000000,-10000000,-10000000,  -232,  -233,  -234,  -235,-10000000,-10000000,
  -281,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,  -195,  -281,-10000000,
  -280,  -280,  -280,  -280,  -253,-10000000,  -290,-10000000,-10000000,-10000000,
-10000000,-10000000,  -265,-10000000,  -241,-10000000,-10000000,-10000000,-10000000,-10000000,
  -236,  -237,  -238,  -239,  -242,  -285,-10000000,-10000000,  -276,  -215,
-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,  -256,  -258,  -267,
-10000000,  -270,-10000000,  -268,-10000000,-10000000,-10000000,  -201,  -201,  -201,
  -201,  -269,  -282,-10000000,  -212,  -279,-10000000,  -280,-10000000,-10000000,
-10000000,  -244,-10000000,-10000000,  -281,  -271,-10000000,-10000000,-10000000,-10000000,
-10000000,  -265,-10000000,  -201,-10000000,-10000000,  -282,  -276,  -194,-10000000,
  -266,  -214,-10000000,  -252,  -194,  -216,-10000000,-10000000,  -214,  -224,
  -274,  -216,-10000000,-10000000,  -289,-10000000,-10000000 };
yytabelem yypgo[]={

     0,   123,   122,   121,   120,    78,   116,   115,   114,   112,
    81,    74,   111,   110,   109,   108,   107,   106,   104,   103,
    82,   102,    75,    73,    69,   101,    77,   100,    72,    68,
    70,    66,    99,    67,    98,    97,    96,    95,    94,    93,
    92,    91,    90,    79,    89,    88,    87,    86,    76,    85,
    84,    83,    71 };
yytabelem yyr1[]={

     0,     1,     1,     5,     5,     6,    12,     8,    11,    13,
    13,    14,    14,     9,     9,    15,    15,    15,    15,    16,
    17,    18,    19,    20,    20,    21,    27,    25,    25,    32,
    32,    33,    34,     7,    26,    35,    35,    37,    36,    36,
    38,    38,    38,    38,     2,     2,     2,     4,     4,     3,
     3,    39,    40,    41,    42,    10,    10,    43,    43,    44,
    28,    28,    28,    28,    22,    23,    46,    46,    46,    47,
    47,    48,    49,    49,    49,     5,    50,    24,    24,    51,
    51,    52,    45,    45,    29,    29,    30,    30,    31,    31 };
yytabelem yyr2[]={

     0,     2,     4,     2,     2,    11,     1,     9,     3,     0,
     4,     3,     5,     0,     4,     2,     2,     2,     2,     7,
     7,     7,     7,     0,     4,    13,     1,    14,     2,     0,
     2,     5,     1,    17,     2,     0,     4,     7,     0,     4,
     2,     2,     2,     2,     3,     3,     3,     3,     5,     2,
     3,     7,     7,     7,     7,     0,     2,     0,     4,    13,
     0,     3,     3,     3,     3,     6,     0,     2,     2,     6,
     2,     7,     3,     3,     3,     3,     3,     0,     8,     6,
     2,     3,     0,     8,     0,     3,     0,     3,     0,     7 };
yytabelem yychk[]={

-10000000,    -1,    -5,    -6,    -7,   289,    -8,   274,   271,    -5,
   281,   -26,   289,   -11,   289,    -9,   -34,   -12,   282,   -15,
   -16,   -17,   -18,   -19,   275,   276,   277,   278,   281,   -13,
   279,   -10,   280,   279,   279,   279,   279,   -35,   -14,   -11,
   -20,   -20,   -20,   -20,   -36,   -37,    -2,   257,   258,   259,
   -11,   -21,   -22,   289,   282,   -38,   -39,   -40,   -41,   -42,
   275,   276,   277,   278,    -3,    -4,   291,   290,   -23,   284,
   -10,   279,   279,   279,   279,   280,   290,   -24,   288,   -46,
   265,   -47,   -48,   -49,   268,   269,   270,   -43,   -43,   -43,
   -43,   283,   284,   285,   286,    -5,   -44,   -28,   260,   261,
   262,   -25,   -26,   -33,   273,   -51,   -52,   289,   -48,   -50,
   289,   -22,   280,   -27,   -11,   285,   286,   -23,   -28,   -52,
   -24,   -29,   259,   -45,   283,   -30,   266,   280,   -29,   -31,
   267,   -30,   -32,   -33,   287,   -31,   291 };
yytabelem yydef[]={

     0,    -2,     1,     3,     4,    75,     0,     0,     0,     2,
    13,    32,    34,     6,     8,     0,     0,     9,    55,    14,
    15,    16,    17,    18,     0,     0,     0,     0,    35,     7,
     0,     5,    56,    23,    23,    23,    23,    38,    10,    11,
    19,    20,    21,    22,     0,    36,     0,    44,    45,    46,
    12,    24,     0,    64,    55,    39,    40,    41,    42,    43,
     0,     0,     0,     0,     0,    49,    50,    47,    77,    66,
    33,    57,    57,    57,    57,    37,    48,     0,     0,     0,
    67,    68,    70,     0,    72,    73,    74,    -2,    -2,    -2,
    -2,     0,     0,    65,     0,     0,    58,     0,    61,    62,
    63,     0,    26,    28,     0,     0,    80,    81,    69,    71,
    76,     0,    25,    60,    31,    78,     0,    77,    84,    79,
    82,    86,    85,     0,    84,    88,    87,    59,    86,    29,
     0,    88,    27,    30,     0,    83,    89 };
typedef struct
#ifdef __cplusplus
	yytoktype
#endif
{ char *t_name; int t_val; } yytoktype;
#ifndef YYDEBUG
#	define YYDEBUG	0	/* don't allow debugging */
#endif

#if YYDEBUG

yytoktype yytoks[] =
{
	"PER_FILE",	257,
	"PER_SESSION",	258,
	"START",	259,
	"FILE_SCOPE",	260,
	"SESSION_SCOPE",	261,
	"FILE_IN_SESSION",	262,
	"REQUEST",	263,
	"NOTIFICATION",	264,
	"VOID_ARGS",	265,
	"QUEUE",	266,
	"OPNUM",	267,
	"IN",	268,
	"OUT",	269,
	"INOUT",	270,
	"OTYPE",	271,
	"INHERIT",	272,
	"FROM",	273,
	"PTYPE",	274,
	"OBSERVE",	275,
	"HANDLE",	276,
	"HANDLE_PUSH",	277,
	"HANDLE_ROTATE",	278,
	"COLON",	279,
	"SEMICOLON",	280,
	"LCURL",	281,
	"RCURL",	282,
	"INFER",	283,
	"LPAREN",	284,
	"RPAREN",	285,
	"COMMA",	286,
	"EQUAL",	287,
	"CONTEXT",	288,
	"TT_IDENTIFIER",	289,
	"TT_STRING",	290,
	"TT_NUMBER",	291,
	"-unknown-",	-1	/* ends search */
};

char * yyreds[] =
{
	"-no such reduction-",
	"types : type",
	"types : types type",
	"type : otype",
	"type : ptype",
	"otype : header LCURL otype_body_list RCURL opt_del",
	"header : OTYPE otid",
	"header : OTYPE otid inheritance_opt",
	"otid : TT_IDENTIFIER",
	"inheritance_opt : /* empty */",
	"inheritance_opt : COLON otid_list",
	"otid_list : otid",
	"otid_list : otid_list otid",
	"otype_body_list : /* empty */",
	"otype_body_list : otype_body_list otype_body",
	"otype_body : otype_observe",
	"otype_body : otype_handle",
	"otype_body : otype_handle_push",
	"otype_body : otype_handle_rotate",
	"otype_observe : OBSERVE COLON osig_list",
	"otype_handle : HANDLE COLON osig_list",
	"otype_handle_push : HANDLE_PUSH COLON osig_list",
	"otype_handle_rotate : HANDLE_ROTATE COLON osig_list",
	"osig_list : /* empty */",
	"osig_list : osig_list osig",
	"osig : op args cntxt_opt INFER rhs SEMICOLON",
	"rhs : ptid",
	"rhs : ptid scope_opt start_opt queue_opt opnum_opt inherit_opt",
	"rhs : inherit",
	"inherit_opt : /* empty */",
	"inherit_opt : inherit",
	"inherit : FROM otid",
	"ptype : PTYPE ptid",
	"ptype : PTYPE ptid LCURL prop_list ptype_body_list RCURL opt_del",
	"ptid : TT_IDENTIFIER",
	"prop_list : /* empty */",
	"prop_list : prop_list prop",
	"prop : prop_id value SEMICOLON",
	"ptype_body_list : /* empty */",
	"ptype_body_list : ptype_body_list ptype_body",
	"ptype_body : observe",
	"ptype_body : handle",
	"ptype_body : handle_push",
	"ptype_body : handle_rotate",
	"prop_id : PER_FILE",
	"prop_id : PER_SESSION",
	"prop_id : START",
	"string_lit : TT_STRING",
	"string_lit : string_lit TT_STRING",
	"value : string_lit",
	"value : TT_NUMBER",
	"observe : OBSERVE COLON psig_list",
	"handle : HANDLE COLON psig_list",
	"handle_push : HANDLE_PUSH COLON psig_list",
	"handle_rotate : HANDLE_ROTATE COLON psig_list",
	"opt_del : /* empty */",
	"opt_del : SEMICOLON",
	"psig_list : /* empty */",
	"psig_list : psig_list psig",
	"psig : scope_opt op args cntxt_opt infer_opt SEMICOLON",
	"scope_opt : /* empty */",
	"scope_opt : FILE_SCOPE",
	"scope_opt : SESSION_SCOPE",
	"scope_opt : FILE_IN_SESSION",
	"op : TT_IDENTIFIER",
	"args : LPAREN args_aux RPAREN",
	"args_aux : /* empty */",
	"args_aux : VOID_ARGS",
	"args_aux : argspecs",
	"argspecs : argspecs COMMA argspec",
	"argspecs : argspec",
	"argspec : mode type name",
	"mode : IN",
	"mode : OUT",
	"mode : INOUT",
	"type : TT_IDENTIFIER",
	"name : TT_IDENTIFIER",
	"cntxt_opt : /* empty */",
	"cntxt_opt : CONTEXT LPAREN cntxt_list RPAREN",
	"cntxt_list : cntxt_list COMMA cntxt",
	"cntxt_list : cntxt",
	"cntxt : TT_IDENTIFIER",
	"infer_opt : /* empty */",
	"infer_opt : INFER start_opt queue_opt opnum_opt",
	"start_opt : /* empty */",
	"start_opt : START",
	"queue_opt : /* empty */",
	"queue_opt : QUEUE",
	"opnum_opt : /* empty */",
	"opnum_opt : OPNUM EQUAL TT_NUMBER",
};
#endif /* YYDEBUG */
/*
 * Copyright (c) 1993 by Sun Microsystems, Inc.
 */

/*
** Skeleton parser driver for yacc output
*/

/*
** yacc user known macros and defines
*/
#define YYERROR		goto yyerrlab
#define YYACCEPT	return(0)
#define YYABORT		return(1)
#define YYBACKUP( newtoken, newvalue )\
{\
	if ( yychar >= 0 || ( yyr2[ yytmp ] >> 1 ) != 1 )\
	{\
		yyerror( "syntax error - cannot backup" );\
		goto yyerrlab;\
	}\
	yychar = newtoken;\
	yystate = *yyps;\
	yylval = newvalue;\
	goto yynewstate;\
}
#define YYRECOVERING()	(!!yyerrflag)
#define YYNEW(type)	malloc(sizeof(type) * yynewmax)
#define YYCOPY(to, from, type) \
	(type *) memcpy(to, (char *) from, yynewmax * sizeof(type))
#define YYENLARGE( from, type) \
	(type *) realloc((char *) from, yynewmax * sizeof(type))
#ifndef YYDEBUG
#	define YYDEBUG	1	/* make debugging available */
#endif

/*
** user known globals
*/
int yydebug;			/* set to 1 to get debugging */

/*
** driver internal defines
*/
#define YYFLAG		(-10000000)

/*
** global variables used by the parser
*/
YYSTYPE *yypv;			/* top of value stack */
int *yyps;			/* top of state stack */

int yystate;			/* current state */
int yytmp;			/* extra var (lasts between blocks) */

int yynerrs;			/* number of errors */
int yyerrflag;			/* error recovery flag */
int yychar;			/* current input token number */



#ifdef YYNMBCHARS
#define YYLEX()		yycvtok(yylex())
/*
** yycvtok - return a token if i is a wchar_t value that exceeds 255.
**	If i<255, i itself is the token.  If i>255 but the neither 
**	of the 30th or 31st bit is on, i is already a token.
*/
#if defined(__STDC__) || defined(__cplusplus)
int yycvtok(int i)
#else
int yycvtok(i) int i;
#endif
{
	int first = 0;
	int last = YYNMBCHARS - 1;
	int mid;
	wchar_t j;

	if(i&0x60000000){/*Must convert to a token. */
		if( yymbchars[last].character < i ){
			return i;/*Giving up*/
		}
		while ((last>=first)&&(first>=0)) {/*Binary search loop*/
			mid = (first+last)/2;
			j = yymbchars[mid].character;
			if( j==i ){/*Found*/ 
				return yymbchars[mid].tvalue;
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
#else/*!YYNMBCHARS*/
#define YYLEX()		yylex()
#endif/*!YYNMBCHARS*/

/*
** yyparse - return 0 if worked, 1 if syntax error not recovered from
*/
#if defined(__STDC__) || defined(__cplusplus)
int _Tt_types_table::yyparse(void)
#else
int _Tt_types_table::yyparse()
#endif
{
	register YYSTYPE *yypvt;	/* top of value stack for $vars */

#if defined(__cplusplus) || defined(lint)
/*
	hacks to please C++ and lint - goto's inside switch should never be
	executed; yypvt is set to 0 to avoid "used before set" warning.
*/
	static int __yaccpar_lint_hack__ = 0;
	switch (__yaccpar_lint_hack__)
	{
		case 1: goto yyerrlab;
		case 2: goto yynewstate;
	}
	yypvt = 0;
#endif

	/*
	** Initialize externals - yyparse may be called more than once
	*/
	yypv = &yyv[-1];
	yyps = &yys[-1];
	yystate = 0;
	yytmp = 0;
	yynerrs = 0;
	yyerrflag = 0;
	yychar = -1;

#if YYMAXDEPTH <= 0
	if (yymaxdepth <= 0)
	{
		if ((yymaxdepth = YYEXPAND(0)) <= 0)
		{
			yyerror("yacc initialization error");
			YYABORT;
		}
	}
#endif

	{
		register YYSTYPE *yy_pv;	/* top of value stack */
		register int *yy_ps;		/* top of state stack */
		register int yy_state;		/* current state */
		register int  yy_n;		/* internal state number info */
	goto yystack;	/* moved from 6 lines above to here to please C++ */

		/*
		** get globals into registers.
		** branch to here only if YYBACKUP was called.
		*/
	yynewstate:
		yy_pv = yypv;
		yy_ps = yyps;
		yy_state = yystate;
		goto yy_newstate;

		/*
		** get globals into registers.
		** either we just started, or we just finished a reduction
		*/
	yystack:
		yy_pv = yypv;
		yy_ps = yyps;
		yy_state = yystate;

		/*
		** top of for (;;) loop while no reductions done
		*/
	yy_stack:
		/*
		** put a state and value onto the stacks
		*/
#if YYDEBUG
		/*
		** if debugging, look up token value in list of value vs.
		** name pairs.  0 and negative (-1) are special values.
		** Note: linear search is used since time is not a real
		** consideration while debugging.
		*/
		if ( yydebug )
		{
			register int yy_i;

			printf( "State %d, token ", yy_state );
			if ( yychar == 0 )
				printf( "end-of-file\n" );
			else if ( yychar < 0 )
				printf( "-none-\n" );
			else
			{
				for ( yy_i = 0; yytoks[yy_i].t_val >= 0;
					yy_i++ )
				{
					if ( yytoks[yy_i].t_val == yychar )
						break;
				}
				printf( "%s\n", yytoks[yy_i].t_name );
			}
		}
#endif /* YYDEBUG */
		if ( ++yy_ps >= &yys[ yymaxdepth ] )	/* room on stack? */
		{
			/*
			** reallocate and recover.  Note that pointers
			** have to be reset, or bad things will happen
			*/
			int yyps_index = (yy_ps - yys);
			int yypv_index = (yy_pv - yyv);
			int yypvt_index = (yypvt - yyv);
			int yynewmax;
#ifdef YYEXPAND
			yynewmax = YYEXPAND(yymaxdepth);
#else
			yynewmax = 2 * yymaxdepth;	/* double table size */
			if (yymaxdepth == YYMAXDEPTH)	/* first time growth */
			{
				char *newyys = (char *)YYNEW(int);
				char *newyyv = (char *)YYNEW(YYSTYPE);
				if (newyys != 0 && newyyv != 0)
				{
					yys = YYCOPY(newyys, yys, int);
					yyv = YYCOPY(newyyv, yyv, YYSTYPE);
				}
				else
					yynewmax = 0;	/* failed */
			}
			else				/* not first time */
			{
				yys = YYENLARGE(yys, int);
				yyv = YYENLARGE(yyv, YYSTYPE);
				if (yys == 0 || yyv == 0)
					yynewmax = 0;	/* failed */
			}
#endif
			if (yynewmax <= yymaxdepth)	/* tables not expanded */
			{
				yyerror( "yacc stack overflow" );
				YYABORT;
			}
			yymaxdepth = yynewmax;

			yy_ps = yys + yyps_index;
			yy_pv = yyv + yypv_index;
			yypvt = yyv + yypvt_index;
		}
		*yy_ps = yy_state;
		*++yy_pv = yyval;

		/*
		** we have a new state - find out what to do
		*/
	yy_newstate:
		if ( ( yy_n = yypact[ yy_state ] ) <= YYFLAG )
			goto yydefault;		/* simple state */
#if YYDEBUG
		/*
		** if debugging, need to mark whether new token grabbed
		*/
		yytmp = yychar < 0;
#endif
		if ( ( yychar < 0 ) && ( ( yychar = YYLEX() ) < 0 ) )
			yychar = 0;		/* reached EOF */
#if YYDEBUG
		if ( yydebug && yytmp )
		{
			register int yy_i;

			printf( "Received token " );
			if ( yychar == 0 )
				printf( "end-of-file\n" );
			else if ( yychar < 0 )
				printf( "-none-\n" );
			else
			{
				for ( yy_i = 0; yytoks[yy_i].t_val >= 0;
					yy_i++ )
				{
					if ( yytoks[yy_i].t_val == yychar )
						break;
				}
				printf( "%s\n", yytoks[yy_i].t_name );
			}
		}
#endif /* YYDEBUG */
		if ( ( ( yy_n += yychar ) < 0 ) || ( yy_n >= YYLAST ) )
			goto yydefault;
		if ( yychk[ yy_n = yyact[ yy_n ] ] == yychar )	/*valid shift*/
		{
			yychar = -1;
			yyval = yylval;
			yy_state = yy_n;
			if ( yyerrflag > 0 )
				yyerrflag--;
			goto yy_stack;
		}

	yydefault:
		if ( ( yy_n = yydef[ yy_state ] ) == -2 )
		{
#if YYDEBUG
			yytmp = yychar < 0;
#endif
			if ( ( yychar < 0 ) && ( ( yychar = YYLEX() ) < 0 ) )
				yychar = 0;		/* reached EOF */
#if YYDEBUG
			if ( yydebug && yytmp )
			{
				register int yy_i;

				printf( "Received token " );
				if ( yychar == 0 )
					printf( "end-of-file\n" );
				else if ( yychar < 0 )
					printf( "-none-\n" );
				else
				{
					for ( yy_i = 0;
						yytoks[yy_i].t_val >= 0;
						yy_i++ )
					{
						if ( yytoks[yy_i].t_val
							== yychar )
						{
							break;
						}
					}
					printf( "%s\n", yytoks[yy_i].t_name );
				}
			}
#endif /* YYDEBUG */
			/*
			** look through exception table
			*/
			{
				register int *yyxi = yyexca;

				while ( ( *yyxi != -1 ) ||
					( yyxi[1] != yy_state ) )
				{
					yyxi += 2;
				}
				while ( ( *(yyxi += 2) >= 0 ) &&
					( *yyxi != yychar ) )
					;
				if ( ( yy_n = yyxi[1] ) < 0 )
					YYACCEPT;
			}
		}

		/*
		** check for syntax error
		*/
		if ( yy_n == 0 )	/* have an error */
		{
			/* no worry about speed here! */
			switch ( yyerrflag )
			{
			case 0:		/* new error */
				yyerror( "syntax error" );
				goto skip_init;
			yyerrlab:
				/*
				** get globals into registers.
				** we have a user generated syntax type error
				*/
				yy_pv = yypv;
				yy_ps = yyps;
				yy_state = yystate;
			skip_init:
				yynerrs++;
				/* FALLTHRU */
			case 1:
			case 2:		/* incompletely recovered error */
					/* try again... */
				yyerrflag = 3;
				/*
				** find state where "error" is a legal
				** shift action
				*/
				while ( yy_ps >= yys )
				{
					yy_n = yypact[ *yy_ps ] + YYERRCODE;
					if ( yy_n >= 0 && yy_n < YYLAST &&
						yychk[yyact[yy_n]] == YYERRCODE)					{
						/*
						** simulate shift of "error"
						*/
						yy_state = yyact[ yy_n ];
						goto yy_stack;
					}
					/*
					** current state has no shift on
					** "error", pop stack
					*/
#if YYDEBUG
#	define _POP_ "Error recovery pops state %d, uncovers state %d\n"
					if ( yydebug )
						printf( _POP_, *yy_ps,
							yy_ps[-1] );
#	undef _POP_
#endif
					yy_ps--;
					yy_pv--;
				}
				/*
				** there is no state on stack with "error" as
				** a valid shift.  give up.
				*/
				YYABORT;
			case 3:		/* no shift yet; eat a token */
#if YYDEBUG
				/*
				** if debugging, look up token in list of
				** pairs.  0 and negative shouldn't occur,
				** but since timing doesn't matter when
				** debugging, it doesn't hurt to leave the
				** tests here.
				*/
				if ( yydebug )
				{
					register int yy_i;

					printf( "Error recovery discards " );
					if ( yychar == 0 )
						printf( "token end-of-file\n" );
					else if ( yychar < 0 )
						printf( "token -none-\n" );
					else
					{
						for ( yy_i = 0;
							yytoks[yy_i].t_val >= 0;
							yy_i++ )
						{
							if ( yytoks[yy_i].t_val
								== yychar )
							{
								break;
							}
						}
						printf( "token %s\n",
							yytoks[yy_i].t_name );
					}
				}
#endif /* YYDEBUG */
				if ( yychar == 0 )	/* reached EOF. quit */
					YYABORT;
				yychar = -1;
				goto yy_newstate;
			}
		}/* end if ( yy_n == 0 ) */
		/*
		** reduction by production yy_n
		** put stack tops, etc. so things right after switch
		*/
#if YYDEBUG
		/*
		** if debugging, print the string that is the user's
		** specification of the reduction which is just about
		** to be done.
		*/
		if ( yydebug )
			printf( "Reduce by (%d) \"%s\"\n",
				yy_n, yyreds[ yy_n ] );
#endif
		yytmp = yy_n;			/* value to switch over */
		yypvt = yy_pv;			/* $vars top of value stack */
		/*
		** Look in goto table for next state
		** Sorry about using yy_state here as temporary
		** register variable, but why not, if it works...
		** If yyr2[ yy_n ] doesn't have the low order bit
		** set, then there is no action to be done for
		** this reduction.  So, no saving & unsaving of
		** registers done.  The only difference between the
		** code just after the if and the body of the if is
		** the goto yy_stack in the body.  This way the test
		** can be made before the choice of what to do is needed.
		*/
		{
			/* length of production doubled with extra bit */
			register int yy_len = yyr2[ yy_n ];

			if ( !( yy_len & 01 ) )
			{
				yy_len >>= 1;
				yyval = ( yy_pv -= yy_len )[1];	/* $$ = $1 */
				yy_state = yypgo[ yy_n = yyr1[ yy_n ] ] +
					*( yy_ps -= yy_len ) + 1;
				if ( yy_state >= YYLAST ||
					yychk[ yy_state =
					yyact[ yy_state ] ] != -yy_n )
				{
					yy_state = yyact[ yypgo[ yy_n ] ];
				}
				goto yy_stack;
			}
			yy_len >>= 1;
			yyval = ( yy_pv -= yy_len )[1];	/* $$ = $1 */
			yy_state = yypgo[ yy_n = yyr1[ yy_n ] ] +
				*( yy_ps -= yy_len ) + 1;
			if ( yy_state >= YYLAST ||
				yychk[ yy_state = yyact[ yy_state ] ] != -yy_n )
			{
				yy_state = yyact[ yypgo[ yy_n ] ];
			}
		}
					/* save until reenter driver code */
		yystate = yy_state;
		yyps = yy_ps;
		yypv = yy_pv;
	}
	/*
	** code supplied by user is placed in this switch
	*/
	switch( yytmp )
	{
		
case 5:
# line 86 "mp_types_gram.y"
{
			  insert_otype(tmp_otype);
			  tmp_otype = new _Tt_otype();
			} break;
case 6:
# line 92 "mp_types_gram.y"
{ tmp_otype->set_otid(tmp_otid); } break;
case 7:
# line 94 "mp_types_gram.y"
{
				tmp_otype->set_ancestors(tmp_otidl);
				tmp_otidl = new _Tt_string_list();
		        } break;
case 8:
# line 101 "mp_types_gram.y"
{
				tmp_otid = yytext;
			} break;
case 11:
# line 111 "mp_types_gram.y"
{
			  tmp_otidl->append(tmp_otid);
		        } break;
case 12:
# line 115 "mp_types_gram.y"
{
			  tmp_otidl->append(tmp_otid);
		        } break;
case 19:
# line 131 "mp_types_gram.y"
{
			  tmp_otype->append_osigs(tmp_sigl);
			  tmp_sigl = new _Tt_signature_list();
		        } break;
case 20:
# line 138 "mp_types_gram.y"
{
			  tmp_otype->append_hsigs(tmp_sigl, TT_HANDLE);
			  tmp_sigl = new _Tt_signature_list();
		        } break;
case 21:
# line 145 "mp_types_gram.y"
{
			  tmp_otype->append_hsigs(tmp_sigl, TT_HANDLE_PUSH);
			  tmp_sigl = new _Tt_signature_list();
		        } break;
case 22:
# line 152 "mp_types_gram.y"
{
			  tmp_otype->append_hsigs(tmp_sigl, TT_HANDLE_ROTATE);
			  tmp_sigl = new _Tt_signature_list();
		        } break;
case 25:
# line 163 "mp_types_gram.y"
{
			  tmp_sig->set_otid(tmp_otype->otid());
			  tmp_sig->set_super_otid(tmp_otid);
			  append_sig(tmp_sig);
			  tmp_sig = new _Tt_signature();
			} break;
case 26:
# line 171 "mp_types_gram.y"
{ tmp_sig->set_ptid(yytext); } break;
case 31:
# line 182 "mp_types_gram.y"
{
			  tmp_sig->set_super_otid(tmp_otid);
			} break;
case 32:
# line 218 "mp_types_gram.y"
{ tmp_ptype->set_ptid(yytext); } break;
case 33:
# line 220 "mp_types_gram.y"
{
			  insert_ptype(tmp_ptype);
			  tmp_ptype = new _Tt_ptype();
			} break;
case 37:
# line 234 "mp_types_gram.y"
{
			  tmp_ptype->appendprop(tmp_propname, tmp_propvalue);
			  tmp_propname = tmp_propvalue = 0;
			} break;
case 44:
# line 251 "mp_types_gram.y"
{ set_tmp_propname(_Tt_string((const unsigned char *)yytext, yyleng)); } break;
case 45:
# line 253 "mp_types_gram.y"
{ set_tmp_propname(_Tt_string((const unsigned char *)yytext, yyleng));} break;
case 46:
# line 255 "mp_types_gram.y"
{ set_tmp_propname(_Tt_string((const unsigned char *)yytext,yyleng)); } break;
case 47:
# line 261 "mp_types_gram.y"
{
			  tmp_propvalue.set((const unsigned char *)yytext+1,
					    yyleng - 2);
                        } break;
case 48:
# line 266 "mp_types_gram.y"
{
			  _Tt_string stringfrag;
			  stringfrag.set((const unsigned char *)yytext+1,
					 yyleng - 2);
			  tmp_propvalue = tmp_propvalue.cat(stringfrag);
		        } break;
case 50:
# line 276 "mp_types_gram.y"
{
			  tmp_propvalue = yytext;
			} break;
case 51:
# line 282 "mp_types_gram.y"
{
			  tmp_ptype->append_osigs(tmp_sigl);
			  tmp_sigl = new _Tt_signature_list();
			} break;
case 52:
# line 289 "mp_types_gram.y"
{
			  tmp_ptype->append_hsigs(tmp_sigl, TT_HANDLE);
			  tmp_sigl = new _Tt_signature_list();
			} break;
case 53:
# line 296 "mp_types_gram.y"
{
			  tmp_ptype->append_hsigs(tmp_sigl, TT_HANDLE_PUSH);
			  tmp_sigl = new _Tt_signature_list();
			} break;
case 54:
# line 303 "mp_types_gram.y"
{
			  tmp_ptype->append_hsigs(tmp_sigl, TT_HANDLE_ROTATE);
			  tmp_sigl = new _Tt_signature_list();
			} break;
case 59:
# line 320 "mp_types_gram.y"
{
				tmp_sig->set_ptid(tmp_ptype->ptid());
				append_sig(tmp_sig);
				tmp_sig = new _Tt_signature();
			} break;
case 61:
# line 328 "mp_types_gram.y"
{
			tmp_sig->set_scope(TT_FILE);
		} break;
case 62:
# line 331 "mp_types_gram.y"
{
			tmp_sig->set_scope(TT_SESSION);
		} break;
case 63:
# line 334 "mp_types_gram.y"
{
			tmp_sig->set_scope(TT_FILE_IN_SESSION);
		} break;
case 64:
# line 340 "mp_types_gram.y"
{
			  tmp_sig->set_op(yytext);
			} break;
case 71:
# line 365 "mp_types_gram.y"
{
			  tmp_sig->append_arg(tmp_arg);
			  tmp_arg = new _Tt_arg();
			} break;
case 72:
# line 371 "mp_types_gram.y"
{ tmp_arg->set_mode(TT_IN); } break;
case 73:
# line 372 "mp_types_gram.y"
{ tmp_arg->set_mode(TT_OUT); } break;
case 74:
# line 373 "mp_types_gram.y"
{ tmp_arg->set_mode(TT_INOUT); } break;
case 75:
# line 377 "mp_types_gram.y"
{
			  _Tt_string tmptext(yytext);
			  tmp_arg->set_type(tmptext);
			} break;
case 76:
# line 384 "mp_types_gram.y"
{
			  _Tt_string tmptext(yytext);
			  tmp_arg->set_name(tmptext);
			} break;
case 81:
# line 399 "mp_types_gram.y"
{
			  _Tt_context_ptr tmpcntxt = new _Tt_context;
			  tmpcntxt->setName(yytext);
			  tmp_sig->append_context(tmpcntxt);
			} break;
case 85:
# line 414 "mp_types_gram.y"
{ tmp_sig->set_reliability(TT_START); } break;
case 87:
# line 418 "mp_types_gram.y"
{ tmp_sig->set_reliability(TT_QUEUE); } break;
case 89:
# line 424 "mp_types_gram.y"
{ tmp_sig->set_opnum(atoi(yytext)); } break;
	}
	goto yystack;		/* reset registers in driver code */
}

