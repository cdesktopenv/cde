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
// $XConsortium: Stack.hh /main/4 1996/08/21 15:43:59 drk $
#ifndef _Stack_hh
#define _Stack_hh

# include "Exceptions.hh"

template <class T> class Stack: public Destructable {
    public:
	Stack ();
	~Stack ();

    public:
	T&	pop ();       // reference type is returned for speed
	void	push (const T);
	T&	top () const;
        int     entries() const { 
	  return(fNumItems);  //ie no. of elements in the stack
	}
        int     empty() const {
          return(!fNumItems);
        }

    private:
	T *	fItems;
	int	fNumItems;
	int	fStackSize;
};

#ifdef EXPAND_TEMPLATES
#include "Stack.C"
#endif

#endif
