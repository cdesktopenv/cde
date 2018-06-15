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
#include <stdio.h>
# define U(x) x
# define NLSTATE yyprevious=YYNEWLINE
# define BEGIN yybgin = yysvec + 1 +
# define INITIAL 0
# define YYLERR yysvec
# define YYSTATE (yyestate-yysvec-1)
# define YYOPTIM 1
# define YYLMAX BUFSIZ
#ifndef __cplusplus
#else
# define output(c) (void)putc(c,yyout)
#endif

#if defined(__cplusplus) || defined(__STDC__)

#if defined(__cplusplus) && defined(__EXTERN_C__)
extern "C" {
#endif
	int yyback(int *, int);
	int yyinput(void);
	int yylook(void);
	void yyoutput(int);
	int yyracc(int);
	int yyreject(void);
	void yyunput(int);
	int yylex(void);
#ifdef YYLEX_E
	void yywoutput(wchar_t);
	wchar_t yywinput(void);
#endif
#ifndef yyless
	void yyless(int);
#endif
#ifndef yywrap
	int yywrap(void);
#endif
#ifdef LEXDEBUG
	void allprint(char);
	void sprint(char *);
#endif
#if defined(__cplusplus) && defined(__EXTERN_C__)
}
#endif

#endif
# define yymore() (yymorfg=1)
#ifndef __cplusplus
#else
# define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):getc(yyin))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)
#endif
#define ECHO fprintf(yyout, "%s",yytext)
# define REJECT { nstr = yyreject(); goto yyfussy;}
int yyleng; extern char yytext[];
int yymorfg;
extern char *yysptr, yysbuf[];
int yytchar;
FILE *yyin = {stdin}, *yyout = {stdout};
extern int yylineno;
struct yywork; struct yysvf { 
	struct yywork *yystoff;
	struct yysvf *yyother;
	int *yystops;};
struct yysvf *yyestate;
extern struct yysvf yysvec[], *yybgin;

# line 2 "mp_types_lex.l"
/*%% 	$XConsortium: frozen.mp_types_lex.C /main/5 1996/04/05 18:49:12 drk $							 */

# line 3 "mp_types_lex.l"
/*%% 									 */

# line 4 "mp_types_lex.l"
/*%%  	RESTRICTED CONFIDENTIAL INFORMATION:                             */

# line 5 "mp_types_lex.l"
/*%% 									 */

# line 6 "mp_types_lex.l"
/*%% 	The information in this document is subject to special		 */

# line 7 "mp_types_lex.l"
/*%% 	restrictions in a confidential disclosure agreement between	 */

# line 8 "mp_types_lex.l"
/*%% 	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this	 */

# line 9 "mp_types_lex.l"
/*%% 	document outside HP, IBM, Sun, USL, SCO, or Univel without	 */

# line 10 "mp_types_lex.l"
/*%% 	Sun's specific written approval.  This document and all copies	 */

# line 11 "mp_types_lex.l"
/*%% 	and derivative works thereof must be returned or destroyed at	 */

# line 12 "mp_types_lex.l"
/*%% 	Sun's request.							 */

# line 13 "mp_types_lex.l"
/*%% 									 */

# line 14 "mp_types_lex.l"
/*%% 	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.	 */

# line 15 "mp_types_lex.l"
/*%% 									 */
#include <stdlib.h>
#include "mp_types_gram.h"
#include "mp_types_table.h"
extern "C" {
int input(void);
void output(int);
void unput(int);
}
static void skip_c_comments(void);
static void skip_cplusplus_comments(void);
# define YYNEWLINE 10
int _Tt_types_table::yylex(){
int nstr; extern int yyprevious;
#ifdef __cplusplus
/* to avoid CC and lint complaining yyfussy not being used ...*/
static int __lex_hack = 0;
if (__lex_hack) goto yyfussy;
#endif
while((nstr = yylook()) >= 0)
yyfussy: switch(nstr){
case 0:
if(yywrap()) return(0); break;
case 1:

# line 30 "mp_types_lex.l"
			skip_c_comments();
break;
case 2:

# line 31 "mp_types_lex.l"
			skip_cplusplus_comments();
break;
case 3:

# line 32 "mp_types_lex.l"
			{ return OTYPE; }
break;
case 4:

# line 33 "mp_types_lex.l"
			{ return INHERIT; }
break;
case 5:

# line 34 "mp_types_lex.l"
			{ return FROM; }
break;
case 6:

# line 35 "mp_types_lex.l"
			{ return PTYPE; }
break;
case 7:

# line 36 "mp_types_lex.l"
		{ return PER_FILE; }
break;
case 8:

# line 37 "mp_types_lex.l"
		{ return PER_SESSION; }
break;
case 9:

# line 38 "mp_types_lex.l"
			{ return START; }
break;
case 10:

# line 39 "mp_types_lex.l"
			{ return OBSERVE; }
break;
case 11:

# line 40 "mp_types_lex.l"
			{ return HANDLE; }
break;
case 12:

# line 41 "mp_types_lex.l"
		{ return HANDLE_PUSH; }
break;
case 13:

# line 42 "mp_types_lex.l"
		{ return HANDLE_ROTATE; }
break;
case 14:

# line 43 "mp_types_lex.l"
			{ return FILE_SCOPE; }
break;
case 15:

# line 44 "mp_types_lex.l"
			{ return SESSION_SCOPE; }
break;
case 16:

# line 45 "mp_types_lex.l"
                { return FILE_IN_SESSION; }
break;
case 17:

# line 46 "mp_types_lex.l"
			{ return REQUEST; }
break;
case 18:

# line 47 "mp_types_lex.l"
		{ return NOTIFICATION; }
break;
case 19:

# line 48 "mp_types_lex.l"
			{ return VOID_ARGS; }
break;
case 20:

# line 49 "mp_types_lex.l"
			{ return IN; }
break;
case 21:

# line 50 "mp_types_lex.l"
			{ return OUT; }
break;
case 22:

# line 51 "mp_types_lex.l"
			{ return INOUT; }
break;
case 23:

# line 52 "mp_types_lex.l"
			{ return QUEUE; }
break;
case 24:

# line 53 "mp_types_lex.l"
			{ return OPNUM; }
break;
case 25:

# line 54 "mp_types_lex.l"
			{ return CONTEXT; }
break;
case 26:

# line 55 "mp_types_lex.l"
			{ return COLON; }
break;
case 27:

# line 56 "mp_types_lex.l"
			{ return SEMICOLON; }
break;
case 28:

# line 57 "mp_types_lex.l"
			{ return LCURL; }
break;
case 29:

# line 58 "mp_types_lex.l"
			{ return RCURL; }
break;
case 30:

# line 59 "mp_types_lex.l"
			{ return INFER; }
break;
case 31:

# line 60 "mp_types_lex.l"
			{ return LPAREN; }
break;
case 32:

# line 61 "mp_types_lex.l"
			{ return RPAREN; }
break;
case 33:

# line 62 "mp_types_lex.l"
			{ return COMMA; }
break;
case 34:

# line 63 "mp_types_lex.l"
			{ return EQUAL; }
break;
case 35:

# line 65 "mp_types_lex.l"
	{ return TT_IDENTIFIER; }
break;
case 36:

# line 66 "mp_types_lex.l"
			{ return TT_STRING; }
break;
case 37:

# line 67 "mp_types_lex.l"
			{ return TT_NUMBER; }
break;
case 38:

# line 69 "mp_types_lex.l"
;
break;
case -1:
break;
default:
(void)fprintf(yyout,"bad switch yylook %d",nstr);
} return(0); }
/* end of yylex */

					     

int yyvstop[] = {
0,

38,
0,

31,
0,

32,
0,

33,
0,

37,
0,

26,
0,

27,
0,

34,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

28,
0,

29,
0,

36,
0,

1,
0,

2,
0,

30,
0,

35,
0,

35,
0,

35,
0,

35,
0,

20,
35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

21,
35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

14,
35,
0,

5,
35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

19,
35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

22,
35,
0,

35,
0,

35,
0,

24,
35,
0,

3,
35,
0,

35,
0,

35,
0,

6,
35,
0,

23,
35,
0,

35,
0,

35,
0,

9,
35,
0,

35,
0,

35,
0,

11,
35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

25,
35,
0,

35,
0,

35,
0,

4,
35,
0,

35,
0,

10,
35,
0,

35,
0,

35,
0,

17,
35,
0,

15,
35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

7,
35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

35,
0,

12,
35,
0,

35,
0,

35,
0,

8,
35,
0,

35,
0,

35,
0,

18,
35,
0,

35,
0,

13,
35,
0,

35,
0,

16,
35,
0,
0};
# define YYTYPE unsigned char
struct yywork { YYTYPE verify, advance; } yycrank[] = {
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	1,3,	1,3,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	1,3,	0,0,	1,4,	
0,0,	0,0,	0,0,	0,0,	
0,0,	1,5,	1,6,	0,0,	
0,0,	1,7,	0,0,	0,0,	
1,8,	1,9,	1,9,	1,9,	
1,9,	1,9,	1,9,	1,9,	
1,9,	1,9,	1,9,	1,10,	
1,11,	0,0,	1,12,	12,30,	
0,0,	0,0,	1,13,	1,13,	
1,13,	1,13,	1,13,	1,13,	
1,13,	1,13,	1,13,	1,13,	
1,13,	1,13,	1,13,	1,13,	
1,13,	1,13,	1,13,	1,13,	
1,13,	1,13,	1,13,	1,13,	
1,13,	1,13,	1,13,	1,13,	
8,28,	0,0,	0,0,	0,0,	
59,76,	8,29,	1,13,	1,13,	
1,14,	1,13,	1,13,	1,15,	
1,13,	1,16,	1,17,	1,13,	
1,13,	1,13,	1,13,	1,18,	
1,19,	1,20,	1,21,	1,22,	
1,23,	1,13,	1,13,	1,24,	
1,13,	1,13,	1,13,	1,13,	
1,25,	4,4,	1,26,	14,31,	
16,34,	17,35,	18,36,	19,37,	
20,41,	4,4,	9,9,	9,9,	
9,9,	9,9,	9,9,	9,9,	
9,9,	9,9,	9,9,	9,9,	
15,32,	19,38,	21,43,	20,42,	
22,44,	19,39,	19,40,	23,45,	
24,47,	15,33,	31,48,	32,49,	
33,50,	34,51,	4,27,	36,54,	
35,52,	37,55,	38,56,	39,57,	
40,58,	41,59,	23,46,	35,53,	
42,60,	4,4,	13,13,	43,61,	
4,4,	13,13,	13,13,	13,13,	
13,13,	13,13,	13,13,	13,13,	
13,13,	13,13,	13,13,	44,62,	
45,63,	46,64,	47,65,	48,66,	
49,67,	4,4,	13,13,	13,13,	
13,13,	13,13,	13,13,	13,13,	
13,13,	13,13,	13,13,	13,13,	
13,13,	13,13,	13,13,	13,13,	
13,13,	13,13,	13,13,	13,13,	
13,13,	13,13,	13,13,	13,13,	
13,13,	13,13,	13,13,	13,13,	
50,68,	51,69,	52,70,	53,71,	
13,13,	54,72,	13,13,	13,13,	
13,13,	13,13,	13,13,	13,13,	
13,13,	13,13,	13,13,	13,13,	
13,13,	13,13,	13,13,	13,13,	
13,13,	13,13,	13,13,	13,13,	
13,13,	13,13,	13,13,	13,13,	
13,13,	13,13,	13,13,	13,13,	
55,73,	56,74,	57,75,	60,77,	
61,78,	62,79,	63,80,	64,81,	
65,82,	66,83,	67,84,	69,85,	
70,86,	71,87,	72,88,	73,89,	
74,90,	75,91,	76,92,	77,94,	
78,95,	79,96,	80,97,	81,98,	
83,99,	84,100,	85,101,	86,102,	
88,103,	89,104,	92,105,	76,93,	
93,106,	96,107,	97,108,	99,109,	
100,110,	101,111,	102,112,	103,113,	
104,114,	105,115,	106,116,	107,117,	
108,118,	110,119,	111,120,	113,122,	
111,121,	115,123,	116,124,	119,125,	
120,126,	121,127,	122,128,	124,129,	
125,130,	126,131,	127,132,	128,133,	
129,134,	130,135,	131,136,	132,137,	
133,138,	134,139,	135,140,	137,141,	
138,142,	140,143,	141,144,	143,145,	
145,146,	0,0,	0,0,	0,0,	
0,0};
struct yysvf yysvec[] = {
0,	0,	0,
yycrank+1,	0,		0,	
yycrank+0,	yysvec+1,	0,	
yycrank+0,	0,		yyvstop+1,
yycrank+-124,	0,		0,	
yycrank+0,	0,		yyvstop+3,
yycrank+0,	0,		yyvstop+5,
yycrank+0,	0,		yyvstop+7,
yycrank+50,	0,		0,	
yycrank+86,	0,		yyvstop+9,
yycrank+0,	0,		yyvstop+11,
yycrank+0,	0,		yyvstop+13,
yycrank+1,	0,		yyvstop+15,
yycrank+125,	0,		yyvstop+17,
yycrank+16,	yysvec+13,	yyvstop+19,
yycrank+39,	yysvec+13,	yyvstop+21,
yycrank+31,	yysvec+13,	yyvstop+23,
yycrank+19,	yysvec+13,	yyvstop+25,
yycrank+19,	yysvec+13,	yyvstop+27,
yycrank+33,	yysvec+13,	yyvstop+29,
yycrank+31,	yysvec+13,	yyvstop+31,
yycrank+29,	yysvec+13,	yyvstop+33,
yycrank+47,	yysvec+13,	yyvstop+35,
yycrank+50,	yysvec+13,	yyvstop+37,
yycrank+41,	yysvec+13,	yyvstop+39,
yycrank+0,	0,		yyvstop+41,
yycrank+0,	0,		yyvstop+43,
yycrank+0,	0,		yyvstop+45,
yycrank+0,	0,		yyvstop+47,
yycrank+0,	0,		yyvstop+49,
yycrank+0,	0,		yyvstop+51,
yycrank+44,	yysvec+13,	yyvstop+53,
yycrank+47,	yysvec+13,	yyvstop+55,
yycrank+45,	yysvec+13,	yyvstop+57,
yycrank+47,	yysvec+13,	yyvstop+59,
yycrank+56,	yysvec+13,	yyvstop+61,
yycrank+43,	yysvec+13,	yyvstop+64,
yycrank+46,	yysvec+13,	yyvstop+66,
yycrank+52,	yysvec+13,	yyvstop+68,
yycrank+42,	yysvec+13,	yyvstop+70,
yycrank+48,	yysvec+13,	yyvstop+72,
yycrank+51,	yysvec+13,	yyvstop+74,
yycrank+47,	yysvec+13,	yyvstop+76,
yycrank+70,	yysvec+13,	yyvstop+78,
yycrank+70,	yysvec+13,	yyvstop+80,
yycrank+69,	yysvec+13,	yyvstop+82,
yycrank+88,	yysvec+13,	yyvstop+84,
yycrank+81,	yysvec+13,	yyvstop+86,
yycrank+71,	yysvec+13,	yyvstop+88,
yycrank+87,	yysvec+13,	yyvstop+90,
yycrank+107,	yysvec+13,	yyvstop+92,
yycrank+117,	yysvec+13,	yyvstop+94,
yycrank+117,	yysvec+13,	yyvstop+96,
yycrank+102,	yysvec+13,	yyvstop+98,
yycrank+116,	yysvec+13,	yyvstop+100,
yycrank+147,	yysvec+13,	yyvstop+102,
yycrank+132,	yysvec+13,	yyvstop+104,
yycrank+138,	yysvec+13,	yyvstop+106,
yycrank+0,	yysvec+13,	yyvstop+108,
yycrank+1,	yysvec+13,	yyvstop+111,
yycrank+139,	yysvec+13,	yyvstop+113,
yycrank+135,	yysvec+13,	yyvstop+115,
yycrank+136,	yysvec+13,	yyvstop+117,
yycrank+139,	yysvec+13,	yyvstop+119,
yycrank+141,	yysvec+13,	yyvstop+121,
yycrank+156,	yysvec+13,	yyvstop+123,
yycrank+156,	yysvec+13,	yyvstop+125,
yycrank+163,	yysvec+13,	yyvstop+127,
yycrank+0,	yysvec+13,	yyvstop+130,
yycrank+151,	yysvec+13,	yyvstop+133,
yycrank+146,	yysvec+13,	yyvstop+135,
yycrank+145,	yysvec+13,	yyvstop+137,
yycrank+160,	yysvec+13,	yyvstop+139,
yycrank+149,	yysvec+13,	yyvstop+141,
yycrank+155,	yysvec+13,	yyvstop+143,
yycrank+164,	yysvec+13,	yyvstop+145,
yycrank+164,	yysvec+13,	yyvstop+147,
yycrank+166,	yysvec+13,	yyvstop+149,
yycrank+167,	yysvec+13,	yyvstop+151,
yycrank+168,	yysvec+13,	yyvstop+153,
yycrank+165,	yysvec+13,	yyvstop+155,
yycrank+155,	yysvec+13,	yyvstop+157,
yycrank+0,	yysvec+13,	yyvstop+159,
yycrank+152,	yysvec+13,	yyvstop+162,
yycrank+168,	yysvec+13,	yyvstop+164,
yycrank+173,	yysvec+13,	yyvstop+166,
yycrank+170,	yysvec+13,	yyvstop+168,
yycrank+0,	yysvec+13,	yyvstop+170,
yycrank+171,	yysvec+13,	yyvstop+173,
yycrank+159,	yysvec+13,	yyvstop+175,
yycrank+0,	yysvec+13,	yyvstop+177,
yycrank+0,	yysvec+13,	yyvstop+180,
yycrank+173,	yysvec+13,	yyvstop+183,
yycrank+179,	yysvec+13,	yyvstop+185,
yycrank+0,	yysvec+13,	yyvstop+187,
yycrank+0,	yysvec+13,	yyvstop+190,
yycrank+166,	yysvec+13,	yyvstop+193,
yycrank+171,	yysvec+13,	yyvstop+195,
yycrank+0,	yysvec+13,	yyvstop+197,
yycrank+167,	yysvec+13,	yyvstop+200,
yycrank+174,	yysvec+13,	yyvstop+202,
yycrank+190,	yysvec+13,	yyvstop+204,
yycrank+170,	yysvec+13,	yyvstop+207,
yycrank+188,	yysvec+13,	yyvstop+209,
yycrank+187,	yysvec+13,	yyvstop+211,
yycrank+181,	yysvec+13,	yyvstop+213,
yycrank+175,	yysvec+13,	yyvstop+215,
yycrank+175,	yysvec+13,	yyvstop+217,
yycrank+182,	yysvec+13,	yyvstop+219,
yycrank+0,	yysvec+13,	yyvstop+221,
yycrank+198,	yysvec+13,	yyvstop+224,
yycrank+182,	yysvec+13,	yyvstop+226,
yycrank+0,	yysvec+13,	yyvstop+228,
yycrank+198,	yysvec+13,	yyvstop+231,
yycrank+0,	yysvec+13,	yyvstop+233,
yycrank+196,	yysvec+13,	yyvstop+236,
yycrank+183,	yysvec+13,	yyvstop+238,
yycrank+0,	yysvec+13,	yyvstop+240,
yycrank+0,	yysvec+13,	yyvstop+243,
yycrank+184,	yysvec+13,	yyvstop+246,
yycrank+183,	yysvec+13,	yyvstop+248,
yycrank+190,	yysvec+13,	yyvstop+250,
yycrank+186,	yysvec+13,	yyvstop+252,
yycrank+0,	yysvec+13,	yyvstop+254,
yycrank+198,	yysvec+13,	yyvstop+257,
yycrank+203,	yysvec+13,	yyvstop+259,
yycrank+190,	yysvec+13,	yyvstop+261,
yycrank+190,	yysvec+13,	yyvstop+263,
yycrank+202,	yysvec+13,	yyvstop+265,
yycrank+197,	yysvec+13,	yyvstop+267,
yycrank+194,	yysvec+13,	yyvstop+269,
yycrank+206,	yysvec+13,	yyvstop+271,
yycrank+214,	yysvec+13,	yyvstop+273,
yycrank+201,	yysvec+13,	yyvstop+275,
yycrank+203,	yysvec+13,	yyvstop+277,
yycrank+199,	yysvec+13,	yyvstop+279,
yycrank+0,	yysvec+13,	yyvstop+281,
yycrank+199,	yysvec+13,	yyvstop+284,
yycrank+206,	yysvec+13,	yyvstop+286,
yycrank+0,	yysvec+13,	yyvstop+288,
yycrank+212,	yysvec+13,	yyvstop+291,
yycrank+217,	yysvec+13,	yyvstop+293,
yycrank+0,	yysvec+13,	yyvstop+295,
yycrank+208,	yysvec+13,	yyvstop+298,
yycrank+0,	yysvec+13,	yyvstop+300,
yycrank+210,	yysvec+13,	yyvstop+303,
yycrank+0,	yysvec+13,	yyvstop+305,
0,	0,	0};
struct yywork *yytop = yycrank+320;
struct yysvf *yybgin = yysvec+1;
char yymatch[] = {
  0,   1,   1,   1,   1,   1,   1,   1, 
  1,   9,   9,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  9,   1,  34,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,  45,   1,   1, 
 48,  48,  48,  48,  48,  48,  48,  48, 
 48,  48,   1,   1,   1,   1,   1,   1, 
  1,  65,  65,  65,  65,  65,  65,  65, 
 65,  65,  65,  65,  65,  65,  65,  65, 
 65,  65,  65,  65,  65,  65,  65,  65, 
 65,  65,  65,   1,   1,   1,   1,  45, 
  1,  65,  65,  65,  65,  65,  65,  65, 
 65,  65,  65,  65,  65,  65,  65,  65, 
 65,  65,  65,  65,  65,  65,  65,  65, 
 65,  65,  65,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
0};
char yyextra[] = {
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0};
/*	Copyright (c) 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

int yylineno =1;
# define YYU(x) x
# define NLSTATE yyprevious=YYNEWLINE
char yytext[YYLMAX];
struct yysvf *yylstate [YYLMAX], **yylsp, **yyolsp;
char yysbuf[YYLMAX];
char *yysptr = yysbuf;
int *yyfnd;
extern struct yysvf *yyestate;
int yyprevious = YYNEWLINE;
#if defined(__cplusplus) || defined(__STDC__)
int _Tt_types_table::yylook(void)
#else
int _Tt_types_table::yylook()
#endif
{
	struct yysvf *yystate, **lsp;
	struct yywork *yyt;
	struct yysvf *yyz;
	int yych, yyfirst;
	struct yywork *yyr;
# ifdef LEXDEBUG
	int debug;
# endif
	char *yylastch;
	/* start off machines */
# ifdef LEXDEBUG
	debug = 0;
# endif
	yyfirst=1;
	if (!yymorfg)
		yylastch = yytext;
	else {
		yymorfg=0;
		yylastch = yytext+yyleng;
		}
	for(;;){
		lsp = yylstate;
		yyestate = yystate = yybgin;
		if (yyprevious==YYNEWLINE) yystate++;
		for (;;){
# ifdef LEXDEBUG
			if(debug)fprintf(yyout,"state %d\n",yystate-yysvec-1);
# endif
			yyt = yystate->yystoff;
			if(yyt == yycrank && !yyfirst){  /* may not be any transitions */
				yyz = yystate->yyother;
				if(yyz == 0)break;
				if(yyz->yystoff == yycrank)break;
				}
#ifndef __cplusplus
			*yylastch++ = yych = input();
#else
			*yylastch++ = yych = input();
#endif
			if(yylastch > &yytext[YYLMAX]) {
				fprintf(yyout,"Input string too long, limit %d\n",YYLMAX);
				exit(1);
			}
			yyfirst=0;
		tryagain:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"char ");
				allprint(yych);
				putchar('\n');
				}
# endif
			yyr = yyt;
			if ( (long)yyt > (long)yycrank){
				yyt = yyr + yych;
				if (yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					if(lsp > &yylstate[YYLMAX]) {
						fprintf(yyout,"Input string too long, limit %d\n",YYLMAX);
						exit(1);
					}
					goto contin;
					}
				}
# ifdef YYOPTIM
			else if((long)yyt < (long)yycrank) {		/* r < yycrank */
				yyt = yyr = yycrank+(yycrank-yyt);
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"compressed state\n");
# endif
				yyt = yyt + yych;
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					if(lsp > &yylstate[YYLMAX]) {
						fprintf(yyout,"Input string too long, limit %d\n",YYLMAX);
						exit(1);
					}
					goto contin;
					}
				yyt = yyr + YYU(yymatch[yych]);
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"try fall back character ");
					allprint(YYU(yymatch[yych]));
					putchar('\n');
					}
# endif
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transition */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					if(lsp > &yylstate[YYLMAX]) {
						fprintf(yyout,"Input string too long, limit %d\n",YYLMAX);
						exit(1);
					}
					goto contin;
					}
				}
			if ((yystate = yystate->yyother) && (yyt= yystate->yystoff) != yycrank){
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"fall back to state %d\n",yystate-yysvec-1);
# endif
				goto tryagain;
				}
# endif
			else
				{unput(*--yylastch);break;}
		contin:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"state %d char ",yystate-yysvec-1);
				allprint(yych);
				putchar('\n');
				}
# endif
			;
			}
# ifdef LEXDEBUG
		if(debug){
			fprintf(yyout,"stopped at %d with ",*(lsp-1)-yysvec-1);
			allprint(yych);
			putchar('\n');
			}
# endif
		while (lsp-- > yylstate){
			*yylastch-- = 0;
			if (*lsp != 0 && (yyfnd= (*lsp)->yystops) && *yyfnd > 0){
				yyolsp = lsp;
				if(yyextra[*yyfnd]){		/* must backup */
					while(yyback((*lsp)->yystops,-*yyfnd) != 1 && lsp > yylstate){
						lsp--;
						unput(*yylastch--);
						}
					}
				yyprevious = YYU(*yylastch);
				yylsp = lsp;
				yyleng = yylastch-yytext+1;
				yytext[yyleng] = 0;
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"\nmatch ");
					sprint(yytext);
					fprintf(yyout," action %d\n",*yyfnd);
					}
# endif
				return(*yyfnd++);
				}
			unput(*yylastch);
			}
		if (yytext[0] == 0  /* && feof(yyin) */)
			{
			yysptr=yysbuf;
			return(0);
			}
#ifndef __cplusplus
		yyprevious = yytext[0] = input();
		if (yyprevious>0)
			output(yyprevious);
#else
		yyprevious = yytext[0] = input();
		if (yyprevious>0)
			output(yyprevious);
#endif
		yylastch=yytext;
# ifdef LEXDEBUG
		if(debug)putchar('\n');
# endif
		}
	}
#if defined(__cplusplus) || defined(__STDC__)
int _Tt_types_table::yyback(int *p, int m)
#else
int _Tt_types_table::yyback(int *p, int m)
#endif
{
	if (p==0) return(0);
	while (*p) {
		if (*p++ == m)
			return(1);
	}
	return(0);
}
	/* the following are only used in the lex library */
