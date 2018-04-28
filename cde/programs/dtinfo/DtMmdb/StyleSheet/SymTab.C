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
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
// $XConsortium: SymTab.cc /main/4 1996/06/11 17:09:34 cde-hal $
#include "SymTab.h"
SymbolName::SymbolName(const char *name)
: CC_String(name)
{
}

unsigned int SymbolName::operator==(const SymbolName &string)
{
  return !compareTo(string, exact);
}

unsigned
SymbolTable::hashsym(const SymbolName &string) 
{
  return string.hash();
}



SymbolTable::SymbolTable()
: hashTable<SymbolName, unsigned int>(hashsym),
  f_IDsAssigned(0)
{
  f_wildCardId = intern("?", true).id();
  f_unlimitedWildCardId = intern("*", true).id();
}

SymbolTable::~SymbolTable()
{
  gSymTab = 0;
  // cleanup after ourselves 
  clearAndDestroy();
}


const Symbol 
SymbolTable::intern(const char *name, unsigned int assignId)
{
//cerr << "intern(): name =<" << name << "> " << "this=" << (void*)this << "\n";

  SymbolName sym_name(name);

  unsigned int *id;

  const SymbolName *strptr = findKeyAndValue(&sym_name, id);
//cerr << "strptr =" << (void*)strptr << "\n";
  if (!strptr)
    {
      SymbolName *newname = new SymbolName(name);
      id = new unsigned int((assignId) ? ++f_IDsAssigned : 0);

      insertKeyAndValue(newname, id);
      strptr = newname;
    }
  
/*
#ifdef DEBUG
  {
    hashTableIterator<SymbolName,unsigned int> next(*this);

    cout << "intern(" << name << ")" << endl;
    while (++next)
      cout << "\tKey: " << *next.key() << "\tValue: " << *next.value() << endl;
  }
#endif
*/
//cerr << "Final strptr used =" << (void*)strptr << "\n";

  return Symbol(strptr, *id);
}

// /////////////////////////////////////////////////////////////////////////
// class Symbol
// /////////////////////////////////////////////////////////////////////////

Symbol::Symbol(const SymbolName *name, unsigned int x)
: f_name(name), f_id(x)
{
}

Symbol::Symbol(const Symbol &sym)
: f_name(sym.f_name), f_id(sym.f_id)
{
}


Symbol
Symbol::operator=(const Symbol &other)
{
  f_name = other.f_name; return *this ;
}

const char *
Symbol::name() const
{
  return *f_name ;
}

unsigned int
Symbol::operator==(const Symbol &sym) const
{
  return sym.f_name == f_name ;
}

// /////////////////////////////////////////////////////////////////////////
// Printing
// /////////////////////////////////////////////////////////////////////////



ostream &operator<<(ostream &o, const Symbol &s)
{
  return s.print(o);
}
ostream &operator<<(ostream &o, const SymbolTable &st)
{
  return st.print(o);
}

ostream &
SymbolTable::print(ostream &o) const
{
  hashTableIterator<SymbolName, unsigned int>
    next(*(hashTable<SymbolName, unsigned int>*)this);
  
  o << '<' << endl;

  while(++next)
    o << next.key() << endl;

  o << '>' << endl;
  
  return o ;

}

ostream &
Symbol::print(ostream &o) const
{
#ifdef DEBUG
  f_name->print(o);
  return o << '(' << id() << ')';
#else
  return f_name -> print(o);
#endif
}

ostream &
SymbolName::print(ostream &o) const
{
  return o << data();
}

