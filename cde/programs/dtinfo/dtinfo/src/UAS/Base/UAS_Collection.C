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
// $XConsortium: UAS_Collection.cc /main/4 1996/07/10 09:40:43 rcs $
# include "UAS_Exceptions.hh"
# include "UAS_Collection.hh"

# define CLASS UAS_Collection
STATIC_SENDER_CC(UAS_CollectionRetrievedMsg)

UAS_Collection::UAS_Collection (UAS_Pointer<UAS_Common> theRoot):fRoot(theRoot){
}

UAS_Pointer<UAS_Common>
UAS_Collection::root () {
    return fRoot;
}

UAS_RetrievalStatus
UAS_Collection::retrieve (void *client_data) {
    UAS_CollectionRetrievedMsg msg (this);
    send_message (msg, client_data);
    return UAS_RETRIEVED;
}
