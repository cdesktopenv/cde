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
/* $XConsortium: OwnerTable.h /main/2 1996/08/13 10:09:13 mgreess $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef OwnerTable_INCLUDED
#define OwnerTable_INCLUDED 1

#include "PointerTable.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

template<class T, class K, class HF, class KF>
class OwnerTable : public PointerTable<T *, K, HF, KF> {
public:
  OwnerTable() { }
  ~OwnerTable();
  void clear();
  void swap(OwnerTable<T, K, HF, KF> &x) {
    PointerTable<T *, K, HF, KF>::swap(x);
  }
private:
  OwnerTable(const OwnerTable<T, K, HF, KF> &) {}
  void operator=(const OwnerTable<T, K, HF, KF> &) {}
};

template<class T, class K, class HF, class KF>
class OwnerTableIter : public PointerTableIter<T *, K, HF, KF> {
public:
  OwnerTableIter(const OwnerTable<T, K, HF, KF> &table)
    : PointerTableIter<T *, K, HF, KF>(table) { }
};

template<class T, class K, class HF, class KF>
class CopyOwnerTable : public OwnerTable<T, K, HF, KF> {
public:
  CopyOwnerTable() { }
  CopyOwnerTable(const CopyOwnerTable<T, K, HF, KF> &tab) { *this = tab; }
  void operator=(const CopyOwnerTable<T, K, HF, KF> &tab);
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not OwnerTable_INCLUDED */

#ifdef SP_DEFINE_TEMPLATES
#include "OwnerTable.C"
#endif
