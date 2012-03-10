/* 
 *  @OSF_COPYRIGHT@
 *  COPYRIGHT NOTICE
 *  Copyright (c) 1990, 1991, 1992, 1993 Open Software Foundation, Inc.
 *  ALL RIGHTS RESERVED (MOTIF). See the file named COPYRIGHT.MOTIF for
 *  the full copyright text.
*/ 
/* 
 * HISTORY
*/ 
/* "$TOG: Uil.y /main/11 1998/01/23 15:05:38 mgreess $" */
/*
*  (c) Copyright 1989, 1990, DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASS. */

/*
**++
**  FACILITY:
**
**      DECwindows Toolkit User Interface Language Compiler (UIL)
**
**  ABSTRACT:
**
**      YACC Grammar for UIL
**--
**/

%{


	/* Begin user declarations section */

#include "UilDefI.h"
#include "UilCompGl.h"

#define		YYSTYPE		yystype

#define		YYDEBUG		1

/*   Declare and initialize stack entry for epsilon productions.    */

YYSTYPE		gz_yynullval = {NULL,0,0,0,0};

/*   Declare token to aid in saving source context	*/

YYSTYPE		prev_yylval;

	/* End user declarations section */

%}    

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *     token name			token value	  token class	      *
 *     ----------                       -----------       -----------         *
 *----------------------------------------------------------------------------*
 */

/* 
**	terminals of the language
**
**	NOTE: the tags which appear in comments at the end of each token
**	definition name the token class for the token. These name the
**	the .dat file processed by WML while constructing token tables.
**	Thus the output of WML and these tags must match, e.g. for
**	the token tag 'special' the file 'special.dat' must exist.
*/

/*    end of file indicator    */

%token	UILEOF					0	/* unused */

/*    names    */

%token	NAME					1	/* identifier */
%token	FONT_NAME				2	/* font */
%token	COLOR_NAME				3	/* color */
%token	ARGUMENT_NAME				4	/* argument */
%token	REASON_NAME				5	/* reason */
%token	CHARSET_NAME				6	/* charset */
%token	ENUMVAL_NAME				7	/* enumval */
%token	CLASS_NAME				8	/* class */
%token  CHILD_NAME				96	/* child */

/*    Literals    */

%token	UNS_FLOAT_LITERAL			9	/* literal */
%token	COMP_STRING				10	/* literal */
%token	CHAR_8_LITERAL				11	/* literal */
%token	UNS_INT_LITERAL				12	/* literal */

/*    Special characters.  Treat these as tokens rather than literals, so
 *    that we can assign token numbers to them rather than have YACC use
 *    the ASCII value as the token number.
 */

%token	LEFT_PAREN				13	/* special */
%token	RIGHT_PAREN				14	/* special */
%token	COLON					15	/* special */
%token	SEMICOLON				16	/* special */
%token	LEFT_BRACE				17	/* special */
%token	RIGHT_BRACE				18	/* special */
%token	COMMA					19	/* special */
%token	EQUAL_SIGN				20	/* special */
%token	NOT					21	/* special */
%token	PLUS					22	/* special */
%token	MINUS					23	/* special */
%token	AND					24	/* special */
%token	OR					25	/* special */
%token	XOR					26	/* special */
%token	MULTIPLY				27	/* special */
%token	DIVIDE					28	/* special */
%token	LEFT_SHIFT				29	/* special */
%token	RIGHT_SHIFT				30	/* special */

/*    Reserved words	*/

%token	LIST					31	/* reserved */
%token	IDENTIFIER				32	/* reserved */
%token	END					33	/* reserved */
%token	MODULE					34	/* reserved */
%token	UILTRUE					35	/* reserved */
%token	UILFALSE				36	/* reserved */
%token	INCLUDE					37	/* reserved */
%token	MACRO					38	/* reserved */
%token	ON					39	/* reserved */
%token	OFF					40	/* reserved */
%token	VALUE					41	/* reserved */
%token	ARGUMENTS				42	/* reserved */
%token	CALLBACKS				43	/* reserved */
%token	PROCEDURES              		44	/* reserved */
%token	CONTROLS				45	/* reserved */
%token	PROCEDURE				46	/* reserved */
%token	OBJECT					47	/* reserved */

/*    Declaratives    */

%token	OBJECTS					48	/* keyword */
%token	WIDGET					49	/* reserved */
%token	GADGET					50	/* reserved */

%token	FONT					51	/* keyword */
%token	ARGUMENT				52	/* keyword */
%token	REASON					53	/* keyword */
%token	PIXMAP					54	/* keyword */
%token	COLOR					55	/* keyword */

%token	NAMES					56	/* keyword */
%token	CHARACTER_SET				57	/* keyword */
%token	CASE_SENSITIVE				58	/* keyword */
%token	CASE_INSENSITIVE			59	/* keyword */

%token	VERSION					60	/* keyword */

%token	MANAGED					61	/* keyword */
%token	UNMANAGED				62	/* keyword */

%token	PRIVATE					63	/* reserved */
%token	IMPORTED				64	/* keyword */
%token	EXPORTED				65	/* reserved */

%token	UILFILE					66	/* keyword */

%token	STRING_TABLE				67	/* keyword */
%token	TRANSLATION_TABLE			68	/* keyword */
%token	COMPOUND_STRING				69	/* keyword */
%token	FONT_TABLE				70	/* keyword */

%token	ANY					71	/* keyword */
%token	STRING					72	/* keyword */
%token	BOOLEAN					73	/* keyword */
%token	ICON					74	/* keyword */
%token	RIGHT_TO_LEFT				75	/* keyword */
%token	BACKGROUND				76	/* keyword */
%token	FOREGROUND				77	/* keyword */
%token	COLOR_TABLE				78	/* keyword */
%token	FLOAT					79	/* keyword */
%token	INTEGER					80	/* keyword */
%token  CLASS_REC_NAME				81	/* keyword */
%token	ASCIZ_TABLE				82	/* keyword */
%token	INTEGER_TABLE				83	/* keyword */
%token	ASCIZ_STRING_TABLE			84	/* keyword */
%token	COMPOUND_STRING_TABLE			85	/* keyword */
%token	XBITMAPFILE				86	/* keyword */
%token	SEPARATE				87	/* keyword */
%token  SIXTEEN_BIT				88	/* keyword */
%token	POUND					89	/* special */
%token	KEYSYM					90	/* keyword */
%token	SINGLE_FLOAT				91	/* keyword */
%token	RGB					92	/* keyword */
%token  WIDE_CHARACTER				93	/* keyword */
%token  LOC_STRING				94	/* literal */ /* New for 1.2 */
%token	FONTSET					95	/* keyword */
%token  COMPOUND_STRING_COMPONENT		97	/* keyword */

%token  PIX					115
%token  PIXEL					103
%token  PIXELS					104
%token  IN					98
%token  INCH					105
%token  INCHES					106
%token  CM					99
%token  CENTIMETER				107
%token  CENTIMETERS				108
%token  MM					100
%token  MILLIMETER				109
%token  MILLIMETERS				110
%token  PT					101
%token  POINT					111
%token  POINTS					112
%token  FU					102
%token  FONT_UNIT				113
%token  FONT_UNITS				114

%start module_block

%%



/* 
**	productions of the grammar
*/


/*	module structure	*/

module_block
    : module_header
	{ sar_save_module_source (); }
      module_declaration_list END MODULE SEMICOLON
    ;

module_header
    : /* empty */				{ sar_create_root ( &$$ ); }
      MODULE id 				{ sar_create_module( &$$, &$3, &$2 ); }
	opt_module_clauses
    ;

opt_module_clauses
    : /* empty */
    | module_clauses
    ;

module_clauses
    : module_clause
    | module_clauses module_clause
    ;
    
module_clause
    : VERSION EQUAL_SIGN character_8_value		{ sar_process_module_version(  &$3 , &$1 ); }
    | NAMES EQUAL_SIGN sensitivity_value		{ sar_process_module_sensitivity( &$3 , &$1 ); }
    | CHARACTER_SET EQUAL_SIGN valid_charset		{ sar_process_module_charset( &$3 , &$1 ); }
    | OBJECTS { sar_make_def_obj (&$1); } EQUAL_SIGN variant_spec
    ;

sensitivity_value
    : CASE_SENSITIVE
    | CASE_INSENSITIVE
    ;

variant_spec
    : LEFT_BRACE RIGHT_BRACE
    | LEFT_BRACE variant_list RIGHT_BRACE
    | error { lex_issue_error (RIGHT_BRACE); } RIGHT_BRACE
    ;

variant_list
    : variant_clause SEMICOLON
    | variant_list variant_clause SEMICOLON
    | error { lex_issue_error (SEMICOLON); } SEMICOLON
    ;

variant_clause
    : CLASS_NAME { sar_make_def_obj (&$1); }
      EQUAL_SIGN variant				{ sar_process_module_variant (&$1, &$4); $$ = gz_yynullval; }
    ;

variant
    : WIDGET						{ $$ = gz_yynullval; $$.b_type = sym_k_widget_entry; }
    | GADGET						{ $$ = gz_yynullval; $$.b_type = sym_k_gadget_entry; }
    ;

module_declaration_list
    : /* empty */
    | module_declaration_list module_declaration
    ;

module_declaration
    : value_declaration
    | identifier_declaration
    | procedure_declaration
    | object_declaration
    | list_declaration
    | include_directive
    ;



/*	VALUE declaration	    */

value_declaration
    : VALUE { $$ = $1; sar_save_section_source (&$1, sym_k_value_section); } value_declaration_list
    ;

value_declaration_list
    : value_decl
    | value_declaration_list value_decl
    | error { lex_issue_error (SEMICOLON); } SEMICOLON
    ;

value_decl
    : id COLON value_definition SEMICOLON		{ sar_bind_value_name( &$1, &$3, &$4 ); }
    ;

value_definition
  : EXPORTED private_value				{ $$ = $2; $$.b_flags |= sym_m_exported; $$.b_flags &= ~sym_m_private; }
    | PRIVATE private_value_plus			{ $$ = $2; $$.b_flags |= sym_m_private; $$.b_flags &= ~sym_m_exported; }
    | private_value_plus				{ $$ = $1; $$.b_flags |= sym_m_private; $$.b_flags &= ~sym_m_exported; }
    | IMPORTED value_type				{ sar_import_value_entry( &$$, &$2 ); }
    ;

value_type
    : INTEGER			{ $$ = $1; $$.b_type = sym_k_integer_value; }
    | STRING			{ $$ = $1; $$.b_type = sym_k_char_8_value; }
    | PIXMAP			{ $$ = $1; $$.b_type = sym_k_pixmap_value; }
    | FLOAT			{ $$ = $1; $$.b_type = sym_k_float_value; }
    | BOOLEAN			{ $$ = $1; $$.b_type = sym_k_bool_value; }
    | FONT			{ $$ = $1; $$.b_type = sym_k_font_value; }
    | REASON			{ $$ = $1; $$.b_type = sym_k_reason_value; }
    | ARGUMENT			{ $$ = $1; $$.b_type = sym_k_argument_value; }
    | COLOR			{ $$ = $1; $$.b_type = sym_k_color_value; }
    | string_table_type		{ $$ = $1; $$.b_type = sym_k_string_table_value; }
    | asciz_table_type		{ $$ = $1; $$.b_type = sym_k_asciz_table_value; }
    | INTEGER_TABLE		{ $$ = $1; $$.b_type = sym_k_integer_table_value; }
    | TRANSLATION_TABLE		{ $$ = $1; $$.b_type = sym_k_trans_table_value; }
    | FONT_TABLE		{ $$ = $1; $$.b_type = sym_k_font_table_value; }
    | ICON			{ $$ = $1; $$.b_type = sym_k_icon_value; }
    | COMPOUND_STRING		{ $$ = $1; $$.b_type = sym_k_compound_string_value; }
    | COMPOUND_STRING_COMPONENT	{ $$ = $1; $$.b_type = sym_k_compound_string_value; }
    | CLASS_REC_NAME		{ $$ = $1; $$.b_type = sym_k_class_rec_name_value; }
    | XBITMAPFILE		{ $$ = $1; $$.b_type = sym_k_xbitmapfile_value; }
    | KEYSYM			{ $$ = $1; $$.b_type = sym_k_keysym_value; }
    | SINGLE_FLOAT		{ $$ = $1; $$.b_type = sym_k_single_float_value; }
    | RGB			{ $$ = $1; $$.b_type = sym_k_rgb_value; }
    | WIDE_CHARACTER		{ $$ = $1; $$.b_type = sym_k_wchar_string_value; }
    | FONTSET			{ $$ = $1; $$.b_type = sym_k_fontset_value; }
    | WIDGET			{ $$ = $1; $$.b_type = sym_k_widget_ref_value; }
    ;

/* Seperately defined for fixing CR 5888 */
arg_value_type
    : INTEGER			{ $$ = $1; $$.b_type = sym_k_integer_value; }
    | STRING			{ $$ = $1; $$.b_type = sym_k_char_8_value; }
    | PIXMAP			{ $$ = $1; $$.b_type = sym_k_pixmap_value; }
    | FLOAT			{ $$ = $1; $$.b_type = sym_k_float_value; }
    | BOOLEAN			{ $$ = $1; $$.b_type = sym_k_bool_value; }
    | FONT			{ $$ = $1; $$.b_type = sym_k_font_value; }
    | REASON			{ $$ = $1; $$.b_type = sym_k_reason_value; }
    | COLOR			{ $$ = $1; $$.b_type = sym_k_color_value; }
    | string_table_type		{ $$ = $1; $$.b_type = sym_k_string_table_value; }
    | asciz_table_type		{ $$ = $1; $$.b_type = sym_k_asciz_table_value; }
    | INTEGER_TABLE		{ $$ = $1; $$.b_type = sym_k_integer_table_value; }
    | TRANSLATION_TABLE		{ $$ = $1; $$.b_type = sym_k_trans_table_value; }
    | FONT_TABLE		{ $$ = $1; $$.b_type = sym_k_font_table_value; }
    | ICON			{ $$ = $1; $$.b_type = sym_k_icon_value; }
    | COMPOUND_STRING		{ $$ = $1; $$.b_type = sym_k_compound_string_value; }
    | KEYSYM			{ $$ = $1; $$.b_type = sym_k_keysym_value; }
    | SINGLE_FLOAT		{ $$ = $1; $$.b_type = sym_k_single_float_value; }
    | WIDE_CHARACTER		{ $$ = $1; $$.b_type = sym_k_wchar_string_value; }
    | FONTSET			{ $$ = $1; $$.b_type = sym_k_fontset_value; }
    | WIDGET			{ $$ = $1; $$.b_type = sym_k_widget_ref_value; }
/* End for fixing CR 5888 */

string_table_type
    : STRING_TABLE
    | COMPOUND_STRING_TABLE
    ;

asciz_table_type
    : ASCIZ_TABLE
    | ASCIZ_STRING_TABLE
    ;


/*	PROCEDURE declaration	    */

procedure_declaration
    : PROCEDURE { $$ = $1; sar_save_section_source (&$1, sym_k_procedure_section); } procedure_declaration_list
    ;

procedure_declaration_list
    : procedure_decl
    | procedure_declaration_list procedure_decl
    | error { lex_issue_error (SEMICOLON); } SEMICOLON
    ;

procedure_decl
    : id opt_formal_parameters opt_class SEMICOLON	{ sar_create_procedure( &$1, &$2, &$3, &$4 );}
    ;

opt_formal_parameters
    : /* empty */					{ $$ = gz_yynullval; }
    | LEFT_PAREN formal_parameter_type RIGHT_PAREN	{ $$ = $2; }
    ;

formal_parameter_type
    : /* empty */					{ $$ = gz_yynullval; $$.b_type = sym_k_no_value; }
    | value_type
    | ANY 						{ $$ = $1; $$.b_type = 
sym_k_any_value; }
    | CLASS_NAME
	{ YYSTYPE temp[2];
	  temp[0] = gz_yynullval;
	  sar_create_object(&temp[0], sym_k_widget_entry);
	  temp[1] = $1; sar_set_object_class(&temp[1]);
	  sar_verify_object(&temp[1]);
	  $$ = temp[0];
	}
    ;

opt_class
    : /* empty */					{ $$.b_flags = sym_m_private; }
/*
    | EXPORTED						{ $$.b_flags = sym_m_exported; }
    | PRIVATE						{ $$.b_flags = sym_m_private; }
    | IMPORTED						{ $$.b_flags = sym_m_imported; }
*/
    ;



/*	IDENTIFIER declaration	    */

identifier_declaration
    : IDENTIFIER { $$ = $1; sar_save_section_source (&$1, sym_k_identifier_section); } identifier_declaration_list
    ;

identifier_declaration_list
    : identifier_decl
    | identifier_declaration_list identifier_decl
    | error { lex_issue_error (SEMICOLON); } SEMICOLON
    ;

identifier_decl
    : id SEMICOLON					{ sar_create_identifier ( &$1, &$2 ); }
    ;



/*    Directives.    */

/*    Include Directive    */

include_directive
    : INCLUDE UILFILE character_8_value SEMICOLON	{ sar_include_file (&$3, &$1, &$4); }
    | error { lex_issue_error (SEMICOLON); } SEMICOLON
    ;



/*	OBJECT declaration	*/

object_declaration
    : OBJECT { $$ = $1; sar_save_section_source (&$1, sym_k_object_section); } object_decl_list
    ;

object_decl_list
    : object_decl SEMICOLON 				{ $$ = $1; sar_save_src_semicolon_pos (&$2 ); }
    | object_decl_list object_decl SEMICOLON  		{ $$ = $1; sar_save_src_semicolon_pos (&$3 ); }
    | error { lex_issue_error (SEMICOLON); } SEMICOLON
    ;

object_decl
    : id
	{ sar_create_object (&$1, sym_k_widget_entry); 
	  sar_link_section (&$1); $$ = gz_yynullval;}
      COLON object_definition
	{ $$ = gz_yynullval; }
    ;

object_definition
    : EXPORTED
        { sar_set_object_flags (&$1, sym_m_exported); }
      object_specification
    | PRIVATE
        { sar_set_object_flags (&$1, sym_m_private); }
      object_specification
    | epsilon_production
        { sar_set_object_flags (&$1, sym_m_exported); }
      object_specification
    | IMPORTED
        { sar_set_object_flags (&$1, sym_m_imported); }
      CLASS_NAME
        { sar_set_object_class (&$3); }
/*
 * Fix for CR 5440 - Call opt_create_proc_ref to check for creation procedures
 *                   for imported user_defined widgets.
 */
      opt_create_proc_ref
/*
 * End fix for CR 5440
 */
      epsilon_production
        { sar_verify_object (&$1); }
    ;

control_object_definition
    : EXPORTED
	{ sar_set_object_flags (&$1, sym_m_exported); }
      object_specification
    | PRIVATE
	{ sar_set_object_flags (&$1, sym_m_private); }
      object_specification
    | MANAGED
	{ sar_set_object_flags (& $1, (sym_m_managed | sym_m_exported)); }
      object_specification
    | UNMANAGED
	{ sar_unset_object_flags (& $1, sym_m_managed), sar_set_object_flags (& $1, sym_m_exported); }
      object_specification
    | epsilon_production
	{ sar_set_object_flags (&$1, sym_m_exported); }
      object_specification
    | IMPORTED
	{ sar_set_object_flags (&$1, sym_m_imported); }
      CLASS_NAME
	{ sar_set_object_class (&$3); }
      epsilon_production
	{ sar_verify_object (&$1); }
    ;

object_specification
    : CLASS_NAME
	{ sar_set_object_class (&$1); }
      opt_create_proc_ref
      opt_variant
      object_spec
	{ sar_verify_object (&$1); }
    ;

opt_variant
    : epsilon_production
	{ sar_set_object_variant (&$1); }
    | variant
	{ sar_set_object_variant (&$1); }
    ;

control_object_specification
    : CLASS_NAME
	{ sar_set_object_class (&$1); $$ = gz_yynullval;}
      opt_create_proc_ref
      opt_variant
      control_object_spec
	{ sar_verify_object (&$1); }
    ;

opt_create_proc_ref
    : epsilon_production
    | PROCEDURE id_ref opt_parens
	{ sar_process_proc_ref (&$2, &gz_yynullval, sym_k_object_proc);
	  sar_save_user_proc_ref_src ( &$1, &$2, &$3 ); }
    ;

opt_parens
    : epsilon_production
    | LEFT_PAREN RIGHT_PAREN { $$ = $2; }
    ;




/*	object specification.    */

object_spec
    : id_ref
	{ sar_object_reference (&$1); }
    | LEFT_BRACE RIGHT_BRACE
    | LEFT_BRACE object_feature_list RIGHT_BRACE
    | error { lex_issue_error (RIGHT_BRACE); } RIGHT_BRACE
    ;

control_object_spec
    : id_ref
	{ sar_object_reference (&$1); }
    | LEFT_BRACE RIGHT_BRACE
      epsilon_production
	{ sar_set_object_flags (&$1, sym_m_exported); }
    | LEFT_BRACE object_feature_list RIGHT_BRACE
      epsilon_production
	{ sar_set_object_flags (&$1, sym_m_exported); }
    | error { lex_issue_error (RIGHT_BRACE); } RIGHT_BRACE
    ;

child_object_spec
    : LEFT_BRACE RIGHT_BRACE
      epsilon_production
	{ sar_set_object_flags (&$1, sym_m_private); }
    | LEFT_BRACE object_feature_list RIGHT_BRACE
      epsilon_production
	{ sar_set_object_flags (&$1, sym_m_private); }
    | error { lex_issue_error (RIGHT_BRACE); } RIGHT_BRACE
    ;

object_feature_list
    : object_feature
    | object_feature_list object_feature
    ;

object_feature
    : argument_list_def SEMICOLON
	{ sar_save_list_end ( &$2); sar_save_feature (&$1); $$ = gz_yynullval; }
    | callback_list_def SEMICOLON
	{ sar_save_list_end ( &$2); sar_save_feature (&$1); $$ = gz_yynullval; }
    | control_list_def SEMICOLON
	{ sar_save_list_end ( &$2); sar_save_feature (&$1); 
sar_update_parent_list (&$1); $$ = gz_yynullval; }
    | error { lex_issue_error (SEMICOLON); } SEMICOLON
    ;

epsilon_production
    : /* empty */
  	{ $$ = gz_yynullval; }
    ;



/*	LIST declaration	*/

list_declaration
    : LIST { $$ = $1; sar_save_section_source (&$1, sym_k_list_section); }
	list_decl_list
    ;

list_decl_list
    : list_decl SEMICOLON { $$ = $1; sar_save_src_semicolon_pos (&$2 ); }
    | list_decl_list list_decl SEMICOLON { $$ = $1; sar_save_src_semicolon_pos (&$3 ); }
    | error { lex_issue_error (SEMICOLON); } SEMICOLON
    ;

list_decl
    : id
	{ sar_create_object (&$1, sym_k_list_entry); 
sar_link_section (&$1); $$ = gz_yynullval;}
      COLON list_specification
	{ $$ = gz_yynullval; }
    ;

list_specification
    : control_list_specification
    | argument_list_specification
    | callback_list_specification
    | procedure_list_specification
    ;

control_list_specification
    : control_list_type
	{ $$ = gz_yynullval; sar_set_list_type (&$1); }
      control_list_spec
	{ sar_verify_object (&$1); }
    ;

argument_list_specification
    : argument_list_type
	{ $$ = gz_yynullval; sar_set_list_type (&$1); }
      argument_list_spec
	{ sar_verify_object (&$1); }
    ;

callback_list_specification
    : callback_list_type
	{ $$ = gz_yynullval; sar_set_list_type (&$1); }
      callback_list_spec
	{ sar_verify_object (&$1); }
    ;

procedure_list_specification
    : procedure_list_type
	{ $$ = gz_yynullval; sar_set_list_type (&$1); }
      procedure_list_spec
	{ sar_verify_object (&$1); }
    ;

argument_list_type
    : ARGUMENTS
	{ $$ = gz_yynullval; $$.b_type = sym_k_argument_list; }
    ;

callback_list_type
    : CALLBACKS
	{ $$ = gz_yynullval; $$.b_type = sym_k_callback_list; }
    ;

procedure_list_type
    : PROCEDURES
	{ $$ = gz_yynullval; $$.b_type = sym_k_proc_ref_list; }
    ;
control_list_type
    : CONTROLS
	{ $$ = gz_yynullval; $$.b_type = sym_k_control_list; }
    ;

argument_list_spec
    : id_ref { sar_object_reference (&$1); }
    | LEFT_BRACE RIGHT_BRACE
    | LEFT_BRACE argument_list_clause_list RIGHT_BRACE
    | error { lex_issue_error (RIGHT_BRACE); } RIGHT_BRACE
    ;

callback_list_spec
    : id_ref { sar_object_reference (&$1); }
    | LEFT_BRACE RIGHT_BRACE
    | LEFT_BRACE callback_list_clause_list RIGHT_BRACE
    | error { lex_issue_error (RIGHT_BRACE); } RIGHT_BRACE
    ;

procedure_list_spec
    : id_ref
	{ sar_object_reference (&$1); }
    | LEFT_BRACE RIGHT_BRACE
    | LEFT_BRACE procedure_list_clause_list RIGHT_BRACE
    | error { lex_issue_error (RIGHT_BRACE); } RIGHT_BRACE
    ;

control_list_spec
    : id_ref { sar_object_reference (&$1); }
    | LEFT_BRACE RIGHT_BRACE
    | LEFT_BRACE control_list_clause_list RIGHT_BRACE
    | error { lex_issue_error (RIGHT_BRACE); } RIGHT_BRACE
    ;

argument_list_clause_list
    : argument_list_clause
    | argument_list_clause_list argument_list_clause
    ;

callback_list_clause_list
    : callback_list_clause
    | callback_list_clause_list callback_list_clause
    ;

procedure_list_clause_list
    : procedure_list_clause
    | procedure_list_clause_list procedure_list_clause
    ;

control_list_clause_list
    : control_list_clause
    | control_list_clause_list control_list_clause
    ;

argument_list_clause
    : argument_list_def SEMICOLON
	{ sar_save_src_entry_end (&$2, &$1); sar_add_list_entry (&$1); $$ = gz_yynullval; }
    | argument_list_item SEMICOLON
	{ sar_save_src_entry_end (&$2, &$1); sar_add_list_entry (&$1); $$ = gz_yynullval; }
    | error { lex_issue_error (SEMICOLON); } SEMICOLON
    ;

callback_list_clause
    : callback_list_def SEMICOLON
	{ sar_save_src_entry_end (&$2, &$1); sar_add_list_entry (&$1); $$ = gz_yynullval; }
    | callback_list_item SEMICOLON
	{ sar_save_src_entry_end (&$2, &$1); sar_add_list_entry (&$1); $$ = gz_yynullval; }
    | error { lex_issue_error (SEMICOLON); } SEMICOLON
    ;

procedure_list_clause
    : procedure_list_def SEMICOLON
	{ sar_save_src_entry_end (&$2, &$1); sar_add_list_entry (&$1); $$ = gz_yynullval; }
    | procedure_list_def_ref SEMICOLON
	{ sar_save_src_entry_end (&$2, &$1); sar_add_list_entry (&$1); $$ = gz_yynullval; }
    | error { lex_issue_error (SEMICOLON); } SEMICOLON
    ;

control_list_clause
    : control_list_def SEMICOLON
	{ sar_save_src_entry_end (&$2, &$1); sar_add_list_entry (&$1); $$ = gz_yynullval; }
    | control_list_item SEMICOLON
	{ sar_save_src_entry_end (&$2, &$1); sar_add_list_entry (&$1); $$ = gz_yynullval; }
    | error { lex_issue_error (SEMICOLON); } SEMICOLON
    ;

control_list_def
    : epsilon_production
	{ $$ = gz_yynullval; sar_create_object (&$1, sym_k_list_entry); }
      control_list_specification
	{ $$ = $1; }
    ;

argument_list_def
    : epsilon_production
	{ $$ = gz_yynullval; sar_create_object (&$1, sym_k_list_entry); }
      argument_list_specification
	{ $$ = $1; }
    ;

callback_list_def
    : epsilon_production
	{ $$ = gz_yynullval; sar_create_object (&$1, sym_k_list_entry); }
      callback_list_specification
	{ $$ = $1; }
    ;

procedure_list_def
    : epsilon_production
	{ $$ = gz_yynullval; sar_create_object (&$1, sym_k_list_entry); }
      procedure_list_specification
	{ $$ = $1; }
    ;

control_list_item

/*	Control item	*/

    : opt_managed control_item
	{ sar_save_control_item (&$1, &$2); $$ = $1; }

    | id 
	{ sar_create_object (& $1, sym_k_widget_entry); $1.b_flags = sym_m_managed; $$ = gz_yynullval;}
      COLON 
      control_object_definition
	{ sar_save_control_widget (& $1, & $2); $$ = $1; }
/* Child */
    | CHILD_NAME opt_child_managed
        { sar_create_child (& $1); $1.b_flags = sym_m_managed; 
	  $$ = gz_yynullval; } 
      child_object_spec
        { sar_save_control_widget (& $1, & $2); $$ = $1; } 
    | CHILD_NAME UNMANAGED
        { sar_create_child (& $1); $$ = gz_yynullval; } 
      child_object_spec
        { sar_save_control_widget (& $1, & $3); $$ = $1; } 
    ;

opt_child_managed
    : epsilon_production
    | MANAGED
    ;


argument_list_item

/*	Argument value pair	*/

    : value EQUAL_SIGN value
	{ sar_save_argument_pair (&$1, &$3, &$2); $$ = $1; }

/*	Argument widget pair	*/

    | value EQUAL_SIGN CLASS_NAME id_ref
	{ YYSTYPE temp [2];
	  temp [0] = gz_yynullval;
	  sar_create_object (&temp [0], sym_k_widget_entry);
	  temp [1] = $3; sar_set_object_class (&temp [1]);
	  temp [1] = $4; sar_object_reference (&temp [1]);
	  sar_verify_object (&temp [1]);
	  sar_save_argument_pair (&$1, &temp [0], &$2); $$ = $1; }
    ;

callback_list_item

/*	Callback reason binding		*/

    : value EQUAL_SIGN procedure_reference
	{ sar_save_reason_binding (&$1, &$3, &$2); $$ = $1; }
    | value EQUAL_SIGN procedure_list_def
	{ sar_save_reason_binding (&$1, &$3, &$2); $$ = $1; }
    ;

control_item
    : epsilon_production
	{ sar_create_object (&$1, sym_k_widget_entry); $$ = gz_yynullval; }
      control_object_specification
	{ $$ = $1; }
    ;

opt_managed
    : epsilon_production
	{ $$.b_flags = sym_m_managed; }
    | MANAGED
	{ $$ = gz_yynullval; $$.b_flags = sym_m_managed; }
    | UNMANAGED
	{ $$ = gz_yynullval; }
    ;

procedure_reference
    : PROCEDURE id_ref opt_procedure_argument
	{ sar_process_proc_ref (&$2, &$3, sym_k_callback_proc); $$ = $2; }
    ;

procedure_list_def_ref
    : id_ref opt_procedure_argument
	{ sar_process_proc_ref (&$1, &$2, sym_k_callback_proc); $$ = $1; }
    ;

opt_procedure_argument
    : epsilon_production
    | LEFT_PAREN value RIGHT_PAREN
	{ $$ = $2; }
    | LEFT_PAREN CLASS_NAME id_ref RIGHT_PAREN
	{ YYSTYPE temp [2];
	  temp [0] = gz_yynullval;
	  sar_create_object (&temp [0], sym_k_widget_entry);
	  temp [1] = $2; sar_set_object_class (&temp [1]);
	  temp [1] = $3; sar_object_reference (&temp [1]);
	  sar_verify_object (&temp [1]);
	  $$ = temp [0]; }
    | LEFT_PAREN RIGHT_PAREN
  	{ $$ = gz_yynullval; }
    ;



/*	definition of an id 	*/

id
    : NAME
    | FONT_NAME
    | COLOR_NAME
    | ARGUMENT_NAME
    | REASON_NAME
    | ENUMVAL_NAME
    | CHILD_NAME
    | keyword						{ sar_map_keyword_to_name( &$$, &$1); }
    ;

/*	reference of an id 	*/

id_ref
    : id
	{ sar_process_id_ref (&$1); $$ = $1; }
    ;

keyword
    : STRING
    | BOOLEAN
    | FLOAT
    | INTEGER
    | FONT
    | ARGUMENT
    | REASON
    | PIXMAP
    | COLOR

    | NAMES
    | CASE_SENSITIVE
    | CASE_INSENSITIVE

    | VERSION

    | MANAGED
    | UNMANAGED

    | UILFILE

    | string_table_type
    | asciz_table_type
    | INTEGER_TABLE
    | TRANSLATION_TABLE
    | COMPOUND_STRING
    | COMPOUND_STRING_COMPONENT
    | CLASS_REC_NAME
    | FONT_TABLE
    | RIGHT_TO_LEFT
    | CHARACTER_SET
    | COLOR_TABLE
    | ICON
    | BACKGROUND
    | FOREGROUND
    | XBITMAPFILE
    | SEPARATE

    | ANY

    | IMPORTED
    | OBJECTS
    | KEYSYM
    | SINGLE_FLOAT
    | RGB
    | WIDE_CHARACTER
    | FONTSET
    ;



/*    values    */

character_8_value    : value	
               { 
                 $$ = $1; 
		 if (($$.b_type != sym_k_error_value) &&
		     ($$.b_type != sym_k_any_value) &&
		    (($$.b_flags &sym_m_forward_ref) == 0))
		       {
			if (($$.b_type != sym_k_char_8_value) &&
			    ($$.b_type != sym_k_localized_string_value))
		  	    sar_value_type_error( &$$, sym_k_char_8_value );
			if (($$.b_flags &sym_m_private) == 0) 
			    sar_private_error( &$$ );
		       }
		}
    ;

private_value
    : value							{ $$ = $1; 
								  if ($$.b_type != sym_k_error_value)
								  {
								    if ((($$.b_flags &sym_m_private) == 0) &&
									(($$.b_flags &sym_m_forward_ref) == 0))
									sar_private_error( &$$ );
								  }
 								} 
     ;
  
 /* BEGIN HaL fix CR 5427 */
 
 private_value_plus
     : non_export_value					        { $$ = $1; 
 								  if ($$.b_type != sym_k_error_value)
 								  {
 								    if ((($$.b_flags &sym_m_private) == 0) &&
 									(($$.b_flags &sym_m_forward_ref) == 0))
 									sar_private_error( &$$ );
 								  }
							}
    ;
     | value							{ $$ = $1; 
 								  if ($$.b_type != sym_k_error_value)
 								  {
 								    if ((($$.b_flags &sym_m_private) == 0) &&
 									(($$.b_flags &sym_m_forward_ref) == 0))
 									sar_private_error( &$$ );
 								  }
 								}  
  
 non_export_value
     : COLOR_TABLE LEFT_PAREN color_list RIGHT_PAREN		{ sar_make_color_table( &$$, &$3, &$1); }
 
 /* END HaL fix CR 5427 */
 

value
    : value_1
    | value OR value_1						{ $$ = $2; sar_binary_op( &$$, &$1, &$3 ); }
    | value XOR value_1						{ $$ = $2; sar_binary_op( &$$, &$1, &$3 ); }
    ;

value_1
    : value_2
    | value_1 AND value_2					{ $$ = $2; sar_binary_op( &$$, &$1, &$3 ); }
    ;

value_2
    : value_3
    | value_2 LEFT_SHIFT value_3				{ $$ = $2; sar_binary_op( &$$, &$1, &$3 ); }
    | value_2 RIGHT_SHIFT value_3				{ $$ = $2; sar_binary_op( &$$, &$1, &$3 ); }
    ;

value_3
    : value_4
    | value_3 PLUS value_4					{ $$ = $2; sar_binary_op( &$$, &$1, &$3 ); }
    | value_3 MINUS value_4					{ $$ = $2; sar_binary_op( &$$, &$1, &$3 ); }
    ;

value_4
    : value_5
    | value_4 MULTIPLY value_5					{ $$ = $2; sar_binary_op( &$$, &$1, &$3 ); }
    | value_4 DIVIDE value_5					{ $$ = $2; sar_binary_op( &$$, &$1, &$3 ); }
    ;

value_5
    : value_6
    | MINUS value_5						{ $$ = $1; sar_unary_op( &$$, &$2 ); }
    | NOT value_5						{ $$ = $1; sar_unary_op( &$$, &$2 ); }
    | PLUS value_5						{ $$ = $1; sar_unary_op( &$$, &$2 ); }
    ;

value_6
    : operand
    | LEFT_PAREN value RIGHT_PAREN				{ $$ = $2; }
    ;

operand
    : UNS_INT_LITERAL						{ sar_make_private_value( &$$, &$1, sym_k_integer_value, 
								  &$1, XmPIXELS); }
    | UNS_INT_LITERAL units_specification			{ sar_make_private_value( &$$, &$1, sym_k_integer_value, 
								  &$1, sar_get_units_type(&$2)); }
    | optional_charset CHAR_8_LITERAL				{ /* Don't use FALSE because it is a token not a Boolean! */
								  Uil_lex_l_charset_specified = 0; 
								  sar_make_private_value( &$$, &$2, sym_k_char_8_value, 
								  &$2, 0); }
    | id							{ sar_process_id( &$$, &$1 );}
    | UNS_FLOAT_LITERAL						{ sar_make_private_value( &$$, &$1, sym_k_float_value, 
								  &$1, XmPIXELS); }
    | UNS_FLOAT_LITERAL units_specification			{ sar_make_private_value( &$$, &$1, sym_k_float_value, 
								  &$1, sar_get_units_type(&$2)); }
    | PIXMAP LEFT_PAREN character_8_value RIGHT_PAREN		{ sar_value_not_implemented( &$$, &$1, "pixmap literal" ); }
    | FONT LEFT_PAREN font_spec RIGHT_PAREN			{ $$ = $3; }
    | FONTSET LEFT_PAREN fontset_spec RIGHT_PAREN		{ $$ = $3; }
    | COLOR LEFT_PAREN color_spec RIGHT_PAREN			{ $$ = $3; }
    | REASON LEFT_PAREN character_8_value RIGHT_PAREN		{ sar_make_private_value(&$$, &$3, sym_k_reason_value, 
								  &$1, 0); }
    | CLASS_REC_NAME LEFT_PAREN character_8_value RIGHT_PAREN	{ sar_make_private_value(&$$, &$3, sym_k_class_rec_name_value,
											 &$1, 0); }
    | ARGUMENT LEFT_PAREN character_8_value opt_arg_type RIGHT_PAREN
								{ sar_make_private_value
								      ( &$$, &$3, sym_k_argument_value, &$1, $4.b_type ); }
    | UILTRUE							{ sar_make_private_value( &$$, &$1, sym_k_bool_value, 
								  &$1, 0); }
    | UILFALSE							{ sar_make_private_value( &$$, &$1, sym_k_bool_value, 
								  &$1, 0); }
    | ON							{ sar_make_private_value( &$$, &$1, sym_k_bool_value, 
								  &$1, 0); }
    | OFF							{ sar_make_private_value( &$$, &$1, sym_k_bool_value, 
								  &$1, 0); }
    | optional_charset COMP_STRING				{ /* Don't use FALSE because it is a token not a Boolean! */
								  Uil_lex_l_charset_specified = 0;
								  sar_make_private_value
								      ( &$$, &$2, sym_k_compound_string_value, &$2, 0); }
    | string_table_type LEFT_PAREN comp_str_list RIGHT_PAREN	{ sar_make_private_value( &$$, &$3, sym_k_string_table_value, 
								  &$1, 0); }
    | asciz_table_type LEFT_PAREN asciz_list RIGHT_PAREN	{ sar_make_private_value( &$$, &$3, sym_k_asciz_table_value, 
								  &$1, 0); }
    | INTEGER_TABLE LEFT_PAREN integer_list RIGHT_PAREN		{ sar_make_private_value( &$$, &$3, sym_k_integer_table_value, 
								  &$1, 0); }
    | TRANSLATION_TABLE LEFT_PAREN string_list RIGHT_PAREN	{ sar_make_private_value( &$$, &$3, sym_k_trans_table_value, 
								  &$1, 0); }
    | COMPOUND_STRING LEFT_PAREN comp_str_result RIGHT_PAREN	{ $$ = $3; }
    | COMPOUND_STRING_COMPONENT LEFT_PAREN comp_str_comp_result RIGHT_PAREN	{ $$ = $3; } 
    | FONT_TABLE LEFT_PAREN font_list RIGHT_PAREN		{ $$ = $3; }
    | ICON LEFT_PAREN icon_spec RIGHT_PAREN			{ $$ = $3; }
    | FLOAT LEFT_PAREN value RIGHT_PAREN			{ $$ = $1; sar_unary_op( &$$, &$3 ); }
    | INTEGER LEFT_PAREN value RIGHT_PAREN			{ $$ = $1; sar_unary_op( &$$, &$3 ); }

    | XBITMAPFILE LEFT_PAREN character_8_value RIGHT_PAREN	{ sar_make_private_value(&$$, &$3, sym_k_xbitmapfile_value,
											 &$1, 0); }

    | KEYSYM LEFT_PAREN character_8_value RIGHT_PAREN		{ sar_make_private_value(&$$, &$3, sym_k_keysym_value,
											 &$1, 0); }

    | SINGLE_FLOAT	LEFT_PAREN value RIGHT_PAREN	        { $$ = $1; sar_unary_op( &$$, &$3 ); }

    | RGB LEFT_PAREN rgb_list RIGHT_PAREN			{ sar_make_private_value( & $$, & $3, sym_k_rgb_value, 
								  &$1, 0); }
    | WIDE_CHARACTER LEFT_PAREN wchar_str_result RIGHT_PAREN	{ $$ = $3; }
    | LOC_STRING						{ sar_make_private_value(&$$, &$1, sym_k_localized_string_value,
								  &$1, 0); }
    ;

string_list
    : /* empty */
	{ $$ = gz_yynullval; }
    | string_list_2
	{ $$ = $1; }
    ;

string_list_2
    : private_value
	{ sar_append_table_value (&$1, &gz_yynullval, sym_k_trans_table_value, &$1); $$ = $1; }
    | string_list_2 COMMA private_value
	{ sar_append_table_value (&$3, &$1, sym_k_trans_table_value, &$2); $$ = $3; }
    ;

asciz_list
    : /* empty */
	{ $$ = gz_yynullval; }
    | asciz_list_2
	{ $$ = $1; }
    ;

asciz_list_2
    : private_value
	{ sar_append_table_value (&$1, &gz_yynullval, sym_k_asciz_table_value, &$1); $$ = $1; }
    | asciz_list_2 COMMA private_value
	{ sar_append_table_value (&$3, &$1, sym_k_asciz_table_value, &$2); $$ = $3; }
    ;

integer_list
    : /* empty */
	{ $$ = gz_yynullval; }
    | integer_list_2
	{ $$ = $1; }
    ;

integer_list_2
    : private_value
	{ sar_append_table_value (&$1, &gz_yynullval, sym_k_integer_table_value, &$1); $$ = $1; }
    | integer_list_2 COMMA private_value
	{ sar_append_table_value (&$3, &$1, sym_k_integer_table_value, &$2); $$ = $3; }
    ;

/* RAP rgb data type */
rgb_list
  : private_value COMMA private_value COMMA private_value
{sar_append_table_value(&$1, & gz_yynullval, sym_k_integer_table_value, & $1);
     sar_append_table_value(& $3, & $1, sym_k_integer_table_value, & $2);
     sar_append_table_value(& $5, & $3, sym_k_integer_table_value, & $4);
     $$ = $5;}
  ;


comp_str_list
    : /* empty */
	{ $$ = gz_yynullval; }
    | comp_str_list_2
	{ $$ = $1; }
    ;

comp_str_list_2
    : private_value
	{ sar_append_table_value (&$1, &gz_yynullval, sym_k_string_table_value, &$1); $$ = $1; }
    | comp_str_list_2 COMMA private_value
	{ sar_append_table_value (&$3, &$1, sym_k_string_table_value, &$2); $$ = $3; }
    ;

opt_arg_type
    : /* empty */					{ $$ = gz_yynullval; $$.b_type = sym_k_any_value; }
    | COMMA arg_value_type					{ $$ = gz_yynullval; $$.b_type = $2.b_type; }
    | COMMA ANY 					{ $$ = gz_yynullval; $$.b_type = sym_k_any_value; }
    ;

comp_str_result
    : private_value 					{ sar_make_comp_str( &$$, &$1, &gz_yynullval, &$-1 ); }
    | private_value comp_str_attrs			{ sar_make_comp_str( &$$, &$1, &$2, &$-1 ); }
    ;

comp_str_comp_result
    : ENUMVAL_NAME	{ sar_make_comp_str_comp(&$$, &$1, &gz_yynullval, &$-1 ); }
    | ENUMVAL_NAME COMMA ENUMVAL_NAME	{ sar_make_comp_str_comp( &$$, &$1, &$3, &$-1 ); } 
    | ENUMVAL_NAME COMMA CHAR_8_LITERAL	{ sar_make_comp_str_comp( &$$, &$1, &$3, &$-1 ); } 
    | ENUMVAL_NAME COMMA LOC_STRING	{ sar_make_comp_str_comp( &$$, &$1, &$3, &$-1 ); } 
    | ENUMVAL_NAME COMMA CHARSET_NAME	{ sar_make_comp_str_comp( &$$, &$1, &$3, &$-1 ); } 
    ;

comp_str_attrs
    : COMMA comp_str_attr				{ sar_chk_comp_str_attr( &$$, &$2, &gz_yynullval ); }
    | comp_str_attrs COMMA comp_str_attr		{ sar_chk_comp_str_attr( &$$, &$3, &$1 ); }
    ;

comp_str_attr
    : CHARACTER_SET EQUAL_SIGN valid_charset		{ $$.b_type = $1.b_type; 
							  $$.b_tag = $3.b_tag;
							  $$.value.az_keyword_entry = $3.value.az_keyword_entry;
							  $$.value.az_symbol_entry = $3.value.az_symbol_entry; }
    | RIGHT_TO_LEFT EQUAL_SIGN private_value		{ $$.b_type = $1.b_type; 
							  $$.value.az_symbol_entry = $3.value.az_symbol_entry; }
    | SEPARATE EQUAL_SIGN private_value			{ $$.b_type = $1.b_type; 
							  $$.value.az_symbol_entry = $3.value.az_symbol_entry; }
    ;

wchar_str_result
    : private_value					{ sar_make_wchar_str( &$$, &$1, &gz_yynullval, &$-1 ); }
    ;

font_spec
    : character_8_value COMMA CHARACTER_SET EQUAL_SIGN valid_charset 
							{ sar_make_font( &$$, &$5, &$1, &$-1 ); }
    | character_8_value					{ sar_make_font( &$$, &gz_yynullval, &$1, &$-1 ); }
    ; 

fontset_spec
    : character_8_value COMMA CHARACTER_SET EQUAL_SIGN valid_charset 
							{ sar_make_fontset( &$$, &$5, &$1, &$-1 ); }
    | character_8_value					{ sar_make_fontset( &$$, &gz_yynullval, &$1, &$-1 ); }
    ; 

font_list						
    : font_item						{ sar_make_font_table( &$$, &$1, &gz_yynullval, &$-1 ); }
    | font_list COMMA font_item				{ sar_make_font_table( &$$, &$3, &$1, &$-1 ); }
    ;

/*
** THIS PRODUCTION DOESN'T LOOK RIGHT!!!!!!!!!!!!!!!!!!!!!!
*/
font_item
    : valid_charset EQUAL_SIGN private_value		{ sar_make_font_item( &$$, &$1, &$3 ); }
    | private_value					{ sar_make_font_item( &$$, &gz_yynullval, &$1 ); }
    ;

optional_charset
    : POUND valid_charset				{ /* Don't use TRUE because it is a token not a Boolean! */
							  $$ = $2; 
							  Uil_lex_l_charset_specified = 1;}
    | epsilon_production
    ;
    

valid_charset
    : CHARSET_NAME					{ sar_charset_verify (&$1); $$ = $1; }
    | CHARACTER_SET LEFT_PAREN charset_info RIGHT_PAREN	{ $$ = $3; }
    | NAME			{ sar_make_fallback_charset(&$1);
				  $$ = $1; } 
			   
    ;

charset_info
    : character_8_value				    	{ sar_make_private_value(&$$, &$1, sym_k_char_8_value, 
							  &$1, 0);
							  sar_make_charset (&$$, &$$, &gz_yynullval, &$1); }
    | character_8_value charset_attrs			{ sar_make_private_value(&$$, &$1, sym_k_char_8_value, 
							  &$1, 0);
							  sar_make_charset (&$$, &$$, &$2, &$1); }
    ;

charset_attrs
    : COMMA charset_attr				{ sar_chk_charset_attr( &$$, &$2, &gz_yynullval ); }
    | charset_attrs COMMA charset_attr			{ sar_chk_charset_attr( &$$, &$3, &$1 ); }
    ;

charset_attr
    : RIGHT_TO_LEFT EQUAL_SIGN private_value		{ $$.b_type = $1.b_type; 
							  if ($3.b_type != sym_k_bool_value) 
							      sar_value_type_error( &$3, sym_k_bool_value );
							  $$.value.az_symbol_entry = $3.value.az_symbol_entry; }
    | SIXTEEN_BIT EQUAL_SIGN private_value		{ $$.b_type = $1.b_type; 
							  if ($3.b_type != sym_k_bool_value) 
							      sar_value_type_error( &$3, sym_k_bool_value );
							  $$.value.az_symbol_entry = $3.value.az_symbol_entry; }
    ;

color_list
    : color_item					{ sar_append_color_item( &$$, &$1, &gz_yynullval ); }
    | color_list COMMA color_item			{ sar_append_color_item( &$$, &$3, &$1 ); }
    ;

color_item
    : BACKGROUND COLOR EQUAL_SIGN character_8_value	{ sar_make_color_item( &$$, &$1, &$4 ); }
    | FOREGROUND COLOR EQUAL_SIGN character_8_value	{ sar_make_color_item( &$$, &$1, &$4 ); }
    | value EQUAL_SIGN character_8_value		{ sar_make_color_item( &$$, &$1, &$3 ); }
    ;

color_spec
    : character_8_value 				{ sar_make_color( &$$, &$1, &gz_yynullval, &$-1 ); }
    | character_8_value COMMA mono_color_spec		{ sar_make_color( &$$, &$1, &$3, &$-1 ); }
    ;

mono_color_spec
    : BACKGROUND
    | FOREGROUND
    ;

icon_spec
    : icon_rows						{ sar_make_icon( &$$, &$1, &gz_yynullval, &$-1); }
    | COLOR_TABLE EQUAL_SIGN private_value COMMA icon_rows
							{ sar_make_icon( &$$, &$5, &$3, &$-1 ); }
    ;

icon_rows
    : private_value
				{ sar_append_table_value (&$1, &gz_yynullval, sym_k_icon_value, &$1); $$ = $1; }
    | icon_rows COMMA private_value
				{ sar_append_table_value (&$3, &$1, sym_k_icon_value, &$2); $$ = $3; }
    ;

units_specification
    : PIX
	{ $$ = $1; }
    | PIXEL
	{ $$ = $1; }
    | PIXELS
	{ $$ = $1; }
    | IN
	{ $$ = $1; }
    | INCH
	{ $$ = $1; }
    | INCHES
	{ $$ = $1; }
    | CM
	{ $$ = $1; }
    | CENTIMETER
	{ $$ = $1; }
    | CENTIMETERS
	{ $$ = $1; }
    | MM
	{ $$ = $1; }
    | MILLIMETER
	{ $$ = $1; }
    | MILLIMETERS
	{ $$ = $1; }
    | PT
	{ $$ = $1; }
    | POINT
	{ $$ = $1; }
    | POINTS
	{ $$ = $1; }
    | FU
	{ $$ = $1; }
    | FONT_UNIT
	{ $$ = $1; }
    | FONT_UNITS
	{ $$ = $1; }
    ;

%%

/*    Dummy error routine for the parser.  We will output our own
      error messages.    */

int yyerror (s)
char	* s;
{
	return 0;
}



