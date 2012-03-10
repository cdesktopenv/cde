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
