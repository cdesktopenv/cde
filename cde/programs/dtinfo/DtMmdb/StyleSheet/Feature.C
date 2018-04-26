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
// $XConsortium: Feature.cc /main/3 1996/06/11 17:06:39 cde-hal $
#include "SymTab.h"
#include "Feature.h"
#include "FeatureValue.h"
#include <assert.h>
#include <stdarg.h>

Feature::Feature(const Symbol &name, FeatureValue *value)
: f_name(name),
  f_value(value)
{
}

Feature::Feature(const Feature &orig_feature)
: f_name(orig_feature.name()),
  f_value(orig_feature.value()->clone())
{
}

unsigned int Feature::operator==(const Feature &f )
{
  return f.name() == f_name;
}

Feature::~Feature()
{
  delete f_value ;
}

FeatureValue *
Feature::evaluate() const
{
  
  return f_value->evaluate();

}

// /////////////////////////////////////////////////////////////////////////
// Printing
// /////////////////////////////////////////////////////////////////////////

ostream &operator << (ostream &o, const Feature &f)
{
  return f.print(o);
}

ostream &
Feature::print(ostream &o) const
{
  return o << f_name << ": " << *f_value ;
}


void
Feature::merge(const Feature &feature_to_merge)
{
  assert(f_name == feature_to_merge.name());

  if (*f_value == *feature_to_merge.value())
    ;
  else
    {
      FeatureValue *new_value = f_value->merge(*feature_to_merge.value());
      delete f_value ;
      f_value = new_value ;
    }
}

