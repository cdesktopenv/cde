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

    DictLink(const K& k, const V& v) : key(k), value(v) { };
    ~DictLink() { if (suc) delete suc; } // delete all links recursively
};


#endif
