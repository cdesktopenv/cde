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
/* $XConsortium: SubstTable.h /main/1 1996/07/29 17:05:53 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef SubstTable_INCLUDED
#define SubstTable_INCLUDED

#include <limits.h>
#include "StringOf.h"
#include "Boolean.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

template<class T>
class SubstTable {
public:
  SubstTable();
  void addSubst(T from, T to);
  void subst(T &c) const { if (table_.size() > 0) c = table_[c]; }
  void subst(String<T> &) const;
  T operator[](T c) const { return table_.size() > 0 ? table_[c] : c; }
  String<T> inverse(T) const;
  void inverseTable(SubstTable<T> &) const;
private:
  String<T> table_;
  String<T> pairs_;		// mutable
  Boolean pairsValid_;		// mutable
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* SubstTable_INCLUDED */

#ifdef SP_DEFINE_TEMPLATES
#include "SubstTable.C"
#endif
