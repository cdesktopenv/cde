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
/* $XConsortium: cc_pvect.h /main/6 1996/08/21 15:49:14 drk $ */

#ifndef _cc_pvector_h
#define _cc_pvector_h 1

#include <stddef.h>
#include "dti_cc/types.h"

template <class T> 
class pointer_vector
{
protected:
   
    typedef T *Tptr;
    
    Tptr *f_array;
    size_t f_size;
    size_t f_items;
    
protected:

public:
   pointer_vector(const pointer_vector<T> &);
   pointer_vector(size_t, T* = 0);
   ~pointer_vector();

   T* operator[](ptrdiff_t) const;
   T*& operator[](ptrdiff_t);

//   size_t entries() const { return f_items; };
   size_t length() const { return f_size; };
};

#ifdef EXPAND_TEMPLATES
#include "cc_pvect.C"
#endif


#endif

