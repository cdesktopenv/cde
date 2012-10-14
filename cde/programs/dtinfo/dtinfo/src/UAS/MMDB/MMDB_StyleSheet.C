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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
// $XConsortium: MMDB_StyleSheet.C /main/4 1996/09/04 01:36:27 cde-hal $
# include <sstream>
using namespace std;
# include "MMDB_StyleSheet.hh"

MMDB_StyleSheet::MMDB_StyleSheet (MMDB &theMMDB,
				  info_base *ib,
				  const oid_t &ssid,
				  StyleSheetType sstype):
			    MMDB_Common (theMMDB, ib),
			    UAS_StyleSheet (sstype),
			    fSSID (ssid),
			    fStyleSheet (ib, ssid) {
}

MMDB_StyleSheet::~MMDB_StyleSheet () {
}

unsigned int
MMDB_StyleSheet::data_length () {
    if (fSSType == SS_HARDCOPY) {
	return fStyleSheet.hardcopy_data_size ();
    } else {
	return fStyleSheet.online_data_size ();
    }
}

UAS_String
MMDB_StyleSheet::data () {
    if (fSSType == SS_HARDCOPY) {
	return UAS_String (fStyleSheet.hardcopy_data(),
			       fStyleSheet.hardcopy_data_size(),
			       UAS_NOT_OWNER);
    } else {
	return UAS_String (fStyleSheet.online_data(),
			       fStyleSheet.online_data_size(),
			       UAS_NOT_OWNER);
    }
}

int
MMDB_StyleSheet::operator == (UAS_Common &obj) {
    //  First make sure obj is an mmdb stylesheet...
    if (obj.implementation_type() != "mmdb" || obj.type() != UAS_STYLE_SHEET)
	return 0;

    //  Because of the above if stmt we know the cast is safe.
    MMDB_StyleSheet &ss = (MMDB_StyleSheet &) obj;
    return infobase() == ss.infobase() && fSSID.eq (ss.fSSID);
}

UAS_String
MMDB_StyleSheet::locale()
{
    return UAS_String(infobase()->get_info_base_locale());
}

