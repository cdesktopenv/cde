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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: cc_povec.h /main/5 1996/08/21 15:49:07 drk $ */

#ifndef _cc_dlist_array_h
#define _cc_dlist_array_h

#include "dti_cc/CC_Dlist.h"
#include "dti_cc/cc_exceptions.h"

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

   T*  at(size_t pos) const  /* throw boundaryException
			      * if list size is smaller than pos
			      */
   {
     // Hack to get it passed to iter
     CC_TPtrSlistIterator<T> iter( *(CC_TPtrSlist<T> *)this );
     for ( size_t i = 0; i <=pos; i++ ) {
       if ( !(++iter) ) {
         throw(CASTCCBEXCEPT ccBoundaryException(0,0,i));
       }
     }

     return( iter.key() );
   }

   T* operator()(size_t i) const { return at(i); };
   T* operator[](size_t i) const { return at(i); };
};

#ifdef EXPAND_TEMPLATES
#include "cc_povec.C"
#endif

#endif


