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
/* $XConsortium: ShortReferenceMap.h /main/1 1996/07/29 17:04:33 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef ShortReferenceMap_INCLUDED
#define ShortReferenceMap_INCLUDED 1
#ifdef __GNUG__
#pragma interface
#endif

#include "Vector.h"
#include "Vector.h"
#include "StringC.h"
#include "Named.h"
#include "Boolean.h"
#include "Entity.h"
#include "Ptr.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class SP_API ShortReferenceMap : public Named {
public:
  ShortReferenceMap();
  ShortReferenceMap(const StringC &);
  Boolean defined() const;
  void setNameMap(Vector<StringC> &map);
  void setEntityMap(Vector<ConstPtr<Entity> > &map);
  Boolean lookup(int i, const StringC *&) const;
  const StringC *entityName(size_t i) const;
  const ConstPtr<Entity> &entity(size_t i) const;
  Boolean used() const;
  void setUsed();
  const Location &defLocation() const;
  void setDefLocation(const Location &);
private:
  ShortReferenceMap(const ShortReferenceMap &);	// undefined
  void operator=(const ShortReferenceMap &);	// undefined
  Vector<StringC> nameMap_;
  Vector<ConstPtr<Entity> > entityMap_;
  ConstPtr<Entity> nullEntity_;
  Boolean used_;
  Location defLocation_;
};

inline
Boolean ShortReferenceMap::defined() const
{
  return nameMap_.size() > 0;
}

inline
const StringC *ShortReferenceMap::entityName(size_t i) const
{
  if (i < nameMap_.size() && nameMap_[i].size() != 0)
    return &nameMap_[i];
  else
    return 0;
}

inline
const ConstPtr<Entity> &ShortReferenceMap::entity(size_t i) const
{
  if (i < entityMap_.size())
    return entityMap_[i];
  else
    return nullEntity_;
}

inline
void ShortReferenceMap::setEntityMap(Vector<ConstPtr<Entity> > &map)
{
  map.swap(entityMap_);
}

inline
Boolean ShortReferenceMap::used() const
{
  return used_;
}

inline
void ShortReferenceMap::setUsed()
{
  used_ = 1;
}

inline
void ShortReferenceMap::setDefLocation(const Location &loc)
{
  defLocation_ = loc;
}

inline
const Location &ShortReferenceMap::defLocation() const
{
  return defLocation_;
}

#ifdef SP_NAMESPACE
}
#endif

#endif /* not ShortReferenceMap_INCLUDED */
