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


