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
// $XConsortium: MMDB_Section.hh /main/9 1996/10/26 18:17:00 cde-hal $
# ifndef _MMDB_Section_hh_
# define _MMDB_Section_hh_

# include "UAS_Common.hh"
# include "MMDB_Common.hh"
# include "UAS_Msgs.hh"

class MMDB_Section: public UAS_Common, public MMDB_Common {
    public:
	MMDB_Section (MMDB &, const UAS_String &);
	MMDB_Section (MMDB &, info_base *, const oid_t&, UAS_String tt);
	~MMDB_Section ();

    public:
	UAS_String title (UAS_TitleType tt = UAS_LONG_TITLE);
	UAS_String tab_title ();

	UAS_String locator ();
	UAS_String id ();
	UAS_String lid ();
	UAS_String bid ();
	UAS_String locale();
	UAS_String content_type ();
	UAS_ObjectType type ();

	UAS_String& precise_id();
	UAS_String& label();

	UAS_String data ();
	unsigned int data_length ();

	UAS_List<UAS_Common> book_tab_list ();
	UAS_List<UAS_StyleSheet> style_sheet_list ();
	UAS_Pointer<UAS_SearchEngine> search_engine ();

	UAS_RetrievalStatus retrieve (void *client_data = 0);

	UAS_Pointer<UAS_Common> next ();
	UAS_Pointer<UAS_Common> previous ();
	UAS_Pointer<UAS_Common> parent ();
	UAS_List<UAS_Common> children ();
	UAS_String implementation_type() {
	    return UAS_String ("mmdb", -1, UAS_NOT_OWNER);
	}

    protected:
	int isbook ();

    protected:
	node_smart_ptr fNode;
	UAS_String fTabTitle;

	UAS_String f_precise_id;
	UAS_String f_label;
};

# endif
