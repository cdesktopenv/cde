/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
// $XConsortium: VariableTable.cc /main/3 1996/06/11 17:09:55 cde-hal $
//
#include "Types.h"
#include "VariableTable.h"

static unsigned shash(const Symbol& key)
{
   return key.hash();
}

VariableTable::VariableTable()
: hashTable<Symbol,Expression>(shash)
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
