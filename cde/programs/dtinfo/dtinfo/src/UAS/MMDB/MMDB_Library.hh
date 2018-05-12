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
// $XConsortium: MMDB_Library.hh /main/4 1996/05/29 12:33:44 rcs $
#ifndef _MMDB_Library_hh_
#define _MMDB_Library_hh_

# include "UAS_Common.hh"
# include "MMDB_Common.hh"
# include "UAS_SearchEngine.hh"


class MMDB_Library: public UAS_Common, public MMDB_Common {
    public:
	MMDB_Library (MMDB &);
	~MMDB_Library();

    public:
	UAS_List<UAS_Common> children ();
	UAS_String title (UAS_TitleType tt = UAS_LONG_TITLE);
	UAS_String locator ();
	UAS_String id ();
	UAS_String lid ();
	UAS_String bid ();
	UAS_String name ();
	UAS_ObjectType type ();
	UAS_Pointer<UAS_SearchEngine> search_engine ();
	UAS_String implementation_type() {
	    return UAS_String("mmdb", -1, UAS_NOT_OWNER);
	}

    private:
	static UAS_Pointer<UAS_SearchEngine> fSearchEngine;

	static int f_mmdbs;
        UAS_String f_name;
        
};

#endif
