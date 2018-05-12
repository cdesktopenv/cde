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
// $XConsortium: MMDB_BookCase.hh /main/5 1996/09/04 01:35:51 cde-hal $
# ifndef _MMDB_BookCase_hh_
# define _MMDB_BookCase_hh_

# include "UAS_Common.hh"
# include "UAS_Collection.hh"
# include "UAS_Exceptions.hh"
# include "MMDB_Common.hh"

class MMDB_BookCase: public UAS_Common, public MMDB_Common {
    public:
	MMDB_BookCase (MMDB &, info_base *);
	MMDB_BookCase (MMDB &, const UAS_String &);

    public:
	UAS_Pointer<UAS_Common> parent ();
	UAS_List<UAS_Common> children ();
	UAS_String title (UAS_TitleType tt = UAS_LONG_TITLE);
	UAS_String locator ();
	UAS_String id ();
	UAS_String lid ();
	UAS_String bid ();
	UAS_String locale();
	UAS_ObjectType type ();
	UAS_Pointer<UAS_SearchEngine> search_engine();
	UAS_String implementation_type() {
	    return UAS_String ("mmdb", -1, UAS_NOT_OWNER);
	}
};

# endif
