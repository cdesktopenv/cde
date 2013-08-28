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
/* $TOG: OL_DataExpr.C /main/4 1997/12/23 11:38:27 bill $ */
#ifndef lint
static const char ol_datasccsid[] = "@(#)yaccpar	1.8 (Berkeley) 01/20/90";
#endif
#define ol_dataBYACC 1
#line 2 "OL_DataExpr.y"
#include <stdio.h>
#include "ExprList.h"
#include "Expression.h"
#include "ContentType.h"

extern int ol_datalex();
extern void ol_dataerror( char *str );

extern ContentType *CurrentContentPtr;

#line 14 "OL_DataExpr.y"
typedef union {
  int        name;
  char       *string;
  OL_Expression *eptr;
} ol_dataSTYPE;
#line 23 "y.tab.c"
#define Reference 257
#define Id 258
#define Literal 259
#define Content 260
#define Concat 261
#define Attr 262
#define FirstOf 263
#define ol_dataERRCODE 256
short ol_datalhs[] = {                                        -1,
    0,    2,    2,    1,    1,    1,    1,    1,    1,    1,
};
short ol_datalen[] = {                                         2,
    1,    1,    3,    1,    1,    4,    4,    4,    1,    1,
};
short ol_datadefred[] = {                                      0,
   10,    4,    9,    5,    0,    0,    0,    0,    0,    1,
    0,    0,    0,    0,    0,    0,    0,    3,    6,    7,
    8,
};
short ol_datadgoto[] = {                                       8,
    9,   10,
};
short ol_datasindex[] = {                                   -255,
    0,    0,    0,    0,  -40,  -30,  -27,    0,  -29,    0,
 -255, -244, -255, -255,  -25,  -24,  -23,    0,    0,    0,
    0,
};
short ol_datarindex[] = {                                      0,
    0,    0,    0,    0,    0,    0,    0,    0,    1,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,
};
short ol_datagindex[] = {                                      0,
    0,   -2,
};
#define ol_dataTABLESIZE 42
short ol_datatable[] = {                                      11,
    2,    1,    2,    3,    4,    5,    6,    7,   15,   12,
   17,   18,   13,   16,   14,   19,   20,   21,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    2,
};
short ol_datacheck[] = {                                      40,
    0,  257,  258,  259,  260,  261,  262,  263,   11,   40,
   13,   14,   40,  258,   44,   41,   41,   41,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   41,
};
#define ol_dataFINAL 8
#ifndef ol_dataDEBUG
#define ol_dataDEBUG 0
#endif
#define ol_dataMAXTOKEN 263
#if ol_dataDEBUG
char *ol_dataname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,"'('","')'",0,0,"','",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"Reference","Id",
"Literal","Content","Concat","Attr","FirstOf",
};
char *ol_datarule[] = {
"$accept : ValList",
"ValList : ExprList",
"ExprList : Expr",
"ExprList : Expr ',' ExprList",
"Expr : Id",
"Expr : Content",
"Expr : Concat '(' ExprList ')'",
"Expr : Attr '(' Id ')'",
"Expr : FirstOf '(' ExprList ')'",
"Expr : Literal",
"Expr : Reference",
};
#endif
#define ol_dataclearin (ol_datachar=(-1))
#define ol_dataerrok (ol_dataerrflag=0)
#ifdef ol_dataSTACKSIZE
#ifndef ol_dataMAXDEPTH
#define ol_dataMAXDEPTH ol_dataSTACKSIZE
#endif
#else
#ifdef ol_dataMAXDEPTH
#define ol_dataSTACKSIZE ol_dataMAXDEPTH
#else
#define ol_dataSTACKSIZE 500
#define ol_dataMAXDEPTH 500
#endif
#endif
int ol_datadebug;
int ol_datanerrs;
int ol_dataerrflag;
int ol_datachar;
short *ol_datassp;
ol_dataSTYPE *ol_datavsp;
ol_dataSTYPE ol_dataval;
ol_dataSTYPE ol_datalval;
short ol_datass[ol_dataSTACKSIZE];
ol_dataSTYPE ol_datavs[ol_dataSTACKSIZE];
#define ol_datastacksize ol_dataSTACKSIZE
#line 97 "OL_DataExpr.y"
	


  


#line 136 "y.tab.c"
#define ol_dataABORT goto ol_dataabort
#define ol_dataACCEPT goto ol_dataaccept
#define ol_dataERROR goto ol_dataerrlab
#if ol_dataDEBUG
#ifndef __cplusplus
extern char *getenv(const char *);
#endif
#endif
int
ol_dataparse()
{
    register int ol_datam, ol_datan, ol_datastate;
#if ol_dataDEBUG
    register char *ol_datas;

    if ((ol_datas = getenv("ol_dataDEBUG")))
    {
        ol_datan = *ol_datas;
        if (ol_datan >= '0' && ol_datan <= '9')
            ol_datadebug = ol_datan - '0';
    }
#endif

    ol_datanerrs = 0;
    ol_dataerrflag = 0;
    ol_datachar = (-1);

    ol_datassp = ol_datass;
    ol_datavsp = ol_datavs;
    *ol_datassp = ol_datastate = 0;

ol_dataloop:
    if ((ol_datan = ol_datadefred[ol_datastate])) goto ol_datareduce;
    if (ol_datachar < 0)
    {
        if ((ol_datachar = ol_datalex()) < 0) ol_datachar = 0;
#if ol_dataDEBUG
        if (ol_datadebug)
        {
            ol_datas = 0;
            if (ol_datachar <= ol_dataMAXTOKEN) ol_datas = ol_dataname[ol_datachar];
            if (!ol_datas) ol_datas = "illegal-symbol";
            printf("ol_datadebug: state %d, reading %d (%s)\n", ol_datastate,
                    ol_datachar, ol_datas);
        }
#endif
    }
    if ((ol_datan = ol_datasindex[ol_datastate]) && (ol_datan += ol_datachar) >= 0 &&
            ol_datan <= ol_dataTABLESIZE && ol_datacheck[ol_datan] == ol_datachar)
    {
#if ol_dataDEBUG
        if (ol_datadebug)
            printf("ol_datadebug: state %d, shifting to state %d\n",
                    ol_datastate, ol_datatable[ol_datan]);
#endif
        if (ol_datassp >= ol_datass + ol_datastacksize - 1)
        {
            goto ol_dataoverflow;
        }
        *++ol_datassp = ol_datastate = ol_datatable[ol_datan];
        *++ol_datavsp = ol_datalval;
        ol_datachar = (-1);
        if (ol_dataerrflag > 0)  --ol_dataerrflag;
        goto ol_dataloop;
    }
    if ((ol_datan = ol_datarindex[ol_datastate]) && (ol_datan += ol_datachar) >= 0 &&
            ol_datan <= ol_dataTABLESIZE && ol_datacheck[ol_datan] == ol_datachar)
    {
        ol_datan = ol_datatable[ol_datan];
        goto ol_datareduce;
    }
    if (ol_dataerrflag) goto ol_datainrecovery;
// Disable for now
#if 0
#ifdef lint
    goto ol_datanewerror;
#endif
ol_datanewerror:
#endif /* 0 */
    ol_dataerror((char*)"syntax error");
// Disable for now
#if 0
#ifdef lint
    goto ol_dataerrlab;
#endif
ol_dataerrlab:
#endif /* 0 */
    ++ol_datanerrs;
ol_datainrecovery:
    if (ol_dataerrflag < 3)
    {
        ol_dataerrflag = 3;
        for (;;)
        {
            if ((ol_datan = ol_datasindex[*ol_datassp]) && (ol_datan += ol_dataERRCODE) >= 0 &&
                    ol_datan <= ol_dataTABLESIZE && ol_datacheck[ol_datan] == ol_dataERRCODE)
            {
#if ol_dataDEBUG
                if (ol_datadebug)
                    printf("ol_datadebug: state %d, error recovery shifting\
 to state %d\n", *ol_datassp, ol_datatable[ol_datan]);
#endif
                if (ol_datassp >= ol_datass + ol_datastacksize - 1)
                {
                    goto ol_dataoverflow;
                }
                *++ol_datassp = ol_datastate = ol_datatable[ol_datan];
                *++ol_datavsp = ol_datalval;
                goto ol_dataloop;
            }
            else
            {
#if ol_dataDEBUG
                if (ol_datadebug)
                    printf("ol_datadebug: error recovery discarding state %d\n",
                            *ol_datassp);
#endif
                if (ol_datassp <= ol_datass) goto ol_dataabort;
                --ol_datassp;
                --ol_datavsp;
            }
        }
    }
    else
    {
        if (ol_datachar == 0) goto ol_dataabort;
#if ol_dataDEBUG
        if (ol_datadebug)
        {
            ol_datas = 0;
            if (ol_datachar <= ol_dataMAXTOKEN) ol_datas = ol_dataname[ol_datachar];
            if (!ol_datas) ol_datas = "illegal-symbol";
            printf("ol_datadebug: state %d, error recovery discards token %d (%s)\n",
                    ol_datastate, ol_datachar, ol_datas);
        }
#endif
        ol_datachar = (-1);
        goto ol_dataloop;
    }
ol_datareduce:
#if ol_dataDEBUG
    if (ol_datadebug)
        printf("ol_datadebug: state %d, reducing by rule %d (%s)\n",
                ol_datastate, ol_datan, ol_datarule[ol_datan]);
#endif
    ol_datam = ol_datalen[ol_datan];
    ol_dataval = ol_datavsp[1-ol_datam];
    switch (ol_datan)
    {
case 1:
#line 36 "OL_DataExpr.y"
{
		CurrentContentPtr->init(ol_datavsp[0].eptr);
	      }
break;
case 2:
#line 42 "OL_DataExpr.y"
{
		ol_dataval.eptr = ol_datavsp[0].eptr;
	      }
break;
case 3:
#line 46 "OL_DataExpr.y"
{
		ol_datavsp[-2].eptr->next = ol_datavsp[0].eptr;
		ol_dataval.eptr = ol_datavsp[-2].eptr;
	      }
break;
case 4:
#line 53 "OL_DataExpr.y"
{
	   OL_Expression *expr = new OL_Expression( GENERIC_ID, ol_datavsp[0].name);
	   ol_dataval.eptr = expr;
	 }
break;
case 5:
#line 59 "OL_DataExpr.y"
{
	   OL_Expression *expr = new OL_Expression( CONTENT );
	   ol_dataval.eptr = expr;
	 }
break;
case 6:
#line 65 "OL_DataExpr.y"
{
	   ExprList   *elist = new ExprList( ol_datavsp[-1].eptr );
	   OL_Expression *expr = new OL_Expression( CONCAT, -1, elist);
	   ol_dataval.eptr = expr;
	 }
break;
case 7:
#line 71 "OL_DataExpr.y"
{
	   OL_Expression *expr = new OL_Expression( REFERENCE, ol_datavsp[-1].name);
	   ol_dataval.eptr = expr;
	 }
break;
case 8:
#line 76 "OL_DataExpr.y"
{
	   ExprList *elist = new ExprList ( ol_datavsp[-1].eptr );
	   OL_Expression *expr = new OL_Expression( FIRSTOF, -1, elist );
	   ol_dataval.eptr = expr;
	 }
break;
case 9:
#line 83 "OL_DataExpr.y"
{
	   OL_Expression *expr = new OL_Expression( LITERAL, -1, ol_datavsp[0].string );
	   ol_dataval.eptr = expr;
	 }
break;
case 10:
#line 89 "OL_DataExpr.y"
{
	   OL_Expression *expr = new OL_Expression( REFERENCE, ol_datavsp[0].name );
	   ol_dataval.eptr = expr;
	 }
break;
#line 350 "y.tab.c"
    }
    ol_datassp -= ol_datam;
    ol_datastate = *ol_datassp;
    ol_datavsp -= ol_datam;
    ol_datam = ol_datalhs[ol_datan];
    if (ol_datastate == 0 && ol_datam == 0)
    {
#if ol_dataDEBUG
        if (ol_datadebug)
            printf("ol_datadebug: after reduction, shifting from state 0 to\
 state %d\n", ol_dataFINAL);
#endif
        ol_datastate = ol_dataFINAL;
        *++ol_datassp = ol_dataFINAL;
        *++ol_datavsp = ol_dataval;
        if (ol_datachar < 0)
        {
            if ((ol_datachar = ol_datalex()) < 0) ol_datachar = 0;
#if ol_dataDEBUG
            if (ol_datadebug)
            {
                ol_datas = 0;
                if (ol_datachar <= ol_dataMAXTOKEN) ol_datas = ol_dataname[ol_datachar];
                if (!ol_datas) ol_datas = "illegal-symbol";
                printf("ol_datadebug: state %d, reading %d (%s)\n",
                        ol_dataFINAL, ol_datachar, ol_datas);
            }
#endif
        }
        if (ol_datachar == 0) goto ol_dataaccept;
        goto ol_dataloop;
    }
    if ((ol_datan = ol_datagindex[ol_datam]) && (ol_datan += ol_datastate) >= 0 &&
            ol_datan <= ol_dataTABLESIZE && ol_datacheck[ol_datan] == ol_datastate)
        ol_datastate = ol_datatable[ol_datan];
    else
        ol_datastate = ol_datadgoto[ol_datam];
#if ol_dataDEBUG
    if (ol_datadebug)
        printf("ol_datadebug: after reduction, shifting from state %d \
to state %d\n", *ol_datassp, ol_datastate);
#endif
    if (ol_datassp >= ol_datass + ol_datastacksize - 1)
    {
        goto ol_dataoverflow;
    }
    *++ol_datassp = ol_datastate;
    *++ol_datavsp = ol_dataval;
    goto ol_dataloop;
ol_dataoverflow:
    ol_dataerror((char*)"yacc stack overflow");
ol_dataabort:
    return (1);
ol_dataaccept:
    return (0);
}
