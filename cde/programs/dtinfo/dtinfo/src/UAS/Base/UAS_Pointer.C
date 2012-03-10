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
