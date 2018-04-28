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
// $XConsortium: UAS_Iterator.cc /main/3 1996/06/11 16:37:52 cde-hal $
template <class T>
UAS_Iterator<T>::UAS_Iterator (const UAS_List<T> &l):
	fList(l), fCurItem(0) {
}

template <class T>
unsigned int
UAS_Iterator<T>::length() const {
    return fList.length();
}

template <class T>
void
UAS_Iterator<T>::reset() {
    fCurItem = 0;
}

template <class T>
void
UAS_Iterator<T>::next() {
    fCurItem ++;
}

template <class T>
int
UAS_Iterator<T>::eol() const {
    return fCurItem < fList.length() ? 0 : 1;
}

template <class T>
UAS_Pointer<T>
UAS_Iterator<T>::item() const {
    if (fCurItem < fList.length())
	return fList[i];
    return UAS_Pointer((T *) 0);
}
