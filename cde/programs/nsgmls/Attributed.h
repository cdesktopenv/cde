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
/* $XConsortium: Attributed.h /main/1 1996/07/29 16:46:44 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef Attributed_INCLUDED
#define Attributed_INCLUDED 1

#include "Ptr.h"
#include "Attribute.h"

// This is used for things that have attribute definitions
// that notations and elements.

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class SP_API Attributed {
public:
  Attributed() { }
  ConstPtr<AttributeDefinitionList> attributeDef() const;
  const AttributeDefinitionList *attributeDefTemp() const;
  Ptr<AttributeDefinitionList> attributeDef();
  void setAttributeDef(const Ptr<AttributeDefinitionList> &);
private:
  Ptr<AttributeDefinitionList> attributeDef_;

};

inline
ConstPtr<AttributeDefinitionList> Attributed::attributeDef() const
{
  return attributeDef_;
}

inline
const AttributeDefinitionList *Attributed::attributeDefTemp() const
{
  return attributeDef_.pointer();
}

inline
Ptr<AttributeDefinitionList> Attributed::attributeDef()
{
  return attributeDef_;
}

inline
void Attributed::setAttributeDef(const Ptr<AttributeDefinitionList> &def)
{
  attributeDef_ = def;
}

#ifdef SP_NAMESPACE
}
#endif

#endif /* not Attributed_INCLUDED */
