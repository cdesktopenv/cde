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

