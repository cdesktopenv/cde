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
