/* $XConsortium: VariableTable.h /main/4 1996/08/21 15:51:00 drk $ */
#ifndef _VariableTable_h
#define _VariableTable_h

#include "SymTab.h"
#include "Expression.h"

#ifndef CDE_NEXT

#else
#include "dti_cc/cc_hdict.h"
#endif

class VariableTable : private hashTable<Symbol, Expression>
{
public:
  VariableTable();
  ~VariableTable();

  unsigned int	exists(const Symbol &name) const; 
  void		enter (const Symbol &name, Expression *value);
  const Expression  &lookup(const Symbol &name) const;

  ostream &print(ostream &) const;

};

ostream &operator <<(ostream &o, const VariableTable &v);

#endif /* _VariableTable_h */
/* DO NOT ADD ANY LINES AFTER THIS #endif */
