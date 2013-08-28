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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: cc_hdict.h /main/5 1996/08/21 15:49:02 drk $ */

#ifndef _cc_hdict_h
#define _cc_hdict_h 1

#include "dti_cc/types.h"
#include "dti_cc/cc_pvect.h"
#include "dti_cc/CC_Slist.h"

#include <iostream>
using namespace std;

template <class K, class V> 
class kv_pair {
public:

   static CC_Boolean f_needRemove;

   kv_pair(K* k, V* v = 0): f_key(k), f_value(v) {};
   ~kv_pair();

   unsigned int operator==(const kv_pair<K, V>&);

#ifdef DEBUG
   ostream& print(ostream&);
   friend ostream& operator<<(ostream& out, kv_pair<K, V>& kv) {
     return kv.print(out);
   }
#endif

   K* f_key;
   V* f_value;
};

#define DEFAULT_BUCKET_NUM 30
   
template <class K, class V> class hashTableIterator;

template <class K, class V> class hashTable
{
   //template <class K, class V> 
   //friend class hashTableIterator;

   friend class hashTableIterator<K, V>;

protected:
   unsigned (*f_hash_func_ptr)(const K&);
   pointer_vector<CC_TPtrSlist<kv_pair<K, V> > > f_buckets;
   size_t f_items;

protected:
   kv_pair<K, V>* _find(const K* k) const;

public:
   hashTable(const hashTable <K,V> &);
   hashTable(unsigned (*)(const K&), 
             size_t init_bucket_num = DEFAULT_BUCKET_NUM
            );
   ~hashTable();

   void clearAndDestroy();

   size_t entries() { return f_items; };

   CC_Boolean contains(const K*) const;

   V* findValue(const K*) const;
   K* findKeyAndValue(const K*, V*&) const;

   void insertKeyAndValue(K*, V*);

   K* remove(const K*);

#ifdef DEBUG
   ostream& print(ostream& out);

   friend ostream& operator<<(ostream& out, hashTable<K, V>& ht) {
    return ht.print(out);
   };
#endif

};

template <class K, class V> 
class hashTableIterator
{
protected:
   size_t f_bucket_num;
   size_t f_pos;
   kv_pair<K, V>* f_rec;
   hashTable<K, V>& f_hashTable;

   CC_Boolean _findNonEmptyBucket();
   CC_Boolean _findNextRecord();

public:
   hashTableIterator(hashTable<K, V>&);
   ~hashTableIterator();

   CC_Boolean operator++();
   K* key();
   V* value() const;
};

#ifdef EXPAND_TEMPLATES
#include "cc_hdict.C"
#endif


#endif
