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
/* $XConsortium: HashTable.h /main/1 1996/07/29 16:52:57 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef HashTable_INCLUDED
#define HashTable_INCLUDED 1

#include <stddef.h>
#include "OwnerTable.h"
#include "Hash.h"
#include "Boolean.h"
#include "HashTableItemBase.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

template<class K, class V>
class HashTableItem : public HashTableItemBase<K> {
public:
  HashTableItem(const K &k, const V &v);
  HashTableItemBase<K> *copy() const;
  V value;
};

template<class K, class V> class HashTableIter;

template<class K, class V>
class HashTable {
public:
  HashTable() { }
  void insert(const K &key, const V &value, Boolean replace = 1);
  const V *lookup(const K &key) const {
    HashTableItem<K, V> *tem = (HashTableItem<K, V> *)table_.lookup(key);
    return tem ? &tem->value : 0;
  }
  size_t count() const { return table_.count(); }
private:
  CopyOwnerTable<HashTableItemBase<K>, K, Hash, HashTableKeyFunction<K> > table_;
friend class HashTableIter<K,V>;
};

template<class K, class V>
class HashTableIter {
public:
  HashTableIter(const HashTable<K, V> &table) : iter_(table.table_) { }
  Boolean next(const K *&key, const V *&value) {
    HashTableItem<K, V> *p = (HashTableItem<K, V> *)iter_.next();
    if (p) {
      key = &p->key;
      value = &p->value;
      return 1;
    }
    else
      return 0;
  }
private:
  OwnerTableIter<HashTableItemBase<K>, K, Hash, HashTableKeyFunction<K> > iter_;
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not HashTable_INCLUDED */

#ifdef SP_DEFINE_TEMPLATES
#include "HashTable.C"
#endif
