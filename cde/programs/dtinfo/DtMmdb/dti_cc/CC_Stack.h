/* $XConsortium: CC_Stack.h /main/4 1996/10/08 19:23:19 cde-hal $ */
#ifndef _Stack_hh
#define _Stack_hh

#include "Exceptions.hh"
#include "CC_Slist.h"

template <class T> class Stack: public Destructable
{

public:
  Stack ();  /* This is a value stack, ie an assignment operator  
              * for T is assumed */
  
  ~Stack ();

public:
  T	pop ();      
  void	push (const T);
  T&	top () const;
  int   entries() const
  { 
    return( Items->entries() );  //ie no. of elements in the stack
  }
  int     empty() const {
    return( Items->entries() == 0 );
  }

private:
  CC_TValSlist<T> *Items;

};

#endif
