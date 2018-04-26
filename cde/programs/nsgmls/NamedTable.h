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
/* $XConsortium: NamedTable.h /main/2 1996/08/13 10:09:04 mgreess $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef NamedTable_INCLUDED
#define NamedTable_INCLUDED 1

#include "Hash.h"
#include "StringC.h"
#include "Named.h"
#include "OwnerTable.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class NamedTableKeyFunction {
public:
  static inline const StringC &key(const Named &obj) { return obj.name(); }
};

template<class T> class NamedTableIter;
template<class T> class ConstNamedTableIter;

template<class T>
class NamedTable {
public:
  NamedTable() { }
  T *insert(T *p) { return (T *)table_.insert(p); }
  T *lookup(const StringC &str) const { return (T *)table_.lookup(str); }
  T *remove(const StringC &str) { return (T *)table_.remove(str); }
  size_t count() const { return table_.count(); }
  void clear() { table_.clear(); }
  void swap(NamedTable<T> &to) { table_.swap(to.table_); }
private:
  NamedTable(const NamedTable<T> &) {}
  void operator=(const NamedTable<T> &) {}
  OwnerTable<Named, StringC, Hash, NamedTableKeyFunction>
    table_;
  friend class NamedTableIter<T>;
  friend class ConstNamedTableIter<T>;
};

template<class T>
class NamedTableIter {
public:
  NamedTableIter(const NamedTable<T> &table) : iter_(table.table_) { }
  T *next() { return (T *)iter_.next(); }
private:
  OwnerTableIter<Named, StringC, Hash, NamedTableKeyFunction> iter_;
};

template<class T>
class ConstNamedTableIter {
public:
  ConstNamedTableIter(const NamedTable<T> &table) : iter_(table.table_) { }
  const T *next() { return (T *)iter_.next(); }
private:
  OwnerTableIter<Named, StringC, Hash, NamedTableKeyFunction> iter_;
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not NamedTable_INCLUDED */
