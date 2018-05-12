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
// $XConsortium: DictLink.hh /main/4 1996/06/11 16:42:36 cde-hal $
#ifndef _TK_ASSOCIATIVE_ARRAY_DICTLINK_HH_
#define _TK_ASSOCIATIVE_ARRAY_DICTLINK_HH_

template<class K, class V> class Dict;
template<class K, class V> class DictIter;

template<class K, class V> class DictLink
{
  friend class Dict<K,V>;
  friend class DictIter<K,V>;

  private:
    const K key;
    V value;

    DictLink<K,V>* pre;
    DictLink<K,V>* suc;

    DictLink(const K& k, const V& v) : key(k), value(v), pre(NULL), suc(NULL) { };
    ~DictLink() { if (suc) delete suc; } // delete all links recursively
};


#endif
