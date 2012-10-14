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
