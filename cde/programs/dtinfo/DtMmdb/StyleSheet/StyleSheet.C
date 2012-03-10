// $XConsortium: StyleSheet.cc /main/3 1996/06/11 17:09:10 cde-hal $

#include "StyleSheet.h"
#include "Types.h"
#include "Feature.h"

PathTable* 	gPathTab;
VariableTable* 	gVariableTable;
SymbolTable* 	gSymTab;
SymbolTable* 	gElemSymTab;
unsigned int	gGI_CASE_SENSITIVE;
const Element	       *gCurrentElement	;
const FeatureSet       *gCurrentLocalSet;
const FeatureSet       *gParentCompleteSet;

StyleSheet::StyleSheet(const char* nm)
: f_GI_CASE_SENSITIVE(false),
  f_TopOfStack(0,new FeatureSet(),new FeatureSet())
{
  if (nm==0) {
     f_name = new char[1];
     f_name[0] = 0;
  } else
     f_name = strdup(nm);

  use();
}

StyleSheet::~StyleSheet()
{
   delete f_name;
}

void
StyleSheet::use()
{
  gSymTab = &f_SymTab;
  gVariableTable = &f_VarTab;
  gPathTab = &f_PathTab;
  gElemSymTab = &f_ElemSymTab;
  gGI_CASE_SENSITIVE = f_GI_CASE_SENSITIVE;
}
