/* $XConsortium: cc_pvect.h /main/6 1996/08/21 15:49:14 drk $ */

#ifndef _cc_pvector_h
#define _cc_pvector_h 1

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

   T* operator[](size_t) const;
   T*& operator[](size_t);

//   size_t entries() const { return f_items; };
   size_t length() const { return f_size; };
};

#ifdef EXPAND_TEMPLATES
#include "cc_pvect.C"
#endif


#endif

