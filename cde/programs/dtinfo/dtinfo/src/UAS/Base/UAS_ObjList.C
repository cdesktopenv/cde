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
// $XConsortium: UAS_ObjList.cc /main/4 1996/06/11 18:32:41 cde-hal $

#include "Managers/CatMgr.hh"
#include "Registration.hh"

template <class T>
UAS_ObjList<T>::UAS_ObjList() {
    fItems = 0;
    fNumItems = 0;
    fListSize = 0;
}

template <class T>
UAS_ObjList<T>::UAS_ObjList (const UAS_ObjList<T> &l) {
    fItems = 0;
    fNumItems = l.fNumItems;
    fListSize = l.fListSize;
    if (fListSize > 0) {
	fItems = new T[fListSize];
	for (int i = 0; i < fNumItems; i ++)
	    fItems[i] = l.fItems[i];
    }
}

template <class T>
UAS_ObjList<T>::~UAS_ObjList () {
    delete [] fItems;
}

template <class T>
UAS_ObjList<T> &
UAS_ObjList<T>::operator = (const UAS_ObjList<T> &l) {
    if (this == &l)
	return *this;
    delete [] fItems;
    fItems = 0;
    fListSize = l.fListSize;
    fNumItems = l.fNumItems;
    if (fListSize > 0) {
	fItems = new T[fListSize];
	for (int i = 0; i < fNumItems; i ++)
	    fItems[i] = l.fItems[i];
    }
    return *this;
}

template <class T>
T &
UAS_ObjList<T>::operator [] (int i) const {
    return item(i);
}

template <class T>
T &
UAS_ObjList<T>::item (int i) const {
    if (i < 0 || i >= fNumItems)
	throw (UAS_Exception (
		(char*)UAS_String(CATGETS(Set_UAS_Base, 3, "File a Bug"))));
    return fItems[i];
}

template <class T>
int
UAS_ObjList<T>::numItems () const {
    return fNumItems;
}

template <class T>
void
UAS_ObjList<T>::append (const T &newItem) {
    if (fNumItems == fListSize) {
	T *newList = new T[fListSize += 4];
	for (int i = 0; i < fNumItems; i ++)
	    newList[i] = fItems[i];
	delete [] fItems;
	fItems = newList;
    }
    fItems[fNumItems ++] = newItem;
}

template <class T>
void
UAS_ObjList<T>::remove (const T &oldItem) {
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
UAS_ObjList<T>::clear () {
    delete [] fItems;
    fItems = 0;
    fNumItems = 0;
    fListSize = 0;
}
