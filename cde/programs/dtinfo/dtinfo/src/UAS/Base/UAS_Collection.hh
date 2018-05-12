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
// $XConsortium: UAS_Collection.hh /main/4 1996/07/10 09:40:51 rcs $
#ifndef _UAS_Collection_hh_
#define _UAS_Collection_hh_

# include "UAS_Common.hh"

class UAS_Collection: public UAS_Common {
    //
    //  Constructors/destructor
    //
    public:
	UAS_Collection (UAS_Pointer<UAS_Common> theRoot);

    //
    //  Methods.
    //
    public:
	UAS_RetrievalStatus retrieve(void *client_data = NULL);
	virtual UAS_Pointer<UAS_Common> root ();
	STATIC_SENDER_HH(UAS_CollectionRetrievedMsg);

    protected:
	UAS_Pointer<UAS_Common> fRoot;
};

#endif
