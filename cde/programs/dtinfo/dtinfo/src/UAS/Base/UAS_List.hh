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
// $XConsortium: UAS_List.hh /main/4 1996/08/21 15:44:25 drk $
#ifndef _UAS_List_h_
#define _UAS_List_h_

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
//  Template class for handling lists in the UAS.
//  Requirements for <class T> are:
//      1.  It must be usable by class UAS_Pointer. See UAS_Pointer.hh for
//          details. A specific requirement here is that operator == be
//          defined for the class.
//      2.  It must define a copy constructor if the const T& versions of
//          insert_item() and set_item() are used.
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template <class T> class UAS_List: public UAS_Base {
    public:
	////////////
	//
	//  Constructors and destructor.
	//
	////////////

	UAS_List();			// default constructor
	UAS_List(unsigned int size);	// Create a list initially of size size
	UAS_List(const UAS_List<T> &);	// copy constructor
	~UAS_List();			// UAS_Base destructor is virtual

	////////////
	//
	//  Public Member functions (const)
	//
	////////////

	//
	//  Return the number of items in the list.
	//
	unsigned int length() const;

	//
	// Return the index'th item, zero based. Returns
	// a "NULL Pointer" if the index is out of range.
	// We'd like to throw an exception here, but we
	// can't count on exceptions being around.
	// Both functions behave identically. The overloaded
	// operator[] is provided for convenience.
	//
	UAS_Pointer<T> item(unsigned int index) const;
	UAS_Pointer<T>& item(unsigned int index);
	UAS_Pointer<T> operator [](unsigned int index) const;
	UAS_Pointer<T>& operator [](unsigned int index);

	//
	//  Ideally, these functions should be friends. However,
	//  there is no need here since it doesn't really make
	//  sense to provide a user-defined conversion before
	//  comparison.
	//
	int operator == (const UAS_List<T>&) const;
	int operator != (const UAS_List<T>&) const;

	//////////
	//
	//  Public member functions (non const)
	//
	//////////

	UAS_List<T>& operator = (const UAS_List<T>&);

	//
	//  These functions expand the list by one and
	//  insert the specified item.
	//  The const T& is copied before being put
	//  into the list (via T's copy constructor)
	//
	int insert_item(const UAS_Pointer<T>&, int index = -1);
#if 0
	//
	//  Took this out because if T is an abstract base class,
	//  this won't work due to the fact that it tries to
	//  use T's copy constructor. SWM
	//
	int insert_item(const T&, int index = -1);
#endif

	//
	//  These replace an item in the list. They return UAS_FALSE
	//  iff index is out of range. Again, the const T& version
	//  makes a copy first. NOTE: we don't have a non-const operator[] 
	//  here because there is no way (without exceptions) to convey
	//  failure.
	//
	int set_item(const UAS_Pointer<T>&, unsigned int index);
#if 0
	int set_item(const T&, unsigned int index);
#endif

	//
	//  These remove the specified item and collapse the list.
	//  to fill the hole created.
	//
	int remove_item(unsigned int index);

	//
	//  This removes the first occurrence of the item.
	//
	int remove_item(const UAS_Pointer<T>&);

    private:
	//////////
	//
	//  fItems holds the items. fNumItems holds the
	//  current number of items. fListSize tells us
	//  how many items fItems can hold. fListSize >= fNumItems.
	//
	//////////
	UAS_Pointer<T> *fItems;
	int fNumItems;
	int fListSize;
};

#ifdef EXPAND_TEMPLATES
#include "UAS_List.C"
#endif

#endif
