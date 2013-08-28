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
// $XConsortium: CC_Dlist.cc /main/5 1996/08/06 09:27:22 rcs $
#ifndef __CC_Dlist_cc
#define __CC_Dlist_cc

#include "Exceptions.hh"
#include "cc_exceptions.h"
#include "CC_Dlist.h"


//---------------------------------------------------------------------
template <class T>
CC_Boolean CC_TPtrDlistIterator<T>::operator+=(size_t n)
{
  for ( size_t i = 0; i < n ; i++ ) {
    if ( !(++(*this)) ) {
      return (FALSE);
    }
  }

  return (TRUE);
}

//---------------------------------------------------------------------
template <class T>
CC_TPtrDlist<T>::CC_TPtrDlist(const CC_TPtrDlist<T>&adlist)
{
  CC_TPtrDlistIterator<T> dlist_iter( *(CC_TPtrDlist<T> *)&adlist );
  while ( dlist_iter() ) {
    insert ( dlist_iter.key() );
  }
}

//---------------------------------------------------------------------
template <class T>
void CC_TPtrDlist<T>::clear()
{
  if ( !this->get_destructed() ) {
    CC_TPtrSlistIterator<T> iter(*this);
    if (++iter) {
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
CC_TPtrDlist<T>::~CC_TPtrDlist()
{
  clear();
}

//---------------------------------------------------------------------
template <class T>
void CC_TPtrDlist<T>::clearAndDestroy()
{
    
    this->set_destructed(TRUE);
    CC_TPtrDlistIterator<T> iter(*this);
    if ( ++iter ) {
	while (1) {
	    CC_Link<T> *elem = (CC_Link<T> *)CC_Listbase::remove( (CC_List_Iterator_base &)iter );
	    if ( elem ) { 
		T *temp_elem = elem->f_element;
		delete temp_elem;
		elem->f_element = NULL; // prevent further destruction on this pointer
		delete elem; 
	    }
	    else { break; }
	}
    }
}      
#endif  
      
