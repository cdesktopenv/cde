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
// $XConsortium: UAS_PtrList.cc /main/5 1996/08/06 09:23:58 rcs $

#include "UAS_Exceptions.hh"
#include "Managers/CatMgr.hh"
#include "Registration.hh"

template <class T>
UAS_PtrList<T>::UAS_PtrList() {
    fItems = 0;
    fNumItems = 0;
    fListSize = 0;
}

template <class T>
UAS_PtrList<T>::UAS_PtrList (const UAS_PtrList<T> &l) {
    fItems = 0;
    fNumItems = l.fNumItems;
    fListSize = l.fListSize;
    if (fListSize > 0) {
	fItems = new Tptr[fListSize];
	for (int i = 0; i < fNumItems; i ++)
	    fItems[i] = l.fItems[i];
    }
}

template <class T>
UAS_PtrList<T>::~UAS_PtrList () {
    delete [] fItems;
}

template <class T>
UAS_PtrList<T> &
UAS_PtrList<T>::operator = (const UAS_PtrList<T> &l) {
    if (this == &l)
	return *this;
    delete [] fItems;
    fItems = 0;
    fListSize = l.fListSize;
    fNumItems = l.fNumItems;
    if (fListSize > 0) {
	fItems = new Tptr[fListSize];
	for (int i = 0; i < fNumItems; i ++)
	    fItems[i] = l.fItems[i];
    }
    return *this;
}

template <class T>
T *
UAS_PtrList<T>::operator [] (int i) const {
    return item(i);
}

template <class T>
T *
UAS_PtrList<T>::item (int i) const {
    if (i < 0 || i >= fNumItems)
	throw (UAS_Exception (
	    (char*)UAS_String(CATGETS(Set_UAS_Base, 4, "File a Bug"))));
    return fItems[i];
}

template <class T>
int
UAS_PtrList<T>::numItems () const {
    return fNumItems;
}

template <class T>
void
UAS_PtrList<T>::append (T *newItem) {
    if (fNumItems == fListSize) {
	T **newList = new Tptr[fListSize += 4];
	for (int i = 0; i < fNumItems; i ++)
	    newList[i] = fItems[i];
	delete [] fItems;
	fItems = newList;
    }
    fItems[fNumItems ++] = newItem;
}

template <class T>
void
UAS_PtrList<T>::remove (T *oldItem) {
    int i;
    for (i = 0; i < fNumItems; i ++)
	if (fItems[i] == oldItem)
	    break;
    if (i >= fNumItems)
	return;
    for (i ++; i < fNumItems; i ++)
	fItems[i-1] = fItems[i];
    fNumItems --;
}

template <class T>
void
UAS_PtrList<T>::clear () {
    delete [] fItems;
    fItems = 0;
    fNumItems = 0;
    fListSize = 0;
}
