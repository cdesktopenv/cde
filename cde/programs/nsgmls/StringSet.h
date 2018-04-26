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
/* $XConsortium: StringSet.h /main/1 1996/07/29 17:05:35 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef StringSet_INCLUDED
#define StringSet_INCLUDED 1

#include "StringC.h"
#include "Hash.h"
#include "Boolean.h"
#include "OwnerTable.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class StringSetKey {
public:
  static inline const StringC &key(const StringC &str) { return str; }
};

class StringSet {
public:
  StringSet();
  Boolean add(const StringC &);	// return 1 if already there
  void swap(StringSet &);
  void clear();
private:
  StringSet(const StringSet &);	// undefined
  void operator=(const StringSet &); // undefined
  OwnerTable<StringC, StringC, Hash, StringSetKey> table_;
};

inline
void StringSet::clear()
{
  table_.clear();
}

#ifdef SP_NAMESPACE
}
#endif

#endif /* not StringSet_INCLUDED */
