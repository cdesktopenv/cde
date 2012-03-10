// $XConsortium: MMDB_StyleSheet.C /main/4 1996/09/04 01:36:27 cde-hal $
# include <stream.h>
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

