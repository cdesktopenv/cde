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
/* $XConsortium: cc_vvect.h /main/5 1996/08/21 15:49:21 drk $ */

#ifndef _cc_vector_h
#define _cc_vector_h

template <class T> class value_vector
{
protected:
   T* f_array;
   size_t f_size;

protected:
   void _grow(size_t);

public:
   value_vector(const value_vector<T>&);

   value_vector(size_t);
   value_vector(size_t, const T&);
   virtual ~value_vector();

   T operator[](size_t) const;
   T& operator[](size_t) ;
   size_t entries() { return f_size; };
};

#ifdef EXPAND_TEMPLATES
#include "cc_vvect.C"
#endif


#endif
