/* $XConsortium: SymTab.h /main/5 1996/08/21 15:50:57 drk $ */
#ifndef _SymTab_h
#define _SymTab_h

#ifndef CDE_NEXT


#else
#include "dti_cc/CC_String.h"
#include "dti_cc/cc_hdict.h"
//#include "StyleSheet/cde_next.h"
#include <iostream.h>
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
