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
/* $XConsortium: Partition.h /main/1 1996/07/29 17:01:35 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef Partition_INCLUDED
#define Partition_INCLUDED 1
#ifdef __GNUG__
#pragma interface
#endif

#include "types.h"
#include "SubstTable.h"
#include "StringOf.h"
#include "ISet.h"
#include "XcharMap.h"
#include "Vector.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class Partition {
public:
  Partition(const ISet<Char> &chars,
	    const ISet<Char> **sets,
	    int nSets,
	    const SubstTable<Char> &subst);
  EquivCode maxCode() const;
  EquivCode charCode(Char c) const;
  EquivCode eECode() const;
  const String<EquivCode> &setCodes(int i) const;
  const XcharMap<EquivCode> &map() const;
private:
  Partition(const Partition &);	// undefined
  void operator=(const Partition &); // undefined
  EquivCode maxCode_;
  Vector<String<EquivCode> > setCodes_;
  XcharMap<EquivCode> map_;
};

inline
EquivCode Partition::maxCode() const
{
  return maxCode_;
}

inline
EquivCode Partition::charCode(Char c) const
{
  return map_[c];
}

inline
EquivCode Partition::eECode() const
{
  return 0;
}

inline
const String<EquivCode> &Partition::setCodes(int i) const
{
  return setCodes_[i];
}

inline
const XcharMap<EquivCode> &Partition::map() const
{
  return map_;
}

#ifdef SP_NAMESPACE
}
#endif

#endif /* not Partition_INCLUDED */
