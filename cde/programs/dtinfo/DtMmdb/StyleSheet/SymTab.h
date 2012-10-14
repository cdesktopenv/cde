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
/* $XConsortium: SymTab.h /main/5 1996/08/21 15:50:57 drk $ */
#ifndef _SymTab_h
#define _SymTab_h

#ifndef CDE_NEXT


#else
#include "dti_cc/CC_String.h"
#include "dti_cc/cc_hdict.h"
//#include "StyleSheet/cde_next.h"
#include <iostream>
using namespace std;
#endif

#include "Types.h"

/* **************************************************************
 Creating a Symbol Table Class
 
 Symbol Table has one user function, intern, which returns a
 reference to a Symbol  


 A Symbol can be compared to other symbols using the == operator.
 Symbols will only be == if they are the same symbol in the same
 SymbolTable

 * ************************************************************** */


// forward declarations 
class Symbol;

/* -------- class SymbolName -------- */
// derived from CC_String to give a version of  ==

// should be privately inherited with a few promotions

class SymbolName : public CC_String
{
public:
  SymbolName(const char *);
  unsigned int operator==(const SymbolName &);
  ostream &print(ostream &) const ;
};





/* **************************************************************
 SymbolTable derives privately from RWTPtrHashSet so only the
 SymbolTable has access to internal operations
 * ************************************************************** */

class SymbolTable : private hashTable<SymbolName, unsigned int>
{
public:
  SymbolTable();
  ~SymbolTable();

  // intern creates symbol if necessary
  const Symbol intern(const char *name, unsigned int createId = false) ;

  ostream &print(ostream &) const ;

  unsigned int IdsAssigned() { 
    return f_IDsAssigned;
  };

  unsigned int wildCardId() { return f_wildCardId; };
  unsigned int unlimitedWildCardId() { return f_unlimitedWildCardId; };

private:
  static unsigned hashsym(const SymbolName &);

  unsigned int f_wildCardId;
  unsigned int f_unlimitedWildCardId;

  unsigned int f_IDsAssigned;

};


/* **************************************************************
 * class Symbol
 * ************************************************************** */

class Symbol  
{
public:
  // constructor 
  Symbol(const Symbol &);

  // assignment 
  Symbol operator=(const Symbol &other) ;

  const char *name() const;
  unsigned int operator==(const Symbol &) const; /* identity operator */

  // some methods need to be public

  ostream &print(ostream &) const ;

  // for path table 
  unsigned int hash() const	{ return f_name->hash(); }
  unsigned int id() const	{ return f_id; }

protected:
  // alternate constructor 
  // only SymbolTable::intern can create these 
  Symbol(const SymbolName *name, unsigned int);

friend const Symbol SymbolTable::intern(const char *, unsigned int assignId);

private:  
  const SymbolName    *f_name ;	/* never delete this */
  unsigned int 	      f_id;
};


/* **************************************************************
 * external declarations
 * ************************************************************** */

ostream &operator<<(ostream &, const Symbol&);
ostream &operator<<(ostream &, const SymbolTable&);


#endif /* _SymTab_h */
/* DO NOT ADD ANY LINES AFTER THIS #endif */
