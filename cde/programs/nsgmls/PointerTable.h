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
/* $XConsortium: PointerTable.h /main/1 1996/07/29 17:01:44 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef PointerTable_INCLUDED
#define PointerTable_INCLUDED 1

#include "Vector.h"
#include "Boolean.h"
#include <stddef.h>

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

template<class P, class K, class HF, class KF> class PointerTableIter;

template<class P, class K, class HF, class KF>
class PointerTable {
  void constraints() {
    P p(0);
    K key = KF::key(*p);
    unsigned long n = HF::hash(key);
    n = 0;			// prevent warning
  }
public:
  PointerTable();
  P insert(P, Boolean replace = 0);
  // Return a reference so that it is possible to do
  // lookups into a table of smart-pointers from multiple threads.
  const P &lookup(const K &) const;
  P remove(const K &);
  size_t count() const { return used_; }
  void clear();
  void swap(PointerTable<P, K, HF, KF> &);
protected:
  size_t used_;
  size_t usedLimit_;
  Vector<P> vec_;
  P null_;

  size_t startIndex(const K &k) const {
    return size_t(HF::hash(k) & (vec_.size() - 1));
  }
  size_t nextIndex(size_t i) const {
    return i == 0 ? vec_.size() - 1 : i - 1;
  }
  friend class PointerTableIter<P, K, HF, KF>;
};

template<class P, class K, class HF, class KF>
class PointerTableIter {
public:
  PointerTableIter(const PointerTable<P, K, HF, KF> &);
  const P &next();
private:
  const PointerTable<P, K, HF, KF> *tablePtr_;
  size_t i_;
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not PointerTable_INCLUDED */

#ifdef SP_DEFINE_TEMPLATES
#include "PointerTable.C"
#endif
