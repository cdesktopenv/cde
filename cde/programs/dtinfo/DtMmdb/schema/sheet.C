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
// $TOG: sheet.C /main/4 1997/12/23 11:20:35 bill $
#ifndef lint
static const char schemasccsid[] = "@(#)yaccpar	1.8 (Berkeley) 01/20/90";
#endif
#define  schemaBYACC 1

#include <stdio.h>
#include <ctype.h>
#include "utility/funcs.h"
#include "schema/store_desc.h"
#include "schema/index_desc.h"
#include "schema/inv_desc.h"
#include "schema/agent_desc.h"
#include "schema/container_desc.h"

extern desc* desc_ptr;
extern desc* last_desc_ptr;

#define CAST_TO_STORED_OBJECT(x) ((stored_object_desc*)x)
#define CAST_TO_PAGE_STORE(x) 	((page_store_desc*)x)

#define CAST_TO_INDEX(x) 	((index_desc*)x)
#define CAST_TO_MPHF_INDEX(x) 	((index_desc*)x)
#define CAST_TO_SMPHF_INDEX(x) 	((smphf_index_desc*)x)

#define CAST_TO_INV(x) 		((inv_desc*)x)

#define CAST_TO_MPHF(x) 	((mphf_desc*)x)
#define CAST_TO_SMPHF(x) 	((smphf_desc*)x)
#define CAST_TO_BTREE(x) 	((btree_desc*)x)

#define CAST_TO_CONTAINER(x) 	((container_desc*)x)

#undef  alloca
#define alloca(x) 	(malloc(x))

extern void  schemaerror(char*);
extern int  schemalex();

#undef  schemawrap

typedef union
{
 char   *string;
 int    integer;
 desc*  trans;
 page_store_desc*  ps_trans;
}  schemaSTYPE;
#define TOKEN 257
#define CONTAINER 258
#define SET 259
#define LIST 260
#define INDEX_NAME 261
#define INV 262
#define COMPRESS 263
#define INV_NAME 264
#define AGENT_NAME 265
#define STORE_NAME 266
#define POSITION 267
#define INDEX 268
#define MPHF_INDEX 269
#define SMPHF_INDEX 270
#define BTREE_INDEX 271
#define INDEX_AGENT 272
#define MPHF 273
#define SMPHF 274
#define BTREE 275
#define HUFFMAN 276
#define DICT 277
#define EQUAL 278
#define NUMBER 279
#define STORE 280
#define PAGE_STORE 281
#define NM 282
#define V_OID 283
#define MODE 284
#define PAGE_SZ 285
#define CACHED_PAGES 286
#undef  BYTE_ORDER
#define BYTE_ORDER 287
#define SEPARATOR 288
#define  schemaERRCODE 256
short  schemalhs[] = {                                        -1,
    0,    1,    1,    2,    2,    2,    2,    2,    2,    3,
    4,    6,    7,    5,    8,   15,   16,   16,   16,   17,
   18,   18,   19,   19,   19,   20,   20,    9,    9,   10,
   10,   12,   12,   13,   13,   11,   11,   14,   14,   21,
   21,   21,   21,   25,   25,   26,   26,   22,   22,   27,
   27,   27,   23,   23,   24,   24,
};
short  schemalen[] = {                                         2,
    1,    2,    1,    2,    2,    2,    2,    2,    2,    3,
    3,    3,    3,    3,    3,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    3,    1,    3,
    1,    3,    1,    3,    1,    3,    1,    3,    1,    1,
    3,    3,    3,    1,    3,    3,    3,    3,    1,    3,
    3,    1,    3,    1,    3,    1,
};
short  schemadefred[] = {                                      0,
    0,    0,    0,    0,    0,    0,    0,    1,    0,   26,
   27,    9,    0,   20,    6,    0,   21,   22,    7,    0,
   17,   18,   19,    5,    0,   23,   25,   24,    8,    0,
   16,    4,    0,    2,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   15,   56,    0,   54,   12,    0,
   13,    0,    0,    0,    0,   11,    0,   52,   49,   14,
    0,    0,    0,    0,    0,   10,    0,   40,   44,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   55,   53,   46,   47,
   38,   32,   34,   50,   51,   48,   30,   36,   45,   43,
   41,   42,   28,
};
short  schemadgoto[] = {                                       7,
    8,    9,   32,   24,   29,   15,   19,   12,   66,   56,
   60,   49,   51,   45,   33,   25,   16,   20,   30,   13,
   67,   57,   46,   47,   68,   48,   59,
};
short  schemasindex[] = {                                   -250,
 -206, -260, -221, -219, -254, -272,    0,    0, -250,    0,
    0,    0, -277,    0,    0, -243,    0,    0,    0, -231,
    0,    0,    0,    0, -230,    0,    0,    0,    0, -229,
    0,    0, -228,    0, -251, -249, -249, -241, -249, -247,
 -217, -216, -215, -214,    0,    0, -223,    0,    0, -222,
    0, -220, -211, -209, -208,    0, -213,    0,    0,    0,
 -212, -207, -205, -204, -201,    0, -210,    0,    0, -185,
 -178, -177, -176, -251, -249, -249, -175, -174, -232, -241,
 -249, -173, -194, -193, -170, -247,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,
};
short  schemarindex[] = {                                      0,
    0,    0,    0,    0,    0,    0,    0,    0,   28,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    1,    0,    0,    3,
    0,   14,    0,    0,    0,    0,   16,    0,    0,    0,
   27,    0,    0,    0,    0,    0,   29,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,
};
short  schemagindex[] = {                                      0,
   79,    0,    0,    0,    0,    0,    0,    0,    4,    9,
   10,   17,   18,   19,    0,    0,    0,    0,    0,    0,
    0,    0,  -32,    0,    0,  -40,    0,
};
#define  schemaTABLESIZE 309
short  schematable[] = {                                      69,
   39,   14,   33,   50,   52,   58,   61,    1,   31,   41,
   35,    2,    3,   35,   42,   31,   42,    4,   26,   27,
   28,    5,   53,   54,   42,   55,   37,    3,   29,    6,
   43,   44,   43,   44,   43,   44,   62,   63,   64,   65,
   43,   44,   50,   52,   36,   69,   96,   58,   61,   21,
   22,   23,   10,   11,   17,   18,   37,   38,   39,   40,
   70,   71,   72,   73,   74,   75,   77,   76,   78,   79,
   82,   87,   83,   84,   80,   81,   85,   86,   88,   89,
   90,   94,   95,   99,  100,  101,  102,   34,   97,  103,
   98,   92,   91,   93,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   39,    0,
   33,    0,   39,   39,   33,   33,    0,    0,   39,    0,
   33,   35,   39,   31,   33,   35,   35,   31,   31,    0,
   39,   35,   33,   31,   37,   35,   29,   31,   37,   37,
   29,   29,    0,   35,   37,   31,   29,    0,   37,    0,
   29,    0,    0,    0,    0,    0,   37,    0,   29,
};
short  schemacheck[] = {                                      40,
    0,  262,    0,   36,   37,   38,   39,  258,  281,  261,
  288,  262,  263,    0,  266,    0,  266,  268,  273,  274,
  275,  272,  264,  265,  266,  267,    0,    0,    0,  280,
  282,  283,  282,  283,  282,  283,  284,  285,  286,  287,
  282,  283,   75,   76,  288,   86,  279,   80,   81,  269,
  270,  271,  259,  260,  276,  277,  288,  288,  288,  288,
  278,  278,  278,  278,  288,  288,  278,  288,  278,  278,
  278,  257,  278,  278,  288,  288,  278,  288,  257,  257,
  257,  257,  257,  257,  279,  279,  257,    9,   80,   86,
   81,   75,   74,   76,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  258,   -1,
  258,   -1,  262,  263,  262,  263,   -1,   -1,  268,   -1,
  268,  258,  272,  258,  272,  262,  263,  262,  263,   -1,
  280,  268,  280,  268,  258,  272,  258,  272,  262,  263,
  262,  263,   -1,  280,  268,  280,  268,   -1,  272,   -1,
  272,   -1,   -1,   -1,   -1,   -1,  280,   -1,  280,
};
#define  schemaFINAL 7
#ifndef  schemaDEBUG
#define  schemaDEBUG 0
#endif
#define  schemaMAXTOKEN 288
#if  schemaDEBUG
char * schemaname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"TOKEN","CONTAINER","SET","LIST",
"INDEX_NAME","INV","COMPRESS","INV_NAME","AGENT_NAME","STORE_NAME","POSITION",
"INDEX","MPHF_INDEX","SMPHF_INDEX","BTREE_INDEX","INDEX_AGENT","MPHF","SMPHF",
"BTREE","HUFFMAN","DICT","EQUAL","NUMBER","STORE","PAGE_STORE","NM","V_OID",
"MODE","PAGE_SZ","CACHED_PAGES","BYTE_ORDER","SEPARATOR",
};
char * schemarule[] = {
"$accept : Sheet",
"Sheet : DescriptionList",
"DescriptionList : Description DescriptionList",
"DescriptionList : Description",
"Description : STORE Store_description",
"Description : INDEX Index_description",
"Description : INV Inv_description",
"Description : COMPRESS Compress_description",
"Description : INDEX_AGENT Index_Agent_description",
"Description : CONTAINER Container_description",
"Store_description : Page_Store_Head SEPARATOR Page_store_descriptions",
"Index_description : Index_Head SEPARATOR MPHF_index_descriptions",
"Inv_description : Inv_Head SEPARATOR Inv_descriptions",
"Compress_description : Compress_Head SEPARATOR Compress_descriptions",
"Index_Agent_description : Index_Agent_Head SEPARATOR Hash_descriptions",
"Container_description : Container_Head SEPARATOR Container_descriptions",
"Page_Store_Head : PAGE_STORE",
"Index_Head : MPHF_INDEX",
"Index_Head : SMPHF_INDEX",
"Index_Head : BTREE_INDEX",
"Inv_Head : INV",
"Compress_Head : HUFFMAN",
"Compress_Head : DICT",
"Index_Agent_Head : MPHF",
"Index_Agent_Head : BTREE",
"Index_Agent_Head : SMPHF",
"Container_Head : SET",
"Container_Head : LIST",
"Page_store_descriptions : Page_Store_Term SEPARATOR Page_store_descriptions",
"Page_store_descriptions : Page_Store_Term",
"MPHF_index_descriptions : MPHF_Index_Term SEPARATOR MPHF_index_descriptions",
"MPHF_index_descriptions : MPHF_Index_Term",
"Inv_descriptions : Stored_Object_Term SEPARATOR Inv_descriptions",
"Inv_descriptions : Stored_Object_Term",
"Compress_descriptions : Stored_Object_Term SEPARATOR Compress_descriptions",
"Compress_descriptions : Stored_Object_Term",
"Hash_descriptions : Stored_Object_Term SEPARATOR Hash_descriptions",
"Hash_descriptions : Stored_Object_Term",
"Container_descriptions : Container_Term SEPARATOR Container_descriptions",
"Container_descriptions : Container_Term",
"Page_Store_Term : Store_Term",
"Page_Store_Term : CACHED_PAGES EQUAL NUMBER",
"Page_Store_Term : BYTE_ORDER EQUAL TOKEN",
"Page_Store_Term : PAGE_SZ EQUAL NUMBER",
"Store_Term : Term",
"Store_Term : MODE EQUAL TOKEN",
"Term : NM EQUAL TOKEN",
"Term : V_OID EQUAL TOKEN",
"MPHF_Index_Term : POSITION EQUAL NUMBER",
"MPHF_Index_Term : Index_Term",
"Index_Term : INV_NAME EQUAL TOKEN",
"Index_Term : AGENT_NAME EQUAL TOKEN",
"Index_Term : Stored_Object_Term",
"Stored_Object_Term : STORE_NAME EQUAL TOKEN",
"Stored_Object_Term : Term",
"Container_Term : INDEX_NAME EQUAL TOKEN",
"Container_Term : Stored_Object_Term",
};
#endif
#define  schemaclearin ( schemachar=(-1))
#define  schemaerrok ( schemaerrflag=0)
#ifdef  schemaSTACKSIZE
#ifndef  schemaMAXDEPTH
#define  schemaMAXDEPTH  schemaSTACKSIZE
#endif
#else
#ifdef  schemaMAXDEPTH
#define  schemaSTACKSIZE  schemaMAXDEPTH
#else
#define  schemaSTACKSIZE 500
#define  schemaMAXDEPTH 500
#endif
#endif
int  schemadebug;
int  schemanerrs;
int  schemaerrflag;
int  schemachar;
short * schemassp;
 schemaSTYPE * schemavsp;
 schemaSTYPE  schemaval;
 schemaSTYPE  schemalval;
short  schemass[ schemaSTACKSIZE];
 schemaSTYPE  schemavs[ schemaSTACKSIZE];
#define  schemastacksize  schemaSTACKSIZE
#define  schemaABORT goto  schemaabort
#define  schemaACCEPT goto  schemaaccept
#define  schemaERROR goto  schemaerrlab
#if  schemaDEBUG
#ifndef __cplusplus
extern char *getenv(const char *);
#endif
#endif
int
 schemaparse()
{
    register int  schemam,  scheman,  schemastate;
#if  schemaDEBUG
    register char * schemas;

    if ( schemas = getenv(" schemaDEBUG"))
    {
         scheman = * schemas;
        if ( scheman >= '0' &&  scheman <= '9')
             schemadebug =  scheman - '0';
    }
#endif

     schemanerrs = 0;
     schemaerrflag = 0;
     schemachar = (-1);

     schemassp =  schemass;
     schemavsp =  schemavs;
    * schemassp =  schemastate = 0;

 schemaloop:
    if ( (scheman =  schemadefred[ schemastate])) goto  schemareduce;
    if ( schemachar < 0)
    {
        if (( schemachar =  schemalex()) < 0)  schemachar = 0;
#if  schemaDEBUG
        if ( schemadebug)
        {
             schemas = 0;
            if ( schemachar <=  schemaMAXTOKEN)  schemas =  schemaname[ schemachar];
            if (! schemas)  schemas = "illegal-symbol";
            printf(" schemadebug: state %d, reading %d (%s)\n",  schemastate,
                     schemachar,  schemas);
        }
#endif
    }
    if (( scheman =  schemasindex[ schemastate]) && ( scheman +=  schemachar) >= 0 &&
             scheman <=  schemaTABLESIZE &&  schemacheck[ scheman] ==  schemachar)
    {
#if  schemaDEBUG
        if ( schemadebug)
            printf(" schemadebug: state %d, shifting to state %d\n",
                     schemastate,  schematable[ scheman]);
#endif
        if ( schemassp >=  schemass +  schemastacksize - 1)
        {
            goto  schemaoverflow;
        }
        *++ schemassp =  schemastate =  schematable[ scheman];
        *++ schemavsp =  schemalval;
         schemachar = (-1);
        if ( schemaerrflag > 0)  -- schemaerrflag;
        goto  schemaloop;
    }
    if (( scheman =  schemarindex[ schemastate]) && ( scheman +=  schemachar) >= 0 &&
             scheman <=  schemaTABLESIZE &&  schemacheck[ scheman] ==  schemachar)
    {
         scheman =  schematable[ scheman];
        goto  schemareduce;
    }
    if ( schemaerrflag) goto  schemainrecovery;
#if 0				/* remove if needed */
#ifdef lint
    goto  schemanewerror;
#endif
 schemanewerror:
#endif
     schemaerror((char*)"syntax error");
#if 0				/* remove if needed */
#ifdef lint
    goto  schemaerrlab;
#endif
 schemaerrlab:
#endif
    ++ schemanerrs;
 schemainrecovery:
    if ( schemaerrflag < 3)
    {
         schemaerrflag = 3;
        for (;;)
        {
            if (( scheman =  schemasindex[* schemassp]) && ( scheman +=  schemaERRCODE) >= 0 &&
                     scheman <=  schemaTABLESIZE &&  schemacheck[ scheman] ==  schemaERRCODE)
            {
#if  schemaDEBUG
                if ( schemadebug)
                    printf(" schemadebug: state %d, error recovery shifting\
 to state %d\n", * schemassp,  schematable[ scheman]);
#endif
                if ( schemassp >=  schemass +  schemastacksize - 1)
                {
                    goto  schemaoverflow;
                }
                *++ schemassp =  schemastate =  schematable[ scheman];
                *++ schemavsp =  schemalval;
                goto  schemaloop;
            }
            else
            {
#if  schemaDEBUG
                if ( schemadebug)
                    printf(" schemadebug: error recovery discarding state %d\n",
                            * schemassp);
#endif
                if ( schemassp <=  schemass) goto  schemaabort;
                -- schemassp;
                -- schemavsp;
            }
        }
    }
    else
    {
        if ( schemachar == 0) goto  schemaabort;
#if  schemaDEBUG
        if ( schemadebug)
        {
             schemas = 0;
            if ( schemachar <=  schemaMAXTOKEN)  schemas =  schemaname[ schemachar];
            if (! schemas)  schemas = "illegal-symbol";
            printf(" schemadebug: state %d, error recovery discards token %d (%s)\n",
                     schemastate,  schemachar,  schemas);
        }
#endif
         schemachar = (-1);
        goto  schemaloop;
    }
 schemareduce:
#if  schemaDEBUG
    if ( schemadebug)
        printf(" schemadebug: state %d, reducing by rule %d (%s)\n",
                 schemastate,  scheman,  schemarule[ scheman]);
#endif
     schemam =  schemalen[ scheman];
     schemaval =  schemavsp[1- schemam];
    switch ( scheman)
    {
case 1:
{
           /*$1 -> asciiOutList(cerr);*/
	   desc_ptr =  schemavsp[0].trans;
	}
break;
case 2:
{
            schemavsp[-1].trans -> set_next_desc( schemavsp[0].trans);
	    schemaval.trans =  schemavsp[-1].trans;
	}
break;
case 3:
{
            schemavsp[0].trans -> set_next_desc(0);
           last_desc_ptr =  schemavsp[0].trans;
	    schemaval.trans =  schemavsp[0].trans;
	}
break;
case 4:
{
	    schemaval.trans =  schemavsp[0].trans;
	}
break;
case 5:
{
	    schemaval.trans =  schemavsp[0].trans;
	}
break;
case 6:
{
	    schemaval.trans =  schemavsp[0].trans;
	}
break;
case 7:
{
	    schemaval.trans =  schemavsp[0].trans;
	}
break;
case 8:
{
	    schemaval.trans =  schemavsp[0].trans;
	}
break;
case 9:
{
	    schemaval.trans =  schemavsp[0].trans;
	}
break;
case 10:
{
	    schemaval.trans = desc_ptr;
	}
break;
case 11:
{
	    schemaval.trans = desc_ptr;
        }
break;
case 12:
{
	    schemaval.trans = desc_ptr;
        }
break;
case 13:
{
	    schemaval.trans = desc_ptr;
        }
break;
case 14:
{
	    schemaval.trans = desc_ptr;
        }
break;
case 15:
{
	    schemaval.trans = desc_ptr;
        }
break;
case 16:
{
           desc_ptr = new page_store_desc;
	}
break;
case 17:
{
           desc_ptr= new mphf_index_desc;
        }
break;
case 18:
{
           desc_ptr= new smphf_index_desc;
        }
break;
case 19:
{
           desc_ptr= new btree_index_desc;
        }
break;
case 20:
{
           desc_ptr= new inv_desc;
        }
break;
case 21:
{
           desc_ptr= new huffman_desc;
        }
break;
case 22:
{
           desc_ptr= new dict_desc;
        }
break;
case 23:
{
           desc_ptr= new mphf_desc;
        }
break;
case 24:
{
           desc_ptr= new btree_desc;
	}
break;
case 25:
{
           desc_ptr= new smphf_desc;
	}
break;
case 26:
{
           desc_ptr= new set_desc;
        }
break;
case 27:
{
           desc_ptr= new list_desc;
	}
break;
case 28:
{
	}
break;
case 29:
{
	}
break;
case 30:
{
        }
break;
case 31:
{
        }
break;
case 32:
{
        }
break;
case 33:
{
        }
break;
case 34:
{
        }
break;
case 35:
{
        }
break;
case 36:
{
        }
break;
case 37:
{
        }
break;
case 38:
{
        }
break;
case 39:
{
        }
break;
case 40:
{
	}
break;
case 41:
{
	   CAST_TO_PAGE_STORE(desc_ptr) -> set_cached_pages( schemavsp[0].integer);
	}
break;
case 42:
{
	   CAST_TO_PAGE_STORE(desc_ptr) -> set_order( schemavsp[0].string);
	}
break;
case 43:
{
	   CAST_TO_PAGE_STORE(desc_ptr) -> set_page_sz( schemavsp[0].integer);
	}
break;
case 44:
{
	}
break;
case 45:
{
	   CAST_TO_PAGE_STORE(desc_ptr) -> set_mode( schemavsp[0].string);
	}
break;
case 46:
{
	   desc_ptr -> set_nm( schemavsp[0].string);
	}
break;
case 47:
{
	   desc_ptr -> set_oid( schemavsp[0].string);
	}
break;
case 48:
{
           CAST_TO_MPHF_INDEX(desc_ptr) -> set_position( schemavsp[0].integer);
	}
break;
case 49:
{
	}
break;
case 50:
{
	   CAST_TO_INDEX(desc_ptr) -> set_inv_nm( schemavsp[0].string);
	}
break;
case 51:
{
           CAST_TO_INDEX(desc_ptr) -> set_agent_nm( schemavsp[0].string);
	}
break;
case 52:
{
	}
break;
case 53:
{
	   CAST_TO_STORED_OBJECT(desc_ptr) -> set_store_nm( schemavsp[0].string);
	}
break;
case 54:
{
	}
break;
case 55:
{
           CAST_TO_CONTAINER(desc_ptr) -> set_index_nm( schemavsp[0].string);
	}
break;
case 56:
{
	}
break;
    }
     schemassp -=  schemam;
     schemastate = * schemassp;
     schemavsp -=  schemam;
     schemam =  schemalhs[ scheman];
    if ( schemastate == 0 &&  schemam == 0)
    {
#if  schemaDEBUG
        if ( schemadebug)
            printf(" schemadebug: after reduction, shifting from state 0 to\
 state %d\n",  schemaFINAL);
#endif
         schemastate =  schemaFINAL;
        *++ schemassp =  schemaFINAL;
        *++ schemavsp =  schemaval;
        if ( schemachar < 0)
        {
            if (( schemachar =  schemalex()) < 0)  schemachar = 0;
#if  schemaDEBUG
            if ( schemadebug)
            {
                 schemas = 0;
                if ( schemachar <=  schemaMAXTOKEN)  schemas =  schemaname[ schemachar];
                if (! schemas)  schemas = "illegal-symbol";
                printf(" schemadebug: state %d, reading %d (%s)\n",
                         schemaFINAL,  schemachar,  schemas);
            }
#endif
        }
        if ( schemachar == 0) goto  schemaaccept;
        goto  schemaloop;
    }
    if (( scheman =  schemagindex[ schemam]) && ( scheman +=  schemastate) >= 0 &&
             scheman <=  schemaTABLESIZE &&  schemacheck[ scheman] ==  schemastate)
         schemastate =  schematable[ scheman];
    else
         schemastate =  schemadgoto[ schemam];
#if  schemaDEBUG
    if ( schemadebug)
        printf(" schemadebug: after reduction, shifting from state %d \
to state %d\n", * schemassp,  schemastate);
#endif
    if ( schemassp >=  schemass +  schemastacksize - 1)
    {
        goto  schemaoverflow;
    }
    *++ schemassp =  schemastate;
    *++ schemavsp =  schemaval;
    goto  schemaloop;
 schemaoverflow:
     schemaerror((char*)"yacc stack overflow");
 schemaabort:
    return (1);
 schemaaccept:
    return (0);
}
