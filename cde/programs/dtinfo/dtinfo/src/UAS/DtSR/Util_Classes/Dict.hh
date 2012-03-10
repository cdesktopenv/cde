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
