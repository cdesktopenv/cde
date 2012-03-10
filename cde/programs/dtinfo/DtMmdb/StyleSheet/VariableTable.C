// $XConsortium: VariableTable.cc /main/3 1996/06/11 17:09:55 cde-hal $
//
#include "Types.h"
#include "VariableTable.h"

static unsigned hash(const Symbol& key)
{
   return key.hash();
}

VariableTable::VariableTable()
: hashTable<Symbol,Expression>(hash)
{
}

VariableTable::~VariableTable()
{
  clearAndDestroy();
}

unsigned int
VariableTable::exists(const Symbol &name) const
{
  return contains(&name);
}
void
VariableTable::enter(const Symbol &name, Expression *value)
{
  Expression *exp = findValue(&name);
  if (exp)
    {
      Symbol *sym = remove(&name);
      delete sym ;
      delete exp ;
    }
  insertKeyAndValue(new Symbol(name), value);
}

const Expression &
VariableTable::lookup(const Symbol &name) const
{
  return *findValue(&name);
}

ostream &
VariableTable::print(ostream &o) const
{
  hashTableIterator<Symbol,Expression>
    next(*(hashTable<Symbol,Expression>*)this); // cast to
							   // non-const  

  while (++next)
    o << *next.key() << "\t" << *next.value() << endl;

  return o << endl;
}

ostream &operator <<(ostream &o, const VariableTable &v)
{
  return v.print(o);
}
