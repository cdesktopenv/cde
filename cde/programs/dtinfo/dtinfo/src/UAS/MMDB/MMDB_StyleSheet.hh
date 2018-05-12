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
// $XConsortium: MMDB_StyleSheet.hh /main/4 1996/09/04 01:36:38 cde-hal $
#ifndef _MMDB_StyleSheet_hh_
#define _MMDB_StyleSheet_hh_

# include "UAS_StyleSheet.hh"
# include "UAS_Collection.hh"
# include "UAS_Exceptions.hh"
# include "MMDB_Common.hh"

class MMDB_StyleSheet: public UAS_StyleSheet, public MMDB_Common {
    public:
	MMDB_StyleSheet (MMDB &, info_base *, const oid_t&, StyleSheetType);
	~MMDB_StyleSheet ();

    public:
	unsigned int data_length ();
	UAS_String data ();
	int operator == (UAS_Common &);
	UAS_String implementation_type () {
	    return UAS_String ("mmdb", -1, UAS_NOT_OWNER);
	}
	UAS_String locale();

    protected:
	stylesheet_smart_ptr fStyleSheet;
	oid_t		     fSSID;
};

#endif
