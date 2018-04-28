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
// $XConsortium: UAS_Pointer.cc /main/5 1996/08/06 09:22:59 rcs $
#include <stdio.h>

template <class T>
UAS_Pointer<T>::UAS_Pointer (): f_pointer((T *) 0) {
}

template <class T> 
UAS_Pointer<T>::UAS_Pointer (const UAS_Pointer<T> &pointer):
	    f_pointer (pointer.f_pointer) {
    if (f_pointer) {
	f_pointer->reference();
    }
}

template <class T> 
UAS_Pointer<T>::UAS_Pointer (T *pointer): f_pointer (pointer) {
    if (f_pointer) {
	f_pointer->reference();
    }
}

template <class T> 
UAS_Pointer<T>::~UAS_Pointer() {
    if (f_pointer) {
	f_pointer->unreference();
    }
}

template <class T>
UAS_Pointer<T>::operator T *() {
    return f_pointer;
}

template <class T>
T *
UAS_Pointer<T>::operator -> () {
    return f_pointer;
}

template <class T>
const T *
UAS_Pointer<T>::operator -> () const {
    return f_pointer;
}

template <class T>
UAS_Pointer<T>::operator T* () const {
    return f_pointer;
}

#ifdef IBM_IGNORES_IT
template <class T>
UAS_Pointer<T>::operator T& () const {
    return *f_pointer;
}
#endif

template <class T>
UAS_Pointer<T> &
UAS_Pointer<T>::operator = (T *pointer) {
    T *oldPointer = f_pointer;
    f_pointer = pointer;
    if (f_pointer)
	f_pointer->reference();
    if (oldPointer)
	oldPointer->unreference();
    return (*this);
}

template <class T>
UAS_Pointer<T> &
UAS_Pointer<T>::operator = (const UAS_Pointer<T> &pointer) {
    T *oldPointer = f_pointer;
    f_pointer = pointer.f_pointer;
    if (f_pointer)
	f_pointer->reference();
    if (oldPointer)
	oldPointer->unreference();
    return (*this);
}

template <class T>
int
UAS_Pointer<T>::operator== (const UAS_Pointer<T> &pointer) const {
    if ((!f_pointer && pointer.f_pointer) || (f_pointer && !pointer.f_pointer)) {
	return 0;
    } else if (f_pointer == pointer.f_pointer) {
	return 1;
    } else {
	return *f_pointer == *pointer.f_pointer;
    }
}

template <class T>
int
UAS_Pointer<T>::operator!= (const UAS_Pointer<T> &pointer) const {
    return !(*this == pointer);
}


template <class T>
int
UAS_Pointer<T>::operator==(const int) const {
    return f_pointer == NULL;
}

template <class T>
int
UAS_Pointer<T>::operator!=(const int) const {
    return f_pointer != NULL;
}
