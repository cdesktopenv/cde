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
