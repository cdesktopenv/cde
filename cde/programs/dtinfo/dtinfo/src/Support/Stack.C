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
// $XConsortium: Stack.cc /main/3 1996/06/11 16:35:31 cde-hal $
template <class T> Stack<T>::Stack () {
    fItems = 0;
    fNumItems = 0;
    fStackSize = 0;
}

template <class T> Stack<T>::~Stack () {
    delete fItems;
}

template <class T>
void
Stack<T>::push (const T newItem) {
    if (fNumItems == fStackSize) {
	T *newList = new T[fStackSize += 16];
	for (int i = 0; i < fNumItems; i ++)
	    newList[i] = fItems[i];
	delete fItems;
	fItems = newList;
    }
    fItems[fNumItems ++] = newItem;
}

template <class T>
T &
Stack<T>::pop () {
    if (fNumItems <= 0)
	throw (Exception());
    return fItems[--fNumItems];
}

template <class T>
T &
Stack<T>::top () const {
    if (fNumItems <= 0)
	throw (Exception());
    return fItems[fNumItems - 1];
}
