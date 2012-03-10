// $XConsortium: Attribute.cc /main/4 1996/06/11 17:05:20 cde-hal $
#include "Attribute.h"

// /////////////////////////////////////////////////////////////////////////
// Attribute
// /////////////////////////////////////////////////////////////////////////

Attribute::Attribute(const Symbol &name, char *value) 
: f_name(name),
  f_value(value)
{
}

Attribute::~Attribute()
{
  delete f_value ;
}

Attribute::operator==(const Attribute &attr) const
{
/*
cerr << "Attribute::operator==\n";
cerr << f_name << "\n";
cerr << attr.f_name << "\n";
cerr << "<" << f_name.name() << ">\n";
cerr << "<" << attr.f_name.name() << ">\n";
cerr << attr.name().operator==(f_name)  << "\n";
*/
  return f_name == attr.name();
}

ostream &
Attribute::print(ostream &o) const
{
  return o << f_name << '=' << f_value ;
}
