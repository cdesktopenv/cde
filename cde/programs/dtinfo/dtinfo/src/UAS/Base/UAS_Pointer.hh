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
