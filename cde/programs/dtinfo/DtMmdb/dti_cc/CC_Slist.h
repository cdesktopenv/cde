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
/* $XConsortium: CC_Slist.h /main/6 1996/08/21 15:48:46 drk $ */
#ifndef __CC_Slist_h
#define __CC_Slist_h

#include "CC_Listbase.h"
#include "cc_exceptions.h"

template <class T> class CC_TPtrSlist;
template <class T> class CC_TPtrSlistIterator;
template <class T> class CC_TPtrDlist;
template <class T> class CC_TPtrDlistIterator;
template <class T> class CC_TValSlist;
template <class T> class CC_TValSlistIterator;
template <class T> class Stack;


template <class T>
class CC_Link : private CC_Link_base
{
friend class CC_TPtrSlist<T>;
friend class CC_TPtrSlistIterator<T>;
friend class CC_TPtrDlist<T>;
friend class CC_TPtrDlistIterator<T>;
friend class CC_TValSlist<T>;
friend class CC_TValSlistIterator<T>;
friend class Stack<T>;

private:
  CC_Link (T *element)
    : f_element (element)
    { }
  
  T*    f_element;
};

   
template <class T> class CC_List_Iterator;

template <class T>
class CC_TPtrSlist : public CC_Listbase
{

//template <class T> friend class CC_List_Iterator;
   friend class CC_List_Iterator<T>;

protected:
   CC_Boolean destructed;

// Inherit public members from CC_Listbase
/*
 * insert
 * append
 * prepend
 * entries
 * first, last
 * removeLast, removeFirst
 */

public:
  CC_TPtrSlist(const CC_TPtrSlist<T> &);

  CC_TPtrSlist() { destructed = FALSE; }
  ~CC_TPtrSlist();

  virtual void clearAndDestroy();
  virtual void clear();           /* clear only removes item, but not calling
				   * individual item's destructor
				   */

  void prepend(T* element)
  { CC_Listbase::prepend (new CC_Link<T> (element)); } 

  void append(T* element) 
  { CC_Listbase::append (new CC_Link<T> (element)); }

  void insert(T* element)
  { CC_Listbase::append (new CC_Link<T> (element)); }

  T*  at(size_t pos) const  /* throw boundaryException
			     * if list size is smaller than pos
			     */
  {
    // Hack to get it passed to iter
    CC_TPtrSlistIterator<T> iter( *(CC_TPtrSlist<T> *)this );
    for ( int i = 0; i <=pos; i++ ) {
      if ( !(++iter) ) {
        throw(CASTCCBEXCEPT ccBoundaryException(0,0,i));
      }
    }

    return( iter.key() );
  }

  T*  removeAt(size_t pos); /* throw boundaryException 
			     * if list size is smaller than pos 
			     */
  T*  removeLast() {
    CC_Link<T> *t = (CC_Link<T> *)(CC_Listbase::removeLast());
    if ( t ) { 
      T * ret = t->f_element;
      delete t;
      return(ret);
    }
    else return(NULL);
  }

  T*  removeFirst() {
    CC_Link<T> *t = (CC_Link<T> *)(CC_Listbase::removeFirst());
    if ( t ) { 
      T *ret = t->f_element;
      delete t;
      return (ret);
    }
    else return(NULL);
  }

  T*  first() const
  { 
    CC_Link<T> *t = (CC_Link<T> *)(CC_Listbase::first());
    if (t) { return( t->f_element ); }
    else return(NULL);
  }

  T*  last() const
  {
    CC_Link<T> *t = (CC_Link<T> *)(CC_Listbase::last());
    if (t) { return( t->f_element ); }
    else return(NULL);
  }

  T*  find(const T*) const;
  T*  find(CC_Boolean (*)(T*, void*), void*) const;

  CC_Boolean contains(const T*) const;
  T*        remove(const T*);

  operator CC_Listbase *() { return(this); }

  CC_Boolean get_destructed() const
    { return (destructed); }

  CC_Boolean set_destructed(CC_Boolean what)
    { destructed = what; }

};


template <class T>
class CC_TPtrSlistIterator : public CC_List_Iterator_base
{
friend class CC_TPtrSlist<T>;

/*
  Inherit all the public/protected member from CC_List_Iterator_base
  reset;
  operator++
  */
public:

  CC_TPtrSlistIterator (CC_TPtrSlist<T> &list)
    : CC_List_Iterator_base ( (CC_Listbase *)&list)
    { }

  T* key() const
    { 
      CC_Link<T> *link_item = (CC_Link<T> *) CC_List_Iterator_base::item();
      if ( link_item ) {
	return ( link_item->f_element );
      }
      else { 
	return(NULL);
      }
    }

  T *operator()()
  {
    if ( ++(*this) ) { return( key() ); }
    else { return(NULL); }
  }


};


template <class T>
class CC_TValSlist : public CC_Listbase 
{

// inherit entries from CC_Listbase

public:
  CC_TValSlist(const CC_TValSlist<T>&);

  CC_TValSlist() {}
  ~CC_TValSlist();
  void append( const T &t) {  /* copies the content of t, also
			       * assumes the copy constructor for type T
			       * exists
			       */

    T *new_element = new T( t );
    CC_Listbase::append(new CC_Link<T>((T *)new_element));
  }

};
  
template <class T>
class CC_TValSlistIterator:public CC_List_Iterator_base
{

  /* inherit public member from CC_List_Iterator_base
   * Boolean operator++()
   */
  
public:
  CC_TValSlistIterator (CC_TValSlist<T> &list)
    : CC_List_Iterator_base ( (CC_Listbase *)&list)
  {}

  T key() const; // Throw ccException if link is undefined

};


#ifdef EXPAND_TEMPLATES
#include "CC_Slist.C"
#endif

#endif /* __CC_Slist_h */
/* DO NOT ADD ANY LINES AFTER THIS #endif */
