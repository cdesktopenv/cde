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
// $XConsortium: CC_Stack.C /main/4 1996/10/08 19:22:53 cde-hal $
#include "CC_Stack.h"

//------------------------------------------------------------------------
template <class T> Stack<T>::Stack ()
{
  Items = new CC_TValSlist<T>();
}

//-------------------------------------------------------------------------
template <class T> Stack<T>::~Stack ()
{
  delete Items;
}

//-------------------------------------------------------------------------
template <class T>
void
Stack<T>::push (const T newItem) 
{
  Items->append ( newItem );
}

//---------------------------------------------------------------------------
template <class T>
T 
Stack<T>::pop () {
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

//---------------------------------------------------------------------------
template <class T>
T& 
Stack<T>::top () const
{
  CC_Link<T> *last_elem = (CC_Link<T> *)Items->last();
  if ( !last_elem ) {
    throw(Exception());
  }

  return ( *last_elem->f_element );
}
