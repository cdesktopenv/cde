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
