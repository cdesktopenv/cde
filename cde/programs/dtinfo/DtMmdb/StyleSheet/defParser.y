/* $XConsortium: defParser.y /main/2 1996/11/11 11:51:02 drk $ */

%{
#include <stdio.h>
#include <ctype.h>
#include <sstream>
#include <iostream.h>
#include "Debug.h"
#include "FeatureDefDictionary.h"

#define alloca(x)       (malloc(x))

def_list_t* g_def_list = 0;

extern void yyerror(char*);
extern int yylex();

#undef yywrap

%}

%union
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

}

%token<charData>
	STAR
	COMMA
	COLON
	SEMI_COLON
	FSOPEN
	FSCLOSE
	OPER_parenopen
	OPER_parenclose

%token<intData>
	INTEGER

%token<realData>
	REAL

%token<charPtrData>
	NORMAL_STRING
	QUOTED_STRING
	REF_NAME
	TYPE

%type<charPtrData>
	Name

%type<valueData>
	Value

%type<valueListPtrData>
	ValueList
 	ValueListOpt

%type<typeValuesPtrData>
	TypeValues

%type<typeValuesListPtrData>
	TypeValuesList

%type<defPtrData>
	Def
	DefReference
	CompositeDef
	WildCardDef
	PrimitiveDef
	

%type<defListPtrData>
	DefList

%start	featureDef

%%

featureDef : DefList
	{
	   g_def_list = $1;
	}
	;

DefList : DefList Def
	{
	   $$=$1;

	   if ( $2 -> type() == FeatureDef::WILDCARD )
	      $$ -> prepend($2);
           else
	      $$ -> append($2);
	}
	|
	Def
	{
	   $$=new def_list_t();
	   $$ -> append($1);
	}
	;

Def: 	CompositeDef
	{
	   $$=$1;
	}
	| PrimitiveDef
	{
	   $$=$1;
	}
	| DefReference
	{
	   $$=$1;
	} 
	| WildCardDef
	{
	   $$=$1;
	} 
	;

CompositeDef	: Name FSOPEN DefList FSCLOSE
	{
	   $$= new FeatureDefComposite((char*)$1, $3);
		delete $1;
	}
	;

PrimitiveDef : Name COLON TypeValuesList SEMI_COLON
	{
	   $$= new FeatureDefPrimitive((char*)$1, $3);
		delete $1;
	}
	;

DefReference : Name SEMI_COLON
	{
	   $$= new FeatureDefReference((char*)$1);
		delete $1;
	}
	;

WildCardDef : STAR  SEMI_COLON
	{
	   $$= new FeatureDefWildCard("*");
	}
	;

Name :   NORMAL_STRING
	{
	  $$=$1;
	}
	;

TypeValuesList :   TypeValuesList COMMA TypeValues
	{
	   $$=$1;
	   $$ -> append($3);
	}
	| TypeValues
	{
	   $$=new type_values_list_t();
	   $$ -> append($1);
	}
	;

TypeValues :  TYPE OPER_parenopen ValueList OPER_parenclose
	{
		$$=new TypeValues((char*)$1, $3);
		delete $1;
	}
	| TYPE
	{
		$$=new TypeValues((char*)$1, 0);
		delete $1;
	}
	;

ValueList : ValueList COMMA Value 
	{
	   $1 -> append($3);
	   $$=$1;
	}
	| Value
	{
	   $$ = new defv_t();
	   $$ -> append($1);
            
	}
	;


Value : INTEGER
	{
	   $$=new FeatureValueInt($1);
	}
	| QUOTED_STRING
	{
	   $$=new FeatureValueString((char*)$1);
           delete $1;
	}
	| REAL
	{
	   $$=new FeatureValueReal($1);
	}
	;


%%

/*
void yyerror(char *errorstr)
{
  cerr << errorstr ;
}
*/
