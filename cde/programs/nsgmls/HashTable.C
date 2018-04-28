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
/* $XConsortium: HashTable.C /main/1 1996/07/29 16:52:50 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef HashTable_DEF_INCLUDED
#define HashTable_DEF_INCLUDED 1

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

template<class K, class V>
void HashTable<K,V>::insert(const K &key, const V &value, Boolean replace)
{
  HashTableItem<K, V> *newItem = new HashTableItem<K, V>(key, value);
  HashTableItem<K, V> *tem = (HashTableItem<K, V> *)table_.insert(newItem);
  if (tem) {
    delete newItem;
    if (replace) {
      tem->key = key;
      tem->value = value;
    }
  }
}

template<class K, class V>
HashTableItem<K,V>::HashTableItem(const K &k, const V &v)
: HashTableItemBase<K>(k), value(v)
{
}

template<class K, class V>
HashTableItemBase<K> *HashTableItem<K,V>::copy() const
{
  return new HashTableItem<K, V>(*this);
}

#ifdef SP_NAMESPACE
}
#endif

#endif /* not HashTable_DEF_INCLUDED */
