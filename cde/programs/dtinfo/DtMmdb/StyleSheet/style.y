/* $XConsortium: style.y /main/4 1996/11/11 11:51:33 drk $ */

%{
#include <stdio.h>
#include <ctype.h>
#include <sstream>
#include <iostream.h>
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


#define alloca(x)       (malloc(x))

extern void yyerror(char*);
extern int yylex();

extern void enter_sgmlgi_context();

extern featureDefDictionary* g_FeatureDefDictionary;
extern unsigned g_validation_mode;
extern unsigned g_hasSemanticError;

static char localCharToCharPtrBuf[2];

#undef yywrap

const char* toUpperCase(unsigned char* string)
{
   static char buffer[512];
   int j=0;
   for ( int i=0; i<strlen((const char*)string); i++ ) 
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

%}

%union
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
}

%token<intData>
	INTEGER
	OPER_equality
	OPER_relational

%token<boolData>
	BOOLVAL

%token<realData>
	REAL

%token<charData>
	OPER_assign
	ARRAYOPEN
	ARRAYCLOSE
	SEPARATOR
	FSOPEN
	FSCLOSE
	OPER_modify
	OPER_parent
	OPER_attr
	OPER_oneof
	OPER_star
	OPER_or
	OPER_and
	OPER_div
	OPER_parenopen
	OPER_parenclose
	OPER_logicalnegate
	PMEMOPEN
	PMEMCLOSE
	OPER_period
	OPER_plus
	OPER_minus

%token<charPtrData>
	DIMENSION	
	NORMAL_STRING
	UNIT_STRING
	QUOTED_STRING
	GI_CASE_SENSITIVE
	SGMLGI_STRING

%type<intData>
	POSITION_VALUE

%type<charPtrData>
	SGMLGI
	STRING
	array_name
	SGMLGI_CONTENT

%type<charData>
	OPER_mult
	OPER_binop
	OPER_add
	OPER_feature

%type<expPtrData>
	multi_expr
	simple_expr

%type<termNodePtrData>
	term
	symbol
	parent
	attr
	dimension

%type<FeatureSetPtrData>
	featureset
	feature_list

%type<FeatureValuePtrData>
	rhs.gp
	array
	array_member

%type<CompositeVariableNodePtrData>
	string_list

%type<FeatureValueSlistPtrData>
	array_member_list

%type<FeaturePtrData>
	feature

%type<PathPtrData>
	path_term_list

%type<PathTermPtrData>
	path_term

%type<PathFeatureListPtrData>
	path_expr_list
	path_expr

%type<charPtrDlistData>
	feature_name_list

%type<PQExprPtrData>
	boolean_expr
	logical_and_expr
	equality_expr
	path_selectorOPTL
	path_selector

%start	stylesheet

%nonassoc OPER_period
%nonassoc SGMLGI_STRING

%%

stylesheet	: sensitivityOPTL statement.gpOPTL
	{
	}
	;

sensitivity	: GI_CASE_SENSITIVE OPER_assign BOOLVAL 
	{
          gGI_CASE_SENSITIVE = $3;
	}
	;

statement.gp	: var_assignment 
	{
	}
	| path_expr 
	{
	  /* copy items form the feature list into the path table */
          PathFeatureListIterator l_Iter(*($1));

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

	  $1 -> clear();
	  delete $1;

	}
	;

var_assignment	: STRING OPER_assign rhs.gp 
	{
           Expression *x = new Expression(new ConstantNode($3));

           if ( gAutoNumberFP.accept((const char*)$1, x) ) {
	         delete $1;
	         delete x;
                 break;
           }

           gVariableTable -> enter( gSymTab -> intern((const char*)$1), x);
	   delete $1;
	}
	;

symbol	: attr
	{
          $$=$1;
	}
	|
        parent string_list 
	{
          $$=$1;
	}
	| string_list
	{
          const Symbol* x = $1 -> convertableToVariable();
          if ( x ) {
            $$=new VariableNode(*x);
            delete $1;
          } else
            $$=$1;
	}
	;

string_list : string_list OPER_feature STRING
	{

	  $1->appendItem(gSymTab->intern(toUpperCase($3)));
	  $$=$1;
	  delete $3 ;
	}
	| STRING
	{
	  $$=new CompositeVariableNode;
          $$ -> appendItem(gSymTab->intern(toUpperCase($1)));
	  delete $1;
	}
	;

parent	: OPER_parent SGMLGI 
	{
/*
	  $$=new
            ParentNode(gSymTab->intern((const char*)$1));
*/
          MESSAGE(cerr, "^ operator not supported.");
          throw(StyleSheetException());
	}
	;

attr	: OPER_attr SGMLGI 
	{
	  $$=new
            SgmlAttributeNode(gSymTab->intern((const char*)$2));
	  delete $2;
	}
	;

rhs.gp	: simple_expr
	{
	  $$=new FeatureValueExpression($1);
	}
	| array 
	{
          $$=$1;
	}
	| featureset 
	{
	  $$=new FeatureValueFeatureSet($1);
	}
	;


simple_expr   : simple_expr OPER_add multi_expr 
	{
	   BinaryOperatorNode::operatorType opType;
           switch ($2) {
             case '+': opType=BinaryOperatorNode::PLUS; break;
             case '-': opType=BinaryOperatorNode::MINUS; break;
             default:
              throw(badEvaluationException());
           }

           FeatureValueExpression* FVexprL = new FeatureValueExpression($1);
           FeatureValueExpression* FVexprR = new FeatureValueExpression($3);
           
           $$ = new Expression(
		 new BinaryOperatorNode(opType, 
					new ConstantNode(FVexprL),
					new ConstantNode(FVexprR)
				       )
			      );
	}
	| multi_expr 
	{
	   $$=$1;
	}
	;

multi_expr	: multi_expr OPER_mult term 
	{
	   BinaryOperatorNode::operatorType opType;
           switch ($2) {
             case '*': opType=BinaryOperatorNode::TIMES; break;
             case '/': opType=BinaryOperatorNode::DIVIDE; break;
             default:
              throw(badEvaluationException());
           }

           FeatureValueExpression* FVexpr = new FeatureValueExpression($1);
           
           $$ = new Expression(
		 new BinaryOperatorNode(opType, new ConstantNode(FVexpr), $3)
			      );
	}
	| term
	{
           $$ = new Expression($1);
	}
	;

OPER_binop : OPER_mult
        {
	   $$=$1;
        }
	| OPER_add
	{
	   $$=$1;
	}
        ;

OPER_mult	: OPER_star
        {
	   $$=$1;
        }
        | OPER_div
        {
	   $$=$1;
        }
        ;

term	: BOOLVAL
        {
          $$=new ConstantNode(new FeatureValueInt(int($1)));
        }
	| symbol UNIT_STRING
	{
	  FeatureValueExpression* fve = 
		new FeatureValueExpression(new Expression($1));
          FeatureValueDimension* x = 
             new FeatureValueDimension(fve, (const char*)$2);
	  delete $2 ;
          $$=new ConstantNode(x);
	}
	| symbol
	{
	  $$=$1;
	}
	| dimension 
	{
	  $$=$1;
	}
	| QUOTED_STRING
        {
          $$=new ConstantNode(new FeatureValueString((const char*)$1));
	  delete $1 ;
        }
        | INTEGER UNIT_STRING
        {
          $$=new ConstantNode(new FeatureValueDimension(new FeatureValueInt($1), (const char*)$2));
	  delete $2 ;
        }
        | REAL UNIT_STRING
        {
          $$=new ConstantNode(new FeatureValueDimension(new FeatureValueReal($1), (const char*)$2));
	  delete $2 ;
        }
        | INTEGER
        {
          $$=new ConstantNode(new FeatureValueInt($1));
        }
        | REAL
        {
          $$=new ConstantNode(new FeatureValueReal($1));
        }
        | OPER_parenopen simple_expr OPER_parenclose
        {
          $$=new ConstantNode(new FeatureValueExpression($2));
        }
	;

array	: array_name ARRAYOPEN array_member_list ARRAYCLOSE 
	{
          FeatureValueArray* x = 
	     new FeatureValueArray((const char*)$1, $3 -> entries());
          CC_TPtrSlistIterator<FeatureValue> iter(*$3);

          int i = 0;
          while ( ++iter ) {
            (*x)[i++] = iter.key();
          }

          delete $1;
          delete $3;

          $$ = x;
	}
	| array_name ARRAYOPEN ARRAYCLOSE
	{
          $$ = new FeatureValueArray((const char*)$1, 0);
          delete $1;
	}
	;

array_name : STRING
	{
          $$ = $1;
	}
	|
	{
          $$ = new unsigned char[1];
          $$[0] = 0;
	}
	;

array_member_list	: array_member SEPARATOR array_member_list
	{
           $3 -> prepend($1);
           $$ = $3;
	}
	| array_member  
	{
           $$=new CC_TPtrSlist<FeatureValue>;
           $$ -> append($1);
	}
	;

array_member : simple_expr
	{
	   $$ = new FeatureValueExpression($1);
	}
	| array
	{
	   $$=$1;
	}
	;

featureset	: FSOPEN feature_list SEPARATOR_OPTL FSCLOSE 
	{
	   $$=$2;
	}
	| FSOPEN FSCLOSE
	{
	  $$ = new FeatureSet ();
	}
	;

feature_list	: feature_list SEPARATOR feature 
	{
	   if ($3 -> name() == Symbol(gSymTab->intern("FAMILY"))) {
	     // the evaluate() call clones $3 
	     FeatureValueFeatureSet *fvfs = 
	       (FeatureValueFeatureSet*) $3->evaluate();
	     const FeatureSet* fs = fvfs->value();
	      const Feature* charsetF =
		fs->lookup(gSymTab->intern("CHARSET"));
	      // charsetF is a mandatory entry in fontfamily
	      assert( charsetF );
	      const FeatureValueString* fv_string =
			(FeatureValueString*)charsetF->value();
	      const char* charset = *fv_string;
	      assert( charset );
	      
	      int entries = $1 -> entries();
	      for (int i=0; i<entries; i++) {
		const Feature* entry = $1->at(i);
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
		  delete $1 -> removeAt(i);
		  break; // escape from for-loop
		}
	      }
	      delete fvfs ;

	      $$ = $1;
	      $$ -> add($3);
	   }
	   else {
	      if ( $1 -> find((Feature*)$3) ) {
		FeatureSet* fs = new FeatureSet();
		fs -> add($3);

		$$ =new FeatureSet(*$1, *fs);
		delete $1;
		delete fs;
	      }
	      else {
		$$=$1;
		$$ -> add($3);
	      }
	   }
	}
	| feature 
	{
	   $$=new FeatureSet();
	   $$ -> add($1);
	}
	;

SEPARATOR_OPTL : SEPARATOR
	{
	}
	|
	{
	}
	;

feature		: feature_name_list OPER_modify rhs.gp
	{
          CC_TPtrDlistIterator<char> l_Iter(*($1));

          FeatureSet *fs = 0;
          Feature *f = 0;
          FeatureValue *fv = $3;
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

             if ( $1 -> last() != cptr ) {
                fs = new FeatureSet();
                fs -> add(f);
                fv = new FeatureValueFeatureSet(fs);
             }
	  }

	  $1->clearAndDestroy();
	  delete $1 ;
	  $$=f;
	}
	;

feature_name_list	: feature_name_list OPER_feature STRING
	{
          $1 -> prepend((char *)$3);
          $$=$1;
	}
	|
	STRING
	{
          $$=new CC_TPtrDlist<char>;
          $$ -> append((char *)$1);
	}
	;

path_expr	: path_term_list featureset
	{
           $$=new PathFeatureList;
           $$ -> append(new PathFeature($1, $2));
	}
	| path_term_list OPER_parenopen path_expr_list OPER_parenclose 
	{
	  
          PathFeatureListIterator l_Iter(*($3));

          while ( ++l_Iter ) {
             (l_Iter.key()) -> path() -> prependPath(*$1);
          }
	  delete $1;
          $$=$3;
	}
	;

path_expr_list :	path_expr_list SEPARATOR path_expr
	{
           $$=$1;
           $$ -> appendList(*$3);
	   delete $3 ;
	}
	| path_expr
	{
           $$=$1;
	}

path_term_list : path_term_list path_term  
	{
	  $1 -> appendPathTerm($2);
	  $$=$1;
	}
	| path_term
	{
          $$ = new SSPath;
          $$ -> appendPathTerm($1);
	}
	;

path_term : SGMLGI path_selectorOPTL 
	{
	  $$=new PathTerm((const char*)$1, $2);
          delete $1;
	}
	| OPER_oneof 
	{
          localCharToCharPtrBuf[0]=$1; localCharToCharPtrBuf[1]=0;
	  $$=new PathTerm(localCharToCharPtrBuf, 0);
	}
	| OPER_star 
	{
          localCharToCharPtrBuf[0]=$1; localCharToCharPtrBuf[1]=0;
	  $$=new PathTerm(localCharToCharPtrBuf, 0);
	}
	;

OPER_feature : OPER_period
	{
	}

OPER_add : OPER_plus
	{
	}
	| OPER_minus
	{
	}
	;

SGMLGI : SGMLGI_CONTENT
	{
	// char % can start an OLIAS internal element which
	// is used only by the browser.
	// Example %BOGUS within HEAD1 in OLIAS book

           if ( $1[0] != '%' && isalnum($1[0]) == 0 ) {
              MESSAGE(cerr, form("%s is not a SGMLGI", $1));
              throw(badEvaluationException());
           }
          /* note, should probably be using RCStrings, would make wide */
          /* char handling better too? */
           if ( gGI_CASE_SENSITIVE == false )
             {
               for (int i=0; i<strlen((const char*)$1); i++)
                 if ( islower($1[i]) )
                   $1[i] = toupper($1[i]);
             }
           $$=$1;
	}
	;

SGMLGI_CONTENT : STRING OPER_period {enter_sgmlgi_context();} SGMLGI_STRING 
	{
           int l = strlen((char*)$1) + strlen((char*)$4) + 2;
           $$=new unsigned char[l];
           strcpy((char*)$$, (char*)$1);
           strcat((char*)$$, ".");
           strcat((char*)$$, (char*)$4);
           delete $1;
           delete $4;
	}
	| STRING OPER_period
	{
           int l = strlen((char*)$1) + 2;
           $$=new unsigned char[l];
           strcpy((char*)$$, (char*)$1);
           strcat((char*)$$, ".");
           delete $1;
	}

	| STRING
	{
           $$=$1;
	}
	;

dimension : DIMENSION
	{
          int i;

          for (i=0; i<strlen((const char*)$1); i++) {

            if ( isalpha($1[i]) ) 
               break;
          }

          char c;
          float x;
          if ( i > 0 ) {
             c = $1[i]; $1[i]=0;
             x = atof((const char*)$1);
             $1[i]=c;
          } else
             x = 1;

          $$=new ConstantNode(new FeatureValueDimension(new FeatureValueReal(x), (const char*)&$1[i]));

          delete $1;
	}

STRING	: NORMAL_STRING
	{
 	   $$=$1;
	}
	| UNIT_STRING
	{
 	   $$=$1;
	}
	;

path_selector	: ARRAYOPEN boolean_expr ARRAYCLOSE 
	{
           $$=$2;
	}
	;

boolean_expr	: logical_and_expr 
	{
//////////////////////////////////////////////////////
// This portion of the code (up to equality_expr) is 
// hacked for V1.1 only. Due to the way 
// PathQualifier.h is written, this code is not 
// general at all. qfc 8/16/94
//////////////////////////////////////////////////////
           $$=$1;
	}
	| boolean_expr OPER_or logical_and_expr 
	{
           $$ = new PQLogExpr($1, PQor, $3);
	}
	;

logical_and_expr	: equality_expr 
	{
           $$=$1;
	}
	| logical_and_expr OPER_and equality_expr 
	{
           $$ = new PQLogExpr($1, PQand, $3);
	}
	;

equality_expr	: OPER_attr SGMLGI OPER_equality QUOTED_STRING
	{
          $$ = new PQAttributeSelector(
			gSymTab->intern((const char*)$2),
			( $3 == EQUAL ) ? PQEqual : PQNotEqual,
			(const char*)$4
				      );
          delete $2;
          delete $4;
	}
	| NORMAL_STRING OPER_equality POSITION_VALUE
	{
           if ( strcasecmp((char*)$1, "position") == 0 ) {
             $$=new PQPosition(
	  	  ( $2 == EQUAL ) ? PQEqual : PQNotEqual, 
		  $3
				);
           } else
           if ( strcasecmp((char*)$1, "sibling") == 0 ) {
             $$=new PQSibling(
	  	  ( $2 == EQUAL ) ? PQEqual : PQNotEqual, 
		  $3
				);
           } else
              throw(StyleSheetException());

           delete $1;
	}
	;

POSITION_VALUE : INTEGER
	{
           $$ = (int)$1;
	}
	| QUOTED_STRING
	{
           if ( strcasecmp((char*)$1, "#LAST") != 0 ) 
              throw(StyleSheetException());

           $$ = -1;
	}
	;


sensitivityOPTL	: sensitivity
	{

	}
	| /* empty */
	{

	}
	;

statement.gpOPTL : statement.gpPLUS
	{
	}
	| /* empty */
	{
	}
	;

statement.gpPLUS	: statement.gpPLUS statement.gp
	{

	}
	| statement.gp
	{

	}
	;

path_selectorOPTL	: path_selector
	{
          $$=$1;
	}
	| /* empty */
	{
          $$=0;
	}
	;

