// $XConsortium: TemplatesAutoNumber.C /main/9 1996/10/08 19:23:56 cde-hal $


#include "HardCopy/autoNumber.h"

#ifdef CDE_NEXT

//#include <StyleSheet/cde_next.h>
#include "dti_cc/CC_String.h"
#include "dti_cc/cc_hdict.h"
#include "dti_cc/CC_Dlist.h"
#include "dti_cc/CC_Stack.C"

#endif /* end of CDE_NEXT */

#if defined(sun) || defined(hpux) || defined(__uxp__)  || defined(USL)

#ifndef CDE_NEXT /* { */

typedef hashTable<CC_String,autoNumberListT> _f10_;
typedef hashTable<CC_String,autoNumber> _f11_;

typedef CC_TPtrSlistDictionary<CC_String,autoNumberListT> _f12_;
typedef CC_TPtrSlistDictionary<CC_String,autoNumber> _f13_;
 
typedef hashTableIterator<CC_String,autoNumber> _f14_;
typedef hashTableIterator<CC_String, autoNumberListT> _f15;

#else

typedef CC_TPtrSlist<autoNumber> _CC_TPtrSlist_autoNumber_;

typedef hashTable<CC_String, autoNumberListT> _hashTable_CC_String_autoNumberListT_;
typedef hashTable<CC_String,autoNumber> _hashTable_CC_String_autoNumber_;

typedef hashTableIterator<CC_String,autoNumber> _hashTableIterator_CC_String_autoNumber_;

typedef hashTableIterator<CC_String, autoNumberListT> _hashTableIterator_CC_String_autoNumberListT_;

typedef Stack<int> _Stack_int_;

#endif /* end of CDE_NEXT } */

#endif

#ifdef _IBMR2

#pragma define ( hashTable<CC_String,autoNumberListT> )
#pragma define ( hashTable<CC_String,autoNumber> )

#ifndef CDE_NEXT
#pragma define ( CC_TPtrSlistDictionary<CC_String,autoNumberListT> )
#pragma define ( CC_TPtrSlistDictionary<CC_String,autoNumber> )
#endif

#pragma define ( CC_TPtrSlist<autoNumber> )
 
#pragma define ( hashTableIterator<CC_String,autoNumber> )
#pragma define ( hashTableIterator<CC_String,autoNumberListT> )

#pragma define ( Stack<int> )

#endif

#ifdef __osf__

#pragma define_template hashTable<CC_String,autoNumberListT>
#pragma define_template hashTable<CC_String,autoNumber>

#ifndef CDE_NEXT
#pragma define_template CC_TPtrSlistDictionary<CC_String,autoNumberListT>
#pragma define_template CC_TPtrSlistDictionary<CC_String,autoNumber>
#endif

#pragma define_template CC_TPtrSlist<autoNumber>
 
#pragma define_template hashTableIterator<CC_String,autoNumber>
#pragma define_template hashTableIterator<CC_String,autoNumberListT> 

#pragma define_template Stack<int>

// still unresolved

#pragma define_template CC_TPtrSlist<kv_pair<CC_String, autoNumberListT> >
#pragma define_template kv_pair<CC_String, autoNumber>
#pragma define_template kv_pair<CC_String, autoNumberListT>
#pragma define_template CC_TPtrSlist<kv_pair<CC_String, autoNumber> >
#pragma define_template pointer_vector<CC_TPtrSlist<kv_pair<CC_String, autoNumber> > >
#pragma define_template pointer_vector<CC_TPtrSlist<kv_pair<CC_String, autoNumberListT> > >

//  The following are initializations for a static member 
//  function that the DEC compiler wouldn't let me initialize
//  in the template definition

CC_Boolean kv_pair<CC_String, autoNumberListT>::f_needRemove = FALSE; 
CC_Boolean kv_pair<CC_String, autoNumber>::f_needRemove = FALSE; 

#endif

#ifdef USL

#pragma instantiate hashTable<CC_String,autoNumberListT>
#pragma instantiate hashTable<CC_String,autoNumber>

#ifndef CDE_NEXT
#pragma instantiate CC_TPtrSlistDictionary<CC_String,autoNumberListT>
#pragma instantiate CC_TPtrSlistDictionary<CC_String,autoNumber>
#endif

#pragma instantiate CC_TPtrSlist<autoNumber>
 
#pragma instantiate hashTableIterator<CC_String,autoNumber>
#pragma instantiate hashTableIterator<CC_String,autoNumberListT> 

#pragma instantiate Stack<int>

// still unresolved

#pragma instantiate CC_TPtrSlist<kv_pair<CC_String, autoNumberListT> >
#pragma instantiate kv_pair<CC_String, autoNumber>
#pragma instantiate kv_pair<CC_String, autoNumberListT>
#pragma instantiate CC_TPtrSlist<kv_pair<CC_String, autoNumber> >
#pragma instantiate pointer_vector<CC_TPtrSlist<kv_pair<CC_String, autoNumber> > >
#pragma instantiate pointer_vector<CC_TPtrSlist<kv_pair<CC_String, autoNumberListT> > >

#endif
