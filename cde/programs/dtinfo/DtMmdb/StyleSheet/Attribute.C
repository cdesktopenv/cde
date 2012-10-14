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

bool
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
