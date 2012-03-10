/* $XConsortium: StyleSheet.h /main/3 1996/06/11 17:09:16 cde-hal $ */

#ifndef _StyleSheet_h
#define _StyleSheet_h

#include "SymTab.h"
#include "VariableTable.h"
#include "PathTable.h"
#include "ResolverStack.h"

class StyleSheet 
{

private:
   SymbolTable 		f_SymTab;
   VariableTable 	f_VarTab;
   PathTable 		f_PathTab;
   SymbolTable  	f_ElemSymTab;
   unsigned int		f_GI_CASE_SENSITIVE;
   ResolverStackElement f_TopOfStack;

   char* f_name;

public:
   StyleSheet(const char* name = 0); 
   ~StyleSheet();

   const char* name() { return f_name; };

   void use();
};

#endif /* _StyleSheet_h */
/* DO NOT ADD ANY LINES AFTER THIS #endif */
