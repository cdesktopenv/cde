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
// $XConsortium: Dict.hh /main/6 1996/08/21 15:45:03 drk $
#ifndef _TK_ASSOCIATIVE_ARRAY_DICT_HH_
#define _TK_ASSOCIATIVE_ARRAY_DICT_HH_

template<class K, class V> class DictIter;
template<class K, class V> class DictLink;

template<class K, class V> class Dict
{
    friend class DictIter<K,V>;

  private:
    DictLink<K,V>* head;
    DictLink<K,V>* current;
    int sz;

    K def_key;
    V def_val;

    static K kdef(); // default K value
    static V vdef(); // default V value

//    void find(const K&);
    void init();

//    Dict(const Dict <K,V>&);
//    Dict <K,V>& operator= (const Dict <K,V>&);

  public:
    Dict();
    Dict(const K&, const V&);
    ~Dict();

    V& operator[] (const K&);

    int size() const { return sz; }
    void clear() { init(); }
//  void remove(const K&);

    DictIter<K,V> element(const K&);
    DictIter<K,V> first();
//  DictIter<K,V> last();
};

#ifdef EXPAND_TEMPLATES
#include "Dict.C"
#endif

#endif
