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
// $TOG: defParser.C /main/5 1997/12/23 11:16:25 bill $
#ifndef lint
static char defParsersccsid[] = "@(#)yaccpar	1.8 (Berkeley) 01/20/90";
#endif
#define defParserBYACC 1
#include <stdio.h>
#include <ctype.h>
#include <sstream>
#include <iostream>
using namespace std;
#include "Debug.h"
#include "FeatureDefDictionary.h"

#ifdef alloca
# undef alloca
#endif
#define alloca(x)       (malloc(x))

def_list_t* g_def_list = 0;

extern void defParsererror(char*);
extern int defParserlex();

#undef defParserwrap

typedef union
{
 unsigned char 	charData;
 unsigned char* charPtrData;
 int   		intData;
 float		realData;
 FeatureValue*	valueData;

 defv_t*    	valueListPtrData;
 TypeValues*   	typeValuesPtrData;
 type_values_list_t*   	typeValuesListPtrData;

 def_list_t*	defListPtrData;
 FeatureDef*	defPtrData;

} defParserSTYPE;
#define STAR 257
#define COMMA 258
#define COLON 259
#define SEMI_COLON 260
#define FSOPEN 261
#define FSCLOSE 262
#define OPER_parenopen 263
#define OPER_parenclose 264
#define INTEGER 265
#define REAL 266
#define NORMAL_STRING 267
#define QUOTED_STRING 268
#define REF_NAME 269
#define TYPE 270
#define ValueListOpt 271
#define defParserERRCODE 256
short defParserlhs[] = {                                        -1,
    0,   11,   11,    6,    6,    6,    6,    8,   10,    7,
    9,    1,    5,    5,    4,    4,    3,    3,    2,    2,
    2,
};
short defParserlen[] = {                                         2,
    1,    2,    1,    1,    1,    1,    1,    4,    4,    2,
    2,    1,    3,    1,    4,    1,    3,    1,    1,    1,
    1,
};
short defParserdefred[] = {                                      0,
    0,   12,    0,    0,    3,    6,    4,    7,    5,    0,
   11,    0,   10,    0,    2,    0,   14,    0,    0,    0,
    0,    9,    8,   19,   21,   20,   18,    0,   13,    0,
   15,   17,
};
short defParserdgoto[] = {                                       3,
    4,   27,   28,   17,   18,    5,    6,    7,    8,    9,
   10,
};
short defParsersindex[] = {                                   -256,
 -258,    0,    0, -240,    0,    0,    0,    0,    0, -256,
    0, -262,    0, -256,    0, -249,    0, -254, -257, -250,
 -262,    0,    0,    0,    0,    0,    0, -251,    0, -250,
    0,    0,
};
short defParserrindex[] = {                                      0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    9,
    0,    0,    0,    0,    0, -236,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,
};
short defParsergindex[] = {                                      0,
    0,  -13,    0,    2,    0,   -7,    0,    0,    0,    0,
   11,
};
#define defParserTABLESIZE 25
short defParsertable[] = {                                       1,
    1,   11,   15,   21,   23,   22,   30,   16,    1,    2,
    2,   15,   31,   20,   24,   25,   32,   26,   12,   13,
   14,   16,   29,   16,   19,
};
short defParsercheck[] = {                                     257,
  257,  260,   10,  258,  262,  260,  258,  270,    0,  267,
  267,   19,  264,  263,  265,  266,   30,  268,  259,  260,
  261,  258,   21,  260,   14,
};
#define defParserFINAL 3
#ifndef defParserDEBUG
#define defParserDEBUG 0
#endif
#define defParserMAXTOKEN 271
#if defParserDEBUG
char *defParsername[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"STAR","COMMA","COLON",
"SEMI_COLON","FSOPEN","FSCLOSE","OPER_parenopen","OPER_parenclose","INTEGER",
"REAL","NORMAL_STRING","QUOTED_STRING","REF_NAME","TYPE","ValueListOpt",
};
char *defParserrule[] = {
"$accept : featureDef",
"featureDef : DefList",
"DefList : DefList Def",
"DefList : Def",
"Def : CompositeDef",
"Def : PrimitiveDef",
"Def : DefReference",
"Def : WildCardDef",
"CompositeDef : Name FSOPEN DefList FSCLOSE",
"PrimitiveDef : Name COLON TypeValuesList SEMI_COLON",
"DefReference : Name SEMI_COLON",
"WildCardDef : STAR SEMI_COLON",
"Name : NORMAL_STRING",
"TypeValuesList : TypeValuesList COMMA TypeValues",
"TypeValuesList : TypeValues",
"TypeValues : TYPE OPER_parenopen ValueList OPER_parenclose",
"TypeValues : TYPE",
"ValueList : ValueList COMMA Value",
"ValueList : Value",
"Value : INTEGER",
"Value : QUOTED_STRING",
"Value : REAL",
};
#endif
#define defParserclearin (defParserchar=(-1))
#define defParsererrok (defParsererrflag=0)
#ifdef defParserSTACKSIZE
#ifndef defParserMAXDEPTH
#define defParserMAXDEPTH defParserSTACKSIZE
#endif
#else
#ifdef defParserMAXDEPTH
#define defParserSTACKSIZE defParserMAXDEPTH
#else
#define defParserSTACKSIZE 500
#define defParserMAXDEPTH 500
#endif
#endif
int defParserdebug;
int defParsernerrs;
int defParsererrflag;
int defParserchar;
short *defParserssp;
defParserSTYPE *defParservsp;
defParserSTYPE defParserval;
defParserSTYPE defParserlval;
short defParserss[defParserSTACKSIZE];
defParserSTYPE defParservs[defParserSTACKSIZE];
#define defParserstacksize defParserSTACKSIZE

/*
void defParsererror(char *errorstr)
{
  cerr << errorstr ;
}
*/
#define defParserABORT goto defParserabort
#define defParserACCEPT goto defParseraccept
#define defParserERROR goto defParsererrlab
#if defParserDEBUG
#ifndef __cplusplus
extern char *getenv(const char *);
#endif
#endif
int
defParserparse()
{
    register int defParserm, defParsern, defParserstate;
#if defParserDEBUG
    register char *defParsers;

    if (defParsers = getenv("defParserDEBUG"))
    {
        defParsern = *defParsers;
        if (defParsern >= '0' && defParsern <= '9')
            defParserdebug = defParsern - '0';
    }
#endif

    defParsernerrs = 0;
    defParsererrflag = 0;
    defParserchar = (-1);

    defParserssp = defParserss;
    defParservsp = defParservs;
    *defParserssp = defParserstate = 0;

defParserloop:
    if (defParsern = defParserdefred[defParserstate]) goto defParserreduce;
    if (defParserchar < 0)
    {
        if ((defParserchar = defParserlex()) < 0) defParserchar = 0;
#if defParserDEBUG
        if (defParserdebug)
        {
            defParsers = 0;
            if (defParserchar <= defParserMAXTOKEN) defParsers = defParsername[defParserchar];
            if (!defParsers) defParsers = "illegal-symbol";
            printf("defParserdebug: state %d, reading %d (%s)\n", defParserstate,
                    defParserchar, defParsers);
        }
#endif
    }
    if ((defParsern = defParsersindex[defParserstate]) && (defParsern += defParserchar) >= 0 &&
            defParsern <= defParserTABLESIZE && defParsercheck[defParsern] == defParserchar)
    {
#if defParserDEBUG
        if (defParserdebug)
            printf("defParserdebug: state %d, shifting to state %d\n",
                    defParserstate, defParsertable[defParsern]);
#endif
        if (defParserssp >= defParserss + defParserstacksize - 1)
        {
            goto defParseroverflow;
        }
        *++defParserssp = defParserstate = defParsertable[defParsern];
        *++defParservsp = defParserlval;
        defParserchar = (-1);
        if (defParsererrflag > 0)  --defParsererrflag;
        goto defParserloop;
    }
    if ((defParsern = defParserrindex[defParserstate]) && (defParsern += defParserchar) >= 0 &&
            defParsern <= defParserTABLESIZE && defParsercheck[defParsern] == defParserchar)
    {
        defParsern = defParsertable[defParsern];
        goto defParserreduce;
    }
    if (defParsererrflag) goto defParserinrecovery;
// Disable for now
#if 0
#ifdef lint
    goto defParsernewerror;
#endif
defParsernewerror:
#endif /* 0 */
    defParsererror((char*)"syntax error");
// Disable for now
#if 0
#ifdef lint
    goto defParsererrlab;
#endif
defParsererrlab:
#endif /* 0 */
    ++defParsernerrs;
defParserinrecovery:
    if (defParsererrflag < 3)
    {
        defParsererrflag = 3;
        for (;;)
        {
            if ((defParsern = defParsersindex[*defParserssp]) && (defParsern += defParserERRCODE) >= 0 &&
                    defParsern <= defParserTABLESIZE && defParsercheck[defParsern] == defParserERRCODE)
            {
#if defParserDEBUG
                if (defParserdebug)
                    printf("defParserdebug: state %d, error recovery shifting\
 to state %d\n", *defParserssp, defParsertable[defParsern]);
#endif
                if (defParserssp >= defParserss + defParserstacksize - 1)
                {
                    goto defParseroverflow;
                }
                *++defParserssp = defParserstate = defParsertable[defParsern];
                *++defParservsp = defParserlval;
                goto defParserloop;
            }
            else
            {
#if defParserDEBUG
                if (defParserdebug)
                    printf("defParserdebug: error recovery discarding state %d\n",
                            *defParserssp);
#endif
                if (defParserssp <= defParserss) goto defParserabort;
                --defParserssp;
                --defParservsp;
            }
        }
    }
    else
    {
        if (defParserchar == 0) goto defParserabort;
#if defParserDEBUG
        if (defParserdebug)
        {
            defParsers = 0;
            if (defParserchar <= defParserMAXTOKEN) defParsers = defParsername[defParserchar];
            if (!defParsers) defParsers = "illegal-symbol";
            printf("defParserdebug: state %d, error recovery discards token %d (%s)\n",
                    defParserstate, defParserchar, defParsers);
        }
#endif
        defParserchar = (-1);
        goto defParserloop;
    }
defParserreduce:
#if defParserDEBUG
    if (defParserdebug)
        printf("defParserdebug: state %d, reducing by rule %d (%s)\n",
                defParserstate, defParsern, defParserrule[defParsern]);
#endif
    defParserm = defParserlen[defParsern];
    defParserval = defParservsp[1-defParserm];
    switch (defParsern)
    {
case 1:
{
	   g_def_list = defParservsp[0].defListPtrData;
	}
break;
case 2:
{
	   defParserval.defListPtrData=defParservsp[-1].defListPtrData;

	   if ( defParservsp[0].defPtrData -> type() == FeatureDef::WILDCARD )
	      defParserval.defListPtrData -> prepend(defParservsp[0].defPtrData);
           else
	      defParserval.defListPtrData -> append(defParservsp[0].defPtrData);
	}
break;
case 3:
{
	   defParserval.defListPtrData=new def_list_t();
	   defParserval.defListPtrData -> append(defParservsp[0].defPtrData);
	}
break;
case 4:
{
	   defParserval.defPtrData=defParservsp[0].defPtrData;
	}
break;
case 5:
{
	   defParserval.defPtrData=defParservsp[0].defPtrData;
	}
break;
case 6:
{
	   defParserval.defPtrData=defParservsp[0].defPtrData;
	}
break;
case 7:
{
	   defParserval.defPtrData=defParservsp[0].defPtrData;
	}
break;
case 8:
{
	   defParserval.defPtrData= new FeatureDefComposite((char*)defParservsp[-3].charPtrData, defParservsp[-1].defListPtrData);
		delete defParservsp[-3].charPtrData;
	}
break;
case 9:
{
	   defParserval.defPtrData= new FeatureDefPrimitive((char*)defParservsp[-3].charPtrData, defParservsp[-1].typeValuesListPtrData);
		delete defParservsp[-3].charPtrData;
	}
break;
case 10:
{
	   defParserval.defPtrData= new FeatureDefReference((char*)defParservsp[-1].charPtrData);
		delete defParservsp[-1].charPtrData;
	}
break;
case 11:
{
	   defParserval.defPtrData= new FeatureDefWildCard("*");
	}
break;
case 12:
{
	  defParserval.charPtrData=defParservsp[0].charPtrData;
	}
break;
case 13:
{
	   defParserval.typeValuesListPtrData=defParservsp[-2].typeValuesListPtrData;
	   defParserval.typeValuesListPtrData -> append(defParservsp[0].typeValuesPtrData);
	}
break;
case 14:
{
	   defParserval.typeValuesListPtrData=new type_values_list_t();
	   defParserval.typeValuesListPtrData -> append(defParservsp[0].typeValuesPtrData);
	}
break;
case 15:
{
		defParserval.typeValuesPtrData=new TypeValues((char*)defParservsp[-3].charPtrData, defParservsp[-1].valueListPtrData);
		delete defParservsp[-3].charPtrData;
	}
break;
case 16:
{
		defParserval.typeValuesPtrData=new TypeValues((char*)defParservsp[0].charPtrData, 0);
		delete defParservsp[0].charPtrData;
	}
break;
case 17:
{
	   defParservsp[-2].valueListPtrData -> append(defParservsp[0].valueData);
	   defParserval.valueListPtrData=defParservsp[-2].valueListPtrData;
	}
break;
case 18:
{
	   defParserval.valueListPtrData = new defv_t();
	   defParserval.valueListPtrData -> append(defParservsp[0].valueData);
            
	}
break;
case 19:
{
	   defParserval.valueData=new FeatureValueInt(defParservsp[0].intData);
	}
break;
case 20:
{
	   defParserval.valueData=new FeatureValueString((char*)defParservsp[0].charPtrData);
           delete defParservsp[0].charPtrData;
	}
break;
case 21:
{
	   defParserval.valueData=new FeatureValueReal(defParservsp[0].realData);
	}
break;
    }
    defParserssp -= defParserm;
    defParserstate = *defParserssp;
    defParservsp -= defParserm;
    defParserm = defParserlhs[defParsern];
    if (defParserstate == 0 && defParserm == 0)
    {
#if defParserDEBUG
        if (defParserdebug)
            printf("defParserdebug: after reduction, shifting from state 0 to\
 state %d\n", defParserFINAL);
#endif
        defParserstate = defParserFINAL;
        *++defParserssp = defParserFINAL;
        *++defParservsp = defParserval;
        if (defParserchar < 0)
        {
            if ((defParserchar = defParserlex()) < 0) defParserchar = 0;
#if defParserDEBUG
            if (defParserdebug)
            {
                defParsers = 0;
                if (defParserchar <= defParserMAXTOKEN) defParsers = defParsername[defParserchar];
                if (!defParsers) defParsers = "illegal-symbol";
                printf("defParserdebug: state %d, reading %d (%s)\n",
                        defParserFINAL, defParserchar, defParsers);
            }
#endif
        }
        if (defParserchar == 0) goto defParseraccept;
        goto defParserloop;
    }
    if ((defParsern = defParsergindex[defParserm]) && (defParsern += defParserstate) >= 0 &&
            defParsern <= defParserTABLESIZE && defParsercheck[defParsern] == defParserstate)
        defParserstate = defParsertable[defParsern];
    else
        defParserstate = defParserdgoto[defParserm];
#if defParserDEBUG
    if (defParserdebug)
        printf("defParserdebug: after reduction, shifting from state %d \
to state %d\n", *defParserssp, defParserstate);
#endif
    if (defParserssp >= defParserss + defParserstacksize - 1)
    {
        goto defParseroverflow;
    }
    *++defParserssp = defParserstate;
    *++defParservsp = defParserval;
    goto defParserloop;
defParseroverflow:
    defParsererror((char*)"yacc stack overflow");
defParserabort:
    return (1);
defParseraccept:
    return (0);
}
