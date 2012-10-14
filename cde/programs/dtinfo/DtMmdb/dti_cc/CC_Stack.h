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
/* $XConsortium: CC_Stack.h /main/4 1996/10/08 19:23:19 cde-hal $ */
#ifndef _Stack_hh
#define _Stack_hh

#include "Exceptions.hh"
#include "CC_Slist.h"

template <class T> class Stack: public Destructable
{

public:
  /* This is a value stack, ie an assignment operator for T is assumed */
  Stack ()
  {
    Items = new CC_TValSlist<T>();
  }
  
  ~Stack ()
  {
    delete Items;
  }

public:
  T	pop ()
  {
    CC_Link<T> *last_elem = (CC_Link<T> *)Items->removeLast();

    if ( !last_elem ) {
      throw (Exception());
    }

    T *ret = last_elem->f_element;
    delete last_elem;

    T ret_value = *ret;
    delete ret;

    return(ret_value);
  }

  void	push (const T newItem)
  {
    Items->append ( newItem );
  }

  T&	top () const
  {
    CC_Link<T> *last_elem = (CC_Link<T> *)Items->last();
    if ( !last_elem ) {
      throw(Exception());
    }

    return ( *last_elem->f_element );
  }

  int   entries() const
  { 
    return( Items->entries() );  //ie no. of elements in the stack
  }

  int     empty() const {
    return( Items->entries() == 0 );
  }

private:
  CC_TValSlist<T> *Items;

};

#endif
