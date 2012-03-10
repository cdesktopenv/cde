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
