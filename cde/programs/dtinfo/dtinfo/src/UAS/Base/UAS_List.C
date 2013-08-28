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
// $XConsortium: UAS_List.cc /main/3 1996/06/11 16:38:13 cde-hal $
static const int UAS_LIST_INCREMENT = 16;

template <class T>
UAS_List<T>::UAS_List() {
    fItems = 0;
    fNumItems = 0;
    fListSize = 0;
}

template <class T>
UAS_List<T>::UAS_List(const UAS_List<T> &l) {
    fItems = 0;
    fNumItems = l.fNumItems;
    fListSize = l.fListSize;
    if (fListSize > 0) {
	fItems = new UAS_Pointer<T>[fListSize];
	for (int i = 0; i < fNumItems; i ++) {
	    fItems[i] = l.fItems[i];
	}
    }
}

template <class T>
UAS_List<T>::UAS_List(unsigned int size) {
    fItems = 0;
    fListSize = size;
    fNumItems = size;
    if (fNumItems > 0) {
	fItems = new UAS_Pointer<T>[size];
    }
}

template <class T>
UAS_List<T>::~UAS_List() {
    delete [] fItems;
}

template <class T>
int
UAS_List<T>::operator == (const UAS_List<T>& l) const {
    if (fNumItems != l.fNumItems)
	return 0;
    for (int i = 0; i < fNumItems; i ++)
	if (fItems[i] != l.fItems[i])
	    return 0;
    return 1;
}

template <class T>
int
UAS_List<T>::operator != (const UAS_List<T>& l) const {
    return ! (*this == l);
}

template <class T>
unsigned int
UAS_List<T>::length () const {
    return fNumItems;
}

template <class T>
UAS_Pointer<T> &
UAS_List<T>::item(unsigned int index) {
    if (index >= (unsigned int)fNumItems) {
	abort ();
    }
    return fItems[index];
}

template <class T>
UAS_Pointer<T>
UAS_List<T>::item(unsigned int index) const {
    if (index >= fNumItems) {
	abort ();
    }
    return fItems[index];
}

template <class T>
UAS_Pointer<T>
UAS_List<T>::operator [] (unsigned int index) const {
    return item(index);
}

template <class T>
UAS_Pointer<T> &
UAS_List<T>::operator [] (unsigned int index) {
    return item(index);
}

template <class T>
int
UAS_List<T>::insert_item(const UAS_Pointer<T>& theItem, int index) {
    //
    //  Flag invalid index.
    //
    if (index < -1 || index > fNumItems)
	return 0;

    //
    //  -1 means append. Update index accordingly.
    //
    if (index == -1)
	index = fNumItems;

    //
    //  Get more room if necessary. Essentially just a realloc.
    //
    if (fNumItems == fListSize) {
	UAS_Pointer<T> *newList = new UAS_Pointer<T>[fListSize += UAS_LIST_INCREMENT];
	for (int i = 0; i < fNumItems; i ++)
	    newList[i] = fItems[i];
	delete [] fItems;
	fItems = newList;
    }

    //
    //  Make the hole for the new item.
    //
    for (int i = fNumItems; i > index; i --)
	fItems[i] = fItems[i - 1];

    //
    //  Finally, we can insert the item in the hole
    //  and bump up the size of the list.
    //
    fItems[index] = theItem;
    fNumItems ++;
    return 1;
}

#if 0
template <class T>
int
UAS_List<T>::insert_item (const T& theItem, int index) {
    return insert_item (UAS_Pointer<T>(new T(theItem)), index);
}
#endif

template <class T>
int
UAS_List<T>::set_item (const UAS_Pointer<T>& theItem, unsigned int index) {
    if (index >= (unsigned int)fNumItems)
	return 0;
    fItems[index] = theItem;
    return 1;
}

#if 0
template <class T>
int
UAS_List<T>::set_item (const T& theItem, unsigned int index) {
    return set_item (UAS_Pointer<T>(new T(theItem)), index);
}
#endif

template <class T>
int
UAS_List<T>::remove_item (unsigned int index) {
    if (index >= (unsigned int)fNumItems)
	return 0;
    for (int i = index + 1; i < fNumItems; i ++)
	fItems[i - 1] = fItems[i];
    fItems[--fNumItems] = (T *) 0;
    return 1;
}

template <class T>
int
UAS_List<T>::remove_item (const UAS_Pointer<T>& theItem) {
    for (unsigned int i = 0; i < (unsigned int)fNumItems; i ++) {
	if (fItems[i] == theItem) {
	    return remove_item (i);
	}
    }
    return 0;
}

template <class T>
UAS_List<T>&
UAS_List<T>::operator = (const UAS_List<T>& l) {
    if (this != &l) {
	delete [] fItems;
	fItems = 0;
	fListSize = l.fListSize;
	fNumItems = l.fNumItems;
	if (fListSize) {
	    fItems = new UAS_Pointer<T>[fListSize];
	    for (int i = 0; i < fNumItems; i ++) {
		fItems[i] = l.fItems[i];
	    }
	}
    }
    return *this;
}
