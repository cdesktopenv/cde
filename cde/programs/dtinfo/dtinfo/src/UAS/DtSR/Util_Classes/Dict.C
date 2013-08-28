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
// $XConsortium: Dict.cc /main/3 1996/06/11 16:42:15 cde-hal $

#include "Dict.hh"
#include "DictIter.hh"
#include "DictLink.hh"

#ifndef NULL
#define NULL	0
#endif

template<class K, class V>
K
Dict<K,V>::kdef()
{
    static K k;
    return k;
}

template<class K, class V>
V 
Dict<K,V>::vdef()
{
    static V v;
    return v;
}

template<class K, class V>
void
Dict<K,V>::init()
{
    if (head)
	delete head;
    head = NULL;
    current = NULL;
    sz = 0;
}

template<class K, class V>
Dict<K,V>::Dict() : def_key(kdef()), def_val(vdef()), head(NULL)
{
    init();
}


template<class K, class V>
Dict<K,V>::Dict(const K& k, const V& v) : head(NULL), def_key(k), def_val(v)
{
    init();
}

template<class K, class V>
Dict<K,V>::~Dict()
{
    if (head)
	delete head;
}


template<class K, class V>
DictIter<K,V>
Dict<K,V>::element(const K& k)
{
    (void) operator[](k); // move current to k
    return DictIter<K,V>(this, current);
}

template<class K, class V>
DictIter<K,V>
Dict<K,V>::first()
{
    return DictIter<K,V>(this, head);
}

template<class K, class V>
V&
Dict<K,V>::operator[](const K& k)
{
    if (head == NULL) {
	current = head = new DictLink<K,V>(k, def_val);
	current->pre = current->suc = NULL;
	sz++;
	return current->value;
    }

    DictLink<K,V>* p = head;
    for (;;) {
	if (p->key == k) { // found
	    current = p;
	    return current->value;
	}

	if (k < p->key) { // insert before p
	    current = new DictLink<K,V>(k, def_val);
	    current->pre = p->pre;
	    current->suc = p;
	    if (p == head)
		head = current;
	    else
		p->pre->suc = current;
	    p->pre = current;
	    sz++;
	    return current->value;
	}

	DictLink<K,V>* s = p->suc;
	if (s == NULL) {
	    current = new DictLink<K,V>(k, def_val);
	    current->pre = p;
	    current->suc = NULL;
	    p->suc = current;
	    sz++;
	    return current->value;
	}
	p = s;
    }
}


