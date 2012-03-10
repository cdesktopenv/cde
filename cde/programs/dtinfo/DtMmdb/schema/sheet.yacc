/*
 * $XConsortium: sheet.y /main/2 1996/07/18 14:50:27 drk $
 *
 * Copyright (c) 1993 HAL Computer Systems International, Ltd.
 * All rights reserved.  Unpublished -- rights reserved under
 * the Copyright Laws of the United States.  USE OF A COPYRIGHT
 * NOTICE IS PRECAUTIONARY ONLY AND DOES NOT IMPLY PUBLICATION
 * OR DISCLOSURE.
 * 
 * THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE
 * SECRETS OF HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.  USE,
 * DISCLOSURE, OR REPRODUCTION IS PROHIBITED WITHOUT THE
 * PRIOR EXPRESS WRITTEN PERMISSION OF HAL COMPUTER SYSTEMS
 * INTERNATIONAL, LTD.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject
 * to the restrictions as set forth in subparagraph (c)(l)(ii)
 * of the Rights in Technical Data and Computer Software clause
 * at DFARS 252.227-7013.
 *
 *          HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.
 *                  1315 Dell Avenue
 *                  Campbell, CA  95008
 * 
 */

%{

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

#define alloca(x) 	(malloc(x))

extern void yyerror(char*);
extern int yylex();

#undef yywrap

%}

%union
{
 char   *string;
 int    integer;
 desc*  trans;
 page_store_desc*  ps_trans;
}

%token <string>
 TOKEN
%token <integer>
 CONTAINER
 SET
 LIST
 INDEX_NAME
 INV
 COMPRESS
 INV_NAME
 AGENT_NAME
 STORE_NAME
 POSITION 
 INDEX 
 MPHF_INDEX
 SMPHF_INDEX
 BTREE_INDEX
 INDEX_AGENT 
 MPHF
 SMPHF
 BTREE
 HUFFMAN 
 DICT 
 EQUAL 
 NUMBER
 STORE
 PAGE_STORE
 NM
 V_OID
 MODE
 PAGE_SZ
 CACHED_PAGES
 BYTE_ORDER
 SEPARATOR
%type <integer>

%type <trans>
 Sheet
 DescriptionList
 Description

 Store_description 
 Index_description 
 Index_Agent_description
 Inv_description
 Compress_description
 Container_description

%type <ps_trans>
 Page_store_descriptions
 MPHF_index_descriptions
 Hash_descriptions
 Inv_descriptions
 Compress_descriptions
 Container_descriptions 

%start Sheet

%%

Sheet : DescriptionList {
           //$1 -> asciiOutList(cerr);
	   desc_ptr = $1;
	}

DescriptionList : Description DescriptionList {
           $1 -> set_next_desc($2);
	   $$ = $1;
	}
 	| Description
	{
           $1 -> set_next_desc(0);
           last_desc_ptr = $1;
	   $$ = $1;
	}

Description : STORE Store_description 
	{
	   $$ = $2;
	} 
	| INDEX Index_description
	{
	   $$ = $2;
	}
	| INV Inv_description
	{
	   $$ = $2;
	}
	| COMPRESS Compress_description
	{
	   $$ = $2;
	}
	| INDEX_AGENT Index_Agent_description
	{
	   $$ = $2;
	}
	| CONTAINER Container_description
	{
	   $$ = $2;
	}

Store_description : Page_Store_Head SEPARATOR Page_store_descriptions
	{
	   $$ = desc_ptr;
	}

Index_description : Index_Head SEPARATOR MPHF_index_descriptions
        {
	   $$ = desc_ptr;
        }

Inv_description : Inv_Head SEPARATOR Inv_descriptions
        {
	   $$ = desc_ptr;
        }

Compress_description : Compress_Head SEPARATOR Compress_descriptions
        {
	   $$ = desc_ptr;
        }

Index_Agent_description : Index_Agent_Head SEPARATOR Hash_descriptions
        {
	   $$ = desc_ptr;
        }

Container_description : Container_Head SEPARATOR Container_descriptions
        {
	   $$ = desc_ptr;
        }

Page_Store_Head : PAGE_STORE
	{
           desc_ptr = new page_store_desc;
	}

Index_Head: MPHF_INDEX
        {
           desc_ptr= new mphf_index_desc;
        }
	| SMPHF_INDEX
        {
           desc_ptr= new smphf_index_desc;
        }
	| BTREE_INDEX
        {
           desc_ptr= new btree_index_desc;
        }

Inv_Head: INV
        {
           desc_ptr= new inv_desc;
        }

Compress_Head: HUFFMAN
        {
           desc_ptr= new huffman_desc;
        }

Compress_Head: DICT
        {
           desc_ptr= new dict_desc;
        }

Index_Agent_Head: MPHF
        {
           desc_ptr= new mphf_desc;
        }
	| BTREE
	{
           desc_ptr= new btree_desc;
	}
	| SMPHF
	{
           desc_ptr= new smphf_desc;
	}

Container_Head: SET
        {
           desc_ptr= new set_desc;
        }
	| LIST
	{
           desc_ptr= new list_desc;
	}

Page_store_descriptions : Page_Store_Term SEPARATOR Page_store_descriptions
	{
	}
	| Page_Store_Term
	{
	}

MPHF_index_descriptions: MPHF_Index_Term SEPARATOR MPHF_index_descriptions
        {
        }
        | MPHF_Index_Term
        {
        }

Inv_descriptions: Stored_Object_Term SEPARATOR Inv_descriptions
        {
        }
        | Stored_Object_Term 
        {
        }

Compress_descriptions: Stored_Object_Term SEPARATOR Compress_descriptions
        {
        }
        | Stored_Object_Term 
        {
        }

Hash_descriptions: Stored_Object_Term SEPARATOR Hash_descriptions
        {
        }
        | Stored_Object_Term 
        {
        }

Container_descriptions: Container_Term SEPARATOR Container_descriptions
        {
        }
        | Container_Term 
        {
        }

Page_Store_Term : Store_Term
	{
	}
	| CACHED_PAGES EQUAL NUMBER 
	{
	   CAST_TO_PAGE_STORE(desc_ptr) -> set_cached_pages($3);
	}
	| BYTE_ORDER EQUAL TOKEN 
	{
	   CAST_TO_PAGE_STORE(desc_ptr) -> set_order($3);
	}
	| PAGE_SZ EQUAL NUMBER 
	{
	   CAST_TO_PAGE_STORE(desc_ptr) -> set_page_sz($3);
	}

Store_Term : Term
	{
	}
	| MODE EQUAL TOKEN  
	{
	   CAST_TO_PAGE_STORE(desc_ptr) -> set_mode($3);
	}

Term: NM EQUAL TOKEN
	{
	   desc_ptr -> set_nm($3);
	}
	| V_OID EQUAL TOKEN
	{
	   desc_ptr -> set_oid($3);
	}

MPHF_Index_Term : POSITION EQUAL NUMBER
	{
           CAST_TO_MPHF_INDEX(desc_ptr) -> set_position($3);
	}
	| Index_Term
	{
	}

Index_Term : INV_NAME EQUAL TOKEN
	{
	   CAST_TO_INDEX(desc_ptr) -> set_inv_nm($3);
	}
	| AGENT_NAME EQUAL TOKEN
	{
           CAST_TO_INDEX(desc_ptr) -> set_agent_nm($3);
	}
	| Stored_Object_Term
	{
	}

Stored_Object_Term : STORE_NAME EQUAL TOKEN
	{
	   CAST_TO_STORED_OBJECT(desc_ptr) -> set_store_nm($3);
	} 
	| Term
	{
	}

Container_Term: INDEX_NAME EQUAL TOKEN
	{
           CAST_TO_CONTAINER(desc_ptr) -> set_index_nm($3);
	}
	| Stored_Object_Term
	{
	}
%%
