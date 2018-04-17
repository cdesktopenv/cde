/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: mp_types_gram.y /main/3 1995/10/20 16:38:43 rswiston $ 			 				 */
%{

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

%}

%start	types

%union	{
	int		num;
	char		*str;
	}

%type <str> 	prop_id  value string_lit

	/* reserved tokens: ptype => PTYPE, ... */

%token	<str>  PER_FILE PER_SESSION START FILE_SCOPE SESSION_SCOPE
               FILE_IN_SESSION REQUEST NOTIFICATION
	       VOID_ARGS
%token	<num>  QUEUE OPNUM
%token	IN OUT INOUT OTYPE INHERIT FROM PTYPE
        OBSERVE HANDLE HANDLE_PUSH HANDLE_ROTATE
        COLON SEMICOLON LCURL RCURL INFER LPAREN RPAREN COMMA EQUAL CONTEXT

	/* variable tokens */

%token  <str>  TT_IDENTIFIER  TT_STRING  TT_NUMBER

%%

/* Grammar rules for otype & ptype definition languages */

types		:	type
		|	types  type
		;

type		:	otype
                |	ptype
                ;

/* OTYPE grammar rules */

/* 
 * This is the grammar definition:
 * 
 * otype	::=  obj_header '{' objbody* '}' [';']
 * 
 * obj_header	::=  'otype' otid [':' otid+]
 * 
 * objbody	::=  'observe:' osignature* | 'handle:' osignature*
 * 
 * osignature	::=  op args [cxtdcl] '=>' rhs ';'
 * 
 * rhs		::=  ptid [scope] ['start'] ['queue'] ['opnum=' number]
 * 		     [inherit] | inherit
 * 
 * inherit	::=  'from' otid
 */

otype		:	header  LCURL  otype_body_list  RCURL  opt_del
			{
			  insert_otype(tmp_otype);
			  tmp_otype = new _Tt_otype();
			}
                ;

header		:	OTYPE  otid { tmp_otype->set_otid(tmp_otid); }
                        inheritance_opt
                        {
				tmp_otype->set_ancestors(tmp_otidl);
				tmp_otidl = new _Tt_string_list();
		        }
                ;

otid		:	TT_IDENTIFIER
			{
				tmp_otid = yytext;
			}
                ;

inheritance_opt	:	/* empty */
                |	COLON  otid_list
                ;

otid_list	:	otid
                        {
			  tmp_otidl->append(tmp_otid);
		        }
                |	otid_list  otid
                        {
			  tmp_otidl->append(tmp_otid);
		        }
                ;

otype_body_list	:	/* empty */
		|	otype_body_list otype_body
		;

otype_body	:	otype_observe
		|	otype_handle
		|	otype_handle_push
		|	otype_handle_rotate
                ;

otype_observe	:	OBSERVE  COLON  osig_list
                        {
			  tmp_otype->append_osigs(tmp_sigl);
			  tmp_sigl = new _Tt_signature_list();
		        }
		;

otype_handle	:	HANDLE  COLON  osig_list
                        {
			  tmp_otype->append_hsigs(tmp_sigl, TT_HANDLE);
			  tmp_sigl = new _Tt_signature_list();
		        }
                ;

otype_handle_push:	HANDLE_PUSH  COLON  osig_list
                        {
			  tmp_otype->append_hsigs(tmp_sigl, TT_HANDLE_PUSH);
			  tmp_sigl = new _Tt_signature_list();
		        }
                ;

otype_handle_rotate:	HANDLE_ROTATE  COLON  osig_list
                        {
			  tmp_otype->append_hsigs(tmp_sigl, TT_HANDLE_ROTATE);
			  tmp_sigl = new _Tt_signature_list();
		        }
                ;

osig_list	:	/* empty */
                |	osig_list  osig
                ;

osig		:	op args cntxt_opt INFER rhs SEMICOLON
			{
			  tmp_sig->set_otid(tmp_otype->otid());
			  tmp_sig->set_super_otid(tmp_otid);
			  append_sig(tmp_sig);
			  tmp_sig = new _Tt_signature();
			}
		;

rhs		:	ptid  { tmp_sig->set_ptid(yytext); } 
                        scope_opt  start_opt  queue_opt  opnum_opt
			inherit_opt
		|	inherit
                ;

inherit_opt	:	/* empty */
		|	inherit
		;

inherit		:	FROM otid
			{
			  tmp_sig->set_super_otid(tmp_otid);
			}
		;

/* PTYPE grammar rules */

/* 
 * The grammar definition:
 * 
 * ptype	::=  'ptype' ptid '{' property* ptype_body* '}' [';']
 *
 * property	::=  property_id value ';'
 * 
 * ptype_body	::=  'observe:' psignature* | 'handle:' psignature*
 * 
 * ptype_body	::=  'handle_push:' psignature* | 'handle_rotate:' psignature*
 * 
 * property_id	::=  'per_file' | 'per_session' | 'start'
 * 
 * value	::=  string | number
 * 
 * psignature	::=  [scope] op args [cxtdcl]  ['=>' ['start'] ['queue']
 * 				      ['opnum=' number]] ';'
 * 
 * scope	::=  'file' | 'session' | 'file_in_session'
 * 
 * args		::=  '(' argspec {, argspec}* ')' | '(void)' | '()'
 *
 * cxtdcl	::=  'context' '(' identifier {, identifier }* ')'
 * 
 * argspec	::=  mode type name
 * 
 * mode		::=  'in' | 'out' | 'inout'
 */

ptype		:	PTYPE  ptid  { tmp_ptype->set_ptid(yytext); }
                        LCURL  prop_list ptype_body_list  RCURL  opt_del
			{
			  insert_ptype(tmp_ptype);
			  tmp_ptype = new _Tt_ptype();
			}
		;

ptid		:	TT_IDENTIFIER
		;

prop_list	:	/* empty */
		|	prop_list prop
		;

prop		:	prop_id value SEMICOLON
			{
			  tmp_ptype->appendprop(tmp_propname, tmp_propvalue);
			  tmp_propname = tmp_propvalue = 0;
			}
		;

ptype_body_list	:	/* empty */
		|	ptype_body_list ptype_body
		;

ptype_body	:	observe
		|	handle
		|	handle_push
		|	handle_rotate
		;

prop_id		:	PER_FILE
{ set_tmp_propname(_Tt_string((const unsigned char *)yytext, yyleng)); }
		|	PER_SESSION
{ set_tmp_propname(_Tt_string((const unsigned char *)yytext, yyleng));}
		|	START
{ set_tmp_propname(_Tt_string((const unsigned char *)yytext,yyleng)); }
		;

/* Rule 10 */

string_lit	:	TT_STRING
                        {
			  tmp_propvalue.set((const unsigned char *)yytext+1,
					    yyleng - 2);
                        }
		|	string_lit TT_STRING
                        {
			  _Tt_string stringfrag;
			  stringfrag.set((const unsigned char *)yytext+1,
					 yyleng - 2);
			  tmp_propvalue = tmp_propvalue.cat(stringfrag);
		        }
		;

value		:	string_lit
		|	TT_NUMBER
			{
			  tmp_propvalue = yytext;
			}
		;

observe		:	OBSERVE  COLON  psig_list
			{
			  tmp_ptype->append_osigs(tmp_sigl);
			  tmp_sigl = new _Tt_signature_list();
			}
		;

handle		:	HANDLE  COLON  psig_list
			{
			  tmp_ptype->append_hsigs(tmp_sigl, TT_HANDLE);
			  tmp_sigl = new _Tt_signature_list();
			}
		;

handle_push	:	HANDLE_PUSH  COLON  psig_list
			{
			  tmp_ptype->append_hsigs(tmp_sigl, TT_HANDLE_PUSH);
			  tmp_sigl = new _Tt_signature_list();
			}
		;

handle_rotate	:	HANDLE_ROTATE  COLON  psig_list
			{
			  tmp_ptype->append_hsigs(tmp_sigl, TT_HANDLE_ROTATE);
			  tmp_sigl = new _Tt_signature_list();
			}
		;

opt_del		:	/* empty */
		|	SEMICOLON
		;

psig_list	:	/* empty */
		|	psig_list  psig
		;

/* Rule 20 */

psig		:	scope_opt  op  args  cntxt_opt infer_opt  SEMICOLON
			{
				tmp_sig->set_ptid(tmp_ptype->ptid());
				append_sig(tmp_sig);
				tmp_sig = new _Tt_signature();
			}
		;

scope_opt	:	/* empty */
		|	FILE_SCOPE {
			tmp_sig->set_scope(TT_FILE);
		}
		|	SESSION_SCOPE {
			tmp_sig->set_scope(TT_SESSION);
		}
                |	FILE_IN_SESSION {
			tmp_sig->set_scope(TT_FILE_IN_SESSION);
		}
		;

op		:	TT_IDENTIFIER
			{
			  tmp_sig->set_op(yytext);
			}
		;

args		:	LPAREN  args_aux  RPAREN
		;

args_aux	:	/* empty */
		|	VOID_ARGS
/*
			{
			  tmp_sig->append_arg(new _Tt_arg(yytext));
			}
*/
		|	argspecs
		;

/* Rule 31 */

argspecs	:	argspecs  COMMA  argspec
		|	argspec
		;

argspec		:	mode  type  name
			{
			  tmp_sig->append_arg(tmp_arg);
			  tmp_arg = new _Tt_arg();
			}
		;

mode		:	IN	{ tmp_arg->set_mode(TT_IN); }
		|	OUT	{ tmp_arg->set_mode(TT_OUT); }
		|	INOUT	{ tmp_arg->set_mode(TT_INOUT); }
		;

type		:	TT_IDENTIFIER
			{
			  _Tt_string tmptext(yytext);
			  tmp_arg->set_type(tmptext);
			}
		;

name		:	TT_IDENTIFIER
			{
			  _Tt_string tmptext(yytext);
			  tmp_arg->set_name(tmptext);
			}
		;

cntxt_opt	:	/* empty */
                |	CONTEXT LPAREN cntxt_list RPAREN
                ;

cntxt_list	:	cntxt_list COMMA  cntxt
		|	cntxt
		;

cntxt		:	TT_IDENTIFIER
			{
			  _Tt_context_ptr tmpcntxt = new _Tt_context;
			  tmpcntxt->setName(yytext);
			  tmp_sig->append_context(tmpcntxt);
			}
		;


	

infer_opt	:	/* empty */
                |	INFER  start_opt  queue_opt  opnum_opt
                ;

start_opt	:	/* empty */
		|	START		{ tmp_sig->set_reliability(TT_START); }
		;

queue_opt	:	/* empty */
		|	QUEUE		{ tmp_sig->set_reliability(TT_QUEUE); }
		;

/* Rule 41 */

opnum_opt	:	/* empty */
		|	OPNUM  EQUAL  TT_NUMBER	{ tmp_sig->set_opnum(atoi(yytext)); }
		;

%%
