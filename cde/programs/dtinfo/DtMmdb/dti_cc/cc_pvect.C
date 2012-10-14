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
// $TOG: cc_pvect.C /main/6 1998/04/17 11:45:20 mgreess $

#include "dti_cc/cc_exceptions.h"

template <class T> 
pointer_vector<T>::pointer_vector(const pointer_vector<T>& pv) :
    f_array(new Tptr[pv.f_size]), f_size(pv.f_size), f_items(pv.f_items)  
{
    for (int i=0; i<pv.f_size; i++ )
	f_array[i] = pv.f_array[i];
    
    cerr << "Warning: pointer_vector(const pointer_vector&) called" << endl;
    exit (-1);
}

template <class T> 
pointer_vector<T>::pointer_vector(size_t n, T* t) 
  : f_array(new Tptr[n]), f_size(n), f_items(0)
{
   for ( int i=0; i<f_size; i++ )
    f_array[i] = t;
}

template <class T> 
pointer_vector<T>::~pointer_vector() 
{
   delete f_array;
}

template <class T> 
T* pointer_vector<T>::operator[](ptrdiff_t i) const
{
  if ( i<0 || i>= f_size )
    throw(CASTCCBEXCEPT ccBoundaryException(0, f_size-1, i));
  else
    return f_array[i];
}

template <class T> 
T*& pointer_vector<T>::operator[](ptrdiff_t i)
{
  if ( i<0 || i>= f_size )
    throw(CASTCCBEXCEPT ccBoundaryException(0, f_size-1, i));
  else
     return f_array[i];
}

