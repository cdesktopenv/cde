/* $XConsortium: OL_DataExpr.y /main/2 1996/11/11 11:51:54 drk $ */
%{
#include <stdio.h>
#include "ExprList.h"
#include "Expression.h"
#include "ContentType.h"

extern int yylex();
extern void yyerror( char *str );

extern ContentType *CurrentContentPtr;

%}

%union {
  int        name;
  char       *string;
  Expression *eptr;
}

%token <name>      Reference
%token <name>      Id
%token <string>    Literal
%token Content
%token Concat
%token Attr
%token FirstOf

%type  <eptr>     Expr
%type  <eptr>     ExprList

%start ValList

%%

ValList : ExprList
              {
		CurrentContentPtr->init($1);
	      }
          ;

ExprList  : Expr
              {
		$$ = $1;
	      }
           | Expr ',' ExprList
              {
		$1->next = $3;
		$$ = $1;
	      }
           ;

Expr : Id
         {
	   Expression *expr = new Expression( GENERIC_ID, $1);
	   $$ = expr;
	 }

     | Content
         {
	   Expression *expr = new Expression( CONTENT );
	   $$ = expr;
	 }

     | Concat '(' ExprList ')'
         {
	   ExprList   *elist = new ExprList( $3 );
	   Expression *expr = new Expression( CONCAT, -1, elist);
	   $$ = expr;
	 }
     | Attr '(' Id ')'
         {
	   Expression *expr = new Expression( REFERENCE, $3);
	   $$ = expr;
	 }
     | FirstOf '(' ExprList ')'
         {
	   ExprList *elist = new ExprList ( $3 );
	   Expression *expr = new Expression( FIRSTOF, -1, elist );
	   $$ = expr;
	 }

     | Literal
         {
	   Expression *expr = new Expression( LITERAL, -1, $1 );
	   $$ = expr;
	 }

     | Reference
         {
	   Expression *expr = new Expression( REFERENCE, $1 );
	   $$ = expr;
	 }
  
     ;


%%	


  


