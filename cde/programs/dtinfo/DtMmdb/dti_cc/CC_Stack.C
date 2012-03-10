// $XConsortium: CC_Stack.C /main/4 1996/10/08 19:22:53 cde-hal $
#include "CC_Stack.h"

//------------------------------------------------------------------------
template <class T> Stack<T>::Stack ()
{
  Items = new CC_TValSlist<T>();
}

//-------------------------------------------------------------------------
template <class T> Stack<T>::~Stack ()
{
  delete Items;
}

//-------------------------------------------------------------------------
template <class T>
void
Stack<T>::push (const T newItem) 
{
  Items->append ( newItem );
}

//---------------------------------------------------------------------------
template <class T>
T 
Stack<T>::pop () {
  CC_Link<T> *last_elem = (CC_Link<T> *)Items->removeLast();

  if ( !last_elem ) {
    throw (Exception());
  }

  T *ret = last_elem->f_element;
  delete last_elem;

  T ret_value = *ret;
  delete ret;

  return(ret_value);
}

//---------------------------------------------------------------------------
template <class T>
T& 
Stack<T>::top () const
{
  CC_Link<T> *last_elem = (CC_Link<T> *)Items->last();
  if ( !last_elem ) {
    throw(Exception());
  }

  return ( *last_elem->f_element );
}
