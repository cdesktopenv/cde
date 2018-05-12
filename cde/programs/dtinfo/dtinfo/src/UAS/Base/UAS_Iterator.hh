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
// $XConsortium: UAS_Iterator.hh /main/3 1996/06/11 16:37:57 cde-hal $
#ifndef _UAS_Iterator_h_
#define _UAS_Iterator_h_


template <class T> class UAS_Iterator: public UAS_Base {
    public:
	UAS_Iterator (const UAS_List<T> &);
    
	unsigned int length () const;
	void reset ();
	void next ();
	int eol () const;
	UAS_Pointer<T> item () const;
    private:
	int fCurItem;
	const UAS_List<T> &fList;
};

#endif
