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
/* "$TOG: wmlparse.y /main/10 1997/04/14 12:56:03 dbl $" */
/*
 * This file defines the grammar for the Motif Widget Meta Language (WML),
 * the declarative language which describes the widget, gadgets, and 
 * resources in the UIL language.
 */


%{

#include "wml.h"

#if defined(__STDC__)
#include <string.h>		/* for strcpy() */
#endif

#ifndef XmConst
#if (defined(__STDC__) && __STDC__)  ||  !defined( NO_CONST )
#define XmConst const
#else
#define XmConst
#endif /* __STDC__ */
#endif /* XmConst */

/*
 * Undefine NULL, since it is defined in stdio
 */
#undef NULL

%}


/*
 * Tokens defining terminals of the language
 */

/*
 * token 0 is EOF, not defined here
 */

/*
 * WML statements
 */
%token	CLASS			1	/* CLASS statement */
%token	RESOURCE		2	/* RESOURCE statement */
%token	DATATYPE		3	/* DATATYPE statement */
%token	CONTROLLIST		4	/* CONTROLLIST statement */
%token	ENUMERATIONSET		5	/* ENUMERATIONSET statement */
%token	ENUMERATIONVALUE	6	/* ENUMERATIONVALUE statement */
%token	CHARACTERSET		7	/* CHARACTERSET statement */
%token	CHILD			50	/* CHILD statement */
/*
 * Statement attributes
 */
%token	DOCNAME			8	/* DOCNAME attribute */
%token	CONVFUNC		9	/* CONVENIENCEFUNCTION attribute */
%token	ALIAS			10	/* ALIAS attribute */
%token	TYPE			11	/* TYPE attribute */
%token	RESOURCELITERAL		12	/* RESOURCELITERAL attribute */
%token	RELATED			13	/* RELATED attribute */
%token	INTERNALLITERAL		14	/* INTERNALLITERAL attribute */
%token	XRMRESOURCE		15	/* XRMRESOURCE attribute */
%token	EXCLUDE			16	/* EXCLUDE attribute */
%token	RESOURCES		17	/* RESOURCES attribute */
%token	SUPERCLASS		18	/* SUPERCLASS attribute */
%token	CONTROLS		19	/* CONTROLS attribute */
%token	WIDGETCLASS		20	/* WIDGETCLASS attribute */
%token	DIALOGCLASS		21	/* DIALOGCLASS attribute */
%token	DEFAULT			22	/* DEFAULT attribute */
%token	ENUMLITERAL		23	/* ENUMLITERAL attribute */
%token	XMSTRINGCHARSETNAME	24	/* XMSTRINGCHARSETNAME attribute */
%token	DIRECTION		25	/* DIRECTION attribute */
%token	PARSEDIRECTION		26	/* PARSEDIRECTION attribute */
%token	CHARACTERSIZE		27	/* CHARACTERSIZE attribute */
%token	CTRLMAPSRESOURCE	28	/* CTRLMAPSRESOURCE attribute */
%token	CHILDREN		51	/* CHILDREN attribute */
%token	PARENTCLASS		52	/* PARENTCLASS attribute */
/*
 * Other keywords
 */
%token	METACLASS		29	/* CLASS type */
%token	WIDGET			30	/* CLASS type */
%token	GADGET			31	/* CLASS type */
%token	ARGUMENT		32	/* RESOURCE type */
%token	REASON			33	/* RESOURCE type */
%token	CONSTRAINT		34	/* RESOURCE type */
%token	SUBRESOURCE		35	/* RESOURCE type */
%token	ATTRTRUE		36	/* attribute value */
%token	ATTRFALSE		37	/* attribute value */
%token	LEFTTORIGHT		38	/* DIRECTION attribute */
%token	RIGHTTOLEFT		39	/* DIRECTION attribute */
%token	ONEBYTE			40	/* CHARACTERSIZE attribute */
%token	TWOBYTE			41	/* CHARACTERSIZE attribute */
%token	MIXED1_2BYTE		42	/* CHARACTERSIZE attribute */


/*
 * Special characters
 */
%token	COLON			43
%token	SEMICOLON		44
%token	EQUALS			45
%token	LBRACE			46
%token	RBRACE			47

/*
 * non-terminals
 */
%token	STRING			48	/* Any user literal */

/*
 * Others
 */
%token	ERRORTOKEN		49	/* Any unrecognized character */


%start statement_block_list

%%


/*
 * Productions of the WML grammar. All routines assume that current
 * data structures are kept in globals. This works as the language has
 * no recursive elements.
 */

/*
 * The language consists of blocks of statements. The blocks may occur
 * in any order, and blocks of the same kind may be arbitrarily repeated.
 */

statement_block_list
	: /* empty */
	| statement_block_list statement_block
	;

statement_block
	: class_statement_block
	| resource_statement_block
        | child_statement_block
	| datatype_statement_block
	| ctrlist_statement_block
	| enumset_statement_block
	| enumval_statement_block
	| charset_statement_block
	| error { LexIssueError(0); } SEMICOLON
	;


/*
 * Statement blocks. Each statement block begins with its identifying
 * keyword, then includes an arbitrary number of statements
 */
class_statement_block
	: CLASS
	    class_statement_list
	    ;

class_statement_list
	: class_statement SEMICOLON
	| class_statement_list class_statement SEMICOLON
	| error { LexIssueError(SEMICOLON); } SEMICOLON
	;

resource_statement_block
	: RESOURCE
	    resource_statement_list
	    ;

resource_statement_list
	: resource_statement SEMICOLON
	| resource_statement_list resource_statement SEMICOLON
	| error { LexIssueError(SEMICOLON); } SEMICOLON
	;

child_statement_block
	: CHILD
	    child_statement_list
	    ;

child_statement_list
	: child_statement SEMICOLON
	| child_statement_list child_statement SEMICOLON
	| error { LexIssueError(SEMICOLON); } SEMICOLON
	;

datatype_statement_block
	: DATATYPE
	    datatype_statement_list
	    ;

datatype_statement_list
	: datatype_statement SEMICOLON
	| datatype_statement_list datatype_statement SEMICOLON
	| error { LexIssueError(SEMICOLON); } SEMICOLON
	;

ctrlist_statement_block
	: CONTROLLIST
	    ctrlist_statement_list
	    ;

ctrlist_statement_list
	: ctrlist_statement SEMICOLON
	| ctrlist_statement_list ctrlist_statement SEMICOLON
	| error { LexIssueError(SEMICOLON); } SEMICOLON
	;

enumset_statement_block
	: ENUMERATIONSET
	    enumset_statement_list
	    ;

enumset_statement_list
	: enumset_statement SEMICOLON
	| enumset_statement_list enumset_statement SEMICOLON
	| error { LexIssueError(SEMICOLON); } SEMICOLON
	;

enumval_statement_block
	: ENUMERATIONVALUE
	    enumval_statement_list
	    ;

enumval_statement_list
	: enumval_statement SEMICOLON
	| enumval_statement_list enumval_statement SEMICOLON
	| error { LexIssueError(SEMICOLON); } SEMICOLON
	;

charset_statement_block
	: CHARACTERSET
	    charset_statement_list
	    ;

charset_statement_list
	: charset_statement SEMICOLON
	| charset_statement_list charset_statement SEMICOLON
	| error { LexIssueError(SEMICOLON); } SEMICOLON
	;



/*
 * The CLASS statement
 */
class_statement
	: name
	    COLON
	    class_type
		{ wmlCreateClass (yynameval, yytknval1); }
	    class_definition
	;

class_type
	: METACLASS		{ yytknval1 = METACLASS; }
	| WIDGET		{ yytknval1 = WIDGET; }
	| GADGET		{ yytknval1 = GADGET; }
	;

class_definition
	: /* empty */
	| LBRACE RBRACE
	| LBRACE class_attribute_list RBRACE
	| error { LexIssueError(RBRACE); } RBRACE
	;

class_attribute_list
	: class_attribute
	| class_attribute_list class_attribute
	;

class_attribute
	: class_attribute_name
	    EQUALS
	    STRING
		{ wmlAddClassAttribute (yytknval1, yystringval); }
	    SEMICOLON
	| boolean_class_attribute_name
	    EQUALS
	    boolean_attribute_value
		{ wmlAddClassAttribute (yytknval1, yytknval2); }
	    SEMICOLON
	| class_resources SEMICOLON
	| class_controls SEMICOLON
        | class_children SEMICOLON
	| error { LexIssueError(SEMICOLON); } SEMICOLON
	;

class_attribute_name
	: SUPERCLASS		{ yytknval1 = SUPERCLASS; }
	| PARENTCLASS		{ yytknval1 = PARENTCLASS; }
	| INTERNALLITERAL	{ yytknval1 = INTERNALLITERAL; }
	| DOCNAME		{ yytknval1 = DOCNAME; }
	| CONVFUNC		{ yytknval1 = CONVFUNC; }
	| WIDGETCLASS		{ yytknval1 = WIDGETCLASS; }
	| CTRLMAPSRESOURCE	{ yytknval1 = CTRLMAPSRESOURCE; }
	;

boolean_class_attribute_name
	: DIALOGCLASS		{ yytknval1 = DIALOGCLASS; }

boolean_attribute_value
	: ATTRTRUE		{ yytknval2 = ATTRTRUE; }
	| ATTRFALSE		{ yytknval2 = ATTRFALSE; }
	;

class_resources
	: RESOURCES class_resources_block
	;

class_resources_block
	: /* empty */
	| LBRACE RBRACE
	| LBRACE class_resource_list RBRACE
	| error { LexIssueError(RBRACE); } RBRACE
	;

class_resource_list
	: class_resource_element
	| class_resource_list class_resource_element
	;

class_resource_element
	: name
		{ wmlAddClassResource (yynameval); }
	    class_resource_attributes
	    SEMICOLON
	| error { LexIssueError(SEMICOLON); } SEMICOLON
	;

class_resource_attributes
	: /* empty */
	| LBRACE RBRACE
	| LBRACE class_resource_attribute_list RBRACE
	| error { LexIssueError(RBRACE); } RBRACE
	;

class_resource_attribute_list
	: class_resource_attribute_element
	| class_resource_attribute_list class_resource_attribute_element
	;

class_resource_attribute_element
	: class_resource_attribute_name
	    EQUALS
	    STRING
		{ wmlAddClassResourceAttribute (yytknval1, yystringval); }
	    SEMICOLON
	| boolean_class_resource_attribute_name
	    EQUALS
	    boolean_attribute_value
		{ wmlAddClassResourceAttribute (yytknval1, yytknval2); }
	    SEMICOLON
	| error { LexIssueError(SEMICOLON); } SEMICOLON
	;

class_resource_attribute_name
	: TYPE			{ yytknval1 = TYPE; }
	| DEFAULT		{ yytknval1 = DEFAULT; }
	;

boolean_class_resource_attribute_name
	: EXCLUDE		{ yytknval1 = EXCLUDE; }
	;

class_controls
	: CONTROLS class_controls_block
	;

class_controls_block
	: /* empty */
	| LBRACE RBRACE
	| LBRACE class_controls_list RBRACE
	| error { LexIssueError(RBRACE); } RBRACE
	;

class_controls_list
	: class_controls_element
	| class_controls_list class_controls_element
	;

class_controls_element
	: name
		{ wmlAddClassControl (yynameval); }
	    class_controls_attributes
	    SEMICOLON
	| error { LexIssueError(SEMICOLON); } SEMICOLON
	;

class_controls_attributes
	: /* empty */
	| LBRACE RBRACE
	| error { LexIssueError(RBRACE); } RBRACE
	;

class_children
	: CHILDREN class_children_block
	;

class_children_block
	: /* empty */
	| LBRACE RBRACE
	| LBRACE class_children_list RBRACE
	| error { LexIssueError(RBRACE); } RBRACE
	;

class_children_list
	: class_children_element
	| class_children_list class_children_element
	;

class_children_element
	: name
		{ wmlAddClassChild (yynameval); }
	    SEMICOLON
	| error { LexIssueError(SEMICOLON); } SEMICOLON
	;



/*
 * The RESOURCE statement
 */
resource_statement
	: name
	    COLON
	    resource_type
		{ wmlCreateResource (yynameval, yytknval1); }
	    resource_definition
	;

resource_type
	: ARGUMENT		{ yytknval1 = ARGUMENT; }
	| REASON		{ yytknval1 = REASON; }
	| CONSTRAINT		{ yytknval1 = CONSTRAINT; }
	| SUBRESOURCE		{ yytknval1 = SUBRESOURCE; }
	;

resource_definition
	: /* empty */
	| LBRACE RBRACE
	| LBRACE resource_attribute_list RBRACE
	| error { LexIssueError(RBRACE); } RBRACE
	;

resource_attribute_list
	: resource_attribute
	| resource_attribute_list resource_attribute
	;

resource_attribute
	: resource_attribute_name
	    EQUALS
	    STRING
		{ wmlAddResourceAttribute (yytknval1, yystringval); }
	    SEMICOLON
	| boolean_resource_resource_attribute_name
	    EQUALS
	    boolean_attribute_value
		{ wmlAddResourceAttribute (yytknval1, yytknval2); }
	    SEMICOLON
	| error { LexIssueError(SEMICOLON); } SEMICOLON
	;

resource_attribute_name
	: TYPE			{ yytknval1 = TYPE; }
	| RESOURCELITERAL	{ yytknval1 = RESOURCELITERAL; }
	| ENUMERATIONSET	{ yytknval1 = ENUMERATIONSET; }
	| INTERNALLITERAL	{ yytknval1 = INTERNALLITERAL; }
	| RELATED		{ yytknval1 = RELATED; }
	| DOCNAME		{ yytknval1 = DOCNAME; }
	| DEFAULT		{ yytknval1 = DEFAULT; }
	| ALIAS			{ yytknval1 = ALIAS; }
	;

boolean_resource_resource_attribute_name
	: XRMRESOURCE		{ yytknval1 = XRMRESOURCE; }
	;


/*
 * The CHILD statement
 */
child_statement
       : name COLON STRING
          { wmlCreateChild(yynameval, yystringval); }
       ;
  

/*
 * DATATYPE statement
 */
datatype_statement
	: name
		{ wmlCreateDatatype (yystringval); }
	    datatype_definition
	    ;

datatype_definition
	: /* empty */
	| LBRACE RBRACE
	| LBRACE datatype_attribute_list RBRACE
	| error { LexIssueError(RBRACE); } RBRACE
	;

datatype_attribute_list
	: datatype_attribute
	| datatype_attribute_list datatype_attribute
	;

datatype_attribute
	: datatype_attribute_name
	    EQUALS
	    STRING
		{ wmlAddDatatypeAttribute (yytknval1, yystringval); }
	    SEMICOLON
	| error { LexIssueError(SEMICOLON); } SEMICOLON
	;

datatype_attribute_name
	: INTERNALLITERAL	{ yytknval1 = INTERNALLITERAL; }
	| DOCNAME		{ yytknval1 = DOCNAME; }
	| XRMRESOURCE		{ yytknval1 = XRMRESOURCE; }
	;



/*
 * The CONTROLLIST statement
 */
ctrlist_statement
	: name
		{ wmlCreateOrAppendCtrlList (yynameval); }
	    ctrlist_definition
	;

ctrlist_definition
	: /* empty */
	| LBRACE RBRACE
	| LBRACE ctrlist_controls_list RBRACE
	| error { LexIssueError(RBRACE); } RBRACE
	;

ctrlist_controls_list
	: ctrlist_control
	| ctrlist_controls_list ctrlist_control
	;

ctrlist_control
	: name
		{ wmlAddCtrlListControl (yynameval); }
	    ctrlist_controls_attributes
	    SEMICOLON
	| error { LexIssueError(SEMICOLON); } SEMICOLON
	;

ctrlist_controls_attributes
	: /* empty */
	| LBRACE RBRACE
	| error { LexIssueError(RBRACE); } RBRACE
	;



/*
 * The ENUMERATIONSET statement
 */
enumset_statement
	: name
	    COLON
	    enumset_data_type
		{ wmlCreateEnumSet (yynameval, yystringval); }
	    enumset_definition
	;

enumset_data_type
	:
	STRING
	;

enumset_definition
	: /* empty */
	| LBRACE RBRACE
	| LBRACE enumset_value_list RBRACE
	| error { LexIssueError(RBRACE); } RBRACE
	;

enumset_value_list
	: enumset_value
	| enumset_value_list enumset_value
	;

enumset_value
	: name
	    { wmlAddEnumSetValue (yynameval); }
	    SEMICOLON
	| error { LexIssueError(SEMICOLON); } SEMICOLON
	;



/*
 * The ENUMERATIONVALUE statement
 */
enumval_statement
	: name
	    COLON    
		{ wmlCreateEnumValue (yynameval); }
	    enumvalue_definition
	;

enumvalue_definition
	: /* empty */
	| LBRACE RBRACE
	| LBRACE enumvalue_attributes_list RBRACE
	| error { LexIssueError(RBRACE); } RBRACE
	;

enumvalue_attributes_list
	: enumvalue_attribute
	| enumvalue_attributes_list enumvalue_attribute
	;

enumvalue_attribute
	: enumvalue_attribute_name
	    EQUALS
	    STRING
	        { wmlAddEnumValueAttribute (yytknval1, yystringval); }
	    SEMICOLON
	| error { LexIssueError(SEMICOLON); } SEMICOLON
	;

enumvalue_attribute_name
	: ENUMLITERAL		{ yytknval1 = ENUMLITERAL; }
	;



/*
 * CHARACTERSET statement
 */
charset_statement
	: name
		{ wmlCreateCharset (yystringval); }
	    charset_definition
	    ;

charset_definition
	: /* empty */
	| LBRACE RBRACE
	| LBRACE charset_attribute_list RBRACE
	| error { LexIssueError(RBRACE); } RBRACE
	;

charset_attribute_list
	: charset_attribute
	| charset_attribute_list charset_attribute
	;

charset_attribute
	: charset_attribute_name
	    EQUALS
	    STRING
		{ wmlAddCharsetAttribute (yytknval1, yystringval); }
	    SEMICOLON
	| predefined_charset_attribute_name
	    EQUALS
	    predefined_charset_attribute_value
		{ wmlAddCharsetAttribute (yytknval1, yytknval2); }
	    SEMICOLON
	| error { LexIssueError(SEMICOLON); } SEMICOLON
	;

charset_attribute_name
	: INTERNALLITERAL	{ yytknval1 = INTERNALLITERAL; }
	| ALIAS			{ yytknval1 = ALIAS; }
	| XMSTRINGCHARSETNAME	{ yytknval1 = XMSTRINGCHARSETNAME; }
	;

predefined_charset_attribute_name
	: DIRECTION		{ yytknval1 = DIRECTION; }
	| PARSEDIRECTION	{ yytknval1 = PARSEDIRECTION; }
	| CHARACTERSIZE		{ yytknval1 = CHARACTERSIZE; }
	;

predefined_charset_attribute_value
	: LEFTTORIGHT		{ yytknval2 = LEFTTORIGHT; }
	| RIGHTTOLEFT		{ yytknval2 = RIGHTTOLEFT; }
	| ONEBYTE		{ yytknval2 = ONEBYTE; }
	| TWOBYTE		{ yytknval2 = TWOBYTE; }
	| MIXED1_2BYTE		{ yytknval2 = MIXED1_2BYTE; }



/*
 * Other productions
 */
name
	: STRING
		{ strcpy (yynameval, (XmConst char *)yystringval); }
	;

%%

/*
 * Dummmy error routine, since we output our own.
 */
int yyerror (s)
char *s;
{
	return 0;
}


#include "wmllex.c"

