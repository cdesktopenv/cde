// $XConsortium: AttributeList.cc /main/4 1996/06/11 17:05:32 cde-hal $
#include "AttributeList.h"

// /////////////////////////////////////////////////////////////////////////
// AttributeList
// /////////////////////////////////////////////////////////////////////////


AttributeList::AttributeList()
: CC_TPtrSlist<Attribute> ()
{
}

AttributeList::~AttributeList()
{
  clearAndDestroy();
}

const Attribute*
AttributeList::lookup(const Symbol &name) const
{
  Attribute attr(name);
  return  find(&attr);
}


void
AttributeList::add(Attribute *attr)
{
  append(attr);
}

ostream &
AttributeList::print(ostream &o) const
{
  CC_TPtrSlistIterator<Attribute> next(*(CC_TPtrSlist<Attribute>*)this);

  while (++next)
    o << ' ' << *next.key() ;

  return o;
}
