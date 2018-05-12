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
// $XConsortium: DictIter.hh /main/5 1996/08/21 15:45:07 drk $
#ifndef _TK_ASSOCIATIVE_ARRAY_DICTITER_HH_
#define _TK_ASSOCIATIVE_ARRAY_DICTITER_HH_

template <class K,class V> class DictLink;

template<class K, class V> class DictIter
{
  friend class Dict<K,V>;

  private:
    Dict<K,V>* f_map;
    DictLink<K,V>* f_link;

    DictIter(Dict<K,V>*, DictLink<K,V>*);

  public:
    DictIter();
    DictIter(Dict<K,V>&);

    DictLink<K,V>* operator()() { return f_link; }

    const K& key();
    V& value();

    DictIter <K,V>& operator--(); // prefix
    void operator--(int);   // postfix
    DictIter <K,V>& operator++();
    void operator++(int);

};

#ifdef EXPAND_TEMPLATES
#include "DictIter.C"
#endif

#endif
