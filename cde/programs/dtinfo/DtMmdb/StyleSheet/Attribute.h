/* $XConsortium: Attribute.h /main/3 1996/06/11 17:05:27 cde-hal $ */
#ifndef _Attribute_h
#define _Attribute_h

#include "SymTab.h"

/* **************************************************************
   class Attribute

   a name/value pairing
   ************************************************************** */

class Attribute
{
public:
  Attribute(const Symbol &name, char *value = 0);
  ~Attribute();

  const Symbol &name() const	{ return f_name; }
  operator==(const Attribute &) const ;

  const char   *value() const	{ return f_value ; } 

  ostream &print(ostream &) const;

private:
  Symbol        f_name;
  char	       *f_value ;
};

inline
ostream &operator<<(ostream &o, const Attribute &a)
{
  return a.print(o);
}

#endif /* _Attribute_h */
/* DO NOT ADD ANY LINES AFTER THIS #endif */
