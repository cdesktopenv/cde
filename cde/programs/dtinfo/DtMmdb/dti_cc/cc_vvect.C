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
  if ( i<0 || i>= f_size )
    throw(ccBoundaryException(0, f_size-1, i));
  else
    return f_array[i];
}

template <class T> 
T& value_vector<T>::operator[](size_t i) 
{
  if ( i<0 || i>= f_size )
    throw(ccBoundaryException(0, f_size-1, i));
  else
    return f_array[i];
}

