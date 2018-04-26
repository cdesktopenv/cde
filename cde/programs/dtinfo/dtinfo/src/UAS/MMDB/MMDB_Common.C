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
// $XConsortium: MMDB_Common.cc /main/3 1996/06/11 16:43:28 cde-hal $
# if 0
# include "UAS_Base.hh"
# include "MMDB_Common.hh"
# include "UAS_Pointer.hh"
# include "UAS_List.hh"
# include "UAS_BookTab.hh"
# include "MMDB_BookTab.hh"

UAS_Pointer<UAS_List<UAS_BookTab> > 
MMDB_Common:: tabList (MMDB &theMMDB, info_base *ib, const oid_t &id) {
    doc_smart_ptr book (ib, id);
    short_list_handler *l = book.tab_list ();
    UAS_List<UAS_BookTab> *theList = new UAS_List<UAS_BookTab>();
    for (int i = 1; i <= (*l)->count(); i ++) {
	pstring_handler *p = (pstring_handler *)((*l)->get_component(i));
	theList->insert_item(new MMDB_BookTab(theMMDB, ib, (*p)->get()));
    }
    return theList;
}
#endif
