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
// $XConsortium: cc_vvect.C /main/4 1996/08/21 15:49:18 drk $

#include "dti_cc/cc_exceptions.h"

template <class T> 
value_vector<T>::value_vector(const value_vector<T>& vv) :
   f_array(new T[vv.f_size]), f_size(vv.f_size)
{
   for (int i=0; i<vv.f_size; i++ )
     f_array[i] = vv.f_array[i];

   cerr << "WARNING: value_vector(const value_vector&) called";
   exit(-1);
}

template <class T> 
value_vector<T>::value_vector(size_t n) : 
   f_array(new T[n]), f_size(n)
{
}

template <class T> 
value_vector<T>::value_vector(size_t n, const T& t) :
   f_array(new T[n]), f_size(n)
{
   for (int i=0; i<f_size; i++ )
     f_array[i] = t;
}

template <class T> 
value_vector<T>::~value_vector()
{
   delete f_array;
}

template <class T> 
void value_vector<T>::_grow(size_t t) 
{
}

template <class T> 
T value_vector<T>::operator[](size_t i) const
{
  if ( (long)i < 0 || i >= f_size )
    throw(ccBoundaryException(0, f_size-1, i));
  else
    return f_array[i];
}

template <class T> 
T& value_vector<T>::operator[](size_t i) 
{
  if ( (long)i < 0 || i >= f_size )
    throw(ccBoundaryException(0, f_size-1, i));
  else
    return f_array[i];
}

