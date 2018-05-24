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
// $XConsortium: TemplatesAutoNumber.C /main/9 1996/10/08 19:23:56 cde-hal $


#include "HardCopy/autoNumber.h"

#ifdef CDE_NEXT

//#include <StyleSheet/cde_next.h>
#include "dti_cc/CC_String.h"
#include "dti_cc/cc_hdict.h"
#include "dti_cc/CC_Dlist.h"
#include "dti_cc/CC_Stack.C"

#endif /* end of CDE_NEXT */

#if defined(sun) || defined(hpux)

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
