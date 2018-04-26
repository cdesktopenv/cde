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
// $XConsortium: DictIter.cc /main/3 1996/06/11 16:42:25 cde-hal $

#include "DictLink.hh"
#include "DictIter.hh"
#include "Dict.hh"

#include <assert.h>

template<class K, class V>
DictIter<K,V>::DictIter()
{
    f_map = NULL;
    f_link = NULL;
}

template<class K, class V>
DictIter<K,V>::DictIter(Dict<K,V>* map, DictLink<K,V>* link)
{
    assert( map );
    
    f_map = map;
    f_link = link;
}


template<class K, class V>
DictIter<K,V>::DictIter(Dict<K,V>& map)
{
    f_map = &map;
    f_link = f_map->head;
}

template<class K, class V>
const K&
DictIter<K,V>::key()
{
    if (f_link)
	return f_link->key;
    else
	return f_map->def_key;
}

template<class K, class V>
V&
DictIter<K,V>::value()
{
    if (f_link)
	return f_link->value;
    else
	return f_map->def_val;
}

template<class K, class V>
DictIter<K,V>&
DictIter<K,V>::operator--()
{
    if (f_link)
	f_link = f_link->pre;
    return *this;
}

template<class K, class V>
void
DictIter<K,V>::operator--(int)
{
    if (f_link)
	f_link = f_link->pre;
}

template<class K, class V>
DictIter<K,V>&
DictIter<K,V>::operator++()
{
    if (f_link)
	f_link = f_link->suc;
    return *this;
}

template<class K, class V>
void
DictIter<K,V>::operator++(int)
{
    if (f_link)
	f_link = f_link->suc;
}


