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
// $TOG: CC_Slist.C /main/5 1998/04/17 11:44:28 mgreess $
#ifndef _CC_Slist_cc
#define _CC_Slist_cc

#include "cc_exceptions.h"
#include "CC_Slist.h"

//-----------------------------------------------------------------------
template<class T>
CC_TPtrSlist<T>::CC_TPtrSlist(const CC_TPtrSlist<T>&slist) 
{

  destructed = FALSE;
  CC_TPtrSlistIterator<T> slist_iter( *(CC_TPtrSlist<T> *)&slist );
  
  while ( slist_iter() ) {
    insert( slist_iter.key() );
  }
  
}

//------------------------------------------------------------------
template<class T>
T *CC_TPtrSlist<T>::removeAt(size_t pos) {

  CC_TPtrSlistIterator<T> iter( *this );
  for( size_t i = 0; i <= pos; i++ ) {
    if ( !(++iter) ) {
      throw(CASTCCBEXCEPT ccBoundaryException(0,0,i));
    }
  }

  T *key_val = iter.key();
  CC_Link<T> *elem = (CC_Link<T> *)CC_Listbase::remove( iter );
  delete elem;

  return key_val;

}

//------------------------------------------------------------------
template<class T>
T *CC_TPtrSlist<T>::find(const T* elem) const
{
  CC_TPtrSlistIterator<T> iter( *(CC_TPtrSlist<T> *)this );

  while ( iter() ) {
    if ( *(iter.key()) == *elem ) 
      return (iter.key());
  }

  return  ( NULL );
}

//------------------------------------------------------------------
template<class T>
T *CC_TPtrSlist<T>::find(CC_Boolean (*testFunc)(T*, void *),
			 void *d) const
{
  CC_TPtrSlistIterator<T> iter( *(CC_TPtrSlist<T> *)this );
  while ( iter() ) {
    if ( testFunc( iter.key(), d ) ) {
      return ( iter.key() );
    }
  }

  return ( NULL );
}
 
//------------------------------------------------------------------  
template<class T>
CC_Boolean CC_TPtrSlist<T>::contains(const T *elem) const
{
  CC_TPtrSlistIterator<T> iter( *(CC_TPtrSlist<T> *)this );
  while (iter()) {
    if ( *(iter.key()) == *elem )
      return (TRUE);
  }

  return (FALSE);
}

//------------------------------------------------------------------  
template<class T>
T *CC_TPtrSlist<T>::remove(const T *elem)
{
  CC_TPtrSlistIterator<T> iter( *this );
  while (iter()) {
    if ( *(iter.key()) == *elem ) {
      CC_Link<T> *key_rec = (CC_Link<T> *)CC_Listbase::remove( iter );
      T *ret = key_rec->f_element;
      delete key_rec; /* since key_rec does not allocate any memory for
		       * f_element, it is ok to delete key_rec
		       */

      return(ret);
    }
  }

  return(NULL);
}

//---------------------------------------------------------------------
template <class T>
CC_TPtrSlist<T>::~CC_TPtrSlist()
{
  if ( !destructed ) {
    CC_TPtrSlistIterator<T> iter(*this);
    if ( ++iter ) {
      while (1) {
	CC_Link<T> *elem = (CC_Link<T> *)CC_Listbase::remove( (CC_List_Iterator_base &)iter );
	if ( elem ) {
	  delete elem;
	}
	else { break; }
      }
    }
  }
}

//---------------------------------------------------------------------    
template <class T>
void CC_TPtrSlist<T>::clearAndDestroy()
{
    destructed = TRUE;
    CC_TPtrSlistIterator<T> iter(*this);
    if ( ++iter ) {
	while (1) {
	    CC_Link<T> *elem = (CC_Link<T> *)CC_Listbase::remove( (CC_List_Iterator_base &)iter );
	    if ( elem ) { 
		T *temp_elem = elem->f_element;
		delete temp_elem;
		elem->f_element = NULL; // prevent further destruction on the pointer
		delete elem; 
	    }
	    else { break; }
	}
    }
}

//---------------------------------------------------------------------
template<class T>
void CC_TPtrSlist<T>::clear()
{
  CC_TPtrSlistIterator<T> iter(*this);
  if ( ++iter ) {
    while (1) {
      CC_Link<T> *elem = (CC_Link<T> *)CC_Listbase::remove( (CC_List_Iterator_base &)iter );
      if ( elem ) { 
	elem->f_element = 0;
	delete elem;
      }
      else { break; }
    }
  }
}

//---------------------------------------------------------------------
template<class T>
CC_TValSlist<T>::CC_TValSlist(const CC_TValSlist<T> &sval_list) 
{
  CC_TValSlistIterator<T> slist_val_iter( *(CC_TValSlist<T> *)&sval_list );
  while ( ++slist_val_iter ) {
    append( slist_val_iter.key() );
  }
}

//---------------------------------------------------------------------
template <class T>
CC_TValSlist<T>::~CC_TValSlist()
{
    CC_TValSlistIterator<T> iter( *this );
    if ( ++iter ) {
	while (1) {
	    CC_Link<T> *elem = (CC_Link<T> *)CC_Listbase::remove( (CC_List_Iterator_base &)iter );
	    if ( elem ) {
		if ( elem->f_element ) { 
		    T *temp_elem = elem->f_element;
		    delete temp_elem;
		}
		delete elem;
	    }
	    else { 
		break; 
	    }
	}
    }
}
  
//---------------------------------------------------------------------
template <class T>
T CC_TValSlistIterator<T>::key() const
{
  CC_Link<T> *link_item = (CC_Link<T> *) CC_List_Iterator_base::item();
  if ( link_item ) {
    return ( *(link_item->f_element) );
  }
  else {
    throw (CASTCCEXCEPT  ccException() );
  }
}

#endif /* _CC_Slist_cc */
/* DO NOT ADD ANY LINES AFTER THIS #endif */
