/* $XConsortium: AttributeList.h /main/3 1996/06/11 17:05:38 cde-hal $ */
#ifndef _AttributeList_h
#define _AttributeList_h

#include "Element.h"
#include "Attribute.h"

/* **************************************************************
   class AttributeList

   A linked list of Attributes 
   ************************************************************** */


class AttributeList : private CC_TPtrSlist<Attribute>
{
public:
  AttributeList();
  ~AttributeList();
  
  void add(Attribute *);

  const Attribute *lookup(const Symbol &name) const;

  ostream &print(ostream &) const ;

};

inline
ostream &operator<<(ostream &o, const AttributeList &attrlist)
{
  return attrlist.print(o);
}

#endif /* _AttributeList_h */
/* DO NOT ADD ANY LINES AFTER THIS #endif */
