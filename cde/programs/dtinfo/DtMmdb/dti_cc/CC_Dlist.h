/* $XConsortium: CC_Dlist.h /main/5 1996/08/21 15:48:36 drk $ */
#ifndef __CC_Dlist_h
#define __CC_Dlist_h

#include "CC_Listbase.h"
#include "CC_Slist.h"

template <class T> class CC_TPtrDlist;
template <class T> class CC_TPtrDlistIterator;

template <class T>
class CC_TPtrDlist : public CC_TPtrSlist<T>
{
friend class CC_TPtrDlistIterator<T>;

// Inherit all the public/protected members from CC_TPtrSlist<T>
/*
  entries();
  prepend(T*element);
  append(T*element);
  insert(T*element);
  at(size_t);
  removeAt
  removeLast
  removeFirst
  first
  last
  T *find(const T*)
  T *find(Boolean...)
  contains
  remove
  operator CC_Listbase *
  */

public:
  CC_TPtrDlist(const CC_TPtrDlist<T>&); 

  CC_TPtrDlist() {} 
  ~CC_TPtrDlist();
  void clear();
  virtual void clearAndDestroy();

  operator CC_TPtrSlist<T> *() { return(this); }
  
};


template <class T>
class CC_TPtrDlistIterator : public CC_TPtrSlistIterator<T>
{

/* all the inherited member from CC_TPtrSlistIterator<T> and
 * List_Iterator_base...
 */
//  From CC_TPtrSlistIterator<T>
//    T *key() const
//    T *operator()()
//    Boolean operator++()


// From CC_Listbase_Iterator
//    Boolean operator--();    
     

public:

  CC_TPtrDlistIterator (CC_TPtrDlist<T> &list)
    : CC_TPtrSlistIterator<T>( *((CC_TPtrSlist<T> *)&list) ) 
    { }


  CC_Boolean operator+=(size_t n);

};

#ifdef EXPAND_TEMPLATES
#include "CC_Dlist.C"
#endif

#endif /* __CC_Slist_h */
/* DO NOT ADD ANY LINES AFTER THIS #endif */

