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
// $XConsortium: UAS_Pointer.hh /main/6 1996/08/21 15:44:33 drk $
#ifndef _UAS_Pointer_hh
#define _UAS_Pointer_hh

template <class T> class UAS_Pointer {
public:
    UAS_Pointer();
    UAS_Pointer (const UAS_Pointer<T> &);
    UAS_Pointer (T *pointer);
    ~UAS_Pointer();

    // type converter from this class to T*
    
    operator T*();
    
    T * operator->();
    const T *operator->() const;
    
#ifdef IBM_IGNORES_IT
    operator T&() const;
#endif
    operator T*() const;
    UAS_Pointer<T> &operator= (T *);
    UAS_Pointer<T> &operator= (const UAS_Pointer<T>&);
    
    int operator== (const UAS_Pointer<T> &pointer) const;
    int operator!= (const UAS_Pointer<T> &pointer) const;
    int operator== (const int) const;
    int operator!= (const int) const;
    
private:
    T *f_pointer;
};

#ifdef EXPAND_TEMPLATES
#include "UAS_Pointer.C"
#endif

#endif
