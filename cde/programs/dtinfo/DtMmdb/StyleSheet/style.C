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
// $TOG: style.C /main/6 1998/04/17 11:51:49 mgreess $
#ifndef lint
static const char stylesccsid[] = "@(#)yaccpar	1.8 (Berkeley) 01/20/90";
#endif
#define styleBYACC 1
#include <stdio.h>
#include <ctype.h>
#include <sstream>
#include <iostream>
using namespace std;
#include <assert.h>
#include "StyleSheetExceptions.h"
#include "VariableTable.h"
#include "FeatureValue.h"
#include "Expression.h"
#include "SSPath.h"
#include "PathTable.h"
#include "Renderer.h"
#include "PathQualifier.h"
#include "Debug.h"
#include "ParserConst.h"
#include "FeatureDefDictionary.h"
#include <utility/funcs.h>

#include "HardCopy/autoNumberFP.h"
extern autoNumberFP gAutoNumberFP;


#ifdef alloca
# undef alloca
#endif
#define alloca(x)       (malloc(x))

extern void styleerror(char*);
extern int stylelex();

extern void enter_sgmlgi_context();

extern featureDefDictionary* g_FeatureDefDictionary;
extern unsigned g_validation_mode;
extern unsigned g_hasSemanticError;

static char localCharToCharPtrBuf[2];

#undef stylewrap

const char* toUpperCase(unsigned char* string)
{
   static char buffer[512];
   int j=0;
   for ( unsigned int i=0; i<strlen((const char*)string); i++ )
   {
		 if (islower(string[i]))
		   buffer[j] = toupper(string[i]) ;
		 else
		   buffer[j] = (char)string[i] ;
		 j++;
   }
   buffer[j] = 0;
   return buffer;
}

typedef union
{
 unsigned char  charData;
 unsigned char* charPtrData;
 unsigned int	boolData;
 int   		intData;
 float		realData;
 Expression*    expPtrData;
 TermNode*      termNodePtrData;
 FeatureValue*	FeatureValuePtrData;
 FeatureSet*	FeatureSetPtrData;
 Feature*	FeaturePtrData;
 SSPath*	PathPtrData;
 PathTerm*	PathTermPtrData;
 charPtrDlist* 	charPtrDlistData;
 PathFeatureList* PathFeatureListPtrData;
 CompositeVariableNode*      CompositeVariableNodePtrData;

 CC_TPtrSlist<FeatureValue>* FeatureValueSlistPtrData;

 PQExpr*	PQExprPtrData;
} styleSTYPE;
#define INTEGER 257
#define OPER_equality 258
#define OPER_relational 259
#define BOOLVAL 260
#define REAL 261
#define OPER_assign 262
#define ARRAYOPEN 263
#define ARRAYCLOSE 264
#define SEPARATOR 265
#define FSOPEN 266
#define FSCLOSE 267
#define OPER_modify 268
#define OPER_parent 269
#define OPER_attr 270
#define OPER_oneof 271
#define OPER_star 272
#define OPER_or 273
#define OPER_and 274
#define OPER_div 275
#define OPER_parenopen 276
#define OPER_parenclose 277
#define OPER_logicalnegate 278
#define PMEMOPEN 279
#define PMEMCLOSE 280
#define OPER_period 281
#define OPER_plus 282
#define OPER_minus 283
#define DIMENSION 284
#define NORMAL_STRING 285
#define UNIT_STRING 286
#define QUOTED_STRING 287
#define GI_CASE_SENSITIVE 288
#define SGMLGI_STRING 289
#define styleERRCODE 256
short stylelhs[] = {                                        -1,
    0,   37,   38,   38,   39,   13,   13,   13,   22,   22,
   14,   15,   19,   19,   19,   11,   11,   10,   10,    7,
    7,    6,    6,   12,   12,   12,   12,   12,   12,   12,
   12,   12,   12,   20,   20,    4,    4,   23,   23,   21,
   21,   17,   17,   18,   18,   40,   40,   24,   29,   29,
   28,   28,   27,   27,   25,   25,   26,   26,   26,    9,
    8,    8,    2,   41,    5,    5,    5,   16,    3,    3,
   34,   30,   30,   31,   31,   32,   32,    1,    1,   35,
   35,   36,   36,   42,   42,   33,   33,
};
short stylelen[] = {                                         2,
    2,    3,    1,    1,    3,    1,    2,    1,    3,    1,
    2,    2,    1,    1,    1,    3,    1,    3,    1,    1,
    1,    1,    1,    1,    2,    1,    1,    1,    2,    2,
    1,    1,    3,    4,    3,    1,    0,    3,    1,    1,
    1,    4,    2,    3,    1,    1,    0,    3,    3,    1,
    2,    4,    3,    1,    2,    1,    2,    1,    1,    1,
    1,    1,    1,    0,    4,    2,    1,    1,    1,    1,
    3,    1,    3,    1,    3,    4,    3,    1,    1,    1,
    0,    1,    0,    2,    1,    1,    0,
};
short styledefred[] = {                                      0,
    0,    0,    0,   80,    0,   58,   59,   69,   70,    0,
    0,   63,    0,   56,    4,    1,   85,    3,    0,    2,
    0,   57,   86,    0,    0,    0,    0,    0,   51,   55,
   84,    0,    0,    0,    0,   74,    0,   24,    0,    0,
    0,    0,   68,   28,    0,    0,    0,    0,   19,    0,
    0,    6,   27,   15,    5,   14,    0,    0,   43,   50,
    0,   45,    0,    0,   54,    0,    0,   71,    0,    0,
   29,   30,   11,   12,   10,    0,    0,   22,   23,    0,
   61,   62,    0,   25,    0,   60,    0,   65,    0,    0,
    0,    0,    0,   52,    0,   78,   79,   77,    0,   75,
   33,   35,    0,   41,    0,    0,   18,    0,    9,   44,
   42,   48,   49,   53,   76,    0,   34,   38,
};
short styledgoto[] = {                                       2,
   98,   10,   28,   46,   12,   80,    0,   83,   87,   47,
   48,   49,   50,   51,   52,   53,   54,   61,   55,   56,
  105,   57,  106,   62,   13,   14,   64,   15,   63,   34,
   35,   36,   22,   23,    3,   16,    4,   17,   18,   90,
   58,   19,
};
short stylesindex[] = {                                   -277,
 -246,    0, -225,    0, -218,    0,    0,    0,    0, -231,
 -228,    0, -259,    0,    0,    0,    0,    0, -225,    0,
 -265,    0,    0, -221,    0, -234, -225, -222,    0,    0,
    0, -229, -196, -206, -204,    0, -213,    0, -187, -229,
 -229, -168,    0,    0,    0, -156, -251, -177,    0, -172,
 -229,    0,    0,    0,    0,    0, -161, -167,    0,    0,
 -141,    0, -237, -255,    0, -133, -249,    0, -265, -265,
    0,    0,    0,    0,    0, -199, -189,    0,    0, -168,
    0,    0, -168,    0, -161,    0, -229,    0, -229, -139,
 -221, -229, -225,    0, -158,    0,    0,    0, -204,    0,
    0,    0, -177,    0, -132, -130,    0, -251,    0,    0,
    0,    0,    0,    0,    0, -168,    0,    0,
};
short stylerindex[] = {                                     37,
    0,    0,  132,    0,    0,    0,    0,    0,    0, -145,
 -257,    0,    0,    0,    0,    0,    0,    0,  135,    0,
    0,    0,    0, -127,    1,    0,    0,   30,    0,    0,
    0,    0,    0,    0, -179,    0,  169,    0,  178,    0,
    0,    0,    0,    0,   54,    0,  123,  130,    0,  200,
    0,    0,    0,    0,    0,    0,   77,    0,    0,    0,
 -129,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0, -127,    0,    0,    0,
    0,    0,    0,    0,  100,    0,    0,    0, -128,    0,
 -127,    0,    0,    0,    0,    0,    0,    0, -160,    0,
    0,    0, -155,    0, -122,    0,    0,  146,    0,    0,
    0,    0,    0,    0,    0, -127,    0,    0,
};
short stylegindex[] = {                                      0,
    0,   71,   -1,    0,    0,    0,    0,    0,   74,   60,
  -42,   64,    0,    0,    0,    0,  134,    0,   57,  -73,
    0,   94,   33,   61,    0,  138,    0,  -24,    0,    0,
   83,   84,    0,    0,    0,    0,    0,  136,    0,    0,
    0,    0,
};
#define styleTABLESIZE 485
short styletable[] = {                                      76,
   66,   11,   65,  104,   32,   67,   26,   96,   67,   93,
    1,    6,    7,   67,   67,    5,   27,   11,   67,   33,
   78,   94,   45,   79,   60,    8,    9,   67,   67,   67,
   91,   21,   59,   24,  103,   37,   81,   97,   38,   39,
   75,   20,  104,   86,   26,    6,    7,   40,   41,   75,
    8,    9,   25,   10,   42,    8,    9,   68,   25,    8,
    9,   67,   43,    8,    9,   44,   69,   37,  114,   70,
   38,   39,   71,  103,  102,   45,    8,  101,   75,   40,
   41,   75,   81,   82,   72,  109,   42,   60,   37,   45,
  113,   38,   39,   72,   43,    8,    9,   44,   72,    7,
   40,   41,   66,   73,   81,   82,   77,   42,   40,   40,
   73,   74,   73,   84,   45,   43,    8,    9,   44,   86,
   87,   88,   17,   89,   95,   87,   87,  111,  115,   13,
   87,   83,  116,  117,   82,   37,   92,   47,   46,   87,
   87,   39,  108,  107,   85,   16,   29,  112,  118,  110,
   30,   99,    0,  100,   31,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   31,    0,
    0,    0,    0,    0,    0,    0,    0,   32,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   26,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   66,    0,
    0,    0,    0,   66,   66,   66,   66,   66,    0,    0,
    0,   66,   66,    0,    0,   66,   66,   66,    0,    0,
    0,    0,   66,   66,    0,   66,   66,   67,    0,   64,
    0,    0,   67,   67,   67,   67,   67,    0,    0,    0,
   67,   67,    0,    0,   67,   67,   67,   81,   81,    0,
    0,   67,   67,    0,   67,   67,   36,   10,   10,    0,
   10,   81,   81,    0,   10,   10,    0,    0,   10,    0,
    0,    0,    0,    0,   10,   10,   10,    0,   10,   10,
    8,    8,    0,    8,    0,    0,    0,    8,    8,    0,
    0,    8,    0,    8,    0,    0,    0,    0,    8,    8,
    0,    8,    8,    7,    7,    0,    7,    0,    0,    0,
    7,    7,    0,    0,    7,    0,    7,    0,    0,    0,
    0,    7,    7,    0,    7,    7,   17,   17,    0,   17,
    0,    0,    0,   17,   13,    0,   13,    0,    0,   17,
   13,   13,    0,    0,   17,   17,    0,   17,   17,   16,
   16,    0,   16,    0,   13,   13,   16,    0,    0,    0,
    0,    0,   16,    0,    0,    0,    0,   16,   16,    0,
   16,   16,   31,   31,    0,   31,    0,    0,    0,   31,
   31,   32,   32,   31,   32,   31,    0,    0,   32,   32,
   31,   31,   32,   31,   32,    0,    0,    0,    0,   32,
   32,    0,   32,   26,   26,    0,   26,    0,    0,    0,
   26,   26,    0,    0,   26,    0,   26,    0,    0,    0,
    0,   26,   26,    0,   26,
};
short stylecheck[] = {                                      42,
    0,    3,   27,   77,  270,  263,  266,  257,  266,  265,
  288,  271,  272,  271,  272,  262,  276,   19,  276,  285,
  272,  277,   24,  275,   26,  285,  286,  285,  286,    0,
  268,  263,  267,  262,   77,  257,    0,  287,  260,  261,
   42,  260,  116,  281,  266,  271,  272,  269,  270,   51,
  285,  286,  281,    0,  276,  285,  286,  264,  281,  285,
  286,  258,  284,  285,  286,  287,  273,  257,   93,  274,
  260,  261,  286,  116,  264,   77,    0,  277,   80,  269,
  270,   83,  282,  283,  264,   87,  276,   89,  257,   91,
   92,  260,  261,  273,  284,  285,  286,  287,  286,    0,
  269,  270,   32,  264,  282,  283,  263,  276,  264,  265,
   40,   41,  273,  286,  116,  284,  285,  286,  287,  281,
  266,  289,    0,  265,  258,  271,  272,  267,  287,    0,
  276,    0,  265,  264,    0,  263,   63,  267,  267,  285,
  286,  264,   83,   80,   51,    0,   13,   91,  116,   89,
   13,   69,   -1,   70,   19,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,    0,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,    0,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,    0,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  258,   -1,
   -1,   -1,   -1,  263,  264,  265,  266,  267,   -1,   -1,
   -1,  271,  272,   -1,   -1,  275,  276,  277,   -1,   -1,
   -1,   -1,  282,  283,   -1,  285,  286,  258,   -1,  289,
   -1,   -1,  263,  264,  265,  266,  267,   -1,   -1,   -1,
  271,  272,   -1,   -1,  275,  276,  277,  271,  272,   -1,
   -1,  282,  283,   -1,  285,  286,  263,  264,  265,   -1,
  267,  285,  286,   -1,  271,  272,   -1,   -1,  275,   -1,
   -1,   -1,   -1,   -1,  281,  282,  283,   -1,  285,  286,
  264,  265,   -1,  267,   -1,   -1,   -1,  271,  272,   -1,
   -1,  275,   -1,  277,   -1,   -1,   -1,   -1,  282,  283,
   -1,  285,  286,  264,  265,   -1,  267,   -1,   -1,   -1,
  271,  272,   -1,   -1,  275,   -1,  277,   -1,   -1,   -1,
   -1,  282,  283,   -1,  285,  286,  264,  265,   -1,  267,
   -1,   -1,   -1,  271,  265,   -1,  267,   -1,   -1,  277,
  271,  272,   -1,   -1,  282,  283,   -1,  285,  286,  264,
  265,   -1,  267,   -1,  285,  286,  271,   -1,   -1,   -1,
   -1,   -1,  277,   -1,   -1,   -1,   -1,  282,  283,   -1,
  285,  286,  264,  265,   -1,  267,   -1,   -1,   -1,  271,
  272,  264,  265,  275,  267,  277,   -1,   -1,  271,  272,
  282,  283,  275,  285,  277,   -1,   -1,   -1,   -1,  282,
  283,   -1,  285,  264,  265,   -1,  267,   -1,   -1,   -1,
  271,  272,   -1,   -1,  275,   -1,  277,   -1,   -1,   -1,
   -1,  282,  283,   -1,  285,
};
#define styleFINAL 2
#ifndef styleDEBUG
#define styleDEBUG 0
#endif
#define styleMAXTOKEN 289
#if styleDEBUG
char *stylename[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"INTEGER","OPER_equality",
"OPER_relational","BOOLVAL","REAL","OPER_assign","ARRAYOPEN","ARRAYCLOSE",
"SEPARATOR","FSOPEN","FSCLOSE","OPER_modify","OPER_parent","OPER_attr",
"OPER_oneof","OPER_star","OPER_or","OPER_and","OPER_div","OPER_parenopen",
"OPER_parenclose","OPER_logicalnegate","PMEMOPEN","PMEMCLOSE","OPER_period",
"OPER_plus","OPER_minus","DIMENSION","NORMAL_STRING","UNIT_STRING",
"QUOTED_STRING","GI_CASE_SENSITIVE","SGMLGI_STRING",
};
char *stylerule[] = {
"$accept : stylesheet",
"stylesheet : sensitivityOPTL statement.gpOPTL",
"sensitivity : GI_CASE_SENSITIVE OPER_assign BOOLVAL",
"statement.gp : var_assignment",
"statement.gp : path_expr",
"var_assignment : STRING OPER_assign rhs.gp",
"symbol : attr",
"symbol : parent string_list",
"symbol : string_list",
"string_list : string_list OPER_feature STRING",
"string_list : STRING",
"parent : OPER_parent SGMLGI",
"attr : OPER_attr SGMLGI",
"rhs.gp : simple_expr",
"rhs.gp : array",
"rhs.gp : featureset",
"simple_expr : simple_expr OPER_add multi_expr",
"simple_expr : multi_expr",
"multi_expr : multi_expr OPER_mult term",
"multi_expr : term",
"OPER_binop : OPER_mult",
"OPER_binop : OPER_add",
"OPER_mult : OPER_star",
"OPER_mult : OPER_div",
"term : BOOLVAL",
"term : symbol UNIT_STRING",
"term : symbol",
"term : dimension",
"term : QUOTED_STRING",
"term : INTEGER UNIT_STRING",
"term : REAL UNIT_STRING",
"term : INTEGER",
"term : REAL",
"term : OPER_parenopen simple_expr OPER_parenclose",
"array : array_name ARRAYOPEN array_member_list ARRAYCLOSE",
"array : array_name ARRAYOPEN ARRAYCLOSE",
"array_name : STRING",
"array_name :",
"array_member_list : array_member SEPARATOR array_member_list",
"array_member_list : array_member",
"array_member : simple_expr",
"array_member : array",
"featureset : FSOPEN feature_list SEPARATOR_OPTL FSCLOSE",
"featureset : FSOPEN FSCLOSE",
"feature_list : feature_list SEPARATOR feature",
"feature_list : feature",
"SEPARATOR_OPTL : SEPARATOR",
"SEPARATOR_OPTL :",
"feature : feature_name_list OPER_modify rhs.gp",
"feature_name_list : feature_name_list OPER_feature STRING",
"feature_name_list : STRING",
"path_expr : path_term_list featureset",
"path_expr : path_term_list OPER_parenopen path_expr_list OPER_parenclose",
"path_expr_list : path_expr_list SEPARATOR path_expr",
"path_expr_list : path_expr",
"path_term_list : path_term_list path_term",
"path_term_list : path_term",
"path_term : SGMLGI path_selectorOPTL",
"path_term : OPER_oneof",
"path_term : OPER_star",
"OPER_feature : OPER_period",
"OPER_add : OPER_plus",
"OPER_add : OPER_minus",
"SGMLGI : SGMLGI_CONTENT",
"$$1 :",
"SGMLGI_CONTENT : STRING OPER_period $$1 SGMLGI_STRING",
"SGMLGI_CONTENT : STRING OPER_period",
"SGMLGI_CONTENT : STRING",
"dimension : DIMENSION",
"STRING : NORMAL_STRING",
"STRING : UNIT_STRING",
"path_selector : ARRAYOPEN boolean_expr ARRAYCLOSE",
"boolean_expr : logical_and_expr",
"boolean_expr : boolean_expr OPER_or logical_and_expr",
"logical_and_expr : equality_expr",
"logical_and_expr : logical_and_expr OPER_and equality_expr",
"equality_expr : OPER_attr SGMLGI OPER_equality QUOTED_STRING",
"equality_expr : NORMAL_STRING OPER_equality POSITION_VALUE",
"POSITION_VALUE : INTEGER",
"POSITION_VALUE : QUOTED_STRING",
"sensitivityOPTL : sensitivity",
"sensitivityOPTL :",
"statement.gpOPTL : statement.gpPLUS",
"statement.gpOPTL :",
"statement.gpPLUS : statement.gpPLUS statement.gp",
"statement.gpPLUS : statement.gp",
"path_selectorOPTL : path_selector",
"path_selectorOPTL :",
};
#endif
#define styleclearin (stylechar=(-1))
#define styleerrok (styleerrflag=0)
#ifdef styleSTACKSIZE
#ifndef styleMAXDEPTH
#define styleMAXDEPTH styleSTACKSIZE
#endif
#else
#ifdef styleMAXDEPTH
#define styleSTACKSIZE styleMAXDEPTH
#else
#define styleSTACKSIZE 500
#define styleMAXDEPTH 500
#endif
#endif
int styledebug;
int stylenerrs;
int styleerrflag;
int stylechar;
short *stylessp;
styleSTYPE *stylevsp;
styleSTYPE styleval;
styleSTYPE stylelval;
short styless[styleSTACKSIZE];
styleSTYPE stylevs[styleSTACKSIZE];
#define stylestacksize styleSTACKSIZE
#define styleABORT goto styleabort
#define styleACCEPT goto styleaccept
#define styleERROR goto styleerrlab
#if styleDEBUG
#ifndef __cplusplus
extern char *getenv(const char *);
#endif
#endif
int
styleparse()
{
    register int stylem, stylen, stylestate;
#if styleDEBUG
    register char *styles;

    if (styles = getenv("styleDEBUG"))
    {
        stylen = *styles;
        if (stylen >= '0' && stylen <= '9')
            styledebug = stylen - '0';
    }
#endif

    stylenerrs = 0;
    styleerrflag = 0;
    stylechar = (-1);

    stylessp = styless;
    stylevsp = stylevs;
    *stylessp = stylestate = 0;

styleloop:
    if ((stylen = styledefred[stylestate])) goto stylereduce;
    if (stylechar < 0)
    {
        if ((stylechar = stylelex()) < 0) stylechar = 0;
#if styleDEBUG
        if (styledebug)
        {
            styles = 0;
            if (stylechar <= styleMAXTOKEN) styles = stylename[stylechar];
            if (!styles) styles = "illegal-symbol";
            printf("styledebug: state %d, reading %d (%s)\n", stylestate,
                    stylechar, styles);
        }
#endif
    }
    if ((stylen = stylesindex[stylestate]) && (stylen += stylechar) >= 0 &&
            stylen <= styleTABLESIZE && stylecheck[stylen] == stylechar)
    {
#if styleDEBUG
        if (styledebug)
            printf("styledebug: state %d, shifting to state %d\n",
                    stylestate, styletable[stylen]);
#endif
        if (stylessp >= styless + stylestacksize - 1)
        {
            goto styleoverflow;
        }
        *++stylessp = stylestate = styletable[stylen];
        *++stylevsp = stylelval;
        stylechar = (-1);
        if (styleerrflag > 0)  --styleerrflag;
        goto styleloop;
    }
    if ((stylen = stylerindex[stylestate]) && (stylen += stylechar) >= 0 &&
            stylen <= styleTABLESIZE && stylecheck[stylen] == stylechar)
    {
        stylen = styletable[stylen];
        goto stylereduce;
    }
    if (styleerrflag) goto styleinrecovery;
#if 0					/* remove if needed */
#ifdef lint
    goto stylenewerror;
#endif
stylenewerror:
#endif	/* if 0 */
    styleerror((char*)"syntax error");
#if 0					/* remove if needed */
#ifdef lint
    goto styleerrlab;
#endif
styleerrlab:
#endif /* if 0 */
    ++stylenerrs;
styleinrecovery:
    if (styleerrflag < 3)
    {
        styleerrflag = 3;
        for (;;)
        {
            if ((stylen = stylesindex[*stylessp]) && (stylen += styleERRCODE) >= 0 &&
                    stylen <= styleTABLESIZE && stylecheck[stylen] == styleERRCODE)
            {
#if styleDEBUG
                if (styledebug)
                    printf("styledebug: state %d, error recovery shifting\
 to state %d\n", *stylessp, styletable[stylen]);
#endif
                if (stylessp >= styless + stylestacksize - 1)
                {
                    goto styleoverflow;
                }
                *++stylessp = stylestate = styletable[stylen];
                *++stylevsp = stylelval;
                goto styleloop;
            }
            else
            {
#if styleDEBUG
                if (styledebug)
                    printf("styledebug: error recovery discarding state %d\n",
                            *stylessp);
#endif
                if (stylessp <= styless) goto styleabort;
                --stylessp;
                --stylevsp;
            }
        }
    }
    else
    {
        if (stylechar == 0) goto styleabort;
#if styleDEBUG
        if (styledebug)
        {
            styles = 0;
            if (stylechar <= styleMAXTOKEN) styles = stylename[stylechar];
            if (!styles) styles = "illegal-symbol";
            printf("styledebug: state %d, error recovery discards token %d (%s)\n",
                    stylestate, stylechar, styles);
        }
#endif
        stylechar = (-1);
        goto styleloop;
    }
stylereduce:
#if styleDEBUG
    if (styledebug)
        printf("styledebug: state %d, reducing by rule %d (%s)\n",
                stylestate, stylen, stylerule[stylen]);
#endif
    stylem = stylelen[stylen];
    styleval = stylevsp[1-stylem];
    switch (stylen)
    {
case 1:
{
	}
break;
case 2:
{
          gGI_CASE_SENSITIVE = stylevsp[0].boolData;
	}
break;
case 3:
{
	}
break;
case 4:
{
	  /* copy items form the feature list into the path table */
          PathFeatureListIterator l_Iter(*(stylevsp[0].PathFeatureListPtrData));

          PathFeature *x = 0;

          while ( ++l_Iter ) {

            x = l_Iter.key();

            if ( g_validation_mode == true )
              if ( g_FeatureDefDictionary -> checkSemantics(x -> featureSet()) == false )
                 g_hasSemanticError = true;
               
	    gPathTab -> addPathFeatureSet( x );


          }

/* clear out the first list so the elements are not deleted
             with the list because they are still referenced by the
             path table */

	  stylevsp[0].PathFeatureListPtrData -> clear();
	  delete stylevsp[0].PathFeatureListPtrData;

	}
break;
case 5:
{
           Expression *x = new Expression(new ConstantNode(stylevsp[0].FeatureValuePtrData));

           if ( gAutoNumberFP.accept((const char*)stylevsp[-2].charPtrData, x) ) {
	         delete stylevsp[-2].charPtrData;
	         delete x;
                 break;
           }

           gVariableTable -> enter( gSymTab -> intern((const char*)stylevsp[-2].charPtrData), x);
	   delete stylevsp[-2].charPtrData;
	}
break;
case 6:
{
          styleval.termNodePtrData=stylevsp[0].termNodePtrData;
	}
break;
case 7:
{
          styleval.termNodePtrData=stylevsp[-1].termNodePtrData;
	}
break;
case 8:
{
          const Symbol* x = stylevsp[0].CompositeVariableNodePtrData -> convertableToVariable();
          if ( x ) {
            styleval.termNodePtrData=new VariableNode(*x);
            delete stylevsp[0].CompositeVariableNodePtrData;
          } else
            styleval.termNodePtrData=stylevsp[0].CompositeVariableNodePtrData;
	}
break;
case 9:
{

	  stylevsp[-2].CompositeVariableNodePtrData->appendItem(gSymTab->intern(toUpperCase(stylevsp[0].charPtrData)));
	  styleval.CompositeVariableNodePtrData=stylevsp[-2].CompositeVariableNodePtrData;
	  delete stylevsp[0].charPtrData ;
	}
break;
case 10:
{
	  styleval.CompositeVariableNodePtrData=new CompositeVariableNode;
          styleval.CompositeVariableNodePtrData -> appendItem(gSymTab->intern(toUpperCase(stylevsp[0].charPtrData)));
	  delete stylevsp[0].charPtrData;
	}
break;
case 11:
{
/*
	  $$=new
            ParentNode(gSymTab->intern((const char*)$1));
*/
          MESSAGE(cerr, "^ operator not supported.");
          throw(CASTSSEXCEPT StyleSheetException());
	}
break;
case 12:
{
	  styleval.termNodePtrData=new
            SgmlAttributeNode(gSymTab->intern((const char*)stylevsp[0].charPtrData));
	  delete stylevsp[0].charPtrData;
	}
break;
case 13:
{
	  styleval.FeatureValuePtrData=new FeatureValueExpression(stylevsp[0].expPtrData);
	}
break;
case 14:
{
          styleval.FeatureValuePtrData=stylevsp[0].FeatureValuePtrData;
	}
break;
case 15:
{
	  styleval.FeatureValuePtrData=new FeatureValueFeatureSet(stylevsp[0].FeatureSetPtrData);
	}
break;
case 16:
{
	   BinaryOperatorNode::operatorType opType;
           switch (stylevsp[-1].charData) {
             case '+': opType=BinaryOperatorNode::PLUS; break;
             case '-': opType=BinaryOperatorNode::MINUS; break;
             default:
              throw(CASTBEEXCEPT badEvaluationException());
           }

           FeatureValueExpression* FVexprL = new FeatureValueExpression(stylevsp[-2].expPtrData);
           FeatureValueExpression* FVexprR = new FeatureValueExpression(stylevsp[0].expPtrData);
           
           styleval.expPtrData = new Expression(
		 new BinaryOperatorNode(opType, 
					new ConstantNode(FVexprL),
					new ConstantNode(FVexprR)
				       )
			      );
	}
break;
case 17:
{
	   styleval.expPtrData=stylevsp[0].expPtrData;
	}
break;
case 18:
{
	   BinaryOperatorNode::operatorType opType;
           switch (stylevsp[-1].charData) {
             case '*': opType=BinaryOperatorNode::TIMES; break;
             case '/': opType=BinaryOperatorNode::DIVIDE; break;
             default:
              throw(CASTBEEXCEPT badEvaluationException());
           }

           FeatureValueExpression* FVexpr = new FeatureValueExpression(stylevsp[-2].expPtrData);
           
           styleval.expPtrData = new Expression(
		 new BinaryOperatorNode(opType, new ConstantNode(FVexpr), stylevsp[0].termNodePtrData)
			      );
	}
break;
case 19:
{
           styleval.expPtrData = new Expression(stylevsp[0].termNodePtrData);
	}
break;
case 20:
{
	   styleval.charData=stylevsp[0].charData;
        }
break;
case 21:
{
	   styleval.charData=stylevsp[0].charData;
	}
break;
case 22:
{
	   styleval.charData=stylevsp[0].charData;
        }
break;
case 23:
{
	   styleval.charData=stylevsp[0].charData;
        }
break;
case 24:
{
          styleval.termNodePtrData=new ConstantNode(new FeatureValueInt(int(stylevsp[0].boolData)));
        }
break;
case 25:
{
	  FeatureValueExpression* fve = 
		new FeatureValueExpression(new Expression(stylevsp[-1].termNodePtrData));
          FeatureValueDimension* x = 
             new FeatureValueDimension(fve, (const char*)stylevsp[0].charPtrData);
	  delete stylevsp[0].charPtrData ;
          styleval.termNodePtrData=new ConstantNode(x);
	}
break;
case 26:
{
	  styleval.termNodePtrData=stylevsp[0].termNodePtrData;
	}
break;
case 27:
{
	  styleval.termNodePtrData=stylevsp[0].termNodePtrData;
	}
break;
case 28:
{
          styleval.termNodePtrData=new ConstantNode(new FeatureValueString((const char*)stylevsp[0].charPtrData));
	  delete stylevsp[0].charPtrData ;
        }
break;
case 29:
{
          styleval.termNodePtrData=new ConstantNode(new FeatureValueDimension(new FeatureValueInt(stylevsp[-1].intData), (const char*)stylevsp[0].charPtrData));
	  delete stylevsp[0].charPtrData ;
        }
break;
case 30:
{
          styleval.termNodePtrData=new ConstantNode(new FeatureValueDimension(new FeatureValueReal(stylevsp[-1].realData), (const char*)stylevsp[0].charPtrData));
	  delete stylevsp[0].charPtrData ;
        }
break;
case 31:
{
          styleval.termNodePtrData=new ConstantNode(new FeatureValueInt(stylevsp[0].intData));
        }
break;
case 32:
{
          styleval.termNodePtrData=new ConstantNode(new FeatureValueReal(stylevsp[0].realData));
        }
break;
case 33:
{
          styleval.termNodePtrData=new ConstantNode(new FeatureValueExpression(stylevsp[-1].expPtrData));
        }
break;
case 34:
{
          FeatureValueArray* x = 
	     new FeatureValueArray((const char*)stylevsp[-3].charPtrData, stylevsp[-1].FeatureValueSlistPtrData -> entries());
          CC_TPtrSlistIterator<FeatureValue> iter(*stylevsp[-1].FeatureValueSlistPtrData);

          int i = 0;
          while ( ++iter ) {
            (*x)[i++] = iter.key();
          }

          delete stylevsp[-3].charPtrData;
          delete stylevsp[-1].FeatureValueSlistPtrData;

          styleval.FeatureValuePtrData = x;
	}
break;
case 35:
{
          styleval.FeatureValuePtrData = new FeatureValueArray((const char*)stylevsp[-2].charPtrData, 0);
          delete stylevsp[-2].charPtrData;
	}
break;
case 36:
{
          styleval.charPtrData = stylevsp[0].charPtrData;
	}
break;
case 37:
{
          styleval.charPtrData = new unsigned char[1];
          styleval.charPtrData[0] = 0;
	}
break;
case 38:
{
           stylevsp[0].FeatureValueSlistPtrData -> prepend(stylevsp[-2].FeatureValuePtrData);
           styleval.FeatureValueSlistPtrData = stylevsp[0].FeatureValueSlistPtrData;
	}
break;
case 39:
{
           styleval.FeatureValueSlistPtrData=new CC_TPtrSlist<FeatureValue>;
           styleval.FeatureValueSlistPtrData -> append(stylevsp[0].FeatureValuePtrData);
	}
break;
case 40:
{
	   styleval.FeatureValuePtrData = new FeatureValueExpression(stylevsp[0].expPtrData);
	}
break;
case 41:
{
	   styleval.FeatureValuePtrData=stylevsp[0].FeatureValuePtrData;
	}
break;
case 42:
{
	   styleval.FeatureSetPtrData=stylevsp[-2].FeatureSetPtrData;
	}
break;
case 43:
{
	  styleval.FeatureSetPtrData = new FeatureSet ();
	}
break;
case 44:
{
	   if (stylevsp[0].FeaturePtrData -> name() == Symbol(gSymTab->intern("FAMILY"))) {
	     /* the evaluate() call clones $3 */
	     FeatureValueFeatureSet *fvfs = 
	       (FeatureValueFeatureSet*) stylevsp[0].FeaturePtrData->evaluate();
	     const FeatureSet* fs = fvfs->value();
	      const Feature* charsetF =
		fs->lookup(gSymTab->intern("CHARSET"));
	      /* charsetF is a mandatory entry in fontfamily*/
	      assert( charsetF );
	      const FeatureValueString* fv_string =
			(FeatureValueString*)charsetF->value();
	      const char* charset = *fv_string;
	      assert( charset );
	      
	      int entries = stylevsp[-2].FeatureSetPtrData -> entries();
	      for (int i=0; i<entries; i++) {
		const Feature* entry = stylevsp[-2].FeatureSetPtrData->at(i);
		if (! (entry->name() == Symbol(gSymTab->intern("FAMILY"))))
		  continue;
		const FeatureSet* entry_fs =
			((FeatureValueFeatureSet*)(entry->evaluate()))->value();
		const Feature* entry_charsetF =
			entry_fs->lookup(gSymTab->intern("CHARSET"));
		assert( entry_charsetF );
		const char* entry_charset =
			*((FeatureValueString*)(entry_charsetF->value()));
		assert( entry_charset );
		if (! strcmp(charset, entry_charset)) {
		  delete stylevsp[-2].FeatureSetPtrData -> removeAt(i);
		  break; /* escape from for-loop*/
		}
	      }
	      delete fvfs ;

	      styleval.FeatureSetPtrData = stylevsp[-2].FeatureSetPtrData;
	      styleval.FeatureSetPtrData -> add(stylevsp[0].FeaturePtrData);
	   }
	   else {
	      if ( stylevsp[-2].FeatureSetPtrData -> find((Feature*)stylevsp[0].FeaturePtrData) ) {
		FeatureSet* fs = new FeatureSet();
		fs -> add(stylevsp[0].FeaturePtrData);

		styleval.FeatureSetPtrData =new FeatureSet(*stylevsp[-2].FeatureSetPtrData, *fs);
		delete stylevsp[-2].FeatureSetPtrData;
		delete fs;
	      }
	      else {
		styleval.FeatureSetPtrData=stylevsp[-2].FeatureSetPtrData;
		styleval.FeatureSetPtrData -> add(stylevsp[0].FeaturePtrData);
	      }
	   }
	}
break;
case 45:
{
	   styleval.FeatureSetPtrData=new FeatureSet();
	   styleval.FeatureSetPtrData -> add(stylevsp[0].FeaturePtrData);
	}
break;
case 46:
{
	}
break;
case 47:
{
	}
break;
case 48:
{
          CC_TPtrDlistIterator<char> l_Iter(*(stylevsp[-2].charPtrDlistData));

          FeatureSet *fs = 0;
          Feature *f = 0;
          FeatureValue *fv = stylevsp[0].FeatureValuePtrData;
          const char* cptr = 0;
	  char buffer[256];
          while (++l_Iter) {
             cptr = l_Iter.key();
	     int index = 0 ;
	     const char *c = cptr ;
	     while (*c)
	       {
		 if (islower(*c))
		   buffer[index] = toupper(*c) ;
		 else
		   buffer[index] = *c ;
		 c++ ;
		 index++;
	       }
	     buffer[index] = 0;
	     /* fprintf(stderr, "converted: %s to %s\n", cptr, buffer); */
	     f = new Feature(gSymTab -> intern(buffer), fv);

             if ( stylevsp[-2].charPtrDlistData -> last() != cptr ) {
                fs = new FeatureSet();
                fs -> add(f);
                fv = new FeatureValueFeatureSet(fs);
             }
	  }

	  stylevsp[-2].charPtrDlistData->clearAndDestroy();
	  delete stylevsp[-2].charPtrDlistData ;
	  styleval.FeaturePtrData=f;
	}
break;
case 49:
{
          stylevsp[-2].charPtrDlistData -> prepend((char *)stylevsp[0].charPtrData);
          styleval.charPtrDlistData=stylevsp[-2].charPtrDlistData;
	}
break;
case 50:
{
          styleval.charPtrDlistData=new CC_TPtrDlist<char>;
          styleval.charPtrDlistData -> append((char *)stylevsp[0].charPtrData);
	}
break;
case 51:
{
           styleval.PathFeatureListPtrData=new PathFeatureList;
           styleval.PathFeatureListPtrData -> append(new PathFeature(stylevsp[-1].PathPtrData, stylevsp[0].FeatureSetPtrData));
	}
break;
case 52:
{
	  
          PathFeatureListIterator l_Iter(*(stylevsp[-1].PathFeatureListPtrData));

          while ( ++l_Iter ) {
             (l_Iter.key()) -> path() -> prependPath(*stylevsp[-3].PathPtrData);
          }
	  delete stylevsp[-3].PathPtrData;
          styleval.PathFeatureListPtrData=stylevsp[-1].PathFeatureListPtrData;
	}
break;
case 53:
{
           styleval.PathFeatureListPtrData=stylevsp[-2].PathFeatureListPtrData;
           styleval.PathFeatureListPtrData -> appendList(*stylevsp[0].PathFeatureListPtrData);
	   delete stylevsp[0].PathFeatureListPtrData ;
	}
break;
case 54:
{
           styleval.PathFeatureListPtrData=stylevsp[0].PathFeatureListPtrData;
	}
break;
case 55:
{
	  stylevsp[-1].PathPtrData -> appendPathTerm(stylevsp[0].PathTermPtrData);
	  styleval.PathPtrData=stylevsp[-1].PathPtrData;
	}
break;
case 56:
{
          styleval.PathPtrData = new SSPath;
          styleval.PathPtrData -> appendPathTerm(stylevsp[0].PathTermPtrData);
	}
break;
case 57:
{
	  styleval.PathTermPtrData=new PathTerm((const char*)stylevsp[-1].charPtrData, stylevsp[0].PQExprPtrData);
          delete stylevsp[-1].charPtrData;
	}
break;
case 58:
{
          localCharToCharPtrBuf[0]=stylevsp[0].charData; localCharToCharPtrBuf[1]=0;
	  styleval.PathTermPtrData=new PathTerm(localCharToCharPtrBuf, 0);
	}
break;
case 59:
{
          localCharToCharPtrBuf[0]=stylevsp[0].charData; localCharToCharPtrBuf[1]=0;
	  styleval.PathTermPtrData=new PathTerm(localCharToCharPtrBuf, 0);
	}
break;
case 60:
{
	}
break;
case 61:
{
	}
break;
case 62:
{
	}
break;
case 63:
{
	/* char % can start an OLIAS internal element which*/
	/* is used only by the browser.*/
	/* Example %BOGUS within HEAD1 in OLIAS book*/

           if ( stylevsp[0].charPtrData[0] != '%' && isalnum(stylevsp[0].charPtrData[0]) == 0 ) {
              MESSAGE(cerr, form("%s is not a SGMLGI", stylevsp[0].charPtrData));
              throw(CASTBEEXCEPT badEvaluationException());
           }
          /* note, should probably be using RCStrings, would make wide */
          /* char handling better too? */
           if ( gGI_CASE_SENSITIVE == false )
             {
               for (unsigned int i=0; i<strlen((const char*)stylevsp[0].charPtrData); i++)
                 if ( islower(stylevsp[0].charPtrData[i]) )
                   stylevsp[0].charPtrData[i] = toupper(stylevsp[0].charPtrData[i]);
             }
           styleval.charPtrData=stylevsp[0].charPtrData;
	}
break;
case 64:
{enter_sgmlgi_context();}
break;
case 65:
{
           int l3 = strlen((char*)stylevsp[-3].charPtrData);
           int l0 = strlen((char*)stylevsp[0].charPtrData);
           int l = l3 + l0 + 2;
           styleval.charPtrData=new unsigned char[l];

           *((char *) memcpy((char*)styleval.charPtrData,
			     (char*)stylevsp[-3].charPtrData, l3) + l3) = '\0';
           *((char *) memcpy((char*)(styleval.charPtrData + l3),
			     ".", 1) + 1) = '\0';
           *((char *) memcpy((char*)(styleval.charPtrData + l3 + 1),
			     (char*)stylevsp[0].charPtrData, l0) + l0) = '\0';

           delete stylevsp[-3].charPtrData;
           delete stylevsp[0].charPtrData;
	}
break;
case 66:
{
           int l1 = strlen((char*)stylevsp[-1].charPtrData);
           int l = l1 + 2;
           styleval.charPtrData=new unsigned char[l];

           *((char *) memcpy((char*)styleval.charPtrData,
			     (char*)stylevsp[-1].charPtrData, l1) + l1) = '\0';
           *((char *) memcpy((char*)styleval.charPtrData + l1,
			     ".", 1) + 1) = '\0';

           delete stylevsp[-1].charPtrData;
	}
break;
case 67:
{
           styleval.charPtrData=stylevsp[0].charPtrData;
	}
break;
case 68:
{
          unsigned int i;

          for (i=0; i<strlen((const char*)stylevsp[0].charPtrData); i++) {

            if ( isalpha(stylevsp[0].charPtrData[i]) ) 
               break;
          }

          char c;
          float x;
          if ( i > 0 ) {
             c = stylevsp[0].charPtrData[i]; stylevsp[0].charPtrData[i]=0;
             x = atof((const char*)stylevsp[0].charPtrData);
             stylevsp[0].charPtrData[i]=c;
          } else
             x = 1;

          styleval.termNodePtrData=new ConstantNode(new FeatureValueDimension(new FeatureValueReal(x), (const char*)&stylevsp[0].charPtrData[i]));

          delete stylevsp[0].charPtrData;
	}
break;
case 69:
{
 	   styleval.charPtrData=stylevsp[0].charPtrData;
	}
break;
case 70:
{
 	   styleval.charPtrData=stylevsp[0].charPtrData;
	}
break;
case 71:
{
           styleval.PQExprPtrData=stylevsp[-1].PQExprPtrData;
	}
break;
case 72:
{
/*////////////////////////////////////////////////////*/
/* This portion of the code (up to equality_expr) is */
/* hacked for V1.1 only. Due to the way */
/* PathQualifier.h is written, this code is not */
/* general at all. qfc 8/16/94*/
/*////////////////////////////////////////////////////*/
           styleval.PQExprPtrData=stylevsp[0].PQExprPtrData;
	}
break;
case 73:
{
           styleval.PQExprPtrData = new PQLogExpr(stylevsp[-2].PQExprPtrData, PQor, stylevsp[0].PQExprPtrData);
	}
break;
case 74:
{
           styleval.PQExprPtrData=stylevsp[0].PQExprPtrData;
	}
break;
case 75:
{
           styleval.PQExprPtrData = new PQLogExpr(stylevsp[-2].PQExprPtrData, PQand, stylevsp[0].PQExprPtrData);
	}
break;
case 76:
{
          styleval.PQExprPtrData = new PQAttributeSelector(
			gSymTab->intern((const char*)stylevsp[-2].charPtrData),
			( stylevsp[-1].intData == EQUAL ) ? PQEqual : PQNotEqual,
			(const char*)stylevsp[0].charPtrData
				      );
          delete stylevsp[-2].charPtrData;
          delete stylevsp[0].charPtrData;
	}
break;
case 77:
{
           if ( strcasecmp((char*)stylevsp[-2].charPtrData, "position") == 0 ) {
             styleval.PQExprPtrData=new PQPosition(
	  	  ( stylevsp[-1].intData == EQUAL ) ? PQEqual : PQNotEqual, 
		  stylevsp[0].intData
				);
           } else
           if ( strcasecmp((char*)stylevsp[-2].charPtrData, "sibling") == 0 ) {
             styleval.PQExprPtrData=new PQSibling(
	  	  ( stylevsp[-1].intData == EQUAL ) ? PQEqual : PQNotEqual, 
		  stylevsp[0].intData
				);
           } else
              throw(CASTSSEXCEPT StyleSheetException());

           delete stylevsp[-2].charPtrData;
	}
break;
case 78:
{
           styleval.intData = (int)stylevsp[0].intData;
	}
break;
case 79:
{
           if ( strcasecmp((char*)stylevsp[0].charPtrData, "#LAST") != 0 ) 
              throw(CASTSSEXCEPT StyleSheetException());

           styleval.intData = -1;
	}
break;
case 80:
{

	}
break;
case 81:
{

	}
break;
case 82:
{
	}
break;
case 83:
{
	}
break;
case 84:
{

	}
break;
case 85:
{

	}
break;
case 86:
{
          styleval.PQExprPtrData=stylevsp[0].PQExprPtrData;
	}
break;
case 87:
{
          styleval.PQExprPtrData=0;
	}
break;
    }
    stylessp -= stylem;
    stylestate = *stylessp;
    stylevsp -= stylem;
    stylem = stylelhs[stylen];
    if (stylestate == 0 && stylem == 0)
    {
#if styleDEBUG
        if (styledebug)
            printf("styledebug: after reduction, shifting from state 0 to\
 state %d\n", styleFINAL);
#endif
        stylestate = styleFINAL;
        *++stylessp = styleFINAL;
        *++stylevsp = styleval;
        if (stylechar < 0)
        {
            if ((stylechar = stylelex()) < 0) stylechar = 0;
#if styleDEBUG
            if (styledebug)
            {
                styles = 0;
                if (stylechar <= styleMAXTOKEN) styles = stylename[stylechar];
                if (!styles) styles = "illegal-symbol";
                printf("styledebug: state %d, reading %d (%s)\n",
                        styleFINAL, stylechar, styles);
            }
#endif
        }
        if (stylechar == 0) goto styleaccept;
        goto styleloop;
    }
    if ((stylen = stylegindex[stylem]) && (stylen += stylestate) >= 0 &&
            stylen <= styleTABLESIZE && stylecheck[stylen] == stylestate)
        stylestate = styletable[stylen];
    else
        stylestate = styledgoto[stylem];
#if styleDEBUG
    if (styledebug)
        printf("styledebug: after reduction, shifting from state %d \
to state %d\n", *stylessp, stylestate);
#endif
    if (stylessp >= styless + stylestacksize - 1)
    {
        goto styleoverflow;
    }
    *++stylessp = stylestate;
    *++stylevsp = styleval;
    goto styleloop;
styleoverflow:
    styleerror((char*)"yacc stack overflow");
styleabort:
    return (1);
styleaccept:
    return (0);
}
