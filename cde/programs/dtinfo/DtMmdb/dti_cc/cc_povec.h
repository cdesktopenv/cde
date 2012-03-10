/* $XConsortium: cc_povec.h /main/5 1996/08/21 15:49:07 drk $ */

#ifndef _cc_dlist_array_h
#define _cc_dlist_array_h

#include "dti_cc/CC_Dlist.h"

template <class T> 
class dlist_array : public CC_TPtrDlist<T>
{
protected:

public:
   dlist_array(const dlist_array<T>&);
   dlist_array(size_t);
   virtual ~dlist_array();

//Get these members from CC_TPtrDlist
/*
   size_t entries() const ;
   void clearAndDestroy();
   void prepend(T*);
   void append(T*);
   T* first() ;
*/

   T* operator()(size_t i) const { return at(i); };
   T* operator[](size_t i) const { return at(i); };
};

#ifdef EXPAND_TEMPLATES
#include "cc_povec.C"
#endif

#endif


