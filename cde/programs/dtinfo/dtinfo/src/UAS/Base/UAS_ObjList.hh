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
// $XConsortium: UAS_ObjList.hh /main/4 1996/08/21 15:44:30 drk $
#ifndef _UAS_ObjList_hh_
#define _UAS_ObjList_hh_

template <class T> class UAS_ObjList {
    public:
	UAS_ObjList ();
	UAS_ObjList (const UAS_ObjList<T> &);
	~UAS_ObjList ();

    public:
	void append (const T &);
	void remove (const T &);
	T &item(int) const;
	int numItems () const;
	UAS_ObjList<T> &operator = (const UAS_ObjList<T> &);
	//  zero-origined
	T & operator [] (int) const;

        void clear();

    private:
	T *fItems;
	int fNumItems;
	int fListSize;
};



#ifdef EXPAND_TEMPLATES
#include "UAS_ObjList.C"
#endif
#endif
